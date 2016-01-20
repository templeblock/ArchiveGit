/*
// $Workfile: CHECKLB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CHECKLB.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:40   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:43:26   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:30   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CHECKLB.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:22   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:18   JAY
// Initial revision.
// 
//    Rev 1.22   27 Feb 1995 14:08:18   FRED
// Changed AdjustParents() to work with more than two levels
// (removed break).
// 
// 
//    Rev 1.21   24 Feb 1995 14:35:32   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.20   21 Jan 1995 10:54:56   FRED
// Changes to make free items listed at
// the end of the phone order list box.
// 
// 
//    Rev 1.19   20 Jan 1995 15:26:36   FRED
// New "free item marking" code.
// 
// 
//    Rev 1.18   19 Jan 1995 14:20:54   JAY
// New free pictures!
// 
//    Rev 1.17   04 Jan 1995 13:30:42   JAY
// Fixed a certain case of "jumpiness" in listbox when gaining focus.
// 
//    Rev 1.16   04 Jan 1995 11:30:28   JAY
// Selective installation.
// 
//    Rev 1.15   22 Dec 1994 17:30:14   JAY
// Scrolling() method to help breathing functions.
// 
//    Rev 1.14   22 Dec 1994 15:16:26   FRED
//  
// 
//    Rev 1.13   22 Dec 1994 11:51:20   FRED
// 
//    Rev 1.12   22 Dec 1994 10:41:04   FRED
// 
//    Rev 1.11   22 Dec 1994 08:58:02   FRED
//  
// 
//    Rev 1.10   16 Dec 1994 09:52:42   FRED
//  
// 
//    Rev 1.9   15 Dec 1994 17:12:56   FRED
//  
// 
//    Rev 1.8   15 Dec 1994 13:09:32   FRED
//  
// 
//    Rev 1.7   15 Dec 1994 09:33:26   FRED
// 
//    Rev 1.6   14 Dec 1994 17:40:46   FRED
// No change.
// 
//    Rev 1.5   14 Dec 1994 17:20:06   FRED
//  
// 
//    Rev 1.4   14 Dec 1994 16:26:04   FRED
// UpdateProductItems() clears flags.
// 
// 
//    Rev 1.3   14 Dec 1994 15:26:50   FRED
//  
// 
//    Rev 1.2   14 Dec 1994 10:18:28   FRED
//  
// 
//    Rev 1.1   13 Dec 1994 18:29:18   FRED
//  
// 
//    Rev 1.0   11 Dec 1994 11:01:56   FRED
// Initial revision.
*/

// checklb.cpp : implementation file
//

#include "stdafx.h"
#include "license.h"
#include "checklb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckListBoxItem

CCheckListBoxItem::CCheckListBoxItem(int nIndent /*=0*/)
{
	m_nIndent = nIndent;
	m_fMark = FALSE;
	m_fSelect = FALSE;
	m_fDisable = FALSE;
}

CCheckListBoxItem::~CCheckListBoxItem()
{
}

void CCheckListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
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

void CCheckListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
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

void CCheckListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

int CCheckListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CCheckListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
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

					
void CCheckListBoxItem::ComputeColors(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground, COLORREF& clCheckBorder, COLORREF& clCheckBackground, COLORREF& clCheck)
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

void CCheckListBoxItem::ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem)
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

void CCheckListBoxItem::PaintBackground(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground)
{
	CBrush cbBackground(clBackground);
	pDC->FillRect(&(pDrawItemStruct->rcItem), &cbBackground);
}

void CCheckListBoxItem::PaintCheckBox(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clBorder, COLORREF clBackground, COLORREF clCheck)
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

void CCheckListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
}

void CCheckListBoxItem::PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	pDC->DrawFocusRect(&(pDrawItemStruct->rcItem));
}

void CCheckListBoxItem::Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

BOOL CCheckListBoxItem::IsChecked(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return Mark();
}

BOOL CCheckListBoxItem::IsSelected(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
}

BOOL CCheckListBoxItem::IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
}

/////////////////////////////////////////////////////////////////////////////
// CProductCheckListBoxItem

CProductCheckListBoxItem::CProductCheckListBoxItem(CProductItem* pItem, int nIndent /*=0*/) :
	CCheckListBoxItem(nIndent)
{
	m_pItem = pItem;
}

