#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "bhtree.h"

/************************************************************************
   original BHTree (fixed size, fixed points locations)

************************************************************************/


static int findBHcloseAtomsInNode(BHnode *node,float *x,float cutoff,
				  int *atom,int maxn);
static int findBHcloseAtomsInNodedist(BHnode *node,float *x,float cutoff,
				      int *atom,float *dist,int maxn);

/*-------------------- generateBHtree --------------------*/

BHtree *generateBHtree(BHpoint **atoms,int nbat,int granularity)
{
  /* 3D space-sort atoms into Barnes-Hut tree with given granularity */
  /* (max number of atoms per leaf-node) */

  BHtree *r;
  BHpoint **p;
  int i,k;

  /* allocate tree data structure */

  r=(BHtree *)malloc(sizeof(BHtree));
  if (r==NULL) return(r);
  r->atom=(BHpoint **)NULL;
  r->bfl=0;
  r->rm=0.0;
  for (i=0;i<nbat;i++)
    if (r->rm<atoms[i]->r) r->rm = atoms[i]->r;
  r->rm += 0.1;
#ifdef STATBHTREE
  r->tot=0;    /* total number of neighbors returned by findBHclose */
  r->max=0; r->min=9999999; /* min and max of these numbers */
  r->nbr=0;     /* number of calls to findBHclose */
#endif

  /* allocate root node data structure */

  r->root=(BHnode *)malloc(sizeof(BHnode));
  if (r->root==NULL) {
    freeBHtree(r);
    return((BHtree *)NULL);
  }

  /* initialize root node data structure */

  r->root->atom=(BHpoint **)NULL;
  r->root->n=0;
  r->root->dim= -1;
  r->root->left=(BHnode *)NULL;
  r->root->right=(BHnode *)NULL;

  /* count atoms in chain */

  if (nbat==0) {
    freeBHtree(r);
    return((BHtree *)NULL);
  }
    
  /* allocate atom pointer array */

  r->atom=atoms;
  if (r->atom == NULL) { 
    freeBHtree(r);
    return((BHtree *)NULL);
  }
  r->root->atom=r->atom;

  /* fill atom pointer array */

  p=r->root->atom;
  r->root->n=nbat;

  /* determine box dimension */

  p=r->root->atom;

  for (k=0;k<3;k++) {
    r->xmin[k]=p[0]->x[k];
    r->xmax[k]=r->xmin[k];
  }

  for (i=1;i<r->root->n;i++) {
    for (k=0;k<3;k++) {
      if (r->xmin[k] > p[i]->x[k]) r->xmin[k] = p[i]->x[k];
      if (r->xmax[k] < p[i]->x[k]) r->xmax[k] = p[i]->x[k];
    }
  }

  /* start recursive 3D space sort */

  divideBHnode(r->root,r->xmin,r->xmax,granularity);

  /* build a lookup table that restores the order in which the points
     where given to build the bhtree */
  r->nodeLookUp = (int *)malloc(r->root->n*sizeof(int));
  if (!r->nodeLookUp) {
    fprintf(stderr, "Error: failed to malloc lookup table" );
    return NULL;
  }
  for (i=0; i<r->root->n; i++)
    r->nodeLookUp[atoms[i]->at] = i;

  /* done... */
  return(r);
}
/*-------------------- find_BHnode --------------------*/

BHnode *findBHnode(BHtree *tree,float *x)
{
  /* find leaf in BH tree that contains 3D point x */

  BHnode *r;
  int k;

  if (tree==NULL) return((BHnode *)NULL);

  /* first check if point is in tree */

  for (k=0;k<3;k++) {
    if (x[k]<tree->xmin[k]) return((BHnode *)NULL);
    if (x[k]>tree->xmax[k]) return((BHnode *)NULL);
  }

  /* if point is in tree, locate the leaf */

  r=tree->root;
  while (r!=NULL) {
    if (r->dim<0) break;
    if (x[r->dim]<r->cut) r=r->left;
    else r=r->right;
  }

  return(r);
}
/*-------------------- freeBHtree --------------------*/

void freeBHtree(BHtree *tree)
{
  int i;

  if (tree->atom!=NULL) {
    for (i=0;i<tree->root->n;i++) free(tree->atom[i]);
    free(tree->atom);
  }
  free(tree->nodeLookUp);
  freeBHnode(tree->root);
  free(tree);
}
/*-------------------- freeBHnode --------------------*/

void freeBHnode(BHnode *node)
{
  if (node!=NULL) {
    freeBHnode(node->left);
    freeBHnode(node->right);
    free(node);
  }
}
/*-------------------- divideBHtree --------------------*/

void divideBHnode(BHnode *node,float *xmin,float *xmax,int granularity)
{
  float cut,dx,xminl[3],xmaxl[3],xminr[3],xmaxr[3];
  int dim,i,j,k,n[NSTEPS],lm,rm;
  BHpoint *a;

  /* if nothing left to divide just return */

  if (node==NULL) return;
  if (granularity<1 || node->n <= granularity) return;
  if (node->atom==NULL) return;

  /* determine dimension along which to cut */

  dim=0;
  if (xmax[1]-xmin[1] > xmax[dim]-xmin[dim]) dim=1;
  if (xmax[2]-xmin[2] > xmax[dim]-xmin[dim]) dim=2;

  /* determine position of cutting plane */

  dx=(xmax[dim]-xmin[dim])/NSTEPS;
  if (dx<0.0001) return;
  for (i=0;i<NSTEPS;i++) n[i]=0;
  for (j=0;j<node->n;j++) {
    i=(node->atom[j]->x[dim]-xmin[dim])/dx;
    if (i>=0 && i<NSTEPS) n[i]++;
  }
  for (i=1;i<NSTEPS;i++) {
    n[i]+=n[i-1];
    if (n[i]>node->n/2) break;
  }
  cut=xmin[dim]+i*dx;
  if (n[i]>=node->n) return;

  /* create left/right descendants */

  node->left=(BHnode *)malloc(sizeof(BHnode));
  if (node->left==NULL) return;
  node->left->dim= -1;
  node->left->left=(BHnode *)NULL;
  node->left->right=(BHnode *)NULL;
        
  node->right=(BHnode *)malloc(sizeof(BHnode));
  if (node->right==NULL) {
    freeBHnode(node->left);
    return;
  }
  node->right->dim= -1;
  node->right->left=(BHnode *)NULL;
  node->right->right=(BHnode *)NULL;

  node->cut=cut;
  node->dim=dim;

  /* sort atoms into left/right descendants */

  lm=0;
  rm=node->n-1;

  while (lm<rm) {
    for(;lm<node->n;lm++) if (node->atom[lm]->x[dim]>=cut) break;
    for(;rm>=0;rm--)      if (node->atom[rm]->x[dim]<cut) break;
    if (lm<rm) {
      a=node->atom[rm];
      node->atom[rm]=node->atom[lm];
      node->atom[lm]=a;
      rm--;
      lm++;
    }
  }

  if (lm==rm) {
    if (node->atom[rm]->x[dim]<cut) lm++;
    else rm--;
  }

  node->left->n=rm+1;
  node->left->atom=node->atom;

  node->right->n=node->n-(rm+1);
  node->right->atom=node->atom+lm;

  /* if descendants are coarse, cut them up... */

  if (node->left->n > granularity) {
    for (k=0;k<3;k++) {
      xminl[k]=xmin[k];
      xmaxl[k]=xmax[k];
    }
    xmaxl[dim]=cut;
    divideBHnode(node->left,xminl,xmaxl,granularity);
  }

  if (node->right->n > granularity) {
    for (k=0;k<3;k++) {
      xminr[k]=xmin[k];
      xmaxr[k]=xmax[k];
    }
    xminr[dim]=cut;
    divideBHnode(node->right,xminr,xmaxr,granularity);
  }

  /* done... */

  return;

}
/*-------------------- findBHcloseAtoms --------------------*/

