// spline3.h
// A 3D spline
// By James Vogh

#ifndef IN_SPLINE3
#define IN_SPLINE3

#include "mcotypes.h"

void spline3(double *x1a,double *x2a,double *x3a,double *ya,int32 m,int32 n, int32 p, double *y2a);
void splint3(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y);

void spline3_s3(double *x1a,double *x2a,double *x3a,double *ya,int32 m,int32 n, int32 p, double *y2a);
void splint3_s3(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y);
void splint3_s3_clip(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y);


#endif