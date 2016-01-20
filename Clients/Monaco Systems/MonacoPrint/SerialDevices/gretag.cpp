////////////////////////////////////////////////////////////////////////////////
// Xrite comminications														  //
// xritecom.c																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#include "gretag.h"
#include "colortran.h"
#include "monacoprint.h"
#include "think_bugs.h"
#include "fullname.h"
#include "assort.h"
#include "errordial.h"
#include <string.h>
#include <stdio.h>

GretagCom::GretagCom(int port_num,DeviceTypes devicenum):InputDevice(devicenum)
{
McoStatus status;

thermd = NULL;
// open the serial port
sp = new serialPort((SerialPortNum)port_num,GT_DEFAULT_BUFF, GT_DEFAULT_XON,GT_DEFAULT_DTS,
				    GT_DEFALUT_BAUD,GT_DEFAULT_DATA,GT_DEFAULT_PARITY,
				    GT_DEFAULT_STOP);
if (sp == NULL) goto bail;
if (sp->OsError != noErr) 
	{
	delete sp;
	sp = NULL;
	goto bail;
	}
	
// set paramaters for the serial port	
sp->setlineend('\n');  // the lineend character
sp->setmaxwait(100);

last_error = 0;

state = MCO_SUCCESS;
return;	
bail:
state = MCO_SERIAL_ERROR;

}

GretagCom::~GretagCom(void)
{
if (sp) delete sp;
sp = 0L;
}




// set the baud rate of the xrite to the specified baud rate
// first have to find the rate that it is currently set to
McoStatus GretagCom::setBaud(int baud)
{
char st[5000];
SerialSpeeds newspeed;

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->setBaud(baud);

sp->clearInputBuf();

sp->sendData("; 0\r\n",5);
sp->getNextLine(st,5000,thermd);
newspeed = b57600;
while ((strncmp(": 11",st,4)) && (newspeed >= b300))
	{
	sp->setBaud(newspeed);
	sp->clearInputBuf();
	sp->sendData("; 0\r\n",5);
	sp->getNextLine(st,5000,thermd);
	newspeed = (SerialSpeeds)((int)newspeed - 1);
	}
	
if (newspeed < b300) return MCO_DEVICE_NOT_FOUND;

return MCO_SUCCESS;
}



McoStatus GretagCom::Initialize(void)
{
McoStatus state = MCO_SUCCESS;
char st[5000];
int		dum,type,error;

if (sp == 0L) return MCO_SERIAL_ERROR;


// set baud
state = setBaud(GT_DEFALUT_BAUD);
if (state != MCO_SUCCESS) goto bail;

// check device type
sp->sendData("; 43\r\n",6);
sp->getNextLine(st,5000,thermd);

sscanf_n_i(st,27,1,&type);
if (type !=2) goto bail;

//reset
sp->sendData("; 90 1 4 5\r\n",12);
state = CheckError();
if (state != MCO_SUCCESS) goto bail;

//set measurment type
sp->sendData("; 77 155\r\n",10);
state = CheckError();
if (state != MCO_SUCCESS) goto bail;

// set the illuminante, angle
sp->sendData("; 22 1 1 3 0\r\n",14);
state = CheckError();
if (state != MCO_SUCCESS) goto bail;

// set the type of output desired (Cie Lab)
sp->sendData("; 177 2 2\r\n",11);
state = CheckError();
if (state != MCO_SUCCESS) goto bail;
mode = ReadLabMode;


bail:
if (thermd != NULL)
	{
	thermd->DisplayTherm(4,4,NULL);	
	thermd->RemoveTherm();
	delete thermd; 
	}
thermd = NULL;


return state;
}


// set the gretag to output lab values
McoStatus GretagCom::SetLabOutput(void)
{
// set the type of output desired (Cie Lab)
if (mode == ReadLabMode) return MCO_SUCCESS;
sp->sendData("; 177 2 2\r\n",11);
state = CheckError();
mode = ReadLabMode;
return state;
}

