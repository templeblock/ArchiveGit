#pragma once

//typedefs and defines for CUrl (essentially the same as the ones from wininet, but with an ATL_ prepended)
typedef WORD ATL_URL_PORT;

enum ATL_URL_SCHEME 
{
	ATL_URL_SCHEME_UNKNOWN = -1, 
	ATL_URL_SCHEME_FTP     = 0,
	ATL_URL_SCHEME_GOPHER  = 1,
	ATL_URL_SCHEME_HTTP    = 2,
	ATL_URL_SCHEME_HTTPS   = 3,
	ATL_URL_SCHEME_FILE    = 4,
	ATL_URL_SCHEME_NEWS    = 5,
	ATL_URL_SCHEME_MAILTO  = 6,
	ATL_URL_SCHEME_SOCKS   = 7,
};


#define ATL_URL_MAX_HOST_NAME_LENGTH   256
#define ATL_URL_MAX_USER_NAME_LENGTH   128
#define ATL_URL_MAX_PASSWORD_LENGTH    128
#define ATL_URL_MAX_PORT_NUMBER_LENGTH 5           // ATL_URL_PORT is unsigned short
#define ATL_URL_MAX_PORT_NUMBER_VALUE  65535       // maximum unsigned short value
#define ATL_URL_MAX_PATH_LENGTH        2048
#define ATL_URL_MAX_SCHEME_LENGTH      32          // longest protocol name length
#define ATL_URL_MAX_URL_LENGTH         (ATL_URL_MAX_SCHEME_LENGTH \
									   + sizeof("://") \
									   + ATL_URL_MAX_PATH_LENGTH)

#define ATL_URL_INVALID_PORT_NUMBER    0           // use the protocol-specific default

#define ATL_URL_DEFAULT_FTP_PORT       21          // default for FTP servers
#define ATL_URL_DEFAULT_GOPHER_PORT    70          //    "     "  gopher "
#define ATL_URL_DEFAULT_HTTP_PORT      80          //    "     "  HTTP   "
#define ATL_URL_DEFAULT_HTTPS_PORT     443         //    "     "  HTTPS  "
#define ATL_URL_DEFAULT_SOCKS_PORT     1080        // default for SOCKS firewall servers.

//Flags
#define ATL_URL_ESCAPE             1   // (un)escape URL characters
#define ATL_URL_NO_ENCODE          2   // Don't convert unsafe characters to escape sequence
#define ATL_URL_DECODE             4   // Convert %XX escape sequences to characters
#define ATL_URL_NO_META            8   // Don't convert .. etc. meta path sequences
#define ATL_URL_ENCODE_SPACES_ONLY 16  // Encode spaces only
#define ATL_URL_BROWSER_MODE       32  // Special encode/decode rules for browser
#define ATL_URL_ENCODE_PERCENT     64  // Encode percent (by default, not encoded)
#define ATL_URL_CANONICALIZE       128 // Internal: used by Canonicalize for AtlEscapeUrl: Cannot be set via SetFlags
#define ATL_URL_COMBINE            256 // Internal: Cannot be set via SetFlags

#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1 
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT  1

