#ifndef _VTABLE_H_
#define _VTABLE_H_

#define EVENT_END		0
#define EVENT_LEFT		1
#define EVENT_UP		2
#define EVENT_RIGHT		3
#define EVENT_DOWN		4
#define EVENT_HOME		5
#define EVENT_TIMEOUT	6

#define SHOT_PREV		0xEFFFFFFF
#define SHOT_STOP		0xDFFFFFFF
#define SHOT_TEST		0xCFFFFFFF
#define MAX_SHOTNAME_LEN 16

//**********************************************************************
// Video 'Shot' definition
//**********************************************************************
typedef long SHOTID;
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
	long	lFlags;
	int		iLastEvent;
	BOOL	bGoesPrev;
	int		fHotspotDisabled;
	} SHOT, HUGE * LPSHOT;
typedef LPSHOT FAR * LPPSHOT;

//**********************************************************************
// Video 'Shot' table definition
//**********************************************************************
#define MAX_TESTVALUES 10

typedef struct _video
	{
	HMCI hDevice;
	long lFrames;
	WORD wDisk;
	LPSHOT lpAllShots;
	int iNumShots;
	SHOTID lCurrentShot;
	SHOTID lPrevShot;
	SHOTID lLastShotToPlay;
	long lPrevFrame;
	LPLONG lpSwitches;
	ITEMID idLoopTimer;
	SHOTID lTimerCurrentShot;
	SHOTID lTimerJumpShot;
	long   lTimerJumpCount;
	int iZoomFactor;
	long lTestValue[MAX_TESTVALUES];
	BOOL bAutoplayUsed;
	} VIDEO, FAR * LPVIDEO;

#endif // _VTABLE_H_
