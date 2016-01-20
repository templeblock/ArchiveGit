///////////////////////////////////////////////////////////////////////////////
//
// Soundoff.cpp
//
// Copyright (C) 1996 - Powerhouse Entertainment, Inc.
//
// Change History
//-----------------------------------------------------------------------------
// 06/17/96		Steve Suggs (SMS)	Modified to use MCI wave playing instead of the
//									wavemix.dll due to older sound board drivers not
//									always returning a done indication.
// 07/08/96		Steve Suggs	(SMS)	Added handling space bar to go back to main menu.
// 07/10/96		Steve Suggs	(SMS)	Fixed problem of not being able to click on animal heads.
// 07/15/96		Steve Suggs (SMS)	Changed PlayIntro to start animations before intro. wave.
//
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <mmsystem.h>
#include "farmer.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "farmerid.h"
#include "soundoff.h"
#include "sounddef.h"


LOCAL void CALLBACK OnSoundNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);
                                       
static RECT rGameClipArea = {47, 30, 592, 433};
static RECT rReplayButtonArea = {295,345,405,410};

Barn Layout = {//animal head    bad     body    guess
                -1,     NULL,   NULL,   NULL,   0,
                -1,     NULL,   NULL,   NULL,   1,
                -1,     NULL,   NULL,   NULL,   2,
                -1,     NULL,   NULL,   NULL,   3,
                -1,     NULL,   NULL,   NULL,   4,
                -1,     NULL,   NULL,   NULL,   5,
                -1,     NULL,   NULL,   NULL,   6,
                -1,     NULL,   NULL,   NULL,   7,
                -1,     NULL,   NULL,   NULL,   8
              };    

static short wLevelNum = 0;

class CSoundOffParser : public CParser
{
public:
    CSoundOffParser(LPSTR lpTableData)
        : CParser(lpTableData) {}

protected:
    BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};  

//************************************************************************
CSoundOffScene::CSoundOffScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_iLevel = -1;
	soundOff = -1;
	fIgnore = FALSE;
	pWrongGuess = NULL;
	pCorrectGuess = NULL;
	wGuess = 0;
	wActiveStalls = 1;
	wChannel = START_RND_CHANNEL;
	m_hWnd = NULL;
	m_hWave = NULL;

	for (int i = 0 ; i < NUMBER_OF_ANIMALS ; i++)
	{
		Layout.Stalls[i].Animal = -1;
		Layout.Stalls[i].m_lpHeadSprite = NULL;
		Layout.Stalls[i].m_lpBadSprite = NULL;
		Layout.Stalls[i].m_lpBodySprite = NULL;
		Layout.Stalls[i].wGuess = i;
		HeadOfCattle[i].beastHead.len = -1;
		HeadOfCattle[i].beastHead.wid = -1;
	}
}

//************************************************************************
CSoundOffScene::~CSoundOffScene()
//************************************************************************
{
   	if (m_hWave != NULL)
		MCIClose (m_hWave);
}
                     
//************************************************************************
int CSoundOffScene::GetLevel()
//************************************************************************
{
	if (m_iLevel >= 0)
		return(m_iLevel);
	else
	if (m_nSceneNo == IDD_SOUNDOFFI)
		return(0);
	else
		return(m_nSceneNo - IDD_SOUNDOFF1);
}

//************************************************************************
BOOL CSoundOffScene::OnInitDialog (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
    CGBScene::OnInitDialog (hWnd, hWndControl, lParam);

	m_hWnd = hWnd;
    
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
			CSoundOffParser parser (lpTableData);
			short i = parser.GetNumEntries();
            
			parser.ParseTable ((DWORD)this);
		}
		if (hData)
		{
			UnlockResource (hData);
			FreeResource (hData);
		}
	}
	return (TRUE);
}

//************************************************************************
void CSoundOffScene::ToonInitDone()
//************************************************************************
{
	PlayIntro();
}

