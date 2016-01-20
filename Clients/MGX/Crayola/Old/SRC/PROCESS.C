//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static BOOL MirrorObj(LPOBJECT lpObject, LPMASK lpMask,
				BOOL xmirror, BOOL ymirror, LPRECT lpRect);
static BOOL MirrorPixmap(LPPIXMAP lpPixmap, LPRECT lpRect,
					LPMASK lpMask, int xOffset, int yOffset,
					BOOL xmirror, BOOL ymirror);
LPFRAME DoSize(LPFRAME lpSrcFrame, int pix, int lin, int res);


/************************************************************************/
BOOL DoMirror(LPIMAGE lpImage, BOOL xmirror, BOOL ymirror, int dirty)
/************************************************************************/
{
RECT rEdit;
LPMASK lpMask;
LPOBJECT lpObject;
BOOL fChanged = NO;

if (!lpImage)
	return(fChanged);

ProgressBegin(ImgCountSelObjects(lpImage, NULL),
			dirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if (!ImgEditInit(lpImage, ET_SELOBJECTS, UT_DATA|UT_ALPHA, NULL))
	{
	ProgressEnd();
	return(FALSE);
	}

lpMask = ImgGetMask(lpImage);
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if ( !MirrorObj(lpObject, lpMask, xmirror, ymirror, &rEdit))
		break;
	ImgEditedObject(lpImage, lpObject, dirty, &rEdit);
	fChanged = YES;
	UpdateImage(&rEdit, YES);
	}
ProgressEnd();
return(fChanged);
}

/************************************************************************/
static BOOL MirrorObj(LPOBJECT lpObject, LPMASK lpMask,
				BOOL xmirror, BOOL ymirror, LPRECT lpRect)
/************************************************************************/
{
	RECT rMask;
	
	// if mask, use mask rect for area, otherwise use objects rect
	if (lpMask)
		MaskRect(lpMask, &rMask);
	else
		rMask = lpObject->rObject;
	
	// if we don't intersect the mask area, get out
	if (!AstralIntersectRect(lpRect, &rMask, &lpObject->rObject))
		return(FALSE);
	
	// get rect in object coordinate system
	rMask = *lpRect;
	OffsetRect(&rMask, -lpObject->rObject.left, -lpObject->rObject.top);
	
	ProgressBegin(lpObject->lpAlpha ? 2:1, 0);
	// mirror objects alpha channel if it exists
	if (lpObject->lpAlpha)
		if (!MirrorPixmap(&lpObject->lpAlpha->Pixmap, &rMask,
						(lpMask == lpObject->lpAlpha) ? (LPMASK)NULL : lpMask,
						lpObject->rObject.left, lpObject->rObject.top,
						xmirror, ymirror))
		{
			ProgressEnd();
			return(FALSE);
		}
		
	// mirror objects data
	if (!MirrorPixmap(&lpObject->Pixmap, &rMask,
					lpMask, lpObject->rObject.left, lpObject->rObject.top,
					xmirror, ymirror))
	{
		ProgressEnd();
		return(FALSE);
	}
	ProgressEnd();
	return( TRUE );
}

/************************************************************************/
static BOOL MirrorPixmap(LPPIXMAP lpPixmap, LPRECT lpRect,
					LPMASK lpMask, int xMaskOffset, int yMaskOffset,
					BOOL xmirror, BOOL ymirror)
