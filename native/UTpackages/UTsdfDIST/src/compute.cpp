/*
  Copyright 2002-2003 The University of Texas at Austin

	Authors: Lalit Karlapalem <ckl@ices.utexas.edu>
	Advisor: Chandrajit Bajaj <bajaj@cs.utexas.edu>

  This is a free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"

using namespace SDFLibrary;

extern int point_in_polygon(myPoint result, int tri);
extern int ray_polygon_intersection (ray r, int tri);
extern void apply_distance_transform(int vi, int vj, int vk);
extern int confirm_SDF(int flag);

int x_assign(int vi, int vj, int vk)
{
	int i, j, k, inters, temp, flag, v;
	listnode* currNode;
	ray r;
	int pts[50];	//the given ray cant intersect the surface more than 50 times...

	//pick a direction n shoot rays to the outside BB.
	i=(int)vi;				j= (int)vj;				k= (int)vk;
	r.ox = (double)vi;		r.oy = (double)vj;		r.oz = (double)vk;
	r.dx =1.0f;				r.dy =0.0f;				r.dz =0.0f;
	inters=0;				v = i;

	for (i=v; i<size; i++)
	{
		if (sdf[i][j][k].type==4)
		{
			currNode = sdf[i][j][k].tindex;

			while(currNode != NULL)
			{
				temp = ray_polygon_intersection(r, currNode->index);

				if (temp ==1)
				{
					flag =0;

					for (temp=0; temp<inters; temp++)
					{
						if (pts[temp] == currNode->index)	flag=1;
					}

					if (flag ==0)
						pts[inters++] = currNode->index;
				}
				currNode = currNode->next;
			}
		}
	}

	return (inters);
}

int y_assign(int vi, int vj, int vk)
{
	int i, j, k, inters, temp, flag, v;
	listnode* currNode;
	ray r;
	int pts[50];	//the given ray cant intersect the surface more than 50 times...

	//pick a direction n shoot rays to the outside BB.
	i=(int)vi;				j= (int)vj;				k= (int)vk;
	r.ox = (double)vi;		r.oy = (double)vj;		r.oz = (double)vk;
	r.dx =0.0f;				r.dy =1.0f;				r.dz =0.0f;
	inters=0;				v = j;

	for (j=v; j<size; j++)
	{
		if (sdf[i][j][k].type==4)
		{
			currNode = sdf[i][j][k].tindex;

			while(currNode != NULL)
			{
				temp = ray_polygon_intersection(r, currNode->index);

				if (temp ==1)
				{
					flag =0;

					for (temp=0; temp<inters; temp++)
					{
						if (pts[temp] == currNode->index)	flag=1;
					}

					if (flag ==0)
						pts[inters++] = currNode->index;
				}
				currNode = currNode->next;
			}
		}
	}

	return (inters);
}

int z_assign(int vi, int vj, int vk)
{
	int i, j, k, inters, temp, flag, v;
	listnode* currNode;
	ray r;
	int pts[50];	//the given ray cant intersect the surface more than 50 times...

	//pick a direction n shoot rays to the outside BB.
	i=(int)vi;				j= (int)vj;				k= (int)vk;
	r.ox = (double)vi;		r.oy = (double)vj;		r.oz = (double)vk;
	r.dx =0.0f;				r.dy =0.0f;				r.dz =1.0f;
	inters=0;				v = k;

	for (k=v; k<size; k++)
	{
		if (sdf[i][j][k].type==4)
		{
			currNode = sdf[i][j][k].tindex;

			while(currNode != NULL)
			{
				temp = ray_polygon_intersection(r, currNode->index);

				if (temp ==1)
				{
					flag =0;

					for (temp=0; temp<inters; temp++)
					{
						if (pts[temp] == currNode->index)	flag=1;
					}

					if (flag ==0)
						pts[inters++] = currNode->index;
				}
				currNode = currNode->next;
			}
		}
	}

	return (inters);
}

int klc_assign(int vi, int vj, int vk)
{
	int inters[3];

	if ((vi<=0) || (vj<=0) || (vk<=0) || (vi>=size) || (vj>=size) || (vk>=size))
	{
		return 1;
	}
	else
	{
		inters[0] = x_assign(vi, vj, vk);
		inters[1] = y_assign(vi, vj, vk);
		inters[2] = z_assign(vi, vj, vk);
	}

	if ( (inters[0]%2 ==0) && (inters[1]%2 ==0) && (inters[2]%2 ==0) )
		return 1;
	else if ( (inters[0]%2 ==1) && (inters[1]%2 ==1) && (inters[2]%2 ==1) )
		return -1;

	//Else, u have run into some doubleing pt. error and need to count the #of intersections.
	if ( ((inters[0]%2) + (inters[1]%2) + (inters[2]%2)) %2 ==1)
		return 1;
	else
		return -1;

	/*
	if ((vi<=0) || (vj<=0) || (vk<=0) || (vi>=size) || (vj>=size) || (vk>=size))
		return 1;

	if (x_assign(vi, vj, vk) %2 ==0)
		return 1;
	return -1;
	*/
}

