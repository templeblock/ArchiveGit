//#include <SANE.h>
#include "calibrate.h"
#include "monacoprint.h"
#include "mcomem.h"

#define NUM_BIAS_PATCHES	(0)

Linearcal::Linearcal(int32 dimension, int16 one_time, int32 total_loc)
{	
	long i;
	
	_err = MCO_MEM_ALLOC_ERROR;

	_weights = 0;
	_core_r = 0;		//core matrix, inverse matrix is needed for this matrix
	_core_g = 0;		//core matrix, inverse matrix is needed for this matrix
	_core_b = 0;		//core matrix, inverse matrix is needed for this matrix
	_ref_data_r = 0;//reference data for r part, left size of equation
	_ref_data_g = 0;//reference data for r part, left size of equation
	_ref_data_b = 0;//reference data for r part, left size of equation
	_refr = 0;
	_refg = 0;
	_refb = 0;
	_rm = 0;		//temporary matrix containing all the data
	_rmt = 0;

	//_weightfile = 0;

//added on 8/24
	_total_loc = total_loc; //total number of patches

	_patchloc = new int16[_total_loc];

	for(i = 0; i < _total_loc; i++)
		_patchloc[i] = i;
	_num_loc = _total_loc;	
	_cur_loc = 0;
	_one_time = one_time;

	//added on 8/17 for bias patch
	_num_bias = NUM_BIAS_PATCHES;
	_bias_id[0] = 180; //I4
	_bias_times[0] = NUM_BIAS_PATCHES; 
	//end of modification

	// set to null in case of error
	_weights = 0L;
	_core_r = 0L;
	_core_g = 0L;
	_core_b = 0L;
	_core_b = 0L;
	_rm = 0L;
	_rmt = 0L;
	_ref_data_r = 0L;
	_ref_data_g = 0L;
	_ref_data_g = 0L;
	_refr = 0L;
	_refg = 0L;
	_refb = 0L;

	_dimension = dimension;
	_weights = new Matrix(3, _dimension); //weights matrix
	if(!_weights || _weights->get_status() != MCO_SUCCESS) goto bail;
	
	_core_r = new Matrix(_dimension);	//left matrix
	if(!_core_r || _core_r->get_status() != MCO_SUCCESS) goto bail;

	_core_g = new Matrix(_dimension);
	if(!_core_g || _core_g->get_status() != MCO_SUCCESS) goto bail;

	_core_b = new Matrix(_dimension);
	if(!_core_b || _core_b->get_status() != MCO_SUCCESS) goto bail;

	_rm =  new Matrix(_dimension, _num_loc+_num_bias);	
	if(!_rm || _rm->get_status() != MCO_SUCCESS) goto bail;

	_rmt =  new Matrix(_num_loc+_num_bias, _dimension);	
	if(!_rmt || _rmt->get_status() != MCO_SUCCESS) goto bail;
	
	_ref_data_r = new Matrix(_dimension, 1); //right matrix
	if(!_ref_data_r || _ref_data_r->get_status() != MCO_SUCCESS) goto bail;

	_ref_data_g = new Matrix(_dimension, 1);
	if(!_ref_data_g || _ref_data_g->get_status() != MCO_SUCCESS) goto bail;

	_ref_data_b = new Matrix(_dimension, 1);
	if(!_ref_data_b || _ref_data_b->get_status() != MCO_SUCCESS) goto bail;

	_refr = new Matrix(_num_loc+_num_bias, 1);
	if(!_refr || _refr->get_status() != MCO_SUCCESS) goto bail;

	_refg = new Matrix(_num_loc+_num_bias, 1);
	if(!_refg || _refg->get_status() != MCO_SUCCESS) goto bail;

	_refb = new Matrix(_num_loc+_num_bias, 1);
	if(!_refb || _refb->get_status() != MCO_SUCCESS) goto bail;
	
	//if use all of the patches

	_err = MCO_SUCCESS;
	return;
bail:	
	_err = MCO_MEM_ALLOC_ERROR;
}

Linearcal::~Linearcal(void)
{
	_deallocate();
	if (_patchloc) delete _patchloc;
}

