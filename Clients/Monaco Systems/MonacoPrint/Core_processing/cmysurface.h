#ifndef CMY_SURFACE_H
#define CMY_SURFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"

class Cmysurface{

protected:

	int32 _one_d_points;	//how many edges in one dimension
	int32 _tri_id[6];		//square id mask
	int32 *_start_id;	
	
	double _minL, _maxL;	//minimum and maximum of L
	double _mingrayL, _maxgrayL;
	double _cL, _mL, _yL;
	
	//surface lab values
	//sequence from c=0, c=1,m=0,m=1,y=0,y=1 6 surfaces
	double *_slab;	
	

	//function to find the maxL and minL of gray scale
	void 	_findminmax(void);
	// get the min and max for the desired lab vaules
	// used when non-neutral paper color is selected
	McoStatus _findminmax(double *maxL_lab,double *minL_lab);	

public:
	Cmysurface(int32 one_d_points, double *lab,double *maxL_lab,double *minL_lab);
	~Cmysurface(void);
	// added by James on 11/11
	McoStatus getMinMax(double &maxgrayL, double &mingrayL);


	McoStatus findboundary(double *inlab, double *boundlab, double *boundcmy, int16* yes);
	McoStatus findboundarynew(double *inlab, double *inlab2, double *boundlab, double *boundcmy, int16* yes,double gammutExp,int32 singCount);
	McoStatus findBoundaryNoNorm(double *inlab, double *inlab2, double *boundlab, double *boundcmy, int16* yes,double gammutExp,int32 singCount);

	
};

#endif	