double sort_3_distances(double vals[3], myPoint closest[3], myPoint* inter)
{
	double dist;

	if (vals[0] <= vals[1])
	{
		if (vals[0] <=vals[2])
		{
			dist = vals[0];
			inter->x = closest[0].x;		inter->y = closest[0].y;		inter->z = closest[0].z;
		}
		else
		{
			dist = vals[2];
			inter->x = closest[2].x;		inter->y = closest[2].y;		inter->z = closest[2].z;
		}
	}
	else
	{
		if (vals[1] <=vals[2])
		{
			dist = vals[1];
			inter->x = closest[1].x;		inter->y = closest[1].y;		inter->z = closest[1].z;
		}
		else
		{
			dist = vals[2];
			inter->x = closest[2].x;		inter->y = closest[2].y;		inter->z = closest[2].z;
		}
	}

	return dist;
}

//project the point proj onto the line i-j and find the nearest point.
double getClipPoint(int one, int two, int i, int j, int k, myPoint* inter)
{
	double denom, theta, tempLen, t;
	double d1, d2, d3, e1, e2, e3;

	//1) Normalize the triangle edge and the line from a vertex to the point.
	d1 = vertices[one].x - vertices[two].x;
	d2 = vertices[one].y - vertices[two].y;
	d3 = vertices[one].z - vertices[two].z;
	denom = d1*d1 + d2*d2 + d3*d3;
    denom = sqrt(denom);
    tempLen = denom; //len of the tri edge
	d1 /=denom;	d2 /=denom;	d3 /=denom;

	e1 = (double)i - vertices[two].x;
	e2 = (double)j - vertices[two].y;
	e3 = (double)k - vertices[two].z;
	denom = e1*e1 + e2*e2 + e3*e3;
	if (isZero(denom))
	{
		//Then, the pt is CLOSE to the vertex TWO.
		inter->x = vertices[two].x;		inter->y = vertices[two].y;		inter->z = vertices[two].z;
		return fabs(denom);
	}
    denom = sqrt(denom); //len of the line from point to tri vertex.
	e1 /=denom;	e2 /=denom;	e3 /=denom;

	//2) Find the angle between these lines.
	theta = e1*d1 + e2*d2 + e3*d3;

	if (isZero(theta))
	{
		d1 = (double)i - vertices[one].x;
		d2 = (double)j - vertices[one].y;
		d3 = (double)k - vertices[one].z;
		theta = sqrt(d1*d1 + d2*d2 + d3*d3);

		if (theta <=denom)
		{
			inter->x = vertices[one].x;		inter->y = vertices[one].y;		inter->z = vertices[one].z;	return fabs(theta);
		}
		else
		{
			inter->x = vertices[two].x;		inter->y = vertices[two].y;		inter->z = vertices[two].z;	return fabs(denom);
		}
	}

    if (theta <0)
	{
		inter->x = vertices[two].x;		inter->y = vertices[two].y;		inter->z = vertices[two].z;
		return (denom);
	}
    else if ((denom * theta) > tempLen)
    {
        d1 = (double)i - vertices[one].x;
		d2 = (double)j - vertices[one].y;
		d3 = (double)k - vertices[one].z;
		theta = d1*d1 + d2*d2 + d3*d3;
		inter->x = vertices[one].x;		inter->y = vertices[one].y;		inter->z = vertices[one].z;
        return sqrt(theta);
    }
    else
    {
        t = denom*theta;				theta = acos(theta);
		inter->x = vertices[two].x + t*(vertices[one].x - vertices[two].x);
		inter->y = vertices[two].y + t*(vertices[one].y - vertices[two].y);
		inter->z = vertices[two].z + t*(vertices[one].z - vertices[two].z);
        return fabs((sin(theta) *denom));
    }
}


