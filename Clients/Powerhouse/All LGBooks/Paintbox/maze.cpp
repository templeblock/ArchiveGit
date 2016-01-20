#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "pain.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "painid.h"
#include "maze.h"
#include "mazedef.h"

class FAR CMazeParser : public CParser
{
public:
	CMazeParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetDecisionPoints(LPMAZE lpMaze, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSuccessWaves(LPMAZESCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues, int iLevel);
	BOOL GetWarp(LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetStop(LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

//************************************************************************
CMazeScene::CMazeScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	int i;

	for (i = 0; i < NUM_DIRECTIONS; ++i)
		m_lpAnnSprite[i] = NULL;

	for (i = 0; i < NUM_DIRECTIONS; ++i)
		m_lpHouseSprite[i] = NULL;

	for (i = 0; i < NUM_DIRECTIONS; ++i)
	{
		m_szAnn[i][0] = '\0';
		m_szHouse[i][0] = '\0';
	}

	for (i = 0 ; i < MAX_SUCCESSWAVES ; i++)
		m_szSuccessLevel[i][0] = '\0';

	m_nAnnCells = 3;
	m_nHouseCells = 3;
	m_nAnnCycle = 8;
	m_nHouseCycle = 8;
	m_nHouseSpeed = 75;
	m_nMazes = 0;
	m_pMazes = NULL;
	m_hHotCursor = LoadCursor (GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));
	m_nRoadIndex = 0;			// Index to the road palette color
	m_nMoveIncrement = 1;
	m_nAnnDirection = RIGHT;
	m_nHouseDirection = RIGHT;
	m_nScanDistance = 50;
	m_dwLastMove = 0;
	m_bAnnStopped = TRUE;
	m_fIntro = FALSE;
	m_fStop = FALSE;
	m_iHouseLast = 0;
	m_fInWarpZone = FALSE;
	m_fHouseMoving = FALSE;
	m_pSound = NULL;
	m_SoundPlaying = NotPlaying;
	m_szIntroWave[0] = '\0';
	m_szSoundTrack[0] = '\0';
	m_pCurrent = NULL;
	m_fLevelComplete = FALSE;
	m_fMovement = FALSE;
}


//************************************************************************
CMazeScene::~CMazeScene()
//************************************************************************
{
	if (m_pMazes)
		delete [] m_pMazes;

	if (m_pSound)
		delete m_pSound;

	if (m_pAnimator)
		m_pAnimator->DeleteAll();

	if (m_hHotCursor)
		DestroyCursor (m_hHotCursor);
}

//************************************************************************
int CMazeScene::GetLevel()
//************************************************************************
{
	if (m_nSceneNo == IDD_MAZEI)
		return (0);
	else
		return (m_nSceneNo - IDD_MAZE1);
}

//************************************************************************
BOOL CMazeScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);

	if (!m_pAnimator)
		return( TRUE );

	HGLOBAL hData;
	LPSTR lpTableData;

	App.SetKeyMapEntry (m_hWnd, VK_UP);
	App.SetKeyMapEntry (m_hWnd, VK_DOWN);
	App.SetKeyMapEntry (m_hWnd, VK_RIGHT);
	App.SetKeyMapEntry (m_hWnd, VK_LEFT);

	if (GetToon())
		GetToon()->SetSendMouseMode (TRUE);

	// read in the connect point information
	lpTableData = GetResourceData (m_nSceneNo, "gametbl", &hData);
	if (lpTableData)
	{
		CMazeParser parser(lpTableData);
		if (m_nMazes = parser.GetNumEntries())
		{
			--m_nMazes;
			m_pMazes = new CMaze[m_nMazes];
			if (m_pMazes)
				parser.ParseTable ((DWORD)this);
		}
	}
	if (hData)
	{
		UnlockResource (hData);
		FreeResource (hData);
	}

	m_pAnimator->SetClipRect (&m_rGameArea);
	m_pAnimator->StopAll();
	m_pCurrent = GetNewMaze();
	if (GetToon() && m_pCurrent)
		GetToon()->SetBackground (m_pCurrent->m_szBackground);
	return (TRUE);
}

//************************************************************************
void CMazeScene::ToonInitDone()
//************************************************************************
{
	m_pSound = new CSound (TRUE);
	if (m_pSound)
		m_pSound->Open (NULL);
	LoadSprites();
	InitMaze();
	PlayIntro();
}

//************************************************************************
void CMazeScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_MAZEI)
				CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
			else
				GetApp()->GotoSceneID (hWnd, id, m_nNextSceneNo);
			break;
		}

		case IDC_HELP:
		{
			PlayIntro();
			break;
		}

		case IDC_LEVEL1:
		case IDC_LEVEL2:
		case IDC_LEVEL3:
		{
			int iLevel = id-IDC_LEVEL1;
		 	if (iLevel >= NUM_LEVELS)
		 		iLevel = NUM_LEVELS-1;
			if (m_pSound)
			{
				m_pSound->StopAndFree();
				m_pSound->Activate (FALSE);
			}
			FNAME szFileName;
			CSound sound;
			sound.StartFile(GetPathName(szFileName, CLICK_WAVE), FALSE, -1, TRUE);
			if (m_pSound)
				m_pSound->Activate (TRUE);
			ChangeLevel (iLevel);
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
BOOL CMazeScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, WM_SETCURSOR, OnSetCursor);
		HANDLE_DLGMSG(hDlg, MM_WOM_DONE, OnWomDone);

		default:
			return(FALSE);
	}
}

//************************************************************************
void CMazeScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	if ( !fDoubleClick )
		SetCapture(hWnd);

//	if (m_SoundPlaying == IntroPlaying || m_SoundPlaying == SuccessPlaying)
//	{
//		OnWomDone( hWnd, NULL/*hDevice*/, NULL/*lpWaveHdr*/);
//		return;
//	}

	if (m_SoundPlaying == IntroPlaying)
	{
		m_SoundPlaying = NotPlaying;
	 	m_pSound->StopChannel(INTRO_CHANNEL);
		StartGame();
	}
	else
	if (m_SoundPlaying == SuccessPlaying)
	{
		m_SoundPlaying = NotPlaying;
		// If Play mode, go to next scene when they catch the house
		if (m_nSceneNo == IDD_MAZEI)
		{
			FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		}
		else
		{
			LPMAZE pMaze = GetNewMaze();
			if (pMaze)
			{
				UnloadMazeSprites();
				m_pCurrent = pMaze;
				GetToon()->LoadBackground (m_pCurrent->m_szBackground);
				InitMaze();
				ResetLevel();
				StartGame();
			}
		}
	}

	m_fMovement = YES;
	m_ptMouse.x = x;
	m_ptMouse.y = y;
}

