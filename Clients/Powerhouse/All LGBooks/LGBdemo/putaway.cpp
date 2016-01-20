#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "house.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "houseid.h"
#include "putaway.h"
#include "putdef.h"
#include "randseq.h"


#define RESHUFFLE			200
#define TOTAL_ITEM_BMPS 	15
#define LVL_ONE_MATCHES		6		// # of matches required to complete level
#define LVL_TWO_MATCHES		12 //8
#define LVL_THREE_MATCHES	12 //10
#define SHOW_ITEM_X			305
#define SHOW_ITEM_Y			80

#define CHEAT_UP			"CHEAT.BMP" 
#define CHEAT_DOWN			"CHEATNO.BMP"


static Items ItemList6[] =
{
//  x    y
	143, 373,
	225, 373,
	180, 303,

	368, 370,
	448, 370,
	405, 303
};


static Items ItemList12[] =
{
//  x    y
	117, 308,
	167, 308,
	270, 305,
	335, 305,
	423, 305,
	503, 308,

	102, 372,
	178, 365,
	253, 370,
	345, 370,
	425, 370,
	505, 375,
};


static Doors DoorList6[] =
{
//  open door     closed door   x    y
	"6op-02.bmp", "6cl-02.bmp", 126, 361,
	"6op-03.bmp", "6cl-03.bmp", 212, 360,	
	"6op-01.bmp", "6cl-04.bmp", 168, 296,	// close 4 used, worked better

	"6op-05.bmp", "6cl-05.bmp", 355, 360,
	"6op-06.bmp", "6cl-06.bmp", 437, 360,
	"6op-04.bmp", "6cl-04.bmp",	398, 297
};

static Items OpenDoorOffset6[] =
{
//  x   	y
	-35,	2,
	52,		5,
	-15,	-31,

	-30,	6,
	58,		5,
	4,		-30
};


static Doors DoorList12[] =
{
//  open door      closed door    x    y
	"12op-01.bmp", "12cl-01.bmp", 102, 298,
	"12op-02.bmp", "12cl-02.bmp", 162, 298,
	"12op-05.bmp", "12cl-05.bmp", 255, 295,
	"12op-06.bmp", "12cl-06.bmp", 320, 295,
	"12op-09.bmp", "12cl-09.bmp", 408, 295,
	"12op-10.bmp", "12cl-10.bmp", 488, 298,

	"12op-03.bmp", "12cl-03.bmp",  83, 358,
	"12op-04.bmp", "12cl-04.bmp", 163, 355,
	"12op-07.bmp", "12cl-07.bmp", 238, 360,
	"12op-08.bmp", "12cl-08.bmp", 339, 359,
	"12op-11.bmp", "12cl-11.bmp", 410, 360,
	"12op-12.bmp", "12cl-12.bmp", 490, 365
};

static Items OpenDoorOffset12[] =
{
//  x   	y
	-35,	-7,
	-8,		-40,
	3,		-35,
	0,		-33,
	2,		-33,
	54,		-2,

	-41,	2,
	-28,	62,
	54,		4,
	-17,	4,
	8,		54,
	56,		0
};


static RECT rGameClipArea = {0, 0, 592, 433};

// Cheat sprite positions	
static Cheat CheatSheet[] =
{
	7, 395,
	7, 359,
	7, 323,
	7, 287,
	7, 251,
};
	

static RECT rGreenRoom = {90, 140, 210, 255};		// left room
static RECT rRedRoom   = {250, 140, 370, 255};		// middle room
static RECT rBlueRoom  = {405, 140, 525, 255};		// right room


static LevelParamList sLevel[] =
{
	// Level 1
	6,					// Door count
	&DoorList6[0],		// Door list array pntr
	&ItemList6[0],		// Item list array pntr
	&OpenDoorOffset6[0],// Door offset array pntr
	3,					// Cheat peeks

	// Level 2						   			
	12,
	&DoorList12[0],
	&ItemList12[0],
	&OpenDoorOffset12[0],
	5,

	// Level 3						   			
	12,
	&DoorList12[0],
	&ItemList12[0],
	&OpenDoorOffset12[0],
	3,
};

static int 		wLevelNum;
static int		wItemList[TOTAL_ITEMS];
static int 		wOldCount;
static LPSPRITE	RoomSprite;	
static WORD 	wRandomSeed;
static BOOL 	fDoorsLoaded;


class CPutAwayParser : public CParser
{
public:
	CPutAwayParser (LPSTR lpTableData) : CParser(lpTableData) {}

protected:
	BOOL HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPPUTAWAYSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues, int iLevel);
};  


