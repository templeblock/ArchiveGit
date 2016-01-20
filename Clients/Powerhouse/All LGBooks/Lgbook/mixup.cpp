#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "lgbook.h"
#include "mixup.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "mixupdef.h"

class CMixupParser : public CParser
{
public:
	CMixupParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetIntros(LPMIXUPSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSuccess(LPMIXUPSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetAnim(LPOUTFIT pOutfit, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CMixupScene::CMixupScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_iLevel = 0;
	m_nOutfits = 0;
	m_pOutfits = NULL;
	SetRectEmpty(&m_rTop);
	SetRectEmpty(&m_rMid);
	SetRectEmpty(&m_rBot);
	SetRectEmpty(&m_rGameArea);
	m_pRandomTable = NULL;
	m_pTopOutfit = NULL;
	m_pMidOutfit = NULL;
	m_pBotOutfit = NULL;
	m_pDstOutfit = NULL;
	m_pSound = NULL;
	for (int i = 0; i < NUM_LEVELS; ++i)
	{
		m_szIntros[i][0] = '\0';
		m_szSuccess[i][0] = '\0';
	}
	m_szSoundTrack[0] = '\0';
	m_SoundPlaying = NotPlaying;
	m_fInteractivity = FALSE;
	m_fLoadOnDemand = FALSE;
}

//************************************************************************
CMixupScene::~CMixupScene()
//************************************************************************
{
	if ( m_pAnimator )
		m_pAnimator->DeleteAll();
	if (m_pOutfits)
		delete [] m_pOutfits;
	if (m_pRandomTable)
		FreeUp(m_pRandomTable);
	if (m_pSound)
		delete m_pSound;
}

//************************************************************************
int CMixupScene::GetLevel()
//************************************************************************
{
	return(m_iLevel);
}

//************************************************************************
void CMixupScene::ChangeLevel(int iNewLevel)
//************************************************************************
{
	m_iLevel = iNewLevel;
	ResetTop();
	ResetMid();
	ResetBot();
	ResetExtra();
	StartGame(TRUE);
}

//************************************************************************
BOOL CMixupScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CGBScene::OnInitDialog(hWnd, hWndControl, lParam);
	return( TRUE );
}

//************************************************************************
void CMixupScene::ToonInitDone()
//************************************************************************
{
	if (m_pAnimator)
	{
		HGLOBAL hData;
		LPSTR lpTableData;
		POINT ptOrigin;
		FNAME szFileName;
		LPSPRITE lpSprite;
		int i;

		if (GetToon())
			GetToon()->SetSendMouseMode(TRUE);

		// read in the connect point information
		lpTableData = GetResourceData(m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CMixupParser parser(lpTableData);
			if (m_nOutfits = parser.GetNumEntries())
			{
				// first entry is for global settings
				m_nOutfits -= 1;
				m_pOutfits = new COutfit[m_nOutfits];
				if (m_pOutfits)
					parser.ParseTable((DWORD)this);
			}
		}
		if (hData)
		{
			UnlockResource( hData );
			FreeResource( hData );
		}

		if (!IsRectEmpty(&m_rGameArea))
			m_pAnimator->SetClipRect(&m_rGameArea);
		ptOrigin.x = 0;
		ptOrigin.y = 0;
		for (i = 0; i < m_nOutfits; ++i)
		{
			// load the animation sprite if needed
			// load first, cause in needs to be on top in z-order
			if (lstrlen(m_pOutfits[i].m_szAnim))
			{
				lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
				GetPathName(szFileName, m_pOutfits[i].m_szAnim);
				lpSprite->AddCells( szFileName, m_pOutfits[i].m_nCells, NULL, NULL, TRUE );
				lpSprite->Jump(m_pOutfits[i].m_ptAnim.x, m_pOutfits[i].m_ptAnim.y);
				lpSprite->SetCellsPerSec(m_pOutfits[i].m_nSpeed);
				m_pOutfits[i].m_lpAnimSprite = lpSprite;
				if (m_pOutfits[i].m_nPauseTime)
				{
					// set initial pause
					lpSprite->Pause(m_pOutfits[i].m_nPauseTime);
					// play cells to end and back to beginning
					//lpSprite->SetCycleBack(TRUE);
					//lpSprite->AddCmdPauseAfterCell(0, PAUSE_FOREVER);
					// marks beginning of a set of commands to repeat
					lpSprite->AddCmdBegin();
					// pause after first cell is displayed
					lpSprite->AddCmdPauseAfterCell(m_pOutfits[i].m_nCells-1, m_pOutfits[i].m_nPauseTime);
					// repeat the commands starting at begin
					lpSprite->AddCmdRepeat();
				}

			}

			if (!m_fLoadOnDemand)
			{
				// load the sprite for the top section
				lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
				GetPathName(szFileName, m_pOutfits[i].m_szTop);
				lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
				lpSprite->Jump(m_rTop.left, m_rTop.top);
				m_pOutfits[i].m_lpTopSprite = lpSprite;

				// load the sprite for the middle section
				lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
				GetPathName(szFileName, m_pOutfits[i].m_szMid);
				lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
				lpSprite->Jump(m_rMid.left, m_rMid.top);
				m_pOutfits[i].m_lpMidSprite = lpSprite;

				// load the sprite for the bottom section
				lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
				GetPathName(szFileName, m_pOutfits[i].m_szBot);
				lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
				lpSprite->Jump(m_rBot.left, m_rBot.top);
				m_pOutfits[i].m_lpBotSprite = lpSprite;
			}
		}

		m_pSound = new CSound(TRUE);
		if (m_pSound)
			m_pSound->Open( NULL/*"powersnd.dll"*/ );

		// show the 0th outfit
		m_pRandomTable = Alloc(m_nOutfits * 4);
		if (m_pRandomTable)
		{
			ResetTop();
			ResetMid();
			ResetBot();
			ResetExtra();
			StartGame(TRUE);
		}
	}
}

//************************************************************************
void CMixupScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_fInteractivity)
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			else
				GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
			break;
		}
		case IDC_HELP:
		{
			if (m_iLevel != 2)
				PlayIntro();
			else
				PlayOutfitIntro();
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
				if (m_pSound)
				{
					m_pSound->StopAndFree();
					m_pSound->Activate(FALSE);
				}
				FNAME szFileName;
				GetPathName(szFileName, CLICK_WAVE);
				CSound sound;
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
BOOL CMixupScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, MM_WOM_DONE, OnWomDone);

		default:
			return(FALSE);
	}
}

