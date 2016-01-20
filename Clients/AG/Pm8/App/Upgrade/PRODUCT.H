/*
// Installation data.
//
// $Workfile: PRODUCT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PRODUCT.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:54   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:41:02   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:02   JAY
// Initial revision.
// 
//    Rev 1.4   05 Dec 1994 14:13:42   JAY
//  
// 
//    Rev 1.3   02 Dec 1994 15:29:32   JAY
// Multiple LRCs per product.
// 
//    Rev 1.2   02 Dec 1994 11:44:54   JAY
//  
// 
//    Rev 1.1   17 Nov 1994 15:51:36   JAY
// Added == operator to CProductData to clean up (and fix) some stuff.
// 
//    Rev 1.0   17 Nov 1994 14:54:46   JAY
// Initial revision.
*/

#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include "carray.h"

/*
// Structure written to the data file, one per product licensed.
// Each record is 'n' bytes long, with a 2-byte verification code appended
// at write time (see PRODUCTINFOEX).
//
// PLEASE! Don't add a virtual function to this struct. Doing so will probably
// break some code that assumes the struct is as big as its data (i.e. no
// v-table. Thanks).
//
// Types:
//		0		Normal info record (offer/product info).
*/

#pragma pack(1)
struct CProductData
{
	BYTE	m_bVersion;				/* Data version number. */
	BYTE	m_bFlags;				/* Flags for this entry/ */
	BYTE	m_bOfferCode[15];		/* Offer code for this product (string). */
	BYTE	m_bProductCode[15];	/* Product code for this product (string). */
	WORD	m_wLicenses;			/* How many licenses do we have? */

	// Flags.
	enum
	{
		CPIF_Licensed = 1
	};
};

struct CProductLicense
{
	BYTE	m_bLicenseCode[16];	/* License Request Code (string). */
};
#pragma pack()

/*
// A class to add functionality to the Data structure.
// I know I could have put the functions in the Data structure, but that
// risks adding non-member-variable data to the memory image (like v-tables).
*/

class CProductInfo : public CProductData
{
public:
	CProductInfo();
	virtual ~CProductInfo();

	/* Generate from code. */
	void FromCodes(LPCSTR pOfferCode, LPCSTR pProductCode);

	/* Generate code from us. */
	void ToCodes(LPSTR pOfferCode, LPSTR pProductCode) const;

	/* Compare codes to us. */
	BOOL EqualsCodes(LPCSTR pOfferCode, LPCSTR pProductCode) const;

	/* Return the offer code for this product (like "GOODTIM"). */
	LPCSTR OfferCode(void) const
		{ return (LPCSTR)m_bOfferCode; }

	/* Return the product code for this product (like "CAL"). */
	LPCSTR ProductCode(void) const
		{ return (LPCSTR)m_bProductCode; }

	/* Set the "licensed" state for the product. */
	void License(BOOL fLicensed = TRUE);

	/* Is the product licensed? */
	BOOL IsLicensed(void) const
		{ return (m_bFlags & CPIF_Licensed) != 0; }

	/* Add a license code for this product. */
	void AddLicenseCode(LPCSTR pLicenseCode);

	/* Compare license code to us. */
	BOOL HasLicenseCode(LPCSTR pLicenseCode) const;

	/* Read from file. */
	BOOL Read(CFile* pFile, BYTE& bEncrypt);

	/* Write to file. */
	BOOL Write(CFile* pFile, BYTE& bEncrypt);

	/* Assignment operator. */
	const CProductInfo& operator=(const CProductInfo&);

protected:
	CArray m_Licenses;
};

/*
// An array of product infos.
*/

class CProductInfoArray
{
public:
	CProductInfoArray();
	~CProductInfoArray();

	CProductInfo* Product(int nIndex) const
		{ return (CProductInfo*)m_cpaProducts.GetAt(nIndex); }
	int Products(void) const
		{ return m_cpaProducts.GetSize(); }
	void AddProduct(const CProductInfo* pProduct);
protected:
	CPtrArray m_cpaProducts;
};

#endif					// #ifndef __PRODUCT_H__
