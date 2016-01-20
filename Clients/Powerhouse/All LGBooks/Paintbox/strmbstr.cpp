#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "pain.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "strmbstr.h"
#include "nutdef.h"
#include "randseq.h"


// Paint drop locations for level 1
static CStormbusterScene::LocList LocLevel1[] = 
{
	// x		// y
	180,		40,
	400,		40
};

// Paint drop locations for level 2
static CStormbusterScene::LocList LocLevel2[] = 
{
	// x		// y
	145,		40,
	295,		40,
	452,		40
};

// Paint drop locations for level 3
static CStormbusterScene::LocList LocLevel3[] = 
{
	// x		// y
	134,		40,
	250,		40,
	363,		40,
	483,		40
};

// Parameter lists for each level
static CStormbusterScene::LevelParamList sLevel[] =
{
	// Level 1
	2,						// Drop count
	&LocLevel1[0],			// Drop locations

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
static BOOL	fPainting;       
static 

class CStormbusterParser : public CParser
{
public:
	CStormbusterParser (LPSTR lpTableData) : CParser(lpTableData) {}

protected:
	BOOL HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPSTORMBUSTERSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CStormbusterScene::CStormbusterScene (int nNextSceneNo) : CGBScene (nNextSceneNo)
//************************************************************************
{
	m_pSound = NULL;
	m_lpPlayerSprite = NULL;
	m_lpPainterSprite = NULL;
	m_SoundPlaying = NotPlaying;
	m_nPlayerSpeed = 500;
	m_nPlayerCells = 0;
	m_nPainterCells = 0;
	m_nObjectSpeed = 150;
	m_nGoodPicks = 0;
	m_nDropTimer = 20;
	m_nKeyboardDistance = 20;
	m_fTimerStarted = FALSE;
	m_nLastColor = -1;
	m_fPickInOrder = FALSE;
	m_bPenalty = FALSE;

	m_ptLastMouseXY.x = m_ptLastMouseXY.y = 0;
	m_rLeftBasket.left = m_rLeftBasket.top = 0;
	m_rLeftBasket.right = m_rLeftBasket.bottom = 50;
	m_rRightBasket.left = m_rRightBasket.top = 0;
	m_rRightBasket.right = m_rRightBasket.bottom = 50;

	m_szPlayerBmp[0] = '\0';
	m_szPainterBmp[0] = '\0';
	m_szPreviewBmp[0] = '\0';

	m_lpRainbow = NULL;
	m_ptRainbowPos.x = m_ptRainbowPos.y = 0;

	// Init. wave filename storage
	m_szSuccessPlay[0] = '\0';
	m_szIntroWave[0] = '\0';
	m_szSoundTrack[0] = '\0';
	m_szGotOneWave[0] = '\0';
	m_szJumpWave[0] = '\0';
	m_szBadCatchWave[0] = '\0';
	for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessLevel[i][0] = '\0';

	// Init. the object sprite structure
	for (i = 0 ; i < MAX_DROPS ; i++)
	{
		m_ObjectList[i].m_szImage[0] = '\0';
		m_ObjectList[i].m_lpSprite = NULL;
		m_ObjectList[i].m_nTotalCells = 0;
		m_ObjectList[i].m_fOneToCatch = FALSE;
		m_ObjectList[i].m_nState = StateUnused;
		m_ObjectList[i].m_nColor = 0;
	}

	for (i = 0 ; i < MAX_STRIPES ; i++)
	{
		m_lpStripes[i] = NULL;
		m_aszStripeBmps[i][0] = '\0';
		m_lpPreview[i] = NULL;
		m_arnStripeColors[i] = 0;
	}

	m_bSourceRed = -1;
	m_bSourceGreen = -1;
	m_bSourceBlue = -1;

}

//************************************************************************
CStormbusterScene::~CStormbusterScene()
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
int CStormbusterScene::GetLevel()
//************************************************************************
{
	if (m_nSceneNo == IDD_STRMBSTRI)
		return (0);
	else
		return (m_nSceneNo - IDD_STRMBSTR1);
}

//************************************************************************
BOOL CStormbusterScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
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
			CStormbusterParser parser (lpTableData);
			parser.ParseTable ((DWORD)this);
		}
		if (hData)
		{
			UnlockResource (hData);
			FreeResource (hData);
		}

		m_pAnimator->SetClipRect (&m_rGameArea);
	}

	if (m_nSceneNo == IDD_STRMBSTRI || m_nSceneNo == IDD_STRMBSTR1)
	{
		m_iLevel = 0;
		m_nMatchesNeeded = 4;
	}
	else if (m_nSceneNo == IDD_STRMBSTR2)
	{
		m_iLevel = 1;
		m_nMatchesNeeded = 6;
	}
	else if (m_nSceneNo == IDD_STRMBSTR3)
	{
		m_iLevel = 2;
		m_nMatchesNeeded = 8;
	}

	// Seed the random number generator
	wRandomSeed = GetRandomNumber (MAX_DROPS);

	return (TRUE);
}

