#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "shelter.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "sailorid.h"

#define NUM_LEVELS	3
#define DRAG_DISTANCE 20

class CShelterParser : public CParser
{
public:
	CShelterParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetMultipleFiles(FILE_SPEC FAR *lpFiles, int nMaxFiles, LPINT lpNumFiles,
									LPSTR lpEntry, LPSTR lpValues, int nValues);
};  

LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

//************************************************************************
CShelterScene::CShelterScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_iLevel = -1;
	m_pSelectedPiece = NULL;
	m_pSound = NULL;
	m_fCapture = FALSE;
	m_ptAnchor.x = m_ptAnchor.y = 0;
	m_ptStart.x = m_ptStart.y = 0;
	m_iSpeed = 300;
	m_iCloseness = 5;
	m_pMovingPiece = NULL;
	m_hHotCursor = LoadCursor(GetApp()->GetInstance(), MAKEINTRESOURCE(ID_POINTER));
	SetRectEmpty(&m_rGameArea);
	m_fDrag = FALSE;
	m_SoundPlaying = NotPlaying;
	m_szSoundTrack[0] = '\0';
	m_szHammer[0] = '\0';
	m_szPickup[0] = '\0';
	m_nShelters = 0;
	m_pShelters = NULL;
	m_pCurrent = NULL;
	m_nSuccess = 0;
	for (int j = 0; j < MAX_SUCCESS; ++j)
	{
		m_szSuccess[j][0] = '\0';
		m_fSuccessPlayed[j] = FALSE;
	}
}

//************************************************************************
CShelterScene::~CShelterScene()
//************************************************************************
{
	if (m_pShelters)
		delete [] m_pShelters;
	if (m_pSound)
		delete m_pSound;
}

//************************************************************************
int CShelterScene::GetLevel()
//************************************************************************
{
	if (m_iLevel >= 0)
		return(m_iLevel);
	else
	if (m_nSceneNo == IDD_HOUSEI)
		return(0);
	else
		return(m_nSceneNo - IDD_HOUSE1);
}

//************************************************************************
BOOL CShelterScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CGBScene::OnInitDialog(hWnd, hWndControl, lParam);

	if (m_pAnimator)
	{
		HGLOBAL hData;
		LPSTR lpTableData;

		// read in the house piece information
		lpTableData = GetResourceData(m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CShelterParser parser(lpTableData);
			m_pShelters = new CShelter[MAX_SHELTERS];
			if (m_pShelters)
					parser.ParseTable((DWORD)this);
		}

		if (hData)
		{
			UnlockResource( hData );
			FreeResource( hData );
		}
		m_pCurrent = GimmeShelter();
		if (GetToon())
			GetToon()->SetBackground(m_pCurrent->m_szBackground);
	}

	return( TRUE );
}

//************************************************************************
void CShelterScene::ToonInitDone()
//************************************************************************
{
	if (m_pAnimator)
	{
		if (GetToon())
			GetToon()->SetSendMouseMode(TRUE);

		m_pAnimator->StopAll();

		if (!IsRectEmpty(&m_rGameArea))
			m_pAnimator->SetClipRect(&m_rGameArea);

		LoadShelter();

		m_pAnimator->StartAll();
		m_pSound = new CSound(TRUE);
		if (m_pSound)
			m_pSound->Open( NULL/*"powersnd.dll"*/ );
		PlayIntro();
	}
}

//************************************************************************
void CShelterScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_ACTIVITIES:
		{
			if (m_nSceneNo == IDD_HOUSEI)
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
BOOL CShelterScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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
void CShelterScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	POINT ptMouse;

	if (!m_pAnimator)
		return;

	if (m_SoundPlaying == IntroPlaying)
	{
		m_pSound->StopChannel(INTRO_CHANNEL);
		m_SoundPlaying = NotPlaying;
		PlaySoundTrack();
	}

	ptMouse.x = x; ptMouse.y = y;
	if (m_pSelectedPiece)
		HandleDrop();
	else
	{
		m_pSelectedPiece = FindPiece(&ptMouse);
		if (m_pSelectedPiece && m_pSelectedPiece->m_lpSprite)
		{
			PlaySoundEffect(m_szPickup);
			m_pAnimator->Unlink(m_pSelectedPiece->m_lpSprite);
			m_pAnimator->AddSprite(m_pSelectedPiece->m_lpSprite, TRUE);
			if (m_pSelectedPiece->m_lpHintSprite)
			 	m_pSelectedPiece->m_lpHintSprite->Show(TRUE);
			if (m_pSelectedPiece->m_lpStartSprite)
			{
				m_pSelectedPiece->m_lpStartSprite->Show(FALSE);
				m_pSelectedPiece->m_lpSprite->Show(TRUE);
			}
			m_ptAnchor = ptMouse;
			m_ptStart = ptMouse;
			SetCapture(hWnd);
			m_fCapture = TRUE;
			m_fDrag = FALSE;
		}
	}
}

