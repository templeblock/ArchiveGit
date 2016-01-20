#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "connect.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "conndef.h"
#include "sailorid.h"

class CConnectParser : public CParser
{
public:
	CConnectParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetPoints(LPCONSTELLATION lpConstellation, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetFlags(LPCONSTELLATION pConstellation, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetTransition(LPCONNECTTRANSITION pTransition, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetWaves(LPCONSTELLATION pConstellation, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetGameFlags(LPCONNECTSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetIntros(LPCONNECTSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSuccess(LPCONNECTSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CConnectScene::CConnectScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_iLevel = 0;
	m_nConstellations = 0;
	m_nCurrent = -1;
	m_pConstellations = NULL;
	m_nCompleted = 0;
	m_nColorIndex = 0;
	m_nLineThickness = 2;
	m_nCloseness = 5;
	SetRectEmpty(&m_rTransitionClip);
	m_ptStart.x = m_ptStart.y = 0;
	m_ptEnd.x = m_ptEnd.y = 0;
	m_nStarCells = 6;
	m_lpStarSprite = NULL;
	m_fCapture = FALSE;
	m_fStarOn = FALSE;
	m_hHotSpotCursor = LoadCursor(GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));
	m_dwDelay = 2000;
	m_bUseLast = FALSE;
	m_lpGuideSprite = NULL;
	for (int i = 0; i < NUM_LEVELS; ++i)
	{
		m_szIntros[i][0] = '\0';
		m_szSuccess[i][0] = '\0';
	}
	m_szSoundTrack[0] = '\0';
	m_szClick[0] = '\0';
	m_pSound = NULL;
	SetRectEmpty(&m_rGameArea);
	m_fIntroPlaying = FALSE;
}

//************************************************************************
CConnectScene::~CConnectScene()
//************************************************************************
{
	if (m_pConstellations)
		delete [] m_pConstellations;
	if (m_pSound)
		delete m_pSound;
}

//************************************************************************
int CConnectScene::GetLevel()
//************************************************************************
{
	return(m_iLevel);
}

//************************************************************************
int CConnectScene::GetNumConstellations()
//************************************************************************
{
	int iCount = 0;
	int iLevel = GetLevel();
	for (int i = 0; i < m_nConstellations; ++i)
	{
		if (m_pConstellations[i].m_iLevel == iLevel)
			++iCount;
	}	
	return(iCount);
}

//************************************************************************
LPCONSTELLATION CConnectScene::GetNextConstellation()
//************************************************************************
{
	int i;
	int iCount = 0;
	int iRandom;

	// count how many uncompleted constellations 
	// there are for this level
	for (i = 0; i < m_nConstellations; ++i)
	{
		if (!m_pConstellations[i].m_fCompleted &&
			(m_pConstellations[i].m_iLevel == m_iLevel))
		{
			++iCount;
		}
	}

	// get random number 0 to iCount-1
	iRandom = GetCycleNumber(iCount);

	// find this constellation
	iCount = 0;
	for (i = 0; i < m_nConstellations; ++i)
	{
		if (!m_pConstellations[i].m_fCompleted &&
			(m_pConstellations[i].m_iLevel == m_iLevel))
		{
			if (iCount == iRandom)
			{
				m_nCurrent = i;
				return(&m_pConstellations[m_nCurrent]);
			}
			++iCount;
		}
	}
	return(NULL);
}

//************************************************************************
BOOL CConnectScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CGBScene::OnInitDialog(hWnd, hWndControl, lParam);
	if (m_pAnimator)
	{
		HGLOBAL hData;
		LPSTR lpTableData;

		if (GetToon())
			GetToon()->SetSendMouseMode(TRUE);

		// read in the connect point information
		lpTableData = GetResourceData(m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CConnectParser parser(lpTableData);
			if (m_nConstellations = parser.GetNumEntries())
			{
				--m_nConstellations;
				m_pConstellations = new CConstellation[m_nConstellations];
				if (m_pConstellations)
				{
					parser.ParseTable((DWORD)this);
					for (int i = 0; i < m_nConstellations; ++i)
						m_pConstellations[i].AdjustPoints();
				}
			}
		}
		if (hData)
		{
			UnlockResource( hData );
			FreeResource( hData );
		}
		// setup first constellation to work on
		LPCONSTELLATION pConstellation = GetNextConstellation();
		if (pConstellation)
		{
			if (GetToon())
				GetToon()->SetBackground(pConstellation->m_szBackground1);
		}
	}
	return( TRUE );
}

//************************************************************************
void CConnectScene::ToonInitDone()
//************************************************************************
{
	POINT ptOrigin;
	FNAME szFileName;

	ptOrigin.x = ptOrigin.y = 0;
	m_lpStarSprite = m_pAnimator->CreateSprite( &ptOrigin );
	if (m_lpStarSprite)
	{
		m_lpStarSprite->SetCellsPerSec(8);
		GetPathName(szFileName, STAR_NAME);
		m_lpStarSprite->AddCells( szFileName, m_nStarCells, NULL, NULL, TRUE );
	}
	if (!IsRectEmpty(&m_rGameArea))
		m_pAnimator->SetClipRect(&m_rGameArea);
	ShowGuideSprite(TRUE);
	ShowStarSprite(TRUE);
	PlayIntro();
}

//************************************************************************
void CConnectScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_CONNECTI)
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
		 	if (iLevel != GetLevel())
			{
				CSound sound;
				FNAME szFileName;

				if (m_pSound)
				{
					m_pSound->StopAndFree();
					m_pSound->Activate(FALSE);
				}
				else
				if (GetSound())
					GetSound()->StopAndFree();
				GetPathName(szFileName, CLICK_WAVE);
				sound.StartFile(szFileName, FALSE, -1, TRUE);
				if (m_pSound)
					m_pSound->Activate(TRUE);
				ChangeLevel(iLevel);
			}
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
BOOL CConnectScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, WM_SETCURSOR, OnSetCursor);
		//HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);
		HANDLE_DLGMSG(hDlg, MM_WOM_DONE, OnWomDone);

		default:
			return(FALSE);
	}
}

//************************************************************************
LOCAL void CALLBACK ToonDrawCallback(PTOON pToon, DWORD dwData)
//************************************************************************
{
	LPCONNECTSCENE lpScene = (LPCONNECTSCENE)dwData;
	lpScene->ShowGuideSprite(TRUE);
}

//************************************************************************
void CConnectScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	int i, dx, dy, sx, sy;
	LPCONSTELLATION pConstellation;
	BOOL fNewLevel = FALSE;