//************************************************************************
void CSoundOffScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	FNAME szFileName;
    
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_hWave != NULL)
			{
				MCIClose (m_hWave);
				m_hWave = NULL;
			}

			if (m_nSceneNo == IDD_SOUNDOFFI)
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			else
				GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
			break;
		}
		case IDC_HELP:
		{
			wState = INTRO_PLAY;    
			MCIPlaySound (GetPathName (szFileName, szIntroWave));
		}
		break;
                
		case IDC_LEVEL1:
		case IDC_LEVEL2:
		case IDC_LEVEL3:
		{
			int iLevel = id-IDC_LEVEL1;
			if (iLevel >= NUM_LEVELS)
				iLevel = NUM_LEVELS-1;
			if (iLevel != GetLevel())
			if (iLevel != GetLevel())
			{
				if (m_hWave != NULL)
				{
					MCIClose (m_hWave);
					m_hWave = NULL;
				}

				CSound sound;
				FNAME szFileName;
				GetPathName (szFileName, CLICK_WAVE);
				sound.StartFile (szFileName, FALSE, -1, TRUE);
				ChangeLevel (iLevel);
			}
			break;
		}
		default:
		{
			if (m_hWave != NULL)
			{
				MCIClose (m_hWave);
				m_hWave = NULL;
			}

			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//************************************************************************
BOOL CSoundOffScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
		HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
		HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);

		default:
			return(FALSE);
    }
}

//***************************************************************************
UINT CSoundOffScene::OnMCINotify (HWND hWindow, UINT codeNotify, HMCI hDevice)
//***************************************************************************
{
	switch (codeNotify)
	{
		case MCI_NOTIFY_SUCCESSFUL:
		{
			// Close last played wave
			if (m_hWave != NULL)
			{
				MCIClose (m_hWave);
				m_hWave = NULL;
			}
			// Let the timer function go to the next event ASAP
			PostMessage (hWindow, WM_TIMER, (WPARAM)SOUNDOFF_TIMER, (LPARAM)NULL);
			return (FALSE);
		}

		// Aborted msg sent during playback
		case MCI_NOTIFY_ABORTED:    // wParam == 4
		{
			return (FALSE);
		}

		// Superseded msg sent during playback if we get a new play command
		case MCI_NOTIFY_SUPERSEDED: // wParam == 2
		case MCI_NOTIFY_FAILURE:    // wParam == 8
		{
			return (FALSE);
		}
	}

	return (TRUE);
}

//************************************************************************
void CSoundOffScene::OnTimer (HWND hWnd, UINT id)
//************************************************************************
{
	FNAME szFileName;
	static short fFirst = TRUE;

	if (id == SOUNDOFF_TIMER)
	{
		if (wState == INTRO_PLAY)    
		{
			wState = PLAYING;   
			PlayOrder();
			GetToon()->SetHintState (TRUE);
			return;
		}

		if (wState == FINISH_PLAY)
		{
			// If we are in Play mode, go to the next scene
			if (GetApp()->m_fPlaying)
			{
				CGBScene::OnCommand (hWnd, IDC_NEXT, NULL, 0);
				return;
			}

			wState = PLAYING;   
			m_iLevel = -1;
			soundOff = -1;
			fIgnore = FALSE;
			pWrongGuess = NULL;
			pCorrectGuess = NULL;
			wGuess = 0;
			wActiveStalls = 1;

			SetPeckingOrder();
			PlayOrder();
		}
    
		if (pCorrectGuess)
		{
			if (wGuess == wActiveStalls)
			{
				if (wActiveStalls < wMatches)
				{
					if (fFirst == TRUE)
					{
						fFirst = FALSE;
						MCIPlaySound (GetPathName (szFileName, szSuccessWave));
					}
					else
					{
						fIgnore = FALSE;
						pCorrectGuess = NULL;
						fFirst = TRUE;
						++wActiveStalls;
						wGuess = 0;
						PlayOrder();
					}
				}
				else
				{
					wState = FINISH_PLAY;
					MCIPlaySound (GetPathName (szFileName, szExitWave));
				}
			}
			else
			{
				fIgnore = FALSE; 
				pCorrectGuess = NULL;
			}
		}
		else if (soundOff < (wActiveStalls-1) && soundOff != -1)
		{
			++soundOff;
			pPeckingOrder[soundOff]->m_lpHeadSprite->SetCellsPerSec (ANIMSPEED);
			pPeckingOrder[soundOff]->m_lpHeadSprite->Pause (0);
			pPeckingOrder[soundOff]->m_lpHeadSprite->AddCmdPauseAfterCell (0,PAUSE_FOREVER);

			MCIPlaySound (GetPathName (szFileName, HeadOfCattle[pPeckingOrder[soundOff]->Animal].szBeastSound));
		}
		else if (soundOff != -1)
		{
			soundOff = -1;
			fIgnore = FALSE;
		}
	}
	else
		CGBScene::OnTimer (hWnd, id);
}

