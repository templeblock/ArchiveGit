/*®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#ifndef _CANIMATE_H_
#define _CANIMATE_H_

typedef void (CALLBACK* NOTIFYPROC)(int);
typedef void (CALLBACK* PLAYSOUNDPROC)(int, LPSTR, BOOL, int);
typedef void _far *LPAOBJECT;

#define NO_X       1		// allow mouse tracking on selected axes
#define NO_Y       2

#ifndef WIN32		// SHORT version of a point.  "Win16 version"
typedef struct tagPOINTS
{
    short x;
    short y;
} POINTS;
typedef POINTS*       PPOINTS;
typedef POINTS NEAR* NPPOINTS;
typedef POINTS FAR*  LPPOINTS;
#endif


// -------------------------- Standard C interface ----------------------

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

LPAOBJECT PASCAL ACreateNewAnimator();               // master animator object
void FAR PASCAL ADeleteAnimator(LPAOBJECT theObject);
int FAR PASCAL ARegisterAobject(
	LPAOBJECT A,
	HBITMAP hObjectBitmap,
	HBITMAP hMaskBitmap,
	LPPOINT lpOffset);
int FAR PASCAL ARegisterAChild(
	LPAOBJECT A,
	int dAobject,
	HBITMAP hObjectBitmap,
	HBITMAP hMaskBitmap,
	LPPOINT lpOffset);
int FAR PASCAL AAddAobject(
	LPAOBJECT A, 
	int dAobject,
	HBITMAP hObjectBitmap,
	HBITMAP hMaskBitmap);
void FAR PASCAL ACopyBackgroundScreen(LPAOBJECT A, HWND hWnd, LPRECT lpClipRect);
void FAR PASCAL AGiveBackgroundBitmap(LPAOBJECT A, HWND hWnd, HBITMAP hNewBitmap, BOOL bTile);
void FAR PASCAL AMoveAllObjects(LPAOBJECT A);
void FAR PASCAL ADeleteAllObjects(LPAOBJECT A);
void FAR PASCAL APaintAllObjects(LPAOBJECT A);
void FAR PASCAL AMove(LPAOBJECT A, int dAobject, int X, int Y);
void FAR PASCAL ADeleteAobject(LPAOBJECT A, int dAobject);
void FAR PASCAL ADeleteInstructions(LPAOBJECT A, int dAobject);
void FAR PASCAL AJump(LPAOBJECT A, int dAobject, int X, int Y);
void FAR PASCAL APause(LPAOBJECT A, int dAobject, int time);
void FAR PASCAL AHide(LPAOBJECT A, int dAobject);
void FAR PASCAL AShow(LPAOBJECT A, int dAobject);
void FAR PASCAL AKill(LPAOBJECT A, int dAobject);
void FAR PASCAL ABeginCycle(LPAOBJECT A, int dAobject);
void FAR PASCAL ARepeatCycle(LPAOBJECT A, int dAobject);
void FAR PASCAL AStartTrack(LPAOBJECT A, int dAobject);
void FAR PASCAL AStopTrack(LPAOBJECT A, int dAobject);
void FAR PASCAL ASetSpeed(LPAOBJECT A, int dAobject, int theSpeed);
int  FAR PASCAL AGetSpeed(LPAOBJECT A, int dAobject);
void FAR PASCAL AIncSpeed(LPAOBJECT A, int dAobject);
void FAR PASCAL ADecSpeed(LPAOBJECT A, int dAobject);
void FAR PASCAL AStopAll(LPAOBJECT A);
void FAR PASCAL AStartAll(LPAOBJECT A);
BOOL FAR PASCAL AWhere(LPAOBJECT A, int dAobject, LPRECT lpRect);
int  FAR PASCAL AGetStopped();
int  FAR PASCAL AMouseInAnyObject(LPAOBJECT A);
void FAR PASCAL ASetTrackLimit(LPAOBJECT A, int dAobject, int limit);
void FAR PASCAL ASetCurrentScript(
    LPAOBJECT A,
	int 	dAobject, 
	char 	far *lpScript);
BOOL FAR PASCAL AExecuteScript( 
	LPAOBJECT	A,
	int 	dAobject, 
	char 	far *lpScript,
	char 	far *lpError,
	int 	nErrorBytes);
void FAR PASCAL AReExecuteScript(LPAOBJECT A, int dAobject);
void FAR PASCAL ASetAutoRepeat(LPAOBJECT A, int dAobject, BOOL bAutoRepeat);
void FAR PASCAL ASetNotification(
	LPAOBJECT	A,
	int 	dAobject, 
	WORD 	newMsg,
	int 	isCont, 
	int 	newWhich);
void FAR PASCAL ASetNotifyProc(LPAOBJECT A,int dAobject, NOTIFYPROC lpfnNotify);
void FAR PASCAL ASetPlaySoundProc(LPAOBJECT A,int dAobject, PLAYSOUNDPROC lpfnSoundCallback);
void FAR PASCAL ASetPlaySound(LPAOBJECT A,int dAobject, LPSTR lpSound, BOOL bLoopSound, int iChannel);
void FAR PASCAL AFlipVertical(LPAOBJECT A, int dAobject);
void FAR PASCAL AFlipHorizontal(LPAOBJECT A, int dAobject);
void FAR PASCAL AActivateFrame(LPAOBJECT A, int dAobject, int nFrame, BOOL bActivate);
void FAR PASCAL AActivateChildFrame(LPAOBJECT A, int dAobject, int Child, int nFrame, BOOL bActivate);
void FAR PASCAL ASetFramesPerSec(LPAOBJECT A, int dAobject, int iFmsPerSec);
void FAR PASCAL ASetChildFramesPerSec(LPAOBJECT A, int dAobject, int Child, int iFmsPerSec);
void FAR PASCAL ASetChildFrameOrder(int dAobject, int Child, int frame, int order);
void FAR PASCAL ASetFrameOffset(LPAOBJECT A, int dAobject, int frame, int x, int y);
void FAR PASCAL ASetChildFrameOffset(LPAOBJECT A, int dAobject, int Child, int frame, int x, int y);
void FAR PASCAL ASetAllFrameOffsets(LPAOBJECT A, int dAobject, LPPOINTS lpOffsets, int nPoints);
HBITMAP FAR PASCAL AMergeToBackground(HBITMAP hBackBitmap, HBITMAP hObjectBitmap);
HBITMAP FAR PASCAL ARepairBackgroundBitmap(
	LPAOBJECT A, 
	HBITMAP hBackBitmap, 
	RECT rSrcRepair, 
	RECT rDstRepair,
	bool bGetPrevious);
void FAR PASCAL ARepaintBackground(LPAOBJECT A);
void FAR PASCAL APlayAllSounds(LPAOBJECT A);

#ifdef __cplusplus
}            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

#endif

