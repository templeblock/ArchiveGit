// TypeCmbo.cpp : implementation file
//

#include <stdafx.h>

#include "TypeCmbo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;
const char SECTION_FontsMRU[] = "Recent Font List";
const char ENTRY_FontMRU[] = "Font%d";

/////////////////////////////////////////////////////////////////////////////
// CTypefaceCombo

CTypefaceCombo::CTypefaceCombo() :
	CBarMRUCombo(0, SECTION_FontsMRU, ENTRY_FontMRU, MAX_FONTS_MRU)
{
	m_nTimer = 0;
	m_bShowPreview = FALSE;
	m_ComboInfo.pCombo = this;
	m_ComboInfo.pMRUList = &m_RecentItemList;
}

CTypefaceCombo::~CTypefaceCombo()
{
}

BOOL CTypefaceCombo::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (INHERITED::Create(dwStyle, rect, pParentWnd, nID))
	{
		m_pMainWnd = AfxGetMainWnd();
		return TRUE;
	}
	return FALSE;
}

void CTypefaceCombo::BuildList()
{
	m_pMainWnd->SendMessage(UM_BUILD_FONT_LIST, 0, (LPARAM)&m_ComboInfo);
}

// Preview window
void CTypefaceCombo::ShowPreview()
{
	if (GetDroppedState())
	{
		UpdatePreview();
		MovePreview();
		m_pMainWnd->SendMessage(UM_SHOW_PREVIEW, 0, SW_SHOWNOACTIVATE);
	}
}

void CTypefaceCombo::HidePreview()
{
	m_pMainWnd->SendMessage(UM_SHOW_PREVIEW, 0, SW_HIDE);
}

void CTypefaceCombo::MovePreview()
{
	// Move the window to be next to the box
	CRect rct;
	GetClientRect(rct);
	ClientToScreen(rct);
	rct.OffsetRect(rct.Width(), 0);
	m_pMainWnd->SendMessage(UM_MOVE_PREVIEW, rct.TopLeft().x, rct.TopLeft().y+30);
}

void CTypefaceCombo::UpdatePreview()
{
	m_pMainWnd->SendMessage(UM_UPDATE_PREVIEW, 0, GetItemData(m_DrawItemStruct.itemID));
}

void CTypefaceCombo::UpdateMRU()
{
	// Make sure preview goes AWAY
	HidePreview();

	CBarMRUCombo::UpdateMRU();
}

BEGIN_MESSAGE_MAP(CTypefaceCombo, CBarMRUCombo)
	//{{AFX_MSG_MAP(CTypefaceCombo)
	ON_WM_DRAWITEM()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTypefaceCombo message handlers

void CTypefaceCombo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// See if we need to set the preview creation timer
	BOOL bPreviewVisible = m_pMainWnd->SendMessage(UM_PREVIEW_VISIBLE, 0, 0);
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0 &&
		(lpDrawItemStruct->itemID != m_DrawItemStruct.itemID || !bPreviewVisible))
	{
		m_DrawItemStruct = *lpDrawItemStruct;
		if (!bPreviewVisible)
		{
			// Kill the last timer
			if (m_nTimer)
			{
				KillTimer(m_nTimer);
				m_nTimer = 0;
			}

			// Set the timer for creation (for a delay)
			if ((m_nTimer = SetTimer(1, 750, NULL)) != 0)
			{
				// All set...
				m_bShowPreview = TRUE;
			}
			else
			{
				// Failed to create timer-- do preview now
				ShowPreview();
			}
		}
		else
		{
			// Update with new font
			UpdatePreview();
		}
	}
	
	CBarMRUCombo::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CTypefaceCombo::OnTimer(UINT nIDEvent) 
{
	// May need to show a preview window
	if (m_bShowPreview)
	{
		ShowPreview();
		m_bShowPreview = FALSE;
	}
}

void CTypefaceCombo::OnKillFocus(CWnd* pNewWnd) 
{
	CBarMRUCombo::OnKillFocus(pNewWnd);	
	HidePreview();
}

void CTypefaceCombo::OnCloseup() 
{
	HidePreview();
}