//************************************************************************
void CSoundOffScene::OnLButtonDown (HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	int i;
	FNAME szFileName;

	if (wState == INTRO_PLAY)    
	{
		wState = PLAYING;   
		PlayOrder();
		GetToon()->SetHintState (TRUE);
		return;
	}
    
	if (fIgnore == FALSE)
	{
		if (x > rReplayButtonArea.left && x < rReplayButtonArea.right &&
			y > rReplayButtonArea.top && y < rReplayButtonArea.bottom)
		{
			PlayOrder();
			return;
		}
    
        for (i = 0 ; i < NUMBER_OF_ANIMALS ; i++)
        {
        	// If the user clicked on an animal, see if it was the correct one
        	if (IsOnAnimal (i, x, y))
			{
				if (i == pPeckingOrder[wGuess]->Animal)
				{
					fIgnore = TRUE;
					pCorrectGuess = GetSide (i);
                        
					pCorrectGuess->m_lpHeadSprite->SetCellsPerSec (ANIMSPEED);
					pCorrectGuess->m_lpHeadSprite->Pause (0);
					pCorrectGuess->m_lpHeadSprite->AddCmdPauseAfterCell (0, PAUSE_FOREVER);

					MCIPlaySound (GetPathName (szFileName, HeadOfCattle[pPeckingOrder[wGuess]->Animal].szBeastSound));
					++wGuess;                       
				}
				else
				{
					pWrongGuess = GetSide (i);
					pWrongGuess->m_lpBadSprite->Show (TRUE);
					pWrongGuess->m_lpHeadSprite->Show (FALSE);
					pWrongGuess->m_lpBadSprite->Pause (0);
					pWrongGuess->m_lpBadSprite->SetCellsPerSec (ANIMSPEED);
					pWrongGuess->m_lpBadSprite->AddCmdPauseAfterCell (0, PAUSE_FOREVER);
				}
				return;
			}
		}
	}   
}

//************************************************************************
void CSoundOffScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CSoundOffScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
LPSTR CSoundOffScene::GetPathName(LPSTR lpPathName, LPSTR lpFileName)
//************************************************************************
{
	GetToon()->GetContentDir (lpPathName);
	lstrcat (lpPathName, lpFileName);
	return (lpPathName);
}

//************************************************************************
void CSoundOffScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	int i;
	xystruct *pSide;

	wState = INTRO_PLAY;    
	GetToon()->SetHintState (FALSE);
    
	ptOrigin.x = 0;
	ptOrigin.y = 0; 

	m_pAnimator->SetClipRect (&rGameClipArea);
    
	SetPeckingOrder();

	pSide = &Layout.Stalls[0];

	for (i = 0 ; i < NUMBER_OF_ANIMALS ; i++)
	{
		pSide->Animal = i;

		pSide->m_lpHeadSprite = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName(szFileName, HeadOfCattle[pSide->Animal].beastHead.szHeadRight);
		pSide->m_lpHeadSprite->SetCellsPerSec (0);
		pSide->m_lpHeadSprite->AddCells (szFileName,
								HeadOfCattle[pSide->Animal].beastHead.LCells, NULL, NULL);
		pSide->m_lpHeadSprite->Jump (HeadOfCattle[pSide->Animal].beastHead.X,
								HeadOfCattle[pSide->Animal].beastHead.Y);
		pSide->m_lpHeadSprite->SetCycleBack (TRUE);
		pSide->m_lpHeadSprite->Pause (PAUSE_FOREVER);                                    
		pSide->m_lpHeadSprite->SetNotifyProc (OnSoundNotify, (DWORD)this);    
		pSide->m_lpHeadSprite->Show (TRUE);

		pSide->m_lpBadSprite = m_pAnimator->CreateSprite (&ptOrigin);
		GetPathName (szFileName, HeadOfCattle[pSide->Animal].beastHead.szHeadWrong);
		pSide->m_lpBadSprite->SetCellsPerSec (0);
		pSide->m_lpBadSprite->AddCells (szFileName, HeadOfCattle[pSide->Animal].beastHead.RCells, NULL, NULL);
		pSide->m_lpBadSprite->Jump (HeadOfCattle[pSide->Animal].beastHead.X,
								HeadOfCattle[pSide->Animal].beastHead.Y);
		pSide->m_lpBadSprite->SetCycleBack (TRUE);
		pSide->m_lpBadSprite->Pause (PAUSE_FOREVER);                                 
		pSide->m_lpBadSprite->SetNotifyProc (OnSoundNotify, (DWORD)this);    
		pSide->m_lpBadSprite->Show (FALSE);

		if (HeadOfCattle[pSide->Animal].szBeastBody[0] != (char) 0)
		{
			pSide->m_lpBodySprite = m_pAnimator->CreateSprite (&ptOrigin);
			GetPathName (szFileName, HeadOfCattle[pSide->Animal].szBeastBody);
			pSide->m_lpBodySprite->AddCell (szFileName, NULL);
			pSide->m_lpBodySprite->Jump (HeadOfCattle[pSide->Animal].bodyx,
									HeadOfCattle[pSide->Animal].bodyy);
			pSide->m_lpBodySprite->SetNotifyProc (OnSoundNotify, (DWORD)this);    
			pSide->m_lpBodySprite->Show (TRUE);
		}
		else
		pSide->m_lpBodySprite = NULL;
        
		++pSide;
	}

	// Start animations before starting the sound, or sound will chop (SMS)
	m_pAnimator->StartAll();
	MCIPlaySound (GetPathName (szFileName, szIntroWave));
}

