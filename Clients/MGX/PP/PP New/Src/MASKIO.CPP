// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "mask.h"
#include "maskutil.h"

// LOCAL prototypes			
LOCAL STATUS_CODE FeatherAlpha(LPIMAGE lpImage, LPOBJECT lpObject, int Inside, int size, int edge, BOOL fNoUndo, LPRECT lpUpdateRect, LPBOOL lpRecovered);
LOCAL LPMASK ReadOldMask(LPSTR szClipName, int iMaxWidth, int iMaxHeight, FRMDATATYPE FrameType);
LOCAL LPCPolyList ReadOldPolyList( FILE_HANDLE fp, BOOL fSwap, LPCPolyList lpPolyList );
LOCAL LPFRAME StrokeAlpha(LPIMAGE lpImage, LPALPHA lpAlpha, HMGXBRUSH hBrush, BOOL bInside, LPRECT lpRect);
LOCAL BOOL EllipseMaskImage(LPIMAGE lpImage, LPRECT lpRect, int iModes,
						ITEMID idDirty, LPRECT lpUpdateRect);
LOCAL BOOL PolyMaskImage(LPIMAGE lpImage, LPCPoly lpPoly, LPCPolyList lpPolyList, int iMode, BOOL bAntialias, ITEMID idDirty, LPRECT lpUpdateRect, BOOL bInitUndo);
LOCAL BOOL tform_poly(LPCPoly lpPoly, LPTFORM lpTForm);
LOCAL STATUS_CODE MaskSmooth_Dots(LPPIXMAP lpPixMap, int gauss, int thresh, LPRECT rWork, LPRECT lprUpdate);

#define TIF_II (0x4949)
#define MYORDER (TIF_II)

/************************************************************************/
BOOL ShapeMaskImage(LPIMAGE lpImage, LPSHAPEMASK_PARMS lpParms)
/************************************************************************/
{
	RECT SelectRect, rUpdate;
	int res;
	CPoly Poly;
	BOOL fRet;
	
	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	SetRect(&SelectRect, lpParms->x1, lpParms->y1, lpParms->x2, lpParms->y2);
	ResConvertRect(lpParms->iBaseRes, res, &SelectRect);
	
	AstralSetRectEmpty(&rUpdate);
	
	if (lpParms->Shape)
	{
		fRet = EllipseMaskImage( lpImage, &SelectRect, lpParms->Mode, 
							lpParms->Common.idDirty, &rUpdate);
	}
	else
	{
		MakeRectPoly(&Poly, &SelectRect, YES);
		fRet = PolyMaskImage(lpImage, &Poly, NULL, lpParms->Mode,	NO /*Antialias*/,
								lpParms->Common.idDirty, &rUpdate, YES);
	}
	if (fRet && !AstralIsRectEmpty(&rUpdate))
	{
		lpParms->Common.UpdateType = UT_ACTIVEAREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	ProgressEnd();
	return(fRet);
}

/************************************************************************/
BOOL FreeMaskImage(LPIMAGE lpImage, LPFREEMASK_PARMS lpParms)
/************************************************************************/
{
	RECT rUpdate;
	int res;
	BOOL fRet;
	CPolyList PolyList;
	
	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));

	// convert back to a polygon & scale
	if (!IntPoints2PolyList(&PolyList, lpParms->lpPoints,lpParms->nPoints, lpParms->iBaseRes, res))
		return(FALSE);
	
	AstralSetRectEmpty(&rUpdate);
	fRet = PolyMaskImage( lpImage, NULL, &PolyList, lpParms->Mode, lpParms->Antialias,
				   		lpParms->Common.idDirty, &rUpdate, YES);

 	FreePolys(&PolyList);
	
	if (fRet && !AstralIsRectEmpty(&rUpdate))
	{
		lpParms->Common.UpdateType = UT_ACTIVEAREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	ProgressEnd();
	return(fRet);
}

/************************************************************************/
BOOL PointEditImage(LPIMAGE lpImage, LPPOINTEDIT_PARMS lpParms)
/************************************************************************/
{
	return(PointEditImageEx(lpImage,lpParms, YES));
}


//	NOTE: if bInitUndo it will call ImgEditInit BUT WILL call ImgEditedObject 
/************************************************************************/
BOOL PointEditImageEx(LPIMAGE lpImage, LPPOINTEDIT_PARMS lpParms, BOOL bInitUndo)
/************************************************************************/
{
	RECT rUpdate;
	int res;
	BOOL fRet, fNewMask;
	LPOBJECT lpBase;
	LPMASK lpMask;
	CPolyList PolyList;
	RECT rErase;
	
	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));

    AstralSetRectEmpty(&rUpdate);

	rErase = lpParms->rErase;
	
	if (AstralIsRectEmpty(&rErase))
	    AstralSetRectEmpty(&rErase);
	else
		ResConvertRect(lpParms->iBaseRes, res, &rErase);
		
	// convert back to a polygon
	if (!IntPoints2PolyList(&PolyList, lpParms->lpPoints,lpParms->nPoints, lpParms->iBaseRes, res))
		return(FALSE);
	
	// init undo if needed
	if (bInitUndo && !AstralIsRectEmpty(&rErase))
	{
		// get mask, create one if one does not exist
		lpMask = ImgGetMaskEx(lpImage, (lpParms->Mode==SHAPE_SUBTRACT), lpParms->Common.idDirty,
								 &fNewMask, &rUpdate);
		if (!lpMask)
			return(FALSE);
		if (fNewMask && (lpParms->Mode==SHAPE_SUBTRACT))
			ImgGetImageRect(lpImage, &rUpdate);

		lpBase = ImgGetBase(lpImage);
		if (!ImgEditInit(lpImage, ET_OBJECT, fNewMask ? UT_CREATEMASK : UT_ALPHA, lpBase))
			return(FALSE);
		bInitUndo = FALSE;
   	}	
	// erase area
	if (!AstralIsRectEmpty(&rErase))
	{
		ClearMaskRect( lpImage, &rErase, lpParms->iEraseVal, NO);
		AstralUnionRect(&rUpdate, &rUpdate, &rErase);
	}
	
	fRet = PolyMaskImage( lpImage, NULL, &PolyList, lpParms->Mode, lpParms->Antialias,
				   		lpParms->Common.idDirty, &rUpdate, bInitUndo);

 	FreePolys(&PolyList);
	
	if (fRet && !AstralIsRectEmpty(&rUpdate))
	{
		lpParms->Common.UpdateType = UT_ACTIVEAREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	ProgressEnd();
	return(fRet);
}


