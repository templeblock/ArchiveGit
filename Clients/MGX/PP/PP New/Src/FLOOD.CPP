//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define ID_PROGRESS1 ID_BALL1
#define ID_PROGRESS2 ID_BALL2
#define ID_PROGRESS3 ID_BALL3
#define ID_PROGRESS4 ID_BALL4

#define MIN_STACK_SIZE 1024
#define MAX_HUES 252 /* must be divisible by 6 */
#define FIXHUE(hue) \
	((hue<0) ? hue+MAX_HUES:((hue>=MAX_HUES) ? hue-MAX_HUES: hue))

typedef BOOL (FAR *LPOK2FLOOD)(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPVOID lpParms);
typedef void (FAR *LPFADE)(LPTR lpInData, LPTR lpOutData, int dx, LPVOID lpParms);
typedef void (FAR *LPSIMILARPROC)(LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPVOID lpParms);
typedef void (FAR *LPFLOODPROCESS)(int x, int y, int nPixel, LPVOID lpParms);
				 
typedef struct _flooddata
{
	// fill by caller
	LPIMAGE lpImage;
	int x;				//file pixels
	int y;				//file pixels
	int Range;
	int Fade;			// 0 to 100
	int idColorModel;
	COLORINFO InColor;	// the color to base the flood on
	// filled on return
	RECT rFlood;
}FLOODDATA, FAR *LPFLOODDATA;

typedef struct _floodparms
{
	int	x;
	int	y;
	int	dir;
	int	pxl;
	int	pxr;
	int	xl;
	int	xr;
	BOOL bLine;
	int	where;
}FLOODPARMS, FAR *LPFLOODPARMS;

typedef struct _process_line
{
	LPTR lpColorBuf;		// buffer to hold solid color to be written
	LPTR lpMaskBuf;			// buffer to hold mask for area under map
	LPFRAME lpMapFrame;		// map of what has been filled
	POINT maskOffset;		// offset of mask from input frame
	POINT offset;			// offset of map from input frame(not fully implimented)
	POINT outOffset;		// offset of input frame from the output frame
	int OutWidth;			// the width of the output pixmap
	int OutHeight; 			// the height of the output pixmap
	RECT MapRect;			// area IN MAP coors on which to work; 
	LPPIXMAP lpOutPix;		// the pixmap to write into
	LPFRAME lpInFrame; 		// the frame to use when deciding in\out	
	LPMASK lpMask;			// Mask to use to combine fill to image
	BOOL bUseColorShield;	// Use mload ?
	BOOL bUseFade;			// Use fade ?
	LPFADE lpFadeProc;		// Proc to compute fade.
	LPVOID lpFadeData;		// Fade params
	int Opacity;			// Opacity for fill
	LPPROCESSPROC lpProcessProc;	// Process proc for writing filled data
} PROCESS_LINE, FAR * LPPROCESS_LINE;

// for remove holes
typedef struct _rhprocess_line
{
	LPPIXMAP lpPixmap;		// the pixmap to write into
//	int width;				// width of pixmap
//	int height;				// height of pixmap
} RHPROCESS_LINE, FAR * LPRHPROCESS_LINE;

typedef struct _do_fill
{
	int	x;
	int	y;
	RECT rFlood;					// returns area flooded (should set to empty)
	RECT rBound;					// the limits of the flood.
	LPOK2FLOOD lpOk2Flood;
	LPVOID lpOk2FloodData;
	LPFLOODPROCESS lpFloodProcess;	// process proc for flood fill
	LPVOID lpProcessLineData;	// data for process proc
	LPPIXMAP lpOutPix;			// the pixmap to write into
	BOOL fEscapable; 			// is the fill escapable?
	BOOL fAbortCheck;			// should we check for the abort?
}DO_FILL, FAR *LPDO_FILL;


typedef struct _decide8
{
	LPFRAME lpInFrame;
	LPFRAME lpMapFrame;
	int minval;
	int maxval;
	int fadeLut[256];
} DECIDE8, FAR * LPDECIDE8;

typedef struct _decide24
{
	LPFRAME lpInFrame;
	LPFRAME lpMapFrame;
	RGBS sRGB;
	long maxdistance;
	int fadeLut[256];
} DECIDE24, FAR * LPDECIDE24;

typedef struct _decide32
{
	LPFRAME lpInFrame;
	LPFRAME lpMapFrame;
	CMYKS sCMYK;
	long maxdistance;
	int fadeLut[256];
} DECIDE32, FAR * LPDECIDE32;

typedef struct _decideHSL
{
	LPFRAME lpInFrame;
	LPFRAME lpMapFrame;
	HSLS sHSL;
	long maxdistance;
	int fadeLut[256];
} DECIDEHSL, FAR * LPDECIDEHSL;

typedef struct _decide_rh
{
	LPFRAME lpInFrame;
} DECIDERH, FAR * LPDECIDERH;


// Static prototypes
STATUS_CODE Mgx_flood(LPFLOODDATA lpFloodData, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode);
STATUS_CODE Mgx_magic_mask(LPFLOODDATA lpFloodData, int iMode,  int dirty);
STATUS_CODE Mgx_magic_mask_similar(LPFLOODDATA lpFloodData, int iMode);

LOCAL STATUS_CODE FloodObject(LPOBJECT lpObject, LPFLOODDATA lpFloodData, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode);
LOCAL  STATUS_CODE MagicMaskObject(LPFLOODDATA lpFloodData, LPOBJECT lpObject, int iModes, int dirty);
LOCAL  STATUS_CODE MagicMaskSimilarObject(LPFLOODDATA lpFloodData, LPOBJECT lpObject, int iMode);

LOCAL  BOOL CreateFillBuffers(LPPROCESS_LINE lppl, int outdepth);
LOCAL  void FreeFillBuffers(LPPROCESS_LINE lppl);
LOCAL void ProcessLine(int x, int y, int nPixel, LPPROCESS_LINE lppl);
LOCAL void RemoveHolesProcessLine(int x, int y, int nPixel, LPRHPROCESS_LINE lprhpl);
LOCAL STATUS_CODE SimilarFillArea(LPSIMILARPROC lpSimilarProc, BYTE similarVal, LPVOID lpSimilarData, LPPROCESS_LINE lppl);

LOCAL LPOK2FLOOD GetOk2FillProc(LPFRAME lpInFrame, LPFRAME lpMapFrame, LPCOLORINFO lpColor, int Range, int Fade, int idColorModel, LPVOID FAR *lppData, LPFADE FAR* lppFadeProc);

LOCAL BOOL CheckMap8(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDE8 lpParms);
LOCAL BOOL CheckMap8PRGB(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDE24 lpParms);
LOCAL BOOL CheckMap8PHL(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDEHSL lpParms);
LOCAL BOOL CheckMap24RGB(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDE24 lpParms);
LOCAL BOOL CheckMap24HL(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDEHSL lpParms);
LOCAL BOOL CheckMap32RGB(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDE32 lpParms);
LOCAL BOOL CheckMap32HL(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDEHSL lpParms);
LOCAL BOOL CheckMapRemoveHoles(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight, LPDECIDERH lpParms);


LOCAL void FadeMap8(LPTR lpInData, LPTR lpOutData, int dx, LPDECIDE8 lpParms);
LOCAL void FadeMap24(LPRGB lpInData, LPTR lpOutData, int dx, LPDECIDE24 lpParms);
LOCAL void FadeMap24HL(LPRGB lpInData, LPTR lpOutData, int dx, LPDECIDEHSL lpParms);
LOCAL void FadeMap32RGB(LPCMYK lpInData, LPTR lpOutData, int dx, LPDECIDE32 lpParms);
LOCAL void FadeMap32HL(LPCMYK lpInData, LPTR lpOutData, int dx, LPDECIDEHSL lpParms);

#ifdef UNUSED
LOCAL BOOL CheckMap24HSL(int x,int y);
LOCAL BOOL CheckMap24H(int x,int y);
LOCAL BOOL CheckMap24S(int x,int y);
LOCAL BOOL CheckMap24L(int x,int y);
LOCAL BOOL CheckMap24HS(int x,int y);
LOCAL BOOL CheckMap24SL(int x,int y);
LOCAL BOOL CheckMap32H(int x,int y);   
LOCAL BOOL CheckMap32S(int x,int y);
LOCAL BOOL CheckMap32L(int x,int y);
LOCAL BOOL CheckMap32HS(int x,int y);
LOCAL BOOL CheckMap32SL(int x,int y);
LOCAL BOOL CheckMap32HSL(int x,int y);
#endif

LOCAL LPSIMILARPROC GetSimilarProc(LPFRAME lpInFrame, LPCOLORINFO lpColor, int Range, int Fade, int idColorModel, LPVOID FAR *lppData);
LOCAL void Similar8		(LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPDECIDE8 lpParms);
LOCAL void Similar8PRGB	(LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPDECIDE24 lpParms);
LOCAL void Similar8PHL	(LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPDECIDEHSL lpParms);
LOCAL void Similar24RGB	(LPTR lpDst, LPRGB lpSrc, int iCount, BYTE similarVal, LPDECIDE24 lpParms);
LOCAL void Similar24HL	(LPTR lpDst, LPRGB lpSrc, int iCount, BYTE similarVal, LPDECIDEHSL lpParms);
LOCAL void Similar32RGB	(LPTR lpDst, LPCMYK lpSrc, int iCount, BYTE similarVal, LPDECIDE32 lpParms);
LOCAL void Similar32HL	(LPTR lpDst, LPCMYK lpSrc, int iCount, BYTE similarVal, LPDECIDEHSL lpParms);

