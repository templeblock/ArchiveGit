/*
// Installation data.
//
// $Workfile: GLICENSE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/GLICENSE.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 11    10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 10    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 9     10/05/98 2:58p Johno
// Check for NULL GlobalPathManager in CLicenseData::FileName()
// 
// 8     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 7     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 6     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 5     7/28/98 4:45p Jayn
// Fixes for errors in the Art & More Store collections
// 
// 4     7/16/98 7:31p Psasse
// BETTER LOCAL BUFFER MANAGEMENT
// 
// 3     7/06/98 5:42p Rlovejoy
// Update with added project types.
// 
// 2     1/21/98 9:50a Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:21:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:36   Fred
// Initial revision.
// 
//    Rev 1.5   05 Jun 1997 14:55:30   dennisp
// Added check for empty string in ProductIsLicensed()
// 
//    Rev 1.4   28 Mar 1997 17:10:44   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.3   18 Sep 1996 10:04:30   Jay
// Fix for totems.
// 
//    Rev 1.2   16 Sep 1996 17:19:50   Jay
// Totem stuff.
// 
//    Rev 1.1   23 Apr 1996 08:27:38   Jay
//  
// 
//    Rev 1.2   19 Apr 1996 10:39:38   JAY
// Fixed last rev.
// 
//    Rev 1.1   19 Apr 1996 10:34:50   JAY
// Maps old CARTOONS category to TOONS1.
// 
//    Rev 1.0   04 Mar 1996 10:56:42   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "glicense.h"
#include "util.h"

/*
// The graphic catalog's LICENSE.DAT file.
*/

CLicenseData LicenseData;

extern WORD CodeBytesToWord(LPBYTE lpData, WORD wCount);	// in instdat.cpp

/////////////////////////////////////////////////////////////////////////////
// CLicenseData

CLicenseData::CLicenseData()
{
	m_fIsValid = FALSE;
	m_Time.Invalidate();
	ResetTotemsLicensed();
}

CLicenseData::~CLicenseData()
{
	for (int nProduct = 0; nProduct < m_cpaProducts.GetSize(); nProduct++)
	{
		delete (CProductInfo*)m_cpaProducts.GetAt(nProduct);
	}
	m_cpaProducts.RemoveAll();
}

BOOL CLicenseData::ProductIsLicensed(LPCSTR pProductCode)
{
	CString csProductCode = pProductCode;

   // If product code is empty, assume it's licensed
   if(csProductCode.IsEmpty())
      return TRUE;

	// Map product codes to upper case.
	csProductCode.MakeUpper();

	// Fix an error in the A&M spreadsheets
	// YB# maps to YB2 ("fill down" did a bad thing)
	if (csProductCode.GetLength() >= 3
			&& csProductCode[0] == 'Y'
			&& csProductCode[1] == 'B'
			&& atoi(csProductCode.Mid(2)) >= 2)
	{
		csProductCode = "YB2";
	}

	// Handle the totems here.
	if (IsTotem(csProductCode))
	{
		return IsLicensedTotem(csProductCode);
	}

	LPVOID pPtr;
	return m_Codes.Lookup(csProductCode, pPtr)
				|| GET_PMWAPP()->OurGetProfileInt("Debug", "CharmQuark", 0)		// All
				|| GET_PMWAPP()->OurGetProfileInt("License", csProductCode, 0);
}

BOOL CLicenseData::IsTotem(LPCSTR pProductCode)
{
	return strncmp(pProductCode, "TOTEM", 5) == 0;
}

