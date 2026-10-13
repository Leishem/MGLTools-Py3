#!/usr/bin/env bash
# Sweep the EDT correction factor and grid step against original MSMS.
ROOT=$(cd "$(dirname "$0")/.." && pwd)
PDB=${PDB:-$ROOT/src/Pmv/doc/hsg1.pdb}
out=$(mktemp -d)
~/mgl156/bin/pythonsh "$ROOT/tests/msms_ref.py" "$PDB" "$out/ref" 1.5 3.0 > /dev/null 2>&1
. ~/mglenv/bin/activate
for h in ${HS:-0.71 0.5 0.35}; do
  for c in ${CS:-0.3 0.4 0.5 0.6}; do
    echo "=== h=$h corr=$c"
    MSLIB_H=$h MSLIB_EDT_CORR=$c python -W ignore "$ROOT/tests/compare_msms.py" "$PDB" "$out/ref" 1.5 3.0 \
      | grep -E "a_ses_area|n_ses_volume|signed offset|max|atoms"
  done
done
