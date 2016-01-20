//////////////////////////////////////////////////////////////////////////////
//
// Hideseek.cpp
//
// Copyright (C) 1996 - Powerhouse Entertainment, Inc.
//
// Change History
//-----------------------------------------------------------------------------
// 06/18/96		Steve Suggs (SMS)	Added level completed wave based on level.
// 07/01/96		Steve Suggs (SMS)	Fixed problem in level 2 & 3 on showing the
//									proper nailed sprite.
// 07/09/96		Steve Suggs (SMS)	Added RestartLevel logic so the restart
//									doesn't reload the dialog.
// 07/16/96		Steve Suggs (SMS)	Ripped out the state 2 & state 3 function
//									for the different levels.
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <ctype.h>
#include "farmer.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "farmerid.h"
#include "hideseek.h"
#include "hidedef.h"
#include "randseq.h"

void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

Doors BarnDoor[] =
{//  X    Y
	232, 168,   // UL
	232, 260,   // LL
	318, 168,   // UR
	318, 260,   // LR
};

Doors Loft = {279,33};

Doors AnimalViews[] =
{
	236, 168,   // UL
	236, 261,   // LL
	319, 168,   // UR
	319, 261,   // LR
	111, 170,	// L
	440, 170,	// R
};

#define LEVEL1_ANIMALS 6
#define LEVEL2_ANIMALS 7
#define LEVEL3_ANIMALS 7

#define LEVEL1_TENTHS 10
#define LEVEL2_TENTHS 7
#define LEVEL3_TENTHS 4

static int iTenths;
static int nAnimals;
static WORD wRandomSeed;
static short wItemNailed[TOTAL_ANIMAL_BMPS];
static short wItemIsUp[TOTAL_ANIMAL_BMPS];

class CHideSeekParser : public CParser
{
public:
	CHideSeekParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPHIDESEEKSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CHideSeekScene::CHideSeekScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	wNailed = 0;
	nAnimals = LEVEL1_ANIMALS;
	iTenths = LEVEL1_TENTHS;
	wSpacer = 47;

	m_iLevel = -1;
	bPlayIntro = NO;
	m_lpLoftSprite = NULL;
	m_iNumberSuccessWaves = 0;
	m_hHotCursor = LoadCursor(GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));
	wAnimalUp = 0;

	// Get a new seed for the selected sequence
	wRandomSeed = GetRandomNumber(TOTAL_ANIMAL_BMPS);

	// Initialize members of "The Ark"
	for (int i = 0 ; i < TOTAL_ANIMAL_BMPS ; i++)
	{
		TheArk[i].szAnimal[0] = '\0';
		TheArk[i].szAnimalLite[0] = '\0';
		TheArk[i].szWave[0] = '\0';

		wItemNailed[i] = 0;
		wItemIsUp[i] = 0;

		m_lpSpriteLite[i] = NULL;
		m_lpAnimalSprite[i] = NULL;
	}

	for (i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessWaves[i][0] = '\0';
}

//************************************************************************
CHideSeekScene::~CHideSeekScene()
//************************************************************************
{
	if (m_hHotCursor)
		DestroyCursor (m_hHotCursor);

	// Delete all sprites
	if (m_pAnimator)
	{
		m_pAnimator->HideAll();
		m_pAnimator->DeleteAll();
	}
}

//************************************************************************
int CHideSeekScene::GetLevel()
//************************************************************************
{
	if (m_iLevel >= 0)
		return (m_iLevel);
	else
	if (m_nSceneNo == IDD_HIDESEEKI)
		return (0);
	else
		return (m_nSceneNo - IDD_HIDESEEK1);
}

