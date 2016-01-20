/*
// $Workfile: MSPACKET.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSPACKET.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:28   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:44   johno
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
// $Log: /PM8/App/LICDLL/MSPACKET.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:54   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:48   JAY
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
//    Rev 1.4   30 Oct 1995 17:21:14   FRED
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

class CMsregPacket;

#ifndef __MSPACKET_H__
#define __MSPACKET_H__

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Base Class for all Packets ////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacket

class CMsregPacket
{
public:

	// supported block types
	enum
	{
		// general purpose blocks
		BLOCK_PacketHeader					=		1,

		// client generated blocks
		BLOCK_RegistrationRequest			=		1000,
		BLOCK_CatalogOrderRequest			=		1001,
		BLOCK_ProductOrderRequest			=		1002,

		// server generated blocks
		BLOCK_RegistrationResponse			=		2000,
		BLOCK_CatalogOrderResponse			=		2001,
		BLOCK_ProductOrderResponse			=		2002,
	};

	// current block version numbers
	enum
	{
		// general purpose blocks
		VERSION_PacketHeader					=		1,

		// client generated blocks
		VERSION_RegistrationRequest		=		1,
		VERSION_CatalogOrderRequest		=		1,
		VERSION_ProductOrderRequest		=		1,

		// server generated blocks
		VERSION_RegistrationResponse		=		1,
		VERSION_CatalogOrderResponse		=		1,
		VERSION_ProductOrderResponse		=		1
	};

	// standard block header, not included in checksum
	struct StandardBlockHeaderType
	{
		DWORD dwVersion;			// Block schema version number
		DWORD dwType;				// Block type
		DWORD dwSize;				// Block size
		DWORD dwChecksum;			// Checksum of block (sum of bytes excluding header)

		// other block fields follow immediately
	};

public:

	// construction and destruction
	CMsregPacket();
	virtual ~CMsregPacket();

public:

	// functions for initiating reading and writing packet to file
	void ReadFromFile(CFile& cfFile);
	void WriteToFile(CFile& cfFile);

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void) = 0;
	virtual DWORD GetFileDataType(void) = 0;

public:

	// useful helper functions

	static void ReadData(CFile& cfFile, void* pBuffer, UINT uCount);
	static void WriteData(CFile& cfFile, const void* pBuffer, UINT uCount);

	static void ReadData(CFile& cfFile, void* pBuffer, UINT uCount, DWORD& dwChecksum);
	static void WriteData(CFile& cfFile, const void* pBuffer, UINT uCount, DWORD& dwChecksum);

	static void ChecksumData(const void* pBuffer, UINT uCount, DWORD& dwChecksum);

	static CMsregPacket* ReadPacket(CFile& cfFile);
	static void WritePacket(CFile& cfFile, CMsregPacket* pPacket);

	static BYTE ReadByte(CFile& cfFile, DWORD& dwChecksum);
	static void WriteByte(CFile& cfFile, BYTE bByte, DWORD& dwChecksum);

	static char ReadChar(CFile& cfFile, DWORD& dwChecksum);
	static void WriteChar(CFile& cfFile, char cChar, DWORD& dwChecksum);

	static WORD ReadWord(CFile& cfFile, DWORD& dwChecksum);
	static void WriteWord(CFile& cfFile, WORD wWord, DWORD& dwChecksum);

	static DWORD ReadDword(CFile& cfFile, DWORD& dwChecksum);
	static void WriteDword(CFile& cfFile, DWORD dwDword, DWORD& dwChecksum);

	static CString ReadString(CFile& cfFile, DWORD& dwChecksum);
	static void WriteString(CFile& cfFile, LPCSTR pszString, DWORD& dwChecksum);

	static DWORD ReadStringLength(CFile& cfFile, DWORD& dwChecksum);
	static void WriteStringLength(CFile& cfFile, LPCSTR pszString, DWORD& dwChecksum);

public:

	// data management
	DWORD Version(void)
		{ return m_Header.dwVersion; }
	DWORD Type(void)
		{ return m_Header.dwType; }

protected:
	
	// data
	StandardBlockHeaderType m_Header;
};


///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Utiltity Packets //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketHeader

class CMsregPacketHeader : public CMsregPacket
{
public:

	// construction and destruction
	CMsregPacketHeader();
	virtual ~CMsregPacketHeader();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management

	CString& CustomerId(void)
		{ return m_csCustomerId; }

	void AddPacket(CMsregPacket* pPacket);
	CMsregPacket* GetPacket(int nIndex);
	int PacketCount(void);
	void DeleteAllPackets(void);

protected:

	// data
	CString m_csCustomerId;
	CPtrArray m_Packets;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Registration Packets ////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketRegistrationRequest

class CMsregPacketRegistrationRequest : public CMsregPacket
{
public:

	// construction and destruction
	CMsregPacketRegistrationRequest();
	virtual ~CMsregPacketRegistrationRequest();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management
	CString& ProductName(void)
		{ return m_csProductName; }

	void AddField(LPCSTR pszKey, LPCSTR pszValue);
	void GetField(int nIndex, CString& csKey, CString& csValue);
	int FieldCount(void);
	void DeleteAllFields(void);

protected:

	// data
	CString m_csProductName;
	CStringArray m_Keys;
	CStringArray m_Values;
};

/////////////////////////////////////////////////////////
// CMsregPacketRegistrationResponse

class CMsregPacketRegistrationResponse : public CMsregPacket
{
public:

	// construction and destruction
	CMsregPacketRegistrationResponse();
	virtual ~CMsregPacketRegistrationResponse();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management
	CString& ProductName(void)
		{ return m_csProductName; }
	DWORD& Registered(void)
		{ return m_dwRegistered; }
	CString& Message(void)
		{ return m_csMessage; }

protected:

	// data
	CString m_csProductName;
	DWORD m_dwRegistered;
	CString m_csMessage;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Catalog Order Packets ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderLineItem

class CMsregPacketCatalogOrderLineItem
{
public:

	// construction and destruction
	CMsregPacketCatalogOrderLineItem();
	virtual ~CMsregPacketCatalogOrderLineItem();
	
public:

	// virtual functions for reading and writing block to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

public:

	// data management
	CString& Category(void)
		{ return m_csCategory; }
	DWORD& Quantity(void)
		{ return m_dwQuantity; }
	DWORD& Cost(void)
		{ return m_dwCost; }

	void AddCollection(LPCSTR pszCollection);
	void GetCollection(int nIndex, CString& csCollection);
	int CollectionCount(void);
	void DeleteAllCollections(void);

protected:

	// data
	CString m_csCategory;
	DWORD m_dwQuantity;
	DWORD m_dwCost;
	CStringArray m_Collections;
};

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderLineItemArray

class CMsregPacketCatalogOrderLineItemArray : public CPtrArray
{
public:

	// construction and destruction
	CMsregPacketCatalogOrderLineItemArray();
	virtual ~CMsregPacketCatalogOrderLineItemArray();
	
public:

	// virtual functions for reading and writing block to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

public:

	void RemoveAll();
	CMsregPacketCatalogOrderLineItem* GetAt(int nIndex) const
		{ return (CMsregPacketCatalogOrderLineItem*)(CPtrArray::GetAt(nIndex)); }
	CMsregPacketCatalogOrderLineItem*& ElementAt(int nIndex)
		{ return (CMsregPacketCatalogOrderLineItem*&)(CPtrArray::ElementAt(nIndex)); }
	void RemoveAt(int nIndex, int nCount = 1, BOOL fDelete = TRUE);
	CMsregPacketCatalogOrderLineItem* operator[](int nIndex) const
		{ return (CMsregPacketCatalogOrderLineItem*)(CPtrArray::GetAt(nIndex)); }
	CMsregPacketCatalogOrderLineItem*& operator[](int nIndex)
		{ return (CMsregPacketCatalogOrderLineItem*&)(CPtrArray::ElementAt(nIndex)); }
};

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderRequest

class CMsregPacketCatalogOrderRequest : public CMsregPacket
{
public:

	// construction and destruction
	CMsregPacketCatalogOrderRequest();
	virtual ~CMsregPacketCatalogOrderRequest();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management
	CString& ProductName(void)
		{ return m_csProductName; }

	void AddField(LPCSTR pszKey, LPCSTR pszValue);
	void GetField(int nIndex, CString& csKey, CString& csValue);
	int FieldCount(void);
	void DeleteAllFields(void);
	
	DWORD& Tax(void)
		{ return m_dwTax; }
	DWORD& Shipping(void)
		{ return m_dwShipping; }
	DWORD& Total(void)
		{ return m_dwTotal; }
	CString& OfferCode(void)
		{ return m_csOfferCode; }
	CString& RequestCode(void)
		{ return m_csRequestCode; }

	void AddLineItem(CMsregPacketCatalogOrderLineItem* pItem);
	void GetLineItem(int nIndex, CMsregPacketCatalogOrderLineItem*& pItem);
	int LineItemCount(void);
	void DeleteAllLineItems(void);

protected:

	// data
	CString m_csProductName;
	CStringArray m_Keys;
	CStringArray m_Values;
	DWORD m_dwTax;
	DWORD m_dwShipping;
	DWORD m_dwTotal;
	CString m_csOfferCode;
	CString m_csRequestCode;
	CMsregPacketCatalogOrderLineItemArray m_LineItems;
};

/////////////////////////////////////////////////////////
// CMsregPacketCatalogOrderResponse

class CMsregPacketCatalogOrderResponse : public CMsregPacket
{
public:
	// Responses to catalog order request.
	enum
	{
		CATALOGORDERRESULT_Ok											=	1,
		CATALOGORDERRESULT_Error										=	2,
		CATALOGORDERRESULT_InvalidCreditCardNumber				=	3,
		CATALOGORDERRESULT_CreditCardExpired						=	4,
		CATALOGORDERRESULT_IncompleteCustomerInformation		=	5
	};
		
public:

	// construction and destruction
	CMsregPacketCatalogOrderResponse();
	virtual ~CMsregPacketCatalogOrderResponse();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management
	CString& ProductName(void)
		{ return m_csProductName; }
	DWORD& Result(void)
		{ return m_dwResult; }
	CString& Message(void)
		{ return m_csMessage; }
	CString& AccessCode(void)
		{ return m_csAccessCode; }
	DWORD& InvoiceNumber(void)
		{ return m_dwInvoiceNumber; }

protected:

	// data
	CString m_csProductName;
	DWORD m_dwResult;
	CString m_csMessage;
	CString m_csAccessCode;
	DWORD m_dwInvoiceNumber;
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Product Order Packets ///////////////////////////
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderLineItem

class CMsregPacketProductOrderLineItem
{
public:

	// construction and destruction
	CMsregPacketProductOrderLineItem();
	virtual ~CMsregPacketProductOrderLineItem();
	
public:

	// virtual functions for reading and writing block to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

public:

	// data management
	WORD& Type(void)
		{ return m_wType; }
	CString& Category(void)
		{ return m_csCategory; }
	DWORD& Quantity(void)
		{ return m_dwQuantity; }
	DWORD& Cost(void)
		{ return m_dwCost; }

	void AddCollection(LPCSTR pszCollection);
	void GetCollection(int nIndex, CString& csCollection);
	int CollectionCount(void);
	void DeleteAllCollections(void);
	
public:

	// line item types
	enum
	{
		LINEITEMTYPE_Product			=	1,
		LINEITEMTYPE_Information	=	2
	};

protected:

	// data
	WORD m_wType;
	CString m_csCategory;
	DWORD m_dwQuantity;
	DWORD m_dwCost;
	CStringArray m_Collections;
};

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderLineItemArray

class CMsregPacketProductOrderLineItemArray : public CPtrArray
{
public:

	// construction and destruction
	CMsregPacketProductOrderLineItemArray();
	virtual ~CMsregPacketProductOrderLineItemArray();
	
public:

	// virtual functions for reading and writing block to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

public:

	void RemoveAll();
	CMsregPacketProductOrderLineItem* GetAt(int nIndex) const
		{ return (CMsregPacketProductOrderLineItem*)(CPtrArray::GetAt(nIndex)); }
	CMsregPacketProductOrderLineItem*& ElementAt(int nIndex)
		{ return (CMsregPacketProductOrderLineItem*&)(CPtrArray::ElementAt(nIndex)); }
	void RemoveAt(int nIndex, int nCount = 1, BOOL fDelete = TRUE);
	CMsregPacketProductOrderLineItem* operator[](int nIndex) const
		{ return (CMsregPacketProductOrderLineItem*)(CPtrArray::GetAt(nIndex)); }
	CMsregPacketProductOrderLineItem*& operator[](int nIndex)
		{ return (CMsregPacketProductOrderLineItem*&)(CPtrArray::ElementAt(nIndex)); }
};

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderRequest

class CMsregPacketProductOrderRequest : public CMsregPacket
{
public:

	// construction and destruction
	CMsregPacketProductOrderRequest();
	virtual ~CMsregPacketProductOrderRequest();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management
	CString& ProductName(void)
		{ return m_csProductName; }

	void AddField(LPCSTR pszKey, LPCSTR pszValue);
	void GetField(int nIndex, CString& csKey, CString& csValue);
	int FieldCount(void);
	void DeleteAllFields(void);
	
	DWORD& Tax(void)
		{ return m_dwTax; }
	DWORD& Shipping(void)
		{ return m_dwShipping; }
	DWORD& Total(void)
		{ return m_dwTotal; }
	CString& OfferCode(void)
		{ return m_csOfferCode; }
	DWORD& ShippingMethod(void)
		{ return m_dwShippingMethod; }

	void AddLineItem(CMsregPacketProductOrderLineItem* pItem);
	void GetLineItem(int nIndex, CMsregPacketProductOrderLineItem*& pItem);
	int LineItemCount(void);
	void DeleteAllLineItems(void);
	
public:

	// available shipping methods
	enum
	{
		SHIPPINGMETHOD_Default			=		1,
		SHIPPINGMETHOD_USPriority		=		2			// "two day" priority mail
	};

protected:

	// data
	CString m_csProductName;
	CStringArray m_Keys;
	CStringArray m_Values;
	DWORD m_dwTax;
	DWORD m_dwShipping;
	DWORD m_dwTotal;
	CString m_csOfferCode;
	DWORD m_dwShippingMethod;
	CMsregPacketProductOrderLineItemArray m_LineItems;
};

/////////////////////////////////////////////////////////
// CMsregPacketProductOrderResponse

class CMsregPacketProductOrderResponse : public CMsregPacket
{
public:
	// Responses to Product order request.
	enum
	{
		PRODUCTORDERRESULT_Ok											=	1,
		PRODUCTORDERRESULT_Error										=	2,
		PRODUCTORDERRESULT_InvalidCreditCardNumber				=	3,
		PRODUCTORDERRESULT_CreditCardExpired						=	4,
		PRODUCTORDERRESULT_IncompleteCustomerInformation		=	5
	};
		
public:

	// construction and destruction
	CMsregPacketProductOrderResponse();
	virtual ~CMsregPacketProductOrderResponse();

protected:

	// virtual functions for reading and writing block data to file
	virtual void ReadDataFromFile(CFile& cfFile, DWORD& dwChecksum);
	virtual void WriteDataToFile(CFile& cfFile, DWORD& dwChecksum);

	// virtual functions used to fill in the header
	virtual DWORD GetFileDataVersion(void);
	virtual DWORD GetFileDataType(void);

public:

	// data management
	CString& ProductName(void)
		{ return m_csProductName; }
	DWORD& Result(void)
		{ return m_dwResult; }
	CString& Message(void)
		{ return m_csMessage; }
	DWORD& InvoiceNumber(void)
		{ return m_dwInvoiceNumber; }

protected:

	// data
	CString m_csProductName;
	DWORD m_dwResult;
	CString m_csMessage;
	DWORD m_dwInvoiceNumber;
};

#endif