/////////////////////////////////////////////////////////////////////////////
//Get the decimal value of a hexadecimal character
inline short AtlHexValue(char chIn) throw()
{
	unsigned char ch = (unsigned char)chIn;
	if (ch >= '0' && ch <= '9')
		return (short)(ch - '0');
	if (ch >= 'A' && ch <= 'F')
		return (short)(ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (short)(ch - 'a' + 10);
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
//Determine if the character is unsafe under the URI RFC document
inline BOOL AtlIsUnsafeUrlChar(char chIn) throw()
{
	unsigned char ch = (unsigned char)chIn;
	switch(ch)
	{
		case ';': case '\\': case '?': case '@': case '&':
		case '=': case '+': case '$': case ',': case ' ':
		case '<': case '>': case '#': case '%': case '\"':
		case '{': case '}': case '|':
		case '^': case '[': case ']': case '`':
			return TRUE;
		default:
		{
			if (ch < 32 || ch > 126)
				return TRUE;
			return FALSE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//Get the default internet port for a particular scheme
inline ATL_URL_PORT AtlGetDefaultUrlPort(ATL_URL_SCHEME m_nScheme) throw()
{
	switch (m_nScheme)
	{
		case ATL_URL_SCHEME_FTP:
			return ATL_URL_DEFAULT_FTP_PORT;
		case ATL_URL_SCHEME_GOPHER:
			return ATL_URL_DEFAULT_GOPHER_PORT;
		case ATL_URL_SCHEME_HTTP:
			return ATL_URL_DEFAULT_HTTP_PORT;
		case ATL_URL_SCHEME_HTTPS:
			return ATL_URL_DEFAULT_HTTPS_PORT;
		case ATL_URL_SCHEME_SOCKS:
			return ATL_URL_DEFAULT_SOCKS_PORT;
		default:
			return ATL_URL_INVALID_PORT_NUMBER;
	}
}

/////////////////////////////////////////////////////////////////////////////
//Escape a meta sequence with lpszOutUrl as the base url and lpszInUrl as the relative url
//i.e. lpszInUrl = ./* or ../*
inline BOOL AtlEscapeUrlMetaHelper(
	LPSTR* ppszOutUrl,
	LPCSTR szPrev,
	DWORD dwOutLen,
	LPSTR* ppszInUrl,
	DWORD* pdwLen, 
	DWORD dwFlags = 0,
	DWORD dwColonPos = ATL_URL_MAX_URL_LENGTH) throw()
{
	ATLASSERT( ppszOutUrl != NULL );
	ATLASSERT( szPrev != NULL );
	ATLASSERT( ppszInUrl != NULL );
	ATLASSERT( pdwLen != NULL);

	LPSTR szOut = *ppszOutUrl;
	LPSTR szIn = *ppszInUrl;
	DWORD dwUrlLen = dwOutLen;
	char chPrev = *szPrev;
	BOOL bRet = FALSE;

	//if the previous character is a directory delimiter
	if (chPrev == '/' || chPrev == '\\')
	{
		char chNext = *szIn;

		//if the next character is a directory delimiter
		if (chNext == '/' || chNext == '\\')
		{
			//the meta sequence is of the form /./*
			szIn++;
			bRet = TRUE;
		}
		else if (chNext == '.' && ((chNext = *(szIn+1)) == '/' || 
			chNext == '\\' || chNext == '\0'))
		{
			//otherwise if the meta sequence is of the form "/../"
			//skip the preceding "/"
			szOut--;

			//skip the ".." of the meta sequence
			szIn+= 2;
			DWORD dwOutPos = dwUrlLen-1;
			LPSTR szTmp = szOut;

			//while we are not at the beginning of the base url
			while (dwOutPos)
			{
				szTmp--;
				dwOutPos--;

				//if it is a directory delimiter
				if (*szTmp == '/' || *szTmp == '\\')
				{
					//if we are canonicalizing the url and NOT combining it
					//and if we have encountered the ':' or we are at a position before the ':'
					if ((dwFlags & ATL_URL_CANONICALIZE) && ((dwFlags & ATL_URL_COMBINE) == 0) &&
						(dwColonPos && (dwOutPos <= dwColonPos+1)))
					{
						//NOTE: this is to match the way that InternetCanonicalizeUrl and 
						//      InternetCombineUrl handle this case
						break;
					}

					//otherwise, set the current output string position to right after the '/'
					szOut = szTmp+1;

					//update the length to match
					dwUrlLen = dwOutPos+1;
					bRet = TRUE;
					break;
				}
			}

			//if we could not properly escape the meta sequence
			if (dwUrlLen != dwOutPos+1)
			{
				//restore everything to its original value
				szIn-= 2;
				szOut++;
			}
			else
			{
				bRet = TRUE;
			}
		}
	}
	//update the strings
	*ppszOutUrl = szOut;
	*ppszInUrl = szIn;
	*pdwLen = dwUrlLen;
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
//Convert all unsafe characters in szStringIn to escape sequences
//lpszStringIn and lpszStringOut should be different strings
inline BOOL AtlEscapeUrl(
	LPCSTR szStringIn,
	LPSTR szStringOut,
	DWORD* pdwStrLen,
	DWORD dwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT( szStringIn != NULL );
	ATLASSERT( szStringOut != NULL );
	ATLASSERT( szStringIn != szStringOut );

	char ch;
	DWORD dwLen = 0;
	BOOL bRet = TRUE;
	BOOL bSchemeFile = FALSE;
	DWORD dwColonPos = 0;
	DWORD dwFlagsInternal = dwFlags;
	while((ch = *szStringIn++) != '\0')
	{
		//if we are at the maximum length, set bRet to FALSE
		//this ensures no more data is written to szStringOut, but
		//the length of the string is still updated, so the user
		//knows how much space to allocate
		if (dwLen == dwMaxLength)
		{
			bRet = FALSE;
		}

		//Keep track of the first ':' position to match the weird way
		//InternetCanonicalizeUrl handles it
		if (ch == ':' && (dwFlagsInternal & ATL_URL_CANONICALIZE) && !dwColonPos)
		{
			if (bRet)
			{
				*szStringOut = '\0';
				_strlwr_s(szStringOut-dwLen, dwLen);

				if (dwLen == 4 && !strncmp("file", (szStringOut-4), 4))
				{
					bSchemeFile = TRUE;
				}
			}

			dwColonPos = dwLen+1;
		}
		else if (ch == '%' && (dwFlagsInternal & ATL_URL_DECODE))
		{
			//decode the escaped sequence
			ch = (char)(16*AtlHexValue(*szStringIn++));
			ch = (char)(ch+AtlHexValue(*szStringIn++));
		}
		else if ((ch == '?' || ch == '#') && (dwFlagsInternal & ATL_URL_BROWSER_MODE))
		{
			//ATL_URL_BROWSER mode does not encode after a '?' or a '#'
			dwFlagsInternal |= ATL_URL_NO_ENCODE;
		}

		if ((dwFlagsInternal & ATL_URL_CANONICALIZE) && (dwFlagsInternal & ATL_URL_NO_ENCODE)==0)
		{
			//canonicalize the '\' to '/'
			if (ch == '\\' && (dwColonPos || (dwFlagsInternal & ATL_URL_COMBINE)) && bRet)
			{
				//if the scheme is not file or it is file and the '\' is in "file:\\"
				//NOTE: This is to match the way InternetCanonicalizeUrl handles this case
				if (!bSchemeFile || (dwLen < 7))
				{
					ch = '/';
				}
			}
			else if (ch == '.' && dwLen > 0 && (dwFlagsInternal & ATL_URL_NO_META)==0)
			{
				//if we are escaping meta sequences, attempt to do so
				if (AtlEscapeUrlMetaHelper(&szStringOut, szStringOut-1, dwLen, (char**)(&szStringIn), &dwLen, dwFlagsInternal, dwColonPos))
					continue;
			}
		}

		//if we are encoding and it is an unsafe character
		if (AtlIsUnsafeUrlChar(ch) && (dwFlagsInternal & ATL_URL_NO_ENCODE)==0)
		{
			//if we are only encoding spaces, and ch is not a space or
			//if we are not encoding meta sequences and it is a dot or
			//if we not encoding percents and it is a percent
			if (((dwFlagsInternal & ATL_URL_ENCODE_SPACES_ONLY) && ch != ' ') ||
				((dwFlagsInternal & ATL_URL_NO_META) && ch == '.') ||
				(((dwFlagsInternal & ATL_URL_ENCODE_PERCENT) == 0) && ch == '%'))
			{
				//just output it without encoding
				if (bRet)
					*szStringOut++ = ch;
			}
			else 
			{
				//if there is not enough space for the escape sequence
				if (dwLen >= (dwMaxLength-3))
				{
					bRet = FALSE;
				}
				if (bRet)
				{
					//output the percent, followed by the hex value of the character
					LPSTR pszTmp = szStringOut;
					*pszTmp++ = '%';
					if ((unsigned char)ch < 16)
					{
						*pszTmp++ = '0';
					}
					_ultoa_s((unsigned char)ch, pszTmp, dwLen, 16);
					szStringOut+= sizeof("%FF")-1;
				}
				dwLen += sizeof("%FF")-2;
			}
		}
		else //safe character
		{
			if (bRet)
				*szStringOut++ = ch;
		}
		dwLen++;
	}

	if (bRet && dwLen < dwMaxLength)
		*szStringOut = '\0';

	if (pdwStrLen)
		*pdwStrLen = dwLen + 1;

	if (dwLen+1 > dwMaxLength)
		bRet = FALSE;

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
//Convert all escaped characters in szString to their real values
//lpszStringIn and lpszStringOut can be the same string	
inline BOOL AtlUnescapeUrl(
	LPCSTR szStringIn,
	LPSTR szStringOut,
	LPDWORD pdwStrLen,
	DWORD dwMaxLength) throw()
{
	ATLASSERT(szStringIn != NULL);
	ATLASSERT(szStringOut != NULL);

	int nValue = 0;
	char ch;
	DWORD dwLen = 0;
	BOOL bRet = TRUE;
	while ((ch = *szStringIn) != 0)
	{
		if (dwLen == dwMaxLength)
			bRet = FALSE;

		if (bRet)
		{
			if (ch == '%')
			{
				if ((*(szStringIn+1) == '\0') || (*(szStringIn+2) == '\0'))
				{
					bRet = FALSE;
					break;
				}
				ch = *(++szStringIn);
				//currently assuming 2 hex values after '%'
				//as per the RFC 2396 document
				nValue = 16*AtlHexValue(ch);
				nValue+= AtlHexValue(*(++szStringIn));
				*szStringOut++ = (char) nValue;
			}
			else //non-escape character
			{
				if (bRet)
					*szStringOut++ = ch;
			}
		}
		dwLen++;
		szStringIn++;
	}

	if (bRet && dwLen < dwMaxLength)
		*szStringOut = '\0';

	if (pdwStrLen)
		*pdwStrLen = dwLen + 1;

	if (dwLen+1 > dwMaxLength)
		bRet = FALSE;

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
//Canonicalize a URL (same as InternetCanonicalizeUrl)
inline BOOL AtlCanonicalizeUrl(
	LPCTSTR szUrl,
	LPTSTR szCanonicalized,
	DWORD* pdwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT( szUrl != NULL );
	ATLASSERT( szCanonicalized != NULL );
	ATLASSERT( pdwMaxLength != NULL);

	return AtlEscapeUrl(szUrl, szCanonicalized, pdwMaxLength, *pdwMaxLength, dwFlags | ATL_URL_CANONICALIZE);
}

/////////////////////////////////////////////////////////////////////////////
//Combine a base and relative URL (same as InternetCombineUrl)
inline BOOL AtlCombineUrl(
	LPCTSTR szBaseUrl,
	LPCTSTR szRelativeUrl,
	LPTSTR szBuffer,
	DWORD* pdwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT(szBaseUrl != NULL);
	ATLASSERT(szRelativeUrl != NULL);
	ATLASSERT(szBuffer != NULL);
	ATLASSERT(pdwMaxLength != NULL);

	size_t nLen1 = _tcslen(szBaseUrl);
	TCHAR szCombined[2*ATL_URL_MAX_URL_LENGTH];
	if (nLen1 >= 2*ATL_URL_MAX_URL_LENGTH)
	{
		return FALSE;
	}

	_tcscpy_s(szCombined, szBaseUrl);

	// if last char of szBaseUrl is not a slash, add it.
	if (nLen1 > 0 && szCombined[nLen1-1] != _T('/'))
	{
		szCombined[nLen1] = _T('/');
		nLen1++;
		szCombined[nLen1] = _T('\0');
	}

	size_t nLen2 = _tcslen(szRelativeUrl);

	if (nLen2+nLen1+1 >= 2*ATL_URL_MAX_URL_LENGTH)
	{
		return FALSE;
	}

	_tcsncpy_s(szCombined+nLen1, 2*ATL_URL_MAX_URL_LENGTH, szRelativeUrl, nLen2+1);
	DWORD dwLen = (DWORD) (nLen1+nLen2);
	if (dwLen >= *pdwMaxLength)
	{
		*pdwMaxLength = dwLen;
		return FALSE;
	}
	return AtlEscapeUrl(szCombined, szBuffer, pdwMaxLength, *pdwMaxLength, dwFlags | ATL_URL_COMBINE | ATL_URL_CANONICALIZE);
}

/////////////////////////////////////////////////////////////////////////////
inline void UnescapeUrl(const CString& strUrlIn, CString& strUrlOut)
{
	DWORD dwLength = strUrlIn.GetLength() + 1;
	DWORD dwMaxLength = dwLength;
	char* pUrlOut = new char[dwMaxLength];
	*pUrlOut = '\0';

	if (AtlUnescapeUrl(strUrlIn, pUrlOut, &dwLength, dwMaxLength))
		strUrlOut = pUrlOut;
	else
		strUrlOut = strUrlIn;

	delete [] pUrlOut;

//j	CUrl Url;
//j	BOOL bOK = Url.CrackUrl(strUrlIn);
//j	bOK = Url.CreateUrl(pUrlOut, &dwLength, ATL_URL_ESCAPE);
}

/////////////////////////////////////////////////////////////////////////////
inline void EscapeUrl(const CString& strUrlIn, CString& strUrlOut)
{
	DWORD dwLength = strUrlIn.GetLength() + 1;
	DWORD dwMaxLength = 2 * dwLength;
	char* pUrlOut = new char[dwMaxLength];
	*pUrlOut = '\0';

	if (AtlEscapeUrl(strUrlIn, pUrlOut, &dwLength, dwMaxLength))
		strUrlOut = pUrlOut;
	else
		strUrlOut = strUrlIn;

	delete [] pUrlOut;
}
