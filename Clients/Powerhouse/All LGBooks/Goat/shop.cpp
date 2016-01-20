#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "goat.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "shop.h"
#include "shopdef.h"
#include "randseq.h"


class FAR CShopParser : public CParser
{
public:
	CShopParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetDecisionPoints(LPMAZE lpMaze, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetWarp(LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetStop(LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues);
	void SaveItemLoc (LPSHOPSCENE lpScene ,LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

static WORD wRandomSeed;

//************************************************************************
CShopScene::CShopScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	int i;

	for (i = 0; i < NUM_DIRECTIONS; ++i)
	{
		m_lpShopSprite[i] = NULL;
		m_szAnn[i][0] = '\0';
	}

	m_szFindItemWave[0] = '\0';
	m_szWrongItemWave[0] = '\0';
	m_szSuccessWave[0] = '\0';

	// Init. the list structures
	for (i = 0 ; i < MAX_SHOP_ITEMS ; i++)
	{
		m_ObjectList[i].m_ptLoc.x = m_ObjectList[i].m_ptLoc.y = 0;
		m_ObjectList[i].m_lpSprite = NULL;
		m_ObjectList[i].m_fFound = FALSE;
		m_ObjectList[i].m_lpNoCheckSprite = NULL;
		m_ObjectList[i].m_lpCheckSprite = NULL;

		m_MasterList[i].m_szImage[0] = '\0';
		m_MasterList[i].m_szNoCheckImage[0] = '\0';
		m_MasterList[i].m_szCheckImage[0] = '\0';
		m_MasterList[i].m_fInUse = FALSE;

		m_LocationList[i].m_ptLoc.x = m_LocationList[i].m_ptLoc.y = 0;
		m_LocationList[i].m_fInUse = FALSE;
	}

	m_nAnnCells = 3;
	m_nAnnCycle = 8;
	m_nMazes = 0;
	m_pMazes = NULL;
	m_nRoadIndex = 0;			// Index to the road palette color
	m_nMoveIncrement = 1;
	m_nDirection = RIGHT;
	m_nScanDistance = 50;
	m_dwLastMove = 0;
	m_fIntro = FALSE;
	m_fStop = FALSE;
	m_fInWarpZone = FALSE;
	m_pSound = NULL;
	m_SoundPlaying = NotPlaying;
	m_szIntroWave[0] = '\0';
	m_szSoundTrack[0] = '\0';
	m_pCurrent = NULL;
	m_nMasterLoadCount = 0;
	m_nItemLoadCount = 0;
	m_nLocationCount = 0;
	m_iItemsCaught = 0;
	m_iItemsNeeded = 0;
	m_fLevelComplete = FALSE;
	m_iVertSpace = 30;
	m_iItemSelected = -1;
	m_fFindInOrder = FALSE;
}


//************************************************************************
CShopScene::~CShopScene()
//************************************************************************
{
	if (m_pMazes)
		delete [] m_pMazes;
	if (m_pSound)
		delete m_pSound;
}

//************************************************************************
int CShopScene::GetLevel()
//************************************************************************
{
	if (m_nSceneNo == IDD_SHOPPINGI)
		return (0);
	else
		return (m_nSceneNo - IDD_SHOPPING1);
}

//************************************************************************
BOOL CShopScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);

	m_pSound = new CSound (FALSE);
	if (m_pSound)
		m_pSound->Open (NULL);

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

		// read in the connect point information
		lpTableData = GetResourceData (m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CShopParser parser(lpTableData);
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
	}

	return (TRUE);
}

//************************************************************************
void CShopScene::ToonInitDone()
//************************************************************************
{
	GetToon()->SetHintState(TRUE);
	LoadSprites();
	InitMaze();
	PlayIntro(NULL/*don't want WOM_DONE*/);
}

//************************************************************************
void CShopScene::OnCommand (HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_HELP:
		{
			PlayIntro(NULL/*don't want WOM_DONE*/);
			break;
		}

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
BOOL CShopScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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
void CShopScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	if ( !fDoubleClick )
		SetCapture(hWnd);

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
		if (m_nSceneNo == IDD_SHOPPINGI)
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
				ResetSprites();
				StartGame();
			}
		}
	}

	m_ptMouse.x = x;
	m_ptMouse.y = y;
}

