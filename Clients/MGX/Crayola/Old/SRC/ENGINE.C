//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static BOOL LineProcessObj(LPIMAGE lpImage, LPOBJECT lpObject,
	LPENGINE lpEngine, LPMASK lpMask);
static BOOL ReadProcessObj(LPIMAGE lpImage, LPOBJECT lpObject,
	BOOL fReadAlpha, LPRECT lpRect, LPREADDATAPROC lpReadDataProc);

typedef BOOL (*LPGETSOURCEPROC)(LPPIXMAP, PIXMAP_TYPE,
			int, int, int, int, LPTR,
			LPRECT,  BOOL, BOOL);


//************************************************************************
//	Calls the ReadEngine on all selected objects.
//	The read engine allows the lpReadDataProc to look at all the data,
//	without modifiying it.
//************************************************************************
BOOL ReadEngineSelObj(LPIMAGE lpImage, LPRECT lpEditRect, LPREADDATAPROC lpReadDataProc)
//************************************************************************
{
RECT rEdit, rClip;
LPOBJECT lpObject;
int nNumPasses = 0;

if ( !lpImage )
	return( FALSE );

ImgGetMaskRect( lpImage, &rClip );
if ( lpEditRect )
	{
	if ( !AstralIntersectRect( &rClip, &rClip, lpEditRect ) )
		return( FALSE );
	}

// Count the number of passes
lpObject = NULL;
while ( lpObject = ImgGetSelObject( lpImage, lpObject ) )
	++nNumPasses;

// Process each object
ProgressBegin( nNumPasses, 0 );
lpObject = NULL;
while ( lpObject = ImgGetSelObject( lpImage, lpObject ) )
	{
	if ( !AstralIntersectRect( &rEdit, &rClip, &lpObject->rObject ) )
		continue;
	ReadProcessObj( lpImage, lpObject, FALSE, &rEdit, lpReadDataProc );
	}
ProgressEnd();

return( TRUE );
}


//************************************************************************
//	The read engine allows the lpReadDataProc to look at all the data, 
//		without modifiying it.
//************************************************************************
static BOOL ReadProcessObj(LPIMAGE lpImage, LPOBJECT lpObject,
	BOOL fReadAlpha, LPRECT lpRect, LPREADDATAPROC lpReadDataProc)
//************************************************************************
{
RECT rObject;
int y, depth;
LPTR lpSrc;
LPPIXMAP lpPixmap;
FRMDATATYPE DataType;

if ( !lpReadDataProc )
	return( FALSE );
if ( !lpRect )
	return( FALSE );

if (fReadAlpha)
		lpPixmap = &lpObject->lpAlpha->Pixmap;
else	lpPixmap = &lpObject->Pixmap;

PixmapGetInfo( lpPixmap, PMT_EDIT, NULL, NULL, &depth, &DataType );
rObject = lpObject->rObject;

ProgressBegin(1,0);
for ( y = lpRect->top; y <= lpRect->bottom; ++y )
	{
	AstralClockCursor( y-lpRect->top, RectHeight(lpRect), NO );
	if ( lpSrc = PixmapPtr( lpPixmap, PMT_EDIT,
		lpRect->left - rObject.left, y - rObject.top, YES ) )
			(*lpReadDataProc)( y, lpRect->left, lpRect->right, lpSrc, depth );
	}
ProgressEnd();

return( TRUE );
}


//************************************************************************
void SetEngineDef(LPENGINE lpEngine)
//************************************************************************
{
	lpEngine->fEditAlpha	= NO;
	lpEngine->lpEditRect 	= NULL; 
	lpEngine->lpColor 		= NULL; 	
	lpEngine->lpSrcFrame 	= NULL;
	lpEngine->FlipHorz 		= NO; 	
	lpEngine->FlipVert 		= NO; 	
	lpEngine->lpDataProc 	= NULL; 
	lpEngine->lpMaskProc 	= NULL; 
	lpEngine->Opacity 		= 255; 	
	lpEngine->MergeMode 	= MM_NORMAL; 	
	lpEngine->MaskType 		= MT_BASE; 	
	lpEngine->ScatterPressure = 255;
	lpEngine->fNoUndo		= FALSE;
}

