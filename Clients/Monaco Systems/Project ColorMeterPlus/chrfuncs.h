/******************************************************************************
**
**  Project Name: SIPCAL
**     File Name: ChromaFUNCS.H
**
**   Description: Header file for Chromafuncs.c
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
** 18Feb97 Added Win16 support.
** 21Mar97 4.0 Renamed to Chrfuncs.h.
**
******************************************************************************/
#ifndef _Chromafuncs_h_
#define _Chromafuncs_h_

#include "sipcal.h"

#define IBMPC_KIT
#undef MAC_KIT

//
// Comm
//
#define BAUD_BITS   0x00e0   /* baud bits valid in comm settings bitmask   */
#define PARITY_BITS 0x0018   /* parity bits valid in comm settings bitmask */
#define STOP_BITS   0x0004   /* stop bits valid in comm settings bitmask   */
#define DATA_BITS   0x0003   /* data bits valid in comm settings bitmask   */

#define BAUD_300    0x0040
#define BAUD_600    0x0060
#define BAUD_1200   0x0080
#define BAUD_2400   0x00a0
#define BAUD_4800   0x00c0
#define BAUD_9600   0x00e0
   
#define NONE        0x0000
#define ODD         0x0008
#define EVEN        0x0018
   
#define COMM_ONESTOPBIT  0x0000
#define COMM_TWOSTOPBITS 0x0004
   
#define SEVENBITS   0x0002
#define EIGHTBITS   0x0003

#define PORT_INVALID           -1

#define MAX_COMBUFFER 1000
#define NEAR_FULL      900

#ifndef  PORT_COM1
#define  PORT_COM1   0
#endif

#ifndef  PORT_COM2
#define  PORT_COM2   1
#endif

#ifndef  PORT_COM3
#define  PORT_COM3   2
#endif

#ifndef  PORT_COM4
#define  PORT_COM4   3
#endif

#ifndef  PORT_COM5
#define  PORT_COM5   4
#endif

#ifndef  PORT_COM6
#define  PORT_COM6   5
#endif

#ifndef  PORT_COM7
#define  PORT_COM7   6
#endif

#ifndef  PORT_COM8
#define  PORT_COM8   7
#endif


#ifndef SUCCESS
#define SUCCESS                     0
#endif

#ifndef FAILURE
#define FAILURE                     -1
#endif

#ifndef NULL
#define NULL   0
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif


//
// Chroma commands
//
#define COMMAND_GET_VERSION               0x00
#define COMMAND_TAKE_RED_MEASUREMENT      0x01
#define COMMAND_TAKE_GREEN_MEASUREMENT    0x02
#define COMMAND_TAKE_BLUE_MEASUREMENT     0x03
#define COMMAND_MEASURE_PERIOD            0x04
#define COMMAND_SET_INTEGRATION_TIME      0x05
#define COMMAND_GET_INTEGRATION_TIME      0x06
#define COMMAND_WRITE_EEPROM_BYTE         0x07
#define COMMAND_READ_EEPROM_BYTE          0x08
#define COMMAND_MEASURE_PERIOD_HI_RES     0x09
#define COMMAND_TAKE_RED_PERIOD_MEAS      0x0a
#define COMMAND_TAKE_GREEN_PERIOD_MEAS    0x0b
#define COMMAND_TAKE_BLUE_PERIOD_MEAS     0x0c
#define COMMAND_TAKE_RGB_PERIOD_MEAS      0x0d
#define COMMAND_UNLOCK_CALIBRATOR         0x0e
#define COMMAND_FORCE_UNLOCK_CALIBRATOR   0x0f

/* define error codes */



//
// Comm timeout
//
#define TIMO_DISABLE                0
#define TIMO_ENABLE                 1