/************************************************************************/
{
int i, dy, lines, depth;
int sy1, sx, sy2, dx;
LPTR lpBuffer[2];
LPTR lpLine;
void (far *lpMirrorProc)(LPTR,int);
LPPROCESSPROC lpProcessProc;
LPFRAME lpFrame;
BOOL bRet = FALSE;
LPTR lpMaskBuf = NULL;

lpFrame = lpPixmap->EditFrame;
switch ( depth = FrameDepth(lpFrame) )
    {
    case 0:
	depth = 1;
    case 1:
	lpMirrorProc = mirror8;
	break;
    case 3:
	lpMirrorProc = mirror24;
	break;
    case 4:
	lpMirrorProc = mirror32;
	break;
    default:
	return( FALSE );
    }
lpProcessProc = GetProcessProc(MM_NORMAL, depth);

dy = RectHeight(lpRect);
lines = 1;
if (ymirror)
	{
	dy = ( dy + 1 ) / 2;
	++lines;
	}

if ( !(AllocLines( lpBuffer, lines, RectWidth(lpRect), depth )) )
	{
	Message( IDS_EMEMALLOC );
	return( FALSE );
	}
ProgressBegin(1,0);
if (lpMask)
	{
	if (!(lpMaskBuf = Alloc(RectWidth(lpRect))))
		{
		Message( IDS_EMEMALLOC );
		goto Exit;
		}
	}
sy1 = lpRect->top;
if (ymirror)
	sy2 = lpRect->bottom;
else	sy2 = sy1;
sx = lpRect->left;
dx = RectWidth(lpRect);
for (i = 0; i < dy; ++i)
	{
	AstralClockCursor(i, dy, NO);
	if ( !FrameRead( lpFrame, sx, sy1, dx, lpBuffer[0], dx) )
		{
		FrameError(IDS_EFRAMEREAD);
		goto Exit;
		}
	if ( xmirror )
		(*lpMirrorProc)( lpBuffer[0], dx );

	if (ymirror)
		{
		if ( !FrameRead(lpFrame, sx, sy2, dx, lpBuffer[1], dx ) )
			{
			FrameError(IDS_EFRAMEREAD);
			goto Exit;
			}
		if (xmirror)
			(*lpMirrorProc)(lpBuffer[1], dx );
		}

	if (!(lpLine = PixmapPtr(lpPixmap, PMT_EDIT, sx, sy2, YES)))
		{
		FrameError(IDS_EFRAMEREAD);
		goto Exit;
		}
	if (lpMask)
		{
		MaskLoad( lpMask, sx+xMaskOffset, sy2+yMaskOffset, dx, lpMaskBuf );
		(*lpProcessProc)(lpLine, lpBuffer[0], lpMaskBuf, dx);
		}
	else
		copy(lpBuffer[0], lpLine, dx*depth);

	if (ymirror)
		{
		if (!(lpLine = PixmapPtr(lpPixmap, PMT_EDIT, sx, sy1, YES)))
			{
			FrameError(IDS_EFRAMEREAD);
			goto Exit;
			}
		if (lpMask)
			{
			MaskLoad( lpMask, sx+xMaskOffset, sy1+yMaskOffset, dx, lpMaskBuf );
			(*lpProcessProc)(lpLine, lpBuffer[1], lpMaskBuf, dx);
			}
		else
			copy(lpBuffer[1], lpLine, dx*depth);
		sy2--;
		}
	else
		sy2++;
	sy1++;
	}
	bRet = TRUE;
Exit:
	ProgressEnd();
	if (lpMaskBuf)
		FreeUp(lpMaskBuf);
	FreeUp(lpBuffer[0]);
	return(bRet);
}

/************************************************************************/
BOOL New( int pix, int lin, int iResolution, int iDepth )
/************************************************************************/
{
LPFRAME	lpFrame;

if ( !(lpFrame = FrameOpen( (FRMDATATYPE)iDepth, pix, lin, iResolution )) )
	{
	FrameError( IDS_EIMAGEOPEN );
	return( FALSE );
	}

if (FrameType(lpFrame) == FDT_CMYKCOLOR)
	FrameSetBackground(lpFrame, 0);
else
	FrameSetBackground(lpFrame, 255);

/* Setup the new image and bring up the new image window */
if ( !NewImageWindow(
	NULL,		// lpOldFrame
	NULL,		// Name
	lpFrame, 	// lpNewFrame
	IDN_FIRSTFILETYPE, // lpImage->FileType
	(iDepth == 0 ? IDC_SAVELA :
	(iDepth == 1 ? IDC_SAVECT :
	(iDepth == 3 ? IDC_SAVE24BITCOLOR :
	IDC_SAVE32BITCOLOR))), // lpImage->DataType
	FALSE,		// New view?
	IMG_DOCUMENT,	// lpImage->DocumentType
	NULL,		// lpImage->ImageName
	MAYBE
	) )
	return( FALSE );

return( TRUE );
}

/************************************************************************/
LPFRAME CALLBACK EXPORT SizeFrame (LPFRAME lpSrcFrame, int iWidth,
  int iHeight, int iRes, BOOL bSmartSize)
/************************************************************************/
{
LPFRAME lpFrame;

if (iWidth == FrameXSize(lpSrcFrame) && iHeight == FrameYSize(lpSrcFrame))
	{
	lpFrame = FrameCopy(lpSrcFrame, NULL);
	lpFrame->Resolution = iRes;
	}
else
if ( bSmartSize && iWidth > 2 && iHeight > 2 )
	lpFrame = DoSmartSize(lpSrcFrame, iWidth, iHeight, iRes);
else
	lpFrame = DoSize(lpSrcFrame, iWidth, iHeight, iRes);
return(lpFrame);
}




