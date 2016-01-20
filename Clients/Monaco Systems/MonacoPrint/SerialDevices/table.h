////////////////////////////////////////////////////////////////////////////////
//table comminications														  //
// table.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_TABLE_COM
#define IN_TABLE_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "Thermometer.h"
#include "techkon.h"
#include "inputdevice.h"


#define PASS_LOC "0100"

#define TK_DEFALUT_BAUD (b4800)
#define TK_DEFAULT_XON	(0)
#define TK_DEFAULT_DTS	(1)
#define TK_DEFAULT_PARITY (0)
#define TK_DEFAULT_STOP (1)
#define TK_DEFAULT_DATA (3)
#define TK_DEFAULT_BUFF (10000)


class Table {
private:
protected:
	
	double *pointArray;
	int32	current_x,current_y;
	double	a,b,c,d,Tx,Ty;
	int32	min_x,min_y;
	int32	max_x,max_y;
	
	int		paperNum;		// the patch number of paper patch
	
public:

Table(void);
~Table(void);

// get the position of a patch
void getPatchPoint(int32 patchnum,double *point);

// translate a patch location to 
void translateToTable(double *patch,int32 *table);

// get the current location of the table
virtual McoStatus getLocation(int32 *position,int ref);

double _getMaxDiff(double x,double y);

// get the rotation, scale, and displacement from a set of points
McoStatus setPoints_1(int32 *tablePoints,double *patchPoints);

// get the rotation, scale, and displacement from a set of points
McoStatus setPoints_2(int32 *tablePoints,double *patchPoints);


// get the rotation, scale, and displacement from a set of points
McoStatus setPoints_3(int32 *tablePoints,double *patchPoints);


McoStatus setUpPatches(int32 numCol,int32 numRow,double w,double h,int first, int direction,int pn);

virtual McoStatus Calibrate(void) {return MCO_SUCCESS;}

// set the gretag to output lab values
virtual McoStatus SetLabOutput(void) {return MCO_SUCCESS;}

// set the gretag to output density values
virtual McoStatus SetDensityOutput(void) {return MCO_SUCCESS;}

virtual McoStatus turnOnline(void) {return MCO_SUCCESS;}

virtual McoStatus turnOffline(void) {return MCO_SUCCESS;}

virtual McoStatus moveDistance(double dx,double dy) {return MCO_OBJECT_NOT_INITIALIZED;}

virtual McoStatus moveToPatch(int32 patchNum,int ref) {return MCO_OBJECT_NOT_INITIALIZED;}

virtual McoStatus getNextPatch(int32 patch,double *lab,int device_id) {return MCO_OBJECT_NOT_INITIALIZED;}
};

#endif