/******************************************************************/
/* basewin.h													  */
/* The base window definition									  */
/* Has been made compatiable with the original dialog code		  */
/* so names are as if this is a dialog							  */
/* May 5, 1995													  */
/******************************************************************/

// An id for each window

#ifndef IN_BASEWIN
#define IN_BASEWIN

#include "mcomem.h"
#include "mcostat.h"
#include <QD3DGeometry.h>
#include "Monaco Slider.h"

#define MaxNumWins 1000

typedef enum {
	No_Dialog = 0,
	Gammut_Dialog1,
	Gammut_Dialog_Dst,
	Gammut_Dialog_Src,
	Startup_Dialog,
	PrintWindow,
	AboutWindow,
	StripDialog,
	CalibDialog,
	SelAdjWindow,
	OptionWindow,
	CropWindow,
	TcrWindow,
	TechkonWindow,
	GretagSLWindow,
	GretagSCWindow,
	DisplayWindow,
	ProcessTherm,
	ProcessThermClose,
	GammutSurfaceTherm1,
	GammutSurfaceTherm_Src,
	GammutSurfaceTherm_Dst,
	GammutCompTherm,
	TweakWindow,
	TabbedDocWindow,
	WinTab,
	BlankWinTab
	} MS_WindowTypes;
	
#define NumWindowTypes 20
	
typedef enum {
	WE_Do_Nothing = 0,
	WE_OK,
	WE_Cancel,
	WE_Open_Window,
	WE_Close_Window,
	WE_Rotate,
	WE_Scale,
	WE_Translate,
	WE_Stop,
	WE_DoGenData,
	WE_DidGenData,
	WE_CloseDoc,
	WE_Doc_Print,
	WE_Doc_Read,
	WE_Source_Read,
	WE_Doc_Open,
	WE_Doc_Save,
	WE_Doc_Close_Save,
	WE_Doc_Save_As,
	WE_Revert,
	WE_PrDoc_Calibrate,
	WE_PrDoc_Gammut,
	WE_PrSource_Gammut,
	WE_PrDoc_Tweak,
	WE_PrDoc_Options,
	WE_PrDoc_BuildTable,
	WE_NewProfileDoc,
	WE_OpenProfileDoc,
	WE_UpdateInputDevice,
	WE_Set_Pref
	} Window_Events;
	
typedef enum {
	WC_SetBack = 0,		// Set the cursor back to where it should be
	WC_Arrow,			// Set to arrow
	WC_Watch,			// Set to watch
	WC_Watch_Next,		// Set to next watch
	WC_EyeDrop,			// Set to eyedrop
	WC_Box,				// Set to box
	WC_CrossTemp,		// temporarily set to cross
	WC_CropTemp,		// temporarily set to crop
	WC_CropFinTemp,		// temporarily set to finish crop
	WC_CropCancelTemp	// temporarily set to cancel crop
	} WinEve_Cursors;
	
	
class Document;

// defines a simple window event

struct WindowCode{
	Window_Events	code;		// the code 
	MS_WindowTypes 	wintype;	// the type of window that the code applies to
	int32		  	winnum;		// the number of the window
	Document		*doc;		// a pointer to a document
	} ;	
	
#define TIMER_ITEM 1000

