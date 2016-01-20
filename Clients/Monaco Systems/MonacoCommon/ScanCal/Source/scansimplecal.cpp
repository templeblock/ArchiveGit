#include <math.h>
#include "scansimplecal.h"

#define LINEAR_NUM	(20)	//linear cal

ScanSimpleCal::ScanSimpleCal(long num_comp)
{
	_num_comp = num_comp;
	_linear = 0L;
	_gcal = 0L;
}

ScanSimpleCal::~ScanSimpleCal(void)
{
	_cleanup();		
}

void ScanSimpleCal::_cleanup(void)
{
	long i;
	
	if(_linear){
		for(i = 0; i < 3; i++)
			delete _linear[i];
		delete _linear;
		_linear = 0L;
	}
	if(_gcal){
		delete _gcal;
		_gcal = 0L;
	}	
	
}

McoStatus ScanSimpleCal::_linearstrech(int32 num_linear, PWlinear **linear, int32 num_data, double* data)
{
	long i, j;
	double *rgb;
	
	rgb = (double*)McoMalloc(sizeof(double)*num_data);
	if(!rgb)	return MCO_MEM_ALLOC_ERROR;
	
	for(i = 0; i < num_linear; i++){
		for(j = 0; j < num_data; j++)
			rgb[j] = data[j*num_linear + i];
			
		linear[i]->apply(num_data, rgb);
		
		for(j = 0; j < num_data; j++)
			data[j*num_linear + i] = rgb[j];
	}	
		
	McoFree(rgb);
	return MCO_SUCCESS;	
}	

McoStatus ScanSimpleCal::calibrate(long size, double *mea, double *ref)
{
	McoStatus status;
	long i, j, k, m;
	double avg, max;
	int32	which;
	long calsize;	//the number of patches used to calibrate
	double *desmea, *desref;
	double *cyanmea, *cyanref;

	//init
	_cleanup();

	_num_data = size;

	//matrix computation
	Matrix matconv(3);
	double mat[9];
	
	mat[0] = 0.5242/2.55;
	mat[1] = 0.3083/2.55;
	mat[2] = 0.1316/2.55;
	mat[3] = 0.2851/2.55;
	mat[4] = 0.6554/2.55;
	mat[5] = 0.0594/2.55;
	mat[6] = 0.0293/2.55;
	mat[7] = 0.1377/2.55;
	mat[8] = 0.6578/2.55;

	matconv.loadstruct(mat);
	matconv.inv();
	status = matconv.get_status();
	if(status != MCO_SUCCESS)	return	status;

	desmea = (double*)McoMalloc(sizeof(double)*_num_data*3);
	if(!desmea)	return	MCO_MEM_ALLOC_ERROR;
	
	desref = (double*)McoMalloc(sizeof(double)*_num_data*3);
	if(!desref)	return	MCO_MEM_ALLOC_ERROR;

	for(j = 0; j < _num_data*3; j++){
		desmea[j] = mea[j];
		desref[j] = ref[j];
	}	

//create global calibration	
	status = _globalcal(matconv, _num_data, desmea, ref);
	if(status != MCO_SUCCESS)	return status;

	McoFree(desmea);
	McoFree(desref);

	return status;
}

McoStatus 	ScanSimpleCal::deltaE(int32 num, double* cmp, double *ref, double *avg, double *max, int32 *which)
{
	McoStatus status;
	long i, j;
	double tempmax, tempsum;
	double *cmpstart, *refstart;

//	fs = fopen("de_debug","w");
//	if (!fs) return MCO_FAILURE;
	
	status = compute(num, cmp, cmp);
	if(status != MCO_SUCCESS)	return status;
		
	tempmax = 0;
	*avg = 0;
	cmpstart = cmp;
	refstart = ref;
	for(i = 0; i < num; i++){
		tempsum = 0;
		for(j = 0; j < 3; j++)
			tempsum += (cmpstart[j] - refstart[j])*(cmpstart[j] - refstart[j]);
		tempsum = sqrt(tempsum);
		
//		fprintf(fs,"%f\n", tempsum);
		
		*avg += tempsum;
		if(tempmax < tempsum){
			*which = i;
			tempmax = tempsum;
		}
		
		cmpstart += 3;
		refstart += 3;	
	}

//	fclose(fs);
	
	*avg /= num;
	*max = tempmax;	
	
	return MCO_SUCCESS;
}

McoStatus ScanSimpleCal::_globalcal(Matrix &mat, long num, double *mea, double *ref)
{
	McoStatus status;
	long i, j;
	double white[3];
	Matrix matdata(num, 3);
	double *rgbref;
	
	white[0] = 96.42;
	white[1] = 100.0;
	white[2] = 82.49;

	rgbref = (double*)McoMalloc(sizeof(double)*num*3);
	if(!rgbref)	return MCO_MEM_ALLOC_ERROR;

	for(i = 0; i < num*3; i++)
		rgbref[i] = ref[i];
	//convert lab to XYZ
	labtonxyzinplace(rgbref, num);
	nxyztoxyzinplace(rgbref, white, num);
	
	//convert XYZ to rgb
	matdata.loadstruct(rgbref);
	matdata.T();
	Matrix matrefrgb = mat*matdata;
	matrefrgb.T();
	matrefrgb.savestruct(rgbref);
		
	//build linearization
	double x[LINEAR_NUM], y[LINEAR_NUM];

	_linear = new PWlinear*[3];	
	for(j = 0; j < 3; j++){
		for(i = 0; i < num; i++){
			y[i] = rgbref[i*3 + j];
			x[i] = mea[i*3 + j];
		}
		_linear[j] = new PWlinear(num, x, y);
	}

	//linearization stretch
	status = _linearstrech(3, _linear, num, mea);
	if(status != MCO_SUCCESS)	return status;
		
	//create matrix	
	for(i = 0; i < num*3; i++)
		rgbref[i] = ref[i];
	//convert lab to xyz
	labtonxyzinplace(rgbref, num);
	nxyztoxyzinplace(rgbref, white, num);

	_gcal = new MultiLcal(3, 3, 3, func3);
	status = _gcal->compute(num, mea, rgbref);
	if(status != MCO_SUCCESS)	{
		delete _gcal;
		return status;
	}
		
	McoFree(rgbref);
	
	return MCO_SUCCESS;
}	

//apply the calibration here
McoStatus	ScanSimpleCal::compute(long num, double *in, double *out)
{
	McoStatus status; 
	long i, m, j, local;
	double rgb[3], lch[3], caled[3];
	double white[3];
	
	white[0] = 96.42;
	white[1] = 100.0;
	white[2] = 82.49;
	
	m = 0;
	for (i=0; i<num; i++){
		rgb[0] = in[m];
		rgb[1] = in[m+1];
		rgb[2] = in[m+2];
		
		status = _linearstrech(3, _linear, 1, rgb);
		if(status != MCO_SUCCESS)	return status;
		
		for(j = 0; j < 3; j++){
			if(rgb[j] < 0)	rgb[j] = 0;
			if(rgb[j] > 255)	rgb[j] = 255.0;
		}	
			
		status = _gcal->apply(1, rgb, caled);
		if(status != MCO_SUCCESS)	return status;
		xyztonxyzinplace(caled, white, 1);
		nxyztoLabinplace(caled, 1);

		out[m] = caled[0];
		out[m+1] = caled[1];
		out[m+2] = caled[2];
		m += 3;
	}

	return MCO_SUCCESS;	
}
