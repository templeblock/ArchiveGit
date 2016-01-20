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

/************************************************************************/
/*  ps.c   PostScript printing functions for astral's picture publisher */
/************************************************************************/

extern HDC hPrinterDC;
extern FARPROC	lpAbortTest;
extern BOOL	fAbortPrint;	/* abort sending job to spooler? */

#define PS(s) \
	{ if ( LaserPrint(s) < 0 ) goto ErrorExit; }
#define P1(s,a) \
	{ if ( LaserPrint(s,a) < 0 ) goto ErrorExit; }
#define P2(s,a,b) \
	{ if ( LaserPrint(s,a,b) < 0 ) goto ErrorExit; }
#define P4(s,a,b,c,d) \
	{ if ( LaserPrint(s,a,b,c,d) < 0 ) goto ErrorExit; }
#define P5(s,a,b,c,d,e) \
	{ if ( LaserPrint(s,a,b,c,d,e) < 0 ) goto ErrorExit; }

/***********************************************************************/
BOOL PSPrint( cSep, xSrc,  ySrc,  dxSrc,  dySrc, xDest, yDest,
	      dxDest, dyDest, iPrResX, iPrResY )
/***********************************************************************/
BYTE cSep;
int xSrc, ySrc, dxSrc, dySrc;
int xDest, yDest, dxDest, dyDest, iPrResX, iPrResY;
{
/* For PostScript devices... */
int	y, yline, ystart, ylast, ret, x;
int	iGridSize;
FIXED	yrate, yoffset;
LPTR	lp, lpBuffer[5], p1Buf, p2Buf, p3Buf, p4Buf;
LPRGB	lpRGB;
BYTE	C, M, Y, K, r, g, b;
BOOL	Negative, Asciize;
STRING	szAngle, szRuling;
LPTR	lpAngle, lpRuling;

#define C_ANGLE Printer.ScreenAngle[0]
#define M_ANGLE Printer.ScreenAngle[1]
#define Y_ANGLE Printer.ScreenAngle[2]
#define K_ANGLE Printer.ScreenAngle[3]
#define C_RULING Printer.ScreenRuling[0]
#define M_RULING Printer.ScreenRuling[1]
#define Y_RULING Printer.ScreenRuling[2]
#define K_RULING Printer.ScreenRuling[3]

lpAngle = szAngle;
lpRuling = szRuling;
ret = TRUE;
Negative = Printer.Negative;
Asciize = YES;

PS( "40 dict begin\r\n" );
PS( "/mysave save def\r\n" );
PS( "/noimage { {pop 1} settransfer image } def\r\n" );
PS( "/colorext systemdict /colorimage known def\r\n" );
/* Send the definition of the read data function */
if ( Asciize )
     { PS( "/readdata { currentfile exch readhexstring pop } bind def\r\n" ); }
else { PS( "/readdata { currentfile exch readstring pop } bind def\r\n" ); }

if ( !Printer.DoHalftoning )
	goto HalftoningDone;

/* Send the definition of the spot function */
if ( Printer.DotShape == IDC_ELLIPSEDOT )
	{
//	PS( "/myspotfunc { 2 mul abs exch dup mul add 3 div 2 mul 1 exch sub" );
//	PS( " } bind def\r\n" );
	PS( "/myspotfunc { abs exch abs 2 copy 3 mul exch 4 mul add 3 sub " );	PS( "dup 0 lt\r\n" );
	PS( "{ pop dup mul exch .75 div dup mul add 4 div 1 exch sub }\r\n" );
	PS( "{ dup 1 gt {pop 1 exch sub dup mul " );	PS( "exch 1 exch sub .75 div dup mul add 4 div 1 sub }\r\n" );	PS( "{ .5 exch sub exch pop exch pop } ifelse } ifelse" );	PS( " } bind def\r\n" );
	}
else
if ( Printer.DotShape == IDC_SQUAREDOT )
	{
	PS( "/myspotfunc { abs exch abs 2 copy gt {pop} {exch pop} " );
	PS( "ifelse 1 exch sub } bind def\r\n" );
	}
else
//if ( Printer.DotShape == IDC_CIRCLEDOT )
	{
//	PS( "/myspotfunc { dup mul exch dup mul add 1 exch sub } bind def\r\n");
	PS( "/myspotfunc { abs exch abs 2 copy add 1 gt\r\n" );
	PS( "{ 1 sub dup mul exch 1 sub dup mul add 1 sub }\r\n" );
	PS( "{ dup mul exch dup mul add 1 exch sub }\r\n" );
	PS( "ifelse } bind def\r\n" );
	}

// Setup the "colorimage" screen angles and frequencies
PS( "colorext {\r\n" );
FixedAscii( C_RULING, lpRuling );
FixedAscii( C_ANGLE, lpAngle );
P2( "%ls %ls /myspotfunc load\r\n", lpRuling, lpAngle );
FixedAscii( M_RULING, lpRuling );
FixedAscii( M_ANGLE, lpAngle );
P2( "%ls %ls /myspotfunc load\r\n", lpRuling, lpAngle );
FixedAscii( Y_RULING, lpRuling );
FixedAscii( Y_ANGLE, lpAngle );
P2( "%ls %ls /myspotfunc load\r\n", lpRuling, lpAngle );
FixedAscii( K_RULING, lpRuling );
FixedAscii( K_ANGLE, lpAngle );
P2( "%ls %ls /myspotfunc load\r\n", lpRuling, lpAngle );
PS( "setcolorscreen } if\r\n" );

// Setup the "image" screen angles and frequencies based on the sep being sent
if ( cSep == 'C' )
	{
	FixedAscii( C_ANGLE, lpAngle );
	FixedAscii( C_RULING, lpRuling );
	}
else
if ( cSep == 'M' )
	{
	FixedAscii( M_ANGLE, lpAngle );
	FixedAscii( M_RULING, lpRuling );
	}
else
if ( cSep == 'Y' )
	{
	FixedAscii( Y_ANGLE, lpAngle );
	FixedAscii( Y_RULING, lpRuling );
	}
else
//if ( cSep == 'K' || cSep == 'X' || !cSep )
	{
	FixedAscii( K_ANGLE, lpAngle );
	FixedAscii( K_RULING, lpRuling );
	}
P2( "%ls %ls /myspotfunc load setscreen\r\n", lpRuling, lpAngle );

HalftoningDone:

// Setup a null transfer curve unless doing seps w/black ink (image operator)
if ( Page.Type == IDC_PRINT_BLACKSEPS )
	{ PS( "/doinvert true def\r\n" ); }
else
if ( Page.Type == IDC_PRINT_COLORSEPS )
	{ PS( "/doinvert colorext not def\r\n" ); }
else	{ PS( "/doinvert false def\r\n" ); }
PS( "doinvert { {1 exch sub} settransfer }{ {} settransfer } ifelse\r\n" );

/* Send the destination point (x,y) in spots */
P2( "%d %d translate\r\n", xDest,  yDest );
/* Send the destination size (w,h) in spots */
P2( "%d %d scale\r\n", dxDest,  dyDest );

/* Compute how many pixels we're going to send */
/* Never send more than 16 pixels per halftone grid (or 4/grid in x and y) */
iGridSize = (1414L * iPrResX) / (1000L * HIWORD(K_RULING));
dxDest = (4L * dxDest) / iGridSize;
iGridSize = (1414L * iPrResY) / (1000L * HIWORD(K_RULING));
dyDest = (4L * dyDest) / iGridSize;

/* Let the printer do any upsizing */
if ( dySrc < dyDest )
	{
	yrate = UNITY;
	dxDest = dxSrc;
	dyDest = dySrc;
	}
else	yrate = FGET( dySrc, dyDest );

/* Send the definition for the line buffers */
P1( "/pps1 %d string def\r\n", dxDest );
P1( "/pps2 %d string def\r\n", dxDest );
P1( "/pps3 %d string def\r\n", dxDest );
P1( "/pps4 %d string def\r\n", dxDest );
P1( "/pps5 %d string def\r\n", dxDest );
if ( cSep ) // Plane at a time
	{ // cSep is either 'C', 'M', 'Y', 'K', 'X'(gray) or NULL
	if ( Page.Type == IDC_PRINT_COLORSEPS )
		{ PS( "/docolorsep colorext def\r\n" ); }
	else	{ PS( "/docolorsep false def\r\n" ); }
	PS( "/doimage\r\n" );
	PS( "docolorsep\r\n" );
	PS( "{{ %% Start color image proc\r\n" );
	if ( cSep == 'C' )
		{
		PS( "{pps1 readdata}\r\n" );
		PS( "{pps2}\r\n" );
		PS( "{pps2}\r\n" );
		PS( "{pps2}\r\n" );
		}
	else
	if ( cSep == 'M' )
		{
		PS( "{pps2}\r\n" );
		PS( "{pps1 readdata}\r\n" );
		PS( "{pps2}\r\n" );
		PS( "{pps2}\r\n" );
		}
	else
	if ( cSep == 'Y' )
		{
		PS( "{pps2}\r\n" );
		PS( "{pps2}\r\n" );
		PS( "{pps1 readdata}\r\n" );
		PS( "{pps2}\r\n" );
		}
	else
	if ( cSep == 'K' || cSep == 'X' )
		{
		PS( "{pps2}\r\n" );
		PS( "{pps2}\r\n" );
		PS( "{pps2}\r\n" );
		PS( "{pps1 readdata}\r\n" );
		}
	PS( "true 4 colorimage }}\r\n" );
	PS( "{{ %% Start gray image proc\r\n" );
	PS( "{pps1 readdata}\r\n" );
	PS( "image }}\r\n" );
	PS( "ifelse def\r\n" );
	}
else
if ( Printer.Type == IDC_PRINTER_IS_CMYK )
	{
	PS( "/doimage\r\n" );
	PS( "colorext\r\n" );
	PS( "{{ %% Start color image proc\r\n" );
	PS( "{pps1 readdata}\r\n" );
	PS( "{pps2 readdata}\r\n" );
	PS( "{pps3 readdata}\r\n" );
	if ( Page.Type == IDC_PRINT_COLORGRAY )
		{ PS( "{pps4 readdata  pps5 readdata pop}\r\n" ); }
	else	{ PS( "{pps4 readdata}\r\n" ); }
	PS( "true 4 colorimage }}\r\n" );
	PS( "{{ %% Start gray image proc\r\n" );
	PS( "{pps1 readdata pop\r\n" );
	PS( "pps2 readdata pop\r\n" );
	PS( "pps3 readdata pop\r\n" );
	if ( Page.Type == IDC_PRINT_COLORGRAY )
		{ PS( "pps4 readdata pop pps5 readdata } image }}\r\n" ); }
	else	{ PS( "pps4 readdata } noimage }}\r\n" ); }
	PS( "ifelse def\r\n" );
	}
else
if ( Printer.Type == IDC_PRINTER_IS_RGB )
	{
	PS( "/doimage\r\n" );
	PS( "colorext\r\n" );
	PS( "{{ %% Start color image proc\r\n" );
	PS( "{pps1 readdata}\r\n" );
	PS( "{pps2 readdata}\r\n" );
	if ( Page.Type == IDC_PRINT_COLORGRAY )
		{ PS( "{pps3 readdata  pps4 readdata pop}\r\n" ); }
	else	{ PS( "{pps3 readdata}\r\n" ); }
	PS( "true 3 colorimage }}\r\n" );
	PS( "{{ %% Start gray image proc\r\n" );
	PS( "{pps1 readdata pop\r\n" );
	PS( "pps2 readdata pop\r\n" );
	if ( Page.Type == IDC_PRINT_COLORGRAY )
		{ PS( "pps3 readdata pop pps4 readdata } image }}\r\n" ); }
	else	{ PS( "pps3 readdata } noimage }}\r\n" ); }
	PS( "ifelse def\r\n" );
	}

/* Send the inline image's size, packing, and transform */
P5( "%d %d %d [%d 0 0 %d 0 0] doimage\r\n",
	dxDest, dyDest, 8, dxDest, dyDest );

for ( x=0; x<5; x++ )
	lpBuffer[x] = NULL;
if (!AllocLines((LPPTR)&lpBuffer[0], 1, dxDest, DEPTH))
	return( FALSE );
if (!AllocLines((LPPTR)&lpBuffer[1], 4, dxDest, 1))
	{
	FreeUp(lpBuffer[0]);
	return( FALSE );
	}

lpRGB = (LPRGB)lpBuffer[0];
p1Buf = lpBuffer[1];
p2Buf = lpBuffer[2];
p3Buf = lpBuffer[3];
p4Buf = lpBuffer[4];

ystart = ySrc;
yline = -1;
yoffset = yrate>>1;
for (y=0; y<dyDest; y++)
	{
	AstralClockCursor( y, dyDest );

	/* Check for user input to abort dialog box */
	(*lpAbortTest)((HDC)hPrinterDC, 0);
	if ( fAbortPrint )
		break;

	ylast = yline;
	yline = ystart + HIWORD( yoffset );
	yoffset += yrate;
	if ( yline != ylast )
		{
		if ( !readimage( yline, xSrc, xSrc+dxSrc-1,
		     dxDest, lpBuffer[0] ) )
			goto ErrorExit; /* Error reading from image file */
		// Do a convert data here someday when we need 16 bit cache
		}

	if ( cSep )
		{ // cSep is either 'C', 'M', 'Y', 'K', 'X'(gray) or NULL
		if ( cSep == 'C' )
			{
			RGBtoCMYK( lpRGB, p1Buf, p2Buf, p3Buf, p4Buf, dxDest );
			if (Negative)
				negate(p1Buf, (long)dxDest);
			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
			}
		else
		if ( cSep == 'M' )
			{
			RGBtoCMYK( lpRGB, p1Buf, p2Buf, p3Buf, p4Buf, dxDest );
			if (Negative)
				negate(p2Buf, (long)dxDest);
			if ( !SendPSData( Asciize, p2Buf, dxDest ) )
				goto ErrorExit;
			}
		else
		if ( cSep == 'Y' )
			{
			RGBtoCMYK( lpRGB, p1Buf, p2Buf, p3Buf, p4Buf, dxDest );
			if (Negative)
				negate(p3Buf, (long)dxDest);
			if ( !SendPSData( Asciize, p3Buf, dxDest ) )
				goto ErrorExit;
			}
		else
		if ( cSep == 'K' )
			{
			RGBtoCMYK( lpRGB, p1Buf, p2Buf, p3Buf, p4Buf, dxDest );
			if (Negative)
				negate(p4Buf, (long)dxDest);
			if ( !SendPSData( Asciize, p4Buf, dxDest ) )
				goto ErrorExit;
			}
		else
		if ( cSep == 'X' )
			{
			ConvertData( lpBuffer[0], DEPTH, dxDest, p1Buf, 1 );
			if (Negative)
				negate(p1Buf, (long)dxDest);
			if ( Printer.DoCalibration )
				map( Printer.CalMap[0].Lut, p1Buf, dxDest );
			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
			}
		}
	else
	if ( Printer.Type == IDC_PRINTER_IS_CMYK )
		{
		RGBtoCMYK( lpRGB, p1Buf, p2Buf, p3Buf, p4Buf, dxDest );
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
		if ( Page.Type == IDC_PRINT_COLORGRAY )
			{
			ConvertData( lpBuffer[0], DEPTH, dxDest, p1Buf, 1 );
			if (Negative)
				negate(p1Buf, (long)dxDest);
			if ( Printer.DoCalibration )
				map( Printer.CalMap[0].Lut, p1Buf, dxDest );
			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
			}
		}
	else
	if ( Printer.Type == IDC_PRINTER_IS_RGB )
		{
		if (Negative)
			negate( (LPTR)lpRGB, (long)dxDest * DEPTH);
		UnshuffleRGB( lpRGB, p1Buf, p2Buf, p3Buf, dxDest );
		if ( !SendPSData( Asciize, p1Buf, dxDest ) )
			goto ErrorExit;
		if ( !SendPSData( Asciize, p2Buf, dxDest ) )
			goto ErrorExit;
		if ( !SendPSData( Asciize, p3Buf, dxDest ) )
			goto ErrorExit;
		if ( Page.Type == IDC_PRINT_COLORGRAY )
			{
			ConvertData( lpBuffer[0], DEPTH, dxDest, p1Buf, 1 );
//			if (Negative)
//				negate(p1Buf, (long)dxDest);
			if ( Printer.DoCalibration )
				map( Printer.CalMap[0].Lut, p1Buf, dxDest );
			if ( !SendPSData( Asciize, p1Buf, dxDest ) )
				goto ErrorExit;
			}
		}
	}

/* Send the save restore command */
PS("\r\nmysave restore\r\n" );
PS( "end\r\n" );
goto OKexit;

ErrorExit:
ret = FALSE;

OKexit:
if ( lpBuffer[0] )
	FreeUp(lpBuffer[0]);
if ( lpBuffer[1] )
	FreeUp(lpBuffer[1]);

return( ret );
}

/************************************************************************/
BOOL SendPSData( Asciize, lpBuffer, dxDest )
/************************************************************************/
BOOL Asciize;
LPTR lpBuffer;
int dxDest;
{
typedef struct _laser
	{
	int	nCount;
	char	cData[2*MAX_IMAGE_LINE];
	} LASER;
typedef LASER far *LPLASER;
LPTR lpBits;
LPLASER	lpLaser;

lpLaser = (LPLASER)LineBuffer[0];
lpBits = lpLaser->cData;

if ( Asciize )
	lpLaser->nCount = asciize( lpBuffer, dxDest, lpBits );
else	{
	copy( lpBuffer, lpBits, dxDest );
	lpLaser->nCount = dxDest;
	}

/* Send raw data to the PostScript device */
if ( Escape( hPrinterDC, DEVICEDATA, lpLaser->nCount, (LPTR)lpLaser, 0 ) < 0 )
	return( FALSE );
return( TRUE );
}
