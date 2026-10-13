%module glextlib 
%feature("python:cdefaultargs");
/*
 * copyright_notice
 */

%{
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef _MSC_VER
#include <windows.h>
#endif

#include "glext_mgltools.h"

#ifdef WIN32
extern GLuint APIENTRY glCreateShader (GLenum a)
{
	// Get a pointer to the extension function.
	typedef GLuint ( WINAPI * functionPointer ) ( GLenum ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glCreateShader" ) ;
	return ( * pglFunction )( a ) ;
}

extern void APIENTRY glAttachShader (GLuint a, GLuint b)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint, GLuint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glAttachShader" ) ;
	( * pglFunction )( a , b ) ;
}

extern void APIENTRY glCompileShader (GLuint a)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glCompileShader" ) ;
	( * pglFunction )( a ) ;
}

extern GLuint APIENTRY glCreateProgram ( void )
{
	// Get a pointer to the extension function.
	typedef GLuint ( WINAPI * functionPointer ) (void) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glCreateProgram" ) ;
	return ( * pglFunction )( ) ;
}

extern void APIENTRY glGetProgramiv ( GLuint a , GLenum b , GLint * aInt )
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint, GLenum, GLint * aInt) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glGetProgramiv" ) ;
	( * pglFunction )( a, b, aInt ) ;
}

extern void APIENTRY glGetProgramInfoLog (GLuint a, GLsizei b, GLsizei * c, GLchar * d)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint , GLsizei , GLsizei * , GLchar * ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glGetProgramInfoLog" ) ;
	( * pglFunction )( a, b, c, d ) ;
}

extern void APIENTRY glGetShaderiv (GLuint a, GLenum b, GLint * aInt)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint, GLenum, GLint * aInt) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glGetShaderiv" ) ;
	( * pglFunction )( a, b, aInt ) ;
}

extern void APIENTRY glGetShaderInfoLog (GLuint a, GLsizei b, GLsizei * c, GLchar * d)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint , GLsizei , GLsizei * , GLchar * ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glGetShaderInfoLog" ) ;
	( * pglFunction )( a, b, c, d ) ;
}

extern GLint APIENTRY glGetUniformLocation (GLuint a, const GLchar * b)
{
	// Get a pointer to the extension function.
	typedef GLint ( WINAPI * functionPointer ) (GLuint , const GLchar * ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glGetUniformLocation" ) ;
	return ( * pglFunction )( a, b ) ;
}

extern void APIENTRY glLinkProgram (GLuint a)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glLinkProgram" ) ;
	( * pglFunction )( a ) ;
}

extern void APIENTRY glShaderSource (GLuint a , GLsizei b , const GLchar * * c , const GLint * d )
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint , GLsizei , const GLchar * * , const GLint * ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glShaderSource" ) ;
	( * pglFunction )( a, b, c, d ) ;
}

extern void APIENTRY glUniform1i (GLint a, GLint b)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLint, GLint ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glUniform1i" ) ;
	( * pglFunction )( a, b ) ;
}

extern void APIENTRY glUniform1f (GLint a, GLfloat b)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLint, GLfloat ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glUniform1f" ) ;
	( * pglFunction )( a, b ) ;
}

extern void APIENTRY glUniform4f (GLint a, GLfloat b, GLfloat c, GLfloat d, GLfloat e)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLint, GLfloat, GLfloat, GLfloat, GLfloat ) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glUniform4f" ) ;
	( * pglFunction )( a, b, c, d, e ) ;
}

extern void APIENTRY glUseProgram (GLuint a)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glUseProgram" ) ;
	( * pglFunction )( a ) ;
}

extern void APIENTRY glValidateProgram (GLuint a)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLuint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glValidateProgram" ) ;
	( * pglFunction )( a ) ;
}

extern void APIENTRY glBindFramebufferEXT (GLenum a, GLuint b)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLenum, GLuint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glBindFramebufferEXT" ) ;
	( * pglFunction )( a , b ) ;
}

extern GLenum APIENTRY glCheckFramebufferStatusEXT (GLenum a)
{
	// Get a pointer to the extension function.
	typedef GLenum ( WINAPI * functionPointer ) (GLenum) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glCheckFramebufferStatusEXT" ) ;
	return ( * pglFunction )( a  ) ;
}

