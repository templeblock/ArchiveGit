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
#include "chase.h"

void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);


static RECT rGameArea = {165, 68, 540, 406}; 
static RECT rGameClipArea = {0, 68, 550, 420};

//Note: need at least annsmesssize entries (from .RC), currently 15 in level 3
static LaundryList AnnsLaundryList[] =
{
	"N.BMP", 		4,	FALSE,	NULL,
	"DRESS.BMP",	6,	FALSE,	NULL,
	"OV.BMP",		4,	FALSE,	NULL,
	"TS-Y.BMP",		6,	FALSE,	NULL,
	"R.BMP",		4,	FALSE,	NULL,
	"BOOTIE.BMP", 	7,	FALSE,	NULL,
	"S.BMP",		8,	FALSE,	NULL,
	"TS.BMP", 		6,	FALSE,	NULL,
	"BJ.BMP",  		8,	FALSE,	NULL,
	"BOOTIE-W.BMP",	7,  FALSE,	NULL,
	"N-BLU.BMP",	4,	FALSE,	NULL,
	"TS-Y.BMP",		6,	FALSE,	NULL,
	"N.BMP", 		4,	FALSE,	NULL,
	"DRESS.BMP",	6,	FALSE,	NULL,
	"OV.BMP",		4,	FALSE,	NULL,
};

static BOOL fLoaded;

// NOTE: make sure and have at least one entry with coordinates for each
// of the four quadrants since this is searched in circular fashion until
// coordinates are found which are in a different quadrant than Ann.
#define MAX_MOVEMENTS 32	// # of AnnsLinenMovement members
static int iLinenSpeed;
static POINT AnnsLinenMovement[] =
{
//	X						Y						
	rGameArea.left + 70,	rGameArea.top + 20,		
	rGameArea.left + 320,	rGameArea.top + 120,	
	rGameArea.left + 150,	rGameArea.top + 90,		
	rGameArea.left + 20,	rGameArea.top + 10,		
	rGameArea.left + 275,	rGameArea.top + 250,	
	rGameArea.left + 60,	rGameArea.top + 160,	
	rGameArea.left + 175,	rGameArea.top + 30,		
	rGameArea.left + 60,	rGameArea.top + 250,	
	rGameArea.left + 290,	rGameArea.top + 50,		
	rGameArea.left + 90,	rGameArea.top + 245,	
	rGameArea.left + 150,	rGameArea.top + 90,		
	rGameArea.left + 60,	rGameArea.top + 120,	
	rGameArea.left + 250,	rGameArea.top + 10,		
	rGameArea.left + 120,	rGameArea.top + 60,		
	rGameArea.left + 190,	rGameArea.top + 275,	
	rGameArea.left + 50,	rGameArea.top + 230,	
	rGameArea.left + 315,	rGameArea.top + 175,	
	rGameArea.left + 30,	rGameArea.top + 50,		
	rGameArea.left + 150,	rGameArea.top + 50,		
	rGameArea.left + 70,	rGameArea.top + 250,	
	rGameArea.left + 80,	rGameArea.top + 75,		
	rGameArea.left + 260,	rGameArea.top + 30,		
	rGameArea.left + 50,	rGameArea.top + 240,	
	rGameArea.left + 290,	rGameArea.top + 90,		
	rGameArea.left + 70,	rGameArea.top + 20,		
	rGameArea.left + 210,	rGameArea.top + 160,	
	rGameArea.left + 60,	rGameArea.top + 30,		
	rGameArea.left + 190,	rGameArea.top + 190,	
	rGameArea.left + 30,	rGameArea.top + 10,		
	rGameArea.left + 310,	rGameArea.top + 235,	
	rGameArea.left + 65,	rGameArea.top + 20,		
	rGameArea.left + 175,	rGameArea.top + 245,	
};


// Array of clothing sprite pointers and caught flags
static ClothingSprites m_lpClothesSprites[MAX_LAUNDRY_SPRITES];
static WORD wLevelNum;
static int iMoveIndex;

class CChaseParser : public CParser
{
public:
	CChaseParser (LPSTR lpTableData) : CParser (lpTableData) {}

protected:
	BOOL HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues,
					int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPCHASESCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues, int iLevel);
};  


//************************************************************************
CChaseScene::CChaseScene (int nNextSceneNo) : CGBScene (nNextSceneNo)
//************************************************************************
{
	iMoveIndex = 0;
	m_pSound = NULL;
	m_iLevel = -1;//wLevelNum;
	LinenCleanedCount = 0;
	m_hSound = NULL;
	walkDir = WALK_RIGHT;
	m_fCarrying = FALSE;
	fLoaded = FALSE;
	m_fLevelComplete = FALSE;
	m_lpLastAnnSprite = NULL;
	ANN_WALK_CELLS = 0;
	ANN_CARRY_CELLS = 0;
	m_fLButtonDown = FALSE;
	m_iLastCaught = -1;
	m_lpLastClothing = NULL;
	m_fInitializing = TRUE;
	m_fClothingOutWindow = FALSE;
	m_ptOutWindow.x = m_ptOutWindow.y = 0;
	m_ptBasketLoc.x = m_ptBasketLoc.y = 0;
	m_fCatchByDoor = FALSE;
	m_SoundPlaying = NotPlaying;
	m_nClothingOutWindowCnt = 0;

	m_hHotCursor = LoadCursor (GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));

	// Init. wave filename storage
	m_szIntroWave[0] = '\0';
	m_szSuccessPlay[0] = '\0';
	m_szCatchWave[0] = '\0';
	m_szDoorCloseWave[0] = '\0';

	for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessLevel[i][0] = '\0';

	// Init. the furniture sprite array
	for (i = 0 ; i < MAX_FURNITURE ; i++)
	{
		m_Furniture[i].szImage[0] = '\0';
		m_Furniture[i].x = 0;
		m_Furniture[i].y = 0;
		m_Furniture[i].lpSprite = NULL;
	}

	for (i = 0 ; i < MAX_LAUNDRY_SPRITES ; i++)
		m_lpClothesSprites[i].fCaught = FALSE;

}

//************************************************************************
CChaseScene::~CChaseScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;

	if (m_hHotCursor)
		DestroyCursor (m_hHotCursor);

	fLoaded = FALSE;
}

//************************************************************************
int CChaseScene::GetLevel()
//************************************************************************
{
	if (m_iLevel >= 0)
		return (m_iLevel);
	else
	if (m_nSceneNo == IDD_CHASEI)
		return (0);
	else
		return (m_nSceneNo - IDD_CHASE1);
}