//************************************************************************
void CShopScene::OnLButtonUp (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	ReleaseCapture();
}

//************************************************************************
void CShopScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if ( m_SoundPlaying != NotPlaying )
		return;

	POINT pt;
	pt.x = x;
	pt.y = y;
	if (PtInRect(&m_rGameArea, pt))
	{
		m_ptMouse.x = x;
		m_ptMouse.y = y;
	}
}

//************************************************************************
BOOL CShopScene::GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY)
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
BOOL CShopScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
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
void CShopScene::OnWomDone (HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	// If Play mode, go to next scene when they catch the house
	if (m_SoundPlaying == SuccessPlaying && m_nSceneNo == IDD_SHOPPINGI)
	{
		m_SoundPlaying = NotPlaying;
		FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
	}
}

//************************************************************************
void CShopScene::OnKey (HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	if (m_fLevelComplete)
		return;

	if (m_SoundPlaying == IntroPlaying)
	{
		if (vk == VK_UP || vk == VK_DOWN || vk == VK_LEFT || vk == VK_RIGHT)
		{
			m_SoundPlaying = NotPlaying;
		 	m_pSound->StopChannel(INTRO_CHANNEL);
			StartGame();
		}
	}

	switch (vk)
	{
		case VK_UP:
			if (m_nDirection != UP)
				ChangeShopperDirection (UP);
			MoveShopper (0, -m_nMoveIncrement);
			break;
		case VK_DOWN:
			if (m_nDirection != DOWN)
				ChangeShopperDirection (DOWN);
			MoveShopper (0, m_nMoveIncrement);
			break;
		case VK_LEFT:
			if (m_nDirection != LEFT)
				ChangeShopperDirection (LEFT);
			MoveShopper (-m_nMoveIncrement, 0);
			break;
		case VK_RIGHT:
			if (m_nDirection != RIGHT)
				ChangeShopperDirection (RIGHT);
			MoveShopper (m_nMoveIncrement, 0);
			break;
		default:
			// Let base class handle the space bar
			CGBScene::OnKey (hWnd, vk, fDown, cRepeat, flags);
			break;
	}

	if ( fDown ) // Means its a real keydown, not one generated by the timer
	{
		LPSPRITE lpSprite = m_lpShopSprite[m_nDirection];
		if (lpSprite)
		{
			int x, y;
			GetSpriteCenterPoint(lpSprite, &x, &y);
			m_ptMouse.x = x;
			m_ptMouse.y = y;
		}
	}
}

//************************************************************************
void CShopScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	if (id != ANIMATOR_TIMER_ID)
	{
		CGBScene::OnTimer (hWnd, id);
		return;
	}

	if (m_SoundPlaying != NotPlaying)
		return;

	// Get Ann's current direction & position	
	LPSPRITE lpSprite = m_lpShopSprite[m_nDirection];
	if (! lpSprite)
		return;

	int x, y;
	GetSpriteCenterPoint (lpSprite, &x, &y);
	int iDeltaX, iDeltaY;
	iDeltaX = m_ptMouse.x - x;
	iDeltaY = m_ptMouse.y - y;

	// If mouse is too close, ignore
	if ((abs(iDeltaX) < 10) && (abs(iDeltaY) < 10))
	{
		m_ptMouse.x = x;
		m_ptMouse.y = y;
		return;
	}

	// Get Ann's new direction
	UINT nKey;
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

//************************************************************************
void CShopScene::PlayIntro(HWND hWnd/*NULL for no WOM_DONE*/)
//************************************************************************
{
	FNAME szFileName;

	// Play the intro wave if one is defined
	if (m_pSound && m_szIntroWave[0] != '\0')
	{
		m_SoundPlaying = IntroPlaying;
		m_pSound->StopChannel (INTRO_CHANNEL);
		m_pSound->StartFile (GetPathName (szFileName, m_szIntroWave), NO/*bLoop*/, 
								INTRO_CHANNEL/*iChannel*/, FALSE, hWnd);
	}
}

