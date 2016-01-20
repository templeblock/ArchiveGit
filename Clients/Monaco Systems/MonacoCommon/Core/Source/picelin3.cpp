// picelin.c

// The picewise linear function 
// The data format is compatiable with the splint3 function 
#include "picelin.h"
#include "picelin3.h"


void piceLin3(double *x1a,double *x2a,double *x3a,double *ya,double *y2a,int32 m,int32 n, int32 p,double *x23,double *x1,int32 nx1,double *y)
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
	piceLin(x2a-1,yy1temp-1,y1temp-1,n,x23[0],yy2temp + i);
	}
for (k=0; k<nx1; k++)
	{
	piceLin(x1a-1,yy2temp-1,y2temp-1,m,x1[k],y+k);
	}
	
delete y1temp;
delete yy1temp;

delete y2temp;
delete yy2temp;
} 
