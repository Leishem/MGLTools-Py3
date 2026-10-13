#include<stdio.h>
#include<assert.h>

#include"cellQueue.h"
#include"octree.h"
#include"e_face.h"
#include"cubes.h"
#include"vtkMarchingCubesCases.h"
#include"pcio.h"
#include"geoframe.h"
#include"normalspline.h"

int* interpol_bit;

Octree::Octree()
{
	//vol_min=0; vol_max=1;
	vol_min=1; vol_max=0;
}

Octree::~Octree()
{
	free(oct_array);
	free(cut_array);
	free(orig_vol);
	free(vtx_idx_arr);
	free(vtx_idx_arr_in);
	free(grid_idx_arr);
	free(minmax);
	// free(BSplineCoeff); //commented out A.O.
	free(vtx_idx_arr_refine);
}


// given the path of a volume,
// load the volume data and construct octree from that
// allocate associate arrays
void Octree::Octree_init(const char* rawiv_fname)
{
	
	vol_fp=fopen(rawiv_fname,"rb");
	
	if (vol_fp==NULL) {
		printf("wrong name : %s\n",rawiv_fname);
		return;
	}

	leaf_num=0;

	read_header();   // read dimension, span, orig and so on

	//printf ("header info: \n");
	//printf("dim[3]: %d, %d, %d \n", dim[0], dim[1], dim[2]);
	//printf ("minext[3]: %f, %f, %f \n", minext[0], minext[1], minext[2]);
	//printf ("maxext[3]: %f, %f, %f \n", maxext[0], maxext[1], maxext[2]);
	//printf ("nverts = %d, ncells = %d \n", nverts, ncells);
	//printf ("orig[3]: %f, %f, %f \n", orig[0], orig[1], orig[2]);
	//printf ("span[3]: %f, %f, %f \n", span[0], span[1], span[2]);
	
	// octree information
	oct_depth   = get_depth(dim[0]);
	octcell_num = get_octcell_num(oct_depth);
	cell_num    = (dim[0]-1)*(dim[1]-1)*(dim[2]-1);

	oct_array   = (octcell*)malloc(sizeof(octcell)*octcell_num);

	memset(oct_array,0,sizeof(octcell)*octcell_num);

	minmax		= (MinMax*)malloc(sizeof(MinMax)*octcell_num);

	memset(minmax,0,sizeof(MinMax)*octcell_num);

	cut_array   = (int*)malloc(sizeof(int)*2*cell_num);


	orig_vol    = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);

	ebit		= (char*)malloc(sizeof(char)*octcell_num*4/8);
	vbit		= (char*)malloc(sizeof(char)*octcell_num*4/8);

	vtx_idx_arr = (int*)malloc(sizeof(int)*octcell_num);
	grid_idx_arr = (int*)malloc(sizeof(int)*dim[0]*dim[1]*dim[2]);
	vtx_idx_arr_in = (int*)malloc(sizeof(int)*octcell_num);
	vtx_idx_arr_refine = (int*)malloc(sizeof(int)*octcell_num);
	int k;
	for (k=0;k<octcell_num;k++) {
		vtx_idx_arr[k]=-1;	vtx_idx_arr_in[k]=-1;	vtx_idx_arr_refine[k]=-1;}
	for (k=0;k<dim[0]*dim[1]*dim[2];k++) grid_idx_arr[k]=-1;	

	qef_array	= (double**) malloc (sizeof(double*) * octcell_num);
	qef_array_in = (double**) malloc (sizeof(double*) * octcell_num);

	assert(qef_array!=NULL);
	assert(qef_array_in!=NULL);

	memset(qef_array,0,octcell_num*sizeof(double*));
	memset(qef_array_in,0,octcell_num*sizeof(double*));

	memset(ebit,0,octcell_num*4/8*sizeof(char));
	memset(vbit,0,octcell_num*4/8*sizeof(char));
	
	read_data();

	// Bspline Interpolation
	//BSplineCoeff = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);
	//TransImg2Spline(orig_vol, BSplineCoeff, dim[0], dim[1], dim[2]);

	int i;
	for (i=0;i<=oct_depth;i++) {
		level_res[i]=(1<<i);
	}
//	e_face_initialization();
	construct_octree((char*)rawiv_fname);
	vol_min=minmax[0].min;
	vol_max=minmax[0].max;
}

bool Octree::Octree_init_from_data(const unsigned char* data, 
				   int dims[3], 
				   unsigned int numVerts,
				   unsigned int numCells,
				   float origin[3], float spans[3])
{
  
  if (data==NULL) {
    printf("Error: data == NULL \n");
    return false;
  }
  
  leaf_num=0;
  
  
  // get dimension, span, orig and so on
  dim[0] = dims[0];
  dim[1] = dims[1];
  dim[2] = dims[2];
  
  minext[0] = 0.0;
  minext[1] = 0.0;
  minext[2] = 0.0;
  
  maxext[0] = dim[0]-1.0;
  maxext[1] = dim[1]-1.0;
  maxext[2] = dim[2]-1.0;
  
  nverts = numVerts;
  ncells = numCells;
  
  if (origin)
    {
      printf("in Octree_init_from_data: setting the origin\n");
      orig[0] = origin[0];
      orig[1] = origin[1];
      orig[2] = origin[2];
    }
  else 
    {
      orig[0] = 0.0;
      orig[1] = 0.0;
      orig[2] = 0.0;
    }
  if (spans)
    {
      span[0] = spans[0];
      span[1] = spans[1];
      span[2] = spans[2];
    }
  else
    {
      span[0] = 1.0;
      span[1] = 1.0;
      span[2] = 1.0;
    }
  // octree information
  oct_depth   = get_depth(dim[0]);
  octcell_num = get_octcell_num(oct_depth);
  cell_num    = (dim[0]-1)*(dim[1]-1)*(dim[2]-1);
  
  oct_array   = (octcell*)malloc(sizeof(octcell)*octcell_num);
  
  memset(oct_array,0,sizeof(octcell)*octcell_num);
  
  minmax		= (MinMax*)malloc(sizeof(MinMax)*octcell_num);
  
  memset(minmax,0,sizeof(MinMax)*octcell_num);
  
  cut_array   = (int*)malloc(sizeof(int)*2*cell_num);
  
  
  orig_vol    = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);
  
  ebit		= (char*)malloc(sizeof(char)*octcell_num*4/8);
  vbit		= (char*)malloc(sizeof(char)*octcell_num*4/8);
  
  vtx_idx_arr = (int*)malloc(sizeof(int)*octcell_num);
  grid_idx_arr = (int*)malloc(sizeof(int)*dim[0]*dim[1]*dim[2]);
  vtx_idx_arr_in = (int*)malloc(sizeof(int)*octcell_num);
  vtx_idx_arr_refine = (int*)malloc(sizeof(int)*octcell_num);
  int k;
  for (k=0;k<octcell_num;k++) {
    vtx_idx_arr[k]=-1;	vtx_idx_arr_in[k]=-1;	vtx_idx_arr_refine[k]=-1;}
  for (k=0;k<dim[0]*dim[1]*dim[2];k++) grid_idx_arr[k]=-1;	
  
  qef_array	= (double**) malloc (sizeof(double*) * octcell_num);
  qef_array_in = (double**) malloc (sizeof(double*) * octcell_num);
  
  assert(qef_array!=NULL);
  assert(qef_array_in!=NULL);
  
  memset(qef_array,0,octcell_num*sizeof(double*));
  memset(qef_array_in,0,octcell_num*sizeof(double*));
  
  memset(ebit,0,octcell_num*4/8*sizeof(char));
  memset(vbit,0,octcell_num*4/8*sizeof(char));
  
  //read_data();
  int i;
  unsigned char *pf = (unsigned char *)orig_vol;
  for(i = 0; i < dim[0]*dim[1]*dim[2]; i++) 
    {
      //pf[4*i] = data[4*i+3];
      //pf[4*i+1] = data[4*i+2];
      //pf[4*i+2] = data[4*i+1];
      //pf[4*i+3] = data[4*i];
      pf[4*i]   = data[4*i];
      pf[4*i+1] = data[4*i+1];
      pf[4*i+2] = data[4*i+2];
      pf[4*i+3] = data[4*i+3];
      orig_vol[i] = - orig_vol[i];
      //if (i<10)
      //printf ("orig_vol[%d]=%f\n", i, orig_vol[i]);
    }

  //  for(i = 0; i < dim[0]*dim[1]*dim[2]; i++) {
//      orig_vol[i] = - orig_vol[i];
//      if (i<10)
//        printf ("orig_vol[%d]=%f\n", i, orig_vol[i]);
//    }
  
  // Bspline Interpolation
  //BSplineCoeff = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);
  //TransImg2Spline(orig_vol, BSplineCoeff, dim[0], dim[1], dim[2]);
  
  
  for (i=0;i<=oct_depth;i++) {
    level_res[i]=(1<<i);
  }
  //	e_face_initialization();
  // construct_octree((char*)rawiv_fname);
  int oc_idx=0;
  int level;
  float min , max;
  while (oc_idx < octcell_num) {
    level=get_level(oc_idx);
    compute_error(oc_idx,level,min,max);
    minmax[oc_idx].min=min;
    minmax[oc_idx].max=max;
    oc_idx++;
  }
  vol_min=minmax[0].min;
  vol_max=minmax[0].max;
  return true;
}

// error computation on each cell of octree
void Octree::construct_octree(char * rawiv_fname)
{
	int oc_idx=0;
	int level;
	FILE* err_fp;
	char err_fname[256];
	float min , max;
	
	strcpy(err_fname,rawiv_fname);
	strcat(err_fname,".err");
	
	err_fp=fopen(err_fname,"rb");
	
	if (err_fp) {
		
		//getFloat((float*)minmax,octcell_num*2,err_fp);
		fread((float*)minmax,sizeof(float),octcell_num*2,err_fp);
		fclose(err_fp);
		
	} else {

		while (oc_idx < octcell_num) {

			level=get_level(oc_idx);
			compute_error(oc_idx,level,min,max);
			minmax[oc_idx].min=min;
			minmax[oc_idx].max=max;
			//oct_array[oc_idx].refine_flag=0;
			oc_idx++;

		} 

		err_fp=fopen(err_fname, "wb");
		//putFloat((float*)minmax, octcell_num*2, err_fp);
		fwrite((float*)minmax,sizeof(float),octcell_num*2,err_fp);
		fclose(err_fp);
	}
	
}

void Octree::collapse()
{	
	CellQueue prev_queue, cur_queue;
	int i, oc_id, level;

	oc_id=0;

	prev_queue.Add(oc_id);
	while (!prev_queue.Empty()) {
		
		//int leaf_idx=leaf_num;
		while (prev_queue.Get(oc_id) >= 0)  {
			level=get_level(oc_id);
			
			//octcell2xyz(oc_id,x,y,z,level);
			
//			if (is_skipcell(oc_id) || level==oct_depth) {
			if (is_skipcell(oc_id) || level==oct_depth || minmax[oc_id].max < iso_val) {

				//cut_array[leaf_num++]=oc_id;
				oct_array[oc_id].refine_flag=0;

			} 
			else {
				oct_array[oc_id].refine_flag=1;
				cur_queue.Add(oc_id);
			}
		}
		
		while (cur_queue.Get(oc_id)>=0) {
			level=get_level(oc_id);
			for (i=0; i<8; i++)
				prev_queue.Add(child(oc_id,level,i));
		}
		
	}
	//return leaf_idx;
}

void Octree::collapse_interval()
{	
	CellQueue prev_queue, cur_queue;
	int i, oc_id, level;

	oc_id=0;

	prev_queue.Add(oc_id);
	while (!prev_queue.Empty()) {
		
		//int leaf_idx=leaf_num;
		while (prev_queue.Get(oc_id) >= 0)  {
			level=get_level(oc_id);
			
			//octcell2xyz(oc_id,x,y,z,level);
			
//			if (is_skipcell(oc_id) || level==oct_depth) {
			if (is_skipcell_interval(oc_id) || level==oct_depth) {

				//cut_array[leaf_num++]=oc_id;
				oct_array[oc_id].refine_flag=0;

			} 
			else {
				oct_array[oc_id].refine_flag=1;
				cur_queue.Add(oc_id);
			}
		}
		
		while (cur_queue.Get(oc_id)>=0) {
			level=get_level(oc_id);
			for (i=0; i<8; i++)
				prev_queue.Add(child(oc_id,level,i));
		}
		
	}
	//return leaf_idx;
}

int Octree::cell_comp(int oc_id, int level, float pt[12][3], float norm[12][3])
{
	float val[8], f1, f2;
	int code, e, i, j, k, edge;

	getCellValues(oc_id,level,val);

	code = 0;
	
	if (val[0] < iso_val) code |= 0x01;
	if (val[1] < iso_val) code |= 0x02;
	if (val[2] < iso_val) code |= 0x04;
	if (val[3] < iso_val) code |= 0x08;
	if (val[4] < iso_val) code |= 0x10;
	if (val[5] < iso_val) code |= 0x20;
	if (val[6] < iso_val) code |= 0x40;
	if (val[7] < iso_val) code |= 0x80;
/*
	if (val[0] > iso_val) code |= 0x01;
	if (val[1] > iso_val) code |= 0x02;
	if (val[2] > iso_val) code |= 0x04;
	if (val[3] > iso_val) code |= 0x08;
	if (val[4] > iso_val) code |= 0x10;
	if (val[5] > iso_val) code |= 0x20;
	if (val[6] > iso_val) code |= 0x40;
	if (val[7] > iso_val) code |= 0x80;
*/
	assert(code!=0 && code!=255);
		
	octcell2xyz(oc_id,i,j,k,level);
		
	for (e=0 ; e < cubeedges[code][0] ; e++) {
		edge = cubeedges[code][1+e];
		EdgeInfo *ei = &edgeinfo[edge];
		
		f1=val[ei->d1];
		f2=val[ei->d2];
	
		switch (ei->dir) {
			case 0:
				interpRect3Dpts_x(i+ei->di, j+ei->dj, k+ei->dk,
					f1, f2, iso_val, pt[e], norm[e], level);
				break;
		
			case 1:
				interpRect3Dpts_y(i+ei->di, j+ei->dj, k+ei->dk,
					f1, f2, iso_val, pt[e], norm[e], level);
				break;
		
			case 2:
				interpRect3Dpts_z(i+ei->di, j+ei->dj, k+ei->dk,
					f1, f2, iso_val, pt[e], norm[e], level);
				break;
		}
	}
	return cubeedges[code][0];
}

int Octree::cell_comp_in(int oc_id, int level, float pt[12][3], float norm[12][3])
{
	float val[8], f1, f2;
	int code, e, i, j, k, edge;

	getCellValues(oc_id,level,val);

	code = 0;
	
	if (val[0] > iso_val_in) code |= 0x01;
	if (val[1] > iso_val_in) code |= 0x02;
	if (val[2] > iso_val_in) code |= 0x04;
	if (val[3] > iso_val_in) code |= 0x08;
	if (val[4] > iso_val_in) code |= 0x10;
	if (val[5] > iso_val_in) code |= 0x20;
	if (val[6] > iso_val_in) code |= 0x40;
	if (val[7] > iso_val_in) code |= 0x80;

	assert(code!=0 && code!=255);
		
	octcell2xyz(oc_id,i,j,k,level);
		
	for (e=0 ; e < cubeedges[code][0] ; e++) {
		edge = cubeedges[code][1+e];
		EdgeInfo *ei = &edgeinfo[edge];
		
		f1=val[ei->d1];
		f2=val[ei->d2];
	
		switch (ei->dir) {
			case 0:
				interpRect3Dpts_x(i+ei->di, j+ei->dj, k+ei->dk,
					f1, f2, iso_val_in, pt[e], norm[e], level);
				break;
		
			case 1:
				interpRect3Dpts_y(i+ei->di, j+ei->dj, k+ei->dk,
					f1, f2, iso_val_in, pt[e], norm[e], level);
				break;
		
			case 2:
				interpRect3Dpts_z(i+ei->di, j+ei->dj, k+ei->dk,
					f1, f2, iso_val_in, pt[e], norm[e], level);
				break;
		}
	}
	return cubeedges[code][0];
}

void Octree::clear (double* a, double* b, double* c)
{
	int i;
	for (i=0;i<3;i++) {
		a[i]=0;
		b[i]=0;
		c[i]=0;
	}

}

void Octree::clear (double* a)
{
	int i;
	for (i=0;i<3;i++) {
		a[i]=0;		
	}

}

