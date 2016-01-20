/* SipCal_glue.c */

#include "stdafx.h"

#include "chrfuncs.h"
#include "c3win95io.h"
#include "memory.h"


char  gTimoFlag     = TIMO_ENABLE;
long  gTimoLimit    = TIMO_DEFAULT_LIMIT;

unsigned char toolkit_rev[] = "Sequel Imaging Chroma Win 95 Toolkit Ver. 4.0.3";

unsigned char  *IOgetttoolkitrev ( void )
{
   return (toolkit_rev);
}


 long IOInit ( short port )
{
	long err;
	err = win_InitSerial ( port )	;
	
	return(err);
	
};
 long IOClose ( void )
{
		
	
	 win_CloseSerial();
	 return(SUCCESS);
	
};
 long IOSendGetData ( short foo, char *outbuf, short soutcount , char *inbuf, short * sincount)
 {
	long err;
	
	short count;

	char myBuffer[2048];
	memcpy(myBuffer,outbuf,soutcount);

	err = win_SendGetCMDandData(myBuffer, &count);

	memcpy(inbuf,myBuffer,count+1);

	*sincount = count;
	return (err); 
	 
 };
