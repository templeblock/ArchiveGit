#pragma once
//////////////////////////////////////////////////////////////////////////////////////////
//   find_blacklab.h																	//
//	 given a K and measured L,a,b for K, find the Lab for the specified K				//
//	 based on find_balck.c																//
//	 Mar 3, 1996																		//
//   By James Vogh																		//
//////////////////////////////////////////////////////////////////////////////////////////

#include "mcotypes.h"
#include "mcomem.h"

class FindBlackLab {
private:
protected:
double *blackL, *blackK, *blacka, *blackb,*L2,*a2,*b2;
double *blackTable;
double ucrgcr;
double maxBlack;
int32  numB;
public:

FindBlackLab(double *bL, double *bK, double *a, double *b, int32 nB);
~FindBlackLab();
void eval(double k,double *lab);
};