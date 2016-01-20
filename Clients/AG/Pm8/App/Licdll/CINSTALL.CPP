/*
// Installation
//
// $Workfile: CINSTALL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CINSTALL.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:58   Fred
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
// $Log: /PM8/App/LICDLL/CINSTALL.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:22   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:22   JAY
// Initial revision.
// 
//    Rev 1.39   28 Nov 1995 17:14:04   JAY
// 
//    Rev 1.38   28 Feb 1995 09:17:48   JAY
// Fixed the LRC product code generation.
// 
//    Rev 1.37   27 Feb 1995 12:50:08   JAY
// Only bumps sequence if something gets licensed.
// 
//    Rev 1.36   27 Feb 1995 11:13:02   FRED
// More multi-level object stuff. Added type flags to CProductItem
// 
//    Rev 1.35   27 Feb 1995 09:01:14   FRED
// Checks for "BigRectType".
// 
// 
//    Rev 1.34   24 Feb 1995 17:19:32   FRED
// Added PF1 and CDL products
// 
//    Rev 1.33   24 Feb 1995 14:34:54   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.32   24 Feb 1995 10:47:36   JAY
// Now bumps sequence after products actually get licensed.
// 
//    Rev 1.31   23 Jan 1995 11:38:12   JAY
// Included "CAR" in graphics products.
// 
//    Rev 1.30   19 Jan 1995 15:26:52   JAY
// Free flag
// 
//    Rev 1.29   19 Jan 1995 14:20:52   JAY
// New free pictures!
// 
//    Rev 1.28   11 Jan 1995 11:39:40   JAY
// New
// 
//    Rev 1.27   04 Jan 1995 11:28:58   JAY
// Selective installation.
// 
//    Rev 1.26   21 Dec 1994 17:13:48   JAY
//  
// 
//    Rev 1.25   21 Dec 1994 14:57:42   JAY
//  
// 
//    Rev 1.24   16 Dec 1994 17:22:06   JAY
// Only licenses a product which is not already licensed.
// 
//    Rev 1.23   16 Dec 1994 12:00:18   JAY
// Shows space required in product list.
// 
//    Rev 1.22   15 Dec 1994 15:01:04   JAY
// Case.
// 
//    Rev 1.21   15 Dec 1994 14:46:14   JAY
// Selective install
// 
//    Rev 1.20   15 Dec 1994 13:11:20   JAY
//  
// 
//    Rev 1.19   15 Dec 1994 09:04:44   JAY
//  
// 
//    Rev 1.18   14 Dec 1994 17:22:04   JAY
// New product scheme
// 
//    Rev 1.17   14 Dec 1994 13:55:38   JAY
// 
//    Rev 1.16   13 Dec 1994 18:29:04   FRED
//  
// 
//    Rev 1.15   08 Dec 1994 10:10:04   JAY
// Changed product encoding.
// 
//    Rev 1.14   06 Dec 1994 14:34:26   JAY
// Added Install congrats dialog.
// 
//    Rev 1.13   06 Dec 1994 10:43:22   JAY
//  
// 
//    Rev 1.12   05 Dec 1994 17:02:32   JAY
// Added InstallSelect() and InstallSelected() to CProductForSale.
// 
//    Rev 1.11   05 Dec 1994 15:28:04   JAY
// Now adds subproducts of container products.
// 
//    Rev 1.10   05 Dec 1994 14:41:54   JAY
// Sequence must now be bumped by hand.
// 
//    Rev 1.9   05 Dec 1994 11:38:26   JAY
//  
// 
//    Rev 1.8   02 Dec 1994 11:45:00   JAY
//  
// 
//    Rev 1.7   17 Nov 1994 14:30:24   JAY
//  
// 
//    Rev 1.6   11 Nov 1994 13:01:58   JAY
// Now disables and re-enables the main window when installing.
// 
//    Rev 1.5   10 Nov 1994 18:31:58   FRED
//  
// 
//    Rev 1.4   10 Nov 1994 17:44:48   JAY
//  
// 
//    Rev 1.3   10 Nov 1994 15:03:32   JAY
//  
// 
//    Rev 1.2   10 Nov 1994 15:00:26   JAY
//  
// 
//    Rev 1.1   10 Nov 1994 13:09:44   JAY
// Expanded the functionality of CInstallInfo.
// 
//    Rev 1.0   08 Nov 1994 15:53:06   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "license.h"

#include "cinstall.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallInfo

CInstallInfo::CInstallInfo()
{
	// Defaults for these pointers are the ones in the app.

	m_pInstallationData = GetApp()->InstallationData();
	m_pOffer = GetApp()->Offer();
}

CInstallInfo::~CInstallInfo()
{
}

/*
// License the products which match this LAC.
// As an added benefit, all licensed products will be selected in the
// "product information" so we can see what happened.
*/

void CInstallInfo::LicenseProducts(const CString& csLAC)
{
	ASSERT(m_pInstallationData != NULL);
	char LRC[20];

	m_pInstallationData->LACtoLRC(csLAC, LRC);

/*
// For this round, we will turn off all products before turning on the ones
// in the passed code(s).
*/

	ASSERT(m_pOffer != NULL);

	m_pOffer->ClearProductFlags();

/* Down to recursive layer. */

	if (LicenseProducts(m_pOffer, LRC))
	{
		m_pInstallationData->BumpSequence();		// New LRC next time.
	}
/* Make sure we're up-to-date. */
	m_pInstallationData->Put();
}

/*
// Recursive layer to license products ordered in LICENSE.DAT.
*/