void Linearcal::_deallocate(void)
{
	if(_weights){
		delete _weights;
		_weights = 0L;
	}	
		
	if(_core_r){
		delete _core_r;	
		_core_r = 0L;
	}	

	if(_core_g){
		delete _core_g;	
		_core_g = 0L;
	}	

	if(_core_b){
		delete _core_b;	
		_core_b = 0L;
	}	
	
	if(_ref_data_r){
		delete _ref_data_r;
		_ref_data_r = 0;
	}
	if(_ref_data_g){
		delete _ref_data_g;
		_ref_data_g = 0;
	}
	if(_ref_data_b){
		delete _ref_data_b;
		_ref_data_b = 0;
	}

	if(_rm){
		delete _rm;
		_rm = 0;
	}
	if(_rmt){
		delete _rmt;
		_rmt = 0;
	}
	if(_refr){
		delete _refr;
		_refr = 0;
	}
	if(_refg){
		delete _refg;
		_refg = 0;
	}
	if(_refb){
		delete _refb;
		_refb = 0;
	}
//	if(_weightfile){
//		delete _weightfile;
//		_weightfile = 0;
//	}
}			

McoStatus Linearcal::loaddirect(double* rgbcolor, double *refrgbin, int16* end)
{
	McoStatus status;
	int32 i, j, k, ip, head;
	int16	copy_patch;
	double	sum;
	double 	xref, yref, zref;
	int		io_err;
	double 	*meargb;
	double	*refrgb;
	double rmea, gmea, bmea;	//first order
	double rgmea, rbmea, gbmea;	//2nd order
	double rmea2, gmea2, bmea2;	//2nd order
	double rmea3, gmea3, bmea3;	//3rd
	double r2gmea, r2bmea, rg2mea, rgbmea, rb2mea;	//3rd
	double g2bmea, gb2mea;	//3rd
			
	//buffer for holding the measured rgb and reference rgb values
	//memcpy( (char*)refrgb, (char*)refrgbin, _total_loc*3*sizeof(double));

	// gamma correction
	
	meargb = rgbcolor;
	refrgb = refrgbin;

	if(_cur_loc == _num_loc){
		*end = 1;
		_rmt->copyT(*_rm);//transverse
		
		//compute the left matrix
		(*_core_r) = (*_rm)*(*_rmt);
		(*_core_g) = (*_rm)*(*_rmt);
		(*_core_b) = (*_rm)*(*_rmt);

		//compute the right vector matrix	
		(*_ref_data_r) = (*_rm)*(*_refr);
		(*_ref_data_g) = (*_rm)*(*_refg);
		(*_ref_data_b) = (*_rm)*(*_refb);

		//free up some big chunk of memory	
		delete _rm;
		_rm = 0;
		delete _rmt;
		_rmt = 0;
		delete _refr;
		_refr = 0;
		delete _refg;
		_refg = 0;
		delete _refb;
		_refb = 0;

		return MCO_SUCCESS;
	}

	copy_patch = _one_time;
	if(_cur_loc+copy_patch >= _num_loc)
		copy_patch = _num_loc - _cur_loc;  

	for(i = 0; i < copy_patch; i++){
	
		ip = i+1+_cur_loc;
		head = _patchloc[i+_cur_loc]*3;	
		rmea = (double)meargb[head++];
		gmea = (double)meargb[head++];
		bmea = (double)meargb[head];

		rmea2 = rmea*rmea;		
		rgmea = rmea*gmea;
		rbmea = rmea*bmea;
		gmea2 = gmea*gmea;
		gbmea = gmea*bmea;
		bmea2 = bmea*bmea;
		
		rmea3 = rmea*rmea2;
		r2gmea =rmea2*gmea;
		r2bmea =rmea2*bmea;
		rg2mea =rmea*gmea2;
		rgbmea =rmea*gmea*bmea;
		rb2mea =rmea*bmea2;
		gmea3 = gmea*gmea2;
		g2bmea = gmea2*bmea;
		gb2mea = gmea*bmea2;
		bmea3 = bmea*bmea2;
		
		zref = refrgb[head--];
		yref = refrgb[head--];
		xref = refrgb[head];

		_rm->setval(1,ip, rmea);
		_rm->setval(2,ip, gmea);
		_rm->setval(3,ip, bmea);
		_rm->setval(4,ip, rmea2);
		_rm->setval(5,ip, rgmea);
		_rm->setval(6,ip, rbmea);
		_rm->setval(7,ip, gmea2);
		_rm->setval(8,ip, gbmea);
		_rm->setval(9,ip, bmea2);

		if(_dimension == 20){
			_rm->setval(10,ip, rmea3);
			_rm->setval(11,ip, r2gmea);
			_rm->setval(12,ip, r2bmea);
			_rm->setval(13,ip, rg2mea);
			_rm->setval(14,ip, rgbmea);
			_rm->setval(15,ip, rb2mea);
			_rm->setval(16,ip, gmea3);
			_rm->setval(17,ip, g2bmea);
			_rm->setval(18,ip, gb2mea);
			_rm->setval(19,ip, bmea3);
			_rm->setval(20,ip, 1);
		}
		else if(_dimension == 10){
			_rm->setval(10,ip, 1);
		}	
		else
			return MCO_FAILURE;
	
		_refr->setval(ip,1, xref);
		_refg->setval(ip,1, yref);
		_refb->setval(ip,1, zref);
	}
	
	_cur_loc += copy_patch;

	return MCO_SUCCESS;		
}

