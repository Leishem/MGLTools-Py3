# setup.py is used with Distutils for building QSlimLib package

import sys, os
from os import path
from distutils.core import setup, Extension
from distutils.command.build_clib import build_clib
from distutils.command.build_ext import build_ext
from distutils.command.build import build
from distutils.command.sdist import sdist
from distutils.command.install_data import install_data

pack_name = "QSlimLib"
ext_name = "_qslimlib"
platform = sys.platform
py_packages =  ['QSlimLib', 'QSlimLib.Tests']
data_files = []

#  HACK: replace cc with CC (gcc with g++)
CC_exe = 'g++'
cc_exe = 'cc'

if platform == "linux2" or platform == "darwin":
    CC_exe = 'g++'
    cc_exe = 'gcc'
from distutils import sysconfig
save_init_posix = sysconfig._init_posix
def my_init_posix():
    save_init_posix()
    g = sysconfig._config_vars
    for n,r in [('LDSHARED',CC_exe),('CC',CC_exe)]:
        if g[n][:3] == cc_exe:
            print 'my_init_posix: changing %s = %r'%(n,g[n]),
            g[n] = r+g[n][3:]
            print 'to',`g[n]`

if platform in ('linux2', 'darwin'):
      sysconfig._init_posix = my_init_posix

# C++ libraries description :
# libgfx.a
## gfx_sources = ["mat2.cxx", "mat3.cxx", "mat4.cxx", "quat.cxx", "symmat3.cxx",
##                "geom3d.cxx", "raster.cxx", "raster-pnm.cxx",
##                "time.cxx",  "script.cxx"]

gfx_sources = ["mat2.cxx", "mat3.cxx", "mat4.cxx", "quat.cxx", "symmat3.cxx",
               "geom3d.cxx", "time.cxx",  "script.cxx", "raster-pnm.cxx"]
for i in range(len(gfx_sources)):
    gfx_sources[i] = path.join("src", "libgfx", "src", gfx_sources[i])
    
gfx_macros = []
if platform == "win32":
    gfx_macros = [("WIN32", None),("HAVE_SSTREAM", 1)]
else:
    gfx_macros = [("HAVE_CONFIG_H", None)]

if platform in ('irix6'):
    gfx_macros.append(("HAVE_STRSTREAM", 1))
    
import numpy
numpy_include =  numpy.get_include()    
gfx_include = [path.join("src", "libgfx", "include"), numpy_include]
libgfx  = ( "gfx", {'sources': gfx_sources,
                    'macros':  gfx_macros,
                    'include_dirs': gfx_include,} )

# libmix.a
mix_sources = ["mixmsg.cxx", "MxCmdParser.cxx", "MxAsp.cxx",
               "MxMat3-jacobi.cxx", "MxMat4-jacobi.cxx",
               "MxGeom3D.cxx", "MxTriProject.cxx", "MxFrame.cxx",
               "MxFitFrame.cxx", "MxFitFrame-2.cxx", "MxGeom2D.cxx",
               "mixmops.cxx",
               "MxBlockModel.cxx", "MxStdModel.cxx", "MxSMF.cxx",
               "MxQMetric3.cxx", "MxQSlim.cxx", "MxStdSlim.cxx",
               "MxQMetric.cxx",  "MxPropSlim.cxx", "MxDualModel.cxx",
               "MxFaceTree.cxx", "MxFaceTree-2.cxx", "MxDualSlim.cxx",
               "MxEdgeFilter.cxx", "MxFeatureFilter.cxx", "MxHeap.cxx"]
for i in range(len(mix_sources)):
    mix_sources[i] = path.join("src", "mixkit" , mix_sources[i])
    
mix_macros = gfx_macros
mix_include = gfx_include
mix_include.append(path.join("src", "mixkit"))
libmix  = ( "mix", {'sources': mix_sources,
                    'macros':  mix_macros,
                    'include_dirs': mix_include,} )

# Overwrite the sub_commands list of the build command so that
# the build_py is called after build_clib and build_ext. This way
# a python module generated  by SWIG in 'build_ext'command is copied to
#the build directory by 'build_py' command
                    
class modified_build(build):

    sub_commands = [('build_clib',    build.has_c_libraries),
                    ('build_ext',     build.has_ext_modules),
                    ('build_py',      build.has_pure_modules),
                    ('build_scripts', build.has_scripts),
                    ]
    def __init__(self, dist):
        if platform != "win32":
            print "running ./configure"
            os.system("./configure --cache-file=/dev/null")
        apply(build.__init__, (self, dist), {})

class modified_build_ext(build_ext):
    def build_extensions(self):
        if platform in ("sunos5", "irix6"):
            self.compiler.compiler_cxx[0] = 'g++'
            self.compiler.compiler_so = ['g++']
            self.compiler.linker_so = ['g++', '-shared']
        #print self.compiler.compiler_so
        build_ext.build_extensions(self)
        
class modified_build_clib(build_clib):

    def build_libraries(self, libraries):
        if platform in ("sunos5", "irix6"):
            #print "compiler: ", self.compiler.compiler
            self.compiler.compiler = ['g++']
            self.compiler.compiler_so = ['g++']
        #print "archiver: ", self.compiler.archiver
        if platform != "win32":
            self.compiler.compiler.append('-fpermissive')
            self.compiler.compiler_so.append('-fpermissive')
        
        build_clib.build_libraries(self, libraries)

# Overwrite the prune_file_list method of sdist to not
# remove automatically the RCS/CVS directory from the distribution.

class modified_sdist(sdist):
    def prune_file_list(self):
        build = self.get_finalized_command('build')
        base_dir = self.distribution.get_fullname()
        self.filelist.exclude_pattern(None, prefix=build.build_base)
        self.filelist.exclude_pattern(None, prefix=base_dir)
       
try:   
    from version import VERSION
except:
    VERSION = "1.0"
sources = []
static_libs = []
if platform == "win32":
    sources.extend(mix_sources)
    sources.extend(gfx_sources)
else:
    static_libs = [libmix, libgfx]
sources.append(path.join("QSlimLib", "qslimlib.i"))

dist = setup(name = pack_name,
             version=VERSION,
             description = "QSlim library extension module",
             author = "Molecular Graphics Laboratory",
             author_email = "mgltools@scripps.edu",
             url = "http://www.scripps.edu/~sanner/python/packager.html",
             packages = py_packages,
             ext_package = pack_name,
             data_files = data_files,
             # use the derived command classes:
             cmdclass = {"build" : modified_build,
                         "build_clib" : modified_build_clib,
                         "build_ext" : modified_build_ext,
                         "sdist": modified_sdist},
             libraries = static_libs,
             ext_modules = [Extension (ext_name,
                                    #   [path.join("QSlimLib", "qslimlib.i")],
                                       sources,
                            include_dirs = mix_include,
                            define_macros = mix_macros,
                            #library_dirs = [],
                            libraries = {"nt":[], "posix":["m"]}.get(os.name),
                            extra_compile_args = {"nt":["/MT"], "posix":["-fpermissive"]}.get(os.name),           
                     ) ] ,)