//************************************************************************
void CShelterScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (!m_fCapture)
		return;
	ReleaseCapture();
	m_fCapture = FALSE;
	if (m_pSelectedPiece)
		HandleDrop(m_fDrag);
}

//************************************************************************
void CShelterScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	RECT rSprite;
	POINT pt;

	if (!m_pSelectedPiece)
		return;

	pt.x = x; pt.y = y;
	if (PtInRect(&m_rGameArea, pt))
	{
		int dx, dy;

		if (!m_fDrag && m_fCapture)
		{
			dx = abs(x - m_ptStart.x);
			dy = abs(y - m_ptStart.y);
			m_fDrag = (dx > DRAG_DISTANCE) || (dy > DRAG_DISTANCE);
		}
		dx = x - m_ptAnchor.x;
		dy = y - m_ptAnchor.y;
		Debug("dx = %d dy = %d x = %d y = %d\n",
							dx, dy, x, y);
		m_ptAnchor.x = x;
		m_ptAnchor.y = y;
		m_pSelectedPiece->m_lpSprite->Location(&rSprite);
		OffsetRect(&rSprite, dx, dy);
		Debug("Jumping to %d %d\n", rSprite.left, rSprite.top);
		Debug("dx = %d dy = %d x = %d y = %d\n",
							dx, dy, x, y);

		m_pSelectedPiece->m_lpSprite->Jump(rSprite.left, rSprite.top);
	}
	if (m_fCapture)
	{
		if (InPosition())
			SetCursor(m_hHotCursor);
		else
		{
			HCURSOR hCursor = (HCURSOR)GetClassWord(GetToon()->GetWindow(), GCW_HCURSOR);
			//if (!hCursor)
			//	hCursor = LoadCursor(NULL, IDC_ARROW);
			SetCursor(hCursor);
		}
	}
}

//************************************************************************
void CShelterScene::OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr)
//************************************************************************
{
	SoundPlaying WasPlaying = m_SoundPlaying;
	m_SoundPlaying = NotPlaying;
	if (WasPlaying == IntroPlaying)
	{
		// play background soundtrack
		PlaySoundTrack();
	}
	else
	if (WasPlaying == SuccessPlaying)
	{
		LPSHELTER pShelter = GimmeShelter();
		if (!pShelter && m_nSceneNo != IDD_HOUSEI)
		{
			ResetLevel();
			pShelter = GimmeShelter();
		}
		if (pShelter)
		{
			UnloadShelter();
			m_pCurrent = pShelter;
			GetToon()->LoadBackground( m_pCurrent->m_szBackground );
			LoadShelter();
			PlaySoundTrack();
		}
		else
		{
			// goto next scene
			if (m_nSceneNo == IDD_HOUSEI)
				FORWARD_WM_COMMAND(m_hWnd, IDC_GOTOSCENE, NULL, m_nNextSceneNo, PostMessage);
		}
	}
}

//************************************************************************
void CShelterScene::HandleDrop(BOOL fRealDrop)
//************************************************************************
{
	if (InPosition())
	{
		if (m_pSelectedPiece->m_lpHintSprite)
		 	m_pSelectedPiece->m_lpHintSprite->Show(FALSE);
		// play hammer sound
		PlaySoundEffect(m_szHammer);

		m_pSelectedPiece->m_lpSprite->Jump(m_pSelectedPiece->m_ptEnd.x,
									m_pSelectedPiece->m_ptEnd.y);
		m_pSelectedPiece->m_fInPosition = TRUE;
		m_pSelectedPiece = NULL;
		// here we check for success and then do something
		if (IsSolved())
		{
			// play scuppers thank you
			m_pCurrent->m_fCompleted = TRUE;
			if (GimmeShelter()) // any more left?
				PlaySuccess();
			else
				PlayFinale();
		}
	}
	else
	if (fRealDrop)
	{
		if (m_pSelectedPiece->m_lpHintSprite)
		 	m_pSelectedPiece->m_lpHintSprite->Show(FALSE);
		m_pSelectedPiece->m_lpSprite->SetSpeed(m_iSpeed);
		m_pSelectedPiece->m_lpSprite->Move(m_pSelectedPiece->m_ptStart.x,
							m_pSelectedPiece->m_ptStart.y);
		m_pMovingPiece = m_pSelectedPiece;
		m_pSelectedPiece = NULL;
	}
}

