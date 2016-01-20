//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

static STATUS_CODE LineProcessObj(LPIMAGE lpImage, LPOBJECT lpObject,
	LPENGINE lpEngine, LPMASK lpMask);
static BOOL ReadProcessObj(LPIMAGE lpImage, LPOBJECT lpObject,
	BOOL fReadAlpha, LPRECT lpRect, LPREADDATAPROC lpReadDataProc);
LOCAL STATUS_CODE LineProcessPixmap(LPPIXMAP lpPixmap, int PixOffX, int PixOffY, LPENGINE lpEngine, LPMASK lpMask, LPRECT lpArea);


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
int y, dx;
LPTR lpSrc;
LPPIXMAP lpPixmap;
FRMTYPEINFO TypeInfo;

if ( !lpReadDataProc )
	return( FALSE );
if ( !lpRect )
	return( FALSE );

if (fReadAlpha)
		lpPixmap = &lpObject->lpAlpha->Pixmap;
else	lpPixmap = &lpObject->Pixmap;

ImgGetTypeInfo(lpImage, &TypeInfo);
rObject = lpObject->rObject;

dx = RectWidth(lpRect);
ProgressBegin(1,0);
for ( y = lpRect->top; y <= lpRect->bottom; ++y )
	{
	AstralClockCursor( y-lpRect->top, RectHeight(lpRect), NO );
	if ( lpSrc = PixmapPtr( lpPixmap, PMT_EDIT,
		lpRect->left - rObject.left, y - rObject.top, YES, dx ) )
			(*lpReadDataProc)( y, lpRect->left, lpRect->right, lpSrc, TypeInfo );
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
	lpEngine->lpParam		= NULL;
	lpEngine->fThread		= YES;
	AstralSetRectEmpty(&lpEngine->rUpdate);
	lpEngine->fEscapable 	= TRUE;
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
STATUS_CODE LineEngineSelObj(LPIMAGE lpImage, LPENGINE lpEngine, int dirty)
//************************************************************************
{
RECT rEdit, rClip;
LPRECT lpOldEditRect;
LPOBJECT lpObject;
LPMASK lpMask = NULL;
BOOL fClip = NO;
STATUS_CODE StatusCode = SC_SUCCESS;
BOOL fChanged = NO;
int nNumPasses = 0;

ASSERT(lpImage);

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
			return(StatusCode);
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
		return(SC_UNDOERROR);
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
	StatusCode = LineProcessObj( lpImage, lpObject, lpEngine, lpMask );
	lpEngine->lpEditRect = lpOldEditRect; 
	if (StatusCode)
		{
		// if we cancelled in the middle we might need to redraw
		if (StatusCode == SC_CANCELLED)
			AstralUnionRect(&lpEngine->rUpdate, &lpEngine->rUpdate, &rEdit);
		if (!lpEngine->fNoUndo)
			ImgEditRecoverSelObj(lpImage, !lpEngine->fEditAlpha, lpEngine->fEditAlpha);
		break;
		}
	if (!lpEngine->fNoUndo)
		ImgEditedObject(lpImage, lpObject, dirty, &rEdit);
	AstralUnionRect(&lpEngine->rUpdate, &lpEngine->rUpdate, &rEdit);
	}
ProgressEnd();
return(StatusCode);
}


