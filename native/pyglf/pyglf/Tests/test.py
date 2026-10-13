BMfont_list = [
    'arbat.bmf', 'arial.bmf', 'brushtype.bmf', 'chicago.bmf', 'courier.bmf',
    'cricket.bmf', 'crystal.bmf', 'fixedsys.bmf', 'gals.bmf', 'greek.bmf',
    'impact.bmf', 'proun.bmf', 'techno.bmf', 'times_new.bmf']

glffont_list = [
    'arial1.glf', 'courier1.glf', 'crystal1.glf', 'techno0.glf', 'techno1.glf',
    'times_new1.glf', 'aksent1.glf', 'alpine1.glf', 'broadway1.glf', 'chicago1.glf',
    'compact1.glf', 'cricket1.glf', 'garamond1.glf', 'gothic1.glf', 'penta1.glf',
    'present_script1.glf']

from DejaVu import Viewer
vi = Viewer()
from opengltk.OpenGL import GL

vi.currentCamera.Activate()

from pyglf import glf
glf.glfInit()

for font in glffont_list:
    i = glf.glfLoadFont("fonts/"+font)
    print i, font
    
# loading BMF fonts has to be done after the context exists
# else nothing appears
bmfd = {}
for font in BMfont_list:
    j = glf.glfLoadBMFFont("fonts/"+font)
    bmfd[j] = font
    print j, font

#glf.glfSetSymbolSpace(0.4)
#glf.glfSetSymbolDepth(.3)
#glf.glfDisable(glf.GLF_TEXTURING)
#glf.glfDisable(glf.GLF_CONTOURING)
#glf.glfEnable(glf.GLF_TEXTURING)
#glf.glfEnable(glf.GLF_CONTOURING);
#glf.glfSetContourColor(1, 1, 1, 1);
#glf.glfSetAnchorPoint(glf.GLF_CENTER_CENTER)

# setting thesee 2 creates bad values
glf.glfSetSymbolSpace(0.8)
glf.glfSetSymbolDepth(0.5)

glf.glfDisable(glf.GLF_TEXTURING)
#glf.glfEnable(glf.GLF_CONTOURING);
glf.glfSetContourColor(1, 1, 1, 1);

from DejaVu.Geom import Geom
class GLFLabels(Geom):
    """Class for sets of labels"""

            
    def Draw(self):
        vi.currentCamera.Activate()
        #GL.glClear(GL.GL_COLOR_BUFFER_BIT);
        #GL.glEnable(GL.GL_LINE_SMOOTH)
        #GL.glColor3f(0, 1, 0);
        #GL.glPushMatrix();
        #GL.glTranslatef(-1.2, 0, 0);
        #GL.glRotatef(90,1, 1, 1);
        #GL.glScalef(0.25, 0.25, 1);
        #GL.glDisable(GL.GL_CULL_FACE)
        #GL.glShadeModel(GL.GL_FLAT)
        GL.glPushMatrix();
        GL.glTranslatef(-10, -10, 0);
        for i, fontname in enumerate(glffont_list):
            glf.glfSetCurrentFont(i)
            GL.glTranslatef(0, 2, 0)
            #glf.glfDraw3DWiredString("Hello World in "+fontname)
            glf.glfDraw3DSolidString("Hello World in "+fontname)
            GL.glTranslatef(0, 2, 0)
            glf.glfDrawSolidString("Hello World in "+fontname)
        GL.glPopMatrix();

        GL.glTranslatef(-10, -10, 0);
        for k,v in bmfd.items():
            GL.glPushMatrix();
            glf.glfSetCurrentBMFFont(k)
            glf.glfStartBitmapDrawing()
            GL.glTranslatef(10, 10+.2*k, 0);
            GL.glScalef(4, 4, 0);
            glf.glfSetBRotateAngle(10)
            glf.glfDrawBString("Hello World in "+v)
            glf.glfStopBitmapDrawing()
            GL.glPopMatrix();
        #GL.glPopMatrix();
        #GL.glFlush();

labels = GLFLabels(immediateRendering=1, lighting=1, cull='none')
vi.lightModel.Set(twoSide=0)
vi.AddObject(labels)
