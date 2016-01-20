/*c3win95funcs.c */


#include <memory.h>
#include <float.h>
#include "c3win95funcs.h"
#include "c3win95io.h"

typedef union 
{
	unsigned char  by[4];
	unsigned short wr;
	float          fl;
	long           lg;
} UNIONR;

static void SIConvertToWord (char *eebuf, short *outval);
static void SIConvertToFloat (char *eebuf, float *outval);
static void SIConvertToLong (char *eebuf, long *outval);
static short SIMatMult ( double *aptr, short nrow, short pcol, double *bptr, short prow, short ncol, double *outptr);

static C3EEPSTRUCT eepStruct;
static C3INFO c3info;
/*********************************************************************************
 *
 * External references
 *
 *********************************************************************************/
extern char          gTimoFlag;
extern long          gTimoLimit;
extern long          gCalIntTime[3];
extern long          gIntTime;
extern short         gCalType;
extern double        gRGBscaled[3];


/*
 * C3Open:	Initializes communication port and communicates with the calibrator.  The
 *			firmware revision string is returned if successful, else an error code.
 */
long C3Open ( int port, char *cVerString )
{
	long	err;

	if((err=win_InitSerial(port)) != 0)
		return err;

if((err=C3GetVersionString(cVerString)) != 0)
		return err;

	return err;
}

/*
 *	C3Close:	Closes the communication port.
 */
long C3Close ( void )
{
	long	err;

	err=win_CloseSerial();

	return err;
}

/*
 *	C3Initialize:	Reads data stored in the calibrator.  This command should be called
 *					once, after C3Open.
 */
long  C3Initialize( void )
{
  short i;
  unsigned char data[128];
  float tmpFloat;
  long(err);
  double time;
  
  C3GetEEPromByte( (char)0, &(data[0]));	// dummy read to reset EEPROM
 
  for (i=0; i < 128 ; i++)		// read entire EEPROM
  {
    if ( (err = C3GetEEPromByte( (char)i, &(data[i]))) != 0 )
      return(err);
  }

  //  Store the eeprom structure
  SIConvertToLong( (char *)&(data[ EEP_SERIALNUMBER ]), (long *)&(eepStruct.sn));
  for (i=0; i<9; i++)
  {
    SIConvertToFloat( (char *)&(data[ EEP_MATRIX + ( (int)i * sizeof(float)) ]), (float *)&tmpFloat );
    eepStruct.matrix[i] = (double)tmpFloat;
  }
  SIConvertToWord( (char *)&(data[ EEP_MEASUREMENT_RES ]),   (short *)&(eepStruct.resolution ));
  SIConvertToWord( (char *)&(data[ EEP_MONITOR_FREQUENCY ]), (short *)&(eepStruct.freqHz));
  SIConvertToWord( (char *)&(data[ EEP_RED_FIELDCOUNT ]),    (short *)&(eepStruct.Rfield));
  SIConvertToWord( (char *)&(data[ EEP_GREEN_FIELDCOUNT ]),  (short *)&(eepStruct.Gfield));
  SIConvertToWord( (char *)&(data[ EEP_BLUE_FIELDCOUNT ]),   (short *)&(eepStruct.Bfield));
  SIConvertToLong( (char *)&(data[ EEP_CALIBRATION_DATE ]),  (long *)&(eepStruct.calDate));
  SIConvertToWord( (char *)&(data[ EEP_EEPROM_VERSION  ]),   (short *)&(eepStruct.version));

  // test for valid EEPROM data
 // if(eepStruct.version >2) return (-1);
  if(_isnan(eepStruct.matrix[0])) return(-1);

  
  c3info.sensorIntegration[0] =  (long) ((( 1e11 / ( (double)eepStruct.freqHz * (double)eepStruct.resolution )) * (double) eepStruct.Rfield ) +0.5 );
  c3info.sensorIntegration[1] =  (long) ((( 1e11 / ( (double)eepStruct.freqHz * (double)eepStruct.resolution )) * (double) eepStruct.Gfield ) +0.5 );
  c3info.sensorIntegration[2] =  (long) ((( 1e11 / ( (double)eepStruct.freqHz * (double)eepStruct.resolution )) * (double) eepStruct.Bfield ) +0.5 );
  c3info.monitorIntegration[0] =  c3info.sensorIntegration[0];
  c3info.monitorIntegration[1] =  c3info.sensorIntegration[1];
  c3info.monitorIntegration[2] =  c3info.sensorIntegration[2];
  
  time = (double)c3info.monitorIntegration[0] *	  (double)eepStruct.resolution * 1e-9;
  C3SetSensorIntegration(time);
    return(0);
}