//************************************************************************
void CMixupScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	POINT pt;
	int iRandom;

	if (m_pSound)
	{
		if (m_SoundPlaying == IntroPlaying)
		{
			m_pSound->StopChannel(INTRO_CHANNEL);
			m_SoundPlaying = NotPlaying;
			if (m_iLevel != 2)
				PlaySoundTrack();
			else
				PlayOutfitIntro();
			return;
		}
		else if (m_SoundPlaying == CluePlaying)
		{
			m_SoundPlaying = NotPlaying;
			PlaySoundTrack();
			return;
		}
	}
	
	pt.x = x;
	pt.y = y;
	if (!IsRectEmpty(&m_rGameArea))
	{
		if (!PtInRect(&m_rGameArea, pt))
			return;
	}

	if (PtInRect(&m_rTop, pt) && (m_iLevel != 1))
	{
		PlayClick();
		iRandom = GetRandom(GetTop(0));
		ShowOutfit(m_pTopOutfit, TopPiece, FALSE);
		m_pTopOutfit = m_pOutfits + iRandom;
		ShowOutfit(m_pTopOutfit, TopPiece, TRUE);
	}
	else
	if (PtInRect(&m_rMid, pt))
	{
		PlayClick();
		iRandom = GetRandom(GetMid(0));
		ShowOutfit(m_pMidOutfit, MidPiece, FALSE);
		m_pMidOutfit = m_pOutfits + iRandom;
		ShowOutfit(m_pMidOutfit, MidPiece, TRUE);
	}
	else
	if (PtInRect(&m_rBot, pt))
	{
		PlayClick();
		iRandom = GetRandom(GetBot(0));
		ShowOutfit(m_pBotOutfit, BotPiece, FALSE);
		m_pBotOutfit = m_pOutfits + iRandom;
		ShowOutfit(m_pBotOutfit, BotPiece, TRUE);
	}
	
	// check for success
	if (m_iLevel == 0)
	{
		// if all are the same we have success
		if (m_pTopOutfit == m_pMidOutfit && m_pTopOutfit == m_pBotOutfit)
		{
			PlaySuccess(m_szSuccess[0]);
			m_SoundPlaying = CluePlaying;
		}
	}
	else
	if (m_iLevel == 1)
	{
		// if all are the same we have success
		if (m_pTopOutfit == m_pMidOutfit && m_pTopOutfit == m_pBotOutfit)
		{
			PlaySuccess(m_szSuccess[1]);
			// If in "play" mode, go to the next scene - note: this can only happen in
			// levels 2 and 3 right now, level 1 doesn't have a success state (SMS).
			if (GetApp()->m_fPlaying)
				CGBScene::OnCommand (m_hWnd, IDC_NEXT, NULL, 0);
			else
				StartGame();
		}
	}
	else
	if (m_iLevel == 2)
	{
		// if all are the same we have success
		if (m_pTopOutfit == m_pDstOutfit &&
			m_pTopOutfit == m_pMidOutfit && m_pTopOutfit == m_pBotOutfit)
		{
			PlaySuccess(m_szSuccess[2]);
			// If in "play" mode, go to the next scene - note: this can only happen in
			// levels 2 and 3 right now, level 1 doesn't have a success state (SMS).
			if (GetApp()->m_fPlaying)
				CGBScene::OnCommand (m_hWnd, IDC_NEXT, NULL, 0);
			else
				StartGame();
		}
	}
}

