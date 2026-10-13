#!/usr/bin/env bash
# Dump the Python stack of the running GUI test (or a given pattern).
. ~/mglenv/bin/activate
pat=${1:-tests/gui_smoke.py}
pid=$(pgrep -f "python.*$pat" | head -n 1)
if [ -z "$pid" ]; then echo "no process matching $pat"; exit 0; fi
echo "pid $pid"; ps -o etime= -p "$pid"
py-spy dump --pid "$pid" 2>&1 | head -n "${2:-45}"
