#include "SimpleExample.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

// header file for the volume renderer
#include <VolumeRenderer.h>
 
/* Global Variables */
int W=640;
int H=480;

int mousex;
int mousey;
int cdx = 0;
int cdy = 0;
int whichbutton;
int state = ROTATE;

VolumeRenderer volumeRenderer;

GLdouble rotMatrix[16];
GLdouble transMatrix[16];

/// the following 3 functions show the interaction with the volume renderer

void InitVolumeRenderer() {
	if (!volumeRenderer.initRenderer()) {
		printf("Warning, there was an error initializing the volume renderer\n");
	}
}

void InitData() {
	// load the colormap from a file
	unsigned char byte_map[256*4];
	FILE* fp;
	// load the data from a file
	fp = fopen( "/home/annao/python/work/vh4.rawiv", "rb" );
	if (!fp) {
		printf("There was an error loading the data file\n");
		exit(1);
	}
	unsigned char* data = new unsigned char[128*128*128];
	// skip the header
	fread(data, sizeof(unsigned char), 68, fp);

	printf("Reading the data file\n");
	// load the data
	fread(data, sizeof(unsigned char), 128*128*128, fp);


	// send the data to the volume renderer
	if (!volumeRenderer.uploadColorMappedData(data, 128, 128, 128)) {
		// woops, maybe color mapped data wasnt supported

	}
	
	// commenting this out since shaded rendering isnt finished yet
	//printf("Calculating gradients\n");
	//volumeRenderer.calculateGradientsFromDensities(data, 128, 128, 128);

	delete [] data;
	fclose(fp);

	fp = fopen( "/home/annao/python/work/colormap.map", "rb" );
	if (!fp) {
		printf("There was an error loading the color map\n");
		exit(1);
	}
	fread(byte_map, sizeof(unsigned char), 256*4, fp);
	fclose(fp);

	volumeRenderer.uploadColorMap(byte_map);




}

void Display() {
	
	// Clear the buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	// Set up the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -4.0);
	glMultMatrixd(transMatrix);
	glMultMatrixd(rotMatrix);
	

	// the volume is rendered
	volumeRenderer.renderVolume();
		
		
	glutSwapBuffers();
}


int BeginGraphics (int* argc, char** argv, const char* name) {
	int win;
	
	glutInit(argc,argv);
	glutInitWindowSize(W,H);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	win = glutCreateWindow(name);
	glutSetWindow(win);
	init();
	
	RegisterCallbacks();
	
	glutMainLoop();
	return 0;
}

void RegisterCallbacks() {
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	glutKeyboardFunc(Keyboard);
}

void init() {
	InitProjection();
	InitModelMatrix();
	InitLighting();
	InitState();
	InitVolumeRenderer();
	InitData();
	
	
}

void InitProjection() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 30.0,
		(GLdouble) W / (GLdouble) H,
		0.01,
		20.0
		);
}

void InitModelMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/* initialize rotMatrix and transMatrix to the identity */
	glGetDoublev(GL_MODELVIEW_MATRIX, rotMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, transMatrix);
}

void InitLighting() {
	GLfloat lightdir[] = {-1.0, 1.0, 1.0, 0.0};
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightdir);
	glEnable(GL_NORMALIZE);
}

void InitState() {
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void Idle() {
	Rotate(cdx, cdy);
	glutPostRedisplay();
}

/* Callback function for the mouse button */
void MouseButton(int button, int mstate, int x, int y) {
	/* sets the initial positions for mousex and mousey */
	if (mstate == GLUT_DOWN) {
		mousex = x;
		mousey = y;
		whichbutton = button;
		cdx = 0;
		cdy = 0;
	}
	
}

/* Callback function for the mouse motion */
void MouseMove(int x, int y) {
	int dx = x-mousex;
	int dy = y-mousey;
	
	switch (state) {
	case ROTATE:
		Rotate(dx, dy);
		break;
	case TRANSLATE:
		if (whichbutton == GLUT_LEFT_BUTTON)
			Translate(dx, -dy,0);
		else
			Translate(0,0,-dy);
		break;
	}
	mousex = x;
	mousey = y;
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	
	switch (key) {
	case 'r':
	case 'R':
		state = ROTATE;
		break;
	case 't':
	case 'T':
		state = TRANSLATE;
		break;
	}
}


void Rotate(int dx, int dy) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotated(((double)dy), 1.0, 0.0, 0.0);
	glRotated(((double)dx), 0.0, 1.0, 0.0);
	glMultMatrixd(rotMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, rotMatrix);
	glPopMatrix();
}

void Translate(int dx, int dy, int dz) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslated(((double)dx)/100.0, ((double)dy)/100.0, ((double)dz)/10.0);
	glMultMatrixd(transMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, transMatrix);
	glPopMatrix();
}


int main(int argc, char** argv) {
  BeginGraphics(&argc, argv, "Simple Example");
//    glEnable(GL_TEXTURE_COLOR_TABLE_SGI);
//    GLenum errCode;
//    errCode = glGetError();
//    if (errCode!=GL_NO_ERROR)
//      printf ("GL ERROR-%d %s \n", errCode, gluErrorString(errCode));
//    if (!glIsEnabled(GL_TEXTURE_COLOR_TABLE_SGI))
//  	  printf ("drawPolygon: GL_TEXTURE_3D is *NOT* enabled\n");
	return 0;
	
}
