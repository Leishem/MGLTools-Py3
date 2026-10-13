#include <iostream>
#include <cstring>
#include <cstdlib>
#include <math.h>

#include "mydrawer.h"
#include "geoframe.h"
#include "octree.h"

float l_err, l_err_in;
float dist_level;



extern void cross(float* dest, const float* v1, const float* v2);
MyDrawer::MyDrawer()
{
  g_frame = 0;
  cut_flag = 0;
  //flat_flag=2;
  flat_flag=1;
  //wireframe_flag=0;
  wireframe_flag=1;
  strcpy(fn,"aaa");

  biggestDim = 1.0;
  centerx = 0.0;
  centery = 0.0;
  centerz = 0.0;

  center[0] = 0.0;
  center[1] = 0.0;
  center[2] = 0.0;


  zoom_z=1;
  plane_x = 32.0f;//(float) ((int)biggestDim/2.0);	//32.0;
  plane_z = 48.0f;
  //cout << "in MyDrawer(): plane_x " << plane_x << "plane_z " << plane_z << "\n";
}

//
void MyDrawer::display_tri(int i, int j, int k, int c, int normalflag, IntArray *newfaces)
{  //std::cout<< "Inside MyDrawer. display_tri\n";
	float v1[3], v2[3], norm[3], v0[3];
	int vert, ii, index, my_bool, f0, f2;
	vector<int> f;

	my_bool =	((g_frame->bound_sign[g_frame->quads[c][0]] == 1) &&
				(g_frame->bound_sign[g_frame->quads[c][1]] == 1) &&
				(g_frame->bound_sign[g_frame->quads[c][2]] == 1) &&
				(g_frame->bound_sign[g_frame->quads[c][3]] == 1)) || normalflag == -1;

	if(my_bool) {

		if(normalflag == -1) {
			vert = g_frame->quads[c][i];	v0[0] = g_frame->verts[vert][0];
			vert = g_frame->quads[c][j];	v1[0] = g_frame->verts[vert][0];
			vert = g_frame->quads[c][k];	v2[0] = g_frame->verts[vert][0];
			if(v0[0] >= plane_x && v1[0] >= plane_x && v2[0] >= plane_x) {
			//if(v0[0] <= plane_x && v1[0] <= plane_x && v2[0] <= plane_x &&
			//	v0[2] <= plane_x && v1[2] <= plane_x && v2[2] <= plane_x) {
				normalflag = -2;
			}
		}
		else {
			vert = g_frame->quads[c][i];
			v1[0] = v2[0] = -g_frame->verts[vert][0];
			v1[1] = v2[1] = -g_frame->verts[vert][1];
			v1[2] = v2[2] = -g_frame->verts[vert][2];

			vert = g_frame->quads[c][j];
			v1[0] += g_frame->verts[vert][0];
			v1[1] += g_frame->verts[vert][1];
			v1[2] += g_frame->verts[vert][2];

			vert = g_frame->quads[c][k];
			v2[0] += g_frame->verts[vert][0];
			v2[1] += g_frame->verts[vert][1];
			v2[2] += g_frame->verts[vert][2];

			cross(norm, v1, v2);

			// normal flipping
			if (normalflag == 1) {
				norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
			}
		}
		/**************************
		glBegin(GL_TRIANGLES);

		for(ii = 0; ii < 3; ii++) {
			index = i;
			if(ii == 1) index = j;
			if(ii == 2) index = k;
			vert = g_frame->quads[c][index];
			if (normalflag == -1 || flat_flag == 2) {	// hexa   flat shading + wireframe
				norm[0] = g_frame->normals[vert][0];
				norm[1] = g_frame->normals[vert][1];
				norm[2] = g_frame->normals[vert][2];
			}
			if(normalflag == -2) {norm[0] = 1.0;	norm[1] = 0.0;	norm[2] = 0.0;}
			glNormal3d(norm[0], norm[1], norm[2]);
			// hexa cross section
			if((g_frame->verts[vert][0]-32.5f)*(g_frame->verts[vert][0]-32.5f)+(g_frame->verts[vert][1]-32.5f)*(g_frame->verts[vert][1]-32.5f) +
				(g_frame->verts[vert][2]-32.5f)*(g_frame->verts[vert][2]-32.5f) < 100000.5f && normalflag != -2 && g_frame->numhexas > 0) {//900.5
				GLfloat diffRefl[] = {0.6f, 0.6f, 1.0f, 1.00f};
				//GLfloat diffRefl[] = {1.0f, 0.7f, 0.0f, 1.00f};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl);
			}
			else {
				//GLfloat diffRefl_1[] = {0.9, 0.7, 0.0, 1.00};
				GLfloat diffRefl_1[] = {1.0f, 0.7f, 0.0f, 1.00f};
				//GLfloat diffRefl_1[] = {0.6, 0.6, 1.0, 1.00};
				//GLfloat diffRefl_1[] = {1.0, 0.0, 0.0, 1.00};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl_1);
			}

			if(normalflag <= -1 && g_frame->verts[vert][0] < plane_x)
			//if(normalflag <= -1 && g_frame->verts[vert][0] < plane_x && g_frame->verts[vert][2] < plane_x)
				glVertex3d(
					//plane_x,
					g_frame->verts[vert][0],
					g_frame->verts[vert][1],
					g_frame->verts[vert][2]);
			else
				glVertex3d(
					g_frame->verts[vert][0],
					g_frame->verts[vert][1],
					g_frame->verts[vert][2]);
		}
		glEnd();
		******************/

		f.resize(3);
		for(ii = 0; ii < 3; ii++)
		  {
		    index = i;
		    if(ii == 1) index = j;
		    if(ii == 2) index = k;
		    f[ii] = g_frame->triangles[c][index];
		  }
		if (normalflag == 1)
		  {
		    f0 = f[0]; f2 = f[2];
		    f[0] = f2; f[2] = f0;
		  }
		newfaces->push_back(f);

	}
}

