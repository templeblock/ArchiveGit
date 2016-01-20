#include <math.h>
#include "scancal.h"

#define GRAY_THRESHOLD	(15.0)

#define CYAN_THRESHOLD		(52)
#define MAGENTA_THRESHOLD	(67)
#define YELLOW_THRESHOLD	(90)
#define RED_THRESHOLD		(69)
#define GREEN_THRESHOLD		(53)
#define BLUE_THRESHOLD		(57)

#define LOCAL_NUM	(12)	//local calibrations	

#define LINEAR_NUM	(23)	//linear cal

ScanCal::ScanCal(long num_comp)
{
	_num_comp = num_comp;
	_linear = 0L;
	_gcal = 0L;
	_lcal = 0L;
	_cyan = 0L;
}

ScanCal::~ScanCal(void)
{
	_cleanup();		
}

void ScanCal::_cleanup(void)
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
	if(_lcal){
		for(i = 0; i < 12; i++)
			delete _lcal[i];
		delete _lcal;
		_lcal = 0L;
	}
	if(_cyan){
		delete _cyan;
		_cyan = 0L;
	}	
	
}

void ScanCal::_arrange_cyan_patch(double *srcmea, double *srcref, double *desmea, double *desref)
{
	long i, j;
	long start, des;

	des = 0;
	for(i = 134; i < 137; i++){
		start = i*3;
		for(j = 0; j < 3; j++){
			desmea[des+j] = srcmea[start+j];
			desref[des+j] = srcref[start+j];
		}	
		des += 3;
	}
	
	for(i = 138; i < 141; i++){
		start = i*3;
		for(j = 0; j < 3; j++){
			desmea[des+j] = srcmea[start+j];
			desref[des+j] = srcref[start+j];
		}	
		des += 3;
	}
	
	start = 126*3;
	for(j = 0; j < 3; j++){
		desmea[des+j] = srcmea[start+j];
		desref[des+j] = srcref[start+j];
	}	
	des += 3;
	
	start = 145*3;
	for(j = 0; j < 3; j++){
		desmea[des+j] = srcmea[start+j];
		desref[des+j] = srcref[start+j];
	}	
	des += 3;

	start = 164*3;
	for(j = 0; j < 3; j++){
		desmea[des+j] = srcmea[start+j];
		desref[des+j] = srcref[start+j];
	}	
	des += 3;

	start = 186*3;
	for(j = 0; j < 3; j++){
		desmea[des+j] = srcmea[start+j];
		desref[des+j] = srcref[start+j];
	}	
	des += 3;

	start = 208*3;
	for(j = 0; j < 3; j++){
		desmea[des+j] = srcmea[start+j];
		desref[des+j] = srcref[start+j];
	}	
	des += 3;

	start = 230*3;
	for(j = 0; j < 3; j++){
		desmea[des+j] = srcmea[start+j];
		desref[des+j] = srcref[start+j];
	}	
		
}


void ScanCal::_arrange_patch(double *srcmea, double *srcref, double *desmea, double *desref)
{
	long i, j, k, m;
	long start;
	
	for(j = 0; j < _num_data*3; j++){
		desmea[j] = srcmea[j];
		desref[j] = srcref[j];
	}	

	if(_num_data == 264){
		start = 0;
		for(i = 0; i < 8; i++){
			j = i*19*3;
			for(m = 0; m < LOCAL_NUM; m++)
				for(k = 0; k < 3; k++){
					desmea[start] = desmea[j];
					desref[start] = srcref[j];
					start++;
					j++;
				}
		}
		
		for(i = 0; i < 4; i++){
			j = (8*19 + i*22)*3;
			
			for(m = 0; m < LOCAL_NUM; m++)
				for(k = 0; k < 3; k++){
					desmea[start] = desmea[j];
					desref[start] = srcref[j];
					start++;
					j++;
				}
		}
	}
	//need to add code to handle 288 case
	else if(_num_data == 288){
		start = 0;
		for(i = 0; i < 12; i++){
			j = i*22*3;
			for(m = 0; m < LOCAL_NUM; m++)
				for(k = 0; k < 3; k++){
					desmea[start] = desmea[j];
					desref[start] = srcref[j];
					start++;
					j++;
				}
		}
	}			
}

McoStatus ScanCal::_linearstrech(int32 num_linear, PWlinear **linear, int32 num_data, double* data)
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


