// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#include "compress.h"
#include "imgutil.h"
#include "getline.h"
#include "cpo.h"
#include "cmsutil.h"
#include "mask.h"

#define MAX_TAGS_SIZE 2048L

static void WriteHorzDiff	( LPTR lpDst, LPTR lpSrc, int iCount, int SamplesPerPixel );
LOCAL void MergeMaskData(LPMASK lpMask, int x, int y, int npix,
					int outDepth, LPTR lpSrc, LPTR lpDst);
LOCAL void Convert8BitTo4Bit(LPTR lpInBuf, LPTR lpOutBuf, int iCount);

/************************************************************************/

BOOL CWriteBitmap::TiffWrite()
{
	TAG         tag;
	int         y, npix, nlin, outDepth, samples, iCount;
	UINT		in_bpl, out_bpl;
	LPWORD      boffptr;
	LPWORD      shtptr;
	DWORD       byteoffset;
	LONG		lCrc = 0;
	WORD        i, numtags, photomet;
	BYTE        bpp;
	LPWORD      lpRed, lpGreen, lpBlue;
	LPTR        lp, lpBuffer, lpImgScanline, lpTags;
	RECT        rSave;
	BOOL        compressInit, fCompressed, TIFFDiff;
	LPLZW_STUFF lpLZW;
	FRMTYPEINFO inType, outType;
	LPOBJECT 	lpObject;
	LPFRAME 	lpFrame = NULL;
	CFrameTypeConvert TypeConvert;
	BOOL 		fConvert = FALSE, fRet = FALSE;
	LPMASK		lpMask = NULL;
	CFile 		theFile;
	CFile*		pTheFile = NULL;
	LONG		cms_size, size;
	char		cms_name[256];
	LPTR		cms_pData = NULL;
	BOOL		fIn16Color;
	int			iColors;
	BOOL		fOptimize;

	LPTIFFOPTIONS lpOptions = (LPTIFFOPTIONS)m_lpOptions;

	ASSERT(lpOptions);
	if (!lpOptions)
		return(FALSE);

	if (m_lpObjList)
	{
		lpObject = m_lpObjList->lpHead;
		if (lpObject)
		{
			lpFrame = ObjGetEditFrame(lpObject);
			if (lpObject->lpAlpha && lpOptions->bSaveMask)
				lpMask = lpObject->lpAlpha;
			// if only one object don't do combines
			if (!lpObject->lpNext)
				lpObject = NULL;
		}
	}
	else
	{
		lpFrame = m_lpFrame;
		lpObject = NULL;
	}

	ASSERT(lpFrame);

	inType = FrameTypeInfo(lpFrame);
	// cause FramePointer never returns line art
	if (inType.DataType == FDT_LINEART)
		inType.DataType = FDT_GRAYSCALE;
	// make sure colormap is inited for outType so we don't free bogus ptr
	FrameSetTypeInfo(&outType, FDT_NONE);
	fIn16Color =(inType.DataType == FDT_PALETTECOLOR) &&
			 	(inType.ColorMap->NumEntries <= 16);

	fCompressed = lpOptions->TIFFCompress;
	TIFFDiff = lpOptions->TIFFDiff;

	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}
		   
	lpBuffer      = NULL;
	lpImgScanline = NULL;
	lpTags = NULL;
	compressInit = NO;

	if (m_fSrcArea)
		rSave = m_rSrcArea;
	else
	{
		rSave.top    = rSave.left = 0;
		rSave.bottom = FrameYSize(lpFrame)-1;
		rSave.right  = FrameXSize(lpFrame)-1;
	}

	npix = RectWidth(&rSave);
	nlin = RectHeight(&rSave);

	switch(m_DataType)
	{
		case FDT_LINEART :
			TIFFDiff = FALSE;
			bpp      = 1;
			out_bpl      = ((npix + 7) / 8);
			numtags  = 12;
			photomet = 1;
			samples  = 1;
			FrameSetTypeInfo(&outType, FDT_LINEART);
			// can't save a mask with line art
			lpMask = NULL;
			outDepth = 0;
		break;

		case FDT_GRAYSCALE :
			bpp      = 8;
			out_bpl      = npix;
			numtags  = 12;
			photomet = 1;
			samples  = 1;
			FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
			outDepth = 1;
		break;

		case FDT_PALETTECOLOR :
			if (m_BitsPerPixel == 4 || fIn16Color)
			{
				bpp      = 4;
				out_bpl      = (npix+1)/2;
				numtags  = 13;
				photomet = 3;
				samples  = 1;
				if (inType.DataType == FDT_PALETTECOLOR)
					outType = inType;
				else
					FrameSetTypeInfo(&outType, FDT_PALETTECOLOR, m_lpColorMap);
				outDepth = 1;
				lpMask = NULL;
				fOptimize = FALSE;
			}
			else
			{ 
				bpp      = 8;
				out_bpl      = npix;
				numtags  = 13;
				photomet = 3;
				samples  = 1;
				if (inType.DataType == FDT_PALETTECOLOR)
					outType = inType;
				else
					FrameSetTypeInfo(&outType, FDT_PALETTECOLOR, m_lpColorMap);
				outDepth = 1;
				fOptimize = TRUE;
			}
			if (bpp == 4)
				iColors = 16;
			else
				iColors = 256;

		break;

		case FDT_RGBCOLOR :
			bpp      = 24;
			out_bpl      = (UINT)npix * 3;
			numtags  = 12;
			photomet = 2;
			samples  = 3;

			FrameSetTypeInfo(&outType, FDT_RGBCOLOR);

			if( IsDstPTSelected( &inType.ptInfo ) && lpOptions->bWritePT )
				{
				if( inType.DataType != FDT_RGBCOLOR )
					{
					SetError(BEC_errPTMismatch);
					//Message( IDS_FILEPTMISMATCH );
					bpp      = 32;
					out_bpl      = (UINT)npix * 4;
					photomet = 5;
					samples  = 4;
					FrameSetTypeInfo(&outType, FDT_CMYKCOLOR);
					}
				numtags += 2;
				lCrc = inType.ptInfo.toRCS;
				}
			outDepth = 3;
		break;

		case FDT_CMYKCOLOR :
			bpp      = 32;
			out_bpl      = (UINT)npix * 4;
			numtags  = 12;
			photomet = 5;
			samples  = 4;

			FrameSetTypeInfo(&outType, FDT_CMYKCOLOR);

			if( IsDstPTSelected( &inType.ptInfo ) && lpOptions->bWritePT )
				{
				if( inType.DataType != FDT_CMYKCOLOR )
					{
					SetError(BEC_errPTMismatch);
					//Message( IDS_FILEPTMISMATCH );
					bpp      = 24;
					out_bpl      = (UINT)npix * 3;
					photomet = 2;
					samples  = 3;
					FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
					}
				numtags += 2;
				lCrc = inType.ptInfo.toRCS;
				}
			outDepth = 4;
		break;

		default :
			goto Exit;
		break;
	}

	if (lpMask)
	{
		out_bpl += (UINT)npix;
		++samples;
		++numtags;
	}

	if ( FrameType(lpFrame) == FDT_LINEART )
		in_bpl = npix;
	else
		in_bpl = (UINT)npix * FrameDepth(lpFrame);

	lpBuffer = Alloc(max(in_bpl, out_bpl));
	lpImgScanline = Alloc(max(in_bpl, out_bpl));

	lpTags = Alloc(MAX_TAGS_SIZE);

	if ( !lpBuffer || !lpImgScanline || !lpTags)
	{
		SetError(BEC_errMemory);
		//Message( IDS_EMEMALLOC );
		goto Exit;
	}

	TRY
	{
		/* first location where any extra data can be stored */
		/* 10 byte header + all tag data (12 bytes each) + 4 bytes (null ifd) */
		byteoffset = 10 + (numtags * sizeof(TAG)) + 4;

		shtptr = (LPWORD)lpTags;
		*shtptr++ = 0x4949;   /* byte order is LSB,MSB */
		*shtptr++ = 0x2a;     /* tiff version number */
		*shtptr++ = 8;        /* byte offset to first image file directory LSW */
		*shtptr++ = 0;        /* byte offset to first image file directory MSW */
		*shtptr++ = numtags;  /* number of entries in IFD */

		tag.tagno  = 0xff;    /* tag 0xff, subfile type */
		tag.type   = 3;       /* field type is short */
		tag.length = 1;       /* number of values */
		tag.value  = 1;       /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x100;   /* tag 0x100, number of pixels */
		tag.type   = 4;       /* field type is long */
		tag.length = 1;       /* number of values */
		tag.value  = npix;    /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x101;   /* tag 0x101, number of lines */
		tag.type   = 4;       /* field type is long */
		tag.length = 1;       /* number of values */
		tag.value  = nlin;    /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x102;   /* tag 0x102, bits per sample */
		tag.type   = 3;       /* field type is short */
		tag.length = samples; /* number of values */
		if (samples == 1)
			tag.value = bpp;  /* value */
		else
		if (samples == 2)
			tag.value = MAKELONG(bpp, bpp);
		else
		{
			tag.value = byteoffset;	/* deferred value */
			byteoffset += (samples*sizeof(short));
		}
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x103;   /* tag 0x103, compression */
		tag.type   = 3;       /* field type is short */
		tag.length = 1;       /* number of values */
		tag.value  = (fCompressed ? 5:1); /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;
	
		tag.tagno  = 0x106;	  /* tag 0x106,photometric inter.(0 = black) */
		tag.type   = 3;	      /* field type is short */
		tag.length = 1;	      /* number of values */
		tag.value  = photomet;	/* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x111;   /* tag 0x111, strip byte offsets */
		tag.type   = 4;       /* field type is long */
		tag.length = 1;       /* number of values */
		tag.value  = 0;       /* dummy location of the start of image data */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		boffptr = (LPWORD)(shtptr+4);  // make boffptr point at tag.value
		shtptr += 6;

		tag.tagno  = 0x115;   /* tag 0x115, samples per pixel*/
		tag.type   = 3;       /* field type is short */
		tag.length = 1;       /* number of values */
		tag.value  = samples; /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x11a;   /* tag 0x11a, xresolution */
		tag.type   = 5;       /* field type is rational */
		tag.length = 1;       /* number of values */
		tag.value = byteoffset;	/* deferered value */
		byteoffset += 8;
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x11b;   /* tag 0x11b, yresolution */
		tag.type   = 5;       /* field type is rational */
		tag.length = 1;       /* number of values */
		tag.value  = byteoffset; /* deferred value */
		byteoffset += 8;
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x11c;   /* tag 0x11c, planar configuration */
		tag.type   = 3;       /* field type is short */
		tag.length = 1;       /* number of values */
		tag.value  = 1;       /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		tag.tagno  = 0x13d;   /* tag 0x13d, use differencing */
		tag.type   = 3;       /* field type is short */
		tag.length = 1;       /* number of values */
		tag.value  = ((fCompressed && bpp >= 8 && TIFFDiff) ? 2:1); /* value */
		lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
		shtptr += 6;

		if ( outType.DataType == FDT_PALETTECOLOR ) // Palette color map
		{
			tag.tagno  = 0x140;      /* tag 0x140, colormap */
			tag.type   = 3;          /* field type is short */
			tag.length = 3*iColors;      /* number of values */
			tag.value  = byteoffset; /* value */
			byteoffset += (2*3*iColors);
			lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
			shtptr += 6;
		}

		if ( lpMask ) // are we writing out the mask channel
		{
			tag.tagno  = 0x152;      /* tag 0x152 Extra Samples */
			tag.type   = 3;          /* field type is short */
			tag.length = 1;      /* number of values */
			tag.value  = 2; /* value */
			lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
			shtptr += 6;
		}

		// write out PT info tags
		if( lCrc )
			{
			cms_size = GetXformDataSize( lCrc, cms_name );
			if( !cms_size )
				{
				SetError(BEC_errGettingPT);
				//Message( IDS_EGETTINGPT );
				goto Exit;
				}
			tag.tagno  = 34391; 	 		/* tag 0x34391, CMS Precision Transform Data. */
			tag.type   = 1;      	 	/* field type is byte */
			tag.length = cms_size;       	/* number of values */
			tag.value  = byteoffset; 	/* value */
			byteoffset += cms_size;
			lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
			shtptr += 6;

			size = lstrlen( cms_name ) + 1;

			tag.tagno  = 34392; 	 		/* tag 0x34392, CMS Precision Transform Name. */
			tag.type   = 2;      	 	/* field type is byte */
			tag.length = size;			/* number of values */
			tag.value  = byteoffset; 	/* value */
			byteoffset += size;
			lmemcpy((LPTR)shtptr,(LPTR)&tag.tagno,12);
			shtptr += 6;
			}

		// Copy the NULL Image File Directory pointer
		*shtptr++ = 0; /* pointer to next IFD */
		*shtptr++ = 0;

		// Copy out the Bits Per Sample, if multiple samples
		if (samples > 2)
		{
			iCount = samples;
			while (--iCount >= 0)
				*shtptr++ = 8;
		}

		// Copy out the X and Y resolution fields
		*shtptr++ = LOWORD(FrameResolution(lpFrame));	/* xreso numerator */
		*shtptr++ = HIWORD(FrameResolution(lpFrame));	/* xreso numerator */
		*shtptr++ = LOWORD(1);		/* xreso denominator */
		*shtptr++ = HIWORD(1);		/* xreso denominator */
		*shtptr++ = LOWORD(FrameResolution(lpFrame));	/* yreso numerator */
		*shtptr++ = HIWORD(FrameResolution(lpFrame));	/* yreso numerator */
		*shtptr++ = LOWORD(1);		/* yreso denominator */
		*shtptr++ = HIWORD(1);		/* yreso denominator */

		// set the location of the image data start
		*boffptr++ = LOWORD(byteoffset);
		*boffptr = HIWORD(byteoffset);

		// Write out the tags, the bpp, and the resolutions
		i = (LPTR)shtptr - (LPTR)lpTags;
		pTheFile->Write(lpTags, i);

		// Write the color palette, if necessary
		if ( outType.DataType == FDT_PALETTECOLOR )
		{
			LPRGB lpRGBmap;

			// see if we need to create a palette
			if (!outType.ColorMap)
				{
				outType.ColorMap = FrameCreateColorMap();
				if (!outType.ColorMap)
					{
					SetError(BEC_errMemory);
					//Message(IDS_EMEMALLOC);
					goto Exit;
					}
				outType.ColorMap->NumEntries = iColors;
				if (!CreateOptimizedPalette(lpObject, lpFrame,
									outType.ColorMap->RGBData,
									&outType.ColorMap->NumEntries,
									fOptimize,
									NULL))
					{
					SetError(BEC_errMemory);
					//Message(IDS_EMEMALLOC);
					goto Exit;
					}
				}
			lpRGBmap = outType.ColorMap->RGBData;

			lpRed   = (LPWORD)lpTags;
			lpGreen = lpRed   + iColors;
			lpBlue  = lpGreen + iColors;
			for ( i=0; i<iColors; i++ )
			{
				*lpRed++   = (WORD)lpRGBmap[i].red   << 8;
				*lpGreen++ = (WORD)lpRGBmap[i].green << 8;
				*lpBlue++  = (WORD)lpRGBmap[i].blue  << 8;
			}
			pTheFile->Write(lpTags, 2*3*iColors);
		}

		// write out the PT info
		if ( lCrc )
		{
			if( !( cms_pData = ( LPTR )Alloc( cms_size ) ) )
				{
				SetError(BEC_errMemory);
				//Message( IDS_EMEMALLOC );
				goto Exit;
				}
			if( !GetXformData( lCrc, cms_pData ) )
				{
				SetError(BEC_errGettingPT);
				//Message( IDS_EGETTINGPT );
				goto Exit;
				}
			pTheFile->WriteHuge( cms_pData, cms_size ); 

			size = lstrlen( cms_name ) + 1;
			pTheFile->WriteHuge( cms_name, size ); 
		}

		if ( fCompressed )
		{
			if ( !( lpLZW = CompressLZW( pTheFile, NULL, 0, NULL ) ) ) /* Initialize */
				goto BadWrite;
			compressInit = YES;
		}

	   fConvert = !( inType.DataType == outType.DataType ||
			FrameColorMapsEqual( inType.ColorMap, outType.ColorMap ) );

		// initialize stuff to do the type conversion
		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto Exit;
		}
		for( y=rSave.top; y<=rSave.bottom; y++ )
		{
			if (Progress( y-rSave.top, nlin, m_bEscapable ))
				goto Cancelled;

			if ( lpObject )
			{
				if (!ObjGetLine( lpObject, rSave.left, y,
					(rSave.right - rSave.left) + 1, lpImgScanline))
					goto BadRead;
				lp = lpImgScanline;
			}
			else
			{
				if ( !(lp = FramePointer( lpFrame, rSave.left, y, NO )) )
					goto BadRead;
			}
			// convert the data to the new type
			TypeConvert.ConvertData(lp, lpBuffer, y, npix);
			if (bpp == 4)
			{
				Convert8BitTo4Bit(lpBuffer, lpImgScanline, npix);
				lp = lpImgScanline;
			}
			else
			if (lpMask)
			{
				MergeMaskData(lpMask, rSave.left, y, npix, outDepth, lpBuffer, lpImgScanline);
				lp = lpImgScanline;
			}
			else
				lp = lpBuffer;

			if ( fCompressed )
			{
				if (TIFFDiff)
					WriteHorzDiff(lp, lp, (int)out_bpl, samples);
				if ( !( CompressLZW( pTheFile, lp, (int)out_bpl, lpLZW ) ) )
					goto BadWrite;
			}
			else
			{
				pTheFile->Write( lp, out_bpl );
			}
		}
		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadWrite;
	}
	END_CATCH_ALL