#ifdef UNUSED
LOCAL void Similar24H		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar24S		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar24L		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar24HS		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar24SL		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar24HSL	(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32RGB	(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32HSL	(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32H		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32S		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32L		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32HS		(LPTR lpDst, LPTR lpSrc, int iCount);
LOCAL void Similar32SL		(LPTR lpDst, LPTR lpSrc, int iCount);
#endif

LOCAL STATUS_CODE FloodFillArea(LPDO_FILL lpDoFill);
LOCAL STATUS_CODE do_region_fill(LPFLOODPARMS lpParms, long MaxCallDepth, LPDO_FILL lpDoFill);

//************************************************************************
BOOL FloodImage(LPIMAGE lpImage, LPFLOOD_PARMS lpParms)
//************************************************************************
{
int res;
LPFRAME lpFrame;
FLOODDATA FloodData;

if (!(lpFrame = ImgGetBaseEditFrame(lpImage)))
	return(FALSE);
res = FrameResolution(lpFrame);

FloodData.lpImage = lpImage;
FloodData.x = lpParms->x;
FloodData.y = lpParms->y;
ResConvertCenter(lpParms->iBaseRes, res, &FloodData.x, &FloodData.y);

FloodData.Range = lpParms->FloodRange;
FloodData.Fade = lpParms->FloodFade;
FloodData.idColorModel = lpParms->idFloodColorModel;
FloodData.InColor = lpParms->InColor;
AstralSetRectEmpty(&FloodData.rFlood);
ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty), NULL, NULL, FALSE /*no clock*/);
lpParms->Common.StatusCode = Mgx_flood(&FloodData, lpParms->FloodOpacity, &lpParms->Color, lpParms->FloodMergeMode);
ProgressEnd(TRUE);

if (!AstralIsRectEmpty(&FloodData.rFlood))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = FloodData.rFlood;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//************************************************************************
BOOL MagicMaskImage(LPIMAGE lpImage, LPMAGICMASK_PARMS lpParms)
//************************************************************************
{
int res;
LPFRAME lpFrame;
FLOODDATA FloodData;

if (!(lpFrame = ImgGetBaseEditFrame(lpImage)))
	return(FALSE);
res = FrameResolution(lpFrame);

FloodData.lpImage = lpImage;
FloodData.x = lpParms->x;
FloodData.y = lpParms->y;
ResConvertCenter(lpParms->iBaseRes, res, &FloodData.x, &FloodData.y);

FloodData.Range = lpParms->WandRange;
FloodData.Fade = lpParms->WandFade;
FloodData.idColorModel = lpParms->idMagicColorModel;
FloodData.InColor = lpParms->Color;
AstralSetRectEmpty(&FloodData.rFlood);

lpParms->Common.StatusCode = Mgx_magic_mask(&FloodData, lpParms->Mode, lpParms->Common.idDirty);
if (!AstralIsRectEmpty(&FloodData.rFlood))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = FloodData.rFlood;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//************************************************************************
BOOL SimilarImage(LPIMAGE lpImage, LPSIMILAR_PARMS lpParms)
//************************************************************************
{
int res;
LPFRAME lpFrame;
FLOODDATA FloodData;

if (!(lpFrame = ImgGetBaseEditFrame(lpImage)))
	return(FALSE);
res = FrameResolution(lpFrame);

FloodData.lpImage = lpImage;

// position used to identify object 
FloodData.x = lpParms->x;
FloodData.y = lpParms->y;
ResConvertCenter(lpParms->iBaseRes, res, &FloodData.x, &FloodData.y);

FloodData.Range = lpParms->WandRange;
FloodData.Fade = lpParms->WandFade;
FloodData.idColorModel = lpParms->idMagicColorModel;
FloodData.InColor = lpParms->Color;
AstralSetRectEmpty(&FloodData.rFlood);

ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty));
lpParms->Common.StatusCode = Mgx_magic_mask_similar(&FloodData, lpParms->Mode);
ProgressEnd();

if (!AstralIsRectEmpty(&FloodData.rFlood))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = FloodData.rFlood;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//************************************************************************
// uses flood fill to remove 'holes' from given frame.
//doesn't do undo stuff
//************************************************************************
STATUS_CODE MgxFloodRemoveHoles(LPPIXMAP lpPixmap, LPRECT rUpdate)
//***********************************************************************
{
	RHPROCESS_LINE pl;
	DO_FILL dofill;
	long x, y;
	int width, height;
	LPFRAME lpFrame;
	DECIDERH Ok2FloodData;
	LPTR lptr;
	STATUS_CODE StatusCode;

	AstralSetRectEmpty(rUpdate);

	clr((LPTR)&dofill, sizeof(dofill));

	lpFrame = PixmapFrame(lpPixmap, PMT_EDIT);
	width = FrameXSize(lpFrame);
	height = FrameYSize(lpFrame);
	
	// MAKE FRAME ALL 1's & 255's
	for (y=0;y<height;y++)
	{
		if (CANCEL)
		{
			if (CheckAbortInPlaceEdit(lpPixmap))
				return(SC_CANCELLED);
		}

		lptr = PixmapPtr(lpPixmap,PMT_EDIT,0, y, YES);
		for (x=0;x<width;x++)
		{
			if(*lptr < 128)
				*lptr++ = 1;
			else
				*lptr++ = 255;
		}
	}
	
	// FLOOD FILL ANYTHING NOT 255 WITH 0'S
	
	dofill.lpOk2FloodData = NULL;
	// the input frame comes from the Pixmap
	pl.lpPixmap = lpPixmap;
//	PixmapGetInfo(lpPixmap, NULL, &pl.width, &pl.height, NULL,NULL)
	
	// Fill the dofill structure & get fade info
	AstralSetRectEmpty(&dofill.rFlood);
	dofill.rBound.left = dofill.rBound.top = 0;
	dofill.rBound.right = width-1;
	dofill.rBound.bottom = height-1;
	dofill.lpOk2Flood = (LPOK2FLOOD)CheckMapRemoveHoles;
	Ok2FloodData.lpInFrame = lpFrame;
	dofill.lpOk2FloodData = (LPVOID)&Ok2FloodData;
	dofill.lpProcessLineData = (LPVOID)&pl;
	dofill.lpFloodProcess = (LPFLOODPROCESS)RemoveHolesProcessLine;
	dofill.lpOutPix = lpPixmap;
	dofill.fEscapable = TRUE;
	dofill.fAbortCheck = TRUE;
	
	// go around perimiter filling any 'holes'
	// ...Do 1st & last line
	for (y=0; y<height; y+=height-1)
	{
		lptr = PixmapPtr(lpPixmap,PMT_EDIT,0, y, NO);
		for (x=0;x<width;x++)
		{
			if (*lptr++ == 1)
			{
				dofill.x = x;
				dofill.y = y;
				StatusCode = FloodFillArea(&dofill);
				if (StatusCode == SC_CANCELLED)
					return(StatusCode);
				AstralUnionRect(rUpdate, rUpdate, &dofill.rFlood);
				lptr = PixmapPtr(lpPixmap,PMT_EDIT,x+1, y, NO);
			}
		}
	}
	// ...Do 1st & last pixel on every other line
	for (y=1; y<height-1; y++)
	{
		lptr = PixmapPtr(lpPixmap,PMT_EDIT,0, y, NO);
		for (x=0;x<width;x+=width-1)
		{
			if (lptr[x] == 1)
			{
				dofill.x = x;
				dofill.y = y;
				StatusCode = FloodFillArea(&dofill);
				AstralUnionRect(rUpdate, rUpdate, &dofill.rFlood);
				if (StatusCode == SC_CANCELLED)
					return(StatusCode);
				lptr = PixmapPtr(lpPixmap,PMT_EDIT,0, y, NO);
			}
		}
	}

	// REPLACE ANY 1's with 255
	for (y=0;y<height;y++)
	{
		if (CANCEL)
		{
			if (CheckAbortInPlaceEdit(lpPixmap))
				return(SC_CANCELLED);
		}
		lptr = PixmapPtr(lpPixmap,PMT_EDIT,0, y, YES);
		setifset(lptr,width, 255);
	}
	
	// since there is no easy way to determine effected area must say all
	rUpdate->left = 0;
	rUpdate->top = 0;
	rUpdate->right = width-1;
	rUpdate->bottom = height-1;
	
	return(SC_SUCCESS);
}

//************************************************************************
//	flood fills the selected object hit by x,y, based on its color at x,y.
// 	fills the with the given opacity and range, with the given color.
// 	x&y are in file coords.
//************************************************************************
LOCAL STATUS_CODE Mgx_flood(LPFLOODDATA lpFloodData, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode)
//************************************************************************
{
	LPOBJECT lpObject;
	RECT rMask;
	STATUS_CODE StatusCode;

	lpObject = ImgFindSelObject(lpFloodData->lpImage, lpFloodData->x, lpFloodData->y, NO);
	if (!lpObject)
		return(SC_BADPARMS);
	StatusCode = FloodObject(lpObject, lpFloodData, opacity, lpColor, MergeMode);

	if (!AstralIsRectEmpty(&lpFloodData->rFlood))
	{
		ImgGetMaskRect(lpFloodData->lpImage, &rMask);
		AstralIntersectRect(&lpFloodData->rFlood, &rMask, &lpFloodData->rFlood);
	}
	return(StatusCode);
}


//************************************************************************
// 	fills the object with the given opacity and range, with the given color.
// 	x&y are in file coords.
// 	x&y MUST be in the visable part of the object
//	Does caching.
//************************************************************************
LOCAL STATUS_CODE FloodObject(LPOBJECT lpObject, LPFLOODDATA lpFloodData, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode)
//************************************************************************
{
	int depth;
	PROCESS_LINE pl;
	DO_FILL dofill;
	int x, y;
	FRMTYPEINFO TypeInfo;
	STATUS_CODE StatusCode;
	
	dofill.lpOk2FloodData = NULL;
	
	// the Pixmap
	pl.lpOutPix = &(lpObject->Pixmap);
	PixmapGetInfo(pl.lpOutPix, 
			PMT_EDIT,
			&pl.OutWidth, 
			&pl.OutHeight,
			&depth,
			NULL);

	// adjust for palette color and line art images
	FrameGetTypeInfo(PixmapFrame(pl.lpOutPix, PMT_EDIT), &TypeInfo);
	if (TypeInfo.DataType == FDT_LINEART ||
		TypeInfo.DataType == FDT_PALETTECOLOR)
		opacity = 255;
	if (TypeInfo.DataType == FDT_LINEART)
		lpFloodData->Range = 0;

	// the input frame and the output framne are the same
	pl.outOffset.x = pl.outOffset.y = 0;
	pl.lpInFrame = PixmapFrame(pl.lpOutPix,PMT_EDIT);
	// move to object coords
	pl.maskOffset.x = lpObject->rObject.left;
	pl.maskOffset.y = lpObject->rObject.top;
	x = lpFloodData->x - pl.maskOffset.x;
	y = lpFloodData->y - pl.maskOffset.y;
	// the mask
	pl.lpMask = ImgGetMask(lpFloodData->lpImage);
	pl.bUseColorShield = TRUE;
	// 	the Process proc
	depth = depth ? depth:1;
	pl.lpProcessProc = GetProcessProc(MergeMode, TypeInfo.DataType);
	pl.Opacity = opacity;
	// setup buffers
	if (!CreateFillBuffers(&pl, depth))
		return(SC_MEMERROR);

	GetColorFromTypeInfo(TypeInfo, lpColor);
	LoadColor(PixmapFrame(pl.lpOutPix,PMT_EDIT), 
				pl.lpColorBuf, 
				RectWidth(&pl.MapRect), 
				lpColor);
				
	// Fill the dofill structure and get fade proc
	dofill.x = x-pl.offset.x;
	dofill.y = y-pl.offset.y;
	AstralSetRectEmpty(&dofill.rFlood);
	dofill.rBound = pl.MapRect;
	dofill.lpOk2Flood = GetOk2FillProc(pl.lpInFrame,
				pl.lpMapFrame,
				&lpFloodData->InColor,
				lpFloodData->Range, 
				lpFloodData->Fade,
				lpFloodData->idColorModel, 
				&dofill.lpOk2FloodData,
				&pl.lpFadeProc);
			
	pl.bUseFade = (pl.lpFadeProc != NULL);
	pl.lpFadeData = dofill.lpOk2FloodData;
				
	if (!dofill.lpOk2Flood)
		goto Exit;
	dofill.lpProcessLineData = &pl;
	dofill.lpFloodProcess = (LPFLOODPROCESS)ProcessLine;
	dofill.lpOutPix = pl.lpOutPix;
	dofill.fEscapable = TRUE;
	dofill.fAbortCheck = TRUE;
	
	// Prepare undo frame
	if (!ImgEditInit(lpFloodData->lpImage, ET_OBJECT, UT_DATA, lpObject))
		goto Exit;
	StatusCode = FloodFillArea(&dofill);
	// set undo
	lpFloodData->rFlood = dofill.rFlood;
	OffsetRect(&lpFloodData->rFlood, 
			pl.offset.x+pl.maskOffset.x, 
			pl.offset.y+pl.maskOffset.y);

	// if cancelled, do the recover, otherside set undo
	if (StatusCode == SC_CANCELLED)
		ImgEditRecoverObj(	lpFloodData->lpImage, lpObject,	TRUE, FALSE);
	else
		ImgEditedObject(lpFloodData->lpImage, lpObject, IDS_UNDOFLOOD, &lpFloodData->rFlood);

	if (dofill.lpOk2FloodData)
		FreeUp(dofill.lpOk2FloodData);
	FreeFillBuffers(&pl);
	return(StatusCode);
Exit:
	if (dofill.lpOk2FloodData)
		FreeUp(dofill.lpOk2FloodData);
	FreeFillBuffers(&pl);
	return(SC_MEMERROR);
}


//************************************************************************
// 	Does a magic mask.  
//	Flood fills into the mask channel based on the selected object hit by x,y. 
// 	Range is the range around the color at x,y to use.
// 	x&y are in file coords.
//************************************************************************
STATUS_CODE Mgx_magic_mask(LPFLOODDATA lpFloodData, int iMode,  int dirty)
//************************************************************************
{
	LPOBJECT lpObject;
	STATUS_CODE StatusCode;
	
	lpObject = ImgFindObject(lpFloodData->lpImage, lpFloodData->x, lpFloodData->y, NO, NO);
	if (!lpObject)
		return(SC_BADPARMS);
	ProgressBegin(1, PROGRESS_ID(dirty), NULL, NULL, FALSE /*no clock*/);
	StatusCode = MagicMaskObject(lpFloodData,lpObject, iMode, dirty);
	ProgressEnd(TRUE);
	return(StatusCode);
}

//************************************************************************
//	Flood fills into the mask channel based on the given object and its
//		color at x,y. 
// 	x&y are in file coords of base image.
// 	x&y should be in the visable part of the object
//	Does caching.
//	Note::: MapRect could be set better. (ie objects off image etc.)
//************************************************************************
LOCAL STATUS_CODE MagicMaskObject(LPFLOODDATA lpFloodData, LPOBJECT lpObject, int iMode, int dirty)
//************************************************************************
{

	RECT rUpdate, rOldMask;
	LPPIXMAP lpInPixmap;
	BOOL fNewmask;
	PROCESS_LINE pl;
	int outdepth;
	DO_FILL dofill;
	int x, y;
	STATUS_CODE StatusCode;
	FRMDATATYPE Type;

	AstralSetRectEmpty(&lpFloodData->rFlood);

	dofill.lpOk2FloodData = NULL;
	
	if (pl.lpMask = ImgGetMask(lpFloodData->lpImage))
		MaskRect(pl.lpMask, &rOldMask);
	else
		AstralSetRectEmpty(&rOldMask);

	// get or create mask channel	
	pl.lpMask = ImgGetMaskEx(lpFloodData->lpImage,
						iMode==SHAPE_SUBTRACT, 
						Control.NoUndo,
						&fNewmask, 
						&rUpdate);
	if (!pl.lpMask)
		return(SC_MEMERROR);

//	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rDummy);

	// the input frame comes from the object
	lpInPixmap = &(lpObject->Pixmap);
	pl.lpInFrame = PixmapFrame(lpInPixmap,PMT_EDIT);
	// get the masks Pixmap to output on.
	pl.lpOutPix = &(pl.lpMask->Pixmap);
	PixmapGetInfo(pl.lpOutPix, 
			PMT_EDIT,
			&pl.OutWidth, 
			&pl.OutHeight,
			&outdepth,
			NULL);
	Type = FrameType(PixmapFrame(pl.lpOutPix, PMT_EDIT));
	outdepth = outdepth ? outdepth:1;
	pl.outOffset.x = lpObject->rObject.left;
	pl.outOffset.y = lpObject->rObject.top;
	// move to object coords
	pl.maskOffset.x = lpObject->rObject.left;
	pl.maskOffset.y = lpObject->rObject.top;
	x = lpFloodData->x - pl.maskOffset.x;
	y = lpFloodData->y - pl.maskOffset.y;
	// no masking
	pl.lpMask = NULL;
	pl.bUseColorShield = NO;
	pl.Opacity = 255;
	// setup buffers
	if (!CreateFillBuffers(&pl, outdepth))
		return(SC_MEMERROR);
	set(pl.lpMaskBuf, RectWidth(&pl.MapRect), 255);
	// 	the Process proc
	switch (iMode)
	{
		case SHAPE_SUBTRACT:
			pl.lpProcessProc = GetProcessProc(MM_SUBTRACTIVE, Type);
			set(pl.lpColorBuf, outdepth*RectWidth(&pl.MapRect), 0); 
		break;
		case SHAPE_REVERSE:
		case SHAPE_ADD:
		default:
			pl.lpProcessProc = GetProcessProc(MM_NORMAL, Type);
			set(pl.lpColorBuf, outdepth*RectWidth(&pl.MapRect), 255); 
		break;
	}
	
	// Fill the dofill structure & get fade info
	dofill.x = x-pl.offset.x;
	dofill.y = y-pl.offset.y;
	AstralSetRectEmpty(&dofill.rFlood);
	dofill.rBound = pl.MapRect;
	dofill.lpOk2Flood = GetOk2FillProc(pl.lpInFrame,
				pl.lpMapFrame,
				&lpFloodData->InColor,
				lpFloodData->Range, 
				lpFloodData->Fade,
				lpFloodData->idColorModel, 
				&dofill.lpOk2FloodData,
				&pl.lpFadeProc);
			
	pl.bUseFade = (pl.lpFadeProc != NULL);
	pl.lpFadeData = dofill.lpOk2FloodData;
	
	if (!dofill.lpOk2Flood)
		goto Exit;
	dofill.lpProcessLineData = &pl;
	dofill.lpFloodProcess = (LPFLOODPROCESS)ProcessLine;
	dofill.lpOutPix = pl.lpOutPix;
	dofill.fEscapable = TRUE;
	dofill.fAbortCheck = !fNewmask;
	
	// Prepare undo frame
	if (!ImgEditInit(lpFloodData->lpImage, 
				ET_OBJECT, 
				fNewmask ? UT_CREATEMASK : UT_ALPHA,
				ImgGetBase(lpFloodData->lpImage)))
		goto Exit;
	EnableMarquee(NO);

	// actually do the flood fill
	
	ProgressBegin(1, dirty, NULL, NULL, FALSE /*no clock*/);
	StatusCode = FloodFillArea(&dofill);
	ProgressEnd(TRUE);

	// if user cancelled and we created a new mask
	// just nuke the mask
	if (StatusCode == SC_CANCELLED && fNewmask)
	{
		ImgSetMask(lpFloodData->lpImage, NULL);
		MaskClose(pl.lpMask);
	}
	else
	if (StatusCode != SC_MEMERROR) // if memory error, we didn't even start
	{
		// set undo
		lpFloodData->rFlood = dofill.rFlood;
		OffsetRect(&lpFloodData->rFlood, 
					pl.offset.x+pl.outOffset.x, 
					pl.offset.y+pl.outOffset.y);
		if (StatusCode == SC_CANCELLED)
		{
			ImgEditRecoverObj(	lpFloodData->lpImage,
								ImgGetBase(lpFloodData->lpImage),
								FALSE, TRUE);

		}
		else
		{
			ImgEditedObject(lpFloodData->lpImage, ImgGetBase(lpFloodData->lpImage), dirty,
						fNewmask ? (LPRECT)NULL : &lpFloodData->rFlood);
		}
		pl.lpMask = ImgGetMask(lpFloodData->lpImage);
		if (iMode!=SHAPE_ADD)
			MaskRectUpdate(pl.lpMask, &lpFloodData->rFlood);
		else
			AstralUnionRect(&pl.lpMask->rMask, &pl.lpMask->rMask, &lpFloodData->rFlood);
		if (ImgGetMaskUpdateRect(lpFloodData->lpImage, fNewmask, NO, &rUpdate))
			AstralUnionRect(&lpFloodData->rFlood, &lpFloodData->rFlood, &rUpdate);
  		AstralUnionRect(&lpFloodData->rFlood, &lpFloodData->rFlood, &rOldMask);
	}

	EnableMarquee(YES);
	if (dofill.lpOk2FloodData)
		FreeUp(dofill.lpOk2FloodData);
	FreeFillBuffers(&pl);
	return(StatusCode);
Exit:
	if (dofill.lpOk2FloodData)
		FreeUp(dofill.lpOk2FloodData);
	FreeFillBuffers(&pl);
	return(SC_MEMERROR);
}

//************************************************************************
// 	Masks all colors that would have been hit by a MgxMagicMask with the
//	same params.
//************************************************************************
STATUS_CODE Mgx_magic_mask_similar(LPFLOODDATA lpFloodData, int iMode)
//************************************************************************
{
	LPOBJECT lpObject;
	STATUS_CODE fRet;

	lpObject = ImgFindObject(lpFloodData->lpImage, lpFloodData->x, lpFloodData->y, NO, NO);
	if (!lpObject)
		return(SC_SUCCESS);
	fRet = MagicMaskSimilarObject(lpFloodData, lpObject, iMode);
	return(fRet);
}

//************************************************************************
// 	Masks all colors that would have been hit by a MagicMaskObject with the
//	same params.
//************************************************************************
LOCAL STATUS_CODE MagicMaskSimilarObject(LPFLOODDATA lpFloodData, LPOBJECT lpObject, int iMode)
//************************************************************************
{
	int outdepth;
	RECT rDummy;
	LPSIMILARPROC lpSimilarProc;
	LPVOID lpSimilarData = NULL;
	LPPIXMAP lpInPixmap;
	BOOL fNewmask;
	PROCESS_LINE pl;
	STATUS_CODE StatusCode;
	BYTE similarVal;

	// get or create mask channel	
	pl.lpMask = ImgGetMaskEx(lpFloodData->lpImage,OFF, Control.NoUndo, &fNewmask, &rDummy);
	if (!pl.lpMask)	 
		return(SC_MEMERROR);
//	UpdateImage(lpFloodData->lpImage, &rDummy, YES);

//	if ( Tool.hRibbon )
//		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
//	UpdateStatusBar( NO, NO, YES, NO );

	// the input frame comes from the object
	lpInPixmap = &(lpObject->Pixmap);
	pl.lpInFrame = PixmapFrame(lpInPixmap,PMT_EDIT);
	// get the masks Pixmap to output on.
	pl.lpOutPix = &(pl.lpMask->Pixmap);
	PixmapGetInfo(pl.lpOutPix, 
				PMT_EDIT,
				&pl.OutWidth, 
				&pl.OutHeight,
				&outdepth,
				NULL);
	outdepth = outdepth ? outdepth:1;
	pl.outOffset.x = lpObject->rObject.left;
	pl.outOffset.y = lpObject->rObject.top;
	pl.offset.x = pl.offset.y = 0;
	// move to object coords
	pl.maskOffset.x = lpObject->rObject.left;
	pl.maskOffset.y = lpObject->rObject.top;
	// no masking
	pl.lpMask = NULL;
	pl.bUseColorShield = NO;
	pl.bUseFade = FALSE;
	// 	setup similarVal
	switch (iMode)
	{
		case SHAPE_SUBTRACT:
			similarVal = 0;
		break;
		case SHAPE_REVERSE:
		case SHAPE_ADD:
		default:
			similarVal = 255;
		break;
	}
	pl.Opacity = 255;
	// Get lpSimilarProc proc
	lpSimilarProc = GetSimilarProc(pl.lpInFrame,&lpFloodData->InColor,lpFloodData->Range, lpFloodData->Fade, lpFloodData->idColorModel, &lpSimilarData);
	if (!lpSimilarProc)
		goto Exit;
	// Prepare undo frame
	if (!ImgEditInit(lpFloodData->lpImage, 
				ET_OBJECT, 
				fNewmask ? UT_CREATEMASK : UT_ALPHA,
				ImgGetBase(lpFloodData->lpImage)))
		goto Exit;
	EnableMarquee(NO);
	RemoveMaskMarquee(lpFloodData->lpImage);
	StatusCode = SimilarFillArea(lpSimilarProc, similarVal, lpSimilarData, &pl);
	// set undo
	lpFloodData->rFlood = ImgGetBase(lpFloodData->lpImage)->rObject;
	OffsetRect(&lpFloodData->rFlood, 
				pl.offset.x+pl.outOffset.x, 
				pl.offset.y+pl.outOffset.y);
	// check satatus codes
	if (StatusCode == SC_CANCELLED)
	{
		ImgEditRecoverObj( lpFloodData->lpImage,
							ImgGetBase(lpFloodData->lpImage),
							FALSE, TRUE);
	}
	else
	{
		ImgEditedObject(lpFloodData->lpImage, ImgGetBase(lpFloodData->lpImage), IDS_UNDOSIMILAR,
					fNewmask ? (LPRECT)NULL : &lpFloodData->rFlood);
	}
	pl.lpMask = ImgGetMask(lpFloodData->lpImage);
	if (iMode!=SHAPE_ADD)
		MaskRectUpdate(pl.lpMask, &rDummy);
	else
		AstralUnionRect(&pl.lpMask->rMask, &pl.lpMask->rMask, &lpFloodData->rFlood);
	EnableMarquee(YES);
	if (lpSimilarData)
		FreeUp(lpSimilarData);
	return(StatusCode);
Exit:
	if (lpSimilarData)
		FreeUp(lpSimilarData);
	return(SC_MEMERROR);
}


//************************************************************************/
//	Flood fills area.
//
//	The folowing must be set up before call:
//			lpOutFrame, lpMask, lpMapFrame, lpMap, offset,	
//			MapRect, Opacity, maskOffset, lpColorBuf, lpMaskBuf, 
//			lpProcessProc.
//										
//	On returns rFlood will be the area of the map frame which was flooded.
// 	x&y are in file co-ords of lpMapFrame.
//************************************************************************/
LOCAL STATUS_CODE FloodFillArea(LPDO_FILL lpDoFill)
//************************************************************************/
{
	long lParms;
	long MaxCallDepth;
	LPFLOODPARMS lpParms;
	STATUS_CODE StatusCode;
	
	MaxCallDepth = (65536L / (long)sizeof(FLOODPARMS)) - 1;
	lParms = MaxCallDepth * (long)sizeof(FLOODPARMS);
	if ( !(lpParms = (LPFLOODPARMS)Alloc(lParms)) )
		return(SC_MEMERROR);
	StatusCode = do_region_fill(lpParms, MaxCallDepth, lpDoFill);
	FreeUp((LPTR)lpParms);
	return(StatusCode);
}



//************************************************************************
// All the region fill routines below use an iterative approach to
// accomplishing the flood fill because of the restricted amount of
// stack we have makes the recursive approach inappropriate, even
// when the algorithm for this type of seed fill is obviously recursive.
//
// The approach works as follows:  Memory is allocated for a buffer
// which is used to save parameters in the region fill routine,
// essentially taking the place of the stack.  The buffer is currently
// set to 64K which allows for the equivalent of 3276 recursive calls,
// with recursion all we could get was 453 calls.  This amount could be
// increased, but you'd have to use something like BumpPtr() to
// calculate the addresses for saving and restoring the parameters.
// This buffer is pointed to by 'lpParms'.  Another variable
// 'CallDepth' is used to keep track of how deep we are in the calling
// sequence, similar to how many times we've pushed parameters on the
// stack without returning.  It's like our stack pointer.  The other
// key variable is 'where'.  This variable is used to keep track of
// where we are in the function.  It's like our instruction pointer.
// 'where' can take on the following values:
//	0 - This means a new call to the edge fill routine.  This is
//		just like making a recursive call.
//	1 - This means we are returning to the first place a recursive
//		call can be made from.  'ret' points to the return value.
//	2 - This means we are returning to the second place a recursive
//		call can be made from.  'ret' points to the return value.
//	3 - This means we are returning to the third place a recursive
//		call can be made from.  'ret' points to the return value.
//************************************************************************

/************************************************************************/
LOCAL STATUS_CODE do_region_fill(LPFLOODPARMS lpParms, long MaxCallDepth, LPDO_FILL lpDoFill)
/************************************************************************/
{
	FLOODPARMS fp;
	int ret;
	long CallDepth;
	BOOL fEscapable;
	RECT rFlood, rBound;
	LPOK2FLOOD lpOk2Flood;
	LPVOID lpOK2FloodData;
	LPVOID lpProcessLineData;
	LPFLOODPROCESS lpFloodProcess;
	LPPIXMAP lpOutPix;

	CallDepth = 1;
	fp.where = 0;
	fp.x = lpDoFill->x;
	fp.y = lpDoFill->y;
	fp.dir = 1;
	fp.pxl = fp.x;
	fp.pxr = fp.x;
	rFlood = lpDoFill->rFlood;
	rBound = lpDoFill->rBound;
	lpOk2Flood 			= lpDoFill->lpOk2Flood;   
	lpOK2FloodData		= lpDoFill->lpOk2FloodData;
	lpProcessLineData 	= lpDoFill->lpProcessLineData;
	lpFloodProcess 		= (LPFLOODPROCESS)lpDoFill->lpFloodProcess;
	lpOutPix			= lpDoFill->lpOutPix;
	fEscapable			= lpDoFill->fEscapable;
	
	while (CallDepth > 0)
	{
	StartCall:
		if (fp.where == 0)
		{
			if ( fp.y < rBound.top || fp.y > rBound.bottom || CallDepth >= MaxCallDepth )
			{
				ret = -1;
				goto DoReturn;
			}
			// check for user cancelling the functions
			if ( fEscapable && CANCEL )
			{
				if (!lpDoFill->fAbortCheck || CheckAbortInPlaceEdit(lpOutPix))
				{
					lpDoFill->rFlood = rFlood;
					return(SC_CANCELLED);
				}
			}

			AstralBeachCursor(ID_PROGRESS1);

			/* scan to determine endpts of seed line seg */
			fp.bLine = TRUE;
			/* scan to the left to determine endpts of seed line seg */
			(*lpOk2Flood)(0,YES,fp.x,fp.y,rBound.left,rBound.right, &fp.xl, &fp.xr, lpOK2FloodData);
			++fp.xl;
			--fp.xr;
			(*lpFloodProcess)(fp.xl, fp.y, (fp.xr-fp.xl+1), lpProcessLineData);
			if (fp.xl < rFlood.left)
				rFlood.left = fp.xl;
			if (fp.xr > rFlood.right)
				rFlood.right = fp.xr;
			if (fp.y < rFlood.top)
				rFlood.top = fp.y;
			if (fp.y > rFlood.bottom)
				rFlood.bottom = fp.y;
		}
		if (fp.where == 0 || fp.where == 1)
		{
			/* find and fill adjacent line segs in same direction */
			if (fp.where == 0)
			{
				fp.bLine = ((fp.y+fp.dir) >= rBound.top && (fp.y+fp.dir) <= rBound.bottom);
				fp.x = fp.xl;
			}
			else
			{
				fp.x = ret;
				if (fp.x >= rBound.left)
					++fp.x;
			}
			// search right for ok area
			if (fp.bLine && fp.x >= rBound.left && fp.x<=Min(rBound.right,fp.xr))
				if ((*lpOk2Flood)(1,NO,fp.x,fp.y+fp.dir,0,Min(rBound.right,fp.xr), NULL, &fp.x, lpOK2FloodData))
				{
					//SaveFloodParams(1);
					lpParms[CallDepth] = fp;
					lpParms[CallDepth].where = 1;
					++CallDepth;
					fp.where = 0;
					fp.y = fp.y+fp.dir;
					fp.pxl = fp.xl;
					fp.pxr = fp.xr;
					goto StartCall;
				}
			fp.where = 0;
		}
		if (fp.where == 0 || fp.where == 2)
		{
			if (fp.where == 0)
			{
				/* find and fill adjacent line segs in opposite direction */
				fp.bLine = ((fp.y-fp.dir) >= rBound.top && (fp.y-fp.dir) <= rBound.bottom);
				fp.x = fp.xl;
			}
			else
			{
				fp.x = ret;
				if (fp.x >= rBound.left)
					++fp.x;
			}
			// search right for ok area
			if (fp.bLine && fp.x >= rBound.left && fp.x<=Min(rBound.right, fp.pxl))
				if ((*lpOk2Flood)(1,NO,fp.x,fp.y-fp.dir,0,Min(rBound.right, fp.pxl), NULL, &fp.x, lpOK2FloodData))
				{
					lpParms[CallDepth] = fp;
					lpParms[CallDepth].where = 2;
					++CallDepth;
					fp.where = 0;
					fp.y = fp.y-fp.dir;
					fp.dir = -fp.dir;
					fp.pxl = fp.xl;
					fp.pxr = fp.xr;
					goto StartCall;
				}
			fp.where = 0;
		}
		if (fp.where == 0 || fp.where == 3)
		{
			if (fp.where == 0)
			{
				fp.bLine = ((fp.y-fp.dir) >= rBound.top && (fp.y-fp.dir) <= rBound.bottom);
				fp.x = fp.pxr;
			}
			else
			{
				fp.x = ret;
				if (fp.x >= rBound.left)
					++fp.x;
			}
			// search right for ok area
			if (fp.bLine && fp.x >= rBound.left && fp.x<=Min(rBound.right, fp.xr))
				if ((*lpOk2Flood)(1,NO,fp.x,fp.y-fp.dir,0,Min(rBound.right, fp.xr), NULL, &fp.x, lpOK2FloodData))
				{
					//SaveFloodParams(3);
					lpParms[CallDepth] = fp;
					lpParms[CallDepth].where = 3;
					++CallDepth;
					fp.where = 0;
					fp.y = fp.y-fp.dir;
					fp.dir = -fp.dir;
					fp.pxl = fp.xl;
					fp.pxr = fp.xr;
					goto StartCall;
				}
			fp.where = 0;
		}
		ret = fp.xr;
DoReturn:
		--CallDepth;
		//RestoreFloodParams();
		fp = lpParms[CallDepth];
	}	// end while
	lpDoFill->rFlood = rFlood;
	
	return(SC_SUCCESS);
}



//************************************************************************
//	Fills the Given lpDst buffer with the fill color for nPixels
//	Takes Opacity and the maksk into account.
// 	Updates the Map to exclude these points in the future.
// 	Note: x,y are in map coords.
//************************************************************************
LOCAL void ProcessLine(int x, int y, int nPixel, LPPROCESS_LINE lppl)
//************************************************************************
{
	LPTR lpDst;
	LPTR lpInData;
	int depth, xin, yin;
	LPTR lpMap;
	
	// set corresponding map bits  (must be done before bounding)
	if (lppl->lpMapFrame)
	{
		lpMap = FramePointer(lppl->lpMapFrame, 0, y, YES);
		lpMap += x;
		set(lpMap, nPixel, 255);
	}
	// check bounds
	yin = y+lppl->offset.y+lppl->outOffset.y;
	if (!WITHIN(yin, 0, lppl->OutHeight-1))
		return;
	xin = x+lppl->offset.x+lppl->outOffset.x;
	if (xin >= lppl->OutWidth)
		return;
	if (xin <0)
	{
		x -= xin;
		if (x >= lppl->MapRect.right)
			return;
		nPixel += xin;
		if (nPixel<=0)
			return;
		xin = 0;
	}
	if (xin+nPixel>lppl->OutWidth)
		nPixel = lppl->OutWidth-xin;
	// 	Get destination line 
	lpDst = PixmapPtr(lppl->lpOutPix,PMT_EDIT,xin, yin, YES);
	// get mask
	if (lppl->bUseColorShield)
	{
		// get source data for color shields
		depth = FrameDepth(lppl->lpInFrame);
		depth = depth ? depth:1;
		lpInData = FramePointer(lppl->lpInFrame, x+lppl->offset.x, y+lppl->offset.y, NO);
		mload(	xin,
				yin,
				nPixel,
				1, 
				lppl->lpMaskBuf, 
				lpInData, 
				FrameTypeInfo(lppl->lpInFrame), 
				lppl->lpMask);
	}	
	else if (lppl->lpMask)
		MaskLoad(lppl->lpMask, xin, yin, nPixel, lppl->lpMaskBuf);
	else if (lppl->bUseFade)
		set(lppl->lpMaskBuf, nPixel, 255);

	// do fase if needed
	if (lppl->bUseFade)
	{
		lpInData = FramePointer(lppl->lpInFrame, x+lppl->offset.x, y+lppl->offset.y, NO);
		(lppl->lpFadeProc)(lpInData,lppl->lpMaskBuf,nPixel, lppl->lpFadeData);
	}
	
	// add opacity
	if ((lppl->lpMask || lppl->bUseColorShield) && lppl->Opacity < 255)
		ScaleData8(lppl->lpMaskBuf, nPixel, (BYTE)lppl->Opacity);
		
	// put solid color into line (with mask)
	(*lppl->lpProcessProc)(lpDst, lppl->lpColorBuf, lppl->lpMaskBuf, nPixel);			 
}

//************************************************************************
//	Fills the Given lpDst buffer with the fill color for nPixels
//	For RemoveHoles
//************************************************************************
LOCAL void RemoveHolesProcessLine(int x, int y, int nPixel, LPRHPROCESS_LINE lprhpl)
//************************************************************************
{
	LPTR lpDst;
	
	// endure bounds
//	if (y < 0 || y>=lprpl.height)
//		return;
//	if (x<0)
//		x=0;
//	if ((x+nPixel) > lprpl.width)
//		nPixel = lprpl.width-x;
	// set pixels to 0's
	lpDst = PixmapPtr(lprhpl->lpPixmap,PMT_EDIT,x, y, YES);
	if (lpDst)
		clr(lpDst, nPixel);
}


//************************************************************************
//	Fills all areas of the output frame (ASSUMED depth = 1) allowed by 
//	lpSimilarProc.
//************************************************************************
LOCAL STATUS_CODE SimilarFillArea(LPSIMILARPROC lpSimilarProc, BYTE similarVal, LPVOID lpSimilarData, LPPROCESS_LINE lppl)
//************************************************************************
{
	int y;
	int width, height;
	LPTR lpSrc, lpDst;
	RECT rIn, rOut;
	POINT outOffset; 
	LPPIXMAP lpOutPix;
	LPFRAME lpInFrame;
	
	lpOutPix = lppl->lpOutPix;
	outOffset = lppl->outOffset;
	lpInFrame = lppl->lpInFrame;
	
	// check bounds
	// get input rect
	rIn.left = lppl->offset.x; 
	rIn.top = lppl->offset.y;
	rIn.right = FrameXSize(lpInFrame)-1;
	rIn.bottom = FrameYSize(lpInFrame)-1;
	// get output rect (adjusted back by outOffset)	
	rOut.left = Max(lppl->offset.x+outOffset.x,0);
	if (rOut.left >= lppl->OutWidth)	
		return(SC_SUCCESS);
	rOut.left -= outOffset.x;
	rOut.top = Max(lppl->offset.y+outOffset.y,0);
	if (rOut.top >= lppl->OutHeight)	
		return(SC_SUCCESS);
	rOut.top -= outOffset.y;
	rOut.right = (lppl->OutWidth-1-outOffset.x);
	rOut.bottom = (lppl->OutHeight-1-outOffset.y);
	// intersect input and output rects
	AstralIntersectRect(&rIn, &rIn, &rOut);
	if (rIn.left>rIn.right||rIn.top>rIn.bottom)
		return(SC_SUCCESS);
	width = RectWidth(&rIn);
	height = RectHeight(&rIn);
	for (y=rIn.top; y<=rIn.bottom; y++)
	{
		if (AstralClockCursor(y-rIn.top, height, YES))
		{
			if (CheckAbortInPlaceEdit(lpOutPix))
				return(SC_CANCELLED);
		}
		// 	Get destination line 
		lpDst = PixmapPtr(lpOutPix,PMT_EDIT,rIn.left+outOffset.x, y+outOffset.y, YES);
		// 	Get source line 
		lpSrc = FramePointer(lpInFrame,rIn.left, y, NO);
		// do it
		(*lpSimilarProc)(lpDst, lpSrc, width, similarVal, lpSimilarData);
	}
	return(SC_SUCCESS);
}

//************************************************************************
// 	Returns appropriate function.
//	Sets up all globals for that function.
// 	Based on lpInFrame.
//	Returns NULL for failure.
//	May fill lppData with data.  This should be freed by caller.
// 	Fills lpFadeProc with the appropriate fade proc (if any)
//************************************************************************
LOCAL LPOK2FLOOD GetOk2FillProc(LPFRAME lpInFrame, LPFRAME lpMapFrame, LPCOLORINFO lpColor, int Range, int Fade, int idColorModel, LPVOID FAR *lppData, LPFADE FAR* lppFadeProc)
//************************************************************************
{
	FRMTYPEINFO TypeInfo;
	int delta;
	int val, i;
	RGBS sRGB;
	CMYKS sCMYK;
	LPVOID lpData = NULL;
	LPOK2FLOOD lpProc = NULL;
	int x1,x2,x3,x4;
	
	if (lppFadeProc)
		*lppFadeProc = NULL;
	FrameGetTypeInfo(lpInFrame, &TypeInfo);
	GetColorFromTypeInfo(TypeInfo, lpColor);
	delta = TOGRAY(Range);
	// allocate data
	switch (TypeInfo.DataType)
	{
		case FDT_LINEART:
		case FDT_GRAYSCALE:
			if (!(lpData = Alloc(sizeof(DECIDE8))))
				goto Done;
			val = lpColor->gray;
			// get fade range
			x1 = val - delta;
			x4 = val + delta;
			val = (long)(x4-x1)*Fade/200L;
			x2 = x1 + val;
			x3 = x4 - val;
			x1 =  bound(x1, 0, 256 );
			x2 =  bound(x2, 0, 256 );
			x3 =  bound(x3, 0, 256 );
			x4 =  bound(x4, 0, 256 );
			
			((LPDECIDE8)lpData)->minval = x1;
			((LPDECIDE8)lpData)->maxval = x4;
			// return function
			lpProc = (LPOK2FLOOD)CheckMap8;
			if (lppFadeProc && Fade)
			{
				*lppFadeProc = (LPFADE)FadeMap8;
				// setup grayscale fade lut
				// lut is 1 to 1 scale value for a given color
				for (i=0;i<x1;i++)
					((LPDECIDE8)lpData)->fadeLut[i] = 0;
				for (;i<x2;i++)
					((LPDECIDE8)lpData)->fadeLut[i] = (i-x1)*255L/(x2-x1);
				for (;i<x3;i++)
					((LPDECIDE8)lpData)->fadeLut[i] = 255;
				for (;i<x4;i++)
					((LPDECIDE8)lpData)->fadeLut[i] = (x4-i)*255L/(x4-x3);
				for (;i<256;i++)
					((LPDECIDE8)lpData)->fadeLut[i] = 0;
			}
		break;
		case FDT_PALETTECOLOR:
			sRGB = TypeInfo.ColorMap->RGBData[lpColor->index];
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_HL:
					if (!(lpData = Alloc(sizeof(DECIDEHSL))))
						goto Done;
					((LPDECIDEHSL)lpData)->maxdistance = (long)delta*(long)delta;
					RGBtoHSL(sRGB.red,sRGB.green,sRGB.blue, &((LPDECIDEHSL)lpData)->sHSL);
//					((LPDECIDE8)lpData)->maxdistance*=2;
					lpProc = (LPOK2FLOOD)CheckMap8PHL;
				break;
				case IDC_MODEL_RGB:
				default:
					if (!(lpData = Alloc(sizeof(DECIDE24))))
						goto Done;
					((LPDECIDE24)lpData)->sRGB = sRGB;
					((LPDECIDE24)lpData)->maxdistance = (long)delta*(long)delta;
					((LPDECIDE24)lpData)->maxdistance*=3;
					lpProc = (LPOK2FLOOD)CheckMap8PRGB;
				break;
			}
		break;
		case FDT_RGBCOLOR:
			// return function
			sRGB = lpColor->rgb;
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_HL:
					if (!(lpData = Alloc(sizeof(DECIDEHSL))))
						goto Done;
					((LPDECIDEHSL)lpData)->maxdistance = (long)delta*(long)delta;
					RGBtoHSL(sRGB.red,sRGB.green,sRGB.blue, &((LPDECIDEHSL)lpData)->sHSL);
//					((LPDECIDE8)lpData)->maxdistance*=2;
					lpProc = (LPOK2FLOOD)CheckMap24HL;
					if (lppFadeProc && Fade)
					{
						// setup hl fade lut
						// based on distance from given color
						x2 = delta;
						val = (long)x2*Fade/100L;
						x1 = x2-val;
						for (i=0;i<x1;i++)
							((LPDECIDEHSL)lpData)->fadeLut[i] = 255;
						for (;i<x2;i++)
							((LPDECIDEHSL)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
						for (;i<256;i++)
							((LPDECIDEHSL)lpData)->fadeLut[i] = 0;
						*lppFadeProc = (LPFADE)FadeMap24HL;
					}
				break;
				case IDC_MODEL_RGB:
				default:
					if (!(lpData = Alloc(sizeof(DECIDE24))))
						goto Done;
					((LPDECIDE24)lpData)->sRGB = sRGB;
					((LPDECIDE24)lpData)->maxdistance = (long)delta*(long)delta;
					((LPDECIDE24)lpData)->maxdistance*=3;
					lpProc = (LPOK2FLOOD)CheckMap24RGB;
					if (lppFadeProc && Fade)
					{
						// setup rgb fade lut
						// based on distance from given color
						x2 = delta;
						val = (long)x2*Fade/100L;
						x1 = x2-val;
						for (i=0;i<x1;i++)
							((LPDECIDE24)lpData)->fadeLut[i] = 255;
						for (;i<x2;i++)
							((LPDECIDE24)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
						for (;i<256;i++)
							((LPDECIDE24)lpData)->fadeLut[i] = 0;
						*lppFadeProc = (LPFADE)FadeMap24;
					}
				break;
			}
		break;
		case FDT_CMYKCOLOR:
			// setup globals
			sCMYK = lpColor->cmyk;
			// return function
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_HL:
					if (!(lpData = Alloc(sizeof(DECIDEHSL))))
						goto Done;
					((LPDECIDEHSL)lpData)->maxdistance = (long)delta*(long)delta;
					CMYKtoHSL(sCMYK.c,sCMYK.m,sCMYK.y,sCMYK.k,&((LPDECIDEHSL)lpData)->sHSL);
//					maxdistance*=2;
					lpProc = (LPOK2FLOOD)CheckMap32HL;
					if (lppFadeProc && Fade)
					{
						// setup hl fade lut
						// based on distance from given color
						x2 = delta;
						val = (long)x2*Fade/100L;
						x1 = x2-val;
						for (i=0;i<x1;i++)
							((LPDECIDEHSL)lpData)->fadeLut[i] = 255;
						for (;i<x2;i++)
							((LPDECIDEHSL)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
						for (;i<256;i++)
							((LPDECIDEHSL)lpData)->fadeLut[i] = 0;
						*lppFadeProc = (LPFADE)FadeMap32HL;
					}
				break;
				case IDC_MODEL_RGB:
				default:
					if (!(lpData = Alloc(sizeof(DECIDE32))))
						goto Done;
					((LPDECIDE32)lpData)->sCMYK = sCMYK;
					((LPDECIDE32)lpData)->maxdistance = (long)delta*(long)delta;
					((LPDECIDE32)lpData)->maxdistance*=4;
					lpProc = (LPOK2FLOOD)CheckMap32RGB;
					if (lppFadeProc && Fade)
					{
						// setup cmyk fade lut
						// based on distance from given color
						x2 = delta;
						val = (long)x2*Fade/100L;
						x1 = x2-val;
						for (i=0;i<x1;i++)
							((LPDECIDE32)lpData)->fadeLut[i] = 255;
						for (;i<x2;i++)
							((LPDECIDE32)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
						for (;i<256;i++)
							((LPDECIDE32)lpData)->fadeLut[i] = 0;
						*lppFadeProc = (LPFADE)FadeMap32RGB;
					}
				break;
			}
		break;
	}
Done:
	if (lpData)
	{
	 	((LPDECIDE8)lpData)->lpInFrame = lpInFrame;
	 	((LPDECIDE8)lpData)->lpMapFrame = lpMapFrame;
	}
	*lppData = lpData;
	return(lpProc);
}


#define CHECKMAP(DATACHECK) \
{ \
	if (direction == 1 || !direction) \
	{ \
		pos = x + (direction == 0); \
		lpdata = lpData+pos/*+offset.x*/; \
		lpmap = lpMap+pos; \
		pos--; \
		while(++pos<=max) \
		{ \
			if (*lpmap++) \
				if (off) \
					break; \
				else \
				{ \
					lpdata++; \
					continue; \
				} \
			DATACHECK \
			lpdata++; \
		} \
		*lpRight = pos; \
		if (pos <= max) \
			bRet = TRUE; \
	} \
	if (direction == -1 || !direction)	 \
	{ \
		pos = x - (direction == 0); \
		lpdata = lpData+pos/*+offset.x*/; \
		lpmap = lpMap+pos; \
		pos++; \
		while(--pos>=min) \
		{ \
			if (*lpmap--) \
				if (off) \
					break; \
				else \
				{ \
					lpdata--; \
					continue; \
				} \
			DATACHECK \
			lpdata--; \
		} \
		*lpLeft = pos; \
		if (pos >= min) \
			bRet = TRUE; \
	} \
}



//************************************************************************
//	Use when lpInFrame is depth==8
// 	Checks the map to see if the location given is ok for filling.
//	Based on value of the lpMapFrame, and the value of lpInFrame;
//	Be sure x&y are in the map!
//************************************************************************
LOCAL BOOL CheckMap8(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDE8 lpParms)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPTR lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	int minval = lpParms->minval;
	int maxval = lpParms->maxval;
	
	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	lpData = FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	CHECKMAP(if(WITHIN(*lpdata, minval, maxval) != off)	break;)
	return(bRet);
}

//************************************************************************
//	Use when lpInFrame is depth==8
//  lpOutData is adjusted based on how far the colors in lpInData
//	vary from the base color.
//************************************************************************
LOCAL void FadeMap8(LPTR lpInData, LPTR lpOutData, int dx, LPDECIDE8 lpParms)
//************************************************************************
{
	LPTR lpout;
	LPTR lpin;
	int pos;
	int val;
	LPINT lut = lpParms->fadeLut;
	
	lpin = lpInData;
	lpout = lpOutData;
	for (pos=dx;pos>0;pos--, lpout++)
	{
		val = lut[*lpin++];
		if (val != 255)
		{
			// compute new out value
			*lpout = (BYTE)(*lpout*(long)val/255);
		}
	}
}

//************************************************************************
//	direction = 1 for right, -1 for left, 0 for both.
// 	searches from the specified point to the edge or limits.
//	the values returned in lpLeft, lpRight are for the first pixels which match the off bool.
//	return TRUE if an edge was hit (as opposed to hitting the limits.)
//	doesnt check the x point if direction == 0;
//************************************************************************
LOCAL BOOL CheckMap24RGB(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDE24 lpParms)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPRGB lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error, tError;
	RGBS sRGB = lpParms->sRGB;
	long maxdistance = lpParms->maxdistance;

	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = (LPRGB)FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	CHECKMAP(	{
				tError = (int)lpdata->red - (int)sRGB.red;
			  	tError *= tError;	
				Error = (int)lpdata->green - (int)sRGB.green;
			  	tError += Error*Error;	
				Error = (int)lpdata->blue - (int)sRGB.blue;
				if(((Error*Error+tError) > maxdistance) == off) break;
				})
	return(bRet);
}


//************************************************************************
//	Use when lpInFrame is depth==24
//  lpOutData is adjusted based on how far the colors in lpInData
//	vary from the base color.
//************************************************************************
LOCAL void FadeMap24(LPRGB lpInData, LPTR lpOutData, int dx, LPDECIDE24 lpParms)
//************************************************************************
{
	LPTR lpout;
	LPRGB lpin;
	int pos;
	int val;
	long Error, tError;
	LPINT lut = lpParms->fadeLut;
	RGBS sRGB = lpParms->sRGB;
	long maxdistance = lpParms->maxdistance;
	
	lpin = lpInData;
	lpout = lpOutData;
	for (pos=dx;pos>0;pos--, lpout++)
	{
		// get the error value
		tError = (int)lpin->red - (int)sRGB.red;
		tError *= tError;	
		Error = (int)lpin->green - (int)sRGB.green;
		tError += Error*Error;	
		Error = (int)lpin++->blue - (int)sRGB.blue;
		tError = Error*Error+tError;
		
		// compute 0-255 scaled distence
		val = (int)sqrt((double)(tError/3));
		
		// scale lpout appropriately
		val = lut[val];
		if (val != 255)
		{
			// compute new out value
			*lpout = (BYTE)(*lpout*(long)val/255);
		}
	}
}



//************************************************************************
LOCAL BOOL CheckMap24HL(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDEHSL lpParms)
//************************************************************************
{

	LPTR lpMap, lpmap;
	LPRGB lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error1, Error2;
	HSLS hsl;
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	
	
	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = (LPRGB)FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	CHECKMAP({
				RGBtoHL(lpdata->red,lpdata->green,lpdata->blue, &hsl);
				Error1 = (int)hsl.hue - (int)sHSL.hue;
				Error2 = -Error1;
				// get into range [0,360) degrees
				Error1 = FIXHUE(Error1);
				Error2 = FIXHUE(Error2);
				Error1 = Min(Error1, Error2);
				Error1 *= Error1;
				Error2 = (int)hsl.lum - (int)sHSL.lum;
				Error2 *= Error2;
				if (((Error1+Error2) > maxdistance) == off) break;
			})

	return(bRet);
}

//************************************************************************
//	Use when lpInFrame is depth==24
//  lpOutData is adjusted based on how far the colors in lpInData
//	vary from the base color.
//************************************************************************
LOCAL void FadeMap24HL(LPRGB lpInData, LPTR lpOutData, int dx, LPDECIDEHSL lpParms)
//************************************************************************
{
	LPTR lpout;
	LPRGB lpin;
	int pos;
	int val;
	LPINT lut = lpParms->fadeLut;
	long Error1, Error2;
	HSLS hsl;
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	
	lpin = lpInData;
	lpout = lpOutData;
	for (pos=dx;pos>0;pos--, lpout++)
	{
		RGBtoHL(lpin->red,lpin->green,lpin->blue, &hsl);
		lpin++;
		// get hue error
		Error1 = (int)hsl.hue - (int)sHSL.hue;
		Error2 = -Error1;
		// get into range [0,360) degrees
		Error1 = FIXHUE(Error1);
		Error2 = FIXHUE(Error2);
		Error1 = Min(Error1, Error2);
		Error1 *= Error1;
		// get lum error
		Error2 = (int)hsl.lum - (int)sHSL.lum;
		Error2 *= Error2;
		// get total error
		Error1 += Error2;
		
		// compute 0-255 scaled distence
		val = (int)sqrt((double)(Error1/2));
		
		// scale lpout appropriately
		val = lut[val];
		if (val != 255)
		{
			// compute new out value
			*lpout = (BYTE)(*lpout*(long)val/255);
		}
	}
}

//************************************************************************
LOCAL BOOL CheckMap8PRGB(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDE24 lpParms)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPTR lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error, tError;
	LPRGB lpRGBmap, lpRGB;
	RGBS sRGB = lpParms->sRGB;
	long maxdistance = lpParms->maxdistance;

	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	lpRGBmap = FrameGetColorMap(lpParms->lpInFrame)->RGBData;
	CHECKMAP(	{
				lpRGB = lpRGBmap + *lpdata;
				tError = (int)lpRGB->red - (int)sRGB.red;
			  	tError *= tError;	
				Error = (int)lpRGB->green - (int)sRGB.green;
			  	tError += Error*Error;	
				Error = (int)lpRGB->blue - (int)sRGB.blue;
				if(((Error*Error+tError) > maxdistance) == off) break;
				})
	return(bRet);
}


