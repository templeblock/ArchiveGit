//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "stdafx.h"
#include "bitmapio.h"
#include "getline.h"
#include "gif.h"
#include "compress.h"
#include "intelio.h"
#include "cpo.h"

/************************************************************************/

static int gifWriteImDesc( CFile *pFile, GIFDESC *imDesc, GIFMAP *ColorMap,
                              int codeSize )
{
	int  i;
	BYTE cSize;

	TRY
	{
		/* write the color map */
		for (i = 0; i < ColorMap->Length; i++)
		{
			pFile->Write((LPTR)&ColorMap->Map[i].red,   1);
			pFile->Write((LPTR)&ColorMap->Map[i].green, 1);
			pFile->Write((LPTR)&ColorMap->Map[i].blue,  1);
		}

		/* write the image header */
		pFile->Write((LPTR)",", 1);
		pFile->Write((LPTR)&imDesc->ImLeft,   2);
		pFile->Write((LPTR)&imDesc->ImTop,    2);
		pFile->Write((LPTR)&imDesc->ImWidth,  2);
		pFile->Write((LPTR)&imDesc->ImHeight, 2);
		pFile->Write((LPTR)&imDesc->Zero,     1);

		/* write the code size */
		cSize = codeSize;
		pFile->Write((LPTR)&cSize, 1);
		return ( FALSE );
	}
	CATCH_ALL(e)
	{
		return(TRUE);
	}
	END_CATCH_ALL
}

/************************************************************************/

