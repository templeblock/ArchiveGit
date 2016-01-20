#ifndef MULTILCAL_H
#define MULTILCAL_H

#include "linearcal.h" 

#define MAX_MULTI	(4)

class MultiLcal{
protected:
	McoStatus _err;
	LinearCal *_linearcal[MAX_MULTI];	
	int32	_num_cal;
	int32	_dimension;
		
public:
MultiLcal(int32 dimension, int32 channel, int32 num_cal, void (*func)(double*, double*));
~MultiLcal();

McoStatus compute(int32 num_data, double* mea, double *ref);

McoStatus apply(int32 num, double *in, double *out);

McoStatus  diff(int32 num, double* mea, double *ref, double *res);

McoStatus sqrtdiff(int32 num, double* mea, double *ref, double *avg, double *max, int32 *which);

Matrix getMatrix();

};

#endif //MULTILCAL_H