//************************************************************************
// 	Adds the ellipse to the mask.
//	Mirrors PointsMaskImage().
//************************************************************************
LOCAL BOOL EllipseMaskImage(LPIMAGE lpImage, LPRECT lpRect, int iModes,	ITEMID idDirty, LPRECT lpUpdateRect)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rDummy, rUpdate;
	BOOL fNewMask = NO;

	// get mask, create one if one does not exist
	lpMask = ImgGetMaskEx(lpImage, iModes==SHAPE_SUBTRACT ? ON:OFF , Control.NoUndo,
							 &fNewMask, &rUpdate);
	if (!lpMask)
		return(FALSE);
	if (fNewMask && (iModes==SHAPE_SUBTRACT))
	{
		ImgGetImageRect(lpImage, &rUpdate);
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUpdate);
	}

	// if not new mask and we are allowed to create undo, initialize undo
	lpBase = ImgGetBase(lpImage);
 	if (!ImgEditInit(lpImage, ET_OBJECT,
 					fNewMask ? UT_CREATEMASK : UT_ALPHA, lpBase))
  		{
			return(FALSE);
  		}
	
	EnableMarqueeEx( lpImage, NO );

	// add ellipse to mask
	MaskAddEllipse(lpMask, lpRect, iModes);

	// if using undo, indicate in-place editing has occurred
	ImgEditedObject(lpImage, lpBase, idDirty,
			  	fNewMask ? (LPRECT)NULL : (LPRECT)lpRect);

	// after subtractive masks we dont know area
	if (iModes!=SHAPE_ADD)
		MaskRectUpdate(lpMask, &rDummy);

	// get area that needs to be redisplayed, may be larger
	// than rUpdate because of slime
	if (ImgGetMaskUpdateRect(lpImage, fNewMask, NO, lpRect))
  	{
		AstralUnionRect(lpUpdateRect, lpUpdateRect, lpRect);
  	}

	EnableMarqueeEx(lpImage, YES);

	return(TRUE);
}

//************************************************************************
// 	Adds the poly or poly list to the image mask
//	Mirrors EllipseMaskImage().
//	lpPoly OR lpPolyList should be null!
//	NOTE: if bInitUndo it will call ImgEditInit BUT WILL call ImgEditedObject 
//************************************************************************
LOCAL BOOL PolyMaskImage(LPIMAGE lpImage, LPCPoly lpPoly, LPCPolyList lpPolyList, int iMode, BOOL bAntialias, ITEMID idDirty, LPRECT lpUpdateRect, BOOL bInitUndo)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rDummy, rUpdate;
	POSITION pos;
	BOOL fNewMask = NO;

	// get mask, create one if one does not exist
	lpMask = ImgGetMaskEx(lpImage, (iMode==SHAPE_SUBTRACT), Control.NoUndo,
							 &fNewMask, &rUpdate);
	if (!lpMask)
		return(FALSE);

	AstralSetRectEmpty(lpUpdateRect);
	if (fNewMask && (iMode==SHAPE_SUBTRACT))
	{
		ImgGetImageRect(lpImage, &rUpdate);
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUpdate);
	}

	// if not new mask and we are allowed to create undo, initialize undo
	if(bInitUndo)
	{
		lpBase = ImgGetBase(lpImage);
		if (!ImgEditInit(lpImage, ET_OBJECT,
				fNewMask ? UT_CREATEMASK : UT_ALPHA, lpBase))
  		{
			return(FALSE);
  		}
	}
	
	EnableMarqueeEx( lpImage, NO );

	if (lpPoly)
	{
		// add poly to mask
		if (!MaskAddPoly(lpMask, lpPoly, iMode, bAntialias))
  		{
			Message( IDS_EMEMALLOC );
			EnableMarqueeEx( lpImage, YES );
			return(FALSE);
  		}
		lpPoly->GetBounds(&rUpdate);
	}
	else if (lpPolyList)
	{
		AstralSetRectEmpty(&rUpdate);
		// add multiple polygons
		pos = lpPolyList->GetHeadPosition();
		// go through every polygon
		while (lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos))
		{
			// add poly to mask
			if (!MaskAddPoly(lpMask, lpPoly, iMode, bAntialias))
	  		{
				Message( IDS_EMEMALLOC );
				EnableMarqueeEx( lpImage, YES );
				return(FALSE);
	  		}
			lpPoly->GetBounds(&rDummy);
			AstralUnionRect(&rUpdate, &rUpdate, &rDummy);
		} // get each poly
			
	}

	// the shapes are larger than the mask will be 
	if (rUpdate.right > INT_MIN)
		rUpdate.right--;
	if (rUpdate.bottom > INT_MIN)
		rUpdate.bottom--;
		
	// after subtractive masks we dont know area
	if (iMode == SHAPE_SUBTRACT)
	{
		MaskRectUpdate(lpMask, &rDummy);
		AstralUnionRect(&rUpdate, &rUpdate, &rDummy);
	}
	
	// if using undo, indicate in-place editing has occurred
	lpBase = ImgGetBase(lpImage);
	ImgEditedObject(lpImage, lpBase, idDirty,
				   		fNewMask ? (LPRECT)NULL : (LPRECT)&rUpdate);

	// get area that needs to be redisplayed, may be larger
	// than rUpdate because of slime
	if (ImgGetMaskUpdateRect(lpImage, fNewMask, NO, &rUpdate))
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUpdate);
	EnableMarqueeEx( lpImage, YES );
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUpdate);
	return(TRUE);
}


/************************************************************************/
BOOL InvertMaskImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
int y, XSize, YSize;
LPPIXMAP lpPixmap;
LPTR lpDst;
LPMASK lpMask;
LPOBJECT lpBase;
RECT rUpdate, rDummy, rMarquee;

if (!lpImage)
	return(FALSE);
if (!(lpMask = ImgGetMask(lpImage)))
	return(TRUE);
ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
lpBase = ImgGetBase(lpImage);
if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpBase))
  	{
	ProgressEnd();
	return(FALSE);
  	}

GetMaskMarqueeRect(lpImage, &rMarquee);
lpPixmap = &lpMask->Pixmap;
PixmapGetInfo(lpPixmap, PMT_EDIT, &XSize, &YSize, NULL, NULL);
for (y = 0; y < YSize; ++y)
	{
	AstralClockCursor(y, YSize, NO);
	lpDst = PixmapPtr(lpPixmap, PMT_EDIT, 0, y, YES);
	if (lpDst)
		negate(lpDst, (long)XSize);
	}
SetRect(&rUpdate, 0, 0, XSize-1, YSize-1);
ImgEditedObject(lpImage, lpBase, lpParms->Common.idDirty, &rUpdate);
ProgressEnd();
MaskRectUpdate(lpMask, &rDummy);
if (ImgGetMaskUpdateRect(lpImage, NO, NO, &rUpdate))
	AstralUnionRect(&rMarquee, &rMarquee, &rUpdate);
lpMask->bInvertState = !lpMask->bInvertState;

if (!AstralIsRectEmpty(&rMarquee))
	{
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rMarquee;
	}
return(TRUE);
}