//************************************************************************
void CSoundOffScene::ChangeLevel(int iLevel)
//************************************************************************
{
	wLevelNum = iLevel;
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	if (iLevel != GetLevel())
		GetApp()->GotoScene(m_hWnd, IDD_SOUNDOFF1+iLevel, m_nNextSceneNo);
}

//************************************************************************
void CSoundOffScene::SetPeckingOrder()
//************************************************************************
{
	DWORD dwRandom,j;
	int i, k;
	short wItemSelected;
	xystruct *pSetOfAnimals[50];  
	xystruct *pHoldSetOfAnimals[50]; 
	xystruct *pHoldAllAnimals[NUMBER_OF_ANIMALS];
	xystruct *pSet;

	for(i=0;i<MAX_MATCHES;i++)
		pPeckingOrder[i] = NULL;
    
	pSet = &Layout.Stalls[0];       
	for(i=0;i<NUMBER_OF_ANIMALS;i++)
	{
		pHoldAllAnimals[i] = pSet;  
		++pSet;
	}   

// RANDOMLY SELECT YOUR ANIMALS
	if(wAnims < NUMBER_OF_ANIMALS)
	{   
		for(k=0;k<wAnims;k++)
		{
			wItemSelected = -1;

			GetRandomNumber(100);       
			dwRandom = (DWORD)rand();
			while(dwRandom > 1000)
				dwRandom -= 1000;
			while(dwRandom > 50)
				dwRandom -= 50;

			i = 0;
			for(j=0;j<dwRandom;j++)
			{
				if(pHoldAllAnimals[i] != NULL)
					wItemSelected = i;      
				++i;
				if(i > (NUMBER_OF_ANIMALS - 1))
				i = 0;
			}
        
			while(wItemSelected == -1)
			{
				if(pHoldAllAnimals[i] == NULL)
					wItemSelected = i;      
				++i;
				if(i > (NUMBER_OF_ANIMALS - 1))
					i = 0;
			}
			pSetOfAnimals[k] = pHoldAllAnimals[wItemSelected]; 
			pHoldAllAnimals[wItemSelected] = NULL;
		}   
		for(i=0;i<wAnims;i++)   
			pHoldSetOfAnimals[i] = pSetOfAnimals[i]; 
	}
	else
	{
		for(i=0;i<wAnims;i++)   
			pHoldSetOfAnimals[i] = pSetOfAnimals[i] = pHoldAllAnimals[i];  
	}   


// SET PECKING ORDER
	for(k=0;k<wMatches;k++)
	{
		wItemSelected = -1;
		if(k == wAnims)     
		{
			for(i=0;i<wAnims;i++)
				pSetOfAnimals[i] = pHoldSetOfAnimals[i];    
		}
        
		GetRandomNumber(100);       
		dwRandom = (DWORD)rand();
		while(dwRandom > 1000)
			dwRandom -= 1000;
		while(dwRandom > 50)
			dwRandom -= 50;

		i = 0;
		for(j=0;j<dwRandom;j++)
		{
			if(pSetOfAnimals[i] != NULL)
				wItemSelected = i;      
			++i;
			if(i > (wAnims - 1))
				i = 0;
		}
        
		while(wItemSelected == -1)
		{
			if(pSetOfAnimals[i] != NULL)
				wItemSelected = i;      
			++i;
			if(i > (wAnims - 1))
				i = 0;
		}
		pPeckingOrder[k] = pSetOfAnimals[wItemSelected]; 
		pSetOfAnimals[wItemSelected] = NULL; 
	}
}