void Octree::get_qef(int child_idx,double* temp_sigma_ni_2,double* temp_sigma_ni_2_pi,double* temp_sigma_ni_2_pi_2)
{
	if (qef_array[child_idx]) {
		temp_sigma_ni_2[0]=qef_array[child_idx][0];
		temp_sigma_ni_2[1]=qef_array[child_idx][1];
		temp_sigma_ni_2[2]=qef_array[child_idx][2];
		
		temp_sigma_ni_2_pi[0]=qef_array[child_idx][3];
		temp_sigma_ni_2_pi[1]=qef_array[child_idx][4];
		temp_sigma_ni_2_pi[2]=qef_array[child_idx][5];
		
		temp_sigma_ni_2_pi_2[0]=qef_array[child_idx][6];
		temp_sigma_ni_2_pi_2[1]=qef_array[child_idx][7];
		temp_sigma_ni_2_pi_2[2]=qef_array[child_idx][8];
	} else {
		//assert(qef_array[child_idx]!=NULL);
		temp_sigma_ni_2[0]=0;
		temp_sigma_ni_2[1]=0;
		temp_sigma_ni_2[2]=0;
		
		temp_sigma_ni_2_pi[0]=0;
		temp_sigma_ni_2_pi[1]=0;
		temp_sigma_ni_2_pi[2]=0;
		
		temp_sigma_ni_2_pi_2[0]=0;
		temp_sigma_ni_2_pi_2[1]=0;
		temp_sigma_ni_2_pi_2[2]=0;
	}

}

void Octree::get_qef_in(int child_idx,double* temp_sigma_ni_2,double* temp_sigma_ni_2_pi,double* temp_sigma_ni_2_pi_2)
{
	if (qef_array_in[child_idx]) {
		temp_sigma_ni_2[0]=qef_array_in[child_idx][0];
		temp_sigma_ni_2[1]=qef_array_in[child_idx][1];
		temp_sigma_ni_2[2]=qef_array_in[child_idx][2];
		
		temp_sigma_ni_2_pi[0]=qef_array_in[child_idx][3];
		temp_sigma_ni_2_pi[1]=qef_array_in[child_idx][4];
		temp_sigma_ni_2_pi[2]=qef_array_in[child_idx][5];
		
		temp_sigma_ni_2_pi_2[0]=qef_array_in[child_idx][6];
		temp_sigma_ni_2_pi_2[1]=qef_array_in[child_idx][7];
		temp_sigma_ni_2_pi_2[2]=qef_array_in[child_idx][8];
	} else {
		//assert(qef_array_in[child_idx]!=NULL);
		temp_sigma_ni_2[0]=0;
		temp_sigma_ni_2[1]=0;
		temp_sigma_ni_2[2]=0;
		
		temp_sigma_ni_2_pi[0]=0;
		temp_sigma_ni_2_pi[1]=0;
		temp_sigma_ni_2_pi[2]=0;
		
		temp_sigma_ni_2_pi_2[0]=0;
		temp_sigma_ni_2_pi_2[1]=0;
		temp_sigma_ni_2_pi_2[2]=0;
	}

}


void Octree::compute_qef()
{
	float norm[12][3], p[12][3];
	double sigma_ni_2[3], sigma_ni_2_pi[3], sigma_ni_2_pi_2[3], qef;
	double temp_sigma_ni_2[3], temp_sigma_ni_2_pi[3], temp_sigma_ni_2_pi_2[3],solution[3];
	int child_idx, intersect_num;
	int oc_id, level, i, x,y,z;
	
	for (oc_id=level_id[oct_depth];oc_id<level_id[oct_depth+1];oc_id++) {
		if (is_skipcell(oc_id)) continue;
		level=get_level(oc_id);
		octcell2xyz(oc_id,x,y,z,level);

		clear(sigma_ni_2,sigma_ni_2_pi,sigma_ni_2_pi_2);
		intersect_num = cell_comp(oc_id, level, p, norm);

		int j;
		for (j=0;j<intersect_num;j++) {
			for (i=0;i<3;i++) {
				sigma_ni_2[i]      += norm[j][i]*norm[j][i];
				sigma_ni_2_pi[i]   += norm[j][i]*norm[j][i]*p[j][i];
				sigma_ni_2_pi_2[i] += norm[j][i]*norm[j][i]*p[j][i]*p[j][i];
			}

		}

		for (i=0;i<3;i++)
			solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i] ;
/*
		// Jessica begin
		int cell_size = (dim[0]-1)/(1<<level);
		if (!(solution[0] > x*cell_size && solution[0] < (x+1)*cell_size) ||
			!(solution[1] > y*cell_size && solution[1] < (y+1)*cell_size) ||
			!(solution[2] > z*cell_size && solution[2] < (z+1)*cell_size)) {
			solution[0] = 0.0;		solution[1] = 0.0;		solution[2] = 0.0;
			for (i = 0; i < intersect_num; i++) {
				solution[0] += p[i][0] / intersect_num;
				solution[1] += p[i][1] / intersect_num;
				solution[2] += p[i][2] / intersect_num;
			}
		}
		// Jessica end
*/
		qef=0;
		for (i=0;i<3;i++)
			qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i] ;

		put_qef(oc_id, sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);
	}

	int l;
	for (l=oct_depth-1;l>=0;l--) {
		for (oc_id=level_id[l];oc_id<level_id[l+1];oc_id++) {
			level=l;

			if (oct_array[oc_id].refine_flag==0) continue;

			clear (temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
			clear (sigma_ni_2,sigma_ni_2_pi,sigma_ni_2_pi_2);
			clear (solution);

			qef=0;
			
			//child(oc_id,level,cid);
			int k;
			for (k=0;k<8;k++) {
				child_idx=child(oc_id,level,k);
				//if (oct_array[child_idx].refine_flag==0) continue;
				if (is_skipcell(child_idx)) continue;
				get_qef(child_idx,temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
				int i;
				for (i=0;i<3;i++) {
					sigma_ni_2[i]+=temp_sigma_ni_2[i];
					sigma_ni_2_pi[i]+=temp_sigma_ni_2_pi[i];
					sigma_ni_2_pi_2[i]+=temp_sigma_ni_2_pi_2[i];
				}
			}
			for (i=0;i<3;i++)
				solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i];
			qef=0;
			for (i=0;i<3;i++) 
				qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i] ;
			put_qef(oc_id,sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);

		}
	}
	
}

void Octree::compute_qef_interval()
{
	float norm[12][3], p[12][3];
	double sigma_ni_2[3], sigma_ni_2_pi[3], sigma_ni_2_pi_2[3], qef;
	double temp_sigma_ni_2[3], temp_sigma_ni_2_pi[3], temp_sigma_ni_2_pi_2[3],solution[3];
	int child_idx, intersect_num, oc_id;
	int level, i, k, x, y, z;
	
	for (oc_id = level_id[oct_depth]; oc_id < level_id[oct_depth+1]; oc_id++) {
		if(is_skipcell_interval(oc_id)) continue;
		level = get_level(oc_id);
		octcell2xyz(oc_id, x, y, z, level);

		clear(sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2);
		
		if(! is_skipcell(oc_id)) {
			intersect_num = cell_comp(oc_id, level, p, norm);
			int j;
			for (j=0;j<intersect_num;j++) {
				for (i=0;i<3;i++) {
					sigma_ni_2[i]      += norm[j][i]*norm[j][i];
					sigma_ni_2_pi[i]   += norm[j][i]*norm[j][i]*p[j][i];
					sigma_ni_2_pi_2[i] += norm[j][i]*norm[j][i]*p[j][i]*p[j][i];
				}

			}

			for (i=0;i<3;i++)
				solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i] ;

			qef=0;
			for (i=0;i<3;i++)
				qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i] ;

			put_qef(oc_id, sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);
		}

		clear(sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2);

		if(! is_skipcell_in(oc_id)) {
			intersect_num = cell_comp_in(oc_id, level, p, norm);
			int j;
			for (j=0;j<intersect_num;j++) {
				for (i=0;i<3;i++) {
					sigma_ni_2[i]      += norm[j][i]*norm[j][i];
					sigma_ni_2_pi[i]   += norm[j][i]*norm[j][i]*p[j][i];
					sigma_ni_2_pi_2[i] += norm[j][i]*norm[j][i]*p[j][i]*p[j][i];
				}

			}

			for (i=0;i<3;i++)
				solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i] ;

			qef=0;
			for (i=0;i<3;i++)
				qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i] ;

			put_qef_in(oc_id, sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);
		}
	}

	int l;
	for (l=oct_depth-1;l>=0;l--) {
		for (oc_id = level_id[l]; oc_id < level_id[l+1]; oc_id++) {
			level=l;

			if (oct_array[oc_id].refine_flag == 0) continue;

			clear (temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
			clear (sigma_ni_2,sigma_ni_2_pi,sigma_ni_2_pi_2);
			clear (solution);

			//child(oc_id,level,cid);
			for (k=0;k<8;k++) {
				child_idx = child(oc_id,level,k);
				//if (oct_array[child_idx].refine_flag==0) continue;
				if (is_skipcell(child_idx)) continue;
				get_qef(child_idx,temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
				for (i=0;i<3;i++) {
					sigma_ni_2[i]+=temp_sigma_ni_2[i];
					sigma_ni_2_pi[i]+=temp_sigma_ni_2_pi[i];
					sigma_ni_2_pi_2[i]+=temp_sigma_ni_2_pi_2[i];
				}
			}
			for (i=0;i<3;i++)
				solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i];
			qef=0;
			for (i=0;i<3;i++) 
				qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i];
			put_qef(oc_id,sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);

			clear (temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
			clear (sigma_ni_2,sigma_ni_2_pi,sigma_ni_2_pi_2);
			clear (solution);

			//child(oc_id,level,cid);
			for (k = 0; k < 8; k++) {
				child_idx = child(oc_id,level,k);
				//if (oct_array[child_idx].refine_flag==0) continue;
				if (is_skipcell_in(child_idx)) continue;
				get_qef_in(child_idx,temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
				for (i=0;i<3;i++) {
					sigma_ni_2[i]+=temp_sigma_ni_2[i];
					sigma_ni_2_pi[i]+=temp_sigma_ni_2_pi[i];
					sigma_ni_2_pi_2[i]+=temp_sigma_ni_2_pi_2[i];
				}
			}
			for (i=0;i<3;i++)
				solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i];
			qef=0;
			for (i=0;i<3;i++) 
				qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i];
			put_qef_in(oc_id,sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);

		}
	}
	
}
/*
void Octree::compute_qef_interval()
{
	float norm[12][3], p[12][3];
	double sigma_ni_2[3], sigma_ni_2_pi[3], sigma_ni_2_pi_2[3], qef;
	double temp_sigma_ni_2[3], temp_sigma_ni_2_pi[3], temp_sigma_ni_2_pi_2[3],solution[3];
	int child_idx, intersect_num, oc_id;
	int level, i, x, y, z, cell_size;
	
	for (oc_id=level_id[oct_depth];oc_id<level_id[oct_depth+1];oc_id++) {
		if (is_skipcell_interval(oc_id)) continue;
		level=get_level(oc_id);
		octcell2xyz(oc_id,x,y,z,level);

		clear(sigma_ni_2,sigma_ni_2_pi,sigma_ni_2_pi_2);
		if(! is_skipcell(oc_id))
			intersect_num = cell_comp(oc_id, level, p, norm);
		else
			intersect_num = cell_comp_in(oc_id, level, p, norm);
		for (int j=0;j<intersect_num;j++) {
			for (i=0;i<3;i++) {
				sigma_ni_2[i]      += norm[j][i]*norm[j][i];
				sigma_ni_2_pi[i]   += norm[j][i]*norm[j][i]*p[j][i];
				sigma_ni_2_pi_2[i] += norm[j][i]*norm[j][i]*p[j][i]*p[j][i];
			}

		}

		for (i=0;i<3;i++)
			solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i] ;

		qef=0;
		for (i=0;i<3;i++)
			qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i] ;

		put_qef(oc_id, sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);
	}

	for (int l=oct_depth-1;l>=0;l--) {
		for (oc_id=level_id[l];oc_id<level_id[l+1];oc_id++) {
			level=l;

			if (oct_array[oc_id].refine_flag==0) continue;

			clear (temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
			clear (sigma_ni_2,sigma_ni_2_pi,sigma_ni_2_pi_2);
			clear (solution);

			qef=0;
			
			//child(oc_id,level,cid);
			for (int k=0;k<8;k++) {
				child_idx=child(oc_id,level,k);
				//if (oct_array[child_idx].refine_flag==0) continue;
				if (is_skipcell_interval(child_idx)) continue;
				get_qef(child_idx,temp_sigma_ni_2,temp_sigma_ni_2_pi,temp_sigma_ni_2_pi_2);
				for (int i=0;i<3;i++) {
					sigma_ni_2[i]+=temp_sigma_ni_2[i];
					sigma_ni_2_pi[i]+=temp_sigma_ni_2_pi[i];
					sigma_ni_2_pi_2[i]+=temp_sigma_ni_2_pi_2[i];
				}
			}
			for (i=0;i<3;i++)
				solution[i] = sigma_ni_2_pi[i] / sigma_ni_2[i];
			qef=0;
			for (i=0;i<3;i++) 
				qef = sigma_ni_2_pi_2[i] - (sigma_ni_2_pi[i]*sigma_ni_2_pi[i]) / sigma_ni_2[i] ;
			put_qef(oc_id,sigma_ni_2, sigma_ni_2_pi, sigma_ni_2_pi_2, solution, qef);

		}
	}
	
}
*/
void Octree::put_qef(int oc_id, double* sigma_ni_2, double* sigma_ni_2_pi,double* sigma_ni_2_pi_2,double* solution,double qef)
{
	//assert(qef_array[oc_id]==NULL);

	if (qef_array[oc_id]==NULL) {
		qef_array[oc_id]=(double*)malloc(sizeof(double)*(3+3+3+3+1));
	}

	qef_array[oc_id][0]=sigma_ni_2[0];
	qef_array[oc_id][1]=sigma_ni_2[1];
	qef_array[oc_id][2]=sigma_ni_2[2];

	qef_array[oc_id][3]=sigma_ni_2_pi[0];
	qef_array[oc_id][4]=sigma_ni_2_pi[1];
	qef_array[oc_id][5]=sigma_ni_2_pi[2];

	qef_array[oc_id][6]=sigma_ni_2_pi_2[0];
	qef_array[oc_id][7]=sigma_ni_2_pi_2[1];
	qef_array[oc_id][8]=sigma_ni_2_pi_2[2];

	qef_array[oc_id][9]=solution[0];
	qef_array[oc_id][10]=solution[1];
	qef_array[oc_id][11]=solution[2];

	qef_array[oc_id][12]=qef;
}

void Octree::put_qef_in(int oc_id, double* sigma_ni_2, double* sigma_ni_2_pi,double* sigma_ni_2_pi_2,double* solution,double qef)
{
	//assert(qef_array_in[oc_id]==NULL);

	if (qef_array_in[oc_id]==NULL) {
		qef_array_in[oc_id]=(double*)malloc(sizeof(double)*(3+3+3+3+1));
	}

	qef_array_in[oc_id][0]=sigma_ni_2[0];
	qef_array_in[oc_id][1]=sigma_ni_2[1];
	qef_array_in[oc_id][2]=sigma_ni_2[2];

	qef_array_in[oc_id][3]=sigma_ni_2_pi[0];
	qef_array_in[oc_id][4]=sigma_ni_2_pi[1];
	qef_array_in[oc_id][5]=sigma_ni_2_pi[2];

	qef_array_in[oc_id][6]=sigma_ni_2_pi_2[0];
	qef_array_in[oc_id][7]=sigma_ni_2_pi_2[1];
	qef_array_in[oc_id][8]=sigma_ni_2_pi_2[2];

	qef_array_in[oc_id][9]=solution[0];
	qef_array_in[oc_id][10]=solution[1];
	qef_array_in[oc_id][11]=solution[2];

	qef_array_in[oc_id][12]=qef;
}

