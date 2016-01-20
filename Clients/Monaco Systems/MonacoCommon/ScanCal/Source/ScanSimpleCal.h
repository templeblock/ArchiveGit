#ifndef	SCAN_SIMPLE_CAL_H
#define SCAN_SIMPLE_CAL_H

#include "multilcal.h"
#include "calfunc.h"
#include "colortran.h"
#include "pwlinear.h"

class ScanSimpleCal{

private:
protected:
	long		_num_data;	//total number of data
	long		_num_comp;
	PWlinear 	**_linear;
	MultiLcal 	*_gcal;
	
	McoStatus 	_globalcal(Matrix &mat, long num, double *mea, double *ref);

	McoStatus 	_linearstrech(int32 num_linear, PWlinear **linear, int32 num_data, double* data);
	void 		_cleanup(void);
	
public:
ScanSimpleCal(long num_data);
~ScanSimpleCal(void);

McoStatus 	calibrate(long size, double *mea, double *ref);
McoStatus	compute(long num, double *in, double *out);
McoStatus 	deltaE(int32 num, double* cmp, double *ref, double *avg, double *max, int32 *which);

};

#endif