//************************************************************************
BOOL CChaseScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);
	
	m_pSound = new CSound (TRUE);
	if (m_pSound)
		m_pSound->Open (NULL);

	App.SetKeyMapEntry (m_hWnd, VK_UP);
	App.SetKeyMapEntry (m_hWnd, VK_DOWN);
	App.SetKeyMapEntry (m_hWnd, VK_RIGHT);
	App.SetKeyMapEntry (m_hWnd, VK_LEFT); 
	
	if (m_pAnimator && ! fLoaded)
	{
		fLoaded = TRUE;
		HGLOBAL hData;
		LPSTR lpTableData;

		if (GetToon())
			GetToon()->SetSendMouseMode (TRUE);

		// Read in the connect point information
		lpTableData = GetResourceData (m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CChaseParser parser (lpTableData);
			short i = parser.GetNumEntries();
			
			parser.ParseTable ((DWORD)this);
		}
		if (hData)
		{
			UnlockResource (hData);
			FreeResource (hData);
		}

		wLevelPiecesNeeded = LEVEL_PIECES;
#ifdef _DEBUG
		if (wLevelPiecesNeeded <= 0)
			Print("Invalid levelpieces (pieces needed) in CHASE.RC");
#endif

		// Set base clothing sprite speed based on level
		if (wLevelNum == 0)
			iLinenSpeed = 40;
		else
		if (wLevelNum == 1)
			iLinenSpeed = 60;
		else
		if (wLevelNum == 2)
			iLinenSpeed = 80;
	}
	return (TRUE);
}

//************************************************************************
BOOL CChaseScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
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

//************************************************************************
void CChaseScene::ToonInitDone()
//************************************************************************
{
	LoadTheLaundry (AnnsLaundryList);
	CreateAnnSprites();
	LoadFurniture (m_Furniture);
	CreateDoorSprites();
	PlayIntro();
}


//************************************************************************
void CChaseScene::MakeMove (short *XPos, short *YPos, short x, short y, LPSPRITE lpSprite)
//************************************************************************
{
	if (!lpSprite)
	{
		MakeMove2 (XPos, YPos, x, y);
		return;
	}

	if (!m_lpLastAnnSprite)
	{
		MakeMove2 (XPos, YPos, x, y);
		return;
	}

	RECT rAnnLoc, rMyLoc;
	m_lpLastAnnSprite->Location (&rAnnLoc);
	lpSprite->Location (&rMyLoc);

	int iX, iY;
	GetSpriteCenterPoint (m_lpLastAnnSprite, &iX, &iY);
	int iAnnQuad = GetQuadrant (iX, iY);
	GetSpriteCenterPoint (lpSprite, &iX, &iY);
	int iMyQuad = GetQuadrant (iX, iY);

	GetNewLocation (XPos, YPos, iAnnQuad, iMyQuad, rAnnLoc, rMyLoc);
}


//************************************************************************
void CChaseScene::MakeMove2 (short *XPos, short *YPos, short x, short y)
//************************************************************************
{
	int i = GetRandomNumber(MAX_MOVEMENTS);

    *XPos = AnnsLinenMovement[i].x; 
    *YPos = AnnsLinenMovement[i].y;

	if ((*XPos + x)	> rGameArea.right)
		*XPos = *XPos - x;
		
	if ((*YPos + y)	> rGameArea.bottom)
		*YPos = *YPos - y;
}


//************************************************************************
int CChaseScene::GetQuadrant (int x, int y)
//************************************************************************
{
// quadrant numbering
// 0 | 1
// -----
// 3 | 2

	int iHalfHeight = (rGameArea.bottom - rGameArea.top) / 2;
	int iHalfWidth = (rGameArea.right - rGameArea.left) / 2;

	int iQuad;
	// Quadrant 0
	if ((x <= (rGameArea.left + iHalfWidth)) && (y <= (rGameArea.top + iHalfHeight)))
		iQuad = 0;
	// Quadrant 1
	else if ((x > (rGameArea.left + iHalfWidth)) && (y <= (rGameArea.top + iHalfHeight)))
		iQuad = 1;
	// Quadrant 2
	else if ((x > (rGameArea.left + iHalfWidth)) && (y > (rGameArea.top + iHalfHeight)))
		iQuad = 2;
	// Quadrant 3
	else
		iQuad = 3;

	return iQuad;
}


//************************************************************************
void CChaseScene::GetNewLocation (short *NewX, short * NewY, int iAnnQuad,
									int iMyQuad, RECT rAnnLoc, RECT rMyLoc)
//************************************************************************
{
	// Find coordinates that will put us in a different quadrant from Ann
	// and a different quadrant than we're in now
	POINT pt;
	int i;
	for ( i=0; i<MAX_MOVEMENTS; i++ )
	{
		if (++iMoveIndex >= MAX_MOVEMENTS)
			iMoveIndex = 0;
		pt = AnnsLinenMovement[iMoveIndex];
		if ( pt.x == rMyLoc.left && pt.y == rMyLoc.top )
			continue;
		int q = GetQuadrant(pt.x, pt.y);
		if ( q == iAnnQuad || q == iMyQuad )
			continue;
		break;
	}

	if ( i == MAX_MOVEMENTS )
	{ // if we've failed so far, just pick a location that's different from our own
		for ( i=0; i<MAX_MOVEMENTS; i++ )
		{
			pt = AnnsLinenMovement[i];
			if ( pt.x != rMyLoc.left || pt.y != rMyLoc.top )
				break;
		}

		if ( i == MAX_MOVEMENTS )
			return; // disaster
	}

	*NewX = pt.x; 
	*NewY = pt.y;
}


