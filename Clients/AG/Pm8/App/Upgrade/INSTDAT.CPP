/*
// Installation data.
//
// $Workfile: INSTDAT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/INSTDAT.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     1/21/98 4:14p Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:41:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:16   Fred
// Initial revision.
// 
//    Rev 1.2   12 Jun 1997 16:30:42   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.1   28 Oct 1996 11:07:24   johno
// Moved strings to resource file
// 
//    Rev 1.0   08 Sep 1996 09:40:28   Fred
// Initial revision.
// 
//    Rev 1.2   20 Mar 1996 13:03:58   FRED
// Fixed "PMINST.DAT across network" bug.
// 
//    Rev 1.1   12 Feb 1996 14:03:54   JAY
// Now reads the main PMINST.DAT (without writing it).
// This allows us to have custom time stamps to keep the LRCs random.
// 
//    Rev 1.0   02 Feb 1996 08:58:22   JAY
// Initial revision.
// 
//    Rev 1.15   01 Mar 1995 11:21:28   FRED
// Licensed product list no longer depends on offer code.
// 
// 
//    Rev 1.14   24 Feb 1995 11:12:02   JAY
//  
// 
//    Rev 1.13   14 Dec 1994 17:22:04   JAY
// New product scheme
// 
//    Rev 1.12   08 Dec 1994 14:53:18   FRED
// 
// 
//    Rev 1.11   08 Dec 1994 10:13:00   JAY
// Now includes offer code in acces code.
// 
//    Rev 1.10   05 Dec 1994 16:06:30   JAY
// 
//    Rev 1.9   05 Dec 1994 14:41:52   JAY
// Sequence must now be bumped by hand.
// 
//    Rev 1.8   05 Dec 1994 14:13:34   JAY
//  
// 
//    Rev 1.7   05 Dec 1994 11:38:24   JAY
//  
// 
//    Rev 1.6   02 Dec 1994 15:29:32   JAY
// Multiple LRCs per product.
// 
//    Rev 1.5   02 Dec 1994 11:45:14   JAY
//  
// 
//    Rev 1.4   17 Nov 1994 15:51:54   JAY
// Added == operator to CProductData to clean up (and fix) some stuff.
// 
//    Rev 1.3   17 Nov 1994 14:30:18   JAY
//  
// 
//    Rev 1.2   11 Nov 1994 17:03:58   JAY
// Official version of LRC/LAC code.
// 
//    Rev 1.1   10 Nov 1994 08:09:32   JAY
//  
// 
//    Rev 1.0   09 Nov 1994 15:59:04   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "instdat.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// Encoding data.

/*
// String encoding parameters.
*/

static const CODEPARAMETERS BASED_CODE CodeParameters_1 =	// Parameters for 1 letter encoding
{
	26,			// Range
	0x000f,		// Mask
	59,			// Prime
	7777			// Seed
};

static const CODEPARAMETERS BASED_CODE CodeParameters_2 =	// Parameters for 2 letter encoding
{
	26*26,		// Range
	0x01ff,		// Mask
	811,			// Prime
	7777			// Seed
};

static const CODEPARAMETERS BASED_CODE CodeParameters_3 =	// Parameters for 3 letter encoding
{
	26*26*26,	// Range
	0x3fff,		// Mask
	21101,		// Prime
	7777			// Seed
};

static const char BASED_CODE SerialNumber_Template[] = "AAA##-##AXN-XXXXAA";
static const char BASED_CODE LAC_Template[] = "AAA-AAA";

/////////////////////////////////////////////////////////////////////////////
// CInstallationData

CInstallationData::CInstallationData()
{
}

CInstallationData::~CInstallationData()
{
}

// Initialization.
BOOL CInstallationData::Init(const CString& csOfferCode, DWORD dwKey)
{
	BOOL fResult = FALSE;

	ASSERT(sizeof(dwKey) == sizeof(m_bTimeStamp));
	memcpy(&(m_bTimeStamp[0]), &dwKey, sizeof(dwKey));

	m_bTimeStamp[0] |= 1;
	m_bTimeStamp[1] |= 1;
	m_bTimeStamp[2] |= 1;
	m_bTimeStamp[3] |= 1;

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

	m_csOfferCode = csOfferCode;

	return TRUE;
}

/*
// Return whether a product is licensed or not.
*/

