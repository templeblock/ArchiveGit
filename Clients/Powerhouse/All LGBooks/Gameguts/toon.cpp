#include <windows.h>
#include "proto.h"
#include "scene.h"
#include "wintoon.h" 
#include "toon.h" 
#include "mmsystem.h"
#include "transit.h"
#include "parser.h"
#include "wing.h"
#include "malloc.h"
#include "dibfx.h"

#define ToonAlloc _fmalloc
#define ToonFree  _ffree

#define ANIMATE_START 20
#define ANIMATE_ENTRIES 226
#define COMMON_DIR "COMMON\\"
#define CLICK_WAVE "CLICK.WAV"
#define HINT_TIME 2000
#define FADE_INC 4
#define FADE_TIME 10
#define DOWN_WAIT	1500L

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
	BOOL GetMultipleShots(LPSHOTID FAR *lppShotID, LPINT lpNumShots, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetRectOrPoint(LPRECT lpRect, LPSTR lpEntry, LPSTR lpValues, int nValues);
	void ChangeMultipleShots(SHOTID FAR *lpShotID, int nNumShots, SHOTID idCur, SHOTID idNew);
	void MultipleShotCheck(SHOTID FAR *lpShotID, int nNumShots);
	BOOL GetTransitionDesc(PTRANSITIONDESC pTransitionDesc, LPSTR lpEntry, LPSTR lpValues, int nValues, LPINT lpSceneID = NULL);
	BOOL GetEvent(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSoundTrack(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetGlobalFlags(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

CHotSpot::CHotSpot()
{
	m_idHotSpot = 0;
	m_nEndShots = 0;
	m_pIdEnd = NULL;
	m_nShowShots = 0;
	m_pIdShow = NULL;
	m_nHideShots = 0;
	m_pIdHide = NULL;
	m_nPreHideShots = 0;
	m_pIdPreHide = NULL;
	m_nPreShowShots = 0;
	m_pIdPreShow = NULL;
	m_nPreDisableShots = 0;
	m_pIdPreDisable = NULL;
	m_nPreEnableShots = 0;
	m_pIdPreEnable = NULL;
	m_nEnableShots = 0;
	m_pIdEnable = NULL;
	m_nDisableShots = 0;
	m_pIdDisable = NULL;
	m_nStopShowShots = 0;
	m_pIdStopShow = NULL;
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
	m_bUpdateNow = FALSE;
	m_bClipDib = TRUE;
	m_bAutoClick = FALSE;
	m_bNoIdleStop = FALSE;
	m_bRestartIdle = FALSE;
	m_bCycleBack = FALSE;
	m_ptPos.x = m_ptPos.y = -1;
	SetRectEmpty(&m_rHot);
	SetRectEmpty(&m_rClip);
	m_szAVIFile[0] = '\0';
	m_szForegroundDib[0] = '\0';
	m_pForegroundDib = NULL;
	m_pHighlightDib = NULL;
	m_szWaveFile[0] = '\0';
	m_szHintWave[0] = '\0';
	m_uEndDelay = 0;
	m_bDisableCursor = FALSE;
	m_idCommand = 0;
	m_lpSprite = NULL;
	m_nCells = 1;
	m_nSpeed = 4;
	m_nPauseTime = 0;
	m_nAnimEvents = 0;
	m_pAnimEvents = NULL;
	m_fVideo = FALSE;
	m_fCenterVideo = FALSE;
	m_iScaleVideo = 0;
	m_ptMovie.x = m_ptMovie.y = 0;
}

CHotSpot::~CHotSpot()
{
	if (m_pForegroundDib)
		delete m_pForegroundDib;
	if (m_pHighlightDib)
		delete m_pHighlightDib;
	if (m_pIdEnd)
		ToonFree(m_pIdEnd);
	if (m_pIdShow)
		ToonFree(m_pIdShow);
	if (m_pIdHide)
		ToonFree(m_pIdHide);
	if (m_pIdPreHide)
		ToonFree(m_pIdPreHide);
	if (m_pIdPreShow)
		ToonFree(m_pIdPreShow);
	if (m_pIdPreDisable)
		ToonFree(m_pIdPreDisable);
	if (m_pIdPreEnable)
		ToonFree(m_pIdPreEnable);
	if (m_pIdEnable)
		ToonFree(m_pIdEnable);
	if (m_pIdDisable)
		ToonFree(m_pIdDisable);
	if (m_pIdStopShow)
		ToonFree(m_pIdStopShow);
	if (m_pAnimEvents)
		ToonFree(m_pAnimEvents);
}

//************************************************************************
BOOL CHotSpot::GetEraseRect(LPRECT lpRect, BOOL bIgnoreVisibleFlag)
//************************************************************************
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

//************************************************************************
BOOL CHotSpot::IsLooping()
//************************************************************************
{
	return(m_bLoop || (m_bIdle && !m_bSprite && !m_nEndShots));
}

//************************************************************************
CTransitionDesc::CTransitionDesc()
//************************************************************************
{
	m_fFade = TRUE;
	m_fClip = TRUE;
	m_uTransition = TRANS_STRETCH_LEFT;
	m_nStepSize = -1;
	m_nTicks = 0;
	m_dwTotalTime = 0;
}			  

//************************************************************************
CToon::CToon(HWND hWnd, int idCursor, int idNullCursor, HINSTANCE hInstance, HPALETTE hPal)
//************************************************************************
{
	m_hToon = NULL;
	m_hWnd = hWnd;                     
	if (hInstance)
		m_hInstance = hInstance;
	else
		m_hInstance = GetWindowInstance(m_hWnd);
	m_hHotSpotCursor = LoadCursor(m_hInstance, MAKEINTRESOURCE(idCursor));
	m_hNullCursor = LoadCursor(m_hInstance, MAKEINTRESOURCE(idNullCursor));
	m_hPal = NULL; //hPal;
	m_nSceneNo = 0;
	m_fSoundTrack = FALSE;
	m_fCursorVisible = TRUE;
	for (int i = 0; i < 256; ++i)
	{
		m_rgbLastColors[i].rgbRed = 0;
		m_rgbLastColors[i].rgbGreen = 0;
		m_rgbLastColors[i].rgbBlue = 0;
		m_rgbLastColors[i].rgbReserved = 0;
	}
	Init();
}

//************************************************************************
void CToon::Init()
//************************************************************************
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
	//m_iPaintMethod = PAINT_INVALIDATE_RECT;
	m_iPaintMethod = PAINT_MULTIPLE_BLT;
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
	m_fVideo = FALSE;
	m_fCenterVideo = FALSE;
	m_iScaleVideo = 0;
	m_nLanguageOffset = 0;
	ShowCursor(FALSE);
	m_fCloseMovie = FALSE;
	m_dwLastDown = 0;
	m_fStartIdleFirst = FALSE;
	m_fIdleSpritesOn = FALSE;
	m_fCursorVisibleState = TRUE;
	m_fHasHotSpots = FALSE;
	m_fInInitialize = FALSE;
	m_fReadOnly = FALSE;
}

//************************************************************************
CToon::~CToon()
//************************************************************************
{        
	FreeUp();
	m_WinGDib.SetPtr(NULL);
	if (m_hToon)
		ToonDestroy(m_hToon);
	//if (m_hHotSpotCursor)
	//	DestroyCursor(m_hHotSpotCursor);
	//if (m_hNullCursor)
	//	DestroyCursor(m_hNullCursor);
	ShowCursor(TRUE);
}
     
//************************************************************************
void CToon::FreeUp()
//************************************************************************
{        
	if (m_hMovie)
	{
		MCIStop(m_hMovie, TRUE/*bWait*/);
		MCIClose(m_hMovie);
	}
	if (m_hSound)
	{
		MCIStop(m_hSound, TRUE/*bWait*/);
		MCIClose(m_hSound);
	}
	if (m_pBackgroundDib)
		delete m_pBackgroundDib;
	if (m_pStageDib)
	{
		hmemcpy( m_rgbLastColors, m_pStageDib->GetColors(), sizeof(m_rgbLastColors));
		if (m_hToon)
			ToonSetBackground(m_hToon, NULL, NULL);
		delete m_pStageDib;
	}
	if (m_pHotSpots)
		delete [] m_pHotSpots;
	// make sure the cursor is disabled between scenes
	ShowCursor(FALSE);
	//SetCursor(LoadCursor( NULL, IDC_ARROW ));
	//EnableCursor(TRUE);
	if (m_hToonPal)
	{
		DeleteObject(m_hToonPal);
		m_hToonPal = NULL;
	}
	SetTimer(/* RESET */);
	Init();
}
     
//************************************************************************
BOOL CToon::CreateToon(int cx, int cy)
//************************************************************************
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

//************************************************************************
BOOL CToon::Create(UINT ToonDrawMessage, int nLanguageOffset)
//************************************************************************
{           
	m_ToonDrawMessage = ToonDrawMessage;
	m_nLanguageOffset = nLanguageOffset;

	// make sure we get a full window paint
	InvalidateRect(m_hWnd, NULL, FALSE);

	return(TRUE);
}

//************************************************************************
BOOL CToon::Initialize(HDC hDCIn)
//************************************************************************
{
	int nLastSceneNo;
	int id;
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

	id = m_nSceneNo + 1000;
	if (lpScene->GetSound() && FindResource(m_hInstance, (LPSTR)MAKEINTRESOURCE(id), "WAVE"))
		m_fSoundStarted = lpScene->GetSound()->StartResourceID(id, FALSE, SOUND_CHANNEL, m_hInstance ); 

	LoadJumpTable();

	if (lstrlen(m_szPath))
	{
		FNAME szPath;

		if (!lstrlen(m_szBGDib))
			lstrcpy(m_szBGDib, FileName(m_szPath));
		StripFile(m_szPath);
		GetApp()->GetContentDir(szPath);
		lstrcat(szPath, m_szPath);
		lstrcpy(m_szPath, szPath);
	}

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
	if (GetApp()->IsNextScene(nLastSceneNo, m_nSceneNo))
		m_Transition = m_LeftTransition;
	else
	if (GetApp()->IsPrevScene(nLastSceneNo, m_nSceneNo))
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
	#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
	#endif
	if (!LoadData(m_szBGDib))
	{
		if (!hDCIn)
			ReleaseDC(m_hWnd, hDC);
		return(FALSE);
	}
	#ifdef _DEBUG
	Debug("Load Data: %ld ms", timeGetTime()-dwTime);
	#endif

    // plug our stage in as the WinToon background
	ToonSetBackground(GetToonHandle(), m_pStageDib->GetInfoHeader(), m_pStageDib->GetPtr());

	// set color table if we are not fading in.  If you
	// set the color table the display palette will change
	if (!m_Transition.m_fFade) 	    
	{
		if ( m_Transition.m_uTransition )
		{
			// If not fading on a palette device, blast out black to avoid ugliness
			RECT rRepair;
			if (m_Transition.m_fClip && !IsRectEmpty(&m_rGlobalClip))
				rRepair = m_rGlobalClip;
			else
				GetClientRect(m_hWnd, &rRepair);
			if ( caps & RC_PALETTE ) // palette device
				BitBlt(hDC, rRepair.left, rRepair.top, rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, hDC, rRepair.left, rRepair.top, BLACKNESS);
		}
		ToonSetColors(GetToonHandle(), m_pStageDib->GetColors());
	}

	// make sure the WinG dib is initialized
	UpdateWinGDib();

    // build the stage, this copies the foreground sprites into the background dib
	#ifdef _DEBUG
	dwTime = timeGetTime();
	#endif
	BuildStage();  // builds the entire stage
	#ifdef _DEBUG
	Debug("Build Stage: %ld ms", timeGetTime()-dwTime);
	#endif

	// copy bits to WinG bitmap
	// only if we have a background bitmap
	// otherwise, BuildStage takes care of this for us
	if (m_pBackgroundDib)
	{
		#ifdef _DEBUG
		dwTime = timeGetTime();
		#endif
		ToonRestoreBackground(GetToonHandle());
		#ifdef _DEBUG
		Debug("Restore Background: %ld ms", timeGetTime()-dwTime);
		#endif
	}

	if (m_ToonDrawMessage)
		ToonSetMessage(GetToonHandle(), m_ToonDrawMessage);

	// see if we need to disable the cursor	                               
	//for (id = 1; id <= m_nHotSpots; ++id)
	//{
	//	PHOTSPOT pHotSpot = GetHotSpot(id);
	//	if (!pHotSpot)
	//		continue;
	//	// disable cursor if we have an auto animation
	//	if (pHotSpot->m_bAuto && !pHotSpot->m_bCanStop)
	//	{
	//		EnableCursor(FALSE);
	//		break;
	//	}
	//}
	
	// init any idle sprites
	InitIdleSprites();

	if (m_Transition.m_fFade)
	{
		ToonPaintDC(GetToonHandle(), hDC);
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
				// update just this hotspot
				BuildStage(id, CToon::UpdateHotSpot);
			}
		}
	}

	// see if we have any hotspots we need a cursor for
	m_fCursorVisibleState = FALSE;
	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (!IsRectEmpty(&pHotSpot->m_rHot))
		{
			m_fCursorVisibleState = TRUE;
			break;
		}
	}

	m_fHasHotSpots = m_fCursorVisibleState;
	ShowCursor(m_fCursorVisibleState);

	if (!hDCIn)
		ReleaseDC(m_hWnd, hDC);

	return(TRUE);
}