float Octree::get_err(int oc_id)
{
	//assert(qef_array[oc_id]!=NULL) ;

	if(is_skipcell(oc_id) == 0) {
		if(qef_array[oc_id] != NULL)
			return (float)qef_array[oc_id][12];
		else
			return -1.0;
	}
	else {
		if(qef_array_in[oc_id] != NULL)
			return (float)qef_array_in[oc_id][12];
		else
			return -1.0;
	}
}
/*
float Octree::get_err_grad(int oc_id)
{
	int level, cell_size, xx, yy, zz, ii, jj, kk;
	float val[8], val_new[13], x, y, z;
	float func_0, func_1, f_x, f_y, f_z, sum, sum_grad, temp;

	level = get_level(oc_id) ;
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_id, xx, yy, zz, level);
	getCellValues(oc_id, level, val);

	bool my_bool = (val[0] < iso_val && val[1] < iso_val && val[2] < iso_val && val[3] < iso_val
		&& val[4] < iso_val && val[5] < iso_val && val[6] < iso_val && val[7] < iso_val)
		|| (val[0] > iso_val && val[1] > iso_val && val[2] > iso_val && val[3] > iso_val
		&& val[4] > iso_val && val[5] > iso_val && val[6] > iso_val && val[7] > iso_val);
	if(flag_type > 3) {
		bool my_bool_in = minmax[oc_id].min > iso_val || minmax[oc_id].max < iso_val_in ||
			(minmax[oc_id].min > iso_val_in && minmax[oc_id].max < iso_val);
		my_bool = my_bool_in;
	}

	val_new[0] = getValue(xx*cell_size+cell_size/2, yy*cell_size, zz*cell_size);
	val_new[1] = getValue(xx*cell_size+cell_size,	yy*cell_size, zz*cell_size+cell_size/2);
	val_new[2] = getValue(xx*cell_size+cell_size/2, yy*cell_size, zz*cell_size+cell_size);
	val_new[3] = getValue(xx*cell_size,				yy*cell_size, zz*cell_size+cell_size/2);
	val_new[4] = getValue(xx*cell_size+cell_size/2, yy*cell_size+cell_size, zz*cell_size);
	val_new[5] = getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size, zz*cell_size+cell_size/2);
	val_new[6] = getValue(xx*cell_size+cell_size/2, yy*cell_size+cell_size, zz*cell_size+cell_size);
	val_new[7] = getValue(xx*cell_size,				yy*cell_size+cell_size, zz*cell_size+cell_size/2);
	val_new[8] = getValue(xx*cell_size,				yy*cell_size+cell_size/2, zz*cell_size);
	val_new[9] = getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2, zz*cell_size);
	val_new[10]= getValue(xx*cell_size,				yy*cell_size+cell_size/2, zz*cell_size+cell_size);
	val_new[11]= getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2, zz*cell_size+cell_size);
	val_new[12]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2, zz*cell_size+cell_size/2);

	sum = 0.0;
	sum_grad = 0.0;
	for(ii = 0; ii < 13; ii++) {
		if(ii == 3 || ii == 7 || ii == 8 || ii == 10)		x = 0.0;
		else if(ii == 1 || ii == 5 || ii == 9 || ii == 11)	x = 1.0;
		else	x = 0.5;

		if(ii < 4)		y = 0.0;
		else if(ii < 8)	y = 1.0;
		else	y = 0.5;

		if(ii == 0 || ii == 4 || ii == 8 || ii == 9)		z = 0.0;
		else if(ii == 2 || ii == 6 || ii == 10 || ii == 11)	z = 1.0;
		else	z = 0.5;

		func_0 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
					+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
					+ x*y*(1-z)*val[5] + x*y*z*val[6];
		func_1 = val_new[ii];

		if(func_1 > func_0)	temp = func_1 - func_0;
		else	temp = func_0 - func_1;

		sum += temp;

		f_x = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
					+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
		f_y = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
					+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
		f_z = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
					+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);

		sum_grad += sqrt(f_x*f_x + f_y*f_y + f_z*f_z);
	}

	for(ii = 0; ii < 2; ii++) {
		for(jj = 0; jj < 2; jj++) {
			for(kk = 0; kk < 2; kk ++) {
				x = ii; y = jj; z = kk;

				f_x = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
							+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
				f_y = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
							+x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
				f_z = (1-x)*(1-y)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
							+x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);

				sum_grad += sqrt(f_x*f_x + f_y*f_y + f_z*f_z);
			}
		}
	}

	if(!my_bool) {
		return (sum/sum_grad);
	}
	else
		return (-1.0);

}

*/
float Octree::get_err_grad_test(int oc_id)
{
	int level, cell_size, xx, yy, zz;
	float val[8], val_new[19], x, y, z;
	float func_0, func_1, f_x, f_y, f_z, sum_grad, temp;

	level = get_level(oc_id) ;
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_id, xx, yy, zz, level);
	getCellValues(oc_id, level, val);

	if(level == oct_depth) 	return (-1.0);

	bool my_bool = (val[0] < iso_val && val[1] < iso_val && val[2] < iso_val && val[3] < iso_val
		&& val[4] < iso_val && val[5] < iso_val && val[6] < iso_val && val[7] < iso_val)
		|| (val[0] > iso_val && val[1] > iso_val && val[2] > iso_val && val[3] > iso_val
		&& val[4] > iso_val && val[5] > iso_val && val[6] > iso_val && val[7] > iso_val);
	if(flag_type > 3) {
		bool my_bool_in = minmax[oc_id].min > iso_val || minmax[oc_id].max < iso_val_in ||
			(minmax[oc_id].min > iso_val_in && minmax[oc_id].max < iso_val);
		my_bool = my_bool_in;
	}

	if(is_skipcell(oc_id) == 0) {
		if(qef_array[oc_id] != NULL) {
			x = qef_array[oc_id][9] / cell_size - xx;
			y = qef_array[oc_id][10] / cell_size - yy;
			z = qef_array[oc_id][11] / cell_size - zz;
		}
	}

	val_new[0] = getValue(xx*cell_size+cell_size/2, yy*cell_size,				zz*cell_size);
	val_new[1] = getValue(xx*cell_size+cell_size,	yy*cell_size,				zz*cell_size+cell_size/2);
	val_new[2] = getValue(xx*cell_size+cell_size/2, yy*cell_size,				zz*cell_size+cell_size);
	val_new[3] = getValue(xx*cell_size,				yy*cell_size,				zz*cell_size+cell_size/2);
	val_new[4] = getValue(xx*cell_size+cell_size/2, yy*cell_size+cell_size,		zz*cell_size);
	val_new[5] = getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size,		zz*cell_size+cell_size/2);
	val_new[6] = getValue(xx*cell_size+cell_size/2, yy*cell_size+cell_size,		zz*cell_size+cell_size);
	val_new[7] = getValue(xx*cell_size,				yy*cell_size+cell_size,		zz*cell_size+cell_size/2);
	val_new[8] = getValue(xx*cell_size,				yy*cell_size+cell_size/2,	zz*cell_size);
	val_new[9] = getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2,	zz*cell_size);
	val_new[10]= getValue(xx*cell_size,				yy*cell_size+cell_size/2,	zz*cell_size+cell_size);
	val_new[11]= getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size);
	val_new[12]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size/2);
	val_new[13]= getValue(xx*cell_size,				yy*cell_size+cell_size/2,	zz*cell_size+cell_size/2);
	val_new[14]= getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size/2);
	val_new[15]= getValue(xx*cell_size+cell_size/2,	yy*cell_size,				zz*cell_size+cell_size/2);
	val_new[16]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size,		zz*cell_size+cell_size/2);
	val_new[17]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2,	zz*cell_size);
	val_new[18]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size);

	sum_grad = 0.0;

	func_0 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
				+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
				+ x*y*(1-z)*val[5] + x*y*z*val[6];
	//func_1 = val_new[ii];

	int new_oc_id;
	if(x < 0.5 && y < 0.5 && z < 0.5) {
		new_oc_id = xyz2octcell(xx*2, yy*2, zz*2, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2;	y = y*2;	z = z*2;
	}
	else if(x > 0.5 && y < 0.5 && z < 0.5) {
		new_oc_id = xyz2octcell(xx*2+1, yy*2, zz*2, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2-1;	y = y*2;	z = z*2;
	}
	else if(x < 0.5 && y > 0.5 && z < 0.5) {
		new_oc_id = xyz2octcell(xx*2, yy*2+1, zz*2, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2;	y = y*2-1;	z = z*2;
	}
	else if(x > 0.5 && y > 0.5 && z < 0.5) {
		new_oc_id = xyz2octcell(xx*2+1, yy*2+1, zz*2, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2-1;	y = y*2-1;	z = z*2;
	}
	else if(x < 0.5 && y < 0.5 && z > 0.5) {
		new_oc_id = xyz2octcell(xx*2, yy*2, zz*2+1, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2;	y = y*2;	z = z*2-1;
	}
	else if(x > 0.5 && y < 0.5 && z > 0.5) {
		new_oc_id = xyz2octcell(xx*2+1, yy*2, zz*2+1, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2-1;	y = y*2;	z = z*2-1;
	}
	else if(x < 0.5 && y > 0.5 && z > 0.5) {
		new_oc_id = xyz2octcell(xx*2, yy*2+1, zz*2+1, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2;	y = y*2-1;	z = z*2-1;
	}
	else  {	//if(x > 0.5 && y > 0.5 && z > 0.5)
		new_oc_id = xyz2octcell(xx*2+1, yy*2+1, zz*2+1, level+1);
		getCellValues(new_oc_id, level+1, val);
		x = x*2-1;	y = y*2-1;	z = z*2-1;
	}

	func_1 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
				+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
				+ x*y*(1-z)*val[5] + x*y*z*val[6];

	//temp = fabs(func_0);
	if(func_1 > func_0)	temp = func_1 - func_0;
	else	temp = func_0 - func_1;

	f_x = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
				+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
	f_y = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
				+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
	f_z = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
				+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);

	sum_grad = temp / sqrt(f_x*f_x + f_y*f_y + f_z*f_z);

	if(!my_bool) {
		return (sum_grad);
	}
	else
		return (-1.0);

}

// the error between two neighboring level
float Octree::get_err_grad(int oc_id)
{
	int level, cell_size, xx, yy, zz, ii;
	float val[8], val_new[19], x, y, z;
	float func_0, func_1, f_x, f_y, f_z, sum_grad, temp;

	level = get_level(oc_id) ;
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_id, xx, yy, zz, level);
	getCellValues(oc_id, level, val);

	bool my_bool = (val[0] < iso_val && val[1] < iso_val && val[2] < iso_val && val[3] < iso_val
		&& val[4] < iso_val && val[5] < iso_val && val[6] < iso_val && val[7] < iso_val)
		|| (val[0] > iso_val && val[1] > iso_val && val[2] > iso_val && val[3] > iso_val
		&& val[4] > iso_val && val[5] > iso_val && val[6] > iso_val && val[7] > iso_val);
	if(flag_type > 3) {
		bool my_bool_in = minmax[oc_id].min > iso_val || minmax[oc_id].max < iso_val_in ||
			(minmax[oc_id].min > iso_val_in && minmax[oc_id].max < iso_val);
		my_bool = my_bool_in;
	}

	val_new[0] = getValue(xx*cell_size+cell_size/2, yy*cell_size,				zz*cell_size);
	val_new[1] = getValue(xx*cell_size+cell_size,	yy*cell_size,				zz*cell_size+cell_size/2);
	val_new[2] = getValue(xx*cell_size+cell_size/2, yy*cell_size,				zz*cell_size+cell_size);
	val_new[3] = getValue(xx*cell_size,				yy*cell_size,				zz*cell_size+cell_size/2);
	val_new[4] = getValue(xx*cell_size+cell_size/2, yy*cell_size+cell_size,		zz*cell_size);
	val_new[5] = getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size,		zz*cell_size+cell_size/2);
	val_new[6] = getValue(xx*cell_size+cell_size/2, yy*cell_size+cell_size,		zz*cell_size+cell_size);
	val_new[7] = getValue(xx*cell_size,				yy*cell_size+cell_size,		zz*cell_size+cell_size/2);
	val_new[8] = getValue(xx*cell_size,				yy*cell_size+cell_size/2,	zz*cell_size);
	val_new[9] = getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2,	zz*cell_size);
	val_new[10]= getValue(xx*cell_size,				yy*cell_size+cell_size/2,	zz*cell_size+cell_size);
	val_new[11]= getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size);
	val_new[12]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size/2);
	val_new[13]= getValue(xx*cell_size,				yy*cell_size+cell_size/2,	zz*cell_size+cell_size/2);
	val_new[14]= getValue(xx*cell_size+cell_size,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size/2);
	val_new[15]= getValue(xx*cell_size+cell_size/2,	yy*cell_size,				zz*cell_size+cell_size/2);
	val_new[16]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size,		zz*cell_size+cell_size/2);
	val_new[17]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2,	zz*cell_size);
	val_new[18]= getValue(xx*cell_size+cell_size/2,	yy*cell_size+cell_size/2,	zz*cell_size+cell_size);

	sum_grad = 0.0;
	for(ii = 0; ii < 19; ii++) {
		if(ii == 3 || ii == 7 || ii == 8 || ii == 10 || ii == 13)		x = 0.0;
		else if(ii == 1 || ii == 5 || ii == 9 || ii == 11 || ii == 14)	x = 1.0;
		else	x = 0.5;

		if(ii < 4 || ii == 15)		y = 0.0;
		else if(ii < 8 || ii == 16)	y = 1.0;
		else	y = 0.5;

		if(ii == 0 || ii == 4 || ii == 8 || ii == 9 || ii == 17)		z = 0.0;
		else if(ii == 2 || ii == 6 || ii == 10 || ii == 11 || ii == 18)	z = 1.0;
		else	z = 0.5;

		func_0 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
					+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
					+ x*y*(1-z)*val[5] + x*y*z*val[6];
		func_1 = val_new[ii];

		if(func_1 > func_0)	temp = func_1 - func_0;
		else	temp = func_0 - func_1;

		f_x = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
					+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
		f_y = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
					+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
		f_z = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
					+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);

		sum_grad += temp / sqrt(f_x*f_x + f_y*f_y + f_z*f_z);
	}

	if(!my_bool) {
		return (sum_grad);
	}
	else
		return (-1.0);
/*
	// head65
	if(!my_bool && zz*cell_size > 16.0f) {
		return (sum_grad);
	}
	else
		return (-1.0);
	
	// mache_65 location
	//if( xx*cell_size >= 21 && xx*cell_size <= 43 && yy*cell_size >= 37 && 
	//	yy*cell_size <= 61 && zz*cell_size >= 23 && zz*cell_size <= 30 && my_bool == 0) return sum_grad;

	// mache_129 location
	//if( xx*cell_size >= 42 && xx*cell_size <= 86 && yy*cell_size >= 74 && 
	//	yy*cell_size <= 122 && zz*cell_size >= 46 && zz*cell_size <= 60 && my_bool == 0) return sum_grad;

	// mache_65129
	if( xx*cell_size >= 53 && xx*cell_size <= 75 && yy*cell_size >= 69 && 
		yy*cell_size <= 91 && zz*cell_size >= 52 && zz*cell_size <= 67 && my_bool == 0) return sum_grad;

	else return (-1.0f);
*/	
}
/*
// the error between the current level and the finest level
float Octree::get_err_grad(int oc_id)
{
	int level, cell_size, xx, yy, zz, i, j, k;
	float val[8], x, y, z;
	float func_0, func_1, f_x, f_y, f_z, sum_grad, temp;

	level = get_level(oc_id) ;
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_id, xx, yy, zz, level);
	getCellValues(oc_id, level, val);

	bool my_bool = (minmax[oc_id].min > iso_val || minmax[oc_id].max < iso_val);
	if(flag_type > 3) {
		bool my_bool_in = minmax[oc_id].min > iso_val || minmax[oc_id].max < iso_val_in ||
			(minmax[oc_id].min > iso_val_in && minmax[oc_id].max < iso_val);
		my_bool = my_bool_in;
	}

	sum_grad = 0.0;
	for(i = 0; i < cell_size+1; i++) {
		for(j = 0; j < cell_size+1; j++) {
			for(k = 0; k < cell_size+1; k++) {
				x = (float)i/cell_size;
				y = (float)j/cell_size;
				z = (float)k/cell_size;

				func_0 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
							+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
							+ x*y*(1-z)*val[5] + x*y*z*val[6];
				func_1 = getValue(((float)xx+x)*cell_size, ((float)yy+y)*cell_size, ((float)zz+z)*cell_size);

				if(func_1 > func_0)	temp = func_1 - func_0;
				else	temp = func_0 - func_1;

				f_x = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
							+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
				f_y = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
							+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
				f_z = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
							+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);

				sum_grad += temp / sqrt(f_x*f_x + f_y*f_y + f_z*f_z);
			}
		}
	}
	if(!my_bool) {
		return (sum_grad/10.0f);
	}
	else
		return (-1.0);

}
*/
void Octree::traverse_qef(float err_tol)
{
	CellQueue prev_queue,cur_queue;
	int i,oc_id,level, start_level, end_level;
	int x, y, z, tx, ty, tz, cell_size;
	float r, r_min, temp, center;

	oc_id=0;
	leaf_num=0;
	center = (dim[0]-1.0f)/2.0f;

	start_level = oct_depth - 3;	// - 3
	end_level = oct_depth;
	if(flag_type == 2) {
		start_level = oct_depth - 3;//3
		end_level = start_level + 1;
	}
	if(flag_type == 3) {
		start_level = oct_depth - 3;//3
		end_level = start_level + 1;
	}

	memset(oct_array,0,sizeof(octcell)*octcell_num);
	prev_queue.Add(oc_id);
	while (!prev_queue.Empty()) {
		
		//leaf_idx=leaf_num;
		while (prev_queue.Get(oc_id) >= 0)  {
			level=get_level(oc_id);
			
			octcell2xyz(oc_id,x,y,z,level);
			cell_size = (dim[0]-1)/(1<<level);
			tx = x*cell_size;		ty = y*cell_size;		tz = z*cell_size;
			r = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			r_min = r;
			tx = (x+1)*cell_size;	ty = y*cell_size;		tz = z*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;

			tx = x*cell_size;		ty = (y+1)*cell_size;	tz = z*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;
			tx = (x+1)*cell_size;	ty = (y+1)*cell_size;	tz = z*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;
			
			tx = x*cell_size;		ty = y*cell_size;		tz = (z+1)*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;
			tx = (x+1)*cell_size;	ty = y*cell_size;		tz = (z+1)*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;

			tx = x*cell_size;		ty = (y+1)*cell_size;	tz = (z+1)*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;
			tx = (x+1)*cell_size;	ty = (y+1)*cell_size;	tz = (z+1)*cell_size;
			temp = (float) sqrt((tx-center)*(tx-center) + (ty-center)*(ty-center) + (tz-center)*(tz-center));
			if(temp > r) r = temp;
			if(temp < r_min) r_min = temp;

			//if (is_skipcell(oc_id)) {
			if(minmax[oc_id].min > iso_val) {
				continue;
			//} else if (level <= 4 ||(get_err_grad(oc_id) > err_tol && level != oct_depth)) { // heart_0
			} else if (level <= start_level || (get_err_grad(oc_id) > err_tol && level < end_level)) {    // head

			//} else if (level <= 3) {  //4 

			//} else if (level <= oct_depth-4 || (level <= oct_depth-3 && is_skipcell(oc_id) == 0)
			//	||(get_err_grad(oc_id) > err_tol && level != oct_depth)) {

			//} else if (level < oct_depth) {

			//} else if ((level <= 1 && r > (dim[0]-1)/4.0*1.733) || (level <= 2 && r < (dim[0]-1)/4.0*1.733)
			//	|| (level <= oct_depth-2 && is_skipcell(oc_id) == 0 && r < (dim[0]-1)/4.0*1.515)
			//	||(get_err_grad(oc_id) > err_tol && level < oct_depth && r < 16*1.8)) { // 14.3

				cur_queue.Add(oc_id);
				oct_array[oc_id].refine_flag=1;
			} else {
				cut_array[leaf_num++]=oc_id;
			}
		}
		
		while (cur_queue.Get(oc_id) >= 0) {
			level=get_level(oc_id);
			for (i=0; i<8; i++)
				prev_queue.Add(child(oc_id,level,i));
		}
		
	}
}

