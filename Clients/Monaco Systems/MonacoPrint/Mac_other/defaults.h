//////////////////////////////////////////////////////////////////////////////////////////
//  default parameters for monacoprint													//
//  defaults.h																		//
//  James Vogh																			//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef IN_DEFAULTS
#define IN_DEFAULTS

#define DEFAULTS_MAGIC_NUM	3495
#define DEFAULTS_VERSION 1

typedef enum {
	DT_None = 0,
	DT_DataFile,
	DT_TiffFile,
	DT_DTP51,
	DT_XriteDTP41,
	DT_Xrite408,
	DT_TechkonCP300,
	DT_TechkonSP820,
	DT_TechkonTCRCP300,
	DT_TechkonTCRSP820,
	DT_GretagHand,
	DT_GretagTable
	} DeviceTypes;
	

class Defaults {
private:
protected:
public:

int	printer_type;

DeviceTypes	input_type;
int patch_format;
int port;

Defaults(void);
~Defaults(void);
};

#endif