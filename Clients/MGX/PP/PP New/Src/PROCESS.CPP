//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

static BOOL MirrorObj(LPOBJECT lpObject, LPMASK lpMask,
				BOOL xmirror, BOOL ymirror, LPRECT lpRect);
static BOOL MirrorPixmap(LPPIXMAP lpPixmap, LPRECT lpRect,
					LPMASK lpMask, int xOffset, int yOffset,
					BOOL xmirror, BOOL ymirror);

typedef struct
{
	LPFRAME lpDataFrame;
	LPFRAME lpAlphaFrame;
} SIZEDATA, FAR *LPSIZEDATA;

/************************************************************************/
BOOL SizeImage(LPIMAGE lpImage, LPSIZE_PARMS lpParms)
/************************************************************************/
{
	int pix, lin, res, i;
	int nObjects, nPasses;
	LPOBJECT lpBase, lpObject;
	LPALPHA lpAlpha;
	RECT rAlpha;
	LFIXED xrate, yrate;
	LPSIZEDATA lpSizeData;

	if (!lpImage)
		return(FALSE);
	lpBase = ImgGetBase(lpImage);
	lpAlpha = lpBase->lpAlpha;
	res = lpParms->Resolution;
	if (lpParms->MaintainSize)
	{
		ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
		if ( !ImgEditInit(lpImage, ET_OBJECT, UT_RESOLUTION, lpBase))
		{
			ProgressEnd();
			return(FALSE);
		}
		FrameSetResolution(lpBase->Pixmap.EditFrame, res);
		ImgEditedObject(lpImage, lpBase, lpParms->Common.idDirty, NULL);
		ProgressEnd();
	}
	else
	{
		pix = FMUL( res, lpParms->Width );
		lin = FMUL( res, lpParms->Height );
		if (lpAlpha && !MaskSizeChange(lpAlpha, pix, lin))
			{
			Message(IDS_EMEMALLOC);
			return(FALSE);
			}

		nObjects = nPasses = 0;
		lpObject = NULL;
		while (lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE))
		{
			++nObjects;
			++nPasses;
			if (lpObject->lpAlpha)
				++nPasses;
		}

		lpSizeData = (LPSIZEDATA)AllocX(sizeof(SIZEDATA)*nObjects, GMEM_ZEROINIT);
		if (!lpSizeData)
		{
			Message(IDS_EMEMALLOC);
			return(FALSE);
		}
		ProgressBegin(nPasses, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
		if ( !ImgEditInit(lpImage, ET_ALLOBJECTS, UT_NEWDATA|UT_NEWALPHA|UT_OBJECTRECT,
					 	lpBase))
		{
			FreeUp(lpSizeData);
			ProgressEnd();
			return(FALSE);
		}
		xrate = FGET( pix, FrameXSize(ObjGetEditFrame(lpBase)) );
		yrate = FGET( lin, FrameYSize(ObjGetEditFrame(lpBase)) );
		lpObject = NULL;
		i = 0;
		while (lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE))
		{
			pix = FMUL(FrameXSize(ObjGetEditFrame(lpObject)), xrate);
			lin = FMUL(FrameYSize(ObjGetEditFrame(lpObject)), yrate);

			lpAlpha = ObjGetAlpha(lpObject);
			if (lpSizeData[i].lpDataFrame = SizeFrame(ObjGetEditFrame(lpObject), pix, lin, res,
										lpParms->SmartSize))
			{
				if (lpAlpha)
					lpSizeData[i].lpAlphaFrame = SizeFrame(
										AlphaGetEditFrame(lpAlpha),
										pix, lin, res, lpParms->SmartSize);
			}

			if ( !lpSizeData[i].lpDataFrame ||
				(lpAlpha && !lpSizeData[i].lpAlphaFrame))
			{
				while (i >= 0)
				{
					if (lpSizeData[i].lpDataFrame)
						FrameClose(lpSizeData[i].lpDataFrame);
					if (lpSizeData[i].lpAlphaFrame)
						FrameClose(lpSizeData[i].lpAlphaFrame);
					--i;
				}
				FreeUp(lpSizeData);
				ProgressEnd();
				return( FALSE );
			}

			++i;
		}

		lpObject = NULL;
		i = 0;
		while (lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE))
		{
			// scale the object rectangle
			lpObject->rObject.left = FMUL(lpObject->rObject.left, xrate);
			lpObject->rObject.top = FMUL(lpObject->rObject.top, yrate);

			lpObject->rObject.right = lpObject->rObject.left + FrameXSize(lpSizeData[i].lpDataFrame) - 1;
			lpObject->rObject.bottom = lpObject->rObject.top + FrameYSize(lpSizeData[i].lpDataFrame) - 1;

			/* Setup the new image and bring up the new image window */
			ImgEditedObjectFrame(lpImage, lpObject, lpParms->Common.idDirty, NULL,
								lpSizeData[i].lpDataFrame,
								lpSizeData[i].lpAlphaFrame);
			// fix mask rectangle for base object if necessary
			lpAlpha = ObjGetAlpha(lpObject);
			if (lpObject == lpBase && lpAlpha)
				MaskRectUpdate(lpAlpha, &rAlpha);
			++i;
		}
		FreeUp(lpSizeData);
		lpParms->Common.UpdateType = UT_SIZE;
		ProgressEnd();
	}
	return( TRUE );
}

