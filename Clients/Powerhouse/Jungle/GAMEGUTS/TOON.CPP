#include <windows.h>
#include "proto.h"
#include "scene.h"
#include "wintoon.h" 
#include "toon.h" 
#include "mmsystem.h"
#include "transit.h"
#include "parser.h"
#include "wing.h"

//#define TIME_TEST

#define ANIMATE_START 20
#define ANIMATE_ENTRIES 226
#define COMMON_DIR "COMMON\\"
#define HINT_TIME 2000
#define FADE_INC 4
#define FADE_TIME 10

LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

class CToonParser : public CParser
{
public:
	CToonParser(LPSTR lpTableData)
		: CParser(lpTableData) {}
	void RemapTable(PHOTSPOT pHotSpots);

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetAnimID(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues);
	BOOL GetEnd(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSetSequence(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetFlags(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetMultipleShots(SHOTID FAR *lpShotID, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetRectOrPoint(LPRECT lpRect, LPSTR lpEntry, LPSTR lpValues, int nValues);
	void ChangeMultipleShots(SHOTID FAR *lpShotID, SHOTID idCur, SHOTID idNew);
	void MultipleShotCheck(SHOTID FAR *lpShotID);
	BOOL GetTransitionDesc(PTRANSITIONDESC pTransitionDesc, LPSTR lpEntry, LPSTR lpValues, int nValues, LPINT lpSceneID = NULL);
	BOOL GetHintState(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetKeepSoundTrack(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetEvent(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSoundTrack(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

CHotSpot::CHotSpot()
{
	int i;

	m_idHotSpot = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idEnd[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idShow[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idHide[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idPreHide[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idPreShow[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idEnable[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idDisable[i] = 0;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		m_idStopShow[i] = 0;
	m_idSequence = 0;
	m_dwFrom = 0;
	m_dwTo = 0;
	m_nSequence = 0;
	m_nSequenceNum = 0;
	m_nCurSequence = 0;
	m_bLoop = FALSE;
	m_bVisible = TRUE;
	m_bEnabled = TRUE;
	m_bAuto = FALSE;
	m_bIdle = FALSE;
	m_bExit = FALSE;
	m_bNoUndraw = FALSE;
	m_bCanStop = FALSE;
	m_bForceEnable = FALSE;
	m_bButton = FALSE;
	m_bAutoHighlight = FALSE;
	m_bSprite = FALSE;
	m_bBrowse = FALSE;
	m_bHighlighted = FALSE;
	m_bNoFade = FALSE;
	m_bAlwaysEnabled = FALSE;
	m_bNoHide = FALSE;
	m_bNoTransparency = FALSE;
	m_ptPos.x = m_ptPos.y = -1;
	SetRectEmpty(&m_rHot);
	SetRectEmpty(&m_rClip);
	m_szForegroundDib[0] = '\0';
	m_pForegroundDib = NULL;
	m_pHighlightDib = NULL;
	m_szWaveFile[0] = '\0';
	m_szHintWave[0] = '\0';
	m_uEndDelay = 0;
	m_bEnableCursor = TRUE;
	m_idCommand = 0;
	m_lpSprite = NULL;
	m_nCells = 1;
	m_nSpeed = 4;
	m_nPauseTime = 0;
	for (i = 0; i < MAX_EVENTS; ++i)
	{
		m_AnimEvents[i].dwFrame = 0;
		m_AnimEvents[i].idHotSpot = 0;
		m_AnimEvents[i].bTriggered = FALSE;
	}
}

CHotSpot::~CHotSpot()
{
	if (m_pForegroundDib)
		delete m_pForegroundDib;
	if (m_pHighlightDib)
		delete m_pHighlightDib;
}

BOOL CHotSpot::GetEraseRect(LPRECT lpRect, BOOL bIgnoreVisibleFlag)
{
	SetRectEmpty(lpRect);
	if ((m_bVisible || bIgnoreVisibleFlag) && m_pForegroundDib)
	{
		if (m_ptPos.x != -1)
		{
			lpRect->left = m_ptPos.x;
			lpRect->top = m_ptPos.y;
			lpRect->right = lpRect->left + m_pForegroundDib->GetWidth();
			lpRect->bottom = lpRect->top + abs(m_pForegroundDib->GetHeight());
		}
		else
		if (!IsRectEmpty(&m_rHot))
		{
			lpRect->left = m_rHot.left;
			lpRect->top = m_rHot.top;
			lpRect->right = lpRect->left + m_pForegroundDib->GetWidth();
			lpRect->bottom = lpRect->top + abs(m_pForegroundDib->GetHeight());
		}
	}
	return(!IsRectEmpty(lpRect));
}

CTransitionDesc::CTransitionDesc()
{
	m_fFade = TRUE;
	m_fClip = TRUE;
	m_uTransition = TRANS_STRETCH_LEFT;
	m_nStepSize = -1;
	m_nTicks = 0;
	m_dwTotalTime = 0;
}			  

CToon::CToon(HWND hWnd, int idCursor, HINSTANCE hInstance, HPALETTE hPal)
{
	m_hToon = NULL;
	m_hWnd = hWnd;                     
	if (hInstance)
		m_hInstance = hInstance;
	else
		m_hInstance = GetWindowInstance(m_hWnd);
	m_hHotSpotCursor = LoadCursor(m_hInstance, MAKEINTRESOURCE(idCursor));
	m_hPal = NULL; //hPal;
	m_nSceneNo = 14;
	m_fSoundTrack = FALSE;
	Init();
}

void CToon::Init()
{
	m_nHotSpots = 0;
	m_pHotSpots = NULL;
	m_TransparentIndex = -1;
	m_hMovie = NULL; 
	m_fPlaying = FALSE;
	m_idCurHotSpot = 0;
	m_idButtonHotSpot = 0;
	m_idBrowseHotSpot = 0;
	m_idHintHotSpot = 0;
	m_idLastHintHotSpot = 0;
	m_pBackgroundDib = NULL;
	m_pStageDib = NULL;
	m_ptMovie.x = m_ptMovie.y = 0;
	m_PlayingState = CToon::Normal;
	m_lLastFrame = -1;
	m_lTotalDropped = 0;
	m_fCursorEnabled = TRUE;
	m_hToonPal = NULL;
	m_iAnimate = 0;
	m_hSound = NULL;
	m_iPaintMethod = PAINT_INVALIDATE_RECT;
	if (SHIFT)
		m_iPaintList = LIST_NORMAL;
	else
		m_iPaintList = LIST_SLOW;
	m_iAutoPlayDelay = 0;
	m_idTimer = 0;
	m_szAVIFile[0] = '\0';
	m_ToonDrawMessage = 0;
	m_fToonSetup = FALSE;
	m_fSendMouseToParent = FALSE;
	m_szPath[0] = '\0';
	SetRectEmpty(&m_rGlobalClip);
	m_bNoTransitionClip = FALSE;
	m_pAnimator = NULL;
	m_bIdleSprites = FALSE;
	m_szWaveFile[0] = '\0';
	m_szBGDib[0] = '\0';
	SetRectEmpty(&m_rSrcOrig);
	m_dwHintTime = 0;
	m_fHintsOn = FALSE;
	m_fInitialized = FALSE;
	m_fSoundStarted = FALSE;
	m_dwStartVol = 0;
	m_nJumpSceneNo = 0;
	m_fClickablesEnabled = TRUE;
	m_idSoundTrack = 0;
	m_szSoundTrack[0] = '\0';
	m_fKeepSoundTrack = FALSE;
}

CToon::~CToon()
{        
	FreeUp();
	m_WinGDib.SetPtr(NULL);
	if (m_hToon)
		ToonDestroy(m_hToon);
	if (m_hHotSpotCursor)
	{
		DestroyCursor(m_hHotSpotCursor);
		m_hHotSpotCursor = NULL;
	}
}
     
void CToon::FreeUp()
{        
	if (m_hMovie)
		MCIClose(m_hMovie);
	if (m_hSound)
		MCIClose(m_hSound);
	if (m_pBackgroundDib)
	{
		if (m_hToon)
			ToonSetBackground(m_hToon, NULL, NULL);
		delete m_pBackgroundDib;
	}
	if (m_pStageDib)
		delete m_pStageDib;
	if (m_pHotSpots)
		delete [] m_pHotSpots;
	SetCursor(LoadCursor( NULL, IDC_ARROW ));
	EnableCursor(TRUE);
	if (m_hToonPal)
	{
		DeleteObject(m_hToonPal);
		m_hToonPal = NULL;
	}
	SetTimer(/* RESET */);
	Init();
}
     
BOOL CToon::CreateToon(int cx, int cy)
{             
	WORD 	wVersion;
                
	// check Wintoon version number	- major version should be =, minor should be >=
	wVersion = WINTOON_VERSION;
	if ( (HIBYTE(ToonVersion()) != HIBYTE(wVersion)) || 
		 (LOBYTE(ToonVersion()) < LOBYTE(wVersion)) )
	{     
		Print("Incorrect version of WinToon %x", ToonVersion());
		return FALSE;
	}      

 	m_hToon = ToonCreate(m_hWnd, cx, cy, 0);
 	if (m_hToon == NULL)
	{
		Print("Not enough memory to create Toon");
		return FALSE;
	}
	return TRUE;
}

BOOL CToon::Create(LPCTSTR lpPath, LPCTSTR lpBGDib, UINT ToonDrawMessage)
{             
	if (lpPath)
		lstrcpy(m_szPath, lpPath);
	lstrcpy(m_szBGDib, lpBGDib);
	m_ToonDrawMessage = ToonDrawMessage;

	// make sure we get a full window paint
	InvalidateRect(m_hWnd, NULL, FALSE);

	return(TRUE);
}

BOOL CToon::Initialize(HDC hDCIn)
{
	int nLastSceneNo;
	int id;
	DWORD dwTime;
	STRING szString;
	BOOL fFadeIn = FALSE;
	HDC hDC;
	FNAME szFileName;

	m_fInitialized = TRUE;
	LPSCENE lpScene = GetApp()->GetCurrentScene();
	if (!lpScene)
	{
		Print("No scene, big problem");
		return(FALSE);
	}
	nLastSceneNo = m_nSceneNo;
	m_nSceneNo = lpScene->GetSceneNo();

	if (lpScene->GetSound() && FindResource(m_hInstance, (LPSTR)MAKEINTRESOURCE(m_nSceneNo), "WAVE"))
		m_fSoundStarted = lpScene->GetSound()->StartResourceID(m_nSceneNo, FALSE, SOUND_CHANNEL, m_hInstance ); 

	LoadJumpTable();

	if (!m_fKeepSoundTrack && m_fSoundTrack)
	{
		m_Sound.StopChannel(SOUND_CHANNEL);
		m_fSoundTrack = FALSE;
	}
	if (!m_fSoundStarted)
	{
		if (m_idSoundTrack && FindResource(m_hInstance, (LPSTR)MAKEINTRESOURCE(m_idSoundTrack), "WAVE"))
			m_fSoundTrack = m_Sound.StartResourceID(m_idSoundTrack, FALSE, SOUND_CHANNEL, m_hInstance ); 
		else
		if (lstrlen(m_szSoundTrack))
		{
			lstrcpy(szFileName, m_szPath);
			lstrcat(szFileName, m_szSoundTrack);
			m_fSoundTrack = m_Sound.StartFile(szFileName, FALSE, SOUND_CHANNEL ); 
		}
	}

	if (m_nJumpSceneNo && m_nJumpSceneNo == nLastSceneNo)
		m_Transition = m_SceneTransition;
	else
	if (m_nSceneNo  == (nLastSceneNo+1))
		m_Transition = m_LeftTransition;
	else
	if (m_nSceneNo  == (nLastSceneNo-1))
		m_Transition = m_RightTransition;
	else
		m_Transition = m_JumpTransition;
	if (hDCIn)
		hDC = hDCIn;
	else
		hDC = GetDC(m_hWnd);
	int caps = GetDeviceCaps(hDC, RASTERCAPS);
	if (!(caps & RC_PALETTE)) // non-palette device
		m_Transition.m_fFade = FALSE;

    if (m_Transition.m_fFade)
	{
    	FadeOut();
		fFadeIn = TRUE;
		m_Transition.m_uTransition = 0;

		// look for hot spots that cannot be faded and make them invisible
		for (int id = 1; id <= m_nHotSpots; ++id)
		{
			PHOTSPOT pHotSpot = GetHotSpot(id);
			if (!pHotSpot)
				continue;
			if (pHotSpot->m_bNoFade)
				pHotSpot->m_bVisible = FALSE;
		}
	}

	// load data from files and resources
	dwTime = timeGetTime();
	if (!LoadData(m_szBGDib))
	{
		if (!hDCIn)
			ReleaseDC(m_hWnd, hDC);
		return(FALSE);
	}
	wsprintf(szString, "Load Data: %ld ms\n", timeGetTime()-dwTime);
	OutputDebugString(szString);

    // plug our stage in as the WinToon background
	ToonSetBackground(GetToonHandle(), m_pStageDib->GetInfoHeader(), m_pStageDib->GetPtr());

	// set color table if we are not fading in.  If you
	// set the color table the display palette will change
	if (!fFadeIn) 	    
		ToonSetColors(GetToonHandle(), m_pStageDib->GetColors());

	// make sure the WinG dib is initialized
	UpdateWinGDib();

    // build the stage, this copies the foreground sprites into the background dib
	dwTime = timeGetTime();
	BuildStage();
	wsprintf(szString, "Build Stage: %ld ms\n", timeGetTime()-dwTime);
	OutputDebugString(szString);

	// copy bits to WinG bitmap
	// only if we have a background bitmap
	// otherwise, BuildStage takes care of this for us
	if (m_pBackgroundDib)
	{
		dwTime = timeGetTime();
		ToonRestoreBackground(GetToonHandle());
		wsprintf(szString, "Restore Background: %ld ms\n", timeGetTime()-dwTime);
		OutputDebugString(szString);
	}

	if (m_ToonDrawMessage)
		ToonSetMessage(GetToonHandle(), m_ToonDrawMessage);

	// see if we need to disable the cursor	                               
	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		// disable cursor if we have an auto animation
		if (pHotSpot->m_bAuto && !pHotSpot->m_bCanStop)
		{
			EnableCursor(FALSE);
			break;
		}
	}
	
	// init any idle sprites
	InitIdleSprites();

	if (fFadeIn)
	{
		ToonPaintDC(GetToonHandle(), hDC);
		OutputDebugString("calling FadeIn\n");
		FadeIn();
		// look for hot spots that cannot be faded and make them visible
		for (int id = 1; id <= m_nHotSpots; ++id)
		{
			PHOTSPOT pHotSpot = GetHotSpot(id);
			if (!pHotSpot)
				continue;
			if (pHotSpot->m_bNoFade)
			{
				pHotSpot->m_bVisible = TRUE;
				BuildStage(id, CToon::UpdateHotSpot);
			}
		}
	}
	if (!hDCIn)
		ReleaseDC(m_hWnd, hDC);

	return(TRUE);
}

void CToon::EndInitialize()
{
	if (m_fSoundStarted)
	{
		OutputDebugString("Calling StopAndFree\n");
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		lpScene->GetSound()->StopChannel(SOUND_CHANNEL);
	}

	LPSCENE lpScene = GetApp()->GetCurrentScene();
	lpScene->ToonInitDone();

	PlayAuto();
}

BOOL CToon::LoadData(LPCTSTR lpBGDib)
{
	CDib dib;
	BOOL fTopDown;
	FNAME szFileName;
	DWORD dwTime;
	STRING szString;

	m_hPal = NULL;
	lstrcpy(szFileName, m_szPath);
	lstrcat(szFileName, lpBGDib);
	dwTime = timeGetTime();
	if (!(m_pBackgroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, TRUE)))
	{
		Print("Error opening file '%s'", (LPSTR)lpBGDib);
		return (FALSE);
	}
	wsprintf(szString, "Load background: %ld ms\n", timeGetTime()-dwTime);
	OutputDebugString(szString);

	LoadForegroundDibs();

	m_hToonPal = CreateIdentityPalette(m_pBackgroundDib->GetColors(), 256);
	if (!m_hToonPal)
	{
		Print("Out of Memory");
		return(FALSE);
	}

	//	Get WinG to recommend the fastest DIB format
	if ( WinGRecommendDIBFormat( dib.GetInfo() ) )
		fTopDown = dib.GetHeight() < 0;
	else
		fTopDown = FALSE; 
	
	if (fTopDown != (m_pBackgroundDib->GetHeight() < 0))
	{
		dwTime = timeGetTime();
		m_pBackgroundDib->DibFlip();
		wsprintf(szString, "Flip Background: %ld ms\n", timeGetTime()-dwTime);
		OutputDebugString(szString);
	}
                       
    // open up the movie
    if (lstrlen(m_szAVIFile))
    {   
		FNAME szFileName;

		// create the  stage dib by copying the background dib
		dwTime = timeGetTime();
		if (!(m_pStageDib = m_pBackgroundDib->DibCopy(FALSE)))
		{
			Print("Out of Memory");
			return(FALSE);
		}
		wsprintf(szString, "Create Stage Dib: %ld ms\n", timeGetTime()-dwTime);
		OutputDebugString(szString);

		lstrcpy(szFileName, m_szPath);
		lstrcat(szFileName, m_szAVIFile);
		//if ((m_hMovie = MCIOpen(GetApp()->m_hDeviceAVI, szFileName, NULL)) == NULL)
		dwTime = timeGetTime();
		if ((m_hMovie = ToonOpenMovie(GetToonHandle(), szFileName, m_TransparentIndex)) == NULL)
		{
			Print("Error opening file '%s'", (LPSTR)m_szAVIFile);
			// let it go for now return(FALSE);
		}
		else
		{    
			if (m_fSoundTrack)
				MCISetAudioOnOff(m_hMovie, FALSE);
			wsprintf(szString, "Open AVI: %ld ms\n", timeGetTime()-dwTime);
			OutputDebugString(szString);
			MCIGetSrcRect(m_hMovie, &m_rSrcOrig);
			//MCISetAudioStream(m_hMovie, 1);
			ToonSetPaintMethod(GetToonHandle(), m_iPaintMethod);
			if (!ToonSetPaintList(GetToonHandle(), m_iPaintList))
			{
				m_iPaintList = LIST_SLOW;
				ToonSetPaintList(GetToonHandle(), m_iPaintList);
			}

			//if (m_iPaintList == LIST_SLOW)
			//	OutputDebugString("using SLOW paint list\n");
			//else
			//if (m_iPaintList == LIST_FAST)
			//	OutputDebugString("using FAST paint list\n");
			//else
			//	OutputDebugString("using NORMAL paint list\n");
			++m_iPaintList;
			if (m_iPaintList > LIST_FAST)
				m_iPaintList = LIST_NORMAL;

		}
	}
	else
	{
		// don't need background dib if you don't have a movie
		m_pStageDib = m_pBackgroundDib;
		m_pBackgroundDib = NULL;
	}
	return(TRUE);
}

BOOL CToon::LoadJumpTable()
{
	HRSRC hResource;
	HGLOBAL hData;
	LPSTR lpTableData;
	DWORD dwSize, dwTime;
	STRING szString;
	int id;

	// read in the hotspot information
	dwSize = 0;
	
	hData = NULL;
	lpTableData = NULL;
	id = m_nSceneNo;
	dwTime = timeGetTime();
	if ( id && (hResource = FindResource( m_hInstance, MAKEINTRESOURCE(id), RT_RCDATA )) )
		if ( (dwSize = SizeofResource( m_hInstance, hResource )) )
			if ( (hData = LoadResource( m_hInstance, hResource )) )
			{
				if ( !(lpTableData = (LPSTR)LockResource( hData )) )
  				{
					FreeResource( hData );
					hData = NULL;
  				}
			}
	wsprintf(szString, "Load Jump Table: %ld ms\n", timeGetTime()-dwTime);
	OutputDebugString(szString);

	dwTime = timeGetTime();
	if (lpTableData)
	{
		CToonParser parser(lpTableData);
		if (m_nHotSpots = parser.GetNumEntries())
		{
			// first entry is global toon settings
			m_nHotSpots -= 1;
			if (m_nHotSpots)
				m_pHotSpots = new CHotSpot[m_nHotSpots];
			parser.SetNumEntries(m_nHotSpots);
			parser.ParseTable((DWORD)this);
			if (m_pHotSpots)
				parser.RemapTable(m_pHotSpots);
			LoadForegroundDibs();
		}
	}

	wsprintf(szString, "Parse Jump Table: %ld ms\n", timeGetTime()-dwTime);
	OutputDebugString(szString);

	if (hData)
	{
		UnlockResource( hData );
		FreeResource( hData );
	}

	return(TRUE);
}

LOCAL void CALLBACK TransCallback(DWORD dwIteration, DWORD dwTotalIterations, DWORD dwCallbackData)
{
	PTOON pToon = (PTOON)dwCallbackData;
	pToon->TransCallback(dwIteration, dwTotalIterations);
}

void CToon::TransCallback(DWORD dwIteration, DWORD dwTotalIterations)
{
	STRING szString;

	dwIteration = dwTotalIterations - dwIteration;

	WORD wVol = LOWORD(m_dwStartVol);
	wVol = (WORD)(((DWORD)wVol * dwIteration) / dwTotalIterations);
	
	wsprintf(szString, "dwStartVol = %lx wVol = %x\n", m_dwStartVol, wVol);
	OutputDebugString(szString);
	MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, wVol, wVol);
}

BOOL CToon::LoadBackground(LPSTR lpFileName, UINT uTransition, int nTicks, int nStepSize, LPRECT lpTransClipRect)
{
	FNAME szFileName;

	if (m_pBackgroundDib)
		delete m_pBackgroundDib;
	else
	if (m_pStageDib)
		delete m_pStageDib;

	lstrcpy(szFileName, m_szPath);
	lstrcat(szFileName, lpFileName);

	m_pBackgroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, TRUE);
	if (ToonIsTopDown(GetToonHandle()) != (m_pBackgroundDib->GetHeight() < 0))
		m_pBackgroundDib->DibFlip();
    if (!lstrlen(m_szAVIFile))
	{
		// don't need background dib if you don't have a movie
		m_pStageDib = m_pBackgroundDib;
		m_pBackgroundDib = NULL;
	}

   	BuildStage();
	ToonSetBackground(GetToonHandle(), m_pStageDib->GetInfoHeader(), m_pStageDib->GetPtr());
	if (m_pBackgroundDib)
		ToonRestoreBackground(GetToonHandle());

	if (uTransition)
	{
		PDIB pWinGDib;
		HPALETTE hPal, hOldPal;
		RECT rRepair;
		TRANSPARMS parms;

		SetRect(&rRepair, 0, 0, GetWidth(), GetHeight());
		HDC hDC = GetDC(m_hWnd);
		pWinGDib = GetWinGDib();
		hPal = CreateIdentityPalette(pWinGDib->GetColors(), 256);
		if (hPal)
		{
			hOldPal = SelectPalette(hDC, hPal, FALSE);
			::RealizePalette(hDC);
		}
		if (!IsRectEmpty(&m_rGlobalClip) && !m_bNoTransitionClip)
			IntersectRect(&rRepair, &rRepair, &m_rGlobalClip);
		if (lpTransClipRect)
			IntersectRect(&rRepair, &rRepair, lpTransClipRect);
		SetTransitionParms( &parms, uTransition, hDC,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, pWinGDib, ToonDC(m_hToon));
		SetTransitionTiming( &parms, nTicks, nStepSize, 7000);
		if (m_fSoundStarted)
		{
			m_dwStartVol = MCIGetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO);
			SetTransitionCallback(&parms, ::TransCallback, (DWORD)this);
		}
		TransitionBlt(&parms);
		if (m_fSoundStarted)
		{
			LPSCENE lpScene = GetApp()->GetCurrentScene();
			lpScene->GetSound()->StopChannel(0);
			MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, LOWORD(m_dwStartVol), HIWORD(m_dwStartVol));
			m_fSoundStarted = FALSE;
		}
			
		if (hPal)
		{
			hOldPal = SelectPalette(hDC, hOldPal, TRUE);
			DeleteObject(hPal);
		}
		ReleaseDC(m_hWnd, hDC);
	}
	else
	{
		UpdateArea(NULL);
	}
	return (m_pBackgroundDib != NULL);
}

PDIB CToon::GetWinGDib()
{
	return(&m_WinGDib);
}

PDIB CToon::UpdateWinGDib()
{
	m_WinGDib.SetPtr(ToonGetDIBPointer(GetToonHandle(), m_WinGDib.GetInfoHeader()));
	ToonGetColors(GetToonHandle(), 0, 256, m_WinGDib.GetColors());
	m_WinGDib.FixHeader();
	return(&m_WinGDib);
}

void CToon::RestoreBackgroundArea(LPRECT lpRect, BOOL fUpdateDisplay)
{
	if (lpRect)
	{
		PDIB pWinGDib = GetWinGDib();
		m_pStageDib->DibBlt(pWinGDib,
							lpRect->left, lpRect->top,
							lpRect->right-lpRect->left, lpRect->bottom-lpRect->top,
							lpRect->left, lpRect->top,
							lpRect->right-lpRect->left, lpRect->bottom-lpRect->top,
							FALSE);
	}
	else
		ToonRestoreBackground(GetToonHandle());
	if (fUpdateDisplay)
	{
		RECT rArea;

		if (lpRect)
			rArea = *lpRect;
		else
			SetRect(&rArea, 0, 0, m_pStageDib->GetWidth(), abs(m_pStageDib->GetHeight()));	
		STRING szString;
		wsprintf(szString, "rArea = %d %d\n", rArea.right-rArea.left, rArea.bottom-rArea.top);
		OutputDebugString(szString);
		UpdateArea(&rArea);
	}
}

BOOL CToon::LoadForegroundDibs(BOOL fDecodeRLE)
{
	SHOTID id;
	BOOL fRet = TRUE;

	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bSprite)
			m_bIdleSprites = TRUE;

		if (lstrlen(pHotSpot->m_szForegroundDib))
		{
			FNAME szFileName;
			OFSTRUCT of;

			// first look in scene directory
			lstrcpy(szFileName, m_szPath);
			lstrcat(szFileName, pHotSpot->m_szForegroundDib);
			if ( OpenFile( szFileName, &of, OF_EXIST ) == HFILE_ERROR )
			{
				// next try common directory
				GetModuleFileName(m_hInstance, szFileName, sizeof(szFileName));
				StripFile(szFileName);
				lstrcat(szFileName, COMMON_DIR);
				lstrcat(szFileName, pHotSpot->m_szForegroundDib);
			}
				
			if (!(pHotSpot->m_pForegroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, fDecodeRLE)))
			{
				Print("Error opening file '%s'", (LPSTR)szFileName);
				// Let it go for now - return (FALSE);
				fRet = FALSE;
			}
			else
			{
				PDIB pDibSrc = pHotSpot->m_pForegroundDib;
				pDibSrc = pDibSrc->GetDib();
				if (pDibSrc && pHotSpot->m_bButton)
				{
					PDIB pDibs[2];
					int iWidth = pDibSrc->GetWidth();
					int iHeight = abs(pDibSrc->GetHeight())/2;
					for (int i = 0; i < 2; ++i)
					{
						LPTR lp;
						int y;

						// create dib the same size as original with no data ptr
						if ( !(pDibs[i] = new CDib(pDibSrc, NULL, FALSE)))
						{
							fRet = FALSE;
							break;
						}

						// reset size of dib and for recalc of size info
						pDibs[i]->SetHeight(iHeight);
						pDibs[i]->SetSizeImage(0);
						pDibs[i]->FixHeader();

						// allocate memory for dib and set new ptr
						if (!(lp = Alloc(pDibs[i]->GetSizeImage())))
						{
							delete pDibs[i];
							pDibs[i] = NULL;
							fRet = FALSE;
							break;
						}
						pDibs[i]->SetPtr(lp);

						// copy the data from the source dib to the destination dib
						y = i * abs(iHeight);
						pDibSrc->DibBlt(	pDibs[i],
											0, 0, iWidth, abs(iHeight),
											0, y, iWidth, abs(iHeight), FALSE);
					}
					pDibSrc->Release();
					delete pDibSrc;
					pHotSpot->m_pForegroundDib = pDibs[0];
					pHotSpot->m_pHighlightDib = pDibs[1];
					pDibSrc = pHotSpot->m_pForegroundDib;
					if (pDibSrc)
						pDibSrc = pDibSrc->GetDib();
				}
				if (pDibSrc)
				{
					if (m_TransparentIndex < 0)
					    m_TransparentIndex = *(pDibSrc->GetXY(0, 0));
					pDibSrc->Release();
				}
				if (pHotSpot->m_pForegroundDib)
				{
					if (pHotSpot->m_rHot.left >= 0 && pHotSpot->m_rHot.top >= 0 &&
						pHotSpot->m_rHot.right < 0 && pHotSpot->m_rHot.bottom < 0)
					{
						pHotSpot->m_rHot.right = pHotSpot->m_rHot.left + pHotSpot->m_pForegroundDib->GetWidth();
						pHotSpot->m_rHot.bottom = pHotSpot->m_rHot.top + abs(pHotSpot->m_pForegroundDib->GetHeight());
					}
				}
			}
		}
	}
	return(fRet);
}

BOOL CToon::InitIdleSprites()
{
	SHOTID id;
	BOOL fRet = TRUE;
	POINT pt;
	PHOTSPOT pHotSpot;
	LPCELL lpCell;
	RECT rErase;

	LPSCENE lpScene = GetApp()->GetCurrentScene();
	m_pAnimator = lpScene->GetAnimator();
	if (!m_pAnimator)
		return(FALSE);

	pt.x = pt.y = 0;
	for (id = 1; id <= m_nHotSpots; ++id)
	{
		pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		// only look for animations that use the sprite engine
		if (!pHotSpot->m_pForegroundDib || !pHotSpot->m_bSprite)
			continue;
			
		// create a sprite for this animation
		pHotSpot->m_lpSprite = m_pAnimator->CreateSprite(&pt);
		if (!pHotSpot->m_lpSprite)
			continue;

		// add the cells of the bitmap to the sprite
		if (!pHotSpot->m_nCells)
			pHotSpot->m_nCells = 1;
		// AddCells() deletes m_pForegroundDib
		pHotSpot->m_lpSprite->AddCells(pHotSpot->m_pForegroundDib, pHotSpot->m_nCells);
		pHotSpot->m_pForegroundDib = NULL;

		// the first cell becomes the dib for the toon engine
		lpCell = pHotSpot->m_lpSprite->GetCurrentCell();
		if (lpCell)
			pHotSpot->m_pForegroundDib = lpCell->pSpriteDib;

		// add the new smaller sprite back to the stage dib
		BuildStage(id, CToon::ShowHotSpot);
		pHotSpot->GetEraseRect(&rErase);
		// now copy it to the WinG dib
		RestoreBackgroundArea(&rErase, FALSE);

		// set sprite settings
		pHotSpot->m_lpSprite->Jump(rErase.left, rErase.top);
		pHotSpot->m_lpSprite->SetCellsPerSec(pHotSpot->m_nSpeed);
		pHotSpot->m_lpSprite->Pause(PAUSE_FOREVER);
		if (pHotSpot->m_nPauseTime)
		{
			// marks beginning of a set of commands to repeat
			pHotSpot->m_lpSprite->AddCmdBegin();
			// pause after last cell has been displayed
			pHotSpot->m_lpSprite->AddCmdPauseAfterCell(pHotSpot->m_nCells-1, pHotSpot->m_nPauseTime);
			// after pause, reset cell to first
			pHotSpot->m_lpSprite->AddCmdSetCurrentCell(0);
			// repeat the commands starting at begin
			pHotSpot->m_lpSprite->AddCmdRepeat();
		}
		if (!pHotSpot->m_bIdle)
			pHotSpot->m_lpSprite->SetNotifyProc(::OnSpriteNotify, (DWORD)this);
	}
	return(TRUE);
}

HPALETTE CToon::GetPalette()
{
	if (!m_hToonPal)
	{
		RGBQUAD rgbQuad[256];

		ToonGetColors(GetToonHandle(), 0, 256, rgbQuad);
		m_hToonPal = CreateCustomPalette(rgbQuad, 256);
	}
	return(m_hToonPal);
}

void CToon::StopPlaying(BOOL fExiting)
{
	if (fExiting)
	{
		HDC hDC = GetDC(m_hWnd);
		int caps = GetDeviceCaps(hDC, RASTERCAPS);
		ReleaseDC(m_hWnd, hDC);
		if (caps & RC_PALETTE) // palette device?
		{
			// look for hot spots that cannot be faded
			for (int id = 1; id <= m_nHotSpots; ++id)
			{
				PHOTSPOT pHotSpot = GetHotSpot(id);
				if (!pHotSpot)
					continue;
				if (pHotSpot->m_bNoFade)
				{
					pHotSpot->m_bVisible = FALSE;
					BuildStage(id, CToon::UpdateHotSpot, TRUE);
				}
			}
		}
	}

	if (m_fPlaying)
	{
		if (m_hSound)
		{
			MCIStop(m_hSound, TRUE);
			MCIClose(m_hSound);
			m_hSound = NULL;
		}
		else
		{
			//OutputDebugString("calling MCIStop\n");
			MCIStop(m_hMovie, TRUE);
		}
		//OutputDebugString("calling Reset\n");
		// call change sprite states instead of reset
		// so that hides and shows will occur
		PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
		if (pHotSpot)
			ChangeSpriteStates(pHotSpot);
		//Reset();
	}
}

BOOL CToon::PlayAuto(BOOL fNoDelay)
{
	SHOTID id;

	if (m_iAutoPlayDelay && !fNoDelay)
	{
		if (SetTimer( CToon::AutoPlayTimer, (UINT)m_iAutoPlayDelay*1000 ))
			return(TRUE);
	}

	StopPlaying();
	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bAuto)
		{
			m_PlayingState = CToon::Auto;
			return(OnHotSpot(pHotSpot->m_idHotSpot));
		}
	}
	// see if we have an idle sprite animation
	return(StartIdle());
}
  
BOOL CToon::PlayConclusion()
{
	SHOTID id;

	StopPlaying();
	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bExit)
		{
			m_PlayingState = CToon::Conclusion;
			return(OnHotSpot(pHotSpot->m_idHotSpot));
		}
	}
	return(FALSE);
}
  
BOOL CToon::CheckSequence(SHOTID idSequence, int nSequence)
{
	PHOTSPOT pHotSpot = GetHotSpot(idSequence);
	if (!pHotSpot)
	{
		ResetSequence();
		return(FALSE);
	}
	++pHotSpot->m_nCurSequence;
	// out of sequence?
	if (pHotSpot->m_nCurSequence != nSequence)
	{
		ResetSequence();
		return(FALSE);
	}
	else
	{
		BOOL fRet = pHotSpot->m_nCurSequence == pHotSpot->m_nSequenceNum;
		if (fRet)
			ResetSequence();
		return(fRet);
	}
}

void CToon::ResetSequence()
{
	SHOTID id;

	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (pHotSpot)
			pHotSpot->m_nCurSequence = 0;
	}
}

void CToon::EnableCursor(BOOL fEnable)
{
	// make sure we are changing enable state 
	if (fEnable != m_fCursorEnabled)
	{
		if (fEnable)
			OutputDebugString("Enabling Cursor\n");
		else
			OutputDebugString("Disabling Cursor\n");
		m_fCursorEnabled = fEnable;
		//if (fEnable)
		//	ReleaseCapture();
		//else
		//	SetCapture(m_hWnd);
		//ShowCursor(fEnable);
	}
}

void CToon::BuildStage(SHOTID idHotSpot, BuildMode mode, BOOL fUpdateDisplay)
{
	SHOTID id;
	RECT rArea; 
	PDIB pDibSrc, pDibDst;
            
    if (!m_pStageDib)
    	return;
    if (m_pBackgroundDib)
    {
		pDibSrc = m_pBackgroundDib;
		pDibDst = m_pStageDib;
    }
    else
    {
		pDibSrc = m_pStageDib;
		pDibDst = GetWinGDib();
    }

	// copy bits from background to stage CDib
	PHOTSPOT pTheHotSpot = GetHotSpot(idHotSpot);
	if (pTheHotSpot)
	{
		pTheHotSpot->GetEraseRect(&rArea, mode == CToon::UpdateHotSpot);
		if (pTheHotSpot->m_idPreHide[0])
		{
			for (int i = 0; i < MAX_SHOTVALUES; ++i)
			{
				RECT rErase;
				PHOTSPOT pOtherHotSpot = GetHotSpot(pTheHotSpot->m_idPreHide[i]);
				if (!pOtherHotSpot)
					break;
				pOtherHotSpot->GetEraseRect(&rErase, TRUE);
				UnionRect(&rArea, &rArea, &rErase);
			}
		}
		if (pTheHotSpot->m_idPreShow[0])
		{
			for (int i = 0; i < MAX_SHOTVALUES; ++i)
			{
				RECT rErase;
				PHOTSPOT pOtherHotSpot = GetHotSpot(pTheHotSpot->m_idPreShow[i]);
				if (!pOtherHotSpot)
					break;
				pOtherHotSpot->GetEraseRect(&rErase, TRUE);
				UnionRect(&rArea, &rArea, &rErase);
			}
		}

		// no area to update!
		if (IsRectEmpty(&rArea))
			return;

		pDibSrc->DibBlt(pDibDst,
						rArea.left, rArea.top,
						rArea.right-rArea.left, rArea.bottom-rArea.top,
						rArea.left, rArea.top,
						rArea.right-rArea.left, rArea.bottom-rArea.top,
						FALSE);
	}
	else
	{
		hmemcpy(pDibDst->GetPtr(), pDibSrc->GetPtr(), pDibDst->GetSizeImage());
		SetRect(&rArea, 0, 0, pDibDst->GetWidth(), abs(pDibDst->GetHeight()));
	}
             
    OutputDebugString("BuildStage\n");              
	for (id = 1; id <= m_nHotSpots ; id++ )
	{
		RECT rErase, rSect;

		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;

		if ((id != idHotSpot || lstrlen(pTheHotSpot->m_szWaveFile) || (mode != CToon::HideHotSpot)) &&
			pHotSpot->GetEraseRect(&rErase) &&
			(!pHotSpot->m_bSprite || pHotSpot->m_lpSprite))
		{
			if (IntersectRect(&rSect, &rArea, &rErase))
			{
				PDIB pDib;
				int sx, sy;

				if (pHotSpot->m_bButton && pHotSpot->m_bHighlighted && pHotSpot->m_pHighlightDib)
					pDib = pHotSpot->m_pHighlightDib;
				else
					pDib = pHotSpot->m_pForegroundDib;
				int iWidth = rSect.right - rSect.left;
				int iHeight = rSect.bottom - rSect.top;
				sx = rSect.left - rErase.left;
				sy = rSect.top - rErase.top;
				if (pDib)
					pDib = pDib->GetDib();	
			
				if (pDib)
				{
					pDib->DibBlt(pDibDst,
								rSect.left,
								rSect.top,
								iWidth,
								iHeight,
								sx/*-m_ptMovie.x*/,
								sy/*-m_ptMovie.y*/,
								iWidth,
								iHeight,
								TRUE);
					pDib->Release();
				}
			}
		}
	}
	if (fUpdateDisplay)
	{
		if (m_pBackgroundDib)
			RestoreBackgroundArea(&rArea, TRUE);
		else
		{
			UpdateArea(&rArea);
		}
	}
}
  
PHOTSPOT CToon::GetHotSpot(SHOTID idHotSpot)
{
	if (idHotSpot <= 0)
		return(NULL);
	if (idHotSpot > m_nHotSpots)
		return(NULL);
	return(&m_pHotSpots[idHotSpot-1]);
}

int CToon::FindHotSpot(int x, int y)
{
	POINT pt;
	SHOTID id;
	int dx, dy;

	pt.x = x;
	pt.y = y;

	for (id = m_nHotSpots; id >= 1 ; id--)
	{				 
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;

		if (!m_fClickablesEnabled && !pHotSpot->m_bAlwaysEnabled)
			continue;

		if (pHotSpot->m_bEnabled &&
			(pHotSpot->m_bVisible || pHotSpot->m_bForceEnable || pHotSpot->m_bBrowse) &&
			PtInRect(&pHotSpot->m_rHot, pt))
		{      
			PDIB pDib = NULL;
			RECT rErase;

			if (!pHotSpot->m_pForegroundDib || pHotSpot->m_bNoTransparency)
				return (int)pHotSpot->m_idHotSpot;

			pHotSpot->GetEraseRect(&rErase, TRUE);
			if (!PtInRect(&rErase, pt))
				return(0);

			pDib = pHotSpot->m_pForegroundDib;
			dx = x - rErase.left;
			dy = y - rErase.top;
			pDib = pDib->GetDib();	

			if (pDib)
			{
				BYTE bPixel;
				BOOL bTrans;

				//dx -= m_ptMovie.x;
				//dy -= m_ptMovie.y;
				bTrans = *(pDib->GetXY(0, 0));
				bPixel = *(pDib->GetXY(dx, dy));
				pDib->Release();
				if (bPixel != bTrans)
					return (int)pHotSpot->m_idHotSpot;
			}
			//break;
		}
	}
	return(0);
}

int CToon::FindHotSpot(LPCTSTR lpForegroundDib)
{
	for (int id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (!lstrcmpi(lpForegroundDib, pHotSpot->m_szForegroundDib))
			return(id);
	}
	return(0);
}

BOOL CToon::OnHotSpot(LPCTSTR lpForegroundDib)
{
	int id = FindHotSpot(lpForegroundDib);
	if (id)
		return(OnHotSpot(id));
	else
		return(FALSE);
}

BOOL CToon::OnHotSpot(SHOTID idHotSpot)
{
	HMCI hDevice;
	BOOL fBuildStage = FALSE;

	PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
	if (!pHotSpot)
		return(FALSE);

	if (pHotSpot->m_idPreHide[0])
	{
		for (int i = 0; i < MAX_SHOTVALUES; ++i)
		{
			PHOTSPOT pOtherHotSpot = GetHotSpot(pHotSpot->m_idPreHide[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bVisible = FALSE;
			fBuildStage = TRUE;
		}
	}
	if (pHotSpot->m_idPreShow[0])
	{
		for (int i = 0; i < MAX_SHOTVALUES; ++i)
		{
			PHOTSPOT pOtherHotSpot = GetHotSpot(pHotSpot->m_idPreShow[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bVisible = TRUE;
			fBuildStage = TRUE;
		}
	}
	
	// make sure there is something to do, otherwise all
	// we do is try to change the sprite states.
	if (pHotSpot->m_dwTo > pHotSpot->m_dwFrom)
	{
		if (lstrlen(pHotSpot->m_szWaveFile))
		{
			FNAME szFileName;

			lstrcpy(szFileName, m_szPath);
			lstrcat(szFileName, pHotSpot->m_szWaveFile);
 			m_hSound = MCIOpen( GetApp()->m_hDeviceWAV, szFileName, NULL/*lpAlias*/ );
			if (!m_hSound)
				return(FALSE);
			hDevice = m_hSound;
			if (fBuildStage)
				BuildStage(idHotSpot, CToon::HideHotSpot, TRUE);
		}
		else
		{
			RECT rSrc, rDst;

			if (!m_hMovie)
				return(FALSE);
			
			if (fBuildStage || !pHotSpot->m_bNoHide)
				BuildStage(idHotSpot);
	
			rDst = m_rSrcOrig;
			if (m_ptMovie.x != 0 || m_ptMovie.y != 0)
				OffsetRect(&rDst, m_ptMovie.x, m_ptMovie.y);
			if (!IsRectEmpty(&m_rGlobalClip))
				IntersectRect(&rDst, &rDst, &m_rGlobalClip);
  			if (!IsRectEmpty(&pHotSpot->m_rClip))
		 		IntersectRect(&rDst, &rDst, &pHotSpot->m_rClip);
			rSrc = rDst;
			if (m_ptMovie.x != 0 || m_ptMovie.y != 0)
				OffsetRect(&rSrc, -m_ptMovie.x, -m_ptMovie.y);
			MCIClip(m_hMovie, &rSrc, &rDst);

			hDevice = m_hMovie;
		}

		m_lLastFrame = (LONG)pHotSpot->m_dwFrom-1;
		m_lTotalDropped = 0;
		m_idCurHotSpot = idHotSpot;
		//MCISetAudioOnOff( hDevice, OFF );
		
		for (int i = 0; i < MAX_EVENTS; ++i)
			pHotSpot->m_AnimEvents[i].bTriggered = FALSE;

		if (MCIPlay(hDevice, m_hWnd, pHotSpot->m_dwFrom, pHotSpot->m_dwTo))
		{
			if (!pHotSpot->m_bIdle && !pHotSpot->m_bCanStop)
				EnableCursor(FALSE);
			m_fPlaying = TRUE;
			//if ( hMidi = MCIOpen( GetApp()->m_hDeviceMID, "power.mid", NULL/*lpAlias*/ ) )
			//{
			//	if ( !MCIPlay( hMidi, GetApp()->GetMainWnd() ) )
			//	{
			//		MCIClose( hMidi );
			//		hMidi = NULL;
			//	} // if it's not looping, it will close itself when it ends
			//}
			return(TRUE);
		}
	}
	else
	if (pHotSpot->m_bSprite)
	{
		if (!pHotSpot->m_lpSprite)
			return(FALSE);

		m_idCurHotSpot = idHotSpot;
		BuildStage(idHotSpot);
		pHotSpot->m_lpSprite->SetCurrentCell(0);
		pHotSpot->m_lpSprite->Pause(0);
		pHotSpot->m_lpSprite->PauseAfterCell(pHotSpot->m_nCells-1);
		pHotSpot->m_lpSprite->Show(TRUE);
	}
	else
	if (pHotSpot->m_idCommand)
	{
		ChangeSpriteStates(pHotSpot, TRUE);
		FORWARD_WM_COMMAND(GetParentDlg(), pHotSpot->m_idCommand, m_hWnd, 0, SendMessage);
	}
	else
	{
		ChangeSpriteStates(pHotSpot, TRUE);
		if (pHotSpot->m_idEnd[0])
			JumpToRandomAnim(pHotSpot->m_idEnd);
	}
	return(FALSE);
}


BOOL CToon::SetTimer( TimerTypes type, UINT uTimeOut)
{
	if (m_idTimer)
	{
		::KillTimer(m_hWnd, m_idTimer);
		m_idTimer = 0;
	}
	if (type != NoTimer)
		m_idTimer = ::SetTimer( m_hWnd, type, uTimeOut, NULL );
	return(m_idTimer != 0);
}

BOOL CToon::RealizePalette(BOOL bForceBackground)
{
	HTOON hToon = GetToonHandle();
    if (hToon == NULL)
		return(FALSE);
	HDC hDC = GetDC(m_hWnd);
	if (!hDC)
		return(FALSE);
	HPALETTE hOldPal = SelectPalette(hDC, m_hToonPal, bForceBackground);
	int nEntriesChanged = ::RealizePalette(hDC);
	SelectPalette(hDC, hOldPal, TRUE);
	ReleaseDC(m_hWnd, hDC);
	
	if (nEntriesChanged)
	{
		UpdateArea(NULL);
		// invalidate all child windows except for toon control
		HWND hChild = ::GetWindow( GetParent(m_hWnd), GW_CHILD );
		while (hChild)
		{
			if ( IsWindowVisible(hChild) && (hChild != m_hWnd) )
				InvalidateRect( hChild, NULL, FALSE );
			hChild = ::GetWindow( hChild, GW_HWNDNEXT );
		}
		return(TRUE);
	}
	return(FALSE);
}

#ifdef UNUSED
BOOL CToon::OnQueryNewPalette(HWND hWnd)
{
	return(RealizePalette(FALSE));
}
  
void CToon::OnPaletteChanged(HWND hWnd, HWND hwndPaletteChanged)
{          
	// in this case the hWnd passed in is for the main application
	// the window we really want to compare against is our
	// window that we passed to Wintoon
	if (m_hWnd != hwndPaletteChanged)
		RealizePalette(TRUE);
}
#endif

BOOL CToon::OnQueryNewPalette(HWND hWnd)
{
	int i;
	HTOON hToon = GetToonHandle();
    if (hToon != NULL)
	{
		i = ToonRealizePalette(hToon);
		if (i || TRUE)
		{
			UpdateArea(NULL);
			// invalidate all child windows except for toon control
			HWND hChild = ::GetWindow( GetParent(m_hWnd), GW_CHILD );
			while (hChild)
			{
				if ( IsWindowVisible(hChild) && (hChild != m_hWnd) )
					InvalidateRect( hChild, NULL, FALSE );
				hChild = ::GetWindow( hChild, GW_HWNDNEXT );
			}
			return(TRUE);
		}
	}
	return(FALSE);
}
  
void CToon::OnPaletteChanged(HWND hWnd, HWND hwndPaletteChanged)
{          
	// in this case the hWnd passed in is for the main application
	// the window we really want to compare against is our
	// window that we passed to Wintoon
	HTOON hToon = GetToonHandle();
	if (hToon != NULL && (m_hWnd != hwndPaletteChanged))
	{
		if (ToonRealizePalette(hToon))
		{
			UpdateArea(NULL);
		}
	}
	// invalidate all child windows except for toon control
	HWND hChild = ::GetWindow( GetParent(m_hWnd), GW_CHILD );
	while (hChild)
	{
		if ( IsWindowVisible(hChild) && (hChild != m_hWnd) )
			InvalidateRect( hChild, NULL, FALSE );
		hChild = ::GetWindow( hChild, GW_HWNDNEXT );
	}
}

int bound(int v, int l, int u)
{
	if (v < l)
		v = l;
	else
	if (v > u)
		v = u;
	return(v);
}

void CToon::OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	SHOTID idHotSpot;                                                 

	m_idHintHotSpot = 0; // turn off hinting on button down

	// can always stop a looping toon
	PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
//	if (!m_fPlaying || (pHotSpot && (pHotSpot->m_bLoop || pHotSpot->m_bIdle || pHotSpot->m_bCanStop)))
	{
		if (idHotSpot = FindHotSpot(x, y))
		{
			PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
			if (pHotSpot->m_bButton)
			{
				if (pHotSpot->m_bAutoHighlight)
				{
					m_idButtonHotSpot = idHotSpot;
					SetCapture(m_hWnd);
					DrawHotSpot(idHotSpot, TRUE);
				}
				else
				if (idHotSpot == m_idButtonHotSpot && pHotSpot->m_idCommand)
					FORWARD_WM_COMMAND(GetParentDlg(), pHotSpot->m_idCommand, m_hWnd, 0, SendMessage);
				return; // not sending to parent in this case
			}		   
			else
			{
				if (m_fPlaying)
					StopPlaying();
				if (m_bIdleSprites)
					StopIdleSprites();
				if (OnNotify(CToon::ClickedOnHotSpot))
				{
					m_PlayingState = CToon::Normal;
					OnHotSpot(idHotSpot);
				}
			}
		}
		else
		if (m_fSendMouseToParent)
		{
			FORWARD_WM_LBUTTONDOWN(GetParentDlg(), fDoubleClick, x, y, keyFlags, SendMessage);
			return;
		}
	}
}       

#ifdef UNUSED
	if (SHIFT)
	{
		STRING szString;
		static BOOL fDib = FALSE;
		FNAME szFileName;


		if (fDib)
			lstrcpy(szFileName, "harbor\\harbor2.dib");
		else
			lstrcpy(szFileName, "harbor\\harbor2.bmp");
		fDib = !fDib;
			
		DWORD dwTime = timeGetTime();
		delete m_pBackgroundDib;
		m_pBackgroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, TRUE);
		if (ToonIsTopDown(GetToonHandle()) != (m_pBackgroundDib->GetHeight() < 0))
			m_pBackgroundDib->DibFlip();
    
	   	BuildStage();                   
		ToonSetBackground(GetToonHandle(), m_pStageDib->GetInfoHeader(), m_pStageDib->GetPtr());
		ToonRestoreBackground(GetToonHandle());
		InvalidateRect(m_hWnd, NULL, FALSE);
		UpdateWindow(m_hWnd);
		dwTime = timeGetTime() - dwTime;
		wsprintf(szString, "Changing background '%s' took %ld milliseconds\n", (LPSTR)szFileName, dwTime);
		//OutputDebugString(szString);

		HWND hFocusWindow = GetFocus();
		MessageBox( NULL/*MessageParent*/, szString, "Timing Tests", MB_OK | MB_TASKMODAL );
		SetFocus( hFocusWindow );

		//LPTR lp = m_pStageDib->GetXY(x, y);
		//WORD wValue = (WORD)*lp;
		//Print("x = %d y = %d wValue = %d", x, y, wValue);
	}
	else
		else
		{
			int i;
			RGBQUAD rgbQuad[236];
			PALETTEENTRY palEntry[236];
			HPALETTE hPal = GetPalette();

			GetPaletteEntries(hPal, 10, 236, (LPPALETTEENTRY)palEntry);
			++m_iAnimate;
			for (i = 0; i < 236; ++i)
			{
				rgbQuad[i].rgbRed = bound(palEntry[i].peRed-m_iAnimate, 0, 255);
				rgbQuad[i].rgbGreen = bound(palEntry[i].peGreen-m_iAnimate, 0, 255);
				rgbQuad[i].rgbBlue = bound(palEntry[i].peBlue-m_iAnimate, 0, 255);
				rgbQuad[i].rgbReserved = 0;
			}
			ToonAnimateColors(GetToonHandle(), rgbQuad, 10, 236);
		}
#endif

void CToon::OnLButtonUp(int x, int y, UINT keyFlags)
{
	if (m_idButtonHotSpot)
	{
		ReleaseCapture();
		DrawHotSpot(m_idButtonHotSpot, FALSE);
		SHOTID idHotSpot = FindHotSpot(x, y);
		PHOTSPOT pHotSpot = GetHotSpot(m_idButtonHotSpot);
		if (idHotSpot == m_idButtonHotSpot && pHotSpot->m_idCommand)
			FORWARD_WM_COMMAND(GetParentDlg(), pHotSpot->m_idCommand, m_hWnd, 0, SendMessage);
		m_idButtonHotSpot = 0;
	}
	else
	if (m_fSendMouseToParent)
		FORWARD_WM_LBUTTONUP(GetParentDlg(), x, y, keyFlags, SendMessage);
}

void CToon::OnMouseMove(int x, int y, UINT keyFlags)
{
	SHOTID idHotSpot = FindHotSpot(x, y);
	if (m_idButtonHotSpot && !m_idBrowseHotSpot)
	{
		DrawHotSpot(m_idButtonHotSpot, idHotSpot == m_idButtonHotSpot);
	}
	else
	{
		// are we setting to a new hotspot for hinting or 
		// the same old one
		if ((!idHotSpot || (idHotSpot != m_idLastHintHotSpot)) && m_fHintsOn)
		{
			m_idLastHintHotSpot = 0;
			// see if we need to reset the hint
			PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
			BOOL fStoppable = (!m_fPlaying || (pHotSpot && (pHotSpot->m_bLoop || pHotSpot->m_bIdle || pHotSpot->m_bCanStop)));
			if (idHotSpot != m_idHintHotSpot || !m_idHintHotSpot || !fStoppable)
			{
				PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
				if (pHotSpot && lstrlen(pHotSpot->m_szHintWave) && fStoppable)
				{
					m_idHintHotSpot = idHotSpot; // yes
					m_dwHintTime = timeGetTime();
				}
				else
				{
					m_idHintHotSpot = 0; // no
				}
			}
		}
		PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
		if (pHotSpot && pHotSpot->m_bBrowse)
		{
			if (idHotSpot != m_idBrowseHotSpot)
			{
				if (m_idBrowseHotSpot)
					BuildStage(m_idBrowseHotSpot, CToon::UpdateHotSpot, TRUE);
				DrawHotSpot(idHotSpot, FALSE);
				m_idBrowseHotSpot = idHotSpot;
				//SetCapture(m_hWnd);
			}
		}
		else
		{
		 	if (m_idBrowseHotSpot)
			{
		 		BuildStage(m_idBrowseHotSpot, CToon::UpdateHotSpot, TRUE);
				m_idBrowseHotSpot = 0;
				ReleaseCapture();
			}
		}
		if (m_fSendMouseToParent)
			FORWARD_WM_MOUSEMOVE(GetParentDlg(), x, y, keyFlags, SendMessage);
	}
}

BOOL CToon::OnSetCursor(HWND hWndCursor, UINT codeHitTest, UINT msg)
{
	if (hWndCursor == m_hWnd && m_hHotSpotCursor) // in our window
	{
		POINT CursorPos;	   

		GetCursorPos( &CursorPos );
		ScreenToClient( m_hWnd, &CursorPos );
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		if ( (lpScene && lpScene->IsOnHotSpot(&CursorPos)) ||
			 (FindHotSpot(CursorPos.x, CursorPos.y) > 0) )
		{
			SetCursor(m_hHotSpotCursor);
			return(TRUE);
		}
	}
	return(FALSE);
}

UINT CToon::OnMCINotify(UINT message, WORD wDeviceID)
{
	STRING szString;

	wsprintf(szString, "OnMCINotify - message = %d\n", message);
	//OutputDebugString(szString);

	HTOON hToon = GetToonHandle();
	if (hToon && m_fPlaying) 
	{
		PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
		wsprintf(szString, "'%s'\nDropped %ld frames out of %ld (%ld%%)\n",
				(LPSTR)m_szAVIFile,
				m_lTotalDropped,
				pHotSpot->m_dwTo-pHotSpot->m_dwFrom+1,
				(m_lTotalDropped*100)/(pHotSpot->m_dwTo-pHotSpot->m_dwFrom+1));
		OutputDebugString(szString);

		#ifdef TIME_TEST
		HWND hFocusWindow = GetFocus();
		MessageBox( NULL/*MessageParent*/, szString, "Timing Tests", MB_OK | MB_TASKMODAL );
		SetFocus( hFocusWindow );
		#endif

		if (message == MCI_NOTIFY_SUCCESSFUL)
			HandleAnimationDone();
	}

	return TRUE;
}		

void CToon::HandleAnimationDone()
{
	PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);

	if (pHotSpot->m_bLoop || (pHotSpot->m_bIdle && !pHotSpot->m_idEnd[0])) // loop it
	{
		// don't disable cursor for looping animation
		EnableCursor(TRUE);
		m_lLastFrame = (LONG)pHotSpot->m_dwFrom-1;
		m_lTotalDropped = 0;
		if (m_hSound)
			MCIPlay(m_hSound, m_hWnd, pHotSpot->m_dwFrom, pHotSpot->m_dwTo);
		else
			MCIPlay(m_hMovie, m_hWnd, pHotSpot->m_dwFrom, pHotSpot->m_dwTo);
	}
	else
	{
		if (m_hSound)
		{
			MCIClose(m_hSound);
			m_hSound = NULL;
		}

		// change states of sprites based on commands for current sprite
		ChangeSpriteStates(pHotSpot, TRUE);

		// it is likely that conclusion will destroy the
		// dialog and in the process destroy the CToon.  This
		// means that we can't access CToon after the OnNotify
		// or we may crash.  It all depends on the timing.
		if (pHotSpot->m_idCommand)
			FORWARD_WM_COMMAND(GetParentDlg(), pHotSpot->m_idCommand, m_hWnd, 0, SendMessage);
		else
		if (m_PlayingState == CToon::Conclusion)
			OnNotify(CToon::ConclusionDone);
		else
		{
			if (m_PlayingState == CToon::Auto)
			{
				// don't send an end until we finish playing all
				// of animations involved in intro
				if (!pHotSpot->m_idEnd[0])
				{
					OnNotify(CToon::AutoDone);
					// turn on hints once we've played the entire intro
					m_fHintsOn = TRUE;
				}
			}
			else // CToon::PlayNormal
				OnNotify(CToon::ToonDone);
              
            if (!m_fPlaying) // OnNotify's could have started an animation
			{
            	if (CheckSequence(pHotSpot->m_idSequence, pHotSpot->m_nSequence))
            		OnHotSpot(pHotSpot->m_idSequence);
				else
				{
					if (pHotSpot->m_idEnd[0]) // jump to
					{
						BOOL fTimerSet = FALSE;

						if (pHotSpot->m_uEndDelay)
							fTimerSet = SetTimer(CToon::EndDelayTimer, pHotSpot->m_uEndDelay);
						if (!fTimerSet)
							JumpToRandomAnim(pHotSpot->m_idEnd);
					}
					else
						StartIdle();
				}
			}
		}
	}
}

void CToon::OnTimer(UINT idTimer)
{
	if (idTimer == ANIMATOR_TIMER_ID)
	{
		// we use the animation engine timer id for hints
		if (m_idHintHotSpot && ((timeGetTime() - m_dwHintTime) >= HINT_TIME))
		{
			FNAME szFileName;
			BOOL fPlayed = FALSE;

			PHOTSPOT pHotSpot = GetHotSpot(m_idHintHotSpot);
			GetModuleFileName(m_hInstance, szFileName, sizeof(szFileName));
			StripFile(szFileName);
			lstrcat(szFileName, COMMON_DIR);
			lstrcat(szFileName, pHotSpot->m_szHintWave);
			LPSCENE lpScene = GetApp()->GetCurrentScene();
			if (lpScene)
			{
				OutputDebugString("PlayHint\n");
				if (m_fPlaying)
					StopPlaying();
				fPlayed = lpScene->PlayHint(szFileName);
				StartIdleAnimation();
			}
			if (fPlayed)
			{
				m_idLastHintHotSpot = m_idHintHotSpot;
				m_idHintHotSpot = 0;
			}
			else
				m_dwHintTime = timeGetTime();
		}
	}
	else
	{
		SetTimer(/* RESET */);
		if (idTimer == CToon::AutoPlayTimer)
			PlayAuto(TRUE);
		else
		if (idTimer == CToon::EndDelayTimer)
		{
			PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
			if (pHotSpot)
				JumpToRandomAnim(pHotSpot->m_idEnd);
		}
	}
}


BOOL CToon::JumpToRandomAnim(SHOTID FAR *lpAnimID)
{
	int i;
	int nAnims = 0;
	SHOTID AnimID;

	for (i = 0; i < MAX_SHOTVALUES; ++i)
	{
		if (lpAnimID[i])
			++nAnims;
	}
	if (nAnims > 1)
	{
		WORD wRandom = GetRandomNumber(nAnims);
		AnimID = lpAnimID[wRandom];
	}
	else
		AnimID = lpAnimID[0];
	return(OnHotSpot(AnimID));
}

void CToon::ChangeSpriteStates(PHOTSPOT pHotSpot, BOOL fNormalCompletion, BOOL fEventTrigger)
{	
	int i;
	PHOTSPOT pOtherHotSpot;
	BOOL bRebuild = TRUE;
	BOOL bUpdateDisplay = TRUE;
	BOOL bEnableCursor;

	TriggerEvents(pHotSpot->m_idHotSpot);

	// set initial state of rebuild flag
 	bRebuild = !pHotSpot->m_bNoUndraw &&			// not flagged as noundraw and
 			   !lstrlen(pHotSpot->m_szWaveFile) &&	// not a wave file clickable and
			   (pHotSpot->m_dwTo > pHotSpot->m_dwFrom) && // we have some animation frames
			   !fEventTrigger; // not for an event trigger

	// if this is a stop and not a normal completion
	// then check to see if we have any special things
	// to be handled for a stop.
	if (!fNormalCompletion)
	{
		// handle shows to be done on a stop
		if (pHotSpot->m_idStopShow[0])
		{
			for (i = 0; i < MAX_SHOTVALUES; ++i)
			{
				pOtherHotSpot = GetHotSpot(pHotSpot->m_idStopShow[i]);
				if (!pOtherHotSpot)
					break;
				pOtherHotSpot->m_bVisible = TRUE;
				if (!bRebuild)
					BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, TRUE /*!fEventTrigger*/);
			}
		}
	}
	// handle show command
	if (pHotSpot->m_idShow[0])
	{
		for (i = 0; i < MAX_SHOTVALUES; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_idShow[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bVisible = TRUE;
			if (!bRebuild)
				BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, TRUE /*!fEventTrigger*/);
		}
	}
	// handle hide command
	if (pHotSpot->m_idHide[0])
	{
		for (i = 0; i < MAX_SHOTVALUES; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_idHide[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bVisible = FALSE;
			if (!bRebuild)
				// we pass fUpdateDisplay parameter based on whether this
				// is a triggered event.  The assumption is that the playing
				// of the video will handle the updating of the display.
				BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, !fEventTrigger);
		}
	}
	// handle enable command
	if (pHotSpot->m_idEnable[0])
	{
		for (i = 0; i < MAX_SHOTVALUES; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_idEnable[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bEnabled = TRUE;
		}
	}
	// handle disable command
	if (pHotSpot->m_idDisable[0])
	{
		for (i = 0; i < MAX_SHOTVALUES; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_idDisable[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bEnabled = FALSE;
		}
	}

	// no rebuild for event trigger
	if (fEventTrigger)
		return;

	// Determine cursor state:
	// If we have a normal completion and this hotspot has
	// another to jump to then leave the cursor disabled
	// otherwise use the state from the jump table
	if (fNormalCompletion && pHotSpot->m_idEnd[0])
		bEnableCursor = FALSE;
	else
		bEnableCursor = pHotSpot->m_bEnableCursor;
	if (bEnableCursor)
		EnableCursor(TRUE);

	m_fPlaying = FALSE;
	if (bRebuild)
	{
		// we don't need to update the display for an idle animation
		// that jumps to another animation
		// this is used to implement random idle animations, where
		// you randomly jump to an idle animation, and you don't
		// want the screen continuously updated.
		if (fNormalCompletion && pHotSpot->m_bIdle && pHotSpot->m_idEnd[0])
			bUpdateDisplay = FALSE;

		// rebuild the stage and update display if necessary
		BuildStage(0 /* Hide Nothing */, CToon::HideHotSpot, bUpdateDisplay);
	}
}

void CToon::OnPaint()
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rRepair;
	BOOL fDidInitialize = FALSE;

	HTOON hToon = GetToonHandle();	
	if ((hDC = BeginPaint(m_hWnd, &ps)) != NULL)
	{
		rRepair = ps.rcPaint;
		if (!IsRectEmpty(&rRepair))
		{
			if (!m_fInitialized)
			{
				Initialize(hDC);
				fDidInitialize = TRUE;
			}

			if (m_Transition.m_uTransition)
			{
				PDIB pWinGDib;
				HPALETTE hPal, hOldPal;
				TRANSPARMS parms;

				pWinGDib = GetWinGDib();
				if (pWinGDib)
				{
					hPal = CreateIdentityPalette(pWinGDib->GetColors(), 256);
					if (hPal)
					{
						hOldPal = SelectPalette(hDC, hPal, FALSE);
						::RealizePalette(hDC);
					}
					if (!IsRectEmpty(&m_rGlobalClip) && !m_bNoTransitionClip)
						IntersectRect(&rRepair, &rRepair, &m_rGlobalClip);
					SetTransitionParms( &parms, (int)m_Transition.m_uTransition, hDC,
							rRepair.left, rRepair.top,
							rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
							rRepair.left, rRepair.top,
							rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, pWinGDib, ToonDC(m_hToon) );
					SetTransitionTiming(&parms, m_Transition.m_nStepSize, m_Transition.m_nTicks, m_Transition.m_dwTotalTime);
					if (m_fSoundStarted)
					{
						m_dwStartVol = MCIGetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO);
						SetTransitionCallback(&parms, ::TransCallback, (DWORD)this);
					}
					TransitionBlt(&parms);
					if (m_fSoundStarted)
					{
						LPSCENE lpScene = GetApp()->GetCurrentScene();
						lpScene->GetSound()->StopChannel(0);
						MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, LOWORD(m_dwStartVol), HIWORD(m_dwStartVol));
						m_fSoundStarted = FALSE;
					}

					if (hPal)
					{
						hOldPal = SelectPalette(hDC, hOldPal, TRUE);
						DeleteObject(hPal);
					}
				}
				m_Transition.m_uTransition = 0;
				//GetApp()->m_bAppNoErase = NO;
			}
			else
			if (hToon)
			{
				if (TRUE)
					ToonPaintDC(hToon, hDC);
				else
				{
					int x = ps.rcPaint.left;
					int y = ps.rcPaint.top;
					int iWidth = ps.rcPaint.right - ps.rcPaint.left;
					int iHeight = ps.rcPaint.bottom - ps.rcPaint.top;
					HDC hWinGDC = ToonDC(hToon);
					HPALETTE hOldPal = SelectPalette(hDC, m_hToonPal, FALSE);
					::RealizePalette(hDC);
					WinGBitBlt(hDC, x, y, iWidth, iHeight, hWinGDC, x, y);
					//ToonPaintDC(hToon, hDC);
					SelectPalette(hDC, hOldPal, TRUE);
				}
			}
		}
		EndPaint(m_hWnd, &ps);
		if (fDidInitialize)
			EndInitialize();
	}
}

