#ifndef LINEARCAL_H
#define LINEARCAL_H

#include "matrix.h"
#include "mcostat.h"

#define MAX_ITEMS			(20) //maximum of items in one function
	
class LinearCal{
private:

protected:
	McoStatus _err;

	int32 	_dimension;	//linear operation dimension
	int32	_channel;	//rgb 3, cmyk 4
	void 	(*_func)(double*, double*);	//user defined functions

	double	_weight[MAX_ITEMS];	//structure to save parameters

public:
	LinearCal(int32 dimension, int32 channel, void (*func)(double*, double*) );
	virtual	~LinearCal(void);

	virtual	McoStatus compute(int32 num_data, double* mea, double *ref);
	virtual	void apply(int32 num, double *in, double *out);

	McoStatus getweight(double *weight);
};
	
#endif	//LINEARCAL_H