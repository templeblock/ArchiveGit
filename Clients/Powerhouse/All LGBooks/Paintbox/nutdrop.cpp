#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "pain.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "nutdrop.h"
#include "nutdef.h"
#include "randseq.h"


// Nut drop locations for level 1
static LocList LocLevel1[] = 
{
	// x		// y
	180,		110,
	400,		110
};

// Nut drop locations for level 2
static LocList LocLevel2[] = 
{
	// x		// y
	145,		110,
	295,		110,
	452,		110
};

// Nut drop locations for level 3
static LocList LocLevel3[] = 
{
	// x		// y
	134,		110,
	250,		110,
	363,		110,
	483,		110
};

// Parameter lists for each level
static LevelParamList sLevel[] =
{
	// Level 1
	2,						// Chute count
	&LocLevel1[0],			// Chute drop locations

	// Level 2						   			
	3,
	&LocLevel2[0],

	// Level 3						   			
	4,
	&LocLevel3[0],
};

//************************************************************************
// Prototypes
//************************************************************************
LOCAL void CALLBACK OnSpriteNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

//************************************************************************
// Globals
//************************************************************************
static WORD wRandomSeed;
static BOOL	fLaunching;

class CNutDropParser : public CParser
{
public:
	CNutDropParser(LPSTR lpTableData) : CParser(lpTableData) {}

protected:
	BOOL HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPNUTDROPSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CNutDropScene::CNutDropScene (int nNextSceneNo) : CGBScene (nNextSceneNo)
//************************************************************************
{
	m_pSound = NULL;
	m_lpPlayerSprite = NULL;
	m_SoundPlaying = NotPlaying;
	m_nPlayerSpeed = 500;
	m_nPlayerCells = 0;
	m_nObjectSpeed = 150;
	m_nMatchesNeeded = 8;
	m_nGoodPicks = 0;
	m_nDropTimer = 20;
	m_nKeyboardDistance = 20;
	m_nScoreCells = 0;
	m_fTimerStarted = FALSE;

	m_ptLastMouseXY.x = m_ptLastMouseXY.y = 0;
	m_rLeftBasket.left = m_rLeftBasket.top = 0;
	m_rLeftBasket.right = m_rLeftBasket.bottom = 50;
	m_rRightBasket.left = m_rRightBasket.top = 0;
	m_rRightBasket.right = m_rRightBasket.bottom = 50;

	m_lpCaughtScore[0] = m_lpCaughtScore[1] = NULL;

	m_szPlayerBmp[0] = '\0';
	m_szScoreBmp[0] = '\0';

	// Init. wave filename storage
	m_szSuccessPlay[0] = '\0';
	m_szIntroWave[0] = '\0';
	m_szSoundTrack[0] = '\0';
	m_szGotOneWave[0] = '\0';
	m_szJumpWave[0] = '\0';
	for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessLevel[i][0] = '\0';

	// Init. the object sprite structure
	for (i = 0 ; i < MAX_PIECES ; i++)
	{
		m_ObjectList[i].m_szImage[0] = '\0';
		m_ObjectList[i].m_lpSprite = NULL;
		m_ObjectList[i].m_nTotalCells = 0;
		m_ObjectList[i].m_fOneToCatch = FALSE;
		m_ObjectList[i].m_nState = StateUnused;
	}

}

//************************************************************************
CNutDropScene::~CNutDropScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;

	if (m_nDropTimer)
	{
		m_fTimerStarted = FALSE;
		KillTimer (m_hWnd, DROP_TIMER);
	}
}

//************************************************************************
int CNutDropScene::GetLevel()
//************************************************************************
{
	if (m_nSceneNo == IDD_NUTDROPI)
		return (0);
	else
		return (m_nSceneNo - IDD_NUTDROP1);
}

