/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef WIN32
#include <windows.h>
#endif

#include <ctype.h>
#include <limits.h>
#include <stdlib.h> 
#include <stdio.h>
#include "framelib.h"
#include "macros.h"
#include "imgutil.h"

class CCPInfo
{
public:
	BYTE m_bLBRange[MAX_LEADBYTES];	//	Table of lead-byte ranges, depending on code page
	BOOL m_fDBCS;					//	Are we running on a double-byte system?
private:
	void InitCPInfo(); 		// 	Initializes data members upon construction
public:
	CCPInfo();
	~CCPInfo();
};

CCPInfo CPInfo;

CCPInfo::CCPInfo()
{
	InitCPInfo();
}

CCPInfo::~CCPInfo()
{
}

/************************************************************************/
void CCPInfo::InitCPInfo() 
/************************************************************************/
{
	CPINFO CPInfo;

	if( GetCPInfo( CP_ACP, &CPInfo ))
	{
		// Check current code page to determine if DBCS enabled
		memcpy( m_bLBRange, CPInfo.LeadByte, MAX_LEADBYTES );
		m_fDBCS = ( CPInfo.MaxCharSize > 1 );
	}
	else
	{
		// default to single-byte system
		m_bLBRange[0] = 0;
		m_fDBCS = FALSE;
	}
}

/************************************************************************/
LPTSTR MyCharNext( LPTSTR pszStr ) 
// optimized version of CharNext()
/************************************************************************/
{
	BYTE bRange = 0;

	if(( CPInfo.m_bLBRange[0] == 0 ) || ( *pszStr < CPInfo.m_bLBRange[0] ))
		return( pszStr + 1 );

	// check to see if *pszStr is a lead byte
	while(( bRange < MAX_LEADBYTES ) && ( CPInfo.m_bLBRange[bRange] != NULL ))
	{
		if(( *pszStr >= CPInfo.m_bLBRange[bRange] ) && ( *pszStr <= CPInfo.m_bLBRange[bRange+1] ))
			return( pszStr + 2 );	// skip two bytes
		else
			bRange += 2;			// go to the next pair of range values
	}
	return( pszStr + 1 );
}

/************************************************************************/
LPTSTR MyCharPrev( LPTSTR pszStart, LPTSTR pszStr ) 
// optimized version of CharPrev()
/************************************************************************/
{
	if( !CPInfo.m_bLBRange || CPInfo.m_bLBRange[0] == NULL )
	{
		if( pszStr > pszStart )
			return( pszStr - 1 );
		else
			return pszStart;
	}

	LPTSTR pszCurrent = pszStr;

	// Special case: test the first byte to the left
	pszStr--;
	if( !IsDBCSLeadByte( *pszStr ))
	{
		pszStr--;
		if( IsDBCSLeadByte( *pszStr ))
			return( pszStr );  			// found a double-byte char
		else
			return( pszCurrent - 1 );	// found a single-byte char
	}

	// Step backward until a true non-lead byte is found
	while( IsDBCSLeadByte( *pszStr ) && ( pszStr != pszStart ))
		pszStr--;

	// pszStr points to a non-lead byte
	if(( pszCurrent - pszStr ) % 2 ) 	// all characters in-between are DBCS
		return( pszCurrent - 2 );
	else
		return( pszCurrent - 1 );		// previous character is single byte
}

/***********************************************************************/
LPTSTR MyCharNextN( LPTSTR pszStr, UINT uPos )
// returns a pointer to the Nth + 1 multibyte character in a string.
// uPos is treated as an array index where uPos == 1 returns a pointer to
// the second multi-byte character in the string.
/***********************************************************************/
{
    if( !pszStr || ( MBStrlen( pszStr ) < uPos ))
        return 0;

    for( int i = 0; i < uPos; i++ )
        pszStr = MyCharNext( pszStr ); 

    return( pszStr );
}

/***********************************************************************/
UINT MBStrlen( LPTSTR pszStr )
// returns the number of MB characters in a string
/***********************************************************************/
{
    UINT uCount = 0;
    LPTSTR psz = pszStr;

    while( *psz && ++uCount )
        psz = MyCharNext( psz );

    return uCount;
}

/***********************************************************************/
BYTE MBToUpper( LPTSTR pszChar )
// Converts a single byte character in a buffer to upper case
/***********************************************************************/
{
    if( !pszChar )
        return 0;

    CharUpperBuff( pszChar, sizeof( TCHAR ));
    return *(( LPBYTE )pszChar );
}

