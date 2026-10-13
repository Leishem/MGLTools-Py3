"""Build script for the MGLTools Python 3 port.

Pure-Python packages live in src/. Native extensions are built from the
SWIG interfaces in native/<pkg>/: SWIG writes the C wrapper to build/swig/
and the Python shadow module into the matching package in src/.
"""
import os
import subprocess
import sys
from pathlib import Path

import numpy
from setuptools import Extension, find_packages, setup
from setuptools.command.build_ext import build_ext

HERE = Path(__file__).resolve().parent
NATIVE = HERE / "native"
SWIG_OUT = HERE / "build" / "swig"


def n(*parts):
    return str(NATIVE.joinpath(*parts))


# name: dotted module, swig: interface, pkgdir: where the shadow .py goes,
# sources: C/C++ sources, inc: include dirs, cpp: C++ wrapper
MODULES = [
    dict(name="bhtree._bhtreelib", swig=n("bhtree", "bhtree", "bhtreelib.i"),
         pkgdir="bhtree", sources=[n("bhtree", "src", "bhtree.c")],
         inc=[n("bhtree", "src"), n("bhtree", "bhtree"), n("bhtree")]),
]


def selected_modules():
    only = os.environ.get("MGL_ONLY")
    if not only:
        return MODULES
    want = set(only.split(","))
    return [m for m in MODULES if m["name"].split(".")[0] in want]


class SwigBuildExt(build_ext):
    """Run SWIG ourselves so the shadow module lands in src/<pkg>/."""

    def build_extension(self, ext):
        spec = ext._mgl
        SWIG_OUT.mkdir(parents=True, exist_ok=True)
        cpp = spec.get("cpp", False)
        wrap = SWIG_OUT / (Path(spec["swig"]).stem + ("_wrap.cpp" if cpp else "_wrap.c"))
        cmd = [os.environ.get("SWIG", "swig"), "-python"]
        if cpp:
            cmd.append("-c++")
        cmd += [f"-I{d}" for d in [n("common")] + spec["inc"]]
        cmd += spec.get("swig_opts", [])
        cmd += ["-outdir", str(HERE / "src" / spec["pkgdir"]), "-o", str(wrap), spec["swig"]]
        self.announce(" ".join(cmd), level=2)
        subprocess.check_call(cmd)
        ext.sources = [str(wrap)] + spec["sources"]
        super().build_extension(ext)


def make_ext(spec):
    inc = [n("common"), numpy.get_include()] + spec["inc"]
    macros = list(spec.get("macros", []))
    if sys.platform == "win32":
        macros.append(("WIN32", None))
    ext = Extension(spec["name"], sources=[], include_dirs=inc,
                    define_macros=macros,
                    libraries=spec.get("libs", []),
                    library_dirs=spec.get("libdirs", []),
                    extra_compile_args=spec.get("cflags", []),
                    extra_link_args=spec.get("ldflags", []),
                    language="c++" if spec.get("cpp") else "c")
    ext._mgl = spec
    return ext


setup(
    name="mgltools",
    version="1.5.6.post1",
    description="MGLTools (AutoDockTools, PMV) ported to Python 3",
    license="MGLTools license (non-commercial); see LICENSES",
    package_dir={"": "src"},
    packages=find_packages("src"),
    include_package_data=True,
    ext_modules=[make_ext(m) for m in selected_modules()],
    cmdclass={"build_ext": SwigBuildExt},
    python_requires=">=3.10",
    install_requires=["numpy>=1.22", "Pmw>=2.1", "Pillow"],
)
