#ifndef _MAZE_H_
#define _MAZE_H_

//#include "caboose.h"

#define MAX_DECISION_PTS	50
#define MAX_SUCCESS			4
#define MAX_WARPS			4
#define NUM_WARP_LOCS		2

#define NUM_DIRECTIONS		4
#define LEFT				0
#define RIGHT				1
#define UP					2
#define DOWN				3
#define ONTOPOF				4


class FAR CWarp
{
public:
	CWarp();
	~CWarp() {};

	FILE_SPEC	m_szFileName;
	int			m_iFrequency;
	int			m_iLocation[NUM_WARP_LOCS];
	LPSPRITE	m_lpSprite[NUM_WARP_LOCS];
};
typedef CWarp FAR * LPWARP;

class FAR CStop
{
public:
	CStop();
	~CStop() {};

	FILE_SPEC	m_szFileName;
	DWORD		m_dwMinFrequency;
	DWORD		m_dwMaxFrequency;
	DWORD		m_dwMinDuration;
	DWORD		m_dwMaxDuration;
	DWORD		m_dwNextTime;
	LPSPRITE	m_lpSprite;
	int			m_iLocation;
};
typedef CStop FAR * LPSTOP;

class FAR CMaze
{     
public:
	CMaze();
	~CMaze();

public:
	FILE_SPEC	m_szBackground;		
	FILE_SPEC 	m_szSuccess;
	int			m_iAnnStart;
	int			m_iHouseStart;
	int			m_iTarget;
	int			m_iVision;
	int			m_iWarpFreq;
	int			m_iNumWarps;
	CWarp 		m_Warps[MAX_WARPS];
	CStop		m_Stop;
	int			m_nPoints;
	POINT		m_ptDecision[MAX_DECISION_PTS];
	BOOL		m_fCompleted;
};
typedef CMaze FAR *LPMAZE;

class FAR CMazeScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying
	};
	// constructors/destructor
	CMazeScene(int nNextSceneNo);
	~CMazeScene();
	void ToonInitDone();
	LPMAZE GetMaze(int nIndex) { return (m_pMazes+nIndex); }
	void OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	BOOL OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void OnTimer(HWND hWnd, UINT id);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	void OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);
	void PlayIntro();
	void PlayFinale();
	void PlaySoundTrack();
	int GetLevel();
	void ChangeLevel(int iLevel);
	void StartGame();
	void InitMaze();
	void LoadSprites();
	void UnloadMazeSprites();
	void LoadMazeSprites();
	BOOL MoveAnn(int x, int y);
	BOOL ChangeAnnDirection(int iNewDirection);
	void StartHouse();
	LPMAZE GetNewMaze();
	int GetNewDirection(int iHousePos, BOOL fCheckMovePossible = FALSE, BOOL fNoReverseCheck = FALSE);
	BOOL WarpHouse(BOOL fForceWarp = FALSE);
	BOOL CheckMove( LPSPRITE pSprite, int dx, int dy, int iSize = 0 );
	void SnapToDecisionPoint(LPPOINT lpPoint);
	void WarpAnn(void);
	void GetSpritePoint(LPSPRITE lpSprite, LPPOINT ptCenter, LPPOINT ptNew);
	BOOL CanGo(LPPOINT lpPoint, int nDirection);
	int GetDecisionPoint(LPPOINT lpCurPoint, int nDirection, LPPOINT lpNewPoint);
	int WhereIsAnn(LPPOINT lpStartPt);
	BOOL CheckCollision();
	BOOL CheckVictory();
	void DisplayStopSign();
	void RemoveStopSign();
	void StartStopSign();
	void ResetLevel();
	BOOL GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY);

public: // add variables to use with parser here
	int			m_nAnnCells;
	int			m_nHouseCells;
	int			m_nAnnCycle;
	int			m_nHouseCycle;
	int			m_nHouseSpeed;
	RECT		m_rGameArea;
	int			m_nRoadIndex;
	int			m_nMoveIncrement;
	int			m_nScanDistance;
	FILE_SPEC	m_szAnn[NUM_DIRECTIONS];
	FILE_SPEC	m_szHouse[NUM_DIRECTIONS];

	FILE_SPEC	m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME 		m_szSuccessPlay;						// Success wave for Play mode
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level 1

private:
	LPSPRITE	m_lpAnnSprite[NUM_DIRECTIONS];
	LPSPRITE	m_lpHouseSprite[NUM_DIRECTIONS];
	int			m_nAnnDirection;
	int			m_nHouseDirection;
	int			m_nMazes;
	LPMAZE		m_pMazes;
	HCURSOR		m_hNullCursor;
	HCURSOR		m_hHotCursor;
	POINT		m_ptAnnLast;
	int			m_iHouseLast;
	DWORD		m_dwLastMove;
	BOOL		m_bAnnStopped;
	BOOL		m_fIntro;
	BOOL		m_fStop;
	BOOL		m_fInWarpZone;
	BOOL		m_fHouseMoving;
	SoundPlaying m_SoundPlaying;
	LPMAZE		m_pCurrent;
	POINT		m_ptMouse;
	BOOL		m_fLevelComplete;
	BOOL		m_fMovement;
};

typedef CMazeScene FAR * LPMAZESCENE;
                            
#endif // _MAZE_H_	H