##################################################################
##################################################################
import sys,os,types, string
from string import find
from platform import machine
platform  = sys.platform
machine_arch = machine()
mgluser = False
#The user has to edit the following 'include' directories if necessary:

if os.name == 'nt': #for Windows
    #gl_include = "C:/Program Files/Microsoft Visual C++ Toolkit 2003/include"
    #gl_include = "C:\Program Files\Microsoft Platform SDK\Include"
    gl_include = "C:\\Program Files\\Microsoft Platform SDK for Windows Server 2003 R2\\Include"
else:
    gl_include = ""

##########  Directory containing GL/glut.h: #######################
#if os.name =='posix': # for POSIX OS
#
#    # if MGL_EXTRAINCLUDE environment variable is defined, use the path
#    # it is set to:
#    if os.environ.has_key('MGL_EXTRAINCLUDE'):
#        glut_include = os.environ['MGL_EXTRAINCLUDE']
#    else:
#        # else use default path
#        glut_include = "./"
#    if mgluser:# for building in MGL:
#        if platform in ("sunos5", "irix6"):
#            glut_include =  "/mgl/apps/include"
#            
#elif os.name == 'nt': #for Windows
#    glut_include = gl_include

########## Directory containing libgl.a : ######################
if os.name=='posix':  # for POSIX OS
    # if MGL_EXTRALIBS environment variable is defined, use the path
    # it is set to:
    if os.environ.has_key('MGL_EXTRALIBS'):
        gl_lib_dir  = [os.environ['MGL_EXTRALIBS']]
    else:
        # else - use path to default directory
        gl_lib_dir= ["/usr/lib"]
    if mgluser: # for building in MGL:
        gl_lib_dir= {"linux2":["/mgl/prog/i86Linux2/lib"], 
                       "irix6": ["/mgl/prog/sgi4DIRIX646/lib"],
                       "sunos5":["/mgl/prog/sun4SunOS5/lib"]}.get(platform, ["/usr/lib"])
else:
    gl_lib_dir = []


############ build_togl  ##########################################

build_togl=1 #if set to 1 - try to build Togl extension
             # 0 - do not build Togl extension
# MACOSX option
# we recommend to build for X11 as our system is not fully tested
# under AQUA and do not run correctly.
# You will need to have a source installation of Tcl/Tk

darwin_aqua = 0 # set to 1 will build Togl for Darwin Aqua
                # set to 0 will build Togl for Darwin X11 (default)

# where the source tree of tcl/tk is needed for MacOSX 
tcltk_parent = "../.."
tcltk_ver = "8.4"
tcltk_full_ver = "8.4.9"
tcl_dir = os.path.join(tcltk_parent,"tcl"+tcltk_full_ver)
tk_dir   = os.path.join(tcltk_parent,"tk"+tcltk_full_ver)

###### Directory containing  libtclstub8.4.a, libtkstub8.4.a: #####
if os.name=='posix':  # for POSIX OS
    if os.environ.has_key('MGL_EXTRALIBS'):
        tcl_lib_dir  = [os.environ['MGL_EXTRALIBS']]

    elif darwin_aqua:
        tcl_lib_dir = []
    else:
        tcl_lib_dir= ["/usr/lib/tcl8.4", "/usr/local/lib"]
    if mgluser: # for building in MGL:
        tcl_lib_dir = {"linux2":["/mgl/prog/i86Linux2/lib"], 
                       "irix6": ["/mgl/prog/sgi4DIRIX646/lib"],
                       "sunos5":["/mgl/prog/sun4SunOS5/lib"]}.get(platform, ["/usr/local/lib"])

        
elif os.name == 'nt':  # for Windows
    tcl_lib_dir = [os.path.join(sys.prefix,'tcl')]
    
###### Tk , tcl include directory #################################