int findBHcloseAtomsdist(BHtree *tree, float *x, float cutoff,
			 int *atom, float *dist, int maxn)
{
  int i;

  if (maxn<1 || tree==NULL || cutoff <=0.0) return(0);
  if (tree->root==NULL) return(0);

  for (i=0;i<3;i++) {
    if (x[i] < tree->xmin[i] - cutoff) break;
    if (x[i] > tree->xmax[i] + cutoff) break;
  }
  if (i<3) return(0);

  return(findBHcloseAtomsInNodedist(tree->root,x,cutoff,atom,dist,maxn));
}
/*-------------------- findBHcloseAtomsInNode --------------------*/

static int findBHcloseAtomsInNodedist(BHnode *node,float *x,float cutoff,
				  int *atom,float *dist,int maxn)
{
  int j,n;
  float d[3],D,C;

  if (node==NULL) return(0);
  if (maxn<=0) return(0);
  if (node->n < 1) return(0);
  
  if (node->dim<0) {
    n=0;
    C=cutoff*cutoff;
    for (j=0;j<node->n;j++) {
      d[0]=x[0]-node->atom[j]->x[0];
      if (d[0]>cutoff || d[0]< -cutoff) continue;
      d[1]=x[1]-node->atom[j]->x[1];
      if (d[1]>cutoff || d[1]< -cutoff) continue;
      d[2]=x[2]-node->atom[j]->x[2];
      if (d[2]>cutoff || d[2]< -cutoff) continue;
      D=d[0]*d[0]+d[1]*d[1]+d[2]*d[2];
      if (D>C) continue;
      if (n<maxn) {
	atom[n]=node->atom[j]->at;
	dist[n]=sqrt(D);
	n++;
      }else{
	n++;
	break;
      }
    }
  }else{
    n=0;
    if (x[node->dim]<node->cut+cutoff) {
      n+=findBHcloseAtomsInNodedist(node->left,x,cutoff,atom,dist,maxn);
    }
    if (x[node->dim]>=node->cut-cutoff) {
      n+=findBHcloseAtomsInNodedist(node->right,x,cutoff,atom+n,dist+n,maxn-n);
    }
  }
  return(n);
}

/*-------------------- findBHcloseAtomsInNodedist2 --------------------*/

static int findBHcloseAtomsInNodedist2(BHnode *node, float *x, float cutoff,
				       int *atom, float *dist, int maxn)
{
  int j,n;
  float d[3],D,C;

  if (node==NULL) return(0);
  if (maxn<=0) return(0);
  if (node->n < 1) return(0);
  
  if (node->dim<0) {
    n=0;
    C=cutoff*cutoff;
    for (j=0;j<node->n;j++) {
      d[0]=x[0]-node->atom[j]->x[0];
      if (d[0]>cutoff || d[0]< -cutoff) continue;
      d[1]=x[1]-node->atom[j]->x[1];
      if (d[1]>cutoff || d[1]< -cutoff) continue;
      d[2]=x[2]-node->atom[j]->x[2];
      if (d[2]>cutoff || d[2]< -cutoff) continue;
      D=d[0]*d[0]+d[1]*d[1]+d[2]*d[2];
      if (D>C) continue;
      if (n<maxn) {
	atom[n]=node->atom[j]->at;
	dist[n]=D;
	n++;
      }else{
	n++;
	break;
      }
    }
  }else{
    n=0;
    if (x[node->dim]<node->cut+cutoff) {
      n+=findBHcloseAtomsInNodedist2(node->left,x,cutoff,atom,dist,maxn);
    }
    if (x[node->dim]>=node->cut-cutoff) {
      n+=findBHcloseAtomsInNodedist2(node->right,x,cutoff,atom+n,dist+n,maxn-n);
    }
  }
  return(n);
}

/*-------------------- findBHcloseAtomsdist2 --------------------*/

int findBHcloseAtomsdist2(BHtree *tree, float *x, float cutoff,
			  int *atom, float *dist, int maxn)
{
  int i;

  if (maxn<1 || tree==NULL || cutoff <=0.0) return(0);
  if (tree->root==NULL) return(0);

  for (i=0;i<3;i++) {
    if (x[i] < tree->xmin[i] - cutoff) break;
    if (x[i] > tree->xmax[i] + cutoff) break;
  }
  if (i<3) return(0);

  return(findBHcloseAtomsInNodedist2(tree->root, x, cutoff,
				     atom, dist, maxn));
}

/*-------------------- findBHcloseAtoms --------------------*/

int findBHcloseAtoms(BHtree *tree,float *x,float cutoff,
		     int *atom,int maxn)
{
  int i,n;

  if (maxn<1 || tree==NULL || cutoff <=0.0) return(0);
  if (tree->root==NULL) return(0);

  for (i=0;i<3;i++) {
    if (x[i] < tree->xmin[i] - cutoff) break;
    if (x[i] > tree->xmax[i] + cutoff) break;
  }
  if (i<3) return(0);

#ifdef STATBHTREE

  n = findBHcloseAtomsInNode(tree->root,x,cutoff,atom,maxn);
  tree->nbr++;
  tree->tot += n;
  if (n>tree->max) tree->max=n;
  if (n<tree->min) tree->min=n;
  return(n);

#else
  return(findBHcloseAtomsInNode(tree->root,x,cutoff,atom,maxn));
#endif
}
/*-------------------- findBHcloseAtomsInNode --------------------*/

static int findBHcloseAtomsInNode(BHnode *node,float *x,float cutoff,
				  int *atom, int maxn)
{
  int j,n;
  float C,D;
  double d1,d2,d3;
  BHpoint *p;
/*
  if (node==NULL) return(0);
  if (maxn<=0) return(0);
  if (node->n < 1) return(0);
*/  
  if (node->dim<0) {

    C=cutoff*cutoff;

    for (n=j=0;j<node->n;j++) {
      p = node->atom[j];
      d1 = x[0] - p->x[0];
      if (d1 > cutoff || d1 < -cutoff) continue;
      d2 = x[1] - p->x[1];
      if (d2 > cutoff || d2 < -cutoff) continue;
      d3 = x[2] - p->x[2];
      if (d3 > cutoff || d3 < -cutoff) continue;

      D=d1*d1 + d2*d2 + d3*d3;

      if (D>C) continue;
      if (n<maxn) {
	atom[n] = p->at;
	n++;
      }else{
	printf("ERROR: findBHcloseAtomsInNode: result array too small\n");
	break;
      }
    }
  } else {
    n=0;
    if (x[node->dim]<node->cut+cutoff) {
      n+=findBHcloseAtomsInNode(node->left,x,cutoff,atom,maxn);
    }
    if (x[node->dim]>=node->cut-cutoff) {
      n+=findBHcloseAtomsInNode(node->right,x,cutoff,atom+n,maxn-n);
    }
  }
  return(n);
}