//************************************************************************
void CShopScene::PlaySuccess()
//************************************************************************
{
	FNAME szFileName;

	// Play the intro wave if one is defined
	if (m_pSound && m_szSuccessWave[0] != '\0')
	{
		m_SoundPlaying = SuccessPlaying;
		m_pSound->StopChannel (SUCCESS_CHANNEL);
		if (m_nSceneNo == IDD_SHOPPINGI)
		{
			m_pSound->StartFile (GetPathName (szFileName, m_szSuccessWave), NO/*bLoop*/,
									SUCCESS_CHANNEL, TRUE, m_hWnd);
			FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
		}
		else
			m_pSound->StartFile (GetPathName (szFileName, m_szSuccessWave), NO/*bLoop*/,
									SUCCESS_CHANNEL, FALSE, m_hWnd);
	}
}

//************************************************************************
void CShopScene::PlaySoundTrack()
//************************************************************************
{
	if (! m_pSound)
		return;

	FNAME szFileName;
	if (lstrlen (m_szSoundTrack))
	{
		m_pSound->StopChannel (MUSIC_CHANNEL);
		GetPathName (szFileName, m_szSoundTrack);
		m_pSound->StartFile (szFileName, TRUE, MUSIC_CHANNEL, FALSE);
	}
}

//************************************************************************
void CShopScene::ChangeLevel (int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS - 1;
	App.GotoScene (m_hWnd, IDD_SHOPPING1 + iLevel, m_nNextSceneNo);
}

//************************************************************************
void CShopScene::ResetLevel()
//************************************************************************
{
	for (int i = 0 ; i < m_nMazes ; i++)
		m_pMazes[i].m_fCompleted = FALSE;

	for (i = 0 ; i < m_nItemLoadCount ; i++)
		m_MasterList[i].m_fInUse = FALSE;

	for (i = 0 ; i < m_nLocationCount ; i++)
		m_LocationList[i].m_fInUse = FALSE;

	m_iItemsCaught = 0;
}

//************************************************************************
void CShopScene::StartGame()
//************************************************************************
{
	// Show all of the items and reset the found flags
	for (int i = 0 ; i < m_iItemsNeeded ; i++)
	{
		m_ObjectList[i].m_fFound = FALSE;
		if (m_ObjectList[i].m_lpSprite)
			m_ObjectList[i].m_lpSprite->Show (TRUE);

		m_ObjectList[i].m_lpCheckSprite->Show (FALSE);
		m_ObjectList[i].m_lpNoCheckSprite->Show (TRUE);
	}

	m_iItemsCaught = 0;
	m_fLevelComplete = FALSE;
	m_iItemSelected = -1;
	SelectItemToSearchFor();
 	PlaySoundTrack();
 	StartStopSign();
}

//************************************************************************
void CShopScene::InitMaze()
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
		if (pMaze->m_iShopperStart >= pMaze->m_nPoints)
			pMaze->m_iShopperStart = 0;
		m_ptAnnLast = pMaze->m_ptDecision[pMaze->m_iShopperStart];
		lpSprite = m_lpShopSprite[m_nDirection];
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

		m_pAnimator->StartAll();
		m_fStop = FALSE;
	}
}

//************************************************************************
void CShopScene::ChangeShopperDirection (int iNewDirection)
//************************************************************************
{
	POINT pt;

	m_lpShopSprite[m_nDirection]->Show(FALSE);
	int iOldDirection = m_nDirection;
	m_nDirection = iNewDirection;

	GetSpritePoint (m_lpShopSprite[m_nDirection], &m_ptAnnLast, &pt);
	m_lpShopSprite[m_nDirection]->Jump (pt.x, pt.y);
	m_lpShopSprite[m_nDirection]->Show (TRUE);

	for ( int i=1; i<10; i++ )
	{ // Move Shopper away from the edge
		if ( iOldDirection == UP )
		{
			if ( MoveShopper(0, +i) ) return; // Move Shopper DOWN off the edge
		}
		else
		if ( iOldDirection == DOWN )
		{
			if ( MoveShopper(0, -i) ) return; // Move Shopper UP off the edge
		}
		else
		if ( iOldDirection == LEFT )
		{
			if ( MoveShopper(+i, 0) ) return; // Move Shopper RIGHT off the edge
		}
		else
		if ( iOldDirection == RIGHT )
		{
			if ( MoveShopper(-i, 0) ) return; // Move Shop LEFT off the edge
		}
	}

	for ( i=1; i<10; i++ )
	{ // Move Shopper away from the edge
		if ( MoveShopper(+i, -i) ) return; // Move Shopper NE off the edge
		if ( MoveShopper(+i, +i) ) return; // Move Shopper SE off the edge
		if ( MoveShopper(-i, +i) ) return; // Move Shopper SW off the edge
		if ( MoveShopper(-i, -i) ) return; // Move Shopper NW off the edge
	}
}

