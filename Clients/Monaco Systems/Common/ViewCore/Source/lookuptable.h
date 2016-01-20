#ifndef	LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include "mcotypes.h"

// A simple sparse table entry.
typedef struct {
	double 	index;
	double	value;
} SparseTable;

// A look-up table class.
class LookUpTable {
	short 	size;
	double 	*valueHdl;
	Boolean isTableMonotonic;

  public:
	LookUpTable();
	LookUpTable(short size);
	Boolean initialize(short size);
	void cleanup();
	~LookUpTable();
	short getSize();	
	double* getValueHdl();
	Boolean setValue(short index, double value);
	Boolean lookUp(short index, double *value);
	Boolean inverseLookUp(double value, short *index, short hint, short direction);
	Boolean constructTable(const SparseTable *theSparseTable, short count);
};


#endif //LOOKUPTABLE_H