//************************************************************************
CPutAwayScene::CPutAwayScene (int nNextSceneNo) : CGBScene (nNextSceneNo)
//************************************************************************
{
	// Unless we are changing levels, make sure we start up again at level 1
	if ( wLevelNum < 10000 )
		wLevelNum = 0;
	else
		wLevelNum -= 10000;

	m_iLevel = wLevelNum;

	RoomSprite = NULL;
	wShuffleCount = RESHUFFLE + 1;
	m_hSound = NULL;
	wMatches = 0;
	m_fItemMove = FALSE;
	m_fItemMoveAgain = FALSE;
	m_fAllowMoveAgain = FALSE;
	wTimerCount = 50;
	m_boShut = FALSE;
	fIgnore = FALSE;
	m_pSound = NULL;
	fDoorsLoaded = FALSE;

	m_hHotCursor = LoadCursor (GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));

	// Init. wave filename storage
	m_szSoundTrack[0] = '\0';
	m_szIntroWave[0] = '\0';
	m_szSuccessPlay[0] = '\0';
	m_szGoodPickWave[0] = '\0';
	m_szBadPickWave[0] = '\0';

	for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessLevel[i][0] = '\0';

	m_lpMainSprite = NULL;
	for (i = 0 ; i < TOTAL_ITEMS ; i++)
	{
		m_lpItemSprite[i] = NULL;
		m_lpRoomSprite[i] = NULL;
		m_lpDoorSprite[i] = NULL;
		m_lpOpenDoorSprite[i] = NULL;
		m_lpAllRoomSprite[i] = NULL;

		m_AllItemSprites[i].m_lpSprite = NULL;
		m_AllItemSprites[i].m_fInUse = FALSE;
		m_AllItemSprites[i].m_fInRoom = FALSE;
	}
	for (i = 0 ; i < TOTAL_CHEATS ; i++)
	{
		m_lpCheatUpSprite[i] = NULL;
		m_lpCheatDownSprite[i] = NULL;
	}

	m_SoundPlaying = NotPlaying;
	// Seed for the random number generator
	wRandomSeed = GetRandomNumber (TOTAL_ITEMS);

}


//************************************************************************
CPutAwayScene::~CPutAwayScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;

	if (m_pAnimator)
		m_pAnimator->DeleteAll();

	if (m_hHotCursor)
		DestroyCursor (m_hHotCursor);
}


//************************************************************************
int CPutAwayScene::GetLevel()
//************************************************************************
{
	if (m_iLevel >= 0)
		return (m_iLevel);
	else
	if (m_nSceneNo == IDD_PUTAWAYI)
		return (0);
	else
		return (m_nSceneNo - IDD_PUTAWAY1);
}


//************************************************************************
BOOL CPutAwayScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);

	m_pSound = new CSound (TRUE);
	if (m_pSound)
		m_pSound->Open (NULL);
	
	if (m_pAnimator)
	{
		HGLOBAL hData;
		LPSTR lpTableData;

		if (GetToon())
			GetToon()->SetSendMouseMode (TRUE);

		// Read in the game table information
		lpTableData = GetResourceData (m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CPutAwayParser parser (lpTableData);
			
			parser.ParseTable ((DWORD)this);
		}
		if (hData)
		{
			UnlockResource (hData);
			FreeResource (hData);
		}
	}

	return (TRUE);
}


//************************************************************************
void CPutAwayScene::ToonInitDone()
//************************************************************************
{
	PlayIntro();
}


//************************************************************************
void CPutAwayScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_LEVEL1:
		case IDC_LEVEL2:
		case IDC_LEVEL3:
		{
			int iLevel = id-IDC_LEVEL1;
		 	if (iLevel >= NUM_LEVELS)
		 		iLevel = NUM_LEVELS-1;

			PlayClickWave();
			ChangeLevel (iLevel);
			break;
		}

		case IDC_HELP:
		{
			if (GetSound() && m_szIntroWave[0] != '\0')
			{
				PlayClickWave();

				FNAME szFileName;
				GetPathName (szFileName, m_szIntroWave);
				GetSound()->StopAndFree();
				m_pSound->StartFile (szFileName, NO, HINT_CHANNEL, FALSE);
				m_SoundPlaying = IntroPlaying;
			}
			break;
		}

		default:
		{
			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
			break;
		}
	}
}