/************************************************************************/
STATUS_CODE LineEngineObj(LPIMAGE lpImage,	LPOBJECT lpObject, LPENGINE lpEngine, int dirty)
/************************************************************************/
{
RECT rEdit, rClip;
STATUS_CODE StatusCode = SC_SUCCESS;
LPMASK lpMask;
LPRECT lpOldEditRect;

ASSERT(lpImage);
ASSERT(lpObject);
ASSERT(!lpEngine->fEditAlpha || lpObject->lpAlpha);

if (lpEngine->MaskType == MT_BASE && (lpMask = ImgGetMask(lpImage)))
	{
	lpMask = ImgGetMask(lpImage);
	ImgGetMaskRect(lpImage, &rClip);
	if (!AstralIntersectRect(&rEdit, &rClip, &lpObject->rObject))
		return(StatusCode);
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
		return(StatusCode);
	}
ProgressBegin(1, dirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if (!lpEngine->fNoUndo)
	{
	if (!ImgEditInit(lpImage, ET_OBJECT, lpEngine->fEditAlpha ? UT_ALPHA : UT_DATA,
				lpObject))
		{
		ProgressEnd();
		return(SC_MEMERROR);
		}
	}
lpOldEditRect = lpEngine->lpEditRect;
lpEngine->lpEditRect = &rEdit; 
if ( StatusCode = LineProcessObj( lpImage, lpObject, lpEngine, lpMask ) )
	{
	// if we cancelled in the middle we might need to redraw
	if (StatusCode == SC_CANCELLED)
		lpEngine->rUpdate = rEdit;
	if (!lpEngine->fNoUndo)
		ImgEditRecoverObj(lpImage, lpObject, !lpEngine->fEditAlpha, lpEngine->fEditAlpha);
	}
else
	{
	if (!lpEngine->fNoUndo)
		ImgEditedObject(lpImage, lpObject, dirty, &rEdit);
	lpEngine->rUpdate = rEdit;
	}
lpEngine->lpEditRect = lpOldEditRect; 
ProgressEnd();
return(StatusCode);
}

typedef struct 
	{
	LPPIXMAP	lpPixmap;
	int			PixOffX;
	int			PixOffY;
	LPENGINE    lpEngine;
	LPMASK		lpMask;
	} ENGINE_PARMS, FAR *LPENGINE_PARMS;

LOCAL BOOL EngineProc(LPVOID lpParam, LPRECT lpArea);

/************************************************************************/
static STATUS_CODE LineProcessObj(LPIMAGE lpImage, LPOBJECT lpObject, LPENGINE lpEngine, LPMASK lpMask)
/************************************************************************/
{
RECT rObject;
LPPIXMAP lpPixmap;
RECT rLines;

rObject = lpObject->rObject;
if (lpEngine->fEditAlpha)
	lpPixmap = &lpObject->lpAlpha->Pixmap;
else
	lpPixmap = &lpObject->Pixmap;
if(lpEngine->lpEditRect)
	rLines = *lpEngine->lpEditRect;
else
	rLines = rObject;

#ifdef WIN32
FRMDATATYPE Type = FrameType(ObjGetEditFrame(lpObject));
if (Type != FDT_LINEART &&
	lpEngine->fThread)
	{
	ENGINE_PARMS parms;

	parms.lpPixmap = lpPixmap;
	parms.PixOffX = rObject.left;
	parms.PixOffY = rObject.top;
	parms.lpEngine = lpEngine;
	parms.lpMask = lpMask;
	return(( STATUS_CODE )SplitThreads(0, EngineProc, NULL, NULL, (LPVOID)&parms, &rLines));
	}
else
#endif
	return(LineProcessPixmap(lpPixmap, rObject.left, rObject.top, lpEngine, lpMask, &rLines));
}

/************************************************************************/
LOCAL BOOL EngineProc(LPVOID lpParam, LPRECT lpArea)
/************************************************************************/
{
LPENGINE_PARMS lpParms;

lpParms = (LPENGINE_PARMS)lpParam;
return(LineProcessPixmap(	lpParms->lpPixmap,
							lpParms->PixOffX,
							lpParms->PixOffY,
							lpParms->lpEngine,
							lpParms->lpMask,
							lpArea));
}

//************************************************************************
LOCAL STATUS_CODE LineProcessPixmap(LPPIXMAP lpPixmap, int PixOffX, int PixOffY, LPENGINE lpEngine, LPMASK lpMask,
		   						LPRECT lpArea)
//************************************************************************
{
	int y;
	RECT rSource;
	int w, h, depth;
	BOOL UsingMask;
	BOOL (FAR *lpGetSourceData)(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
				int x, int y, int dx, int dy, HPTR hpData,
				LPRECT lpRect,  BOOL HorzFlip, BOOL VertFlip);
	LPPROCESSPROC lpProcessProc;
	FRMDATATYPE DataType;
	PIXMAP srcPixmap;
	LPMASKPROC lpMaskProc;
	LPDATAPROC lpDataProc;
	LPCOLORINFO lpColor;
	int Opacity, xMaskOff, yMaskOff, ScatterPressure;
	BOOL FlipHorz, FlipVert, fEscapable;
	LPFRAME lpEditFrame;
	FRMTYPEINFO TypeInfo;
	LPVOID lpParam;
	LPTR lpDst;
	STATUS_CODE StatusCode = SC_SUCCESS;
	LPTR lpSrc = NULL;
	LPTR lpMsk = NULL;
	LPINT16 lpErr = NULL;
	RECT rLines;

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

	rLines = *lpArea;

	// get width, height, and depth for operation
	w = RectWidth(&rLines);
	h = RectHeight(&rLines);
	PixmapGetInfo(lpPixmap, PMT_EDIT, NULL, NULL, &depth, &DataType);

	// allocate necessary buffers
	if (!(lpMsk = Alloc((long)w)))
		goto MemError;
	if (lpEngine->ScatterPressure < 255)
	{
		if (!(lpErr = (LPINT16)Alloc((long)(w+1) * sizeof(INT16))))
			goto MemError;
		clr((LPTR)lpErr, (w+1) * sizeof(INT16));
	}
	if (!(lpSrc = Alloc((long)w * (long)depth)))
		goto MemError;
	
	lpProcessProc = GetProcessProc(lpEngine->MergeMode, DataType);

	// get all values that are used in loop for faster access
	lpDataProc = lpEngine->lpDataProc;
	lpMaskProc = lpEngine->lpMaskProc;
	Opacity = lpEngine->Opacity;
	FlipHorz = lpEngine->FlipHorz;
	FlipVert = lpEngine->FlipVert;
	lpEditFrame = lpPixmap->EditFrame;
	FrameGetTypeInfo(lpEditFrame, &TypeInfo);
	ScatterPressure = lpEngine->ScatterPressure;
	lpParam = lpEngine->lpParam;
	fEscapable = lpEngine->fEscapable;

	lpGetSourceData = NULL;
	if (lpColor = lpEngine->lpColor)
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
		if (AstralClockCursor(y-rLines.top, h, fEscapable))
		{
			if (CheckAbortInPlaceEdit(lpPixmap))
				goto Cancelled;
		}

		if (UsingMask) /* If DMASK being used */
		{
			// get lpDst here without bModify, just in case we
			// are editing the mask.
			if (!(lpDst = PixmapPtr(lpPixmap, PMT_EDIT,
					rLines.left-PixOffX, y-PixOffY, NO, w)))
				continue;
			mload( rLines.left+xMaskOff, y+yMaskOff, w, 1, lpMsk, lpDst,
						TypeInfo, lpMask );
			ScaleData8(lpMsk, w, (BYTE)Opacity);
		}
		else if (lpMaskProc || ScatterPressure < 255)
			set(lpMsk, w, Opacity);
		if (lpMaskProc)
			(*lpMaskProc)(y, rLines.left, rLines.right, lpMsk, lpParam);
		if (ScatterPressure < 255)
			ScatterMask(lpMsk, lpMsk, lpErr, w, 1, ScatterPressure);

		if (!(lpDst = PixmapPtr(lpPixmap, PMT_EDIT,
				rLines.left-PixOffX, y-PixOffY, YES, w)))
			continue;

		if (lpGetSourceData)
			if (!(*lpGetSourceData)((LPPIXMAP)&srcPixmap, PMT_EDIT,
					rLines.left, y, w, 1,
				
					lpSrc, (LPRECT)&rSource, FlipHorz, FlipVert))
				continue;

		if (lpDataProc)
			(*lpDataProc)(y, rLines.left, rLines.right, lpDst, lpSrc, depth, lpParam);
		(*lpProcessProc)(lpDst, lpSrc, lpMsk, w);
		if (lpColor && lpDataProc)
			LoadColor(lpEditFrame, lpSrc, w, lpColor);
	}
	goto Exit;

	MemError:
	StatusCode = SC_MEMERROR;
	goto Exit;
	
	Cancelled:
	StatusCode = SC_CANCELLED;

	Exit:
	FreeUp(lpMsk);
	FreeUp((LPTR)lpErr);
	FreeUp(lpSrc);
	ProgressEnd();
	return(StatusCode);
}


