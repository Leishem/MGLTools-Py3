#!/usr/bin/env bash
# Probe which TkGL option combinations the current X server/GL driver accept.
cd "$(dirname "$0")/.."
probe() { echo "== $*"; FORCE_XVFB=${FORCE_XVFB:-1} timeout 60 bash tools/xrun.sh python -W ignore tests/tkgl_probe.py "$@" 2>&1 | grep -E 'GL_VERSION|Error|error' | head -3; }
probe -rgba true -double true -depth true -stencil true -multisample true
probe -rgba true -double true -depth true -stencil true -multisample true -stereo none -alpha true
