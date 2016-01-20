/******************************************************************************
**
**  Project Name: Chroma Toolkit
**     File Name: ChrFuncs.c
**
**   Description: Interface to calibrator routines
**
*******************************************************************************
**                         C O P Y R I G H T
*******************************************************************************
**
**     Copyright (c) 1989-97 Sequel Imaging, Inc., Londonderry NH
**            THIS INFORMATION IS COMPANY CONFIDENTIAL
**
** NOTICE:  This material is a confidential trade secret and proprietary 
** information of Sequel Imaging, Inc. which may not be reproduced, used, 
** sold, or transferred to any third party without the prior written consent 
** of Sequel Imaging.  This material is also copyrighted as an unpublished work 
** under sections 104 and 408 of Title 17 of the United States Code.  
** Unauthorized use, copying, or other unauthorized reproduction of any form
** is prohibited.
**
*******************************************************************************
**                   R E V I S I O N   H I S T O R Y
*******************************************************************************
**
** 16Jan97 0.1 Original
** 18Feb96 Added support for Win16.
** 28Feb97 Added Period measure routines for Chroma4.
** 21Mar97 4.0 Renamed to Chrfuncs.c. Renamed all routines to Chroma...
**  9Apr97 4.0.1 Added source file matrix.c.
** 20Jun97 4.0.3 Added check for Chroma2 Serial (version 15.001.0)
** 25Jun97 Changed MeasureXYZByPeriod to include num of periods parameters.
**
********************************************************************************/
 #include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include "sipcal.h"
#include "chrfuncs.h"
#include "matrix.h"

static long SIParseChromaEEPROM ( void );
static long ChromaDetermineCalibratorType ( char *szVerString );

// globals
long     gCalIntTime[3] = {0L, 0L, 0L};
long     gIntTime;
short    gCalType, gEEPROMState;
double   gMonFreq;
double   gRGBscaled[3];

// EEPROM structure
unsigned long  eeSerialNumber;
double         eeMatrix[9] = {1.0, 0.0, 0.0,
                              0.0, 1.0, 0.0,
                              0.0, 0.0, 1.0};
unsigned short eeMeasResolution;
unsigned short eeCalMonFreq;
unsigned short eeCalRedCount;
unsigned short eeCalGreenCount;
unsigned short eeCalBlueCount;
unsigned long  eeCalDate;
unsigned short eeEEPROMVersion;


/*****************************************************************************
 * -Routine:      ChromaOpen ( short sPort, char *szVerString )
 *
 * -Description:  Initializes communication port and communicates with the
 *                calibrator.
 *
 * -Arguments:    port    which serial port
 *
 *                cVerString  pointer to buffer where calibrator firmware
 *                      string is returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaOpen ( short sPort, char *szVerString, unsigned char *ucOption )
{
   long  err;

   if((err=IOInit(sPort))!=SUCCESS)
      return err;

/*
 * Query for the rom version string
 */
   if ( (err=ChromaGetVersionString(szVerString)) != SUCCESS)
   {
      ChromaClose();   /* close the communications port because of the error */
      return err;
   }
   
   err=ChromaDetermineCalibratorType(szVerString);
   if(err == ERR_LOCKED_CALIBRATOR)
   {
      if(ucOption == NULL)
         return ERR_LOCKED_CALIBRATOR;
      err=ChromaUnlockCalibrator(ucOption);
      
      if ( (err=ChromaGetVersionString(szVerString)) != SUCCESS)
      {
         ChromaClose();   /* close the communications port because of the error */
         return err;
      }
      err=ChromaDetermineCalibratorType(szVerString);
   }
   

   return err;
}