//
void MyDrawer::display_tri0(int i, int j, int k, int c, int normalflag, int wire_flag, IntArray *newfaces)
{ //std::cout<< "Inside MyDrawer. display_tri0\n";
	float v1[3], v2[3], norm[3];
	int vert, ii, index, my_bool, my_bool_0, f0, f2, nflag;
	vector<int> f;

	my_bool =	(g_frame->bound_sign[g_frame->triangles[c][0]] == 1) &&
				(g_frame->bound_sign[g_frame->triangles[c][1]] == 1) &&
				(g_frame->bound_sign[g_frame->triangles[c][2]] == 1);
	my_bool_0 =	(g_frame->bound_sign[g_frame->triangles[c][0]] == -1) &&
				(g_frame->bound_sign[g_frame->triangles[c][1]] == -1) &&
				(g_frame->bound_sign[g_frame->triangles[c][2]] == -1);

	if(my_bool || my_bool_0) {

		vert = g_frame->triangles[c][i];
		v1[0] = v2[0] = -g_frame->verts[vert][0];
		v1[1] = v2[1] = -g_frame->verts[vert][1];
		v1[2] = v2[2] = -g_frame->verts[vert][2];

		vert = g_frame->triangles[c][j];
		v1[0] += g_frame->verts[vert][0];
		v1[1] += g_frame->verts[vert][1];
		v1[2] += g_frame->verts[vert][2];

		vert = g_frame->triangles[c][k];
		v2[0] += g_frame->verts[vert][0];
		v2[1] += g_frame->verts[vert][1];
		v2[2] += g_frame->verts[vert][2];

		cross(norm, v1, v2);
                nflag = 0;

		// normal flipping
		if (normalflag == 1) {
			norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
		}
		if (g_frame->bound_tri[c] == 1) {
		  //std::cout << "in display_tri0 normal flipping, c = " << c << "\n";
		  nflag = 1;
		  norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
		}

		vert = g_frame->triangles[c][i];
		v1[0] = g_frame->verts[vert][0];
		v1[1] = g_frame->verts[vert][1];
		v1[2] = g_frame->verts[vert][2];
		/****************
		if(my_bool_0) {
		//if(my_bool && v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2] < 90000.0) {
			GLfloat diffRefl[] = {1.0f, 0.0f, 0.0f, 1.00f};
			//GLfloat diffRefl[] = {1.0, 0.7, 0.0, 1.00};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl);
		}
		else {
			//GLfloat diffRefl_1[] = {0.0, 0.6, 0.5, 1.00};
			GLfloat diffRefl_1[] = {1.0f, 0.7f, 0.0f, 1.0f};
			//GLfloat diffRefl_1[] = {0.6f, 0.6f, 1.0f, 1.00f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl_1);
		}
		***************/
		/****************
		if(wire_flag == 0) {
			glBegin(GL_TRIANGLES);
			for(ii = 0; ii < 3; ii++) {
				index = i;
				if(ii == 1) index = j;
				if(ii == 2) index = k;
				vert = g_frame->triangles[c][index];
				if(flat_flag == 2) {
					norm[0] = g_frame->normals[vert][0];
					norm[1] = g_frame->normals[vert][1];
					norm[2] = g_frame->normals[vert][2];
				}
				glNormal3d(norm[0], norm[1], norm[2]);
				glVertex3d( g_frame->verts[vert][0],
							g_frame->verts[vert][1],
							g_frame->verts[vert][2] );
			}
			glEnd();
		}
		else {
			glBegin(GL_LINE_STRIP);

			for(ii = 0; ii < 3; ii++) {
				index = i;
				if(ii == 1) index = j;
				if(ii == 2) index = k;
				vert = g_frame->triangles[c][index];
				if(flat_flag == 2) {
					norm[0] = g_frame->normals[vert][0];
					norm[1] = g_frame->normals[vert][1];
					norm[2] = g_frame->normals[vert][2];
					if (my_bool_0) {
						norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
					}
				}
				glNormal3d(norm[0], norm[1], norm[2]);
				glVertex3d(
					g_frame->verts[vert][0],
					g_frame->verts[vert][1],
					g_frame->verts[vert][2]);
			}

			vert = g_frame->triangles[c][i];
			glNormal3d(norm[0], norm[1], norm[2]);
			glVertex3d(
				g_frame->verts[vert][0],
				g_frame->verts[vert][1],
				g_frame->verts[vert][2]);
			glEnd();
		}
		**********************/
		f.resize(3);
		for(ii = 0; ii < 3; ii++)
		  {
		    index = i;
		    if(ii == 1) index = j;
		    if(ii == 2) index = k;
		    f[ii] = g_frame->triangles[c][index];
		  }
		if (normalflag == 1 && nflag == 0)
		  {
		    f0 = f[0]; f2 = f[2];
		    f[0] = f2; f[2] = f0;
		  }
		newfaces->push_back(f);
	}
}
//
void MyDrawer::display_hexa(int c, int normalflag, int wireframe,IntArray *newfaces)
{ //std::cout<< "Inside MyDrawer. display_hexa\n";
	float v0[3], v1[3], v2[3], v3[3], v4[3], v5[3], v6[3], v7[3];
	int vert, ii, jj, my_bool, my_bool_1, my_bool_2, my_bool_3;

	vert = g_frame->quads[6*c][0];
	for(ii = 0; ii < 3; ii++)	v0[ii] = g_frame->verts[vert][ii];
	vert = g_frame->quads[6*c][1];
	for(ii = 0; ii < 3; ii++)	v3[ii] = g_frame->verts[vert][ii];
	vert = g_frame->quads[6*c][2];
	for(ii = 0; ii < 3; ii++)	v7[ii] = g_frame->verts[vert][ii];
	vert = g_frame->quads[6*c][3];
	for(ii = 0; ii < 3; ii++)	v4[ii] = g_frame->verts[vert][ii];

	vert = g_frame->quads[6*c+1][0];
	for(ii = 0; ii < 3; ii++)	v2[ii] = g_frame->verts[vert][ii];
	vert = g_frame->quads[6*c+1][1];
	for(ii = 0; ii < 3; ii++)	v1[ii] = g_frame->verts[vert][ii];
	vert = g_frame->quads[6*c+1][2];
	for(ii = 0; ii < 3; ii++)	v5[ii] = g_frame->verts[vert][ii];
	vert = g_frame->quads[6*c+1][3];
	for(ii = 0; ii < 3; ii++)	v6[ii] = g_frame->verts[vert][ii];

	my_bool =	(v0[0] <= plane_x) && (v1[0] <= plane_x) && (v2[0] <= plane_x) && (v3[0] <= plane_x) &&
				(v4[0] <= plane_x) && (v5[0] <= plane_x) && (v6[0] <= plane_x) && (v7[0] <= plane_x);
	my_bool_1 = (v0[0] >= plane_x) && (v1[0] > plane_x) && (v2[0] > plane_x) && (v3[0] >= plane_x) &&
				(v4[0] >= plane_x) && (v5[0] > plane_x) && (v6[0] > plane_x) && (v7[0] >= plane_x);
	my_bool_3 = (v0[0] == plane_x) && (v3[0] == plane_x) && (v4[0] == plane_x) && (v7[0] == plane_x);
	/******************
	if(wireframe == 0) {
		if(my_bool) {
			for(ii = 0; ii < 6; ii++) {
				display_tri(0, 1, 2, 6*c+ii, normalflag);
				display_tri(2, 3, 0, 6*c+ii, normalflag);
			}
		}
		else {
			if(my_bool_1 == 0 || my_bool_3 == 1) {
				for(ii = 0; ii < 6; ii++) {
					normalflag = -1;
					display_tri(0, 1, 2, 6*c+ii, normalflag);
					display_tri(2, 3, 0, 6*c+ii, normalflag);
				}
			}
		}
	}
	else {
		for (ii = 0; ii < 6; ii++) {
			glBegin(GL_LINE_STRIP);

			my_bool_2 =	abs(g_frame->bound_sign[g_frame->quads[6*c+ii][0]]) == 1 &&
						abs(g_frame->bound_sign[g_frame->quads[6*c+ii][1]]) == 1 &&
						abs(g_frame->bound_sign[g_frame->quads[6*c+ii][2]]) == 1 &&
						abs(g_frame->bound_sign[g_frame->quads[6*c+ii][3]]) == 1;
			if(my_bool) {
				if(my_bool_2) {
					for(jj = 0; jj < 4; jj++) {
						vert = g_frame->quads[6*c+ii][jj];
						glVertex3d(g_frame->verts[vert][0], g_frame->verts[vert][1], g_frame->verts[vert][2]);
					}
					vert = g_frame->quads[6*c+ii][0];
					glVertex3d(g_frame->verts[vert][0], g_frame->verts[vert][1], g_frame->verts[vert][2]);
				}
			}
			else {
				if(my_bool_1 == 0 || my_bool_3 == 1) {
					for(jj = 0; jj < 4; jj++) {
						vert = g_frame->quads[6*c+ii][jj];
						if(g_frame->verts[vert][0] > plane_x)
							//glVertex3d(plane_x, g_frame->verts[vert][1], g_frame->verts[vert][2]);
							glVertex3d(g_frame->verts[vert][0], g_frame->verts[vert][1], g_frame->verts[vert][2]);
						else
							glVertex3d(g_frame->verts[vert][0], g_frame->verts[vert][1], g_frame->verts[vert][2]);
					}
					vert = g_frame->quads[6*c+ii][0];
					if(g_frame->verts[vert][0] > plane_x)
						//glVertex3d(plane_x, g_frame->verts[vert][1], g_frame->verts[vert][2]);
						glVertex3d(g_frame->verts[vert][0], g_frame->verts[vert][1], g_frame->verts[vert][2]);
					else
						glVertex3d(g_frame->verts[vert][0], g_frame->verts[vert][1], g_frame->verts[vert][2]);
				}
			}
			glEnd();
		}
	}
	****************/
	if(my_bool) {
	  for(ii = 0; ii < 6; ii++) {
	    display_tri(0, 1, 2, 6*c+ii, normalflag, newfaces);
	    display_tri(2, 3, 0, 6*c+ii, normalflag, newfaces);
	  }
	}
	else {
	  if(my_bool_1 == 0 || my_bool_3 == 1) {
	    for(ii = 0; ii < 6; ii++) {
	      normalflag = -1;
	      display_tri(0, 1, 2, 6*c+ii, normalflag, newfaces);
	      display_tri(2, 3, 0, 6*c+ii, normalflag, newfaces);
				}
	  }
	}

}