CProductInfo* CInstallationData::FindProduct(LPCSTR pProductCode, LPCSTR pOfferCode /*=NULL*/) const
{
/* Fix up the offer code. */
	if (pOfferCode == NULL)
	{
		pOfferCode = m_csOfferCode;
	}

/* Run through all the products looking for it. */

	int nProducts = m_Products.Products();

	for (int nProduct = 0; nProduct < nProducts; nProduct++)
	{
		CProductInfo* pProduct = m_Products.Product(nProduct);

		if (pProduct->EqualsCodes(pOfferCode, pProductCode))
		{
			return pProduct;
		}
	}
	return NULL;
}

/*
// Record a product as being ordered.
*/

CProductInfo* CInstallationData::AddProduct(LPCSTR pProductCode)
{
	CProductInfo* pProduct;

/* See if the product exists. */

	if ((pProduct = FindProduct(pProductCode)) == NULL)
	{
	/* Normal case. Product does not exist so we must create it. */

		CProductInfo Info;

		Info.FromCodes(m_csOfferCode, pProductCode);

		m_Products.AddProduct(&Info);
		pProduct = FindProduct(pProductCode);
	}

	return pProduct;
}

BOOL CInstallationData::ProductIsLicensed(LPCSTR pProductCode) const
{
	CProductInfo* pProduct;

	if ((pProduct = FindProduct(pProductCode)) != NULL)
	{
		return pProduct->IsLicensed();
	}
	return FALSE;
}

/*
// License all products with the specified code.
// TRUE is returned if any products match the license code (not necessarily
// whether any products changed state to being licensed from not licensed).
*/

BOOL CInstallationData::LicenseProducts(LPCSTR pLAC)
{
/* Compute the license request code for this access code. */

	char LicenseCode[30];
	LACtoLRC(pLAC, LicenseCode);

/* Now run through and license all products matching the license code. */

	BOOL fLicensed = FALSE;
	int nProducts = Products();

	for (int i = 0; i < nProducts; i++)
	{
		CProductInfo* pProduct = Product(i);
		if (pProduct != NULL
//				&& pProduct->OfferCode() == m_csOfferCode
				&& pProduct->HasLicenseCode(LicenseCode))
		{
		/* License code matches. License this! */
			pProduct->License();
			fLicensed = TRUE;
		}
	}
	return fLicensed;
}

/*
// License the product with corresponding product code if it has the given
// request code. TRUE is returned of the product qualifies for licensing.
*/

BOOL CInstallationData::LicenseProduct(LPCSTR pProductCode, LPCSTR pLRC)
{
	CProductInfo* pInfo = FindProduct(pProductCode);
	if (pInfo != NULL)
	{
		if (pInfo->HasLicenseCode(pLRC))
		{
			pInfo->License();
			return TRUE;
		}
	}
	return FALSE;
}

/*
// Encrypt a string using "random" data.
*/

void CInstallationData::EncryptString(LPBYTE lpData, LPBYTE lpRandomData)
{
	WORD i;

	for (i = 1; i <= (WORD)(*lpData); i++)
	{
		lpData[i] ^= lpRandomData[i];
	}
}

WORD CInstallationData::CodeString(LPCSTR lpszString, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed)
{
	return (CodeBytes((LPBYTE)lpszString, strlen(lpszString), wRange, wMask, wPrime, wSeed));
}

WORD CInstallationData::CodeBytesToWord(LPBYTE lpData, WORD wCount)
{
	return (CodeBytes(lpData, wCount, 0xffff, 0x7fff, 1, 7777));
}

/*
// CodeBytes
//
// Given a block of data, generates a 16-bit hash value, which is intended 
// to be non-obvious so that the following operations on the string are 
// unlikely to preserve the hash value:
//
//		Incrementing one character
//		Incrementing one and decrementing its neighbor
//		Exchanging two characters
//
// Arguments:
//
//		lpData	-		pointer to data bytes to encode
//		wCount	-		number of bytes to encode
// 	wRange	-		the result will be in the range [0..wRange-1]
//		wMask		-		the upper 16 bits of MASK where MASK is the largest
//							2^N-1 such that MASK/wRange <= 65535
//		wPrime	-		an arbitrary prime number that should be
//							greater than wRange and not nearly a mulitple
//							of wrange/2
//		wSeed		-		anything other than zero
*/

#pragma warning(disable:4035)