int *findClosePairsInTree(BHtree *bht, float tolerance)
{
  BHpoint *bhpp1, *bhpp2;
  BHpoint **atoms;
  int *bondsp, *newbonds;
  int maxNbBonds=20000, newmaxNbBonds, nbb, nba;
  int cl[200], id, i, j, k;
  float cld[200], cut, r, lim2, dist2;

  bondsp = (int *)malloc((1+maxNbBonds)*sizeof(int));
  if (bondsp==NULL) return NULL;
  atoms = bht->root->atom;

  /* loops over all points */
  for (i=0, nbb=1; i<bht->root->n; i++) {
    bhpp1 = atoms[i];
    r = bhpp1->r;
    id = bhpp1->at;

    /* find all atoms within max cutoff of point */
    cut = (r + bht->maxr)*tolerance;
    nba = findBHcloseAtomsdist2(bht, bhpp1->x, cut, cl, cld, 200);

    /*
    if (id==186) {
      printf("%d %d %f %f %f %f\n", id, nba, bhpp1->x[0], bhpp1->x[1], bhpp1->x[2], bhpp1->r);
      }
    */

    /* select the points within strict cutoff */
    for (j=0; j<nba; j++) {
      k = cl[j];
/*
      if (id==186) {
	printf("  k=%d, dist=%f\n", k, cld[j]);
      }

printf("%d %f %f %f %f\n", k, bhpp2->x[0], bhpp2->x[1], bhpp2->x[2], bhpp2->r);
printf("%f %f\n", lim2*lim2, dist2);
      */
      if (k<=id) continue;
      bhpp2 = atoms[bht->nodeLookUp[k]];
      lim2 = (r + bhpp2->r)*tolerance;
      dist2 = cld[j];

      if (dist2 < lim2*lim2) { /* atom k is bonded to atom id */

	bondsp[nbb] = id;
	bondsp[nbb+1] = k;
	nbb+=2;
	if (nbb>maxNbBonds-1) {
	  newmaxNbBonds = maxNbBonds + 20000;
	  newbonds = (int *)malloc((1+newmaxNbBonds)*sizeof(int));
	  if (newbonds==NULL) {
	    free(bondsp);
	    return NULL;
	  }
	  memcpy(newbonds, bondsp, (1+maxNbBonds)*sizeof(int));
	  maxNbBonds = newmaxNbBonds;
	  free(bondsp);
	  bondsp = newbonds;
	}
      }
    }
  }
  bondsp[0] = nbb;
  return bondsp;
}

/* Retrieve pairs of points (pi,pj) such that their distance is less
   than tolerance*(sum of radii) and pi is a point in the tree and pj
   is a point in the set provided to the function. */

int *findClosePairs( BHtree *bht, float xyz[1][3], int *nbxyz, float rad[1], 
		     int nbrad, float tolerance )
{
  BHpoint *bhpp;
  BHpoint **atoms;
  int *bondsp, *newbonds;
  int maxNbBonds=20000, newmaxNbBonds, nbb, nba;
  int cl[200], i, j, k;
  float cld[200], maxr, r, cut, lim2, dist2;

  bondsp = (int *)malloc((1+maxNbBonds)*sizeof(int));
  if (bondsp==NULL) return NULL;
  atoms = bht->root->atom;

  /* find largest radius */
  maxr = bht->maxr;
  for (i=0; i<nbxyz[0]; i++)
    if (rad[i] >maxr) maxr = rad[i];

  /* loops over all given points */
  for (i=0, nbb=1; i<nbxyz[0]; i++) {
    r = rad[i];

    /* find all atoms within max cutoff of point */
    cut = (r + maxr)*tolerance;
    nba = findBHcloseAtomsdist2(bht, xyz[i], cut, cl, cld, 200);

    /* select the points within strict cutoff */
    for (j=0; j<nba; j++) {

      k = cl[j];
      bhpp = atoms[bht->nodeLookUp[k]];
      lim2 = (r + bhpp->r)*tolerance;
      dist2 = cld[j];

      if (dist2 < lim2*lim2) { /* atom k is close to atom i */

	bondsp[nbb] = i;
	bondsp[nbb+1] = k;
	nbb+=2;
	if (nbb>maxNbBonds-1) {
	  newmaxNbBonds = maxNbBonds + 20000;
	  newbonds = (int *)malloc((1+newmaxNbBonds)*sizeof(int));
	  if (newbonds==NULL) {
	    free(bondsp);
	    return NULL;
	  }
	  memcpy(newbonds, bondsp, (1+maxNbBonds)*sizeof(int));
	  maxNbBonds = newmaxNbBonds;
	  bondsp = newbonds;
	}
      }
    }
  }
  bondsp[0] = nbb;
  return bondsp;
}
/* For every point in a given set of 3D points - xyz[1][3]-
   find a closest atom within a given cutoff. An array of
   corresponding atom indices is returned,
   if not close point is found Null is returned if returnNullIfFail is true
   else -1 is put into the index list
*/
int *findClosestAtoms( BHtree *bht, float xyz[1][3], int *nbxyz, 
		       float cut, int returnNullIfFail )
{
  int *cl_inds;
  int cl[2000], closest, i, j, k, nba;
  float cld[2000], lim;

  cl_inds = (int *)malloc((nbxyz[0]+1)*sizeof(int));
  if (cl_inds==NULL) {
    printf("Failed to allocate array of %d integers \n", nbxyz[0]);
    return NULL;
  }
  cl_inds[0] = nbxyz[0];
  /* loops over all given points */
  for (i=1; i<nbxyz[0]+1; i++) {
    /* FIXME cl and cld should be allocated dynamically and resized if
       nba becomes larger, else we silently put a wrong value into cl_inds */
    /* find all atoms within cutoff of a point */
    nba = findBHcloseAtomsdist2(bht, xyz[i-1], cut, cl, cld, 2000);

    /* find the closest atom */
    lim = 9999999.;
    closest = -1;
    for (j=0; j<nba; j++) {
      k = cl[j];
      if (cld[j] < lim){
	lim = cld[j];
	closest = k;
      }
    }
    if (closest >= 0){
      if (closest > bht->root->n) {
	printf("ERROR %d %d %d %f %f %f %f\n", i,closest, nba, cut, xyz[i-1][0], xyz[i-1][1],
	       xyz[i-1][2]);
      }
      cl_inds[i]=closest; 
      /*cl_inds[i] = bht->nodeLookUp[closest];*/
      /*printf ("i=%d, closest =%d, nba = %d \n", i-1, cl_inds[i], nba); */
    }
    else if (returnNullIfFail) {
      free(cl_inds);
      printf ("No atoms found for point %d. Cutoff: %f \n", i-1, cut);
      return NULL;
    } else {
      cl_inds[i]=-1;
    }
  }
  
  return cl_inds;
}


/************************************************************************
   mutable bhtrees: (Chris Carrillo)

   TBHtree: points can be translated
   RTBHtree: points can be translated, added and deleted

************************************************************************/

static int RegenerateRBHTree(RBHTree *tree);

TBHTree *GenerateTBHTree(TBHPoint *Pts,
			   int NumPts,
			   int Granularity,
			   int LeafPadding,
			   float SpacePadding)
{
  TBHTree *r;
  int i, j;
  float xmin[3], xmax[3], sxmin[3], sxmax[3];
  
  r = (TBHTree *)malloc((size_t)sizeof(TBHTree));
  if(r == NULL) return(r);
  
  r->bfl = 0;

  r->rm=0.0;
  for (i=0;i<NumPts;i++)
    if (r->rm<Pts[i].Rad) r->rm = Pts[i].Rad;
  r->rm += 0.1;

#ifdef STATBHTREE
  r->tot = 0;
  r->max = 0;
  r->min = 9999999;
  r->nbr = 0;
#endif
  
  r->Root=(TBHNode *)malloc((size_t)sizeof(TBHNode));
  if (r->Root==NULL) {
    FreeTBHTree(r);
    return((TBHTree *)NULL);
  }
  
  r->Root->Index.NumPts = 0;
  r->Root->Index.Pts = (TBHPoint **)NULL;
  r->Root->Index.Size = 0;
  r->Root->Parent = (TBHNode *)NULL;
  r->Root->dim = -1;
  r->Root->Left = (TBHNode *)NULL;
  r->Root->Right = (TBHNode *)NULL;
  
  if(NumPts == 0)
    {
      FreeTBHTree(r);
      return((TBHTree *)NULL);
    }
  
  for(j = BH_X; j <= BH_Z; j++)
    {
      xmin[j] = Pts[0].Pos[j];
      xmax[j] = xmin[j];
    }
  
  for(i = 1; i < NumPts; i++)
    {
      for(j = BH_X; j <= BH_Z; j++)
	{
	  if (xmin[j] > Pts[i].Pos[j]) xmin[j] = Pts[i].Pos[j];
	  if (xmax[j] < Pts[i].Pos[j]) xmax[j] = Pts[i].Pos[j];
	}
    }
  
  r->Pts = Pts;
  r->NumPts = NumPts;
  r->Root->Index.Pts = (TBHPoint **)NULL;
  r->Root->Index.NumPts = NumPts;
  for(j = BH_X; j <= BH_Z; j++)
    {
      sxmin[j] = xmin[j] - SpacePadding;
      sxmax[j] = xmax[j] + SpacePadding;
      r->xmin[j] = sxmin[j];
      r->xmax[j] = sxmax[j];
    }

  r->Root->Buffer = (TBHPoint **)malloc(sizeof(TBHPoint *) * NumPts);
  if(r->Root->Buffer == NULL) return (TBHTree *)NULL;
  for(i = 0; i < NumPts; i++)
    {
      Pts[i].ID = i;
      r->Root->Buffer[i] = &Pts[i];
    }
  DivideTBHNode(r->Root, xmin, xmax, sxmin, sxmax,
		Granularity, LeafPadding);
  if(r->Root->dim == -1 && r->Root->Index.Size == 0)
    {
      for(i = BH_X; i <= BH_Z; i++)
	{
	  r->Root->xmin[i] = sxmin[i];
	  r->Root->xmax[i] = sxmax[i];
	}

      r->Root->Index.Size = r->Root->Index.NumPts + LeafPadding;
      r->Root->Index.Pts = (TBHPoint **)malloc(sizeof(TBHPoint *) *
					       r->Root->Index.Size);
      for(i = 0; i < r->Root->Index.NumPts; i++)
	{
	  r->Root->Index.Pts[i] = r->Root->Buffer[i];
	  r->Root->Index.Pts[i]->Box = r->Root;
	}
    }
 
  return(r);
}

