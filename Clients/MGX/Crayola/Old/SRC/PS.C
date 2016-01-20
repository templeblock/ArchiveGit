//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static BOOL SendPSIDString(int id, ...);
static BOOL SendCRLF(void);
static BOOL SendPSData( BOOL Asciize, LPTR lpBuffer, int dxDest );
static void UnshuffleRGB( LPRGB lpRGB, LPTR lpRed, LPTR lpGreen, LPTR lpBlue, int iCount );

/************************************************************************/
/*  ps.c   PostScript printing functions for astral's picture publisher */
/************************************************************************/

extern HDC hPrinterDC;
extern ABORTPROC lpAbortTest;
extern BOOL fAbortPrint;	/* abort sending job to spooler? */

typedef struct _laser
{
	int	nCount;
	char	cData[1];
} LASER, far *LPLASER;

#define PS_STR(str)\
{ \
	if ( !LaserPrint(str) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

#define PS_ID(id)\
{ \
	if ( !SendPSIDString(id) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

#define PS_ID1(id,a)\
{ \
	if ( !SendPSIDString(id,a) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

#define PS_ID2(id,a,b)\
{ \
	if ( !SendPSIDString(id,a,b) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

#define PS_ID3(id,a,b,c)\
{ \
	if ( !SendPSIDString(id,a,b,c) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

#define PS_ID4(id,a,b,c,d)\
{ \
	if ( !SendPSIDString(id,a,b,c,d) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

#define PS_ID5(id,a,b,c,d,e)\
{ \
	if ( !SendPSIDString(id,a,b,c,d,e) ) \
		goto ErrorExit; \
	if ( !SendCRLF()) \
		goto ErrorExit; \
}

/***********************************************************************/
//	use lpImage or lpFrame
/***********************************************************************/
BOOL PSPrint(
	LPIMAGE lpImage,
	LPFRAME lpFrame,
	BYTE    cSep,
	int     xSrc,
	int     ySrc,
	int     dxSrc,
	int     dySrc,
	int     xDest,
	int     yDest,
	int     dxDest,
	int     dyDest,
	int     iPrResX,
	int     iPrResY )
{
int y, yline, ystart, ylast, x, depth;
LFIXED yrate, yoffset;
LPTR lpBuffer[5], p1Buf, p2Buf, p3Buf, p4Buf;
LPSTR lpAngle, lpRuling;
LPTR lpImageData;
BOOL Negative, Asciize;
STRING szAngle, szRuling;
long lSize;
LPFRAME lpBaseFrame;

#define C_ANGLE Halftone.ScreenAngle[0]
#define M_ANGLE Halftone.ScreenAngle[1]
#define Y_ANGLE Halftone.ScreenAngle[2]
#define K_ANGLE Halftone.ScreenAngle[3]
#define C_RULING Halftone.ScreenRuling[0]
#define M_RULING Halftone.ScreenRuling[1]
#define Y_RULING Halftone.ScreenRuling[2]
#define K_RULING Halftone.ScreenRuling[3]

ProgressBegin(1,0);

lpAngle = szAngle;
lpRuling = szRuling;
Negative = Page.Negative;
Asciize = !Page.BinaryPS;

PS_ID( IDS_PS_DICTDEF );

/* Send the definition of the read data function */
if ( Asciize )
{
	PS_ID( IDS_PS_HEXDATA );
}
else
{
	PS_ID( IDS_PS_BINDATA );
}

if ( !Halftone.DoHalftoning )
	goto HalftoningDone;

/* Send the definition of the spot function */
if ( Halftone.DotShape == IDC_ELLIPSEDOT )
	{
	PS_ID( IDS_PS_ELLDOT1 );
	PS_ID( IDS_PS_ELLDOT2 );
	}
else
if ( Halftone.DotShape == IDC_SQUAREDOT )
	{
	PS_ID( IDS_PS_SQUDOT );
	}
else
if ( Halftone.DotShape == IDC_CIRCLEDOT )
	{
	PS_ID( IDS_PS_CIRDOT );
	}
else
if ( Halftone.DotShape == IDC_TRIANGLEDOT )
	{
	PS_ID( IDS_PS_TRIDOT );
	}
else
if ( Halftone.DotShape == IDC_PROPELLERDOT )
	{
	PS_ID( IDS_PS_PROPDOT );
	}

if ( Page.OutputType == IDC_PRINT_BLACKSEPS ||
     Page.OutputType == IDC_PRINT_GRAY )
	{ // Setup the "image" screen angles and freqs based on the sep
	if ( cSep == 'C' )
		{
		FixedAscii( C_ANGLE,  lpAngle,  -2 );
		FixedAscii( C_RULING, lpRuling, -2 );
		}
	else
	if ( cSep == 'M' )
		{
		FixedAscii( M_ANGLE,  lpAngle,  -2 );
		FixedAscii( M_RULING, lpRuling, -2 );
		}
	else
	if ( cSep == 'Y' )
		{
		FixedAscii( Y_ANGLE,  lpAngle,  -2 );
		FixedAscii( Y_RULING, lpRuling, -2 );
		}
	else
	//if ( cSep == 'K' || cSep == 'X' || !cSep )
		{
		FixedAscii( K_ANGLE,  lpAngle,  -2 );
		FixedAscii( K_RULING, lpRuling, -2 );
		}
	PS_ID2( IDS_PS_SETSCREEN, lpRuling, lpAngle );
	}
else
	{ // Setup the "colorimage" screen angles and frequencies
	PS_ID( IDS_PS_COLOREXT );
	PS_ID( IDS_PS_STARTBLOCK );

	FixedAscii( C_RULING, lpRuling, -2 );
	FixedAscii( C_ANGLE,  lpAngle,  -2 );
	PS_ID2( IDS_PS_SETSPOT, lpRuling, lpAngle );

	FixedAscii( M_RULING, lpRuling, -2 );
	FixedAscii( M_ANGLE,  lpAngle,  -2 );
	PS_ID2( IDS_PS_SETSPOT, lpRuling, lpAngle );

	FixedAscii( Y_RULING, lpRuling, -2 );
	FixedAscii( Y_ANGLE,  lpAngle,  -2 );
	PS_ID2( IDS_PS_SETSPOT, lpRuling, lpAngle );

	FixedAscii( K_RULING, lpRuling, -2 );
	FixedAscii( K_ANGLE,  lpAngle,  -2 );
	PS_ID2( IDS_PS_SETSPOT, lpRuling, lpAngle );
	PS_ID( IDS_PS_SETCOLORSCREEN );
	PS_ID( IDS_PS_ENDBLOCK );

	PS_ID( IDS_PS_STARTBLOCK );
	FixedAscii( K_RULING, lpRuling, -2 );
	FixedAscii( K_ANGLE,  lpAngle,  -2 );
	PS_ID2( IDS_PS_SETSCREEN, lpRuling, lpAngle );
	PS_ID( IDS_PS_ENDBLOCK );
	PS_ID( IDS_PS_IFELSE );
	}

HalftoningDone:

// Setup a null transfer curve unless doing seps w/black ink (image operator)
if ( Page.OutputType == IDC_PRINT_BLACKSEPS )
{
	PS_ID( IDS_PS_BLACKSEPS );
}
else
if ( Page.OutputType == IDC_PRINT_COLORSEPS )
{
	PS_ID( IDS_PS_COLORSEPS );
}
else
{
	PS_ID( IDS_PS_NOINVERT );
}

PS_ID( IDS_PS_CHECKINVERT );

/* Send the destination point (x,y) in spots */
PS_ID2( IDS_PS_TRANSLATE, xDest, yDest );

/* Send the destination size (w,h) in spots */
PS_ID2( IDS_PS_SCALE, dxDest, dyDest );

if (lpImage)
	lpBaseFrame =  ImgGetBaseEditFrame(lpImage);
else 
	lpBaseFrame = lpFrame;
/* Compute how many pixels we're going to send */
/* Never send more than 16 pixels per halftone grid (or 4/grid in x and y) */
if (depth = FrameDepth( lpBaseFrame ))
	{
	if ( iPrResX < 600 )
		dxDest /= 4;
	else	dxDest /= 8;
	if ( iPrResY < 600 )
		dyDest /= 4;
	else	dyDest /= 8;
	}

if (depth == 0) depth = 1;

/* Let the printer do any upsizing */
if ( dySrc < dyDest )
	{
	yrate = UNITY;
	dxDest = dxSrc;
	dyDest = dySrc;
	}
else yrate = FGET( dySrc, dyDest );

/* Send the definition for the line buffers */
PS_ID1( IDS_PS_LINE1, dxDest );
PS_ID1( IDS_PS_LINE2, dxDest );
PS_ID1( IDS_PS_LINE3, dxDest );
PS_ID1( IDS_PS_LINE4, dxDest );
PS_ID1( IDS_PS_LINE5, dxDest );

if ( cSep ) // Plane at a time
{ // cSep is either 'C', 'M', 'Y', 'K', 'X'(gray) or NULL
	if ( Page.OutputType == IDC_PRINT_COLORSEPS )
	{
		PS_ID( IDS_PS_DOCOLORSEPDEF );
	}
	else
	{
		PS_ID( IDS_PS_NOCOLORSEPDEF );
	}

	PS_ID( IDS_PS_DOIMAGEDEF );
	PS_ID( IDS_PS_COLORSEPVAL );

	// Start color image proc
	PS_ID( IDS_PS_STARTBLOCK );
	PS_ID( IDS_PS_STARTBLOCK );

	if ( cSep == 'C' )
	{
		PS_ID( IDS_PS_SEPCYAN );
	}
	else
	if ( cSep == 'M' )
	{
		PS_ID( IDS_PS_SEPMAGENTA );
	}
	else
	if ( cSep == 'Y' )
	{
		PS_ID( IDS_PS_SEPYELLOW );
	}
	else
	if ( cSep == 'K' || cSep == 'X' )
	{
		PS_ID( IDS_PS_SEPBLACK );
	}

	PS_ID( IDS_PS_COLORIMAGE4 );

	PS_ID( IDS_PS_ENDBLOCK );
	PS_ID( IDS_PS_ENDBLOCK );

	// Start gray image proc
	PS_ID( IDS_PS_STARTBLOCK );
	PS_ID( IDS_PS_STARTBLOCK );

	PS_ID( IDS_PS_GETLINE1 );
	PS_ID( IDS_PS_IMAGE );
	PS_ID( IDS_PS_ENDBLOCK );
	PS_ID( IDS_PS_ENDBLOCK );

	PS_ID( IDS_PS_IFELSE );
	PS_ID( IDS_PS_DEF );
}
else
if ( Page.Type == IDC_PRINTER_IS_CMYK )
{
	if (Page.OutputType == IDC_PRINT_COLORGRAY)
	{
		PS_ID( IDS_PS_DOIMAGEDEF );
		PS_ID( IDS_PS_COLOREXT );

		// Start color image proc
		PS_ID( IDS_PS_STARTBLOCK );
			PS_ID( IDS_PS_STARTBLOCK );

				PS_ID( IDS_PS_GETLINE1 );
				PS_ID( IDS_PS_GETLINE2 );
				PS_ID( IDS_PS_GETLINE3 );

				PS_ID( IDS_PS_STARTBLOCK );
					PS_ID( IDS_PS_GETLINE4 );
					PS_ID( IDS_PS_GETLINE5 );
					PS_ID( IDS_PS_POP );
				PS_ID( IDS_PS_ENDBLOCK );

				PS_ID( IDS_PS_COLORIMAGE4 );
			PS_ID( IDS_PS_ENDBLOCK );
		PS_ID( IDS_PS_ENDBLOCK );

		// Start gray image proc
		PS_ID( IDS_PS_STARTBLOCK );
			PS_ID( IDS_PS_STARTBLOCK );
				PS_ID( IDS_PS_STARTBLOCK );
					PS_ID( IDS_PS_DUMPLINE1 );
					PS_ID( IDS_PS_DUMPLINE2 );
					PS_ID( IDS_PS_DUMPLINE3 );
					PS_ID( IDS_PS_STARTBLOCK );
						PS_ID( IDS_PS_GETLINE4 );
						PS_ID( IDS_PS_GETLINE5 );
					PS_ID( IDS_PS_ENDBLOCK );
					PS_ID( IDS_PS_IMAGE );
				PS_ID( IDS_PS_ENDBLOCK );
			PS_ID( IDS_PS_ENDBLOCK );
		PS_ID( IDS_PS_ENDBLOCK );

		PS_ID( IDS_PS_IFELSE );
		PS_ID( IDS_PS_DEF );
	}
	else
	{
		PS_ID( IDS_PS_DOIMAGEDEF );
		PS_ID( IDS_PS_COLOREXT );

		// Start color image proc
		PS_ID( IDS_PS_STARTBLOCK );
			PS_ID( IDS_PS_STARTBLOCK );
				PS_ID( IDS_PS_GETLINE1 );
				PS_ID( IDS_PS_GETLINE2 );
				PS_ID( IDS_PS_GETLINE3 );
				PS_ID( IDS_PS_GETLINE4 );
				PS_ID( IDS_PS_COLORIMAGE4 );
			PS_ID( IDS_PS_ENDBLOCK );
		PS_ID( IDS_PS_ENDBLOCK );

		// Start gray image proc
		PS_ID( IDS_PS_STARTBLOCK );
			PS_ID( IDS_PS_STARTBLOCK );
				PS_ID( IDS_PS_STARTBLOCK );
					PS_ID( IDS_PS_DUMPLINE1 );
					PS_ID( IDS_PS_DUMPLINE2 );
					PS_ID( IDS_PS_DUMPLINE3 );
					PS_ID( IDS_PS_GETLINE4 );
				PS_ID( IDS_PS_ENDBLOCK );
				PS_ID( IDS_PS_NOIMAGE );
			PS_ID( IDS_PS_ENDBLOCK );
		PS_ID( IDS_PS_ENDBLOCK );

		PS_ID( IDS_PS_IFELSE );
		PS_ID( IDS_PS_DEF );
	}
}
else
if ( Page.Type == IDC_PRINTER_IS_RGB )
{
	PS_ID( IDS_PS_DOIMAGEDEF );
	PS_ID( IDS_PS_COLOREXT );

	// Start color image proc
	PS_ID( IDS_PS_STARTBLOCK );
	PS_ID( IDS_PS_STARTBLOCK );

	PS_ID( IDS_PS_GETLINE1 );
	PS_ID( IDS_PS_GETLINE2 );
	PS_ID( IDS_PS_GETLINE3 );

	if ( Page.OutputType == IDC_PRINT_COLORGRAY )
	{
		PS_ID( IDS_PS_DUMPLINE4 );
	}

	PS_ID( IDS_PS_COLORIMAGE3 );
	PS_ID( IDS_PS_ENDBLOCK );
	PS_ID( IDS_PS_ENDBLOCK );

	// Start gray image proc
	PS_ID( IDS_PS_STARTBLOCK );
	PS_ID( IDS_PS_STARTBLOCK );

	PS_ID( IDS_PS_STARTBLOCK );
	PS_ID( IDS_PS_DUMPLINE1 );
	PS_ID( IDS_PS_DUMPLINE2 );

	if ( Page.OutputType == IDC_PRINT_COLORGRAY )
	{
		PS_ID( IDS_PS_DUMPLINE3 );
		PS_ID( IDS_PS_GETLINE4 );
		PS_ID( IDS_PS_ENDBLOCK );
		PS_ID( IDS_PS_IMAGE );
	}
	else
	{
		PS_ID( IDS_PS_GETLINE3 );
		PS_ID( IDS_PS_ENDBLOCK );
		PS_ID( IDS_PS_NOIMAGE );
	}
	PS_ID( IDS_PS_ENDBLOCK );
	PS_ID( IDS_PS_ENDBLOCK );
	PS_ID( IDS_PS_IFELSE );
	PS_ID( IDS_PS_DEF );
}

/* Send the inline image's size, packing, and transform */
PS_ID5( IDS_PS_TRANSFORM, dxDest, dyDest, 8, dxDest, dyDest );

if ( !Asciize )
{
	// The size must include the doimage command that follows
	lSize = (long)dxDest * dyDest * depth;
	PS_ID1( IDS_PS_BEGINBINARY, lSize + 9 + 2 );
}

PS_ID( IDS_PS_DOIMAGE ); // Should be 9 characters

for ( x=0; x<5; x++ )
	lpBuffer[x] = NULL;

if (!AllocLines((LPPTR)&lpImageData, 1, dxSrc, depth))
{
	ProgressEnd();
	return(FALSE);
}

if (!AllocLines((LPPTR)&lpBuffer[0], 1, dxDest, depth))
{
	FreeUp( lpImageData );
	ProgressEnd();
	return( FALSE );
}

if (!AllocLines((LPPTR)&lpBuffer[1], 4, dxDest, 1))
{
	FreeUp(lpBuffer[0]);
	FreeUp( lpImageData );
	ProgressEnd();
	return( FALSE );
}

p1Buf  = lpBuffer[1];
p2Buf  = lpBuffer[2];
p3Buf  = lpBuffer[3];
p4Buf  = lpBuffer[4];

ystart  = ySrc;
yline   = -1;
yoffset = (long)yrate>>1;

for (y=0; y<dyDest; y++)
{
	if (AstralClockCursor( y, dyDest, YES ))
	{
		fAbortPrint = YES;
		break;
	}

	/* Check for user input to abort dialog box */
	(*lpAbortTest)(hPrinterDC, 0);
	if ( fAbortPrint )
		break;

	ylast = yline;
#ifdef WIN32
	yline = ystart + WHOLE( yoffset );
#else
	yline = ystart + HIWORD( yoffset );
#endif
	yoffset += yrate;
	if ( yline != ylast )
	{
		LFIXED rate;

		if (lpImage)
			ImgGetLine( lpImage, NULL, xSrc, yline, dxSrc, lpImageData );
		else
			copy(FramePointer(lpBaseFrame, xSrc, yline, NO), lpImageData, dxSrc*depth);
		rate = FGET( dxSrc, dxDest );

		FrameSample(
			lpBaseFrame,
			lpImageData,
			0,
			lpBuffer[0],
			0,
			dxDest,
			rate);
	}

	if ( cSep )
	{ // cSep is either 'C', 'M', 'Y', 'K', 'X'(gray) or NULL
		if (cSep != 'X')
		{
			LPTR lpOutBuf;
			int  iPlaneOffset;

			switch(cSep)
			{
				case 'C' : lpOutBuf = p1Buf; iPlaneOffset = 0; break;
				case 'M' : lpOutBuf = p2Buf; iPlaneOffset = 1; break;
				case 'Y' : lpOutBuf = p3Buf; iPlaneOffset = 2; break;
				case 'K' : lpOutBuf = p4Buf; iPlaneOffset = 3; break;
			}

			switch(depth)
			{
				case 0 :
				case 1 :
					lpOutBuf = (LPTR)lpBuffer[0];
				break;

				case 3 :
					ClrRGBtoCMYK( (LPRGB)lpBuffer[0],
						p1Buf, p2Buf, p3Buf,p4Buf,dxDest,YES);
				break;

				case 4 :
				{
					LPTR lpSrc  = (LPTR)lpBuffer[0];
					LPTR lpDst  = lpOutBuf;
					int  iCount = dxDest;

					lpSrc += iPlaneOffset;

					while(iCount-- > 0)
					{
						*lpDst++ = *lpSrc;
						lpSrc += 4;
					}
				}
				break;
			}

			if (Negative)
				negate(lpOutBuf, (long)dxDest);
			if ( !SendPSData( Asciize, lpOutBuf, dxDest ) )
				goto ErrorExit;
		}
		else
		{
			ConvertData( lpBuffer[0], depth, dxDest, p1Buf, 1 );
			if (Negative)
				negate(p1Buf, (long)dxDest);
			CorrectGray( p1Buf, dxDest, YES, YES );
			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
		}
	}
	else if ( Page.Type == IDC_PRINTER_IS_CMYK )
	{
		switch(depth)
		{
			case 0 :
			case 1 :
				copy( lpBuffer[0], p1Buf, dxDest );
				copy( lpBuffer[0], p2Buf, dxDest );
				copy( lpBuffer[0], p3Buf, dxDest );
				copy( lpBuffer[0], p4Buf, dxDest );
			break;

			case 3 :
				ClrRGBtoCMYK( (LPRGB)lpBuffer[0],
					p1Buf, p2Buf, p3Buf,p4Buf,dxDest,YES);
			break;

			case 4 :
			{
				LPTR lpSrc  = (LPTR)lpBuffer[0];
				LPTR lpDst1 = p1Buf;
				LPTR lpDst2 = p2Buf;
				LPTR lpDst3 = p3Buf;
				LPTR lpDst4 = p4Buf;
				int  iCount = dxDest;

				while(iCount-- > 0)
				{
					*lpDst1++ = *lpSrc++;
					*lpDst2++ = *lpSrc++;
					*lpDst3++ = *lpSrc++;
					*lpDst4++ = *lpSrc++;
				}
			}
			break;
		}

		if (Negative)
		{
			negate(p1Buf, (long)dxDest);
			negate(p2Buf, (long)dxDest);
			negate(p3Buf, (long)dxDest);
			negate(p4Buf, (long)dxDest);
		}

		if ( !SendPSData( Asciize, p1Buf, dxDest ) )
			goto ErrorExit;
		if ( !SendPSData( Asciize, p2Buf, dxDest ) )
			goto ErrorExit;
		if ( !SendPSData( Asciize, p3Buf, dxDest ) )
			goto ErrorExit;
		if ( !SendPSData( Asciize, p4Buf, dxDest ) )
			goto ErrorExit;

		if ( Page.OutputType == IDC_PRINT_COLORGRAY )
		{
			ConvertData( lpBuffer[0], depth, dxDest, p1Buf, 1 );

			if (Negative)
				negate(p1Buf, (long)dxDest);

			CorrectGray( p1Buf, dxDest, YES, YES);

			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
		}
	}
	else if ( Page.Type == IDC_PRINTER_IS_RGB )
	{
		switch(depth)
		{
			case 0 :
			case 1 :
				copy( lpBuffer[0], p1Buf, dxDest );
				copy( lpBuffer[0], p2Buf, dxDest );
				copy( lpBuffer[0], p3Buf, dxDest );
			break;

			case 3 :
				UnshuffleRGB( (LPRGB)lpBuffer[0],
					p1Buf, p2Buf, p3Buf, dxDest );
			break;

			case 4 :
			{
				LPCMYK lpCMYK = (LPCMYK)lpBuffer[0];
				LPTR lpDst1 = p1Buf;
				LPTR lpDst2 = p2Buf;
				LPTR lpDst3 = p3Buf;
				RGBS rgb;
				int  iCount = dxDest;

				while(iCount-- > 0)
				{
					CMYKtoRGB(lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);
					lpCMYK++;

					*lpDst1++ = rgb.red;
					*lpDst2++ = rgb.green;
					*lpDst3++ = rgb.blue;
				}
			}
			break;
		}

		if (Negative)
		{
			negate( p1Buf, dxDest );
			negate( p2Buf, dxDest );
			negate( p3Buf, dxDest );
		}

		if ( !SendPSData( Asciize, p1Buf, dxDest ) )
			goto ErrorExit;

		if ( !SendPSData( Asciize, p2Buf, dxDest ) )
			goto ErrorExit;

		if ( !SendPSData( Asciize, p3Buf, dxDest ) )
			goto ErrorExit;

		if ( Page.OutputType == IDC_PRINT_COLORGRAY )
		{
			ConvertData( (LPTR)lpBuffer[0], depth, dxDest, p1Buf, 1 );

			if (Negative)
				negate(p1Buf, (long)dxDest);

			CorrectGray( p1Buf, dxDest, YES, YES);

			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
		}
	}
}

if ( !Asciize )
{
	PS_ID( IDS_PS_ENDBINARY );
}

/* Send the save restore command */
PS_ID( IDS_PS_MYSAVERESTORE );
PS_ID( IDS_PS_END );

ErrorExit:

if ( lpBuffer[0] )
	FreeUp(lpBuffer[0]);
if ( lpBuffer[1] )
	FreeUp(lpBuffer[1]);
if ( lpImageData )
	FreeUp( lpImageData );
ProgressEnd();
return( TRUE );
}


/************************************************************************/
void copyPS(
/************************************************************************/
LPTR 	lptr1, 
LPTR	lptr2,
int 	count)
{
BYTE c;

while ( --count >= 0 )
	{
	*lptr2 = *lptr1++;
	c = *lptr2 & 0x7F;
	if ( c == 0x1A || c == 0x03 || c == 0x09 || c == 0xEB )
		*lptr2 -= 1;
	else
	if ( c == 0x04 || c == 0x0A || c == 0x0D || c == 0x1B || c == 0x14 )
		*lptr2 += 1;
	lptr2++;
	}
}


static int iBlockLevel = 0;

/***********************************************************************/
static BOOL SendPSIDString(int id, ...)
/***********************************************************************/
{
	LPLASER lpLaser;
	LPTR    lpFormat;
	LPTR    lpArguments;
	int     iErr, i;

	lpArguments = (LPTR)&id + sizeof(id);

	if (AstralStr(id, (LPSTR FAR *)&lpFormat) <= 0)
		return(FALSE);

	lpLaser = (LPLASER)LineBuffer[0];
	/* Send short escape sequences to the Intel or DPTek grayscale */
	/* board with arguments specified exactly like printf */

	if (id == IDS_PS_ENDBLOCK)
		iBlockLevel--;

	if (iBlockLevel < 0)
	{
		iBlockLevel = 0;

		iBlockLevel = 4;

		for (i = 0;i < iBlockLevel*4;i++)
		{
			lpLaser->cData[i] = '%';
		}

		lpLaser->nCount = wvsprintf( &lpLaser->cData[iBlockLevel*4], (LPSTR)lpFormat, (LPSTR)lpArguments );
		lpLaser->nCount += (iBlockLevel * 4);
		iErr = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPSTR)lpLaser, 0 );

		iBlockLevel = 0;
	}
	else
	{
		for (i = 0;i < iBlockLevel*4;i++)
		{
			lpLaser->cData[i] = ' ';
		}
		lpLaser->nCount = wvsprintf( &lpLaser->cData[iBlockLevel*4], (LPSTR)lpFormat, (LPSTR)lpArguments );
		lpLaser->nCount += (iBlockLevel * 4);
		iErr = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPSTR)lpLaser, 0 );
	}

	if (id == IDS_PS_STARTBLOCK)
		iBlockLevel++;

	return( iErr >= 0 );
}

/***********************************************************************/
static BOOL SendCRLF()
/***********************************************************************/
{
	BOOL bResult;
	int iSaveLevel = iBlockLevel;

	iBlockLevel = 0;
	
	bResult = SendPSIDString(IDS_PS_CRLF);

	iBlockLevel = iSaveLevel;

	return(bResult);
}

/************************************************************************/
static BOOL SendPSData( BOOL Asciize, LPTR lpBuffer, int dxDest )
/************************************************************************/
{
typedef struct _laser
	{
	int	nCount;
	char	cData[1];
	} LASER;
typedef LASER far *LPLASER;
LPTR lpBits;
LPLASER	lpLaser;
int iErr;

lpLaser = (LPLASER)LineBuffer[0];
lpBits = (LPTR)lpLaser->cData;

if ( Asciize )
	lpLaser->nCount = asciize( lpBuffer, dxDest, lpBits );
else	{
	copyPS( lpBuffer, lpBits, dxDest );
	lpLaser->nCount = dxDest;
	}

/* Send raw data to the PostScript device */
iErr = Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPSTR)lpLaser, 0 );
return( iErr >= 0 );
}


/************************************************************************/
static void UnshuffleRGB( LPRGB lpRGB, LPTR lpRed, LPTR lpGreen, LPTR lpBlue, int iCount )
/************************************************************************/
{
LPTR lp;

lp = (LPTR)lpRGB;
while ( --iCount >= 0 )
	{
	*lpRed++   = *lp++;
	*lpGreen++ = *lp++;
	*lpBlue++  = *lp++;
	}
}


static char aschex[16] = 
{ '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

/************************************************************************/
int asciize(
/************************************************************************/
	LPTR 	in,
	int 	count,
	LPTR 	out)
{
	register int lCount;
	register int rCount;
	unsigned char temp;

	rCount = (2*count);
	lCount = 0;

	while (--count >= 0)
	{
		temp =*in++;
		*out++ = aschex[((temp >> 4) & 15)];
		*out++ = aschex[temp & 15];
		if (++lCount >= 40)
		{
			*out++ = '\r';
			*out++ = '\n';
			rCount += 2;
			lCount = 0;
		}
	}
	*out++ = '\r';
	*out++ = '\n';
	rCount += 2;
	*out = '\0';

	return( rCount );
}