//************************************************************************
//************************************************************************
void SetEngineOld(LPENGINE lpEngine,BOOL fEditAlpha, LPRECT lpEditRect, LPCOLORINFO lpColor,LPFRAME lpSrcFrame, BOOL FlipHorz, BOOL FlipVert,LPDATAPROC lpDataProc,LPMASKPROC lpMaskProc,int Opacity, MERGE_MODE MergeMode)
//************************************************************************
{
	SetEngineDef(lpEngine);
	lpEngine->fEditAlpha	= fEditAlpha;
	lpEngine->lpEditRect 	= lpEditRect; 
	lpEngine->lpColor 		= lpColor; 	
	lpEngine->lpSrcFrame 	= lpSrcFrame;
	lpEngine->FlipHorz 		= FlipHorz; 	
	lpEngine->FlipVert 		= FlipVert; 	
	lpEngine->lpDataProc 	= lpDataProc; 
	lpEngine->lpMaskProc 	= lpMaskProc; 
	lpEngine->Opacity 		= Opacity; 	
	lpEngine->MergeMode 	= MergeMode;
}	

//************************************************************************
//************************************************************************
void SetEngineDraw(LPENGINE lpEngine,LPDATAPROC lpDataProc, int Opacity, MERGE_MODE MergeMode)
//************************************************************************
{
	SetEngineDef(lpEngine);
	lpEngine->lpDataProc	= lpDataProc;
	lpEngine->Opacity 		= Opacity;
	lpEngine->MergeMode 	= MergeMode;
}

//************************************************************************
//************************************************************************
void SetEngineColor(LPENGINE lpEngine,LPCOLORINFO lpColor, int Opacity, MERGE_MODE MergeMode)
//************************************************************************
{
	SetEngineDef(lpEngine);
	lpEngine->lpColor = 		lpColor;
	lpEngine->Opacity = 		Opacity;
	lpEngine->MergeMode = 	MergeMode;
}


//************************************************************************
BOOL LineEngineSelObj(LPIMAGE lpImage, LPENGINE lpEngine, int dirty)
//************************************************************************
{
RECT rEdit, rClip;
LPRECT lpOldEditRect;
LPOBJECT lpObject;
LPMASK lpMask = NULL;
BOOL fClip = NO;
BOOL fChanged = NO;
int nNumPasses = 0;

if (!lpImage)
	return(fChanged);

if (lpEngine->MaskType == MT_BASE && (lpMask = ImgGetMask(lpImage)))
	{
	ImgGetMaskRect(lpImage, &rClip);
	fClip = YES;
	}

if (lpEngine->lpEditRect)
	{
	if (fClip)
		{
		if (!AstralIntersectRect(&rClip, &rClip, lpEngine->lpEditRect))
			return(fChanged);
		}
	else
		rClip = *lpEngine->lpEditRect;
	fClip = YES;
	}
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (fClip)
		{
		if (!AstralIntersectRect(&rEdit, &rClip, &lpObject->rObject))
			continue;
		}
	++nNumPasses;
	}
ProgressBegin(nNumPasses, dirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if (!lpEngine->fNoUndo)
	{
	if (!ImgEditInit(lpImage, ET_SELOBJECTS,
					lpEngine->fEditAlpha ? UT_ALPHA : UT_DATA, NULL))
		{
		ProgressEnd();
		return(fChanged);
		}
	}
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (fClip)
		{
		if (!AstralIntersectRect(&rEdit, &rClip, &lpObject->rObject))
			continue;
		}
	else
		rEdit = lpObject->rObject;
	lpOldEditRect = lpEngine->lpEditRect;
	lpEngine->lpEditRect = &rEdit; 
	if (lpEngine->MaskType == MT_OBJECT)
		lpMask = lpObject->lpAlpha;
	if ( !LineProcessObj( lpImage, lpObject, lpEngine, lpMask ) )
		{
		lpEngine->lpEditRect = lpOldEditRect; 
		ImgEditRecover(lpImage);
		break;
		}
	lpEngine->lpEditRect = lpOldEditRect; 
	if (!lpEngine->fNoUndo)
		ImgEditedObject(lpImage, lpObject, dirty, &rEdit);
	UpdateImage(&rEdit, YES);
	}
ProgressEnd();
return(fChanged);
}


