//  splinesegment.cpp
// a multisegment spline code


splineTwoPoint(double x0,double y0, double s0, double x1,double y1, double s1,
			   double xf,double *f)
{
	int i,k;
	double p,qn,sig,un,u[2];
	double y2[2];
	
	double h,a,b;
	
	y2[0] = -0.5;
	u[0]=(3.0/(x1-x0))*((y1-y0)/(x1-x0)-s0);	
	
	qn=0.5;
	un=(3.0/(x1-x0))*(s1-(y1-y0)/(x1-x0));
	y2[1]=(un-qn*u[0])/(qn*y2[0]+1.0);
	y2[0]=y2[0]*y2[1]+u[0];
	
	h = x1-x0;
	
	a=(x1-xf)/h;
	b=(xf-x0)/h;
	*f=a*y0+b*y1+((a*a*a-a)*y2[0]+(b*b*b-b)*y2[1])*(h*h)*0.16666666667;
}

SplineMultiSegment(double *x,double *y, long num, double xf, double *f)
{
long	i,j,k,klo,khi;
double *slopes;
double *deltas;

slopes = new double[num];
deltas = new double [num];


for (i=1; i<num; i++)
	{
	deltas[i-1] = (y[i]-y[i-1])/(x[i]-x[i-1]);
	}


for (i=1; i<num; i++)
	{
	if (deltas[i-1]*deltas[i] <=0)
		slopes[i] = 0;
	else 
		slopes[i] = 2.0*deltas[i-1]*deltas[i]/(deltas[i]+deltas[i-1]);
	}
	
if (num == 2)
	slopes[0] = slopes[1] = deltas[0];
else
	{
	if (deltas[0] == 0.0)
		slopes[0] = 0.0;
	else
		slopes[0] = 2.0*deltas[0]*deltas[0]/(slopes[1]+deltas[0]);
	
	if (deltas[num - 2] == 0.0)
		slopes[num-1] = 0.0;
	else
		slopes[num-1] = 2.0*deltas[num-2]*deltas[num-2]/(slopes[num-2]+deltas[num-2]);
	}
	
	
	klo=0;
	khi=num-1;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (x[k] > xf) khi=k;
		else klo=k;
	}	
	
splineTwoPoint(x[klo],y[klo],slopes[klo],x[khi],y[khi],slopes[khi],xf,f);

delete slopes;
delete deltas;
}