BOOL CToon::OnToonDraw(HTOON hToon, LONG lFrame)
{
	//STRING szString;
	//wsprintf(szString, "OnToonDraw - lFrame = %ld\n", lFrame);
	//OutputDebugString(szString);
	if ((lFrame-m_lLastFrame) != 1)
	{
		long lDropped = lFrame - m_lLastFrame - 1;
		if (lDropped > 0)
			m_lTotalDropped += lDropped;
		//wsprintf(szString, "LastFrame = %ld CurrentFrame = %ld\n",
		//			m_lLastFrame, lFrame);
		//OutputDebugString(szString);
	}
	m_lLastFrame = lFrame;
	ToonDrawCurrentFrame(hToon);

	// check to see if any events need to be fired
	TriggerEvents(m_idCurHotSpot, (DWORD)lFrame);

	return(TRUE);
}

void CToon::FadeOut()
{
	RGBQUAD rgbQuad[ANIMATE_ENTRIES];

	// We want to animate from the current palette to black
	//HDC hDC = GetDC(m_hWnd);
	//GetSystemPaletteEntries(hDC, ANIMATE_START, ANIMATE_ENTRIES, (LPPALETTEENTRY)rgbQuad);
	//ReleaseDC(m_hWnd, hDC);

	ToonGetColors(m_hToon, ANIMATE_START, ANIMATE_ENTRIES, rgbQuad);

	int iCount = 256 / FADE_INC;
	DWORD dwTime = timeGetTime();
	DWORD dwLastTime = dwTime - FADE_TIME;
	while (--iCount >= 0)
	{
		while ((dwTime - dwLastTime) < FADE_TIME)
			dwTime = timeGetTime();
		dwLastTime = dwTime;
		
		for (int i = 0; i < ANIMATE_ENTRIES; ++i)
		{
			if (rgbQuad[i].rgbRed > FADE_INC)
				rgbQuad[i].rgbRed -= FADE_INC;
			else
				rgbQuad[i].rgbRed = 0;
			if (rgbQuad[i].rgbGreen > FADE_INC)
				rgbQuad[i].rgbGreen -= FADE_INC;
			else
				rgbQuad[i].rgbGreen = 0;
			if (rgbQuad[i].rgbBlue > FADE_INC)
				rgbQuad[i].rgbBlue -= FADE_INC;
			else
				rgbQuad[i].rgbBlue = 0;
		}
		ToonAnimateColors(m_hToon, rgbQuad, ANIMATE_START, ANIMATE_ENTRIES);
	}
}

