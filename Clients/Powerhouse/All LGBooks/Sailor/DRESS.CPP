#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "dress.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "dressdef.h"

class CDressParser : public CParser
{
public:
	CDressParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetIntros(LPDRESSSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
	BOOL GetSuccess(LPDRESSSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

//************************************************************************
CDressScene::CDressScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_iLevel = 0;
	m_nOutfits = 0;
	m_pOutfits = NULL;
	SetRectEmpty(&m_rTop);
	SetRectEmpty(&m_rMid);
	SetRectEmpty(&m_rBot);
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
}

//************************************************************************
CDressScene::~CDressScene()
//************************************************************************
{
	if (m_pOutfits)
		delete [] m_pOutfits;
	if (m_pRandomTable)
		FreeUp(m_pRandomTable);
	if (m_pSound)
		delete m_pSound;
}

//************************************************************************
int CDressScene::GetLevel()
//************************************************************************
{
	return(m_iLevel);
}

//************************************************************************
void CDressScene::ChangeLevel(int iNewLevel)
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
BOOL CDressScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CScene::OnInitDialog(hWnd, hWndControl, lParam);
	return( TRUE );
}

//************************************************************************
void CDressScene::ToonInitDone()
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
			CDressParser parser(lpTableData);
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
		ptOrigin.x = 0;
		ptOrigin.y = 0;
		for (i = 0; i < m_nOutfits; ++i)
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
void CDressScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_DRESSI)
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			else
				GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
			break;
		}
		case IDC_HELP:
		{
			if (m_iLevel == 2)
				PlayOutfitIntro();
			else
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

				m_pSound->StopAndFree();
				if (m_pSound)
					m_pSound->Activate(FALSE);
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
BOOL CDressScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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
void CDressScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
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
			PlaySoundTrack();
			PlayOutfitIntro();
		}
		else
		if (m_SoundPlaying == SuccessPlaying)
			return;
	}
	
	pt.x = x;
	pt.y = y;
	if (PtInRect(&m_rTop, pt) && (m_iLevel != 1))
	{
		PlayClick();
		iRandom = GetRandom(GetTop(0));
		m_pTopOutfit->m_lpTopSprite->Show(FALSE);
		m_pTopOutfit = m_pOutfits + iRandom;
		m_pTopOutfit->m_lpTopSprite->Show(TRUE);
	}
	else
	if (PtInRect(&m_rMid, pt))
	{
		PlayClick();
		iRandom = GetRandom(GetMid(0));
		m_pMidOutfit->m_lpMidSprite->Show(FALSE);
		m_pMidOutfit = m_pOutfits + iRandom;
		m_pMidOutfit->m_lpMidSprite->Show(TRUE);
	}
	else
	if (PtInRect(&m_rBot, pt))
	{
		PlayClick();
		iRandom = GetRandom(GetBot(0));
		m_pBotOutfit->m_lpBotSprite->Show(FALSE);
		m_pBotOutfit = m_pOutfits + iRandom;
		m_pBotOutfit->m_lpBotSprite->Show(TRUE);
	}
	// check for success
	if (m_iLevel == 1)
	{
		// if all are the same we have success
		if (m_pTopOutfit == m_pMidOutfit && m_pTopOutfit == m_pBotOutfit)
			PlaySuccess(m_pTopOutfit->m_szSuccess);
	}
	else
	if (m_iLevel == 2)
	{
		// if all are the same we have success
		if (m_pTopOutfit == m_pDstOutfit &&
			m_pTopOutfit == m_pMidOutfit && m_pTopOutfit == m_pBotOutfit)
		{
			PlaySuccess(m_pTopOutfit->m_szSuccess);
		}
	}
}

//************************************************************************
void CDressScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CDressScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CDressScene::OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	SoundPlaying WasPlaying = m_SoundPlaying;
	m_SoundPlaying = NotPlaying;
	if (WasPlaying == IntroPlaying)
	{
		// play background soundtrack
		PlaySoundTrack();
		PlayOutfitIntro();
	}
	else
	if (WasPlaying == SuccessPlaying)
	{
		StartGame();
	}
	else
	if (WasPlaying == CluePlaying)
	{
		if (SOUNDTRACK_CHANNEL == CLUE_CHANNEL)
			PlaySoundTrack();
	}
}

//************************************************************************
LPSTR CDressScene::GetPathName(LPSTR lpPathName, LPSTR lpFileName)
//************************************************************************
{
	GetToon()->GetContentDir(lpPathName);
	//lstrcat(lpPathName, CONTENT_DIR);
	lstrcat(lpPathName, lpFileName);
	return(lpPathName);
}

