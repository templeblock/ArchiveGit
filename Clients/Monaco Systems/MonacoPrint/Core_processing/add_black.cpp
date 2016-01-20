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
#include "monacoprint.h"
#include "add_black.h"
#include "find_black.h"
#include "colortran.h"

#include "spline.h"
#include "splint.h"

#include "picelin.h"

#include "spline3.h"
#include "spline.h"
#include "splint.h"
#include "interpolate.h"

#include "sort2.h"

#include "SLFunctions.h"


#define BLACK_STEP 50
#define SAT_ITERATION 5
#define  MAX_DIST 10

add_black::add_black(ProfileDoc *d,calib_base *cal)
{
// copy the parameters from the document structure
doc = d;
labTableH 			= doc->_labtableH;
cmykTableH 			= doc->_con_to_dev_tableH;
patchD 				= doc->patchD;
blackTableH 		= doc->_blacktableH;
gammutSurfaceH 		= doc->_gammutSurfaceH;
gammutSurfaceBlackH = doc->_gammutSurfaceBlackH;
gammutSurfaceManyBlackH 	= doc->_gammutSurfaceManyBlackH;
gammutSurfaceManyBlackLH 	= doc->_gammutSurfaceManyBlackLH;
out_of_gamutH 		= doc->_out_of_gamutH;

_rev_table_grid 	= doc->_rev_table_grid;
_rev_table_size 	= _rev_table_grid*_rev_table_grid*_rev_table_grid*_rev_table_grid;

minL 				= doc->_mingrayL;
maxInk 				= doc->_condata.total_ink_limit;
ucrgcr 				= doc->_condata.separation_type;	// ucr = 1, grc = 0

thermd = doc->thermd;

calib = cal;
bL = 0L;
bK = 0L;
a = 0L;
b = 0L;

double *BT;
int	i;

BT = (double*)McoLockHandle(blackTableH);
for (i=0; i<101; i++) blackT[i] = 100*BT[i];
doc->linear_data->CnvtLinear(blackT,101,3);
McoUnlockHandle(blackTableH);

}

add_black::~add_black(void) 
{
if (bL != 0L) delete bL;
if (bK != 0L) delete bK;
if (a != 0L) delete a;
if (b != 0L) delete b;


}

void add_black::setMinL(double mL)
{
minL = mL;
}

void add_black::setMinMaxL(double mingrayL, double maxgrayL)
{
	minL = mingrayL;
	//maxL = maxgrayL;
}

Boolean add_black::testInGammut(double *lab,double	range,double *dist)
{
double lch[3];
int32  L,hue,sat;

	*dist = 300;

	if (lab[0] < 0) return TRUE;
	if (lab[0] > 100) return TRUE;
	
	if (lab[1] < -128) return TRUE;
	if (lab[1] > 128) return TRUE;
	
	if (lab[2] < -128) return TRUE;
	if (lab[2] > 128) return TRUE;
	
	if (lab[0] + range < minLCMY) 
		{
		*dist = minLCMY-lab[0];
		return TRUE;
		}
	if (lab[0] - range > maxLCMY) 
		{
		*dist = lab[0]-maxLCMY;
		return TRUE;
		}

	labtolch(lab,lch);
	L = (int32)(lab[0]*2.55);
	
	hue = (int32)lch[2];
	
	sat = gammutSurface[L*4+hue*4*256+3];
	*dist = 0;
	if (sat + range >= lch[1]) return FALSE;
	
	*dist = lch[1] - sat;
	return TRUE;
	
}

void add_black::clipMaxInk(double *lab,double *cmyk)
{
int i;
double inksum = 0,cmysum = 0;
double sat,black_down,cmy_down;
double cmyk_out[4];

for (i=0; i<4; i++) 
	{
	cmyk_out[i] = doc->linear_data->CnvtLinearInv(cmyk[i],i);
	}


for (i=0; i<4; i++)
	{
	if (cmyk[i] <0) cmyk[i] = 0;
	if (cmyk[i] >100) cmyk[i] = 100;
	inksum += cmyk_out[i];
	if (i<3) cmysum += cmyk_out[i];
	}

if (inksum > maxInk)
	{
	sat = lab[1]*lab[1]+lab[2]*lab[2];
	black_down = 1 - exp(-sat*0.00091827);
	cmyk_out[3] -= (inksum-maxInk)*black_down;
	cmy_down = (inksum-maxInk)*(1-black_down);
	cmy_down = (cmysum-cmy_down)/cmysum;
	cmyk_out[0] = cmyk_out[0]*cmy_down;
	cmyk_out[1] = cmyk_out[1]*cmy_down;
	cmyk_out[2] = cmyk_out[2]*cmy_down;
	}

for (i=0; i<4; i++) 
	{
	cmyk[i] = doc->linear_data->CnvtLinear(cmyk_out[i],i);
	}

}


