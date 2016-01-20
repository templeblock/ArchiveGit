#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "goat.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "balloon.h"
#include "balldef.h"
#include "randseq.h"


LOCAL void CALLBACK OnSpriteNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);
static WORD wRandomSeed;

class FAR CBalloonParser : public CParser
{
public:
	CBalloonParser(LPSTR lpTableData) : CParser(lpTableData) {}

protected:
	BOOL HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetSuccessWaves (LPBALLOONSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	void SaveItemLoc (LPBALLOONSCENE lpScene ,LPSTR lpEntry, LPSTR lpValues, int nValues, int iType);
};  

//************************************************************************
CBalloonScene::CBalloonScene (int nNextSceneNo) : CGBScene (nNextSceneNo)
//************************************************************************
{
	m_pSound = NULL;

	m_lpGoatSprite = NULL;
	m_SoundPlaying = NotPlaying;
	m_nJumpHeight = 0;
	m_nGoatSpeed = 500;
	m_nBalloonSpeed = 150;
	m_nGoatJumpState = 0;
	m_fGoatJumping = FALSE;
	m_nMatchesNeeded = 6;
	m_nGoodPicks = 0;
	m_nDropTimer = 200;
	m_nKeyboardDistance = 20;
	m_rGoatHand.left = m_rGoatHand.top = m_rGoatHand.right = m_rGoatHand.bottom = 0;
	m_nSelectedShape = -1;
	m_nSelectedColor = -1;
	m_bSourceRed = -1;
	m_bSourceGreen = -1;
	m_bSourceBlue = -1;

	// Init. wave filename storage
	m_szIntroWave[0] = '\0';
	m_szSoundTrack[0] = '\0';
	m_szBadPickWave[0] = '\0';
	m_szGoodPickWave[0] = '\0';
	m_szJumpWave[0] = '\0';
	for (int i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessLevel[i][0] = '\0';

	// Init. the balloon sprite structure
	for (i = 0 ; i < MAX_BALLOONS ; i++)
	{
		m_BalloonList[i].m_szImage[0] = '\0';
		m_BalloonList[i].m_lpSprite = NULL;
		m_BalloonList[i].m_nTotalCells = 0;
		m_BalloonList[i].m_nFloatCells = 0;
		m_BalloonList[i].m_lpMenuSprite = NULL;
		m_BalloonList[i].m_fTarget = FALSE;
		m_BalloonList[i].m_nShape = -1;
		m_BalloonList[i].m_nColor = -1;
	}

	// Init. menu item location storage
	m_nMenuLocCount = 0;
	m_nCaughtLocCount = 0;
	for (i = 0 ; i < MAX_BALLOONS ; i++)
	{
		m_MenuLocs[i].m_ptLoc.x = m_MenuLocs[i].m_ptLoc.y = 0;
		m_MenuLocs[i].m_fInUse = FALSE;
		m_CaughtLocs[i].m_ptLoc.x = m_CaughtLocs[i].m_ptLoc.y = 0;
		m_CaughtLocs[i].m_fInUse = FALSE;
	}
}

//************************************************************************
CBalloonScene::~CBalloonScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;

	if (m_nDropTimer)
		KillTimer (m_hWnd, DROP_TIMER);
}

//************************************************************************
int CBalloonScene::GetLevel()
//************************************************************************
{
	if (m_nSceneNo == IDD_BALLOONI)
		return (0);
	else
		return (m_nSceneNo - IDD_BALLOON1);
}

//************************************************************************
BOOL CBalloonScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);
	
	m_pSound = new CSound (FALSE);
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
			CBalloonParser parser (lpTableData);
			parser.ParseTable ((DWORD)this);
		}
		if (hData)
		{
			UnlockResource (hData);
			FreeResource (hData);
		}

		m_pAnimator->SetClipRect (&m_rGameArea);
	}

	// Seed the random number generator
	wRandomSeed = GetRandomNumber (MAX_BALLOONS);

	return (TRUE);
}

