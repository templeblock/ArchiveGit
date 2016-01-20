////////////////////////////////////////////////////////////////////////////
//	gammut_comp.h														  //
//																		  //
//	calibrate by building multiple calibrations and and blending the 	  //
//  calibrations together												  //
//																		  //
//  Created on Feb 16, 1996												  //
//  By James Vogh														  //
////////////////////////////////////////////////////////////////////////////

#ifndef IN_GAMMUT_COMP
#define IN_GAMMUT_COMP

#include "mcostat.h"
#include "mcomem.h"


#include "Thermobject.h"


#include "rawdata.h"

#include "calib_multi.h"

// The number of black steps
//#define GAM_NUMB 10

class GammutComp {

	McoHandle _gammutSurfaceH;
	McoHandle _gammutSurfaceBlackH;
	McoHandle _gammutSurfaceManyBlackH;
	McoHandle _gammutSurfaceManyBlackLH;
	// The cmyk to lab table added by James on 11/11
	Boolean *didGammutSurface; // added by James on 1/19
	
	// a set of luts for decoding the index into _gammutSurfaceBlackH or _gammutSurfaceH
	short	*_indexlut;

	McoHandle	_out_of_gamutH;
	
// The large inverse table, allocate and interpolate if it is 0L
	McoHandle 	_inverseTableH;
	Boolean		myinverseTable;
	
//need a Lab table because of gamut compression
	McoHandle 	_labtableH;
	McoHandle 	_labtableCopyH;
	
	RawData		*patchD;
	double		*_patch;


	double	_SaturationAmount; // The distance to move into the gammut surface
	double	_PaperWhite;	   // The amount to move towards the color of the paper
	double	_col_bal_cut;	   // The L level where neutritiy is returned 


//table of ratio of compression boundary on 9/5
	McoHandle 	_cratioH;
	long		_hue_samples;
	
	//Max white and black points, used to stretch the L values for the CMYK to LAB table
	//These values are set in apply gamut
	double	_maxgrayL,_mingrayL;
	
	// maximum and minimum values used for gammut compression
	double minGam,maxGam;
	
	//Added by James on 4/16/96 to allow setting of min L value if max ink is less than 400
	double _inkMinL;

	double	_l_linear;

	//added by Peter to specify the max and min gray value of the labtable value
	double _maxrequiredgrayL, _minrequiredgrayL;
	//end of modification
	
	// the amount of gammut expansion
	double _gammutExp;

	// curves that specify actual lab values for the neutral colors
	double *min_max_curve;
	double *min_max_curve_sm;

	calib_base	*calib;
	
	
	
	McoStatus _smooth_Lab(double	*laborig,double *gammutSurface);
	McoStatus _smooth_Lab2(double	*laborig,double *gammutSurface);
	McoStatus _linerizeWhiteBlack(void);
	
	// create data for black == 10,30,40,60,80 by interpolating existing data
	// fill in data for 0,20,50,70,90
	McoStatus interpolateBlack(void);
	
	// build a set of luts that is used to find the index into a gamutsurface
	McoStatus buildIndexLut(void);

	// get the index from the index lut
	McoStatus getgammutSurfaceIndex(double *lchin,long *ai, long *bi);


public:
	ThermObject	*thermd;

	GammutComp(RawData *pD,McoHandle lH,McoHandle lcH,McoHandle ogH,
		McoHandle gsH,McoHandle gsbH,McoHandle gsmbH,McoHandle gsmbLH,McoHandle invTH,
		double _SaturationAmount, double _PaperWhite, double color_cutoff,double l_linear,
		double graymax, double graymin, double sgraymax, double sgraymin, double inkMinL,calib_base *cb,ThermObject *thd);
	~GammutComp(void);

	// build a Lab curve that goes from the absolute lightest point to the absolute darkest point
	McoStatus _buildMinMaxCurve(void);


	McoStatus apply_gamut_comp2(double gammutExp);
	void getMinMaxL(double &mingrayL,double &maxgrayL);
	// get the minimum and maximum values used for gammut compressiion
	void getMinMaxG(double &min,double &max);
	// Create a gammut surface buffer by interpolating values returned by findboundarynew
	McoStatus get_gamut_surface(void);

	//added by Peter on 4/15
	//dmaxgrayL and dmingrayL are device lightness gamut	
	short 	need_lightness_stretch(double* dmaxgrayL, double *dmingrayL);

};

double delta_angle(double a1,double a2);

#endif