//************************************************************************
BOOL CShopScene::MoveShopper (int dx, int dy)
//************************************************************************
{ // Note: the m_nRoadIndex value is the index into the palette for the road color

	if (m_fStop)
		return( NO );

	LPSPRITE lpSprite = m_lpShopSprite[m_nDirection];
	if (!lpSprite)
		return( NO );

	LPMAZE pMaze = m_pCurrent;
	if (!pMaze)
		return( NO );

	lpSprite->CycleCells();

	RECT rSprite;
	while (1)
	{
		if ( !dx && !dy )
			return( NO );

		lpSprite->Location(&rSprite);
		OffsetRect(&rSprite, dx, dy);
		
		RECT rArea = rSprite;
//		IntersectRect(&rArea, &rSprite, &m_rGameArea);
//		if (!IntersectRect(&rArea, &rSprite, &m_rGameArea))
//			return( NO );
	
		if ( CheckMove( lpSprite, rArea, rSprite ) )
			break;

		// Try a move of a smaller increment
		if ( dx )
			dx += ( dx < 0 ? +1 : -1 );
		else
		if ( dy )
			dy += ( dy < 0 ? +1 : -1 );
	}

	// see if this move will cause ann to collide with stop sign
	if (pMaze->m_Stop.m_lpSprite && pMaze->m_Stop.m_lpSprite->IsVisible())
	{
		if (m_pAnimator->CheckCollision (lpSprite, pMaze->m_Stop.m_lpSprite, &rSprite))
			return( NO );
	}

	POINT pt;
	m_ptAnnLast.x += dx;
	m_ptAnnLast.y += dy;
	GetSpritePoint (lpSprite, &m_ptAnnLast, &pt);
	lpSprite->Jump (pt.x, pt.y);
	WarpAnn();
	CheckCollision();

	return( YES );
}

//************************************************************************
BOOL CShopScene::CheckMove( LPSPRITE lpSprite, RECT rArea, RECT rSprite )
//************************************************************************
{
	if (!lpSprite)
		return( FALSE );

	PDIB pRoadDib = GetToon()->GetStageDib();
	if (!pRoadDib)
		return( FALSE );

	LPCELL lpCell = lpSprite->GetHeadCell();
	if (!lpCell || !lpCell->pSpriteDib)
		return( FALSE );

	PDIB pAnnDib = lpCell->pSpriteDib;
	HPTR hpAnn = pAnnDib->GetXY (0, 0);
	BYTE Trans = *hpAnn;
	for (int yline = rArea.top ; yline < rArea.bottom ; ++yline)
	{
		HPTR hpRoad = pRoadDib->GetXY (rArea.left, yline);
		hpAnn = pAnnDib->GetXY (rArea.left-rSprite.left, yline-rSprite.top);
		int iCount = rArea.right - rArea.left;
		while (--iCount >= 0)
		{
			if (*hpAnn != Trans && *hpRoad != m_nRoadIndex)
				return( FALSE );
			++hpAnn;
			++hpRoad;		
		}
	}
	return( YES );
}                    

//************************************************************************
void CShopScene::WarpAnn()
//************************************************************************
{
	int i, j;
	POINT pt;
	LPSPRITE lpSprite, lpWarpSprite;

	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return;

	lpSprite = m_lpShopSprite[m_nDirection];
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
void CShopScene::LoadSprites()
//************************************************************************
{
	POINT ptOrigin;
	FNAME szFileName;
	int i;

	ptOrigin.x = ptOrigin.y = 0;
	// Create the shopper sprite
	for (i = 0 ; i < NUM_DIRECTIONS ; ++i)
	{
		if (m_lpShopSprite[i])
		{
			m_pAnimator->DeleteSprite (m_lpShopSprite[i]);
			m_lpShopSprite[i] = NULL;
		}
		if (lstrlen (m_szAnn[i]))
		{
			m_lpShopSprite[i] = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_lpShopSprite[i])
			{
				m_lpShopSprite[i]->SetCellsPerSec (0);
				GetPathName (szFileName, m_szAnn[i]);
				m_lpShopSprite[i]->AddCells (szFileName, m_nAnnCells, NULL, NULL, TRUE);
			}
		}
	}

	LoadItemSprites();

}