/************************************************************************/
BOOL LineEngineObj(LPIMAGE lpImage,	LPOBJECT lpObject, LPENGINE lpEngine, int dirty)
/************************************************************************/
{
RECT rEdit, rClip;
BOOL fChanged = NO;
LPMASK lpMask;
LPRECT lpOldEditRect;

if (!lpImage || !lpObject || (lpEngine->fEditAlpha && !lpObject->lpAlpha))
	return(fChanged);
if (lpEngine->MaskType == MT_BASE && (lpMask = ImgGetMask(lpImage)))
	{
	lpMask = ImgGetMask(lpImage);
	ImgGetMaskRect(lpImage, &rClip);
	if (!AstralIntersectRect(&rEdit, &rClip, &lpObject->rObject))
		return(fChanged);
	}
else // MaskType == MT_OBJECT || MaskType == MT_NONE
	{
	if (lpEngine->MaskType == MT_OBJECT)
		lpMask = lpObject->lpAlpha;
	else
		lpMask = NULL;
	rEdit = lpObject->rObject;
	}
if (lpEngine->lpEditRect)
	{
	if (!AstralIntersectRect(&rEdit, &rEdit, lpEngine->lpEditRect))
		return(fChanged);
	}
ProgressBegin(1, dirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if (!lpEngine->fNoUndo)
	{
	if (!ImgEditInit(lpImage, ET_OBJECT, lpEngine->fEditAlpha ? UT_ALPHA : UT_DATA,
				lpObject))
		{
		ProgressEnd();
		return(fChanged);
		}
	}
lpOldEditRect = lpEngine->lpEditRect;
lpEngine->lpEditRect = &rEdit; 
if ( LineProcessObj( lpImage, lpObject, lpEngine, lpMask ) )
	{
	if (!lpEngine->fNoUndo)
		ImgEditedObject(lpImage, lpObject, dirty, &rEdit);
	UpdateImage(&rEdit, YES);
	}
lpEngine->lpEditRect = lpOldEditRect; 
ProgressEnd();
return(fChanged);
}



/************************************************************************/
static BOOL LineProcessObj(LPIMAGE lpImage, LPOBJECT lpObject, LPENGINE lpEngine, LPMASK lpMask)
/************************************************************************/
{
RECT rObject;
LPPIXMAP lpPixmap;

rObject = lpObject->rObject;
if (lpEngine->fEditAlpha)
	lpPixmap = &lpObject->lpAlpha->Pixmap;
else
	lpPixmap = &lpObject->Pixmap;
return(LineProcessPixmap(lpPixmap, rObject.left, rObject.top, lpEngine, lpMask));
}

//************************************************************************
BOOL LineProcessPixmap(LPPIXMAP lpPixmap, int PixOffX, int PixOffY, LPENGINE lpEngine, LPMASK lpMask)
//************************************************************************
{
int y;
RECT rSource, rLines;
int w, h, depth;
BOOL UsingMask;
LPGETSOURCEPROC lpGetSourceData;// Proc to get source data
LPPROCESSPROC lpProcessProc;
FRMDATATYPE DataType;
PIXMAP srcPixmap;
LPMASKPROC lpMaskProc;
LPDATAPROC lpDataProc;
LPCOLORINFO lpColor;
int Opacity, xMaskOff, yMaskOff, ScatterPressure;
BOOL FlipHorz, FlipVert;
LPFRAME lpEditFrame;
LPTR lpDst;
BOOL bRet = FALSE;
LPTR lpSrc = NULL;
LPTR lpMsk = NULL;
LPINT lpErr = NULL;

ProgressBegin(1,0);

// Setup mask status
UsingMask = lpMask || (ColorMask.Mask && ColorMask.On);
if (lpEngine->MaskType == MT_OBJECT)
	{
	xMaskOff = -PixOffX;
	yMaskOff = -PixOffY;
	}
else
	xMaskOff = yMaskOff = 0;

// setup the area we are going to edit
if(lpEngine->lpEditRect)
	rLines = *lpEngine->lpEditRect;
else
	{
	PixmapGetInfo(lpPixmap, PMT_EDIT, &w, &h, NULL,NULL);
	rLines.left = PixOffX;
	rLines.top = PixOffX;
	rLines.right = PixOffX+w-1;
	rLines.bottom = PixOffX+h-1;
	}

// get width, height, and depth for operation
w = RectWidth(&rLines);
h = RectHeight(&rLines);
PixmapGetInfo(lpPixmap, PMT_EDIT, NULL, NULL, &depth, &DataType);

// allocate necessary buffers
if (!(lpMsk = Alloc((long)w)))
	goto Exit;
if (lpEngine->ScatterPressure < 255)
	{
	if (!(lpErr = (LPINT)Alloc((long)(w+1) * 2L)))
		goto Exit;
	clr((LPTR)lpErr, (w+1) * 2);
	}
if (!(lpSrc = Alloc((long)w * (long)depth)))
	goto Exit;
	
lpProcessProc = GetProcessProc(lpEngine->MergeMode, depth);

// get all values that are used in loop for faster access
lpDataProc = lpEngine->lpDataProc;
lpMaskProc = lpEngine->lpMaskProc;
lpColor = lpEngine->lpColor;
Opacity = lpEngine->Opacity;
FlipHorz = lpEngine->FlipHorz;
FlipVert = lpEngine->FlipVert;
lpEditFrame = lpPixmap->EditFrame;
ScatterPressure = lpEngine->ScatterPressure;

lpGetSourceData = NULL;
if (lpColor)
	LoadColor(lpEditFrame, lpSrc, w, lpColor);
else
if (lpEngine->lpSrcFrame)
	{
	lpGetSourceData = LoadTiledData;
	PixmapSetup(&srcPixmap, lpEngine->lpSrcFrame, YES);
	}

if (!UsingMask && !lpMaskProc && ScatterPressure >= 255)
	set(lpMsk, w, Opacity);

for (y = rLines.top; y <= rLines.bottom; ++y)
	{
	AstralClockCursor(y-rLines.top, h, NO);

	if (UsingMask) /* If DMASK being used */
		{
		// get lpDst here without bModify, just in case we
		// are editing the mask.
		if (!(lpDst = PixmapPtr(lpPixmap, PMT_EDIT,
				rLines.left-PixOffX, y-PixOffY, NO)))
			continue;
		mload( rLines.left+xMaskOff, y+yMaskOff, w, 1, lpMsk, lpDst,
					depth, lpMask );
		ScaleData8(lpMsk, w, (BYTE)Opacity);
		}
	else if (lpMaskProc || ScatterPressure < 255)
		set(lpMsk, w, Opacity);
	if (lpMaskProc)
		(*lpMaskProc)(y, rLines.left, rLines.right, lpMsk);

	if (ScatterPressure < 255)
		ScatterMask(lpMsk, lpMsk, lpErr, w, 1, ScatterPressure);

	if (!(lpDst = PixmapPtr(lpPixmap, PMT_EDIT,
			rLines.left-PixOffX, y-PixOffY, YES)))
		continue;

	if (lpGetSourceData)
		if (!(*lpGetSourceData)((LPPIXMAP)&srcPixmap, PMT_EDIT,
				rLines.left, y, w, 1,
				
				lpSrc, (LPRECT)&rSource, FlipHorz, FlipVert))
			continue;

	if (lpDataProc)
		(*lpDataProc)(y, rLines.left, rLines.right, lpDst, lpSrc, depth);
	(*lpProcessProc)(lpDst, lpSrc, lpMsk, w);
	if (lpColor && lpDataProc)
		LoadColor(lpEditFrame, lpSrc, w, lpColor);
	}
bRet = TRUE;
	
Exit:
FreeUp(lpMsk);
FreeUp((LPTR)lpErr);
FreeUp(lpSrc);
ProgressEnd();
return(bRet);
}


//************************************************************************
// 	Writes a line of data with mask&color shield into lpDstPix.
//	No merge mode or opacity.
//	lpDstPix is at offx, offy from the mask.
//************************************************************************
void WriteLine(LPTR lpSrcData, LPPIXMAP lpDstPix, LPMASK lpMask,int y, int offx, int offy)
//************************************************************************
{
	LPTR lpDstData, lpMaskData;	
	LPPROCESSPROC lpProcessProc;
	int nPixel, depth;
	
	
	PixmapGetInfo(lpDstPix, PMT_EDIT, &nPixel,NULL,&depth,NULL);
	lpDstData = PixmapPtr(lpDstPix, PMT_EDIT,0,y,YES);
	if (!lpDstData)
		return;
	lpProcessProc = GetProcessProc(MM_NORMAL, depth);
	lpMaskData = (LPTR)LineBuffer[0];
	MaskLoad(lpMask, offx, offy+y, nPixel, lpMaskData);
	(*lpProcessProc)(lpDstData, lpSrcData, lpMaskData, nPixel);
}

/************************************************************************/
void mload(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, int depth, LPMASK lpMask)
/************************************************************************/
{
switch(depth)
	{
	case 1 :
	mload8(xs, ys, dx, dy, pm, pd, lpMask);
	break;

	case 3 :
	mload24(xs, ys, dx, dy, pm, (LPRGB)pd, lpMask);
	break;

	case 4 :
	mload32(xs, ys, dx, dy, pm, (LPCMYK)pd, lpMask);
	break;
	}
}

/************************************************************************/
void mload8(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
LPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoad( lpMask, xs, y, dx, tpm );
if (ColorMask.Mask && ColorMask.On)
	Shield8(pm, pd, (WORD)dx * (WORD)dy);
}

/************************************************************************/
void mload24(int xs,int ys,int dx,int dy, LPTR pm, LPRGB pd, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
LPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoad( lpMask, xs, y, dx, tpm );
if (ColorMask.Mask && ColorMask.On)
	Shield24(pm, pd, (WORD)dx * (WORD)dy);
}

/************************************************************************/
void mload32(int xs,int ys,int dx,int dy, LPTR pm, LPCMYK pd, LPMASK lpMask)
/************************************************************************/
{
	int y, ye;
	LPTR tpm;

	ye = ys + dy - 1;
	for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
		MaskLoad( lpMask, xs, y, dx, tpm );

	if (ColorMask.Mask && ColorMask.On)
		Shield32(pm, pd, (WORD)dx * (WORD)dy);
}

#ifdef C_CODE
/************************************************************************/
#ifdef __cplusplus
extern "C"            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

void ScatterMask(
/************************************************************************/
LPTR 	lpSrc,
LPTR 	lpDst,
LPINT 	lpErrors,
int 	dx,
int		dy,
int 	iPressure)
{
int error, iCount;
LPINT lpError;
static ULONG lSeed = 0xDEADBABA;

clr( (LPTR)lpErrors, (dx+1)*2 );
while (--dy >= 0)
	{
	lpError = lpErrors;
	iCount = dx;
	while (--iCount >= 0)
		{
		error = iPressure;
		if ((error += *lpError) > 127 )
			{
			*lpDst++ = *lpSrc++;
			error -= 255;
			}
		else
			{
			*lpDst++ = 0;
			++lpSrc;
			}
		if ( lSeed & BIT18 )
			{
			lSeed += lSeed;
			lSeed ^= BITMASK;
			*lpError++ = 0;
			*lpError += error;
			}
		else	{
			lSeed += lSeed;
			*lpError++ = error;
			}
		}
	}
}
#endif

/************************************************************************/
void LoadColor(LPFRAME lpFrame, LPTR lpBuf, int iCount, LPCOLORINFO lpColor)
/************************************************************************/
{
	long lColor;

	switch(FrameType(lpFrame))
	{
		case FDT_LINEART   :
		case FDT_GRAYSCALE :
			lColor = lpColor->gray;
		break;

		case FDT_RGBCOLOR :
			lColor = 0;
			CopyRGB( &lpColor->rgb, &lColor );
		break;

		case FDT_CMYKCOLOR :
			CopyCMYK( &lpColor->cmyk, &lColor );
		break;
	}

	FrameSetPixel(lpFrame, lColor, lpBuf, iCount);
}

/***********************************************************************/
BOOL LoadSourceData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, int dx, int dy, LPTR lpData,
			LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip)