//************************************************************************
LOCAL BOOL CheckMap8PHL(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDEHSL lpParms)
//************************************************************************
{

	LPTR lpMap, lpmap;
	LPTR lpData, lpdata;
	LPRGB lpRGBmap, lpRGB;
	BOOL bRet = FALSE;
	int pos;
	long Error1, Error2;
	HSLS hsl;
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	
	
	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	lpRGBmap = FrameGetColorMap(lpParms->lpInFrame)->RGBData;
	CHECKMAP({
				lpRGB = lpRGBmap + *lpdata;
				RGBtoHL(lpRGB->red,lpRGB->green,lpRGB->blue, &hsl);
				Error1 = (int)hsl.hue - (int)sHSL.hue;
				Error2 = -Error1;
				// get into range [0,360) degrees
				Error1 = FIXHUE(Error1);
				Error2 = FIXHUE(Error2);
				Error1 = Min(Error1, Error2);
				Error1 *= Error1;
				Error2 = (int)hsl.lum - (int)sHSL.lum;
				Error2 *= Error2;
				if (((Error1+Error2) > maxdistance) == off) break;
			})

	return(bRet);
}

//************************************************************************
LOCAL BOOL CheckMap32RGB(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDE32 lpParms)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPCMYK lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error, tError;
	CMYKS sCMYK = lpParms->sCMYK;
	long maxdistance = lpParms->maxdistance;
	
	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = (LPCMYK)FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	CHECKMAP(	{
				tError = (int)lpdata->c - (int)sCMYK.c;
			  	tError *= tError;	
				Error = (int)lpdata->m - (int)sCMYK.m;
			  	tError += Error*Error;	
				Error = (int)lpdata->y - (int)sCMYK.y;
			  	tError += Error*Error;	
				Error = (int)lpdata->k - (int)sCMYK.k;
				if(((Error*Error+tError) > maxdistance) == off) break;
				})
	return(bRet);
}