//compute the least distance of the triangle TRI to the vertex (i,j,k).
double point_2_plane(int tri, int i, int j, int k, myPoint* inter)
{
	double  dist, temp[3];
	int fact;
	myPoint res, val[3];

	//1) First compute the shortest signed distance between the Vertex and the Plane of the Triangle.
	dist = ( ((( (double)i)* normals[tri].x +	((double)j)* normals[tri].y +
		((double)k)* normals[tri].z)) + distances[tri]);

	if (isZero(dist))
	{
		res.x =1.0f*i;	res.y =1.0f*j;	res.z =1.0f*k;
		if(point_in_polygon(res, tri))
		{
			(*inter).x = res.x;		(*inter).y = res.y;			(*inter).z = res.z;
			return fabs(dist);
		}
	}
	if (dist <0) fact = -1;
	else fact = 1;

	//2) Then chq if the projected point is within the triangle or not. if yes, then the above is the correct shortest distance.
	res.x = (double)(i -normals[tri].x*dist);
	res.y = (double)(j -normals[tri].y*dist);
	res.z = (double)(k -normals[tri].z*dist);
	if (point_in_polygon(res, tri))
	{
		(*inter).x = res.x;		(*inter).y = res.y;			(*inter).z = res.z;
		return fabs(dist);
	}

    //3) now, project the planeProj onto the edge "i <-> j" and get the nearest point on the line segment to this point.
	temp[0] = getClipPoint(surface[tri].v1, surface[tri].v2, i, j, k, &val[0]);
    temp[1] = getClipPoint(surface[tri].v3, surface[tri].v2, i, j, k, &val[1]);
    temp[2] = getClipPoint(surface[tri].v1, surface[tri].v3, i, j, k, &val[2]);

    dist = sort_3_distances(temp, val, inter);

	if (dist >= (MAX_DIST) || dist <= (-1*MAX_DIST))
		printf("err vert= %d %d %d tri= %d\n", i, j, k, tri);
	return dist;
}

//compute the distance of each Triangle in the cell (ci,cj,ck) from the Vertex vert.
int each_cell(int ci, int cj, int ck, int vi, int vj, int vk)
{
	int vert, currrentTri;
	listnode* currNode;
	double val;
	myPoint temp;
	int res =0;

	vert = index2vert(vi, vj, vk);
	currNode = sdf[ci][cj][ck].tindex;
if ((vert==10301) || (vert ==10302))
res=0;
	while (currNode != NULL)
	{
		currrentTri = currNode->index;
		val = (double)point_2_plane(currrentTri, vi, vj, vk, &temp);
		if (val < values[vert].value )
		{
			values[vert].value = (float)val;
			values[vert].closestV.x = temp.x;
			values[vert].closestV.y = temp.y;
			values[vert].closestV.z = temp.z;
		}

		currNode = currNode->next;
		res =1;
	}

	values[vert].processed =1;

	if (values[vert].value >= (MAX_DIST) || values[vert].value <= (-1*MAX_DIST))
		printf("err vert= %d %d %d \n", vi, vj, vk);

	return res;
}