McoStatus add_black::compute(void)
{
	int32	i,j,k,m,n;
	double		*patchData;
	double	L1[27],a1[27],b1[27],sat[27],hue[27];
	double	Lt,at,bt,st;
	McoStatus	status;

	
	numB = patchD->format.numslow;
	
	patchData = (double*)McoLockHandle(patchD->dataH);
	
	bL = new double[numB];
	bK = new double[numB];
	a = new double[numB];
	b = new double[numB];
	
	for (i=0; i<numB; i++)
		{
		j = patchD->getDataIndex(i);
		// is always the first
		bK[i] = patchD->format.slowValues[i];
		bL[i] = patchData[j];
		a[i] = patchData[j+1];
		b[i] = patchData[j+2];
		}
		
	McoUnlockHandle(patchD->dataH);
	return MCO_SUCCESS;
}

// Smooth the K so that "bumps" are not created in smoothly shaded 
// areas
McoStatus add_black::smooth_K(double	*cmyk)
{
int32	h,i,j,k;
int32	l,m,n;
int32	p,q,r;
int32	s;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};
McoHandle	Ktable2H;
double	*cmyk_p;
double	*k_p;
double	lch[3],lch2[3];
long	ai1,bi1;
		
	cmyk_p = cmyk+3;
	Ktable2H = McoNewHandle(sizeof(double)*33*33*33);
	k_p = (double*)McoLockHandle(Ktable2H);
	

 	for (i=0; i<33; i++)
 		{
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
 				*k_p++ = *cmyk_p;
 				cmyk_p+=4;
				}
			}
		}

	cmyk_p = cmyk+3;
	k_p = (double*)McoLockHandle(Ktable2H);

	for (h=0; h<2; h++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 				i = 32*h;
 				if ((i == 0) && ((j>14) && ( j< 18)) && ((k>14) && ( k< 18))) 
 					cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
 				else 
 					{
					cmyk_p[i*4356+j*132+k*4] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i;
							m = j+q;
							n = k+r;
							s = abs(m-j)+abs(n-k);
							
						 	cmyk_p[i*4356+j*132+k*4] += b[s]*k_p[l*1089+m*33+n];
							}
					}
				}
			}
		}
		
 	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (k=1; k<32; k++)
 				{
 				j = 32*h;

				cmyk_p[i*4356+j*132+k*4] = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j;
						n = k+r;
						s = abs(l-i)+abs(n-k);
						
					 	cmyk_p[i*4356+j*132+k*4] += b[s]*k_p[l*1089+m*33+n];
						}
				}
			}
		}
	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (j=1; j<32; j++)
 				{
 				k = 32*h;

				cmyk_p[i*4356+j*132+k*4] = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j+r;
						n = k;
						s = abs(l-i)+abs(m-j);
						
						cmyk_p[i*4356+j*132+k*4] += b[s]*k_p[l*1089+m*33+n];
						}	
				}
			}
		}
		

 	for (i=1; i<32; i++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 //				if (((j>14) && ( j< 18)) && ((k>14) && ( k< 18)))	cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
 //				else
 					{
					cmyk_p[i*4356+j*132+k*4] = 0;
	 				for (p=-1; p<=1; p++)
	 					for (q=-1; q<=1; q++)
	 						for (r=-1; r<=1; r++)
	 							{
	 							l = i+p;
	 							m = j+q;
	 							n = k+r;
	 							s = abs(l-i)+abs(m-j)+abs(n-k);
	 							
								cmyk_p[i*4356+j*132+k*4] += a[s]*k_p[l*1089+m*33+n];
	 							
	 							}
					}
				}
			}
		}
		
	McoDeleteHandle(Ktable2H);
	return MCO_SUCCESS;
}