//************************************************************************
void CBalloonScene::ToonInitDone()
//************************************************************************
{
	GetToon()->SetHintState(TRUE);
	CreateSprites();
	PickTargetBalloons();
	PlayIntro(m_hWnd);
	StartGame();
}

//************************************************************************
void CBalloonScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
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
			ChangeLevel (iLevel);
			break;
		}

		case IDC_HELP:
		{
			PlayIntro(m_hWnd);
			break;
		}

		case IDC_TOON:
		{
			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
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
BOOL CBalloonScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
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
BOOL CBalloonScene::OnMessage (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
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
void CBalloonScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	FNAME szFileName;
	if (m_SoundPlaying == IntroPlaying)
	{
		m_SoundPlaying = NotPlaying;
	}	
	else
	if (m_SoundPlaying == SuccessPlaying)
	{
		m_SoundPlaying = NotPlaying;
	 	GetSound()->StopChannel(SUCCESS_CHANNEL);

		// If Play mode, go to the next scene
		if (m_nSceneNo == IDD_BALLOONI)
			FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		else
		{
			PickTargetBalloons();
			PlayIntro(m_hWnd);
		}
	}

	if (m_fGoatJumping)
		return;

	// Play the goat jump sound if one is defined
	if (GetSound() && m_szJumpWave[0] != '\0')
	{
		GetSound()->StopChannel(NORMAL_CHANNEL);
		GetSound()->StartFile (GetPathName (szFileName, m_szJumpWave),
							NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, FALSE);
	}

	// Make the goat jump
	if (m_lpGoatSprite)
	{
		RECT rLoc;
		m_lpGoatSprite->Location (&rLoc);
		m_lpGoatSprite->DeleteAllActions();
		m_lpGoatSprite->AddCmdMove (rLoc.left, rLoc.top - m_nJumpHeight);
		m_lpGoatSprite->AddCmdMove (rLoc.left, m_nScreenBottom);
		m_nGoatJumpState = 0;
		m_fGoatJumping = TRUE;
	}
}

//************************************************************************
void CBalloonScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CBalloonScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fGoatJumping || !m_lpGoatSprite)
		return;

	int iWidth, iTmp;
	m_lpGoatSprite->GetMaxSize (&iWidth, &iTmp);
	// Let the goat go over to the right more since his left hand pops the balloons
	iWidth /= 3;

	if (x < m_nScreenLeftEdge)
	{
		m_lpGoatSprite->Jump(m_nScreenLeftEdge, m_nScreenBottom);
	}
	else if ((x + iWidth) > m_nScreenRightEdge)
	{
		m_lpGoatSprite->Jump(m_nScreenRightEdge - iWidth, m_nScreenBottom);
	}
	else
	{
		iTmp = x;
		if ((x + iWidth) > m_nScreenRightEdge)
			iTmp = m_nScreenRightEdge - iWidth;
		m_lpGoatSprite->Jump(iTmp, m_nScreenBottom);
	}
}

//************************************************************************
void CBalloonScene::OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	static BOOL fInuse = FALSE;
	if (m_fGoatJumping || !m_lpGoatSprite)
		return;

	// In case of fast key repeat
	if (fInuse)
		return;
	fInuse = TRUE;

	if (m_SoundPlaying == IntroPlaying)
	{
		if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT)
		{
			m_SoundPlaying = NotPlaying;
		}
	}

	RECT rLoc;
	int iWidth, iTmp, iWidth3;
	// Get current goat location and size info.
	m_lpGoatSprite->Location (&rLoc);
	m_lpGoatSprite->GetMaxSize (&iWidth, &iTmp);
	iWidth3 = iWidth / 3;

	switch (vk)
	{
		// Make the goat jump
		case VK_UP:
			OnLButtonDown (m_hWnd, FALSE, 0, 0, 0);
			break;

		// Move the goat left
		case VK_LEFT:
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

		// Move the goat right
		case VK_RIGHT:
			if ((rLoc.left + iWidth3 + m_nKeyboardDistance) > m_nScreenRightEdge)
			{
				rLoc.left = m_nScreenRightEdge - iWidth3;
				rLoc.right = rLoc.left + iWidth;
			}
			else
			{
				rLoc.left += m_nKeyboardDistance;
				rLoc.right += m_nKeyboardDistance;
			}
			break;

		case VK_DOWN:
			break;

		default:
			CGBScene::OnKey (hWnd, vk, fDown, cRepeat, flags);
			break;
	}
	m_lpGoatSprite->Jump(rLoc.left, m_nScreenBottom);
	fInuse = FALSE;
}