//memory leak for_r, for_g, for_b
McoStatus Linearcal::compute(void)
{
	McoStatus status;
	int32 i, j;
	double val;
	
	_core_r->inv();
	if( (status = _core_r->get_status()) != MCO_SUCCESS) //singular or ?
		return status;
	_core_g->inv();
	if( (status = _core_g->get_status()) != MCO_SUCCESS) //singular or ?
		return status;
	_core_b->inv();
	if( (status = _core_b->get_status()) != MCO_SUCCESS) //singular or ?
		return status;
		
	Matrix refr = (*_core_r)*(*_ref_data_r);
	Matrix refg = (*_core_g)*(*_ref_data_g);
	Matrix refb = (*_core_b)*(*_ref_data_b);
	
	for(i = 1; i <= _dimension; i++){
		refr.getval(i, 1, &val);
		_weights->setval(1, i, val);
		refg.getval(i, 1, &val);
		_weights->setval(2, i, val);
		refb.getval(i, 1, &val);
		_weights->setval(3, i, val);
	}	

	//save the weights to the struct	
	status = _weights->savestruct(_rw);
	if(status != MCO_SUCCESS) return status;

	return MCO_SUCCESS;		
}


//which parameter channel should be used, red, green or blue
void Linearcal::_setparmchannel(int16 cur_channel)
{
	_cur_channel = cur_channel;
}

double* Linearcal::_getparmchannel(void)
{
	switch(_cur_channel){
		case(1): //red
			return (_rw);

		case(2): //green
			return (&_rw[_dimension]);

		case(3): //blue
			return (&_rw[_dimension*2]);

		default: //red, should not come here
			return (_rw);
	}
}

//set start location in one channel
void Linearcal::_setstartloc(int16 start_loc)
{
	_start_loc = start_loc;
}

//set start location in one channel
int16	Linearcal::_getstartloc(void){
	return _start_loc;
}



