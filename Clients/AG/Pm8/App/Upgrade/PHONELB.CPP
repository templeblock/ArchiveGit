/*
// $Workfile: PHONELB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PHONELB.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:26   Fred
// Initial revision.
// 
//    Rev 1.1   09 Jul 1997 09:55:22   Fred
// Moved strings to resource file
// 
//    Rev 1.0   08 Sep 1996 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:18   JAY
// Initial revision.
// 
//    Rev 1.12   27 Feb 1995 12:47:44   FRED
// Does not add blank lines for items with no
// collection names.
// 
// 
//    Rev 1.11   24 Feb 1995 14:35:02   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.10   21 Jan 1995 10:54:52   FRED
// Changes to make free items listed at
// the end of the phone order list box.
// 
// 
//    Rev 1.9   20 Jan 1995 10:49:10   JAY
// Touch-ups
// 
//    Rev 1.8   19 Jan 1995 14:20:26   JAY
// New free pictures!
// 
//    Rev 1.7   17 Jan 1995 16:11:12   FRED
// Better fix for flag changing bug. The problem was
// the signature for the virtual function Select()
// was wrong.
// 
// 
//    Rev 1.6   17 Jan 1995 15:44:12   FRED
// Fixed bug where item flags could be changed
// by mouse clicks.
// 
// 
//    Rev 1.5   16 Dec 1994 17:09:28   FRED
//  
// 
//    Rev 1.4   16 Dec 1994 12:52:50   JAY
// Changed SetTextColor to SetBkColor where required.
// 
//    Rev 1.3   15 Dec 1994 17:29:48   FRED
//  
// 
//    Rev 1.2   15 Dec 1994 13:09:28   FRED
//  
// 
//    Rev 1.1   14 Dec 1994 17:19:52   FRED
//  
// 
//    Rev 1.0   13 Dec 1994 18:30:54   FRED
// Initial revision.
*/

// phonelb.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "phonelb.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhoneListBoxItem

CPhoneListBoxItem::CPhoneListBoxItem(CProductItem* pItem, int nIndent /*=0*/) :
	CProductCheckListBoxItem(pItem, nIndent)
{
}

CPhoneListBoxItem::~CPhoneListBoxItem()
{
}

void CPhoneListBoxItem::ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem)
{
	CProductCheckListBoxItem::ComputeBounds(pList, pDC, pDrawItemStruct, crCheckBox, crItem);
	
	if (!crCheckBox.IsRectEmpty())
	{
		if (!crItem.IsRectEmpty())
		{
			crItem.left = crCheckBox.left;
		}
		
		crCheckBox.SetRectEmpty();
	}
}

void CPhoneListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CProductItem* pItem = Item();
	
	if (pItem != NULL)
	{
		if (!pItem->TestFlag(PRODUCT_ITEM_Free))
		{
			CRect crListBox;
			pList->GetWindowRect(crListBox);
		
			CWnd* pDialog = pList->GetParent();
		
			CWnd* pPriceAlign = pDialog->GetDlgItem(IDC_PRICE_ALIGN);
			CRect crPriceAlign;
			pPriceAlign->GetWindowRect(crPriceAlign);
		
			CString csName;
			CString csPrice;
			BOOL fFree = pItem->TestFlag(PRODUCT_ITEM_Free);
			CString csZero = "";
			if (fFree)
			{
				Util::SafeLoadString(IDS_FREE, csZero);
			}
			
			if (pItem->TestFlag(PRODUCT_ITEM_Marked))
			{
				csName = pItem->Name();
				Util::PriceString(csPrice, pItem->Price(), TRUE, csZero);
			}
			else
			{
				csName = pItem->CollectionName();
				csPrice = "";
			}
				
			int nNameAlign = crBounds.left;
			int nPriceAlign = crPriceAlign.left-crListBox.left-pDC->GetTextExtent(csPrice, csPrice.GetLength()).cx;
			
			COLORREF clOldForeground = pDC->SetTextColor(clForeground);
			COLORREF clOldBackground = pDC->SetBkColor(clBackground);
		
			pDC->TextOut(nNameAlign,  crBounds.top, csName,  csName.GetLength());
			if (fFree)
			{
				pDC->SetTextColor(RGB(255, 0, 0));
			}
			pDC->TextOut(nPriceAlign, crBounds.top, csPrice, csPrice.GetLength());
			
			pDC->SetTextColor(clOldForeground);
			pDC->SetBkColor(clOldBackground);
		}
		else
		{
			COLORREF clOldForeground = pDC->SetTextColor(RGB(255, 0, 0));
			
			CString csText;
			Util::SafeLoadString(IDS_INCLUDED_FREE, csText);
			
			pDC->TextOut(crBounds.left,  crBounds.top, csText,  csText.GetLength());
			
			pDC->SetTextColor(clOldForeground);
		}
	}
}

