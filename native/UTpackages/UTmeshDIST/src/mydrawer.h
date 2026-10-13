#ifndef MYDRAWER_H
#define MYDRAWER_H
#include "geoframe.h"
#include <vector>
#include <iostream>
using namespace std;

struct plane {
        plane(): a(1.0), b(0.0), c(0.0), d(0.0) {}
        plane(double newa, double newb, double newc, double newd) :
                a(newa), b(newb), c(newc), d(newd) {}

        double a, b, c, d;
};

typedef vector<vector<float> > FloatArray;
typedef vector <vector<int> > IntArray;

class MyDrawer
{
 public:
  
  MyDrawer();
  
  geoframe *g_frame;
  

  void display(IntArray *, FloatArray *);
  void display_tri(int, int, int, int, int, IntArray *);
  void display_hexa(int, int, int, IntArray *);
  void display_tri0(int, int, int, int, int, int, IntArray *);
  void display_tri00(int, int, int, int, int, int, int, IntArray *);
  
  void display_tetra(int, int, int, IntArray *, FloatArray *);
  void display_tetra_in(int, int, int, IntArray *, FloatArray *);
  void display_tri_vv(float*, float*, float*, int, int, int, FloatArray *);
  void display_permute_1(float*, float*, float*, float*);
  void display_permute_2(float*, float*, float*, float*);
  void display_permute_3(float*, float*, float*, float*);
  void display_permute_1_z(float*, float*, float*, float*);
  void display_permute_2_z(float*, float*, float*, float*);
  void display_permute_3_z(float*, float*, float*, float*);
  void display_1(int*, int, float*, float*, float*, float*, int, int, FloatArray *);
  void display_2(int*, int, float*, float*, float*, float*, int, int, FloatArray *);
  void display_3(int*, int, float*, float*, float*, float*, int, int, FloatArray *);
  void display_1_z(int*, int, float*, float*, float*, float*, int, int, FloatArray *);
  void display_2_z(int*, int, float*, float*, float*, float*, int, int, FloatArray *);
  void display_3_z(int*, int, float*, float*, float*, float*, int, int, FloatArray * );
  void setGeo(geoframe* g);
  void setCutting(int flag) { cut_flag = flag; };
  void setExtents(double biggest, double cx, double cy, double cz);
  void setPlaneX(float planex) {plane_x = planex;};
  void setPlaneZ(float planez) {plane_z = planez;};
  
  
 protected:
  
  
 private:
  
  void clearGeo() 
    {
      g_frame->Clear();
    }
  
  int cut_flag;
  int flat_flag;
  int wireframe_flag;
  char fn[100];
  
  int r_oldmx, r_oldmy;		
  int t_oldmx, t_oldmy, t_oldmz;
  int zoom_z;
  float plane_x, plane_z;  
  plane frustum[6];
  
  float dist;
  
  double biggestDim;
  double center[3], centerx, centery, centerz;
  double t_x,t_y,t_z;
  int vcount;
  
};


#endif