#define CPUTIMO_1S      1L   /*   1 second  */
#define CPUTIMO_3S      3L   /*   3 seconds */
#define CPUTIMO_5S      5L   /*   5 seconds */
#define CPUTIMO_10S    10L   /*  10 seconds */
#define CPUTIMO_20S    20L   /*  20 seconds */
#define CPUTIMO_30S    30L   /*  30 seconds */
#define CPUTIMO_1M     60L   /*   1 minute  */
#define CPUTIMO_3M    180L   /*   3 minutes */
#define CPUTIMO_5M    300L   /*   5 minutes */

#define TIMO_DEFAULT_LIMIT  CPUTIMO_3S  /* default timeout limit */

//
// Chroma3 EEPROM structure
//
#define  EE_SERIAL_NUMBER     0
#define  EE_MATRIX            4
#define  EE_RESOLUTION        40
#define  EE_CAL_MON_FREQ      42
#define  EE_CAL_RED_COUNT     44
#define  EE_CAL_GREEN_COUNT   46
#define  EE_CAL_BLUE_COUNT    48
#define  EE_CALIBRATION_DATE  50
#define  EE_EEPROM_VERSION    126

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

// EEPROM structure
extern unsigned long  eeSerialNumber;
extern double         eeMatrix[9];
extern unsigned short eeMeasResolution;
extern unsigned short eeCalMonFreq;
extern unsigned short eeCalRedCount;
extern unsigned short eeCalGreenCount;
extern unsigned short eeCalBlueCount;
extern unsigned long  eeCalDate;
extern unsigned short eeEEPROMVersion;

//extern unsigned char toolkit_rev[];



extern void SIConvertToWord ( char *, short * ) ;
extern void SIConvertToLong (char *, long * );
extern void SIConvertToFloat (char *, float * );


extern long IOInit ( short  );
extern long IOClose ( void );
extern long IOSendGetData ( short , char *, short , char *, short * );

extern unsigned char *IOgetttoolkitrev ( void );





//
// ChromaFuncs.c prototypes
//
extern long ChromaOpen ( short , char *, unsigned char * );
extern long ChromaInitialize ( void );
extern long ChromaClose ( void );
extern long ChromaGetVersionString ( char * );
extern long ChromaMeasureRedChannel ( long * );
extern long ChromaMeasureGreenChannel ( long * );
extern long ChromaMeasureBlueChannel ( long * );
extern long ChromaMeasureRGBScaled ( double * );
extern long ChromaMeasureRGBbyPeriodScaled ( double *, unsigned char , unsigned char , unsigned char  );
extern long ChromaAutoMeasureRGBScaled ( double * );
extern long ChromaMeasureXYZ ( double * );
extern long ChromaMeasureXYZbyPeriod ( double *XYZdata, unsigned char ucRNum, unsigned char ucGNum, unsigned char ucBNum );
extern long ChromaAutoMeasureXYZ ( dXYZ *dXYZdata, short *sper );
extern long ChromaSetIntegrationTime ( long  );
extern long ChromaGetIntegrationTime ( long * );
extern long ChromaMeasurePeriod ( long * );
extern long ChromaMeasurePeriodHiRes ( long * );
extern long ChromaReadEEPROMByte ( unsigned char , unsigned char * );
extern long ChromaWriteEEPROMByte ( unsigned char , unsigned char  );
extern long ChromaSetTimeOut ( char, long );
extern long ChromaSetMeasurementTimeOut ( long * );

extern long ChromaMeasureRedPeriod ( long *, unsigned char  );
extern long ChromaMeasureGreenPeriod ( long *, unsigned char  );
extern long ChromaMeasureBluePeriod ( long *, unsigned char  );
extern long ChromaMeasureRGBPeriod ( long *, unsigned char , unsigned char , unsigned char  );
extern long ChromaUnlockCalibrator ( unsigned char * );
extern long ChromaDetermineEquivalentRGBValues ( dXYZ dXYZsource, dRGB *dRGBresult );


#endif  /* _Chromafuncs_h_ */
/******************************************************************************
**                         F O O T E R
*******************************************************************************
**
**     Copyright (c) 1989-97 Sequel Imaging, Inc., Londonderry NH
**            THIS INFORMATION IS COMPANY CONFIDENTIAL
******************************************************************************/
