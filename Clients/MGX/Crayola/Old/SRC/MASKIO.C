// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes		    
static BOOL GetNewMaskState(LPSHAPE lpShape);

#define TIF_II (0x4949)
#define MYORDER (TIF_II)

/************************************************************************/
void InvertMask( LPIMAGE lpImage )
/************************************************************************/
{
int y, XSize, YSize;
LPPIXMAP lpPixmap;
LPTR lpDst;
LPMASK lpMask;
LPOBJECT lpBase;
RECT rUpdate, rDummy;

if (!lpImage)
	return;
if (!(lpMask = ImgGetMask(lpImage)))
	return;
ProgressBegin(1, IDS_PROGMASKINVERT);
lpBase = ImgGetBase(lpImage);
if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpBase))
  	{
	ProgressEnd();
	return;
  	}
EnableMarquee( NO );
RemoveMaskMarquee(lpImage);
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
ImgEditedObject(lpImage, lpBase, IDS_UNDOMASKINVERT, &rUpdate);
MaskRectUpdate(lpMask, &rDummy);
if (ImgGetMaskUpdateRect(lpImage, NO, NO, &rUpdate))
	{
	File2DispRect(&rUpdate, &rUpdate);
	++rUpdate.right; ++rUpdate.bottom;
	InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
	}
lpMask->bInvertState = !lpMask->bInvertState;
EnableMarquee( YES );
UpdateStatusBar( NO, NO, YES, NO );
ProgressEnd();
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
	RemoveMask();
return(TRUE);
}

//************************************************************************
//	Sets the area given to the value given.
//	If bGetBackup = TRUE then it will return a copy of the 
//		data under the area cleared.
//	No Caching is done.
//************************************************************************
LPFRAME ClearMaskRect(LPRECT lpRect, BYTE value, BOOL bGetBackup)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rUpdate;
	int y, width;
	LPTR lptr;
	LPFRAME lpFrame, lpBackupFrame;
	LPPIXMAP lpPixmap;
	BOOL fNewMask = NO;
	
	if ( !lpImage )
		return(NULL);
	
	EnableMarquee(NO);
	lpBase = ImgGetBase(lpImage);
	lpMask = ImgGetMaskEx(lpImage, OFF, Control.NoUndo, &fNewMask);
	if (!lpMask)
		return(NULL);

	lpPixmap = &lpMask->Pixmap;
	lpFrame = PixmapFrame(lpPixmap,PMT_EDIT);
	if (!lpFrame)
		return(NULL);
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
		set(lptr,width,value);
	}
	rUpdate  =  *lpRect;
	File2DispRect(&rUpdate, &rUpdate);
	++rUpdate.right; ++rUpdate.bottom;
	InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
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
	BOOL fNewMask = NO;
	
	if ( !lpImage )
		return;
	
	EnableMarquee(NO);
	lpBase = ImgGetBase(lpImage);
	lpMask = ImgGetMaskEx(lpImage, OFF, Control.NoUndo, &fNewMask);
	if (!lpMask)
		return;

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
	File2DispRect(&rUpdate, &rUpdate);
	++rUpdate.right; ++rUpdate.bottom;
	InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
	EnableMarquee(YES);
}

