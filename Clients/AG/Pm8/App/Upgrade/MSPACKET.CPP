/*
// $Workfile: MSPACKET.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MSPACKET.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:20   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:00   JAY
// Initial revision.
// 
//    Rev 1.3   21 Dec 1995 15:46:56   FRED
// Properly handles product order line item type
// 
//    Rev 1.1   21 Dec 1995 15:03:18   FRED
// Product Orders
// 
//    Rev 1.1   05 Dec 1995 12:23:16   FRED
// Catalog orders
// 
//    Rev 1.5   30 Oct 1995 17:21:14   FRED
//  
// 
//    Rev 1.4   30 Oct 1995 12:11:50   FRED
//  
// 
//    Rev 1.3   27 Oct 1995 16:28:54   FRED
// Some cleanup
// 
//    Rev 1.2   27 Oct 1995 16:13:20   FRED
// More communications stuff.
// 
//    Rev 1.1   26 Oct 1995 17:59:44   FRED
//  
// 
//    Rev 1.0   26 Oct 1995 16:59:20   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "mspacket.h"


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Base Class for all Packets ////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacket

CMsregPacket::CMsregPacket()
{
	m_Header.dwVersion = 0;
	m_Header.dwType = 0;
	m_Header.dwChecksum = 0;
}

CMsregPacket::~CMsregPacket()
{
}

void CMsregPacket::ReadFromFile(CFile& cfFile)
{
	// Call the top level object in the derivation chain to actually
	// read the data from the file. A checksum will be accumulated as
	// the reading progresses. Note that the header will not be included
	// in the checksum.

	DWORD dwChecksum = 0;

	ReadDataFromFile(cfFile, dwChecksum);

	// Check that the header read matches the accumulated values.

	if (m_Header.dwChecksum != dwChecksum)
	{
		AfxThrowFileException(CFileException::invalidFile);
	}
}

void CMsregPacket::WriteToFile(CFile& cfFile)
{
	// Save the current position so we can return and write out
	// the correct checksum.

	DWORD dwPosition = cfFile.GetPosition();

	// Call the top level object in the derivation chain to actually
	// write the data to the file. A checksum will be accumulated as
	// the writing progresses. Note that the header will not be included
	// in the checksum. The size of the block is determined after the
	// block data has been written. The block size is used to skip
	// unknown blocks.

	m_Header.dwSize = 0;
	m_Header.dwChecksum = 0;

	WriteDataToFile(cfFile, m_Header.dwChecksum);

	// All the data has been written. Rewrite the header with the real values.
	// Note that the checksum value has already been filled in. The block size
	// is difference between the current file position and the starting position.

	DWORD dwNewPosition = cfFile.GetPosition();
	m_Header.dwSize = dwNewPosition-dwPosition;

	cfFile.Seek(dwPosition, CFile::begin);

	CMsregPacket::WriteDataToFile(cfFile, m_Header.dwChecksum);

	// Move back to the end of the block.

	cfFile.Seek(dwNewPosition, CFile::begin);
}

void CMsregPacket::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Read the header. Note that the header is not included in the checksum.
	ReadData(cfFile, &m_Header, sizeof(m_Header));
}

void CMsregPacket::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Initialize the header data values. The checksum is assumed to already
	// be computed. Note that this function is called twice. The first time
	// the check sum will be zero, the second call will write the tur value.

	m_Header.dwVersion = GetFileDataVersion();
	m_Header.dwType = GetFileDataType();

	// Write the header. Note that the header is not included in the checksum.
	WriteData(cfFile, &m_Header, sizeof(m_Header));
}

void CMsregPacket::ReadData(CFile& cfFile, void* pBuffer, UINT uCount)
{
	if (cfFile.Read(pBuffer, uCount) != uCount)
	{
		AfxThrowFileException(CFileException::endOfFile);
	}
}

void CMsregPacket::WriteData(CFile& cfFile, const void* pBuffer, UINT uCount)
{
	cfFile.Write(pBuffer, uCount);
}

void CMsregPacket::ReadData(CFile& cfFile, void* pBuffer, UINT uCount, DWORD& dwChecksum)
{
	ReadData(cfFile, pBuffer, uCount);
	ChecksumData(pBuffer, uCount, dwChecksum);
}

void CMsregPacket::WriteData(CFile& cfFile, const void* pBuffer, UINT uCount, DWORD& dwChecksum)
{
	WriteData(cfFile, pBuffer, uCount);
	ChecksumData(pBuffer, uCount, dwChecksum);
}

void CMsregPacket::ChecksumData(const void* pBuffer, UINT uCount, DWORD& dwChecksum)
{
	LPBYTE p = (LPBYTE)pBuffer;

	while (uCount-- > 0)
	{
		dwChecksum += *(p++);
	}
}

CMsregPacket* CMsregPacket::ReadPacket(CFile& cfFile)
{
	// ReadPacket will return NULL if the packet exists, but the
	// type is unknown. The file pointer will be positioned at the
	// byte following the unknown block.

	// Read the header.
	CMsregPacket::StandardBlockHeaderType Header;
	ReadData(cfFile, &Header, sizeof(Header));

	// Seek back to the start of the header.
	cfFile.Seek(-((LONG)sizeof(Header)), CFile::current);

	// Create a new object based on the type field of the header.

	CMsregPacket* pPacket = NULL;

	switch (Header.dwType)
	{
		case BLOCK_PacketHeader:
		{
			pPacket = new CMsregPacketHeader;
			break;
		}

		case BLOCK_RegistrationRequest:
		{
			pPacket = new CMsregPacketRegistrationRequest;
			break;
		}

		case BLOCK_RegistrationResponse:
		{
			pPacket = new CMsregPacketRegistrationResponse;
			break;
		}

		case BLOCK_CatalogOrderRequest:
		{
			pPacket = new CMsregPacketCatalogOrderRequest;
			break;
		}

		case BLOCK_CatalogOrderResponse:
		{
			pPacket = new CMsregPacketCatalogOrderResponse;
			break;
		}

		case BLOCK_ProductOrderRequest:
		{
			pPacket = new CMsregPacketProductOrderRequest;
			break;
		}

		case BLOCK_ProductOrderResponse:
		{
			pPacket = new CMsregPacketProductOrderResponse;
			break;
		}

		default:
		{
			// The block type is unknown, skip to the next block.
			cfFile.Seek(Header.dwSize, CFile::current);
			break;
		}
	}

	if (pPacket != NULL)
	{
		// Ask the packet to read itself.
		pPacket->ReadFromFile(cfFile);
	}

	return pPacket;
}

void CMsregPacket::WritePacket(CFile& cfFile, CMsregPacket* pPacket)
{
	// Ask the packet to write itself to the file.
	pPacket->WriteToFile(cfFile);
}

BYTE CMsregPacket::ReadByte(CFile& cfFile, DWORD& dwChecksum)
{
	BYTE Data;
	ReadData(cfFile, &Data, sizeof(Data), dwChecksum);
	return Data;
}

void CMsregPacket::WriteByte(CFile& cfFile, BYTE Data, DWORD& dwChecksum)
{
	WriteData(cfFile, &Data, sizeof(Data), dwChecksum);
}

char CMsregPacket::ReadChar(CFile& cfFile, DWORD& dwChecksum)
{
	char Data;
	ReadData(cfFile, &Data, sizeof(Data), dwChecksum);
	return Data;
}

void CMsregPacket::WriteChar(CFile& cfFile, char Data, DWORD& dwChecksum)
{
	WriteData(cfFile, &Data, sizeof(Data), dwChecksum);
}

WORD CMsregPacket::ReadWord(CFile& cfFile, DWORD& dwChecksum)
{
	WORD Data;
	ReadData(cfFile, &Data, sizeof(Data), dwChecksum);
	return Data;
}

void CMsregPacket::WriteWord(CFile& cfFile, WORD Data, DWORD& dwChecksum)
{
	WriteData(cfFile, &Data, sizeof(Data), dwChecksum);
}

DWORD CMsregPacket::ReadDword(CFile& cfFile, DWORD& dwChecksum)
{
	DWORD Data;
	ReadData(cfFile, &Data, sizeof(Data), dwChecksum);
	return Data;
}

void CMsregPacket::WriteDword(CFile& cfFile, DWORD Data, DWORD& dwChecksum)
{
	WriteData(cfFile, &Data, sizeof(Data), dwChecksum);
}

CString CMsregPacket::ReadString(CFile& cfFile, DWORD& dwChecksum)
{
	CString csResult;

	TRY
	{
		DWORD dwLength = ReadStringLength(cfFile, dwChecksum);
		
		if (dwLength == 0)
		{
			csResult.Empty();
		}
		else
		{
			ReadData(cfFile, csResult.GetBuffer((int)dwLength), (UINT)dwLength, dwChecksum);
			csResult.ReleaseBuffer();
		}
	}
	CATCH_ALL(e)
	{
		csResult.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL

	return csResult;
}

void CMsregPacket::WriteString(CFile& cfFile, LPCSTR pszString, DWORD& dwChecksum)
{
	WriteStringLength(cfFile, pszString, dwChecksum);
	WriteData(cfFile, pszString, strlen(pszString), dwChecksum);
}

DWORD CMsregPacket::ReadStringLength(CFile& cfFile, DWORD& dwChecksum)
{
	return ReadDword(cfFile, dwChecksum);
}

void CMsregPacket::WriteStringLength(CFile& cfFile, LPCSTR pszString, DWORD& dwChecksum)
{
	WriteDword(cfFile, strlen(pszString), dwChecksum);
}


///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Utiltity Packets //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketHeader

CMsregPacketHeader::CMsregPacketHeader()
{
}

CMsregPacketHeader::~CMsregPacketHeader()
{
	DeleteAllPackets();
}

void CMsregPacketHeader::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Read inherited data.
	CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

	// Read customer ID.
	m_csCustomerId = ReadString(cfFile, dwChecksum);

	// Read packet count.
	DWORD dwPackets = ReadDword(cfFile, dwChecksum);

	// Read packets.
	DeleteAllPackets();
	for (DWORD i = 0; i < dwPackets; i++)
	{
		AddPacket(ReadPacket(cfFile));
	}
}

void CMsregPacketHeader::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Write inherited data.
	CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

	// Write customer ID.
	WriteString(cfFile, m_csCustomerId, dwChecksum);

	// Write packet count.
	WriteDword(cfFile, PacketCount(), dwChecksum);

	// Write packets.
	for (int i = 0; i < PacketCount(); i++)
	{
		WritePacket(cfFile, GetPacket(i));
	}
}

DWORD CMsregPacketHeader::GetFileDataVersion(void)
{
	return VERSION_PacketHeader;
}

DWORD CMsregPacketHeader::GetFileDataType(void)
{
	return BLOCK_PacketHeader;
}

void CMsregPacketHeader::AddPacket(CMsregPacket* pPacket)
{
	ASSERT(pPacket != NULL);

	if (pPacket != NULL)
	{
		m_Packets.Add(pPacket);
	}
}

CMsregPacket* CMsregPacketHeader::GetPacket(int nIndex)
{
	return (CMsregPacket*)(m_Packets[nIndex]);
}

int CMsregPacketHeader::PacketCount(void)
{
	return m_Packets.GetSize();
}

void CMsregPacketHeader::DeleteAllPackets(void)
{
	for (int i = 0; i < m_Packets.GetSize(); i++)
	{
		CMsregPacket* pPacket = (CMsregPacket*)(m_Packets[i]);
		
		ASSERT(pPacket != NULL);
		
		delete pPacket;
		m_Packets.SetAt(i, NULL);
	}
	
	m_Packets.RemoveAll();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Registration Packets ////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketRegistrationRequest

CMsregPacketRegistrationRequest::CMsregPacketRegistrationRequest()
{
}

CMsregPacketRegistrationRequest::~CMsregPacketRegistrationRequest()
{
	DeleteAllFields();
}

void CMsregPacketRegistrationRequest::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csKey;
	CString csValue;

	TRY
	{
		// Read inherited data.
		CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

		// Read the product name.
		m_csProductName = ReadString(cfFile, dwChecksum);

		// Read the field count.
		DWORD dwFields = ReadDword(cfFile, dwChecksum);

		// Read Fields.
		DeleteAllFields();
		for (DWORD i = 0; i < dwFields; i++)
		{
			csKey = ReadString(cfFile, dwChecksum);
			csValue = ReadString(cfFile, dwChecksum);
			AddField(csKey, csValue);
		}
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketRegistrationRequest::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csKey;
	CString csValue;

	TRY
	{
		// Write inherited data.
		CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

		// Write the product name.
		WriteString(cfFile, m_csProductName, dwChecksum);

		// Write the field count.
		WriteDword(cfFile, FieldCount(), dwChecksum);

		// Write Fields.
		for (int i = 0; i < FieldCount(); i++)
		{
			GetField(i, csKey, csValue);
			WriteString(cfFile, csKey, dwChecksum);
			WriteString(cfFile, csValue, dwChecksum);
		}
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

DWORD CMsregPacketRegistrationRequest::GetFileDataVersion(void)
{
	return VERSION_RegistrationRequest;
}

DWORD CMsregPacketRegistrationRequest::GetFileDataType(void)
{
	return BLOCK_RegistrationRequest;
}

void CMsregPacketRegistrationRequest::AddField(LPCSTR pszKey, LPCSTR pszValue)
{
	m_Keys.Add(pszKey);
	m_Values.Add(pszValue);
}

void CMsregPacketRegistrationRequest::GetField(int nIndex, CString& csKey, CString& csValue)
{
	csKey = m_Keys[nIndex];
	csValue = m_Values[nIndex];
}

int CMsregPacketRegistrationRequest::FieldCount(void)
{
	return m_Keys.GetSize();
}

void CMsregPacketRegistrationRequest::DeleteAllFields(void)
{
	m_Keys.RemoveAll();
	m_Values.RemoveAll();
}

/////////////////////////////////////////////////////////
// CMsregPacketRegistrationResponse

CMsregPacketRegistrationResponse::CMsregPacketRegistrationResponse()
{
}

CMsregPacketRegistrationResponse::~CMsregPacketRegistrationResponse()
{
}

void CMsregPacketRegistrationResponse::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Read inherited data.
	CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

	// Read the product name.
	m_csProductName = ReadString(cfFile, dwChecksum);

	// Read the registered flag.
	m_dwRegistered = ReadDword(cfFile, dwChecksum);

	// Read the message (normally empty if successful).
	m_csMessage = ReadString(cfFile, dwChecksum);
}

void CMsregPacketRegistrationResponse::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Write inherited data.
	CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

	// Write the product name.
	WriteString(cfFile, m_csProductName, dwChecksum);

	// Write the registered flag.
	WriteDword(cfFile, m_dwRegistered, dwChecksum);

	// Write the message (normally empty if successful).
	WriteString(cfFile, m_csMessage, dwChecksum);
}

DWORD CMsregPacketRegistrationResponse::GetFileDataVersion(void)
{
	return VERSION_RegistrationResponse;
}

DWORD CMsregPacketRegistrationResponse::GetFileDataType(void)
{
	return BLOCK_RegistrationResponse;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Catalog Order Packets ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderLineItem

CMsregPacketCatalogOrderLineItem::CMsregPacketCatalogOrderLineItem()
{
}

CMsregPacketCatalogOrderLineItem::~CMsregPacketCatalogOrderLineItem()
{
	DeleteAllCollections();
}

void CMsregPacketCatalogOrderLineItem::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csCollection;

	TRY
	{
		// Read the category name.
		m_csCategory = CMsregPacket::ReadString(cfFile, dwChecksum);

		// Read the quantity.
		m_dwQuantity = CMsregPacket::ReadDword(cfFile, dwChecksum);

		// Write the cost.
		m_dwCost = CMsregPacket::ReadDword(cfFile, dwChecksum);

		// Read the collection count.
		DWORD dwCollections = CMsregPacket::ReadDword(cfFile, dwChecksum);

		// Read Collections.
		DeleteAllCollections();
		for (DWORD i = 0; i < dwCollections; i++)
		{
			csCollection = CMsregPacket::ReadString(cfFile, dwChecksum);
			AddCollection(csCollection);
		}
	}
	CATCH_ALL(e)
	{
		csCollection.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketCatalogOrderLineItem::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csCollection;

	TRY
	{
		// Write the category name.
		CMsregPacket::WriteString(cfFile, m_csCategory, dwChecksum);

		// Write the quantity.
		CMsregPacket::WriteDword(cfFile, m_dwQuantity, dwChecksum);

		// Write the cost.
		CMsregPacket::WriteDword(cfFile, m_dwCost, dwChecksum);

		// Write the field count.
		CMsregPacket::WriteDword(cfFile, CollectionCount(), dwChecksum);

		// Write collection names.
		for (int i = 0; i < CollectionCount(); i++)
		{
			GetCollection(i, csCollection);
			CMsregPacket::WriteString(cfFile, csCollection, dwChecksum);
		}
	}
	CATCH_ALL(e)
	{
		csCollection.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketCatalogOrderLineItem::AddCollection(LPCSTR pszCollection)
{
	m_Collections.Add(pszCollection);
}

void CMsregPacketCatalogOrderLineItem::GetCollection(int nIndex, CString& csCollection)
{
	csCollection = m_Collections[nIndex];
}

int CMsregPacketCatalogOrderLineItem::CollectionCount(void)
{
	return m_Collections.GetSize();
}

void CMsregPacketCatalogOrderLineItem::DeleteAllCollections(void)
{
	m_Collections.RemoveAll();
}

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderLineItemArray

CMsregPacketCatalogOrderLineItemArray::CMsregPacketCatalogOrderLineItemArray()
{
}

CMsregPacketCatalogOrderLineItemArray::~CMsregPacketCatalogOrderLineItemArray()
{
	RemoveAll();
}
	
void CMsregPacketCatalogOrderLineItemArray::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CMsregPacketCatalogOrderLineItem* pLineItem = NULL;
	
	TRY
	{
		// Remove any current line items.
		RemoveAll();
		
		// Read the line item count.
		DWORD dwLineItems = CMsregPacket::ReadDword(cfFile, dwChecksum);
		
		// Read the line items.
		for (DWORD i = 0; i < dwLineItems; i++)
		{
			pLineItem = new CMsregPacketCatalogOrderLineItem;
			pLineItem->ReadDataFromFile(cfFile, dwChecksum);
			Add(pLineItem);
			pLineItem = NULL;
		}
	}
	CATCH_ALL(e)
	{
		delete pLineItem;
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketCatalogOrderLineItemArray::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Write the line item count.
	DWORD dwLineItems = GetSize();
	CMsregPacket::WriteDword(cfFile, dwLineItems, dwChecksum);
		
	// Write the line items.
	for (DWORD i = 0; i < dwLineItems; i++)
	{
		GetAt((int)i)->WriteDataToFile(cfFile, dwChecksum);
	}
}

void CMsregPacketCatalogOrderLineItemArray::RemoveAll()
{
	int i;
	
	for (i = 0; i < GetSize(); i++)
	{
		CMsregPacketCatalogOrderLineItem* pItem = GetAt(i);
		
		ASSERT(pItem != NULL);
		
		delete pItem;
		SetAt(i, NULL);
	}
	
	CPtrArray::RemoveAll();
}

void CMsregPacketCatalogOrderLineItemArray::RemoveAt(int nIndex, int nCount /*=1*/, BOOL fDelete /*=TRUE*/)
{
	if (fDelete)
	{
		for (int i = 0; i < nCount; i++)
		{
			if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
			{
				CMsregPacketCatalogOrderLineItem* pItem = GetAt(nIndex+i);

				ASSERT(pItem != NULL);

				delete pItem;
				SetAt(nIndex+i, NULL);
			}
		}
	}
	
	CPtrArray::RemoveAt(nIndex, nCount);
}

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderRequest

CMsregPacketCatalogOrderRequest::CMsregPacketCatalogOrderRequest()
{
}

CMsregPacketCatalogOrderRequest::~CMsregPacketCatalogOrderRequest()
{
	DeleteAllFields();
	DeleteAllLineItems();
}

void CMsregPacketCatalogOrderRequest::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csKey;
	CString csValue;

	TRY
	{
		// Read inherited data.
		CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

		// Read the product name.
		m_csProductName = ReadString(cfFile, dwChecksum);

		// Read the field count.
		DWORD dwFields = ReadDword(cfFile, dwChecksum);

		// Read Fields.
		DeleteAllFields();
		for (DWORD i = 0; i < dwFields; i++)
		{
			csKey = ReadString(cfFile, dwChecksum);
			csValue = ReadString(cfFile, dwChecksum);
			AddField(csKey, csValue);
		}
		
		// Read the tax.
		m_dwTax = ReadDword(cfFile, dwChecksum);
		
		// Read the shipping.
		m_dwShipping = ReadDword(cfFile, dwChecksum);
		
		// Read the total.
		m_dwTotal = ReadDword(cfFile, dwChecksum);
		
		// Read the offer code.
		m_csOfferCode = ReadString(cfFile, dwChecksum);
		
		// Read the request code.
		m_csRequestCode = ReadString(cfFile, dwChecksum);
		
		// Read the line items.
		m_LineItems.ReadDataFromFile(cfFile, dwChecksum);
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketCatalogOrderRequest::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csKey;
	CString csValue;

	TRY
	{
		// Write inherited data.
		CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

		// Write the product name.
		WriteString(cfFile, m_csProductName, dwChecksum);

		// Write the field count.
		WriteDword(cfFile, FieldCount(), dwChecksum);

		// Write Fields.
		for (int i = 0; i < FieldCount(); i++)
		{
			GetField(i, csKey, csValue);
			WriteString(cfFile, csKey, dwChecksum);
			WriteString(cfFile, csValue, dwChecksum);
		}
		
		// Write the tax.
		WriteDword(cfFile, m_dwTax, dwChecksum);
		
		// Write the shipping.
		WriteDword(cfFile, m_dwShipping, dwChecksum);
		
		// Write the total.
		WriteDword(cfFile, m_dwTotal, dwChecksum);
		
		// Write the offer code.
		WriteString(cfFile, m_csOfferCode, dwChecksum);
		
		// Write the request code.
		WriteString(cfFile, m_csRequestCode, dwChecksum);
		
		// Write the line items.
		m_LineItems.WriteDataToFile(cfFile, dwChecksum);
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

DWORD CMsregPacketCatalogOrderRequest::GetFileDataVersion(void)
{
	return VERSION_CatalogOrderRequest;
}

DWORD CMsregPacketCatalogOrderRequest::GetFileDataType(void)
{
	return BLOCK_CatalogOrderRequest;
}

void CMsregPacketCatalogOrderRequest::AddField(LPCSTR pszKey, LPCSTR pszValue)
{
	m_Keys.Add(pszKey);
	m_Values.Add(pszValue);
}

void CMsregPacketCatalogOrderRequest::GetField(int nIndex, CString& csKey, CString& csValue)
{
	csKey = m_Keys[nIndex];
	csValue = m_Values[nIndex];
}

int CMsregPacketCatalogOrderRequest::FieldCount(void)
{
	return m_Keys.GetSize();
}

void CMsregPacketCatalogOrderRequest::DeleteAllFields(void)
{
	m_Keys.RemoveAll();
	m_Values.RemoveAll();
}
	
void CMsregPacketCatalogOrderRequest::AddLineItem(CMsregPacketCatalogOrderLineItem* pItem)
{
	m_LineItems.Add(pItem);
}

void CMsregPacketCatalogOrderRequest::GetLineItem(int nIndex, CMsregPacketCatalogOrderLineItem*& pItem)
{
	pItem = m_LineItems.GetAt(nIndex);
}

int CMsregPacketCatalogOrderRequest::LineItemCount(void)
{
	return m_LineItems.GetSize();
}

void CMsregPacketCatalogOrderRequest::DeleteAllLineItems(void)
{
	m_LineItems.RemoveAll();
}

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderResponse

CMsregPacketCatalogOrderResponse::CMsregPacketCatalogOrderResponse()
{
}

CMsregPacketCatalogOrderResponse::~CMsregPacketCatalogOrderResponse()
{
}

void CMsregPacketCatalogOrderResponse::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Read inherited data.
	CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

	// Read the product name.
	m_csProductName = ReadString(cfFile, dwChecksum);

	// Read the result value.
	m_dwResult = ReadDword(cfFile, dwChecksum);

	// Read the message (normally empty if successful).
	m_csMessage = ReadString(cfFile, dwChecksum);
	
	// Read the access code.
	m_csAccessCode = ReadString(cfFile, dwChecksum);
	
	// Read the invoice number.
	m_dwInvoiceNumber = ReadDword(cfFile, dwChecksum);
}

void CMsregPacketCatalogOrderResponse::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Write inherited data.
	CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

	// Write the product name.
	WriteString(cfFile, m_csProductName, dwChecksum);

	// Write the result value.
	WriteDword(cfFile, m_dwResult, dwChecksum);

	// Write the message (normally empty if successful).
	WriteString(cfFile, m_csMessage, dwChecksum);
	
	// Write the access code.
	WriteString(cfFile, m_csAccessCode, dwChecksum);
	
	// Write the invoice number.
	WriteDword(cfFile, m_dwInvoiceNumber, dwChecksum);
}

