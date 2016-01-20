#include "stdafx.h"
#include "Ctp.h"
#include "Highlight.h"

void * CHighlight::pObject;
bool CHighlight::g_bOn;

///////////////////////////////////////////////////////////////////////////////
CHighlight::CHighlight(CDocWindow* pDocWnd)
{
	m_pDocWnd = pDocWnd;
	m_pHlSym = NULL;
	m_uiTimer = 0;
}

///////////////////////////////////////////////////////////////////////////////
CHighlight::~CHighlight()
{
	StopHighlight();
	if (m_pDocWnd && m_pHlSym)
		m_pDocWnd->SymbolInvalidate(m_pHlSym);
}

///////////////////////////////////////////////////////////////////////////////
void CHighlight::StopHighlight()
{
	if (m_uiTimer)
	{
		KillTimer(NULL, m_uiTimer);
		m_uiTimer = 0;
	}

	m_Track.StopHighlight();

	if (m_pDocWnd->m_Select.SymSelected())
	{
		m_pDocWnd->m_Select.ResumeTracking();
		m_pDocWnd->SymbolInvalidate(m_pDocWnd->m_Select.GetSym());
	}

	if (m_pDocWnd && m_pHlSym)
		m_pDocWnd->SymbolInvalidate(m_pHlSym);

	m_pHlSym = NULL;
}

//////////////////////////////////////////////////////////////////////
void CALLBACK CHighlight::SelFlashProc_Wrapper(HWND hWnd, UINT message, UINT idTimer, DWORD dwTime)
{
	CHighlight* pHighlight = (CHighlight*)pObject;
	if (idTimer == pHighlight->m_uiTimer)
	{
		g_bOn = !g_bOn;
		pHighlight->Highlight();
	}
}

///////////////////////////////////////////////////////////////////////////////
void CHighlight::Highlight()
{
	m_Track.Highlight(true, NULL, g_bOn);
	m_pDocWnd->SymbolInvalidate(m_pHlSym);
}

///////////////////////////////////////////////////////////////////////////////
void CHighlight::StartHighlight()
{
	if (!m_pDocWnd->m_pClientDC)
		return;

	RECT SymbolRect = m_pHlSym->GetDestRect();
	RECT BoundRect = SymbolRect;
	RECT PageRect = m_pDocWnd->m_PageRect;
	CAGMatrix ViewToDeviceMatrix = m_pDocWnd->m_pClientDC->GetViewToDeviceMatrix();
	CAGMatrix SymbolMatrix = m_pHlSym->GetMatrix();
	CGrid rGrid = m_pDocWnd->m_Grid;

	if (m_pDocWnd->m_Select.SymSelected())
		m_pDocWnd->m_Select.SuspendTracking();

	m_Track.Init(m_pDocWnd->m_pClientDC, SymbolRect, BoundRect, PageRect, ViewToDeviceMatrix, SymbolMatrix, rGrid);

	m_uiTimer = SetTimer(NULL,						// handle to main window 
						NULL,						// timer identifier 
						500,						// 1-second interval 
			(TIMERPROC) SelFlashProc_Wrapper);		// timer callback 

	pObject = this;
	g_bOn = false;
}

///////////////////////////////////////////////////////////////////////////////
void CHighlight::DoHighlight(int x, int y, CAGSym* pDragSym, bool bDrop)
{
	if (bDrop)
	{
		StopHighlight();
		return;
	}

	if (m_pHlSym == pDragSym)
		return;

	StopHighlight();
	if (pDragSym && pDragSym->IsAddAPhoto())
	{
		m_pHlSym = pDragSym;
		StartHighlight();
	}
}

///////////////////////////////////////////////////////////////////////////////
void CHighlight::Paint(HDC pDC)
{
	m_Track.Paint(pDC);
}


//////////////////////////////////////////////////////////////////////
//void CHighlight::StartHighlight(bool bOn, HDC hDC)
//{
//	if (!m_pAGDC)
//		return;
//
//	m_bIsHighlight = bOn;
//
//	m_uiTimer = SetTimer(m_pAGDC->GetWnd(),			// handle to main window 
//					IDT_SEL_FLASH,					// timer identifier 
//					500,							// 1-second interval 
//			(TIMERPROC) SelFlashProc_Wrapper);		// timer callback 
//
//	//if (!m_uiTimer)
//	//	ATLTRACE("Failed to start timer\n");
//
//	pObject = this;
//	g_bOn = false;
//}
//
