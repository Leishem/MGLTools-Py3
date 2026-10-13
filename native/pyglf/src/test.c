/*
=========================================================================
| GLF Library Test
|
| Author: Roman Podobedov
| Email: romka@ut.ee
| WEB: http://romka.demonews.com
=========================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include "glf.h"

int WindW, WindH;
int i;

void Draw(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 1);
	
	glfStringCentering(GL_FALSE);
	glfSetRotateAngle(0.0f);

	// Info message
	glfStringDirection(GLF_LEFT);
	glPushMatrix();
	glTranslatef(-1.2, 0.9, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("GLF Library - Text rotation and direction example");
	glPopMatrix();

	// Write down
	glfStringDirection(GLF_DOWN);
	glPushMatrix();
	glTranslatef(-1.2, 0.5, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Direction - DOWN");
	glPopMatrix();

	// Write up
	glfStringDirection(GLF_UP);
	glPushMatrix();
	glTranslatef(1.2, -0.5, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Direction - UP");
	glPopMatrix();

	// Write left
	glfStringDirection(GLF_LEFT);
	glPushMatrix();
	glTranslatef(0, 0.5, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Direction - LEFT");
	glPopMatrix();

	// Write right
	glfStringDirection(GLF_RIGHT);
	glPushMatrix();
	glTranslatef(0, -0.5, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Direction - RIGHT");
	glPopMatrix();

	// Rotated text
	glfStringCentering(GL_TRUE);
	
	glfStringDirection(GLF_LEFT);
	glfSetRotateAngle(45);

	glPushMatrix();
	glTranslatef(-0.5, 0, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Rotated text");
	glPopMatrix();

	glfSetRotateAngle(-45);
	glfStringDirection(GLF_DOWN);
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Rotated text");
	glPopMatrix();

	glfSetRotateAngle(-45);
	glfStringDirection(GLF_UP);
	glPushMatrix();
	glTranslatef(0, -0.5, 0);
	glScalef(0.05, 0.05, 1);
	glfDrawSolidString("Rotated text");
	glPopMatrix();

	for (i=0; i<12; i++)
	{
		glfSetRotateAngle(i*30);
		glfStringDirection(GLF_UP);
		glPushMatrix();
		glTranslatef(0.6, -0.5, 0);
		glScalef(0.03, 0.03, 1);
		glfDrawSolidString(" ABC");
		glPopMatrix();
	}

	for (i=0; i<12; i++)
	{
		glfSetRotateAngle(i*30);
		glfStringDirection(GLF_RIGHT);
		glPushMatrix();
		glTranslatef(-0.6, 0.5, 0);
		glScalef(0.03, 0.03, 1);
		glfDrawSolidString(" FLG");
		glPopMatrix();
	}


	glFlush();
	glutSwapBuffers();
}


void Reshape(int width, int height)
{
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(-1.33, 1.33, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  WindW = width;
  WindH = height;
}

static void Key(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: exit(0);
             break;
  }
}

void timf(int value)
{
  glutPostRedisplay();
  glutTimerFunc(1, timf, 0);
}


void main(int argc, char *argv[])
{
  WindW = 1024;
  WindH = 768;

  glfInit();
  glfLoadFont("fonts/times_new1.glf");
  glutInit(&argc, argv);
  glutInitWindowSize(WindW, WindH);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  (void)glutCreateWindow("GLF Demo - Solid Text");

  glutReshapeFunc(Reshape);
  glutDisplayFunc(Draw);
  glutKeyboardFunc(Key);
  glutTimerFunc(40, timf, 0);

  glClearColor(0, 0, 0, 0);
  glutMainLoop();
}
