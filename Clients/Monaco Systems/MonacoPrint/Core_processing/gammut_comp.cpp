////////////////////////////////////////////////////////////////////////////
//	gammut_comp.c														  //
//																		  //
//	calibrate by building multiple calibrations and and blending the 	  //
//  calibrations together												  //
//																		  //
//  Created on Feb 16, 1996												  //
//  By James Vogh														  //
////////////////////////////////////////////////////////////////////////////

#include "colortran.h"
#include "xyztorgb.h"
#include "monacoprint.h"
#include "gammut_comp.h"
#include "cmysurface.h"
#include "spline.h"
#include "splint.h"
#include "SLFunctions.h"


#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

#ifdef IN_MAC_OS_CODE

#include "waitforegrd.h"
#include "errordial.h"

#endif


#define Max_dist 20

McoStatus saveGammutSurfaceAsTiff(double *labh);
McoStatus saveGammutSurfaceAsTiffSmall(double *labh);

//maxgray and mingray is the max and min gray value of the 
GammutComp::GammutComp(RawData *pD,McoHandle lH,McoHandle lcH,
	McoHandle ogH,McoHandle gsH,McoHandle gsbH, McoHandle gsmbH,McoHandle gsmbLH, 
	McoHandle invTH,double sa, double pw,double cc,double ll,
	double maxgray, double mingray,double smaxgray, double smingray, double inkMinL,calib_base *cb,
	ThermObject	*td)
{
patchD = pD;
_labtableH = lH;
_labtableCopyH = lcH;

_out_of_gamutH = ogH;
_gammutSurfaceH = gsH;
_gammutSurfaceBlackH = gsbH;
_gammutSurfaceManyBlackH = gsmbH;
_gammutSurfaceManyBlackLH = gsmbLH;

_inverseTableH = 0L;


_SaturationAmount = sa; // The distance to move into the gammut surface
_PaperWhite = pw;	   // The amount to move towards the color of the paper
_col_bal_cut = cc; 		// The paper color cutoff point

_maxgrayL = maxgray;
_mingrayL = mingray;
	
_maxrequiredgrayL = smaxgray;
_minrequiredgrayL = smingray;

_inkMinL = inkMinL;

_l_linear = ll;

_gammutExp = 1.2;	// the default amount of gammut expansion

calib = cb;

thermd = td;

_indexlut = 0L;

_buildMinMaxCurve();
}

GammutComp::~GammutComp(void)
{
if (_inverseTableH) McoDeleteHandle(_inverseTableH);
if (min_max_curve) delete min_max_curve;
if (min_max_curve_sm) delete min_max_curve_sm;
if (_indexlut) delete _indexlut;
}


// build a Lab curve that goes from the absolute lightest point to the absolute darkest point
McoStatus GammutComp::_buildMinMaxCurve(void)
{
int i,j,k,l;
double	*lab,*labh;
double	paperlab[3];
double  L,A,B;
double  cut,fl;
double	tempLab[3];



	cut = (double)(_col_bal_cut)/50.0;
	cut = 10*cut*cut;

	patchD->getPaperLab(tempLab);
	A = tempLab[1];
	B = tempLab[2];

		
	A = A * _PaperWhite/100.0;
	B = B * _PaperWhite/100.0;
		


	min_max_curve = new double[256*3];
	min_max_curve_sm = new double[50*3];



for (i=0; i<256; i++)
	{
	L = 1-((double)i)/255.0;
	fl = exp(-cut*L);
	min_max_curve[i*3] = (double)i/2.55;
	min_max_curve[i*3+1] = fl*A;
	min_max_curve[i*3+2] = fl*B;	
	}
	
for (i=0; i<50; i++)
	{
	L = 1-((double)i)/49.0;
	fl = exp(-cut*L);
	min_max_curve_sm[i*3] = (double)i*2.0408163265;
	min_max_curve_sm[i*3+1] = fl*A;
	min_max_curve_sm[i*3+2] = fl*B;	
	}
		
return MCO_SUCCESS;
}



//dmaxgrayL and dmingrayL are device lightness gamut	
short 	GammutComp::need_lightness_stretch(double* dmaxgrayL, double *dmingrayL)
{
	double delta;
	double mingrayL, maxgrayL;
	
	maxgrayL = *dmaxgrayL;
	mingrayL = *dmingrayL;
	
	//if the required lightness is inside the device lightness gamut
	//return no stretch
	if(_maxrequiredgrayL <= maxgrayL && _minrequiredgrayL >= mingrayL)
		return 0;
		
	//if the required lightness is outside the device lightness gamut
	//return no stretch		
	if(_maxrequiredgrayL >= maxgrayL && _minrequiredgrayL <= mingrayL)	
		return 1;

	//if the required hightlight is outside the device lightness gamut
	if(_maxrequiredgrayL >= maxgrayL && _minrequiredgrayL >= mingrayL){
		if( (maxgrayL - mingrayL) >= (_maxrequiredgrayL - _minrequiredgrayL) )
			 *dmingrayL += (maxgrayL - mingrayL) - (_maxrequiredgrayL - _minrequiredgrayL); 
		return 1;
	}
	else{
		if( (maxgrayL - mingrayL) >= (_maxrequiredgrayL - _minrequiredgrayL) )
			*dmaxgrayL -= (maxgrayL - mingrayL) - (_maxrequiredgrayL - _minrequiredgrayL); 
		return 1;
	}							 	
}


