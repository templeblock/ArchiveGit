//////////////////////////////////////////////////////////////////////////////////////////
//   find_blacklab.c																	//
//	 given a K and measured L,a,b for K, find the Lab for the specified K				//
//	 based on find_balck.c																//
//	 Mar 3, 1996																		//
//   By James Vogh																		//
//////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "spline.h"
#include "splint.h"
#include "find_blacklab.h"


FindBlackLab::FindBlackLab(double *bL, double *bK, double *a, double *b, int32 nB)
{
double	*tx,*ty,yd1,ydn;

	numB = nB;
	blackL = bL;
	blackK = bK;
	blacka = a;
	blackb = b;

	tx = blackK;
	ty = blackL;
	
	L2 = new double[numB];
	a2 = new double[numB];
	b2 = new double[numB];
	
	yd1=(ty[1]-ty[0])/(tx[1]-tx[0])/4;
	ydn=(ty[numB-1]-ty[numB-2])/(tx[numB-1]-tx[numB-2])/4;
	
	spline_0(tx,ty,numB,yd1,ydn,L2);
	
	tx = blackK;
	ty = blacka;
	
	yd1=(ty[1]-ty[0])/(tx[1]-tx[0])/4;
	ydn=(ty[numB-1]-ty[numB-2])/(tx[numB-1]-tx[numB-2])/4;
	
	spline_0(tx,ty,numB,yd1,ydn,a2);
	
	tx = blackK;
	ty = blackb;
	
	yd1=(ty[1]-ty[0])/(tx[1]-tx[0])/4;
	ydn=(ty[numB-1]-ty[numB-2])/(tx[numB-1]-tx[numB-2])/4;
	
	spline_0(tx,ty,numB,yd1,ydn,b2);

}
	
	
FindBlackLab::~FindBlackLab()
{
	
if (L2 != 0L) delete L2;
if (a2 != 0L) delete a2;
if (b2 != 0L) delete b2;
}

void FindBlackLab::eval(double K,double *lab)
{
int32  KI;
double sat;
double aK,bK;
double diff;
double *tx,*ty;

	tx = blackK;
	ty = blackL;

	splint_0(tx,ty,L2,numB,K,&lab[0]);
	
	tx = blackK;
	ty = blacka;
	
	splint_0(tx,ty,a2,numB,K,&lab[1]);
	
	tx = blackK;
	ty = blackb;
	
	splint_0(tx,ty,b2,numB,K,&lab[2]);
	
}
	
