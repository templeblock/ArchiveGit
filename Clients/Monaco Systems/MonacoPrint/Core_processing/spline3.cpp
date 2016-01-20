// spline3.c
// A 3D spline
// By James Vogh

#include "spline3.h"
#include "spline.h"
#include "splint.h"
#include "picelin.h"

void spline3(double *x1a,double *x2a,double *x3a,double *ya,int32 m,int32 n, int32 p, double *y2a)
{
int i,j;

for (i=0; i<m; i++)
	for (j=0; j<n; j++)
		{
		spline_0(x3a,ya+i*n*p+j*p,p,1.0e30,1.0e30,y2a+i*n*p+j*p);
		}
}


void splint3(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
int i,j,k;



y1temp = new double[n];
yy1temp = new double[n];

y2temp = new double[m];
yy2temp = new double[m];


for (i=0; i<m; i++)
	{
	for (j=0; j<n; j++)
		{
		splint_0(x3a,ya+i*n*p+j*p ,y2a+i*n*p+j*p ,p,x23[1],yy1temp+j);
		}
	spline_0(x2a,yy1temp,n,1.0e30,1.0e30,y1temp);
	splint_0(x2a,yy1temp,y1temp,n,x23[0],yy2temp + i);
	}
spline_0(x1a,yy2temp,m,1.0e30,1.0e30,y2temp);
for (k=0; k<nx1; k++)
	{
	splint_0(x1a,yy2temp,y2temp,m,x1[k],y+k);
	}
	
delete y1temp;
delete yy1temp;

delete y2temp;
delete yy2temp;
} 

// the third dimension varies 
void spline3_s3(double *x1a,double *x2a,double *x3a,double *ya,int32 m,int32 n, int32 p, double *y2a)
{
int i,j;

for (i=0; i<m; i++)
	for (j=0; j<n; j++)
		{
		spline_0(x3a+i*n*p+j*p ,ya+i*n*p+j*p ,p,1.0e30,1.0e30,y2a+i*n*p+j*p);
		}
}

// the third dimension varies 
void splint3_s3(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
int i,j,k;



y1temp = new double[n];
yy1temp = new double[n];

y2temp = new double[m];
yy2temp = new double[m];


for (i=0; i<m; i++)
	{
	for (j=0; j<n; j++)
		{
		splint_0(x3a+i*n*p+j*p,ya+i*n*p+j*p ,y2a+i*n*p+j*p ,p,x23[1],yy1temp+j);
		}
	spline_0(x2a,yy1temp,n,1.0e30,1.0e30,y1temp);
	splint_0(x2a,yy1temp,y1temp,n,x23[0],yy2temp + i);
	}
spline_0(x1a,yy2temp,m,1.0e30,1.0e30,y2temp);
for (k=0; k<nx1; k++)
	{
	splint_0(x1a,yy2temp,y2temp,m,x1[k],y+k);
	}
	
delete y1temp;
delete yy1temp;

delete y2temp;
delete yy2temp;
}


// the third dimension varies 
void splint3_s3_clip(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
int i,j,k;



y1temp = new double[n];
yy1temp = new double[n];

y2temp = new double[m];
yy2temp = new double[m];


for (i=0; i<m; i++)
	{
	for (j=0; j<n; j++)
		{
		splint_0_clip(x3a+i*n*p+j*p,ya+i*n*p+j*p ,y2a+i*n*p+j*p ,p,x23[1],yy1temp+j);
		}
	spline_0(x2a,yy1temp,n,1.0e30,1.0e30,y1temp);
	splint_0_clip(x2a,yy1temp,y1temp,n,x23[0],yy2temp + i);
	}
spline_0(x1a,yy2temp,m,1.0e30,1.0e30,y2temp);
for (k=0; k<nx1; k++)
	{
	splint_0_clip(x1a,yy2temp,y2temp,m,x1[k],y+k);
	}
	
delete y1temp;
delete yy1temp;

delete y2temp;
delete yy2temp;
}




// a linear version of the above
/*
void splint3(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
int i,j,k;



y1temp = new double[n];
yy1temp = new double[n];

y2temp = new double[m];
yy2temp = new double[m];


for (i=0; i<m; i++)
	{
	for (j=0; j<n; j++)
		{
		piceLin(x3a-1,ya+i*n*p+j*p - 1,y2a+i*n*p+j*p - 1,p,x23[1],yy1temp+j);
		}
	//spline(x2a-1,yy1temp-1,n,1.0e30,1.0e30,y1temp-1);
	piceLin(x2a-1,yy1temp-1,y1temp-1,n,x23[0],yy2temp + i);
	}
//spline(x1a-1,yy2temp-1,m,1.0e30,1.0e30,y2temp-1);
for (k=0; k<nx1; k++)
	{
	piceLin(x1a-1,yy2temp-1,y2temp-1,m,x1[k],y+k);
	}
	
delete y1temp;
delete yy1temp;

delete y2temp;
delete yy2temp;
} */