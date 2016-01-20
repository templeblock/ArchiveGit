/*c3Win95io.c */

#include "stdafx.h"

#include "c3win95io.h"
#include <windows.h>

#define  INQUE_SIZE            1024
#define  OUTQUE_SIZE           1024

#define nil  0
#define LINES_HIGH		1
#define LINES_LOW		0
#define PowerOnTimeDelay       2500L

/*******************************************************
 * static defines for rs232 serial i/o
 *******************************************************/
static DCB         CommDCB;               // DCB for comm port
static HANDLE       PortID=0;                // The comm port id
static COMSTAT     CommStat;              // COMSTAT info buffer for comm port
static char     comPortString[8]={'C', 'O', 'M', 0, 0, 0, 0, 0};

/*******************************************************
 * Prototypes for Windows95 rs232 only routines
 *******************************************************/
static short win_getCommErr_return ( int );
static long setup_dcb ( GIZ_PORT port, short settings, short lineflag )	;
static void  SISetLinesLow ( void );
static void  SISetLinesHigh ( void );


/* Globals */
//char           gizTimoFlag     = TIMO_ENABLE;
//           gizTimoLimit    = TIMO_DEFAULT_LIMIT;

 COMMTIMEOUTS TimeOutData;

/*=========================================================================
 *
 * NOTE To Developers:
 *
 * The following routines are not platform specific but must be present
 * in the platform specific file (the file you are reading!).  Later in this
 * file, a second note distinguishes the platform specific and the non-platform
 * specific functions.
 *
=========================================================================*/
/**************************************************************************
 * -Name:        win_InitSerial ( port, settings )
 *
 * -Description: This routine initializes the communications port.
 *
 * -Arguments:   GIZ_PORT porttoopen - communications port to be used
 *                       GIZ_PORT_COM1 - for communications port 1
 *                       GIZ_PORT_COM2 - for communications port 2
 *               short portsettings - communications port parameters
 *                    modes are OR'd together to achieve comm port settings.
 *                    Default: GIZBAUD_9600 | GIZNONE | GIZONESTOPBIT | GIZEIGHTBITS.
 *
 *                    baud rates : GIZBAUD_300, GIZBAUD_600, GIZBAUD_1200,
 *                                 GIZBAUD_2400, GIZBAUD_4800, GIZBAUD_9600
 *
 * -Notes:       Gizzmo must have comm port settings of no parity (GIZNONE),
 *               1 stop bit (GIZONESTOPBITS), and 8 data bits (GIZEIGHTBITS).
 *               The baud rate is the only settings that can currently
 *               be varied.  To do this, please review the programmers'
 *               guide on changing the baud rate.
 *
 * -Returns : 0 if comm port setup okay
 *************************************************************************/
long  win_InitSerial ( int port )
{
	long err;
	DWORD  start,current;
    char aBuf[16];
	DWORD bytesRead;
	if((err = setup_dcb( (GIZ_PORT)(port-1), 0, (short)LINES_LOW))!=0)
		return err;
	SISetLinesLow();
	while ((current-start) < PowerOnTimeDelay) current = GetTickCount(); // PowerOnTime delay before launch to let calibrator wake up and stabilize
	
	/* create timeout data structure */
	GetCommTimeouts(PortID,&TimeOutData);
	TimeOutData.ReadIntervalTimeout = 2500;// millisecs between characters
	TimeOutData.ReadTotalTimeoutMultiplier = 250;	// millisec multiplier by number of characters
	TimeOutData.ReadTotalTimeoutConstant = 2500; // 2.5 sec + nchar * 250 msec total time out(about 3secs);
	TimeOutData.WriteTotalTimeoutMultiplier = 250;
	TimeOutData.WriteTotalTimeoutConstant = 250;
	SetCommTimeouts(PortID,&TimeOutData);
	start = GetTickCount();
	current = GetTickCount();
	SISetLinesHigh();
	while ((current-start) < PowerOnTimeDelay) current = GetTickCount(); // PowerOnTime delay before launch to let calibrator wake up and stabilize
	
	ReadFile(
		PortID,		// handle of file to read 
		aBuf,		// address of buffer that receives data  
		1,			// number of bytes to read: the count byte and the command byte
		&bytesRead,	// address of number of bytes read 
		NULL 		// address of structure for data 
	);
   	return (err);
}

