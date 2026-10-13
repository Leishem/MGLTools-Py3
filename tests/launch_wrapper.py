"""Run a launcher (adt/pmv) with exceptions also written to a log file, so
errors survive the IDLE shell taking over sys.stderr.
Usage: python tests/launch_wrapper.py adt|pmv [launcher args...]"""
import os
import sys
import tkinter
import traceback

LOG = open(os.environ.get("MGL_ERRLOG", "launch_errors.log"), "w", buffering=1)


def log_exc(*exc):
    LOG.write("".join(traceback.format_exception(*exc)) + "\n-----\n")


_orig_hook = sys.excepthook
sys.excepthook = lambda *e: (log_exc(*e), _orig_hook(*e))
tkinter.Tk.report_callback_exception = lambda self, *e: log_exc(*e)

which = sys.argv[1]
sys.argv = [which] + sys.argv[2:]
from mgltools import launch  # noqa: E402
try:
    getattr(launch, which)()
except BaseException:
    log_exc(*sys.exc_info())
    raise
