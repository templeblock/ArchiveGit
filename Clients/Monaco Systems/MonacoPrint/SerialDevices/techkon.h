////////////////////////////////////////////////////////////////////////////////
// Techkon comminications														  //
// techkon.h																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#ifndef TECHKON_COM
#define TECHKON_COM

#include "serialPort.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "Thermometer.h"
#include "tweak_patch.h"
#include "inputdevice.h"

#define PASS_LOC "0100"

#define TK_DEFALUT_BAUD (b4800)
#define TK_DEFAULT_XON	(0)
#define TK_DEFAULT_DTS	(1)
#define TK_DEFAULT_PARITY (0)
#define TK_DEFAULT_STOP (1)
#define TK_DEFAULT_DATA (3)
#define TK_DEFAULT_BUFF (10000)

typedef enum {
	Tech_Measure = 0,
	Tech_Calibrate,
	Tech_Illum_A,
	Tech_Illum_C,
	Tech_Illum_D50,
	Tech_Illum_D65,
	Tech_Obs_2,
	Tech_Obs_10,
	Tech_COM_Bin,
	Tech_COM_ASCII,
	Tech_COM_XYZ,
	Tech_Trans_XYZ,
	Tech_Trans_Rem
	} TechkonCom;


class Techkon: public InputDevice {
private:
protected:

	serialPort *sp;
	
public:
		

Techkon(int port_num,DeviceTypes devicenum);
~Techkon(void);

McoStatus setBaud(int baud);
McoStatus Initialize(void);
Boolean IsBufferEmpty(void);
Boolean IsDataPresent(void);
McoStatus sendCommand(TechkonCom com);
McoStatus sendReadCommand(void);
McoStatus Calibrate(void);
McoStatus translatetoLab(double *xyz,double *lab);
McoStatus getNextPatch(double *lab);
McoStatus GetPaperDensity(void);
};

#endif