//************************************************************************
void CToon::EndInitialize()
//************************************************************************
{
	if (m_fSoundStarted)
	{
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		lpScene->GetSound()->StopChannel(SOUND_CHANNEL);
	}

	LPSCENE lpScene = GetApp()->GetCurrentScene();
	lpScene->ToonInitDone();

	PlayAuto();
}

//************************************************************************
BOOL CToon::LoadData(LPCTSTR lpBGDib)
//************************************************************************
{
	CDib dib;
	BOOL fTopDown;
	FNAME szFileName;
	DWORD dwTime;
	BOOL fHaveVideo = FALSE;

	m_hPal = NULL;
	lstrcpy(szFileName, m_szPath);
	lstrcat(szFileName, lpBGDib);
	#ifdef _DEBUG
	dwTime = timeGetTime();
	#endif
	if ((m_pBackgroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, TRUE)) == NULL)
	{
		Print("Error opening file '%s'", (LPSTR)szFileName);
		return (FALSE);
	}
	#ifdef _DEBUG
	Debug("Load background: %ld ms", timeGetTime()-dwTime);
	#endif

	dwTime = timeGetTime();
	if (!LoadForegroundDibs())
		return(FALSE);
	#ifdef _DEBUG
	Debug("Load foreground dibs: %ld ms", timeGetTime()-dwTime);
	#endif

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
		#ifdef _DEBUG
		dwTime = timeGetTime();
		#endif
		m_pBackgroundDib->DibFlip();
		#ifdef _DEBUG
		Debug("Flip Background: %ld ms", timeGetTime()-dwTime);
		#endif
	}
            
               
	// if we don't have a movie, see if any hotspots have
	// a movie
	if (lstrlen(m_szAVIFile))
		fHaveVideo = TRUE;
	else
	{
		for (int id = 1; id <= m_nHotSpots; ++id)
		{
			PHOTSPOT pHotSpot = GetHotSpot(id);
			if (!pHotSpot)
				continue;
			if (lstrlen(pHotSpot->m_szAVIFile))
			{
				fHaveVideo = TRUE;
				break;
			}
		}
	}
    // open up the movie
    if (fHaveVideo)
	{
		// create the  stage dib by copying the background dib
		#ifdef _DEBUG
		dwTime = timeGetTime();
		#endif
		if (!(m_pStageDib = m_pBackgroundDib->DibCopy(FALSE)))
		{
			Print("Out of Memory");
			return(FALSE);
		}
		#ifdef _DEBUG
		Debug("Create Stage Dib: %ld ms", timeGetTime()-dwTime);
		#endif
	}
	else
	{
		// don't need background dib if you don't have a movie
		m_pStageDib = m_pBackgroundDib;
		m_pBackgroundDib = NULL;
	}

	if (lstrlen(m_szAVIFile))
    {   
		FNAME szFileName;

		#ifdef _DEBUG
		dwTime = timeGetTime();
		#endif
		// always use WinToon to play any video, so that
		// we can get OnToonDraw() and handle event triggering
		if (FindContent(m_szAVIFile, FALSE, szFileName))
		{
			if (m_fVideo && FALSE)
				m_hMovie = MCIOpen(GetApp()->m_hDeviceAVI, szFileName, NULL, m_hWnd);
			else
				m_hMovie = ToonOpenMovie(GetToonHandle(), szFileName, m_TransparentIndex);
		}
		else
				m_hMovie = NULL;

		if (m_hMovie == NULL)
		{
			Print("Error opening file '%s'", (LPSTR)szFileName);
			// let it go for now return(FALSE);
		}
		else
		{    
			m_fCloseMovie = FALSE;
			//MCISetVideoSpeed(m_hMovie, 500);
			if (m_fSoundTrack)
				MCISetAudioOnOff(m_hMovie, FALSE);
			#ifdef _DEBUG
			Debug("Open AVI: %ld ms", timeGetTime()-dwTime);
			#endif
			MCIGetSrcRect(m_hMovie, &m_rSrcOrig);
			//MCISetAudioStream(m_hMovie, 1);
			ToonSetPaintMethod(GetToonHandle(), m_iPaintMethod);
			if (!ToonSetPaintList(GetToonHandle(), m_iPaintList))
			{
				m_iPaintList = LIST_SLOW;
				ToonSetPaintList(GetToonHandle(), m_iPaintList);
			}

			//if (m_iPaintList == LIST_SLOW)
			//	Debug("using SLOW paint list");
			//else
			//if (m_iPaintList == LIST_FAST)
			//	Debug("using FAST paint list");
			//else
			//	Debug("using NORMAL paint list");
			++m_iPaintList;
			if (m_iPaintList > LIST_FAST)
				m_iPaintList = LIST_NORMAL;

		}
	}
	return(TRUE);
}

//************************************************************************
BOOL CToon::LoadJumpTable()
//************************************************************************
{
	HRSRC hResource;
	HGLOBAL hData;
	LPSTR lpTableData;
	DWORD dwSize;
	int id;

	// read in the hotspot information
	dwSize = 0;
	
	hData = NULL;
	lpTableData = NULL;
	id = m_nSceneNo;
	#ifdef _DEBUG
	DWORD dwTime = timeGetTime();
	#endif

	// check for a language specific version of the jump table
	if (id && m_nLanguageOffset)
	{
		id += m_nLanguageOffset;
		if (FindResource( m_hInstance, MAKEINTRESOURCE(id), RT_RCDATA) == NULL)
			id -= m_nLanguageOffset;
	}
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
	#ifdef _DEBUG
	Debug("Load Jump Table: %ld ms", timeGetTime()-dwTime);
	#endif

	#ifdef _DEBUG
	dwTime = timeGetTime();
	#endif
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
		}
	}

	#ifdef _DEBUG
	Debug("Parse Jump Table: %ld ms", timeGetTime()-dwTime);
	#endif

	if (hData)
	{
		UnlockResource( hData );
		FreeResource( hData );
	}

	return(TRUE);
}

//************************************************************************
LPSTR CToon::GetContentDir(LPSTR lpContentDir)
//************************************************************************
{
	lstrcpy(lpContentDir, m_szPath);
	return(lpContentDir);
}

//************************************************************************
LOCAL void CALLBACK TransCallback(DWORD dwIteration, DWORD dwTotalIterations, DWORD dwCallbackData)
//************************************************************************
{
	PTOON pToon = (PTOON)dwCallbackData;
	pToon->TransCallback(dwIteration, dwTotalIterations);
}

//************************************************************************
void CToon::TransCallback(DWORD dwIteration, DWORD dwTotalIterations)
//************************************************************************
{
	if ( dwTotalIterations <= 2 )
		return;
	dwIteration = dwTotalIterations - dwIteration;
	WORD wVol = LOWORD(m_dwStartVol);
	if (!wVol || (dwIteration > dwTotalIterations))
		return;
	// increase the total by 33% to be sure the fade doesn't go to zero
	// This is because it will sound like zero at about 33% of the volume
	DWORD dw = (dwTotalIterations/3);
	dwIteration += dw;
	dwTotalIterations += dw;
	wVol = (WORD)(((DWORD)wVol * dwIteration) / dwTotalIterations);
	
	#ifdef _DEBUG
	Debug("dwStartVol = %lx wVol = %x", m_dwStartVol, wVol);
	#endif
	MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, wVol, wVol);
}

//************************************************************************
void CToon::SetBackground(LPSTR lpFileName)
//************************************************************************
{
	lstrcpy(m_szBGDib, lpFileName);
}

//************************************************************************
BOOL CToon::LoadBackground(	LPSTR lpFileName, UINT uTransition,
							int nTicks, int nStepSize, DWORD dwTotalTime,
							LPRECT lpTransClipRect,
							TOONDRAWPROC lpDrawProc, DWORD dwData)
//************************************************************************
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
	if (!m_pBackgroundDib)
		return(FALSE);

	if (ToonIsTopDown(GetToonHandle()) != (m_pBackgroundDib->GetHeight() < 0))
		m_pBackgroundDib->DibFlip();
    if (!lstrlen(m_szAVIFile))
	{
		// don't need background dib if you don't have a movie
		m_pStageDib = m_pBackgroundDib;
		m_pBackgroundDib = NULL;
	}

	// rebuild the entire stage
   	BuildStage();
	ToonSetBackground(GetToonHandle(), m_pStageDib->GetInfoHeader(), m_pStageDib->GetPtr());
	if (m_pBackgroundDib)
		ToonRestoreBackground(GetToonHandle());

	if (lpDrawProc)
		(*lpDrawProc)(this, dwData);

	if (uTransition)
	{
		PDIB pWinGDib;
		HPALETTE hPal, hOldPal;
		RECT rRepair;
		TRANSPARMS parms;

		SetRect(&rRepair, 0, 0, GetWidth(), GetHeight());
		if (!IsRectEmpty(&m_rGlobalClip))
			IntersectRect(&rRepair, &rRepair, &m_rGlobalClip);
		if (lpTransClipRect)
			IntersectRect(&rRepair, &rRepair, lpTransClipRect);

		HDC hDC = GetDC(m_hWnd);

		pWinGDib = GetWinGDib();
		hPal = m_hToonPal;
		if (hPal)
		{
			hOldPal = SelectPalette(hDC, hPal, FALSE);
			::RealizePalette(hDC);
		}

		SetTransitionParms( &parms, uTransition, hDC,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, pWinGDib, ToonDC(m_hToon));
		SetTransitionTiming( &parms, nTicks, nStepSize, dwTotalTime);
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
			hOldPal = SelectPalette(hDC, hOldPal, TRUE);
		ReleaseDC(m_hWnd, hDC);
	}
	else
	{
		UpdateArea(NULL);
	}
	return (m_pBackgroundDib != NULL);
}

//************************************************************************
PDIB CToon::GetWinGDib()
//************************************************************************
{
	return(&m_WinGDib);
}

//************************************************************************
PDIB CToon::UpdateWinGDib()
//************************************************************************
{
	m_WinGDib.SetPtr(ToonGetDIBPointer(GetToonHandle(), m_WinGDib.GetInfoHeader()));
	ToonGetColors(GetToonHandle(), 0, 256, m_WinGDib.GetColors());
	m_WinGDib.FixHeader();
	return(&m_WinGDib);
}