//************************************************************************
void CShopScene::LoadItemSprites()
//************************************************************************
{
	POINT ptOrigin;
	FNAME szFileName;
	int i, iSel, iItemX, iItemY;
	int iX = m_ptStartList.x;
	int iY = m_ptStartList.y;

	ptOrigin.x = ptOrigin.y = 0;

	wRandomSeed = GetRandomNumber (m_nItemLoadCount);
	CRandomSequence RandomSequence;
	RandomSequence.Init ((DWORD)m_nItemLoadCount, (DWORD)wRandomSeed);

	// Create the object and list sprites
	for (i = 0 ; i < m_iItemsNeeded ; i++)
	{
		for (int j = 0 ; j < m_nItemLoadCount ; j++)
		{		
			iSel = (int)RandomSequence.GetNextNumber();
			// Find an item that is not used yet
			if (! m_MasterList[iSel].m_fInUse)
            	break;
		}
		m_MasterList[iSel].m_fInUse = TRUE;

		if (m_ObjectList[i].m_lpSprite)
		{
			m_pAnimator->DeleteSprite (m_ObjectList[i].m_lpSprite);
			m_ObjectList[i].m_lpSprite = NULL;
		}
		if (m_MasterList[i].m_szImage[0] != '\0')
		{
			m_ObjectList[i].m_lpSprite = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_ObjectList[i].m_lpSprite)
			{
				GetItemLocation (&iItemX, &iItemY);
				m_ObjectList[i].m_ptLoc.x = iItemX;
				m_ObjectList[i].m_ptLoc.y = iItemY;

				GetPathName (szFileName, m_MasterList[iSel].m_szImage);
				m_ObjectList[i].m_lpSprite->AddCell (szFileName, NULL);
				m_ObjectList[i].m_lpSprite->Jump (m_ObjectList[i].m_ptLoc.x,
													m_ObjectList[i].m_ptLoc.y);
				m_ObjectList[i].m_lpSprite->Show (TRUE);
			}
		}

		if (m_ObjectList[i].m_lpNoCheckSprite)
		{
			m_pAnimator->DeleteSprite (m_ObjectList[i].m_lpNoCheckSprite);
			m_ObjectList[i].m_lpNoCheckSprite = NULL;
		}
		if (m_ObjectList[i].m_lpCheckSprite)
		{
			m_pAnimator->DeleteSprite (m_ObjectList[i].m_lpCheckSprite);
			m_ObjectList[i].m_lpCheckSprite = NULL;
		}

		// Create the not checked list sprite
		if (m_MasterList[iSel].m_szNoCheckImage[0] != '\0')
		{
			m_ObjectList[i].m_lpNoCheckSprite = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_ObjectList[i].m_lpNoCheckSprite)
			{
				GetPathName (szFileName, m_MasterList[iSel].m_szNoCheckImage);
				m_ObjectList[i].m_lpNoCheckSprite->AddCell (szFileName, NULL);
				m_ObjectList[i].m_lpNoCheckSprite->Jump (iX, iY);
				m_ObjectList[i].m_lpNoCheckSprite->Show (TRUE);
			}
		}

		// Create the checked list sprite
		if (m_MasterList[i].m_szCheckImage[0] != '\0')
		{
			m_ObjectList[i].m_lpCheckSprite = m_pAnimator->CreateSprite (&ptOrigin);
			if (m_ObjectList[i].m_lpCheckSprite)
			{
				GetPathName (szFileName, m_MasterList[iSel].m_szCheckImage);
				m_ObjectList[i].m_lpCheckSprite->AddCell (szFileName, NULL);
				m_ObjectList[i].m_lpCheckSprite->Jump (iX, iY);
				m_ObjectList[i].m_lpCheckSprite->Show (FALSE);
			}
		}
		iY += m_iVertSpace;
	}
}

//************************************************************************
void CShopScene::GetItemLocation (int * lpiX, int * lpiY)
//************************************************************************
{
	int iSel;
	wRandomSeed = GetRandomNumber (m_nLocationCount);
	CRandomSequence RandomSequence;
	RandomSequence.Init ((DWORD)m_nLocationCount, (DWORD)wRandomSeed);

	for (int j = 0 ; j < m_nLocationCount ; j++)
	{		
		iSel = (int)RandomSequence.GetNextNumber();
		// Find an item that is not used yet
		if (! m_LocationList[iSel].m_fInUse)
           	break;
	}
	m_LocationList[iSel].m_fInUse = TRUE;
	*lpiX = m_LocationList[iSel].m_ptLoc.x;
	*lpiY = m_LocationList[iSel].m_ptLoc.y;
}

