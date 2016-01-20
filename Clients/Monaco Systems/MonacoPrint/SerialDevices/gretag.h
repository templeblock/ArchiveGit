////////////////////////////////////////////////////////////////////////////////
// gretag comminications														  //
// gretag.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef IN_GRETAG_COM
#define IN_GRETAG_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "inputdevice.h"

#define XRITE_MESS 17009

#define GT_DEFALUT_BAUD (b9600)
#define GT_DEFAULT_XON	(0)
#define GT_DEFAULT_DTS	(0)
#define GT_DEFAULT_PARITY (0)
#define GT_DEFAULT_STOP (0)
#define GT_DEFAULT_DATA (3)
#define GT_DEFAULT_BUFF (10000)


class GretagCom : public InputDevice{
private:
protected:

	serialPort *sp;
	int		last_error;
	int		error_code;

	
public:
	
GretagCom(int port_num,DeviceTypes devicenum);
~GretagCom(void);

McoStatus CheckError(void);

McoStatus setBaud(int baud);
virtual McoStatus Initialize(void);
McoStatus Calibrate(void);
Boolean IsBufferEmpty(void);
Boolean IsDataPresent(void);
McoStatus getNextPatch(double *lab);

McoStatus GetPaperDensity(void);

// set the gretag to output lab values
McoStatus SetLabOutput(void);

// set the gretag to output density values
McoStatus SetDensityOutput(void);
};

#endif