//************************************************************************
void CToon::RestoreBackgroundArea(LPRECT lpRect, BOOL fUpdateDisplay)
//************************************************************************
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
		UpdateArea(&rArea);
	}
}

//************************************************************************
void CToon::GrabBackgroundArea(LPRECT lpRect, BOOL fUpdateDisplay)
//************************************************************************
{
	PDIB pWinGDib = GetWinGDib();
	pWinGDib->DibBlt( 	m_pStageDib,
					  	lpRect->left, lpRect->top,
					  	lpRect->right-lpRect->left, lpRect->bottom-lpRect->top,
					  	lpRect->left, lpRect->top,
					  	lpRect->right-lpRect->left, lpRect->bottom-lpRect->top,
					  	FALSE);
	if (fUpdateDisplay)
		UpdateArea(lpRect);
}

//************************************************************************
BOOL CToon::LoadForegroundDibs(BOOL fDecodeRLE)
//************************************************************************
{
	SHOTID id;
	BOOL fRet = TRUE;

	for (id = 1; id <= m_nHotSpots; ++id)
	{
		if (!fRet)
			break;

		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bSprite && pHotSpot->m_bIdle)
			m_bIdleSprites = TRUE;

		if (lstrlen(pHotSpot->m_szForegroundDib))
		{
			FNAME szFileName;

			if (!FindContent(pHotSpot->m_szForegroundDib, TRUE, szFileName))
			{
				Print("Error opening file '%s'", (LPSTR)pHotSpot->m_szForegroundDib);
				return(FALSE);
			}
			if (!(pHotSpot->m_pForegroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, fDecodeRLE)))
			{
				// Don't display a message here, because it is possible
				// that	a WM_CLOSE was posted to the main app window
				// if we call print then the WM_CLOSE will get processed
				// and we will end up in a fucked up state.
				Debug("Error opening file '%s'", (LPSTR)szFileName);
				return(FALSE);
			}
			else
			{
				if (pHotSpot->m_bButton)
				{
					PDIB pDibSrc = pHotSpot->m_pForegroundDib->GetDib();
					// This will show buttons that are "alwaysenabled" when the book is in "Play"
					// mode, but not in "Read" mode (SMS).  This flag is set in the dialog OnInit.
					if (pDibSrc && (pHotSpot->m_bAlwaysEnabled || !m_fReadOnly))
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
								pDibSrc->Release();
								delete pHotSpot->m_pForegroundDib;
								pHotSpot->m_pForegroundDib = NULL;
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
								pDibSrc->Release();
								delete pHotSpot->m_pForegroundDib;
								pHotSpot->m_pForegroundDib = NULL;
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
						delete pHotSpot->m_pForegroundDib;
						pHotSpot->m_pForegroundDib = pDibs[0];
						pHotSpot->m_pHighlightDib = pDibs[1];
					}
					else
					{
						delete pHotSpot->m_pForegroundDib;
						pHotSpot->m_pForegroundDib = NULL;
					}
				}
				//if (pDibSrc)
				//{
				//	if (m_TransparentIndex < 0)
				//	    m_TransparentIndex = *(pDibSrc->GetXY(0, 0));
				//	pDibSrc->Release();
				//}
				if (pHotSpot->m_pForegroundDib)
				{  // if it's a "hot" point (not a static "pos" point), fix the size
					if (pHotSpot->m_ptPos.x == -1)
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

//************************************************************************
BOOL CToon::InitIdleSprites()
//************************************************************************
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

	// set the clipping rectangle for the sprite engine
	if (!IsRectEmpty(&m_rGlobalClip))
		m_pAnimator->SetClipRect(&m_rGlobalClip);

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
		{
			pHotSpot->m_pForegroundDib = lpCell->pSpriteDib;
			if (pHotSpot->m_pForegroundDib)
			{  // if it's a "hot" point (not a static "pos" point), fix the size
				if (pHotSpot->m_ptPos.x == -1)
				{
					pHotSpot->m_rHot.right = pHotSpot->m_rHot.left + pHotSpot->m_pForegroundDib->GetWidth();
					pHotSpot->m_rHot.bottom = pHotSpot->m_rHot.top + abs(pHotSpot->m_pForegroundDib->GetHeight());
				}
			}
		}

		// add the new smaller sprite back to the stage dib
		BuildStage(id, CToon::ShowHotSpot);
		pHotSpot->GetEraseRect(&rErase, TRUE);
		// now copy it to the WinG dib
		RestoreBackgroundArea(&rErase, FALSE);

		// set sprite settings
		pHotSpot->m_lpSprite->Jump(rErase.left, rErase.top);
		pHotSpot->m_lpSprite->SetCellsPerSec(pHotSpot->m_nSpeed);
		pHotSpot->m_lpSprite->Pause(PAUSE_FOREVER);
		// see if we need to set cycle back flag
		if (pHotSpot->m_bCycleBack)
			pHotSpot->m_lpSprite->SetCycleBack(TRUE);
		if (pHotSpot->m_nPauseTime)
		{
			// marks beginning of a set of commands to repeat
			pHotSpot->m_lpSprite->AddCmdBegin();
			if (pHotSpot->m_bCycleBack)
			{
				// pause after last cell has been displayed
				pHotSpot->m_lpSprite->AddCmdPauseAfterCell(0, pHotSpot->m_nPauseTime);
			}
			else
			{
				// pause after last cell has been displayed
				pHotSpot->m_lpSprite->AddCmdPauseAfterCell(pHotSpot->m_nCells-1, pHotSpot->m_nPauseTime);
				// after pause, reset cell to first
				pHotSpot->m_lpSprite->AddCmdSetCurrentCell(0);
			}
			// repeat the commands starting at begin
			pHotSpot->m_lpSprite->AddCmdRepeat();
		}
		if (!pHotSpot->m_bIdle)
			pHotSpot->m_lpSprite->SetNotifyProc(::OnSpriteNotify, (DWORD)this);
	}
	return(TRUE);
}

//************************************************************************
HPALETTE CToon::GetPalette()
//************************************************************************
{
	if (!m_hToonPal)
	{
		RGBQUAD rgbQuad[256];

		ToonGetColors(GetToonHandle(), 0, 256, rgbQuad);
		m_hToonPal = CreateCustomPalette(rgbQuad, 256);
	}
	return(m_hToonPal);
}

//************************************************************************
void CToon::StopPlaying(BOOL fStopIdleSprites, BOOL fExiting)
//************************************************************************
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
					// update just this sprite
					BuildStage(id, CToon::UpdateHotSpot, TRUE);
				}
			}
		}
	}

	if (fStopIdleSprites)
		StopIdleSprites();
	if (m_fPlaying)
	{
		// turn on hints on if we aborting playing something
		// because ordinarily they are only turned on
		// when the intro is done
		m_fHintsOn = TRUE;

		if (m_hSound)
		{
			MCIStop(m_hSound, TRUE);
			MCIClose(m_hSound);
			m_hSound = NULL;
		}
		else
		{
			MCIStop(m_hMovie, TRUE);
			if (m_fCloseMovie)
			{
				MCIClose(m_hMovie);
				m_hMovie = NULL;
			}
		}
		// call change sprite states instead of reset
		// so that hides and shows will occur
		PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
		if (pHotSpot)
			ChangeSpriteStates(pHotSpot);
		//Reset();
	}
}

//************************************************************************
BOOL CToon::PlayAuto(BOOL fNoDelay)
//************************************************************************
{
	SHOTID id;

	if (m_iAutoPlayDelay && !fNoDelay)
	{
		if (SetTimer( CToon::AutoPlayTimer, (UINT)m_iAutoPlayDelay*1000 ))
		{
			return(TRUE);
		}
	}

	StopPlaying();
	if (m_bIdleSprites && m_fStartIdleFirst)
		StartIdle();

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
  
//************************************************************************
BOOL CToon::PlayConclusion()
//************************************************************************
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
			// hide the cursor when playing conclusion animation
			ShowCursor(FALSE);
			m_PlayingState = CToon::Conclusion;
			return(OnHotSpot(pHotSpot->m_idHotSpot));
		}
	}
	return(FALSE);
}
  
//************************************************************************
BOOL CToon::CheckSequence(SHOTID idSequence, int nSequence)
//************************************************************************
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

//************************************************************************
void CToon::ResetSequence()
//************************************************************************
{
	SHOTID id;

	for (id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (pHotSpot)
			pHotSpot->m_nCurSequence = 0;
	}
}

//************************************************************************
void CToon::EnableCursor(BOOL fEnable)
//************************************************************************
{
	// make sure we are changing enable state 
	if (fEnable != m_fCursorEnabled)
	{
		m_fCursorEnabled = fEnable;
	}
}

//
// idHotSpot - id of hotspot to remove or add to stage. (0 builds entire stage)
// mode - CToon::HideHotSpot - removes hotspot from stage (normal case).
// 		  CToon::ShowHotSpot - draws the hotspot.  If hotspot is invisible
//							   then nothing is done.
//		  CToon::UpdateHotSpot - update the area occupied by the hotspot.
// fUpdateDisplay - draw the changes to the display.  Ordinarily, this
//					is not done.  The display gets updated when the video
//					plays to prevent flashing.
//
//************************************************************************
void CToon::BuildStage(SHOTID idHotSpot, BuildMode mode, BOOL fUpdateDisplay, LPRECT lpUpdateRect)
//************************************************************************
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
		if (lpUpdateRect && !IsRectEmpty(lpUpdateRect))
			UnionRect(&rArea, &rArea, lpUpdateRect);

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
		//hmemcpy(pDibDst->GetPtr(), pDibSrc->GetPtr(), pDibDst->GetSizeImage());
		CopyBytes(pDibDst->GetPtr(), pDibSrc->GetPtr(), pDibDst->GetSizeImage());
		SetRect(&rArea, 0, 0, pDibDst->GetWidth(), abs(pDibDst->GetHeight()));
	}
             
    //Debug("BuildStage");              
	for (id = 1; id <= m_nHotSpots ; id++ )
	{
		RECT rErase, rSect, rHotSpot;

		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;

		if ((id != idHotSpot || lstrlen(pTheHotSpot->m_szWaveFile) || (mode != CToon::HideHotSpot)) &&
			pHotSpot->GetEraseRect(&rErase) &&
			(!pHotSpot->m_bSprite || pHotSpot->m_lpSprite))
		{
			rHotSpot = rErase;
			if (pHotSpot->m_bClipDib)
			{
				if (!IsRectEmpty(&m_rGlobalClip))
					IntersectRect(&rHotSpot, &rHotSpot, &m_rGlobalClip);
  				if (!IsRectEmpty(&pHotSpot->m_rClip))
		 			IntersectRect(&rHotSpot, &rHotSpot, &pHotSpot->m_rClip);
			}
			if (IntersectRect(&rSect, &rArea, &rHotSpot))
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
				{
					#ifdef _DEBUG
					//DWORD dwTime = timeGetTime();
					#endif
					pDib = pDib->GetDib();
					#ifdef _DEBUG
					//Debug("GetDib - %x ms", timeGetTime()-dwTime);
					#endif
				}
			
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
  
//************************************************************************
PHOTSPOT CToon::GetHotSpot(SHOTID idHotSpot)
//************************************************************************
{
	if (idHotSpot <= 0)
		return(NULL);
	if (idHotSpot > m_nHotSpots)
		return(NULL);
	return(&m_pHotSpots[idHotSpot-1]);
}

//************************************************************************
int CToon::FindHotSpot(int x, int y, BOOL fSearchAll)
//************************************************************************
{
	POINT pt;
	SHOTID id;
	int dx, dy;
	BOOL fInHotSpot;

	pt.x = x;
	pt.y = y;

	for (id = m_nHotSpots; id >= 1 ; id--)
	{				 
		RECT rErase;
		RECT rHot;

		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;

		if (!m_fClickablesEnabled && !pHotSpot->m_bAlwaysEnabled)
			continue;

		// get erase rect for this hotspot
		pHotSpot->GetEraseRect(&rErase, TRUE);
		// if we have no hotspot, use the erase rect
		// could be a bitmap sitting on top of a hotspot
		rHot = pHotSpot->m_rHot;
		if (IsRectEmpty(&rHot))
			rHot = rErase;
		if (pHotSpot->m_bClipDib)
		{
			if (!IsRectEmpty(&m_rGlobalClip))
				IntersectRect(&rHot, &rHot, &m_rGlobalClip);
			if (!IsRectEmpty(&pHotSpot->m_rClip))
	 			IntersectRect(&rHot, &rHot, &pHotSpot->m_rClip);
		}

		if (pHotSpot->m_bEnabled &&
			(pHotSpot->m_bVisible || pHotSpot->m_bForceEnable || pHotSpot->m_bBrowse || fSearchAll) &&
			PtInRect(&rHot, pt))
		{      
			PDIB pDib = NULL;

			// if in hotspot, then check to see if there is no
			// foreground dib or we are not doing a transparency check
			fInHotSpot = PtInRect(&pHotSpot->m_rHot, pt);
			if (fInHotSpot)
			{
				if (!pHotSpot->m_pForegroundDib || pHotSpot->m_bNoTransparency)
					return (int)pHotSpot->m_idHotSpot;
			}

			// can't do a transparency check, so not in hotspot
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
				{
					if (fInHotSpot)
						return (int)pHotSpot->m_idHotSpot;
					else // in a bitmap that has no hotspot
						return(0);
				}
			}
			//break;
		}
	}
	return(0);
}

//************************************************************************
int CToon::FindHotSpot(LPCTSTR lpForegroundDib)
//************************************************************************
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

//************************************************************************
int CToon::FindHotSpot(UINT idCmd)
//************************************************************************
{
	for (int id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_idCommand == idCmd)
			return(id);
	}
	return(0);
}