// create data for black == 10,30,40,60,80 by interpolating existing data
// fill in data for 0,20,50,70,90
McoStatus GammutComp::interpolateBlack(void)
{
int i;
McoStatus state;
double	*inverseTable;
double	*vals;
double	*valsK;
double	gam_dim_k_vals[] = GAM_DIM_K_VALS;

	_inverseTableH = McoNewHandle(GAM_DIM_CUBE*GAM_DIM_K*3*sizeof(double));
	if (_inverseTableH == 0L) return MCO_MEM_ALLOC_ERROR;
	inverseTable = (double*)McoLockHandle(_inverseTableH);

	vals = new double[GAM_DIM];
	for (i=0; i<GAM_DIM; i++) 
		{
		vals[i] = 100*(double)i/(GAM_DIM-1);
		}
		
	valsK = new double[GAM_DIM_K];
	for (i=0; i<GAM_DIM_K; i++) 
		{
		valsK[i] = gam_dim_k_vals[i];
		}	
		
	state = patchD->doFourDInterp(inverseTable,GAM_DIM,GAM_DIM_K,vals,valsK);
	McoUnlockHandle(_inverseTableH);
	delete vals;
	delete valsK;

	return state;
}	
				
	
// Create a gammut surface buffer by interpolating values returned by findboundarynew
McoStatus GammutComp::get_gamut_surface(void)
{
int c,i,j,k,cnt;
McoHandle gammutSurfaceLRH = 0L;
McoHandle gammutSurfaceBlackLRH = 0L;
double	  *gammutSurface, *gammutSurfaceBlack;
double	  *gammutSurfaceLR, *gammutSurfaceBlackLR;
double	  inlch[3];
double	  inlab[3];
double	  inlab2[3];
double	  boundlab[3];
double 	  boundlch[3];
double	  boundcmy[3];
int16	  yes;
McoStatus state;
int		a,b,ai1,bi1,ai2,bi2;
double	t,u;
double  l;
double  L,A,B;
double	dist;
double  min_dist;
double	dist1,dist2,dist3,dist4,dist5;
double	maxgrayL2,mingrayL2,mingrayL3;
double	*i_patch;
double  mindist;
double *gammutSurfaceManyBlack = 0L;
double *gammutSurfaceManyBlackL = 0L;
Boolean IgnoreBad = FALSE;
int32	singCount = 1;
double	maxL_lab[3];
double	minL_lab[3];
double	*patch;
int32	L_index;

double	sat;
double	cmyk[4];
double	gam_dim_k_vals[] = GAM_DIM_K_VALS;
double	space_dist;

	
	patch = (double*)McoLockHandle(patchD->dataH);
	
	// first find the starting points
	maxL_lab[0] = 100;
	maxL_lab[1] = min_max_curve[255*3+1];
	maxL_lab[2] = min_max_curve[255*3+2];
	
	minL_lab[0] = 0;
	minL_lab[1] = min_max_curve[1];
	minL_lab[2] = min_max_curve[2];


	Cmysurface **cmysurface = 0L;

	cmysurface = new Cmysurface*[GAM_DIM_K];
	if(!cmysurface) return MCO_MEM_ALLOC_ERROR;	
	for (i=0; i<GAM_DIM_K; i++) cmysurface[i] = 0L;
	cmysurface[0] = new Cmysurface(patchD->format.cubedim[0], patch,maxL_lab,minL_lab);
	if(!cmysurface[0]) return MCO_MEM_ALLOC_ERROR;
	McoUnlockHandle(patchD->dataH);
// get the min and max

//	i_patch = (double*)McoLockHandle(_inverseTableH);
//	cmysurface[0] = new Cmysurface(GAM_DIM,i_patch,maxL_lab,minL_lab);

	cmysurface[0]->getMinMax(_maxgrayL,_mingrayL);
	
	if (patchD->type == CMYKPatches) interpolateBlack();
// find the black by searching over all of the patches
	if (patchD->type == CMYKPatches)
		{
		i_patch = (double*)McoLockHandle(_inverseTableH);
		mingrayL3 = _mingrayL;
		for (i=1; i<GAM_DIM_K; i++)
			{
			cmysurface[i] = new Cmysurface(GAM_DIM,i_patch+i*GAM_DIM_CUBE*3,maxL_lab,minL_lab);
			if (!cmysurface[i]) {state = MCO_MEM_ALLOC_ERROR; goto bail;}
			cmysurface[i]->getMinMax(maxgrayL2,mingrayL2);
			if (maxgrayL2 > _maxgrayL) _maxgrayL = maxgrayL2;
			if (mingrayL2 < mingrayL3) mingrayL3 = mingrayL2;
			}

		}

		
	
	McoUnlockHandle(_inverseTableH);
	
	patch = (double*)McoLockHandle(patchD->dataH);	
	for (i=0; i<patchD->format.total; i++)
		{
		if (patch[i*3]<_mingrayL) _mingrayL = patch[i*3];
		}
	McoUnlockHandle(patchD->dataH);	
	
	if (patchD->type == RGBPatches) mingrayL3 = _mingrayL;
		
// set mingrayL3 to the value defined by the max ink

	if (_inkMinL > mingrayL3) mingrayL3 = _inkMinL;
	if (_inkMinL > _mingrayL) _mingrayL = _inkMinL;

// First find a low res version by using findboundary

	gammutSurfaceLRH = McoNewHandle(sizeof(double)*51*180*3);
	if (!gammutSurfaceLRH) {state = MCO_MEM_ALLOC_ERROR; goto bail;}
	gammutSurfaceLR = (double*)McoLockHandle(gammutSurfaceLRH);
	
	gammutSurfaceBlackLRH = McoNewHandle(sizeof(double)*51*180*3);
	if (!gammutSurfaceBlackLRH) {state = MCO_MEM_ALLOC_ERROR; goto bail;}	
	gammutSurfaceBlackLR = (double*)McoLockHandle(gammutSurfaceBlackLRH);
	
	if (_gammutSurfaceManyBlackH != 0L)
		gammutSurfaceManyBlack = (double*)McoLockHandle(_gammutSurfaceManyBlackH);

	if (_gammutSurfaceManyBlackLH != 0L)
		gammutSurfaceManyBlackL = (double*)McoLockHandle(_gammutSurfaceManyBlackLH);
 
 	SLDisable();
	
	if (calib != 0L) space_dist = calib->getSpacing();
	
	// build a description of the surface
	c = 0;
	inlch[1] = 200;
	if (calib != 0L) calib->start();
	if (patchD->type == RGBPatches) for(j = 0; j < 72; j++){
		if (j > 70) SLEnable();
		inlch[2] = j*5;	
		if (thermd && !thermd->DisplayTherm(j,Process_GammutSurface)) goto cancel_gs;
		for(i = 0; i < 50; i++){	
			inlch[0] = i*2.04082;		
			lchtolab(inlch, inlab);
			
			// the from color
			inlab[1] +=  min_max_curve_sm[i*3+1]; // shift towards paper color
			inlab[2] +=  min_max_curve_sm[i*3+2];
			// the too color
			inlab2[0] = mingrayL3 + 0.1 + inlab[0] * (_maxgrayL - 0.1 - mingrayL3)/100;
			inlab2[1] = min_max_curve_sm[i*3+1];
			inlab2[2] = min_max_curve_sm[i*3+2];
			yes = -1;		
			state = cmysurface[0]->findBoundaryNoNorm(inlab,inlab2,boundlab, boundcmy, &yes,_gammutExp,singCount);
			if (state != MCO_SUCCESS) 
				{
#ifdef IN_MAC_OS_CODE
				if (!IgnoreBad) 
					{
					if (McoMessAlert(MCO_BAD_PROFILE,"\p")) goto bail;
					IgnoreBad = TRUE;
					singCount = 0x7FFFFFFF;
					}
#endif
				}
			if ((calib != 0L) && (yes))
				{
				dist = calib->getDistance(boundlab,0,space_dist);
			/*	if (dist > Max_dist)
					{
					sat = sqrt(dist);
					labtolch(boundlab,boundlch);
					boundlch[1] -= sat;
					lchtolab(boundlch,boundlab);
					}			*/
				cnt = 0;
				while ((dist > space_dist) && (cnt < 10) && (boundlch[1] >0))
					{
					labtolch(boundlab,boundlch);
					boundlch[1] = boundlch[1] - space_dist*0.25;
					if (boundlch[1] < 0) boundlch[1] = 0;
					lchtolab(boundlch,boundlab);
					dist = calib->getDistance(boundlab,0,space_dist);
					cnt ++;
					} 
				}
			if (yes)
				{
				gammutSurfaceLR[c*3] = boundlab[0];
				gammutSurfaceLR[c*3+1] = boundlab[1];
				gammutSurfaceLR[c*3+2] = boundlab[2];
				}
			else
				{
				gammutSurfaceLR[c*3] = inlab2[0];
				gammutSurfaceLR[c*3+1] = 0;
				gammutSurfaceLR[c*3+2] = 0;
				}
			
			c++;
			
			}	
		}

//added by Peter on 4/15
//if the required gray scale is inside of the gamut of the device color space
//the lightness stretching is not necessary
//	stretch = need_lightness_stretch(&maxgrayL, &mingrayL3);
//end of modification
	
	minGam = mingrayL3 + 0.1;
	maxGam = _maxgrayL;
	
	if (singCount < 100) singCount = 5;
	

	
	SLDisable();
	
	if (patchD->type == CMYKPatches)
		{
		
		c = 0;
		inlch[1] = 200;
		for(j = 0; j < 72; j++){
			inlch[2] = j*5;	
			if (j > 70) SLEnable();
			for(i = 0; i < 50; i++){
				if (thermd && !thermd->DisplayTherm(72+j*11,Process_GammutSurface)) goto cancel_gs;
				inlch[0] = i*2.04082;	
				
				lchtolab(inlch, inlab);
				// the from color
				inlab[1] +=  min_max_curve_sm[i*3+1]; // shift towards paper color
				inlab[2] +=  min_max_curve_sm[i*3+2];
				// the too color
				inlab2[0] = mingrayL3 + 0.1 + inlab[0] * (_maxgrayL - 0.1 - mingrayL3)/100;
				inlab2[1] = min_max_curve_sm[i*3+1];
				inlab2[2] = min_max_curve_sm[i*3+2];
				yes = -1;
				mindist = 1E20;
				state = cmysurface[0]->findBoundaryNoNorm(inlab,inlab2,boundlab, boundcmy, &yes,_gammutExp,singCount);
				if (state != MCO_SUCCESS) 
					{
#ifdef IN_MAC_OS_CODE
					if (!IgnoreBad) 
						{
						if (McoMessAlert(MCO_BAD_PROFILE,"\p")) goto bail;
						IgnoreBad = TRUE;
						singCount = 0x7FFFFFFF;
						}
#endif
					}
				if ((calib != 0L) && (yes))
					{
					dist = calib->getDistance(boundlab,gam_dim_k_vals[0],space_dist);
				/*	if (dist > Max_dist)
						{
						sat = sqrt(dist);
						labtolch(boundlab,boundlch);
						boundlch[1] -= sat;
						lchtolab(boundlch,boundlab);
						}			*/
					cnt = 0;
					while ((dist > space_dist) && (cnt < 10) && (boundlch[1] >0))
						{
						labtolch(boundlab,boundlch);
						boundlch[1] = boundlch[1] - space_dist*0.25;
						if (boundlch[1] < 0) boundlch[1] = 0;
						lchtolab(boundlch,boundlab);
						dist = calib->getDistance(boundlab,gam_dim_k_vals[0],space_dist);
						cnt ++;
						} 
					}
				if (gammutSurfaceManyBlack != 0L)
					{
					if (yes) gammutSurfaceManyBlack[c] = sqrt((boundlab[1]*boundlab[1]) + (boundlab[2]*boundlab[2]));
					else gammutSurfaceManyBlack[c] = 0.0;
					gammutSurfaceManyBlackL[c] = boundlab[0];
					}
				if (yes)
					{
					 mindist = (inlab[0]-boundlab[0])*(inlab[0]-boundlab[0]) +
								 (inlab[1]-boundlab[1])*(inlab[1]-boundlab[1]) +
								 (inlab[2]-boundlab[2])*(inlab[2]-boundlab[2]);
					gammutSurfaceBlackLR[c*3] = boundlab[0];
					gammutSurfaceBlackLR[c*3+1] = boundlab[1];
					gammutSurfaceBlackLR[c*3+2] = boundlab[2];
					gammutSurfaceLR[c*3] = boundlab[0];
					gammutSurfaceLR[c*3+1] = boundlab[1];
					gammutSurfaceLR[c*3+2] = boundlab[2];
					}
				else
					{
					gammutSurfaceBlackLR[c*3] = inlab2[0];
					gammutSurfaceBlackLR[c*3+1] = 0;
					gammutSurfaceBlackLR[c*3+2] = 0;
					gammutSurfaceLR[c*3] = inlab2[0];
					gammutSurfaceLR[c*3+1] = 0;
					gammutSurfaceLR[c*3+2] = 0;
					}
				for (k=1; k<GAM_DIM_K; k++)
					{		
					state = cmysurface[k]->findBoundaryNoNorm(inlab,inlab2,boundlab, boundcmy, &yes,_gammutExp,singCount);
					if (state != MCO_SUCCESS) 
						{
						if (!IgnoreBad) 
							{
#if defined(_WIN32)
							goto bail;
#else
							if (McoMessAlert(MCO_BAD_PROFILE,"\p")) goto bail;
#endif
							IgnoreBad = TRUE;
							singCount = 0x7FFFFFFF;
							}
						}
					if ((calib != 0L) && (yes))
						{
						dist = calib->getDistance(boundlab,gam_dim_k_vals[k],space_dist);
					/*	if (dist > Max_dist)
							{
							sat = sqrt(dist);
							labtolch(boundlab,boundlch);
							boundlch[1] -= sat;
							lchtolab(boundlch,boundlab);
							} */
						cnt = 0;
						while ((dist > space_dist) && (cnt < 10) && (boundlch[1] >0))
							{
							labtolch(boundlab,boundlch);
							boundlch[1] = boundlch[1] - space_dist*0.25;
							if (boundlch[1] < 0) boundlch[1] = 0;
							lchtolab(boundlch,boundlab);
							dist = calib->getDistance(boundlab,gam_dim_k_vals[k],space_dist);
							cnt ++;
							} 
						} 
					if (gammutSurfaceManyBlack != 0L)
						{
						if (yes) gammutSurfaceManyBlack[c+3600*k] = sqrt((boundlab[1]*boundlab[1]) + (boundlab[2]*boundlab[2]));
						else gammutSurfaceManyBlack[c+3600*k] = 0.0;
						gammutSurfaceManyBlackL[c+3600*k] = boundlab[0];
						}
					if (yes) dist1 = (inlab[0]-boundlab[0])*(inlab[0]-boundlab[0]) +
									 (inlab[1]-boundlab[1])*(inlab[1]-boundlab[1]) +
									 (inlab[2]-boundlab[2])*(inlab[2]-boundlab[2]);
					else dist1 = 2E20; 
					if (dist1 < mindist)
						{
						mindist = dist1;
						gammutSurfaceBlackLR[c*3] = boundlab[0];
						gammutSurfaceBlackLR[c*3+1] = boundlab[1];
						gammutSurfaceBlackLR[c*3+2] = boundlab[2];
						}
					} 
				c++;
				
				}	
			}
		
		}
	if (calib != 0L) calib->end();	
	
		
	for (i=0; i<GAM_DIM_K; i++) if (cmysurface[i] != 0L) delete cmysurface[i];
	delete cmysurface;
	cmysurface = 0L;
	
	gammutSurface = (double*)McoLockHandle(_gammutSurfaceH);
	gammutSurfaceBlack = (double*)McoLockHandle(_gammutSurfaceBlackH);

//	saveGammutSurfaceAsTiffSmall(gammutSurfaceBlackLR);

// Now build the high res version by using interpolation

	for (j=0; j<360; j++)
		{
		if (j % 100  == 0) if (thermd && !thermd->DisplayTherm(900+j/3,Process_GammutSurface)) goto cancel_gs;	
		
		b = 5*(j/5);
		bi1 = b/5;
		bi2 = bi1+1;
		if (bi2 > 71) bi2 = 0;
		
		u = (double)(j - b)/5.0;
		
		min_dist = 100000000;
		for (i=0; i<256; i++)
			{
			l = 0.48*(float)i/2.55;
			a = l;
			ai1 = a;
			ai2 = ai1+1;
			t = (l - a);
			
			
			gammutSurface[i*4+j*4*256] = L = (1-t)*(1-u)*gammutSurfaceLR[bi1*3*50+3*ai1] + t*(1-u)*gammutSurfaceLR[bi1*3*50+3*ai2] +
				t*u*gammutSurfaceLR[bi2*3*50+3*ai2] + (1-t)*u*gammutSurfaceLR[bi2*3*50+3*ai1];
				
			A = (1-t)*(1-u)*gammutSurfaceLR[bi1*3*50+3*ai1+1] + t*(1-u)*gammutSurfaceLR[bi1*3*50+3*ai2+1] +
				t*u*gammutSurfaceLR[bi2*3*50+3*ai2+1] + (1-t)*u*gammutSurfaceLR[bi2*3*50+3*ai1+1];
			
			gammutSurface[i*4+j*4*256+1] = A;
				
			B = (1-t)*(1-u)*gammutSurfaceLR[bi1*3*50+3*ai1+2] + t*(1-u)*gammutSurfaceLR[bi1*3*50+3*ai2+2] +
				t*u*gammutSurfaceLR[bi2*3*50+3*ai2+2] + (1-t)*u*gammutSurfaceLR[bi2*3*50+3*ai1+2];
				
			gammutSurface[i*4+j*4*256+2] = B;	
			
			L_index = L*2.55;
			
			A += min_max_curve[L_index*3+1];			
			
			B += min_max_curve[L_index*3+2];
				
			gammutSurface[i*4+j*4*256+3] = sqrt(A*A + B*B);			
			
			 if (patchD->type == CMYKPatches)
				{
				gammutSurfaceBlack[i*4+j*4*256] = (1-t)*(1-u)*gammutSurfaceBlackLR[bi1*3*50+3*ai1] + t*(1-u)*gammutSurfaceBlackLR[bi1*3*50+3*ai2] +
					t*u*gammutSurfaceBlackLR[bi2*3*50+3*ai2] + (1-t)*u*gammutSurfaceBlackLR[bi2*3*50+3*ai1];
					
				A = (1-t)*(1-u)*gammutSurfaceBlackLR[bi1*3*50+3*ai1+1] + t*(1-u)*gammutSurfaceBlackLR[bi1*3*50+3*ai2+1] +
					t*u*gammutSurfaceBlackLR[bi2*3*50+3*ai2+1] + (1-t)*u*gammutSurfaceBlackLR[bi2*3*50+3*ai1+1];
				
				gammutSurfaceBlack[i*4+j*4*256+1] = A;
					
				B = (1-t)*(1-u)*gammutSurfaceBlackLR[bi1*3*50+3*ai1+2] + t*(1-u)*gammutSurfaceBlackLR[bi1*3*50+3*ai2+2] +
					t*u*gammutSurfaceBlackLR[bi2*3*50+3*ai2+2] + (1-t)*u*gammutSurfaceBlackLR[bi2*3*50+3*ai1+2];
					
				gammutSurfaceBlack[i*4+j*4*256+2] = B;	
				
				L_index = L*2.55;	
			
				A += min_max_curve[L_index*3+1];			
				
				B += min_max_curve[L_index*3+2];
					
				gammutSurfaceBlack[i*4+j*4*256+3] = sqrt(A*A + B*B);
				} 

			}	
		}
	
// dont need the low res version any more
	
//	saveGammutSurfaceAsTiff(gammutSurface);
	
	McoUnlockHandle(gammutSurfaceLRH);
	McoDeleteHandle(gammutSurfaceLRH);	
	
	McoUnlockHandle(gammutSurfaceBlackLRH);
	McoDeleteHandle(gammutSurfaceBlackLRH);	


	McoUnlockHandle(_gammutSurfaceH);
	McoUnlockHandle(_gammutSurfaceBlackH);
	if (_gammutSurfaceManyBlackH) McoUnlockHandle(_gammutSurfaceManyBlackH);
	if (_gammutSurfaceManyBlackLH) McoUnlockHandle(_gammutSurfaceManyBlackLH);
	
	McoUnlockHandle(patchD->dataH);
	
	if (cmysurface != 0L)
		{
		for (i=0; i<GAM_DIM_K; i++) if (cmysurface[i] != 0L) delete cmysurface[i];
		delete cmysurface;
		}

	return MCO_SUCCESS;


bail:
// user has canceled
cancel_gs:

	if (cmysurface != 0L)
		{
		for (i=0; i<GAM_DIM_K; i++) if (cmysurface[i] != 0L) delete cmysurface[i];
		delete cmysurface;
		}
	
	if (gammutSurfaceLRH) McoDeleteHandle(gammutSurfaceLRH);	
	
	if (gammutSurfaceBlackLRH) McoDeleteHandle(gammutSurfaceBlackLRH);	

	McoUnlockHandle(_gammutSurfaceH);
	McoUnlockHandle(_gammutSurfaceBlackH);
	
	McoUnlockHandle(patchD->dataH);
	
	return MCO_CANCEL; 
}




