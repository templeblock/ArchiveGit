#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "boat.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "painid.h"
#include "parser.h"
#include "boatdef.h"

static BOOL bWaitForNoRepeat;

#define START_CARS

void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

class CDoggerParser : public CParser
{
public:
	CDoggerParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetIntros(LPBOATSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetMultipleFiles(FILE_SPEC FAR *lpFiles, int nMaxFiles, LPINT lpNumFiles,
									LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CBoatScene::CBoatScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_fMovement = NO;
	m_bNoErase = YES;
	m_lpCarSprite = NULL;
	m_iLevel = 0;
	m_dwDeactivateTime = 0;
	m_bReady = FALSE;
	m_bSpritesLoaded = FALSE;
	m_bCarStopped = TRUE;
	m_pSound = NULL;
	for (int i = 0; i < NUM_LEVELS; ++i)
	{
		m_szIntros[i][0] = '\0';
		m_nSuccess[i] = 0;
		for (int j = 0; j < MAX_SUCCESS; ++j)
		{
			m_szSuccess[i][j][0] = '\0';
			if (i == 0)
				m_fSuccessPlayed[j] = FALSE;
		}
	}
}

//************************************************************************
CBoatScene::~CBoatScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;
}

//************************************************************************
BOOL CBoatScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CGBScene::OnInitDialog(hWnd, hWndControl, lParam);
	if (m_pAnimator)
	{
		HGLOBAL hData;
		LPSTR lpTableData;

		// read in the connect point information
		lpTableData = GetResourceData(m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CDoggerParser parser(lpTableData);
			parser.ParseTable((DWORD)this);
		}
		if (hData)
		{
			UnlockResource( hData );
			FreeResource( hData );
		}
	}
	return( TRUE );
}

//************************************************************************
void CBoatScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_BOATI)
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			else
				GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
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
			FNAME szFileName;

			if (m_pSound)
			{
				m_pSound->StopAndFree();
				m_pSound->Activate(FALSE);
			}
			GetPathName(szFileName, CLICK_WAVE);
			CSound sound;
			sound.StartFile(szFileName, FALSE, -1, TRUE);
			if (m_pSound)
				m_pSound->Activate(TRUE);
			ChangeLevel(iLevel);
			break;
		}
		default:
		{
			CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//************************************************************************
void CBoatScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	POINT pt;

	pt.x = x;
	pt.y = y;
	if (!PtInRect(&rGameArea, pt))
		return;
		
	LPSPRITE lpSprite = m_lpCarSprite;
	if (!lpSprite)
		return;
	
	GetSpriteCenterPoint( lpSprite, &pt.x, &pt.y );

	int iDeltaX, iDeltaY;
	iDeltaX = x - pt.x;
	iDeltaY = y - pt.y;

	// If mouse is too close, ignore
	if ((abs(iDeltaX) < 10) && (abs(iDeltaY) < 10))
	{
		m_fMovement = NO;
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

	m_ptMouse.x = x;
	m_ptMouse.y = y;
	OnKey(hWnd, nKey, NO/*fDown*/, NO/*cRepeat*/, 0/*flags*/);
}

//************************************************************************
void CBoatScene::OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CBoatScene::OnMouseMove (HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (!m_bReady)
		return;

	POINT pt;
	pt.x = x;
	pt.y = y;
	if (PtInRect(&GetToon()->m_rGlobalClip, pt))
	{
		m_fMovement = YES;
		m_ptMouse.x = x;
		m_ptMouse.y = y;
	}
}

//************************************************************************
BOOL CBoatScene::GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY)
//************************************************************************
{
	if (! lpSprite)
		return FALSE;

	RECT rLoc;
	lpSprite->Location (&rLoc);
	*lpiX = (rLoc.left + rLoc.right) / 2;
	*lpiY = (rLoc.top + rLoc.bottom) / 2;

	return TRUE;
}

//************************************************************************
BOOL CBoatScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
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
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			// must set real return value for us to take over the cursor
	   		SetDlgMsgResult(hWnd, WM_SETCURSOR, TRUE);
			return(TRUE);
		}
	}
	return(FALSE);
}

