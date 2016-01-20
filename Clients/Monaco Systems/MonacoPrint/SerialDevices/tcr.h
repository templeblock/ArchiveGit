////////////////////////////////////////////////////////////////////////////////
// Techkon comminications														  //
// techkon.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef TCR_COM
#define TCR_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "Thermometer.h"
#include "techkon.h"
#include "table.h"


#define PASS_LOC "0100"

#define TK_DEFALUT_BAUD (b4800)
#define TK_DEFAULT_XON	(0)
#define TK_DEFAULT_DTS	(1)
#define TK_DEFAULT_PARITY (0)
#define TK_DEFAULT_STOP (1)
#define TK_DEFAULT_DATA (3)
#define TK_DEFAULT_BUFF (10000)


class TCR:public Techkon , public Table{
private:
protected:
	
	
public:

int		doPenDown;

TCR(int port_num,DeviceTypes device_type);
~TCR(void);

McoStatus Initialize(void);

// get the current location of the table
McoStatus getLocation(int32 *position,int refrence);


McoStatus moveDistance(double dx,double dy);

McoStatus moveToPatch(int32 patchNum,int ref);

McoStatus getNextPatch(long patch,double *lab);
};

#endif