//************************************************************************
void CStormbusterScene::ToonInitDone()
//************************************************************************
{
	if (GetToon())
		GetToon()->SetHintState(TRUE);

	CreateSprites();
	PlayIntro(m_hWnd);
}

//************************************************************************
void CStormbusterScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
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
BOOL CStormbusterScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
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
BOOL CStormbusterScene::OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
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
void CStormbusterScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
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
		if (m_nSceneNo == IDD_STRMBSTRI)
			FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		else
			StartGame();
	}
}

//************************************************************************
void CStormbusterScene::TakeStripeAway()
//************************************************************************
{
	int iObjIdx;

	if (m_bPenalty && m_nGoodPicks)
	{
		for (int i = m_nMatchesNeeded - 1 ; i >= 0 ; i--)
		{
			if (m_lpStripes[i] != NULL && m_lpStripes[i]->IsVisible())
			{
				iObjIdx = GetObjectIndexByColor (m_arnStripeColors[i]);
				if (iObjIdx != -1)
					m_ObjectList[iObjIdx].m_nState = StateUnused;
				m_lpStripes[i]->Show (FALSE);  

// Craig - Added //
				m_lpPreview[i]->Show (TRUE);				
//****************//
				m_nGoodPicks--;
				return;
			}
		}
	}
}

//************************************************************************
void CStormbusterScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CStormbusterScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	static BOOL fInuse = FALSE;

	if (fInuse || fPainting)
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
void CStormbusterScene::OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
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
	if (fInuse || fPainting)
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
void CStormbusterScene::ChangeDirection (enum WalkDirection eDir)
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
void CStormbusterScene::OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	if (m_SoundPlaying == IntroPlaying || m_SoundPlaying == SuccessPlaying)
		OnLButtonDown (hWnd, FALSE/*fDoubleClick*/, 0/*x*/, 0/*y*/, NULL/*keyFlags*/);
}

//***************************************************************************
UINT CStormbusterScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
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
		LPSTORMBUSTERSCENE lpScene = (LPSTORMBUSTERSCENE)dwNotifyData;
		lpScene->OnSNotify (lpSprite, Notify);
	}
}

//************************************************************************
void CStormbusterScene::OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (!lpSprite)
		return;

	static int iObjectIdx;
	if (Notify == SN_MOVED)
	{
		// Check for collision
		if (!fPainting && lpSprite != m_lpPlayerSprite && CheckObjectCollision (lpSprite) )
		{
			if (m_nLastColor != -1 && m_lpPlayerSprite && m_lpPainterSprite)
			{
				iObjectIdx = GetGoodMatchIdx (m_nLastColor);
				if (iObjectIdx != -1)
				{ // it's a hit!
					// Don't let them try to move the player while running the painting sequence
					fPainting = TRUE;
					m_lpPlayerSprite->Move (m_ptPainterPos.x, m_nScreenBottom);
				}
				// Play the bad catch wave
				else
				{
					// Play the got one sound if one is defined
					if (GetSound() && m_szBadCatchWave[0] != '\0')
					{
						FNAME szFileName;
						GetSound()->StopChannel (NORMAL_CHANNEL);
						GetSound()->StartFile (GetPathName (szFileName, m_szBadCatchWave),
											NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, FALSE);
					}
					// This will take away a stripe
					if ( m_bPenalty == TRUE )       
					{
						// Stop and hide the sprite so no more SN_MOVED 
						// messages come in on it (Mike)               
						lpSprite->DeleteAllActions();
						lpSprite->Show (FALSE);
						TakeStripeAway();
					}
				}
			}
		}
	}
	else
	if (Notify == SN_MOVEDONE)
	{
		if ( !fPainting )
		{
			if (m_lpPlayerSprite && lpSprite == m_lpPlayerSprite)
				m_lpPlayerSprite->SetCellsPerSec (0);
		}
		else
		{
			if (m_lpPlayerSprite && lpSprite == m_lpPlayerSprite)
			{
				m_lpPlayerSprite->Show (FALSE);
				m_lpPainterSprite->Jump (m_ptPainterPos.x-64, m_nScreenBottom-94);
				m_lpPainterSprite->Show (TRUE);
				m_lpPainterSprite->Move (m_ptPainterPos.y-64, m_nScreenBottom-94);
			}
			else
			if (m_lpPainterSprite && lpSprite == m_lpPainterSprite)
			{
				if (m_lpPlayerSprite)
				{
					m_lpPainterSprite->Show (FALSE);
					m_lpPlayerSprite->Jump (m_ptPainterPos.y, m_nScreenBottom);
					m_lpPlayerSprite->Show (TRUE);
					// Update the rainbow
					ProcessRainbow (iObjectIdx);
					fPainting = FALSE;
				}
				if (m_nGoodPicks >= m_nMatchesNeeded)
					PlayLevelSuccessWave();
			}
		}
	}
}