//************************************************************************
void CMazeScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	ReleaseCapture();
#ifdef _DEBUG
		// Draw a box around all decision points
		LPMAZE pMaze = m_pCurrent;
		if (!pMaze)
			return;
		HDC hDC = GetDC(hWnd);
		for (int i = 0 ; i < pMaze->m_nPoints ; ++i)
		{
			POINT pt = pMaze->m_ptDecision[i];
			RECT r;
			r.left   = pt.x - 8;
			r.right  = pt.x + 8;
			r.top    = pt.y - 8;
			r.bottom = pt.y + 8;
			MoveToEx( hDC, pt.x, pt.y, NULL );
			LineTo( hDC, pt.x+1, pt.y );
			MoveToEx( hDC, r.left, r.top, NULL );
			LineTo( hDC, r.left, r.bottom );
			LineTo( hDC, r.right, r.bottom );
			LineTo( hDC, r.right, r.top );	
			LineTo( hDC, r.left, r.top );
		}
		ReleaseDC( hWnd, hDC );
#endif
}

//************************************************************************
void CMazeScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if ( m_SoundPlaying != NotPlaying )
		return;

	POINT pt;
	pt.x = x;
	pt.y = y;
	if (PtInRect(&m_rGameArea, pt))
	{
		m_fMovement = YES;
		m_ptMouse.x = x;
		m_ptMouse.y = y;
	}
}

//************************************************************************
BOOL CMazeScene::GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY)
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
BOOL CMazeScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	// set cursor visibility based on game area
	// only turn off cursor when we have capture
	if (PtInRect(&m_rGameArea, pt))
	{
		SetCursor( LoadCursor(NULL, IDC_ARROW) );
		// must set real return value for us to take over the cursor
   		SetDlgMsgResult(hWnd, WM_SETCURSOR, TRUE);
		return(TRUE);
	}
	return(FALSE);
}

//************************************************************************
void CMazeScene::OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	SoundPlaying WasPlaying = m_SoundPlaying;
	m_SoundPlaying = NotPlaying;

	if (WasPlaying == IntroPlaying)
		StartGame();
	else if (WasPlaying == SuccessPlaying)
	{
		// If Play mode, go to next scene when they catch the house
		if (m_nSceneNo == IDD_MAZEI)
		{
			FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		}
		else
		{
			LPMAZE pMaze = GetNewMaze();
			if (!pMaze && m_nSceneNo != IDD_MAZEI)
			{
				ResetLevel();
				pMaze = GetNewMaze();
			}
			if (pMaze)
			{
				UnloadMazeSprites();
				m_pCurrent = pMaze;
				GetToon()->LoadBackground( m_pCurrent->m_szBackground );
				InitMaze();
				StartGame();
			}
		}
	}
}

//************************************************************************
void CMazeScene::OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	if (m_fLevelComplete)
		return;

	LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
	if (lpSprite && lpSprite->Moving())
		return;

	if (vk != VK_UP && vk != VK_DOWN && vk != VK_LEFT && vk != VK_RIGHT)
	{	// Let base class handle the space bar
		CGBScene::OnKey (hWnd, vk, fDown, cRepeat, flags);
		return;
	}

	if (m_SoundPlaying == IntroPlaying)
	{
		if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT)
		{
		 	m_pSound->StopChannel(INTRO_CHANNEL);
			m_SoundPlaying = NotPlaying;
			StartGame();
		}
	}

	switch (vk)
	{
		case VK_UP:
			if (m_nAnnDirection != UP)
				if ( !ChangeAnnDirection(UP) )
					break;
			MoveAnn(0, -m_nMoveIncrement);
			break;
		case VK_DOWN:
			if (m_nAnnDirection != DOWN)
				if ( !ChangeAnnDirection(DOWN) )
					break;
			MoveAnn(0, m_nMoveIncrement);
			break;
		case VK_LEFT:
			if (m_nAnnDirection != LEFT)
				if ( !ChangeAnnDirection(LEFT) )
					break;
			MoveAnn(-m_nMoveIncrement, 0);
			break;
		case VK_RIGHT:
			if (m_nAnnDirection != RIGHT)
				if ( !ChangeAnnDirection(RIGHT) )
					break;
			MoveAnn(m_nMoveIncrement, 0);
			break;
	}

	if ( fDown ) // Means its a real keydown, not one generated by the timer
	{
		LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
		if (lpSprite)
		{
			int x, y;
			GetSpriteCenterPoint(lpSprite, &x, &y);
			m_fMovement = YES;
			m_ptMouse.x = x;
			m_ptMouse.y = y;
		}
	}
}

//************************************************************************
void CMazeScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	CGBScene::OnTimer (hWnd, id);
	if (id != ANIMATOR_TIMER_ID)
		return;

	if (m_SoundPlaying != NotPlaying)
		return;

	// Get Ann's current direction & position	
	LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
	if (! lpSprite)
		return;

	int x, y;
	GetSpriteCenterPoint (lpSprite, &x, &y);
	int iDeltaX, iDeltaY;
	iDeltaX = m_ptMouse.x - x;
	iDeltaY = m_ptMouse.y - y;

	// If mouse is too close, ignore
//	if (lpSprite->MouseInSprite(&m_ptMouse, NO/*fCheckTransparent*/, NULL/*lpWhereRect*/))
	if ((abs(iDeltaX) < 10) && (abs(iDeltaY) < 10))
	{
		m_ptMouse.x = x;
		m_ptMouse.y = y;
		return;
	}

	// Get Ann's new direction
	UINT nKey;
	if ( abs(iDeltaX) > abs(iDeltaY) )
	{ // dx is bigger
		if ( iDeltaX < 0 )
			nKey = VK_LEFT;
		else
			nKey = VK_RIGHT;
	}
	else
	{ // dy is bigger
		if ( iDeltaY < 0 )
			nKey = VK_UP;
		else
			nKey = VK_DOWN;
	}

	OnKey (hWnd, nKey, FALSE, 0, 0);
}

//************************************************************************
void CMazeScene::PlayIntro()
//************************************************************************
{
	if (! m_pSound)
		return;

	FNAME szFileName;
	GetToon()->SetHintState (FALSE);
	m_pSound->StopChannel (INTRO_CHANNEL);
	m_SoundPlaying = IntroPlaying;
	m_pSound->StartFile(GetPathName(szFileName, m_szIntroWave), FALSE, INTRO_CHANNEL, FALSE, m_hWnd);
}

//************************************************************************
void CMazeScene::PlayFinale()
//************************************************************************
{
	if (! m_pSound)
		return;

	FNAME szFilename;
	LPSTR lpWave;

	// Get the sound to play based on the level or if playing the game
	if (m_nSceneNo == IDD_MAZEI)
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
		m_pSound->StopChannel (SUCCESS_CHANNEL);
		m_SoundPlaying = SuccessPlaying;
		m_pSound->StartFile(GetPathName(szFilename, lpWave), FALSE, SUCCESS_CHANNEL, FALSE, m_hWnd);
	}
}

//************************************************************************
void CMazeScene::PlaySoundTrack()
//************************************************************************
{
	if (! m_pSound)
		return;

	FNAME szFileName;
	GetToon()->SetHintState (TRUE);
	if (lstrlen (m_szSoundTrack))
	{
		m_pSound->StopChannel (SOUNDTRACK_CHANNEL);
		m_pSound->StartFile(GetPathName(szFileName, m_szSoundTrack), TRUE, SOUNDTRACK_CHANNEL, FALSE);
	}
}