#ifdef OLD_STUFF
//************************************************************************
void CShelterScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	POINT ptMouse;

	if (!m_pAnimator)
		return;

	if (m_hSound)
	{
		MCIStop(m_hSound, TRUE);
		MCIClose(m_hSound);
		m_hSound = NULL;
	}

	ptMouse.x = x; ptMouse.y = y;
	if (m_pSelectedPiece)
	{
	}
	else
	{
		m_pSelectedPiece = FindPiece(&ptMouse);
		if (m_pSelectedPiece)
		{
			if (m_pSelectedPiece->m_lpHintSprite)
			 	m_pSelectedPiece->m_lpHintSprite->Show(TRUE);
			if (m_pSelectedPiece->m_lpStartSprite)
			{
				m_pSelectedPiece->m_lpStartSprite->Show(FALSE);
				m_pSelectedPiece->m_lpSprite->Show(TRUE);
			}
			SetCapture(hWnd);
			if (!IsRectEmpty(&m_rGameArea))
			{
				RECT rClip;

				GetClipCursor(&m_rOldClip);

				rClip = m_rGameArea;

				ClientToScreen(hWnd, (LPPOINT)&rClip.left);
				ClientToScreen(hWnd, (LPPOINT)&rClip.right);
				ClipCursor(&rClip);
			}
			m_fCapture = TRUE;
			m_ptAnchor = ptMouse;
		}
	}
}

//************************************************************************
void CShelterScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	if (m_fCapture)
	{
		ReleaseCapture();
		ClipCursor(&m_rOldClip);
		m_fCapture = FALSE;
		if (m_pSelectedPiece)
		{   
			//m_pSelectedPiece->m_lpSprite->Show(FALSE);
			if (m_pSelectedPiece->m_lpHintSprite)
			 	m_pSelectedPiece->m_lpHintSprite->Show(FALSE);
			//m_pSelectedPiece->m_lpSprite->Show(TRUE);
			if (InPosition())
			{
				// play hammer sound

				m_pSelectedPiece->m_lpSprite->Jump(m_pSelectedPiece->m_ptEnd.x,
											m_pSelectedPiece->m_ptEnd.y);
				m_pSelectedPiece->m_fInPosition = TRUE;
				// here we check for success and then do something
				if (IsSolved())
				{
					// play scuppers thank you

					// goto next scene
					if (m_nSceneNo == IDD_HOUSEI)
						FORWARD_WM_COMMAND(m_hWnd, IDC_GOTOSCENE, NULL, m_nNextSceneNo, PostMessage);
					else
					if (GetLevel() < (NUM_LEVELS-1))
						FORWARD_WM_COMMAND(m_hWnd, IDC_LEVEL1+GetLevel()+1, NULL, 0, PostMessage);
				}
			}
			else
			{
		   		m_pSelectedPiece->m_lpSprite->SetSpeed(m_iSpeed);
				m_pSelectedPiece->m_lpSprite->Move(m_pSelectedPiece->m_ptStart.x,
									m_pSelectedPiece->m_ptStart.y);
				m_pMovingPiece = m_pSelectedPiece;
			}
			m_pSelectedPiece = NULL;
		}
	}
}

//************************************************************************
void CShelterScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
	RECT rSprite;

	if (!m_fCapture)
		return;
	if (!m_pSelectedPiece)
		return;

	int dx = x - m_ptAnchor.x;
	int dy = y - m_ptAnchor.y;
	Debug("dx = %d dy = %d x = %d y = %d\n",
						dx, dy, x, y);
	m_ptAnchor.x = x;
	m_ptAnchor.y = y;
	m_pSelectedPiece->m_lpSprite->Location(&rSprite);
	OffsetRect(&rSprite, dx, dy);
	Debug("Jumping to %d %d\n", rSprite.left, rSprite.top);
	Debug("dx = %d dy = %d x = %d y = %d\n",
						dx, dy, x, y);

	m_pSelectedPiece->m_lpSprite->Jump(rSprite.left, rSprite.top);
	if (InPosition())
		SetCursor(m_hHotCursor);
	else
	{
		HCURSOR hCursor = (HCURSOR)GetClassWord(GetToon()->GetWindow(), GCW_HCURSOR);
		//if (!hCursor)
		//	hCursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(hCursor);
	}
}
#endif