// Smooth the K so that "bumps" are not created in smoothly shaded 
// areas
McoStatus add_black::median_K(double	*cmyk)
{
int32	h,i,j,k,c;
int32	l,m,n;
int32	p,q,r;
int32	s;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};
McoHandle	Ktable2H;
double	*cmyk_p;
double	*k_p;
double	lch[3],lch2[3];
long	ai1,bi1;
double  values[27];
		
	cmyk_p = cmyk+3;
	Ktable2H = McoNewHandle(sizeof(double)*33*33*33);
	k_p = (double*)McoLockHandle(Ktable2H);
	
	//copy data
 	for (i=0; i<33; i++)
 		{
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
 				*k_p++ = *cmyk_p;
 				cmyk_p+=4;
				}
			}
		}

	cmyk_p = cmyk+3;
	k_p = (double*)McoLockHandle(Ktable2H);
	
	// note that edges are excluded
	
	// median two sides of the cube

	for (h=0; h<2; h++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 				i = 32*h;
 				c = 0;
 				if ((j==16) && (k==16))
 					cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
 				else 
 					{
 					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i;
							m = j+q;
							n = k+r;
							values[c++] = k_p[l*1089+m*33+n];
							}
					sort(9,values-1);
					if (fabs(k_p[i*1089+j*33+k]-values[5]) > 0.1)
						cmyk_p[i*4356+j*132+k*4] = values[5];
					else cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
					}
				}
			}
		}
		
	// median two sides of the cube	
		
 	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (k=1; k<32; k++)
 				{
 				j = 32*h;

				c = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j;
						n = k+r;
						values[c++] = k_p[l*1089+m*33+n];
						}
				sort(9,values-1);
				if (fabs(k_p[i*1089+j*33+k]-values[5]) > 0.1)
					cmyk_p[i*4356+j*132+k*4] = values[5];
				else cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
				}
			}
		}
		
	// median two sides of the cube	
		
	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (j=1; j<32; j++)
 				{
 				k = 32*h;
				c = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j+r;
						n = k;
						values[c++] = k_p[l*1089+m*33+n];
						}
				sort(9,values-1);
				if (fabs(k_p[i*1089+j*33+k]-values[5]) > 0.1)
					cmyk_p[i*4356+j*132+k*4] = values[5];
				else cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];

				}
			}
		}
		
	// do median for the enterior of the cube	

 	for (i=1; i<32; i++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{			
				c = 0;
				if ((j==16) && (k==16))
 					cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
 				else
 					{
	 				for (p=-1; p<=1; p++)
	 					for (q=-1; q<=1; q++)
	 						for (r=-1; r<=1; r++)
	 							{
	 							l = i+p;
	 							m = j+q;
	 							n = k+r;
	 							
								values[c++] = k_p[l*1089+m*33+n];
	 							
	 							}
					sort(27,values-1);
					if (fabs(k_p[i*1089+j*33+k]-values[13]) > 0.1)
						cmyk_p[i*4356+j*132+k*4] = values[13];
					else cmyk_p[i*4356+j*132+k*4] = k_p[i*1089+j*33+k];
					}
				}
			}
		}
		
	McoDeleteHandle(Ktable2H);
	return MCO_SUCCESS;
}