void CToon::FadeIn()
{
	RGBQUAD rgbQuad[ANIMATE_ENTRIES];
	LPRGBQUAD lpColors;
	int iCount, i;
	DWORD dwTime, dwLastTime, dwVol;
	WORD wVol, wInc;
	STRING szString;

	lpColors = m_pStageDib->GetColors();
	lpColors += ANIMATE_START;
	iCount = 256 / FADE_INC;
	dwTime = timeGetTime();
	dwLastTime = dwTime - FADE_TIME;
	if (m_fSoundStarted)
	{
		dwVol = MCIGetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO);
		wVol = LOWORD(dwVol);
		wInc = wVol / iCount;
	}
	while (--iCount >= 0)
	{
		while ((dwTime - dwLastTime) < FADE_TIME)
			dwTime = timeGetTime();
		dwLastTime = dwTime;
		
		for (i = 0; i < ANIMATE_ENTRIES; ++i)
		{
			int v;

			v = (int)lpColors[i].rgbRed - (iCount * FADE_INC);
			if (v < 0)
				v = 0;
			rgbQuad[i].rgbRed = v;
			v = (int)lpColors[i].rgbGreen - (iCount * FADE_INC);
			if (v < 0)
				v = 0;
			rgbQuad[i].rgbGreen = v;
			v = (int)lpColors[i].rgbBlue - (iCount * FADE_INC);
			if (v < 0)
				v = 0;
			rgbQuad[i].rgbBlue = v;
		}
		ToonAnimateColors(m_hToon, rgbQuad, ANIMATE_START, ANIMATE_ENTRIES);
		if (m_fSoundStarted)
		{
			wVol -= wInc;
			wsprintf(szString, "dwVol = %lx wVol = %x\n", dwVol, wVol);
			OutputDebugString(szString);
			MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, wVol, wVol);
		}
	}
	if (m_fSoundStarted)
	{
		OutputDebugString("Calling StopAndFree\n");
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		lpScene->GetSound()->StopChannel(0);
		MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, LOWORD(dwVol), HIWORD(dwVol));
		m_fSoundStarted = FALSE;
	}
	ToonSetColors(GetToonHandle(), m_pStageDib->GetColors());
	UpdateWinGDib();
}