McoStatus ScanCal::calibrate(long size, double *mea, double *ref)
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

	_arrange_patch(mea, ref, desmea, desref);

	long src, des;
	
	src = 0;
	des = 12*12*3;
	for(i = 0; i < 12*2*3;i++){
		desmea[des] = desmea[src];
		desref[des++] = desref[src++];
	}						
				
//create local calibration
	status = _compute_local(_linear, desmea, desref);
	if(status != MCO_SUCCESS)	return status;

	McoFree(desmea);
	McoFree(desref);
	
	cyanmea = (double*)McoMalloc(sizeof(double)*12*3);
	if(!cyanmea)	return	MCO_MEM_ALLOC_ERROR;
	
	cyanref = (double*)McoMalloc(sizeof(double)*12*3);
	if(!cyanref)	return	MCO_MEM_ALLOC_ERROR;
	
	_arrange_cyan_patch(mea, ref, cyanmea, cyanref);	

//create cyan calibration
	status = _compute_cyan(_linear, cyanmea, cyanref);
	if(status != MCO_SUCCESS)	return status;

	McoFree(cyanmea);
	McoFree(cyanref);

	return status;
}

McoStatus	ScanCal::_compute_cyan(PWlinear **linear, double *mea, double *ref)
{
	McoStatus status;
	double *rgbref;
	double white[3];
	
	white[0] = 96.42;
	white[1] = 100.0;
	white[2] = 82.49;

	//linearization stretch
	status = _linearstrech(3, linear, 12, mea);
	if(status != MCO_SUCCESS)	return status;
		
	rgbref = ref;
	//convert lab to xyz
	labtonxyzinplace(rgbref, 12);
	nxyztoxyzinplace(rgbref, white, 12);
	
	_cyan = new MultiLcal(3, 3, 3, func3);
	
	status = _cyan->compute(12, mea, ref);
	if(status != MCO_SUCCESS){
		delete _cyan;
		return status;
	}
	
	return MCO_SUCCESS;
}



McoStatus	ScanCal::_compute_local(PWlinear **linear, double *mea, double *ref)
{
	McoStatus status;
	long i, j;
	double *rgbref;
	double white[3];
	
	white[0] = 96.42;
	white[1] = 100.0;
	white[2] = 82.49;

	//linearization stretch
	status = _linearstrech(3, linear, 168, mea);
	if(status != MCO_SUCCESS)	return status;
		
	rgbref = ref;
	//convert lab to xyz
	labtonxyzinplace(rgbref, 168);
	nxyztoxyzinplace(rgbref, white, 168);
	
	_lcal = new MultiLcal*[12];
	if(!_lcal)	return MCO_MEM_ALLOC_ERROR;
	
	for(i = 0; i < 12; i++){
		_lcal[i] = new MultiLcal(3, 3, 3, func3);
	
		status = _lcal[i]->compute(36, &mea[12*3*i], &rgbref[12*3*i]);
		if(status != MCO_SUCCESS)	{
			for(j = 0; j < 12; j++)
				delete _lcal[j];
			return status;
		}
	}
	
	return MCO_SUCCESS;
}

//apply the calibration here
McoStatus	ScanCal::compute(long num, double *in, double *out)
{
	McoStatus status;

	status = _localcal(_linear, _gcal, _lcal, num, in, out);
	if(status != MCO_SUCCESS)
		return status;
	
	return MCO_SUCCESS;	
}

char *IT8KODAKINDEX[] = 
{
"A1","A2","A3","A4","A5","A6","A7","A8","A9","A10","A11","A12","A13","A14","A15","A16","A17","A18","A19",
"B1","B2","B3","B4","B5","B6","B7","B8","B9","B10","B11","B12","B13","B14","B15","B16","B17","B18","B19",
"C1","C2","C3","C4","C5","C6","C7","C8","C9","C10","C11","C12","C13","C14","C15","C16","C17","C18","C19",
"D1","D2","D3","D4","D5","D6","D7","D8","D9","D10","D11","D12","D13","D14","D15","D16","D17","D18","D19",
"E1","E2","E3","E4","E5","E6","E7","E8","E9","E10","E11","E12","E13","E14","E15","E16","E17","E18","E19",
"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15","F16","F17","F18","F19",
"G1","G2","G3","G4","G5","G6","G7","G8","G9","G10","G11","G12","G13","G14","G15","G16","G17","G18","G19",
"H1","H2","H3","H4","H5","H6","H7","H8","H9","H10","H11","H12","H13","H14","H15","H16","H17","H18","H19",
"I1","I2","I3","I4","I5","I6","I7","I8","I9","I10","I11","I12","I13","I14","I15","I16","I17","I18","I19","I20","I21","I22",
"J1","J2","J3","J4","J5","J6","J7","J8","J9","J10","J11","J12","J13","J14","J15","J16","J17","J18","J19","J20","J21","J22",
"K1","K2","K3","K4","K5","K6","K7","K8","K9","K10","K11","K12","K13","K14","K15","K16","K17","K18","K19","K20","K21","K22",
"L1","L2","L3","L4","L5","L6","L7","L8","L9","L10","L11","L12","L13","L14","L15","L16","L17","L18","L19","L20","L21","L22",
"M0","M1","M2","M3","M4","M5","M6","M7","M8","M9", "M10","M11","M12","M13","M14","M15","M16","M17","M18","M19","M20","M21",
"M22","M23"
};