//************************************************************************
void CBalloonScene::OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	m_SoundPlaying = NotPlaying;
	if (m_SoundPlaying == SuccessPlaying && m_nSceneNo == IDD_BALLOONI)
		FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
}

//***************************************************************************
UINT CBalloonScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
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
		LPBALLOONSCENE lpScene = (LPBALLOONSCENE)dwNotifyData;
		lpScene->OnSNotify (lpSprite, Notify);
	}
}

//************************************************************************
void CBalloonScene::OnSNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (!lpSprite)
		return;

	if (Notify == SN_MOVED)
	{
	}
	else
	if (Notify == SN_MOVEDONE)
	{
		if (lpSprite == m_lpGoatSprite)
		{
			m_nGoatJumpState++;
			// If the goat is at the top of his jump, check for collision
			if (m_nGoatJumpState == 1)
			{
				CheckBalloonCollision();
			}
			else if (m_nGoatJumpState > 1)
			{
				m_nGoatJumpState = 0;
				m_fGoatJumping = FALSE;
			}
		}
	}
}

//************************************************************************
void CBalloonScene::CheckBalloonCollision()
//************************************************************************
{
	static BOOL fBusy = FALSE;
	
	if (fBusy || !m_lpGoatSprite || !m_pAnimator)
		return;

	fBusy = TRUE;

	RECT rGoatHand;
	m_lpGoatSprite->Location (&rGoatHand);

	// Adjust the goat location to be just his left hand
	// Note: m_rGoatHand is not a rectangle but an X, Y, CX, CY of the goat's hand
	rGoatHand.left += m_rGoatHand.left;
	rGoatHand.top += m_rGoatHand.top;
	rGoatHand.right = rGoatHand.left + m_rGoatHand.right;
	rGoatHand.bottom = rGoatHand.top + m_rGoatHand.bottom;

	int iLevel = GetLevel();
	for (int iB = 0 ; iB < MAX_BALLOONS ; iB++)
	{
		if (m_BalloonList[iB].m_lpSprite)
		{
			if (m_pAnimator->CheckCollision (m_lpGoatSprite, m_BalloonList[iB].m_lpSprite, &rGoatHand))
			{
				// Pop this balloon
				PopBalloon(m_BalloonList, m_BalloonList[iB].m_lpSprite, iB);

				// Check level restrictions
				// Level 1 matches on color
				if (iLevel == 0)
				{
					if (m_BalloonList[iB].m_nColor != m_nSelectedColor)
					{
						PlayBadPop();
						fBusy = FALSE;
						return;
					}
				}
				// Level 2 matches on shape
				else
				if (iLevel == 1)
				{
					if (m_BalloonList[iB].m_nShape != m_nSelectedShape)
					{
						PlayBadPop();
						fBusy = FALSE;
						return;
					}
				}
				// Level 3 items must be caught in exact order
				else
				if (iLevel == 2)
				{
					if (m_arnPickOrder[m_nGoodPicks] != iB)
					{
						PlayBadPop();
						PutOneBack();
						fBusy = FALSE;
						return;
					}
				}

				// See if this is a target balloon
				if (!m_BalloonList[iB].m_fTarget)
				{
					PlayBadPop();
					fBusy = FALSE;
					return;
				}

				// YOU GOT ONE!!
				m_BalloonList[iB].m_fTarget = FALSE;
				m_nGoodPicks++;
		
				PlayGoodPop();
		
				// Move the menu sprite over to the caught list
				if (m_BalloonList[iB].m_lpMenuSprite)
				{
					m_BalloonList[iB].m_lpMenuSprite->Move(
						m_CaughtLocs[m_nGoodPicks-1].m_ptLoc.x,
						m_CaughtLocs[m_nGoodPicks-1].m_ptLoc.y);
				}
			}
		}
	}

	fBusy = FALSE;
}

