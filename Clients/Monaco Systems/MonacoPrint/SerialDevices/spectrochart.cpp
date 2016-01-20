////////////////////////////////////////////////////////////////////////////////
// gretaq table comminications												  //
// xritecom.c																  //
//  James Vogh																  //
//  (c) Monaco Systems Inc.													  //
//  6/25/97																	  //
////////////////////////////////////////////////////////////////////////////////

#include "spectrochart.h"
#include "colortran.h"
#include "assort.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

SpectroChart::SpectroChart(int port_num,DeviceTypes devicenum):GretagCom(port_num,devicenum),Table()
{
char  st[255];


Tx = 355;
Ty = 273;

a = 10;
b = 0;
c = 0;
d = -10;

min_x = 186;
min_y = 230;

max_x = 3360;
max_y = 2617;

onLine = 0;

// make sure that calibration occurs
patchCount = 100;

first = 1;
if (sp == 0L) return;

sp->setmaxwait(300);

thermd = new ThermDialog;
if (thermd != 0L) thermd->SetUpTherm(7);

}

SpectroChart::~SpectroChart(void)
{
if (sp) sp->sendData("; 208 6 0\r\n",11);

if (sp) delete sp;
sp = 0L;

}

McoStatus SpectroChart::Initialize(void)
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
//sp->sendData("; 177 2 2\r\n",11);
//state = CheckError();
//if (state != MCO_SUCCESS) goto bail;


bail:
if (thermd != 0L)
	{
	thermd->DisplayTherm(4,4,0L);	
	thermd->RemoveTherm();
	delete thermd; 
	}
thermd = 0L;


return state;
}


// set the gretag to output lab values
McoStatus SpectroChart::SetLabOutput(void)
{
mode = ReadLabMode;
return state;
}

// set the gretag to output density values
McoStatus SpectroChart::SetDensityOutput(void)
{
mode = ReadDensityMode;
return state;
}


McoStatus SpectroChart::CheckErrorChart(void)
{
char *c;
int code_error[3];
char	st[5000];

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->getNextLine(st,5000,thermd);
if ((st[0] != ';') && (st[0] != ':'))
	{
	last_error = -4;
	return MCO_FAILURE;
	}
c = sscanf_n_i(st,1,3,code_error);
if (c == 0L)
	{
	last_error = -5;
	return MCO_FAILURE;
	}
if (code_error[0] != 209)
	{
	last_error = -6;
	error_code = code_error[0];
	return MCO_FAILURE;
	}
if (code_error[1] != 128)
	{
	last_error = -7;
	error_code = code_error[0];
	return MCO_FAILURE;
	}
if (code_error[2] != 0)
	{
	last_error = code_error[2];
	return MCO_FAILURE;
	}
return MCO_SUCCESS;
}


// get the current location of the table
McoStatus SpectroChart::getLocation(int32 *position,int refrence)
{
char st[5000];

if (sp == 0L) return MCO_SERIAL_ERROR;

sprintf(st,"; 208 5 %d\r\n",refrence);

sp->sendData(st,strlen(st));
sp->getNextLine(st,5000,thermd);
sscanf_n_li(st,5,2,position);
return MCO_SUCCESS;
}




// move a specifed distance in mm
McoStatus SpectroChart::moveDistance(double dx,double dy)
{
char st[5000];
int32 dx_i,dy_i;
int	error;
McoStatus state;
int32 pos[2];

if (sp == 0L) return MCO_SERIAL_ERROR;

getLocation(pos,1);

dx_i = (10*(dx)+0.5);
dy_i = (-10*(dy)+0.5);

pos[0] += dx_i;
pos[1] += dy_i;

pos[0] = MaxVal(pos[0],min_x);
pos[0] = MinVal(pos[0],max_x);

pos[1] = MaxVal(pos[1],min_y);
pos[1] = MinVal(pos[1],max_y);

// turn online
state = turnOnline();
if (state != MCO_SUCCESS) return state;

sprintf(st,"; 208 0 %ld %ld\r\n",pos[0],pos[1]);

sp->sendData(st,strlen(st));
waitTenthSec();
state = CheckErrorChart();

current_x = pos[0];
current_y = pos[1];
return state;
}


int SpectroChart::checkOnLine(void)
{
char st[5000];
int	status;
char *c;

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->clearInputBuf();
sp->sendData("; 208 36\r\n",10);
sp->getNextLine(st,5000,thermd);

if ((st[0] != ';') && (st[0] != ':'))
	{
	last_error = -4;
	}	
c = sscanf_n_i(st,3,1,&status);
if (c == 0L)
	{
	last_error = -5;
	}
if (status != 209)
	{
	last_error = -6;
	error_code = status;
	}
if (status & 0x10) return 1;
return 0;
}	
	
	
McoStatus SpectroChart::turnOnline(void)
{
char st[5000];
McoStatus state;

if (sp == 0L) return MCO_SERIAL_ERROR;

if (checkOnLine()) return MCO_SUCCESS;
sp->sendData("; 208 16\r\n",10);
state = CheckErrorChart();
if (state != MCO_SUCCESS) return state; 
onLine = 1;
return MCO_SUCCESS;
}

McoStatus SpectroChart::turnOffline(void)
{
char st[5000];
McoStatus state;

if (!checkOnLine()) return MCO_SUCCESS;
if (sp == 0L) return MCO_SERIAL_ERROR;

sp->sendData("; 208 17\r\n",10);
state = CheckErrorChart();
if (state != MCO_SUCCESS) return state; 
onLine = 0;
return MCO_SUCCESS;
}

