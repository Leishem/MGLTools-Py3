#
#
# The contents of this file are subject to the Mozilla Public
# License Version 1.1 (the "License"); you may not use this file
# except in compliance with the License. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
# 
# Software distributed under the License is distributed on an "AS
# IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
# implied. See the License for the specific language governing
# rights and limitations under the License.
# The Original Code is "Java-Python Extension libplus (JPE-libplus)".
# 
# The Initial Developer of the Original Code is Frederic Bruno Giacometti.
# Portions created by Frederic Bruno Giacometti are
# Copyright (C) 2001-2002 Frederic Bruno Giacometti. All Rights Reserved.
# 
# Contributor(s): frederic.giacometti@arakne.com
# 
# Acknowledgments:
# Particular gratitude is expressed to the following parties for their
# contributing support to the development of JPE-libplus:
#     - The Molecular Graphics Laboratory (MGL)
#       at The Scripps Research Institute (TSRI), in La Jolla, CA, USA;
#       and in particular to Michel Sanner and Arthur Olson.
#

'''wrapper for command line tools
ex:
python tool.py locate --name PYTHONPATH py.*

or:
python -c "from tool import run; run()" command args


A typical configuration consists in defining:

alias ptool='python -c "from too import run; run()"'

The command line syntax is simialr to cvs ...
'''

from __future__ import nested_scopes

import re, os, sys, copy

from os import path
from operator import add
from warnings import warn

class Option:
    def __init__( self, description, long, short=None, name=None, fun=None):
        self.description = description
        self.long = self.longopt = long
        assert name is None or name
        self.name = name and name or long
        assert short is None or len( short) == 1 # single char string...
        assert short is None or short
        self.short = self.shortopt = short
        self.fun = fun
        self.value = self.default = 0
    def __str__( self):
        return '%s =<%s>' % (self.helpmsg(), self.value)
    def helpmsg( self):
        result = '  --' + self.long
        if self.short:
            result += '|-' + self.short
        if hasattr( self, 'default'):
            result += ' %s' % self.default
        result += ': ' + self.description
        return result
    def set( self, value=None):
        if self.fun:
            self.fun()
        self.value = 1

class ValueOption( Option):
    def __init__( self, description, long, default=None, short=None,
                  fun=None, name=None):
        Option.__init__( self, description, long, short, name)
        self.shortopt = self.short and self.short + ':' or self.short
        self.longopt = self.long + '='
        self.value = self.default = default
        self.fun = fun
    def set( self, value):
        if self.fun is not None:
            value = self.fun( value)
        self.value = value

class MultipleValueOption( ValueOption):
    def __init__( self, description, long, default=None, short=None,
                  fun=None, name=None):
        ValueOption.__init__( self, description, long,
                              default and default or [],
                              short, fun, name)
        self.defaultvalue = 1
    def set( self, value):
        if self.defaultvalue:
            self.value = [ value]
            self.defaultvalue = 0
        else:
            self.value.append( value)

class CommandLineError( StandardError):
    def __str__( self):
        return `map( str, self.args)`

class Argv:
    def __init__( self, description, args, optdefs=()):
        optdefs = [ Option( 'print help', 'help')]\
                  + map( copy.copy, optdefs)
        self.description = str( description)
        options = {}
        optdic = {}
        for opt in optdefs:
            options[ opt.name] = opt
            if opt.short:
                optdic[ '-' + opt.short] = opt
            optdic[ '--' + opt.long] = opt
        self.options = options
        self.optdic = optdic
        optdefs.sort( lambda x, y: cmp( x.long, y.long))
        shortopts = reduce( add, [opt.shortopt
                                  for opt in optdefs if opt.short], '')
        longopts = [opt.longopt for opt in optdefs if opt.long]
        from getopt import getopt, GetoptError
        try:
            opts, self.args = getopt( args[ 1:], shortopts, longopts)
        except GetoptError, excval:
            raise
        for name, value in opts:
            self.optdic[ name].set( value)
        self.optdefs = optdefs
    def __getitem__( self, key):
        return self.options[ key].value
    def __setitem__( self, key, value):
        self.options[ key].value = value
    def __str__( self):
        return reduce( add, ['\n  %s' % opt for opt in self.optdefs],
                       self.description)
    def has_key( self, key):
        return self.options.has_key( key)
    def helpmsg( self):
        return reduce( add, [opt.helpmsg() + '\n' for opt in self.optdefs],
                       self.description + '\n')


