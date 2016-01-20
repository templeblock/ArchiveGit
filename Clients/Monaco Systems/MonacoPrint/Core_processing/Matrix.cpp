#include <math.h>
#include "matrix.h"
#include "mcomem.h"


Matrix::Matrix(int32 rows)
{
	_err = MCO_SUCCESS;

	if( rows <= 0 ){
		_err = MCO_FAILURE;
		return;
	}
 
	_row = _col = rows;
	_allocate();
	if(_err == MCO_SUCCESS){		
		for(int32 i = 0; i < _row; i++)
			for(int32 j = 0; j < _col; j++)
				_m[i][j] = 0;
	}		
}


Matrix::Matrix(int32 rows, int32 cols)
{
	_err = MCO_SUCCESS;

	if( rows <= 0 || cols <= 0){
		_err = MCO_FAILURE;
		return;
	}

	_row = rows;
	_col = cols;
	
	_allocate();
	if(_err == MCO_SUCCESS){		
		for(int32 i = 0; i < _row; i++)
			for(int32 j = 0; j < _col; j++)
				_m[i][j] = 0;
	}		
}

Matrix::Matrix(int32 rows, int32 cols, double val)
{
	_err = MCO_SUCCESS;

	if( rows <= 0 || cols <= 0){
		_err = MCO_FAILURE;
		return;
	}

	_row = rows;
	_col = cols;
	
	_allocate();
	if(_err == MCO_SUCCESS){		
		for(int32 i = 0; i < _row; i++)
			for(int32 j = 0; j < _col; j++)
				_m[i][j] = val;
	}		
}

//added on 8/12
/*
Matrix::Matrix(int32 rows, double val)
{
	_err = MCO_SUCCESS;
	_row = _col = rows;
	
	_allocate();
	if(_err == MCO_SUCCESS){		
		for(int i = 0; i < _row; i++)
			for(int32 j = 0; j < _col; j++)
				_m[i][j] = val;
	}		
*/
//end of modification


Matrix::Matrix(Matrix& inmatrix)
{
	_err = MCO_SUCCESS;
	_row = inmatrix._row;
	_col = inmatrix._col;

	_allocate();
	if(_err == MCO_SUCCESS){		
		for(int32 i = 0; i < _row; i++)
			for(int32 j = 0; j < _col; j++)
				_m[i][j] = inmatrix._m[i][j];
	}		
}


Matrix::~Matrix(void)
{
	_deallocate();
}

McoStatus Matrix::set(int32 rows, int32 cols)
{
	_deallocate();
	_err = MCO_SUCCESS;
	
	if( rows <= 0 || cols <= 0){
		_err = MCO_FAILURE;
		return _err;
	}

	_row = rows;
	_col = cols;
	_allocate();
	
	return _err;
}

//_row and _col must be set before calling this method
void Matrix::_allocate(void)
{
	_m =  (double**)McoMalloc(sizeof(double*)*_row);

	if(!_m)
		_err = MCO_MEM_ALLOC_ERROR;
	else{
		for(int32 i = 0; i < _row; i++){
			_m[i] = (double*)McoMalloc(sizeof(double)*_col);
			if(!_m[i]){	//not enough memory
				for(int32 j = i-1; j >= 0; j--)
					McoFree((void *) _m[j]);

				McoFree((void *) _m);
				_m = 0;
				_err = MCO_MEM_ALLOC_ERROR;
				break;
			}
		}
	}

	//modified on 8/12 to take out smatrix class
	//for square matrix, also need to allocate for the
	//additional memory 	
	_indx = 0;
	if( _row == _col){
		if(_err == MCO_SUCCESS){
			_indx = (double*)McoMalloc(sizeof(double)*_row);
			if(!_indx){
				_deallocate();
				_err = MCO_MEM_ALLOC_ERROR;
				return;
			}
		}
	}
	//end of modification

}

