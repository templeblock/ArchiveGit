
//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#include "intelio.h"
#include "getline.h"
#include "cpo.h"

typedef struct _pcxmap
{
	BYTE Red;
	BYTE Green;
	BYTE Blue;
} PCXMAP;

typedef struct _pcxhdr
{
	BYTE Manufacturer;
	BYTE Version;
	BYTE Encoding;
	BYTE BitsPerPixel;
	WORD Xmin, Ymin, Xmax, Ymax;
	WORD HDpi;
	WORD VDpi;
	BYTE Colormap [48];	/* rgb rgb rgb rgb ... */
	BYTE Reserved;
	BYTE NPlanes;
	WORD BytesPerLine;
	WORD PaletteInfo;
	WORD HscreenSize;
	WORD VscreenSize;
	BYTE Filler [54];
} PCXHDR;

typedef struct _bufio
{
	CFile *pFile;
	LPTR lpFileBuffer;
	LPTR lpFileData;
	int  iFileBufCount;
} BUFIO, FAR *LPBUFIO;

#define FILE_BUF_SIZE 8192


#define FLUSH_BUFFER(lpIO) \
{ \
	if ((lpIO)->iFileBufCount > 0) \
	{ \
		(lpIO)->pFile->Write( (lpIO)->lpFileBuffer, (lpIO)->iFileBufCount ); \
		(lpIO)->lpFileData = (lpIO)->lpFileBuffer; \
		(lpIO)->iFileBufCount = 0; \
	} \
}

#define WRITE_CHAR(lpIO, c) \
{ \
	if ((lpIO)->iFileBufCount >= FILE_BUF_SIZE) \
		FLUSH_BUFFER((lpIO)); \
	*(lpIO)->lpFileData++ = c; \
	(lpIO)->iFileBufCount++; \
}

/************************************************************************
 * FUNCTION pcxEncPut
 *	Put encoded image data into PCX file.
 */

#define pcxEncPut(lpIO, cnt, byte) \
{ \
	if (cnt) \
	{ \
		if ((cnt == 1) && (0xC0 != (0xC0 & byte))) \
		{ \
			WRITE_CHAR((lpIO), byte); \
		} \
		else \
		{ \
			cnt |= 0xC0; \
			WRITE_CHAR((lpIO), cnt); \
			WRITE_CHAR((lpIO), byte); \
		} \
	} \
}

/************************************************************************
 * FUNCTION pcxEncLine
 *	Put line of image data into PCX file.
 */

static void pcxEncLine( LPBUFIO lpIO, LPTR buf, int len )
{
	unsigned char current, last;
	unsigned char runCount;
	int srcindex;

	runCount = 1;
	last = *buf;

	for (srcindex = 1; srcindex < len; srcindex++)
	{
		current = *(++buf);
		if (current == last)
		{
			runCount++;
			if (runCount == 63)
			{
				pcxEncPut (lpIO, runCount, last);
				runCount = 0;
			}
		}
		else
		{
			if (runCount)
				pcxEncPut (lpIO, runCount, last);
			last = current;
			runCount = 1;
		}
	}
	if (runCount)
		pcxEncPut (lpIO, runCount, last);
}

/************************************************************************/

