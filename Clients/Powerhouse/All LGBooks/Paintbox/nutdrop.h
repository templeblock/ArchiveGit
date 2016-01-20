#ifndef _NUTDROP_H_
#define _NUTDROP_H_

#include "pain.h"

#define		MAX_PIECES		20
#define		DROP_TIMER		111

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


class FAR CNutDropScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying
	};

	// constructors/destructor
	CNutDropScene (int nNextSceneNo);
	~CNutDropScene();
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
	void CheckObjectCollision (LPSPRITE lpSprite);
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
	void StartGame();
	void ResetSpriteScore (LPSPRITE lpScore1, LPSPRITE lpScore2);
	void UpdateSpriteScore (LPSPRITE lpScore1, LPSPRITE lpScore2, int iScore);
	LPSPRITE CreateScoreSprite (int x, int y, int iCell);

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
	int			m_nMatchesNeeded;
	int			m_nObjectSpeed;
	int			m_nKeyboardDistance;
	int			m_nScoreCells;
	int			m_nScoreWidth;

	FNAME		m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME 		m_szSuccessPlay;						// Success wave for Play mode
	FNAME		m_szGotOneWave;
	FNAME		m_szJumpWave;
	FNAME		m_szScoreBmp;
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level
	ObjectList	m_ObjectList[MAX_PIECES];
	int			m_nDropTimer;
	POINT		m_ptCaughtScore;

private:
	int			m_iLevel;
	int			m_nLastIdx;
	int			m_nGoodPicks;
	WalkDirection m_WalkDir;
	POINT		m_ptLastMouseXY;
	LPSPRITE	m_lpPlayerSprite;
	LPSPRITE	m_lpCaughtScore[2];
	BOOL		m_fTimerStarted;
	SoundPlaying m_SoundPlaying;
};

typedef CNutDropScene FAR * LPNUTDROPSCENE;
                            
#endif // _NUTDROP_H_