//************************************************************************
BOOL CBoatScene::OnMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
		HANDLE_DLGMSG(hWnd, MM_WOM_DONE, OnWomDone);
		HANDLE_DLGMSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_DLGMSG(hWnd, WM_RBUTTONDOWN, OnRButtonDown);
		HANDLE_DLGMSG(hWnd, WM_SETCURSOR, OnSetCursor);

		default:
			return CGBScene::OnMessage(hWnd, msg, wParam, lParam);
	}
}

//************************************************************************
void CBoatScene::OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	if (m_bReady)
	{
	 	if (m_nSceneNo == IDD_BOATI)
	 		FORWARD_WM_COMMAND(m_hWnd, IDC_GOTOSCENE, NULL, m_nNextSceneNo, PostMessage);
		else
		{
			if ( m_lpCarSprite )
				m_lpCarSprite->Jump( CAR_X, CAR_Y );
			m_bCarStopped = FALSE;
			PlaySoundTrack();
		}
	}
	else
		StartGame();
}

//************************************************************************
void CBoatScene::ToonInitDone()
//************************************************************************
{
	if (m_pAnimator)
	{
		FNAME szFileName;
		POINT ptOrigin;

		if (GetToon())
			GetToon()->SetSendMouseMode(TRUE);

		App.SetKeyMapEntry(m_hWnd, VK_UP);
		App.SetKeyMapEntry(m_hWnd, VK_DOWN);
		App.SetKeyMapEntry(m_hWnd, VK_RIGHT);
		App.SetKeyMapEntry(m_hWnd, VK_LEFT);

		// call this to make sure rand() is seeded
		GetRandomNumber(100);
		m_pAnimator->StopAll();
		m_pAnimator->SetClipRect(&rGameArea);
		LoadMotorPool(L2RMotorPool, sizeof(L2RMotorPool) / sizeof(MOTORPOOL));
		LoadMotorPool(R2LMotorPool, sizeof(R2LMotorPool) / sizeof(MOTORPOOL));      
		
		LoadStills(Stills, sizeof(Stills) / sizeof(STILLSPRITE));   
		
		ptOrigin.x = 0;
		ptOrigin.y = 0; 
		GetPathName(szFileName, DOGGER_CAR);
		m_lpCarSprite = m_pAnimator->CreateSprite( &ptOrigin );
		if ( m_lpCarSprite )
		{
			m_lpCarSprite->AddCell( szFileName, NULL );
			m_lpCarSprite->SetNotifyProc(OnSpriteNotify, (DWORD)this);
		}
		InitDoggerCar();
		m_pSound = new CSound(TRUE);
		if (m_pSound)
			m_pSound->Open( NULL/*"powersnd.dll"*/ );

		PlayIntro();
		m_bSpritesLoaded = TRUE;
		#ifdef START_CARS
			InitLanes();
			m_pAnimator->StartAll();
		#endif
		
		ShowStills(TRUE);
	}
}

//************************************************************************
void CBoatScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

	if (GetToon())
		GetToon()->SetHintState(FALSE);
	m_bReady = FALSE;
	if (m_pSound)
	{
		m_pSound->StopChannel(TRAFFIC_CHANNEL);
		m_pSound->StartFile( GetPathName(szFileName, m_szIntros[m_iLevel]), NO/*bLoop*/, INTRO_CHANNEL/*iChannel*/, TRUE, m_hWnd );
	}
}

//************************************************************************
void CBoatScene::PlaySoundTrack()
//************************************************************************
{
	FNAME szFileName;

	if (m_pSound)
	{
		m_pSound->StopChannel(TRAFFIC_CHANNEL);
		m_pSound->StartFile( GetPathName(szFileName, TRAFFIC_WAVE), YES/*bLoop*/, TRAFFIC_CHANNEL/*iChannel*/, FALSE );
	}
}

//************************************************************************
void CBoatScene::StartGame()
//************************************************************************
{
	#ifndef START_CARS
		InitLanes();
		if (m_pAnimator)
			m_pAnimator->StartAll();
	#endif
	PlaySoundTrack();
	m_bReady = TRUE;
	m_bCarStopped = FALSE;
	if (GetToon())
		GetToon()->SetHintState(TRUE);
}