/***********************************************************************/
BYTE MBToLower( LPTSTR pszChar )
// Converts a single byte character in a buffer to lower case
/***********************************************************************/
{
    if( !pszChar )
        return 0;

    CharLowerBuff( pszChar, sizeof( TCHAR ));
    return *(( LPBYTE )pszChar );
}

/***********************************************************************/
WORD MBToUpperW( LPTSTR pszChar )
// Converts a double byte character in a buffer to upper case
/***********************************************************************/
{
    if( !pszChar )
        return 0;

    CharUpperBuff( pszChar, sizeof( WORD ));
    return *(( LPWORD )pszChar );
}

/***********************************************************************/
WORD MBToLowerW( LPTSTR pszChar )
// Converts a double byte character in a buffer to lower case
/***********************************************************************/
{
    if( !pszChar )
        return 0;

    CharLowerBuff( pszChar, sizeof( WORD ));
    return *(( LPWORD )pszChar );
}

/***********************************************************************/
LPTSTR MBStrchr( LPTSTR pszStr, WORD wChar )
// returns a pointer to the first occurrence of a character in the string.
/***********************************************************************/
{
    if( !pszStr || !wChar )
        return( NULL );

    while( *pszStr )
    {
        if( IsDBCSLeadByte(( BYTE )*pszStr ))
        {
            if( *(( LPWORD )pszStr ) == wChar )
                return( pszStr );
        }
        else
        {
            if( *pszStr == wChar )
                return( pszStr );
        }
        pszStr = MyCharNext( pszStr );
    }
    return( NULL );
}

/***********************************************************************/
LPTSTR MBStrrchr( LPTSTR pszStr, WORD wChar )
// returns a pointer to the last occurrence of a character in the string.
// NULL if wChar not found
/***********************************************************************/
{
    if( !pszStr || !wChar )
        return( NULL );

    LPTSTR pszFound = NULL;

    while( *pszStr )
    {
        if( IsDBCSLeadByte(( BYTE )*pszStr ))
        {
            if( *(( LPWORD )pszStr ) == wChar )
                pszFound = pszStr;
        }
        else
        {
            if( *pszStr == wChar )
                pszFound = pszStr;
        }
        pszStr = MyCharNext( pszStr );
    }
    return( pszFound );  // NULL if no match
}

/***********************************************************************/
UINT MBStrcpyN( LPTSTR pszBuffer, LPTSTR pszStr, UINT uCount )
// copies no more than uCount single-byte or multi-byte characters from 
// pszStr into pszBuffer. The number of characters copied is returned.
/***********************************************************************/
{
	*pszBuffer = _T('\0');

    if( !pszStr || !uCount )
        return( 0 );

	LPTSTR pszBuf, pszCurrent;
	UINT uCopied = 0;

	pszBuf = pszBuffer;
	pszCurrent = pszStr;

    while( *pszCurrent && uCopied < uCount )
    {
        if( IsDBCSLeadByte(( BYTE )*pszCurrent ))
            *(( LPWORD )pszBuf ) = *(( LPWORD )pszCurrent );
        else
            *(( LPBYTE )pszBuf ) = *(( LPBYTE )pszCurrent );

		uCopied++; 
        pszCurrent = MyCharNext( pszCurrent );
        pszBuf = MyCharNext( pszBuf );
    }
	*pszBuf = _T('\0');
    return( uCopied );
}

/************************************************************************/
BOOL StringsEqual( LPTSTR pszStr1, LPTSTR pszStr2 )
/************************************************************************/
{
    int	i = lstrcmpi( pszStr1, pszStr2 );
    return( i ? NO : YES );
}

/************************************************************************/
LOCAL void TerminateString( LPTSTR pszStr, int len )
/************************************************************************/
{
	while( len > 0 )
    {
        len -= ( IsDBCSLeadByte(( BYTE )*pszStr ) ? sizeof( WORD ):sizeof( TCHAR ));
		pszStr = MyCharNext( pszStr );
    }
	*pszStr = _T('\0');
}

/************************************************************************/
BOOL StringsEqualN( LPTSTR pszStr1, LPTSTR pszStr2, int len )
/************************************************************************/
{
    int     len1, len2;
    FNAME	psz1, psz2;

    len1 = lstrlen( pszStr1 );
    len2 = lstrlen( pszStr2 );

    if( len1 < len || len2 < len )
	    return( NO );

    lstrcpy( psz1, pszStr1 );
    TerminateString( psz1, len );

    lstrcpy( psz2, pszStr2 );
    TerminateString( psz2, len );

    return StringsEqual( psz1, psz2 );
}

