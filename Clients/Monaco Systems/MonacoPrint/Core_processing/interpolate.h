#ifndef _INTERPOLATE_H
#define _INTERPOLATE_H

void linearinterp3Dreg(double *src, long srcs, double *des, long dess);

//void linearinterp3D(double *src, long srcs, double *des, long dess);
void linearinterp3D(double *src, double *sgap, long srcs, 
	double *des, double *dgap, long dess);

//linear interpolation in 1D case
//src_points is number of points in one interpolation node
void linearinterp1D(double *src, double *sgap, long srcs, double *des, 
double *dgap, long dess, long src_points);

void linearinterp4to3over(unsigned char *table, long gridp, unsigned char *src, long srcsize);
void linearinterp3to4(unsigned char *table, long gridp, unsigned char *src, long srcsize, unsigned char *dest);
void linearinterp3to4d(double *table, long gridp, unsigned char *src, long srcsize, double *dest);
void linearinterp3to3s(unsigned short *table, long gridp, unsigned char *src, long srcsize, double *dest);
void linearinterp3to3c(unsigned char *table, long gridp, unsigned char *src, long srcsize, double *dest);

// interpolate 3 to 3 where both are double
// and use table where table is 33x33x33

void interpolate_33(double *lab,double *labc,double *table);


void spline2d(double *xa1, double *xa2, double *ya, long m, long n, double *y2a);
void splint2d(double *xa1, double *xa2, double *ya, long m, long n, double *y2a, double *ask, double *ans);
void spline3d(double *xa1, double *xa2, double *xa3, double *ya, long m, long n, long p, double *y2a);
void splint3d(double *xa1, double *xa2, double *xa3, double *ya, long m, long n, long p, double *y2a, double *ask, double *ans);

#endif