/************************************************************************/
BOOL ConfirmMaskRemove(LPIMAGE lpImage)
/************************************************************************/
{
int retc;

if (!ImgGetMask(lpImage))
	return(TRUE);
retc = AstralConfirm(IDS_OKTOREMOVEMASK);
if (retc == IDCANCEL)
	return(FALSE);
if (retc == IDYES)
	{
	CMD_PARMS parms;

	RemoveMaskImage(lpImage, &parms);
	PostCommand(lpImage->lpCmdList, IDS_CMD_REMOVEMASK, &parms);
	}
return(TRUE);
}

//************************************************************************
//	Sets the area given to the value given.
//	If bGetBackup = TRUE then it will return a copy of the 
//		data under the area cleared.
//	No Caching is done.
//************************************************************************
LPFRAME ClearMaskRect( LPIMAGE lpImage, LPRECT lpRect, BYTE value, BOOL bGetBackup)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rUpdate, rMask;
	int y, width;
	LPTR lptr;
	LPFRAME lpFrame, lpBackupFrame;
	LPPIXMAP lpPixmap;
	BOOL fNewMask = NO;
	HWND hActiveWnd;

	if (!lpImage)
	{
		if (!(hActiveWnd = GetActiveDoc()))
			return(NULL);
		lpImage = GetImagePtr(hActiveWnd);
	}
	
	EnableMarquee(NO);
	lpBase = ImgGetBase(lpImage);
	lpMask = ImgGetMaskEx(lpImage, OFF, Control.NoUndo, &fNewMask, &rUpdate);
	if (!lpMask)
		return(NULL);
	MaskRect(lpMask, &rUpdate);
	UpdateImage(lpImage, &rUpdate, YES);
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	UpdateStatusBar( NO, NO, YES, NO );

	lpPixmap = &lpMask->Pixmap;
	lpFrame = PixmapFrame(lpPixmap,PMT_EDIT);
	if (!lpFrame)
		return(NULL);
	// be sure we stay in mask
	rMask.left = rMask.top = 0;
	rMask.right = FrameXSize(lpFrame) -1;
	rMask.bottom = FrameYSize(lpFrame) -1;
	AstralIntersectRect(lpRect, lpRect, & rMask);
	width = RectWidth(lpRect);
	// create backup frame
	lpBackupFrame = NULL;
	if (bGetBackup)
		if (!(lpBackupFrame = FrameCopy( lpFrame,lpRect)))
  		{
			return(NULL);
  		}
	// clear it
	for (y=lpRect->top;y<=lpRect->bottom; y++)
	{
		lptr = PixmapPtr(lpPixmap, PMT_EDIT, lpRect->left, y, YES);
		if (lptr)
			set(lptr,width,value);
	}
	EnableMarquee(YES);
	return(lpBackupFrame);
}


//************************************************************************
//	Puts the data from lpRestoreFrame into the mask in lpRect.
//	No scaling, or depth coversion is done.
//	No Caching is done.
//************************************************************************
void RestoreMaskRect(LPRECT lpRect, LPFRAME lpRestoreFrame)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rUpdate;
	int y, width;
	LPFRAME lpFrame;
	LPTR lpSrc, lpDst;
	HWND hActiveWnd;
	LPIMAGE lpImage;
	BOOL fNewMask = NO;

	if (!(hActiveWnd = GetActiveDoc()))
		return;
	lpImage = GetImagePtr(hActiveWnd);
	
	EnableMarquee(NO);
	lpBase = ImgGetBase(lpImage);
	lpMask = ImgGetMaskEx(lpImage, OFF, Control.NoUndo, &fNewMask, &rUpdate);
	if (!lpMask)
		return;
	MaskRect(lpMask, &rUpdate);
	UpdateImage(lpImage, &rUpdate, YES);
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	UpdateStatusBar( NO, NO, YES, NO );

	lpFrame = lpMask->Pixmap.EditFrame;
	if (!lpFrame)
		return;
	width = RectWidth(lpRect);
	// set it
	for (y=lpRect->top;y<=lpRect->bottom; y++)
	{
		lpDst = FramePointer(lpFrame, lpRect->left, y, YES);
		lpSrc = FramePointer(lpRestoreFrame, 0, y-lpRect->top, NO);
		copy(lpSrc,lpDst,width);
	}
	rUpdate  =  *lpRect;
	File2DispRect(hActiveWnd, &rUpdate, &rUpdate);
	++rUpdate.right; ++rUpdate.bottom;
	InvalidateRect(hActiveWnd, &rUpdate, FALSE);
	EnableMarquee(YES);
}

/************************************************************************/
void RemoveMask()
/************************************************************************/
{
RemoveMaskEx(GetActiveImage(), IDS_UNDOMASKREMOVE);
}

/************************************************************************/
BOOL RemoveMaskImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
LPMASK lpMask;
RECT rUpdate, rMarquee;
LPOBJECT lpBase;

lpBase = ImgGetBase(lpImage);
if ( !(lpMask = ImgGetMask(lpImage)) )
	return(TRUE); // already gone

// initialize undo
if ( !ImgEditInit(lpImage, ET_OBJECT, UT_DELETEMASK, lpBase))
	return(FALSE);

/* unlink and close any mask */
GetMaskMarqueeRect(lpImage, &rMarquee);
if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rUpdate))
	AstralUnionRect(&rMarquee, &rMarquee, &rUpdate);
ImgSetMask(lpImage, NULL);

ImgEditedObject(lpImage, lpBase, lpParms->Common.idDirty, NULL);

if (!AstralIsRectEmpty(&rMarquee))
	{
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rMarquee;
	}
return(TRUE);
}

/************************************************************************/
void RemoveMaskEx(LPIMAGE lpImage, int dirty)
/************************************************************************/
{
ASSERT(lpImage);

LPMASK lpMask;
RECT rUpdate;
BOOL fUpdate;
LPOBJECT lpBase;
HWND hActiveWnd;

if (!(hActiveWnd = GetActiveDoc()))
	return;

lpBase = ImgGetBase(lpImage);
if ( !(lpMask = ImgGetMask(lpImage)) )
	return;
if (dirty > 0)
	{
	if ( !ImgEditInit(lpImage, ET_OBJECT, UT_DELETEMASK, lpBase))
		return;
	}

/* unlink and close any mask */
EnableMarquee( NO );
RemoveMaskMarquee(lpImage);
fUpdate = ImgGetMaskUpdateRect(lpImage, YES, NO, &rUpdate);
ImgSetMask(lpImage, NULL);
if (dirty > 0)
	ImgEditedObject(lpImage, lpBase, IDS_UNDOMASKREMOVE, NULL);
//MaskClose( lpMask );

if (fUpdate)
	{
	File2DispRect(hActiveWnd, &rUpdate, &rUpdate);
	++rUpdate.right; ++rUpdate.bottom;
	InvalidateRect(hActiveWnd, &rUpdate, FALSE);
	}
if ( Tool.hRibbon )
	SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
EnableMarquee( YES );
}