/***********************************************************************/
{
LPTR lp;
RECT rDest;
int width, cwidth, sx, sdx, sy, Xsize, Ysize, depth, ey, ex;
int left, iHeight;

// get pixmap info
PixmapGetInfo(lpPixmap, pmType, &Xsize, &Ysize, &depth, NULL);

// width = width of a line in buffer in bytes
// cwidth = number of bytes to copy to buffer
cwidth = width = dx * depth;

// Make sure requested area lies in frame
if (y >= Ysize)		// below image
	return(FALSE);
if ((ey = (y + dy - 1)) < 0)	// above image
	return(FALSE);
if (x >= Xsize)		// right of image
	return(FALSE);
if ((ex = (x + dx - 1)) < 0)	// left of image
	return(FALSE);

// save source x and y points
sx = x;
sy = y;

// check clipping for top of area to be read
if (y < 0)
	{ // advance in -y number of lines into data buffer
	lpData += (-y)*width;
	rDest.top = 0; // clip to top of frame
	}
else
	rDest.top = y; // way cool

// check clipping for bottom of area to be read
if (ey >= Ysize)
	rDest.bottom = Ysize-1; // clip to bottom of frame
else
	rDest.bottom = ey; // way cool

// check clipping for left side of area to be read
if (x < 0)
	{ // advance in -x number of pixels into data buffer
	sdx = (-x)*depth;
	lpData += sdx;
	cwidth -= sdx; // reduce copy width
	rDest.left = 0; // clip to left side of frame
	}
else
	rDest.left = x; // way cool

// check clipping for right side of area to be read
if (ex >= Xsize)
	{
	rDest.right = Xsize - 1; // clip to right side of frame
	cwidth -= (ex-rDest.right)*depth; // reduce copy width
	}
else
	rDest.right = ex; // way cool

// after all that god damn checking, copy the fricking data from the frame
y = rDest.top;
left = rDest.left;
iHeight = rDest.bottom - rDest.top + 1;
while (--iHeight >= 0)
	{
	if (lp = PixmapPtr(lpPixmap, pmType, left, y, NO))
		copy(lp, lpData, cwidth);
	lpData += width;
	++y;
	}

// Offset to make coordinates in rect relative to upper-left of buffer
*lpRect = rDest;
OffsetRect(lpRect, -sx, -sy);
return(TRUE);
}

