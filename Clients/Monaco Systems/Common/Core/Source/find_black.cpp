//////////////////////////////////////////////////////////////////////////////////////////
//   find_black.c																		//
//	 given L, a, b, ucr & gcr, measured L for K, and the user black curve, and the max  //
//   black,  find the amount of black to add.											//
//   This black level is not the final black level.										//
//																						//
//	 Feb 21, 1996																		//
//   By James Vogh																		//
//////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "spline.h"
#include "splint.h"
#include "find_black.h"
#include "picelin.h"
#include "colortran.h"
#include "spline3.h"
#include "spline2.h"



FindBlack::FindBlack(double *bL, double *bK, double *a, double *b, int32 nB, 
					 double u_g, double miL, double maL, double *bT,
					 McoHandle gsmbH,McoHandle gsmbLH,double minGam, double maxGam,calib_base *cal)
{
double	*tx,*ty,yd1,ydn;
int i;
double *gammutSurfaceManyBlack;
double *gammutSurfaceManyBlackL;
double	gam_dim_k_vals[] = GAM_DIM_K_VALS;

	_gammutSurfaceManyBlackH = gsmbH;
	_gammutSurfaceManyBlackLH = gsmbLH;

	numB = nB;
	
	calib = cal;
	
	blackL = 0L;
	
	
/*	blackL = new double[numB];
	for (i=0; i<numB; i++)
		{
		blackL[i] = 100-bL[i];
		}
*/	
	//blackL = bL;
	blackK = bK;
	blacka = a;
	blackb = b;

//	tx = blackL;
	ty = blackK;
	
	K2 = new double[numB];
	if (!K2) goto bail;
	a2 = new double[numB];
	if (!a2) goto bail;
	b2 = new double[numB];
	if (!b2) goto bail;
	
//	yd1=(ty[2]-ty[1])/(tx[2]-tx[1])/4;
//	ydn=(ty[numB-1]-ty[numB-2])/(tx[numB-1]-tx[numB-2])/4;
	
//	spline(tx-1,ty-1,numB,yd1,ydn,K2-1);
	
	tx = blackK;
	ty = blacka;
	
	yd1=(ty[2]-ty[1])/(tx[2]-tx[1])/4;
	ydn=(ty[numB-1]-ty[numB-2])/(tx[numB-1]-tx[numB-2])/4;
	
	spline(tx-1,ty-1,numB,yd1,ydn,a2-1);
	
	tx = blackK;
	ty = blackb;
	
	yd1=(ty[2]-ty[1])/(tx[2]-tx[1])/4;
	ydn=(ty[numB-1]-ty[numB-2])/(tx[numB-1]-tx[numB-2])/4;
	
	spline(tx-1,ty-1,numB,yd1,ydn,b2-1);
	
	ucrgcr = u_g;
	//ucrgcr = u_g;
	
	minL = miL;
	maxL = maL;
	blackTable = bT;
	
	y2a = new double[GAM_DIM_K*50*72];
	if (!y2a) goto bail;
	
	fixmanyBlackL();
	
	gammutSurfaceManyBlack = (double*)McoLockHandle(_gammutSurfaceManyBlackH);
	gammutSurfaceManyBlackL = (double*)McoLockHandle(_gammutSurfaceManyBlackLH);

	
	x1 = new double[GAM_DIM_K];
	if (!x1) goto bail;
	x2 = new double[72];
	if (!x2) goto bail;
	x3 = new double[72];
	if (!x3) goto bail;
	
	
	for (i=0; i<GAM_DIM_K; i++) 
		{
		x1[i] = gam_dim_k_vals[i]/100.0;
		}
	for (i=0; i<72; i++) x2[i] = (double)i*5.0;

	spline3_s3(x1,x2,gammutSurfaceManyBlackL,gammutSurfaceManyBlack,GAM_DIM_K,72,50, y2a);

	McoUnlockHandle(_gammutSurfaceManyBlackH);
	McoUnlockHandle(_gammutSurfaceManyBlackH);

	findNeutralMins();
	extendRange();
	
	error = MCO_SUCCESS;
	return;	
bail:
	error = MCO_MEM_ALLOC_ERROR;
	return;
}
	
	
FindBlack::~FindBlack()
{
	
if (K2 != 0L) delete K2;
if (a2 != 0L) delete a2;
if (b2 != 0L) delete b2;
//if (blackL != 0L) delete blackL;
if (y2a != 0L) delete y2a;
if (x1 != 0L) delete x1;
if (x2 != 0L) delete x2;
if (x3 != 0L) delete x3;
}

