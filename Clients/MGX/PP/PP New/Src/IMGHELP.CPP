//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

static LPTR lpGLHookBuf, lpGLMaskBuf, lpGLTransformBuf;
static long lDataBufSize, lMaskBufSize;

static BOOL ImgGetLineAlloc(int dx, int depth);

/************************************************************************/
// if !lpImage then it used the object list lpBaseObject
/************************************************************************/
/************************************************************************/
BOOL ImgGetLine(LPIMAGE lpImage, LPOBJECT lpBaseObject, int x, int y, int dx,
                LPTR lpOut, CHookHandler *lpImageHook, BOOL fSelected)
/************************************************************************/
{
LPOBJECT    lpObject;
LPALPHA     lpAlpha;
LPMASK      lpMask;
LPTR        lp;
LPPROCESSPROC lpProcessProc;
RECT        rLine, rSect;
int         nPixels, ox, oy, depth;
FRMTYPEINFO TypeInfo;
FRMDATATYPE DataType;
long        lOffset;
LPFRAME		lpFrame;

// get base image object
if (lpBaseObject)
    lpObject = lpBaseObject;
else if (lpImage)
    lpObject = lpImage->GetBase();
else
    return(FALSE);

// fill buffer with base image object
// use depth of base image object
lpFrame = ObjGetEditFrame(lpObject);
FrameGetTypeInfo(lpFrame, &TypeInfo);
DataType = TypeInfo.DataType;
depth = FrameDepth(lpFrame);
if (TypeInfo.DataType == FDT_LINEART)
	{
    depth = 1;
    TypeInfo.DataType = FDT_GRAYSCALE;
	}

if (!ImgGetLineAlloc(dx, depth))
    {
    Message(IDS_EMEMALLOC);
    return(FALSE);
    }

if (!FrameRead(lpFrame, x, y, dx, lpOut, dx))
    return(FALSE);

// allocate memory for a hook proc to use
if (lpImage)
    lpMask = lpImage->GetMask();
else
    lpMask = NULL;

// if an image hook has been setup, call it if base is selected
if (lpImageHook )
    if (lpImage->IsSelectedObject(lpObject))
        lpImageHook->ImageHookProc(x, y, dx, TypeInfo, lpGLHookBuf, lpOut, 
            lpGLMaskBuf, lpMask);

// setup rect for this line for intersect check
rLine.left = x;
rLine.right = x + dx - 1;
rLine.top = rLine.bottom = y;
// start with first object on top of base image

while(lpObject = (lpImage ? lpImage->GetNextObject(lpObject, YES, NO):(LPOBJECT)lpObject->lpNext))
    {
    if (fSelected && !lpObject->fSelected)
      continue;
    // if object cannot be rendered through raster means
    if (lpObject->fDeleted || 
        (lpObject->RenderType != RT_RASTER && lpObject->RenderType != RT_FRAME) ||
        !AstralIntersectRect(&rSect, &lpObject->rObject, &rLine))
        continue;
    // we can render frames ourselves
    if (lpObject->RenderType == RT_FRAME && ObjGetEditFrame(lpObject))
        {
        // get merge proc for this object
        lpProcessProc = GetProcessProc(lpObject->MergeMode, DataType);

        ox = rSect.left-lpObject->rObject.left;
        oy = y-lpObject->rObject.top;
        nPixels = (rSect.right - rSect.left + 1);
        lOffset = (long)(rSect.left - rLine.left) * depth;
        if (!lpObject->fTransform)
            {
            lp = FramePointer((LPFRAME)ObjGetEditFrame(lpObject),
                                    ox, oy, NO, nPixels);
            if (lp && lpImageHook)
                if (lpImage->IsSelectedObject(lpObject))
                    {
                    copy(lp, lpGLTransformBuf, nPixels*depth);
                    lp = lpGLTransformBuf;
                    lpImageHook->ImageHookProc(rSect.left, y, nPixels, TypeInfo,
                                    lpGLHookBuf, lp, lpGLMaskBuf, lpMask);
                    }
            if (lp)
                {
                if (lpAlpha = ObjGetAlpha(lpObject))
                    {
                    MaskLoad(lpAlpha, ox, oy, nPixels, lpGLMaskBuf);
                    if (lpObject->Opacity != 255)
                        ScaleData8(lpGLMaskBuf, nPixels, lpObject->Opacity);
                    }
                else
                    set(lpGLMaskBuf, nPixels, lpObject->Opacity);
                }
            }
        else
            {
            if ( TransformLine( lpObject, ox, oy, nPixels, lpGLTransformBuf, depth,
                lpGLMaskBuf, 1, NULL, Mask.DisplayQuality && Mask.PasteQuality) )
                {
                lp = lpGLTransformBuf;
                if (lpObject->Opacity != 255)
                    ScaleData8(lpGLMaskBuf, nPixels, lpObject->Opacity);
                if (lpObject->fPasteIntoMask && lpMask)
                    MaskLoader(lpMask, rSect.left, rSect.top, nPixels, 
                                lpGLMaskBuf, NO, CR_MULTIPLY);
                if (ColorMask.Mask && ColorMask.On)
                    Shield(lpGLMaskBuf, lp, (WORD)nPixels, TypeInfo);
                }
            else
                lp = NULL;
            }
        if (lp)
            (*lpProcessProc)(lpOut+lOffset, lp, lpGLMaskBuf, nPixels);
        }
    // call routines that knows how to render vector objects
    else if (lpObject->ObjectType == OT_VECTOR)
        {
        }
    }
return(TRUE);
}

