#!/usr/bin/env bash
# Run the AutoDockTools preparation pipeline with the original MGLTools 1.5.6
# (~/mgl156, Python 2.5) and with the Python 3 port, then diff the outputs.
# Usage: tests/compare_adt.sh [receptor.pdb ligand.pdb]
set -u
ROOT=$(cd "$(dirname "$0")/.." && pwd)
REC=${1:-$ROOT/src/Pmv/doc/hsg1.pdb}
LIG=${2:-$ROOT/src/Pmv/doc/ind.pdb}
REF_PY=~/mgl156/bin/pythonsh
REF_U=~/mgl156/MGLToolsPckgs/AutoDockTools/Utilities24
NEW_U=$ROOT/src/AutoDockTools/Utilities24
. ~/mglenv/bin/activate
export PYTHONPATH=$ROOT/src

out=$(mktemp -d)
mkdir -p "$out/ref" "$out/new"
r=$(basename "$REC" .pdb); l=$(basename "$LIG" .pdb)
cp "$REC" "$LIG" "$out/ref/"; cp "$REC" "$LIG" "$out/new/"

run() {  # run <ref|new> <script> args...
  local side=$1 script=$2; shift 2
  if [ "$side" = ref ]; then
    (cd "$out/ref" && $REF_PY "$REF_U/$script" "$@" > "$script.log" 2>&1)
  else
    (cd "$out/new" && python -W ignore "$NEW_U/$script" "$@" > "$script.log" 2>&1)
  fi
  echo "$side $script exit=$?"
}

for side in ref new; do
  run $side prepare_receptor4.py -r "$r.pdb" -o "$r.pdbqt"
  run $side prepare_ligand4.py -l "$l.pdb" -o "$l.pdbqt"
  run $side prepare_gpf4.py -l "$l.pdbqt" -r "$r.pdbqt" -o "$r.gpf"
  run $side prepare_dpf42.py -l "$l.pdbqt" -r "$r.pdbqt" -o "${l}_$r.dpf"
done

status=0
for f in "$r.pdbqt" "$l.pdbqt" "$r.gpf" "${l}_$r.dpf"; do
  if [ ! -s "$out/new/$f" ]; then
    echo "MISSING new/$f"; status=1; missing=1
    continue
  fi
  # REMARK lines carry dates/paths; ignore them
  if diff <(grep -v '^REMARK' "$out/ref/$f") <(grep -v '^REMARK' "$out/new/$f") > "$out/$f.diff"; then
    echo "SAME     $f ($(wc -l < "$out/new/$f") lines)"
  else
    echo "DIFFERS  $f ($(grep -c '^[<>]' "$out/$f.diff") changed lines) -> $out/$f.diff"
    head -12 "$out/$f.diff" | sed 's/^/    /'
    status=1
  fi
done
[ -n "${missing:-}" ] && for g in "$out/new/"*.log; do echo "--- $(basename $g)"; tail -n 4 "$g" | sed 's/^/    /'; done
echo "outputs in $out"
exit $status
