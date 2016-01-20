////////////////////////////////////////////////////////////////////////////////////////////////
//	The device input dialog																  //
//  tcrindialog.h																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef INPUT_DIALOG_H
#define INPUT_DIALOG_H


#include	<Sound.h>
#include "monacoprint.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "basewin.h"
#include "mcomem.h"
#include <QDOffscreen.h>
#include "profiledocinf.h"
#include "rawdata.h"
#include "mcoiccmac.h"

typedef enum {
	Ok_Box = 0,
	Save_as,
	Strip_Rect,
	Message,
	Cur_sheet,
	Cur_strip,
	Patch_Rect,
	New_Patch_Rect,
	Slider,
	Redo,
	Start,
	VoiceSet,
	ThreeDBox,
	UpdateEvent,
	TimeEvent,
	Position_Up,
	Position_Down,
	Position_Left,
	Position_Right,
	Lift
	} Input_Dial_IDS;
	
#define OK	1
#define Cancel 3

#define NumInputIDS (Lift+1)

typedef enum {
	DType_Unknown = 0,
	DType_Density,
	DType_Lab
	} Input_dial_DType;

class InputDialog:public baseWin {

private:
protected:

short		ids[NumInputIDS];

// the lab data
// initilalize to -10000
McoHandle	_patchH;
McoHandle	_patchcopyH;
double		*patch_data;

// the number of patches that has been read in
int32		patches_read;

// the current patch
int32		current_patch;


int32		patch_per_strip;
int32		strip_per_sheet;

int32		current_strip;
int32		current_sheet;

Boolean 	NotAlreadFinished; // if patches have already been read in then do not disable cancel


GWorldPtr	BigGW;	// The gworld with the patches in it
int32		top_patch;	// The top patch in the display
int32		num_display_patch; // the number of patches displayed in the window
int32		data_patches;
int32		total_patches;
int32		num_used_patches;

int32		current_top; 	// the currently displayed top patch
int32		current_bottom; // the currently displayed bottom patch
int32		current_disp;	// the current displacement

int32		_first;
int32		_waiting;
int32		_done;
int32		_pagesize;

McoHandle	_rgbpatchH;
McoHandle	_labpatchrefH;


ProfileDocInf 	*doc;
RawData		*patchD;

//A flag to indicate the type a patches (RGB or CMYK)
PatchType patchtype,oldpatchtype;



double	 	gamma;

// The sound channel used for alerting the user to errors
SndChannelPtr	chan;
short			voice_set;

int32	_last_tick_time2;
McoXformIccMac    *lab_rgb_xform,*cmyk_rgb_xform;

Boolean Scramble;

Input_dial_DType	dtype;
public:


InputDialog(ProfileDoc *dc,RawData *pD,int strips);
~InputDialog(void);

McoStatus StorePatches(void);
McoStatus StoreDataToTemp(void);

Boolean checkFinished(void);
void init(void);
McoStatus DrawSinglePatch(int32 patch_num);
McoStatus DisplayPatches(int32 top_patch,int32 bottom_patch);
McoStatus ScrollDisplay(int32 patch_num,int32 direction);
McoStatus ShowPatch(int32	patch_num);
McoStatus ShowPatchScroll(int32	patch_num);
int32	whichPatch(Point thePoint);
McoStatus playSound(short sn);
McoStatus makeRGB(double *rgb_data,double *lab_data);
McoStatus loadSwop(double *rgb_data,double *lab_data);
void ConvertToRGB(double *lab,double *rgb,int n);
void ConvertDensityToRGB(double d,int c,double *rgb);
void ConvertToLAB(double *rgb,double *lab, int n);
void getpatchRGB(void);
McoStatus checkError(int32 start_patch);
McoStatus SetSheetStripText(void);
int32 findNextPatch(void);
Boolean TimerEventPresent(void);
McoStatus DrawWindow(void);
McoStatus UpdateWindow(void);
Boolean isMyObject(Point where,short *item);
Boolean KeyFilter(EventRecord *theEvent,short *itemHit );
// return true if a black strip is to be added to the first of each column
virtual int32 BlackPatch(int32 i) {return 0;}

};

#endif