//************************************************************************
BOOL CNutDropScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);
	
	m_pSound = new CSound (TRUE);
	if (m_pSound)
		m_pSound->Open (NULL);

	// Keyboard keys that we want
	App.SetKeyMapEntry (m_hWnd, VK_UP);
	App.SetKeyMapEntry (m_hWnd, VK_DOWN);
	App.SetKeyMapEntry (m_hWnd, VK_RIGHT);
	App.SetKeyMapEntry (m_hWnd, VK_LEFT);
	
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
			CNutDropParser parser (lpTableData);
			parser.ParseTable ((DWORD)this);
		}
		if (hData)
		{
			UnlockResource (hData);
			FreeResource (hData);
		}

		m_pAnimator->SetClipRect (&m_rGameArea);
	}

	if (m_nSceneNo == IDD_NUTDROPI || m_nSceneNo == IDD_NUTDROP1)
		m_iLevel = 0;
	else if (m_nSceneNo == IDD_NUTDROP2)
		m_iLevel = 1;
	else if (m_nSceneNo == IDD_NUTDROP3)
		m_iLevel = 2;

	// Seed the random number generator
	wRandomSeed = GetRandomNumber (MAX_PIECES);

	return (TRUE);
}

//************************************************************************
void CNutDropScene::ToonInitDone()
//************************************************************************
{
	if (GetToon())
		GetToon()->SetHintState(TRUE);

	CreateSprites();
	PlayIntro(m_hWnd);
}

//************************************************************************
void CNutDropScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
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
			PlayIntro(NULL);
			break;
		}

		default:
			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
			break;
	}
}

//************************************************************************
BOOL CNutDropScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	// set cursor visibility based on game area
	// only turn off cursor when we have capture
	if ( GetToon() )
	{
		if (PtInRect(&GetToon()->m_rGlobalClip, pt))
		{
			SetCursor(NULL);
			// must set real return value for us to take over the cursor
	   		SetDlgMsgResult(hWnd, WM_SETCURSOR, TRUE);
			return(TRUE);
		}
	}
	return(FALSE);
}

//************************************************************************
BOOL CNutDropScene::OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, WM_SETCURSOR, OnSetCursor);
		HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);
		HANDLE_DLGMSG(hDlg, MM_WOM_DONE, OnWomDone);

		default:
			return (FALSE);
	}
}

//************************************************************************
void CNutDropScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_SoundPlaying == IntroPlaying)
	{
		m_SoundPlaying = NotPlaying;
	 	m_pSound->StopChannel (NORMAL_CHANNEL);
		StartGame();
	}	
	else
	if (m_SoundPlaying == SuccessPlaying)
	{
		m_SoundPlaying = NotPlaying;
	 	GetSound()->StopChannel(NORMAL_CHANNEL);
		// Start the looping soundtrack if one is defined
		FNAME szFileName;
		if (GetSound() && m_szSoundTrack[0] != '\0')
			GetSound()->StartFile (GetPathName (szFileName, m_szSoundTrack), YES/*bLoop*/,
							MUSIC_CHANNEL, FALSE);
		// If Play mode, go to the next scene
		if (m_nSceneNo == IDD_NUTDROPI)
			FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		else
			StartGame();
	}	
}

//************************************************************************
void CNutDropScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CNutDropScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	static BOOL fInuse = FALSE;

	if (fInuse)
		return;
	fInuse = TRUE;

	// Reduce mouse sensitivity
	if (abs (m_ptLastMouseXY.x - x) < 5)
	{
		fInuse = FALSE;
		return;
	}

	if (!m_lpPlayerSprite)
	{
		fInuse = FALSE;
		return;
	}

	int iWidth, iTmp;
	m_lpPlayerSprite->GetMaxSize (&iWidth, &iTmp);

	// See if the player changed directions
	if (x < m_ptLastMouseXY.x)
		ChangeDirection (WALKLEFT);
	else
		ChangeDirection (WALKRIGHT);

	m_lpPlayerSprite->SetCellsPerSec (10);
	if (x < m_nScreenLeftEdge)
		m_lpPlayerSprite->Move (m_nScreenLeftEdge, m_nScreenBottom);
	else
	if ((x + iWidth) > m_nScreenRightEdge)
		m_lpPlayerSprite->Move (m_nScreenRightEdge - iWidth, m_nScreenBottom);
	else
	{
		iTmp = x;
		if ((x + iWidth) > m_nScreenRightEdge)
			iTmp = m_nScreenRightEdge - iWidth;
		m_lpPlayerSprite->Move (iTmp, m_nScreenBottom);
	}

	m_ptLastMouseXY.x = x;
	m_ptLastMouseXY.y = y;
	fInuse = FALSE;
}