TBHNode *FindTBHNode(TBHTree *tree, float *x)
{
  TBHNode *r;
  int k;

  if (tree == NULL) return((TBHNode *)NULL);
  
  for(k = 0; k < 3; k++)
    {
      if(x[k] < tree->xmin[k]) return((TBHNode *)NULL);
      if(x[k] > tree->xmax[k]) return((TBHNode *)NULL);
    }
  
  r = tree->Root;
  while(r != NULL)
    {
      if(r->dim < 0) break;
      if(x[r->dim] < r->cut) r = r->Left;
      else r = r->Right;
    }
  
  return(r);
}

TBHNode *FindTBHNodeUp(TBHNode *node, float x[3])
{
  TBHNode *r;
  int i, j, k;

  r = node;

  i = BH_NO;
  while(i == BH_NO)
    {
      r = r->Parent;
      if(r == NULL) return ((TBHNode *)NULL);

      k = BH_YES;
      for(j = BH_X; j <= BH_Z && k == BH_YES; j++)
	if(x[j] > r->xmax[j] || x[j] < r->xmin[j]) k = BH_NO;
      if(k == BH_YES) i = BH_YES;
    }

  while(r != NULL)
    {
      if(r->dim < 0) break;
      if(x[r->dim] < r->cut) r = r->Left;
      else r = r->Right;
    }
  
  return(r);
}

void FreeTBHTree(TBHTree *tree)
{
  if(tree->Pts != NULL)
    free((void *)(tree->Pts));

  free((void *)tree->Root->Buffer);
  FreeTBHNode(tree->Root);

  free((void *)tree);
}

void FreeTBHNode(TBHNode *node)
{
  if(node != NULL)
    {
      if(node->Left != NULL)
	FreeTBHNode(node->Left);
      if(node->Right != NULL)
	FreeTBHNode(node->Right);
      
      if(node->Index.Size > 0)
	free((void *)(node->Index.Pts));

      free((void *)(node));
    }
}

int MoveTBHPoint(TBHTree *tree,
		   int ID,
		   float NewPos[3],
		   int FindDirection)
{
  int i, j;
  TBHNode *OrigBox;
  TBHNode *NewBox;
  TBHPoint *Pt;

  if(ID >= tree->NumPts || ID < 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveBHPoint: Invalid point number.\n");
#endif
      return BH_INVALID_POINT;
    }

  OrigBox = tree->Pts[ID].Box;
  if(OrigBox == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveBHPoint: NULL box pointer.\n");
#endif
      return BH_ALREADY_DELETED;
    }

  j = BH_YES;
  for(i = BH_X; i <= BH_Z && j == BH_YES; i++)
    {
      if(NewPos[i] > OrigBox->xmax[i] ||
	 NewPos[i] < OrigBox->xmin[i]) j = BH_NO;
    }
  if(j == BH_YES) 
    {
      tree->Pts[ID].Pos[BH_X] = NewPos[BH_X];
      tree->Pts[ID].Pos[BH_Y] = NewPos[BH_Y];
      tree->Pts[ID].Pos[BH_Z] = NewPos[BH_Z];
      return BH_YES;
    }
  if(OrigBox->Index.NumPts == 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveBHPoint: Empty box.\n");
#endif
      return BH_EMPTY_BOX;
    }

  tree->Pts[ID].Pos[BH_X] = NewPos[BH_X];
  tree->Pts[ID].Pos[BH_Y] = NewPos[BH_Y];
  tree->Pts[ID].Pos[BH_Z] = NewPos[BH_Z];

  if(FindDirection == BH_SEARCH_UP)
    NewBox = FindTBHNodeUp(OrigBox, NewPos); 
  else
    NewBox = FindTBHNode(tree, NewPos);

  if(NewBox == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveBHPoint: Point outside the tree.\n");
#endif
      return BH_OUTSIDE_TREE;    /* Regenerate the tree */
    }

  Pt = &tree->Pts[ID];
  for(i = 0; i < OrigBox->Index.NumPts; i++)
    {
      if(OrigBox->Index.Pts[i] == Pt)
	break;
    }
  if (i == OrigBox->Index.NumPts)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "ERROR: DeleteTBHPoint: Unable to find point\n\
   in leaf array.\n");
#endif
      return BH_INVALID_POINT;
    }

  for(; i < OrigBox->Index.NumPts - 1; i++)
    OrigBox->Index.Pts[i] = OrigBox->Index.Pts[i + 1];
  OrigBox->Index.NumPts--;

  if(NewBox->Index.NumPts == NewBox->Index.Size)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveTBHPoint: Leaf padding filled.\n");
#endif
      return BH_FILLED_PADDING;
    }

  tree->Pts[ID].Box = NewBox;
  NewBox->Index.Pts[NewBox->Index.NumPts] = &tree->Pts[ID];
  NewBox->Index.NumPts++;

  return BH_YES;
}