/***********************************************************************/
BOOL LoadTiledData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, int dx, int dy, LPTR lpData,
			LPRECT lpRect,  BOOL HorzFlip, BOOL VertFlip)
/***********************************************************************/
{
LPTR lp;
int width, cwidth, awidth, iCount, nlines, Xsize, Ysize, ox, oy;
int xTile, yTile, depth2, depth, xpos, temp, pair_width, pair_height;
int ystart, yend, yinc, lines;

// get pixmap info
PixmapGetInfo(lpPixmap, pmType, &Xsize, &Ysize, &depth, NULL);

// width = width of a line in buffer in bytes
// cwidth = number of bytes to copy to buffer
cwidth = width = dx * depth;

// set up the return rectangle, we can always fill it up
lpRect->top = lpRect->left = 0;
lpRect->bottom = dy - 1; lpRect->right = dx - 1;

// save original x and y on image
ox = x;
oy = y;

// calculate the width and height of horizontally and
// vertically flipped tiles.  The flipped tiles don't
// use the last pixel or the last line so as to tile more
// naturally - so the theory goes...
// these width and heights include the even tile which is
// is not flipped and the odd tile which is flipped and
// is smaller by 1.
pair_width = (2*Xsize)-1; // width of an even and odd tile
pair_height = (2*Ysize)-1; // height of an even and odd tile

// precalculate depth * 2 for speed
depth2 = depth * 2;

// process all lines in for destination buffer
lines = dy;
while (lines > 0)
	{
	if (VertFlip)
		{
		// get position within two tile pair
		y = oy-((oy/pair_height)*pair_height);
		if (oy < 0)
			y += pair_height;
		// determine whether this is an even or odd file
		yTile = y/Ysize;
		}
	else 
		{
		if (oy < 0)
			y = (Ysize - ((-oy)%Ysize)) % Ysize;
		else
			y = oy%Ysize;
		}
	if (VertFlip && yTile & 1) // vertical flipping and
		{				// odd tile
		y -= Ysize;
		nlines = min(lines, (Ysize-y-1));
		ystart = Ysize-y-2;
		yend = ystart - nlines;
		yinc = -1;
		}
	else	// even tile - same for normal and vertical flip
		{
		nlines = min(lines, (Ysize-y));
		ystart = y;
		yend = y + nlines;
		yinc = 1;
		}
	lines -= nlines;	// decrement lines still to process
	oy += nlines;	// increment position on image
	for (y = ystart; y != yend; y += yinc)
		{
		xpos = ox;	// get position on image
		iCount = cwidth; // get amount of data to transfer
		while (iCount > 0)
			{
			if (HorzFlip)
				{
				// get position within two tile pair
				x = xpos-((xpos/pair_width)*pair_width);
				if (xpos < 0)
					x += pair_width;
				// determine whether this is an even or odd file
					xTile = x/Xsize;
				}
			else
				{
				if (xpos < 0)
					x = (Xsize - ((-xpos)%Xsize)) % Xsize;
				else
					x = xpos%Xsize;
				}
				if (HorzFlip && xTile & 1) // horz flip and
				{				// odd tile
				x -= Xsize;
				// get amount of data left in this tile
				awidth = min(iCount, (Xsize-x-1)*depth);
				// reduce total amount to transfer
				iCount -= awidth;
				
				// get pointer to cache data
				if (lp = PixmapPtr(lpPixmap, pmType, Xsize-x-2, y, NO))
					{
					// get number of pixels to move
					awidth /= depth;
					// increment position on image
					xpos += awidth;
					while (--awidth >= 0)
						{
						// move pixel by pixel going
						// backwards through source
							temp = depth;
							while (--temp >= 0)
								*lpData++ = *lp++;
							lp -= depth2;
							}
						}
					else
						{
					// no data - just advance pointers
					// and xpos
							lpData += awidth;
						xpos += (awidth/depth);
						}
				}
			else // even tile - same for normal and horz flip
				{
				// get amount of data left in this tile
				awidth = min(iCount, (Xsize-x)*depth);
				// get pointer to cache data
				if (lp = PixmapPtr(lpPixmap, pmType, x, y, NO))
					copy(lp, lpData, awidth);
				// adjust counts and pointers
				iCount -= awidth;
				lpData += awidth;
				xpos += (awidth/depth);
				}
			// always starts on a new tile
			x = 0;
			}
		}
	// always starts on a new tile
	y = 0;
	}
return(TRUE);
}

