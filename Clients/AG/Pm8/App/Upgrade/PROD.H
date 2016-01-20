/*
// $Workfile: PROD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PROD.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:54   Fred
// Initial revision.
// 
//    Rev 1.2   09 Jul 1997 09:55:34   Fred
// Moved strings to resource file
// 
//    Rev 1.1   12 Jun 1997 16:30:46   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.0   08 Sep 1996 09:41:00   Fred
// Initial revision.
// 
//    Rev 1.2   24 Apr 1996 16:42:34   JAY
// Now gets PPN from entry. Demo handling.
// 
//    Rev 1.1   12 Feb 1996 13:53:46   JAY
// Added product level param for correct product
// 
//    Rev 1.0   02 Feb 1996 08:58:58   JAY
// Initial revision.
// 
//    Rev 1.25   28 Feb 1995 09:15:10   JAY
// Added NotLicensedAndNotFree to CProductItem.
// 
//    Rev 1.24   27 Feb 1995 11:13:48   FRED
// More multi-level object stuff. Added type flags to CProductItem
// 
//    Rev 1.23   24 Feb 1995 14:35:04   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.22   21 Jan 1995 10:54:24   FRED
// Changes to make free items listed at
// the end of the phone order list box.
// 
// 
//    Rev 1.21   19 Jan 1995 14:20:50   JAY
// New free pictures!
// 
//    Rev 1.20   16 Jan 1995 15:53:22   FRED
// Uses CATALOG.INF instead of OFFER.INI.
// 
// 
//    Rev 1.19   04 Jan 1995 11:30:22   JAY
// Selective installation.
// 
//    Rev 1.18   21 Dec 1994 14:57:06   JAY
//  
// 
//    Rev 1.17   16 Dec 1994 17:09:28   FRED
//  
// 
//    Rev 1.16   15 Dec 1994 14:46:16   JAY
// Selective install
// 
//    Rev 1.15   15 Dec 1994 13:11:20   JAY
//  
// 
//    Rev 1.14   15 Dec 1994 09:48:54   FRED
// Added CProductItem::IsLicensed().
// 
// 
//    Rev 1.13   15 Dec 1994 09:04:48   JAY
//  
// 
//    Rev 1.12   14 Dec 1994 15:26:38   FRED
//  
// 
//    Rev 1.11   14 Dec 1994 11:33:54   FRED
//  
// 
//    Rev 1.10   13 Dec 1994 18:29:14   FRED
//  
// 
//    Rev 1.9   11 Dec 1994 10:56:50   FRED
//  
// 
//    Rev 1.8   06 Dec 1994 10:40:08   FRED
//  
// 
//    Rev 1.7   05 Dec 1994 17:02:02   JAY
// Added InstallSelect() and InstallSelected().
// 
//    Rev 1.6   05 Dec 1994 16:35:56   FRED
// 
//    Rev 1.5   05 Dec 1994 15:27:56   JAY
// Fixed Products()
// 
//    Rev 1.4   05 Dec 1994 09:43:26   JAY
// CProductForSale is now derived from CObject so we can do dynamic creation of
// derived classes.
// 
//    Rev 1.3   02 Dec 1994 14:50:58   FRED
//  
// 
//    Rev 1.2   02 Dec 1994 11:45:06   JAY
//  
// 
//    Rev 1.1   02 Dec 1994 08:37:32   FRED
//  
// 
//    Rev 1.0   01 Dec 1994 14:43:38   FRED
// Initial revision.
*/

/*
// Offer/Product information class structure:
//
//		COfferInformation
//
//			This class encapsulates the information from the
//			file OFFER.INI. This includes information about
//			the specific offer including the offer code,
//			phone numbers, etc. It also includes an array
//			(CProductsForSale) that holds pointers to the
//			objects (CProductForSale) describing each product
//			being offered.
//
//		CProductsForSale
//
//			This is an array of pointers to CProductForSale
//			objects (actually, derivatives thereof). The objects
//			are listed in the array in the same order as they
//			are listed on the Products= line of OFFER.INI.
//
//		CProductForSale
//
//			This object completely describes a product (and
//			any subproducts) that is for sale as part of the
//			catalog offer. Eventually, this class will be a
//			wrapper around a DLL that encapsulates the product.
//
//			One method of CProductForSale returns a pointer to
//			the top of the product item tree (CProductItem) for
//			that product.
//
//			The CProductForSale object also supplies the
//			information to the catalog program used to build
//			the catalog categories and pages.
//
//		CProductItems, CProductItem
//
//			These object are used to build a tree of product items.
//			This tree is used to display and allow the user to make
//			selections in the tree. Each CProductItem contains
//			information about the product including the name,
//			price, and required disk space.
*/

// prod.h : Defines the base class representing a product.
//

class CProductItems;
class CProductItem;
class COfferInformation;
class CInstallationData;
class CInstallInfo;

#ifndef __PROD_H__
#define __PROD_H__

class CSimpleCategory;

