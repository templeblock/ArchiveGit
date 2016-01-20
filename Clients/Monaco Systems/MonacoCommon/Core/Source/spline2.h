// spline2.h
// A 3D spline
// By James Vogh

#ifndef IN_SPLINE2
#define IN_SPLINE2

#include "mcotypes.h"

void spline2(double *x1a,double *x2a,double *ya,int32 m,int32 n, double *y2a);
void splint2(double *x1a,double *x2a,double *ya,double *y2a,int32 m,int32 n,double x2,double *x1,int32 nx1,double *y);
void splint2_clip(double *x1a,double *x2a,double *ya,double *y2a,int32 m,int32 n,double x2,double *x1,int32 nx1,double *y);



#endif