//************************************************************************
void CMazeScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	App.GotoScene (m_hWnd, IDD_MAZE1+iLevel, m_nNextSceneNo);
}

//************************************************************************
void CMazeScene::ResetLevel()
//************************************************************************
{
	for (int i = 0 ; i < m_nMazes ; ++i)
		m_pMazes[i].m_fCompleted = FALSE;

}

//************************************************************************
void CMazeScene::StartGame()
//************************************************************************
{
	m_fLevelComplete = FALSE;
 	PlaySoundTrack();
 	StartStopSign();
 	StartHouse();
}

//************************************************************************
void CMazeScene::InitMaze()
//************************************************************************
{
	LPMAZE pMaze = m_pCurrent;
	if (pMaze)
	{
		POINT pt;
		LPSPRITE lpSprite;
		int i, j;

		LoadMazeSprites();
		pMaze->m_Stop.m_iLocation = -1;
		for (i = 0; i < pMaze->m_iNumWarps; ++i)
		{
			LPWARP pWarp = &pMaze->m_Warps[i];
			for (j = 0 ; j < NUM_WARP_LOCS ; ++j)
			{
				POINT pt;

				if (pWarp->m_iLocation[j] >= pMaze->m_nPoints)
					pWarp->m_iLocation[j] = 0;
				pt = pMaze->m_ptDecision[pWarp->m_iLocation[j]];
				lpSprite = pWarp->m_lpSprite[j];
				if (lpSprite)
				{
					GetSpritePoint (lpSprite, &pt, &pt);
					lpSprite->Jump (pt.x, pt.y);
					lpSprite->Show (TRUE);
				}
			}
		}

		m_fInWarpZone = FALSE;
		if (pMaze->m_iAnnStart >= pMaze->m_nPoints)
			pMaze->m_iAnnStart = 0;
		m_ptAnnLast = pMaze->m_ptDecision[pMaze->m_iAnnStart];
		lpSprite = m_lpAnnSprite[m_nAnnDirection];
		if (lpSprite)
		{
			GetSpritePoint (lpSprite, &m_ptAnnLast, &pt);
			lpSprite->Jump (pt.x, pt.y);
			lpSprite->Show (TRUE);
			for (int i = 0 ; i < pMaze->m_iNumWarps ; ++i)
			{
				for (int j = 0 ; j < NUM_WARP_LOCS ; ++j)
				{
					LPSPRITE lpWarpSprite = pMaze->m_Warps[i].m_lpSprite[j];
					if (lpWarpSprite)
					{
						if (m_pAnimator->CheckCollision (lpSprite, lpWarpSprite))
						{
							m_fInWarpZone = TRUE;
							break;
						}
					}
				}
				if (m_fInWarpZone)
					break;
			}
		}
		if (pMaze->m_iHouseStart >= pMaze->m_nPoints)
			pMaze->m_iHouseStart = 0;
		m_iHouseLast = pMaze->m_iHouseStart;
		lpSprite = m_lpHouseSprite[m_nHouseDirection];
		if (lpSprite)
		{
			GetSpritePoint(lpSprite, &pMaze->m_ptDecision[m_iHouseLast], &pt);
			lpSprite->Jump(pt.x, pt.y);
			lpSprite->Show(TRUE);
		}

		m_pAnimator->StartAll();
		m_fStop = FALSE;
	}
}

//************************************************************************
BOOL CMazeScene::ChangeAnnDirection (int iNewDirection)
//************************************************************************
{
	if ( !m_fMovement )
		 return( NO );
	m_fMovement = NO;

	POINT ptLoc = m_ptAnnLast;
	POINT ptLocOrig = ptLoc;
	int iOldDirection = m_nAnnDirection;
	// Are we turning a corner?
	if ( (iOldDirection == DOWN || iOldDirection == UP) &&
		 (iNewDirection == LEFT || iNewDirection == RIGHT) )
			SnapToDecisionPoint(&ptLoc);
	else
	if ( (iOldDirection == LEFT || iOldDirection == RIGHT) &&
		 (iNewDirection == DOWN || iNewDirection == UP) )
			SnapToDecisionPoint(&ptLoc);

	// See if its OK to go that way
	int dx = ptLoc.x - ptLocOrig.x;
	int dy = ptLoc.y - ptLocOrig.y;
	if (iNewDirection == UP)
		dy -= m_nMoveIncrement;
	else
	if (iNewDirection == DOWN)
		dy += m_nMoveIncrement;
	else
	if (iNewDirection == LEFT)
		dx -= m_nMoveIncrement;
	else
	//if (iNewDirection == RIGHT)
		dx += m_nMoveIncrement;

	// check point scan distance away from current location
	int iSize = 0;
	if ( GetLevel() == 0 ) iSize = 28/2; else
	if ( GetLevel() == 1 ) iSize = 22/2; else
	if ( GetLevel() == 2 ) iSize = 18/2;

	LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
	if ( !CheckMove( lpSprite, dx, dy, iSize ) )
		return(NO);

	lpSprite->Show(FALSE);

	m_ptAnnLast = ptLoc;
	m_nAnnDirection = iNewDirection;

	lpSprite = m_lpAnnSprite[m_nAnnDirection];
	POINT pt;
	GetSpritePoint(lpSprite, &m_ptAnnLast, &pt);
	lpSprite->Jump(pt.x, pt.y);
	lpSprite->Show(TRUE);
	return(YES);
}

//************************************************************************
BOOL CMazeScene::MoveAnn (int dx, int dy)
//************************************************************************
{ // Note: the m_nRoadIndex value is the index into the palette for the road color

	if (m_fStop)
		return( NO );

	LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
	if (!lpSprite)
		return( NO );

	LPMAZE pMaze = m_pCurrent;
	if (!pMaze)
		return( NO );

	lpSprite->CycleCells();

	int iSize = 0;
	if ( GetLevel() == 0 ) iSize = 28/2; else
	if ( GetLevel() == 1 ) iSize = 22/2; else
	if ( GetLevel() == 2 ) iSize = 18/2;

	while (1)
	{
		if ( !dx && !dy )
			return( NO );

		if ( CheckMove( lpSprite, dx, dy, iSize ) )
			break;

		// Try a move of a smaller increment
		if ( dx )
			dx += ( dx < 0 ? +1 : -1 );
		else
		if ( dy )
			dy += ( dy < 0 ? +1 : -1 );
	}

	// see if this move will cause ann to collide with stop sign
	POINT pt;
	if (pMaze->m_Stop.m_lpSprite && pMaze->m_Stop.m_lpSprite->IsVisible())
	{
		RECT rSprite;
		lpSprite->Location(&rSprite);
		if (m_pAnimator->CheckCollision (lpSprite, pMaze->m_Stop.m_lpSprite, &rSprite))
		{ // You hit a stop sign; back to square one
			LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
			int i = 0;
//			// Pick a random spot to jump to
//			while ((i = GetRandomNumber(pMaze->m_nPoints)) == pMaze->m_iTarget)
//				;
			m_ptAnnLast = pMaze->m_ptDecision[i];
			GetSpritePoint(lpSprite, &m_ptAnnLast, &pt);
			lpSprite->Move(pt.x, pt.y);
			FNAME szFileName;
			GetSound()->StopChannel (INTRO_CHANNEL);
			GetSound()->StartFile(GetPathName(szFileName, "wind.wav"), FALSE/*bLoop*/, INTRO_CHANNEL, FALSE/*bWait*/);
			return( NO );
		}
	}

	m_ptAnnLast.x += dx;
	m_ptAnnLast.y += dy;
	GetSpritePoint(lpSprite, &m_ptAnnLast, &pt);
	lpSprite->Jump(pt.x, pt.y);
	WarpAnn();
	CheckCollision();
	CheckVictory();

	return( YES );
}