//************************************************************************
BOOL CStormbusterScene::CheckObjectCollision (LPSPRITE lpSprite)
//************************************************************************
{
	if (!m_lpPlayerSprite || !m_pAnimator)
		return(FALSE);

	// See if they've picked enough already
	if (m_nGoodPicks >= m_nMatchesNeeded)
		return(FALSE);

	static BOOL fBusy;
	if (fBusy)
		return(FALSE);
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
			m_nLastColor = -1;
			fBusy = FALSE;
			return(FALSE);
		}

		// Save the color picked
		m_nLastColor = m_ObjectList[idx].m_nColor;

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

		fBusy = FALSE;
		return(TRUE);
	}

	fBusy = FALSE;
	return(FALSE);
}

//************************************************************************
void CStormbusterScene::ProcessRainbow (int iObjectIdx)
//************************************************************************
{
	int iIdx = -1;

	// If they haven't caught one, nothing to do
	if (m_nLastColor == -1 || iObjectIdx == -1)
		return;

	for (int i = 0 ; i < m_nMatchesNeeded ; i++)
	{
		if (m_arnPickColors[i] == m_nLastColor)
		{
			// In case of duplicate colored drops
			if (m_ObjectList[iObjectIdx].m_nState == StateGoodPick)
				return;

			// Set this drop as caught
			m_ObjectList[iObjectIdx].m_nState = StateGoodPick;
			m_nGoodPicks++;

// Craig - I added the braces and the preview //
			if (m_lpStripes[i])  
			{
				m_lpStripes[i]->Show (TRUE);
				m_lpPreview[i]->Show (FALSE);
			}				
		}
	}
	m_nLastColor = -1;
}

//************************************************************************
int CStormbusterScene::GetGoodMatchIdx (int nLastColor)
//************************************************************************
{
	if (m_fPickInOrder)
	{
		if (m_arnPickColors[m_nGoodPicks] == nLastColor)
			return (GetObjectIndexByColor (m_nLastColor));
	}
	else
	{
		for (int i = 0 ; i < m_nMatchesNeeded ; i++)
		{
			if (m_arnPickColors[i] == nLastColor)
				return (GetObjectIndexByColor (m_nLastColor));
		}
	}
	return -1;
}

//************************************************************************
int CStormbusterScene::GetObjectIndex (LPSPRITE lpSprite)
//************************************************************************
{
	if (!lpSprite)
		return -1;

	for (int i = 0 ; i < MAX_DROPS ; i++)
	{
		if (m_ObjectList[i].m_lpSprite == lpSprite)
			return i;
	}

	return -1;
}

//************************************************************************
int CStormbusterScene::GetObjectIndexByColor (int iColor)
//************************************************************************
{
	for (int i = 0 ; i < MAX_DROPS ; i++)
	{
		if (m_ObjectList[i].m_lpSprite && m_ObjectList[i].m_nColor == iColor)
			return i;
	}
	return -1;
}