/************************************************************************/
BOOL ImgGetObjectLine(LPIMAGE lpImage, LPOBJECT lpBaseObject,
                        int x, int y, int dx, LPTR lpOut,
                        BOOL fSelected, BOOL bClipBaseToMask, LPCOLORINFO lpColor)
/************************************************************************/
{
LPOBJECT        lpObject;
LPALPHA     lpAlpha;
LPTR            lp;
LPFRAME     lpFrame;
LPPROCESSPROC lpProcessProc;
RECT            rLine, rSect,rObject;
int         depth, nPixels, ox, oy;
long            lOffset;
BOOL            fBaseSelected;
FRMDATATYPE		DataType;

if (!lpBaseObject)
    lpBaseObject = lpImage->GetBase();
if (!lpImage)
    fSelected = NO;

lpFrame = ObjGetEditFrame(lpBaseObject);
if ( !(depth = FrameDepth(lpFrame)) )
    depth = 1; // never worry about lineart
DataType = FrameType(lpFrame);

if (!ImgGetLineAlloc(dx, depth))
    {
    Message(IDS_EMEMALLOC);
    return(FALSE);
    }

// setup rect for this line for intersect check
rLine.left = x;
rLine.right = x + dx - 1;
rLine.top = rLine.bottom = y;
// start with first object on top of base image

if (lpColor &&
    (fSelected ||
	!lpImage ||
    rLine.left < 0 || rLine.top < 0 || 
    rLine.right >= FrameXSize(lpFrame) ||
    rLine.bottom >= FrameYSize(lpFrame)) )
    LoadColor(lpFrame, lpOut, dx, lpColor);

if (fSelected)
    {
    lpObject = lpImage->GetSelObject();
    fBaseSelected = lpObject == lpImage->GetBase();
    }
else
    lpObject = lpBaseObject;

//PixmapGetInfo(&lpBaseObject->Pixmap, PMT_EDIT, &iImageWidth, &iImageHeight, &dummy, NULL);
while (lpObject)
    {
    // if object cannot be rendered through raster means
    rObject = lpObject->rObject;
//    BoundRect(&rObject, 0, 0, iImageWidth-1, iImageHeight-1);
    if ((lpObject->RenderType == RT_RASTER ||
            lpObject->RenderType == RT_FRAME) &&
            AstralIntersectRect(&rSect, &rObject, &rLine))
        {
        // we can render frames ourselves
        if (lpObject->RenderType == RT_FRAME)
            {
            // get merge proc for this object
            lpProcessProc = GetProcessProc(
                fSelected ? MM_NORMAL : lpObject->MergeMode, DataType);

            ox = rSect.left-lpObject->rObject.left;
            oy = y-lpObject->rObject.top;
            nPixels = (rSect.right - rSect.left + 1);
            lOffset = (long)(rSect.left - rLine.left) * depth;
            lp = FramePointer((LPFRAME)ObjGetEditFrame(lpObject),
                                    ox, oy, NO, nPixels);
            if (lp)
                {
                if (lpObject == lpBaseObject && !bClipBaseToMask)
                    set(lpGLMaskBuf, nPixels, 255);
                else
                if (lpAlpha = ObjGetAlpha(lpObject))
                    {
                    MaskLoad(lpAlpha, ox, oy, nPixels, lpGLMaskBuf);
                    if (fSelected && !fBaseSelected)
                        setifset(lpGLMaskBuf, nPixels, 255);
                    else
                    if (lpObject->Opacity != 255)
                        ScaleData8(lpGLMaskBuf, nPixels, lpObject->Opacity);
                    }
                else
                    set(lpGLMaskBuf, nPixels, lpObject->Opacity);
                if (lp)
                    (*lpProcessProc)(lpOut+lOffset, lp, lpGLMaskBuf, nPixels);
                }
            }
        // call routines that knows how to render vector objects
        else if (lpObject->ObjectType == OT_VECTOR)
            {
            }
        }
    if (fSelected)
        lpObject = lpImage->GetSelObject(lpObject);
    else if (lpImage)
        lpObject = lpImage->GetNextObject(lpObject, YES, NO);
    else
        lpObject = (LPOBJECT)lpObject->lpNext;
    }
return(TRUE);
}

