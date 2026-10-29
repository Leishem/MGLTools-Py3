#!/usr/bin/env bash
# Build a self-contained, portable macOS package of MGLTools (ADT, PMV, Vision).
#
# The package bundles its own relocatable CPython (python-build-standalone,
# which includes Tk) with the mgltools wheel and all dependencies installed,
# so the target Mac needs no Python, Homebrew or Xcode.
#
# Usage: tools/mac/make_portable_macos.sh [arm64|x86_64]   (default: host arch)
#   PY_VERSION=3.12.15 PBS_TAG=20261013   pick the bundled Python build
#
# Build requirements (build machine only): Xcode Command Line Tools, curl.
# For x86_64 on Apple Silicon, Rosetta 2 must be installed.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ARCH="${1:-$(uname -m)}"
PY_VERSION="${PY_VERSION:-3.12.15}"
PBS_TAG="${PBS_TAG:-20261013}"
VERSION="1.5.6"

case "$ARCH" in
    arm64|aarch64) ARCH=arm64; PBS_ARCH=aarch64; RUN="" ;;
    x86_64)        PBS_ARCH=x86_64; RUN="arch -x86_64" ;;
    *) echo "unknown arch: $ARCH (use arm64 or x86_64)"; exit 1 ;;
esac

NAME="MGLTools-$VERSION-Py3-macOS-$ARCH-Portable"
OUT="$ROOT/dist-macos"
PKG="$OUT/$NAME"
CACHE="$OUT/cache"
ZIP_FILE="$OUT/$NAME.zip"

echo "=== Building $NAME ==="
rm -rf "$PKG" "$ZIP_FILE"
mkdir -p "$PKG" "$CACHE"

# 1. Relocatable Python with Tk
PBS="cpython-$PY_VERSION+$PBS_TAG-$PBS_ARCH-apple-darwin-install_only_stripped.tar.gz"
if [ ! -f "$CACHE/$PBS" ]; then
    echo "Downloading $PBS..."
    curl -fSL -o "$CACHE/$PBS" \
        "https://github.com/astral-sh/python-build-standalone/releases/download/$PBS_TAG/${PBS//+/%2B}"
fi
tar -xzf "$CACHE/$PBS" -C "$PKG"            # -> $PKG/python
PY="$PKG/python/bin/python3"
export PYTHONNOUSERSITE=1

# 2. Build the wheel with the bundled interpreter (same ABI/arch) and install it
WHEEL_DIR="$OUT/wheel-$ARCH"
rm -rf "$WHEEL_DIR"
$RUN "$PY" -m pip install --quiet --upgrade pip
$RUN "$PY" -m pip wheel "$ROOT" -w "$WHEEL_DIR" --no-deps
$RUN "$PY" -m pip install --no-warn-script-location "$WHEEL_DIR"/mgltools-*.whl
$RUN "$PY" -m pip uninstall --quiet -y pip
find "$PKG/python" -name __pycache__ -type d -prune -exec rm -rf {} +
# console-script shims hard-code the build path; the launchers below replace them
rm -f "$PKG/python/bin/"{adt,pmv,vision}

# 3. App icon (from the PMV icon set)
ICNS="$OUT/adt.icns"
ICONSET="$OUT/adt.iconset"
rm -rf "$ICONSET"; mkdir -p "$ICONSET"
for s in 16 32 128; do
    cp "$ROOT/src/Pmv/Icons/${s}x${s}/adt.png" "$ICONSET/icon_${s}x${s}.png"
done
cp "$ROOT/src/Pmv/Icons/32x32/adt.png" "$ICONSET/icon_16x16@2x.png"
cp "$ROOT/src/Pmv/Icons/64x64/adt.png" "$ICONSET/icon_32x32@2x.png"
sips -z 256 256 "$ROOT/src/Pmv/Icons/128x128/adt.png" --out "$ICONSET/icon_128x128@2x.png" >/dev/null
iconutil -c icns "$ICONSET" -o "$ICNS"
rm -rf "$ICONSET"