BOOL CLicenseData::IsLicensedTotem(LPCSTR pProductCode)
{
#if 0
   if (m_fTotemsLicensed == -1)
	{
		// We need to determine the "licensed" status of the totems.
		// Totems are licensed if the totem album exists. We only check this
		// once. If for some reason this status can change, you will need to
		// find a reasonable place to reset the m_fTotemsLicensed variable to -1.

		CString csTotemName = GET_PMWAPP()->OurGetProfileString(SECTION_Album,
//																				  ENTRY_TotemAlbum,
																				  "TotemAlbum",
																				  GetConfiguration()->TotemPictureAlbum());
		if (csTotemName.IsEmpty())
		{
			m_fTotemsLicensed = TRUE;
		}
		else
		{
			BOOL fOldPromptForCD = path_manager.GetPromptForCD();
			path_manager.SetPromptForCD(FALSE);
			m_fTotemsLicensed = path_manager.file_exists(csTotemName);
			path_manager.SetPromptForCD(fOldPromptForCD);
		}
	}
	return m_fTotemsLicensed;
#endif
   return FALSE;
}

BOOL CLicenseData::Update(void)
{
	if (NeedsUpdating())
	{
		m_fIsValid = FALSE;

	/*
	// Open the file.
	*/
		CFile File;

		if (!File.Open(FileName(), CFile::modeRead | CFile::shareDenyWrite))
		{
		// Error!
			return FALSE;
		}

	/*
	// Read the data.
	*/

		if (!ReadData(File))
			return FALSE;

		// Update the project mask
		GET_PMWAPP()->UpdateLicensedProducts();

		// We have read something.
		m_fIsValid = TRUE;
	}
	return TRUE;
}

//
// Write the license data.
//

BOOL CLicenseData::Write(void)
{
	BOOL fResult = FALSE;
	// We must be valid to write.
	if (m_fIsValid)
	{
		// Create the file.
		CFile File;

		if (File.Open(FileName(),
						  CFile::modeReadWrite | CFile::modeCreate | CFile::shareExclusive))
		{
			// Write the data.
			fResult = WriteData(File);
		}
	}
	return fResult;
}

//
// Make sure a product is licensed.
//

void CLicenseData::LicenseProduct(LPCSTR pszProductCode, BOOL fOrderedOnline /*=FALSE*/)
{
	BOOL fFound = FALSE;
	// Look through the product array.
	for (int nProduct = 0; nProduct < m_cpaProducts.GetSize(); nProduct++)
	{
		CProductInfo* pProduct = (CProductInfo*)m_cpaProducts.GetAt(nProduct);
		if (strcmp((LPCSTR)pProduct->m_bProductCode, pszProductCode) == 0)
		{
			// This is the product.
			// Make sure it is licensed.
			pProduct->m_bFlags |= CProductData::CPIF_Licensed;
			if (fOrderedOnline)
			{
				pProduct->m_bFlags |= CProductData::CPIF_OrderedOnline;
			}

			fFound = TRUE;
			// We assume only one within the license info.
			break;
		}
	}

	// Did we find it?
	if (!fFound)
	{
		// Add a new product code for this guy.
		CProductInfo* pProduct = new CProductInfo;
		strcpy((LPSTR)pProduct->m_bProductCode, pszProductCode);
		pProduct->m_bFlags |= CProductData::CPIF_Licensed;
		if (fOrderedOnline)
		{
			pProduct->m_bFlags |= CProductData::CPIF_OrderedOnline;
		}
		m_cpaProducts.Add(pProduct);
	}

	// Add it to our codes.
	m_Codes.SetAt(pszProductCode, NULL);
}

void CLicenseData::RemoveProduct(LPCSTR pszProductCode)
{
	// Look through the product array.
	for (int nProduct = 0; nProduct < m_cpaProducts.GetSize(); nProduct++)
	{
		CProductInfo* pProduct = (CProductInfo*)m_cpaProducts.GetAt(nProduct);
		if (strcmp((LPCSTR)pProduct->m_bProductCode, pszProductCode) == 0)
		{
			// This is the product.
			m_cpaProducts.RemoveAt(nProduct);

			// Delete the product.
			delete pProduct;

			// We assume only one within the license info.
			break;
		}
	}

	// Remove it from the map.
	m_Codes.RemoveKey(pszProductCode);
}

//
// Update the licensed status of online products.
// This is used to synchronize the server with the local license info.
// The passed map identifies which products are licensed.
//