def substitute( args, options={}):
    '''string substitution using regular expressions
    '''
    import re
    argv = Argv( 'string replacement using regular expressions', args,
                 (Option( 'no change', 'nochange', short='n'),
                  Option( 'process directories recursively',
                          'recursive', short='r'),
                  Option( 'process as binary files', 'binary', short='b'),
                  MultipleValueOption( 'directories to visit',
                                       'dir', default=['.']),
                  ValueOption( 'match string', 'match', short='m',
                               fun=lambda x: re.compile( x, re.MULTILINE),
                               default=None),
                  ValueOption( 'substitution string', 'sub', short='s',
                               default=None),
                  Option( 'convert DOS to text', 'dos2text'),
                  Option( 'convert text to DOS', 'dosfromtext'),
                  ))
    if argv[ 'help']:
        return [argv.helpmsg()]
    assert not argv[ 'dos2text'] or not argv[ 'dosfromtext']
    if argv[ 'dos2text']:            
        argv[ 'binary'] = 1
        subpat = re.compile( r'\r\n')
        substr = r'\n'
    elif argv[ 'dosfromtext']:
        argv[ 'binary'] = 1
        subpat = re.compile( r'([^\r])\n')
        substr = r'\1\r\n'
    else:
        subpat = argv[ 'match']
        substr = argv[ 'sub']
    assert subpat is not None
    assert substr is not None
        
    binaryflag = argv[ 'binary'] and 'b' or ''
    def visit( (pats, modified), dirname, names):
        if not argv[ 'recursive']:
            idx = 0
            while idx < len( names):
                if not path.isfile( path.join( dirname, names[ idx])):
                    del names[ idx]
                idx += 1
        fnames = [name
                  for name in names
                  for pat in pats
                  if (pat.match( name)
                      and path.isfile( path.join( dirname, name)))]
        dict = {}
        for name in fnames: # filter duplicates
            dict[ name] = None
        files = [path.join( dirname, name) for name in dict.keys()]
        for file in files:
            txt = open( file, 'r' + binaryflag).read()
            if subpat.search( txt):
                nsub = 0
                if not argv[ 'nochange']:
                    newtxt, nsub = subpat.subn( substr, txt)
                    open( file, 'w' + binaryflag).write( newtxt)
                modified.append( (file, nsub))
                
    import re
    pats = [re.compile( pat + '$')
            for pat in (argv.args and argv.args or [ '.*'])]
    modified = []
    for dir in argv[ 'dir']:
        path.walk( dir, visit, (pats, modified))
    return modified


def locate( args, options={}):
    '''locate from a path variable
    '''
    argv = Argv( 'locate matching files from path', args,
                 (ValueOption( 'path env variable name', 'path', 'PATH'),))
    if argv[ 'help']:
        return [argv.helpmsg()]
    from os import path
    compflags = 0
    dirlist = os.environ[ argv[ 'path']].split( os.pathsep)
    if sys.platform in ('win32',):
        compflags |= re.IGNORECASE
        if 'PATH' == argv[ 'path']:
            dirlist = [ '.'] + dirlist
    pats = [ re.compile( x + '$', compflags) for x in argv.args]
    return reduce( add, [ [ path.join( dir, name)
                            for name in os.listdir( dir)
                            for pat in pats
                            if pat.match( name)]
                          for dir in dirlist
                          if path.isdir( dir)])


def install( args, options={}):
    '''file installation
    '''
    argv = Argv( 'file installer', args, (
        ))
    if argv[ 'help']:
        return [argv.helpmsg()]
    import fileplus
    apply( fileplus.installf, args[ 1:])


