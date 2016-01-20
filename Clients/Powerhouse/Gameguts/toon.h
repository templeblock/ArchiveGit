#ifndef _TOON_H_
#define _TOON_H_
#include "wintoon.h"
#include "macros.h"
#include "sprite.h"
#include "sound.h"

#define ALLEVENTS 0xFFFFFFFFL
// channel 0 used for transitional sound as well as sound tracks
#define SOUND_CHANNEL 0

#define MAX_FILE_SPEC	32	// max length of filenames used w/o paths
typedef char FILE_SPEC[MAX_FILE_SPEC];

typedef struct _animevent
{
	DWORD	dwFrame;	// 4
	SHOTID	idHotSpot;	// 4
	BOOL	bTriggered;	// 2
} ANIMEVENT, FAR *LPANIMEVENT;

class FAR CHotSpot
{     
public:
	CHotSpot();
	~CHotSpot();

	BOOL GetEraseRect(LPRECT lpRect, BOOL bIgnoreVisibleFlag = FALSE);
	BOOL IsLooping();
	       
public:
// Flag values
	SHOTID	m_idHotSpot;			// 4 - id of hotspot
	int		 m_nEndShots;			// 2 - number of shots in m_pIdEnd
	LPSHOTID m_pIdEnd;				// 4 - jump to 'id' when this animation done
	int		 m_nShowShots;			// 2 - number of shots in m_pIdEnd
	LPSHOTID m_pIdShow;				// 4 - show these sprites when animation done
	int		 m_nHideShots;			// 2 - number of shots in m_pIdHide
	LPSHOTID m_pIdHide;				// 4 - hide these sprite when animation done
	int		 m_nEnableShots; 		// 2 - number of shots in m_pIdEnable
	LPSHOTID m_pIdEnable; 			// 4 - enable these sprites when animation done
	int		 m_nDisableShots; 		// 2 - number of shots in m_pIdDisable
	LPSHOTID m_pIdDisable;			// 4 - disable these sprites when animation done
	int		 m_nPreHideShots; 		// 2 - number of shots in m_pIdPreHide
	LPSHOTID m_pIdPreHide;			// 4 - hide these sprites before playing animation
	int		 m_nPreShowShots; 		// 2 - number of shots in m_pIdPreShow
	LPSHOTID m_pIdPreShow;			// 4 - show these sprites before playing animation
	int		 m_nPreEnableShots;  	// 2 - number of shots in m_pIdPreEnable
	LPSHOTID m_pIdPreEnable;		// 4 - enable these sprites when animation done
	int		 m_nPreDisableShots;  	// 2 - number of shots in m_pIdPreDisable
	LPSHOTID m_pIdPreDisable;		// 4 - disable these sprites when animation done
	int		 m_nStopShowShots;  	// 2 - number of shots in m_pIdStopShow
	LPSHOTID m_pIdStopShow;			// 4 - show these sprites when animation is stopped
	SHOTID	m_idSequence;			// 4 - id of animation to sequence
	DWORD	m_dwFrom;				// 4 - start position in AVI(frames) or wave file(ms)
	DWORD	m_dwTo;					// 4 - end position in AVI(frames) or wave file(ms)
	int		m_nSequence;			// 2 - sequence order of this animation
	int		m_nSequenceNum;			// 2 - number of animations in sequence
	int		m_nCurSequence;			// 2 - current sequence state
	BOOL	m_bLoop;				// 2 - loops to beginning when finished
	BOOL	m_bVisible;				// 2 - visible? (can you see it)
	BOOL	m_bEnabled;				// 2 - enabled? (can you click on it)
	BOOL	m_bAuto;				// 2 - auto-animation (played when scene begins)
	BOOL	m_bIdle;				// 2 - idle-animation (played when nothing else happening)
	BOOL	m_bExit;				// 2 - exit-animation (played when scene exits)
	BOOL	m_bNoUndraw;			// 2 - don't undraw animation when done (performance)
	BOOL	m_bCanStop;				// 2 - is this animation interruptable?
	BOOL	m_bForceEnable;			// 2 - enable sprite to be clicked on when invisible
	BOOL	m_bButton;				// 2 - this is a special purpose button
	BOOL	m_bAutoHighlight;		// 2 - auto-highlight for button
	BOOL	m_bSprite;				// 2 - sprite-engine based animation
	BOOL	m_bBrowse;				// 2 - only show this sprite when mouse is over
	BOOL	m_bHighlighted;			// 2 - highlight state for buttons
	BOOL	m_bDisableCursor;		// 2 - disable cursor before animation is played (default is FALSE)
	BOOL	m_bNoFade;				// 2 - cannot be faded
	BOOL	m_bAlwaysEnabled;		// 2 - cannot disable this clickable
	BOOL	m_bNoHide;				// 2 - don't hide sprite when clicked on
	BOOL	m_bNoTransparency;		// 2 - don't pay attention to transparent pixels in FindHotSpot()
	BOOL	m_bUpdateNow;			// 2 - for hide, update immediately
	BOOL	m_bClipDib;				// 2 - to clip dib
	BOOL	m_bAutoClick;			// 2 - Click noise for autobutton
	BOOL	m_fVideo;				// 2 - is avi a video?
	BOOL	m_fCenterVideo;			// 2 - if video, should it be centered?
	BOOL	m_bNoIdleStop;			// 2 - don't stop idle for this hotspot
	BOOL	m_bRestartIdle;			// 2 - restart idle after command hotspot
	BOOL	m_bCycleBack;			// 2 - cycle back sprite animation
	POINT	m_ptPos;				// 4 - position of this sprite
	POINT	m_ptMovie;				// 4 - position of video
	RECT	m_rHot;					// 8 - hotspot for this sprite, if no position, this is used
	RECT	m_rClip;				// 8 - clipping rectangle for playing AVI
	FILE_SPEC m_szAVIFile;			// 32 - name of toon video
	FILE_SPEC m_szForegroundDib;	// 32 - name of foreground dib
	PDIB	m_pForegroundDib;		// 4 - foreground dib
	PDIB	m_pHighlightDib;		// 4 - highlight dib for buttons - see m_bButton
	FILE_SPEC m_szWaveFile; 		// 32 - wavefile to play instead of AVI
	FILE_SPEC m_szHintWave; 		// 32 - wavefile to play for hint
	HMCI	m_hSound;				// 2 - handle to wave file
	UINT	m_uEndDelay;			// 2 - delay before jumping to m_idEnd (millseconds)
	UINT 	m_idCommand;			// 2 - command to send to dialog when hotspot clicked
	class CSprite FAR * m_lpSprite; // 4 - sprite to be used with sprite engine (see m_bSprite)
	int		m_nCells;				// 2 - number of cells in sprite
	int		m_nSpeed;				// 2 - cells per second for sprite
	int		m_nPauseTime;			// 2 - pause between cycling of sprite
	int		m_nAnimEvents;			// 2 - number of events in m_pAnimEvents
	int		m_iScaleVideo;
	LPANIMEVENT m_pAnimEvents;		// 4 - array for event triggering
};