void Matrix::_deallocate(void)
{
	if(_m){
		for(int32 i = 0; i < _row; i++){
			McoFree((void *) _m[i]);
		}
		McoFree((void *) _m);
		_m = 0;
	}

	//modified on 8/12 to take out smatrix class
	//for square matrix, also need to deallocate for the
	//additional memory 	
	if(_indx){
		McoFree((void *) _indx);
		_indx = 0;
	}

}


void Matrix::_set_status(McoStatus status)	//set current error status
{
	_err = status;
}	

McoStatus Matrix::get_status(void)	//check current error status
{
	return _err;
}
	
//assign part of value of one matrix to another matrix
//the two matrix does not to have same dimension
//inrow, incol are input copying start point
//row, col are original matrix start point
//numrow, numcol are how many rows and cols to copy
McoStatus Matrix::assignval(Matrix& inmatrix, int32 inrow, int32 incol,
						int32 row, int32 col, int32 numrow, int32 numcol)
{
	int32 i, j;
	int32 rowend, colend;
	
	rowend = row + numrow -1;
	colend = col + numcol -1;
	inrow--;
	incol--;
	for(i = row-1; i < rowend; i++)
		for(j = col-1; j < colend; j++)
			_m[i][j] = inmatrix._m[inrow+i][incol+j];						

	return MCO_SUCCESS;
}	
	
//note: if inmatrix has different row and col from this matrix
//this matrix is cast to has the same row and col as inmatrix
Matrix& Matrix::operator=(Matrix& inmatrix)
{
	if( _err != MCO_SUCCESS)
		return *this;
		
	int32 inrow = inmatrix._row;
	int32 incol = inmatrix._col;	
	
	if(_row != inrow || _col != incol){

//modified on 8/12 to take out the smatrix class
		_row = inrow;
		_col = incol;
		_allocate();

		if(_err == MCO_SUCCESS){				
			for(int32 i = 0; i < _row; i++){
				for(int32 j = 0; j < _col; j++){
					_m[i][j] = inmatrix._m[i][j];
				}
			}		
		}
//end of modification		
	}
	
	else{
		for(int32 i = 0; i < _row; i++){
			for(int32 j = 0; j < _col; j++){
				_m[i][j] = inmatrix._m[i][j];
			}
		}
	}
	
	return *this;
}			
			

//single matrix operation		
Matrix& operator*(Matrix& inmatrix, double inval)
{
	int32 _row = inmatrix._row;
	int32 _col = inmatrix._col;
	
	for(int32 i = 0; i < _row; i++){
		for(int32 j = 0; j < _col; j++){
			inmatrix._m[i][j] *= inval;
		}
	}
	return inmatrix;
}
			
Matrix& operator*(double inval, Matrix& inmatrix)
{
int32	i,j;

	int32 _row = inmatrix._row;
	int32 _col = inmatrix._col;
	
	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			inmatrix._m[i][j] *= inval;
		}
	}
	return inmatrix;
}
	

Matrix& Matrix::operator-(void)
{
int32	i,j;

	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			_m[i][j] = -_m[i][j];
		}
	}
	
	return *this;
}
	

Matrix& Matrix::operator/(double val)
{
	int32	i,j;
	if(val == 0.0)
		return *this;
		
	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			_m[i][j] /= val;
		}
	}
	
	return *this;
}