	if (m_fIntroPlaying)
	{
		
		if (m_pSound)
			m_pSound->StopChannel(INTRO_CHANNEL);
		m_fIntroPlaying = FALSE;
		PlaySoundTrack();
	}

	SetCapture(hWnd);
	m_fCapture = TRUE;
	pConstellation = GetConstellation(m_nCurrent);
	if ((i = pConstellation->FindPoint(x, y, m_nCloseness, m_bUseLast)) < 0)
	{
		// child clicked in the wrong spot
	}
	else
	{
		PlaySoundEffect(m_szClick);
		ShowStarSprite(FALSE);
		dx = pConstellation->m_pts[i].x;
		dy = pConstellation->m_pts[i].y;
		if (i)
		{
			sx = pConstellation->m_pts[i-1].x;
			sy = pConstellation->m_pts[i-1].y;
		}
		else
		{
			sx = dx;
			sy = dy;
		}
		pConstellation->m_fConnected[i] = TRUE;
		DrawLine(sx, sy, dx, dy);
		if (IsSolved() && !m_bUseLast)
		{
			if (pConstellation->m_pts[i+1].x || pConstellation->m_pts[i+1].y)
				DrawLine(dx, dy, pConstellation->m_pts[i+1].x, pConstellation->m_pts[i+1].y);
		}
		else
			ShowStarSprite(TRUE);
	}
	if (IsSolved())
	{
		FNAME szFileName;
		int iWave;

		// tell the kid how great they are and move onto the next puzzle
		//MSG msg;
		//while (GetMessage(&msg, NULL, 0, 0))
		//{
		//	if (ESCAPE)
		//		break;
		//
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}

