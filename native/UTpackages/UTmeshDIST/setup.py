# setup.py is used with Distutils for building UTmesh package

import sys, os
from os import path
from distutils.core import setup, Extension
from distutils.command.build_clib import build_clib
from distutils.command.build_ext import build_ext
from distutils.command.build import build
from distutils.command.sdist import sdist
from distutils.command.install_data import install_data

packFullName = "UTpackages.UTmesh"
packName = "UTmesh"
ext_name = "_lbiemesher"
platform = sys.platform
py_packages =  [packFullName, packFullName+'.Tests']


#  HACK: replace cc with CC (gcc with g++)
CC_exe = 'CC'
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

if platform != "win32":
      sysconfig._init_posix = my_init_posix

# C++ sources :
# 
lbie_sources = ["cellQueue.cpp", "e_face.cpp", "geoframe.cpp", "hexa.cpp",
                "LBIE_Mesher.cpp", "normalspline.cpp", "octree.cpp",
                "pcio.cpp", "tetra.cpp", "mydrawer.cpp"]
               
for i in range(len(lbie_sources)):
    lbie_sources[i] = path.join("src", lbie_sources[i])

# Lists of macros, compiler and linker options 
lbie_macros = []

import numpy
numpy_include =  numpy.get_include()
lbie_include = ["src", numpy_include]
comp_opts = []
link_opts = []

if platform == "darwin":
    lbie_include.append("/usr/include/malloc")
elif platform == "irix6":
    comp_opts.append( "-LANG:std" )
    link_opts.append("-LANG:std")

elif platform == "win32":
    lbie_macros = [("WIN32", None)]
    comp_opts.append("/MT") 
libs = []
if platform == "sunos5":
    libs = ["Crun", "Cstd"]

data_files = [(path.join(packName,"Tests"),
               [path.join(packName, "Tests", "head65.rawiv"),] ), ]

    
# Modify the order of commands called by 'build' command -
# 'build-py' should go after 'build_ext'. This way a python module generated
# by SWIG in 'build_ext'command is copied to the build directory by
# 'build_py' command.
class modified_build(build):

    sub_commands = [('build_clib',    build.has_c_libraries),
                    ('build_ext',     build.has_ext_modules),
                    ('build_py',      build.has_pure_modules),
                    ('build_scripts', build.has_scripts),
                    ]

# Overwrite the run method of the install_data to install the data files
# in the package instead of a particular data directory

class modified_install_data(install_data):

    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')+"UTpackages"
        return install_data.run(self)

# This class overwrites the prune_file_list method of sdist to not
# remove automatically the RCS/CVS directory from the distribution.

 
class modified_sdist(sdist):
    def prune_file_list(self):
 
        build = self.get_finalized_command('build')
        base_dir = self.distribution.get_fullname()
        self.filelist.exclude_pattern(None, prefix=build.build_base)
        self.filelist.exclude_pattern(None, prefix=base_dir)
 

dist = setup(name = packFullName, version="1.0",
             description = "LBIE_Mesher library extension module",
             author = "Molecular Graphics Laboratory",
             author_email = "mgltools@scripps.edu",
             url = "http://www.scripps.edu/~sanner/python/packager.html",
             packages = py_packages,
             package_dir = {packFullName: packName},
             ext_package = packFullName,
             data_files = data_files,
             # use the derived command classes:
             cmdclass = {"build" : modified_build,
                         "install_data": modified_install_data,
                         "sdist" : modified_sdist},
             ext_modules = [Extension (ext_name,
                   [path.join(packName, "lbiemesher.i")]+lbie_sources,
                                       include_dirs = lbie_include,
                                       define_macros = lbie_macros,
                                       #library_dirs = [],
                                       libraries = libs,
                                       extra_compile_args = comp_opts,
                                       extra_link_args = link_opts,          
                                       ) ] ,)