if os.name =='posix': # for POSIX OS
    if os.environ.has_key('MGL_EXTRAINCLUDE'):
        tcl_include_dir = [os.environ['MGL_EXTRAINCLUDE']]
    elif sys.platform == 'darwin':
        tcl_include_dir = [os.path.join(tcl_dir,"generic"),
                           os.path.join(tk_dir,"generic")]
        if darwin_aqua:
            tcl_include_dir.append(os.path.join(tk_dir,"macosx"))
            tcl_include_dir.append(os.path.join(tk_dir,"xlib"))            
    else:
         tcl_include_dir = ["/usr/local/include", "/usr/include/tcl8.4"]
         # for building in MGL:
    if mgluser:
        if platform in ("sunos5", "linux2", "irix6"):
            tcl_include_dir = ["/mgl/prog/share/include"]
         
elif os.name == 'nt': #for Windows
	tcl_include_dir = ["./include/tcltk"] 
# End
###################################################################
###################################################################

tcl_libs = {'posix':[ "tclstub8.4", "tkstub8.4"],
            'nt':[ "tclstub84", "tkstub84"]}.get(os.name,[])

togl_incldirs=tcl_include_dir

if type(tcl_lib_dir) is types.StringType:
    togl_libdirs = [tcl_lib_dir]
else:
    togl_libdirs = tcl_lib_dir[:] # make a copy

if  sys.platform == 'darwin' and darwin_aqua:
    togl_libs = tcl_libs[:]

elif os.name == 'posix':
    togl_libs =['GLU',"GL",  "X11", "Xmu", "Xext",
                "Xt", "m", "dl"] + tcl_libs

elif os.name == 'nt':
       togl_libs = ['GLU32',"OPENGL32","gdi32","user32"] +tcl_libs

if platform == "sunos5":
    togl_incldirs.append("/usr/openwin/include")
    togl_libdirs.extend(["/usr/openwin/lib","/usr/X11/lib"] )

elif platform == "irix6":
    togl_libdirs.extend(["/usr/X11/lib"])
    
elif platform == "linux2":
    togl_incldirs.append("/usr/X11R6/include")
    togl_libdirs.extend(["/usr/X11R6/lib","/usr/X11/lib"])
    if '64' in machine_arch:
        togl_libdirs.extend(["/usr/X11R6/lib64","/usr/X11/lib64"])

elif platform == "darwin" and not darwin_aqua:
    togl_incldirs.append("/usr/X11R6/include")
    togl_libdirs.append("/usr/X11R6/lib")

elif platform == 'darwin' and darwin_aqua:
    togl_libdirs.extend(["/Library/Frameworks/Tcl.framework",
                         "/Library/Frameworks/Tk.framework"])

from distutils.core import setup, Extension
from distutils.cmd import Command
import os, sys
from distutils.file_util import copy_file
from distutils.util import get_platform
from os import path
from distutils.command.build import build
from distutils.command.build_ext import build_ext
from distutils.command.build_clib import build_clib
from distutils.command.install_data import install_data
from distutils.command.install import install
from distutils.command.sdist import sdist
from tools import tool

# Change the order of commands that are called by "build"
# so that 'byuld_py' comes after 'build_ext'. In this case
# 'build_py' will install python modules generated by
# 'build_ext'.

class modified_build(build):

    sub_commands = [('build_clib',    build.has_c_libraries),
                    ('build_ext',     build.has_ext_modules),
                    ('build_py',      build.has_pure_modules),
                    ('build_scripts', build.has_scripts),
                    ]
    
# Overvrite the build_ext command to generate opngltk/extent/xxx_i.h files
# (by parcing and filtering the opengl header files ). These files are
# included in the SWIG interface files - xxxlib.i

class MyExtension(Extension):
    def __init__(self, name, sources, **kw):
        headers = kw.get("headers")
        if headers :
            kw.pop('headers')
            self.headers = headers
        else:
            self.headers = None
        #print "sources:", sources
        #print "kw:", kw
        apply(Extension.__init__, (self, name, sources), kw)