void CPhoneListBoxItem::Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPhoneListBox

CPhoneListBox::CPhoneListBox()
{
}

CPhoneListBox::~CPhoneListBox()
{
}

BEGIN_MESSAGE_MAP(CPhoneListBox, CProductCheckListBox)
	//{{AFX_MSG_MAP(CPhoneListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPhoneListBox::AddProducts(CProductItems* pItems)
{
	AddItems(pItems, 0);
	AddFreeItems(pItems, TRUE);
	AdjustSelections();
}

void CPhoneListBox::AddItems(CProductItems* pItems, int nIndent)
{
	int nCount = pItems->ItemCount();
	int nIndex;
	
	for (nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = pItems->Item(nIndex);
		
		if (pItem != NULL)
		{
			if (!pItem->IsLicensed())
			{
				if (!pItem->TestFlag(PRODUCT_ITEM_Free))
				{
					if ((pItem->TestFlag(PRODUCT_ITEM_Marked))
					 || (pItem->Items()->CountProductFlags(PRODUCT_ITEM_Marked, CProductItems::NotLicensedAndNotFree) > 0))
					{
						CString csName;
						
						if (pItem->TestFlag(PRODUCT_ITEM_Marked))
						{
							csName = pItem->Name();
						}
						else
						{
							csName = pItem->CollectionName();
						}
						
						if (!csName.IsEmpty())
						{
							CPhoneListBoxItem* pListBoxItem = new CPhoneListBoxItem(pItem, nIndent);
						
							if (pListBoxItem != NULL)
							{
								pListBoxItem->Mark(pItem->TestFlag(PRODUCT_ITEM_Marked));
								pListBoxItem->Select(pItem->TestFlag(PRODUCT_ITEM_Selected));
								
								AddString((LPCSTR)pListBoxItem);
								
								if (!pItem->TestFlag(PRODUCT_ITEM_Marked))
								{
									AddItems(pItem->Items(), nIndent+1);
								}
							}
						}
						else
						{
							if (!pItem->TestFlag(PRODUCT_ITEM_Marked))
							{
								AddItems(pItem->Items(), nIndent);
							}
						}
					}
				}
			}
		}
	}
}

void CPhoneListBox::AddFreeItems(CProductItems* pItems, BOOL fFirst)
{
	int nCount = pItems->ItemCount();
	
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = pItems->Item(nIndex);
		
		if (pItem != NULL)
		{
			if (!pItem->IsLicensed())
			{
				if ((pItem->TestFlag(PRODUCT_ITEM_Marked))
				 || (pItem->Items()->CountProductFlags(PRODUCT_ITEM_Marked, CProductItems::NotLicensed) > 0))
				{
					if (pItem->TestFlag(PRODUCT_ITEM_Free))
					{
						CPhoneListBoxItem* pListBoxItem = new CPhoneListBoxItem(pItem, 0);
						
						if (pListBoxItem != NULL)
						{
							pListBoxItem->Mark(pItem->TestFlag(PRODUCT_ITEM_Marked));
							pListBoxItem->Select(pItem->TestFlag(PRODUCT_ITEM_Selected));

							if (fFirst)
							{
								CPhoneListBoxItem* pBlankListBoxItem = new CPhoneListBoxItem(NULL, 0);
								
								if (pBlankListBoxItem != NULL)
								{
									AddString((LPCSTR)pBlankListBoxItem);
								}
								
								fFirst = FALSE;
							}
															
							AddString((LPCSTR)pListBoxItem);
						}
					}
					else
					{
						AddFreeItems(pItem->Items(), fFirst);
					}
				}
			}
		}
	}
}

BOOL CPhoneListBox::Select(int nIndex, BOOL fMark, BOOL fDragging)
{
	// Do not select anything, keep flags the same.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPhoneListBox message handlers