//************************************************************************
void CSoundOffScene::PlayOrder ()
//************************************************************************
{
	FNAME szFileName;

	soundOff = 0;
	fIgnore = TRUE;
    
	pPeckingOrder[soundOff]->m_lpHeadSprite->SetCellsPerSec( ANIMSPEED );
	pPeckingOrder[soundOff]->m_lpHeadSprite->Pause(0);
	pPeckingOrder[soundOff]->m_lpHeadSprite->AddCmdPauseAfterCell(0,PAUSE_FOREVER);

	MCIPlaySound (GetPathName (szFileName, HeadOfCattle[pPeckingOrder[soundOff]->Animal].szBeastSound));
}


//************************************************************************
xystruct *CSoundOffScene::GetSide(int wCount)
//************************************************************************
{
	xystruct *pRet;

	pRet = &Layout.Stalls[0];
	for(wCount=wCount;wCount > 0;wCount--)
	{
		++pRet;
	}
	return(pRet);
}


//************************************************************************
LOCAL void CALLBACK OnSoundNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPSOUNDOFFSCENE lpScene = (LPSOUNDOFFSCENE)dwNotifyData;
		lpScene->OnSNotify(lpSprite, Notify);
	}
}


//************************************************************************
void CSoundOffScene::OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (Notify == SN_PAUSEAFTER && pWrongGuess != NULL)
	{
		pWrongGuess->m_lpHeadSprite->Show(TRUE);
		pWrongGuess->m_lpBadSprite->Show(FALSE);
		pWrongGuess = NULL;
		wActiveStalls = 1;
		wGuess = 0;
        
		// Kill any currently playing wave
		if (m_hWave)
		{
			MCIClose (m_hWave);
			m_hWave = NULL;
		}

		FNAME szFileName;
		GetPathName (szFileName, szFailWave);
		if (m_hWave = MCIOpen (GetApp()->m_hDeviceWAV, szFileName, NULL/*Alias*/))
		{
			MCIPlay (m_hWave);
			MCIClose (m_hWave);
			m_hWave = NULL;
		}

		if(boWrongReset)
			SetPeckingOrder();
                    
		PlayOrder();
	}
}

//************************************************************************
BOOL CSoundOffScene::MCIPlaySound (LPSTR lpWaveFile)
//************************************************************************
{
	// Kill any currently playing wave
	if (m_hWave != NULL)
	{
		MCIClose (m_hWave);
		m_hWave = NULL;
	}

	if (m_hWave = MCIOpen (GetApp()->m_hDeviceWAV, lpWaveFile, NULL/*Alias*/))
	{
		if (! m_hWave)
			return FALSE;

		if (MCIPlay (m_hWave, m_hWnd))
			return TRUE;
	}
	return FALSE;
}


#ifdef UNUSED
//************************************************************************
BOOL CSoundOffScene::PlaySound (LPCTSTR lpWaveFile, BOOL fHint)
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
#endif

#ifdef UNUSED
//************************************************************************
BOOL CSoundOffScene::PlayHint(LPCTSTR lpHintWaveFile)
//************************************************************************
{
	if (m_pSound)
	{
		m_pSound->StopChannel(HINT_CHANNEL);
		m_pSound->StartFile((LPSTR)lpHintWaveFile, FALSE, HINT_CHANNEL, FALSE);
		return(TRUE);
	}
	return(FALSE);
}
#endif


//************************************************************************
BOOL CSoundOffScene::IsOnAnimal (int i, int x, int y)
//************************************************************************
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	xystruct *pSide;        
	pSide = &Layout.Stalls[i];
            
	if (pSide->m_lpBodySprite)
	{       
		if (pSide->m_lpBodySprite->MouseInSprite (&pt, TRUE))
			return (TRUE);
	}

	if (pSide->m_lpHeadSprite)
	{       
		if (pSide->m_lpHeadSprite->MouseInSprite (&pt, TRUE))
			return (TRUE);
	}

	return (FALSE);
}

