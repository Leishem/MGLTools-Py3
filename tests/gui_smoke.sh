#!/usr/bin/env bash
# Run tests/gui_smoke.py on a private Xvfb display (never on the desktop).
# Full log: build/gui/log.txt (kept even if the test hangs and is killed).
cd "$(dirname "$0")/.."
pkill -f "tests/gui_smoke.py" 2>/dev/null; pkill -x Xvfb 2>/dev/null
rm -rf build/gui; mkdir -p build/gui
MGL_TK_WARN=${MGL_TK_WARN:-1} PYTHONUNBUFFERED=1 FORCE_XVFB=1 timeout -k 5 "${TIMEOUT:-300}" \
  bash tools/xrun.sh python -W ignore tests/gui_smoke.py build/gui "$@" \
  > build/gui/log.txt 2>&1 < /dev/null
echo "exit=$?"
pkill -x Xvfb 2>/dev/null
grep -vE '^\s*[\^~]+\s*$' build/gui/log.txt | tail -n "${LINES_OUT:-50}"
