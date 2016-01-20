#ifndef _TOON_H_
#define _TOON_H_
#include "wintoon.h"
#include "macros.h"
#include "sprite.h"
#include "sound.h"

#define MAX_SHOTVALUES 15
#define MAX_EVENTS 5
// channel 0 used for transitional sound as well as sound tracks
#define SOUND_CHANNEL 0

typedef struct _animevent
{
	DWORD	dwFrame;
	SHOTID	idHotSpot;
	BOOL	bTriggered;
} ANIMEVENT, FAR *LPANIMEVENT;

class FAR CHotSpot
{     
public:
	CHotSpot();
	~CHotSpot();

	BOOL GetEraseRect(LPRECT lpRect, BOOL bIgnoreVisibleFlag = FALSE);
	       
public:
// Flag values
	SHOTID	m_idHotSpot;			// id of hotspot
	SHOTID	m_idEnd[MAX_SHOTVALUES];	// jump to 'id' when this animation done
	SHOTID	m_idShow[MAX_SHOTVALUES];	// show these sprites when animation done
	SHOTID	m_idHide[MAX_SHOTVALUES];	// hide these sprite when animation done
	SHOTID	m_idEnable[MAX_SHOTVALUES];	// enable these sprites when animation done
	SHOTID	m_idDisable[MAX_SHOTVALUES];// disable these sprites when animation done
	SHOTID	m_idPreHide[MAX_SHOTVALUES];// hide these sprites before playing animation
	SHOTID	m_idPreShow[MAX_SHOTVALUES];// show these sprites before playing animation
	SHOTID	m_idStopShow[MAX_SHOTVALUES];// show these sprites when animation is stopped
	SHOTID	m_idSequence;			// id of animation to sequence
	DWORD	m_dwFrom;				// start position in AVI(frames) or wave file(ms)
	DWORD	m_dwTo;					// end position in AVI(frames) or wave file(ms)
	int		m_nSequence;			// sequence order of this animation
	int		m_nSequenceNum;			// number of animations in sequence
	int		m_nCurSequence;			// current sequence state
	BOOL	m_bLoop;				// loops to beginning when finished
	BOOL	m_bVisible;				// visible? (can you see it)
	BOOL	m_bEnabled;				// enabled? (can you click on it)
	BOOL	m_bAuto;				// auto-animation (played when scene begins)
	BOOL	m_bIdle;				// idle-animation (played when nothing else happening)
	BOOL	m_bExit;				// exit-animation (played when scene exits)
	BOOL	m_bNoUndraw;			// don't undraw animation when done (performance)
	BOOL	m_bCanStop;				// is this animation interruptable?
	BOOL	m_bForceEnable;			// enable sprite to be clicked on when invisible
	BOOL	m_bButton;				// this is a special purpose button
	BOOL	m_bAutoHighlight;		// auto-highlight for button
	BOOL	m_bSprite;				// sprite-engine based animation
	BOOL	m_bBrowse;				// only show this sprite when mouse is over
	BOOL	m_bHighlighted;			// highlight state for buttons
	BOOL	m_bEnableCursor;		// enable cursor when animation is done (default is TRUE)
	BOOL	m_bNoFade;				// cannot be faded
	BOOL	m_bAlwaysEnabled;		// cannot disable this clickable
	BOOL	m_bNoHide;				// don't hide sprite when clicked on
	BOOL	m_bNoTransparency;		// don't pay attention to transparent pixels in FindHotSpot()
	POINT	m_ptPos;				// position of this sprite
	RECT	m_rHot;					// hotspot for this sprite, if no position, this is used
	RECT	m_rClip;				// clipping rectangle for playing AVI
	FNAME	m_szForegroundDib;		// name of foreground dib
	PDIB	m_pForegroundDib;		// foreground dib
	PDIB	m_pHighlightDib;		// highlight dib for buttons - see m_bButton
	FNAME	m_szWaveFile;			// wavefile to play instead of AVI
	FNAME	m_szHintWave;			// wavefile to play for hint
	HMCI	m_hSound;				// handle to wave file
	UINT	m_uEndDelay;			// delay before jumping to m_idEnd (millseconds)
	UINT 	m_idCommand;			// command to send to dialog when hotspot clicked
	class CSprite FAR * m_lpSprite; // sprite to be used with sprite engine (see m_bSprite)
	int		m_nCells;				// number of cells in sprite
	int		m_nSpeed;				// cells per second for sprite
	int		m_nPauseTime;			// pause between cycling of sprite
	ANIMEVENT m_AnimEvents[MAX_EVENTS]; // array for event triggering
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
	CToon(HWND hWnd, int idCursor, HINSTANCE hInstance = NULL, HPALETTE hPal = NULL);
	// destructor
	~CToon();
	
	BOOL CreateToon(int cx = 640, int cy = 480);
	BOOL Create(LPCTSTR lpPath, LPCTSTR lpBGDib, UINT ToonDrawMessage = 0);
	void Init();
	void FreeUp();
	void SetSendMouseMode(BOOL fSendMouseToParent) { m_fSendMouseToParent = fSendMouseToParent; }
	void SetHintState(BOOL fHintsOn) { m_fHintsOn = fHintsOn; }

	int GetWidth() { return m_pStageDib->GetWidth(); }
	int GetHeight() { return abs(m_pStageDib->GetHeight()); } 
	HWND GetWindow() { return m_hWnd; }
	HTOON GetToonHandle() { return m_hToon; }
	PHOTSPOT GetHotSpot(SHOTID idHotSpot);
	CDib FAR *GetBackgroundDib() { return m_pBackgroundDib; }
	CDib FAR *GetStageDib() { return m_pStageDib; }
	CDib FAR *GetWinGDib();
	CDib FAR *UpdateWinGDib();
	void RestoreBackgroundArea(LPRECT lpRect, BOOL fUpdateDisplay);

