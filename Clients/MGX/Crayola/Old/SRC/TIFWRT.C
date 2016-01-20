// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "tiffio.h"

static void SwapTag(TAG* pTag);

/************************************************************************/

int tifwrt(
LPSTR 		lpFileName,
LPOBJECT 	lpObject,
LPFRAME 	lpFrame,
LPRECT 		lpRect,
int 		flag,
BOOL 		fCompressed)
{
	int iRet;
	int ofp;
	BOOL bEscapable;

	bEscapable = !FileExists(lpFileName);
	if ((ofp = _lcreat(lpFileName,0)) < 0)
	{
		Message( IDS_EWRITE, lpFileName );
		return(-1);
	}
	iRet = TiffWriteFrame(
		ofp, lpFileName, lpObject, lpFrame,
		lpRect, flag, fCompressed, bEscapable);

	_lclose(ofp);
	return(iRet);
}

/************************************************************************/

int TiffWriteFrame(
	int      oFile,
	LPSTR     lpFileName,
	LPOBJECT lpObject,
	LPFRAME  lpFrame,
	LPRECT   lpRect,
	int      flag,
	BOOL     fCompressed,
	BOOL     bEscapable)
{
	TAG    tag;
	int    y, bpl, npix, nlin, ofp;
	LPLONG lngptr,boffptr;
	LPWORD shtptr;
	DWORD  byteoffset;
	WORD   i, numtags, photomet, samples;
	BYTE   bpp;
	LPWORD lpRed, lpGreen, lpBlue;
	RGBS   RGBmap[256];
	LPTR   lp, lpBuffer, lpOutputPointer, lpImgScanline;
	FNAME  temp;
	RECT   rSave;
	BOOL   compressInit;
#ifdef STATIC16 // only in new framelib
	CFrameTypeConvert FrameTypeConvert;
	FRMTYPEINFO SrcTypeInfo;	
	FRMTYPEINFO DstTypeInfo;	
#endif

	lpBuffer      = NULL;
	lpImgScanline = NULL;

	if (!lpFrame)
		return( -1 );

	ProgressBegin(1,0);

	if ((ofp = oFile) < 0)
		bEscapable = !FileExists(lpFileName);

	if ((ofp = oFile) < 0 && (ofp = _lcreat(lpFileName,0)) < 0)
	{
		Message( IDS_EWRITE, lpFileName );
		goto Exit;
	}

	if (lpRect)
		rSave = *lpRect;
	else
	{
		rSave.top    = rSave.left = 0;
		rSave.bottom = FrameYSize(lpFrame)-1;
		rSave.right  = FrameXSize(lpFrame)-1;
	}

	npix = RectWidth(&rSave);
	nlin = RectHeight(&rSave);

	switch(flag)
	{
		case IDC_SAVELA :
		case IDC_SAVESP :
			bpp      = 1;
			bpl      = ((npix + 7) / 8);
			numtags  = 11;
			photomet = 1;
			samples  = 1;
		break;

		case IDC_SAVECT :
			bpp      = 8;
			bpl      = npix;
			numtags  = 11;
			photomet = 1;
			samples  = 1;
		break;

		case IDC_SAVE4BITCOLOR :
		case IDC_SAVE8BITCOLOR :
			bpp      = 8;
			bpl      = npix;
			numtags  = 12;
			photomet = 3;
			samples  = 1;
		break;

		case IDC_SAVE24BITCOLOR :
			bpp      = 24;
			bpl      = npix * 3;
			numtags  = 11;
			photomet = 2;
			samples  = 3;
		break;

		case IDC_SAVE32BITCOLOR :
			bpp      = 32;
			bpl      = npix * 4;
			numtags  = 11;
			photomet = 5;
			samples  = 4;
		break;

		default :
			goto Exit;
		break;
	}

	compressInit = NO;

	if ( bpp == 1 )
	{
		AllocLines( &lpBuffer,      1, npix, 2 );
		AllocLines( &lpImgScanline, 1, npix, 1 );
	}
	else
	{
		AllocLines( &lpBuffer,      1, max(bpl, FrameByteWidth(lpFrame)), 1 );
		AllocLines( &lpImgScanline, 1, max(bpl, FrameByteWidth(lpFrame)), 1 );
	}

	if ( !lpBuffer || !lpImgScanline )
	{
		Message( IDS_EMEMALLOC );
		_lclose( ofp );
		goto Exit;
	}

	/* first location where any extra data can be stored */
	/* 10 byte header + all tag data (12 bytes each) + 4 bytes (null ifd) */
	byteoffset = 10 + (numtags * sizeof(TAG)) + 4;

	shtptr = (LPWORD)LineBuffer[0];
	SetNextWord(&shtptr, 0x4949);   /* byte order is LSB,MSB */
	SetNextWord(&shtptr, 0x2a);     /* tiff version number */
	SetNextWord(&shtptr, 8);        /* byte offset to first image file directory LSW */
	SetNextWord(&shtptr, 0);        /* byte offset to first image file directory MSW */
	SetNextWord(&shtptr, numtags);  /* number of entries in IFD */

	tag.tagno  = 0xff;    /* tag 0xff, subfile type */
	tag.type   = 3;       /* field type is short */
	tag.length = 1;       /* number of values */
	tag.value  = 1;       /* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x100;   /* tag 0x100, number of pixels */
	tag.type   = 3;       /* field type is short */
	tag.length = 1;       /* number of values */
	tag.value  = npix;    /* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x101;   /* tag 0x101, number of lines */
	tag.type   = 3;       /* field type is short */
	tag.length = 1;       /* number of values */
	tag.value  = nlin;    /* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x102;   /* tag 0x102, bits per sample */
	tag.type   = 3;       /* field type is short */
	tag.length = samples; /* number of values */

	if ( samples == 3 || samples == 4)
	{
		tag.value = byteoffset;	/* deferred value */
		byteoffset += (samples*sizeof(short));
	}
	else
		tag.value = bpp;  /* value */

#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x103;   /* tag 0x103, compression */
	tag.type   = 3;       /* field type is short */
	tag.length = 1;       /* number of values */
	tag.value  = (fCompressed ? 5:1); /* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x106;	  /* tag 0x106,photometric inter.(0 = black) */
	tag.type   = 3;	      /* field type is short */
	tag.length = 1;	      /* number of values */
	tag.value  = photomet;	/* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x111;   /* tag 0x111, strip byte offsets */
	tag.type   = 4;       /* field type is long */
	tag.length = 1;       /* number of values */
	tag.value  = 0;       /* dummy location of the start of image data */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	boffptr = (LPLONG)(shtptr+4);  // make boffptr point at tag.value
	shtptr += 6;

	tag.tagno  = 0x115;   /* tag 0x115, samples per pixel*/
	tag.type   = 3;       /* field type is short */
	tag.length = 1;       /* number of values */
	tag.value  = samples; /* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x11a;   /* tag 0x11a, xresolution */
	tag.type   = 5;       /* field type is rational */
	tag.length = 1;       /* number of values */
	tag.value = byteoffset;	/* deferered value */
	byteoffset += 8;
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x11b;   /* tag 0x11b, yresolution */
	tag.type   = 5;       /* field type is rational */
	tag.length = 1;       /* number of values */
	tag.value  = byteoffset; /* deferred value */
	byteoffset += 8;
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	tag.tagno  = 0x11c;   /* tag 0x11c, planar configuration */
	tag.type   = 3;       /* field type is short */
	tag.length = 1;       /* number of values */
	tag.value  = 1;       /* value */
#ifdef _MAC
	SwapTag(&tag);
#endif	
	lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
	shtptr += 6;

	if ( photomet == 3 ) // Palette color map
	{
		tag.tagno  = 0x140;      /* tag 0x140, colormap */
		tag.type   = 3;          /* field type is short */
		tag.length = 3*256;      /* number of values */
		tag.value  = byteoffset; /* value */
		byteoffset += (2*3*256);
#ifdef _MAC
	SwapTag(&tag);
#endif	
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;
	}

	// Copy the NULL Image File Directory pointer
	SetNextWord(&shtptr, 0); /* pointer to next IFD */
	SetNextWord(&shtptr, 0);

	// Copy out the Bits Per Sample, if multiple samples
	if ( samples == 3 )  // The bits per pixel per sample
	{
		SetNextWord(&shtptr, 8);
		SetNextWord(&shtptr, 8);
		SetNextWord(&shtptr, 8);
	}

	// Copy out the Bits Per Sample, if multiple samples
	if ( samples == 4 )  // The bits per pixel per sample
	{
		SetNextWord(&shtptr, 8);
		SetNextWord(&shtptr, 8);
		SetNextWord(&shtptr, 8);
		SetNextWord(&shtptr, 8);
	}

	// Copy out the X and Y resolution fields
	lngptr = (LPLONG)shtptr;

#ifdef PPVIDEO
	SetNextLong(&lngptr, FrameResolution(lpFrame) * 2); /* xreso numerator */
	SetNextLong(&lngptr, 2);							/* xreso denominator */
	SetNextLong(&lngptr, FrameResolution(lpFrame) * 2); /* yreso numerator */
	SetNextLong(&lngptr, 2);							/* yreso denominator */
#else
	SetNextLong(&lngptr, FrameResolution(lpFrame));		/* xreso numerator */
	SetNextLong(&lngptr, 1);							/* xreso denominator */
	SetNextLong(&lngptr, FrameResolution(lpFrame));		/* yreso numerator */
	SetNextLong(&lngptr, 1);							/* yreso denominator */
#endif

	*boffptr = byteoffset;
#ifdef _MAC
	swapl((LPDWORD)boffptr);
#endif	

	// Write out the tags, the bpp, and the resolutions
	i = (LPTR)lngptr - (LPTR)LineBuffer[0];
	if ( _lwrite(ofp, LineBuffer[0], i) != i )
		goto BadWrite;

	// Write the color palette, if necessary
	if ( photomet == 3 ) // Palette color map
	{
		if (!OptimizeBegin(lpObject, lpFrame, RGBmap, 256, 
			NULL /*(LPROC)AstralClockCursor*/,  // No Progress report
			NO, Convert.fOptimize, Convert.fScatter, Convert.fDither, npix))
			goto BadWrite;

		lpRed   = (LPWORD)LineBuffer[0];
		lpGreen = lpRed   + 256;
		lpBlue  = lpGreen + 256;
		for ( i=0; i<256; i++ )
		{
			*lpRed++   = (WORD)RGBmap[i].red   << 8;
			*lpGreen++ = (WORD)RGBmap[i].green << 8;
			*lpBlue++  = (WORD)RGBmap[i].blue  << 8;
		}
		if ( _lwrite(ofp, LineBuffer[0], 2*3*256) != 2*3*256 )
			goto BadWrite;
	}

	if ( fCompressed )
	{
		if ( CompressLZW( ofp, NULL, 0 ) < 0 ) /* Initialize */
			goto BadWrite;
		compressInit = YES;
	}

	switch(bpp)
	{
		case 1 :
			for( y=rSave.top; y<=rSave.bottom; y++ )
			{
				if (AstralClockCursor( y-rSave.top, nlin, bEscapable ))
					goto Cancelled;

				if ( lpObject )
				{
					if (!ImgGetLine( NULL, lpObject, rSave.left, y,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;
					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
						goto BadRead;
				}

				if (FrameDepth(lpFrame) == 0)
				{
					if (flag == IDC_SAVESP)
						diffuse( 0, i, 0, NULL, lp, npix, lpBuffer );
					else
						con2la( lp, npix, lpBuffer );
				}
				else
				{
					ConvertData( lp, FrameDepth(lpFrame), npix, lpBuffer+npix, 1 );
					if ( flag == IDC_SAVESP )
						diffuse( 0, i, 0, NULL, lpBuffer+npix, npix, lpBuffer );
					else
						con2la( lpBuffer+npix, npix, lpBuffer );
				}

				if ( fCompressed )
				{
					if ( CompressLZW( ofp, lpBuffer, bpl ) < 0 )
						goto BadWrite;
				}
				else
				{
					if ( _lwrite( ofp, (LPSTR)lpBuffer, bpl ) != bpl )
						goto BadWrite;
				}
			}
		break;

		case 8 :
			for( y=rSave.top; y<=rSave.bottom; y++ )
			{
				if (AstralClockCursor( y-rSave.top, nlin, bEscapable ))
					goto Cancelled;

				if ( lpObject )
				{
					if (!ImgGetLine( NULL, lpObject, rSave.left, y,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;
					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
						goto BadRead;
				}

				if (FrameDepth(lpFrame) == 0)
				{
					if ( photomet == 3 ) // If we are storing palette color
						OptimizeData(0, y, npix, lp, lpBuffer, 1 );
					else
						ConvertData( lp, 1, npix, lpBuffer, 1 );
				}
				else
				{
					if ( photomet == 3 ) // If we are storing palette color
						OptimizeData(0, y, npix, lp, lpBuffer, FrameDepth(lpFrame));
					else
						ConvertData( lp, FrameDepth(lpFrame), npix, lpBuffer, 1 );
				}
					
				if ( fCompressed )
				{
					if ( CompressLZW( ofp, lpBuffer, bpl ) < 0 )
						goto BadWrite;
				}
				else
				{
					if ( _lwrite( ofp, (LPSTR)lpBuffer, bpl ) != bpl )
						goto BadWrite;
				}
			}
		break;

		case 24 :
			for( y=rSave.top; y<=rSave.bottom; y++ )
			{
				if (AstralClockCursor( y-rSave.top, nlin, bEscapable ))
					goto Cancelled;

				if ( lpObject )
				{
					if (!ImgGetLine( NULL, lpObject, rSave.left, y,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;
					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
						goto BadRead;
				}

				if (FrameType(lpFrame) != FDT_RGBCOLOR)
				{
					if (FrameType(lpFrame) != FDT_LINEART)
					{
#ifdef STATIC16
						SrcTypeInfo.DataType = FrameType(lpFrame);
						SrcTypeInfo.ColorMap = NULL;
						SrcTypeInfo.DataType = FDT_RGBCOLOR;
						SrcTypeInfo.ColorMap = NULL;

						FrameTypeConvert.Init(SrcTypeInfo, DstTypeInfo, npix);
						FrameTypeConvert.ConvertData((LPTR)lp, (LPTR)lpBuffer, y, npix);
#else					
						FrameTypeConvert(
							(LPTR)lp, FrameType(lpFrame), NULL,
							y,
							(LPTR)lpBuffer, FDT_RGBCOLOR, NULL,
							npix);
#endif							
					}
					else
					{
#ifdef STATIC16
						SrcTypeInfo.DataType = FDT_GRAYSCALE;
						SrcTypeInfo.ColorMap = NULL;
						SrcTypeInfo.DataType = FDT_RGBCOLOR;
						SrcTypeInfo.ColorMap = NULL;

						FrameTypeConvert.Init(SrcTypeInfo, DstTypeInfo, npix);
						FrameTypeConvert.ConvertData((LPTR)lp, (LPTR)lpBuffer, y, npix);
#else					
						FrameTypeConvert(
							(LPTR)lp, FDT_GRAYSCALE, NULL,
							y,
							(LPTR)lpBuffer, FDT_RGBCOLOR, NULL,
							npix);
#endif							
					}

					lpOutputPointer = lpBuffer;
				}
				else
				{
					lpOutputPointer = lp;
				}

				if ( fCompressed )
				{
					if ( CompressLZW( ofp, lpOutputPointer, bpl ) < 0 )
						goto BadWrite;
				}
				else
				{
					if ( _lwrite( ofp, (LPSTR)lpOutputPointer, bpl ) != bpl )
						goto BadWrite;
				}
			}
		break;

		case 32 :
			for( y=rSave.top; y<=rSave.bottom; y++ )
			{
				if (AstralClockCursor( y-rSave.top, nlin, bEscapable ))
					goto Cancelled;

				if ( lpObject )
				{
					if (!ImgGetLine( NULL, lpObject, rSave.left, y,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;
					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
						goto BadRead;
				}

				if (FrameType(lpFrame) != FDT_CMYKCOLOR)
				{
					if (FrameType(lpFrame) != FDT_LINEART)
					{
#ifdef STATIC16
						SrcTypeInfo.DataType = FrameType(lpFrame);
						SrcTypeInfo.ColorMap = NULL;
						SrcTypeInfo.DataType = FDT_CMYKCOLOR;
						SrcTypeInfo.ColorMap = NULL;

						FrameTypeConvert.Init(SrcTypeInfo, DstTypeInfo, npix);
						FrameTypeConvert.ConvertData((LPTR)lp, (LPTR)lpBuffer, y, npix);
#else					
						FrameTypeConvert(
							(LPTR)lp, FrameType(lpFrame), NULL,
							y,
							(LPTR)lpBuffer, FDT_CMYKCOLOR, NULL,
							npix);
#endif							
					}
					else
					{
#ifdef STATIC16
						SrcTypeInfo.DataType = FDT_GRAYSCALE;
						SrcTypeInfo.ColorMap = NULL;
						SrcTypeInfo.DataType = FDT_CMYKCOLOR;
						SrcTypeInfo.ColorMap = NULL;

						FrameTypeConvert.Init(SrcTypeInfo, DstTypeInfo, npix);
						FrameTypeConvert.ConvertData((LPTR)lp, (LPTR)lpBuffer, y, npix);
#else					
						FrameTypeConvert(
							(LPTR)lp, FDT_GRAYSCALE, NULL,
							y,
							(LPTR)lpBuffer, FDT_CMYKCOLOR, NULL,
							npix);
#endif							
					}

					lpOutputPointer = lpBuffer;
				}
				else
				{
					lpOutputPointer = lp;
				}

				if ( fCompressed )
				{
					if ( CompressLZW( ofp, lpOutputPointer, bpl ) < 0 )
						goto BadWrite;
				}
				else
				{
					if ( _lwrite( ofp, (LPSTR)lpOutputPointer, bpl ) != bpl )
						goto BadWrite;
				}
			}
		break;
	}

	if ( compressInit )
		if ( CompressLZW( ofp, NULL, 0 ) < 0 ) /* Terminate */
			goto BadWrite;

	compressInit = NO;
	OptimizeEnd();

	if (ofp != oFile)
		_lclose(ofp);

	if (lpBuffer)
		FreeUp( lpBuffer );

	if (lpImgScanline)
		FreeUp( lpImgScanline );

	ProgressEnd();

	return( 0 );

BadWrite:
	Message( IDS_EWRITE, lpFileName );
	goto BadTiff;

BadRead:
	Message( IDS_EREAD, (LPTR)Control.RamDisk );

Cancelled:
BadTiff:
	if ( compressInit )
		if ( CompressLZW( ofp, NULL, 0 ) < 0 ) /* Terminate */
			goto BadWrite;

	compressInit = NO;

	OptimizeEnd();

	if (ofp != oFile)
		_lclose(ofp);

	lstrcpy(temp,lpFileName);
	FileDelete(temp);

Exit:

	if (lpBuffer)
		FreeUp( lpBuffer );

	if (lpImgScanline)
		FreeUp( lpImgScanline );

	ProgressEnd();

	return( -1 );
}


static void SwapTag(TAG* pTag)
{
	swapw((LPWORD)&pTag->tagno);
	swapw((LPWORD)&pTag->type);
	swapl((LPDWORD)&pTag->length);
	swapl((LPDWORD)&pTag->value);
}