BOOL CWriteBitmap::GIFWrite()
{
	GIFHDR      hdr;
	GIFDESC     imDesc;
	GIFMAP      ColorMap;
	int         y, i, j, npix, nlin;
	LPTR        lp, lpBuffer, lpImgScanline;
	BYTE        cTerm;
	int         codeSize;
	RECT        rSave;
	BOOL        bEscapable;
	BOOL        compressInit;
	LPLZW_STUFF lpLZW;
	LPRGB lpRGBmap;
	LPFRAME lpFrame;
	LPOBJECT lpObject;
	FRMTYPEINFO inType, outType, inTypeLA, outTypeLA;
	CFrameTypeConvert TypeConvert, LineArtConvert;
	BOOL fRet = FALSE;
	CFile 	theFile;
	CFile*	pTheFile = NULL;

	if (m_lpObjList)
	{
		lpObject = m_lpObjList->lpHead;
		if (lpObject)
			lpFrame = ObjGetEditFrame(lpObject);
	}
	else
	{
		lpFrame = m_lpFrame;
		lpObject = NULL;
	}

	ASSERT(lpFrame);
	
	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	TRY
	{

		lpBuffer      = NULL;
		lpImgScanline = NULL;
		compressInit = NO;

		/* full color not supported, force mini color */
		if (m_DataType != FDT_GRAYSCALE && m_DataType != FDT_LINEART)
			m_DataType = FDT_PALETTECOLOR;

		if (m_fSrcArea)
			rSave = m_rSrcArea;
		else
		{
			rSave.top    = rSave.left = 0;
			rSave.bottom = FrameYSize(lpFrame)-1;
			rSave.right  = FrameXSize(lpFrame)-1;
		}

		inType = FrameTypeInfo(lpFrame);
		// cause FramePointer never returns line art
		if (inType.DataType == FDT_LINEART)
			inType.DataType = FDT_GRAYSCALE;
		// make sure colormap is inited for outType so we don't free bogus ptr
		FrameSetTypeInfo(&outType, FDT_NONE);

		npix = RectWidth(&rSave);
		nlin = RectHeight(&rSave);

		/* initialize file header */
		hdr.ScnWidth    = npix;
		hdr.ScnHeight   = nlin;
		hdr.Background  = 0;
		hdr.GlobalMap   = TRUE;
		hdr.ColorRes    = 8;

		/* initialize image descriptor */
		imDesc.ImLeft   = 0;
		imDesc.ImTop    = 0;
		imDesc.ImWidth  = npix;
		imDesc.ImHeight = nlin;
		imDesc.Zero     = 0;

		/* determine type of image to create */
		if ( m_DataType == FDT_GRAYSCALE )
		{
			/* gray */
			hdr.ColorRes = 8;
			hdr.bpp      = 8;
			codeSize     = 8;
			FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
		}
		else
		if ( m_DataType == FDT_LINEART )
		{
			/* line art or scatter */
			hdr.ColorRes = 1;
			hdr.bpp      = 1;
			codeSize     = 2;
			FrameSetTypeInfo(&outType, FDT_LINEART);
		}
		else
		//if ( m_DataType == FDT_PALETTECOLOR )
 		{
			/* mini color */
			hdr.ColorRes = 8;
			hdr.bpp      = 8;
			codeSize     = 8;
			if (inType.DataType == FDT_PALETTECOLOR)
				outType = inType;
			else
				FrameSetTypeInfo(&outType, FDT_PALETTECOLOR);
		}

		AllocLines( &lpBuffer, 1, npix, 4 ); // max size is cool
		AllocLines( &lpImgScanline, 1, npix, 4 );

		if ( !lpBuffer || !lpImgScanline )
		{
			SetError(BEC_errMemory);
			goto Exit;
		}

		/* set raw info byte */
		hdr.RawImInfo = 0x80 | ((hdr.ColorRes-1) << 4) | (hdr.bpp-1);

		/* write file header */
		pTheFile->Write((LPTR)"GIF87a", 6);
		pTheFile->Write((LPTR)&hdr.ScnWidth, 2);
		pTheFile->Write((LPTR)&hdr.ScnHeight, 2);
		pTheFile->Write((LPTR)&hdr.RawImInfo, 1);
		pTheFile->Write((LPTR)&hdr.Background, 1);
		pTheFile->Write((LPTR)"", 1);

		switch (m_DataType)
		{
			case FDT_LINEART:
				/* initialize the color map */
				ColorMap.Length = 2;
				ColorMap.Map[0].red =
				ColorMap.Map[0].green =
				ColorMap.Map[0].blue = 0xFF;
				ColorMap.Map[1].red =
				ColorMap.Map[1].green =
				ColorMap.Map[1].blue = 0x00;
	
				/* write image descriptor, color map and code size */
				if (gifWriteImDesc (pTheFile, &imDesc, &ColorMap, codeSize))
					goto BadWrite;
			break;

			case FDT_GRAYSCALE:
				/* initialize the color map */
				ColorMap.Length = 256;
				for (i = 0; i < 256; i++)
				{
					ColorMap.Map[i].red   =
					ColorMap.Map[i].green =
					ColorMap.Map[i].blue  = i;
				}
	
				/* write image descriptor, color map and code size */
				if (gifWriteImDesc (pTheFile, &imDesc, &ColorMap, codeSize))
					goto BadWrite;
			break;

			case FDT_PALETTECOLOR:
				// see if we need to create a palette
				if (!outType.ColorMap)
					{
					outType.ColorMap = FrameCreateColorMap();
					if (!outType.ColorMap)
						{
						SetError(BEC_errMemory);
						goto Exit;
						}
					outType.ColorMap->NumEntries = 256;
					if (!CreateOptimizedPalette(lpObject, lpFrame,
										outType.ColorMap->RGBData,
										&outType.ColorMap->NumEntries,
										TRUE,
										NULL))
						{
						SetError(BEC_errMemory);
						goto Exit;
						}
					}
				lpRGBmap = outType.ColorMap->RGBData;

				/* initialize the color map */
				ColorMap.Length = 256;
				for (i = 0; i < 256; i++)
				{
					ColorMap.Map[i].red   = lpRGBmap[i].red;
					ColorMap.Map[i].green = lpRGBmap[i].green;
					ColorMap.Map[i].blue  = lpRGBmap[i].blue;
				}
	
				/* write image descriptor, color map and code size */
				if (gifWriteImDesc (pTheFile, &imDesc, &ColorMap, codeSize))
					goto BadWrite;

			break;
		}


		// initialize stuff to do the type conversion
		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
			SetError(BEC_errMemory);
			goto Exit;
		}

		if (outType.DataType == FDT_LINEART)
		{
			FrameSetTypeInfo(&inTypeLA, FDT_LINEART);
			FrameSetTypeInfo(&outTypeLA, FDT_GRAYSCALE);
			if (!LineArtConvert.Init(inTypeLA, outTypeLA, npix))
			{
				SetError(BEC_errMemory);
				goto Exit;
			}
		}
	
		/* write the image */
		if ( !( lpLZW = CompressLZW_GIF (pTheFile, NULL, 0, codeSize, NULL ) ) )
			goto BadWrite;

		compressInit = YES;

		for (y = rSave.top; y <= rSave.bottom; y++)
		{
			if (Progress (y-rSave.top, nlin, bEscapable ))
				goto Exit;

			if (lpObject)
			{
				if (!ObjGetLine( lpObject, rSave.left, y,
								npix, lpImgScanline))
					goto BadRead;
				lp = lpImgScanline;
			}
			else
			{
				if ( !(lp = FramePointer(lpFrame, rSave.left, y, NO)) )
					goto BadRead;
			}

			// convert the data to the new type
			TypeConvert.ConvertData(lp, lpBuffer, y, npix);
	
			if (outType.DataType == FDT_LINEART)
			{
				/* convert back to depth one, but with pixel values of 0 and 1 */
				LineArtConvert.ConvertData(lpBuffer, lpImgScanline, y, npix);
				for (lp = lpImgScanline, j = 0; j < npix; j++, lp++)
					*lp = (*lp) ? 0 : 1;
				lp = lpImgScanline;
			}
			else
				lp = lpBuffer;

			/* compress the data */
			if (!(CompressLZW_GIF (pTheFile, lp, npix, codeSize, lpLZW)))
				goto BadWrite;
		}

	   if ( compressInit )
		   CompressLZW_GIF (pTheFile, NULL, 0, codeSize, lpLZW);
		cTerm = 0;
		pTheFile->Write((LPTR)&cTerm, 1);

		cTerm = GIFTerm;
		pTheFile->Write((LPTR)&cTerm, 1);

		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadWrite;
	}
	END_CATCH_ALL

BadWrite:
	SetError(BEC_errWriteFile);
	goto Exit;

BadRead:
	SetError(BEC_errFrameRead);

Exit:
	/* clean up */
	CloseFile(pTheFile, fRet);
	compressInit = NO;

	if (lpBuffer)
		FreeUp (lpBuffer);
	if (lpImgScanline)
		FreeUp ( lpImgScanline );
	if (outType.ColorMap && (outType.ColorMap != inType.ColorMap))
		FrameDestroyColorMap(outType.ColorMap);

	ProgressEnd();

	return (fRet);
}




