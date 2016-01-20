/*
// $Workfile: PROD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/PROD.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     8/31/98 1:13p Dennis
// Phone # correction
// 
// 2     8/20/98 4:13p Jayn
// LICENSE.DAT filename now sent from the app.
// Now supports exclusion filters.
// 
//    Rev 1.0   14 Aug 1997 15:36:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:00   Fred
// Initial revision.
// 
//    Rev 1.2   11 Jun 1997 11:10:42   Jay
// Changes
// 
//    Rev 1.1   10 Jun 1997 15:44:00   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:50   johno
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
// $Log: /PM8/App/LICDLL/PROD.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     8/31/98 1:13p Dennis
// Phone # correction
// 
// 2     8/20/98 4:13p Jayn
// LICENSE.DAT filename now sent from the app.
// Now supports exclusion filters.
// 
//    Rev 1.32   12 Nov 1996 16:49:12   JAY
// Removed font code; DGROUP reduction
// 
//    Rev 1.31   27 Feb 1996 17:24:40   JAY
// Offer code changes.
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

#include "license.h"
#include "util.h"
#include "prod.h"

extern CString g_cstrCodeToMark;

////////////////////////////////////////////////////////////////////////////
// CProductsInformation

#define	OFFER_CODE_BUFFER_SIZE						(64)
#define	PHONE_NUMBER_BUFFER_SIZE					(64)
#define	FAX_NUMBER_BUFFER_SIZE						(64)
#define	DEPARTMENT_NAME_BUFFER_SIZE				(128)

COfferInformation::COfferInformation()
{
	m_csOfferCode.Empty();
}

COfferInformation::~COfferInformation()
{
}


void COfferInformation::Init(const CString& OfferFile)
{
char buf[1024];
BOOL retval = TRUE;

 if(GetPrivateProfileString("OFFER","OfferCode","",buf,sizeof(buf) - 2,OfferFile))
 {
	m_csOfferCode = buf;
	m_csOfferCode += '-';

	m_csOfferCode += GetApp()->ParentPartNumber();
 }
 else
	 retval = FALSE;

if(GetPrivateProfileString("OFFER","PhoneNumber","",buf,sizeof(buf) - 2,OfferFile))
	m_csPhoneNumber = buf;
 else
	 retval = FALSE;

if(GetPrivateProfileString("OFFER","FaxNumber","",buf,sizeof(buf) - 2,OfferFile))
	m_csFaxNumber = buf;
 else
	 retval = FALSE;

if(GetPrivateProfileString("OFFER","DepartmentName","",buf,sizeof(buf) - 2,OfferFile))
	m_csDepartmentName = buf;
 else
	 retval = FALSE;

if(GetPrivateProfileString("OFFER","ProcessingFee","",buf,sizeof(buf) - 2,OfferFile))
	m_lProcessingFee = strtol(buf,NULL,10);
else
	 retval = FALSE;

if(!GetPrivateProfileString("OFFER","Products","",buf,sizeof(buf) - 2,OfferFile))
	 retval = FALSE;

#pragma message("messagebox here.")
if(!retval){
	AfxMessageBox("Could not find entry in offer.ini");
}else{
	LPSTR pszProducts = buf;
	DefineProducts(this, pszProducts);
}

//old way using encrypted catalog.inf
/*	BOOL fSuccess = FALSE;
	
	m_csOfferCode = "CD";
	m_csPhoneNumber = "1-800-888-0436";
	m_csFaxNumber = "1-800-933-0571";
	m_csDepartmentName = "CD Catalog Dept.";
	m_lProcessingFee = 295;
	
	CString csOfferFile;
	Util::ConstructPath(csOfferFile, csDataDirectory, "CATALOG.INF");
	
	CFile cfOfferFile;
	
	if (cfOfferFile.Open(csOfferFile, CFile::modeRead|CFile::shareDenyNone))
	{
		struct
		{
			char szOfferCode[64];
			char szPhoneNumber[64];
			char szFaxNumber[64];
			char szDepartmentName[128];
			char szProcessingFee[64];
			char szProducts[1024];
		}  OfferInformation;
		
		if (cfOfferFile.Read(&OfferInformation, sizeof(OfferInformation)) == sizeof(OfferInformation))
		{
			if (EncryptBuffer((BYTE*)&OfferInformation, sizeof(OfferInformation), "GreatGraphics"))
			{
				strcpy(m_csOfferCode.GetBuffer(OFFER_CODE_BUFFER_SIZE), OfferInformation.szOfferCode);
				m_csOfferCode.ReleaseBuffer();
				m_csOfferCode += '-';

				m_csOfferCode += GetApp()->ParentPartNumber();
			
				strcpy(m_csPhoneNumber.GetBuffer(PHONE_NUMBER_BUFFER_SIZE), OfferInformation.szPhoneNumber);
				m_csPhoneNumber.ReleaseBuffer();
			
				strcpy(m_csFaxNumber.GetBuffer(FAX_NUMBER_BUFFER_SIZE), OfferInformation.szFaxNumber);
				m_csFaxNumber.ReleaseBuffer();
				
				strcpy(m_csDepartmentName.GetBuffer(DEPARTMENT_NAME_BUFFER_SIZE), OfferInformation.szDepartmentName);
				m_csDepartmentName.ReleaseBuffer();
			
				m_lProcessingFee = atol(OfferInformation.szProcessingFee);
			
				LPSTR pszProducts = OfferInformation.szProducts;
				DefineProducts(this, pszProducts);
				
				fSuccess = TRUE;
			}
		}
	}

	if(!fSuccess)
		THROW(new CNotSupportedException);

  */



}