/*****************************************************************************
 * -Routine:      ChromaInitialize ( void )
 *
 * -Description:  Reads calibrator's EEPROM and sets up static data structures.
 *
 * -Arguments:    none
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaInitialize ( void )
{
   long  err=SUCCESS;

   if((err=SIParseChromaEEPROM())!=SUCCESS)
   {
      ChromaClose();
      return(err);
   }

   if(eeEEPROMVersion != 0xffff)
   {
      if(!( (eeCalMonFreq == 0)     ||
            (eeMeasResolution == 0) ||
            (eeCalRedCount == 0)    ||
            (eeCalGreenCount == 0)  ||
            (eeCalBlueCount == 0)) )
      {
         gCalIntTime[0] = (long)(((double)1e11 / ((double)eeCalMonFreq * (double)eeMeasResolution))   * (double)eeCalRedCount + 0.5);
         gCalIntTime[1] = (long)(((double)1e11 / ((double)eeCalMonFreq * (double)eeMeasResolution))   * (double)eeCalGreenCount + 0.5);
         gCalIntTime[2] = (long)(((double)1e11 / ((double)eeCalMonFreq * (double)eeMeasResolution))   * (double)eeCalBlueCount + 0.5);
      }
   }
   else
      gEEPROMState = EEPROM_UNINITIALIZED;

// Set up static gIntTime.
   if((err=ChromaGetIntegrationTime(&gIntTime)) != SUCCESS)
      return err;

   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      long ChromaClose ( void )
 *
 * -Description:  Closes port and disables communication with the calibrator.
 *
 * -Arguments:     
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *             else, error code returned
 *****************************************************************************/
long ChromaClose ( void )
{
   return(IOClose());
}

/*****************************************************************************
 * -Routine:      long ChromaGetVersionString ( char *szVerString )
 *
 * -Description:  Gets the firmware version string from the calibrator.
 *
 * -Arguments:     verstring  pointer to char buffer where string is returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *             else, error code returned
 *****************************************************************************/
long ChromaGetVersionString ( char *szVerString )
{
   char     outbuf[5], inbuf[10], *cptr;
      short    incount, i;
      long  err=SUCCESS;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_GET_VERSION;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   // format version into null-terminated string
   szVerString[0] = inbuf[1];
   szVerString[1] = inbuf[2];
   szVerString[2] = inbuf[3];
   szVerString[3] = inbuf[4];
   szVerString[4] = 0x00;

#if 0
   cptr = szVerString;
   for(i=1; i<incount; i++)   // skip command byte
      *cptr++ = inbuf[i];
   *cptr = 0x00;           // null terminate
#endif

   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureRedChannel ( long *lRedData )
 *
 * -Description:  Instructs the calibrator to take a reading using the current
 *                set integration time on the Red channel and return the result.
 *
 * -Arguments:    lRedData   Red data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureRedChannel ( long *lRedData )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;
   long     oldtimeout;
   
   if((err=ChromaSetMeasurementTimeOut(&oldtimeout)) != SUCCESS)
      return err;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_TAKE_RED_MEASUREMENT;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lRedData);
   
   if(oldtimeout != 0L)
      if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
         return err;
         
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureGreenChannel ( long *lGreenData )
 *
 * -Description:  Instructs the calibrator to take a reading using the current
 *                set integration time on the Green channel and return the result.
 *
 * -Arguments:    lGreenData Green data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureGreenChannel ( long *lGreenData )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;
   long     oldtimeout;
   
   if((err=ChromaSetMeasurementTimeOut(&oldtimeout)) != SUCCESS)
      return err;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_TAKE_GREEN_MEASUREMENT;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lGreenData);
   
   if(oldtimeout != 0L)
      if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
         return err;
         
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureBlueChannel ( long *lBlueData )
 *
 * -Description:  Instructs the calibrator to take a reading using the current
 *                set integration time on the Blue channel and return the result.
 *
 * -Arguments:    lBlueData  Blue data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long ChromaMeasureBlueChannel ( long *lBlueData )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;
   long     oldtimeout;
   
   if((err=ChromaSetMeasurementTimeOut(&oldtimeout)) != SUCCESS)
      return err;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_TAKE_BLUE_MEASUREMENT;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lBlueData);

   if(oldtimeout != 0L)
      if((err=ChromaSetTimeOut (TIMO_ENABLE, TIMO_DEFAULT_LIMIT)) != SUCCESS)
         return err;
         
   return err;
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureRGBScaled ( double *dRGBdata )
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
long ChromaMeasureRGBScaled ( double *dRGBdata )
{
   long     lData[3];
   long     err;

   if(gIntTime == 0L)
   {
      dRGBdata[0] = 0.0;
      dRGBdata[1] = 0.0;
      dRGBdata[2] = 0.0;
      return ERR_ZERO_INT_TIME;
   }
   
   if((err=ChromaMeasureRedChannel(&lData[0])) != SUCCESS)
      return err;

   if((err=ChromaMeasureGreenChannel(&lData[1])) != SUCCESS)
      return err;

   if((err=ChromaMeasureBlueChannel(&lData[2])) != SUCCESS)
      return err;

   
   dRGBdata[0] = (double)lData[0] * ( (double)gCalIntTime[0] / (double)gIntTime );
   dRGBdata[1] = (double)lData[1] * ( (double)gCalIntTime[1] / (double)gIntTime );
   dRGBdata[2] = (double)lData[2] * ( (double)gCalIntTime[2] / (double)gIntTime );

   return SUCCESS;   
}