/************************************************************************/
LPFRAME SizeFrame(LPFRAME lpSrcFrame, int iWidth, int iHeight, int iRes,
					BOOL bSmartSize)
/************************************************************************/
{
LPFRAME lpFrame;
FRMDATATYPE Type;

Type = FrameType(lpSrcFrame);
if (iWidth == FrameXSize(lpSrcFrame) && iHeight == FrameYSize(lpSrcFrame))
	{
	lpFrame = FrameCopy(lpSrcFrame, NULL);
	lpFrame->Resolution = iRes;
	}
else
if ( bSmartSize && iWidth > 2 && iHeight > 2 && CANDOTRANSPARENCY(Type))
	lpFrame = DoSmartSize(lpSrcFrame, iWidth, iHeight, iRes);
else
	lpFrame = DoSize(lpSrcFrame, iWidth, iHeight, iRes);
return(lpFrame);
}

/************************************************************************/
BOOL ExpandImage(LPIMAGE lpImage, LPEXPAND_PARMS lpParms)
/************************************************************************/
{
int pix, lin, res, left, top, opix, olin;
LPFRAME lpDataFrame;
LPFRAME lpAlphaFrame;
LPOBJECT lpBase, lpObject;
LPALPHA lpAlpha;
RECT rAlpha;
COLORINFO black;
FRMTYPEINFO TypeInfo;

if (!lpImage)
	return(FALSE);

lpBase = ImgGetBase(lpImage);
lpAlpha = lpBase->lpAlpha;

res = FrameResolution(lpBase->Pixmap.EditFrame);
ImgGetInfo(lpImage, &opix, &olin, NULL, NULL);
if (lpParms->fUseRightBottom)
	{
	left = FMUL( res, lpParms->Left );
	top = FMUL( res, lpParms->Top );
	pix = opix + left + FMUL( res, lpParms->Right );
	lin = olin + top + FMUL( res, lpParms->Bottom );
	}
else
	{
	pix = FMUL( res, lpParms->Width );
	lin = FMUL( res, lpParms->Height );
	left = FMUL( res, lpParms->Left );
	top = FMUL( res, lpParms->Top );
	}

// never make a dimension smaller
if (pix < opix)
	pix = opix;
if (lin < olin)
	lin = olin;
// make sure we have something to do
if ((pix <= opix) && (lin <= olin))
	return(FALSE);

if (lpAlpha && !MaskSizeChange(lpAlpha, pix, lin))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}