	void StopPlaying(BOOL fExiting = FALSE);
	void Reset(SHOTID iHotSpot = 0, BOOL fNoUndraw = FALSE, BOOL fEnableCursor = TRUE);
	BOOL PlayAuto(BOOL fNoDelay = FALSE);
	BOOL PlayConclusion();
	void EnableCursor(BOOL fEnable);
	HPALETTE GetPalette();
	BOOL LoadBackground(LPSTR lpFileName, UINT uTransition = 0, int nTicks = -1, int nStepSize = -1, LPRECT lpTransClipRect = NULL);
	BOOL MoveHotSpot(LPCTSTR lpForegroundDib, int x, int y, BOOL fRelative = FALSE);
	BOOL OnHotSpot(LPCTSTR lpForegroundDib);
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
	void OnMouseMove(int x, int y, UINT keyFlags);
	UINT OnMCINotify(UINT message, WORD wDeviceID);
	BOOL OnSetCursor(HWND hWndCursor, UINT codeHitTest, UINT msg);
	BOOL OnToonDraw(HTOON hToon, LONG lFrame);
	void OnTimer(UINT idTimer);
	void OnSpriteNotify(class CSprite FAR* lpSprite, SPRITE_NOTIFY Notify);

	virtual BOOL OnNotify(CToon::NotifyCode code) { return TRUE; }
	void TransCallback(DWORD dwIteration, DWORD dwTotalIterations);
	 
protected:	
	virtual HWND GetParentDlg() { return ::GetParent(m_hWnd); }
	BOOL LoadData(LPCTSTR lpBGDib);
	BOOL LoadJumpTable();
	void BuildStage(SHOTID idHotSpot = 0, BuildMode mode = CToon::HideHotSpot, BOOL fUpdateDisplay = FALSE);
	int FindHotSpot(int x, int y);
	int FindHotSpot(LPCTSTR lpForegroundDib);
	BOOL OnHotSpot(SHOTID idHotSpot);
	BOOL LoadForegroundDibs(BOOL fDecodeRLE = FALSE);
	BOOL CheckSequence(SHOTID idSequence, int nSequence);
	void ResetSequence();
	void ChangeSpriteStates(PHOTSPOT pHotSpot, BOOL fNormalCompletion = FALSE, BOOL fEventTrigger = FALSE);
	BOOL SetTimer(TimerTypes type = NoTimer, UINT uTimeOut = 0);
	BOOL RealizePalette(BOOL bForceBackground = FALSE);
	void FadeIn();
	void FadeOut();
	void DrawHotSpot(SHOTID idHotSpot, BOOL bHighlight);
	void StartIdleSprites();
	void StopIdleSprites();
	void HandleAnimationDone();
	void UpdateArea(LPRECT lpRect);
	BOOL JumpToRandomAnim(SHOTID FAR *lpAnimID);
	BOOL StartIdleAnimation();
	BOOL StartIdle();
	BOOL Initialize(HDC hDCIn = NULL);
	void EndInitialize();
	BOOL InitIdleSprites();
	void TriggerEvents(SHOTID idHotSpot, DWORD dwFrame = 0xFFFFFFFFL);

public:	
	CTransitionDesc m_LeftTransition;
	CTransitionDesc m_RightTransition;
	CTransitionDesc m_JumpTransition;
	CTransitionDesc m_SceneTransition;
	int				m_nJumpSceneNo;
	RECT			m_rGlobalClip;
	FNAME			m_szAVIFile;
	FNAME			m_szWaveFile;
	POINT			m_ptMovie;
	int				m_iAutoPlayDelay;
	int				m_idSoundTrack;
	FNAME			m_szSoundTrack;
	BOOL			m_fKeepSoundTrack;

protected: 
	HWND		m_hWnd;
	HTOON		m_hToon;
	HINSTANCE 	m_hInstance;
	HPALETTE 	m_hPal;
	HPALETTE 	m_hToonPal;
	FNAME		m_szPath;
	FNAME		m_szBGDib;				 
	UINT		m_idTimer;
	UINT		m_ToonDrawMessage;
	int			m_nSceneNo;
	int			m_iAnimate;
	int			m_iPaintMethod;
	int			m_iPaintList;
	int			m_TransparentIndex;
	int			m_nHotSpots;
	PHOTSPOT 	m_pHotSpots;
	PDIB		m_pBackgroundDib;
	PDIB		m_pStageDib;
	CDib		m_WinGDib;
	HMCI		m_hMovie; 
	HMCI		m_hSound;
	HCURSOR 	m_hHotSpotCursor;
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
	RECT		m_rSrcOrig;
	class CAnimator FAR *m_pAnimator;
	BOOL		m_fPlaying;
	BOOL		m_fCursorEnabled;
	BOOL 		m_fToonSetup;
	BOOL		m_fSendMouseToParent;
	BOOL		m_bNoTransitionClip;
	BOOL		m_bIdleSprites;
	BOOL		m_fInitialized;
	BOOL		m_fHintsOn;
	BOOL		m_fSoundStarted;
	BOOL		m_fClickablesEnabled;
	BOOL		m_fSoundTrack;
	CTransitionDesc m_Transition;
	CSound		m_Sound;
};

typedef CToon FAR *PTOON;

#endif // _TOON_H_
