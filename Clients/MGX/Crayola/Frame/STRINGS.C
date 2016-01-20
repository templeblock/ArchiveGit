/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include <ctype.h>
#include <limits.h>
#include "framelib.h"
#include "macros.h"

#ifndef EOF
#define EOF	(-1)
#endif

/************************************************************************/
BOOL StringsEqual( LPSTR lpString1, LPSTR lpString2 )
/************************************************************************/
{
int i, length1, length2;
char c1, c2;

length1 = lstrlen(lpString1);
length2 = lstrlen(lpString2);
if (length1 != length2)
	return(NO);
for (i = 0; i < length1; ++i)
	{
	c1 = lpString1[i];
	c2 = lpString2[i];
	if (isupper(c1))
	c1 = tolower(c1);
	if (isupper(c2))
	c2 = tolower(c2);
	if (c1 != c2)
	return(NO);
	}
return(YES);
}


/************************************************************************/
BOOL StringsEqualN( LPSTR lpString1, LPSTR lpString2, int length )
/************************************************************************/
{
int i, length1, length2;
char c1, c2;

length1 = lstrlen(lpString1);
length2 = lstrlen(lpString2);
if (length1 < length || length2 < length)
	return(NO);
for (i = 0; i < length; ++i)
	{
	c1 = lpString1[i];
	c2 = lpString2[i];
	if (isupper(c1))
	c1 = tolower(c1);
	if (isupper(c2))
	c2 = tolower(c2);
	if (c1 != c2)
	return(NO);
	}
return(YES);
}


/************************************************************************/
LPSTR Lowercase( LPSTR lp )
/************************************************************************/
{
BYTE c;
LPSTR lpString;

lpString = lp;
while ( c = *lp )
	{
	if ( c >= 'A' && c <= 'Z' )
		*lp = c - ( 'A' - 'a' );
	lp++;
	}
return( lpString );
}


/************************************************************************/
LPSTR Uppercase( LPSTR lp )
/************************************************************************/
{
BYTE c;
LPSTR lpString;

lpString = lp;
while ( c = *lp )
	{
	if (islower(c))
		*lp = toupper(c);
	lp++;
	}
return( lpString );
}


/************************************************************************/
LPSTR SkipSpaces( LPSTR lpString )
/************************************************************************/
{
if ( !lpString )
	return( NULL );
while ( *lpString )
	{ // Skip any leading white space
	if ( *lpString > ' ' )
		break;
	lpString++;
	}
return( lpString );
}


/************************************************************************/
LPSTR RemoveWhiteSpace( LPSTR lpString )
/************************************************************************/
{
LPSTR lp;

if ( !lpString )
	return( NULL );
lp = lpString;
while ( *lpString )
	{ // Don't copy white space
	if ( *lpString > ' ' )
		*lp++ = *lpString;
	lpString++;
	}
*lp = '\0';
return( lpString );
}


/************************************************************************/
LPSTR StripTrailer( LPSTR lpString, char cChar )
/************************************************************************/
{
int l;

if ( !lpString )
	return( NULL );

l = lstrlen( lpString );
while ( --l >= 0 )
	{ // Skip any trailing characters that match
	if ( *(lpString+l) != cChar )
		break;
	*(lpString+l) = '\0';
	}
return( lpString );
}


/************************************************************************/
LPSTR lstrfind( LPSTR lpString, LPSTR lpSearchString )
/************************************************************************/
{
LPSTR lp, lpStart, lpSearch;
char c, c1;

if ( !(lp = lpString) )
	return( NULL );
if ( !(lpSearch = lpSearchString) )
	return( NULL );

lpStart = NULL;
while ( c = *lp )
	{
	if ( c >= 'a' && c <= 'z' ) // Convert to upper case
		c -= ('a'-'A');
	c1 = *lpSearch;
	if ( c1 >= 'a' && c1 <= 'z' ) // Convert to upper case
		c1 -= ('a'-'A');
	if ( c == c1 )
		{ // If a character match...
		lpSearch++;
		if ( !lpStart )
			lpStart = lp;
		}
	else    { // If not a character match...
		lpSearch = lpSearchString;
		lpStart = NULL;
		}
	if ( !*lpSearch ) // If at the end of the search string...
		break;
	lp++;
	}

return( lpStart );
}


