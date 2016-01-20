/*
// $Workfile: PROD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PROD.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 6     10/13/98 11:44a Mwilson
// Changes for craft upgrade
// 
// 5     10/13/98 11:42a Mwilson
// added craft upgrade
// 
// 4     9/16/98 2:42p Jayn
// 7.0 changes.
// 
// 3     8/21/98 12:32p Mwilson
// changed price to $20
// 
// 2     7/09/98 10:51a Mwilson
// added upgrade info for AG print standard
// 
//    Rev 1.0   14 Aug 1997 15:42:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:28   Fred
// Initial revision.
// 
//    Rev 1.3   09 Jul 1997 09:55:24   Fred
// Moved strings to resource file
// 
//    Rev 1.2   12 Jun 1997 16:30:42   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.1   08 Sep 1996 09:51:36   Fred
// PPS changed to P3S for PMW 3.0
// 
//    Rev 1.0   08 Sep 1996 09:40:36   Fred
// Initial revision.
// 
//    Rev 1.12   25 Apr 1996 12:47:58   FRED
// Upgrades from demo are $49.95.
// 
// 
//    Rev 1.11   24 Apr 1996 16:42:36   JAY
// Now gets PPN from entry. Demo handling.
// 
//    Rev 1.10   19 Apr 1996 09:37:42   JAY
// CU2->CU3
// 
//    Rev 1.9   20 Mar 1996 08:55:26   FRED
// Back to normal BP PPN.
// 
// 
//    Rev 1.8   18 Mar 1996 14:59:14   JAY
// PPN for Office Max BP.
// 
//    Rev 1.7   08 Mar 1996 08:23:12   JAY
// 
//    Rev 1.6   05 Mar 1996 18:09:06   JAY
// New longer offer code
// 
//    Rev 1.5   01 Mar 1996 12:57:08   FRED
// Changed PCD to PPS. In both cases: CL --> PS and BP --> PS, the
// product code will be PPS. The offer codes and prices will be different
// DU1-CLPPN/$29.95 and DU1-BPPPN/$19.95.
// 
// 
//    Rev 1.4   20 Feb 1996 16:35:14   FRED
// CL->PS is $29.95, BP->PS is $19.95
// 
//    Rev 1.3   14 Feb 1996 22:27:08   FRED
// CL to PS costs $19.95
// 
//    Rev 1.2   12 Feb 1996 13:53:48   JAY
// Added product level param for correct product
// 
//    Rev 1.1   05 Feb 1996 17:41:04   FRED
// Changes for upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:24   JAY
// Initial revision.
// 
//    Rev 1.30   28 Feb 1995 14:12:48   FRED
// Eliminated annoying TRACE.
// 
// 
//    Rev 1.29   28 Feb 1995 09:15:24   JAY
// Added NotLicensedAndNotFree to CProductItem.
// 
//    Rev 1.28   27 Feb 1995 13:33:14   FRED
// Reads real product string.
// 
// 
//    Rev 1.27   27 Feb 1995 11:13:08   FRED
// More multi-level object stuff. Added type flags to CProductItem
// 
//    Rev 1.26   24 Feb 1995 17:19:18   FRED
// Added PF1 and CDL products
// 
//    Rev 1.25   24 Feb 1995 14:36:02   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.24   21 Jan 1995 10:54:54   FRED
// Changes to make free items listed at
// the end of the phone order list box.
// 
// 
//    Rev 1.23   17 Jan 1995 13:23:32   JAY
// Fixed a processing fee bug.
// 
//    Rev 1.22   16 Jan 1995 16:01:20   FRED
// Added ASSERT to catch failures when debugging.
// 
// 
//    Rev 1.21   16 Jan 1995 15:53:44   FRED
// Uses CATALOG.INF instead of OFFER.INI.
// 
// 
//    Rev 1.20   10 Jan 1995 10:49:02   FRED
// Does not use "grep ".
// 
// 
//    Rev 1.19   04 Jan 1995 11:29:40   JAY
// Selective installation.
// 
//    Rev 1.18   21 Dec 1994 14:57:50   JAY
//  
// 
//    Rev 1.17   16 Dec 1994 17:09:46   FRED
//  
// 
//    Rev 1.16   15 Dec 1994 13:11:24   JAY
//  
// 
//    Rev 1.15   15 Dec 1994 11:07:02   FRED
// 
// 
//    Rev 1.14   15 Dec 1994 11:05:00   FRED
// Fixed bug in CProductItem::IsLicensed().
// 
// 
//    Rev 1.13   15 Dec 1994 09:49:10   FRED
// Added CProductItem::IsLicensed().
// 
// 
//    Rev 1.12   15 Dec 1994 09:04:36   JAY
//  
// 
//    Rev 1.11   14 Dec 1994 15:26:20   FRED
//  
// 
//    Rev 1.10   14 Dec 1994 11:33:50   FRED
//  
// 
//    Rev 1.9   13 Dec 1994 18:28:52   FRED
//  
// 
//    Rev 1.8   11 Dec 1994 10:57:08   FRED
//  
// 
//    Rev 1.7   06 Dec 1994 10:40:20   FRED
//  
// 
//    Rev 1.6   05 Dec 1994 17:02:14   JAY
// Added InstallSelect() and InstallSelected().
// 
//    Rev 1.5   05 Dec 1994 16:36:02   FRED
// 
//    Rev 1.4   05 Dec 1994 09:43:58   JAY
// CProductForSale is now derived from CObject so we can do dynamic creation of
// derived classes.
// 
//    Rev 1.3   02 Dec 1994 14:51:28   FRED
//  
// 
//    Rev 1.2   02 Dec 1994 11:45:02   JAY
//  
// 
//    Rev 1.1   02 Dec 1994 08:37:54   FRED
//  
// 
//    Rev 1.0   01 Dec 1994 14:43:56   FRED
// Initial revision.
*/

