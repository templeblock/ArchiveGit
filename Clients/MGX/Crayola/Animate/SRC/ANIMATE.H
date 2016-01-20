/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#ifndef _ANIMATE_H_
#define _ANIMATE_H_

typedef void (CALLBACK* NOTIFYPROC)(int);
typedef void (CALLBACK* PLAYSOUNDPROC)(int, LPSTR, BOOL, int);
class CAnimateObj;

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

#define USE_TIMERCOUNT	1

// -----------------------master Animator-----------------------------------

class CAnimator                          // master orchestrator or all animation
{   friend class CAnimateObj;

public:
	CAnimator();
	~CAnimator();

	int  RegisterAobject(
		HBITMAP hObjectBitmap, 
		HBITMAP hMaskBitmap,
		LPPOINT lpOrigin);

	int  RegisterAChild(
		int dAobject, 
		HBITMAP hObjectBitmap, 
		HBITMAP hMaskBitmap,
		LPPOINT lpOrigin);

	int  AddAobject(int dAobject, HBITMAP hObjectBitmap, HBITMAP hMaskBitmap);
	void CopyBackgroundScreen(HWND hWnd, LPRECT lpClipRect);
    void GiveBackgroundBitmap(HWND hWnd, HBITMAP hBitmap, BOOL bTile);
	void MoveAllObjects();
	void PaintAllObjects();
	
	void DeleteAobject(int dAobject);
	void DeleteAllObjects();
	void DeleteInstructions(int dAobject);

    static HBITMAP MergeToBackground(HBITMAP hBackBitmap, HBITMAP hObjectBitmap);
	HBITMAP RepairBackgroundBitmap(
		HBITMAP hBackBitmap, 
		RECT rSrcRepair, 
		RECT rDstRepair, 
		bool bGetPrevious);
	void RepaintBackground();
	void PlayAllSounds();
	
	void SetCurrentScript(
		int 	dAobject, 
		char 	far *lpScript);
	bool ExecuteScript(
		int 	dAobject, 
		char 	far *lpScript,
		char 	far *lpError,
		int 	nErrorBytes);
	bool ExecuteScriptSetup(
		int 	dAobject, 
		char 	far *lpScript,
		char 	far *lpError,
		int 	nErrorBytes);
    void ReExecuteScript(int dAobject);
	void Pause(int dAobject, int time);
	void Move(int dAobject, int X, int Y);
	void MovePoints(
		int dAobject, 
		int nPoints, 
		LPPOINT lpPoints);
	void Hide(int dAobject);
	void Show(int dAobject);
	void Jump(int dAobject, int X, int Y);
	void Kill(int dAobject);
	void BeginCycle(int dAobject);
	void RepeatCycle(int dAobject);
	void StartTrack(int dAobject);
	void StopTrack(int dAobject);
	void SetTrackLimit(int dAobject, int limit);
	int  MouseInAnyObject();
	void SetSpeed(int dAobject, int newSpeed);
	int  GetSpeed(int dAobject);
	void IncSpeed(int dAobject);
	void DecSpeed(int dAobject);
	int  GetStopped()  { return stopped;  };
	void StopAll()     { stopped = TRUE;  };
	void StartAll();
	void SetCurrentFrame(int dAobject, int nFrame);
    void SetCurrentChildFrame(int dAobject, int nChild, int nFrame);
	BOOL Where(int dAobject, LPRECT lpRect);
	void SetNotification(
		int dAobject, 
		WORD newMsg, 
		int isCont, 
		int newWhich);
	void SetNotifyProc(
		int dAobject, 
		NOTIFYPROC lpfnNotify);
	void SetPlaySoundProc(
		int dAobject,
		PLAYSOUNDPROC lpfnSoundCallback);
	void SetPlaySound(
		int dAobject,
		LPSTR lpSound,
		BOOL bLoopSound,
		int iChannel);
	void FlipVertical(int dAobject);
	void FlipHorizontal(int dAobject);
	int  GetNumObjects();
	void ActivateFrame(
		int dAobject, 
		int nStartFrame, 
		int nEndFrame, 
		BOOL bActivate);
	void ActivateChildFrame(
		int dAobject, 
		int Child, 
		int nStartFrame, 
		int nEndFrame, 
		BOOL bActivate);
    void SetFramesPerSec(int dAobject, int iFmsPerSec);
    void SetChildFramesPerSec(int dAobject, int Child, int iFmsPerSec);
    void SetFrameOrder(int dAobject, int frame, int order);              
    void SetChildFrameOrder(int dAobject, int Child, int frame, int order);              
    void SetFrameOffset(int dAobject, int frame, int x, int y);
    void SetAllFrameOffsets(int dAobject, LPPOINTS lpOffsets, int nPoints);
    void SetChildFrameOffset(int dAobject, int Child, int frame, int x, int y);
    void SetRepeatCycle(int dAobject, bool bRepeat);
    void SetAutoRepeat(int dAobject, bool bAutoRepeat);
	void WaitLastFrame(int dAobject);

private:	
	HDC     virginMap;		// virgin copy of the background screen
	HWND    hBackWnd;		// the window for these objects
	BOOL    bBackDestroy;	// does the window need to be destroyed?
	
	HBITMAP hvirginBitmap;
	
	CAnimateObj *headA;		// first in a linked list of objects
	
	int		lastANum;		// object "handles"
	int		stopped;		// to stop all objects from moving
	
private:
	int  ResizeLoadBitmap(int newWidth, int newHeight);
	CAnimateObj *FindAobject(int dAobject);
	int GetChildObjectID(int dAobject, int Child);
	void DeleteAobjects(CAnimateObj *newOne);
	void HideAndDeleteAobjects(CAnimateObj *newOne);
};

// ---------------------------------------------- ----------------------

#define NO_X       1		// allow mouse tracking on selected axes
#define NO_Y       2


#endif


