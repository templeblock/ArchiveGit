/*
// $Workfile: ORDERLB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/ORDERLB.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:56   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:43:54   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:46   johno
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
// $Log: /PM8/App/LICDLL/ORDERLB.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.1   05 Feb 1996 17:41:10   FRED
// Changes for upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:26   JAY
// Initial revision.
// 
//    Rev 1.15   01 Mar 1995 16:54:58   FRED
// 
//    Rev 1.14   24 Feb 1995 14:36:12   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.13   21 Jan 1995 10:54:58   FRED
// No change.
// 
//    Rev 1.12   20 Jan 1995 15:26:38   FRED
// New "free item marking" code.
// 
// 
//    Rev 1.11   20 Jan 1995 10:57:32   JAY
// Doesn't draw "Free" in red if the item is licensed.
// 
//    Rev 1.10   20 Jan 1995 10:49:22   JAY
// Touch-ups
// 
//    Rev 1.9   19 Jan 1995 14:20:34   JAY
// New free pictures!
// 
//    Rev 1.8   11 Jan 1995 16:26:18   FRED
//  
// 
//    Rev 1.7   10 Jan 1995 14:24:48   JAY
// Order state reflected back into order dialog.
// 
//    Rev 1.6   22 Dec 1994 10:41:26   FRED
// 
//    Rev 1.5   16 Dec 1994 12:52:18   JAY
// Changed SetTextColor to SetBkColor where needed.
// 
//    Rev 1.4   16 Dec 1994 09:53:00   FRED
//  
// 
//    Rev 1.3   15 Dec 1994 17:29:44   FRED
//  
// 
//    Rev 1.2   15 Dec 1994 17:12:32   FRED
//  
// 
//    Rev 1.1   13 Dec 1994 18:29:18   FRED
//  
// 
//    Rev 1.0   11 Dec 1994 11:01:58   FRED
// Initial revision.
*/

// instlb.cpp : implementation file
//

#include "stdafx.h"
#include "license.h"
#include "orderlb.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderListBoxItem

COrderListBoxItem::COrderListBoxItem(CProductItem* pItem, int nIndent /*=0*/) :
	CProductCheckListBoxItem(pItem, nIndent)
{
}

COrderListBoxItem::~COrderListBoxItem()
{
}

void COrderListBoxItem::PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CProductItem* pItem = Item();
	
	if (pItem != NULL)
	{
		CRect crListBox;
		pList->GetWindowRect(crListBox);
		
		CWnd* pDialog = pList->GetParent();
		
		CWnd* pPriceAlign = pDialog->GetDlgItem(IDC_PRICE_ALIGN);
		CRect crPriceAlign;
		pPriceAlign->GetWindowRect(crPriceAlign);
		
		CWnd* pExtensionAlign = pDialog->GetDlgItem(IDC_EXTENSION_ALIGN);
		CRect crExtensionAlign;
		pExtensionAlign->GetWindowRect(crExtensionAlign);
		
		CString csName = pItem->Name();
		
//		if (Indent() == 0)
//		{
//			csName += " *";
//		}

		int nNameAlign = crBounds.left;
		BOOL fFree = pItem->TestFlag(PRODUCT_ITEM_Free);
		LPCSTR szZero = fFree ? "Free" : "";

		CString csPrice;
		Util::PriceString(csPrice, pItem->Price(), TRUE, szZero);
		int nPriceAlign = crPriceAlign.left-crListBox.left-pDC->GetTextExtent(csPrice, csPrice.GetLength()).cx;
		
		CString csExtension = "";
		Util::PriceString(csExtension, pItem->Price(), TRUE, szZero);
		int nExtensionAlign = crExtensionAlign.left-crListBox.left-pDC->GetTextExtent(csExtension, csExtension.GetLength()).cx;
		
		COLORREF clOldForeground = pDC->SetTextColor(clForeground);
		COLORREF clOldBackground = pDC->SetBkColor(clBackground);
	
		pDC->TextOut(nNameAlign,  crBounds.top, csName,  csName.GetLength());

		if (fFree && !pItem->TestFlag(PRODUCT_ITEM_Licensed))
		{
		/* "Free" should be red. */
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		pDC->TextOut(nPriceAlign, crBounds.top, csPrice, csPrice.GetLength());
		
		if (Select())
		{
			pDC->TextOut(nExtensionAlign, crBounds.top, csExtension, csExtension.GetLength());
		}
		
		pDC->SetTextColor(clOldForeground);
		pDC->SetBkColor(clOldBackground);
	}
}