//************************************************************************
BOOL CPutAwayScene::OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG (hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG (hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG (hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG (hDlg, MM_MCINOTIFY, OnMCINotify);
		HANDLE_DLGMSG (hDlg, WM_SETCURSOR, OnSetCursor);

		default:
			return (FALSE);
	}
}


//************************************************************************
void CPutAwayScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	int i;
	BOOL boDone;
	RECT rPos;
	BOOL boDrop;
	FNAME szFileName;

	if (fIgnore)
		return;

	if (m_SoundPlaying == IntroPlaying)
	{
		if (GetSound())
			GetSound()->StopChannel (HINT_CHANNEL);
		m_SoundPlaying = NotPlaying;
	}
	
	if (m_fItemMove == TRUE)
	{
		boDrop = FALSE;
		if (y > rGreenRoom.top && y < rGreenRoom.bottom  && x > rGreenRoom.left && x < rGreenRoom.right)
			boDrop = TRUE;

		if (y > rRedRoom.top && y < rRedRoom.bottom  && x > rRedRoom.left && x < rRedRoom.right)
			boDrop = TRUE;

		if (y > rBlueRoom.top && y < rBlueRoom.bottom  && x > rBlueRoom.left && x < rBlueRoom.right)
			boDrop = TRUE;

		if (boDrop)		
		{
			m_fItemMove = FALSE;
			m_lpRoomSprite[wItemMove]->Jump (x, y);
			m_AllItemSprites[wItemSelected].m_fInRoom = TRUE;
			if (! m_fItemMoveAgain)
			{
				++wMatches;
				// See if they have enough matches to complete the level
				if ((wMatches == LVL_ONE_MATCHES   && wLevelNum == 0) ||
					(wMatches == LVL_TWO_MATCHES   && wLevelNum == 1) ||
					(wMatches == LVL_THREE_MATCHES && wLevelNum == 2))
				{
					// Play a success wave based on level
					PlayLevelSuccessWave();
					RestartLevel();
				}
				else
				{
					// New seed for the random number generator
					wRandomSeed = GetRandomNumber (TOTAL_ITEMS);
					SelectItemToSearchFor();
				}
			}
			m_fItemMoveAgain = FALSE;
		}
		return;
	}
	
	
	boDone = FALSE;
	for (i = 0 ; i < sLevel[wLevelNum].wDoorCount && boDone == FALSE ; i++)
	{
		if (m_lpDoorSprite[i] != NULL)
		{
			m_lpDoorSprite[i]->Location (&rPos);
			if (x > rPos.left && x < rPos.right) 
			{
				if (y > rPos.top && y < rPos.bottom)
				{
					boDone = TRUE;
					// Show the crate door open
					m_lpDoorSprite[i]->Show (FALSE);
					m_lpOpenDoorSprite[i]->Show (TRUE);

					// See if they selected the correct item
					if (wItemSelected == wItemList[i])
					{
						if (GetSound() && m_szGoodPickWave[0] != '\0')
						{
							GetSound()->StopChannel (NORMAL_CHANNEL);
							GetSound()->StartFile (GetPathName (szFileName, m_szGoodPickWave),
													NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, TRUE);
						}

						// Delete the closed door sprite
						if (m_lpDoorSprite[i] != NULL)
						{
							m_lpDoorSprite[i]->Show (FALSE);
							m_pAnimator->DeleteSprite (m_lpDoorSprite[i]);
						}
						m_lpDoorSprite[i] = NULL;

						// Hide the crate item
						if (m_lpItemSprite[i] != NULL)
							m_lpItemSprite[i]->Show (FALSE);

						// Delete the search for item
						if (m_lpMainSprite != NULL)
						{
							m_lpMainSprite->Show (FALSE);
							m_pAnimator->DeleteSprite (m_lpMainSprite);
						}
						m_lpMainSprite = NULL;

						// Show the room version of the item
						if (m_pAnimator)
							m_pAnimator->OrderSprite (m_lpRoomSprite[i], NO/*fTop*/);
						m_lpRoomSprite[i]->Show (TRUE);

						wItemMove = i;
						m_fItemMove = TRUE;
					}
					else
					{
						if (GetSound() && m_szBadPickWave[0] != '\0')
						{
							GetSound()->StopChannel (NORMAL_CHANNEL);
							GetSound()->StartFile (GetPathName (szFileName, m_szBadPickWave),
												NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, TRUE);
						}
						fIgnore = TRUE;
						m_boShut = FALSE;
 						wTimerCount = 90;
 					}
				}
			}
		}
	}
	
	if (boDone == FALSE)
	{
		BOOL bCheat = FALSE;
		// See if they clicked on one of the cheat sprites
		for (i = 0 ; i < sLevel[wLevelNum].wCheatPeeks && boDone == FALSE ; i++)
		{
			if (m_lpCheatUpSprite[i] != NULL)
			{
				m_lpCheatUpSprite[i]->Location (&rPos);
				if (x > rPos.left && x < rPos.right) 
				{
					if (y > rPos.top && y < rPos.bottom)
					{
						boDone = TRUE;
						if (m_lpCheatUpSprite[i] != NULL)
							m_pAnimator->DeleteSprite (m_lpCheatUpSprite[i]);
						m_lpCheatUpSprite[i] = NULL;

						m_lpCheatDownSprite[i]->Show (TRUE);
						OpenAllTheDoors();
						fIgnore = TRUE;
						m_boShut = FALSE;
						wTimerCount = 70;
						bCheat = TRUE;
					}
				}
			}
		}

		if (m_fAllowMoveAgain && ! bCheat)
		{
			// See if they clicked to pick up an item already in a room
			for (i = 0 ; i < sLevel[wLevelNum].wDoorCount ; i++)
			{
				if (m_lpRoomSprite[i] != NULL)
				{
					m_lpRoomSprite[i]->Location (&rPos);
					if (x > rPos.left && x < rPos.right && y > rPos.top && y < rPos.bottom)
					{
						wItemMove = i;
						m_fItemMove = TRUE;
						m_fItemMoveAgain = TRUE;
						if (m_pAnimator)
						{
							m_pAnimator->OrderSprite (m_lpRoomSprite[i], NO/*fTop*/);
							m_lpRoomSprite[i]->Draw();
						}
						break;
					}
				}
			}
		}
	}
}


