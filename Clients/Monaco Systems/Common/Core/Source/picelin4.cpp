// picelin4.c

// The picewise linear function 
// The data format is compatiable with the splint3 function 
#include "picelin.h"
#include "picelin4.h"


void piceLin4(double *xa,double *ya,int32 m,double *x,double *y)
{
double *y1temp;
double *yy1temp;
double *y2temp;
double *yy2temp;
double *y3temp;
double *yy3temp;
int i,j,k,l;



y1temp = new double[m];
yy1temp = new double[m];

y2temp = new double[m];
yy2temp = new double[m];

y3temp = new double[m];
yy3temp = new double[m];


for (i=0; i<m; i++)
	{
	for (j=0; j<m; j++)
		{
		for (k=0; k<m; k++)
			{
			piceLin(xa-1,ya+i*m*m*m+j*m*m+k*m - 1,ya+i*m*m*m+j*m*m+k*m - 1,m,x[3],yy1temp+j);
			}
		piceLin(xa-1,yy1temp-1,y1temp-1,m,x[2],yy2temp + i);
		}
	piceLin(xa-1,yy2temp-1,y2temp-1,m,x[1],yy3temp + i);
	}

piceLin(xa-1,yy3temp-1,y3temp-1,m,x[0],y);

	
delete y1temp;
delete yy1temp;

delete y2temp;
delete yy2temp;

delete y3temp;
delete yy3temp;
} 
