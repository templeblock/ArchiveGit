//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

/* get rid of some of the windows stuff to free heap space */
#define NOMETAFILE
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS

#include "stdafx.h"
#include "mgxbm.h"
#include "intelio.h"

#define BUF_SIZE 16 * 1024

typedef struct _pcxmap {
	BYTE Red;
	BYTE Green;
	BYTE Blue;
} PCXMAP;

typedef struct _pcxhdr {
	BYTE Manufacturer;
	BYTE Version;
	BYTE Encoding;
	BYTE BitsPerPixel;
	WORD Xmin, Ymin, Xmax, Ymax;
	WORD HDpi;
	WORD VDpi;
	BYTE Colormap [48];  /* rgb rgb rgb rgb ... */
	BYTE Reserved;
	BYTE NPlanes;
	WORD BytesPerLine;
	WORD PaletteInfo;
	WORD HscreenSize;
	WORD VscreenSize;
	BYTE Filler [54];
} PCXHDR;

// Static ptototypes
static BITMAP_ERROR_CODE pcxReadHeader( FILEBUF *ifd, PCXHDR FAR *hdr );
static int pcxReadPlane( FILEBUF *ifd, int bpl, LPTR buffer );
static int pcxReadRow( FILEBUF *ifd, PCXHDR far *hdr, LPTR buffer );
static void BuildPalette( int LevelsPerColor, LPRGB RGBmap );

/************************************************************************
 * P-FUNCTION pcxReadHeader
 */

static BITMAP_ERROR_CODE pcxReadHeader (FILEBUF *ifd, PCXHDR FAR *hdr)
{
	FileFDRead (ifd, (LPTR) &hdr->Manufacturer, 1);
	FileFDRead (ifd, (LPTR) &hdr->Version, 1);
	FileFDRead (ifd, (LPTR) &hdr->Encoding, 1);
	FileFDRead (ifd, (LPTR) &hdr->BitsPerPixel, 1);
	intelReadWord (ifd, &hdr->Xmin);
	intelReadWord (ifd, &hdr->Ymin);
	intelReadWord (ifd, &hdr->Xmax);
	intelReadWord (ifd, &hdr->Ymax);
	intelReadWord (ifd, &hdr->HDpi);
	intelReadWord (ifd, &hdr->VDpi);
	FileFDRead (ifd, (LPTR) hdr->Colormap, 48);
	FileFDRead (ifd, (LPTR) &hdr->Reserved, 1);
	FileFDRead (ifd, (LPTR) &hdr->NPlanes, 1);
	intelReadWord (ifd, &hdr->BytesPerLine);
	intelReadWord (ifd, &hdr->PaletteInfo);
	intelReadWord (ifd, &hdr->HscreenSize);
	intelReadWord (ifd, &hdr->VscreenSize);
	if (ifd->err)
		return (BEC_errFileRead);

	/* validate header values */

	switch (hdr->Version)
	{
		case 0:
		case 2:
		case 3:
		case 4:
	 	case 5:
		break;

		default:
			return(BEC_errBadVersion);
			//Message( IDS_BADVERSION );
		break;
	}

	if (hdr->Encoding != 1)
	{
		return(BEC_errBadEncoding);
        //Message( IDS_BADENCODING );		
	}

	switch (hdr->BitsPerPixel)
	{
		case 1:
		case 2:
		case 4:
		case 8:
 		break;

		default:
			return(BEC_errBadBitsPerPixel);
            //Message( IDS_BADBITSPERPIXEL );
		break;
	}

#ifdef UNUSED
	if (hdr->Reserved != 0)
	{
        Message( IDS_BADRESERVEDBYTE );
		return( -1 );
	}

	if (hdr->BytesPerLine & 1)
	{
        Message( IDS_BADBYTESPERLINE );
		return( -1 );
	}
#endif

return( BEC_errNone );
}

/************************************************************************
 * P-FUNCTION pcxReadPlane
 */

static int pcxReadPlane (FILEBUF *ifd, int bpl, LPTR buffer)
{
	int  cnt;
	char data;

	while (bpl > 0)
	{
		/* get count and data value */
		cnt = 1;
		FileFDRead (ifd, (LPTR) &data, 1);
		if (ifd->err)
	    	return (-1);

		if (0xC0 == (0xC0 & data))
		{
		    cnt = 0x3F & data;
	    	FileFDRead (ifd, (LPTR) &data, 1);
		    if (ifd->err)
				return (-1);
		}

		/* put values into buffer */
		bpl -= cnt;
		while (cnt--)
	    	*buffer++ = data;
	}

	return (0);
}