//************************************************************************
BOOL CMazeScene::CheckMove( LPSPRITE lpSprite, int dx, int dy, int iSize )
//************************************************************************
{
	if (!lpSprite)
		return( FALSE );

	RECT rSprite;
	lpSprite->Location(&rSprite);
	OffsetRect(&rSprite, dx, dy);
	if ( iSize )
	{
		RECT r = rSprite;
		rSprite.left   = (r.left + r.right)/2;
		rSprite.right  = (r.left + r.right + 1)/2;
		rSprite.top    = (r.top  + r.bottom)/2;
		rSprite.bottom = (r.top  + r.bottom + 1)/2;
		InflateRect(&rSprite, iSize, iSize);
	}
		
	PDIB pRoadDib = GetToon()->GetStageDib();
	if (!pRoadDib)
		return( FALSE );

	int x = pRoadDib->GetWidth();
	int y = abs(pRoadDib->GetHeight());
	if ( rSprite.left < 0 || rSprite.right  >= x ||
		 rSprite.top  < 0 || rSprite.bottom >= y )
		 	return( FALSE );

	LPCELL lpCell = lpSprite->GetCurrentCell();
	if (!lpCell || !lpCell->pSpriteDib)
		return( FALSE );

	PDIB pDib = lpCell->pSpriteDib;
	HPTR hp = pDib->GetXY (0, 0);
	BYTE Trans = *hp;
	HPTR hpRoad;

	// Check the top row
	int iEdges, iEdgeIntersections = 0;
	iEdges = 0;
	for (x = rSprite.left; x < rSprite.right; ++x)
	{
		hpRoad = pRoadDib->GetXY(x, rSprite.top);
		hp = pDib->GetXY(x-rSprite.left, rSprite.top-rSprite.top);
		if (/* *hp != Trans && */ *hpRoad == m_nRoadIndex) // its a hit on the roadway...
			iEdges++;
	}
	if ( iEdges <= 2 )
		iEdgeIntersections += iEdges;

	// Check the bottom row
	iEdges = 0;
	for (x = rSprite.left; x < rSprite.right; ++x)
	{
		hpRoad = pRoadDib->GetXY(x, rSprite.bottom-1);
		hp = pDib->GetXY(x-rSprite.left, rSprite.bottom-1-rSprite.top);
		if (/* *hp != Trans && */ *hpRoad == m_nRoadIndex) // its a hit on the roadway...
			iEdges++;
	}
	if ( iEdges <= 2 )
		iEdgeIntersections += iEdges;

	// Check the left col
	iEdges = 0;
	for (y = rSprite.top; y < rSprite.bottom; ++y)
	{
		hpRoad = pRoadDib->GetXY(rSprite.left, y);
		hp = pDib->GetXY(rSprite.left-rSprite.left, y-rSprite.top);
		if (/* *hp != Trans && */ *hpRoad == m_nRoadIndex) // its a hit on the roadway...
			iEdges++;
	}
	if ( iEdges <= 2 )
		iEdgeIntersections += iEdges;

	// Check the right col
	iEdges = 0;
	for (y = rSprite.top; y < rSprite.bottom; ++y)
	{
		hpRoad = pRoadDib->GetXY(rSprite.right-1, y);
		hp = pDib->GetXY(rSprite.right-1-rSprite.left, y-rSprite.top);
		if (/* *hp != Trans && */ *hpRoad == m_nRoadIndex) // its a hit on the roadway...
			iEdges++;
	}
	if ( iEdges <= 2 )
		iEdgeIntersections += iEdges;

	return( iEdgeIntersections == 3 || iEdgeIntersections == 4 ||
			iEdgeIntersections == 6 || iEdgeIntersections == 8 );
}                    

//************************************************************************
void CMazeScene::SnapToDecisionPoint(LPPOINT lpPoint)
//************************************************************************
{
	if (!lpPoint)
		return;

	LPMAZE pMaze = m_pCurrent;
	if (!pMaze)
		return;

	int closest = -1;
	long closestdist = -1;
	for (int i = 0 ; i < pMaze->m_nPoints ; ++i)
	{
		long xdist = lpPoint->x - pMaze->m_ptDecision[i].x;
		long ydist = lpPoint->y - pMaze->m_ptDecision[i].y;
		long dist = (xdist * xdist) + (ydist * ydist);
		if (closest < 0 || dist < closestdist)
		{
			closest = i;
			closestdist = dist;
		}
	}

	if (closest >= 0)
		*lpPoint = pMaze->m_ptDecision[closest];
}

//************************************************************************
void CMazeScene::WarpAnn()
//************************************************************************
{
	int i, j;
	POINT pt;
	LPSPRITE lpSprite, lpWarpSprite;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return;

	lpSprite = m_lpAnnSprite[m_nAnnDirection];
	if (! lpSprite)
		return;

	for (i = 0 ; i < pMaze->m_iNumWarps ; ++i)
	{
		for (j = 0; j < NUM_WARP_LOCS; ++j)
		{
			lpWarpSprite = pMaze->m_Warps[i].m_lpSprite[j];
			if (m_pAnimator->CheckCollision (lpSprite, lpWarpSprite))
			{
				// indicate we are now in a warp zone
				if (! m_fInWarpZone)
				{
					m_fInWarpZone = TRUE;

					// hide the sprite in the current direction
					if (lpSprite)
						lpSprite->Show (FALSE);

					// warp to new spot
					// where are we warping to
					if (j & 1)
						--j;
					else
						++j;

					// show the sprite in the correct spot
					m_ptAnnLast = pMaze->m_ptDecision[pMaze->m_Warps[i].m_iLocation[j]];
					GetSpritePoint (lpSprite, &m_ptAnnLast, &pt);
					lpSprite->Jump (pt.x, pt.y);
					lpSprite->Show (TRUE);
				}
				return;
			}
		}
	}
	m_fInWarpZone = FALSE;
}