//************************************************************************
void CDressScene::ResetTop()
//************************************************************************
{
	ResetTable(GetTop(0));
}

//************************************************************************
void CDressScene::ResetMid()
//************************************************************************
{
	ResetTable(GetMid(0));
}

//************************************************************************
void CDressScene::ResetBot()
//************************************************************************
{
	ResetTable(GetBot(0));
}

//************************************************************************
void CDressScene::ResetExtra()
//************************************************************************
{
	ResetTable(GetExtra(0));
}

//************************************************************************
void CDressScene::StartGame(BOOL fPlayIntro)
//************************************************************************
{
	if (m_pTopOutfit)
		m_pTopOutfit->m_lpTopSprite->Show(FALSE);
	if (m_pMidOutfit)
		m_pMidOutfit->m_lpMidSprite->Show(FALSE);
	if (m_pBotOutfit)
		m_pBotOutfit->m_lpBotSprite->Show(FALSE);
	if (m_iLevel == 0)
	{
		*GetTop(0) = TRUE;
		*GetMid(0) = TRUE;
		*GetBot(0) = TRUE;

		m_pTopOutfit = &m_pOutfits[0];
		m_pMidOutfit = &m_pOutfits[0];
		m_pBotOutfit = &m_pOutfits[0];
	}
	else
	if (m_iLevel == 1)
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
	m_pTopOutfit->m_lpTopSprite->Show(TRUE);
	m_pMidOutfit->m_lpMidSprite->Show(TRUE);
	m_pBotOutfit->m_lpBotSprite->Show(TRUE);

	if (fPlayIntro)
		PlayIntro();
	else
	if (m_iLevel == 2)
		PlayOutfitIntro();
	else
	if (SUCCESS_CHANNEL == SOUNDTRACK_CHANNEL)
		PlaySoundTrack();
}

//************************************************************************
void CDressScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

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
void CDressScene::PlaySoundTrack()
//************************************************************************
{
	FNAME szFileName;

	GetToon()->SetHintState(TRUE);
	if (m_pSound)
	{
		m_pSound->StopChannel(SOUNDTRACK_CHANNEL);
 		GetPathName(szFileName, m_szSoundTrack);
		m_pSound->StartFile(szFileName, TRUE, SOUNDTRACK_CHANNEL, FALSE);
	}
}

//************************************************************************
void CDressScene::PlayOutfitIntro()
//************************************************************************
{
	if (m_iLevel == 2)
	{
		GetToon()->SetHintState(FALSE);
		if (m_pSound)
		{
			FNAME szFileName;

			m_pSound->StopChannel(CLUE_CHANNEL);
			GetPathName(szFileName, m_pDstOutfit->m_szWave3);
			m_pSound->StartFile(szFileName, FALSE, CLUE_CHANNEL, FALSE, m_hWnd);
			m_SoundPlaying = CluePlaying;
		}
	}
}

//************************************************************************
void CDressScene::PlayClick()
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
void CDressScene::PlaySuccess(LPSTR lpSuccess)
//************************************************************************
{
	FNAME szFileName;

	GetToon()->SetHintState(FALSE);
	if (m_pSound)
	{
		if (lstrlen(lpSuccess))
			GetPathName(szFileName, lpSuccess);
		else
			GetPathName(szFileName, "SUCCESS.WAV");
		m_pSound->StopChannel(SUCCESS_CHANNEL);
		m_pSound->StartFile(szFileName, FALSE, SUCCESS_CHANNEL, FALSE, m_hWnd);
		m_SoundPlaying = SuccessPlaying;
	}
}

//************************************************************************
int CDressScene::GetRandom(LPBYTE lpTable)
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
void CDressScene::ResetTable(LPBYTE lpTable)
//************************************************************************
{
	int i;

	for (i = 0; i < m_nOutfits; ++i)
		lpTable[i] = FALSE;
}

//************************************************************************
BOOL CDressScene::PlaySound(LPCTSTR lpWaveFile, BOOL fHint)
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
BOOL CDressScene::IsOnHotSpot(LPPOINT lpPoint)
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
	m_lpTopSprite = NULL;
	m_lpMidSprite = NULL;
	m_lpBotSprite = NULL;
}

//************************************************************************
COutfit::~COutfit()
//************************************************************************
{
}

//************************************************************************
BOOL CDressParser::GetIntros(LPDRESSSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
BOOL CDressParser::GetSuccess(LPDRESSSCENE lpScene, LPSTR lpEntry, LPSTR lpValues, int nValues)
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
BOOL CDressParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPDRESSSCENE lpScene = (LPDRESSSCENE)dwUserData;
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
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