/************************************************************************/
LOCAL LPMASK ReadOldMask(LPSTR szClipName, int iMaxWidth, int iMaxHeight, FRMDATATYPE FrameType)
/************************************************************************/
{
	FILE_HANDLE fp;
	short order;
	RECT rMask;
	LPMASK lpMask = NULL;
	char type[4];
	TFORM tform;
	CPolyList PolyList;
	
	if ( (fp = FileOpen(szClipName, FO_READ)) == FILE_HANDLE_INVALID )
		goto erropen;
	
	AstralCursor(IDC_WAIT);
	if (!FileRead(fp, type, 3))
		goto errread;
	if (!StringsEqualN(type, "MSK", 3))
		goto errtype;
	
	if (!FileRead(fp,&order, 2))
		goto errread;
	if (!ReadOldPolyList(fp, order != MYORDER, &PolyList))
		goto errmem;
	if (!iMaxWidth || !iMaxHeight)
	{
		PolyListGetBounds(&PolyList, &rMask);
		// move the shapes
		TInit(&tform );
		TMove(&tform, -rMask.left, -rMask.top);
		PolyList.ForEach1((LPLISTPROC1)PolyTForm, (LONG)&tform);
		iMaxWidth = RectWidth(&rMask);
		iMaxHeight = RectHeight(&rMask);
	}
	if ( !( lpMask = (LPMASK)MaskCreate(NULL, 
							iMaxWidth, iMaxHeight,
							NO, Control.NoUndo, FrameType)))
		goto errmem;
	if (!MaskAddPoly(lpMask, &PolyList, SHAPE_REVERSE, NO /*bAntialias*/))
	{
		MaskClose(lpMask);
		lpMask = NULL;
		goto errmem;
	}
	goto okp;
	
errtype:
	Message( IDS_EBADMASK, (LPTR)szClipName );
	goto errc;
errmem:
	Message( IDS_EMEMALLOC );
	goto errc;
erropen:
	Message ( IDS_EOPEN, (LPTR)szClipName );
	goto errc;
errread:
	Message ( IDS_EREAD, (LPTR)szClipName );
	goto errc;
		
errc:	
okp:	
	if (!PolyList.IsEmpty())
		FreePolys(&PolyList);
	FileClose(fp);
	AstralCursor(NULL);
	return(lpMask);
}

/************************************************************************/
//	Dont forget to free up the polygons allocated here.
/************************************************************************/
LOCAL LPCPolyList ReadOldPolyList( FILE_HANDLE fp, BOOL fSwap, LPCPolyList lpPolyList )
/************************************************************************/
{
	short count, nPoints, i;
	LPWORD lpWord;
	LPPOINT lpPoint, lpFirstPoint = NULL;
	short nShapes;
	TFORM ShapeTForm;
	BOOL fCircle;
	LPCPoly lpPoly = NULL;
	
	if (!lpPolyList)
		return(NULL);
		
	if (!FileRead(fp,(LPSTR)&nShapes, 2))
		return(NULL);
	if (fSwap) swapw((LPWORD)&nShapes);
	if (nShapes <= 0)
		return(NULL);
	// go through shapes
	for (i = 0; i < nShapes; ++i)
	{
		// read header info
		if (!FileRead(fp,(LPSTR)&fCircle, 2))
			return(NULL);
		if (fSwap) swapw((LPWORD)&fCircle);
		if (!FileRead(fp,(LPSTR)&ShapeTForm.ax, 4))
			return(NULL);
		if (fSwap) swapl((LPDWORD)&ShapeTForm.ax);
		if (!FileRead(fp,(LPSTR)&ShapeTForm.ay, 4))
			return(NULL);
		if (fSwap) swapl((LPDWORD)&ShapeTForm.ay);
		if (!FileRead(fp,(LPSTR)&ShapeTForm.bx, 4))
			return(NULL);
		if (fSwap) swapl((LPDWORD)&ShapeTForm.bx);
		if (!FileRead(fp,(LPSTR)&ShapeTForm.by, 4))
			return(NULL);
		if (fSwap) swapl((LPDWORD)&ShapeTForm.by);
		if (!FileRead(fp,(LPSTR)&ShapeTForm.cx, 4))
			return(NULL);
		if (fSwap) swapl((LPDWORD)&ShapeTForm.cx);
		if (!FileRead(fp,(LPSTR)&ShapeTForm.cy, 4))
			return(NULL);
		if (fSwap) swapl((LPDWORD)&ShapeTForm.cy);
		if (!FileRead(fp,(LPSTR)&nPoints, 2))
	 		return(NULL);
		lpWord = (LPWORD)&nPoints;
		if (fSwap) 
			swapw(lpWord);
		// get temp storage (not needed)
		lpFirstPoint = (LPPOINT)Alloc((long)(nPoints+1) * (long)sizeof(POINT));
		if (!lpFirstPoint)
			goto MemErr;
		lpPoly = new CPoly;
		if (!lpPoly)
			goto MemErr;
		count = nPoints;
		lpPoint = lpFirstPoint;
		// read data to temp storage
		while (--count >= 0)
		{
			if (!FileRead(fp,(LPSTR)&lpPoint->x, 2))
				return(NULL);
		   if (fSwap) swapw((LPWORD)&lpPoint->x);
			if (!FileRead(fp,(LPSTR)&lpPoint->y, 2))
				return(NULL);
		   if (fSwap) swapw((LPWORD)&lpPoint->y);
			++lpPoint;
		}
		// Copy to polygon
		count = nPoints;
		lpPoint = lpFirstPoint;
		while (--count >= 0)
		{
			if (lpPoint->x == BEZIER_MARKER)
			{
				if (!lpPoly->AddBezierLine2(&lpPoint[1], &lpPoint[2], &lpPoint[3]))
					goto MemErr;
				lpPoint+=3;
				count-=3;
			}
			else 
			{
				if (!lpPoly->AddPoint(lpPoint))
					goto MemErr;
			}
			++lpPoint;
		}
		FreeUp((LPTR)lpFirstPoint);
		lpFirstPoint = NULL;
		// apply the transform
		PolyTForm(lpPoly, &ShapeTForm);
		// add to poly list
		if (!lpPolyList->AddTail(lpPoly))
			goto MemErr;
		lpPoly = NULL;
	}
	return(lpPolyList);
MemErr:
	if (lpFirstPoint)
		FreeUp((LPTR)lpFirstPoint);
	if (lpPoly)
		delete(lpPoly);
	if (lpPolyList)
		FreePolys(lpPolyList);
	return(NULL);
}