//************************************************************************
void CMazeScene::StartHouse()
//************************************************************************
{
	int nNewDirection;
	LPSPRITE lpSprite;
	POINT ptHouseLast, pt, ptDecision;
	BOOL fWarped;

	m_fHouseMoving = FALSE;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return;

	// see if it's time to put up a stop sign
	DisplayStopSign();

	// see if we are over a warp zone
	fWarped = WarpHouse();

	// Get new direction for house to travel
	nNewDirection = GetNewDirection (m_iHouseLast, FALSE, fWarped);
	// if we are cornered try to force a warp
	if (nNewDirection < 0)
	{
		// force a warp
		fWarped = fWarped || WarpHouse(TRUE);
		// get a new direction to travel
		nNewDirection = GetNewDirection (m_iHouseLast, FALSE, fWarped);
	}

	// we damn well better have at least one possible direction
	if (nNewDirection >= 0)
	{
		// get last postion of house
		ptHouseLast = pMaze->m_ptDecision[m_iHouseLast];

		// get current sprite
		lpSprite = m_lpHouseSprite[m_nHouseDirection];

		// only need to switch sprites if direction changes
		if (nNewDirection != m_nHouseDirection)
		{
			// hide the sprite in the current direction
			if (lpSprite)
				lpSprite->Show (FALSE);

			// change to new direction
			m_nHouseDirection = nNewDirection;

			// get us going in the new direction
			lpSprite = m_lpHouseSprite[m_nHouseDirection];
			if (lpSprite)
			{
				// show the sprite in the correct spot
				GetSpritePoint (lpSprite, &ptHouseLast, &pt);

				lpSprite->Jump (pt.x, pt.y);
				lpSprite->Show (TRUE);
			}
		}

		// move the sprite to the next decision point
		if (lpSprite)
		{
			m_iHouseLast = GetDecisionPoint (&ptHouseLast, m_nHouseDirection, &ptDecision);
			GetSpritePoint (lpSprite, &ptDecision, &pt);
			lpSprite->Move (pt.x, pt.y);
			m_fHouseMoving = TRUE;
		}
	}
}

//************************************************************************
int CMazeScene::GetNewDirection (int iHousePos, BOOL fCheckMovePossible, BOOL fNoReverseCheck)
//************************************************************************
{
	int Directions[NUM_DIRECTIONS];
	int Reverse[] = {RIGHT, LEFT, DOWN, UP};
	int nDirections = 0;
	int nNewDirection = -1;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return (nNewDirection);

	POINT ptHouseLast = pMaze->m_ptDecision[iHousePos];

	// get list of possible directions
	for (int i = 0 ; i < NUM_DIRECTIONS ; ++i)
	{
		// don't reverse direction
		if (! fNoReverseCheck)
		{
			if (i == Reverse[m_nHouseDirection])
				continue;
		}
		if (CanGo (&ptHouseLast, i))
		{
			Directions[nDirections] = i;
			++nDirections;
		}
	}

	// reverse directions if that is all that you can do
	if (! fNoReverseCheck && ! nDirections && CanGo (&ptHouseLast, Reverse[m_nHouseDirection]))
	{
		Directions[nDirections] = Reverse[m_nHouseDirection];
		++nDirections;
	}

	// hopefully we have at least one possible direction (we're not cornered)
	if (nDirections)
	{
		// get new direction
		if (fCheckMovePossible)
			nNewDirection = Directions[0];
		else
			nNewDirection = Directions[GetRandomNumber(nDirections)];
	}

	return (nNewDirection);
}

//************************************************************************
BOOL CMazeScene::WarpHouse (BOOL fForceWarp)
//************************************************************************
{
	int i, j, iNewPos;
	WORD wRandom;
	POINT ptHouseLast, pt;
	LPSPRITE lpSprite;
	BOOL fWarped = FALSE;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return (FALSE);

	for (i = 0 ; i < pMaze->m_iNumWarps ; ++i)
	{
		if (! pMaze->m_Warps[i].m_iFrequency)
			continue;

		for (j = 0 ; j < NUM_WARP_LOCS ; ++j)
		{
			if (pMaze->m_Warps[i].m_iLocation[j] == m_iHouseLast)
			{
				if (j & 1)
					--j;
				else
					++j;

				if (i == 1)
					pt = ptHouseLast;

				iNewPos = pMaze->m_Warps[i].m_iLocation[j];
				// this is a check to see whether we
				// will land directly on top of Ann
				pt = pMaze->m_ptDecision[iNewPos];
				int iDir = WhereIsAnn (&pt);
				if (iDir < 0)
					iDir = 0;

				// make sure we can move in some direction 
				// when we warp and make sure we don't land
				// on top of Ann when we warp
				if ((GetNewDirection (iNewPos, TRUE, TRUE) >= 0) &&
					((iDir & ONTOPOF) == 0))
				{
					// see if it's time to warp
					if (fForceWarp)
						wRandom = 0;
					else
						wRandom = GetRandomNumber (pMaze->m_Warps[i].m_iFrequency);
					if (wRandom == 0) // warp it!
					{
						// hide the sprite in the current direction
						lpSprite = m_lpHouseSprite[m_nHouseDirection];
						if (lpSprite)
							lpSprite->Show (FALSE);

						m_iHouseLast = iNewPos;

						// show the sprite in the correct spot
						ptHouseLast = pMaze->m_ptDecision[m_iHouseLast];
						GetSpritePoint (lpSprite, &ptHouseLast, &pt);

						lpSprite->Jump (pt.x, pt.y);
						lpSprite->Show (TRUE);
						fWarped = TRUE;
					}
				}
				break;
			}
		}
	}
	return (fWarped);
}

//************************************************************************
void CMazeScene::LoadSprites()
//************************************************************************
{
	POINT ptOrigin;
	FNAME szFileName;
	int i;

	ptOrigin.x = ptOrigin.y = 0;
	for (i = 0 ; i < NUM_DIRECTIONS ; ++i)
	{
		if (m_lpAnnSprite[i])
		{
			m_pAnimator->DeleteSprite (m_lpAnnSprite[i]);
			m_lpAnnSprite[i] = NULL;
		}
		if (lstrlen (m_szAnn[i]))
		{
			m_lpAnnSprite[i] = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_lpAnnSprite[i])
			{
				m_lpAnnSprite[i]->SetCellsPerSec (0);
				GetPathName (szFileName, m_szAnn[i]);
				m_lpAnnSprite[i]->AddCells (szFileName, m_nAnnCells, NULL, NULL, TRUE);
			}
		}
                         
		if (m_lpHouseSprite[i])
		{
			m_pAnimator->DeleteSprite (m_lpHouseSprite[i]);
			m_lpHouseSprite[i] = NULL;
		}
		if (lstrlen (m_szHouse[i]))
		{
			m_lpHouseSprite[i] = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_lpHouseSprite[i])
			{
				m_lpHouseSprite[i]->SetNotifyProc (OnSpriteNotify, (DWORD)this);
				m_lpHouseSprite[i]->SetSpeed (m_nHouseSpeed);
				m_lpHouseSprite[i]->SetCellsPerSec (m_nHouseCycle);
				GetPathName (szFileName, m_szHouse[i]);
				m_lpHouseSprite[i]->AddCells (szFileName, m_nHouseCells, NULL, NULL, TRUE);
			}
		}
	}
}