		if (m_pSound)
		{
			delete m_pSound;
			m_pSound = NULL;
		}
		
		if (GetSound())
		{
			if (lstrlen(pConstellation->m_szNoiseFile))
				GetSound()->StartFile( GetPathName(szFileName, pConstellation->m_szNoiseFile), NO/*bLoop*/, -1/*iChannel*/, TRUE );
			iWave = GetRandomNumber(pConstellation->m_nWaves);
			GetSound()->StartFile( GetPathName(szFileName, pConstellation->m_szWaveFile[iWave]), NO/*bLoop*/, -1/*iChannel*/, FALSE );
		}
		GetToon()->LoadBackground(	pConstellation->m_szBackground2,
									m_Trans1.m_uTransition, m_Trans1.m_nTicks, 
									m_Trans1.m_nStepSize, m_Trans1.m_dwTotalTime,
									&m_rTransitionClip,
									ToonDrawCallback, (DWORD)this);
		++m_nCompleted;

		// move the boat
		MoveBoat();

		Delay(m_dwDelay);

		pConstellation = GetNextConstellation();
		if (m_nSceneNo == IDD_CONNECTI && (pConstellation == NULL))
		{
			if (GetSound())
				GetSound()->StartFile( GetPathName(szFileName, m_szSuccess[GetLevel()]), NO/*bLoop*/, -1/*iChannel*/, TRUE );
			App.GotoScene(hWnd, m_nNextSceneNo);
		}
		else
		if (pConstellation)
		{
			GetToon()->LoadBackground(	pConstellation->m_szBackground1,
										m_Trans2.m_uTransition,
										m_Trans2.m_nTicks,
										m_Trans2.m_nStepSize,
										m_Trans2.m_dwTotalTime, &m_rTransitionClip,
										ToonDrawCallback, (DWORD)this);
			ShowStarSprite(TRUE);
			PlaySoundTrack();
		}
		else // level change?
		{
			if (GetSound())
				GetSound()->StartFile( GetPathName(szFileName, m_szSuccess[GetLevel()]), NO/*bLoop*/, -1/*iChannel*/, TRUE );

			// Fix not getting clicks when the activity starts over in the same level (SMS).
			if (!m_pSound)
			{
				m_pSound = new CSound(TRUE);
				if (m_pSound)
					m_pSound->Open(NULL/*"powersnd.dll"*/);
			}

			ChangeLevel(GetLevel(), FALSE);
			//if (GetLevel() < (NUM_LEVELS-1))
			//{
			//	// bumping to next level, play some intro
			//	if (GetToon())
			//	{
			//		STRING szForegroundDib;
			//
			//		int iLevel = GetLevel()+1;
			//		if (iLevel == 1)
			//			lstrcpy(szForegroundDib, "lvl2off.bmp");
			//		else
			//			lstrcpy(szForegroundDib, "lvl3off.bmp");
			//		GetToon()->OnHotSpot(szForegroundDib);
			//	}
			//}
		}
	}
}

//************************************************************************
void CConnectScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fCapture)
	{
		ReleaseCapture();
		m_fCapture = FALSE;
	}
}

//************************************************************************
void CConnectScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
BOOL CConnectScene::OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	POINT pt;
	int i;

	LPCONSTELLATION pConstellation = GetConstellation(m_nCurrent);
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	i = pConstellation->FindPoint(pt.x, pt.y, m_nCloseness, m_bUseLast);
	if (i < 0)
	{
		return(FALSE); // not handled
	}
	else
	{
		SetCursor(m_hHotSpotCursor);
		// must set real return value for us to take over the cursor
   		SetDlgMsgResult(hWnd, WM_SETCURSOR, TRUE);
		return(TRUE); // handled
	}
}

//************************************************************************
void CConnectScene::OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	m_fIntroPlaying = FALSE;
	PlaySoundTrack();
}

