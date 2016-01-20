////////////////////////////////////////////////////////////////////////////////
// techkon communications													  //
// techkon.cpp																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#include "techkon.h"
#include "colortran.h"
#include "monacoprint.h"
#include "think_bugs.h"
#include "fullname.h"
#include "errordial.h"
#include <string.h>
#include <stdio.h>

Techkon::Techkon(int port_num,DeviceTypes devicenum):InputDevice(devicenum)
{
FileFormat file;
FSSpec	fspec;
Str255	inname;
  
Str255 	dirname = MONACO_FOLDER;
Str255 	dirname2 = MONACO_FOLDER;
FILE	*fs;
McoStatus status;

tweak = 0L;

sp = new serialPort((SerialPortNum)port_num,TK_DEFAULT_BUFF, TK_DEFAULT_XON,TK_DEFAULT_DTS,
				    TK_DEFALUT_BAUD,TK_DEFAULT_DATA,TK_DEFAULT_PARITY,
				    TK_DEFAULT_STOP);
if (sp == 0L) goto bail;
if (sp->OsError != noErr) 
	{
	delete sp;
	sp = NULL;
	goto bail;
	}
	
/*
	inname[0] = 0;
	pstrcat(inname,filename1);

	ptocstr(filename1);
	ptocstr(dirname);
	status = file.findFilefromPref(&fspec, (char*)filename1,(char*)dirname);

	if (status == MCO_SUCCESS)
		{
		PathNameFromDirID(fspec.parID, fspec.vRefNum, inname);
		ptocstr(inname);	
			
		fs = fopen( (char*)inname, "r" );
		if(fs) 
			{
			tweak1 = new Tweak_Patch();
			tweak1->Load_Data(fs);
			}
		}
*/

state = MCO_SUCCESS;
return;	
bail:
state = MCO_SERIAL_ERROR;

}

Techkon::~Techkon(void)
{
if (sp) delete sp;
tweak = 0L;
sp = 0L;
}




// set the baud rate of the xrite to the specified baud rate
// first have to find the rate that it is currently set to
McoStatus Techkon::setBaud(int baud)
{
if (sp == 0L) return MCO_SERIAL_ERROR;

sp->setBaud(baud);

sp->clearInputBuf();

return MCO_SUCCESS;
}



McoStatus Techkon::Initialize(void)
{
McoStatus state;
Str255	the_string;

if (sp == 0L) return MCO_SERIAL_ERROR;

state = setBaud(TK_DEFALUT_BAUD);

sendCommand(Tech_Illum_D50);
sendCommand(Tech_Obs_2);


return MCO_SUCCESS;
}

Boolean Techkon::IsBufferEmpty(void)
{
if (sp == 0L) return TRUE;
if (sp->numInChars() < 18) return TRUE;
return FALSE;
}

Boolean Techkon::IsDataPresent(void)
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


McoStatus Techkon::sendCommand(TechkonCom com)
{
unsigned char st[500];
int tc;

if (sp == 0L) return MCO_SERIAL_ERROR;

st[0] = 0xAA;
st[1] = 0x55;
switch (com) {
	case Tech_Measure:	
		st[2] = 0x01;
		break;
	case Tech_Calibrate:	
		st[2] = 0x02;
		break;		
	case Tech_Illum_A:
		st[2] = 0x10;
		break;
	case Tech_Illum_C:
		st[2] = 0x11;
		break;
	case Tech_Illum_D50:
		st[2] = 0x12;
		break;
	case Tech_Illum_D65:
		st[2] = 0x13;
		break;
	case Tech_Obs_2:
		st[2] = 0x14;
		break;
	case Tech_Obs_10:
		st[2] = 0x15;
		break;
	case Tech_COM_Bin:
		st[2] = 0x30;
		break;		
	case Tech_COM_ASCII:
		st[2] = 0x31;
		break;		
	case Tech_COM_XYZ:
		st[2] = 0x32;
		break;		
	case Tech_Trans_XYZ:
		st[2] = 0x50;
		break;
	case Tech_Trans_Rem:
		st[2] = 0x58;
		break;		
	}
st[3] = 0xAA;

sp->sendData(st,4);

return MCO_SUCCESS;
}


McoStatus Techkon::sendReadCommand(void)
{
int32 tc;

if (sp == 0L) return MCO_SERIAL_ERROR;

tc = TickCount();

sendCommand(Tech_Measure);
do {} while (IsBufferEmpty() && (TickCount() - tc < 120));
return MCO_SUCCESS;
}


McoStatus Techkon::translatetoLab(double *xyz,double *lab)
{
// convert to lab for 5000K

// normalize first
xyz[0] = 0.3585*xyz[0]/(100*0.3457);
xyz[1] = xyz[1]/(100);
xyz[2] = 0.3585*xyz[2]/(100*0.2958);

// convert to lab
nxyztoLab(xyz,lab);

if (lab[0] < 0) lab[0] = 0;
if (lab[0] > 100) lab[0] = 100;

if (lab[1] < -128) lab[1] = -128;
if (lab[1] > 128) lab[1] = 128;

if (lab[2] < -128) lab[2] = -128;
if (lab[2] > 128) lab[2] = 128;

// now apply the tweak if it exist
if (tweak != NULL) tweak->eval(lab,lab,1);
return MCO_SUCCESS;

}


McoStatus Techkon::Calibrate(void)
{
char st[5000];
char c;
int	 code,error;

McoMessAlert(MCO_CALIB,0L);
}

McoStatus Techkon::getNextPatch(double *lab)
{
char j1[255],j2[255],j3[255];
unsigned char st[5000];
char *p;
int32	tc;
double xyz[3];
int32 X,Y,Z;

if (sp == 0L) return MCO_SERIAL_ERROR;

// get the 18 byte packet of info
sp->getData(st,18,tc);
// check length
if (tc != 18) 
	{
	sp->clearInputBuf();
	return MCO_FAILURE;
	}

// do some checking
if (st[0] != 0xAA)
	{
	sp->clearInputBuf();
	return MCO_FAILURE;
	}


//get the xyz values
X = ((char)st[10]<<8)+st[11];
Y = ((char)st[12]<<8)+st[13];
Z = ((char)st[14]<<8)+st[15];


xyz[0] = (double)X/100.0;
xyz[1] = (double)Y/100.0;
xyz[2] = (double)Z/100.0;

// convert to lab for 5000K

translatetoLab(xyz,lab);


return MCO_SUCCESS;
}


McoStatus Techkon::GetPaperDensity(void)
{
long tick = TickCount();

while (TickCount() - tick < 240)
	{
	if ((!IsBufferEmpty()) && (IsDataPresent())) return getNextPatch(paperDensity);
	}
return MCO_FAILURE;
}