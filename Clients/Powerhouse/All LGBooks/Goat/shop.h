#ifndef _SHOP_H_
#define _SHOP_H_

#include "goat.h"

#define MAX_DECISION_PTS	50
#define MAX_WARPS			4
#define NUM_WARP_LOCS		2

#define NUM_DIRECTIONS		4
#define LEFT				0
#define RIGHT				1
#define UP					2
#define DOWN				3
#define ONTOPOF				4


#define MAX_SHOP_ITEMS		12

typedef struct _MasterList
{
	FNAME		m_szImage;
//	POINT		m_ptLoc;
	FNAME		m_szNoCheckImage;
	FNAME		m_szCheckImage;
	BOOL		m_fInUse;

} MasterList, FAR * LPMasterList;

typedef struct _ObjectList
{
	POINT		m_ptLoc;
	LPSPRITE	m_lpSprite;
	BOOL		m_fFound;

	LPSPRITE	m_lpNoCheckSprite;
	LPSPRITE	m_lpCheckSprite;

} ObjectList, FAR *LPObjectList;

typedef struct _LocationList
{
	POINT	m_ptLoc;
	BOOL	m_fInUse;

} LocationList, FAR * LPLocationList;


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
	int			m_iShopperStart;
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

class FAR CShopScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying
	};
	// constructors/destructor
	CShopScene(int nNextSceneNo);
	~CShopScene();
	virtual void ToonInitDone();
	LPMAZE GetMaze(int nIndex) { return (m_pMazes+nIndex); }
	void OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	void SaveObjectBmp (LPSTR lpFilename);
	void SaveListBmp (LPSTR lpFilename, int fChecked);
	void SaveItemLoc (int iX, int iY);

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
	void PlayIntro(HWND hWnd = NULL);
	void PlaySuccess();
	void PlaySoundTrack();
	int GetLevel();
	void ChangeLevel(int iLevel);
	void StartGame();
	void InitMaze();
	void LoadSprites();
	void LoadItemSprites();
	void UnloadMazeSprites();
	void LoadMazeSprites();
	BOOL MoveShopper (int dx, int dy);
	BOOL CheckMove( LPSPRITE lpSprite, RECT rArea, RECT rSprite );
	void ChangeShopperDirection (int iNewDirection);
	void SelectItemToSearchFor();
	LPMAZE GetNewMaze();
	void WarpAnn();
	void GetSpritePoint(LPSPRITE lpSprite, LPPOINT ptCenter, LPPOINT ptNew);
	BOOL CanGo(LPPOINT lpPoint, int nDirection);
	int GetDecisionPoint(LPPOINT lpCurPoint, int nDirection, LPPOINT lpNewPoint);
	BOOL CheckCollision();
	void DisplayStopSign();
	void RemoveStopSign();
	void StartStopSign();
	void ResetLevel();
	BOOL GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY);
	void CheckListItem (LPSPRITE lpSprite, BOOL bCheck);
	void ResetSprites();
	void GetItemLocation (int * lpiX, int * lpiY);

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

	FILE_SPEC	m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME		m_szFindItemWave;
	FNAME		m_szWrongItemWave;
	FILE_SPEC	m_szSuccessWave;
	int			m_nMasterLoadCount;
	int			m_nItemLoadCount;
	int			m_nLocationCount;
	int			m_iItemsNeeded;
	POINT		m_ptStartList;
	int			m_iVertSpace;
	BOOL		m_fFindInOrder;

private:
	LPSPRITE	m_lpShopSprite[NUM_DIRECTIONS];
	int			m_nDirection;
	int			m_nMazes;
	LPMAZE		m_pMazes;
	POINT		m_ptAnnLast;
	DWORD		m_dwLastMove;
	BOOL		m_fIntro;
	BOOL		m_fStop;
	BOOL		m_fInWarpZone;
	BOOL		m_fLevelComplete;
	SoundPlaying m_SoundPlaying;
	LPMAZE		m_pCurrent;
	POINT		m_ptMouse;
	LocationList m_LocationList[MAX_SHOP_ITEMS];
	MasterList	m_MasterList[MAX_SHOP_ITEMS];
	ObjectList	m_ObjectList[MAX_SHOP_ITEMS];
	int			m_iItemsCaught;
	int			m_iItemSelected;
};

typedef CShopScene FAR * LPSHOPSCENE;

#endif // _SHOP_H_
