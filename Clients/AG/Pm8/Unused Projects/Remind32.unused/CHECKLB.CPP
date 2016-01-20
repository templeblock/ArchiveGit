/*
// $Workfile: CHECKLB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/CHECKLB.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:24   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:44   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "ownerlb.h"
#include "checklb.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckedListBoxItem

CCheckedListBoxItem::CCheckedListBoxItem(int nIndent /*=0*/)
{
	m_nIndent = nIndent;
	m_fMark = FALSE;
	m_fSelect = FALSE;
	m_fDisable = FALSE;
}

CCheckedListBoxItem::~CCheckedListBoxItem()
{
}

void CCheckedListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if (pDrawItemStruct->itemID != -1)
	{
		if ((pDrawItemStruct->itemAction & (ODA_DRAWENTIRE|ODA_SELECT|ODA_FOCUS)) != 0)
		{
			// Paint the item.
			Paint(pList, pDrawItemStruct);
		}
			
		if (IsFocused(pList, pDrawItemStruct))
		{
			 // This item is focused, preview it.
			Preview(pList, pDrawItemStruct);
		}
	}
}

void CCheckedListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	// Set the item dimensions based on the current font.
		
	CClientDC cdcWnd(pList);
				
	CFont* pcfWnd = pList->GetFont();
			
	if (pcfWnd != NULL)
	{
		CFont* pcfOld = cdcWnd.SelectObject(pcfWnd);
				
		if (pcfOld != NULL)
		{
			TEXTMETRIC tmFont;
					
			if (cdcWnd.GetTextMetrics(&tmFont))
			{
				CRect crClient;
				pList->GetClientRect(crClient);
						
				pMeasureItemStruct->itemWidth = crClient.Width();
				pMeasureItemStruct->itemHeight = tmFont.tmHeight;
			}
					
			cdcWnd.SelectObject(pcfOld);
		}
	}
}

void CCheckedListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

int CCheckedListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CCheckedListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Get the DC and rectangle to use.
			
	CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);
						
	if (pDC != NULL)
	{
		// Get the colors to use.

		COLORREF clForeground;
		COLORREF clBackground;
		COLORREF clCheckBorder;
		COLORREF clCheckBackground;
		COLORREF clCheck;
					
		ComputeColors(pList, pDC, pDrawItemStruct, clForeground, clBackground, clCheckBorder, clCheckBackground, clCheck);

		// Compute the bounding rectangles for the check box and the item.
					
		CRect crCheckBox;
		CRect crItem;
										
		ComputeBounds(pList, pDC, pDrawItemStruct, crCheckBox, crItem);
					
		// Draw the background.
					
		PaintBackground(pList, pDC, pDrawItemStruct, clBackground);
					
		// Draw the checkbox.
					
		PaintCheckBox(pList, pDC, pDrawItemStruct, crCheckBox, clCheckBorder, clCheckBackground, clCheck);
					
		// Draw the item.
					
		PaintItem(pList, pDC, pDrawItemStruct, crItem, clForeground, clBackground);
					
		// If the item has the focus, draw the focus indicator.
					
		if (IsFocused(pList, pDrawItemStruct))
		{
			PaintFocus(pList, pDC, pDrawItemStruct);
		}
	}
}

					
void CCheckedListBoxItem::ComputeColors(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground, COLORREF& clCheckBorder, COLORREF& clCheckBackground, COLORREF& clCheck)
{
	if (IsSelected(pList, pDrawItemStruct))
	{
		// Selected colors.
		
		clForeground			= GetSysColor(COLOR_HIGHLIGHTTEXT);
		clBackground			= GetSysColor(COLOR_HIGHLIGHT);
		clCheckBorder			= GetSysColor(COLOR_WINDOWTEXT);
		clCheckBackground		= GetSysColor(COLOR_WINDOW);
		clCheck					= GetSysColor(COLOR_WINDOWTEXT);
	}
	else
	{
		// Non selected colors.
		
		clForeground			= GetSysColor(COLOR_WINDOWTEXT);
		clBackground			= GetSysColor(COLOR_WINDOW);
		clCheckBorder			= GetSysColor(COLOR_WINDOWTEXT);
		clCheckBackground		= GetSysColor(COLOR_WINDOW);
		clCheck					= GetSysColor(COLOR_WINDOWTEXT);
	}
	
	if (Disable())
	{
		clForeground			= GetSysColor(COLOR_GRAYTEXT);
		clCheckBorder			= GetSysColor(COLOR_GRAYTEXT);
		clCheck					= GetSysColor(COLOR_GRAYTEXT);
	}
}

