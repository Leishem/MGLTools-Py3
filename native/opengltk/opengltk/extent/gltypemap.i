/*
 * copyright_notice
 */

%typemap(in)
  const GLbitfield*,
  const GLboolean*,
  const GLbyte*,
  const GLclampd*,
  const GLclampf*,
  const GLdouble*,
  const GLdouble[ANY],
  const GLenum*,
  const GLfloat*,
  const GLint*,
  const GLint[ANY],
  const GLshort*,
  const GLsizei*,
  const GLubyte*,
  const GLuint*,
  const GLushort*,
  const GLvoid*
{
  int buffer_len;
  if (PyObject_AsReadBuffer( $input, (const void**)&$1, &buffer_len))
    return NULL;
  if (! $1) return PyErr_Format( PyExc_ValueError,
				      "NULL buffer not accepted");
}

%typemap(in)
  GLbitfield*,
  GLboolean*,
  GLbyte*,
  GLclampd*,
  GLclampf*,
  GLdouble*,
  GLenum*,
  GLfloat*,
  GLint*, int*, /*glX*/
  GLshort*,
  GLsizei*,
  GLubyte*,
  GLuint*,
  GLushort*,
  GLvoid*,
  GLint[ANY]
{
  long buffer_len;
  if (PyObject_AsWriteBuffer( $input, (void**)&$1, &buffer_len))
    return NULL;
  if (! $1) return PyErr_Format( PyExc_ValueError,
				      "NULL buffer not accepted");
}

%typemap(argout)
  Glvoid**
{
  $result = PyCObject_FromVoidPtr( *$1, NULL);
}

%typemap(out)
  char*,
  const char*,
  char const*,
  unsigned char*,
  const unsigned char*,
  unsigned char const*
{
  if ($1) $result = PyString_FromString( (char const*)$1);
  else
    {
      Py_INCREF( Py_None);
      $result = Py_None;
    }
}

%include "glArrayTypemap.i"