BOOL COfferInformation::EncryptBuffer(BYTE* bBuffer, int nSize, char *szEncrypt)
{
	BOOL fResult = TRUE;

	char *e = szEncrypt;
	
	for (int i = 0; i < nSize; i++)
	{
		*(bBuffer++) ^= (BYTE)(*(e++));
		
		if (*e == '\0')
		{
			e = szEncrypt;
		}
	}
		
	return fResult;
}

void COfferInformation::DefineProducts(CProductItems* pDestination, LPSTR& pszCodes)
{
	CProductItem* pCurrentItem = NULL;
	char szWord[16];
	
	while (NextWord(pszCodes, &(szWord[0])))
	{
		if (strcmpi(szWord, ")") == 0)
		{
			break;
		}
		
		if (strcmpi(szWord, "(") == 0)
		{
			if (pCurrentItem == NULL)
			{
				DefineProducts(pDestination, pszCodes);
			}
			else
			{
				DefineProducts(pCurrentItem->Items(), pszCodes);
			}
		}
		else
		{
			pCurrentItem = CProductItem::CreateProductItemFromIni(&(szWord[0]));

			if (pCurrentItem != NULL)
			{
				pDestination->Add(pCurrentItem);
			}
		}
	}
}

BOOL COfferInformation::NextWord(LPSTR& pszDescription, LPSTR pszWord)
{
	int nLength = 0;
	
	// Skip leading white space.
	
   while ((*pszDescription == ' ') || (*pszDescription == '\t'))
   {
   	pszDescription++;
   }
   
   while ((*pszDescription != ')')
   	 && (*pszDescription != '(')
   	 && (*pszDescription != ' ')
   	 && (*pszDescription != '\t')
   	 && (*pszDescription != '\0'))
   {
   	pszWord[nLength++] = *(pszDescription++);
   }
   
   if (nLength == 0)
   {
   	if ((*pszDescription == '(')
	    || (*pszDescription == ')'))
	   {
	   	pszWord[nLength++] = *(pszDescription++);
	   }
	}
	
	pszWord[nLength] = '\0';
	
	return nLength > 0;   
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

CString CProductItem::GetIDsFromIni(const CString csKeyName,const CString csSection, BOOL fEmptyOK /*=FALSE*/)
{
	char buf[100];
	buf[0] = '\0';

	::GetPrivateProfileString(csSection,csKeyName,"",buf,sizeof(buf) - 2,GetApp()->GetOrderFile());
	if (strlen(buf) == 0 && !fEmptyOK)
	{
#ifdef WIN32	
		AfxThrowFileException(CFileException::fileNotFound,-1,csKeyName);
#else 
		AfxThrowFileException(CFileException::fileNotFound);
#endif		
	}
	if (stricmp(buf,"N/A") == 0)//The top catalog category will need these entries changed to blank so that they don't appear in the phone order dialog.
		buf[0] = '\0';
	return CString(buf);
}

/*
CProductItem* CProductItem::CreateProductItem(LPCSTR pszCode)
{
	// Try to find the runtime class for this product.
	// The name of the class is derived algorithmicly from the product name.
			
	char szClassName[64];
	sprintf(szClassName, "C%sProductItem", pszCode);

	CRuntimeClass* pClass;
	for (pClass = CRuntimeClass::pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		if (strcmp(szClassName, pClass->m_lpszClassName) == 0)
		{
			break;
		}
	}

	CProductItem* pItem;
		
	if (pClass == NULL)
	{
		TRACE2("Unable to find runtime class '%s' for product item '%s'\n", szClassName, pszCode);
		pItem = new CProductItem;
	}
	else
	{
		pItem = (CProductItem*)pClass->CreateObject();
	}
	
	return pItem;
	return NULL;
}*/


CProductItem* CProductItem::CreateProductItemFromIni(LPCSTR pszCode)
{
	CString csSection = pszCode;
	csSection += "_ID";

	// See if we should be filtered out.
	CString csFilters = GetIDsFromIni("Filters", csSection, TRUE);

	CStringArray csaFilters;
	Util::FillArrayFromDelimitedString(csFilters, ",", csaFilters);
	const CStringArray& csaAppFilters = GetApp()->GetFilters();

	for(int x = 0; x < csaAppFilters.GetSize(); x++)
	{
		for(int n = 0; n < csaFilters.GetSize(); n++)
		{
			if(csaFilters.GetAt(n).CompareNoCase(csaAppFilters.GetAt(x)) == 0)
			{
				// Skip this item. It has been filtered out.
				return NULL;
			}
		}
	}

	// OK to proceed. Read the rest of the pertinent info.
	CString csCode = GetIDsFromIni("Code",csSection);
	CString csName = GetIDsFromIni("Name",csSection);
	CString csCollectionCode = GetIDsFromIni("CollectionCode",csSection);
	CString csCollectionName = GetIDsFromIni("CollectionName",csSection);
	long lPrice = strtol(GetIDsFromIni("Price",csSection), NULL, 10);
	unsigned long lType = strtol(GetIDsFromIni("Type",csSection), NULL, 10);

#pragma message("memory leak here?")
	CProductItem* pItem = new CProductItem(csCode,csName,csCollectionCode,csCollectionName,lPrice,lType);
	unsigned int i = 1;

	csSection = pszCode;
	csSection += "_FILES";
	//keys are numbered from 1 to ...
	//retrieve info about files in this group and add to Items list.
	char buf[50];
	char buf2[200];
	CString csTemp;
	while(GetPrivateProfileString(csSection,itoa(i,buf,10),"",buf2,sizeof(buf2) - 2,GetApp()->GetOrderFile()))
	{
		i++;
		csCode = strtok(buf2,",");
		csCode.TrimLeft();
		csName = strtok(NULL,",");
		csName.TrimLeft();
		csCollectionCode = strtok(NULL,",");
		csCollectionCode.TrimLeft();
		csCollectionName = strtok(NULL,",");
		csCollectionName.TrimLeft();
		csTemp = strtok(NULL,",");
		csTemp.TrimLeft();
		lPrice = strtol(csTemp,NULL,10);
		csTemp = strtok(NULL,",");
		csTemp.TrimLeft();
		lType = strtol(csTemp,NULL,10);
		pItem->Items()->Add(new CProductItem(csCode,csName,csCollectionCode,csCollectionName,lPrice,lType));
	}

	return pItem;
}
			
int CProductItem::Install(CInstallInfo& Info)
{
	TRACE1("Installing %s\n", (LPCSTR)m_csCode);
	return FALSE;
}

BOOL CProductItem::UpdateLicenseFlags(const CInstallationData* pData)
{
	BOOL fLicensed = pData->ProductIsLicensed(Code());

	if(g_cstrCodeToMark == Code()){
		SetFlag(PRODUCT_ITEM_Marked);
		return FALSE;
	}

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