// Smooth the K so that "bumps" are not created in smoothly shaded 
// areas
McoStatus add_black::median_CMY(double *lab,double	*cmyk)
{
int32	h,i,j,k,c,d;
int32	l,m,n;
int32	p,q,r;
int32	s;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};
McoHandle	Ktable2H;
double	*cmyk_p;
double	*k_p;
double	lch[3],lch2[3];
long	ai1,bi1;
double  values_c[27];
double  values_m[27];
double  values_y[27];
		
	cmyk_p = cmyk;
	Ktable2H = McoNewHandle(sizeof(double)*33*33*33*4);
	if (!Ktable2H) return MCO_MEM_ALLOC_ERROR;
	k_p = (double*)McoLockHandle(Ktable2H);
	
	// copy data

 	for (i=0; i<33; i++)
 		{
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
 				*k_p++ = *cmyk_p++;
 				*k_p++ = *cmyk_p++;
 				*k_p++ = *cmyk_p++;
 				*k_p++ = *cmyk_p++;
				}
			}
		}

	cmyk_p = cmyk;
	k_p = (double*)McoLockHandle(Ktable2H);

	// smooth two sides

	for (h=0; h<2; h++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 				i = 32*h;
 				c = 0;

				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i;
						m = j+q;
						n = k+r;
						values_c[c] = k_p[l*4356+m*132+n*4];
						values_m[c] = k_p[l*4356+m*132+n*4+1];
						values_y[c++] = k_p[l*4356+m*132+n*4+2];
						}
				sort(9,values_c-1);
				sort(9,values_m-1);
				sort(9,values_y-1);
				if ((fabs(k_p[i*4356+j*132+k*4]-values_c[4]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+1]-values_m[4]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+2]-values_y[4]) > 8.0))
				   	{
					cmyk_p[i*4356+j*132+k*4] = values_c[4];
					cmyk_p[i*4356+j*132+k*4+1] = values_m[4];
					cmyk_p[i*4356+j*132+k*4+2] = values_y[4];
					calib->evalCMYSpace(&lab[i*3267+j*99+k*3],&cmyk_p[i*4356+j*132+k*4]);
					}	
				}
			}
		}
		
	// smooth two sides
		
 	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (k=1; k<32; k++)
 				{
 				j = 32*h;

				c = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j;
						n = k+r;
						values_c[c] = k_p[l*4356+m*132+n*4];
						values_m[c] = k_p[l*4356+m*132+n*4+1];
						values_y[c++] = k_p[l*4356+m*132+n*4+2];
						}
				sort(9,values_c-1);
				sort(9,values_m-1);
				sort(9,values_y-1);
				if ((fabs(k_p[i*4356+j*132+k*4]-values_c[4]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+1]-values_m[4]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+2]-values_y[4]) > 8.0))
				   	{
					cmyk_p[i*4356+j*132+k*4] = values_c[4];
					cmyk_p[i*4356+j*132+k*4+1] = values_m[4];
					cmyk_p[i*4356+j*132+k*4+2] = values_y[4];
					calib->evalCMYSpace(&lab[i*3267+j*99+k*3],&cmyk_p[i*4356+j*132+k*4]);
					}	
				}
			}
		}
		
	// smooth two sides	
		
	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (j=1; j<32; j++)
 				{
 				k = 32*h;
				c = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j+r;
						n = k;
						values_c[c] = k_p[l*4356+m*132+n*4];
						values_m[c] = k_p[l*4356+m*132+n*4+1];
						values_y[c++] = k_p[l*4356+m*132+n*4+2];
						}
				sort(9,values_c-1);
				sort(9,values_m-1);
				sort(9,values_y-1);
				if ((fabs(k_p[i*4356+j*132+k*4]-values_c[4]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+1]-values_m[4]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+2]-values_y[4]) > 8.0))
				   	{
					cmyk_p[i*4356+j*132+k*4] = values_c[4];
					cmyk_p[i*4356+j*132+k*4+1] = values_m[4];
					cmyk_p[i*4356+j*132+k*4+2] = values_y[4];
					calib->evalCMYSpace(&lab[i*3267+j*99+k*3],&cmyk_p[i*4356+j*132+k*4]);
					}	
				}
			}
		}
		
	// smooth the inside of the cube	

 	for (i=1; i<32; i++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{			
				c = 0;
 				for (p=-1; p<=1; p++)
 					for (q=-1; q<=1; q++)
 						for (r=-1; r<=1; r++)
 							{
 							l = i+p;
 							m = j+q;
 							n = k+r;
 							
							values_c[c] = k_p[l*4356+m*132+n*4];
							values_m[c] = k_p[l*4356+m*132+n*4+1];
							values_y[c++] = k_p[l*4356+m*132+n*4+2];
 							
 							}
				sort(27,values_c-1);
				sort(27,values_m-1);
				sort(27,values_y-1);
				if ((fabs(k_p[i*4356+j*132+k*4]-values_c[12]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+1]-values_m[12]) > 8.0) ||
				   (fabs(k_p[i*4356+j*132+k*4+2]-values_y[12]) > 8.0))
				   	{
					cmyk_p[i*4356+j*132+k*4] = values_c[12];
					cmyk_p[i*4356+j*132+k*4+1] = values_m[12];
					cmyk_p[i*4356+j*132+k*4+2] = values_y[12];
					calib->evalCMYSpace(&lab[i*3267+j*99+k*3],&cmyk_p[i*4356+j*132+k*4]);
					}	
				}
			}
		}
		
	McoDeleteHandle(Ktable2H);
	return MCO_SUCCESS;
}