/************************************************************************/
LPTSTR Lowercase( LPTSTR psz )
/************************************************************************/
{
    return CharLower( psz );
}

/************************************************************************/
LPTSTR Uppercase( LPTSTR psz )
/************************************************************************/
{
    return CharUpper( psz );
}

/************************************************************************/
LPTSTR SkipSpaces( LPTSTR pszStr )
/************************************************************************/
{
    if( !pszStr )
	    return( NULL );

	WORD wChar;

    while( *pszStr )
    { 
		if( IsDBCSLeadByte(( BYTE )*pszStr ))
            wChar = *(( LPWORD )pszStr );
        else
            wChar = *pszStr;

         // Skip any leading white space
    	if( wChar > _T(' ') )
	    	break;

	    pszStr = MyCharNext( pszStr );
    }
    return( pszStr );
}

/************************************************************************/
LPTSTR RemoveWhiteSpace( LPTSTR pszStr )
/************************************************************************/
{
    if( !pszStr )
	    return( NULL );

    LPTSTR psz = pszStr;
	WORD wChar;

    while( *pszStr )
    { 
		if( IsDBCSLeadByte(( BYTE )*pszStr ))
            wChar = *(( LPWORD )pszStr );
        else
            wChar = *pszStr;

    	if( wChar > _T(' ') )
        {
            *psz = *pszStr;
            psz = MyCharNext( psz );
        }
	    pszStr = MyCharNext( pszStr );
    }
    *psz = _T('\0');

    return( pszStr );
}

/************************************************************************/
LPTSTR StripTrailer( LPTSTR pszStr, TCHAR cChar )
/************************************************************************/
{
    if( !pszStr )
	    return( NULL );

    int len = MBStrlen( pszStr );

    // find end of string
    LPTSTR psz = pszStr;
    while( *( MyCharNext( psz )))
        psz = MyCharNext( psz );

    while( len-- > 0 )
    { 
        BOOL fDBCS = IsDBCSLeadByte(( BYTE )*psz );

        // if no match, quit
       	if(( fDBCS && ( *(( LPWORD )psz )) != cChar ) || ( *psz != cChar ))
		    break;

        psz = MyCharPrev( pszStr, psz ); 	// points to prev character in string.
        *( MyCharNext( psz )) = _T('\0');  	// terminate string where match is found.
    }
    return( pszStr );
}

/************************************************************************/
LPTSTR lstrfind( LPTSTR pszStr, LPTSTR pszSearchStr )
/************************************************************************/
{
    LPTSTR psz, pszSearch, pszStart = NULL;
    WORD wChar, wCharSearch;

    if( !( psz = Uppercase( pszStr )))
	    return( NULL );

    if( !( pszSearch = Uppercase( pszSearchStr )))
	    return( NULL );

    while( *psz )
    {
        if( IsDBCSLeadByte(( BYTE )*psz ))
            wChar = *(( LPWORD )psz );
        else
            wChar = *psz;

        if( IsDBCSLeadByte(( BYTE )*pszSearch ))
    	    wCharSearch = *(( LPWORD )pszSearch );
        else
    	    wCharSearch = *pszSearch;

	    if( wChar == wCharSearch ) 	// If a character match...
	    { 
		    pszSearch = MyCharNext( pszSearch );
		    if( !pszStart )
			    pszStart = psz;
	    }
	    else                		// If not a character match...
        {
		    pszSearch = pszSearchStr;
		    pszStart = NULL;
	    }

	    if( !( *pszSearch )) 		// If at the end of the search string...
		    break;

        psz = MyCharNext( psz );
    }
    return( pszStart );
}

/************************************************************************/
LPTSTR lStrChr( LPTSTR pszStr, TCHAR cChar )
/************************************************************************/
{
    LPTSTR psz;
    WORD wChar;

    if( !( psz = pszStr ))
	    return( NULL );

    while( *psz )
    {
        if( IsDBCSLeadByte(( BYTE )*psz ))
            wChar = *(( LPWORD )psz );
        else
            wChar = *psz;

	    if( wChar == cChar )
		    return( psz );

        psz = MyCharNext( psz );
    }
    return( NULL );
}