//transverse of matrix
Matrix& Matrix::T(void)
{
	int32	i,j;
	double ** newm;
	double tempval;

//added on 8/12 to take out smatrix class	
	if( _row == _col){
		for(i = 0; i < _row; i++){
			for(j = i; j < _col; j++){
				tempval = _m[i][j];
				_m[i][j] = _m[j][i];
				_m[j][i] = tempval;
			}
		}
	
		return *this;
	}
//end of modification

	
	newm = (double**)McoMalloc(sizeof(double*)*_col);
	if(!newm){
		_set_status(MCO_MEM_ALLOC_ERROR);
		return *this;
	}
		
	for(i = 0; i < _col; i++){
		newm[i] = (double*)McoMalloc(sizeof(double)*_row);
		if(!newm[i]){
			for(j = i-1; j >= 0; j--)
				McoFree((void *) newm[j]);
			McoFree((void *) newm);
		}
	}
			
	for( i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			newm[j][i] = _m[i][j];
		}
	}
	
	for(i = 0; i < _row; i++){
		McoFree((void *) _m[i]);
	}
	McoFree((void *) _m);
	
	_m = newm;
	int32 temprow = _col;
	_col = _row;
	_row = temprow;
	
	return *this;
}

//copy transverse of a matrix to this matrix
Matrix& Matrix::copyT(Matrix& inmatrix)
{
	int32	i,j;
	
	if(_row != inmatrix._col || _col != inmatrix._row){
		_err = MCO_FAILURE;
		return *this;
	}
			
	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			_m[i][j] = inmatrix._m[j][i];
		}
	}
	
	return *this;
}

//matrix exchange row1 and row2
Matrix& Matrix::exchangerow(int16 row1, int16 row2)
{
	double *temp;
	
	if( row1 <= 0 || row2 <= 0 || _row+1 < row1 || _row+1 < row2)
		return *this;
		
	temp = _m[row1-1];
	_m[row1-1] = _m[row2-1];
	_m[row2-1] = temp;
	return *this;
}		
	
//matrix exchange col1 and col2
Matrix& Matrix::exchangecol(int16 col1, int16 col2)
{
	double temp;
	
	if( col1 <= 0 || col2 <= 0 || _col+1 < col1 || _col+1 < col2)
		return *this;
		
	for(int32 i = 0; i < _row; i++){
		temp = _m[i][col1-1];	
		_m[i][col1-1] = _m[i][col2-1];
		_m[i][col2-1] = temp;
	}
	
	return *this;
}

//matrix copy row1 to row2
Matrix& Matrix::copyrow(int16 row1, int16 row2)
{	
	if( row1 <= 0 || row2 <= 0 || _row+1 < row1 || _row+1 < row2)
		return *this;
	
	for(int32 i = 0; i < _col; i++)
		_m[row2-1][i] = _m[row1-1][i];	

	return *this;
}		

//matrix copy col1 to col2
Matrix& Matrix::copycol(int16 col1, int16 col2)
{
int32	i;	
	
	if( col1 <= 0 || col2 <= 0 || _col+1 < col1 || _col+1 < col2)
		return *this;
		
	for(i = 0; i < _row; i++)
		_m[i][col2-1] = _m[i][col1-1];
	
	return *this;
}

//matrix row data times double
void Matrix::rowtimes(int16 row, double val)
{	
	if( row <= 0 || _row+1 < row)
		return;
	
	for(int32 i = 0; i < _col; i++)
		_m[row-1][i] *= val;	
}		

//matrix col data times double
void Matrix::coltimes(int16 col, double val)
{
	int32	i;	
	
	if( col <= 0 || _col+1 < col)
		return;
		
	for(i = 0; i < _row; i++)
		_m[i][col-1] *= val;	
}

//change matrix val from double to int
Matrix& Matrix::double2int(void)
{
	int32	i,j;
	int32	temp;
	
	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			temp = (int32)_m[i][j];
			_m[i][j] = temp;
		}
	}	
	
	return *this;
}
	
//double matrix operation
Matrix Matrix::operator+(Matrix& inmatrix)
{
	int32	i,j;
	Matrix newmatrix(*this);
	
	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			newmatrix._m[i][j] += inmatrix._m[i][j];
		}
	}

	return newmatrix;
}

	
Matrix Matrix::operator-(Matrix& inmatrix)
{
	int32	i,j;
	Matrix newmatrix(*this);
	
	for(i = 0; i < _row; i++){
		for(j = 0; j < _col; j++){
			newmatrix._m[i][j] -= inmatrix._m[i][j];
		}
	}

	return newmatrix;
}


