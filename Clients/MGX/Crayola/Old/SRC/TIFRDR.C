//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "tiffio.h"
#include <framelib.h>


// Static prototypes
static void ReadHorzDiff8( LPTR lpImage, int iCount );
static void ReadHorzDiff24( LPTR lpImage, int iCount );
static void ReadHorzDiff32( LPTR lpImage, int iCount );
static void diff_fixtagval( LPTAG tagp );
static void rightjust_tagval( LPTAG tagp );
static int flexiread( int ifh, LPTR ptr, int size );
static void skip_to_strip(void);
static void Expand4to8bits( LPTR lpIn, int iInByteCount );
static void Expand6to8bits( LPTR lpIn, int iInByteCount );
static void Expand4to24bits( LPTR lpIn, int iInByteCount );
static void Expand8to24bits( LPTR lpIn, int iInByteCount );
static void copyn( LPTR lpIn, LPTR lpOut, int iCount, int incr );
static int image_write (LPTR lpSrc, int xSrc, unsigned size_of_ptr, int count, int ifh);

static TIFFHEADER th;

#define INLINE_FILE 22222

/************************************************************************/
LPFRAME CALLBACK EXPORT tifrdr (LPSTR lpFileName, int outDepth,
					   			LPINT lpDataType, BOOL bReadOnly)
/************************************************************************/
{
return( ReadTiffData( -1/*ifh*/, lpFileName, outDepth, lpDataType, bReadOnly,
	NULL/*lpRect*/, NO/*bCrop*/, NULL/*lpFullFileX*/, NULL/*lpFullFileY*/, NULL/*lpFullresolution*/) );
}


#define REALSIZE(pix, depth) ((depth == 0) ? ((pix+7)/8) : (pix*depth) )

/************************************************************************/
LPFRAME ReadTiffData( int iFile, LPSTR lpFileName, int outDepth,
	LPINT lpDataType, BOOL bReadOnly, LPRECT lpRect, BOOL bCrop,
	LPINT lpFullFileX, LPINT lpFullFileY, LPINT lpFullResolution)