// fix the L values when saturation is zero
McoStatus FindBlack::fixmanyBlackL(void)
{
int i,j,k,l,sk;
double	h,a;
double	start_L,end_L;
double	*gammutSurfaceManyBlack,*gammutSurfaceManyBlackL;

gammutSurfaceManyBlack = (double*)McoLockHandle(_gammutSurfaceManyBlackH);
gammutSurfaceManyBlackL = (double*)McoLockHandle(_gammutSurfaceManyBlackLH);

for (i=0; i<GAM_DIM_K; i++)
	{
	for (j=0; j<72; j++)
		{
		for (k=0; k<50; k++)
			{
			if (gammutSurfaceManyBlack[i*3600+j*50+k] > 0) break;
			}
		if (k != 50)
			{
			sk = k;
			end_L = gammutSurfaceManyBlackL[i*3600+j*50+k];
			}
		for (k=49; k>=0; k--)
			{
			if (gammutSurfaceManyBlack[i*3600+j*50+k] > 0) break;
			}
		if ((k != -1) && (k != sk))
			{
			h = (k-sk);
			start_L = gammutSurfaceManyBlackL[i*3600+j*50+k];
			for (l=0; l<50; l++)
				{
				a = ((double)l-sk)/h;
				gammutSurfaceManyBlackL[i*3600+j*50+l] = end_L*(1-a)+start_L*a;
				}
			}
		else if (k == sk)
			{
			end_L = gammutSurfaceManyBlackL[i*3600+j*50+k]+2;
			start_L = gammutSurfaceManyBlackL[i*3600+j*50+k]-2;
			k +=1;
			sk -=1;
			h = (k-sk);
			for (l=0; l<50; l++)
				{
				a = ((double)l-sk)/h;
				gammutSurfaceManyBlackL[i*3600+j*50+l] = end_L*(1-a)+start_L*a;
				}
			}
		}
	}
McoUnlockHandle(_gammutSurfaceManyBlackH);
McoUnlockHandle(_gammutSurfaceManyBlackLH);
return MCO_SUCCESS;
}

#define bigger(a, b) ((a >= b) ? a :b)

// The code is lch based and fails to perform correctly when the lab value is neutral
// therefore create a special set of data for neutral colors
// this is created by finding the distance with the getdistance function
McoStatus FindBlack::findNeutralMins(void)
{
int i,j,k;
double lab[3];
double cmyk[4];
double dist;


for (i=0; i<GAM_DIM_K; i++)
	{	
	for (k=0; k<50; k++)
		{
		lab[0] = k*2;
		lab[1] = 0;
		lab[2] = 0;
		dist = calib->getDistance(lab,100*x1[i],0);
		if (dist == 0) neutralmins[k+i*50] = 1000;
		else if (dist < 10) neutralmins[k+i*50] = 1/dist;
		else  neutralmins[k+i*50] = 0;
		}
	}

return MCO_SUCCESS;
}

/*
McoStatus FindBlack::findNeutralMins(void)
{
int i,j,k;
double *gammutSurfaceManyBlack;
double *gammutSurfaceManyBlackL;
double y2[50];
double sat;


gammutSurfaceManyBlack = (double*)McoLockHandle(_gammutSurfaceManyBlackH);
gammutSurfaceManyBlackL = (double*)McoLockHandle(_gammutSurfaceManyBlackLH);

for (i=0; i<GAM_DIM_K; i++)
	{
	for (j=0; j<72; j++)
		{
		for (k=0; k<50; k++) neutralmins[k+i*50] = 0;
		spline_0(gammutSurfaceManyBlackL+i*50*72+j*50,gammutSurfaceManyBlack+i*50*72+j*50,50,1e30,1e30,y2);
		for (k=0; k<50; k++)
			{
			splint_0_clip(gammutSurfaceManyBlackL+i*50*72+j*50,gammutSurfaceManyBlack+i*50*72+j*50,y2,50,(double)k*2,&sat);
			neutralmins[k+i*50] = bigger(neutralmins[k+i*50],sat);
			}
		}
	}

McoUnlockHandle(_gammutSurfaceManyBlackH);
McoUnlockHandle(_gammutSurfaceManyBlackLH);
return MCO_SUCCESS;
}
*/