#ifdef UNUSED
//************************************************************************
BOOL CChaseScene::MoveAwayFromAnn (short *iNewX, short *iNewY, RECT rAnnLoc, RECT rMyLoc)
//************************************************************************
{
	int XPos, YPos;
	int iMyWidth = rMyLoc.right - rMyLoc.left;
	int iMyHeight = rMyLoc.bottom - rMyLoc.top;
	int iMoveAmt = (iMyHeight > iMyWidth) ? iMyHeight * 2 : iMyWidth * 2;

	// Generate a new move based on Ann's position and direction from our
	// current position
	switch (walkDir)
	{
		case WALK_UP:
			if (rAnnLoc.top < rMyLoc.top)
			{
				if (rAnnLoc.left < rMyLoc.left)
					XPos = rMyLoc.left + iMoveAmt;
				else
					XPos = rMyLoc.left - iMoveAmt;
				YPos = rMyLoc.top + iMoveAmt;
			}
			else
			{
				if (rAnnLoc.left < rMyLoc.left)
					XPos = rMyLoc.left - iMoveAmt;
				else
					XPos = rMyLoc.left + iMoveAmt;
				YPos = rMyLoc.top - iMoveAmt;
			}
			break;

		case WALK_DOWN:
			if (rAnnLoc.top < rMyLoc.top)
			{
				if (rAnnLoc.left < rMyLoc.left)
					XPos = rMyLoc.left + iMoveAmt;
				else
					XPos = rMyLoc.left - iMoveAmt;
				YPos = rMyLoc.top + iMoveAmt;
			}
			else
			{
				if (rAnnLoc.left < rMyLoc.left)
					XPos = rMyLoc.left - iMoveAmt;
				else
					XPos = rMyLoc.left + iMoveAmt;
				YPos = rMyLoc.top - iMoveAmt;
			}
			break;

		case WALK_LEFT:
			if (rAnnLoc.left < rMyLoc.left)
			{
				if (rAnnLoc.top < rMyLoc.top)
					YPos = rMyLoc.top + iMoveAmt;
				else
					YPos = rMyLoc.top - iMoveAmt;
				XPos = rMyLoc.left + iMoveAmt;
			}
			else
			{
				if (rAnnLoc.top < rMyLoc.top)
					YPos = rMyLoc.top + iMoveAmt;
				else
					YPos = rMyLoc.top - iMoveAmt;
				XPos = rMyLoc.left - iMoveAmt;
			}
			break;

		case WALK_RIGHT:
			if (rAnnLoc.left < rMyLoc.left)
			{
				if (rAnnLoc.top < rMyLoc.top)
					YPos = rMyLoc.top + iMoveAmt;
				else
					YPos = rMyLoc.top - iMoveAmt;
				XPos = rMyLoc.left - iMoveAmt;
			}
			else
			{
				if (rAnnLoc.top < rMyLoc.top)
					YPos = rMyLoc.top + iMoveAmt;
				else
					YPos = rMyLoc.top - iMoveAmt;
				XPos = rMyLoc.left + iMoveAmt;
			}
			break;
		default:
			return FALSE;
	}

	if (XPos < rGameArea.left)
		XPos = rGameArea.left;
	else if (XPos > rGameArea.right - iMyWidth)
		XPos = rGameArea.right - iMyWidth;

	if (YPos < rGameArea.top)
		YPos = rGameArea.top;
	else if (YPos > rGameArea.bottom - iMyHeight)
		YPos = rGameArea.bottom - iMyHeight;

	*iNewX = XPos;
	*iNewY = YPos;

	return TRUE;
}
#endif

//************************************************************************
BOOL CChaseScene::CreateAnnSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;

	m_pAnimator->SetClipRect (&rGameClipArea);
	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	// Set up the walking Ann sprites		
	m_lpAnnRightSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_WALK_R);
	m_lpAnnRightSprite->SetCellsPerSec (0);
	m_lpAnnRightSprite->AddCells (szFileName, ANN_WALK_CELLS, NULL, NULL);

	m_lpAnnLeftSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_WALK_L);
	m_lpAnnLeftSprite->SetCellsPerSec (0);
	m_lpAnnLeftSprite->AddCells (szFileName, ANN_WALK_CELLS, NULL, NULL);

	m_lpAnnUpSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_WALK_U);
	m_lpAnnUpSprite->SetCellsPerSec (0);
	m_lpAnnUpSprite->AddCells (szFileName, ANN_WALK_CELLS, NULL, NULL);

	m_lpAnnDownSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_WALK_D);
	m_lpAnnDownSprite->SetCellsPerSec (0);
	m_lpAnnDownSprite->AddCells (szFileName, ANN_WALK_CELLS, NULL, NULL);
		
	// Create the Ann carrying sprites
	m_lpAnnGetRightSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_GET_R);
	m_lpAnnGetRightSprite->AddCells (szFileName, ANN_CARRY_CELLS, NULL, NULL);

	m_lpAnnGetLeftSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_GET_L);
	m_lpAnnGetLeftSprite->AddCells (szFileName, ANN_CARRY_CELLS, NULL, NULL);

	m_lpAnnGetUpSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_GET_U);
	m_lpAnnGetUpSprite->AddCells (szFileName, ANN_CARRY_CELLS, NULL, NULL);

	m_lpAnnGetDownSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_GET_D);
	m_lpAnnGetDownSprite->AddCells (szFileName, ANN_CARRY_CELLS, NULL, NULL);

	// Show the initial Ann sprite
	m_lpAnnRightSprite->Jump (ANN_X, ANN_Y);
	m_lpAnnRightSprite->Show (TRUE);

	return TRUE;
}


//************************************************************************
BOOL CChaseScene::CreateDoorSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;

	m_pAnimator->SetClipRect (&rGameClipArea);
	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	// Set up the bedroom door sprites
	m_lpAnnBackDoorOpenSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_BDOOR_OPEN);
	m_lpAnnBackDoorOpenSprite->AddCell (szFileName, NULL);

	m_lpAnnBackDoorShutSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, ANN_BDOOR_SHUT);
	m_lpAnnBackDoorShutSprite->AddCell (szFileName, NULL);
		
	m_lpAnnBackDoorShutSprite->Jump (DOOR_X_SHUT, DOOR_Y_SHUT);
	m_lpAnnBackDoorOpenSprite->Jump (DOOR_X_OPEN, DOOR_Y_OPEN);

	m_lpAnnBackDoorShutSprite->Show (TRUE);

	return TRUE;
}


//************************************************************************
BOOL CChaseScene::LoadTheLaundry (LPLaundryList lpLaundryList)
//************************************************************************
{
	int i,iWidth, iHeight;
	POINT ptOrigin;
	FNAME szFileName;
	short XPos,YPos;
	
	m_pAnimator->SetClipRect (&rGameClipArea);
	ptOrigin.x = 0;
	ptOrigin.y = 0;
	
	for (i = 0 ; i < ANNS_MESS_SIZE ; i++)
	{
		lpLaundryList[i].fGarmentInUse = FALSE;
		m_lpClothesSprites[i].lpClothesSprite = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, lpLaundryList[i].lpGarmentName);
 		m_lpClothesSprites[i].lpClothesSprite->AddCells (szFileName, lpLaundryList[i].nCells, NULL, NULL);
		m_lpClothesSprites[i].lpClothesSprite->SetCellsPerSec (8);

		m_lpClothesSprites[i].lpClothesSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);
		lpLaundryList[i].lpSprite = m_lpClothesSprites[i].lpClothesSprite;

		// Set initial laundry location
		m_lpClothesSprites[i].lpClothesSprite->Jump (m_ptLaundryStart.x, m_ptLaundryStart.y);
		m_lpClothesSprites[i].lpClothesSprite->GetMaxSize (&iWidth, &iHeight);
		
		m_lpClothesSprites[i].lpClothesSprite->SetSpeed (iLinenSpeed * (1+GetRandomNumber(3)));
		m_lpClothesSprites[i].lpClothesSprite->Show(YES);

		MakeMove (&XPos, &YPos, iWidth, iHeight);
		m_lpClothesSprites[i].lpClothesSprite->Move (XPos, YPos);
	}
	
	return (TRUE);
}