def rename( args, options={}):
    '''renaming file
    '''
    argv = Argv( 'rename file', args, ())
    if argv[ 'help']:
        return [argv.helpmsg()]
    import fileplus
    old, new = args[ 1:]
    if path.exists( new):
        os.remove( new)
    os.renames( old, new)


def classtree( args, options={}):
    argv = Argv( 'return class hierarchy - args is a list of modules',
                 args, ())
    if argv[ 'help']:
        return [argv.helpmsg()]

    import types
    ktree = {}
    newtypes = []
    for modname in args[ 1:]:
        __import__( modname)
        mod = sys.modules[ modname]
        classes = [x for x in vars( mod).values()
                   if isinstance( x, types.ClassType)]
        newtypes.extend( ['%s.%s' % (mod.__name__, x)
                          for x in vars( mod).values()
                          if isinstance( x, types.TypeType)])
        
        def registerclasstree( klass, tree):
            if not tree.has_key( klass):
                tree[ klass] = []
            for parent in klass.__bases__:
                registerclasstree( parent, tree)
                lst = tree[ parent]
                if klass not in lst:
                    lst.append( klass)

        for klass in classes:
            registerclasstree( klass, ktree)
            
    rootclasses = [x for x in ktree.keys() if not x.__bases__]
    rootclasses.sort( lambda x, y: cmp( str( x), str( y)))

    def subtree( klass):
        return (klass, [subtree( x) for x in ktree[ klass]])

    def subtree2str( stree):
        stree[ 1].sort( lambda x, y: cmp( str( x), str( y)))
        return '\n    '.join( [str( stree[ 0])]
                              + [re.sub( r'\n', r'\n    ', subtree2str( x))
                                 for x in stree[ 1]])

    return [subtree2str( subtree( x)) for x in rootclasses] + newtypes


def envpy( args, options={}):
    '''run python with environment vars
    '''
    argv = Argv( 'env=defs command.py args', args, ())
    if argv[ 'help']:
        return [argv.helpmsg()]
    import re, os
    args.pop( 0)
    while re.match( '\w+=', args[ 0]):
        key, val = args.pop( 0).split( '=', 1)
        os.environ[ key] = val
    sys.argv = args
    execfile( args[ 0], {'__name__': '__main__'})


def env( args, options={}):
    '''emulation of the posix env command
    '''
    argv = Argv( 'env=defs command.py args', args, ())
    if argv[ 'help']:
        return [argv.helpmsg()]
    import re, os
    envdic = {}
    for name, val in os.environ.items():
        envdic[ name] = val
    args = args[:] # duplicate - avoid side effect
    args.pop( 0)
    while re.match( '\w+=', args[ 0]):
        key, val = args.pop( 0).split( '=', 1)
        envdic[ key] = val
    #envdic.update( os.environ)
    #os.execvpe( args[ 0], args, envdic) # actually spawn process on win32???
    #sys.exit( os.spawnvpe( os.P_WAIT, args[ 0], args, envdic))
    ## waiting spawnpve introduction in python 2.2
    cmd = args[ 0]
    from os import path
    if not path.isabs( cmd):
        import fileplus
        cmd = fileplus.findpath( cmd)
    try:
        sys.exit( os.spawnve( os.P_WAIT, cmd, args, envdic))
    except OSError:
        warn( `(args[ 0], args, envdic)`)
        raise