/*****************************************************************************
 * -Routine:      long ChromaMeasureRGBbyPeriodScaled ( double *dRGBdata, unsigned char ucRNum, unsigned char ucGNum, unsigned char ucBNum )
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
long ChromaMeasureRGBbyPeriodScaled ( double *dRGBdata, unsigned char ucRNum, unsigned char ucGNum, unsigned char ucBNum )
{
   double   dData[3], dCalIntTimeSecs[3];
   long     ltemp;
   long     err;

   if(gIntTime == 0L)
      return ERR_ZERO_INT_TIME;

   // Calculate current and calibration times in seconds.   
   dCalIntTimeSecs[0] = (double)gCalIntTime[0] * (double)eeMeasResolution * 1e-9;
   dCalIntTimeSecs[1] = (double)gCalIntTime[1] * (double)eeMeasResolution * 1e-9;
   dCalIntTimeSecs[2] = (double)gCalIntTime[2] * (double)eeMeasResolution * 1e-9;

   // Measure the period of each channel. 
   if((err=ChromaMeasureRedPeriod(&ltemp, ucRNum)) != SUCCESS)
      return err;
   dData[0] = (double)ltemp;
   
   if((err=ChromaMeasureGreenPeriod(&ltemp, ucGNum)) != SUCCESS)
      return err;
   dData[1] = (double)ltemp;
   
   if((err=ChromaMeasureBluePeriod(&ltemp, ucBNum)) != SUCCESS)
      return err;
   dData[2] = (double)ltemp;

   // Calculate the equivalent raw counts.
   dRGBdata[0] = dCalIntTimeSecs[0] / ((dData[0] / (double)ucRNum) * 0.25e-6);
   dRGBdata[1] = dCalIntTimeSecs[0] / ((dData[1] / (double)ucGNum) * 0.25e-6);
   dRGBdata[2] = dCalIntTimeSecs[0] / ((dData[2] / (double)ucBNum) * 0.25e-6);

   return SUCCESS;   
}

/*****************************************************************************
 * -Routine:      long ChromaAutoMeasureRGBScaled ( double *dRGBdata )
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
long ChromaAutoMeasureRGBScaled ( double *dRGBdata )
{
   long     lData[3], ltemp;
   long     err;
   double   dData[3], dCalIntTimeSecs[3];
   long     saveinttime;
   unsigned char  numper, intmeasflag;

   if(gIntTime == 0L)
   {
      dRGBdata[0] = 0.0;
      dRGBdata[1] = 0.0;
      dRGBdata[2] = 0.0;
      return ERR_ZERO_INT_TIME;
   }
                                         
   // Calculate current and calibration times in seconds.   
   dCalIntTimeSecs[0] = (double)gCalIntTime[0] * (double)eeMeasResolution * 1e-9;
   dCalIntTimeSecs[1] = (double)gCalIntTime[1] * (double)eeMeasResolution * 1e-9;
   dCalIntTimeSecs[2] = (double)gCalIntTime[2] * (double)eeMeasResolution * 1e-9;

   // save the current integration time
   if((err=ChromaGetIntegrationTime(&saveinttime)) != SUCCESS)
      return err;

   // set an int time of one field (hard-code for now)
   if((err=ChromaSetIntegrationTime(14866L)) != SUCCESS)
      return err;

   // take a quick measurement
   if((err=ChromaMeasureRedChannel(&lData[0])) != SUCCESS)
      return err;

   if((err=ChromaMeasureGreenChannel(&lData[1])) != SUCCESS)
      return err;

   if((err=ChromaMeasureBlueChannel(&lData[2])) != SUCCESS)
      return err;

   // reset the int time
   if((err=ChromaSetIntegrationTime(saveinttime)) != SUCCESS)
      return err;

   // use the quick data to determine if a period measurement is needed
   intmeasflag = 0;
   if(lData[0] <= 8L)
   {
      numper = (unsigned char)((double)lData[0] * 10.0 + 0.5);
      if(numper == 0) numper = 5;
      if((err=ChromaMeasureRedPeriod(&ltemp, numper)) != SUCCESS)
         return err;
      dData[0] = (double)ltemp;
      dRGBdata[0] = dCalIntTimeSecs[0] / ((dData[0] / (double)numper) * 0.25e-6);
   }
   else
      intmeasflag |= 0x01;
      
   if(lData[1] <= 8L)
   {
      numper = (unsigned char)((double)lData[0] * 10.0 + 0.5);
      if(numper == 0) numper = 5;
      if((err=ChromaMeasureGreenPeriod(&ltemp, numper)) != SUCCESS)
         return err;
      dData[1] = (double)ltemp;
      dRGBdata[1] = dCalIntTimeSecs[1] / ((dData[1] / (double)numper) * 0.25e-6);
   }
   else
      intmeasflag |= 0x02;
      
   if(lData[2] <= 8L)
   {
      numper = (unsigned char)((double)lData[0] * 10.0 + 0.5);
      if(numper == 0) numper = 5;
      if((err=ChromaMeasureBluePeriod(&ltemp, numper)) != SUCCESS)
         return err;
      dData[2] = (double)ltemp;
      dRGBdata[2] = dCalIntTimeSecs[2] / ((dData[2] / (double)numper) * 0.25e-6);
   }
   else
      intmeasflag |= 0x04;

   // check the flag to see if an integration measurement is needed
   if(intmeasflag)
   {
      if((err=ChromaMeasureRedChannel(&lData[0])) != SUCCESS)
         return err;
   
      if((err=ChromaMeasureGreenChannel(&lData[1])) != SUCCESS)
         return err;
   
      if((err=ChromaMeasureBlueChannel(&lData[2])) != SUCCESS)
         return err;
   
      if(intmeasflag & 0x01)
         dRGBdata[0] = (double)lData[0] * ( (double)gCalIntTime[0] / (double)gIntTime );
      if(intmeasflag & 0x02)
         dRGBdata[1] = (double)lData[1] * ( (double)gCalIntTime[1] / (double)gIntTime );
      if(intmeasflag & 0x04)
         dRGBdata[2] = (double)lData[2] * ( (double)gCalIntTime[2] / (double)gIntTime );
   }

   return SUCCESS;   
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
long ChromaMeasureXYZ ( double *XYZdata )
{
   double   dData[3];
   long     err;
   
   if((err=ChromaMeasureRGBScaled(dData)) != SUCCESS)
      return err;

   // copy the scaled RGB data to the global variable
   gRGBscaled[0] = dData[0];
   gRGBscaled[1] = dData[1];
   gRGBscaled[2] = dData[2];
   
   if(MatMult(&eeMatrix[0], 3, 3, &dData[0], 3, 1, XYZdata))
      return ERR_INVALID_MATRICES_ORDER;

   return SUCCESS;
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
long ChromaMeasureXYZbyPeriod ( double *XYZdata, unsigned char ucRNum, unsigned char ucGNum, unsigned char ucBNum )
{
   double   dData[3];
   long     err;

   if((err=ChromaMeasureRGBbyPeriodScaled(dData, ucRNum, ucGNum, ucBNum)) != SUCCESS)
      return err;

   // copy the scaled RGB data to the global variable
   gRGBscaled[0] = dData[0];
   gRGBscaled[1] = dData[1];
   gRGBscaled[2] = dData[2];
   
   if(MatMult(&eeMatrix[0], 3, 3, &dData[0], 3, 1, XYZdata))
      return ERR_INVALID_MATRICES_ORDER;

   return SUCCESS;
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
long ChromaAutoMeasureXYZ ( dXYZ *dXYZdata, short *sper )
{
   double         dData[3];                        
   long           err;
   double         dXYZtemp[3];
   //dXYZ           dXYZdata;
   unsigned char  numpermeas;


   if((err=ChromaMeasureRGBScaled(dData)) != SUCCESS)
      return err;

   // copy the scaled RGB data to the global variable
   gRGBscaled[0] = dData[0];
   gRGBscaled[1] = dData[1];
   gRGBscaled[2] = dData[2];
   
   if(MatMult(&eeMatrix[0], 3, 3, &dData[0], 3, 1, dXYZtemp))
      return ERR_INVALID_MATRICES_ORDER;

   if(dXYZtemp[1] > 1.0)
   {
      dXYZdata->X = dXYZtemp[0];
      dXYZdata->Y = dXYZtemp[1];
      dXYZdata->Z = dXYZtemp[2];

      *sper = 0;
      
      return SUCCESS;
   }

   else if(dXYZtemp[1] >= 0.5)
      numpermeas = 10;
   else if(dXYZtemp[1] >= 0.2)
      numpermeas = 5;
   else if(dXYZtemp[1] >= 0.05)
      numpermeas = 3;
   else
      numpermeas = 1;

   *sper = (short)numpermeas;

   if((err=ChromaMeasureRGBbyPeriodScaled(dData, numpermeas, numpermeas, numpermeas)) != SUCCESS)
      return err;

   // copy the scaled RGB data to the global variable
   gRGBscaled[0] = dData[0];
   gRGBscaled[1] = dData[1];
   gRGBscaled[2] = dData[2];
   
   if(MatMult(&eeMatrix[0], 3, 3, &dData[0], 3, 1, dXYZtemp))
      return ERR_INVALID_MATRICES_ORDER;
   
   dXYZdata->X = dXYZtemp[0];
   dXYZdata->Y = dXYZtemp[1];
   dXYZdata->Z = dXYZtemp[2];

   return SUCCESS;
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
long ChromaSetIntegrationTime ( long lIntTime )
{
   char           outbuf[5], inbuf[10];
   short          incount;
   long           err=SUCCESS;
   unsigned char  *ucptr;

   outbuf[0] = 5;
   outbuf[1] = COMMAND_SET_INTEGRATION_TIME;

   ucptr = (unsigned char *)&lIntTime;
#if (defined MAC_KIT)
   outbuf[2] = *(ucptr+0);
   outbuf[3] = *(ucptr+1);
   outbuf[4] = *(ucptr+2);
   outbuf[5] = *(ucptr+3);
#endif
#if (defined IBMPC_KIT | WIN16_KIT)
   outbuf[2] = *(ucptr+3);
   outbuf[3] = *(ucptr+2);
   outbuf[4] = *(ucptr+1);
   outbuf[5] = *(ucptr+0);
#endif

   if((err=IOSendGetData((short)NULL, outbuf, 6, inbuf, &incount)) != SUCCESS)
      return err;

   if((err=ChromaGetIntegrationTime(&gIntTime)) != SUCCESS)
      return err;

   return err;

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
long ChromaGetIntegrationTime ( long *lIntTime )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long     err=SUCCESS;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_GET_INTEGRATION_TIME;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lIntTime);
   
   return err;
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
long ChromaMeasurePeriod ( long *lPeriod )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long  err=SUCCESS;

   if(gCalType == CHROMA_2)
      return ERR_FUNCTION_NOT_AVAILABLE;

   outbuf[0] = 1;
   outbuf[1] = COMMAND_MEASURE_PERIOD;

   if((err=IOSendGetData((short)NULL, outbuf, 2, inbuf, &incount )) != SUCCESS)
      return err;

   if(err==SUCCESS)
      SIConvertToLong(&inbuf[1], lPeriod);
   
   return err;
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
long ChromaReadEEPROMByte ( unsigned char address, unsigned char *data )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long  err=SUCCESS;

   outbuf[0] = 2;
   outbuf[1] = COMMAND_READ_EEPROM_BYTE;
   outbuf[2] = address;
   if((err=IOSendGetData((short)NULL, outbuf, 3, inbuf, &incount)) != SUCCESS)
      return err;
   *data = inbuf[2];
   return err;
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
long ChromaWriteEEPROMByte ( unsigned char address, unsigned char data )
{
   char     outbuf[5], inbuf[10];
   short    incount;
   long  err=SUCCESS;
   unsigned char  uctemp;

   if((err=ChromaReadEEPROMByte(address, &uctemp)) != SUCCESS)
      return err;
   if(uctemp == data)
      return SUCCESS;

   outbuf[0] = 3;
   outbuf[1] = COMMAND_WRITE_EEPROM_BYTE;
   outbuf[2] = address;
   outbuf[3] = data;
   if((err=IOSendGetData((short)NULL, outbuf, 4, inbuf, &incount)) != SUCCESS)
      return err;
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
long ChromaUnlockCalibrator ( unsigned char *ucCode )
{
   char           outbuf[5], inbuf[10];
   short          incount;
   long           err=SUCCESS;

   outbuf[0] = 5;
   outbuf[1] = COMMAND_UNLOCK_CALIBRATOR;
   outbuf[2] = *(ucCode);
   outbuf[3] = *(ucCode+1);
   outbuf[4] = *(ucCode+2);
   outbuf[5] = *(ucCode+3);

   if((err=IOSendGetData((short)NULL, outbuf, 6, inbuf, &incount)) != SUCCESS)
      return err;

   return err;
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
long SIParseChromaEEPROM ( void )
{
   short          i, j;
   unsigned char  uc[4], eeaddress;
   float          ftemp;
   long           err;

// Reset the EEPROM by doing some reads (necessary for Chroma2)
   eeaddress = EE_SERIAL_NUMBER;
   for(i=0; i<4; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
         
// Serial Number
   eeaddress = EE_SERIAL_NUMBER;
   for(i=0; i<4; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToLong((char *)uc, (long *)&eeSerialNumber);

// Matrix
   eeaddress = EE_MATRIX;
   for(i=0; i<9; i++)
   {
      for(j=0; j<4; j++)
         if((err=ChromaReadEEPROMByte(eeaddress++, &uc[j])) != SUCCESS)
            return err;
      SIConvertToFloat((char *)uc, &ftemp);
      eeMatrix[i] = (double)ftemp;
   }

// Resolution
   eeaddress = EE_RESOLUTION;
   for(i=0; i<2; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToWord((char *)uc, (short *)&eeMeasResolution);

// Calibration monitor frequency
   eeaddress = EE_CAL_MON_FREQ;
   for(i=0; i<2; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToWord((char *)uc, (short *)&eeCalMonFreq);

// Calibration Red count
   eeaddress = EE_CAL_RED_COUNT;
   for(i=0; i<2; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToWord((char *)uc, (short *)&eeCalRedCount);

// Calibration Green count
   eeaddress = EE_CAL_GREEN_COUNT;
   for(i=0; i<2; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToWord((char *)uc,(short *) &eeCalGreenCount);

// Calibration Blue count
   eeaddress = EE_CAL_BLUE_COUNT;
   for(i=0; i<2; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToWord((char *)uc, (short *)&eeCalBlueCount);

// Calibration Date
   eeaddress = EE_CALIBRATION_DATE;
   for(i=0; i<4; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToLong((char *)uc,(long *) &eeCalDate);

// EEPROM Version
   eeaddress = EE_EEPROM_VERSION;
   for(i=0; i<2; i++)
      if((err=ChromaReadEEPROMByte(eeaddress++, &uc[i])) != SUCCESS)
         return err;
   SIConvertToWord((char *)uc,(short *) &eeEEPROMVersion);
   
   return SUCCESS;
}

/*****************************************************************************
 * -Name:         SIConvertToWord (char *eebuf, short *outval)
 *                FOR INTERNAL TOOLKIT USE ONLY!!
 *   
 * -Description:  This routine converts 2 successive bytes read from the
 *                GIF eeprom into the destination machine word (16-bit)
 *                format.
 *
 * -Arguments:    char *eebuf   - buffer containing 2 GIF bytes
 *                short *outval - returned as convert machine 16-bit word
 *
 * -Returns:      None
 *
 * -Notes:        
 *                FOR INTERNAL TOOLKIT USE ONLY!!
 *****************************************************************************/
