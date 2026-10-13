#!/usr/bin/env bash
# Start the real launcher (adt or pmv) on Xvfb with tests/launch_check.py.
cd "$(dirname "$0")/.."
. ~/mglenv/bin/activate
which=${1:-adt}
mkdir -p build/gui
export MGL_USE_SRC=1 PYTHONPATH=$PWD/src MGL_LAUNCH_OUT=$PWD/build/gui/launch_$which.png
export MGL_ERRLOG=$PWD/build/gui/launch_${which}_errors.log
FORCE_XVFB=1 timeout -k 5 120 bash tools/xrun.sh python -W ignore tests/launch_wrapper.py $which -s ${NOSHELL:+-i} tests/launch_check.py \
  > build/gui/launch_$which.log 2>&1 < /dev/null
echo "exit=$?"; grep -E "LAUNCH_OK|Error|Traceback" build/gui/launch_$which.log | head -n 8
pkill -x Xvfb 2>/dev/null; true