//************************************************************************
void CNutDropScene::OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	if (vk != VK_LEFT && vk != VK_RIGHT)
	{
		CGBScene::OnKey (hWnd, vk, fDown, cRepeat, flags);
		return;
	}		

	static BOOL fInuse = FALSE;
	if (!m_lpPlayerSprite)
		return;

	// In case of fast key repeat
	if (fInuse)
		return;
	fInuse = TRUE;

	if (m_SoundPlaying == IntroPlaying || m_SoundPlaying == SuccessPlaying)
	{
	 	m_pSound->StopChannel (NORMAL_CHANNEL);
		m_SoundPlaying = NotPlaying;
		StartGame();
	}

	RECT rLoc;
	int iWidth, iTmp;
	// Get current player location and size info.
	m_lpPlayerSprite->Location (&rLoc);
	m_lpPlayerSprite->GetMaxSize (&iWidth, &iTmp);

	switch (vk)
	{
		// Move the player left
		case VK_LEFT:
			ChangeDirection (WALKLEFT);
			if (rLoc.left - m_nKeyboardDistance < m_nScreenLeftEdge)
			{
				rLoc.left = m_nScreenLeftEdge;
				rLoc.right = rLoc.left + iWidth;
			}
			else
			{
				rLoc.left -= m_nKeyboardDistance;
				rLoc.right -= m_nKeyboardDistance;
			}
			break;

		// Move the player right
		case VK_RIGHT:
			ChangeDirection (WALKRIGHT);
			if ((rLoc.left + iWidth + m_nKeyboardDistance) > m_nScreenRightEdge)
			{
				rLoc.left = m_nScreenRightEdge - iWidth;
				rLoc.right = rLoc.left + iWidth;
			}
			else
			{
				rLoc.left += m_nKeyboardDistance;
				rLoc.right += m_nKeyboardDistance;
			}
			break;
	}

	m_lpPlayerSprite->Move (rLoc.left, m_nScreenBottom);
	m_lpPlayerSprite->CycleCells();
	m_lpPlayerSprite->Draw();
	fInuse = FALSE;
}

//************************************************************************
void CNutDropScene::ChangeDirection (enum WalkDirection eDir)
//************************************************************************
{
	if (m_WalkDir == eDir)
		return;

	if (eDir == WALKLEFT)
	{
		m_lpPlayerSprite->ActivateCells(0, (m_nPlayerCells/2)-1, NO);
		m_lpPlayerSprite->ActivateCells(m_nPlayerCells/2, m_nPlayerCells, YES);
		m_WalkDir = WALKLEFT;
	}
	else
	{
		m_lpPlayerSprite->ActivateCells(0, (m_nPlayerCells/2)-1, YES);
		m_lpPlayerSprite->ActivateCells(m_nPlayerCells/2, m_nPlayerCells, NO);
		m_WalkDir = WALKRIGHT;
	}

	m_lpPlayerSprite->CycleCells();
	m_lpPlayerSprite->Draw();
}

//************************************************************************
void CNutDropScene::OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	if (m_SoundPlaying == IntroPlaying || m_SoundPlaying == SuccessPlaying)
		OnLButtonDown (hWnd, FALSE/*fDoubleClick*/, 0/*x*/, 0/*y*/, NULL/*keyFlags*/);
}

//***************************************************************************
UINT CNutDropScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
//***************************************************************************
{
	if (codeNotify == MCI_NOTIFY_SUCCESSFUL)
	{
	}
	return(TRUE);
}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPNUTDROPSCENE lpScene = (LPNUTDROPSCENE)dwNotifyData;
		lpScene->OnSNotify (lpSprite, Notify);
	}
}

//************************************************************************
void CNutDropScene::OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (! lpSprite)
		return;

	if (Notify == SN_MOVED)
	{
		// Check for collision
		if (lpSprite != m_lpPlayerSprite)
			CheckObjectCollision (lpSprite);
	}
	else
	if (Notify == SN_MOVEDONE)
	{
		if (m_lpPlayerSprite && lpSprite == m_lpPlayerSprite)
			m_lpPlayerSprite->SetCellsPerSec (0);
	}
}