#ifdef UNUSED
//***************************************************************************
UINT CConnectScene::OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{
	if (codeNotify == MCI_NOTIFY_SUCCESSFUL)
	{
		if (hDevice == m_hSound)
		{
			MCIClose(m_hSound);
			m_hSound = NULL;
		}
		PlaySoundTrack();
	}
	return(TRUE);
}
#endif

//************************************************************************
void CConnectScene::DrawLine(int sx, int sy, int dx, int dy)
//************************************************************************
{
	RECT rUpdate;

	HDC hDC = ToonDC(GetToon()->GetToonHandle());
	HPALETTE hPal = GetPalette();
	HPALETTE hOldPal = SelectPalette(hDC, hPal, FALSE);

	HPEN hPen = CreatePen(PS_SOLID, m_nLineThickness, PALETTEINDEX(m_nColorIndex));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	MoveTo(hDC, sx, sy);
	LineTo(hDC, dx, dy);
	SelectObject(hDC, hOldPen);
	SelectPalette(hDC, hOldPal, TRUE);
	DeleteObject(hPen);
	rUpdate.left = min(sx, dx);
	rUpdate.right = max(sx, dx);
	rUpdate.top = min(sy, dy);
	rUpdate.bottom = max(sy, dy);
	InflateRect(&rUpdate, m_nLineThickness, m_nLineThickness);

	GetToon()->GrabBackgroundArea(&rUpdate, TRUE);
}

//************************************************************************
BOOL CConnectScene::IsSolved()
//************************************************************************
{
	int i;
	LPCONSTELLATION pConstellation;

	pConstellation = GetConstellation(m_nCurrent);
	for (i = pConstellation->GetFirst(m_bUseLast); i <= pConstellation->GetLast(m_bUseLast); ++i)
	{
		// skip null points
		if (!pConstellation->IsPointConnected(i))
			return(FALSE);
	}
	pConstellation->m_fCompleted = TRUE;
	return(TRUE);
}

//************************************************************************
void CConnectScene::ShowGuideSprite(BOOL fShow)
//************************************************************************
{
	if (!m_pAnimator)
		return;

	if (m_lpGuideSprite)
	{
		m_lpGuideSprite->Show(FALSE);
		m_pAnimator->DeleteSprite(m_lpGuideSprite);
		m_lpGuideSprite = NULL;
	}
	if (fShow)
	{
		LPCONSTELLATION pConstellation = GetConstellation(m_nCurrent);
		if (pConstellation && lstrlen(pConstellation->m_szGuide))
		{
			POINT ptOrigin;

			ptOrigin.x = ptOrigin.y = 0;
			m_lpGuideSprite = m_pAnimator->CreateSprite( &ptOrigin );
			if (m_lpGuideSprite)
			{
				FNAME szFileName;

				GetPathName(szFileName, pConstellation->m_szGuide);
				m_lpGuideSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
				m_lpGuideSprite->Jump(53, 36);
				m_lpGuideSprite->Show(TRUE);
			}
		}
	}
}


//************************************************************************
void CConnectScene::ShowStarSprite(BOOL fShow)
//************************************************************************
{
	if (m_lpStarSprite && fShow != m_fStarOn)
	{
		if (fShow)
		{
			int iWidth, iHeight, x, y, i;

			LPCONSTELLATION pConstellation = GetConstellation(m_nCurrent);

			for (i = pConstellation->GetFirst(m_bUseLast); i <= pConstellation->GetLast(m_bUseLast); ++i)
			{
				if (!pConstellation->IsPointConnected(i))
					break;
			}
			if ( (i <= pConstellation->GetLast(m_bUseLast)) &&
				(GetLevel() == 0 || (GetLevel() == 1 && i == pConstellation->GetFirst(m_bUseLast))) )
			{
				m_lpStarSprite->GetMaxSize(&iWidth, &iHeight);
				x = pConstellation->m_pts[i].x - (iWidth / 2);
				y = pConstellation->m_pts[i].y - (iHeight / 2);
				m_lpStarSprite->Jump(x, y);
				m_lpStarSprite->Show(TRUE);
				m_fStarOn = TRUE;
			}
		}
		else
		{
			m_lpStarSprite->Show(FALSE);
			m_fStarOn = FALSE;
		}
	}
}

