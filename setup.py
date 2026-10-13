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


def files(*parts, names=None, exts=(".c", ".cpp", ".cxx")):
    d = NATIVE.joinpath(*parts)
    if names:
        return [str(d / f) for f in names]
    return sorted(str(p) for p in d.iterdir() if p.suffix in exts)


WIN = sys.platform == "win32"
MAC = sys.platform == "darwin"
LIBM = [] if WIN else ["m"]

if WIN:
    GL_LIBS = ["opengl32", "glu32"]
    GL_LINK = []
elif MAC:
    GL_LIBS = []
    GL_LINK = ["-framework", "OpenGL"]
else:
    GL_LIBS = ["GL", "GLU"]
    GL_LINK = []

# name: dotted module, swig: interface, pkgdir: where the shadow .py goes,
# sources: C/C++ sources, inc: include dirs, cpp: C++ wrapper
MODULES = [
    dict(name="bhtree._bhtreelib", swig=n("bhtree", "bhtree", "bhtreelib.i"),
         pkgdir="bhtree", sources=[n("bhtree", "src", "bhtree.c")],
         inc=[n("bhtree", "src"), n("bhtree", "bhtree"), n("bhtree")]),
    # GROMACS .xtc reader; needs Sun XDR (libtirpc on Linux, libSystem on
    # macOS). No XDR implementation ships for Windows, so it is skipped there.
    *([] if WIN else [dict(
        name="cMolKit._xtcparser", swig=n("cMolKit", "cMolKit", "xtcparser.i"),
        pkgdir="cMolKit",
        sources=files("cMolKit", "src", names=["libxdrf.c", "xtcio.c"]),
        inc=[n("cMolKit", "src"), n("cMolKit", "cMolKit")]
        + ([] if MAC else ["/usr/include/tirpc"]),
        libs=[] if MAC else ["tirpc"])]),
    dict(name="gle._gle", swig=n("gle", "gle", "gle.i"), pkgdir="gle",
         sources=files("gle", "src"), inc=[n("gle", "src"), n("gle", "gle")],
         libs=GL_LIBS + LIBM, ldflags=GL_LINK),
    dict(name="pyglf._glf", swig=n("pyglf", "pyglf", "glf.i"), pkgdir="pyglf",
         sources=files("pyglf", "src", names=["glf.c"]),
         inc=[n("pyglf", "src"), n("pyglf", "pyglf")],
         libs=GL_LIBS + LIBM, ldflags=GL_LINK),
    dict(name="sff._prmlib", swig=n("sff", "sff", "prmlib.i"), pkgdir="sff",
         sources=files("sff", "src", names=["bhtree.c", "binpos.c", "memutil.c",
                                            "rand2.c", "prm.c", "sff.c", "conjgrad.c"]),
         inc=[n("sff", "src"), n("sff", "sff")], libs=LIBM),
    dict(name="geomutils._efitlib", swig=n("geomutils", "geomutils", "efitlib.i"),
         pkgdir="geomutils", sources=files("geomutils", "src", "efit"),
         inc=[n("geomutils", "src", "efit"), n("geomutils", "geomutils")],
         macros=[("NO_DRAND48", None), ("random", "rand"), ("srandom", "srand")] if WIN else [],
         libs=LIBM),
    dict(name="geomutils._geomalgorithms",
         swig=n("geomutils", "geomutils", "geomalgorithms.i"), pkgdir="geomutils",
         sources=files("geomutils", "src", "geomAlgorithms"), cpp=True,
         inc=[n("geomutils", "src", "geomAlgorithms"), n("geomutils", "geomutils")]),
    dict(name="geomutils._sdflib", swig=n("geomutils", "geomutils", "sdf.i"),
         pkgdir="geomutils", sources=[],
         inc=[n("geomutils", "src", "efit"), n("geomutils", "geomutils")], libs=LIBM),
    dict(name="QSlimLib._qslimlib", swig=n("QSlimLib", "QSlimLib", "qslimlib.i"),
         pkgdir="QSlimLib", cpp=True,
         sources=files("QSlimLib", "src", "libgfx", "src", names=[
             "mat2.cxx", "mat3.cxx", "mat4.cxx", "quat.cxx", "symmat3.cxx",
             "geom3d.cxx", "time.cxx", "script.cxx", "raster-pnm.cxx"])
         + files("QSlimLib", "src", "mixkit", names=[
             "mixmsg.cxx", "MxCmdParser.cxx", "MxAsp.cxx", "MxMat3-jacobi.cxx",
             "MxMat4-jacobi.cxx", "MxGeom3D.cxx", "MxTriProject.cxx", "MxFrame.cxx",
             "MxFitFrame.cxx", "MxFitFrame-2.cxx", "MxGeom2D.cxx", "mixmops.cxx",
             "MxBlockModel.cxx", "MxStdModel.cxx", "MxSMF.cxx", "MxQMetric3.cxx",
             "MxQSlim.cxx", "MxStdSlim.cxx", "MxQMetric.cxx", "MxPropSlim.cxx",
             "MxDualModel.cxx", "MxFaceTree.cxx", "MxFaceTree-2.cxx",
             "MxDualSlim.cxx", "MxEdgeFilter.cxx", "MxFeatureFilter.cxx", "MxHeap.cxx"]),
         inc=[n("QSlimLib", "src", "libgfx", "include"), n("QSlimLib", "src", "mixkit"),
              n("QSlimLib", "QSlimLib")],
         macros=[("HAVE_SSTREAM", 1), ("HAVE_BOOL", 1)]
         + ([("WIN32", None)] if WIN else [("HAVE_GETRUSAGE", 1)]),
         cflags=[] if WIN else ["-fpermissive", "-w"], libs=LIBM),
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
        # .i files include "src/..." relative to the module root (parent of
        # the directory holding the .i), as the upstream setup.py ran there.
        root = str(Path(spec["swig"]).parent.parent)
        cmd += [f"-I{d}" for d in [n("common"), root] + spec["inc"]]
        cmd += spec.get("swig_opts", [])
        cmd += ["-outdir", str(HERE / "src" / spec["pkgdir"]), "-o", str(wrap), spec["swig"]]
        self.announce(" ".join(cmd), level=2)
        subprocess.check_call(cmd)
        ext.sources = [str(wrap)] + spec["sources"]
        super().build_extension(ext)


def make_ext(spec):
    root = str(Path(spec["swig"]).parent.parent)
    inc = [n("common"), numpy.get_include(), root] + spec["inc"]
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