def filestdin( args, options={}):
    '''process files from stdin
    '''
    argv = Argv( 'no args', args,
                  (ValueOption( 'match string', 'match', short='m',
                               fun=lambda x: re.compile( x, re.MULTILINE),
                               default=None),
                  Option( 'process as binary files', 'binary', short='b'),
                  ValueOption( 'substitution string', 'sub', short='s',
                               default=None),
                  ValueOption( 'substitution string from file',
                               'subfile', short='f', default=None),
                  Option( 'convert DOS to text', 'dos2text'),
                  Option( 'convert text to DOS', 'dosfromtext'),
                  ))
    if argv[ 'help']:
        return [argv.helpmsg()]
    assert not argv[ 'dos2text'] or not argv[ 'dosfromtext']
    if argv[ 'dos2text']:            
        argv[ 'binary'] = 1
        subpat = re.compile( r'\r\n')
        substr = r'\n'
    elif argv[ 'dosfromtext']:
        argv[ 'binary'] = 1
        subpat = re.compile( r'([^\r])\n')
        substr = r'\1\r\n'
    else:
        assert not argv[ 'sub'] or not argv[ 'subfile']
        substr = None
        if argv[ 'sub']:
            substr = eval( '"%s"' % argv[ 'sub'])
        elif argv[ 'subfile']:
            substr = open( argv[ 'subfile']).read()
        subpat = argv[ 'match']
    assert subpat is not None
        
    binaryflag = argv[ 'binary'] and 'b' or ''
    for file in [x[ :-1] for x in sys.stdin.xreadlines()]:
        txt = open( file, 'r' + binaryflag).read()
        res = re.search( subpat, txt)
        if res:
            print '%s:\t<%s>' % (file, res.group())
            if substr is not None:
                txt = re.sub( subpat, substr, txt)
                open( file, 'w' + binaryflag).write( txt)


def find( args, options={}):
    '''process files from stdin
    '''
    import os, re
    from os import path
    argv = Argv( 'directories', args, (
        ValueOption( 'match function', 'match', short='m',
                     fun=lambda y: lambda x: eval( y), default=lambda x: 1),
        ValueOption( 'exec function', 'exec', short='e',
                     fun=lambda y: lambda x: eval( y), default=lambda x: None),
        ))
    if argv[ 'help']:
        return [argv.helpmsg()]

    import operator, fileplus

    matchfun = argv[ 'match']
    execfun = argv[ 'exec']
    for file in reduce( operator.add,
                       [fileplus.find( x, filefun = matchfun, fullpath = 1)
                        for x in argv.args],
                       []):
        print '<%s> <%s>' % (file, execfun( file))

def __runfunc( inputfile, code):
    # cannot be nested because of exec (-> SyntaxError on Python 2.1...)
    import sys, re, os
    import preprocess
    # Python 2.2: SyntaxWarning: import * only allowed at module levelimport *
    locs = locals()
    for item in preprocess.__all__:
        locs[ item] = getattr( preprocess, item)
    x = open( inputfile).read()
    exec code
    return x


def headerfilter( args, options={}):
    '''process files from stdin
    '''
    argv = Argv( 'no args', args,
                  (ValueOption( 'output file', 'output', short = 'o',
                                default = None),
                   MultipleValueOption( 'input file', 'input', short = 'i',
                                        default = None),
                   MultipleValueOption( 'exec code', 'exec', short = 'e',
                                        default = 'pass'),
                   ))
    if argv[ 'help']:
        return [argv.helpmsg()]

    assert len( argv[ 'input']) == len( argv[ 'exec']),\
           (len( argv[ 'input']), len( argv[ 'exec']))
    
    headerout = ''.join( map( __runfunc, argv[ 'input'], argv[ 'exec']))
    
    (argv[ 'output'] and open( argv[ 'output'], 'w') or sys.stdout
     ).write( headerout)

def __runfilterfunc( inputfile, execs):
    # cannot be nested because of exec (-> SyntaxError on Python 2.1...)
    exevars = {'x': open( inputfile).read()}
    for code in execs:
        try:
            exec code in exevars
        except:
            warn( code)
            raise
    return exevars[ 'x']

def filter( args, options={}):
    '''apply transform on file or datastream
    '''
    argv = Argv( 'no args', args,
                  (ValueOption( 'output file', 'output', short = 'o',
                                default = None),
                   ValueOption( 'input file', 'input', short = 'i',
                                default = None),
                   MultipleValueOption( 'exec code', 'exec', short = 'e'),
                   ))
    if argv[ 'help']:
        return [argv.helpmsg()]

    outputdata = __runfilterfunc( argv[ 'input'], argv[ 'exec'])
    
    (argv[ 'output'] and open( argv[ 'output'], 'w') or sys.stdout
     ).write( outputdata)

