/*
// $Workfile: SIMPLECB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIMPLECB.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:58   FRED
// Initial revision.
// 
//    Rev 1.0   02 May 1995 15:41:18   FRED
// Initial revision.
*/

// simplecb.cpp : implementation file
//

#include "stdafx.h"
#include "ownercb.h"
#include "simplecb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpleComboBoxItem

CSimpleComboBoxItem::CSimpleComboBoxItem()
{
}

CSimpleComboBoxItem::~CSimpleComboBoxItem()
{
}

void CSimpleComboBoxItem::DrawItem(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Paint the item.
	Paint(pComboBox, pDrawItemStruct);
	
	if (IsFocused(pComboBox, pDrawItemStruct))
	{
		 // This item is focused, preview it.
		Preview(pComboBox, pDrawItemStruct);
	}
}

void CSimpleComboBoxItem::MeasureItem(COwnerDrawComboBox* pComboBox, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	// Set the item dimensions based on the current font.
		
	TEXTMETRIC tmFont;
			
	if (pComboBox->FontMetrics(tmFont))
	{
		CRect crClient;
		pComboBox->GetClientRect(crClient);
						
		pMeasureItemStruct->itemWidth = crClient.Width();
		pMeasureItemStruct->itemHeight = tmFont.tmHeight;
		
		if (pMeasureItemStruct->itemID == -1)
		{
			pMeasureItemStruct->itemHeight += 7;
		}			
	}
}

void CSimpleComboBoxItem::DeleteItem(COwnerDrawComboBox* pComboBox, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

int CSimpleComboBoxItem::CompareItem(COwnerDrawComboBox* pComboBox, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CSimpleComboBoxItem::Paint(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct)
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
						
			ComputeColors(pComboBox, pDC, pDrawItemStruct, clForeground, clBackground);
	
			// Compute the bounding rectangles for the item.
						
			CRect crItem;
											
			ComputeBounds(pComboBox, pDC, pDrawItemStruct, crItem);
						
			// Draw the background.
						
			PaintBackground(pComboBox, pDC, pDrawItemStruct, clBackground);
						
			// Draw the item.
						
			PaintItem(pComboBox, pDC, pDrawItemStruct, crItem, clForeground, clBackground);
			
			// Draw the focus if the item is focused.
			
			if (IsFocused(pComboBox, pDrawItemStruct))
			{
				PaintFocus(pComboBox, pDC, pDrawItemStruct);
			}
		}
		
		else if ((pDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		{
			PaintFocus(pComboBox, pDC, pDrawItemStruct);
		}
	}
}

					
void CSimpleComboBoxItem::ComputeColors(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground)
{
	if (IsSelected(pComboBox, pDrawItemStruct))
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

void CSimpleComboBoxItem::ComputeBounds(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crItem)
{
	crItem = pDrawItemStruct->rcItem;
}

void CSimpleComboBoxItem::PaintBackground(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground)
{
	CBrush cbBackground(clBackground);
	pDC->FillRect(&(pDrawItemStruct->rcItem), &cbBackground);
}

void CSimpleComboBoxItem::PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
}

void CSimpleComboBoxItem::PaintFocus(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	pDC->DrawFocusRect(&(pDrawItemStruct->rcItem));
}

void CSimpleComboBoxItem::Preview(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

BOOL CSimpleComboBoxItem::IsSelected(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
}

BOOL CSimpleComboBoxItem::IsFocused(COwnerDrawComboBox* pComboBox, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	return (pDrawItemStruct->itemState & ODS_FOCUS) != 0;
}