void Octree::traverse_qef_interval(float err_tol, float err_tol_in)
{
	CellQueue prev_queue,cur_queue;
	int i,oc_id,level;

	oc_id=0;
	leaf_num=0;

	memset(oct_array,0,sizeof(octcell)*octcell_num);
	prev_queue.Add(oc_id);
	while (!prev_queue.Empty()) {
		
		//leaf_idx=leaf_num;
		while (prev_queue.Get(oc_id) >= 0)  {
			level=get_level(oc_id);
			
			//octcell2xyz(oc_id,x,y,z,level);
			
			//if (is_skipcell(oc_id)) {
			if(minmax[oc_id].min > iso_val || minmax[oc_id].max < iso_val_in) {
				continue;
			//} else if (level <= 3 ||(get_err_grad(oc_id) > err_tol && level != oct_depth)) {
			} else if (level <= 3 ||(is_skipcell(oc_id) == 0  && get_err_grad(oc_id) > err_tol && level != oct_depth)
				||(minmax[oc_id].max > iso_val_in &&  minmax[oc_id].min < iso_val_in && get_err_grad(oc_id) > err_tol_in && level != oct_depth)) {
			/*
			} else if (level <= oct_depth-4 ||(is_skipcell_interval(oc_id) == 0  && get_err_grad(oc_id) > err_tol && level != oct_depth)
				|| (level <= oct_depth-3 && is_skipcell_interval(oc_id) == 0)
				|| (minmax[oc_id].max > iso_val_in &&  minmax[oc_id].min < iso_val_in && get_err_grad(oc_id) > err_tol_in && level != oct_depth)) {
			*/
			//} else if (level <= oct_depth-2) {
				cur_queue.Add(oc_id);
				oct_array[oc_id].refine_flag=1;
			} else {
				cut_array[leaf_num++]=oc_id;
			}
		}
		
		while (cur_queue.Get(oc_id) >= 0) {
			level=get_level(oc_id);
			for (i=0; i<8; i++)
				prev_queue.Add(child(oc_id,level,i));
		}
		
	}
}

