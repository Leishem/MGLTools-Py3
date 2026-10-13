#include "contour3d.h"
#include <malloc.h>
#include <stdio.h>

Contour3d::Contour3d()
{
	nvert=0;
	ntri=0;

	vsize=500;
	tsize=1000;

	vert  = (float (*)[3])malloc(sizeof(float[3]) * vsize);
	tri   = (unsigned int (*)[3])malloc(sizeof(unsigned int[3]) * tsize);
}

Contour3d::~Contour3d()
{
	free(vert);
	free(tri);
}

int Contour3d::AddVert(float v_pos[3], float norm[3])
{
	int i;
	if (nvert+1 > vsize) {
		vsize<<=1;
		vert = (float (*)[3])realloc(vert,sizeof(float[3])*vsize);
	}
	for (i=0;i<3;i++)
		vert[nvert][i]=v_pos[i];
	return nvert++;
}

int Contour3d::AddTri(unsigned int v1, unsigned int v2, unsigned int v3)
{

   if (ntri+1 > tsize) {
      tsize<<=1;
      tri = (unsigned int (*)[3])realloc(tri, sizeof(unsigned int[3]) * tsize);
   }

   tri[ntri][0] = v1;
   tri[ntri][1] = v2;
   tri[ntri][2] = v3;

   return ntri++;
}

void Contour3d::Clear()
{
	nvert=0;
	ntri=0;
}

void Contour3d::display()
{
	int i;
	
	/*
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

	*/










	/*
	FILE* out_fp=fopen("output.smf","w+");

	fprintf(out_fp,"#Inventor V2.1 ascii\n");
	fprintf(out_fp,"Separator {\n");
	fprintf(out_fp,"BaseColor{\n");
	fprintf(out_fp,"rgb 0.1 0.45 0.2 }\n");
	fprintf(out_fp,"MaterialBinding { value OVERALL }\n");
	fprintf(out_fp,"Coordinate3 {\n");
	fprintf(out_fp,"point [\n");

	for (i=0;i<nvert-1;i++) {
		fprintf(out_fp,"%f %f %f,\n",vert[i][0],vert[i][1],vert[i][2]);
	}
	fprintf(out_fp,"%f %f %f\n",vert[i][0],vert[i][1],vert[i][2]);
	fprintf(out_fp,"]\n}\n");
	fprintf(out_fp,"ShapeHints{ vertexOrdering  COUNTERCLOCKWISE }\n");
	fprintf(out_fp,"IndexedFaceSet {\n coordIndex [\n");

	for (i=0;i<ntri-1;i++) {
		fprintf(out_fp,"%d, %d, %d, -1,\n",tri[i][1],tri[i][0],tri[i][2]);
		//fprintf(out_fp,"%d, %d, %d, -1,\n",tri[i][0],tri[i][1],tri[i][2]);
	}
	fprintf(out_fp,"%d, %d, %d, -1\n",tri[i][1],tri[i][0],tri[i][2]);
	//fprintf(out_fp,"%d, %d, %d, -1\n",tri[i][0],tri[i][1],tri[i][2]);
	fprintf(out_fp,"]\n}\n}\n");


	*/

/*  remove this

	for (i=0;i<nvert;i++) {
		fprintf(out_fp,"v %f %f %f\n",vert[i][0],vert[i][1],vert[i][2]);
	}
	for (i=0;i<ntri;i++) {
		fprintf(out_fp,"f %d %d %d\n",tri[i][0]+1,tri[i][1]+1,tri[i][2]+1);
		//fprintf(out_fp,"f %d %d %d\n",tri[i][1]+1,tri[i][0]+1,tri[i][2]+1);
	}
*/

	//fclose(out_fp);
}

int Contour3d::center_vtx(int v1,int v2,int v3)
{
	float center_vtx[3],norm[3];
	center_vtx[0]=(vert[v1][0] + vert[v2][0] + vert[v3][0])/3.;
	center_vtx[1]=(vert[v1][1] + vert[v2][1] + vert[v3][1])/3.;
	center_vtx[2]=(vert[v1][2] + vert[v2][2] + vert[v3][2])/3.;
	return AddVert(center_vtx,norm);
}
