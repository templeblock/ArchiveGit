/******************************************************************************
**
**  Project Name: SIPCAL
**     File Name: SIPCAL.C
**
**   Description: Developer routines for Chroma3 Calibrator
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
** 18Feb97 Added Win16 support; added new routines to get serial number and calibration
**          date.  Added luminance units selection to AutoMeasure and MeasureYxy.
** 21Mar97 4.0
**
******************************************************************************/

#include "stdafx.h"


#include <stdio.h>
#include <time.h>
#include "sipcal.h"
#include "chrfuncs.h"



/*****************************************************************************
 * -Routine:      sipOpenCalibrator ( short sPort, char *szVersion )
 *
 * -Description:  Opens communication port and reads calibration data from
 *                the calibrator.
 *
 * -Arguments:    short sPort       which serial port calibrator is attached to
 *
 *                char *szVersion   calibrator version string returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipOpenCalibrator ( short sPort, char *szVersion, unsigned char *ucOption )
{
   long  err;
                                                       
   if((err=ChromaOpen(sPort, szVersion, ucOption)) != SUCCESS)
      return err;

   if((err=ChromaInitialize()) != SUCCESS)
      return err;

   return SUCCESS;
}


/*****************************************************************************
 * -Routine:      sipCloseCalibrator ( void )
 *
 * -Description:  Closes communication port.
 *
 * -Arguments:    none
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipCloseCalibrator ( void )
{
   ChromaClose();
   
   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      sipAutoMeasureYxy ( short Units, dYxy *YxyData, double *dMonFreq )
 *
 * -Description:  Stand-alone call to take a measurement.
 *
 * -Arguments:    short Units       units in which Luminance value (Y) is returned
 *                YxyData Yxy       data returned
 *                double *dMonFreq  monitor frequency returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipAutoMeasureYxy ( short Units, dYxy *YxyData, short *sPeriods )
{
   long     err=SUCCESS;
   double   dDenom;
   dXYZ     dXYZmeas;

// measure frequency and set integration time
//   if((err=sipMeasMonFreqAndSetIntTime(100, dMonFreq)) != SUCCESS)
//      return err;

// if not enough signal to measure frequency, set a long int time
//   if(*dMonFreq == 0.0)
//      if((err=sipSetIntegrationTime(2.0)) != SUCCESS)
//         return err;
   
// take data

   if((err=ChromaAutoMeasureXYZ ( &dXYZmeas, sPeriods)) != SUCCESS)
      return err;

   YxyData->Y = dXYZmeas.Y;
   
   dDenom = dXYZmeas.X + dXYZmeas.Y + dXYZmeas.Z;
   if(dDenom == 0.0)
      return ERR_DIVIDE_BY_ZERO;
      
   YxyData->x = dXYZmeas.X / dDenom;
   YxyData->y = dXYZmeas.Y / dDenom;
   YxyData->z = dXYZmeas.Z / dDenom;

   if(Units == CANDELAS)
      YxyData->Y *= CANDELA_FACTOR;
   
   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      sipMeasureXYZ ( dXYZ *XYZData )
 *
 * -Description:  Instructs the calibrator to take a reading.  Data is returned
 *                in XYZ units.
 *
 * -Arguments:    dXYZ *XYZData  XYZ data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipMeasureXYZ ( dXYZ *XYZData )
{
   long     err=SUCCESS;
   double   dXYZtemp[3];

   if((err=ChromaMeasureXYZ ( dXYZtemp )) != SUCCESS)
      return err;

   XYZData->X = dXYZtemp[0];
   XYZData->Y = dXYZtemp[1];
   XYZData->Z = dXYZtemp[2];

   return err;
}

/*****************************************************************************
 * -Routine:      sipMeasureYxy ( short Units, dYxy *YxyData )
 *
 * -Description:  Instructs the calibrator to take a reading.  Data is returned
 *                in Yxy units.
 *
 * -Arguments:    short Units    units of Y luminance returned
 *                *YxyData Yxy   data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipMeasureYxy ( short Units, dYxy *YxyData )
{
   long     err=SUCCESS;
   double   dXYZtemp[3], dDenom;
   
   if((err=ChromaMeasureXYZ ( dXYZtemp )) != SUCCESS)
      return err;

   YxyData->Y = dXYZtemp[1];

   dDenom = dXYZtemp[0] + dXYZtemp[1] + dXYZtemp[2];
   if(dDenom == 0.0)
      return ERR_DIVIDE_BY_ZERO;

   YxyData->x = dXYZtemp[0] / dDenom;
   YxyData->y = dXYZtemp[1] / dDenom;
   YxyData->z = dXYZtemp[2] / dDenom;

   if(Units == CANDELAS)
      YxyData->Y *= CANDELA_FACTOR;

   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      sipMeasureRGB ( dRGB *RGBData )
 *
 * -Description:  Instructs the calibrator to take a reading.  Data is returned
 *                in RGB raw counts.
 *
 * -Arguments:    dRGB *RGBData  raw RGB data returned
 *
 * -Notes:     
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipMeasureRGB ( dRGB *RGBData )
{
   long     err=SUCCESS;
   double   dRGBtemp[3];
   
   if((err=ChromaMeasureRGBScaled ( dRGBtemp )) != SUCCESS)
      return err;

   RGBData->R = dRGBtemp[0];
   RGBData->G = dRGBtemp[1];
   RGBData->B = dRGBtemp[2];

   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      sipMeasMonFreqAndSetIntTime ( short sNumFields, double *dMonFreq )
 *
 * -Description:  Instructs the calibrator to measure the vertical refresh
 *                rate of the monitor to be measured and set the integration
 *                time.
 *
 * -Arguments:    short sNumFields  number of vertical fields to measure for 
 *                                  subsequent measurements
 *
 *                double *dMonFreq  vertical frequency of monitor returned
 *
 * -Notes:        Integration time is for each channel: red, green and blue.
 *                The total measurement time is at least three times the 
 *                integration time set.  Once set, the integration time is 
 *                valid for each subsequent measurement, until a new value
 *                is set, or the calibrator is turned off (sipCloseCalibrator
 *                called).
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipMeasMonFreqAndSetIntTime ( short sNumFields, double *dMonFreq )
{
   long     err;
   long     lPeriodTime;
   double   dIntTime, dPeriodSum;
   short    i;

#define NUM_FREQ_MEAS_TO_AVG  10

   dPeriodSum = 0.0;
   for(i=0; i<NUM_FREQ_MEAS_TO_AVG; i++)                                         // get average of NUM_FREQ_MEAS_TO_AVG period measurements
   {
      if(gCalType == CHROMA_4)
      {
         if((err=ChromaMeasurePeriodHiRes(&lPeriodTime)) != SUCCESS)
            return err;
      }
      else
      {
         if((err=ChromaMeasurePeriod(&lPeriodTime)) != SUCCESS)
            return err;
      }
      
      if(lPeriodTime == 0L)
      {
         *dMonFreq = 0.0;
         return ERR_FF_COULD_NOT_SYNC;
      }
      dPeriodSum += (double)lPeriodTime;
   }
   if(gCalType == CHROMA_4)                                    // convert hi res time to useconds
      dPeriodSum /= 4.0;

   dPeriodSum /= (double)NUM_FREQ_MEAS_TO_AVG;                 // find average
   
   dIntTime = dPeriodSum / ((double)eeMeasResolution * 0.001); // solve for int time for one field
   *dMonFreq = (double)1.0 / (dPeriodSum * 1e-6);              // calculate monitor freq for return only
   lPeriodTime = (long)((dIntTime * sNumFields) + 0.5);        // multiply int time by number of fields to measure
   if((err=ChromaSetIntegrationTime(lPeriodTime)) != SUCCESS)      // set the new int time
      return err;

   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      sipSetIntegrationTime ( double dSeconds )
 *
 * -Description:  Sets the amount of time which the calibrator integrates
 *                a measurement for each channel.
 *
 * -Arguments:    double dSeconds   time in seconds
 *
 * -Notes:        Integration time is for each channel: red, green and blue.
 *                The total measurement time is at least three times the 
 *                integration time set.  Once set, the integration time is 
 *                valid for each subsequent measurement, until a new value
 *                is set, or the calibrator is turned off (sipCloseCalibrator
 *                called).
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipSetIntegrationTime ( double dSeconds )
{
   long     err=SUCCESS;
   long     lITime;

   lITime = (long)((dSeconds / ((double)eeMeasResolution * 1e-9)) + 0.5);
   if((err=ChromaSetIntegrationTime(lITime)) != SUCCESS)
      return err;
      
   return SUCCESS;
}

/*****************************************************************************
 * -Routine:      sipGetIntegrationTime ( double *dSeconds )
 *
 * -Description:  Returns the current integration time set in the calibrator.
 *
 * -Arguments:    double *dSeconds  time in seconds returned
 *
 * -Notes:        Integration time is for each channel.  The total measurement
 *                time is at least three times the integration time set.
 *
 * -Returns:      0 if SUCCESS
 *                else, error code returned
 *****************************************************************************/