/************************************************************************/
{
int	i, ifh, y, yout, p;
LPFRAME lpFrame;
LPTR lpIn, lpOut, lpCropLine;
BOOL compressInit, bSkip;
LFIXED xrate, yrate, yoffset;
int ylast, yline, opix, olin, xstart, xend, ystart, yend;
int Resolution;
LPCOLORMAP lpColorMap;

lpFrame = NULL;
compressInit = NO;

ProgressBegin(1,0);

// open the file if necessary
if ( (ifh = iFile) < 0 && (ifh = _tlopen(lpFileName, OF_READ)) < 0)
	{
	Message (IDS_EOPEN, lpFileName);
	ProgressEnd();
	return(NULL);
	}

if ( !ReadTiffHeader( ifh, lpFileName, outDepth, lpDataType, bReadOnly, &th ) )
	goto BadRead;

if ( lpFullFileX )
	*lpFullFileX = th.npix;
if ( lpFullFileY )
	*lpFullFileY = th.nlin;
if ( lpFullResolution)
	*lpFullResolution = (int)th.lResolution;

if ( lpRect && !bCrop )
	{
	xstart = 0;
	xend = th.npix - 1;
	ystart = 0;
	yend = th.nlin - 1;
	opix = RectWidth(lpRect);
	olin = RectHeight(lpRect);
	xrate = ScaleToFit(&opix, &olin, th.npix, th.nlin);
	if (opix > th.npix || olin > th.nlin)
		{ // No upsizing allowed
		opix = th.npix;
		olin = th.nlin;
		}
	Resolution = FMUL(th.lResolution, xrate);
	xrate = FGET( th.npix, opix );
	yrate = FGET( olin, th.nlin );
	}
else
if ( lpRect && bCrop )
	{
	xstart = bound( lpRect->left, 0, th.npix-1 );
	xend = bound( lpRect->right, 0, th.npix-1 );
	ystart = bound( lpRect->top, 0, th.nlin-1 );
	yend = bound( lpRect->bottom, 0, th.nlin-1 );
	opix = xend - xstart + 1;
	olin = yend - ystart + 1;
	xrate = UNITY;
	yrate = UNITY;
	Resolution = (int)th.lResolution;
	}
else
	{
	xstart = 0;
	xend = th.npix - 1;
	ystart = 0;
	yend = th.nlin - 1;
	opix = th.npix;
	olin = th.nlin;
	xrate = UNITY;
	yrate = UNITY;
	Resolution = (int)th.lResolution;
	}

if ( xrate == UNITY )
	th.lpSampleProc = NULL;

/* Create the image frame store */

if ( !(lpFrame = FrameOpen((FRMDATATYPE)th.OutDepth, opix, olin, Resolution)) )
	{ FrameError(IDS_EIMAGEOPEN); goto BadTiff; }

if ( th.bColormap && bReadOnly )
	{ // Copy the color map from the header to the frame
	if ( lpColorMap = FrameCreateColorMap() )
		{
		lpColorMap->NumEntries = 256;
		for ( i=0; i<256; i++ )
			lpColorMap->RGBData[i] = th.RGBmap[i];
		FrameSetColorMap( lpFrame, lpColorMap );
		}
	}

if (th.bCompressed)
	{
	if (DecompressLZW (ifh, NULL, 0) < 0) /* Initialize */
		{
		Message (IDS_ECOMPRESS);
		goto BadTiff;
		}
	compressInit = YES;
	}

if ( lpRect )
	th.bOKtoPreload = NO;

// Loop on all planes and lines
// Read, expand, sample, convert, and stuff the frame
for ( p=0; p<th.InPlanes; p++ )
	{
	yoffset = (long)yrate >> 1;
	ylast = -1;

	if ( !th.bOKtoPreload )
		y = 0;
	else
		{ // if all the right circumstances...
#ifdef FILE_HANDLE 		
		y = FramePreload( lpFrame, (FILE_HANDLE)ifh, th.bNegate, AstralClockCursor );
#else		
		y = FramePreload( lpFrame, ifh, th.bNegate, AstralClockCursor );
#endif		
		if (y == -1)
			goto BadRead;
		else
		if (y == -2)
			goto BadWrite;
		else
		if (y == -3)
			goto Cancelled;
		}

	yout = y;
	for ( ; y<th.nlin; y++ )
		{
		if ( AstralClockCursor( y + (th.nlin*p), th.nlin*th.InPlanes, YES ) )
			goto Cancelled;

#ifdef WIN32
		yline = WHOLE( yoffset );
#else		
		yline = HIWORD( yoffset );
#endif		
		yoffset += UNITY;
		bSkip = ( yline == ylast || yline < ystart || yline > yend );

		if (th.bCompressed)
			{
			if ( DecompressLZW( ifh, th.lpFileLine, th.FileBPL ) < 0 )
				goto BadRead;
			if ( th.lpPredictorProc && !bSkip )
				(*th.lpPredictorProc)( (LPTR)th.lpFileLine, (int)th.FileBPL );
			}
		else
			{
			if ( image_read( th.lpFileLine, th.FileBPL, 1, ifh, !bSkip ) != th.FileBPL )
				goto BadRead;
			}

		if ( bSkip )
			continue;
		ylast = yline;

		if ( th.lpExpandProc ) // Expand in place
			(*th.lpExpandProc)( th.lpFileLine, th.FileBPL );

		// After expansion th.FileBPL will not reflect the byte count
		// use th.npix * th.InDepth

		// crop data in place - just get crop start position
		lpCropLine = th.lpFileLine + REALSIZE(xstart, th.InDepth);

		if ( th.lpSampleProc ) // Sample in place
			(*th.lpSampleProc)( lpCropLine, 0, lpCropLine, 0, opix, xrate );

		// After crop or sample, th.FileBPL will not reflect the byte count
		// use opix * th.InDepth

		if ( !(lpOut = FramePointerRaw( lpFrame, 0, yout++, YES)) )
			goto BadWrite;

		if ( th.InDepth != th.OutDepth )
			{
			ConvertData( lpCropLine, th.InDepth, opix, th.lpConvertLine,
				th.OutDepth);
			lpIn = th.lpConvertLine;
			}
		else
			lpIn = lpCropLine;

		// After depth conversion the byte count at lpIn is opix * th.OutDepth

		if ( th.bNegate ) // Negate in place
			negate( lpIn, REALSIZE(opix,th.OutDepth));

		if ( th.InPlanes != 1 )
			copyn( lpIn, lpOut+p, opix, th.InPlanes );
		else
			copy( lpIn, lpOut, REALSIZE(opix,th.OutDepth));
		}

	skip_to_strip();
	}

goto Cleanup;

BadRead:
Message (IDS_EREAD, lpFileName);
goto BadTiff;

BadWrite:
Message(IDS_EFRAMEREAD);

Cancelled:
BadTiff:

if ( lpFrame )
	FrameClose( lpFrame );
lpFrame = NULL;

Cleanup:

if (compressInit)
	{
	if (DecompressLZW (ifh, NULL, 0) < 0) /* Terminate */
		Message (IDS_ECOMPRESS);
	compressInit = NO;
	}

FreeTiffHeader( &th );
if ( ifh != iFile )
	_tlclose(ifh);
ProgressEnd();
return( lpFrame );
}

/************************************************************************/
int WriteTiffData( int iFile, LPSTR lpFileName,
					LPFRAME lpSrcFrame, int xSrc, int ySrc)