/************************************************************************/
static BOOL ImgGetLineAlloc(int dx, int depth)
/************************************************************************/
{
long lDataBufNeeded, lMaskBufNeeded;

// allocate file buffer
lDataBufNeeded = (long)dx * depth;
if (lDataBufNeeded > lDataBufSize)
    {
    if (lpGLHookBuf)
        FreeUp(lpGLHookBuf);
    lpGLHookBuf = NULL;
    if (lpGLTransformBuf)
        FreeUp(lpGLTransformBuf);
    lpGLTransformBuf = NULL;
    lDataBufSize = lDataBufNeeded;
    if (!(lpGLHookBuf = Alloc(lDataBufSize)) ||
        !(lpGLTransformBuf = Alloc(lDataBufSize)))
        {
        if (lpGLHookBuf)
            FreeUp(lpGLHookBuf);
        lpGLHookBuf = NULL;
        lDataBufSize = 0;
        return(FALSE);
        }
    }

// allocate mask buffer
lMaskBufNeeded = (long)dx;
if (lMaskBufNeeded > lMaskBufSize)
    {
    if (lpGLMaskBuf)
        FreeUp(lpGLMaskBuf);
    lMaskBufSize = lMaskBufNeeded;
    if (!(lpGLMaskBuf = Alloc(lMaskBufSize)))
        {
        lMaskBufSize = 0;
        FreeUp(lpGLHookBuf);
        lpGLHookBuf = NULL;
        FreeUp(lpGLTransformBuf);
        lpGLTransformBuf = NULL;
        lDataBufSize = 0;
        return(FALSE);
        }
    }
return(TRUE);
}

/************************************************************************/
BOOL ImgMaskLoad(LPIMAGE lpImage, LPOBJECT lpObject, LPMASK lpMaskIn,
                int x, int y, int dx, LPTR lpMaskBuf, BOOL fSelected,
                COMBINE_ROP ROP, int xOffset, int yOffset)
/************************************************************************/
{
LPOBJECT lpBase;
LPMASK      lpMask;
RECT        rLine, rSect, rObject;
int         nPixels, ox, oy;
long        lOffset;
BOOL        fBaseSelect;

if (!lpImage)
    return(FALSE);

// if no get starting object...
if ( !lpObject )
    lpObject = lpImage->GetBase();

// clear mask buffer initially
clr(lpMaskBuf, dx);

// set flag to check if special base image selection
if (fBaseSelect = (!lpMaskIn && fSelected))
    lpBase = lpImage->GetBase();

// setup up rectangle for this buffer
rLine.left = x;
rLine.right = x + dx - 1;
rLine.top = rLine.bottom = y;

// start with first object on top of base image
while (lpObject || lpMaskIn)
    {
    rObject = lpObject->rObject;
    OffsetRect(&rObject, xOffset, yOffset);
    // if object can have an alpha or is an alpha
    if ((lpObject->ObjectType == OT_FRAME ||
         lpObject->ObjectType == OT_ALPHA) &&
        AstralIntersectRect(&rSect, &rObject, &rLine))
        {
        lOffset = (long)(rSect.left - rLine.left);
        nPixels = (rSect.right - rSect.left + 1);
        if (fBaseSelect && (lpObject == lpBase))
            set(lpMaskBuf+lOffset, nPixels, 255);
        else
        if ((lpMask = lpMaskIn) || (lpMask = ObjGetAlpha(lpObject)))
            {
            ox = rSect.left-rObject.left;
            oy = y-rObject.top;

            if (!lpObject->fTransform)
                MaskLoader(lpMask, ox, oy, nPixels, lpMaskBuf+lOffset, NO, ROP);
            else
            if ( TransformLine( lpObject, ox, oy, nPixels, NULL, 0,
                LineBuffer[0], 1, NULL, Mask.DisplayQuality && Mask.PasteQuality) )
                CombineData( LineBuffer[0], lpMaskBuf+lOffset, nPixels, NO, ROP );
            else
                set( lpMaskBuf+lOffset, nPixels, 255 );
            }
        }
    if (lpMaskIn)
        break;
    if (fSelected)
        lpObject = lpImage->GetSelObject(lpObject);
    else
        lpObject = lpImage->GetNextObject(lpObject, YES, NO);
    }
return(TRUE);
}