// prod.cpp : Defines the base class representing a product.
//

#include "stdafx.h"

#include "upgrade.h"
#include "util.h"
#include "prod.h"
#include "instdat.h"

////////////////////////////////////////////////////////////////////////////
// CProductsInformation

#define	OFFER_CODE_BUFFER_SIZE						(64)
#define	PHONE_NUMBER_BUFFER_SIZE					(64)
#define	FAX_NUMBER_BUFFER_SIZE						(64)
#define	DEPARTMENT_NAME_BUFFER_SIZE				(128)

COfferInformation::COfferInformation()
{
	m_csOfferCode.Empty();
	m_fInitialized = FALSE;
}

COfferInformation::~COfferInformation()
{
}

void COfferInformation::Init(const CString& csDataDirectory, int nProductLevel, DWORD dwPPN)
{
	if (!m_fInitialized)
	{
		m_lProcessingFee = 295;

		CProductItem* pProduct = NULL;

		// Init the offer code.
		char cbCode[40];
		sprintf(cbCode, "CU5-%06ld", dwPPN);
		m_csOfferCode = cbCode;

		CString csUpgradeProductName;
		Util::SafeLoadString(IDS_UPGRADE_PMW, csUpgradeProductName);

		switch (nProductLevel)
		{
			case 1:			// Demo
			case 5:			// UpgradableDemo
			case 3:			// Bonus Pack
			{
				break;
			}
			case 2:			// Classic
			{
			/*
			// Create our single PS Upgrade product.
			*/
				pProduct = new CProductItem("P7S",
													 csUpgradeProductName,
													 "",
													 "",
													 995,
													 PRODUCT_TYPE_Software);
				break;
			}
			case 150:			// American Greetings Print Standard
			{
			/*
			// Create our single PS Upgrade product.
			*/
				Util::SafeLoadString(IDS_UPGRADE_AG_PRINT, csUpgradeProductName);
				pProduct = new CProductItem("AGP",
													 csUpgradeProductName,
													 "",
													 "",
													 2000,
													 PRODUCT_TYPE_Software);
				break;
			}
			case 152:			// American Greetings Craft Standard
			{
			/*
			// Create our single PS Upgrade product.
			*/
				Util::SafeLoadString(IDS_UPGRADE_AG_CRAFT, csUpgradeProductName);
				pProduct = new CProductItem("AGC",
													 csUpgradeProductName,
													 "",
													 "",
													 2000,
													 PRODUCT_TYPE_Software);
				break;
			}
			default:
			{
				break;
			}
		}

		if (pProduct != NULL)
		{
			pProduct->SetFlag(PRODUCT_ITEM_Marked|PRODUCT_ITEM_Selected);
			Add(pProduct);
			m_fInitialized = TRUE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// CProductItems

CProductItems::CProductItems()
{
}

CProductItems::~CProductItems()
{
	int nCount = ItemCount();
	
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		delete Item(nIndex);
	}
}

void CProductItems::ClearProductFlags(DWORD dwFlags /*=PRODUCT_FLAGS_Transient*/)
{
	int nCount = ItemCount();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = Item(nIndex);

		if (pItem != NULL)
		{
			pItem->ClearFlag(dwFlags);
			pItem->Items()->ClearProductFlags(dwFlags);
		}
	}
}

int CProductItems::CountProductFlags(DWORD dwFlags /*=0xffffffffL*/, CProductItems::PPRODUCTITEMSTESTFN pTestFn /*=NULL*/)
{
	int nResult = 0;
	
	int nCount = ItemCount();
	
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = Item(nIndex);
		
		if (pItem != NULL)
		{
			if ((pTestFn == NULL) || ((this->*pTestFn)(pItem)))
			{
				if (pItem->TestFlag(dwFlags))
				{
					nResult++;
				}
			}
			
			nResult += pItem->Items()->CountProductFlags(dwFlags, pTestFn);
		}
	}
	
	return nResult;
}