//************************************************************************
BOOL CShelterScene::IsOnHotSpot(LPPOINT lpPoint)
//************************************************************************
{
	if (m_pSelectedPiece)
		return(InPosition());
	else
		return(FindPiece(lpPoint) != NULL);
}

//************************************************************************
BOOL CShelterScene::InPosition()
//************************************************************************
{
	int dx, dy;
	RECT rSprite;

	if (!m_pSelectedPiece)
		return(FALSE);

	m_pSelectedPiece->m_lpSprite->Location(&rSprite);
	dx = rSprite.left - m_pSelectedPiece->m_ptEnd.x;
	dy = rSprite.top - m_pSelectedPiece->m_ptEnd.y;
	if (abs(dx) > m_iCloseness || abs(dy) > m_iCloseness)
		return(FALSE);
	//if (GetLevel() == 0)
	//	return(TRUE);
	//for (i = 0; i < m_nPieces; ++i)
	//{
	//	if (&m_pPieces[i] == m_pSelectedPiece)
	//		return(TRUE);
	//	if (!m_pPieces[i].m_fInPosition)
	//		break;
	//}
	//return(FALSE);
	return(TRUE);
}

//************************************************************************
void CShelterScene::ChangeLevel(int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	if (iLevel != GetLevel())
		App.GotoScene(m_hWnd, IDD_HOUSE1+iLevel, m_nNextSceneNo);
}

//************************************************************************
void CShelterScene::ResetLevel()
//************************************************************************
{
	for (int i = 0; i < m_nShelters; ++i)
	{
		m_pShelters[i].m_fCompleted = FALSE;
		for (int j = 0; j < m_pShelters[i].m_nPieces; ++j)
			m_pShelters[i].m_pPieces[j].m_fInPosition = FALSE;
	}

}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPSHELTERSCENE lpScene = (LPSHELTERSCENE)dwNotifyData;
		lpScene->OnNotify(lpSprite, Notify);
	}
}

//************************************************************************
void CShelterScene::OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	if (Notify == SN_MOVEDONE)
	{
		if (m_pMovingPiece)
		{
			if (m_pMovingPiece->m_lpStartSprite)
			{
				m_pMovingPiece->m_lpSprite->Show(FALSE);
				m_pMovingPiece->m_lpStartSprite->Show(TRUE);
			}
			m_pMovingPiece = NULL;
		}
		// play hammer sound

		//if (m_pSelectedPiece)
		//{
		//	m_pSelectedPiece->m_fInPosition = TRUE;
		//	m_pSelectedPiece = NULL;
		//	// here we check for success and then do something
		//	if (IsSolved())
		//	{
		//		// play scuppers thank you
		//
		//		// goto next scene
		//		if (m_nSceneNo == IDD_HOUSEI)
		//			FORWARD_WM_COMMAND(m_hWnd, IDC_GOTOSCENE, NULL, m_nNextSceneNo, PostMessage);
		//		else
		//		if (GetLevel() < (NUM_LEVELS-1))
		//			FORWARD_WM_COMMAND(m_hWnd, IDC_LEVEL1+GetLevel()+1, NULL, 0, PostMessage);
		//	}
		//}
	}
}

//************************************************************************
LOCAL LPSTR GetRandomFile(FILE_SPEC FAR *lpFileName, int nFiles, BOOL FAR *fFileUsed, LPSTR lpReturnFile)
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
	
	iFile = GetRandomNumber(iCount);
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
void CShelterScene::PlayIntro()
//************************************************************************
{
	FNAME szFileName;

	if (!m_pSound)
		return;
	GetToon()->SetHintState(FALSE);
	m_pSound->StopChannel(INTRO_CHANNEL);
	GetPathName(szFileName, m_szIntro);
	m_SoundPlaying = IntroPlaying;
	m_pSound->StartFile(szFileName, FALSE, INTRO_CHANNEL, FALSE, m_hWnd);
}

//************************************************************************
void CShelterScene::PlaySuccess()
//************************************************************************
{
	FNAME szFileName;

	if (!m_pSound)
		return;
	m_pSound->StopChannel(SUCCESS_CHANNEL);
	GetPathName(szFileName, m_pCurrent->m_szSuccess);
	m_SoundPlaying = SuccessPlaying;
	m_pSound->StartFile(szFileName, FALSE, SUCCESS_CHANNEL, FALSE, m_hWnd);
}

