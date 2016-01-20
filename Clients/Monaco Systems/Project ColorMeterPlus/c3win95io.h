



typedef enum GIZ_PORT
{
  GIZ_PORT_COM1    = 0,
  GIZ_PORT_COM2    = 1,
  GIZ_PORT_COM3    = 2,
  GIZ_PORT_COM4    = 3,
  GIZ_PORT_COM5    = 4,
  GIZ_PORT_COM6    = 5,
  GIZ_PORT_COM7    = 6,
  GIZ_PORT_COM8    = 7
}GIZ_PORT;

typedef enum 
{

	CMD_OK,
	CMD_ERROR_CMD_RET

}CMD_ERROR;




long win_SendGetCMDandData(char *inBuf, short *sPtrnumBytes);

long  win_InitSerial ( int port );
/*  Opens the serial i/o on port.
    The port is always set to GIZNONE | GIZONESTOPBIT | GIZBAUD_9600 | GIZEIGHTBITS 
--------------------------------------------------------------------------------------------------------*/

long  win_CloseSerial ( void );
/*  Closes the serial port after flushing any data in it.
--------------------------------------------------------------------------------------------------------*/
void win_SetTimeOut(double dataInSecs);






