#!/usr/bin/env bash
# Regenerate tests/data/adt_ref_156/ from the original MGLTools 1.5.6 (~/mgl156).
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
DEST=$ROOT/tests/data/adt_ref_156
U=~/mgl156/MGLToolsPckgs/AutoDockTools/Utilities24
mkdir -p "$DEST"
w=$(mktemp -d)
cp "$ROOT/src/Pmv/doc/hsg1.pdb" "$ROOT/src/Pmv/doc/ind.pdb" "$w/"
cd "$w"
~/mgl156/bin/pythonsh $U/prepare_receptor4.py -r hsg1.pdb -o hsg1.pdbqt > /dev/null
~/mgl156/bin/pythonsh $U/prepare_ligand4.py -l ind.pdb -o ind.pdbqt > /dev/null
~/mgl156/bin/pythonsh $U/prepare_gpf4.py -l ind.pdbqt -r hsg1.pdbqt -o hsg1.gpf > /dev/null
~/mgl156/bin/pythonsh $U/prepare_dpf42.py -l ind.pdbqt -r hsg1.pdbqt -o ind_hsg1.dpf > /dev/null
cp hsg1.pdbqt ind.pdbqt hsg1.gpf ind_hsg1.dpf "$DEST/"
~/mgl156/bin/pythonsh "$ROOT/tests/msms_ref.py" "$ROOT/src/Pmv/doc/hsg1.pdb" "$DEST/msms_hsg1" 1.5 3.0 > /dev/null 2>&1
rm -f "$DEST/msms_hsg1_atomarea.txt"
gzip -f "$DEST/msms_hsg1_verts.txt"
ls -la "$DEST"
