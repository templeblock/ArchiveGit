////////////////////////////////////////////////////////////////////////////////
// Xrite 408 comminications													  //
// xrite408.cpp 															  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////


#include "xrite408.h"
#include "errordial.h"

XRite408::XRite408(int port_num):InputDevice(DT_Xrite408)
{
sp = new serialPort((SerialPortNum)port_num,XR408_DEFAULT_BUFF, XR408_DEFAULT_XON,XR408_DEFAULT_DTS,
				    XR408_DEFALUT_BAUD,XR408_DEFAULT_DATA,XR408_DEFAULT_PARITY,
				    XR408_DEFAULT_STOP);
if (sp == 0L) goto bail;
if (sp->OsError != noErr) 
	{
	delete sp;
	sp = NULL;
	goto bail;
	}
	
state = MCO_SUCCESS;
return;	
bail:
state = MCO_SERIAL_ERROR;
}

XRite408::~XRite408(void)
{
if (sp) delete sp;
sp = 0L;
}

McoStatus XRite408::Initialize(void)
{
McoStatus state;
Str255	the_string;

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->clearInputBuf();


return MCO_SUCCESS;
}

Boolean XRite408::IsBufferEmpty(void)
{
if (sp == 0L) return TRUE;
if (sp->numInChars() < 10) return TRUE;
return FALSE;
}

Boolean XRite408::IsDataPresent(void)
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


McoStatus XRite408::Calibrate(void)
{
char st[5000];
char c;
int	 code,error;

McoMessAlert(MCO_CALIB_XRITE,0L);
}


McoStatus XRite408::_parseText(char *c,double *density)
{
int i,j,f;
char temp[5];

f = 0;
for (i=0; i<strlen(c); i++) if (c[i] == 'C')
	{
	for (j=0; j<5; j++) temp[j] = c[i+1+j];
	temp[j] = 0;
	f = 1;
	}
sscanf(temp,"%lf",&density[0]);
if (!f) return MCO_FAILURE;

f = 0;
for (i=0; i<strlen(c); i++) if (c[i] == 'M')
	{
	for (j=0; j<5; j++) temp[j] = c[i+1+j];
	temp[j] = 0;
	f = 1;
	}
sscanf(temp,"%lf",&density[1]);
if (!f) return MCO_FAILURE;

f = 0;
for (i=0; i<strlen(c); i++) if (c[i] == 'Y')
	{
	for (j=0; j<5; j++) temp[j] = c[i+1+j];
	temp[j] = 0;
	f = 1;
	}
sscanf(temp,"%lf",&density[2]);
if (!f) return MCO_FAILURE;

f = 0;
for (i=0; i<strlen(c); i++) if (c[i] == 'V')
	{
	for (j=0; j<5; j++) temp[j] = c[i+1+j];
	temp[j] = 0;
	f = 1;
	}
sscanf(temp,"%lf",&density[3]);
if (!f) return MCO_FAILURE;

return MCO_SUCCESS;
}

McoStatus XRite408::getNextPatch(double *density)
{
char j1[255],j2[255],j3[255];
char st[5000];
char *p;
int32	tc;
double xyz[3];
int32 X,Y,Z;

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->getNextLine(st,5000,thermd);

// read in density

return _parseText(st,density);

}

McoStatus XRite408::GetPaperDensity(void)
{
int i;
double temp[4];
McoStatus state;

state = getNextPatch(temp);

for (i=0; i<4; i++) paperDensity[i] = temp[i];
return state;
}