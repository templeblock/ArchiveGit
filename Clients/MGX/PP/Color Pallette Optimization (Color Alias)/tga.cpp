
// types
typedef unsigned char *PTR;
typedef RGBS *PRGB;
typedef struct _rgb
	{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;
typedef RGBS FAR *PRGB;

// local functions
static int CountRunPixels24(PRGB lp, int iWidth);
static int CountRawPixels24(PRGB lp, int iWidth);


// code snippet for setting data type field in header
		switch (bitCount)
		{
			case 1:
			case 8:
				/* gray */
				PicHdr.dataType = (fCompressed) ? 9 : 1;
			break;

			case 24:
			case 32:
				/* color */
				PicHdr.dataType = (fCompressed) ? 10 : 2;
				}
			break;
		}

//
// Function for compressing data for TGA format
// This code only supports 24-bit data
//
int CompressTarga(	PTR lpSrc,		/* source data buffer */
					PTR lpDst,		/* dest data buffer, should be bigger than lpSrc */
									/* just in case */
					int npix,		/* number of pixels */
					int bitCount)	/* bit depth - 8,24, etc */
{
	PTR lpStart;
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
				nCount = CountRawPixels24((PRGB)lpSrc, npix);
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
					RGBS color = *((PRGB)lpSrc);
					nCount = CountRunPixels24((PRGB)lpSrc, npix);
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

static int CountRawPixels24(PRGB lp, int iWidth)
{
	int nRaw = 0;
	// count number of pixels until we find a run
	--iWidth;
	PRGB lpN = lp + 1;
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

static int CountRunPixels24(PRGB lp, int iWidth)
{
	// start with one because a run always includes at least 2 pixels
	int nRun = 1;
	// count number of pixels until run stops
	--iWidth;
	PRGB lpN = lp + 1;
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