void CCheckedListBoxItem::ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem)
{
	crCheckBox.SetRectEmpty();
	crItem.SetRectEmpty();
	
	int nXResolution = pDC->GetDeviceCaps(LOGPIXELSX);
	int nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);
						
	int nYOffset = 2;
	int nXOffset = 2;
						
 	int nYSize = (pDrawItemStruct->rcItem.bottom-pDrawItemStruct->rcItem.top)-2*nYOffset;
	int nXSize = MulDiv(nYSize, nXResolution, nYResolution);
						
	crCheckBox.left   = pDrawItemStruct->rcItem.left+nXOffset;
	crCheckBox.top    = pDrawItemStruct->rcItem.top+nYOffset;
	crCheckBox.right  = crCheckBox.left+nXSize;
	crCheckBox.bottom = crCheckBox.top+nYSize;
				
	crCheckBox.OffsetRect(m_nIndent*(nXSize+nXOffset), 0);
				
	if ((crCheckBox.left < pDrawItemStruct->rcItem.left) || (crCheckBox.right > pDrawItemStruct->rcItem.right))
	{
		crCheckBox.SetRectEmpty();
	}
				
	if (!crCheckBox.IsRectEmpty())
	{
		crItem.left   = crCheckBox.right+2*nXOffset;
		crItem.top    = pDrawItemStruct->rcItem.top;
		crItem.right  = pDrawItemStruct->rcItem.right;
		crItem.bottom = pDrawItemStruct->rcItem.bottom;
	}
}

void CCheckedListBoxItem::PaintBackground(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground)
{
	CBrush cbBackground(clBackground);
	pDC->FillRect(&(pDrawItemStruct->rcItem), &cbBackground);
}

void CCheckedListBoxItem::PaintCheckBox(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clBorder, COLORREF clBackground, COLORREF clCheck)
{
	int nContext = 0;
	
	CPen cpBorder;
	CBrush cbBackground;
	CPen cpCheck;
	CBrush cbCheck;
	
	TRY
	{
		if (!crBounds.IsRectEmpty())
		{
			if (!cpBorder.CreatePen(PS_SOLID, 0, clBorder))		AfxThrowResourceException();
			if (!cbBackground.CreateSolidBrush(clBackground))	AfxThrowResourceException();
			if (!cpCheck.CreatePen(PS_SOLID, 0, clCheck))		AfxThrowResourceException();
			if (!cbCheck.CreateSolidBrush(clCheck))				AfxThrowResourceException();
		
			if ((nContext = pDC->SaveDC()) != 0)
			{
				// Draw the check box.
				
				pDC->SelectObject(&cpBorder);
				pDC->SelectObject(&cbBackground);
				pDC->Rectangle(crBounds);
				
				BOOL fChecked = IsChecked(pList, pDrawItemStruct);
				
				if (fChecked)
				{
					if ((crBounds.Height() >= 17) && (crBounds.Width() >= 17))
					{
						// Draw really thick "X"
								
						pDC->SelectObject(&cpCheck);
							
						int nX0 = crBounds.left+2;
						int nY0 = crBounds.top+2;
												
						int nX1 = crBounds.right-2;
						int nY1 = crBounds.bottom-2;
												
						pDC->MoveTo(nX0, nY0);
						pDC->LineTo(nX1, nY1);
												
						pDC->MoveTo(nX0, nY0+1);
						pDC->LineTo(nX1-1, nY1);
												
						pDC->MoveTo(nX0, nY0+2);
						pDC->LineTo(nX1-2, nY1);
												
						pDC->MoveTo(nX0+1, nY0);
						pDC->LineTo(nX1, nY1-1);
					
						pDC->MoveTo(nX0+2, nY0);
						pDC->LineTo(nX1, nY1-2);
					
						pDC->MoveTo(nX1-1, nY0);
						pDC->LineTo(nX0-1, nY1);
												
						pDC->MoveTo(nX1-1-1, nY0);
						pDC->LineTo(nX0-1, nY1-1);
												
						pDC->MoveTo(nX1-1-2, nY0);
						pDC->LineTo(nX0-1, nY1-2);
												
						pDC->MoveTo(nX1-1, nY0+1);
						pDC->LineTo(nX0-1+1, nY1);
							
						pDC->MoveTo(nX1-1, nY0+2);
						pDC->LineTo(nX0-1+2, nY1);
					}
					
					else if ((crBounds.Height() >= 9) && (crBounds.Width() >= 9))
					{
						// Draw thick "X"
								
						pDC->SelectObject(&cpCheck);
							
						int nX0 = crBounds.left+2;
						int nY0 = crBounds.top+2;
												
						int nX1 = crBounds.right-2;
						int nY1 = crBounds.bottom-2;
												
						pDC->MoveTo(nX0, nY0);
						pDC->LineTo(nX1, nY1);
												
						pDC->MoveTo(nX0, nY0+1);
						pDC->LineTo(nX1-1, nY1);
												
						pDC->MoveTo(nX0+1, nY0);
						pDC->LineTo(nX1, nY1-1);
					
						pDC->MoveTo(nX1-1, nY0);
						pDC->LineTo(nX0-1, nY1);
												
						pDC->MoveTo(nX1-1-1, nY0);
						pDC->LineTo(nX0-1, nY1-1);
												
						pDC->MoveTo(nX1-1, nY0+1);
						pDC->LineTo(nX0-1+1, nY1);
					}
					
					else
					{
						// Draw thin "X"
								
						pDC->SelectObject(&cpCheck);
							
						int nX0 = crBounds.left+2;
						int nY0 = crBounds.top+2;
												
						int nX1 = crBounds.right-2;
						int nY1 = crBounds.bottom-2;
												
						pDC->MoveTo(nX0, nY0);
						pDC->LineTo(nX1, nY1);
												
						pDC->MoveTo(nX1-1, nY0);
						pDC->LineTo(nX0-1, nY1);
					}
				}

				pDC->RestoreDC(nContext);
				nContext = 0;
			}
		}
	}
	CATCH_ALL(e)
	{
		if (nContext != 0)
		{
			pDC->RestoreDC(nContext);
		}
	}
	END_CATCH_ALL
}

void CCheckedListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
}

void CCheckedListBoxItem::PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	pDC->DrawFocusRect(&(pDrawItemStruct->rcItem));
}

void CCheckedListBoxItem::Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

BOOL CCheckedListBoxItem::IsChecked(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return Mark();
}

BOOL CCheckedListBoxItem::IsSelected(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
}

BOOL CCheckedListBoxItem::IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// CCheckedListBox

CCheckedListBox::CCheckedListBox(DWORD dwStyle /*=0*/)
{
	m_nFocus = 0;
	m_dwStyle = dwStyle;
	m_fMark = FALSE;
	m_fScrolling = FALSE;
}

CCheckedListBox::~CCheckedListBox()
{
}

BEGIN_MESSAGE_MAP(CCheckedListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CCheckedListBox)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CANCELMODE()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCheckedListBox::OnDestroy()
{
	StopTimer();
	COwnerDrawListBox::OnDestroy();
}

int CCheckedListBox::GetSel(int nIndex) const
{
	CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem != NULL)
	{
		return (int)(pListBoxItem->Select());
	}
	
	return LB_ERR;
}
	
CCheckedListBoxItem* CCheckedListBox::GetListBoxItem(int nIndex) const
{
	DWORD dwData = GetItemData(nIndex);
	
	if (dwData == LB_ERR)
	{
		return NULL;
	}
	
	return (CCheckedListBoxItem*)dwData;
}

BOOL CCheckedListBox::Select(int nIndex, BOOL fMark, BOOL fDragging)
{
	CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem != NULL)
	{
		if (!pListBoxItem->Disable())
		{
			if (pListBoxItem->Mark() != fMark)
			{
				MarkItem(pListBoxItem, nIndex, fMark);
				AdjustChildren(nIndex);
				AdjustFree();
				AdjustParents();
				AdjustSelections();
			}
		}
	}
	return TRUE;
}

void CCheckedListBox::MarkItem(CCheckedListBoxItem* pListBoxItem, int nIndex, BOOL fMark)
{
	if (pListBoxItem->Mark() != fMark)
	{
		pListBoxItem->Mark(fMark);
		
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) != LB_ERR)
		{
			InvalidateRect(crItem, FALSE);
		}
	}
}