/************************************************************************/
LPTSTR lStrCpyN( LPTSTR pszDst, LPTSTR pszSrc, int length )
/************************************************************************/
{
#ifdef WIN32
	int i;

	LPTSTR pszdst = pszDst;
	if( !( i=length ))
		return( pszDst );

	while( *pszdst++ = *pszSrc++ )
		if( --i <= 0 )
			break;
	return( pszDst );
#else
    return( lstrcpyn( pszDst, pszSrc, length ));
#endif
}

/***********************************************************************/
LPTSTR RGBAscii( LPRGB pRGB, LPTSTR pszAscii )
/***********************************************************************/
{
    LPTSTR pszStr = pszAscii;

    Int2Ascii( pszStr, pRGB->red );
    lstrcat( pszStr, _T(",") );
    pszStr += lstrlen( pszStr );

    Int2Ascii( pszStr, pRGB->green );
    lstrcat( pszStr, _T(",") );
    pszStr += lstrlen( pszStr );

    Int2Ascii( pszStr, pRGB->blue );

    return( pszAscii );
}

/***********************************************************************/
void AsciiRGB( LPTSTR pszAscii, LPRGB pRGB )
/***********************************************************************/
{
    LPTSTR psz, pszStr = pszAscii;
	WORD wChar;

    // replace commas with NULL terminators
   	while( *pszStr )
    {
		if( IsDBCSLeadByte(( BYTE )*pszStr ))
            wChar = *(( LPWORD )pszStr );
        else
            wChar = *pszStr;

	    if( wChar == _T(',') )
        {
            psz = pszStr;
            pszStr = MyCharNext( pszStr );
		    *psz = _T('\0');
        }
        else
            pszStr = MyCharNext( pszStr );
    }

    pRGB->red = Ascii2Int( pszAscii );
    pszAscii += (lstrlen( pszAscii ) + 1 );

    pRGB->green = Ascii2Int( pszAscii );
    pszAscii += (lstrlen( pszAscii ) + 1 );

    pRGB->blue = Ascii2Int( pszAscii );
}

/***********************************************************************/
LPTSTR Int4ToAscii( LPTSTR pszStr, int a, int b, int c, int d )
/***********************************************************************/
{
    LPTSTR psz = pszStr;

    Int2Ascii( psz, a ); lstrcat( psz, _T(",") ); psz += lstrlen( psz );
    Int2Ascii( psz, b ); lstrcat( psz, _T(",") ); psz += lstrlen( psz );
    Int2Ascii( psz, c ); lstrcat( psz, _T(",") ); psz += lstrlen( psz );
    Int2Ascii( psz, d );

    return( pszStr );
}

/***********************************************************************/
LPTSTR Int2ToAscii( LPTSTR pszStr, int a, int b )
/***********************************************************************/
{
    LPTSTR psz = pszStr;

    Int2Ascii( psz, a ); lstrcat( psz, _T(",") ); psz += lstrlen( psz );
    Int2Ascii( psz, b );

    return( pszStr );
}

/***********************************************************************/
void AsciiToInt4( LPTSTR pszAscii, LPINT a, LPINT b, LPINT c, LPINT d )
/***********************************************************************/
{
    LPTSTR psz, pszStr = pszAscii;
	WORD wChar;
 
    while( *pszStr )
    {
		if( IsDBCSLeadByte(( BYTE )*pszStr ))
            wChar = *(( LPWORD )pszStr );
        else
            wChar = *pszStr;

    	if( wChar == _T(',') )
        {
            psz = pszStr;
            pszStr = MyCharNext( pszStr );
       	    *psz = _T('\0');
        }
        else
            pszStr = MyCharNext( pszStr );
    }

    psz = pszStr;
    *a = Ascii2Int( psz ); psz += (lstrlen( psz ) + 1 );
    *b = Ascii2Int( psz ); psz += (lstrlen( psz ) + 1 );
    *c = Ascii2Int( psz ); psz += (lstrlen( psz ) + 1 );
    *d = Ascii2Int( psz );
}

/***********************************************************************/
void AsciiToInt2( LPTSTR pszAscii, LPINT a, LPINT b )
/***********************************************************************/
{
    LPTSTR psz, pszStr = pszAscii;
	WORD wChar;

    // replace commas with NULL terminators
    while( *pszStr )
    {
		if( IsDBCSLeadByte(( BYTE )*pszStr ))
            wChar = *(( LPWORD )pszStr );
        else
            wChar = *pszStr;

    	if( wChar == _T(',') )
        {
            psz = pszStr;
            pszStr = MyCharNext( pszStr );
    	    *psz = _T('\0');
        }
        else
    	    pszStr = MyCharNext( pszStr );
    }

    pszStr = pszAscii;
    *a = Ascii2Int( pszStr ); pszStr += ( lstrlen( pszStr ) + 1 );
    *b = Ascii2Int( pszStr );
}

