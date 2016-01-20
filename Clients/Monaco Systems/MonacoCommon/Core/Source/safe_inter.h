//////////////////////////////////////////////////////////////////////////
//  safe_inter.h														//
//  interpolate with spline or picewise linear based on the number of 	//
//  points																//
//  James Vogh															//
//////////////////////////////////////////////////////////////////////////



void sinterp_init(double *x,double *y,int n,double yp1,double ypn,double *y2);

void sinterp_eval(double *xa,double *ya,double *y2a,int n,double x,double *y);
