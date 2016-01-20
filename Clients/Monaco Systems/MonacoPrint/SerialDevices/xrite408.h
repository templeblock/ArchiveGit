////////////////////////////////////////////////////////////////////////////////
// Techkon comminications														  //
// techkon.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef XRITE408_COM
#define XRITE408_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "Thermometer.h"
#include "inputdevice.h"

#define PASS_LOC "0100"

#define XR408_DEFALUT_BAUD (b9600)
#define XR408_DEFAULT_XON	(0)
#define XR408_DEFAULT_DTS	(1)
#define XR408_DEFAULT_PARITY (0)
#define XR408_DEFAULT_STOP (1)
#define XR408_DEFAULT_DATA (3)
#define XR408_DEFAULT_BUFF (10000)




class XRite408: public InputDevice {
private:
protected:

	serialPort *sp;
	
public:
		

XRite408(int port_num);
~XRite408(void);

McoStatus Initialize(void);
Boolean IsBufferEmpty(void);
Boolean IsDataPresent(void);
McoStatus Calibrate(void);
McoStatus _parseText(char *c,double *density);
McoStatus getNextPatch(double *density);
McoStatus GetPaperDensity(void);
};

#endif