/***********************************************************************/
LPTSTR Int2Ascii( LPTSTR pszAscii, int i )
/***********************************************************************/
{
    wsprintf( pszAscii, "%d", i );
    return( pszAscii );
}

/***********************************************************************/
int Ascii2Int( LPTSTR pszAscii )
/***********************************************************************/
{
	WORD wChar;
	BOOL neg = NO;
	long lValue = 0;

	while( *pszAscii )
	{
        if( IsDBCSLeadByte(( BYTE )*pszAscii ))
            wChar = *(( LPWORD )pszAscii );
        else
            wChar = *pszAscii;

		if( wChar == _T('-') )
			neg = YES;
		else
		if( wChar >= _T('0') && wChar <= _T('9') )
		{
			lValue *= 10L;
			lValue += ( wChar - _T('0') );
		}
		else
			if( wChar >= _T(' ') )
				break;

        pszAscii = MyCharNext( pszAscii );
	}

	if( lValue > INT_MAX )
		lValue = INT_MAX;

	if( neg )
		lValue = -lValue;

	return(( int )lValue );
}

/***********************************************************************/
LONG Ascii2Long( LPTSTR pszAscii )
/***********************************************************************/
{
    WORD wChar;
    BOOL neg = NO;
    LONG lValue = 0L;

    while( *pszAscii )
    {
        if( IsDBCSLeadByte(( BYTE )*pszAscii ))
            wChar = *(( LPWORD )pszAscii );
        else
            wChar = *pszAscii;

	    if( wChar == _T('-') )
		    neg = YES;
	    else
	    if( wChar >= _T('0') && wChar <= _T('9') )
	    {
		    lValue *= 10;
		    if( lValue < 0 )
		    {
			    lValue = LONG_MAX;
			    break;
		    }
		    lValue += ( wChar - _T('0') );

		    if( lValue < 0 )
		    {
			    lValue = LONG_MAX;
			    break;
		    }
	    }
	    else
	    if( wChar >= _T(' ') )
		    break;

        pszAscii = MyCharNext( pszAscii );
    }

    if( neg )
	    lValue = -lValue;

    return( lValue );
}

/***********************************************************************/
ULONG Ascii2ULong( LPTSTR pszAscii )
/***********************************************************************/
{
    WORD wChar;
    ULONG lValue = 0L;

    while( *pszAscii )
    {
        if( IsDBCSLeadByte(( BYTE )*pszAscii ))
            wChar = *(( LPWORD )pszAscii );
        else
            wChar = *pszAscii;

	    if( wChar >= _T('0') && wChar <= _T('9') )
	    {
		    lValue *= 10;
		    lValue += ( wChar - _T('0') );
	    }
	    else
	    if( wChar >= _T(' ') )
		    break;

        pszAscii = MyCharNext( pszAscii );
    }
    return( lValue );
}

// only set once, so its ok
static TCHAR cDecimal = _T('\0');

/***********************************************************************/
// use  FixedAscii if possible
/***********************************************************************/
LPTSTR DoubleAscii( double value, LPTSTR pszAscii )
/***********************************************************************/
{
	TCHAR szDouble[50];
	
	sprintf( szDouble, "%g", value );
	lstrcpy( pszAscii, szDouble );
	return( pszAscii );
}

/***********************************************************************/
// use  AsciiFixed if possible
/***********************************************************************/
double AsciiDouble( LPTSTR  pszAscii, LPINT pbTranslated )
/***********************************************************************/
{
	double val = atof( pszAscii );
	*pbTranslated = TRUE;
	return( val );
}

/************************************************************************/
DWORD HexToBytes( LPTSTR lpHex, LPTR lpBytes )
/************************************************************************/
{
	DWORD dwBytes = 0;
	char hex[3];

	hex[2] = _T('\0');
	while (*lpHex)
	{
		hex[0] = *lpHex++;
		if (*lpHex)
			hex[1] = *lpHex++;
		else
			hex[1] = _T('\0');
		*lpBytes++ = htoi(hex);
		++dwBytes;
	}
	return(dwBytes);
}