//************************************************************************
void CNutDropScene::CheckObjectCollision (LPSPRITE lpSprite)
//************************************************************************
{
	if (!m_lpPlayerSprite || !m_pAnimator)
		return;

	// See if they've picked enough already
	if (m_nGoodPicks >= m_nMatchesNeeded)
		return;

	static BOOL fBusy = FALSE;
	if (fBusy)
		return;
	fBusy = TRUE;

	RECT rBasket, rBasketX;
	rBasket = (m_WalkDir == WALKLEFT ? m_rLeftBasket : m_rRightBasket );
	m_lpPlayerSprite->Location (&rBasketX);
	OffsetRect( &rBasket, rBasketX.left, rBasketX.top );
	IntersectRect( &rBasket, &rBasket, &rBasketX );

	if (m_pAnimator->CheckCollision (m_lpPlayerSprite, lpSprite, &rBasket))
	{
		int idx = GetObjectIndex (lpSprite);
		if (idx == -1 || m_ObjectList[idx].m_nState == StateGoodPick)
		{
			fBusy = FALSE;
			return;
		}

		m_ObjectList[idx].m_nState = StateGoodPick;
		m_nGoodPicks++;

		// Play the got one sound if one is defined
		FNAME szFileName;
		GetPathName (szFileName, m_szGotOneWave);
		if (m_nGoodPicks < m_nMatchesNeeded)
		{
			if (GetSound() && m_szGotOneWave[0] != '\0')
			{
				GetSound()->StopChannel (NORMAL_CHANNEL);
				GetSound()->StartFile (szFileName, NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, FALSE);
			}
		}
		else
			PlaySound(szFileName, FALSE); // how to play with wait AND wavemix

		// Hide this object
		lpSprite->Show (FALSE);

		// Update the caught score
		UpdateSpriteScore (m_lpCaughtScore[0], m_lpCaughtScore[1], m_nGoodPicks);
		
		// See if they've picked enough to win this level
		if (m_nGoodPicks >= m_nMatchesNeeded)
			PlayLevelSuccessWave();
	}
	fBusy = FALSE;
}

//************************************************************************
int CNutDropScene::GetObjectIndex (LPSPRITE lpSprite)
//************************************************************************
{
	if (!lpSprite)
		return -1;

	for (int i = 0 ; i < MAX_PIECES ; i++)
	{
		if (m_ObjectList[i].m_lpSprite == lpSprite)
			return i;
	}

	return -1;
}

//************************************************************************
void CNutDropScene::StartGame()
//************************************************************************
{
	m_SoundPlaying = NotPlaying;
	ResetAllObjects();
	SelectObject();

	// Reset the scores
	m_nGoodPicks = 0;
	ResetSpriteScore (m_lpCaughtScore[0], m_lpCaughtScore[1]);

	// Start up the timers to release the objects
	if (m_nDropTimer && ! m_fTimerStarted)
	{
		m_fTimerStarted = TRUE;
		SetTimer (m_hWnd, DROP_TIMER, m_nDropTimer, NULL);
	}
}

//************************************************************************
void CNutDropScene::ResetObjects()
//************************************************************************
{
	for (int i = 0 ; i < MAX_PIECES ; i++)
	{
		if (m_ObjectList[i].m_lpSprite)
		{
		 	if (m_ObjectList[i].m_nState == StateUsed ||
		 		m_ObjectList[i].m_nState == StateGoodPick)
		 	{
				m_ObjectList[i].m_lpSprite->Show (FALSE);
				m_ObjectList[i].m_nState = StateUnused;
			}
		}
	}
}

//************************************************************************
void CNutDropScene::ResetAllObjects()
//************************************************************************
{
	for (int i = 0 ; i < MAX_PIECES ; i++)
	{
		if (m_ObjectList[i].m_lpSprite)
		{
			m_ObjectList[i].m_lpSprite->Show (FALSE);
			m_ObjectList[i].m_lpSprite->DeleteAllActions();
		}
		m_ObjectList[i].m_nState = StateUnused;
		m_ObjectList[i].m_fOneToCatch = FALSE;
	}
}