//************************************************************************
// 	Adds the shape to the mask.
//	Sets the dirty flag for the mask to dirty if dirty != -1.
// 	If dirty == -1 then no undo is created.
//************************************************************************
void SetNewMaskShapes(LPSHAPE lpShapes, int dirty)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rUpdate, rDummy;
	BOOL fUsingUndo, bUpdateMaskRect;
	BOOL fNewMask = NO;

	// any image?
	if ( !lpImage )
		return;
	
	// disable mask marquee while changing mask
	EnableMarquee(NO);

	// get mask, create one if one does not exist
	lpMask = ImgGetMaskEx(lpImage, GetNewMaskState(lpShapes), Control.NoUndo,
							 &fNewMask);
	if (!lpMask)
		return;

	// if not new mask and we are allowed to create undo, initialize undo
	lpBase = ImgGetBase(lpImage);
	if (fUsingUndo = ((dirty != -1) || fNewMask))
	{
		if (!ImgEditInit(lpImage, ET_OBJECT,
				fNewMask ? UT_CREATEMASK : UT_ALPHA, lpBase))
  		{
			return;
  		}
	}
	
	// add shapes to mask
	if (!MaskAddShapes(lpMask, lpShapes))
  	{
		Message( IDS_EMEMALLOC );
		return;
  	}

	// find out area occupied by shapes
	rUpdate = lpShapes->rShape;
	bUpdateMaskRect = (lpShapes->iModes != SHAPE_ADD);
	lpShapes = lpShapes->lpNext;
	while (lpShapes)
	{
		AstralUnionRect(&rUpdate,&rUpdate, &lpShapes->rShape);
		lpShapes = lpShapes->lpNext;
		bUpdateMaskRect = bUpdateMaskRect || (lpShapes->iModes != SHAPE_ADD);
	} 							
	
	// if using undo, indicate in-place editing has occurred
	if (fUsingUndo)
		ImgEditedObject(lpImage, lpBase, dirty,
						fNewMask ? (LPRECT)NULL : (LPRECT)&rUpdate);
	
	// after subtractive masks we dont know area
	if (bUpdateMaskRect)
		MaskRectUpdate(lpMask, &rDummy);
	// get area that needs to be redisplayed, may be larger
	// than rUpdate because of slime
	if (ImgGetMaskUpdateRect(lpImage, fNewMask, NO, &rUpdate))
  	{
		File2DispRect(&rUpdate, &rUpdate);
		++rUpdate.right; ++rUpdate.bottom;
		InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
  	}

	// Turn on marquee display
	EnableMarquee(YES);
}

//************************************************************************
// 	Adds the ellipse to the mask.
//	Sets the dirty flag for the mask to dirty if dirty != -1.
// 	If dirty == -1 then no undo is created.
//	Mirrors SetNewMaskShapes.
//************************************************************************
void SetNewMaskEllipse(LPRECT lpRect, int dirty, int iModes)
//************************************************************************
{
	LPOBJECT lpBase;
	LPMASK lpMask;
	RECT rUpdate, rDummy;
	BOOL fNewMask = NO;
	BOOL fUsingUndo;

	// any image?
	if ( !lpImage )
		return;
	
	// disable mask marquee while changing mask
	EnableMarquee(NO);

	// get mask, create one if one does not exist
	lpMask = ImgGetMaskEx(lpImage, iModes==SHAPE_SUBTRACT ? ON:OFF , Control.NoUndo,
							 &fNewMask);
	if (!lpMask)
		return;

	// if not new mask and we are allowed to create undo, initialize undo
	lpBase = ImgGetBase(lpImage);
	if (fUsingUndo = (fNewMask || (dirty != -1)))
	{
		if (!ImgEditInit(lpImage, ET_OBJECT,
				fNewMask ? UT_CREATEMASK : UT_ALPHA, lpBase))
  		{
			return;
  		}
	}
	
	// add ellipse to mask
	MaskAddEllipse(lpMask, lpRect, iModes);

	// find out area occupied by shapes
	rUpdate = *lpRect;
	
	// if using undo, indicate in-place editing has occurred
	if (fUsingUndo)
		ImgEditedObject(lpImage, lpBase, dirty,
					fNewMask ? (LPRECT)NULL : (LPRECT)&rUpdate);
	// after subtractive masks we dont know area
	if (iModes!=SHAPE_ADD)
		MaskRectUpdate(lpMask, &rDummy);
	// get area that needs to be redisplayed, may be larger
	// than rUpdate because of slime
	if (ImgGetMaskUpdateRect(lpImage, fNewMask, NO, &rUpdate))
  	{
		File2DispRect(&rUpdate, &rUpdate);
		++rUpdate.right; ++rUpdate.bottom;
		InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
  	}
	
	// Turn on marquee display
	EnableMarquee(YES);
}