BOOL CWriteBitmap::PCXWrite()
{
	PCXHDR hdr;
	PCXMAP rgbentry;
	RGBS  RGBmap[256];
	LPRGB lpRGB, lpRGBmap;
	LPTR  lpOut;
	int   i, k, npix, nlin;
	LPTR  lp, lpBuffer, lpFileLine, lpImgScanline;
	RECT  rSave;
	BOOL  bEscapable;
	LPOBJECT lpObject;
	LPFRAME lpFrame;
	FRMTYPEINFO outType, inType;
	CFrameTypeConvert TypeConvert;
	BUFIO io;
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
	
	if (m_pFile)
	{
		pTheFile = m_pFile;
	}
	else
	{
		if (!theFile.Open(m_lpFileName, CFile::modeCreate|CFile::modeWrite))
		{
			SetError(BEC_errWriteFile);
			//Message (IDS_EWRITE, m_lpFileName);
			return(FALSE);
		}
		pTheFile = &theFile;
	}

	TRY
	{
		ProgressBegin(1);

		io.pFile = pTheFile;
		io.lpFileBuffer = NULL;
		io.lpFileData = NULL;
		io.iFileBufCount = 0;

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

		lpBuffer      = NULL;
		lpImgScanline = NULL;
		lpFileLine    = NULL;

		inType = FrameTypeInfo(lpFrame);
		// cause FramePointer never returns line art
		if (inType.DataType == FDT_LINEART)
			FrameSetTypeInfo(&inType, FDT_GRAYSCALE);
		// make sure colormap is inited for outType so we don't free bogus ptr
		FrameSetTypeInfo(&outType, FDT_NONE);

		/* initialize file header */
		lp = (LPTR) & hdr;
		for (i = 0; i < sizeof (hdr); i++)
			*lp++ = 0;

		hdr.Manufacturer = 10;
		hdr.Version = 5;
		hdr.Encoding = 1;
		hdr.Xmax = npix-1;
		hdr.Ymax = nlin-1;
		hdr.HDpi = FrameResolution(lpFrame);
		hdr.VDpi = FrameResolution(lpFrame);
		hdr.NPlanes = 1;
		hdr.BytesPerLine = 0;
		hdr.PaletteInfo = 1;
		hdr.HscreenSize = 640;
		hdr.VscreenSize = 480;

		lpRGBmap = RGBmap;
		/* determine type of image to create */
		switch (m_DataType)
		{
			case FDT_GRAYSCALE:
				/* gray */
				FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
				hdr.BitsPerPixel = 8;
				hdr.BytesPerLine = npix;
				hdr.PaletteInfo = 2;

				/* build the gra color map */
				for (i = 0; i < 256; i++)
				{
					RGBmap[i].red =
					RGBmap[i].green =
					RGBmap[i].blue = i;
				}
			break;
 
			case FDT_RGBCOLOR:
			case FDT_CMYKCOLOR:
				/* full color */
				FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
				hdr.BitsPerPixel = 8;
				hdr.NPlanes = 3;
				hdr.BytesPerLine = npix;
				hdr.PaletteInfo = 1;
			break;

			case FDT_PALETTECOLOR:
				/* mini color */
				FrameSetTypeInfo(&outType, FDT_PALETTECOLOR);
				hdr.BitsPerPixel = 8;
				hdr.BytesPerLine = npix;
				hdr.PaletteInfo = 1;

				if (inType.DataType == FDT_PALETTECOLOR)
					outType = inType;
				else
				{
					outType.ColorMap = FrameCreateColorMap();
					if (!outType.ColorMap)
					{
						SetError(BEC_errMemory);
						//Message(IDS_EMEMALLOC);
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
						//Message(IDS_EMEMALLOC);
						goto Exit;
					}
				}
				lpRGBmap = outType.ColorMap->RGBData;
			break;

			case FDT_LINEART:
				/* line art or scatter */
				FrameSetTypeInfo(&outType, FDT_LINEART);
				hdr.BitsPerPixel = 1;
				hdr.BytesPerLine = ((npix + 7) / 8);
				hdr.PaletteInfo = 2;
				hdr.Colormap[0] = hdr.Colormap[1] = hdr.Colormap[2] = 0;
				hdr.Colormap[3] = hdr.Colormap[4] = hdr.Colormap[5] = 255;
			break;

			default:
				//Print ("Unknown flag value");
				goto Exit;
			break;
		}

		// initialize stuff to do the type conversion
		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{
			SetError(BEC_errMemory);
			//Message(IDS_EMEMALLOC);
			goto Exit;
		}

		/* round bytes per line to a 16 bit boundry */
		hdr.BytesPerLine = (hdr.BytesPerLine + 1) & ~1;

		AllocLines( &lpImgScanline, 1, npix, 4 );
		AllocLines( &lpBuffer, 1, max (hdr.BytesPerLine, npix), 3);

		io.lpFileBuffer = Alloc(FILE_BUF_SIZE);

		if (!lpImgScanline || !lpBuffer || !io.lpFileBuffer)
		{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC);
			goto Exit;
		}

		io.lpFileData = io.lpFileBuffer;
		io.iFileBufCount = 0;

		/* write file header */
		pTheFile->Write((LPTR) &hdr.Manufacturer, 1);
		pTheFile->Write((LPTR) &hdr.Version, 1);
		pTheFile->Write((LPTR) &hdr.Encoding, 1);
		pTheFile->Write((LPTR) &hdr.BitsPerPixel, 1);
		pTheFile->Write((LPTR) &hdr.Xmin, 2);
		pTheFile->Write((LPTR) &hdr.Ymin, 2);
		pTheFile->Write((LPTR) &hdr.Xmax, 2);
		pTheFile->Write((LPTR) &hdr.Ymax, 2);
		pTheFile->Write((LPTR) &hdr.HDpi, 2);
		pTheFile->Write((LPTR) &hdr.VDpi, 2);
		pTheFile->Write((LPTR) hdr.Colormap, 48);
		pTheFile->Write((LPTR) &hdr.Reserved, 1);
		pTheFile->Write((LPTR) &hdr.NPlanes, 1);
		pTheFile->Write((LPTR) &hdr.BytesPerLine, 2);
		pTheFile->Write((LPTR) &hdr.PaletteInfo, 2);
		pTheFile->Write((LPTR) &hdr.HscreenSize, 2);
		pTheFile->Write((LPTR) &hdr.VscreenSize, 2);
		pTheFile->Write((LPTR) hdr.Filler, 54);

		/* write the image */
		if (outType.DataType == FDT_RGBCOLOR)
		{
			if (!AllocLines (&lpFileLine, 1, hdr.BytesPerLine, 1))
			{
				SetError(BEC_errMemory);
				//Message (IDS_EMEMALLOC);
				goto Exit;
			}

			for (i = rSave.top; i <= rSave.bottom; i++)
			{
				if (Progress(i-rSave.top, nlin, bEscapable ))
					goto Cancelled;

				if (lpObject)
				{
					if (!ObjGetLine( lpObject, rSave.left, i,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;

					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, i, NO)) )
						FreeUp (lpFileLine);
				}

				// convert the data to the new type
				TypeConvert.ConvertData(lp, lpBuffer, i, npix);

				/* save red plane */
				lpRGB = (LPRGB)lpBuffer;
				lpOut = lpFileLine;
				for (k = 0; k < npix; k++)
				{
					*lpOut++ = (lpRGB++)->red;
				}
				pcxEncLine (&io, lpFileLine, hdr.BytesPerLine);

				/* save green plane */
				lpRGB = (LPRGB)lpBuffer;
				lpOut = lpFileLine;
				for (k = 0; k < npix; k++)
					*lpOut++ = (lpRGB++)->green;
				pcxEncLine (&io, lpFileLine, hdr.BytesPerLine);

				/* save blue plane */
				lpRGB = (LPRGB)lpBuffer;
				lpOut = lpFileLine;
				for (k = 0; k < npix; k++)
					*lpOut++ = (lpRGB++)->blue;
				pcxEncLine (&io, lpFileLine, hdr.BytesPerLine);
			}

			FLUSH_BUFFER(&io);
		}
		else
		{
			int depth = FrameDepth( lpFrame );

			if (depth == 0)
				depth = 1;

			for (i = rSave.top; i <= rSave.bottom; i++)
			{
				if (Progress(i-rSave.top, nlin, bEscapable ))
					goto Cancelled;

				if (lpObject)
				{
					if (!ObjGetLine( lpObject, rSave.left, i,
						(rSave.right - rSave.left) + 1, lpImgScanline))
						goto BadRead;

					lp = lpImgScanline;
				}
				else
				{
					if ( !(lp = FramePointer( lpFrame, rSave.left, i, NO)) )
						goto BadRead;
				}

				// convert the data to the new type
				TypeConvert.ConvertData(lp, lpBuffer, i, npix);

				pcxEncLine (&io, lpBuffer, hdr.BytesPerLine);
			}

			FLUSH_BUFFER(&io);

			if (hdr.BitsPerPixel == 8)
			{
				/* write extended color map */
				rgbentry.Red = 12;
				pTheFile->Write((LPTR) &rgbentry.Red, 1);
				for (i = 0; i < 256; i++)
				{
					rgbentry.Red   = lpRGBmap[i].red;
					rgbentry.Green = lpRGBmap[i].green;
					rgbentry.Blue  = lpRGBmap[i].blue;
					pTheFile->Write((LPTR) &rgbentry, 3);
				}
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
	//Message (IDS_EWRITE, m_lpFileName);
	goto Exit;

BadRead:
	SetError(BEC_errFrameRead);
	//Message (IDS_EREAD, (LPTR)Control.RamDisk);

Cancelled:
Exit:
	/* clean up */
	if (!m_pFile) // did we create the file?
	{
		TRY
		{
			pTheFile->Close();
			if (!fRet )
				CFile::Remove(m_lpFileName);
		}
		END_TRY
	}

	if (lpBuffer)
		FreeUp (lpBuffer);
	if (lpImgScanline)
		FreeUp( lpImgScanline );
	if (lpFileLine)
		FreeUp( lpFileLine );
	if (io.lpFileBuffer)
		FreeUp( io.lpFileBuffer );
	if (outType.ColorMap && (outType.ColorMap != inType.ColorMap))
		FrameDestroyColorMap(outType.ColorMap);

	ProgressEnd();

	return (fRet);
}