// Smooth the cmy so that "bumps" are not created in smoothly shaded 
// areas
McoStatus add_black::smooth_CMY(double	*cmyk)
{
int32	h,i,j,k;
int32	l,m,n;
int32	p,q,r;
int32	s;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};
McoHandle	Ktable2H;
double	*cmyk_p;
double	*k_p;
double	lch[3],lch2[3];
long	ai1,bi1;
		
	cmyk_p = cmyk;
	Ktable2H = McoNewHandle(sizeof(double)*33*33*33*3);
	k_p = (double*)McoLockHandle(Ktable2H);
	

 	for (i=0; i<33; i++)
 		{
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
 				*k_p++ = *cmyk_p++;
 				*k_p++ = *cmyk_p++;
 				*k_p++ = *cmyk_p++;
 				cmyk_p++;
				}
			}
		}

	cmyk_p = cmyk;
	k_p = (double*)McoLockHandle(Ktable2H);

	for (h=0; h<2; h++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 				i = 32*h;
 				if ((i == 0) && ( j == 16) && (k == 16)) 
 					{
 					cmyk_p[i*4356+j*132+k*4] = k_p[i*3267+j*99+k*3];
 					cmyk_p[i*4356+j*132+k*4+1] = k_p[i*3267+j*99+k*3+1];
 					cmyk_p[i*4356+j*132+k*4+2] = k_p[i*3267+j*99+k*3+2];
 					}
 				else 
 					{
					cmyk_p[i*4356+j*132+k*4] = 0;
					cmyk_p[i*4356+j*132+k*4+1] = 0;
					cmyk_p[i*4356+j*132+k*4+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i;
							m = j+q;
							n = k+r;
							s = abs(m-j)+abs(n-k);
							
						 	cmyk_p[i*4356+j*132+k*4] += b[s]*k_p[l*3267+m*99+n*3];
						 	cmyk_p[i*4356+j*132+k*4+1] += b[s]*k_p[l*3267+m*99+n*3+1];
						 	cmyk_p[i*4356+j*132+k*4+2] += b[s]*k_p[l*3267+m*99+n*3+2];
							}
					}
				}
			}
		}
		
 	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (k=1; k<32; k++)
 				{
 				j = 32*h;

				cmyk_p[i*4356+j*132+k*4] = 0;
				cmyk_p[i*4356+j*132+k*4+1] = 0;
				cmyk_p[i*4356+j*132+k*4+2] = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j;
						n = k+r;
						s = abs(l-i)+abs(n-k);
						
					 	cmyk_p[i*4356+j*132+k*4] += b[s]*k_p[l*3267+m*99+n*3];
					 	cmyk_p[i*4356+j*132+k*4+1] += b[s]*k_p[l*3267+m*99+n*3+1];
					 	cmyk_p[i*4356+j*132+k*4+2] += b[s]*k_p[l*3267+m*99+n*3+2];
						}
				}
			}
		}
	for (h=0; h<2; h++)
 		{
 		for (i=1; i<32; i++)
 			{
 			for (j=1; j<32; j++)
 				{
 				k = 32*h;

				cmyk_p[i*4356+j*132+k*4] = 0;
				cmyk_p[i*4356+j*132+k*4+1] = 0;
				cmyk_p[i*4356+j*132+k*4+2] = 0;
				for (q=-1; q<=1; q++)
					for (r=-1; r<=1; r++)
						{
						l = i+q;
						m = j+r;
						n = k;
						s = abs(l-i)+abs(m-j);
						
					 	cmyk_p[i*4356+j*132+k*4] += b[s]*k_p[l*3267+m*99+n*3];
					 	cmyk_p[i*4356+j*132+k*4+1] += b[s]*k_p[l*3267+m*99+n*3+1];
					 	cmyk_p[i*4356+j*132+k*4+2] += b[s]*k_p[l*3267+m*99+n*3+2];
						}	
				}
			}
		}
		

 	for (i=1; i<32; i++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{			
				cmyk_p[i*4356+j*132+k*4] = 0;
				cmyk_p[i*4356+j*132+k*4+1] = 0;
				cmyk_p[i*4356+j*132+k*4+2] = 0;
 				for (p=-1; p<=1; p++)
 					for (q=-1; q<=1; q++)
 						for (r=-1; r<=1; r++)
 							{
 							l = i+p;
 							m = j+q;
 							n = k+r;
 							s = abs(l-i)+abs(m-j)+abs(n-k);
 							
						 	cmyk_p[i*4356+j*132+k*4] += a[s]*k_p[l*3267+m*99+n*3];
						 	cmyk_p[i*4356+j*132+k*4+1] += a[s]*k_p[l*3267+m*99+n*3+1];
						 	cmyk_p[i*4356+j*132+k*4+2] += a[s]*k_p[l*3267+m*99+n*3+2];
	 							
 							}

				}
			}
		}
		
	McoDeleteHandle(Ktable2H);
	return MCO_SUCCESS;
}

McoStatus saveLABTiff(McoHandle labH, long width, long length);