//************************************************************************
// 	Writes a line of data with mask&color shield into lpDstPix.
//	No merge mode or opacity.
//	lpDstPix is at offx, offy from the mask.
//
//  Writes data starting from lpSrcData[0] int lpDstPixmap offset by xstart.
//	Writes width pixels, (or -1 for all)
//************************************************************************
void WriteLine(LPTR lpSrcData, LPPIXMAP lpDstPix, LPMASK lpMask,int y, int offx, int offy, int xstart/*0*/, int width/*-1*/)
//************************************************************************
{
	LPTR lpDstData, lpMaskData;	
	LPPROCESSPROC lpProcessProc;
	int nPixel, depth;
	FRMDATATYPE DataType;
	
	PixmapGetInfo(lpDstPix, PMT_EDIT, &nPixel,NULL,&depth,&DataType);
	lpDstData = PixmapPtr(lpDstPix, PMT_EDIT,xstart,y,YES);
	if (!lpDstData)
		return;
	if (width>=0)
		nPixel = min(width, nPixel);
	lpProcessProc = GetProcessProc(MM_NORMAL, DataType);
	lpMaskData = LineBuffer[0];
	MaskLoad(lpMask, offx+xstart, offy+y, nPixel, lpMaskData);
	(*lpProcessProc)(lpDstData, lpSrcData, lpMaskData, nPixel);
}