/************************************************************************/
{
int	ifh, y, p, yin;
int ret = 0;
LPTR lpSrc, lpDst;
BOOL compressInit, bSkip;
int opix, olin, xstart, xend, ystart, yend;
int depth;

compressInit = NO;

ProgressBegin(1,0);
// open the file if necessary
if ( (ifh = iFile) < 0 && (ifh = _tlopen(lpFileName, OF_READWRITE)) < 0)
	{
	Message (IDS_EOPEN, lpFileName);
	ProgressEnd();
	return(NULL);
	}

depth = FrameDepth(lpSrcFrame);

if ( !ReadTiffHeader( ifh, lpFileName, -1, NULL, NO/*bReadOnly*/, &th ) )
	goto BadRead;

if (th.bCompressed)
	{
	Print("Can't write chunks to compressed files");
	goto BadTiff;
	}

opix = FrameXSize(lpSrcFrame);
olin = FrameYSize(lpSrcFrame);
xstart = bound( xSrc, 0, th.npix-1 );
xend = bound( xstart+opix-1, 0, th.npix-1 );
ystart = bound( ySrc, 0, th.nlin-1 );
yend = bound( ystart+olin-1, 0, th.nlin-1 );

if (th.bCompressed)
	{
	if (DecompressLZW (ifh, NULL, 0) < 0) /* Initialize */
		{
		Message (IDS_ECOMPRESS);
		goto BadTiff;
		}
	compressInit = YES;
	}

// Loop on all planes and lines
// Read, expand, sample, convert, and stuff the frame
for ( p=0; p<th.InPlanes; p++ )
	{
	yin = 0;
	for (y = 0; y < th.nlin; y++ )
		{
		AstralClockCursor( y + (th.nlin*p), th.nlin*th.InPlanes, NO );

		bSkip = ( y < ystart || y > yend );

		if (th.bCompressed)
			{
			if ( DecompressLZW( ifh, th.lpFileLine, th.FileBPL ) < 0 )
				goto BadRead;
			}
		else
			{
			if ( image_read( th.lpFileLine, th.FileBPL, 1, ifh, NO ) != th.FileBPL )
				goto BadRead;
			}

		if ( bSkip )
			continue;

// maybe we need expansion to combine with our data, plug in later
//		if ( th.lpExpandProc ) // Expand in place
//			(*th.lpExpandProc)( th.lpFileLine, th.FileBPL );

		// After expansion th.FileBPL will not reflect the byte count
		// use th.npix * th.InDepth

		// crop data in place - just get crop start position
		lpDst = th.lpFileLine + REALSIZE(xstart, th.InDepth);

		if (!(lpSrc = FramePointerRaw(lpSrcFrame, 0, yin++, NO)))
				goto BadRead;

		// After depth conversion the byte count at lpIn is opix * th.OutDepth
		if ( th.InDepth != depth )
			{
			ConvertData( lpSrc, depth, opix, th.lpConvertLine,
				th.InDepth);
			lpSrc = th.lpConvertLine;
			}

		if ( th.InPlanes != 1 )
			copyn( lpSrc, lpDst+p, opix, th.InPlanes );
		else
			copy( lpSrc, lpDst, REALSIZE(opix, th.InDepth) );

		if ( th.bNegate ) // Negate in place
			negate( lpDst, REALSIZE(opix, th.InDepth) );

		if ( image_write( lpDst, xstart, REALSIZE(opix, th.InDepth),
						1, ifh ) < 0 )
			goto BadWrite;
		}

	skip_to_strip();
	}

goto Cleanup;

BadRead:
Message (IDS_EREAD, lpFileName);
goto BadTiff;

BadWrite:
FrameError(IDS_EFRAMEREAD);

BadTiff:
ret = -1;

Cleanup:

if (compressInit)
	{
	if (DecompressLZW (ifh, NULL, 0) < 0) /* Terminate */
		Message (IDS_ECOMPRESS);
	compressInit = NO;
	}

FreeTiffHeader( &th );
if ( ifh != iFile )
	_tlclose(ifh);
ProgressEnd();
return( ret );
}

static long tof;
static short order;

/************************************************************************/
int image_read (LPTR ptr, unsigned size_of_ptr, int count, int ifh, BOOL bRead)
/************************************************************************/
{
long req_bytes, strip_start, strip_end, strip_pos;
long bytes_read, bytes_in_strip, bytes_to_read;
int k;

if (!bRead)
	{
	th.ImagePos += (long)size_of_ptr * (long)count;
	return(count);
	}
	
if (th.StripsPerImage == 1)
	{
	if (th.ImagePos)
		{
		_tllseek (ifh, th.ImagePos, 1);
		th.ImagePos = 0;
		}
	bytes_to_read = size_of_ptr * count;
	bytes_read = _tlread (ifh, ptr, (WORD)bytes_to_read);
//	if (bytes_read != bytes_to_read)
//		return(-1);
	return (bytes_read);
	}
else
	{
	req_bytes = (long)size_of_ptr * (long)count;
	strip_start = 0;
	for (k = 0; k < th.StripsPerImage; k++)
		{
		strip_end = strip_start + th.StripByteCounts[k] - 1;
		if ((th.ImagePos >= strip_start) && (th.ImagePos <= strip_end))
			{
			strip_pos = th.StripOffsets[k] + (th.ImagePos - strip_start);
			_tllseek (ifh, strip_pos+tof, 0);
			bytes_in_strip = strip_end - th.ImagePos + 1;
			if (bytes_in_strip > req_bytes)
					bytes_to_read = req_bytes;
			else	bytes_to_read = bytes_in_strip;
			bytes_read = _tlread (ifh, ptr, (WORD)bytes_to_read);
			th.ImagePos += bytes_read;
			if (bytes_read != bytes_to_read)
					return(-1);
			else	return (bytes_read / size_of_ptr);
			}
		strip_start = strip_end + 1;
		}
	}
return (-1);
}