void SIConvertToWord (char *eebuf, short *outval)
{
   char  *ptr;
    union {
        unsigned char by[2];
        short wr;
    } r;

    ptr = eebuf;
#ifndef MAC_KIT
/* convert to Intel format */
    r.by[1] = *ptr++;
    r.by[0] = *ptr;
#else
/* straight copy for Motorola format */
    r.by[0] = *ptr++;
    r.by[1] = *ptr;
#endif

    *outval = r.wr;
    return;
}

/***********************************************************************
 * -Name:         SIConvertToLong (char *eebuf, long *outval)
 *                FOR INTERNAL TOOLKIT USE ONLY!!
 *   
 * -Description:  This routine converts 4 successive bytes read from the
 *                GIF eeprom into the destination machine long (4-byte)
 *                format.
 *
 * -Arguments:    char *eebuf  - buffer containing 4 GIF bytes
 *                long *outval - returned as convert machine long
 *
 * -Returns:      None
 *
 * -Notes:        FOR INTERNAL TOOLKIT USE ONLY!!
 ************************************************************************/
void SIConvertToLong (char *eebuf, long *outval)
{
   char   *ptr;
   union {
      unsigned char  by[4];
      unsigned short wr;
      float          fl;
      long           lg;
   } r;

   ptr = eebuf;
#if (defined MAC_KIT)
/* straight copy, do not swap bytes*/
   r.by[0] = *ptr++;
   r.by[1] = *ptr++;
   r.by[2] = *ptr++;
   r.by[3] = *ptr;
#endif
#if (defined IBMPC_KIT | WIN16_KIT)
/* swap the bytes */
   r.by[3] = *ptr++;
   r.by[2] = *ptr++;
   r.by[1] = *ptr++;
   r.by[0] = *ptr;
#endif

   *outval = r.lg;
   return;
}

