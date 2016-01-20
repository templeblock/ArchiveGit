// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"
#include <direct.h>

#define MAXVALUE 255
#define THRESH	127
#define CLUMP	1
#define DECISION_X ((x++ % CLUMP) == 0)

/************************************************************************/
int bound(val,lower,upper)
/************************************************************************/
int val,lower,upper;
{
if(val < lower)
	return(lower);
if(val > upper)
	return(upper);
return(val);
}

/***********************************************************************/
void AstralUnionRect( lpDestRect, lpSrc1Rect, lpSrc2Rect )
/***********************************************************************/
LPRECT lpDestRect, lpSrc1Rect, lpSrc2Rect;
{
lpDestRect->top = min(lpSrc1Rect->top, lpSrc2Rect->top);
lpDestRect->left = min(lpSrc1Rect->left, lpSrc2Rect->left);
lpDestRect->bottom = max(lpSrc1Rect->bottom, lpSrc2Rect->bottom);
lpDestRect->right = max(lpSrc1Rect->right, lpSrc2Rect->right);
}

/************************************************************************/
BOOL AstralIntersectRect(lpDest, lpSrc1, lpSrc2)
/************************************************************************/
LPRECT lpDest, lpSrc1, lpSrc2;
{
if (lpSrc1->left > lpSrc2->right ||
    lpSrc1->right < lpSrc2->left ||
    lpSrc1->top > lpSrc2->bottom ||
    lpSrc1->bottom < lpSrc2->top)
	{
	return(FALSE);
	}
lpDest->left = max(lpSrc1->left, lpSrc2->left);
lpDest->right = min(lpSrc1->right, lpSrc2->right);
lpDest->top = max(lpSrc1->top, lpSrc2->top);
lpDest->bottom = min(lpSrc1->bottom, lpSrc2->bottom);
return(TRUE);
}

/************************************************************************/
BOOL AstralSubtractRect(lpDest, lpSrc)
/************************************************************************/
LPRECT lpDest, lpSrc;
{
RECT rSect;

if (lpDest->right < lpDest->left)
	return;

if (AstralIntersectRect(&rSect, lpDest, lpSrc))
	{
	// first see if we can reduce height
	if (rSect.left == lpDest->left && rSect.right == lpDest->right)
		{
		if (rSect.top == lpDest->top && rSect.bottom == lpDest->bottom)
			{
			lpDest->top = lpDest->left = 0;
			lpDest->bottom = lpDest->right = -1;
			}
		else if (rSect.top == lpDest->top)
			{
			lpDest->top = rSect.bottom+1;
			}
		else if (rSect.bottom == lpDest->bottom)
			{
			lpDest->bottom = rSect.top-1;
			}
		}
	else if (rSect.top == lpDest->top && rSect.bottom == lpDest->bottom)
		{
		if (rSect.left == lpDest->left && rSect.right == lpDest->right)
			{
			lpDest->top = lpDest->left = 0;
			lpDest->bottom = lpDest->right = -1;
			}
		else if (rSect.left == lpDest->left)
			{
			lpDest->left = rSect.right+1;
			}
		else if (rSect.right == lpDest->right)
			{
			lpDest->right = rSect.left-1;
			}
		}
	}
}

/***********************************************************************/
void BoundRect( lpRect, left, top, right, bottom )
/***********************************************************************/
LPRECT lpRect;
int left, top, right, bottom;
{
// Check the left
if ( lpRect->left < left )	lpRect->left = left;	else
if ( lpRect->left > right )	lpRect->left = right;

// Check the right
if ( lpRect->right < left )	lpRect->right = left;	else
if ( lpRect->right > right )	lpRect->right = right;

// Check the top
if ( lpRect->top < top )	lpRect->top = top;	else
if ( lpRect->top > bottom )	lpRect->top = bottom;

// Check the bottom
if ( lpRect->bottom < top )	lpRect->bottom = top;	else
if ( lpRect->bottom > bottom )	lpRect->bottom = bottom;
}