class build_EXT(build_ext):
        
    def run(self):
        if self.extensions:
            for ext in self.extensions:
                if ext.headers:
                    # create '_i.h' files that are included in '.i' files
                    name = ext.headers.get('name')
                    dir = ext.headers.get('dir')
                    extern = ext.headers.get('extern')
                    n1, n2 = path.splitext(path.basename(name))
                    _i_name = path.join("opengltk","extent", n1+"_i"+n2)
                    argv = ['-c', 'headerfilter', '--output',
                            _i_name, '--input', path.join(dir, name),
                            '--exec', extern]
                    tool.run(argv)
                #print ext.export_symbols
        build_ext.run(self)
        if build_togl:
            if sys.platform == "darwin" and os.environ.has_key('MGL_EXTRALIBS'):
                # on MacOS ranlib command should be used for updating
                # the table of contents of archives copied to 'extralibs'
                # directory (libtclstub8.4.a, libtkstub8.4.a)
                if path.basename(tcl_lib_dir[0]) == "extralibs":
                    statlib1 = path.join(tcl_lib_dir[0], "libtclstub8.4.a")
                    statlib2 = path.join(tcl_lib_dir[0], "libtkstub8.4.a")
                    if path.isfile(statlib1) and path.isfile(statlib2):
                        self.spawn(["ranlib", "-s", statlib1])
                        self.spawn(["ranlib", "-s", statlib2])
            togl_build(self,self.include_dirs,
                       self.library_dirs,
                       togl_libs)
        #if platform in ["linux2", "win32"]:
        if platform in ["win32"]:
            tkdnd_build(self)
        return
    

# Overwrite the prune_file_list method of sdist to not
# remove automatically the CVS directories from the distribution.

class modified_sdist(sdist):
    def prune_file_list(self):
        build = self.get_finalized_command('build')
        base_dir = self.distribution.get_fullname()
        self.filelist.exclude_pattern(None, prefix=build.build_base)
        self.filelist.exclude_pattern(None, prefix=base_dir)           

# build togl
# inspired by PyOpenGL distribution
from distutils.dep_util import newer_group