//************************************************************************
BOOL CHideSeekScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);

	m_hWnd = hWnd;
	
	if (GetToon())
		GetToon()->SetHintState(TRUE);

	if (m_pAnimator)
	{
		m_pAnimator->StartAll();

		HGLOBAL hData;
		LPSTR lpTableData;

		if (GetToon())
			GetToon()->SetSendMouseMode (TRUE);

		// read in the connect point information
		lpTableData = GetResourceData (m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CHideSeekParser parser (lpTableData);
			short i = parser.GetNumEntries();
			
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
void CHideSeekScene::ToonInitDone()
//************************************************************************
{
	CreateAnimalSprites();
	CreateBarndoorSprites();
	PlayIntro();
}

//************************************************************************
void CHideSeekScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_HIDESEEKI)
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			else
			{
				// Hide all sprites
				if (m_pAnimator)
					m_pAnimator->HideAll();
				GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
			}
			break;
		}

		case IDC_HELP:
		{
			if (GetSound())
			{
				GetSound()->StopAndFree();
				GetSound()->Activate(FALSE);
			}
			FNAME szFileName;
			GetPathName (szFileName, szIntroWave);
			CSound sound;
			sound.StartFile(szFileName, NO/*bLoop*/, -1/*iChannel*/, TRUE/*bWait*/, m_hWnd);
			if (GetSound())
				GetSound()->Activate(FALSE);
		}
		break;
	
		case IDC_LEVEL1:
		case IDC_LEVEL2:
		case IDC_LEVEL3:
		{
			int iLevel = id-IDC_LEVEL1;
		 	if (iLevel >= NUM_LEVELS)
		 		iLevel = NUM_LEVELS-1;
		 	if (iLevel != GetLevel())
			{
				if (GetSound())
				{
					GetSound()->StopAndFree();
					GetSound()->Activate(FALSE);
				}
				FNAME szFileName;
				GetPathName (szFileName, CLICK_WAVE);
				CSound sound;
				sound.StartFile(szFileName, NO/*bLoop*/, -1/*iChannel*/, TRUE/*bWait*/, m_hWnd);
				if (GetSound())
					GetSound()->Activate(FALSE);
				ChangeLevel(iLevel);
			}
			break;
		}
		case IDC_TOON:
		{
			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
			break;
		}
		default:
		{
			// Hide all sprites
			if (m_pAnimator)
				m_pAnimator->HideAll();
			CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}


//************************************************************************
BOOL CHideSeekScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
//per Craig S. - wants the hotspot cursor all the time in this activity (SMS)
//	POINT pt;
//	LPSPRITE 	lpSprite = NULL;

//	GetCursorPos (&pt);
//	ScreenToClient (hWnd, &pt);

	// See if the mouse is over a sprite
//	lpSprite = m_pAnimator->MouseInAnySprite (&pt, TRUE);
//	if (lpSprite && m_hHotCursor)
//	{
		SetCursor (m_hHotCursor);
   		SetDlgMsgResult (hWnd, WM_SETCURSOR, TRUE);
		return TRUE;	// handled
//	}
//	else
//		return FALSE;	// not handled

}

//************************************************************************
BOOL CHideSeekScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
        HANDLE_DLGMSG(hDlg, MM_WOM_DONE, OnWomDone);
		HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);
		HANDLE_DLGMSG(hDlg, WM_SETCURSOR, OnSetCursor);

		default:
			return(FALSE);
	}
}

//************************************************************************
void CHideSeekScene::OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	if (bPlayIntro == YES)
	{
		bPlayIntro = NO;
		SelectItemToSearchFor();
	}
}

//************************************************************************
void CHideSeekScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (bPlayIntro == YES)
	{
		bPlayIntro = NO;
		SelectItemToSearchFor();
		return;
	}

	if (wItemSelected != wAnimalUp)
		return;

	POINT pt;
	pt.x = x;
	pt.y = y;
	if ( !m_lpAnimalSprite[wAnimalUp]->MouseInSprite( &pt, YES/*fCheckTransparent*/, NULL) )
//	RECT rPos;
//	m_lpAnimalSprite[wAnimalUp]->Location (&rPos);
//	if (x < rPos.left || x > rPos.right || y < rPos.top || y > rPos.bottom)
		return;

	// Hide the animal
	m_lpAnimalSprite[wAnimalUp]->DeleteAllActions();
	m_lpAnimalSprite[wAnimalUp]->Show(FALSE);

	// Mark this item as nailed
	wItemNailed[wAnimalUp] = YES;
	
	// Bump the count
	++wNailed;
	
	RECT rRect;
	GetClientRect (m_hWnd, &rRect);
	m_pAnimator->SetClipRect (&rRect);
				
	ShutTheBarnDoor();

	int iWidth, iHeight;
	m_lpSpriteLite[wAnimalUp]->GetMaxSize(&iWidth, &iHeight);
	m_lpSpriteLite[wAnimalUp]->Jump (wSpacer, GROUND_LEVEL-iHeight);
    m_lpSpriteLite[wAnimalUp]->Show (TRUE);
	wSpacer += SPACING;
	
	if (wNailed >= nAnimals)
	{
		// You win!!! Set the flag to re-start activity at the same level

		// Play a success wave based on level
		PlayLevelSuccessWave();
		// We just finished playing the success wave for the current level,
		// restart the activity or go to the next scene if "playing"
		RestartLevel();
		bPlayIntro = YES;
	}
	else
	{
	    SelectItemToSearchFor();
	}
}