//************************************************************************
BOOL CToon::OnHotSpot(LPCTSTR lpForegroundDib)
//************************************************************************
{
	int id = FindHotSpot(lpForegroundDib);
	if (id)
		return(OnHotSpot(id));
	else
		return(FALSE);
}

//************************************************************************
BOOL CToon::OnHotSpot(SHOTID idHotSpot)
//************************************************************************
{
	HMCI hDevice;
	RECT rPreArea, rErase;
	FNAME szFileName;

	PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
	if (!pHotSpot)
		return(FALSE);

	StopPlaying(!pHotSpot->m_bNoIdleStop);

	SetRectEmpty(&rPreArea);
	if (pHotSpot->m_nPreHideShots)
	{
		for (int i = 0; i < pHotSpot->m_nPreHideShots; ++i)
		{
			PHOTSPOT pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdPreHide[i]);
			if (!pOtherHotSpot)
				break;
			if (pOtherHotSpot->m_bVisible)
			{
				pOtherHotSpot->m_bVisible = FALSE;
				if (pOtherHotSpot->m_bUpdateNow)
					BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, TRUE);
				else
				{
					pOtherHotSpot->GetEraseRect(&rErase, TRUE);
					UnionRect(&rPreArea, &rPreArea, &rErase);
				}
			}
		}
	}
	if (pHotSpot->m_nPreShowShots)
	{
		for (int i = 0; i < pHotSpot->m_nPreShowShots; ++i)
		{
			PHOTSPOT pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdPreShow[i]);
			if (!pOtherHotSpot)
				break;
			if (!pOtherHotSpot->m_bVisible)
			{
				pOtherHotSpot->m_bVisible = TRUE;
				BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, TRUE);
			}
		}
	}
	if (pHotSpot->m_nPreDisableShots)
	{
		for (int i = 0; i < pHotSpot->m_nPreDisableShots; ++i)
		{
			PHOTSPOT pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdPreDisable[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bEnabled = FALSE;
		}
	}
	if (pHotSpot->m_nPreEnableShots)
	{
		for (int i = 0; i < pHotSpot->m_nPreEnableShots; ++i)
		{
			PHOTSPOT pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdPreEnable[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bEnabled = TRUE;
		}
	}
	
	// make sure there is something to do, otherwise all
	// we do is try to change the sprite states.
	if (pHotSpot->m_dwTo > pHotSpot->m_dwFrom ||
		lstrlen(pHotSpot->m_szWaveFile) ||
		lstrlen(pHotSpot->m_szAVIFile))
	{			
		long lFrom = pHotSpot->m_dwFrom;
		long lTo = pHotSpot->m_dwTo;
		// if no from-to specified, then play to whole thing
		// only for wave files
		if (!lFrom && !lTo)
			lFrom = lTo = -1;

		if (lstrlen(pHotSpot->m_szWaveFile))
		{
			if (!FindContent(pHotSpot->m_szWaveFile, TRUE, szFileName))
				return(FALSE);
 			m_hSound = MCIOpen( GetApp()->m_hDeviceWAV, szFileName, NULL/*lpAlias*/ );
			if (!m_hSound)
				return(FALSE);
			hDevice = m_hSound;
			if (!IsRectEmpty(&rPreArea))
				BuildStage(0, CToon::HideHotSpot, TRUE, &rPreArea);
		}
		else
		{
			RECT rSrc, rDst, rSrcOrig;
			int iScaleVideo;
			BOOL fVideo, fCenterVideo;
			POINT ptMovie;

			if (m_hMovie)
			{
				rSrcOrig = m_rSrcOrig;
				fVideo = m_fVideo;
				fCenterVideo = m_fCenterVideo;
				iScaleVideo = m_iScaleVideo;
				ptMovie = m_ptMovie;
			}
			else
			{
				if (lstrlen(pHotSpot->m_szAVIFile))
				{
					lstrcpy(szFileName, m_szPath);
					lstrcat(szFileName, pHotSpot->m_szAVIFile);
					m_hMovie = ToonOpenMovie(GetToonHandle(), szFileName, 0 /*m_TransparentIndex*/);
					if (m_hMovie == NULL)
						Print("Error opening file '%s'", (LPSTR)szFileName);
					else
					{
						MCIGetSrcRect(m_hMovie, &rSrcOrig);
						m_fCloseMovie = TRUE;
						fVideo = pHotSpot->m_fVideo;
						fCenterVideo = pHotSpot->m_fCenterVideo;
						iScaleVideo = pHotSpot->m_iScaleVideo;
						ptMovie = pHotSpot->m_ptMovie;
					}
				}
				if (!m_hMovie)
					return(FALSE);
			}
			// playing normal video?
			if (fVideo)
			{
				POINT pt;
				int dx, cx, cy;
				RECT rArea;

				if (!IsRectEmpty(&m_rGlobalClip))
					rArea = m_rGlobalClip;
				else
					GetClientRect(m_hWnd, &rArea);
				if (!IsRectEmpty(&pHotSpot->m_rClip))
					IntersectRect(&rArea, &rArea, &pHotSpot->m_rClip);

				rSrc = rSrcOrig; // original video size
				if (!iScaleVideo) // scale to fit clipping rectangle
				{
			  		rDst = rArea;
			   		pt.x = rDst.left;
					pt.y = rDst.top;
					MapWindowPoints( m_hWnd, NULL, &pt, 1 ); // Map toon to screen
					int oldx = pt.x;
					pt.x += 3;
					pt.x &= (~3); // For it to live on a 4 pixel (32 bit) boundary
					// adjust on all sides so video is still centered
					dx = pt.x - oldx;
					InflateRect(&rDst, -dx, -dx);
				}
				else // specific scale factor
				{
					rDst = rSrc;
					// scale destination based on scale factor
					ScaleRect(&rDst, iScaleVideo);
					cx = rDst.right-rDst.left;
					cy = rDst.bottom-rDst.top;
					if (fCenterVideo) // center in clipping rectangle
					{
						pt.x = (rArea.right + rArea.left - cx)/2;
						pt.y = (rArea.bottom + rArea.top - cy)/2;
					}
					else // user supplied position
					{
						pt = ptMovie;
					}
					MapWindowPoints( m_hWnd, NULL, &pt, 1 ); // Map toon to screen
					pt.x += 3;
					pt.x &= (~3); // For it to live on a 4 pixel (32 bit) boundary
					MapWindowPoints( NULL, m_hWnd, &pt, 1 ); // Map screen to toon
					rDst.left = pt.x;
					rDst.top = pt.y;
					rDst.right = rDst.left + cx;
					rDst.bottom = rDst.top + cy;
				}
			}
			else // normal toon handling
			{
				if (!IsRectEmpty(&rPreArea) || !pHotSpot->m_bNoHide)
				{
					if (pHotSpot->m_bNoHide)
						BuildStage(0, CToon::HideHotSpot, FALSE, &rPreArea);
					else
						BuildStage(idHotSpot, CToon::HideHotSpot, FALSE, &rPreArea);
				}
	
				rDst = rSrcOrig;
				if (ptMovie.x != 0 || ptMovie.y != 0)
					OffsetRect(&rDst, ptMovie.x, ptMovie.y);
				if (!IsRectEmpty(&m_rGlobalClip))
					IntersectRect(&rDst, &rDst, &m_rGlobalClip);
  				if (!IsRectEmpty(&pHotSpot->m_rClip))
		 			IntersectRect(&rDst, &rDst, &pHotSpot->m_rClip);
				rSrc = rDst;
				if (ptMovie.x != 0 || ptMovie.y != 0)
					OffsetRect(&rSrc, -ptMovie.x, -ptMovie.y);
			}

			MCIClip(m_hMovie, &rSrc, &rDst);

			hDevice = m_hMovie;
		}

		m_lLastFrame = lFrom-1;
		m_lTotalDropped = 0;
		m_idCurHotSpot = idHotSpot;
		//MCISetAudioOnOff( hDevice, OFF );
		
		for (int i = 0; i < pHotSpot->m_nAnimEvents; ++i)
			pHotSpot->m_pAnimEvents[i].bTriggered = FALSE;

		MCIStop(hDevice, YES/*bWait*/);
//		if (MCIPlay(hDevice, m_hWnd, lFrom, lTo, 0, FALSE, pHotSpot->IsLooping()))
		if (MCIPlay(hDevice, m_hWnd, lFrom, lTo))
		{
			if (pHotSpot->m_bDisableCursor)
				ShowCursor(FALSE);
			//if (!pHotSpot->m_bIdle && !pHotSpot->m_bCanStop)
			//	EnableCursor(FALSE);
			m_fPlaying = TRUE;
			return(TRUE);
		}
	}
	else
	if (pHotSpot->m_bSprite)
	{
		if (!pHotSpot->m_lpSprite)
			return(FALSE);

		m_idCurHotSpot = idHotSpot;
		BuildStage(idHotSpot, CToon::HideHotSpot, FALSE, &rPreArea);
		pHotSpot->m_lpSprite->SetCurrentCell(0);
		pHotSpot->m_lpSprite->Pause(0);
		pHotSpot->m_lpSprite->PauseAfterCell(pHotSpot->m_nCells-1);
		pHotSpot->m_lpSprite->Show(TRUE);
		if (pHotSpot->m_bDisableCursor)
			ShowCursor(FALSE);
	}
	else
	if (pHotSpot->m_idCommand)
	{
		int nSceneNo;
		LPSCENE lpScene;
		BOOL bRestartIdle;

		ChangeSpriteStates(pHotSpot, TRUE);

		// get current scene no
		lpScene = GetApp()->GetCurrentScene();
		nSceneNo = lpScene->GetSceneNo();
		bRestartIdle = pHotSpot->m_bRestartIdle;
		FORWARD_WM_COMMAND(GetParentDlg(), pHotSpot->m_idCommand, m_hWnd, 0, SendMessage);
	   	lpScene = GetApp()->GetCurrentScene();
		// if scene no is the same and we have restart flag, restart the idle
	   	if (lpScene && (nSceneNo == lpScene->GetSceneNo()) && bRestartIdle)
			StartIdle();
	}
	else
	{
		ChangeSpriteStates(pHotSpot, TRUE);
		if (pHotSpot->m_nEndShots)
			JumpToRandomAnim(pHotSpot->m_pIdEnd, pHotSpot->m_nEndShots);
		else
			StartIdle();
	}
	return(FALSE);
}

//************************************************************************
BOOL CToon::SetTimer( TimerTypes type, UINT uTimeOut)
//************************************************************************
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

//************************************************************************
BOOL CToon::RealizePalette(BOOL bForceBackground)
//************************************************************************
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
		HWND hChild = ::GetWindow( GetParentDlg(), GW_CHILD );
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
//************************************************************************
BOOL CToon::OnQueryNewPalette(HWND hWnd)
//************************************************************************
{
	return(RealizePalette(FALSE));
}
  
//************************************************************************
void CToon::OnPaletteChanged(HWND hWnd, HWND hwndPaletteChanged)
//************************************************************************
{          
	// in this case the hWnd passed in is for the main application
	// the window we really want to compare against is our
	// window that we passed to Wintoon
	if (m_hWnd != hwndPaletteChanged)
		RealizePalette(TRUE);
}
#endif

//************************************************************************
BOOL CToon::OnQueryNewPalette(HWND hWnd)
//************************************************************************
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
			HWND hChild = ::GetWindow( GetParentDlg(), GW_CHILD );
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
  
//************************************************************************
void CToon::OnPaletteChanged(HWND hWnd, HWND hwndPaletteChanged)
//************************************************************************
{          
	// in this case the hWnd passed in is for the main application
	// the window we really want to compare against is our
	// window that we passed to Wintoon
	#ifdef _DEBUG
	Debug("OnPaletteChanged - m_hWnd = %x hwndPaletteChanged = %x", m_hWnd, hwndPaletteChanged);
	#endif
	HTOON hToon = GetToonHandle();
	if (hToon != NULL && (m_hWnd != hwndPaletteChanged))
	{
		if (ToonRealizePalette(hToon))
		{
			UpdateArea(NULL);
		}
	}
}

//************************************************************************
void CToon::OnLButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (!m_fCursorVisible)
		return;

	m_idHintHotSpot = 0; // turn off hinting on button down

	SHOTID idHotSpot;                                                 
	if (idHotSpot = FindHotSpot(x, y))
	{
		PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
		if (!pHotSpot)
			return;
		if (pHotSpot->m_dwTo > pHotSpot->m_dwFrom && !lstrlen(pHotSpot->m_szWaveFile))
		{
			// use a slight delay to prevent double-clicking
			DWORD dwTime = timeGetTime();
			if ((dwTime - m_dwLastDown) <= DOWN_WAIT)
				return;
			m_dwLastDown = dwTime;
		}
	
		SetTimer();

		if (pHotSpot->m_bButton)
		{
			if (pHotSpot->m_bAutoHighlight)
			{ // highlight the button
				pHotSpot->m_bHighlighted = TRUE;
				BuildStage(idHotSpot, UpdateHotSpot, TRUE /* fUpdateDisplay*/);
				if (pHotSpot->m_bAutoClick)
					PlayClick();
				else
					Delay(500);
				pHotSpot->m_bHighlighted = FALSE;
				BuildStage(idHotSpot, UpdateHotSpot, TRUE);
				m_idButtonHotSpot = 0;
				m_PlayingState = CToon::Normal;
				OnHotSpot(idHotSpot);
			}
			else // I assume this is for handling double clicks?
			if (idHotSpot == m_idButtonHotSpot && pHotSpot->m_idCommand)
			{
				if (pHotSpot->m_bAutoClick)
					PlayClick();
				m_idButtonHotSpot = 0;
				m_PlayingState = CToon::Normal;
				OnHotSpot(idHotSpot);
			}
		}		   
		else
		{
			if (OnNotify(CToon::ClickedOnHotSpot))
			{
				if (pHotSpot->m_bAutoClick)
					PlayClick();
				m_idButtonHotSpot = 0;
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

//************************************************************************
void CToon::OnLButtonUp(int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_idButtonHotSpot)
	{
		SHOTID idHotSpot = FindHotSpot(x, y);
		PHOTSPOT pHotSpot = GetHotSpot(m_idButtonHotSpot);
		pHotSpot->m_bHighlighted = FALSE;
		BuildStage(m_idButtonHotSpot, UpdateHotSpot, TRUE);
		if (idHotSpot == m_idButtonHotSpot && pHotSpot->m_idCommand)
		{
			// reset this now in case the command causes message
			// queue to get processed.  Like "Printing"
			m_idButtonHotSpot = 0;
			m_PlayingState = CToon::Normal;
			OnHotSpot(idHotSpot);
		}
		else
			m_idButtonHotSpot = 0;
	}
	else
	if (m_fSendMouseToParent)
		FORWARD_WM_LBUTTONUP(GetParentDlg(), x, y, keyFlags, SendMessage);
}

//************************************************************************
void CToon::OnRButtonDown(BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	#ifdef _DEBUG
		// Just a test to try and change the language
//		LPLGBAPP pApp = (LPLGBAPP)GetApp();
//		int i = pApp->GetLanguage();
//		if ( i==0) i = 1; else i = 0;
//		pApp->SetLanguage((CLGBApp::Language)(i));

		// Draw a box around all hotspots
		HDC hDC = GetDC(GetParentDlg());
		for (int id = 1; id <= m_nHotSpots; ++id)
		{
			PHOTSPOT pHotSpot = GetHotSpot(id);
			if (!pHotSpot)
				continue;
			RECT r;
			pHotSpot->GetEraseRect(&r, YES/*bIgnoreVisibleFlag*/);
			MoveToEx( hDC, r.left-1, r.top-1, NULL );
			LineTo( hDC, r.left-1, r.bottom );
			LineTo( hDC, r.right, r.bottom );
			LineTo( hDC, r.right, r.top-1 );	
			LineTo( hDC, r.left-1, r.top-1 );
		}
		ReleaseDC( GetParentDlg(), hDC );
	#endif
	if (m_fSendMouseToParent)
		FORWARD_WM_RBUTTONDOWN(GetParentDlg(), fDoubleClick, x, y, keyFlags, SendMessage);
}

//************************************************************************
void CToon::OnRButtonUp(int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fSendMouseToParent)
		FORWARD_WM_RBUTTONUP(GetParentDlg(), x, y, keyFlags, SendMessage);
}

//************************************************************************
void CToon::OnMouseMove(int x, int y, UINT keyFlags)
//************************************************************************
{
	if (!m_fCursorVisible)
		return;

	SHOTID idHotSpot = FindHotSpot(x, y);
	if (!HandleButtonHotSpot(idHotSpot))
	{
		HandleHints(idHotSpot);
		HandleBrowseHotSpot(idHotSpot);
		if (m_fSendMouseToParent)
			FORWARD_WM_MOUSEMOVE(GetParentDlg(), x, y, keyFlags, SendMessage);
	}
}

//************************************************************************
BOOL CToon::HandleButtonHotSpot(SHOTID idHotSpot)
//************************************************************************
{
	if (m_idButtonHotSpot)
	{
		PHOTSPOT pHotSpot = GetHotSpot(m_idButtonHotSpot);
		pHotSpot->m_bHighlighted = idHotSpot == m_idButtonHotSpot;
		BuildStage(m_idButtonHotSpot, UpdateHotSpot, TRUE);
		return(TRUE);
	}
	else
		return(FALSE);
}

//************************************************************************
void CToon::HandleHints(SHOTID idHotSpot)
//************************************************************************
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
}

//************************************************************************
void CToon::HandleBrowseHotSpot(SHOTID idHotSpot)
//************************************************************************
{
	PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
	// are we over a browsable hotspot?
	if (!pHotSpot || !pHotSpot->m_bBrowse)
		idHotSpot = 0; // not browsable

	// see if we need to turn off a hotspot that's on
	if (m_idBrowseHotSpot && m_idBrowseHotSpot != idHotSpot)
	{
		PHOTSPOT pCurBrowseHotSpot = GetHotSpot(m_idBrowseHotSpot);
		// only turn one off if it is enabled
		if (pCurBrowseHotSpot && pCurBrowseHotSpot->m_bVisible && pCurBrowseHotSpot->m_bEnabled)
		{
			pCurBrowseHotSpot->m_bVisible = FALSE;
			BuildStage(m_idBrowseHotSpot, CToon::UpdateHotSpot, TRUE);
		}
		m_idBrowseHotSpot = 0; // turn it off
	}


	// see if we need to turn one on
	if (idHotSpot && m_idBrowseHotSpot != idHotSpot)
	{
		if (!pHotSpot->m_bVisible)
		{
			pHotSpot->m_bVisible = TRUE;
			BuildStage(idHotSpot, CToon::UpdateHotSpot, TRUE);
			m_idBrowseHotSpot = idHotSpot;
		}
	}
}

//************************************************************************
BOOL CToon::OnSetCursor(HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	if (hWndCursor == m_hWnd) // in our window
	{
		if (!m_fCursorVisible)
		{
			if (m_hNullCursor)
			{
				SetCursor(m_hNullCursor);
				return(TRUE);
			}
		}
		else
		if (m_hHotSpotCursor)
		{
			POINT CursorPos;	   

			GetCursorPos( &CursorPos );
			ScreenToClient( m_hWnd, &CursorPos );
			if (IsOnHotSpot(&CursorPos))
			{
		   		SetCursor(m_hHotSpotCursor);
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

//************************************************************************
BOOL CToon::IsOnHotSpot(LPPOINT lpPoint)
//************************************************************************
{
	LPSCENE lpScene = GetApp()->GetCurrentScene();
	if ( (lpScene && lpScene->IsOnHotSpot(lpPoint)) ||
		 (FindHotSpot(lpPoint->x, lpPoint->y) > 0) )
		return(TRUE);
	else
		return(FALSE);
}

//************************************************************************
UINT CToon::OnMCINotify(UINT message, WORD wDeviceID)
//************************************************************************
{
	#ifdef _DEBUG
	Debug("OnMCINotify - message = %d", message);
	#endif

	HTOON hToon = GetToonHandle();
	if (hToon && m_fPlaying) 
	{
		PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);
		#ifdef _DEBUG
		Debug("'%s'\nDropped %ld frames out of %ld (%ld%%)",
				(LPSTR)m_szAVIFile,
				m_lTotalDropped,
				pHotSpot->m_dwTo-pHotSpot->m_dwFrom+1,
				(m_lTotalDropped*100)/(pHotSpot->m_dwTo-pHotSpot->m_dwFrom+1));
		#endif

		if (message == MCI_NOTIFY_SUCCESSFUL)
			HandleAnimationDone();
	}
	return TRUE;
}		

//************************************************************************
BOOL CToon::PlayClick()
//************************************************************************
{
	FNAME szFileName;

	LPSCENE lpScene = GetApp()->GetCurrentScene();
	if (!lpScene)
		return(FALSE);

	// stop everything from playing
	StopPlaying();

	if (!FindContent(CLICK_WAVE, TRUE, szFileName))
		return(FALSE);
	if (FileExistsExpand(szFileName, NULL))
	{
		lpScene->PlaySound(szFileName, FALSE);
		//if (lpScene->GetSound())
		//	return(lpScene->GetSound()->StartFile(szFileName, FALSE, SOUND_CHANNEL, TRUE));
	}
	return(FALSE);
}

//************************************************************************
void CToon::HandleAnimationDone()
//************************************************************************
{
	PHOTSPOT pHotSpot = GetHotSpot(m_idCurHotSpot);

	if (pHotSpot->IsLooping())
	{
		// reset the triggered events
		for (int i = 0; i < pHotSpot->m_nAnimEvents; ++i)
			pHotSpot->m_pAnimEvents[i].bTriggered = FALSE;

		// don't disable cursor for looping animation
		//EnableCursor(TRUE);
		m_lLastFrame = (LONG)pHotSpot->m_dwFrom-1;
		m_lTotalDropped = 0;
		if (m_hSound)
		{
			MCIStop( m_hSound, YES/*bWait*/ );
			MCISeek( m_hSound, 0 );
			MCIPlay( m_hSound, m_hWnd, pHotSpot->m_dwFrom, pHotSpot->m_dwTo );
		}
		else
		{
			MCIStop( m_hMovie, YES/*bWait*/ );
//			MCISeek( m_hMovie, 0 );
			MCIPlay( m_hMovie, m_hWnd, pHotSpot->m_dwFrom, pHotSpot->m_dwTo );
		}
	}
	else
	{
		if (m_hSound)
		{
			MCIClose(m_hSound);
			m_hSound = NULL;
		}

		if (m_hMovie && m_fCloseMovie)
		{
			MCIClose(m_hMovie);
			m_hMovie = NULL;
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
		if (m_PlayingState == CToon::Conclusion && !pHotSpot->m_nEndShots)
			OnNotify(CToon::ConclusionDone);
		else
		{
			if (m_PlayingState == CToon::Auto)
			{
				// don't send an end until we finish playing all
				// of animations involved in intro
				if (!pHotSpot->m_nEndShots)
				{
					OnNotify(CToon::AutoDone);
					// turn on hints once we've played the entire intro
					m_fHintsOn = TRUE;
				}
			}
			else
			if (m_PlayingState == CToon::Normal)
				OnNotify(CToon::ToonDone);
              
            if (!m_fPlaying) // OnNotify's could have started an animation
			{
            	if (CheckSequence(pHotSpot->m_idSequence, pHotSpot->m_nSequence))
            		OnHotSpot(pHotSpot->m_idSequence);
				else
				{
					if (pHotSpot->m_nEndShots) // jump to
					{
						BOOL fTimerSet = FALSE;

						if (pHotSpot->m_uEndDelay)
							fTimerSet = SetTimer(CToon::EndDelayTimer, pHotSpot->m_uEndDelay);
						if (!fTimerSet)
							JumpToRandomAnim(pHotSpot->m_pIdEnd, pHotSpot->m_nEndShots);
					}
					else
						StartIdle();
				}
			}
		}
	}
}

//************************************************************************
void CToon::OnTimer(UINT idTimer)
//************************************************************************
{
	if (idTimer == ANIMATOR_TIMER_ID)
	{
		// we use the animation engine timer id for hints
		if (m_idHintHotSpot && ((timeGetTime() - m_dwHintTime) >= HINT_TIME))
		{
			FNAME szFileName;
			BOOL fPlayed = FALSE;

			PHOTSPOT pHotSpot = GetHotSpot(m_idHintHotSpot);
			if (FindContent(pHotSpot->m_szHintWave, TRUE, szFileName))
			{
				LPSCENE lpScene = GetApp()->GetCurrentScene();
				if (lpScene)
				{
					StopPlaying();
					fPlayed = lpScene->PlaySound(szFileName, TRUE);
					StartIdle();
				}
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
			if (pHotSpot && pHotSpot->m_nEndShots)
				JumpToRandomAnim(pHotSpot->m_pIdEnd, pHotSpot->m_nEndShots);
		}
	}
}


//************************************************************************
BOOL CToon::JumpToRandomAnim(SHOTID FAR *lpAnimID, int nNumShots)
//************************************************************************
{
	SHOTID AnimID;

	if (!nNumShots)
		return(FALSE);

	if (nNumShots > 1)
	{
		WORD wRandom = GetRandomNumber(nNumShots);
		AnimID = lpAnimID[wRandom];
	}
	else
		AnimID = lpAnimID[0];
	return(OnHotSpot(AnimID));
}

//************************************************************************
void CToon::ChangeSpriteStates(PHOTSPOT pHotSpot, BOOL fNormalCompletion, BOOL fEventTrigger,
							BOOL fUpdateDisplay)
//************************************************************************
{	
	int i;
	PHOTSPOT pOtherHotSpot;
	BOOL bRebuild = TRUE;
	BOOL bUpdateDisplay = TRUE;
	//BOOL bEnableCursor;

	// set initial state of rebuild flag
 	bRebuild = 	!pHotSpot->m_bNoUndraw &&			// not flagged as noundraw and
 		   		!lstrlen(pHotSpot->m_szWaveFile) &&	// not a wave file clickable and
		   		(pHotSpot->m_dwTo > pHotSpot->m_dwFrom || pHotSpot->m_bSprite) && // we have some animation frames
		   		!fEventTrigger &&					// not for an event trigger
		   		fUpdateDisplay;						// only for display update

	// any trigger events
	if (pHotSpot->m_nAnimEvents)
	{
		// only update display on successful completion
		// if we abort a shot that has events, we need to do
		// a full rebuild instead of updating one by one
		TriggerEvents(pHotSpot->m_idHotSpot, ALLEVENTS, fNormalCompletion);
		if (!fNormalCompletion)
			bRebuild = TRUE;
	}

	// if this is a stop and not a normal completion
	// then check to see if we have any special things
	// to be handled for a stop.
	if (!fNormalCompletion)
	{
		// handle shows to be done on a stop
		if (pHotSpot->m_nStopShowShots)
		{
			for (i = 0; i < pHotSpot->m_nStopShowShots; ++i)
			{
				pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdStopShow[i]);
				if (!pOtherHotSpot)
					break;
				ChangeSpriteStates(pOtherHotSpot, fNormalCompletion, TRUE, fUpdateDisplay);
//				pOtherHotSpot->m_bVisible = TRUE;
//				if (!bRebuild && fUpdateDisplay)
//					BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, TRUE /*!fEventTrigger*/);
			}
		}
	}
	// handle show command
	if (pHotSpot->m_nShowShots)
	{
		for (i = 0; i < pHotSpot->m_nShowShots; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdShow[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bVisible = TRUE;
			if (!bRebuild && fUpdateDisplay)
				BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, TRUE /*!fEventTrigger*/);
		}
	}
	// handle hide command
	if (pHotSpot->m_nHideShots)
	{
		for (i = 0; i < pHotSpot->m_nHideShots; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdHide[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bVisible = FALSE;
			if (!bRebuild && fUpdateDisplay)
				// we pass fUpdateDisplay parameter based on whether this
				// is a triggered event.  The assumption is that the playing
				// of the video will handle the updating of the display.
				BuildStage(pOtherHotSpot->m_idHotSpot, CToon::UpdateHotSpot, !fEventTrigger || pHotSpot->m_bUpdateNow);
		}
	}
	// handle enable command
	if (pHotSpot->m_nEnableShots)
	{
		for (i = 0; i < pHotSpot->m_nEnableShots; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdEnable[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bEnabled = TRUE;
		}
	}
	// handle disable command
	if (pHotSpot->m_nDisableShots)
	{
		for (i = 0; i < pHotSpot->m_nDisableShots; ++i)
		{
			pOtherHotSpot = GetHotSpot(pHotSpot->m_pIdDisable[i]);
			if (!pOtherHotSpot)
				break;
			pOtherHotSpot->m_bEnabled = FALSE;
		}
	}

	// no rebuild for event trigger
	if (fEventTrigger)
		return;

	// Determine cursor state:
	// if we don't have a normal completion or
	// we have no end shots, then restore the cursor state
	if (!fNormalCompletion || (!pHotSpot->m_nEndShots && m_PlayingState != Conclusion))
		ShowCursor(m_fCursorVisibleState);

	// If we have a normal completion and this hotspot has
	// another to jump to then leave the cursor disabled
	// otherwise use the state from the jump table
	//if (fNormalCompletion && pHotSpot->m_nEndShots)
	//	bEnableCursor = FALSE;
	//else
	//	bEnableCursor = pHotSpot->m_bEnableCursor;
	//if (bEnableCursor)
	//	EnableCursor(TRUE);

	m_fPlaying = FALSE;
	if (bRebuild)
	{
		// we don't need to update the display for an idle animation
		// that jumps to another animation
		// this is used to implement random idle animations, where
		// you randomly jump to an idle animation, and you don't
		// want the screen continuously updated.
		if (fNormalCompletion && pHotSpot->m_bIdle && pHotSpot->m_nEndShots)
			bUpdateDisplay = FALSE;

		// quickly stop the idle sprites so that they
		// won't disappear during BuildStage
		if (pHotSpot->m_bNoIdleStop && m_bIdleSprites)
			StopIdleSprites();
		// rebuild the stage and update display if necessary
		BuildStage(0 /* Hide Nothing */, CToon::HideHotSpot, bUpdateDisplay);
		// restart idle sprites
		if (pHotSpot->m_bNoIdleStop && m_bIdleSprites)
			StartIdleSprites();
	}
}

//************************************************************************
void CToon::OnPaint()
//************************************************************************
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rRepair;
	BOOL fDidInitialize = FALSE;

	HTOON hToon = GetToonHandle();	
	if ((hDC = BeginPaint(m_hWnd, &ps)) != NULL)
	{
		rRepair = ps.rcPaint;

		if (!IsRectEmpty(&rRepair) && !m_fInInitialize)
		{
			//Debug("rRepair = %d %d %d %d",
			//	rRepair.left, rRepair.top, rRepair.right, rRepair.bottom);
			if (!m_fInitialized)
			{
				m_fInInitialize = TRUE;
				Initialize(hDC);
				m_fInInitialize = FALSE;
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
					if (m_Transition.m_fClip && !IsRectEmpty(&m_rGlobalClip))
						IntersectRect(&rRepair, &rRepair, &m_rGlobalClip);

					hPal = m_hToonPal;
					if (hPal)
					{
						hOldPal = SelectPalette(hDC, hPal, FALSE);
						::RealizePalette(hDC);
					}

					SetTransitionParms( &parms, (int)m_Transition.m_uTransition, hDC,
							rRepair.left, rRepair.top,
							rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
							rRepair.left, rRepair.top,
							rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, pWinGDib, ToonDC(m_hToon) );
					SetTransitionTiming(&parms, m_Transition.m_nTicks, m_Transition.m_nStepSize, m_Transition.m_dwTotalTime);
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
						SelectPalette(hDC, hOldPal, TRUE);
						//DeleteObject(hPal);
					}
				}
				m_Transition.m_uTransition = 0;
				//GetApp()->m_bAppNoErase = NO;
			}

			if (hToon)
				ToonPaintDC(hToon, hDC);
		}
		EndPaint(m_hWnd, &ps);
		if (fDidInitialize)
			EndInitialize();
	}
}

//************************************************************************
BOOL CToon::OnToonDraw(HTOON hToon, LONG lFrame)
//************************************************************************
{
	if (m_fVideo)
		lFrame = MCIGetPosition(m_hMovie);
	//Debug("OnToonDraw - lFrame = %ld\n", lFrame);
	if ((lFrame-m_lLastFrame) != 1)
	{
		long lDropped = lFrame - m_lLastFrame - 1;
		if (lDropped > 0)
			m_lTotalDropped += lDropped;
		//Debug("LastFrame = %ld CurrentFrame = %ld\n",
		//			m_lLastFrame, lFrame);
	}
	m_lLastFrame = lFrame;
	ToonDrawCurrentFrame(hToon);

	// check to see if any events need to be fired
	TriggerEvents(m_idCurHotSpot, (DWORD)lFrame);

	return(TRUE);
}

//************************************************************************
void CToon::FadeOut()
//************************************************************************
{
	RGBQUAD rgbQuad[256];
	LPRGBQUAD lpQuad;
	int i;

	// We want to animate from the current palette to black
	hmemcpy(rgbQuad, m_rgbLastColors, sizeof(rgbQuad));
	ToonSetColors(m_hToon, rgbQuad);
	lpQuad = &rgbQuad[ANIMATE_START];

	int iCount = 256 / FADE_INC;
	DWORD dwTime = timeGetTime();
	DWORD dwLastTime = dwTime - FADE_TIME;
	while (--iCount >= 0)
	{
		while ((dwTime - dwLastTime) < FADE_TIME)
		{
			dwTime = timeGetTime();
		}
		dwLastTime = dwTime;
		
		for (i = 0; i < ANIMATE_ENTRIES; ++i)
		{
			if (lpQuad[i].rgbRed > FADE_INC)
				lpQuad[i].rgbRed -= FADE_INC;
			else
				lpQuad[i].rgbRed = 0;
			if (lpQuad[i].rgbGreen > FADE_INC)
				lpQuad[i].rgbGreen -= FADE_INC;
			else
				lpQuad[i].rgbGreen = 0;
			if (lpQuad[i].rgbBlue > FADE_INC)
				lpQuad[i].rgbBlue -= FADE_INC;
			else
				lpQuad[i].rgbBlue = 0;
		}
		ToonAnimateColors(m_hToon, lpQuad, ANIMATE_START, ANIMATE_ENTRIES);
	}
}

//************************************************************************
void CToon::FadeIn()
//************************************************************************
{
	RGBQUAD rgbQuad[ANIMATE_ENTRIES];
	LPRGBQUAD lpColors;
	int iCount, i;
	DWORD dwTime, dwLastTime, dwVol;
	WORD wVol, wInc;
                                
    if (!m_pStageDib)
    	return;
    	
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
		{
			dwTime = timeGetTime();
		}
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
			#ifdef _DEBUG
			Debug("dwVol = %lx wVol = %x\n", dwVol, wVol);
			#endif
			MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, wVol, wVol);
		}
	}
	if (m_fSoundStarted)
	{
		LPSCENE lpScene = GetApp()->GetCurrentScene();
		lpScene->GetSound()->StopChannel(0);
		MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, LOWORD(dwVol), HIWORD(dwVol));
		m_fSoundStarted = FALSE;
	}
	ToonSetColors(GetToonHandle(), m_pStageDib->GetColors());
	UpdateWinGDib();
}

