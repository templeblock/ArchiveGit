//////////////////////////////////////////////////////////////////////////
//  safe_inter.cpp														//
//  interpolate with spline or picewise linear based on the number of 	//
//  points																//
//  James Vogh															//
//////////////////////////////////////////////////////////////////////////

#include "spline.h"
#include "splint.h"
#include "picelin.h"
#include "safe_inter.h"

void sinterp_init(double *x,double *y,int n,double yp1,double ypn,double *y2)
{

if (n < 15)
	{
	spline_0(x,y,n,yp1,ypn,y2);
	}
}

void sinterp_eval(double *xa,double *ya,double *y2a,int n,double x,double *y)
{

if (n < 15)
	{
	splint_0(xa,ya,y2a,n,x,y);
	}
else 
	{
	piceLin_0(xa,ya,y2a,n,x,y);
	}
}