ProgressBegin(lpAlpha ? 2 : 1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if ( !ImgEditInit(lpImage, ET_OBJECT,
			 UT_NEWDATA|UT_OBJECTRECT|UT_MOVEOBJECTS|UT_NEWALPHA, lpBase))
	{
	ProgressEnd();
	return(FALSE);
	}

ImgGetTypeInfo(lpImage, &TypeInfo);
GetColorFromTypeInfo(TypeInfo, &lpParms->Color);
lpAlphaFrame = NULL;
if (lpDataFrame = ExpandFrame(lpBase->Pixmap.EditFrame, pix, lin,
							left, top, &lpParms->Color))
	{
	if (lpAlpha)
		{
		black.gray = 0;
		SetColorInfo(&black, &black, CS_GRAY);
		lpAlphaFrame = ExpandFrame(lpAlpha->Pixmap.EditFrame, pix, lin,
								left, top, &black);
		}
	}

if ( !lpDataFrame || (lpAlpha && !lpAlphaFrame))
	{
	if (lpDataFrame)
		FrameClose(lpDataFrame);
	if (lpAlphaFrame)
		FrameClose(lpAlphaFrame);
	ProgressEnd();
	return( FALSE );
	}

// modify the object rectangles of all floating objects
lpObject = lpBase;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	OffsetRect(&lpObject->rObject, left, top);

// setup changed base
ImgEditedObjectFrame(lpImage, lpBase, lpParms->Common.idDirty, NULL, lpDataFrame,
					 lpAlphaFrame);

// fix object rectangle and mask rectangle if necessary
lpBase->rObject.right = FrameXSize(lpDataFrame)-1;
lpBase->rObject.bottom = FrameYSize(lpDataFrame)-1;
if (lpAlpha)
	MaskRectUpdate(lpAlpha, &rAlpha);
lpParms->Common.UpdateType = UT_SIZE;
ProgressEnd();
return( TRUE );
}

/************************************************************************/
LPFRAME ExpandFrame(LPFRAME lpSrcFrame, int pix, int lin, int left, int top, 
					LPCOLORINFO lpBackColor)
