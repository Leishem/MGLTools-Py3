#!/usr/bin/env bash
set -e

ROOT="/mnt/d/Reverse_Engineer/MGLTOOLS"
OUT_DIR="/mnt/d/Reverse_Engineer/MGLTools-1.5.6-Py3-Linux-x86_64-Portable"
TAR_FILE="/mnt/d/Reverse_Engineer/MGLTools-1.5.6-Py3-Linux-x86_64-Portable.tar.gz"

echo "=== Building Portable MGLTools 1.5.6 for Linux (x86_64) ==="

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

# 1. Create a local venv inside the portable directory
python3 -m venv "$OUT_DIR/python-env"

# 2. Install prebuilt wheel and dependencies
WHEEL=$(ls "$ROOT/dist-linux"/mgltools-*.whl | head -1)
echo "Installing wheel: $WHEEL"
"$OUT_DIR/python-env/bin/pip" install --upgrade pip
"$OUT_DIR/python-env/bin/pip" install "$WHEEL"

# 3. Create portable launcher scripts (using relative paths)
cat << 'EOF' > "$OUT_DIR/adt"
#!/usr/bin/env bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PYTHONHOME=""
export VIRTUAL_ENV="$DIR/python-env"
export PATH="$DIR/python-env/bin:$PATH"
exec "$DIR/python-env/bin/python3" -m mgltools.launch adt "$@"
EOF
chmod +x "$OUT_DIR/adt"

cat << 'EOF' > "$OUT_DIR/pmv"
#!/usr/bin/env bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PYTHONHOME=""
export VIRTUAL_ENV="$DIR/python-env"
export PATH="$DIR/python-env/bin:$PATH"
exec "$DIR/python-env/bin/python3" -m mgltools.launch pmv "$@"
EOF
chmod +x "$OUT_DIR/pmv"

cat << 'EOF' > "$OUT_DIR/vision"
#!/usr/bin/env bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PYTHONHOME=""
export VIRTUAL_ENV="$DIR/python-env"
export PATH="$DIR/python-env/bin:$PATH"
exec "$DIR/python-env/bin/python3" -m mgltools.launch vision "$@"
EOF
chmod +x "$OUT_DIR/vision"

cat << 'EOF' > "$OUT_DIR/adt-util"
#!/usr/bin/env bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ -z "$1" ]; then
    echo "Usage: ./adt-util [script_name] [arguments]"
    echo "Example: ./adt-util prepare_receptor4.py -r hsg1.pdb -o hsg1.pdbqt"
    echo "Available scripts in AutoDockTools/Utilities24:"
    ls "$DIR/python-env/lib/python"*/site-packages/AutoDockTools/Utilities24/*.py | xargs -n 1 basename
    exit 1
fi
SCRIPT="$1"
shift
UTIL_DIR=$(ls -d "$DIR/python-env/lib/python"*/site-packages/AutoDockTools/Utilities24)
exec "$DIR/python-env/bin/python3" "$UTIL_DIR/$SCRIPT" "$@"
EOF
chmod +x "$OUT_DIR/adt-util"

# 4. Copy Licenses and Add README
cp "$ROOT/LICENSES" "$OUT_DIR/LICENSES.txt"

cat << 'EOF' > "$OUT_DIR/README.txt"
================================================================================
 MGLTools 1.5.6 (Python 3 Port) - Portable Linux (x86_64) Distribution
================================================================================

This package is standalone and portable for Linux x86_64 systems.

REQUIREMENTS:
- Standard Linux libraries (libGL, libGLU, libX11). On Debian/Ubuntu:
  sudo apt install libgl1-mesa-glx libglu1-mesa
- Python 3 with Tkinter. On Debian/Ubuntu:
  sudo apt install python3-tk

--------------------------------------------------------------------------------
HOW TO RUN:
--------------------------------------------------------------------------------

1. AutoDockTools (ADT) GUI:
   ./adt

2. Python Molecular Viewer (PMV) GUI:
   ./pmv

3. Vision Network Editor:
   ./vision

4. Preparation Scripts (prepare_receptor4, prepare_ligand4, etc.):
   ./adt-util prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
   ./adt-util prepare_ligand4.py -l ligand.pdb -o ligand.pdbqt
   ./adt-util prepare_gpf4.py -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
   ./adt-util prepare_dpf42.py -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf

--------------------------------------------------------------------------------
LICENSE:
--------------------------------------------------------------------------------
MGLTools is free for non-commercial educational and academic research use.
See LICENSES.txt for full copyright and licensing terms.
EOF

# 5. Fix virtualenv relocateability:
sed -i '1s|#!.*|#!/usr/bin/env python3|' "$OUT_DIR/python-env/bin/"* 2>/dev/null || true

# 6. Verify Execution
echo "Verifying portable execution..."
"$OUT_DIR/python-env/bin/python3" -c "import AutoDockTools, Pmv, DejaVu, MolKit, mslib; print('LINUX_PORTABLE_VERIFY_SUCCESS')"

# 7. Compress Archive
echo "Creating tarball: $TAR_FILE..."
cd "$OUT_DIR/.."
tar -czf "$TAR_FILE" "$(basename "$OUT_DIR")"
echo "Done! Package size: $(du -h "$TAR_FILE" | cut -f1)"
