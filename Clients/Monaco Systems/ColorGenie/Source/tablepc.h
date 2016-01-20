////////////////////////////////////////////////////////////////////////////////
//table comminications														  //
// table.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_TABLE_COM
#define IN_TABLE_COM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcostat.h"
#include "mcotypes.h"

typedef struct TableRect{
	int32	min_x;
	int32	max_x;
	int32	min_y;
	int32	max_y;
} TableRect;

class Table {
private:
protected:
	double *pointArray;
	int32	current_x,current_y;
	double	a,b,c,d,Tx,Ty;
	int32	min_x,min_y;
	int32	max_x,max_y;
	
public:

Table(void);
~Table(void);

// get the position of a patch
void getPatchPoint(int32 patchnum,double *point);

// translate a patch location to 
//void translateToTable(double *patch,int32 *table);
void translateToTable(int patchnum,int32 *table);

// get the current location of the table
virtual McoStatus getLocation(int32 *position,int ref);

double _getMaxDiff(double x,double y);

// get the rotation, scale, and displacement from a set of points
McoStatus setPoints_1(int32 *tablePoints,double *patchPoints);

// get the rotation, scale, and displacement from a set of points
McoStatus setPoints_2(int32 *tablePoints,double *patchPoints);


// get the rotation, scale, and displacement from a set of points
McoStatus setPoints_3(int32 *tablePoints,double *patchPoints);


McoStatus setUpPatches(int32 numCol,int32 numRow,double w,double h,int first, int direction, TableRect *tablerect);

virtual McoStatus Calibrate(void) {return MCO_SUCCESS;}

virtual McoStatus turnOnline(void) {return MCO_SUCCESS;}

virtual McoStatus turnOffline(void) {return MCO_SUCCESS;}

virtual McoStatus moveDistance(double dx,double dy) {return MCO_OBJECT_NOT_INITIALIZED;}

virtual McoStatus moveToPatch(int32 patchNum,int ref) {return MCO_OBJECT_NOT_INITIALIZED;}

virtual McoStatus getNextPatch(int32 patch,double *lab,int device_id) {return MCO_OBJECT_NOT_INITIALIZED;}
};

#endif