//************************************************************************
BOOL CBoatScene::LoadMotorPool(LPMOTORPOOL lpMotorPool, int iNumCars)
//************************************************************************
{
	if ( !m_pAnimator )
		return( FALSE );

	int i;
	POINT ptOrigin;
	LPSPRITE lpSprite;
	FNAME szFileName;

	ptOrigin.x = 0;
	ptOrigin.y = 0;
	for (i = 0; i < iNumCars; ++i)
	{
		lpMotorPool[i].fCarInUse = FALSE;
		lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
		if ( lpSprite )
		{
			GetPathName(szFileName, lpMotorPool[i].lpCarName);
	 		lpSprite->AddCells(szFileName, lpMotorPool[i].nCells, NULL, NULL);
			lpSprite->SetCellsPerSec( 8 );
			lpSprite->SetNotifyProc(OnSpriteNotify, (DWORD)this);
		}
		lpMotorPool[i].lpSprite = lpSprite;
	}
	return(TRUE);
}

//************************************************************************
void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPBOATSCENE lpScene = (LPBOATSCENE)dwNotifyData;
		if ( lpScene )
			lpScene->OnNotify(lpSprite, Notify);
	}
}

//************************************************************************
void CBoatScene::OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	RECT rCar, rMoving;
	BOOL fCrash = FALSE;
	FNAME szFileName;

	if (!lpSprite)
		return;
	if (!m_lpCarSprite)
		return;
	if (!m_pAnimator)
		return;
	if (Notify == SN_MOVED)
	{
		m_lpCarSprite->Location(&rCar);
		// if it is the main car moving we need to see whether it
		// ran into any of the other cars
		if (lpSprite == m_lpCarSprite)
		{
			lpSprite = m_pAnimator->GetHeadSprite();
			while (lpSprite)
			{
				if (lpSprite != m_lpCarSprite)
				{
					if (fCrash = m_pAnimator->CheckCollision(m_lpCarSprite, lpSprite))
						break;
				}
				lpSprite = lpSprite->GetNextSprite();
			}
		}
		// if it is an obstacle car moving we only need to see if
		// the obstacle car ran into the main car
		else
		{
			lpSprite->Location(&rMoving);
			fCrash = m_pAnimator->CheckCollision(m_lpCarSprite, lpSprite);
		}

		if (fCrash)
		{
			if (m_pSound)
				m_pSound->StartFile( GetPathName(szFileName, CRASH_WAVE), NO/*bLoop*/, CRASH_CHANNEL/*iChannel*/, FALSE );

			m_lpCarSprite->Jump( CAR_X, CAR_Y );
			bWaitForNoRepeat = TRUE;
		}
	}
	else
	if (Notify == SN_MOVEDONE)
	{
		if (!ResetCarInUse(lpSprite, L2RMotorPool, sizeof(L2RMotorPool)/sizeof(MOTORPOOL)))
			ResetCarInUse(lpSprite, R2LMotorPool, sizeof(R2LMotorPool)/sizeof(MOTORPOOL));
	}     
	else
	if (Notify == SN_PAUSEAFTER)
	{
		int i;     
		int iNumStills = sizeof(Stills) / sizeof(STILLSPRITE);
		for (i = 0; i < iNumStills; ++i)
		{              
			if (lpSprite == Stills[i].lpSprite)
			{
				lpSprite->PauseAfterCell( Stills[i].nCells-1, Stills[i].nDelay );
				lpSprite->SetCurrentCell(0);
			}
		}
	}
}

//************************************************************************
BOOL CBoatScene::ResetCarInUse(LPSPRITE lpSprite, LPMOTORPOOL lpMotorPool, int iNumCars)
//************************************************************************
{
	int i;

	for (i = 0; i < iNumCars; ++i)
	{
		if (lpMotorPool[i].lpSprite == lpSprite)
		{
			lpMotorPool[i].fCarInUse = FALSE;
			return(TRUE);
		}
	}
	return(FALSE);
}

//************************************************************************
void CBoatScene::OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	if ( (flags & KF_REPEAT) && bWaitForNoRepeat )
		return;

	bWaitForNoRepeat = FALSE;

	switch (vk)
	{
		case VK_UP:
			MoveCar(0, -CAR_MOVE_YINC);
			break;
		case VK_DOWN:
			MoveCar(0, CAR_MOVE_YINC);
			break;
		case VK_RIGHT:
			MoveCar(CAR_MOVE_XINC, 0);
			break;
		case VK_LEFT:
			MoveCar(-CAR_MOVE_XINC, 0);
			break;
		default:
			CGBScene::OnKey(hWnd, vk, fDown, cRepeat, flags);
			break;
	}

	if ( fDown ) // Means its a real keydown, not one generated by the timer
	{
		LPSPRITE lpSprite = m_lpCarSprite;
		if (lpSprite)
		{
			int x, y;
			GetSpriteCenterPoint(lpSprite, &x, &y);
			m_fMovement = NO;
			m_ptMouse.x = x;
			m_ptMouse.y = y;
		}
	}
}

