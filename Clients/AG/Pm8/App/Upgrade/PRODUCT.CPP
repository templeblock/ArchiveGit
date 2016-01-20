/*
// Product info and data
//
// $Workfile: PRODUCT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PRODUCT.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:28   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:36   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:20   JAY
// Initial revision.
// 
//    Rev 1.5   01 Mar 1995 11:20:56   FRED
// Licensed product list no longer depends on offer code.
// 
// 
//    Rev 1.4   05 Dec 1994 14:13:30   JAY
//  
// 
//    Rev 1.3   02 Dec 1994 15:29:32   JAY
// Multiple LRCs per product.
// 
//    Rev 1.2   02 Dec 1994 11:45:10   JAY
//  
// 
//    Rev 1.1   17 Nov 1994 15:51:52   JAY
// Added == operator to CProductData to clean up (and fix) some stuff.
// 
//    Rev 1.0   17 Nov 1994 14:54:46   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "product.h"

/////////////////////////////////////////////////////////////////////////////
// CProductData

/////////////////////////////////////////////////////////////////////////////
// CProductInfo

CProductInfo::CProductInfo()
		: m_Licenses(sizeof(CProductLicense))
{
	m_bVersion = 0;
	m_bFlags = 0;
	m_bOfferCode[0] = '\0';
	m_bProductCode[0] = '\0';
	m_wLicenses = 0;
}

CProductInfo::~CProductInfo()
{
}

void CProductInfo::FromCodes(LPCSTR pOfferCode, LPCSTR pProductCode)
{
	ASSERT(strlen(pOfferCode) < sizeof(m_bOfferCode)-1);
	strcpy((LPSTR)m_bOfferCode, pOfferCode);
	ASSERT(strlen(pProductCode) < sizeof(m_bProductCode)-1);
	strcpy((LPSTR)m_bProductCode, pProductCode);
}

/*
// Does this product have these defining codes?
*/

BOOL CProductInfo::EqualsCodes(LPCSTR pOfferCode, LPCSTR pProductCode) const
{
	switch (m_bVersion)
	{
		case 0:
		{
			return  /*strcmp((LPSTR)m_bOfferCode, pOfferCode) == 0
					&&*/ strcmp((LPSTR)m_bProductCode, pProductCode) == 0;
		}
		default:
		{
			ASSERT(FALSE);
			// Unknown type!
			return FALSE;
		}
	}
}

void CProductInfo::License(BOOL fLicensed /*=TRUE*/)
{
	m_bFlags = fLicensed
					? (m_bFlags | CPIF_Licensed)
					: (m_bFlags & ~CPIF_Licensed);
}

void CProductInfo::ToCodes(LPSTR pOfferCode, LPSTR pProductCode) const
{
	switch (m_bVersion)
	{
		case 0:
		{
			strcpy(pOfferCode, OfferCode());
			strcpy(pProductCode, ProductCode());
			break;
		}
		default:
		{
			ASSERT(FALSE);
		// Unknown type at this time. */
			*pOfferCode = '\0';
			*pProductCode = '\0';
			break;
		}
	}
}

/*
// Does this product have this license code?
*/

BOOL CProductInfo::HasLicenseCode(LPCSTR pLicenseCode) const
{
	switch (m_bVersion)
	{
		case 0:
		{
		/* Look through all licenses to see if we ave one that matches. */

			int nCount = m_Licenses.GetSize();

			if (nCount != 0)
			{
				CProductLicense* pLicense = (CProductLicense*)m_Licenses.GetAt(0);

				for (int i = 0; i < nCount; i++, pLicense++)
				{
					if (strcmp((LPSTR)pLicense->m_bLicenseCode, pLicenseCode) == 0)
					{
						return TRUE;
					}
				}
			}
			break;
		}
		default:
		{
			ASSERT(FALSE);
			// Unknown type!
			break;
		}
	}
	return FALSE;
}

void CProductInfo::AddLicenseCode(LPCSTR pLicenseCode)
{
	if (!HasLicenseCode(pLicenseCode))
	{
		CProductLicense License;

		ASSERT(strlen(pLicenseCode) < sizeof(License.m_bLicenseCode)-1);
		strcpy((LPSTR)License.m_bLicenseCode, pLicenseCode);

		m_Licenses.Add(&License);
		// Update the license count in the structure.
		m_wLicenses = m_Licenses.GetSize();
	}
}