void win_SetTimeOut(double dataInSecs)
{


  GetCommTimeouts(PortID,&TimeOutData);
	
	TimeOutData.ReadTotalTimeoutConstant = short(dataInSecs * 1000) + 500; // 2.5 sec + nchar * 250 msec total time out(about 3secs);
	
	SetCommTimeouts(PortID,&TimeOutData);





}
/**************************************************************************
 * -Routine:     setup_dcb ( GIZ_PORT, short, short )
 *
 * -Description: Setups the Device Control Block
 *
 * -Arguments:   None
 *
 * -Notes:       
 *
 * -Returns :    gizzmo error code for the windows' comm error passed in.
 *             
 *************************************************************************/
static long setup_dcb ( GIZ_PORT port, short settings, short lineflag )
{
     DWORD commerr;
     BOOL err;

// append the decimal comm port number to the string "COMx"
  if ((port >= 0) && (port <= 8)) 
  {
    comPortString[3] = (char)(port+1 + 0x30);
    comPortString[4] = 0x00;
  }

  PortID = CreateFile(comPortString,
	  GENERIC_READ|GENERIC_WRITE,
	  0,
	  0,
	  OPEN_EXISTING,
	  0,
	  0);
  
  if(PortID == INVALID_HANDLE_VALUE) 
	  return ((long)INVALID_HANDLE_VALUE);
  
  GetCommState(PortID,&CommDCB);  // Get the current system default

   // Setup DCB 
    CommDCB.DCBlength = sizeof(DCB);           
    CommDCB.BaudRate = CBR_9600;            // current baud rate           
    CommDCB.fBinary = TRUE;          // binary mode, no EOF check   
    CommDCB.fParity =FALSE;          // enable parity checking      
    CommDCB.fOutxCtsFlow = FALSE;      // CTS output flow control     
    CommDCB.fOutxDsrFlow = FALSE;      // DSR output flow control     
    if (lineflag == LINES_HIGH) 
    {

	    	

    	CommDCB.fDtrControl= DTR_CONTROL_ENABLE;       // DTR flow control type 
    	CommDCB.fRtsControl= RTS_CONTROL_ENABLE;	   // RTS flow control
    }
    
    else
	{
    
         CommDCB.fDtrControl= DTR_CONTROL_DISABLE;       // DTR flow control type 
         CommDCB.fRtsControl= RTS_CONTROL_DISABLE;		// RTS flow control
    }
          
    CommDCB.fDsrSensitivity = FALSE;   // DSR sensitivity             
    CommDCB.fTXContinueOnXoff= FALSE; // XOFF continues Tx           

    CommDCB.fOutX= FALSE;        // XON/XOFF out flow control       
    CommDCB.fInX= FALSE;         // XON/XOFF in flow control        
    CommDCB.fErrorChar= FALSE;   // enable error replacement        
    CommDCB.fNull= FALSE;        // enable null stripping 
               
    CommDCB.fAbortOnError= FALSE; // abort reads/writes on error     
    //CommDCB.fDummy2:17;      // reserved                        
    // wReserved;        // not currently used              
    
    CommDCB.XonLim;           // transmit XON threshold          

    CommDCB.XoffLim;          // transmit XOFF threshold         
    CommDCB.ByteSize = 8;         // number of bits/byte, 4-8        
    CommDCB.Parity = NOPARITY;           // 0-4=no,odd,even,mark,space      
    CommDCB.StopBits = 0;         // 0,1,2 = 1, 1.5, 2               
         
	SetCommState(PortID,&CommDCB);

	err = SetupComm( PortID, INQUE_SIZE, OUTQUE_SIZE );



  if(err==FALSE)
  {
   	  commerr = GetLastError();
	  return (commerr);
  }
  else
   return(0); //return 0 if ok;
}