//************************************************************************
void CHideSeekScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CHideSeekScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//***************************************************************************
UINT CHideSeekScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
//***************************************************************************
{
	if (codeNotify == MCI_NOTIFY_SUCCESSFUL)
	{
	}
	return (TRUE);
}

//************************************************************************
LPSTR CHideSeekScene::GetPathName (LPSTR lpPathName, LPSTR lpFileName)
//************************************************************************
{
	GetToon()->GetContentDir (lpPathName);
	lstrcat (lpPathName, lpFileName);
	return (lpPathName);
}

//************************************************************************
void CHideSeekScene::PlayIntro()
//************************************************************************
{
	if (GetSound())
	{
		bPlayIntro = YES;
		FNAME szFileName;
		GetPathName (szFileName, szIntroWave);
		GetSound()->StopAndFree();
		GetSound()->StartFile (szFileName, NO/*bLoop*/, -1/*iChannel*/, FALSE/*bWait*/, m_hWnd);
	}
}

//************************************************************************
void CHideSeekScene::ChangeLevel(int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	if (iLevel != GetLevel())
		GetApp()->GotoScene(m_hWnd, IDD_HIDESEEK1+iLevel, m_nNextSceneNo);

	if ( iLevel <= 0 )
		nAnimals = LEVEL1_ANIMALS;
	else
	if ( iLevel == 1 )
		nAnimals = LEVEL2_ANIMALS;
    else
    	nAnimals = LEVEL3_ANIMALS;

	if ( iLevel <= 0 )
		iTenths = LEVEL1_TENTHS;
	else
	if ( iLevel == 1 )
		iTenths = LEVEL2_TENTHS;
    else
    	iTenths = LEVEL3_TENTHS;
}


//************************************************************************
void CHideSeekScene::RestartLevel()
//************************************************************************
{
	// If we are in Play mode, go to the next scene
	if (GetApp()->m_fPlaying)
	{
		CGBScene::OnCommand (m_hWnd, IDC_NEXT, NULL, 0);
		return;
	}

	wNailed = 0;
	wSpacer = 47;

	for (int i = 0 ; i < TOTAL_ANIMAL_BMPS ; i++)
	{
		wItemNailed[i] = 0;
		wItemIsUp[i] = 0;
	}

	// Get a new seed for the selected sequence
	wRandomSeed = GetRandomNumber(TOTAL_ANIMAL_BMPS);

	RECT rRect;
	GetClientRect (m_hWnd, &rRect);
	m_pAnimator->SetClipRect (&rRect);

	// Hide all sprites
	m_pAnimator->HideAll();

	ShutTheBarnDoor();
}


//************************************************************************
void CHideSeekScene::ShutTheBarnDoor()
//************************************************************************
{
	for (int i = 0 ; i < NUMBER_BARN_DOORS ; i++)
	{
		m_lpDoorShutSprite[i]->Show(TRUE);
		m_lpDoorOpenSprite[i]->Show(FALSE);
	}
}


//************************************************************************
void CHideSeekScene::CreateBarndoorSprites()
//************************************************************************
{
	FNAME szFileName;
	for (int i = 0 ; i < NUMBER_BARN_DOORS ; i++)
	{
		// Set up a closed door sprite
		m_lpDoorShutSprite[i] = m_pAnimator->CreateSprite (NULL);
		if ( m_lpDoorShutSprite[i] )
		{
			GetPathName (szFileName, BARN_DOOR_BMP);
			m_lpDoorShutSprite[i]->AddCell (szFileName, NULL);
			m_lpDoorShutSprite[i]->Jump (BarnDoor[i].x, BarnDoor[i].y);
			m_lpDoorShutSprite[i]->Show (TRUE);
		}

		// Set up an open door sprite		
		m_lpDoorOpenSprite[i] = m_pAnimator->CreateSprite (NULL);
		if ( m_lpDoorOpenSprite[i] )
		{
			// Set in the correct door bitmap
			if (i < 2)
				GetPathName (szFileName, BARN_LOPEN_BMP);
			else
				GetPathName (szFileName, BARN_ROPEN_BMP);
			m_lpDoorOpenSprite[i]->AddCell (szFileName, NULL);
	
			// Set the jump dependent on door position
			if (i < 2)
				m_lpDoorOpenSprite[i]->Jump (BarnDoor[i].x - OPEN_LEFT_DOOR_OFFSET, BarnDoor[i].y);
			else
				m_lpDoorOpenSprite[i]->Jump (BarnDoor[i].x + OPEN_RIGHT_DOOR_OFFSET, BarnDoor[i].y);
			
			m_lpDoorOpenSprite[i]->Show (FALSE);
		}		
	}
}


