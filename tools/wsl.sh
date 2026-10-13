#!/usr/bin/env bash
# Run a command inside the WSL dev venv from the repo root.
# Usage (from Windows): wsl -d Ubuntu -- bash /mnt/d/Reverse_Engineer/mgltools-py3/tools/wsl.sh <cmd...>
set -e
cd "$(dirname "$0")/.."
. ~/mglenv/bin/activate
exec "$@"
