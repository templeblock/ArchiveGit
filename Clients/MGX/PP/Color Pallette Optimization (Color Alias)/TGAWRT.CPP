#include "stdafx.h"
#include "phtypes.h"
#include "targa.h"
#include "dib.h"

/*=========================================================================*/

int CountRawPixels24(LPRGB lp, int iWidth)
{
	int nRaw = 0;
	// count number of pixels until we find a run
	--iWidth;
	LPRGB lpN = lp + 1;
	while (--iWidth >= 0)
	{
		if (lp->red == lpN->red &&
			lp->green == lpN->green &&
			lp->blue == lpN->blue)
		{
			return(nRaw);
		}
		++lp;
		++lpN;
		++nRaw;
	}
	// add one here because last pixel in row cannot make a run
	++nRaw;
	return(nRaw);
}

int CountRunPixels24(LPRGB lp, int iWidth)
{
	// start with one because a run always includes at least 2 pixels
	int nRun = 1;
	// count number of pixels until run stops
	--iWidth;
	LPRGB lpN = lp + 1;
	while (--iWidth >= 0)
	{
		if (lp->red != lpN->red ||
			lp->green != lpN->green ||
			lp->blue != lpN->blue)
		{
			return(nRun);
		}
		++lp;
		++lpN;
		++nRun;
	}
	return(nRun);
}

int CompressTarga(LPTR lpSrc, LPTR lpDst, int npix, int bitCount)
{
	LPTR lpStart;
	int nCount;
	int nPixels, nBytes;

	lpStart = lpDst;
	switch (bitCount)
	{
		case 24:
		{
			// start of with count of 1 pixel
			while (npix)
			{
				nCount = CountRawPixels24((LPRGB)lpSrc, npix);
			   	npix -= nCount;
			   	while (nCount)
			   	{
			   		nPixels = 128;
			   		if (nPixels > nCount)
			   			nPixels = nCount;
			   		*lpDst++ = (nPixels-1);
			   		nBytes = nPixels * 3;
			   		memcpy(lpDst, lpSrc, nBytes);
			   		lpDst += nBytes;
			   		lpSrc += nBytes;
			   		nCount -= nPixels;
			   	}
				// any run left to output
				if (npix)
				{
					RGBS color = *((LPRGB)lpSrc);
					nCount = CountRunPixels24((LPRGB)lpSrc, npix);
					lpSrc += (3 * nCount);
					npix -= nCount;
					while (nCount)
					{
						nPixels = 128;
						if (nPixels > nCount)
							nPixels = nCount;
						*lpDst++ = (BYTE)(nPixels-1) | 0x80;
						*lpDst++ = color.red;
						*lpDst++ = color.green;
						*lpDst++ = color.blue;
						nCount -= nPixels;
					}
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return(lpDst-lpStart);
}

BOOL WriteTarga(PDIB pDib, LPCSTR lpFileName)
{
	TGAHDR  PicHdr;
	int     i, bpl, npix, nlin, bitCount;
	LPTR    lp, lpBuffer;
	LPRGB lpRGBmap;
	RGBS rgb;
	BOOL fCompressed = TRUE;
	BOOL fRet = FALSE;
	CFile	theFile;
	char szMessage[256];

	if (!theFile.Open(lpFileName, CFile::modeCreate|CFile::modeWrite))
	{
		wsprintf(szMessage, "Cannot create file: '%s'", lpFileName);
		MessageBox(NULL, szMessage, "Error", MB_ICONSTOP|MB_OK);
		return(FALSE);
	}

	TRY
	{
		npix = pDib->GetWidth();
		nlin = abs(pDib->GetHeight());
		bitCount = pDib->GetBitCount();

		/* image compression does not currently work */
		//fCompressed = NO;
	
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

		switch (bitCount)
		{
			case 1:
			case 8:
				/* gray */
				PicHdr.mapType = 1;
				PicHdr.mapOrig = 0;
				PicHdr.mapLength = 256;
				PicHdr.CMapBits = 24;
				PicHdr.dataType = (fCompressed) ? 9 : 1;
				PicHdr.dataBits = 8;
				bpl = npix;
			break;

			case 24:
			case 32:
				/* full color */
				PicHdr.mapType = 0;	/* no color map data */
				PicHdr.mapOrig = 0;
				PicHdr.mapLength = 0;
				PicHdr.CMapBits = 0;
				//PicHdr.dataType = 2;
				PicHdr.dataType = (fCompressed) ? 10 : 2;
				if (pDib->GetBitCount() == 32)
				{
					PicHdr.dataBits = 32;
					bpl = npix * 4;
				}
				else
				{
					PicHdr.dataBits = 24;
					bpl = npix * 3;
				}
			break;

			default:
				//Print ("Unknown flag value");
				goto BadWrite;
			break;
		}

		// make buffer double the maximum size
		lpBuffer = (LPTR)malloc((long)npix * 4L * 2L);
		if ( !lpBuffer )
		{
			goto BadMem;
		}

		/* write file header */
		theFile.Write(&PicHdr.textSize, 1);
		theFile.Write(&PicHdr.mapType,  1);
		theFile.Write(&PicHdr.dataType, 1);

		theFile.Write(&PicHdr.mapOrig,   2);
		theFile.Write(&PicHdr.mapLength, 2);
		theFile.Write(&PicHdr.CMapBits,  1);
		theFile.Write(&PicHdr.XOffset,   2);
		theFile.Write(&PicHdr.YOffset,   2);
		theFile.Write(&PicHdr.x,         2);
		theFile.Write(&PicHdr.y,         2);
		theFile.Write(&PicHdr.dataBits,  1);
		theFile.Write(&PicHdr.imType,    1);

		switch (bitCount)
		{
			case 1:
			case 8:
				/* gray */
				/* write the color map */
				for (i = 0; i < 256; i++) 
				{
					rgb.red =
					rgb.green =
					rgb.blue = i;
					theFile.Write((LPTR) &rgb, 3);
				}
				/* write the color map */
				for (i = 0; i < 256; i++)
				{
					theFile.Write((LPTR) &lpRGBmap[i].blue,  1);
					theFile.Write((LPTR) &lpRGBmap[i].green, 1);
					theFile.Write((LPTR) &lpRGBmap[i].red,   1);
				}
			break;

			case 24:
			case 32:
				/* full color */
			break;
		}

		/* write the image */
		for (i = nlin-1; i >= 0; i--) 
		{
			lp = pDib->GetXY(0, i);
			if (fCompressed)
			{
				bpl = CompressTarga(lp, lpBuffer, npix, bitCount);
				lp = lpBuffer;
			}

			theFile.Write(lp, bpl);
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
	wsprintf(szMessage, "Error writing to file: '%s'", lpFileName);
	MessageBox(NULL, szMessage, "Error", MB_ICONSTOP|MB_OK);
	goto Exit;

BadMem:
	MessageBox(NULL, "Out of memory", "Error", MB_ICONSTOP|MB_OK);

Exit:
	/* clean up */
	theFile.Close();

	if (lpBuffer)
		free(lpBuffer);
	return (fRet);
}

/*=========================================================================*/