//************************************************************************
void CConnectScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

	GetToon()->SetHintState(FALSE);

	if (!m_pSound)
	{
		m_pSound = new CSound(TRUE);
		if (m_pSound)
			m_pSound->Open( NULL/*"powersnd.dll"*/ );
	}

	if (!m_pSound)
		return;

 	GetPathName(szFileName, m_szIntros[GetLevel()]);
	m_pSound->StopChannel(INTRO_CHANNEL);
	m_pSound->StartFile(szFileName, FALSE, INTRO_CHANNEL, FALSE, m_hWnd);
	m_fIntroPlaying = TRUE;
}

//************************************************************************
void CConnectScene::PlaySoundTrack()
//************************************************************************
{
	FNAME szFileName;

	GetToon()->SetHintState(TRUE);
	if (!m_pSound)
	{
		m_pSound = new CSound(TRUE);
		if (m_pSound)
			m_pSound->Open( NULL/*"powersnd.dll"*/ );
	}
	if (m_pSound)
	{
 		GetPathName(szFileName, m_szSoundTrack);
		m_pSound->StopChannel(SOUNDTRACK_CHANNEL);
		m_pSound->StartFile(szFileName, TRUE, SOUNDTRACK_CHANNEL, FALSE);
	}
}

//************************************************************************
void CConnectScene::PlaySoundEffect(LPSTR lpEffect)
//************************************************************************
{
	FNAME szFileName;

	if (!m_pSound)
		return;
	if (!lstrlen(lpEffect))
		return;
	m_pSound->StopChannel(SFX_CHANNEL);
	GetPathName(szFileName, lpEffect);
	m_pSound->StartFile(szFileName, FALSE, SFX_CHANNEL, FALSE);
}

//************************************************************************
BOOL CConnectScene::PlaySound(LPCTSTR lpWaveFile, BOOL fHint)
//************************************************************************
{
	BOOL fRet = FALSE;

	if (fHint)
	{
		if (m_pSound)
		{
			m_pSound->StopChannel(HINT_CHANNEL);
			fRet = m_pSound->StartFile((LPSTR)lpWaveFile, FALSE, HINT_CHANNEL, FALSE);
		}
	}
	else
	{
		CSound sound;

		// this nonsense is here because in order to make buttons
		// in action strip play the click wave when they are clicked
		// on we need to synchronously play the sound so the click
		// happens while the button is down.  WaveMix does not play
		// sounds synchronously, so we deactivate WaveMix and use
		// sndPlaySound stuff.
		if (m_pSound)
			m_pSound->Activate(FALSE);
		fRet = sound.StartFile((LPSTR)lpWaveFile, FALSE, -1, TRUE);
		if (m_pSound)
			m_pSound->Activate(TRUE);
	}
	return(fRet);
}

//************************************************************************
void CConnectScene::ChangeLevel(int iLevel, BOOL fPlayIntro)
//************************************************************************
{
	if (m_pSound)
		m_pSound->StopAndFree();

	ShowStarSprite(FALSE);
	for (int i = 0; i < m_nConstellations; ++i)
		m_pConstellations[i].Reset();
	m_iLevel = iLevel;
	m_nCurrent = -1;
	m_nCompleted = 0;
	MoveBoat();
	LPCONSTELLATION pConstellation = GetNextConstellation();
	if (pConstellation)
	{
		ShowGuideSprite(FALSE);
		GetToon()->LoadBackground(	pConstellation->m_szBackground1 );
		ShowGuideSprite(TRUE);
		ShowStarSprite(TRUE);
		if (fPlayIntro)
			PlayIntro();
	}
}

//************************************************************************
void CConnectScene::MoveBoat()
//************************************************************************
{
	int x = m_ptStart.x + (((m_ptEnd.x - m_ptStart.x) * m_nCompleted) / GetNumConstellations());
	GetToon()->MoveHotSpot(BOAT_NAME, x, m_ptStart.y);
}

