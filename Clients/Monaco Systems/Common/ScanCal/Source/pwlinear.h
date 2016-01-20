#ifndef PWLINEAR_H
#define PWLINEAR_H

#include "mcostat.h"
#include "mcotypes.h"
#include "mcomem.h"

//class for handling piecewise linear curve

class PWlinear{
protected:
	McoStatus _err;
	int32	_size;
	double *_y;
	double *_x;
	
	void _deallocate(void);
	void _sequence(void);
	
public:
	PWlinear(int32 size, double *x, double *y);
	~PWlinear();
	
	McoStatus reset(int32 size, double *x, double *y);
	McoStatus apply(int32 num, double *in);
	McoStatus apply(int32 num, double *x, double *y);	
};	
		
#endif //PWLINEAR_H


