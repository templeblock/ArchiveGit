#ifndef COLOR_TRAN_H
#define COLOR_TRAN_H

#include "mcostat.h"

void labtonxyz(double *LAB, double *xyz);
void labtonxyzinplace(double *LAB, long num);

//normalized xyz to LAB
void nxyztoLab(double *xyz, double *LAB);

//normalized xyz to LAB in place
void nxyztoLabinplace(double *xyz, long num);

//normalized xyz to specified xyz
void nxyztoxyz(double *inxyz, double *white, double *outxyz);

//normalized xyz to specified xyz
void nxyztoxyzinplace(double *inxyz, double *white, long size);


void labtolch(double *LAB, double *lch);

//added on 9/5
void lchtolab(double *lch, double *LAB);

//added on 9/27
void labtontscrgb(double *LAB, unsigned char *rgb);

//added by James on 1/22/96
void luvtolab(double *luv, double *lab);

//added by James on 1/22/96
void sxyztolab(double *Lxy, double *lab);

//rgb to normalized xyz
//mat is the rgb to nxyz conversion matrix
void rgbtonxyz(double *mat, double gamma, unsigned char* rgb, double *nxyz, long num);
void rgbtonxyzover(double *mat, double gamma, double *xyz, long num);
void rgbtonxyzover2(double *mat, double *rgammat, double *ggammat,
	double *bgammat, double *xyz, long num);

//normalized xyz to rgb
//mat is the nxyz to rgb conversion matrix
void nxyztorgb(double *mat, double gamma, double *nxyz, unsigned char* rgb, long num);
void nxyztorgb2(double *mat, double *rgammat, double *ggammat,
	double *bgammat, double *nxyz, unsigned char* rgb, long num);

McoStatus buildrgbxyz(double *monitor, double *xyztorgb, double *rgbtoxyz);

McoStatus newbuildrgbxyz(double *monitor, double *xyztorgb, double *rgbtoxyz);

#endif