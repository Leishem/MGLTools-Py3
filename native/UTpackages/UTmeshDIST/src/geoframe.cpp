#include "geoframe.h"
#include <stdio.h>
#include <math.h>

geoframe::geoframe()
{
	normals = 0;
	curvatures = 0;
	numverts = 0;
	numtris = 0;
	num_tris = 0;
	numquads = 0;
	numhexas = 0;
	
	vsize=100000;
	tsize=100000;
	qsize=100000;
	
	verts  = (float (*)[3])malloc(sizeof(float[3]) * vsize);
	funcs  = (float (*)[1])malloc(sizeof(float[1]) * vsize);
	normals  = (float (*)[3])malloc(sizeof(float[3]) * tsize);
	curvatures  = (float (*)[2])malloc(sizeof(float[2]) * tsize);
	triangles   = (unsigned int (*)[3])malloc(sizeof(unsigned int[3]) * tsize);
	quads = (unsigned int (*)[4])malloc(sizeof(unsigned int[4]) * qsize);
	bound_sign   = (unsigned int (*))malloc(sizeof(unsigned int) * vsize);
	bound_tri   = (unsigned int (*))malloc(sizeof(unsigned int) * tsize);
	vtxnew_sign   = (unsigned int (*))malloc(sizeof(unsigned int) * vsize);
	bound_edge   = (unsigned int (*)[18])malloc(sizeof(unsigned int[18]) * vsize);

	vtx_idx_arr_extend = (int*)malloc(sizeof(int)*1000000);
	int k;
	for (k = 0; k < 1000000; k++) vtx_idx_arr_extend[k] = -1;
}

geoframe::~geoframe()
{
	free(triangles);
	free(quads);
	free(verts);
	free(funcs);
  	free(normals);
  	free(curvatures);
	free(bound_sign);
	free(bound_tri);
	free(vtx_idx_arr_extend);
	free(vtxnew_sign);
	free(bound_edge);
}

/*
void geoframe::loadgeoframe(const char * name, int num)
{
  char buffer[256];
  
  sprintf(buffer, "%s%05d.raw", name, num );
  //sprintf(buffer, "%s", name);
  QFile file(buffer);

  file.open(IO_ReadOnly);
  QTextStream stream(&file);

  stream >> numverts >> numtris;
  verts = new double[numverts*3];
  triangles = new int[numtris*3];
  int c;
  //int min = 1<<30;
  for (c=0; c<numverts; c++) {
    stream >> verts[c*3+0];
    stream >> verts[c*3+1];
    stream >> verts[c*3+2];
  }
  for (c=0; c<numtris; c++) {
    stream >> triangles[c*3+0];
    //min = obj.faceArray[c*3+0]<min?obj.faceArray[c*3+0]:min;
    stream >> triangles[c*3+1];
    //min = obj.faceArray[c*3+0]<min?obj.faceArray[c*3+1]:min;
    stream >> triangles[c*3+2];
    //min = obj.faceArray[c*3+0]<min?obj.faceArray[c*3+2]:min;
  }
  calculatenormals();
  calculateExtents();
}
*/

void cross(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[1]*v2[2] - v1[2]*v2[1];
	dest[1] = v1[2]*v2[0] - v1[0]*v2[2];
	dest[2] = v1[0]*v2[1] - v1[1]*v2[0];
}


void geoframe::calculateTriangleNormal(float* norm, unsigned int c)
{
	
  float v1[3], v2[3];
  int vert;
  vert = triangles[c][0];
  v1[0] = v2[0] = -verts[vert][0];
  v1[1] = v2[1] = -verts[vert][1];
  v1[2] = v2[2] = -verts[vert][2];

  vert = triangles[c][1];
  v1[0] += verts[vert][0];
  v1[1] += verts[vert][1];
  v1[2] += verts[vert][2];

  vert = triangles[c][2];
  v2[0] += verts[vert][0];
  v2[1] += verts[vert][1];
  v2[2] += verts[vert][2];

  cross(norm, v1, v2);
  
}