/************************************************************************/
void SetNewMask(
/************************************************************************/
LPPOINT 	lpPoint,
LPINT 		count,
int 		nShapes,
int 		dirty,
int 		iModes)
{
	LPSHAPE lpShapes;
	
	// create shapes from the point list
	if ( !(lpShapes = MaskCreateShapes(lpPoint, count, nShapes, iModes,
										YES, NULL)) )
	{
		Message( IDS_EMEMALLOC );
		return;
	}

	// create new mask
	SetNewMaskShapes(lpShapes, dirty);
	FreeUpShapes(lpShapes);
	return;
}


/************************************************************************/
void MaskSqueeze(
/************************************************************************/
LPPOINT 	lpInPoint,
LPINT 		nPoints,
int 		nShapes,
BOOL 		fDisplayConvert)
{
LPPOINT	lpOutPoint;
int j, x, y, xlast, ylast, xlastout, ylastout, iOutCount, iInCount;
int xdir, ydir, xstartdir, ystartdir;
int nBezierPoints;
BOOL fGotVert, fGotHorz;

lpOutPoint = lpInPoint;
for (j = 0; j < nShapes; ++j)
	{
	iOutCount = 0;
	xlast = ylast = xlastout = ylastout = 32767;
	fGotVert = fGotHorz = NO;
	iInCount = nPoints[j];
	while (--iInCount >= 0)
		{
		x = lpInPoint->x;
		y = lpInPoint->y;
		if (IS_BEZIER(lpInPoint))
			{
			if (fGotVert || fGotHorz )
				{
				lpOutPoint->x = xlast;
				lpOutPoint->y = ylast;
				lpOutPoint++;
				iOutCount++;
				fGotVert = NO;
				fGotHorz = NO;
				}
			lpOutPoint->x = x;
			lpOutPoint->y = y;
			nBezierPoints = BEZIER_TOTAL_POINTS(lpInPoint);
			lpOutPoint++;
			iOutCount++;
			lpInPoint++;
			while (--nBezierPoints >= 0 && --iInCount >= 0)
				{
				// check for starting a new bezier
				if (IS_BEZIER(lpInPoint))
				{
					++iInCount;
					break;
				}
				x = lpInPoint->x;
				y = lpInPoint->y;
				if (fDisplayConvert)
//					Display2File1( &x, &y );
					Display2File( &x, &y );
				lpOutPoint->x = x;
				lpOutPoint->y = y;
				lpOutPoint++;
				iOutCount++;
				lpInPoint++;
				}
			xlast = ylast = xlastout = ylastout = 32767;
			}
		else
			{ /* Don't bother with 1 pixel vectors */
			if (fDisplayConvert)
//				Display2File1( &x, &y );
				Display2File( &x, &y );
			xdir = x - xlast;
			ydir = y - ylast;
			xdir = mbound(xdir, -1, 1);
			ydir = mbound(ydir, -1, 1);
			if ((fGotVert && ((xlast != x)||(ydir != ystartdir))) ||
				(fGotHorz && ((ylast != y)||(xdir != xstartdir))))
				{
				lpOutPoint->x = xlast;
				lpOutPoint->y = ylast;
				lpOutPoint++;
				iOutCount++;
				fGotVert = NO;
				fGotHorz = NO;
				}
			if ( !fDisplayConvert || (abs(x-xlastout) + abs(y-ylastout) != 1))
				{
				if (x == xlast && ydir)
					{
					ystartdir = ydir;
					fGotVert = YES;
					}
				else if (y == ylast && xdir)
					{
					xstartdir = xdir;
					fGotHorz = YES;
					}
				else
					{
					xlastout = x;
					ylastout = y;
					lpOutPoint->x = x;
					lpOutPoint->y = y;
					lpOutPoint++;
					iOutCount++;
					}
				}
			xlast = x;
			ylast = y;
			lpInPoint++;
			}
		}
	if (fGotVert || fGotHorz)
		{
		lpOutPoint->x = xlast;
		lpOutPoint->y = ylast;
		lpOutPoint++;
		iOutCount++;
		}
	nPoints[j] = iOutCount;
	}
}