//************************************************************************
LPSTR GetRandomFile(FILE_SPEC FAR *lpFileName, int nFiles, BOOL FAR *fFileUsed, LPSTR lpReturnFile)
//************************************************************************
{
	int i, iFile, iFileFind;
	int iCount = 0;

	// count the number of files not used
	for (i = 0; i < nFiles; ++i)
	{
		if (!fFileUsed[i])
			++iCount;	
	}
	if (!iCount)
	{
		// reset all files to not used
		for (i = 0; i < nFiles; ++i)
			fFileUsed[i] = FALSE;
		iCount = nFiles;
	}
	
	iFile = GetCycleNumber(iCount);
	iFileFind = 0;
	for (i = 0; i < nFiles; ++i)
	{
		// only look at ones not used
		if (fFileUsed[i])
			continue;
		// see if we found the one
		if (iFileFind == iFile)
			break;
		// look for next one
		++iFileFind;
	}
	lstrcpy(lpReturnFile, lpFileName[i]);
	// if there was only one opening, reset the used table
	if (iCount == 1)
	{
		// reset all files to not used
		for (int j = 0; j < nFiles; ++j)
			fFileUsed[j] = FALSE;
	}
	fFileUsed[i] = TRUE;

	return(lpReturnFile);
}

//************************************************************************
BOOL CBoatScene::MoveCar(int dx, int dy)
//************************************************************************
{
	RECT rLoc, rSect;
	int inc, sign, iWidth, iHeight;
	FNAME szFileName;

	// see if we are not ready yet, but user is!
	if (!m_bReady)
	{
		if (m_pSound)
			m_pSound->StopChannel(INTRO_CHANNEL);
		StartGame();
		return(FALSE);
	}

	if (m_bCarStopped)
		return(FALSE);
	if (!m_lpCarSprite)
		return(FALSE);

	GetDisplaySize(&iWidth, &iHeight);

	if (dx)
	{
		inc = abs(dx);
		sign = dx < 0 ? -1 : 1;
	}
	else
	{
		inc = abs(dy);
		sign = dy < 0 ? -1 : 1;
	}

	BOOL fBump = FALSE;
	for (; inc >= 1; --inc)
	{
		m_lpCarSprite->Location(&rLoc);
		if (dx)
			OffsetRect(&rLoc, inc*sign, 0);
		else
			OffsetRect(&rLoc, 0, inc*sign);

		// see if we are trying to go off the screen
		RECT r = rLoc;
		IntersectRect( &r, &rLoc, &rGameArea );
		if (!EqualRect(&r, &rLoc))
			continue;
        else
        	break;
        	
		// next check restricted zones
		/*for (i = 0; i < NUM_NOZONES; ++i)
		{
			if (IntersectRect(&rSect, &rLoc, &NoZones[m_iLevel][i]))
				break;
		}
		if (i == NUM_NOZONES) */
		//	break;              
				
		/*else
		{
			fBump = TRUE;
			inc = 0;
			break;
		}*/
		
	}

	// if we can't move, get out
	if (inc <= 0)
		return(FALSE);

	// If we are moving up to the edge
	if ( dx && (rLoc.left == rGameArea.left || rLoc.right  == rGameArea.right) )
    	fBump = TRUE;
	if ( dy && (rLoc.top  == rGameArea.top  || rLoc.bottom == rGameArea.bottom) )
    	fBump = TRUE;
	
	if ( fBump )
	{
		if (m_pSound)
			m_pSound->StartFile( GetPathName(szFileName, BUMP_WAVE), NO/*bLoop*/, BUMP_CHANNEL/*iChannel*/, FALSE );
	}

	m_lpCarSprite->Jump(rLoc.left, rLoc.top);

	// next check win zone
	if (IntersectRect(&rSect, &rLoc, &WinZones[m_iLevel]))
	{
		if (EqualRect(&rSect, &rLoc))
		{
			m_bCarStopped = TRUE;
			if (m_pSound)
  			{
				FILE_SPEC szSuccess;

				m_pSound->StopAndFree();
						
				GetRandomFile(m_szSuccess[m_iLevel], m_nSuccess[m_iLevel],
								m_fSuccessPlayed, szSuccess);

				FNAME szFileName;
				if (m_pSound)
				{
					m_pSound->StopChannel(SUCCESS_CHANNEL);
					if (m_nSceneNo == IDD_BOATI)
					{ // Play this wave without wavemix
						LPSOUND lpSound = new CSound;
						if ( lpSound )
						{
							m_pSound->Activate (FALSE);
							lpSound->StartFile( GetPathName(szFileName, szSuccess),
								NO/*bLoop*/, NORMAL_CHANNEL/*iChannel*/, TRUE/*bWait*/, m_hWnd);
							delete lpSound;
							GetSound()->Activate (TRUE);
							FORWARD_WM_COMMAND (m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
						}
					}
					else
					{
						m_pSound->StartFile(GetPathName (szFileName, szSuccess), NO/*bLoop*/, SUCCESS_CHANNEL/*iChannel*/, TRUE/*bWait*/, m_hWnd);
						//StartGame();
					}
				}
			}
		}
	}

	return(TRUE);
}

//************************************************************************
void CBoatScene::OnTimer(HWND hWnd, UINT id)
//************************************************************************
{
	int i, j, nCar, nCars, nNum, iWidth, iHeight, iLaneSize, x, y, iNumCars;
	DWORD dwRandom;
	LPSPRITE lpSprite;
	BOOL fLeftToRight;
	LPMOTORPOOL lpMotorPool;
    
	CGBScene::OnTimer(hWnd, id);
	if (id != m_idAnimatorTimer)
		return;
	#ifndef START_CARS
	if (!m_bReady)
		return;
	#endif
	if (!m_bSpritesLoaded)
		return;

	// check each lane to see if it's time to start a car
	for (i = 0; i < NUM_LANES; ++i)
	{          
		// Make sure we want to mess with this lane
		if ( !Lanes[i].fUse[m_iLevel] )
			continue;
			
		// see if it's time to start a car
		if (IsStartTime(i))
		{
			// figure out which motor pool to use
			if (Lanes[i].Direction == RANDOM)
			{
				dwRandom = (DWORD)rand();
				fLeftToRight = dwRandom > (RAND_MAX/2);
			}
			else
				fLeftToRight = Lanes[i].Direction == LEFT_TO_RIGHT;
			if (fLeftToRight)
			{
				lpMotorPool = L2RMotorPool;
				iNumCars = sizeof(L2RMotorPool) / sizeof(MOTORPOOL);
			}
			else
			{
				lpMotorPool = R2LMotorPool;
				iNumCars = sizeof(R2LMotorPool) / sizeof(MOTORPOOL);
			}

			// count number of cars that are not in use
			nCars = 0;
			for (j = 0; j < iNumCars; ++j)
			{
				if (!lpMotorPool[j].fCarInUse)
					++nCars;
			}
			// any cars available?
			if (nCars)
			{
				// pick car based on random number
				dwRandom = (DWORD)rand();
				nCar = (int)((dwRandom * ((DWORD)nCars)) / ((DWORD)RAND_MAX+1));
				if (nCar >= nCars)
					nCar = nCars - 1;
				nNum = 0;
				// pick nCar based on available cars
				for (j = 0; j < iNumCars; ++j)
				{
					if (!lpMotorPool[j].fCarInUse)
					{
						if (nNum == nCar)
							break;
						++nNum;
					}
				}
				// Indicate that this car is now in use
				lpMotorPool[j].fCarInUse = TRUE;
				// Indicate that we are no longer waiting for a car
				Lanes[i].fWaitingForCar = FALSE;

				// Center car in lane
				lpSprite = lpMotorPool[j].lpSprite;
				if ( !lpSprite )
					continue;
				lpSprite->GetMaxSize(&iWidth, &iHeight);
				iLaneSize = Lanes[i].iBottom - Lanes[i].iTop;
				y = Lanes[i].iTop + ((iLaneSize - iHeight) / 2);
				if (fLeftToRight)
					x = rGameArea.left - iWidth - 1;
				else  
				    x = rGameArea.right + 1;
				lpSprite->Jump(x, y);

				// show the car and move it to the end of the lane
				lpSprite->AddCmdSetSpeed(Lanes[i].iSpeed[m_iLevel]);
				lpSprite->AddCmdShow();
				if (fLeftToRight) 
					lpSprite->AddCmdMove(rGameArea.right + 2, y);
				else   
					lpSprite->AddCmdMove(rGameArea.left - iWidth - 2, y);
				lpSprite->AddCmdHide();
				// setup next start time
				SetNextStart(i);
			}
			else
				Lanes[i].fWaitingForCar = TRUE;
		}
	}

	// Move the boat
	if (m_bReady && m_fMovement)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		OnLButtonDown(hWnd, NO/*fDoubleClick*/, pt.x, pt.y, 0/*keyFlags*/);
	}
}

