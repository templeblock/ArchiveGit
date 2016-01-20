#include "lookuptable.h"

#define ABS(x) ((x)>=(0.0)?(x):(-(x)))

#define VIRTUALLY_ZERO (0.000001)

// Returns the largest index in the sparse table less than 
// or equal to the parameter index. 
short findFloorIndex(const SparseTable *aSparseTable, short count, double index) {
	short i, where;
	double largest;
	
	where = 0;
	largest = 0.0;
	for (i = 0; i < count; i++) {
		if ((aSparseTable[i].index <= index)&&(aSparseTable[i].index >= largest)) {
			largest = aSparseTable[i].index;
			where = i;
		}
	}
	return(where);
}

// Returns the smallest index in the sparse table greater than 
// or equal to the parameter index. 
short findCeilIndex(const SparseTable *aSparseTable, short count, double index) {
	short i, where;
	double smallest;
	
	where = count-1;
	smallest = 1.0;
	for (i = 0; i < count; i++) {
		if ((aSparseTable[i].index >= index)&&(aSparseTable[i].index <= smallest)) {
			smallest = aSparseTable[i].index;
			where = i;
		}
	}
	return(where);
}

// The LookUpTable class member function definitions.
LookUpTable::LookUpTable() {
	size  				= 0;
	valueHdl 			= 0;
	isTableMonotonic 	= FALSE;
}

LookUpTable::LookUpTable(short size) {
	size 				= 0;
	valueHdl 			= 0;
	isTableMonotonic 	= FALSE;
	initialize(size);
}

// Allocate memory space.
Boolean LookUpTable::initialize(short size) {
	if (valueHdl!=0) return (FALSE);
	valueHdl = new double[(size*sizeof(double))];
	if (valueHdl==0) return (FALSE);
	else {
		(this->size = size);
		return(TRUE);
	}
}

// Deallocate memory space.
void LookUpTable::cleanup() 
{
	if (valueHdl!=0)
	{
		delete []valueHdl;
	}
}

LookUpTable::~LookUpTable() {
	cleanup();
}

inline short LookUpTable::getSize() {
	return (size);
}
	
inline double *LookUpTable::getValueHdl() {
	return(valueHdl);
}

// Set table value.  This allows for another funciton to construct the table.
inline Boolean LookUpTable::setValue(short index, double value) {
	if (valueHdl == 0) return (FALSE);
	if (index >= size) return (FALSE);
	valueHdl[index] = value;
	return (TRUE);
}

// Forward look-up.
inline Boolean LookUpTable::lookUp(short index, double *value) {
	if (valueHdl == 0) return (FALSE);
	if (index >= size) return (FALSE);
	(*value) = valueHdl[index];
	return (TRUE);
}


// Simple linear search.  If the table is monotonic, use hint information
// to get quicker search.  Hint specifies the initial start of the search,
// and direction specifies the search direction (+ = forward search, 
// - = backward search).  
Boolean LookUpTable::inverseLookUp(double value, short *index, 
									   short hint, short direction) {
	short i;
	double smallestDiff = 99999.0, tableValue, diff;

	if (valueHdl == 0) return (FALSE);

	// Make sure things are what they should be.
	if ((hint < 0)||(hint >= size)) {
		hint = 0;
		direction = 1;
	}

	// Do exhaustive search if table values are not monotonic.
	if (!isTableMonotonic) {
		hint = 0;
		direction = 1;
	}

	if (direction > 0) {   // forward search	
		(*index) = -1;
		for (i = hint; i<size; i++) {
			tableValue = valueHdl[i];
			diff = ABS(value - tableValue);
			if ((diff - smallestDiff) <= VIRTUALLY_ZERO) {
				(*index) = i;
				smallestDiff = diff;
			}
			else {
				if (isTableMonotonic) break;
			}
		}
		if ((*index)== hint) {  // just in case hint is past the index of value
			for (i = hint; i>=0; i--) {
				tableValue = valueHdl[i];
				diff = ABS(value - tableValue);
				if ( (diff - smallestDiff) < VIRTUALLY_ZERO) {
					(*index) = i;
					smallestDiff = diff;
				}
				else {
					if (isTableMonotonic) break;
				}
			}
		}
		if ((*index) == -1) return (FALSE);
		else return (TRUE);
	}
	else {  // Backward search
		(*index) = -1;
		for (i = hint; i>=0; i--) {
			tableValue = valueHdl[i];
			diff = ABS(value - tableValue);
			if ((diff - smallestDiff) <= VIRTUALLY_ZERO) {
				(*index) = i;
				smallestDiff = diff;
			}
			else {
				if (isTableMonotonic) break;
			}
		}
		if ((*index)== hint) {  // just in case hint is past the index of value
			for (i = hint; i<size; i++) {
				tableValue = valueHdl[i];
				diff = ABS(value - tableValue);
				if ((diff - smallestDiff) < VIRTUALLY_ZERO) {
					(*index) = i;
					smallestDiff = diff;
				}
				else {
					if (isTableMonotonic) break;
				}
			}
		}
		if ((*index) == -1) return (FALSE);
		else return (TRUE);
	}
}

// Construct the look-up table from a sparse table.  The Sparse
// table is an array consisting of a normalized index and the 
// value for that index.  The look-up table is constructed by 
// interpolating between values in the sparse table.
Boolean LookUpTable::constructTable(const SparseTable *theSparseTable, short count) {
	double normIndex, offset, distance;
	short theFloor, theCeil, i,j;

	// check for monotonicity of the sparse table, assuming
	// the indices and values are ordered the same way.
	isTableMonotonic = TRUE;
	for (i = 0; i < count; i++) {
		for (j = i+1; j<count; j++) { 
			if ((theSparseTable[j].index > theSparseTable[i].index)&&
				(theSparseTable[j].value < theSparseTable[i].value))
				isTableMonotonic = FALSE;
			if ((theSparseTable[j].index < theSparseTable[i].index)&&
				(theSparseTable[j].value > theSparseTable[i].value))
				isTableMonotonic = FALSE;
		}
	}


	theFloor = 0;
	theCeil = 0;
	for (i = 0; i < size; i++) {

		normIndex = (double) i / ((double) (size -1));
		
		// Find upper and lower index values.
		if ((normIndex < theSparseTable[theFloor].index) ||
		     (normIndex > theSparseTable[theCeil].index)) {
			theFloor = findFloorIndex(theSparseTable,count, normIndex);
			theCeil = findCeilIndex(theSparseTable,count, normIndex);
		}
	
		if (theFloor==theCeil) {
			offset = 0.0;
			distance = 1.0;
		}
		else { 
			offset = normIndex - theSparseTable[theFloor].index; 
			distance = theSparseTable[theCeil].index - theSparseTable[theFloor].index;
		}
		
		 // Interpolate.
		valueHdl[i] = (theSparseTable[theFloor].value*(distance - offset) +
				theSparseTable[theCeil].value*(offset))/distance;
	}
	
	return(TRUE);
}
