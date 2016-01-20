////////////////////////////////////////////////////////////////////////////////
// Xrite comminications														  //
// xritecom.c																  //
//  James Vogh																  //
////////////////////////////////////////////////////////////////////////////////

#include "tcr.h"
#include "colortran.h"
#include "assort.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

TCR::TCR(int port_num,DeviceTypes device_type):
Techkon(port_num,device_type),Table()
{
char  st[255];

a = 40;
b = 0;
c = 0;
d = 40;

Tx = 400;

Ty = 11000;

if (sp == 0L) return;

sp->setlineend((char)0x0d);

sprintf(st,"PD;");

sp->sendData(st,strlen(st));

sprintf(st,"VS %ld;",10);

waitTenthSec();

sp->sendData(st,strlen(st));

sprintf(st,"PU;");

sp->sendData(st,strlen(st));

min_x = 400;
min_y = 712;

max_x = 15462;
max_y = 11880;

doPenDown = 1;

}

TCR::~TCR(void)
{
char	st[255];
if (sp != 0L)
	{
	sprintf(st,"PA 400,11000;");

	sp->sendData(st,strlen(st));
	delete sp;
	sp = 0L;
	}
}

McoStatus TCR::Initialize(void)
{
char	st[255];
if (sp == 0L) return MCO_SERIAL_ERROR;


	sprintf(st,"IN; PA 400,11000; PD; VS 10;");

	sp->sendData(st,strlen(st));

return MCO_SUCCESS;
}


// get the current location of the table
McoStatus TCR::getLocation(int32 *position,int refrence)
{
char st[5000];

if (sp == 0L) return MCO_SERIAL_ERROR;

sprintf(st,"OA;");
sp->setDTR();
waitTenthSec();
sp->sendData(st,strlen(st));
waitTenthSec();
sp->getNextLine(st,5000,thermd);
sp->clearDTR();
sscanf(st,"%ld,%ld",&position[0],&position[1]);
return MCO_SUCCESS;
}


// move a specifed distance in mm
McoStatus TCR::moveDistance(double dx,double dy)
{
char st[500];
int32 dx_i,dy_i;
int32 pos[2];

if (sp == 0L) return MCO_SERIAL_ERROR;

getLocation(pos,0);

dx_i = (40*(dx)+0.5);
dy_i = (40*(dy)+0.5);

pos[0] += dx_i;
pos[1] += dy_i;

pos[0] = MaxVal(pos[0],min_x);
pos[0] = MinVal(pos[0],max_x);

pos[1] = MaxVal(pos[1],min_y);
pos[1] = MinVal(pos[1],max_y);

sprintf(st,"PA %ld %ld;",pos[0],pos[1]);

current_x = pos[0];
current_y = pos[1];

sp->sendData(st,strlen(st));
waitTenthSec();

return MCO_SUCCESS;
}

// move the device to the specified patch

McoStatus TCR::moveToPatch(int32 patchNum, int ref)
{
char st[500];
int32 dx_i,dy_i;
double dist;
long tc;
int waitTime;
int32	table[2];

if (sp == 0L) return MCO_SERIAL_ERROR;

translateToTable(&pointArray[patchNum*2],table);

//dx_i = table[0]-current_x;
//dy_i = table[1]-current_y;

//sprintf(st,"PR %ld %ld;",dx_i,dy_i);	// the move command

table[0] = MaxVal(table[0],min_x);
table[0] = MinVal(table[0],max_x);

table[1] = MaxVal(table[1],min_y);
table[1] = MinVal(table[1],max_y);

sprintf(st,"PA %ld %ld;",(long)table[0],(long)table[1]);	// the move command

sp->sendData(st,strlen(st));


dx_i = table[0]-current_x;
dy_i = table[1]-current_y;

// wait a time that is determined by the distance moved
dist = sqrt((double)(dx_i*dx_i+dy_i*dy_i))*0.025;

// assume that it moves 250mm/sec

waitTime = 36+(int)(60.0*dist/150.0);

tc = TickCount();
while (TickCount() - tc < waitTime) {}

//current_x = pointArray[patchNum*2];
//current_y = pointArray[patchNum*2+1];

current_x += dx_i;
current_y += dy_i;

return MCO_SUCCESS;
}


//McoStatus TCR::compute


McoStatus TCR::getNextPatch(long patchnum,double *lab)
{
char j1[255],j2[255],j3[255];
unsigned char st[5000];
char *p;
int32	tc;
double xyz[3];
int32 X,Y,Z;
int i,j;

if (sp == 0L) return MCO_SERIAL_ERROR;

sp->clearInputBuf();

if (doPenDown)
	{
	sprintf(j1,"PU;");

	sp->sendData(j1,strlen(j1));
	}
else
	{
	sprintf(j1,"PD;");
	sp->sendData(j1,strlen(j1));
	}

// move to the patch
moveToPatch(patchnum,0);


if (doPenDown)
	{
	sprintf(j1,"PD;");

	sp->sendData(j1,strlen(j1));

	tc = TickCount();
	while (TickCount() - tc < 10) {}
	}

for (j=0; j<3; j++)
	{
	// send read command
	sendReadCommand();

	// get the 18 byte packet of info
	for (i=0; i<2; i++)
		{
		sp->getData(st,18,tc);
		if (tc) break;
		waitTenthSec();
		}
	if (tc) break;
	waitTenthSec();
	}
// check length
if (tc != 18) return MCO_FAILURE;

// do some checking
if (st[0] != 0xAA) return MCO_FAILURE;

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