//************************************************************************
void CShopScene::UnloadMazeSprites()
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
void CShopScene::LoadMazeSprites()
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
			pMaze->m_Stop.m_lpSprite->AddCell (szFileName, NULL, 0, 0, TRUE);
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
void CShopScene::SelectItemToSearchFor()
//************************************************************************
{
	if (m_fFindInOrder)
		m_iItemSelected++;
	else
		m_iItemSelected = -1;

}

//************************************************************************
void CShopScene::GetSpritePoint (LPSPRITE lpSprite, LPPOINT ptCenter, LPPOINT ptNew)
//************************************************************************
{
	int iWidth, iHeight;

	lpSprite->GetMaxSize (&iWidth, &iHeight);
	ptNew->x = ptCenter->x - (iWidth/2);
	ptNew->y = ptCenter->y - (iHeight/2);
}

//************************************************************************
BOOL CShopScene::CanGo (LPPOINT lpPoint, int nDirection)
//************************************************************************
{
	int x, y;
	int iWidth, iHeight;
	LPTR lp;
	POINT pt;

	PDIB pRoadDib = GetToon()->GetStageDib();
	if (! pRoadDib)
		return (FALSE);

	x = lpPoint->x;
	y = lpPoint->y;
	if (nDirection == UP)
		y -= m_nScanDistance;
	else if (nDirection == DOWN)
		y += m_nScanDistance;
	else if (nDirection == LEFT)
		x -= m_nScanDistance;
	else
	//if (nDirection == RIGHT)
		x += m_nScanDistance;

	// check point scan distance away from current location
	iWidth = pRoadDib->GetWidth();
	iHeight = abs (pRoadDib->GetHeight());

	if (x < 0 || y < 0 || x >= iWidth || y >= iHeight)
		return (FALSE);

	lp = pRoadDib->GetXY (x, y);
	if (*lp != m_nRoadIndex)
		return (FALSE);
	
	// check to make sure we can find a point in our
	// decision list to move to
	return (GetDecisionPoint (lpPoint, nDirection, &pt) >= 0);
}

//************************************************************************
int CShopScene::GetDecisionPoint (LPPOINT lpCurPoint, int nDirection, LPPOINT lpNewPoint)
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
BOOL CShopScene::CheckCollision()
//************************************************************************
{
	if (m_fStop)
		return (FALSE);
	if (!m_lpShopSprite[m_nDirection])
		return (FALSE);

	FNAME szFileName;
	LPSPRITE lpShopper = m_lpShopSprite[m_nDirection];

	// See if the shopper has caught any items
	for (int i = 0; i < m_iItemsNeeded ; ++i)
	{
		if (m_ObjectList[i].m_lpSprite != NULL && !m_ObjectList[i].m_fFound)
		{
			if (m_pAnimator->CheckCollision (lpShopper, m_ObjectList[i].m_lpSprite))
			{
				// Must find items in order for levels 2 & 3
				if (m_iItemSelected != -1)
				{
				 	if (i != m_iItemSelected)
					{
						// Play the wrong item wave
						if (m_pSound != NULL && m_szWrongItemWave[0] != '\0')
						{
							m_pSound->StopChannel (SUCCESS_CHANNEL);
							m_pSound->StartFile (GetPathName (szFileName, m_szWrongItemWave),
												FALSE, SUCCESS_CHANNEL, FALSE, m_hWnd);
						}
						return FALSE;
					}
				}

				// Play the find item wave
				if (m_pSound != NULL && m_szFindItemWave[0] != '\0')
				{
					m_pSound->StopChannel (SUCCESS_CHANNEL);
					m_pSound->StartFile (GetPathName (szFileName, m_szFindItemWave),
										FALSE, SUCCESS_CHANNEL, FALSE, m_hWnd);
				}
				m_ObjectList[i].m_lpSprite->Show (FALSE);
				m_ObjectList[i].m_fFound = TRUE;
				m_iItemsCaught++;

				// Update the shopping list
				CheckListItem (m_ObjectList[i].m_lpSprite, TRUE);
				SelectItemToSearchFor();

				// See if they are done yet
				if (m_iItemsCaught >= m_iItemsNeeded)
				{
					m_pAnimator->StopAll();
					m_pCurrent->m_fCompleted = TRUE;
					m_fLevelComplete = TRUE;
					PlaySuccess();
				}
			}
		}
	}
	
	return (TRUE);
}