/************************************************************************/
static int image_write (LPTR lpSrc, int xSrc, unsigned size_of_ptr, int count, int ifh)
/************************************************************************/
{
long req_bytes, strip_start, strip_end, strip_pos;
long bytes_written, bytes_in_strip, bytes_to_write;
int k, bytes_per_pixel, ret = -1;
long cur_pos, lOffset, lImagePos;

bytes_per_pixel = th.FileBPL / th.npix;

// calculate offset for current position to write data
lOffset = REALSIZE(xSrc,bytes_per_pixel) - (long)th.FileBPL;
if (th.StripsPerImage == 1)
	{
	if (th.ImagePos)
		lOffset += th.ImagePos;
	cur_pos = _tllseek(ifh, 0L, 1);
	// seek to position to write data
	_tllseek(ifh, lOffset, 1);

	bytes_to_write = size_of_ptr * count;
	bytes_written = _tlwrite (ifh, lpSrc, (WORD)bytes_to_write);
	if (bytes_written == bytes_to_write)
		ret = count;
	_tllseek(ifh, cur_pos, 0);
	}
else
	{
	lImagePos = th.ImagePos;
	th.ImagePos += lOffset;
	req_bytes = (long)size_of_ptr * (long)count;
	strip_start = 0;
	for (k = 0; k < th.StripsPerImage; k++)
		{
		strip_end = strip_start + th.StripByteCounts[k] - 1;
		if ((th.ImagePos >= strip_start) && (th.ImagePos <= strip_end))
			{
			strip_pos = th.StripOffsets[k] + (th.ImagePos - strip_start);
			_tllseek (ifh, strip_pos+tof, 0);
			bytes_in_strip = strip_end - th.ImagePos + 1;
			if (bytes_in_strip > req_bytes)
					bytes_to_write = req_bytes;
			else	bytes_to_write = bytes_in_strip;
			bytes_written = _tlwrite (ifh, lpSrc, (WORD)bytes_to_write);
			th.ImagePos += bytes_written;
			if (bytes_written == bytes_to_write)
				ret =  (bytes_written / size_of_ptr);
			break;
			}
		strip_start = strip_end + 1;
		}
	th.ImagePos = lImagePos;
	}
return (ret);
}

/************************************************************************/
static void skip_to_strip(void)
/************************************************************************/
{
	long strip_start, strip_end;
	int k;

	if (th.StripsPerImage == 1) 
	{
		return;
	}

	strip_start = 0;
	for (k = 0; k < th.StripsPerImage; k++)
	{
		strip_end = strip_start + th.StripByteCounts[k] - 1;

		if ((th.ImagePos >= strip_start) &&
			(th.ImagePos <= strip_end))
		{
			/* We know the current strip */
			th.ImagePos = strip_end+1;
			break;
		}
		strip_start = strip_end + 1;
	}
}

/************************************************************************/
static void ReadHorzDiff8( LPTR lpImage, int iCount )
/************************************************************************/
{
int col;

for (col = 1; col < iCount; col++)
	lpImage[col] += lpImage[col-1];
}

/************************************************************************/
static void ReadHorzDiff24( LPTR lpImage, int iCount )
/************************************************************************/
{
int col;

for (col = 3; col < iCount; col++)
	lpImage[col] += lpImage[col-3];
}

/************************************************************************/
static void ReadHorzDiff32( LPTR lpImage, int iCount )
/************************************************************************/
{
int col;

for (col = 4; col < iCount; col++)
	lpImage[col] += lpImage[col-4];
}

/************************************************************************/
void image_read_startstrip()
/************************************************************************/
{
int	k;
long	strip_start;

strip_start = 0;
for (k = 0; (k < th.StripsPerImage) && (th.ImagePos > strip_start); k++)
 	strip_start += th.StripByteCounts[k];
th.ImagePos = strip_start;
}

/* TIFF defines */
#define TIF_BYTE 1
#define TIF_ASCII 2
#define TIF_SHORT 3
#define TIF_LONG 4
#define TIF_RATIONAL 5
#define TIF_MM (0x4D4D)
#define TIF_II (0x4949)
#ifdef _MAC
  #define MYORDER (TIF_MM)
#else  
  #define MYORDER (TIF_II)
#endif
#define TIFF_VERSION 42

/************************************************************************/
static void diff_fixtagval (LPTAG tagp)
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
static void rightjust_tagval (LPTAG tagp)
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
	 tagp->value = * ((LPWORD) (& (tagp->value)));
else
if (length == 1)
	 tagp->value = * ((LPTR) (& (tagp->value)));
}