McoStatus SpectroChart::moveToPatch(int32 patchNum,int ref)
{
char st[500];
int32 dx_i,dy_i;
double dist;
int tc;
int waitTime;
int32	table[2];
int error;
McoStatus state;

if (sp == 0L) return MCO_SERIAL_ERROR;

// turn online
state = turnOnline();
if (state != MCO_SUCCESS) return state;

translateToTable(&pointArray[patchNum*2],table);

table[0] = MaxVal(table[0],min_x);
table[0] = MinVal(table[0],max_x);

table[1] = MaxVal(table[1],min_y);
table[1] = MinVal(table[1],max_y);

sprintf(st,"; 208 0 %d %ld %ld\r\n",ref,(long)table[0],(long)table[1]);

dx_i = table[0]-current_x;
dy_i = table[1]-current_y;


sp->sendData(st,strlen(st));
state = CheckErrorChart();
return state;
}


McoStatus SpectroChart::Calibrate(void)
{
char st[5000];
int error;
McoStatus state;

if (sp == 0L) return MCO_SERIAL_ERROR;

// turn online
state = turnOnline();
if (state != MCO_SUCCESS) return state;


sp->sendData("; 208 6 0\r\n",11);
state = CheckErrorChart();
if (state != MCO_SUCCESS) return state; 

// put head down

sp->sendData("; 208 4\r\n",9);
state = CheckErrorChart();
if (state != MCO_SUCCESS) return state; 

// warm it up 5 times
if (first)
	{
	
	sp->sendData("; 32\r\n",6);
	state = CheckError();
	if (state != MCO_SUCCESS) return state; 
//	sp->getNextLine(st,5000,thermd);

	
	sp->sendData("; 32\r\n",6);
	state = CheckError();
	if (state != MCO_SUCCESS) return state; 
//	sp->getNextLine(st,5000,thermd);

	
	sp->sendData("; 32\r\n",6);
	state = CheckError();
	if (state != MCO_SUCCESS) return state;
//	sp->getNextLine(st,5000,thermd);

	
	sp->sendData("; 32\r\n",6);
	state = CheckError();
	if (state != MCO_SUCCESS) return state; 
//	sp->getNextLine(st,5000,thermd);

	sp->sendData("; 32\r\n",6);
	state = CheckError();
	if (state != MCO_SUCCESS) return state; 
//	sp->getNextLine(st,5000,thermd);
	first = 0;
	}

// calibrate

sp->sendData("; 34 9 7\r\n",10);
state = CheckError();
if (state != MCO_SUCCESS) return state; 

// move the head up

sp->sendData("; 208 3\r\n",9);
state = CheckErrorChart();
if (state != MCO_SUCCESS) return state; 

// set patch count to zero

patchCount = 0; 

return MCO_SUCCESS;
}

McoStatus SpectroChart::sendReadCommand(char *reading)
{
char st[5000];
int error;
McoStatus state;

if (sp == 0L) return MCO_SERIAL_ERROR;

// turn online
state = turnOnline();
if (state != MCO_SUCCESS) return state;


// put head down

sp->sendData("; 208 4\r\n",9);
state = CheckErrorChart();
if (state != MCO_SUCCESS) return state; 

// make a measurment
sp->clearInputBuf();
sp->sendData("; 32\r\n",6);
state = CheckError();
if (state != MCO_SUCCESS) return state;
//waitTenthSec(); 
//sp->getNextLine(reading,5000,thermd);

// move the head up
/*
sp->sendData("; 208 3\r\n",9);
sp->getNextLine(st,5000,thermd);
sscanf_n_i(st,3,1,&error);
if (error != 0) return MCO_FAILURE;
*/
return MCO_SUCCESS;
}


McoStatus SpectroChart::getNextPatch(long patchnum,double *lab)
{
char j1[255],j2[255],j3[255];
char st[5000];
char *p;
int32	tc;
double xyz[3];
int32 X,Y,Z;
McoStatus status = MCO_SUCCESS;
int	code;
double t;
int 	i;

if (sp == 0L) return MCO_SERIAL_ERROR;

// do a calibration if 100 patches have been read so far
if (patchCount >= 100) status = Calibrate();
if (status != MCO_SUCCESS) return status;

// move to the patch
status = moveToPatch(patchnum,0);
if (status != MCO_SUCCESS) return status;

// send read command
status = sendReadCommand(st);
if (status != MCO_SUCCESS) return status;



if (mode == ReadLabMode)
	{
	// now read the data in
	sp->sendData("; 186 9 1\r\n",11);
	sp->getNextLine(st,5000,thermd);

	// check for an error
	sscanf_n_i(st,1,1,&code);
	if (code != 187) return MCO_FAILURE;
	
	// read in the data
	sscanf_n(st,2,3,lab);

	// check for an error again
	sscanf_n_i(st,6,1,&code);
	if (code != 0) return MCO_FAILURE;

	// now apply the tweak if it exist
	if (tweak != NULL) tweak->eval(lab,lab,1);
	}
else if (mode == ReadDensityMode)
	{
	// now read the data in
	sp->sendData("; 188 9\r\n",9);
	sp->getNextLine(st,5000,thermd);

	// check for an error
	sscanf_n_i(st,1,1,&code);
	if (code != 189) return MCO_FAILURE;
		
	// read in the black
	sscanf_n(st,1,4,lab);
	
	// check for an error again
	sscanf_n_i(st,6,1,&code);
	if (code != 0) return MCO_FAILURE;
	
	t = lab[0];
	lab[0] = lab[1];
	lab[1] = lab[2];
	lab[2] = lab[3];
	lab[3] = t;
	
	if (lab != paperDensity) for (i=0; i<4; i++)
		{
		lab[i] -= paperDensity[i];
		if (lab[i] < 0) lab[i] = 0;
		}

	
	}

patchCount++;



return MCO_SUCCESS;
}

McoStatus SpectroChart::GetPaperDensity(void)
{
return getNextPatch(paperNum,paperDensity);
}