void CToon::DrawHotSpot(SHOTID idHotSpot, BOOL bHighlight)
{
	RECT rArea;
	PDIB pDib;

	PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
	if (!pHotSpot)
		return;
	pHotSpot->GetEraseRect(&rArea, TRUE);
	if (IsRectEmpty(&rArea))
		return;
	if (!m_pStageDib)
		return;
	if (bHighlight && pHotSpot->m_pHighlightDib)
		pDib = pHotSpot->m_pHighlightDib;
	else
		pDib = pHotSpot->m_pForegroundDib;
	if (pDib)
		pDib = pDib->GetDib();
	if (!pDib)
		return;

	if (m_pBackgroundDib)
	{
		m_pBackgroundDib->DibBlt(m_pStageDib,
							rArea.left, rArea.top,
							rArea.right-rArea.left, rArea.bottom-rArea.top,
							rArea.left, rArea.top,
							rArea.right-rArea.left, rArea.bottom-rArea.top,
							FALSE);
		pDib->DibBlt(m_pStageDib,
					rArea.left, rArea.top,
					rArea.right-rArea.left, rArea.bottom-rArea.top,
					0, 0,
					rArea.right-rArea.left, rArea.bottom-rArea.top,
					TRUE);
		RestoreBackgroundArea(&rArea, TRUE);
	}
	else
	{
		PDIB pWinGDib = GetWinGDib();
		m_pStageDib->DibBlt(pWinGDib,
							rArea.left, rArea.top,
							rArea.right-rArea.left, rArea.bottom-rArea.top,
							rArea.left, rArea.top,
							rArea.right-rArea.left, rArea.bottom-rArea.top,
							FALSE);
	  	pDib->DibBlt(pWinGDib,
	  				rArea.left, rArea.top,
	  				rArea.right-rArea.left, rArea.bottom-rArea.top,
   	  				0, 0,
   	  				rArea.right-rArea.left, rArea.bottom-rArea.top,
   	  				TRUE);
		UpdateArea(&rArea);
	}
  	pDib->Release();
}