BOOL CInstallInfo::LicenseProducts(CProductItems* pItems, const CString& csLRC)
{
	BOOL fLicensedSomething = FALSE;

/*
// Run through all the products in the offer and license and select those
// which match the computed LRC.
*/

//	int nProducts = m_pOffer->ProductCount();

	int nItems = pItems->ItemCount();

	for (int nItem = 0; nItem < nItems; nItem++)
	{
		CProductItem* pItem = pItems->Item(nItem);
		CProductItems* pSubItems = pItem->Items();
		int nSubItems = pSubItems->ItemCount();

		if (nSubItems == 0)
		{
			if (m_pInstallationData->LicenseProduct(pItem->Code(), csLRC))
			{
				pItem->SetFlag(PRODUCT_ITEM_Install | PRODUCT_ITEM_Licensed);
				fLicensedSomething = TRUE;
			}
		}
		else
		{
		/* Try the subitems. */
			if (LicenseProducts(pSubItems, csLRC))
			{
				fLicensedSomething = TRUE;
			}
		}
	}
	return fLicensedSomething;
}

/*
// Sum the bytes in a string, wrapping the carry.
*/

static BYTE SumString(LPCSTR pStr)
{
	BYTE bSum = 0;

	BYTE b;

	while ((b = (BYTE)*pStr++) != '\0')
	{
		// Simulate byte rotate left.
		int bCarry = (bSum & 0x80) ? 1 : 0;
		bSum = (bSum << 1) + bCarry;

		// Add in the new code.
		bSum += b;
	}
	return bSum;
}

BYTE ProductSum(CProductItems* pItems)
{
	BYTE bSum = 0;

	int nCount = pItems->ItemCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CProductItem* pItem = pItems->Item(nIndex);

		if (pItem->TestFlag(PRODUCT_ITEM_Marked))
		{
		/* Add in this code. */
			if (pItem->NotLicensedAndNotFree())
			{
				bSum += SumString(pItem->Code());
			}
		}
		else
		{
			CProductItems* pSubItems = pItem->Items();

			if (pItem->CollectionCode() == "")
			{
			/* No collection code. Get the sum for any children. */
				bSum += ProductSum(pSubItems);
			}
			else
			{
			/*
			// Count if this guy has any subitems selected.
			*/

				int nSubSelCount = 0;
				int nSubCount = pSubItems->ItemCount();

				for (int nSubIndex = 0; nSubIndex < nSubCount; nSubIndex++)
				{
					CProductItem* pSubItem = pSubItems->Item(nSubIndex);
					if (pSubItem->TestFlag(PRODUCT_ITEM_Marked))
					{
						if (pSubItem->NotLicensedAndNotFree())
						{
							nSubSelCount++;
						}
					}
				}

			/* If any sub items are selected, then we do something special. */

				if (nSubSelCount != 0)
				{
					bSum += SumString(pItem->CollectionCode());
					bSum += nSubSelCount;
				}
			}
		}
	}

	return bSum;
}

/*
// Generate a License Request Code for the selected products.
*/

void CInstallInfo::GetLRCFromProducts(CString& csLRC)
{
	/*
	// Generate the LRC from the code pieces.
	*/

	ASSERT(m_pOffer != NULL);

	BYTE bSum = ProductSum(m_pOffer);

	char cProductEncoding = (bSum % 26) + 'A';

	char lrcBuffer[40];

	m_pInstallationData->GetLicenseRequestCode(cProductEncoding, lrcBuffer);

	csLRC = lrcBuffer;
}

/*
// Underlying code to handled licensing of products.
*/

void CInstallInfo::DoLicenseProducts(LPCSTR pszLRC)
{
	ASSERT(m_pOffer != NULL);

/* Clear all the install flags. */

	m_pOffer->ClearProductFlags(PRODUCT_ITEM_Install);
	
	LicenseProductItems(m_pOffer, pszLRC);
	
// Record our changes.
	if (pszLRC == NULL)
	{
	// Licensing now. Bump the sequence.
		m_pInstallationData->BumpSequence();		// New LRC next time.
	}
	m_pInstallationData->Put();
}

/*
// License all selected products.
// This is a one-shot deal (like for phone orders where we don't need to
// remember the codes for later). As such, we don't bother to record the
// LRC with the products. They simply become licensed.
*/

void CInstallInfo::LicenseSelectedProducts(void)
{
	DoLicenseProducts(NULL);
}

void CInstallInfo::LicenseProductItems(CProductItems* pItems, LPCSTR pszLRC)
{
	int nItems = pItems->ItemCount();
	
	for (int nItem = 0; nItem < nItems; nItem++)
	{
		CProductItem* pItem = pItems->Item(nItem);
		
		if (pItem->Items()->ItemCount() == 0)
		{
			if (pItem->TestFlag(PRODUCT_ITEM_Marked) && !pItem->TestFlag(PRODUCT_ITEM_Licensed))
			{
				// Add this product.
				CProductInfo* pInfo;
	
				if ((pInfo = m_pInstallationData->AddProduct(pItem->Code())) != NULL)
				{
					// Mark the product as licensed.
					if (pszLRC == NULL)
					{
						// License it. We don't need the access code.
						pInfo->License();
						pItem->SetFlag(PRODUCT_ITEM_Install | PRODUCT_ITEM_Licensed);
					}
					else
					{
						// Add the license code so we can license it later.
						pInfo->AddLicenseCode(pszLRC);
					}
				}
			}
		}
		else
		{
			LicenseProductItems(pItem->Items(), pszLRC);
		}
	}
}
		
/*
// "Order" all selected products.
// This will store entries in the installation data for the selected products.
// The LRC will be remembered for later unlocking via LicenseProducts().
// This is a two-pass deal (like for a mail order where we need to remember
// the codes for later).
*/

void CInstallInfo::OrderSelectedProducts(const CString& csLRC)
{
	DoLicenseProducts(csLRC);
}