/************************************************************************/
LPSTR lStrChr( LPSTR lpString, char cChar )
/************************************************************************/
{
LPSTR lp;
char c;

if ( !(lp = lpString) )
	return( NULL );
while ( c = *lp )
	{
	if ( c == cChar )
		return( lp );
	lp++;
	}

return( NULL );
}


/************************************************************************/
LPSTR lStrCpyN( LPSTR lpDst, LPSTR lpSrc, int length )
/************************************************************************/
{
#ifdef WIN32
int i;
LPSTR lpdst;


lpdst = lpDst;
if (!(i=length))
	return(lpDst);

while (*lpdst++=*lpSrc++)
	if(--i<=0)
		break;
return(lpDst);
#else
return(lstrcpyn(lpDst, lpSrc, length));
#endif
}


/***********************************************************************/
LPSTR RGBAscii( LPRGB lpRGB, LPSTR lpAscii )
/***********************************************************************/
{
LPSTR lpString;

lpString = lpAscii;
Int2Ascii( lpString, lpRGB->red );
lstrcat( lpString, "," );
lpString += lstrlen( lpString );
Int2Ascii( lpString, lpRGB->green );
lstrcat( lpString, "," );
lpString += lstrlen( lpString );
Int2Ascii( lpString, lpRGB->blue );
return( lpAscii );
}


/***********************************************************************/
void AsciiRGB( LPSTR lpAscii, LPRGB lpRGB )
/***********************************************************************/
{
LPSTR lpString;

lpString = lpAscii;
while ( *lpString )
	{
	if ( *lpString == ',' )
		*lpString = '\0';
	lpString++;
	}
lpRGB->red = Ascii2Int( lpAscii );
lpAscii += (lstrlen( lpAscii ) + 1);
lpRGB->green = Ascii2Int( lpAscii );
lpAscii += (lstrlen( lpAscii ) + 1);
lpRGB->blue = Ascii2Int( lpAscii );
}


/***********************************************************************/
LPSTR Int4ToAscii( LPSTR lpString, int a, int b, int c, int d )
/***********************************************************************/
{
LPSTR lp;

lp = lpString;
Int2Ascii( lp, a ); lstrcat( lp, "," ); lp += lstrlen( lp );
Int2Ascii( lp, b ); lstrcat( lp, "," ); lp += lstrlen( lp );
Int2Ascii( lp, c ); lstrcat( lp, "," ); lp += lstrlen( lp );
Int2Ascii( lp, d );
return( lpString );
}


/***********************************************************************/
LPSTR Int2ToAscii( LPSTR lpString, int a, int b )
/***********************************************************************/
{
LPSTR lp;

lp = lpString;
Int2Ascii( lp, a ); lstrcat( lp, "," ); lp += lstrlen( lp );
Int2Ascii( lp, b );
return( lpString );
}

/***********************************************************************/
void AsciiToInt4( LPSTR lpString, LPINT a, LPINT b, LPINT c, LPINT d )
/***********************************************************************/
{
LPSTR lp;

lp = lpString;
while ( *lp )
	{
	if ( *lp == ',' )
		*lp = '\0';
	lp++;
	}
lp = lpString;
*a = Ascii2Int( lp ); lp += (lstrlen( lp ) + 1);
*b = Ascii2Int( lp ); lp += (lstrlen( lp ) + 1);
*c = Ascii2Int( lp ); lp += (lstrlen( lp ) + 1);
*d = Ascii2Int( lp );
}


