/******************************************************************************
**
**  Project Name: SIPCAL
**     File Name: SIPCAL.H
**
**   Description: Main Header file for SIPCAL
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
** 18Feb97 Added support for Win16.
** 21Mar97 4.0
** 11Apr97 4.0.1 Added 2 routines used for RGB matching.
**
******************************************************************************/
#ifndef _sipcal_h_
#define _sipcal_h_


/***************** I M P O R T A N T  for  D E V E L O P E R S *********
 *
 * Only ONE (1) of the following #defines can be defined at one time:
 * MAC_KIT, IBMPC_KIT, IBMPC_POLL_IO, WIN16_KIT
 *
 * These have been placed here instead of having them as compile line
 * options because not all compilers (Think C on the Mac) do not support
 * compile line options.
 *
 * For the Macintosh, MAC_KIT must be defined.  For MPW developers, the
 * #define MPW_MAC_KIT must be defined, else Think C will be used.
 *
 * For the IBM PC, IBMPC_KIT must be defined.  There are no more #defines
 * for compilers since the toolkit works with both Microsoft and Borland.
 *
 ***************** I M P O R T A N T  for  D E V E L O P E R S *********/

#if 0
// defined in compiler options

#undef   MAC_KIT             /* defined means Macintosh                    */
#undef   MPW_MAC_KIT         /* defined means MPW, undefined means Think C */

#undef   IBMPC_KIT           /* defined means IBM PC                       */
#undef   IBMPC_POLL_IO       /* defined polled, undefined means interrupt  */

#undef   WIN16_KIT           /* defined means MS Windows 16-bit            */

#endif

/***************************************************************************/


typedef struct dYxy
{
   double   Y;
   double   x;
   double   y;
   double   z;
} dYxy;

typedef struct dXYZ
{
   double   X;
   double   Y;
   double   Z;
} dXYZ;

typedef struct dRGB
{
   double  R;
   double  G;
   double  B;
} dRGB;

#define  FOOT_LAMBERTS  0
#define  CANDELAS       1

//
// Function prototypes
//
extern long sipOpenCalibrator ( short sPort, char *szVersion, unsigned char *ucOption );
extern long sipCloseCalibrator ( void );
extern long sipAutoMeasureYxy ( short Units, dYxy *YxyData, short *sPeriods );
extern long sipMeasureXYZ ( dXYZ *XYZData );
extern long sipMeasureYxy ( short Units, dYxy *YxyData );
extern long sipMeasureRGB ( dRGB *RGBData );
extern long sipMeasMonFreqAndSetIntTime ( short sNumFields, double *dMonFreq );
extern long sipSetIntegrationTime ( double dSeconds );
extern long sipGetIntegrationTime ( double *dSeconds );
extern unsigned char *sipGetToolkitRev ( void );
extern unsigned long sipGetCalibratorSerialNumber ( void );
extern unsigned char *sipGetCalibrationDate ( void );
extern short sipGetCalibratorType ( void );
extern long sipGetRGBvaluesFromYxy ( short sUnits, dYxy dYxySource, dRGB *dRGBResult );
extern void sipGetLastRGBscaledValues ( dRGB *dRGBResult );

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

//
// sipCal error codes
//
#define ERR_COMM_INVALID_PORT       -69 /* invalid comm port to initialize */
#define ERR_COMM_DATAREADY_TIMO     -66 /* timed out waiting to accept data */
#define ERR_COMM_SEND_TOO_MANY      -70 /* string to send exceeds max transfer count */
#define ERR_COMM_DSR_TIMO           -71 /* timed out waiting to send command */
#define ERR_WRONG_VERSTRING         -103 /* invalid version string received */
#define ERR_INVALID_TIMOFLAG        -108 /* invalid timeout flag */
#define ERR_INVALID_TIMEOUT         -109 /* invalid timeout time (count) */