/************************************************************************/
static int flexiread (
/************************************************************************/
int		ifh,
LPTR	ptr,
REG int	size)
{
REG int	rc;
LPTAG tagp;

if ((rc = _tlread (ifh, ptr, size)) != size)
	return (rc);

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
BOOL ReadTiffHeader( int iFile, LPSTR lpFileName, int OutDepth, LPINT lpDataType,
	BOOL bReadOnly, LPTIFFHEADER lpHeader )
/************************************************************************/
{
long ifd_ptr, xresden, xresnum, yresden, yresnum, lSavePos, lTemp, offset;
short sTemp;
int ifh;
short npix, nlin, holder, photomet, i, k, iMaxValue, samples, predictor;
short inkset, dotrange0, dotrange1, bpp, iColormapEntries, iExpandFactor;
TAG	tag;
LPRGB lpRGB;
LPWORD lpRed, lpGreen, lpBlue, lpWord;
RGBS RGBmap[256];
BOOL bCompressed, bMustShiftDown, bOKtoPreload, bPlanar;
long StripsPerImage, lColormap;
LPLONG StripByteCounts, StripOffsets;

// Clear out all the header data
clr( (LPTR)lpHeader, sizeof(TIFFHEADER) );

// open the file if necessary
if ( (ifh = iFile) < 0 && (ifh = _tlopen(lpFileName, OF_READ)) < 0)
	{
	Message (IDS_EOPEN, lpFileName);
	return( FALSE );
	}

/* assumed to be full res data, tag 0xff value = 1 */

/* default values */
bpp = 1; // Lineart
photomet = 0; // min value is white
xresnum = 0;
xresden = 1;
yresnum = 0;
yresden = 1;
lColormap = 0;
iColormapEntries = 0;
iMaxValue = 255;
samples = 1;
bPlanar = NO;
bCompressed = NO;
predictor = 1;
bOKtoPreload = (iFile == INLINE_FILE ? NO : YES );
StripsPerImage = 1;
StripByteCounts = NULL;
StripOffsets = NULL;
/* end default values */

// save top-of-file
tof = _tllseek (ifh, 0L, 1);

if (_tlread (ifh, (LPTR)&order, 2) != 2) /* read byte order */
	goto BadRead;
if (order != TIF_II && order != TIF_MM)
	{ Message (IDS_EIMGBAD, lpFileName); goto BadTiff; }
if (flexiread (ifh, (LPTR)&holder, 2) != 2) /* read tiff version number */
	goto BadRead;
if (holder != 0x2a) /* only version 42 allowed for now */
	{ Message (IDS_EIMGVERSION, lpFileName); }
if (flexiread (ifh, (LPTR)&ifd_ptr, 4) != 4) /* read ifd pointer */
	goto BadRead;

/****************************************************************/

next_ifd:
if (_tllseek (ifh, ifd_ptr+tof, 0) == -1) /* seek to location of ifd */
	goto BadRead;
if (flexiread (ifh, (LPTR)&holder, 2) != 2) /* read # of tag field entries */
	goto BadRead;
for (i=0; i<holder; i++)
	{
	if (flexiread (ifh, (LPTR)&tag, 12) != 12) /* read the tag entry */
		goto BadRead;
	switch (tag.tagno)
		{
		case 0xff: /* sub-file type */
			if (tag.value != 1) /* want only full res data */
				{
				if (flexiread (ifh, (LPTR)&tag, 12) != 12)
					goto BadRead;
				if (flexiread (ifh, (LPTR)&ifd_ptr, 4) != 4)
					goto BadRead;
				if (ifd_ptr == 0)
					goto BadTiff;
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
			if (tag.length == 1)
				bpp = tag.value;
			else
				{ // the value is a file pointer
				/* save the current file position */
				lSavePos = _tllseek (ifh, 0L, 1);

				/* find the first offset */
				/* seek to line loc */
				if (_tllseek (ifh, tag.value+tof, 0) == -1)
					goto BadRead;

				bpp = 0;
				/* read the first value */
				for (k = 0; k < tag.length; ++k)
					{
					if (flexiread (ifh, (LPTR)&sTemp, 2) == 0)
						goto BadRead;
					bpp += sTemp;
					}
				/* seek to where we were */
				if (_tllseek (ifh, lSavePos, 0) == -1)
					goto BadRead;
				}
			// Bound bpp
			if ( bpp > 24 )
				bpp = 32;
			else
			if ( bpp > 8 )
				bpp = 24;
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
				{ Message (IDS_EIMGCOMP, lpFileName); goto BadTiff; }
			break;
		case 0x106: /* photometeric interpretation */
			if ((tag.value != 0) && (tag.value != 1) &&
				(tag.value != 2) && (tag.value != 3) &&
				(tag.value != 5))
				{ Message (IDS_EIMGPHOTO, lpFileName); }
			photomet = tag.value;
			break;
		case 0x10a: /* fill order */
			if (tag.value != 1) /* not default, try to continue */
				{
				Message (IDS_EIMGFILL, lpFileName);
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
					Message (IDS_EMEMALLOC);
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
					Message (IDS_EMEMALLOC);
					goto BadRead;
					}
				/* save the current file position */
				lSavePos = _tllseek (ifh, 0L, 1);
				/* find the first offset */
				/* seek to line loc */
				if (_tllseek (ifh, tag.value+tof, 0) == -1)
					goto BadRead;
				/* read the first line pointer */
				for (k = 0; k < tag.length; ++k)
					{
					if (tag.type == TIF_LONG)
						{
						if (flexiread (ifh, (LPTR)&lTemp, 4) == 0)
							goto BadRead;
						}
					else
						{
						if (flexiread (ifh, (LPTR)&sTemp, 2) == 0)
							goto BadRead;
						lTemp = sTemp;
						}
					StripOffsets[k] = lTemp;
					}
				/* seek to where we were */
				if (_tllseek (ifh, lSavePos, 0) == -1)
					goto BadRead;
				}
			break;
		case 0x112: /* orientation */
			if (tag.value != 1) /* not default, try to continue */
				{
				Message (IDS_EIMGORIENT, lpFileName);
				bOKtoPreload = NO;
				}
			break;
		case 0x115: /* samples per pixel */
			samples = tag.value;
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
					Message (IDS_EMEMALLOC);
					goto BadRead;
					}
				StripByteCounts[0] = tag.value;
				}
			else
				{
				StripByteCounts = (LPLONG)Alloc (tag.length * sizeof(long));
				if (!StripByteCounts)
					{
					Message (IDS_EMEMALLOC);
					goto BadRead;
					}
				/* save the current file position */
				lSavePos = _tllseek (ifh, 0L, 1);
				/* find the first offset */
				/* seek to line loc */
				if (_tllseek (ifh, tag.value+tof, 0) == -1)
					goto BadRead;
				/* read the first line pointer */
				for (k = 0; k < tag.length; ++k)
					{
					if (tag.type == TIF_LONG)
						{
						if (flexiread (ifh, (LPTR)&lTemp, 4) == 0)
							goto BadRead;
						}
					else
						{
						if (flexiread (ifh, (LPTR)&sTemp, 2) == 0)
							goto BadRead;
						lTemp = sTemp;
						}
					StripByteCounts[k] = lTemp;
					}
				/* seek to where we were */
				if (_tllseek (ifh, lSavePos, 0) == -1)
					goto BadRead;
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
			lSavePos = _tllseek (ifh, 0L, 1);
			/* find the first offset */
			/* seek to line loc */
			if (_tllseek (ifh, tag.value+tof, 0) == -1)
				goto BadRead;
			if (tag.type == TIF_SHORT)
				{
				unsigned short sTemp;

				flexiread(ifh, (LPTR)&sTemp, 2);
				dotrange0 = sTemp;
				flexiread(ifh, (LPTR)&sTemp, 2);
				dotrange1 = sTemp;
				}
			else
				{
				unsigned char cTemp;

				flexiread(ifh, (LPTR)&cTemp, 1);
				dotrange0 = cTemp;
				flexiread(ifh, (LPTR)&cTemp, 1);
				dotrange1 = cTemp;
				}
			/* seek to where we were */
			if (_tllseek (ifh, lSavePos, 0) == -1)
				goto BadRead;
			break;
		default:
			break;
		}
	}

if ( !StripOffsets )
	goto BadTiff;

/* if byte counts tag is missing, fake it */
if ( !StripByteCounts )
	{
	if ( !(StripByteCounts = (LPLONG)Alloc( StripsPerImage * sizeof(long) )) )
		{
		Message (IDS_EMEMALLOC);
		goto BadRead;
		}
	for (k = 0; k < StripsPerImage-1; k++)
		{
		StripByteCounts[k] =
			StripOffsets[k+1] - StripOffsets[k];
		if (StripByteCounts[k] < 1)
			goto BadTiff;
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
	if (_tllseek (ifh, xresnum+tof, 0) == -1) /* seek to loc of xres */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&xresnum, 4) != 4) /* read xres numerator */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&xresden, 4) != 4) /* read xres denominator */
		goto BadRead;
	xresnum /= xresden;
	}