/************************************************************************/
{
	int	depth, srcpix, srclin, sy, dy, yTotal, yProcessed;
	WORD wBytes, wLeftBytes;
	LPTR	lpSrc, lpDst;
	LPFRAME	lpFrame;
	FRMTYPEINFO TypeInfo;
	long Color;
	BOOL fFill = FALSE;

	srcpix = FrameXSize(lpSrcFrame);
	srclin = FrameYSize(lpSrcFrame);

	ProgressBegin(1,0);
	FrameGetTypeInfo(lpSrcFrame, &TypeInfo);
	if ( !(lpFrame = FrameOpen( TypeInfo, pix, lin,
								FrameResolution(lpSrcFrame) )) )
	{
		FrameError( IDS_EPROCESSOPEN );
		ProgressEnd();
		return( NULL );
	}
	if (TypeInfo.DataType == FDT_CMYKCOLOR)
	{
		CMYKS cmyk;

		cmyk = lpBackColor->cmyk;
		if (cmyk.c == 0 && cmyk.m == 0 && cmyk.y == 0 && cmyk.k == 0)
			FrameSetBackground(lpFrame, 0);
		else
		if (cmyk.c == 255 && cmyk.m == 255 && cmyk.y == 255 && cmyk.k == 255)
			FrameSetBackground(lpFrame, 255);
		else
		{
			fFill = TRUE;
			CopyCMYK( &cmyk, &Color );
		}
	}
	else if (TypeInfo.DataType == FDT_RGBCOLOR)
	{
		RGBS rgb;

		rgb = lpBackColor->rgb;
		if (rgb.red == 0 && rgb.green == 0 && rgb.blue == 0)
			FrameSetBackground(lpFrame, 0);
		else
		if (rgb.red == 255 && rgb.green == 255 && rgb.blue == 255)
			FrameSetBackground(lpFrame, 255);
		else
		{
			fFill = TRUE;
			CopyRGB( &rgb, &Color );
		}
	}
	else if (TypeInfo.DataType == FDT_PALETTECOLOR)
   {
	 	fFill = TRUE;
      Color = FrameGetNearestIndex(TypeInfo.ColorMap, &lpBackColor->rgb);
   }
	else // FDT_LINEART || FDT_GRAYSCALE
	{
		BYTE gray;

		gray = lpBackColor->gray;
		if (TypeInfo.DataType == FDT_LINEART)
		{
			if (gray > 127)
				gray = 255;
			else
				gray = 0;
		}
		if (gray == 0)
			FrameSetBackground(lpFrame, 0);
		else
		if (gray == 255)
			FrameSetBackground(lpFrame, 255);
		else
		{
			fFill = TRUE;
			Color = gray;
		}
	}

	depth = FrameDepth(lpFrame);
	if (!depth)
		depth = 1;

	wBytes = (WORD)srcpix * (WORD)depth;
	wLeftBytes = (WORD)left * (WORD)depth;
	yTotal = srclin;
	yProcessed = 0;
	if (fFill)
	{
		yTotal = lin;
		for (dy = 0; dy < top; ++dy)
		{
			if (AstralClockCursor( yProcessed, yTotal, YES ))
				goto ErrorExit;
			lpDst = FramePointer(lpFrame, 0, dy, YES);
			if (!lpDst)
			{
				FrameError(IDS_EFRAMEREAD);
				goto ErrorExit;
			}
			FrameSetPixel(lpFrame, Color, lpDst, pix);
			++yProcessed;
		}
	}
	for (sy = 0, dy = top; sy < srclin; ++sy, ++dy)
	{
		if (AstralClockCursor( yProcessed, yTotal, YES ))
			goto ErrorExit;
		lpSrc = FramePointer(lpSrcFrame, 0, sy, NO);
		if (!lpSrc)
		{
			FrameError(IDS_EFRAMEREAD);
			goto ErrorExit;
		}
		lpDst = FramePointer(lpFrame, 0, dy, YES);
		if (!lpDst)
		{
			FrameError(IDS_EFRAMEREAD);
			goto ErrorExit;
		}
		if (fFill)
			FrameSetPixel(lpFrame, Color, lpDst, pix);

		lpDst += wLeftBytes;
		copy(lpSrc, lpDst, wBytes);
		++yProcessed;
	}

	if (fFill)
	{
		for (; dy < lin; ++dy)
		{
			if (AstralClockCursor( yProcessed, yTotal, YES ))
				goto ErrorExit;
			lpDst = FramePointer(lpFrame, 0, dy, YES);
			if (lpDst)
				FrameSetPixel(lpFrame, Color, lpDst, pix);
			++yProcessed;
		}
	}
	ProgressEnd();
	return( lpFrame );

	ErrorExit:
	FrameClose( lpFrame );
	ProgressEnd();
	return( NULL );
}

/************************************************************************/
BOOL MirrorImage(LPIMAGE lpImage, LPMIRROR_PARMS lpParms)
/************************************************************************/
{
RECT     rEdit, UpdateRect;
LPMASK   lpMask;
LPOBJECT lpObject;
BOOL fChanged = NO;

if (!lpImage)
	return(fChanged);

ProgressBegin(ImgCountSelObjects(lpImage, NULL),
			lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if (!ImgEditInit(lpImage, ET_SELOBJECTS, UT_DATA|UT_ALPHA, NULL))
	{
	ProgressEnd();
	return(FALSE);
	}

AstralSetRectEmpty(&UpdateRect);
lpMask = ImgGetMask(lpImage);
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if ( !MirrorObj(lpObject, lpMask, lpParms->xmirror, lpParms->ymirror, &rEdit))
		break;
	ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, &rEdit);
	fChanged = YES;
   AstralUnionRect(&UpdateRect, &UpdateRect, &rEdit);
	}