void CCheckedListBox::SelectItem(CCheckedListBoxItem* pListBoxItem, int nIndex, BOOL fSelect)
{
	if (pListBoxItem->Select() != fSelect)
	{
		pListBoxItem->Select(fSelect);
		
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) != LB_ERR)
		{
			InvalidateRect(crItem, FALSE);
		}
	}
}

void CCheckedListBox::DisableItem(CCheckedListBoxItem* pListBoxItem, int nIndex, BOOL fDisable)
{
	if (pListBoxItem->Disable() != fDisable)
	{
		pListBoxItem->Disable(fDisable);
		
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) != LB_ERR)
		{
			InvalidateRect(crItem, FALSE);
		}
	}
}

void CCheckedListBox::AdjustFree(void)
{
// Nothing in particular to do.
}

BOOL CCheckedListBox::SiblingIsMarked(int nIndex)
{
	int nIndent = GetListBoxItem(nIndex)->Indent();
	int nCount = GetCount();
	int nChildIndex;
		
	if (nCount != LB_ERR)
	{
	// Look back.
		for (nChildIndex = nIndex-1; nChildIndex >= 0; nChildIndex--)
		{
			CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nChildIndex);
			
			if (pListBoxItem != NULL)
			{
				if (pListBoxItem->Indent() < nIndent)
				{
					break;
				}
	
				if (pListBoxItem->Mark())
				{
					return TRUE;
				}
			}
		}
	// Look forward.
		for (nChildIndex = nIndex+1; nChildIndex < nCount; nChildIndex++)
		{
			CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nChildIndex);

			if (pListBoxItem != NULL)
			{
				if (pListBoxItem->Indent() < nIndent)
				{
					break;
				}
	
				if (pListBoxItem->Mark())
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CCheckedListBox::AdjustChildren(int nIndex)
{
	CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem != NULL)
	{
		BOOL fMark = pListBoxItem->Mark();
		
		int nCount = GetCount();
			
		if (nCount != LB_ERR)
		{
			for (int nChildIndex = nIndex+1; nChildIndex < nCount; nChildIndex++)
			{
				CCheckedListBoxItem* pChildListBoxItem = GetListBoxItem(nChildIndex);
				
				if (pChildListBoxItem != NULL)
				{
					if (pChildListBoxItem->Indent() <= pListBoxItem->Indent())
					{
						break;
					}
					
					if (pChildListBoxItem->Indent() > pListBoxItem->Indent())
					{
						if (!pChildListBoxItem->Disable())
						{
							MarkItem(pChildListBoxItem, nChildIndex, fMark);
						}
					}
				}
			}
		}
	}
}

BOOL CCheckedListBox::AdjustParents(int nIndex /*=0*/)
{
	BOOL fResult = FALSE;
	
	int nCount = GetCount();
	
	if ((nCount > 0) && (nCount != LB_ERR))
	{
		CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
		
		if (pListBoxItem != NULL)
		{
			fResult = pListBoxItem->Mark();
			
			BOOL fSeenChild = FALSE;
		
			for (int nChildIndex = nIndex+1; nChildIndex < nCount; nChildIndex++)
			{
				CCheckedListBoxItem* pChildListBoxItem = GetListBoxItem(nChildIndex);
				
				if (pChildListBoxItem != NULL)
				{
					if (pChildListBoxItem->Indent() <= pListBoxItem->Indent())
					{
						break;
					}
					
					if (!fSeenChild)
					{
						fResult = TRUE;
						fSeenChild = TRUE;
					}
						
					if (pChildListBoxItem->Indent() == pListBoxItem->Indent()+1)
					{
						fResult &= AdjustParents(nChildIndex);
					}
				}
			}
			
			if ((!fResult) || ((m_dwStyle & CHECK_LIST_BOX_DisableAutoMarkParent) == 0))
			{
				MarkItem(pListBoxItem, nIndex, fResult);
			}
		}
	}
	
	return fResult;
}

int CCheckedListBox::AdjustSelections(int nIndex /*=0*/, int nIndent /*=0*/, BOOL fForceDeselect /*=FALSE*/)
{
	int nCount = GetCount();
	
	if ((nCount > 0) && (nCount != LB_ERR))
	{
		while (nIndex < nCount)
		{
			CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
			BOOL fSelect = FALSE;

			if (pListBoxItem != NULL)
			{			
				if (pListBoxItem->Indent() < nIndent)
				{
					return nIndex;
				}
				
				fSelect = !fForceDeselect && pListBoxItem->Mark();
		
				if (!pListBoxItem->Disable())
				{
					SelectItem(pListBoxItem, nIndex, fSelect);
				}
			}
				
			nIndex = AdjustSelections(nIndex+1, nIndent+1, fForceDeselect || fSelect);
		}
	}
	
	return nCount;
}

void CCheckedListBox::MouseSelect(UINT nFlags, CPoint point)
{
	int nTopIndex = GetTopIndex();

	if (GetCapture() == this)
	{
		CRect crClient;
		GetClientRect(crClient);
		
		int nIndex = Index(point);
		
		if (nIndex >= 0)
		{
			if (nIndex != m_nCurrent)
			{
				int nItem = m_nCurrent;
				
				while (nItem != nIndex)
				{
					if (nItem < nIndex)
					{
						nItem++;
					}
					else
					{
						nItem--;
					}
					
					Select(nItem, m_fMark, TRUE);
				}

				UpdateWindow();
				
				SetCurSel(nIndex);
				m_nCurrent = nIndex;
				
				Util::SendWmCommand(GetParent(), GetDlgCtrlID(), this, LBN_SELCHANGE);
			}
		}
	}
	m_fScrolling = (GetTopIndex() != nTopIndex);
}

BOOL CCheckedListBox::RemoveSelectIfNotFocused(void)
{
	// Default is to leave nothing selected when list box is not focused,
	return TRUE;
}

void CCheckedListBox::StartTimer(void)
{
	SetTimer(TIMER_ID, 100, NULL);
}

void CCheckedListBox::StopTimer(void)
{
	KillTimer(TIMER_ID);
}
	
/////////////////////////////////////////////////////////////////////////////
// CCheckedListBox message handlers

void CCheckedListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CCheckedListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetCapture() != this)
	{
		int nIndex = Index(point);
		BOOL fCapture = TRUE;
		
		if (nIndex >= 0)
		{
			CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
			if (pListBoxItem != NULL)
			{
				m_fMark = !pListBoxItem->Mark();
				m_nCurrent = nIndex;
				
				SetCurSel(nIndex);
				fCapture = Select(nIndex, m_fMark, FALSE);
				SetFocus();
				
				Util::SendWmCommand(GetParent(), GetDlgCtrlID(), this, LBN_SELCHANGE);
			}
		}
		
		if (fCapture)
		{
			StartTimer();
			SetCapture();
		}
	}
}

void CCheckedListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_fScrolling = FALSE;
	StopTimer();
	ReleaseCapture();
}

void CCheckedListBox::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CCheckedListBox::OnCancelMode()
{
	ReleaseCapture();
	COwnerDrawListBox::OnCancelMode();
}

void CCheckedListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COwnerDrawListBox::OnChar(nChar, nRepCnt, nFlags);
}

void CCheckedListBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COwnerDrawListBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCheckedListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetCapture() != this)
	{
		if (nChar == VK_SPACE)
		{
			int nIndex = GetCurSel();
			
			if (nIndex >= 0)
			{
				CCheckedListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
				
				if (pListBoxItem != NULL)
				{
					Select(nIndex, !pListBoxItem->Mark(), FALSE);
					Util::SendWmCommand(GetParent(), GetDlgCtrlID(), this, LBN_SELCHANGE);
				}
			}
		}
		
		else
		{
			COwnerDrawListBox::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}
}

void CCheckedListBox::OnSetFocus(CWnd* pOldWnd)
{
	COwnerDrawListBox::OnSetFocus(pOldWnd);
	if (GetCurSel() == LB_ERR)
	{
		SetCurSel(m_nFocus);
	}
}

void CCheckedListBox::OnKillFocus(CWnd* pNewWnd)
{
	COwnerDrawListBox::OnKillFocus(pNewWnd);
	m_nFocus = GetCurSel();
	
	if (RemoveSelectIfNotFocused())
	{
		SetCurSel(-1);
	}
}

void CCheckedListBox::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TIMER_ID)
	{
		if (GetCapture() == this)
		{
			CPoint cpPosition;
			GetCursorPos(&cpPosition);
			ScreenToClient(&cpPosition);
			MouseSelect(0, cpPosition);
		}
	}
	else
	{
		COwnerDrawListBox::OnTimer(nIDEvent);
	}
}
