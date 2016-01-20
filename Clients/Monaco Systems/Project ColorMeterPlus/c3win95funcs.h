/*c3win95funcs.h */


#ifndef _c3win95funcs_h 
#define _c3win95funcs_h 

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


typedef enum _EEPROM_CHROMA3
{
  EEP_SERIALNUMBER		= 0,
  EEP_MATRIX			= 4,  // 3x3 matrix of 4-byte floats
  EEP_MEASUREMENT_RES	= 40, // resolution X 10^9 (in seconds)
  EEP_MONITOR_FREQUENCY = 42, // Frequency X 100 (in HZ with 2 decimal point resolution )
  EEP_RED_FIELDCOUNT	= 44,
  EEP_GREEN_FIELDCOUNT	= 46,
  EEP_BLUE_FIELDCOUNT	= 48,
  EEP_CALIBRATION_DATE	= 50, 
  EEP_EEPROM_VERSION	= 126
}EEPROM_CHROMA3;



#define  RED      0
#define  GREEN    1
#define  BLUE     2


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
#define COMMAND_MEASURE_PERIOD_HI_RES     0x09 // Chroma 4
#define COMMAND_TAKE_RED_PERIOD_MEAS      0x0a
#define COMMAND_TAKE_GREEN_PERIOD_MEAS    0x0b
#define COMMAND_TAKE_BLUE_PERIOD_MEAS     0x0c
#define COMMAND_TAKE_RGB_PERIOD_MEAS      0x0d
#define COMMAND_UNLOCK_CALIBRATOR         0x0e
#define COMMAND_FORCE_UNLOCK_CALIBRATOR   0x0f


typedef struct GAMMA_HEADER1 
{
   unsigned short RedIntTime;
   unsigned short GreenIntTime;
   unsigned short BlueIntTime;
} GAMMA_HEADER;

typedef struct C3EEP
{
  long  sn;
  double matrix[9];
  short resolution;
  short freqHz;
  short Rfield;
  short Gfield;
  short Bfield;
  long  calDate;
  char  notused[72];
  short version;
} C3EEPSTRUCT;

typedef struct C3INFO
{
 
  long sensorIntegration[3];
  long monitorIntegration[3];
 
} C3INFO;


typedef enum _C3ERROR
{ 
  C3_OK = 0,
  C3_FUNCTION_NOTSUPPORTED,
  C3_BAD_RETURN_COMMANDBYTE,
  
}C3ERROR; 

long C3Open ( int port, char *cVerString );
long C3Close ( void );
long C3Initialize( void );
long C3GetEEPromByte(char address, unsigned char * data);
long C3GetVersionString( char *data );
long C3GetRedData( long *data );
long C3GetGreenData( long *data ) ;
long C3GetBlueData( long *data );
long C3SetSensorIntegration ( double data );
long C3GetSensorIntegration ( double *data );
long C3MeasurePeriod ( double *data );
long C3GetXYZData( double *data);
long C3GetSerialNumber( long * );


long C3SetEEPromByte();
long C3SetCalDate();
long C3SetSerialNumber();
long C3GetCalDate();
long C3GetRGBraw( );
long C3GetFrequency();
long C3MeasureDark( );
long ChromaMeasurePeriodHiRes ( long *lPeriod );
long ChromaMeasureRedPeriod ( long *lRedData, unsigned char ucNumPeriods );
long ChromaMeasureGreenPeriod ( long *lGreenData, unsigned char ucNumPeriods );
long ChromaMeasureBluePeriod ( long *lBlueData, unsigned char ucNumPeriods );




long C3SetMonitorIntegrationTimeSerial();
long C3GetMatrix();
long C3GetMatrixN();
long C3SetMatrix();
long C3SetMatrixN();






#endif // define