/***********************************************************************/
void AsciiToInt2( LPSTR lpString, LPINT a, LPINT b )
/***********************************************************************/
{
LPSTR lp;

lp = lpString;
while ( *lp )
	{
	if ( *lp == ',' )
		*lp = '\0';
	lp++;
	}
lp = lpString;
*a = Ascii2Int( lp ); lp += (lstrlen( lp ) + 1);
*b = Ascii2Int( lp );
}



/***********************************************************************/
LPSTR Int2Ascii( LPSTR lpAscii, int i )
/***********************************************************************/
{
wsprintf( lpAscii, "%d", i );
return( lpAscii );
}


/***********************************************************************/
int Ascii2Int( LPSTR lpAscii )
/***********************************************************************/
{
	BYTE c;
	BOOL neg;
	long lValue;

	if ( !lpAscii )
		return( 0 );
	neg = NO;
	lValue = 0;

	while ( c = (*lpAscii++) )
	{
		if ( c == '-' )
			neg = YES;
		else
		if ( c >= '0' && c <= '9' )
		{
			lValue *= 10L;
			lValue += (c-'0');
		}
		else
			if ( c >= ' ' )
				break;
	}

	if (lValue > 32767)
		lValue = 32767;

	if ( neg )
		lValue = -lValue;

	return( (int)lValue );
}


/***********************************************************************/
LONG Ascii2Long( LPSTR lpAscii )
/***********************************************************************/
{
BYTE c;
BOOL neg;
LONG lValue;

if ( !lpAscii )
	return( 0L );
neg = NO;
lValue = 0L;
while ( c = (*lpAscii++) )
	{
	if ( c == '-' )
		neg = YES;
	else
	if ( c >= '0' && c <= '9' )
		{
		lValue *= 10;
		if (lValue < 0)
			{
			lValue = LONG_MAX;
			break;
			}
		lValue += (c-'0');
		if (lValue < 0)
			{
			lValue = LONG_MAX;
			break;
			}
		}
	else
	if ( c >= ' ' )
		break;
	}

if ( neg )
	lValue = -lValue;
return( lValue );
}


// only set once, so its ok
static char cDecimal = '\0';

/***********************************************************************/
// use  FixedAscii if possible
/***********************************************************************/
LPSTR DoubleAscii( double value, LPSTR lpAscii)
/***********************************************************************/
{
	char szDouble[50];
	
	long l = (long)value;
	wsprintf(szDouble, "%lu", l);

//	sprintf(szDouble, "%g", value);
		
	lstrcpy( lpAscii, szDouble );
	return( lpAscii );
}

/***********************************************************************/
// use  AsciiFixed if possible
/***********************************************************************/
double AsciiDouble( LPSTR  lpAscii, LPINT lpbTranslated )
/***********************************************************************/
{
return 1.0;		// sscanf doesn't exest in WIN32

//	float f;
//	int ret;
	
//	ret = sscanf(lpAscii, "%f", &f);
//	if (lpbTranslated)
//	{
//		if (!ret || ret ==EOF)
//			*lpbTranslated = FALSE;
//		else
//			*lpbTranslated = TRUE;
//	}
//	return(f);
}



/************************************************************************/
DWORD HexToBytes( LPSTR lpHex, LPTR lpBytes )
/************************************************************************/
{
DWORD dwBytes = 0;
char hex[3];

hex[2] = '\0';
while (*lpHex)
	{
	hex[0] = *lpHex++;
	if (*lpHex)
		hex[1] = *lpHex++;
	else
		hex[1] = '\0';
	*lpBytes++ = htoi(hex);
	++dwBytes;
	}
return(dwBytes);
}

/************************************************************************/
void BytesToHex( LPTR lpBytes, LPSTR lpHex, DWORD dwBytes )
/************************************************************************/
{
char hex[5];

while (dwBytes)
	{
	wsprintf(hex, "%02X", (int)*lpBytes);
	*lpHex++ = hex[0];
	*lpHex++ = hex[1];
	++lpBytes;
	--dwBytes;
	}
*lpHex = '\0';
}