//   Currently will not scale up.
//   Adjusts lpRect in the scale case to maintain aspect ratio.
LPFRAME ImgCopyFrame(LPIMAGE lpImage, LPOBJECT lpBaseObject, LPRECT lpRect,
                    BOOL bCrop, BOOL fSelected)
{
return( ImgCopyMaskedFrame(lpImage, lpBaseObject,lpRect,bCrop,fSelected, NO, NULL));
}

//   Currently will not scale up.
//   Adjusts lpRect in the scale case to maintain aspect ratio.
//	 
//	lpColor: if this doesn't == NULL then the background will be filled with
//			it before copying the frame. 
//	NOTE: if there is an lpImage supplied then the base object is drawn 
//		without masking.
LPFRAME ImgCopyMaskedFrame(LPIMAGE lpImage, LPOBJECT lpBaseObject, LPRECT lpRect,
                    BOOL bCrop, BOOL fSelected, BOOL bClipToMask, LPCOLORINFO lpColor)
{
LPFRAME lpBaseFrame, lpNewFrame;
LPOBJECT lpObject;
int     depth, dx, dy, y, width, height;
LPTR    lpDataBuf;
RECT    rMask;
LFIXED  rate, line;
LPTR    lpDst;
int     yread;
COLORINFO Color;

if (!lpImage)
    fSelected = NO;
//if (fSelected)
//  {
//  if (CountSelObjects(NULL) > 1)
//      fSelected = NO;
//  }
if (lpBaseObject)
    lpObject = lpBaseObject;
else if (lpImage)
    lpObject = lpImage->GetBase();
else
    return (FALSE);
    
lpBaseFrame = ObjGetEditFrame(lpObject);

if (lpRect)
    rMask = *lpRect;
else if (lpImage)
    lpImage->GetMaskRect(&rMask);
else
    rMask = lpObject->rObject;  // why not?
    
if ( !(depth = FrameDepth(lpBaseFrame)) )
    depth = 1; // never worry about lineart
dx = RectWidth(&rMask);
dy = RectHeight(&rMask);
width = FrameXSize(lpBaseFrame);
height = FrameYSize(lpBaseFrame);
if (!bCrop)
    {
    dy = Min(height, dy);
    dx = Min(width, dx);
    ScaleToFit(&dx, &dy, width, height);
    dy = Max(1, dy);
    dx = Max(1, dx);    
    }

if (!(lpDataBuf = Alloc((long)depth*(bCrop ? dx:width))))
    return(NULL);
if (!(lpNewFrame = FrameOpen(FrameTypeInfo(lpBaseFrame), dx, dy, FrameResolution(lpBaseFrame))))
    {
    FreeUp(lpDataBuf);
    return(NULL);
    }

if (!lpColor)
{
	Color.gray = 255;
	SetColorInfo(&Color, &Color, CS_GRAY);
	lpColor = &Color;
}

if (bCrop)
    {
    for (y = rMask.top; y <= rMask.bottom; ++y)
        {
        if (!ImgGetObjectLine(lpImage, lpObject, rMask.left, y, dx, lpDataBuf,
                             fSelected, bClipToMask, lpColor) ||
            !FrameWrite(lpNewFrame, 0, y-rMask.top, dx, lpDataBuf, dx))
            {
            Message( IDS_EREAD, (LPSTR)Control.RamDisk );
            FreeUp(lpDataBuf);
            FrameClose(lpNewFrame);
            return(NULL);
            }
        }
    }
else
    {
    rate = FGET(width, dx);
    // sample frame
    line = 0L;
    yread = 0;
    for (y=0;y<dy;y++)
        {
        yread = WHOLE(line);
        lpDst = FramePointer(lpNewFrame,0,y,YES);
        if (!ImgGetObjectLine(lpImage, lpObject, 0, yread, width, lpDataBuf,
                            fSelected, bClipToMask, lpColor) || !lpDst)
            {
            FrameClose(lpNewFrame);
            return(NULL);
            }
        FrameSample(lpNewFrame, lpDataBuf, 0, lpDst, 0, dx, rate);
        line += rate; 
        }   // for y
    }
FreeUp(lpDataBuf);
return(lpNewFrame);
}