/************************************************************************/
void BytesToHex( LPTR lpBytes, LPTSTR lpHex, DWORD dwBytes )
/************************************************************************/
{
	char hex[5];

	while (dwBytes)
	{
		wsprintf(hex, "%02X", (int)*lpBytes);
		*lpHex++ = _T(hex[0]);
		*lpHex++ = _T(hex[1]);
		++lpBytes;
		--dwBytes;
	}
	*lpHex = _T('\0');
}

/************************************************************************/
LPTSTR itoh( int i, LPTSTR lpHex )   	/* convert an int to ascii hex  */
/************************************************************************/
{
wsprintf( lpHex, "%x", i );
return( lpHex );
}

/************************************************************************/
int htoi( LPTSTR s )                    /* convert ascii hex to an int  */
/************************************************************************/
{
	int i, n, m;

	for(n=0, i=0; s[i]; i++)
	{
		if (s[i] >= _T('0') && s[i] <= _T('9') )
			m = s[i] - _T('0');
		else if (s[i] >= _T('a') && s[i] <= _T('f'))
			m = s[i] - _T('a') + 10;
		else if (s[i] >= _T('A') && s[i] <= _T('F'))
			m = s[i] - _T('A') + 10;
		else 
			break;

		n = 16 * n + m;
	}
	return n;
}

/****************************************************************************/
// 	WILDCARD STUFF STOLEN FROM MGXUTIL
/****************************************************************************/

/********************************* Constants ********************************/
#define MAX_RECURSE_LEVELS      128

#define RETURN_MATCH    \
	{bMatch = TRUE; goto ReturnPoint; }

#define RETURN_MISMATCH \
	{if (Level) goto EndLevel; goto ReturnPoint;}

/*********************************** Types **********************************/
typedef struct
{
    unsigned PatPos;
    unsigned TxtPos;
} MATCH_INFO;