/************************************************************************/
LPSTR itoh( int i )                    /* convert an int to ascii hex */
/************************************************************************/
{
STATIC16 char hex[5];

wsprintf( hex, "%x", i );
return( hex );
}


/************************************************************************/
int htoi( LPSTR s )                     /* convert ascii hex to an int */
/************************************************************************/
{
int i, n, m;

for(n=0, i=0; s[i]; i++)
	{
	if (s[i] >= '0' && s[i] <= '9')
		m = s[i] - '0';
	else if (s[i] >= 'a' && s[i] <= 'f')
		m = s[i] - 'a' + 10;
	else if (s[i] >= 'A' && s[i] <= 'F')
		m = s[i] - 'A' + 10;
	else break;
	n = 16 * n + m;
	}
return n;
}

/************************************************************************/
// 	WILDCARD STUFF STOLEN FROM MGXUTIL
/************************************************************************/

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
BOOL MatchWildcard ( LPSTR lpText,LPSTR lpPattern,WORD wFlags )
/************************************************************************/
{
    LPTR hmi;
    BOOL   bResult = FALSE;
    BOOL   bMatch  = FALSE;
	 char 	c1, c2;

    hmi = Alloc (MAX_RECURSE_LEVELS*sizeof (MATCH_INFO));
    if (hmi)
    {
        MATCH_INFO FAR *mi = (MATCH_INFO FAR *)hmi;
        int  Level = 0;

        mi[Level].TxtPos = 0;
        mi[Level].PatPos = 0;

BeginLevel:
        while (lpPattern[mi[Level].PatPos])
        {
            char PatChar = lpPattern[mi[Level].PatPos];

            switch (PatChar)
            {
            case '*':
                mi[Level].PatPos++;
               /***************************************************************
                * If pattern ends with an asterisk, then any remaining text
                *    will match, so just force a TRUE return value.
                ***************************************************************/
                if (lpPattern[mi[Level].PatPos]==0)
                    RETURN_MATCH;

                while (lpText[mi[Level].TxtPos])
                {
                    if (Level >= MAX_RECURSE_LEVELS)
                    {
                        RETURN_MISMATCH;
                    }
                    else
                    {
                        Level++;
                        mi[Level].TxtPos = mi[Level-1].TxtPos;
                        mi[Level].PatPos = mi[Level-1].PatPos;
                        goto BeginLevel;
                    }
EndLevel:
                    Level--;
                    mi[Level].TxtPos++;
                }
                RETURN_MISMATCH;
                break;

            case '?':
                mi[Level].PatPos++;
                if (lpText[mi[Level].TxtPos]==0)
                {
                    RETURN_MISMATCH;
                }
                mi[Level].TxtPos++;
                break;

            case '\\':
                if (wFlags & MW_ESCAPES)
                    mi[Level].PatPos++;
                // Fall through to default case;

            default:
                // Handle slashes and all other characters

                if (wFlags & MW_CASE)
				 {
				 	c1 = UPPERCASE(lpText[mi[Level].TxtPos]);
					c2 = UPPERCASE(lpPattern[mi[Level].PatPos]);
				 }
				 else
				 {
				 	c1 = lpText[mi[Level].TxtPos];
					c2 = lpPattern[mi[Level].PatPos];
				 }
				 if (c1 == c2)
				 {
                    mi[Level].PatPos++;
                    mi[Level].TxtPos++;
                }
                else
                    RETURN_MISMATCH;
            }
        }
        bMatch = lpText[mi[Level].TxtPos]==0;
//        if (!bMatch)
//            RETURN_MISMATCH;
    }
ReturnPoint:
    if (hmi)
       FreeUp(hmi);
    return (bMatch);
}


