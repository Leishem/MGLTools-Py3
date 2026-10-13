/*

#include <stdio.h>
#include "octree.h"

void Octree::interp_bit_generate()
{
	
	int i_bit;
	int case_num;
	int i;
	

	case_num=(1<<18);
	interpol_bit=(int*)malloc(sizeof(int)*case_num);
	//fprintf(fp,"int interpol_bit[%d] = {\n",case_num);
	for (i=0;i<case_num;i++) {
		i_bit=0;
		if (!( ((1<<0)&i) &&
		     ((1<<2)&i) &&
		     ((1<<6)&i) )) {
			i_bit|=(1<<0);
		}
		if (!( ((1<<1)&i) &&
		     ((1<<2)&i) &&
		     ((1<<8)&i) )) {
			i_bit|=(1<<1);
		}
		if (!( ((1<<2)&i))) {
			i_bit|=(1<<2);
		}
		if (!( ((1<<2)&i) &&
		     ((1<<3)&i) &&
		     ((1<<9)&i) )) {
			i_bit|=(1<<3);
		}
		if (!( ((1<<2)&i) &&
		     ((1<<4)&i) &&
		     ((1<<11)&i) )) {
			i_bit|=(1<<4);
		}
		if (!( ((1<<5)&i) &&
		     ((1<<6)&i) &&
		     ((1<<8)&i) )) {
			i_bit|=(1<<5);
		}
		if (!( ((1<<6)&i))) {
			i_bit|=(1<<6);
		}
		if (!( ((1<<6)&i) &&
		     ((1<<7)&i) &&
		     ((1<<9)&i) )) {
			i_bit|=(1<<7);
		}
		if (!( ((1<<8)&i))) {
			i_bit|=(1<<8);
		}
		if (!( ((1<<9)&i))) {
			i_bit|=(1<<9);
		}
		if (!( ((1<<8)&i) &&
		     ((1<<10)&i) &&
		     ((1<<11)&i) )) {
			i_bit|=(1<<10);
		}
		if (!( ((1<<11)&i))) {
			i_bit|=(1<<11);
		}
		if (!( ((1<<9)&i) &&
		     ((1<<11)&i) &&
		     ((1<<12)&i) )) {
			i_bit|=(1<<12);
		}
		if (!( ((1<<6)&i) &&
		     ((1<<13)&i) &&
		     ((1<<15)&i) )) {
			i_bit|=(1<<13);
		}
		if (!( ((1<<8)&i) &&
		     ((1<<14)&i) &&
		     ((1<<15)&i) )) {
			i_bit|=(1<<14);
		}
		if (!( ((1<<15)&i))) {
			i_bit|=(1<<15);
		}
		if (!( ((1<<9)&i) &&
		     ((1<<15)&i) &&
		     ((1<<16)&i) )) {
			i_bit|=(1<<16);
		}
		if (!( ((1<<11)&i) &&
		     ((1<<15)&i) &&
		     ((1<<17)&i) )) {
			i_bit|=(1<<17);
		}
		// change start
		//if (i!=case_num-1) fprintf(fp,"\t interpol_bit[%d]=%d;\n",i,interpol_bit);
		//else fprintf(fp,"\t interpol_bit[%d]=%d;\n",i,interpol_bit);
		interpol_bit[i]=i_bit;

		// change end
	}
	//fprintf(fp,"};\n");
	
}

*/