Matrix Matrix::operator*(Matrix& inmatrix)
{
	int32	i,j,k;
	int32 inrow = inmatrix._row;
	int32 incol = inmatrix._col;
	double	temp;	

	Matrix newmatrix(_row, incol);
	
	for(i = 0; i < _row; i++){
		for(j = 0; j < incol; j++){
			newmatrix._m[i][j] = 0.0;
			for(k = 0; k < _col; k++){
				newmatrix._m[i][j] += _m[i][k]*inmatrix._m[k][j];
			}	
		}
	}

	return newmatrix;
/*	
	Matrix *newmatrix;

	newmatrix = new Matrix(_row, incol);
	for(int32 i = 0; i < _row; i++){
		for(int32 j = 0; j < incol; j++){
			newmatrix->_m[i][j] = 0.0;
			for(int32 k = 0; k < _col; k++){
				temp = (double)(_m[i][k])*(double)(inmatrix._m[k][j]);
				newmatrix->_m[i][j] += temp;
			}	
		}
	}

	return *newmatrix;
*/
		
}


//divide, not implemented yet
Matrix Matrix::operator/(Matrix& inmatrix)
{
	Matrix newmatrix(_row);
	
	return newmatrix;
}


//matrix determinent
//the original matrix is saved in stead of destoryed, so the copy of 
//matrix is used	
double Matrix::det(void)
{
	McoStatus status;
	int32	i, j, k;
	double	**max, **temp;
	double 	returnval;

	//if not a square matrix, return singular error
	if( _row != _col){
		_set_status(MCO_SINGULAR);
		return 0;
	}
	
	max = (double**)McoMalloc(sizeof( double*)*_row);
	if(!max){
		_set_status(MCO_MEM_ALLOC_ERROR);
		return 0;
	}
	for( j = 0; j < _row; j++){
		max[j] = (double*)McoMalloc(sizeof(double)*_col);
		if(!max[j]){
			_set_status(MCO_MEM_ALLOC_ERROR);
			for(k = j-1; k >= 0; k--)
				McoFree((void *) max[k]);	
			McoFree((void *) max);
			return 0.0;
		}
	}
		
	//copy the original matrix
	for( i = 0; i < _row; i++)
		for( j = 0; j < _col; j++)
			max[i][j] = _m[i][j];

	temp = _m;
	_m = max;
	status = _ludcmp();
	if(status != MCO_SUCCESS){ //singular matrix, everything assigned to 0
		returnval = 0.0;
	}	
	else {
		returnval = _m[0][0];
		for( i = 1; i < _row; i++)
			returnval *= _m[i][i];
	}				
	
	for( i = 0; i < _row; i++)				
		McoFree((void *) _m[i]);
	McoFree((void *) _m);	
		
	_m = temp;

	return returnval;
}	


