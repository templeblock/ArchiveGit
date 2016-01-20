#ifndef _COALDROP_H_
#define _COALDROP_H_

#include "cab.h"

#define		MAX_COALPIECES	20
#define		MAX_CHUTES		4
#define		DROP_TIMER		111

enum ObjectState
{
	StateUnused,
	StateUsed,
	StateGoodPick,
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


class FAR CCoalDropScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying
	};

	// constructors/destructor
	CCoalDropScene (int nNextSceneNo);
	~CCoalDropScene();
	virtual void ToonInitDone();
	void OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify);

private:
	// dialog procedure overridables
	BOOL OnInitDialog (HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnLButtonDown (HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp (HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove (HWND hwnd, int x, int y, UINT keyFlags);
	BOOL OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	void OnTimer (HWND hWnd, UINT id);
	UINT OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice);
	void OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void SetPlayerCell (int iX);
	void OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);
	void CheckObjectCollision (LPSPRITE lpSprite);
	LPSTR GetPathName (LPSTR lpPathName, LPSTR lpFileName);
	int  SelectObject();
	void ResetObjects();
	void ResetAllObjects();
	void PlayIntro();
	void PlayIntroWave (HWND hWnd);
	void PlayLevelSuccessWave(BOOL bWait);
	void PlayClickWave (BOOL fSync=TRUE);
	int  GetLevel();
	void CreateSprites();
	void ChangeLevel (int iLevel);
	int  GetObjectIndex (LPSPRITE lpSprite);
	void StartGame();
	void UpdateSpriteScore (LPSPRITE lpScore1, LPSPRITE lpScore2, int iScore);
	void ResetSpriteScore (LPSPRITE lpScore1, LPSPRITE lpScore2);
	LPSPRITE CreateScoreSprite (int x, int y, int iCell);

public: // add variables to use with parser here
	FNAME 		m_szPlayerBmp;
	RECT		m_rGameArea;
	RECT		m_rGoatHand;

	int 		m_nScreenLeftEdge;
	int 		m_nScreenRightEdge;
	int			m_nScreenBottom;
	int			m_nCartSpeed;
	int			m_nCartcells;
	int			m_nMatchesNeeded;
	int			m_nObjectSpeed;
	int			m_nKeyboardDistance;

	FNAME		m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME 		m_szSuccessPlay;						// Success wave for Play mode
	FNAME		m_szGotOneWave;
	FNAME		m_szJumpWave;
	FNAME		m_szScoreBmp;
	int			m_nScoreCells;
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level
	ObjectList	m_ObjectList[MAX_COALPIECES];
	int			m_nDropTimer;
	int			m_nScoreWidth;
	POINT		m_ptScore;
	POINT		m_ptChute1Score;
	POINT		m_ptChute2Score;
	POINT		m_ptChute3Score;
	POINT		m_ptChute4Score;

private:
	int			m_iLevel;
	int			m_nGoodPicks;
	int			m_nLastIdx;
	int			m_nCaughtScore;
	int			m_nChuteScores[4];
	LPSPRITE	m_lpPlayerSprite;
	LPSPRITE	m_lpCaughtScore[2];
	LPSPRITE	m_lpChute1Score[2];
	LPSPRITE	m_lpChute2Score[2];
	LPSPRITE	m_lpChute3Score[2];
	LPSPRITE	m_lpChute4Score[2];
	SoundPlaying m_SoundPlaying;
	BOOL		m_fTimerStarted;
};

typedef CCoalDropScene FAR * LPCOALDROPSCENE;
                            
#endif // _COALDROP_H_
