/*
// Installation data.
//
// $Workfile: GLICENSE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/GLICENSE.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 7     10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 6     10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 5     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 4     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 3     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 2     1/21/98 9:50a Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:21:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:00   Fred
// Initial revision.
// 
//    Rev 1.2   16 Sep 1996 17:19:50   Jay
// Totem stuff.
// 
//    Rev 1.1   01 Sep 1996 12:41:38   Jay
// More configurable albums
// 
//    Rev 1.0   14 Mar 1996 13:34:16   Jay
// Initial revision.
// 
//    Rev 1.0   06 Mar 1996 10:28:26   Jay
// Changes from 2.1
// 
//    Rev 1.0   04 Mar 1996 10:56:44   JAY
// Initial revision.
*/

#ifndef __GLICENSE_H__
#define __GLICENSE_H__

#include "timestmp.h"

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
		CPIF_Licensed = 1,
		CPIF_OrderedOnline = 2
	};
};

struct CProductLicense
{
	BYTE	m_bLicenseCode[16];	/* License Request Code (string). */
};

/*
// A class to add functionality (and members) to the Data structure.
*/

class CProductInfo : public CProductData
{
public:
	CProductInfo();

	/* Read from file. */
	BOOL Read(CFile* pFile, BYTE& bEncrypt);

	/* Write to file. */
	BOOL Write(CFile* pFile, BYTE& bEncrypt);

protected:
	CArray<CProductLicense,CProductLicense&> m_Licenses;
};

/*
// Structure of LICENSE.DAT file (header).
*/

typedef struct
{
	BYTE bCopyright[96];
	WORD wVersion;
	BYTE bReserved[30];
	BYTE bSerialNumber[33];
	BYTE bRandomData[182];
	BYTE bTimeStamp[4];
	WORD wProductCount;
	WORD wOrderSequence;
	WORD wCheckSum;
} LICENSEDAT;
#pragma pack()

class CLicenseData
{
public:
	CLicenseData();
	virtual ~CLicenseData();

	BOOL Update(void);
	BOOL ProductIsLicensed(LPCSTR pProductCode);

	void ResetTotemsLicensed(void)
		{ m_fTotemsLicensed = -1; }

	LPCSTR FileName(void);

	BOOL IsValid(void) const
		{ return m_fIsValid; }

	void MakeBlank(void);

	BOOL Write(void);

	void LicenseProduct(LPCSTR pszProductCode, BOOL fOrderedOnline = FALSE);
	void RemoveProduct(LPCSTR pszProductCode);

	// Update online products. Used to synchronize this list with the server.
	void UpdateOnlineProducts(const CMapStringToPtr& LicensedMap, BOOL fLicensedOnly = FALSE);

protected:
	BOOL NeedsUpdating(void);
	void EncodeInstallationData(LICENSEDAT* pData);
	BOOL DecodeInstallationData(LICENSEDAT* pData);
	BOOL ReadData(CFile& File);
	BOOL WriteData(CFile& File);

	BOOL IsTotem(LPCSTR pProductCode);
	BOOL IsLicensedTotem(LPCSTR pProductCode);

protected:
	BOOL m_fIsValid;

	CString m_csFileName;
	CTimeStamp m_Time;

	CMapStringToPtr m_Codes;
	BOOL m_fTotemsLicensed;

	char m_szSerialNumber[32];			// Serial number to use.
	BYTE m_bTimeStamp[4];				// Time stamp of file.
	WORD m_wOrderSequence;				// Order sequence number for LRCs.
	CPtrArray m_cpaProducts;
};

#define PRODUCTCOUNT_ENCODING		0xa55a
#define FIRSTPRODUCT_ENCODING		0xcd

/*
// The graphic catalog's LICENSE.DAT file.
*/

extern CLicenseData LicenseData;

#endif