//************************************************************************
//	Use when lpInFrame is depth==32
//  lpOutData is adjusted based on how far the colors in lpInData
//	vary from the base color.
//************************************************************************
LOCAL void FadeMap32RGB(LPCMYK lpInData, LPTR lpOutData, int dx, LPDECIDE32 lpParms)
//************************************************************************
{
	LPTR lpout;
	LPCMYK lpin;
	int pos;
	int val;
	long Error, tError;
	LPINT lut = lpParms->fadeLut;
	CMYKS sCMYK = lpParms->sCMYK;
	long maxdistance = lpParms->maxdistance;
	
	lpin = lpInData;
	lpout = lpOutData;
	for (pos=dx;pos>0;pos--, lpout++)
	{
		// get the error value
		tError = (int)lpin->c - (int)sCMYK.c;
		tError *= tError;	
		Error = (int)lpin->m - (int)sCMYK.m;
		tError += Error*Error;	
		Error = (int)lpin->y - (int)sCMYK.y;
		tError += Error*Error;	
		Error = (int)lpin->k - (int)sCMYK.k;
		tError = Error*Error+tError;
		
		// compute 0-255 scaled distence
		val = (int)sqrt((double)(tError/4));
		
		// scale lpout appropriately
		val = lut[val];
		if (val != 255)
		{
			// compute new out value
			*lpout = (BYTE)(*lpout*(long)val/255);
		}
	}
}