//************************************************************************
void CMazeScene::UnloadMazeSprites()
//************************************************************************
{
	int i, j;
	LPWARP pWarp;

	// unload stop sign sprite
	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return;

	if (pMaze->m_Stop.m_lpSprite)
	{
		pMaze->m_Stop.m_lpSprite->Show (FALSE);
		m_pAnimator->DeleteSprite (pMaze->m_Stop.m_lpSprite);
		pMaze->m_Stop.m_lpSprite = NULL;
	}
	// unload all warp sprites
	for (i = 0 ; i < pMaze->m_iNumWarps ; ++i)
	{
		pWarp = &pMaze->m_Warps[i];			
		for (j = 0 ; j < NUM_WARP_LOCS ; ++j)
		{
			if (pWarp->m_lpSprite[j])
			{
				pWarp->m_lpSprite[j]->Show (FALSE);
				m_pAnimator->DeleteSprite (pWarp->m_lpSprite[j]);
				pWarp->m_lpSprite[j] = NULL;
			}
		}
	}
}

//************************************************************************
void CMazeScene::LoadMazeSprites()
//************************************************************************
{
	POINT ptOrigin;
	FNAME szFileName;
	int i, j;
	LPWARP pWarp;

	LPMAZE pMaze = m_pCurrent;
	if (!pMaze)
		return;

	ptOrigin.x = ptOrigin.y = 0;

	// load stop sign sprite
	if (pMaze->m_Stop.m_lpSprite)
	{
		m_pAnimator->DeleteSprite (pMaze->m_Stop.m_lpSprite);
		pMaze->m_Stop.m_lpSprite = NULL;
	}
	if (lstrlen(pMaze->m_Stop.m_szFileName))
	{
		pMaze->m_Stop.m_lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
		if (pMaze->m_Stop.m_lpSprite)
		{
			GetPathName (szFileName, pMaze->m_Stop.m_szFileName);
			pMaze->m_Stop.m_lpSprite->AddCells (szFileName, 4/*nCells*/, NULL, NULL, TRUE);
		}
	}

	// load all warp sprites
	for (i = 0 ; i < pMaze->m_iNumWarps ; ++i)
	{
		pWarp = &pMaze->m_Warps[i];			
		for (j = 0 ; j < NUM_WARP_LOCS ; ++j)
		{
			if (pWarp->m_lpSprite[j])
			{
				m_pAnimator->DeleteSprite (pWarp->m_lpSprite[j]);
				pWarp->m_lpSprite[j] = NULL;
			}
			if (lstrlen (pWarp->m_szFileName))
			{
				pWarp->m_lpSprite[j] = m_pAnimator->CreateSprite (&ptOrigin);
				if (pWarp->m_lpSprite[j])
				{
					GetPathName (szFileName, pWarp->m_szFileName);
					pWarp->m_lpSprite[j]->AddCell (szFileName, NULL, 0, 0, TRUE);
				}
			}
		}
	}
}

//************************************************************************
void CMazeScene::GetSpritePoint (LPSPRITE lpSprite, LPPOINT ptCenter, LPPOINT ptNew)
//************************************************************************
{
	int iWidth, iHeight;

	lpSprite->GetMaxSize (&iWidth, &iHeight);
	ptNew->x = ptCenter->x - (iWidth/2);
	ptNew->y = ptCenter->y - (iHeight/2);
}

//************************************************************************
BOOL CMazeScene::CanGo (LPPOINT lpPoint, int nDirection)
//************************************************************************
{
	int x = lpPoint->x;
	int y = lpPoint->y;
	if (nDirection == UP)
		y -= m_nScanDistance;
	else
	if (nDirection == DOWN)
		y += m_nScanDistance;
	else
	if (nDirection == LEFT)
		x -= m_nScanDistance;
	else
	//if (nDirection == RIGHT)
		x += m_nScanDistance;

	// check point scan distance away from current location
	LPSPRITE lpSprite = m_lpHouseSprite[m_nHouseDirection];
	int dx, dy;
	GetSpriteCenterPoint(lpSprite, &dx, &dy);
	dx = x - dx;
	dy = y - dy;
	if ( !CheckMove( lpSprite, dx, dy ) )
		return (FALSE);
	
	// check to make sure we can find a point in our
	// decision list to move to
	POINT pt;
	return (GetDecisionPoint (lpPoint, nDirection, &pt) >= 0);
}

//************************************************************************
int CMazeScene::GetDecisionPoint (LPPOINT lpCurPoint, int nDirection, LPPOINT lpNewPoint)
//************************************************************************
{
	int i, closest, closestdist, dist;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return(-1);

	closest = -1;
	for (i = 0 ; i < pMaze->m_nPoints ; ++i)
	{
		dist = -1;
		// can't go to where stop sign is
		if (nDirection == UP || nDirection == DOWN)
		{
			// see if point has same x
			if (pMaze->m_ptDecision[i].x == lpCurPoint->x)
			{
				if (nDirection == UP)
					dist = lpCurPoint->y - pMaze->m_ptDecision[i].y;
				else
					dist = pMaze->m_ptDecision[i].y - lpCurPoint->y;
			}
		}
		else
		{
			// see if point has same y
			if (pMaze->m_ptDecision[i].y == lpCurPoint->y)
			{
				if (nDirection == LEFT)
					dist = lpCurPoint->x - pMaze->m_ptDecision[i].x;
				else
					dist = pMaze->m_ptDecision[i].x - lpCurPoint->x;
			}
		}		
		if (dist > 0)
		{
			if (closest < 0 || dist < closestdist)
			{
				closest = i;
				closestdist = dist;
			}
		}
	}
	if (closest >= 0)
	{
		if (closest == pMaze->m_Stop.m_iLocation)
			closest = -1;
		else
			*lpNewPoint = pMaze->m_ptDecision[closest];
	}
	return (closest);
}

//************************************************************************
int CMazeScene::WhereIsAnn (LPPOINT lpStartPt)
//************************************************************************
{
	int i;
	int x, y, px, py, dx, dy;
	int iClosest, iClosestDist, iDist, iDirection, iDepth;
	POINT pt;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return (FALSE);

	// find the decision point Ann is closest to
	x = m_ptAnnLast.x;
	y = m_ptAnnLast.y;
	iClosest = -1;

	for (i = 0 ; i < pMaze->m_nPoints ; ++i)
	{
		px = pMaze->m_ptDecision[i].x;
		py = pMaze->m_ptDecision[i].y;
		iDist = max (abs(x-px), abs (y-py));
		if (iClosest < 0 || iDist < iClosestDist)
		{
			iClosest = i;
			iClosestDist = iDist;
		}
	}

	px = pMaze->m_ptDecision[iClosest].x;
	py = pMaze->m_ptDecision[iClosest].y;

	// on top of house?
	if (px == lpStartPt->x && py == lpStartPt->y)
	{
		dx = m_ptAnnLast.x - lpStartPt->x;
		dy = m_ptAnnLast.y - lpStartPt->y;
		if (abs(dx) > abs(dy))
			return (dx < 0 ? ONTOPOF|LEFT : ONTOPOF|RIGHT);
		else
			return (dy < 0 ? ONTOPOF|UP : ONTOPOF|DOWN);
	}
	else
	// even with house horizontally or vertically?
	if (px == lpStartPt->x || py == lpStartPt->y)
	{
		// above or below house?
		if (px == lpStartPt->x)
		{
			if (py > lpStartPt->y)
				iDirection = DOWN;
			else
				iDirection = UP;
		}
		else
		// left or right of house?
		//if (py == lpStartPt->y)
		{
			if (px > lpStartPt->x)
				iDirection = RIGHT;
			else
				iDirection = LEFT;
		}

		pt = *lpStartPt;
		iDepth = pMaze->m_iVision;		
		while (--iDepth >= 0)
		{
			if (! CanGo (&pt, iDirection))
				break;

			GetDecisionPoint (&pt, iDirection, &pt);
			if (pt.x == px && pt.y == py)
				return (iDirection);
		}
	}

	return (-1);
}