BadWrite:
    if (m_lpFileName)            
		SetError(BEC_errWrite);
    	//Message( IDS_EWRITE, m_lpFileName );
    else
		SetError(BEC_errWriteFile);
    	//Message (IDS_EWRITEFILE);
	goto Exit;

BadRead:
	SetError(BEC_errFrameRead);
	//Message( IDS_EREAD, (LPTR)Control.RamDisk );

Cancelled:
Exit:
	if ( compressInit )
		if ( !( CompressLZW( pTheFile, NULL, 0, lpLZW ) ) ) /* Terminate */
			goto BadWrite;

	compressInit = NO;

	CloseFile(pTheFile, fRet);

	if (lpBuffer)
		FreeUp( lpBuffer );
	if (lpImgScanline)
		FreeUp( lpImgScanline );
	if (lpTags)
		FreeUp ( lpTags );
	if (cms_pData)
		FreeUp ( cms_pData );
	if (outType.ColorMap && (outType.ColorMap != inType.ColorMap) &&
		(outType.ColorMap != m_lpColorMap))
		FrameDestroyColorMap(outType.ColorMap);
	ProgressEnd();

	return( fRet );
}


/************************************************************************/
LOCAL void MergeMaskData(LPMASK lpMask, int x, int y, int npix,
					int outDepth, LPTR lpSrc, LPTR lpDst)