// extend the range of gammutsurfacemany black by convolving with a Gaussian kernal
// interpolation errors often reduce the range of the gamut, this extends that range back out
McoStatus FindBlack::extendRange(void)
{
int i,j,k,l,m;
int count;
double *gammutSurfaceManyBlack;
double kernal[5];
double sum[6];
double temp[50];
double a;

gammutSurfaceManyBlack = (double*)McoLockHandle(_gammutSurfaceManyBlackH);

for (i=0; i<6; i++) sum[i] = 0.0;

// create the kernal
for (i=-2; i<3; i++) 
	{
	kernal[i+2] = exp(-(double)(i*i)*0.9);
	sum[5] += kernal[i+2];
	}
// the sums at the edges
sum[3] = 1+kernal[1]+kernal[0];
sum[4] = 1+kernal[1]*2+kernal[0];

for (i=0; i<GAM_DIM_K; i++)
	{
	// for all 72 hues
	for (j=0; j<72; j++)
		{
		// a temp buffer
		for (k=0; k<50; k++) temp[k] = gammutSurfaceManyBlack[k+j*50+i*50*72];
		for (k=0; k<50; k++)
			{
			count = 0;
			a = 0;
			for (l=-2; l<3; l++)
				{
				m = l+k;
				if ((m >=0) && (m <50))
					{
					count ++;
					a += temp[m]*kernal[l+2];
					}
				}
			gammutSurfaceManyBlack[k+j*50+i*50*72] = a/sum[count];
			}
		}
	// now do the same for the neutral data
	for (k=0; k<50; k++) temp[k] = neutralmins[k+i*50];
	for (k=0; k<50; k++)
		{
		count = 0;
		a = 0;
		for (l=-2; l<3; l++)
			{
			m = l+k;
			if ((m >=0) && (m <50))
				{
				count ++;
				a += temp[m]*kernal[l+2];
				}
			}
		neutralmins[k+i*50] = a/sum[count];
		}	
	}
McoUnlockHandle(_gammutSurfaceManyBlackH);
return MCO_SUCCESS;
}
					


McoStatus FindBlack::findMinMaxOpt(double *lab, double *Kmin,double *Kmax,double *Kopt)
{
int32 i,km,kp,c;
double *gammutSurfaceManyBlack;
double *gammutSurfaceManyBlackL;
double x23[2];
double lch[3];
double x11[101]; // find 100 black values
double y[101];
double x2n[50];
double y2n[GAM_DIM_K*50];
Boolean start = TRUE;
double	best_s;
double Kmint,Kmaxt;

Kmint = -1;
Kmaxt = -1;

labtolch(lab,lch);

km = floor(*Kmin);
kp = ceil(*Kmax);

if ((lab[1]*lab[1]+lab[2]*lab[2]) <= 1.0)
	{
	
	for (i=0; i<50; i++) x2n[i] = (double)i*2;
	
	for (i=km; i<=kp; i++) x11[i] = (double)i/100;
	
	c = 1+kp-km;
	
	spline2(x1,x2n,neutralmins,GAM_DIM_K,50,y2n);
	splint2_clip(x1,x2n,neutralmins,y2n,GAM_DIM_K,50,lab[0],x11,c,y);
	}
else
	{

	gammutSurfaceManyBlack = (double*)McoLockHandle(_gammutSurfaceManyBlackH);
	gammutSurfaceManyBlackL = (double*)McoLockHandle(_gammutSurfaceManyBlackLH);

	x23[0] = lch[2];
	x23[1] = lab[0];


	for (i=km; i<=kp; i++) x11[i] = (double)i/100;
	
	c = 1+kp-km;

	splint3_s3_clip(x1,x2,gammutSurfaceManyBlackL,gammutSurfaceManyBlack,y2a,GAM_DIM_K,72,50,x23,x11,c,y);

	McoUnlockHandle(_gammutSurfaceManyBlackH);
	McoUnlockHandle(_gammutSurfaceManyBlackLH);
	}
best_s = 0;

c = 0;


for (i=km; i<=kp; i++)
	{
	if ((y[c] > 0) && (y[c] + 0.8 > lch[1])) // insert a small tolerence here
		{
		if (start) 
			{
			Kmint = (double)i;
			start = FALSE;
			}
		Kmaxt = (double)i;
		}
	if (y[c] > best_s) 
		{
		best_s = y[c];
		*Kopt = (double)i;
		}
	c++;
	}
*Kmin = Kmint;
*Kmax = Kmaxt;	
	
	
if (*Kmin == -1) *Kmin = *Kopt;	
if (*Kmax == -1) *Kmax = *Kopt;	

return MCO_SUCCESS;
}

