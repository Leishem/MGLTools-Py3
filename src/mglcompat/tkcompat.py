"""Tk 8.4 geometry-manager leniency on Tk 8.6+.

MGLTools widgets often pack themselves in __init__ and the caller then
grids them (or the reverse). Tk 8.4 accepted the first call and let the
second take over; Tk 8.6 raises "cannot use geometry manager X inside M
which already has slaves managed by Y" at the first, conflicting call.

install() wraps pack/grid so that a call rejected for *only* that reason is
skipped -- the layout ends up as under Tk 8.4, where the later call won.
Set MGL_TK_WARN=1 to print each skipped call.
"""
import os
import tkinter

# Tk 8.6 wording, then Tk 9 wording
_CONFLICTS = ("already has slaves managed by",
              "is already managing its content windows")
_installed = False


def _lenient(fn, name):
    def wrapper(self, *args, **kw):
        try:
            return fn(self, *args, **kw)
        except tkinter.TclError as e:
            if not any(c in str(e) for c in _CONFLICTS):
                raise
            if os.environ.get("MGL_TK_WARN"):
                import traceback
                where = traceback.extract_stack(limit=3)[0]
                print(f"[tkcompat] skipped {name} of {self}: {e} "
                      f"({os.path.basename(where.filename)}:{where.lineno})")
    wrapper.__name__ = fn.__name__
    wrapper.__doc__ = fn.__doc__
    return wrapper


def install():
    global _installed
    if _installed:
        return
    _installed = True
    P, G = tkinter.Pack, tkinter.Grid
    P.pack_configure = P.configure = P.config = P.pack = \
        _lenient(P.pack_configure, "pack")
    G.grid_configure = G.configure = G.config = G.grid = \
        _lenient(G.grid_configure, "grid")
    # Widget inherits pack/grid through these mixins; refresh the aliases
    # that tkinter.Widget binds at class creation.
    for cls in (tkinter.Widget,):
        cls.pack = cls.pack_configure = P.pack_configure
        cls.grid = cls.grid_configure = G.grid_configure

    # Python 2 (and Tcl's "after cancel") ignored unknown/None ids; Python
    # 3.12 raises ValueError for after_cancel(None).
    _after_cancel = tkinter.Misc.after_cancel

    def after_cancel(self, id):
        if not id:
            return
        return _after_cancel(self, id)

    tkinter.Misc.after_cancel = after_cancel

    # Python 2's Variable(master) used the default root for any falsy master,
    # so MGLTools writes tkinter.IntVar(0) meaning "value 0, default root".
    # Python 3 only treats None that way.
    _var_init = tkinter.Variable.__init__

    def var_init(self, master=None, value=None, name=None):
        if not master and master is not None and not isinstance(master, tkinter.Misc):
            master = None
        return _var_init(self, master, value, name)

    tkinter.Variable.__init__ = var_init

    # Python 2 named unnamed widgets repr(id(widget)) (".1234.5678"); Python 3
    # uses ".!toplevel.!label". MGLTools passes widgets where Tk expects canvas
    # tags, and Tk 8.6 parses '!' in a tag as the NOT operator. Use numeric
    # names again, from a counter: id() values get reused once a Python widget
    # object dies while its Tk window lives on (DejaVu's throwaway tkgl probes),
    # which would hand a new widget the path of an existing window.
    import itertools
    _ids = itertools.count(1000000)
    _setup = tkinter.BaseWidget._setup

    def setup(self, master, cnf):
        if 'name' not in cnf:
            cnf = dict(cnf)
            cnf['name'] = str(next(_ids))
        return _setup(self, master, cnf)

    tkinter.BaseWidget._setup = setup

    # tk_menuBar was a no-op kept for compatibility in Python 2's tkinter.
    if not hasattr(tkinter.Misc, 'tk_menuBar'):
        tkinter.Misc.tk_menuBar = lambda self, *args: None


install()
