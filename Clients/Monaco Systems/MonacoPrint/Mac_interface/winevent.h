/******************************************************************/
/* winevent.h													  */
/* The main event handler										  */
/* Get the event and then handle the events						  */
/* This code is from maincurvedial.c 							  */
/* and has been adapted for MonacoPRINT							  */
/* May 5, 1995												      */
/* James Vogh													  */
/******************************************************************/

#ifndef IN_WINEVENT
#define IN_WINEVENT

#include "basewin.h"
#include "share_data.h"
#include "qd3d_share.h"
#include "QD3DPick.h"
#include "document.h"
#include "xyztorgb.h"

#define NumCursors 13 
#define EyeDropCursor 16000
#define WatchCursor	16001
#define NumWatchCursor 7
#define BoxCursor	16008

#define CropCursor		1003
#define CropFinCursor	1004
#define CropCancelCursor 1005

#define G_MENU_B 1801
#define G_FILE_MENU 1803
#define G_OPEN_ITEM 1
#define G_CLOSE_ITEM 2

#define MaxNumDocs 1000

// The custum lab attribute type

#define LabAttributeType 'laba'

typedef enum {	
	WC_Nums_Eyedrop = 0,
	WC_Nums_Cross,
	WC_Nums_Box,
	WC_Nums_Crop,
	WC_Nums_CropFin,
	WC_Nums_CropCancel,
	WC_Nums_WatchStart
	} Win_Cursor_Nums;


void ProcessEvent(EventRecord *event);

class AllWins {
private:
protected:
int	numWins;
int	maxWins;
baseWin	**wins;
baseWin *frontWin;	// A handy pointer that points the the front window
					// This window is also contained in wins
					
long	QD3D_Present;

int	num_shared_data;					
Shared_Data **shared_data;



Document	*WaitingDoc;

McoSafeMem *parameters;

CursHandle		cursors[NumCursors];
WinEve_Cursors	currentCursor;
int16			watch_num;

MenuHandle		g_file_handle;



XyztoRgb 	*xyztorgb;
double		monitor_z;

public:
Boolean			DoQuit;
Boolean			DidLogo;
Document	**docs;
int	num_documents;

long	max_priority;
long	process_count;

int _monacoprint;

// Pointers to the data here will be used by the three dialogs


McoStatus Initialize3D(void);
McoStatus LoadXyzToRgb(void);
AllWins(Boolean Init3D);
~AllWins(void);
WindowPtr	getFrontWin(void) 
	{ 
	if (frontWin) return frontWin->dp;
	else return NULL;
	}
McoStatus 	AddWin(baseWin *newwin,Document *doc);
McoStatus 	addToShared(baseWin *win,MS_WindowTypes type);
McoStatus 	addDocument(Document *doc);
McoStatus 	removeDocument(Document *doc);
Boolean 	isDocumentMain(WindowPtr win,Document **doc);
McoStatus	newProfileDoc(void);
McoStatus 	openProfileDoc(void);
McoStatus 	openProfileDoc(FSSpec *fspec);
Boolean 	checkCloseDocs(Document *doc);
Document* 	FindDocFromWin(MS_WindowTypes type,int32 winNum);
Document* 	FindDocFromWin(baseWin *awin);
Boolean 	isCodeDocCode(Window_Events code);

McoStatus   openMainCurve(void);
McoStatus	OpenGammutDialog(void *data,MS_WindowTypes wintype,Document *doc);
McoStatus 	openLogoWin(void);
McoStatus 	openAboutDialog(void);
McoStatus 	openMainWindow(Document *doc);
McoStatus 	openStripWindow(Document *doc);
McoStatus 	openDisplayWindow(Document *doc,void *data,int n);
McoStatus 	openCalibWindow(Document *doc);
McoStatus 	openTweakWin(Document *doc);
McoStatus 	openOptionWin(Document *doc);
McoStatus 	openCropWin(Document *doc);
McoStatus 	openTechkonWindow(Document *doc);
McoStatus	openTcrWindow(Document *doc);
McoStatus 	openGTSLWindow(Document *doc);
McoStatus 	openSpChWindow(Document *doc);
// Open a gammut surface dialog, use info passed in data if data is valid
McoStatus 	openGammutSurface1(void);
// Open a gammut surface dialog, use info passed in data if data is valid
McoStatus 	openGammutSurface2(Document *doc,void *data,MS_WindowTypes type);
McoStatus 	openGammutCompTherm(Document *doc);
McoStatus 	openProcessTherm(Document *doc,MS_WindowTypes type);
// open the main dialog
McoStatus 	openProfDocWin(Document *doc);
McoStatus   openWindow(MS_WindowTypes type,int32 winNum,void *data,Document *doc);
// Set the cursor to the proper state
McoStatus 	setCursor(WinEve_Cursors thecursor);
Boolean 	my_window(WindowPtr win);
baseWin* 	get_window(WindowPtr win);
baseWin*	get_window(MS_WindowTypes winType, int32 winNum);

Boolean		TimerEventPresent(baseWin **win);
McoStatus   getEventRegion(RgnHandle	fullRgn, RgnHandle tempRgn,RgnHandle mouseRgn);
	
McoStatus   FrontWindow(WindowPtr winPtr, MS_WindowTypes winType, int32 winNum, Boolean select,void *data,Document *doc);
McoStatus 	MoveWindow(WindowPtr winPtr,Point where);
McoStatus 	findMaxPriority(void);
McoStatus   removeWindow(MS_WindowTypes winType,int32 winNum);
McoStatus 	removeWindow(WindowPtr win);
McoStatus 	removeWindow(baseWin *win);
McoStatus 	removeWindows(void);
McoStatus 	updateWindowData(int changed);
McoStatus 	DoAlwaysUpdate(Boolean &Changed);
McoStatus 	DoDialogEvent(EventRecord *theEvent,int16 *flag,Boolean &finish,Boolean &Changed, Boolean TimerEvent);
McoStatus 	DoDocumentCommand(WindowCode *wc,WindowCode *wc2,Document *doc,void **data,Boolean *finish,Boolean *changed);
McoStatus 	DoCommand(WindowCode *wc,int num_codes,Point clickPoint,void *data,Boolean *finish,Boolean *changed);

McoStatus 	InitMenues(void);
McoStatus 	ClearMenues(void);
McoStatus 	SetMenu(void);
McoStatus 	Handle_menu(int32 menu_choice,Boolean *finish,Boolean *changed);
Boolean 	DoKeyEvent(EventRecord *theEvent,Boolean *finish,Boolean *Changed);
McoStatus 	AllEvents(void);

};

#endif