/**************************************************************************
 * -Routine:     win_CloseSerial ( void )
 *
 * -Description: This routine MUST be called when finished with the serial
 *               interface.
 *
 * -Arguments:   None
 *
 * -Notes:       None
 *
 * -Returns :    0
 *************************************************************************/
long win_CloseSerial ( void )
{
  DWORD   commerr; 
  BOOL err;
  COMSTAT comstat;           
            
  err = PurgeComm(PortID,PURGE_TXABORT	); /* flush output queue */
  if (err != TRUE) {
     ClearCommError(PortID,&commerr,&comstat); // because an error occurred, we MUST
                          // make this call to clear it.
  }
     
   SISetLinesLow();

   err = CloseHandle(PortID);
    
   PortID = 0;      // invalidating PortID because we just closed it
   return 0;
}

/**************************************************************************
 * -Routine:     win_SendGetCMDandData(char *inBuf, short *sPtrnumBytes)
 *
 * -Description: 
 *
 * -Arguments:   None
 *
 * -Notes:       inbuf[0] = number of bytes following in command
 *				 inbuf[1] = command
 *				 inbuf[n] = command data (if any)
 *
 * -Returns :    0
 *************************************************************************/
long win_SendGetCMDandData(char *inBuf, short *sPtrnumBytes)
{
	short          err=0;
	unsigned long  length;
	char            gotNumBytes=FALSE, cmdByte ;
	char			aBuf[256],bBuf[356],  cBuf[256],*cPtr;
	int            bytesLeft ;
	

	DWORD		  count,bytesRead,bytesRead1;

   
  	cmdByte = inBuf[1]; //this is the command which is sent;
	
	// Send the command buffer 
	WriteFile(
		PortID,		// handle of file to write to 
		inBuf,		// address of data to write to file 
		inBuf[0]+1,	// number of bytes to write (plus 1 for count-byte sent)
		&length,	// address of number of bytes written 
		NULL 		// addr. of structure needed for overlapped I/O  
	);

	bytesRead = 0;
	bytesRead1 = 0;
	bytesLeft = 1;   /* number of bytes to read first to get binary data to follow */

	// Get the count and echoed command back 

	if(ReadFile(
		PortID,		// handle of file to read 
		cBuf,		// address of buffer that receives data  
		2,			// number of bytes to read: the count byte and the command byte
		&bytesRead,	// address of number of bytes read 
		NULL 		// address of structure for data 
	)!= TRUE)
		return(GetLastError());
   
	count = (DWORD)cBuf[0];	// number of bytes returned from calibrator
	inBuf[0] = cBuf[1];
	if(bytesRead != 2)
		return (-1); //comm error has occured

	

	if((count > 0) && (count < 10))	// data returned
	{
		if(ReadFile(
			PortID,			// handle of file to read 
			cBuf,			// address of buffer that receives data  
			count-1,		// number of bytes to read: minus command byte
			&bytesRead1,	// address of number of bytes read 
			NULL 			// address of structure for data 
		)!=TRUE)
		return(GetLastError());

	if( (bytesRead1 != (count-1) ))



			return -1;		// comm error
	
		memcpy(&inBuf[1],&cBuf[0],bytesRead1);
		*sPtrnumBytes = (short)bytesRead1;
	}
	else			// no data returned
		*sPtrnumBytes = -1;

	return err;
}

 void  SISetLinesLow ( void )
{
   LONG  lCommErr;

   lCommErr = EscapeCommFunction( PortID, CLRDTR );
   lCommErr = EscapeCommFunction( PortID, CLRRTS );
}


void  SISetLinesHigh ( void )
{
   LONG  lCommErr;

   lCommErr = EscapeCommFunction( PortID, SETDTR );
   lCommErr = EscapeCommFunction( PortID, SETRTS );
}
