"""GL extensions used by DejaVu (VBOs, framebuffer objects, GLSL shaders),
formerly the SWIG module _glextlib.

Only extension entry points are exported: DejaVu does
``from opengltk.extent._glextlib import *`` after importing opengltk.OpenGL.GL,
so re-exporting core GL here would replace the opengltk-style wrappers.
"""
import opengltk  # noqa: F401
from OpenGL.GL.ARB.vertex_buffer_object import *  # noqa: F401,F403
from OpenGL.GL.EXT.framebuffer_object import *  # noqa: F401,F403
from OpenGL.GL.EXT.packed_depth_stencil import *  # noqa: F401,F403
from OpenGL import GL as _GL
from OpenGL.GL import (glCreateShader, glCompileShader,  # noqa: F401
                       glCreateProgram, glAttachShader, glLinkProgram,
                       glValidateProgram, glUseProgram, glUniform1i,
                       glUniform1f, glUniform4f, GL_VERTEX_SHADER,
                       GL_FRAGMENT_SHADER, GL_COMPILE_STATUS, GL_LINK_STATUS,
                       GL_VALIDATE_STATUS, GL_ELEMENT_ARRAY_BUFFER,
                       GL_ARRAY_BUFFER, GL_STATIC_DRAW, GL_DYNAMIC_DRAW)
from OpenGL.GL.ARB import vertex_buffer_object as _vbo
from OpenGL.GL.EXT import framebuffer_object as _fbo
import numpy as _np

# opengltk's SWIG wrappers took C-style arguments and *returned* output
# parameters; DejaVu calls them that way.


def glShaderSource(shader, count, string, length=None):
    if isinstance(string, (str, bytes)):
        string = [string]
    _GL.glShaderSource(shader, list(string)[:count])


def glGetShaderiv(shader, pname, params=None):
    return int(_np.ravel(_GL.glGetShaderiv(shader, pname))[0])


def glGetProgramiv(program, pname, params=None):
    return int(_np.ravel(_GL.glGetProgramiv(program, pname))[0])


def _log(s):
    s = s.decode("latin-1") if isinstance(s, bytes) else (s or "")
    return len(s), s


def glGetShaderInfoLog(shader, maxLength=None, length=None, infoLog=None):
    return _log(_GL.glGetShaderInfoLog(shader))


def glGetProgramInfoLog(program, maxLength=None, length=None, infoLog=None):
    return _log(_GL.glGetProgramInfoLog(program))


def glGetUniformLocation(program, name):
    return int(_GL.glGetUniformLocation(program, name))


def glGenBuffersARB(n, buffers=None):
    return _np.atleast_1d(_np.asarray(_vbo.glGenBuffersARB(n), _np.uint32))


def glDeleteBuffersARB(n, buffers):
    _vbo.glDeleteBuffersARB(n, _np.ascontiguousarray(buffers, _np.uint32))


def glGenFramebuffersEXT(n, framebuffers=None):
    r = _np.atleast_1d(_np.asarray(_fbo.glGenFramebuffersEXT(n), _np.uint32))
    return int(r[0]) if n == 1 else r


def glGenRenderbuffersEXT(n, renderbuffers=None):
    r = _np.atleast_1d(_np.asarray(_fbo.glGenRenderbuffersEXT(n), _np.uint32))
    return int(r[0]) if n == 1 else r


def glDeleteFramebuffersEXT(n, framebuffers):
    _fbo.glDeleteFramebuffersEXT(n, _np.ascontiguousarray(_np.atleast_1d(framebuffers), _np.uint32))


from opengltk import _pyogl as _pyogl_
_pyogl_.plain_constants(globals())
