//////////////////////////////////////////////////////////////////////////////////////////
//   buildtableindex.h																	//
//																						//
//	build a set of tables that index into the large inverse table						//
//																						//
//	 Sept 17, 1996																		//
//   By James Vogh																		//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef IN_BUILDTABLEINDEX
#define IN_BUILDTABLEINDEX

#include "mcostat.h"
#include "Thermobject.h"

class BuildTableIndex {
private:
protected:
unsigned char 	*FullTable;
int				FullDim;
unsigned char	*ManyKTable;
int				ManyDim;

double			*InvTable;
int				InvDim;

// The maximum variations in L,a, & b
double			L,a,b;

ThermObject	*thermd;

public:

BuildTableIndex(int fd,int md,ThermObject	*td);
~BuildTableIndex(void);

McoStatus InitializeTables(void);

McoStatus SetInverseTable(double *it,int itd);

// find the maximum difference between adjacent lab values in the table
McoStatus FindMaxLabDiff(void);

McoStatus Build(void);

McoStatus GetIndex(double *lab,double K,int *cmy_index);

// given a lab value and K, find the CMY indexes to search over, or indicate that the color is out of gamut
// return a -1 in cmy_index[0] if out of gamut
McoStatus GetMinMaxK(double *lab,double *minK,double *maxK,short *gamut);

double getSpacing(void);

};

#endif