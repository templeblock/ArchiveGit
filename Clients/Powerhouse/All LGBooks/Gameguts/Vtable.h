#ifndef _VTABLE_H_
#define _VTABLE_H_

#define EVENT_END		0
#define EVENT_LEFT		1
#define EVENT_UP		2
#define EVENT_RIGHT		3
#define EVENT_DOWN		4
#define EVENT_HOME		5
#define EVENT_INS		6
#define EVENT_DEL		7
#define EVENT_TIMEOUT	8

#define SHOT_PREV		0x7FFFFFFF
#define SHOT_PREV2		0x6FFFFFFF
#define SHOT_STOP		0x5FFFFFFF
#define SHOT_TEST		0x4FFFFFFF
#define SHOT_NEXT		0x3FFFFFFF
#define SHOT_THIS		0x2FFFFFFF
#define SHOT_CONT		0x1FFFFFFF

#define MAX_SHOTNAME_LEN 24

#define EDIT_FLAG_NOUSED	0x00000000	// Not currrently used by the editor
#define EDIT_FLAG_INTREE	0x00000001	// Loaded into the list tree

//**********************************************************************
// Video flag values
//**********************************************************************
#define VFLAG_GIVEMOUSE 0x00000001

//**********************************************************************
// Registers
//**********************************************************************
#define MAX_REGISTERS  85

//**********************************************************************
// Video 'Shot' definition
//**********************************************************************
typedef long SHOTID;
typedef SHOTID FAR *LPSHOTID;
typedef struct _shot
	{
public:
	char	szShotName[MAX_SHOTNAME_LEN];
	SHOTID	lShotID;
	ATOM	aShotID;
	WORD	wDisk;
	long	lStartFrame;
	long	lEndFrame;
	long	lIndentFrames;
	long	lIndexReg;
	long	lRegNum;
	long	lRegValue;
	long	lCompReg;
	long	lCompValue;
	long	lPromptTime;
	SHOTID  lPromptShot;
	SHOTID	lEndShot;
	long	lEndCount;
	long	lEndLoop;
	BOOL	bLeftIsSubShot;
	SHOTID	lLeftShot;
	long	lLeftCount;
	long	lLeftHotspot;
	BOOL	bUpIsSubShot;
	SHOTID	lUpShot;
	long	lUpCount;
	long	lUpHotspot;
	BOOL	bRightIsSubShot;
	SHOTID	lRightShot;
	long	lRightCount;
	long	lRightHotspot;
	BOOL	bDownIsSubShot;
	SHOTID	lDownShot;
	long	lDownCount;
	long	lDownHotspot;
	BOOL	bHomeIsSubShot;
	SHOTID	lHomeShot;
	long	lHomeCount;
	long	lHomeHotspot;
	BOOL	bInsIsSubShot;
	SHOTID	lInsShot;
	long	lInsCount;
	long	lInsHotspot;
	BOOL	bDelIsSubShot;
	SHOTID	lDelShot;
	long	lDelCount;
	long	lDelHotspot;
	long	lFlags;
	long	lValue;
	int		iLastEvent;
	BOOL	bGoesPrev;
	int		fHotspotDisabled;
	BYTE    fEditFlag;
#ifndef WIN32	
	char	cPadding[68];		// Padding to make length 256 for Win 3.1
#endif	
	} SHOT, HUGE * LPSHOT;
typedef LPSHOT FAR * LPPSHOT;

//**********************************************************************
// Video 'Shot' table definition
//**********************************************************************
typedef BOOL (CALLBACK* EVENTPROC)( HWND, struct _video FAR *, int );

typedef struct _video
	{
	HWND hWnd;
	HMCI hDevice;
	long lFrames;
	WORD wDisk;
	LPSHOT lpAllShots;
	int iNumShots;
	SHOTID lCurrentShot;
	SHOTID lPrevShot;
	SHOTID lLastShot;
	SHOTID lLastShotToPlay;
	long lPrevFrame;
	LPLONG lpSwitches;
	ITEMID idLoopTimer;
	SHOTID lTimerCurrentShot;
	SHOTID lTimerJumpShot;
	long   lTimerJumpCount;
	SHOTID lPromptShot;
	DWORD  dwPromptStartTime;
	DWORD  dwPromptEndTime;
	ITEMID idPromptTimer;
	int iZoomFactor;
	long lRegisterValue[MAX_REGISTERS];
	BOOL bAutoplayUsed;
	BOOL bCenterSaved;
	BOOL bNotifyParent;
	int iCenterX;
	int iCenterY;
	EVENTPROC lpEventProc;
	} VIDEO, FAR * LPVIDEO;

#endif // _VTABLE_H_