//************************************************************************
void CNutDropScene::ResetSpriteScore (LPSPRITE lpScore1, LPSPRITE lpScore2)
//************************************************************************
{
	if (!lpScore1 || !lpScore2)
		return;

	lpScore1->SetCurrentCell (m_nScoreCells - 1);
	lpScore2->SetCurrentCell (0);
	lpScore1->Draw();
	lpScore2->Draw();
}

//************************************************************************
void CNutDropScene::UpdateSpriteScore (LPSPRITE lpScore1, LPSPRITE lpScore2, int iScore)
//************************************************************************
{
	if (!lpScore1 || !lpScore2)
		return;

	int iTmp = iScore;

	if (iTmp > 99)
		return;

	if (iTmp < 10)
	{
		lpScore2->SetCurrentCell (iTmp);
		lpScore2->Draw();
	}
	else
	{
		lpScore1->SetCurrentCell (iTmp / 10);
		while (iTmp > 10)
			iTmp -= 10;
		if (iTmp == 10)
			iTmp = 0;
		lpScore2->SetCurrentCell (iTmp);
		lpScore1->Draw();
		lpScore2->Draw();
	}
}

//************************************************************************
void CNutDropScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	if (id != DROP_TIMER)
	{
		CGBScene::OnTimer (hWnd, id);
		return;
	}

	if (m_SoundPlaying == SuccessPlaying || m_SoundPlaying == IntroPlaying)
		return;

	// Select the next object
	if (SelectObject() == -1)
	{
		// If out of objects, re-cycle them
		ResetObjects();
		SelectObject();
	}
}

//************************************************************************
int CNutDropScene::SelectObject()
//************************************************************************
{
	CRandomSequence RandomSequence;
	wRandomSeed = GetRandomNumber (MAX_PIECES);
	RandomSequence.Init ((DWORD)MAX_PIECES, (DWORD)wRandomSeed);

	if (fLaunching)
		return 0;
	fLaunching = TRUE;

	BOOL bFound = FALSE;
	int nSelectedObject = -1;

	// Note - this still randomly picks the pieces, which isn't really needed
	// the way the game is now but I left it in case I need it later.
	for (int i = 0 ; i < MAX_PIECES ; i++)
	{		
		nSelectedObject = (int)RandomSequence.GetNextNumber();
		// Find an item that hasn't been used
		if (m_ObjectList[nSelectedObject].m_nState == StateUnused && 
		  m_ObjectList[nSelectedObject].m_lpSprite != NULL)
		{
			bFound = TRUE;
            break;
		}
	}

	// Return -1 if we ran out of items
	if (! bFound)
	{
		fLaunching = FALSE;
		return -1;
	}

	m_ObjectList[nSelectedObject].m_nState = StateUsed;

	// Start this object falling from a randomly selected chute
	if (m_ObjectList[nSelectedObject].m_lpSprite)
	{
		int iCX, iCY, iX;
		m_ObjectList[nSelectedObject].m_lpSprite->GetMaxSize (&iCX, &iCY);
		// Get a random chute index based on number of chutes
		iX = GetRandomNumber (sLevel[m_iLevel].m_wChuteCount);
		// Add a random offset
		int iOffset = GetRandomNumber (200) - 100;		

		// Get the coordinates from the structure for this level
		iCX = sLevel[m_iLevel].m_lpLocs[iX].x + iOffset;
		iCY = sLevel[m_iLevel].m_lpLocs[iX].y;

		if (iCX < m_nScreenLeftEdge)
			iCX = m_nScreenLeftEdge;
		if (iCX > m_nScreenRightEdge)
			iCX = m_nScreenRightEdge;

		m_ObjectList[nSelectedObject].m_lpSprite->AddCmdJump (iCX, iCY);
		m_ObjectList[nSelectedObject].m_lpSprite->AddCmdShow();
		// Move the object to the bottom of the screen
		m_ObjectList[nSelectedObject].m_lpSprite->AddCmdMove (iCX, m_rGameArea.bottom);
	}
	fLaunching = FALSE;
	return nSelectedObject;
}