//************************************************************************
void CToon::StartIdleSprites()
//************************************************************************
{
	if (!m_pAnimator)
		return;
	if (m_fIdleSpritesOn)
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
			// remove this sprite from the stage dib for the toon engine
			pHotSpot->m_bVisible = FALSE;
			BuildStage(id, UpdateHotSpot);
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

	// Moved to after processing the hotspots (SMS - 8/26/96)
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

	m_fIdleSpritesOn = TRUE;
}

//************************************************************************
void CToon::StopIdleSprites()
//************************************************************************
{
	if (!m_pAnimator)
		return;
	if (!m_fIdleSpritesOn)
		return;
	for (int id = 1; id <= m_nHotSpots; ++id)
	{
		PHOTSPOT pHotSpot = GetHotSpot(id);
		if (!pHotSpot)
			continue;
		if (pHotSpot->m_bIdle &&
			pHotSpot->m_bSprite &&
			pHotSpot->m_lpSprite)
		{
			// add this sprite back to the stage dib for the toon engine
			pHotSpot->m_bVisible = TRUE;
			BuildStage(id, UpdateHotSpot);
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
	m_fIdleSpritesOn = FALSE;
}

//************************************************************************
void CToon::UpdateArea(LPRECT lpRect)
//************************************************************************
{
	RECT rUpdate;
	
	PDIB pDib = GetWinGDib();
	if (!pDib)
		return;
	if (!m_hToonPal)
		return;
	
	if (lpRect)
		rUpdate = *lpRect;
	else
		SetRect(&rUpdate, 0, 0, pDib->GetWidth(), abs(pDib->GetHeight()));
	
	//Debug("UpdateArea");
	HDC hDC = GetDC(m_hWnd);
	HPALETTE hOldPal = SelectPalette(hDC, m_hToonPal, FALSE);
	::RealizePalette(hDC);
	WinGBitBlt( hDC, rUpdate.left, rUpdate.top, RectWidth(&rUpdate), RectHeight(&rUpdate), ToonDC(m_hToon), rUpdate.left, rUpdate.top);
	SelectPalette(hDC, hOldPal, TRUE);
	ReleaseDC(m_hWnd, hDC);
	//InvalidateRect(m_hWnd, lpRect, FALSE);
	//UpdateWindow(m_hWnd);
	// these functions should work, however, they update the entire window
	//ToonMarkRect(GetToonHandle(), &rArea);
	//ToonPaintRects(GetToonHandle());
}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		PTOON pToon = (PTOON)dwNotifyData;
		pToon->OnSpriteNotify(lpSprite, Notify);
	}
}