else
	xresnum = 75;

if (yresnum)
	{
	if (_tllseek (ifh, yresnum+tof, 0) == -1) /* seek to loc of yres */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&yresnum, 4) != 4) /* read yres numerator */
		goto BadRead;
	if (flexiread (ifh, (LPTR)&yresden, 4) != 4) /* read yres denominator */
		goto BadRead;
	yresnum /= yresden;
	}
else
	yresnum = 75;

if ( photomet != 3 ) // turn off the color map if its not palette color
	lColormap = 0;

if (lColormap)
	{
	if (_tllseek (ifh, tof+lColormap, 0) == -1) /* seek to loc of lColormap */
		goto BadRead;
	if (_tlread (ifh, LineBuffer[0], (UINT)(2*3*(iColormapEntries))) != 2*3*(iColormapEntries))
		goto BadRead;
	if (MYORDER != order)
		{
		lpWord = (LPWORD)LineBuffer[0];
		for (i=0; i<3*(iColormapEntries); i++, lpWord++)
			swapw (lpWord);
		}

	lpWord = (LPWORD)LineBuffer[0];
	bMustShiftDown = NO;
	for (i=0; i<3*iColormapEntries; i++)
		{ // See if the pallete needs to be shifted down
		if (*lpWord++ > 255)
			{
			bMustShiftDown = YES;
			break;
			}
		}

	lpRGB = RGBmap;
	lpRed = (LPWORD)LineBuffer[0];
	lpGreen = lpRed + iColormapEntries;
	lpBlue = lpGreen + iColormapEntries;
	for (i=0; i<iColormapEntries; i++)
		{
		if (bMustShiftDown)
			{
			lpRGB->red   = *lpRed++ >> 8;
			lpRGB->green = *lpGreen++ >> 8;
			lpRGB->blue  = *lpBlue++ >> 8;
			}
		else
			{
			lpRGB->red   = *lpRed++;
			lpRGB->green = *lpGreen++;
			lpRGB->blue  = *lpBlue++;
			}
		lpRGB++;
		}
	}

// Seek to beginning of the data
if (_tllseek (ifh, StripOffsets[0]+tof, 0) == -1)
	goto BadRead;

if ( bpp == 8 && iMaxValue == 63 )
	bpp = 6;

if ( samples == 3 && bpp != 3 )
	bpp = 24;

if ( samples == 4 )
	bpp = 32;

