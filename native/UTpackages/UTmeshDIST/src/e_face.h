#ifndef __E_FACE_H__
#define __E_FACE_H__

typedef struct _edge_face {
	int faceidx;
	int orientation[4];
	int facebit;
} edge_face;

#endif __E_FACE_H__