//matrix inverse ( only matrix with _row == _col can be inversed	
//if the matrix is singular, 0 matrix will be given
//mathod to use in matrix operation is singular value decomposition
Matrix& Matrix::inv(void)
{
	McoStatus status;
	int32	i, j, k;
	double	**max;
	
	//if not a square matrix, return singular error
	if( _row != _col){
		_set_status(MCO_SINGULAR);
		return *this;
	}

	status = _ludcmp();
	if(status != MCO_SUCCESS){ //singular matrix, everything assigned to 0
		for(i = 0; i < _row; i++)
			for(j = 0; j < _col; j++)
				_m[i][j] = 0.0;
		_set_status(MCO_SINGULAR);
	}
				
	else{
		max = (double**)McoMalloc(sizeof(double*)*_row);
		if(!max){
			_set_status(MCO_MEM_ALLOC_ERROR);
			return *this;
		}
		for( j = 0; j < _row; j++){
			max[j] = (double*)McoMalloc(sizeof(double)*_col);
			if(!max[j]){
				_set_status(MCO_MEM_ALLOC_ERROR);
				for(k = j-1; k >= 0; k--)
					McoFree((void *) max[k]);	
				McoFree((void *) max);
				return *this;
			}
		}
		
		//set to a I matrix and solve the inverse
		for( i = 0; i < _row; i++){
			for( j = 0; j < _col; j++)
				max[i][j] = 0.0;
			max[i][i] = 1.0;	
			
			_lubksb(max[i]);
		}
		
		//fix on 8/12
		//note!!, because the max contains the transverse of
		//of the inverse, do transverse and get it back
		double tempval;
		for( i = 0; i < _row; i++){
			for( j = i; j < _col; j++){
				tempval = max[i][j];
				max[i][j] = max[j][i];
				max[j][i] = tempval;
			}
		}
		//end of fix
		
		for( i = 0; i < _row; i++)				
			McoFree((void *) _m[i]);
		
		McoFree((void *) _m);	
		
		_m = max;		
	}				
	
	return *this;
}	

//utility operation
	
int32 Matrix::getrow(void)
{
	return _row;
}


int32 Matrix::getcol(void)	
{
	return _col;
}

void Matrix::setval(int32 row, int32 col, double val)
{
	if(row < 1 || row > _row || col < 1 || col > _col)
		return;
		
	_m[row-1][col-1] = val;
}
	
McoStatus Matrix::getval(int32 row, int32 col, double* val)
{
	McoStatus status;
	
	if(row < 1 || row > _row || col < 1 || col > _col)
		return MCO_FAILURE;
		
	*val = _m[row-1][col-1];
	return MCO_SUCCESS;
}
		
McoStatus Matrix::load(const char * fname)
{
//takeout
	return MCO_FAILURE;
/*
	if(_err != MCO_SUCCESS)
		return _err;
		
	ifstream fs(fname, 1, 0644);
	
	for(int32 i = 0; i < _row; i++){
		for(int32 j = 0; j < _col; j++){
			fs >> _m[i][j];
		}
	}
	
	fs.close();		
	return _err;
*/
}


McoStatus Matrix::save(const char * fname)
{
//take out
	return MCO_FAILURE;
/*
	if(_err != MCO_SUCCESS)
		return _err;
		
	ofstream fs;
	fs.open(fname, 2, 0644);
	
	for(int32 i = 0; i < _row; i++){
		for(int32 j = 0; j < _col; j++){
			fs << _m[i][j] << '\t';
		}
		fs << '\r';
	}
	
	fs.close();		
		
	return _err;
*/
}

/*
McoStatus Matrix::loadbi(FileFormat * ff)
{
	int32	rows, cols;
	int32	i;
	if(_err != MCO_SUCCESS)
		return _err;
	
	ff->relRead(4, (char*)&rows);
	ff->relRead(4, (char*)&cols);
	
	if(rows < 1 || cols < 1)
		return MCO_FAILURE;

	if(_row != rows || _col != cols){
		//delete original matrix values
		_deallocate();

		//allocate a new one
		_row = rows;
		_col = cols;
		_allocate();
	}

	if(_err == MCO_SUCCESS)
		for( i = 0; i < _row; i++)
			ff->relRead(_col*sizeof(double), (char*)_m[i]);
	
	return _err;
}


McoStatus Matrix::savebi(FileFormat * ff)
{
	int32	i, j;
	
	if(_err != MCO_SUCCESS)
		return _err;

	ff->relWrite(4, (char*)&_row);
	ff->relWrite(4, (char*)&_col);
			
	for(i = 0; i < _row; i++)
		ff->relWrite(_col*sizeof(double), (char*)_m[i]);

	return _err;
}
*/

McoStatus Matrix::loadstruct(double *st)
{
	int32	i;
	if(_err != MCO_SUCCESS)
		return _err;
	
	for(i = 0; i < _row; i++)
		memcpy((char*)_m[i], (char*)(st + _col*i), _col*sizeof(double));
	
	return _err;
}


