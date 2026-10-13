from distutils.core import setup, Extension
from distutils.cmd import Command
from distutils.command.install import install
from distutils.command.build import build
from distutils.command.sdist import sdist
from distutils import log
from glob import glob
from distutils.command.build_ext import build_ext
import sys, os
from os import path
import string
import traceback

pack_name = "UTpackages"
packages = ["UTpackages","UTpackages.Tests"]
subpackages = ["UTblur", "UTisocontour", "UTvolrend", "UTmesh", "UTsdf", "UTmolderivatives"]
## if sys.platform == "linux2":
##     subpackages.append("UTimposter")
currdir = os.getcwd()
status=[]


#Overwrite run() method of Distutils build class to loop over
# subpackages and execute their setup.py

class modified_build(build):
    sub_commands = [('build_clib',    build.has_c_libraries),
                    ('build_ext',     build.has_ext_modules),
                    ('build_py',      build.has_pure_modules),
                    ('build_scripts', build.has_scripts),
                   ]
    def run(self):
        args = sys.argv
        if args[1] != "install":
            for pack in subpackages:
                os.chdir(pack+"DIST")
                log.info("Current directory : %s"%os.getcwd() )
                cmd = "%s setup.py build"%sys.executable
                st = os.system(cmd)
                if st != 0:
                    status.append([pack,cmd])
                os.chdir(currdir)
        log.info("Current directory : %s"%os.getcwd())
        #sys.argv = args
        build.run(self)
        

class modified_sdist(sdist):

    def prune_file_list(self):
        """Overvrite the 'sdist' method to not remove automatically the RCS/CVS
        directories from the distribution. """
        build = self.get_finalized_command('build')
        base_dir = self.distribution.get_fullname()
        self.filelist.exclude_pattern(None, prefix=build.build_base)
        self.filelist.exclude_pattern(None, prefix=base_dir)
        
    def run(self):
        for pack in subpackages:
            os.chdir(pack+"DIST")
            log.info("Current directory : %s"%os.getcwd() )
            cmd = "%s setup.py sdist --manifest-only"%sys.executable
            #print "running command: ", cmd
            st = os.system(cmd)
            if st != 0:
                status.append([pack, cmd])
            os.chdir(currdir)
        log.info("Current directory : %s"%os.getcwd() )
        sdist.run(self)

    def read_manifest (self, dirname =None):
        """Read the manifest file (named by 'self.manifest') and use it to
        fill in 'self.filelist', the list of files to include in the source
        distribution.
        """
        log.info("reading manifest file '%s'", self.manifest)
        manifest = open(self.manifest)
        while 1:
            line = manifest.readline()
            if line == '':              # end of file
                break
            if line[-1] == '\n':
                line = line[0:-1]
            if dirname:
                line = path.join(dirname, line)
            self.filelist.append(line)


    def write_manifest (self):
        currmanifest = self.manifest
        for pack in subpackages:
            self.manifest = path.join(pack+"DIST", "MANIFEST")
            self.read_manifest(dirname = pack+"DIST")
        self.manifest = currmanifest
        #print self.filelist.files
        sdist.write_manifest(self)
        
#Overwrite run() method of Distutils install class to loop over
# subpackages and execute their setup.py

class modified_install(install):

    def run(self):
        self.install_lib = self.install_platlib
        install_platlib =  path.abspath(self.install_platlib)
        install_purelib = install_platlib
        print  "install_platlib=", install_platlib
        print "install_purelib=", install_purelib
        args = sys.argv
        bdist = 0
        for i, arg in enumerate(args[1:]):
            if string.find(arg, "--install-platlib") != -1:
                platlib_fl = 1
                args[i+1] = "--install-platlib="+install_platlib
            elif string.find(arg, "--install-purelib") != -1:
                purelib_fl = 1
                args[i+1] = "--install-purelib="+install_purelib
            elif string.find(arg, "bdist") != -1:
                bdist = 1
                args[i+1] = "install"
                args.append("--install-platlib="+install_platlib)
        cmd = "%s"%sys.executable
        for e in args:
            cmd = cmd + " " + e
        print "command: ", cmd
        for pack in subpackages:
            os.chdir(pack+"DIST")
            log.info("Current directory : %s"%os.getcwd() )
            #sys.argv = args 
            #print "args:" , sys.argv
            st = os.system(cmd)
            if st != 0:
                status.append([pack, cmd])
            os.chdir(currdir)
        log.info("Current directory : %s"%os.getcwd() )
        install.run(self)
        
try:   
    from version import VERSION
except:
    VERSION = "1.0"
    
dist = setup(name=pack_name,
             version=VERSION,
             description = "UT Austin software Python extensions",
             author = 'Molecular Graphics Laboratory',
             author_email = "mgltools@scripps.edu",
             download_url = "http://www.scripps.edu/~sanner/python/packager.html",
             url = 'http://www.scripps.edu/~sanner/software/index.html',
             
             packages = packages,
             cmdclass = {'build':modified_build,
                         'sdist':modified_sdist,
                         'install': modified_install},
             
             )
if len(status):
    for i in status:
        print "Error in  package %s , command: '%s' "%(i[0], i[1]) 