//************************************************************************
void CMixupScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CMixupScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CMixupScene::OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	SoundPlaying WasPlaying = m_SoundPlaying;
	m_SoundPlaying = NotPlaying;
	if (WasPlaying == IntroPlaying)
	{
		// play background soundtrack
		if (m_iLevel != 2)
			PlaySoundTrack();
		else
			PlayOutfitIntro();
	}
	else
	if (WasPlaying == CluePlaying)
	{
		PlaySoundTrack();
	}
}

//************************************************************************
void CMixupScene::ShowOutfit(LPOUTFIT pOutfit, MixPiece piece, BOOL bShow)
//************************************************************************
{
	LPSPRITE lpSprite = NULL, lpAnimSprite = NULL;

	if (piece == TopPiece)
		lpSprite = GetSprite(&pOutfit->m_lpTopSprite, pOutfit->m_szTop, (LPPOINT)&m_rTop);
	else
	if (piece == MidPiece)
		lpSprite = GetSprite(&pOutfit->m_lpMidSprite, pOutfit->m_szMid, (LPPOINT)&m_rMid);
	else
	if (piece == BotPiece)
		lpSprite = GetSprite(&pOutfit->m_lpBotSprite, pOutfit->m_szBot, (LPPOINT)&m_rBot);

	if (!lpSprite)
		return;

	lpAnimSprite = pOutfit->m_lpAnimSprite;
	if (lpAnimSprite && lpAnimSprite->IsVisible() != bShow)
	{
		RECT rSprite, rAnim, rSect;

		lpSprite->Location(&rSprite);
		lpAnimSprite->Location(&rAnim);
		if (IntersectRect(&rSect, &rSprite, &rAnim))
			lpAnimSprite->Show(bShow);
	}
	lpSprite->Show(bShow);
}

//************************************************************************
LPSPRITE CMixupScene::GetSprite(LPSPRITE FAR *lppSprite, LPSTR lpFileName, LPPOINT lpLocation)
//************************************************************************
{
	LPSPRITE lpSprite;
	POINT ptOrigin;
	FNAME szFileName;

	lpSprite = *lppSprite;

	if (!lpSprite)
	{
		ptOrigin.x = ptOrigin.y = 0;
		lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
		GetPathName(szFileName, lpFileName);
		lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
		lpSprite->Jump(lpLocation->x, lpLocation->y);
		*lppSprite = lpSprite;
	}
	return(lpSprite);
}

//************************************************************************
void CMixupScene::ResetTop()
//************************************************************************
{
	ResetTable(GetTop(0));
}

//************************************************************************
void CMixupScene::ResetMid()
//************************************************************************
{
	ResetTable(GetMid(0));
}

//************************************************************************
void CMixupScene::ResetBot()
//************************************************************************
{
	ResetTable(GetBot(0));
}

//************************************************************************
void CMixupScene::ResetExtra()
//************************************************************************
{
	ResetTable(GetExtra(0));
}