/************************************************************************/
void LoadMaskDataHuge(int xs,int ys,int dx,int dy, HPTR pm, LPMASK lpMask, COMBINE_ROP rop)
/************************************************************************/
{
int y, ye;
HPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoader( lpMask, xs, y, dx, tpm, NO, rop );
}

/************************************************************************/
void LoadMaskData(int xs,int ys,int dx,int dy, LPTR pm, LPMASK lpMask, COMBINE_ROP rop)
/************************************************************************/
{
int y, ye;
LPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoader( lpMask, xs, y, dx, tpm, NO, rop );
}

/************************************************************************/
void mload(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
switch(TypeInfo.DataType)
	{
	case FDT_LINEART :
	case FDT_GRAYSCALE :
	mload8(xs, ys, dx, dy, pm, pd, TypeInfo, lpMask);
	break;

	case FDT_PALETTECOLOR:
	mload8P(xs, ys, dx, dy, pm, pd, TypeInfo, lpMask);
	break;

	case FDT_RGBCOLOR :
	mload24(xs, ys, dx, dy, pm, (LPRGB)pd, TypeInfo, lpMask);
	break;

	case FDT_CMYKCOLOR :
	mload32(xs, ys, dx, dy, pm, (LPCMYK)pd, TypeInfo, lpMask);
	break;
	}
}

