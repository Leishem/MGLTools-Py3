
'''GL signature substitutions
cf. gltypemap and glArrayTypemap.i for usage ...
'''

sigsub = (
    'glClipPlane( GLenum plane, const GLdouble xxx[4]);',
    'glPolygonStipple( const GLubyte xxx[128]);',
    'glColor3bv( const GLbyte xxx[3]);',
    'glColor3dv( const GLdouble xxx[3]);',
    'glColor3fv( const GLfloat xxx[3]);',
    'glColor3iv( const GLint xxx[3]);',
    'glColor3sv( const GLshort xxx[3]);',
    'glColor3ubv( const GLubyte xxx[3]);',
    'glColor3uiv( const GLuint xxx[3]);',
    'glColor3usv( const GLushort xxx[3]);',
    'glColor4bv( const GLbyte xxx[4]);',
    'glColor4dv( const GLdouble xxx[4]);',
    'glColor4fv( const GLfloat xxx[4]);',
    'glColor4iv( const GLint xxx[4]);',
    'glColor4sv( const GLshort xxx[4]);',
    'glColor4ubv( const GLubyte xxx[4]);',
    'glColor4uiv( const GLuint xxx[4]);',
    'glColor4usv( const GLushort xxx[4]);',
    'glNormal3bv( const GLbyte xxx[3]);',
    'glNormal3dv( const GLdouble xxx[3]);',
    'glNormal3fv( const GLfloat xxx[3]);',
    'glNormal3iv( const GLint xxx[3]);',
    'glNormal3sv( const GLshort xxx[3]);',
    'glRasterPos2dv( const GLdouble xxx[2]);',
    'glRasterPos2fv( const GLfloat xxx[2]);',
    'glRasterPos2iv( const GLint xxx[2]);',
    'glRasterPos2sv( const GLshort xxx[2]);',
    'glRasterPos3dv( const GLdouble xxx[3]);',
    'glRasterPos3fv( const GLfloat xxx[3]);',
    'glRasterPos3iv( const GLint xxx[3]);',
    'glRasterPos3sv( const GLshort xxx[3]);',
    'glRasterPos4dv( const GLdouble xxx[4]);',
    'glRasterPos4fv( const GLfloat xxx[4]);',
    'glRasterPos4iv( const GLint xxx[4]);',
    'glRasterPos4sv( const GLshort xxx[4]);',
    'glVertex2dv( const GLdouble xxx[2]);',
    'glVertex2fv( const GLfloat xxx[2]);',
    'glVertex2iv( const GLint xxx[2]);',
    'glVertex2sv( const GLshort xxx[2]);',
    'glVertex3dv( const GLdouble xxx[3]);',
    'glVertex3fv( const GLfloat xxx[3]);',
    'glVertex3iv( const GLint xxx[3]);',
    'glVertex3sv( const GLshort xxx[3]);',
    'glVertex4dv( const GLdouble xxx[4]);',
    'glVertex4fv( const GLfloat xxx[4]);',
    'glVertex4iv( const GLint xxx[4]);',
    'glVertex4sv( const GLshort xxx[4]);',
    'glMultMatrixd( const GLdouble xxx[16]);',
    'glMultMatrixf( const GLfloat xxx[16]);',
    'glLoadMatrixd( const GLdouble xxx[16]);',
    'glLoadMatrixf( const GLfloat xxx[16]);',
    
    'gluPickMatrix( GLdouble x, GLdouble y, GLdouble delX,'
    ' GLdouble delY, GLint xxx[4]);',
    
    'gluUnProject( GLdouble winX, GLdouble winY,'
    ' GLdouble winZ, const GLdouble xxx[16], const GLdouble xxx[16],'
    ' const GLint xxx[4], GLdouble* objX, GLdouble* objY, GLdouble* objZ);',
    
    'gluProject( GLdouble objX, GLdouble objY, GLdouble objZ,'
    ' const GLdouble xxx[16], const GLdouble xxx[16], const GLint xxx[4],'
    ' GLdouble* winX, GLdouble* winY, GLdouble* winZ);'
    )

import re
fnamepat = re.compile( r'(\w+)\s*\(.*?\)\s*;') #, re.DOTALL) #pb with re.sub?
# -> glu functions not replaced

subdic = {} # dictionary of function definitions to change
for fundef in sigsub:
    subdic[ fnamepat.match( fundef).group( 1)] = fundef

def fixsigs( txt):
    def subfun( omatch):
        return subdic.get( omatch.group( 1), omatch.group())
    return fnamepat.sub( subfun, txt)