double percIn1(double x);
double percIn1(double x)
{
double t;

if (x == 0) return 0;
//x = x/100;

t = 1/(x*x*x*x*x*x);


return ((0.05/(1+15625*t)));
}

double percIn2(double x);
double percIn2(double x)
{
double t;

if (x == 0) return 0;

t = 1/(x*x*x*x*x);


return ((0.075/(1+15625*t)));
}

double FindBlack::eval(double *lab,double minK,double maximumk)
{
double K;
double Kmin,Kmax,Kopt;
double Kmin2,Kmax2;
int32  KI;
double sat;
double aK,bK;
double diff;
double *tx,*ty;
double L;
int32 Li;

//if (ucrgcr == 1.0) return 0.0;
//if (ucrgcr == 2.0) return 1.0;

//	tx = blackL;
//	ty = blackK;
	
//	piceLin(tx-1,ty-1,K2-1,numB,100-lab[0],&K);
	//splint(tx-1,ty-1,K2-1,numB,lab[0],&K);
	
//	if (K < 0) K = 0;
//	if (K > 1) K = 1;
//	K = K*100;
	K = 100;
	L = lab[0];
	L = 100*(L-minL)/(maxL -minL);
	Li = (int32)(100-L);
	if (Li < 0) Li = 0;
	if (Li > 100) Li = 100;
	
	K = K*blackTable[Li]/100;
	
	if (blackTable[Li] == 0.0) return 0.0;
	
// find the a and b of the K and calculate difference in color
	
	tx = blackK;
	ty = blacka;
	
	splint(tx-1,ty-1,a2-1,numB,K,&aK);
	
	tx = blackK;
	ty = blackb;
	
	splint(tx-1,ty-1,b2-1,numB,K,&bK);
	
	diff = (aK - lab[1])*(aK-lab[1])+(bK-lab[2])*(bK-lab[2]);
	
	// do the ucr/gcr calculation
	
	// K = K*exp(-(diff*ucrgcr*ucrgcr));
	
	// find the min,max, and optimal K for the Lab value
	// optimal is that level of K where the hue for Lab has the most saturation
	
	Kmin = Kmax = Kopt = K;
	
	Kmin = minK;
	Kmax = maximumk;
	
	findMinMaxOpt(lab,&Kmin,&Kmax,&Kopt);
	
	if (ucrgcr == 0)
		{
		Kmin2 = (percIn1(Kmin)*Kopt+(1-percIn1(Kmin))*Kmin);
		Kmax2 = (percIn1(100-Kmax)*Kopt+(1-percIn1(100-Kmax))*Kmax);

		if (K < Kmin2) K = Kmin2;
		if (K > Kmax2) K = Kmax2;
		}
	else 
		{
		Kmin2 = (percIn2(Kmin)*Kopt+(1-percIn2(Kmin))*Kmin);
		Kmax2 = (percIn2(100-Kmax)*Kopt+(1-percIn2(100-Kmax))*Kmax);

		if (K < Kmin2) K = Kmin2;
		if (K > Kmax2) K = Kmax2;
		}

	
	if (K > blackTable[100]) K = blackTable[100];
	
	return K;
}
	
	
double FindBlack::maxK()
{
return blackTable[100]/100.0;
}