//may have a bug, because of train_r,g,b size are too large
void Linearcal::testR(double* rgbcolor, double *refrgb, 
	double *rv_r, double *rv_g, double *rv_b)
{
	double rsum, gsum, bsum;
	double ssy_r, ssy_g, ssy_b;
	double sse_r, sse_g, sse_b;
	double *train_r, *train_g, *train_b; //beware of size; 
	double tempd;
	double *r_nw, *g_nw, *b_nw;
	double 	*meargb;

	double	r, g, b;
	double rg, rb, gb;	//2nd order
	double r2, g2, b2;	//2nd order
	double r3, g3, b3;	//3rd
	double r2g, r2b, rg2, rgb, rb2;	//3rd
	double g2b, gb2;	//3rd
	int32 	i, j, head;
	double hold[20];

	//create some temporary memory
	train_r = (double*)McoMalloc(_total_loc*sizeof(double));
	train_g = (double*)McoMalloc(_total_loc*sizeof(double));
	train_b = (double*)McoMalloc(_total_loc*sizeof(double));
	
	//setup the parameter
/*
	r_nw = _rw;
	g_nw = &_rw[_dimension];
	b_nw = &_rw[_dimension*2];
*/
	meargb = rgbcolor;
	
	//compute the average of the ref rgb values
	rsum = 0.0;
	gsum = 0.0;
	bsum = 0.0;
	for(i = 0; i < _num_loc; i++){ 
		head = i*3;
		rsum += refrgb[head++];
		gsum += refrgb[head++];
		bsum += refrgb[head];
	}
	
	rsum /= _num_loc;
	gsum /= _num_loc;
	bsum /= _num_loc;
	
	//compute the ssy
	ssy_r = 0;
	ssy_g = 0;
	ssy_b = 0;
	for(i = 0; i < _num_loc; i++){ 
		head = i*3;
		tempd = refrgb[head] - rsum;
		ssy_r += tempd*tempd;
		head++;

		tempd = refrgb[head] - gsum;
		ssy_g += tempd*tempd;
		head++;

		tempd = refrgb[head] - bsum;
		ssy_b += tempd*tempd;
	}
	
	for(i = 0; i < _num_loc; i++){
		head = _patchloc[i]*3;	
		r = meargb[head++];
		g = meargb[head++];
		b = meargb[head];
	
		r2 = r*r;		
		rg = r*g;
		rb = r*b;
		g2 = g*g;
		gb = g*b;
		b2 = b*b;
		
		r3 = r*r2;
		r2g =r2*g;
		r2b =r2*b;
		rg2 =r*g2;
		rgb =r*g*b;
		rb2 =r*b2;
		g3 = g*g2;
		g2b = g2*b;
		gb2 = g*b2;
		b3 = b*b2;

		hold[0] = r;
		hold[1] = g;
		hold[2] = b;
	
		hold[3] = r2;
		hold[4] = rg;
		hold[5] = rb;
		hold[6] = g2;
		hold[7] = gb;
		hold[8] = b2;
	
		if(_dimension == 10){
			hold[9] = 1;
		}
		else if(_dimension == 20){
			hold[9] = r3;
			hold[10] = r2g;
			hold[11] = r2b;
			hold[12] = rg2;
			hold[13] = rgb;
			hold[14] = rb2;
			hold[15] = g3;
			hold[16] = g2b;
			hold[17] = gb2;
			hold[18] = b3;
			hold[19] = 1;
		}
		else
			return;

		r_nw = _rw;
		train_r[i] = 0.0;
		for( j = 0; j < _dimension; j++){
			train_r[i] += hold[j]*r_nw[j];
		}
 
		r_nw = &_rw[_dimension];
		train_g[i] = 0.0;
		for( j = 0; j < _dimension; j++){
			train_g[i] += hold[j]*r_nw[j];
		}

		r_nw = &_rw[_dimension*2];
		train_b[i] = 0.0;
		for( j = 0; j < _dimension; j++){
			train_b[i] += hold[j]*r_nw[j];
		}

/*
		train_r[i] =r*r_nw[0]+g*r_nw[1]+b*r_nw[2]+
			r2*r_nw[3]+rg*r_nw[4]+rb*r_nw[5]+g2*r_nw[6]+gb*r_nw[7]+b2*r_nw[8]+
			r3*r_nw[9]+r2g*r_nw[10]+r2b*r_nw[11]+rg2*r_nw[12]+
			rgb*r_nw[13]+rb2*r_nw[14]+g3*r_nw[15]+g2b*r_nw[16]+
			gb2*r_nw[17]+b3*r_nw[18]+r_nw[19];
			
		train_g[i] =r*g_nw[0]+g*g_nw[1]+b*g_nw[2]+
			r2*g_nw[3]+rg*g_nw[4]+rb*g_nw[5]+g2*g_nw[6]+gb*g_nw[7]+b2*g_nw[8]+
			r3*g_nw[9]+r2g*g_nw[10]+r2b*g_nw[11]+rg2*g_nw[12]+
			rgb*g_nw[13]+rb2*g_nw[14]+g3*g_nw[15]+g2b*g_nw[16]+
			gb2*g_nw[17]+b3*g_nw[18]+g_nw[19];
			
		
		train_b[i] =r*b_nw[0]+g*b_nw[1]+b*b_nw[2]+
			r2*b_nw[3]+rg*b_nw[4]+rb*b_nw[5]+g2*b_nw[6]+gb*b_nw[7]+b2*b_nw[8]+
			r3*b_nw[9]+r2g*b_nw[10]+r2b*b_nw[11]+rg2*b_nw[12]+
			rgb*b_nw[13]+rb2*b_nw[14]+g3*b_nw[15]+g2b*b_nw[16]+
			gb2*b_nw[17]+b3*b_nw[18]+b_nw[19];
*/
	}
			
	//compute the sse
	sse_r = 0;
	sse_g = 0;
	sse_b = 0;
	for(i = 0; i < _num_loc; i++){
		head = i*3;
		tempd = refrgb[head] - train_r[i];
		sse_r += tempd*tempd;
		head++;

		tempd = refrgb[head] - train_g[i];
		sse_g += tempd*tempd;
		head++;

		tempd = refrgb[head] - train_b[i];
		sse_b += tempd*tempd;
	}
	
	//compute the r value
	*rv_r = (ssy_r - sse_r)/ssy_r;
	*rv_g = (ssy_g - sse_g)/ssy_g;
	*rv_b = (ssy_b - sse_b)/ssy_b;

	McoFree(train_r);
	McoFree(train_g);
	McoFree(train_b);
}