void CLicenseData::UpdateOnlineProducts(const CMapStringToPtr& LicensedMap, BOOL fLicensedOnly /*=FALSE*/)
{
	// Make sure we are current before doing this.
	Update();

	if (!IsValid())
	{
		// Not initialized yet. Do it now.
		MakeBlank();
	}

	if (!fLicensedOnly)
	{
		// First, handle the unlicensed products.
		for (int nProduct = 0; nProduct < m_cpaProducts.GetSize(); nProduct++)
		{
			CProductInfo* pProduct = (CProductInfo*)m_cpaProducts.GetAt(nProduct);
			if (pProduct->m_bFlags & CProductData::CPIF_OrderedOnline)
			{
				// A product ordered online. See if it is currently licensed.
				LPCSTR pszProductCode = (LPCSTR)pProduct->m_bProductCode;
				LPVOID p;
				if (!LicensedMap.Lookup(pszProductCode, p))
				{
					// Remove the key.
					m_Codes.RemoveKey(pszProductCode);

					// Not licensed!
					m_cpaProducts.RemoveAt(nProduct);

					// Delete the product.
					delete pProduct;

					// Account for the incredible shrinking array.
					nProduct--;
				}
			}
		}
	}

	// Next, handle all the licensed products.
	POSITION pos = LicensedMap.GetStartPosition();
	while (pos != NULL)
	{
		// Get the next licensed product.
		CString csKey;
		LPVOID p;
		LicensedMap.GetNextAssoc(pos, csKey, p);

		// License this product.
		LicenseProduct(csKey, TRUE);
	}

	// And write this out.
	Write();
}

// Convert length-encoded string to zero-terminated string.
static void l2z(LPSTR d, LPBYTE s)
{
	int nLength = (int)s[0];
	if (nLength > 0)
	{
		memcpy(d, s+1, nLength);
	}
	d[nLength] = 0;
}

/*
// Encrypt a string using "random" data.
*/

static void EncryptString(LPBYTE lpData, LPBYTE lpRandomData)
{
	WORD i;

	for (i = 1; i <= (WORD)(*lpData); i++)
	{
		lpData[i] ^= lpRandomData[i];
	}
}

/*
// Read the license data.
*/

BOOL CLicenseData::ReadData(CFile& File)
{
	BOOL fResult = FALSE;
	LICENSEDAT Data;

	TRY
	{
		if (File.Read(&Data, sizeof(Data)) == sizeof(Data))
		{
			if (DecodeInstallationData(&Data))
			{
			/*
			// We have decoded the data. Extract the pertinent data into our
			// variables.
			*/

				l2z(m_szSerialNumber, Data.bSerialNumber);

				m_bTimeStamp[0] = Data.bTimeStamp[0];
				m_bTimeStamp[1] = Data.bTimeStamp[1];
				m_bTimeStamp[2] = Data.bTimeStamp[2];
				m_bTimeStamp[3] = Data.bTimeStamp[3];

				m_wOrderSequence = Data.wOrderSequence;

			/*
			// Read the products which have been licensed.
			// Products are stored delta'd one to the next (via xor).
			// The first product is stored delta'd with FIRSTPRODUCT_ENCODING.
			*/

				fResult = TRUE;

				BYTE bEncoding = FIRSTPRODUCT_ENCODING ^ (BYTE)Data.wProductCount;

				od("Product: %d\r\n", Data.wProductCount);

				for (WORD w = 0; w < Data.wProductCount; w++)
				{
					CProductInfo* pProductInfo = new CProductInfo;

					if (!pProductInfo->Read(&File, bEncoding))
					{
						delete pProductInfo;
						fResult = FALSE;
						break;
					}

					// Add it to our list.
					m_cpaProducts.Add(pProductInfo);

					// See if it is a licensed product.
					if (pProductInfo->m_bFlags & CProductData::CPIF_Licensed)
					{
						// Old cartoons category maps to Toons 1.
						if (stricmp((LPCSTR)pProductInfo->m_bProductCode, "CAR") == 0)
						{
							strcpy((LPSTR)pProductInfo->m_bProductCode, "TOA1");
						}

						// Add this to our map. The actual pointer is irrelevant.
						m_Codes.SetAt((LPCSTR)pProductInfo->m_bProductCode, NULL);
					}
				}
			}
		}
	}
	END_TRY

	return fResult;
}