//************************************************************************
void CStormbusterScene::StartGame()
//************************************************************************
{
	m_SoundPlaying = NotPlaying;
	ResetAllObjects();
	PickTargetSprites();
	SelectObject();
	m_nGoodPicks = 0;

	// Start up the timers to release the objects
	if (m_nDropTimer && ! m_fTimerStarted)
	{
		m_fTimerStarted = TRUE;
		SetTimer (m_hWnd, DROP_TIMER, m_nDropTimer, NULL);
	}
}

//************************************************************************
void CStormbusterScene::PickTargetSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	PDIB  pDib;
	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	CRandomSequence RandomSequence;
	wRandomSeed = GetRandomNumber (MAX_DROPS - 1);
	RandomSequence.Init((DWORD)MAX_DROPS - 1, (DWORD)wRandomSeed);

	// Randomly pick m_nMatchesNeeded balloons to be the good picks
	int iLevel = GetLevel();
	for (int iTarget = 0 ; iTarget < m_nMatchesNeeded ; iTarget++)
	{
		for (int j = 0 ; j < MAX_DROPS ; j++)
		{
			int iSel = (int)RandomSequence.GetNextNumber();

			// If already a target, try again...
			if (m_ObjectList[iSel].m_fOneToCatch || !m_ObjectList[iSel].m_lpSprite)
				continue;

			// It's finally a target
			m_ObjectList[iSel].m_fOneToCatch = TRUE;
			m_ObjectList[iSel].m_nIndex = iTarget;
			// Save the target colors
			m_arnPickColors[iTarget] = m_ObjectList[iSel].m_nColor;

			// Create this rainbow stripe
			if (m_aszStripeBmps[iTarget][0] != '\0')
			{
				if (m_lpStripes[iTarget])
					m_pAnimator->DeleteSprite (m_lpStripes[iTarget]);
				m_lpStripes[iTarget] = m_pAnimator->CreateSprite (&ptOrigin);
				if (m_lpStripes[iTarget])
				{
					GetPathName (szFileName, m_aszStripeBmps[iTarget]);
					pDib = CDib::LoadDibFromFile (szFileName, NULL, FALSE);
					if (pDib)
					{
						// Set the color for this stripe
						SetDibColor (pDib, m_ObjectList[iSel].m_nColor);
						m_arnStripeColors[iTarget] = m_ObjectList[iSel].m_nColor;
						m_lpStripes[iTarget]->AddCell (pDib, 0, 0);
					//	m_lpStripes[iTarget]->Jump (m_ptRainbowPos.x, m_ptRainbowPos.y);
/*Craig*/				m_lpStripes[iTarget]->Jump (m_ptStripePos[iTarget].x,m_ptStripePos[iTarget].y);
						m_lpStripes[iTarget]->Show (FALSE);
					}

					// Create the preview rainbow stripe
					if (m_lpPreview[iTarget])
						m_pAnimator->DeleteSprite (m_lpPreview[iTarget]);
					m_lpPreview[iTarget] = m_pAnimator->CreateSprite (&ptOrigin);
					if (m_lpPreview[iTarget] && m_szPreviewBmp[0] != '\0')
					{
						GetPathName (szFileName, m_szPreviewBmp);
						pDib = CDib::LoadDibFromFile (szFileName, NULL, FALSE);
						if (pDib)
						{
							// Set the color for this preview stripe
							SetDibColor (pDib, m_ObjectList[iSel].m_nColor);
							m_lpPreview[iTarget]->AddCell (pDib, 0, 0);
						//	m_lpPreview[iTarget]->Jump (m_ptPreviewPos.x, m_ptPreviewPos.y + (m_nPreviewHeight * iTarget));
/*Craig*/					m_lpPreview[iTarget]->Jump (m_ptPreviewPos.x, m_ptPreviewPos.y);
							m_lpPreview[iTarget]->Show (TRUE);
						}
					}

				}
			}
			break;
		}
	}
}

//************************************************************************
void CStormbusterScene::ResetObjects()
//************************************************************************
{
	// Set all objects that are used and hidden to be unused
	for (int i = 0 ; i < MAX_DROPS ; i++)
	{
		if (m_ObjectList[i].m_lpSprite)
		{
			if (! m_ObjectList[i].m_lpSprite->IsVisible() &&
				m_ObjectList[i].m_nState == StateUsed)
		 	{
				m_ObjectList[i].m_nState = StateUnused;
			}
		}
	}
}

