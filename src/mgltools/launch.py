"""Entry points for the adt / pmv / vision commands.

Equivalent to the 1.5.6 scripts AutoDockTools/bin/runAdt.py,
Pmv/bin/runPmv.py and Vision/bin/runVision.py.
"""
import sys


def adt():
    import AutoDockTools
    AutoDockTools.runADT(sys.argv, ownInterpreter=True)


def pmv():
    import Pmv
    Pmv.runPmv(sys.argv, ownInterpreter=True)


def vision():
    import runpy
    import Vision
    import os
    script = os.path.join(os.path.dirname(Vision.__file__), "bin", "runVision.py")
    sys.argv[0] = script
    # Vision's default shell is code.interact on stdin; started from a
    # desktop launcher there is no terminal, so EOF would quit it at once.
    if not (sys.stdin and sys.stdin.isatty()) and not {"-t", "--noTerminal"} & set(sys.argv):
        sys.argv.insert(1, "--noTerminal")
    runpy.run_path(script, run_name="__main__")


if __name__ == "__main__":
    {"adt": adt, "pmv": pmv, "vision": vision}[sys.argv.pop(1)]()
