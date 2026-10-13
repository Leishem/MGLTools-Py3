#!/usr/bin/env bash
# Build native modules in place, one at a time, reporting pass/fail.
# Usage: tools/build_native.sh [pkg ...]   (default: every package in setup.py)
cd "$(dirname "$0")/.."
. ~/mglenv/bin/activate
mkdir -p build/logs
pkgs=("$@")
if [ ${#pkgs[@]} -eq 0 ]; then
  pkgs=($(python - <<'EOF'
import re
src = open("setup.py").read()
print(" ".join(sorted(set(re.findall(r'name="(\w+)\._', src)))))
EOF
))
fi
for p in "${pkgs[@]}"; do
  if MGL_ONLY=$p python setup.py build_ext --inplace -j 8 > build/logs/$p.log 2>&1; then
    echo "OK    $p"
  else
    echo "FAIL  $p  ($(grep -cE 'error' build/logs/$p.log) error lines)"
    grep -E 'error|Error' build/logs/$p.log | grep -v 'Werror' | head -8 | sed 's/^/        /'
  fi
done