//************************************************************************
LOCAL BOOL CheckMap32HL(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDEHSL lpParms)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPCMYK lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error1, Error2;
	HSLS hsl;
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	
	
	lpMap = FramePointer(lpParms->lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = (LPCMYK)FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	CHECKMAP({
				CMYKtoHL(lpdata,&hsl);
				Error1 = (int)hsl.hue - (int)sHSL.hue;
				Error2 = -Error1;
				// get into range [0,360) degrees
				Error1 = FIXHUE(Error1);
				Error2 = FIXHUE(Error2);
				Error1 = Min(Error1, Error2);
				Error1 *= Error1;
				Error2 = (int)hsl.lum - (int)sHSL.lum;
				Error2 *= Error2;
				if (((Error1+Error2) > maxdistance) == off) break;
			})
	return(bRet);
}

//************************************************************************
//	Use when lpInFrame is depth==32
//  lpOutData is adjusted based on how far the colors in lpInData
//	vary from the base color.
//************************************************************************
LOCAL void FadeMap32HL(LPCMYK lpInData, LPTR lpOutData, int dx, LPDECIDEHSL lpParms)
//************************************************************************
{
	LPTR lpout;
	LPCMYK lpin;
	int pos;
	int val;
	LPINT lut = lpParms->fadeLut;
	long Error1, Error2;
	HSLS hsl;
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	
	lpin = lpInData;
	lpout = lpOutData;
	for (pos=dx;pos>0;pos--, lpout++)
	{
		CMYKtoHL(lpin,&hsl);
		lpin++;
		// get hue error
		Error1 = (int)hsl.hue - (int)sHSL.hue;
		Error2 = -Error1;
		// get into range [0,360) degrees
		Error1 = FIXHUE(Error1);
		Error2 = FIXHUE(Error2);
		Error1 = Min(Error1, Error2);
		Error1 *= Error1;
		// get lum error
		Error2 = (int)hsl.lum - (int)sHSL.lum;
		Error2 *= Error2;
		// get total error
		Error1 += Error2;
		
		// compute 0-255 scaled distence
		val = (int)sqrt((double)(Error1/2));
		
		// scale lpout appropriately
		val = lut[val];
		if (val != 255)
		{
			// compute new out value
			*lpout = (BYTE)(*lpout*(long)val/255);
		}
	}
}