void CToon::StartIdleSprites()
{
	if (!m_pAnimator)
		return;
	// see if we have an idle soundtrack
	if (lstrlen(m_szWaveFile))
	{
		FNAME szFileName;

		lstrcpy(szFileName, m_szPath);
		lstrcat(szFileName, m_szWaveFile);
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		if (lpScene && lpScene->GetSound())
			lpScene->GetSound()->StartFile(szFileName, TRUE, 0);
	}
	for (int id = 1; id <= m_nHotSpots; ++id)
	{
		RECT rErase;
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bIdle &&
			pHotSpot->m_bSprite &&
			pHotSpot->m_lpSprite &&
			pHotSpot->GetEraseRect(&rErase))
		{
			// remove this sprite from the stage dib for the toon engine
			BuildStage(id);
			// make sure we always start at first cell
			pHotSpot->m_lpSprite->SetCurrentCell(0);
			// show the sprite using the sprite engine
			pHotSpot->m_lpSprite->Show(TRUE);
			// set initial pause time, once sprite kicks it
			// the command actions will pause after displaying
			// the last cell
			pHotSpot->m_lpSprite->Pause(pHotSpot->m_nPauseTime);
		}
	}
}

void CToon::StopIdleSprites()
{
	if (!m_pAnimator)
		return;
	for (int id = 1; id <= m_nHotSpots; ++id)
	{
		RECT rErase;
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bIdle &&
			pHotSpot->m_bSprite &&
			pHotSpot->m_lpSprite &&
			pHotSpot->GetEraseRect(&rErase))
		{
			// add this sprite back to the stage dib for the toon engine
			BuildStage(id, CToon::ShowHotSpot);
			// hide the sprite using the sprite engine
			pHotSpot->m_lpSprite->Show(FALSE);
			pHotSpot->m_lpSprite->Pause(PAUSE_FOREVER);
		}
	}
	// stop any idle soundtrack
	if (lstrlen(m_szWaveFile))
	{
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		if (lpScene && lpScene->GetSound())
			lpScene->GetSound()->Stop();
	}
}

