///////////////////////////////////////////////////////////////////////////////
//
// Cleanup.cpp
//
// Copyright (C) 1996 - Powerhouse Entertainment, Inc.
//
// Change History
//-----------------------------------------------------------------------------
// 06/20/96		Steve Suggs	(SMS)	Fixed field sprites showing up on top of current
//									animal.  Added randomizing the order the animals
//									come up.  Added support for a level complete
//									success wave based on level.
// 06/30/96		Steve Suggs (SMS)	Modified level complete code to just restart the
//									the activity and not reload the dialog.
// 07/06/96		Steve Suggs (SMS)	Changed OnSetCursor to always use the hot spot cursor.
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
#include "cleanup.h"
#include "cleandef.h"


LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

class CCleanupParser : public CParser
{
public:
	CCleanupParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPCLEANUPSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  



//************************************************************************
CCleanupScene::CCleanupScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	short i;
	
	m_iLevel = -1;
	m_hSound = NULL;
	m_pSound = NULL;
	wIntroFlag = TRUE;
	wLevelCompleteFlag = FALSE;
	m_iNumberSuccessWaves = 0;
	wPartNum = 0;
	m_lpSparkSprite = NULL;
	m_hHotCursor = LoadCursor(GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));
	m_GameState = InProgress;

	wAnimal = -1;

	for (i = 0 ; i < MAX_NUMBER_OF_PARTS ; i++)
	{	
		m_lpDotSprite[i] = NULL;
		m_lpAnimalSprite[i] = NULL;
		wParts[i] = FALSE;
	}
	wCount = -1;

	for (i = 0 ; i < MAX_CLEAN_ANIMALS ; i++)
		m_lpFieldSprite[i] = NULL;
	
	for (i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessWaves[i][0] = '\0';

}

//************************************************************************
CCleanupScene::~CCleanupScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;

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
int CCleanupScene::GetLevel()
//************************************************************************
{
	if (m_iLevel >= 0)
		return (m_iLevel);
	else if (m_nSceneNo == IDD_CLEANUPI)
		return (0);
	else
		return (m_nSceneNo - IDD_CLEANUP1);
}

//************************************************************************
BOOL CCleanupScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	short i;
	
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

		// Read in the game table data
		lpTableData = GetResourceData (m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CCleanupParser parser (lpTableData);
			i = parser.GetNumEntries();
			
			parser.ParseTable ((DWORD)this);

			// Initialize complete flags for all animals
			for (int i = 0 ; i < MAX_CLEAN_ANIMALS ; i++)
				Meal [i].wComplete = FALSE;
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
void CCleanupScene::ToonInitDone()
//************************************************************************
{
	PlayIntro();
}

//************************************************************************
void CCleanupScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	FNAME szFileName;
	
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_CLEANUPI)
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
			wIntroFlag = TRUE;
			GetToon()->SetHintState(FALSE);
	
			if (GetSound())
			{
				GetSound()->StopAndFree();
				GetSound()->StartFile (GetPathName (szFileName, szIntroWave),
								NO/*bLoop*/, 2/*iChannel*/, FALSE, m_hWnd);
			}
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
				CSound sound;
				FNAME szFileName;

				// Play a click sound without wavemix
				m_pSound->StopAndFree();
				if (m_pSound)
					m_pSound->Activate (FALSE);
				GetPathName (szFileName, CLICK_WAVE);
				sound.StartFile (szFileName, NO/*bLoop*/, -1/*iChannel*/, TRUE);
				if (m_pSound)
					m_pSound->Activate (TRUE);
				ChangeLevel (iLevel);
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
			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//************************************************************************