//************************************************************************
void CMixupScene::StartGame(BOOL fPlayIntro)
//************************************************************************
{
	if (m_pTopOutfit)
		ShowOutfit(m_pTopOutfit, TopPiece, FALSE);
	if (m_pMidOutfit)
		ShowOutfit(m_pMidOutfit, MidPiece, FALSE);
	if (m_pBotOutfit)
		ShowOutfit(m_pBotOutfit, BotPiece, FALSE);
//	if (m_iLevel == 0)
//	{
//		*GetTop(0) = TRUE;
//		*GetMid(0) = TRUE;
//		*GetBot(0) = TRUE;
//
//		m_pTopOutfit = &m_pOutfits[0];
//		m_pMidOutfit = &m_pOutfits[0];
//		m_pBotOutfit = &m_pOutfits[0];
//	}
//	else
//	if (m_iLevel == 1)
	if (m_iLevel == 0 || m_iLevel == 1)
	{
		int iTopRandom, iRandom;

		iTopRandom = GetRandom(GetTop(0));
		m_pTopOutfit = &m_pOutfits[iTopRandom];
		do
			iRandom = GetRandom(GetMid(0));
		while (iRandom == iTopRandom);
		ResetMid();
		*GetMid(iRandom) = TRUE;
		m_pMidOutfit = &m_pOutfits[iRandom];
		do
			iRandom = GetRandom(GetBot(0));
		while (iRandom == iTopRandom);
		ResetBot();
		*GetBot(iRandom) = TRUE;
		m_pBotOutfit = &m_pOutfits[iRandom];
	}
	else
	if (m_iLevel == 2)
	{
		int iRandom;

		do
			iRandom = GetRandom(GetExtra(0));
		while (iRandom == 0);
		m_pDstOutfit = &m_pOutfits[iRandom];
		*GetExtra(iRandom) = TRUE;

		*GetTop(0) = TRUE;
		*GetMid(0) = TRUE;
		*GetBot(0) = TRUE;

		m_pTopOutfit = &m_pOutfits[0];
		m_pMidOutfit = &m_pOutfits[0];
		m_pBotOutfit = &m_pOutfits[0];
	}
	ShowOutfit(m_pTopOutfit, TopPiece, TRUE);
	ShowOutfit(m_pMidOutfit, MidPiece, TRUE);
	ShowOutfit(m_pBotOutfit, BotPiece, TRUE);
	if (fPlayIntro)
		PlayIntro();
	else
	if (m_iLevel == 2)
		PlayOutfitIntro();
	else
		PlaySoundTrack();
}

//************************************************************************
void CMixupScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

	if (GetToon())
		GetToon()->SetHintState(FALSE);
	if (m_pSound)
	{
		m_pSound->StopChannel(INTRO_CHANNEL);
 		GetPathName(szFileName, m_szIntros[m_iLevel]);
		m_pSound->StartFile(szFileName, FALSE, INTRO_CHANNEL, FALSE, m_hWnd);
		m_SoundPlaying = IntroPlaying;
	}
}

//************************************************************************
void CMixupScene::PlaySoundTrack()
//************************************************************************
{
	FNAME szFileName;

	if (GetToon())
		GetToon()->SetHintState(TRUE);
	if (m_pSound)
	{
		m_pSound->StopChannel(SOUNDTRACK_CHANNEL);
 		GetPathName(szFileName, m_szSoundTrack);
		m_pSound->StartFile(szFileName, TRUE, SOUNDTRACK_CHANNEL, FALSE, m_hWnd);
		m_SoundPlaying = SoundtrackPlaying;
	}
}

//************************************************************************
void CMixupScene::PlayOutfitIntro()
//************************************************************************
{
	GetToon()->SetHintState(FALSE);
	if (m_pSound)
	{
		FNAME szFileName;

		GetPathName(szFileName, m_pDstOutfit->m_szWave3);
		m_pSound->StopAndFree();
		m_pSound->Activate(FALSE);
		CSound sound;
		sound.StartFile(szFileName, FALSE, -1, TRUE);
		m_pSound->Activate(TRUE);
		m_SoundPlaying = CluePlaying;
	}
}

//************************************************************************
void CMixupScene::PlayClick()
//************************************************************************
{
	FNAME szFileName;

	if (m_pSound)
	{
		GetPathName(szFileName, "click.wav");
		m_pSound->StartFile(szFileName, FALSE, CLICK_CHANNEL, FALSE);
	}
}

//************************************************************************
void CMixupScene::PlaySuccess (LPSTR lpSuccess)
//************************************************************************
{
	FNAME szFileName;

	GetToon()->SetHintState (FALSE);
	if (m_pSound)
	{
		if (lstrlen (lpSuccess))
			GetPathName (szFileName, lpSuccess);
		else
			GetPathName (szFileName, "SUCCESS.WAV");
		m_pSound->StopAndFree();
		m_pSound->Activate (FALSE);
		CSound sound;
		sound.StartFile (szFileName, FALSE, -1, TRUE);
		m_pSound->Activate (TRUE);
		m_SoundPlaying = NotPlaying;
	}
}

//************************************************************************
int CMixupScene::GetRandom(LPBYTE lpTable)
//************************************************************************
{
	int i, iRandom;

	// do a quick check to see of all numbers are in use
	for (i = 0; i < m_nOutfits; ++i)
	{
		// do we have an open spot?
		if (!lpTable[i])
			break;
	}
	// if all are in use reset the table
	if (i == m_nOutfits)
		ResetTable(lpTable);

	while (TRUE)
	{
		// get a random number
		iRandom = GetRandomNumber(m_nOutfits);
		// now see if it's in use
		if (!lpTable[iRandom])
		{
			lpTable[iRandom] = TRUE;
			break;
		}
	}
	return(iRandom);
}