//************************************************************************
void CPutAwayScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}


//************************************************************************
void CPutAwayScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fItemMove == TRUE && m_lpRoomSprite[wItemMove] != NULL)
		m_lpRoomSprite[wItemMove]->Jump (x, y);
}


//************************************************************************
BOOL CPutAwayScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	// Always want the hot spot cursor
	if (m_hHotCursor)
	{
		SetCursor (m_hHotCursor);
		SetDlgMsgResult (hWnd, WM_SETCURSOR, TRUE);
		return TRUE;
	}
	else
		return FALSE;
}


//***************************************************************************
UINT CPutAwayScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
//***************************************************************************
{
	return (TRUE);
}


//************************************************************************
void CPutAwayScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

 	GetRandomNumber (100);
 	fIgnore = TRUE;

	m_pAnimator->SetClipRect (&rGameClipArea);

	// Create sprites
	CreateCheats();
	CreateCloseDoorSprites();
	CreateOpenDoorSprites();
	CreateItemSprites();

    wTimerCount = 1010;
    m_boShut = FALSE;
	wOldCount = 0;
	wMatches = 0;
	m_fItemMove = FALSE;

	if (wLevelNum == 2)
		wShuffleCount = 0;

	if (GetSound())
	{
		if (m_szIntroWave[0] != '\0')
			GetSound()->StartFile (GetPathName (szFileName, m_szIntroWave), NO/*bLoop*/,
								NORMAL_CHANNEL/*iChannel*/, TRUE);
		m_SoundPlaying = IntroPlaying;
	}

}


//************************************************************************
void CPutAwayScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel > NUM_LEVELS - 1)
		iLevel = NUM_LEVELS - 1;
	wLevelNum = iLevel + 10000;
	App.GotoScene (m_hWnd, IDD_PUTAWAY1+iLevel, m_nNextSceneNo);
}


//************************************************************************
void CPutAwayScene::CreateCheats()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;

	for (int i = 0 ; i < sLevel[wLevelNum].wCheatPeeks ; i++)
	{
		m_lpCheatUpSprite[i] = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, CHEAT_UP);
		m_lpCheatUpSprite[i]->AddCell (szFileName, NULL);
		m_lpCheatUpSprite[i]->Jump (CheatSheet[i].x, CheatSheet[i].y);
		m_lpCheatUpSprite[i]->Show (TRUE);

		m_lpCheatDownSprite[i] = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, CHEAT_DOWN);
		m_lpCheatDownSprite[i]->AddCell (szFileName, NULL);
		m_lpCheatDownSprite[i]->Jump (CheatSheet[i].x, CheatSheet[i].y);
		m_lpCheatDownSprite[i]->Show (FALSE);
    }
}