void DivideTBHNode(TBHNode *node,
		     float *xmin,
		     float *xmax,
		     float *sxmin,
		     float *sxmax,
		     int granularity,
		     int LeafPadding)
{
  float cut, dx, xminl[3], xmaxl[3], xminr[3], xmaxr[3];
  float sxminl[3], sxmaxl[3], sxminr[3], sxmaxr[3];
  int dim, i, j, k, n[NSTEPS], lm, rm;
  TBHPoint *a;

  if(node == NULL) return;
  if(granularity < 1 || node->Index.NumPts <= granularity) return;
  if(node->Buffer == NULL) return;
  
  for(i = BH_X; i <= BH_Z; i++)
    {
      node->xmin[i] = sxmin[i];
      node->xmax[i] = sxmax[i];
    }

  dim = BH_X;
  if (xmax[BH_Y] - xmin[BH_Y] > xmax[dim] - xmin[dim]) dim = BH_Y;
  if (xmax[BH_Z] - xmin[BH_Z] > xmax[dim] - xmin[dim]) dim = BH_Z;
  
  dx = (xmax[dim] - xmin[dim]) / NSTEPS;
  if(dx < 0.0001) return;
  for(i = 0; i < NSTEPS; i++) n[i]=0;
  for(j = 0; j < node->Index.NumPts; j++)
    {
      i = (node->Buffer[j]->Pos[dim] - xmin[dim]) / dx;
      if(i >= 0 && i < NSTEPS) n[i]++;
    }
  for(i = 1; i < NSTEPS; i++)
    {
      n[i] += n[i-1];
      if(n[i] > node->Index.NumPts / 2) break;
    }
  cut = xmin[dim] + i * dx;
  if (n[i] >= node->Index.NumPts) return;
  

  node->Left = (TBHNode *)malloc((size_t)sizeof(TBHNode));
  if(node->Left == NULL) return;
  
  node->Left->dim = -1;
  node->Left->Index.NumPts = 0;
  node->Left->Index.Size = 0;
  node->Left->Index.Pts = (TBHPoint **)NULL;
  node->Left->Parent = node;
  node->Left->Left = (TBHNode *)NULL;
  node->Left->Right = (TBHNode *)NULL;
  
  node->Right = (TBHNode *)malloc((size_t)sizeof(TBHNode));
  if(node->Right == NULL)
    {
      FreeTBHNode(node->Left);
      return;
    }
  
  node->Right->dim = -1;
  node->Right->Index.NumPts = 0;
  node->Right->Index.Size = 0;
  node->Right->Index.Pts = (TBHPoint **)NULL;
  node->Right->Parent = node;
  node->Right->Left = (TBHNode *)NULL;
  node->Right->Right = (TBHNode *)NULL;
  
  node->cut = cut;
  node->dim = dim;
  
  lm = 0;
  rm = node->Index.NumPts - 1;
  while(lm < rm)
    {
      for(;lm < node->Index.NumPts; lm++)
	if(node->Buffer[lm]->Pos[dim] >= cut) break;
      for(;rm >= 0; rm--)
	if(node->Buffer[rm]->Pos[dim] < cut) break;
      if(lm < rm)
	{
	  a = node->Buffer[rm];
	  node->Buffer[rm] = node->Buffer[lm];
	  node->Buffer[lm] = a;
	  rm--;
	  lm++;
	}
    }
  
  if(lm == rm)
    {
      if(node->Buffer[rm]->Pos[dim] < cut)
	lm++;
      else rm--;
    }
  
  node->Left->Index.NumPts = rm + 1;
  node->Left->Buffer = node->Buffer;
  
  node->Right->Index.NumPts = node->Index.NumPts - (rm + 1);
  node->Right->Buffer = node->Buffer + lm;
  
  for(k = BH_X; k <= BH_Z; k++)
    {
      xminl[k] = xmin[k];
      xmaxl[k] = xmax[k];
      sxminl[k] = sxmin[k];
      sxmaxl[k] = sxmax[k];
    }
  xmaxl[dim] = cut;
  sxmaxl[dim] = cut;

  if(node->Left->Index.NumPts > granularity)
    DivideTBHNode(node->Left, xminl, xmaxl, sxminl, sxmaxl,
		    granularity, LeafPadding);

  if(node->Left->dim == -1 && node->Left->Index.Size == 0)
    {
      for(i = BH_X; i <= BH_Z; i++)
	{
	  node->Left->xmin[i] = sxminl[i];
	  node->Left->xmax[i] = sxmaxl[i];
	}

      node->Left->Index.Size = node->Left->Index.NumPts + LeafPadding;
      node->Left->Index.Pts = (TBHPoint **)malloc(sizeof(TBHPoint *) *
						  node->Left->Index.Size);
      for(i = 0; i < node->Left->Index.NumPts; i++)
	{
	  node->Left->Index.Pts[i] = node->Left->Buffer[i];
	  node->Left->Index.Pts[i]->Box = node->Left;
	}
    }
  
  for(k = BH_X; k <= BH_Z; k++)
    {
      xminr[k] = xmin[k];
      xmaxr[k] = xmax[k];
      sxminr[k] = sxmin[k];
      sxmaxr[k] = sxmax[k];
    }
  xminr[dim] = cut;
  sxminr[dim] = cut;

  if(node->Right->Index.NumPts > granularity)
    DivideTBHNode(node->Right, xminr, xmaxr, sxminr, sxmaxr,
		    granularity, LeafPadding);
  if(node->Right->dim == -1 && node->Right->Index.Size == 0)
    {
      for(i = BH_X; i <= BH_Z; i++)
	{
	  node->Right->xmin[i] = sxminr[i];
	  node->Right->xmax[i] = sxmaxr[i];
	}

      node->Right->Index.Size = node->Right->Index.NumPts + LeafPadding;
      node->Right->Index.Pts = (TBHPoint **)malloc(sizeof(TBHPoint *) *
						   node->Right->Index.Size);
      for(i = 0; i < node->Right->Index.NumPts; i++)
	{
	  node->Right->Index.Pts[i] = node->Right->Buffer[i];
	  node->Right->Index.Pts[i]->Box = node->Right;
	}
    }
  
  return;
}

int FindTBHCloseAtomsDist(TBHTree *tree, float *x, float cutoff,
			    int *atom, float *dist, int maxn)
{
  int i;
  
  if(maxn < 1 || tree == NULL || cutoff <= 0.0) return(0);
  if(tree->Root == NULL) return(0);
  
  for(i = 0; i < 3; i++)
    {
      if(x[i] < tree->xmin[i] - cutoff) break;
      if(x[i] > tree->xmax[i] + cutoff) break;
    }
  if(i < 3) return(0);
  
  return (FindTBHCloseAtomsInNodeDist(tree->Root, x, cutoff,
					atom, dist, maxn));
}

int FindTBHCloseAtomsInNodeDist(TBHNode *node, float *x, float cutoff,
				int *atom, float *dist, int maxn)
{
  int j, n;
  float d[3], D, C;
  
  if(node == NULL) return(0);
  if(maxn <= 0) return(0);
  if(node->Index.NumPts < 1) return(0);
  
  if(node->dim < 0)
    {
      n=0;
      C=cutoff*cutoff;
      for(j = 0; j < node->Index.NumPts; j++)
	{
	  d[0] = x[0] - node->Index.Pts[j]->Pos[0];
	  if(d[0] > cutoff || d[0] < -cutoff) continue;
	  
	  d[1] = x[1] - node->Index.Pts[j]->Pos[1];
	  if(d[1] > cutoff || d[1] < -cutoff) continue;

	  d[2] = x[2] - node->Index.Pts[j]->Pos[2];
	  if(d[2] > cutoff || d[2] < -cutoff) continue;
	  
	  D = d[0]*d[0] + d[1]*d[1] + d[2]*d[2];
	  if(D > C) continue;
	  
	  if(n < maxn)
	    {
	      atom[n] = node->Index.Pts[j]->ID;
	      dist[n]=D;
	      n++;
	    }
	  else
	    {
	      n++;
	      break;
	    }
	}
    }
  else
    {
      n = 0;
      if(x[node->dim] < node->cut + cutoff)
	{
	  n += FindTBHCloseAtomsInNodeDist(node->Left, x,
					     cutoff, atom, dist, maxn);
	}
      if(x[node->dim] >= node->cut - cutoff)
	{
	  n += FindTBHCloseAtomsInNodeDist(node->Right,
					     x, cutoff, atom + n,
					     dist + n, maxn - n);
	}
    }
  return(n);
}

int FindTBHCloseAtoms(TBHTree *tree, float *x, float cutoff,
		      int *atom, int maxn)
{
  int i;

  if(maxn < 1 || tree == NULL || cutoff <= 0.0) return(0);
  if(tree->Root == NULL) return(0);

  for(i = 0; i < 3; i++) 
    {
      if(x[i] < tree->xmin[i] - cutoff) break;
      if(x[i] > tree->xmax[i] + cutoff) break;
    }

  if(i < 3) return(0);

#ifdef STATBHTREE
  i = FindTBHCloseAtomsInNode(tree->Root, x, cutoff, atom, maxn);
  tree->nbr++;
  tree->tot += i;
  if(i > tree->max) tree->max = i;
  if(i < tree->min) tree->min = i;
  return(i);
#else
  return(FindTBHCloseAtomsInNode(tree->Root, x, cutoff, atom, maxn));
#endif
}