//************************************************************************
BOOL CChaseScene::LoadFurniture (LPFurnitureList lpFurnitureList)
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;
	
	for (int i = 0 ; i < MAX_FURNITURE ; i++)
	{
		if (lpFurnitureList[i].szImage[0] != '\0')
		{
			lpFurnitureList[i].lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
			if (lpFurnitureList[i].lpSprite != NULL)
			{
				GetPathName (szFileName, lpFurnitureList[i].szImage);
				lpFurnitureList[i].lpSprite->AddCells (szFileName, 1, NULL, NULL);
				lpFurnitureList[i].lpSprite->Jump (lpFurnitureList[i].x, lpFurnitureList[i].y);
				lpFurnitureList[i].lpSprite->Show (TRUE);
			}
#ifdef _DEBUG
			else
				Debug("Error creating furniture sprite");
#endif
		}
	}
	return (TRUE);
}

//************************************************************************
void CChaseScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
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
BOOL CChaseScene::OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG (hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG (hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG (hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG (hDlg, MM_MCINOTIFY, OnMCINotify);
		HANDLE_DLGMSG (hDlg, WM_KEYDOWN, OnKey);
		HANDLE_DLGMSG (hDlg, WM_SETCURSOR, OnSetCursor);

		default:
			return (FALSE);
	}
}

//************************************************************************
void CChaseScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fLevelComplete)
		return;

	if (m_SoundPlaying == IntroPlaying)
	{
		if (GetSound())
			GetSound()->StopChannel (HINT_CHANNEL);
		m_SoundPlaying = NotPlaying;
	}

	m_fLButtonDown = TRUE;
	m_ptMouse.x = x;
	m_ptMouse.y = y;
	SetTimer (hWnd, CHASE_TIMER, 50, NULL);
}

//************************************************************************
void CChaseScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	m_fLButtonDown = FALSE;
	KillTimer (hWnd, CHASE_TIMER);
}

//************************************************************************
void CChaseScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fLButtonDown)
	{
		m_ptMouse.x = x;
		m_ptMouse.y = y;
	}
}

//************************************************************************
void CChaseScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	CGBScene::OnTimer (hWnd, id);

	if (id == CHASE_TIMER)
	{
		int iX, iY, iDeltaX, iDeltaY;
		UINT nKey;

		if (! m_lpLastAnnSprite)
			return;

		GetSpriteCenterPoint (m_lpLastAnnSprite, &iX, &iY);
		iDeltaX = m_ptMouse.x - iX;
		iDeltaY = m_ptMouse.y - iY;

		// If mouse is too close, ignore
		if ((abs(iDeltaX) < 25) && (abs(iDeltaY) < 25))
			return;

		// Get Ann's new direction
		if (iDeltaX < 0 && iDeltaY < 0)
		{
			iDeltaX = abs(iDeltaX);
			iDeltaY = abs(iDeltaY);
			if (iDeltaX < iDeltaY)
				nKey = VK_UP;
			else
				nKey = VK_LEFT;
		}
		else if (iDeltaX >= 0 && iDeltaY < 0)
		{
			iDeltaX = abs(iDeltaX);
			iDeltaY = abs(iDeltaY);
			if (iDeltaY > iDeltaX)
				nKey = VK_UP;
			else
				nKey = VK_RIGHT;
		}
		else if (iDeltaX >= 0 && iDeltaY >= 0)
		{
			iDeltaX = abs(iDeltaX);
			iDeltaY = abs(iDeltaY);
			if (iDeltaX > iDeltaY)
				nKey = VK_RIGHT;
			else
				nKey = VK_DOWN;
		}
		else
		{
			iDeltaX = abs(iDeltaX);
			iDeltaY = abs(iDeltaY);
			if (iDeltaX > iDeltaY)
				nKey = VK_LEFT;
			else
				nKey = VK_DOWN;
		}

		OnKey (hWnd, nKey, FALSE, 0, 0);
	}
}


//***************************************************************************
UINT CChaseScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
//***************************************************************************
{
	return (TRUE);
}

//************************************************************************
void CChaseScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

	if (GetSound() && m_szIntroWave[0] != '\0')
	{
		GetSound()->StartFile (GetPathName (szFileName, m_szIntroWave), NO/*bLoop*/,
								HINT_CHANNEL, FALSE/*TRUE*/);
		m_SoundPlaying = IntroPlaying;
	}

 	m_pAnimator->StartAll();
	m_fInitializing = FALSE;
	// Prime the Ann info.
	OnKey (m_hWnd, VK_RIGHT, FALSE, 0, 0);
}

