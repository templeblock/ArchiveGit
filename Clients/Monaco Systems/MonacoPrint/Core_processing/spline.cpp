
/* the spline routinue adapted from the NRC code */

//#include "fixed_math.h"

 
// The old, slow, yet readable spline code
void spline(double *x,double *y,int n,double yp1,double ypn,double *y2)
{
	int i,k;
	double p,qn,sig,un,*u;
	
	u = new double[n];


	if (yp1 > 0.99e30)
		y2[1]=u[1]=0.0;
	else {
		y2[1] = -0.5;
		u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}
	for (i=2;i<=n-1;i++) {
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0;
		y2[i]=(sig-1.0)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30)
		qn=un=0.0;
	else {
		qn=0.5;
		un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}
	y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
	
	delete u;

} 

void spline_0(double *x,double *y,int n,double yp1,double ypn,double *y2)
{
	int i,k;
	double p,qn,sig,un,*u;
		

	u = new double[n];

	if (yp1 > 0.99e30)
		y2[0]=u[0]=0.0;
	else {
		y2[0] = -0.5;
		u[0]=(3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
	}
	for (i=1;i<=n-2;i++) {
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0;
		y2[i]=(sig-1.0)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30)
		qn=un=0.0;
	else {
		qn=0.5;
		un=(3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
	}
	y2[n-1]=(un-qn*u[n-2])/(qn*y2[n-2]+1.0);
	for (k=n-2;k>=0;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];

	delete u;
	

} 

/*
// The faster spline code, lots of luck reading it
void spline(double *x,double *y,int n,double yp1,double ypn,double *y2)
{
	int i,k;
	double p,qn,sig,un,u[MaxNumHands+1];
	
	double *x_p_m,*x_p,*x_p_p;
	double *y_p_m,*y_p,*y_p_p;
	double *y2_p,*u_p;



	if (yp1 > 0.99e30)
		y2[1]=u[1]=0.0;
	else {
		y2[1] = -0.5;
		u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}
	x_p_m = x;
	x_p_m++;
	x_p = x_p_m+1;
	x_p_p = x_p+1;

	y_p_m = y;
	y_p_m++;
	y_p = y_p_m+1;
	y_p_p = y_p+1;	

	y2_p = y2;
	y2_p++;

	u_p = u;
	u_p += 2;

	for (i=2;i<=n-1;i++) {
		sig=((*x_p) - (*x_p_m))/((*x_p_p) - (*x_p_m));
		p=1/(sig*(*y2_p)+2.0);
		(*y2_p++)=(sig-1.0)*p;
		(*u_p) = ((*y_p_p++) - (*y_p))/((*x_p_p) - (*x_p)) - ((*y_p) - (*y_p_m++))/((*x_p) - (*x_p_m));
		(*u_p) = (6.0*(*u_p)/((*x_p_p++) - (*x_p_m++))-sig*(*(u_p-1)))*p;


		u_p++;
		x_p++;
		y_p++;
		
	}
	if (ypn > 0.99e30)
		qn=un=0.0;
	else {
		qn=0.5;
		un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}
	y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
	

}

void spline_int(int32 *x,int32 *y,int n,int32 yp1,int32 ypn,int32 *y2)
{
	int i,k;
	int32 p,qn,sig,un,u[MaxNumHands+1];
	
	
	for (i=1; i<=n; i++) 
		{
		x[i] = x[i] << 16;
		y[i] = y[i] << 16;
		}
		
	yp1 = yp1 << 16;
	ypn = ypn << 16;
	
	if (yp1 > 2147483647)
		y2[1]=u[1]=0;
	else {
		y2[1] = -0x8000;
		u[1]=fm2((fd2(0x2FFFD,(x[2]-x[1])) ),( fd2((y[2]-y[1]),(x[2]-x[1]))-yp1));
	}
	for (i=2;i<=n-1;i++) {
		sig=fd2((x[i]-x[i-1]),(x[i+1]-x[i-1]));
		p=fm2(sig,y2[i-1])+0x1FFFE;
		y2[i]=fd2((sig-0xFFFF),p);
		u[i]=fd2((y[i+1]-y[i]),(x[i+1]-x[i])) - 
			 fd2((y[i]-y[i-1]),(x[i]-x[i-1]));		 
			 
		u[i]= fd2( ( fm2(fm2(0x5FFFA,u[i]) , (x[i+1]-x[i-1])) - fm2(sig,u[i-1])), p);
	}
	if (ypn >2147483647)
		qn=un=0;
	else {
		qn=0x8000;
		un= fm2( ( fd2(0x2FFFD,(x[n]-x[n-1]))) , (ypn-fd2((y[n]-y[n-1]),(x[n]-x[n-1]))));
	}
	y2[n]=fd2((un-fm2(qn,u[n-1])),(fm2(qn,y2[n-1])+0xFFFF));
	for (k=n-1;k>=1;k--)
		y2[k]=fm2(y2[k],y2[k+1])+u[k];


}
*/