//************************************************************************
BOOL CBoatScene::IsStartTime(int i)
//************************************************************************
{
	if (!Lanes[i].iSpeed[m_iLevel])
		return(FALSE);

	if (Lanes[i].fWaitingForCar)
		return(TRUE);

	DWORD dwCurrentTime = timeGetTime();
	DWORD dwTime = dwCurrentTime - Lanes[i].dwWaitStart;
	return (dwTime >= Lanes[i].dwWaitTime);
}

//************************************************************************
void CBoatScene::OnAppActivate(BOOL fActivate)
//************************************************************************
{
	CGBScene::OnAppActivate(fActivate);
	if (fActivate)
	{
		if (m_bReady)
		{
			if (m_dwDeactivateTime)
			{
				DWORD dwTimeDelta = timeGetTime() - m_dwDeactivateTime;
				for (int i = 0; i < NUM_LANES; ++i)
					Lanes[i].dwWaitStart += dwTimeDelta;
				if (m_pAnimator)
					m_pAnimator->AdjustTime(dwTimeDelta);
			}
			PlaySoundTrack();
		}
		else
		{
			PlayIntro();
		}
	}
	else
	{
		m_dwDeactivateTime = timeGetTime();
		if (m_pSound)
			m_pSound->StopAndFree();
	}
}