//************************************************************************
void CStormbusterScene::ResetAllObjects()
//************************************************************************
{
	for (int i = 0 ; i < MAX_DROPS ; i++)
	{
		if (m_ObjectList[i].m_lpSprite)
		{
			m_ObjectList[i].m_lpSprite->Show (FALSE);
			m_ObjectList[i].m_lpSprite->DeleteAllActions();
		}
		m_ObjectList[i].m_nState = StateUnused;
		m_ObjectList[i].m_fOneToCatch = FALSE;
	}

	for (i = 0 ; i < m_nMatchesNeeded ; i++)
	{
		if (m_lpStripes[i])
			m_lpStripes[i]->Show (FALSE);
		if (m_lpPreview[i])
			m_lpPreview[i]->Show (FALSE);
	}
}

//************************************************************************
void CStormbusterScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	if (id != DROP_TIMER)
	{
		CGBScene::OnTimer (hWnd, id);
		return;
	}

	if (m_SoundPlaying == SuccessPlaying || m_SoundPlaying == IntroPlaying)
		return;

	// Set hidden drop states so they can be re-used
	ResetObjects();
	// Select the next drop
	SelectObject();
}

//************************************************************************
int CStormbusterScene::SelectObject()
//************************************************************************
{
	CRandomSequence RandomSequence;
	wRandomSeed = GetRandomNumber (MAX_DROPS);
	RandomSequence.Init ((DWORD)MAX_DROPS, (DWORD)wRandomSeed);

	if (fLaunching)
		return 0;
	fLaunching = TRUE;

	BOOL bFound = FALSE;
	int nSelectedObject = -1;

	// Make sure that the targets come up frequently (1 out of 5)
	static int iTarget;
	if (++iTarget >= 4) iTarget = 0;
	BOOL fDoTarget = (iTarget == 0);

	if (m_fPickInOrder && fDoTarget)
	{
		for (int i = 0 ; i < MAX_DROPS ; i++)
		{		
			// Find the next target
			if (m_ObjectList[i].m_lpSprite &&
				m_ObjectList[i].m_nState == StateUnused && 
		  		m_ObjectList[i].m_fOneToCatch == fDoTarget &&
		  		m_ObjectList[i].m_nColor == m_arnStripeColors[m_nGoodPicks])
			{
				nSelectedObject = i;
				bFound = TRUE;
            	break;
			}
		}
	}
	else
	{
		// Randomly pick a piece
		for (int i = 0 ; i < MAX_DROPS ; i++)
		{		
			nSelectedObject = (int)RandomSequence.GetNextNumber();
			// Find an item that hasn't been used
				if (m_ObjectList[nSelectedObject].m_lpSprite &&
					m_ObjectList[nSelectedObject].m_nState == StateUnused && 
			  		m_ObjectList[nSelectedObject].m_fOneToCatch == fDoTarget)
			{
					bFound = TRUE;
	            break;
			}
		}
	}
	// Return -1 if we ran out of items
	if (! bFound)
	{
		fLaunching = FALSE;
		return -1;
	}

	m_ObjectList[nSelectedObject].m_nState = StateUsed;

	// Start this object falling from a randomly selected drop point
	if (m_ObjectList[nSelectedObject].m_lpSprite)
	{
		int iCX, iCY, iX;
		m_ObjectList[nSelectedObject].m_lpSprite->GetMaxSize (&iCX, &iCY);
		// Get a random drop index based on number of drop points
		iX = GetRandomNumber (sLevel[m_iLevel].m_wChuteCount);
		// Add a random offset
		int iOffset = GetRandomNumber (100) - 50;		

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
		m_ObjectList[nSelectedObject].m_lpSprite->AddCmdHide();
	}
	fLaunching = FALSE;
	return nSelectedObject;
}

//************************************************************************
void CStormbusterScene::PlayIntro(HWND hWnd/*NULL for no WOM_DONE*/)
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
void CStormbusterScene::PlayLevelSuccessWave()
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
			if (m_nSceneNo == IDD_STRMBSTRI)
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
void CStormbusterScene::PlayClickWave (BOOL fSync/*=TRUE*/)
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
void CStormbusterScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS - 1;

	App.GotoScene (m_hWnd, IDD_STRMBSTR1 + iLevel, m_nNextSceneNo);
}