//************************************************************************
void CShelterScene::PlayFinale()
//************************************************************************
{
	FNAME szFileName;
	FILE_SPEC szSuccess;

	if (!m_pSound)
		return;

	m_pSound->StopChannel(SUCCESS_CHANNEL);
	GetRandomFile(m_szSuccess, m_nSuccess, m_fSuccessPlayed, szSuccess);
	GetPathName(szFileName, szSuccess);
	m_SoundPlaying = SuccessPlaying;
	m_pSound->StartFile(szFileName, FALSE, SUCCESS_CHANNEL, FALSE, m_hWnd);
}

//************************************************************************
void CShelterScene::PlaySoundTrack()
//************************************************************************
{
	FNAME szFileName;

	if (!m_pSound)
		return;
	GetToon()->SetHintState(TRUE);
	if (lstrlen(m_szSoundTrack))
	{
		m_pSound->StopChannel(SOUNDTRACK_CHANNEL);
		GetPathName(szFileName, m_szSoundTrack);
		m_pSound->StartFile(szFileName, TRUE, SOUNDTRACK_CHANNEL, FALSE);
	}
}

//************************************************************************
void CShelterScene::PlaySoundEffect(LPSTR lpEffect)
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
BOOL CShelterScene::PlaySound(LPCTSTR lpWaveFile, BOOL fHint)
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
LPSHELTER CShelterScene::GimmeShelter()
//************************************************************************
{
	int iRand;
	int iCount = 0;
	for (int i = 0; i < m_nShelters; ++i)
	{
		if (!m_pShelters[i].m_fCompleted)
			++iCount;
	}
	if (!iCount)
		return(NULL);
	iRand = GetCycleNumber(iCount);
	iCount = 0;
	for (i = 0; i < m_nShelters; ++i)
	{
		if (!m_pShelters[i].m_fCompleted)
		{
			if (iCount == iRand)
				break;
			++iCount;
		}
	}
	return(&m_pShelters[i]);
}

//************************************************************************
BOOL CShelterScene::LoadShelter()
//************************************************************************
{
	POINT ptOrigin;
	LPSPRITE lpSprite;
	LPSHELTERPIECE pPieces;
	FNAME szFileName;

	if (!m_pCurrent)
		return(FALSE);
	ptOrigin.x = 0;
	ptOrigin.y = 0;
	pPieces = &m_pCurrent->m_pPieces[0];
	for (int i = m_pCurrent->m_nPieces-1; i >= 0; --i)
	{

		// load the start position sprite if necessary
		if (lstrlen(pPieces[i].m_szStartName))
		{
			lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
			GetPathName(szFileName, pPieces[i].m_szStartName);
			lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
			lpSprite->Jump(pPieces[i].m_ptStart.x, pPieces[i].m_ptStart.y);
			lpSprite->Show(TRUE);
			pPieces[i].m_lpStartSprite = lpSprite;
		}
		// load the sprite for the piece
		lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
		GetPathName(szFileName, pPieces[i].m_szFileName);
		lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
		lpSprite->Jump(pPieces[i].m_ptStart.x, pPieces[i].m_ptStart.y);
		if (!pPieces[i].m_lpStartSprite)
			lpSprite->Show(TRUE);
		lpSprite->SetNotifyProc(OnSpriteNotify, (DWORD)this);
		pPieces[i].m_lpSprite = lpSprite;

		// load the sprite for the hint if necessary
		if (lstrlen(pPieces[i].m_szHintName))
		{
			lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
			GetPathName(szFileName, pPieces[i].m_szHintName);
			lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE );
			lpSprite->Jump(pPieces[i].m_ptEnd.x, pPieces[i].m_ptEnd.y);
			pPieces[i].m_lpHintSprite = lpSprite;
		}
	}
	return(TRUE);
}

//************************************************************************
BOOL CShelterScene::UnloadShelter()
//************************************************************************
{
	LPSHELTERPIECE pPieces;

	if (!m_pCurrent)
		return(FALSE);
	pPieces = &m_pCurrent->m_pPieces[0];
	for (int i = 0; i < m_pCurrent->m_nPieces; ++i)
	{

		if (pPieces[i].m_lpStartSprite)
		{
			m_pAnimator->DeleteSprite(pPieces[i].m_lpStartSprite);
			pPieces[i].m_lpStartSprite = NULL;
		}
		if (pPieces[i].m_lpSprite)
		{
			m_pAnimator->DeleteSprite(pPieces[i].m_lpSprite);
			pPieces[i].m_lpSprite = NULL;
		}
		if (pPieces[i].m_lpHintSprite)
		{
			m_pAnimator->DeleteSprite(pPieces[i].m_lpHintSprite);
			pPieces[i].m_lpHintSprite = NULL;
		}
	}
	return(TRUE);
}