/*****************************************************************************
 * -Name:         SIConvertToFloat (char *eebuf, float *outval)
 *                FOR INTERNAL TOOLKIT USE ONLY!!
 *   
 * -Description:  This routine converts 4 successive bytes read from the
 *                GIF eeprom into the destination machine float (4-byte)
 *                format.
 *
 * -Arguments:    char *eebuf   - buffer containing 4 GIF bytes
 *                float *outval - returned as convert machine float
 *
 * -Returns:      None
 *
 * -Notes:        FOR INTERNAL TOOLKIT USE ONLY!!
 *****************************************************************************/
void SIConvertToFloat (char *eebuf, float *outval)
{
    char  *ptr;
    union {
      unsigned char by[4];
        float fl;
    } r;

    ptr = eebuf;
#if (defined IBMPC_KIT | WIN16_KIT)
/* convert to Intel format */
   r.by[3] = *ptr++;
   r.by[2] = *ptr++;
   r.by[1] = *ptr++;
   r.by[0] = *ptr;
#else
/* straight copy for Motorola format */
   r.by[0] = *ptr++;
   r.by[1] = *ptr++;
   r.by[2] = *ptr++;
   r.by[3] = *ptr;
#endif

   *outval = r.fl;
   return;
}

/*****************************************************************************
 * -Name:        ChromaSetTimeOut ( char, long )
 *
 * -Description: This routine enables/disables and sets the host timeout.
 *               If enabled, the timeout limit can be be set.
 *              
 * -Arguments:   char timeoutflag - 0 disables timeout
 *                                  1 enables timeout and sets timeout limit
 *                                    to second argument.
 *               long timeoutlimit - timeout limit, if timeout is enabled
 *
 * -Note:        None
 *
 * -Returns:     SUCCESS,
 *               else, timeout occurred and returned
 *               ERR_INVALID_TIMOFLAG invalid timeout flag
 *               ERR_INVALID_TIMEOUT  invalid timeout (count)
 *****************************************************************************/
