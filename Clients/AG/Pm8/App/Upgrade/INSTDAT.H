/*
// Installation data.
//
// $Workfile: INSTDAT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/INSTDAT.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     1/21/98 4:14p Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:41:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:40   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:30:44   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.0   08 Sep 1996 09:40:50   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:00   JAY
// Initial revision.
// 
//    Rev 1.9   14 Dec 1994 17:22:06   JAY
// New product scheme
// 
//    Rev 1.8   08 Dec 1994 10:12:50   JAY
// Now includes offer code in acces code.
// 
//    Rev 1.7   05 Dec 1994 14:41:34   JAY
// Sequence must now be bumped by hand.
// 
//    Rev 1.6   05 Dec 1994 11:38:36   JAY
//  
// 
//    Rev 1.5   02 Dec 1994 15:29:34   JAY
// Multiple LRCs per product.
// 
//    Rev 1.4   02 Dec 1994 11:45:12   JAY
//  
// 
//    Rev 1.3   17 Nov 1994 14:30:12   JAY
//  
// 
//    Rev 1.2   11 Nov 1994 17:04:10   JAY
// Official version of LRC/LAC code.
// 
//    Rev 1.1   10 Nov 1994 08:09:34   JAY
//  
// 
//    Rev 1.0   09 Nov 1994 15:59:04   JAY
// Initial revision.
*/

#ifndef __INSTDAT_H__
#define __INSTDAT_H__

#include "product.h"

/*
// Structure for holding the parameters for string encoding.
*/

typedef struct
{
	WORD wRange;
	WORD wMask;
	WORD wPrime;
	WORD wSeed;
} CODEPARAMETERS;

#pragma pack()

class CInstallationData
{
public:
/* Construction/Destruction. */

	CInstallationData();
	virtual ~CInstallationData();

	// Initialization.
	BOOL Init(const CString& csOfferCode, DWORD dwKey);

/* Get and Put support. */

	BOOL Get(LPCSTR pInstName);
//	BOOL Put(LPCSTR pInstName = NULL);

/* Ordered product support. */

	// Has a product been ordered?
	CProductInfo* FindProduct(LPCSTR pProductCode, LPCSTR pOfferCode = NULL) const;
	CProductInfo* AddProduct(LPCSTR pProductCode);

	// Has a product been licensed?
	BOOL ProductIsLicensed(LPCSTR pProductCode) const;

	// License all products matching the specified License Access Code.
	BOOL LicenseProducts(LPCSTR pLAC);
	BOOL LicenseProduct(LPCSTR pProductCode, LPCSTR pLRC);

	int Products(void) const
		{ return m_Products.Products(); }
	CProductInfo* Product(int nProduct)
		{ return m_Products.Product(nProduct); }

//	void BumpSequence(void)
//		{ m_wOrderSequence++; }

/*
// LRC/LAC support.
*/

	BOOL GetLicenseRequestCode(char cProductEncoding, LPSTR lpszLicenseRequestCode);
	BOOL ValidateLicenseAccessCode(LPCSTR lpszLicenseAccessCode);

/* For testing... */

	void LACtoLRC(LPCSTR lpszLAC, LPSTR lpszLRC);
	void LRCtoLAC(LPCSTR lpszLRC, LPSTR lpszLAC);

protected:

//	BOOL Open(CFile& File, LPCSTR pInstName);
//	BOOL ReadData(CFile& File);

/*
// Data.
*/

	char m_szSerialNumber[32];			// Serial number to use.
	BYTE m_bTimeStamp[4];				// Time stamp of file.
	CProductInfoArray m_Products;		// Products licensed.
	CString m_csOfferCode;				// Offer code in effect.
	WORD m_wOrderSequence;				// Order sequence number for LRCs.

/*
// Locally invoked functions.
*/

//	static void EncodeInstallationData(INSTDATA* pData);
//	static BOOL DecodeInstallationData(INSTDATA* pData, BOOL fVirginOK);

	static void EncryptString(LPBYTE lpData, LPBYTE lpRandomData);
	static WORD CodeString(LPCSTR lpszString, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed);
	static WORD CodeBytesToWord(LPBYTE lpData, WORD wCount);
	static WORD CodeBytes(const BYTE* lpData, WORD wCount, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed);
	static void Delay(WORD wTicks);
	static LPSTR l2z(LPSTR d, LPBYTE s);

	static BOOL ValidateSerialNumber(LPSTR lpszSerialNumber);
	static BOOL ValidateString(LPCSTR lpszString, LPCSTR lpszTemplate);
	static BOOL ValidateCodeString(LPSTR lpszString, WORD Length);
	static void EncodeString(LPCSTR lpszString, WORD Length, LPSTR lpszCode);

	static DWORD ValueBase26(LPSTR lpszString);
	static void StringBase26(DWORD dwNumber, WORD wLength, LPSTR lpszString);
	static LPSTR substr(LPSTR d, LPCSTR s, WORD c, WORD l);
};

#define PRODUCTCOUNT_ENCODING		0xa55a
#define FIRSTPRODUCT_ENCODING		0xcd

#endif					// #ifndef __INSTDAT_H__