Togl ='Togl'
def togl_build(self, include_dirs, lib_dirs, libs): # a simple function, not a class !!!
	# maybe we should use build as base, but we need an initialized
	# compiler object from build_ext
	# so let us get a instance of 'build' now
	build = self.get_finalized_command ('build')
	
	include_dirs = include_dirs + []
	lib_dirs = lib_dirs + []
	libs = libs + []
	
	extra_compile_args = [] # ['-Wall','-O', '-DPRINT_DEBUG=1']
	extra_link_args = [] # ['-s']
	# Name for the shared lib: (distutils will change it to Togl.{so|dll})
	output_name = 'togl'
	# where to put the built shared object (only for build process)
	output_dir = os.path.join('opengltk','OpenGL','Tk','Togl')
	export_symbols = ['Togl_Init']
	sources = [os.path.join(Togl,'togl.c')]
	
	include_dirs = togl_incldirs
	lib_dirs=togl_libdirs
        libs = togl_libs
	frameworks =[]
	macros = [('USE_TCL_STUBS',1),
                  ('USE_TK_STUBS',1),
		  ('USE_LOCAL_TK_H',1)]

	if sys.platform == 'win32':
		# VC++ 6.0 needs this, why togl doesn't use _WIN32?
		macros.append(('WIN32',1))
		#macros.append(('TOGL_WGL',1))
		#extra_compile_args.append('-mno-cygwin')
                
	if sys.platform  =='darwin' and darwin_aqua == 1:
	    macros.append(('TOGL_AGL',1 )) 
	    macros.append(('TOGL_USE_FONTS',1 ))
            macros.append(('HAVE_LIMITS_H',1))
            #extra_compile_args.append('-framework Tcl -framework Tk')
	    extra_link_args = ['-framework','AGL','-framework','OpenGL',
                               '-framework', 'ApplicationServices']

        elif sys.platform  =='darwin':
            (osname, host, release, version, machine) = os.uname()
            if release[0] == '9': #leopard
                extra_link_args=['-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib']

        elif os.name == 'posix':
            macros.append(('TOGL_X11',1))


	# rest of this function was inspired by build_ext.py , build_extensions() 
	
	if self.inplace:
		# ignore build-base -- put the compiled extension into
		# the source tree along with pure Python modules
		pass
	else:
		output_dir = os.path.join (build.build_lib, output_dir)
                
	if os.environ.has_key('CFLAGS'):
		extra_compile_args.extend(string.split(os.environ['CFLAGS']))

	# what is the name of the resulting file
	output_filename = self.compiler.shared_object_filename(
			basename=output_name,
			output_dir=output_dir)

	if not (self.force or newer_group(sources, output_filename, 'newer')):
		self.announce ('skipping "%s" (up-to-date)' % output_name)  
	else:
		self.announce ('building "%s"' % output_name)

		# compile source files        
		objects = self.compiler.compile (sources,
                                                 output_dir=self.build_temp,
                                                 macros=macros,
                                                 include_dirs=include_dirs,
                                                 debug=self.debug,
                                                 extra_postargs=extra_compile_args)
                
		# link all together
                if platform == "darwin":
                    #print "linker_so: ", self.compiler.linker_so
                    old_linker_so = self.compiler.linker_so[:]
                    linker_so = [old_linker_so[0]]
                    linker_so.append('-dynamiclib')
                    self.compiler.linker_so = linker_so
		self.compiler.link_shared_object (
				objects, 
				output_filename,
				'', # <= output_dir
				libraries=libs,
				library_dirs=lib_dirs,
				runtime_library_dirs=[],
				extra_postargs=extra_link_args,
				export_symbols=export_symbols, 
				debug=self.debug,
				build_temp=self.build_temp)
                if platform == "darwin":
                    self.compiler.linker_so = old_linker_so
                import shutil
                if platform == 'win32':
                    infile = os.path.join('opengltk','OpenGL','Tk',
                                        'Togl','pkgIndex.tcl.win32')
                    outputfile = os.path.join(output_dir,'pkgIndex.tcl')
                    
                    shutil.copyfile(infile,outputfile)
                else:
                    infile = os.path.join('opengltk','OpenGL','Tk',
                                        'Togl','pkgIndex.tcl.unix')
                    outputfile = os.path.join(output_dir,'pkgIndex.tcl')
                    shutil.copyfile(infile,outputfile)
        
# togl_build    

def tkdnd_build(self):
	# maybe we should use build as base, but we need an initialized
	# compiler object from build_ext
	# so let us get a instance of 'build' now
	build = self.get_finalized_command ('build')
	
	
	extra_compile_args = [] # ['-Wall','-O', '-DPRINT_DEBUG=1']
	extra_link_args = [] # ['-s']
	# Name for the shared lib: 
	output_name = 'libtkdnd20'
	# where to put the built shared object (only for build process)
	output_dir = os.path.join('opengltk','OpenGL','Tk','tkdnd2.0')
	export_symbols = []
	if platform == "linux2":
	    sources = [os.path.join("tkdnd2.0", "unix", "TkDND_XDND.c")]
            include_dirs = togl_incldirs + ["./include"]
	    libs = ['X11', 'tclstub8.4', 'tkstub8.4']	
	if platform == "win32":
	    sources = [os.path.join("tkdnd2.0", "win", "TkDND_OleDND.cpp")]
	    include_dirs = togl_incldirs + ["./include/tcltk" , "tkdnd2.0/win"]
	    libs = ['kernel32', 'user32', 'shell32', 'ole32', 'oleaut32', 'uuid'] + tcl_libs
	lib_dirs=togl_libdirs

	## macros = [("TKDND_PACKAGE", '"\\"tkdnd\\""'),  ("TKDND_VERSION", '"\\"2.0\\""'),