long ChromaSetTimeOut ( char timeoutflag, long timeoutlimit )
{
   if ( (timeoutflag != TIMO_ENABLE) && (timeoutflag != TIMO_DISABLE) ) {
      return ERR_INVALID_TIMOFLAG;
   }
   if ( timeoutlimit < 0L) {
      return ERR_INVALID_TIMEOUT;
   }

   gTimoFlag = timeoutflag;
   if (gTimoFlag == TIMO_ENABLE) {  /* are timeouts enabled ??  */
      gTimoLimit = timeoutlimit;   /* set timeouts to argument */
   }
   return SUCCESS;
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
 *             else, error code returned
 *****************************************************************************/
long ChromaSetMeasurementTimeOut ( long *lOldTimeoutLimit )
{
   long  err, timeouttime;
   
   timeouttime = (long)((double)gIntTime * (eeMeasResolution * 1e-9) + 0.5);

   if((gTimoFlag == TIMO_ENABLE) && (timeouttime > gTimoLimit-2))
   {
      *lOldTimeoutLimit = gTimoLimit;
      if((err=ChromaSetTimeOut (TIMO_ENABLE, timeouttime+2)) != SUCCESS)
         return err;
   }
   else
   {
      *lOldTimeoutLimit = 0L;
   }
   return SUCCESS;
}
                    

/*****************************************************************************
 * -Name:        ChromaDetermineCalibratorType ( short *sCalType, char *szVerString )
 *
 * -Description: This routine examines the Calibrator's version string and
 *               sets a global variable defining what version it is.
 *              
 * -Arguments:   short *sCalType    - pointer to global variable where calibrator
 *                                  type returned
 *               char *szVerString  - version string
 *
 * -Note:        None
 *
 * -Returns:     SUCCESS, sCalType set,
 *               ERR_UNSUPPORTED_CALIBRATOR if unknown version string
 *****************************************************************************/
static long ChromaDetermineCalibratorType ( char *szVerString )
{
   char  cTemp[5];

   strncpy(cTemp, szVerString, 4);
   cTemp[4] = 0;
   if(!strcmp(cTemp, "LOCK"))
   {
      gCalType = CHROMA_LOCKED;
      return ERR_LOCKED_CALIBRATOR;
   }
   cTemp[2] = 0;     // null-terminate the string
   if     (!strcmp(cTemp, "14"))   gCalType = CHROMA_2;
   else if(!strcmp(cTemp, "15"))   gCalType = CHROMA_2;
   else if(!strcmp(cTemp, "2."))   gCalType = CHROMA_3;
   else if(!strcmp(cTemp, "3."))   gCalType = CHROMA_4;
   else if(!strcmp(cTemp, "4."))   gCalType = CHROMA_4;
   else
   {
      gCalType = CHROMA_UNKNOWN;
      return ERR_UNKNOWN_CALIBRATOR;
   }
      
   return SUCCESS;
}

/*****************************************************************************
 * -Name:        ChromaDetermineEquivalentRGBValues ( dXYZ dXYZsource, dRGB *dRGBresult )
 *
 * -Description: Returns equivalent RGB values for a given XYZ value.
 *              
 * -Arguments:   dXYZ dXYZSource    given source
 *               dRGB *dRGBResult   pointer to RGB result
 *
 * -Notes:       Y is in Foot-Lamberts.
 *
 * -Returns:     SUCCESS
 *               
 *****************************************************************************/
long ChromaDetermineEquivalentRGBValues ( dXYZ dXYZsource, dRGB *dRGBresult )
{
   short          i;
   double         InvertedMatrix[9];
   double         RGBtemp[3];
   
   for(i=0; i<9; i++)                     // get a copy of the pod's matrix
      InvertedMatrix[i] = eeMatrix[i];   
   
   Invert(InvertedMatrix, 3);             // invert it

   if(MatMult(InvertedMatrix, 3, 3, (double *)&dXYZsource, 3, 1, RGBtemp))   // multiply XYZ through inverted matrix to get RGB
      return ERR_INVALID_MATRICES_ORDER;

   dRGBresult->R = RGBtemp[0];
   dRGBresult->G = RGBtemp[1];
   dRGBresult->B = RGBtemp[2];

   return SUCCESS;
}