#define	PRODUCT_ITEM_Marked				(0x00000001L)
#define	PRODUCT_ITEM_Selected			(0x00000002L)
#define	PRODUCT_ITEM_Licensed			(0x00000004L)
#define	PRODUCT_ITEM_Install				(0x00000008L)
#define	PRODUCT_ITEM_Free					(0x00000010L)

#define PRODUCT_FLAGS_Transient (PRODUCT_ITEM_Marked|PRODUCT_ITEM_Selected)

#define	PRODUCT_TYPE_Graphics			(0x00000001L)
#define	PRODUCT_TYPE_Fonts				(0x00000002L)
#define	PRODUCT_TYPE_Software			(0x00000004L)

class CProductItems : public CPtrArray
{

public:

	typedef BOOL (CProductItems::*PPRODUCTITEMSTESTFN)(CProductItem* pItem);

public:

	CProductItems();
	virtual ~CProductItems();

	virtual int ItemCount(void) const
		{ return GetSize(); }
	virtual CProductItem* Item(int nIndex) const
		{ return (CProductItem*)(GetAt(nIndex)); }
		
	virtual void ClearProductFlags(DWORD dwValue = PRODUCT_FLAGS_Transient);
	virtual int CountProductFlags(DWORD dwFlags = 0xffffffffL, CProductItems::PPRODUCTITEMSTESTFN pTestFn = NULL);
	
	virtual BOOL NotLicensed(CProductItem* pItem);
	virtual BOOL NotLicensedAndNotFree(CProductItem* pItem);

	virtual BOOL UpdateLicenseFlags(const CInstallationData* pData);

	CProductItem* LookupItem(LPCSTR pszCode);
};

class CProductItem : public CObject
{
	DECLARE_DYNCREATE(CProductItem)

public:

	CProductItem();
	CProductItem(LPCSTR pszCode, LPCSTR pszName, LPCSTR pszCollectionCode, LPCSTR pszCollectionName, long lPrice, unsigned long lType);
	virtual ~CProductItem();
	
public:

	virtual const CString& Code(void) const
		{ return m_csCode; }
	virtual const CString& Name(void) const
		{ return m_csName; }
	virtual const CString& CollectionCode(void) const
		{ return m_csCollectionCode; }
	virtual const CString& CollectionName(void) const
		{ return m_csCollectionName; }
	virtual long Price(void) const
		{ return m_lPrice; }
	
	virtual CProductItems* Items(void)
		{ return &m_Items; }
		
	virtual DWORD Flags(void) const
		{ return m_dwFlags; }
	virtual void Flags(DWORD dwFlags)
		{ m_dwFlags = dwFlags; }
	virtual BOOL TestFlag(DWORD dwFlag) const
		{ return (Flags() & dwFlag) != 0; }
	virtual void SetFlag(DWORD dwFlag)
		{ Flags(Flags() | dwFlag); }
	virtual void ClearFlag(DWORD dwFlag)
		{ Flags(Flags() & ~dwFlag); }
		
	virtual BOOL HasGraphics(void)
		{ return (m_lType & PRODUCT_TYPE_Graphics) != 0; }
	virtual BOOL HasFonts(void)
		{ return (m_lType & PRODUCT_TYPE_Fonts) != 0; }

	virtual BOOL UpdateLicenseFlags(const CInstallationData* pData);
	virtual BOOL IsLicensed(void);
	virtual BOOL NotLicensedAndNotFree(void);
	CProductItem* LookupItem(LPCSTR pszCode);

	// Installation support.
	
	virtual int Install(CInstallInfo& Info);
	
	// Selective install support.

	virtual void Pieces(int nPieces);
	virtual int Pieces(void) const;
	virtual void SelectPiece(int nIndex, BOOL fSelect = TRUE);
	virtual BOOL PieceIsSelected(int nIndex) const;
	virtual void SelectAllPieces(BOOL fSelect = TRUE);
	virtual int SelectedPieces(void) const;
	virtual void UserChoosePieces(CInstallInfo* pInfo);
	virtual void Category(CSimpleCategory* pCategory);
	virtual CSimpleCategory* Category(void) const;

protected:

	DWORD m_dwFlags;
	CString m_csCode;
	CString m_csName;
	CString m_csCollectionCode;
	CString m_csCollectionName;
	long m_lPrice;
	unsigned long m_lType;

	int m_nPieces;
	CByteArray m_cbaSelectedPieces;
	CSimpleCategory* m_pCategory;

	CProductItems m_Items;
};

class COfferInformation : public CProductItems
{

public:

	COfferInformation();
	virtual ~COfferInformation();
	
	virtual void Init(const CString& csDataDirectory, int nProductLevel, DWORD dwPPN);
	
	virtual const CString& OfferCode(void) const
		{ return m_csOfferCode; }
	virtual long ProcessingFee(void) const
		{ return m_lProcessingFee; }
		
protected:

	CString m_csOfferCode;
	long m_lProcessingFee;

	BOOL m_fInitialized;
};

#endif
	