CProductCheckListBoxItem::~CProductCheckListBoxItem()
{
}

void CProductCheckListBoxItem::UpdateProductItem(void)
{
	if (Mark())
	{
		m_pItem->SetFlag(PRODUCT_ITEM_Marked);
	}
	else
	{
		m_pItem->ClearFlag(PRODUCT_ITEM_Marked);
	}
	
	if (Select())
	{
		m_pItem->SetFlag(PRODUCT_ITEM_Selected);
	}
	else
	{
		m_pItem->ClearFlag(PRODUCT_ITEM_Selected);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCheckListBox

CCheckListBox::CCheckListBox(DWORD dwStyle /*=0*/)
{
	m_nFocus = 0;
	m_dwStyle = dwStyle;
	m_fMark = FALSE;
	m_fScrolling = FALSE;
}

CCheckListBox::~CCheckListBox()
{
	StopTimer();
}

BEGIN_MESSAGE_MAP(CCheckListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CCheckListBox)
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

int CCheckListBox::GetSel(int nIndex) const
{
	CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem != NULL)
	{
		return (int)(pListBoxItem->Select());
	}
	
	return LB_ERR;
}
	
CCheckListBoxItem* CCheckListBox::GetListBoxItem(int nIndex) const
{
	DWORD dwData = GetItemData(nIndex);
	
	if (dwData == LB_ERR)
	{
		return NULL;
	}
	
	return (CCheckListBoxItem*)dwData;
}

BOOL CCheckListBox::Select(int nIndex, BOOL fMark, BOOL fDragging)
{
	CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
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

void CCheckListBox::MarkItem(CCheckListBoxItem* pListBoxItem, int nIndex, BOOL fMark)
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

void CCheckListBox::SelectItem(CCheckListBoxItem* pListBoxItem, int nIndex, BOOL fSelect)
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

void CCheckListBox::DisableItem(CCheckListBoxItem* pListBoxItem, int nIndex, BOOL fDisable)
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

void CCheckListBox::AdjustFree(void)
{
// Nothing in particular to do.
}

BOOL CCheckListBox::SiblingIsMarked(int nIndex)
{
	int nIndent = GetListBoxItem(nIndex)->Indent();
	int nCount = GetCount();
	int nChildIndex;
		
	if (nCount != LB_ERR)
	{
	// Look back.
		for (nChildIndex = nIndex-1; nChildIndex >= 0; nChildIndex--)
		{
			CCheckListBoxItem* pListBoxItem = GetListBoxItem(nChildIndex);
			
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
			CCheckListBoxItem* pListBoxItem = GetListBoxItem(nChildIndex);

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

void CCheckListBox::AdjustChildren(int nIndex)
{
	CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem != NULL)
	{
		BOOL fMark = pListBoxItem->Mark();
		
		int nCount = GetCount();
			
		if (nCount != LB_ERR)
		{
			for (int nChildIndex = nIndex+1; nChildIndex < nCount; nChildIndex++)
			{
				CCheckListBoxItem* pChildListBoxItem = GetListBoxItem(nChildIndex);
				
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

BOOL CCheckListBox::AdjustParents(int nIndex /*=0*/)
{
	BOOL fResult = FALSE;
	
	int nCount = GetCount();
	
	if ((nCount > 0) && (nCount != LB_ERR))
	{
		CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
		
		if (pListBoxItem != NULL)
		{
			fResult = pListBoxItem->Mark();
			
			BOOL fSeenChild = FALSE;
		
			for (int nChildIndex = nIndex+1; nChildIndex < nCount; nChildIndex++)
			{
				CCheckListBoxItem* pChildListBoxItem = GetListBoxItem(nChildIndex);
				
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

int CCheckListBox::AdjustSelections(int nIndex /*=0*/, int nIndent /*=0*/, BOOL fForceDeselect /*=FALSE*/)
{
	int nCount = GetCount();
	
	if ((nCount > 0) && (nCount != LB_ERR))
	{
		while (nIndex < nCount)
		{
			CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
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

void CCheckListBox::MouseSelect(UINT nFlags, CPoint point)
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
				
#ifdef WIN32
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
				GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
			}
		}
	}
	m_fScrolling = (GetTopIndex() != nTopIndex);
}

void CCheckListBox::StartTimer(void)
{
	SetTimer(TIMER_ID, 100, NULL);
}

void CCheckListBox::StopTimer(void)
{
	if (m_hWnd != NULL)
	{
		KillTimer(TIMER_ID);
	}
}
	
/////////////////////////////////////////////////////////////////////////////
// CCheckListBox message handlers

void CCheckListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CCheckListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetCapture() != this)
	{
		int nIndex = Index(point);
		BOOL fCapture = TRUE;
		
		if (nIndex >= 0)
		{
			CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
			if (pListBoxItem != NULL)
			{
				m_fMark = !pListBoxItem->Mark();
				m_nCurrent = nIndex;
				
				SetCurSel(nIndex);
				fCapture = Select(nIndex, m_fMark, FALSE);
				SetFocus();
				
#ifdef WIN32
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
				GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
			}
		}
		
		if (fCapture)
		{
			StartTimer();
			SetCapture();
		}
	}
}

void CCheckListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_fScrolling = FALSE;
	StopTimer();
	ReleaseCapture();
}

void CCheckListBox::OnMouseMove(UINT nFlags, CPoint point)
{
}

void CCheckListBox::OnCancelMode()
{
	ReleaseCapture();
	COwnerDrawListBox::OnCancelMode();
}

void CCheckListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COwnerDrawListBox::OnChar(nChar, nRepCnt, nFlags);
}

void CCheckListBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COwnerDrawListBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCheckListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GetCapture() != this)
	{
		if (nChar == VK_SPACE)
		{
			int nIndex = GetCurSel();
			
			if (nIndex >= 0)
			{
				CCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
				
				if (pListBoxItem != NULL)
				{
					Select(nIndex, !pListBoxItem->Mark(), FALSE);
#ifdef WIN32
					GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
					GetParent()->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
				}
			}
		}
		
		else
		{
			COwnerDrawListBox::OnKeyDown(nChar, nRepCnt, nFlags);
		}
	}
}

void CCheckListBox::OnSetFocus(CWnd* pOldWnd)
{
	if (GetCurSel() == LB_ERR)
	{
		SetCurSel(m_nFocus);
	}
}

void CCheckListBox::OnKillFocus(CWnd* pNewWnd)
{
	m_nFocus = GetCurSel();
	SetCurSel(-1);
}

void CCheckListBox::OnTimer(UINT nIDEvent)
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

/////////////////////////////////////////////////////////////////////////////
// CProductCheckListBox

CProductCheckListBox::CProductCheckListBox(DWORD dwStyle /*=0*/) :
	CCheckListBox(dwStyle)
{
}

CProductCheckListBox::~CProductCheckListBox()
{
}

BEGIN_MESSAGE_MAP(CProductCheckListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CProductCheckListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CProductCheckListBoxItem* CProductCheckListBox::GetListBoxItem(int nIndex) const
{
	return (CProductCheckListBoxItem*)(CCheckListBox::GetListBoxItem(nIndex));
}

CProductItem* CProductCheckListBox::GetItem(int nIndex) const
{
	CProductCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem == NULL)
	{
		return NULL;
	}
	
	return pListBoxItem->Item();
}
	
void CProductCheckListBox::DisableLicensedItems(void)
{
	int nCount = GetCount();
	
	if (nCount != LB_ERR)
	{
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			CProductCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
			if (pListBoxItem != NULL)
			{
				BOOL fLicensed = pListBoxItem->Item()->IsLicensed();
				
				DisableItem(pListBoxItem, nIndex, fLicensed);
				
				if (fLicensed)
				{
					MarkItem(pListBoxItem, nIndex, TRUE);
					SelectItem(pListBoxItem, nIndex, FALSE);
				}
			}
		}
	}
}

void CProductCheckListBox::UpdateProductItems(void)
{
	GetApp()->Offer()->ClearProductFlags();
	
	int nCount = GetCount();
	
	if (nCount != LB_ERR)
	{
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			CProductCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
			if (pListBoxItem != NULL)
			{
				pListBoxItem->UpdateProductItem();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProductCheckListBox message handlers