/*
// "Encrypt" some data.
*/

static void near Encrypt(LPBYTE pData, int nLength, BYTE& bEncrypt)
{
	while (nLength-- > 0)
	{
		*pData = (bEncrypt ^= *pData);
		pData++;
	}
}

/*
// "Decrypt" some data.
*/

static void near Decrypt(LPBYTE pData, int nLength, BYTE& bDecrypt)
{
	while (nLength-- > 0)
	{
		BYTE bOldData = *pData;
		*pData ^= bDecrypt;
		bDecrypt = bOldData;
		pData++;
	}
}

/*
// Read from file.
*/

BOOL CProductInfo::Read(CFile* pFile, BYTE& bDecrypt)
{
	TRY
	{
	/*
	// Get the product data into a local struct so we can encrypt it.
	*/
		CProductData ProductData;

		if (pFile->Read(&ProductData, sizeof(ProductData)) != sizeof(ProductData))
		{
			return FALSE;
		}

		Decrypt((LPBYTE)&ProductData, sizeof(ProductData), bDecrypt);

		*(CProductData*)this = ProductData;

		int nCount = m_wLicenses;

		for (int i = 0; i < nCount; i++)
		{
			CProductLicense License;

			if (pFile->Read(&License, sizeof(License)) != sizeof(License))
			{
				return FALSE;
			}
			Decrypt((LPBYTE)&License, sizeof(License), bDecrypt);

			m_Licenses.Add(&License);
		}
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}

/*
// Write to file.
*/

BOOL CProductInfo::Write(CFile* pFile, BYTE& bEncrypt)
{
	TRY
	{
	/*
	// Get the product data into a local struct so we can encrypt it.
	*/

		CProductData ProductData = *(CProductData*)this;

		Encrypt((LPBYTE)&ProductData, sizeof(ProductData), bEncrypt);

		pFile->Write(&ProductData, sizeof(ProductData));

		int nCount = m_Licenses.GetSize();
		ASSERT(nCount == (int)m_wLicenses);
		if (nCount != 0)
		{
			const CProductLicense* pLicense = (const CProductLicense*)m_Licenses.GetAt(0);
	
			for (int i = 0; i < nCount; i++)
			{
				CProductLicense License = *pLicense++;
	
				Encrypt((LPBYTE)&License, sizeof(License), bEncrypt);
	
				pFile->Write(&License, sizeof(License));
			}
		}
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}

/*
// Assignment operator.
*/

const CProductInfo& CProductInfo::operator=(const CProductInfo& Info)
{
	m_bVersion = Info.m_bVersion;
	m_bFlags = Info.m_bFlags;
	memcpy(m_bOfferCode, Info.m_bOfferCode, sizeof(m_bOfferCode));
	memcpy(m_bProductCode, Info.m_bProductCode, sizeof(m_bProductCode));
	m_wLicenses = Info.m_wLicenses;

	int nLicenses = Info.m_Licenses.GetSize();
	ASSERT(nLicenses == (int)m_wLicenses);

	if (nLicenses != 0)
	{
		const CProductLicense* pLicense = (const CProductLicense*)Info.m_Licenses.GetAt(0);

		for (int i = 0; i < nLicenses; i++)
		{
			m_Licenses.Add(pLicense++);
		}
	}
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// CProductInfoArray

CProductInfoArray::CProductInfoArray()
{
}

/*
// Destructor.
// Delete all products in the array.
*/

CProductInfoArray::~CProductInfoArray()
{
	int nProducts = Products();

	for (int nProduct = 0; nProduct < nProducts; nProduct++)
	{
		delete Product(nProduct);
	}
}

/*
// Add a product to the array.
*/

void CProductInfoArray::AddProduct(const CProductInfo* pProduct)
{
	CProductInfo* pNewProduct = NULL;

	TRY
	{
		pNewProduct = new CProductInfo;
	}
	END_TRY

	if (pNewProduct != NULL)
	{
	/* Copy the data. */
		*pNewProduct = *pProduct;
		m_cpaProducts.Add(pNewProduct);
	}
}

