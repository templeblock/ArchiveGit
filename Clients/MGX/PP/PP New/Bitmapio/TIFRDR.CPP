//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "stdafx.h"
#include "bitmapio.h"
#include "compress.h"
#include "cmsutil.h"
#include "mask.h"
#include "object.h"

// Static prototypes
static void ReadHorzDiff( LPTR lpImage, int iCount, int SamplesPerPixel );
static void diff_fixtagval( LPTAG tagp );
static void rightjust_tagval( LPTAG tagp );
static int flexiread( CFile *pFile, LPTR ptr, long size, short order );
static void skip_to_strip(LPTIFFHEADER lpTH);
static void Move4to8bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Expand4to8bits  ( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Expand6to8bits  ( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Expand4to24bits ( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Expand8to24bits ( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Shrinkto8bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Shrinkto24bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void Shrinkto32bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine );
static void copyn( LPTR lpIn, LPTR lpOut, int iCount, int incr );
static BOOL image_write (LPTIFFHEADER lpTH, LPTR lpSrc, int xSrc, UINT numBytes, CFile *pFile);
LOCAL DWORD TiffReadBytes(DWORD dwData, LPTR lpDst, DWORD dwBytes);

#define REALSIZE(pix, depth) ((depth == 0) ? ((pix+7)/8) : (pix*depth) )

/************************************************************************/
LOCAL BOOL TiffProgress(int nPart, int nWhole, BOOL bEscapable, DWORD dwStatusData)
/************************************************************************/
{
	CReadBitmap *pReader = (CReadBitmap *)dwStatusData;
	return(pReader->Progress(nPart, nWhole, bEscapable));
}

/************************************************************************/
BOOL CReadBitmap::TiffRead()
/************************************************************************/
{
int	y, yin, p, i, linread;
LPFRAME lpFrame, lpAlphaFrame;
LPOBJECT lpObject;
LPALPHA lpAlpha;
LPTR lpIn, lpOut, lpCropLine;
BOOL compressInit, bSkip, fRet;
LFIXED xrate, yrate, yoffset;
int ylast, yline, opix, olin, xstart, xend, ystart, yend;
int Resolution;
LPTIFFHEADER lpTH;
LPLZW_STUFF  lpLZW;  
CFrameTypeConvert TypeConvert;
BOOL fConvert = FALSE;
CFile*	pTheFile;

ProgressBegin(1);

if ((pTheFile = OpenFile()) == NULL)
	{
	ProgressEnd();
	return(FALSE);
	}

fRet = FALSE;
lpFrame = lpAlphaFrame = NULL;
lpAlpha = NULL;
compressInit = NO;

if ( !(lpTH = ReadTiffHeader(pTheFile)) )
	goto BadRead;

if ( m_SampleMode != modeNoSampling )
	{
	xstart = 0;
	xend = lpTH->npix - 1;
	ystart = 0;
	yend = lpTH->nlin - 1;
	if (m_SampleMode == modeSampleRect)
		{
		opix = RectWidth(&m_rArea);
		olin = RectHeight(&m_rArea);
		}
	else
		{
		xrate = FGET(m_SampleRes, lpTH->lResolution);
		opix = FMUL(lpTH->npix, xrate);
		olin = FMUL(lpTH->nlin, xrate);
		}
	xrate = ScaleToFit(&opix, &olin, lpTH->npix, lpTH->nlin);
	if (opix > lpTH->npix || olin > lpTH->nlin)
		{ // No upsizing allowed
		opix = lpTH->npix;
		olin = lpTH->nlin;
		Resolution = lpTH->lResolution;
		}
	else
		Resolution = FMUL(lpTH->lResolution, xrate);
	xrate = FGET( lpTH->npix, opix );
	yrate = FGET( lpTH->nlin, olin );
	}
else
if ( m_fCrop )
	{
	xstart = bound( m_rArea.left, 0, lpTH->npix-1 );
	xend = bound( m_rArea.right, 0, lpTH->npix-1 );
	ystart = bound( m_rArea.top, 0, lpTH->nlin-1 );
	yend = bound( m_rArea.bottom, 0, lpTH->nlin-1 );
	opix = xend - xstart + 1;
	olin = yend - ystart + 1;
	xrate = FUNITY;
	yrate = FUNITY;
	Resolution = (int)lpTH->lResolution;
	}
else
	{
	xstart = 0;
	xend = lpTH->npix - 1;
	ystart = 0;
	yend = lpTH->nlin - 1;
	opix = lpTH->npix;
	olin = lpTH->nlin;
	xrate = FUNITY;
	yrate = FUNITY;
	Resolution = (int)lpTH->lResolution;
	}

if ( xrate == FUNITY )
	lpTH->lpSampleProc = NULL;

// make sure we can setup our type converter if we need it
if ((!FrameTypeInfoEqual(lpTH->inType, lpTH->outType) ||
	(IsDstPTSelected(&lpTH->outType.ptInfo) && lpTH->pXform)))
	{
	fConvert = TRUE;
	if (!TypeConvert.Init(lpTH->inType, lpTH->outType, opix, m_DitherType,
			lpTH->pXform, lpTH->size))
		{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto Exit;
		}
	}

/* Create the image frame store */
if ( !(lpFrame = FrameOpen(lpTH->outType, opix, olin, Resolution)) )
	{
	SetError(BEC_errFrameOpen);
	//FrameError(IDS_EIMAGEOPEN);
	goto Exit;
	}

// if this image contains extra samples, we read the first extra
// sample into an alpha channel for the returned object
if (lpTH->ExtraSamples && lpTH->lpExpandProc && !m_fCrop)
	{
	if ( !(lpAlphaFrame = FrameOpen(FDT_GRAYSCALE, opix, olin, Resolution)) )
		{
		SetError(BEC_errFrameOpen);
		//FrameError(IDS_EIMAGEOPEN);
		goto Exit;
		}
	}

if (lpTH->bCompressed)
	{
	if (!(lpLZW = DecompressLZW (pTheFile, NULL, NULL, 0, NULL))) /* Initialize */
		{
		SetError(BEC_errCompress);
		//Message (IDS_ECOMPRESS);
		goto Exit;
		}
	compressInit = YES;
	}

if ( lpTH->lpSampleProc || m_fCrop || fConvert )
	lpTH->bOKtoPreload = NO;
#ifndef WIN32
// if using cache don't preload
if ( lpFrame->CacheFile != FILE_HANDLE_INVALID )
	lpTH->bOKtoPreload = NO;
#endif

// Loop on all planes and lines
// Read, expand, sample, convert, and stuff the frame
for ( p=0; p<lpTH->InPlanes; p++ )
	{
	yoffset = FDIV2(yrate);
	ylast = -1;

	if ( !lpTH->bOKtoPreload )
		y = 0;
	else
		{ // if all the right circumstances...
		y = FramePreload( lpFrame, lpTH->bNegate, (DWORD)pTheFile, TiffReadBytes, (DWORD)this, TiffProgress );
		if (y == -1)
			goto BadRead;
		else
		if (y == -2)
			goto BadWrite;
		else
		if (y == -3)
			goto Exit;
		}

	yin = y;
	for ( ; y<olin; y++ )
		{
		if (Progress( y + (olin*p), olin*lpTH->InPlanes, YES ) )
			goto Exit;

		// get src line corresponding to this dst line
		yline = ystart + WHOLE(yoffset);
		yoffset += yrate;
		
		// how many lines to read to get there?
		linread = yline - ylast;
		if (linread <= 0)
			continue; // should never happen
		
		// skip to desired line
		for (i=0; i<linread; i++)
		{
			bSkip = i < linread-1;
			if (lpTH->bCompressed)
				{
				if ( !( DecompressLZW( pTheFile, lpTH, lpTH->lpFileLine, lpTH->FileBPL, lpLZW ) ) )
					goto BadRead;
				if ( lpTH->lpPredictorProc && !bSkip )
					(*lpTH->lpPredictorProc)( (LPTR)lpTH->lpFileLine, (int)lpTH->FileBPL, (int)lpTH->SamplesPerPixel );
				}
			else
				{
				if ( image_read( lpTH, lpTH->lpFileLine, lpTH->FileBPL, pTheFile, !bSkip ) != lpTH->FileBPL)
					goto BadRead;
				}
		}
		ylast = yline;

		if ( lpTH->lpExpandProc ) // Expand in place
			(*lpTH->lpExpandProc)( lpTH, lpTH->lpFileLine, lpTH->FileBPL, lpTH->lpAlphaLine );

		// see if we are creating an alpha frame
		if (lpAlphaFrame)
			{
			// crop data in place - just get crop start position
			lpCropLine = lpTH->lpAlphaLine + xstart;

			if ( lpTH->lpSampleProc ) // Sample in place
				Sample_8( lpCropLine, 0, lpCropLine, 0, opix, xrate );

			// After crop or sample, lpTH->FileBPL will not reflect the byte count
			// use opix 
			if ( !(lpOut = FramePointerRaw( lpAlphaFrame, 0, y, YES)) )
				goto BadWrite;
			copy( lpCropLine, lpOut, opix);
			}

		// After expansion lpTH->FileBPL will not reflect the byte count
		// use lpTH->npix * lpTH->InDepth

		// crop data in place - just get crop start position
		lpCropLine = lpTH->lpFileLine + REALSIZE(xstart, lpTH->InDepth);

		if ( lpTH->lpSampleProc ) // Sample in place
			(*lpTH->lpSampleProc)( lpCropLine, 0, lpCropLine, 0, opix, xrate );

		// After crop or sample, lpTH->FileBPL will not reflect the byte count
		// use opix * lpTH->InDepth

		if ( !(lpOut = FramePointerRaw( lpFrame, 0, y, YES)) )
			goto BadWrite;

		if (fConvert)
			{
			TypeConvert.ConvertData(lpCropLine, lpTH->lpConvertLine, yline, opix);
			lpIn = lpTH->lpConvertLine;
			}
		else
			lpIn = lpCropLine;

		// After depth conversion the byte count at lpIn is opix * lpTH->OutDepth
		if ( lpTH->bNegate ) // Negate in place
			negate( lpIn, REALSIZE(opix,lpTH->OutDepth));

		if ( lpTH->InPlanes != 1 )
			copyn( lpIn, lpOut+p, opix, lpTH->InPlanes );
		else
			copy( lpIn, lpOut, REALSIZE(opix,lpTH->OutDepth));
		}

	skip_to_strip(lpTH);
	}

m_iWidth = lpTH->npix;
m_iHeight = lpTH->nlin;
m_iRes = (int)lpTH->lResolution;

m_ObjList.lpHead = m_ObjList.lpTail = NULL;
if ( lpAlphaFrame )
{
	RECT rMask;
	FRMDATATYPE DataType = FDT_GRAYSCALE;

    if ((FrameType(lpFrame) == FDT_LINEART) && m_fOneBitMask)
    	DataType = FDT_LINEART;

    if( !( lpAlpha = MaskCreate( lpAlphaFrame,
								FrameXSize( lpAlphaFrame ),
				   		        FrameYSize( lpAlphaFrame ),
				   		        OFF,
								m_fNoUndo,
								DataType )))
    {
        FrameClose( lpAlphaFrame );
        lpAlphaFrame = NULL;
    }
    else
    if( !MaskRectUpdate( lpAlpha, &rMask ))
    {
        MaskClose( lpAlpha );
        lpAlpha = NULL;
		lpAlphaFrame = NULL;
    }
	lpObject = ObjCreateFromFrame( ST_PERMANENT, lpFrame, lpAlpha,
                               	NULL, m_fNoUndo );
	if( !lpObject )
	{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto Exit;
	}
	ObjAddTail( &m_ObjList, lpObject );
}
else
{
	m_lpFrame = lpFrame;
}

fRet = TRUE;

goto Exit;

BadRead:
SetError(BEC_errFileRead);
//if (m_lpFileName)
//	Message (IDS_EREAD, m_lpFileName);
goto Exit;

BadWrite:
SetError(BEC_errFrameRead);
//Message(IDS_EFRAMEREAD);

Exit:

if (compressInit)
	{
	if (!(DecompressLZW (pTheFile, NULL, NULL, 0, lpLZW))) /* Terminate */
		{
		SetError(BEC_errCompress);
		//Message (IDS_ECOMPRESS);
		}
	compressInit = NO;
	}

FreeTiffHeader( lpTH );
CloseFile(pTheFile);
if (!fRet && lpFrame)
	FrameClose(lpFrame);
if (!fRet && lpAlphaFrame)
	FrameClose(lpAlphaFrame);

ProgressEnd();
return( fRet );
}

/************************************************************************/
BOOL CReadBitmap::WriteTiffData( LPFRAME lpSrcFrame, int xSrc, int ySrc)
/************************************************************************/
{
int	         y, p, yin;
BOOL            fRet = FALSE;
LPTR           lpSrc, lpDst;
BOOL           compressInit, bSkip, fConvert;
int            opix, olin, xstart, xend, ystart, yend;
FRMTYPEINFO	DataType;
FRMTYPEINFO	inType;
CFrameTypeConvert TypeConvert;
LPTIFFHEADER   lpTH;
LPLZW_STUFF    lpLZW;
CFile			theFile;
CFile			*pTheFile;

compressInit = NO;

ProgressBegin(1);

if (m_pFile)
	pTheFile = m_pFile;
else
	{
	if (!theFile.Open(m_lpFileName, CFile::modeReadWrite))
		{
		SetError(BEC_errOpen);
		//Message(IDS_EOPEN, m_lpFileName);
		ProgressEnd();
		return(FALSE);
		}
	pTheFile = &theFile;
	}

FrameGetTypeInfo(lpSrcFrame, &DataType);
FrameSetTypeInfo(&inType, FDT_NONE);

if ( !(lpTH = ReadTiffHeader(pTheFile)) )
	goto BadRead;

if (lpTH->bCompressed)
	{
	SetError(BEC_errCompressFastBits);
	//Message(IDS_COMPRESSFASTBITS);
	goto BadTiff;
	}

opix = FrameXSize(lpSrcFrame);
olin = FrameYSize(lpSrcFrame);
xstart = bound( xSrc, 0, lpTH->npix-1 );
xend = bound( xstart+opix-1, 0, lpTH->npix-1 );
ystart = bound( ySrc, 0, lpTH->nlin-1 );
yend = bound( ystart+olin-1, 0, lpTH->nlin-1 );

if (lpTH->bCompressed)
	{
	if (!(lpLZW = DecompressLZW (pTheFile, NULL, NULL, 0, NULL))) /* Initialize */
		{
		SetError(BEC_errCompress);
		//Message (IDS_ECOMPRESS);
		goto BadTiff;
		}
	compressInit = YES;
	}

// make sure we can setup our type converter if we need it
fConvert = (DataType.DataType != lpTH->inType.DataType) ||
		!FrameColorMapsEqual(DataType.ColorMap, lpTH->inType.ColorMap);
if (fConvert)
	{
	if (!TypeConvert.Init(DataType, lpTH->inType, opix, DT_DEFAULT))
		{
		SetError(BEC_errMemory);
		//Message(IDS_EMEMALLOC);
		goto BadTiff;
		}
	}

// Loop on all planes and lines
// Read, expand, sample, convert, and stuff the frame
for ( p=0; p<lpTH->InPlanes; p++ )
	{
	yin = 0;
	for (y = 0; y < lpTH->nlin; y++ )
		{
		Progress( y + (lpTH->nlin*p), lpTH->nlin*lpTH->InPlanes, NO );

		bSkip = ( y < ystart || y > yend );

		if (lpTH->bCompressed)
			{
			if ( !( DecompressLZW( pTheFile, lpTH, lpTH->lpFileLine, lpTH->FileBPL, lpLZW ) ) )
				goto BadRead;
			}
		else
			{
			if ( image_read( lpTH, lpTH->lpFileLine, lpTH->FileBPL, pTheFile, NO ) != lpTH->FileBPL)
				goto BadRead;
			}

		if ( bSkip )
			continue;

// maybe we need expansion to combine with our data, plug in later
//		if ( lpTH->lpExpandProc ) // Expand in place
//			(*lpTH->lpExpandProc)( lpTH->lpFileLine, lpTH->FileBPL );

		// After expansion lpTH->FileBPL will not reflect the byte count
		// use lpTH->npix * lpTH->InDepth

		// crop data in place - just get crop start position
		lpDst = lpTH->lpFileLine + REALSIZE(xstart, lpTH->InDepth);

		if (!(lpSrc = FramePointerRaw(lpSrcFrame, 0, yin++, NO)))
				goto BadRead;

		// After depth conversion the byte count at lpIn is opix * lpTH->OutDepth
		if ( fConvert )
			{
			TypeConvert.ConvertData(lpSrc, lpTH->lpConvertLine, y, opix);
			lpSrc = lpTH->lpConvertLine;
			}

		if ( lpTH->InPlanes != 1 )
			copyn( lpSrc, lpDst+p, opix, lpTH->InPlanes );
		else
			copy( lpSrc, lpDst, REALSIZE(opix, lpTH->InDepth) );

		if ( lpTH->bNegate ) // Negate in place
			negate( lpDst, REALSIZE(opix, lpTH->InDepth) );

		if ( !image_write( lpTH, lpDst, xstart, REALSIZE(opix, lpTH->InDepth),
						pTheFile )  )
			goto BadWrite;
		}

	skip_to_strip(lpTH);
	}

fRet = TRUE;
goto Cleanup;

BadRead:
SetError(BEC_errFileRead);
//Message (IDS_EREAD, lpFileName);
goto BadTiff;

BadWrite:
SetError(BEC_errFrameRead);
//FrameError(IDS_EFRAMEREAD);

BadTiff:

Cleanup:

if (compressInit)
	{
	if (!(DecompressLZW (pTheFile, NULL, NULL, 0, lpLZW))) /* Terminate */
		SetError(BEC_errCompress);
		//Message (IDS_ECOMPRESS);
	compressInit = NO;
	}

FreeTiffHeader( lpTH );
TRY
	if (!m_pFile)
		pTheFile->Close();
END_TRY
ProgressEnd();
return( fRet );
}

/************************************************************************/
UINT image_read (LPTIFFHEADER lpTH, LPTR ptr, UINT numBytes, CFile *pFile, BOOL bRead)
/************************************************************************/
{
long strip_start, strip_end, strip_pos;
long bytes_in_strip;
UINT req_bytes, bytes_read, bytes_to_read;
int k;

TRY
	{
	if (!bRead)
		{
		lpTH->ImagePos += numBytes;
		return(numBytes);
		}
	
	if (lpTH->StripsPerImage == 1)
		{
		if (lpTH->ImagePos)
			{
			pFile->Seek (lpTH->ImagePos, CFile::current);
			lpTH->ImagePos = 0;
			}
		bytes_to_read = numBytes;
		bytes_read = pFile->Read(ptr, (UINT)bytes_to_read);
		return(bytes_read);
		}
	else
		{
		req_bytes = numBytes;
		strip_start = 0;
		for (k = 0; k < lpTH->StripsPerImage; k++)
			{
			strip_end = strip_start + lpTH->StripByteCounts[k] - 1;
			if ((lpTH->ImagePos >= strip_start) && (lpTH->ImagePos <= strip_end))
				{
				strip_pos = lpTH->StripOffsets[k] + (lpTH->ImagePos - strip_start);
				pFile->Seek (strip_pos+lpTH->tof, CFile::begin);
				bytes_in_strip = strip_end - lpTH->ImagePos + 1;
				if (bytes_in_strip > (long)req_bytes)
					bytes_to_read = req_bytes;
				else
					bytes_to_read = (UINT)bytes_in_strip;
				bytes_read = pFile->Read (ptr, bytes_to_read);
				lpTH->ImagePos += bytes_read;
				return (bytes_read);
				}
			strip_start = strip_end + 1;
			}
		}
	}
END_TRY

return (0);
}

/************************************************************************/
static BOOL image_write (LPTIFFHEADER lpTH, LPTR lpSrc, int xSrc, UINT numBytes, CFile *pFile)
/************************************************************************/
{
long strip_start, strip_end, strip_pos;
long bytes_in_strip;
UINT bytes_to_write, req_bytes;
int k, bytes_per_pixel;
long cur_pos, lOffset, lImagePos;

TRY
	{
	bytes_per_pixel = lpTH->FileBPL / lpTH->npix;

	// calculate offset for current position to write data
	lOffset = REALSIZE(xSrc,bytes_per_pixel) - (long)lpTH->FileBPL;
	if (lpTH->StripsPerImage == 1)
		{
		if (lpTH->ImagePos)
			lOffset += lpTH->ImagePos;
		// seek to position to write data
		cur_pos = pFile->Seek(0L, CFile::current);
		pFile->Seek(lOffset, CFile::current);
		bytes_to_write = numBytes;
		pFile->Write (lpSrc, bytes_to_write);
		pFile->Seek(cur_pos, CFile::begin);
		return(TRUE);
		}
	else
		{
		lImagePos = lpTH->ImagePos;
		lpTH->ImagePos += lOffset;
		req_bytes = numBytes;
		strip_start = 0;
		for (k = 0; k < lpTH->StripsPerImage; k++)
			{
			strip_end = strip_start + lpTH->StripByteCounts[k] - 1;
			if ((lpTH->ImagePos >= strip_start) && (lpTH->ImagePos <= strip_end))
				{
				strip_pos = lpTH->StripOffsets[k] + (lpTH->ImagePos - strip_start);
				pFile->Seek (strip_pos+lpTH->tof, CFile::begin);
				bytes_in_strip = strip_end - lpTH->ImagePos + 1;
				if (bytes_in_strip > req_bytes)
			 		bytes_to_write = req_bytes;
				else
					bytes_to_write = bytes_in_strip;
				pFile->Write (lpSrc, bytes_to_write);
				lpTH->ImagePos += bytes_to_write;
				return (TRUE);
				}
			strip_start = strip_end + 1;
			}
		lpTH->ImagePos = lImagePos;
		}
	}
END_TRY
return (FALSE);
}

/************************************************************************/
static void skip_to_strip(LPTIFFHEADER lpTH)
/************************************************************************/
{
	long strip_start, strip_end;
	int k;

	if (lpTH->StripsPerImage == 1) 
	{
		return;
	}

	strip_start = 0;
	for (k = 0; k < lpTH->StripsPerImage; k++)
	{
		strip_end = strip_start + lpTH->StripByteCounts[k] - 1;

		if ((lpTH->ImagePos >= strip_start) &&
			(lpTH->ImagePos <= strip_end))
		{
			/* We know the current strip */
			lpTH->ImagePos = strip_end+1;
			break;
		}
		strip_start = strip_end + 1;
	}
}

/************************************************************************/
static void ReadHorzDiff( LPTR lpImage, int iCount, int SamplesPerPixel )
/************************************************************************/
{
int col;

for (col = SamplesPerPixel; col < iCount; col++)
	lpImage[col] += lpImage[col-SamplesPerPixel];
}

/************************************************************************/
void image_read_startstrip(LPTIFFHEADER lpTH)
/************************************************************************/
{
int	k;
long	strip_start;

strip_start = 0;
for (k = 0; (k < lpTH->StripsPerImage) && (lpTH->ImagePos > strip_start); k++)
 	strip_start += lpTH->StripByteCounts[k];
lpTH->ImagePos = strip_start;
}

/* TIFF defines */
#define TIF_BYTE 1
#define TIF_ASCII 2
#define TIF_SHORT 3
#define TIF_LONG 4
#define TIF_RATIONAL 5
#define TIF_MM (0x4D4D)
#define TIF_II (0x4949)
#define MYORDER (TIF_II)
#define TIFF_VERSION 42

/************************************************************************/
static void diff_fixtagval ( LPTAG tagp )
/************************************************************************/
{
long length;

/* this is necessary because tag values fields are left justified */
/* and need to be swapped if a different machine is reading the file */
/* - this is executed both on the way in and on the way out */
if (tagp->type == TIF_LONG)  length = 4;
else
if (tagp->type == TIF_SHORT) length = 2;
else
if (tagp->type == TIF_BYTE)  length = 1;
else
if (tagp->type == TIF_ASCII) length = 1;
length *= tagp->length;

if (length == 1) /* will this work for read and write? */
	tagp->value = * ((LPTR) (& (tagp->value)));
else
if (length == 2)
	{
	swapw ((LPWORD)&tagp->value);
	tagp->value = * ((LPWORD) (& (tagp->value)));
	}
else
//if (length > 2)
	swapl ((LPDWORD)&tagp->value);
}

/************************************************************************/
static void rightjust_tagval ( LPTAG tagp )
/************************************************************************/
{
long length;

/* this is necessary because tag values fields are left justified */
/* in the file, but are converted to longs when read in */
if (tagp->type == TIF_LONG)  length = 4;
else
if (tagp->type == TIF_SHORT) length = 2;
else
if (tagp->type == TIF_BYTE)  length = 1;
else
if (tagp->type == TIF_ASCII) length = 1;
length *= tagp->length;

if (length == 2)
	 tagp->value = (DWORD)(* ((LPWORD) (& (tagp->value))));
else
if (length == 1)
	 tagp->value = (DWORD)(* ((LPTR) (& (tagp->value))));
}

/************************************************************************/
static int flexiread (CFile *pFile, LPTR ptr, REG int size, short order)
/************************************************************************/
{
REG int	rc;
LPTAG tagp;

TRY
	{
	if ((rc = pFile->Read (ptr, size)) != size)
		return (rc);
	}
CATCH(CFileException, e)
	{
	return(0);
	}
END_CATCH

if (MYORDER == order)
	{
	if (size == sizeof (TAG))
		rightjust_tagval ((LPTAG)ptr);
	}
else
	{
	if (size == sizeof(short))
		swapw ((LPWORD)ptr);
	if (size == sizeof(long))
		swapl ((LPDWORD)ptr);
	if (size == sizeof (TAG))
		{
		tagp = (LPTAG)ptr;
		swapw ((LPWORD)& (tagp->tagno));
		swapw ((LPWORD)& (tagp->type));
		swapl ((LPDWORD)& (tagp->length));
		diff_fixtagval (tagp);
		}
	}
return (rc);
}

/************************************************************************/
LPTIFFHEADER CReadBitmap::ReadTiffHeader(CFile *pTheFile)
/************************************************************************/
{
INT32 lSavePos, offset;
INT32 ifd_ptr, lTemp, xresden, xresnum, yresden, yresnum;
INT16 holder, sTemp;
UINT8 cTemp;
int	npix, nlin, photomet, i, k, iMaxValue, samples, predictor;
int inkset, dotrange0, dotrange1, bpp, iColormapEntries, iExpandFactor;
TAG	tag;
LPRGB lpRGB;
LPWORD lpRed, lpGreen, lpBlue, lpWord, lpLineBuffer = NULL;
BOOL bCompressed, bMustShiftDown, bOKtoPreload, bPlanar;
long StripsPerImage, lColormap;
LPLONG StripByteCounts, StripOffsets;
LPTIFFHEADER lpHeader;
short order;
LPCOLORMAP lpColorMap;
long 		lValueBPP, lLengthBPP;
BOOL 		fRet = FALSE, fTransform = FALSE;
CFile 	theFile;
BOOL		GotNewSub, fOpened = FALSE;

if (!(lpHeader = (LPTIFFHEADER)Alloc((long)sizeof(TIFFHEADER))))
	{
	SetError(BEC_errMemory);
	//Message(IDS_EMEMALLOC);
	return(NULL);
	}

// Clear out all the header data
clr( (LPTR)lpHeader, sizeof(TIFFHEADER) );

if (!pTheFile)
	{
	if ((pTheFile = OpenFile()) == NULL)
		{
	 	FreeUp((LPTR)lpHeader);
	 	return( NULL );
		}
	fOpened = TRUE;
	}

/* assumed to be full res data, tag 0xff value = 1 */

/* default values */
lpHeader->ExtraSamples = 0;
bpp = 1; // Lineart
photomet = 0; // min value is white
xresnum = 0;
xresden = 1;
yresnum = 0;
yresden = 1;
lColormap = 0;
iColormapEntries = 0;
iMaxValue = 255;
lpHeader->SamplesPerPixel = 1;
bPlanar = NO;
bCompressed = NO;
predictor = 1;
bOKtoPreload = YES;
StripsPerImage = 1;
StripByteCounts = NULL;
StripOffsets = NULL;
lpColorMap = NULL;
lValueBPP = 0;
/* end default values */

// save top-of-file
TRY
	{
	lpHeader->tof = pTheFile->GetPosition();

	if (pTheFile->Read ((LPTR)&order, 2) != 2) /* read byte order */
		goto BadRead;
	if (order != TIF_II && order != TIF_MM)
		{
		SetError(BEC_errBadTIFF);
		//Message (IDS_EIMGBAD, m_lpFileName);
		goto Exit;
		}
	if (flexiread (pTheFile, (LPTR)&holder, 2, order) != 2) /* read tiff version number */
		goto BadRead;
	if (holder != 0x2a) /* only version 42 allowed for now */
		{
		SetError(BEC_errTIFFVersion);
		//Message (IDS_EIMGVERSION, m_lpFileName);
		}
	if (flexiread (pTheFile, (LPTR)&ifd_ptr, 4, order) != 4) /* read ifd pointer */
		goto BadRead;

	/****************************************************************/

	next_ifd:
	pTheFile->Seek(ifd_ptr+lpHeader->tof, CFile::begin); /* seek to location of ifd */
	if (flexiread (pTheFile, (LPTR)&holder, 2, order) != 2) /* read # of tag field entries */
		goto BadRead;
	GotNewSub = FALSE;
	for (i=0; i<holder; i++)
		{
		if (flexiread (pTheFile, (LPTR)&tag, 12, order) != 12) /* read the tag entry */
			goto BadRead;
		switch (tag.tagno)
			{
			case 0xfe: // new sub-file type */
				GotNewSub = TRUE;
				if (tag.value) /* want only full res data */
					{
               for (i=0; i < holder; ++i)
                  {
					   if (flexiread (pTheFile, (LPTR)&tag, 12, order) != 12)
						   goto BadRead;
                  }
					if (flexiread (pTheFile, (LPTR)&ifd_ptr, 4, order) != 4)
						goto BadRead;
					if (ifd_ptr == 0)
						goto Exit;
					goto next_ifd;
					}
				break;
			case 0xff: /* sub-file type */
				if (!GotNewSub && tag.value != 1) /* want only full res data */
					{
               for (i=0; i < holder; ++i)
                  {
					   if (flexiread (pTheFile, (LPTR)&tag, 12, order) != 12)
						   goto BadRead;
                  }
					if (flexiread (pTheFile, (LPTR)&ifd_ptr, 4, order) != 4)
						goto BadRead;
					if (ifd_ptr == 0)
						goto Exit;
					goto next_ifd;
					}
				break;
			case 0x100: /* image width */
				npix = tag.value;
				break;
			case 0x101: /* image height */
				nlin = tag.value;
				break;
			case 0x102: /* bits per sample */
				if (tag.length <= 2)
					bpp = LOWORD(tag.value);
				else
					{
					lLengthBPP = tag.length;
					// the value is a file pointer
					lValueBPP = tag.value;
					}
				break;
			case 0x103: /* compression */
				if (tag.value == 5)
					{
					bCompressed = YES;
					bOKtoPreload = NO;
					}
				else
				if (tag.value == 1)
					bCompressed = NO;
				else /* other bCompressed images, we don't handle */
					{
					SetError(BEC_errUnsupportedTIFFCompress);
					//Message (IDS_EIMGCOMP, m_lpFileName);
					goto Exit;
					}
				break;
			case 0x106: /* photometeric interpretation */
				if ((tag.value != 0) && (tag.value != 1) &&
					(tag.value != 2) && (tag.value != 3) &&
					(tag.value != 5))
					{
					SetError(BEC_errUnsupportedTIFFPhoto);
					//Message (IDS_EIMGPHOTO, m_lpFileName);
					}
				photomet = tag.value;
				break;
			case 0x10a: /* fill order */
				if (tag.value != 1) /* not default, try to continue */
					{
					SetError(BEC_errUnsupportedTIFFFill);
					//Message (IDS_EIMGFILL, m_lpFileName);
					bOKtoPreload = NO;
					}
				break;
			case 0x111: /* strip offsets */
				StripsPerImage = tag.length;
				if (tag.length == 1)
					{
					StripOffsets = (long FAR *)
						Alloc (tag.length * sizeof(long));
					if (!StripOffsets)
						{
						SetError(BEC_errMemory);
						//Message (IDS_EMEMALLOC);
						goto BadRead;
						}
						StripOffsets[0] = tag.value;
					}
				else
					{
					StripOffsets = (long FAR *)
						Alloc (tag.length * sizeof(long));
					if (!StripOffsets)
						{
						SetError(BEC_errMemory);
						//Message (IDS_EMEMALLOC);
						goto BadRead;
						}
					/* save the current file position */
					lSavePos = pTheFile->GetPosition();
					/* find the first offset */
					/* seek to line loc */
					pTheFile->Seek (tag.value+lpHeader->tof, CFile::begin);
					/* read the first line pointer */
					for (k = 0; k < tag.length; ++k)
						{
						if (tag.type == TIF_LONG)
							{
							if (flexiread (pTheFile, (LPTR)&lTemp, 4, order) == 0)
								goto BadRead;
							}
						else
							{
							if (flexiread (pTheFile, (LPTR)&sTemp, 2, order) == 0)
								goto BadRead;
							lTemp = sTemp;
							}
						StripOffsets[k] = lTemp;
						}
					/* seek to where we were */
					pTheFile->Seek (lSavePos, CFile::begin);
					}
				break;
			case 0x112: /* orientation */
				if (tag.value != 1) /* not default, try to continue */
					{
					SetError(BEC_errUnsupportedTIFFOrient);
					//Message (IDS_EIMGORIENT, m_lpFileName);
					bOKtoPreload = NO;
					}
				break;
			case 0x115: /* samples per pixel */
				lpHeader->SamplesPerPixel = tag.value;
				break;
			case 0x116: /* rows per strip */
				break;
			case 0x117:
				if (tag.length == 1)
					{
					/* save the pos of byte counts */
					StripByteCounts = (LPLONG)Alloc (tag.length * sizeof(long));
					if (!StripByteCounts)
						{
						SetError(BEC_errMemory);
						//Message (IDS_EMEMALLOC);
						goto BadRead;
						}
					StripByteCounts[0] = tag.value;
					}
				else
					{
					StripByteCounts = (LPLONG)Alloc (tag.length * sizeof(long));
					if (!StripByteCounts)
						{
						SetError(BEC_errMemory);
						//Message (IDS_EMEMALLOC);
						goto BadRead;
						}
					/* save the current file position */
					lSavePos = pTheFile->GetPosition();
					/* find the first offset */
					/* seek to line loc */
					pTheFile->Seek (tag.value+lpHeader->tof, CFile::begin);
					/* read the first line pointer */
					for (k = 0; k < tag.length; ++k)
						{
						if (tag.type == TIF_LONG)
							{
							if (flexiread (pTheFile, (LPTR)&lTemp, 4, order) == 0)
								goto BadRead;
							}
						else
							{
							if (flexiread (pTheFile, (LPTR)&sTemp, 2, order) == 0)
								goto BadRead;
							lTemp = sTemp;
							}
						StripByteCounts[k] = lTemp;
						}
					/* seek to where we were */
					pTheFile->Seek (lSavePos, CFile::begin);
					}
					break;
			case 0x118: /* min sample value */
				break;
			case 0x119: /* max sample value */
				iMaxValue = tag.value;
				break;
			case 0x11a: /* xresolution */
				xresnum = tag.value;
				break;
			case 0x11b: /* yresolution */
				yresnum = tag.value;
				break;
			case 0x11c: /* planar configuration */
				if ( bPlanar = (tag.value == 2) )
					bOKtoPreload = NO;
				break;
			case 0x13d: /* predictor */
				predictor = tag.value;
				break;
			case 0x12d: /* color response curves */
			case 0x140: /* color map */
				lColormap = tag.value;
				iColormapEntries = tag.length/3;
				if ( iColormapEntries > 256 )
					iColormapEntries = 256;
				break;
			case 0x14C: /* InkSet */
				inkset = tag.value;
				break;
			case 0x150: /* DotRange */
				/* save the current file position */
				lSavePos = pTheFile->GetPosition();
				/* find the first offset */
				/* seek to line loc */
				pTheFile->Seek (tag.value+lpHeader->tof, CFile::begin);
				if (tag.type == TIF_SHORT)
					{
					unsigned short sTemp;

					flexiread(pTheFile, (LPTR)&sTemp, 2, order);
					dotrange0 = sTemp;
					flexiread(pTheFile, (LPTR)&sTemp, 2, order);
					dotrange1 = sTemp;
					}
				else
					{
					flexiread(pTheFile, (LPTR)&cTemp, 1, order);
					dotrange0 = cTemp;
					flexiread(pTheFile, (LPTR)&cTemp, 1, order);
					dotrange1 = cTemp;
					}
				/* seek to where we were */
				pTheFile->Seek (lSavePos, CFile::begin);
				break;
         	case 0x152:   /* ExtraSamples */
            	lpHeader->ExtraSamples = tag.length;
				bOKtoPreload = NO;
				break;
			case 34391: /* Precision Transform */

				if( !tag.length )
					break;

				fTransform = TRUE;

				/* save the current file position */
				lSavePos = pTheFile->GetPosition();
				/* find the first offset */
				/* seek to line loc */
				pTheFile->Seek (tag.value+lpHeader->tof, CFile::begin);
				
				if( !( lpHeader->pXform = Alloc( tag.length ) ) )
					{
					SetError(BEC_errMemory);
					//Message( IDS_EMEMALLOC );
					goto Exit;
					}
				lpHeader->size = tag.length;

				pTheFile->ReadHuge( lpHeader->pXform, tag.length );

				/* seek to where we were */
				pTheFile->Seek (lSavePos, CFile::begin);
				break;

			case 34392: /* Precision Transform */

				if( !tag.length || !fTransform )
					break;

				lSavePos = pTheFile->GetPosition();
				pTheFile->Seek (tag.value+lpHeader->tof, CFile::begin);
				pTheFile->Read (lpHeader->szXformName, tag.length);
				pTheFile->Seek (lSavePos, CFile::begin);
				break;
				
			default:
				break;
			}
		}

	// adjust samples to not include extra samples
	samples = lpHeader->SamplesPerPixel - lpHeader->ExtraSamples;

	if (lValueBPP)
		{
		/* save the current file position */
		lSavePos = pTheFile->GetPosition();

		/* find the first offset */
		/* seek to line loc */
		pTheFile->Seek (lValueBPP+lpHeader->tof, CFile::begin);

		bpp = 0;
		i = (int)lLengthBPP;
		if (i >= samples)
			i = samples;

		/* read the first value */
		for (k = 0; k < i; ++k)
			{
			if (flexiread (pTheFile, (LPTR)&sTemp, 2, order) == 0)
				goto BadRead;
			bpp += sTemp;
			}
		/* seek to where we were */
		pTheFile->Seek (lSavePos, CFile::begin);
		}
	// Bound bpp
	if ( bpp > 24 )
		bpp = 32;
	else
	if ( bpp > 8 )
		bpp = 24;

	if ( !StripOffsets )
		goto Exit;

	/* if byte counts tag is missing, fake it */
	if ( !StripByteCounts )
		{
		if ( !(StripByteCounts = (LPLONG)Alloc( StripsPerImage * sizeof(long) )) )
			{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC);
			goto BadRead;
			}
		for (k = 0; k < StripsPerImage-1; k++)
			{
			StripByteCounts[k] =
				StripOffsets[k+1] - StripOffsets[k];
			if (StripByteCounts[k] < 1)
				goto Exit;
			}
		StripByteCounts[k] = 1L << 30;
		}

	/* if preload is still ok, check for multiple strips in order */
	if (bOKtoPreload)
		{
		if (StripsPerImage > 1)
			{
			offset = StripOffsets[0];
			for (k = 1; (k < StripsPerImage) && bOKtoPreload; k++)
				{
				offset += StripByteCounts[k-1];
				if (offset != StripOffsets[k])
					bOKtoPreload = NO;
				}
			}
		}

	if (xresnum)
		{
		pTheFile->Seek (xresnum+lpHeader->tof, CFile::begin); /* seek to loc of xres */
		if (flexiread (pTheFile, (LPTR)&xresnum, 4, order) != 4) /* read xres numerator */
			goto BadRead;
		if (flexiread (pTheFile, (LPTR)&xresden, 4, order) != 4) /* read xres denominator */
			goto BadRead;
		xresnum /= xresden;
		}
	else
		xresnum = 75;

	if (yresnum)
		{
		pTheFile->Seek (yresnum+lpHeader->tof, CFile::begin); /* seek to loc of yres */
		if (flexiread (pTheFile, (LPTR)&yresnum, 4, order) != 4) /* read yres numerator */
			goto BadRead;
		if (flexiread (pTheFile, (LPTR)&yresden, 4, order) != 4) /* read yres denominator */
			goto BadRead;
		yresnum /= yresden;
		}
	else
		yresnum = 75;

	if ( photomet != 3 ) // turn off the color map if its not palette color
		lColormap = 0;

	if (lColormap)
		{
		pTheFile->Seek (lpHeader->tof+lColormap, CFile::begin); /* seek to loc of lColormap */
   		if( !( lpLineBuffer = (LPWORD)Alloc( 2*3*iColormapEntries ) ) )
      		goto BadRead;
		if (pTheFile->Read (lpLineBuffer, (2*3*(iColormapEntries))) != 2*3*(iColormapEntries))
			goto BadRead;
		if (MYORDER != order)
			{
			lpWord = lpLineBuffer;
			for (i=0; i<3*(iColormapEntries); i++, lpWord++)
				swapw (lpWord);
			}

		lpWord = lpLineBuffer;
		bMustShiftDown = NO;
		for (i=0; i<3*iColormapEntries; i++)
			{ // See if the pallete needs to be shifted down
			if (*lpWord++ > 255)
				{
				bMustShiftDown = YES;
				break;
				}
			}

		lpColorMap = FrameCreateColorMap();
		if (!lpColorMap)
			{
			SetError(BEC_errMemory);
			//Message (IDS_EMEMALLOC);
			goto BadRead;
			}
		lpColorMap->NumEntries = iColormapEntries;
		lpRGB = lpColorMap->RGBData;
		lpRed = lpLineBuffer;
		lpGreen = lpRed + iColormapEntries;
		lpBlue = lpGreen + iColormapEntries;
		for (i=0; i<iColormapEntries; i++)
			{
			if (bMustShiftDown)
				{
				lpRGB->red   = (BYTE)(*lpRed++ >> 8);
				lpRGB->green = (BYTE)(*lpGreen++ >> 8);
				lpRGB->blue  = (BYTE)(*lpBlue++ >> 8);
				}
			else
				{
				lpRGB->red   = (BYTE)(*lpRed++);
				lpRGB->green = (BYTE)(*lpGreen++);
				lpRGB->blue  = (BYTE)(*lpBlue++);
				}
			lpRGB++;
			}
		}

	// Seek to beginning of the data
	pTheFile->Seek (StripOffsets[0]+lpHeader->tof, CFile::begin);

	if ( bpp == 8 && iMaxValue == 63 )
		bpp = 6;

	if ( samples == 3 && bpp != 3 )
		bpp = 24;

	if ( samples == 4 )
		bpp = 32;

	if (lpHeader->ExtraSamples && bpp != 8 && bpp != 24 && bpp != 32)
		{
		SetError(BEC_errTIFFSamples);
		//Message(IDS_ESAMPLES, m_lpFileName);
		goto Exit;
		}

	m_BitsPerPixel = bpp;
	switch ( bpp )
		{
		case 1:
			FrameSetTypeInfo(&lpHeader->inType, FDT_LINEART);
			iExpandFactor = 8; // Only because the Sample_1 unpacks in place
			lpHeader->lpExpandProc = NULL;
			lpHeader->lpSampleProc = Sample_1; // Happens after expansion
			lpHeader->InDepth = 0; // Depth after expansion routine
			lpHeader->lpPredictorProc = NULL; // Happens before expansion
			lpHeader->InPlanes = 1;
			lpHeader->FileBPL  = (npix+7)/8;
			break;
		case 4:
			if ( lColormap )
				{ // Palette color
				FrameSetTypeInfo(&lpHeader->inType, FDT_PALETTECOLOR, lpColorMap);
				iExpandFactor = 2;
				lpHeader->lpExpandProc = Move4to8bits;
				lpHeader->lpSampleProc = Sample_8; // Happens after expansion
				lpHeader->InDepth = 1; // Depth after expansion routine
				}
			else
				{
				FrameSetTypeInfo(&lpHeader->inType, FDT_GRAYSCALE);
				iExpandFactor = 2;
				lpHeader->lpExpandProc = Expand4to8bits;
				lpHeader->lpSampleProc = Sample_8; // Happens after expansion
				lpHeader->InDepth = 1; // Depth after expansion routine
				}
			lpHeader->lpPredictorProc = NULL; // Happens before expansion
			lpHeader->InPlanes = 1;
			lpHeader->FileBPL = (npix+1)/2;
			bOKtoPreload = NO;
			break;
		case 6:
			bOKtoPreload = NO;
			iExpandFactor = 1;
			lpHeader->lpExpandProc = Expand6to8bits;
		case 8:
			if ( lColormap ) // Palette color
				{
				FrameSetTypeInfo(&lpHeader->inType, FDT_PALETTECOLOR, lpColorMap);
				if (FALSE)
					{
					iExpandFactor = 3;
					lpHeader->lpExpandProc = Expand8to24bits;
					lpHeader->lpSampleProc = Sample_24; // Happens after expansion
					lpHeader->InDepth = 3; // Depth after expansion routine
					bOKtoPreload = NO;
					}
				else
					{
					iExpandFactor = 1;
         			if (lpHeader->ExtraSamples)
            			lpHeader->lpExpandProc = Shrinkto8bits;
         			else
			   			lpHeader->lpExpandProc = NULL;
					lpHeader->lpSampleProc = Sample_8; // Happens after expansion
					lpHeader->InDepth = 1; // Depth after expansion routine
					}
				}
			else
				{
				FrameSetTypeInfo(&lpHeader->inType, FDT_GRAYSCALE);
				iExpandFactor = 1;
         		if (lpHeader->ExtraSamples)
            		lpHeader->lpExpandProc = Shrinkto8bits;
         		else
			   		lpHeader->lpExpandProc = NULL;
				lpHeader->lpSampleProc = Sample_8; // Happens after expansion
				lpHeader->InDepth = 1; // Depth after expansion routine
				}
			lpHeader->lpPredictorProc = ReadHorzDiff; // Happens before expansion
			lpHeader->InPlanes = 1;
			lpHeader->FileBPL = ( bPlanar ? npix : lpHeader->SamplesPerPixel * npix );
			break;
		case 24:
			FrameSetTypeInfo(&lpHeader->inType, FDT_RGBCOLOR );
			iExpandFactor = 1;
         	if (lpHeader->ExtraSamples)
            	lpHeader->lpExpandProc = Shrinkto24bits;
         	else
			   	lpHeader->lpExpandProc = NULL;
			lpHeader->lpSampleProc = Sample_24; // Happens after expansion
			lpHeader->InDepth = 3; // Depth after expansion routine
			lpHeader->lpPredictorProc = ReadHorzDiff; // Happens before expansion
			lpHeader->InPlanes = ( bPlanar ? 3 : 1 );
			lpHeader->FileBPL = ( bPlanar ? npix : lpHeader->SamplesPerPixel * npix );
			break;
		case 32:
			FrameSetTypeInfo(&lpHeader->inType, FDT_CMYKCOLOR );
			iExpandFactor = 1;
         	if (lpHeader->ExtraSamples)
            	lpHeader->lpExpandProc = Shrinkto32bits;
         	else
			   	lpHeader->lpExpandProc = NULL;
			lpHeader->lpSampleProc = Sample_32; // Happens after expansion
			lpHeader->InDepth = 4; // Depth after expansion routine
			lpHeader->lpPredictorProc = ReadHorzDiff; // Happens before expansion
			lpHeader->InPlanes = ( bPlanar ? 4 : 1 );
			lpHeader->FileBPL = ( bPlanar ? npix : lpHeader->SamplesPerPixel * npix );
			break;
		default:
			goto Exit;
		}

	if (!SetupTypes(&lpHeader->inType, &lpHeader->outType, FALSE, lpHeader->pXform))
		goto Exit;

	if ( !FrameTypeInfoEqual( lpHeader->inType, lpHeader->outType ) )
		bOKtoPreload = NO;

	switch (lpHeader->outType.DataType)
		{
		case FDT_LINEART:
			lpHeader->OutDepth = 0;
			break;
		case FDT_GRAYSCALE:
			lpHeader->OutDepth = 1;
			break;
		case FDT_PALETTECOLOR:
			lpHeader->OutDepth = 1;
			break;
		case FDT_RGBCOLOR:
			lpHeader->OutDepth = 3;
			break;
		case FDT_CMYKCOLOR:
			lpHeader->OutDepth = 4;
			break;
		}

	// Setup the basic file information
	lpHeader->lResolution = xresnum;
	lpHeader->npix = npix;
	lpHeader->nlin = nlin;
	lpHeader->bOKtoPreload = bOKtoPreload;
	lpHeader->bNegate = (photomet == 0); // min value is white
	lpHeader->bCompressed = bCompressed;
	lpHeader->bColormap = lColormap;

	// Setup the strip information
	lpHeader->StripsPerImage = StripsPerImage;
	lpHeader->StripByteCounts = StripByteCounts;
	lpHeader->StripOffsets = StripOffsets;

	// Allocate a single input, expansion and sampling buffer
	if ( !AllocLines( &lpHeader->lpFileLine, 1, lpHeader->FileBPL, iExpandFactor ) )
		{ // Provide for worst case expansion in place
		SetError(BEC_errMemory);
		//Message (IDS_EMEMALLOC);
		goto Exit;
		}

	// Allocate a depth conversion buffer
	if ( !AllocLines( &lpHeader->lpConvertLine, 1, npix, lpHeader->OutDepth ? lpHeader->OutDepth : 1 ) )
		{
		SetError(BEC_errMemory);
		//Message (IDS_EMEMALLOC);
		goto BadRead;
		}

	// Allocate a depth conversion buffer
	if ( !AllocLines( &lpHeader->lpAlphaLine, 1, npix, 1 ) )
		{
		SetError(BEC_errMemory);
		//Message (IDS_EMEMALLOC);
		goto BadRead;
		}

	// Zap the LZW compression predictor proc, if necessary
	if ( !bCompressed || predictor != 2 )
		lpHeader->lpPredictorProc = NULL; // Happens before expansion

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
//if (m_lpFileName)
//	Message (IDS_EREAD, m_lpFileName);

Exit:
if( lpLineBuffer )
   	FreeUp( lpLineBuffer );
if ( fOpened )
	CloseFile(pTheFile);
if (fRet)
	return(lpHeader);
else
	{
	FreeTiffHeader(lpHeader);
	return( NULL );
	}
}


/************************************************************************/
void CReadBitmap::FreeTiffHeader( LPTIFFHEADER lpHeader )
/************************************************************************/
{
if (!lpHeader)
	return;
if ( lpHeader->lpFileLine )
	FreeUp( lpHeader->lpFileLine );
if ( lpHeader->lpConvertLine )
	FreeUp( lpHeader->lpConvertLine );
if ( lpHeader->lpAlphaLine )
	FreeUp( lpHeader->lpAlphaLine );
if ( lpHeader->StripOffsets )
	FreeUp( (LPTR)lpHeader->StripOffsets );
if ( lpHeader->StripByteCounts )
	FreeUp( (LPTR)lpHeader->StripByteCounts );
if ( lpHeader->inType.ColorMap )
	FrameDestroyColorMap(lpHeader->inType.ColorMap);
if ( lpHeader->pXform )
	FreeUp(lpHeader->pXform);
FreeUp((LPTR)lpHeader);
//clr( (LPTR)lpHeader, sizeof(TIFFHEADER) );
}

/************************************************************************/
LOCAL DWORD TiffReadBytes(DWORD dwData, LPTR lpDst, DWORD dwBytes)
/************************************************************************/
{
	CFile *pFile;

	pFile = (CFile *)dwData;
	TRY
		{
		return(pFile->Read(lpDst, dwBytes));
		}
	CATCH(CFileException, e)
		{
		return(0);
		}
	END_CATCH
}

/************************************************************************/
static void Expand6to8bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
/************************************************************************/
{
map( ShiftUp2Lut, lpIn, iInByteCount );
}

/************************************************************************/
static void Expand4to24bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
/************************************************************************/
{
LPRGB lpRGBOut;
BYTE b, c;
LPRGB lpRGBmap;

lpRGBmap = lpTH->inType.ColorMap->RGBData;
if (!lpRGBmap)
	return;
lpRGBOut = (LPRGB)lpIn;
lpRGBOut += (2 * iInByteCount - 1);
lpIn += (iInByteCount - 1);
while ( --iInByteCount >= 0 )
	{
	b = *lpIn--;
	c = b & 0xF;
	lpRGBOut->red   = lpRGBmap[ c ].red;
	lpRGBOut->green = lpRGBmap[ c ].green;
	lpRGBOut->blue  = lpRGBmap[ c ].blue;
	lpRGBOut--;
	c = (b >> 4);
	lpRGBOut->red   = lpRGBmap[ c ].red;
	lpRGBOut->green = lpRGBmap[ c ].green;
	lpRGBOut->blue  = lpRGBmap[ c ].blue;
	lpRGBOut--;
	}
}

//static BYTE Nibbles[] = {
//0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 255 };

/************************************************************************/
static void Move4to8bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
						LPTR lpAlphaLine)
// converts 4-bit data to 8 bits but does no scaling.
/************************************************************************/
{
    LPTR lpOut;
    BYTE b;

    lpOut = lpIn;
    lpOut += (2 * iInByteCount - 1);
    lpIn += (iInByteCount - 1);
    while ( --iInByteCount >= 0 )
    {
	    b = *lpIn--;
	    *lpOut-- = (b) & 0x0F;
	    *lpOut-- = (b >> 4) & 0x0F;
    }
}

/************************************************************************/
static void Expand4to8bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
// converts 4-bit data to 8 bits and scales the data.
/************************************************************************/
{
LPTR lpOut;
BYTE b;

lpOut = lpIn;
lpOut += (2 * iInByteCount - 1);
lpIn += (iInByteCount - 1);
while ( --iInByteCount >= 0 )
	{
	b = *lpIn--;
	*lpOut-- = (b << 4) & 0xF0;
	*lpOut-- = (b) & 0xF0;
	}
}


/************************************************************************/
static void Expand8to24bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
/************************************************************************/
{
LPRGB lpRGBOut;
BYTE b;
LPRGB lpRGBmap;

lpRGBmap = lpTH->inType.ColorMap->RGBData;
if (!lpRGBmap)
	return;

lpRGBOut = (LPRGB)lpIn;
lpRGBOut += (iInByteCount - 1);
lpIn += (iInByteCount - 1);
while ( --iInByteCount >= 0 )
	{
	b = *lpIn--;
	lpRGBOut->red   = lpRGBmap[ b ].red;
	lpRGBOut->green = lpRGBmap[ b ].green;
	lpRGBOut->blue  = lpRGBmap[ b ].blue;
	lpRGBOut--;
	}
}


/************************************************************************/
static void Shrinkto8bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
/************************************************************************/
{
LPTR  lpOut;
int offset;

iInByteCount /= lpTH->SamplesPerPixel;
offset = lpTH->SamplesPerPixel - 1;
if (lpAlphaLine)
	--offset;
lpOut = lpIn;
while ( --iInByteCount >= 0 )
	{
   *lpOut++ = *lpIn++;
	if (lpAlphaLine)
		*lpAlphaLine++ = *lpIn++;
   lpIn += offset;
	}
}

/************************************************************************/
static void Shrinkto24bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
/************************************************************************/
{
LPTR  lpOut;
int offset;

iInByteCount /= lpTH->SamplesPerPixel;
offset = lpTH->SamplesPerPixel - 3;
if (lpAlphaLine)
	--offset;
lpOut = lpIn;
while ( --iInByteCount >= 0 )
	{
	*lpOut++ = *lpIn++;
	*lpOut++ = *lpIn++;
	*lpOut++ = *lpIn++;
	if (lpAlphaLine)
		*lpAlphaLine++ = *lpIn++;
	lpIn += offset;
}
}

/************************************************************************/
static void Shrinkto32bits( LPTIFFHEADER lpTH, LPTR lpIn, int iInByteCount,
							LPTR lpAlphaLine)
/************************************************************************/
{
LPTR  lpOut;
int offset;

iInByteCount /= lpTH->SamplesPerPixel;
offset = lpTH->SamplesPerPixel - 4;
if (lpAlphaLine)
	--offset;
lpOut = lpIn;
while ( --iInByteCount >= 0 )
	{
	*lpOut++ = *lpIn++;
	*lpOut++ = *lpIn++;
	*lpOut++ = *lpIn++;
	*lpOut++ = *lpIn++;
	if (lpAlphaLine)
		*lpAlphaLine++ = *lpIn++;
	lpIn += offset;
	}
}

/***********************************************************************/
static void copyn( LPTR lpIn, LPTR lpOut, int iCount, int incr )
/***********************************************************************/
{
while ( --iCount >= 0 )
	{
	*lpOut = *lpIn++;
	lpOut += incr;
	}
}