//************************************************************************
BOOL CMazeScene::CheckCollision()
//************************************************************************
{
	if (m_fStop)
		return (FALSE);
	if (!m_lpAnnSprite[m_nAnnDirection] || !m_lpHouseSprite[m_nHouseDirection])
		return (FALSE);
	if (!m_pAnimator->CheckCollision(m_lpAnnSprite[m_nAnnDirection], m_lpHouseSprite[m_nHouseDirection]))
		return (FALSE);

	LPMAZE pMaze = m_pCurrent;
	if ( !pMaze )
		return(FALSE);

	POINT pt;
	LPSPRITE lpSprite;

	lpSprite = m_lpAnnSprite[m_nAnnDirection];
	int i = 0;
//	// Pick a random spot to jump to
//	while ((i = GetRandomNumber(pMaze->m_nPoints)) == pMaze->m_iTarget)
//		;
	m_ptAnnLast = pMaze->m_ptDecision[i];
	GetSpritePoint(lpSprite, &m_ptAnnLast, &pt);
	lpSprite->Move(pt.x, pt.y);
	FNAME szFileName;
	GetSound()->StopChannel (INTRO_CHANNEL);
	GetSound()->StartFile(GetPathName(szFileName, "wind.wav"), FALSE/*bLoop*/, INTRO_CHANNEL, FALSE/*bWait*/);

	lpSprite = m_lpHouseSprite[m_nHouseDirection];
	m_iHouseLast = pMaze->m_iHouseStart;
	GetSpritePoint(lpSprite, &pMaze->m_ptDecision[m_iHouseLast], &pt);
	lpSprite->Jump(pt.x, pt.y);
	StartHouse();

	return (TRUE);
}

//************************************************************************
BOOL CMazeScene::CheckVictory()
//************************************************************************
{
	if (m_fStop)
		return(FALSE);

	LPSPRITE lpSprite = m_lpAnnSprite[m_nAnnDirection];
	if (!lpSprite)
		return(FALSE);

	LPMAZE pMaze = m_pCurrent;
	if ( !pMaze )
		return(FALSE);

	POINT pt;
	pt = pMaze->m_ptDecision[pMaze->m_iTarget];
	if ( !lpSprite->MouseInSprite( &pt, YES/*fCheckTransparent*/, NULL/*lpRect*/) )
		return (FALSE);

	// You found the target point; you win!!!
	m_pAnimator->StopAll();
	m_pCurrent->m_fCompleted = TRUE;

	GetNewMaze();
	PlayFinale();

	m_fStop = TRUE;
	return (TRUE);
}

//************************************************************************
void CMazeScene::RemoveStopSign()
//************************************************************************
{
	BOOL bSetupStopSign = TRUE;

	LPMAZE pMaze = m_pCurrent;
	if (!pMaze)
		return;
	if (!pMaze->m_Stop.m_lpSprite || !pMaze->m_Stop.m_lpSprite->IsVisible())
		return;

	if (timeGetTime() >= pMaze->m_Stop.m_dwNextTime)
	{
		pMaze->m_Stop.m_lpSprite->Show (FALSE);
		pMaze->m_Stop.m_iLocation = -1;
		StartStopSign();
	}
}

//************************************************************************
void CMazeScene::StartStopSign()
//************************************************************************
{
	LPMAZE pMaze = m_pCurrent;
	if (!pMaze)
		return;

	DWORD dwRandom = (DWORD)rand();
	DWORD dwRange = pMaze->m_Stop.m_dwMaxFrequency - pMaze->m_Stop.m_dwMinFrequency;
	dwRange = (dwRange * dwRandom) / (DWORD)RAND_MAX;
	pMaze->m_Stop.m_dwNextTime = timeGetTime() + pMaze->m_Stop.m_dwMinFrequency + dwRange;
}

//************************************************************************
void CMazeScene::DisplayStopSign()
//************************************************************************
{
	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return;
	if (! pMaze->m_Stop.m_lpSprite || pMaze->m_Stop.m_lpSprite->IsVisible())
		return;

	if (timeGetTime() >= pMaze->m_Stop.m_dwNextTime)
	{
		int i;
		POINT pt;

		// first figure out where to display stop sign
		// select a random location that is not on top
		// of the house or ann
		while (TRUE)
		{
			LPSPRITE lpSprite;

			i = GetRandomNumber (pMaze->m_nPoints);
			// don't put stop sign on top of house
			if (i == m_iHouseLast)
				continue;
			// don't put stop sign on top of ann
			GetSpritePoint (pMaze->m_Stop.m_lpSprite, &pMaze->m_ptDecision[i], &pt);
			pMaze->m_Stop.m_lpSprite->Jump (pt.x, pt.y);
			lpSprite = m_lpAnnSprite[m_nAnnDirection];
			if (lpSprite)
			{
				if (m_pAnimator->CheckCollision (lpSprite, pMaze->m_Stop.m_lpSprite))
					continue;
			}
			break;
		}
		pMaze->m_Stop.m_lpSprite->Show (TRUE);
		pMaze->m_Stop.m_iLocation = i;

		DWORD dwRandom = (DWORD)rand();
		DWORD dwRange = pMaze->m_Stop.m_dwMaxDuration - pMaze->m_Stop.m_dwMinDuration;
		dwRange = (dwRange * dwRandom) / (DWORD)RAND_MAX;
		pMaze->m_Stop.m_dwNextTime = timeGetTime() + pMaze->m_Stop.m_dwMinDuration + dwRange;
	}
}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPMAZESCENE lpScene = (LPMAZESCENE)dwNotifyData;
		lpScene->OnNotify (lpSprite, Notify);
	}
}

//************************************************************************
void CMazeScene::OnNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (Notify == SN_MOVED)
	{
		RemoveStopSign();
		CheckCollision();
	}
	else
	if (Notify == SN_MOVEDONE)
	{
		if (! m_fStop)
			StartHouse();
	}
}

