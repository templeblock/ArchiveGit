/*
// $Workfile: MSPCKDLL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSPCKDLL.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.10   23 Oct 1996 01:59:52   FRED
// International Catalog and Instant Upgrade structures
// 
//    Rev 1.9   24 Sep 1996 09:51:58   FRED
// Added SystemInformation to registration packet
// 
//    Rev 1.8   10 Jul 1996 12:47:06   FRED
// International registration fields.
// 
//    Rev 1.7   11 Mar 1996 16:31:06   FRED
// New registration packet
// 
//    Rev 1.6   04 Jan 1996 12:04:14   FRED
// Removed collections from Product Order Request packets.
// 
// Added Product Information Request Packets.
// 
// 
//    Rev 1.5   28 Dec 1995 09:28:32   FRED
// GetRequest now NULLs handle if packet not recognized.
// Added PutProductOrderResponse() (ordinal 4).
// 
// 
//    Rev 1.4   21 Dec 1995 15:55:24   FRED
// Product Orders
// 
//    Rev 1.3   12 Dec 1995 17:56:56   FRED
// ParentPartNumber
// 
//    Rev 1.2   12 Dec 1995 13:29:20   FRED
// Product version
// 
//    Rev 1.1   05 Dec 1995 12:23:20   FRED
// Catalog orders
// 
//    Rev 1.0   04 Dec 1995 07:52:52   FRED
// Initial revision.
// 
//    Rev 1.1   28 Nov 1995 16:22:28   FRED
// Added Email
// 
//    Rev 1.0   31 Oct 1995 15:09:26   FRED
// Uses CWnd::CenterWindow()
*/ 

// MSPCKDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mspacket.h"
#include "MSPCKDLL.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMspckDllApp

BEGIN_MESSAGE_MAP(CMspckDllApp, CWinApp)
	//{{AFX_MSG_MAP(CMspckDllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMspckDllApp construction

CMspckDllApp::CMspckDllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMspckDllApp object

CMspckDllApp theApp;

BOOL CMspckDllApp::InitInstance() 
{
	if (!CWinApp::InitInstance())
	{
		return FALSE;
	}

	return TRUE;
}

/*
// Utility function to unpack a pascal string into a CString.
*/

CString PascalToCString(LPBYTE pPascalString)
{
	CString csResult;
	
	TRY
	{
		if (pPascalString != NULL)
		{
			UINT uLength = (*pPascalString);
			
			if (uLength > 0)
			{
				LPBYTE pString = (LPBYTE)(csResult.GetBuffer(uLength+1));
				memcpy(pString, &(pPascalString[1]), uLength);
				pString[uLength] = '\0';
				csResult.ReleaseBuffer();
			}
		}
	}
	CATCH_ALL(e)
	{
		csResult.Empty();
	}
	END_CATCH_ALL
	
	return csResult;
}

/*
// Utility function to copy a null terminated string into a pascal string buffer.
*/
 
void CopyToPascalString(LPCSTR pszString, LPBYTE pPascalString, UINT nBufferSize)
{
	if ((pPascalString == NULL) || (nBufferSize == 0))
	{
		AfxThrowUserException();
	}
	
	if (pszString == NULL)
	{
		*pPascalString = 0;
	}
	else
	{
		UINT nLength = strlen(pszString);
		if (nLength >= nBufferSize)
		{
			nLength = nBufferSize-1;
		}
		pPascalString[0] = (BYTE)nLength;
		memcpy(&(pPascalString[1]), pszString, nLength);
	}
}

/*
// Utility function to search for the value of a given field in a registration
// request packet.
*/