//************************************************************************
CConstellation::CConstellation()
//************************************************************************
{
	for (int i = 0; i < MAX_POINTS; ++i)
	{
		m_pts[i].x = m_pts[i].y = 0;
		m_fConnected[i] = FALSE;
	}
	m_nPoints = 0;
	m_szBackground1[0] = '\0';
	m_szBackground2[0] = '\0';
	m_nWaves = 0;
	for (i = 0; i < MAX_WAVES; ++i)
		m_szWaveFile[i][0] = '\0';
	m_szNoiseFile[0] = '\0';
	m_szGuide[0] = '\0';
	m_iLevel = 0;
	m_ptOffset.x = m_ptOffset.y = 0;
	m_fCompleted = FALSE;
}

//************************************************************************
CConstellation::~CConstellation()
//************************************************************************
{
}

//************************************************************************
int CConstellation::FindPoint(int x, int y, int closeness, BOOL bUseLast)
//************************************************************************
{
	// find the first point that's closest, that point must not
	// have any previous points that are not connected
	for (int i = GetFirst(bUseLast); i <= GetLast(bUseLast); ++i)
	{
		// only check points that are not already connected
		if (!IsPointConnected(i))
		{
			int dx = m_pts[i].x;
			int dy = m_pts[i].y;
			// did the user click close enough
			if (abs(x - dx) <= closeness &&
				abs(y - dy) <= closeness)
			{
				return(i);
			}
			else
			{
				// user clicked on the wrong point
				// tell the poor child they are a sniveling idiot
				// and will never amount to anything!
				break;
			}
		}
	}
	return(-1);
}

//************************************************************************
BOOL CConstellation::IsPointConnected(int i)
//************************************************************************
{
	// treat null points as already connected
	if (m_pts[i].x == 0 && m_pts[i].y == 0)
		return(TRUE);
	// treat points after null point as already connected
	if (i && m_pts[i-1].x == 0 && m_pts[i-1].y == 0)
		return(TRUE);
	// use connected state
	return(m_fConnected[i]);
}

//************************************************************************
void CConstellation::AdjustPoints()
//************************************************************************
{
	for (int i = 0; i < m_nPoints; ++i)
	{
		// only adjust non-null points
		if (m_pts[i].x || m_pts[i].y)
		{
			m_pts[i].x += m_ptOffset.x;
			m_pts[i].y += m_ptOffset.y;
		}
	}
}

//************************************************************************
void CConstellation::Reset()
//************************************************************************
{
	for (int i = 0; i < m_nPoints; ++i)
		m_fConnected[i] = FALSE;
	m_fCompleted = FALSE;
}

//************************************************************************
CConnectTransition::CConnectTransition()
//************************************************************************
{
	m_uTransition = TRANS_DISSOLVE;
	m_nStepSize = 10;
	m_nTicks = 0;
	m_dwTotalTime = 1000;
}			  

//************************************************************************
BOOL CConnectParser::GetPoints(LPCONSTELLATION lpConstellation, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	int i;

	if (!nValues || (nValues & 1))
	{
		Print("'%ls'\n Invalid point list '%ls'", lpEntry, lpValues);
		return(FALSE);
	}
	nValues /= 2;
	i = lpConstellation->m_nPoints;
	lpConstellation->m_nPoints += nValues;
	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%ls'\n Bad number at '%ls'", lpEntry, lpValue);
			return(FALSE);
		}
		lpConstellation->m_pts[i].x = (int)l;
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%ls'\n Bad number at '%ls'", lpEntry, lpValue);
			return(FALSE);
		}
		lpConstellation->m_pts[i].y = (int)l;
		++i;
	}
	return(TRUE);
}

//************************************************************************
BOOL CConnectParser::GetFlags(LPCONSTELLATION pConstellation, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		Print("'%ls'\n Unknown flag value '%ls'", lpEntry, lpValue);
	}
	return(TRUE);
}