WORD CInstallationData::CodeBytes(const BYTE* lpData, WORD wCount, WORD wRange, WORD wMask, WORD wPrime, WORD wSeed)
{
	WORD retval;
	_asm
	{	
		mov	bx,wSeed				; BX = Checksum <-- seed

#ifdef _WIN32
		mov	esi,lpData			; ESI <-- pointer to data

		movsx	ecx,wCount			; CX <-- count
		jecxz	done

#else
		push	si
		push	ds

		cld

		lds	si,lpData			; DS:SI <-- pointer to data

		mov	cx,wCount			; CX <-- count
		jcxz	done

#endif

		xor	ax,ax

next:

		lodsb							; AL <-- next byte
		add	bx,ax					; Add...

		mov 	dl,al					; ...and scramble
		and 	dl,00fh
		xchg 	dl,cl
		rol 	bx,cl
		xchg 	dl,cl

		loop	next

done:
										; BX now contains the checksum
		mov	ax,bx					; AX <-- checksum
		mul	wPrime
		and	dx,wMask				; prevent divide overflow
		div	wRange				; DX <-- remainder=code
		mov	retval,dx			; return value

#ifndef _WIN32
		pop	ds
		pop	si
#endif
	}
	return retval;
}

#pragma warning(default:4035)

void CInstallationData::Delay(WORD wTicks)
{
	DWORD wStartTime;
	DWORD wCurrentTime;

	wStartTime = GetTickCount();

	do
	{
		wCurrentTime = ::GetTickCount();
	} while (wCurrentTime >= wStartTime && wCurrentTime < wStartTime + wTicks);
}

#pragma warning(disable:4035)

LPSTR CInstallationData::l2z(LPSTR d, LPBYTE s)
{
	_asm
	{
#ifdef _WIN32
		mov	edi,d
		mov	esi,s

		lodsb
		xor	ah, ah
		movzx	ecx, ax

		rep	movsb

		xor	al, al
		stosb
#else
		push	si
		push	di
		push	ds

		les	di,d
		lds	si,s

		lodsb
		mov	cl,al
		xor	ch,ch

		rep	movsb

		xor	al, al
		stosb

		pop	ds
		pop	di
		pop	si
#endif
	}
	return d;
}

#pragma warning(default:4035)

BOOL CInstallationData::ValidateSerialNumber(LPSTR lpszSerialNumber)
{
	return (ValidateString(lpszSerialNumber, SerialNumber_Template)
		  && ValidateCodeString(lpszSerialNumber, 2));
}

BOOL CInstallationData::ValidateString(LPCSTR lpszString, LPCSTR lpszTemplate)
{
	if (strlen(lpszString) != strlen(lpszTemplate))
	{
		return FALSE;
	}

	while (*lpszString != 0)
	{
		char c;
		char t;

		c = *(lpszString++);
		t = *(lpszTemplate++);

		switch (t)
		{
			case 'A':
			{
				if ((c < 'A') || (c > 'Z')) return FALSE;
				break;
			}

			case '#':
			{
				if ((c < '0') || (c > '9')) return FALSE;
				break;
			}

			case 'X':
			{
				if (((c < '0') || (c > '9')) && ((c < 'A') && (c > 'F'))) return FALSE;
				break;
			}

			case 'N':
			{
				if (((c < '0') || (c > '9')) && ((c < 'A') && (c > 'Z'))) return FALSE;
				break;
			}

			default:
			{
				if (c != t) return FALSE;
				break;
			}
		}
	}

	return TRUE;
}

BOOL CInstallationData::ValidateCodeString(LPSTR lpszString, WORD Length)
{
	BOOL wResult;

	wResult = FALSE;

	if (strlen(lpszString) > Length)
	{
		char szString[100];
		char szCode[100];
		char szNewCode[100];

		/*
		// We validate the coding by reencoding the string and
		// comparing the results;
		*/

		substr(szString, lpszString, 0, strlen(lpszString)-Length);
		substr(szCode, lpszString, strlen(lpszString)-Length, Length);

		EncodeString(szString, Length, szNewCode);

		wResult = (strcmp(szCode, szNewCode) == 0);
	}

	return (wResult);
}

void CInstallationData::EncodeString(LPCSTR lpszString, WORD Length, LPSTR lpszCode)
{
	const CODEPARAMETERS* lpCodeParameters;
	WORD wCode;

	*lpszCode = 0;

	switch (Length)
	{
		case 1:
		{
			lpCodeParameters = &CodeParameters_1;
			break;
		}

		case 2:
		{
			lpCodeParameters = &CodeParameters_2;
			break;
		}

		case 3:
		{
			lpCodeParameters = &CodeParameters_3;
			break;
		}

		default:
		{
			return;
		}
	}

	wCode = CodeString(
					lpszString,
					lpCodeParameters->wRange,
					lpCodeParameters->wMask,
					lpCodeParameters->wPrime,
					lpCodeParameters->wSeed);

	StringBase26(wCode, Length, lpszCode);
}