#ifdef UNUSED
/************************************************************************/
BOOL WriteMask( LPSTR szClipName, LPMASK lpMask, BOOL fRelative )
/************************************************************************/
{
int fp;
BOOL status = TRUE;
short order;

if (!lpMask)
	return( FALSE );
if ( (fp = _lcreat(szClipName, 0)) < 0 )
	goto errcreate;
AstralCursor(IDC_WAIT);
if (_lwrite(fp, "MSK", 3) != 3)
	goto errwrite;
order = MYORDER;
if (_lwrite(fp,(LPSTR)&order, 2) != 2)
	goto errwrite;
if (!WriteShapes(fp, lpMask, fRelative))
	goto errwrite;
goto okc;

errcreate:
Message ( IDS_ECREATE, (LPTR)szClipName );
status = FALSE;
goto okc;
errwrite:
Message( IDS_EWRITE, (LPTR)szClipName );
status = FALSE;
okc:
_lclose( fp );
AstralCursor(NULL);
return(status);
}
#endif

/************************************************************************/
BOOL WriteMask(LPSTR lpClipName, LPMASK lpMask, BOOL fMaskedArea)
/************************************************************************/
{
RECT rMask;
FRMTYPEINFO TypeInfo;
ITEMID      DataType = IDC_SAVECT;

if (!lpMask)
	return( FALSE );

if (fMaskedArea)
	rMask = lpMask->rMask;
else
	{
	rMask.left = rMask.top = 0;
	rMask.right = FrameXSize(lpMask->Pixmap.EditFrame);
	rMask.bottom = FrameYSize(lpMask->Pixmap.EditFrame);
	}
FrameGetTypeInfo(lpMask->Pixmap.EditFrame, &TypeInfo);
if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
    DataType = IDC_SAVELA;
CWriteImage write(lpClipName, IDN_TIFF);
write.SetSourceArea(&rMask);
if (!write.WriteFrame(lpMask->Pixmap.EditFrame, DataType))
	{
	Message( IDS_EWRITE, lpClipName );
	return(FALSE);
	}
else
	return(TRUE);
}

/************************************************************************/
LPMASK ReadMask(LPIMAGE lpImage, LPSTR lpClipName, int iMaxWidth, int iMaxHeight)
/************************************************************************/
{
	LPMASK lpMask;
	LPFRAME lpFrame;
	LPSTR lpExt;
	FRMTYPEINFO TypeInfo;
	FRMDATATYPE FrameType = FDT_GRAYSCALE;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
   	FrameType = FDT_LINEART;

	lpExt = extension(lpClipName);
	if (StringsEqual(lpExt, ".MSK"))
		return(ReadOldMask(lpClipName, iMaxWidth, iMaxHeight, FrameType));

	FrameSetTypeInfo(&TypeInfo, FrameType, NULL);

	if (lpImage &&
		lpImage->GetRootCompoundStorage() &&
		IsStorageMaskbit(lpClipName) )
	{
		LPSTORAGE lpStorage;
		COleStreamFile *pFile = OpenClipStream(lpImage->GetRootCompoundStorage(), lpClipName, &lpStorage);
		if (!pFile)
		{
			Message ( IDS_EOPEN, lpClipName );
			return(NULL);
		}
		CReadImage read(pFile, GetPPFileType(lpClipName));
		lpFrame = read.ReadFrame(TypeInfo);
		CloseClipStream(lpStorage, pFile);
	}
	else
	{
		CReadImage read(lpClipName);
		lpFrame = read.ReadFrame(TypeInfo);
	}
	if (!lpFrame)
	{
		if (FileExists(lpClipName))
			Message ( IDS_EMASKREAD, lpClipName );
		else
			Message ( IDS_EOPEN, lpClipName );
		return(NULL);
	}

	if ( !( lpMask = (LPMASK)MaskCreate(lpFrame, 
							FrameXSize(lpFrame), FrameYSize(lpFrame),
							NO, Control.NoUndo, FrameType)))
	{
		Message( IDS_EMEMALLOC );
		FrameClose(lpFrame);
		return( NULL );
	}
	return(lpMask);
}