/************************************************************************/
void mload8(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
LPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoad( lpMask, xs, y, dx, tpm );
if (ColorMask.Mask && ColorMask.On)
	Shield8(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void MLoad8Huge(int xs,int ys,int dx,int dy, HPTR pm, HPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
HPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoaderHuge( lpMask, xs, y, dx, tpm, NO, CR_COPY );
if (ColorMask.Mask && ColorMask.On)
	Shield8Huge(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void Shield(LPTR pm, LPTR pd, WORD wCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
switch (TypeInfo.DataType)
	{
	case FDT_LINEART:
	case FDT_GRAYSCALE:
		Shield8(pm, pd, wCount, TypeInfo);
	break;

	case FDT_PALETTECOLOR:
		Shield8P(pm, pd, wCount, TypeInfo);
	break;

	case FDT_RGBCOLOR:
		Shield24(pm, (LPRGB)pd, wCount, TypeInfo);
	break;

	case FDT_CMYKCOLOR:
		Shield32(pm, (LPCMYK)pd, wCount, TypeInfo);
	break;
	}
}

/************************************************************************/
void Shield8(LPTR pm, LPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
LPTR RMask, GMask, BMask;
BYTE Include, NotInclude;
BYTE value, mask;

if (!dwCount)
	return;

RMask = ColorMask.RMask;
GMask = ColorMask.GMask;
BMask = ColorMask.BMask;
if (ColorMask.Include)
	{
	Include = 255;
	NotInclude = 0;
	}
else
	{
	Include = 0;
	NotInclude = 255;
	}
while (dwCount-- != 0)
	{
	value = *pd++;
	if (!(mask = RMask[value]))
		*pm++ &= NotInclude; /* not in range */
	else
	if (!(mask &= GMask[value]))
		*pm++ &= NotInclude; /* not in range */
	else
	if ((mask &= BMask[value]))
		*pm++ &= Include; /* in range */
	else
		*pm++ &= NotInclude;	/* not in range */
	}
}

/************************************************************************/
void Shield8Huge(HPTR pm, HPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
#ifdef WIN32
	Shield8(pm, pd, dwCount, TypeInfo);
#else
	DWORD dwLeft, dwMskLeft;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)pd & 0xFFFFL);
		dwMskLeft = 65536L - ((DWORD)pm & 0xFFFFL);
		if (dwMskLeft < dwLeft)
			dwLeft = dwMskLeft;
		// see if we will cross a segment boundary
		if (dwLeft > INT_MAX)
			dwLeft = INT_MAX; // never try to copy more than maxint bytes
		if (dwLeft > dwCount)
			dwLeft = dwCount;
		Shield8(pm, pd, (int)dwLeft, TypeInfo);
		dwCount -= dwLeft;
		if (!dwCount)
			break;
		pd += dwLeft;
		pm += dwLeft;
	}
#endif
}

/************************************************************************/
void mload8P(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
LPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoad( lpMask, xs, y, dx, tpm );
if (ColorMask.Mask && ColorMask.On)
	Shield8P(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void MLoad8PHuge(int xs,int ys,int dx,int dy, HPTR pm, HPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
HPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoaderHuge( lpMask, xs, y, dx, tpm, NO, CR_COPY );
if (ColorMask.Mask && ColorMask.On)
	Shield8PHuge(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void Shield8P(LPTR pm, LPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
BYTE mask;
LPTR RMask, GMask, BMask;
BOOL Include, NotInclude;
RGBS rgb;
LPRGB lpRGBmap;

if (!dwCount)
	return;
RMask = ColorMask.RMask;
GMask = ColorMask.GMask;
BMask = ColorMask.BMask;
if (ColorMask.Include)
	{
	Include = 255;
	NotInclude = 0;
	}
else
	{
	Include = 0;
	NotInclude = 255;
	}
lpRGBmap = TypeInfo.ColorMap->RGBData;
while (dwCount-- != 0)
	{
	rgb = lpRGBmap[*pd++];
	if (!(mask = RMask[rgb.red]))
		*pm++ &= NotInclude; /* not in range */
	else
	if (!(mask &= GMask[rgb.green]))
		*pm++ &= NotInclude; /* not in range */
	else
	if ((mask &= BMask[rgb.blue]))
		*pm++ &= Include; /* in range */
	else
		*pm++ &= NotInclude;	/* not in range */
	}
}

/************************************************************************/
void Shield8PHuge(HPTR pm, HPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
#ifdef WIN32
	Shield8P(pm, pd, dwCount, TypeInfo);
#else
	DWORD dwLeft, dwMskLeft;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)pd & 0xFFFFL);
		dwMskLeft = 65536L - ((DWORD)pm & 0xFFFFL);
		if (dwMskLeft < dwLeft)
			dwLeft = dwMskLeft;
		// see if we will cross a segment boundary
		if (dwLeft > INT_MAX)
			dwLeft = INT_MAX; // never try to copy more than maxint bytes
		if (dwLeft > dwCount)
			dwLeft = dwCount;
		Shield8P(pm, pd, (int)dwLeft, TypeInfo);
		dwCount -= dwLeft;
		if (!dwCount)
			break;
		pd += dwLeft;
		pm += dwLeft;
	}
#endif
}

/************************************************************************/
void mload24(int xs,int ys,int dx,int dy, LPTR pm, LPRGB pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
HPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoad( lpMask, xs, y, dx, tpm );
if (ColorMask.Mask && ColorMask.On)
	Shield24(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void MLoad24Huge(int xs,int ys,int dx,int dy, HPTR pm, HPRGB pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
int y, ye;
HPTR tpm;

ye = ys + dy - 1;
for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
	MaskLoaderHuge( lpMask, xs, y, dx, tpm, NO, CR_COPY );
if (ColorMask.Mask && ColorMask.On)
	Shield24Huge(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void Shield24(LPTR pm, LPRGB pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
BYTE mask;
LPTR RMask, GMask, BMask;
BOOL Include, NotInclude;

if (!dwCount)
	return;
RMask = ColorMask.RMask;
GMask = ColorMask.GMask;
BMask = ColorMask.BMask;
if (ColorMask.Include)
	{
	Include = 255;
	NotInclude = 0;
	}
else
	{
	Include = 0;
	NotInclude = 255;
	}
while (dwCount-- != 0)
	{
	if (!(mask = RMask[pd->red]))
		*pm++ &= NotInclude; /* not in range */
	else
	if (!(mask &= GMask[pd->green]))
		*pm++ &= NotInclude; /* not in range */
	else
	if ((mask &= BMask[pd->blue]))
		*pm++ &= Include; /* in range */
	else
		*pm++ &= NotInclude;	/* not in range */
	++pd;
	}
}

/************************************************************************/
void Shield24Huge(HPTR pm, HPRGB pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
	DWORD dwLeft, dwMskLeft;
	RGBS  dstRGB;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)pd & 0xFFFFL);
		dwLeft /= 3;
		if (!dwLeft) // RGB spans segment boundary
		{
			dstRGB.red = pd->red;
			dstRGB.green = pd->green;
			dstRGB.blue = pd->blue;
			Shield24(pm, &dstRGB, 1, TypeInfo);
			++pm;
			++pd;
			--dwCount;
		}
		else
		{
			dwMskLeft = 65536L - ((DWORD)pm & 0xFFFFL);
			if (dwMskLeft < dwLeft)
				dwLeft = dwMskLeft;
			// see if we will cross a segment boundary
			if (dwLeft > INT_MAX)
				dwLeft = INT_MAX; // never try to copy more than maxint bytes
			if (dwLeft > dwCount)
				dwLeft = dwCount;
			Shield24(pm, pd, (int)dwLeft, TypeInfo);
			dwCount -= dwLeft;
			pm += dwLeft;
			pd += dwLeft;
		}
	}
}

/************************************************************************/
void mload32(int xs,int ys,int dx,int dy, LPTR pm, LPCMYK pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
	int y, ye;
	LPTR tpm;

	ye = ys + dy - 1;
	for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
		MaskLoad( lpMask, xs, y, dx, tpm );

	if (ColorMask.Mask && ColorMask.On)
		Shield32(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void MLoad32Huge(int xs,int ys,int dx,int dy, HPTR pm, HPCMYK pd, FRMTYPEINFO TypeInfo, LPMASK lpMask)
/************************************************************************/
{
	int y, ye;
	HPTR tpm;

	ye = ys + dy - 1;
	for (y = ys, tpm = pm; y <= ye; ++y, tpm += dx)
		MaskLoaderHuge( lpMask, xs, y, dx, tpm, NO, CR_COPY );

	if (ColorMask.Mask && ColorMask.On)
		Shield32Huge(pm, pd, (DWORD)dx * (DWORD)dy, TypeInfo);
}

/************************************************************************/
void Shield32(LPTR pm, LPCMYK pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
	BYTE mask;
	LPTR CMask, MMask, YMask, KMask;
 
	if (!dwCount)
		return;
	CMask = ColorMask.CMask;
	MMask = ColorMask.MMask;
	YMask = ColorMask.YMask;
	KMask = ColorMask.KMask;

	if (ColorMask.Include)
	{
		while (dwCount-- != 0)
		{
			if (!(mask  = CMask[pd->c]))
				*pm++ &= 0; /* not in range */
			else
			if (!(mask &= MMask[pd->m]))
				*pm++ &= 0; /* not in range */
			else
			if (!(mask &= YMask[pd->y]))
				*pm++ &= 0; /* not in range */
			else
			if (!(mask &= KMask[pd->k]))
				*pm++ &= 0; /* not in range */
			else
				*pm++ &= 255; /* in range */
			++pd;
		}
	}
	else
	{
		while (dwCount-- != 0)
		{
			if (!(mask  = CMask[pd->c]))
				*pm++ &= 255; /* not in range */
			else
			if (!(mask &= MMask[pd->m]))
				*pm++ &= 255; /* not in range */
			else
			if (!(mask &= YMask[pd->y]))
				*pm++ &= 255; /* not in range */
			else
			if (!(mask &= KMask[pd->k]))
				*pm++ &= 255; /* not in range */
			else
				*pm++ &= 0;	/* in range */
			++pd;
		}
	}
}

/************************************************************************/
void Shield32Huge(HPTR pm, HPCMYK pd, DWORD dwCount, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
	DWORD dwLeft, dwMskLeft;

	// get current position in segment
	while (dwCount)
	{
		// get how many bytes left in segment
		dwLeft = 65536L - ((DWORD)pd & 0xFFFFL);
		dwLeft /= 4;
		dwMskLeft = 65536L - ((DWORD)pm & 0xFFFFL);
		if (dwMskLeft < dwLeft)
			dwLeft = dwMskLeft;
		// see if we will cross a segment boundary
		if (dwLeft > INT_MAX)
			dwLeft = INT_MAX; // never try to copy more than maxint bytes
		if (dwLeft > dwCount)
			dwLeft = dwCount;
		Shield32(pm, pd, (int)dwLeft, TypeInfo);
		dwCount -= dwLeft;
		pm += dwLeft;
		pd += dwLeft;
	}
}

#ifdef C_CODE
/************************************************************************/
void ScatterLine( LPTR lpSrc, LPTR lpDst, LPINT16 lpErrors, int dx,
                     int iPressure )
/************************************************************************/
{
static ULONG lSeed = 0xDEADBABA;
int error;

while (--dx >= 0)
	{
	error = iPressure;
	if ((error += *lpErrors) > 127 )
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
		*lpErrors++ = 0;
		*lpErrors += error;
		}
	else	{
		lSeed += lSeed;
		*lpErrors++ = error;
		}
	}
}
#endif

/************************************************************************/
void ScatterMask( LPTR lpSrc, LPTR lpDst, LPINT16 lpErrors, int dx, int dy,
                     int iPressure )
/************************************************************************/
{
clr( (LPTR)lpErrors, (dx+1)*sizeof(INT16) );
while (--dy >= 0)
	{
	ScatterLine(lpSrc, lpDst, lpErrors, dx, iPressure);
	lpSrc += dx;
	lpDst += dx;
	}
}

/************************************************************************/
void ScatterMaskHuge( HPTR lpSrc, HPTR lpDst, LPINT16 lpErrors, int dx, int dy,
                     int iPressure )
/************************************************************************/
{
DWORD dwLeft;
int iCount;
LPINT16 lpError;

clr( (LPTR)lpErrors, (dx+1)*sizeof(INT16) );
while (--dy >= 0)
	{
	iCount = dx;
	lpError = lpErrors;
	while (iCount)
		{
		dwLeft = 65536L - ((DWORD)lpSrc & 0xFFFFL);
		if (dwLeft > (DWORD)iCount)
			dwLeft = iCount;
		ScatterLine(lpSrc, lpDst, lpError, (int)dwLeft, iPressure);
		lpSrc += dwLeft;
		lpDst += dwLeft;
		lpError += dwLeft;
		iCount -= (int)dwLeft;
		}
	}
}

/************************************************************************/
void LoadColor(LPFRAME lpFrame, LPTR lpBuf, DWORD dwCount, LPCOLORINFO lpColor)
/************************************************************************/
{
	long lColor;
	FRMTYPEINFO TypeInfo;

	FrameGetTypeInfo(lpFrame, &TypeInfo);
	GetColorFromTypeInfo(TypeInfo, lpColor);
	switch(TypeInfo.DataType)
	{
		case FDT_LINEART   :
		case FDT_GRAYSCALE :
			hset(lpBuf, dwCount, lpColor->gray);
		break;

		case FDT_PALETTECOLOR :
			hset(lpBuf, dwCount, lpColor->index);
		break;

		case FDT_RGBCOLOR :
			lColor = 0;
			CopyRGB( &lpColor->rgb, &lColor );
			hset24((LPRGB)lpBuf, dwCount, lColor);
		break;

		case FDT_CMYKCOLOR :
			CopyCMYK( &lpColor->cmyk, &lColor );
			hset32((LPCMYK)lpBuf, dwCount, lColor);
		break;
	}
}

/***********************************************************************/
BOOL LoadSourceData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, int dx, int dy, HPTR hpData,
			LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip)
/***********************************************************************/
{
LPTR lp;
RECT rDest;
FRMDATATYPE Type;
int sx, sdx, sy, Xsize, Ysize, depth, ey, ex, nPixels = 0;
unsigned width, cwidth;
int left, iHeight;

// get pixmap info
PixmapGetInfo(lpPixmap, pmType, &Xsize, &Ysize, &depth, &Type);

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
	if (hpData)
		hpData += (long)(-y)*(long)width;
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
	if (hpData)
		hpData += sdx;
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

if (Type == FDT_LINEART)
	nPixels = cwidth / depth;

// after all that god damn checking, copy the fricking data from the frame
if (hpData)
	{
	y = rDest.top;
	left = rDest.left;
	iHeight = rDest.bottom - rDest.top + 1;
	while (--iHeight >= 0)
		{
		if (lp = PixmapPtr(lpPixmap, pmType, left, y, NO, nPixels))
			CopyToHuge(lp, hpData, cwidth);
		hpData += width;
		++y;
		}
	}

// Offset to make coordinates in rect relative to upper-left of buffer
*lpRect = rDest;
OffsetRect(lpRect, -sx, -sy);
return(TRUE);
}

/***********************************************************************/
BOOL LoadTiledData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, int dx, int dy, HPTR hpData,
			LPRECT lpRect,  BOOL HorzFlip, BOOL VertFlip)
/***********************************************************************/
{
LPTR lp;
int width, cwidth, awidth, iCount, nlines, Xsize, Ysize, ox, oy;
int xTile, yTile, depth2, depth, xpos, temp, pair_width, pair_height;
int ystart, yend, yinc, lines;

// set up the return rectangle, we can always fill it up
lpRect->top = lpRect->left = 0;
lpRect->bottom = dy - 1; lpRect->right = dx - 1;
if (!hpData)
	return(TRUE);

// get pixmap info
PixmapGetInfo(lpPixmap, pmType, &Xsize, &Ysize, &depth, NULL);

// width = width of a line in buffer in bytes
// cwidth = number of bytes to copy to buffer
cwidth = width = dx * depth;

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
								*hpData++ = *lp++;
							lp -= depth2;
							}
						}
					else
						{
					// no data - just advance pointers
					// and xpos
							hpData += awidth;
						xpos += (awidth/depth);
						}
				}
			else // even tile - same for normal and horz flip
				{
				// get amount of data left in this tile
				awidth = min(iCount, (Xsize-x)*depth);
				// get pointer to cache data
				if (lp = PixmapPtr(lpPixmap, pmType, x, y, NO))
					CopyToHuge(lp, hpData, awidth);
				// adjust counts and pointers
				iCount -= awidth;
				hpData += awidth;
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
			int x, int y, int dx, int dy, HPTR hpData,
			LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip)
