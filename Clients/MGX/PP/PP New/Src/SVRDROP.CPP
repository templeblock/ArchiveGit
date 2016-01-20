/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                svrdrop.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "mdiframe.h"
#include "svrdoc.h"
#include "svrview.h"

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

/* ********************************** Macros ******************************** */
/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* ******************************* Public Data ****************************** */

/* *************************** Private Functions **************************** */
 
BOOL IsOurMDIClient (CWnd *pWnd, CPoint *pt = NULL)
   {
   BOOL ret = PictPubApp.m_pMainWnd == pWnd;
   // we are doing this because the foundation is going from to to bottom
   // in the stack of windows to find a target.  This gives the effect
   // of dropping things through dialogs which is not pleasant

   if (pt)
      {
      CPoint   p = *pt;
      CWnd    *pWnd2;
      pWnd->ClientToScreen(&p);
      pWnd2 = CWnd::WindowFromPoint(p);
      ret = ret && (pWnd2->GetParent() == PictPubApp.m_pMainWnd);
      }
	return (ret);
   }

/* ***************************** Public Methods ***************************** */

DROPEFFECT CPPDropTarget::OnDragEnter(
CWnd* pWnd, 
COleDataObject* pDataObject,
DWORD dwKeyState, 
CPoint point)
{
	ASSERT_VALID(this);
	if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)) &&
		!IsOurMDIClient(pWnd, &point))
		return DROPEFFECT_NONE;

	BOOL bRet;
	if (!IsOurMDIClient(pWnd, &point))
	{
		CView* pView = (CView*)pWnd;
		ASSERT_VALID(pView);
		bRet = pView->OnDragEnter(pDataObject, dwKeyState, point);
	}
	else
	{
		CMDIFrame *pMDIFrame = (CMDIFrame*)pWnd;
		bRet = pMDIFrame->OnDragEnter(pDataObject, dwKeyState, point);
	}
	return bRet;
}

DROPEFFECT CPPDropTarget::OnDragOver(
CWnd* pWnd, 
COleDataObject* pDataObject,
DWORD dwKeyState, 
CPoint point)
{
	ASSERT_VALID(this);

	if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)) &&
		!IsOurMDIClient(pWnd, &point))
		return DROPEFFECT_NONE;

	BOOL bRet;
	if (!IsOurMDIClient(pWnd, &point))
	{
		CView* pView = (CView*)pWnd;
		ASSERT_VALID(pView);
		bRet = pView->OnDragOver(pDataObject, dwKeyState, point);
	}
	else
	{
		CMDIFrame *pMDIFrame = (CMDIFrame*)pWnd;
		bRet = pMDIFrame->OnDragOver(pDataObject, dwKeyState, point);
	}
	return bRet;
}

BOOL CPPDropTarget::OnDrop(
CWnd* pWnd, 
COleDataObject* pDataObject,
DROPEFFECT dropEffect, 
CPoint point)
{
	ASSERT_VALID(this);

	if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)) &&
		!IsOurMDIClient(pWnd, &point))
		return FALSE;

	BOOL bRet;
	if (!IsOurMDIClient(pWnd, &point))
	{
		CView* pView = (CView*)pWnd;
		ASSERT_VALID(pView);
		bRet = pView->OnDrop(pDataObject, dropEffect, point);
	}
	else
	{
		CMDIFrame* pMDIFrame = (CMDIFrame*)pWnd;
		bRet = pMDIFrame->OnDrop(pDataObject, dropEffect, point);
	}
	return bRet;
}

void CPPDropTarget::OnDragLeave(
CWnd* pWnd)
{
	ASSERT_VALID(this);

	if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)) &&
		!IsOurMDIClient(pWnd))
		return;

	BOOL bRet;
	if (!IsOurMDIClient(pWnd))
	{
		CView* pView = (CView*)pWnd;
		ASSERT_VALID(pView);
		pView->OnDragLeave();
	}
	else
	{
		CMDIFrame* pMDIFrame = (CMDIFrame*)pWnd;
		pMDIFrame->OnDragLeave();
	}
}

BOOL CPPDropTarget::OnDragScroll(
CWnd* pWnd, 
DWORD dwKeyState, 
CPoint point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pWnd);

	// CWnds are allowed, but don't support autoscrolling
	if (!pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))
		return FALSE;

	CView* pView = (CView*)pWnd;

	// get client rectangle of destination window
	CRect rectClient;
	pWnd->GetClientRect(&rectClient);
	CRect rect = rectClient;

	// hit-test against inset region
	UINT nTimerID = MAKEWORD(-1, -1);
	rect.InflateRect(-nScrollInset, -nScrollInset);
	CSplitterWnd* pSplitter;
	if (rectClient.PtInRect(point) && !rect.PtInRect(point))
	{
		// determine which way to scroll along both X & Y axis
		if (point.x < rect.left)
			nTimerID = MAKEWORD(SB_LINEUP, HIBYTE(nTimerID));
		else if (point.x >= rect.right)
			nTimerID = MAKEWORD(SB_LINEDOWN, HIBYTE(nTimerID));
		if (point.y < rect.top)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEUP);
		else if (point.y >= rect.bottom)
			nTimerID = MAKEWORD(LOBYTE(nTimerID), SB_LINEDOWN);
		ASSERT(nTimerID != MAKEWORD(-1, -1));

		// check for valid scroll first
		pSplitter = CView::GetParentSplitter(pView, FALSE);
		BOOL bEnableScroll = FALSE;
		if (pSplitter != NULL)
			bEnableScroll = pSplitter->DoScroll(pView, nTimerID, FALSE);
		else
			bEnableScroll = pView->OnScroll(nTimerID, 0, FALSE);
		if (!bEnableScroll)
			nTimerID = MAKEWORD(-1, -1);
	}

	if (nTimerID == MAKEWORD(-1, -1))
	{
		if (m_nTimerID != MAKEWORD(-1, -1))
		{
			// send fake OnDragEnter when transition from scroll->normal
			COleDataObject dataObject;
			dataObject.Attach(m_lpDataObject, FALSE);
			OnDragEnter(pWnd, &dataObject, dwKeyState, point);
			m_nTimerID = MAKEWORD(-1, -1);
		}
		return FALSE;
	}

	// save tick count when timer ID changes
	DWORD dwTick = GetTickCount();
	if (nTimerID != m_nTimerID)
	{
		m_dwLastTick = dwTick;
		m_nScrollDelay = nScrollDelay;
	}

	// scroll if necessary
	if (dwTick - m_dwLastTick > m_nScrollDelay)
	{
		if (pSplitter != NULL)
			pSplitter->DoScroll(pView, nTimerID, TRUE);
		else
			pView->OnScroll(nTimerID, 0, TRUE);
		m_dwLastTick = dwTick;
		m_nScrollDelay = nScrollInterval;
	}
	if (m_nTimerID == MAKEWORD(-1, -1))
	{
		// send fake OnDragLeave when transitioning from normal->scroll
		OnDragLeave(pWnd);
	}

	m_nTimerID = nTimerID;
	return TRUE;
}


/******************************* EXPORTED Functions *****************************/