//it is the user's reponsibility to check st not be dirty
McoStatus Matrix::savestruct(double* st)
{
	int32	i, j;
	
		
	if(_err != MCO_SUCCESS)
		return _err;
			
	for(i = 0; i < _row; i++)
		memcpy((char*)(st + _col*i), (char*)_m[i], _col*sizeof(double));

	return _err;
}


McoStatus Matrix::_ludcmp(void)
{
	int32 i, j, k;
	int32 imax; // row number with largest pivot
	double aamax, sum, dum;
	double *vv; //for save the scaling
	
	vv = (double*)McoMalloc(sizeof(double)*_row);
	if(!vv)
		return MCO_MEM_ALLOC_ERROR;
	
	for(i = 0; i < _row; i++)
		_indx[i] = i;
	
	_d = 1;
	
	for(i = 0; i < _row; i++){
		aamax = 0.0;
		for( j = 0; j < _col; j++)
			if(fabs(_m[i][j]) > aamax)
				aamax = fabs(_m[i][j]);
		if( aamax == 0.0){
			McoFree((void *) vv);
			return MCO_SINGULAR;
		}

		vv[i] = 1.0/aamax;
	}
	
	//loop over columns
	
	for( j = 0; j < _col; j++){
	
		for( i = 0; i < j; i++) {	
			sum = _m[i][j];
			for( k = 0; k < i; k++)
				sum -= _m[i][k]*_m[k][j];
			_m[i][j] = sum;	
		}			
		
		//search for largest pivot element
		aamax = 0;
		imax = j;
		
		for( i = j; i < _col; i++){
			sum = _m[i][j];
			for( k = 0; k < j; k++)
				sum -= _m[i][k]*_m[k][j];
			_m[i][j] = sum;
		
			dum = vv[i]*fabs(sum); //for testing pivot
			if( dum > aamax){
				imax = i;
				aamax = dum;
			}
		}
		
		if( j != imax) { //need interchange rows? j = i 
			for( k = 0; k < _col; k++){
			
				//interchange rows and change parity 
				dum = _m[imax][k];
				_m[imax][k] = _m[j][k];
				_m[j][k] = dum;
				_d = -_d;
				
				//interchange scale factor
				dum = vv[imax];
				vv[imax] = vv[j];
			}
			
			//save the interchange information
			_indx[j] = imax;
		}
		
		//divide by the pivot element
		if( j != _col){
			if( _m[j][j] == 0.0){ //pivot value is 0, singular matrix
				McoFree((void *) vv);
				return MCO_SINGULAR;//at least to the precision of the algorithm
			}
					 		
			dum = 1.0/ _m[j][j];
			for(i = j+1; i < _row; i++)
				_m[i][j] *= dum;		
		}
	}
	
	McoFree((void *) vv);

	if( _m[_row-1][_col-1] == 0.0)
		return MCO_SINGULAR;
		
	return MCO_SUCCESS;
}						


//forward and backward substitution for lu solution
void Matrix::_lubksb(double * vec)
{
	int32 i, j, k;
	int32	ii;	// index of the first nonvanishing element of solved vec
	int32	ll;
	double	sum;
	
	//forward substitution
	//note the diagonal elements of Lower matrix is always 0
	
	for(i = 0; i < _row; i++){
		
		ll = (long)_indx[i];
		sum = vec[ll];
		vec[ll] = vec[i];
		for( j = 0; j < i; j++)
			sum -= _m[i][j]*vec[j];
			
		vec[i] = sum;	
	}
	
	//backward substitution
	for(i = _row -1; i >= 0; i--){
		
		sum = vec[i];
		for( j = i+1; j < _row; j++)
			sum -= _m[i][j]*vec[j];
			
		vec[i] = sum/_m[i][i];	 	
	}
}		