void CToon::UpdateArea(LPRECT lpRect)
{
	InvalidateRect(m_hWnd, lpRect, FALSE);
	UpdateWindow(m_hWnd);
	// these functions should work, however, they update the entire window
	//ToonMarkRect(GetToonHandle(), &rArea);
	//ToonPaintRects(GetToonHandle());
}

LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
{
	if (dwNotifyData)
	{
		PTOON pToon = (PTOON)dwNotifyData;
		pToon->OnSpriteNotify(lpSprite, Notify);
	}
}

void CToon::OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
{
	if (Notify == SN_PAUSEAFTER)
		HandleAnimationDone();
}

BOOL CToon::MoveHotSpot(LPCTSTR lpForegroundDib, int x, int y, BOOL fRelative)
{
	RECT rErase;
	PHOTSPOT pHotSpot;
	int id;

	id = FindHotSpot(lpForegroundDib);
	if (!id)
		return(FALSE);

	pHotSpot = GetHotSpot(id);
	if (!pHotSpot)
		return(FALSE);

	BuildStage(id);
	if (pHotSpot->GetEraseRect(&rErase))
		UpdateArea(&rErase);
	if (pHotSpot->m_ptPos.x != -1)
	{
		if (fRelative)
		{
			pHotSpot->m_ptPos.x += x;
			pHotSpot->m_ptPos.y += y;
		}
		else
		{
			pHotSpot->m_ptPos.x = x;
			pHotSpot->m_ptPos.y = y;
		}
	}
	else
	if (!IsRectEmpty(&pHotSpot->m_rHot))
	{
		if (fRelative)
			OffsetRect(&pHotSpot->m_rHot, x, y);
		else
		{
			int iWidth = pHotSpot->m_rHot.right - pHotSpot->m_rHot.left;
			int iHeight = pHotSpot->m_rHot.bottom - pHotSpot->m_rHot.top;
			SetRect(&pHotSpot->m_rHot, x, y, x+iWidth, y+iHeight);
		}
	}
	// add this sprite back to the stage dib for the toon engine
	BuildStage(id, CToon::ShowHotSpot);
	if (pHotSpot->GetEraseRect(&rErase))
		UpdateArea(&rErase);
	return(TRUE);
}

