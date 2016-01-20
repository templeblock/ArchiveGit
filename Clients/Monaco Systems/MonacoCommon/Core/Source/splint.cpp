/* The splint code adapted from the NRC code */

//#include "fixed_math.h"
#include "splint.h"

void splint(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	int klo,khi,k;
	double h,b,a;

	// use linear interpolation if outside of bounds
	if (x < xa[1])
		{
		h=xa[2]-xa[1];
		if (h == 0) 
			{
			*y = 0;
			return;
			}
		a=(x-xa[1])/h;
		*y=(1-a)*ya[1]+a*ya[2];
		return;
		}
	if (x > xa[n])
		{
		h=xa[n]-xa[n-1];
		if (h == 0) 
			{
			*y = 0;
			return;
			}
		a=(x-xa[n-1])/h;
		*y=(1-a)*ya[n-1]+a*ya[n];
		return;
		}

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0) 
		{
		*y = 0;
		return;
		}
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)*0.16666666667;
}

void splint_0(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	register int klo,khi,k;
	double h,hinv,b,a;
	int	m = n-1;
	int	p = m-1;

	// use linear interpolation if outside of bounds
	if (x < xa[0])
		{
		h=xa[1]-xa[0];
		if (h == 0) 
			{
			*y = 0;
			return;
			}
		a=(x-xa[0])/h;
		*y=(1-a)*ya[0]+a*ya[1];
		return;
		}
	if (x > xa[m])
		{
		h=xa[m]-xa[p];
		if (h == 0) 
			{
			*y = 0;
			return;
			}
		a=(x-xa[p])/h;
		*y=(1-a)*ya[p]+a*ya[m];
		return;
		}

	klo=0;
	khi=m;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0) 
		{
		*y = 0;
		return;
		}
	hinv = 1/h;
	a=(xa[khi]-x)*hinv;
	b=(x-xa[klo])*hinv;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)*0.16666666667;
}


void splint_0_clip(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	register int klo,khi,k;
	double h,hinv,b,a;
	int	m = n-1;
	int	p = m-1;

	// use linear interpolation if outside of bounds
	if (x < xa[0])
		{
		h=xa[1]-xa[0];
		if (h == 0) 
			{
			*y = 0;
			return;
			}
		a=(x-xa[0])/h;
		*y=(1-a)*ya[0]+a*ya[1];
		return;
		}
	if (x > xa[m])
		{
		h=xa[m]-xa[p];
		if (h == 0) 
			{
			*y = 0;
			return;
			}
		a=(x-xa[p])/h;
		*y=(1-a)*ya[p]+a*ya[m];
		return;
		}

	klo=0;
	khi=m;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0) 
		{
		*y = 0;
		return;
		}
	hinv = 1/h;
	a=(xa[khi]-x)*hinv;
	b=(x-xa[klo])*hinv;
	if ((ya[klo] == 0) && (ya[khi] == 0)) *y = 0;
	else *y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)*0.16666666667;
	if ((*y < 0) && (ya[klo] >= 0) && (ya[khi] >= 0))  *y=a*ya[klo]+b*ya[khi];
}


/*
void splint(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	int klo,khi,k;
	double h,hi,b,a;
	double	xh,xl;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	xh = xa[khi];
	xl = xa[klo];
	h = xh-xl;
	hi = 1/h;
	a=(xh-x)*hi;
	b=(x-xl)*hi;
	*y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}


void splint_int(int32 *xa,int32 *ya,int32 *y2a,int n,int32 x,int32 *y)
{
	int klo,khi,k;
	int32 h,b,a;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	a= fd2((xa[khi]-x),h);
	b= fd2((x-xa[klo]),h);
	*y = fm2(a,ya[klo]) + fm2(b,ya[khi]) +
		 fd2( 
		 	 fm2( fm2( (fm3(a,a,a)-a) , y2a[klo]) +
		 	      fm2( (fm3(b,b,b)-b) , y2a[khi])  , fm2(h,h))
		 	 , 0x5FFFA);
	
	*y = *y >> 16;
}
*/