// find the CMYK values when many black patches are available
McoStatus add_black::evalMB(double minGam, double maxGam)
{
	double K;

	double 		L2;
	double		*lab,lab2[3],lch[3];
	double		*cmyk;
	Boolean 	NotInGammut;
	
	int32	c,i,j,k,l,m,n;
	int32 	cl,ml,yl;

	double  *tx,*ty;
	double 	yd1,ydn;
	double	s_n,s_n2;
	double	*S2;
	
	double	lp1,lp2;
	
	McoStatus	status = MCO_SUCCESS;
	double		dev,dev_p,dev_s;
	double		calc_L,calc_sat,calc_hue;

	double		m_s;
	double		m_l;
	FindBlack  *findblack;
	int32	calib_start;
	double		K_goal,K_inc;
	double 		dist;
	double		min_dist;
	int32		min_dist_K;
	double		K_best;
	double		mL;
	double		minK,maxK;
	double		max_dist;
	double    	check_dist;
	short		gamut = 0;
	
	if (ucrgcr == 1.0) K_inc = 0.01;
	else K_inc = -0.01;

	//saveLABTiff(labTableH, 1089,  33);
		
	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);	
	lab = (double*)McoLockHandle(labTableH);
	cmyk = (double*)McoLockHandle(cmykTableH);		
	
	
	// see if initilization has occured
	if (bK == 0L) status = compute();
	if (status != MCO_SUCCESS) return status;
	
	// get the minimum L from the table
	mL = lab[544*3];
	findblack = new FindBlack(bL,bK, a, b, numB,ucrgcr,mL,doc->_maxgrayL,blackT,gammutSurfaceManyBlackH,gammutSurfaceManyBlackLH,minGam,maxGam,calib);	
	if (!findblack) return MCO_MEM_ALLOC_ERROR;
	if (findblack->error) {status = findblack->error; goto bail;}
		
	calib->start();
	check_dist = calib->getSpacing();
			
		
	// find the black first
	for( i = 0; i < 33; i++)
		{
		for( j = 0; j < 1089; j++)
			{
			NotInGammut = TRUE;
			calib->getMinMaxK(lab,&minK,&maxK,&gamut);
			K_goal = findblack->eval(lab,minK,maxK);
			K = K_goal;
			
			// check to see if too much has been added
			min_dist = 1E20;
			K_best = K;
		//	if (K > 400)
			if ((K > 2) || (!gamut))
				{
				K_best = K;
				dist = calib->getDistance(lab,K,check_dist);
				if ((i==0) && (j == 544) && (dist < MAX_DIST)) K_best = K_goal;
				else if ((dist > check_dist) || (!gamut))
					{
					max_dist = MinVal(dist,MAX_DIST);
					K = maxK;
					c = 0;
					do {
						dist = calib->getDistance(lab,K,max_dist);
						if (dist < min_dist)
							{
							K_best = K;
							min_dist = dist;
							}
						if ((dist > max_dist) && (K >= minK)) K -= 4;
						c++;
						if ((c%10== 0) && (thermd != 0L)) if (!thermd->DisplayTherm(2000+i*75,Process_BuildingTable)) goto bail;
						} while ((K > minK) && (dist > max_dist));
					}
				K = K_best;
				} 
			if ((j%66 == 0) && (thermd != 0L)) if (!thermd->DisplayTherm(2000+i*75,Process_BuildingTable)) goto bail;
			if (K < 0) K = 0;
			cmyk[3] = K;	// set K
			lab += 3;
			cmyk += 4;
			}
		}
	// Now smooth the K to prevent bumps
	cmyk = (double*)McoLockHandle(cmykTableH);
	lab = (double*)McoLockHandle(labTableH);	
	
	median_K(cmyk);
	smooth_K(cmyk);
	smooth_K(cmyk);
//	smooth_K(cmyk);

	SLDisable();
	
	// now calculate the CMY values
	for( i = 0; i < 33; i++)
		{
		if (i >31) SLEnable();
		for( j = 0; j < 1089; j++)
			{
			if ((j%66 == 0) && (thermd != 0L)) if (!thermd->DisplayTherm(4500+i*167,Process_BuildingTable)) goto bail;
			calib->eval(lab,cmyk,1,&dist);
			clipMaxInk(lab,cmyk);
			lab += 3;
			cmyk += 4;
			}			
		}

	cmyk = (double*)McoLockHandle(cmykTableH);
	lab = (double*)McoLockHandle(labTableH);	
		
//	test_debug();	

	median_CMY(lab,cmyk);
		