//************************************************************************
void CBalloonScene::StartGame()
//************************************************************************
{
	// Start up the timer to release the balloons
	if (m_nDropTimer)
		SetTimer (m_hWnd, DROP_TIMER, m_nDropTimer, NULL);
}

//************************************************************************
void CBalloonScene::ResetAllBalloons()
//************************************************************************
{
	for (int i = 0 ; i < MAX_BALLOONS ; i++)
	{
		m_BalloonList[i].m_fTarget = FALSE;

		if (m_BalloonList[i].m_lpSprite)
		{
			m_BalloonList[i].m_lpSprite->Show(FALSE);
			m_BalloonList[i].m_lpSprite->ActivateCells (0, m_BalloonList[i].m_nFloatCells - 1, TRUE);
			m_BalloonList[i].m_lpSprite->ActivateCells (m_BalloonList[i].m_nFloatCells, m_BalloonList[i].m_nTotalCells - 1, FALSE);
			m_BalloonList[i].m_lpSprite->DeleteAllActions();
		}

		if (m_BalloonList[i].m_lpMenuSprite)
		{
			m_BalloonList[i].m_lpMenuSprite->Show(FALSE);
			m_BalloonList[i].m_lpMenuSprite->DeleteAllActions();
		}
	}
}

//************************************************************************
void CBalloonScene::PickTargetBalloons()
//************************************************************************
{
	ResetAllBalloons();
	m_nGoodPicks = 0;

	CRandomSequence RandomSequence;
	wRandomSeed = GetRandomNumber(MAX_BALLOONS);
	RandomSequence.Init((DWORD)MAX_BALLOONS, (DWORD)wRandomSeed);

	// Randomly pick m_nMatchesNeeded balloons to be the good picks
	int iLevel = GetLevel();
	for (int iTarget = 0 ; iTarget < m_nMatchesNeeded ; iTarget++)
	{
		for (int j = 0 ; j < MAX_BALLOONS ; j++)
		{
			int iSel = (int)RandomSequence.GetNextNumber();

			// If already a target, try again...
			if (m_BalloonList[iSel].m_fTarget || !m_BalloonList[iSel].m_lpSprite)
				continue;

			// Is it a match for this level?
			BOOL bOK =
				((iTarget == 0) ||
				(iLevel == 0 && m_BalloonList[iSel].m_nColor == m_nSelectedColor) ||
				(iLevel == 1 && m_BalloonList[iSel].m_nShape == m_nSelectedShape) ||
				(iLevel == 2));
			if ( !bOK )
				continue;

			// It's finally a target
			m_BalloonList[iSel].m_fTarget = TRUE;
			m_BalloonList[iSel].m_nIndex = iTarget;
			m_arnPickOrder[iTarget] = iSel; // Save the order for level 3

			// If level 1 or 2, save the first shape or color to match on
			if (iTarget == 0)
			{
				if (iLevel == 0)
					m_nSelectedColor = m_BalloonList[iSel].m_nColor;
				else
				if (iLevel == 1)
					m_nSelectedShape = m_BalloonList[iSel].m_nShape;
			}

			// Show menu sprites
			if (m_BalloonList[iSel].m_lpMenuSprite)
			{
				m_BalloonList[iSel].m_lpMenuSprite->Jump(
					m_MenuLocs[iTarget].m_ptLoc.x,
					m_MenuLocs[iTarget].m_ptLoc.y);
				m_BalloonList[iSel].m_lpMenuSprite->Show (TRUE);
			}
			break;
		}
	}
}

