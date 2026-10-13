"""List triple-quoted strings whose runtime value looks like Python code
(contains 'def ' or 'import ') but does not compile as Python 3."""
import ast, io, pathlib, sys, textwrap, tokenize
SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
n = 0
for p in SRC.rglob("*.py"):
    try:
        toks = tokenize.generate_tokens(io.StringIO(p.read_text(encoding="utf-8")).readline)
        for t in toks:
            if t.type != tokenize.STRING or '"""' not in t.string[:5] and "'''" not in t.string[:5]:
                continue
            try:
                v = ast.literal_eval(t.string)
            except Exception:
                continue
            if not isinstance(v, str) or not ("def " in v or "import " in v):
                continue
            try:
                compile(textwrap.dedent(v), "<s>", "exec")
            except SyntaxError as e:
                if "print " in v or "except " in v or "has_key" in v or "raise " in v:
                    n += 1
                    print(f"{p.relative_to(SRC)}:{t.start[0]}: {e.msg}")
    except (tokenize.TokenError, SyntaxError):
        pass
print(n, "suspicious code strings")