//************************************************************************
BOOL CSoundOffScene::IsOnHotSpot(LPPOINT lpPoint)
//************************************************************************
{
	short i;
	xystruct *pSide;        
        
	if (PtInRect (&rReplayButtonArea, *lpPoint))	// Replay Button
		return (TRUE);

	pSide = &Layout.Stalls[0];
            
	for (i = 0 ; i < NUMBER_OF_ANIMALS ; i++)       // Animal Rectangles
	{
		#ifdef NEED_SMALLER_RECTANGLES
		if (lpPoint->x > HeadOfCattle[i].x && lpPoint->x < (HeadOfCattle[i].x + HeadOfCattle[i].len)) 
		{
			if (lpPoint->y > HeadOfCattle[i].y && lpPoint->y < (HeadOfCattle[i].y + HeadOfCattle[i].wid))
			{
				return (TRUE);
			}
		}
		#endif

		if (pSide->m_lpBodySprite != NULL)
		{       
			if (pSide->m_lpBodySprite->MouseInSprite (lpPoint, TRUE))
				return (TRUE);
		}

		if (pSide->m_lpHeadSprite != NULL)
		{       
			if (pSide->m_lpHeadSprite->MouseInSprite (lpPoint, TRUE))
				return (TRUE);
		}

		++pSide;
	}

	return (FALSE);
}


//************************************************************************
BOOL CSoundOffParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPSOUNDOFFSCENE lpScene = (LPSOUNDOFFSCENE)dwUserData;
	static BOOL boBody;

	if (!lstrcmpi(lpKey, "flagwrongreset"))
		GetInt(&lpScene->boWrongReset, lpEntry, lpValues, nValues);
    
	if (!lstrcmpi(lpKey, "matches"))
		GetInt(&lpScene->wMatches, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "animals"))
		GetInt(&lpScene->wAnims, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "animspeed"))
		GetInt(&lpScene->ANIMSPEED, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "introwav"))
		GetFilename(lpScene->szIntroWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "failwav"))
		GetFilename(lpScene->szFailWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "exitwav"))
		GetFilename(lpScene->szExitWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "successwav"))
		GetFilename(lpScene->szSuccessWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "wave"))
	{
		boBody = FALSE;
		GetFilename(lpScene->HeadOfCattle[nIndex].szBeastSound, lpEntry, lpValues, nValues);
	}

	if (!lstrcmpi(lpKey, "thebody"))        
	{
		boBody = TRUE;
		GetFilename(lpScene->HeadOfCattle[nIndex].szBeastBody, lpEntry, lpValues, nValues);
	}
    
	if (!lstrcmpi(lpKey, "bodyx"))
		GetInt(&lpScene->HeadOfCattle[nIndex].bodyx, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "bodyy"))
		GetInt(&lpScene->HeadOfCattle[nIndex].bodyy, lpEntry, lpValues, nValues);
    
	if (!lstrcmpi(lpKey, "correcthead"))
		GetFilename(lpScene->HeadOfCattle[nIndex].beastHead.szHeadRight, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "ccells"))
		GetInt(&lpScene->HeadOfCattle[nIndex].beastHead.LCells, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "x"))
		GetInt(&lpScene->HeadOfCattle[nIndex].beastHead.X, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "y"))
		GetInt(&lpScene->HeadOfCattle[nIndex].beastHead.Y, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "incorrecthead"))
		GetFilename(lpScene->HeadOfCattle[nIndex].beastHead.szHeadWrong, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "wcells"))
		GetInt(&lpScene->HeadOfCattle[nIndex].beastHead.RCells, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "mousex"))
		GetInt(&lpScene->HeadOfCattle[nIndex].x, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "mousey"))
		GetInt(&lpScene->HeadOfCattle[nIndex].y, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "mouselen"))
		GetInt(&lpScene->HeadOfCattle[nIndex].len, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "mousewid"))
	{
		GetInt(&lpScene->HeadOfCattle[nIndex].wid, lpEntry, lpValues, nValues);
		if(boBody == FALSE)
		{
			lpScene->HeadOfCattle[nIndex].szBeastBody[0] = (char) 0;
		}
	}

	if (!lstrcmpi(lpKey, "headlen"))
		GetInt(&lpScene->HeadOfCattle[nIndex].beastHead.len, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "headwid"))
		GetInt(&lpScene->HeadOfCattle[nIndex].beastHead.wid, lpEntry, lpValues, nValues);

	return(TRUE);
}

