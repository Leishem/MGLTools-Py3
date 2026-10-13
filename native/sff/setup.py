import sys, os
from os import path
from distutils.core import setup, Extension
from distutils.command.build_clib import build_clib
from distutils.command.install_data import install_data
from distutils.command.sdist import sdist
from os import path
from glob import glob

pack_name = "sff"
ext_name = "_prmlib"
platform = sys.platform

# Lists of include dirs, pre-processor macros, libraries at link-time
# (build_ext).
libs = {'nt':[], 'posix':["m"]}.get(os.name, [])
import numpy
numpy_include =  numpy.get_include()
incl_dir = ["src", numpy_include]
if platform == 'win32':
    macros = [("WIN32", None),]
elif platform == 'darwin':
    macros = [("__APPLE__", None),]
else:
    #macros = [("UNIX", None),]
    macros = []
comp_args = []
if platform == "sunos5":
    comp_args.extend(['-Kpic', '-mt'])

#source files list:   
src_files = [ "bhtree.c", "binpos.c", "memutil.c", "rand2.c",
              "conjgrad.c", "prm.c", "sff.c"]
for i in range(len(src_files)):
    src_files[i] = path.join("src", src_files[i])
src_files.append(path.join("sff", "prmlib.i"))

#data files:
data_files = []
for dir in ["sff/Tests"]:
    files = []
    for f in glob(os.path.join(dir, '*')):
        if f[-3:] != '.py' and f[-4:-1] != '.py' and os.path.isfile(f):
            files.append(f)
    data_files.append((dir, files))
#print "data_files: ", data_files


# Overwrite the run method of the install_data to install the data files
# in the package instead of a particular data directory

class modified_install_data(install_data):

    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')
        return install_data.run(self)

# Overwrite the prune_file_list method of sdist to not
# remove automatically the CVS directories from the distribution.

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
dist = setup(name = pack_name,
             version=VERSION,
             description = "SFF python extension",
             author = 'Molecular Graphics Laboratory',
             author_email = 'mgltools@scripps.edu',
             download_url = 'http://www.scripps.edu/~sanner/software/packager.html',
             url = 'http://www.scripps.edu/~sanner/software/index.html',
             py_modules = ['sff.__init__',
                           'sff.amber'],
             packages = ["sff.Tests"],
             ext_package = pack_name,
             cmdclass = {'install_data': modified_install_data,
                         'sdist': modified_sdist},
             data_files = data_files,
             ext_modules = [Extension (ext_name, src_files,
                                       include_dirs = incl_dir,
                                       define_macros = macros,
                                       #library_dirs = [lib_dir],
                                       libraries = libs,
                                       extra_compile_args = comp_args,
                                       
                                       ) ] ,)

# cp built .so from ./build/lib.platform to ./pack_name/ - for testing.
##  from distutils.util import get_platform
##  from distutils.file_util import copy_file

##  plat_specifier = ".%s-%s" % (get_platform(), sys.version[0:3])
##  orig_ext = path.join(".", "build", "lib"+plat_specifier , pack_name, ext_name+".so")
##  cp_ext = path.join(pack_name, ext_name+".so")
##  if path.isfile(orig_ext):
##      copy_file(orig_ext, cp_ext, update=1)