BOOL CProductItems::NotLicensed(CProductItem* pItem)
{
	return !(pItem->IsLicensed());
}

BOOL CProductItems::NotLicensedAndNotFree(CProductItem* pItem)
{
	return pItem->NotLicensedAndNotFree();
}

BOOL CProductItems::UpdateLicenseFlags(const CInstallationData* pData)
{
	BOOL fSomethingLicensed = FALSE;
	int nCount = ItemCount();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = Item(nIndex);

		if (pItem != NULL)
		{
			if (pItem->UpdateLicenseFlags(pData))
			{
				fSomethingLicensed = TRUE;
			}
		}
	}
	return fSomethingLicensed;
}

/*
// Lookup an item by product code.
*/

CProductItem* CProductItems::LookupItem(LPCSTR pszCode)
{
	CProductItem* pFoundItem = NULL;
	int nItems = ItemCount();
	for (int nItem = 0; nItem < nItems; nItem++)
	{
		CProductItem* pItem = Item(nItem);
		if (pItem != NULL && (pFoundItem = pItem->LookupItem(pszCode)) != NULL)
		{
			break;
		}
	}
	return pFoundItem;
}

////////////////////////////////////////////////////////////////////////////
// CProductItem

IMPLEMENT_DYNCREATE(CProductItem, CObject)

CProductItem::CProductItem()
{
	m_dwFlags = 0;
	m_csCode.Empty();
	m_csName.Empty();
	m_csCollectionCode.Empty();
	m_csCollectionName.Empty();
	m_lPrice = 0;
	m_lType = 0;
	m_nPieces = 0;
	m_pCategory = NULL;
}

CProductItem::CProductItem(LPCSTR pszCode, LPCSTR pszName, LPCSTR pszCollectionCode, LPCSTR pszCollectionName, long lPrice, unsigned long lType)
{
	m_dwFlags = 0;
	m_csCode = pszCode;
	m_csName = pszName;
	m_csCollectionCode = pszCollectionCode;
	m_csCollectionName = pszCollectionName;
	m_lPrice = lPrice;
	m_lType = lType;
	m_nPieces = 0;
	m_pCategory = NULL;
}