#define ERR_WIN_BADPORT          -136 /* MS Windows bad port */
#define ERR_WIN_BADDCB           -137 /* MS Windows bad device control block */
#define ERR_WIN_BADBYTESSENT     -138 /* MS Windows bad bytes sent */
#define ERR_WIN_INVALID_COUNT    -139 /* MS Windows invalid count  */

#define ERR_WIN_OUTBUF_TOOSMALL  -171 /* the comm outbuffer size is less than string being sent */
#define ERR_WIN_IE_BADID         -172 /* Invalid or unsupported COMM ID */
#define ERR_WIN_IE_BAUDRATE      -173 /* Unsupported baud rate */
#define ERR_WIN_IE_BYTESIZE      -174 /* Invalid byte size for rec/trans queue */
#define ERR_WIN_IE_DEFAULT       -175 /* Error in COMM default values. */
#define ERR_WIN_IE_HARDWARE      -176 /* Comm hardware not present */
#define ERR_WIN_IE_MEMORY        -177 /* Windows unable to allocate rec/trans queue */
#define ERR_WIN_IE_NOPEN         -178 /* Comm device not open */
#define ERR_WIN_IE_OPEN          -179 /* Comm device already open */
#define ERR_WIN_CE_BREAK         -180 /* Comm hardware detects a break */
#define ERR_WIN_CE_CTSTO         -181 /* Comm: Clear-to-send timeout */
#define ERR_WIN_CE_DSRTO         -182 /* Comm: Data-set-ready timeout */
#define ERR_WIN_CE_FRAME         -183 /* Comm: hardware detects a framing error */
#define ERR_WIN_CE_MODE          -184 /* Comm: requested mode not supported */
#define ERR_WIN_CE_OVERRUN       -185 /* Comm: a character is not read from hardware before next arrives and is lost */
#define ERR_WIN_CE_RLSDTO        -186 /* Comm: RLSDT timeout */
#define ERR_WIN_CE_RXOVER        -187 /* Comm: Receive buffer overflow */
#define ERR_WIN_CE_RXPARITY      -188 /* Comm: Hardware detects a parity error */
#define ERR_WIN_CE_TXFULL        -189 /* Comm: transmit queue is full while trying to queue a character */
#define ERR_MTX_NOTCOLORPOD      -190 /* not a color pod attached therefore matrix stuff invalid to use */

#define ERR_FF_COULD_NOT_SYNC       -200 /* could not sync to source */
#define ERR_INVALID_MATRICES_ORDER  -500 /* invalid size matrices to multiply */
#define ERR_DIVIDE_BY_ZERO          -501 /* zero value measured in at least one channel */
#define ERR_ZERO_INT_TIME           -502 /* attempted to take a measurement with an integration time of zero */
#define ERR_UNKNOWN_CALIBRATOR      -503 /* calibrator firmware version found is not supported by this software */
#define ERR_FUNCTION_NOT_AVAILABLE  -504 /* attempted function is not available on this mode of calibrator */
#define ERR_LOCKED_CALIBRATOR       -505 /* calibrator is locked */

//
// Default number of fields for integration times
//
#define LONG_INT_TIME      150
#define NOMINAL_INT_TIME   100
#define SHORT_INT_TIME     50

#define CANDELA_FACTOR     (10.76 / 3.14159265)

//
// Define Calibrator types
//
#define CHROMA_LOCKED   -1
#define CHROMA_UNKNOWN  0
#define CHROMA_2        2
#define CHROMA_3        3
#define CHROMA_4        4

//
// Define EEPROM States
//
#define  EEPROM_OK            0
#define  EEPROM_UNINITIALIZED 1
#define  EEPROM_CORRUPT       2


#endif  /* _sipcal_h_ */
/******************************************************************************
**                         F O O T E R
*******************************************************************************
**
**     Copyright (c) 1989-97 Sequel Imaging, Inc., Londonderry NH
**            THIS INFORMATION IS COMPANY CONFIDENTIAL
******************************************************************************/