/***********************************************************************/
void diffuse( x, y, iColor, lpLut, lpIn, count, lpOut )
/***********************************************************************/
int	x, y, iColor;
LPTR	lpLut, lpIn;
int	count;
LPTR	lpOut;
{
BYTE	byte, bit;
LPINT	lpError;
int	pixel, iErrorCount;
static LPTR lpDiffuseMemory;
static int iDiffuseCount;
static ULONG lSeed = 0xDEADBABA;

count++; /* need 1 additional slot for errors */
iErrorCount = count * ( iColor ? (3*sizeof(int)) : sizeof(int) );
if ( lpDiffuseMemory && (iErrorCount != iDiffuseCount) )
	{
	FreeUp( lpDiffuseMemory );
	lpDiffuseMemory = NULL;
	}

if ( !lpDiffuseMemory )
	{
	iDiffuseCount = iErrorCount;
	if ( !(lpDiffuseMemory = Alloc( (long)iDiffuseCount )) )
		return;
	clr( lpDiffuseMemory, iDiffuseCount );
	}
else
if ( y == 0 ) /* if this the the start of the image, clear the error buffer */
	clr( lpDiffuseMemory, iDiffuseCount );

lpError = (LPINT)lpDiffuseMemory;
if ( iColor )
	lpError += (count * (iColor-1));

byte = 0;
bit = 128;
while (--count > 0)
	{
	if ( lpLut )
		pixel = lpLut[ *lpIn++ ];
	else	pixel = *lpIn++;
	if ( (pixel += *lpError) > THRESH )
		{
		byte |= bit;
		pixel -= MAXVALUE;
		}

	if ( lSeed & BIT18 )
		{
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		*lpError  += pixel;
		}
	else	{
		lSeed += lSeed;
		*lpError++ = pixel;
		}

	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
}

#ifdef UNUSED
/***********************************************************************/
void diffuse( x, y, lpIn, count, lpOut )
/***********************************************************************/
int	x, y;
LPSTR	lpIn;
int	count;
LPSTR	lpOut;
{
LPSTR	pDecision;
LPINT	pError;

BYTE	byte, bit;
int	pixel;
char	decision, decision_y;
int	clear;

pError = (LPINT)lpDiffuseMemory; /* Need two buffers since errors are ints */
pDecision = lpDiffuseMemory + (long)(count*2);

if ( y == 0 )	/* if this the the start of the image, */
	{	/* clear out the error buffer          */
	clear = count;
	while (--clear >= 0)
		{
		*pError++ = 0;
		*pDecision++ = 0;
		}
	/* reset the pointers */
	pError = (LPINT)lpDiffuseMemory;
	pDecision = lpDiffuseMemory + (long)(count*2);
	}

decision_y = ((y % CLUMP) == 0);

x = 0;
byte = 0;
bit = 128;
while (--count >= 0)
	{
	pixel = (BYTE)(*lpIn++);
	pixel += *pError;

	if ( decision_y )
		{
		if ( DECISION_X )
			decision = ( pixel > THRESH );
		/* else decision remains unchanged */
		*pDecision++ = decision;
		}
	else	decision = *pDecision++;

	if ( decision )
		{
		byte |= bit;
		pixel -= MAXVALUE;
		}

	if ( rand() >= 16384 )
		{
		*pError++ = 0;
		*pError  += pixel;
		}
	else	{
		*pError++ = pixel;
		}

	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
}
#endif

/***********************************************************************/
void dither( x, y, lpIn, count, lpOut )
/***********************************************************************/
int	x, y;
LPTR	lpIn;
int	count;
LPTR	lpOut;
{
BYTE	byte, bit;
PTR	pPat, pEndPat, pStartPat;
static BYTE Pattern[] = {
	112, 96,104,120,143,159,151,135, 
	 72,  0,  8, 32,183,254,247,223, 
	 80, 16, 24, 40,175,239,231,215, 
	 88, 48, 56, 64,167,207,199,191, 
	139,155,147,131,116,100,108,124, 
	179,254,243,219, 76,  0, 12, 36, 
	171,235,227,211, 84, 20, 28, 44, 
	163,203,195,187, 92, 52, 60, 68 };

pStartPat = Pattern   + 8*(y&7);
pEndPat   = pStartPat + 8;
pPat      = pStartPat + (x&7);

byte = 0;
bit = 128;
while (--count >= 0)
	{
	if (*lpIn++ > *pPat++)
		byte |= bit;
	
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}

	if ( pPat >= pEndPat )
		pPat = pStartPat;
	}

if ( bit != 128 )
	*lpOut++ = byte;
}


/***********************************************************************/
LPTR stripdir( pathname )
/***********************************************************************/
LPTR pathname;
{
LPTR ptr;

ptr = pathname;
ptr += lstrlen( pathname );
while( ptr >= pathname && *ptr != '\\' && *ptr != ':' )
	ptr--;
ptr++;
return( ptr );
}


/***********************************************************************/
void lmemcpy( lpOut, lpIn, iCount )
/***********************************************************************/
LPTR	lpOut, lpIn;
int	iCount;
{
while ( --iCount >= 0 )
	*lpOut++ = *lpIn++;
}


/************************************************************************/
void mirror( pixels, count )
/************************************************************************/
LPTR	pixels;
int	count;
{
LPTR	endpixels;
BYTE	byte;

endpixels = pixels + (count - 1);

count /= 2;
while( --count >= 0 )
	{
	byte = *endpixels;
	*endpixels-- = *pixels;
	*pixels++ = byte;
	}
}


/************************************************************************/
void mirror16( pixels, count )
/************************************************************************/
LPWORD	pixels;
int	count;
{
LPWORD	endpixels;
WORD	word;

endpixels = pixels + (count - 1);

count /= 2;
while( --count >= 0 )
	{
	word = *endpixels;
	*endpixels-- = *pixels;
	*pixels++ = word;
	}
}


/************************************************************************/
void mirror24( pixels, count )
/************************************************************************/
LPTR	pixels;
int	count;
{
LPTR	endpixels;
BYTE r, g, b;

endpixels = pixels + ((count - 1)*3L);

count /= 2;
while( --count >= 0 )
	{
	r = *endpixels;
	g = *(endpixels+1);
	b = *(endpixels+2);
	*(endpixels) = *(pixels);
	*(endpixels+1) = *(pixels+1);
	*(endpixels+2) = *(pixels+2);
	endpixels -= 3;
	*pixels++ = r;
	*pixels++ = g;
	*pixels++ = b;
	}
}


#define FINT(f)	((int)HIWORD(f))
#define FRAC(f) ((unsigned)LOWORD(f))
/***********************************************************************/
FIXED	fget( numer, denom )
/***********************************************************************/
long	numer, denom;
{
long	val;
char	flag;

if ( numer < 0L )
	{
	numer = -numer;
	flag = 1;
	}
else	flag = 0;

while ( numer > 0xFFFFL )
	{
	numer++; numer >>= 1;
	denom++; denom >>= 1;
	}

if ( !denom )
	return(0);

val = ((unsigned long)numer << 16) / denom; /* always round it down */

if ( flag )
	return( -val );
else	return( val );
}


/***********************************************************************/
long	fmul( a, b )
/***********************************************************************/
long a, b;
{
ULONG t;
BYTE negative;

if ( negative = (a<0) )
	a = -a;
if ( b<0 )
	{
	b = -b;
	negative ^= 1;
	}

t = ( (ULONG)LOWORD(a) * LOWORD(b) ) + HALF;
t = HIWORD(t);

if ( HIWORD(a) )
	{
	t += ( (ULONG)HIWORD(a) * HIWORD(b) ) << 16;
	t += ( (ULONG)HIWORD(a) * LOWORD(b) );
	}
if ( HIWORD(b) )
	t += ( (ULONG)LOWORD(a) * HIWORD(b) );
if ( negative )
	return( -t );
else	return( t );
}


/***********************************************************************/
long	tmul( a, b )
/***********************************************************************/
long a, b;
{
ULONG t;
BYTE negative;

if ( negative = (a<0) )
	a = -a;
if ( b<0 )
	{
	b = -b;
	negative ^= 1;
	}

t = ( (ULONG)LOWORD(a) * LOWORD(b) );
t = HIWORD(t);

if ( HIWORD(a) )
	{
	t += ( (ULONG)HIWORD(a) * HIWORD(b) ) << 16;
	t += ( (ULONG)HIWORD(a) * LOWORD(b) );
	}
if ( HIWORD(b) )
	t += ( (ULONG)LOWORD(a) * HIWORD(b) );
if ( negative )
	return( -t );
else	return( t );
}


/***********************************************************************/
LPTR FixedAscii( Fixed, lpAscii )
/***********************************************************************/
FIXED	Fixed;
LPTR	lpAscii;
{
int	len, intg, frac;
char	flag, szFixed[16], *p;

if ( Fixed < 0L )
	{
	Fixed = -Fixed;
	flag = 1;
	}
else	flag = 0;

intg = FINT( Fixed );
frac = FMUL( 1000, FRAC( Fixed ) );
if ( frac >= 1000 )
	{
	frac = 0;
	intg += 1;
	}
if ( flag )
	{
	szFixed[0] = '-';
	itoa( intg, szFixed+1, 10 );
	}
else	itoa( intg, szFixed, 10 );
len = strlen( szFixed );
itoa( 1000+frac, &szFixed[len], 10 );
szFixed[len] = '.';
len = strlen( szFixed );
p = &szFixed[len] - 1;
while( *p == '0' )
	*p-- = '\0';
lstrcpy( lpAscii, szFixed );
return( lpAscii );
}


/***********************************************************************/
FIXED AsciiFixed( lpAscii )
/***********************************************************************/
LPTR	lpAscii;
{
char	szFixed[16], neg;
PSTR	pFixed, pFrac, p;
FIXED	Fixed, Frac;
int	len;
static int table[] = { 1, 10, 100, 1000 };
#define DECIMAL 3

pFixed = szFixed;
lstrcpy( pFixed, lpAscii );

/* See if there is a fractional part */
pFrac = 0;
p = pFixed;
neg = 0;
while ( *p )
	{
	if ( *p == '-' )
		neg = 1; /* to trap the '-.123' cases */
	if ( *p == '.' )
		{
		*p = '\0';
		pFrac = p+1;
		break;
		}
	p++;
	}

/* The integer part */
Fixed = FGET( atoi( pFixed ), 1 );
if ( pFrac )
	{
	if ( ( len = strlen( pFrac ) ) > 0 )
		{
		if ( len > DECIMAL )
			{
			len = DECIMAL;
			pFrac[len] = '\0';
			}
		/* The fractional part */
		Frac = FGET( atoi( pFrac ), table[len] );
		if ( Fixed < 0 || neg )
			Fixed -= Frac;
		else	Fixed += Frac;
		}
	}

return( Fixed );
}


/***********************************************************************/
PTR RGBAscii( lpRGB, pAscii )
/***********************************************************************/
LPRGB lpRGB;
PTR pAscii;
{
PTR pString;

pString = pAscii;
itoa( lpRGB->red, pString, 10 );
lstrcat( pString, "," );
pString += lstrlen( pString );
itoa( lpRGB->green, pString, 10 );
lstrcat( pString, "," );
pString += lstrlen( pString );
itoa( lpRGB->blue, pString, 10 );
return( pAscii );
}


/***********************************************************************/
void AsciiRGB( pAscii, lpRGB )
/***********************************************************************/
PTR pAscii;
LPRGB lpRGB;
{
PTR pString;

pString = pAscii;
while ( *pString )
	{
	if ( *pString == ',' )
		*pString = '\0';
	pString++;
	}
lpRGB->red = atoi( pAscii );
pAscii += (lstrlen( pAscii ) + 1);
lpRGB->green = atoi( pAscii );
pAscii += (lstrlen( pAscii ) + 1);
lpRGB->blue = atoi( pAscii );
}


/***********************************************************************/
FIXED ScaleToFit( DestWidth, DestHeight, SrcWidth, SrcHeight )
/***********************************************************************/
int	*DestWidth, *DestHeight, SrcWidth, SrcHeight;
{
FIXED	scale1, scale2, rate;

/* Scale the source window to fit the destination window... */
scale1 = FGET( *DestWidth, SrcWidth );
scale2 = FGET( *DestHeight, SrcHeight );

/* by using the smaller of the two scale factors */
if ( scale1 < scale2 )
	rate = scale1;
else	rate = scale2;

/* Adjust the desination size, and return the scale factor */
*DestHeight = FMUL( SrcHeight, rate );
*DestWidth  = FMUL( SrcWidth, rate );
return( rate );
}


/***********************************************************************/
void la2con( lpIn, iCount, lpOut, normal )
/***********************************************************************/
LPTR	lpIn, lpOut;
int	iCount, normal;
{
BYTE	byte, bit, black, white;

if ( normal )
	{ white = 0;  black = 255; }
else	{ white = 255; black = 0;  }

bit = 1;
while ( --iCount >= 0 )
	{
	if ( --bit == 0 )
		{
		byte = *lpIn++;
		bit = 8;
		}
	if ( byte > THRESH )
		*lpOut++ = black;
	else	*lpOut++ = white;
	byte <<= 1;
	}
}


/***********************************************************************/
void con2la( lpIn, iCount, lpOut )
/***********************************************************************/
LPTR	lpIn;
int	iCount;
LPTR	lpOut;
{
BYTE	byte, bit;

byte = 0;
bit = 128;
while ( --iCount >= 0 )
	{
	if (*lpIn++ > THRESH)
		byte |= bit;
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
}


#ifdef UNUSED
/************************************************************************/
wild_delete( path, file )
/************************************************************************/
PTR	path, file;
{
char	filename[MAX_FNAME_LEN];
char	data[MAX_FNAME_LEN];

chdir( path );
strcpy( filename, path );
strcat( filename, file );
bdos( 0x1a, (unsigned int)data, 0 );  /* set the disk transfer area */
if ( bdos(0x4e, (unsigned int)filename, 0) == 2 )  /* search for first */
	return( NO );
while (1) /* delete all scratch files */
	{
	strcpy( filename, path );
	strcat( filename, &data[30] );
	unlink( filename );
	if ( bdos(0x4f, 0, 0) == 18 )  /* search for next */
		return( YES );
	}
}
#endif


/************************************************************************/
BOOL FileExists( szFileName )
/************************************************************************/
LPTR	szFileName;
{
OFSTRUCT of;

if ( OpenFile( szFileName, &of, OF_READ|OF_EXIST ) == -1 )
	return( NO );
else	return( YES );
}


/************************************************************************/
LPTR itoh(i)			/* convert an int to ascii hex */
/************************************************************************/
WORD i;
{
static BYTE hex[5];

itoa( i, hex, 16 );
return( hex );
}


/************************************************************************/
int htoi(s)			/* convert ascii hex to an int */
/************************************************************************/
LPTR s;
{
WORD i, n, m;

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
LPTR Lowercase( lp )
/************************************************************************/
LPTR lp;
{
BYTE c;
LPTR lpString;

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
void negate( buf, lCount )
/************************************************************************/
LPTR buf;
long lCount;
{
while ( --lCount >= 0 )
	{
	*buf++ ^= 255;
	}
}


/************************************************************************/
void ramp( lut, inx1, outx1, inx2, outx2 )
/************************************************************************/
LPTR	lut;
int	inx1, inx2, outx1, outx2;
{
int	i;
FIXED	accum, rate;

rate = FGET( (outx2-outx1), (inx2-inx1) );
lut += inx1;
accum = ((ULONG)outx1 << 16) + HALF;
for ( i=inx1; i<=inx2; i++ )
	{
	*lut++ = HIWORD( accum );
	accum += rate;
	}
}

#ifdef C_CODE
/************************************************************************/
void copy( lptr1, lptr2, count )
/************************************************************************/
LPTR lptr1, lptr2;
int count;
{
while ( --count >= 0 )
	{
	*lptr2++ = *lptr1++;
	}
}

/************************************************************************/
void clr( ptr, iCount )
/************************************************************************/
LPTR	ptr;
int	iCount;
{
while ( --iCount >= 0 )
	*ptr++ = NULL;
}
#endif

/************************************************************************/
void lclr( ptr, lCount )
/************************************************************************/
LPTR	ptr;
long	lCount;
{
while ( --lCount >= 0 )
	*ptr++ = NULL;
}


/************************************************************************/
void set16( ptr, count, value )
/************************************************************************/
LPWORD	ptr;
int	count;
WORD	value;
{
while ( --count >= 0 )
	*ptr++ = value;
}


#ifdef C_CODE
/************************************************************************/
void set( ptr, count, value )
/************************************************************************/
LPTR	ptr;
int	count, value;
{
while ( --count >= 0 )
	*ptr++ = value;
}


/************************************************************************/
void set24( ptr, count, value )
/************************************************************************/
LPTR	ptr;
int	count;
long	value;
{
BYTE	b1, b2, b3;
RGBS rgb;

CopyRGB(&value, &rgb);
b1 = rgb.red;
b2 = rgb.green;
b3 = rgb.blue;

while ( --count >= 0 )
	{
	*ptr++ = b1;
	*ptr++ = b2;
	*ptr++ = b3;
	}
}
#endif

#ifdef UNUSED
/************************************************************************/
void lset( ptr, lCount, value )
/************************************************************************/
LPTR	ptr;
int	value;
long	lCount;
{
while ( --lCount >= 0 )
	*ptr++ = value;
}
#endif


/************************************************************************/
void hist( pixels, count, histo )
/************************************************************************/
LPTR	pixels;
int	count;
long	histo[256];
{
while ( --count >= 0 )
	{
	histo[*pixels++]++;
	}
}


/************************************************************************/
LPTR Alloc( lCount )
/************************************************************************/
long	lCount;
{
HANDLE	hMemory;
LPTR	lpMemory;

// Round up to the next 16 byte boundary
lCount = ((lCount + 15) / 16) * 16;
/* Allocate the memory from the global heap - NOT limited to 64K */
//Print("calling GlobalAlloc");
if ( !(hMemory = GlobalAlloc(GMEM_MOVEABLE, lCount )) )
	return( NULL );

/* Get the memory pointer from the handle */
//Print("calling GlobalLock");
if (Control.IsEnchanted)
	{
	if ( !(lpMemory = (LPTR)GlobalLock( hMemory )) )
		GlobalFree( hMemory );
	}
else
	{
	if ( !(lpMemory = (LPTR)GlobalWire( hMemory )) )
		GlobalFree( hMemory );
	}
//Print("back from GlobalLock");
if ( IsEMS(lpMemory) || ((long)lpMemory & 0xFFFFL) )
	{ /* Major problem */
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	}
//dbg( "alloc=%lx (%ld)", lpMemory, lCount );
return( lpMemory );
}


/************************************************************************/
LPTR AllocExtend( lpMemory, lCount )
/************************************************************************/
LPTR lpMemory;
long lCount;
{
HANDLE	hMemory;
long lValue;

if ( !lpMemory )
	return( NULL );
if ( IsEMS( lpMemory ) )
	return( lpMemory );

lValue = GlobalHandle( HIWORD(lpMemory) );
if ( HIWORD(lValue) != HIWORD(lpMemory) ) /* segment portions should match */
	{
	FreeUp( lpMemory );
	return( NULL );
	}

hMemory = LOWORD( lValue );

// Round up to the next 16 byte boundary
lCount = ((lCount + 15) / 16) * 16;
/* Allocate the memory from the global heap - NOT limited to 64K */
if ( !(hMemory = GlobalReAlloc( hMemory, lCount, GMEM_MOVEABLE )) )
	return( NULL );

/* Get the memory pointer from the handle */
if (Control.IsEnchanted)
	{
	if ( !(lpMemory = (LPTR)GlobalLock( hMemory )) )
		GlobalFree( hMemory );
	}
else
	{
	if ( !(lpMemory = (LPTR)GlobalWire( hMemory )) )
		GlobalFree( hMemory );
	}
if ( IsEMS(lpMemory) || ((long)lpMemory & 0xFFFFL) )
	{ /* Major problem */
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	}
// Print( "alloc=%lx (%d)", lpMemory, hMemory );
return( lpMemory );
}


/************************************************************************/
void FreeUp( lpMemory )
/************************************************************************/
LPTR	lpMemory;
{
HANDLE	hMemory;
long lValue;

if ( !lpMemory )
	return;
if ( IsEMS( lpMemory ) )
	{
	EMSfree( lpMemory );
	return;
	}

lValue = GlobalHandle( HIWORD(lpMemory) );
//dbg( "free=%lx", lpMemory );
if ( HIWORD(lValue) == HIWORD(lpMemory) ) /* segment portions should match */
	{
	hMemory = LOWORD( lValue );
	if (Control.IsEnchanted)
		GlobalUnlock( hMemory );
	else
		GlobalUnWire ( hMemory );
	GlobalFree( hMemory );
	}
else	{ /* Major problem */
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	MessageBeep(0);
	}
}


/************************************************************************/
DWORD AvailableMemory()
/************************************************************************/
{
return( GlobalCompact(0L) );
}


/************************************************************************/
DWORD ConventionalMemory()
/************************************************************************/
{
if ( Control.Is30 )
	return( GetFreeSpace(0) );
else	return( GlobalCompact(0L) );
}


/************************************************************************/
void swapw( word )
/************************************************************************/
LPWORD word;
{
LPTR pb;
BYTE b1, b2;

pb = (LPTR)(word);
b1 = *pb;
pb++;
b2 = *pb;
*pb = b1;
pb--;
*pb = b2;
}


/************************************************************************/
void swapl( dword )
/************************************************************************/
LPDWORD dword;
{
LPWORD pw;
WORD w1, w2;

pw = (LPWORD)(dword);
w1 = *pw;
pw++;
w2 = *pw;
*pw = w1;
swapw( pw );
pw--;
*pw = w2;
swapw( pw );
}


/************************************************************************/
void Unpack4( lpIn, iCount, lpOut )
/************************************************************************/
LPTR	lpIn, lpOut;
int	iCount;
{
BYTE	b;

while ( --iCount >= 0 )
	{
	b = *lpIn++;
	*lpOut++ = b & 0xF0;
	*lpOut++ = (b << 4);
	}
}


#ifdef UNUSED
/************************************************************************/
BOOL Random()
/************************************************************************/
{
static ULONG lSeed = 0xDEADBABA;

if ( lSeed & BIT18 )
	{
	lSeed += lSeed;
	lSeed ^= BITMASK;
	return( 1 );
	}
else	{
	lSeed += lSeed;
	return( 0 );
	}
}
#endif


/************************************************************************/
void ScaleRect( lpRect, Scale )
/************************************************************************/
LPRECT lpRect;
FIXED Scale;
{
lpRect->left   = FMUL( lpRect->left, Scale );
lpRect->top    = FMUL( lpRect->top, Scale );
lpRect->right  = FMUL( lpRect->right+1, Scale ) - 1;
lpRect->bottom = FMUL( lpRect->bottom+1, Scale ) - 1;
}


static WORD sintab[180] = {
    0, 1144, 2287, 3430, 4572, 5712, 6850, 7987, 9121,10252,
11380,12505,13626,14742,15855,16962,18064,19161,20252,21336,
22415,23486,24550,25607,26656,27697,28729,29753,30767,31772,
32768,33754,34729,35693,36647,37590,38521,39441,40348,41243,
42126,42995,43852,44695,45525,46341,47143,47930,48703,49461,
50203,50931,51643,52339,53020,53684,54332,54963,55578,56175,
56756,57319,57865,58393,58903,59396,59870,60326,60764,61183,
61584,61966,62328,62672,62997,63303,63589,63856,64104,64332,
64540,64729,64898,65048,65177,65287,65376,65446,65496,65526,
65535,65526,65496,65446,65376,65287,65177,65048,64898,64729,
64540,64332,64104,63856,63589,63303,62997,62672,62328,61966,
61584,61183,60764,60326,59870,59396,58903,58393,57865,57319,
56756,56175,55578,54963,54332,53684,53020,52339,51643,50931,
50203,49461,48703,47930,47143,46341,45525,44695,43852,42995,
42126,41243,40348,39441,38521,37590,36647,35693,34729,33754,
32768,31772,30767,29753,28729,27697,26656,25607,24550,23486,
22415,21336,20252,19161,18064,16962,15855,14742,13626,12505,
11380,10252, 9121, 7987, 6850, 5712, 4572, 3430, 2287, 1144 };

/************************************************************************/
void SinCos( iAngle, lplSin, lplCos )
/************************************************************************/
int iAngle;
LPLONG lplSin, lplCos;
{
long l;

while ( iAngle < 0 ) iAngle += 360;
while ( iAngle > 360 ) iAngle -= 360;
if ( iAngle < 180)
	{
	l =  (long)sintab[ iAngle ];
	if ( l ==  65535L ) l++;
	}
else	{
	l = -(long)sintab[ iAngle-180 ];
	if ( l == -65535L ) l--;
	}
*lplSin = l;

iAngle += 90;
if ( iAngle > 360 ) iAngle -= 360;
if ( iAngle < 180 )
	{
	l =  (long)sintab[ iAngle ];
	if ( l ==  65535L ) l++;
	}
else	{
	l = -(long)sintab[ iAngle-180 ];
	if ( l == -65535L ) l--;
	}
*lplCos = l;
}


/************************************************************************/
void Delay( lTicks )
/************************************************************************/
long lTicks;
{
DWORD dwBaseTime;

/* Pause for a few milliseconds */
dwBaseTime = GetTickCount() + lTicks;
while ( GetTickCount() < dwBaseTime )
	;
}


/************************************************************************/
BOOL DelayIfMouseDown( lTicks )
/************************************************************************/
long lTicks;
{
DWORD dwBaseTime;
POINT Point;
long lPoint;
int x, y;

lPoint = GetMessagePos();
x = LOWORD(lPoint);
y = HIWORD(lPoint);

/* Pause for a few milliseconds */
dwBaseTime = GetTickCount() + lTicks;
while ( GetTickCount() < dwBaseTime )
	{
	GetCursorPos(&Point);
	Point.x -= x;
	Point.y -= y;
	if ( !LBUTTON )
		return( NO );
	if ( abs(Point.x) > 2 || abs(Point.y) > 2 )
		return( YES );
	}
return( YES );
}


static unsigned int roots[257] = {
    0,    1,    4,    9,   16,   25,   36,   49,   64,   81,  100,  121,
  144,  169,  196,  225,  256,  289,  324,  361,  400,  441,  484,  529,
  576,  625,  676,  729,  784,  841,  900,  961, 1024, 1089, 1156, 1225,
 1296, 1369, 1444, 1521, 1600, 1681, 1764, 1849, 1936, 2025, 2116, 2209,
 2304, 2401, 2500, 2601, 2704, 2809, 2916, 3025, 3136, 3249, 3364, 3481,
 3600, 3721, 3844, 3969, 4096, 4225, 4356, 4489, 4624, 4761, 4900, 5041,
 5184, 5329, 5476, 5625, 5776, 5929, 6084, 6241, 6400, 6561, 6724, 6889,
 7056, 7225, 7396, 7569, 7744, 7921, 8100, 8281, 8464, 8649, 8836, 9025,
 9216, 9409, 9604, 9801,10000,10201,10404,10609,10816,11025,11236,11449,
11664,11881,12100,12321,12544,12769,12996,13225,13456,13689,13924,14161,
14400,14641,14884,15129,15376,15625,15876,16129,16384,16641,16900,17161,
17424,17689,17956,18225,18496,18769,19044,19321,19600,19881,20164,20449,
20736,21025,21316,21609,21904,22201,22500,22801,23104,23409,23716,24025,
24336,24649,24964,25281,25600,25921,26244,26569,26896,27225,27556,27889,
28224,28561,28900,29241,29584,29929,30276,30625,30976,31329,31684,32041,
32400,32761,33124,33489,33856,34225,34596,34969,35344,35721,36100,36481,
36864,37249,37636,38025,38416,38809,39204,39601,40000,40401,40804,41209,
41616,42025,42436,42849,43264,43681,44100,44521,44944,45369,45796,46225,
46656,47089,47524,47961,48400,48841,49284,49729,50176,50625,51076,51529,
51984,52441,52900,53361,53824,54289,54756,55225,55696,56169,56644,57121,
57600,58081,58564,59049,59536,60025,60516,61009,61504,62001,62500,63001,
63504,64009,64516,65025,65535 };

/************************************************************************/
WORD lsqrt( lValue )
/************************************************************************/
long lValue;
{
UINT i, r, hi, lo, Shifts;
long l;

Shifts = 0;
while ( lValue > 65535 )
	{
	lValue >>= 2;
	++Shifts;
	}

lo = 0;
hi = 256;
while (1)
	{
	i = (lo+hi) >> 1;
	if ( i == lo ) /* hi and lo must be consecutive */
		break;
	r = roots[i];
	l = lValue + i;
	if ( l <= r )
		hi = i;
	else
	if ( l >= r )
		lo = i;
	else	break;
	}
if ( Shifts )
	i <<= Shifts;
return( i );
}


/************************************************************************/
BOOL CopyFile( lpInFile, lpOutFile )
/************************************************************************/
LPTR lpInFile, lpOutFile;
{
int i, ifp, ofp;
BYTE Buffer[2048];

lstrcpy( Buffer, lpInFile );
if ((ifp = _lopen( Buffer, OF_READ )) < 0)
	return( FALSE );
lstrcpy( Buffer, lpOutFile );
if ((ofp = _lcreat( Buffer, 0 )) < 0)
	{
	_lclose( ifp );
	return( FALSE );
	}

i = sizeof(Buffer);
while( 1 )
	{
	if ( (i = _lread(ifp, Buffer, i)) <= 0 )
		{
		_lclose( ifp );
		_lclose( ofp );
		return( i == 0 );
		}
	if ( _lwrite(ofp, Buffer, i) != i )
		{
		_lclose( ifp );
		_lclose( ofp );
		return( FALSE );
		}
	}
}


/************************************************************************/
BOOL AllocLines( lpList, nLines, iWidth, iDepth )
/************************************************************************/
LPPTR lpList;
int nLines, iWidth, iDepth;
{
long lCount;
LPTR lp;
LPFRAME lpFrame;

if ( lpFrame = frame_set( NULL ) )
	{
	if ( iDepth <= 0 )
		iDepth = lpFrame->Depth;
	if ( iWidth <= 0 )
		iWidth = lpFrame->Width;
	}
if ( nLines <= 0 )
	nLines = 1;

iWidth *= iDepth;
iWidth = ((iWidth + 15) / 16) * 16;
lCount = (long)nLines * iWidth;
if ( !(lp = Alloc( lCount )) )
	return( NO );
while ( --nLines >= 0 )
	{
	*lpList++ = lp;
	lp += iWidth;
	}
return( YES );
}


/************************************************************************/
DWORD DiskSpace( lpPath )
/************************************************************************/
LPTR lpPath;
{
DWORD lSpace;
BYTE Device;
struct diskfree_t DiskFree;

Device = *lpPath++;
if ( *lpPath != ':' )
	Device = 0; // The current drive
else
if ( Device >= 'a' && Device <= 'z' )
	Device -= ( 'a' - 1 );
else
if ( Device >= 'A' && Device <= 'Z' )
	Device -= ( 'A' - 1 );

if ( _dos_getdiskfree( Device, &DiskFree ) )
	return( 0 );
lSpace = (DWORD)DiskFree.avail_clusters * DiskFree.sectors_per_cluster * 
		DiskFree.bytes_per_sector;
return( lSpace );
}


/************************************************************************/
void File2DispTForm(lpTForm)
/************************************************************************/
LPTFORM lpTForm;
{
TMove( lpTForm, -lpImage->lpDisplay->FileRect.left,
	-lpImage->lpDisplay->FileRect.top );
TScale( lpTForm, lpImage->lpDisplay->DispRate, lpImage->lpDisplay->DispRate);
TMove( lpTForm, lpImage->lpDisplay->DispRect.left,
	lpImage->lpDisplay->DispRect.top );
}

/************************************************************************/
void Disp2FileTForm(lpTForm)
/************************************************************************/
LPTFORM lpTForm;
{
TMove( lpTForm, -lpImage->lpDisplay->DispRect.left,
	-lpImage->lpDisplay->DispRect.top );
TScale( lpTForm, lpImage->lpDisplay->FileRate, lpImage->lpDisplay->FileRate);
TMove( lpTForm, lpImage->lpDisplay->FileRect.left,
	lpImage->lpDisplay->FileRect.top );
}

/************************************************************************/
BOOL StringsEqual(lpString1, lpString2)
/************************************************************************/
LPTR lpString1, lpString2;
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
LPTR GetCurrentDir(lpBuffer, maxlen)
/************************************************************************/
LPTR lpBuffer;
int maxlen;
{
FNAME szCurrentDir;

getcwd(szCurrentDir, MAX_FNAME_LEN);
lstrcpy(lpBuffer, szCurrentDir);
return(lpBuffer);
}

/************************************************************************/
BOOL SetCurrentDir(lpCurrentDir)
/************************************************************************/
LPTR lpCurrentDir;
{
FNAME szCurrentDir;
int status, length;

lstrcpy(szCurrentDir, lpCurrentDir);
if (szCurrentDir[1] == ':')
	{
	status = _chdrive(tolower(szCurrentDir[0]) - 'a' + 1);
	if (status != 0)
		return(FALSE);
	lstrcpy(szCurrentDir, lpCurrentDir+2);
	}
length = strlen(szCurrentDir);
while (length && szCurrentDir[length-1] == '\\')
	{
	szCurrentDir[length-1] = '\0';
	--length;
	}
status = chdir(szCurrentDir);
return(status == 0);
}

/************************************************************************/
HANDLE AstralLoadLibrary(lpLibraryName)
/************************************************************************/
LPTR lpLibraryName;
{
LPTR lpName;
FNAME szLibraryDir;
FNAME szCurrentDir;
HANDLE hModule;

lpName = stripdir(lpLibraryName);
lstrcpy(szLibraryDir, lpLibraryName);
stripfile(szLibraryDir);
GetCurrentDir(szCurrentDir, MAX_FNAME_LEN);
if (lstrlen(szLibraryDir) > 0)
	SetCurrentDir(szLibraryDir);
hModule = LoadLibrary(lpName);
SetCurrentDir(szCurrentDir);
return(hModule);
}