//************************************************************************
void CPutAwayScene::CreateItemSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;

	for (int i = 0 ; i < TOTAL_ITEMS ; i++)
	{
		if (m_AllItemSprites[i].m_lpSprite)
		{
			m_pAnimator->DeleteSprite (m_AllItemSprites[i].m_lpSprite);
			m_AllItemSprites[i].m_lpSprite = NULL;
		}

		// Create the item sprite
		GetPathName (szFileName, ItemBumps[i].szBump);
		m_AllItemSprites[i].m_lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
		if (m_AllItemSprites[i].m_lpSprite)
		{
			m_AllItemSprites[i].m_lpSprite->AddCell (szFileName, NULL);
			m_AllItemSprites[i].m_lpSprite->Jump (ptOrigin.x, ptOrigin.y);
			m_AllItemSprites[i].m_lpSprite->Show (FALSE);
		}

		if (m_lpAllRoomSprite[i])
		{
			m_pAnimator->DeleteSprite (m_lpAllRoomSprite[i]);
			m_lpAllRoomSprite[i] = NULL;
		}

		// Create the room sprite
		GetPathName (szFileName, ItemBumps[i].szRoom);
		m_lpAllRoomSprite[i] = m_pAnimator->CreateSprite (&ptOrigin);
		if (m_lpAllRoomSprite[i])
		{
			m_lpAllRoomSprite[i]->AddCell (szFileName, NULL);
			m_lpAllRoomSprite[i]->Jump (ptOrigin.x, ptOrigin.y);
			m_lpAllRoomSprite[i]->Show (FALSE);
		}
	}
}


//************************************************************************
void CPutAwayScene::CreateOpenDoorSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;

	// Create open door sprites
	for (int j = 0 ; j < sLevel[wLevelNum].wDoorCount ; j++)
	{
		m_lpOpenDoorSprite[j] = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, sLevel[wLevelNum].pDoorList[j].szOpenDoor);
		m_lpOpenDoorSprite[j]->AddCell (szFileName, NULL);
		m_lpOpenDoorSprite[j]->Jump (sLevel[wLevelNum].pDoorList[j].x + sLevel[wLevelNum].pDoorOffset[j].x,
									sLevel[wLevelNum].pDoorList[j].y + sLevel[wLevelNum].pDoorOffset[j].y);
		m_lpOpenDoorSprite[j]->Show (FALSE);
	}

	// Create closed door sprites
	for (j = 0 ; j < sLevel[wLevelNum].wDoorCount ; j++)
	{
		m_lpDoorSprite[j] = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, sLevel[wLevelNum].pDoorList[j].szClosedDoor);
		m_lpDoorSprite[j]->AddCell (szFileName, NULL);
		m_lpDoorSprite[j]->Jump (sLevel[wLevelNum].pDoorList[j].x,
								sLevel[wLevelNum].pDoorList[j].y);
		m_lpDoorSprite[j]->Show (FALSE);
	}
}


//************************************************************************
void CPutAwayScene::CreateCloseDoorSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;

	// Create closed door sprites
	for (int j = 0 ; j < sLevel[wLevelNum].wDoorCount ; j++)
	{
		m_lpDoorSprite[j] = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, sLevel[wLevelNum].pDoorList[j].szClosedDoor);
		m_lpDoorSprite[j]->AddCell (szFileName, NULL);
		m_lpDoorSprite[j]->Jump (sLevel[wLevelNum].pDoorList[j].x,
								sLevel[wLevelNum].pDoorList[j].y);
		m_lpDoorSprite[j]->Show (FALSE);
	}
}


//************************************************************************
void CPutAwayScene::ShutAllTheDoors()
//************************************************************************
{
	for (int i = 0 ; i < sLevel[wLevelNum].wDoorCount ; i++)
	{
		if (m_lpOpenDoorSprite[i] != NULL)
			m_lpOpenDoorSprite[i]->Show (FALSE);			
		if (m_lpDoorSprite[i] != NULL)
			m_lpDoorSprite[i]->Show (TRUE);
	}
	fIgnore = FALSE;
}


//************************************************************************
void CPutAwayScene::OpenAllTheDoors()
//************************************************************************
{
	for (int i = 0 ; i < sLevel[wLevelNum].wDoorCount ; i++)
	{
		if (m_lpDoorSprite[i] != NULL)
			m_lpDoorSprite[i]->Show (FALSE);
		if (m_lpOpenDoorSprite[i] != NULL)
			m_lpOpenDoorSprite[i]->Show (TRUE);			
	}
}


