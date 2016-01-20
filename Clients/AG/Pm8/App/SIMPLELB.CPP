/*
// $Workfile: SIMPLELB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIMPLELB.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:00   FRED
// Initial revision.
// 
//    Rev 1.0   13 May 1995 11:57:40   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "simplelb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleListBoxItem

CSimpleListBoxItem::CSimpleListBoxItem()
{
}

CSimpleListBoxItem::~CSimpleListBoxItem()
{
}

void CSimpleListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Paint the item.
	Paint(pList, pDrawItemStruct);
	
	if (IsFocused(pList, pDrawItemStruct))
	{
		 // This item is focused, preview it.
		Preview(pList, pDrawItemStruct);
	}
}

void CSimpleListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
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

void CSimpleListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

int CSimpleListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CSimpleListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Get the DC and rectangle to use.
			
	CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);
						
	if (pDC != NULL)
	{
		if ((pDrawItemStruct->itemAction & (ODA_DRAWENTIRE|ODA_SELECT)) != 0)
		{
			// Get the colors to use.
	
			COLORREF clForeground;
			COLORREF clBackground;
						
			ComputeColors(pList, pDC, pDrawItemStruct, clForeground, clBackground);
	
			// Compute the bounding rectangles for the item.
						
			CRect crItem;
											
			ComputeBounds(pList, pDC, pDrawItemStruct, crItem);
						
			// Draw the background.
						
			PaintBackground(pList, pDC, pDrawItemStruct, clBackground);
						
			// Draw the item.
						
			PaintItem(pList, pDC, pDrawItemStruct, crItem, clForeground, clBackground);
			
			// Draw the focus if the item is focused.
			
			if (IsFocused(pList, pDrawItemStruct))
			{
				PaintFocus(pList, pDC, pDrawItemStruct);
			}
		}
		
		else if ((pDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		{
			PaintFocus(pList, pDC, pDrawItemStruct);
		}
	}
}

					
void CSimpleListBoxItem::ComputeColors(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground)
{
	if (IsSelected(pList, pDrawItemStruct))
	{
		// Selected colors.
		
		clForeground			= GetSysColor(COLOR_HIGHLIGHTTEXT);
		clBackground			= GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		// Non selected colors.
		
		clForeground			= GetSysColor(COLOR_WINDOWTEXT);
		clBackground			= GetSysColor(COLOR_WINDOW);
	}
}

void CSimpleListBoxItem::ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crItem)
{
	crItem = pDrawItemStruct->rcItem;
}

void CSimpleListBoxItem::PaintBackground(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground)
{
	CBrush cbBackground(clBackground);
	pDC->FillRect(&(pDrawItemStruct->rcItem), &cbBackground);
}

void CSimpleListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
}

void CSimpleListBoxItem::PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	pDC->DrawFocusRect(&(pDrawItemStruct->rcItem));
}

void CSimpleListBoxItem::Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

BOOL CSimpleListBoxItem::IsSelected(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
}

BOOL CSimpleListBoxItem::IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_FOCUS) != 0;
}