//************************************************************************
//	Use for the RemoveHoles function.
// 	Checks the map to see if the location given is ok for filling.
//	Based on value of the lpInFrame;
//	Be sure x&y are in the map!
//************************************************************************
LOCAL BOOL CheckMapRemoveHoles(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight, LPDECIDERH lpParms)
//************************************************************************
{
	LPTR lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	BYTE val;

	lpData = FramePointer(lpParms->lpInFrame, 0, y/*+offset.y*/, NO);
	
	if (direction == 1 || !direction) 
	{ 
		pos = x + (direction == 0); 
		lpdata = lpData+pos; 
		pos--;
		while(++pos<=max) 
		{
			val = *lpdata++;
			// have we already been here?
			if (!val) 
			{
				if (off) 
					break; 
				else 
					continue; 
			}
			// is it out of bounds?
			if ((val == 1) != off)
				break;
		} 
		*lpRight = pos; 
		if (pos <= max) 
			bRet = TRUE; 
	} 
	if (direction == -1 || !direction)	 
	{ 
		pos = x - (direction == 0); 
		lpdata = lpData+pos/*+offset.x*/; 
		pos++; 
		while(--pos>=min) 
		{ 
			val = *lpdata--;
			// have we already been here?
			if (!val) 
			{
				if (off) 
					break; 
				else 
					continue; 
			}
			// is it out of bounds?
			if ((val == 1) != off)
				break;
		} 
		*lpLeft = pos; 
		if (pos >= min) 
			bRet = TRUE; 
	} 
	return(bRet);
}



