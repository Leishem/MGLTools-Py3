"""Report versions of the runtime dependencies (and whether they import)."""
import importlib

for mod, attr in [("numpy", "__version__"), ("OpenGL", "__version__"),
                  ("tkinter_gl", "__version__"), ("Pmw", "version"),
                  ("PIL", "__version__"), ("SimPy", "__version__"),
                  ("SimPy.Simulation", None), ("tkinter", "TkVersion")]:
    try:
        m = importlib.import_module(mod)
        v = getattr(m, attr, "") if attr else "ok"
        print(f"{mod:18} {v() if callable(v) else v}")
    except Exception as e:  # noqa: BLE001
        print(f"{mod:18} MISSING ({type(e).__name__}: {e})")