//************************************************************************
void CNutDropScene::PlayIntro(HWND hWnd/*NULL for no WOM_DONE*/)
//************************************************************************
{
	FNAME szFileName;

	// Play the intro wave if one is defined
	if (GetSound() && m_szIntroWave[0] != '\0')
	{
		m_SoundPlaying = IntroPlaying;
		GetSound()->StopChannel (NORMAL_CHANNEL);
		GetSound()->StartFile (GetPathName (szFileName, m_szIntroWave), NO/*bLoop*/, 
								NORMAL_CHANNEL/*iChannel*/, FALSE, hWnd);
	}
}

//************************************************************************
void CNutDropScene::PlayLevelSuccessWave()
//************************************************************************
{
	// Get the sound to play based on the level
	int iCount = 0;
	for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
	{
		if (m_szSuccessLevel[i][0] != '\0')
			iCount++;
	}

	LPSTR lpWave;
	if (iCount <= 0)
	{
		lpWave = m_szSuccessLevel[0];
	}
	else
	{
		i = GetRandomNumber (iCount);
		lpWave = m_szSuccessLevel[i];
	}

	if (*lpWave != '\0')
	{
		FNAME szFileName;
		if (GetSound() )
		{
			m_SoundPlaying = SuccessPlaying;
			GetSound()->StopChannel(NORMAL_CHANNEL);
			if (m_nSceneNo == IDD_NUTDROPI)
			{ // Play this wave without wavemix
				LPSOUND lpSound = new CSound;
				if ( lpSound )
				{
					GetSound()->Activate (FALSE);
					lpSound->StartFile( GetPathName(szFileName, lpWave),
						NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, TRUE/*bWait*/, m_hWnd);
					delete lpSound;
					GetSound()->Activate (TRUE);
					FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
				}
			}
			else
				GetSound()->StartFile(GetPathName (szFileName, lpWave), NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, FALSE/*bWait*/, m_hWnd);
		}
	}
}

//************************************************************************
void CNutDropScene::PlayClickWave (BOOL fSync/*=TRUE*/)
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
	else if (m_pSound)
	{
		m_pSound->StopAndFree();
		if (GetToon()->FindContent (CLICK_WAVE, TRUE, szFileName))
			m_pSound->StartFile (szFileName, FALSE, HINT_CHANNEL, TRUE);
	}
}

//************************************************************************
void CNutDropScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS - 1;

	App.GotoScene (m_hWnd, IDD_NUTDROP1 + iLevel, m_nNextSceneNo);
}

//************************************************************************
void CNutDropScene::CreateSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	// Create the object sprites
	for (int i = 0 ; i < MAX_PIECES ; i++)
	{
		if (m_ObjectList[i].m_szImage[0] != '\0')
		{
			m_ObjectList[i].m_lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_ObjectList[i].m_lpSprite)
			{
				GetPathName (szFileName, m_ObjectList[i].m_szImage);
				// Set up the sprite parameters
				m_ObjectList[i].m_lpSprite->AddCells (szFileName, m_ObjectList[i].m_nTotalCells, NULL);
				m_ObjectList[i].m_lpSprite->SetCellsPerSec (10);
				m_ObjectList[i].m_lpSprite->SetSpeed (m_nObjectSpeed);
				m_ObjectList[i].m_lpSprite->SetCycleBack (TRUE);
				m_ObjectList[i].m_lpSprite->Jump (0, 0);
				m_ObjectList[i].m_lpSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);    
				m_ObjectList[i].m_lpSprite->Show (FALSE);
			}
		}
	}

	// Create the player sprite
	m_lpPlayerSprite = m_pAnimator->CreateSprite (&ptOrigin);
	if (m_lpPlayerSprite)
	{
		if (! m_nPlayerCells)
			m_nPlayerCells = 1;
		GetPathName (szFileName, m_szPlayerBmp); 
		m_lpPlayerSprite->AddCells (szFileName, m_nPlayerCells, NULL);
		m_lpPlayerSprite->ActivateCells(0, (m_nPlayerCells/2)-1, YES);
		m_lpPlayerSprite->ActivateCells(m_nPlayerCells/2, m_nPlayerCells, NO);
		m_lpPlayerSprite->SetCellsPerSec (0);
		m_lpPlayerSprite->SetSpeed (m_nPlayerSpeed);
		m_lpPlayerSprite->Jump (m_nScreenLeftEdge, m_nScreenBottom);
		m_lpPlayerSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);    
		m_lpPlayerSprite->Show (TRUE);
	}

	// Initially walking right
	m_WalkDir = WALKRIGHT;

	// Create the score sprites
	m_lpCaughtScore[0] = CreateScoreSprite (m_ptCaughtScore.x, m_ptCaughtScore.y, 10);
	m_lpCaughtScore[1] = CreateScoreSprite (m_ptCaughtScore.x + m_nScoreWidth, m_ptCaughtScore.y, 0);
}