//************************************************************************
void CHideSeekScene::CreateAnimalSprites()
//************************************************************************
{
	FNAME szFileName;
		
	for (int i=0 ; i<TOTAL_ANIMAL_BMPS; i++)
	{
		GetPathName (szFileName, TheArk[i].szAnimal);
		m_lpAnimalSprite[i] = m_pAnimator->CreateSprite (NULL);
		if (m_lpAnimalSprite[i])
		{
			m_lpAnimalSprite[i]->AddCell (szFileName, NULL);
			m_lpAnimalSprite[i]->Show (FALSE);
		}
	
		GetPathName (szFileName, TheArk[i].szAnimalLite);
		m_lpSpriteLite[i] = m_pAnimator->CreateSprite (NULL);
		if (m_lpSpriteLite[i])
		{
			m_lpSpriteLite[i]->AddCell (szFileName, NULL);
			m_lpSpriteLite[i]->Show (FALSE);
		}
	}
}

//************************************************************************
void CHideSeekScene::SelectItemToSearchFor()
//************************************************************************
{
	// Choose "nAnimals" from TOTAL_ANIMAL_BMPS
	CRandomSequence RandomSequence;
	RandomSequence.Init(TOTAL_ANIMAL_BMPS, wRandomSeed);

	for (int i=0; i<nAnimals; i++)
	{		
		wItemSelected = (int)RandomSequence.GetNextNumber();
		if ( !wItemNailed[wItemSelected] )
            break;
	}

	// Delete the old loft sprite
	if (m_lpLoftSprite)
	{
		m_pAnimator->DeleteSprite (m_lpLoftSprite);
		m_lpLoftSprite = NULL;
	}

	// Create the new loft sprite
	m_lpLoftSprite = m_pAnimator->CreateSprite (NULL);
	if (m_lpLoftSprite)
	{
		FNAME szFileName;
		GetPathName (szFileName, TheArk[wItemSelected].szAnimal); 
		m_lpLoftSprite->AddCell (szFileName, NULL);
		ShowAnimalInLoft(m_lpLoftSprite); // the loft animal
	}
}

//************************************************************************
void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPHIDESEEKSCENE lpScene = (LPHIDESEEKSCENE)dwNotifyData;
		lpScene->OnSNotify(lpSprite, Notify);
	}
}


//************************************************************************
void CHideSeekScene::OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (Notify != SN_MOVEDONE )
		return;

	if ( lpSprite == m_lpLoftSprite )
	{ // play the chosen animal's sound and wait till done
		if (GetSound())
		{
			FNAME szFileName;
			GetPathName (szFileName, TheArk[wItemSelected].szWave);
			GetSound()->StopAndFree();
			GetSound()->StartFile (szFileName, NO/*bLoop*/, -1/*iChannel*/, TRUE/*bWait*/, m_hWnd);
		}

		// Bring up a new animal
		wAnimalUp = ChooseAnimalToShow();
	}
	else
	if ( lpSprite == m_lpAnimalSprite[wAnimalUp] )
	{
		// If it hasn't moved down, get out
		if ( wItemIsUp[wAnimalUp] = !wItemIsUp[wAnimalUp] )
			return;
		
		// Hide the animal
		m_lpAnimalSprite[wAnimalUp]->Show(FALSE);

		RECT rRect;
		GetClientRect (m_hWnd, &rRect);
		m_pAnimator->SetClipRect (&rRect);

		ShutTheBarnDoor();

		// Bring up a new animal
		wAnimalUp = ChooseAnimalToShow();
				
		// If this is a harder level, mix up the animal locations
		int iLevel = GetLevel();
		if (iLevel == 1 || iLevel == 2)
			MixupAnimalLocations();
	}
}


//************************************************************************
void CHideSeekScene::ShowAnimalInLoft(LPSPRITE lpSprite)
//************************************************************************
{
	int x = Loft.x;
	int y = Loft.y;

	RECT rClipArea;
	rClipArea.left = x;
	rClipArea.top = y;
	rClipArea.right	= x + 90;

	int iVertJump = 88;	// Set default vertical jump value
	rClipArea.bottom = y + iVertJump;

	m_pAnimator->SetClipRect (&rClipArea);
	lpSprite->SetSpeed (MOVE_UP_SPEED);
	lpSprite->Show(YES);
	lpSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);
	lpSprite->Jump(x, y + iVertJump);
	lpSprite->Move(x, y);
}