DWORD CMsregPacketCatalogOrderResponse::GetFileDataVersion(void)
{
	return VERSION_CatalogOrderResponse;
}

DWORD CMsregPacketCatalogOrderResponse::GetFileDataType(void)
{
	return BLOCK_CatalogOrderResponse;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Product Order Packets ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderLineItem

CMsregPacketProductOrderLineItem::CMsregPacketProductOrderLineItem()
{
}

CMsregPacketProductOrderLineItem::~CMsregPacketProductOrderLineItem()
{
	DeleteAllCollections();
}

void CMsregPacketProductOrderLineItem::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csCollection;

	TRY
	{
		// Read the type.
		m_wType = CMsregPacket::ReadWord(cfFile, dwChecksum);
		
		// Read the category name.
		m_csCategory = CMsregPacket::ReadString(cfFile, dwChecksum);

		// Read the quantity.
		m_dwQuantity = CMsregPacket::ReadDword(cfFile, dwChecksum);

		// Write the cost.
		m_dwCost = CMsregPacket::ReadDword(cfFile, dwChecksum);

		// Read the collection count.
		DWORD dwCollections = CMsregPacket::ReadDword(cfFile, dwChecksum);

		// Read Collections.
		DeleteAllCollections();
		for (DWORD i = 0; i < dwCollections; i++)
		{
			csCollection = CMsregPacket::ReadString(cfFile, dwChecksum);
			AddCollection(csCollection);
		}
	}
	CATCH_ALL(e)
	{
		csCollection.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketProductOrderLineItem::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csCollection;

	TRY
	{
		// Write the type.
		CMsregPacket::WriteWord(cfFile, m_wType, dwChecksum);
		
		// Write the category name.
		CMsregPacket::WriteString(cfFile, m_csCategory, dwChecksum);

		// Write the quantity.
		CMsregPacket::WriteDword(cfFile, m_dwQuantity, dwChecksum);

		// Write the cost.
		CMsregPacket::WriteDword(cfFile, m_dwCost, dwChecksum);

		// Write the field count.
		CMsregPacket::WriteDword(cfFile, CollectionCount(), dwChecksum);

		// Write collection names.
		for (int i = 0; i < CollectionCount(); i++)
		{
			GetCollection(i, csCollection);
			CMsregPacket::WriteString(cfFile, csCollection, dwChecksum);
		}
	}
	CATCH_ALL(e)
	{
		csCollection.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketProductOrderLineItem::AddCollection(LPCSTR pszCollection)
{
	m_Collections.Add(pszCollection);
}

void CMsregPacketProductOrderLineItem::GetCollection(int nIndex, CString& csCollection)
{
	csCollection = m_Collections[nIndex];
}

int CMsregPacketProductOrderLineItem::CollectionCount(void)
{
	return m_Collections.GetSize();
}

void CMsregPacketProductOrderLineItem::DeleteAllCollections(void)
{
	m_Collections.RemoveAll();
}

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderLineItemArray

CMsregPacketProductOrderLineItemArray::CMsregPacketProductOrderLineItemArray()
{
}

CMsregPacketProductOrderLineItemArray::~CMsregPacketProductOrderLineItemArray()
{
	RemoveAll();
}
	
void CMsregPacketProductOrderLineItemArray::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CMsregPacketProductOrderLineItem* pLineItem = NULL;
	
	TRY
	{
		// Remove any current line items.
		RemoveAll();
		
		// Read the line item count.
		DWORD dwLineItems = CMsregPacket::ReadDword(cfFile, dwChecksum);
		
		// Read the line items.
		for (DWORD i = 0; i < dwLineItems; i++)
		{
			pLineItem = new CMsregPacketProductOrderLineItem;
			pLineItem->ReadDataFromFile(cfFile, dwChecksum);
			Add(pLineItem);
			pLineItem = NULL;
		}
	}
	CATCH_ALL(e)
	{
		delete pLineItem;
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketProductOrderLineItemArray::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Write the line item count.
	DWORD dwLineItems = GetSize();
	CMsregPacket::WriteDword(cfFile, dwLineItems, dwChecksum);
		
	// Write the line items.
	for (DWORD i = 0; i < dwLineItems; i++)
	{
		GetAt((int)i)->WriteDataToFile(cfFile, dwChecksum);
	}
}

void CMsregPacketProductOrderLineItemArray::RemoveAll()
{
	int i;
	
	for (i = 0; i < GetSize(); i++)
	{
		CMsregPacketProductOrderLineItem* pItem = GetAt(i);
		
		ASSERT(pItem != NULL);
		
		delete pItem;
		SetAt(i, NULL);
	}
	
	CPtrArray::RemoveAll();
}

void CMsregPacketProductOrderLineItemArray::RemoveAt(int nIndex, int nCount /*=1*/, BOOL fDelete /*=TRUE*/)
{
	if (fDelete)
	{
		for (int i = 0; i < nCount; i++)
		{
			if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
			{
				CMsregPacketProductOrderLineItem* pItem = GetAt(nIndex+i);

				ASSERT(pItem != NULL);

				delete pItem;
				SetAt(nIndex+i, NULL);
			}
		}
	}
	
	CPtrArray::RemoveAt(nIndex, nCount);
}

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderRequest

CMsregPacketProductOrderRequest::CMsregPacketProductOrderRequest()
{
}

CMsregPacketProductOrderRequest::~CMsregPacketProductOrderRequest()
{
	DeleteAllFields();
	DeleteAllLineItems();
}

void CMsregPacketProductOrderRequest::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csKey;
	CString csValue;

	TRY
	{
		// Read inherited data.
		CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

		// Read the product name.
		m_csProductName = ReadString(cfFile, dwChecksum);

		// Read the field count.
		DWORD dwFields = ReadDword(cfFile, dwChecksum);

		// Read Fields.
		DeleteAllFields();
		for (DWORD i = 0; i < dwFields; i++)
		{
			csKey = ReadString(cfFile, dwChecksum);
			csValue = ReadString(cfFile, dwChecksum);
			AddField(csKey, csValue);
		}
		
		// Read the tax.
		m_dwTax = ReadDword(cfFile, dwChecksum);
		
		// Read the shipping.
		m_dwShipping = ReadDword(cfFile, dwChecksum);
		
		// Read the total.
		m_dwTotal = ReadDword(cfFile, dwChecksum);
		
		// Read the offer code.
		m_csOfferCode = ReadString(cfFile, dwChecksum);
		
		// Read the shipping method.
		m_dwShippingMethod = ReadDword(cfFile, dwChecksum);
		
		// Read the line items.
		m_LineItems.ReadDataFromFile(cfFile, dwChecksum);
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CMsregPacketProductOrderRequest::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	CString csKey;
	CString csValue;

	TRY
	{
		// Write inherited data.
		CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

		// Write the product name.
		WriteString(cfFile, m_csProductName, dwChecksum);

		// Write the field count.
		WriteDword(cfFile, FieldCount(), dwChecksum);

		// Write Fields.
		for (int i = 0; i < FieldCount(); i++)
		{
			GetField(i, csKey, csValue);
			WriteString(cfFile, csKey, dwChecksum);
			WriteString(cfFile, csValue, dwChecksum);
		}
		
		// Write the tax.
		WriteDword(cfFile, m_dwTax, dwChecksum);
		
		// Write the shipping.
		WriteDword(cfFile, m_dwShipping, dwChecksum);
		
		// Write the total.
		WriteDword(cfFile, m_dwTotal, dwChecksum);
		
		// Write the offer code.
		WriteString(cfFile, m_csOfferCode, dwChecksum);
		
		// Write the shipping method.
		WriteDword(cfFile, m_dwShippingMethod, dwChecksum);
		
		// Write the line items.
		m_LineItems.WriteDataToFile(cfFile, dwChecksum);
	}
	CATCH_ALL(e)
	{
		csKey.Empty();
		csValue.Empty();
		THROW_LAST();
	}
	END_CATCH_ALL
}

DWORD CMsregPacketProductOrderRequest::GetFileDataVersion(void)
{
	return VERSION_ProductOrderRequest;
}

DWORD CMsregPacketProductOrderRequest::GetFileDataType(void)
{
	return BLOCK_ProductOrderRequest;
}

void CMsregPacketProductOrderRequest::AddField(LPCSTR pszKey, LPCSTR pszValue)
{
	m_Keys.Add(pszKey);
	m_Values.Add(pszValue);
}

void CMsregPacketProductOrderRequest::GetField(int nIndex, CString& csKey, CString& csValue)
{
	csKey = m_Keys[nIndex];
	csValue = m_Values[nIndex];
}

int CMsregPacketProductOrderRequest::FieldCount(void)
{
	return m_Keys.GetSize();
}

void CMsregPacketProductOrderRequest::DeleteAllFields(void)
{
	m_Keys.RemoveAll();
	m_Values.RemoveAll();
}
	
void CMsregPacketProductOrderRequest::AddLineItem(CMsregPacketProductOrderLineItem* pItem)
{
	m_LineItems.Add(pItem);
}

void CMsregPacketProductOrderRequest::GetLineItem(int nIndex, CMsregPacketProductOrderLineItem*& pItem)
{
	pItem = m_LineItems.GetAt(nIndex);
}

int CMsregPacketProductOrderRequest::LineItemCount(void)
{
	return m_LineItems.GetSize();
}

void CMsregPacketProductOrderRequest::DeleteAllLineItems(void)
{
	m_LineItems.RemoveAll();
}

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderResponse

CMsregPacketProductOrderResponse::CMsregPacketProductOrderResponse()
{
}

CMsregPacketProductOrderResponse::~CMsregPacketProductOrderResponse()
{
}

void CMsregPacketProductOrderResponse::ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Read inherited data.
	CMsregPacket::ReadDataFromFile(cfFile, dwChecksum);

	// Read the product name.
	m_csProductName = ReadString(cfFile, dwChecksum);

	// Read the result value.
	m_dwResult = ReadDword(cfFile, dwChecksum);

	// Read the message (normally empty if successful).
	m_csMessage = ReadString(cfFile, dwChecksum);
	
	// Read the invoice number.
	m_dwInvoiceNumber = ReadDword(cfFile, dwChecksum);
}

void CMsregPacketProductOrderResponse::WriteDataToFile(CFile& cfFile, DWORD& dwChecksum)
{
	// Write inherited data.
	CMsregPacket::WriteDataToFile(cfFile, dwChecksum);

	// Write the product name.
	WriteString(cfFile, m_csProductName, dwChecksum);

	// Write the result value.
	WriteDword(cfFile, m_dwResult, dwChecksum);

	// Write the message (normally empty if successful).
	WriteString(cfFile, m_csMessage, dwChecksum);
	
	// Write the invoice number.
	WriteDword(cfFile, m_dwInvoiceNumber, dwChecksum);
}

DWORD CMsregPacketProductOrderResponse::GetFileDataVersion(void)
{
	return VERSION_ProductOrderResponse;
}

DWORD CMsregPacketProductOrderResponse::GetFileDataType(void)
{
	return BLOCK_ProductOrderResponse;
}
