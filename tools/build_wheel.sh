#!/usr/bin/env bash
# Build a wheel in an isolated environment (as CI does). Log: build/wheel.log
cd "$(dirname "$0")/.."
. ~/mglenv/bin/activate
mkdir -p build
out=${1:-$HOME/wheeltest}
rm -rf "$out"; mkdir -p "$out"
pip wheel . -w "$out" --no-deps -v > build/wheel.log 2>&1
status=$?
echo "exit=$status"
ls -la "$out"
exit $status