/************************************************************************/
BOOL ApplyMaskSelObj(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
	ENGINE Engine;
	LPMASK lpMask;
	LPFRAME lpMaskFrame;

	if (!(lpMask = ImgGetMask(lpImage)))
		return(TRUE); // not really an error, just can't proceed
	lpMaskFrame = AlphaGetEditFrame(lpMask);

	ASSERT(lpMaskFrame);

	SetEngineDef(&Engine);
	Engine.fEditAlpha = YES;
	Engine.MergeMode = MM_MULTIPLY;
	Engine.lpSrcFrame = lpMaskFrame;
	Engine.MaskType = MT_NONE;
	lpParms->Common.StatusCode = LineEngineSelObj(lpImage, &Engine, lpParms->Common.idDirty);
	if (!AstralIsRectEmpty(&Engine.rUpdate))
	{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = Engine.rUpdate;
	}
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

static LPFRAME lpMskFrame;
static RECT rMsk;
LOCAL void FeatherAlphaProcIn(int y, int left, int right, LPTR lpMsk, LPVOID lpParms );
LOCAL void FeatherAlphaProcOut(int y, int left, int right, LPTR lpMsk, LPVOID lpParms );

/************************************************************************/
BOOL FeatherAlphaSelObj(LPIMAGE lpImage, LPFEATHERALPHA_PARMS lpParms)
/************************************************************************/
{
	LPOBJECT lpObject = NULL;
	int num = 0;
	RECT rUpdate;
	int res, size;
	BOOL fNeedRecover;

	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	size = lpParms->size;
	ResConvertDist(lpParms->iBaseRes, res, &size);
	
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		num++;
	if (!num)
		return(FALSE);
	ProgressBegin(num, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	if (!ImgEditInit(lpImage, ET_SELOBJECTS, UT_ALPHA, NULL))
	{
		ProgressEnd();
		return(FALSE);
	}
	lpObject = NULL	;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
		lpParms->Common.StatusCode = FeatherAlpha(lpImage, lpObject, lpParms->inside, size,
							lpParms->edge, YES, &rUpdate, &fNeedRecover);

		if (lpParms->Common.StatusCode == SC_CANCELLED && fNeedRecover)
		{
			ImgEditRecoverSelObj(lpImage, FALSE, TRUE);
			rUpdate = lpObject->rObject;
		}
		if (!AstralIsRectEmpty(&rUpdate))
		{
			lpParms->Common.UpdateType = UT_AREA;
			AstralUnionRect(&lpParms->Common.rUpdateArea,
							&lpParms->Common.rUpdateArea,
							&rUpdate);
		}
		if (lpParms->Common.StatusCode == SC_SUCCESS)
			ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, &rUpdate);
		else
			break;
	}
	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//************************************************************************
//	bInside == 1 gives inside.
//	bInside == 0 gives all (gaussian blur).
//	bInside == -1 gives outside.
//************************************************************************
BOOL FeatherMaskImage(LPIMAGE lpImage, LPFEATHERALPHA_PARMS lpParms)
/************************************************************************/
{
	RECT rUpdate, rDummy;
	LPOBJECT lpObject;
	int res, size;
	BOOL fNeedRecover;

	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	size = lpParms->size;
	ResConvertDist(lpParms->iBaseRes, res, &size);

	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	lpObject = ImgGetBase(lpImage);
	lpParms->Common.StatusCode = FeatherAlpha(lpImage, lpObject, lpParms->inside, size,
					lpParms->edge, NO, &rUpdate, &fNeedRecover);
	if (lpParms->Common.StatusCode == SC_CANCELLED && fNeedRecover)
	{
		ImgEditRecoverObj(lpImage, lpObject, FALSE, TRUE);
	}
	if (!AstralIsRectEmpty(&rUpdate))
	{
		lpParms->Common.UpdateType = UT_ACTIVEAREA;
		lpParms->Common.rUpdateArea = rUpdate;
		MaskRectUpdate(ObjGetAlpha(lpObject), &rDummy);
	}
	if (lpParms->Common.StatusCode == SC_SUCCESS)
		ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, &rUpdate);

  	ProgressEnd();
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//************************************************************************
//	bInside == 1 gives inside.
//	bInside == 0 gives all (gaussian blur).
//	bInside == -1 gives outside.
//************************************************************************
LOCAL STATUS_CODE FeatherAlpha(LPIMAGE lpImage, LPOBJECT lpObject, int Inside, int size, int edge, BOOL fNoUndo,
							LPRECT lpUpdateRect, LPBOOL lpNeedRecover)
/************************************************************************/
{
	HMGXBRUSH hBrush;
	LPALPHA lpAlpha;
	COLORINFO Color;
	LPPIXMAP lpPixmap;
	PIXMAP pixmap;
	int radius;
	int dx, dy;
	RECT rArea;
	ENGINE Engine;
	BOOL bInside;
	STATUS_CODE StatusCode = SC_MEMERROR;

	*lpNeedRecover = FALSE;

	AstralSetRectEmpty(lpUpdateRect);
	if (!lpImage || !lpObject)
		return(SC_BADPARMS);
	
	lpAlpha = ObjGetAlpha(lpObject);
	if (!lpAlpha)
		return(SC_BADPARMS);
	if (!MaskRect(lpAlpha, &rArea))
		return(SC_BADPARMS);
	
	
	radius = (size+1)/2;
	radius = bound(radius, 0, 24);
	
	lpMskFrame = NULL;
	if (!Inside)
	{
		// just do a gaussian
		lpPixmap = &lpAlpha->Pixmap;
		ASSERT(lpPixmap);
		if (!fNoUndo)
			if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpObject))
				return(SC_UNDOERROR);
		StatusCode = GaussPixmap(lpPixmap, NULL, edge, radius, 0,0, TRUE, &rArea);
		ImgGetInfo(lpImage, &dx, &dy, NULL, NULL);
		if (StatusCode == SC_SUCCESS)
		{
			if (!fNoUndo)
				ImgEditedObject(lpImage, lpObject, IDS_UNDOFEATHER, &lpObject->rObject);
			SetRect(lpUpdateRect, 0, 0, dx-1, dy-1);
		}
		else
		if (StatusCode == SC_CANCELLED)
		{
			*lpNeedRecover = TRUE;
			SetRect(lpUpdateRect, 0, 0, dx-1, dy-1);
		}
		return(StatusCode);
	}
	
	bInside = (Inside != -1);
	ProgressBegin(2, 0);
	if (!(hBrush = CreateFeatherMaskBrush(radius*2+1, 255)))
		goto Exit;
	lpMskFrame = StrokeAlpha(lpImage, lpAlpha, hBrush, bInside, &rMsk);
	DestroyMgxBrush(hBrush);
	if (!lpMskFrame)
		goto Exit;
	
	PixmapSetup(&pixmap,lpMskFrame,YES);                                   
	// gauss whole lpMskFrame frame
	StatusCode = GaussPixmap(&pixmap, NULL, edge, radius, 0, 0, FALSE, NULL);
	
	if (StatusCode == SC_SUCCESS)
	{
		SetEngineDef(&Engine);
		Engine.lpColor = &Color;
		Engine.fEditAlpha = YES;
		OffsetRect(&rMsk, lpObject->rObject.left, lpObject->rObject.top);
		Engine.lpEditRect = &rMsk;
		Engine.fNoUndo = fNoUndo;
		Engine.MaskType = MT_OBJECT;
		if (bInside)
		{
			Color.gray = 0;
			Engine.lpMaskProc = FeatherAlphaProcIn;
		}
		else
		{
			Color.gray = 255;
			Engine.lpMaskProc = FeatherAlphaProcOut;
		}
		SetColorInfo(&Color, &Color, CS_GRAY);
		StatusCode = LineEngineObj(lpImage, lpObject, &Engine, IDS_UNDOFEATHER);
		if (!AstralIsRectEmpty(&Engine.rUpdate))
			*lpUpdateRect = Engine.rUpdate;
	}
Exit:
	if (lpMskFrame)
		FrameClose(lpMskFrame);
	ProgressEnd();
	return(StatusCode);
}

/************************************************************************/
LOCAL void FeatherAlphaProcIn(int y, int left, int right, LPTR lpMsk, LPVOID lpParms )
/************************************************************************/
{
int dx;
LPTR lpBrushMask;

dx = right-left+1;
setifset(lpMsk, dx, 255);
lpBrushMask = FramePointer(lpMskFrame, left-rMsk.left, y-rMsk.top, NO);
if (lpBrushMask)
	ScaleDataBuf8(lpMsk, dx, lpBrushMask);
}

/************************************************************************/
LOCAL void FeatherAlphaProcOut(int y, int left, int right, LPTR lpMsk, LPVOID lpParms )
/************************************************************************/
{
int dx;
LPTR lpBrushMask, lptr;

// 255->0 all else goes to 255
dx = right-left+1;
lptr = lpMsk;
while(dx--)
{
	*lptr = (*lptr<255) ? 255:0;
	lptr++;
}
dx = right-left+1;
lpBrushMask = FramePointer(lpMskFrame, left-rMsk.left, y-rMsk.top, NO);
if (lpBrushMask)
	ScaleDataBuf8(lpMsk, dx, lpBrushMask);
}
							   