/************************************************************************
 * P-FUNCTION pcxReadRow
 */

static int pcxReadRow (FILEBUF *ifd, PCXHDR FAR *hdr, LPTR buffer)
{
	int  i, k;
	LPTR lpIn, lpOut;
	char byte;
	int  bytesperplane;
	int  bytes;
	int  shift;

	bytes = hdr->BytesPerLine * hdr->NPlanes;

	/* read one plane for the row */
	if (pcxReadPlane (ifd, bytes, buffer) < 0)
		return (-1);

	lpIn = buffer + bytes - 1;
	bytesperplane = hdr->BytesPerLine;

	switch (hdr->BitsPerPixel)
	{
		case 1:
			bytesperplane *= 8;
			lpOut = buffer + 8 * bytes - 1;
			for (k = 0; k < bytes; k++)
			{
	    		byte = *lpIn--;
			    *lpOut-- = byte & 0x01;
			    *lpOut-- = (byte >> 1) & 0x01;
			    *lpOut-- = (byte >> 2) & 0x01;
			    *lpOut-- = (byte >> 3) & 0x01;
			    *lpOut-- = (byte >> 4) & 0x01;
			    *lpOut-- = (byte >> 5) & 0x01;
			    *lpOut-- = (byte >> 6) & 0x01;
			    *lpOut-- = (byte >> 7) & 0x01;
			}
		break;

		case 2:
			bytesperplane *= 4;
			lpOut = buffer + 4 * bytes - 1;

			for (k = 0; k < bytes; k++)
			{
	    		byte = *lpIn--;
			    *lpOut-- = byte & 0x03;
			    *lpOut-- = (byte >> 2) & 0x03;
			    *lpOut-- = (byte >> 4) & 0x03;
			    *lpOut-- = (byte >> 6) & 0x03;
			}
		break;

		case 4:
			bytesperplane *= 2;
			lpOut = buffer + 2 * bytes - 1;
			for (k = 0; k < bytes; k++)
			{
			    byte = *lpIn--;
			    *lpOut-- = byte & 0x0F;
			    *lpOut-- = (byte >> 4) & 0x0F;
			}
		break;
	}

	/* now pack the planes into the first set of bytes */
	bytes = hdr->Xmax - hdr->Xmin + 1;

	for (i = 1; i < hdr->NPlanes; i++)
	{
		shift = i * hdr->BitsPerPixel;
		lpOut = buffer;
		lpIn = lpOut + i * bytesperplane;

		for (k = 0; k < bytes; k++, lpIn++, lpOut++)
		    *lpOut |= (*lpIn << shift);
	}
	return (0);
}

/************************************************************************/

static void BuildPalette (int LevelsPerColor, RGBS FAR *RGBmap)
{
	int Factor, Index, Red, Green, Blue;

	Factor = 255 / (LevelsPerColor - 1);
	Index = 0;

	for (Red = 0; Red < LevelsPerColor; Red++)
	{
		for (Green = 0; Green < LevelsPerColor; Green++)
		{
	    	for (Blue = 0; Blue < LevelsPerColor; Blue++)
			{
				RGBmap[Index].red   = Factor * Red;
				RGBmap[Index].green = Factor * Green;
				RGBmap[Index].blue  = Factor * Blue;
				Index++;
		    }
		}
	}
}

/************************************************************************/