//************************************************************************
void CToon::OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (Notify == SN_PAUSEAFTER)
		HandleAnimationDone();
}

//************************************************************************
BOOL CToon::ModifyHotSpot(UINT idCmd, BOOL fShow, BOOL fEnable)
//************************************************************************
{
	PHOTSPOT pHotSpot;
	int id;

	id = FindHotSpot(idCmd);
	if (!id)
		return(FALSE);

	pHotSpot = GetHotSpot(id);
	if (!pHotSpot)
		return(FALSE);
	pHotSpot->m_bVisible = fShow;
	pHotSpot->m_bEnabled = fEnable;
	BuildStage(id, UpdateHotSpot, TRUE);
	return(TRUE);
}

//************************************************************************
BOOL CToon::MoveHotSpot(LPCTSTR lpForegroundDib, int x, int y, BOOL fRelative)
//************************************************************************
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

//************************************************************************
BOOL CToon::StartIdle()
//************************************************************************
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

//************************************************************************
BOOL CToon::StartIdleAnimation()
//************************************************************************
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

//************************************************************************
void CToon::ShowCursor(BOOL fShow)
//************************************************************************
{
	HCURSOR hCursor;

	if (fShow != m_fCursorVisible)
	{
		m_fCursorVisible = fShow;
		if (m_fCursorVisible)
		{
			POINT pt;

			GetCursorPos(&pt);
			ScreenToClient(m_hWnd, &pt);
			if (IsOnHotSpot(&pt))
				hCursor = m_hHotSpotCursor;
			else
				hCursor = (HCURSOR)GetClassWord(m_hWnd, GCW_HCURSOR);
		}
		else
			hCursor = m_hNullCursor;

		if (hCursor)
			SetCursor(hCursor);
	}
}
	