int FindTBHCloseAtomsInNode(TBHNode *node, float *x,
			      float cutoff, int *atom, int maxn)
{
  int j, n;
  float d[3], D, C;
  
  if(node == NULL) return(0);
  if(maxn <= 0) return(0);
  if(node->Index.NumPts < 1) return(0);
  
  if(node->dim < 0)
    {
      n = 0;
      C = cutoff * cutoff;
      
      for(j = 0; j < node->Index.NumPts; j++)
	{
	  d[0] = x[0] - node->Index.Pts[j]->Pos[0];
	  if(d[0] > cutoff || d[0] < -cutoff) continue;
	  
	  d[1] = x[1] - node->Index.Pts[j]->Pos[1];
	  if(d[1] > cutoff || d[1] < -cutoff) continue;
	  
	  d[2] = x[2] - node->Index.Pts[j]->Pos[2];
	  if(d[2] > cutoff || d[2] < -cutoff) continue;
	  
	  D = d[0]*d[0] + d[1]*d[1] + d[2]*d[2];
	  if(D > C) continue;
	  if(n < maxn)
	    {
	      atom[n] = node->Index.Pts[j]->ID;
	      n++;
            } 
	  else
	    {
	      n++;
	      break;
            }
        }
    } 
  else
    {
      n = 0;

      if(x[node->dim] < node->cut + cutoff)
	n += FindTBHCloseAtomsInNode(node->Left, x, cutoff, atom, maxn);

      if(x[node->dim] >= node->cut - cutoff)
	n += FindTBHCloseAtomsInNode(node->Right, x, cutoff,
				       atom + n, maxn - n);
    }

  return(n);
}

RBHTree *GenerateRBHTree(TBHPoint *Pts,
			     int NumPts,
			     int MaxPts,
			     int Granularity,
			     int LeafPadding,
			     int DeletePadding,
			     float SpacePadding,
			     int OwnsMemory)
{
  RBHTree *r;
  int i, j;
  float xmin[3], xmax[3], sxmin[3], sxmax[3];

  r = (RBHTree *)malloc((size_t)sizeof(RBHTree));
  if(r == NULL) return(r);
  
  r->Granularity = Granularity;
  r->LeafPadding = LeafPadding;
  r->SpacePadding = SpacePadding;
  r->Flags = 0;

  r->SizePts = MaxPts;
  r->FreePts.NumPts = MaxPts - NumPts;
  r->FreePts.Size = (MaxPts - NumPts) + DeletePadding;
  r->FreePts.Pts = (TBHPoint **)malloc(sizeof(TBHPoint *) * 
				       r->FreePts.Size);
  if(r->FreePts.Pts == NULL) return NULL;

  for(i = 0; i < NumPts; i++)
    Pts[i].ID = i;
  for(i = 0; i < r->FreePts.NumPts; i++)
    {
      Pts[i + NumPts].ID = i + NumPts;
      Pts[i + NumPts].Box = NULL;
      r->FreePts.Pts[i] = &Pts[MaxPts - i - 1];
    }

  r->bfl = 0;

  r->rm=0.0;
  for (i=0;i<NumPts;i++)
    if (r->rm<Pts[i].Rad) r->rm = Pts[i].Rad;
  r->rm += 0.1;

#ifdef STATBHTREE
  r->tot = 0;
  r->max = 0;
  r->min = 9999999;
  r->nbr = 0;
#endif
  
  r->Root = (TBHNode *)malloc((size_t)sizeof(TBHNode));
  if(r->Root == NULL) 
    {
      FreeRBHTree(r);
      return((RBHTree *)NULL);
    }
  
  r->Root->Index.NumPts = 0;
  r->Root->Index.Size = 0;
  r->Root->Index.Pts = (TBHPoint **)NULL;
  r->Root->Parent = (TBHNode *)NULL;
  r->Root->dim = -1;
  r->Root->Left = (TBHNode *)NULL;
  r->Root->Right = (TBHNode *)NULL;
  
  r->Pts = Pts;
  r->Flags = 0;
  if(OwnsMemory)
    r->Flags |= FLAG_OWNSMEMORY;
  r->NumPts = NumPts;
  r->Root->Index.Pts = (TBHPoint **)NULL;
  r->Root->Index.NumPts = NumPts;
  r->Root->Buffer = (TBHPoint **)NULL;

  if(NumPts == 0)
    {
      r->Flags |= FLAG_EMPTY_TREE;
      return r;
    }
  
  for(j = BH_X; j <= BH_Z; j++)
    {
      xmin[j] = Pts[0].Pos[j];
      xmax[j] = xmin[j];
    }
  
  for(i = 1; i < NumPts; i++)
    {
      for(j = BH_X; j <= BH_Z; j++)
	{
	  if (xmin[j] > Pts[i].Pos[j]) xmin[j] = Pts[i].Pos[j];
	  if (xmax[j] < Pts[i].Pos[j]) xmax[j] = Pts[i].Pos[j];
	}
    }

  for(j = BH_X; j <= BH_Z; j++)
    {
      sxmin[j] = xmin[j] - SpacePadding;
      sxmax[j] = xmax[j] + SpacePadding;
      r->xmin[j] = sxmin[j];
      r->xmax[j] = sxmax[j];
    }

  r->Root->Buffer = (TBHPoint **)malloc(sizeof(TBHPoint *) * NumPts);
  if(r->Root->Buffer == NULL) return NULL;
  for(i = 0; i < NumPts; i++)
    {
      Pts[i].ID = i;
      r->Root->Buffer[i] = &Pts[i];
    }

  DivideTBHNode(r->Root, xmin, xmax, sxmin, sxmax,
		  Granularity, LeafPadding);

  if(r->Root->dim == -1 && r->Root->Index.Size == 0)
    {
      for(i = BH_X; i <= BH_Z; i++)
	{
	  r->Root->xmin[i] = sxmin[i];
	  r->Root->xmax[i] = sxmax[i];
	}

      r->Root->Index.Size = r->Root->Index.NumPts + LeafPadding;
      r->Root->Index.Pts = (TBHPoint **)malloc(sizeof(TBHPoint *) *
					       r->Root->Index.Size);
      for(i = 0; i < r->Root->Index.NumPts; i++)
	{
	  r->Root->Index.Pts[i] = r->Root->Buffer[i];
	  r->Root->Index.Pts[i]->Box = r->Root;
	}
    }

  return(r);
}

TBHNode *FindRBHNode(RBHTree *tree, float *x)
{
  TBHNode *r;
  int k;

  if(tree == NULL) 
    return((TBHNode *)NULL);
  if(tree->Flags & FLAG_EMPTY_TREE)
    return((TBHNode *)NULL);
  
  for(k = 0; k < 3; k++)
    {
      if(x[k] < tree->xmin[k]) return((TBHNode *)NULL);
      if(x[k] > tree->xmax[k]) return((TBHNode *)NULL);
    }
  
  r = tree->Root;
  while(r != NULL)
    {
      if(r->dim < 0) break;
      if(x[r->dim] < r->cut) r = r->Left;
      else r = r->Right;
    }
  
  return(r);
}

void FreeRBHTree(RBHTree *tree)
{
  if(!tree)
    return;

  if(tree->Pts != NULL && (tree->Flags & FLAG_OWNSMEMORY))
    free((void *)(tree->Pts));
  free((void *)(tree->FreePts.Pts));
  
  if(!(tree->Flags & FLAG_EMPTY_TREE))
    {
      free((void *)tree->Root->Buffer);
      FreeTBHNode(tree->Root);
    }
  free((void *)tree);
}