/*
 *	C3GetVersionString:	Reads the calibrator's firmware revision string.
 */
long C3GetVersionString( char *data )
{
	char inBuf[256];
	short numBytes;
	long err;
	
	inBuf[0] = 1;
	inBuf[1] = COMMAND_GET_VERSION; 
	
	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err != 0)
		return (err);

	if(numBytes == - 1)
		return (1); // the number of bytes indicates that the return value is wrong
	memcpy(data,inBuf,numBytes);
	data[numBytes] = 0; // null terminate the string

	return (0);
}

/*
 * C3GetRedData:	Instructs the calibrator to take a reading on the Red channel and 
 *					return the data.  The calibrator uses the current integration time
 *					set.  Data is returned when the measurement is complete.
 */
long C3GetRedData( long *data )
{
	char inBuf[256];
	short numBytes;
	long err;
	
	inBuf[0] = 1;
	inBuf[1] = COMMAND_TAKE_RED_MEASUREMENT; 
	

	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err !=0)
		return (err);
	SIConvertToLong( (char *)&(inBuf[0]), (long *)data) ;

	return (0);
}

/*
 * C3GetGreenData:	Instructs the calibrator to take a reading on the Green channel and 
 *					return the data.  The calibrator uses the current integration time
 *					set.  Data is returned when the measurement is complete.
 */
long C3GetGreenData( long *data )
{
	char inBuf[256];
	short numBytes;
	long err;
	
	inBuf[0] = 1;
	inBuf[1] = COMMAND_TAKE_GREEN_MEASUREMENT; 
	
	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err !=0)
		return (err);

	SIConvertToLong( (char *)&(inBuf[0]), (long *)data) ;

	return (0);

}

/*
 * C3GetBlueData:	Instructs the calibrator to take a reading on the Blue channel and 
 *					return the data.  The calibrator uses the current integration time
 *					set.  Data is returned when the measurement is complete.
 */
long C3GetBlueData( long *data )
{
	char inBuf[256];
	short numBytes;
	long err;
	
	inBuf[0] = 1;
	inBuf[1] = COMMAND_TAKE_BLUE_MEASUREMENT; 
	
	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err !=0)
		return (err);

	SIConvertToLong( (char *)&(inBuf[0]), (long *)data) ;

	return (0);
}

/*
 * C3SetSensorIntegration:	This command sets the current integration time which the
 *							calibrator will use for subsequent measurements.
 */
long C3SetSensorIntegration ( double  dataInSecs )
{
	char inBuf[256], *cptr;
	short numBytes;
	long err, data;
	 double dtemp;
	data = (long)(dataInSecs/(double)eepStruct.resolution *1e9);

	cptr = (char *)&data;
	inBuf[0] = 5;
	inBuf[1] = COMMAND_SET_INTEGRATION_TIME;
	inBuf[2] = *(cptr+3);			// byte swap for Intel platform
	inBuf[3] = *(cptr+2);
	inBuf[4] = *(cptr+1);
	inBuf[5] = *cptr;

	err = win_SendGetCMDandData(inBuf, &numBytes);
	c3info.monitorIntegration[0] =  data;
	c3info.monitorIntegration[1] =  data;
	c3info.monitorIntegration[2] =  data;
	
	C3GetSensorIntegration(&dtemp);
	win_SetTimeOut( dataInSecs);

	return (err);
}

/*
 * C3GetSensorIntegration:	This command returns the current integration time currently
 *							set in the calibrator.
 */
long C3GetSensorIntegration( double *dataInSecs )
{
	char inBuf[256];
	short numBytes;
	long err, data;
	
	
	inBuf[0] = 1;
	inBuf[1] = COMMAND_GET_INTEGRATION_TIME;

	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err !=0)
		return (err);

	SIConvertToLong( (char *)&(inBuf[0]), &data) ;

	*dataInSecs = (double)data * (double)eepStruct.resolution * 1e-9;

	return (err);
}