int Octree::is_min_edge(int oc_id, int e_id, unsigned int* vtx, int& vtx_num, int intersect_id, geoframe& geofrm)
{
	int x,y,z,level,i;
	unsigned int temp_vtx[4];
	
	level=get_level(oc_id);
	octcell2xyz(oc_id,x,y,z,level);
	
	vtx_num=4;

	switch (e_id) {
		case 0 : 
			if (is_refined(x,y,z-1,level) || is_refined(x,y-1,z-1,level) || is_refined(x,y-1,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y,z-1,level,geofrm);
			temp_vtx[2]=min_vtx(x,y-1,z-1,level,geofrm);
			temp_vtx[3]=min_vtx(x,y-1,z,level,geofrm);
			break;

		case 1 : 
			if (is_refined(x,y-1,z,level) || is_refined(x+1,y-1,z,level) || is_refined(x+1,y,z,level)) return 0;
			temp_vtx[1]=min_vtx(x+1,y,z,level,geofrm);
			temp_vtx[2]=min_vtx(x+1,y-1,z,level,geofrm);
			temp_vtx[3]=min_vtx(x,y-1,z,level,geofrm);
			break;

		case 2 : 
			if (is_refined(x,y,z+1,level) || is_refined(x,y-1,z+1,level) || is_refined(x,y-1,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y,z+1,level,geofrm);
			temp_vtx[2]=min_vtx(x,y-1,z+1,level,geofrm);
			temp_vtx[3]=min_vtx(x,y-1,z,level,geofrm);
			break;

		case 3 : 
			if (is_refined(x,y-1,z,level) || is_refined(x-1,y-1,z,level) || is_refined(x-1,y,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y-1,z,level,geofrm);
			temp_vtx[2]=min_vtx(x-1,y-1,z,level,geofrm);
			temp_vtx[3]=min_vtx(x-1,y,z,level,geofrm);
			break;

		case 4 : 
			if (is_refined(x,y,z-1,level) || is_refined(x,y+1,z-1,level) || is_refined(x,y+1,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y+1,z,level,geofrm);
			temp_vtx[2]=min_vtx(x,y+1,z-1,level,geofrm);
			temp_vtx[3]=min_vtx(x,y,z-1,level,geofrm);
			break;

		case 5 :
			if (is_refined(x,y+1,z,level) || is_refined(x+1,y,z,level) || is_refined(x+1,y+1,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y+1,z,level,geofrm);
			temp_vtx[2]=min_vtx(x+1,y+1,z,level,geofrm);
			temp_vtx[3]=min_vtx(x+1,y,z,level,geofrm);
			break;

		case 6 :
			if (is_refined(x,y+1,z,level) || is_refined(x,y+1,z+1,level) || is_refined(x,y,z+1,level)) return 0;
			temp_vtx[1]=min_vtx(x,y+1,z,level,geofrm);
			temp_vtx[2]=min_vtx(x,y+1,z+1,level,geofrm);
			temp_vtx[3]=min_vtx(x,y,z+1,level,geofrm);
			break;

		case 7 :
			if (is_refined(x-1,y,z,level) || is_refined(x-1,y+1,z,level) || is_refined(x,y+1,z,level)) return 0;
			temp_vtx[1]=min_vtx(x-1,y,z,level,geofrm);
			temp_vtx[2]=min_vtx(x-1,y+1,z,level,geofrm);
			temp_vtx[3]=min_vtx(x,y+1,z,level,geofrm);
			break;

		case 8 :
			if (is_refined(x,y,z-1,level) || is_refined(x-1,y,z-1,level) || is_refined(x-1,y,z,level)) return 0;
			temp_vtx[1]=min_vtx(x-1,y,z,level,geofrm);
			temp_vtx[2]=min_vtx(x-1,y,z-1,level,geofrm);
			temp_vtx[3]=min_vtx(x,y,z-1,level,geofrm);
			break;

		case 9 :
			if (is_refined(x,y,z-1,level) || is_refined(x+1,y,z-1,level) || is_refined(x+1,y,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y,z-1,level,geofrm);
			temp_vtx[2]=min_vtx(x+1,y,z-1,level,geofrm);
			temp_vtx[3]=min_vtx(x+1,y,z,level,geofrm);
			break;

		case 10 :
			if (is_refined(x,y,z+1,level) || is_refined(x-1,y,z+1,level) || is_refined(x-1,y,z,level)) return 0;
			temp_vtx[1]=min_vtx(x,y,z+1,level,geofrm);
			temp_vtx[2]=min_vtx(x-1,y,z+1,level,geofrm);
			temp_vtx[3]=min_vtx(x-1,y,z,level,geofrm);
			break;

		case 11 :
			if (is_refined(x,y,z+1,level) || is_refined(x+1,y,z+1,level) || is_refined(x+1,y,z,level)) return 0;
			temp_vtx[1]=min_vtx(x+1,y,z,level,geofrm);
			temp_vtx[2]=min_vtx(x+1,y,z+1,level,geofrm);
			temp_vtx[3]=min_vtx(x,y,z+1,level,geofrm);
			break;
	}

	temp_vtx[0] = min_vtx(x,y,z,level,geofrm);

	assert(intersect_id==1 || intersect_id==-1 || intersect_id==3 || intersect_id==-3);

	if (intersect_id==1 || intersect_id==3) 
		for (i=0;i<4;i++) vtx[i]=temp_vtx[i];
	else if (intersect_id==-1 || intersect_id==-3)
		for (i=0;i<4;i++) vtx[i]=temp_vtx[3-i];

	return 1;

}

void Octree::get_solution(int oc_id, float* pos)
{
	int x,y,z;
	float val[8];
	int level = get_level(oc_id);
	
	int cell_size = (dim[0]-1)/(1<<level);
		
	getCellValues(oc_id, level, val);
	octcell2xyz(oc_id, x, y, z, level);

//	assert(minmax[oc_id].max >= iso_val);
	assert(minmax[oc_id].max >= iso_val || minmax[oc_id].min <= iso_val_in);

	//if(in_out == 0) {
	if(is_skipcell(oc_id) == 0) {
		pos[0]=qef_array[oc_id][9];
		pos[1]=qef_array[oc_id][10];
		pos[2]=qef_array[oc_id][11];
	}
	else {
		pos[0]=qef_array_in[oc_id][9];
		pos[1]=qef_array_in[oc_id][10];
		pos[2]=qef_array_in[oc_id][11];
	}

	// to be corrected
	if (!(pos[0] > x*cell_size && pos[0] < (x+1)*cell_size)) {
		pos[0] = x*cell_size + cell_size/2.;
	}
		
	if (!(pos[1] > y*cell_size && pos[1] < (y+1)*cell_size)) {
		pos[1] = y*cell_size + cell_size/2.;
	}
		
	if (!(pos[2] > z*cell_size && pos[2] < (z+1)*cell_size)) {
		pos[2] = z*cell_size + cell_size/2.;
	}
}

void Octree::get_vtx(int x, int y, int z, int level, float* pos)
{
	int oc_id;
	oc_id = xyz2octcell(x,y,z,level);
	get_solution(oc_id,pos);
}

void Octree::get_VtxNorm(float* vtx, float* norm) 
{
	// finest resolution
	int oc_id, x, y, z;
	float val[8], tx, ty, tz;

	x = (int) vtx[0];	tx = vtx[0] - (float) x;
	y = (int) vtx[1];	ty = vtx[1] - (float) y;
	z = (int) vtx[2];	tz = vtx[2] - (float) z;

	oc_id = xyz2octcell(x, y, z, oct_depth);
	getCellValues(oc_id, oct_depth, val);

	norm[0] = (1-ty)*(1-tz)*(val[1] - val[0]) + (1-ty)*tz*(val[2] - val[3])
				+ ty*(1-tz)*(val[5] - val[4]) + ty*tz*(val[6] - val[7]);
	norm[1] = (1-tx)*(1-tz)*(val[4] - val[0]) + (1-tx)*tz*(val[7] - val[3])
				+ tx*(1-tz)*(val[5] - val[1]) + tx*tz*(val[6] - val[2]);
	norm[2] = (1-tx)*(1-tz)*(val[3] - val[0]) + (1-tx)*ty*(val[7] - val[4])
				+ tx*(1-ty)*(val[2] - val[1]) + tx*ty*(val[6] - val[5]);
}

int Octree::min_vtx(int x, int y, int z, int level, geoframe& geofrm)
{
	int tx, ty, tz, vert;
	float vtx[3], norm[3];
	tx = x;	ty = y;	tz = z;

	assert( tx>=0 && ty>=0 && tz>=0 );
	assert( !is_refined(tx,ty,tz,level) );

	while ( level==0 || !is_refined(tx/2 , ty/2 , tz/2 , level-1 )) {
		tx/=2;
		ty/=2;
		tz/=2;
		level--;
	}

	int oc_id = xyz2octcell(tx, ty, tz, level);
	int cell_size = (dim[0]-1)/(1<<level);

//	if(minmax[oc_id].max <= iso_val)
	if(minmax[oc_id].max <= iso_val && minmax[oc_id].min >= iso_val_in)
		return -1;
	else {
		get_vtx(tx, ty, tz, level, vtx);
		//getVertGrad(tx*cell_size, ty*cell_size, tz*cell_size, norm);
		get_VtxNorm(vtx, norm);
		if(in_out == 0) {
			if ((vert = vtx_idx_arr[xyz2octcell(tx, ty, tz, level)]) == -1) {
				vert = geofrm.AddVert(vtx, norm);
				geofrm.AddBound(vert, 1);
				vtx_idx_arr[xyz2octcell(tx, ty, tz, level)] = vert;
				return vert;
			}
			else
				return vert;
		}
		else {
			if ((vert = vtx_idx_arr_in[xyz2octcell(tx, ty, tz, level)]) == -1) {
				vert = geofrm.AddVert(vtx, norm);
				geofrm.AddBound(vert, -1);
				vtx_idx_arr_in[xyz2octcell(tx, ty, tz, level)] = vert;
				return vert;
			}
			else
				return vert;
		}
	}
}

void Octree::eflag_clear()
{
	memset(ebit,0,octcell_num*4/8);
}

int Octree::is_eflag_on(int x, int y, int z, int level, int e)
{
	int idx;
	switch (e) {
	case 0 : 
		idx = 3*xyz2octcell(x,y,z,level) + 0;
		break;
	case 1 :
		idx = 3*xyz2octcell(x+1,y,z,level) + 2;
		break;
	case 2 :
		idx = 3*xyz2octcell(x,y,z+1,level) + 0;
		break;
	case 3 :
		idx = 3*xyz2octcell(x,y,z,level) + 2;
		break;
	case 4 :
		idx = 3*xyz2octcell(x,y+1,z,level) + 0;
		break;
	case 5 :
		idx = 3*xyz2octcell(x+1,y+1,z,level) + 2;
		break;
	case 6 :
		idx = 3*xyz2octcell(x,y+1,z+1,level) + 0;
		break;
	case 7 :
		idx = 3*xyz2octcell(x,y+1,z,level) + 2;
		break;
	case 8 :
		idx = 3*xyz2octcell(x,y,z,level) + 1;
		break;
	case 9 :
		idx = 3*xyz2octcell(x+1,y,z,level) + 1;
		break;
	case 10 :
		idx = 3*xyz2octcell(x,y,z+1,level) + 1;
		break;
	case 11 :
		idx = 3*xyz2octcell(x+1,y,z+1,level) + 1;
		break;
	}
	
	if (ebit[idx/8]&(1<<(idx%8))) return 1;
	else return 0;
	
}

void Octree::eflag_on(int x, int y, int z, int level, int e)
{
	int idx;
	switch (e) {
	case 0 : 
		idx = 3*xyz2octcell(x,y,z,level) + 0;
		break;
	case 1 :
		idx = 3*xyz2octcell(x+1,y,z,level) + 2;
		break;
	case 2 :
		idx = 3*xyz2octcell(x,y,z+1,level) + 0;
		break;
	case 3 :
		idx = 3*xyz2octcell(x,y,z,level) + 2;
		break;
	case 4 :
		idx = 3*xyz2octcell(x,y+1,z,level) + 0;
		break;
	case 5 :
		idx = 3*xyz2octcell(x+1,y+1,z,level) + 2;
		break;
	case 6 :
		idx = 3*xyz2octcell(x,y+1,z+1,level) + 0;
		break;
	case 7 :
		idx = 3*xyz2octcell(x,y+1,z,level) + 2;
		break;
	case 8 :
		idx = 3*xyz2octcell(x,y,z,level) + 1;
		break;
	case 9 :
		idx = 3*xyz2octcell(x+1,y,z,level) + 1;
		break;
	case 10 :
		idx = 3*xyz2octcell(x,y,z+1,level) + 1;
		break;
	case 11 :
		idx = 3*xyz2octcell(x+1,y,z+1,level) + 1;
		break;
	}
	
	ebit[idx/8]|=(1<<(idx%8));
	
}

void Octree::find_oc_id(int x, int y, int z, int level, int j, int intersect_id, int* oc_id) {

	int i, temp_id[4];

	oc_id[0] = xyz2octcell(x, y, z, level);

	switch (j) {
		case 0 : 
			oc_id[1] = xyz2octcell(x,	y,		z-1,	level);
			oc_id[2] = xyz2octcell(x,	y-1,	z-1,	level);
			oc_id[3] = xyz2octcell(x,	y-1,	z,		level);
			break;

		case 1 : 
			oc_id[1] = xyz2octcell(x+1,	y,		z,		level);
			oc_id[2] = xyz2octcell(x+1,	y-1,	z,		level);
			oc_id[3] = xyz2octcell(x,	y-1,	z,		level);
			break;

		case 2 : 
			oc_id[1] = xyz2octcell(x,	y,		z+1,	level);
			oc_id[2] = xyz2octcell(x,	y-1,	z+1,	level);
			oc_id[3] = xyz2octcell(x,	y-1,	z,		level);
			break;

		case 3 : 
			oc_id[1] = xyz2octcell(x,	y-1,	z,		level);
			oc_id[2] = xyz2octcell(x-1,	y-1,	z,		level);
			oc_id[3] = xyz2octcell(x-1,	y,		z,		level);
			break;

		case 4 : 
			oc_id[1] = xyz2octcell(x,	y+1,	z,		level);
			oc_id[2] = xyz2octcell(x,	y+1,	z-1,	level);
			oc_id[3] = xyz2octcell(x,	y,		z-1,	level);
			break;

		case 5 :
			oc_id[1] = xyz2octcell(x,	y+1,	z,		level);
			oc_id[2] = xyz2octcell(x+1,	y+1,	z,		level);
			oc_id[3] = xyz2octcell(x+1,	y,		z,		level);
			break;

		case 6 :
			oc_id[1] = xyz2octcell(x,	y+1,	z,		level);
			oc_id[2] = xyz2octcell(x,	y+1,	z+1,	level);
			oc_id[3] = xyz2octcell(x,	y,		z+1,	level);
			break;

		case 7 :
			oc_id[1] = xyz2octcell(x-1,	y,		z,		level);
			oc_id[2] = xyz2octcell(x-1,	y+1,	z,		level);
			oc_id[3] = xyz2octcell(x,	y+1,	z,		level);
			break;

		case 8 :
			oc_id[1] = xyz2octcell(x-1,	y,		z,		level);
			oc_id[2] = xyz2octcell(x-1,	y,		z-1,	level);
			oc_id[3] = xyz2octcell(x,	y,		z-1,	level);
			break;

		case 9 :
			oc_id[1] = xyz2octcell(x,	y,		z-1,	level);
			oc_id[2] = xyz2octcell(x+1,	y,		z-1,	level);
			oc_id[3] = xyz2octcell(x+1,	y,		z,		level);
			break;

		case 10 :
			oc_id[1] = xyz2octcell(x,	y,		z+1,	level);
			oc_id[2] = xyz2octcell(x-1,	y,		z+1,	level);
			oc_id[3] = xyz2octcell(x-1,	y,		z,		level);
			break;

		case 11 :
			oc_id[1] = xyz2octcell(x+1,	y,		z,		level);
			oc_id[2] = xyz2octcell(x+1,	y,		z+1,	level);
			oc_id[3] = xyz2octcell(x,	y,		z+1,	level);
			break;
		}
					
		for(i = 0; i < 4; i++)	temp_id[i] = oc_id[i];
		if (intersect_id == -1) 
			for (i = 0; i < 4; i++) oc_id[i] = temp_id[3-i];

}

void Octree::find_vtx_new(geoframe& geofrm, int x, int y, int z, int level, int j, int intersect_id, unsigned int* vtx_new) {

	int i, oc_id_new[4], cell_size, vert, tx, ty, tz;
	float vtx[3], norm[3], val[8];

	switch (j) {
		case 0 : 
			oc_id_new[0] = xyz2octcell(2*x, 2*y, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y, 2*z, level+1, j, intersect_id, oc_id_new);
			break;

		case 1 : 
			oc_id_new[0] = xyz2octcell(2*x+1, 2*y, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x+1, 2*y, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 2 : 
			oc_id_new[0] = xyz2octcell(2*x, 2*y, 2*z+1, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y, 2*z+1, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 3 : 
			oc_id_new[0] = xyz2octcell(2*x, 2*y, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x, 2*y, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 4 : 
			oc_id_new[0] = xyz2octcell(2*x, 2*y+1, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y+1, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y+1, 2*z, level+1, j, intersect_id, oc_id_new);
			break;

		case 5 :
			oc_id_new[0] = xyz2octcell(2*x+1, 2*y+1, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x+1, 2*y+1, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y+1, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 6 :
			oc_id_new[0] = xyz2octcell(2*x, 2*y+1, 2*z+1, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y+1, 2*z+1, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y+1, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 7 :
			oc_id_new[0] = xyz2octcell(2*x, 2*y+1, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y+1, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x, 2*y+1, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 8 :
			oc_id_new[0] = xyz2octcell(2*x, 2*y, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x, 2*y+1, 2*z, level+1, j, intersect_id, oc_id_new);
			break;

		case 9 :
			oc_id_new[0] = xyz2octcell(2*x+1, 2*y, 2*z, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x+1, 2*y, 2*z, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y+1, 2*z, level+1, j, intersect_id, oc_id_new);
			break;

		case 10 :
			oc_id_new[0] = xyz2octcell(2*x, 2*y, 2*z+1, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x, 2*y, 2*z+1, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x, 2*y+1, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;

		case 11 :
			oc_id_new[0] = xyz2octcell(2*x+1, 2*y, 2*z+1, level+1);
			getCellValues(oc_id_new[0], level+1, val);
			if(is_intersect(val, j) == 1 || is_intersect(val, j) == -1)
				find_oc_id(2*x+1, 2*y, 2*z+1, level+1, j, intersect_id, oc_id_new);
			else
				find_oc_id(2*x+1, 2*y+1, 2*z+1, level+1, j, intersect_id, oc_id_new);
			break;
		}
					
		for(i = 0; i < 4; i++) {
			octcell2xyz(oc_id_new[i], tx, ty, tz, level+1);

			cell_size = (dim[0]-1)/(1<<(level+1));
			//get_vtx(tx, ty, tz, level+1, vtx);
			get_solution(oc_id_new[i], vtx);
			getVertGrad(tx*cell_size, ty*cell_size, tz*cell_size, norm);
			//get_VtxNorm(vtx, norm);
			if(in_out == 0) {
				if ((vert = vtx_idx_arr[xyz2octcell(tx, ty, tz, level+1)]) == -1) {
					vert = geofrm.AddVert(vtx, norm);
					geofrm.AddBound(vert, 1);
					vtx_idx_arr[xyz2octcell(tx, ty, tz, level+1)] = vert;
				}
			}
			else {
				if ((vert = vtx_idx_arr_in[xyz2octcell(tx, ty, tz, level+1)]) == -1) {
					vert = geofrm.AddVert(vtx, norm);
					geofrm.AddBound(vert, -1);
					vtx_idx_arr_in[xyz2octcell(tx, ty, tz, level+1)] = vert;
				}
			}
			vtx_new[i] = (unsigned int) vert;

		}

}

void Octree::get_vtx_new(geoframe& geofrm, int oc_id, unsigned int vtx)
{
/*
	// average all the minimizer points inside this cell
	int xx, yy, zz, tx, ty, tz, level, i, j, my_id, num;
	float pos[3], pos_average[3];

	level = get_level(oc_id) ;
	octcell2xyz(oc_id, xx, yy, zz, level);

	if(i == 0 || i == 3 || i == 4 || i == 7) tx = 2*xx;
	else tx = 2*xx + 1;
	if(i < 4) ty = 2*yy;
	else ty = 2*yy + 1;
	if(i == 0 || i == 1 || i == 4 || i == 5) tz = 2*zz;
	else tz = 2*zz + 1;

	num = 0;
	for(i = 0; i < 3; i++) pos_average[i] = 0.0f;

	for(i = 0; i < 8; i++) {
		my_id = xyz2octcell(tx, ty, tz, level+1);
		if(is_skipcell(my_id) == 0) {
			num++;
			get_solution(my_id, pos);
			for(j = 0; j < 3; j++) pos_average[j] += pos[j];
		}
	}
	if(num > 1) {
		for(j = 0; j < 3; j++) {
			geofrm.verts[vtx][j] = (geofrm.verts[vtx][j] + pos_average[j]/num)/2.0f;
		}
	}

	// find the intersection point with the trilinear function (the current resolution)
	int level, cell_size, xx, yy, zz, i, my_bool;
	float val[8], x, y, z, step, func, func0;

	level = get_level(oc_id) ;
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_id, xx, yy, zz, level);
	getCellValues(oc_id, level, val);

	x = geofrm.verts[vtx][0]/cell_size - (float)xx;
	y = geofrm.verts[vtx][1]/cell_size - (float)yy;
	z = geofrm.verts[vtx][2]/cell_size - (float)zz;

	func0 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
				+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
				+ x*y*(1-z)*val[5] + x*y*z*val[6];

	step = 0.001f;
	if(func0 < 0.0f) step = -0.001f;
	for(i = 1; i < 16000; i++) {
		x = x - geofrm.normals[vtx][0]*step;
		y = y - geofrm.normals[vtx][1]*step;
		z = z - geofrm.normals[vtx][2]*step;
		func = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
					+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
					+ x*y*(1-z)*val[5] + x*y*z*val[6];
		my_bool = (x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f);
		if(func*func0 <= 0.0f || my_bool == 0) break;
		func0 = func;
	}

	geofrm.verts[vtx][0] = ((float)xx + x)*(float)cell_size;
	geofrm.verts[vtx][1] = ((float)yy + y)*(float)cell_size;
	geofrm.verts[vtx][2] = ((float)zz + z)*(float)cell_size;

	geofrm.normals[vtx][0] = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
								+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
	geofrm.normals[vtx][1] = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
								+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
	geofrm.normals[vtx][2] = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
								+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);
*/
	// find the intersection point with the trilinear function (the finest resolution)
	int level, cell_size, xx, yy, zz, tx, ty, tz, i, my_bool, my_id;
	float val[8], x, y, z, mx, my, mz, step, func, func0, n[3], temp;

	level = get_level(oc_id) ;
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_id, xx, yy, zz, level);

	mx = geofrm.verts[vtx][0]/cell_size - (float)xx;
	my = geofrm.verts[vtx][1]/cell_size - (float)yy;
	mz = geofrm.verts[vtx][2]/cell_size - (float)zz;

	tx = cell_size*xx + (int)(mx*cell_size);
	ty = cell_size*yy + (int)(my*cell_size);
	tz = cell_size*zz + (int)(mz*cell_size);

	my_id = xyz2octcell(tx, ty, tz, oct_depth);
	getCellValues(my_id, oct_depth, val);

	x = geofrm.verts[vtx][0] - (float)tx;
	y = geofrm.verts[vtx][1] - (float)ty;
	z = geofrm.verts[vtx][2] - (float)tz;

	func0 = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
				+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
				+ x*y*(1-z)*val[5] + x*y*z*val[6] - iso_val;

	for(i = 0; i < 3; i++) n[i] = geofrm.normals[vtx][i];
	temp = (float) sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	if(temp > 0.001f) for(i = 0; i < 3; i++) n[i] /= temp;

	if(fabs(func0) >= 0.001733f) { //0.2
		for(i = 1; i < 1000*cell_size; i++) {

			if(fabs(func0) < 0.001733f) break;

			step = -0.001f;
			if(func0 < 0.0f) step = 0.001f;
			/*		
			n[0] = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
										+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
			n[1] = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
										+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
			n[2] = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
										+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);
			temp = (float) sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
			if(temp > 0.001f) for(int j = 0; j < 3; j++) n[j] /= temp;
			*/
			x = x + n[0]*step;
			y = y + n[1]*step;
			z = z + n[2]*step;
			func = (1-x)*(1-y)*(1-z)*val[0] + (1-x)*(1-y)*z*val[3] + (1-x)*y*(1-z)*val[4]
						+ x*(1-y)*(1-z)*val[1] + (1-x)*y*z*val[7] + x*(1-y)*z*val[2]
						+ x*y*(1-z)*val[5] + x*y*z*val[6] - iso_val;
			my_bool = (x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f);
			if(func*func0 <= 0.0f && my_bool) break;
			func0 = func;
			if(my_bool == 0) {
				if(x < 0.0f) {tx--;	x = x + 1.0f;}	else {tx++;	x = x - 1.0f;}
				if(y < 0.0f) {ty--;	y = y + 1.0f;}	else {ty++; y = y - 1.0f;}
				if(z < 0.0f) {tz--;	z = z + 1.0f;}	else {tz++; z = z - 1.0f;}
				my_id = xyz2octcell(tx, ty, tz, oct_depth);
				getCellValues(my_id, oct_depth, val);
			}
		}

		//if(my_bool == 0 && i < 1000*cell_size) {
			geofrm.verts[vtx][0] = (float)tx + x;
			geofrm.verts[vtx][1] = (float)ty + y;
			geofrm.verts[vtx][2] = (float)tz + z;
		//}
	
		/*
		// calculate normal using the trilinear function
		geofrm.normals[vtx][0] = (1-y)*(1-z)*(val[1] - val[0]) + (1-y)*z*(val[2] - val[3])
									+ y*(1-z)*(val[5] - val[4]) + y*z*(val[6] - val[7]);
		geofrm.normals[vtx][1] = (1-x)*(1-z)*(val[4] - val[0]) + (1-x)*z*(val[7] - val[3])
									+ x*(1-z)*(val[5] - val[1]) + x*z*(val[6] - val[2]);
		geofrm.normals[vtx][2] = (1-x)*(1-z)*(val[3] - val[0]) + (1-x)*y*(val[7] - val[4])
									+ x*(1-y)*(val[2] - val[1]) + x*y*(val[6] - val[5]);
		
		temp = sqrt(geofrm.normals[vtx][0]*geofrm.normals[vtx][0] +
				geofrm.normals[vtx][1]*geofrm.normals[vtx][1] + geofrm.normals[vtx][2]*geofrm.normals[vtx][2]);
		for(i = 0; i < 3; i++)	geofrm.normals[vtx][i] /= temp;
		
		// calculate curvature using the trilinear function
		float df2_dxdy = -(1-z)*(val[1] - val[0]) + -z*(val[2] - val[3]) + (1-z)*(val[5] - val[4]) + z*(val[6] - val[7]);
		float df2_dxdz = -(1-y)*(val[1] - val[0]) + (1-y)*(val[2] - val[3]) - y*(val[5] - val[4]) + y*(val[6] - val[7]);
		float df2_dydz = -(1-x)*(val[4] - val[0]) + (1-x)*(val[7] - val[3]) - x*(val[5] - val[1]) + x*(val[6] - val[2]);

		float coef = sqrt((df2_dxdy*df2_dxdy + df2_dxdz*df2_dxdz + df2_dydz*df2_dydz)/3.0f);
		float theta = acos(df2_dxdy*df2_dxdz*df2_dydz/(coef*coef*coef));
		if(fabs(df2_dxdy*df2_dxdz*df2_dydz/(coef*coef*coef)) > 1.0) theta = 0.0f;

		float eigenvalue_0 = coef*2.0f*cos(theta/3.0f);
		float eigenvalue_1 = coef*2.0f*cos((theta + 2.0f*3.1415926f)/3.0f);
		float eigenvalue_2 = coef*2.0f*cos((theta - 2.0f*3.1415926f)/3.0f);

		float kapa_1 = eigenvalue_0;
		float kapa_2 = eigenvalue_0;
		if(kapa_1 < eigenvalue_1) kapa_1 = eigenvalue_1;
		if(kapa_1 < eigenvalue_2) kapa_1 = eigenvalue_2;
		if(kapa_2 > eigenvalue_1) kapa_2 = eigenvalue_1;
		if(kapa_2 > eigenvalue_2) kapa_2 = eigenvalue_2;

		geofrm.curvatures[vtx][0] = kapa_1;
		geofrm.curvatures[vtx][1] = kapa_2;
		*/
	}

}

void Octree::quad_adaptive(geoframe& geofrm, int* oc_id, float err_tol, unsigned int* vtx, int flag_method) {

	switch (flag_method) {
	case 1:							
		quad_adaptive_method1(geofrm, oc_id, err_tol, vtx);
		break;

	case 2:						
		quad_adaptive_method2(geofrm, oc_id, err_tol, vtx);
		break;

	case 3:						
		quad_adaptive_method3(geofrm, oc_id, err_tol, vtx, flag_method);
		break;

	case 4:						
		quad_adaptive_method3(geofrm, oc_id, err_tol, vtx, flag_method);
		break;

	case 5:						
		quad_adaptive_method5(geofrm, oc_id, err_tol, vtx);
		break;
	}

}

void Octree::quad_adaptive_method1(geoframe& geofrm, int* oc_id, float err_tol, unsigned int* vtx) {

	int i, j, vtx_num, xx, yy, zz, level, cell_size, vtx_new_num;
	unsigned int vtx_new[12];
	float x, y, z;
	vtx_num = 4;
	
	if (get_err_grad(oc_id[0]) > err_tol || get_err_grad(oc_id[1]) > err_tol ||
		get_err_grad(oc_id[2]) > err_tol || get_err_grad(oc_id[3]) > err_tol) {

		vtx_new_num = 4;
		geofrm.AddVert_adaptive(vtx, vtx_new);
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		for(i = 0; i < 4; i++) get_vtx_new(geofrm, oc_id[i], vtx[i]);
		geofrm.AddQuad_adaptive(vtx, vtx_new, vtx_num);
	}
	else
		geofrm.AddQuad(vtx, vtx_num);
	
}

void Octree::quad_adaptive_method2(geoframe& geofrm, int* oc_id, float err_tol, unsigned int* vtx) {

	int num_id, i, j, vtx_num, xx, yy, zz, level, cell_size, vtx_new_num;
	unsigned int temp_vtx[4], vtx_new[12];
	float x, y, z;

	vtx_num = 4;

	num_id = 0;
	if(get_err_grad(oc_id[0]) > err_tol) num_id++;
	if(get_err_grad(oc_id[1]) > err_tol) num_id++;
	if(get_err_grad(oc_id[2]) > err_tol) num_id++;
	if(get_err_grad(oc_id[3]) > err_tol) num_id++;

	for(i = 0; i < 4; i++) {
		//if(get_err_grad(oc_id[i]) > err_tol) get_vtx_new(geofrm, oc_id[i], vtx[i]);
		get_vtx_new(geofrm, oc_id[i], vtx[i]);

		//if(geofrm.vtxnew_sign[vtx[i]] == 0) {
		//	get_vtx_new(geofrm, oc_id[i], vtx[i]);
		//	geofrm.AddVtxNew(vtx[i], 1);
		//}
	}

	for(i = 0; i < 4; i++)	temp_vtx[i] = vtx[i];
	if(num_id == 0)
		geofrm.AddQuad(vtx, vtx_num);
	else if(num_id == 1) {
		if(get_err_grad(oc_id[1]) > err_tol) {
			vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
			vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
		}
		else if(get_err_grad(oc_id[2]) > err_tol) {
			vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
			vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
		}
		else if(get_err_grad(oc_id[3]) > err_tol) {
			vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
			vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
		}
		geofrm.AddVert_adaptive_2_1(vtx, vtx_new);
		vtx_new_num = 6;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
			//if(geofrm.vtxnew_sign[vtx_new[i]] == 0) {
			//	get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
			//	geofrm.AddVtxNew(vtx_new[i], 1);
			//}
		}
		geofrm.AddQuad_adaptive_2_1(vtx, vtx_new, vtx_num);
	}
	else if(num_id == 2) {
		if( (get_err_grad(oc_id[0]) > err_tol && get_err_grad(oc_id[2]) > err_tol) ||
			(get_err_grad(oc_id[1]) > err_tol && get_err_grad(oc_id[3]) > err_tol) ) {
			geofrm.AddVert_adaptive_4(vtx, vtx_new);
			vtx_new_num = 12;
			for(i = 0; i < vtx_new_num; i++) {
				for(j = 0; j < 4; j++) {
					level = get_level(oc_id[j]) ;
					cell_size = (dim[0]-1)/(1<<level);
					octcell2xyz(oc_id[j], xx, yy, zz, level);
					x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
					y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
					z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
					if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
				}
				if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
				//if(geofrm.vtxnew_sign[vtx_new[i]] == 0) {
				//	get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
				//	geofrm.AddVtxNew(vtx_new[i], 1);
				//}
			}
			geofrm.AddQuad_adaptive_4(vtx, vtx_new, vtx_num);
		}
		else {
			if(get_err_grad(oc_id[1]) > err_tol && get_err_grad(oc_id[2]) > err_tol) {
				vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
				vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
			}
			else if(get_err_grad(oc_id[2]) > err_tol && get_err_grad(oc_id[3]) > err_tol) {
				vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
				vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
			}
			else if(get_err_grad(oc_id[3]) > err_tol && get_err_grad(oc_id[0]) > err_tol) {
				vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
				vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
			}
			geofrm.AddVert_adaptive_2_3(vtx, vtx_new);
			vtx_new_num = 8;
			for(i = 0; i < vtx_new_num; i++) {
				for(j = 0; j < 4; j++) {
					level = get_level(oc_id[j]) ;
					cell_size = (dim[0]-1)/(1<<level);
					octcell2xyz(oc_id[j], xx, yy, zz, level);
					x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
					y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
					z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
					if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
				}
				if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
				//if(geofrm.vtxnew_sign[vtx_new[i]] == 0) {
				//	get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
				//	geofrm.AddVtxNew(vtx_new[i], 1);
				//}
			}
			geofrm.AddQuad_adaptive_2_3(vtx, vtx_new, vtx_num);
		}
	}
	else {
		geofrm.AddVert_adaptive_4(vtx, vtx_new);
		vtx_new_num = 12;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
			//if(geofrm.vtxnew_sign[vtx_new[i]] == 0) {
			//	get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
			//	geofrm.AddVtxNew(vtx_new[i], 1);
			//}
		}
		geofrm.AddQuad_adaptive_4(vtx, vtx_new, vtx_num);
	}

}

void Octree::quad_adaptive_method3(geoframe& geofrm, int* oc_id, float err_tol, unsigned int* vtx, int flag_method) {

	int num_id, i, j, vtx_num, xx, yy, zz, level, cell_size, vtx_new_num;
	unsigned int temp_vtx[4], vtx_new[12];
	float x, y, z;

	vtx_num = 4;

	num_id = 0;
	if(get_err_grad(oc_id[0]) > err_tol) num_id++;
	if(get_err_grad(oc_id[1]) > err_tol) num_id++;
	if(get_err_grad(oc_id[2]) > err_tol) num_id++;
	if(get_err_grad(oc_id[3]) > err_tol) num_id++;

	for(i = 0; i < 4; i++) {
		//if(get_err_grad(oc_id[i]) > err_tol) get_vtx_new(geofrm, oc_id[i], vtx[i]);
		get_vtx_new(geofrm, oc_id[i], vtx[i]);
	}

	for(i = 0; i < 4; i++)	temp_vtx[i] = vtx[i];
	if(num_id == 0)
		geofrm.AddQuad(vtx, vtx_num);
	else if(num_id == 1) {
		if(get_err_grad(oc_id[1]) > err_tol) {
			vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
			vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
		}
		else if(get_err_grad(oc_id[2]) > err_tol) {
			vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
			vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
		}
		else if(get_err_grad(oc_id[3]) > err_tol) {
			vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
			vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
		}
		geofrm.AddVert_adaptive_3_1(vtx, vtx_new);
		vtx_new_num = 3;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		geofrm.AddQuad_adaptive_3_1(vtx, vtx_new, vtx_num);
	}
	else if(num_id == 2) {
		if( (get_err_grad(oc_id[0]) > err_tol && get_err_grad(oc_id[2]) > err_tol) ||
			(get_err_grad(oc_id[1]) > err_tol && get_err_grad(oc_id[3]) > err_tol) ) {
			if(get_err_grad(oc_id[1]) > err_tol && get_err_grad(oc_id[3]) > err_tol) {
				vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
				vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
			}
			if(flag_method == 3) {
				geofrm.AddVert_adaptive_3_2b(vtx, vtx_new);
				vtx_new_num = 5;
			}
			else {
				geofrm.AddVert_adaptive_4_2b(vtx, vtx_new);
				vtx_new_num = 8;
			}
			for(i = 0; i < vtx_new_num; i++) {
				for(j = 0; j < 4; j++) {
					level = get_level(oc_id[j]) ;
					cell_size = (dim[0]-1)/(1<<level);
					octcell2xyz(oc_id[j], xx, yy, zz, level);
					x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
					y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
					z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
					if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
				}
				if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
			}
			if(flag_method == 3) geofrm.AddQuad_adaptive_3_2b(vtx, vtx_new, vtx_num);
			else geofrm.AddQuad_adaptive_4_2b(vtx, vtx_new, vtx_num);
		}
		else {
			if(get_err_grad(oc_id[1]) > err_tol && get_err_grad(oc_id[2]) > err_tol) {
				vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
				vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
			}
			else if(get_err_grad(oc_id[2]) > err_tol && get_err_grad(oc_id[3]) > err_tol) {
				vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
				vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
			}
			else if(get_err_grad(oc_id[3]) > err_tol && get_err_grad(oc_id[0]) > err_tol) {
				vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
				vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
			}
			geofrm.AddVert_adaptive_3_2a(vtx, vtx_new);
			vtx_new_num = 8;
			for(i = 0; i < vtx_new_num; i++) {
				for(j = 0; j < 4; j++) {
					level = get_level(oc_id[j]) ;
					cell_size = (dim[0]-1)/(1<<level);
					octcell2xyz(oc_id[j], xx, yy, zz, level);
					x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
					y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
					z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
					if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
				}
				if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
			}
			geofrm.AddQuad_adaptive_3_2a(vtx, vtx_new, vtx_num);
		}
	}
	else if(num_id == 3) {
		if(get_err_grad(oc_id[0]) <= err_tol) {
			vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
			vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
		}
		else if(get_err_grad(oc_id[1]) <= err_tol) {
			vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
			vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
		}
		else if(get_err_grad(oc_id[2]) <= err_tol) {
			vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
			vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
		}
		geofrm.AddVert_adaptive_3_3(vtx, vtx_new);
		vtx_new_num = 10;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		geofrm.AddQuad_adaptive_3_3(vtx, vtx_new, vtx_num);
	}
	else {
		geofrm.AddVert_adaptive_4(vtx, vtx_new);
		vtx_new_num = 12;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		geofrm.AddQuad_adaptive_4(vtx, vtx_new, vtx_num);
	}

}

void Octree::quad_adaptive_method5(geoframe& geofrm, int* oc_id, float err_tol, unsigned int* vtx) {

	int num_id, i, j, vtx_num, xx, yy, zz, level, cell_size, vtx_new_num;
	unsigned int temp_vtx[4], vtx_new[12];
	float x, y, z;

	vtx_num = 4;

	num_id = 0;
	if(vtx_idx_arr_refine[oc_id[0]] == 1) num_id++;
	if(vtx_idx_arr_refine[oc_id[1]] == 1) num_id++;
	if(vtx_idx_arr_refine[oc_id[2]] == 1) num_id++;
	if(vtx_idx_arr_refine[oc_id[3]] == 1) num_id++;

	for(i = 0; i < 4; i++) {
		//if(get_err_grad(oc_id[i]) > err_tol) get_vtx_new(geofrm, oc_id[i], vtx[i]);
		get_vtx_new(geofrm, oc_id[i], vtx[i]);
	}

	for(i = 0; i < 4; i++)	temp_vtx[i] = vtx[i];
	if(num_id == 0)
		geofrm.AddQuad(vtx, vtx_num);
	else if(num_id == 1) {
		if(vtx_idx_arr_refine[oc_id[1]] == 1) {
			vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
			vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
		}
		else if(vtx_idx_arr_refine[oc_id[2]] == 1) {
			vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
			vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
		}
		else if(vtx_idx_arr_refine[oc_id[3]] == 1) {
			vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
			vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
		}
		geofrm.AddVert_adaptive_3_1(vtx, vtx_new);
		vtx_new_num = 3;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		geofrm.AddQuad_adaptive_3_1(vtx, vtx_new, vtx_num);
	}
	else if(num_id == 2) {
		if(vtx_idx_arr_refine[oc_id[1]] == 1 && vtx_idx_arr_refine[oc_id[2]] == 1) {
			vtx[0] = temp_vtx[1];	vtx[1] = temp_vtx[2];
			vtx[2] = temp_vtx[3];	vtx[3] = temp_vtx[0];
		}
		else if(vtx_idx_arr_refine[oc_id[2]] == 1 && vtx_idx_arr_refine[oc_id[3]] == 1) {
			vtx[0] = temp_vtx[2];	vtx[1] = temp_vtx[3];
			vtx[2] = temp_vtx[0];	vtx[3] = temp_vtx[1];
		}
		else if(vtx_idx_arr_refine[oc_id[3]] == 1 && vtx_idx_arr_refine[oc_id[0]] == 1) {
			vtx[0] = temp_vtx[3];	vtx[1] = temp_vtx[0];
			vtx[2] = temp_vtx[1];	vtx[3] = temp_vtx[2];
		}
		geofrm.AddVert_adaptive_3_2a(vtx, vtx_new);
		vtx_new_num = 8;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		geofrm.AddQuad_adaptive_3_2a(vtx, vtx_new, vtx_num);
	}
	else if(num_id == 4) {
		geofrm.AddVert_adaptive_4(vtx, vtx_new);
		vtx_new_num = 12;
		for(i = 0; i < vtx_new_num; i++) {
			for(j = 0; j < 4; j++) {
				level = get_level(oc_id[j]) ;
				cell_size = (dim[0]-1)/(1<<level);
				octcell2xyz(oc_id[j], xx, yy, zz, level);
				x = geofrm.verts[vtx_new[i]][0]/cell_size - (float)xx;
				y = geofrm.verts[vtx_new[i]][1]/cell_size - (float)yy;
				z = geofrm.verts[vtx_new[i]][2]/cell_size - (float)zz;
				if(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f && z >= 0.0f && z <= 1.0f) break;
			}
			if(j < 4) get_vtx_new(geofrm, oc_id[j], vtx_new[i]);
		}
		geofrm.AddQuad_adaptive_4(vtx, vtx_new, vtx_num);
	}

}

void Octree::assign_refine_sign_quad(geoframe& geofrm, float err_tol) {

	int x, y, z, valid_leaf, level, num_id, refine_sign;
	int intersect_id, i, j, k, oc_id[4];
	float val[8];

	for (k = 0; k < octcell_num; k++) vtx_idx_arr_refine[k] = -1;

	for (i = 0; i < leaf_num; i++ ) {
		valid_leaf = cut_array[i] ;
		level = get_level(valid_leaf) ;
		octcell2xyz(valid_leaf, x, y, z, level);
		getCellValues(valid_leaf, level, val);

		for (j = 0 ; j < 12 ; j++ ) {
			if (is_eflag_on(x,y,z,level,j)) continue;
			intersect_id = is_intersect(val, j);

			if (intersect_id == 1 || intersect_id == -1) {
				eflag_on(x, y, z, level, j);
				find_oc_id(x, y, z, level, j, intersect_id, oc_id);
				
				num_id = 0;
				if(get_err_grad(oc_id[0]) > err_tol) num_id++;
				if(get_err_grad(oc_id[1]) > err_tol) num_id++;
				if(get_err_grad(oc_id[2]) > err_tol) num_id++;
				if(get_err_grad(oc_id[3]) > err_tol) num_id++;
				
				//if( num_id > 2 || 
				//	(num_id == 2 && get_err_grad(oc_id[0]) > err_tol && get_err_grad(oc_id[2]) > err_tol) ||
				//	(num_id == 2 && get_err_grad(oc_id[1]) > err_tol && get_err_grad(oc_id[3]) > err_tol) ) {
				if(num_id == 4) {
					vtx_idx_arr_refine[oc_id[0]] = 1;
					vtx_idx_arr_refine[oc_id[1]] = 1;
					vtx_idx_arr_refine[oc_id[2]] = 1;
					vtx_idx_arr_refine[oc_id[3]] = 1;
				}

			}
		}	// end j
	}		// end i

	eflag_clear();

	refine_sign = 1;

	while(refine_sign == 1) {

		refine_sign = 0;

		for (i = 0; i < leaf_num; i++ ) {
			valid_leaf = cut_array[i] ;
			level = get_level(valid_leaf) ;
			octcell2xyz(valid_leaf, x, y, z, level);
			getCellValues(valid_leaf, level, val);

			for (j = 0 ; j < 12 ; j++ ) {
				if (is_eflag_on(x,y,z,level,j)) continue;
				intersect_id = is_intersect(val, j);

				if (intersect_id == 1 || intersect_id == -1) {
					eflag_on(x, y, z, level, j);
					find_oc_id(x, y, z, level, j, intersect_id, oc_id);

					num_id = 0;
					if(vtx_idx_arr_refine[oc_id[0]] == 1) num_id++;
					if(vtx_idx_arr_refine[oc_id[1]] == 1) num_id++;
					if(vtx_idx_arr_refine[oc_id[2]] == 1) num_id++;
					if(vtx_idx_arr_refine[oc_id[3]] == 1) num_id++;

					if( num_id > 2 || 
						(num_id == 2 && vtx_idx_arr_refine[oc_id[0]] == 1 && vtx_idx_arr_refine[oc_id[2]] == 1) ||
						(num_id == 2 && vtx_idx_arr_refine[oc_id[1]] == 1 && vtx_idx_arr_refine[oc_id[3]] == 1) ) {
						if(vtx_idx_arr_refine[oc_id[0]] != 1) {vtx_idx_arr_refine[oc_id[0]] = 1; refine_sign = 1;}
						if(vtx_idx_arr_refine[oc_id[1]] != 1) {vtx_idx_arr_refine[oc_id[1]] = 1; refine_sign = 1;}
						if(vtx_idx_arr_refine[oc_id[2]] != 1) {vtx_idx_arr_refine[oc_id[2]] = 1; refine_sign = 1;}
						if(vtx_idx_arr_refine[oc_id[3]] != 1) {vtx_idx_arr_refine[oc_id[3]] = 1; refine_sign = 1;}
					}

				}
			}	// end j
		}		// end i

		eflag_clear();

	}			// end while

}

void Octree::tetra_to_4_hexa(geoframe& geofrm) {

	// get a list of leaf cells satisfying the error tolerance.
	// for each cell, get the centroid vertex

	int x, y, z, valid_leaf, cell_size, level;
	int vtx_num, intersect_id, i, j;
	unsigned int vtx[4];
	float val[8];

	int k;
	for (k=0;k<octcell_num;k++) vtx_idx_arr[k] = -1;

	for (i = 0; i < leaf_num; i++ ) {
		valid_leaf = cut_array[i] ;
		level = get_level(valid_leaf) ;
		cell_size = (dim[0]-1)/(1<<level);
		octcell2xyz(valid_leaf, x, y, z, level);
		getCellValues(valid_leaf, level, val);
		for (j = 0 ; j < 12 ; j++ ) {
			if (is_eflag_on(x,y,z,level,j)) continue;
			intersect_id = is_intersect(val , j);

			if (intersect_id == 1 || intersect_id == -1) {
				if (is_min_edge(valid_leaf , j , vtx , vtx_num , intersect_id , geofrm)) {
					eflag_on(x , y , z , level , j);
					geofrm.AddQuad_hexa(vtx , vtx_num);
				}
			}
		}
	}

}

void Octree::polygonize(geoframe& geofrm) {

	// get a list of leaf cells satisfying the error tolerance.
	// for each cell, get the centroid vertex

	int x, y, z, valid_leaf, cell_size, level;
	int vtx_num, intersect_id, i, j;
	unsigned int vtx[4];
	float val[8];

	in_out = 0;
	int k;
	for (k = 0; k < octcell_num; k++) vtx_idx_arr[k] = -1;

	for (i = 0; i < leaf_num; i++ ) {
		valid_leaf = cut_array[i] ;
		level = get_level(valid_leaf) ;
		cell_size = (dim[0]-1)/(1<<level);
		octcell2xyz(valid_leaf, x, y, z, level);
		getCellValues(valid_leaf, level, val);

		for (j = 0 ; j < 12 ; j++ ) {
			if (is_eflag_on(x,y,z,level,j)) continue;
			intersect_id = is_intersect(val , j);

			if (intersect_id == 1 || intersect_id == -1) {
				if (is_min_edge(valid_leaf, j, vtx, vtx_num, intersect_id, geofrm)) {
					eflag_on(x, y, z, level, j);
					geofrm.Add_2_Tri(vtx);
					//geofrm.AddQuad(vtx, 4);
					//geofrm.AddQuad_indirect(vtx);
				}
			}
		}
	}

}

void Octree::polygonize_quad(geoframe& geofrm, float err_tol) {

	int x, y, z, valid_leaf, level;
	int vtx_num, intersect_id, i, j, k, oc_id[4], flag_method;
	unsigned int vtx[4];
	float val[8];

	in_out = 0;
	for (k = 0; k < octcell_num; k++) {vtx_idx_arr[k] = -1;}

	flag_method = 5;	// 1, 2, 3, 4, 5
	if(flag_method == 5) assign_refine_sign_quad(geofrm, err_tol);

	for (i = 0; i < leaf_num; i++ ) {
		valid_leaf = cut_array[i] ;
		level = get_level(valid_leaf) ;
		octcell2xyz(valid_leaf, x, y, z, level);
		getCellValues(valid_leaf, level, val);

		for (j = 0 ; j < 12 ; j++ ) {
			if (is_eflag_on(x,y,z,level,j)) continue;
			intersect_id = is_intersect(val, j);

			if (intersect_id == 1 || intersect_id == -1) {
				if (is_min_edge(valid_leaf, j, vtx, vtx_num, intersect_id, geofrm)) {
					eflag_on(x, y, z, level, j);
					find_oc_id(x, y, z, level, j, intersect_id, oc_id);
					//find_vtx_new(geofrm, x, y, z, level, j, intersect_id, vtx_new);
					quad_adaptive(geofrm, oc_id, err_tol, vtx, flag_method);
				}
			}
		}	// end j
	}		// end i
}

void Octree::polygonize_interval(geoframe& geofrm) {

	// get a list of leaf cells satisfying the error tolerance.
	// for each cell, get the centroid vertex

	int x, y, z, valid_leaf, cell_size, level;
	int vtx_num, intersect_id, i, j;
	unsigned int vtx[4];
	float val[8];

	int k;
	for (k = 0; k < octcell_num; k++) {vtx_idx_arr[k] = -1;	vtx_idx_arr_in[k] = -1;}

	for (i = 0; i < leaf_num; i++ ) {
		valid_leaf = cut_array[i] ;
		level = get_level(valid_leaf) ;
		cell_size = (dim[0]-1)/(1<<level);
		octcell2xyz(valid_leaf, x, y, z, level);
		getCellValues(valid_leaf, level, val);

		for (j = 0; j < 12; j++ ) {
			if (is_eflag_on(x, y, z, level, j)) continue;
			intersect_id = is_intersect_interval(val, j);

			if (intersect_id == 1 || intersect_id == -1) {
				if(is_skipcell(valid_leaf) == 0) in_out = 0;
				else in_out = 1;
				if (is_min_edge(valid_leaf, j, vtx, vtx_num, intersect_id, geofrm)) {
					eflag_on(x, y, z, level, j);
					//geofrm.AddQuad(vtx, vtx_num);
					geofrm.Add_2_Tri(vtx);
				}
			}

			if (intersect_id == 3 || intersect_id == -3) {
				in_out = 1;
				if (is_min_edge(valid_leaf, j, vtx, vtx_num, intersect_id, geofrm)) {
					eflag_on(x, y, z, level, j);
					//geofrm.AddQuad(vtx, vtx_num);
					geofrm.Add_2_Tri(vtx);
					in_out = 0;
					is_min_edge(valid_leaf, j, vtx, vtx_num, intersect_id, geofrm);
					//geofrm.AddQuad(vtx, vtx_num);
					geofrm.Add_2_Tri(vtx);
				}
			}
		}
	}

}

void Octree::func_val(geoframe& geofrm) {
	int i, j, x, y, z, oc_id, vtx[8];
	float dx, dy, dz, val[8], func_min, func_max;
	float* func_vol;
	FILE *func_fp;

	func_fp = fopen("rawiv/1MAH_pot_129.rawiv","rb");
	
	if (func_fp==NULL) {
		printf("wrong name : %s\n","1MAH_pot_129.rawiv");
		return;
	}
	
	getFloat(minext,3,func_fp);	getFloat(maxext,3,func_fp);
	getInt(&nverts,1,func_fp);	getInt(&ncells,1,func_fp);
	getInt(dim,3,func_fp);		getFloat(orig,3,func_fp);		getFloat(span,3,func_fp);

	func_vol = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);
	getFloat(func_vol, dim[0]*dim[1]*dim[2], func_fp);

	fclose(func_fp);

	func_min = 100.0f;
	func_max = -100.0f;
	for(i = 0; i < geofrm.numverts; i++) {
		x = (int) geofrm.verts[i][0];
		y = (int) geofrm.verts[i][1];
		z = (int) geofrm.verts[i][2];

		dx = geofrm.verts[i][0] - x;
		dy = geofrm.verts[i][1] - y;
		dz = geofrm.verts[i][2] - z;

		oc_id = xyz2octcell(x, y, z, oct_depth);
		idx2vtx(oc_id, oct_depth, vtx);
		for (j = 0; j < 8; j++)	val[j] = func_vol[vtx[j]];

		geofrm.funcs[i][0] = (1-dx)*(1-dy)*(1-dz)*val[0] + (1-dx)*(1-dy)*dz*val[3]
						+ (1-dx)*dy*(1-dz)*val[4] + dx*(1-dy)*(1-dz)*val[1] + (1-dx)*dy*dz*val[7]
						+ dx*(1-dy)*dz*val[2] + dx*dy*(1-dz)*val[5] + dx*dy*dz*val[6];
		if(geofrm.funcs[i][0] < func_min)	func_min = geofrm.funcs[i][0];
		if(geofrm.funcs[i][0] > func_max)	func_max = geofrm.funcs[i][0];
	}
	//for(i = 0; i < geofrm.numverts; i++) {
	//	geofrm.funcs[i][0] = (geofrm.funcs[i][0] - func_min) / (func_max - func_min)*2.0f - 1.0f;
	//}

	free(func_vol);
}

void Octree::mesh_extract(geoframe& geofrm, float err_tol)
{
	geofrm.Clear();
	eflag_clear();
	vflag_clear();

	//flag_type = 1;
	in_out = 0;
	flag_extend = 1;
	int func = 0;

	//Commented out: A.O.
	  //BSplineCoeff = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);
	//TransImg2Spline(orig_vol, BSplineCoeff, dim[0], dim[1], dim[2]);

	switch (flag_type) {
	case 0:							// isosurface
		polygonize(geofrm);
		if(func == 1)	func_val(geofrm);
		break;

	case 1:							// tetra mesh
		tetrahedralize(geofrm);
		break;

	case 2:							// hexa  mesh
		hexahedralize(geofrm, err_tol);
		break;

	case 3:
		//tetra_to_4_hexa(geofrm);
		polygonize_quad(geofrm, err_tol);
		break;

	case 4:							// interval
		polygonize_interval(geofrm);
		break;

	case 5:							// interval volume
		tetrahedralize_interval(geofrm);
		break;
	}
}

void Octree::quality_improve(geoframe& geofrm)
{
/*	int i;
	unsigned int v[4];

	// edge contraction -- surface
	for (i = 0; i < geofrm.numtris; i++) {
		v[0] = geofrm.triangles[i][0];
		v[1] = geofrm.triangles[i][1];
		v[2] = geofrm.triangles[i][2];
		if(geofrm.bound_sign[v[0]] == 1 && geofrm.bound_sign[v[1]] == 1
										&& geofrm.bound_sign[v[2]] == 1)
			geofrm.edge_contraction(v, 3);
	}

	if(flag_type == 1) {
		// edge contraction -- tetrahedra
		for (i = 0; i < (geofrm.numtris / 4); i++) {
			v[0] = geofrm.triangles[i][0];
			v[1] = geofrm.triangles[i][1];
			v[2] = geofrm.triangles[i][2];
			v[3] = geofrm.triangles[i][3];
			if(geofrm.bound_sign[v[0]] == 1 && geofrm.bound_sign[v[1]] == 1
											&& geofrm.bound_sign[v[2]] == 1)
				geofrm.edge_contraction(v, 4);
			//if(geofrm.bound_sign[v[0]] == 1 && geofrm.bound_sign[v[1]] == 1
			//								&& geofrm.bound_sign[v[2]] == 0 && geofrm.bound_sign[v[3]] == 0)
			//	geofrm.edge_contraction(v, 5);
		}
	}
*/
}

int Octree::is_intersect(float* val, int e_id)
{
	float f1,f2;

	f1 = val[cube_eid[e_id][0]];
	f2 = val[cube_eid[e_id][1]];

	if (iso_val <= f1 && iso_val >= f2)
		return -1;
	else if (iso_val <= f2 && iso_val >= f1)
		return 1;
	else if (iso_val >= f1 && f1 >= f2)
		return -2;
	else if (iso_val >= f2 && f2 >= f1)
		return 2;
	else return 0;

}

int Octree::is_intersect_interval(float* val, int e_id)
{
	float f1,f2;

	f1 = val[cube_eid[e_id][0]];
	f2 = val[cube_eid[e_id][1]];

	if ((iso_val <= f1 && iso_val >= f2 && f2 >= iso_val_in) ||
		(iso_val_in <= f1 && iso_val_in >= f2 && f1 <= iso_val))
		return -1;
	else if ((iso_val <= f2 && iso_val >= f1 && f1 >= iso_val_in) ||
			 (iso_val_in <= f2 && iso_val_in >= f1 && f2 <= iso_val))
		return 1;
	else if (iso_val >= f1 && f1 >= f2 && f2 >= iso_val_in)
		return -2;
	else if (iso_val >= f2 && f2 >= f1 && f1 >= iso_val_in)
		return 2;
	else if (f1 >= iso_val && iso_val_in >= f2)
		return -3;
	else if (f2 >= iso_val && iso_val_in >= f1)
		return 3;
	else return 0;

}

void Octree::read_header()
{
	getFloat(minext,3,vol_fp);
	getFloat(maxext,3,vol_fp);
	
	getInt(&nverts,1,vol_fp);
	getInt(&ncells,1,vol_fp);
	
	getInt(dim,3,vol_fp);
	getFloat(orig,3,vol_fp);
	getFloat(span,3,vol_fp);
	
}

void Octree::read_data()
{
	int i;
	//int j, k, index;
	//float r, r0, r1;
	float center;
	center = (dim[0] - 1.0f)/2.0f;
	//float *temp;

	//temp = (float*)malloc(sizeof(float)*dim[0]*dim[1]*dim[2]);

	// currently support only float data
	printf ("in read_data: dims: %d, %d, %d\n",dim[0], dim[1], dim[2]); 
	getFloat(orig_vol, dim[0]*dim[1]*dim[2], vol_fp);
	//getFloat(temp, dim[0]*dim[1]*dim[2], vol_fp);

	// remove the outer sphere
	for(i = 0; i < dim[0]*dim[1]*dim[2]; i++) {
		orig_vol[i] = - orig_vol[i];
	}
/*
	for(k = 0; k < dim[2]; k++)
		for(j = 0; j < dim[1]; j++)
			for(i = 0; i < dim[0]; i++) {
				index = k*dim[0]*dim[1] + j*dim[0] + i;
				if( i == 0 || i > 66 || j == 0 || 
					j == dim[1]-1 || k == 0 || k == dim[2]-1 || (i < 60 && j < 60 && k < 60))
					orig_vol[index] = 10.0f;
			}

	for(k = 0; k < dim[2]; k++)
		for(j = 0; j < dim[1]; j++)
			for(i = 0; i < dim[0]; i++) {
				index = k*dim[0]*dim[1] + j*dim[0] + i;
				r = (float) sqrt((i-center)*(i-center) + (j-center)*(j-center) + (k-center)*(k-center));
				if(r > 28.0f && orig_vol[index] <= 0.0f) orig_vol[index] = 1.0-orig_vol[index];
			}


	int index_0;
	for(k = 0; k < dim[2]; k++)
		for(j = 0; j < dim[1]; j++)
			for(i = 0; i < dim[0]; i++) {
				index_0 = k*dim[0]*dim[1] + j*dim[0] + i;
				index = (k-8)*dim[0]*dim[1] + (j-8)*dim[0] + (i-8);
				if(k > 8 && j > 8 & i > 8) orig_vol[index_0] = temp[index];
				else orig_vol[index_0] = 1.0;
			}

	//r0 = center - 0.1f;
	r0 = 45.0f;
	for(k = 0; k < dim[2]; k++)
		for(j = 0; j < dim[1]; j++)
			for(i = 0; i < dim[0]; i++) {
				index = k*dim[0]*dim[1] + j*dim[0] + i;
				//orig_vol[index] = 0.5f - orig_vol[index];
				orig_vol[index] = orig_vol[index] - 1.0f;
				r = (float) sqrt((i-center)*(i-center) + (j-center)*(j-center) + (k-center)*(k-center));
				if(orig_vol[index] < 0.0) {
					if((r>r0) || ((r0-r) < -orig_vol[index]))	orig_vol[index] = r - r0;
				}
			}

	//free(temp);

	r0 = 10.0f;	
	r1 = 30.0f;
	for(k = 0; k < dim[2]; k++)
		for(j = 0; j < dim[1]; j++)
			for(i = 0; i < dim[0]; i++) {
				index = k*dim[0]*dim[1] + j*dim[0] + i;
				r = sqrt((i-32)*(i-32) + (j-32)*(j-32) + (k-32)*(k-32));
				if(r < r0) orig_vol[index] = r0 - r;
				else if(r > r1) orig_vol[index] = r - r1;
				else {
					if(r < (r0+r1)/2.0) orig_vol[index] = r0 - r;
					else orig_vol[index] = r - r1;
				}
			}

	r1 = 15.0f;
	for(k = 0; k < dim[2]; k++)
		for(j = 0; j < dim[1]; j++)
			for(i = 0; i < dim[0]; i++) {
				index = k*dim[0]*dim[1] + j*dim[0] + i;
				r = sqrt((i-32)*(i-32) + (j-32)*(j-32) + (k-32)*(k-32));
				orig_vol[index] = r - r1;
			}
*/
}


int Octree::get_neighbor_bit(int id,int level)
{
	int x,y,z;
	int reg_bitmask=0;
	
	octcell2xyz(id,x,y,z,level);
	reg_bitmask|=(is_refined(x  , y-1, z-1, level)<<0);
	reg_bitmask|=(is_refined(x-1, y  , z-1, level)<<1);
	reg_bitmask|=(is_refined(x  , y  , z-1, level)<<2);
	reg_bitmask|=(is_refined(x+1, y  , z-1, level)<<3);
	reg_bitmask|=(is_refined(x  , y+1, z-1, level)<<4);
	
	reg_bitmask|=(is_refined(x-1, y-1, z, level)<<5);
	reg_bitmask|=(is_refined(x  , y-1, z, level)<<6);
	reg_bitmask|=(is_refined(x+1, y-1, z, level)<<7);
	reg_bitmask|=(is_refined(x-1, y  , z, level)<<8);
	reg_bitmask|=(is_refined(x+1, y  , z, level)<<9);
	reg_bitmask|=(is_refined(x-1, y+1, z, level)<<10);
	reg_bitmask|=(is_refined(x  , y+1, z, level)<<11);
	reg_bitmask|=(is_refined(x+1, y+1, z, level)<<12);
	
	reg_bitmask|=(is_refined(x  , y-1, z+1, level)<<13);
	reg_bitmask|=(is_refined(x-1, y  , z+1, level)<<14);
	reg_bitmask|=(is_refined(x  , y  , z+1, level)<<15);
	reg_bitmask|=(is_refined(x+1, y  , z+1, level)<<16);
	reg_bitmask|=(is_refined(x  , y+1, z+1, level)<<17);
	
	return reg_bitmask;
}


int Octree::is_refined2(int x, int y, int z, int level)
{
	int idx=0;
	int res;
	
	res = (1<<level) ;
	
	if (x<0 || y<0 || z<0) return 0;
	if (x>=res || y>=res || z>=res) return 0;
	
	idx = level_id[level] + x + y*res + z*res*res;
	if (oct_array[idx].refine_flag==1) return 1;
	else return 0;
	//return oct_array[idx].refine_flag;
}

int Octree::is_refined(int x, int y, int z, int level)
{
	int idx=0;
	int res;
	
	res = (1<<level) ;
	
	if (x<0 || y<0 || z<0) return 1;
	if (x>=res || y>=res || z>=res) return 1;
	
	idx = level_id[level] + x + y*res + z*res*res;
	if (oct_array[idx].refine_flag==0) return 0;
	else return 1;
	//return oct_array[idx].refine_flag;
}

int Octree::get_depth(int res)
{
	int i=0;
	while (1) {
		if (res<=(1<<i)+1) break;
		i++;
	}
	if (res!=(1<<i)+1) {
		printf("unsupported resolution : %d\n",res);
		//		exit(0);
	}
	return i;
}

int Octree::get_octcell_num(int depth)
{
	int num=0;
	for (int i=0;i<=depth;i++) {
		num+=(1<<(i*3));
	}
	return num;
}

int Octree::get_level(int oc_id)
{
	int num=0;
	int i=0;
	while (1) {
		num+=(1<<(i*3));
		if (num>oc_id) break;
		i++;
	}
	return i;
}

void Octree::octcell2xyz(int oc_id,int& x,int& y,int& z,int level)
{
	int idx;
	int lres;
	
	idx=oc_id-level_id[level];
	lres=level_res[level];
	x=idx%lres;
	y=(idx/lres)%lres;
	z=idx/(lres*lres);
}

int Octree::xyz2octcell(int x,int y,int z,int level)
{
	int lres;
	
	lres=level_res[level];
	if (x<0 || y<0 || z<0 || x>=lres || y>=lres || z>=lres) return -1;
	return level_id[level]+x+y*lres+z*lres*lres;
}

int Octree::child(int oc_id, int level, int i)
{
	int x,y,z;
	int ret_idx;
	octcell2xyz(oc_id,x,y,z,level);
	
	switch (i) {
	case 0 :
		ret_idx= xyz2octcell(x*2,y*2,z*2,level+1);
		break;
	case 1 :
		ret_idx= xyz2octcell(x*2+1,y*2,z*2,level+1);
		break;
	case 2 :
		ret_idx= xyz2octcell(x*2,y*2+1,z*2,level+1);
		break;
	case 3 :
		ret_idx= xyz2octcell(x*2+1,y*2+1,z*2,level+1);
		break;
	case 4 :
		ret_idx= xyz2octcell(x*2,y*2,z*2+1,level+1);
		break;
	case 5 :
		ret_idx= xyz2octcell(x*2+1,y*2,z*2+1,level+1);
		break;
	case 6 :
		ret_idx= xyz2octcell(x*2,y*2+1,z*2+1,level+1);
		break;
	case 7 :
		ret_idx= xyz2octcell(x*2+1,y*2+1,z*2+1,level+1);
		break;
	}
	return ret_idx;
}


int Octree::xyz2vtx(int x, int y, int z)
{
	return x+y*dim[0]+z*dim[0]*dim[1];
}


void Octree::idx2vtx(int oc_id, int level, int* vtx)
{
	int x,y,z;
	int x0,x2,y0,y2,z0,z2;
	int cell_size;
	
	cell_size = (dim[0]-1)/(1<<level);
	
	octcell2xyz(oc_id,x,y,z,level);
	
	x0=x*cell_size; x2=x0+cell_size;
	y0=y*cell_size; y2=y0+cell_size;
	z0=z*cell_size; z2=z0+cell_size;
	
	vtx[0]=xyz2vtx(x0,y0,z0);
	vtx[1]=xyz2vtx(x2,y0,z0);
	vtx[2]=xyz2vtx(x2,y0,z2);
	vtx[3]=xyz2vtx(x0,y0,z2);
	vtx[4]=xyz2vtx(x0,y2,z0);
	vtx[5]=xyz2vtx(x2,y2,z0);
	vtx[6]=xyz2vtx(x2,y2,z2);
	vtx[7]=xyz2vtx(x0,y2,z2);
	
}


int Octree::is_intersect(int in_idx ,float isovalue , float* in_value 
						 ,int& iv_idx , int i, int j, int k , int level , int faceidx, geoframe& geofrm)
{
	float pt[3], norm[3];
	
	EdgeInfo *ei = &edge_dir[faceidx][in_idx];
	float f1,f2;
	
	f1=in_value[ei->d1];
	f2=in_value[ei->d2];
	
	if (!((f1<=isovalue && f2>=isovalue)||(f1>=isovalue && f2<=isovalue))) {
		return 0; // false : not intersect
	} 
	if (f1==f2) return 0;
	
	switch (ei->dir) {
	case 0:
		interpRect3Dpts_x(2*i+ei->di,2*j+ei->dj,2*k+ei->dk, 
			f1, f2,isovalue,pt, norm, level+1);
		break;
		
	case 1:
		interpRect3Dpts_y(2*i+ei->di,2*j+ei->dj,2*k+ei->dk,
			f1, f2, isovalue,pt, norm, level+1);
		break;
		
	case 2:
		interpRect3Dpts_z(2*i+ei->di,2*j+ei->dj,2*k+ei->dk,
			f1, f2, isovalue,pt, norm, level+1);
		break;
	}
	assert(ei->dir==0 || ei->dir==1 || ei->dir==2);
	
	iv_idx = geofrm.AddVert(pt, norm);
	return 1;
	
}


void Octree::interpRect3Dpts_x(int i1, int j1, int k1, 
							   float d1, float d2, float val, float *pt,float *norm,int level)
{
	double ival, gval;
	int cell_size;
	float g1[3], g2[3];
	
	cell_size = (dim[0]-1)/(1<<level);
	
	ival = (val - d1)/(d2 - d1);
	pt[0] = orig[0] + span[0]*(i1 + ival)*cell_size;
	pt[1] = orig[1] + span[1]*j1*cell_size;
	pt[2] = orig[2] + span[2]*k1*cell_size;
	
	//#ifdef GRAD
	getVertGrad( (i1+ival)*cell_size   , j1*cell_size , k1*cell_size , g1);
	getVertGrad( (i1+ival)*cell_size+1 , j1*cell_size , k1*cell_size , g2);
	gval=(double)((i1+ival)*cell_size)-(double)((int)((i1+ival)*cell_size));
	
	norm[0] = g1[0]*(1-gval) + g2[0]*gval;
	norm[1] = g1[1]*(1-gval) + g2[1]*gval;
	norm[2] = g1[2]*(1-gval) + g2[2]*gval;
	
	float len= sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);
	norm[0]/=len;
	norm[1]/=len;
	norm[2]/=len;
	
	/*
	
	  // grad addtion
	  norm[0] = g1[0]*(1.0-ival) + g2[0]*ival;
	  norm[1] = g1[1]*(1.0-ival) + g2[1]*ival;
	  norm[2] = g1[2]*(1.0-ival) + g2[2]*ival;
	*/
	//#endif
	
}

void Octree::interpRect3Dpts_y(int i1, int j1, int k1, 
							   float d1, float d2, float val, float *pt,float *norm,int level)
							   
{
	double ival,gval;
	int cell_size;
	float g1[3],g2[3];
	cell_size = (dim[0]-1)/(1<<level);
	
	ival = (val - d1)/(d2 - d1);
	pt[0] = orig[0] + span[0]*i1*cell_size;
	pt[1] = orig[1] + span[1]*(j1 + ival)*cell_size;
	pt[2] = orig[2] + span[2]*k1*cell_size;
	
	//#ifdef GRAD
	// grad addtion
	
	getVertGrad( i1*cell_size   , (j1+ival)*cell_size , k1*cell_size , g1);
	getVertGrad( i1*cell_size , (j1+ival)*cell_size+1 , k1*cell_size , g2);
	gval=(double)((j1+ival)*cell_size)-(double)((int)((j1+ival)*cell_size));
	
	

	norm[0] = g1[0]*(1-gval) + g2[0]*gval;
	norm[1] = g1[1]*(1-gval) + g2[1]*gval;
	norm[2] = g1[2]*(1-gval) + g2[2]*gval;

	float len= sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);
	norm[0]/=len;
	norm[1]/=len;
	norm[2]/=len;

	
	//   norm[0] = g1[0]*(1.0-ival) + g2[0]*ival;
	//   norm[1] = g1[1]*(1.0-ival) + g2[1]*ival;
	//   norm[2] = g1[2]*(1.0-ival) + g2[2]*ival;
	//#endif
}


void Octree::interpRect3Dpts_z(int i1, int j1, int k1,
							   float d1, float d2, float val, float *pt,float *norm,int level)
							   
{
	double ival,gval;
	int cell_size;
	
	float g1[3],g2[3];
	
	cell_size = (dim[0]-1)/(1<<level);
	
	ival = (val - d1)/(d2 - d1);
	pt[0] = orig[0] + span[0]*i1*cell_size;
	pt[1] = orig[1] + span[1]*j1*cell_size;
	pt[2] = orig[2] + span[2]*(k1+ival)*cell_size;
	
	//#ifdef GRAD
	
	getVertGrad( i1*cell_size   , j1*cell_size , (k1+ival)*cell_size , g1);
	getVertGrad( i1*cell_size , j1*cell_size , (k1+ival)*cell_size +1, g2);
	gval=(double)((k1+ival)*cell_size)-(double)((int)((k1+ival)*cell_size));

	
	norm[0] = g1[0]*(1-gval) + g2[0]*gval;
	norm[1] = g1[1]*(1-gval) + g2[1]*gval;
	norm[2] = g1[2]*(1-gval) + g2[2]*gval;

	float len= sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);
	norm[0]/=len;
	norm[1]/=len;
	norm[2]/=len;

	//#endif
}

float Octree::compute_error(int oc_idx,int level,float& min, float& max)
{
	int x,y,z,x_orig,y_orig,z_orig;
	float sum=0;
	int vtx[8];
	float temp1,temp2,temp3,temp4,temp5,temp6,diff;
	float orig_val,interp_val;
	int cell_size;
	float val[8];
	float x_ratio,y_ratio,z_ratio;
	min=FLOAT_MAXIMUM;
	max=FLOAT_MINIMUM;
	
	cell_size = (dim[0]-1)/(1<<level);
	octcell2xyz(oc_idx,x_orig,y_orig,z_orig,level);
	
	x_orig=x_orig*cell_size;
	y_orig=y_orig*cell_size;
	z_orig=z_orig*cell_size;
	
	idx2vtx(oc_idx,level,vtx);
	int i;
	for (i=0;i<8;i++) {
		val[i]=orig_vol[vtx[i]];
	}
	
	for (z=z_orig;z<=z_orig+cell_size;z++) {
		for (y=y_orig;y<=y_orig+cell_size;y++) {
			for (x=x_orig;x<=x_orig+cell_size;x++) {
				orig_val=orig_vol[xyz2vtx(x,y,z)];
				
				if (min>orig_val) min=orig_val;
				if (max<orig_val) max=orig_val;
				
				x_ratio=((float)(x-x_orig))/((float)cell_size);
				y_ratio=((float)(y-y_orig))/((float)cell_size);
				z_ratio=((float)(z-z_orig))/((float)cell_size);
				
				temp1 = val[0] + (val[1]-val[0])*x_ratio;
				temp2 = val[2] + (val[3]-val[2])*x_ratio;
				temp3 = val[4] + (val[5]-val[4])*x_ratio;
				temp4 = val[6] + (val[7]-val[6])*x_ratio;
				temp5 = temp1  + (temp2-temp1)*y_ratio;
				temp6 = temp3  + (temp4-temp3)*y_ratio;
				interp_val = temp5  + (temp6-temp5)*z_ratio;
				
				diff = (orig_val>interp_val) ? orig_val-interp_val : interp_val-orig_val;
				sum+=diff*diff;
				//sum+=diff;
			}
		}
	}
	
	if (level==oct_depth) return 0;
	
	//if (sum<=0.01) return 0.01;
	//return oct_depth-get_level(oc_idx);
	//printf("sum : %f\n",sum);
	return sum;
}


void Octree::getCellValues(int oc_id,int level,float* val)
{
	int vtx[8];
	idx2vtx(oc_id,level,vtx);
	int i;
	for (i=0;i<8;i++) {
		//val[i]=orig_vol[vtx[i]];
		val[i] = orig_vol[vtx[i]];
	}
}


float Octree::getValue(int i, int j, int k)
{
	/*
	int cell_size=(dim[0]-1)/(1<<level);
	return orig_vol[i*cell_size + j*cell_size*dim[0] + k*cell_size*dim[0]*dim[1]];
	*/

	return orig_vol[i + j*dim[0] + k*dim[0]*dim[1]];
}

//------------------------------------------------------------------
// compute vertex gradient
// Three method are availble now:
// 1. simple finite difference
// 2. use the limit B-spline approximation
// 3. B-spline convolution
//-------------------------------------------------------------------
void Octree::getVertGrad(int i, int j, int k, float g[3]) 
{

	if(flag_normal == 1) {	// central difference
		int lres=dim[0];
		if (i==0) {
			g[0] = getValue(i+1, j, k) - getValue(i, j, k);
		}
		else if (i>=lres-1) {
			g[0] = getValue(i, j, k) - getValue(i-1, j, k);
		}
		else {
			g[0] = (getValue(i+1, j, k) - getValue(i-1, j, k)) * 0.5;
		}

		if (j==0) {
			g[1] = getValue(i, j+1, k) - getValue(i, j, k);
		}
		else if (j>=lres-1) {
			g[1] = getValue(i, j, k) - getValue(i, j-1, k);
		}
		else {
			g[1] = (getValue(i, j+1, k) - getValue(i, j-1, k)) * 0.5;
		}
		
		if (k==0) {
			g[2] = getValue(i, j, k+1) - getValue(i, j, k);
		}
		else if (k>=lres-1) {
			g[2] = getValue(i, j, k) - getValue(i, j, k-1);
		}
		else {
			g[2] = (getValue(i, j, k+1) - getValue(i, j, k-1)) * 0.5;
		}
	}
	// A.O
	//else if(flag_normal == 0) {	// Bspline Convolution
	else{ // Bspline Convolution         flag_normal == 0
		float v[27];
		int ix[3], iy[3], iz[3];
		int l, m, n, t = 0;

		ix[0] = (i-1 >= 0)? i-1:0;
		ix[1] = i;
		ix[2] = (i+1 < dim[0])? i+1:i;
		iy[0] = (j-1 >= 0)? j-1:0;
		iy[1] = j;
		iy[2] = (j+1 < dim[1])? j+1:j;
		iz[0] = (k-1 >= 0)? k-1:0;
		iz[1] = k;
		iz[2] = (k+1 < dim[2])? k+1:k;

		for (n = 0; n < 3; n++) {
			for (m = 0; m < 3; m++) {
				for (l = 0; l < 3; l++) {
					v[t] = getValue(ix[l], iy[m], iz[n]);
					t++;
				}
			}
		}

		g[0] = g[1] = g[2] = 0;
		for (l = 0; l < 27; l++) {
			g[0] += x_grad_mask[l]*v[l];
			g[1] += y_grad_mask[l]*v[l];
			g[2] += z_grad_mask[l]*v[l];
		}

		g[0] /= span[0];
		g[1] /= span[1];
		g[2] /= span[2];
	}
	// A.O
	/****
	else {	// Bspline Interpolation
		GradientAtPoint(BSplineCoeff, (float)i, (float)j, (float)k, dim[0], dim[1], dim[2], g);
		g[0]=g[0]/span[0];
		g[1]=g[1]/span[1];
		g[2]=g[2]/span[2];
	}
	***/

}


int Octree::is_skipcell(int oc_id)
{
	if ((minmax[oc_id].max > iso_val) && (minmax[oc_id].min < iso_val)) 
		return 0;
	else return 1;
}

int Octree::is_skipcell_in(int oc_id)
{
	if ((minmax[oc_id].max > iso_val_in) && (minmax[oc_id].min < iso_val_in)) 
		return 0;
	else return 1;
}

int Octree::is_skipcell_interval(int oc_id)
{
	if ((minmax[oc_id].max > iso_val && minmax[oc_id].min < iso_val) ||
		(minmax[oc_id].max > iso_val_in && minmax[oc_id].min < iso_val_in)) 
		return 0;
	else return 1;
}