int InsertRBHPoint(RBHTree *tree,
		      float *Pos, float Rad, void *Data, int uInt,
		      int *ID)
{
  int  f;
  TBHPoint *point;
  TBHNode *NewBox, *PBox;

  if(!tree)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: InsertRBHPoint: Sent a NULL pointer.\n");
#endif
      return BH_NO;
    }

  if(tree->FreePts.NumPts == 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: InsertRBHPoint: No free space left.\n");
#endif
    return BH_NO;
    }

  f = tree->FreePts.NumPts - 1;
  *ID = tree->FreePts.Pts[f]->ID;

  point = &tree->Pts[*ID];
  point->Pos[0] = Pos[0];
  point->Pos[1] = Pos[1];
  point->Pos[2] = Pos[2];
  point->Rad = Rad;
  point->Data = Data;
  point->uInt = uInt;
  point->Box = tree->Root;  /* For regenerate; show that point is used */
  tree->FreePts.NumPts--;
  tree->NumPts++;

  NewBox = FindRBHNode(tree, Pos);

  if(NewBox == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Warning: InsertRBHPoint: Regenerating the tree.\n");
#endif

      if(!RegenerateRBHTree(tree))
	{
#ifdef VERBOSE_BH
	  fprintf(stderr, "Error: InsertRBHPoint: Failed to regenerate"
		  "the tree.\n");
#endif

	  return BH_NO;
	}

      return BH_YES;
    }

  if(NewBox->Index.NumPts == NewBox->Index.Size)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: InsertRBHPoint: Leaf padding filled.\n");
#endif

      if(!RegenerateRBHTree(tree))
	{
#ifdef VERBOSE_BH
	  fprintf(stderr, "Error: InsertRBHPoint: Failed to regenerate"
		  "the tree.\n");
#endif

	  return BH_NO;
	}

      return BH_YES;
    }

  point->Box = NewBox;

  NewBox->Index.Pts[NewBox->Index.NumPts] = &tree->Pts[*ID];

  PBox = NewBox;
  while(PBox != NULL)
    {
      PBox->Index.NumPts += 1;
      PBox = PBox->Parent;
    }

  return BH_YES;
}

static void FreeTreeStructure(TBHNode *node)
{
  if(!node)
    return;

  FreeTreeStructure(node->Left);
  FreeTreeStructure(node->Right);

  if(node->Index.Pts)
    free((void *)node->Index.Pts);
  free((void *)node);

  return;
}

static int RegenerateRBHTree(RBHTree *tree)
{
  int i, j;
  float xmin[3], xmax[3], sxmin[3], sxmax[3];
  TBHPoint **RealPts;

  /* This function restructures the tree while keeping the structure
     of the Pts array in O(n log(n)) */

  if(tree == NULL) 
    return 0;
  
  RealPts = (TBHPoint **)malloc(sizeof(TBHPoint *) * tree->NumPts);
  for(i = j = 0; i < tree->NumPts; i++)
    {
      while(tree->Pts[j].Box == NULL)
	j++;
      RealPts[i] = &(tree->Pts[j]);
      j++;
    }

  if(tree->NumPts)
    tree->Flags &= ~FLAG_EMPTY_TREE;

  tree->bfl = 0;

  tree->rm=0.0;
  for(i = 0; i < tree->NumPts; i++)
    if(tree->rm < RealPts[i]->Rad) tree->rm = RealPts[i]->Rad;
  tree->rm += 0.1;

#ifdef STATBHTREE
  tree->tot = 0;
  tree->max = 0;
  tree->min = 9999999;
  tree->nbr = 0;
#endif
  
  if(tree->Root->Buffer)
    free((void *)tree->Root->Buffer);
  FreeTreeStructure(tree->Root);
  tree->Root = (TBHNode *)malloc((size_t)sizeof(TBHNode));
  if(tree->Root == NULL)
    {
      free((void *)RealPts);
      return 0;
    }

  tree->Root->Index.NumPts = 0;
  tree->Root->Index.Pts = (TBHPoint **)NULL;
  tree->Root->Index.Size = 0;
  tree->Root->Parent = (TBHNode *)NULL;
  tree->Root->dim = -1;
  tree->Root->Left = (TBHNode *)NULL;
  tree->Root->Right = (TBHNode *)NULL;
  
  if(tree->NumPts == 0)
    {
      /* This will never happen */
      free((void *)RealPts);
      return 0;
    }
  
  for(j = BH_X; j <= BH_Z; j++)
    {
      xmin[j] = RealPts[0]->Pos[j];
      xmax[j] = xmin[j];
    }
  
  for(i = 1; i < tree->NumPts; i++)
    {
      for(j = BH_X; j <= BH_Z; j++)
	{
	  if (xmin[j] > RealPts[i]->Pos[j]) xmin[j] = RealPts[i]->Pos[j];
	  if (xmax[j] < RealPts[i]->Pos[j]) xmax[j] = RealPts[i]->Pos[j];
	}
    }
  
  tree->Root->Index.Pts = (TBHPoint **)NULL;
  tree->Root->Index.NumPts = tree->NumPts;
  for(j = BH_X; j <= BH_Z; j++)
    {
      sxmin[j] = xmin[j] - tree->SpacePadding;
      sxmax[j] = xmax[j] + tree->SpacePadding;
      tree->xmin[j] = sxmin[j];
      tree->xmax[j] = sxmax[j];
    }

  tree->Root->Buffer = (TBHPoint **)malloc(sizeof(TBHPoint *) * 
					   tree->NumPts);
  if(tree->Root->Buffer == NULL)
    {
      free((void *)RealPts);
      return 0;
    }
  for(i = 0; i < tree->NumPts; i++)
    tree->Root->Buffer[i] = RealPts[i];

  DivideTBHNode(tree->Root, xmin, xmax, sxmin, sxmax,
		  tree->Granularity, tree->LeafPadding);

  if(tree->Root->dim == -1 && tree->Root->Index.Size == 0)
    {
      for(i = BH_X; i <= BH_Z; i++)
	{
	  tree->Root->xmin[i] = sxmin[i];
	  tree->Root->xmax[i] = sxmax[i];
	}

      tree->Root->Index.Size = tree->Root->Index.NumPts + tree->LeafPadding;
      tree->Root->Index.Pts = (TBHPoint **)malloc(sizeof(TBHPoint *) *
					       tree->Root->Index.Size);
      for(i = 0; i < tree->Root->Index.NumPts; i++)
	{
	  tree->Root->Index.Pts[i] = tree->Root->Buffer[i];
	  tree->Root->Index.Pts[i]->Box = tree->Root;
	}
    }
 
  free((void *)RealPts);
  return 1;
}

int DeleteRBHPoint(RBHTree *tree,
		      int ID)
{
  int i;
  TBHNode *OrigBox;
  TBHPoint *Pt;

  if(!tree)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: DeleteRBHPoint: Sent a NULL pointer.\n");
#endif
      return BH_NO;
    }
  if(tree->Flags & FLAG_EMPTY_TREE)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: DeleteRBHPoint: Sent an empty tree.\n");
#endif
      return BH_NO;
    }

  if(ID >= tree->SizePts || ID < 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: DeleteRBHPoint: Invalid point ID.\n");
#endif
      return BH_INVALID_POINT;
    }

  OrigBox = tree->Pts[ID].Box;
  if(OrigBox == NULL) 
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "ERROR: DeleteRBHPoint: Tried to delete\n\
   already deleted point.\n");
#endif
      return BH_ALREADY_DELETED;
    }
  if(OrigBox->Index.NumPts == 0) 
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "ERROR: DeleteRBHPoint: Tried to delete\n\
   a point from an empty box.\n");