//************************************************************************/
// 	Takes a Shape list and compresses the points.
//	Compression:
//		1: All edges will be at least MinEdge in length.
//		2: Any series of edges which deviate from a line by
//			Tolerance or less are turned into one line.
//		3: beziers are ignored.
//************************************************************************/
void CompressShapes(LPSHAPE lpHeadShape, int MinEdge, int Tolerance)
//************************************************************************/
{
	while (lpHeadShape)
	{
		CompressMask(MinEdge,
					Tolerance,
					lpHeadShape->lpPoints,
					&lpHeadShape->nPoints,
					1);
		if (lpHeadShape->bClosed && lpHeadShape->nPoints) 
			if (lpHeadShape->lpPoints[lpHeadShape->nPoints-1].x == lpHeadShape->lpPoints[0].x &&
					lpHeadShape->lpPoints[lpHeadShape->nPoints-1].y == lpHeadShape->lpPoints[0].y &&
					lpHeadShape->nPoints>1)
				lpHeadShape->nPoints--;								
		lpHeadShape = lpHeadShape->lpNext;
	}	
	
}
//************************************************************************/
// 	Takes a set of points and compresses them.
//	The 'array' nPoints is set to the new values.
//	Compression:
//		1: All edges will be at least MinEdge in length.
//		2: Any series of edges which deviate from a line by
//			Tolerance or less are turned into one line.
//		3: beziers are ignored.
//************************************************************************/
void CompressMask(int MinEdge, int Tolerance, LPPOINT lpPoints,LPINT nPoints, int nShapes)
//************************************************************************/
{
	LPPOINT lpOut, lpBegin, lpEnd;
	int i, j, count, num, begin, end;
	POINT base, vect;
	int dx, dy;
	LONG edge, tolerance;
	LONG length, veclen, dist;		
	BOOL done;

	
	edge = (MinEdge-1)*(MinEdge-1);
	tolerance = Tolerance;

	lpOut = lpPoints;
	lpEnd = lpPoints-1;
	for (j = 0; j < nShapes; ++j)
	{
		count = 0;	
		num = nPoints[j];
		if (num <= 0)
			continue;
		end = 0;
		lpEnd++;
		if(!MinEdge)
		{
			// just remove duplicate points
			while (end<num)
			{
				*lpOut++ = *lpEnd;
				count++;
				base = *lpEnd;
				while(lpEnd->x == base.x && lpEnd->y == base.y)
				{
					lpEnd++;
					end++;
				}
			}
			continue;
		}
		else
			while (end<num)
			{
				*lpOut++ = *lpEnd;
				count++;
				lpBegin = lpEnd;
				begin = end;
				base = *lpBegin;
				// begin search to see how far to go
				length = 0;
				done = FALSE;
				end++;
				lpEnd++;
				while (!done && end<num)
				{
					// test length condition
					if (length < edge)
					{
						dx = lpEnd->x - base.x;		
						dy = lpEnd->y - base.y;		
						length = dx*(long)dx+dy*(long)dy;
					}
					if (length >= edge && end > begin+1)
					{
						// test tolerance condition for each intermediate
						vect.x = lpEnd->x-base.x;
						vect.y = lpEnd->y-base.y;
						veclen = vect.x*(long)vect.x+vect.y*(long)vect.y;
						if (!veclen)
						{
							// backup one step
							end--;
							lpEnd--;
							done = TRUE;
							continue;
						}
						for (i=1; i<end-begin; i++)
						{
							// compute orthoginal distance to vector
							dist = (long)vect.x*(lpBegin[i].y-base.y) 
									- (long)vect.y*(lpBegin[i].x-base.x);
							dist /= lsqrt(labs(veclen));
							dist = labs(dist);
							if(dist > tolerance)
							{
								// backup one step
								end--;
								lpEnd--;
								done = TRUE;
								break;
							}  // end if
						}	// end for
					}	// end if 
					if (!done)
					{
						end++;
						lpEnd++;
					}
				}	// end while
			}	// end while
		nPoints[j] = count;
	}
}

