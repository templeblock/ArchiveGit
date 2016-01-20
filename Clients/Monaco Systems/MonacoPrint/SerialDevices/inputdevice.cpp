/*
	inputdevice.cpp
	
	The base class for an input device
	
	James Vogh
	
*/

#include "inputdevice.h"
#include "monacoprint.h"
#include "pascal_string.h"
#include "fullname.h"
#include "think_bugs.h"


InputDevice::InputDevice(DeviceTypes dn)
{
FileFormat file;
FSSpec	fspec;
Str255	inname;
  
Str255 	dirname = MONACO_FOLDER;
Str255 	dirname2 = MONACO_DEVICES;
Str255 	filename;
FILE	*fs;
McoStatus status;

device_num = dn;

	tweak = 0L;
	thermd = 0L;
	mode = -1;

	switch (device_num) {
		case DT_DTP51:
			copy_str(filename,MONACO_DTP51);
			break;
		case DT_TechkonCP300:
		case DT_TechkonTCRCP300:
			copy_str(filename,MONACO_TECHKON_CP300);
			break;
		default:
			return;
		}
			
		inname[0] = 0;
		pstrcat(inname,dirname);
		pstrcat(inname,"\p:");
		pstrcat(inname,dirname2);

		ptocstr(filename);
		ptocstr(inname);
		status = file.findFilefromPref(&fspec, (char*)filename,(char*)inname);

		if (status == MCO_SUCCESS)
			{
			PathNameFromDirID(fspec.parID, fspec.vRefNum, inname);
			ptocstr(inname);	
				
			fs = fopen( (char*)inname, "r" );
			if(fs) 
				{
				tweak = new Tweak_Patch(Calibrate_Tweak);
				tweak->Load_Data(fs);
				}
			fclose(fs);
			}
}


InputDevice::~InputDevice(void)
{
if (tweak) delete tweak;
if (thermd) delete thermd;
tweak = 0L;
thermd = 0L;
}

void InputDevice::SetPaperDensity(double *density)
{
int i;

for (i=0; i<4; i++) paperDensity[i] = density[i];
}