//************************************************************************
void CBalloonScene::PutOneBack()
//************************************************************************
{
	// If they have any good picks - put the last one back
	// into circulation
	if (m_nGoodPicks)
	{
		int i = m_arnPickOrder[m_nGoodPicks-1];
		if (!m_BalloonList[i].m_fTarget && m_BalloonList[i].m_lpMenuSprite)
		{
			m_BalloonList[i].m_lpMenuSprite->Move(
				m_MenuLocs[m_BalloonList[i].m_nIndex].m_ptLoc.x,
				m_MenuLocs[m_BalloonList[i].m_nIndex].m_ptLoc.y);
			m_BalloonList[i].m_fTarget = TRUE;
		}
		m_nGoodPicks--;
	}
}

//************************************************************************
void CBalloonScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	if (id != DROP_TIMER)
	{
		CGBScene::OnTimer (hWnd, id);
		return;
	}

	if (m_SoundPlaying == SuccessPlaying || m_SoundPlaying == IntroPlaying)
		return;

	// Select the next balloon
	if ( SelectBalloon() < -1 )
	{ // The game is over...
	}
}

//************************************************************************
int CBalloonScene::SelectBalloon()
//************************************************************************
{
	// See if they've already picked enough to win this level
	if (m_nGoodPicks >= m_nMatchesNeeded)
	{ // We won't start any more, and when none are left in play, we'll return -2 (OVER!!)
		for (int i = 0 ; i < MAX_BALLOONS ; i++)
		{ // If we find any items in use (i.e., not hidden) return -1
			if ( m_BalloonList[i].m_lpSprite &&
				 m_BalloonList[i].m_lpSprite->IsVisible() )
					return -1;
		}
		
		PlayLevelSuccessWave();
		return -2;
	}

	// Make sure that the target balloons come up frequently (1 out of 4 balloons)
	static int iTarget;
	if ( ++iTarget >= 3 ) iTarget = 0;
	BOOL fDoTarget = (iTarget == 0);
	
	CRandomSequence RandomSequence;
	wRandomSeed = GetRandomNumber (MAX_BALLOONS);

	BOOL bFound = FALSE;
	int iSel;

	Start:
	RandomSequence.Init ((DWORD)MAX_BALLOONS, (DWORD)wRandomSeed);
	for (int i = 0 ; i < MAX_BALLOONS ; i++)
	{		
		iSel = (int)RandomSequence.GetNextNumber();
		// Find an item that isn't in use (i.e., its hidden)
		if ( m_BalloonList[iSel].m_lpSprite &&
			!m_BalloonList[iSel].m_lpSprite->IsVisible() &&
		     m_BalloonList[iSel].m_fTarget == fDoTarget )
		{
			bFound = TRUE;
            break;
		}
	}

	// Try the other balloons if we didn't find one
	if (!bFound)
	{
		fDoTarget = !fDoTarget;
		goto Start;
	}

	// Start this balloon falling from the top
	#define WHITE_SPACE 20
	int x, y;
	m_BalloonList[iSel].m_lpSprite->GetMaxSize (&x, &y);
	x -= (WHITE_SPACE*2);  // a fair reduction of the typical balloon width
	if ( x < 0 ) x = 0;
	int iUsableWidth = m_nScreenRightEdge - m_nScreenLeftEdge - x;
	x = m_nScreenLeftEdge - WHITE_SPACE + GetRandomNumber(iUsableWidth/2);
	static BOOL bRightSide;
	if ( bRightSide )
		x += iUsableWidth/2;
	bRightSide = !bRightSide;
	
	// Move the balloon to the bottom of the screen, then hide it for re-use
	m_BalloonList[iSel].m_lpSprite->DeleteAllActions();
	m_BalloonList[iSel].m_lpSprite->AddCmdSetRepeatCycle(FALSE);
	m_BalloonList[iSel].m_lpSprite->AddCmdActivateCells(0, m_BalloonList[iSel].m_nFloatCells - 1, TRUE);
	m_BalloonList[iSel].m_lpSprite->AddCmdActivateCells(m_BalloonList[iSel].m_nFloatCells, m_BalloonList[iSel].m_nTotalCells - 1, FALSE);
	m_BalloonList[iSel].m_lpSprite->AddCmdJump(x, -30);
	m_BalloonList[iSel].m_lpSprite->AddCmdShow();
	m_BalloonList[iSel].m_lpSprite->AddCmdMove(x, m_rGameArea.bottom);
	m_BalloonList[iSel].m_lpSprite->AddCmdHide();
	return iSel;
}

