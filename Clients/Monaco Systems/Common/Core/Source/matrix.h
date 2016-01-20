//definition of matrix class

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include "mcotypes.h"
#include "mcostat.h"

//#include "fileformat.h"

static const int32 DEFAULT_ROW = 3;
static  int32 DEFAULT_COL = 3;
static  double DEFAULT_VAL = 1.0;

class Matrix{
	//single matrix operation
	friend Matrix& operator*(Matrix&, double);
	friend Matrix& operator*(double, Matrix&);

protected:
	McoStatus _err;
	int32 _row;
	int32 _col;
	
	double **_m;
	virtual void _set_status(McoStatus);	//set current error status
	void _allocate(void);
	void _deallocate(void);

//modified on 8/12 to remove smatrix class
	int16 _d; //+-1 depending on whether the number of row interchanges was
			  //even or odd
	double* _indx;  // a vector records the row permutation effected by the 
	McoStatus _ludcmp(void); //lu decompostion
	void _lubksb(double * vec);
//end of modification

public:
	Matrix(int32 rows = DEFAULT_ROW);
	Matrix(int32 rows, int32 cols);
	Matrix(int32 rows, int32 cols, double val);
	//Matrix(int32 rows, double val);
	
	Matrix(Matrix& matrix);
	~Matrix(void);
	
	Matrix& operator=(Matrix&);

	McoStatus set(int32 rows, int32 cols);

	int32 getrow(void);	
	int32 getcol(void);
	void setval(int32 row, int32 col, double val);	//set value to entry row, col
	McoStatus getval(int32 row, int32 col, double* val);//get value from entry row, col
	
	//multiple matrix operation

	//assign part of matrix to another matrix, two matrix may have
	//different dimension
	McoStatus assignval(Matrix& inmatrix, int32 inrow, int32 incol,
						int32 row, int32 col, int32 numrow, int32 numcol);
	
	//matrix plus matrix
	virtual Matrix operator+(Matrix&);
	
	//matrix minus matrix	
	virtual Matrix operator-(Matrix&);
	
	//matrix multiple matrix	
	virtual Matrix operator*(Matrix&);

	//matrix divide, not implemented
	virtual Matrix operator/(Matrix& inmatrix);

	//copy transverse of a matrix to this matrix
	Matrix& copyT(Matrix&);

	//single matrix operation

	//matrix negative		
	virtual Matrix& operator-(void);
	
	//matrix divide
	virtual Matrix& operator/(double val);
	
	//matrix transform
	virtual Matrix& T(void);
	
	//matrix exchange row1 and row2
	Matrix& exchangerow(int16 row1, int16 row2);
	
	//matrix exchange col1 and col2
	Matrix& exchangecol(int16 col1, int16 col2);

	//matrix copy row1 to row2
	Matrix& copyrow(int16 row1, int16 row2);

	//matrix copy col1 to col2
	Matrix& copycol(int16 col1, int16 col2);

	//matrix row data times double
	void rowtimes(int16 row, double val);

	//matrix col data times double
	void coltimes(int16 col, double val);

	//change matrix val from double to int
	Matrix& double2int(void);

	//matrix inverse, abstract virtual method
	virtual Matrix& inv(void);

	virtual	double det(void);
	
	//utility operation
	virtual McoStatus load(const char *);
	virtual McoStatus save(const char *);
//	virtual McoStatus loadbi(FileFormat *);	//load binary
//	virtual McoStatus savebi(FileFormat *);	//save binary
	virtual McoStatus loadstruct(double *st);
	virtual McoStatus savestruct(double *st);
	virtual McoStatus get_status(void);	//check current error status
};

#endif //MATRIX_H	
		
	