BOOL CToon::StartIdle()
{
	if (m_bIdleSprites)
	{
		StartIdleSprites();
		return(TRUE);
	}
	else
	{
		return(StartIdleAnimation());
	}
}

BOOL CToon::StartIdleAnimation()
{
	for (int id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bIdle && !pHotSpot->m_bSprite)
		{
			m_PlayingState = CToon::Normal;
			return(OnHotSpot(id));
		}
	}
	return(FALSE);
}

static BOOL fDummy;

void CToon::TriggerEvents(SHOTID idHotSpot, DWORD dwFrame)
{
	PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
	if (pHotSpot)
	{
		for (int i = 0; i < MAX_EVENTS; ++i)
		{
			if (!pHotSpot->m_AnimEvents[i].dwFrame)
				break;
			
			if (dwFrame >= pHotSpot->m_AnimEvents[i].dwFrame &&
				!pHotSpot->m_AnimEvents[i].bTriggered)
			{
				PHOTSPOT pTriggerHotSpot = GetHotSpot(pHotSpot->m_AnimEvents[i].idHotSpot);
				if (pTriggerHotSpot)
					ChangeSpriteStates(pTriggerHotSpot, TRUE, TRUE);
				pHotSpot->m_AnimEvents[i].bTriggered = TRUE;
			}
		}
	}
}