//
void MyDrawer::display_tri00(int i, int j, int k, int c, int normalflag, int wire_flag, int num_eq, IntArray *newfaces)
{ //std::cout<< "Inside MyDrawer. display_tri00\n";
	float v1[3], v2[3], norm[3];
	int vert, ii, index, my_bool, my_bool_0 = 0, my_bool_1, my_bool_2, f0, f2, nflag;

	vector<int> f;
	nflag = 0;
	my_bool =	(g_frame->bound_sign[g_frame->triangles[c][0]] == 1) &&
				(g_frame->bound_sign[g_frame->triangles[c][1]] == 1) &&
				(g_frame->bound_sign[g_frame->triangles[c][2]] == 1);
	my_bool_0 =	(g_frame->bound_sign[g_frame->triangles[c][0]] == -1) &&
				(g_frame->bound_sign[g_frame->triangles[c][1]] == -1) &&
				(g_frame->bound_sign[g_frame->triangles[c][2]] == -1);

	my_bool_1 = (g_frame->verts[g_frame->triangles[c][i]][0] == plane_x) &&
				(g_frame->verts[g_frame->triangles[c][j]][0] == plane_x) &&
				(g_frame->verts[g_frame->triangles[c][k]][0] == plane_x) && (num_eq == 3);

	my_bool_2 = (g_frame->verts[g_frame->triangles[c][i]][2] == plane_z) &&
				(g_frame->verts[g_frame->triangles[c][j]][2] == plane_z) &&
				(g_frame->verts[g_frame->triangles[c][k]][2] == plane_z) && (num_eq == -3);
	//if(my_bool || abs(num_eq) == 3) {
	if(my_bool || my_bool_0 || my_bool_1 || my_bool_2) {

		vert = g_frame->triangles[c][i];
		v1[0] = v2[0] = -g_frame->verts[vert][0];
		v1[1] = v2[1] = -g_frame->verts[vert][1];
		v1[2] = v2[2] = -g_frame->verts[vert][2];

		vert = g_frame->triangles[c][j];
		v1[0] += g_frame->verts[vert][0];
		v1[1] += g_frame->verts[vert][1];
		v1[2] += g_frame->verts[vert][2];

		vert = g_frame->triangles[c][k];
		v2[0] += g_frame->verts[vert][0];
		v2[1] += g_frame->verts[vert][1];
		v2[2] += g_frame->verts[vert][2];

		cross(norm, v1, v2);

		// normal flipping
		if (normalflag == 1 && my_bool) {
			norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
		}
		if (g_frame->bound_tri[c] == 1) {
		  //std::cout << "in display_tri00 normal flipping, c = " << c << "\n";
		  nflag = 1;
		  norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
		}

		vert = g_frame->triangles[c][i];
		v1[0] = g_frame->verts[vert][0];
		v1[1] = g_frame->verts[vert][1];
		v1[2] = g_frame->verts[vert][2];
		/**********************
		if(my_bool_0) {
		//if(my_bool && v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2] < 90000.0) {
			GLfloat diffRefl[] = {1.0, 0.0, 0.0, 1.00};
			//GLfloat diffRefl[] = {1.0, 0.7, 0.0, 1.00};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl);
		}
		else {
			//GLfloat diffRefl_1[] = {0.0, 0.6, 0.5, 1.00};
			GLfloat diffRefl_1[] = {1.0f, 0.7f, 0.0f, 1.00f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffRefl_1);
		}
		*********************/
		/*********************
		if(wire_flag == 0) {
			glBegin(GL_TRIANGLES);
			for(ii = 0; ii < 3; ii++) {
				index = i;
				if(ii == 1) index = j;
				if(ii == 2) index = k;
				vert = g_frame->triangles[c][index];
				if(flat_flag == 2) {
					norm[0] = g_frame->normals[vert][0];
					norm[1] = g_frame->normals[vert][1];
					norm[2] = g_frame->normals[vert][2];
					if (my_bool_0) {
						norm[0] = -norm[0];		norm[1] = -norm[1];		norm[2] = -norm[2];
					}
				}
				if((my_bool == 0 || my_bool_0 == 1) && num_eq == 3) {
					norm[0] =  1.0;		norm[1] =  0.0;		norm[2] =  0.0;
				}
				if((my_bool == 0 || my_bool_0 == 1) && num_eq == -3) {
					norm[0] =  0.0;		norm[1] =  0.0;		norm[2] =  1.0;
				}
				glNormal3d(norm[0], norm[1], norm[2]);
				glVertex3d(
					g_frame->verts[vert][0],
					g_frame->verts[vert][1],
					g_frame->verts[vert][2]);
			}
			glEnd();
		}
		else {
			glBegin(GL_LINE_STRIP);

			for(ii = 0; ii < 3; ii++) {
				index = i;
				if(ii == 1) index = j;
				if(ii == 2) index = k;
				vert = g_frame->triangles[c][index];
				if(flat_flag == 2) {
					norm[0] = g_frame->normals[vert][0];
					norm[1] = g_frame->normals[vert][1];
					norm[2] = g_frame->normals[vert][2];
				}
				if((my_bool == 0 || my_bool_0 == 1) && num_eq == 3) {
					norm[0] =  1.0;		norm[1] =  0.0;		norm[2] =  0.0;
				}
				if((my_bool == 0 || my_bool_0 == 1) && num_eq == -3) {
					norm[0] =  0.0;		norm[1] =  0.0;		norm[2] =  1.0;
				}
				glNormal3d(norm[0], norm[1], norm[2]);
				glVertex3d(
					g_frame->verts[vert][0],
					g_frame->verts[vert][1],
					g_frame->verts[vert][2]);
			}

			vert = g_frame->triangles[c][i];
			glNormal3d(norm[0], norm[1], norm[2]);
			glVertex3d(
				g_frame->verts[vert][0],
				g_frame->verts[vert][1],
				g_frame->verts[vert][2]);
			glEnd();
		}
		************/
		if (my_bool_1 && cut_flag) // this triangle is on the cut plane and is displayed                                             // by display_tri_vv()
		  {
		    //std::cout << "[" << f[0] << " ," << f[1] << " ," << f[2] << "]," << "\n";
		    return;
		  }
		if (my_bool_2 && cut_flag) // this triangle is on the cut plane and is displayed                                             // by display_tri_vv()
		  return;
		else
		  {
		    f.resize(3);
		    for(ii = 0; ii < 3; ii++)
		      {
			index = i;
			if(ii == 1) index = j;
			if(ii == 2) index = k;
			f[ii] = g_frame->triangles[c][index];
		      }
		    if (normalflag == 1 && nflag == 0)
		      {
			f0 = f[0]; f2 = f[2];
			f[0] = f2; f[2] = f0;
		      }
		    newfaces->push_back(f);
		  }
	}
}