McoStatus Linearcal::saveLabtoCMYK(McoHandle cmykH)
{
	long 	i, j, k, m;
	double	stepL;
	double 	L, a, b;
	double  *cmyk;

	
	cmyk = (double*)McoLockHandle(cmykH);
	stepL = 100.0/32.0;
	
	for(i = 0; i < 33; i++){
		L = i*stepL;
		for(j = -16; j < 16; j++){				
			a = j*8;
			for(m = -16; m < 16; m++){
				b = m*8;
				//save to a structure
				_applyf(L, a, b, cmyk);
				cmyk += 4;
			}
			
			_applyf(L, a, 127.0, cmyk);
			cmyk += 4;
		}

		for(m = -16; m < 16; m++){
			b = m*8;
			//save to a structure
			_applyf(L, 127.0, b, cmyk);
			cmyk += 4;
		}
		
		_applyf(L, 127.0, 127.0, cmyk);
		cmyk += 4;		
	}

	McoUnlockHandle(cmykH);
			
	return MCO_SUCCESS;
}


McoStatus Linearcal::saveLabtoCMYK2(McoHandle cmykH,  McoHandle labH)
{
	long 	i, j, k, m;
	double 	L, a, b;
	double  *cmyk;
	double  *lab;
	
	cmyk = (double*)McoLockHandle(cmykH);
	lab = (double*)McoLockHandle(labH);

	for( i = 0; i < TABLE_ENTRIES; i++){
		L = *lab++;
		a = *lab++;
		b = *lab++;
		_applyf(L, a, b, cmyk);
		//cmyk[0] = L;
		//cmyk[1] = a;
		//cmyk[2] = b;
		//cmyk[3] = 0;
		cmyk += 4;
	}

	McoUnlockHandle(labH);
	McoUnlockHandle(cmykH);
			
	return MCO_SUCCESS;
}

McoStatus Linearcal::saveLabtoCMYK3(McoHandle cmykH,  McoHandle labH)
{
	long 	i, j, k, m;
	double 	L, a, b;
	double  *cmyk;
	double  *lab;
	
	cmyk = (double*)McoLockHandle(cmykH);
	lab = (double*)McoLockHandle(labH);

	for( i = 0; i < TABLE_ENTRIES; i++){
		L = *lab++;
		a = *lab++;
		b = *lab++;
		//_applyf(L, a, b, cmyk);
		cmyk[0] = L;
		cmyk[1] = a;
		cmyk[2] = b;
		cmyk[3] = 0;
		cmyk += 4;
	}

	McoUnlockHandle(labH);
	McoUnlockHandle(cmykH);
			
	return MCO_SUCCESS;
}


//added on 8/24, 
McoStatus Linearcal::gray_to_cmy(double *cmyk)
{
	long 	i, j, k, m;
	double 	L, a, b;
	
	for( i = 0; i < 101; i++){
		L = i;
		a = 0.0;
		b = 0.0;
		_applyf(L, a, b, cmyk);
		cmyk += 4;
	}

	return MCO_SUCCESS;
}