//************************************************************************
void CShopScene::CheckListItem (LPSPRITE lpSprite, BOOL bCheck)
//************************************************************************
{
	for (int i = 0 ; i < m_iItemsNeeded ; i++)
	{
		if (m_ObjectList[i].m_lpSprite == lpSprite)
		{
			if (bCheck)
			{
				m_ObjectList[i].m_lpNoCheckSprite->Show (FALSE);
				m_ObjectList[i].m_lpCheckSprite->Show (TRUE);
			}
			else
			{
				m_ObjectList[i].m_lpCheckSprite->Show (FALSE);
				m_ObjectList[i].m_lpNoCheckSprite->Show (TRUE);
			}
			break;
		}
	}
}

//************************************************************************
void CShopScene::ResetSprites()
//************************************************************************
{
	ResetLevel();
	LoadItemSprites();
}

//************************************************************************
void CShopScene::RemoveStopSign()
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
		Debug ("RemoveStopSign");
		pMaze->m_Stop.m_lpSprite->Show (FALSE);
		pMaze->m_Stop.m_iLocation = -1;
		StartStopSign();
	}
}

//************************************************************************
void CShopScene::StartStopSign()
//************************************************************************
{
	LPMAZE pMaze = m_pCurrent;
	if (! pMaze)
		return;

	DWORD dwRandom = (DWORD)rand();
	DWORD dwRange = pMaze->m_Stop.m_dwMaxFrequency - pMaze->m_Stop.m_dwMinFrequency;
	dwRange = (dwRange * dwRandom) / (DWORD)RAND_MAX;
	pMaze->m_Stop.m_dwNextTime = timeGetTime() + pMaze->m_Stop.m_dwMinFrequency + dwRange;
	Debug ("StartStopSign - dwNextTime = %ld timeGetTime = %ld", pMaze->m_Stop.m_dwNextTime, timeGetTime());
}

//************************************************************************
void CShopScene::DisplayStopSign()
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
		// of the dog or shopper
		while (TRUE)
		{
			LPSPRITE lpSprite;

			i = GetRandomNumber (pMaze->m_nPoints);
			// Don't put stop sign on top of goat
			GetSpritePoint (pMaze->m_Stop.m_lpSprite, &pMaze->m_ptDecision[i], &pt);
			pMaze->m_Stop.m_lpSprite->Jump (pt.x, pt.y);
			lpSprite = m_lpShopSprite[m_nDirection];
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
		Debug("DisplayStopSign - dwNextTime = %ld timeGetTime = %ld", pMaze->m_Stop.m_dwNextTime, timeGetTime());
	}
}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPSHOPSCENE lpScene = (LPSHOPSCENE)dwNotifyData;
		lpScene->OnNotify (lpSprite, Notify);
	}
}

//************************************************************************
void CShopScene::OnNotify (LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (Notify == SN_MOVED)
	{
		RemoveStopSign();
		CheckCollision();
	}
	else if (Notify == SN_MOVEDONE)
	{
//		if (! m_fStop)
//			StartHouse();
	}
}

//************************************************************************
LPMAZE CShopScene::GetNewMaze()
//************************************************************************
{
	// Just one for now
	return (&m_pMazes[0]);
}

//************************************************************************
void CShopScene::SaveObjectBmp (LPSTR lpFilename)
//************************************************************************
{
	if (m_nMasterLoadCount < MAX_SHOP_ITEMS)
		lstrcpy (m_MasterList[m_nMasterLoadCount].m_szImage, lpFilename);
}

//************************************************************************
void CShopScene::SaveItemLoc (int iX, int iY)
//************************************************************************
{
	m_LocationList[m_nLocationCount].m_ptLoc.x = iX;
	m_LocationList[m_nLocationCount].m_ptLoc.y = iY;
	m_nLocationCount++;
}

