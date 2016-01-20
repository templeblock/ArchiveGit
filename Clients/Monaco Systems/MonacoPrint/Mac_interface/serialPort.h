#ifndef SERIAL_PORT
#define SERIAL_PORT

//////////////////////////////////////////////////////////////////////////////////////////
//  Serial port library																	//
//	serialPort.h																			//
//	James Vogh																			// 
//////////////////////////////////////////////////////////////////////////////////////////

#include "mcotypes.h"
#include "mcostat.h"
#include "Thermometer.h"

typedef enum {
SPmodem = 0,
SPprinter
} SerialPortNum;

typedef enum {
	b300 = 0,
	b1200,
	b2400,
	b4800,
	b9600,
	b19200,
	b38400,
	b57600
	} SerialSpeeds;

class serialPort {
private:
protected:
	short	OutputRefNum;
	short	InputRefNum;
	Handle	InputBufHandle;
	
	
	int 	Baud,Stop,Data,Parity;
	
	char	lineend;
	int		maxwait;

public:
OSErr	OsError;
serialPort(SerialPortNum portnum,int32 bufSize,int XonXoff, int DtrCts, 
	int baud,int data,int parity,int stop);
~serialPort(void);

void setlineend(char e);
void setmaxwait(int wait);
void setBaud(int baud);
void sendData(void *buf,int32 length);	
void getData(void *buf,int32 max_length,int32 &read_length);
void getNextLine(char *buf,int32 max_length,ThermDialog *thermd);
void clearInputBuf(void);
long numInChars(void);
McoStatus checkState(void);
McoStatus setDTR(void);
McoStatus clearDTR(void);
};

void waitTenthSec(void);
#endif