//given a rgb(Lab), get a cmyk, k=0 at this stage
//no clipping, clipping is defered to the next stage
void Linearcal::_applyf(double r, double g, double b, double *cmyk)
{
	long i, j;
	double rg, rb, gb;	//2nd order
	double r2, g2, b2;	//2nd order
	double r3, g3, b3;	//3rd
	double r2g, r2b, rg2, rgb, rb2;	//3rd
	double g2b, gb2;	//3rd
	double *r_nw, *g_nw, *b_nw;
	double hold[20];
/*	
	//setup the parameter
	r_nw = _rw;
	g_nw = &_rw[20];
	b_nw = &_rw[40];
*/

	//prepare
	r2 = r*r;		
	rg = r*g;
	rb = r*b;
	g2 = g*g;
	gb = g*b;
	b2 = b*b;
	
	r3 = r*r2;
	r2g =r2*g;
	r2b =r2*b;
	rg2 =r*g2;
	rgb =r*g*b;
	rb2 =r*b2;
	g3 = g*g2;
	g2b = g2*b;
	gb2 = g*b2;
	b3 = b*b2;

	hold[0] = r;
	hold[1] = g;
	hold[2] = b;

	hold[3] = r2;
	hold[4] = rg;
	hold[5] = rb;
	hold[6] = g2;
	hold[7] = gb;
	hold[8] = b2;

	if(_dimension == 10){
		hold[9] = 1;
	}
	else if (_dimension == 20){
		hold[9] = r3;
		hold[10] = r2g;
		hold[11] = r2b;
		hold[12] = rg2;
		hold[13] = rgb;
		hold[14] = rb2;
		hold[15] = g3;
		hold[16] = g2b;
		hold[17] = gb2;
		hold[18] = b3;
		hold[19] = 1;
	}
	else
		return;

//apply the function to it
/*
	r_nw = _rw;
	cmyk[0] = 0.0;
	for( j = 0; j < 20; j++)
		cmyk[0] += hold[j]*r_nw[j];

	r_nw = &_rw[20];
	cmyk[1] = 0.0;
	for( j = 0; j < 20; j++)
		cmyk[1] += hold[j]*r_nw[j];

	r_nw = &_rw[40];
	cmyk[2] = 0.0;
	for( j = 0; j < 20; j++)
		cmyk[2] += hold[j]*r_nw[j];

	cmyk[3] = 0.0;
*/
	r_nw = _rw;
	for( i = 0; i < 3; i++){
		cmyk[i] = 0;
		for( j = 0; j < _dimension; j++){
			cmyk[i] += hold[j]*r_nw[j];
		}
/*
		cmyk[i] =r*r_nw[0]+g*r_nw[1]+b*r_nw[2]+
			r2*r_nw[3]+rg*r_nw[4]+rb*r_nw[5]+g2*r_nw[6]+gb*r_nw[7]+b2*r_nw[8]+
			r3*r_nw[9]+r2g*r_nw[10]+r2b*r_nw[11]+rg2*r_nw[12]+
			rgb*r_nw[13]+rb2*r_nw[14]+g3*r_nw[15]+g2b*r_nw[16]+
			gb2*r_nw[17]+b3*r_nw[18]+r_nw[19];
*/
		r_nw += _dimension;
	}
	cmyk[3] = 0.0;

}


//given a rgb(Kab), get back a' and b'
//no clipping, clipping is defered to the next stage
void Linearcal::_applyf_ab(double r, double g, double b, double *ab)
{
	long i, j;
	double rg, rb, gb;	//2nd order
	double r2, g2, b2;	//2nd order
	double r3, g3, b3;	//3rd
	double r2g, r2b, rg2, rgb, rb2;	//3rd
	double g2b, gb2;	//3rd
	double *r_nw, *g_nw, *b_nw;
	double hold[20];

	//prepare
	r2 = r*r;		
	rg = r*g;
	rb = r*b;
	g2 = g*g;
	gb = g*b;
	b2 = b*b;
	
	r3 = r*r2;
	r2g =r2*g;
	r2b =r2*b;
	rg2 =r*g2;
	rgb =r*g*b;
	rb2 =r*b2;
	g3 = g*g2;
	g2b = g2*b;
	gb2 = g*b2;
	b3 = b*b2;

	hold[0] = r;
	hold[1] = g;
	hold[2] = b;

	hold[3] = r2;
	hold[4] = rg;
	hold[5] = rb;
	hold[6] = g2;
	hold[7] = gb;
	hold[8] = b2;

	if(_dimension == 10){
		hold[9] = 1;
	}
	else if (_dimension == 20){
		hold[9] = r3;
		hold[10] = r2g;
		hold[11] = r2b;
		hold[12] = rg2;
		hold[13] = rgb;
		hold[14] = rb2;
		hold[15] = g3;
		hold[16] = g2b;
		hold[17] = gb2;
		hold[18] = b3;
		hold[19] = 1;
	}
	else
		return;

	r_nw = _rw;
	for( i = 0; i < 2; i++){
		ab[i] = 0;
		for( j = 0; j < _dimension; j++){
			ab[i] += hold[j]*r_nw[j];
		}
		r_nw += _dimension;
	}

}

