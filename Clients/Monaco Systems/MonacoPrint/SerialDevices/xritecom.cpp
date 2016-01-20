////////////////////////////////////////////////////////////////////////////////
// Xrite comminications														  //
// xritecom.c																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#include "xritecom.h"
#include <string.h>
#include <stdio.h>

XriteCom::XriteCom(int port_num,DeviceTypes devicenum,int p_type):InputDevice(devicenum)
{
sp = new serialPort((SerialPortNum)port_num,XR_DEFAULT_BUFF, XR_DEFAULT_XON,XR_DEFAULT_DTS,
				    XR_DEFALUT_BAUD,XR_DEFAULT_DATA,XR_DEFAULT_PARITY,
				    XR_DEFAULT_STOP);
//sp = new serialPort((SerialPortNum)port_num,XR_DEFAULT_BUFF, 1,0,
//				    XR_DEFALUT_BAUD,XR_DEFAULT_DATA,XR_DEFAULT_PARITY,
//				    XR_DEFAULT_STOP);

page_type = p_type;

if (sp == 0L) goto bail;
if (sp->OsError != noErr) 
	{
	delete sp;
	sp = 0L;
	goto bail;
	}

thermd = 0L;

state = MCO_SUCCESS;
return;
bail:
state = MCO_SERIAL_ERROR;

}

XriteCom::~XriteCom(void)
{
if (sp) delete sp;
sp = 0L;
}

Boolean XriteCom::IsXriteSV(char *st)
{
int i;
char xst[] = "X-Rite DTP";

for (i=0; i<strlen(xst); i++)
	{
	if (st[i] != xst[i]) return FALSE;
	}
return TRUE;
}


// set the baud rate of the xrite to the specified baud rate
// first have to find the rate that it is currently set to
McoStatus XriteCom::setBaud(int baud)
{
char st[5000];
int i;
int32	l;
Boolean	isSV = FALSE;
Str255	the_string;

if (sp == 0L) return MCO_SERIAL_ERROR;

GetIndString(the_string,XRITE_MESS,1);
if (thermd != NULL) if (!thermd->DisplayTherm(1,4,the_string)) return MCO_FAILURE;

if ((baud < b1200) || (baud >b19200)) return MCO_FAILURE;
// first test at the current baud

sp->setBaud(baud);

sp->clearInputBuf();

sp->sendData("SV\r\n",3);
sp->getNextLine(st,5000,thermd);
if (IsXriteSV(st)) return MCO_SUCCESS;

for (i=(int)b19200; i>=(int)b1200; i--)
	{
	sp->setBaud(i);
	
	sp->clearInputBuf();
	waitTenthSec();
	sp->sendData("SV\r\n",3);
	waitTenthSec();
	waitTenthSec();
	sp->getNextLine(st,5000,thermd);
	isSV = IsXriteSV(st);
	if (isSV) break;
	waitTenthSec();
	waitTenthSec();
	if (thermd != NULL) if (!thermd->DisplayTherm(1,4,the_string)) return MCO_FAILURE;
	}
if (isSV)
	{
	waitTenthSec();
	waitTenthSec();
	waitTenthSec();
	switch (baud) {
		case 1: sp->sendData("06BR\r\n",5);
			break;
		case 2: sp->sendData("0CBR\r\n",5);
			break;
		case 3: sp->sendData("18BR\r\n",5);
			break;
		case 4: sp->sendData("30BR\r\n",5);
			break;
		case 5: sp->sendData("60BR\r\n",5);
			break;
		}
	sp->sendData("\r\n",1);
	waitTenthSec();
	waitTenthSec();
	sp->setBaud(baud);
	waitTenthSec();
	sp->clearInputBuf();
	
	sp->sendData("SV\r\n",3);
	waitTenthSec();
	sp->getNextLine(st,5000,thermd);
	if (IsXriteSV(st)) return MCO_SUCCESS;
	}
return MCO_FAILURE;	
}

McoStatus XriteCom::setXriteStripDef(char *stripDef,char *passes_loc)
{
char	st[5000];
int32	l;
char 	*p;
int32	tc;
Str255	the_string;

if (sp == 0L) return MCO_SERIAL_ERROR;

GetIndString(the_string,XRITE_MESS,2);
if (thermd != NULL) if (!thermd->DisplayTherm(2,4,the_string)) return MCO_FAILURE;


// first check to see if configuration is already OK
waitTenthSec();
waitTenthSec();
waitTenthSec();
sp->sendData("00SD\r\n",5);
waitTenthSec();
sp->getData(st,5000,l);

p = strstr(st,stripDef);
if (p == NULL)
	{
	sp->clearInputBuf();
	sp->sendData(passes_loc,strlen(passes_loc));
	sp->sendData("DS\r\n",3);
	waitTenthSec();
	tc = TickCount();
	do {
		sp->getData(st,1,l);
		} while ((st[0] != '*') && (TickCount() - tc < 600));
	if (st[0] != '*') return MCO_FAILURE;
	sp->clearInputBuf();
	sp->sendData(stripDef,strlen(stripDef));
	sp->sendData("\r\n",2);
	waitTenthSec();
	waitTenthSec();
	sp->clearInputBuf();
	sp->sendData("SV\r\n",4);
	waitTenthSec();
	sp->getNextLine(st,5000,thermd);
//	if (strstr(st,"X-Rite DTP") != NULL) return MCO_SUCCESS;
	return MCO_SUCCESS;
	}
else return MCO_SUCCESS;
}