//************************************************************************
void CToon::OnAppActivate(BOOL fActivate)
//************************************************************************
{
	if (!fActivate)
	{
		SetTimer(/* RESET */);
		StopPlaying();
	}
}

//************************************************************************
BOOL CToon::FindContent(LPSTR lpFileSpec, BOOL bTryCommon, LPSTR lpReturnName)
//************************************************************************
{
	BOOL fRet = TRUE;
	FNAME szAppPath;

	// first look in scene directory
	lstrcpy(lpReturnName, m_szPath);
	lstrcat(lpReturnName, lpFileSpec);
	if (!FileExistsExpand( lpReturnName, NULL ))
	{
		// make sure the CD is inserted by checking for app
		GetModuleFileName( GetApp()->GetInstance(), szAppPath, sizeof( szAppPath ) );
		#ifdef _DEBUG
		FNAME szAppName;
		lstrcpy(szAppName, FileName(szAppPath));
		GetApp()->GetContentDir(szAppPath);
		lstrcat(szAppPath, szAppName);
		#endif

		// make user put in CD
		if (!GetApp()->FindContent(szAppPath, NULL))
			return(FALSE);

		// first look in scene directory
		lstrcpy(lpReturnName, m_szPath);
		lstrcat(lpReturnName, lpFileSpec);
		if (!FileExistsExpand( lpReturnName, NULL ))
		{
			if (bTryCommon)
			{
				// next try common directory
				GetApp()->GetContentDir(lpReturnName);
				lstrcat(lpReturnName, COMMON_DIR);
				lstrcat(lpReturnName, lpFileSpec);
				if (!FileExistsExpand(lpReturnName, NULL))
				{
					// use the original path for FindContent
					// that will get called from LoadDibFromFile
					lstrcpy(lpReturnName, m_szPath);
					lstrcat(lpReturnName, lpFileSpec);
					fRet = FALSE;
				}
			}
			else
				fRet = FALSE;
		}
	}
	return(fRet);
}

//************************************************************************
void CToon::TriggerEvents(SHOTID idHotSpot, DWORD dwFrame, BOOL fUpdateDisplay)
//************************************************************************
{
	PHOTSPOT pHotSpot = GetHotSpot(idHotSpot);
	if (pHotSpot)
	{
		BOOL bLoop = pHotSpot->m_bLoop || (pHotSpot->m_bIdle && !pHotSpot->m_nEndShots);
		// see if we are starting a looping animation over
		// if so, reset the trigger state
		if (bLoop && (dwFrame == pHotSpot->m_dwFrom))
		{
			for (int i = 0; i < pHotSpot->m_nAnimEvents; ++i)
				pHotSpot->m_pAnimEvents[i].bTriggered = FALSE;
		}
		for (int i = 0; i < pHotSpot->m_nAnimEvents; ++i)
		{
			if (dwFrame >= pHotSpot->m_pAnimEvents[i].dwFrame &&
				!pHotSpot->m_pAnimEvents[i].bTriggered)
			{
				PHOTSPOT pTriggerHotSpot = GetHotSpot(pHotSpot->m_pAnimEvents[i].idHotSpot);
				if (pTriggerHotSpot)
					ChangeSpriteStates(pTriggerHotSpot, TRUE, TRUE, fUpdateDisplay);
				pHotSpot->m_pAnimEvents[i].bTriggered = TRUE;
			}
		}
	}
}