//************************************************************************
LPSHELTERPIECE CShelterScene::FindPiece(LPPOINT lpPoint)
//************************************************************************
{
	if (!m_pCurrent)
		return(NULL);

	for (int i = 0; i < m_pCurrent->m_nPieces; ++i)
	{
		LPSPRITE lpSprite;

		// don't ever look at the selected piece
		if (m_pSelectedPiece == &m_pCurrent->m_pPieces[i])
			continue;

		lpSprite = m_pCurrent->m_pPieces[i].m_lpStartSprite;
		if (!lpSprite)
			lpSprite = m_pCurrent->m_pPieces[i].m_lpSprite;
		if (lpSprite && lpSprite->MouseInSprite(lpPoint, TRUE))
		{
			if (!m_pCurrent->m_pPieces[i].m_fInPosition)
				return(&m_pCurrent->m_pPieces[i]);
		}
	}
	return(NULL);
}

//************************************************************************
BOOL CShelterScene::IsSolved()
//************************************************************************
{
	int i;

	if (!m_pCurrent)
		return(FALSE);
	for (i = 0; i < m_pCurrent->m_nPieces; ++i)
	{
		if (!m_pCurrent->m_pPieces[i].m_fInPosition)
			return(FALSE);
	}
	return(TRUE);
}

//************************************************************************
CShelter::CShelter()
//************************************************************************
{
	m_szBackground[0] = '\0';
	m_nPieces = 0;
	m_fCompleted = FALSE;
}

//************************************************************************
CShelter::~CShelter()
//************************************************************************
{
}

//************************************************************************
CShelterPiece::CShelterPiece()
//************************************************************************
{
	m_szFileName[0] = '\0';
	m_szHintName[0] = '\0';
	m_szStartName[0] = '\0';
	m_ptStart.x = m_ptStart.y = 0;
	m_ptEnd.x = m_ptEnd.y = 0;
	m_ptFill.x = m_ptFill.y = 0;
	m_lpSprite = NULL;
	m_lpHintSprite = NULL;
	m_lpStartSprite = NULL;
	m_fInPosition = FALSE;
}

//************************************************************************
CShelterPiece::~CShelterPiece()
//************************************************************************
{
}

//************************************************************************
BOOL CShelterParser::GetMultipleFiles(FILE_SPEC FAR *lpFiles, int nMaxFiles, LPINT lpNumFiles,
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
BOOL CShelterParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPSHELTERSCENE lpScene = (LPSHELTERSCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "intro"))
			GetFilename(lpScene->m_szIntro, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "speed"))
			GetInt(&lpScene->m_iSpeed, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "closeness"))
			GetInt(&lpScene->m_iCloseness, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "gamearea"))
			GetRect(&lpScene->m_rGameArea, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetFilename(lpScene->m_szSoundTrack, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hammer"))
			GetFilename(lpScene->m_szHammer, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "pickup"))
			GetFilename(lpScene->m_szPickup, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success"))
			GetMultipleFiles(lpScene->m_szSuccess, MAX_SUCCESS, &lpScene->m_nSuccess, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		// bg marks beginning of new house
		if (!lstrcmpi(lpKey, "bg"))
			++lpScene->m_nShelters;

		if (!lpScene->m_nShelters)
			return(TRUE);

   		LPSHELTER pShelter = lpScene->GetShelter(lpScene->m_nShelters-1);
		LPSHELTERPIECE pShelterPiece = &pShelter->m_pPieces[pShelter->m_nPieces];

		if (!lstrcmpi(lpKey, "bg"))
			GetFilename(pShelter->m_szBackground, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "success"))
			GetFilename(pShelter->m_szSuccess, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "startpos"))
			GetPoint(&pShelterPiece->m_ptStart, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "endpos"))
		{
			GetPoint(&pShelterPiece->m_ptEnd, lpEntry, lpValues, nValues);
			++pShelter->m_nPieces;
		}
		else
		if (!lstrcmpi(lpKey, "fillpos"))
			GetPoint(&pShelterPiece->m_ptFill, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "piece"))
			GetFilename(pShelterPiece->m_szFileName, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "hint"))
			GetFilename(pShelterPiece->m_szHintName, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "startpiece"))
			GetFilename(pShelterPiece->m_szStartName, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