//************************************************************************
void CChaseScene::RestartLevel()
//************************************************************************
{
	// If we are in Play mode, go to the next scene
	if (m_nSceneNo == IDD_CHASEI)
	{
		FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		return;
	}

	// Clean up any left over laundry
	for (int i = 0 ; i < ANNS_MESS_SIZE ; i++)
	{
		if (AnnsLaundryList[i].lpSprite != NULL)
		{
			AnnsLaundryList[i].lpSprite->Kill();
			AnnsLaundryList[i].lpSprite = NULL;
		}
	}

	// Kill all furniture sprites
	for (i = 0 ; i < MAX_FURNITURE ; i++)
	{
		if (m_Furniture[i].lpSprite != NULL)
		{
			m_Furniture[i].lpSprite->Kill();
			m_Furniture[i].lpSprite = NULL;
		}
	}

	// Kill all Ann sprites
	if ( m_lpAnnUpSprite )
	{
		m_lpAnnUpSprite->Kill();
		m_lpAnnUpSprite = NULL;
	}
	if ( m_lpAnnDownSprite )
	{
		m_lpAnnDownSprite->Kill();
		m_lpAnnDownSprite = NULL;
	}
	if ( m_lpAnnLeftSprite )
	{
		m_lpAnnLeftSprite->Kill();
		m_lpAnnLeftSprite = NULL;
	}
	if ( m_lpAnnRightSprite )
	{
		m_lpAnnRightSprite->Kill();
		m_lpAnnRightSprite = NULL;
	}
	if ( m_lpAnnGetUpSprite )
	{
		m_lpAnnGetUpSprite->Kill();
		m_lpAnnGetUpSprite = NULL;
	}                     
	if ( m_lpAnnGetDownSprite )
	{
		m_lpAnnGetDownSprite->Kill();
		m_lpAnnGetDownSprite = NULL;
	}
	if ( m_lpAnnGetLeftSprite )
	{
		m_lpAnnGetLeftSprite->Kill();
		m_lpAnnGetLeftSprite = NULL;
	}
	if ( m_lpAnnGetRightSprite )
	{
		m_lpAnnGetRightSprite->Kill();
		m_lpAnnGetRightSprite = NULL;
	}

	// Clear the caught flag for the laundry sprites
	for (i = 0 ; i < MAX_LAUNDRY_SPRITES ; i++)
		m_lpClothesSprites[i].fCaught = FALSE;

	// These have to be created in this order or Ann will be going over the furniture
	// or under the clothing - re-create the sprites.
	LoadTheLaundry (AnnsLaundryList);
	CreateAnnSprites();
	LoadFurniture (m_Furniture);
	CreateDoorSprites();

	LinenCleanedCount = 0;
	m_fCarrying = FALSE;
	m_fLevelComplete = FALSE;
	m_fClothingOutWindow = FALSE;

	// Place Ann back to her starting point
	walkDir = WALK_RIGHT;
	m_lpAnnRightSprite->Jump (ANN_X, ANN_Y);
	m_lpAnnRightSprite->Show (TRUE);

	PlayIntro();
}

//************************************************************************
void CChaseScene::PlayLevelSuccessWave()
//************************************************************************
{
	LPSTR lpWave;

	// Get the sound to play based on the level or if playing the game
	if (GetApp()->m_fPlaying)
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
void CChaseScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	wLevelNum = iLevel;

	m_fInitializing = TRUE;
	App.GotoScene (m_hWnd, IDD_CHASE1+iLevel, m_nNextSceneNo);
}


//************************************************************************
void CALLBACK OnSpriteNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPCHASESCENE lpScene = (LPCHASESCENE)dwNotifyData;
		lpScene->OnSNotify (lpSprite, Notify);
	}
}

//************************************************************************
void CChaseScene::OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	int iWidth, iHeight;	
	short XPos, YPos;

	if (! lpSprite)
		return;
	
	if (Notify == SN_MOVED && !m_fInitializing)
	{
		IsAnnToDoor();
		CheckCollision();
	}
	
	if (Notify == SN_MOVEDONE)
	{
		// See if this is the move from the last piece of clothing needed
		if (m_fLevelComplete && lpSprite == m_lpLastClothing)
		{
			if (m_fLButtonDown)
			{
				m_fLButtonDown = FALSE;
				KillTimer (m_hWnd, CHASE_TIMER);
			}
			m_nClothingOutWindowCnt = 0;
			m_fClothingOutWindow = TRUE;

			// Animate the caught clothes out the window
			ClothingOutWindow();
			m_lpLastClothing = NULL;

			// Play a success wave based on level
			PlayLevelSuccessWave();
		}
		// See if all of the clothing has moved out the window
		else if (m_fLevelComplete && m_fClothingOutWindow)
		{
			m_nClothingOutWindowCnt++;
			if (m_nClothingOutWindowCnt >= wLevelPiecesNeeded)
			{
				m_pAnimator->StopAll();
				m_pAnimator->HideAll();
				// Restart the activity or move to next scene if in Play mode
				RestartLevel();
			}
		}
		else
		{
			if (! ClothesCaught (lpSprite))
			{
				lpSprite->DeleteAllActions();
				lpSprite->GetMaxSize (&iWidth, &iHeight);
				lpSprite->SetSpeed (iLinenSpeed * (1+GetRandomNumber(3)));
				MakeMove (&XPos, &YPos, iWidth, iHeight, lpSprite);
				RECT rect;
				lpSprite->Location(&rect);
				if ( XPos != rect.left || YPos != rect.top )
					lpSprite->Move (XPos, YPos);
				else
					lpSprite->Move (rect.left+1, rect.top+1);
			}
		}
	}
}


//************************************************************************
BOOL CChaseScene::ClothesCaught (LPSPRITE lpSprite)
//************************************************************************
{
	for (int i = 0 ; i < MAX_LAUNDRY_SPRITES ; i++)
	{
		if (m_lpClothesSprites[i].lpClothesSprite == lpSprite)
			return( m_lpClothesSprites[i].fCaught );
	}
	return FALSE;
}


//************************************************************************
void CChaseScene::ClothingOutWindow()
//************************************************************************
{
	int iDelay = 1;
	for (int i = 0 ; i < MAX_LAUNDRY_SPRITES ; i++)
	{
		if (m_lpClothesSprites[i].lpClothesSprite &&
			m_lpClothesSprites[i].fCaught)
		{
			m_lpClothesSprites[i].lpClothesSprite->DeleteAllActions();
			m_lpClothesSprites[i].lpClothesSprite->AddCmdPause (iDelay);
			m_lpClothesSprites[i].lpClothesSprite->AddCmdSetSpeed (100);			
			m_lpClothesSprites[i].lpClothesSprite->AddCmdMove (m_ptOutWindow.x, m_ptOutWindow.y);
			m_lpClothesSprites[i].lpClothesSprite->AddCmdHide();
			iDelay += 10;
		}
	}
}