//************************************************************************
void CStormbusterScene::CreateSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	PDIB pDib;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	// Create the object sprites
	for (int i = 0 ; i < MAX_DROPS ; i++)
	{
		if (m_ObjectList[i].m_szImage[0] != '\0')
		{
			m_ObjectList[i].m_lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_ObjectList[i].m_lpSprite)
			{
				GetPathName (szFileName, m_ObjectList[i].m_szImage);
				pDib = CDib::LoadDibFromFile (szFileName, NULL, FALSE);
				if (!pDib)
					continue;

				// Set the color for this drop
				SetDibColor (pDib, m_ObjectList[i].m_nColor);

				// Set up the sprite parameters
				m_ObjectList[i].m_lpSprite->AddCells (pDib, m_ObjectList[i].m_nTotalCells, NULL);
				m_ObjectList[i].m_lpSprite->SetCellsPerSec (10);
				m_ObjectList[i].m_lpSprite->SetSpeed (m_nObjectSpeed);
				m_ObjectList[i].m_lpSprite->SetCycleBack (TRUE);
				m_ObjectList[i].m_lpSprite->Jump (0, 0);
				m_ObjectList[i].m_lpSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);    
				m_ObjectList[i].m_lpSprite->Show (FALSE);
			}
		}
	}

	// Create the painter sprite
	m_lpPainterSprite = m_pAnimator->CreateSprite (&ptOrigin);
	if (m_lpPainterSprite)
	{
		if (! m_nPainterCells)
			m_nPainterCells = 1;
		GetPathName (szFileName, m_szPainterBmp); 
		m_lpPainterSprite->AddCells (szFileName, m_nPainterCells, NULL);
		m_lpPainterSprite->SetCellsPerSec (10);
		m_lpPainterSprite->SetCycleBack (TRUE);
		m_lpPainterSprite->SetSpeed (100/*m_nPlayerSpeed*/);
		m_lpPainterSprite->Jump (m_ptPainterPos.x-64, m_nScreenBottom-94);
		m_lpPainterSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);    
		m_lpPainterSprite->Show (FALSE);
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
		m_lpPlayerSprite->SetCellsPerSec (10);
		m_lpPlayerSprite->SetSpeed (m_nPlayerSpeed);
		m_lpPlayerSprite->Jump (m_nScreenLeftEdge, m_nScreenBottom);
		m_lpPlayerSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);    
		m_lpPlayerSprite->Show (TRUE);
	}

	// Initially walking right
	m_WalkDir = WALKRIGHT;

	// Create the rainbow sprite
	m_lpRainbow = m_pAnimator->CreateSprite (&ptOrigin);
	if (m_lpRainbow)
	{
		GetPathName (szFileName, m_szRainbowBmp); 
		m_lpRainbow->AddCell (szFileName, NULL);
		m_lpRainbow->Jump (m_ptRainbowPos.x, m_ptRainbowPos.y);
		m_lpRainbow->Show (TRUE);
	}
}

//************************************************************************
void CStormbusterScene::SetDibColor (PDIB pDib, int nColor)
//************************************************************************
{
	if (!pDib || pDib->GetBitCount() != 8)
		return;

	if (nColor < 0 || nColor > 255)
		return;

	// If not defined in the RC file, do nothing
	if (m_bSourceRed == -1 || m_bSourceGreen == -1 || m_bSourceBlue == -1)
		return;

	LPRGBQUAD pRgb = pDib->GetColors();
	for (int n = 0 ; n < 256 ; n++)
	{
		// If this index is the source color we want to change
		if (pRgb[n].rgbRed == m_bSourceRed &&
		    pRgb[n].rgbGreen == m_bSourceGreen &&
		    pRgb[n].rgbBlue == m_bSourceBlue)
		{
			// Spin through the data changing this index to the new index passed in
			HPTR lpData = pDib->GetPtr();
			DWORD dwSize = pDib->GetSizeImage();
			while (dwSize)
			{
				if (*lpData == n)
					*lpData = nColor;
				++lpData;
				--dwSize;
			}
		}
	}
}

