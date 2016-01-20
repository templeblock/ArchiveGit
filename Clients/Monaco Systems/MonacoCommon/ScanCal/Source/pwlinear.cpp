#include "pwlinear.h"


PWlinear::PWlinear(int32 size, double *x, double *y)
{
	_x = 0;
	_y = 0;
	_size = 0;

	reset(size, x, y);
}

PWlinear::~PWlinear()
{
	_deallocate();
}

void PWlinear::_deallocate(void)
{
	if(_x){
		delete []_x;
		_x = 0;
	}
	if(_y){
		delete []_y;
		_y = 0;
	}		
}

void PWlinear::_sequence(void)
{
	long i, j, k;
	double tempx, tempy;

	//using the bubble sort
	for(i = 1; i < _size; i++){
		if(_x[i] < _x[i-1]){			
			j = i-1;
			while(j > 0 && _x[i] < _x[j])
				j--;
				
			//shift to larger side
			tempx = _x[i];
			tempy = _y[i];
			for(k = i-1; k >= j; k--){
				_x[k+1] = _x[k];
				_y[k+1] = _y[k];
			}
				
			_x[j] = tempx;
			_y[j] = tempy;
		}		
	}	
					
}		

McoStatus PWlinear::reset(int32 size, double *x, double *y)
{
	long i;
	_err = MCO_SUCCESS;
	
	_deallocate();
	
	if(size <= 0) {
		_err = MCO_FAILURE;
		return _err;
	}
		
	_size = size;
	_x = new double[_size];
	if(!_x){
		_err = MCO_MEM_ALLOC_ERROR;
		return _err;
	}
	else{
		for(i = 0; i < _size; i++)
			_x[i] = x[i];
	}
	
	_y = new double[_size];
	if(!_y){
		_err = MCO_MEM_ALLOC_ERROR;
		return _err;
	}
	else{
		for(i = 0; i < _size; i++)
			_y[i] = y[i];
	}
		
	if(_err == MCO_SUCCESS)
		_sequence();
		
	return _err;
}

McoStatus PWlinear::apply(int32 num, double *in)
{
	long i, j, index;
	double *sin, *sout, *sx;
	double w;
	
	if(_err != MCO_SUCCESS)	return _err;
	
	sin = in;
		
	for(i = 0; i < num; i++){
		index = _size;
		sx = _x;
		
		for(j = 0; j < _size; j++){
			if(*sin < *sx){
				index = j; 
				break;
			}
			sx++;
		}
		
		if(index == 0){	//extropolate to the low end
			index = 1;
		}
		if(index == _size){//extropolate to the high end
			index = _size -1;		
		}

		if(_x[index] != _x[index-1])
			sin[0] = ( (_y[index] - _y[index-1])*sin[0] + _y[index-1]*_x[index] - _y[index]*_x[index-1])/(_x[index] - _x[index-1]);		
		else
			sin[0] = _y[index];
		sin++;
	}	
	
	return _err;
}	

McoStatus PWlinear::apply(int32 num, double *in, double *out)
{
	long i, j, index;
	double *sin, *sout, *sx;
	double w;
	
	if(_err != MCO_SUCCESS)	return _err;
	
	sin = in;
	sout = out;
		
	for(i = 0; i < num; i++){
		index = _size;
		sx = _x;
		
		for(j = 0; j < _size; j++){
			if(*sin < *sx){
				index = j; 
				break;
			}
			sx++;
		}
		
		if(index == 0){	//extropolate to the low end
			index = 1;
		}
		if(index == _size){//extropolate to the high end
			index = _size -1;		
		}

		if(_x[index] != _x[index-1])
			*sout = ( (_y[index] - _y[index-1])*sin[0] + _y[index-1]*_x[index] - _y[index]*_x[index-1])/(_x[index] - _x[index-1]);		
		else
			*sout = _y[index];
		sin++;
		sout++;
	}	
	
	return _err;
}	
