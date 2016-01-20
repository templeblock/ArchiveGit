//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "stdafx.h"
#include "getline.h"
#include "merge.h"
#include "imgutil.h"
#include "mask.h"

static LPTR lpGLHookBuf, lpGLMaskBuf, lpGLTransformBuf;
static long lDataBufSize, lMaskBufSize;

static BOOL ObjGetLineAlloc(int dx, int depth);

/************************************************************************/
BOOL ObjGetLine(LPOBJECT lpObject, int x, int y, int dx, LPTR lpOut)
/************************************************************************/
{
	LPALPHA     lpAlpha;
	LPTR        lp;
	LPPROCESSPROC lpProcessProc;
	RECT        rLine, rSect;
	int         nPixels, ox, oy, depth;
	FRMTYPEINFO TypeInfo;
	FRMDATATYPE DataType;
	long        lOffset;
	LPFRAME		lpFrame;

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

	if (!ObjGetLineAlloc(dx, depth))
   	{
    	//Message(IDS_EMEMALLOC);
    	return(FALSE);
   	}

	if (!FrameRead(lpFrame, x, y, dx, lpOut, dx))
    	return(FALSE);

	// setup rect for this line for intersect check
	rLine.left = x;
	rLine.right = x + dx - 1;
	rLine.top = rLine.bottom = y;
	// start with first object on top of base image

	while(lpObject = lpObject->lpNext)
   	{
    	// if object cannot be rendered through raster means
    	if (lpObject->fDeleted || 
        	!AstralIntersectRect(&rSect, &lpObject->rObject, &rLine))
        	continue;

        // get merge proc for this object
        lpProcessProc = GetProcessProc(lpObject->MergeMode, DataType);

        ox = rSect.left-lpObject->rObject.left;
        oy = y-lpObject->rObject.top;
        nPixels = (rSect.right - rSect.left + 1);
        lOffset = (long)(rSect.left - rLine.left) * depth;
        lp = FramePointer(ObjGetEditFrame(lpObject), ox, oy, NO, nPixels);
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
        	(*lpProcessProc)(lpOut+lOffset, lp, lpGLMaskBuf, nPixels);
        }
    }
	return(TRUE);
}

/************************************************************************/
static BOOL ObjGetLineAlloc(int dx, int depth)
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