long sipGetIntegrationTime ( double *dSeconds )
{
   long     err=SUCCESS;
   long     lITime;

   if((err=ChromaGetIntegrationTime(&lITime)) != SUCCESS)
      return err;
      
   *dSeconds = (double)lITime * ((double)eeMeasResolution * 1e-9);
      
   return SUCCESS;
}

/***********************************************************************
 * -Name:         sipGetToolkitRev ( void )
 *   
 * -Description:  Returns the software toolkit revision.
 *
 * -Arguments:    None
 *
 * -Returns:      Pointer to null-terminated ASCII string.
 *
 * -Notes:        None
 ************************************************************************/
unsigned char *sipGetToolkitRev ( void )
{
    return (IOgetttoolkitrev());
}

/***********************************************************************
 * -Name:         sipGetCalibratorSerialNumber ( void )
 *   
 * -Description:  Returns the calibrator's serial number.
 *
 * -Arguments:    None
 *
 * -Returns:      Unsigned long serial number.
 *
 * -Notes:        None
 ************************************************************************/
unsigned long sipGetCalibratorSerialNumber ( void )
{
   return (eeSerialNumber);
}

/***********************************************************************
 * -Name:         sipGetCalibrationDate ( void )
 *   
 * -Description:  Returns the software toolkit revision.
 *
 * -Arguments:    None
 *
 * -Returns:      Pointer to null-terminated ASCII string.
 *
 * -Notes:        None
 ************************************************************************/