// set the gretag to output density values
McoStatus GretagCom::SetDensityOutput(void)
{
// set the type of output desired (Cie Lab)
if (mode == ReadDensityMode) return MCO_SUCCESS;
sp->sendData("; 177 4 15\r\n",12);
state = CheckError();
mode = ReadDensityMode;
return state;
}


// see if an error has occured
McoStatus GretagCom::CheckError(void)
{
char *c;
int code_error[2];
char	st[5000];

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->getNextLine(st,5000,thermd);
if ((st[0] != ';') && (st[0] != ':'))  // is start character correct?
	{
	last_error = -1;
	return MCO_SERIAL_DEVICE_ERROR;
	}
c = sscanf_n_i(st,1,2,code_error);	
if (c == 0L)		// has a code been returned?
	{
	last_error = -2;
	return MCO_SERIAL_DEVICE_ERROR;
	}
if ((code_error[0] != 31) && (code_error[0] != 37)) // is code the right code?
	{
	last_error = -3;
	error_code = code_error[0];
	return MCO_SERIAL_DEVICE_ERROR;
	}
if ((code_error[1] != 0) && (code_error[1] != 19)) 
	{
	last_error = code_error[1];
	return MCO_SERIAL_DEVICE_ERROR;
	}
return MCO_SUCCESS;
}

// send a calibrate command

McoStatus GretagCom::Calibrate(void)
{
char st[5000];
char c;
int	 code,error;

McoMessAlert(MCO_CALIB_2,0L);

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->sendData("; 34 9 7\r\n",10);

return CheckError();
}

// see if any characters are in the buffer
Boolean GretagCom::IsBufferEmpty(void)
{
if (sp == 0L) return TRUE;
if (sp->numInChars() < 1) return TRUE;
return FALSE;
}


Boolean GretagCom::IsDataPresent(void)
{

int32	last_size;
int32	current_size;

// Just wait around until buffer gets large enough 
// and stabilizes
if (sp == 0L) return FALSE;


do {
	last_size = sp->numInChars();

	waitTenthSec();
	current_size = sp->numInChars();
	} while (last_size != current_size);

if (current_size < 1) return FALSE;
return TRUE;
}

// get the lab value for the next patch

McoStatus GretagCom::getNextPatch(double *lab)
{
char j1[255],j2[255],j3[255];
char st[5000];
char *p;
int32	tc;
double xyz[3];
int	   code;

if (sp == 0L) return MCO_SERIAL_ERROR;

// get the packet of info
sp->getNextLine(st,5000,thermd);

// check for an error
sscanf_n_i(st,1,1,&code);
if (code != 190) return MCO_FAILURE;

if (mode == ReadLabMode)
	{
	// read in the data
	sscanf_n(st,2,3,lab);

	// check for an error again
	sscanf_n_i(st,1,1,&code);
	if (code != 0) return MCO_FAILURE;

	// now apply the tweak if it exist
	if (tweak != NULL) tweak->eval(lab,lab,1);
	}
else if (mode == ReadDensityMode)
	{
	// read in the black
	sscanf_n(st,7,1,&lab[3]);
	
	// read in the cyan
	sscanf_n(st,2,1,&lab[0]);
	
	// read in the magenta
	sscanf_n(st,2,1,&lab[1]);
	
	// read in the yellow
	sscanf_n(st,2,1,&lab[2]);
	
	// check for an error again
	sscanf_n_i(st,1,1,&code);
	if (code != 0) return MCO_FAILURE;
	
	if (lab != paperDensity) 
		{
		lab[0] -= paperDensity[0];
		lab[1] -= paperDensity[1];
		lab[2] -= paperDensity[2];
		lab[3] -= paperDensity[3];
		}
	}

return MCO_SUCCESS;
}


McoStatus GretagCom::GetPaperDensity(void)
{
long tick = TickCount();

while (TickCount() - tick < 240)
	{
	if ((!IsBufferEmpty()) && (IsDataPresent())) return getNextPatch(paperDensity);
	}
return MCO_FAILURE;
}