#!/usr/bin/env bash
# Show the tail of a log from the most recent compare_adt.sh run.
# Usage: tools/lastlog.sh [logname] [lines]
d=$(ls -td /tmp/tmp.* 2>/dev/null | head -1)
tail -n "${2:-15}" "$d/new/${1:-prepare_receptor4.py.log}"