//************************************************************************
void CBalloonScene::PopBalloon (LPBalloonList lpList, LPSPRITE lpSprite, int iSel)
//************************************************************************
{
	if (!lpSprite || iSel == -1)
		return;

	// Enable the popping sequence cells
	lpSprite->DeleteAllActions();
	lpSprite->AddCmdActivateCells(
		m_BalloonList[iSel].m_nFloatCells,
		m_BalloonList[iSel].m_nTotalCells - 1, TRUE);
	// Disable all but the popping cells
	lpSprite->AddCmdActivateCells (0, m_BalloonList[iSel].m_nFloatCells - 1, FALSE);
	// Then disable all but the last cell to leave it on the last cell
	lpSprite->AddCmdActivateCells (0, m_BalloonList[iSel].m_nTotalCells - 1, FALSE);
	// Make sure there is enough time to see the Pop cells
	lpSprite->AddCmdPause(2); // pause 2 tenths
	lpSprite->AddCmdHide();
	lpSprite->AddCmdJump(0, 0);
}

//************************************************************************
void CBalloonScene::PlayIntro(HWND hWnd/*NULL for no WOM_DONE*/)
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
void CBalloonScene::PlayGoodPop()
//************************************************************************
{
	FNAME szFileName;

	// Play the good pick sound if one is defined
	if (GetSound() && m_szGoodPickWave[0] != '\0')
	{
		GetSound()->StopChannel (NORMAL_CHANNEL);
		GetSound()->StartFile (GetPathName (szFileName, m_szGoodPickWave),
							NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, FALSE, m_hWnd);
	}
}

//************************************************************************
void CBalloonScene::PlayBadPop()
//************************************************************************
{
	FNAME szFileName;

	// Play the bad pop wave if one is defined
	if (GetSound() && m_szBadPickWave[0] != '\0')
	{
		GetSound()->StopChannel (NORMAL_CHANNEL);
		GetSound()->StartFile (GetPathName (szFileName, m_szBadPickWave),
							NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, FALSE, m_hWnd);
	}
}

//************************************************************************
void CBalloonScene::PlayLevelSuccessWave()
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
			GetSound()->StopChannel(SUCCESS_CHANNEL);
			if (m_nSceneNo == IDD_BALLOONI)
			{
				GetSound()->StartFile(GetPathName (szFileName, lpWave), NO/*bLoop*/, SUCCESS_CHANNEL/*iChannel*/, TRUE/*bWait*/, m_hWnd);
				FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
			}
			else
				GetSound()->StartFile(GetPathName (szFileName, lpWave), NO/*bLoop*/, SUCCESS_CHANNEL/*iChannel*/, FALSE/*bWait*/, m_hWnd);
		}
	}
}

//************************************************************************
void CBalloonScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS - 1;

	App.GotoScene (m_hWnd, IDD_BALLOON1 + iLevel, m_nNextSceneNo);
}

