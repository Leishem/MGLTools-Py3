#!/usr/bin/env bash
# Run tests/gui_smoke.py on a private Xvfb display (never on the desktop).
cd "$(dirname "$0")/.."
rm -rf build/gui
PYTHONUNBUFFERED=1 FORCE_XVFB=1 timeout "${TIMEOUT:-300}" bash tools/xrun.sh python -W ignore tests/gui_smoke.py build/gui "$@" 2>&1 \
  | grep -vE '^\s*[\^~]+\s*$' | tail -n "${LINES_OUT:-50}"