CString GetRegistrationFieldValue(CMsregPacketRegistrationRequest* pPacket, LPCSTR pszKey, BOOL fRequired = TRUE)
{
	CString csKey;
	CString csValue;
	
	TRY
	{
		int nFields = pPacket->FieldCount();
		int nField;
		           
		for (nField = 0; nField < nFields; nField++)
		{
			pPacket->GetField(nField, csKey, csValue);
			
			if (csKey.CompareNoCase(pszKey) == 0)
			{
				break;
			}
		}
		
		if (nField >= nFields)
		{
			if (fRequired)
			{
				AfxThrowUserException();
			}
			else
			{
				csKey.Empty();
				csValue.Empty();
			}
		}
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return csValue;
}

/*
// Utility function to search for the value of a given field in a catalog order
// request packet.
*/

CString GetCatalogOrderFieldValue(CMsregPacketCatalogOrderRequest* pPacket, LPCSTR pszKey, BOOL fRequired = TRUE)
{
	CString csKey;
	CString csValue;
	
	TRY
	{
		int nFields = pPacket->FieldCount();
		int nField;
		           
		for (nField = 0; nField < nFields; nField++)
		{
			pPacket->GetField(nField, csKey, csValue);
			
			if (csKey.CompareNoCase(pszKey) == 0)
			{
				break;
			}
		}
		
		if (nField >= nFields)
		{
			if (fRequired)
			{
				AfxThrowUserException();
			}
			else
			{
				csKey.Empty();
				csValue.Empty();
			}
		}
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return csValue;
}

/*
// Utility function to search for the value of a given field in a product order
// request packet.
*/

CString GetProductOrderFieldValue(CMsregPacketProductOrderRequest* pPacket, LPCSTR pszKey, BOOL fRequired = TRUE)
{
	CString csKey;
	CString csValue;
	
	TRY
	{
		int nFields = pPacket->FieldCount();
		int nField;
		           
		for (nField = 0; nField < nFields; nField++)
		{
			pPacket->GetField(nField, csKey, csValue);
			
			if (csKey.CompareNoCase(pszKey) == 0)
			{
				break;
			}
		}
		
		if (nField >= nFields)
		{
			if (fRequired)
			{
				AfxThrowUserException();
			}
			else
			{
				csKey.Empty();
				csValue.Empty();
			}
		}
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return csValue;
}

/*
// Utility function to search for the value of a given field in a product information
// request packet.
*/

CString GetProductInformationFieldValue(CMsregPacketProductInformationRequest* pPacket, LPCSTR pszKey, BOOL fRequired = TRUE)
{
	CString csKey;
	CString csValue;
	
	TRY
	{
		int nFields = pPacket->FieldCount();
		int nField;
		           
		for (nField = 0; nField < nFields; nField++)
		{
			pPacket->GetField(nField, csKey, csValue);
			
			if (csKey.CompareNoCase(pszKey) == 0)
			{
				break;
			}
		}
		
		if (nField >= nFields)
		{
			if (fRequired)
			{
				AfxThrowUserException();
			}
			else
			{
				csKey.Empty();
				csValue.Empty();
			}
		}
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return csValue;
}

/*
// Unpack a registration request from a file.
*/

void GetRegistrationRequest(CMsregPacketHeader* pHeader, CMsregPacketRegistrationRequest* pPacket, HGLOBAL* pHandle)
{
	CString csValue;
	HGLOBAL hInformation = NULL;
	RegistrationRequestInformation* pInformation = NULL;
	
	TRY
	{
		*pHandle = NULL;
		
		// Make sure we have something we know about.
		
		if ((pPacket->Type() != CMsregPacket::BLOCK_RegistrationRequest)
		 || (pPacket->Version() != CMsregPacket::VERSION_RegistrationRequest))
		{
			AfxThrowUserException();
		}
		
		// Allocate the memory for the information block.
		
		hInformation = GlobalAlloc(GMEM_MOVEABLE, sizeof(RegistrationRequestInformation));
		
		if (hInformation != NULL)
		{
			pInformation = (RegistrationRequestInformation*)GlobalLock(hInformation);
		}
		
		if ((hInformation == NULL)
		 || (pInformation == NULL))
		{
			AfxThrowUserException();
		}
		
		// Unpack the registration request data into the structure.
		
		pInformation->RequestType = REQUESTPACKETTYPE_Registration;
		pInformation->RequestVersion = REQUESTPACKETVERSION_Registration;
		
		csValue = pPacket->ProductName();
		CopyToPascalString(csValue, &(pInformation->ProductName[0]), sizeof(pInformation->ProductName));
			
		csValue = GetRegistrationFieldValue(pPacket, "Version", FALSE);
		CopyToPascalString(csValue, &(pInformation->ProductVersion[0]), sizeof(pInformation->ProductVersion));
			
		csValue = GetRegistrationFieldValue(pPacket, "ParentPartNumber", FALSE);
		CopyToPascalString(csValue, &(pInformation->ParentPartNumber[0]), sizeof(pInformation->ParentPartNumber));
			
		csValue = GetRegistrationFieldValue(pPacket, "LastName");
		CopyToPascalString(csValue, &(pInformation->LastName[0]), sizeof(pInformation->LastName));
			
		csValue = GetRegistrationFieldValue(pPacket, "FirstName");
		CopyToPascalString(csValue, &(pInformation->FirstName[0]), sizeof(pInformation->FirstName));
			
		csValue = GetRegistrationFieldValue(pPacket, "Company", FALSE);
		CopyToPascalString(csValue, &(pInformation->Company[0]), sizeof(pInformation->Company));
			
		csValue = GetRegistrationFieldValue(pPacket, "Title", FALSE);
		CopyToPascalString(csValue, &(pInformation->Title[0]), sizeof(pInformation->Title));
			
		csValue = GetRegistrationFieldValue(pPacket, "Address1");
		CopyToPascalString(csValue, &(pInformation->Address1[0]), sizeof(pInformation->Address1));
			
		csValue = GetRegistrationFieldValue(pPacket, "Address2", FALSE);
		CopyToPascalString(csValue, &(pInformation->Address2[0]), sizeof(pInformation->Address2));
			
		csValue = GetRegistrationFieldValue(pPacket, "City");
		CopyToPascalString(csValue, &(pInformation->City[0]), sizeof(pInformation->City));
			
		csValue = GetRegistrationFieldValue(pPacket, "State");
		CopyToPascalString(csValue, &(pInformation->State[0]), sizeof(pInformation->State));
			
		csValue = GetRegistrationFieldValue(pPacket, "Zip");
		CopyToPascalString(csValue, &(pInformation->Zip[0]), sizeof(pInformation->Zip));
			
		csValue = GetRegistrationFieldValue(pPacket, "Country", FALSE);
		CopyToPascalString(csValue, &(pInformation->Country[0]), sizeof(pInformation->Country));
			
		csValue = GetRegistrationFieldValue(pPacket, "Phone", FALSE);
		CopyToPascalString(csValue, &(pInformation->Phone[0]), sizeof(pInformation->Phone));
			
		csValue = GetRegistrationFieldValue(pPacket, "Email", FALSE);
		CopyToPascalString(csValue, &(pInformation->Email[0]), sizeof(pInformation->Email));
		  
		csValue = GetRegistrationFieldValue(pPacket, "OfferCode", FALSE);
		CopyToPascalString(csValue, &(pInformation->OfferCode[0]), sizeof(pInformation->OfferCode));
			
		csValue = GetRegistrationFieldValue(pPacket, "SystemInformation", FALSE);
		CopyToPascalString(csValue, &(pInformation->SystemInformation[0]), sizeof(pInformation->SystemInformation));
			
		// Return the handle of the information.
		
		GlobalUnlock(hInformation);
		pInformation = NULL;
		*pHandle = hInformation;
		hInformation = NULL;
	}
	CATCH_ALL(e)
	{
		csValue.Empty();
		
		if (hInformation != NULL)
		{
			if (pInformation != NULL)
			{
				GlobalUnlock(hInformation);
				pInformation = NULL;
			}
			
			GlobalFree(hInformation);
			hInformation = NULL;
		}
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Unpack a catalog order request from a file.
*/

void GetCatalogOrderRequest(CMsregPacketHeader* pHeader, CMsregPacketCatalogOrderRequest* pPacket, HGLOBAL* pHandle)
{
	CString csValue;
	HGLOBAL hInformation = NULL;
	CatalogOrderRequestInformation* pInformation = NULL;
	int nItem;
	int nCollection;
	
	TRY
	{
		*pHandle = NULL;
		
		// Make sure we have something we know about.
		if ((pPacket->Type() != CMsregPacket::BLOCK_CatalogOrderRequest)
		 || (pPacket->Version() != CMsregPacket::VERSION_CatalogOrderRequest))
		{
			AfxThrowUserException();
		}
		
		// Figure out how large a block of memory we will need.
		
		DWORD dwSize = 0;
		
		dwSize += sizeof(CatalogOrderRequestInformation);
		
		// Add in the size of the line items
		for (nItem = 0; nItem < pPacket->LineItemCount(); nItem++)
		{
			// Add in size of line item.
      	dwSize += sizeof(CatalogOrderRequestLineItemInformation);
      	
      	// Get a pointer to the line item.
			CMsregPacketCatalogOrderLineItem* pItem;
			pPacket->GetLineItem(nItem, pItem);
			
			// Add in the size of item's collections.
			for (nCollection = 0; nCollection < pItem->CollectionCount(); nCollection++)
			{
				// Get the collection name.
				pItem->GetCollection(nCollection, csValue);

				// Add in the size of the collection name as Pascal LString.
				dwSize += csValue.GetLength()+1;
			}
		}
		
		// Allocate the memory for the information block.
		hInformation = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		
		if (hInformation != NULL)
		{
			pInformation = (CatalogOrderRequestInformation*)GlobalLock(hInformation);
		}
		
		if ((hInformation == NULL)
		 || (pInformation == NULL))
		{
			AfxThrowUserException();
		}
		
		// Unpack the registration request data into the structure.
		pInformation->RequestType = REQUESTPACKETTYPE_CatalogOrder;
		pInformation->RequestVersion = REQUESTPACKETVERSION_CatalogOrder;
		
		csValue = pPacket->ProductName();
		CopyToPascalString(csValue, &(pInformation->ProductName[0]), sizeof(pInformation->ProductName));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Version", FALSE);
		CopyToPascalString(csValue, &(pInformation->ProductVersion[0]), sizeof(pInformation->ProductVersion));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "ParentPartNumber", FALSE);
		CopyToPascalString(csValue, &(pInformation->ParentPartNumber[0]), sizeof(pInformation->ParentPartNumber));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "LastName");
		CopyToPascalString(csValue, &(pInformation->LastName[0]), sizeof(pInformation->LastName));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "FirstName");
		CopyToPascalString(csValue, &(pInformation->FirstName[0]), sizeof(pInformation->FirstName));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Company", FALSE);
		CopyToPascalString(csValue, &(pInformation->Company[0]), sizeof(pInformation->Company));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Title", FALSE);
		CopyToPascalString(csValue, &(pInformation->Title[0]), sizeof(pInformation->Title));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Address1");
		CopyToPascalString(csValue, &(pInformation->Address1[0]), sizeof(pInformation->Address1));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Address2", FALSE);
		CopyToPascalString(csValue, &(pInformation->Address2[0]), sizeof(pInformation->Address2));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "City");
		CopyToPascalString(csValue, &(pInformation->City[0]), sizeof(pInformation->City));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "State");
		CopyToPascalString(csValue, &(pInformation->State[0]), sizeof(pInformation->State));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Zip");
		CopyToPascalString(csValue, &(pInformation->Zip[0]), sizeof(pInformation->Zip));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Country", FALSE);
		CopyToPascalString(csValue, &(pInformation->Country[0]), sizeof(pInformation->Country));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Phone", FALSE);
		CopyToPascalString(csValue, &(pInformation->Phone[0]), sizeof(pInformation->Phone));
			
		csValue = GetCatalogOrderFieldValue(pPacket, "Email", FALSE);
		CopyToPascalString(csValue, &(pInformation->Email[0]), sizeof(pInformation->Email));
		  
		csValue = GetCatalogOrderFieldValue(pPacket, "PaymentMethod", FALSE);
		CopyToPascalString(csValue, &(pInformation->PaymentMethod[0]), sizeof(pInformation->PaymentMethod));
		
		csValue = GetCatalogOrderFieldValue(pPacket, "CardNumber", FALSE);
		CopyToPascalString(csValue, &(pInformation->CardNumber[0]), sizeof(pInformation->CardNumber));
		
		csValue = GetCatalogOrderFieldValue(pPacket, "ExpirationDate", FALSE);
		CopyToPascalString(csValue, &(pInformation->ExpirationDate[0]), sizeof(pInformation->ExpirationDate));
		
		CopyToPascalString(pPacket->OfferCode(), &(pInformation->OfferCode[0]), sizeof(pInformation->OfferCode));
		
		CopyToPascalString(pPacket->RequestCode(), &(pInformation->RequestCode[0]), sizeof(pInformation->RequestCode));
		
		pInformation->Tax = pPacket->Tax();
		
		pInformation->Shipping = pPacket->Shipping();
		
		pInformation->Total = pPacket->Total();
		
		// Write out the variable length data.
		LPBYTE pData = (LPBYTE)(pInformation)+sizeof(CatalogOrderRequestInformation);

		// Write out the line items.
		pInformation->LineItemCount = pPacket->LineItemCount();
		
		for (nItem = 0; nItem < pPacket->LineItemCount(); nItem++)
		{
			// Get pointer to line item information block.
			CatalogOrderRequestLineItemInformation* pItemInformation = (CatalogOrderRequestLineItemInformation*)pData;
			pData += sizeof(CatalogOrderRequestLineItemInformation);
			
			// Get a pointer to the line item.
			CMsregPacketCatalogOrderLineItem* pItem;
			pPacket->GetLineItem(nItem, pItem);

			// Fill in line item information.
			pItemInformation->LineItemNumber = nItem;
			CopyToPascalString(pItem->Category(), &(pItemInformation->Category[0]), sizeof(pItemInformation->Category));
			pItemInformation->Quantity = (WORD)(pItem->Quantity());
			pItemInformation->Cost = pItem->Cost();
			
			// Write out line item collections.
			pItemInformation->CollectionCount = pItem->CollectionCount();
			
			for (nCollection = 0; nCollection < pItem->CollectionCount(); nCollection++)
			{
				// Get pointer to collection information block.
				LPBYTE pCollectionInformation = (LPBYTE)pData;
				
				// Get the collection name.
				pItem->GetCollection(nCollection, csValue);
				
				// Write collection name.
				CopyToPascalString(csValue, pCollectionInformation, 256);

				// Move past the collection name.
				pData += (*pData)+1;
			}
		}
		
		// Return the handle of the information.
		
		GlobalUnlock(hInformation);
		pInformation = NULL;
		*pHandle = hInformation;
		hInformation = NULL;
	}
	CATCH_ALL(e)
	{
		csValue.Empty();
		
		if (hInformation != NULL)
		{
			if (pInformation != NULL)
			{
				GlobalUnlock(hInformation);
				pInformation = NULL;
			}
			
			GlobalFree(hInformation);
			hInformation = NULL;
		}
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Unpack a product order request from a file.
*/

void GetProductOrderRequest(CMsregPacketHeader* pHeader, CMsregPacketProductOrderRequest* pPacket, HGLOBAL* pHandle)
{
	CString csValue;
	HGLOBAL hInformation = NULL;
	ProductOrderRequestInformation* pInformation = NULL;
	int nItem;
	
	TRY
	{
		*pHandle = NULL;
		
		// Make sure we have something we know about.
		if ((pPacket->Type() != CMsregPacket::BLOCK_ProductOrderRequest)
		 || (pPacket->Version() != CMsregPacket::VERSION_ProductOrderRequest))
		{
			AfxThrowUserException();
		}
		
		// Figure out how large a block of memory we will need.
		
		DWORD dwSize = 0;
		
		dwSize += sizeof(ProductOrderRequestInformation);
		
		// Add in the size of the line items
		for (nItem = 0; nItem < pPacket->LineItemCount(); nItem++)
		{
			// Add in size of line item.
      	dwSize += sizeof(ProductOrderRequestLineItemInformation);
      	
      	// Get a pointer to the line item.
			CMsregPacketProductOrderLineItem* pItem;
			pPacket->GetLineItem(nItem, pItem);
		}
		
		// Allocate the memory for the information block.
		hInformation = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		
		if (hInformation != NULL)
		{
			pInformation = (ProductOrderRequestInformation*)GlobalLock(hInformation);
		}
		
		if ((hInformation == NULL)
		 || (pInformation == NULL))
		{
			AfxThrowUserException();
		}
		
		// Unpack the registration request data into the structure.
		pInformation->RequestType = REQUESTPACKETTYPE_ProductOrder;
		pInformation->RequestVersion = REQUESTPACKETVERSION_ProductOrder;
		
		csValue = pPacket->ProductName();
		CopyToPascalString(csValue, &(pInformation->ProductName[0]), sizeof(pInformation->ProductName));
			
		csValue = GetProductOrderFieldValue(pPacket, "Version", FALSE);
		CopyToPascalString(csValue, &(pInformation->ProductVersion[0]), sizeof(pInformation->ProductVersion));
			
		csValue = GetProductOrderFieldValue(pPacket, "ParentPartNumber");
		CopyToPascalString(csValue, &(pInformation->ParentPartNumber[0]), sizeof(pInformation->ParentPartNumber));
			
		csValue = GetProductOrderFieldValue(pPacket, "LastName");
		CopyToPascalString(csValue, &(pInformation->LastName[0]), sizeof(pInformation->LastName));
			
		csValue = GetProductOrderFieldValue(pPacket, "FirstName");
		CopyToPascalString(csValue, &(pInformation->FirstName[0]), sizeof(pInformation->FirstName));
			
		csValue = GetProductOrderFieldValue(pPacket, "Company", FALSE);
		CopyToPascalString(csValue, &(pInformation->Company[0]), sizeof(pInformation->Company));
			
		csValue = GetProductOrderFieldValue(pPacket, "Title", FALSE);
		CopyToPascalString(csValue, &(pInformation->Title[0]), sizeof(pInformation->Title));
			
		csValue = GetProductOrderFieldValue(pPacket, "Address1");
		CopyToPascalString(csValue, &(pInformation->Address1[0]), sizeof(pInformation->Address1));
			
		csValue = GetProductOrderFieldValue(pPacket, "Address2", FALSE);
		CopyToPascalString(csValue, &(pInformation->Address2[0]), sizeof(pInformation->Address2));
			
		csValue = GetProductOrderFieldValue(pPacket, "City");
		CopyToPascalString(csValue, &(pInformation->City[0]), sizeof(pInformation->City));
			
		csValue = GetProductOrderFieldValue(pPacket, "State");
		CopyToPascalString(csValue, &(pInformation->State[0]), sizeof(pInformation->State));
			
		csValue = GetProductOrderFieldValue(pPacket, "Zip");
		CopyToPascalString(csValue, &(pInformation->Zip[0]), sizeof(pInformation->Zip));
			
		csValue = GetProductOrderFieldValue(pPacket, "Phone");
		CopyToPascalString(csValue, &(pInformation->Phone[0]), sizeof(pInformation->Phone));
			
		csValue = GetProductOrderFieldValue(pPacket, "Email", FALSE);
		CopyToPascalString(csValue, &(pInformation->Email[0]), sizeof(pInformation->Email));
		  
		csValue = GetProductOrderFieldValue(pPacket, "PaymentMethod");
		CopyToPascalString(csValue, &(pInformation->PaymentMethod[0]), sizeof(pInformation->PaymentMethod));
		
		csValue = GetProductOrderFieldValue(pPacket, "CardNumber");
		CopyToPascalString(csValue, &(pInformation->CardNumber[0]), sizeof(pInformation->CardNumber));
		
		csValue = GetProductOrderFieldValue(pPacket, "ExpirationDate");
		CopyToPascalString(csValue, &(pInformation->ExpirationDate[0]), sizeof(pInformation->ExpirationDate));
		
		CopyToPascalString(pPacket->OfferCode(), &(pInformation->OfferCode[0]), sizeof(pInformation->OfferCode));
		
		CopyToPascalString("", &(pInformation->RequestCode[0]), sizeof(pInformation->RequestCode));
		
		pInformation->Tax = pPacket->Tax();
		
		pInformation->Shipping = pPacket->Shipping();
		
		pInformation->Total = pPacket->Total();
		
		// Write out the variable length data.
		LPBYTE pData = (LPBYTE)(pInformation)+sizeof(ProductOrderRequestInformation);

		// Write out the line items.
		pInformation->LineItemCount = pPacket->LineItemCount();
		
		for (nItem = 0; nItem < pPacket->LineItemCount(); nItem++)
		{
			// Get pointer to line item information block.
			ProductOrderRequestLineItemInformation* pItemInformation = (ProductOrderRequestLineItemInformation*)pData;
			pData += sizeof(ProductOrderRequestLineItemInformation);
			
			// Get a pointer to the line item.
			CMsregPacketProductOrderLineItem* pItem;
			pPacket->GetLineItem(nItem, pItem);

			// Fill in line item information.
			pItemInformation->LineItemNumber = nItem;
			CopyToPascalString(pItem->ProductCode(), &(pItemInformation->ParentPartNumber[0]), sizeof(pItemInformation->ParentPartNumber));
			pItemInformation->Quantity = (WORD)(pItem->Quantity());
			pItemInformation->Cost = pItem->Cost();
			
			// Convert type to flags.
			pItemInformation->Flags = 0;
		}
		
		// Return the handle of the information.
		
		GlobalUnlock(hInformation);
		pInformation = NULL;
		*pHandle = hInformation;
		hInformation = NULL;
	}
	CATCH_ALL(e)
	{
		csValue.Empty();
		
		if (hInformation != NULL)
		{
			if (pInformation != NULL)
			{
				GlobalUnlock(hInformation);
				pInformation = NULL;
			}
			
			GlobalFree(hInformation);
			hInformation = NULL;
		}
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Unpack a product information request from a file.
*/

void GetProductInformationRequest(CMsregPacketHeader* pHeader, CMsregPacketProductInformationRequest* pPacket, HGLOBAL* pHandle)
{
	CString csValue;
	HGLOBAL hInformation = NULL;
	ProductInformationRequestInformation* pInformation = NULL;
	int nItem;
	
	TRY
	{
		*pHandle = NULL;
		
		// Make sure we have something we know about.
		if ((pPacket->Type() != CMsregPacket::BLOCK_ProductInformationRequest)
		 || (pPacket->Version() != CMsregPacket::VERSION_ProductInformationRequest))
		{
			AfxThrowUserException();
		}
		
		// Figure out how large a block of memory we will need.
		
		DWORD dwSize = 0;
		
		dwSize += sizeof(ProductInformationRequestInformation);
		
		// Add in the size of the line items
		for (nItem = 0; nItem < pPacket->LineItemCount(); nItem++)
		{
			// Add in size of line item.
      	dwSize += sizeof(ProductInformationRequestLineItemInformation);
      	
      	// Get a pointer to the line item.
			CMsregPacketProductInformationLineItem* pItem;
			pPacket->GetLineItem(nItem, pItem);
		}
		
		// Allocate the memory for the information block.
		hInformation = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		
		if (hInformation != NULL)
		{
			pInformation = (ProductInformationRequestInformation*)GlobalLock(hInformation);
		}
		
		if ((hInformation == NULL)
		 || (pInformation == NULL))
		{
			AfxThrowUserException();
		}
		
		// Unpack the registration request data into the structure.
		pInformation->RequestType = REQUESTPACKETTYPE_ProductInformation;
		pInformation->RequestVersion = REQUESTPACKETVERSION_ProductInformation;
		
		csValue = pPacket->ProductName();
		CopyToPascalString(csValue, &(pInformation->ProductName[0]), sizeof(pInformation->ProductName));
			
		csValue = GetProductInformationFieldValue(pPacket, "Version", FALSE);
		CopyToPascalString(csValue, &(pInformation->ProductVersion[0]), sizeof(pInformation->ProductVersion));
			
		csValue = GetProductInformationFieldValue(pPacket, "ParentPartNumber");
		CopyToPascalString(csValue, &(pInformation->ParentPartNumber[0]), sizeof(pInformation->ParentPartNumber));
			
		csValue = GetProductInformationFieldValue(pPacket, "LastName");
		CopyToPascalString(csValue, &(pInformation->LastName[0]), sizeof(pInformation->LastName));
			
		csValue = GetProductInformationFieldValue(pPacket, "FirstName");
		CopyToPascalString(csValue, &(pInformation->FirstName[0]), sizeof(pInformation->FirstName));
			
		csValue = GetProductInformationFieldValue(pPacket, "Company", FALSE);
		CopyToPascalString(csValue, &(pInformation->Company[0]), sizeof(pInformation->Company));
			
		csValue = GetProductInformationFieldValue(pPacket, "Title", FALSE);
		CopyToPascalString(csValue, &(pInformation->Title[0]), sizeof(pInformation->Title));
			
		csValue = GetProductInformationFieldValue(pPacket, "Address1");
		CopyToPascalString(csValue, &(pInformation->Address1[0]), sizeof(pInformation->Address1));
			
		csValue = GetProductInformationFieldValue(pPacket, "Address2", FALSE);
		CopyToPascalString(csValue, &(pInformation->Address2[0]), sizeof(pInformation->Address2));
			
		csValue = GetProductInformationFieldValue(pPacket, "City");
		CopyToPascalString(csValue, &(pInformation->City[0]), sizeof(pInformation->City));
			
		csValue = GetProductInformationFieldValue(pPacket, "State");
		CopyToPascalString(csValue, &(pInformation->State[0]), sizeof(pInformation->State));
			
		csValue = GetProductInformationFieldValue(pPacket, "Zip");
		CopyToPascalString(csValue, &(pInformation->Zip[0]), sizeof(pInformation->Zip));
			
		csValue = GetProductInformationFieldValue(pPacket, "Phone", FALSE);
		CopyToPascalString(csValue, &(pInformation->Phone[0]), sizeof(pInformation->Phone));
			
		csValue = GetProductInformationFieldValue(pPacket, "Email", FALSE);
		CopyToPascalString(csValue, &(pInformation->Email[0]), sizeof(pInformation->Email));
		  
		csValue = GetProductInformationFieldValue(pPacket, "PaymentMethod", FALSE);
		CopyToPascalString(csValue, &(pInformation->PaymentMethod[0]), sizeof(pInformation->PaymentMethod));
		
		csValue = GetProductInformationFieldValue(pPacket, "CardNumber", FALSE);
		CopyToPascalString(csValue, &(pInformation->CardNumber[0]), sizeof(pInformation->CardNumber));
		
		csValue = GetProductInformationFieldValue(pPacket, "ExpirationDate", FALSE);
		CopyToPascalString(csValue, &(pInformation->ExpirationDate[0]), sizeof(pInformation->ExpirationDate));
		
		CopyToPascalString(pPacket->OfferCode(), &(pInformation->OfferCode[0]), sizeof(pInformation->OfferCode));
		
		CopyToPascalString("", &(pInformation->RequestCode[0]), sizeof(pInformation->RequestCode));
		
		pInformation->Tax = pPacket->Tax();
		
		pInformation->Shipping = pPacket->Shipping();
		
		pInformation->Total = pPacket->Total();
		
		// Write out the variable length data.
		LPBYTE pData = (LPBYTE)(pInformation)+sizeof(ProductInformationRequestInformation);

		// Write out the line items.
		pInformation->LineItemCount = pPacket->LineItemCount();
		
		for (nItem = 0; nItem < pPacket->LineItemCount(); nItem++)
		{
			// Get pointer to line item information block.
			ProductInformationRequestLineItemInformation* pItemInformation = (ProductInformationRequestLineItemInformation*)pData;
			pData += sizeof(ProductInformationRequestLineItemInformation);
			
			// Get a pointer to the line item.
			CMsregPacketProductInformationLineItem* pItem;
			pPacket->GetLineItem(nItem, pItem);

			// Fill in line item information.
			pItemInformation->LineItemNumber = nItem;
			CopyToPascalString(pItem->ProductCode(), &(pItemInformation->ParentPartNumber[0]), sizeof(pItemInformation->ParentPartNumber));
			pItemInformation->Quantity = (WORD)(pItem->Quantity());
			pItemInformation->Cost = pItem->Cost();
			
			// Convert type to flags.
			pItemInformation->Flags = 0;
		}
		
		// Return the handle of the information.
		
		GlobalUnlock(hInformation);
		pInformation = NULL;
		*pHandle = hInformation;
		hInformation = NULL;
	}
	CATCH_ALL(e)
	{
		csValue.Empty();
		
		if (hInformation != NULL)
		{
			if (pInformation != NULL)
			{
				GlobalUnlock(hInformation);
				pInformation = NULL;
			}
			
			GlobalFree(hInformation);
			hInformation = NULL;
		}
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Unpack a request from a file.
*/

extern "C" DWORD EXPORT PASCAL GetRequest(LPBYTE pFileName, HGLOBAL* pInformationHandle)
{
	DWORD dwResult = 0;
	
	CStdioFile cfFile;
	CString csFileName;
	CMsregPacketHeader* pPacketHeader = NULL;
	CMsregPacket* pRequestPacket = NULL;
	CString csValue;
	
	TRY
	{
		*pInformationHandle = NULL;
	
		// Open the file containing the packet.
		
		csFileName = PascalToCString(pFileName);
		
		if (!cfFile.Open(csFileName, CFile::typeBinary|CFile::modeRead))
		{
			AfxThrowUserException();
		}
		
		// Read the packet (assume it has the correct structure).
		
		pPacketHeader = (CMsregPacketHeader*)(CMsregPacket::ReadPacket(cfFile));
		
		// Validate the packet.
		
		if ((pPacketHeader == NULL)
		 || (pPacketHeader->Type() != CMsregPacket::BLOCK_PacketHeader)
		 || (pPacketHeader->Version() != CMsregPacket::VERSION_PacketHeader)
		 || (pPacketHeader->PacketCount() != 1)
		 || ((pRequestPacket = pPacketHeader->GetPacket(0)) == NULL))
		{
			AfxThrowUserException();
		}
		
		// Branch on the type of packet.
		switch (pRequestPacket->Type())
		{
			case CMsregPacket::BLOCK_RegistrationRequest:
			{
				GetRegistrationRequest(pPacketHeader, (CMsregPacketRegistrationRequest*)pRequestPacket, pInformationHandle);
				break;
			}
			
			case CMsregPacket::BLOCK_CatalogOrderRequest:
			{
				GetCatalogOrderRequest(pPacketHeader, (CMsregPacketCatalogOrderRequest*)pRequestPacket, pInformationHandle);
				break;
			}
			
			case CMsregPacket::BLOCK_ProductOrderRequest:
			{
				GetProductOrderRequest(pPacketHeader, (CMsregPacketProductOrderRequest*)pRequestPacket, pInformationHandle);
				break;
			}
			
			case CMsregPacket::BLOCK_ProductInformationRequest:
			{
				GetProductInformationRequest(pPacketHeader, (CMsregPacketProductInformationRequest*)pRequestPacket, pInformationHandle);
				break;
			}
			
			default:
			{
				AfxThrowUserException();
			}
		}
		
		dwResult = 1;
	}
	CATCH_ALL(e)
	{
		*pInformationHandle = NULL;
		dwResult = 0;
	}
	END_CATCH_ALL
	
	delete pPacketHeader;
	
	return dwResult;
}

/*
// Pack a registration response into a file.
*/

extern "C" DWORD EXPORT PASCAL PutRegistrationResponse(LPBYTE pFileName, DWORD dwRegistered)
{
	DWORD dwResult = 0;
	
	CStdioFile cfFile;
	CString csFileName;
	CMsregPacketHeader PacketHeader;
	CMsregPacketRegistrationResponse RegistrationResponse;
	
	TRY
	{
		// Open the file which will contain the packet.
		
		csFileName = PascalToCString(pFileName);
		
		if (!cfFile.Open(csFileName, CFile::typeBinary|CFile::modeCreate|CFile::modeWrite))
		{
			AfxThrowUserException();
		}
		
		// Fill in the packets.
		
		RegistrationResponse.ProductName() = "";
		RegistrationResponse.Registered() = dwRegistered;
		RegistrationResponse.Message() = "";
		
		PacketHeader.CustomerId() = "";
		PacketHeader.AddPacket(&RegistrationResponse);
		
		// Write the packets to the file.
		
		CMsregPacket::WritePacket(cfFile, &PacketHeader);
		
		dwResult = 1;
	}
	CATCH_ALL(e)
	{
		dwResult = 0;
	}
	END_CATCH_ALL
	
	return dwResult;
}

/*
// Pack a catalog order response into a file.
*/

extern "C" DWORD EXPORT PASCAL PutCatalogOrderResponse(LPBYTE pFileName, DWORD dwCatalogOrderResult, LPBYTE pAccessCode, DWORD dwInvoiceNumber)
{
	DWORD dwResult = 0;
	
	CStdioFile cfFile;
	CString csFileName;
	CMsregPacketHeader PacketHeader;
	CMsregPacketCatalogOrderResponse CatalogOrderResponse;
	
	TRY
	{
		// Open the file which will contain the packet.
		
		csFileName = PascalToCString(pFileName);
		
		if (!cfFile.Open(csFileName, CFile::typeBinary|CFile::modeCreate|CFile::modeWrite))
		{
			AfxThrowUserException();
		}
		
		// Fill in the packets.
		
		CatalogOrderResponse.ProductName() = "";
		CatalogOrderResponse.Result() = dwCatalogOrderResult;
		CatalogOrderResponse.Message() = "";
		CatalogOrderResponse.AccessCode() = PascalToCString(pAccessCode);
		CatalogOrderResponse.InvoiceNumber() = dwInvoiceNumber;
		
		PacketHeader.CustomerId() = "";
		PacketHeader.AddPacket(&CatalogOrderResponse);
		
		// Write the packets to the file.
		
		CMsregPacket::WritePacket(cfFile, &PacketHeader);
		
		dwResult = 1;
	}
	CATCH_ALL(e)
	{
		dwResult = 0;
	}
	END_CATCH_ALL
	
	return dwResult;
}

/*
// Pack a product order response into a file.
*/

extern "C" DWORD EXPORT PASCAL PutProductOrderResponse(LPBYTE pFileName, DWORD dwProductOrderResult, DWORD dwInvoiceNumber)
{
	DWORD dwResult = 0;
	
	CStdioFile cfFile;
	CString csFileName;
	CMsregPacketHeader PacketHeader;
	CMsregPacketProductOrderResponse ProductOrderResponse;
	
	TRY
	{
		// Open the file which will contain the packet.
		
		csFileName = PascalToCString(pFileName);
		
		if (!cfFile.Open(csFileName, CFile::typeBinary|CFile::modeCreate|CFile::modeWrite))
		{
			AfxThrowUserException();
		}
		
		// Fill in the packets.
		
		ProductOrderResponse.ProductName() = "";
		ProductOrderResponse.Result() = dwProductOrderResult;
		ProductOrderResponse.Message() = "";
		ProductOrderResponse.InvoiceNumber() = dwInvoiceNumber;
		
		PacketHeader.CustomerId() = "";
		PacketHeader.AddPacket(&ProductOrderResponse);
		
		// Write the packets to the file.
		
		CMsregPacket::WritePacket(cfFile, &PacketHeader);
		
		dwResult = 1;
	}
	CATCH_ALL(e)
	{
		dwResult = 0;
	}
	END_CATCH_ALL
	
	return dwResult;
}

/*
// Pack a product information response into a file.
*/

extern "C" DWORD EXPORT PASCAL PutProductInformationResponse(LPBYTE pFileName, DWORD dwProductInformationResult, DWORD dwInvoiceNumber)
{
	DWORD dwResult = 0;
	
	CStdioFile cfFile;
	CString csFileName;
	CMsregPacketHeader PacketHeader;
	CMsregPacketProductInformationResponse ProductInformationResponse;
	
	TRY
	{
		// Open the file which will contain the packet.
		
		csFileName = PascalToCString(pFileName);
		
		if (!cfFile.Open(csFileName, CFile::typeBinary|CFile::modeCreate|CFile::modeWrite))
		{
			AfxThrowUserException();
		}
		
		// Fill in the packets.
		
		ProductInformationResponse.ProductName() = "";
		ProductInformationResponse.Result() = dwProductInformationResult;
		ProductInformationResponse.Message() = "";
		ProductInformationResponse.InvoiceNumber() = dwInvoiceNumber;
		
		PacketHeader.CustomerId() = "";
		PacketHeader.AddPacket(&ProductInformationResponse);
		
		// Write the packets to the file.
		
		CMsregPacket::WritePacket(cfFile, &PacketHeader);
		
		dwResult = 1;
	}
	CATCH_ALL(e)
	{
		dwResult = 0;
	}
	END_CATCH_ALL
	
	return dwResult;
}
