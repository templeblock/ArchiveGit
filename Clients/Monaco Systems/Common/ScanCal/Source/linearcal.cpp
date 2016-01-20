#include "linearcal.h"
#include "mcomem.h"

LinearCal::LinearCal(int32 dimension, int32 channel, void (*func)(double*, double*) )
{	
	_dimension = dimension;
	_channel = channel;
	_func = func;
	
	_err = MCO_SUCCESS;
}

LinearCal::~LinearCal(void)
{

}

McoStatus LinearCal::compute(int32 num_data, double* mea, double *ref)
{
	McoStatus status;
	int32 i, j, k;
	double  *meastart, *refstart, val;
	double hold[20];
			
	Matrix	mea_mat, trans_mea_mat, ref_mat;

	Matrix core_mat(_dimension, _dimension);
	Matrix ref_data_mat(_dimension, 1);

	status = mea_mat.set(_dimension, num_data);	
	if(status != MCO_SUCCESS) return status;

	trans_mea_mat.set(num_data, _dimension);	
	if(status != MCO_SUCCESS) return status;

	ref_mat.set(num_data, 1);
	if(status != MCO_SUCCESS) return status;
	
	meastart = mea;
	refstart = ref;	
	
	for(i = 1; i <= num_data; i++){
		_func(meastart, hold);
		for(j = 0; j < _dimension; j++)
			mea_mat.setval(j+1,i, hold[j]);
				
		ref_mat.setval(i,1, refstart[0]);
		
		meastart += _channel;
		refstart++;
	}
	
	trans_mea_mat.copyT(mea_mat);//transverse
	
	//compute the left matrix
	core_mat = mea_mat*trans_mea_mat;

	//compute the right vector matrix	
	ref_data_mat = mea_mat*ref_mat;
	
	core_mat.inv();
	if( (status = core_mat.get_status()) != MCO_SUCCESS) //singular or ?
		return status;
		
	Matrix refr = core_mat*ref_data_mat;

	status = refr.savestruct(_weight);
	if(status != MCO_SUCCESS) return status;
		
	return MCO_SUCCESS;		
}


void LinearCal::apply(int32 num, double *in, double *out)
{
	long i, j;
	double hold[20];
	double *instart, *outstart;

	instart = in;
	outstart = out;
	
	for(i = 0; i < num; i++){
		_func(instart, hold);
		
		*outstart = 0;
		for( j = 0; j < _dimension; j++)
			*outstart += hold[j]*_weight[j];
			
		instart += _channel;
		outstart++;
	}		
}

McoStatus LinearCal::getweight(double *weight)
{
	memcpy((char*)weight, (char*)_weight, _dimension*sizeof(double));
	return _err;
}	