#ifndef __CONTOUR3D_H__
#define __CONTOUR3D_H__


class Contour3d
{
	int nvert, ntri, nquad;
	int vsize,tsize;
	float (*vert)[3];
	unsigned int (*tri)[3];
	unsigned int (*quad)[4];
	

public :
	Contour3d();
	~Contour3d();

	void Clear();
	int getNTri(void) { return ntri; }
	int getNQuad(void) { return nquad; }
	int getNVert(void) { return nvert; }
	int AddTri(unsigned int v1,unsigned int v2,unsigned int v3);
	int AddVert(float v_pos[3], float norm[3]);
	int center_vtx(int v1,int v2,int v3);
	void display();
};
#endif __CONTOUR3D_H__