// outputs vertices for the cutout section
void MyDrawer::display_tri_vv(float* v0, float* v1, float* v2, int c, int normalflag, int wire_flag, FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_tri_vv\n";
	float vv01[3], vv02[3], norm[3];
	int ii, my_bool, my_bool_0, vert;

	if(c == -1) {
		my_bool = 1;	my_bool_0 = 0;
	}
	else {
		my_bool =	(g_frame->bound_sign[g_frame->triangles[c][0]] == 1) &&
					(g_frame->bound_sign[g_frame->triangles[c][1]] == 1) &&
					(g_frame->bound_sign[g_frame->triangles[c][2]] == 1);
		my_bool_0 =	(g_frame->bound_sign[g_frame->triangles[c][0]] == -1) &&
					(g_frame->bound_sign[g_frame->triangles[c][1]] == -1) &&
					(g_frame->bound_sign[g_frame->triangles[c][2]] == -1);
	}

	for(ii = 0; ii < 3; ii++) {
		vv01[ii] = v1[ii] - v0[ii];
		vv02[ii] = v2[ii] - v0[ii];
	}
	cross(norm, vv01, vv02);

	// normal flipping
	if (normalflag == 1) {
		for(ii = 0; ii < 3; ii++) norm[ii] = - norm[ii];
	}
	/****
	vert = g_frame->triangles[c][0]; // this gave segmentation fault whith c= -1
	vv01[0] = g_frame->verts[vert][0];
	vv01[1] = g_frame->verts[vert][1];
	vv01[2] = g_frame->verts[vert][2];
	***/

	/*****************
	if(wire_flag == 1)	{

	}

	else {
		glBegin(GL_TRIANGLES);
		if(flat_flag == 2 && c != -1) {
			vert = g_frame->triangles[c][0];
			if(g_frame->bound_sign[vert] == -1) {
			  norm[0] = -norm[0]; norm[1]= -norm[1]; norm[2]=-norm[2]
			}
			else {
			  norm[0] = g_frame->normals[vert][0];
			  norm[1] = g_frame->normals[vert][1];
			  norm[2] = g_frame->normals[vert][2];
			}
			glVertex3d(v0[0], v0[1], v0[2]);

			vert = g_frame->triangles[c][1];
			if(g_frame->bound_sign[vert] == -1) {
				glNormal3d(-norm[0], -norm[1], -norm[2]);
			}
			else {
				glNormal3d(g_frame->normals[vert][0],
					g_frame->normals[vert][1], g_frame->normals[vert][2]);
			}
			glVertex3d(v1[0], v1[1], v1[2]);

			vert = g_frame->triangles[c][2];
			if(g_frame->bound_sign[vert] == -1) {
				glNormal3d(-norm[0], -norm[1], -norm[2]);
			}
			else {
				glNormal3d(g_frame->normals[vert][0],
					g_frame->normals[vert][1], g_frame->normals[vert][2]);
			}
			glVertex3d(v2[0], v2[1], v2[2]);
		}
		else {
			glNormal3d(norm[0], norm[1], norm[2]);
			glVertex3d(v0[0], v0[1], v0[2]);
			glVertex3d(v1[0], v1[1], v1[2]);
			glVertex3d(v2[0], v2[1], v2[2]);
		}
		glEnd();
	}
	********************/

	int noremove = 0; // indicates if the vertex can be removed from the list of vertices if it is duplicated
	if(c != -1) // vertices on the intersection of clipping planes with the isosurface
	  {
	    //std::cout << vcount << ", " <<vcount+1 <<", "<<vcount+2<< ",";
	    noremove = 1;
	  }

	// There are triangles at the clipping planes intersection that go beyond
	// the clipping planes (do not clip each other) . The following code tries to
	// identify such triangles.
	if(c == -1)
	  {
	    int add_tri = 0;
	    if (v0[2]< plane_z || v1[2] < plane_z || v2[2] < plane_z)
	      {

		if (v0[2] <plane_z)
		  {
		    if (v1[2] > plane_z || v2[2] > plane_z)
		      {
			v0[2] = plane_z;
			add_tri = 1;
		      }

		  }
		if (v1[2]< plane_z)
		  {
		    if (v0[2] > plane_z || v2[2] > plane_z)
		      {
			v1[2] = plane_z;
			add_tri = 1;
		      }
		  }
		if ( v2[2] < plane_z)
		  {
		    if (v0[2] > plane_z || v1[2] > plane_z)
		      {
			v2[2] = plane_z;
			add_tri = 1;
		      }
		  }
		if (! add_tri)
		  return;
		/*******
		else{
		  std::cout << "vcount: " << vcount << "\n";
		  std::cout << "z_plane: old verts: \n" ;
		  std::cout<< "["<<vo0[0]<<" ," <<vo0[1]<< "," <<vo0[2]<<"],";
		  std::cout<< "["<<vo1[0]<<" ," <<vo1[1]<< "," <<vo1[2]<<"],";
		  std::cout<< "["<<vo2[0]<<" ," <<vo2[1]<< "," <<vo2[2]<<"]," << "\n";
		  std:: cout << "z_plane, new verts \n";
		  std::cout<< "["<<v0[0]<<" ," <<v0[1]<< "," <<v0[2]<<"],";
		  std::cout<< "["<<v1[0]<<" ," <<v1[1]<< "," <<v1[2]<<"],";
		  std::cout<< "["<<v2[0]<<" ," <<v2[1]<< "," <<v2[2]<<"]," << "\n";
		}
		*****/

	      }
	    add_tri = 0;
	    if (v0[0]<plane_x || v1[0]<plane_x || v2[0]<plane_x)
	      {
		if (v0[0] <plane_x)
		  {
		    if (v1[0] > plane_x || v2[0] > plane_x)
		      {
			v0[0] = plane_x;
			add_tri = 1;
		      }

		  }
		if (v1[0]< plane_x)
		  {
		    if (v0[0] > plane_x || v2[0] > plane_x)
		      {
			v1[0] = plane_x;
			add_tri = 1;
		      }
		  }
		if(v2[0] < plane_x)
		  {
		    if (v0[0] > plane_x || v1[0] > plane_x)
		      {
			v2[0] = plane_x;
			add_tri = 1;
		      }
		  }
		if (!add_tri)
		  return;
		/*******
		else{
		  std::cout << "vcount: " << vcount << "\n";
		  std::cout << "x_plane: old verts: \n" ;
		  std::cout<< "["<<vo0[0]<<" ," <<vo0[1]<< "," <<vo0[2]<<"],";
		  std::cout<< "["<<vo1[0]<<" ," <<vo1[1]<< "," <<vo1[2]<<"],";
		  std::cout<< "["<<vo2[0]<<" ," <<vo2[1]<< "," <<vo2[2]<<"]," << "\n";
		  std:: cout << "x_plane, new verts \n";
		  std::cout<< "["<<v0[0]<<" ," <<v0[1]<< "," <<v0[2]<<"],";
		  std::cout<< "["<<v1[0]<<" ," <<v1[1]<< "," <<v1[2]<<"],";
		  std::cout<< "["<<v2[0]<<" ," <<v2[1]<< "," <<v2[2]<<"]," << "\n";
		}
		*********/
	      }
	    // the following finds triangles at the plane intersection and marks the
	    // the vertices so that the duplicated vertices are not removed from the list
	    // of vertices .
	    if ((v0[0] == plane_x) && (v1[0] == plane_x) && (v2[0] == plane_x))
	      {
		if (v0[2] == plane_z || v1[2] == plane_z || v2[2] == plane_z)
		  noremove = 1;
	      }
	    else if ((v0[2] == plane_z) && (v1[2] == plane_z) && (v2[2] == plane_z))
	      {
		if (v0[0] == plane_x || v1[0] == plane_x || v2[0] == plane_x)
		  noremove = 1;
	      }

	  }
	std::vector<float> t1, t2, t3;
	t1.resize(4);
	t2.resize(4);
	t3.resize(4);
	t1[0] = v0[0]; t1[1] = v0[1]; t1[2] = v0[2]; t1[3] = noremove;
	t2[0] = v1[0]; t2[1] = v1[1]; t2[2] = v1[2]; t2[3] = noremove;
	t3[0] = v2[0]; t3[1] = v2[1]; t3[2] = v2[2]; t3[3] = noremove;

	if (normalflag == 1)
	  {
	    vertset->push_back(t3);
	    vertset->push_back(t2);
	    vertset->push_back(t1);

	  }
	else
	  {
	    vertset->push_back(t1);
	    vertset->push_back(t2);
	    vertset->push_back(t3);
	  }
	/******
	std::vector<float> v;
	v.resize(3);
	v[0] = v0[0];  v[1]=v0[1]; v[2]= v0[2];
	vertset->push_back(v);
	v[0] = v1[0]; v[1] = v1[1]; v[2] = v1[2];
	vertset->push_back(v);
	v[0] = v2[0]; v[1]= v2[1]; v[2] = v2[2];
	vertset->push_back(v);
	***/
	vcount = vcount +3;
}



