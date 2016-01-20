////////////////////////////////////////////////////////////////////////////////////////////////
//	The new xrite input dialog																  //
//  inputdialog2.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef INPUT_DIALOG2_H
#define INPUT_DIALOG2_H


#include	<Sound.h>
#include "monacoprint.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "basewin.h"
#include "mcomem.h"
#include "xritecom.h"
#include <QDOffscreen.h>
#include "profiledoc.h"
#include "xyztorgb.h"

class InputDialog2:public baseWin {

private:
protected:


// the lab data
// initilalize to -10000
double		patch_data[MAX_NUM_PATCHES*3];

// the number of patches that has been read in
int32		patches_read;

// the current patch
int32		current_patch;

int32		patch_per_strip;
int32		strip_per_sheet;

int32		current_strip;
int32		current_sheet;


GWorldPtr	BigGW;	// The gworld with the patches in it
int32		top_patch;	// The top patch in the display
int32		num_display_patch; // the number of patches displayed in the window
int32		data_patches;
int32		total_patches;
int32		num_used_patches;

int32		_waiting;
int32		_done;
int32		_pagesize;

McoHandle	_rgbpatchH;
McoHandle	_labpatchrefH;

XriteCom	*xcom;	//Xrite communications

ProfileDoc 	*doc;

//A flag to indicate the type a patches (RGB or CMYK)
PatchType patchtype;


XyztoRgb 	*xyztorgb;
double	 	gamma;

// The sound channel used for alerting the user to errors
SndChannelPtr	chan;
short			voice_set;

Boolean Scramble;
public:


InputDialog2(ProfileDoc *dc,XyztoRgb *xtr,double g);
~InputDialog2(void);

McoStatus StorePatches(void);

void checkFinished(void);
void init(void);
McoStatus DrawSinglePatch(int32 patch_num);
McoStatus DisplayPatches(int32 top_patch,int32 bottom_patch);
McoStatus ScrollDisplay(int32 patch_num,int32 direction);
McoStatus ShowPatch(int32	patch_num);
McoStatus playSound(short sn);
void makeRGB(double *rgb_data);
McoStatus loadSwop(double *lab_data);
void ConvertToRGB(double *lab,double *rgb,int n);
void ConvertToLAB(double *rgb,double *lab, int n);
void getpatchRGB(void);
McoStatus _save_target(void);
McoStatus checkError(int32 start_patch);
McoStatus readInPatches(void);
McoStatus SetSheetStripText(void);
int32 findNextPatch(void);
Boolean TimerEventPresent(void);
McoStatus DrawWindow(void);
McoStatus UpdateWindow(void);
Boolean isMyObject(Point where,short *item);
McoStatus DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed);
};

#endif