/*
 * C3MeasurePeriod:	This command instructs the pod to measure the period of the target
 *					source.  A return value of zero indicates that the period could not
 *					be measured.
 */
long C3MeasurePeriod ( double *dataInSecs )
{
	char inBuf[256];
	short numBytes;
	long err , data;
	
	
	inBuf[0] = 1;
	inBuf[1] = COMMAND_MEASURE_PERIOD;

	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err !=0)
		return (err);

	SIConvertToLong( (char *)&(inBuf[0]), &data) ;

	*dataInSecs = (double)data *1e-6 ;
	return (err);
}

/*
 * C3GetXYZData:	This routine collects a Red, Green, and Blue measurement from the
 *					calibrator and, using the calibration data for this calibrator,
 *					calculates XYZ values for the measurement.
 */
long C3GetXYZData( double *data)
{
	long lred,lgreen,lblue;
	double dRGB[3];
	long	err;

	if((err=C3GetRedData( &lred )) != 0)
		return err;
	if((err=C3GetGreenData( &lgreen )) != 0)
		return err;
	if((err=C3GetBlueData(&lblue)) != 0)
		return err;

	dRGB[0]  = ((double)lred   * (double)c3info.sensorIntegration[0] / (double)c3info.monitorIntegration[0] + 0.5 );
	dRGB[1]  = ((double)lgreen * (double)c3info.sensorIntegration[1] / (double)c3info.monitorIntegration[1] + 0.5 );
	dRGB[2]  = ((double)lblue  * (double)c3info.sensorIntegration[2] / (double)c3info.monitorIntegration[2] + 0.5 );

	err=SIMatMult(&eepStruct.matrix[0],3,3,&dRGB[0],3,1,data);

	return(err);
}

/*
 * C3GetSerialNumber:	This command reads the serial number stored in the calibrator.
 */
long C3GetSerialNumber ( long *ulSerNo )
{
	long			err;
	short			i, offset;
	unsigned char	data[4];

	for(i=0, offset=EEP_SERIALNUMBER; i<4; i++, offset++)
	{
		if ( (err = C3GetEEPromByte( (char)offset, &(data[i]))) != 0 )
		  return(err);
	}
	SIConvertToLong( (char *)&(data[0]), (long *)ulSerNo);
	
	return 0;
}