//************************************************************************
BOOL CChaseScene::CheckCollision()
//************************************************************************
{
	BOOL boHit = FALSE;
	RECT rLoc;

	// If Ann hasn't dropped off the clothing yet, nothing to do
	if (m_fCarrying)
		return FALSE;

	// Get Ann's current direction & position	
	LPSPRITE lpTempSprite = NULL;
	switch (walkDir)
	{
		case WALK_UP:		lpTempSprite = m_lpAnnUpSprite;		break;
		case WALK_DOWN:		lpTempSprite = m_lpAnnDownSprite;	break;
		case WALK_LEFT:		lpTempSprite = m_lpAnnLeftSprite;	break;
		case WALK_RIGHT:	lpTempSprite = m_lpAnnRightSprite;	break;
	}

	if (!lpTempSprite)
		return FALSE;
	lpTempSprite->Location (&rLoc);

	// See if Ann is allowed to catch clothing right in front of the door
	if (!m_fCatchByDoor && m_lpAnnBackDoorShutSprite)
	{
		int iX, iY, iDoorX, iDoorY;
		GetSpriteCenterPoint (lpTempSprite, &iX, &iY);
		GetSpriteCenterPoint (m_lpAnnBackDoorShutSprite, &iDoorX, &iDoorY);
		if (abs (iX - iDoorX) < 100 && abs (iY - iDoorY) < 50)
			return FALSE;
	}

	// See if Ann has caught any pieces of clothing
	for (int i = 0; i < ANNS_MESS_SIZE; ++i)
	{
		if (m_lpClothesSprites[i].lpClothesSprite != NULL)
		{
			if (m_pAnimator->CheckCollision (lpTempSprite, m_lpClothesSprites[i].lpClothesSprite))
			{
				// Update the positions for the carrying Ann sprites
				m_lpAnnGetUpSprite->Jump (rLoc.left, rLoc.top);
				m_lpAnnGetDownSprite->Jump (rLoc.left, rLoc.top);
				m_lpAnnGetLeftSprite->Jump (rLoc.left, rLoc.top);
				m_lpAnnGetRightSprite->Jump (rLoc.left, rLoc.top);

				// Change Ann to the appropriate carrying sprite
				lpTempSprite->Show (FALSE);
				switch (walkDir)
				{
					case WALK_UP:	lpTempSprite = m_lpAnnGetUpSprite;		break;
					case WALK_DOWN:	lpTempSprite = m_lpAnnGetDownSprite;    break;
					case WALK_LEFT:	lpTempSprite = m_lpAnnGetLeftSprite;	break;
					case WALK_RIGHT:lpTempSprite = m_lpAnnGetRightSprite;	break;
				}
				lpTempSprite->Show (TRUE);

				m_fCarrying = TRUE;
				AnnGotOne (i);
				boHit = TRUE;
				break;
			} 
		}
	}

	return (boHit);
}


//************************************************************************
BOOL CChaseScene::IsAnnToDoor()
//************************************************************************
{
	if (!m_fCarrying)
		return FALSE;

	// Get current Ann Sprite
	LPSPRITE lpTempSprite = NULL;
	switch (walkDir)
	{
		case WALK_UP:
			lpTempSprite = m_fCarrying ? m_lpAnnGetUpSprite : m_lpAnnUpSprite;
			break;
		case WALK_DOWN:
			lpTempSprite = m_fCarrying ? m_lpAnnGetDownSprite : m_lpAnnDownSprite; 
			break;
		case WALK_LEFT:		
			lpTempSprite = m_fCarrying ? m_lpAnnGetLeftSprite : m_lpAnnLeftSprite; 	
			break;
		case WALK_RIGHT:	
			lpTempSprite = m_fCarrying ? m_lpAnnGetRightSprite : m_lpAnnRightSprite; 	
			break;
	}

	if ( !lpTempSprite )
		return(FALSE);

	// See if Ann has reached the bedroom door
	if (m_pAnimator->CheckCollision (lpTempSprite, m_lpAnnBackDoorOpenSprite) ||
		m_pAnimator->CheckCollision (lpTempSprite, m_lpAnnBackDoorShutSprite))
	{
		RECT rLoc;
		lpTempSprite->Location (&rLoc);
		// Update the positions for the walking Ann sprites
		m_lpAnnUpSprite->Jump (rLoc.left, rLoc.top);
		m_lpAnnDownSprite->Jump (rLoc.left, rLoc.top);
		m_lpAnnLeftSprite->Jump (rLoc.left, rLoc.top);
		m_lpAnnRightSprite->Jump (rLoc.left, rLoc.top);

		// Show the caught clothing moving to the laundry basket
		if (m_iLastCaught != -1)
		{
			m_pAnimator->SetClipRect (&rGameArea);
			m_pAnimator->OrderSprite (m_lpClothesSprites[m_iLastCaught].lpClothesSprite, NO);
			// Position the clothes just inside the room, then move it to the basket
			m_lpClothesSprites[m_iLastCaught].lpClothesSprite->Jump (rLoc.left - 65, rLoc.top - 20);
			m_lpClothesSprites[m_iLastCaught].lpClothesSprite->Show (TRUE);
			m_lpClothesSprites[m_iLastCaught].lpClothesSprite->Move (m_ptBasketLoc.x, m_ptBasketLoc.y);
			m_pAnimator->SetClipRect (&rGameClipArea);
			m_lpLastClothing = m_lpClothesSprites[m_iLastCaught].lpClothesSprite;
			m_iLastCaught = -1;
		}

		// Show the bedroom door closed
		m_lpAnnBackDoorOpenSprite->DeleteAllActions();
		m_lpAnnBackDoorOpenSprite->AddCmdPause (2);
		m_lpAnnBackDoorOpenSprite->AddCmdHide();

		m_lpAnnBackDoorShutSprite->DeleteAllActions();
		m_lpAnnBackDoorShutSprite->AddCmdPause (2);
		m_lpAnnBackDoorShutSprite->AddCmdShow();
		m_fCarrying = FALSE;

		// Switch Ann back to not carrying anything
		AnnNoCarry();

		FNAME szFileName;
		// Play the door close wave
		if (GetSound() && m_szDoorCloseWave[0] != '\0')
			GetSound()->StartFile (GetPathName (szFileName, m_szDoorCloseWave), NO/*bLoop*/,
									1/*iChannel*/, FALSE);

		// See if Ann has caught enough to complete this level
		if (LinenCleanedCount >= wLevelPiecesNeeded)
		{
			m_fLevelComplete = TRUE;
			m_fInitializing = TRUE;
		}
	}
	return FALSE;
}


