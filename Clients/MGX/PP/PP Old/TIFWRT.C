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
tifwrt(ifile, lpFrame, lpRect, flag, fCompressed)
/************************************************************************/
LPTR ifile;
LPFRAME lpFrame;
LPRECT lpRect;
int flag;
BOOL fCompressed;
{
TAG tag;
int ofp, bpl, npix, nlin;
LPLONG lngptr,boffptr;
LPWORD shtptr;
DWORD byteoffset;
WORD i, numtags, photomet, samples, iCount;
BYTE bpp;
LPWORD lpRed, lpGreen, lpBlue;
RGBS RGBmap[256], rgb;
WORD wMini;
long color;
LPTR lp, lpBuffer, lpLine;
FNAME temp;
LPTR lpColorLut, lpOut;
LPFRAME lpOldFrame;
RECT rSave;

if (!lpFrame)
    return;

lpOldFrame = NULL;
if (lpRect)
	rSave = *lpRect;
else
	{
	rSave.top = rSave.left = 0;
	rSave.bottom = lpFrame->Ysize-1;
	rSave.right = lpFrame->Xsize-1;
	}
npix = RectWidth(&rSave);
nlin = RectHeight(&rSave);
if((ofp = _lcreat(ifile,0)) < 0)
	{
	Message( IDS_EOPEN, ifile );
	return(-1);
	}

if ( flag == IDC_SAVECT )
	{
	bpp = 8;
	bpl = npix;
	numtags = 11;
	photomet = 1;
	samples = 1;
	}
else
if ( flag == IDC_SAVE24BITCOLOR )
	{
	bpp = 24;
	bpl = npix * 3;
	numtags = 11;
	photomet = 2;
	samples = 3;
	}
else
if ( flag == IDC_SAVE8BITCOLOR )
	{
	bpp = 8;
	bpl = npix;
	numtags = 12;
	photomet = 3;
	samples = 1;
	}
else
if ( flag == IDC_SAVELA || flag == IDC_SAVESP )
	{
	bpp = 1;
	bpl = ((npix + 7) / 8);
	numtags = 11;
	photomet = 1;
	samples = 1;
	}

#ifdef OLD
if ( !AllocLines( &lpBuffer, 1, bpl, 1 ) )
	{
	Message( IDS_EMEMALLOC );
	_lclose( ofp );
	return( -1 );
	}
#else
if ( bpp == 1 )
	AllocLines( &lpBuffer, 1, npix, 2 );
else	AllocLines( &lpBuffer, 1, bpl, 1 );
if ( !lpBuffer )
	{
	Message( IDS_EMEMALLOC );
	_lclose( ofp );
	return( -1 );
	}
#endif

/* first location where any extra data can be stored */
/* 10 byte header + all tag data (12 bytes each) + 4 bytes (null ifd) */
byteoffset = 10 + (numtags * sizeof(TAG)) + 4;

shtptr = (LPWORD)LineBuffer[0];
*shtptr++ = 0x4949;	/* byte order is LSB,MSB */
*shtptr++ = 0x2a;	/* tiff version number */
*shtptr++ = 8;		/* byte offset to first image file directory LSW */
*shtptr++ = 0;		/*	"	"	"	"	"    MSW */
*shtptr++ = numtags;	/* number of entries in IFD */

tag.tagno = 0xff;	/* tag 0xff, subfile type */
tag.type = 3;		/* field type is short */
tag.length = 1;		/* number of values */
tag.value = 1;		/* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x100;	/* tag 0x100, number of pixels */
tag.type = 3;	/* field type is short */
tag.length = 1;	/* number of values */
tag.value = npix; /* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x101;	/* tag 0x101, number of lines */
tag.type = 3;	/* field type is short */
tag.length = 1;	/* number of values */
tag.value = nlin; /* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x102;	/* tag 0x102, bits per sample */
tag.type = 3;	/* field type is short */
tag.length = samples; /* number of values */
if ( samples == 3 )
	{
	tag.value = byteoffset;	/* deferred value */
	byteoffset += (samples*sizeof(short));
	}
else	tag.value = bpp; /* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x103;	/* tag 0x103, compression */
tag.type = 3;	/* field type is short */
tag.length = 1;	/* number of values */
tag.value = (fCompressed ? 5:1); /* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x106;	/* tag 0x106,photometric inter.(0 = black) */
tag.type = 3;	/* field type is short */
tag.length = 1;	/* number of values */
tag.value = photomet;	/* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x111;	/* tag 0x111, strip byte offsets */
tag.type = 4;	/* field type is long */
tag.length = 1;	/* number of values */
tag.value = 0;	/* dummy location of the start of image data */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
boffptr = (LPLONG)(shtptr+4);  // make boffptr point at tag.value
shtptr += 6;

tag.tagno = 0x115;	/* tag 0x115, samples per pixel*/
tag.type = 3;	/* field type is short */
tag.length = 1;	/* number of values */
tag.value = samples;	/* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x11a;	/* tag 0x11a, xresolution */
tag.type = 5;	/* field type is rational */
tag.length = 1;	/* number of values */
tag.value = byteoffset;	/* deferered value */
byteoffset += 8;
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x11b;	/* tag 0x11b, yresolution */
tag.type = 5;	/* field type is rational */
tag.length = 1;	/* number of values */
tag.value = byteoffset;	/* deferred value */
byteoffset += 8;
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

tag.tagno = 0x11c;	/* tag 0x11c, planar configuration */
tag.type = 3;	/* field type is short */
tag.length = 1;	/* number of values */
tag.value = 1;		/* value */
lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
shtptr += 6;

if ( photomet == 3 ) // Palette color map
	{
	tag.tagno = 0x140;	/* tag 0x140, colormap */
	tag.type = 3;	/* field type is short */
	tag.length = 3*256;	/* number of values */
	tag.value = byteoffset;	/* value */
	byteoffset += (2*3*256);
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;
	}

// Copy the NULL Image File Directory pointer
*shtptr++ = 0;		/* pointer to next IFD */
*shtptr++ = 0;

// Copy out the Bits Per Sample, if multiple samples
if ( samples == 3 )  // The bits per pixel per sample
	{
	*shtptr++ = 8;
	*shtptr++ = 8;
	*shtptr++ = 8;
	}

// Copy out the X and Y resolution fields
lngptr = (LPLONG)shtptr;
#ifdef PPVIDEO
*lngptr++ = lpFrame->Resolution * 2; /* xreso numerator */
*lngptr++ = 2;		/* xreso denominator */
*lngptr++ = lpFrame->Resolution * 2; /* yreso numerator */
*lngptr++ = 2;		/* yreso denominator */
#else
*lngptr++ = lpFrame->Resolution;	/* xreso numerator */
*lngptr++ = 1;		/* xreso denominator */
*lngptr++ = lpFrame->Resolution;	/* yreso numerator */
*lngptr++ = 1;		/* yreso denominator */
#endif

*boffptr = byteoffset;
// Write out the tags, the bpp, and the resolutions
i = (LPTR)lngptr - (LPTR)LineBuffer[0];
if ( _lwrite(ofp, LineBuffer[0], i) != i )
	goto BadWrite;

lpOldFrame = frame_set(lpFrame);

// Write the color palette, if necessary
if ( photomet == 3 ) // Palette color map
	{
	if ( !(lpColorLut = CreateOptimizedPalette(RGBmap)) )
		goto BadWrite;

	lpRed = (LPWORD)LineBuffer[0];
	lpGreen = lpRed + 256;
	lpBlue = lpGreen + 256;
	for ( i=0; i<256; i++ )
		{
		*lpRed++   = RGBmap[i].red << 8;
		*lpGreen++ = RGBmap[i].green << 8;
		*lpBlue++  = RGBmap[i].blue << 8;
		}
	if ( _lwrite(ofp, LineBuffer[0], 2*3*256) != 2*3*256 )
		goto BadWrite;
	}

if ( fCompressed )
	if ( CompressLZW( ofp, NULL, 0 ) < 0 ) /* Initialize */
		goto BadWrite;

lpOldFrame = frame_set(lpFrame);
if ( bpp == 8 )
	{
	for(i=rSave.top; i<=rSave.bottom; i++)
		{
		AstralClockCursor( i, nlin );
		if ( !(lp = frame_ptr( 0, rSave.left, i, NO )) )
			goto BadRead;
		if ( photomet == 3 ) // If we are storing palette color
			{
			lpLine = lp;
			lpOut = lpBuffer;
			iCount = npix;
			while ( --iCount >= 0 )
				{
        			frame_getRGB( lpLine, &rgb );
	    			wMini = RGBtoMiniRGB( &rgb );
				*lpOut++ = *( lpColorLut + wMini );
				lpLine += DEPTH;
			        }
			}
		else	{
			ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 1);
			}
		if ( fCompressed )
			{
			if ( CompressLZW( ofp, lpBuffer, bpl ) < 0 )
				goto BadWrite;
			}
		else	{
			if ( _lwrite( ofp, lpBuffer, bpl ) != bpl )
				goto BadWrite;
			}
		}
	}