//************************************************************************
BOOL CStormbusterParser::GetSuccessWaves (LPSTORMBUSTERSCENE lpScene, LPSTR lpEntry,
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
BOOL CStormbusterParser::HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues,
								int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPSTORMBUSTERSCENE lpScene = (LPSTORMBUSTERSCENE)dwUserData;
	int	iTmp;

	if (!lstrcmpi (lpKey, "gamearea"))
		GetRect (&lpScene->m_rGameArea, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "item"))
	{
		if (nIndex < MAX_DROPS)
			GetFilename (lpScene->m_ObjectList[nIndex].m_szImage, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "tcells"))
	{
		if (nIndex < MAX_DROPS)
			GetInt (&lpScene->m_ObjectList[nIndex].m_nTotalCells, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "color"))
	{
		if (nIndex < MAX_DROPS)
			GetInt (&lpScene->m_ObjectList[nIndex].m_nColor, lpEntry, lpValues, nValues);
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

	if (!lstrcmpi (lpKey, "badcatchwave"))
		GetFilename (lpScene->m_szBadCatchWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "Player"))
		GetFilename (lpScene->m_szPlayerBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "LeftBasket"))
		GetRect (&lpScene->m_rLeftBasket, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "RightBasket"))
		GetRect (&lpScene->m_rRightBasket, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PlayerCells"))
		GetInt (&lpScene->m_nPlayerCells, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PainterBmp"))
		GetFilename (lpScene->m_szPainterBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PainterPos"))
		GetPoint (&lpScene->m_ptPainterPos, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PainterCells"))
		GetInt (&lpScene->m_nPainterCells, lpEntry, lpValues, nValues);

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

	if (!lstrcmpi (lpKey, "jumpwave"))
		GetFilename (lpScene->m_szJumpWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "keyboardmove"))
		GetInt (&lpScene->m_nKeyboardDistance, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "RainbowPos"))
		GetPoint (&lpScene->m_ptRainbowPos, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "RainbowBmp"))
		GetFilename (lpScene->m_szRainbowBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "srcred"))
	{
		GetInt (&iTmp, lpEntry, lpValues, nValues);
		lpScene->m_bSourceRed = (BYTE)iTmp;
	}

	if (!lstrcmpi (lpKey, "srcgreen"))
	{
		GetInt (&iTmp, lpEntry, lpValues, nValues);
		lpScene->m_bSourceGreen = (BYTE)iTmp;
	}

	if (!lstrcmpi (lpKey, "srcBlue"))
	{
		GetInt (&iTmp, lpEntry, lpValues, nValues);
		lpScene->m_bSourceBlue = (BYTE)iTmp;
	}

	if (!lstrcmpi (lpKey, "StripeBmp1"))
		GetFilename (lpScene->m_aszStripeBmps[0], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp2"))
		GetFilename (lpScene->m_aszStripeBmps[1], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp3"))
		GetFilename (lpScene->m_aszStripeBmps[2], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp4"))
		GetFilename (lpScene->m_aszStripeBmps[3], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp5"))
		GetFilename (lpScene->m_aszStripeBmps[4], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp6"))
		GetFilename (lpScene->m_aszStripeBmps[5], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp7"))
		GetFilename (lpScene->m_aszStripeBmps[6], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripeBmp8"))
		GetFilename (lpScene->m_aszStripeBmps[7], lpEntry, lpValues, nValues);
// Craig //

	if (!lstrcmpi (lpKey, "StripePos1"))
		GetPoint (&lpScene->m_ptStripePos[0], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos2"))
		GetPoint (&lpScene->m_ptStripePos[1], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos3"))
		GetPoint (&lpScene->m_ptStripePos[2], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos4"))
		GetPoint (&lpScene->m_ptStripePos[3], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos5"))
		GetPoint (&lpScene->m_ptStripePos[4], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos6"))
		GetPoint (&lpScene->m_ptStripePos[5], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos7"))
		GetPoint (&lpScene->m_ptStripePos[6], lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "StripePos8"))
		GetPoint (&lpScene->m_ptStripePos[7], lpEntry, lpValues, nValues);
//  Craig //
	if (!lstrcmpi (lpKey, "PreviewBmp"))
		GetFilename (lpScene->m_szPreviewBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PreviewLoc"))
		GetPoint (&lpScene->m_ptPreviewPos, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PreviewHeight"))
		GetInt (&lpScene->m_nPreviewHeight, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "PickInOrder"))
		GetInt (&lpScene->m_fPickInOrder, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "Penalty"))
		GetInt (&lpScene->m_bPenalty, lpEntry, lpValues, nValues);

	return (TRUE);
}