//************************************************************************
void CChaseScene::OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	if (m_fLevelComplete || m_fInitializing)
		return;

	// Let base class handle the space bar
	if (vk == VK_SPACE)
	{
		m_pAnimator->StopAll();
		CGBScene::OnKey (hWnd, vk, fDown, cRepeat, flags);
		return;
	}

	LPSPRITE lpTempSprite = NULL;
	switch (walkDir)
	{
		case WALK_UP:
			lpTempSprite = m_fCarrying ? m_lpAnnGetUpSprite : m_lpAnnUpSprite;
			break;
		case WALK_DOWN:
			lpTempSprite = m_fCarrying ? m_lpAnnGetDownSprite : m_lpAnnDownSprite;
			break;
		case WALK_LEFT:
			lpTempSprite = m_fCarrying ? m_lpAnnGetLeftSprite : m_lpAnnLeftSprite; 	
			break;
		case WALK_RIGHT:
			lpTempSprite = m_fCarrying ? m_lpAnnGetRightSprite : m_lpAnnRightSprite; 
			break;
	}

	if ( !lpTempSprite )
		return;

	RECT rLoc;
	lpTempSprite->Location (&rLoc);

	// Ann can't move through the furniture
	if (FurnitureCollision (lpTempSprite, rLoc, vk))
		return;

	lpTempSprite->Show (FALSE);

	if (m_lpLastAnnSprite)
	{
		m_lpLastAnnSprite->Jump (rLoc.left, rLoc.top);
		m_lpLastAnnSprite->Show (FALSE);
	}

	lpTempSprite = NULL;
	switch (vk)
	{
		case VK_UP:
			walkDir = WALK_UP;
			rLoc.top -= ANN_STEP;
			if (rLoc.top < rGameArea.top)
				rLoc.top = rGameArea.top;
			lpTempSprite = m_fCarrying ? m_lpAnnGetUpSprite : m_lpAnnUpSprite;
			break;
			
		case VK_DOWN:
			walkDir = WALK_DOWN;
			rLoc.top += ANN_STEP;
			if (rLoc.bottom > rGameArea.bottom)
				rLoc.top = rGameArea.bottom - (rLoc.bottom - rLoc.top);
			lpTempSprite = m_fCarrying ? m_lpAnnGetDownSprite : m_lpAnnDownSprite;
			break;
			
		case VK_RIGHT:
			walkDir = WALK_RIGHT;
			rLoc.left += ANN_STEP;
			if (rLoc.right > rGameArea.right)
				rLoc.left = rGameArea.right - (rLoc.right - rLoc.left);
			lpTempSprite = m_fCarrying ? m_lpAnnGetRightSprite : m_lpAnnRightSprite;
			break;
			
		case VK_LEFT:
			walkDir = WALK_LEFT;
			rLoc.left -= ANN_STEP;
			if (rLoc.left < rGameArea.left)
				rLoc.left = rGameArea.left;
			lpTempSprite = m_fCarrying ? m_lpAnnGetLeftSprite : m_lpAnnLeftSprite;
			break;

		default:
			break;
	}

	// Save a pointer to the current sprite
	if ( lpTempSprite )
	{
		m_lpLastAnnSprite = lpTempSprite;
		m_lpLastAnnSprite->Jump (rLoc.left, rLoc.top);
		m_lpLastAnnSprite->Show (TRUE); 
		m_lpLastAnnSprite->CycleCells();
	}
}


//************************************************************************
void CChaseScene::AnnNoCarry()
//************************************************************************
{
	// This is only used when a level is complete and we want to change
	// Ann back to not holding anything since OnKey can't change her back.

	RECT rLoc;
	SetRectEmpty(&rLoc);
	if ( m_lpLastAnnSprite )
	{
		m_lpLastAnnSprite->Location (&rLoc);
		m_lpLastAnnSprite->Show (FALSE);
	}

	switch (walkDir)
	{
		case WALK_UP:		m_lpLastAnnSprite = m_lpAnnUpSprite;	break;
		case WALK_DOWN:		m_lpLastAnnSprite = m_lpAnnDownSprite;	break;
		case WALK_LEFT:		m_lpLastAnnSprite = m_lpAnnLeftSprite;	break;
		case WALK_RIGHT:	m_lpLastAnnSprite = m_lpAnnRightSprite; break;
	}

	if ( m_lpLastAnnSprite )
	{
		if ( !IsRectEmpty(&rLoc) )
			m_lpLastAnnSprite->Jump (rLoc.left, rLoc.top);
		m_lpLastAnnSprite->Show (TRUE);
	}
}


//************************************************************************
BOOL CChaseScene::FurnitureCollision (LPSPRITE lpAnnSprite, RECT rLoc, UINT nKey)
//************************************************************************
{
	if (! lpAnnSprite)
		return FALSE;
    
    int iWidth, iHeight;
	RECT rAnnLoc = rLoc;
	lpAnnSprite->GetMaxSize (&iWidth, &iHeight);		

	// Build a rectangle with Ann's predicted location limiting it to the game area
	switch (nKey)
	{
		case VK_UP:
			rAnnLoc.top -= ANN_STEP;
			rAnnLoc.bottom -= ANN_STEP;
			if (rAnnLoc.top < rGameArea.top)
			{
				rAnnLoc.top = rGameArea.top;
				rAnnLoc.bottom = rAnnLoc.top + iHeight;
			}
			break;
			
		case VK_DOWN:
			rAnnLoc.top += ANN_STEP;
			rAnnLoc.bottom += ANN_STEP;
			if (rAnnLoc.bottom > rGameArea.bottom)
			{
				rAnnLoc.top = rGameArea.bottom - (rLoc.bottom - rLoc.top);
				rAnnLoc.bottom = rAnnLoc.top + iHeight;
			}
			break;
			
		case VK_RIGHT:
			rAnnLoc.left += ANN_STEP;
			rAnnLoc.right += ANN_STEP;
			if (rAnnLoc.right > rGameArea.right)
			{
				rAnnLoc.left = rGameArea.right - (rLoc.right - rLoc.left);
				rAnnLoc.right = rAnnLoc.left + iWidth;
			}
			break;
			
		case VK_LEFT:
			rAnnLoc.left -= ANN_STEP;
			rAnnLoc.right -= ANN_STEP;
			if (rAnnLoc.left < rGameArea.left)
			{
				rAnnLoc.left = rGameArea.left;
				rAnnLoc.right = rAnnLoc.left + iWidth;
			}
			break;
	}

	RECT rFurniture;

	// Check all furniture for a collision
	for (int i = 0 ; i < MAX_FURNITURE ; i++)
	{
		if (m_Furniture[i].lpSprite != NULL)
		{
			if (m_pAnimator->CheckCollision (lpAnnSprite, m_Furniture[i].lpSprite, &rAnnLoc))
			{
				// Don't let Ann get stuck to the furniture
				m_Furniture[i].lpSprite->Location (&rFurniture);
				switch (nKey)
				{
					case VK_UP:
						if (rLoc.top < rFurniture.top)
							return FALSE;
						break;
					case VK_DOWN:
						if (rLoc.bottom > rFurniture.bottom)
							return FALSE;
						break;
					case VK_RIGHT:
						if (rLoc.right > rFurniture.right)
							return FALSE;
						break;
					case VK_LEFT:
						if (rLoc.left < rFurniture.left)
							return FALSE;
						break;
				}
				return (TRUE);
			}
		}
	}

	return FALSE;
}

