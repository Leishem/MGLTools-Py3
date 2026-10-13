"""Ensure a module-level import line exists in each file, placed just before
the first top-level import statement (found with ast, so docstrings and
comments are never mistaken for code). Removes misplaced copies first.

Usage: python tools/add_import.py "from x import y" file.py [file.py ...]
"""
import ast
import sys

line, files = sys.argv[1], sys.argv[2:]
for path in files:
    with open(path, encoding="utf-8") as f:
        text = f.read()
    lines = text.split("\n")
    lines = [l for l in lines if l.strip() != line.strip()]
    text = "\n".join(lines)
    tree = ast.parse(text)
    first = next((n for n in tree.body if isinstance(n, (ast.Import, ast.ImportFrom))
                  and not (isinstance(n, ast.ImportFrom) and n.module == "__future__")),
                 None)
    if first is None:
        # after a module docstring, else at top
        at = tree.body[0].end_lineno if (tree.body and isinstance(tree.body[0], ast.Expr)
                                          and isinstance(getattr(tree.body[0], "value", None), ast.Constant)) else 0
    else:
        at = first.lineno - 1
    lines.insert(at, line)
    new = "\n".join(lines)
    ast.parse(new)
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(new)
    print(f"{path}: line {at + 1}")
