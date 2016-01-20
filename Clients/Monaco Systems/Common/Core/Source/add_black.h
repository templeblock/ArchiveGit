////////////////////////////////////////////////////////////////////////////
//	add_black.h															  //
//																		  //
//	add black to the cmyk table given the gammut compressed lab table,    //
//  the black patches, and the user black curve							  //
//																		  //
//  Created on Feb 16, 1996												  //
//  By James Vogh														  //
////////////////////////////////////////////////////////////////////////////

#ifndef IN_ADD_BLACK
#define IN_ADD_BLACK

#include "monacoprint.h"
#include "mcomem.h"
#include "calib_multi.h"

#include "rawdata.h"

#include "profiledoc.h"

#include "thermobject.h"


class add_black {
private:
protected:

long 	_rev_table_grid;
long	_rev_table_size;

RawData *patchD;

McoHandle labTableH;
McoHandle cmykTableH;
McoHandle blackTableH;
McoHandle gammutSurfaceH;
McoHandle gammutSurfaceBlackH;
McoHandle gammutSurfaceManyBlackH;
McoHandle gammutSurfaceManyBlackLH;
McoHandle out_of_gamutH;


calib_base	*calib;

double minL;
double absMinL;
double maxInk;
double ucrgcr;

double 		*gammutSurface;

double 		*bL,*bK,*a,*b;

int32		numB;

double		minLCMY,maxLCMY;	// maximum and minimum CMY L values

ThermObject	*thermd;
ProfileDoc	*doc;

double		blackT[101];

public:

add_black(ProfileDoc *doc,calib_base *calib);
~add_black(void);

void 	setMinL(double ml);
void	setMinMaxL(double mingrayL, double maxgrayL);
Boolean testInGammut(double *lab,double	range,double *dist);
void clipMaxInk(double *lab,double *cmyk);

McoStatus compute(void);

// Smooth the K 
McoStatus smooth_K(double	*cmyk);

// run a median filter over the K
McoStatus median_K(double	*cmyk);

McoStatus smooth_CMY(double	*cmyk);
McoStatus median_CMY(double *lab,double	*cmyk);

McoStatus evalMB(double minG,double maxG);
// find the CMYK values when the K only switch is active
McoStatus eval_K_Only(double *lab_K,double *cmyk_K,int32 num);
McoStatus _getlab(void);
McoStatus _getlabs(void);

McoStatus findMinLMB(double *lab);
};

#endif