//************************************************************************
void CPutAwayScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	short i;
	FNAME szFileName;
	POINT ptOrigin;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	CGBScene::OnTimer (hWnd, id);
	
	if (wTimerCount < 1000)
		++wTimerCount;
	
	if (wShuffleCount < RESHUFFLE)
		++wShuffleCount;
		
	if (wShuffleCount == RESHUFFLE)
		ReshuffleDeck();		

	if (wTimerCount > 1000)
	{
		if (wTimerCount > 1100 && m_boShut == FALSE)		
		{
			m_boShut = TRUE;
			// New seed for the random number generator
			wRandomSeed = GetRandomNumber (TOTAL_ITEMS);
			SelectItemToSearchFor();	
			ShutAllTheDoors();
			wTimerCount = 1000;
			if (m_szSoundTrack[0])
			{
				if (GetSound())
				{
					GetSound()->StartFile (GetPathName (szFileName, m_szSoundTrack),
						YES/*bLoop*/, MUSIC_CHANNEL, FALSE);
				}
			}
		}
		else
		{
			++wTimerCount;
			if ((wTimerCount - wOldCount) > 10 || (wOldCount - wTimerCount ) > 10)
			{
				int x, y;
				wOldCount = wTimerCount;

				ClearBumpList();
				// Display a random set of objects just for a quick peek				
				for (i = 0 ; i < sLevel[wLevelNum].wDoorCount ; i++)
				{		
					if (m_lpDoorSprite[i] != NULL)
						GetSpriteCenterPoint (m_lpDoorSprite[i], &x, &y);

					m_lpItemSprite[i] = SelectItemForBox (m_lpItemSprite[i],
										 	 x, y, &wItemList[i]);    

					m_lpRoomSprite[i] = RoomSprite; 													 	 
            	}
            }
		}
	}
	else
	{
		if (wTimerCount > 100 && m_boShut == FALSE)		
		{
			m_boShut = TRUE;
			ShutAllTheDoors();
		}
	}	
}


//************************************************************************
void CPutAwayScene::SelectItemToSearchFor()
//************************************************************************
{
	CRandomSequence RandomSequence;
	RandomSequence.Init ((DWORD)TOTAL_ITEMS, (DWORD)wRandomSeed);

	for (int i = 0 ; i < TOTAL_ITEMS ; i++)
	{		
		wItemSelected = (int)RandomSequence.GetNextNumber();
		// Find an item that is in use and not in a room yet
		if (m_AllItemSprites[wItemSelected].m_fInUse &&
		  ! m_AllItemSprites[wItemSelected].m_fInRoom)
            break;
	}

	FNAME szFileName;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	// Create the search for item
	m_lpMainSprite = m_pAnimator->CreateSprite (&ptOrigin);
	if (m_lpMainSprite)
	{
		GetPathName (szFileName, ItemBumps[wItemSelected].szBump);
		m_lpMainSprite->AddCell (szFileName, NULL);
		m_lpMainSprite->Jump (SHOW_ITEM_X, SHOW_ITEM_Y);
		m_lpMainSprite->Show (TRUE);
	}
}


//************************************************************************
LPSPRITE CPutAwayScene::SelectItemForBox (LPSPRITE m_lpSprite, int centerX, int centerY, int *pwItemList)
//************************************************************************
{
	CRandomSequence RandomSequence;
	RandomSequence.Init ((DWORD)TOTAL_ITEMS, (DWORD)wRandomSeed);

	for (int i = 0 ; i < TOTAL_ITEMS ; i++)
	{
		wItemSelected = (int)RandomSequence.GetNextNumber();
		if (wItemSelected < 0)
			break;
		if (! m_AllItemSprites[wItemSelected].m_fInUse)
            break;
	}

	if (wItemSelected < 0)
	{
		for (i = 0 ; i < TOTAL_ITEMS ; i++)
		{
			wItemSelected = i;
			if (! m_AllItemSprites[wItemSelected].m_fInUse)
				break;
		}
	}

	int iX, iY, iCX, iCY;

	// Save the index and pointer to the selected item sprite
	*pwItemList = wItemSelected;
	m_lpSprite = m_AllItemSprites[wItemSelected].m_lpSprite;
	m_AllItemSprites[wItemSelected].m_fInUse = TRUE;
	if (m_lpSprite)
	{
		// Center the sprite in the crate
		m_lpSprite->GetMaxSize (&iCX, &iCY);
		iX = centerX - (iCX / 2);
		iY = centerY - (iCY / 2);
		m_lpSprite->Jump (iX, iY);
		m_lpSprite->Show (TRUE);
	}

	// Save the pointer to the room sprite for the selected item
	RoomSprite = m_lpAllRoomSprite[wItemSelected];
	if (RoomSprite)
	{
		RoomSprite->GetMaxSize (&iCX, &iCY);
		iX = centerX - (iCX / 2);
		iY = centerY - (iCY / 2);
		RoomSprite->Jump (iX, iY);
		RoomSprite->Show (FALSE);
	}
	
	return (m_lpSprite);
}


