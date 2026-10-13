#!/usr/bin/env bash
# Install the original MGLTools 1.5.6 (Python 2.5, x86_64 Linux) into
# ~/mgl156 as a reference implementation for output comparisons.
set -e
TARBALL=/mnt/d/Reverse_Engineer/work/linux_dl/mgltools_x86_64Linux2_1.5.6.tar.gz
DEST=~/mgl156
if [ -x "$DEST/bin/pythonsh" ]; then echo "already installed: $DEST"; exit 0; fi
tmp=$(mktemp -d)
tar -xzf "$TARBALL" -C "$tmp"
cd "$tmp"/mgltools_x86_64Linux2_1.5.6
./install.sh -d "$DEST" > "$tmp/install.log" 2>&1 || { tail -20 "$tmp/install.log"; exit 1; }
tail -3 "$tmp/install.log"
ls "$DEST/bin"