##                     ("USE_TCL_STUBS", None), ("USE_TK_STUBS", None), ("HAVE_UNISTD_H", None),
##                     ("HAVE_LIMITS_H", None) ]
        macros = [("USE_TCL_STUBS", None), ("USE_TK_STUBS", None), ("HAVE_UNISTD_H", None),
                    ("HAVE_LIMITS_H", None) ]

	#if sys.platform == 'win32':

                
	if self.inplace:
		# ignore build-base -- put the compiled extension into
		# the source tree along with pure Python modules
		pass
	else:
		output_dir = os.path.join (build.build_lib, output_dir)
                
	# what is the name of the resulting file
	output_filename = self.compiler.shared_object_filename(
			basename=output_name,
			output_dir=output_dir)

	if not (self.force or newer_group(sources, output_filename, 'newer')):
		self.announce ('skipping "%s" (up-to-date)' % output_name)  
	else:
		self.announce ('building "%s"' % output_name)

		# compile source files        
		objects = self.compiler.compile (sources,
                                                 output_dir=self.build_temp,
                                                 macros=macros,
                                                 include_dirs=include_dirs,
                                                 debug=self.debug,
                                                 )
                
		# link all together
	        print "build_temp:", self.build_temp
		self.compiler.link_shared_object (
				objects, 
				output_filename,
				'', # <= output_dir
				libraries=libs,
				library_dirs=lib_dirs,
				runtime_library_dirs=[],
				extra_postargs=extra_link_args,
				export_symbols=export_symbols, 
				debug=self.debug,
				build_temp=self.build_temp)

                import shutil
                infile = os.path.join('tkdnd2.0','pkgIndex.tcl')
                print "infile:", infile
                outputfile = os.path.join(output_dir,'pkgIndex.tcl')
                print "outputfile:", outputfile
                shutil.copyfile(infile,outputfile)
                indir = os.path.join('tkdnd2.0','library')
                print "indir:", indir
                outputdir = os.path.join(output_dir,'library')
                shutil.copytree(indir,outputdir)
                
# tkdnd_build    


# Overwrite the run method of the install_data to install additional files
# (other than python files or extensions) in the package instead
# of a particular data directory

class modified_install_data(install_data):

    def run(self):
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')
        return install_data.run(self)

pack_name = "opengltk"
platform =  sys.platform

# List of the python packages to be included in this distribution.
# sdist doesn't go recursively into subpackages so they need to be
# explicitaly listed.
# From these packages only the python modules will be taken
packages =  ["opengltk.OpenGL",
             "opengltk.wrapper",
             "opengltk.Tests"]

#if platform in ["linux2", "win32"]:
if platform in ["win32"]:	
             packages.append("TkinterDnD2")

# List of the python modules included in the distribution that are not
# part of the above packages.

py_modules = ["opengltk.__init__",
              "opengltk.ccallback",
              "opengltk.exception",
              "opengltk.glplus",
              "opengltk.util",
              "opengltk.extent.__init__",
              "opengltk.extent.utillib",
              "opengltk.OpenGL.Tk.__init__"]

# List of macros used by C compiler
macros = []
if platform == "linux2":
    macros.append(('GLMesa', None))
elif platform == "irix6":
    macros.append(('_LANGUAGE_C', None))
elif platform == "darwin":
    macros.append(("__APPLE__", None))

# List of include directories used by C compiler
import numpy
incldirs = ["./opengltk/extent", numpy.get_include()]
    
# gllib extension:
# where to find the headers file
if platform == 'darwin':
    h_dir= "/System/Library/Frameworks/OpenGL.framework/Headers"
    h_file = "gl.h"
    extra_link_args=["-framework", "OpenGL"]
    gl_libs=[]