//************************************************************************
void CPutAwayScene::ClearBumpList()
//************************************************************************
{
	// New seed for the random number generator
	wRandomSeed = GetRandomNumber (TOTAL_ITEMS);

	for (int i = 0 ; i < sLevel[wLevelNum].wDoorCount ; i++)
	{
		if (m_lpItemSprite[i] != NULL)
			m_lpItemSprite[i]->Show (FALSE);

		m_lpItemSprite[i] = NULL;
	}

	for (i = 0 ; i < TOTAL_ITEMS ; i++)
	{
		wItemList[i] = -1;
		m_AllItemSprites[i].m_fInUse = FALSE;
		m_AllItemSprites[i].m_fInRoom = FALSE;
	}

}


//************************************************************************
void CPutAwayScene::ReshuffleDeck()
//************************************************************************
{
	wShuffleCount = 0;			
}


//************************************************************************
BOOL CPutAwayScene::GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY)
//************************************************************************
{
	if (! lpSprite)
		return FALSE;

	int iCX, iCY;
	RECT rLoc;
	lpSprite->Location (&rLoc);
	lpSprite->GetMaxSize (&iCX, &iCY);

	*lpiX = rLoc.left + (iCX / 2);
	*lpiY = rLoc.top + (iCY / 2);

	return TRUE;
}


//************************************************************************
void CPutAwayScene::RestartLevel()
//************************************************************************
{
	// If in Play mode, go to next scene
	if (m_nSceneNo == IDD_PUTAWAYI)
	{
		FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		return;
	}

	m_pAnimator->HideAll();
	fDoorsLoaded = FALSE;

	for (int i = 0 ; i < sLevel[wLevelNum].wDoorCount ; i++)
	{
		if (m_lpItemSprite[i] != NULL)
		{
			m_lpItemSprite[i]->Kill();
			m_lpItemSprite[i] = NULL;
		}
		if (m_lpDoorSprite[i] != NULL)
		{
			m_lpDoorSprite[i]->Kill();
			m_lpDoorSprite[i] = NULL;
		}
		if (m_lpOpenDoorSprite[i] != NULL)
		{
			m_lpOpenDoorSprite[i]->Kill();
			m_lpOpenDoorSprite[i] = NULL;
		}
	}

	for (i = 0 ; i < sLevel[wLevelNum].wCheatPeeks ; i++)
	{
		if (m_lpCheatUpSprite[i] != NULL)
		{
			m_lpCheatUpSprite[i]->Kill();
			m_lpCheatUpSprite[i] = NULL;
		}
		if (m_lpCheatDownSprite[i] != NULL)
		{
			m_lpCheatDownSprite[i]->Kill();
			m_lpCheatDownSprite[i] = NULL;
		}
	}

	for (i = 0 ; i < TOTAL_ITEMS ; i++)
	{
		m_AllItemSprites[i].m_fInUse = FALSE;
		m_AllItemSprites[i].m_fInRoom = FALSE;
	}

	PlayIntro();
}


//************************************************************************
void CPutAwayScene::PlayLevelSuccessWave()
//************************************************************************
{
	LPSTR lpWave;

	// Get the sound to play based on the level or if playing the game
	if (m_nSceneNo == IDD_PUTAWAYI)
		lpWave = m_szSuccessPlay;
	else
	{
		int iCount = 0;
		for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		{
			if (m_szSuccessLevel[i][0] != '\0')
				iCount++;
		}
		if (iCount <= 0)
		{
			lpWave = m_szSuccessLevel[0];
		}
		else
		{
			i = GetRandomNumber (iCount);
			lpWave = m_szSuccessLevel[i];
		}
	}

	if (*lpWave != '\0')
	{
		if (GetSound() )
		{
			// Play this wave without wavemix
			GetSound()->StopAndFree();
			GetSound()->Activate (FALSE);
		}
		FNAME szFileName;
		GetPathName (szFileName, lpWave);
		CSound sound;
		sound.StartFile (szFileName, NO/*bLoop*/, -1/*iChannel*/, TRUE/*bWait*/, m_hWnd);
		if (GetSound())
			GetSound()->Activate (TRUE);
	}
}


