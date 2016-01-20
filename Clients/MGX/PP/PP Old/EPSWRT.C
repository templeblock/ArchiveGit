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

time_t ltime;
static int ofp, WriteError;

/************************************************************************/
epswrt( ifile, lpSomedayFrame, lpSomedayRect, DataType, fCompressed, SplitDir )
/************************************************************************/
LPTR ifile;
LPFRAME lpSomedayFrame;
LPRECT lpSomedayRect;
int DataType;
BOOL fCompressed;
LPTR SplitDir;
{
int j, sfp, DestWidth, DestHeight, x, length, SepFlags;
int xsize, ysize, line, split_on, loop_rw, lrem, lpbuf;
unsigned i;
long endps, endtiff, temp;
BYTE packpix, bpp, r, g, b;
FIXED rate;
FNAME splitfile;
LPWORD shtptr;
LPRGB lpRGB, lpRGBline;
TAG tag;
HSLS hsl;
LPTR lp, lpDir, lpLine;
BOOL Negative, bSinglePlane;

#define C_ANGLE Printer.ScreenAngle[0]
#define M_ANGLE Printer.ScreenAngle[1]
#define Y_ANGLE Printer.ScreenAngle[2]
#define K_ANGLE Printer.ScreenAngle[3]
#define C_RULING Printer.ScreenRuling[0]
#define M_RULING Printer.ScreenRuling[1]
#define Y_RULING Printer.ScreenRuling[2]
#define K_RULING Printer.ScreenRuling[3]

Negative = FALSE;
SepFlags = RGB_SEPS;
bSinglePlane = ( DEPTH == 1 || !SepFlags ||
		(SepFlags & (C_SEP | M_SEP | Y_SEP | K_SEP)) );

WriteError = NO;

if (DEPTH != 1)
	bpp = 8;
else
if ( DataType == IDC_SAVECT )
	bpp = 8;	/* local bits per pixel */
else	bpp = 1;	/* if lineart or scatterprint set */

if (bpp == 1)  /* round to word boundary */
	lpImage->npix = (lpImage->npix >> 4) << 4;

if (lstrlen(SplitDir) != 0)  /* see if user set-up the split dir */
	{
	split_on = TRUE;
	lpDir = ifile;
	lpDir += lstrlen( lpDir );
	while ( *lpDir != '\\' && lpDir > ifile )
		lpDir--;
	lstrcpy(splitfile,SplitDir);
	lstrcat(splitfile,lpDir);
	if ((sfp = _lcreat(splitfile,0)) < 0)
		{
		Message( IDS_EOPEN, (LPTR)splitfile );
		return(-1);
		}
	}
else	split_on = FALSE;

if ((ofp = _lcreat(ifile,0)) < 0)
	{
	Message( IDS_EOPEN, ifile );
	return(-1);
	}
LineBuffer[0][0] = 0xc5;
LineBuffer[0][1] = 0xd0;
LineBuffer[0][2] = 0xd3;
LineBuffer[0][3] = 0xc6;
LineBuffer[0][4] = 0x1e;
set(&LineBuffer[0][5], 23, 0);
LineBuffer[0][28] = 0xff;
LineBuffer[0][29] = 0xff;
if (_lwrite(ofp,LineBuffer[0],30) != 30)
	goto BadWrite;

PSout( "%%!PS-Adobe-2.0 EPSF-1.2\r\n" );
xsize = ((long)lpImage->npix * 72) / lpImage->xres;
ysize = ((long)lpImage->nlin * 72) / lpImage->yres;
PSout( "%%%%BoundingBox: 0 0 %d %d\r\n", xsize, ysize);
PSout( "%%%%Creator: PicturePublisher(TM), Astral Dev.\r\n" );
PSout( "%%%%Title:%ls\r\n", (LPTR)ifile);
time(&ltime);	/* get the current system time */
PSout( "%%%%Creation Date: %ls\r\n", (LPTR)ctime(&ltime));
PSout( "%%%%EndComments\r\n" );
PSout( "40 dict begin\r\n" );
PSout( "gsave\r\n" );
//PSout( "/myspotfunc { dup mul exch dup mul add 1 exch sub } bind def\r\n" );
PSout( "/readdata {currentfile exch readhexstring pop} def\r\n" );
if ( bSinglePlane )
	{
	if (bpp == 1)
		PSout( "/pps %d string def\r\n",((lpImage->npix+7) >> 3));
	else	PSout( "/pps %d string def\r\n",lpImage->npix);
	PSout(  "/doimage {{pps readdata} image} def\r\n" );
	}
else	{
	PSout( "/_currentgray systemdict /currentgray get def\r\n" );
	PSout( "/colorext systemdict /colorimage known def\r\n" );
	PSout( "/foureq {\r\n" );
	PSout( "4 index eq 8 1 roll\r\n" );
	PSout( "4 index eq 8 1 roll\r\n" );
	PSout( "4 index eq 8 1 roll\r\n" );
	PSout( "4 index eq 8 1 roll\r\n" );
	PSout( "pop pop pop pop and and and} def\r\n" );
	PSout( "colorext {/plane 0 def} {/plane 5 def} ifelse\r\n" );
	PSout( "/setcmykcolor where {pop\r\n" );
	PSout( "1 0 0 0 setcmykcolor _currentgray 1 exch sub\r\n" );
	PSout( "0 1 0 0 setcmykcolor _currentgray 1 exch sub\r\n" );
	PSout( "0 0 1 0 setcmykcolor _currentgray 1 exch sub\r\n" );
	PSout( "0 0 0 1 setcmykcolor _currentgray 1 exch sub\r\n" );
	PSout( "4 {4 copy} repeat\r\n" );
	PSout( "1 0 0 0 foureq {/plane 1 store} if\r\n" );
	PSout( "0 1 0 0 foureq {/plane 2 store} if\r\n" );
	PSout( "0 0 1 0 foureq {/plane 3 store} if\r\n" );
	PSout( "0 0 0 1 foureq {/plane 4 store} if\r\n" );
	PSout( "0 0 0 0 foureq {/plane 6 store} if} if\r\n" );
	PSout( "/noimage {pop 1} def\r\n" );
	PSout(  "/pps1 %d string def\r\n", lpImage->npix );
	PSout(  "/pps2 %d string def\r\n", lpImage->npix );
	PSout(  "/pps3 %d string def\r\n", lpImage->npix );
	PSout(  "/pps4 %d string def\r\n", lpImage->npix );
	if (SepFlags & CMYK_SEPS)
		{
		PSout(  "/pps5 %d string def\r\n", lpImage->npix );
		PSout( "/doimage\r\n" );
		PSout( "plane 0 eq\r\n" );
		PSout( "{{{pps1 readdata}\r\n" );
		PSout( "{pps2 readdata}\r\n" );
		PSout( "{pps3 readdata}\r\n" );
		PSout( "{pps4 readdata  pps5 readdata pop}\r\n" );
		PSout( "true 4 colorimage}} if\r\n" );

		PSout( "plane 1 eq\r\n" );
		PSout( "{{{pps1 readdata\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata pop\r\n" );
		PSout( "pps5 readdata pop} image}} if\r\n" );

		PSout( "plane 2 eq\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata pop\r\n" );
		PSout( "pps5 readdata pop} image}} if\r\n" );

		PSout( "plane 3 eq\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata\r\n" );
		PSout( "pps4 readdata pop\r\n" );
		PSout( "pps5 readdata pop} image}} if\r\n" );

		PSout( "plane 4 eq\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata\r\n" );
		PSout( "pps5 readdata pop} image}} if\r\n" );

		PSout( "plane 5 eq\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata pop\r\n" );
		PSout( "pps5 readdata} image}} if\r\n" );

		PSout( "plane 6 eq\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata pop\r\n" );
		PSout( "pps5 readdata} noimage}} if\r\ndef\r\n" );
		}
	else	{
		PSout( "/doimage\r\n" );
		PSout( "plane 0 eq\r\n" );
		PSout( "{{{pps1 readdata}\r\n" );
		PSout( "{pps2 readdata}\r\n" );
		PSout( "{pps3 readdata pps4 readdata pop}\r\n" );
		PSout( "true 3 colorimage}} if\r\n" );

		PSout( "plane 4 eq plane 5 eq or\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata} image}} if\r\n" );

		PSout( "plane 4 eq plane 5 eq plane 0 eq or or not\r\n" );
		PSout( "{{{pps1 readdata pop\r\n" );
		PSout( "pps2 readdata pop\r\n" );
		PSout( "pps3 readdata pop\r\n" );
		PSout( "pps4 readdata} noimage}} if\r\n" );
		}
	PSout( "def\r\n" );
	}

PSout( "%d %d scale\r\n", xsize, ysize );
PSout( "%d %d %d [%d 0 0 -%d 0 %d] doimage\r\n",
	lpImage->npix, lpImage->nlin, bpp, lpImage->npix,
	lpImage->nlin, lpImage->nlin );

if (bpp == 1)
{
/* line buffer is used by lineart eps as follows */
/* grayscale data is read into LineBuffer[0] */
/* this data is either thresholded or diffused into LineBuffer[3] */
/* the resultant bitmap is then asciized back into LineBuffer[0]	*/
/*  ______asciized______________________________			*/
/* 									*/
/* -------------------------------------------------------------------	*/
/* |  grayscale data				| bitmap data  |	*/
/* -------------------------------------------------------------------	*/
/* 									*/
/* LineBuffer[0]				LineBuffer[3]	*/
/*									*/
for (i=0; i<lpImage->nlin; i++)
	{
	AstralClockCursor( i, lpImage->nlin );
	if ( !readimage( i, 0, lpImage->npix-1, lpImage->npix, LineBuffer[0] ) )
		goto BadRead;
	if ( DataType == IDC_SAVESP )
		diffuse( 0, i, 0, NULL, LineBuffer[0], lpImage->npix,
		LineBuffer[3] );
	else	{
		loop_rw = 0;
		packpix = 0;
		lrem = 0;
		for (lpbuf = 0; lpbuf <= lpImage->npix; lpbuf++)
			{
			if (LineBuffer[0][lpbuf] < 32)
				packpix &= 0xfe;
			else	packpix |= 1;
			if (lrem == 8)
				{
				LineBuffer[3][loop_rw] = packpix;
				loop_rw++;
				lrem = 0;
				packpix = 0;
				}
			packpix <<= 1;
			lrem++;
			}
		}
	lrem = lpImage->npix >> 3;  /* divide by 8 for line art */
	/* convert to ascii for postscript */
	length = asciize(LineBuffer[3],lrem,LineBuffer[0]);
	/* write lines */
	if (split_on)
		{
		if (_lwrite(sfp,LineBuffer[0], length ) != length )
			goto BadWriteSplit;
		}
	else
	if (_lwrite(ofp,LineBuffer[0], length ) != length )
		goto BadWrite;
	}
}
else
if (DEPTH == 1)
/* line buffer is used by grayscle eps as follows */
/* grayscale data is read into LineBuffer[0] */
/* this data is modified by applylut back into LineBuffer[0] */
/* the resultant grayscale is then asciized into LineBuffer[2]		*/
/*   ______asciized_____________________________			*/
/*  									*/
/* -------------------------------------------------------------------	*/
/* |  grayscale data				| asciized data |	*/
/* -------------------------------------------------------------------	*/
/* 									*/
/* LineBuffer[0]				LineBuffer[2]	*/
/*									*/
{
for (i=0; i<lpImage->nlin; i++)
	{
	AstralClockCursor( i, lpImage->nlin );
	if ( !readimage( i, 0, lpImage->npix-1, lpImage->npix, LineBuffer[0] ) )
		goto BadRead;

	length = asciize(LineBuffer[0],lpImage->npix,LineBuffer[2]);
	if (split_on)
		{
		if (_lwrite(sfp,LineBuffer[2], length ) != length )
			goto BadWriteSplit;
		}
	else
	if (_lwrite(ofp,LineBuffer[2], length ) != length ) /* write lines */
		goto BadWrite;
	}
}
else
/* line buffer is used by grayscle eps as follows */
/* grayscale data is read into LineBuffer[0] */
/* this data is modified by applylut back into LineBuffer[0] */
/* the resultant grayscale is then asciized into LineBuffer[2]	*/
/*   ______asciized___________________________________	*/
/*  								*/
/* ------------------------------------------------------------------- */
/* |  grayscale data				| asciized data |*/
/* ------------------------------------------------------------------- */
/* 								*/
/* LineBuffer[0]				LineBuffer[2]    */
/*								*/
{
for (i=0; i<lpImage->nlin; i++)
	{
	AstralClockCursor( i, lpImage->nlin );
	if (!(lpRGBline = (LPRGB)frame_ptr(0, 0, i, NO)))
		goto BadRead;
	lpRGB = lpRGBline;
	lp = LineBuffer[0];
	for (x = 0; x < lpImage->npix; x++, lpRGB++)
		*lp++ = lpRGB->red;
	if (Negative)
		negate(LineBuffer[0], (long)lpImage->npix);
	length = asciize(LineBuffer[0],lpImage->npix,LineBuffer[2]);
	if (split_on)
		{
		if (_lwrite(sfp,LineBuffer[2], length ) != length )
			goto BadWriteSplit;
		}
	else
	if (_lwrite(ofp,LineBuffer[2], length ) != length ) /* write lines */
		goto BadWrite;
	lpRGB = lpRGBline;
	lp = LineBuffer[0];
	for (x = 0; x < lpImage->npix; x++, lpRGB++)
		*lp++ = lpRGB->green;
	if (Negative)
		negate(LineBuffer[0], (long)lpImage->npix);
	length = asciize(LineBuffer[0],lpImage->npix,LineBuffer[2]);
	if (split_on)
		{
		if (_lwrite(sfp,LineBuffer[2], length ) != length )
			goto BadWriteSplit;
		}
	else
	if (_lwrite(ofp,LineBuffer[2], length ) != length ) /* write lines */
		goto BadWrite;
	lpRGB = lpRGBline;
	lp = LineBuffer[0];
	for (x = 0; x < lpImage->npix; x++, lpRGB++)
		*lp++ = lpRGB->blue;
	if (Negative)
		negate(LineBuffer[0], (long)lpImage->npix);
	length = asciize(LineBuffer[0],lpImage->npix,LineBuffer[2]);
	if (split_on)
		{
		if (_lwrite(sfp,LineBuffer[2], length ) != length )
			goto BadWriteSplit;
		}
	else
	if (_lwrite(ofp,LineBuffer[2], length ) != length ) /* write lines */
		goto BadWrite;
	lpRGB = lpRGBline;
	lp = LineBuffer[0];
	for (x = 0; x < lpImage->npix; x++, lpRGB++)
		{
		r = lpRGB->red;
		g = lpRGB->green;
		b = lpRGB->blue;
		*lp++ = TOLUM(r,g,b);
		}
	if (Negative)
		negate(LineBuffer[0], (long)lpImage->npix);
	length = asciize(LineBuffer[0],lpImage->npix,LineBuffer[2]);
	if (split_on)
		{
		if (_lwrite(sfp,LineBuffer[2], length ) != length )
			goto BadWriteSplit;
		}
	else
	if (_lwrite(ofp,LineBuffer[2], length ) != length ) /* write lines */
		goto BadWrite;
	}
}

PSout( "grestore\r\n" );
PSout( "end\r\n" );
if (split_on)  /* close the eps image file */
	_lclose(sfp);

endps =_llseek( ofp, 0L, 1 );
if (_llseek(ofp,8L,0) < 0)
	goto BadWrite;
temp = endps - 30;
if (_lwrite(ofp,(LPTR)&temp,4) != 4) /* write length of postscript section */
	goto BadWrite;
if (_llseek(ofp,20L,0) < 0)
	goto BadWrite;
if (_lwrite(ofp,(LPTR)&endps,4) != 4) /* write file position of tiff section */
	goto BadWrite;
if ((temp = _llseek(ofp,endps,0)) < 0) /* seek to the end of the file */
	goto BadWrite;

shtptr = (LPWORD)LineBuffer[0];
*shtptr++ = 0x4949;	/* byte order is LSB,MSB */
*shtptr++ = 0x2a;	/* tiff version number */
*shtptr++ = 8;		/* byte offset to first image file directory LSW */
*shtptr++ = 0;		/*  "   "   "   "	MSW */
*shtptr++ = 10;		/* number of entries in IFD */

tag.tagno = 0xff;	/* tag 0xff, subfile type */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = 1;		/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
DestWidth = ((long)lpImage->npix*72) / lpImage->xres;
DestWidth = (DestWidth >> 3) << 3;
DestHeight = ((long)lpImage->nlin*72) / lpImage->yres;

tag.tagno = 0x100;	/* tag 0x100, number of pixels */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = DestWidth;	/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x101;	/* tag 0x101, number of lines */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = DestHeight;	/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x102;	/* tag 0x102, bits per sample */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = 1 ;		/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x103;	/* tag 0x103, commpression */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = 1 ;		/* value, no compression */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x106;	/* tag 0x106,photometric inter.(0 = black) */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = 1;		/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x111;	/* tag 0x111,strip byte offsets */
tag.type = 4;		/* field type is long */
tag.length = 1;		/* number of values */
tag.value = 148;	/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x115;	/* tag 0x115, samples per pixel*/
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = 1;		/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x11a;	/* tag 0x11a, xresolution */
tag.type = 5;		/* field type is rational */
tag.length = 1;		/* number of values */
tag.value = 132;	/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
tag.tagno = 0x11b;	/* tag 0x11b, yresolution */
tag.type = 5;		/* field type is rational */
tag.length = 1;		/* number of values */
tag.value = 140;	/* value */
lmemcpy( (LPTR)shtptr, (LPTR)&tag.tagno, 12);
shtptr += 6;
*shtptr++ = 0;		/* pointer to next IFD,lsw */
*shtptr = 0;		/*" "   "   "   " ,msw */
*shtptr++ = 72;		/* write the resolution as 72 dpi for the display */
*shtptr++ = 0;
*shtptr++ = 1;
*shtptr++ = 0;
*shtptr++ = 72;
*shtptr++ = 0;
*shtptr++ = 1;
*shtptr++ = 0;
if (_lwrite(ofp,LineBuffer[0],148) < 0)
	goto BadWrite;

rate = FGET( lpImage->nlin, DestHeight );
for (i=0; i<DestHeight; i++)
	{
	AstralClockCursor( i, DestHeight );
	line = FMUL( i, rate );
	if ( !readimage(line,0,lpImage->npix-1,DestWidth,LineBuffer[0]) )
		goto BadRead;
	lp = lpLine = LineBuffer[0];
	for ( j=0; j<DestWidth; j++ )
		{
		frame_getHSL( lpLine, &hsl );
		*lp++ = hsl.lum;
		lpLine += DEPTH;
		}
	diffuse( 0, i, 0, NULL, LineBuffer[0], DestWidth, LineBuffer[1]);
	if (_lwrite(ofp,LineBuffer[1],(DestWidth >> 3)) != (DestWidth >> 3))
		goto BadWrite;
	}
endtiff =_llseek( ofp, 0L, 1 );
if (_llseek(ofp,24L,0) < 0)
	goto BadWrite;
temp = endtiff - endps;
if (_lwrite(ofp,(LPTR)&temp,4) != 4) /* write length of postscript section */
	goto BadWrite;

_lclose(ofp);
return(0);

BadWriteSplit:
Message( IDS_EWRITE, (LPTR)splitfile );
goto BadEps;

BadWrite:
Message( IDS_EWRITE, ifile );
goto BadEps;

BadRead:
Message( IDS_EREAD, (LPTR)Control.RamDisk );

BadEps:
_lclose(ofp);
if (split_on)
	{
	_lclose(sfp);
	unlink(splitfile);
	}
lstrcpy(splitfile,ifile);
unlink(splitfile);
return(-1);
}

static char aschex[16] = 
	{ '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

/************************************************************************/
int asciize(in,count,out)
/************************************************************************/
LPTR in;
LPTR out;
int count;
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


/************************************************************************/
void PSout( lpFormat, ... )
/************************************************************************/
LPTR lpFormat;
{
LPTR lpArguments;
int i;
BYTE szBuffer[MAX_IMAGE_LINE];

lpArguments = (LPTR)&lpFormat + sizeof(lpFormat);
wvsprintf( szBuffer, lpFormat, lpArguments );
i = lstrlen( szBuffer );
if ( _lwrite( ofp, szBuffer, i ) != i )
	WriteError = YES;;
}