else:
    h_dir = {#'nt':"C:\Program Files\Microsoft Visual Studio\VC98\Include",
	     'nt':gl_include,
             'posix': "/usr/include",
             }.get( os.name, "")
    h_file = os.path.join("GL","gl.h")
    extra_link_args=""

    gl_libs = {'posix': ['GL'], 'nt': ['OPENGL32']}.get( os.name, [])

filter = {#'posix':"x=removeextern( x);from opengltk.extent.glsigsub import fixsigs; x = fixsigs( x)",
    'posix':"x=removeextern( x);",
    'nt':"x = removeextern( x); from opengltk.extent.glsigsub import fixsigs; x = re.sub( r'WINGDIAPI|APIENTRY|CALLBACK|__cdecl|__stdcall', '', x);x = fixsigs( x)"}.get(os.name,"")

if platform == "linux2":
    filter = "x=removeextern( x);from opengltk.extent.glsigsub import fixsigs;x=re.sub(r'#ifndef\s*GL_MESA_trace', '/** #ifndef GL_MESA_trace', x); x=re.sub(r'#endif\s*\D*\s*GL_MESA_trace\s*\\**/', '#endif ***/ /* GL_MESA_trace */', x);x = re.sub( r'glProgramCallbackMESA|glGetProgramRegisterfvMESA|glBlendEquationSeparateATI', '', x); x = fixsigs( x);"

gllib_ext = MyExtension("extent._gllib",
                        [#path.join("opengltk", "extent", "pythonplus.c"),
                         path.join("opengltk", "extent", "gllib.i")], #sources
                        define_macros = macros,
                        include_dirs = incldirs,
                        #library_dirs = [],
                        libraries = gl_libs,
                        extra_link_args=extra_link_args,
                        headers = {
                                  'name': h_file,
                                  'dir':h_dir,
                                  'extern':filter  
                                  }
                        )

# Describe extensions ( installed in opengltk/extent):

# _glextlib.so:
if platform == 'darwin':
    extra_link_args = ["-framework", "OpenGL"]
    libraries = ''
    macros = []
elif platform == 'win32':
    extra_link_args = ''
    libraries = gl_libs
    macros = [('WIN32', None)]
else:
    extra_link_args = ''
    libraries = ''
    macros = []

glextlib_ext = MyExtension("extent._glextlib",
                         [path.join('opengltk', 'extent', 'glextlib.i')],
                         include_dirs = ['./opengltk/extent'],
                         libraries = libraries,
                         extra_link_args=extra_link_args,
                         headers = {
                             'name': 'glext_mgltools.h',
                             'dir': './opengltk/extent'
                             }
                         )


# _glulib.so:
# where to find the headers file
if platform == 'darwin':
    h_dir= "/System/Library/Frameworks/OpenGL.framework/Headers"
    h_file = "glu.h"
    extra_link_args=["-framework", "OpenGL"]
    glu_libs = []
else:
    h_dir = {#'nt':"C:\Program Files\Microsoft Visual Studio\VC98\Include",
             'nt':gl_include,
             'posix': "/usr/include",
             }.get( os.name, "")
    h_file = os.path.join("GL","glu.h")
    extra_link_args=""
    glu_libs = {'posix': ['GLU'], 'nt': ['GLU32']}.get( os.name, []) + gl_libs
