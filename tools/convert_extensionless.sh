#!/usr/bin/env bash
# Convert Python 2 files without a .py extension (resource-file templates and
# bin/ scripts) with the same pipeline used for src/**/*.py.
set -e
cd "$(dirname "$0")/.."
. ~/mglenv/bin/activate
files=(src/Pmv/_pmvrc src/Vision/_visionrc
       src/AutoDockTools/bin/adt src/AutoDockTools/bin/adt3 src/AutoDockTools/bin/adt4
       src/AutoDockTools/bin/runAdt src/Pmv/bin/pmv src/Pmv/bin/runPmv
       src/Vision/bin/runVision src/Vision/bin/vision)
python tools/convert_py.py --stage1-only "${files[@]}"
python -W ignore -m fissix -w -n --no-diffs "${files[@]}" 2>&1 | grep -E "Can't parse|Error" || true
python tools/fix_py3_misc.py "${files[@]}"
for f in "${files[@]}"; do python -c "import sys; compile(open(sys.argv[1],'rb').read(), sys.argv[1], 'exec')" "$f" && echo "ok $f"; done