/***********************************************************************/
BOOL LoadMirroredData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, int dx, int dy, LPTR lpData,
			LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip)
/***********************************************************************/
{
LPTR lp;
int width, cwidth, sx, sdx, sy, Xsize, Ysize, depth, pixels, iy, yend;
void (far *lpMirrorProc)(LPTR,int);
RECT	mRect;

// get pixmap info
PixmapGetInfo(lpPixmap, pmType, &Xsize, &Ysize, &depth, NULL);

if (HorzFlip)
	{
	switch ( depth )
		{
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
	}
else
	lpMirrorProc = NULL;

// width = width of a line in buffer in bytes
// cwidth = number of bytes to copy to buffer
cwidth = width = dx * depth;

// Make sure requested area lies in frame
if (y >= Ysize)		// below image
	return(FALSE);
if ((y + dy) <= 0)	// above image
	return(FALSE);
if (x >= Xsize)		// right of image
	return(FALSE);
if ((x + dx) <= 0)	// left of image
	return(FALSE);

// save source x and y points
sx = x;
sy = y;

// check clipping for top of area to be read
if (y < 0)
	{ // advance in -y number of lines into data buffer
	lpData += (-y)*width;
	lpRect->top = 0; // clip to top of frame
	}
else
	lpRect->top = y; // way cool

// check clipping for bottom of area to be read
y = y + dy - 1;
if (y >= Ysize)
	lpRect->bottom = Ysize-1; // clip to bottom of frame
else
	lpRect->bottom = y; // way cool

// check clipping for left side of area to be read
if (x < 0)
	{ // advance in -x number of pixels into data buffer
	sdx = (-x)*depth;
	lpData += sdx;
	cwidth -= sdx; // reduce copy width
	lpRect->left = 0; // clip to left side of frame
	}
else
	lpRect->left = x; // way cool

// check clipping for right side of area to be read
x = x + dx - 1;
if (x >= Xsize)
	{
	lpRect->right = Xsize - 1; // clip to right side of frame
	cwidth -= (x-lpRect->right)*depth; // reduce copy width
	}
else
	lpRect->right = x; // way cool

// after all that god damn checking, copy the fricking data from the frame
MgxMirrorRect(lpRect, &mRect, Xsize, Ysize, HorzFlip, VertFlip);
if (HorzFlip)
	pixels = mRect.right - mRect.left + 1;
if (VertFlip)
	{
	y = mRect.bottom;
	yend = mRect.top;
	iy = -1;
	}
else
	{
	y = mRect.top;
	yend = mRect.bottom;
	iy = 1;
	}
while (TRUE)
	{
	if (lp = PixmapPtr(lpPixmap, pmType, mRect.left, y, NO))
		{
		copy(lp, lpData, cwidth);
		if (HorzFlip)
			(*lpMirrorProc)(lpData, pixels);
		}
	if (y == yend)
		break;
	lpData += width;
	y += iy;
		}

// Offset to make coordinates in rect relative to upper-left of buffer
OffsetRect(lpRect, -sx, -sy);
return(TRUE);
}

/************************************************************************/
void copyifset(
/************************************************************************/
LPTR 	lptr1,
LPTR	lptr2,
int 	count)
{
while ( --count >= 0 )
	{
	if ( *lptr2 )
		*lptr2++ = *lptr1++;
	else
		{
		lptr2++;
		lptr1++;
		}
	}
}

#ifdef C_CODE
/************************************************************************/
void TexturizeData24(
/************************************************************************/
LPRGB	lpDst,
LPTR	lpMsk,
int		iCount)
{
WORD	wMsk;
HSLS	hsl;

while (--iCount >= 0)
	{
//	RGBtoHSL(lpDst->red, lpDst->green, lpDst->blue, &hsl);
	wMsk = *lpMsk++ + 1;
	lpDst->red = (wMsk * (WORD)lpDst->red) >> 8;
	lpDst->green = (wMsk * (WORD)lpDst->green) >> 8;
	lpDst->blue = (wMsk * (WORD)lpDst->blue) >> 8;
//	hsl.lum = (wMsk * (WORD)hsl.lum) >> 8;
//	HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, lpDst);
	++lpDst;
	}
}


/************************************************************************/
void TexturizeData8(
/************************************************************************/
LPTR	lpDst,
LPTR	lpMsk,
int		iCount)
{
WORD	wMsk;
HSLS	hsl;

while (--iCount >= 0)
	{
	wMsk = *lpMsk++ + 1;
	*lpDst++ = (wMsk * (WORD)(*lpDst)) >> 8;
	}
}
#endif

/************************************************************************/

void TexturizeData32(
LPCMYK 	lpDst,
LPTR 	lpMsk,
int		iCount)
{
	register int iMask;

	LPTR lpDstB = (LPTR)lpDst;

	while (iCount-- > 0)
	{
		iMask = *lpMsk++ + 1;

		*lpDstB++ = (iMask * (WORD)(*lpDstB)) >> 8;
		*lpDstB++ = (iMask * (WORD)(*lpDstB)) >> 8;
		*lpDstB++ = (iMask * (WORD)(*lpDstB)) >> 8;
		*lpDstB++ = (iMask * (WORD)(*lpDstB)) >> 8;
	}
}

/************************************************************************/
void ScaleData8( LPTR lpDst, int iCount, BYTE factor )
/************************************************************************/
{
#ifdef C_CODE
WORD wFactor;

wFactor = factor;
++wFactor;
while ( --iCount >= 0 )
	*lpDst++ = ((WORD)*lpDst * wFactor) >> 8;
#else
__asm   {
	pushf
	push	di
	cld
	les     di, lpDst
	mov     cx, iCount
	mov     bl, factor
	sub		bh,bh
	inc		bx
Scale8Loop:
	mov     al, BYTE PTR es:[di]
	sub		ah,ah
	mul		bx
	mov		BYTE PTR es:[di],ah
	inc		di
	loop    Scale8Loop
	pop		di
	popf
	}
#endif
}

/************************************************************************/
void ScaleDataBuf8( LPTR lpDst, int iCount, LPTR lpMsk )
/************************************************************************/
{
#ifdef C_CODE
WORD wFactor;

while ( --iCount >= 0 )
	{
	wFactor = (WORD)(*lpMsk++) + 1;
	*lpDst++ = ((WORD)*lpDst * wFactor) >> 8;
	}
#else
__asm   {
	push	ds
	pushf
	push	di
	push	si
	cld
	les     di, lpDst
	lds		si, lpMsk
	mov     cx, iCount
	sub		bh,bh
SDB8_LOOP:
	lodsb				// get factor in al
	sub		ah,ah		// cast to WORD
	inc		ax
	mov     bl, BYTE PTR es:[di]
	mul		bx
	mov		BYTE PTR es:[di],ah
	inc		di
	loop    SDB8_LOOP
	pop		si
	pop		di
	popf
	pop		ds
	}
#endif
}