long C3GetEEPromByte( char address, unsigned char *data)
{
	char inBuf[256],ctemp;
	short numBytes;
	long err;
	
	inBuf[0] = 2;
	inBuf[1] = COMMAND_READ_EEPROM_BYTE; 
	inBuf[2] = address;

	err = win_SendGetCMDandData(inBuf, &numBytes);

	if(err !=0)
		return (err);

	ctemp = inBuf[1];
	*data = ctemp;
	return (0);
}
/*****************************************************************************
 * -Routine:      
 *
 * -Description:  
 *
 * -Arguments:     
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasurePeriodHiRes ( long *lPeriod )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long  err=SUCCESS;

   if( (gCalType == CHROMA_2) || (gCalType == CHROMA_3) )
      return ERR_FUNCTION_NOT_AVAILABLE;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_MEASURE_PERIOD_HI_RES;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lPeriod);
   
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureRedPeriod ( long *lRedData, unsigned char ucNumPeriods )
 *
 * -Description:  Instructs the calibrator to take a period reading of the Red
 *                channel only.
 *
 * -Arguments:    lRedData   Red data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureRedPeriod ( long *lRedData, unsigned char ucNumPeriods )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;

   if(gCalType != CHROMA_4)
      return ERR_FUNCTION_NOT_AVAILABLE;
         
   if((err=ChromaSetTimeOut (TIMO_ENABLE, CPUTIMO_20S)) != SUCCESS)
      return err;

   outbuf[0] = 2;
   outbuf[1] = COMMAND_TAKE_RED_PERIOD_MEAS;
   outbuf[2] = ucNumPeriods;

   if((err=IOSendGetData((short)NULL, outbuf, 3, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lRedData);
   
   if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
      return err;
         
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureGreenPeriod ( long *lGreenData, unsigned char ucNumPeriods )
 *
 * -Description:  Instructs the calibrator to take a period reading of the Green
 *                channel only.
 *
 * -Arguments:    lGreenData Green data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureGreenPeriod ( long *lGreenData, unsigned char ucNumPeriods )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;
   
   if(gCalType != CHROMA_4)
      return ERR_FUNCTION_NOT_AVAILABLE;
         
   if((err=ChromaSetTimeOut (TIMO_ENABLE, CPUTIMO_20S)) != SUCCESS)
      return err;

   outbuf[0] = 2;
   outbuf[1] = COMMAND_TAKE_GREEN_PERIOD_MEAS;
   outbuf[2] = ucNumPeriods;

   if((err=IOSendGetData((short)NULL, outbuf, 3, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lGreenData);
   
   if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
      return err;
         
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureBluePeriod ( long *lBlueData, unsigned char ucNumPeriods )
 *
 * -Description:  Instructs the calibrator to take a period reading of the 
 *                Blue channel only.
 *
 * -Arguments:    lBlueData  Blue data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureBluePeriod ( long *lBlueData, unsigned char ucNumPeriods )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;
   
   if(gCalType != CHROMA_4)
      return ERR_FUNCTION_NOT_AVAILABLE;
         
   if((err=ChromaSetTimeOut (TIMO_ENABLE, CPUTIMO_20S)) != SUCCESS)
      return err;

   outbuf[0] = 2;
   outbuf[1] = COMMAND_TAKE_BLUE_PERIOD_MEAS;
   outbuf[2] = ucNumPeriods;

   if((err=IOSendGetData((short)NULL, outbuf, 3, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lBlueData);

   if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
      return err;
         
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureRGBPeriod ( long *lRedData, unsigned char ucRNum, unsigned char ucGNum, unsigned char ucBNum )
 *
 * -Description:  Instructs the calibrator to take a period reading of all channels
 *                concurrently.  Red period data is returned.
 *
 * -Arguments:    lRedData   Red period data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureRGBPeriod ( long *lRedData, unsigned char ucRNum, unsigned char ucGNum, unsigned char ucBNum )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;
   
   if(gCalType != CHROMA_4)
      return ERR_FUNCTION_NOT_AVAILABLE;
         
   if((err=ChromaSetTimeOut (TIMO_ENABLE, CPUTIMO_20S)) != SUCCESS)
      return err;

   outbuf[0] = 4;
   outbuf[1] = COMMAND_TAKE_RGB_PERIOD_MEAS;
   outbuf[2] = ucRNum;
   outbuf[3] = ucGNum;
   outbuf[4] = ucBNum;

   if((err=IOSendGetData((short)NULL, outbuf, 5, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lRedData);
   
   if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
      return err;
         
   return err;
}



void SIConvertToWord (char *eebuf, short *outval)
{
	char  *ptr;
	UNIONR r;

	ptr = eebuf;
/* swap the bytes */
	r.by[1] = *ptr++;
	r.by[0] = *ptr;

	*outval = r.wr;
	return;
}

void SIConvertToFloat (char *eebuf, float *outval)
{
	char   *ptr;
	UNIONR r;

	ptr = eebuf;
/* swap the bytes */
	r.by[3] = *ptr++;
	r.by[2] = *ptr++;
	r.by[1] = *ptr++;
	r.by[0] = *ptr;

	*outval = r.fl;
	return;
}

void SIConvertToLong (char *eebuf, long *outval)
{
	char   *ptr;
	UNIONR r;
	
	ptr = eebuf;
/* swap the bytes */
	r.by[3] = *ptr++;
	r.by[2] = *ptr++;
	r.by[1] = *ptr++;
	r.by[0] = *ptr;

	*outval = r.lg;
	return;
}

short  SIMatMult ( double *aptr, short nrow, short pcol, double *bptr, short prow, short ncol, double *outptr)
{
  double *cptr, *dptr, *eptr;
  short i,j,k,ibase;

   if(pcol != prow)
    return(-1); /* The matrices are not order correctly! */
   
   for ( i = 0; i < nrow; i++)
    for( j = 0; j < ncol; j++){
     cptr = outptr + (i * ncol) + j;
     *cptr = 0.0000;
     ibase = i * pcol;

       for (k= 0; k < pcol; k++){
        dptr = aptr + ibase + k;
        eptr = bptr + ( k * ncol) + j;
        *cptr = *cptr + (*dptr) * (*eptr);
       }
     }
   return 0;
}
