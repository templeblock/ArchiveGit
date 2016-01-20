#include <math.h>
#include "multilcal.h"
#include "mcomem.h"

MultiLcal::MultiLcal(int32 dimension, int32 channel, int32 num_cal, void (*func)(double*, double*))
{
	long i;
	
	_err = MCO_SUCCESS;
	_num_cal = num_cal;
	_dimension = dimension;
	
	for(i = 0; i < _num_cal; i++){
		_linearcal[i] = new LinearCal(dimension, channel, func);
		if(!_linearcal[i])	_err = MCO_MEM_ALLOC_ERROR;
	}	
}

MultiLcal::~MultiLcal()
{
	long i;
	
	for(i = 0; i < _num_cal; i++)
		if(_linearcal[i]) delete _linearcal[i];
}

McoStatus MultiLcal::compute(int32 num_data, double* mea, double *ref)
{
	McoStatus status;
	long i, j;
	double *ref_data;
	
	if(_err != MCO_SUCCESS)	return _err;
	
	ref_data = (double*)McoMalloc(sizeof(double)*num_data);
	if(!ref_data)	return MCO_MEM_ALLOC_ERROR;
	
	for(i = 0; i < _num_cal; i++){
		for(j = 0; j < num_data; j++)
			ref_data[j] = ref[j*_num_cal + i];
			
		status = _linearcal[i]->compute(num_data, mea, ref_data);
		if(status != MCO_SUCCESS)
			break;
	}	

	McoFree(ref_data);
	return status;
}

McoStatus MultiLcal::apply(int32 num, double *in, double *out)
{
	long i, j;
	double *outdata;
	
	if(_err != MCO_SUCCESS)	return _err;

	outdata = (double*)McoMalloc(sizeof(double)*num);
	if(!outdata)	return MCO_MEM_ALLOC_ERROR;
	
	for(i = 0; i < _num_cal; i++){
		_linearcal[i]->apply(num, in, outdata);
		for(j = 0; j < num; j++)
			out[j*_num_cal+i] = outdata[j]; 
	}
	
	McoFree(outdata);
	return MCO_SUCCESS;		
}

McoStatus  MultiLcal::diff(int32 num, double* mea, double *ref, double *res)
{
	McoStatus status;
	long i, total;
	double *cmpdata;	//computed data
	
	if(_err != MCO_SUCCESS)	return _err;

	cmpdata = (double*)McoMalloc(sizeof(double)*num*_num_cal);
	if(!cmpdata)	return MCO_MEM_ALLOC_ERROR;
	
	status = apply(num, mea, ref);
	if(status != MCO_SUCCESS)	goto done;
	
	total = _num_cal*num;
	
	for(i = 0; i < total; i++)
		res[i] = cmpdata[i] - ref[i];
		
done:
	McoFree(cmpdata);
	return status;	
}

McoStatus MultiLcal::sqrtdiff(int32 num, double* mea, double *ref, double *avg, double *max, int32 *which)
{
	McoStatus status;
	long i, j;
	double *cmpdata;	//computed data
	double sum, tempmax, tempsum;
	double *cmpstart, *refstart;
	
	if(_err != MCO_SUCCESS)	return _err;

	cmpdata = (double*)McoMalloc(sizeof(double)*num*_num_cal);
	if(!cmpdata)	return MCO_MEM_ALLOC_ERROR;
	
	status = apply(num, mea, cmpdata);
	if(status != MCO_SUCCESS)	goto done;
		
	tempmax = 0;
	*avg = 0;
	cmpstart = cmpdata;
	refstart = ref;
	for(i = 0; i < num; i++){
		tempsum = 0;
		for(j = 0; j < _num_cal; j++)
			tempsum += (cmpstart[j] - refstart[j])*(cmpstart[j] - refstart[j]);
		tempsum = sqrt(tempsum);
		*avg += tempsum;
		if(tempmax < tempsum){
			*which = i;
			tempmax = tempsum;
		}
		
		cmpstart += _num_cal;
		refstart += _num_cal;	
	}
	
	*avg /= num;
	*max = tempmax;	
done:
	McoFree(cmpdata);
	return status;	
}

Matrix MultiLcal::getMatrix()
{
	McoStatus status;
	long i;
	double weight[80];
	
	
	for(i = 0; i < _num_cal; i++){
		status = _linearcal[i]->getweight(&weight[i*_dimension]);
		if(status != MCO_SUCCESS)
			return 0;
	}
	
	Matrix mat(_num_cal, _dimension);
	mat.loadstruct(weight);
	
	return mat;
}			
