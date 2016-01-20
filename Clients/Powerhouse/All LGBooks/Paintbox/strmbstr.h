#ifndef _STRMBSTR_H_
#define _STRMBSTR_H_

#include "pain.h"

#define		MAX_DROPS		12
#define		DROP_TIMER		111
#define		MAX_STRIPES		8


class FAR CStormbusterScene : public CGBScene
{
public:

	enum ObjectState
	{
		StateUnused,
		StateUsed,
		StateGoodPick,
	};

	enum WalkDirection
	{
		WALKLEFT,
		WALKRIGHT,
	};

	typedef struct _objectlist
	{
		FNAME			m_szImage;
		LPSPRITE		m_lpSprite;
		int 			m_nTotalCells;
		BOOL			m_fOneToCatch;
		ObjectState		m_nState;
		int				m_nIndex;
		int				m_nColor;
	
	} ObjectList, FAR * LPObjectList;

	typedef struct _MenuList
	{
		POINT	m_ptLoc;
		BOOL	m_fInUse;
	
	} MenuList, FAR * LPMenuList;

	typedef struct _loclist
	{
		int			x;
		int			y;
	
	} LocList, FAR * LPLoclist;

	typedef struct _levelStruct
	{
		short 	m_wChuteCount;
		LocList *m_lpLocs;
	
	} LevelParamList;

	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying
	};

	// constructors/destructor
	CStormbusterScene (int nNextSceneNo);
	~CStormbusterScene();
	virtual void ToonInitDone();
	void OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify);

private:
	// dialog procedure overridables
	BOOL OnInitDialog (HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	BOOL OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnLButtonDown (HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp (HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove (HWND hwnd, int x, int y, UINT keyFlags);
	void OnTimer (HWND hWnd, UINT id);
	UINT OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice);
	void OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void ChangeDirection (enum WalkDirection eDir);
	void OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);
	BOOL CheckObjectCollision (LPSPRITE lpSprite);
	int  SelectObject();
	void ResetObjects();
	void ResetAllObjects();
	void PlayIntro(HWND hWnd);
	void PlayLevelSuccessWave();
	void PlayClickWave (BOOL fSync=TRUE);
	int  GetLevel();
	void CreateSprites();
	void ChangeLevel (int iLevel);
	int  GetObjectIndex (LPSPRITE lpSprite);
	int  GetObjectIndexByColor (int iColor);
	void StartGame();
	void SetDibColor (PDIB pDib, int nColor);
	void PickTargetSprites();
	void ProcessRainbow (int iObjectIdx);
	int GetGoodMatchIdx (int nLastColor);
	void TakeStripeAway();

public: // add variables to use with parser here
	FNAME 		m_szPlayerBmp;
	RECT		m_rGameArea;
	RECT		m_rLeftBasket;
	RECT		m_rRightBasket;

	int 		m_nScreenLeftEdge;
	int 		m_nScreenRightEdge;
	int			m_nScreenBottom;
	int			m_nPlayerSpeed;
	int			m_nPlayerCells;
	int			m_nPainterCells;
	int			m_nMatchesNeeded;
	int			m_nObjectSpeed;
	int			m_nKeyboardDistance;

	FNAME		m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME 		m_szSuccessPlay;						// Success wave for Play mode
	FNAME		m_szGotOneWave;
	FNAME		m_szBadCatchWave;
	FNAME		m_szJumpWave;
	FNAME		m_szRainbowBmp;
	FNAME		m_szPainterBmp;
	FNAME		m_szPreviewBmp;
	FNAME		m_aszStripeBmps[MAX_STRIPES];
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level
	ObjectList	m_ObjectList[MAX_DROPS];
	int			m_nDropTimer;
	POINT		m_ptRainbowPos;
	POINT		m_ptPainterPos;
	POINT		m_ptPreviewPos; 
	POINT		m_ptStripePos[MAX_STRIPES];
	int			m_nPreviewHeight;
	BYTE		m_bSourceRed;
	BYTE		m_bSourceGreen;
	BYTE		m_bSourceBlue;
	BOOL		m_fPickInOrder;
	BOOL		m_bPenalty;

private:
	int			m_iLevel;
	int			m_nLastColor;
	int			m_nGoodPicks;
	int			m_arnPickColors[MAX_STRIPES];
	WalkDirection m_WalkDir;
	POINT		m_ptLastMouseXY;
	LPSPRITE	m_lpPlayerSprite;
	LPSPRITE	m_lpPainterSprite;
	LPSPRITE	m_lpRainbow;
	LPSPRITE	m_lpStripes[MAX_STRIPES];
	int			m_arnStripeColors[MAX_STRIPES];
	LPSPRITE	m_lpPreview[MAX_STRIPES];
	BOOL		m_fTimerStarted;
	SoundPlaying m_SoundPlaying;
};

typedef CStormbusterScene FAR * LPSTORMBUSTERSCENE;
                            
#endif // _STRMBSTR_H_