//************************************************************************
void CChaseScene::AnnGotOne (unsigned int TheCaught)
//************************************************************************
{
	// Save the index to the caught clothing and hide it
	if ( TheCaught >= MAX_LAUNDRY_SPRITES )
		return;
	m_iLastCaught = TheCaught;
	if (m_lpClothesSprites[m_iLastCaught].lpClothesSprite)
	{
		m_lpClothesSprites[m_iLastCaught].lpClothesSprite->Show (FALSE);
		m_lpClothesSprites[m_iLastCaught].lpClothesSprite->DeleteAllActions();
		m_lpClothesSprites[m_iLastCaught].fCaught = TRUE;
	}
	
	++LinenCleanedCount;

	FNAME szFileName;
	// Play the catch wave
	if (GetSound() && m_szCatchWave[0] != '\0')
		GetSound()->StartFile (GetPathName (szFileName, m_szCatchWave), NO/*bLoop*/,
								1/*iChannel*/, FALSE/*TRUE*/);

	// Show the bedroom door open when Ann catches one
	m_lpAnnBackDoorShutSprite->Show (FALSE);
	m_lpAnnBackDoorOpenSprite->Show (TRUE);
}


//************************************************************************
void CChaseScene::PlayClickWave (BOOL fSync/*=TRUE*/)
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
BOOL CChaseScene::GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY)
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
void CChaseScene::SaveFurnitureBmp (LPSTR lpFilename, int nIndex)
//************************************************************************
{
	if (nIndex >= 0 && nIndex < MAX_FURNITURE)
		lstrcpy (m_Furniture[nIndex].szImage, lpFilename);
}


//************************************************************************
void CChaseScene::SaveFurnitureX (int iX, int nIndex)
//************************************************************************
{
	if (nIndex >= 0 && nIndex < MAX_FURNITURE)
		m_Furniture[nIndex].x = iX;
}

//************************************************************************
void CChaseScene::SaveFurnitureY (int iY, int nIndex)
//************************************************************************
{
	if (nIndex >= 0 && nIndex < MAX_FURNITURE)
		m_Furniture[nIndex].y = iY;
}

//************************************************************************
BOOL CChaseParser::GetSuccessWaves (LPCHASESCENE lpScene, LPSTR lpEntry,
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
BOOL CChaseParser::HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues,
								int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPCHASESCENE lpScene = (LPCHASESCENE)dwUserData;

	if (!lstrcmpi(lpKey, "introwav"))
		GetFilename(lpScene->m_szIntroWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "successplay"))
		GetFilename(lpScene->m_szSuccessPlay, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "successlevel"))
		GetSuccessWaves (lpScene, lpEntry, lpValues, nValues, 1);

	if (!lstrcmpi(lpKey, "catchwav"))
		GetFilename(lpScene->m_szCatchWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "doorclosewav"))
		GetFilename(lpScene->m_szDoorCloseWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "basketx"))
		GetInt(&lpScene->m_ptBasketLoc.x, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "baskety"))
		GetInt(&lpScene->m_ptBasketLoc.y, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "windowx"))
		GetInt(&lpScene->m_ptOutWindow.x, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "windowy"))
		GetInt(&lpScene->m_ptOutWindow.y, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "annwalkr"))
		GetFilename(lpScene->ANN_WALK_R, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annwalku"))
		GetFilename(lpScene->ANN_WALK_U, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annwalkl"))
		GetFilename(lpScene->ANN_WALK_L, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annwalkd"))
		GetFilename(lpScene->ANN_WALK_D, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "anngetr"))
		GetFilename(lpScene->ANN_GET_R, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "anngetu"))
		GetFilename(lpScene->ANN_GET_U, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "anngetl"))
		GetFilename(lpScene->ANN_GET_L, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "anngetd"))
		GetFilename(lpScene->ANN_GET_D, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annbdooropen"))
		GetFilename(lpScene->ANN_BDOOR_OPEN, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annbdoorshut"))
		GetFilename(lpScene->ANN_BDOOR_SHUT, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "annsmesssize"))
	{
		GetInt(&lpScene->ANNS_MESS_SIZE, lpEntry, lpValues, nValues);
		if (lpScene->ANNS_MESS_SIZE > MAX_LAUNDRY_SPRITES )
			lpScene->ANNS_MESS_SIZE = MAX_LAUNDRY_SPRITES;
	}
		
	if (!lstrcmpi(lpKey, "doorxshut"))
		GetInt(&lpScene->DOOR_X_SHUT, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "dooryshut"))
		GetInt(&lpScene->DOOR_Y_SHUT, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "doorxopen"))
		GetInt(&lpScene->DOOR_X_OPEN, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "dooryopen"))
		GetInt(&lpScene->DOOR_Y_OPEN, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annstep"))
		GetInt(&lpScene->ANN_STEP, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annx"))
		GetInt(&lpScene->ANN_X, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "anny"))
		GetInt(&lpScene->ANN_Y, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "annwalkcells"))
		GetInt(&lpScene->ANN_WALK_CELLS, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "anncarrycells"))
		GetInt(&lpScene->ANN_CARRY_CELLS, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "levelpieces"))
		GetInt(&lpScene->LEVEL_PIECES, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "catchbydoor"))
		GetInt(&lpScene->m_fCatchByDoor, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "laundrystartx"))
		GetInt(&lpScene->m_ptLaundryStart.x, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "laundrystarty"))
		GetInt(&lpScene->m_ptLaundryStart.y, lpEntry, lpValues, nValues);

	FNAME szFilename;
	int iTmp;
	if (!lstrcmpi(lpKey, "furniture"))
	{
		GetFilename (szFilename, lpEntry, lpValues, nValues);
		lpScene->SaveFurnitureBmp (szFilename, nIndex);
	}
	if (!lstrcmpi(lpKey, "furnx"))
	{
		GetInt(&iTmp, lpEntry, lpValues, nValues);
		lpScene->SaveFurnitureX (iTmp, nIndex);
	}
	if (!lstrcmpi(lpKey, "furny"))
	{
		GetInt(&iTmp, lpEntry, lpValues, nValues);
		lpScene->SaveFurnitureY (iTmp, nIndex);
	}

	return (TRUE);
}