/************************************************************************/
{
	LPTR lpM;

	MaskLoad(lpMask, x, y, npix, lpMask->mbuffer);
	lpM = lpMask->mbuffer;
	if (outDepth == 1)
	{
		while (--npix >= 0)
		{
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpM++;
		}
	}
	else
	if (outDepth == 3)
	{
		while (--npix >= 0)
		{
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpM++;
		}
	}
	else
	if (outDepth == 4)
	{
		while (--npix >= 0)
		{
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpSrc++;
			*lpDst++ = *lpM++;
		}
	}
}


/************************************************************************/
static void WriteHorzDiff( LPTR lpDst, LPTR lpSrc, int iCount, int SamplesPerPixel )
/************************************************************************/
{
int col, min;

for (col = iCount-1; col >= SamplesPerPixel ; col--)
	lpDst[col] = lpSrc[col] - lpSrc[col-SamplesPerPixel];
min = Min(SamplesPerPixel,iCount);
for (col = 0; col < min; col++)
	lpDst[col] = lpSrc[col];
}


void Convert8BitTo4Bit(
	LPTR lpInBuf, 
	LPTR lpOutBuf, 
	int iCount)
{
	register BYTE p, op;
	BOOL IsOdd = NO;

	while (--iCount >= 0)
   	{
		p = *lpInBuf++;
    	if ( IsOdd )
		{
			op += p;
			*lpOutBuf++ = op;
			IsOdd = NO;
	   	}
    	else
	   	{
			op = p << 4;
			IsOdd = YES;
	   	}
    }

	if ( IsOdd )
    	*lpOutBuf = op;
}


