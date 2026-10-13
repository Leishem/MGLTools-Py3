#!/usr/bin/env bash
# Run a command in the WSL venv with a display: WSLg if present, else Xvfb.
cd "$(dirname "$0")/.."
. ~/mglenv/bin/activate
export LIBGL_ALWAYS_SOFTWARE=${LIBGL_ALWAYS_SOFTWARE:-1}
if [ -z "$DISPLAY" ] && [ -S /tmp/.X11-unix/X0 ]; then export DISPLAY=:0; fi
if [ -n "$DISPLAY" ] && [ -z "$FORCE_XVFB" ]; then
  exec "$@"
else
  exec xvfb-run -a -s "-screen 0 1600x1000x24 +extension GLX" "$@"
fi