unsigned char *sipGetCalibrationDate ( void )
{
   return ((unsigned char *)ctime((time_t *)&eeCalDate));
}

/***********************************************************************
 * -Name:         sipGetCalibratorType ( void )
 *   
 * -Description:  Returns the calibrator type.
 *
 * -Arguments:    None
 *
 * -Returns:      Short calibrator type.
 *
 * -Notes:        None
 ************************************************************************/
short sipGetCalibratorType ( void )
{
   return (gCalType);
}

/***********************************************************************
 * -Name:         sipGetRGBvaluesFromYxy ( short sUnits, dXYZ dXYZSource, dRGB *dRGBResult )
 *   
 * -Description:  Finds the scaled RGB values which will convert to the given
 *                colorimetric data.
 *
 * -Arguments:    
 *
 * -Returns:      
 *
 * -Notes:        
 ************************************************************************/
long sipGetRGBvaluesFromYxy ( short sUnits, dYxy dYxySource, dRGB *dRGBResult )
{
   long     err=SUCCESS;
   dXYZ     dXYZtemp;
   
   if(sUnits == CANDELAS)
      dXYZtemp.Y = dYxySource.Y / (double)CANDELA_FACTOR;
   else
      dXYZtemp.Y = dYxySource.Y;

   dXYZtemp.X = (dYxySource.x * dXYZtemp.Y) / dYxySource.y;
   dXYZtemp.Z = ((1.0 - dYxySource.x - dYxySource.y) * dXYZtemp.Y) / dYxySource.y;
   
   err=ChromaDetermineEquivalentRGBValues(dXYZtemp, dRGBResult);
   
   return err;
}

/***********************************************************************
 * -Name:         void sipGetLastRGBscaledValues ( dRGB *dRGBResult )
 *   
 * -Description:  
 *
 * -Arguments:    
 *
 * -Returns:      
 *
 * -Notes:        None
 ************************************************************************/
void sipGetLastRGBscaledValues ( dRGB *dRGBResult )
{
   dRGBResult->R = gRGBscaled[0];
   dRGBResult->G = gRGBscaled[1];
   dRGBResult->B = gRGBscaled[2];
}

/******************************************************************************
**                         F O O T E R
*******************************************************************************
**
**     Copyright (c) 1989-97 Sequel Imaging, Inc., Londonderry NH
**            THIS INFORMATION IS COMPANY CONFIDENTIAL
******************************************************************************/
