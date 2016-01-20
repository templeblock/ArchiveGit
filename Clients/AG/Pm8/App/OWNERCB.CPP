/*
// $Workfile: OWNERCB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/OWNERCB.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:32   Fred
// Initial revision.
// 
//    Rev 1.4   05 Nov 1996 14:06:52   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.3   01 Nov 1996 17:12:24   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.2   31 Aug 1996 17:35:52   Fred
// Sets proper edit height for 3.1 Combo box
// 
//    Rev 1.1   24 Aug 1996 12:04:22   Fred
// Fixed combo height bug
// 
//    Rev 1.0   14 Mar 1996 13:36:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:50   FRED
// Initial revision.
// 
//    Rev 1.3   13 Oct 1995 09:36:58   JAY
//  
// 
//    Rev 1.2   06 Oct 1995 15:23:22   FRED
// Win32 SendMessage() changes
// 
//    Rev 1.1   13 May 1995 11:10:06   FRED
// More warp preview stuffwarpshap.h
// 
//    Rev 1.0   02 May 1995 15:41:16   FRED
// Initial revision.
*/

// ownercb.cpp : implementation file
//      

#include "stdafx.h"
#include "ownercb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawComboBoxItem object
//
// One of these objects is attached to each item in the owner draw list box.

COwnerDrawComboBoxItem::COwnerDrawComboBoxItem()
{
}

COwnerDrawComboBoxItem::~COwnerDrawComboBoxItem()
{
}

void COwnerDrawComboBoxItem::DrawItem(COwnerDrawComboBox* pCombo, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

void COwnerDrawComboBoxItem::MeasureItem(COwnerDrawComboBox* pCombo, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
}

int COwnerDrawComboBoxItem::CompareItem(COwnerDrawComboBox* pCombo, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void COwnerDrawComboBoxItem::DeleteItem(COwnerDrawComboBox* pCombo, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
}

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawComboBox

COwnerDrawComboBox::COwnerDrawComboBox()
{
}

COwnerDrawComboBox::~COwnerDrawComboBox()
{
}

BEGIN_MESSAGE_MAP(COwnerDrawComboBox, CComboBox)
	//{{AFX_MSG_MAP(COwnerDrawComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawComboBox message handlers

void COwnerDrawComboBox::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if (pDrawItemStruct->itemID != (UINT)-1)
	{
		if ((COwnerDrawComboBoxItem*)pDrawItemStruct->itemData != NULL)
		{
			((COwnerDrawComboBoxItem*)pDrawItemStruct->itemData)->DrawItem(this, pDrawItemStruct);
		}
	}
}

void COwnerDrawComboBox::MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	pMeasureItemStruct->itemWidth = 0;
	pMeasureItemStruct->itemHeight = 0;
	
	// Get a pointer to the item to query.
		
	COwnerDrawComboBoxItem* pItem;
		
	if ((GetStyle() & CBS_OWNERDRAWVARIABLE) != 0)
	{
		pItem = (COwnerDrawComboBoxItem*)(pMeasureItemStruct->itemData);
	}
		
	else
	{
		pItem = (COwnerDrawComboBoxItem*)GetItemDataPtr(0);
	}
		
	// Ask the item for its dimensions.
			
	if (pItem != (COwnerDrawComboBoxItem*)(-1))
	{
		pItem->MeasureItem(this, pMeasureItemStruct);
	}
	else
	{
		// Set the item dimensions based on the current font.
		
		TEXTMETRIC tmFont;
		
		if (FontMetrics(tmFont))
		{
			CRect crClient;
			GetClientRect(crClient);
							
			pMeasureItemStruct->itemWidth = crClient.Width();
			pMeasureItemStruct->itemHeight = tmFont.tmHeight+2;
		}
	}
}

int COwnerDrawComboBox::CompareItem(LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return ((COwnerDrawComboBoxItem*)pCompareItemStruct->itemData1)->CompareItem(this, pCompareItemStruct);
}

void COwnerDrawComboBox::DeleteItem(LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	if (pDeleteItemStruct->itemData != NULL)
	{
		((COwnerDrawComboBoxItem*)pDeleteItemStruct->itemData)->DeleteItem(this, pDeleteItemStruct);
	}
}

void COwnerDrawComboBox::SizeItems(void)
{
	MEASUREITEMSTRUCT mis;
				
	mis.CtlType = ODT_COMBOBOX;
	mis.CtlID = GetDlgCtrlID();
	mis.itemWidth = 0;
	mis.itemHeight = 0;
				
	DWORD dwStyle = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
			
	mis.itemID = (UINT)-1;
	mis.itemData = 0;

	if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
	{
#ifdef WIN32
		SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
		SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
		
		// It seems that on some 16-bit systems, CB_SETITEMHEIGHT(-1, ...) sets the height
		// of the control, not the edit area. So, we have to check for this and reset the
		// height to a large value if this is the case.
		CRect crRect;
		GetWindowRect(crRect);
		if (crRect.Height() <= (int)mis.itemHeight)
		{
			SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight+6, 0));
		}
#endif
	}
	
	if ((dwStyle & CBS_OWNERDRAWFIXED) != 0)
	{
		mis.itemID = 0;
		mis.itemData = 0;
				
		if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
		{
#ifdef WIN32
			SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
			SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
		}
	}
			
	if ((dwStyle & CBS_OWNERDRAWVARIABLE) != 0)
	{
		UINT nCount = (UINT)SendMessage(CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				
		for (mis.itemID = 0; mis.itemID < nCount; mis.itemID++)
		{
			mis.itemData = SendMessage(CB_GETITEMDATA, (WPARAM)(mis.itemID), (LPARAM)0);
				
			if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
			{
#ifdef WIN32
				SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
				SendMessage(CB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
			}
		}
	}
}

BOOL COwnerDrawComboBox::FontMetrics(TEXTMETRIC& Metrics)
{
	BOOL fResult = FALSE;
	
	CClientDC cdcWnd(this);
					
	CFont* pcfWnd = GetFont();
				
	if (pcfWnd != NULL)
	{
		CFont* pcfOld = cdcWnd.SelectObject(pcfWnd);
					
		if (pcfOld != NULL)
		{
			if (cdcWnd.GetTextMetrics(&Metrics))
			{
				fResult = TRUE;
			}
						
			cdcWnd.SelectObject(pcfOld);
		}
	}
	
	return fResult;
}
