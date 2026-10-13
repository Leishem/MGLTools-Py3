#ifndef __glext_mgltools_h_
#define __glext_mgltools_h_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef extern
#define extern extern
#endif

/********************************* gl.h definitions *******************/
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
/**********************************************************************/

/********************************* glext.h definitions *******************/
#include <stddef.h>
#ifndef GL_VERSION_2_0
/* GL type for program/shader text */
typedef char GLchar;			/* native character */
#endif

#ifndef GL_VERSION_1_5
/* GL types for handling large vertex buffer objects */
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
#endif

#ifndef GL_ARB_vertex_buffer_object
/* GL types for handling large vertex buffer objects */
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;
#endif

#ifndef GL_ARB_shader_objects
/* GL types for handling shader object handles and program/shader text */
typedef char GLcharARB;		/* native character */
typedef unsigned int GLhandleARB;	/* shader object handle */
#endif

/* GL types for "half" precision (s10e5) float data in host memory */
#ifndef GL_ARB_half_float_pixel
typedef unsigned short GLhalfARB;
#endif

#ifndef GL_NV_half_float
typedef unsigned short GLhalfNV;
#endif

#ifndef GL_EXT_timer_query
typedef signed long long GLint64EXT;
typedef unsigned long long GLuint64EXT;
#endif
/**********************************************************************/


/************ add here the needed functionalities from glext.h **********/

/****************** shaders *********************************************/
#define GL_COMPILE_STATUS                 0x8B81
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83
#define GL_VERTEX_SHADER                  0x8B31

extern GLuint APIENTRY glCreateShader (GLenum);
extern void APIENTRY glAttachShader (GLuint, GLuint);
extern void APIENTRY glCompileShader (GLuint);
extern GLuint APIENTRY glCreateProgram (void);
extern void APIENTRY glGetProgramiv (GLuint, GLenum, GLint * aInt);
extern void APIENTRY glGetProgramInfoLog (GLuint, GLsizei, GLsizei *, GLchar *);
extern void APIENTRY glGetShaderiv (GLuint, GLenum, GLint * aInt);
extern void APIENTRY glGetShaderInfoLog (GLuint, GLsizei, GLsizei *, GLchar *);
extern GLint APIENTRY glGetUniformLocation (GLuint, const GLchar *);
extern void APIENTRY glLinkProgram (GLuint);
extern void APIENTRY glShaderSource (GLuint, GLsizei, const GLchar * *, const GLint *);
extern void APIENTRY glUniform1i (GLint, GLint);
extern void APIENTRY glUniform1f (GLint, GLfloat);
extern void APIENTRY glUniform4f (GLint, GLfloat, GLfloat, GLfloat, GLfloat);
extern void APIENTRY glUseProgram (GLuint);
extern void APIENTRY glValidateProgram (GLuint);
/**********************************************************************/

/****************** frame buffer object *******************************/
#define GL_COLOR_ATTACHMENT0_EXT		  0x8CE0
#define GL_DEPTH_ATTACHMENT_EXT           0x8D00
#define GL_DEPTH_STENCIL_EXT              0x84F9
#define GL_FRAMEBUFFER_EXT				  0x8D40
#define GL_STENCIL_ATTACHMENT_EXT         0x8D20
#define GL_FRAMEBUFFER_COMPLETE_EXT       0x8CD5

extern void APIENTRY glBindFramebufferEXT (GLenum, GLuint);
extern GLenum APIENTRY glCheckFramebufferStatusEXT (GLenum);
extern void APIENTRY glFramebufferTexture2DEXT (GLenum, GLenum, GLenum, GLuint, GLint);
extern void APIENTRY glGenFramebuffersEXT (GLsizei, GLuint *);
/**********************************************************************/

/****************** duplicated gl calls *******************************/
//extern void APIENTRY glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat,
		                           GLsizei width, GLsizei height, GLint border,
		                           GLenum format, GLenum type, const GLvoid *pixels);
/**********************************************************************/

/**********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* __glext_mgltools_h_ */