/************************************************************************/
BOOL MatchWildcard( LPTSTR pszText, LPTSTR pszPattern, WORD wFlags )
/************************************************************************/
{
    BOOL bMatch  = FALSE;

    LPTR hmi = Alloc( MAX_RECURSE_LEVELS * sizeof( MATCH_INFO ));

    if( hmi )
    {
        MATCH_INFO FAR *mi = ( MATCH_INFO FAR * )hmi;

        int Level = 0;

        mi[ Level ].TxtPos = 0;
        mi[ Level ].PatPos = 0;

        BYTE PatOffset, TxtOffset;
        WORD C1, C2, PatChar, TxtChar;

BeginLevel:
        while( TRUE )
        {
            if( IsDBCSLeadByte(( BYTE )pszPattern[ mi[ Level ].PatPos ] ))
            {
                PatChar = MAKEWORD( pszPattern[ mi[ Level ].PatPos ], pszPattern[ mi[ Level ].PatPos+1 ] );
                PatOffset = sizeof( WORD );
            }
            else
            {
                PatChar = pszPattern[ mi[ Level ].PatPos ];
                PatOffset = sizeof( TCHAR );
            }

            if( !PatChar )
                break;

            switch( PatChar )
            {
                case _T('*'):
                    mi[ Level ].PatPos += PatOffset;

                    // If pattern ends with an asterisk, then any remaining 
                    // text will match, so just force a TRUE return value.
                    if( IsDBCSLeadByte(( BYTE )pszPattern[ mi[ Level ].PatPos ] )) 
                    {
                        if( MAKEWORD( pszPattern[ mi[ Level ].PatPos ], pszPattern[ mi[ Level ].PatPos+1 ] ) == 0 )
                            RETURN_MATCH;
                    }
                    else
                    if(( BYTE )pszPattern[ mi[ Level ].PatPos ] == 0 )
                        RETURN_MATCH;

                    while( TRUE )
                    {
                        if( IsDBCSLeadByte(( BYTE )pszText[ mi[ Level ].TxtPos ] )) 
                        {
                            TxtChar = MAKEWORD( pszText[ mi[ Level ].TxtPos ], pszText[ mi[ Level ].TxtPos+1 ] );
                            TxtOffset = sizeof( WORD );
                        }
                        else
                        {
                            TxtChar = MAKEWORD( pszText[ mi[ Level ].TxtPos ], 0 );
                            TxtOffset = sizeof( TCHAR );
                        }

                        if( !TxtChar )
                            break;

                        if( Level >= MAX_RECURSE_LEVELS )
                        {
                            RETURN_MISMATCH;
                        }
                        else
                        {
                            Level++;
                            mi[ Level ].TxtPos = mi[ Level-1 ].TxtPos;
                            mi[ Level ].PatPos = mi[ Level-1 ].PatPos;
                            goto BeginLevel;
                        }
EndLevel:
                        Level--;
                        mi[ Level ].TxtPos += TxtOffset;
                    }
                    RETURN_MISMATCH;
                    break;

                case _T('?'):
                    mi[ Level ].PatPos += PatOffset;

                    if( IsDBCSLeadByte(( BYTE )pszText[ mi[ Level ].TxtPos ] )) 
                    {
                        TxtChar = MAKEWORD( pszText[ mi[ Level ].TxtPos ], pszText[ mi[ Level ].TxtPos+1 ] );
                        TxtOffset = sizeof( WORD );
                    }
                    else
                    {
                        TxtChar = pszText[ mi[ Level ].TxtPos ];
                        TxtOffset = sizeof( TCHAR );
                    }

                    if( !TxtChar )
                    {
                        RETURN_MISMATCH;
                    }
                    mi[ Level ].TxtPos += TxtOffset;
                    break;

                case _T('\\'):
                    if( wFlags & MW_ESCAPES )
                        mi[ Level ].PatPos += PatOffset;
                    // Fall through to default case;

                default:
                    // Handle slashes and all other characters

                    if( IsDBCSLeadByte(( BYTE )pszText[ mi[ Level ].TxtPos ] )) 
                    {
				 	    C1 = MAKEWORD( pszText[ mi[ Level ].TxtPos ], pszText[ mi[ Level ].TxtPos+1 ] );
                        TxtOffset = sizeof( WORD );
                    }
                    else
                    {
				 	    C1 = pszText[ mi[ Level ].TxtPos ];
                        TxtOffset = sizeof( TCHAR );
                    }

                    if( IsDBCSLeadByte(( BYTE )pszPattern[ mi[ Level ].PatPos ] )) 
                    {
				 	    C2 = MAKEWORD( pszPattern[ mi[ Level ].PatPos ], pszPattern[ mi[ Level ].PatPos+1 ] );
                        PatOffset = sizeof( WORD );
                    }
                    else
                    {
				 	    C2 = pszPattern[ mi[ Level ].PatPos ];
                        PatOffset = sizeof( TCHAR );
                    }

                    if( wFlags & MW_CASE )
				    {
                        if( TxtOffset == sizeof( WORD ))
       				 	    C1 = MBToUpperW(( LPTSTR )&C1 );
                        else
       				 	    C1 = MBToUpper(( LPTSTR )&C1 );

                        if( PatOffset == sizeof( WORD ))    
    					    C2 = MBToUpperW(( LPTSTR )&C2 );
                        else
    					    C2 = MBToUpper(( LPTSTR )&C2 );
				    }

				    if( C1 == C2 )
				    {
                        mi[ Level ].PatPos += PatOffset;
                        mi[ Level ].TxtPos += TxtOffset;
                    }
                    else
                        RETURN_MISMATCH;
            }
        }

        if( IsDBCSLeadByte(( BYTE )pszText[ mi[ Level ].TxtPos ] )) 
            bMatch = MAKEWORD( pszText[ mi[ Level ].TxtPos ], pszText[ mi[ Level ].TxtPos+1 ] ) == 0;
        else
            bMatch = ( BYTE )pszText[ mi[ Level ].TxtPos ] == 0;
    }

ReturnPoint:
    if( hmi )
       FreeUp( hmi );

    return( bMatch );
}

/***********************************************************************/
int GetIntParm( LPTSTR pszCommand, int nParam, LPINT pbTranslated )
/***********************************************************************/
{
    if( !( pszCommand = SkipParm( pszCommand, nParam )))
    {
	    if( pbTranslated )
		    *pbTranslated = FALSE;

	    return( 0 );
    }
    if( pbTranslated )
	    *pbTranslated = TRUE;

    return( Ascii2Int( pszCommand ));
}

/***********************************************************************/
long GetLongParm(LPTSTR pszCommand, int nParam, LPINT pbTranslated)
/***********************************************************************/
{
	if( !( pszCommand = SkipParm( pszCommand, nParam )))
	{
		if( pbTranslated )
			*pbTranslated = FALSE;

		return( 0 );
	}

	if( pbTranslated )
		*pbTranslated = TRUE;

	return( Ascii2Long( pszCommand ));
}

/***********************************************************************/
long GetULongParm(LPTSTR pszCommand, int nParam, LPINT pbTranslated)
/***********************************************************************/
{
	if (!(pszCommand = SkipParm(pszCommand, nParam)))
	{
		if (pbTranslated)
			*pbTranslated = FALSE;

		return(0);
	}

	if (pbTranslated)
		*pbTranslated = TRUE;

	return(Ascii2ULong(pszCommand));
}