//************************************************************************
BOOL CBoatScene::ChangeLevel(int iLevel)
//************************************************************************
{
	if (!m_pAnimator)
		return(FALSE);
	m_iLevel = iLevel;
    
    // Hide any still animated sprites
	ShowStills(FALSE);
	
	// turn off sound
	if (m_pSound)
	{
		m_pSound->StopChannel(INTRO_CHANNEL);
		m_pSound->StopChannel(TRAFFIC_CHANNEL);
	}
	if (m_pAnimator)
	{
		m_pAnimator->StopAll();
		HideAllCars(L2RMotorPool, sizeof(L2RMotorPool) / sizeof(MOTORPOOL));
		HideAllCars(R2LMotorPool, sizeof(R2LMotorPool) / sizeof(MOTORPOOL));
		if (m_lpCarSprite)
			m_lpCarSprite->Show(FALSE);
		InitDoggerCar();
	}

	if (m_lpCarSprite)
		m_lpCarSprite->Show(TRUE);

	m_bReady = FALSE;
	m_bCarStopped = TRUE;
	if (GetToon())
		GetToon()->SetHintState(TRUE);
	PlayIntro();
	#ifdef START_CARS
		InitLanes();
		m_pAnimator->StartAll();
	#endif
                                      
	// Show still animations                                      
	ShowStills(TRUE);
	                  
	return(TRUE);
}

//************************************************************************
void CBoatScene::HideAllCars(LPMOTORPOOL lpMotorPool, int iNumCars)
//************************************************************************
{
	for (int i = 0; i < iNumCars; ++i)
	{
		LPSPRITE lpSprite = lpMotorPool[i].lpSprite;
		if (lpSprite)
			lpSprite->Show(FALSE);
	}
}

//************************************************************************
void CBoatScene::InitLanes()
//************************************************************************
{
	for (int i = 0; i < NUM_LANES; ++i)
	{
		Lanes[i].dwWaitStart = 0;
		Lanes[i].dwWaitTime = 0;
		Lanes[i].fWaitingForCar = FALSE;
		SetNextStart(i, TRUE);
	}

}