extern void APIENTRY glFramebufferTexture2DEXT (GLenum a , GLenum b, GLenum c, GLuint d, GLint e)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLenum, GLenum, GLenum, GLuint, GLint) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glFramebufferTexture2DEXT" ) ;
	( * pglFunction )( a , b , c , d , e ) ;
}

extern void APIENTRY glGenFramebuffersEXT (GLsizei a, GLuint * b)
{
	// Get a pointer to the extension function.
	typedef void ( WINAPI * functionPointer ) (GLsizei, GLuint *) ;
	functionPointer pglFunction = ( functionPointer ) wglGetProcAddress ( "glGenFramebuffersEXT" ) ;
	( * pglFunction )( a , b ) ;
}

#endif //WIN32

%}

%include typemaps.i


%typemap (in) (const GLchar * *) (GLchar * lCode)
%{
	lCode = PyString_AS_STRING ( $input ) ;
	$1 =  & lCode ;
	//printf ( "code %s \n" , * $1 ) ;
%}


%typemap (in) (const GLint *) (int lAnInt)
%{
	lAnInt = (GLint) PyInt_AsLong( $input ) ;
	if (lAnInt == 0x7FFFFFFF) // max python integer
	{
		$1 = NULL ;
	}
	else
	{
		$1 = & lAnInt ;
 		//printf ( "lAnInt %ld \n" , (long)* $1 ) ;
	}
%}


%typemap (in) (GLsizei *) (int lAnIntSize)
%{
	lAnIntSize = (GLsizei) PyInt_AsLong( $input ) ;
	$1 = & lAnIntSize ;
	//printf ( "lAnIntSize %ld \n" , (long)* $1 ) ;
	
%}

%typemap (argout) (GLsizei *)
{
	$result = PyInt_FromLong((long) * $1);
	//printf ( "resultobject %ld \n" , (long) * $1 ) ;
}


%typemap (in) (GLchar *) (GLchar * lAString)
%{
	lAString = PyString_AS_STRING ( $input ) ;
	$1 =  lAString ;
	//printf ( "the string %s \n" , $1 ) ;
%}

%typemap (argout, fragment="t_output_helper") (GLchar *)
{
	PyObject * o = PyString_FromString((char *) $1);
	//printf ( "resultobject %ld \n" , (long) * $1 ) ;
    $result = mgl_t_output_helper($result, o);
}


%typemap (in) (const GLchar *) (GLchar * lAString)
%{
	lAString = PyString_AS_STRING ( $input ) ;
	$1 =  lAString ;
	//printf ( "the const string %s \n" , $1 ) ;
%}

%typemap (argout) (const GLchar *)
{
}


%typemap (in) (GLuint *) (unsigned int lAnIntSize)
%{
	lAnIntSize = (GLuint) PyInt_AsLong( $input ) ;
	$1 = & lAnIntSize ;
	//printf ( "lAnIntSize %ld \n" , (long)* $1 ) ;
%}

%typemap (argout) (GLuint *)
{
	$result = PyInt_FromLong((long) * $1);
	//printf ( "resultobject %ld \n" , (long) * $1 ) ;
}


%typemap (in) (GLint * aInt) (int lAnIntSize)
%{
	lAnIntSize = (GLint) PyInt_AsLong( $input ) ;
	$1 = & lAnIntSize ;
	//printf ( "lAnIntSize %ld \n" , (long)* $1 ) ;
%}

%typemap (argout) (GLint * aInt)
{
	$result = PyInt_FromLong((long) * $1);
	//printf ( "resultobject %ld \n" , (long) * $1 ) ;
}


%typemap (in) (const GLvoid *) //(Py_ssize_t buffer_len)
%{
//	if ( PyInt_AsLong( $input ) != 0 ) // allows to pass 0 in python in order to obtain a null pointer
//	{
//		Py_ssize_t buffer_len;
//		if ( PyObject_AsReadBuffer( $input ,
//                                    ( const void * * ) & $1 ,
//                                    & buffer_len ) != 0)
//		{
//			$1 = NULL ;
//		}
//	}
//	else
//	{
		$1 = NULL ;
//	}
%}


%include "glext_mgltools_i.h"