//Just compute the Distance Function for the given vert.
void compute_SDF(int i, int j, int k)
{
	int level, ci, cj, ck;

	//Get the corresponding Octree cell.
	if (i == size) ci = i-1;	else	ci=i;
	if (j == size) cj = j-1;	else	cj=j;
	if (k == size) ck = k-1;	else	ck=k;

	level =1;

	for (ci=i-level; ci<=i+level; ci++)
	{
		for (cj=j-level; cj<=j+level; cj++)
		{
			for (ck=k-level; ck<=k+level; ck++)
			{
				if ((ci < 0) || (ci >= size))  continue;
				if ((cj < 0) || (cj >= size))  continue;
				if ((ck < 0) || (ck >= size))  continue;

				if (sdf[ci][cj][ck].useful >0)
					each_cell(ci, cj, ck, i, j, k);
			}
		}
	}
}

void compute_boundarySDF()
{
	int ind, i, j, k;

	for (ind =0; ind<all_verts_touched; ind++)
	{
		_vert2index(queues[ind], i, j, k);
		compute_SDF(i, j, k);
		if (ind%5000 ==0) printf("computing the boundary SDF %3.4f %% over\n", (100.0*ind/all_verts_touched));
	}
}


void compute_signs()
{
	int i, j, k, sgn, ind;

	printf("\nnow going to compute.\n");
	//FILE* fp = fopen("chq.txt","w");

	for(i=0; i<=size; i++)
	{
		//fprintf(fp, "%d\n", i);
		for(j=0; j<=size; j++)
		{
			for(k=0; k<=size; k++)
			{
				sgn= klc_assign(i, j, k);
				ind = index2vert(i, j, k);
				values[ind].signe = sgn;

				//if (sgn== -1)
				//	fprintf(fp, "0");
				//else
				//	fprintf(fp, "1");
			}
			//fprintf(fp, "\n");
		}
		printf("SIGN: %d %d %d \n",i, j, k);
	}
	//fclose(fp);
}
void compute()
{
	int grid_pts;
	int i, j, k, m, ind, prevMin, prevVerts;
	double t1, t2 ,t;

	//First compute the SIGN for all the volume grid points.
	t1 = getTime();
	compute_signs();
	t2 = getTime();		t = t2-t1;
	printf("Sign computations done in %f seconds\n", (t2-t1));

	//Next, compute the SDF for the boundary voxels.
	t1 = getTime();
	compute_boundarySDF();
	t2 = getTime();		t += t2-t1;
	printf("Function evaluated at the %d boundary vertices in %f seconds\n",all_verts_touched,  (t2-t1));

	//Finally, propagate the distances to the interior voxels.
	grid_pts = (size+1)*(size+1)*(size+1);
	printf("total grid points: %d and starting with %d points\n", grid_pts, all_verts_touched);

	m=0;	prevMin =0;		prevVerts = all_verts_touched;		t1 = getTime();

	do {
		//FOR EACH VOXEL IN THE BOUNDARY_VERTS
		for (ind=prevMin; ind<prevVerts; ind++)
		{
			_vert2index(queues[ind], i, j, k);

			if ( (prevMin !=0) && (values[queues[ind]].processed == 1)	)	continue;

			apply_distance_transform(i, j, k);

			values[queues[ind]].processed =1;
			if (ind%10000 ==0)	printf("iter#%d: %d processed\n", m, ind);
		}

		prevMin = prevVerts;
		prevVerts = all_verts_touched;
		m++;
		printf("in Iteration# %d, with %d vertices in the queue\n", m, prevVerts);

		if (prevMin == prevVerts)
		{
			printf("SDF propagation saturated. Now, checking for untouched voxels... \n");
			confirm_SDF(0);
			break;
		}

	} while(all_verts_touched != grid_pts);

	t2 = getTime();		t += t2-t1;
	printf("Distance Propagation for %d grid points done in %f seconds\n", all_verts_touched, (t2-t1));
	printf("All of the SDF computations are done in %f seconds!!! \n", t);

	confirm_SDF(1);
}