//
void MyDrawer::display_permute_1(float* v0, float* v1, float* v2, float* v3)
{ //std::cout<< "Inside MyDrawer. display_permute_1\n";
	float vv0[3], vv1[3], vv2[3], vv3[3];
	int ii;

	for(ii = 0; ii < 3; ii++) {
		vv0[ii] = v0[ii];	vv1[ii] = v1[ii];
		vv2[ii] = v2[ii];	vv3[ii] = v3[ii];
	}
	if(vv0[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv2[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv1[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv2[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv1[ii];
		}
	}
	if(vv2[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv0[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv2[ii];
		}
	}
}
//
void MyDrawer::display_permute_1_z(float* v0, float* v1, float* v2, float* v3)
{ //std::cout<< "Inside MyDrawer. display)permute_1_z\n";
	float vv0[3], vv1[3], vv2[3], vv3[3];
	int ii;

	for(ii = 0; ii < 3; ii++) {
		vv0[ii] = v0[ii];	vv1[ii] = v1[ii];
		vv2[ii] = v2[ii];	vv3[ii] = v3[ii];
	}
	if(vv0[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv2[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv1[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv2[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv1[ii];
		}
	}
	if(vv2[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv0[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv2[ii];
		}
	}
}
//
void MyDrawer::display_permute_2(float* v0, float* v1, float* v2, float* v3)
{ //std::cout<< "Inside MyDrawer. display_permute_2\n";
	float vv0[3], vv1[3], vv2[3], vv3[3];
	int ii;

	for(ii = 0; ii < 3; ii++) {
		vv0[ii] = v0[ii];	vv1[ii] = v1[ii];
		vv2[ii] = v2[ii];	vv3[ii] = v3[ii];
	}
	if(vv0[0] <= plane_x && vv2[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv2[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv1[ii];
		}
	}
	if(vv0[0] <= plane_x && vv3[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv1[ii];	v3[ii] = vv2[ii];
		}
	}
	if(vv2[0] <= plane_x && vv1[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv2[ii];	v1[ii] = vv1[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv1[0] <= plane_x && vv3[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv2[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv2[0] <= plane_x && vv3[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv2[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv0[ii];	v3[ii] = vv1[ii];
		}
	}
}
//
void MyDrawer::display_permute_2_z(float* v0, float* v1, float* v2, float* v3)
{ //std::cout<< "Inside MyDrawer. display_permute_2_z.\n";

	float vv0[3], vv1[3], vv2[3], vv3[3];
	int ii;

	for(ii = 0; ii < 3; ii++) {
		vv0[ii] = v0[ii];	vv1[ii] = v1[ii];
		vv2[ii] = v2[ii];	vv3[ii] = v3[ii];
	}
	if(vv0[2] <= plane_z && vv2[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv2[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv1[ii];
		}
	}
	if(vv0[2] <= plane_z && vv3[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv1[ii];	v3[ii] = vv2[ii];
		}
	}
	if(vv2[2] <= plane_z && vv1[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv2[ii];	v1[ii] = vv1[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv1[2] <= plane_z && vv3[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv2[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv2[2] <= plane_z && vv3[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv2[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv0[ii];	v3[ii] = vv1[ii];
		}
	}
}
//
void MyDrawer::display_permute_3(float* v0, float* v1, float* v2, float* v3)
{ //std::cout<< "Inside MyDrawer. display_permute_3.\n";

	float vv0[3], vv1[3], vv2[3], vv3[3];
	int ii;

	for(ii = 0; ii < 3; ii++) {
		vv0[ii] = v0[ii];	vv1[ii] = v1[ii];
		vv2[ii] = v2[ii];	vv3[ii] = v3[ii];
	}
	if(vv1[0] <= plane_x && vv2[0] <= plane_x && vv3[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv2[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv0[0] <= plane_x && vv2[0] <= plane_x && vv3[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv2[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv1[ii];
		}
	}
	if(vv0[0] <= plane_x && vv1[0] <= plane_x && vv3[0] <= plane_x) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv0[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv2[ii];
		}
	}
}
//
void MyDrawer::display_permute_3_z(float* v0, float* v1, float* v2, float* v3)
{ //std::cout<< "Inside MyDrawer. display_permute_3_z\n";
	float vv0[3], vv1[3], vv2[3], vv3[3];
	int ii;

	for(ii = 0; ii < 3; ii++) {
		vv0[ii] = v0[ii];	vv1[ii] = v1[ii];
		vv2[ii] = v2[ii];	vv3[ii] = v3[ii];
	}
	if(vv1[2] <= plane_z && vv2[2] <= plane_z && vv3[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv3[ii];
			v2[ii] = vv2[ii];	v3[ii] = vv0[ii];
		}
	}
	if(vv0[2] <= plane_z && vv2[2] <= plane_z && vv3[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv0[ii];	v1[ii] = vv2[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv1[ii];
		}
	}
	if(vv0[2] <= plane_z && vv1[2] <= plane_z && vv3[2] <= plane_z) {
		for(ii = 0; ii < 3; ii++) {
			v0[ii] = vv1[ii];	v1[ii] = vv0[ii];
			v2[ii] = vv3[ii];	v3[ii] = vv2[ii];
		}
	}
}
//
void MyDrawer::display_1(int* vert_bound, int c, float* v0, float* v1, float* v2,
			 float* v3,  int normalflag, int wire_flag,
			 FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_1.\n";

	float vv0[3], vv1[3], vv2[3], ratio_0, ratio_1, ratio_2;

	vv0[0] = plane_x;
	ratio_0 = (plane_x - v0[0]) / (v3[0] - v0[0]);
	vv0[1] = v0[1] + ratio_0 * (v3[1] - v0[1]);
	vv0[2] = v0[2] + ratio_0 * (v3[2] - v0[2]);

	vv1[0] = plane_x;
	ratio_1 = (plane_x - v1[0]) / (v3[0] - v1[0]);
	vv1[1] = v1[1] + ratio_1 * (v3[1] - v1[1]);
	vv1[2] = v1[2] + ratio_1 * (v3[2] - v1[2]);

	vv2[0] = plane_x;
	ratio_2 = (plane_x - v2[0]) / (v3[0] - v2[0]);
	vv2[1] = v2[1] + ratio_2 * (v3[1] - v2[1]);
	vv2[2] = v2[2] + ratio_2 * (v3[2] - v2[2]);
	display_tri_vv(vv0, vv2, vv1, -1, 1, wire_flag, vertset);
	if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
		display_tri_vv(vv1, vv2, v3,  4*c+1, 1, wire_flag, vertset);
	if(abs(vert_bound[0]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
		display_tri_vv(vv2, vv0, v3,  4*c+2, 1, wire_flag, vertset);
	if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3)
		display_tri_vv(vv0, vv1, v3,  4*c+3, 1, wire_flag, vertset);
}

//
void MyDrawer::display_1_z(int* vert_bound, int c, float* v0, float* v1, float* v2,
			   float* v3, int normalflag, int wire_flag, FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_1_z.\n";


	float vv0[3], vv1[3], vv2[3], ratio_0, ratio_1, ratio_2;

	vv0[2] = plane_z;
	ratio_0 = (plane_z - v0[2]) / (v3[2] - v0[2]);
	vv0[1] = v0[1] + ratio_0 * (v3[1] - v0[1]);
	vv0[0] = v0[0] + ratio_0 * (v3[0] - v0[0]);

	vv1[2] = plane_z;
	ratio_1 = (plane_z - v1[2]) / (v3[2] - v1[2]);
	vv1[1] = v1[1] + ratio_1 * (v3[1] - v1[1]);
	vv1[0] = v1[0] + ratio_1 * (v3[0] - v1[0]);

	vv2[2] = plane_z;
	ratio_2 = (plane_z - v2[2]) / (v3[2] - v2[2]);
	vv2[1] = v2[1] + ratio_2 * (v3[1] - v2[1]);
	vv2[0] = v2[0] + ratio_2 * (v3[0] - v2[0]);

	display_tri_vv(vv0, vv2, vv1, -1, 1, wire_flag, vertset);
	if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
		display_tri_vv(vv1, vv2, v3,  4*c+1, 1, wire_flag, vertset);
	if(abs(vert_bound[0]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
		display_tri_vv(vv2, vv0, v3,  4*c+2, 1, wire_flag, vertset);
	if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3)
		display_tri_vv(vv0, vv1, v3,  4*c+3, 1, wire_flag, vertset);
}
//
void MyDrawer::display_2(int* vert_bound, int c, float* v0, float* v1, float* v2,
			 float* v3,  int normalflag,
			 int wire_flag, FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_2.\n";

	float vv0[3], vv1[3], vv2[3], vv3[3], ratio_0, ratio_1, ratio_2, ratio_3;

	vv0[0] = plane_x;
	ratio_0 = (plane_x - v0[0]) / (v3[0] - v0[0]);
	vv0[1] = v0[1] + ratio_0 * (v3[1] - v0[1]);
	vv0[2] = v0[2] + ratio_0 * (v3[2] - v0[2]);

	vv1[0] = plane_x;
	ratio_1 = (plane_x - v1[0]) / (v3[0] - v1[0]);
	vv1[1] = v1[1] + ratio_1 * (v3[1] - v1[1]);
	vv1[2] = v1[2] + ratio_1 * (v3[2] - v1[2]);

	vv2[0] = plane_x;
	ratio_2 = (plane_x - v0[0]) / (v2[0] - v0[0]);
	vv2[1] = v0[1] + ratio_2 * (v2[1] - v0[1]);
	vv2[2] = v0[2] + ratio_2 * (v2[2] - v0[2]);

	vv3[0] = plane_x;
	ratio_3 = (plane_x - v1[0]) / (v2[0] - v1[0]);
	vv3[1] = v1[1] + ratio_3 * (v2[1] - v1[1]);
	vv3[2] = v1[2] + ratio_3 * (v2[2] - v1[2]);

	if(ratio_0 != 0.0 && ratio_1 == 0.0) {
		display_tri_vv(vv0, v1,  vv2, -1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) == 1)
			display_tri_vv(vv0, vv2, v0,  4*c+2, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv0, v0,  v1,  4*c+3, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3)
			display_tri_vv(vv2, v1,  v0,  4*c, 1, wire_flag, vertset);
	}
	if(ratio_0 == 0.0 && ratio_1 != 0.0) {
		display_tri_vv(vv1, vv3, v0,  -1, 1, wire_flag, vertset);
		if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv1, v1,  vv3, 4*c+1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv1, v0,  v1,  4*c+3, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3)
			display_tri_vv(vv3, v1,  v0,  4*c, 1, wire_flag, vertset);
	}
	if(ratio_0 != 0.0 && ratio_1 != 0.0) {
		display_tri_vv(vv0, vv1, vv2, -1, 1, wire_flag, vertset);
		display_tri_vv(vv1, vv3, vv2, -1, 1, wire_flag, vertset);
		if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv3, vv1, v1,  4*c+1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv0, vv2, v0,  4*c+2, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3) {
			display_tri_vv(vv1, vv0, v0,  4*c+3, 1, wire_flag, vertset);
			display_tri_vv(vv1, v0,  v1,  4*c+3, 1, wire_flag, vertset);
		}
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3) {
			display_tri_vv(vv2, vv3, v1,  4*c, 1, wire_flag, vertset);
			display_tri_vv(vv2, v1,  v0,  4*c, 1, wire_flag, vertset);
		}
	}
}
//
void MyDrawer::display_2_z(int* vert_bound, int c, float* v0, float* v1, float* v2,
			   float* v3,  int normalflag, int wire_flag,
			   FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_2_z.\n";

	float vv0[3], vv1[3], vv2[3], vv3[3], ratio_0, ratio_1, ratio_2, ratio_3;

	vv0[2] = plane_z;
	ratio_0 = (plane_z - v0[2]) / (v3[2] - v0[2]);
	vv0[1] = v0[1] + ratio_0 * (v3[1] - v0[1]);
	vv0[0] = v0[0] + ratio_0 * (v3[0] - v0[0]);

	vv1[2] = plane_z;
	ratio_1 = (plane_z - v1[2]) / (v3[2] - v1[2]);
	vv1[1] = v1[1] + ratio_1 * (v3[1] - v1[1]);
	vv1[0] = v1[0] + ratio_1 * (v3[0] - v1[0]);

	vv2[2] = plane_z;
	ratio_2 = (plane_z - v0[2]) / (v2[2] - v0[2]);
	vv2[1] = v0[1] + ratio_2 * (v2[1] - v0[1]);
	vv2[0] = v0[0] + ratio_2 * (v2[0] - v0[0]);

	vv3[2] = plane_z;
	ratio_3 = (plane_z - v1[2]) / (v2[2] - v1[2]);
	vv3[1] = v1[1] + ratio_3 * (v2[1] - v1[1]);
	vv3[0] = v1[0] + ratio_3 * (v2[0] - v1[0]);

	if(ratio_0 != 0.0 && ratio_1 == 0.0) {
		display_tri_vv(vv0, v1,  vv2, -1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) == 1)
			display_tri_vv(vv0, vv2, v0,  4*c+2, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv0, v0,  v1,  4*c+3, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3)
			display_tri_vv(vv2, v1,  v0,  4*c, 1, wire_flag, vertset);
	}
	if(ratio_0 == 0.0 && ratio_1 != 0.0) {
		display_tri_vv(vv1, vv3, v0,  -1, 1, wire_flag, vertset);
		if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv1, v1,  vv3, 4*c+1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv1, v0,  v1,  4*c+3, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3)
			display_tri_vv(vv3, v1,  v0,  4*c, 1, wire_flag, vertset);
	}
	if(ratio_0 != 0.0 && ratio_1 != 0.0) {
		display_tri_vv(vv0, vv1, vv2, -1, 1, wire_flag, vertset);
		display_tri_vv(vv1, vv3, vv2, -1, 1, wire_flag,vertset);
		if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv3, vv1, v1,  4*c+1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3)
			display_tri_vv(vv0, vv2, v0,  4*c+2, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[3]) == 3) {
			display_tri_vv(vv1, vv0, v0,  4*c+3, 1, wire_flag, vertset);
			display_tri_vv(vv1, v0,  v1,  4*c+3, 1, wire_flag, vertset);
		}
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3) {
			display_tri_vv(vv2, vv3, v1,  4*c, 1, wire_flag, vertset);
			display_tri_vv(vv2, v1,  v0,  4*c, 1, wire_flag, vertset);
		}
	}
}

//
void MyDrawer::display_3(int* vert_bound, int c, float* v0, float* v1, float* v2,
			 float* v3, int normalflag, int wire_flag,  FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_3.\n";

	float vv0[3], vv1[3], vv2[3], ratio_0, ratio_1, ratio_2;

	vv0[0] = plane_x;
	ratio_0 = (plane_x - v0[0]) / (v3[0] - v0[0]);
	vv0[1] = v0[1] + ratio_0 * (v3[1] - v0[1]);
	vv0[2] = v0[2] + ratio_0 * (v3[2] - v0[2]);

	vv1[0] = plane_x;
	ratio_1 = (plane_x - v1[0]) / (v3[0] - v1[0]);
	vv1[1] = v1[1] + ratio_1 * (v3[1] - v1[1]);
	vv1[2] = v1[2] + ratio_1 * (v3[2] - v1[2]);

	vv2[0] = plane_x;
	ratio_2 = (plane_x - v2[0]) / (v3[0] - v2[0]);
	vv2[1] = v2[1] + ratio_2 * (v3[1] - v2[1]);
	vv2[2] = v2[2] + ratio_2 * (v3[2] - v2[2]);


	if(ratio_0 <= 0.001 && ratio_1 <= 0.01 && ratio_2 <= 0.001) {
	//if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 0) {
		display_tri_vv(vv0, vv1, vv2, -1, 1, wire_flag, vertset);
	}
	else {
		display_tri_vv(vv0, vv1, vv2, -1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3)
			display_tri_vv(v0,  v2,  v1,  4*c, 1, wire_flag, vertset);
		if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3) {
			display_tri_vv(v1,  v2,  vv2, 4*c+1, 1, wire_flag, vertset);
			display_tri_vv(v1,  vv2, vv1, 4*c+1, 1, wire_flag, vertset);
		}
		if(abs(vert_bound[0]) + abs(vert_bound[2]) == 2) {
			display_tri_vv(v2,  v0,  vv2, 4*c+2, 1, wire_flag, vertset);
			display_tri_vv(vv2, v0,  vv0, 4*c+2, 1, wire_flag, vertset);
		}
		if(abs(vert_bound[0]) + abs(vert_bound[1]) == 2) {
			display_tri_vv(v1,  vv0, v0,  4*c+3, 1, wire_flag, vertset);
			display_tri_vv(v1,  vv1, vv0, 4*c+3, 1, wire_flag, vertset);
		}
	}
}
//
void MyDrawer::display_3_z(int* vert_bound, int c, float* v0, float* v1, float* v2,
			   float* v3, int normalflag, int wire_flag,
			   FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_3_z\n";

	float vv0[3], vv1[3], vv2[3], ratio_0, ratio_1, ratio_2;

	vv0[2] = plane_z;
	ratio_0 = (plane_z - v0[2]) / (v3[2] - v0[2]);
	vv0[1] = v0[1] + ratio_0 * (v3[1] - v0[1]);
	vv0[0] = v0[0] + ratio_0 * (v3[0] - v0[0]);

	vv1[2] = plane_z;
	ratio_1 = (plane_z - v1[2]) / (v3[2] - v1[2]);
	vv1[1] = v1[1] + ratio_1 * (v3[1] - v1[1]);
	vv1[0] = v1[0] + ratio_1 * (v3[0] - v1[0]);

	vv2[2] = plane_z;
	ratio_2 = (plane_z - v2[2]) / (v3[2] - v2[2]);
	vv2[1] = v2[1] + ratio_2 * (v3[1] - v2[1]);
	vv2[0] = v2[0] + ratio_2 * (v3[0] - v2[0]);

	if(ratio_0 == 0.0 && ratio_1 == 0.0 && ratio_2 == 0.0) {
		display_tri_vv(vv0, vv1, vv2, -1, 1, wire_flag, vertset);
	}
	else {
		display_tri_vv(vv0, vv1, vv2, -1, 1, wire_flag, vertset);
		if(abs(vert_bound[0]) + abs(vert_bound[1]) + abs(vert_bound[2]) == 3)
			display_tri_vv(v0,  v2,  v1,  4*c, 1, wire_flag, vertset);
		if(abs(vert_bound[1]) + abs(vert_bound[2]) + abs(vert_bound[3]) == 3) {
			display_tri_vv(v1,  v2,  vv2, 4*c+1, 1, wire_flag, vertset);
			display_tri_vv(v1,  vv2, vv1, 4*c+1, 1, wire_flag, vertset);
		}
		if(abs(vert_bound[0]) + abs(vert_bound[2]) == 2) {
			display_tri_vv(v2,  v0,  vv2, 4*c+2, 1, wire_flag, vertset);
			display_tri_vv(vv2, v0,  vv0, 4*c+2, 1, wire_flag, vertset);
		}
		if(abs(vert_bound[0]) + abs(vert_bound[1]) == 2) {
			display_tri_vv(v1,  vv1, vv0, 4*c+3, 1, wire_flag, vertset);
			display_tri_vv(v1,  vv0, v0,  4*c+3, 1, wire_flag, vertset);
		}
	}
}
//
void MyDrawer::display_tetra(int c, int normalflag, int wire_flag, IntArray *newfaces, FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_tetra\n";

	float v0[3], v1[3], v2[3], v3[3], v[4][3];
	int vert, ii, jj, num_le, num_eq, vert_bound[4];

	for(ii = 0; ii < 3; ii++) {
		vert = g_frame->triangles[4*c][ii];
		vert_bound[ii] = g_frame->bound_sign[vert];
		for(jj = 0; jj < 3; jj++)
			v[ii][jj] = g_frame->verts[vert][jj];
	}
	vert = g_frame->triangles[4*c+1][2];
	vert_bound[3] = g_frame->bound_sign[vert];
	for(jj = 0; jj < 3; jj++)
		v[3][jj] = g_frame->verts[vert][jj];

	num_le = 0;	num_eq = 0;
	for(ii = 0; ii < 4; ii++) {
		if(v[ii][0] <= plane_x) num_le++;
		if(v[ii][0] == plane_x) num_eq++;
	}

	assert(num_le >= 0 && num_le <= 4);
	for(ii = 0; ii < 3; ii++) {
		v0[ii] = v[0][ii];		v1[ii] = v[2][ii];
		v2[ii] = v[1][ii];		v3[ii] = v[3][ii];
	}

	if(num_le == 1) {
		display_permute_1(v0, v1, v2, v3);
		display_1(vert_bound, c, v0, v1, v2, v3, normalflag, wire_flag, vertset);
	}
	if(num_le == 2) {
		display_permute_2(v0, v1, v2, v3);
		display_2(vert_bound, c, v0, v1, v2, v3, normalflag, wire_flag,vertset);
	}
	if(num_le == 3) {
		display_permute_3(v0, v1, v2, v3);
		display_3(vert_bound, c, v0, v1, v2, v3, normalflag, wire_flag, vertset);
	}
	if(num_le == 4) {
		display_tri00(0, 1, 2, 4*c,   normalflag, wire_flag, num_eq, newfaces);
		display_tri00(0, 1, 2, 4*c+1, normalflag, wire_flag, num_eq, newfaces);
		display_tri00(0, 1, 2, 4*c+2, normalflag, wire_flag, num_eq, newfaces);
		display_tri00(0, 1, 2, 4*c+3, normalflag, wire_flag, num_eq, newfaces);
	}
}
//
void MyDrawer::display_tetra_in(int c, int normalflag, int wire_flag, IntArray *newfaces, FloatArray * vertset)
{ //std::cout<< "Inside MyDrawer. display_tetra_in.\n";

	float v0[3], v1[3], v2[3], v3[3], v[4][3];
	int vert, ii, jj, num_le, num_eq, vert_bound[4];

	for(ii = 0; ii < 3; ii++) {
		vert = g_frame->triangles[4*c][ii];
		vert_bound[ii] = g_frame->bound_sign[vert];
		for(jj = 0; jj < 3; jj++)
			v[ii][jj] = g_frame->verts[vert][jj];
	}
	vert = g_frame->triangles[4*c+1][2];
	vert_bound[3] = g_frame->bound_sign[vert];
	for(jj = 0; jj < 3; jj++)
		v[3][jj] = g_frame->verts[vert][jj];

	num_le = 0;	num_eq = 0;
	for(ii = 0; ii < 4; ii++) {
		if(v[ii][2] <= plane_z) num_le++;
		if(v[ii][2] == plane_z) num_eq++;
	}

	assert(num_le >= 0 && num_le <= 4);

	for(ii = 0; ii < 3; ii++) {
		v0[ii] = v[0][ii];		v1[ii] = v[2][ii];
		v2[ii] = v[1][ii];		v3[ii] = v[3][ii];
	}

	if( (v[0][2] >= plane_z && v[0][0] >= plane_x) || (v[1][2] >= plane_z && v[1][0] >= plane_x) ||
		(v[2][2] >= plane_z && v[2][0] >= plane_x) || (v[3][2] >= plane_z && v[3][0] >= plane_x)) {

		display_tetra(c, normalflag, wire_flag, newfaces, vertset);

		if(num_le == 1) {
			display_permute_1_z(v0, v1, v2, v3);
			display_1_z(vert_bound, c, v0, v1, v2, v3, normalflag, wire_flag, vertset);
		}
		if(num_le == 2) {
			display_permute_2_z(v0, v1, v2, v3);
			display_2_z(vert_bound, c, v0, v1, v2, v3, normalflag, wire_flag, vertset);
		}
		if(num_le == 3) {
			display_permute_3_z(v0, v1, v2, v3);
			display_3_z(vert_bound, c, v0, v1, v2, v3, normalflag, wire_flag, vertset);
		}
		if(num_le == 4)
		  {
		    display_tri00(0, 1, 2, 4*c,   normalflag, wire_flag, -num_eq, newfaces);
		    display_tri00(0, 1, 2, 4*c+1, normalflag, wire_flag, -num_eq, newfaces);
		    display_tri00(0, 1, 2, 4*c+2, normalflag, wire_flag, -num_eq, newfaces);
		    display_tri00(0, 1, 2, 4*c+3, normalflag, wire_flag, -num_eq, newfaces);
		  }

	}
	else {
		display_tri0(0, 1, 2, 4*c,   normalflag, wire_flag, newfaces);
		display_tri0(0, 1, 2, 4*c+1, normalflag, wire_flag, newfaces);
		display_tri0(0, 1, 2, 4*c+2, normalflag, wire_flag, newfaces);
		display_tri0(0, 1, 2, 4*c+3, normalflag, wire_flag, newfaces);
	}
}

void get_trinorm(float* tn, geoframe* g, int c, int normal_flag)
{ //std::cout<< "Inside MyDrawer. get_trinorm.\n";
	float v1[3],v2[3];
	int vert;

	vert = g->triangles[c][0];
	v1[0] = v2[0] = -g->verts[vert][0];
	v1[1] = v2[1] = -g->verts[vert][1];
	v1[2] = v2[2] = -g->verts[vert][2];

	vert = g->triangles[c][1];
	v1[0] += g->verts[vert][0];
	v1[1] += g->verts[vert][1];
	v1[2] += g->verts[vert][2];

	vert = g->triangles[c][2];
	v2[0] += g->verts[vert][0];
	v2[1] += g->verts[vert][1];
	v2[2] += g->verts[vert][2];

	cross(tn, v1, v2);


	// normal flipping
	if (normal_flag==1) {
		tn[0]=-tn[0];
		tn[1]=-tn[1];
		tn[2]=-tn[2];
	}

}
//
void MyDrawer::display(IntArray *newfaces, FloatArray * vertset)
{
  //std::cout<< "Inside MyDrawer. display. " << "cut_flag = " << cut_flag << "\n";
  int vert, c, index, ii, my_bool;
  //char* p;
  int normal_flag = 1; // 0, 1
  std::vector<int> f;
  //std::cout << "In Display : plane_x " << plane_x << ", plane_z " << plane_z << "\n";

  //if (p=strstr("Head",fn)) normal_flag = 0;
  vcount =0;

  if (g_frame)
    {

      if((g_frame->numhexas * 6) != g_frame->numquads) cut_flag = 0;

      if(cut_flag == 1)
	{
	  for(c = 0; c < ((g_frame->numtris)/4); c++)
	    {
	      display_tetra_in(c, normal_flag, 0, newfaces, vertset);
	      //display_tetra(c, normal_flag, 0);
	    }
	  for(c = 0; c < g_frame->numhexas; c++)
	    {
	      display_hexa(c, normal_flag, 0, newfaces);
	    }
	}
      else if(cut_flag == 2)
	{
	  for (c=0; c<((g_frame->numtris)/4); c++)
	    {
	      display_tetra_in(c, normal_flag, 0, newfaces, vertset);
	    }
	}
      else
	{
	  for (c = 0; c < g_frame->numtris; c++)
	    display_tri0(0, 1, 2, c, normal_flag, 0, newfaces);
	  //display_tri0(0, 1, 2, c, 1, 0);
	  for (c = 0; c < g_frame->numquads; c++)
	    {
	      //display_tri(0, 1, 2, c, normal_flag, newfaces);
	      //display_tri(2, 3, 0, c, normal_flag,  newfaces);
	      my_bool =	abs((int)g_frame->bound_sign[g_frame->quads[c][0]]) == 1 &&
		abs((int)g_frame->bound_sign[g_frame->quads[c][1]]) == 1 &&
		abs((int)g_frame->bound_sign[g_frame->quads[c][2]]) == 1 &&
		abs((int)g_frame->bound_sign[g_frame->quads[c][3]]) == 1;
	      if(my_bool)
		{
		  f.push_back(g_frame->quads[c][3]);
		  f.push_back(g_frame->quads[c][2]);
		  f.push_back(g_frame->quads[c][1]);
		  f.push_back(g_frame->quads[c][0]);
		  newfaces->push_back(f);
		  f.clear();
		}
	    }
	}

    }
}

void MyDrawer::setGeo(geoframe *g)
{ //std::cout<< "Inside MyDrawer. setGeo\n";
 g_frame = g;

}


void MyDrawer::setExtents(double biggest, double cx, double cy, double cz)
{ //std::cout<< "Inside MyDrawer. setExtents\n";

  biggestDim = biggest;
  centerx = cx;
  centery = cy;
  centerz = cz;
}