switch ( bpp )
	{
	case 1:
		if (lpDataType)
			*lpDataType = IDC_SAVELA;
		iExpandFactor = 8; // Only because the Sample_1 unpacks in place
		lpHeader->lpExpandProc = NULL;
		lpHeader->lpSampleProc = Sample_1; // Happens after expansion
		lpHeader->InDepth = 0; // Depth after expansion routine
		lpHeader->lpPredictorProc = NULL; // Happens before expansion
		lpHeader->InPlanes = 1;
		lpHeader->FileBPL  = (npix+7)/8;
		break;
	case 4:
		if ( lColormap && bReadOnly )
			{ // Palette color; expand to frame with depth=1
			if (lpDataType)
				*lpDataType = IDC_SAVE8BITCOLOR;
			iExpandFactor = 2;
			lpHeader->lpExpandProc = Expand4to8bits;
			lpHeader->lpSampleProc = Sample_8; // Happens after expansion
			lpHeader->InDepth = 1; // Depth after expansion routine
			}
		else
		if ( lColormap )
			{ // Palette color; expand to frame with depth=3
			if (lpDataType)
				*lpDataType = IDC_SAVE8BITCOLOR;
			iExpandFactor = 6;
			lpHeader->lpExpandProc = Expand4to24bits;
			lpHeader->lpSampleProc = Sample_24; // Happens after expansion
			lpHeader->InDepth = 3; // Depth after expansion routine
			}
		else
			{
			if (lpDataType)
				*lpDataType = IDC_SAVECT;
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
		// Fall through
	case 8: // Palette color
		if ( lColormap && bReadOnly )
			{ // Palette color; expand to frame with depth=1
			if (lpDataType)
				*lpDataType = IDC_SAVE8BITCOLOR;
			iExpandFactor = 1;
			lpHeader->lpExpandProc = NULL;
			lpHeader->lpSampleProc = Sample_8; // Happens after expansion
			lpHeader->InDepth = 1; // Depth after expansion routine
			bOKtoPreload = NO;
			}
		else
		if ( lColormap )
			{ // Palette color; expand to frame with depth=3
			if (lpDataType)
				*lpDataType = IDC_SAVE8BITCOLOR;
			iExpandFactor = 3;
			lpHeader->lpExpandProc = Expand8to24bits;
			lpHeader->lpSampleProc = Sample_24; // Happens after expansion
			lpHeader->InDepth = 3; // Depth after expansion routine
			bOKtoPreload = NO;
			}
		else
			{
			if (lpDataType)
				*lpDataType = IDC_SAVECT;
			iExpandFactor = 1;
			lpHeader->lpExpandProc = NULL;
			lpHeader->lpSampleProc = Sample_8; // Happens after expansion
			lpHeader->InDepth = 1; // Depth after expansion routine
			}
		lpHeader->lpPredictorProc = ReadHorzDiff8; // Happens before expansion
		lpHeader->InPlanes = 1;
		lpHeader->FileBPL = npix;
		break;
	case 24:
		if (lpDataType)
			*lpDataType = IDC_SAVE24BITCOLOR;
		iExpandFactor = 1;
		lpHeader->lpExpandProc = NULL;
		lpHeader->lpSampleProc = Sample_24; // Happens after expansion
		lpHeader->InDepth = 3; // Depth after expansion routine
		lpHeader->lpPredictorProc = ReadHorzDiff24; // Happens before expansion
		lpHeader->InPlanes = ( bPlanar ? 3 : 1 );
		lpHeader->FileBPL = ( bPlanar ? npix : 3 * npix );
		break;
	case 32:
		if (lpDataType)
			*lpDataType = IDC_SAVE32BITCOLOR;
		iExpandFactor = 1;
		lpHeader->lpExpandProc = NULL;
		lpHeader->lpSampleProc = Sample_32; // Happens after expansion
		lpHeader->InDepth = 4; // Depth after expansion routine
		lpHeader->lpPredictorProc = ReadHorzDiff32; // Happens before expansion
		lpHeader->InPlanes = ( bPlanar ? 4 : 1 );
		lpHeader->FileBPL = ( bPlanar ? npix : 4 * npix );
		break;
	default:
		goto BadTiff;
	}

// Make adjustments to output depth
if ( OutDepth < 0 )
	{
	OutDepth = lpHeader->InDepth;
	if ( !OutDepth && Control.LineArtAsGray )
		OutDepth = 1;
	}

if ( OutDepth != lpHeader->InDepth )
	bOKtoPreload = NO;

// Setup the basic file information
lpHeader->lResolution = xresnum;
lpHeader->npix = npix;
lpHeader->nlin = nlin;
lpHeader->OutDepth = OutDepth;
lpHeader->bOKtoPreload = bOKtoPreload;
lpHeader->bNegate = (photomet == 0); // min value is white
lpHeader->bCompressed = bCompressed;
lpHeader->bColormap = lColormap;
copy( (LPTR)RGBmap, (LPTR)lpHeader->RGBmap, 3*iColormapEntries );

// Setup the strip information
lpHeader->StripsPerImage = StripsPerImage;
lpHeader->StripByteCounts = StripByteCounts;
lpHeader->StripOffsets = StripOffsets;

// Allocate a single input, expansion and sampling buffer
if ( !AllocLines( &lpHeader->lpFileLine, 1, lpHeader->FileBPL, iExpandFactor ) )
	{ // Provide for worst case expansion in place
	Message (IDS_EMEMALLOC);
	goto BadRead;
	}

// Allocate a depth conversion buffer
if ( !AllocLines( &lpHeader->lpConvertLine, 1, npix, OutDepth ? OutDepth : 1 ) )
	{
	Message (IDS_EMEMALLOC);
	goto BadRead;
	}

// Zap the LZW compression predictor proc, if necessary
if ( !bCompressed || predictor != 2 )
	lpHeader->lpPredictorProc = NULL; // Happens before expansion

if ( ifh != iFile )
	_tlclose(ifh);
return( TRUE );

BadRead:
BadTiff:

if ( ifh != iFile )
	_tlclose(ifh);
return( FALSE );
}


