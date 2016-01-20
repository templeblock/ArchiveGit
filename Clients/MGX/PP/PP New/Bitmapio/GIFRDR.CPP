//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "bitmapio.h"
#include "gif.h"
#include "compress.h"
#include "intelio.h"

// Static prototypes
static int gifReadHeader( FILEBUF *ifd, GIFHDR *hdr );
static int gifReadImDesc( FILEBUF *ifd, GIFDESC *ImDesc );
static void gifInitColorMap( GIFMAP *map );
static int gifReadColorMap( FILEBUF *ifd, int bpp, GIFMAP *map );

typedef struct {
    int		first;
    int		step;
} GIFROW;

static GIFROW gifRowMap [5] = {
    {0, 1},	/* not interlaced */
    {0, 8},	/* first frame of interlaced date */
    {4, 8},	/* second frame of interlaced date */
    {2, 4},	/* third frame of interlaced date */
    {1, 2},	/* fourth frame of interlaced date */
};

#define  BUF_SIZE 16 * 1024

/************************************************************************
 * P-FUNCTION gifReadHeader
 */

static int gifReadHeader( FILEBUF *ifd, GIFHDR *hdr )
{
    FileFDRead (ifd, (LPTR) hdr->Signiture, 6);
    intelReadWord (ifd, &hdr->ScnWidth);
    intelReadWord (ifd, &hdr->ScnHeight);
    FileFDRead (ifd, (LPTR) &hdr->RawImInfo, 1);
    FileFDRead (ifd, (LPTR) &hdr->Background, 1);
    FileFDRead (ifd, (LPTR) &hdr->Zero, 1);
    if (ifd->err)
	return (-1);

/* validate header values */
    if ((hdr->Signiture[0] != 'G') ||
		(hdr->Signiture[1] != 'I') ||
		(hdr->Signiture[2] != 'F'))
	return (-1);

//    if (hdr->Zero != 0)
//	return (-1);

/* pick apart the RawImInfo byte */
    hdr->GlobalMap = (hdr->RawImInfo & 0x80) ? TRUE : FALSE;
    hdr->ColorRes = ((hdr->RawImInfo >> 4) & 0x07) + 1;
    hdr->bpp = (hdr->RawImInfo & 0x07) + 1;

    return (0);
}


/************************************************************************
 * P-FUNCTION gifReadImDesc
 */

static int gifReadImDesc( FILEBUF *ifd, GIFDESC *ImDesc )
{
    intelReadWord (ifd, &ImDesc->ImLeft);
    intelReadWord (ifd, &ImDesc->ImTop);
    intelReadWord (ifd, &ImDesc->ImWidth);
    intelReadWord (ifd, &ImDesc->ImHeight);
    FileFDRead (ifd, (LPTR) &ImDesc->RawImInfo, 1);

/* pick apart the RawImInfo byte */
    ImDesc->LocalMap = (ImDesc->RawImInfo & 0x80) ? TRUE : FALSE;
    ImDesc->Interlaced = (ImDesc->RawImInfo & 0x40) ? TRUE : FALSE;
    ImDesc->bpp = (ImDesc->RawImInfo & 0x07) + 1;

    return (ifd->err);
}


/************************************************************************
 * P-FUNCTION gifInitColorMap
 */

static void gifInitColorMap( GIFMAP *map )
{
    int		i;

    map->Length = 256;

/* Initialize the map */
    for (i = 0; i < 256; i++) {
	map->Map[i].red =
	map->Map[i].green =
	map->Map[i].blue = i;
    }
}


/************************************************************************
 * P-FUNCTION gifReadColorMap
 *	(if ifd == NULL, initialize the color map)
 */

