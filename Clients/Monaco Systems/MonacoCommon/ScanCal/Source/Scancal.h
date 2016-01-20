#ifndef	SCAN_CAL_H
#define SCAN_CAL_H

#include "multilcal.h"
#include "calfunc.h"
#include "colortran.h"
#include "pwlinear.h"

class ScanCal{

private:
protected:
	long		_num_data;	//total number of data
	long		_num_comp;
	PWlinear 	**_linear;
	MultiLcal 	*_gcal, **_lcal;

//added for fixing the cyan problem	
	MultiLcal	*_cyan;		
	void		_arrange_cyan_patch(double *srcmea, double *srcref, double *desmea, double *desref);
	McoStatus	_compute_cyan(PWlinear **linear, double *mea, double *ref);
//end of modification
	
	McoStatus 	_globalcal(Matrix &mat, long num, double *mea, double *ref);
	McoStatus 	_localcal(PWlinear **linear, MultiLcal *gcal, MultiLcal **lcal, long num, double *in, double *out);
	long 		_select_local(double *lch);

	McoStatus	_compute_local(PWlinear **linear, double *mea, double *ref);
	void 		_arrange_patch(double *srcmea, double *srcref, double *desmea, double *desref);
	McoStatus 	_linearstrech(int32 num_linear, PWlinear **linear, int32 num_data, double* data);
	void 		_cleanup(void);
	
public:
ScanCal(long num_data);
~ScanCal(void);

McoStatus 	calibrate(long size, double *mea, double *ref);
McoStatus	compute(long num, double *in, double *out);
McoStatus 	deltaE(int32 num, double* cmp, double *ref, double *avg, double *max, int32 *which);

};

#endif