//************************************************************************
void CMixupScene::ResetTable(LPBYTE lpTable)
//************************************************************************
{
	int i;

	for (i = 0; i < m_nOutfits; ++i)
		lpTable[i] = FALSE;
}

//************************************************************************
BOOL CMixupScene::PlaySound(LPCTSTR lpWaveFile, BOOL fHint)
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
		// this nonsense is here because in order to make buttons
		// in action strip play the click wave when they are clicked
		// on we need to synchronously play the sound so the click
		// happens while the button is down.  WaveMix does not play
		// sounds synchronously, so we deactivate WaveMix and use
		// sndPlaySound stuff.
		if (m_pSound)
			m_pSound->Activate(FALSE);
		CSound sound;
		fRet = sound.StartFile((LPSTR)lpWaveFile, FALSE, -1, TRUE);
		if (m_pSound)
			m_pSound->Activate(TRUE);
	}
	return(fRet);
}

//************************************************************************
BOOL CMixupScene::IsOnHotSpot(LPPOINT lpPoint)
//************************************************************************
{
	if (PtInRect(&m_rTop, *lpPoint) && (m_iLevel != 1))
		return(TRUE);
	else
	if (PtInRect(&m_rMid, *lpPoint))
		return(TRUE);
	if (PtInRect(&m_rBot, *lpPoint))
		return(TRUE);
	else
		return(FALSE);
}

//************************************************************************
COutfit::COutfit()
//************************************************************************
{
	m_szTop[0] = '\0';
	m_szMid[0] = '\0';
	m_szBot[0] = '\0';
	m_szWave3[0] = '\0';
	m_szSuccess[0] = '\0';
	m_szAnim[0] = '\0';
	m_ptAnim.x = m_ptAnim.y = 0;
	m_nCells = 1;
	m_nSpeed = 4;
	m_nPauseTime = 3000;
	m_lpTopSprite = NULL;
	m_lpMidSprite = NULL;
	m_lpBotSprite = NULL;
	m_lpAnimSprite = NULL;
}

//************************************************************************
COutfit::~COutfit()
//************************************************************************
{
}

//************************************************************************
BOOL CMixupParser::GetIntros(LPMIXUPSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
BOOL CMixupParser::GetSuccess(LPMIXUPSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
		lstrcpy(lpScene->m_szSuccess[i], lpValue);
	}
 	return(TRUE);
}

//************************************************************************
BOOL CMixupParser::GetAnim(LPOUTFIT pOutfit, LPSTR lpEntry, LPSTR lpValues, int nValues)
//************************************************************************
{
	LPSTR lpValue;
	long l;
	BOOL bError;

	if (nValues != 6)
	{
			Print("'%s'\n Invalid animation key '%s'", lpEntry, lpValues);
			return(FALSE);
	}

	lpValue = GetNextValue(&lpValues);
	lstrcpy(pOutfit->m_szAnim, lpValue);

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pOutfit->m_ptAnim.x = (int)l;

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pOutfit->m_ptAnim.y = (int)l;

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pOutfit->m_nCells = (int)l;

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pOutfit->m_nSpeed = (int)l;

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
	  	return(FALSE);
	}
	pOutfit->m_nPauseTime = (int)l;

 	return(TRUE);
}

//************************************************************************
BOOL CMixupParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPMIXUPSCENE lpScene = (LPMIXUPSCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "toprect"))
			GetRect(&lpScene->m_rTop, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "midrect"))
			GetRect(&lpScene->m_rMid, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "botrect"))
			GetRect(&lpScene->m_rBot, lpEntry, lpValues, nValues);
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
		if (!lstrcmpi(lpKey, "interactivity"))
			GetInt(&lpScene->m_fInteractivity, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "loadondemand"))
			GetInt(&lpScene->m_fLoadOnDemand, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "gamearea"))
			GetRect(&lpScene->m_rGameArea, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		LPOUTFIT pOutfit = lpScene->GetOutfit(nIndex-1);
		if (!lstrcmpi(lpKey, "top"))
			GetFilename(pOutfit->m_szTop, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "mid"))
			GetFilename(pOutfit->m_szMid, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "bot"))
			GetFilename(pOutfit->m_szBot, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "wave3"))
			GetFilename(pOutfit->m_szWave3, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success"))
			GetFilename(pOutfit->m_szSuccess, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "anim"))
			GetAnim(pOutfit, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