/*
// Write the license data.
*/

BOOL CLicenseData::WriteData(CFile& File)
{
	LICENSEDAT Data;

	static char* pszCopyright =
		"Product Licenses. Copyright 1998 Mindscape, Inc.%c";

	sprintf((LPSTR)Data.bCopyright, pszCopyright, 0x1a);

	strcpy((char *)Data.bSerialNumber+1, m_szSerialNumber);
	Data.bSerialNumber[0] = strlen((char *)Data.bSerialNumber+1);

	Data.bTimeStamp[0] = m_bTimeStamp[0];
	Data.bTimeStamp[1] = m_bTimeStamp[1];
	Data.bTimeStamp[2] = m_bTimeStamp[2];
	Data.bTimeStamp[3] = m_bTimeStamp[3];

	WORD wProductCount = m_cpaProducts.GetSize();
	Data.wProductCount = wProductCount;
	Data.wOrderSequence = m_wOrderSequence;

	EncodeInstallationData(&Data);

	/*
	// Attempt to create the file and write out the installation record.
	*/

	BOOL fResult = FALSE;

	TRY
	{
		File.Write(&Data, sizeof(Data));

		/*
		// Write all the products.
		// Data is stored as deltas (xor generated). The first product
		// is generated delta'd with FIRSTPRODUCT_ENCODING.
		*/

		fResult = TRUE;

		BYTE bEncoding = FIRSTPRODUCT_ENCODING ^ (BYTE)wProductCount;

		for (WORD w = 0; w < wProductCount; w++)
		{
			CProductInfo* pInfo = (CProductInfo*)m_cpaProducts.GetAt(w);

			if (!pInfo->Write(&File, bEncoding))
			{
				fResult = FALSE;
				break;
			}
		}
	}
	END_TRY

	return fResult;
}

/*
// Decode the installation data.
*/

BOOL CLicenseData::DecodeInstallationData(LICENSEDAT* pData)
{
	if (pData->wVersion != 0)
	{
		return FALSE;
	}

	for (int i = 0; i < sizeof(pData->bReserved); i++)
	{
		if (pData->bReserved[i] != 0)
		{
			return FALSE;
		}
	}

	char szSerialNumber[sizeof(pData->bSerialNumber)];

	if (pData->wCheckSum != CodeBytesToWord((LPBYTE)pData, sizeof(*pData)-sizeof(pData->wCheckSum)))
	{
		return FALSE;
	}

	EncryptString(pData->bSerialNumber, pData->bRandomData);

	l2z(szSerialNumber, pData->bSerialNumber);

	pData->wProductCount ^= PRODUCTCOUNT_ENCODING;

	return TRUE;
}

/*
// Encode some installation data.
*/

void CLicenseData::EncodeInstallationData(LICENSEDAT* pData)
{
	WORD i;
	BYTE bRandom;

	pData->wVersion = 0;

	for (i = 0; i < sizeof(pData->bReserved); i++)
	{
		pData->bReserved[i] = 0;
	}

	for (i = pData->bSerialNumber[0]+1; i < sizeof(pData->bSerialNumber); i++)
	{
		pData->bSerialNumber[i] = bRandom; bRandom = bRandom*3+13;
	}

	for (i = 0; i < sizeof(pData->bRandomData); i++)
	{
		pData->bRandomData[i] = bRandom; bRandom = bRandom*7+1;
	}

	EncryptString(pData->bSerialNumber, pData->bRandomData);

	pData->wProductCount ^= PRODUCTCOUNT_ENCODING;
	pData->wCheckSum = CodeBytesToWord((LPBYTE)pData, sizeof(*pData)-sizeof(pData->wCheckSum));
}

