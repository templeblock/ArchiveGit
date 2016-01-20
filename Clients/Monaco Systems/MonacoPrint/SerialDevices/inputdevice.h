/*
	inputdevice.h
	
	The base class for an input device
	
	James Vogh
	
*/

#ifndef IN_INPUTDEVICE
#define IN_INPUTDEVICE

#include "defaults.h"
#include "Thermometer.h"
#include "tweak_patch.h"

typedef enum {
	ReadLabMode = 0,
	ReadDensityMode
	};


class  InputDevice {
private:
protected:

	ThermDialog *thermd;
	
	Tweak_Patch *tweak;
	
	int			mode;
	
	// the density values for the paper
	double		paperDensity[4];
	
	
public:
	DeviceTypes device_num;

	McoStatus	state;
		
	InputDevice(DeviceTypes device_num);
	virtual ~InputDevice(void);

virtual McoStatus Calibrate(void) {return MCO_SUCCESS;}
virtual McoStatus Initialize(void) {return MCO_SUCCESS;}
virtual Boolean IsBufferEmpty(void) {return FALSE;}
virtual Boolean IsDataPresent(void) {return FALSE;}
virtual McoStatus sendReadCommand(void) {return MCO_SUCCESS;}
virtual McoStatus getNextPatch(double *lab) {return MCO_SUCCESS;}
virtual McoStatus getNextPatch(long patchnum,double *lab) {return MCO_SUCCESS;}

virtual McoStatus GetPaperDensity(void) {return MCO_FAILURE;}
void SetPaperDensity(double *density);

virtual McoStatus SetLabOutput(void) {return MCO_SUCCESS;}

virtual McoStatus SetDensityOutput(void) {return MCO_FAILURE;}
};

#endif