//************************************************************************
void CHideSeekScene::ShowAnimal( int iIndex, int iDoor )
//************************************************************************
{
	LPSPRITE lpSprite = m_lpAnimalSprite[iIndex];
	if ( !lpSprite )
		return;

	if ( iDoor < 0 || iDoor > 5 )
		iDoor = GetRandomNumber(6);

	int x = AnimalViews[iDoor].x;
	int y = AnimalViews[iDoor].y;
	wItemIsUp[wAnimalUp] = NO;

	for (int i = 0 ; i < NUMBER_BARN_DOORS ; i++)
	{
		if (abs(x - BarnDoor[i].x) < 20 && abs(y - BarnDoor[i].y) < 20)
		{
			RECT rRect;
			GetClientRect (m_hWnd, &rRect);
			m_pAnimator->SetClipRect (&rRect);
			m_lpDoorOpenSprite[i]->Show (TRUE);
			m_lpDoorShutSprite[i]->Show (FALSE);
			break;
		}
	}

	RECT rClipArea;
	rClipArea.left = x;
	rClipArea.top = y;
	rClipArea.right	= x + 90;

	int iVertJump = 88;	// Set default vertical jump value
	rClipArea.bottom = y + iVertJump;

	m_pAnimator->SetClipRect (&rClipArea);
	lpSprite->SetSpeed (MOVE_UP_SPEED);
	lpSprite->Show(YES);
	lpSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);
	lpSprite->AddCmdJump (x, y + iVertJump);
	lpSprite->AddCmdMove (x, y);
	lpSprite->AddCmdPause(iTenths);
	lpSprite->AddCmdMove (x, y + iVertJump);

	if (GetSound())
	{
		FNAME szFileName;
		GetPathName (szFileName, TheArk[iIndex].szWave);
		GetSound()->StopAndFree();
		GetSound()->StartFile (szFileName, NO/*bLoop*/, -1/*iChannel*/, FALSE/*bWait*/, m_hWnd);
	}
}

//************************************************************************
short CHideSeekScene::ChooseAnimalToShow()
//************************************************************************
{
	// Clear an array of animals in use
	int iDoorIndex[TOTAL_ANIMAL_BMPS];
	int iNotNailed[TOTAL_ANIMAL_BMPS];
	for (int i=0 ; i<nAnimals; i++)
	{
		iDoorIndex[i] = 0;
		iNotNailed[i] = 0;
	}

	// Choose "nAnimals" from TOTAL_ANIMAL_BMPS with the same seed
	CRandomSequence RandomSequence;
	RandomSequence.Init(TOTAL_ANIMAL_BMPS, wRandomSeed);

	// Count how animals aren't nailed and mark them in the array
	int iCount = 0;
	for (i=0; i<nAnimals; i++)
	{		
		int item = (int)RandomSequence.GetNextNumber();
		if ( !wItemNailed[item] )
		{
			iDoorIndex[iCount] = i;
			iNotNailed[iCount] = item;
			iCount++;
		}
    }

	// Pick a random animal from the ones left
	iCount = GetRandomNumber( iCount );
	// Pick a door to expose the animal in
	int iDoor;
	if ( GetLevel() <= 0 && nAnimals <= 6 )
		iDoor = iDoorIndex[iCount];
	else
		iDoor = -1;
	ShowAnimal( iNotNailed[iCount], iDoor );
	return( iNotNailed[iCount] );
}


//************************************************************************
void CHideSeekScene::MixupAnimalLocations()
//************************************************************************
{
	POINT ptLoc[6];

	// Copy the current locations
	for (int i=0 ; i<6 ; i++)
	{
		ptLoc[i].x = AnimalViews[i].x;
		ptLoc[i].y = AnimalViews[i].y;
	}

	CRandomSequence RandomSequence;
	RandomSequence.Init(6,GetRandomNumber(6));

	// Randomize the locations
	for ( i=0; i<6; i++ )
	{
		long l = RandomSequence.GetNextNumber();
		// if ( l < 0 ) then done
		AnimalViews[i].x = ptLoc[l].x;
		AnimalViews[i].y = ptLoc[l].y;
	}
}

//************************************************************************
void CHideSeekScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	CGBScene::OnTimer (hWnd, id);
}


