#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "colorbok.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "colordef.h"

class CColorbookParser : public CParser
{
public:
	CColorbookParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};  

//************************************************************************
CColorbookScene::CColorbookScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_nPages = 0;
	m_pPages = NULL;
	m_nPanels = 0;
	m_nCurrentPanel = 0;
	m_iHighlighted = 0;
	m_lpHighlightSprite = NULL;
}

//************************************************************************
CColorbookScene::~CColorbookScene()
//************************************************************************
{
	if (m_pPages)
		delete [] m_pPages;
}

//************************************************************************
BOOL CColorbookScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CGBScene::OnInitDialog(hWnd, hWndControl, lParam);
	return( TRUE );
}

//************************************************************************
void CColorbookScene::ToonInitDone()
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
			CColorbookParser parser(lpTableData);
			if (m_nPages = parser.GetNumEntries())
			{
				// first entry is for global settings
				m_nPages -= 1;
				m_pPages = new CPage[m_nPages];
				if (m_pPages)
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
		// load the highlight bitmap
		if (lstrlen(m_szHighlight))
		{
			lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
			GetPathName(szFileName, m_szHighlight);
			if (lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE ))
				m_lpHighlightSprite = lpSprite;
			else
				m_pAnimator->DeleteSprite(lpSprite);
		}

		// determine number of panels
		for (i = 0; i < m_nPages; ++i)
		{
			if (m_pPages[i].m_nPanel > m_nPanels)
				m_nPanels = m_pPages[i].m_nPanel;
		}
		++m_nPanels; // cause 0 relative

		for (i = 0; i < m_nPages; ++i)
		{
			// load the sprite for thumbnail
			lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
			GetPathName(szFileName, m_pPages[i].m_szThumb);
			if (lpSprite->AddCell( szFileName, NULL, 0, 0, TRUE ))
			{
				lpSprite->Jump(m_pPages[i].m_ptPos.x, m_pPages[i].m_ptPos.y);
				m_pPages[i].m_lpSprite = lpSprite;
			}
			else
				m_pAnimator->DeleteSprite(lpSprite);
		}
		ShowCurrentPanel();
		HighlightPage(m_iHighlighted, TRUE);
	}
}

//************************************************************************
void CColorbookScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_PREVPANEL:
		case IDC_NEXTPANEL:
		{
			if (id == IDC_PREVPANEL && m_nCurrentPanel > 0)
				m_nCurrentPanel -= 1;
			else
			if (id == IDC_NEXTPANEL && (m_nCurrentPanel < m_nPanels-1))
				m_nCurrentPanel += 1;
			else
				break;

			HighlightPage(m_iHighlighted, FALSE);
			for (int i = 0; i < m_nPages; ++i)
			{
				if (m_pPages[i].m_nPanel == m_nCurrentPanel)
				{
					m_iHighlighted = i;
					break;
				}
			}
			ShowCurrentPanel();
			HighlightPage(m_iHighlighted, TRUE);
			break;
		}

		case IDC_PRINT:
		{
			FNAME szFileName;
			HCURSOR hCurSave;

			hCurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));
			GetPathName(szFileName, m_pPages[m_iHighlighted].m_szPage);
			PDIB pDib = CDib::LoadDibFromFile(szFileName, NULL);
			if (pDib)
			{
				STRING szLabel;
				lstrcpy( szLabel, m_pPages[m_iHighlighted].m_szName );
				lstrcat( szLabel, " - ©1996 Golden Books Publishing Company, Inc." );
				pDib->Print(hWnd, GetApp()->GetInstance(), 0, szLabel);
				delete pDib;
			}
			SetCursor(hCurSave);
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
BOOL CColorbookScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  

		default:
			return(FALSE);
	}
}

//************************************************************************
void CColorbookScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	POINT pt;

	pt.x = x; pt.y = y;
	LPSPRITE lpSprite = m_pAnimator->MouseInAnySprite(&pt);
	for (int i = 0; i < m_nPages; ++i)
	{
		if (m_pPages[i].m_lpSprite == lpSprite)
		{
			HighlightPage(m_iHighlighted, FALSE);
			m_iHighlighted = i;
			HighlightPage(m_iHighlighted, TRUE);
			break;
		}
	}
}

//************************************************************************
void CColorbookScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CColorbookScene::HighlightPage(int iPage, BOOL fHighlight)
//************************************************************************
{
	if (!m_lpHighlightSprite)
		return;
	if (fHighlight)
	{
		LPPAGE pPage = GetPage(iPage);
		if (pPage && pPage->m_lpSprite)
		{
			RECT rSprite, rHighlight;
			int dx, dy, x, y;

			pPage->m_lpSprite->Location(&rSprite);
			m_lpHighlightSprite->Location(&rHighlight);
			dx = (RectWidth(&rHighlight) - RectWidth(&rSprite)) / 2;
			x = rSprite.left - dx;
			dy = (RectHeight(&rHighlight) - RectHeight(&rSprite)) / 2;
			y = rSprite.top - dy;
			m_lpHighlightSprite->Jump(x, y);
			m_lpHighlightSprite->Show(TRUE);
		}
	}
	else
	{
		m_lpHighlightSprite->Show(FALSE);
	}
}

//************************************************************************
void CColorbookScene::ShowCurrentPanel()
//************************************************************************
{
	int i;

	// first hide all panels
	for (i = 0; i < m_nPages; ++i)
		m_pPages[i].m_lpSprite->Show(FALSE);

	// then show the current panel
	for (i = 0; i < m_nPages; ++i)
	{
		if (m_pPages[i].m_nPanel == m_nCurrentPanel)
			m_pPages[i].m_lpSprite->Show(TRUE);
	}
}

//************************************************************************
BOOL CColorbookScene::IsOnHotSpot(LPPOINT lpPoint)
//************************************************************************
{
	return(m_pAnimator->MouseInAnySprite(lpPoint) != NULL);
}

//************************************************************************
CPage::CPage()
//************************************************************************
{
	m_szThumb[0] = '\0';
	m_szPage[0] = '\0';
	m_szName[0] = '\0';
	m_nPanel = 0;
	m_lpSprite = NULL;
	m_ptPos.x = m_ptPos.y = 0;
}

//************************************************************************
CPage::~CPage()
//************************************************************************
{
}

//************************************************************************
BOOL CColorbookParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPCOLORBOOKSCENE lpScene = (LPCOLORBOOKSCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "highlight"))
			GetFilename(lpScene->m_szHighlight, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
	else
	{
		LPPAGE pPage = lpScene->GetPage(nIndex-1);
		if (!lstrcmpi(lpKey, "thumb"))
			GetFilename(pPage->m_szThumb, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "page"))
			GetFilename(pPage->m_szPage, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "name"))
			GetString(pPage->m_szName, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "panel"))
			GetInt(&pPage->m_nPanel, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "pos"))
			GetPoint(&pPage->m_ptPos, lpEntry, lpValues, nValues);
		else
			Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	}
		
	return(TRUE);
}