CProductItem::~CProductItem()
{
}

int CProductItem::Install(CInstallInfo& Info)
{
	TRACE("Installing %s\n", (LPCSTR)m_csCode);
	return FALSE;
}

BOOL CProductItem::UpdateLicenseFlags(const CInstallationData* pData)
{
	BOOL fLicensed = pData->ProductIsLicensed(Code());

	if (fLicensed)
	{
		SetFlag(PRODUCT_ITEM_Licensed);
	}
	else
	{
		ClearFlag(PRODUCT_ITEM_Licensed);
	}

	if (Items()->UpdateLicenseFlags(pData))
	{
		fLicensed = TRUE;
	}
	return fLicensed;
}

BOOL CProductItem::IsLicensed(void)
{
	// Check if licensed flag is set. If so, then it's licensed.
	
	if (TestFlag(PRODUCT_ITEM_Licensed))
	{
		return TRUE;
	}
	
	// The licensed flag is not set, but the product is still defined
	// as licensed if all of its sub items are licensed.
	
	int nCount = Items()->ItemCount();
	
	if (nCount == 0)
	{
		return FALSE;
	}
	
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = Items()->Item(nIndex);
		
		if (pItem != NULL)
		{
			if (!pItem->IsLicensed())
			{
				return FALSE;
			}
		}
	}
	
	return TRUE;	
}

/*
// Say if an item is not licensed and not free.
*/

BOOL CProductItem::NotLicensedAndNotFree(void)
{
	return !IsLicensed() && !TestFlag(PRODUCT_ITEM_Free);
}

/*
// Lookup an item by code.
*/

CProductItem* CProductItem::LookupItem(LPCSTR pszCode)
{
	if (pszCode == Code())
	{
	/* Me! It's me! */
		return this;
	}
	else
	{
	/* Go down the road a ways... */
		return Items()->LookupItem(pszCode);
	}
}

/*
// Return how many pieces are in this product item.
*/

int CProductItem::Pieces(void) const
{
	return m_nPieces;
}

/*
// Set the number of pieces in this product item.
*/

void CProductItem::Pieces(int nPieces)
{
	m_nPieces = nPieces;
	m_cbaSelectedPieces.SetSize(nPieces);
}

/*
// Is a piece selected?
*/

BOOL CProductItem::PieceIsSelected(int nIndex) const
{
	if (nIndex >= m_cbaSelectedPieces.GetSize())
	{
		return FALSE;
	}
	return (BOOL)m_cbaSelectedPieces.GetAt(nIndex);
}

/*
// Select a piece.
*/

void CProductItem::SelectPiece(int nIndex, BOOL fSelect /*=TRUE*/)
{
	m_cbaSelectedPieces.SetAtGrow(nIndex, (BYTE)fSelect);
}

/*
// Set the select state of all pieces to some value.
*/

void CProductItem::SelectAllPieces(BOOL fSelect /*=TRUE*/)
{
	int nPieces = Pieces();

	for (int nPiece = 0; nPiece < nPieces; nPiece++)
	{
		SelectPiece(nPiece, fSelect);
	}
}

/*
// Return how many pieces are selected.
*/

int CProductItem::SelectedPieces(void) const
{
	int nCount = 0;
	int nPieces = Pieces();

	for (int nPiece = 0; nPiece < nPieces; nPiece++)
	{
		if (PieceIsSelected(nPiece))
		{
			nCount++;
		}
	}
	return nCount;
}

void CProductItem::UserChoosePieces(CInstallInfo* pInfo)
{
// Default does nothing.
}

void CProductItem::Category(CSimpleCategory* pCategory)
{
	m_pCategory = pCategory;
}

CSimpleCategory* CProductItem::Category(void) const
{
	return m_pCategory;
}
