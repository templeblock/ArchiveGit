//////////////////////////////////////////////////////////////////////////////////////////
//  Serial port library																	//
//	serialPort.c																			//
//	James Vogh																			// 
//////////////////////////////////////////////////////////////////////////////////////////

#include 	"serialPort.h"
#include	<Serial.h>
#include 	"mcomem.h"

serialPort::serialPort(SerialPortNum portnum,int32 bufSize,int XonXoff, int DtrCts, 
	int baud,int data,int parity,int stop)
{
char *bufptr;
SerShk	mySerShk;
int		configParam;

//OsError = noErr;
//return;

InputBufHandle = NULL;
lineend = '\r';
maxwait = 600;

switch (portnum) 
	{
	// Open the port
	
	case SPmodem:
		OsError = OpenDriver("\p.AOut",&OutputRefNum);
		if (OsError == noErr) OsError = OpenDriver("\p.AIn",&InputRefNum);
		break;
	case SPprinter:
		OsError = OpenDriver("\p.BOut",&OutputRefNum);
		if (OsError == noErr) OsError = OpenDriver("\p.BIn",&InputRefNum);
		break;
	}
	// if an error has occured then exit
	if (OsError != noErr) 
		{
		return;
		}
	
	// set the input buffer size
	InputBufHandle = McoNewHandle(bufSize);
	if (InputBufHandle == NULL)
		{
		return;
		}	
	bufptr = (char *)McoLockHandle(InputBufHandle);
	SerSetBuf(InputRefNum,bufptr,bufSize);
	
	// set the flow control
	mySerShk.fXOn= XonXoff;
	mySerShk.fInX = XonXoff;
	mySerShk.fCTS = DtrCts;
	mySerShk.fDTR = DtrCts;
	mySerShk.errs = 0;
	mySerShk.evts = 0;
	OsError = Control(OutputRefNum,14,&mySerShk);
	
	Baud = baud;
	Data = data;
	Parity = parity;
	Stop = stop;
	
	// set the baud 
	switch (baud) {
		case b300: configParam = baud300;
			break;
		case b1200: configParam = baud1200;
			break;
		case b2400: configParam = baud2400;
			break;
		case b4800: configParam = baud4800;
			break;
		case b9600: configParam = baud9600;
			break;
		case b19200: configParam = baud19200;
			break;
		case b38400: configParam = baud38400;
			break;		
		case b57600: configParam = baud57600;
			break;			
		}
		
	switch (data) {
		case 0: configParam += data5;
			break;
		case 1: configParam += data6;
			break;
		case 2: configParam += data7;
			break;
		case 3: configParam += data8;
		}
		
	switch (parity) {
		case 0: configParam += noParity;
			break;
		case 1: configParam += oddParity;
			break;	
		case 2: configParam += evenParity;
			break;
		}
		
	switch (stop) {
		case 0: configParam += stop10;
			break;
		case 1: configParam += stop15;
			break;	
		case 2: configParam += stop20;
			break;
		}
	OsError = SerReset(OutputRefNum,configParam);
	if (OsError != noErr) 
		{
		return;
		}		
}	
	
serialPort::~serialPort(void)	
{
char *bufptr;

// get rid of the buffer
if (InputBufHandle != NULL)
	{
	bufptr = *InputBufHandle;
	
	SerSetBuf(InputRefNum,bufptr,0);
	McoUnlockHandle(InputBufHandle);
	McoDeleteHandle(InputBufHandle);
	}

// Close the driver

OsError = KillIO(OutputRefNum);
if (OsError == noErr) OsError = CloseDriver(InputRefNum);
if (OsError == noErr) OsError = CloseDriver(OutputRefNum);
}

void serialPort::setlineend(char e)
{
lineend = e;
}

void serialPort::setmaxwait(int wait)
{
maxwait = wait;
}


void serialPort::setBaud(int baud)
{
int		configParam;

	switch (baud) {
		case b300: configParam = baud300;
			break;
		case b1200: configParam = baud1200;
			break;
		case b2400: configParam = baud2400;
			break;
		case b4800: configParam = baud4800;
			break;
		case b9600: configParam = baud9600;
			break;
		case b19200: configParam = baud19200;
			break;
		case b38400: configParam = baud38400;
			break;		
		case b57600: configParam = baud57600;
			break;			
		}
		
	switch (Data) {
		case 0: configParam += data5;
			break;
		case 1: configParam += data6;
			break;
		case 2: configParam += data7;
			break;
		case 3: configParam += data8;
		}
		
	switch (Parity) {
		case 0: configParam += noParity;
			break;
		case 1: configParam += oddParity;
			break;	
		case 2: configParam += evenParity;
			break;
		}
		
	switch (Stop) {
		case 0: configParam += stop10;
			break;
		case 1: configParam += stop15;
			break;	
		case 2: configParam += stop20;
			break;
		}
	OsError = SerReset(OutputRefNum,configParam);
}

void serialPort::sendData(void *buf,int32 length)
{
ParamBlockRec	myPB;

myPB.ioParam.ioRefNum = OutputRefNum;
myPB.ioParam.ioBuffer = (Ptr)buf;
myPB.ioParam.ioReqCount = length;
myPB.ioParam.ioCompletion = NULL;
myPB.ioParam.ioVRefNum = 0;
myPB.ioParam.ioPosMode = 0;

OsError = PBWrite(&myPB,FALSE);
}

void serialPort::getData(void *buf,int32 max_length,int32 &read_length)
{
int32	length;
ParamBlockRec	myPB;

length = 0;
OsError = SerGetBuf(InputRefNum,&length);

if (length > 0)
	{
	if (length>max_length) length = max_length;
	myPB.ioParam.ioRefNum = InputRefNum;
	myPB.ioParam.ioBuffer = (Ptr)buf;
	myPB.ioParam.ioReqCount = length;
	myPB.ioParam.ioCompletion = NULL;
	myPB.ioParam.ioVRefNum = 0;
	myPB.ioParam.ioPosMode = 0;
	read_length = length;
	OsError = PBRead(&myPB,FALSE);
	}
else read_length = 0;
}


// GetNext reads a line of text and then returns
// It will wait 5 seconds before it times out

void serialPort::getNextLine(char *line,int32 max_length,ThermDialog *thermd)
{
int32 tc;
char 	c;
int32	l;
int32	cl= 0;

tc = TickCount();
while (TickCount() - tc < maxwait)
	{
	getData(&c,1,l);
	if (l>0) 
		{
		line[cl] = c;
		if (c == lineend) { line[cl+1] = 0; return; }
		cl++;
		if (cl == max_length) return;
		}
	if (thermd != NULL) if (!thermd->DisplayTherm(-1,-1,NULL)) return;
	}
}

void serialPort::clearInputBuf(void)
{
int l;
char st[5000];

do {
	getData(st,5000,l);
	} while(l>0);
}

long serialPort::numInChars(void)
{
long length;

OsError = SerGetBuf(InputRefNum,&length);
return length;
}

McoStatus serialPort::checkState(void)
{
if (OsError == noErr) return MCO_SUCCESS;
return MCO_FAILURE;
}

void waitTenthSec(void)
{
int j = 0;
int tc;

tc = TickCount();
while (TickCount() - tc < 6) 
	{
	j ++;
	}
return;
}


McoStatus serialPort::setDTR(void)
{
Control(OutputRefNum,18,NULL);	//assert DTR->0V 
return MCO_SUCCESS;
}

McoStatus serialPort::clearDTR(void)
{
Control(OutputRefNum,17,NULL);	//assert DTR->5V 
return MCO_SUCCESS;
}