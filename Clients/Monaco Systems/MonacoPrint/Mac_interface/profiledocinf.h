///////////////////////////////////////////////////////////////////////////////////
//	profiledocinf.h															  	 //
//																			  	 //
//	User interface parts of profiledoc											 //
//  This is the Mac specific part											     //
//	James Vogh																 	 //
// June 4, 1996																  	 //
///////////////////////////////////////////////////////////////////////////////////

#ifndef PRINT_PROFILE_INF_H
#define PRINT_PROFILE_INF_H


#include "profiledoc.h"
#include "csprofile.h"
#include "gammutwin.h"
#include "gamutwindata.h"
#include "defaults.h"
#include "inputdevice.h"
#include "patchset.h"

class ProfileDocInf:public ProfileDoc{

private:
protected:
public:

GammutWinData	gdata;
RawData			*waitingPatches;
Defaults		*defaults;

DeviceTypes		last_type;
int				last_port;

PatchSet		*patchset;

InputDevice		*tcom;

ProfileDocInf(void);
~ProfileDocInf(void);

// check the dongle and decrement the counter
McoStatus _checkEve(void);
//read the file, now only read the data file
McoStatus _read_datafile(RawData **patchd_2,FILE *fs);
// open the dialog 
McoStatus _handle_read(short *item,short *which,int density);
// save the file from the menu
// data may need to be copied from the input dialog
McoStatus  save_datafile_menu(void);
// Remove a window from the list of windows
McoStatus removeWindow(baseWin *win);
// what is the status of the user interface
long	inputStatus(void);
// if p is patchDCopy then copy it into patchD
McoStatus copyWaiting(RawData *p);
// open an input device
McoStatus openInputDevice(int xrite_num,int handheld, int density);
// change the current input device
// do nothing if the input dialog is open
McoStatus updateInputDevice(int xrite_num);
// close the input device if it is open
McoStatus closeInputDevice(void);

// Do a command
McoStatus doCommand(WindowCode *wc,WindowCode *wc2,void **data,Boolean *changed);
Boolean checkCloseDoc(void);
int	checkDocSave(void);
int	checkDocCanBeSave(void);
int isMeasuring(void);
};

#endif