BOOL CCleanupScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
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
BOOL CCleanupScene::OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
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
void CCleanupScene::OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	short i;
	FNAME szFileName;
	POINT ptOrigin;

	m_GameState = InProgress;

	// The user completed a level and the level success wave just played
	if (wLevelCompleteFlag == TRUE)
	{
		wLevelCompleteFlag = FALSE;

		// This will either restart the level or go to the next scene
		RestartLevel();
	}
	// If introduction time
	else if (wIntroFlag == TRUE)
	{
		wIntroFlag = FALSE;
		GetToon()->SetHintState (TRUE);
		if (GetSound())
		{
			GetSound()->StopAndFree();
			GetSound()->StartFile (GetPathName (szFileName, "sndtrack.wav"),
									YES/*bLoop*/, 3/*iChannel*/, TRUE);
		}
	}
	else
	{
		ptOrigin.x = 0;
		ptOrigin.y = 0; 

		if (m_lpFieldSprite[wAnimalNum] != NULL)
		{
			m_pAnimator->DeleteSprite (m_lpFieldSprite[wAnimalNum]);
			m_lpFieldSprite[wAnimalNum] = NULL;
		}

		m_lpFieldSprite[wAnimalNum] = m_pAnimator->CreateSprite (& ptOrigin);
		GetPathName (szFileName, Meal[wAnimalNum].pAnimal[wPartNum].szPartName);
		m_lpFieldSprite[wAnimalNum]->AddCell (szFileName, NULL);
		m_lpFieldSprite[wAnimalNum]->Jump (Meal[wAnimalNum].pAnimal[wPartNum].x,
										Meal[wAnimalNum].pAnimal[wPartNum].y);

		// Hide the completed animal pieces
		for (i = 0 ; i < Meal[wAnimalNum].wParts ; i++)
		{
			m_lpAnimalSprite[i]->Show (FALSE);
		}

		// Show the animal in the field after the completion sound
		// has played.
		m_lpFieldSprite[wAnimalNum]->Show (TRUE);

		// Get the next animal randomly
		SetNextAnimal();

		// See if all animals are complete for this level
		if (! LevelComplete())
		{
			wPartNum = 0;
    		wCount = -1;	
			for (i = 0 ; i < MAX_NUMBER_OF_PARTS ; i++)
				wParts[i] = FALSE;     
			
			ButcherAnimal();
			if (GetSound())
			{
				GetSound()->StopAndFree();
				GetSound()->StartFile (GetPathName (szFileName, "sndtrack.wav"),
										YES/*bLoop*/, 3/*iChannel*/, TRUE);
			}
			
		}
		else
		{
			if (GetSound())
			{
				// Set a flag so we'll know they finished this level
 				wLevelCompleteFlag = TRUE;

				GetSound()->StopAndFree();
				
				int x;
 				// Get the sound to play based on the level or if playing the game
				if (GetApp()->m_fPlaying)
					x = 3;
				else
 					x = GetLevel();
 				// Validity check
 				if (x >= 0 || x <= m_iNumberSuccessWaves - 1)
				{
 					if (m_szSuccessWaves[x][0] != '\0')
 					{
 						m_GameState = WaitingForDone;
 						GetPathName (szFileName, m_szSuccessWaves[x]);
						GetSound()->StartFile (szFileName, NO/*bLoop*/,
												2/*iChannel*/, FALSE, m_hWnd);
 					}
					else
					{
#ifdef _DEBUG
						Print ("No success wave for index = <%d>", x);
#endif
						// Post ourselves another done message so the activity can continue
						PostMessage (hWnd, MM_WOM_DONE, 0, 0L);
					}
				}
				else
				{
					// Post ourselves another done message so the activity can continue
					PostMessage (hWnd, MM_WOM_DONE, 0, 0L);
				}
			}
		}
	}
}


//************************************************************************
void CCleanupScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	RECT rPos;
	FNAME szFileName;

	// This is a safety catch so that a left button-down can simulate a MM_WOM_DONE
	// to give the activity a kick start - this is only a problem on a few PC's
	// with old sound drivers that lose an occasional done int. from the board (SMS).
	if (m_GameState == WaitingForDone)
	{
		m_GameState = InProgress;
		OnWomDone (NULL, NULL, NULL);
	}

	if (wIntroFlag == TRUE)
	{		
		wIntroFlag = FALSE;
		if (GetSound())
		{
			GetSound()->StopAndFree();
			GetSound()->StartFile (GetPathName (szFileName, "sndtrack.wav"),
									YES/*bLoop*/, 3/*iChannel*/, TRUE);
		}
		GetToon()->SetHintState (TRUE);
	}

	if (m_lpSparkSprite)
	{
		m_lpSparkSprite->Location (&rPos);	

		if (x > rPos.left && x < rPos.right &&
			y > rPos.top  && y < rPos.bottom)
		{
			if (m_lpDotSprite[wPartNum] != NULL)
				m_lpDotSprite[wPartNum]->Show (FALSE);
			if (m_lpAnimalSprite[wPartNum] != NULL)
				m_lpAnimalSprite[wPartNum]->Show (TRUE);

			wParts[wPartNum] = TRUE;
			SetNextDot();
		}
	}
	
}