typedef CHotSpot FAR *PHOTSPOT;

class FAR CTransitionDesc
{
public:
	CTransitionDesc();
	~CTransitionDesc() {}
public:
	BOOL	m_fFade;
	BOOL	m_fClip;
	UINT	m_uTransition;
	int		m_nStepSize;
	int		m_nTicks;
	DWORD	m_dwTotalTime;
};
                
typedef CTransitionDesc FAR *PTRANSITIONDESC;
typedef void (CALLBACK* TOONDRAWPROC)( class CToon FAR *pToon, DWORD dwData );

class FAR CToon
{          
public:
	enum NotifyCode
	{ 
		ClickedOnHotSpot	=			0,
		ToonDone			=			1,
		AutoDone			=			2,
		ConclusionDone		=			3
	};
	enum PlayingState
	{ 
		Normal		=			0,
		Auto		=			1,
		Conclusion	=			2
	};
	enum TimerTypes
	{ 
		NoTimer			=	0,
		AutoPlayTimer	=	100,
		EndDelayTimer	=	101
	};
	enum BuildMode
	{ 
		HideHotSpot		=	0, 	// normal build stage case
		ShowHotSpot		=	1, 	// to show a particular sprite
		UpdateHotSpot	=	2	// to update the area occupied by a sprite
	};

	// constructor
	CToon(HWND hWnd, int idCursor, int idNullCursor, HINSTANCE hInstance = NULL, HPALETTE hPal = NULL);
	// destructor
	~CToon();
	
	BOOL CreateToon(int cx = 640, int cy = 480);
	BOOL Create(UINT ToonDrawMessage = 0, int nLanguageOffset = 0);
	void Init();
	void FreeUp();
	void SetSendMouseMode(BOOL fSendMouseToParent) { m_fSendMouseToParent = fSendMouseToParent; }
	void SetHintState(BOOL fHintsOn) { m_fHintsOn = fHintsOn; }