//	smooth_CMY(cmyk);
//	smooth_CMY(cmyk);
	
		
	McoUnlockHandle(cmykTableH);
	McoUnlockHandle(labTableH);
	McoUnlockHandle(gammutSurfaceH);

	delete findblack;
	

	
	calib->end();
	
	return MCO_SUCCESS;

	
bail:

	if (calib) calib->end();
	
	if (findblack) delete findblack;
	
	McoUnlockHandle(cmykTableH);
	McoUnlockHandle(labTableH);
	McoUnlockHandle(gammutSurfaceH);
	
	
	return MCO_CANCEL;
}



// find the CMYK values when the K only switch is active
McoStatus add_black::eval_K_Only(double *lab_K,double *cmyk_K,int32 num)
{
	double K;

	double 		L2;
	double		*lab,lab2[3],lch[3];
	double		*cmyk;
	Boolean 	NotInGammut;
	
	int32	c,d,i,j,k,l,m,n;
	int32 	cl,ml,yl;

	double  *tx,*ty;
	double 	yd1,ydn;
	double	s_n,s_n2;
	double	*S2;
	
	double	lp1,lp2;
	
	McoStatus	status = MCO_SUCCESS;
	double		dev,dev_p,dev_s;
	double		calc_L,calc_sat,calc_hue;

	double		m_s;
	double		m_l;
	FindBlack  *findblack;
	double		K_goal,K_inc;
	double 		dist;
	double		min_dist;
	int32		min_dist_K;
	double		K_best;
	double		labt[3];
	double		cmykt[4];
	double 		sort_c[5],sort_m[5],sort_y[5],sort_k[5];
	double		Labd[3];
	
	if (ucrgcr == 1.0) K_inc = 0.01;
	else K_inc = -0.01;


	//saveLABTiff(labTableH, 1089,  33);
		
	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);	
	lab = lab_K;
	cmyk = cmyk_K;

	
	// see if initilization has occured
	if (bK == 0L) status = compute();
	if (status != MCO_SUCCESS) return status;

	// Now set the last entry to 0,0,0,100	
	cmyk_K[(num-1)*4] = 0;
	cmyk_K[(num-1)*4+1] = 0;
	cmyk_K[(num-1)*4+2] = 0;
	cmyk_K[(num-1)*4+3] = 100;
	
	calib->start();
	
	// set the lab of the last entry to the lab value for 0,0,0,100
	calib->convtCMYK_LAB(&cmyk_K[(num-1)*4],labt);
			
	// now modify the curve to get a smooth transition	
	
	Labd[0] = labt[0]-lab_K[(num-1)*3];
	Labd[1] = labt[1]-lab_K[(num-1)*3+1];
	Labd[2] = labt[2]-lab_K[(num-1)*3+2];
	for (i=(num/2); i<num; i++)
		{
		lab_K[i*3] = lab_K[i*3] + ((double)i-num/2)/(-1+num-num/2)*Labd[0];
		lab_K[i*3+1] = lab_K[i*3+1] + ((double)i-num/2)/(-1+num-num/2)*Labd[1];
		lab_K[i*3+2] = lab_K[i*3+2] + ((double)i-num/2)/(-1+num-num/2)*Labd[2];	
		}
	
		//(1-((double)i-num/2)/(-1+num-num/2))*
	// first build with as much black as possible	
	for( i = 0; i < num-1; i++)
		{
		K_goal = 100;
		K = K_goal;
			
		// now take out the black
		min_dist = 1E20;
		K_best = K;

		do {
			cmyk[3] = K;
			calib->eval(lab,cmyk,1,&dist);
			if (dist < min_dist)
				{
				K_best = K;
				min_dist = dist;
				}
			if (dist > 10) K -= 1.0;
			} while ((K >= 0) && (dist > 10));
		K = K_best;
		if (thermd != 0L) if (!thermd->DisplayTherm(i,Process_BuildingTable)) goto bail;
		if (K < 0) K = 0;
		cmyk[3] = K;	// set K
		calib->eval(lab,cmyk,1,&dist);
		lab += 3;
		cmyk += 4;
		}
		
	// now fix any reversals that occur between table entries by smoothing

	for( i = 2; i < num-2; i++)
		{
		for ( j = -2; j<=2; j++)
			{
			sort_c[j+2] = cmyk_K[(i+j)*4];
			sort_m[j+2] = cmyk_K[(i+j)*4+1];
			sort_y[j+2] = cmyk_K[(i+j)*4+2];
			sort_k[j+2] = cmyk_K[(i+j)*4+3];
			}
		sort(5,sort_c-1);
		sort(5,sort_m-1);
		sort(5,sort_y-1);
		sort(5,sort_k-1);
		if ((fabs(cmyk_K[i*4]  -sort_c[2]) > 15.0) ||
			(fabs(cmyk_K[i*4+1]-sort_m[2]) > 15.0) ||
			(fabs(cmyk_K[i*4+2]-sort_y[2]) > 15.0) ||
			(fabs(cmyk_K[i*4+3]-sort_k[2]) > 15.0))
			{
			cmyk_K[i*4] =   (sort_c[1] + sort_c[2] + sort_c[3])/3.0;
			cmyk_K[i*4+1] = (sort_m[1] + sort_m[2] + sort_m[3])/3.0;
			cmyk_K[i*4+2] = (sort_y[1] + sort_y[2] + sort_y[3])/3.0;
			cmyk_K[i*4+3] = (sort_k[1] + sort_k[2] + sort_k[3])/3.0;
			calib->eval(&lab_K[i*3],&cmyk_K[i*4],1,&dist);
			}
		}
			