//************************************************************************
LPSPRITE CNutDropScene::CreateScoreSprite (int x, int y, int iCell)
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	LPSPRITE lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
	if (lpSprite && m_szScoreBmp[0] != '\0')
	{
		GetPathName (szFileName, m_szScoreBmp); 
		lpSprite->AddCells (szFileName, m_nScoreCells, NULL);
		lpSprite->SetCellsPerSec (0);
		lpSprite->SetCurrentCell (iCell);
		lpSprite->Jump (x, y);
		lpSprite->Show (TRUE);
	}
	return lpSprite;
}


//************************************************************************
BOOL CNutDropParser::GetSuccessWaves (LPNUTDROPSCENE lpScene, LPSTR lpEntry,
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
		lstrcpy (lpScene->m_szSuccessLevel[i], lpValue);
	}
 	return (TRUE);
}

//************************************************************************
BOOL CNutDropParser::HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues,
								int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPNUTDROPSCENE lpScene = (LPNUTDROPSCENE)dwUserData;
	int	iTmp;

	if (!lstrcmpi (lpKey, "gamearea"))
		GetRect (&lpScene->m_rGameArea, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "item"))
	{
		if (nIndex < MAX_PIECES)
			GetFilename (lpScene->m_ObjectList[nIndex].m_szImage, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "tcells"))
	{
		if (nIndex < MAX_PIECES)
			GetInt (&lpScene->m_ObjectList[nIndex].m_nTotalCells, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "droptimer"))
	{
		// Drop timer in tenths of a second
		GetInt (&iTmp, lpEntry, lpValues, nValues);
		if (iTmp < 0)
			lpScene->m_nDropTimer = 0;
		else
			lpScene->m_nDropTimer = 100 * iTmp;
	}

	if (!lstrcmpi (lpKey, "introwave"))
		GetFilename (lpScene->m_szIntroWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "gotonewave"))
		GetFilename (lpScene->m_szGotOneWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "Player"))
		GetFilename (lpScene->m_szPlayerBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "LeftBasket"))
		GetRect (&lpScene->m_rLeftBasket, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "RightBasket"))
		GetRect (&lpScene->m_rRightBasket, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PlayerCells"))
		GetInt (&lpScene->m_nPlayerCells, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "screenleftedge"))
		GetInt (&lpScene->m_nScreenLeftEdge, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi (lpKey, "screenrightedge"))
		GetInt (&lpScene->m_nScreenRightEdge, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "screenbottom"))
		GetInt (&lpScene->m_nScreenBottom, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "successlevel"))
		GetSuccessWaves (lpScene, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PlayerSpeed"))
		GetInt (&lpScene->m_nPlayerSpeed, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "dropspeed"))
		GetInt (&lpScene->m_nObjectSpeed, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "matchesneeded"))
		GetInt (&lpScene->m_nMatchesNeeded, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "jumpwave"))
		GetFilename (lpScene->m_szJumpWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "keyboardmove"))
		GetInt (&lpScene->m_nKeyboardDistance, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "CaughtScoreLoc"))
		GetPoint (&lpScene->m_ptCaughtScore, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "ScoreWidth"))
		GetInt (&lpScene->m_nScoreWidth, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "ScoreBmp"))
		GetFilename (lpScene->m_szScoreBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "ScoreCells"))
		GetInt (&lpScene->m_nScoreCells, lpEntry, lpValues, nValues);

	return (TRUE);
}