/************************************************************************/
//	if !bInside then the resulting frame is bigger by 1/2 brushsize, but
//		bits are not bigger.  The strokes also take place on the oppsite 
//		transition.
/************************************************************************/
LOCAL LPFRAME StrokeAlpha(LPIMAGE lpImage, LPALPHA lpAlpha, HMGXBRUSH hBrush, BOOL bInside, LPRECT lpRect)
/************************************************************************/
{
int x, y, i, iWidth, brushSize;
LPTR lpMaskBuf[3];
BYTE p, n, c;
LPTR lpP, lpC, lpN;
int prev, curr, next, temp;
RECT rAlpha, rStroke, rMax;
LPFRAME lpMskFrame;
LPPIXMAP lpPixmap;

if (!lpAlpha)
	return(NULL);
if (lpAlpha != ImgGetMask(lpImage))
	if (!MaskRectUpdate(lpAlpha, &rAlpha))
		return(NULL);

rAlpha = lpAlpha->rMask;
InflateRect(&rAlpha, 1, 1);
rStroke = rAlpha;
InflateRect(&rAlpha, 1, 1);

iWidth = RectWidth(&rAlpha);
for (i = 0; i < 3; ++i)
	{
	if (!(lpMaskBuf[i] = Alloc((long)iWidth)))
		{
		 if (i>=2)
		 	FreeUp(lpMaskBuf[1]);
		 if (i>=1)
		 	FreeUp(lpMaskBuf[0]);
		return(NULL);
		}
	}

lpPixmap = &lpAlpha->Pixmap;
SetRect(&rMax, 0, 0, FrameXSize(lpPixmap->EditFrame)-1,
		FrameYSize(lpPixmap->EditFrame)-1);
brushSize = GetMgxBrushSize(hBrush);
if (!bInside)
	InflateRect(&rStroke,(brushSize+1)/2, (brushSize+1)/2);
if (!MgxOSBrushBegin(hBrush, &rStroke, &rMax))
	{
	for (i = 0; i < 3; ++i)
		FreeUp(lpMaskBuf[i]);
	return(NULL);
	}

prev = 0;
curr = 1;
next = 2;

// get mask for previous line
y = rAlpha.top;
if ( !(MaskLoader(lpAlpha, rAlpha.left, y, iWidth, lpMaskBuf[prev], !bInside, CR_COPY )) )
	clr( lpMaskBuf[prev], iWidth );

// get mask for first line
++y;
if ( !(MaskLoader(lpAlpha, rAlpha.left, y, iWidth, lpMaskBuf[curr], !bInside, CR_COPY )) )
	clr( lpMaskBuf[curr], iWidth );

while ( y < rAlpha.bottom )
	{
	if ( !(MaskLoader(lpAlpha, rAlpha.left, y+1, iWidth, lpMaskBuf[next], !bInside, CR_COPY )) )
		clr( lpMaskBuf[next], iWidth );

	lpP = lpMaskBuf[prev];
	lpC = lpMaskBuf[curr];
	lpN = lpMaskBuf[next];

	p = *lpC++;
	++lpP;
	++lpN;
	c = *lpC++;
	x = rAlpha.left + 1;
	while(x < rAlpha.right)
		{
		n = *lpC++;
		if (!c)  // a pixel is on
			{
			if (p) // pixel to left off?
				{
				MgxOSBrushStroke(x, y);
				}
			else
			if (n) // pixel to right off
				{
				MgxOSBrushStroke(x, y);
				}
			else
			if ((*lpP))  // pixel above off?
				{
				MgxOSBrushStroke(x, y);
				}
			else
			if ((*lpN)) // pixel below off?
				{
				MgxOSBrushStroke(x, y);
				}
			}
		p = c;
		c = n;
		++lpP; ++lpN;
		++x;
		}
	temp = prev;
	prev = curr;
	curr = next;
	next = temp;
	++y;
	}
lpMskFrame = MgxOSBrushEnd(lpRect);
for (i = 0; i < 3; ++i)
	FreeUp(lpMaskBuf[i]);
return(lpMskFrame);
}

// LOCAL prototypes
LOCAL BOOL FeatherObj_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void FeatherObj_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFeatherObjProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, FeatherObj_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, FeatherObj_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FEATHEROBJECT);
	}
}

/************************************************************************/
LOCAL BOOL FeatherObj_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CheckComboItem( hDlg,  IDC_FEATHEROBJ_EDGE, IDC_FEATHEROBJ_HARD, IDC_FEATHEROBJ_SOFT,
	Mask.wFeatherEdge + IDC_FEATHEROBJ_HARD);
CheckComboItem( hDlg,  IDC_FEATHEROBJ_INOUT, IDC_FEATHEROBJ_OUT, IDC_FEATHEROBJ_IN,
	Mask.FeatherInside +IDC_FEATHEROBJ_ALL);
InitDlgItemSpin( hDlg, IDC_FEATHEROBJ_AMOUNT, Mask.FeatherAmount,
					NO, 1, 50 );
CenterPopup( hDlg );
return(TRUE);
}

/************************************************************************/
LOCAL void FeatherObj_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;

switch (id)
	{
	case IDC_FEATHEROBJ_INOUT:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Mask.FeatherInside = id - IDC_FEATHEROBJ_ALL;
	break;
	case IDC_FEATHEROBJ_AMOUNT:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		GetDlgItemSpin( hDlg, IDC_FEATHEROBJ_AMOUNT, &Bool, NO );
	break;
	case IDC_FEATHEROBJ_EDGE:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Mask.wFeatherEdge = id - IDC_FEATHEROBJ_HARD;
	break;
	
	case IDOK:
		Mask.FeatherAmount = GetDlgItemSpin( hDlg, IDC_FEATHEROBJ_AMOUNT,
								 &Bool, NO );
		AstralDlgEnd( hDlg, TRUE );
	break;
	
	case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
	break;
	
   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Feather_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Feather_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFeatherProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Feather_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Feather_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FEATHER);
	}
}

/************************************************************************/
LOCAL BOOL Feather_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CheckComboItem( hDlg, IDC_FEATHER_EDGE, IDC_FEATHER_HARD, IDC_FEATHER_SOFT,
	Mask.wFeatherEdge + IDC_FEATHER_HARD);
CheckComboItem( hDlg, IDC_FEATHER_INOUT, IDC_FEATHER_OUT, IDC_FEATHER_IN,
	Mask.FeatherInside + IDC_FEATHER_ALL);
InitDlgItemSpin( hDlg, IDC_FEATHER_AMOUNT, Mask.FeatherAmount,
					NO, 1, 50 );
CenterPopup( hDlg );
return(TRUE);
}

/************************************************************************/
LOCAL void Feather_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;

switch (id)
	{
	case IDC_FEATHER_INOUT:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Mask.FeatherInside = id - IDC_FEATHER_ALL;
	break;
	case IDC_FEATHER_AMOUNT:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		GetDlgItemSpin( hDlg, IDC_FEATHER_AMOUNT, &Bool, NO );
	break;
	case IDC_FEATHER_EDGE:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Mask.wFeatherEdge = id - IDC_FEATHER_HARD;
	break;
	
	case IDOK:
		Mask.FeatherAmount = GetDlgItemSpin( hDlg, IDC_FEATHER_AMOUNT,
								 &Bool, NO );
		AstralDlgEnd( hDlg, TRUE );
	break;
	
	case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
	break;
	
   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL MaskSmooth_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MaskSmooth_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void MaskSmooth_OnDestroy(HWND hDlg);

//***FIX move to Data.h
static int gauss = 3;

/***********************************************************************/
BOOL WINPROC EXPORT DlgMaskSmoothProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MaskSmooth_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MaskSmooth_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DESTROY, MaskSmooth_OnDestroy);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MASK_SMOOTH);
	}
}

