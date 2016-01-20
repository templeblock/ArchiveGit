// picelin.c

// The picewise linear function 
// The data format is compatiable with the splint function 

void piceLin(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	int klo,khi,k;
	double h,a;


	if (x < xa[1])
		{
		h=xa[2]-xa[1];
		if (h == 0)  { *y=0; return;}
		a=(x-xa[1])/h;
		*y=(1-a)*ya[1]+a*ya[2];
		return;
		}
	if (x > xa[n])
		{
		h=xa[n]-xa[n-1];
		if (h == 0)  { *y=0; return;}
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
	if (h == 0) { *y=0; return;}
	a=(x-xa[klo])/h;
	*y=(1-a)*ya[klo]+a*ya[khi];
}

void piceLin_0(double *xa,double *ya,double *y2a,int n,double x,double *y)
{
	int klo,khi,k;
	double h,a;
	int	m = n-1;
	int	p = m-1;

	if (x < xa[0])
		{
		h=xa[1]-xa[0];
		if (h == 0)  { *y=0; return;}
		a=(x-xa[0])/h;
		*y=(1-a)*ya[0]+a*ya[1];
		return;
		}
	if (x > xa[m])
		{
		h=xa[m]-xa[p];
		if (h == 0)  { *y=0; return;}
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
	if (h == 0) { *y=0; return;}
	a=(x-xa[klo])/h;
	*y=(1-a)*ya[klo]+a*ya[khi];
}