BOOL CToonParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
{
	PTOON pToon = (PTOON)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "left"))
			GetTransitionDesc(&pToon->m_LeftTransition, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "right"))
			GetTransitionDesc(&pToon->m_RightTransition, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "jump"))
			GetTransitionDesc(&pToon->m_JumpTransition, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "jumpscene"))
			GetTransitionDesc(&pToon->m_SceneTransition, lpEntry, lpValues, nValues, &pToon->m_nJumpSceneNo);
		else
		if (!lstrcmpi(lpKey, "clip"))
			GetRect(&pToon->m_rGlobalClip, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "toon"))
			GetFilename(pToon->m_szAVIFile, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "wave"))
			GetFilename(pToon->m_szWaveFile, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "pos"))
			GetPoint(&pToon->m_ptMovie, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "delay"))
			GetInt(&pToon->m_iAutoPlayDelay, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hints"))
			GetHintState(pToon, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetSoundTrack(pToon, lpEntry, lpValues, nValues);
		else		  
		if (!lstrcmpi(lpKey, "keepsoundtrack"))
			GetKeepSoundTrack(pToon, lpEntry, lpValues, nValues);
		else		  
			Print("'%s'\n Unknown key '%s'", lpEntry, lpKey);
	}
	else
	{
		PHOTSPOT pHotSpot = pToon->GetHotSpot(nIndex);
		if (!pHotSpot)
			return(TRUE);

		// see if we have an id yet
		if (!pHotSpot->m_idHotSpot)
			GetAnimID(pHotSpot, lpEntry, lpKey, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "show"))
			GetMultipleShots(pHotSpot->m_idShow, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hide"))
			GetMultipleShots(pHotSpot->m_idHide, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "prehide"))
			GetMultipleShots(pHotSpot->m_idPreHide, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "preshow"))
			GetMultipleShots(pHotSpot->m_idPreShow, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "enable"))
			GetMultipleShots(pHotSpot->m_idEnable, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "disable"))
			GetMultipleShots(pHotSpot->m_idDisable, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "setsequence"))
			GetSetSequence(pHotSpot, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "sequence"))
			GetInt(&pHotSpot->m_nSequenceNum, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "end"))
			GetEnd(pHotSpot, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "flags"))
			GetFlags(pHotSpot, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hot"))
			GetRectOrPoint(&pHotSpot->m_rHot, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "pos"))
			GetPoint((LPPOINT)&pHotSpot->m_ptPos, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "clip"))
			GetRect(&pHotSpot->m_rClip, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "fg"))
			GetFilename(pHotSpot->m_szForegroundDib, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "wave"))
			GetFilename(pHotSpot->m_szWaveFile, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hint"))
			GetFilename(pHotSpot->m_szHintWave, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "cmd"))
			GetInt((LPINT)&pHotSpot->m_idCommand, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "cells"))
			GetInt(&pHotSpot->m_nCells, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "speed"))
			GetInt(&pHotSpot->m_nSpeed, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "pause"))
			GetInt(&pHotSpot->m_nPauseTime, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "stopshow"))
			GetMultipleShots(pHotSpot->m_idStopShow, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "event"))
			GetEvent(pHotSpot, lpEntry, lpValues, nValues);
		else
			Print("'%s'\n Unknown key '%s'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

BOOL CToonParser::GetAnimID(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	InitAtomTable( 0 );
	InitAtomTable( GetNumEntries() );

	if ( !(l = GetShotID(lpEntry, lpKey)) )
	  	return(FALSE);
	pHotSpot->m_idHotSpot = l;
	if (nValues != 2)
	{
		Print("'%s'\n Expected start and end values", lpEntry);
	  	return(FALSE);
	}

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pHotSpot->m_dwFrom = l;
	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pHotSpot->m_dwTo = l;
	return(TRUE);
}

BOOL CToonParser::GetMultipleShots(SHOTID FAR *lpShotID, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	int i;
	LPSTR lpValue;

	if (nValues > MAX_SHOTVALUES)
	{
		Print("'%s'\n Engine currently only handles %d animation values", lpEntry, MAX_SHOTVALUES);
		return(FALSE);
	}
	
	for (i = 0; i < nValues; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		if ( !(l = GetShotID(lpEntry, lpValue)) )
			return(FALSE);
		lpShotID[i] = l;
	}
	return(TRUE);
}

BOOL CToonParser::GetSetSequence(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (nValues != 2)
	{
		Print("'%s'\n Wrong number of arguments for setsequence command '%s'", lpEntry, lpValues);
		return(FALSE);
	}
	lpValue = GetNextValue(&lpValues);
	if ( !(l = GetShotID(lpEntry, lpValue)) )
		return(FALSE);
	pHotSpot->m_idSequence = l;
	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pHotSpot->m_nSequence = (int)l;
	return(TRUE);
}

BOOL CToonParser::GetEnd(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	int i;

	lpValue = GetNextValue(&lpValues);

	if (!lstrcmpi(lpValue, "loop"))
		pHotSpot->m_bLoop = TRUE;
	else
	{
		if (!lstrcmpi(lpValue, "delay"))
		{
			--nValues;
			lpValue = GetNextValue(&lpValues);
			l = latol(lpValue, &bError);
			if ( bError || l < 0 )
			{
				Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			  	return(FALSE);
			}
			if (l <= 10) // assume seconds specified, otherwise milliseconds
				l *= 1000;
			pHotSpot->m_uEndDelay = (UINT)l;
			lpValue = GetNextValue(&lpValues);
		}

		if (nValues > MAX_SHOTVALUES)
		{
			Print("'%s'\n Engine currently only handles %d animation values", lpEntry, MAX_SHOTVALUES);
			return(FALSE);
		}
	
		for (i = 0; i < nValues; ++i)
		{
			if ( !(l = GetShotID(lpEntry, lpValue)) )
				return(FALSE);
			pHotSpot->m_idEnd[i] = l;
			lpValue = GetNextValue(&lpValues);
		}
	}
	return(TRUE);
}

BOOL CToonParser::GetFlags(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	LPSTR lpValue;

	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		if (!lstrcmpi(lpValue, "auto"))
			pHotSpot->m_bAuto = TRUE;
		else
		if (!lstrcmpi(lpValue, "idle"))
			pHotSpot->m_bIdle = TRUE;
		else
		if (!lstrcmpi(lpValue, "exit"))
			pHotSpot->m_bExit = TRUE;
		else
		if (!lstrcmpi(lpValue, "invisible"))
			pHotSpot->m_bVisible = FALSE;
		else
		if (!lstrcmpi(lpValue, "disabled"))
			pHotSpot->m_bEnabled = FALSE;
		else
		if (!lstrcmpi(lpValue, "noundraw"))
			pHotSpot->m_bNoUndraw = TRUE;
		else
		if (!lstrcmpi(lpValue, "nocursor"))
			pHotSpot->m_bEnableCursor = FALSE;
		else
		if (!lstrcmpi(lpValue, "enablestop"))
			pHotSpot->m_bCanStop = TRUE;
		else
		if (!lstrcmpi(lpValue, "alwayshot"))
			pHotSpot->m_bForceEnable = TRUE;
		else
		if (!lstrcmpi(lpValue, "autobutton"))
		{
			pHotSpot->m_bButton = TRUE;
			pHotSpot->m_bAutoHighlight = TRUE;
		}
		else
		if (!lstrcmpi(lpValue, "button"))
			pHotSpot->m_bButton = TRUE;
		else
		if (!lstrcmpi(lpValue, "sprite"))
			pHotSpot->m_bSprite = TRUE;
		else
		if (!lstrcmpi(lpValue, "browse"))
			pHotSpot->m_bBrowse = TRUE;
		else
		if (!lstrcmpi(lpValue, "highlight"))
			pHotSpot->m_bHighlighted = TRUE;
		else
		if (!lstrcmpi(lpValue, "nofade"))
			pHotSpot->m_bNoFade = TRUE;
		else
		if (!lstrcmpi(lpValue, "alwaysenabled"))
			pHotSpot->m_bAlwaysEnabled = TRUE;
		else
		if (!lstrcmpi(lpValue, "nohide"))
			pHotSpot->m_bNoHide = TRUE;
		else
		if (!lstrcmpi(lpValue, "notrans"))
			pHotSpot->m_bNoTransparency = TRUE;
		else
			Print("'%s'\n Unknown flag value '%s'", lpEntry, lpValue);
	}
	return(TRUE);
}

BOOL CToonParser::GetRectOrPoint(LPRECT lpRect, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	LPINT lpRectValue;
	BOOL bError;

	if (nValues != 4 && nValues != 2)
	{
		Print("'%s'\n Invalid rectangle '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	lpRect->left = lpRect->top = lpRect->right = lpRect->bottom = -1;
	lpRectValue = (LPINT)lpRect;
	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		*lpRectValue++ = (int)l;
	}

 	return(TRUE);
}

BOOL CToonParser::GetTransitionDesc(PTRANSITIONDESC pDesc, LPSTR lpEntry, LPSTR lpValues, int nValues, LPINT lpSceneID)
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (lpSceneID)
	{
		if (nValues != 4 && nValues != 6 && nValues != 7)
		{
			Print("'%s'\n Invalid transition description '%s'", lpEntry, lpValues);
			return(FALSE);
		}
	}
	else
	{
		if (nValues != 3 && nValues != 5 && nValues != 6)
		{
			Print("'%s'\n Invalid transition description '%s'", lpEntry, lpValues);
			return(FALSE);
		}
	}
	if (lpSceneID)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		*lpSceneID = (int)l;  
		--nValues;
	}

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	pDesc->m_fFade = (BOOL)l;
	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	pDesc->m_fClip = (BOOL)l;
	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	pDesc->m_uTransition = (UINT)l;
	if (nValues > 3)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}						    
		pDesc->m_nStepSize = (int)l;
	}
	if (nValues > 4)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		pDesc->m_nTicks = (int)l;
	}
	if (nValues > 5)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		pDesc->m_dwTotalTime = (DWORD)l;
	}
 	return(TRUE);
}

BOOL CToonParser::GetHintState(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (nValues != 1)
	{
		Print("'%s'\n Invalid hint state '%s'", lpEntry, lpValues);
		return(FALSE);
	}
	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	pToon->SetHintState(l != 0);
	return(TRUE);
}

BOOL CToonParser::GetKeepSoundTrack(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (nValues != 1)
	{
		Print("'%s'\n Invalid keepsoundtrack '%s'", lpEntry, lpValues);
		return(FALSE);
	}
	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	pToon->m_fKeepSoundTrack = (l != 0);
	return(TRUE);
}

BOOL CToonParser::GetEvent(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	int nEvents, i;
	
	nEvents = nValues / 2;
	if (nEvents > MAX_EVENTS)
	{
		Print("Too many events - maximum is %d", MAX_EVENTS);
		return(FALSE);
	}
	for (i = 0; i < nEvents; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		  	return(FALSE);
		}
		pHotSpot->m_AnimEvents[i].dwFrame = l;

		lpValue = GetNextValue(&lpValues);
		if ( !(l = GetShotID(lpEntry, lpValue)) )
		  	return(FALSE);
		pHotSpot->m_AnimEvents[i].idHotSpot = l;
	}
	return(TRUE);
}

BOOL CToonParser::GetSoundTrack(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues)
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (nValues != 1)
	{
		Print("'%s'\n Invalid soundtrack '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	if (!GetFilename(pToon->m_szSoundTrack, lpEntry, lpValues, nValues))
		return(FALSE);

	if (lstrcmpi(Extension(pToon->m_szSoundTrack), ".wav"))
	{
		l = latol(pToon->m_szSoundTrack, &bError);
		if ( bError || l <= 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		pToon->m_idSoundTrack = (int)l;
	}
	return(TRUE);
}

// Re-map the video table to make all entries and indices sequential
void CToonParser::RemapTable(PHOTSPOT pHotSpots)
{
	SHOTID lCurShot;
	SHOTID idx, idx2;
	int iNumShots;
	PHOTSPOT pHotSpot, pHotSpot2;

	iNumShots = GetNumEntries();

	// Loop through the shots, and find a shot index to map each the shot id's to
	// Stuff in a negative value, so we know its been changed
	for ( idx = 1; idx <= iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		pHotSpot = &pHotSpots[idx-1];

		for ( idx2 = 1; idx2 <= iNumShots; idx2++ )
		{
			pHotSpot2 = &pHotSpots[idx2-1];

			lCurShot = pHotSpot2->m_idHotSpot;

			ChangeMultipleShots(pHotSpot->m_idEnd, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idShow, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idHide, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idPreHide, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idPreShow, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idEnable, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idDisable, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_idStopShow, lCurShot, -idx2);
			for (int i = 0; i < MAX_EVENTS; ++i)
			{
				if (pHotSpot->m_AnimEvents[i].dwFrame &&
					pHotSpot->m_AnimEvents[i].idHotSpot == lCurShot)
					pHotSpot->m_AnimEvents[i].idHotSpot = -idx2;
			}
			if ( pHotSpot->m_idSequence	== lCurShot )	pHotSpot->m_idSequence = -idx2;
		}
	}

	// Go back and change each shot's id to an index, and check for errors, and negate everything
	for ( idx = 1; idx <= iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		pHotSpot = &pHotSpots[idx-1];

		pHotSpot->m_idHotSpot = idx;

		MultipleShotCheck(pHotSpot->m_idEnd);
		MultipleShotCheck(pHotSpot->m_idShow);
		MultipleShotCheck(pHotSpot->m_idHide);
		MultipleShotCheck(pHotSpot->m_idPreHide);
		MultipleShotCheck(pHotSpot->m_idPreShow);
		MultipleShotCheck(pHotSpot->m_idEnable);
		MultipleShotCheck(pHotSpot->m_idDisable);
		MultipleShotCheck(pHotSpot->m_idStopShow);
		for (int i = 0; i < MAX_EVENTS; ++i)
		{
			if (pHotSpot->m_AnimEvents[i].dwFrame)
				pHotSpot->m_AnimEvents[i].idHotSpot = ShotCheck(pHotSpot->m_AnimEvents[i].idHotSpot);
		}
		pHotSpot->m_idSequence	= ShotCheck( pHotSpot->m_idSequence );
	}
}

void CToonParser::ChangeMultipleShots(SHOTID FAR *lpShotID, SHOTID idCur, SHOTID idNew)
{
	int i;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
	{
		if ( lpShotID[i] == idCur )
			lpShotID[i] = idNew;
	}
}

void CToonParser::MultipleShotCheck(SHOTID FAR *lpShotID)
{
	int i;
	for (i = 0; i < MAX_SHOTVALUES; ++i)
		lpShotID[i] = ShotCheck(lpShotID[i]);
}