#pragma warning(default:4035)

DWORD CInstallationData::ValueBase26(LPSTR lpszString)
{
	DWORD dwResult;

	dwResult = 0;

	while (*lpszString != 0)
	{
		BYTE ch;

		ch = *(lpszString++);

		dwResult *= 26;
		dwResult += ch-'A';
	}

	return dwResult;
}

void CInstallationData::StringBase26(DWORD dwNumber, WORD wLength, LPSTR lpszString)
{
	WORD i;

	*(lpszString+wLength) = 0;

	i = wLength;

	while (i-- > 0)
	{
		BYTE bDigit;

		bDigit = (BYTE)(dwNumber%26);
		*(lpszString+i) = bDigit+'A';
		dwNumber /= 26;
	}
}

#pragma warning(disable:4035)

LPSTR CInstallationData::substr(LPSTR d, LPCSTR s, WORD c, WORD l)
{
	_asm
	{
#ifdef _WIN32
		mov	edi,d
		mov	esi,s

		movsx eax, c
		add	esi,eax

		movsx	ecx,l

		rep	movsb

		xor	al, al
		stosb

#else
		push	si
		push	di
		push	ds

		cld

		les	di,d
		lds	si,s

		add	si,c

		mov	cx,l

		rep	movsb

		xor	al, al
		stosb

		pop	ds
		pop	di
		pop	si
#endif
	}
	return d;
}

#pragma warning(default:4035)

/////////////////////////////////////////////////////////////////////////////
// LicenseRequest code.

BOOL CInstallationData::GetLicenseRequestCode(char cProductEncoding, LPSTR lpszLicenseRequestCode)
{
	char szData[40];
	LPSTR d = szData;

/*
// Generate a 3-character code:
//
// ABC
//
// A = single-character encoding of machine time stamp & sequence
// B = single-character encoding of products being ordered
// C = single-character encoding (checksum) of entire string
//     (i.e. OOOOOOO-AB)
*/

	*(d++) = m_bTimeStamp[0];
	*(d++) = m_bTimeStamp[1];
	*(d++) = m_bTimeStamp[2];
	*(d++) = m_bTimeStamp[3];
	*(d++) = (char)m_wOrderSequence;				/* Can be 0. That's OK. */
//	TRACE("Sequence == %u\n", m_wOrderSequence);

	*(d++) = 0;

	char szMachineCode[1+1];
	char szChecksum[1+1];

	EncodeString((LPSTR)szData, 1, (LPSTR)szMachineCode);
	sprintf(szData, "%s-%c%c",
				(LPCSTR)m_csOfferCode,
				*szMachineCode,
				cProductEncoding);

	EncodeString(szData, 1, szChecksum);

	lpszLicenseRequestCode[0] = *szMachineCode;
	lpszLicenseRequestCode[1] = cProductEncoding;
	lpszLicenseRequestCode[2] = *szChecksum;
	lpszLicenseRequestCode[3] = '\0';

#ifdef _DEBUG
	// A test!
	char szLAC[40];
	LRCtoLAC(lpszLicenseRequestCode, szLAC);
	TRACE("LRC: %s; LAC: %s\n",
					lpszLicenseRequestCode,
					szLAC);
#endif

	return TRUE;
}

BOOL CInstallationData::ValidateLicenseAccessCode(LPCSTR lpszLicenseAccessCode)
{
	BOOL wResult;

	wResult = FALSE;

	if (ValidateString(lpszLicenseAccessCode, LAC_Template))
	{
		char szUserLRC[20];
		char szUserLAC[20];

		/* Get the LRC implied by the user's LAC */
		LACtoLRC(lpszLicenseAccessCode, szUserLRC);

		/* See if this is the correct LAC for this LRC */
		LRCtoLAC(szUserLRC, szUserLAC);
		wResult = (strcmp(szUserLAC, lpszLicenseAccessCode) == 0);
	}

	return (wResult);
}

void CInstallationData::LACtoLRC(LPCSTR lpszLAC, LPSTR lpszLRC)
{
	// Take the first 3 characters of XXX-XXX
	substr(lpszLRC, lpszLAC, 0, 3);
}

void CInstallationData::LRCtoLAC(LPCSTR lpszLRC, LPSTR lpszLAC)
{
	char buffer[80];

	sprintf(buffer, "%s-%s-", (LPCSTR)m_csOfferCode, lpszLRC);
	char Code[3+1];

	EncodeString(buffer, 3, Code);
	wsprintf(lpszLAC, "%s-%s", lpszLRC, Code);
}
