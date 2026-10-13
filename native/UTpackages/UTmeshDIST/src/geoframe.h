#ifndef __GEOFRAME_H__
#define __GEOFRAME_H__

#include<stdio.h>
#include<malloc.h>
#include<assert.h>
#include<math.h>

#define GRAD

class geoframe {
public:
	geoframe();
	~geoframe();
	
	void Clear() { numtris=0; numverts=0; numquads=0; numhexas = 0;}
	int getNTri(void) { return numtris; }
	int getNQuad(void) { return numquads; }
	int getNHexa(void) { return numhexas; }
	int getNVert(void) { return numverts; }

	int TestNum(unsigned int* v) {
		int i;
		float dis_0, dis_1, dis_2, dis_3;

		dis_0 = 0.0;	dis_1 = 0.0;
		dis_2 = 0.0;	dis_3 = 0.0;
		for(i = 0; i < 3; i++) {
			dis_0 += (verts[v[1]][i] - verts[v[0]][i])*(verts[v[1]][i] - verts[v[0]][i]);
			dis_1 += (verts[v[2]][i] - verts[v[1]][i])*(verts[v[2]][i] - verts[v[1]][i]);
			dis_2 += (verts[v[3]][i] - verts[v[2]][i])*(verts[v[3]][i] - verts[v[2]][i]);
			dis_3 += (verts[v[0]][i] - verts[v[3]][i])*(verts[v[0]][i] - verts[v[3]][i]);
		}
		dis_0 = (float)sqrt(dis_0);	dis_1 = (float)sqrt(dis_1);
		dis_2 = (float)sqrt(dis_2);	dis_3 = (float)sqrt(dis_3);

		if(dis_0 == 0.0 || dis_1 == 0.0) {
			//v[1] = v[2];	v[2] = v[3];
			num_tris++;
			return 3;
		}
		else if(dis_2 == 0.0 || dis_3 == 0.0) {
			//v[3] = v[1];	v[1] = v[2];	v[2] = v[3];
			num_tris++;
			return 3;
		}
		else
			return 4;
	}

	int AddQuad(unsigned int* v , int num)
	{
		assert (num==3 || num==4);
		num = TestNum(v);
		num = 4;

		if (numquads >= qsize) {
			qsize<<=1;
			quads = (unsigned int (*)[4])realloc(quads, sizeof(unsigned int[4]) * qsize);
		}

		if (num == 4) {
			quads[numquads][0] = v[0];
			quads[numquads][1] = v[1];
			quads[numquads][2] = v[2];
			quads[numquads][3] = v[3];
			return numquads++;
		} 
		else  {    // (num==3)
			triangles[numtris][0] = v[0];
			triangles[numtris][1] = v[1];
			triangles[numtris][2] = v[2];
			return numtris++;
		}
		
	}

	void AddQuad_indirect(unsigned int* v)
	{
		float pv[3], pv0[3], pv1[3], pv2[3], pv3[3];
		float nv[3], nv0[3], nv1[3], nv2[3], nv3[3];
		int i;
		unsigned int v_new[5], v_quad[4];

		for(i = 0; i < 3; i++) {
			pv0[i] = (verts[v[0]][i] + verts[v[1]][i]) / 2.0f;
			pv1[i] = (verts[v[1]][i] + verts[v[2]][i]) / 2.0f;
			pv2[i] = (verts[v[2]][i] + verts[v[3]][i]) / 2.0f;
			pv3[i] = (verts[v[3]][i] + verts[v[0]][i]) / 2.0f;

			nv0[i] = (normals[v[0]][i] + normals[v[1]][i]) / 2.0f;
			nv1[i] = (normals[v[1]][i] + normals[v[2]][i]) / 2.0f;
			nv2[i] = (normals[v[2]][i] + normals[v[3]][i]) / 2.0f;
			nv3[i] = (normals[v[3]][i] + normals[v[0]][i]) / 2.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);

		if(v[0] == v[1]) {
			for(i = 0; i < 3; i++) {
				pv[i] = (verts[v[0]][i] + 2.0f*verts[v_new[2]][i]) / 3.0f;
				nv[i] = (normals[v[0]][i] + 2.0f*normals[v_new[2]][i]) / 3.0f;
			}
			v_new[4] = AddVert(pv, nv);		AddBound(v_new[4], 1);
			v_quad[0] = v[0];		v_quad[1] = v_new[1];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[3];	AddQuad(v_quad, 4);

			v_quad[0] = v[2];		v_quad[1] = v_new[2];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[1];	AddQuad(v_quad, 4);

			v_quad[0] = v[3];		v_quad[1] = v_new[3];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[2];	AddQuad(v_quad, 4);
		}
		else if(v[1] == v[2]) {
			for(i = 0; i < 3; i++) {
				pv[i] = (verts[v[1]][i] + 2.0f*verts[v_new[3]][i]) / 3.0f;
				nv[i] = (normals[v[1]][i] + 2.0f*normals[v_new[3]][i]) / 3.0f;
			}
			v_new[4] = AddVert(pv, nv);		AddBound(v_new[4], 1);
			v_quad[0] = v[0];		v_quad[1] = v_new[0];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[3];	AddQuad(v_quad, 4);
			
			v_quad[0] = v[1];		v_quad[1] = v_new[2];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[0];	AddQuad(v_quad, 4);

			v_quad[0] = v[3];		v_quad[1] = v_new[3];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[2];	AddQuad(v_quad, 4);
		}
		else if(v[2] == v[3]) {
			for(i = 0; i < 3; i++) {
				pv[i] = (verts[v[2]][i] + 2.0f*verts[v_new[0]][i]) / 3.0f;
				nv[i] = (normals[v[2]][i] + 2.0f*normals[v_new[0]][i]) / 3.0f;
			}
			v_new[4] = AddVert(pv, nv);		AddBound(v_new[4], 1);
			v_quad[0] = v[0];		v_quad[1] = v_new[0];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[3];	AddQuad(v_quad, 4);
			
			v_quad[0] = v[1];		v_quad[1] = v_new[1];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[0];	AddQuad(v_quad, 4);

			v_quad[0] = v[2];		v_quad[1] = v_new[3];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[1];	AddQuad(v_quad, 4);
		}
		else if(v[3] == v[0]) {
			for(i = 0; i < 3; i++) {
				pv[i] = (verts[v[0]][i] + 2.0f*verts[v_new[1]][i]) / 3.0f;
				nv[i] = (normals[v[0]][i] + 2.0f*normals[v_new[1]][i]) / 3.0f;
			}
			v_new[4] = AddVert(pv, nv);		AddBound(v_new[4], 1);
			v_quad[0] = v[0];		v_quad[1] = v_new[0];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[2];	AddQuad(v_quad, 4);
			
			v_quad[0] = v[1];		v_quad[1] = v_new[1];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[0];	AddQuad(v_quad, 4);

			v_quad[0] = v[2];		v_quad[1] = v_new[2];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[1];	AddQuad(v_quad, 4);
		}
		else {
			for(i = 0; i < 3; i++) {
				pv[i] = (verts[v[0]][i] + verts[v[1]][i] + verts[v[2]][i] + verts[v[3]][i]) / 4.0f;
				nv[i] = (normals[v[0]][i] + normals[v[1]][i] + normals[v[2]][i] + normals[v[3]][i]) / 4.0f;
			}
			v_new[4] = AddVert(pv, nv);		AddBound(v_new[4], 1);
			v_quad[0] = v[0];		v_quad[1] = v_new[0];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[3];	AddQuad(v_quad, 4);
			
			v_quad[0] = v[1];		v_quad[1] = v_new[1];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[0];	AddQuad(v_quad, 4);

			v_quad[0] = v[2];		v_quad[1] = v_new[2];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[1];	AddQuad(v_quad, 4);

			v_quad[0] = v[3];		v_quad[1] = v_new[3];
			v_quad[2] = v_new[4];	v_quad[3] = v_new[2];	AddQuad(v_quad, 4);
		}
	}

