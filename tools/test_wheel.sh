#!/usr/bin/env bash
# Install the built wheel into a fresh venv and run the tests against the
# installed package (not src/): catches missing package data / extensions.
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
WHEEL=$(ls ${1:-$HOME/wheeltest}/mgltools-*.whl | head -n 1)
V=$HOME/wheelenv
rm -rf "$V"; python3 -m venv "$V"; . "$V/bin/activate"
pip -q install "$WHEEL" pytest
cd /tmp
python - <<'EOF'
import AutoDockTools, Pmv, DejaVu, MolKit, mslib, opengltk.extent._utillib, bhtree
print("installed at", AutoDockTools.__file__)
EOF
which adt pmv vision
# run the suite with src/ hidden so the installed package is used
mkdir -p /tmp/wt && cp -r "$ROOT/tests" /tmp/wt/ && cp "$ROOT/pytest.ini" /tmp/wt/
mkdir -p /tmp/wt/src/Pmv/doc /tmp/wt/src/AutoDockTools
cp "$ROOT/src/Pmv/doc/hsg1.pdb" "$ROOT/src/Pmv/doc/ind.pdb" /tmp/wt/src/Pmv/doc/
SITE=$(python -c "import AutoDockTools,os;print(os.path.dirname(os.path.dirname(AutoDockTools.__file__)))")
ln -sfn "$SITE/AutoDockTools/Utilities24" /tmp/wt/src/AutoDockTools/Utilities24
cd /tmp/wt && python -m pytest -q 2>&1 | tail -n 5
