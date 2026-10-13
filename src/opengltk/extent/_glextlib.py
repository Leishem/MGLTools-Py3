"""GL extensions used by DejaVu (VBOs, framebuffer objects, GLSL shaders),
formerly the SWIG module _glextlib."""
from opengltk.extent._gllib import *  # noqa: F401,F403
from OpenGL.GL.ARB.vertex_buffer_object import *  # noqa: F401,F403
from OpenGL.GL.EXT.framebuffer_object import *  # noqa: F401,F403
from OpenGL.GL.EXT.packed_depth_stencil import *  # noqa: F401,F403
from OpenGL.GL import (glCreateShader, glShaderSource, glCompileShader,  # noqa: F401
                       glGetShaderiv, glGetShaderInfoLog, glCreateProgram,
                       glAttachShader, glLinkProgram, glValidateProgram,
                       glGetProgramiv, glGetProgramInfoLog, glUseProgram,
                       glGetUniformLocation, glUniform1i, glUniform1f,
                       glUniform4f, GL_VERTEX_SHADER, GL_FRAGMENT_SHADER,
                       GL_COMPILE_STATUS, GL_LINK_STATUS, GL_VALIDATE_STATUS,
                       GL_ELEMENT_ARRAY_BUFFER)