	int GetWidth() { return m_pStageDib->GetWidth(); }
	int GetHeight() { return abs(m_pStageDib->GetHeight()); } 
	HWND GetWindow() { return m_hWnd; }
	HTOON GetToonHandle() { return m_hToon; }     
	HMCI  GetToonMovie()  { return m_hMovie; } 
	
	PHOTSPOT GetHotSpot(SHOTID idHotSpot);
	CDib FAR *GetBackgroundDib() { return m_pBackgroundDib; }
	CDib FAR *GetStageDib() { return m_pStageDib; }
	CDib FAR *GetWinGDib();
	CDib FAR *UpdateWinGDib();
	void RestoreBackgroundArea(LPRECT lpRect, BOOL fUpdateDisplay);
	void GrabBackgroundArea(LPRECT lpRect, BOOL fUpdateDisplay);
	void UpdateArea(LPRECT lpRect);
	LPSTR GetContentDir(LPSTR lpContentDir);

	void StopPlaying(BOOL fStopIdleSprites = TRUE, BOOL fExiting = FALSE);
	void Reset(SHOTID iHotSpot = 0, BOOL fNoUndraw = FALSE, BOOL fEnableCursor = TRUE);
	BOOL PlayAuto(BOOL fNoDelay = FALSE);
	BOOL PlayConclusion();
	void EnableCursor(BOOL fEnable);
	HPALETTE GetPalette();
	void SetBackground(LPSTR lpFileName);
	BOOL LoadBackground(LPSTR lpFileName, UINT uTransition = 0, int nTicks = -1, int nStepSize = -1, DWORD dwTotalTime = 0, LPRECT lpTransClipRect = NULL, TOONDRAWPROC lpDrawProc = NULL, DWORD dwData = 0 );
	BOOL MoveHotSpot(LPCTSTR lpForegroundDib, int x, int y, BOOL fRelative = FALSE);
	BOOL OnHotSpot(LPCTSTR lpForegroundDib);
	BOOL ModifyHotSpot(UINT idCmd, BOOL fShow, BOOL fEnable);
	void EnableClickables(BOOL fEnable) { m_fClickablesEnabled = fEnable; }
	         
	// the following messages go to the main application window
	// so the main app must have a way to get to the CToon object
	// so that it can call these functions (hWnd may be different
	// than the hWnd passed in the constructor
	BOOL OnQueryNewPalette(HWND hWnd);
	void OnPaletteChanged(HWND hWnd, HWND hwndPaletteChanged);
	
	// the following messages go to the window the Toon is displayed in
	void OnPaint();
	void OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(int x, int y, UINT keyFlags);
	void OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnRButtonUp(int x, int y, UINT keyFlags);
	void OnMouseMove(int x, int y, UINT keyFlags);
	UINT OnMCINotify(UINT message, WORD wDeviceID);
	BOOL OnSetCursor(HWND hWndCursor, UINT codeHitTest, UINT msg);
	BOOL OnToonDraw(HTOON hToon, LONG lFrame);
	void OnTimer(UINT idTimer);
	void OnSpriteNotify(class CSprite FAR* lpSprite, SPRITE_NOTIFY Notify);
	void OnAppActivate(BOOL fActivate);
	void ShowCursor(BOOL fShow);

	virtual BOOL OnNotify(CToon::NotifyCode code) { return TRUE; }
	void TransCallback(DWORD dwIteration, DWORD dwTotalIterations);
	BOOL FindContent(LPSTR lpFileSpec, BOOL bTryCommon, LPSTR lpReturnName);
	     