if (fChanged)
   {
   lpParms->Common.UpdateType = UT_AREA;
   lpParms->Common.rUpdateArea = UpdateRect;
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
FRMTYPEINFO TypeInfo;

lpFrame = lpPixmap->EditFrame;
FrameGetTypeInfo(lpFrame, &TypeInfo);
switch ( TypeInfo.DataType )
    {
    case FDT_LINEART:
    case FDT_GRAYSCALE:
	case FDT_PALETTECOLOR:
	depth = 1;
	lpMirrorProc = mirror8;
	break;
    case FDT_RGBCOLOR:
	depth = 3;
	lpMirrorProc = mirror24;
	break;
    case FDT_CMYKCOLOR:
	depth = 4;
	lpMirrorProc = mirror32;
	break;
    default:
	return( FALSE );
    }
lpProcessProc = GetProcessProc(MM_NORMAL, TypeInfo.DataType);

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
LPFRAME DoSize(LPFRAME lpSrcFrame, int pix, int lin, int res)
/************************************************************************/
{
int	yline, ylast, depth, srcpix, srclin;
LPTR	lpBuffer;
LFIXED	yrate, yoffset;
int	sy;
LPFRAME	lpFrame;
FRMTYPEINFO TypeInfo;

srcpix = FrameXSize(lpSrcFrame);
srclin = FrameYSize(lpSrcFrame);
yrate = FGET( srclin, lin );
yoffset = FDIV2(yrate);

ProgressBegin(1,0);
FrameGetTypeInfo(lpSrcFrame, &TypeInfo);
if ( !(lpFrame = FrameOpen( TypeInfo, pix, lin, res )) )
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
	yline = WHOLE(yoffset);
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


/************************************************************************/
LPFRAME DoSmartSize(LPFRAME lpSrcFrame, int pix, int lin, int res)
/************************************************************************/
{
int	yline, depth, srcpix, srclin;
LPTR	lpBuffer, lp;
int	sy;
LPFRAME	lpFrame = NULL;
BOOL	fNeedNewLine;
LPVOID lpSmart;
FRMTYPEINFO TypeInfo;

srcpix = FrameXSize(lpSrcFrame);
srclin = FrameYSize(lpSrcFrame);
yline = 0;

ProgressBegin(1,0);
FrameGetTypeInfo(lpSrcFrame, &TypeInfo);
if ( !(lpFrame = FrameOpen( TypeInfo, pix, lin, res )) )
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

if ( !(lpSmart = SmartSetup( srcpix, pix, srclin, lin, depth ) ))
	{
	FrameClose( lpFrame );
	FreeUp( lpBuffer );
	ProgressEnd();
	return( NULL );
	}

for ( sy=0; sy<lin; )
	{
	if (AstralClockCursor( sy, lin, YES ))
		goto ErrorExit;

	if ( !(lp = FramePointer( lpSrcFrame, 0, yline, NO)) )
		{
		FrameError(IDS_EFRAMEREAD);
		goto ErrorExit;
		}
	if ( SmartSize(lpSmart, lp, lpBuffer, &fNeedNewLine ) )
		{
		if ( !FrameWrite( lpFrame, 0, sy, pix, lpBuffer, pix) )
			{
			FrameError(IDS_EFRAMEREAD);
			goto ErrorExit;
			}
		sy++;
		}
	if ( fNeedNewLine )
		yline += 1;
	}

SmartDone(lpSmart);
FreeUp( lpBuffer );
ProgressEnd();
return( lpFrame );

ErrorExit:

SmartDone(lpSmart);
FrameClose( lpFrame );
FreeUp( lpBuffer );
ProgressEnd();
return( NULL );
}

/************************************************************************/
BOOL NewImage( LPIMAGE lpImage, LPNEW_PARMS lpParms )
/************************************************************************/
{
	int	pix, lin, y;
	LPFRAME	lpFrame;
	ITEMID DataType;
	LPNEWIMAGEINFO lpInfo;
	FRMTYPEINFO TypeInfo;
	long Color;
	LPTR lpDst;
	BOOL fFill = FALSE;

	lpInfo = (LPNEWIMAGEINFO)Alloc(sizeof(NEWIMAGEINFO));
	if (!lpInfo)
		{
		Message(IDS_EMEMALLOC);
		return(FALSE);
		}
	clr((LPTR)lpInfo, sizeof(NEWIMAGEINFO));

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	pix = FMUL( lpParms->Resolution, lpParms->Width );
	lin = FMUL( lpParms->Resolution, lpParms->Height );
	FrameSetTypeInfo(&TypeInfo, lpParms->Type, NULL, &lpParms->CmsInfo.dst.ptInfo);
	if ( !(lpFrame = FrameOpen( TypeInfo, pix, lin, lpParms->Resolution )) )
		{
		ProgressEnd();
		FrameError( IDS_EIMAGEOPEN );
		FreeUp(lpInfo);
		return( FALSE );
		}

	if (TypeInfo.DataType == FDT_CMYKCOLOR)
	{
		CMYKS cmyk;

		cmyk = lpParms->Color.cmyk;
		if (cmyk.c == 0 && cmyk.m == 0 && cmyk.y == 0 && cmyk.k == 0)
			FrameSetBackground(lpFrame, 0);
		else
		if (cmyk.c == 255 && cmyk.m == 255 && cmyk.y == 255 && cmyk.k == 255)
			FrameSetBackground(lpFrame, 255);
		else
		{
			fFill = TRUE;
			CopyCMYK( &cmyk, &Color );
		}
	}
	else
	if (TypeInfo.DataType == FDT_RGBCOLOR)
	{
		RGBS rgb;

		rgb = lpParms->Color.rgb;
		if (rgb.red == 0 && rgb.green == 0 && rgb.blue == 0)
			FrameSetBackground(lpFrame, 0);
		else
		if (rgb.red == 255 && rgb.green == 255 && rgb.blue == 255)
			FrameSetBackground(lpFrame, 255);
		else
		{
			fFill = TRUE;
			CopyRGB( &rgb, &Color );
		}
	}
	else // FDT_LINEART || FDT_GRAYSCALE
	{
		BYTE gray;

		gray = lpParms->Color.gray;
		if (TypeInfo.DataType == FDT_LINEART)
		{
			if (gray > 127)
				gray = 255;
			else
				gray = 0;
		}
		if (gray == 0)
			FrameSetBackground(lpFrame, 0);
		else
		if (gray == 255)
			FrameSetBackground(lpFrame, 255);
		else
		{
			fFill = TRUE;
			Color = gray;
		}
	}

	if (fFill)
	{
		for (y = 0; y < lin; ++y)
		{
			if (AstralClockCursor(y, lin, TRUE))
			{
				FrameClose(lpFrame);
				FreeUp(lpInfo);
				return(FALSE);
			}
			lpDst = FramePointer(lpFrame, 0, y, TRUE);
			if (lpDst)
				FrameSetPixel(lpFrame, Color, lpDst, pix);
		}
	}

	switch (TypeInfo.DataType)
	{
		case FDT_LINEART:
			DataType = IDC_SAVELA;
			break;
		case FDT_RGBCOLOR:
			DataType = IDC_SAVE24BITCOLOR;
			break;
		case FDT_CMYKCOLOR:
			DataType = IDC_SAVE32BITCOLOR;
			break;
		case FDT_GRAYSCALE:
		default:
			DataType = IDC_SAVECT;
			break;
	}
	lpImage = CreateImage(	NULL,		// Name
							lpFrame, 	// lpNewFrame
							NULL,		// new base object
							lpParms->lpCmdList,	// command list
							Control.DefaultFileType,	// lpImage->FileType
							DataType, // lpImage->DataType
							IMG_DOCUMENT,	// lpImage->DocumentType
							NULL);		// lpImage->ImageName
	ProgressEnd();
	if (lpImage)
	{
		lpInfo->lpImage = lpImage;
		ListAddTail(&lpParms->Common.NewImageList, lpInfo);
		lpParms->Common.UpdateType = UT_NEWIMAGE;
		return(TRUE);
	}
	FreeUp(lpInfo);
	return(FALSE);
}