/***********************************************************************/
{
LPTR lp;
int width, cwidth, sx, sdx, sy, Xsize, Ysize, depth, pixels, iy, yend, nPixels = 0;
void (FAR *lpMirrorProc)(HPTR,int);
RECT	mRect;
FRMDATATYPE Type;
 
// get pixmap info
PixmapGetInfo(lpPixmap, pmType, &Xsize, &Ysize, &depth, &Type);

if (HorzFlip)
	{
	switch ( depth )
		{
		case 1:
#ifdef WIN32
			lpMirrorProc = mirror8;
#else
			lpMirrorProc = Mirror8Huge;
#endif
			break;
		case 3:
#ifdef WIN32
			lpMirrorProc = mirror24;
#else
			lpMirrorProc = Mirror24Huge;
#endif
			break;
		case 4:
#ifdef WIN32
			lpMirrorProc = mirror32;
#else
			lpMirrorProc = Mirror32Huge;
#endif
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
	if (hpData)
		hpData += (-y)*width;
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
	if (hpData)
		hpData += sdx;
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
if (hpData)
	{
	if (Type == FDT_LINEART)
		nPixels = cwidth / depth;
	while (TRUE)
		{
		if (lp = PixmapPtr(lpPixmap, pmType, mRect.left, y, NO, nPixels))
			{
			CopyToHuge(lp, hpData, cwidth);
			if (HorzFlip)
				(*lpMirrorProc)(hpData, pixels);
			}
		if (y == yend)
			break;
		hpData += width;
		y += iy;
		}
	}

// Offset to make coordinates in rect relative to upper-left of buffer
OffsetRect(lpRect, -sx, -sy);
return(TRUE);
}

/************************************************************************/
void copyifset( LPTR lptr1, LPTR lptr2, int count )
/************************************************************************/
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
void TexturizeData24( LPRGB lpDst, LPTR lpMsk, int iCount )
/************************************************************************/
{
WORD	wMsk;

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
void TexturizeData8( LPTR lpDst, LPTR lpMsk, int iCount )
/************************************************************************/
{
WORD	wMsk;

while (--iCount >= 0)
	{
	wMsk = *lpMsk++ + 1;
	*lpDst++ = (wMsk * (WORD)(*lpDst)) >> 8;
	}
}
#endif

/************************************************************************/

void TexturizeData32(LPCMYK lpDst, LPTR lpMsk, int iCount)
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