/***********************************************************************/
LFIXED GetFixedParm( LPTSTR pszCommand, int nParam, LPINT pbTranslated )
/***********************************************************************/
{
    TCHAR parm[20];
	WORD wChar;

    if( !( pszCommand = SkipParm( pszCommand, nParam )))
    {
	    if( pbTranslated )
		    *pbTranslated = FALSE;

	    return( 0 );
    }

    LPTSTR psz = parm;

    while( TRUE )
    {
		BOOL fDBCS = IsDBCSLeadByte(( BYTE )*pszCommand );

		if( fDBCS )
            wChar = *(( LPWORD )pszCommand );
        else
            wChar = *pszCommand;

		if( wChar == _T(' ') || wChar == _T('\0') ) 
			break;

		if( fDBCS )
			*(( LPWORD )psz ) = wChar;
		else
			*psz = *pszCommand;

    	psz = MyCharNext( psz ); pszCommand = MyCharNext( pszCommand );
    }
    *psz = _T('\0');

    if( pbTranslated )
	    *pbTranslated = TRUE;

    return( AsciiFixed( parm ));
}

/***********************************************************************/
LPTSTR GetStringParm( LPTSTR pszCommand, int nParam, LPTSTR pszParm, LPINT pbTranslated )
/***********************************************************************/
{
    *pszParm = _T('\0');
	WORD wChar;
	BOOL fDBCS;

    if( !( pszCommand = SkipParm( pszCommand, nParam )))
    {
	    if( pbTranslated )
		    *pbTranslated = FALSE;

	    return( pszParm );
    }

    if( pszCommand )
    {
	    LPTSTR psz = pszParm;

		fDBCS = IsDBCSLeadByte(( BYTE )*pszCommand );
		if( fDBCS )
            wChar = *(( LPWORD )pszCommand );
        else
            wChar = *pszCommand;

	    if( wChar == STRING_DELIM )
	    {
            pszCommand = MyCharNext( pszCommand );

			fDBCS = IsDBCSLeadByte(( BYTE )*pszCommand );
			if( fDBCS )
            	wChar = *(( LPWORD )pszCommand );
        	else
            	wChar = *pszCommand;

            while( wChar != STRING_DELIM && wChar != _T('\0') )
            {
				if( fDBCS )
				    *(( LPWORD )psz ) = wChar;
				else
				    *psz = *pszCommand;

                psz = MyCharNext( psz ); pszCommand = MyCharNext( pszCommand );

				fDBCS = IsDBCSLeadByte(( BYTE )*pszCommand );
				if( fDBCS )
            		wChar = *(( LPWORD )pszCommand );
        		else
            		wChar = *pszCommand;
            }
	    }
	    else
	    {
		    while( wChar != STRING_DELIM && wChar != _T('\0') )
            {
				if( fDBCS )
					*(( LPWORD )psz ) = wChar;
				else
				    *psz = *pszCommand;

                psz = MyCharNext( psz ); pszCommand = MyCharNext( pszCommand );

				fDBCS = IsDBCSLeadByte(( BYTE )*pszCommand );
				if( fDBCS )
            		wChar = *(( LPWORD )pszCommand );
        		else
            		wChar = *pszCommand;
            }
	    }
	    *psz = _T('\0');
    }

    if( pbTranslated )
	    *pbTranslated = TRUE;

    return( pszParm );
}

/***********************************************************************/
LOCAL LPTSTR SkipParm( LPTSTR pszCommand, int nParam )
/***********************************************************************/
{
    pszCommand = SkipSpaces( pszCommand );
	WORD wChar;
                                     
    // skip over current parameter
    while( --nParam >= 0 )
	{
		if( IsDBCSLeadByte(( BYTE )*pszCommand ))
            wChar = *(( LPWORD )pszCommand );
        else
            wChar = *pszCommand;

	    if( wChar == STRING_DELIM )
		    pszCommand = lStrChr( pszCommand+sizeof( TCHAR ), STRING_DELIM );
	    else
		    pszCommand = lStrChr( pszCommand, _T(' ') );
													  
	    if( !pszCommand )
		    return( NULL );

        pszCommand = MyCharNext( pszCommand );
	    pszCommand = SkipSpaces( pszCommand );
	}
    return( pszCommand );
}