	void AddVert_adaptive(unsigned int* v , unsigned int* v_new)
	{
		float pv[3], pv0[3], pv1[3], pv2[3], pv3[3];
		float nv[3], nv0[3], nv1[3], nv2[3], nv3[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv[i] = (verts[v[0]][i] + verts[v[1]][i] + verts[v[2]][i] + verts[v[3]][i]) / 4.0f;
			pv0[i] = (verts[v[0]][i] + 2.0f*pv[i]) / 3.0f;
			pv1[i] = (verts[v[1]][i] + 2.0f*pv[i]) / 3.0f;
			pv2[i] = (verts[v[2]][i] + 2.0f*pv[i]) / 3.0f;
			pv3[i] = (verts[v[3]][i] + 2.0f*pv[i]) / 3.0f;

			nv[i] = (normals[v[0]][i] + normals[v[1]][i] + normals[v[2]][i] + normals[v[3]][i]) / 4.0f;
			nv0[i] = (normals[v[0]][i] + 2.0f*nv[i]) / 3.0f;
			nv1[i] = (normals[v[1]][i] + 2.0f*nv[i]) / 3.0f;
			nv2[i] = (normals[v[2]][i] + 2.0f*nv[i]) / 3.0f;
			nv3[i] = (normals[v[3]][i] + 2.0f*nv[i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
	}

	void AddQuad_adaptive(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[4], v_quad[4];
		int i;

		for(i = 0; i < 4; i++) vv[i] = v_new[i];

		v_quad[0] =  v[0];	v_quad[1] =  v[1];
		v_quad[2] = vv[1];	v_quad[3] = vv[0];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] =  v[2];
		v_quad[2] = vv[2];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] =  v[3];
		v_quad[2] = vv[3];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] =  v[0];
		v_quad[2] = vv[0];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		v_quad[0] = vv[0];	v_quad[1] = vv[1];
		v_quad[2] = vv[2];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_2_1(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3], nv5[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[1]][i] + verts[v[0]][i]) / 3.0f;
			pv2[i] = (pv1[i] + 2.0f*(2.0f*verts[v[2]][i] + verts[v[3]][i])/3.0f) / 3.0f;
			pv3[i] = (2.0f*pv0[i] + (2.0f*verts[v[3]][i] + verts[v[2]][i])/3.0f) / 3.0f;
			pv4[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv5[i] = (2.0f*verts[v[3]][i] + verts[v[0]][i]) / 3.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[1]][i] + normals[v[0]][i]) / 3.0f;
			nv2[i] = (nv1[i] + 2.0f*(2.0f*normals[v[2]][i] + normals[v[3]][i])/3.0f) / 3.0f;
			nv3[i] = (2.0f*nv0[i] + (2.0f*normals[v[3]][i] + normals[v[2]][i])/3.0f) / 3.0f;
			nv4[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv5[i] = (2.0f*normals[v[3]][i] + normals[v[0]][i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		v_new[4] = AddVert(pv4, nv4);
		v_new[5] = AddVert(pv5, nv5);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	AddBound(v_new[5], 1);
	}

	void AddQuad_adaptive_2_1(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[6], v_quad[4];
		int i;

		for(i = 0; i < 6; i++) vv[i] = v_new[i];

		v_quad[0] = vv[0];	v_quad[1] = vv[1];
		v_quad[2] = vv[2];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] =  v[2];
		v_quad[2] = vv[2];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] =  v[3];
		v_quad[2] = vv[5];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[3];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] = vv[2];	v_quad[1] = vv[5];
		v_quad[2] = vv[4];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_2_3(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3], pv6[3], pv7[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3], nv5[3], nv6[3], nv7[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[1]][i] + verts[v[0]][i]) / 3.0f;
			pv2[i] = (2.0f*verts[v[1]][i] + verts[v[2]][i]) / 3.0f;
			pv3[i] = (2.0f*verts[v[2]][i] + verts[v[1]][i]) / 3.0f;
			pv4[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv5[i] = (2.0f*verts[v[3]][i] + verts[v[0]][i]) / 3.0f;
			pv6[i] = (5.0f*pv1[i] + (2.0f*verts[v[2]][i] + verts[v[3]][i])/3.0f) / 6.0f;
			pv7[i] = (5.0f*pv0[i] + (2.0f*verts[v[3]][i] + verts[v[2]][i])/3.0f) / 6.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[1]][i] + normals[v[0]][i]) / 3.0f;
			nv2[i] = (2.0f*normals[v[1]][i] + normals[v[2]][i]) / 3.0f;
			nv3[i] = (2.0f*normals[v[2]][i] + normals[v[1]][i]) / 3.0f;
			nv4[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv5[i] = (2.0f*normals[v[3]][i] + normals[v[0]][i]) / 3.0f;
			nv6[i] = (5.0f*nv1[i] + (2.0f*normals[v[2]][i] + normals[v[3]][i])/3.0f) / 6.0f;
			nv7[i] = (5.0f*nv0[i] + (2.0f*normals[v[3]][i] + normals[v[2]][i])/3.0f) / 6.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		v_new[4] = AddVert(pv4, nv4);
		v_new[5] = AddVert(pv5, nv5);
		v_new[6] = AddVert(pv6, nv6);
		v_new[7] = AddVert(pv7, nv7);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	AddBound(v_new[5], 1);
		AddBound(v_new[6], 1);	AddBound(v_new[7], 1);
		
	}

	void AddQuad_adaptive_2_3(unsigned int* v, unsigned int* v_new, int num)
	{
		unsigned int vv[8], v_quad[4];
		int i;

		for(i = 0; i < 8; i++) vv[i] = v_new[i];

		v_quad[0] = vv[0];	v_quad[1] = vv[1];
		v_quad[2] = vv[6];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[2];
		v_quad[2] = vv[6];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[7];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] = vv[6];	v_quad[1] = vv[2];
		v_quad[2] = vv[4];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		v_quad[0] = vv[2];	v_quad[1] = vv[3];
		v_quad[2] = vv[5];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] =  v[3];
		v_quad[2] = vv[5];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_4(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3], pv6[3], pv7[3], pv8[3], pv9[3], pv10[3], pv11[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3], nv5[3], nv6[3], nv7[3], nv8[3], nv9[3], nv10[3], nv11[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[1]][i] + verts[v[0]][i]) / 3.0f;
			pv2[i] = (2.0f*verts[v[1]][i] + verts[v[2]][i]) / 3.0f;
			pv3[i] = (2.0f*verts[v[2]][i] + verts[v[1]][i]) / 3.0f;
			pv4[i] = (2.0f*verts[v[2]][i] + verts[v[3]][i]) / 3.0f;
			pv5[i] = (2.0f*verts[v[3]][i] + verts[v[2]][i]) / 3.0f;
			pv6[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv7[i] = (2.0f*verts[v[3]][i] + verts[v[0]][i]) / 3.0f;
			pv8[i] = (2.0f*pv0[i] + pv5[i]) / 3.0f;
			pv9[i] = (2.0f*pv5[i] + pv0[i]) / 3.0f;
			pv10[i] = (2.0f*pv1[i] + pv4[i]) / 3.0f;
			pv11[i] = (2.0f*pv4[i] + pv1[i]) / 3.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[1]][i] + normals[v[0]][i]) / 3.0f;
			nv2[i] = (2.0f*normals[v[1]][i] + normals[v[2]][i]) / 3.0f;
			nv3[i] = (2.0f*normals[v[2]][i] + normals[v[1]][i]) / 3.0f;
			nv4[i] = (2.0f*normals[v[2]][i] + normals[v[3]][i]) / 3.0f;
			nv5[i] = (2.0f*normals[v[3]][i] + normals[v[2]][i]) / 3.0f;
			nv6[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv7[i] = (2.0f*normals[v[3]][i] + normals[v[0]][i]) / 3.0f;
			nv8[i] = (2.0f*nv0[i] + nv5[i]) / 3.0f;
			nv9[i] = (2.0f*nv5[i] + nv0[i]) / 3.0f;
			nv10[i] = (2.0f*nv1[i] + nv4[i]) / 3.0f;
			nv11[i] = (2.0f*nv4[i] + nv1[i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		v_new[4] = AddVert(pv4, nv4);
		v_new[5] = AddVert(pv5, nv5);
		v_new[6] = AddVert(pv6, nv6);
		v_new[7] = AddVert(pv7, nv7);
		v_new[8] = AddVert(pv8, nv8);
		v_new[9] = AddVert(pv9, nv9);
		v_new[10] = AddVert(pv10, nv10);
		v_new[11] = AddVert(pv11, nv11);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	AddBound(v_new[5], 1);
		AddBound(v_new[6], 1);	AddBound(v_new[7], 1);
		AddBound(v_new[8], 1);	AddBound(v_new[9], 1);
		AddBound(v_new[10], 1); AddBound(v_new[11], 1);
		
	}

	void AddQuad_adaptive_4(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[12], v_quad[4];
		int i;

		for(i = 0; i < 12; i++) vv[i] = v_new[i];

		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[8];	v_quad[3] = vv[6];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[2];
		v_quad[2] = vv[10];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[4];
		v_quad[2] = vv[11];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[7];
		v_quad[2] = vv[9];	v_quad[3] = vv[5];
		AddQuad(v_quad, num);
		v_quad[0] = vv[0];	v_quad[1] = vv[1];
		v_quad[2] = vv[10];	v_quad[3] = vv[8];
		AddQuad(v_quad, num);
		v_quad[0] = vv[2];	v_quad[1] = vv[3];
		v_quad[2] = vv[11];	v_quad[3] = vv[10];
		AddQuad(v_quad, num);
		v_quad[0] = vv[4];	v_quad[1] = vv[5];
		v_quad[2] = vv[9];	v_quad[3] = vv[11];
		AddQuad(v_quad, num);
		v_quad[0] = vv[6];	v_quad[1] = vv[8];
		v_quad[2] = vv[9];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		v_quad[0] = vv[8];	v_quad[1] = vv[10];
		v_quad[2] = vv[11];	v_quad[3] = vv[9];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_3_1(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], nv0[3], nv1[3], nv2[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*pv0[i] + (2.0f*verts[v[3]][i] + verts[v[2]][i])/3.0f) / 3.0f;
			pv2[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*nv0[i] + (2.0f*normals[v[3]][i] + normals[v[2]][i])/3.0f) / 3.0f;
			nv2[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	
	}

	void AddQuad_adaptive_3_1(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[3], v_quad[4];
		int i;

		for(i = 0; i < 3; i++) vv[i] = v_new[i];

		v_quad[0] =  v[1];	v_quad[1] =  v[2];
		v_quad[2] = vv[1];	v_quad[3] = vv[0];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] =  v[3];
		v_quad[2] = vv[2];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[1];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_3_2a(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3], pv6[3], pv7[3], pv8[3], pv9[3], pv10[3], pv11[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3], nv5[3], nv6[3], nv7[3], nv8[3], nv9[3], nv10[3], nv11[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[1]][i] + verts[v[0]][i]) / 3.0f;
			pv2[i] = (2.0f*verts[v[1]][i] + verts[v[2]][i]) / 3.0f;
			pv3[i] = (2.0f*verts[v[2]][i] + verts[v[1]][i]) / 3.0f;
			pv4[i] = (2.0f*verts[v[2]][i] + verts[v[3]][i]) / 3.0f;
			pv5[i] = (2.0f*verts[v[3]][i] + verts[v[2]][i]) / 3.0f;
			pv6[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv7[i] = (2.0f*verts[v[3]][i] + verts[v[0]][i]) / 3.0f;
			pv8[i] = (2.0f*pv0[i] + pv5[i]) / 3.0f;
			pv9[i] = (2.0f*pv5[i] + pv0[i]) / 3.0f;
			pv10[i] = (2.0f*pv1[i] + pv4[i]) / 3.0f;
			pv11[i] = (2.0f*pv4[i] + pv1[i]) / 3.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[1]][i] + normals[v[0]][i]) / 3.0f;
			nv2[i] = (2.0f*normals[v[1]][i] + normals[v[2]][i]) / 3.0f;
			nv3[i] = (2.0f*normals[v[2]][i] + normals[v[1]][i]) / 3.0f;
			nv4[i] = (2.0f*normals[v[2]][i] + normals[v[3]][i]) / 3.0f;
			nv5[i] = (2.0f*normals[v[3]][i] + normals[v[2]][i]) / 3.0f;
			nv6[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv7[i] = (2.0f*normals[v[3]][i] + normals[v[0]][i]) / 3.0f;
			nv8[i] = (2.0f*nv0[i] + nv5[i]) / 3.0f;
			nv9[i] = (2.0f*nv5[i] + nv0[i]) / 3.0f;
			nv10[i] = (2.0f*nv1[i] + nv4[i]) / 3.0f;
			nv11[i] = (2.0f*nv4[i] + nv1[i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv10, nv10);
		v_new[4] = AddVert(pv8, nv8);
		v_new[5] = AddVert(pv6, nv6);
		v_new[6] = AddVert(pv11, nv11);
		v_new[7] = AddVert(pv9, nv9);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	AddBound(v_new[5], 1);
		AddBound(v_new[6], 1);	AddBound(v_new[7], 1);
		
	}

	void AddQuad_adaptive_3_2a(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[8], v_quad[4];
		int i;

		for(i = 0; i < 8; i++) vv[i] = v_new[i];

		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[4];	v_quad[3] = vv[5];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[2];
		v_quad[2] = vv[3];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[6];
		v_quad[2] = vv[3];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[5];
		v_quad[2] = vv[4];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		v_quad[0] = vv[0];	v_quad[1] = vv[1];
		v_quad[2] = vv[3];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] = vv[3];	v_quad[1] = vv[6];
		v_quad[2] = vv[7];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] = vv[7];	v_quad[1] = vv[6];
		v_quad[2] = v[2];	v_quad[3] = v[3];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_3_2b(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[2]][i] + verts[v[1]][i]) / 3.0f;
			pv2[i] = (2.0f*verts[v[2]][i] + verts[v[3]][i]) / 3.0f;
			pv3[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv4[i] = (verts[v[0]][i] + verts[v[1]][i] + verts[v[2]][i] + verts[v[3]][i]) / 4.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[2]][i] + normals[v[1]][i]) / 3.0f;
			nv2[i] = (2.0f*normals[v[2]][i] + normals[v[3]][i]) / 3.0f;
			nv3[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv4[i] = (normals[v[0]][i] + normals[v[1]][i] + normals[v[2]][i] + normals[v[3]][i]) / 43.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		v_new[4] = AddVert(pv4, nv4);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	
		
	}

	void AddQuad_adaptive_3_2b(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[5], v_quad[4];
		int i;

		for(i = 0; i < 5; i++) vv[i] = v_new[i];

		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[4];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[1];
		v_quad[2] = vv[4];	v_quad[3] = vv[0];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[2];
		v_quad[2] = vv[4];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[3];
		v_quad[2] = vv[4];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_3_3(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3], pv6[3], pv7[3], pv8[3], pv9[3], pv10[3], pv11[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3], nv5[3], nv6[3], nv7[3], nv8[3], nv9[3], nv10[3], nv11[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[1]][i] + verts[v[0]][i]) / 3.0f;
			pv2[i] = (2.0f*verts[v[1]][i] + verts[v[2]][i]) / 3.0f;
			pv3[i] = (2.0f*verts[v[2]][i] + verts[v[1]][i]) / 3.0f;
			pv4[i] = (2.0f*verts[v[2]][i] + verts[v[3]][i]) / 3.0f;
			pv5[i] = (2.0f*verts[v[3]][i] + verts[v[2]][i]) / 3.0f;
			pv6[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv7[i] = (2.0f*verts[v[3]][i] + verts[v[0]][i]) / 3.0f;
			pv8[i] = (2.0f*pv0[i] + pv5[i]) / 3.0f;
			pv9[i] = (2.0f*pv5[i] + pv0[i]) / 3.0f;
			pv10[i] = (2.0f*pv1[i] + pv4[i]) / 3.0f;
			pv11[i] = (2.0f*pv4[i] + pv1[i]) / 3.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[1]][i] + normals[v[0]][i]) / 3.0f;
			nv2[i] = (2.0f*normals[v[1]][i] + normals[v[2]][i]) / 3.0f;
			nv3[i] = (2.0f*normals[v[2]][i] + normals[v[1]][i]) / 3.0f;
			nv4[i] = (2.0f*normals[v[2]][i] + normals[v[3]][i]) / 3.0f;
			nv5[i] = (2.0f*normals[v[3]][i] + normals[v[2]][i]) / 3.0f;
			nv6[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv7[i] = (2.0f*normals[v[3]][i] + normals[v[0]][i]) / 3.0f;
			nv8[i] = (2.0f*nv0[i] + nv5[i]) / 3.0f;
			nv9[i] = (2.0f*nv5[i] + nv0[i]) / 3.0f;
			nv10[i] = (2.0f*nv1[i] + nv4[i]) / 3.0f;
			nv11[i] = (2.0f*nv4[i] + nv1[i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv1, nv1);
		v_new[2] = AddVert(pv2, nv2);
		v_new[3] = AddVert(pv3, nv3);
		v_new[4] = AddVert(pv4, nv4);
		v_new[5] = AddVert(pv11, nv11);
		v_new[6] = AddVert(pv6, nv6);
		v_new[7] = AddVert(pv10, nv10);
		v_new[8] = AddVert(pv8, nv8);
		v_new[9] = AddVert(pv9, nv9);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	AddBound(v_new[5], 1);
		AddBound(v_new[6], 1);	AddBound(v_new[7], 1);
		AddBound(v_new[8], 1);	AddBound(v_new[9], 1);
		
	}

	void AddQuad_adaptive_3_3(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[10], v_quad[4];
		int i;

		for(i = 0; i < 10; i++) vv[i] = v_new[i];

		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[8];	v_quad[3] = vv[6];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[2];
		v_quad[2] = vv[7];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[4];
		v_quad[2] = vv[5];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[6];
		v_quad[2] = vv[8];	v_quad[3] = vv[9];
		AddQuad(v_quad, num);
		v_quad[0] = vv[0];	v_quad[1] = vv[1];
		v_quad[2] = vv[7];	v_quad[3] = vv[8];
		AddQuad(v_quad, num);
		v_quad[0] = vv[2];	v_quad[1] = vv[3];
		v_quad[2] = vv[5];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		v_quad[0] = vv[4];	v_quad[1] = v[3];
		v_quad[2] = vv[9];	v_quad[3] = vv[5];
		AddQuad(v_quad, num);
		v_quad[0] = vv[9];	v_quad[1] = vv[8];
		v_quad[2] = vv[7];	v_quad[3] = vv[5];
		AddQuad(v_quad, num);
		
	}

	void AddVert_adaptive_4_2b(unsigned int* v , unsigned int* v_new)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3], pv6[3], pv7[3], pv8[3], pv9[3], pv10[3], pv11[3];
		float nv0[3], nv1[3], nv2[3], nv3[3], nv4[3], nv5[3], nv6[3], nv7[3], nv8[3], nv9[3], nv10[3], nv11[3];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (2.0f*verts[v[0]][i] + verts[v[1]][i]) / 3.0f;
			pv1[i] = (2.0f*verts[v[1]][i] + verts[v[0]][i]) / 3.0f;
			pv2[i] = (2.0f*verts[v[1]][i] + verts[v[2]][i]) / 3.0f;
			pv3[i] = (2.0f*verts[v[2]][i] + verts[v[1]][i]) / 3.0f;
			pv4[i] = (2.0f*verts[v[2]][i] + verts[v[3]][i]) / 3.0f;
			pv5[i] = (2.0f*verts[v[3]][i] + verts[v[2]][i]) / 3.0f;
			pv6[i] = (2.0f*verts[v[0]][i] + verts[v[3]][i]) / 3.0f;
			pv7[i] = (2.0f*verts[v[3]][i] + verts[v[0]][i]) / 3.0f;
			pv8[i] = (2.0f*pv0[i] + pv5[i]) / 3.0f;
			pv9[i] = (2.0f*pv5[i] + pv0[i]) / 3.0f;
			pv10[i] = (2.0f*pv1[i] + pv4[i]) / 3.0f;
			pv11[i] = (2.0f*pv4[i] + pv1[i]) / 3.0f;

			nv0[i] = (2.0f*normals[v[0]][i] + normals[v[1]][i]) / 3.0f;
			nv1[i] = (2.0f*normals[v[1]][i] + normals[v[0]][i]) / 3.0f;
			nv2[i] = (2.0f*normals[v[1]][i] + normals[v[2]][i]) / 3.0f;
			nv3[i] = (2.0f*normals[v[2]][i] + normals[v[1]][i]) / 3.0f;
			nv4[i] = (2.0f*normals[v[2]][i] + normals[v[3]][i]) / 3.0f;
			nv5[i] = (2.0f*normals[v[3]][i] + normals[v[2]][i]) / 3.0f;
			nv6[i] = (2.0f*normals[v[0]][i] + normals[v[3]][i]) / 3.0f;
			nv7[i] = (2.0f*normals[v[3]][i] + normals[v[0]][i]) / 3.0f;
			nv8[i] = (2.0f*nv0[i] + nv5[i]) / 3.0f;
			nv9[i] = (2.0f*nv5[i] + nv0[i]) / 3.0f;
			nv10[i] = (2.0f*nv1[i] + nv4[i]) / 3.0f;
			nv11[i] = (2.0f*nv4[i] + nv1[i]) / 3.0f;
		}

		v_new[0] = AddVert(pv0, nv0);
		v_new[1] = AddVert(pv3, nv3);
		v_new[2] = AddVert(pv4, nv4);
		v_new[3] = AddVert(pv6, nv6);
		v_new[4] = AddVert(pv8, nv8);
		v_new[5] = AddVert(pv10, nv10);
		v_new[6] = AddVert(pv11, nv11);
		v_new[7] = AddVert(pv9, nv9);
		AddBound(v_new[0], 1);	AddBound(v_new[1], 1);
		AddBound(v_new[2], 1);	AddBound(v_new[3], 1);
		AddBound(v_new[4], 1);	AddBound(v_new[5], 1);
		AddBound(v_new[6], 1);	AddBound(v_new[7], 1);
		
	}

	void AddQuad_adaptive_4_2b(unsigned int* v , unsigned int* v_new, int num)
	{
		unsigned int vv[8], v_quad[4];
		int i;

		for(i = 0; i < 8; i++) vv[i] = v_new[i];

		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[4];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[5];
		v_quad[2] = vv[4];	v_quad[3] = vv[0];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[1];
		v_quad[2] = vv[6];	v_quad[3] = vv[5];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[2];
		v_quad[2] = vv[6];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[7];
		v_quad[2] = vv[6];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[3];
		v_quad[2] = vv[4];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		v_quad[0] = vv[4];	v_quad[1] = vv[5];
		v_quad[2] = vv[6];	v_quad[3] = vv[7];
		AddQuad(v_quad, num);
		
	}

	void AddQuad_hexa(unsigned int* v , int num)
	{
		float pv0[3], pv1[3], pv2[3], pv3[3], pv4[3], pv5[3], pv6[3], norm[3];
		unsigned int vv[7], v_quad[4];
		int i;

		for(i = 0; i < 3; i++) {
			pv0[i] = (verts[v[0]][i] + verts[v[1]][i]) / 2.0f;
			pv1[i] = (verts[v[1]][i] + verts[v[2]][i]) / 2.0f;
			pv2[i] = (verts[v[2]][i] + verts[v[3]][i]) / 2.0f;
			pv3[i] = (verts[v[3]][i] + verts[v[0]][i]) / 2.0f;
			pv4[i] = (verts[v[0]][i] + verts[v[2]][i]) / 2.0f;
			pv5[i] = (verts[v[0]][i] + verts[v[1]][i] + verts[v[2]][i]) / 3.0f;
			pv6[i] = (verts[v[0]][i] + verts[v[2]][i] + verts[v[3]][i]) / 3.0f;
		}

		vv[0] = AddVert(pv0, norm);
		vv[1] = AddVert(pv1, norm);
		vv[2] = AddVert(pv2, norm);
		vv[3] = AddVert(pv3, norm);
		vv[4] = AddVert(pv4, norm);
		vv[5] = AddVert(pv5, norm);
		vv[6] = AddVert(pv6, norm);

		AddBound(vv[0], 1);	AddBound(vv[1], 1);
		AddBound(vv[2], 1);	AddBound(vv[3], 1);
		AddBound(vv[4], 1);	AddBound(vv[5], 1);
		AddBound(vv[6], 1);

		v_quad[0] =  v[0];	v_quad[1] = vv[0];
		v_quad[2] = vv[5];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] =  v[1];	v_quad[1] = vv[1];
		v_quad[2] = vv[5];	v_quad[3] = vv[0];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[4];
		v_quad[2] = vv[5];	v_quad[3] = vv[1];
		AddQuad(v_quad, num);
		v_quad[0] =  v[2];	v_quad[1] = vv[2];
		v_quad[2] = vv[6];	v_quad[3] = vv[4];
		AddQuad(v_quad, num);
		v_quad[0] =  v[3];	v_quad[1] = vv[3];
		v_quad[2] = vv[6];	v_quad[3] = vv[2];
		AddQuad(v_quad, num);
		v_quad[0] =  v[0];	v_quad[1] = vv[4];
		v_quad[2] = vv[6];	v_quad[3] = vv[3];
		AddQuad(v_quad, num);
		
	}

	void AddVert_hexa_adaptive_1_center(unsigned int* v, unsigned int* v_new)
	{
		float pv[8][3], nv[8][3], po[3], no[3];
		int i, j;

		for(i = 0; i < 3; i++) {
			po[i] = 0.0f;	no[i] = 0.0f;
			for(j = 0; j < 8; j++) {
				po[i] += verts[v[j]][i];
				no[i] += normals[v[j]][i];
			}
			po[i] /= 8.0f;	no[i] /= 8.0f;

			for(j = 0; j < 8; j++) {
				pv[j][i] = (2.0f*po[i] + verts[v[j]][i])/3.0f;
				nv[j][i] = (2.0f*no[i] + normals[v[j]][i])/3.0f;
				//pv[j][i] = (po[i] + verts[v[j]][i])/2.0f;
				//nv[j][i] = (no[i] + normals[v[j]][i])/2.0f;
			}
		}

		for(i = 0; i < 8; i++) {
			for(j = 0; j < 3; j++) {
				po[j] = pv[i][j];	no[j] = nv[i][j];
			}
			v_new[i] = AddVert(po, no);
			//AddBound(v_new[i], 1);
		}
		
	}

	void AddVert_hexa_adaptive_1_top(unsigned int* v, unsigned int* v_new)
	{
		float pv[8][3], nv[8][3], po[3], no[3];
		int i, j;

		for(i = 0; i < 3; i++) {
			po[i] = 0.0f;	no[i] = 0.0f;
			for(j = 0; j < 4; j++) {
				po[i] += verts[v[j]][i];
				no[i] += normals[v[j]][i];
			}
			po[i] /= 4.0f;	no[i] /= 4.0f;

			for(j = 0; j < 4; j++) {
				pv[j][i] = (2.0f*po[i] + verts[v[j]][i])/3.0f;
				nv[j][i] = (2.0f*no[i] + normals[v[j]][i])/3.0f;
			}

			for(j = 4; j < 8; j++) {
				pv[j][i] = (po[i] + verts[v[j]][i])/2.0f;
				nv[j][i] = (no[i] + normals[v[j]][i])/2.0f;
			}

		}

		for(i = 0; i < 8; i++) {
			for(j = 0; j < 3; j++) {
				po[j] = pv[i][j];	no[j] = nv[i][j];
			}
			v_new[i] = AddVert(po, no);
			if(i < 4) AddBound(v_new[i], 1);
		}
		
	}

	void AddVert_hexa_adaptive_2(unsigned int* v, int* edge_id, unsigned int* v_new)
	{
		float pv[64][3], nv[64][3], po[3], no[3];
		int i, j;

		for(i = 0; i < 3; i++) {
			pv[0][i]  = verts[v[0]][i];
			pv[3][i]  = verts[v[1]][i];
			pv[51][i] = verts[v[5]][i];
			pv[48][i] = verts[v[4]][i];
			pv[12][i] = verts[v[3]][i];
			pv[15][i] = verts[v[2]][i];
			pv[63][i] = verts[v[6]][i];
			pv[60][i] = verts[v[7]][i];
			
			nv[0][i]  = normals[v[0]][i];
			nv[3][i]  = normals[v[1]][i];
			nv[51][i] = normals[v[5]][i];
			nv[48][i] = normals[v[4]][i];
			nv[12][i] = normals[v[3]][i];
			nv[15][i] = normals[v[2]][i];
			nv[63][i] = normals[v[6]][i];
			nv[60][i] = normals[v[7]][i];

			pv[16][i] = (2.0f*pv[0][i]  + pv[48][i])/3.0f;
			pv[32][i] = (2.0f*pv[48][i] + pv[0][i])/3.0f;
			pv[19][i] = (2.0f*pv[3][i]  + pv[51][i])/3.0f;
			pv[35][i] = (2.0f*pv[51][i] + pv[3][i])/3.0f;
			pv[28][i] = (2.0f*pv[12][i] + pv[60][i])/3.0f;
			pv[44][i] = (2.0f*pv[60][i] + pv[12][i])/3.0f;
			pv[31][i] = (2.0f*pv[15][i] + pv[63][i])/3.0f;
			pv[47][i] = (2.0f*pv[63][i] + pv[15][i])/3.0f;

			nv[16][i] = (2.0f*nv[0][i]  + nv[48][i])/3.0f;
			nv[32][i] = (2.0f*nv[48][i] + nv[0][i])/3.0f;
			nv[19][i] = (2.0f*nv[3][i]  + nv[51][i])/3.0f;
			nv[35][i] = (2.0f*nv[51][i] + nv[3][i])/3.0f;
			nv[28][i] = (2.0f*nv[12][i] + nv[60][i])/3.0f;
			nv[44][i] = (2.0f*nv[60][i] + nv[12][i])/3.0f;
			nv[31][i] = (2.0f*nv[15][i] + nv[63][i])/3.0f;
			nv[47][i] = (2.0f*nv[63][i] + nv[15][i])/3.0f;

			for(j = 0; j < 4; j++) {
				pv[4+16*j][i]  = (2.0f*pv[0+16*j][i]  + pv[12+16*j][i])/3.0f;
				pv[8+16*j][i]  = (2.0f*pv[12+16*j][i] + pv[0+16*j][i])/3.0f;
				pv[7+16*j][i]  = (2.0f*pv[3+16*j][i]  + pv[15+16*j][i])/3.0f;
				pv[11+16*j][i] = (2.0f*pv[15+16*j][i] + pv[3+16*j][i])/3.0f;
				pv[1+16*j][i]  = (2.0f*pv[0+16*j][i]  + pv[3+16*j][i])/3.0f;
				pv[2+16*j][i]  = (2.0f*pv[3+16*j][i]  + pv[0+16*j][i])/3.0f;
				pv[13+16*j][i] = (2.0f*pv[12+16*j][i] + pv[15+16*j][i])/3.0f;
				pv[14+16*j][i] = (2.0f*pv[15+16*j][i] + pv[12+16*j][i])/3.0f;
				pv[5+16*j][i]  = (2.0f*pv[4+16*j][i]  + pv[7+16*j][i])/3.0f;
				pv[6+16*j][i]  = (2.0f*pv[7+16*j][i]  + pv[4+16*j][i])/3.0f;
				pv[9+16*j][i]  = (2.0f*pv[8+16*j][i]  + pv[11+16*j][i])/3.0f;
				pv[10+16*j][i] = (2.0f*pv[11+16*j][i] + pv[8+16*j][i])/3.0f;

				nv[4+16*j][i]  = (2.0f*nv[0+16*j][i]  + nv[12+16*j][i])/3.0f;
				nv[8+16*j][i]  = (2.0f*nv[12+16*j][i] + nv[0+16*j][i])/3.0f;
				nv[7+16*j][i]  = (2.0f*nv[3+16*j][i]  + nv[15+16*j][i])/3.0f;
				nv[11+16*j][i] = (2.0f*nv[15+16*j][i] + nv[3+16*j][i])/3.0f;
				nv[1+16*j][i]  = (2.0f*nv[0+16*j][i]  + nv[3+16*j][i])/3.0f;
				nv[2+16*j][i]  = (2.0f*nv[3+16*j][i]  + nv[0+16*j][i])/3.0f;
				nv[13+16*j][i] = (2.0f*nv[12+16*j][i] + nv[15+16*j][i])/3.0f;
				nv[14+16*j][i] = (2.0f*nv[15+16*j][i] + nv[12+16*j][i])/3.0f;
				nv[5+16*j][i]  = (2.0f*nv[4+16*j][i]  + nv[7+16*j][i])/3.0f;
				nv[6+16*j][i]  = (2.0f*nv[7+16*j][i]  + nv[4+16*j][i])/3.0f;
				nv[9+16*j][i]  = (2.0f*nv[8+16*j][i]  + nv[11+16*j][i])/3.0f;
				nv[10+16*j][i] = (2.0f*nv[11+16*j][i] + nv[8+16*j][i])/3.0f;
			}
		}

		for(i = 0; i < 64; i++) {
			if(i == 0) v_new[i] = v[0];
			else if(i == 3) v_new[i] = v[1];
			else if(i == 51) v_new[i] = v[5];
			else if(i == 48) v_new[i] = v[4];
			else if(i == 12) v_new[i] = v[3];
			else if(i == 15) v_new[i] = v[2];
			else if(i == 63) v_new[i] = v[6];
			else if(i == 60) v_new[i] = v[7];
			else {
				for(j = 0; j < 3; j++) {
					po[j] = pv[i][j];	no[j] = nv[i][j];
				}
				v_new[i] = AddVert(po, no);
			}
			if((edge_id[0] > 0) && (i < 16)) AddBound(v_new[i], 1);
			if((edge_id[1] > 0) && (i > 47)) AddBound(v_new[i], 1);
			if((edge_id[2] > 0) && ((i%4) == 0)) AddBound(v_new[i], 1);
			if((edge_id[3] > 0) && ((i%4) == 3)) AddBound(v_new[i], 1);
			if((edge_id[4] > 0) && ((i%16) < 4)) AddBound(v_new[i], 1);
			if((edge_id[5] > 0) && ((i%16) > 11)) AddBound(v_new[i], 1);
		}
		
		// define bound_edge ...
		for(i = 0; i < 64; i++) {
			if(CheckBound_edge(v_new[0], v_new[3]) == 1) {AddBound(v_new[1], 1); AddBound(v_new[2], 1);}
			if(CheckBound_edge(v_new[3], v_new[15]) == 1) {AddBound(v_new[7], 1); AddBound(v_new[11], 1);}
			if(CheckBound_edge(v_new[15],v_new[12]) == 1) {AddBound(v_new[13], 1); AddBound(v_new[14], 1);}
			if(CheckBound_edge(v_new[0], v_new[12]) == 1) {AddBound(v_new[4], 1); AddBound(v_new[8], 1);}

			if(CheckBound_edge(v_new[48], v_new[51]) == 1) {AddBound(v_new[49], 1); AddBound(v_new[50], 1);}
			if(CheckBound_edge(v_new[51], v_new[63]) == 1) {AddBound(v_new[55], 1); AddBound(v_new[59], 1);}
			if(CheckBound_edge(v_new[63], v_new[60]) == 1) {AddBound(v_new[61], 1); AddBound(v_new[62], 1);}
			if(CheckBound_edge(v_new[48], v_new[60]) == 1) {AddBound(v_new[52], 1); AddBound(v_new[56], 1);}

			if(CheckBound_edge(v_new[0], v_new[48]) == 1) {AddBound(v_new[16], 1); AddBound(v_new[32], 1);}
			if(CheckBound_edge(v_new[3], v_new[51]) == 1) {AddBound(v_new[19], 1); AddBound(v_new[35], 1);}
			if(CheckBound_edge(v_new[12],v_new[60]) == 1) {AddBound(v_new[28], 1); AddBound(v_new[44], 1);}
			if(CheckBound_edge(v_new[15],v_new[63]) == 1) {AddBound(v_new[31], 1); AddBound(v_new[47], 1);}
		}
		
	}

	void AddVert_hexa_adaptive_2_1(unsigned int* v, int* edge_id, unsigned int* v_new)
	{
		float pv[7][3], nv[7][3], po[3], t0[3], t1[3];
		int i, j;

		for(i = 0; i < 3; i++) {
			pv[0][i] = (2.0f*verts[v[0]][i]  + verts[v[1]][i])/3.0f;

			po[i] = (2.0f*verts[v[3]][i]  + verts[v[2]][i])/3.0f;
			pv[1][i] = (2.0f*pv[0][i]  + po[i])/3.0f;

			pv[2][i] = (2.0f*verts[v[0]][i]  + verts[v[3]][i])/3.0f;
			pv[3][i] = (2.0f*verts[v[0]][i]  + verts[v[4]][i])/3.0f;

			po[i] = (2.0f*verts[v[1]][i]  + verts[v[5]][i])/3.0f;
			pv[4][i] = (2.0f*pv[3][i]  + po[i])/3.0f;

			t0[i] = (2.0f*verts[v[4]][i]  + verts[v[5]][i])/3.0f;
			t1[i] = (2.0f*verts[v[7]][i]  + verts[v[6]][i])/3.0f;
			po[i] = (2.0f*t0[i]  + t1[i])/3.0f;
			pv[5][i] = (2.0f*pv[1][i]  + po[i])/3.0f;

			po[i] = (2.0f*verts[v[3]][i]  + verts[v[7]][i])/3.0f;
			pv[6][i] = (2.0f*pv[3][i]  + po[i])/3.0f;

			// normal
			nv[0][i] = (2.0f*normals[v[0]][i]  + normals[v[1]][i])/3.0f;

			po[i] = (2.0f*normals[v[3]][i]  + normals[v[2]][i])/3.0f;
			nv[1][i] = (2.0f*nv[0][i]  + po[i])/3.0f;

			nv[2][i] = (2.0f*normals[v[0]][i]  + normals[v[3]][i])/3.0f;
			nv[3][i] = (2.0f*normals[v[0]][i]  + normals[v[4]][i])/3.0f;

			po[i] = (2.0f*normals[v[1]][i]  + normals[v[5]][i])/3.0f;
			nv[4][i] = (2.0f*nv[3][i]  + po[i])/3.0f;

			t0[i] = (2.0f*normals[v[4]][i]  + normals[v[5]][i])/3.0f;
			t1[i] = (2.0f*normals[v[7]][i]  + normals[v[6]][i])/3.0f;
			po[i] = (2.0f*t0[i]  + t1[i])/3.0f;
			nv[5][i] = (2.0f*nv[1][i]  + po[i])/3.0f;

			po[i] = (2.0f*normals[v[3]][i]  + normals[v[7]][i])/3.0f;
			nv[6][i] = (2.0f*nv[3][i]  + po[i])/3.0f;
		}
		for(i = 0; i < 7; i++) {
			for(j = 0; j < 3; j++) {
				po[j] = pv[i][j];	t0[j] = nv[i][j];
			}
			v_new[i] = AddVert(po, t0);
			//AddBound(v_new[i], 0);
		}
		if(edge_id[0] > 0) {AddBound(v_new[0], 1);	AddBound(v_new[1], 1);	AddBound(v_new[2], 1);}
		if(edge_id[2] > 0) {AddBound(v_new[2], 1);	AddBound(v_new[3], 1);	AddBound(v_new[6], 1);}
		if(edge_id[4] > 0) {AddBound(v_new[0], 1);	AddBound(v_new[3], 1);	AddBound(v_new[4], 1);}
		
		if(CheckBound_edge(v_new[0], v_new[1]) == 1) AddBound(v_new[0], 1);
		if(CheckBound_edge(v_new[0], v_new[3]) == 1) AddBound(v_new[2], 1);
		if(CheckBound_edge(v_new[0], v_new[4]) == 1) AddBound(v_new[3], 1);
		
	}

	void AddVert_hexa_adaptive_2_2(unsigned int* v, int* edge_id, unsigned int* v_new)
	{
		float pv[64][3], nv[64][3], po[3], no[3];
		int i, j, vv;

		for(i = 0; i < 3; i++) {
			pv[0][i]  = verts[v[0]][i];
			pv[3][i]  = verts[v[1]][i];
			pv[51][i] = verts[v[5]][i];
			pv[48][i] = verts[v[4]][i];
			pv[12][i] = verts[v[3]][i];
			pv[15][i] = verts[v[2]][i];
			pv[63][i] = verts[v[6]][i];
			pv[60][i] = verts[v[7]][i];
			
			nv[0][i]  = normals[v[0]][i];
			nv[3][i]  = normals[v[1]][i];
			nv[51][i] = normals[v[5]][i];
			nv[48][i] = normals[v[4]][i];
			nv[12][i] = normals[v[3]][i];
			nv[15][i] = normals[v[2]][i];
			nv[63][i] = normals[v[6]][i];
			nv[60][i] = normals[v[7]][i];

			pv[16][i] = (2.0f*pv[0][i]  + pv[48][i])/3.0f;
			pv[32][i] = (2.0f*pv[48][i] + pv[0][i])/3.0f;
			pv[19][i] = (2.0f*pv[3][i]  + pv[51][i])/3.0f;
			pv[35][i] = (2.0f*pv[51][i] + pv[3][i])/3.0f;
			pv[28][i] = (2.0f*pv[12][i] + pv[60][i])/3.0f;
			pv[44][i] = (2.0f*pv[60][i] + pv[12][i])/3.0f;
			pv[31][i] = (2.0f*pv[15][i] + pv[63][i])/3.0f;
			pv[47][i] = (2.0f*pv[63][i] + pv[15][i])/3.0f;

			nv[16][i] = (2.0f*nv[0][i]  + nv[48][i])/3.0f;
			nv[32][i] = (2.0f*nv[48][i] + nv[0][i])/3.0f;
			nv[19][i] = (2.0f*nv[3][i]  + nv[51][i])/3.0f;
			nv[35][i] = (2.0f*nv[51][i] + nv[3][i])/3.0f;
			nv[28][i] = (2.0f*nv[12][i] + nv[60][i])/3.0f;
			nv[44][i] = (2.0f*nv[60][i] + nv[12][i])/3.0f;
			nv[31][i] = (2.0f*nv[15][i] + nv[63][i])/3.0f;
			nv[47][i] = (2.0f*nv[63][i] + nv[15][i])/3.0f;

			for(j = 0; j < 4; j++) {
				pv[4+16*j][i]  = (2.0f*pv[0+16*j][i]  + pv[12+16*j][i])/3.0f;
				pv[8+16*j][i]  = (2.0f*pv[12+16*j][i] + pv[0+16*j][i])/3.0f;
				pv[7+16*j][i]  = (2.0f*pv[3+16*j][i]  + pv[15+16*j][i])/3.0f;
				pv[11+16*j][i] = (2.0f*pv[15+16*j][i] + pv[3+16*j][i])/3.0f;
				pv[1+16*j][i]  = (2.0f*pv[0+16*j][i]  + pv[3+16*j][i])/3.0f;
				pv[2+16*j][i]  = (2.0f*pv[3+16*j][i]  + pv[0+16*j][i])/3.0f;
				pv[13+16*j][i] = (2.0f*pv[12+16*j][i] + pv[15+16*j][i])/3.0f;
				pv[14+16*j][i] = (2.0f*pv[15+16*j][i] + pv[12+16*j][i])/3.0f;
				pv[5+16*j][i]  = (2.0f*pv[4+16*j][i]  + pv[7+16*j][i])/3.0f;
				pv[6+16*j][i]  = (2.0f*pv[7+16*j][i]  + pv[4+16*j][i])/3.0f;
				pv[9+16*j][i]  = (2.0f*pv[8+16*j][i]  + pv[11+16*j][i])/3.0f;
				pv[10+16*j][i] = (2.0f*pv[11+16*j][i] + pv[8+16*j][i])/3.0f;

				nv[4+16*j][i]  = (2.0f*nv[0+16*j][i]  + nv[12+16*j][i])/3.0f;
				nv[8+16*j][i]  = (2.0f*nv[12+16*j][i] + nv[0+16*j][i])/3.0f;
				nv[7+16*j][i]  = (2.0f*nv[3+16*j][i]  + nv[15+16*j][i])/3.0f;
				nv[11+16*j][i] = (2.0f*nv[15+16*j][i] + nv[3+16*j][i])/3.0f;
				nv[1+16*j][i]  = (2.0f*nv[0+16*j][i]  + nv[3+16*j][i])/3.0f;
				nv[2+16*j][i]  = (2.0f*nv[3+16*j][i]  + nv[0+16*j][i])/3.0f;
				nv[13+16*j][i] = (2.0f*nv[12+16*j][i] + nv[15+16*j][i])/3.0f;
				nv[14+16*j][i] = (2.0f*nv[15+16*j][i] + nv[12+16*j][i])/3.0f;
				nv[5+16*j][i]  = (2.0f*nv[4+16*j][i]  + nv[7+16*j][i])/3.0f;
				nv[6+16*j][i]  = (2.0f*nv[7+16*j][i]  + nv[4+16*j][i])/3.0f;
				nv[9+16*j][i]  = (2.0f*nv[8+16*j][i]  + nv[11+16*j][i])/3.0f;
				nv[10+16*j][i] = (2.0f*nv[11+16*j][i] + nv[8+16*j][i])/3.0f;
			}
		}

		for(i = 0; i < 28; i++) {
			if(i == 0) v_new[i] = v[0];
			else if(i == 3) v_new[i] = v[1];
			else if(i == 11) v_new[i] = v[2];
			else if(i == 10) v_new[i] = v[3];
			else if(i == 24) v_new[i] = v[4];
			else if(i == 25) v_new[i] = v[5];
			else if(i == 27) v_new[i] = v[6];
			else if(i == 26) v_new[i] = v[7];
			else {
				if(i < 8) vv = i;
				else if(i < 10) vv = i+1;
				else if(i == 10) vv = 12;
				else if(i == 11) vv = 15;
				else if(i < 20) vv = i+4;
				else if(i < 22) vv = i+13;
				else vv = i+19;				//if(i < 24) 

				for(j = 0; j < 3; j++) {po[j] = pv[vv][j];	no[j] = nv[vv][j];}
				v_new[i] = AddVert(po, no);
				//AddBound(v_new[i], 0);
			}
			if((edge_id[0] > 0) && (i < 12)) AddBound(v_new[i], 1);
			if((edge_id[2] > 0) && (i == 4 || i == 12 || i == 16)) AddBound(v_new[i], 1);
			if((edge_id[3] > 0) && (i == 7 || i == 15 || i == 19)) AddBound(v_new[i], 1);
			if((edge_id[4] > 0) && ((i%12 < 4 && i < 16) || i == 20 || i == 21)) AddBound(v_new[i], 1);
		}
			
		for(i = 0; i < 28; i++) {
			if(CheckBound_edge(v_new[0], v_new[3]) == 1) {AddBound(v_new[1], 1); AddBound(v_new[2], 1);}
			if(CheckBound_edge(v_new[3], v_new[11]) == 1) AddBound(v_new[7], 1);
			if(CheckBound_edge(v_new[0], v_new[10]) == 1) AddBound(v_new[4], 1);

			if(CheckBound_edge(v_new[0], v_new[24]) == 1) AddBound(v_new[12], 1);
			if(CheckBound_edge(v_new[3], v_new[25]) == 1) AddBound(v_new[15], 1);
		}
		
	}

	void AddVert_hexa_adaptive_2_4(unsigned int* v, int* edge_id, unsigned int* v_new)
	{
		float pv[68][3], nv[68][3], po[3], no[3];
		int i, j, vv;

		for(i = 0; i < 3; i++) {
			pv[0][i]  = verts[v[0]][i];
			pv[3][i]  = verts[v[1]][i];
			pv[51][i] = verts[v[5]][i];
			pv[48][i] = verts[v[4]][i];
			pv[12][i] = verts[v[3]][i];
			pv[15][i] = verts[v[2]][i];
			pv[63][i] = verts[v[6]][i];
			pv[60][i] = verts[v[7]][i];
			
			nv[0][i]  = normals[v[0]][i];
			nv[3][i]  = normals[v[1]][i];
			nv[51][i] = normals[v[5]][i];
			nv[48][i] = normals[v[4]][i];
			nv[12][i] = normals[v[3]][i];
			nv[15][i] = normals[v[2]][i];
			nv[63][i] = normals[v[6]][i];
			nv[60][i] = normals[v[7]][i];

			pv[16][i] = (2.0f*pv[0][i]  + pv[48][i])/3.0f;
			pv[32][i] = (2.0f*pv[48][i] + pv[0][i])/3.0f;
			pv[19][i] = (2.0f*pv[3][i]  + pv[51][i])/3.0f;
			pv[35][i] = (2.0f*pv[51][i] + pv[3][i])/3.0f;
			pv[28][i] = (2.0f*pv[12][i] + pv[60][i])/3.0f;
			pv[44][i] = (2.0f*pv[60][i] + pv[12][i])/3.0f;
			pv[31][i] = (2.0f*pv[15][i] + pv[63][i])/3.0f;
			pv[47][i] = (2.0f*pv[63][i] + pv[15][i])/3.0f;

			nv[16][i] = (2.0f*nv[0][i]  + nv[48][i])/3.0f;
			nv[32][i] = (2.0f*nv[48][i] + nv[0][i])/3.0f;
			nv[19][i] = (2.0f*nv[3][i]  + nv[51][i])/3.0f;
			nv[35][i] = (2.0f*nv[51][i] + nv[3][i])/3.0f;
			nv[28][i] = (2.0f*nv[12][i] + nv[60][i])/3.0f;
			nv[44][i] = (2.0f*nv[60][i] + nv[12][i])/3.0f;
			nv[31][i] = (2.0f*nv[15][i] + nv[63][i])/3.0f;
			nv[47][i] = (2.0f*nv[63][i] + nv[15][i])/3.0f;

			for(j = 0; j < 4; j++) {
				pv[4+16*j][i]  = (2.0f*pv[0+16*j][i]  + pv[12+16*j][i])/3.0f;
				pv[8+16*j][i]  = (2.0f*pv[12+16*j][i] + pv[0+16*j][i])/3.0f;
				pv[7+16*j][i]  = (2.0f*pv[3+16*j][i]  + pv[15+16*j][i])/3.0f;
				pv[11+16*j][i] = (2.0f*pv[15+16*j][i] + pv[3+16*j][i])/3.0f;
				pv[1+16*j][i]  = (2.0f*pv[0+16*j][i]  + pv[3+16*j][i])/3.0f;
				pv[2+16*j][i]  = (2.0f*pv[3+16*j][i]  + pv[0+16*j][i])/3.0f;
				pv[13+16*j][i] = (2.0f*pv[12+16*j][i] + pv[15+16*j][i])/3.0f;
				pv[14+16*j][i] = (2.0f*pv[15+16*j][i] + pv[12+16*j][i])/3.0f;
				pv[5+16*j][i]  = (2.0f*pv[4+16*j][i]  + pv[7+16*j][i])/3.0f;
				pv[6+16*j][i]  = (2.0f*pv[7+16*j][i]  + pv[4+16*j][i])/3.0f;
				pv[9+16*j][i]  = (2.0f*pv[8+16*j][i]  + pv[11+16*j][i])/3.0f;
				pv[10+16*j][i] = (2.0f*pv[11+16*j][i] + pv[8+16*j][i])/3.0f;

				nv[4+16*j][i]  = (2.0f*nv[0+16*j][i]  + nv[12+16*j][i])/3.0f;
				nv[8+16*j][i]  = (2.0f*nv[12+16*j][i] + nv[0+16*j][i])/3.0f;
				nv[7+16*j][i]  = (2.0f*nv[3+16*j][i]  + nv[15+16*j][i])/3.0f;
				nv[11+16*j][i] = (2.0f*nv[15+16*j][i] + nv[3+16*j][i])/3.0f;
				nv[1+16*j][i]  = (2.0f*nv[0+16*j][i]  + nv[3+16*j][i])/3.0f;
				nv[2+16*j][i]  = (2.0f*nv[3+16*j][i]  + nv[0+16*j][i])/3.0f;
				nv[13+16*j][i] = (2.0f*nv[12+16*j][i] + nv[15+16*j][i])/3.0f;
				nv[14+16*j][i] = (2.0f*nv[15+16*j][i] + nv[12+16*j][i])/3.0f;
				nv[5+16*j][i]  = (2.0f*nv[4+16*j][i]  + nv[7+16*j][i])/3.0f;
				nv[6+16*j][i]  = (2.0f*nv[7+16*j][i]  + nv[4+16*j][i])/3.0f;
				nv[9+16*j][i]  = (2.0f*nv[8+16*j][i]  + nv[11+16*j][i])/3.0f;
				nv[10+16*j][i] = (2.0f*nv[11+16*j][i] + nv[8+16*j][i])/3.0f;
			}

			pv[64][i] = (pv[21][i] + pv[37][i])/2.0f;
			pv[65][i] = (pv[22][i] + pv[38][i])/2.0f;
			pv[66][i] = (pv[25][i] + pv[41][i])/2.0f;
			pv[67][i] = (pv[26][i] + pv[41][i])/2.0f;

			nv[64][i] = (nv[21][i] + nv[37][i])/2.0f;
			nv[65][i] = (nv[22][i] + nv[38][i])/2.0f;
			nv[66][i] = (nv[25][i] + nv[41][i])/2.0f;
			nv[67][i] = (nv[26][i] + nv[41][i])/2.0f;
		}

		for(i = 0; i < 48; i++) {
			if(i == 0) v_new[i] = v[0];
			else if(i == 3) v_new[i] = v[1];
			else if(i == 15) v_new[i] = v[2];
			else if(i == 12) v_new[i] = v[3];
			else if(i == 44) v_new[i] = v[4];
			else if(i == 45) v_new[i] = v[5];
			else if(i == 47) v_new[i] = v[6];
			else if(i == 46) v_new[i] = v[7];
			else {
				if(i < 32) vv = i;
				else if(i < 36) vv = i+32;
				else if(i < 38) vv = i-3;
				else if(i == 38) vv = 36;
				else if(i < 41) vv = i;
				else if(i == 41) vv = 43;
				else vv = i+3;		// if(i < 44) 

				for(j = 0; j < 3; j++) {po[j] = pv[vv][j];	no[j] = nv[vv][j];}
				v_new[i] = AddVert(po, no);
				//if(i!=21 && i!=22 && i!=25 && i!=26 && i!=32 && i!=33 && i!=34 && i!=35) AddBound(v_new[i], 1);
			}
			
			if((edge_id[0] > 0) && (i < 16)) AddBound(v_new[i], 1);
			if((edge_id[2] > 0) && ((i%4 == 0 && i < 29) || i == 38 || i == 40)) AddBound(v_new[i], 1);
			if((edge_id[3] > 0) && ((i%4 == 3 && i < 32) || i == 39 || i == 41)) AddBound(v_new[i], 1);
			if((edge_id[4] > 0) && ((i%16 < 4 && i < 20) || i == 36 || i == 37)) AddBound(v_new[i], 1);
			if((edge_id[5] > 0) && ((i%16 > 11&& i < 32) || i == 42 || i == 43)) AddBound(v_new[i], 1);
		}
		
		for(i = 0; i < 48; i++) {
			if(CheckBound_edge(v_new[0], v_new[3]) == 1) {AddBound(v_new[1], 1); AddBound(v_new[2], 1);}
			if(CheckBound_edge(v_new[3], v_new[15]) == 1) {AddBound(v_new[7], 1); AddBound(v_new[11], 1);}
			if(CheckBound_edge(v_new[15], v_new[12]) == 1) {AddBound(v_new[13], 1); AddBound(v_new[14], 1);}
			if(CheckBound_edge(v_new[0], v_new[12]) == 1) {AddBound(v_new[4], 1); AddBound(v_new[8], 1);}

			if(CheckBound_edge(v_new[0], v_new[44]) == 1) AddBound(v_new[16], 1);
			if(CheckBound_edge(v_new[3], v_new[45]) == 1) AddBound(v_new[19], 1);
			if(CheckBound_edge(v_new[12], v_new[46]) == 1) AddBound(v_new[28], 1);
			if(CheckBound_edge(v_new[15], v_new[47]) == 1) AddBound(v_new[31], 1);
		}
		
	}

	int AddTri(unsigned int v1, unsigned int v2, unsigned int v3)
	{
		if (numtris+1 >= tsize) {
			tsize<<=1;
			triangles = (unsigned int (*)[3])realloc(triangles, sizeof(unsigned int[3]) * tsize);
			bound_tri = (unsigned int (*))realloc(bound_tri, sizeof(unsigned int) * tsize);
		}

		bound_tri[numtris] = 0;
		triangles[numtris][0] = v1;
		triangles[numtris][1] = v2;
		triangles[numtris][2] = v3;
		
		return numtris++;
	}

	float get_aspect_ratio(unsigned int v0, unsigned int v1, unsigned int v2) {
		float a, b, c, p, s, r_in, r_out;
		int i;

		a = 0.0;		b = 0.0;		c = 0.0;
		for(i = 0; i < 3; i++) {
			a += (verts[v1][i] - verts[v0][i])*(verts[v1][i] - verts[v0][i]);
			b += (verts[v2][i] - verts[v1][i])*(verts[v2][i] - verts[v1][i]);
			c += (verts[v0][i] - verts[v2][i])*(verts[v0][i] - verts[v2][i]);
		}
		a = (float)sqrt(a);		b = (float)sqrt(b);		c = (float)sqrt(c);
		p = (a + b + c) / 2.0f;
		s = (float)sqrt(p * (p - a) * (p - b) * (p - c));
		r_in = s / p;
		r_out = a * b * c / (4.0f * s);
		return (r_in / r_out);
	}

	void Add_2_Tetra(unsigned int* v, unsigned int my_vertex) {
		float aspect_ratio_0, aspect_ratio_1, temp;

		// dectect duplicate vertices
		if(v[0] == v[1]) {
			AddTetra(v[1], v[3], v[2], my_vertex);
		}
		else if(v[1] == v[2]) {
			AddTetra(v[0], v[3], v[1], my_vertex);
		}
		else if(v[2] == v[3] || v[3] == v[0]) {
			AddTetra(v[0], v[2], v[1], my_vertex);
		}
		else {
			aspect_ratio_0 = get_aspect_ratio(v[0], v[2], v[1]);
			temp = get_aspect_ratio(v[0], v[3], v[2]);
			if(temp < aspect_ratio_0) aspect_ratio_0 =  temp;

			aspect_ratio_1 = get_aspect_ratio(v[0], v[3], v[1]);
			temp = get_aspect_ratio(v[1], v[3], v[2]);
			if(temp < aspect_ratio_1) aspect_ratio_1 =  temp;

			if(aspect_ratio_0 > aspect_ratio_1) {
				AddTetra(v[0], v[2], v[1], my_vertex);
				AddTetra(v[0], v[3], v[2], my_vertex);
			}
			else {
				AddTetra(v[0], v[3], v[1], my_vertex);
				AddTetra(v[1], v[3], v[2], my_vertex);
			}
		}
	}

	void Extend_Tri(unsigned int v0, unsigned int v1, unsigned int v2) {
		unsigned int vv0, vv1, vv2, t0, t1, t2, t, i, nstep;
		float v_pos[3], norm[3], r, step, radius_o, center;

		vv0 = v0;	vv1 = v1;	vv2 = v2;
		if(vv1 < vv0 && vv1 < vv2) {v0 = vv1;	v1 = vv2;	v2 = vv0;}
		if(vv2 < vv0 && vv2 < vv1) {v0 = vv2;	v1 = vv0;	v2 = vv1;}
		t0 = v0;	t1 = v1;	t2 = v2;

		center = (129.0-1.0f)/2.0f;
		radius_o = (129.0-1.0f)/2.0f;
		step = (30.0f*20.0f-radius_o)/10.0f; //40 times -- 10
		//step = 268.0/21.0;   // 20 times -- 6
		//step = 270.0f/55.0f;   // 20 times -- 10
		nstep = 5;

		if (vtx_idx_arr_extend[v0] == -1) {
			for(i = 1; i < nstep; i++) {
				r = (float)sqrt((verts[v0][0]-center)*(verts[v0][0]-center) + 
								(verts[v0][1]-center)*(verts[v0][1]-center) + 
								(verts[v0][2]-center)*(verts[v0][2]-center));
				for(t = 0; t < 3; t++) {
					v_pos[t] = (verts[v0][t]-center) * (radius_o + step*i*(i+1)/2.0f)/r + center;
					norm[t] = normals[v0][t];
				}
				vv0 = AddVert(v_pos, norm);
				AddBound(vv0, 1);
				if(i == 1) vtx_idx_arr_extend[v0] = vv0;
			}
		}
		if (vtx_idx_arr_extend[v1] == -1) {
			for(i = 1; i < nstep; i++) {
				r = (float)sqrt((verts[v1][0]-center)*(verts[v1][0]-center) + 
								(verts[v1][1]-center)*(verts[v1][1]-center) + 
								(verts[v1][2]-center)*(verts[v1][2]-center));
				for(t = 0; t < 3; t++) {
					v_pos[t] = (verts[v1][t]-center) * (radius_o + step*i*(i+1)/2.0f)/r + center;
					norm[t] = normals[v1][t];
				}
				vv1 = AddVert(v_pos, norm);
				AddBound(vv1, 1);
				if(i == 1) vtx_idx_arr_extend[v1] = vv1;
			}
		}
		if (vtx_idx_arr_extend[v2] == -1) {
			for(i = 1; i < nstep; i++) {
				r = (float)sqrt((verts[v2][0]-center)*(verts[v2][0]-center) + 
								(verts[v2][1]-center)*(verts[v2][1]-center) + 
								(verts[v2][2]-center)*(verts[v2][2]-center));
				for(t = 0; t < 3; t++) {
					v_pos[t] = (verts[v2][t]-center) * (radius_o + step*i*(i+1)/2.0f)/r + center;
					norm[t] = normals[v2][t];
				}
				vv2 = AddVert(v_pos, norm);
				AddBound(vv2, 1);
				if(i == 1) vtx_idx_arr_extend[v2] = vv2;
			}
		}

		for(i = 1; i < nstep; i++) {
			vv0 = vtx_idx_arr_extend[v0] + (i - 1);
			vv1 = vtx_idx_arr_extend[v1] + (i - 1);
			vv2 = vtx_idx_arr_extend[v2] + (i - 1);

			if(v0 < v1 && v1 < v2)	{
				AddTetra(vv0, vv1, vv2, t0);
				AddTetra(vv2, vv1, t1, t0);
				AddTetra(vv2, t1, t2, t0);
			}
			else  {	//if(v0 < v2 && v2 < v1)
				AddTetra(vv0, vv1, vv2, t0);
				AddTetra(vv2, vv1, t2, t0);
				AddTetra(vv1, t1, t2, t0);
			}
			t0 = vv0;	t1 = vv1;	t2 = vv2;
		}
	}

	void Extend_Tetra(unsigned int* v) {
		float aspect_ratio_0, aspect_ratio_1, temp, radius, center;
		
		center = 64.0f;

		radius = (float)sqrt((verts[v[0]][0]-center)*(verts[v[0]][0]-center) + 
							(verts[v[0]][1]-center)*(verts[v[0]][1]-center) +
							(verts[v[0]][2]-center)*(verts[v[0]][2]-center));

		if(radius > 40.0) {
			// dectect duplicate vertices
			if(v[0] == v[1]) {
				Extend_Tri(v[1], v[3], v[2]);
			}
			else if(v[1] == v[2]) {
				Extend_Tri(v[0], v[3], v[1]);
			}
			else if(v[2] == v[3] || v[3] == v[0]) {
				Extend_Tri(v[0], v[2], v[1]);
			}
			else {
				aspect_ratio_0 = get_aspect_ratio(v[0], v[2], v[1]);
				temp = get_aspect_ratio(v[0], v[3], v[2]);
				if(temp < aspect_ratio_0) aspect_ratio_0 =  temp;

				aspect_ratio_1 = get_aspect_ratio(v[0], v[3], v[1]);
				temp = get_aspect_ratio(v[1], v[3], v[2]);
				if(temp < aspect_ratio_1) aspect_ratio_1 =  temp;

				if(aspect_ratio_0 > aspect_ratio_1) {
					Extend_Tri(v[0], v[2], v[1]);
					Extend_Tri(v[0], v[3], v[2]);
				}
				else {
					Extend_Tri(v[0], v[3], v[1]);
					Extend_Tri(v[1], v[3], v[2]);
				}
			}
		}
	}

	void Add_2_Tri(unsigned int* v) {
		float aspect_ratio_0, aspect_ratio_1, temp;

		// dectect duplicate vertices
		if(v[0] == v[1]) {
			AddTri(v[1], v[2], v[3]);
			//AddTri(-1, -1, -1);
		}
		else if(v[1] == v[2]) {
			AddTri(v[0], v[1], v[3]);
			//AddTri(-1, -1, -1);
		}
		else if(v[2] == v[3] || v[3] == v[0]) {
			AddTri(v[0], v[1], v[2]);
			//AddTri(-1, -1, -1);
		}
		else {

			aspect_ratio_0 = get_aspect_ratio(v[0], v[2], v[1]);
			temp = get_aspect_ratio(v[0], v[3], v[2]);
			if(temp < aspect_ratio_0) aspect_ratio_0 = temp;

			aspect_ratio_1 = get_aspect_ratio(v[0], v[3], v[1]);
			temp = get_aspect_ratio(v[1], v[3], v[2]);
			if(temp < aspect_ratio_1) aspect_ratio_1 = temp;

			if(aspect_ratio_0 > aspect_ratio_1) {
				AddTri(v[0], v[1], v[2]);
				AddTri(v[2], v[3], v[0]);
			}
			else {
				AddTri(v[0], v[1], v[3]);
				AddTri(v[1], v[2], v[3]);
			}
		}
	}

	float getRadius(float a[3][3], float b[3], float v0[3]) {

		float temp0, x, y, z;
		int i;

		if(fabs(a[0][0]) < fabs(a[1][0]) && fabs(a[2][0]) <= fabs(a[1][0])) {
			for(i = 0; i < 3; i++) {
				temp0 = a[0][i];	a[0][i] = a[1][i];	a[1][i] = temp0;
			}
			temp0 = b[0];	b[0] = b[1];	b[1] = temp0;
		}
		else {
			if(fabs(a[0][0]) < fabs(a[2][0]) && fabs(a[1][0]) <= fabs(a[2][0])) {
				for(i = 0; i < 3; i++) {
					temp0 = a[0][i];	a[0][i] = a[2][i];	a[2][i] = temp0;
				}
				temp0 = b[0];	b[0] = b[2];	b[2] = temp0;
			}
		}

		for(i = 1; i < 3; i++) {
			if(fabs(a[i][0]) > pow(10., -12)) {
				a[i][1] = a[0][0] * a[i][1] / a[i][0] - a[0][1];
				a[i][2] = a[0][0] * a[i][2] / a[i][0] - a[0][2];
				b[i] = a[0][0] * b[i] / a[i][0] - b[0];
				a[i][0] = 0.0;
			}
		}

		if(fabs(a[1][1]) < fabs(a[2][1])) {
			for(i = 0; i < 3; i++) {
				temp0 = a[1][i];	a[1][i] = a[2][i];	a[2][i] = temp0;
			}
			temp0 = b[1];	b[1] = b[2];	b[2] = temp0;
		}

		if(fabs(a[2][1]) > pow(10., -12)) {
			a[2][2] = a[1][1] * a[2][2] / a[2][1] - a[1][2];
			b[2] = a[1][1] * b[2] / a[2][1] - b[1];
			a[2][1] = 0.0;
		}

		if(fabs(a[0][0]) > pow(10., -12) && fabs(a[1][1]) > pow(10., -12) 
			&& fabs(a[2][2]) > pow(10., -12)) {
			z = b[2] / a[2][2];
			y = (b[1] - a[1][2] * z) / a[1][1];
			x = (b[0] - a[0][1] * y - a[0][2] * z) / a[0][0];
		}
		else
			printf("--- singular ---\n");

		float delt_x = x - v0[0];
		float delt_y = y - v0[1];
		float delt_z = z - v0[2];
		float radius = (float)sqrt(delt_x*delt_x + delt_y*delt_y + delt_z*delt_z);

		return radius;

	}

	// return 1 -- bad quality;		return 0 -- good quality
	int testTetrahedron(float v0[3], float v1[3], float v2[3], float v3[3]) {

		// min/max angles
		float v01[3], v02[3], n[3], v[3], t, min, max, temp;
		float v03[3], v13[3], v23[3], v0p[3], v1p[3], v2p[3];

		int i;
		for(i = 0; i < 3; i++) {
			v01[i] = v1[i] - v0[i];
			v02[i] = v2[i] - v0[i];
		}

		n[0] = v01[1]*v02[2] - v01[2]*v02[1];
		n[1] = v01[2]*v02[0] - v01[0]*v02[2];
		n[2] = v01[0]*v02[1] - v01[1]*v02[0];

		temp = (float)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
		n[0] = - n[0] / temp;
		n[1] = - n[1] / temp;
		n[2] = - n[2] / temp;

		t = (v2[0] - v3[0])*n[0] + (v2[1] - v3[1])*n[1] + (v2[2] - v3[2])*n[2];
		if(t < 0.0)	return 1;	// RHS

		for(i = 0; i < 3; i++) {
			v[i] = v3[i] + n[i] * t;

			v03[i] = v3[i] - v0[i];
			v0p[i] = v[i] - v0[i];

			v13[i] = v3[i] - v1[i];
			v1p[i] = v[i] - v1[i];

			v23[i] = v3[i] - v2[i];
			v2p[i] = v[i] - v2[i];
		}

		float a_dot_b = v03[0]*v0p[0] + v03[1]*v0p[1] + v03[2]*v0p[2];
		float module_a = (float)sqrt(v03[0]*v03[0] + v03[1]*v03[1] + v03[2]*v03[2]);
		float module_b = (float)sqrt(v0p[0]*v0p[0] + v0p[1]*v0p[1] + v0p[2]*v0p[2]);

		float angle0;
		if(module_b < pow(10., -12))
			angle0 = 90.0f;
		else
			angle0 = (float)acos(a_dot_b / (module_a * module_b)) * 180.0f / 3.1415926f;

		a_dot_b = v13[0]*v1p[0] + v13[1]*v1p[1] + v13[2]*v1p[2];
		module_a = (float)sqrt(v13[0]*v13[0] + v13[1]*v13[1] + v13[2]*v13[2]);
		module_b = (float)sqrt(v1p[0]*v1p[0] + v1p[1]*v1p[1] + v1p[2]*v1p[2]);

		float angle1;
		if(module_b < pow(10., -12))
			angle1 = 90.0f;
		else
			angle1 = (float)acos(a_dot_b / (module_a * module_b)) * 180.0f / 3.1415926f;

		a_dot_b = v23[0]*v2p[0] + v23[1]*v2p[1] + v23[2]*v2p[2];
		module_a = (float)sqrt(v23[0]*v23[0] + v23[1]*v23[1] + v23[2]*v23[2]);
		module_b = (float)sqrt(v2p[0]*v2p[0] + v2p[1]*v2p[1] + v2p[2]*v2p[2]);

		float angle2;
		if(module_b < pow(10., -12))
			angle2 = 90.0f;
		else
			angle2 = (float)acos(a_dot_b / (module_a * module_b)) * 180.0f / 3.1415926f;

		min = angle0;	max = angle0;
		if(angle1 < min)	min = angle1;
		else	max = angle1;

		if(angle2 < min)	min = angle2;
		if(angle2 > max)	max = angle2;

		if((min < 10.0) || (max > 160.0)) return 1;
		
		// tetrahedral quality measure = volume of tetrahedron / volume of equilateral tetrahedron
		//                               with same circumsphere radius
		float h = (float)sqrt((v[0] - v3[0])*(v[0] - v3[0]) + (v[1] - v3[1])*(v[1] - v3[1])
						+ (v[2] - v3[2])*(v[2] - v3[2]));
		float side_a = (float)sqrt((v1[0] - v0[0])*(v1[0] - v0[0]) + (v1[1] - v0[1])*(v1[1] - v0[1])
							+ (v1[2] - v0[2])*(v1[2] - v0[2]));
		float side_b = (float)sqrt((v2[0] - v1[0])*(v2[0] - v1[0]) + (v2[1] - v1[1])*(v2[1] - v1[1])
							+ (v2[2] - v1[2])*(v2[2] - v1[2]));
		float side_c = (float)sqrt((v2[0] - v0[0])*(v2[0] - v0[0]) + (v2[1] - v0[1])*(v2[1] - v0[1])
							+ (v2[2] - v0[2])*(v2[2] - v0[2]));
		float periphery = side_a + side_b + side_c;
		float area = 0.25f*(float)sqrt(periphery*(periphery - 2*side_a)*(periphery - 2*side_b)*(periphery - 2*side_c));
		float volume_1 = area * h / 3.0f;

		float matrix_lhs[3][3], vector_rhs[3], radius;
		for(i = 0; i < 3; i++) {
			matrix_lhs[0][i] = v1[i] - v0[i];
			matrix_lhs[1][i] = v2[i] - v0[i];
			matrix_lhs[2][i] = v3[i] - v0[i];
		}
		vector_rhs[0] = (v1[0]*v1[0] - v0[0]*v0[0] + v1[1]*v1[1] - v0[1]*v0[1] 
						+ v1[2]*v1[2] - v0[2]*v0[2]) / 2.0f; 
		vector_rhs[1] = (v2[0]*v2[0] - v0[0]*v0[0] + v2[1]*v2[1] - v0[1]*v0[1] 
						+ v2[2]*v2[2] - v0[2]*v0[2]) / 2.0f; 
		vector_rhs[2] = (v3[0]*v3[0] - v0[0]*v0[0] + v3[1]*v3[1] - v0[1]*v0[1] 
						+ v3[2]*v3[2] - v0[2]*v0[2]) / 2.0f;

		if(volume_1 > pow(10., -12))
			radius = getRadius(matrix_lhs, vector_rhs, v0);
		else
			return 1;

		float volume_2 = 0.25f*(float)sqrt(6.0f)*radius*radius*radius;

		if(volume_1/volume_2 < 0.02) return 1;
		else return 0;

	}

	void edge_contraction_tri(unsigned int* v) {
		float aspect_ratio, a, b, c;
		int i;

		aspect_ratio = get_aspect_ratio(v[0], v[1], v[2]);
		if(aspect_ratio < 0.1) {
			a = 0.0;		b = 0.0;		c = 0.0;
			for(i = 0; i < 3; i++) {
				a += (verts[v[1]][i] - verts[v[0]][i])*(verts[v[1]][i] - verts[v[0]][i]);
				b += (verts[v[2]][i] - verts[v[1]][i])*(verts[v[2]][i] - verts[v[1]][i]);
				c += (verts[v[0]][i] - verts[v[2]][i])*(verts[v[0]][i] - verts[v[2]][i]);
			}
			if(a <= b && a <= c) {
				for(i = 0; i < 3; i++)
					verts[v[1]][i] = verts[v[0]][i];
			}
			else if(b <= a && b <= c) {
				for(i = 0; i < 3; i++)
					verts[v[2]][i] = verts[v[1]][i];
			}
			else {
				for(i = 0; i < 3; i++)
					verts[v[0]][i] = verts[v[2]][i];
			}
		}
	}

	void edge_contraction_tetra(unsigned int* v, int num) {
		float min, max, e[6];
		float v0[3], v1[3], v2[3], v3[3];
		int i, id_min, sign;

		for(i = 0; i < 6; i++)	e[i] = 0.0;
		for(i = 0; i < 3; i++) {
			e[0] += (verts[v[1]][i] - verts[v[0]][i])*(verts[v[1]][i] - verts[v[0]][i]);	//e_01
			e[1] += (verts[v[2]][i] - verts[v[1]][i])*(verts[v[2]][i] - verts[v[1]][i]);	//e_12
			e[2] += (verts[v[0]][i] - verts[v[2]][i])*(verts[v[0]][i] - verts[v[2]][i]);	//e_20
			e[3] += (verts[v[3]][i] - verts[v[0]][i])*(verts[v[3]][i] - verts[v[0]][i]);	//e_03
			e[4] += (verts[v[3]][i] - verts[v[1]][i])*(verts[v[3]][i] - verts[v[1]][i]);	//e_13
			e[5] += (verts[v[3]][i] - verts[v[2]][i])*(verts[v[3]][i] - verts[v[2]][i]);	//e_23
			v0[i] = verts[v[0]][i];	v1[i] = verts[v[1]][i];	
			v2[i] = verts[v[2]][i];	v3[i] = verts[v[3]][i];
		}

		min = e[0];		max = e[0];
		id_min = 0;
		for(i = 1; i < 6; i++) {
			if(e[i] < min)	{min = e[i];	id_min = i;}
			if(e[i] > max)	 max = e[i];
		}

		//float aspect_ratio = min / max;
		//sign = testRHS(v0, v1, v2, v3);
		sign = testTetrahedron(v0, v1, v2, v3);
		//if(aspect_ratio < 0.1) { //0.2
		if(sign == 1) {
			if(id_min == 0) {
				for(i = 0; i < 3; i++)	verts[v[1]][i] = verts[v[0]][i];
			}
			else if(id_min == 1) {
				for(i = 0; i < 3; i++)	verts[v[2]][i] = verts[v[1]][i];
			}
			else if(id_min == 2) {
				for(i = 0; i < 3; i++)	verts[v[2]][i] = verts[v[0]][i];
			}
			else if(id_min == 3) {
				for(i = 0; i < 3; i++)	verts[v[3]][i] = verts[v[0]][i];
			}
			else if(id_min == 4) {
				for(i = 0; i < 3; i++)	verts[v[3]][i] = verts[v[1]][i];
			}
			else {	// id_min = 5
				for(i = 0; i < 3; i++)	verts[v[3]][i] = verts[v[2]][i];
			}
		}
	}

	void edge_contraction(unsigned int* v, int num) {
		if(num == 3) {
			edge_contraction_tri(v);
		}
		else {	// num == 4 or 5
			edge_contraction_tetra(v, num);
		}
	}

	void AddPyramid(unsigned int* v , unsigned int my_vertex, int num) {
		float vv0[3], vv1[3], vv2[3], vv3[3];
		int t, sign;

		AddQuad(v, num);

		for(t = 0; t < 3; t++) {
			vv0[t] = verts[v[0]][t];	vv1[t] = verts[v[1]][t];
			vv2[t] = verts[v[2]][t];	vv3[t] = verts[my_vertex][t];
		}
		sign = testRHS(vv0, vv1, vv2, vv3);

		if(sign == 0) {
			AddTri(v[0], my_vertex, v[1]);	AddTri(v[1], my_vertex, v[2]);
		}
		else {
			AddTri(v[1], my_vertex, v[0]);	AddTri(v[2], my_vertex, v[1]);
		}

		for(t = 0; t < 3; t++) {
			vv0[t] = verts[v[0]][t];	vv1[t] = verts[v[2]][t];
			vv2[t] = verts[v[3]][t];	vv3[t] = verts[my_vertex][t];
		}
		sign = testRHS(vv0, vv1, vv2, vv3);

		if(sign == 0) {
			AddTri(v[2], my_vertex, v[3]);	AddTri(v[3], my_vertex, v[0]);
		}
		else {
			AddTri(v[3], my_vertex, v[2]);	AddTri(v[0], my_vertex, v[3]);
		}
	}


	// RHS -- 0;	LHS -- 1;
	int testRHS(float v0[3], float v1[3], float v2[3], float v3[3]) {

		float v01[3], v02[3], v03[3], n[3], sign;

		for(int i = 0; i < 3; i++) {
			v01[i] = v1[i] - v0[i];
			v02[i] = v2[i] - v0[i];
			v03[i] = v3[i] - v0[i];
		}
		//float len = sqrt(v03[0]*v03[0] + v03[1]*v03[1] + v03[2]*v03[2]);
		//for(i = 0; i < 3; i++) v03[i] /= len;

		n[0] = v01[1]*v02[2] - v01[2]*v02[1];
		n[1] = v01[2]*v02[0] - v01[0]*v02[2];
		n[2] = v01[0]*v02[1] - v01[1]*v02[0];
		//len = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
		//for(i = 0; i < 3; i++) n[i] /= len;

		sign = v03[0]*n[0] + v03[1]*n[1] + v03[2]*n[2];

		if(sign < 0.0)	return 1;
		else if(sign == 0.0) return -1;
		else	return 0;
	}

	void AddTetra(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4) {
		float vv0[3], vv1[3], vv2[3], vv3[3];
		int t, sign;

		for(t = 0; t < 3; t++) {
			vv0[t] = verts[v1][t];
			vv1[t] = verts[v2][t];
			vv2[t] = verts[v3][t];
			vv3[t] = verts[v4][t];
		}
		sign = testRHS(vv0, vv1, vv2, vv3);

		if(sign == 1) {
			AddTri(v1, v3, v2);
			AddTri(v2, v3, v4);
			AddTri(v1, v4, v3);		AddTri(v1, v2, v4);
		}
		if(sign == 0) {
			AddTri(v1, v2, v3);
			bound_tri[numtris-1] = 1;
			AddTri(v3, v2, v4);
			AddTri(v1, v3, v4);		AddTri(v1, v4, v2);
		}
	}

	int AddVert(float v_pos[3], float norm[3])
	{
		int i;
/*
		// Jessica begin
		float v0[3], dist;
		int num = 0;
		if(numverts - 500 > 0) num = numverts - 500;
		for(i = num; i < numverts; i++) {
			v0[0] = verts[i][0];
			v0[1] = verts[i][1];
			v0[2] = verts[i][2];
			dist = (v_pos[0] - v0[0])*(v_pos[0] - v0[0]) + (v_pos[1] - v0[1])*(v_pos[1] - v0[1])
						+ (v_pos[2] - v0[2])*(v_pos[2] - v0[2]);
			if(sqrt(dist) < 0.0001)	{
				return i;
				break;
			}
		}
		// Jessica end
*/
		if (numverts+1 > vsize) {
			vsize<<=1;
			verts = (float (*)[3])realloc(verts,sizeof(float[3])*vsize);
			funcs = (float (*)[1])realloc(funcs,sizeof(float[1])*vsize);
//#ifdef GRAD
			// grad addtion
			normals = (float(*)[3])realloc(normals,sizeof(float[3])*vsize);
			curvatures = (float(*)[2])realloc(normals,sizeof(float[2])*vsize);
//#endif GRAD
			bound_sign = (unsigned int (*))realloc(bound_sign,sizeof(unsigned int)*vsize);
			vtxnew_sign = (unsigned int (*))realloc(vtxnew_sign,sizeof(unsigned int)*vsize);
			bound_edge   = (unsigned int (*)[18])realloc(bound_edge,sizeof(unsigned int[18])*vsize);
		}

		bound_sign[numverts] = 0;
		vtxnew_sign[numverts] = 0;
		for (i = 0; i < 18; i++) bound_edge[numverts][i] = 0;
		for (i = 0; i < 3; i++) verts[numverts][i] = v_pos[i];
//#ifdef GRAD
		for (i=0;i<3;i++)
			normals[numverts][i] = norm[i];
		for (i=0;i<2;i++)
			curvatures[numverts][i] = 0.0f;
//#endif 
		return numverts++;
	}

	// sign = 1 -- outer	// sign = -1 -- inside
	// sign = 0 -- not on boundary
	void AddBound(int index, int sign)
	{
		bound_sign[index] = sign;
	}

	void AddBound_edge(unsigned int index_1, unsigned int index_2)
	{
		int i, temp;
		if(index_2 < index_1) {
			temp = index_1;	index_1 = index_2;	index_2 = temp;
		}
		for(i = 0; i < 18; i++) {
			if(bound_edge[index_1][i] == index_2) break;
			if(bound_edge[index_1][i] == 0) bound_edge[index_1][i] = index_2;
		}
	}

	int CheckBound_edge(unsigned int index_1, unsigned int index_2)
	{
		int i, temp;
		if(index_2 < index_1) {
			temp = index_1;	index_1 = index_2;	index_2 = temp;
		}
		for(i = 0; i < 18; i++) {
			if(bound_edge[index_1][i] == index_2) return 1;
		}
		return 0;
	}

	void AddVtxNew(int index, int sign)
	{
		vtxnew_sign[index] = sign;
	}

	int center_vtx(int v1,int v2,int v3)
	{
		float center_vtx[3],norm[3];
		int i;
		for (i=0; i<3; i++) {
			center_vtx[i]=(verts[v1][i] + verts[v2][i] + verts[v3][i])/3.0f;
			// grad addtion
			norm[i]=(normals[v1][i]+normals[v2][i]+normals[v3][i])/3.0f;
		}
		
		return AddVert(center_vtx,norm);
	}
	
	//void loadgeoframe(const char * name, int num);
	void calculatenormals();
	void calculateTriangleNormal(float* norm, unsigned int c);
	void calculateExtents();
	void read_raw(const char * rawiv_fname);
	
	//void display();
	
	
	int numverts;
	int numtris;
	int num_tris;
	int numquads;
	int numhexas;
	int tsize, vsize, qsize;
	float (*verts)[3];
	float (*normals)[3];
	float (*curvatures)[2];
	float (*funcs)[1];
	unsigned int (*triangles)[3];
	unsigned int (*quads)[4];
	unsigned int *bound_sign;
	unsigned int *bound_tri;
	int* vtx_idx_arr_extend;
	unsigned int *vtxnew_sign;
	unsigned int (*bound_edge)[18];
	
	double biggestDim;
	double centerx, centery, centerz;
};

#endif //__GEOFRAME_H__