//************************************************************************
void CPutAwayScene::PlayClickWave (BOOL fSync/*=TRUE*/)
//************************************************************************
{
	FNAME szFileName;

	// Play synchronously ?
	if (fSync)
	{
		CSound sound;
		if (m_pSound)
		{
			m_pSound->StopAndFree();
			m_pSound->Activate (FALSE);
		}
		if (GetToon()->FindContent (CLICK_WAVE, TRUE, szFileName))
			sound.StartFile (szFileName, NO/*bLoop*/, -1, TRUE);

		if (m_pSound)
			m_pSound->Activate (TRUE);
	}
	else
	{
		m_pSound->StopAndFree();
		if (GetToon()->FindContent (CLICK_WAVE, TRUE, szFileName))
			m_pSound->StartFile (szFileName, FALSE, HINT_CHANNEL, FALSE);
	}
}


//************************************************************************
BOOL CPutAwayParser::GetSuccessWaves (LPPUTAWAYSCENE lpScene, LPSTR lpEntry,
										LPSTR lpValues, int nValues, int iLevel)
//************************************************************************
{
	LPSTR lpValue;

	if (nValues < 1)
	{
		Print ("'%s'\n Invalid success waves key '%s'", lpEntry, lpValues);
		return (FALSE);
	}

	// Don't overrun our buffer
	if (nValues > MAX_SUCCESSWAVES)
	{
		Print ("Too many Success Waves entered, currently %d is supported", MAX_SUCCESSWAVES);
		nValues = MAX_SUCCESSWAVES;
	}

	for (int i = 0 ; i < nValues ; ++i)
	{
		lpValue = GetNextValue (&lpValues);
		lstrcpy (lpScene->m_szSuccessLevel[i], lpValue);
	}
 	return (TRUE);
}


//************************************************************************
BOOL CPutAwayParser::HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPPUTAWAYSCENE lpScene = (LPPUTAWAYSCENE)dwUserData;
	
	if (!lstrcmpi(lpKey, "soundtrack"))
		GetFilename(lpScene->m_szSoundTrack, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "introwav"))
		GetFilename(lpScene->m_szIntroWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "successplay"))
		GetFilename(lpScene->m_szSuccessPlay, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "successlevel"))
		GetSuccessWaves (lpScene, lpEntry, lpValues, nValues, 1);

	if (!lstrcmpi(lpKey, "itembed"))
		GetFilename(lpScene->ItemBumps[0].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itembpic"))
		GetFilename(lpScene->ItemBumps[1].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itembroom"))
		GetFilename(lpScene->ItemBumps[2].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemchair"))
		GetFilename(lpScene->ItemBumps[3].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemchest"))
		GetFilename(lpScene->ItemBumps[4].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemdresser"))
		GetFilename(lpScene->ItemBumps[5].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemfridge"))
		GetFilename(lpScene->ItemBumps[6].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemhammer"))
		GetFilename(lpScene->ItemBumps[7].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemhamper"))
		GetFilename(lpScene->ItemBumps[8].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemhpic"))
		GetFilename(lpScene->ItemBumps[9].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemlamp"))
		GetFilename(lpScene->ItemBumps[10].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itempants"))
		GetFilename(lpScene->ItemBumps[11].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemsofa"))
		GetFilename(lpScene->ItemBumps[12].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemstove"))
		GetFilename(lpScene->ItemBumps[13].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemtv"))
		GetFilename(lpScene->ItemBumps[14].szBump, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrbed"))
		GetFilename(lpScene->ItemBumps[0].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrbpic"))
		GetFilename(lpScene->ItemBumps[1].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrbroom"))
		GetFilename(lpScene->ItemBumps[2].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrchair"))
		GetFilename(lpScene->ItemBumps[3].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrchest"))
		GetFilename(lpScene->ItemBumps[4].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrdresser"))
		GetFilename(lpScene->ItemBumps[5].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrfridge"))
		GetFilename(lpScene->ItemBumps[6].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrhammer"))
		GetFilename(lpScene->ItemBumps[7].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrhamper"))
		GetFilename(lpScene->ItemBumps[8].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrhpic"))
		GetFilename(lpScene->ItemBumps[9].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrlamp"))
		GetFilename(lpScene->ItemBumps[10].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrpants"))
		GetFilename(lpScene->ItemBumps[11].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrsofa"))
		GetFilename(lpScene->ItemBumps[12].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrstove"))
		GetFilename(lpScene->ItemBumps[13].szRoom, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "itemrtv"))
		GetFilename(lpScene->ItemBumps[14].szRoom, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "goodpick"))
		GetFilename(lpScene->m_szGoodPickWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "badpick"))
		GetFilename(lpScene->m_szBadPickWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "allowmoveagain"))
		GetInt (&lpScene->m_fAllowMoveAgain, lpEntry, lpValues, nValues);
		
	return(TRUE);
}