/*	for (i=num-2; i>=0; i--)
		{
		
		do {	
			c = 0;
			d = 0;
			for (j=0; j<10; j++) // interpolate the cmyk values
				{
				cmykt[0] = cmyk_K[i*4]*(1-(double)j/9)+cmyk[i*4+4]*(double)j/9;
				cmykt[1] = cmyk_K[i*4+1]*(1-(double)j/9)+cmyk[i*4+5]*(double)j/9;
				cmykt[2] = cmyk_K[i*4+2]*(1-(double)j/9)+cmyk[i*4+6]*(double)j/9;
				cmykt[3] = cmyk_K[i*4+3]*(1-(double)j/9)+cmyk[i*4+7]*(double)j/9;
				// convert to lab
				calib->convtCMYK_LAB(cmykt,labt);
				// look for reversal
				if (labt[0] > lab_K[i*3]) 
					{
					cmyk_K[i*4+3] = 0.9*cmyk_K[i*4+3]+0.1*cmyk_K[i*4+7];
					calib->eval(&lab_K[i*3],&cmyk_K[i*4],1,&dist);
					c ++;
					d = 1;
					break;
					}
				}
			}while ((c<10) && (d == 1));
		}
*/		
	
bail:
	calib->end();
	
	McoUnlockHandle(cmykTableH);
	McoUnlockHandle(labTableH);
	McoUnlockHandle(gammutSurfaceH);
	
	
	return MCO_CANCEL;
}




// The following function finds the minimum L value for the given max K and the max ink.

McoStatus add_black::findMinLMB(double *lab)
{
	double		lab2[3],lch[3];
	double		K;
	double		L;
	Boolean		AboveMaxInk = TRUE;
	Boolean		NotInGammut	= TRUE;
	Boolean		OverMax = FALSE;
	McoStatus	status = MCO_SUCCESS;
	double		*tx,*ty;
	double		calc_L,calc_sat,calc_hue;
	double		cmyk[4];
	double		inksum;
	double		s_n,s_n2;
	double		lp1,lp2;
	int			k;
	double		dist;
	double		min_dist = 1E30;
	double		min_dist_L;
	
	gammutSurface = (double*)McoLockHandle(gammutSurfaceH);	
		
		// see if initilization has occured
	//if (bK == 0L) status = compute();
	if (status != MCO_SUCCESS) return status;

	lab[0] = 0;
	lab[1] = 0;
	lab[2] = 0;
	
	
	K = blackT[100];
	
	
	L = 0;
	
	calib->start();
	
	while ((AboveMaxInk)  && (L <=100))
		{
		lab[0] = L;
		cmyk[3] = K;
		
		calib->eval(lab,cmyk,1,&dist);
		
		OverMax = FALSE;
		if (cmyk[0] > 100.0) OverMax = TRUE;
		if (cmyk[1] > 100.0) OverMax = TRUE;
		if (cmyk[2] > 100.0) OverMax = TRUE;
		if (OverMax) 
			{
			L = L + 0.05;
			}
		else 
			{
			inksum = cmyk[0];
			inksum += cmyk[1];
			inksum += cmyk[2];	
			inksum += cmyk[3];
			if (inksum <= maxInk) 
				{
				if (dist < min_dist) 
					{
					min_dist = dist;
					min_dist_L = L;
					}
				if (dist < 3.0) AboveMaxInk = FALSE;
				else L = L + 0.05;
				}
			else L = L + 0.05;
			}
		}	
		
calib->end();
lab[0] = min_dist_L;
return MCO_SUCCESS;
}
	