McoStatus 	ScanCal::deltaE(int32 num, double* cmp, double *ref, double *avg, double *max, int32 *which)
{
	McoStatus status;
	long i, j;
	double tempmax, tempsum;
	double *cmpstart, *refstart;

	FILE *fs = fopen("de_debug.txt","w");
	if (!fs) return MCO_FAILURE;
	
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
		
		fprintf(fs,"%d\t%s\t%f\n", i, IT8KODAKINDEX[i], tempsum);
		
		*avg += tempsum;
		if(tempmax < tempsum){
			*which = i;
			tempmax = tempsum;
		}
		
		cmpstart += 3;
		refstart += 3;	
	}

	fclose(fs);
	
	*avg /= num;
	*max = tempmax;	
	
	return MCO_SUCCESS;
}

McoStatus ScanCal::_globalcal(Matrix &mat, long num, double *mea, double *ref)
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
	long start = num - 23;
	long end = num;

	_linear = new PWlinear*[3];	
	for(j = 0; j < 3; j++){
		for(i = start; i < end; i++){
			y[i-start] = rgbref[i*3 + j];
			x[i-start] = mea[i*3 + j];
		}
		_linear[j] = new PWlinear(LINEAR_NUM, x, y);
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

McoStatus ScanCal::_localcal(PWlinear **linear, MultiLcal *gcal, MultiLcal **lcal, long num, double *in, double *out)
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
		
		status = _linearstrech(3, linear, 1, rgb);
		if(status != MCO_SUCCESS)	return status;
		
		for(j = 0; j < 3; j++){
			if(rgb[j] < 0)	rgb[j] = 0;
			if(rgb[j] > 255)	rgb[j] = 255.0;
		}	
			
		status = gcal->apply(1, rgb, caled);
		if(status != MCO_SUCCESS)	return status;
		xyztonxyzinplace(caled, white, 1);
		nxyztoLabinplace(caled, 1);
		labtolch(caled, lch);
		
	
		if(lch[1] > GRAY_THRESHOLD){
			local = _select_local(lch);
			status = lcal[local]->apply(1, rgb, caled);
			if(status != MCO_SUCCESS)	return status;
			xyztonxyzinplace(caled, white, 1);
			nxyztoLabinplace(caled, 1);
		}

//fix cyan here, may introduce new bug
/*
		if(lch[1] > GRAY_THRESHOLD && lch[2] > 215 && lch[2] < 250){
			status = _cyan->apply(1, rgb, caled);
			if(status != MCO_SUCCESS)	return status;
			xyztonxyzinplace(caled, white, 1);
			nxyztoLabinplace(caled, 1);
		}
*/
		
		out[m] = caled[0];
		out[m+1] = caled[1];
		out[m+2] = caled[2];
		m += 3;
	}
			
	return MCO_SUCCESS;		
}


long ScanCal::_select_local(double *lch)
{
	long i;
	//the hue value of the location, including the start and end of hue
	double local_h[LOCAL_NUM*2];	

	i = 0;
	local_h[i++] = (15+65)/2;
	local_h[i++] = (40+93)/2;
	local_h[i++] = (65+120)/2;
	local_h[i++] = (93+160)/2;
	local_h[i++] = (120+190)/2;
	local_h[i++] = (160+230)/2;
	local_h[i++] = (190+275)/2;
	local_h[i++] = (230+295)/2;
	local_h[i++] = (275+320)/2;
	local_h[i++] = (295+345)/2;
	local_h[i++] = 347.5;
	local_h[i++] = 12.5;

	double min = 100;
	long index = 0;
	double diff;
	
	for(i = 0; i < 12; i++){
		diff = fabs(lch[2]-local_h[i]);
		if( diff < min){
			min = diff;
			index = i;
		}
	}
	
	return index;
}