BOOL CReadBitmap::PCXRead()
{
	LPTR    lpFileLine, lpCacheLine, lpRGBLine, lpLineBuffer;
	LPFRAME lpFrame;
	LPTR    lp, lpOut;
	LPRGB   lpRGB, RGBmap;
	FILEBUF ifd;            /* file descriptor (buffered) */
	PCXHDR  hdr;
	PCXMAP  rgbentry;
	BOOL    headermap;
	BOOL    graymap;
	BYTE    colormapflag;
	int     i, k;
	int     FileBPL;        /* bytes per line (file) */
	int     xres;           /* pixels per inch */
	int     npix;           /* image width (pixels) */
	int     nlin;           /* image height (pixels) */
	FRMTYPEINFO inType, inTypeReal, outType;
	int     bpp, outDepth;
	LPCOLORMAP lpColorMap;
	CFrameTypeConvert TypeConvert;
	BOOL fConvert;
	CFile*	pTheFile;
	BITMAP_ERROR_CODE error;
	BOOL	fRet = FALSE;

	ProgressBegin(1);

	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	lpFileLine = NULL;
	lpCacheLine = NULL;
	lpLineBuffer = NULL;
	lpFrame = NULL;

	TRY
	{
		lpColorMap = FrameCreateColorMap();
		if (!lpColorMap)
		{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto Exit;
		}
		RGBmap = lpColorMap->RGBData;

		if( !( lpLineBuffer = Alloc( BUF_SIZE ) ) )
        {
			SetError(BEC_errMemory);
            //Message( IDS_EMEMALLOC );
            goto Exit;
        }

		FileFDOpenRdr (&ifd, pTheFile, lpLineBuffer, BUF_SIZE);

		/* read pcx file header */
		error = pcxReadHeader( &ifd, &hdr);
		if (error != BEC_errNone)
		{
			SetError(error);
			goto Exit;      // error messages handled in called function.
		}

		i = hdr.Version;
	//	j = hdr.BitsPerPixel;
		k = hdr.NPlanes;

		/* get width of image in pixels */
		npix = hdr.Xmax - hdr.Xmin + 1;
		nlin = hdr.Ymax - hdr.Ymin + 1;
		xres = (hdr.HDpi) ? hdr.HDpi : 75;
	//	yres = (hdr.VDpi) ? hdr.VDpi : 75;

		headermap = TRUE;
		graymap = TRUE;

		/* determine total number of bits per pixel */
		if ((hdr.NPlanes == 3) && (hdr.BitsPerPixel == 8))
		{
			bpp = 24;
			graymap = FALSE;
	//		dbg("24bitcolor");
		}
		else
		{
			if (hdr.NPlanes * hdr.BitsPerPixel > 8)
			{
				SetError(BEC_errBadNumColorImages);
                //Message( IDS_BADNUMCOLORIMAGES );
		    	goto Exit;
			}
			bpp = hdr.NPlanes * hdr.BitsPerPixel;

			/***********************************/
			/* get the color map for the image */
			/***********************************/
			/* test for Version == 3, no palette information */

			if ( hdr.Version == 3 || hdr.Version == 4 )
			{
		    	switch (hdr.BitsPerPixel * hdr.NPlanes)
				{
			    	case 1:
						headermap = FALSE;
						graymap = TRUE;
						lpColorMap->NumEntries = 2;
						RGBmap[0].red = RGBmap[0].green = RGBmap[0].blue = 0;
						RGBmap[1].red = RGBmap[1].green = RGBmap[1].blue = 255;
	//					dbg("2 color");
					break;

	    			case 2:
						headermap = FALSE;
						graymap = TRUE;
						lpColorMap->NumEntries = 4;
						RGBmap[0].red = RGBmap[0].green = RGBmap[0].blue = 0;
						RGBmap[1].red = RGBmap[1].green = RGBmap[1].blue = 128;
						RGBmap[2].red = RGBmap[2].green = RGBmap[2].blue = 192;
						RGBmap[3].red = RGBmap[3].green = RGBmap[3].blue = 255;
	//					dbg("4 color");
					break;

			    	case 3:
						headermap = FALSE;
						graymap = FALSE;
						lpColorMap->NumEntries = 8;
						BuildPalette (2, RGBmap);
	//					dbg("8 color");
					break;

					case 4:
						headermap = FALSE;
						graymap = FALSE;
						lpColorMap->NumEntries = 16;
						for (i = 0; i < 16; i++)
						{
					    	RGBmap[i].red   = (i&4) ? ((i&8) ? 0xFF : 0x7F) : 0;
					    	RGBmap[i].green = (i&2) ? ((i&8) ? 0xFF : 0x7F) : 0;
					    	RGBmap[i].blue  = (i&1) ? ((i&8) ? 0xFF : 0x7F) : 0;
						}
	//					dbg("16 color");
					break;

			    	case 6:
						headermap = FALSE;
						graymap = FALSE;
						lpColorMap->NumEntries = 16;
						BuildPalette (4, RGBmap);
	//					dbg("16 color");
					break;

	    			case 8:
						headermap = FALSE;
						graymap = TRUE;
						lpColorMap->NumEntries = 256;
						for (i = 0; i < 256; i++)
						{
					    	RGBmap[i].red   = i;
					    	RGBmap[i].green = i;
					    	RGBmap[i].blue  = i;
						}
	//					dbg("256 gray");
					break;
	    		}
			}

			/* read 256 color color map, if any */
			else if ((hdr.BitsPerPixel == 8) && (hdr.Version == 5))
			{
				/* seek to loc of colormap */
		    	if ( FileFDSeek (&ifd, -769L, 2) < 0)
					goto BadRead;

		    	FileFDRead (&ifd, (LPTR) &colormapflag, 1);
		    	if (ifd.err)
					goto BadRead;

		    	if (colormapflag == 12)
				{
			    	/* read the color map */
	//				dbg("256 color");
					headermap = FALSE;
					graymap = TRUE;

					lpColorMap->NumEntries = 256;
					for (i = 0; i < 256; i++)
					{
				    	FileFDRead (&ifd, (LPTR) &rgbentry, 3);
				    	RGBmap[i].red   = rgbentry.Red;
				    	RGBmap[i].green = rgbentry.Green;
				    	RGBmap[i].blue  = rgbentry.Blue;
				    	if (graymap)
						{
							graymap = (rgbentry.Red == rgbentry.Green) &&
						  	(rgbentry.Red == rgbentry.Blue);
				    	}
					}

					if (ifd.err != 0)
			    		goto BadRead;
	    		}
			}

			/* use colormap from header if extended map not being used */
			if (headermap)
			{
	    		k = 0;
		    	headermap = FALSE;
		    	graymap = TRUE;
	//			dbg("-%d bits", hdr.BitsPerPixel * hdr.NPlanes);
		    	if ((hdr.BitsPerPixel * hdr.NPlanes) == 1 )
				{
					lpColorMap->NumEntries = 2;
					graymap = TRUE;
					RGBmap[0].red = RGBmap[0].green = RGBmap[0].blue = 0;
					RGBmap[1].red = RGBmap[1].green = RGBmap[1].blue = 255;
				}
		    	else
	    		for (i = 0; i < (1 << (hdr.BitsPerPixel * hdr.NPlanes)); i++)
				{
					++lpColorMap->NumEntries;
					RGBmap[i].red   = hdr.Colormap [k++];
					RGBmap[i].green = hdr.Colormap [k++];
					RGBmap[i].blue  = hdr.Colormap [k++];
	//				dbg("(%d,%d,%d)", RGBmap[i].red,RGBmap[i].green,RGBmap[i].blue);
					if (graymap)
					{
				    	graymap = (RGBmap[i].red == RGBmap[i].green) &&
							(RGBmap[i].red == RGBmap[i].blue);
					}
	    		}
			}
		}

		/* set inType */
		if (bpp == 1)
			FrameSetTypeInfo(&inType, FDT_LINEART);
		else
		if (bpp == 24)
			FrameSetTypeInfo(&inType, FDT_RGBCOLOR);
		else
		if (graymap)
			FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
		else
			FrameSetTypeInfo(&inType, FDT_PALETTECOLOR, lpColorMap);

		if (!SetupTypes(&inType, &outType) )
			goto Exit;
		
		// once we've used inType to setup outType see if inType is lineart
		// because if it is, it gets converted to grayscale on the way in
		inTypeReal = inType;
		if (inType.DataType == FDT_LINEART)
			FrameSetTypeInfo(&inType, FDT_GRAYSCALE);

		/* Create the image frame store */
		lpFrame = FrameOpen(outType, npix, nlin, xres);
		if ( !lpFrame )
		{
			SetError(BEC_errFrameOpen);
			//FrameError(IDS_EIMAGEOPEN);
			goto Exit;
		}
		outDepth = FrameDepth(lpFrame);
		if (!outDepth)
			outDepth = 1;

		/* set number of bytes per line (1 byte per pixel) */
		FileBPL = hdr.BytesPerLine * hdr.NPlanes;

		switch (hdr.BitsPerPixel)
		{
			case 1:
				FileBPL *= 8;
			break;

			case 2:
				FileBPL *= 4;
			break;

			case 4:
				FileBPL *= 2;
			break;
		}

		/* seek to loc of image data */
		if ( FileFDSeek (&ifd, 128L, 0) < 128)
			goto BadRead;

		/* allocate space for one line of the image */
		if ( !AllocLines (&lpCacheLine, 1, npix, outDepth))
		{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC );
			goto BadWrite;
		}

		/* allocate space for one line of the image (file) */
		if ( !AllocLines (&lpFileLine, 1, FileBPL, (graymap) ? 1 : 4))
		{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC);
			goto BadWrite;
		}

		// make sure we can setup our type converter if we need it
		fConvert = !FrameTypeInfoEqual(inType, outType);
		if (bpp != 24)
			fConvert = TRUE;
		if (fConvert)
		{
			if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
			{
				SetError(BEC_errMemory);
				//Message(IDS_EMEMALLOC);
				goto Exit;	
			}
		}

		/* convert the image */
		lpRGBLine = lpFileLine + FileBPL;

		for (i = 0; i < nlin; i++)
		{
			if (Progress( i, nlin, YES ))
				goto Cancelled;

			lpOut = FramePointerRaw( lpFrame, 0, i, TRUE );

			if (!lpOut)
				break;

			if (bpp == 24)
			{
				/* get red, grn, blu */
		    	if (pcxReadPlane (&ifd, FileBPL, lpFileLine) < 0)
					goto BadRead;

		    	lp = lpFileLine;

				if ( fConvert )
				{
					// Get red data
	    			lpRGB = (LPRGB)lpRGBLine;
					k = npix;
					while(k-- > 0)
					{
						lpRGB->red = *lp++;
						lpRGB++;
					}

					// Get green data
					lp = lpFileLine + hdr.BytesPerLine;
		    		lpRGB = (LPRGB)lpRGBLine;
					k = npix;
					while(k-- > 0)
					{
						lpRGB->green = *lp++;
						lpRGB++;
					}

					// Get blue data
					lp = lpFileLine + (hdr.BytesPerLine * 2);
		    		lpRGB = (LPRGB)lpRGBLine;
					k = npix;
					while(k-- > 0)
					{
						lpRGB->blue = *lp++;
						lpRGB++;
					}
					TypeConvert.ConvertData(lpRGBLine, lpOut, i, npix);
				}
				else
				{
					// Get red data
	    			lpRGB = (LPRGB)lpOut;
					k = npix;
					while(k-- > 0)
					{
						lpRGB->red = *lp++;
						lpRGB++;
					}

					// Get green data
					lp = lpFileLine + hdr.BytesPerLine;
		    		lpRGB = (LPRGB)lpOut;
					k = npix;
					while(k-- > 0)
					{
						lpRGB->green = *lp++;
						lpRGB++;
					}

					// Get blue data
					lp = lpFileLine + (hdr.BytesPerLine * 2);
		    		lpRGB = (LPRGB)lpOut;
					k = npix;
					while(k-- > 0)
					{
						lpRGB->blue = *lp++;
						lpRGB++;
					}
				}
			}
			else
			{
	    		if (pcxReadRow (&ifd, (PCXHDR far *) &hdr, lpFileLine))
					goto BadRead;

	    		if (graymap)
				{
					lp = lpFileLine;
					for (k = 0; k < npix; k++, lp++)
				    	*lp = RGBmap[*lp].red;
				}
				TypeConvert.ConvertData(lpFileLine, lpOut, i, npix);
			}
		}
		m_iWidth = npix;
		m_iHeight = nlin;
		m_iRes = xres;
		m_ObjList.lpHead = m_ObjList.lpTail = NULL;
		m_lpFrame = lpFrame;
		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

BadRead:
	SetError(BEC_errFileRead);
	//Message( IDS_EREAD, m_lpFileName );
	goto Exit;

BadWrite:
	SetError(BEC_errFrameRead);
	//FrameError(IDS_EFRAMEREAD);

Cancelled:
Exit:
	if ( lpFileLine )
		FreeUp( lpFileLine );
	if ( lpCacheLine )
		FreeUp( lpCacheLine );
	if ( lpLineBuffer )
		FreeUp( lpLineBuffer );
	CloseFile(pTheFile);
	if (!fRet && lpFrame)
		FrameClose(lpFrame);
	if (lpColorMap)
		FrameDestroyColorMap(lpColorMap);
	ProgressEnd();
	return( fRet );
}

