// spline2.cpp
// A 2D spline
// adapted from NRC

#include "spline.h"
#include "splint.h"
#include "spline2.h"

void spline2(double *x1a,double *x2a,double *ya,int32 m,int32 n, double *y2a)
{
int i,j;

for (i=0; i<m; i++)
	{
	spline_0(x2a,ya+i*n,n,1.0e30,1.0e30,y2a+i*n);
	}
}


void splint2(double *x1a,double *x2a,double *ya,double *y2a,int32 m,int32 n,double x2,double *x1,int32 nx1,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
int i,j,k;



y2temp = new double[m];
yy2temp = new double[m];


for (i=0; i<m; i++)
	{
	splint_0(x2a,ya+i*n,y2a+i*n,n,x2,yy2temp + i);
	}
spline_0(x1a,yy2temp,m,1.0e30,1.0e30,y2temp);
for (k=0; k<nx1; k++)
	{
	splint_0(x1a,yy2temp,y2temp,m,x1[k],y+k);
	}
	

delete y2temp;
delete yy2temp;
} 


void splint2_clip(double *x1a,double *x2a,double *ya,double *y2a,int32 m,int32 n,double x2,double *x1,int32 nx1,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
int i,j,k;



y2temp = new double[m];
yy2temp = new double[m];


for (i=0; i<m; i++)
	{
	splint_0_clip(x2a,ya+i*n,y2a+i*n,n,x2,yy2temp + i);
	}
spline_0(x1a,yy2temp,m,1.0e30,1.0e30,y2temp);
for (k=0; k<nx1; k++)
	{
	splint_0_clip(x1a,yy2temp,y2temp,m,x1[k],y+k);
	}
	

delete y2temp;
delete yy2temp;
} 