//************************************************************************
void CHideSeekScene::PlayLevelSuccessWave()
//************************************************************************
{
	int x;

	// Get the sound to play based on the level or if playing the game
	if (GetApp()->m_fPlaying)
		x = 3;
	else
		x = GetLevel();

	// Validity check
	if (x >= 0 && x < m_iNumberSuccessWaves)
	{
		if (m_szSuccessWaves[x][0] != '\0')
		{
			if ( GetSound() )
			{ // Play the file and wait
				FNAME szFileName;
				GetPathName (szFileName, m_szSuccessWaves[x]);
				GetSound()->StopAndFree();
				GetSound()->StartFile (szFileName, NO/*bLoop*/, -1/*iChannel*/, TRUE/*bWait*/, m_hWnd);
			}
 		}
	}
}


//************************************************************************
BOOL CHideSeekScene::IsOnHotSpot(LPPOINT lpPoint)
//************************************************************************
{
	return(FALSE);
}

//************************************************************************
BOOL CHideSeekParser::GetSuccessWaves (LPHIDESEEKSCENE lpScene, LPSTR lpEntry,
										LPSTR lpValues, int nValues)
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
		lstrcpy (lpScene->m_szSuccessWaves[i], lpValue);
		lpScene->m_iNumberSuccessWaves++;
	}
 	return (TRUE);
}

//************************************************************************
BOOL CHideSeekParser::HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues,
								int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPHIDESEEKSCENE lpScene = (LPHIDESEEKSCENE)dwUserData;

	if (! lstrcmpi (lpKey, "animalhorsedark"))
		GetFilename (lpScene->TheArk[0].szAnimal, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalgoatdark"))
		GetFilename (lpScene->TheArk[1].szAnimal, lpEntry, lpValues, nValues);	
		
	if (! lstrcmpi (lpKey, "animalsheepdark"))
		GetFilename (lpScene->TheArk[2].szAnimal, lpEntry, lpValues, nValues);	
		
	if (! lstrcmpi (lpKey, "animalduckdark"))
		GetFilename (lpScene->TheArk[3].szAnimal, lpEntry, lpValues, nValues);	
		
	if (! lstrcmpi (lpKey, "animaldonkeydark"))
		GetFilename (lpScene->TheArk[4].szAnimal, lpEntry, lpValues, nValues);	
		
	if (! lstrcmpi (lpKey, "animalcowdark"))
		GetFilename (lpScene->TheArk[5].szAnimal, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalcatdark"))
		GetFilename (lpScene->TheArk[6].szAnimal, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalchickendark"))
		GetFilename (lpScene->TheArk[7].szAnimal, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalmousedark"))
		GetFilename (lpScene->TheArk[8].szAnimal, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalroosterdark"))
		GetFilename (lpScene->TheArk[9].szAnimal, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalhorselit"))
		GetFilename (lpScene->TheArk[0].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalgoatlit"))
		GetFilename (lpScene->TheArk[1].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalsheeplit"))
		GetFilename (lpScene->TheArk[2].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalducklit"))
		GetFilename (lpScene->TheArk[3].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animaldonkeylit"))
		GetFilename (lpScene->TheArk[4].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalcowlit"))
		GetFilename (lpScene->TheArk[5].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalcatlit"))
		GetFilename (lpScene->TheArk[6].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalchickenlit"))
		GetFilename (lpScene->TheArk[7].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalmouselit"))
		GetFilename (lpScene->TheArk[8].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalroosterlit"))
		GetFilename (lpScene->TheArk[9].szAnimalLite, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalhorsewave"))
		GetFilename (lpScene->TheArk[0].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalgoatwave"))
		GetFilename (lpScene->TheArk[1].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalsheepwave"))
		GetFilename (lpScene->TheArk[2].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalduckwave"))
		GetFilename (lpScene->TheArk[3].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animaldonkeywave"))
		GetFilename (lpScene->TheArk[4].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalcowwave"))
		GetFilename (lpScene->TheArk[5].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalcatwave"))
		GetFilename (lpScene->TheArk[6].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalchickenwave"))
		GetFilename (lpScene->TheArk[7].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalmousewave"))
		GetFilename (lpScene->TheArk[8].szWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "animalroosterwave"))
		GetFilename (lpScene->TheArk[9].szWave, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "introwav"))
		GetFilename (lpScene->szIntroWave, lpEntry, lpValues, nValues);
		
	if (! lstrcmpi (lpKey, "successwavs"))
		GetSuccessWaves (lpScene, lpEntry, lpValues, nValues);

	return (TRUE);  
}