void geoframe::calculatenormals()
{
	
	
	int c, vert;
	float normal[3];
	float len;
	
	// for each triangle
	for (c=0; c<numtris; c++) {
		calculateTriangleNormal(normal, c);
		normals[c][0] = normal[0];
		normals[c][1] = normal[1];
		normals[c][2] = normal[2];
	}
	
	// normalize the vectors
	for (vert=0; vert<numtris; vert++) {
		len = (float) sqrt(
			normals[vert][0] * normals[vert][0] +
			normals[vert][1] * normals[vert][1] +
			normals[vert][2] * normals[vert][2]);
		normals[vert][0]/=len;
		normals[vert][1]/=len;
		normals[vert][2]/=len;
	}
	
}

void geoframe::calculateExtents()
{
  int c;
  float max_x, min_x;
  float max_y, min_y;
  float max_z, min_z;
  float value;

  for (c=0; c<numverts; c++) {
    if (c==0) {
      max_x = min_x = verts[c][0];
      max_y = min_y = verts[c][1];
      max_z = min_z = verts[c][2];
    }
    else {
      value = verts[c][0];
      max_x = (value>max_x?value:max_x);
      min_x = (value<min_x?value:min_x);

      value = verts[c][1];
      max_y = (value>max_y?value:max_y);
      min_y = (value<min_y?value:min_y);

      value = verts[c][2];
      max_z = (value>max_z?value:max_z);
      min_z = (value<min_z?value:min_z);
    }
  }

    biggestDim = (max_y-min_y>max_x-min_x?max_y-min_y:max_x-min_x);
    biggestDim = (max_z-min_z>biggestDim?max_z-min_z:biggestDim);
    centerx = (max_x+min_x)/2.0;
    centery = (max_y+min_y)/2.0;
    centerz = (max_z+min_z)/2.0;
}

/*void geoframe::display()
{
  int vert;
  glBegin(GL_TRIANGLES);
  for (int c=0; c<numtris; c++) {
    vert = triangles[c*3+0];
    glNormal3d( 
      normals[vert*3+0],
      normals[vert*3+1],
      normals[vert*3+2]);
    glVertex3d(
      verts[vert*3+0],
      verts[vert*3+1],
      verts[vert*3+2]);
    vert = triangles[c*3+1];
    glNormal3d( 
      normals[vert*3+0],
      normals[vert*3+1],
      normals[vert*3+2]);
    glVertex3d(
      verts[vert*3+0],
      verts[vert*3+1],
      verts[vert*3+2]);
    vert = triangles[c*3+2];
    glNormal3d( 
      normals[vert*3+0],
      normals[vert*3+1],
      normals[vert*3+2]);
    glVertex3d(
      verts[vert*3+0],
      verts[vert*3+1],
      verts[vert*3+2]);
  }
  glEnd();
}*/

void geoframe::read_raw(const char * rawiv_fname) {
	
	float temp0, temp1, temp2;
	int nv, ntri, t0, t1, t2;
	FILE* vol_fp = fopen(rawiv_fname,"r");
	
	if (vol_fp==NULL) {
		printf("wrong name : %s\n",rawiv_fname);
		return;
	}

	fscanf(vol_fp,"%d %d\n", &nv, &ntri);
	numverts = nv;
	numtris = ntri;

	verts  = (float (*)[3])malloc(sizeof(float[3]) * nv);
	triangles   = (unsigned int (*)[3])malloc(sizeof(unsigned int[3]) * ntri);
	int i;
	for (i=0;i<nv;i++) {
		fscanf(vol_fp,"%f %f %f\n", &temp0, &temp1, &temp2);
		verts[i][0] = temp0;
		verts[i][1] = temp1;
		verts[i][2] = temp2;
	}
	for (i=0;i<ntri;i++) {
		fscanf(vol_fp,"%d %d %d\n", &t0, &t1, &t2);
		triangles[i][0] = t0;
		triangles[i][1] = t1;
		triangles[i][2] = t2;
	}
	fclose(vol_fp);

}
