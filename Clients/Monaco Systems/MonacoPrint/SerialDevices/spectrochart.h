////////////////////////////////////////////////////////////////////////////////
// Gretag table comminications														  //
// spectrochart.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_SPECCHART_COM
#define IN_SPECCHART_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "Thermometer.h"
#include "gretag.h"
#include "table.h"

#define PASS_LOC "0100"



class SpectroChart:public GretagCom,public Table {
private:
protected:
	int	patchCount;
	int first;
	
	int onLine;
public:

SpectroChart(int port_num,DeviceTypes devicenum);
~SpectroChart(void);

// set the gretag to output lab values
McoStatus SetLabOutput(void);

// set the gretag to output density values
McoStatus SetDensityOutput(void);

McoStatus Initialize(void);

McoStatus CheckErrorChart(void);

// get the current location of the table
McoStatus getLocation(int32 *position,int refrence);

McoStatus moveDistance(double dx,double dy);

McoStatus Calibrate(void);

int checkOnLine(void);

McoStatus turnOnline(void);

McoStatus turnOffline(void);

McoStatus moveToPatch(int32 patchNum,int ref);

McoStatus sendReadCommand(char *st);

McoStatus getNextPatch(long patch,double *lab);

McoStatus GetPaperDensity(void);
};

#endif