/************************************************************************/
void RemoveMask()
/************************************************************************/
{
RemoveMaskEx(lpImage, IDS_UNDOMASKREMOVE);
}

/************************************************************************/
void RemoveMaskEx(LPIMAGE lpImage, int dirty)
/************************************************************************/
{
LPMASK lpMask;
RECT rUpdate;
BOOL fUpdate;
LPOBJECT lpBase;

if ( !lpImage )
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
// dont free mask if it was stored for later undo/redo
if ( dirty <= 0 || !lpBase->lpUndoAlpha)
	MaskClose(lpMask);
ImgSetMask(lpImage, NULL);
if (dirty > 0)
	ImgEditedObject(lpImage, lpBase, IDS_UNDOMASKREMOVE, NULL);
//MaskClose( lpMask );

if (fUpdate)
	{
	File2DispRect(&rUpdate, &rUpdate);
	++rUpdate.right; ++rUpdate.bottom;
	InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
	}
if ( Tool.hRibbon )
	SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
EnableMarquee( YES );
}


/************************************************************************/
BOOL WriteMask(LPSTR lpClipName, LPMASK lpMask, BOOL fMaskedArea)
/************************************************************************/
{
RECT rMask;

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
if (!AstralFrameSave( lpClipName, lpMask->Pixmap.EditFrame, &rMask, IDC_SAVECT,
					 IDN_TIFF, NO ))
	{
	Message( IDS_EWRITE, lpClipName );
	return(FALSE);
	}
else
	return(TRUE);
}

/************************************************************************/
LPMASK ReadMask(LPSTR lpClipName, int iMaxWidth, int iMaxHeight)
/************************************************************************/
{
LPMASK lpMask;
LPFRAME lpFrame;
ITEMID DataType;

if (!(lpFrame = AstralFrameLoad(lpClipName, 1, &DataType, NULL)))
	{
	if (FileExists(lpClipName))
		Message ( IDS_EMASKREAD, lpClipName );
	else
		Message ( IDS_EOPEN, lpClipName );
	return(NULL);
	}

if ( !( lpMask = (LPMASK)MaskCreate(lpFrame, 
						FrameXSize(lpFrame), FrameYSize(lpFrame),
						NO, Control.NoUndo)))
	{
	Message( IDS_EMEMALLOC );
	FrameClose(lpFrame);
	return( NULL );
	}
return(lpMask);
}

/************************************************************************/
void OffsetShapes(
/************************************************************************/
LPSHAPE 	lpShape,
int 		dx,
int			dy)
{
while (lpShape)
	{
	OffsetRect(&lpShape->rShape, dx, dy);
	TMove(&lpShape->tform, dx, dy);
	lpShape = lpShape->lpNext;
	}
}

/************************************************************************/
BOOL ApplyMaskSelObj(LPIMAGE lpImage)
/************************************************************************/
{
ENGINE Engine;
LPMASK lpMask;
LPFRAME lpMaskFrame;

if (!(lpMask = ImgGetMask(lpImage)))
	return(FALSE);
if (!(lpMaskFrame = lpMask->Pixmap.EditFrame))
	return(FALSE);
SetEngineDef(&Engine);
Engine.fEditAlpha = YES;
Engine.MergeMode = MM_MULTIPLY;
Engine.lpSrcFrame = lpMaskFrame;
Engine.MaskType = MT_NONE;
return(LineEngineSelObj(lpImage, &Engine, IDS_UNDOMERGEMASK));
}


/************************************************************************/
static BOOL GetNewMaskState(LPSHAPE lpShape)
/************************************************************************/
{
while (lpShape)
	{
	if (lpShape->iModes != SHAPE_SUBTRACT)
		return(OFF);
	lpShape = lpShape->lpNext;
	}
return(ON);
}