McoStatus XriteCom::Initialize()
{
McoStatus state;
Str255	the_string;
char	st[5000];

if (sp == 0L) return MCO_SERIAL_ERROR;

thermd = new ThermDialog;
if (thermd != NULL) thermd->SetUpTherm(7);

state = setBaud(XR_DEFALUT_BAUD);
if (state != MCO_SUCCESS) goto bail;


waitTenthSec();
sp->sendData("0PR\r\n",5);
waitTenthSec();
sp->sendData("0104CF\r\n",8);	// turn on hardware handshaking
waitTenthSec();

switch (page_type) {
	case 2:
		state = setXriteStripDef(XR_STRIPDEF_11_17,PASS_LOC);
		break;
	case 1:
	default:
		state = setXriteStripDef(XR_STRIPDEF_8_11,PASS_LOC);
		break;
	}
if (state != MCO_SUCCESS) goto bail;


GetIndString(the_string,XRITE_MESS,1);
if (thermd != NULL) if (!thermd->DisplayTherm(3,4,the_string)) goto bail;


waitTenthSec();
waitTenthSec();
waitTenthSec();
sp->sendData("1PK\r\n",5);
waitTenthSec();
sp->sendData("0RS\r\n",5);
waitTenthSec();
sp->sendData("CE\r\n",4);
waitTenthSec();
sp->sendData("0105CF\r\n",8);
waitTenthSec();
sp->clearInputBuf();


bail:
if (thermd != NULL)
	{
	thermd->DisplayTherm(4,4,NULL);	
	thermd->RemoveTherm();
	delete thermd; 
	}
thermd = NULL;
	
return MCO_SUCCESS;
}

Boolean XriteCom::IsBufferEmpty(void)
{
if (sp == 0L) return TRUE;
if (sp->numInChars() > 0) return FALSE;
return TRUE;
}
/*
Boolean XriteCom::IsStripPresent(void)
{
char st[5000];
int32	length;
char	*p;
int32	tc;

strcpy(st,"");

waitTenthSec();
sp->getData(st,5000,length);
p = strstr(st,"<00>");
if (p != NULL)
	{
	waitTenthSec();
	sp->sendData("0PR\r\n",5);
	waitTenthSec();
	sp->sendData("1PK\r\n",5);
	waitTenthSec();
	sp->sendData("1RS\r\n",5);
	waitTenthSec();
	sp->sendData("CE\r\n",4);
	waitTenthSec();
	return TRUE;
	}
waitTenthSec();
sp->sendData("CE\r\n",4);
tc = TickCount();
while ((sp->numInChars() < 4) && (TickCount() - tc < 800)) {p =NULL;}
sp->clearInputBuf();
waitTenthSec();
return FALSE;
}

McoStatus XriteCom::getNextPatch(double *lab)
{
char j1[255],j2[255],j3[255];
char st[5000];

waitTenthSec();
sp->clearInputBuf();
sp->sendData("NP\r\n",3);
waitTenthSec();
sp->getNextLine(st,5000);

if (strlen(st) > 0)
	{
	sscanf(st,"%s %lf %s %lf %s %lf",j1,&lab[0],j2,&lab[1],j3,&lab[2]);
	if (!(strstr(j1,"L")) || !(strstr(j2,"a")) || !(strstr(j3,"b"))) return MCO_FAILURE;
	}
else return MCO_FAILURE;
waitTenthSec();
sp->clearInputBuf();
return MCO_SUCCESS;
}
*/

Boolean XriteCom::IsDataPresent(void)
{

int32	last_size;
int32	current_size;

// Just wait around until buffer gets large enough 
// and stabilizes
if (sp == 0L) return FALSE;

do {
	last_size = sp->numInChars();
	//waitTenthSec();
	//waitTenthSec();
	//waitTenthSec();
	waitTenthSec();
	current_size = sp->numInChars();
	} while (last_size != current_size);
	
if (current_size < 30) 
	{
	//sp->clearInputBuf();
	waitTenthSec();
	return FALSE;
	}
// Turn off strip reading	
//sp->sendData("0SM\r\n",5);	
//waitTenthSec();
return TRUE;
}

McoStatus XriteCom::TurnOnStripRead(void)
{
if (sp == 0L) return MCO_SERIAL_ERROR;
sp->clearInputBuf();
sp->sendData("1SM\r\n",5);	
waitTenthSec();
return MCO_SUCCESS;
}

McoStatus XriteCom::getNextPatch(double *lab)
{
char j1[255],j2[255],j3[255];
char st[5000];
char *p;
int32	tc;

if (sp == 0L) return MCO_SERIAL_ERROR;
//waitTenthSec();
//sp->clearInputBuf();
//sp->sendData("NP\r\n",3);
//waitTenthSec();
sp->getNextLine(st,5000,thermd);

p = strstr(st,"L");
tc = TickCount();
while ((p == NULL) && (sp->numInChars() > 0) && (TickCount() - tc < 800))
	{
	sp->getNextLine(st,5000,thermd);
	p = strstr(st,"L");
	}

if (p == NULL) return MCO_FAILURE;

if (strlen(p) > 0)
	{
	sscanf(p,"%s %lf %s %lf %s %lf",j1,&lab[0],j2,&lab[1],j3,&lab[2]);
	if (!(strstr(j1,"L")) || !(strstr(j2,"a")) || !(strstr(j3,"b"))) return MCO_FAILURE;
	}
else return MCO_FAILURE;
// now apply the tweak if it exist
if (tweak != NULL) tweak->eval(lab,lab,1);
return MCO_SUCCESS;
}