static int gifReadColorMap( FILEBUF *ifd, int bpp, GIFMAP *map )
{
    int		i;

    map->Length = 1 << bpp;

/* load the color map */
    for (i = 0; i < map->Length; i++) {
	FileFDRead (ifd, &map->Map[i].red, 1);
	FileFDRead (ifd, &map->Map[i].green, 1);
	FileFDRead (ifd, &map->Map[i].blue, 1);
//	dbg( "gifrdr %d: (%d,%d,%d)", i,
//		map->Map[i].red, map->Map[i].green, map->Map[i].blue );
    }

/* Initialize the rest */
    for ( ; i < 256; i++) {
	map->Map[i].red =
	map->Map[i].green =
	map->Map[i].blue = i;
    }

    return (ifd->err);
}


/************************************************************************/
BOOL CReadBitmap::GIFRead()
/************************************************************************/
{
    GIFHDR		   hdr;
    GIFDESC		   ImDesc;
    GIFMAP		   GlobalMap;
    GIFMAP		   LocalMap;
    BYTE		      cTemp;
    LPTR		      lpFileLine, lpLineBuffer, lpOut;
    LPFRAME		   lpFrame;
    FILEBUF		   ifd;		/* file descriptor (buffered) */
    BOOL		      graymap;
    int			   i;
    int			   sy;
    int			   xres;		/* pixels per inch */
    int			   npix;		/* image width (pixels) */
    int			   nlin;		/* image height (pixels) */
    BYTE		      codeSize;
    int			   iCodeSize;
    int			   iRowMapIndex;
    BOOL		      compressInit;
    LPLZW_STUFF   lpLZW;
	FRMTYPEINFO inType, outType;
	LPCOLORMAP lpColorMap;
	CFrameTypeConvert TypeConvert;
	CFile 	theFile;
	CFile*	pTheFile;
	BOOL	fRet = FALSE;

	ProgressBegin(1);
	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		return(FALSE);
	}

	TRY
	{
    	lpFileLine = NULL;
    	lpFrame = NULL;
    	lpLineBuffer = NULL;
    	compressInit = NO;
		lpColorMap = NULL;

    	if (!(lpLineBuffer = Alloc (BUF_SIZE)))
	      	goto Exit;

    	FileFDOpenRdr (&ifd, pTheFile, lpLineBuffer, BUF_SIZE);

		/* initialize the Global and local color maps */
    	gifInitColorMap (&GlobalMap);
    	gifInitColorMap (&LocalMap);

		/* read gif file header */
    	if (gifReadHeader (&ifd, &hdr))
		goto BadRead;

		/* get global color map, if any */
    	if (hdr.GlobalMap) {
		if (gifReadColorMap (&ifd, hdr.bpp, &GlobalMap))
	    	goto BadRead;
    	}
		
		/* look for start of image */
    	while (1) {
		FileFDRead (&ifd, (LPTR) &cTemp, 1);
		if (ifd.err)
	    	goto BadRead;

    	/* test for image separator character */
		if (cTemp == GIFImSep)
	    	break;

    	/* test for terminator character (no image blocks in file?) */
		if (cTemp == GIFImSep)
	    	goto BadRead;

    	/* test for extension block character */
		if (cTemp == GIFExtBlk) {

		/* Skip over the extension block */

		/* read function code */
	    	FileFDRead (&ifd, (LPTR) &cTemp, 1);

	    	do {
	    	/* read byte count */
			FileFDRead (&ifd, (LPTR) &cTemp, 1);

	    	/* skip data bytes */
			if (cTemp)
		    	FileFDSeek (&ifd, (long) cTemp, 1);
	    	} while (cTemp);
		}
    	}

	/* now at the start of the first image */
    	if (gifReadImDesc (&ifd, &ImDesc))
		goto BadRead;

	/* read local color map, if any */
    	if (ImDesc.LocalMap) {
		if (gifReadColorMap (&ifd, ImDesc.bpp, &LocalMap))
	    	goto BadRead;
    	}
    	else {
		LocalMap = GlobalMap;
		ImDesc.bpp = hdr.bpp;
    	}

	/* check for gray map */
    	graymap = TRUE;
    	for (i = 0; (i < LocalMap.Length) && graymap; i++)
		graymap = (LocalMap.Map[i].red == LocalMap.Map[i].green)
		    	&& (LocalMap.Map[i].red == LocalMap.Map[i].blue);

		lpColorMap = FrameCreateColorMap();
		if (!lpColorMap)
		{
			SetError(BEC_errMemory);
			goto Exit;
		}
		lpColorMap->NumEntries = LocalMap.Length;
		for (i = 0; i < LocalMap.Length; ++i)
			lpColorMap->RGBData[i] = LocalMap.Map[i];


	/* get width of image in pixels */
    	npix = ImDesc.ImWidth;
    	nlin = ImDesc.ImHeight;
    	xres = 75;

		if (hdr.bpp == 1)
			FrameSetTypeInfo(&inType, FDT_LINEART);
		else
			FrameSetTypeInfo(&inType, FDT_PALETTECOLOR, lpColorMap);

		if (!SetupTypes(&inType, &outType, graymap))
			goto Exit;

		FrameSetTypeInfo(&inType, FDT_PALETTECOLOR, lpColorMap);

		if (!TypeConvert.Init(inType, outType, npix, m_DitherType))
		{			   
			SetError(BEC_errMemory);
			goto Exit;	
		}

	/* allocate space for one line of the image (file) */
    	if ( !AllocLines (&lpFileLine, 1, npix, 1)) {
		SetError(BEC_errMemory);
		goto BadWrite;
    	}

	/* Create the image frame store */
    	lpFrame = FrameOpen(outType, npix, nlin, xres);
    	if ( !lpFrame ) {
		SetError(BEC_errFrameOpen);
		goto Exit;
    	}

	/* convert the image */
    	if (FileFDRead (&ifd, &codeSize, 1) == -1)
		goto BadRead;

    	iCodeSize = codeSize;

    	if (FileFDSeek (&ifd, 0L, 1) == -1)
		goto BadRead;

    	if ( !( lpLZW = DecompressLZW_GIF (ifd.pFile, NULL, 0, iCodeSize, NULL)))
		goto BadRead;
    	compressInit = YES;
    	if (ImDesc.Interlaced)
		iRowMapIndex = 1;
    	else
		iRowMapIndex = 0;

		sy = gifRowMap [iRowMapIndex].first;
		for (i = 0; i < nlin; i++)
		{
			if (Progress (i, nlin, YES))
				goto Exit;
			if (!(DecompressLZW_GIF (ifd.pFile, lpFileLine, npix, iCodeSize, lpLZW)))
				goto BadRead;
			if ( !(lpOut = FramePointerRaw(lpFrame, 0, sy, YES)) )
				goto BadWrite;
			TypeConvert.ConvertData(lpFileLine, lpOut, sy, npix);
			sy += gifRowMap [iRowMapIndex].step;
			if (sy >= ImDesc.ImHeight)
			{
	    		iRowMapIndex++;
		    	sy = gifRowMap [iRowMapIndex].first;
			}
		}

		m_iWidth = npix;
		m_iHeight = nlin;
		m_iRes = xres;
		m_lpFrame = lpFrame;
		fRet = TRUE;
		goto Exit;
	}
	CATCH_ALL(e)
	{
		goto BadRead;
	}
	END_CATCH_ALL

	ProgressEnd();
	return (TRUE);

BadRead:
	SetError(BEC_errFileRead);
    goto Exit;

BadWrite:
	SetError(BEC_errFrameRead);

Exit:
    if (compressInit)
 	   DecompressLZW_GIF (ifd.pFile, NULL, 0, iCodeSize, lpLZW);
    compressInit = NO;
    if ( lpFileLine )
	FreeUp( lpFileLine );
    if ( lpLineBuffer )
	FreeUp (lpLineBuffer);
	if ( lpColorMap )
		FrameDestroyColorMap(lpColorMap);
	CloseFile(pTheFile);

	if (!fRet && lpFrame)
		FrameClose(lpFrame);

	ProgressEnd();
    return(fRet);
}