//************************************************************************
void CBalloonScene::CreateSprites()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	PDIB pDib;

	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	// Create the goat sprite
	m_lpGoatSprite = m_pAnimator->CreateSprite (&ptOrigin);
	GetPathName (szFileName, m_szGoatBmp); 
	m_lpGoatSprite->AddCell (szFileName, NULL);
	m_lpGoatSprite->Jump (m_nScreenLeftEdge, m_nScreenBottom);
	m_lpGoatSprite->SetSpeed (m_nGoatSpeed);
	m_lpGoatSprite->SetNotifyProc (OnSpriteNotify, (DWORD)this);    
	m_lpGoatSprite->Show (FALSE);

	// Create the balloon sprites
	for (int i = 0 ; i < MAX_BALLOONS ; i++)
	{
		if (!m_BalloonList[i].m_szImage[0])
			continue;

		m_BalloonList[i].m_lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
		if (!m_BalloonList[i].m_lpSprite)
			continue;

		GetPathName (szFileName, m_BalloonList[i].m_szImage);
		pDib = CDib::LoadDibFromFile (szFileName, NULL, FALSE);
		if (!pDib)
			continue;

		// Set the color for this balloon
		SetDibColor (pDib, m_BalloonList[i].m_nColor);

		// Set up the sprite parameters
		m_BalloonList[i].m_lpSprite->AddCells(pDib, m_BalloonList[i].m_nTotalCells, NULL);
		m_BalloonList[i].m_lpSprite->SetCellsPerSec(10);
		m_BalloonList[i].m_lpSprite->Jump(0, 0);
		m_BalloonList[i].m_lpSprite->SetSpeed(m_nBalloonSpeed);
		m_BalloonList[i].m_lpSprite->SetCycleBack(TRUE);
		m_BalloonList[i].m_lpSprite->SetNotifyProc(OnSpriteNotify, (DWORD)this);    
		m_BalloonList[i].m_lpSprite->Show(FALSE);

		// Create the menu item sprite from the first cell of the target balloon
		m_BalloonList[i].m_lpMenuSprite = m_pAnimator->CreateSprite (&ptOrigin);
		if (!m_BalloonList[i].m_lpMenuSprite)
			continue;

		// Point to the sprite's head DIB, and copy it into the new "menu" sprite
		pDib = m_BalloonList[i].m_lpSprite->GetHeadCell()->pSpriteDib;
		if (pDib)
			pDib = new CDib(pDib, pDib->GetPtr(), YES/*fCopyBits*/);
		if (pDib)
		{
			// Set the color for this balloon
			SetDibColor (pDib, m_BalloonList[i].m_nColor);
			m_BalloonList[i].m_lpMenuSprite->AddCells (pDib, 1, NULL);
			m_BalloonList[i].m_lpMenuSprite->SetCellsPerSec(10);
			m_BalloonList[i].m_lpMenuSprite->Show (FALSE);
		}
	}

	// Now show the goat sprite
	m_lpGoatSprite->Show (TRUE);
}

//************************************************************************
void CBalloonScene::SaveItemLoc (int iX, int iY)
//************************************************************************
{
	m_MenuLocs[m_nMenuLocCount].m_ptLoc.x = iX;
	m_MenuLocs[m_nMenuLocCount].m_ptLoc.y = iY;
	m_nMenuLocCount++;
}

//************************************************************************
void CBalloonScene::SaveCaughtLoc (int iX, int iY)
//************************************************************************
{
	m_CaughtLocs[m_nCaughtLocCount].m_ptLoc.x = iX;
	m_CaughtLocs[m_nCaughtLocCount].m_ptLoc.y = iY;
	m_nCaughtLocCount++;
}

