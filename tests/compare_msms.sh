#!/usr/bin/env bash
# Original MSMS (MGLTools 1.5.6, ~/mgl156) vs the py3 mslib replacement.
# Usage: tests/compare_msms.sh [pdb] [probe] [density]
ROOT=$(cd "$(dirname "$0")/.." && pwd)
PDB=${1:-$ROOT/src/Pmv/doc/hsg1.pdb}
PROBE=${2:-1.5}
DENS=${3:-3.0}
out=$(mktemp -d)
~/mgl156/bin/pythonsh "$ROOT/tests/msms_ref.py" "$PDB" "$out/ref" "$PROBE" "$DENS" || exit 1
. ~/mglenv/bin/activate
python -W ignore "$ROOT/tests/compare_msms.py" "$PDB" "$out/ref" "$PROBE" "$DENS"
