////////////////////////////////////////////////////////////////////////////////
// Xrite comminications														  //
// xritecom.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef XRITE_COM
#define XRITE_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "inputdevice.h"

#define XRITE_MESS 17009
					   
//#define XR_STRIPDEF_8_11  "MONACOP1000F1000000ST1481730000ST2481725000ST3481720000ST4481715000ST5481710000"
#define XR_STRIPDEF_8_11  "MONACOP1000F1000000ST1481730000"
//#define XR_STRIPDEF_11_17 "MONACOP1000F1000000ST1482830000ST2482825000ST3482820000ST4481715000ST5482810000"
#define XR_STRIPDEF_11_17 "MONACOP1000F1000000ST1482830000"

#define PASS_LOC "0100"

#define XR_DEFALUT_BAUD (b19200)
#define XR_DEFAULT_XON	(0)
#define XR_DEFAULT_DTS	(1)
#define XR_DEFAULT_PARITY (0)
#define XR_DEFAULT_STOP (0)
#define XR_DEFAULT_DATA (3)
#define XR_DEFAULT_BUFF (10000)


class XriteCom : public InputDevice{
private:
protected:

	serialPort *sp;

	int page_type;
public:

XriteCom(int port_num,DeviceTypes devicenum,int p_type);
~XriteCom(void);

Boolean IsXriteSV(char *st);
McoStatus setBaud(int baud);
McoStatus setXriteStripDef(char *stripDef,char *passes_loc);
McoStatus Initialize(void);
Boolean IsBufferEmpty(void);
Boolean IsDataPresent(void);
McoStatus TurnOnStripRead(void);
McoStatus getNextPatch(double *lab);
};

#endif