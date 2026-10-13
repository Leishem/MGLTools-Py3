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


/*
 * Sample parameters: -i eight.raw -e 0 -s 64
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sdfLib.h"

int size;
bool normal,insidezero;
int nverts, ntris;
int* tris = NULL;
float* verts = NULL;
float* values = NULL;
char* ifn = NULL;

double buffarr[6];

//Functions needed by the RAWIV file writer...
int putFloat(float *flts, int n, FILE *fp)
{
//#ifndef WIN32
//	return fwrite(flts, sizeof(float), n, fp);
//#else

	unsigned char *pb = new unsigned char[n*4];
	unsigned char *pf = (unsigned char *)flts;
    int nbytes;

	//swap the byte order
	for(int i = 0; i < n; i++)
	{
		pb[4*i] = pf[4*i+3];
		pb[4*i+1] = pf[4*i+2];
		pb[4*i+2] = pf[4*i+1];
		pb[4*i+3] = pf[4*i];
	}

	nbytes = fwrite(pb, 1, 4*n, fp);
	delete pb;
	return nbytes;
//#endif
}


int putInt(int *Ints, int n, FILE *fp)
{
//#ifndef WIN32
//	return fwrite(Ints, sizeof(int), n, fp);
//#else

	unsigned char *pb = new unsigned char[4*n];
	unsigned char *pf = (unsigned char *)Ints;
	int	nbytes;

	//swap the byte order
	for(int i = 0; i < n; i++)
	{
		pb[4*i] = pf[4*i+3];
		pb[4*i+1] = pf[4*i+2];
		pb[4*i+2] = pf[4*i+1];
		pb[4*i+3] = pf[4*i];
	}

	nbytes=fwrite(pb, 1, 4*n, fp);
	delete pb;
	return nbytes;
//#endif
}

void write_RAWIV()
{
    FILE	*fp;
	long int max;
	char buff[4000];
	int temp;
	float tfloat;

	sprintf(buff, "output.rawiv");

	if ((fp=fopen(buff,"wb")) == NULL)
	{
		printf("Cannot open the Output file for RAW output\n");
		exit(0);
	}

	printf("writing head info \n");

	//The origin 0,0,0
	tfloat = 0;
	putFloat(&tfloat,1,fp);  putFloat(&tfloat,1,fp);  putFloat(&tfloat,1,fp);

	//The max (size+1),(size+1),(size+1)
	tfloat = (float) size+1;
	putFloat(&tfloat,1,fp);  putFloat(&tfloat,1,fp);  putFloat(&tfloat,1,fp);

	//The #of vertices
	temp=(size+1)*(size+1)*(size+1);
	putInt(&temp,1,fp);

	//The #of cells
	temp=(size)*(size)*(size);
	putInt(&temp,1,fp);

	//The dim of the volume
	temp=size+1;
	putInt(&temp,1,fp); putInt(&temp,1,fp); putInt(&temp,1,fp);

	//The Origin of the volume
	tfloat = 0;
	putFloat(&tfloat,1,fp);  putFloat(&tfloat,1,fp);  putFloat(&tfloat,1,fp); // origin

	//The span of the volume
	tfloat = 1;
	putFloat(&tfloat,1,fp); putFloat(&tfloat,1,fp); putFloat(&tfloat,1,fp); //span

	printf("writing data \n");

	max = (long int)((size+1)* (size+1) * (size+1)); //for the Voxels.
	putFloat(&(values[0]),max,fp);

	fclose(fp);
}

void readGeometry(char* ifname)
{
    FILE	*fp;
    int	i;
	float temp[3];

    if ((fp = fopen(ifname, "r")) == NULL)
	{
		fprintf(stderr, "ERROR: fopen(%s)\n", ifname);
		exit(0);
    }

    printf("Reading Geometry: %s\n", ifname);

	if (fscanf(fp,"%d %d", &nverts, &ntris) == EOF)
	{
		printf("Input file is not valid....Exiting...\n");
		exit(0);
	}

	printf("vert= %d and tri = %d \n", nverts,ntris);

	verts = (float*) malloc (sizeof (float) *3 * nverts);
	tris = (int*) malloc (sizeof (int) *3 * ntris);

	for (i=0; i<nverts; i++)
	{
		if (fscanf(fp,"%f %f %f", &temp[0], &temp[1], &temp[2] ) == EOF)
		{
			printf("Input file has to have %d Vertices....Exiting...\n",nverts);
			exit(0);
		}

		verts[3*i+0] = temp[0];	verts[3*i+1] = temp[1];	verts[3*i+2] = temp[2];

		if (!(i% 5000))
			printf("still working on points !!!! %d \n",i);
	}

	printf("Finished reading the Vertices.. Now reading the Triangles\n");

	for (i=0; i<ntris; i++)
	{
		if (fscanf(fp,"%d %d %d", &tris[3*i+0], &tris[3*i+1], &tris[3*i+2] ) == EOF)
		{
			printf("Input file has to have %d Triangles....Exiting...\n",ntris);
			exit(0);
		}

		if (!(i% 5000))
			printf("still working on Triangles !!!! %d \n",i);
	}

	fclose(fp);
	printf("File %s read.. \n",ifname);
}

static void usage()
{
	printf("Usage: sdf -i <input file> -s <grid_size> -n <flip_normals> -z <insideZero> -h <help_msg>\n");
	printf("\t <input_file> (string) Name of the input file (.RAW format)\n");
	printf("\t <grid_size> (int) The grid size has to be 64, 128 or 256 or 512\n");
	printf("\t <flip_normals> (0 or 1) Flip normals of the input file?\n");

}

void parse_config (int argc, char *argv[])
{
	int i, j;

	for (i=1; i<argc; i++)
	{
		if (!strcmp(argv[i],"-h") && !strcmp(argv[i],"-H"))
		{
			usage();
			exit(0);
		}

		if (!strcmp("-i",argv[i]) || !strcmp("-I",argv[i]))
			ifn=argv[++i];
		else if (!strcmp("-s",argv[i]) || !strcmp("-S",argv[i]))
			size=atoi(argv[++i]);
		else if (!strcmp("-n",argv[i]) || !strcmp("-N",argv[i]))
		{
			j =atoi(argv[++i]);
			if (j==0)	normal =0;
			else		normal =1;
		}
		else if (!strcmp("-z",argv[i]) || !strcmp("-Z",argv[i]))
		{
			j =atoi(argv[++i]);
			if (j==0)	insidezero =0;
			else		insidezero =1;
		}
	}
}


int main(int argc, char *argv[])
{

        buffarr[0] =6;		buffarr[1] =20;
	buffarr[2] =1;		buffarr[3] =5;
	buffarr[4] =5;		buffarr[5] =17;
	size = 64;
	normal =0;
	insidezero =1;

	parse_config(argc, argv);

	if(ifn == NULL)
	{
		printf("ifname is null\n");
		usage();
		exit(1);
	}

	if ((size!=16) &&(size!=32) &&(size!=64) && (size!=128) && (size!=256) &&(size!=512) &&(size!=1024))
	{
		printf("size is incorrect\n");
		usage();
		exit(1);
	}

	//First, set the parameters.

	setParameters(size, normal, insidezero, buffarr);

	//Then, read the input data.
	readGeometry(ifn);

	//Finally, call the SDF code.
	values = computeSDF(nverts, verts, ntris, tris);

	write_RAWIV();

	return 0;
}