//************************************************************************
LPMAZE CMazeScene::GetNewMaze()
//************************************************************************
{
	int iRand;
	int iCount = 0;

	for (int i = 0; i < m_nMazes; ++i)
	{
		if (!m_pMazes[i].m_fCompleted)
			++iCount;
	}
	if (!iCount)
		return (NULL);

	iRand = GetCycleNumber(iCount);
	iCount = 0;
	for (i = 0 ; i < m_nMazes ; ++i)
	{
		if (! m_pMazes[i].m_fCompleted)
		{
			if (iCount == iRand)
				break;
			++iCount;
		}
	}
	return (&m_pMazes[i]);
}

//************************************************************************
CWarp::CWarp()
//************************************************************************
{
	int i;

	m_szFileName[0] = '\0';
	for (i = 0 ; i < NUM_WARP_LOCS ; ++i)
	{
		m_iLocation[i] = -1;
		m_lpSprite[i] = NULL;
	}
}

//************************************************************************
CStop::CStop()
//************************************************************************
{
	m_szFileName[0] = '\0';
	m_dwMinFrequency = 0;
	m_dwMaxFrequency = 0;
	m_dwMinDuration = 0;
	m_dwMaxDuration = 0;
	m_dwNextTime = 0;
	m_lpSprite = NULL;
	m_iLocation = -1;
}

//************************************************************************
CMaze::CMaze()
//************************************************************************
{
	m_szBackground[0] = '\0';		
	m_iAnnStart = 0;
	m_iHouseStart = 0;
	m_iVision = 1;
	m_iWarpFreq = 1;
	m_iNumWarps = 0;
	m_nPoints = 0;
	for (int i = 0 ; i < MAX_DECISION_PTS ; ++i)
		m_ptDecision[i].x = m_ptDecision[i].y = 0;
	m_fCompleted = FALSE;
}

//************************************************************************
CMaze::~CMaze()
//************************************************************************
{
}

//************************************************************************
BOOL CMazeParser::GetDecisionPoints (LPMAZE lpMaze, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	int i, nPoints;

	nPoints = nValues / 2;
	if (!nValues || (nValues & 1) || nPoints > MAX_DECISION_PTS)
	{
		Print("'%ls'\n Invalid point list '%ls'", lpEntry, lpValues);
		return (FALSE);
	}

	i = lpMaze->m_nPoints;
	lpMaze->m_nPoints += nPoints;

	while (--nPoints >= 0)
	{
		lpValue = GetNextValue (&lpValues);
		l = latol (lpValue, &bError);
		if (bError || l < 0)
		{
			Print ("'%ls'\n Bad number at '%ls'", lpEntry, lpValue);
			return (FALSE);
		}

		lpMaze->m_ptDecision[i].x = (int)l;
		lpValue = GetNextValue (&lpValues);
		l = latol (lpValue, &bError);
		if (bError || l < 0)
		{
			Print ("'%ls'\n Bad number at '%ls'", lpEntry, lpValue);
			return (FALSE);
		}
		lpMaze->m_ptDecision[i].y = (int)l;
		++i;
	}
	return (TRUE);
}

//************************************************************************
BOOL CMazeParser::GetSuccessWaves (LPMAZESCENE lpScene, LPSTR lpEntry,
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
BOOL CMazeParser::GetWarp (LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	int i;
	LPSTR lpValue;
	BOOL bError;
	long l;

	if (nValues != 4)
	{
		Print ("'%ls'\n Invalid warp specification '%ls'", lpEntry, lpValues);
		return (FALSE);
	}

	i = pMaze->m_iNumWarps;
	++pMaze->m_iNumWarps;

	lpValue = GetNextValue (&lpValues);
	lstrcpy (pMaze->m_Warps[i].m_szFileName, lpValue);

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Warps[i].m_iFrequency = (int)l;

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Warps[i].m_iLocation[0] = (int)l;

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Warps[i].m_iLocation[1] = (int)l;

	return (TRUE);
}

//************************************************************************
BOOL CMazeParser::GetStop (LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;
	BOOL bError;
	long l;

	if (nValues != 5)
	{
		Print ("'%ls'\n Invalid stop specification '%ls'", lpEntry, lpValues);
		return (FALSE);
	}

	lpValue = GetNextValue (&lpValues);
	lstrcpy (pMaze->m_Stop.m_szFileName, lpValue);

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Stop.m_dwMinFrequency = l;

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Stop.m_dwMaxFrequency = l;

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Stop.m_dwMinDuration = l;

	lpValue = GetNextValue (&lpValues);
	l = latol (lpValue, &bError);
	if (bError || l < 0)
	{
		Print ("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return (FALSE);
	}
	pMaze->m_Stop.m_dwMaxDuration = l;

	return (TRUE);
}

//************************************************************************
BOOL CMazeParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPMAZESCENE lpScene = (LPMAZESCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "intro"))
			GetFilename(lpScene->m_szIntroWave, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetFilename(lpScene->m_szSoundTrack, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "successplay"))
			GetFilename(lpScene->m_szSuccessPlay, lpEntry, lpValues, nValues);
		else
		if (! lstrcmpi (lpKey, "successlevel"))
			GetSuccessWaves (lpScene, lpEntry, lpValues, nValues, 1);
		else
		if (!lstrcmpi(lpKey, "anncells"))
			GetInt(&lpScene->m_nAnnCells, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "housecells"))
			GetInt(&lpScene->m_nHouseCells, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "anncycle"))
			GetInt(&lpScene->m_nAnnCycle, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "housecycle"))
			GetInt(&lpScene->m_nHouseCycle, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "housespeed"))
			GetInt(&lpScene->m_nHouseSpeed, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "gamearea"))
			GetRect(&lpScene->m_rGameArea, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "roadindex"))
			GetInt(&lpScene->m_nRoadIndex, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "moveinc"))
			GetInt(&lpScene->m_nMoveIncrement, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "scandistance"))
			GetInt(&lpScene->m_nScanDistance, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "annl"))
			GetFilename(lpScene->m_szAnn[LEFT], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "annr"))
			GetFilename(lpScene->m_szAnn[RIGHT], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "annu"))
			GetFilename(lpScene->m_szAnn[UP], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "annd"))
			GetFilename(lpScene->m_szAnn[DOWN], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "housel"))
			GetFilename(lpScene->m_szHouse[LEFT], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "houser"))
			GetFilename(lpScene->m_szHouse[RIGHT], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "houseu"))
			GetFilename(lpScene->m_szHouse[UP], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "housed"))
			GetFilename(lpScene->m_szHouse[DOWN], lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		LPMAZE lpMaze = lpScene->GetMaze(nIndex-1);
		if (!lstrcmpi(lpKey, "bg"))
			GetFilename(lpMaze->m_szBackground, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "annstart"))
			GetInt(&lpMaze->m_iAnnStart, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "housestart"))
			GetInt(&lpMaze->m_iHouseStart, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "target"))
			GetInt(&lpMaze->m_iTarget, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "warpfreq"))
			GetInt(&lpMaze->m_iWarpFreq, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "vision"))
			GetInt(&lpMaze->m_iVision, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "decision"))
			GetDecisionPoints(lpMaze, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "warp"))
			GetWarp(lpMaze, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "stop"))
			GetStop(lpMaze, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

