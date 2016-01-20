/*=========================================================================*\

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

	®PL1¯®FD1¯®TP0¯®BT0¯

\*=========================================================================*/

/*=========================================================================*\
	tgawrt.c
\*=========================================================================*/

#include "stdafx.h"
#include "bitmapio.h"
#include "targa.h"
#include "getline.h"
#include "cpo.h"
#include "mask.h"

LOCAL void MergeMaskData(LPMASK lpMask, int x, int y, int npix,
					int outDepth, LPTR lpSrc, LPTR lpDst);
/*=========================================================================*/

BOOL CWriteBitmap::TGAWrite()
{
	TGAHDR  PicHdr;
	int     i, bpl, npix, nlin;
	LPTR    lp, lpBuffer, lpImgScanline;
	RECT    rSave;
	BOOL    bEscapable;
	LPRGB lpRGBmap;
	RGBS rgb;
	BOOL fCompressed;
	LPFRAME lpFrame = NULL;
	LPOBJECT lpObject;
	FRMTYPEINFO inType, outType;
	CFrameTypeConvert TypeConvert;
	BOOL fRet = FALSE;
	CFile*	pTheFile = NULL;
	LPMASK lpMask = NULL;

	LPTGAOPTIONS lpOptions = (LPTGAOPTIONS)m_lpOptions;

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
	
 	ProgressBegin(1);
 
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	TRY
	{
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

		/* image compression does not currently work */
		fCompressed = NO;
	
		/* initialize file header */
		PicHdr.textSize = 0;	/* no id field */
		PicHdr.XOffset  = 0;
		PicHdr.YOffset  = 0;
		PicHdr.x        = npix;
		PicHdr.y        = nlin;
		PicHdr.imType   = 0;

		/*************************************/
		/* determine type of image to create */
		/*************************************/

		/* save line art and scatter as gray */

		switch (m_DataType)
		{
			case FDT_LINEART:
			case FDT_GRAYSCALE:
				/* gray */
				PicHdr.mapType = 1;
				PicHdr.mapOrig = 0;
				PicHdr.mapLength = 256;
				PicHdr.CMapBits = 24;
				PicHdr.dataType = (fCompressed) ? 9 : 1;
				PicHdr.dataBits = 8;
				bpl = npix;
				FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
				lpMask = NULL;
			break;

			case FDT_PALETTECOLOR:
				/* mini color */
				PicHdr.mapType = 1;
				PicHdr.mapOrig = 0;
				PicHdr.mapLength = 256;
				PicHdr.CMapBits = 24;
				PicHdr.dataType = (fCompressed) ? 9 : 1;
				PicHdr.dataBits = 8;
				bpl = npix;
				if (inType.DataType == FDT_PALETTECOLOR)
					outType = inType;
				else
					FrameSetTypeInfo(&outType, FDT_PALETTECOLOR);
				lpMask = NULL;
			break;

			case FDT_RGBCOLOR:
			case FDT_CMYKCOLOR:
				/* full color */
				PicHdr.mapType = 0;	/* no color map data */
				PicHdr.mapOrig = 0;
				PicHdr.mapLength = 0;
				PicHdr.CMapBits = 0;
				PicHdr.dataType = 2;
				PicHdr.dataType = (fCompressed) ? 10 : 2;
				if (m_DataType == FDT_CMYKCOLOR)
					lpMask = NULL;
				if (lpMask || (m_DataType == FDT_CMYKCOLOR))
				{
					PicHdr.dataBits = 32;
					bpl = npix * 4;
				}
				else
				{
					PicHdr.dataBits = 24;
					bpl = npix * 3;
				}
				FrameSetTypeInfo(&outType, m_DataType);
			break;

			default:
				//Print ("Unknown flag value");
				goto BadRead;
			break;
		}

		AllocLines (&lpBuffer, 1, npix, 4);
		AllocLines (&lpImgScanline, 1, npix, 4 );

		if ( !lpBuffer || !lpImgScanline )
		{
			SetError (BEC_errMemory);
			goto Exit;
		}

		/* write file header */
		pTheFile->Write(&PicHdr.textSize, 1);
		pTheFile->Write(&PicHdr.mapType,  1);
		pTheFile->Write(&PicHdr.dataType, 1);

		pTheFile->Write(&PicHdr.mapOrig,   2);
		pTheFile->Write(&PicHdr.mapLength, 2);
		pTheFile->Write(&PicHdr.CMapBits,  1);
		pTheFile->Write(&PicHdr.XOffset,   2);
		pTheFile->Write(&PicHdr.YOffset,   2);
		pTheFile->Write(&PicHdr.x,         2);
		pTheFile->Write(&PicHdr.y,         2);
		pTheFile->Write(&PicHdr.dataBits,  1);
		pTheFile->Write(&PicHdr.imType,    1);

		switch (outType.DataType)
		{
			case FDT_LINEART:
			case FDT_GRAYSCALE:
				/* gray */
				/* write the color map */
				for (i = 0; i < 256; i++) 
				{
					rgb.red =
					rgb.green =
					rgb.blue = i;
					pTheFile->Write((LPTR) &rgb, 3);
				}
			break;

			case FDT_PALETTECOLOR:
				/* mini color */
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

				/* write the color map */
				for (i = 0; i < 256; i++)
				{
					pTheFile->Write((LPTR) &lpRGBmap[i].blue,  1);
					pTheFile->Write((LPTR) &lpRGBmap[i].green, 1);
					pTheFile->Write((LPTR) &lpRGBmap[i].red,   1);
				}
			break;

			case FDT_RGBCOLOR:
			case FDT_CMYKCOLOR:
				/* full color */
			break;
		}

		// initialize stuff to do the type conversion
		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
			SetError(BEC_errMemory);
			goto Exit;
		}

		/* write the image */
		for (i = rSave.bottom; i >= rSave.top; i--) 
		{
			if (Progress(rSave.bottom-i, nlin, bEscapable))
				goto Exit;

			if (lpObject)
			{
				if (!ObjGetLine( lpObject, rSave.left, i,
					(rSave.right - rSave.left) + 1, lpImgScanline))
					goto BadRead;
				lp = lpImgScanline;
			}
			else
			{
				if ( !(lp = FramePointer(lpFrame, rSave.left, i, NO)) )
					goto BadRead;
			}

			// convert the data to the new type
			TypeConvert.ConvertData(lp, lpBuffer, i, npix);

			if (outType.DataType == FDT_RGBCOLOR)
			{
				swapBGR( lpBuffer, lpBuffer, npix );
				if (lpMask)
				{
					MergeMaskData(lpMask, rSave.left, i, npix, 3, lpBuffer, lpImgScanline);
					lp = lpImgScanline;
				}
				else
					lp = lpBuffer;
			}
			else
				lp = lpBuffer;
		
	#ifdef UNUSED
			if ( fCompressed ) 
			{
				if (tgaComp8Bit (ofh, lp, npix))
					goto BadWrite;
			}
			else 
	#endif // UNUSED
			{
				pTheFile->Write(lp, bpl);
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
	SetError(BEC_errWriteFile);
	goto Exit;

BadRead:
	SetError(BEC_errFrameRead);

Exit:
	/* clean up */
	CloseFile(pTheFile, fRet);

	if (lpBuffer)
		FreeUp (lpBuffer);
	if (lpImgScanline)
		FreeUp( lpImgScanline );
	if (outType.ColorMap && (outType.ColorMap != inType.ColorMap))
		FrameDestroyColorMap(outType.ColorMap);

	ProgressEnd();

	return (fRet);
}

/*=========================================================================*/

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