# 4. .app bundles; they run ../python, so they must stay next to that folder
create_app() {
    local exe="$1" title="$2"
    local app="$PKG/$title.app"
    mkdir -p "$app/Contents/MacOS" "$app/Contents/Resources"
    cp "$ICNS" "$app/Contents/Resources/adt.icns"
    cat > "$app/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>        <string>$exe</string>
    <key>CFBundleIdentifier</key>        <string>org.scripps.mgltools.$exe</string>
    <key>CFBundleName</key>              <string>$title</string>
    <key>CFBundleDisplayName</key>       <string>$title</string>
    <key>CFBundleIconFile</key>          <string>adt.icns</string>
    <key>CFBundlePackageType</key>       <string>APPL</string>
    <key>CFBundleShortVersionString</key><string>$VERSION</string>
    <key>CFBundleVersion</key>           <string>$VERSION</string>
    <key>LSMinimumSystemVersion</key>    <string>11.0</string>
    <key>NSHighResolutionCapable</key>   <true/>
</dict>
</plist>
EOF
    sed -e "s|@EXE@|$exe|g" -e "s|@TITLE@|$title|g" > "$app/Contents/MacOS/$exe" << 'EOF'
#!/bin/bash
# Launcher for @TITLE@.app: runs the bundled Python next to the .app bundles.
APP="$(cd "$(dirname "$0")/../.." && pwd)"
DIR="$(dirname "$APP")"
PY="$DIR/python/bin/python3"
alert() {  # alert <title> <message> [buttons applescript list] -> button name
    osascript - "$1" "$2" "${3:-{\"OK\"}}" << 'OSA'
on run argv
    set btns to run script (item 3 of argv)
    return button returned of (display alert (item 1 of argv) message (item 2 of argv) buttons btns default button (count of btns))
end run
OSA
}
if [[ "$APP" == */AppTranslocation/* ]]; then
    alert "macOS is running @TITLE@ from a temporary location" "This package was downloaded from the internet. Right-click \"Fix-Gatekeeper.command\" in the MGLTools folder, choose Open, then open @TITLE@ again."
    exit 1
fi
# Downloads, Desktop and Documents are privacy-protected: macOS silently
# refuses reads by this unsigned app ("Operation not permitted"), so Python
# cannot even load its standard library. Offer to move the folder.
if ! ls "$DIR/python/lib" >/dev/null 2>&1; then
    case "$DIR" in
        "$HOME/Downloads"*) where="Downloads" ;;
        "$HOME/Desktop"*)   where="Desktop" ;;
        "$HOME/Documents"*) where="Documents" ;;
        *)                  where="this" ;;
    esac
    name="$(basename "$DIR")"
    btn=$(alert "@TITLE@ can't run from the $where folder" "macOS does not let apps that are not from the App Store or an identified developer read files in the $where folder, so MGLTools cannot load its bundled Python there.

Move the \"$name\" folder to your Applications folder (Finder will do it for you), or drag it there yourself." '{"Quit", "Show Me", "Move to Applications"}')
    if [ "$btn" = "Move to Applications" ]; then
        mkdir -p "$HOME/Applications"
        if [ -e "$HOME/Applications/$name" ]; then
            alert "Already in Applications" "\"$name\" already exists in your Applications folder ($HOME/Applications). Open @TITLE@ from there, or remove the old copy first."
            open -R "$HOME/Applications/$name"
            exit 1
        fi
        if osascript -e 'on run argv' -e 'set src to POSIX file (item 1 of argv)' -e 'set dst to POSIX file (item 2 of argv)' -e 'tell application "Finder" to move src to dst' -e 'end run' "$DIR" "$HOME/Applications" >/dev/null 2>&1; then
            open "$HOME/Applications/$name/@TITLE@.app"
        else
            alert "Could not move the folder" "Please drag \"$name\" from $where into the Applications folder yourself, then open @TITLE@ from there."
            open -R "$DIR"; open "$HOME/Applications"
        fi
    elif [ "$btn" = "Show Me" ]; then
        mkdir -p "$HOME/Applications"
        open -R "$DIR"; open "$HOME/Applications"
    fi
    exit 1
fi
if [ ! -x "$PY" ]; then
    alert "Bundled Python not found" "Keep @TITLE@.app inside the MGLTools folder, next to the python folder."
    exit 1
fi
export PYTHONNOUSERSITE=1 PYTHONWARNINGS=ignore::SyntaxWarning,ignore::DeprecationWarning
unset PYTHONHOME PYTHONPATH
mkdir -p "$HOME/.mgltools"; cd "$HOME"
exec "$PY" -m mgltools.launch @EXE@ "$@" >> "$HOME/.mgltools/@EXE@.log" 2>&1
EOF
    chmod +x "$app/Contents/MacOS/$exe"
}
create_app adt    AutoDockTools
create_app pmv    PMV
create_app vision Vision

# 5. Terminal helpers
cat > "$PKG/adt-util.command" << 'EOF'
#!/bin/bash
# Run an AutoDockTools Utilities24 script with the bundled Python, e.g.
#   ./adt-util.command prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
DIR="$(cd "$(dirname "$0")" && pwd)"
PY="$DIR/python/bin/python3"
export PYTHONNOUSERSITE=1 PYTHONWARNINGS=ignore::SyntaxWarning,ignore::DeprecationWarning
unset PYTHONHOME PYTHONPATH
UTIL_DIR="$("$PY" -c "import AutoDockTools, os; print(os.path.join(os.path.dirname(AutoDockTools.__file__), 'Utilities24'))")"
if [ -z "$1" ]; then
    echo "Usage: ./adt-util.command <script.py> [arguments]"
    echo "Example: ./adt-util.command prepare_receptor4.py -r hsg1.pdb -o hsg1.pdbqt"
    echo "Available scripts:"
    ls "$UTIL_DIR" | grep '\.py$' | grep -v '^__' | column
    exit 1
fi
SCRIPT="$1"; shift
exec "$PY" "$UTIL_DIR/$SCRIPT" "$@"
EOF

cat > "$PKG/Fix-Gatekeeper.command" << 'EOF'
#!/bin/bash
# Remove the "downloaded from the internet" flag so the apps can run in place.
DIR="$(cd "$(dirname "$0")" && pwd)"
xattr -dr com.apple.quarantine "$DIR"
echo "Done. You can now double-click AutoDockTools.app, PMV.app or Vision.app."
EOF
chmod +x "$PKG/"*.command

# 6. Licenses and README
cp "$ROOT/LICENSES" "$PKG/LICENSES.txt"
cat > "$PKG/README.txt" << EOF
================================================================================
 MGLTools $VERSION (Python 3 Port) - Portable macOS ($ARCH)
================================================================================

Self-contained: includes its own Python $PY_VERSION, Tk and all libraries.
Nothing else needs to be installed. Requires macOS 11 or later on
$( [ "$ARCH" = arm64 ] && echo "an Apple Silicon Mac (M1/M2/M3/M4...)" || echo "an Intel Mac" ).

FIRST RUN
---------
1. Move the unzipped "$NAME" folder to your Applications folder
   (or any folder that is NOT Downloads, Desktop or Documents: macOS does not
   let unsigned apps read files there, and the apps will offer to move the
   folder for you). Keep all files together: the .app files use the
   "python" folder next to them.
2. Because the apps are not signed by Apple, macOS blocks them the first time.
   Right-click "Fix-Gatekeeper.command" > Open > Open. This removes the
   download flag from the folder. (Alternative, in Terminal:
     xattr -dr com.apple.quarantine /path/to/$NAME )

RUN
---
- AutoDockTools.app   AutoDockTools GUI
- PMV.app             Python Molecular Viewer
- Vision.app          Vision visual-programming environment

Errors are written to ~/.mgltools/adt.log (pmv.log, vision.log).

COMMAND-LINE PREPARATION SCRIPTS
--------------------------------
In Terminal, from this folder:
  ./adt-util.command prepare_receptor4.py -r receptor.pdb -o receptor.pdbqt
  ./adt-util.command prepare_ligand4.py   -l ligand.pdb   -o ligand.pdbqt
  ./adt-util.command prepare_gpf4.py      -l ligand.pdbqt -r receptor.pdbqt -o grid.gpf
  ./adt-util.command prepare_dpf42.py     -l ligand.pdbqt -r receptor.pdbqt -o dock.dpf

LICENSE
-------
MGLTools is free for non-commercial educational and academic research use.
See LICENSES.txt for full copyright and licensing terms.
EOF

# 7. Verify, then zip (ditto keeps the exec bits and bundle structure)
echo "Verifying..."
$RUN "$PY" -c "import AutoDockTools, Pmv, Vision, DejaVu, MolKit, mslib, tkinter_gl, opengltk.extent._utillib; print('MACOS_PORTABLE_VERIFY_SUCCESS')"
# drop the build-time caches, then precompile so first launch is quiet and fast
find "$PKG/python" -name __pycache__ -type d -prune -exec rm -rf {} +
$RUN "$PY" -W ignore -m compileall -q -j 0 "$PKG/python/lib" >/dev/null || true
ditto -c -k --sequesterRsrc --keepParent "$PKG" "$ZIP_FILE"
echo "=== Done: $PKG"
echo "    Archive: $ZIP_FILE ($(du -h "$ZIP_FILE" | cut -f1))"