#ifdef powerc

#include "tiffformat.h"

McoStatus saveAsTiff(double *labh)
{
McoHandle	   labH;
char *labBuffer;
double *labBufferF;	
TiffFormat	*outtiff;
int i;
McoStatus status;
StandardFileReply soutReply;	
Str255		prompt = "\pSave the Current File As";
Str255		defaultfilename = "\p";
	u_int16		photometric;
	int16		compression;
	int16		fillorder;
	int16		orientation;
	int32		rowsperstrip;
	int32 		imagesize;
	u_int16		planarconfig;
	u_int16		resolutionunit;	
	u_int32		xresnumerator;
	u_int32		yresnumerator;
	u_int32		xresdenominator;
	u_int32		yresdenominator;	
	int16		end = 0;
	int32		c,j,k;

// Allocate the array


waitTillForeground();


labH = McoNewHandle(4096*3*sizeof(char));
labBuffer = (char *)McoLockHandle(labH);

// Open the Lab file


	outtiff = new TiffFormat;
	if ( outtiff == 0L )  return MCO_MEM_ALLOC_ERROR; 
	
// Get the buffer size
	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(soutReply.sfGood)
		status = outtiff->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, '8BIM', 'TIFF');
	else 
		{
		McoDeleteHandle(labH);
		delete outtiff;
		return MCO_SUCCESS;
		}
	if( status != MCO_SUCCESS )
		{
		McoDeleteHandle(labH);
		delete outtiff;
		return status;
		}
	
	if( (status = outtiff->setSamplesperpixel(  3   )) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff; return status; }
	if( (status = outtiff->setWidth(1089)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
	if( (status = outtiff->setLength(33)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff; return status; }
	if( (status = outtiff->setBitspersample(8)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		


	photometric = PHOTOMETRIC_LAB;	

	if( (status = outtiff->setPhotometric(photometric)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		
	compression = 1;	
	if( (status = outtiff->setCompression(compression)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
 
	rowsperstrip = 1089;
	
	if( (status = outtiff->setRowsperstrip(rowsperstrip)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		 
		 
	fillorder = 1; 
	if( (status = outtiff->setFillorder(fillorder)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status;}
	
	orientation = 1;
	if( (status = outtiff->setOrientation(orientation)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status;}
	
	planarconfig = 1;
	if( (status = outtiff->setPlanarconfig(planarconfig)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;  return status; }



	resolutionunit = 2;
	xresnumerator = 720000;  //720000
	xresdenominator = 10000; //Fix2Long( (parms->filterParameters.imageHRes - (Fixed) xresnumerator)*65536);
	
	yresnumerator = 720000;	//720000
	yresdenominator = 10000; //Fix2Long( (parms->filterParameters.imageVRes - (Fixed) yresnumerator)*65536);


	if( (status = outtiff->setResolution(resolutionunit, xresnumerator, xresdenominator,
		yresnumerator, yresdenominator)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status; }
		 
		 
	imagesize = 64*4096*3;
					
	if( (status = outtiff->setImagesize( imagesize )) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
 		 
 	
	if( (status = outtiff->prepareWrite()) != MCO_SUCCESS) {
 		 outtiff->closeFile(); delete outtiff;  return status; }
 		
 	
	outtiff->setReuse(TRUE);
 	
 	
//McoShowMem("\pAfter tiff5.4",FALSE);

	if( (status = outtiff->startWrite(labH, 1089*3) ) != MCO_SUCCESS) {
 		 outtiff->closeFile(); delete outtiff; return status; }
 		  		 
 	for (i=0; i<33; i++)
 		{
 		c = 0;
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
 				labBuffer[c*3] = (unsigned char)(labh[i*3267+j*99+k*3]*2.55);
				labBuffer[c*3+1] = (char)(labh[i*3267+j*99+k*3+1]);
				labBuffer[c*3+2] = (char)(labh[i*3267+j*99+k*3+2]);
				c++;
				}
			}
		status = (McoStatus)outtiff->continueWrite(&end);
		}
	
	outtiff->closeFile();
	delete outtiff;
	McoUnlockHandle(labH);
	McoDeleteHandle(labH);
	//McoUnlockHandle(labHandle);
	return status;
}

#else
McoStatus saveAsTiff(double *labh)
{
return MCO_SUCCESS;
}
#endif

// Smooth the lab space
McoStatus GammutComp::_smooth_Lab(double	*laborig,double *gamutS)
{
int32	h,i,j,k;
int32	l,m,n;
int32	p,q,r;
int32	s;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};
McoHandle	_labtable2H;
double	*labh;
double	*labh2;
double	lch[3],lch2[3];
long	ai1,bi1;
		
	labh = (double*)McoLockHandle(_labtableCopyH);
	labh2 = (double*)McoLockHandle(_labtableH);
	

 	for (i=0; i<33; i++)
 		{
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
	 			labh2[i*3267+j*99+k*3] = labh[i*3267+j*99+k*3];
				labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
				labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
				}
			}
		}

	for (h=0; h<2; h++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 				i = 32*h;
 				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
 				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{
 					labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i;
							m = j+q;
							n = k+r;
							s = abs(m-j)+abs(n-k);
							
						 	labh2[i*3267+j*99+k*3] += b[s]*labh[l*3267+m*99+n*3];
							labh2[i*3267+j*99+k*3+1] += b[s]*labh[l*3267+m*99+n*3+1];
							labh2[i*3267+j*99+k*3+2] += b[s]*labh[l*3267+m*99+n*3+2];
							}
					}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
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
 				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
 				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{
	 				labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i+q;
							m = j;
							n = k+r;
							s = abs(l-i)+abs(n-k);
							
						 	labh2[i*3267+j*99+k*3] += b[s]*labh[l*3267+m*99+n*3];
							labh2[i*3267+j*99+k*3+1] += b[s]*labh[l*3267+m*99+n*3+1];
							labh2[i*3267+j*99+k*3+2] += b[s]*labh[l*3267+m*99+n*3+2];
							}
					}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
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
 				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
 				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{
	 				labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i+q;
							m = j+r;
							n = k;
							s = abs(l-i)+abs(m-j);
							
						 	labh2[i*3267+j*99+k*3] += b[s]*labh[l*3267+m*99+n*3];
							labh2[i*3267+j*99+k*3+1] += b[s]*labh[l*3267+m*99+n*3+1];
							labh2[i*3267+j*99+k*3+2] += b[s]*labh[l*3267+m*99+n*3+2];
							}
					}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
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
  				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{				
					labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
	 				for (p=-1; p<=1; p++)
	 					for (q=-1; q<=1; q++)
	 						for (r=-1; r<=1; r++)
	 							{
	 							l = i+p;
	 							m = j+q;
	 							n = k+r;
	 							s = abs(l-i)+abs(m-j)+abs(n-k);
	 							
	 							labh2[i*3267+j*99+k*3] += a[s]*labh[l*3267+m*99+n*3];
								labh2[i*3267+j*99+k*3+1] += a[s]*labh[l*3267+m*99+n*3+1];
								labh2[i*3267+j*99+k*3+2] += a[s]*labh[l*3267+m*99+n*3+2];
	 							
	 							}
	 				}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
					}
				}
			}
		}
		
//	saveAsTiff(labh2);
	McoUnlockHandle(_labtableH);
	McoUnlockHandle(_labtableCopyH);
	return MCO_SUCCESS;
}

// Smooth the lab space
McoStatus GammutComp::_smooth_Lab2(double	*laborig,double *gamutS)
{
int32	h,i,j,k;
int32	l,m,n;
int32	p,q,r;
int32	s;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};
McoHandle	labtable2H;
double	*labh;
double	*labh2;
double	lch[3],lch2[3];
long	ai1,bi1;
		
	labtable2H = McoNewHandle(sizeof(double)*33*33*33*3);
	labh = (double*)McoLockHandle(labtable2H);
	labh2 = (double*)McoLockHandle(_labtableH);
	

 	for (i=0; i<33; i++)
 		{
 		for (j=0; j<33; j++)
 			{
 			for (k=0; k<33; k++)
 				{
	 			labh[i*3267+j*99+k*3] = labh2[i*3267+j*99+k*3];
				labh[i*3267+j*99+k*3+1] = labh2[i*3267+j*99+k*3+1];
				labh[i*3267+j*99+k*3+2] = labh2[i*3267+j*99+k*3+2];
				}
			}
		}

	for (h=0; h<2; h++)
 		{
 		for (j=1; j<32; j++)
 			{
 			for (k=1; k<32; k++)
 				{
 				i = 32*h;
 				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
 				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{
 					labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i;
							m = j+q;
							n = k+r;
							s = abs(m-j)+abs(n-k);
							
						 	labh2[i*3267+j*99+k*3] += b[s]*labh[l*3267+m*99+n*3];
							labh2[i*3267+j*99+k*3+1] += b[s]*labh[l*3267+m*99+n*3+1];
							labh2[i*3267+j*99+k*3+2] += b[s]*labh[l*3267+m*99+n*3+2];
							}
					}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
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
 				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
 				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{
	 				labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i+q;
							m = j;
							n = k+r;
							s = abs(l-i)+abs(n-k);
							
						 	labh2[i*3267+j*99+k*3] += b[s]*labh[l*3267+m*99+n*3];
							labh2[i*3267+j*99+k*3+1] += b[s]*labh[l*3267+m*99+n*3+1];
							labh2[i*3267+j*99+k*3+2] += b[s]*labh[l*3267+m*99+n*3+2];
							}
					}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
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
 				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
 				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{
	 				labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i+q;
							m = j+r;
							n = k;
							s = abs(l-i)+abs(m-j);
							
						 	labh2[i*3267+j*99+k*3] += b[s]*labh[l*3267+m*99+n*3];
							labh2[i*3267+j*99+k*3+1] += b[s]*labh[l*3267+m*99+n*3+1];
							labh2[i*3267+j*99+k*3+2] += b[s]*labh[l*3267+m*99+n*3+2];
							}
					}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
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
  				labtolch(&laborig[i*3267+j*99+k*3],lch);
 				ai1 = lch[0]*2.55;
				bi1 = lch[2];
				labtolch(&gamutS[ai1*4+bi1*4*256],lch2);
 				if (lch[1] + 2 > lch2[1])
 					{				
					labh2[i*3267+j*99+k*3] = 0;
					labh2[i*3267+j*99+k*3+1] = 0;
					labh2[i*3267+j*99+k*3+2] = 0;
	 				for (p=-1; p<=1; p++)
	 					for (q=-1; q<=1; q++)
	 						for (r=-1; r<=1; r++)
	 							{
	 							l = i+p;
	 							m = j+q;
	 							n = k+r;
	 							s = abs(l-i)+abs(m-j)+abs(n-k);
	 							
	 							labh2[i*3267+j*99+k*3] += a[s]*labh[l*3267+m*99+n*3];
								labh2[i*3267+j*99+k*3+1] += a[s]*labh[l*3267+m*99+n*3+1];
								labh2[i*3267+j*99+k*3+2] += a[s]*labh[l*3267+m*99+n*3+2];
	 							
	 							}
	 				}
				else
					{
					labh2[i*3267+j*99+k*3]   = labh[i*3267+j*99+k*3];
					labh2[i*3267+j*99+k*3+1] = labh[i*3267+j*99+k*3+1];
					labh2[i*3267+j*99+k*3+2] = labh[i*3267+j*99+k*3+2];
					}
				}
			}
		}
		
//	saveAsTiff(labh2);
	McoUnlockHandle(_labtableH);
	McoDeleteHandle(labtable2H);
	return MCO_SUCCESS;
}