/************************************************************************/
void FreeTiffHeader( LPTIFFHEADER lpHeader )
/************************************************************************/
{
if ( lpHeader->lpFileLine )
	FreeUp( lpHeader->lpFileLine );
if ( lpHeader->lpConvertLine )
	FreeUp( lpHeader->lpConvertLine );
if ( lpHeader->StripOffsets )
	FreeUp( (LPTR)lpHeader->StripOffsets );
if ( lpHeader->StripByteCounts )
	FreeUp( (LPTR)lpHeader->StripByteCounts );
clr( (LPTR)lpHeader, sizeof(TIFFHEADER) );
}

/************************************************************************/
static void Expand6to8bits( LPTR lpIn, int iInByteCount )
/************************************************************************/
{
map( ShiftUp2Lut, lpIn, iInByteCount );
}

/************************************************************************/
static void Expand4to24bits( LPTR lpIn, int iInByteCount )
/************************************************************************/
{
LPRGB lpRGBOut;
BYTE b, c;

lpRGBOut = (LPRGB)lpIn;
lpRGBOut += (2 * iInByteCount - 1);
lpIn += (iInByteCount - 1);
while ( --iInByteCount >= 0 )
	{
	b = *lpIn--;
	c = b & 0xF;
	lpRGBOut->red   = th.RGBmap[ c ].red;
	lpRGBOut->green = th.RGBmap[ c ].green;
	lpRGBOut->blue  = th.RGBmap[ c ].blue;
	lpRGBOut--;
	c = (b >> 4);
	lpRGBOut->red   = th.RGBmap[ c ].red;
	lpRGBOut->green = th.RGBmap[ c ].green;
	lpRGBOut->blue  = th.RGBmap[ c ].blue;
	lpRGBOut--;
	}
}

//static BYTE Nibbles[] = {
//0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 255 };

/************************************************************************/
static void Expand4to8bits( LPTR lpIn, int iInByteCount )
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
static void Expand8to24bits( LPTR lpIn, int iInByteCount )
/************************************************************************/
{
LPRGB lpRGBOut;
BYTE b;

lpRGBOut = (LPRGB)lpIn;
lpRGBOut += (iInByteCount - 1);
lpIn += (iInByteCount - 1);
while ( --iInByteCount >= 0 )
	{
	b = *lpIn--;
	lpRGBOut->red   = th.RGBmap[ b ].red;
	lpRGBOut->green = th.RGBmap[ b ].green;
	lpRGBOut->blue  = th.RGBmap[ b ].blue;
	lpRGBOut--;
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


static LPTR lpInLineData, lpInLineDataHead, lpInLineDataEnd;

#define BAD_PTR() ( lpInLineData < lpInLineDataHead || lpInLineData > lpInLineDataEnd )

/***********************************************************************/
HFILE WINAPI _tlopen( LPCSTR lpFileName, int iMode )
/***********************************************************************/
{
WORD wType;
DWORD dwSize, dwValue;
HGLOBAL hMemory;

wType = *((LPWORD)lpFileName);
if ( wType == TIF_II || wType == TIF_MM )
	{
	lpInLineData = lpInLineDataHead = (LPTR)lpFileName;
#ifdef WIN32
	hMemory = GlobalHandle( lpInLineDataHead );
#else
	dwValue = GlobalHandle( HIWORD(lpInLineDataHead) );
	if ( HIWORD(dwValue) != HIWORD(lpInLineDataHead) )
		return( -1 ); // segment portions should match
	hMemory = (HGLOBAL)LOWORD( dwValue );
#endif	
	dwSize = GlobalSize( hMemory );
	lpInLineDataEnd = lpInLineDataHead + dwSize - 1;
	return( INLINE_FILE );
	}
return( _lopen( lpFileName, iMode ) );
}

/***********************************************************************/
HFILE WINAPI _tlclose( HFILE hFile )
/***********************************************************************/
{
if ( hFile == INLINE_FILE )
	return( 0 );
return( _lclose( hFile ) );
}

/***********************************************************************/
LONG WINAPI _tllseek( HFILE hFile, LONG lPos, int iMode )
/***********************************************************************/
{
if ( hFile == INLINE_FILE )
	{
	switch ( iMode )
		{
		case 0: // from beginning
			lpInLineData = lpInLineDataHead + lPos;
			break;
		case 1: // from from current
			lpInLineData += lPos;
			break;
		case 2: // from end
			lpInLineData = lpInLineDataEnd + lPos;
			break;
		}
	if ( BAD_PTR() )
		return( -1 );
	return( (long)(lpInLineData - lpInLineDataHead) );
	}
return( _llseek( hFile, lPos, iMode ) );
}

/***********************************************************************/
UINT WINAPI _tlread( HFILE hFile, void _huge* lpData, UINT wCount )
/***********************************************************************/
{
WORD wCountLeft;

if ( hFile == INLINE_FILE )
	{
	if ( BAD_PTR() )
		return( -1 );
	wCountLeft = lpInLineDataEnd - lpInLineData + 1;
	if ( wCount > wCountLeft )
		wCount = wCountLeft;
	hmemcpy( lpData, lpInLineData, wCount );
	lpInLineData += wCount;
	return( wCount );
	}
return( _lread( hFile, lpData, wCount ) );
}

/***********************************************************************/
UINT WINAPI _tlwrite( HFILE hFile, const void _huge* lpData, UINT wCount )
/***********************************************************************/
{
if ( hFile == INLINE_FILE )
	{
	if ( BAD_PTR() )
		return( -1 );
	hmemcpy( lpInLineData, lpData, wCount );
	lpInLineData += wCount;
	return( wCount );
	}
return( _lwrite( hFile, (LPSTR)lpData, wCount ) );
}