//************************************************************************
BOOL CConnectParser::GetTransition(LPCONNECTTRANSITION pTransition, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (nValues < 1 || nValues > 4)
	{
			Print("'%s'\n Invalid transition description '%s'", lpEntry, lpValues);
			return(FALSE);
	}

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	pTransition->m_uTransition = (UINT)l;
	if (nValues > 1)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}						    
		pTransition->m_nStepSize = (int)l;
	}
	if (nValues > 2)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		pTransition->m_nTicks = (int)l;
	}
	if (nValues > 3)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		pTransition->m_dwTotalTime = (DWORD)l;
	}
 	return(TRUE);
}

//************************************************************************
BOOL CConnectParser::GetWaves(LPCONSTELLATION pConstellation, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	if (nValues < 1 || nValues > MAX_WAVES)
	{
			Print("'%s'\n Invalid wave key '%s'", lpEntry, lpValues);
			return(FALSE);
	}

	pConstellation->m_nWaves = nValues;
	for (int i = 0; i < nValues; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		lstrcpy(pConstellation->m_szWaveFile[i], lpValue);
	}
 	return(TRUE);
}

//************************************************************************
BOOL CConnectParser::GetGameFlags(LPCONNECTSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		if (!lstrcmpi(lpValue, "uselast"))
			lpScene->m_bUseLast = TRUE;
		else
			Print("'%s'\n Unknown flag value '%s'", lpEntry, lpValue);
	}
	return(TRUE);
}

//************************************************************************
BOOL CConnectParser::GetIntros(LPCONNECTSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	if (nValues < 1)
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
BOOL CConnectParser::GetSuccess(LPCONNECTSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	if (nValues < 1)
	{
			Print("'%s'\n Invalid success key '%s'", lpEntry, lpValues);
			return(FALSE);
	}

	for (int i = 0; i < nValues; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		lstrcpy(lpScene->m_szSuccess[i], lpValue);
	}
 	return(TRUE);
}

//************************************************************************
BOOL CConnectParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPCONNECTSCENE lpScene = (LPCONNECTSCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "colorindex"))
			GetInt(&lpScene->m_nColorIndex, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "transitionclip"))
			GetRect(&lpScene->m_rTransitionClip, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "linethickness"))
			GetInt(&lpScene->m_nLineThickness, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "closeness"))
			GetInt(&lpScene->m_nCloseness, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "startpt"))
			GetPoint(&lpScene->m_ptStart, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "endpt"))
			GetPoint(&lpScene->m_ptEnd, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "starcells"))
			GetInt(&lpScene->m_nStarCells, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "trans1"))
			GetTransition(&lpScene->m_Trans1, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "trans2"))
			GetTransition(&lpScene->m_Trans2, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "delay"))
		{
			int iDelay;
			GetInt(&iDelay, lpEntry, lpValues, nValues);
			lpScene->m_dwDelay = (DWORD)iDelay * 1000L;
		}
		else
		if (!lstrcmpi(lpKey, "flags"))
			GetGameFlags(lpScene, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "intros"))
			GetIntros(lpScene, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success"))
			GetSuccess(lpScene, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetFilename(lpScene->m_szSoundTrack, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "click"))
			GetFilename(lpScene->m_szClick, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "gamearea"))
			GetRect(&lpScene->m_rGameArea, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		LPCONSTELLATION pConstellation = lpScene->GetConstellation(nIndex-1);
		if (!lstrcmpi(lpKey, "pts"))
			GetPoints(pConstellation, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "bg1"))
			GetFilename(pConstellation->m_szBackground1, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "bg2"))
			GetFilename(pConstellation->m_szBackground2, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "wave"))
			GetWaves(pConstellation, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "noise"))
			GetFilename(pConstellation->m_szNoiseFile, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "guide"))
			GetFilename(pConstellation->m_szGuide, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "level"))
		{
			GetInt(&pConstellation->m_iLevel, lpEntry, lpValues, nValues);
			--pConstellation->m_iLevel;
		}
		else
		if (!lstrcmpi(lpKey, "flags"))
			GetFlags(pConstellation, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "offset"))
			GetPoint(&pConstellation->m_ptOffset, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