//************************************************************************
BOOL CToonParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	PTOON pToon = (PTOON)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "bg"))
			GetFilename(pToon->m_szPath, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "prev"))
			GetTransitionDesc(&pToon->m_LeftTransition, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "next"))
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
		if (!lstrcmpi(lpKey, "flags"))
			GetGlobalFlags(pToon, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetSoundTrack(pToon, lpEntry, lpValues, nValues);
		else		  
		if (!lstrcmpi(lpKey, "scalevideo"))
			GetInt(&pToon->m_iScaleVideo, lpEntry, lpValues, nValues);
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
			GetMultipleShots(&pHotSpot->m_pIdShow, &pHotSpot->m_nShowShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hide"))
			GetMultipleShots(&pHotSpot->m_pIdHide, &pHotSpot->m_nHideShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "prehide"))
			GetMultipleShots(&pHotSpot->m_pIdPreHide, &pHotSpot->m_nPreHideShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "preshow"))
			GetMultipleShots(&pHotSpot->m_pIdPreShow, &pHotSpot->m_nPreShowShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "predisable"))
			GetMultipleShots(&pHotSpot->m_pIdPreDisable, &pHotSpot->m_nPreDisableShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "preenable"))
			GetMultipleShots(&pHotSpot->m_pIdPreEnable, &pHotSpot->m_nPreEnableShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "enable"))
			GetMultipleShots(&pHotSpot->m_pIdEnable, &pHotSpot->m_nEnableShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "disable"))
			GetMultipleShots(&pHotSpot->m_pIdDisable, &pHotSpot->m_nDisableShots, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "stopshow"))
			GetMultipleShots(&pHotSpot->m_pIdStopShow, &pHotSpot->m_nStopShowShots, lpEntry, lpValues, nValues);
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
		if (!lstrcmpi(lpKey, "waveifplay"))
		{
			FNAME szDummy;
			LPSTR lpWaveFile;
			if (pToon->m_fReadOnly)
			{ // this is a hack to fix scene 3 in paintbox
				lpWaveFile = szDummy;
				pHotSpot->m_idCommand = 502/*IDC_NEXT*/;
			}
			else
				lpWaveFile = pHotSpot->m_szWaveFile;
			GetFilename(lpWaveFile, lpEntry, lpValues, nValues);
		}
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
		if (!lstrcmpi(lpKey, "event"))
			GetEvent(pHotSpot, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "toon"))
			GetFilename(pHotSpot->m_szAVIFile, lpEntry, lpValues, nValues);
		else		  
		if (!lstrcmpi(lpKey, "scalevideo"))
			GetInt(&pHotSpot->m_iScaleVideo, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "videopos"))
			GetPoint(&pHotSpot->m_ptMovie, lpEntry, lpValues, nValues);
		else
			Print("'%s'\n Unknown key '%s'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetAnimID(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues)
//************************************************************************
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

//************************************************************************
BOOL CToonParser::GetMultipleShots(LPSHOTID FAR *lppShotID, LPINT lpNumShots, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	long l;
	int i;
	LPSTR lpValue;
	LPSHOTID lpShots;

	if (*lpNumShots)
	{
		Print("Duplicate entry:\n'%s'", lpEntry);
		return(FALSE);
	}
	lpShots = (LPSHOTID)ToonAlloc(sizeof(SHOTID)*nValues);
	if (lpShots == NULL)
	{
		Print("Out of memory");
		return(FALSE);
	}
	for (i = 0; i < nValues; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		if ( !(l = GetShotID(lpEntry, lpValue)) )
		{
			ToonFree(lpShots);
			return(FALSE);
		}
		lpShots[i] = l;
	}
	*lppShotID = lpShots;
	*lpNumShots = nValues;
	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetSetSequence(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
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

//************************************************************************
BOOL CToonParser::GetEnd(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	int i;
	LPSHOTID lpShots;

	if (pHotSpot->m_nEndShots)
	{
		Print("Duplicate entry:\n'%s'", lpEntry);
		return(FALSE);
	}

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

		
		lpShots = (LPSHOTID)ToonAlloc(sizeof(SHOTID)*nValues);
		if (lpShots == NULL)
		{
			Print("Out of memory");
			return(FALSE);
		}

		for (i = 0; i < nValues; ++i)
		{
			if ( !(l = GetShotID(lpEntry, lpValue)) )
			{
				ToonFree(lpShots);
				return(FALSE);
			}
			lpShots[i] = l;
			lpValue = GetNextValue(&lpValues);
		}
		pHotSpot->m_nEndShots = nValues;
		pHotSpot->m_pIdEnd = lpShots;
	}
	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetFlags(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
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
		if (!lstrcmpi(lpValue, "invisifread"))
			pHotSpot->m_bVisible = !GetApp()->GetCurrentScene()->GetToon()->m_fReadOnly;
		else
		if (!lstrcmpi(lpValue, "invisifplay"))
			pHotSpot->m_bVisible = GetApp()->GetCurrentScene()->GetToon()->m_fReadOnly;
		else
		if (!lstrcmpi(lpValue, "disabled"))
			pHotSpot->m_bEnabled = FALSE;
		else
		if (!lstrcmpi(lpValue, "noundraw"))
			pHotSpot->m_bNoUndraw = TRUE;
		else
		if (!lstrcmpi(lpValue, "disablecursor"))
			pHotSpot->m_bDisableCursor = TRUE;
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
		if (!lstrcmpi(lpValue, "updatenow"))
			pHotSpot->m_bUpdateNow = TRUE;
		else
		if (!lstrcmpi(lpValue, "noclip"))
			pHotSpot->m_bClipDib = FALSE;
		else
		if (!lstrcmpi(lpValue, "click"))
			pHotSpot->m_bAutoClick = TRUE;
		else
		if (!lstrcmpi(lpValue, "video"))
			pHotSpot->m_fVideo = TRUE;
		else
		if (!lstrcmpi(lpValue, "centervideo"))
			pHotSpot->m_fCenterVideo = TRUE;
		else
		if (!lstrcmpi(lpValue, "noidlestop"))
			pHotSpot->m_bNoIdleStop = TRUE;
		else
		if (!lstrcmpi(lpValue, "restartidle"))
			pHotSpot->m_bRestartIdle = TRUE;
		else
		if (!lstrcmpi(lpValue, "cycleback"))
			pHotSpot->m_bCycleBack = TRUE;
		else
			Print("'%s'\n Unknown flag value '%s'", lpEntry, lpValue);
	}
	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetRectOrPoint(LPRECT lpRect, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
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

//************************************************************************
BOOL CToonParser::GetTransitionDesc(PTRANSITIONDESC pDesc, LPSTR lpEntry, LPSTR lpValues, int nValues, LPINT lpSceneID)
//************************************************************************
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
		pDesc->m_dwTotalTime = (DWORD)l;
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
		pDesc->m_nTicks = (int)l;
	}
 	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetGlobalFlags(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;

	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		if (!lstrcmpi(lpValue, "hintson"))
			pToon->SetHintState(TRUE);
		else
		if (!lstrcmpi(lpValue, "keepsoundtrack"))
			pToon->m_fKeepSoundTrack = TRUE;
		else
		if (!lstrcmpi(lpValue, "video"))
			pToon->m_fVideo = TRUE;
		else
		if (!lstrcmpi(lpValue, "centervideo"))
			pToon->m_fCenterVideo = TRUE;
		else
		if (!lstrcmpi(lpValue, "startidlefirst"))
			pToon->m_fStartIdleFirst = TRUE;
		else
			Print("'%s'\n Unknown flag value '%s'", lpEntry, lpValue);
	}
	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetEvent(PHOTSPOT pHotSpot, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	int nEvents, i;
	LPANIMEVENT lpAnimEvents;
	
	nEvents = nValues / 2;
	lpAnimEvents = (LPANIMEVENT)ToonAlloc(sizeof(ANIMEVENT)*nEvents);
	if (lpAnimEvents == NULL)
	{
		Print("Out of memory");
		return(FALSE);
	}
	for (i = 0; i < nEvents; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			ToonFree(lpAnimEvents);
		  	return(FALSE);
		}
		lpAnimEvents[i].dwFrame = l;

		lpValue = GetNextValue(&lpValues);
		if ( !(l = GetShotID(lpEntry, lpValue)) )
		  	return(FALSE);
		lpAnimEvents[i].idHotSpot = l;
		lpAnimEvents[i].bTriggered = FALSE;
	}
	pHotSpot->m_nAnimEvents = nEvents;
	pHotSpot->m_pAnimEvents = lpAnimEvents;
	return(TRUE);
}

//************************************************************************
BOOL CToonParser::GetSoundTrack(PTOON pToon, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
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
//************************************************************************
void CToonParser::RemapTable(PHOTSPOT pHotSpots)
//************************************************************************
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

			ChangeMultipleShots(pHotSpot->m_pIdEnd, pHotSpot->m_nEndShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdShow, pHotSpot->m_nShowShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdHide, pHotSpot->m_nHideShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdPreHide, pHotSpot->m_nPreHideShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdPreShow, pHotSpot->m_nPreShowShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdPreDisable, pHotSpot->m_nPreDisableShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdPreEnable, pHotSpot->m_nPreEnableShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdEnable, pHotSpot->m_nEnableShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdDisable, pHotSpot->m_nDisableShots, lCurShot, -idx2);
			ChangeMultipleShots(pHotSpot->m_pIdStopShow, pHotSpot->m_nStopShowShots, lCurShot, -idx2);
			for (int i = 0; i < pHotSpot->m_nAnimEvents; ++i)
			{
				if (pHotSpot->m_pAnimEvents[i].idHotSpot == lCurShot)
					pHotSpot->m_pAnimEvents[i].idHotSpot = -idx2;
			}
			if ( pHotSpot->m_idSequence	== lCurShot )	pHotSpot->m_idSequence = -idx2;
		}
	}

	// Go back and change each shot's id to an index, and check for errors, and negate everything
	for ( idx = 1; idx <= iNumShots; idx++ )
	{ // The shot table entries treats shots as 1-n, not 0-(n-1)
		pHotSpot = &pHotSpots[idx-1];

		pHotSpot->m_idHotSpot = idx;

		MultipleShotCheck(pHotSpot->m_pIdEnd, pHotSpot->m_nEndShots);
		MultipleShotCheck(pHotSpot->m_pIdShow, pHotSpot->m_nShowShots);
		MultipleShotCheck(pHotSpot->m_pIdHide, pHotSpot->m_nHideShots);
		MultipleShotCheck(pHotSpot->m_pIdPreHide, pHotSpot->m_nPreHideShots);
		MultipleShotCheck(pHotSpot->m_pIdPreShow, pHotSpot->m_nPreShowShots);
		MultipleShotCheck(pHotSpot->m_pIdPreDisable, pHotSpot->m_nPreDisableShots);
		MultipleShotCheck(pHotSpot->m_pIdPreEnable, pHotSpot->m_nPreEnableShots);
		MultipleShotCheck(pHotSpot->m_pIdEnable, pHotSpot->m_nEnableShots);
		MultipleShotCheck(pHotSpot->m_pIdDisable, pHotSpot->m_nDisableShots);
		MultipleShotCheck(pHotSpot->m_pIdStopShow, pHotSpot->m_nStopShowShots);
		for (int i = 0; i < pHotSpot->m_nAnimEvents; ++i)
			pHotSpot->m_pAnimEvents[i].idHotSpot = ShotCheck(pHotSpot->m_pAnimEvents[i].idHotSpot);
		pHotSpot->m_idSequence	= ShotCheck( pHotSpot->m_idSequence );
	}
}

//************************************************************************
void CToonParser::ChangeMultipleShots(SHOTID FAR *lpShotID, int nNumShots, SHOTID idCur, SHOTID idNew)
//************************************************************************
{
	int i;
	for (i = 0; i < nNumShots; ++i)
	{
		if ( lpShotID[i] == idCur )
			lpShotID[i] = idNew;
	}
}

//************************************************************************
void CToonParser::MultipleShotCheck(SHOTID FAR *lpShotID, int nNumShots)
//************************************************************************
{
	int i;
	for (i = 0; i < nNumShots; ++i)
		lpShotID[i] = ShotCheck(lpShotID[i]);
}


//************************************************************************
BOOL CToon::DrawHotSpot(int x, int y, BOOL bShow, BOOL bUpdate)
//************************************************************************
{
	PHOTSPOT pHotSpot;
	int id;

	id = FindHotSpot(x, y, TRUE);
	if (!id)
		return(FALSE);

	pHotSpot = GetHotSpot(id);
	if (!pHotSpot)
		return(FALSE);

	pHotSpot->m_bVisible = bShow;		
	BuildStage(id, CToon::UpdateHotSpot, bUpdate);

	return(TRUE);
}

//************************************************************************
BOOL CToon::SetHotSpotFG(int x, int y, LPCTSTR lpForegroundDib)
//************************************************************************
{
	PHOTSPOT pHotSpot;
	int id;
                 
	id = FindHotSpot(x, y, TRUE);
	if (!id)
		return(FALSE);

	pHotSpot = GetHotSpot(id);
	if (!pHotSpot)
		return(FALSE);

	lstrcpy(pHotSpot->m_szForegroundDib, lpForegroundDib);
	if (lstrlen(pHotSpot->m_szForegroundDib))
	{
		FNAME szFileName;

		if (!FindContent(pHotSpot->m_szForegroundDib, TRUE, szFileName))
		{
			Print("Error opening file '%s'", (LPSTR)pHotSpot->m_szForegroundDib);
			return(FALSE);
		}              
		
		// Remove previous dib
		if (pHotSpot->m_pForegroundDib)
			delete pHotSpot->m_pForegroundDib;
			
		if (!(pHotSpot->m_pForegroundDib = CDib::LoadDibFromFile(szFileName, m_hPal, FALSE, FALSE)))
		{
			Debug("Error opening file '%s'", (LPSTR)szFileName);
			return(FALSE);
		}       
	}
    
    return TRUE;
}
