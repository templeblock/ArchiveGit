/*
// $Workfile: OWNERLB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/OWNERLB.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:34   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:58   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "ownerlb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBoxItem object
//
// One of these objects is attached to each item in the owner draw list box.

COwnerDrawListBoxItem::COwnerDrawListBoxItem()
{
}

COwnerDrawListBoxItem::~COwnerDrawListBoxItem()
{
}

void COwnerDrawListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

void COwnerDrawListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
}

int COwnerDrawListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void COwnerDrawListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
}


/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBox

WNDPROC COwnerDrawListBox::m_pfnSuperWndProc = NULL;

COwnerDrawListBox::COwnerDrawListBox()
{
}

COwnerDrawListBox::~COwnerDrawListBox()
{
}

BEGIN_MESSAGE_MAP(COwnerDrawListBox, CListBox)
	//{{AFX_MSG_MAP(COwnerDrawListBox)
	ON_WM_CHARTOITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int COwnerDrawListBox::Index(const CPoint& cpPoint)
{
	CRect crClient;
	GetClientRect(crClient);
	
	int nCount = GetCount();
	int nIndex = GetTopIndex();
	
	while (nIndex < nCount)
	{
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) != LB_ERR)
		{
			if (crItem.top >= crClient.bottom)
			{
				break;
			}
			
			if (cpPoint.y < crItem.top)
			{
				nIndex--;
				
				if (nIndex < 0)
				{
					nIndex = 0;
				}
				
				break;
			}
			
			if (cpPoint.y < crItem.bottom)
			{
				break;
			}
		}
		
		nIndex++;
	}

	if (nIndex == nCount)
	{
		// Past last item, return -1.
		nIndex = -1;
	}
	
	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBox message handlers

void COwnerDrawListBox::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if ((COwnerDrawListBoxItem*)pDrawItemStruct->itemData != NULL)
	{
		((COwnerDrawListBoxItem*)pDrawItemStruct->itemData)->DrawItem(this, pDrawItemStruct);
	}
}

void COwnerDrawListBox::MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	pMeasureItemStruct->itemWidth = 0;
	pMeasureItemStruct->itemHeight = 0;
	
	// Get a pointer to the item to query.
	
	COwnerDrawListBoxItem* pItem;
	
	if ((GetStyle() & LBS_OWNERDRAWVARIABLE) != 0)
	{
		pItem = (COwnerDrawListBoxItem*)(pMeasureItemStruct->itemData);
	}
	
	else
	{
		pItem = (COwnerDrawListBoxItem*)GetItemDataPtr(0);
	}
	
	// Ask the item for its dimensions.
		
	if (pItem != (COwnerDrawListBoxItem*)(-1))
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
			pMeasureItemStruct->itemHeight = tmFont.tmHeight;
		}
	}
}

int COwnerDrawListBox::CompareItem(LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return ((COwnerDrawListBoxItem*)pCompareItemStruct->itemData1)->CompareItem(this, pCompareItemStruct);
}

void COwnerDrawListBox::DeleteItem(LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	if (pDeleteItemStruct->itemData != 0)
	{
		((COwnerDrawListBoxItem*)pDeleteItemStruct->itemData)->DeleteItem(this, pDeleteItemStruct);
	}
}

void COwnerDrawListBox::SizeItems(void)
{
	MEASUREITEMSTRUCT mis;
				
	mis.CtlType = ODT_LISTBOX;
	mis.CtlID = GetDlgCtrlID();
	mis.itemWidth = 0;
	mis.itemHeight = 0;
				
	DWORD dwStyle = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);
			
	if ((dwStyle & LBS_OWNERDRAWFIXED) != 0)
	{
		mis.itemID = 0;
		mis.itemData = 0;
				
		if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
		{
#ifdef WIN32
			SendMessage(LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
			SendMessage(LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
		}
	}
			
	if ((dwStyle & LBS_OWNERDRAWVARIABLE) != 0)
	{
		UINT nCount = (UINT)SendMessage(LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
				
		for (mis.itemID = 0; mis.itemID < nCount; mis.itemID++)
		{
			mis.itemData = SendMessage(LB_GETITEMDATA, (WPARAM)(mis.itemID), (LPARAM)0);
				
			if (GetParent()->SendMessage(WM_MEASUREITEM, (WPARAM)(mis.CtlID), (LPARAM)(&mis)) != 0)
			{
#ifdef WIN32
				SendMessage(LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), (LPARAM)(mis.itemHeight));
#else
				SendMessage(LB_SETITEMHEIGHT, (WPARAM)(mis.itemID), MAKELPARAM(mis.itemHeight, 0));
#endif
			}
		}
	}
}

int COwnerDrawListBox::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex)
{
	// NOTE: WM_CHARTOITEM is sent to the parent of the list box, and MFC does
	// not reflect it down to the control as it does for DrawItem(), etc. So this
	// code will only get called if the parent passes WM_CHARTOITEM on to us.
	
	// Default is do nothing.
	return -2;
}

BOOL COwnerDrawListBox::FontMetrics(TEXTMETRIC& Metrics)
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
