// linearize data
// take a set of c,m,y,k curves and linearize c,m,y,k values for these curves
// 
// James Vogh
// Copyright 1997 Monaco Systems

#ifndef IN_LINEARIZE_DATA
#define IN_LINEARIZE_DATA

#include "mcostat.h"
#include "mcotypes.h"

class LinearData {
private:
protected:
public:
	int		numc;
	int32	*sizes;
	double	**curves;
	double  **y2;
	double  **y3;
	double  **values;
	
	
	LinearData(void);
	~LinearData(void);
	
	void CleanUp(void);
	
	// remove reversals in the curve data
	McoStatus cleanReversals(double *curve, double *values,int32 *size);

	// use splines to linearize tables
	McoStatus setUp(double *data,double *vals, double *paper,int nc,int s);
	
	// use splines to linearize tables
	// the polaroid version (data is in density format)
	McoStatus setUp(double *data,double *vals,int array_mult,int nc,int *s);

	// convert values to linear values
	McoStatus CnvtLinear(double *data, int32 num, int32 ch);

	// convert a value to a linear value
	double CnvtLinear(double data,int32 ch);
	
	// convert values to linear values
	McoStatus CnvtLinearInv(double *data, int32 num, int32 ch);

	// convert a value to a linear value
	double CnvtLinearInv(double data,int32 ch);
	};
	
#endif