void COrderListBoxItem::Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

/////////////////////////////////////////////////////////////////////////////
// COrderListBox

COrderListBox::COrderListBox() :
	CProductCheckListBox(CHECK_LIST_BOX_DisableAutoMarkParent)
{
}

COrderListBox::~COrderListBox()
{
}

BEGIN_MESSAGE_MAP(COrderListBox, CProductCheckListBox)
	//{{AFX_MSG_MAP(COrderListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COrderListBox::AddProducts(CProductItems* pItems)
{
	AddItems(pItems, 0);
	AdjustSelections();
}

void COrderListBox::AddItems(CProductItems* pItems, int nIndent)
{
	int nCount = pItems->ItemCount();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = pItems->Item(nIndex);

		if (pItem != NULL)
		{
			COrderListBoxItem* pListBoxItem = new COrderListBoxItem(pItem, nIndent);

			if (pListBoxItem != NULL)
			{
				pListBoxItem->Mark(pItem->TestFlag(PRODUCT_ITEM_Marked));
				pListBoxItem->Select(pItem->TestFlag(PRODUCT_ITEM_Selected));

				AddString((LPCSTR)pListBoxItem);

				AddItems(pItem->Items(), nIndent+1);
			}
		}
	}
}

BOOL COrderListBox::Select(int nIndex, BOOL fMark, BOOL fDragging)
{
	CProductCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
	
	if (pListBoxItem != NULL)
	{
		if (!pListBoxItem->Disable())
		{
			CProductItem* pItem = pListBoxItem->Item();
			if (pItem->TestFlag(PRODUCT_ITEM_Free))
			{
			/* Attempting to change state of a free catagory. */
				if (!fDragging)
				{
					::AfxMessageBox("This collection contains free graphics.\n"
							 "It is automatically added to your first order.");
					return FALSE;
				}
				return TRUE;
			}
		}
	}

	return CProductCheckListBox::Select(nIndex, fMark, fDragging);
}

void COrderListBox::AdjustFree(void)
{
	int nCount;
	int nIndex;
	
	// Check if an item is marked that costs money... Only then do
	// we shower the user with free item(s)!
	
	BOOL fNonFreeMarked = FALSE;
	
	nCount = GetCount();

	if (nCount != LB_ERR)
	{
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CProductCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
			
			if ((pListBoxItem != NULL) && (pListBoxItem->Mark()))
			{
				CProductItem* pItem = pListBoxItem->Item();

				if ((pItem != NULL) && ((!(pItem->TestFlag(PRODUCT_ITEM_Free)))))
				{
					fNonFreeMarked = TRUE;
					break;
				}
			}
		}
		
		// Go through and mark or clear all the free items.
			
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CProductCheckListBoxItem* pListBoxItem = GetListBoxItem(nIndex);
				
			if (pListBoxItem != NULL)
			{
				CProductItem* pItem = pListBoxItem->Item();
	
				if ((pItem != NULL) && (pItem->TestFlag(PRODUCT_ITEM_Free)))
				{
					MarkItem(pListBoxItem, nIndex, fNonFreeMarked);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// COrderListBox message handlers

void COrderListBox::UpdateMarkedItems(void)
{
	int nCount = GetCount();

	if (nCount != LB_ERR)
	{
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			CProductCheckListBoxItem* pChild = GetListBoxItem(nIndex);

			if (!pChild->Disable())
			{
				CProductItem* pItem = pChild->Item();

				MarkItem(pChild, nIndex, pItem->TestFlag(PRODUCT_ITEM_Marked));
			}
		}

		AdjustFree();
		AdjustParents();
		AdjustSelections();
	}
}