//************************************************************************
// 	Returns appropriate function.
//	Sets up all globals for that function.
// 	Based on lpInFrame.
//	Returns NULL for failure.
//************************************************************************
LOCAL LPSIMILARPROC GetSimilarProc(LPFRAME lpInFrame, LPCOLORINFO lpColor, int Range, int Fade, int idColorModel, LPVOID FAR *lppData)
//************************************************************************
{
	FRMTYPEINFO TypeInfo;
	int delta;		
	int val;
	RGBS sRGB;
	CMYKS sCMYK;
	LPVOID lpData = NULL;
	LPSIMILARPROC lpProc = NULL;
	int x1,x2,x3,x4;
	int i;
	
	FrameGetTypeInfo(lpInFrame, &TypeInfo);
	GetColorFromTypeInfo(TypeInfo, lpColor);
	delta = TOGRAY(Range);
	// allocate data
	switch (TypeInfo.DataType)
	{
		case FDT_LINEART:
		case FDT_GRAYSCALE:
			if (!(lpData = Alloc(sizeof(DECIDE8))))
				goto Done;
			val = lpColor->gray;
			// get fade range
			x1 = val - delta;
			x4 = val + delta;
			val = (long)(x4-x1)*Fade/200L;
			x2 = x1 + val;
			x3 = x4 - val;
			x1 =  bound(x1, 0, 256 );
			x2 =  bound(x2, 0, 256 );
			x3 =  bound(x3, 0, 256 );
			x4 =  bound(x4, 0, 256 );
			
			((LPDECIDE8)lpData)->minval = x1;
			((LPDECIDE8)lpData)->maxval = x4;
			// return function
			lpProc = (LPSIMILARPROC)Similar8;
			// setup grayscale fade lut
			// lut is 1 to 1 scale value for a given color
			for (i=0;i<x1;i++)
				((LPDECIDE8)lpData)->fadeLut[i] = 0;
			for (;i<x2;i++)
				((LPDECIDE8)lpData)->fadeLut[i] = (i-x1)*255L/(x2-x1);
			for (;i<x3;i++)
				((LPDECIDE8)lpData)->fadeLut[i] = 255;
			for (;i<x4;i++)
				((LPDECIDE8)lpData)->fadeLut[i] = (x4-i)*255L/(x4-x3);
			for (;i<256;i++)
				((LPDECIDE8)lpData)->fadeLut[i] = 0;
		break;
		case FDT_PALETTECOLOR:
			// return function
			sRGB = TypeInfo.ColorMap->RGBData[lpColor->index];
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_HL:
					if (!(lpData = Alloc(sizeof(DECIDEHSL))))
						goto Done;
					((LPDECIDEHSL)lpData)->maxdistance = (long)delta*(long)delta;
					RGBtoHSL(sRGB.red,sRGB.green,sRGB.blue, &((LPDECIDEHSL)lpData)->sHSL);
//					((LPDECIDE8)lpData)->maxdistance*=2;
					lpProc = (LPSIMILARPROC)Similar8PHL;
				break;
				case IDC_MODEL_RGB:
				default:
					if (!(lpData = Alloc(sizeof(DECIDE24))))
						goto Done;
					((LPDECIDE24)lpData)->sRGB = sRGB;
					((LPDECIDE24)lpData)->maxdistance = (long)delta*(long)delta;
					((LPDECIDE24)lpData)->maxdistance*=3;
					lpProc = (LPSIMILARPROC)Similar8PRGB;
				break;
			}
		break;
		case 3:
			// return function
			sRGB = lpColor->rgb;
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_HL:
					if (!(lpData = Alloc(sizeof(DECIDEHSL))))
						goto Done;
					((LPDECIDEHSL)lpData)->maxdistance = (long)delta*(long)delta;
					RGBtoHSL(sRGB.red,sRGB.green,sRGB.blue, &((LPDECIDEHSL)lpData)->sHSL);
//					((LPDECIDE8)lpData)->maxdistance*=2;
					lpProc = (LPSIMILARPROC)Similar24HL;
					// setup hl fade lut
					// based on distance from given color
					x2 = delta;
					val = (long)x2*Fade/100L;
					x1 = x2-val;
					for (i=0;i<x1;i++)
						((LPDECIDEHSL)lpData)->fadeLut[i] = 255;
					for (;i<x2;i++)
						((LPDECIDEHSL)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
					for (;i<256;i++)
						((LPDECIDEHSL)lpData)->fadeLut[i] = 0;
				break;
				case IDC_MODEL_RGB:
				default:
					if (!(lpData = Alloc(sizeof(DECIDE24))))
						goto Done;
					((LPDECIDE24)lpData)->sRGB = sRGB;
					((LPDECIDE24)lpData)->maxdistance = (long)delta*(long)delta;
					((LPDECIDE24)lpData)->maxdistance*=3;
					lpProc = (LPSIMILARPROC)Similar24RGB;
					// setup rgb fade lut
					// based on distance from given color
					x2 = delta;
					val = (long)x2*Fade/100L;
					x1 = x2-val;
					for (i=0;i<x1;i++)
						((LPDECIDE24)lpData)->fadeLut[i] = 255;
					for (;i<x2;i++)
						((LPDECIDE24)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
					for (;i<256;i++)
						((LPDECIDE24)lpData)->fadeLut[i] = 0;
				break;
			}
		break;
		case FDT_CMYKCOLOR:
			// setup globals
			sCMYK = lpColor->cmyk;
			// return function
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_HL:
					if (!(lpData = Alloc(sizeof(DECIDEHSL))))
						goto Done;
					((LPDECIDEHSL)lpData)->maxdistance = (long)delta*(long)delta;
					CMYKtoHSL(sCMYK.c,sCMYK.m,sCMYK.y,sCMYK.k,&((LPDECIDEHSL)lpData)->sHSL);
//					maxdistance*=2;
					lpProc = (LPSIMILARPROC)Similar32HL;
					// setup hl fade lut
					// based on distance from given color
					x2 = delta;
					val = (long)x2*Fade/100L;
					x1 = x2-val;
					for (i=0;i<x1;i++)
						((LPDECIDEHSL)lpData)->fadeLut[i] = 255;
					for (;i<x2;i++)
						((LPDECIDEHSL)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
					for (;i<256;i++)
						((LPDECIDEHSL)lpData)->fadeLut[i] = 0;
				break;
				case IDC_MODEL_RGB:
				default:
					if (!(lpData = Alloc(sizeof(DECIDE32))))
						goto Done;
					((LPDECIDE32)lpData)->sCMYK = sCMYK;
					((LPDECIDE32)lpData)->maxdistance = (long)delta*(long)delta;
					((LPDECIDE32)lpData)->maxdistance*=4;
					lpProc = (LPSIMILARPROC)Similar32RGB;
					// setup cmyk fade lut
					// based on distance from given color
					x2 = delta;
					val = (long)x2*Fade/100L;
					x1 = x2-val;
					for (i=0;i<x1;i++)
						((LPDECIDE32)lpData)->fadeLut[i] = 255;
					for (;i<x2;i++)
						((LPDECIDE32)lpData)->fadeLut[i] = (x2-i)*255L/(x2-x1);
					for (;i<256;i++)
						((LPDECIDE32)lpData)->fadeLut[i] = 0;
				break;
			}
		break;
	}
Done:
	if (lpData)
	{
	 	((LPDECIDE8)lpData)->lpInFrame = lpInFrame;
	 	((LPDECIDE8)lpData)->lpMapFrame = NULL;
	}
	*lppData = lpData;
	return(lpProc);
}


//************************************************************************
//	for similar-- mimics CheckMap function.
//	lpDst is assumed depth 1.
//	if pixel is within range then it fills it with similarVal.
//	otherwise it is unchanged
//************************************************************************
LOCAL void Similar8 (LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPDECIDE8 lpParms)
//************************************************************************
{
	int val;
	LPINT lut = lpParms->fadeLut;
	
	while (iCount--)
	{
		val = lut[*lpSrc];
		if (val == 255)
			*lpDst=similarVal;
		else
		{
			// compute new out value
			*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
		}
		lpDst++;
		lpSrc++;
	}
}


//************************************************************************
//	for similar-- mimics CheckMap function.
//	lpDst is assumed depth 1.
//************************************************************************
//************************************************************************
LOCAL void Similar8PRGB	(LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPDECIDE24 lpParms)
//************************************************************************
{
	long Error, tError;
	RGBS sRGB = lpParms->sRGB;
	LPRGB lpRGBmap, lpRGB;
	long maxdistance = lpParms->maxdistance;
	int val;
	LPINT lut = lpParms->fadeLut;
	
	lpRGBmap = FrameGetColorMap(lpParms->lpInFrame)->RGBData;
	if (!lpRGBmap)
		return;
	while (iCount--)
	{
		lpRGB = lpRGBmap + *lpSrc++;
		// get the error value
		tError = (int)lpRGB->red - (int)sRGB.red;
		tError *= tError;	
		Error = (int)lpRGB->green - (int)sRGB.green;
		tError += Error*Error;	
		Error = (int)lpRGB->blue - (int)sRGB.blue;
		tError += Error*Error;
		
		if (tError <= maxdistance)
		{
			// compute 0-255 scaled distence
			val = (int)sqrt((double)(tError/3));
		
			// scale lpout appropriately
			val = lut[val];
			if (val == 255)
				*lpDst = similarVal;
			else
			{
				// compute new out value
				*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
			}
		}
		lpDst++;
	}
}

