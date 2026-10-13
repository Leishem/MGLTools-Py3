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
from setuptools.command.build import build
from setuptools.command.build_ext import build_ext

HERE = Path(__file__).resolve().parent
NATIVE = HERE / "native"


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

# opengltk: GL/GLU bindings now come from PyOpenGL; only the DejaVu C
# helpers (utillib: dejavu.i drawing loops, trackball, normals) stay native.
OGL = ("opengltk", "opengltk", "extent")
MODULES += [
    dict(name="opengltk.extent._utillib", pkgdir="opengltk/extent",
         swig=n(*OGL, "utillib.i"),
         sources=[n(*OGL, "pythonplus.c")],
         inc=[n(*OGL)],
         macros=[("WIN32", None)] if WIN else [],
         libs=GL_LIBS + LIBM, ldflags=GL_LINK),
]

# UTpackages (Bajaj lab, UT Austin): all C++, one sub-package each.
UT = ("UTpackages",)
UT_CFLAGS = [] if WIN else ["-fpermissive", "-w"]
MODULES += [
    dict(name="UTpackages.UTblur._blur", pkgdir="UTpackages/UTblur", cpp=True,
         swig=n(*UT, "UTblurDIST", "UTblur", "blur.i"), sources=[],
         inc=[n(*UT, "UTblurDIST", "UTblur")], cflags=UT_CFLAGS, libs=LIBM),
    dict(name="UTpackages.UTisocontour._isocontour", pkgdir="UTpackages/UTisocontour",
         cpp=True, swig=n(*UT, "UTisocontourDIST", "UTisocontour", "isocontour.i"),
         sources=files(*UT, "UTisocontourDIST", "src", names=[
             "contour.cpp", "edgehash.cpp", "cellqueue.cpp", "range.cpp",
             "seedcells.cpp", "data.cpp", "dataslc.cpp", "datavol.cpp",
             "segtree.cpp", "contour2d.cpp", "contour3d.cpp", "conplot_p.cpp",
             "conplot.cpp", "conplot2d.cpp", "queue.cpp", "squeue.cpp",
             "rangeprop.cpp", "conplot3d.cpp", "rangesweep.cpp", "datareg2.cpp",
             "conplotreg2.cpp", "dirseeds.cpp", "inttree.cpp", "bucketsearch.cpp",
             "datareg3.cpp", "conplotreg3.cpp", "regprop.cpp", "dirseedsreg2.cpp",
             "regprop2.cpp", "seedchkr2.cpp", "seedchkr3.cpp", "seedall.cpp",
             "respprop2.cpp", "seeddirreg3.cpp", "cubes.cpp", "dict.c"]),
         inc=[n(*UT, "UTisocontourDIST", "src"), n(*UT, "UTisocontourDIST", "UTisocontour")],
         macros=[("SP2", None), ("_BOOL", None), ("USEDICT", None)],
         cflags=UT_CFLAGS, libs=LIBM),
    dict(name="UTpackages.UTmesh._lbiemesher", pkgdir="UTpackages/UTmesh", cpp=True,
         swig=n(*UT, "UTmeshDIST", "UTmesh", "lbiemesher.i"),
         sources=files(*UT, "UTmeshDIST", "src", names=[
             "cellQueue.cpp", "e_face.cpp", "geoframe.cpp", "hexa.cpp",
             "LBIE_Mesher.cpp", "normalspline.cpp", "octree.cpp", "pcio.cpp",
             "tetra.cpp", "mydrawer.cpp"]),
         inc=[n(*UT, "UTmeshDIST", "src"), n(*UT, "UTmeshDIST", "UTmesh")],
         macros=[("WIN32", None)] if WIN else [], cflags=UT_CFLAGS, libs=LIBM),
    dict(name="UTpackages.UTmolderivatives._molderivatives",
         pkgdir="UTpackages/UTmolderivatives", cpp=True,
         swig=n(*UT, "UTmolderivativesDIST", "UTmolderivatives", "molderivatives.i"),
         sources=files(*UT, "UTmolderivativesDIST", "src", "Curvatures", names=[
             "Curvature.cpp", "CurvaturesGridVoxel.cpp", "SumOfGaussiansCurvature.cpp"])
         + files(*UT, "UTmolderivativesDIST", "src", "UsefulMath", names=[
             "LinearAlgebra.cpp", "AreaVolumes.cpp", "Matrix.cpp", "Ray.cpp",
             "Tuple.cpp", "Quaternion.cpp", "TrilinearGrid.cpp", "Vector.cpp"]),
         inc=[n(*UT, "UTmolderivativesDIST", "src", "Curvatures"),
              n(*UT, "UTmolderivativesDIST", "src", "UsefulMath"),
              n(*UT, "UTmolderivativesDIST", "UTmolderivatives")],
         cflags=UT_CFLAGS, libs=LIBM),
    dict(name="UTpackages.UTsdf._utsdf", pkgdir="UTpackages/UTsdf", cpp=True,
         swig=n(*UT, "UTsdfDIST", "UTsdf", "utsdf.i"),
         sources=files(*UT, "UTsdfDIST", "src", names=[
             "common.cpp", "head.cpp", "main.cpp", "octree.cpp", "testing.cpp",
             "compute.cpp", "init.cpp", "new_adjust.cpp", "propagate.cpp"]),
         inc=[n(*UT, "UTsdfDIST", "src"), n(*UT, "UTsdfDIST", "UTsdf")],
         cflags=UT_CFLAGS, libs=LIBM),
    dict(name="UTpackages.UTvolrend._UTVolumeLibrary", pkgdir="UTpackages/UTvolrend",
         cpp=True, swig=n(*UT, "UTvolrendDIST", "UTvolrend", "utvollib.i"),
         sources=files(*UT, "UTvolrendDIST", "src", names=[
             "ClipCube.cpp", "Extent.cpp", "FragmentProgramARBImpl.cpp",
             "FragmentProgramImpl.cpp", "MyExtensions.cpp", "PalettedImpl.cpp",
             "Plane.cpp", "Polygon.cpp", "PolygonArray.cpp", "Renderer.cpp",
             "RendererBase.cpp", "RGBABase.cpp", "SGIColorTableImpl.cpp",
             "SimpleRGBAImpl.cpp", "UnshadedBase.cpp", "VolumeRenderer.cpp",
             "VolumeRendererFactory.cpp", "SimpleRGBA2DImpl.cpp", "Paletted2DImpl.cpp"]),
         inc=[n(*UT, "UTvolrendDIST", "src"), n(*UT, "UTvolrendDIST", "UTvolrend")],
         macros=[("WIN32", None)] if WIN else ([("__APPLE__", None)] if MAC else [("UNIX", None)]),
         cflags=UT_CFLAGS,
         libs=GL_LIBS + LIBM, ldflags=GL_LINK),
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
        # per-platform/per-interpreter dir (build/temp.<plat>-<py>/swig) so
        # concurrent builds for different platforms do not collide
        swig_out = Path(self.build_temp).resolve() / "swig"
        swig_out.mkdir(parents=True, exist_ok=True)
        cpp = spec.get("cpp", False)
        wrap = swig_out / (Path(spec["swig"]).stem + ("_wrap.cpp" if cpp else "_wrap.c"))
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
    # accept NumPy scalars (and anything with __float__/__int__) where the
    # C API wants double/int, as SWIG 1.3 did
    macros = list(spec.get("macros", [])) + [("SWIG_PYTHON_CAST_MODE", None)]
    cflags = list(spec.get("cflags", []))
    if WIN:
        macros += [("WIN32", None), ("_USE_MATH_DEFINES", None),
                   ("_CRT_SECURE_NO_WARNINGS", None)]
        # stand-ins for POSIX headers (unistd.h, sys/time.h, strings.h)
        inc.append(n("common", "win"))
    if MAC:
        # <GL/gl.h> -> <OpenGL/gl.h> forwarding headers; OpenGL is deprecated
        # on macOS but still supported (legacy 2.1 profile)
        inc.insert(0, n("common", "macos"))
        macros.append(("GL_SILENCE_DEPRECATION", None))
        cflags += ["-Wno-deprecated-declarations",
                   # 1990s/2000s C: Apple clang makes these errors, gcc warns
                   "-Wno-error=implicit-function-declaration",
                   "-Wno-error=implicit-int",
                   "-Wno-error=incompatible-function-pointer-types",
                   "-Wno-error=int-conversion"]
    # Python 2 C-API aliases (and Windows GL macros) for every translation unit
    force = ["/FImgl_py2compat.h"] if WIN else ["-include", "mgl_py2compat.h"]
    ext = Extension(spec["name"], sources=[], include_dirs=inc,
                    define_macros=macros,
                    libraries=spec.get("libs", []),
                    library_dirs=spec.get("libdirs", []),
                    extra_compile_args=force + cflags,
                    extra_link_args=spec.get("ldflags", []),
                    language="c++" if spec.get("cpp") else "c")
    ext._mgl = spec
    return ext


class BuildExtFirst(build):
    """Run build_ext before build_py so SWIG-generated proxies are packaged."""
    sub_commands = [c for c in build.sub_commands if c[0] == "build_ext"] + \
                   [c for c in build.sub_commands if c[0] != "build_ext"]


setup(
    name="mgltools",
    version="1.5.6.post1",
    description="MGLTools 1.5.6 (AutoDockTools, PMV, Vision) on Python 3",
    long_description=(HERE / "README.md").read_text(encoding="utf-8")
    if (HERE / "README.md").exists() else "",
    long_description_content_type="text/markdown",
    license="MGLTools license (non-commercial); see LICENSES",
    package_dir={"": "src"},
    packages=find_packages("src"),
    package_data={"": ["**/*"]},
    exclude_package_data={"": ["**/__pycache__/*", "**/*.pyc", "**/CVS/*",
                               "**/*.o", "**/*.obj"]},
    ext_modules=[make_ext(m) for m in selected_modules()],
    cmdclass={"build_ext": SwigBuildExt, "build": BuildExtFirst},
    python_requires=">=3.10",
    install_requires=[
        "numpy>=1.22",
        "scipy>=1.10",         # mslib molecular surfaces
        "Pmw>=2.1",
        "Pillow",
        "PyOpenGL>=3.1.7",
        "tkinter-gl>=1.1",     # TkGL: Togl-compatible Tk OpenGL widget
        "simpy==2.3.1",        # classic SimPy API used by Scenario2 animations
    ],
    entry_points={"gui_scripts": ["adt = mgltools.launch:adt",
                                  "pmv = mgltools.launch:pmv",
                                  "vision = mgltools.launch:vision"]},
)
