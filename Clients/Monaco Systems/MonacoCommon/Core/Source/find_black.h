#pragma once
//////////////////////////////////////////////////////////////////////////////////////////
//   find_black.c																		//
//	 given L, a, b, ucr & gcr, measured L for K and the user black curve				//
//   find the amount of black to add													//
//																						//
//	 Feb 21, 1996																		//
//   By James Vogh																		//
//////////////////////////////////////////////////////////////////////////////////////////

#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"
#include "monacoprint.h"
#include "calib_multi.h"

class FindBlack {
private:
protected:
double *blackL, *blackK, *blacka, *blackb,*K2,*a2,*b2;
double *blackTable;
double ucrgcr;
double minL,maxL;
int32  numB;
McoHandle _gammutSurfaceManyBlackH;
McoHandle _gammutSurfaceManyBlackLH;
double	*y2a;
double *x1,*x2,*x3;
double neutralmins[GAM_DIM_K*50];
calib_base *calib;


public:
McoStatus error;
FindBlack(double *bL, double *bK, double *a, double *b, int32 nB, double u_g, double miL, double maL,double *bTH,McoHandle gsmbH,McoHandle gsmbLH,double minGam, double maxGam,calib_base *cal);
~FindBlack();
McoStatus fixmanyBlackL(void);
// The code is lch based and fails to perform correctly when the lab value is neutral
McoStatus findNeutralMins(void);
// extend the range of gammutsurfacemany black by convolving with a Gaussian kernal
// interpolation errors often reduce the range of the gamut, this extends that range back out
McoStatus extendRange(void);
McoStatus findMinMaxOpt(double *lab, double *Kmin,double *Kmax,double *Kopt);
double eval(double *lab,double minK,double maxk);
double maxK();
};