//************************************************************************
LOCAL void Similar8PHL(LPTR lpDst, LPTR lpSrc, int iCount, BYTE similarVal, LPDECIDEHSL lpParms)
//************************************************************************
{
	HSLS sHSL = lpParms->sHSL;
	LPRGB lpRGBmap, lpRGB;
	long maxdistance = lpParms->maxdistance;
	long Error1, Error2;
	LPINT lut = lpParms->fadeLut;
	HSLS hsl;
	int val;
	
	lpRGBmap = FrameGetColorMap(lpParms->lpInFrame)->RGBData;
	if (!lpRGBmap)
		return;
	while (iCount--)
	{
		lpRGB = lpRGBmap + *lpSrc++;
		RGBtoHL(lpRGB->red,lpRGB->green,lpRGB->blue, &hsl);
		// get hue error
		Error1 = (int)hsl.hue - (int)sHSL.hue;
		Error2 = -Error1;
		// get into range [0,360) degrees
		Error1 = FIXHUE(Error1);
		Error2 = FIXHUE(Error2);
		Error1 = Min(Error1, Error2);
		Error1 *= Error1;
		// get lum error
		Error2 = (int)hsl.lum - (int)sHSL.lum;
		Error2 *= Error2;
		// get total error
		Error1 += Error2;
		
		if (Error1 <= maxdistance)
		{		
			// compute 0-255 scaled distence
			val = (int)sqrt((double)(Error1/2));
		
			// scale lpout appropriately
			val = lut[val];
			if (val == 255)
				*lpDst = similarVal;
			else
			{
				// compute new out value
				*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
			}
		}
		lpDst++;
	}
}

//************************************************************************
//	for similar-- mimics CheckMap function.
//	lpDst is assumed depth 1.
//************************************************************************
//************************************************************************
LOCAL void Similar24RGB	(LPTR lpDst, LPRGB lpSrc, int iCount, BYTE similarVal, LPDECIDE24 lpParms)
//************************************************************************
{
	long Error, tError;
	RGBS sRGB = lpParms->sRGB;
	long maxdistance = lpParms->maxdistance;
	int val;
	LPINT lut = lpParms->fadeLut;
	
	while (iCount--)
	{
		// get the error value
		tError = (int)lpSrc->red - (int)sRGB.red;
		tError *= tError;	
		Error = (int)lpSrc->green - (int)sRGB.green;
		tError += Error*Error;	
		Error = (int)lpSrc->blue - (int)sRGB.blue;
		tError += Error*Error;
		
		if (tError <= maxdistance)
		{
			// compute 0-255 scaled distence
			val = (int)sqrt((double)(tError/3));
		
			// scale lpout appropriately
			val = lut[val];
			if (val == 255)
				*lpDst = similarVal;
			else
			{
				// compute new out value
				*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
			}
		}
		lpDst++;
		lpSrc++;
	}
}

//************************************************************************
LOCAL void Similar24HL(LPTR lpDst, LPRGB lpSrc, int iCount, BYTE similarVal, LPDECIDEHSL lpParms)
//************************************************************************
{
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	long Error1, Error2;
	LPINT lut = lpParms->fadeLut;
	HSLS hsl;
	int val;
	
	while (iCount--)
	{
		RGBtoHL(lpSrc->red,lpSrc->green,lpSrc->blue, &hsl);
		// get hue error
		Error1 = (int)hsl.hue - (int)sHSL.hue;
		Error2 = -Error1;
		// get into range [0,360) degrees
		Error1 = FIXHUE(Error1);
		Error2 = FIXHUE(Error2);
		Error1 = Min(Error1, Error2);
		Error1 *= Error1;
		// get lum error
		Error2 = (int)hsl.lum - (int)sHSL.lum;
		Error2 *= Error2;
		// get total error
		Error1 += Error2;
		
		if (Error1 <= maxdistance)
		{		
			// compute 0-255 scaled distence
			val = (int)sqrt((double)(Error1/2));
		
			// scale lpout appropriately
			val = lut[val];
			if (val == 255)
				*lpDst = similarVal;
			else
			{
				// compute new out value
				*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
			}
		}
		lpDst++;
		lpSrc++;
	}
}


//************************************************************************
//	for similar-- mimics CheckMap function.
//	lpDst is assumed depth 1.
//************************************************************************
//************************************************************************
LOCAL void Similar32RGB	(LPTR lpDst, LPCMYK lpSrc, int iCount, BYTE similarVal, LPDECIDE32 lpParms)
//************************************************************************
{

	long Error, tError;
	CMYKS sCMYK = lpParms->sCMYK;
	long maxdistance = lpParms->maxdistance;
	int val;
	LPINT lut = lpParms->fadeLut;

	while (iCount--)
	{
		tError = (int)lpSrc->c - (int)sCMYK.c;
		tError *= tError;	
		Error = (int)lpSrc->m - (int)sCMYK.m;
		tError += Error*Error;	
		Error = (int)lpSrc->y - (int)sCMYK.y;
		tError += Error*Error;	
		Error = (int)lpSrc->k - (int)sCMYK.k;
		tError += Error*Error;	
		if (tError <= maxdistance)
		{
			// compute 0-255 scaled distence
			val = (int)sqrt((double)(tError/4));
		
			// scale lpout appropriately
			val = lut[val];
			if (val == 255)
				*lpDst = similarVal;
			else
			{
				// compute new out value
				*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
			}
		}
		lpSrc++;
		lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32HL(LPTR lpDst, LPCMYK lpSrc, int iCount, BYTE similarVal, LPDECIDEHSL lpParms)
//************************************************************************
{
	long Error1, Error2;
	HSLS hsl;
	HSLS sHSL = lpParms->sHSL;
	long maxdistance = lpParms->maxdistance;
	LPINT lut = lpParms->fadeLut;
	int val;
	
	while (iCount--)
	{
		CMYKtoHL(lpSrc,&hsl);
		Error1 = (int)hsl.hue - (int)sHSL.hue;
		Error2 = -Error1;
		// get into range [0,360) degrees
		Error1 = FIXHUE(Error1);
		Error2 = FIXHUE(Error2);
		Error1 = Min(Error1, Error2);
		Error1 *= Error1;
		Error2 = (int)hsl.lum - (int)sHSL.lum;
		Error1 += Error2*Error2;
		if (Error1 <= maxdistance)
		{
			// compute 0-255 scaled distence
			val = (int)sqrt((double)(Error1/2));
		
			// scale lpout appropriately
			val = lut[val];
			if (val == 255)
				*lpDst = similarVal;
			else
			{
				// compute new out value
				*lpDst = (BYTE)((similarVal*(long)val + (long)(255-val)* *lpDst)/255);
			}
		}
		lpSrc++;
		lpDst++;
	}
}


#ifdef UNUSED
//************************************************************************
LOCAL void Similar24HSL(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance24HSL(lpSrc,(LPTR)&sRGB) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=3;;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar24H(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance24H(lpSrc,(LPTR)&sRGB) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=3;;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar24S(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance24S(lpSrc,(LPTR)&sRGB) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=3;;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar24L(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance24L(lpSrc,(LPTR)&sRGB) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=3;;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar24HS(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance24HS(lpSrc,(LPTR)&sRGB) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=3;;
		*lpDst++;
	}
}


//************************************************************************
LOCAL void Similar24SL(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	long Error1, Error2;
	HSLS hsl;
	
	while (iCount--)
	{
		RGBtoHL(((LPRGB)lpSrc)->red,((LPRGB)lpSrc)->green,((LPRGB)lpSrc)->blue, &hsl);
		Error1 = (int)hsl.hue - (int)sHSL.hue;
		Error2 = -Error1;
		// get into range [0,360) degrees
		Error1 = FIXHUE(Error1);
		Error2 = FIXHUE(Error2);
		Error1 = Min(Error1, Error2);
		Error1 *= Error1;
		Error2 = (int)hsl.lum - (int)sHSL.lum;
		Error2 *= Error2;
		if ((Error1+Error2) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=3;;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32HSL(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance32HSL(lpSrc,(LPTR)&sCMYK) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=4;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32H(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance32H(lpSrc,(LPTR)&sCMYK) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=4;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32S(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance32S(lpSrc,(LPTR)&sCMYK) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=4;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32L(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance32L(lpSrc,(LPTR)&sCMYK) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=4;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32HS(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance32HS(lpSrc,(LPTR)&sCMYK) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=4;
		*lpDst++;
	}
}

//************************************************************************
LOCAL void Similar32SL(LPTR lpDst, LPTR lpSrc, int iCount)
//************************************************************************
{
	while (iCount--)
	{
		if (Distance32SL(lpSrc,(LPTR)&sCMYK) <= maxdistance)
			*lpDst=similarVal;
		lpSrc+=4;
		*lpDst++;
	}
}
#endif

//************************************************************************
//	Sets buffers needed to fill, based on lpInFrame:
//		lpMapFrame, lpMaskFrame, lpColorBuf. 
//	Also sets offset & MapRect.
// 	outdepth should be the depth of the output frame.
//************************************************************************
LOCAL  BOOL CreateFillBuffers(LPPROCESS_LINE lppl, int outdepth)
//***********************************************************************
{
	lppl->lpMapFrame = NULL;
	lppl->lpMaskBuf = lppl->lpColorBuf = NULL;
	if (!lppl->lpInFrame)
		return(FALSE);
	// MapRect can be made smaller in the future
	lppl->MapRect.left = lppl->MapRect.top = 0;
	lppl->MapRect.right = FrameXSize(lppl->lpInFrame)-1;
	lppl->MapRect.bottom = FrameYSize(lppl->lpInFrame)-1;
	// if offset is ever set to other than 0,0 the OK2FLOOD Procs
	// 		will need to be updated.
	lppl->offset.x = lppl->offset.y = 0;

	// setup lpMapFrame	
//	lpMapFrame = FrameOpen(FDT_LINEART, RectWidth(&MapRect), RectHeight(&MapRect), 1); 
	lppl->lpMapFrame = FrameOpen(FDT_GRAYSCALE, 
					RectWidth(&lppl->MapRect), 
					RectHeight(&lppl->MapRect), 
					1); 
	if (!lppl->lpMapFrame)
		return(FALSE);
	FrameSetBackground(lppl->lpMapFrame, 0);
	// setup lpMaskBuf
	lppl->lpMaskBuf = Alloc((long)FrameXSize(lppl->lpMapFrame));
	if (!lppl->lpMaskBuf)
		goto Exit;
	// setup color buffer 
	lppl->lpColorBuf = Alloc((long)FrameXSize(lppl->lpMapFrame)*outdepth);
	if (!lppl->lpColorBuf)
		goto Exit;
	return(TRUE);
Exit:
	if (lppl->lpMapFrame)
		FrameClose(lppl->lpMapFrame);
	if (lppl->lpMaskBuf)
		FreeUp(lppl->lpMaskBuf);
	if (lppl->lpColorBuf)
		FreeUp(lppl->lpColorBuf);
	return(FALSE);
}

//************************************************************************
//	Frees buffers needed to fill, based on lpInFrame:
//		lpMapFrame, lpMaskFrame, lpColorBuf. 
//************************************************************************
LOCAL void FreeFillBuffers(LPPROCESS_LINE lppl)
//***********************************************************************
{
	if (lppl->lpMapFrame)
		FrameClose(lppl->lpMapFrame);
	if (lppl->lpMaskBuf)
		FreeUp(lppl->lpMaskBuf);
	if (lppl->lpColorBuf)
		FreeUp(lppl->lpColorBuf);
}

