from distutils.core import setup, Extension
from distutils.cmd import Command
from distutils.command.install_data import install_data
from distutils.command.build import build
from distutils.command.sdist import sdist
from glob import glob
from distutils.command.build_ext import build_ext

import sys, os
from os import path

pack_name = "geomutils"
platform = sys.platform
########################################################################
# Had to overwrite the run method of the install_data to install the data file
# in the package instead of a particular data directory
########################################################################

class modified_install_data(install_data):

    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')
        return install_data.run(self)

#####################################################################
# Had to overwrite the prune_file_list method of sdist to not
# remove automatically the RCS/CVS directory from the distribution.
####################################################################

class modified_sdist(sdist):
    def prune_file_list(self):

        build = self.get_finalized_command('build')
        base_dir = self.distribution.get_fullname()
        self.filelist.exclude_pattern(None, prefix=build.build_base)
        self.filelist.exclude_pattern(None, prefix=base_dir)

########################################################################
# Had to overwrite the sub_commands list of the build command so that
# the build_py is called after build_clib and build_ext. This way
# when building the extension the .py is generated before the other
# py module are. This avoid to have to first call build then install.
########################################################################
        

class modified_build(build):
    sub_commands = [('build_clib',    build.has_c_libraries),
                    ('build_ext',     build.has_ext_modules),
                    ('build_py',      build.has_pure_modules),
                    ('build_scripts', build.has_scripts),
                   ]
    
########################################################################

# list of the python packages to be included in this distribution.
# sdist doesn't go recursively into subpackages so they need to be
# explicitaly listed.
# From these packages only the python modules will be taken
packages = ['geomutils', 'geomutils.Tests', 'geomutils.Tests.Data']

# list of the python modules not part of a package. Give the path and the
# filename without the extension. i.e you want to add the
# test.py module which is located in MyPack/Tests/ you give
# 'MyPack/Tests/test'
py_modules = []

# list of the Macros
definemacros = []
if platform == 'sunos5':
    definemacros.append(('sqrtf', 'sqrt'))
if platform == 'win32':
    definemacros.append(('sqrtf', 'sqrt'))
    definemacros.append(('NO_DRAND48', None))
    definemacros.append(('random', 'rand'))
    definemacros.append(('srandom', 'srand'))

class modified_build_ext (build_ext):
    def spawn(self, cmd, search_path=1, level=1):
        cmd.insert(-1, "-outdir")
        cmd.insert(-1, pack_name)
        print "command: ", cmd
        apply(build_ext.spawn, (self, cmd), {})

    def build_extension(self, ext):
        comp_modif = 0
        swig_cpp = self.swig_cpp
        language = self.compiler.detect_language(ext.sources)
        print "language: ", language
        if language == "c++":
            if platform != "win32":
                comp = self.compiler.compiler[0]
                if comp == "gcc":
                    self.compiler.compiler[0] = 'g++'
                    self.compiler.compiler_so[0] = 'g++'
                    self.compiler.linker_so[0] = 'g++'
                elif comp == "cc":
                    self.compiler.compiler[0] = 'CC'
                    self.compiler.compiler_so[0] = 'CC'
                    self.compiler.linker_so[0] = 'CC'
            self.swig_cpp = 1
            comp_modif = 1
        build_ext.build_extension(self, ext)
        if comp_modif:
             if platform != "win32":
                 if comp == "gcc":
                     self.compiler.compiler[0] = 'gcc'
                     self.compiler.compiler_so[0] = 'gcc'
                     self.compiler.linker_so[0] = 'gcc'
                 elif comp == "cc":
                     self.compiler.compiler[0] = 'cc'
                     self.compiler.compiler_so[0] = 'cc'
                     self.compiler.linker_so[0] = 'cc'
             self.swig_cpp = swig_cpp


if platform == 'win32':
    libs =[]
elif platform == 'darwin':
    libs = ["mx"]
else:
    libs=["m"]

efit_srcfiles = []
for f in ["efit.c", "efit_aux.c", "efit_io.c", "vec.c"]:
    efit_srcfiles.append(path.join("src", "efit", f))
efit_srcfiles.append(path.join("geomutils", "efitlib.i"))
import numpy
numpy_include =  numpy.get_include()
efit_ext = Extension ("_efitlib", efit_srcfiles,
                                       include_dirs = [path.join("src", "efit"),numpy_include],
                                       define_macros = definemacros,
                                       libraries = libs
                                       )
geomalg_libs = []
geomalg_srcfiles = []
geomalg_comp_args = []
geomalg_link_args = []
for f in ["point.cpp", "Vector.cpp", "distance.cpp", "intersections.cpp",
          "polygonArea.cpp", "boundingContainers.cpp",
          "objfile.cpp"
          ]:
    geomalg_srcfiles.append(path.join("src", "geomAlgorithms", f))
geomalg_srcfiles.append(path.join("geomutils", "geomalgorithms.i"))
if platform == "sunos5":
    geomalg_libs.extend(["Crun", "Cstd"])
elif platform == "irix6":
    geomalg_comp_args.append( "-LANG:std" )
    geomalg_link_args.append("-LANG:std")
elif platform == "win32":
     geomalg_comp_args.extend(["/MT"])

geomalg_ext = Extension("_geomalgorithms", geomalg_srcfiles,
                        include_dirs = [path.join("src", "geomAlgorithms"),
                                        numpy_include],
                        define_macros = [],
                        libraries = geomalg_libs,
                        extra_compile_args = geomalg_comp_args,
                        extra_link_args=geomalg_link_args
                        )


sdf_ext = Extension ("_sdflib", [path.join("geomutils", "sdf.i")],
                                       include_dirs = [path.join("src", "efit"),
                                                       numpy_include],
                                       define_macros = [],
                                       libraries = []
                                       )            
########################################################################
# description of what is going to be included in the distribution and
# installed.

try:   
    from version import VERSION
except:
    VERSION = "1.0"
    
if platform == "win32":
    all_modules =  [efit_ext, geomalg_ext]
else:
     all_modules =  [efit_ext, geomalg_ext, sdf_ext]    
dist = setup(name=pack_name,
             version=VERSION,
             description = "geomutils library",
             author = 'Molecular Graphics Laboratory',
             author_email = 'mgltools@scripps.edu',
             download_url = 'http://www.scripps.edu/~sanner/software/packager.html',
             url = 'http://www.scripps.edu/~sanner/software/index.html',
             packages = packages,
             py_modules = py_modules,
             cmdclass = {'build': modified_build,
                         'build_ext': modified_build_ext,
                         'sdist' : modified_sdist, 
                      },
             ext_package = "geomutils",
             ext_modules = all_modules#[efit_ext, geomalg_ext, sdf_ext],
             )
                             
# Remove the .py and wrap.c files generated by swig from the
# MyPack directory
##  import os
##  pyName = './%s/%slib.py'%(pack_name, pack_name)
##  if os.path.exists(pyName):
##      os.system("rm -f %s"%pyName)
##      print "Removing %s generated by swig"%pyName
##  wrapName = './%s/%slib_wrap.c'%(pack_name, pack_name)
##  if os.path.exists(wrapName):
##      os.system("rm -f %s"%wrapName)
##      print "Removing %s generated by swig"%wrapName