else
if ( bpp == 24 )
	{
	for(i=rSave.top; i<=rSave.bottom; i++)
		{
		AstralClockCursor( i, nlin );
		if ( !(lp = frame_ptr( 0, rSave.left, i, NO )) )
			goto BadRead;
		ConvertData (lp, lpFrame->Depth, npix, lpBuffer, 3);
		if ( fCompressed )
			{
			if ( CompressLZW( ofp, lpBuffer, bpl ) < 0 )
				goto BadWrite;
			}
		else	{
			if ( _lwrite( ofp, lpBuffer, bpl ) != bpl )
				goto BadWrite;
			}
		}
	}
else
if ( bpp == 1 )
	{
	for(i=rSave.top; i<=rSave.bottom; i++)
		{
		AstralClockCursor( i, nlin );
		if ( !(lp = frame_ptr( 0, rSave.left, i, NO )) )
			goto BadRead;
		ConvertData (lp, lpFrame->Depth, npix, lpBuffer+npix, 1);
		if ( flag == IDC_SAVESP )
			diffuse( 0, i, 0, NULL, lpBuffer+npix, npix, lpBuffer );
		else	{
			con2la( lpBuffer+npix, npix, lpBuffer );
			}
		if ( fCompressed )
			{
			if ( CompressLZW( ofp, lpBuffer, bpl ) < 0 )
				goto BadWrite;
			}
		else	{
			if ( _lwrite( ofp, lpBuffer, bpl ) != bpl )
				goto BadWrite;
			}
		}
	}

if ( fCompressed )
	if ( CompressLZW( ofp, NULL, 0 ) < 0 ) /* Terminate */
		goto BadWrite;
_lclose(ofp);
FreeUp( lpBuffer );
if (lpOldFrame)
    frame_set(lpOldFrame);
return(0);

BadWrite:
Message( IDS_EWRITE, ifile );
goto BadTiff;

BadRead:
Message( IDS_EREAD, (LPTR)Control.RamDisk );

BadTiff:
_lclose(ofp);
FreeUp( lpBuffer );
lstrcpy(temp,ifile);
unlink(temp);
if (lpOldFrame)
    frame_set(lpOldFrame);
return(-1);
}
