#!/usr/bin/env bash
# Build portable macOS .app bundles (AutoDockTools.app and PMV.app) and zip package
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUT_DIR="${1:-$DIR/dist-macos}"
APP_DIR="$OUT_DIR/MGLTools-1.5.6-macOS-Portable"
ZIP_FILE="$DIR/MGLTools-1.5.6-Py3-macOS-Portable.zip"

echo "=== Packaging MGLTools for macOS ==="
mkdir -p "$APP_DIR"

# 1. Create AutoDockTools.app bundle structure
create_app() {
    local name="$1"
    local title="$2"
    local cmd="$3"
    local app="$APP_DIR/$title.app"
    
    echo "Creating $app..."
    rm -rf "$app"
    mkdir -p "$app/Contents/MacOS"
    mkdir -p "$app/Contents/Resources"
    
    # Info.plist
    cat << EOF > "$app/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>$name</string>
    <key>CFBundleIdentifier</key>
    <string>org.scripps.$name</string>
    <key>CFBundleName</key>
    <string>$title</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.5.6</string>
    <key>LSMinimumSystemVersion</key>
    <string>12.0</string>
</dict>
</plist>
EOF

    # Executable launcher script
    cat << EOF > "$app/Contents/MacOS/$name"
#!/usr/bin/env bash
DIR="\$(cd "\$(dirname "\$0")/../../.." && pwd)"
# Look for local python-env or user Python with Tk
if [ -d "\$DIR/python-env" ]; then
    PYTHON="\$DIR/python-env/bin/python3"
elif [ -d "\$HOME/.mgltools-py3-env" ]; then
    PYTHON="\$HOME/.mgltools-py3-env/bin/python3"
elif command -v python3 &>/dev/null; then
    PYTHON="\$(command -v python3)"
else
    osascript -e 'display alert "Python 3 Not Found" message "Please install Python with Tk via Homebrew (brew install python-tk) or python.org installer."'
    exit 1
fi

exec "\$PYTHON" -m mgltools.launch $cmd "\$@"
EOF
    chmod +x "$app/Contents/MacOS/$name"
}

create_app "adt" "AutoDockTools" "adt"
create_app "pmv" "PMV" "pmv"
create_app "vision" "Vision" "vision"

# 2. Add adt-util command runner
cat << 'EOF' > "$APP_DIR/adt-util.command"
#!/usr/bin/env bash
DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -d "$DIR/python-env" ]; then
    PYTHON="$DIR/python-env/bin/python3"
elif [ -d "$HOME/.mgltools-py3-env" ]; then
    PYTHON="$HOME/.mgltools-py3-env/bin/python3"
else
    PYTHON="python3"
fi

if [ -z "$1" ]; then
    echo "Usage: ./adt-util.command [script_name] [arguments]"
    echo "Example: ./adt-util.command prepare_receptor4.py -r hsg1.pdb -o hsg1.pdbqt"
    exit 1
fi
SCRIPT="$1"
shift
UTIL_DIR=$("$PYTHON" -c "import AutoDockTools, os; print(os.path.join(os.path.dirname(AutoDockTools.__file__), 'Utilities24'))")
exec "$PYTHON" "$UTIL_DIR/$SCRIPT" "$@"
EOF
chmod +x "$APP_DIR/adt-util.command"

# 3. Add setup / installer helper
cat << 'EOF' > "$APP_DIR/setup_mac.command"
#!/usr/bin/env bash
set -e
DIR="$(cd "$(dirname "$0")" && pwd)"
echo "=== Setting up MGLTools on macOS ==="

if ! command -v brew &>/dev/null && ! command -v python3 &>/dev/null; then
    echo "Python 3 is required. Please install Python from https://www.python.org or Homebrew."
    exit 1
fi

ENV_DIR="$DIR/python-env"
if [ ! -d "$ENV_DIR" ]; then
    echo "Creating isolated Python environment in $ENV_DIR..."
    python3 -m venv "$ENV_DIR"
fi

echo "Installing MGLTools dependencies..."
"$ENV_DIR/bin/pip" install --upgrade pip
if ls "$DIR"/mgltools-*.whl 1>/dev/null 2>&1; then
    "$ENV_DIR/bin/pip" install "$DIR"/mgltools-*.whl
else
    "$ENV_DIR/bin/pip" install -e "$DIR/.."
fi

echo "Setup complete! You can now double-click AutoDockTools.app or PMV.app."
EOF
chmod +x "$APP_DIR/setup_mac.command"

# 4. Copy Licenses and Add README
cp "$DIR/LICENSES" "$APP_DIR/LICENSES.txt"

cat << 'EOF' > "$APP_DIR/README.txt"
================================================================================
 MGLTools 1.5.6 (Python 3 Port) - Portable macOS Distribution
================================================================================

Works on:
- Apple Silicon Macs (M1, M2, M3, M4)
- Intel Macs (x86_64)

--------------------------------------------------------------------------------
FIRST TIME SETUP:
--------------------------------------------------------------------------------
1. Ensure Python 3 with Tkinter is installed on your Mac.
   If using Homebrew:
     brew install python-tk
2. Double-click `setup_mac.command` to initialize the isolated environment.

--------------------------------------------------------------------------------
HOW TO RUN:
--------------------------------------------------------------------------------
- Double-click `AutoDockTools.app` to launch AutoDockTools GUI.
- Double-click `PMV.app` to launch Python Molecular Viewer.
- Double-click `Vision.app` to launch Vision Network Editor.
- Use `adt-util.command` in Terminal for command-line preparation scripts:
  ./adt-util.command prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
  ./adt-util.command prepare_ligand4.py -l ligand.pdb -o ligand.pdbqt
  ./adt-util.command prepare_gpf4.py -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
  ./adt-util.command prepare_dpf42.py -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf

--------------------------------------------------------------------------------
LICENSE:
--------------------------------------------------------------------------------
MGLTools is free for non-commercial educational and academic research use.
See LICENSES.txt for full copyright and licensing terms.
EOF

# 5. Compress into zip if zip command available
if command -v zip &>/dev/null; then
    echo "Creating archive: $ZIP_FILE..."
    cd "$OUT_DIR"
    zip -r -q "$ZIP_FILE" "$(basename "$APP_DIR")"
    echo "Archive created: $ZIP_FILE"
fi

echo "=== macOS portable package built at $APP_DIR ==="