def genimport( args, options={}):
    '''generate lib import file (see ctool.wrapper.importutil.importtargets)
    '''
    argv = Argv( 'no args', args,
                  (ValueOption( 'lib name', 'name', short = 'n'),
                   ValueOption( 'python executable', 'python', short = 'p'),
                   MultipleValueOption( 'prefix to remove', 'prefix',
                                        short = 'p'),
                   ))
    if argv[ 'help']:
        return [argv.helpmsg()]


    def makemap( names, prefixes):
        def prefixstrip( name, lprefixes):
            import string
            for prefix in lprefixes:
                sz = len( prefix)
                if name[ :sz] == prefix\
                   and (prefix[ -1] == '_' or name[ sz] in string.uppercase)\
                   and name[ sz] in (string.letters + '_'):
                    return name[ sz:]
            else:
                return name
        glmap = [ (prefixstrip( x, prefixes), x) for x in names
                  if x[ :2] != '__'] # and x not in ('cvar',)]
        glmap.sort()
        return glmap

    extlibname = argv[ 'name'] + 'lib'

    pythonexe = argv[ 'python']
    if pythonexe:
        import osplus
        symbols = eval( osplus.pcommand( '%s -c "import %s; print dir( %s)"'
                                         % (pythonexe,
                                            extlibname, extlibname)))
    else:
        symbols = dir( __import__( extlibname))
    
    result = '\n'.join(
        ['"""automatically generated"""', '\nimport %s\n' % extlibname]
        + ['%s = %s.%s' % (x[ 0], extlibname, x[ 1])
           for x in makemap( symbols, argv[ 'prefix'])]
        ) + '\n'
    
    open( '%simport.py' % argv[ 'name'], 'w').write( result)

def bufferedexec( args, options={}):
    '''execute Python code with totally buffered output
    '''
    argv = Argv( 'code', args,
                 (ValueOption( 'output', 'output', short = 'o'),
                  ))
    if argv[ 'help']:
        return [argv.helpmsg()]

    import cStringIO
    prevout = sys.stdout
    sys.stdout = strio = cStringIO.StringIO()
    globs = {}
    for arg in argv.args:
        exec arg in globs
    sys.stdout = prevout
    open( argv[ 'output'], 'w').write( strio.getvalue())
    strio.close()


def touch( args, options={}):
    '''emulation of touch
    '''
    argv = Argv( 'files to touch', args)
    if argv[ 'help']:
        return [argv.helpmsg()]

    for fname in argv.args:
        if path.exists( fname):
            os.utime( fname, None)
        else:
            open( fname, 'w').close()
        

def test( args, options={}):
    '''test routine
    '''
    txt = open( args[ 1], 'rb').read()
    #return [len( txt)]
    return [repr( txt)]

def tool( args, toolmap, options=()):
    '''tools wrapper
    '''
    argv = Argv( 'tool wrapper', args, options)
    if argv[ 'help'] or not argv.args:
        tooldesc = [ '%s: %s' % (x[ 0],
                                 x[ 1].__doc__
                                     and x[ 1].__doc__.split( '\n')[ 0] or '')
                     for x in toolmap.items() if callable( x[ 1])]
        tooldesc.sort()
        return [argv.helpmsg()] + tooldesc
    cmdname = argv.args[ 0]
    if not toolmap.has_key( cmdname):
        cmdlist = toolmap.keys()
        cmdlist.sort()
        raise CommandLineError( '<%s> not in %s' % (cmdname, cmdlist), argv)
    result = toolmap[ cmdname]( argv.args, argv.options)
    if result is None:
        result = []
    return result

def run( argv=None):
    if argv is None:
        import sys
        argv = sys.argv
    for line in tool( argv, globals()):
        print line