//************************************************************************
void CCleanupScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CCleanupScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//***************************************************************************
UINT CCleanupScene::OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{
	return(TRUE);
}

//************************************************************************
LPSTR CCleanupScene::GetPathName(LPSTR lpPathName, LPSTR lpFileName)
//************************************************************************
{
	GetToon()->GetContentDir(lpPathName);
	lstrcat(lpPathName, lpFileName);
	return(lpPathName);
}

//************************************************************************
void CCleanupScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	GetRandomNumber (100);
 	GetToon()->SetHintState (FALSE);
	
	wPartNum = 0;
	// Randomly select the first animal
	SetNextAnimal();

	// Free up old sprite
	if (m_lpSparkSprite)
	{
		m_pAnimator->DeleteSprite (m_lpSparkSprite);
		m_lpSparkSprite = NULL;
	}

	m_lpSparkSprite = m_pAnimator->CreateSprite (& ptOrigin);
	if (m_lpSparkSprite)
	{
		GetPathName (szFileName, SPARKLE_BMP);
		m_lpSparkSprite->AddCells (szFileName, SPARKLE_ANIM_CELLS, NULL, NULL);
		m_lpSparkSprite->SetCellsPerSec (10);
		m_lpSparkSprite->Jump (0, 0);
		m_lpSparkSprite->Show (FALSE);
	}

	ButcherAnimal();

	if (GetSound())
	{
		GetSound()->StopAndFree();
		GetSound()->StartFile (GetPathName (szFileName, szIntroWave),
								NO/*bLoop*/, 2/*iChannel*/, FALSE ,m_hWnd);
	}

}

//************************************************************************
void CCleanupScene::ChangeLevel(int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	if (iLevel != GetLevel())
	{
		// Hide all sprites
		if (m_pAnimator)
			m_pAnimator->HideAll();
		GetApp()->GotoScene(m_hWnd, IDD_CLEANUP1+iLevel, m_nNextSceneNo);
	}
}

//************************************************************************
void CCleanupScene::RestartLevel()
//************************************************************************
{
	if (GetSound())
		GetSound()->StopAndFree();

	// If we are in Play mode, go to the next scene
	if (GetApp()->m_fPlaying)
	{
		CGBScene::OnCommand (m_hWnd, IDC_NEXT, NULL, 0);
		return;
	}

	// Hide all field sprites
	m_pAnimator->HideAll();

	// Initialize complete flags for all animals
	for (int i = 0 ; i < NUMBER_OF_CLEAN_ANIMALS ; i++)
		Meal[i].wComplete = FALSE;

	for (i = 0 ; i < MAX_NUMBER_OF_PARTS ; i++)
		wParts[i] = FALSE;     

	wIntroFlag = TRUE;
	wLevelCompleteFlag = FALSE;
	wCount = -1;
	PlayIntro();
}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPCLEANUPSCENE lpScene = (LPCLEANUPSCENE)dwNotifyData;
		lpScene->OnSNotify(lpSprite, Notify);
	}
}


//************************************************************************
void CCleanupScene::OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
}