/************************************************************************/
LPFRAME CALLBACK EXPORT ExpandFrame (LPFRAME lpSrcFrame, int pix, int lin,
  int left, int top, BYTE Background)
/************************************************************************/
{
int	depth, srcpix, srclin, sy, dy;
WORD wBytes;
LPTR	lpSrc, lpDst;
LPFRAME	lpFrame;
RGBS Pixel;
int y;
int Height;

srcpix = FrameXSize(lpSrcFrame);
srclin = FrameYSize(lpSrcFrame);

ProgressBegin(1,0);
if ( !(lpFrame = FrameOpen( FrameType(lpSrcFrame), pix, lin,
							FrameResolution(lpSrcFrame) )) )
	{
	FrameError( IDS_EPROCESSOPEN );
	ProgressEnd();
	return( NULL );
	}
FrameSetBackground(lpFrame, Background);

depth = FrameDepth(lpFrame);
if (!depth)
	depth = 1;

    Height = FrameYSize (lpFrame);
    Pixel.red   = 255;
    Pixel.green = 255;
    Pixel.blue  = 255;

// Clear this Frame 
    for (y=0;y<Height;y++)
    {
        lpDst = FramePointer (lpFrame, 0, y, TRUE);
        FrameSetRGB (lpFrame, &Pixel, lpDst, pix);
    }

wBytes = (WORD)srcpix * (WORD)depth;
for (sy = 0, dy = top; sy < srclin; ++sy, ++dy)
	{
	if (AstralClockCursor( sy, srclin, YES ))
		goto ErrorExit;
	lpSrc = FramePointer(lpSrcFrame, 0, sy, NO);
	if (!lpSrc)
		{
		FrameError(IDS_EFRAMEREAD);
		goto ErrorExit;
		}
	lpDst = FramePointer(lpFrame, left, dy, YES);
	if (!lpDst)
		{
		FrameError(IDS_EFRAMEREAD);
		goto ErrorExit;
		}
	copy(lpSrc, lpDst, wBytes);
	}

ProgressEnd();
return( lpFrame );

ErrorExit:
FrameClose( lpFrame );
ProgressEnd();
return( NULL );
}


/************************************************************************/
LPFRAME DoSize(LPFRAME lpSrcFrame, int pix, int lin, int res)
/************************************************************************/
{
int	yline, ylast, depth, srcpix, srclin;
LPTR	lpBuffer;
LFIXED	yrate, yoffset;
int	sy;
LPFRAME	lpFrame;

srcpix = FrameXSize(lpSrcFrame);
srclin = FrameYSize(lpSrcFrame);
yrate = FGET( srclin, lin );
yoffset = (long)yrate>>1;

ProgressBegin(1,0);
if ( !(lpFrame = FrameOpen( FrameType(lpSrcFrame), pix, lin, res )) )
	{
	FrameError( IDS_EPROCESSOPEN );
	ProgressEnd();
	return( NULL );
	}

depth = FrameDepth(lpFrame);
if (!depth)
	depth = 1;
if ( !(AllocLines( &lpBuffer, 1, pix, depth )) )
	{
	FrameClose( lpFrame );
	Message( IDS_EMEMALLOC );
	ProgressEnd();
	return( NULL );
	}

ylast = -1;
for ( sy=0; sy<lin; sy++ )
	{
	if (AstralClockCursor( sy, lin, YES ))
		goto ErrorExit;
#ifdef WIN32
	yline = WHOLE( yoffset );
#else
	yline = HIWORD( yoffset );
#endif
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;
		if ( !FrameRead( lpSrcFrame, 0, yline, srcpix, lpBuffer, pix) )
			{
			FrameError(IDS_EFRAMEREAD);
			goto ErrorExit;
			}
		}
	if ( !FrameWrite( lpFrame, 0, sy, pix, lpBuffer, pix) )
		{
		FrameError(IDS_EFRAMEREAD);
		goto ErrorExit;
		}
	}

FreeUp( lpBuffer );
ProgressEnd();
return( lpFrame );

ErrorExit:
FrameClose( lpFrame );
FreeUp( lpBuffer );
ProgressEnd();
return( NULL );
}