void GammutComp::getMinMaxL(double &mingrayL,double &maxgrayL)
{
mingrayL = _mingrayL;
maxgrayL = _maxgrayL;
}


void GammutComp::getMinMaxG(double &mingrayL,double &maxgrayL)
{
mingrayL = minGam;
maxgrayL = maxGam;
}


double delta_angle(double a1,double a2)
{
double d;

d = a1 - a2;
if (d > 180) d -= 360;
if (d < 0-180) d += 360;
return d;
}



#ifdef powerc
McoStatus saveGammutSurfaceAsTiff(double *labh)
{
McoHandle	   labH;
char *labBuffer;
double *labBufferF;	
TiffFormat	*outtiff;
int i;
McoStatus status;
StandardFileReply soutReply;	
Str255		prompt = "\pSave the Current File As";
Str255		defaultfilename = "\p";
	u_int16		photometric;
	int16		compression;
	int16		fillorder;
	int16		orientation;
	int32		rowsperstrip;
	int32 		imagesize;
	u_int16		planarconfig;
	u_int16		resolutionunit;	
	u_int32		xresnumerator;
	u_int32		yresnumerator;
	u_int32		xresdenominator;
	u_int32		yresdenominator;	
	int16		end = 0;
	int32		c,j,k;
	double		*gammutSurface;

// Allocate the array

labH = McoNewHandle(360*3*sizeof(char));
labBuffer = (char *)McoLockHandle(labH);


// Open the Lab file


	outtiff = new TiffFormat;
	if ( outtiff == 0L )  return MCO_MEM_ALLOC_ERROR; 
	
// Get the buffer size

	waitTillForeground();
	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(soutReply.sfGood)
		status = outtiff->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, '8BIM', 'TIFF');
	else 
		{
		McoDeleteHandle(labH);
		delete outtiff;
		return MCO_SUCCESS;
		}
	if( status != MCO_SUCCESS )
		{
		McoDeleteHandle(labH);
		delete outtiff;
		return status;
		}
	
	if( (status = outtiff->setSamplesperpixel(  3   )) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff; return status; }
	if( (status = outtiff->setWidth(360)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
	if( (status = outtiff->setLength(256)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff; return status; }
	if( (status = outtiff->setBitspersample(8)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		


	photometric = PHOTOMETRIC_LAB;	

	if( (status = outtiff->setPhotometric(photometric)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		
	compression = 1;	
	if( (status = outtiff->setCompression(compression)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
 
	rowsperstrip = 360;
	
	if( (status = outtiff->setRowsperstrip(rowsperstrip)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		 
		 
	fillorder = 1; 
	if( (status = outtiff->setFillorder(fillorder)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status;}
	
	orientation = 1;
	if( (status = outtiff->setOrientation(orientation)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status;}
	
	planarconfig = 1;
	if( (status = outtiff->setPlanarconfig(planarconfig)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;  return status; }



	resolutionunit = 2;
	xresnumerator = 720000;  //720000
	xresdenominator = 10000; //Fix2Long( (parms->filterParameters.imageHRes - (Fixed) xresnumerator)*65536);
	
	yresnumerator = 720000;	//720000
	yresdenominator = 10000; //Fix2Long( (parms->filterParameters.imageVRes - (Fixed) yresnumerator)*65536);


	if( (status = outtiff->setResolution(resolutionunit, xresnumerator, xresdenominator,
		yresnumerator, yresdenominator)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status; }
		 
		 
	imagesize = 360*256*3;
					
	if( (status = outtiff->setImagesize( imagesize )) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
 		 
 	
	if( (status = outtiff->prepareWrite()) != MCO_SUCCESS) {
 		 outtiff->closeFile(); delete outtiff;  return status; }
 		
 	
	outtiff->setReuse(TRUE);
 	
 	
//McoShowMem("\pAfter tiff5.4",FALSE);

	if( (status = outtiff->startWrite(labH, 360*3) ) != MCO_SUCCESS) {
 		 outtiff->closeFile(); delete outtiff; return status; }
 		 	 
 	for (i=0; i<256; i++)
 		{
 		c = 0;
 		for (j=0; j<360; j++)
 			{
			labBuffer[c*3] = (unsigned char)(labh[i*4+j*4*256]*2.55);
			labBuffer[c*3+1] = (char)(labh[i*4+j*4*256+1]);
			labBuffer[c*3+2] = (char)(labh[i*4+j*4*256+2]);
			c++;
			}
		status = (McoStatus)outtiff->continueWrite(&end);
		}
	
	outtiff->closeFile();
	delete outtiff;
	McoUnlockHandle(labH);
	McoDeleteHandle(labH);
	return status;
}

McoStatus saveGammutSurfaceAsTiffSmall(double *labh)
{
McoHandle	   labH;
char *labBuffer;
double *labBufferF;	
TiffFormat	*outtiff;
int i;
McoStatus status;
StandardFileReply soutReply;	
Str255		prompt = "\pSave the Current File As";
Str255		defaultfilename = "\p";
	u_int16		photometric;
	int16		compression;
	int16		fillorder;
	int16		orientation;
	int32		rowsperstrip;
	int32 		imagesize;
	u_int16		planarconfig;
	u_int16		resolutionunit;	
	u_int32		xresnumerator;
	u_int32		yresnumerator;
	u_int32		xresdenominator;
	u_int32		yresdenominator;	
	int16		end = 0;
	int32		c,j,k;
	double		*gammutSurface;

// Allocate the array

labH = McoNewHandle(72*3*sizeof(char));
labBuffer = (char *)McoLockHandle(labH);


// Open the Lab file


	outtiff = new TiffFormat;
	if ( outtiff == 0L )  return MCO_MEM_ALLOC_ERROR; 
	
// Get the buffer size

	waitTillForeground();
	
	StandardPutFile(prompt, defaultfilename, &soutReply);
	if(soutReply.sfGood)
		status = outtiff->createFilewithtype(&(soutReply.sfFile), soutReply.sfScript, '8BIM', 'TIFF');
	else 
		{
		delete outtiff;
		return MCO_SUCCESS;
		}
	if( status != MCO_SUCCESS )
		{
		delete outtiff;
		return status;
		}
	
	if( (status = outtiff->setSamplesperpixel(  3   )) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff; return status; }
	if( (status = outtiff->setWidth(72)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
	if( (status = outtiff->setLength(50)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff; return status; }
	if( (status = outtiff->setBitspersample(8)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		


	photometric = PHOTOMETRIC_LAB;	

	if( (status = outtiff->setPhotometric(photometric)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		
	compression = 1;	
	if( (status = outtiff->setCompression(compression)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
 
	rowsperstrip = 72;
	
	if( (status = outtiff->setRowsperstrip(rowsperstrip)) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
		 
		 
	fillorder = 1; 
	if( (status = outtiff->setFillorder(fillorder)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status;}
	
	orientation = 1;
	if( (status = outtiff->setOrientation(orientation)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status;}
	
	planarconfig = 1;
	if( (status = outtiff->setPlanarconfig(planarconfig)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;  return status; }



	resolutionunit = 2;
	xresnumerator = 720000;  //720000
	xresdenominator = 10000; //Fix2Long( (parms->filterParameters.imageHRes - (Fixed) xresnumerator)*65536);
	
	yresnumerator = 720000;	//720000
	yresdenominator = 10000; //Fix2Long( (parms->filterParameters.imageVRes - (Fixed) yresnumerator)*65536);


	if( (status = outtiff->setResolution(resolutionunit, xresnumerator, xresdenominator,
		yresnumerator, yresdenominator)) != MCO_SUCCESS) {
		outtiff->closeFile(); delete outtiff;   return status; }
		 
		 
	imagesize = 72*50*3;
					
	if( (status = outtiff->setImagesize( imagesize )) != MCO_SUCCESS) {
		 outtiff->closeFile(); delete outtiff;  return status; }
 		 
 	
	if( (status = outtiff->prepareWrite()) != MCO_SUCCESS) {
 		 outtiff->closeFile(); delete outtiff;  return status; }
 		
 	
	outtiff->setReuse(TRUE);
 	
 	
//McoShowMem("\pAfter tiff5.4",FALSE);

	if( (status = outtiff->startWrite(labH, 72*3) ) != MCO_SUCCESS) {
 		 outtiff->closeFile(); delete outtiff; return status; }
 		 	 
 	for (i=0; i<50; i++)
 		{
 		c = 0;
 		for (j=0; j<72; j++)
 			{
			labBuffer[c*3] = (unsigned char)(labh[j*3*50+i*3]*2.55);
			labBuffer[c*3+1] = (char)(labh[j*3*50+i*3+1]);
			labBuffer[c*3+2] = (char)(labh[j*3*50+i*3+2]);
			c++;
			}
		status = (McoStatus)outtiff->continueWrite(&end);
		}
	
	outtiff->closeFile();
	delete outtiff;
	McoUnlockHandle(labH);
	McoDeleteHandle(labH);
	return status;
}

#else
McoStatus saveGammutSurfaceAsTiff(double *labh)
{
return MCO_SUCCESS;
}


McoStatus saveGammutSurfaceAsTiffSmall(double *labh)
{
return MCO_SUCCESS;
}
#endif


// build a set of luts that is used to find the index into a gamutsurface
McoStatus GammutComp::buildIndexLut(void)
{
int i,j,k;
double	*gamutSurface,*gp1,*gp2,*g1,*g2;
double	L;

if (patchD->type == CMYKPatches)
	gamutSurface = (double*)McoLockHandle(_gammutSurfaceBlackH);
else 
	gamutSurface = (double*)McoLockHandle(_gammutSurfaceH);


_indexlut = new short[4096*360];
if (_indexlut == 0L) return MCO_MEM_ALLOC_ERROR;

for (i=0; i<360; i++)
	{
	gp1 = &gamutSurface[i*256*4];
	gp2 = &gamutSurface[i*256*4+4];
	for (j=0; j<4096; j++)
		{
		L = j/40.95;
		g1 = gp1;
		g2 = gp2;
		for (k=0; k<255; k++)
			{
			if (*g1 > L) break;
			if ((*g1 < L) && (*g2 >= L)) break;
			g1 += 4;
			g2 += 4;
			}
		if (k >= 255) _indexlut[i*4096+j] = 255;
		else if ( (L-*g1) < (*g2-L) ) _indexlut[i*4096+j] = k;
		else _indexlut[i*4096+j] = k+1;
		}
	}
	
if (patchD->type == CMYKPatches) McoUnlockHandle(_gammutSurfaceBlackH);
else McoUnlockHandle(_gammutSurfaceH);

return MCO_SUCCESS;
}
/*
// build a set of luts that is used to find the index into a gamutsurface
McoStatus GammutComp::buildIndexLut(void)
{
int i,j,k;
double	*gamutSurface;
double	L;

if (patchD->type == CMYKPatches)
	gamutSurface = (double*)McoLockHandle(_gammutSurfaceBlackH);
else 
	gamutSurface = (double*)McoLockHandle(_gammutSurfaceH);


_indexlut = new short[4096*360];
if (_indexlut == 0L) return MCO_MEM_ALLOC_ERROR;

for (i=0; i<360; i++)
	{
	for (j=0; j<4096; j++)
		{
		L = j/40.95;
		for (k=0; k<255; k++)
			{
			if (gamutSurface[i*256*4+k*4] > L) break;
			if ((gamutSurface[i*256*4+k*4] < L) && (gamutSurface[i*256*4+(k+1)*4] >= L)) break;
			}
		if ( (L-gamutSurface[i*256*4+k*4]) < (gamutSurface[i*256*4+(k+1)*4]-L) ) _indexlut[i*4096+j] = k;
		else _indexlut[i*4096+j] = k+1;
		}
	}
	
if (patchD->type == CMYKPatches) McoUnlockHandle(_gammutSurfaceBlackH);
else McoUnlockHandle(_gammutSurfaceH);

return MCO_SUCCESS;
}
*/


// get the index from the index lut
inline McoStatus GammutComp::getgammutSurfaceIndex(double *lchin,long *ai, long *bi)
{
short L_i,h_i;

L_i = (short)(lchin[0]*40.95);
h_i = (short)lchin[2];

*ai = _indexlut[h_i*4096+L_i];
*bi = h_i;

return MCO_SUCCESS;
}

// a picewise linear function written especially for apply_gamut_comp2
// speed is the primary factor for this function
double piceLin_gc(double *xa,double *ya,double x);
inline double piceLin_gc(double *xa,double *ya,double x)
{
	int klo,khi,k;
	int n = 18;
	double h,a;

	klo=0;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (*(xa+k) > x) khi=k;
		else klo=k;
	}
	h=*(xa+khi)-*(xa+klo);
	if (h == 0) { return 0;}
	a=(x-*(xa+klo))/h;
	return (1-a)**(ya+klo)+a**(ya+khi);
}

McoStatus GammutComp::apply_gamut_comp2(double gammutExp)
{
McoStatus state;
int i,j,k,l;
double *gammutSurface;
double *gamSp;
double	L,A,B,L2,L3,A2,B2,C,C2,H;
double  LD;
double	D,E,SAm;
int32	Lmin,Lmax;
double	labin[3],labin2[3];
double	lchin[3],lchin2[3];
double	t,s,u,v;
double  min_dist,dist;
double	*lab,*labh,*labp,*labcopy;
int32	ai1,bi1;
int32	h;
double	delta_s;
double  n;
//double	maxgrayL,mingrayL;
double	tx[4],ty[4],y2[4],yd1,ydn;
double  tx_2[3],ty_2[3],y2_2[3];
double  tx_3[3],ty_3[3],y2_3[3];
double	inter_low,inter_high;
double  blueness;
double	da;
double	lw,hw,st;
double	satdist;
double	Lt;
double	a;
double	*laborig;
double	plab[3];
double	P1,P2;
double	SatCurve;
double	SatDown;
double	SatScale;
double	SatPerOut;
double	MaxSat = 128;
int32	L_index,L_index2;
unsigned char *out_of_gamut;
double	sd_x[19],sd_y[19];
int32	hm,hp;

double	L_exp;

	

	_gammutExp = gammutExp;

	laborig = new double[TABLE_ENTRIES*3];
	if (!laborig) return MCO_MEM_ALLOC_ERROR;
	labp = laborig;
	
	lab = (double*)McoLockHandle(_labtableH);
	
	out_of_gamut = (unsigned char*)McoLockHandle(_out_of_gamutH);
	
//	saveAsTiff(lab);
	
	for( i = 0; i < TABLE_ENTRIES; i++){
		*labp++ = *lab++;
		*labp++ = *lab++;
		*labp++ = *lab++;
	}
	McoUnlockHandle(_labtableH); 


	SatCurve = _SaturationAmount/100+0.70*(1-_SaturationAmount/100);
	SatDown = 12.5*(1-_SaturationAmount/100);
	SatScale = _SaturationAmount/100 + 0.45*(1-_SaturationAmount/100);
	SatPerOut = 0.6;
	
	sd_x[0] = 0;
	sd_y[0] = 0;
	for (i=0; i<18; i++)
		{
		sd_x[i+1] = (1<<i)/2.0;
		sd_y[i+1] = pow((1<<i)/2.0,SatCurve);
		}
	
	state = get_gamut_surface();
	if (state != MCO_SUCCESS) goto cancel_gc;
	
	state = buildIndexLut();
	if (state != MCO_SUCCESS) goto cancel_gc;
	//_maxgrayL = maxgrayL;
	//_mingrayL = mingrayL;
	
	// build a curve to correct luminance in in-gammut colors
	// Otherwise these colors have L values that are above and below the max and min values
	// The boundary between in and out of gammut colors is not continous
	
	//modified by Peter on 4/15	
/*	
	tx[1] = 0;
	tx[2] = 50;
	tx[3] = 100;
	
	ty[1] = _mingrayL;
	ty[2] = 50;
	ty[3] = _maxgrayL;	
*/
short stretch;	
double dmingrayL, dmaxgrayL;

	 if (patchD->type == CMYKPatches)
		gammutSurface = (double*)McoLockHandle(_gammutSurfaceBlackH);
	else 
		gammutSurface = (double*)McoLockHandle(_gammutSurfaceH);
		
	// verify that _mingrayL is correct by searching gammutSurface for the minimum value
	// _mingrayL should not be larger than this
	
	dmingrayL = 1000;
	for (i=0; i<360; i++) dmingrayL = MinVal(dmingrayL,gammutSurface[i*256*4]);
	_mingrayL = MaxVal(dmingrayL-2.0,_mingrayL);
		
	
	dmingrayL = _mingrayL;
	dmaxgrayL = _maxgrayL;
	stretch = need_lightness_stretch(&dmaxgrayL, &dmingrayL);

	if(stretch){
		tx[1] = _minrequiredgrayL;
		tx[2] = 50;
		tx[3] = _maxrequiredgrayL;
		
		ty[1] = dmingrayL;
		ty[2] = 50;
		ty[3] = dmaxgrayL;
	}
	else{
		tx[1] = _minrequiredgrayL;
		tx[2] = 50;
		tx[3] = _maxrequiredgrayL;
		
		ty[1] = _minrequiredgrayL;
		ty[2] = 50;
		ty[3] = _maxrequiredgrayL;
	}	
	
	
	yd1=(ty[2]-ty[1])/(tx[2]-tx[1])/4;
	ydn=(ty[3]-ty[2])/(tx[3]-tx[2])/4;
	
	// the spline for in gammut colors near the edge
	spline(tx,ty,3,yd1,ydn,y2);
	
	// attempt to make lightness linear by compressing at low and high L
	
	if (ty[1] > tx[1]) inter_low = ty[1]+10;
	else inter_low = tx[1] + 10;
	inter_low = inter_low;
	
	if (ty[3] < tx[3]) inter_high = ty[3]-10;
	else inter_high = tx[3]-10;
	inter_high = inter_high;
	
	// make the first spline for compression
	
	tx_2[1] = tx[1];
	ty_2[1] = ty[1];
	tx_2[2] = inter_low;
	ty_2[2] = inter_low;
	
	yd1 = 1e30;
	ydn = 1;
	spline(tx_2,ty_2,2,yd1,ydn,y2_2);
	
	// verify that there is no dip
	
	 splint(tx_2,ty_2,y2_2,2,tx_2[1]+1,&L3);
	 if (L3 < ty_2[1]) 
	 	{
		yd1 = 1e30;
		ydn = 1e30;
		spline(tx_2,ty_2,2,yd1,ydn,y2_2);
		}
	// make the second spline for compression
	
	tx_3[1] = inter_high;
	ty_3[1] = inter_high;
	tx_3[2] = tx[3];
	ty_3[2] = ty[3];
	
	yd1 = 1;
	ydn = 1e30;
	spline(tx_3,ty_3,2,yd1,ydn,y2_3);
	
	// verify that there is no dip
	splint(tx_3,ty_3,y2_3,2,tx_3[2]-1,&L3);
	if (L3 > ty_2[2]) 
	 	{
		yd1 = 1e30;
		ydn = 1e30;
		spline(tx_3,ty_3,2,yd1,ydn,y2_3);
		}
	
	lab = (double*)McoLockHandle(_labtableH);
	labh = lab;	
	labp = lab;
	labcopy = (double*)McoLockHandle(_labtableCopyH);
	
//	saveAsTiff(lab);
	



//	saveGammutSurfaceAsTiff(gammutSurface);

//	SLDisable();
			
	for( i = 0; i < 33; i++){
		//if (i >31) SLEnable();
		for( j = 0; j < 1089; j++){
				
			if (j % 30  == 0) if (thermd && !thermd->DisplayTherm(1000+i*30,Process_GammutSurface)) goto cancel_gc;	
				
			L = *labh++;
			A = *labh++;
			B = *labh++;	
	
			// compress L
			L2 = (L - _minrequiredgrayL)/(_maxrequiredgrayL - _minrequiredgrayL); 
			if (L2 < 0) L2 = 0;
			if (L2 > 1) L2 = 1; 
			L2 = (1-L2)*_mingrayL + L2*_maxgrayL;	// The new L value
			L3 = L;
			if (L3 < inter_low) splint(tx_2,ty_2,y2_2,2,L,&L3);
			else if (L3 > inter_high) splint(tx_3,ty_3,y2_3,2,L,&L3);						
			LD = (1-_l_linear)*L2+_l_linear*L3;	// make true linear

			labin[0] = LD;
			labin[1] = A;
			labin[2] = B;	

		
			*labcopy++ = L;
			*labcopy++ = A;
			*labcopy++ = B;
			
			L_index = LD*2.55;
			
			labin[1] -= min_max_curve[L_index*3+1];
			labin[2] -= min_max_curve[L_index*3+1];	
			
			labtolch(labin,lchin);
			
			C = lchin[1];
			H = lchin[2];
					
			// check to see if in gammut
			getgammutSurfaceIndex(lchin,&ai1,&bi1);
			//ai1 = lchin[0]*2.55;
			//bi1 = lchin[2];
			
			labin2[0] = gammutSurface[ai1*4+bi1*4*256];	
			labin2[1] = gammutSurface[ai1*4+bi1*4*256+1];	
			labin2[2] = gammutSurface[ai1*4+bi1*4*256+2];	
			
			L_index = labin2[0]*2.55;
			
			labin2[1] -= min_max_curve[L_index*3+1];
			labin2[2] -= min_max_curve[L_index*3+1];	
						
			labtolch(labin2,lchin2);
			
			// decide if color is in gammut
			if ((lchin[1] > lchin2[1]) || (fabs(lchin[0]-labin2[0]) > 2.5))// || (L > maxgrayL) || (L < mingrayL))// out of gammut
				{
				*out_of_gamut++ = 1;
				// find the desired L value
		
				// apply the dirty hack to boost the red
				
				v = (lchin[1] - lchin2[1])/5;
				v = -v*v;
				v = 2/(1+exp(v))-1;
				
				t = (A/85-1.5);
				t = -t*t;
				
				s = (B/85-1.5);
				s = -s*s;
				
				u = (L/50-1);
				u = -u*u;
				
				n = exp(t)*exp(s)*exp(u)*v;
				
				t = 0.8*C;
				t = t*t*t;
				t = t/(1+t);
					
				n *= t;
				
				L = L - 14*n;
				if (L<0) L = 0;
				A = A + 9*n;
				B = B - 9*n;
				
				labin[0] = L;
				labin[1] = A;
				labin[2] = B;
				
				L_index = L*2.55;
				
				labin[1] -= min_max_curve[L_index*3+1];
				labin[2] -= min_max_curve[L_index*3+1];			
									
				labtolch(labin,lchin);
				
				// apply the dirty hack to make blue less red
				
				s = -1.0E9;
				if ((lchin[2] > 265) && (lchin[2] < 315)) s = 0;
				else if (lchin[2] > 315) s = (lchin[2] - 315)/80;
				else s = (lchin[2] - 265)/80;
				
				s = -s*s;
				
				n = exp(s)*v;
					
				n *= t;
				
				lchin[2] = lchin[2] - 13*n;
				
				C = lchin[1];
				H = lchin[2];
				
				// find the range to search over
				Lmin = LD*40.95 - 480; //688
				Lmax = LD*40.95 + 480;
				if (Lmin < 0)  
					{
					Lmin = 0;
					Lmax = 960;
					}
				if (Lmax>4095)
					{
					Lmax = 4095;
					Lmin = 3135;
					}	
					
			
				min_dist = 10E20;
				
				// lightness is less important for dark regions
				//lw = 2+3*(1-exp(-L*L*0.002));	
				L_exp = exp(-L*L*0.002);
				lw = 3+4*(1-L_exp);	
				
				//temporary hack
				//lw *= 10;
				
				if (C < 1)
					{
					hm = -180;
					hp = 180;
					}
				else
					{
					hm = 0;
					hp = 40;
					}
				
				for (l=hm; l<hp; l++)
					{
					h = H + l - (hp-hm)/2;
					if (h<0) h = 360 + h;
					if (h>=360) h = h - 360;
					
					L_index = _indexlut[Lmin+h*4096];
					L_index2 = _indexlut[Lmax+h*4096];
					
					gamSp = &gammutSurface[h*4*256 + L_index*4];
					
					// hue is more important for dark bluish regions
					da = delta_angle(315,h);
					blueness = exp(-da*da*0.002);
					
				//	da = delta_angle(260,h);
				//	blueness += exp(-da*da*0.002);
					// hue is more important for dark bluish regions and light regions
					hw = 1+L_exp*blueness; // + exp(-(72-L)*(72-L)*0.002)*blueness;
					// hue difference is more apperent for saturated blues
					hw *= 1+(blueness*C*0.05);
					
					// hue is unimportant when saturation is very low
					
					t = 3*C;
					t = t*t*t*t;
					
					hw *= t/(1+t);	
					
					for (k=L_index; k<L_index2; k ++)
						{
						
						t = *(gamSp) - LD; //Lightness 
						t = t*t;
						
						st = *(gamSp+3); //saturation
						
						s = C-st;   //st - C;
						if (s < 0) s = 0;
						s = s*s;
						
						//st = s;
						// apply a curve to satration
						//s = pow(s,SatCurve);
						s = piceLin_gc(sd_x,sd_y,s);

						u = l-20;	//hue
						u = u*u;
											
						dist =  lw*t + s + hw*u;
						if (dist < min_dist) // the new best match
							{
							min_dist = dist;
							L2 = *(gamSp++);
							A2 = *(gamSp++);
							B2 = *(gamSp++);
							C2 = *(gamSp++);
							}
						else gamSp += 4;
						}
					}
				// ensure that oversaturation did not occur	
				// oversaturation can occur when ligthness compression is being performed
				if (C2 > C)
					{ // desaturate
					A2 = A2*C/C2;
					B2 = B2*C/C2;
					}			
				// compress the saturation
				P1 = C2 - SatDown;
				if (P1<10) P1 = C2*SatScale;
				P2 = (1-SatPerOut)*C2+SatPerOut*MaxSat;
				if (C<P2) // compress saturation
					{
					D = (C-P1)/(P2-P1);
					D = (1-D)*P1 + D*C2;
					D = D/C2;
					A2 *= D;
					B2 *= D;
					}				

				// write the best match into the table
				*lab++ = L2;
				*lab++ = A2;
				*lab++ = B2;
				}
			else
				{
				*out_of_gamut++ = 0;
				
				C2 = lchin2[1];
				C = lchin[1];
				P1 = C2 - SatDown;
				if (P1<10) P1 = C2*SatScale;
				P2 = (1-SatPerOut)*C2+SatPerOut*MaxSat;
				if (C > P1)
					{
					D = (C-P1)/(P2-P1);
					D = (1-D)*P1 + D*C2;
					D = D/C2;
					A *= D;
					B *= D;				
					}
				// apply the curve
				*lab++ = LD;
				*lab++ = A;
				*lab++ = B;
				}
			}
		} 
	
	McoUnlockHandle(_labtableH);
	McoUnlockHandle(_labtableCopyH);
	//_linerizeWhiteBlack();
	
	lab = (double*)McoLockHandle(_labtableH);
	labcopy = (double*)McoLockHandle(_labtableCopyH);
	for( i = 0; i < 35937; i++){
		*labcopy++ = *lab++;
		*labcopy++ = *lab++;
		*labcopy++ = *lab++;
	}
	McoUnlockHandle(_labtableH);
	McoUnlockHandle(_labtableCopyH);
	
	double L_bef[33];
	
	for (i=0; i<33; i++) L_bef[i]=labp[i*3267+1632];
	
//	saveAsTiff(labp);	
	
	_smooth_Lab(laborig,gammutSurface);
//	_smooth_Lab2(laborig,gammutSurface);
//	_smooth_Lab2(laborig,gammutSurface);
//	_smooth_Lab2(laborig,gammutSurface);

	
	
	for (i=0; i<33; i++) L_bef[i]=labp[i*3267+1632];
	
	// Fix the white point
	// The index of the white table entry is 106176
	
	// First find the color of the paper
	
	patchD->getPaperLab(plab);
	

	plab[1] = plab[1] * _PaperWhite/100.0;
	plab[2] = plab[2] * _PaperWhite/100.0;
	
	lab = (double*)McoLockHandle(_labtableH);
	
	
	lab[106176] = plab[0];
	lab[106177] = plab[1];
	lab[106178] = plab[2];

	
	McoUnlockHandle(patchD->dataH);
	
//	saveAsTiff(lab);
	labcopy = (double*)McoLockHandle(_labtableCopyH);
	for( i = 0; i < 35937; i++){
		*labcopy++ = *lab++;
		*labcopy++ = *lab++;
		*labcopy++ = *lab++;
	}
	
	delete laborig;
	McoUnlockHandle(_labtableH);
	McoUnlockHandle(_labtableCopyH);
	McoUnlockHandle(_out_of_gamutH);
	if (patchD->type == CMYKPatches) McoUnlockHandle(_gammutSurfaceBlackH);
	//else
	 McoUnlockHandle(_gammutSurfaceH);
	return MCO_SUCCESS;
	
cancel_gc:

	delete laborig;
	McoUnlockHandle(patchD->dataH);
	McoUnlockHandle(_labtableH);
	McoUnlockHandle(_labtableCopyH);
	McoUnlockHandle(_out_of_gamutH);
	if (patchD->type == CMYKPatches) McoUnlockHandle(_gammutSurfaceBlackH);
	//else
	 McoUnlockHandle(_gammutSurfaceH);
	 
	return MCO_CANCEL;
}