filter = "x = removeextern( x); x = re.sub( r'WINGDIAPI|APIENTRY|CALLBACK|__cdecl|__stdcall', '', x);x=re.sub(r'void\s*gluQuadricCallback\s*\(\s*GLUquadric\s*\*\w*,\s*GLenum\s*\w*,\s*\w*\s*\(\s*\*\s*\w*\)\s*\(\s*\)\s*\);','',x);x=re.sub(r'void\s*gluTessCallback\s*\(\s*GLUtesselator\s*\*\w*,\s*GLenum\s*\w*,\s*\w*\s*\(\s*\*\s*\w*\)\s*\(\s*\)\s*\);','',x);x=re.sub(r'void\s*gluNurbsCallback\s*\(\s*GLUnurbs\s*\*\w*,\s*GLenum\s*\w*,\s*\w*\s*\(\s*\*\s*\w*\)\s*\(\s*\)\s*\);','',x);from opengltk.extent.glsigsub import fixsigs; x = fixsigs( x)" 
glulib_ext = MyExtension("extent._glulib",
                         [path.join("opengltk", "extent", "glulib.i"),
                          #path.join("opengltk", "extent", "pythonplus.c")
                          ], #sources
                         define_macros = macros,
                         include_dirs = incldirs,
                         #library_dirs = [],
                         libraries = glu_libs,
                         extra_link_args = extra_link_args,
                         headers = {
                             'name': h_file,
                             'dir': h_dir,
                             'extern':filter
                             }
                         )


# _utillib.so:
if platform == 'darwin':
    util_libs = []
else:
    util_libs = glu_libs
    if platform in ["sunos5", "irix6"]:
        util_libs.extend(['Xi','Xmu'])
util_incldirs = incldirs[:]
util_comp_args = [] # list of additional compiler flags
util_link_args =  extra_link_args
utillib_ext = MyExtension("extent._utillib",
                          [path.join("opengltk", "extent", "utillib.i"),
                           path.join("opengltk", "extent", "pythonplus.c")], #sources
                          define_macros = macros,
                          include_dirs = util_incldirs,
                          library_dirs = gl_lib_dir,
                          libraries = util_libs,
                          extra_compile_args = util_comp_args,
                          extra_link_args=util_link_args,
                          )

# _glxlib.so:

glx_incldirs = incldirs[:]
if platform == 'darwin':
    h_dir= "/usr/X11R6/include/"
    extra_link_args=[]
    gl_libs=['GL']
    (osname, host, release, version, machine) = os.uname()
    if release[0] == '9': #leopard
        extra_link_args=['-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib']
    
else:
    h_dir = {#'nt':"C:\Program Files\Microsoft Visual Studio\VC98\Include",
	     'nt':gl_include,
             'posix': "/usr/include",
             }.get( os.name, "")
    extra_link_args=""
#incldirs.append(h_dir)
glx_incldirs.append(h_dir)
glx_libdirs =  {'linux2': ['/usr/X11R6/lib'],'darwin': ['/usr/X11R6/lib']}.get( platform, [])
if '64' in machine_arch:
    glx_libdirs.insert(0,'/usr/X11R6/lib64')
glxlib_ext = MyExtension("extent._glxlib",
                          [path.join("opengltk", "extent", "glxlib.i"),
                         #path.join("opengltk", "extent", "pythonplus.c")
                           ], #sources
                          define_macros = macros,
                          include_dirs = glx_incldirs,
                          library_dirs = glx_libdirs,
                          libraries = ['X11'] + gl_libs,
                          headers = {
                             'name': path.join("GL","glx.h"),
                             'dir': h_dir,
                          'extern': "x = removeextern( x, 'extern')" 
                             },
                         extra_link_args = extra_link_args
                          )


# setup does the job:
if platform == "win32":
    ext_modules = [gllib_ext, glextlib_ext, glulib_ext, utillib_ext]
else:
    ext_modules = [gllib_ext, glextlib_ext, glulib_ext, utillib_ext, glxlib_ext]
try:   
    from version import VERSION
except:
    VERSION = "1.0"
dist = setup(name = "opengltk",
             version = VERSION,
             packages = packages,
             py_modules = py_modules,
             ext_package = "opengltk",
             cmdclass = {'build':modified_build,
                         'build_ext': build_EXT,
                         'install_data':modified_install_data,
                         'sdist': modified_sdist},
             ext_modules = ext_modules,
             #data_files=[("opengltk/OpenGL/Tk/Togl",
             #            [data_file ,])]
             )