//************************************************************************
void CBalloonScene::SetDibColor (PDIB pDib, int nColor)
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
BOOL CBalloonParser::GetSuccessWaves (LPBALLOONSCENE lpScene, LPSTR lpEntry,
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
void CBalloonParser::SaveItemLoc (LPBALLOONSCENE lpScene ,LPSTR lpEntry,
									LPSTR lpValues, int nValues, int iType)
//************************************************************************
{
	LPSTR lpValue;
	int iX, iY;

	if (nValues != 2)
	{
		Print ("Invalid itemloc entry in RC file.");
		return;
	}

	iX = iY = 0;
	lpValue = GetNextValue (&lpValues);
	if (lpValue)
		iX = latoi (lpValue);

	lpValue = GetNextValue (&lpValues);
	if (lpValue)
		iY = latoi (lpValue);

	if (iType == 0)
		lpScene->SaveItemLoc (iX, iY);
	else if (iType == 1)
		lpScene->SaveCaughtLoc (iX, iY);
}

//************************************************************************
BOOL CBalloonParser::HandleKey (LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues,
								int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPBALLOONSCENE lpScene = (LPBALLOONSCENE)dwUserData;
	int	iTmp;

	if (!lstrcmpi (lpKey, "gamearea"))
		GetRect (&lpScene->m_rGameArea, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "goathand"))
		GetRect (&lpScene->m_rGoatHand, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "item"))
	{
		if (nIndex < MAX_BALLOONS)
			GetFilename (lpScene->m_BalloonList[nIndex].m_szImage, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "tcells"))
	{
		if (nIndex < MAX_BALLOONS)
			GetInt (&lpScene->m_BalloonList[nIndex].m_nTotalCells, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "fcells"))
	{
		if (nIndex < MAX_BALLOONS)
			GetInt (&lpScene->m_BalloonList[nIndex].m_nFloatCells, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "shape"))
	{
		if (nIndex < MAX_BALLOONS)
			GetInt (&lpScene->m_BalloonList[nIndex].m_nShape, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "color"))
	{
		if (nIndex < MAX_BALLOONS)
			GetInt (&lpScene->m_BalloonList[nIndex].m_nColor, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi (lpKey, "droptimer"))
	{
		// timer in tenths of a second -> 0 = no balloons
		GetInt (&iTmp, lpEntry, lpValues, nValues);
		if (iTmp < 0)
			lpScene->m_nDropTimer = 0;
		else
			lpScene->m_nDropTimer = 100 * iTmp;
	}

	if (!lstrcmpi (lpKey, "introwave"))
		GetFilename (lpScene->m_szIntroWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "goodpickwave"))
		GetFilename (lpScene->m_szGoodPickWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "badpickwave"))
		GetFilename (lpScene->m_szBadPickWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "goatbmp"))
		GetFilename (lpScene->m_szGoatBmp, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "screenleftedge"))
		GetInt (&lpScene->m_nScreenLeftEdge, lpEntry, lpValues, nValues);
		
	if (!lstrcmpi (lpKey, "screenrightedge"))
		GetInt (&lpScene->m_nScreenRightEdge, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "screenbottom"))
		GetInt (&lpScene->m_nScreenBottom, lpEntry, lpValues, nValues);

	if (! lstrcmpi (lpKey, "successlevel"))
		GetSuccessWaves (lpScene, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "jumpheight"))
		GetInt (&lpScene->m_nJumpHeight, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "goatspeed"))
		GetInt (&lpScene->m_nGoatSpeed, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "balloonspeed"))
		GetInt (&lpScene->m_nBalloonSpeed, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "matchesneeded"))
		GetInt (&lpScene->m_nMatchesNeeded, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "itemloc"))
		SaveItemLoc (lpScene, lpEntry, lpValues, nValues, 0);

	if (!lstrcmpi (lpKey, "caughtloc"))
		SaveItemLoc (lpScene, lpEntry, lpValues, nValues, 1);

	if (!lstrcmpi (lpKey, "jumpwave"))
		GetFilename (lpScene->m_szJumpWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi (lpKey, "keyboardmove"))
		GetInt (&lpScene->m_nKeyboardDistance, lpEntry, lpValues, nValues);

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

	return (TRUE);
}