//************************************************************************
void CCleanupScene::SetNextDot()
//************************************************************************
{
	FNAME szFileName;
    
    wCount = -1;
	SelectDot();
    
	if (wPartNum != -1)
	{
		if (m_lpSparkSprite)
			m_lpSparkSprite->Jump (Meal[wAnimalNum].pAnimal[wPartNum].x + SPARKLE_X_OFFSET,
								Meal[wAnimalNum].pAnimal[wPartNum].y + SPARKLE_Y_OFFSET);
		wCount = TIME_UP;							  
	}
	else
	{
		// POWERHOUSE --> Here is where you place all action taken when the user has
		//					cleaned up the animal.

		// Set the complete flag for this animal
		Meal[wAnimalNum].wComplete = TRUE;

		wPartNum = Meal[wAnimalNum].wParts;
		if (m_lpSparkSprite)
			m_lpSparkSprite->Show (FALSE);

		// Play the complete wave for this animal
		GetPathName (szFileName, Meal[wAnimalNum].szSound);
		if (GetSound())
		{
			GetSound()->StopAndFree();
			// Set the game state to waiting for MM_WOM_DONE
			m_GameState = WaitingForDone;
			GetSound()->StartFile (szFileName, NO/*bLoop*/, 2/*iChannel*/,
									FALSE, m_hWnd);
		}
	}
}


//************************************************************************
void CCleanupScene::ButcherAnimal()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	short i;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	for (i = 0 ; i < Meal[wAnimalNum].wParts ; i++)
	{
		if (m_lpDotSprite[i] != NULL) 
			m_pAnimator->DeleteSprite (m_lpDotSprite[i]);
			
		if (m_lpAnimalSprite[i] != NULL)
			m_pAnimator->DeleteSprite (m_lpAnimalSprite[i]);

		// These sprites are created with the fAddToTop flag set to TRUE so that they
		// will show up on top of the field sprites (SMS).
		m_lpDotSprite[i] = m_pAnimator->CreateSprite (& ptOrigin, 0L, TRUE/*fAddToTop*/);
		GetPathName (szFileName, "DOT.BMP");
		m_lpDotSprite[i]->AddCell (szFileName, NULL);
		m_lpDotSprite[i]->Jump (Meal[wAnimalNum].pAnimal[i].x,Meal[wAnimalNum].pAnimal[i].y);
		m_lpDotSprite[i]->Show (TRUE);
		
		m_lpAnimalSprite[i] = m_pAnimator->CreateSprite (& ptOrigin, 0L, TRUE/*fAddToTop*/);
		GetPathName (szFileName, Meal[wAnimalNum].pAnimal[i].szPartName);
		m_lpAnimalSprite[i]->AddCell (szFileName, NULL);
		m_lpAnimalSprite[i]->Jump (Meal[wAnimalNum].pAnimal[i].x,Meal[wAnimalNum].pAnimal[i].y);
		m_lpAnimalSprite[i]->Show (FALSE);
	}

	if (m_lpSparkSprite)
	{
		m_lpSparkSprite->Jump (Meal[wAnimalNum].pAnimal[wPartNum].x + SPARKLE_X_OFFSET,
						  	Meal[wAnimalNum].pAnimal[wPartNum].y + SPARKLE_Y_OFFSET);
		m_lpSparkSprite->Show (TRUE);
	}

	wPartNum = 0;
	wCount = TIME_UP;	
}

	
//************************************************************************
void CCleanupScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	CGBScene::OnTimer (hWnd, id);
	if (wCount == 0)
	{
		SetNextDot();
	}
	else
	{
		if (wCount > 0)
			--wCount;
	}
}
	

//************************************************************************
void CCleanupScene::SelectDot()
//************************************************************************
{
	DWORD dwRandom,j;
	int i;
	POINT ptOrigin;
		
	wPartNum = -1;	
	
	dwRandom = (DWORD)rand();
	while(dwRandom > 1000)
		dwRandom -= 1000;
	while(dwRandom > 50)
		dwRandom -= 50;
		
	i = 0;
	
//	while (wParts[wPartNum] == TRUE && wPartNum < Meal[wAnimalNum].wParts)
//		++wPartNum;
	
	for (j = 0 ; j < dwRandom ; j++)
	{
		if (wParts[i] == FALSE)
		{
			wPartNum = i;		
		}
		++i;
		if (i > (Meal[wAnimalNum].wParts -1))
			i = 0;
	}
	
	if (wPartNum == -1)
	{
		for (i = 0 ; i < Meal[wAnimalNum].wParts && wPartNum == -1 ; i++)
		{
			if (wParts[i] == FALSE)
			{
				wPartNum = i;		
			}
		}
	}

	ptOrigin.x = 0;
	ptOrigin.y = 0; 
}