class baseWin {
private:
protected:
MS_WindowTypes	DialogType;
Rect 		r;
DialogTHndl dt;


Boolean		Changed;
public:

McoStatus object_state;

WindowPtr 	dp;


Boolean		Front;

MS_WindowTypes	WinType;	// The type of window (main, option, preview etc)
int32			WinNum;		// The number of the window (for use with selective color)

Boolean		waitingData; 	// Set to true if window is waiting for general data
Boolean 	gettingData;	// This flag is set when general data is being collected
Boolean		disabled;

long		priority;		// The priority level of the window

baseWin(void) {}
virtual ~baseWin(void);
Boolean isMyWindowPtr(WindowPtr win) { return (dp == win);}
Boolean isMyWindowType(MS_WindowTypes winType) { return (WinType == winType);}
virtual Boolean isMyWindowNum(int32 winNum) { 
	if (winNum <0) return TRUE;
	return (winNum == WinNum);
	}

// if true then always update window
virtual Boolean alwaysUpdate(void) { return FALSE;}
// See if to see if a overall refresh needs to be done
// This is used for example for text fields that cause a long update to occur
// If the user wants to type in '100' the system could update three different times
// This feature allows a window to wait to cause an update	
virtual Boolean checkChanged(void) { return FALSE;}
// set pointers within dialog to pointers contained within params
virtual McoStatus 	setDataPointers(void) {return MCO_SUCCESS;}
// filter the key that was pressed
virtual Boolean 	KeyFilter(EventRecord *theEvent,short *itemHit ) {return FALSE;}
// Draw the contents of the window
virtual McoStatus 	DrawWindow(void) {return MCO_SUCCESS;}
virtual McoStatus   forceUpdateData(void) {return MCO_SUCCESS;}

virtual McoStatus   updateDataForControl(int num) {return MCO_SUCCESS;}
// Perform an update
virtual McoStatus 	UpdateWindow(void);
// Process the events 
virtual McoStatus DoEvents(short item, Point clickPoint,WindowCode *wc, int32 *numwc,void **data,Boolean &changed)
 {return MCO_SUCCESS;}
// Something has happened in another dialog that requires an update to occur
virtual McoStatus 	updateWindowData(int changed) {return MCO_SUCCESS;}
// Define a region where events are reported (preview window for example)
virtual McoStatus 	makeEventRegion(RgnHandle tempRgn,RgnHandle mouseRgn) {return MCO_SUCCESS;}
// Process the event where the mouse is within an active region
virtual	McoStatus 	doPointInWindowActiveRgn(Point where, WinEve_Cursors &cursortype) { return MCO_SUCCESS;}
// Move a window to the back
virtual Boolean 	MoveToBack(void) { return MCO_SUCCESS;}
// Move a window to the front
virtual McoStatus 	MoveToFront(Boolean select);
// Drag the window and update data within job
virtual McoStatus 	MoveWindow(Point where);
// Delete the window
virtual McoStatus 		removeWindow(void) {return MCO_SUCCESS;}
// The size of the structure, used for debuging
virtual long		theSize(void) {return sizeof(baseWin);}
// Return true if a timer event has occured
virtual Boolean TimerEventPresent(void) {return FALSE;}
virtual void DoTransform(int code, double v1, double v2,double v3) {}
McoStatus setDialog(int32 dialogid);
McoStatus setDialog(int32 dialogid,int h, int v);
int32 GetSlider(Point clickPoint,short item);
McoStatus setSlider(int16 *picts,int16 *offsets, int16 slider_id,RefConType	*theRefCon);
McoStatus DrawColorBox(short item,double *rgb);
void getDialogRect (Rect *theRect, Boolean entireWindow);

virtual Boolean isMyObject(Point where,short *item) { return FALSE;}

void frame_button(int item);
void threeD_box(int item);
virtual	void GetRotation(TQ3Matrix4x4 *rotation) {}
virtual	void SetRotation(TQ3Matrix4x4 *rotation) {}
// debugging
virtual void showNum(int32 num) {}

void SetItemText(short item,unsigned char*);
void SetItemTextForce(short item,unsigned char*);
void GetItemText(short item,unsigned char *st);
void SetItemTextToNum(short item,long num);
void SetItemTextToNum(short item,double num,int perc);

int GetItemTextFromNum(short item,long *num,long min,long max);
int GetItemTextFromNum(short item,double *num,double min,double max,int perc);
};

pascal void DrawNormalSur(DialogPtr dp, short item);
void box_item(DialogPtr dp, short item);
void unbox_item(DialogPtr dp, short item);
short isPressed(unsigned short k );
void Disable(DialogPtr inDialog, short inItemNum);
void Enable(DialogPtr inDialog, short inItemNum);
#endif