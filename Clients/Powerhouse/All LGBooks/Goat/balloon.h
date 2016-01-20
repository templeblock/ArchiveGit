#ifndef _BALLOON_H_
#define _BALLOON_H_

#include "goat.h"

#define		MAX_BALLOONS	36
#define		DROP_TIMER		111

typedef struct _balloonlist
{
	FNAME			m_szImage;
	LPSPRITE		m_lpSprite;
	int 			m_nTotalCells;
	int 			m_nFloatCells;
	LPSPRITE		m_lpMenuSprite;
	BOOL			m_fTarget;
	int				m_nIndex;
	int				m_nShape;
	int				m_nColor;

} BalloonList, FAR * LPBalloonList;

typedef struct _MenuList
{
	POINT	m_ptLoc;
	BOOL	m_fInUse;

} MenuList, FAR * LPMenuList;


class FAR CBalloonScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying
	};

	// constructors/destructor
	CBalloonScene (int nNextSceneNo);
	~CBalloonScene();
	virtual void ToonInitDone();
	void OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	void SaveItemLoc (int iX, int iY);
	void SaveCaughtLoc (int iX, int iY);

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
	void OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);
	void CheckBalloonCollision();
	int  SelectBalloon();
	void ResetBalloons();
	void ResetAllBalloons();
	void PopBalloon(LPBalloonList lpList, LPSPRITE lpSprite, int iSel);
	void PickTargetBalloons();
	void PutOneBack();
	void PlayIntro(HWND hWnd = NULL);
	void PlayGoodPop();
	void PlayBadPop();
	void PlayLevelSuccessWave();
	int  GetLevel();
	void CreateSprites();
	void ChangeLevel(int iLevel);
	void StartGame();
	void SetDibColor (PDIB pDib, int nColor);

public: // add variables to use with parser here
	FNAME 		m_szGoatBmp;
	RECT		m_rGameArea;
	RECT		m_rGoatHand;

	int 		m_nScreenLeftEdge;
	int 		m_nScreenRightEdge;
	int			m_nScreenBottom;
	int			m_nJumpHeight;
	int			m_nGoatSpeed;
	int			m_nMatchesNeeded;
	int			m_nBalloonSpeed;
	int			m_nKeyboardDistance;
	BOOL		m_fGoatJumping;

	FNAME		m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME		m_szBadPickWave;
	FNAME		m_szGoodPickWave;
	FNAME		m_szJumpWave;
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level
	BalloonList	m_BalloonList[MAX_BALLOONS];
	MenuList 	m_MenuLocs[MAX_BALLOONS];
	int			m_nMenuLocCount;
	MenuList 	m_CaughtLocs[MAX_BALLOONS];
	int			m_nCaughtLocCount;
	int			m_nDropTimer;
	BYTE		m_bSourceRed;
	BYTE		m_bSourceGreen;
	BYTE		m_bSourceBlue;

private:
	int			m_nGoodPicks;
	int			m_nGoatJumpState;
	int			m_arnPickOrder[10];
	int			m_nSelectedShape;
	int			m_nSelectedColor;
	LPSPRITE	m_lpGoatSprite;
	SoundPlaying m_SoundPlaying;
};

typedef CBalloonScene FAR * LPBALLOONSCENE;
                            
#endif // _BALLOON_H_