#endif
      return BH_EMPTY_BOX;
    }

  Pt = &tree->Pts[ID];
  for(i = 0; i < OrigBox->Index.NumPts; i++)
    {
      if(OrigBox->Index.Pts[i] == Pt)
	break;
    }
  if (i == OrigBox->Index.NumPts)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "ERROR: DeleteRBHPoint: Unable to find point\n\
   in leaf array.\n");
#endif
      return BH_INVALID_POINT;
    }

  for(; i < OrigBox->Index.NumPts - 1; i++)
    OrigBox->Index.Pts[i] = OrigBox->Index.Pts[i + 1];
  OrigBox->Index.NumPts--;

  if(tree->FreePts.NumPts == tree->FreePts.Size)
    {
      tree->FreePts.Size += BH_PADDING;
      tree->FreePts.Pts = (TBHPoint **)realloc(tree->FreePts.Pts,
					       sizeof(TBHPoint *) *
					       tree->FreePts.Size);
      if(tree->FreePts.Pts == NULL) return BH_NO;
#ifdef VERBOSE_BH
      fprintf(stderr,
	      "WARNING: DeleteRBHPoint: Too many points deleted from\n\
   RBHTree, reallocating space in tree->FreePts.Pts\n");
#endif
    }

  tree->FreePts.Pts[tree->FreePts.NumPts] = &tree->Pts[ID];
  tree->Pts[ID].Box = NULL;
  tree->FreePts.NumPts++;
  tree->NumPts--;

  return BH_YES;
}

int MoveRBHPoint(RBHTree *tree,
		    int ID,
		    float NewPos[3],
		    int FindDirection)
{
  int i, j;
  TBHNode *OrigBox;
  TBHNode *NewBox;
  TBHPoint *Pt;

  if(!tree || (tree->Flags & FLAG_EMPTY_TREE))
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveRBHPoint: Sent an empty tree.\n");
#endif
      return BH_NO;
    }

  if(ID >= tree->SizePts || ID < 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveRBHPoint: Invalid point ID.\n");
#endif
      return BH_INVALID_POINT;
    }

  OrigBox = tree->Pts[ID].Box;
  if(OrigBox == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveRBHPoint: Point already deleted.\n");
#endif
      return BH_ALREADY_DELETED;
    }
  j = BH_YES;
  for(i = BH_X; i <= BH_Z && j == BH_YES; i++)
    {
      if(NewPos[i] > OrigBox->xmax[i] ||
	 NewPos[i] < OrigBox->xmin[i]) j = BH_NO;
    }
  if(j == BH_YES)
    {
      tree->Pts[ID].Pos[BH_X] = NewPos[BH_X];
      tree->Pts[ID].Pos[BH_Y] = NewPos[BH_Y];
      tree->Pts[ID].Pos[BH_Z] = NewPos[BH_Z];
      return BH_YES;
    }
  if(OrigBox->Index.NumPts == 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveRBHPoint: Empty box.\n");
#endif
      return BH_EMPTY_BOX;
    }

  tree->Pts[ID].Pos[BH_X] = NewPos[BH_X];
  tree->Pts[ID].Pos[BH_Y] = NewPos[BH_Y];
  tree->Pts[ID].Pos[BH_Z] = NewPos[BH_Z];

  if(FindDirection == BH_SEARCH_UP)
    NewBox = FindTBHNodeUp(OrigBox, NewPos); 
  else
    NewBox = FindRBHNode(tree, NewPos);

  if(NewBox == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveRBHPoint: Point outside tree.\n");
#endif
      return BH_OUTSIDE_TREE;    /* Regenerate the tree */
    }

  Pt = &tree->Pts[ID];
  for(i = 0; i < OrigBox->Index.NumPts; i++)
    {
      if(OrigBox->Index.Pts[i] == Pt)
	break;
    }
  if (i == OrigBox->Index.NumPts)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "ERROR: DeleteRBHPoint: Unable to find point\n\
   in leaf array.\n");
#endif
      return BH_INVALID_POINT;
    }

  for(; i < OrigBox->Index.NumPts - 1; i++)
    OrigBox->Index.Pts[i] = OrigBox->Index.Pts[i + 1];
  OrigBox->Index.NumPts--;

  if(NewBox->Index.NumPts == NewBox->Index.Size)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: MoveRBHPoint: Leaf padding filled.\n");
#endif
      if(!RegenerateRBHTree(tree))
	{
#ifdef VERBOSE_BH
	  fprintf(stderr, "Error: InsertRBHPoint: Failed to regenerate"
		  "the tree.\n");
#endif

	  return BH_NO;
	}

      return BH_YES;
    }

  tree->Pts[ID].Box = NewBox;
  NewBox->Index.Pts[NewBox->Index.NumPts] = &tree->Pts[ID];
  NewBox->Index.NumPts++;

  return BH_YES;
}

int ModifyRBHPoint(RBHTree *tree,
		      int ID,
		      float Rad)
{
  if(!tree || (tree->Flags & FLAG_EMPTY_TREE))
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Sent an empty tree.\n");
#endif
      return BH_NO;
    }

  if(ID >= tree->SizePts || ID < 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Invalid point ID.\n");
#endif
      return BH_INVALID_POINT;
    }
  if(tree->Pts[ID].Box == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Invalid point ID;\
 NULL box.\n");
#endif
      return BH_INVALID_POINT;
    }

  tree->Pts[ID].Rad = Rad;
  if (Rad > tree->rm) tree->rm = Rad;
  return BH_YES;
}

int ModifyBHPoint(TBHTree *tree,
		     int ID,
		     float Rad)
{
  if(!tree)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Sent an empty tree.\n");
#endif
      return BH_NO;
    }

  if(ID >= tree->NumPts || ID < 0)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Invalid point ID.\n");
#endif
      return BH_INVALID_POINT;
    }
  if(tree->Pts[ID].Box == NULL)
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Invalid point ID;\
 NULL box.\n");
#endif
      return BH_INVALID_POINT;
    }

  tree->Pts[ID].Rad = Rad;
  if (Rad > tree->rm) tree->rm = Rad;
  return BH_YES;
}

int FindRBHCloseAtomsDist(RBHTree *tree, float *x, float cutoff,
			     int *atom, float *dist, int maxn)
{
  int i;

  if(!tree || (tree->Flags & FLAG_EMPTY_TREE))
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Sent an empty tree.\n");
#endif
      return BH_NO;
    }

  if(maxn < 1 || tree == NULL || cutoff <= 0.0) return(0);
  if(tree->Root == NULL) return(0);
  
  for(i = 0; i < 3; i++)
    {
      if(x[i] < tree->xmin[i] - cutoff) break;
      if(x[i] > tree->xmax[i] + cutoff) break;
    }
  if(i < 3) return(0);
  
  return (FindTBHCloseAtomsInNodeDist(tree->Root, x, cutoff,
				      atom, dist, maxn));
}

int FindRBHCloseAtoms(RBHTree *tree, float *x, float cutoff,
			 int *atom, int maxn)
{
  int i;

  if(!tree || (tree->Flags & FLAG_EMPTY_TREE))
    {
#ifdef VERBOSE_BH
      fprintf(stderr, "Error: ModifyRBHPoint: Sent an empty tree.\n");
#endif
      return BH_NO;
    }

  if(maxn < 1 || tree == NULL || cutoff <= 0.0) return(0);
  if(tree->Root == NULL) return(0);

  for(i = BH_X; i <= BH_Z; i++) 
    {
      if(x[i] < tree->xmin[i] - cutoff) break;
      if(x[i] > tree->xmax[i] + cutoff) break;
    }

  if(i < 3) return(0);
#ifdef STATBHTREE
  i = FindTBHCloseAtomsInNode(tree->Root, x, cutoff, atom, maxn);
  tree->nbr++;
  tree->tot += i;
  if(i > tree->max) tree->max = i;
  if(i < tree->min) tree->min = i;
  return(i);
#else
  return(FindTBHCloseAtomsInNode(tree->Root, x, cutoff, atom, maxn));
#endif
}
