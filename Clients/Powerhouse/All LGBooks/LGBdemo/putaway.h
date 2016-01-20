#ifndef _PUTAWAY_H_
#define _PUTAWAY_H_

#include "house.h"

#define TOTAL_ITEMS		12
#define TOTAL_CHEATS	5


typedef struct _doors
{
	LPSTR		szOpenDoor;
	LPSTR		szClosedDoor;
	int			x;
	int			y;
} Doors;


typedef struct _items
{
	int			x;
	int			y;
} Items;


typedef struct _cheat
{
	int			x;
	int			y;
} Cheat;


typedef struct _itembmplist
{
	FNAME		szBump;
	FNAME		szRoom;
} ItemBumpList;


typedef struct _levelStruct
{
	short wDoorCount;
	Doors *pDoorList; 
	Items *pItemList;
	Items *pDoorOffset;
	short wCheatPeeks;
} LevelParamList;


typedef struct _masterlist
{
	LPSPRITE	m_lpSprite;
	BOOL		m_fInUse;
	BOOL		m_fInRoom;
} MasterList;


class FAR CPutAwayScene : public CGBScene
{
public:
	// constructors/destructor
	CPutAwayScene(int nNextSceneNo);
	~CPutAwayScene();
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	BOOL OnInitDialog (HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown (HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp (HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove (HWND hwnd, int x, int y, UINT keyFlags);
	void ShutAllTheDoors(); 
	void OpenAllTheDoors();
	void SelectItemToSearchFor();
	void ClearBumpList();  
	void ReshuffleDeck();
	LPSPRITE SelectItemForBox (LPSPRITE m_lpSprite, int centerX, int centerY, int *pwItemList);
	void OnTimer (HWND hWnd, UINT id);
	BOOL OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	UINT OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice );
	void PlayIntro();
	int GetLevel();
	void ChangeLevel (int iLevel);
	void PlayLevelSuccessWave();
	void RestartLevel();
	void CreateCheats();
	void CreateItemSprites();
	void CreateOpenDoorSprites();
	void CreateCloseDoorSprites();
	void PlayClickWave (BOOL fSync=TRUE);
	BOOL GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY);

public: // add variables to use with parser here

	ItemBumpList ItemBumps[15];
	FNAME		m_szSoundTrack;
	FNAME 		m_szIntroWave;
	FNAME 		m_szSuccessPlay;						// Success wave for Play mode
	FILE_SPEC	m_szSuccessLevel[MAX_SUCCESSWAVES];		// Success waves for level
	FNAME		m_szGoodPickWave;
	FNAME		m_szBadPickWave;
	BOOL		m_fAllowMoveAgain;
	
private:
	int			m_iLevel;
	HMCI		m_hSound;
	HCURSOR		m_hHotCursor;
	
	int			wItemSelected;
	short 		fIgnore;
	short		wItemMove;
	BOOL		m_fItemMove;
	BOOL		m_fItemMoveAgain;
	BOOL 		m_boShut;
	short 		wTimerCount; 
	short 		wShuffleCount;
	short 		wMatches;

	LPSPRITE	m_lpItemSprite[TOTAL_ITEMS];
	LPSPRITE	m_lpRoomSprite[TOTAL_ITEMS];
	MasterList  m_AllItemSprites[TOTAL_ITEMS];
	LPSPRITE	m_lpAllRoomSprite[TOTAL_ITEMS];
	LPSPRITE	m_lpDoorSprite[TOTAL_ITEMS];
	LPSPRITE	m_lpOpenDoorSprite[TOTAL_ITEMS];
	LPSPRITE	m_lpMainSprite;
	LPSPRITE	m_lpCheatUpSprite[TOTAL_CHEATS];
	LPSPRITE	m_lpCheatDownSprite[TOTAL_CHEATS];
	SoundPlaying m_SoundPlaying;
	
};

typedef CPutAwayScene FAR * LPPUTAWAYSCENE;
                            
#endif // _PUTAWAY_H_