//************************************************************************
void CBoatScene::InitDoggerCar(void)
//************************************************************************
{
	if (!m_lpCarSprite)
		return;
	m_lpCarSprite->Jump( CAR_X, CAR_Y );
	m_lpCarSprite->Show(TRUE);
}

//************************************************************************
void CBoatScene::SetNextStart(int i, BOOL fInit)
//************************************************************************
{
	if (Lanes[i].iSpeed[m_iLevel])
	{
		DWORD dwRandom = (DWORD)rand();
		DWORD dwRange = Lanes[i].dwMaximumTime[m_iLevel] - Lanes[i].dwMinimumTime[m_iLevel];
		dwRange = (dwRange * dwRandom) / (DWORD)RAND_MAX;
		if (fInit)
			dwRange /= 8;
		Lanes[i].dwWaitTime = Lanes[i].dwMinimumTime[m_iLevel] + dwRange;
		Lanes[i].dwWaitStart = timeGetTime();
	}
}

//************************************************************************
BOOL CBoatScene::LoadStills(LPSTILLSPRITE lpStills, int iNumStills)
//************************************************************************
{
	if ( !m_pAnimator )
		return( FALSE );

	int i;
	POINT ptOrigin;
	LPSPRITE lpSprite;
	FNAME szFileName;

	ptOrigin.x = 0;
	ptOrigin.y = 0;       
	
	for (i = 0; i < iNumStills; ++i)
	{
		lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
		if ( lpSprite )
		{
			GetPathName(szFileName, lpStills[i].lpName);
	 		lpSprite->AddCells(szFileName, lpStills[i].nCells, NULL, NULL);
			lpSprite->SetCellsPerSec( 8 );     
			lpSprite->PauseAfterCell( lpStills[i].nCells-1, Stills[i].nDelay );
			lpSprite->SetNotifyProc(OnSpriteNotify, (DWORD)this);
		}
		lpStills[i].lpSprite = lpSprite;
	}
	return(TRUE);
}

//************************************************************************
void CBoatScene::ShowStills(BOOL bShow)
//************************************************************************
{                 
	int i;
	int nNumStills = sizeof(Stills) / sizeof(STILLSPRITE);
	LPSPRITE lpSprite;
	    
	for( i=0; i<nNumStills; i++)
	{
		lpSprite = Stills[i].lpSprite;
		                           
		int iTemp = 1 << m_iLevel;
				                           
		// Make sure this still sprite is to be shown for this level
		if (bShow && (Stills[i].fLevels & (1 << m_iLevel)))
		{
			lpSprite->Jump(Stills[i].x, Stills[i].y);
			lpSprite->Show(bShow);
		}                         
		
		// Make sure the sprite is hidden regardless of level
		else if (!bShow)
			lpSprite->Show(bShow);
	}
}    
   
//************************************************************************
BOOL CDoggerParser::GetIntros(LPBOATSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	if (!lpScene)
		return(FALSE);

	LPSTR lpValue;

	if (nValues != NUM_LEVELS)
	{
		Print("'%s'\n Invalid intros key '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	for (int i = 0; i < nValues; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		lstrcpy(lpScene->m_szIntros[i], lpValue);
	}
 	return(TRUE);
}

//************************************************************************
BOOL CDoggerParser::GetMultipleFiles(FILE_SPEC FAR *lpFiles, int nMaxFiles, LPINT lpNumFiles,
									LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	if (nValues > MAX_SUCCESS)
	{
			Print("'%s'\n Invalid success key '%s'", lpEntry, lpValues);
			return(FALSE);
	}

	for (int i = 0; i < nValues; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		lstrcpy(lpFiles[i], lpValue);
		++*lpNumFiles;
	}
	return(TRUE);
}

//************************************************************************
BOOL CDoggerParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPBOATSCENE lpScene = (LPBOATSCENE)dwUserData;
	if (!lpScene)
		return(FALSE);

	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "intros"))
			GetIntros(lpScene, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success1"))
			GetMultipleFiles(lpScene->m_szSuccess[0], MAX_SUCCESS, &lpScene->m_nSuccess[0], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success2"))
			GetMultipleFiles(lpScene->m_szSuccess[1], MAX_SUCCESS, &lpScene->m_nSuccess[1], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success3"))
			GetMultipleFiles(lpScene->m_szSuccess[2], MAX_SUCCESS, &lpScene->m_nSuccess[2], lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