/************************************************************************/
LOCAL BOOL MaskSmooth_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	LPMASK_SMOOTH_PARMS lpParm;
	
	// setup LPMASK_SMOOTH_PARMS
	if (!(lpParm = (LPMASK_SMOOTH_PARMS)lParam))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpParm);
	// set to defaults (pixel units)
	lpParm->gauss = gauss;
	lpParm->thresh = 128;
	
	InitDlgItemSpin( hDlg, IDC_RH_AMOUNT, lpParm->gauss,
					NO, 1, 50 );
	return(TRUE);
}

/************************************************************************/
LOCAL void MaskSmooth_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	LPMASK_SMOOTH_PARMS lpParm;
	int temp;
	BOOL Bool;
	
	switch (id)
	{
		case IDC_RH_DOTS:
			if (!(lpParm = (LPMASK_SMOOTH_PARMS)GetWindowLong(hDlg, DWL_USER)))
				break;
		break;
		
		case IDC_RH_AMOUNT:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			if (!(lpParm = (LPMASK_SMOOTH_PARMS)GetWindowLong(hDlg, DWL_USER)))
				break;
			temp = GetDlgItemSpin( hDlg, id, &Bool, NO );
			if (Bool)
				lpParm->gauss = temp;
		break;
		
		case IDOK:
			if (!(lpParm = (LPMASK_SMOOTH_PARMS)GetWindowLong(hDlg, DWL_USER)))
				break;
			gauss = lpParm->gauss;
			AstralDlgEnd( hDlg, TRUE );
		break;
		
		case IDCANCEL:
			AstralDlgEnd( hDlg, FALSE );
		break;
		
		default:
		break;
	}
}


/***************************************************************************/
LOCAL void MaskSmooth_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	// to prevent Dialog_OnDestroy from trying to free DWL_USER
	// which was passed in to dialog.
	SetWindowLong(hDlg, DWL_USER, 0L);
	Dialog_OnDestroy(hDlg);
}


//************************************************************************
BOOL MaskSmoothImage(LPIMAGE lpImage, LPMASK_SMOOTH_PARMS lpParms)
//************************************************************************
{
	RECT rUpdate, rDummy, rMask;
	LPOBJECT lpObject;
	LPALPHA lpAlpha;
	int gauss, res;

	lpObject = ImgGetBase(lpImage);
	lpAlpha = ObjGetAlpha(lpObject);
	if (!lpAlpha)
		return(FALSE);
		
	MaskRect(lpAlpha, &rMask);
	
	if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpObject))
		return(FALSE);
	ProgressBegin(1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	EnableMarquee(NO);
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	gauss = lpParms->gauss;
	ResConvertDist(lpParms->iBaseRes, res, &gauss);
	
	lpParms->Common.StatusCode = MaskSmooth_Dots(&lpAlpha->Pixmap, gauss, lpParms->thresh, &rMask, &rUpdate);
	if (lpParms->Common.StatusCode == SC_SUCCESS)
	{
		ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, &rUpdate);
	}
	else
	if (lpParms->Common.StatusCode == SC_CANCELLED)
	{
		ImgEditRecoverObj(lpImage, lpObject, FALSE, TRUE);
		rUpdate = lpObject->rObject;
	}


	if (!AstralIsRectEmpty(&rUpdate))
	{
	 	MaskRectUpdate(lpAlpha, &rDummy);
		lpParms->Common.UpdateType = UT_ACTIVEAREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	
	EnableMarquee(YES);
	ProgressEnd();
	return( lpParms->Common.StatusCode == SC_SUCCESS );
}

//************************************************************************
BOOL RemoveHolesImage(LPIMAGE lpImage, LPREMOVE_HOLES_PARMS lpParms)
//************************************************************************
{
	RECT rUpdate, rDummy;
	LPOBJECT lpObject;
	LPALPHA lpAlpha;
	STATUS_CODE StatusCode;

	lpObject = ImgGetBase(lpImage);
	lpAlpha = ObjGetAlpha(lpObject);
	if (!lpAlpha)
		return(FALSE);

	if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpObject))
		return(FALSE);
	EnableMarquee(NO);
	ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty), NULL, NULL, FALSE /*no clock*/);
	StatusCode = MgxFloodRemoveHoles(&lpAlpha->Pixmap, &rUpdate);
	ProgressEnd(TRUE);

	if (StatusCode == SC_SUCCESS)
		ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, &rUpdate);
	else
	if (StatusCode == SC_CANCELLED)
	{
		ImgEditRecoverObj(lpImage, lpObject, FALSE, TRUE);
		rUpdate = lpObject->rObject;
	}
	MaskRectUpdate(lpAlpha, &rDummy);
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rUpdate;
	EnableMarquee(YES);
	return(StatusCode == SC_SUCCESS);
}

LOCAL STATUS_CODE MaskSmooth_Dots(LPPIXMAP lpPixmap, int gauss, int thresh, LPRECT lprWork,  LPRECT lprUpdate)
{
	int x, y;
	int width, height, Wpixmap, Hpixmap;
	LPTR lpLine;
	int edge;
	RECT rWork;
	STATUS_CODE StatusCode;
	
	ProgressBegin(2, 0);

	AstralSetRectEmpty(lprUpdate);
	// Apply gaussian 
    edge = (Mask.FeatherEdgeSoft*255L)/(gauss*100L);
	
	PixmapGetInfo(lpPixmap, PMT_EDIT, &Wpixmap, &Hpixmap, NULL, NULL);
	if (lprWork && !AstralIsRectEmpty(lprWork))
		rWork = *lprWork;
	else
		SetRect(&rWork,0,0,Wpixmap-1, Wpixmap-1);
		
	StatusCode = GaussPixmap(lpPixmap, NULL, edge, gauss, 0, 0, TRUE, &rWork);
	if (StatusCode)
	{
		ProgressEnd();
		return(StatusCode);
	}
	// expand rWork for gaussian 
	rWork.left = max(0, rWork.left-(gauss*2+1));
	rWork.top = max(0, rWork.top-(gauss*2+1));
	rWork.right = min(Wpixmap-1, rWork.right+(gauss*2+1));
	rWork.bottom = min(Hpixmap-1, rWork.bottom+(gauss*2+1));
	width = RectWidth(&rWork);
	height = RectWidth(&rWork);
	
	// Threshold
	for (y = rWork.top; y<=rWork.bottom; y++)
	{
		if (AstralClockCursor(y-rWork.top, height, YES))
		{
			if (CheckAbortInPlaceEdit(lpPixmap))
			{
				ProgressEnd();
				return(SC_CANCELLED);
			}
		}
		lpLine = PixmapPtr(lpPixmap,PMT_EDIT,rWork.left, y, YES);
		for (x=0;x<width;x++)
		{
			if (*lpLine > thresh)
				*lpLine++ = 255;
			else
				*lpLine++ = 0;
		}
	}
	*lprUpdate = rWork;
	ProgressEnd();
	return(SC_SUCCESS);
}