//************************************************************************
void CCleanupScene::SetNextAnimal()
//************************************************************************
{
	int i;
	int iCount = 0;
	int iRandom;

	// Count uncompleted animals
	for (i = 0 ; i < NUMBER_OF_CLEAN_ANIMALS ; ++i)
	{
		if (! Meal[i].wComplete)
			iCount++;
	}

	// Get random number 0 to iCount-1
	iRandom = GetRandomNumber(iCount);

	// Find this animal
	iCount = 0;
	for (i = 0 ; i < NUMBER_OF_CLEAN_ANIMALS ; ++i)
	{
		if (! Meal[i].wComplete)
		{
			if (iCount == iRandom)
			{
				wAnimalNum = i;
				break;
			}
			++iCount;
		}
	}
}

//************************************************************************
BOOL CCleanupScene::LevelComplete()
//************************************************************************
{
	for (int i = 0 ; i < NUMBER_OF_CLEAN_ANIMALS ; ++i)
	{
		if (! Meal[i].wComplete)
			return FALSE;
	}
	return TRUE;
}

//************************************************************************
BOOL CCleanupParser::GetSuccessWaves (LPCLEANUPSCENE lpScene, LPSTR lpEntry,
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
		lpValue = GetNextValue (& lpValues);
		lstrcpy (lpScene->m_szSuccessWaves[i], lpValue);
		lpScene->m_iNumberSuccessWaves++;
	}
 	return (TRUE);
}

//************************************************************************
BOOL CCleanupParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPCLEANUPSCENE lpScene = (LPCLEANUPSCENE)dwUserData;


	if (!lstrcmpi(lpKey, "sparkleanimcells"))
		GetInt(&lpScene->SPARKLE_ANIM_CELLS, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "sparklexoffset"))
	{
		GetInt(&lpScene->SPARKLE_X_OFFSET, lpEntry, lpValues, nValues);
		lpScene->SPARKLE_X_OFFSET *= -1;		
	}
		
	if (!lstrcmpi(lpKey, "sparkleyoffset"))
	{
		GetInt(&lpScene->SPARKLE_Y_OFFSET, lpEntry, lpValues, nValues);
		lpScene->SPARKLE_Y_OFFSET *= -1;				
	}

	if (!lstrcmpi(lpKey, "sparklebmp"))
		GetFilename(lpScene->SPARKLE_BMP, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "timeup"))
		GetInt(&lpScene->TIME_UP, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "introwav"))
		GetFilename(lpScene->szIntroWave, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "successwavs"))
		GetSuccessWaves (lpScene, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "parts"))
	{
		lpScene->wPart = -1;
		++lpScene->wAnimal;
		lpScene->NUMBER_OF_CLEAN_ANIMALS = lpScene->wAnimal + 1;
		GetInt (& lpScene->Meal[lpScene->wAnimal].wParts, lpEntry, lpValues, nValues);
    }
    
	if (!lstrcmpi(lpKey, "wave"))
		GetFilename(lpScene->Meal[lpScene->wAnimal].szSound, lpEntry, lpValues, nValues);


	if (!lstrcmpi(lpKey, "wholebmp"))
		GetFilename(lpScene->Meal[lpScene->wAnimal].pAnimal[lpScene->Meal[lpScene->wAnimal].wParts].szPartName,
					lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "wholex"))
		GetInt(&lpScene->Meal[lpScene->wAnimal].pAnimal[lpScene->Meal[lpScene->wAnimal].wParts].x, 
				lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "wholey"))
		GetInt(&lpScene->Meal[lpScene->wAnimal].pAnimal[lpScene->Meal[lpScene->wAnimal].wParts].y, 
				lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "partbmp"))
	{
		++lpScene->wPart;
		GetFilename(lpScene->Meal[lpScene->wAnimal].pAnimal[lpScene->wPart].szPartName, lpEntry, lpValues, nValues);
    }
    
	if (!lstrcmpi(lpKey, "x"))
		GetInt(&lpScene->Meal[lpScene->wAnimal].pAnimal[lpScene->wPart].x, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi(lpKey, "y"))
		GetInt(&lpScene->Meal[lpScene->wAnimal].pAnimal[lpScene->wPart].y, lpEntry, lpValues, nValues);

	return (TRUE);
}