	BOOL DrawHotSpot(int x, int y, BOOL bShow = TRUE, BOOL bUpdate = TRUE);
	BOOL SetHotSpotFG(int x, int y, LPCTSTR lpForegroundDib);
	     
protected:	
	virtual HWND GetParentDlg() { return ::GetParent(m_hWnd); }
	BOOL LoadData(LPCTSTR lpBGDib);
	BOOL LoadJumpTable();
	void BuildStage(SHOTID idHotSpot = 0, BuildMode mode = CToon::HideHotSpot, BOOL fUpdateDisplay = FALSE, LPRECT lpUpdateRect = NULL);
	int FindHotSpot(int x, int y, BOOL fSearchAll = FALSE);
	int FindHotSpot(LPCTSTR lpForegroundDib);
	int FindHotSpot(UINT idCmd);
	BOOL OnHotSpot(SHOTID idHotSpot);
	BOOL LoadForegroundDibs(BOOL fDecodeRLE = FALSE);
	BOOL CheckSequence(SHOTID idSequence, int nSequence);
	void ResetSequence();
	void ChangeSpriteStates(PHOTSPOT pHotSpot, BOOL fNormalCompletion = FALSE, BOOL fEventTrigger = FALSE, BOOL fUpdateDisplay = TRUE);
	BOOL SetTimer(TimerTypes type = NoTimer, UINT uTimeOut = 0);
	BOOL RealizePalette(BOOL bForceBackground = FALSE);
	void FadeIn();
	void FadeOut();
	void StartIdleSprites();
	void StopIdleSprites();
	void HandleAnimationDone();
	BOOL JumpToRandomAnim(LPSHOTID lpAnimID, int nNumShots);
	BOOL StartIdleAnimation();
	BOOL StartIdle();
	BOOL Initialize(HDC hDCIn = NULL);
	void EndInitialize();
	BOOL InitIdleSprites();
	void TriggerEvents(SHOTID idHotSpot, DWORD dwFrame = ALLEVENTS, BOOL fUpdateDisplay = TRUE);
	BOOL HandleButtonHotSpot(SHOTID idHotSpot);
	void HandleBrowseHotSpot(SHOTID idHotSpot);
	void HandleHints(SHOTID idHotSpot);
	BOOL PlayClick();
	BOOL IsOnHotSpot(LPPOINT lpPoint);

public:	
	CTransitionDesc m_LeftTransition;
	CTransitionDesc m_RightTransition;
	CTransitionDesc m_JumpTransition;
	CTransitionDesc m_SceneTransition;
	int				m_nJumpSceneNo;
	RECT			m_rGlobalClip;
	FILE_SPEC		m_szAVIFile;
	FILE_SPEC		m_szWaveFile;
	POINT			m_ptMovie;
	int				m_iAutoPlayDelay;
	int				m_idSoundTrack;
	FILE_SPEC		m_szSoundTrack;
	BOOL			m_fKeepSoundTrack;
	BOOL			m_fVideo;
	BOOL			m_fCenterVideo;
	BOOL			m_fStartIdleFirst;
	int				m_iScaleVideo;
	FNAME			m_szPath;
	BOOL			m_fReadOnly;

protected: 
	HWND		m_hWnd;
	HTOON		m_hToon;
	HINSTANCE 	m_hInstance;
	HPALETTE 	m_hPal;
	HPALETTE 	m_hToonPal;
	FILE_SPEC	m_szBGDib;				 
	UINT		m_idTimer;
	UINT		m_ToonDrawMessage;
	int			m_nSceneNo;
	int			m_iAnimate;
	int			m_iPaintMethod;
	int			m_iPaintList;
	int			m_TransparentIndex;
	int			m_nHotSpots;
	int			m_nLanguageOffset;
	PHOTSPOT 	m_pHotSpots;
	PDIB		m_pBackgroundDib;
	PDIB		m_pStageDib;
	CDib		m_WinGDib;
	HMCI		m_hMovie; 
	HMCI		m_hSound;
	HCURSOR 	m_hHotSpotCursor;
	HCURSOR		m_hNullCursor;
	PlayingState m_PlayingState;
	SHOTID	 	m_idCurHotSpot; 
	SHOTID		m_idButtonHotSpot;
	SHOTID		m_idBrowseHotSpot;
	SHOTID		m_idHintHotSpot;
	SHOTID		m_idLastHintHotSpot;
	LONG		m_lLastFrame;
	LONG		m_lTotalDropped;
	DWORD		m_dwHintTime;
	DWORD		m_dwStartVol;
	DWORD 		m_dwLastDown;
	RECT		m_rSrcOrig;
	class CAnimator FAR *m_pAnimator;
	BOOL		m_fPlaying;
	BOOL		m_fCursorEnabled;
	BOOL 		m_fToonSetup;
	BOOL		m_fSendMouseToParent;
	BOOL		m_bIdleSprites;
	BOOL		m_fInitialized;
	BOOL		m_fHintsOn;
	BOOL		m_fSoundStarted;
	BOOL		m_fClickablesEnabled;
	BOOL		m_fSoundTrack;
	BOOL		m_fCursorVisible;
	BOOL		m_fCloseMovie;
	BOOL		m_fIdleSpritesOn;
	BOOL		m_fCursorVisibleState;
	BOOL		m_fHasHotSpots;
	BOOL		m_fInInitialize;
	CTransitionDesc m_Transition;
	CSound		m_Sound;
	RGBQUAD		m_rgbLastColors[256];
};

typedef CToon FAR *PTOON;

#endif // _TOON_H_