//************************************************************************
void CShopScene::SaveListBmp (LPSTR lpFilename, int fChecked)
//************************************************************************
{
	if (m_nMasterLoadCount < MAX_SHOP_ITEMS)
	{
		if (fChecked)
		{
			lstrcpy (m_MasterList[m_nMasterLoadCount].m_szCheckImage, lpFilename);
			// This is the last piece of info. loaded, so bump the index/count
			m_nMasterLoadCount++;
			m_nItemLoadCount++;
		}
		else
			lstrcpy (m_MasterList[m_nMasterLoadCount].m_szNoCheckImage, lpFilename);
	}
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
	int i;

	m_szBackground[0] = '\0';		
	m_iShopperStart = 0;
	m_iVision = 1;
	m_iWarpFreq = 1;
	m_iNumWarps = 0;
	m_nPoints = 0;
	for (i = 0 ; i < MAX_DECISION_PTS ; ++i)
		m_ptDecision[i].x = m_ptDecision[i].y = 0;
	m_fCompleted = FALSE;
}

//************************************************************************
CMaze::~CMaze()
//************************************************************************
{
}

//************************************************************************
BOOL CShopParser::GetDecisionPoints (LPMAZE lpMaze, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
BOOL CShopParser::GetWarp (LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
BOOL CShopParser::GetStop (LPMAZE pMaze, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
void CShopParser::SaveItemLoc (LPSHOPSCENE lpScene ,LPSTR lpEntry, LPSTR lpValues, int nValues)
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

	lpScene->SaveItemLoc (iX, iY);
}

//************************************************************************
BOOL CShopParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	FNAME szFilename;
	LPSHOPSCENE lpScene = (LPSHOPSCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "intro"))
			GetFilename(lpScene->m_szIntroWave, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetFilename(lpScene->m_szSoundTrack, lpEntry, lpValues, nValues);
		else
		if (! lstrcmpi (lpKey, "success"))
			GetFilename(lpScene->m_szSuccessWave, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shoppercells"))
			GetInt(&lpScene->m_nAnnCells, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shoppercycle"))
			GetInt(&lpScene->m_nAnnCycle, lpEntry, lpValues, nValues);
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
		if (!lstrcmpi(lpKey, "itemsneeded"))
			GetInt(&lpScene->m_iItemsNeeded, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shoplistX"))
			GetInt(&lpScene->m_ptStartList.x, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shoplistY"))
			GetInt(&lpScene->m_ptStartList.y, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "vertspace"))
			GetInt(&lpScene->m_iVertSpace, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "findinorder"))
			GetInt(&lpScene->m_fFindInOrder, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shopperl"))
			GetFilename(lpScene->m_szAnn[LEFT], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shopperr"))
			GetFilename(lpScene->m_szAnn[RIGHT], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shopperu"))
			GetFilename(lpScene->m_szAnn[UP], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "shopperd"))
			GetFilename(lpScene->m_szAnn[DOWN], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "finditemwave"))
			GetFilename(lpScene->m_szFindItemWave, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "wrongitemwave"))
			GetFilename(lpScene->m_szWrongItemWave, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		LPMAZE lpMaze = lpScene->GetMaze(nIndex-1);
		if (!lstrcmpi(lpKey, "bg"))
			GetFilename(lpMaze->m_szBackground, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "shopperstart"))
			GetInt(&lpMaze->m_iShopperStart, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "warpfreq"))
			GetInt(&lpMaze->m_iWarpFreq, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "vision"))
			GetInt(&lpMaze->m_iVision, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "decision"))
			GetDecisionPoints(lpMaze, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "warp"))
			GetWarp(lpMaze, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "stop"))
			GetStop(lpMaze, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "shopitem"))
		{
			GetFilename (szFilename, lpEntry, lpValues, nValues);
			lpScene->SaveObjectBmp (szFilename);
		}

		if (!lstrcmpi(lpKey, "itemloc"))
			SaveItemLoc (lpScene, lpEntry, lpValues, nValues);

		if (!lstrcmpi(lpKey, "listnocheck"))
		{
			GetFilename (szFilename, lpEntry, lpValues, nValues);
			lpScene->SaveListBmp (szFilename, 0);
		}

		if (!lstrcmpi(lpKey, "listcheck"))
		{
			GetFilename (szFilename, lpEntry, lpValues, nValues);
			lpScene->SaveListBmp (szFilename, 1);
		}

	}
		
	return(TRUE);
}