/*
// Get the name of the license file.
*/

LPCSTR CLicenseData::FileName(void)
{
	if (m_csFileName.IsEmpty())
	{
		// Construct the path to the LICENSE.DAT file.
		// Get the global shared directory.
		IPathManager* pPathManager = GetGlobalPathManager();
		if (pPathManager != NULL)
		{
			CString csDefaultPath = pPathManager->ExpandPath("[[s]]");

			CString csPath = csDefaultPath;

			// Allow INI file override.
			IUserSettings* pUserSettings = GetGlobalUserSettings();
			if (pUserSettings != NULL)
			{
				csPath = pUserSettings->GetString("Paths", "LicensePath", csDefaultPath);
			}

			// Make sure the directory exists.
			Util::MakeDirectory(csPath);

			// Construct the final LICENSE.DAT filename.
			Util::ConstructPath(m_csFileName, csPath, "LICENSE.DAT");
		}
		else
			m_csFileName = "";
	}
	return m_csFileName;
}

/*
// Does the license data need updating (from the file).
*/

BOOL CLicenseData::NeedsUpdating(void)
{
	CTimeStamp Time;
	if (!Time.SetFileModify(FileName()))
	{
		// Can't get modify time! What to do!
		return FALSE;
	}

	BOOL fNeedsUpdating = (!m_Time.IsValid()) || (m_Time != Time);
	m_Time = Time;

	return fNeedsUpdating;
}

//
// Set blank data into the license data file in preparation for a first write.
//

void CLicenseData::MakeBlank(void)
{
#ifdef WIN32
	FILETIME Time;
	GetSystemTimeAsFileTime(&Time);
	DWORD dwTime = Time.dwHighDateTime^Time.dwLowDateTime;
	ASSERT(sizeof(m_bTimeStamp) == sizeof(dwTime));
	memcpy(&(m_bTimeStamp[0]), &dwTime, sizeof(dwTime));
#else
	time_t Time;
	time(&Time);
	ASSERT(sizeof(m_bTimeStamp) == sizeof(Time));
	memcpy(&(m_bTimeStamp[0]), &Time, sizeof(Time));
#endif

	m_bTimeStamp[0] |= 1;
	m_bTimeStamp[1] |= 1;
	m_bTimeStamp[2] |= 1;
	m_bTimeStamp[3] |= 1;

	// Build the serial number by hand (overkill?)
	m_szSerialNumber[0]  = 'P';
	m_szSerialNumber[1]  = 'M';
	m_szSerialNumber[2]  = 'W';
	m_szSerialNumber[3]  = '1';
	m_szSerialNumber[4]  = '0';
	m_szSerialNumber[5]  = '-';
	m_szSerialNumber[6]  = '0';
	m_szSerialNumber[7]  = '1';
	m_szSerialNumber[8]  = 'Y';
	m_szSerialNumber[9]  = 'F';
	m_szSerialNumber[10] = '5';
	m_szSerialNumber[11] = '-';
	m_szSerialNumber[12] = '4';
	m_szSerialNumber[13] = '6';
	m_szSerialNumber[14] = '2';
	m_szSerialNumber[15] = '3';
	m_szSerialNumber[16] = 'N';
	m_szSerialNumber[17] = 'W';
	m_szSerialNumber[18] = '\0';

	m_wOrderSequence = 0;

	m_fIsValid = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Helpers

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

/////////////////////////////////////////////////////////////////////////////
// CProductInfo

CProductInfo::CProductInfo()
{
	m_bVersion = 0;
	m_bFlags = 0;
	m_bOfferCode[0] = '\0';
	m_bProductCode[0] = '\0';
	m_wLicenses = 0;
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

			m_Licenses.Add(License);
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
			for (int i = 0; i < nCount; i++)
			{
				CProductLicense License = m_Licenses.GetAt(i);
	
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
