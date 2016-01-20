//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "animate.h"

#ifdef _MAC
#define LINES_PER_BLT 20
#else
#define LINES_PER_BLT 20
#endif // _MAC

extern HWND hActiveWindow;
LPBLTPROCESS ImgPixelProc = NULL;

// Static prototypes
static BOOL ImgAbort();
static BOOL ImgPaintAlloc(int FileWidth, int DispWidth, int depth, int sDepth );
static BOOL ImgGetLineAlloc(int dx, int depth);
static BOOL ImgPaintGDI(LPIMAGE lpImage, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid);
static BOOL ImgPaintDisplayHook(LPIMAGE lpImage, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid);
static void ImgSlimeLine(LPIMAGE lpImage, LPOBJECT lpObject, int x, int y, int dx, LPTR lpFilePtr, LPTR lpSlimePtr, int depth);
static LPOBJECT ImgGetSelObjectEx(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fBottomUp);
static WORD ImgGetNextGroupID(LPIMAGE lpImage);
static BOOL ImgSelectObjects(LPIMAGE lpImage, LPOBJECT lpObject, LPRECT lpRect, BOOL fSelect,
				BOOL fBottomUp, BOOL fExcludeBase);
static void ImgAdjustGroupSelection(LPIMAGE lpImage, BOOL fSelect);

/************************************************************************/
void ImgGetInfo(LPIMAGE lpImage, LPINT lpXSize, LPINT lpYSize,
					LPINT lpDepth, LPFRMDATATYPE lpType)
/************************************************************************/
{
LPOBJECT lpBase;

if (!lpImage)
	return;
lpBase = ImgGetBase(lpImage);
PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, lpXSize, lpYSize, lpDepth, lpType);
}

/************************************************************************/
LPOBJECT CALLBACK EXPORT ImgGetBase(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(NULL);
return((LPOBJECT)lpImage->ObjList.lpHead);
}

/************************************************************************/
LPFRAME CALLBACK EXPORT ImgGetBaseEditFrame(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(NULL);
return(ObjGetEditFrame(ImgGetBase(lpImage)));
}


/************************************************************************/
void CALLBACK EXPORT ImgReplaceBaseEditFrame(LPIMAGE lpImage, LPFRAME lpNewFrame)
/************************************************************************/
{
    LPFRAME  lpOrigFrame;
    LPOBJECT lpBaseObject;
    
    if (!lpImage || !lpNewFrame || !(lpBaseObject = ImgGetBase(lpImage)))
	    return;
    
    lpOrigFrame = ObjGetEditFrame(lpBaseObject);
    lpBaseObject->Pixmap.EditFrame = lpNewFrame; 
    if (lpOrigFrame)
        FrameClose(lpOrigFrame);
}



/************************************************************************/
LPFRAME ImgGetBaseUndoFrame(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(NULL);
return(ObjGetUndoFrame(ImgGetBase(lpImage)));
}

/************************************************************************/
LPMASK ImgGetMask(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(NULL);
return(ObjGetAlpha(ImgGetBase(lpImage)));
}

//************************************************************************
//	Gets the current mask if any.
// 	If no mask exists then create one using fOn and fNoUndo.
//	bCreated is TRUE if a new mask was created.
//************************************************************************
LPMASK ImgGetMaskEx(LPIMAGE lpImage, BOOL fOn, BOOL fNoUndo, LPINT bCreated)
//************************************************************************
{
	LPMASK lpMask;
	LPFRAME lpFrame;

	if (!lpImage)
		return(NULL);
	if (lpMask = ImgGetMask(lpImage))
	{
		if (bCreated)
			*bCreated = FALSE;
		return(lpMask);
	}
	lpFrame = ImgGetBaseEditFrame(lpImage);
	if (!(lpMask = MaskCreate(NULL, FrameXSize(lpFrame),
								FrameYSize(lpFrame),
								fOn,
								fNoUndo)))
  	{
		Message( IDS_EMEMALLOC );
		return(NULL);
  	}
	if (bCreated)
		*bCreated = TRUE;
	if (!Control.UseMaskAndObjects)
		RemoveObjectMarquee(lpImage);
	ImgSetMask(lpImage,lpMask);
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	UpdateStatusBar( NO, NO, YES, NO );
	return(lpMask);
}

/************************************************************************/
void ImgSetMask(LPIMAGE lpImage, LPMASK lpMask)
/************************************************************************/
{
if (!lpImage)
	return;
ImgGetBase(lpImage)->lpAlpha = lpMask;
}

/************************************************************************/
BOOL ImgGetMaskRect(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
LPMASK lpMask;

if (lpMask = ImgGetMask(lpImage))
	{
	MaskRect(lpMask, lpRect);
	return(TRUE);
	}
else
	{
	ImgGetImageRect(lpImage, lpRect);
	return(FALSE);
	}
}

/************************************************************************/
void ImgGetImageRect(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
LPFRAME lpFrame;

if (!lpImage)
	return;
lpFrame = ImgGetBaseEditFrame(lpImage);
SetRect(lpRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
}

/************************************************************************/
void CALLBACK EXPORT ImgChangeSelection(LPIMAGE lpImage, LPOBJECT lpObject,
										BOOL fSelect, BOOL fExtendedSel)
/************************************************************************/
{
if (!lpImage)
	return;
if (!lpObject)
	return;
if (!fExtendedSel && fSelect)
	ObjDeselectAll(&lpImage->ObjList);
ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
ImgAdjustGroupSelection(lpImage, fSelect);
SetupMiniViews(NULL, YES);
}

/************************************************************************/
BOOL ImgSelObject(LPIMAGE lpImage, LPOBJECT lpObject, LPRECT lpRect,
					BOOL fExtendedSel, BOOL fExcludeBase)
/************************************************************************/
{
BOOL fSelected, fSelect;

if (!lpImage)
	return(FALSE);

// deselect anything currently selected if not an extended selection
if (!fExtendedSel)
	ObjDeselectAll(&lpImage->ObjList);

// Try to select a new object(s)
fSelected = ImgSelectObjects(lpImage, lpObject, lpRect, YES, NO, fExcludeBase);

// nothing selected and not extended select mode, we're done
if (!fSelected && !fExtendedSel)
	return(FALSE);

// if we couldn't select something, try to deselect something instead,
// but only if we are in extended select mode
if (fSelected)
	fSelect = YES;
else
	{
	fSelected = ImgSelectObjects(lpImage, lpObject, lpRect, NO, NO,
								fExcludeBase);
	fSelect = NO;
	}

// nothing selected or deselected, we're done
if (!fSelected)
	return(FALSE);

// if we are selecting/unselecting we need to make sure all
// grouped objects have the same select/unselect state
ImgAdjustGroupSelection(lpImage, fSelect);

SetupMiniViews(NULL, YES);
return(fSelected);
}

/************************************************************************/
static void ImgAdjustGroupSelection(LPIMAGE lpImage, BOOL fSelect)
/************************************************************************/
{
LPOBJECT lpObject, lpNextObject;

// if we are selecting/unselecting we need to make sure all
// grouped objects have the same select/unselect state
lpNextObject = NULL;
while (lpNextObject = ImgGetNextObject(lpImage, lpNextObject, YES, NO))
	{
	if (lpNextObject->fSelected != fSelect)
		continue;
	if (!lpNextObject->wGroupID)
		continue;
	lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		{
		if (lpObject->wGroupID == lpNextObject->wGroupID)
			ObjSelectObject((LPPRIMOBJECT)lpObject, lpNextObject->fSelected);
		}
	}
// Now just make sure all grouped objects have the same select state
lpNextObject = NULL;
while (lpNextObject = ImgGetNextObject(lpImage, lpNextObject, YES, NO))
	{
	if (!lpNextObject->wGroupID)
		continue;
	lpObject = lpNextObject;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		{
		if (lpObject->wGroupID == lpNextObject->wGroupID)
			ObjSelectObject((LPPRIMOBJECT)lpObject, lpNextObject->fSelected);
		}
	}
}


/************************************************************************/
void ImgPlaySingleAnimation (LPIMAGE lpImage, LPOBJECT lpObject, BOOL bPressed)
/************************************************************************/
// Plays animation for one single object without yielding.
{
	STRING 		szString;
	FNAME 		szFileName;
	HINSTANCE 	hToy = 0;
	int 		hAObject = 0;
	LPAOBJECT 	Asession = NULL; // The session handle

    if (lpImage && lpObject &&
        lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
    {
        ProgressBegin(1, IDS_PROGPASTE);

		wsprintf( szString, "%04d", (int)(WORD)lpObject->dwObjectData );
        if ( GetToyFileName( szString, szFileName ) )
        {
            lpObject->fHidden = TRUE;
            lpObject->fChanged = TRUE;
            UpdateImage(&lpObject->rObject, YES);
            AnimateToy (lpImage->hWnd,lpObject->rObject.left,
                        lpObject->rObject.top,szFileName,bPressed,
                        hToy, hAObject, Asession);
            lpObject->fHidden = FALSE;
            UpdateImage(&lpObject->rObject, YES);
        }
        ProgressEnd();
    }
}


/************************************************************************/
BOOL ImgSelectObjects(LPIMAGE lpImage, LPOBJECT lpStartObject, LPRECT lpRect,
				BOOL fSelect, BOOL fBottomUp, BOOL fExcludeBase)
/************************************************************************/
{
BOOL 		fHaveRect, fSelected;
BOOL		fLoop;
LPOBJECT 	lpObject, lpBase;

if (lpRect)
	fHaveRect = lpRect->left < lpRect->right;
fLoop = lpStartObject != NULL;
lpObject = lpStartObject;
fSelected = FALSE;
lpBase = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, fBottomUp, fLoop))
	{
	if (!fExcludeBase || lpObject != lpBase)
		{
		if (!lpRect)
			{
			if (fSelect != lpObject->fSelected)
				{
				ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
				fSelected = TRUE;
				}
			}
		else if (fHaveRect)
			{
			if (lpObject->rObject.left >= lpRect->left &&
				lpObject->rObject.right <= lpRect->right &&
				lpObject->rObject.top >= lpRect->top &&
				lpObject->rObject.bottom <= lpRect->bottom)
				{
				if (fSelect != lpObject->fSelected)
					{
					ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
					fSelected = TRUE;
					}
				}
			}
		else
			{
			if (PtInRect(&lpObject->rObject, *((LPPOINT)lpRect)))
				{
				if (lpObject == lpBase || !lpObject->lpAlpha ||
					IsMasked(lpObject->lpAlpha,
							lpRect->left-lpObject->rObject.left,
							lpRect->top-lpObject->rObject.top))
					{
					if (fSelect != lpObject->fSelected)
						{
						ObjSelectObject((LPPRIMOBJECT)lpObject, fSelect);
						fSelected = TRUE;
						}
					break;
					}
				}
			}
		}
	if (lpObject == lpStartObject)
		break;
	}
return(fSelected);
}

/************************************************************************/
int ImgCountSelObjects(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
int nCount = 0;

if (lpImage)
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		++nCount;
return(nCount);
}

/************************************************************************/
int CALLBACK EXPORT ImgCountObjects (LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
int nCount = 0;

if (lpImage)
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		++nCount;
return(nCount);
}

/************************************************************************/
LPOBJECT ImgGetSelObject(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
return(ImgGetSelObjectEx(lpImage, lpObject, YES));
}

/************************************************************************/
LPOBJECT ImgGetSelObjectEx(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fBottomUp)
/************************************************************************/
{
LPOBJECT lpSelObject = NULL;

if (!lpImage)
	return(NULL);

lpSelObject = ImgGetNextObject(lpImage, lpObject, fBottomUp, NO);
if (Control.UseMaskAndObjects || !ImgGetMask(lpImage))
	{
	while (lpSelObject && !lpSelObject->fSelected)
		lpSelObject = ImgGetNextObject(lpImage, lpSelObject, fBottomUp, NO);
	if (!lpSelObject && !lpObject)
		lpSelObject = ImgGetBase(lpImage);
	}
return(lpSelObject);
}

/************************************************************************/
void ImgInvertSelection(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject;

lpObject = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	ObjSelectObject((LPPRIMOBJECT)lpObject, !lpObject->fSelected);
SetupMiniViews(NULL, YES);
}

/************************************************************************/
BOOL ImgIsSelectedObject(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
LPOBJECT lpNext = NULL;

while (lpNext = ImgGetSelObject(lpImage, lpNext))
	{
	if (lpNext == lpObject)
		return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
LPOBJECT CALLBACK EXPORT ImgGetNextObject(LPIMAGE lpImage, LPOBJECT lpObject,
											BOOL fBottomUp, BOOL fLoop)
/************************************************************************/
{
return(ImgGetNextObjectEx(lpImage, lpObject, fBottomUp, fLoop, NO));
}

/************************************************************************/
LPOBJECT ImgGetNextObjectEx(LPIMAGE lpImage, LPOBJECT lpObject,
						BOOL fBottomUp, BOOL fLoop, BOOL fGetDeletedObjects)
/************************************************************************/
{
LPOBJECT lpNextObject = NULL;

if (!lpImage)
	return(NULL);

lpNextObject = (LPOBJECT)ObjGetNextObject(&lpImage->ObjList,
				(LPPRIMOBJECT)lpObject, fBottomUp);
while (lpNextObject && (lpNextObject->ObjectType != OT_FRAME ||
		(lpNextObject->fDeleted && !fGetDeletedObjects)))
	lpNextObject = (LPOBJECT)ObjGetNextObject(&lpImage->ObjList,
					(LPPRIMOBJECT)lpNextObject, fBottomUp);
if (fLoop && !lpNextObject)
	{
	lpNextObject = (LPOBJECT)ObjGetNextObject(&lpImage->ObjList,
						NULL, fBottomUp);
	while (lpNextObject && (lpNextObject->ObjectType != OT_FRAME ||
		(lpNextObject->fDeleted && !fGetDeletedObjects)))
		lpNextObject = (LPOBJECT)ObjGetNextObject(&lpImage->ObjList,
					(LPPRIMOBJECT)lpNextObject, fBottomUp);
	}
return(lpNextObject);
}

/************************************************************************/
LPOBJECT ImgFindSelObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp)
/************************************************************************/
{
POINT pt;
LPOBJECT lpObject = NULL;
LPOBJECT lpBase;
BYTE byte;

if (!lpImage)
	return(NULL);
pt.x = x;
pt.y = y;
lpBase = ImgGetBase(lpImage);
while (lpObject = ImgGetSelObjectEx(lpImage, lpObject, fBottomUp))
	{
	if (PtInRect(&lpObject->rObject, pt))
		{
		if (lpObject == lpBase)
			return(lpObject);
		if (!lpObject->lpAlpha)
			return(lpObject);
		if ( lpObject->fTransform )
			{
			if ( TransformSingleLine( lpObject,
				x-lpObject->rObject.left, y-lpObject->rObject.top,
				1, lpObject->lpAlpha->Pixmap.EditFrame, &byte, 1/*Depth*/ ) )
				if ( byte )
					return(lpObject);
			}
		else
			{
			if (IsMasked(lpObject->lpAlpha,
				x-lpObject->rObject.left, y-lpObject->rObject.top))
					return(lpObject);
			}
		}
	}
return(NULL);
}

/************************************************************************/
LPOBJECT CALLBACK EXPORT ImgFindObject(LPIMAGE lpImage, int x, int y,
									BOOL fBottomUp, BOOL fExcludeBase)
/************************************************************************/
{
POINT pt;
LPOBJECT lpObject = NULL;
LPOBJECT lpBase;
BYTE byte;

if (!lpImage)
	return(NULL);
pt.x = x;
pt.y = y;
lpBase = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, fBottomUp, NO))
	{
	if (fExcludeBase && lpObject == lpBase)
		continue;
	if (PtInRect(&lpObject->rObject, pt))
		{
		if (lpObject == lpBase)
			return(lpObject);
		if (!lpObject->lpAlpha)
			return(lpObject);
		if ( lpObject->fTransform )
			{
			if ( TransformSingleLine( lpObject,
				x-lpObject->rObject.left, y-lpObject->rObject.top,
				1, lpObject->lpAlpha->Pixmap.EditFrame, &byte, 1/*Depth*/ ) )
				if ( byte )
					return(lpObject);
			}
		else
			{
			if (IsMasked(lpObject->lpAlpha,
				x-lpObject->rObject.left, y-lpObject->rObject.top))
					return(lpObject);
			}
		}
	}
return(NULL);
}

/************************************************************************/
BOOL ImgGetSelObjectRect( LPIMAGE lpImage, LPRECT lpRect, BOOL bExcludeBase )
/************************************************************************/
{
LPOBJECT lpObject;
BOOL bFoundOne;

SetRect( lpRect, 32767, 32767, -32767, -32767 );
if ( !lpImage )
	return( NO );

bFoundOne = NO;
lpObject = ( bExcludeBase ? ImgGetBase(lpImage) : NULL );
while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
	{
	AstralUnionRect( lpRect, lpRect, &lpObject->rObject );
	bFoundOne = YES;
	}

return( bFoundOne );
}

/************************************************************************/
BOOL ImgGetFloatingObjectsRect( LPIMAGE lpImage, LPRECT lpRect )
/************************************************************************/
{
LPOBJECT lpObject;

SetRect( lpRect, 32767, 32767, -32767, -32767 );
if ( !lpImage )
	return( NO );

if ( !(lpObject = ImgGetNextObject(lpImage, ImgGetBase(lpImage), YES, NO) ))
	return( NO );

while ( lpObject )
	{
	AstralUnionRect( lpRect, lpRect, &lpObject->rObject );
	lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
	}
return( YES );
}

/************************************************************************/
void CALLBACK EXPORT ImgAdjustLayerSelObj(LPIMAGE lpImage, BOOL fToTop,
											BOOL fOneLevel)
/************************************************************************/
{
LPOBJECT lpObject, lpFirstObject, lpNext, lpBase, lpLinkObject;

lpBase = ImgGetBase(lpImage);
if (fToTop)
	lpObject = ImgGetNextObject(lpImage, lpBase, YES, NO);
else
	lpObject = ImgGetNextObject(lpImage, NULL, NO, NO);
if (fOneLevel)
	{
	while (lpObject && lpObject != lpBase)
		{
		// find first selected object
		if (!lpObject->fSelected)
			{
			lpObject = ImgGetNextObject(lpImage, lpObject, fToTop, NO);
			continue;
			}
		// got a selected object, now find next unselected object to link after
		lpLinkObject = ImgGetNextObject(lpImage, lpObject, fToTop, NO);
		while (lpLinkObject && lpLinkObject->fSelected)
			lpLinkObject = ImgGetNextObject(lpImage, lpLinkObject, fToTop, NO);
		// if no unselected objects to link before or after, nothing to do
		if (!lpLinkObject)
			break;
		// can't move objects below base
		if (lpLinkObject == lpBase && !fToTop)
			break;
		// now move all consecutive selected objects up/down one level
		while (lpObject && lpObject->fSelected)
			{
			lpNext = ImgGetNextObject(lpImage, lpObject, fToTop, NO);
			ObjUnlinkObject(&lpImage->ObjList, (LPPRIMOBJECT)lpObject);
			ObjLinkObject(&lpImage->ObjList, (LPPRIMOBJECT)lpLinkObject,
						(LPPRIMOBJECT)lpObject, fToTop);
			lpLinkObject = lpObject;
			lpObject = lpNext;
			}
		lpObject = ImgGetNextObject(lpImage, lpLinkObject, fToTop, NO);
		}
	}
else
	{
		// find first selected object
	while (lpObject)
		{
		if (lpObject->fSelected)
			break;
		lpObject = ImgGetNextObject(lpImage, lpObject, fToTop, NO);
		}
  	lpFirstObject = lpObject;
	while (lpObject)
		{
		lpNext = ImgGetNextObject(lpImage, lpObject, fToTop, NO);
		if (lpObject->fSelected && (lpObject != lpBase))
			{
			ObjUnlinkObject(&lpImage->ObjList, (LPPRIMOBJECT)lpObject);
			if (fToTop)
				ObjLinkObject(&lpImage->ObjList,
							(LPPRIMOBJECT)lpImage->ObjList.lpTail,
							(LPPRIMOBJECT)lpObject, YES);
			else
				ObjLinkObject(&lpImage->ObjList,
							(LPPRIMOBJECT)lpImage->ObjList.lpHead,
							(LPPRIMOBJECT)lpObject, YES);
			}
		lpObject = lpNext;
		if (lpObject == lpFirstObject)
			break;
		}
	}
SetupMiniViews(NULL, NO);
}

/************************************************************************/
void ImgAdjustLayerObject(LPIMAGE lpImage, LPOBJECT lpObject,
						LPOBJECT lpToObject, BOOL fAfter)
/************************************************************************/
{
LPOBJECT lpBase;

if (!lpImage)
	return;
if (!lpObject || !lpToObject)
	return;
if (lpObject == lpToObject)
	return;
lpBase = ImgGetBase(lpImage);
if (lpObject == lpBase || lpToObject == lpBase)
	return;
ObjUnlinkObject(&lpImage->ObjList, (LPPRIMOBJECT)lpObject);
ObjLinkObject(&lpImage->ObjList, (LPPRIMOBJECT)lpToObject,
			(LPPRIMOBJECT)lpObject, fAfter);
SetupMiniViews(NULL, NO);
}

/************************************************************************/
void ImgGroupSelObjects(LPIMAGE lpImage, BOOL fGroup)
/************************************************************************/
{
WORD wGroupID;
LPOBJECT lpObject = NULL;

if (!lpImage)
	return;
if (fGroup)
	wGroupID = ImgGetNextGroupID(lpImage);
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (fGroup)
		lpObject->wGroupID = wGroupID;
	else
		lpObject->wGroupID = 0;
	}
}

/************************************************************************/
BOOL ImgIfSelObjectsGrouped(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject;
WORD wGroupID;

if (!lpImage)
	return(FALSE);

lpObject = ImgGetSelObject(lpImage, NULL);
wGroupID = lpObject->wGroupID;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (lpObject->wGroupID != wGroupID)
		return(FALSE);
	}
return(wGroupID != 0);
}

/************************************************************************/
BOOL ImgMultipleObjects(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
int	nCount = 0;

if (!lpImage)
	return(FALSE);

while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	++nCount;
	if (nCount > 1)
		return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
LPOBJECT ImgGetDisplayObject(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject;

if (!lpImage)
	return(NULL);
if (ImgGetViewMode(lpImage) == VM_NORMAL)
	lpObject = ImgGetBase(lpImage);
else if (lpImage->AltObjList.lpHead) // mask edit mode
	lpObject = (LPOBJECT)lpImage->AltObjList.lpHead;
else
	lpObject = (LPOBJECT)lpImage->ObjList.lpHead;
if (lpObject->RenderType != RT_FRAME)
	return(NULL);
else
	return(lpObject);
}

/************************************************************************/
BOOL ImgDisplayMarquees(LPIMAGE lpImage)
/************************************************************************/
{
// displaying normal marquee
if (ImgGetMaskMarqueeObject(lpImage))
	return(TRUE);
if (ImgGetObjectMarqueeObject(lpImage))
	return(TRUE);
return FALSE;	
}

/************************************************************************/
LPOBJECT ImgGetMaskMarqueeObject(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;

if (!lpImage)
	return(NULL);
if (!IsMarqueeVisible(lpImage))
	return(NULL);
if (ImgGetViewMode(lpImage) == VM_NORMAL)
	{
	// Use mask of object currently being edited
	lpObject = ImgGetBase(lpImage);
	}
else if (ImgGetViewMode(lpImage) == VM_SLIME)
	{
	// If in slime mode and we are editing a different object
	// than we are displaying, use object we are editing for marquee
	if (ImgGetBase(lpImage) != ImgGetDisplayObject(lpImage))
		lpObject = ImgGetBase(lpImage);
	}
return(lpObject);
}

/************************************************************************/
LPOBJECT ImgGetObjectMarqueeObject(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;

if (!lpImage)
	return(NULL);
if (!lpImage->UseObjectMarquee)
	return(NULL);

if (Control.UseMaskAndObjects || !ImgGetMask(lpImage))
	{
	lpObject = ImgGetSelObject(lpImage, NULL);
	}
return(lpObject);
}

/************************************************************************/
LPMASK ImgGetMarqueeMask(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;

if (!lpImage)
	return(NULL);
lpObject = ImgGetMaskMarqueeObject(lpImage);
if (lpObject)
	return(ObjGetAlpha(lpObject));
else
	return(NULL);
}

/************************************************************************/
LPOBJECT ImgGetSlimeObject(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(NULL);
if (lpImage->hWnd != hActiveWindow)
	return(NULL);
// slime only displayed when slime is on
if (ImgGetViewMode(lpImage) != VM_SLIME)
	return(NULL);
// Always use object we are editing for slime
return(ImgGetBase(lpImage));
}

/************************************************************************/
BOOL ImgGetMaskUpdateRect(LPIMAGE lpImage, BOOL fNewMask, BOOL fInvert, LPRECT lpRect)
/************************************************************************/
{
LPMASK lpMask;
LPOBJECT lpObject;

if (!lpImage)
	return(FALSE);
if (!(lpObject = ImgGetDisplayObject(lpImage)))
	return(FALSE);
if (!(lpMask = lpObject->lpAlpha))
	return(FALSE);

if (fNewMask || fInvert)
	{
	if (ImgGetViewMode(lpImage) == VM_NORMAL)
		return(FALSE);
	if (!fInvert && lpMask->bInvert)
		*lpRect = lpMask->rMask;
	else
		*lpRect = lpImage->lpDisplay->FileRect;
	}
return(TRUE);
}

/************************************************************************/
BOOL ImgGetUndoState(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
BOOL ObjUndoNotRedo;
int UndoNotRedo = -1;

if (!lpImage)
	return(TRUE);
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (Control.UndoObjects && !ImgIsSelectedObject(lpImage, lpObject))
		continue;
	if (lpObject->DataDirty)
		ObjUndoNotRedo = lpObject->DataUndoNotRedo;
	else if ((lpObject != ImgGetBase(lpImage)) && lpObject->lpAlpha &&
			lpObject->AlphaDirty)
		ObjUndoNotRedo = lpObject->AlphaUndoNotRedo;
	else
		continue;
	if (UndoNotRedo < 0)
		UndoNotRedo = ObjUndoNotRedo;
	else if (ObjUndoNotRedo != UndoNotRedo)
		return(TRUE);
	}
return(UndoNotRedo);
}

/************************************************************************/
int ImgGetDirtyState(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
int	dirty = NO;
int ObjDirty;

if (!lpImage)
	return(dirty);
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (Control.UndoObjects && !ImgIsSelectedObject(lpImage, lpObject))
		continue;
	if (lpObject->DataDirty)
		ObjDirty = lpObject->DataDirty;
	else if ((lpObject != ImgGetBase(lpImage)) && lpObject->lpAlpha &&
			lpObject->AlphaDirty)
		ObjDirty = lpObject->AlphaDirty;
	else
		continue;
	if (!dirty)
		dirty = ObjDirty;
	else if (ObjDirty != dirty)
		return(IDS_EDITS);
	}
return(dirty);
}

/************************************************************************/
BOOL ImgChanged(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
OBJECTLIST ObjList;
BOOL fChanged;

if ( !lpImage )
	return( NO );

if (lpImage->fChanged)
	return(YES);

while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (lpObject->fChanged)
		return( YES );
	}

fChanged = NO;
if (lpImage->AltObjList.lpHead)
	{
	ObjList = lpImage->ObjList;
	lpImage->ObjList = lpImage->AltObjList;
	lpImage->AltObjList = ObjList;
	lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		{
		if (lpObject->fChanged)
			{
			fChanged = YES;
			break;
			}
		}
	ObjList = lpImage->ObjList;
	lpImage->ObjList = lpImage->AltObjList;
	lpImage->AltObjList = ObjList;
	}

return( fChanged );
}

/************************************************************************/
void SetImgChanged(LPIMAGE lpImage, BOOL fChanged)
/************************************************************************/
{
	LPOBJECT lpObject = NULL;

	if ( !lpImage )
		return;

	lpImage->fChanged = fChanged;

	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
		lpObject->fChanged = fChanged;
	}
}

/************************************************************************/
BOOL CALLBACK EXPORT ImgCombineObjects(LPIMAGE lpImage, BOOL fNoUndo,
										BOOL fCreateMask, BOOL fForceIt)
/************************************************************************/
{
LPOBJECT lpBase, lpObject;
int		iWidth, iHeight, depth, y, iImageWidth, iImageHeight;
LPTR 	lpDataBuf, lpMaskPtr;
RECT 	rFloaters, rMask;
BOOL fNewMask;
LPMASK lpMask;
UNDO_TYPE UndoType;

// get rectangle for all floating objects
if (!ImgGetFloatingObjectsRect(lpImage, &rFloaters))
	return(FALSE);

// Get base object
lpBase = ImgGetBase(lpImage);

// get pixmap information
PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, &iImageWidth, &iImageHeight, &depth, NULL);
BoundRect(&rFloaters, 0, 0, iImageWidth-1, iImageHeight-1);

iWidth = RectWidth(&rFloaters);
iHeight = RectHeight(&rFloaters);

// allocate buffer for alpha channel combining
if (!(lpDataBuf = Alloc((long)iWidth*depth)))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}

lpMask = NULL;
if (fCreateMask)
	lpMask = ImgGetMaskEx(lpImage, OFF, Control.NoUndo, &fNewMask);

// do it to it
ProgressBegin(lpMask != NULL ? 2 : 1, IDS_PROGCOMBINE);

if (!fNoUndo)
	{
	UndoType = UT_DATA|UT_DELETEOBJECTS;
	if (lpMask)
		{
		if (fNewMask)
			UndoType |= UT_CREATEMASK;
		else
			UndoType |= UT_ALPHA;
		}
	if (!ImgEditInit(lpImage, ET_OBJECT, UndoType, lpBase))
		{
		if (!fForceIt)
			{
			ProgressEnd();
			FreeUp(lpDataBuf);
			return(FALSE);
			}
		}
	}

// do it to it
ProgressBegin(1, 0);
for (y = rFloaters.top; y <= rFloaters.bottom; ++y)
	{
	AstralClockCursor(y-rFloaters.top, iHeight, NO);
	ImgGetLine(lpImage, NULL, rFloaters.left, y, iWidth, lpDataBuf);
	PixmapWrite(&lpBase->Pixmap, PMT_EDIT, rFloaters.left, y, iWidth,
				lpDataBuf, iWidth);
	}
ProgressEnd();

if (lpMask && (lpObject = ImgGetNextObject(lpImage, lpBase, YES, NO)))
	{
	ProgressBegin(1, 0);
	for (y = rFloaters.top; y <= rFloaters.bottom; ++y)
		{
		AstralClockCursor(y-rFloaters.top, iHeight, NO);
		lpMaskPtr = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, rFloaters.left,
							y, YES);
		if (!lpMaskPtr)
			continue;
		ImgMaskLoad(lpImage, lpObject, NULL,
				rFloaters.left, y, iWidth, lpDataBuf, NO,
				CR_OR, 0, 0);
		CombineData(lpDataBuf, lpMaskPtr, iWidth, NO, CR_OR);
		}
	MaskRectUpdate(lpMask, &rMask);
	ProgressEnd();
	}

FreeUp(lpDataBuf);

RemoveObjectMarqueeEx(lpImage, NO);
lpObject = lpBase;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
// always let undo stuff initialize this
//	lpObject->fUndoDeleted = NO;
	lpObject->fDeleted = YES;
	}
//ImgDeleteObjects(lpImage, (LPOBJECT)lpBase->lpNext, NO);

if (!fNoUndo)
	ImgEditedObject(lpImage, lpBase, IDS_UNDOCHANGE, &rFloaters);
else
	lpBase->fChanged = YES;

if ( Tool.hRibbon )
	SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
ProgressEnd();
SetupMiniViews(NULL, NO);
return(TRUE);
}

/************************************************************************/
void CALLBACK EXPORT ImgDeleteObjects(LPIMAGE lpImage, LPOBJECT lpObject,
										BOOL bOnlyIfSelected)
/************************************************************************/
{
LPOBJECT lpNext;
BOOL fDeleteAll;

fDeleteAll = (lpObject == NULL) && !bOnlyIfSelected;

if (!lpObject)
	lpObject = ImgGetBase(lpImage);

while (lpObject)
	{
	lpNext = (LPOBJECT)lpObject->lpNext;
	if ( !bOnlyIfSelected || lpObject->fSelected )
		ImgDeleteObject( lpImage, lpObject );
	lpObject = lpNext;
	}
if (lpImage->AltObjList.lpHead && fDeleteAll)
	{
	lpImage->ObjList = lpImage->AltObjList;
	lpObject = ImgGetBase(lpImage);
	// this is a hack, but I don't have time for elegance
	if (lpObject && lpObject->lpAlpha)
		{
		FreeUp((LPTR)lpObject->lpAlpha);
		lpObject->lpAlpha = NULL;
		}
	while (lpObject)
		{
		lpNext = (LPOBJECT)lpObject->lpNext;
		ImgDeleteObject( lpImage, lpObject );
		lpObject = lpNext;
		}
	}
}

/************************************************************************/
void ImgPurgeObjects(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpNext, lpObject;

lpObject = ImgGetBase(lpImage);
while (lpObject)
	{
	lpNext = (LPOBJECT)lpObject->lpNext;
	if ( lpObject->fDeleted )
		ImgDeleteObject( lpImage, lpObject );
	lpObject = lpNext;
	}
}

/************************************************************************/
void CALLBACK EXPORT ImgDeleteObject(LPIMAGE lpImage, LPOBJECT lpObject )
/************************************************************************/
{
if ( !lpImage || !lpObject )
	return;
ObjDeleteObject(&lpImage->ObjList, (LPPRIMOBJECT)lpObject);
SetupMiniViews(NULL, NO);
}

static LPTR lpGLHookBuf, lpGLMaskBuf, lpGLTransformBuf;
static long lDataBufSize, lMaskBufSize;

/************************************************************************/
// if !lpImage then it used the object list lpBaseObject
/************************************************************************/
BOOL ImgGetLine(LPIMAGE lpImage, LPOBJECT lpBaseObject, int x, int y, int dx,
				LPTR lpOut)
/************************************************************************/
{
LPOBJECT 	lpObject;
LPALPHA		lpAlpha;
LPMASK		lpMask;
LPTR		lp;
LPPROCESSPROC lpProcessProc;
LPIMAGEHOOK	lpImageHook;
RECT		rLine, rSect;
int			depth, nPixels, ox, oy;
long		lOffset;


// get base image object
if (lpBaseObject)
	lpObject = lpBaseObject;
else if (lpImage)
	lpObject = ImgGetBase(lpImage);
else
	return(FALSE);

// fill buffer with base image object
// use depth of base image object
if ( !(depth = FrameDepth(ObjGetEditFrame(lpObject))) )
	depth = 1; // never worry about lineart

if (!ImgGetLineAlloc(dx, depth))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}

if (!FrameRead(ObjGetEditFrame(lpObject), x, y, dx, lpOut, dx))
	return(FALSE);

// allocate memory for a hook proc to use
if (lpImage)
	{
	lpImageHook = lpImage->lpDisplay->lpImageHook;
	lpMask = ImgGetMask(lpImage);
	}
else
	{
	lpImageHook = NULL;
	lpMask = NULL;
	}

// if an image hook has been setup, call it if base is selected
if (lpImageHook )
	if (ImgIsSelectedObject(lpImage, lpObject))
		(*lpImageHook)(x, y, dx, depth, lpGLHookBuf, lpOut, lpGLMaskBuf, lpMask);

// setup rect for this line for intersect check
rLine.left = x;
rLine.right = x + dx - 1;
rLine.top = rLine.bottom = y;
// start with first object on top of base image

while(lpObject = (lpImage ? ImgGetNextObject(lpImage, lpObject, YES, NO):(LPOBJECT)lpObject->lpNext))
	{
	// if object cannot be rendered through raster means
	if (lpObject->fDeleted ||
		(lpObject->RenderType != RT_RASTER && lpObject->RenderType != RT_FRAME) ||
		!AstralIntersectRect(&rSect, &lpObject->rObject, &rLine))
		continue;
    // if object is temporarily hidden, then skip it
    if (lpObject->fHidden)
        continue;
	// we can render frames ourselves
	if (lpObject->RenderType == RT_FRAME && ObjGetEditFrame(lpObject))
		{
		// get merge proc for this object
		lpProcessProc = GetProcessProc(lpObject->MergeMode, depth);

		ox = rSect.left-lpObject->rObject.left;
		oy = y-lpObject->rObject.top;
		nPixels = (rSect.right - rSect.left + 1);
		lOffset = (long)(rSect.left - rLine.left) * depth;
		if (!lpObject->fTransform)
			{
			lp = FramePointer((LPFRAME)ObjGetEditFrame(lpObject),
									ox, oy, NO);
			if (lp && lpImageHook)
				if (ImgIsSelectedObject(lpImage, lpObject))
					{
					copy(lp, lpGLTransformBuf, nPixels*depth);
					lp = lpGLTransformBuf;
					(*lpImageHook)(rSect.left, y, nPixels, depth,
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
				lpGLMaskBuf, 1 ) )
				{
				lp = lpGLTransformBuf;
				if (lpObject->Opacity != 255)
					ScaleData8(lpGLMaskBuf, nPixels, lpObject->Opacity);
				if (lpObject->fPasteIntoMask && lpMask)
					MaskLoader(lpMask, rSect.left, rSect.top, nPixels,
								lpGLMaskBuf, NO, CR_MULTIPLY);
				if (ColorMask.Mask && ColorMask.On)
					Shield(lpGLMaskBuf, lp, nPixels, depth);
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
						BOOL fSelected, LPCOLORINFO lpColor)
/************************************************************************/
{
LPOBJECT		lpObject;
LPALPHA		lpAlpha;
LPTR			lp;
LPFRAME		lpFrame;
LPPROCESSPROC lpProcessProc;
RECT			rLine, rSect;
int			depth, nPixels, ox, oy;
long			lOffset;
BOOL			fBaseSelected;

if (!lpBaseObject)
	lpBaseObject = ImgGetBase(lpImage);
if (!lpImage)
	fSelected = NO;

lpFrame = ObjGetEditFrame(lpBaseObject);
if ( !(depth = FrameDepth(lpFrame)) )
	depth = 1; // never worry about lineart

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
	rLine.left < 0 || rLine.top < 0 ||
	rLine.right >= FrameXSize(lpFrame) ||
	rLine.bottom >= FrameYSize(lpFrame)) )
	LoadColor(lpFrame, lpOut, dx, lpColor);

if (fSelected)
	{
	lpObject = ImgGetSelObject(lpImage, NULL);
	fBaseSelected = lpObject == ImgGetBase(lpImage);
	}
else
	lpObject = lpBaseObject;

while (lpObject)
	{
	// if object cannot be rendered through raster means
	if ((lpObject->RenderType == RT_RASTER ||
		 	lpObject->RenderType == RT_FRAME) &&
			AstralIntersectRect(&rSect, &lpObject->rObject, &rLine))
		{
		// we can render frames ourselves
		if (lpObject->RenderType == RT_FRAME)
			{
			// get merge proc for this object
			lpProcessProc = GetProcessProc(
				fSelected ? MM_NORMAL : lpObject->MergeMode, depth);

			ox = rSect.left-lpObject->rObject.left;
			oy = y-lpObject->rObject.top;
			nPixels = (rSect.right - rSect.left + 1);
			lOffset = (long)(rSect.left - rLine.left) * depth;
	 		lp = FramePointer((LPFRAME)ObjGetEditFrame(lpObject),
									ox, oy, NO);
			if (lp)
				{
				if (lpObject == lpBaseObject)
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
		lpObject = ImgGetSelObject(lpImage, lpObject);
	else if (lpImage)
		lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
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
LPMASK		lpMask;
RECT		rLine, rSect, rObject;
int			nPixels, ox, oy;
long		lOffset;
BOOL		fBaseSelect;

if (!lpImage)
	return(FALSE);

// if no get starting object...
if ( !lpObject )
	lpObject = ImgGetBase(lpImage);

// clear mask buffer initially
clr(lpMaskBuf, dx);

// set flag to check if special base image selection
if (fBaseSelect = (!lpMaskIn && fSelected))
	lpBase = ImgGetBase(lpImage);

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
				(LPTR)LineBuffer[0], 1 ) )
				CombineData( (LPTR)LineBuffer[0], lpMaskBuf+lOffset, nPixels, NO, ROP );
			else
				set( lpMaskBuf+lOffset, nPixels, 255 );
			}
		}
	if (lpMaskIn)
		break;
	if (fSelected)
		lpObject = ImgGetSelObject(lpImage, lpObject);
	else
		lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
	}
return(TRUE);
}


/************************************************************************/
BOOL ImgSetupMaskEditMode(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpNewBase, lpOldBase;
LPMASK lpMask, lpNewMask;
RECT rUpdate;
BOOL fMaskDeleted;
static BOOL fMaskCreate;
COLORINFO Color;
int dirty;
static BOOL fNoSize;
static int iUndoCreateMaskDirty;
static COLORINFO SaveActive, SaveAlternate;

if (!lpImage)
	return(FALSE);

if ( !(lpOldBase = (LPOBJECT)lpImage->AltObjList.lpHead) )
	{ // turn on mask editing mode
	lpOldBase = ImgGetBase(lpImage);
	lpMask = ImgGetMaskEx(lpImage,OFF,Control.NoUndo,&fMaskCreate);
	if (!lpMask)
		return(FALSE);

	// if a both are dirty apply alpha changes
	if (lpOldBase->fBothDirty)
		ObjEditApply(lpImage, lpOldBase, NO, YES, NO);

	// Create a new base object
	if ( !(lpNewBase = (LPOBJECT)ObjCreateFromPixmap(ST_PERMANENT,
		&lpMask->Pixmap, NULL, NULL, Control.NoUndo)) )
			{
			Message(IDS_EMEMALLOC);
			if (fMaskCreate)
				MaskClose(lpMask);
			return(FALSE);
			}

	RemoveObjectMarqueeEx(lpImage, NO);

	// Update Undo Information for alpha
	lpNewBase->DataUndoType = 0;
	lpNewBase->DataDirty = NO;
	if (lpOldBase->AlphaUndoType & UT_ALPHA)
		{
		lpNewBase->DataUndoType |= UT_DATA;
		lpNewBase->DataDirty = lpOldBase->AlphaDirty;
		}
	if (lpOldBase->AlphaUndoType & UT_NEWALPHA)
		{
		lpNewBase->DataUndoType |= UT_NEWDATA;
		lpNewBase->DataDirty = lpOldBase->AlphaDirty;
		}
	iUndoCreateMaskDirty = NO;
	if ((lpNewBase->DataUndoType == 0) &&
		(lpOldBase->AlphaUndoType & UT_CREATEMASK))
		iUndoCreateMaskDirty = lpOldBase->AlphaDirty;
	lpNewBase->DataUndoNotRedo = lpOldBase->AlphaUndoNotRedo;
	lpOldBase->fBothDirty = NO;

	// Swap the two object list pointers
	lpImage->AltObjList = lpImage->ObjList;
	lpImage->ObjList.lpHead = lpImage->ObjList.lpTail = (LPPRIMOBJECT)lpNewBase;
	// Redisplay if not in slime view mode

	fNoSize = lpImage->fNoSize;
	lpImage->fNoSize = YES;

	GetActiveColor(&SaveActive);
	GetAlternateColor(&SaveAlternate);
	Color.gray = 255;
	SetActiveColor(&Color, CS_GRAY, NO);
	Color.gray = 0;
	SetAlternateColor(&Color, CS_GRAY, NO);

	if (ImgGetViewMode(lpImage) != VM_SLIME || fMaskCreate)
		{
		SetupImagePalette(lpImage, YES, YES);
		UpdateImage(NULL, YES);
		}
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	SetupMiniViews(NULL, NO);
	}
else
	{ // turn off mask editing mode
	dirty = NO;
	if ( !(lpNewBase = ImgGetBase(lpImage)) )
		return(FALSE);
//	if (ImgMultipleObjects(lpImage))
//		{
//		Message(IDS_OBJECTSNEEDCOMBINE);
//		return(FALSE);
//		}
	// Get rid of any new base mask
	if ( lpNewMask = lpNewBase->lpAlpha )
		MaskClose( lpNewMask );

	// Combine any floating objects
	ImgCombineObjects( lpImage, NO, NO, YES );
	ImgPurgeObjects(lpImage);

	// Copy the new base Pixmap structure over to the the old mask Pixmap
	// and recompute the mask rectangle
	fMaskDeleted = NO;
	if ( lpMask = lpOldBase->lpAlpha )
		{
		lpMask->Pixmap = lpNewBase->Pixmap;
		if ( (fMaskCreate && !lpNewBase->fChanged) ||
			!MaskRectUpdate( lpMask, &rUpdate ) )
			{ // If the mask was empty, get rid of it...
			MaskClose( lpMask );
			lpOldBase->lpAlpha = NULL;
			fMaskDeleted = YES;
			lpOldBase->AlphaUndoType = 0;
			lpOldBase->AlphaUndoNotRedo = YES;
			lpOldBase->AlphaDirty = NO;
			}
		else
			{
			lpOldBase->AlphaUndoType = 0;
			lpOldBase->AlphaDirty = NO;
			if (lpNewBase->DataUndoType & UT_DATA && lpMask->Pixmap.UndoFrame)
				{
				lpOldBase->AlphaUndoType |= UT_ALPHA;
				lpOldBase->AlphaDirty = lpNewBase->DataDirty;
				}
			if (lpNewBase->DataUndoType & UT_NEWDATA && lpMask->Pixmap.UndoFrame)
				{
				lpOldBase->AlphaUndoType |= UT_NEWALPHA;
				lpOldBase->AlphaDirty = lpNewBase->DataDirty;
				}
			if (!lpMask->Pixmap.UndoFrame)
				dirty = iUndoCreateMaskDirty;
			if (lpOldBase->AlphaDirty)
				{
				SetRect(&lpMask->rUndoMask, 0, 0,
						FrameXSize(lpMask->Pixmap.UndoFrame)-1,
						FrameYSize(lpMask->Pixmap.UndoFrame)-1);
				PixmapSyncLines(&lpMask->Pixmap);
				if (!MaskRectDwindle(lpMask, &lpMask->rUndoMask,
								lpMask->Pixmap.UndoFrame))
					{
					PixmapFreeUndo(&lpMask->Pixmap);
					dirty = lpOldBase->AlphaDirty;
				   	lpOldBase->AlphaUndoType = 0;
					lpOldBase->AlphaDirty = NO;
					}
				}
			if (!lpOldBase->AlphaDirty && lpMask->Pixmap.UndoFrame)
				PixmapFreeUndo(&lpMask->Pixmap);
			lpOldBase->AlphaUndoNotRedo = lpNewBase->DataUndoNotRedo;
			}
		}
	// Free the new base object structure
	FreeUp((LPTR)lpNewBase);
	// Swap the two object list pointers
	lpImage->ObjList = lpImage->AltObjList;
	lpImage->AltObjList.lpHead = lpImage->AltObjList.lpTail = NULL;

	if (dirty)
		{
		ImgEditInit(lpImage, ET_OBJECT, UT_CREATEMASK, lpOldBase);
		ImgEditedObject(lpImage, ImgGetBase(lpImage), dirty, NULL);
		}

	lpImage->fNoSize = fNoSize;

	// Redisplay if not in slime view mode (or we had a mask)
	if (ImgGetViewMode(lpImage) != VM_SLIME || lpNewMask /*had a new mask*/ ||
			fMaskDeleted)
		{
		if (ImgGetViewMode(lpImage) != VM_SLIME)
			SetupImagePalette(lpImage, YES, YES);
		UpdateImage(NULL, YES);
		}
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	SetupMiniViews(NULL, NO);
	SetActiveColor(&SaveActive, SaveActive.ColorSpace, NO);
	SetAlternateColor(&SaveAlternate, SaveAlternate.ColorSpace, NO);
	return(TRUE);
	}
	return TRUE;	
}

/************************************************************************/
BOOL ImgInMaskEditMode(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(FALSE);
return(lpImage->AltObjList.lpHead != NULL);
}

/************************************************************************/
void ImgSetViewMode(LPIMAGE lpImage, VIEW_MODE ViewMode)
/************************************************************************/
{
if (!lpImage)
	return;
if (lpImage->ViewMode == ViewMode)
	return;
lpImage->ViewMode = ViewMode;
if (ImgInMaskEditMode(lpImage))
	SetupImagePalette(lpImage, YES, YES);
UpdateImage(NULL, YES);
}

/************************************************************************/
VIEW_MODE ImgGetViewMode(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return(VM_NORMAL);
return(lpImage->ViewMode);
}

/************************************************************************/
LPOBJECT ImgAddFrameObject(LPIMAGE lpImage, LPFRAME lpFrame, LPMASK lpAlpha, LPRECT lpRect)
/************************************************************************/
{
LPOBJECT lpObject;
RECT	rObject;

if (!lpImage)
	return(NULL);
if (lpRect)
	rObject = *lpRect;
else
	SetRect(&rObject, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
lpObject = ObjCreateFromFrame(ST_PERMANENT, lpFrame, lpAlpha,
							&rObject, Control.NoUndo);
if (lpObject)
	ObjAddTail(&lpImage->ObjList, (LPPRIMOBJECT)lpObject);
return(lpObject);
}

/************************************************************************/
LPOBJECT ImgCreateMaskObject(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
int XSize, YSize;
LPOBJECT lpBase, lpObject;
LPFRAME lpMaskFrame;
LPMASK lpMask, lpAlpha;

if (!lpImage)
	return( NULL );

lpBase = ImgGetBase(lpImage);
PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, &XSize, &YSize, NULL, NULL);

// Create the mask frame
if ( !(lpMask = ImgGetMask( lpImage )) )
	lpMaskFrame = NULL;
else
	{
	if ( lpMaskFrame = PixmapFrame( &lpMask->Pixmap, PMT_EDIT ) )
		lpMaskFrame = FrameCopy( lpMaskFrame, lpRect );
	}

// Create a new mask from the frame
if ( !(lpAlpha = MaskCreate( lpMaskFrame, RectWidth(lpRect),
		RectHeight(lpRect), ON, Control.NoUndo)) )
	{
	Message(IDS_EMEMALLOC);
	FrameClose(lpMaskFrame);
	return( NULL );
	}
lpAlpha->bInvert = lpMask->bInvert;
if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, NULL, lpAlpha,
	lpRect, Control.NoUndo )) )
	{
	Message(IDS_EMEMALLOC);
	MaskClose( lpAlpha );
	return( NULL );
	}

ImgAddNewObject(lpImage, lpObject);
return( lpObject );
}

/************************************************************************/
BOOL ImgCrop( LPIMAGE lpImage, LPRECT lpCropRect )
/************************************************************************/
{
RECT rCrop, rMask, rSect;
LPOBJECT lpBase, lpObject;
LPMASK lpMask;
LPFRAME lpDataFrame, lpMaskFrame;
UNDO_TYPE UndoType;

if ( !lpImage )
	return( NULL );

if ( !(lpBase = ImgGetBase( lpImage )) )
	return( FALSE );
lpMask = ImgGetMask( lpImage );

ProgressBegin(lpMask != NULL ? 2 : 1, IDS_PROGCROP);

if (lpCropRect)
	rCrop = *lpCropRect;
else if (lpMask)
	rCrop = lpMask->rMask;
else
	return(FALSE);

UndoType = UT_NEWDATA|UT_MOVEOBJECTS|UT_OBJECTRECT;

// will mask go away
rMask = rCrop;
if (lpMask && !MaskRectDwindle( lpMask, &rMask, NULL ))
	{
	UndoType |= UT_DELETEMASK;
	lpMask = NULL;
	}
else if (lpMask)
	UndoType |= UT_NEWALPHA;
if (!lpCropRect)
	rCrop = rMask;

lpObject = lpBase;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (!AstralIntersectRect(&rSect, &rCrop, &lpObject->rObject))
		{
		UndoType |= UT_DELETEOBJECTS;
		break;
		}
	}
if ( !ImgEditInit(lpImage, ET_OBJECT, UndoType, lpBase))
	{
	ProgressEnd();
	return(FALSE);
	}

// Copy out the new data frame
if ( !(lpDataFrame = FrameCopy( ImgGetBaseEditFrame(lpImage), &rCrop )) )
	{
	ProgressEnd();
	Message(IDS_EMEMALLOC);
	return( NULL );
	}

// Copy out the new mask frame if there is a mask
lpMaskFrame = NULL;
if (lpMask)
	{
	if ( !(lpMaskFrame = FrameCopy( lpMask->Pixmap.EditFrame, &rCrop )) )
		{
		ProgressEnd();
		FrameClose( lpDataFrame );
		Message(IDS_EMEMALLOC);
		return( NULL );
		}
	}

lpObject = lpBase;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (!AstralIntersectRect(&rSect, &rCrop, &lpObject->rObject))
		{
// always let undo stuff initialize this
//		lpObject->fUndoDeleted = NO;
		lpObject->fDeleted = YES;
		}
	else
		OffsetRect(&lpObject->rObject, -rCrop.left, -rCrop.top);
	}

if (UndoType & UT_DELETEMASK)
	RemoveMaskEx(lpImage, -1);

ImgEditedObjectFrame(lpImage, lpBase, IDS_UNDOCHANGE, NULL, lpDataFrame, lpMaskFrame);
// Setup the new image and bring up the new image window
lpBase->rObject.right = FrameXSize(lpDataFrame)-1;
lpBase->rObject.bottom = FrameYSize(lpDataFrame)-1;
if (lpMask)
	{
	lpMask->rMask = rMask;
	OffsetRect(&lpMask->rMask, -rCrop.left, -rCrop.top);
	}

UpdateImageSize();
SetupMiniViews(NULL, NO);
ProgressEnd();
return( TRUE );
}

/************************************************************************/
LPOBJECT ImgCreateCutoutObject(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
int XSize, YSize;
LPOBJECT lpBase, lpObject;
LPFRAME lpDataFrame, lpMaskFrame;
LPMASK lpMask, lpAlpha;

if (!lpImage)
	return( NULL );

lpBase = ImgGetBase(lpImage);
PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, &XSize, &YSize, NULL, NULL);

if ( !(lpDataFrame = ImgCopyFrame(lpImage, NULL, lpRect, YES, NO)) )
	{
	Message(IDS_EMEMALLOC);
	return( NULL );
	}

// Create the mask frame
if ( !(lpMask = ImgGetMask( lpImage )) )
	lpMaskFrame = NULL;
else
	{
	if ( lpMaskFrame = PixmapFrame( &lpMask->Pixmap, PMT_EDIT ) )
		lpMaskFrame = FrameCopy( lpMaskFrame, lpRect );
	}

// Create a new mask from the frame
if ( !(lpAlpha = MaskCreate( lpMaskFrame, RectWidth(lpRect),
		RectHeight(lpRect), ON, Control.NoUndo)) )
	{
	Message(IDS_EMEMALLOC);
	FrameClose(lpDataFrame);
	FrameClose(lpMaskFrame);
	return( NULL );
	}

if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, lpDataFrame, lpAlpha,
	lpRect, Control.NoUndo )) )
	{
	Message(IDS_EMEMALLOC);
	MaskClose( lpAlpha );
	FrameClose(lpDataFrame);
	return( NULL );
	}
ImgAddNewObject(lpImage, lpObject);
return( lpObject );
}

/************************************************************************/
void CALLBACK EXPORT ImgAddNewObject(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
RemoveObjectMarquee(lpImage);
ObjAddTail( &lpImage->ObjList, (LPPRIMOBJECT)lpObject ); // Add to the image's list
ObjDeselectAll(&lpImage->ObjList); // Deselect anything currently selected
ObjSelectObject( (LPPRIMOBJECT)lpObject, YES ); // Select the new object
UpdateImage(&lpObject->rObject, YES);
SetupMiniViews(NULL, NO);
}

/************************************************************************/
void ImgDeleteTempObjects(LPIMAGE lpImage)
/************************************************************************/
{
if (!lpImage)
	return;
ObjDeleteObjects(&lpImage->ObjList, ST_TEMPORARY);
SetupMiniViews(NULL, NO);
}

/************************************************************************/
BOOL ImgKeepFrameTempObjects(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;

if (!lpImage)
	return(FALSE);
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if ( lpObject->StorageType == ST_TEMPORARY &&
		 lpObject->ObjectType == OT_FRAME )
			lpObject->StorageType = ST_PERMANENT;
	}
return(TRUE);
}

/************************************************************************/
//	 Currently will not scale up.
//	 Adjusts lpRect in the scale case to maintain aspect ratio.
/************************************************************************/
LPFRAME ImgCopyFrame(LPIMAGE lpImage, LPOBJECT lpBaseObject, LPRECT lpRect,
					BOOL bCrop, BOOL fSelected)
/************************************************************************/
{
LPFRAME lpBaseFrame, lpNewFrame;
LPOBJECT lpObject;
int		depth, dx, dy, y, width, height;
LPTR	lpDataBuf;
RECT	rMask;
LFIXED 	rate, line;
LPTR	lpDst;
WORD 	yread;
COLORINFO Color;

if (!lpImage)
	fSelected = NO;
//if (fSelected)
//	{
//	if (ImgCountSelObjects(lpImage, NULL) > 1)
//		fSelected = NO;
//	}
if (lpBaseObject)
	lpObject = lpBaseObject;
else if (lpImage)
	lpObject = ImgGetBase(lpImage);
else
	return (FALSE);

lpBaseFrame = ObjGetEditFrame(lpObject);

if (lpRect)
	rMask = *lpRect;
else if (lpImage)
	ImgGetMaskRect(lpImage, &rMask);
else
	rMask = lpObject->rObject;	// why not?

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
if (!(lpNewFrame = FrameOpen((FRMDATATYPE)depth, dx, dy, FrameResolution(lpBaseFrame))))
	{
	FreeUp(lpDataBuf);
	return(NULL);
	}
Color.gray = 255;
SetColorInfo(&Color, &Color, CS_GRAY);
if (bCrop)
	{
	for (y = rMask.top; y <= rMask.bottom; ++y)
		{
		if (!ImgGetObjectLine(lpImage, lpObject, rMask.left, y, dx, lpDataBuf,
							 fSelected, &Color) ||
			!FrameWrite(lpNewFrame, 0, y-rMask.top, dx, lpDataBuf, dx))
			{
			Message( IDS_EREAD, (LPTR)Control.RamDisk );
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
		yread = (long)line >> 16;
		lpDst = FramePointer(lpNewFrame,0,y,YES);
	   	if (!ImgGetObjectLine(lpImage, lpObject, 0, yread, width, lpDataBuf,
							fSelected, &Color) || !lpDst)
			{
			FrameClose(lpNewFrame);
			return(NULL);
			}
		FrameSample(lpNewFrame, lpDataBuf, 0, lpDst, 0, dx, rate);
		line += rate;
		} 	// for y
	}
FreeUp(lpDataBuf);
return(lpNewFrame);
}

/************************************************************************/
BOOL ImgPaint(LPIMAGE lpImage, HDC hDC, LPRECT lpRepair,
				LPRECT lpInvalid)
/************************************************************************/
{
if (!lpImage)
	return(NO);
SelectClipRect(hDC, &lpImage->lpDisplay->DispRect, NULL);
if (!ImgPaintSetup(lpImage, lpInvalid != NULL))
	goto ExitNo;
if (!ImgPaintRaster(lpImage, hDC, lpRepair, lpInvalid))
	goto ExitNo;
if (lpInvalid && lpInvalid->right >= lpInvalid->left)
	ExcludeClipRect(hDC, lpInvalid->left, lpInvalid->top,
					lpInvalid->right+1, lpInvalid->bottom+1);
if (!ImgPaintGDI(lpImage, hDC, lpRepair, lpInvalid))
	goto ExitNo;
if (!ImgPaintDisplayHook(lpImage, hDC, lpRepair, lpInvalid))
	goto ExitNo;
return( YES );

ExitNo:
//if (bOldClip)
//	SelectClipRect(hDC, &OldClip, NULL);
return(NO);
}

static LPTR lpPRDispBuf,  lpPRFileBuf,  lpPRSlimeBuf, lpWideSlimeBuf;
static long lDispBufSize, lFileBufSize, lSlimeBufSize, lWideSlimeBufSize;
static LPOBJECT lpDispObject, lpSlimeObject;
static BOOL fAnyObjects, fDoSample, fDoInterrupt;
static LPFRAME lpDispFrame;
static int depth, sDepth;
static RECT rDisp, rFile;
static int xDiva, yDiva;
static LFIXED xrate, yrate;

/************************************************************************/
BOOL ImgPaintSetup(LPIMAGE lpImage, BOOL fDoInterruptIn)
/************************************************************************/
{
	int DispWidth, FileWidth;

	// head of list should be base frame
	// always use depth of base frame
	if (!(lpDispObject = ImgGetDisplayObject(lpImage)))
		return(FALSE);
	fDoInterrupt = fDoInterruptIn;
	fAnyObjects = ImgGetNextObject(lpImage, lpDispObject, YES, NO) != NULL;
	if (lpImage->lpDisplay->lpImageHook)
	  	fAnyObjects = YES;

	lpDispFrame = ObjGetEditFrame(lpDispObject);
	lpSlimeObject = ImgGetSlimeObject(lpImage);

	if ( !(depth = FrameDepth(lpDispFrame)) )
		depth = 1; // never worry about lineart

	sDepth = depth;

	// Force gray slime to be color...
	if (lpSlimeObject && sDepth == 1)
	{
		sDepth = 3;
	}

	// get display and file rects for faster access, set dither offsets
	// so the dither pattern holds when scrolling
	rDisp = lpImage->lpDisplay->DispRect;
	rFile = lpImage->lpDisplay->FileRect;
	xDiva = lpImage->lpDisplay->xDiva;
	yDiva = lpImage->lpDisplay->yDiva;

	// get pixel width of display and file
	DispWidth = RectWidth( &rDisp );
	FileWidth = RectWidth( &rFile );

	if (!ImgPaintAlloc(FileWidth, DispWidth, depth, sDepth))
		{
		lpDispObject = NULL;
		return(FALSE);
		}

	// compute x and y display rates
	yrate = FGET(RectHeight( &rFile),RectHeight(&rDisp));
	xrate = FGET(FileWidth, DispWidth);
#ifdef _MAC
	xrate = FONE;
	yrate = FONE;
#endif	
	fDoSample = xrate != FONE;

	return(TRUE);
}

/************************************************************************/
BOOL ImgPaintRaster(LPIMAGE lpImage, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid)
/************************************************************************/
{
	int sdx, ddx, ddy;
	int PaintWidth;
	int y, yline, ylast, xstart, xend, xtotal;
	int dxCount, increment;
	LFIXED yoffset, xoffset;
	RECT rPaint;
	BLTSESSION BltSession;
	LPTR lpFilePtr, lpFileLine, lpDispPtr, lpWideSlimePtr;
	LPTR lpOptimizeBuf = NULL;
	long lOffset;
	LPOBJECT lpOldSlimeObject;
#ifdef STATIC16  // Only in the new framelib
	FRMTYPEINFO TypeInfo;
    CFrameTypeConvert TypeConvert;
#endif

	// head of list should be base frame
	// always use depth of base frame
	if (!lpDispObject)
		return(NO);

	// if no repair rect, we are redisplay the whole image
	if ( !lpRepair )
		rPaint = rDisp;
	else
	{ // bound repair rect to the display rect
		rPaint = *lpRepair;
		BoundRect( &rPaint, rDisp.left, rDisp.top, rDisp.right, rDisp.bottom );
	}

	// get pixel width of display and file
	PaintWidth = RectWidth( &rPaint );

	if ((PaintWidth <= 0) || (RectHeight(&rPaint) <= 0))
		return(NO);

	if (lpBltScreen->bOptimized)
	{
		AllocLines(&lpOptimizeBuf, 1, PaintWidth, 1);

		if (!lpOptimizeBuf)
			return(NO);

		lpOldSlimeObject = lpSlimeObject;
		lpSlimeObject = NULL;
	}

	// get offsets from display edge to paint rect to adjust xoffset/yoffset
	ddx = rPaint.left - rDisp.left;
	ddy = rPaint.top - rDisp.top;
	dxCount = ddx + PaintWidth;
	yoffset = (ddy*yrate)+((long)yrate>>1);
	xoffset = (ddx*xrate)+((long)xrate>>1);

	// Find x-position in file coordinates where painting begins and ends
#ifdef WIN32	
	sdx = WHOLE(xoffset);
	xstart = rFile.left + sdx;
	xoffset += ((PaintWidth-1)*xrate);
	xend = rFile.left + WHOLE(xoffset);
#else
	sdx = HIWORD(xoffset);
	xstart = rFile.left + sdx;
	xoffset += ((PaintWidth-1)*xrate);
	xend = rFile.left + HIWORD(xoffset);
#endif

	if (xend > rFile.right)
		xend = rFile.right;

	xtotal = xend - xstart + 1;

	lOffset = (long)sdx * (long)depth;
	if (fAnyObjects || lpSlimeObject)
		{
		lpFilePtr = lpPRFileBuf + lOffset;
		lpFileLine = lpPRFileBuf;
		}

	lpWideSlimePtr = lpWideSlimeBuf + ((long)sdx * (long)sDepth);

	if (fDoSample)
		lpDispPtr = lpPRDispBuf;
	else
	{
		if (sDepth == depth)
			lpDispPtr = lpFilePtr;
		else
			lpDispPtr = lpWideSlimePtr;
	}

	ylast = -1;
	increment = LINES_PER_BLT;

#ifdef STATIC16 // Only in the new framelib
	FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
	StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint, TypeInfo,
		LINES_PER_BLT, xDiva, yDiva, YES, ImgPixelProc );
#else
	StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint, sDepth,
		LINES_PER_BLT, xDiva, yDiva, YES, ImgPixelProc );
#endif

	for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
#ifdef WIN32	
		yline = rFile.top + WHOLE(yoffset);
#else
		yline = rFile.top + HIWORD(yoffset);
#endif
		yoffset += yrate;
		if ( yline != ylast )
		{
			ylast = yline;
			if (depth == sDepth)
			{
				if (fAnyObjects || lpSlimeObject)
				{
					ImgGetLine(lpImage, lpDispObject, xstart, yline, xtotal, lpFilePtr);
				}
				else
				{
					lpFileLine = FramePointer(lpDispFrame, rFile.left, yline, NO);
					if (!lpFileLine)
						goto Exit;
					lpFilePtr = lpFileLine + lOffset;
					if (!fDoSample)
						lpDispPtr = lpFilePtr;
				}
				if (lpSlimeObject)
					ImgSlimeLine(lpImage, lpSlimeObject, xstart, yline, xtotal,
						lpFilePtr, lpPRSlimeBuf, depth);

				if (fDoSample)
					FrameSample(lpDispFrame, lpFileLine, 0, lpPRDispBuf,
						ddx, dxCount, xrate);
			}
			else
			{
				// Get the grayscale data
				if (fAnyObjects || lpSlimeObject)
				{
					ImgGetLine(lpImage, lpDispObject, xstart, yline, xtotal, lpFilePtr);
				}
				else
				{
					lpFileLine = FramePointer(lpDispFrame, rFile.left, yline, NO);
					if (!lpFileLine)
						goto Exit;
					lpFilePtr = lpFileLine + lOffset;
					if (!fDoSample)
						lpDispPtr = lpWideSlimePtr;
				}
				// Expand the gray data to RGB...
#ifdef STATIC16  // Only in the new framelib
                TypeConvert.GrayScaleToRGBColor( lpFilePtr, (LPRGB)lpWideSlimePtr, y, xtotal );
#else
				GrayScaleToRGBColor( lpFilePtr, lpWideSlimePtr, xtotal );
#endif
				ImgSlimeLine(lpImage, lpSlimeObject, xstart, yline, xtotal,
					lpWideSlimePtr, lpPRSlimeBuf, sDepth);

				if (fDoSample)
					Sample_24(lpWideSlimeBuf, 0, lpPRDispBuf,
						ddx, dxCount, xrate);
			}
		}

		if (lpBltScreen->bOptimized)
		{
			OptimizeData(rPaint.left, y, PaintWidth, lpDispPtr, lpOptimizeBuf, sDepth );
			SuperBlt( &BltSession, lpOptimizeBuf );
		}
		else
		{
			SuperBlt( &BltSession, lpDispPtr );
		}
#ifndef _MAC
		if ( fDoInterrupt && (--increment <= 0) && y < rPaint.bottom )
		{
			increment = LINES_PER_BLT;
			if (ImgAbort())
			{
				lpInvalid->left = rPaint.left;
				lpInvalid->right = rPaint.right;
				lpInvalid->top = y+1;
				lpInvalid->bottom = rPaint.bottom;
				break;
			}
		}
#endif // _MAC		
	}

Exit:
	SuperBlt( &BltSession, NULL );

	if (lpOptimizeBuf)
	{
		lpSlimeObject = lpOldSlimeObject;
		FreeUp( lpOptimizeBuf );
	}

	return( YES );
}

/************************************************************************/
void CALLBACK EXPORT ImgSetPixelProc( LPBLTPROCESS lpPixelProc )
/************************************************************************/
{
ImgPixelProc = lpPixelProc;
}

/************************************************************************/
static BOOL ImgPaintAlloc(int FileWidth, int DispWidth, int depth, int sDepth)
/************************************************************************/
{
	long lDispBufNeeded, lFileBufNeeded, lSlimeBufNeeded, lWideSlimeBufNeeded;

	// allocate display buffer
	lDispBufNeeded = (long)DispWidth * sDepth;
	if (lDispBufNeeded > lDispBufSize)
	{
		if (lpPRDispBuf)
			FreeUp(lpPRDispBuf);
		lDispBufSize = lDispBufNeeded;
		if (!(lpPRDispBuf = Alloc(lDispBufSize)))
		{
			lDispBufSize = 0;
			return(FALSE);
		}
	}

	// allocate file buffer
	lFileBufNeeded = (long)FileWidth * depth;
	if (lFileBufNeeded > lFileBufSize)
	{
		if (lpPRFileBuf)
			FreeUp(lpPRFileBuf);
		lFileBufSize = lFileBufNeeded;
		if (!(lpPRFileBuf = Alloc(lFileBufSize)))
		{
			lpPRFileBuf = NULL;
			lFileBufSize = 0;
			FreeUp(lpPRDispBuf);
			lpPRDispBuf = NULL;
			lDispBufSize = 0;
			return(FALSE);
		}
	}

	// allocate slime buffer
	lSlimeBufNeeded = (long)FileWidth;
	if (lSlimeBufNeeded > lSlimeBufSize)
	{
		if (lpPRSlimeBuf)
			FreeUp(lpPRSlimeBuf);
		lSlimeBufSize = lSlimeBufNeeded;
		if (!(lpPRSlimeBuf = Alloc(lSlimeBufSize)))
		{
			lSlimeBufSize = 0;
			FreeUp(lpPRFileBuf);
			lpPRFileBuf = NULL;
			lFileBufSize = 0;
			FreeUp(lpPRDispBuf);
			lpPRDispBuf = NULL;
			lDispBufSize = 0;
			return(FALSE);
		}
	}

	if (depth != sDepth)
	{
		// allocate wide slime buffer
		lWideSlimeBufNeeded = (long)FileWidth * sDepth;
		if (lWideSlimeBufNeeded > lWideSlimeBufSize)
		{
			if (lpWideSlimeBuf)
				FreeUp(lpWideSlimeBuf);
			lWideSlimeBufSize = lWideSlimeBufNeeded;
			if (!(lpWideSlimeBuf = Alloc(lWideSlimeBufSize)))
			{
				lWideSlimeBufSize = 0;
				FreeUp(lpPRFileBuf);
				lpPRFileBuf = NULL;
				lFileBufSize = 0;
				FreeUp(lpPRDispBuf);
				lpPRDispBuf = NULL;
				lDispBufSize = 0;
				FreeUp(lpPRSlimeBuf);
				lpPRSlimeBuf = NULL;
				lSlimeBufSize = 0;
				return(FALSE);
			}
		}
	}
	return(TRUE);
}

/************************************************************************/
static BOOL ImgPaintGDI(LPIMAGE lpImage, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid)
/************************************************************************/
{
return(YES);
}

/************************************************************************/
static BOOL ImgPaintDisplayHook(LPIMAGE lpImage, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid)
/************************************************************************/
{
if (lpImage->lpDisplay->lpDisplayHook)
	(*lpImage->lpDisplay->lpDisplayHook)(hDC, lpRepair);
return(YES);
}


/************************************************************************/
static BOOL ImgAbort()
/************************************************************************/
{ // Only interrupt on things that the user initiates
#ifdef _MAC
	return FALSE;
#else
	MSG msg;

	if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE|PM_NOYIELD ) )
		return(TRUE);
	return(FALSE);
#endif
}

/************************************************************************/
static void ImgSlimeLine(LPIMAGE lpImage, LPOBJECT lpObject,
						int x, int y, int dx,
						LPTR lpLine, LPTR lpSlime, int depth)
/************************************************************************/
{
	int Slime1, Slime2, Slime3, Slime4;
	int iCount = dx;
#ifdef C_CODE
	int m1, m2;
#endif
	switch(Control.MaskTint)
	{
		case 0 : // RED
			Slime1 = 255;
			Slime2 = 0;
			Slime3 = 0;
			Slime4 = 0;
		break;

		case 1 : // GREEN
			Slime1 = 0;
			Slime2 = 255;
			Slime3 = 0;
			Slime4 = 0;
		break;

		case 2 : // BLUE
			Slime1 = 0;
			Slime2 = 0;
			Slime3 = 255;
			Slime4 = 0;
		break;

		case 3 : // CYAN
			Slime1 = 0;
			Slime2 = 255;
			Slime3 = 255;
			Slime4 = 0;
		break;

		case 4 : // MAGENTA
			Slime1 = 255;
			Slime2 = 0;
			Slime3 = 255;
			Slime4 = 0;
		break;

		case 5 : // YELLOW
			Slime1 = 255;
			Slime2 = 255;
			Slime3 = 0;
			Slime4 = 0;
		break;
	}

	if (lpObject == ImgGetDisplayObject(lpImage))
		MaskLoad( lpObject->lpAlpha, x, y, dx, lpSlime);
	else
		ImgGetLine(lpImage, lpObject, x, y, dx, lpSlime);

	switch(depth)
	{
		case 0 :
		case 1 :
			while (iCount-- > 0)
			{
				// slime off pixels
				if (*lpSlime++ <= 127)
					*lpLine = (*lpLine + Slime1) >> 1;
				++lpLine;
			}
		break;

		case 3 :
#ifdef C_CODE
			while (iCount-- > 0)
			{
				m1 = ((*lpSlime++)/2)+128;
				m2 = 255 - m1;
				*lpLine++ = ((*lpLine*m1) + (Slime1*m2)) >> 8;
				*lpLine++ = ((*lpLine*m1) + (Slime2*m2)) >> 8;
				*lpLine++ = ((*lpLine*m1) + (Slime3*m2)) >> 8;
			}
#else // !C_CODE
			__asm {
				push ds
				push es

//			s24begin:
				lds si, lpSlime ; Slime Data pointer
				les	di, lpLine	; Scanline Data Pointer
				mov	cx, iCount  ; Pixel Count

			s24l1:
				xor ax, ax      ; Clear AX
				lodsb			; Get Slime value -> AL
				cmp	al, 255		; Check Hi Value
				je	s24zero		; No Slime here...
				cmp	al, 0		; Check our slime
				jnz	s24grad		; No special, so slime it

//			s24full:
				xor ax, ax
				mov al, es:[di]   ; Get Source Red value
				add ax, Slime1	  ; Add Red Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di], al	  ; Put back Red value

				xor ax, ax
				mov al, es:[di+1] ; Get Source Green value
				add ax, Slime2	  ; Add Green Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di+1], al ; Put back Green value

				xor ax, ax
				mov al, es:[di+2] ; Get Source Blue value
				add ax, Slime3	  ; Add Blue Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di+2], al ; Put back Blue value

			s24zero:
				add	di, 3
				dec	cx
				jnz s24l1
				jmp	s24done

			s24grad:
				mov	dl, al		  ; Put Slime value -> DL
				shr dl, 1		  ; Divide by 2
				add dl, 128		  ; Add 128 for m1
				mov dh, 255		  ;
				sub	dh, dl		  ; m2 = 255 - m1

				xor ax, ax		  ; Clear AX
				mov al, es:[di]   ; Get Source Red value   -> AL
				mul	dl			  ; Multiply Red * m1      -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime1    ; Get Red Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di], ah   ; Save new Red Value

				xor ax, ax		  ; Clear AX
				mov al, es:[di+1] ; Get Source Green value -> AL
				mul	dl			  ; Multiply Green * m1    -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime2    ; Get Green Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di+1], ah ; Save new Green Value

				xor ax, ax		  ; Clear AX
				mov al, es:[di+2] ; Get Source Blue value  -> AL
				mul	dl			  ; Multiply Blue * m1     -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime3    ; Get Blue Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di+2], ah ; Save new Green Value

				add	di, 3
				dec	cx
				jz	s24done
				jmp	s24l1

			s24done:
				pop	es
				pop	ds
			}
#endif // !C_CODE
		break;

		case 4 :
			Slime1 ^= 0xFF;
			Slime2 ^= 0xFF;
			Slime3 ^= 0xFF;
#ifdef C_CODE
			while (iCount-- > 0)
			{
				m1 = ((*lpSlime++)/2)+128;
				m2 = (255 - m1);
				*lpLine++ = ((*lpLine*m1) + (Slime1*m2)) >> 8;
				*lpLine++ = ((*lpLine*m1) + (Slime2*m2)) >> 8;
				*lpLine++ = ((*lpLine*m1) + (Slime3*m2)) >> 8;
				*lpLine++ = ((*lpLine*m1) + (Slime4*m2)) >> 8;
			}
#else // !C_CODE
			__asm {
				push ds
				push es

//			s32begin:
				lds si, lpSlime ; Slime Data pointer
				les	di, lpLine	; Scanline Data Pointer
				mov	cx, iCount  ; Pixel Count

			s32l1:
				xor ax, ax      ; Clear AX
				lodsb			; Get Slime value -> AL
				cmp	al, 255		; Check Hi Value
				je	s32zero		; No Slime here...
				cmp	al, 0		; Check our slime
				jnz	s32grad		; No special, so slime it

//			s32full:
				xor ax, ax
				mov al, es:[di]   ; Get Source Yellow value
				add ax, Slime1	  ; Add Yellow Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di], al	  ; Put back Yellow value

				xor ax, ax
				mov al, es:[di+1] ; Get Source Magenta value
				add ax, Slime2	  ; Add Magenta Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di+1], al ; Put back Magenta value

				xor ax, ax
				mov al, es:[di+2] ; Get Source Cyan value
				add ax, Slime3	  ; Add Cyan Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di+2], al ; Put back Cyan value

				xor ax, ax
				mov al, es:[di+3] ; Get Source Black value
				add ax, Slime4	  ; Add Black Slime Value
				shr ax, 1		  ; Divide by 2
				mov	es:[di+3], al ; Put back Black value

			s32zero:
				add	di, 4
				dec	cx
				jnz s32l1
				jmp	s32done

			s32grad:
				mov	dl, al		  ; Put Slime value -> DL
				shr dl, 1		  ; Divide by 2
				add dl, 128		  ; Add 128 for m1
				mov dh, 255		  ;
				sub	dh, dl		  ; m2 = 255 - m1

				xor ax, ax		  ; Clear AX
				mov al, es:[di]   ; Get Source Yellow value   -> AL
				mul	dl			  ; Multiply Yellow * m1      -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime1    ; Get Yellow Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di], ah   ; Save new Yellow Value

				xor ax, ax		  ; Clear AX
				mov al, es:[di+1] ; Get Source Magenta value -> AL
				mul	dl			  ; Multiply Magenta * m1    -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime2    ; Get Magenta Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di+1], ah ; Save new Magenta Value

				xor ax, ax		  ; Clear AX
				mov al, es:[di+2] ; Get Source Cyan value  -> AL
				mul	dl			  ; Multiply Cyan * m1     -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime3    ; Get Cyan Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di+2], ah ; Save new Cyan Value

				xor ax, ax		  ; Clear AX
				mov al, es:[di+3] ; Get Source Black value  -> AL
				mul	dl			  ; Multiply Black * m1     -> AX
				mov bx, ax		  ; Temp storage
				mov	ax, Slime4    ; Get Black Slime Value
				mul dh			  ; Multiply Slime *m2     -> AX
				add ax, bx        ; Add values -> AX
				mov es:[di+3], ah ; Save new Black Value

				add	di, 4
				dec	cx
				jz	s32done
				jmp	s32l1

			s32done:
				pop	es
				pop	ds
			}
#endif // !C_CODE
		break;
	}
}

/************************************************************************/
static WORD ImgGetNextGroupID(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
WORD wGroupID = 0;

while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (lpObject->wGroupID > wGroupID)
		wGroupID = lpObject->wGroupID;
	}
return(wGroupID+1);
}

/************************************************************************/
LPOBJECT ImgCreateClipInObject(LPIMAGE lpImage,BOOL fBuildMask)
/************************************************************************/
{
int bpp, depth, xAlpha, yAlpha, xData, yData;
LPOBJECT lpObject;
LPFRAME lpDataFrame, lpFrame;
LPMASK lpAlpha;
RECT rObject, rDisplay;

if ( !lpImage )
	return( NULL );
if ( !(lpFrame = ImgGetBaseEditFrame(lpImage)) )
	return( NULL );

if (Names.PasteMaskFile[0] && FileExists(Names.PasteMaskFile))
	ProgressBegin(2, 0);
else
	ProgressBegin(1, 0);

depth = FrameDepth(lpFrame);
if ( !(lpDataFrame = AstralFrameLoad( Names.PasteImageFile, depth, &bpp, NULL )))
	{
	ProgressEnd();
	if (FileExists(Names.PasteImageFile))
		Message ( IDS_EMASKREAD, (LPTR)Names.PasteImageFile );
	else
		Message(IDS_EIMAGEOPEN, (LPTR)Names.PasteImageFile);
	return( NULL );
	}

xData = FrameXSize(lpDataFrame);
yData = FrameYSize(lpDataFrame);

// Read in clipboard mask or use a newly created (empty) mask
if (fBuildMask)
{
    LPFRAME lpMaskFrame = MaskCreateAlphaFrame( lpDataFrame );
    if (lpMaskFrame)
        lpAlpha = MaskCreate( lpMaskFrame, 0, 0, ON, YES/*fNoUndo*/ );

    if ( !lpAlpha )
    {
		ProgressEnd();
        Message(IDS_EMEMALLOC);
		FrameClose(lpDataFrame);
		return( NULL );
    }
}
else if ( !Names.PasteMaskFile[0] ||
          !(lpAlpha = ReadMask( Names.PasteMaskFile, xData, yData )) )
	{
	if ( !(lpAlpha = MaskCreate( NULL, xData, yData, ON, YES/*fNoUndo*/ )) )
		{
		ProgressEnd();
		Message(IDS_EMASKREAD, (LPTR)Names.PasteMaskFile);
		FrameClose(lpDataFrame);
		return( NULL );
		}
	}

xAlpha = FrameXSize(lpAlpha->Pixmap.EditFrame);
yAlpha = FrameYSize(lpAlpha->Pixmap.EditFrame);

if ( xAlpha != xData || yAlpha != yData ||
	FrameDepth(lpAlpha->Pixmap.EditFrame) != 1 )
	{
	ProgressEnd();
	Message(IDS_EBADCLIP);
	MaskClose(lpAlpha);
	FrameClose(lpDataFrame);
	return( NULL );
	}

rDisplay = lpImage->lpDisplay->FileRect;
rObject.left = ( rDisplay.left + rDisplay.right - xData ) / 2;
rObject.top  = ( rDisplay.top + rDisplay.bottom - yData ) / 2;
if (rObject.left < rDisplay.left)
	rObject.left = rDisplay.left;
if (rObject.top < rDisplay.top)
	rObject.top = rDisplay.top;
rObject.right = rObject.left + xData - 1;
rObject.bottom = rObject.top + yData - 1;

if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, lpDataFrame, lpAlpha,
	&rObject, Control.NoUndo )) )
	{
	ProgressEnd();
	Message(IDS_EMEMALLOC);
	MaskClose( lpAlpha );
	FrameClose(lpDataFrame);
	return( NULL );
	}

ImgAddNewObject(lpImage, lpObject);
ProgressEnd();
return( lpObject );
}

/************************************************************************/
BOOL ImgWriteClipOut(LPIMAGE lpImage, LPSTR lpFileName, LPSTR lpMaskName,
					LPRECT lpRect, int DataType )
/************************************************************************/
{
FNAME szFileName, szMaskName;
LPFRAME lpDataFrame, lpAlphaFrame;
LPALPHA lpAlpha;
LPOBJECT lpObject;
LPTR lpAlphaBuf;
RECT rClip;
BOOL bSaveNames;
int iWidth, iHeight, y;

if ( !lpFileName )
	{
	bSaveNames = YES;
	GetExtNamePath( szFileName, IDN_CLIPBOARD );
	lstrcat( szFileName, "WINCLIP.TIF" );
	lpFileName = szFileName;
	GetExtNamePath( szMaskName, IDN_MASK );
	lstrcat( szMaskName, "WINMASK.TIF" );
	lpMaskName = szMaskName;
	}
else
	bSaveNames = NO;

if (ImgGetMaskRect(lpImage, &rClip))
	{ // There is a mask...
	ProgressBegin(lpMaskName ? 2 : 1, 0);
	if (lpMaskName)
		{
		lpAlpha = ImgGetMask( lpImage );
		if ( !AstralFrameSave( lpMaskName, lpAlpha->Pixmap.EditFrame,
			&rClip, IDC_SAVECT, IDN_TIFF, NO))
			{
			ProgressEnd();
			return( NO );
			}
		}
	if ( !ImgMultipleObjects( lpImage ) )
		{ // Mask with only one object (the base)
		if ( !AstralFrameSave( lpFileName, ImgGetBaseEditFrame(lpImage),
			&rClip, DataType, IDN_TIFF, NO))
			{
			if (lpMaskName)
				FileDelete( lpMaskName );
			ProgressEnd();
			return( NO );
			}
		}
	else
		{ // Mask with multiple objects
		if (!(lpDataFrame = ImgCopyFrame(lpImage, NULL, &rClip, YES, NO)))
			{
			ProgressEnd();
			Message(IDS_EMEMALLOC);
			if (lpMaskName)
				FileDelete( lpMaskName );
			return( NO );
			}
		if ( !AstralFrameSave( lpFileName, lpDataFrame,
			NULL, DataType, IDN_TIFF, NO))
			{
			ProgressEnd();
			FrameClose( lpDataFrame );
			if (lpMaskName)
				FileDelete( lpMaskName );
			return( NO );
			}
		FrameClose( lpDataFrame );
		}
	}
else
	{ // No mask...
	ImgGetSelObjectRect(lpImage, &rClip, NO);
	if ( ImgCountSelObjects( lpImage, NULL ) == 1 &&
		 ImgGetSelObject(lpImage, NULL) == ImgGetBase( lpImage ) )
		{ // Only the base is selected
		ProgressBegin(1, 0);
		if ( !AstralFrameSave( lpFileName, ImgGetBaseEditFrame(lpImage),
			NULL, DataType, IDN_TIFF, NO))
			{
			ProgressEnd();
			return( NO );
			}
		if (lpMaskName)
			lpMaskName[0] = '\0'; // No mask file will be created
		SetRectEmpty( &rClip );
		}
	else
		{ // Multiple objects are selected
		ProgressBegin(lpMaskName ? 2 : 1, 0);
		if (!(lpDataFrame = ImgCopyFrame(lpImage, NULL, &rClip, YES, YES)))
			{
			ProgressEnd();
			Message(IDS_EMEMALLOC);
			return( NO );
			}
		if ( !AstralFrameSave( lpFileName, lpDataFrame,
			NULL, DataType, IDN_TIFF, NO))
			{
			ProgressEnd();
			FrameClose( lpDataFrame );
			return( NO );
			}
		FrameClose( lpDataFrame );
		if (lpMaskName)
			{
			iWidth = RectWidth(&rClip);
			iHeight = RectHeight(&rClip);
			if ( !(lpAlphaFrame = FrameOpen(FDT_GRAYSCALE, iWidth, iHeight, 75 )) )
				{
				ProgressEnd();
				Message(IDS_EMEMALLOC);
				FileDelete( lpFileName );
				return( NO );
				}
			if (!(lpAlphaBuf = Alloc((long)iWidth)))
				{
				ProgressEnd();
				Message(IDS_EMEMALLOC);
				FrameClose(lpAlphaFrame);
				FileDelete( lpFileName );
				return( NO );
				}
			lpObject = ImgGetSelObject(lpImage, NULL);
			for (y = 0; y < iHeight; ++y)
				{
				ImgMaskLoad(lpImage, lpObject, NULL, rClip.left, y+rClip.top,
					iWidth, lpAlphaBuf, YES, CR_OR, 0, 0);
				FrameWrite(lpAlphaFrame, 0, y, iWidth, lpAlphaBuf, iWidth);
				}
			FreeUp( lpAlphaBuf );
			if ( !AstralFrameSave( lpMaskName, lpAlphaFrame,
				NULL, IDC_SAVECT, IDN_TIFF, NO))
				{
				ProgressEnd();
				FrameClose(lpAlphaFrame);
				FileDelete( lpFileName );
				return( NO );
				}
			FrameClose(lpAlphaFrame);
			}
		}
	}

// Remember the locations of the files we have cut or copied
// and use them when we render; only set these strings here
if ( bSaveNames )
	{
	lstrcpy( Names.ClipImageFile, lpFileName );
	lstrcpy( Names.ClipMaskFile, lpMaskName );
	}

if ( lpRect )
	*lpRect = rClip; // Pass back the size of the cut object
ProgressEnd();
return( YES );
}

/************************************************************************/
LPOBJECT ImgCreateClipOutObject(LPIMAGE lpImage)
/************************************************************************/
{
LPFRAME lpDataFrame, lpAlphaFrame;
LPMASK lpMask;
LPALPHA lpAlpha;
LPOBJECT lpObject;
LPTR lpAlphaBuf;
RECT	rClip;
int iWidth, iHeight, y;

lpAlphaFrame = NULL;
if (ImgGetMaskRect(lpImage, &rClip))
	{
	if (!(lpDataFrame = ImgCopyFrame(lpImage, NULL, &rClip, YES, NO)))
		{
		Message(IDS_EMEMALLOC);
		return(NULL);
		}
	lpMask = ImgGetMask(lpImage);
	if (!(lpAlphaFrame = FrameCopy(lpMask->Pixmap.EditFrame, &rClip)))
		{
		FrameClose(lpDataFrame);
		Message(IDS_EMEMALLOC);
		return(NULL);
		}
	}
else
	{
	ImgGetSelObjectRect(lpImage, &rClip, NO);
	if (!(lpDataFrame = ImgCopyFrame(lpImage, NULL, &rClip, YES, YES)))
		{
		Message(IDS_EMEMALLOC);
		return(NULL);
		}
	if ( ImgCountSelObjects( lpImage, NULL ) != 1 ||
		 ImgGetSelObject(lpImage, NULL) != ImgGetBase( lpImage ) )
		{
		iWidth = RectWidth(&rClip);
		iHeight = RectHeight(&rClip);
		if (!(lpAlphaFrame = FrameOpen(FDT_GRAYSCALE, iWidth, iHeight,
							FrameResolution(lpDataFrame))))
			{
			FrameClose(lpDataFrame);
			Message(IDS_EMEMALLOC);
			return(NULL);
			}
		if (!(lpAlphaBuf = Alloc((long)iWidth)))
			{
			FrameClose(lpDataFrame);
			FrameClose(lpAlphaFrame);
			Message(IDS_EMEMALLOC);
			return(NULL);
			}
		lpObject = ImgGetSelObject(lpImage, NULL);
		for (y = 0; y < iHeight; ++y)
			{
			ImgMaskLoad(lpImage, lpObject, NULL, rClip.left, y+rClip.top,
					iWidth, lpAlphaBuf, YES, CR_OR, 0, 0);
			FrameWrite(lpAlphaFrame, 0, y, iWidth, lpAlphaBuf, iWidth);
			}
		FreeUp(lpAlphaBuf);
		}
	}
lpAlpha = NULL;
if (lpAlphaFrame)
	{
	if (!(lpAlpha = MaskCreate(lpAlphaFrame, 0, 0, NO, YES)))
		{
		FrameClose(lpDataFrame);
		FrameClose(lpAlphaFrame);
		Message(IDS_EMEMALLOC);
		return(NULL);
		}
	}
OffsetRect(&rClip, -rClip.left, -rClip.top);
if (!(lpObject = ObjCreateFromFrame(ST_PERMANENT, lpDataFrame, lpAlpha,
									&rClip, YES)))
	{
	FrameClose(lpDataFrame);
	if (lpAlpha)
		MaskClose(lpAlpha);
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
return(lpObject);
}


/************************************************************************/
BOOL ImgDuplicateSelObjects(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpDupObject;
OBJECTLIST DupList;
LPOBJECT lpObject;
RECT rUpdate;
WORD wGroupID, wOldGroupID, wLastGroupID;

if (!ImgEditInit( lpImage, ET_OBJECT, UT_DELETEOBJECTS, ImgGetBase(lpImage) ))
	return(FALSE);

DupList.lpHead = DupList.lpTail = NULL;
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (!(lpDupObject = ObjDuplicateObject(lpObject)))
		{
		ObjDeleteObjects(&DupList, ST_ALL);
		return(FALSE);
		}
	ObjAddTail(&DupList, (LPPRIMOBJECT)lpDupObject);
	}
wLastGroupID = wGroupID = ImgGetNextGroupID(lpImage);
lpDupObject = NULL;
while (lpDupObject = (LPOBJECT)ObjGetNextObject(&DupList,
					(LPPRIMOBJECT)lpDupObject, YES))
	{
	if (!lpDupObject->wGroupID || lpDupObject->wGroupID >= wLastGroupID)
		continue;
	wOldGroupID = lpDupObject->wGroupID;
	lpDupObject->wGroupID = wGroupID;
	lpObject = lpDupObject;
	while (lpObject = (LPOBJECT)ObjGetNextObject(&DupList,
					(LPPRIMOBJECT)lpObject, YES))
		{
		if (lpObject->wGroupID == wOldGroupID)
			lpObject->wGroupID = wGroupID;
		}
	++wGroupID;
	}

RemoveObjectMarquee(lpImage);
ObjDeselectAll(&lpImage->ObjList);
while (lpObject = (LPOBJECT)ObjGetNextObject(&DupList, NULL, YES))
	{
	ObjUnlinkObject(&DupList, (LPPRIMOBJECT)lpObject);
	ObjSelectObject( (LPPRIMOBJECT)lpObject, YES ); // Select the new object
	ObjAddTail( &lpImage->ObjList, (LPPRIMOBJECT)lpObject ); // Add to the image's list
	lpObject->fUndoDeleted = YES;
	}
ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDOCHANGE, NULL);

ImgGetSelObjectRect(lpImage, &rUpdate, NO);
UpdateImage(&rUpdate, YES);
SetupMiniViews(NULL, NO);
return(TRUE);
}



/************************************************************************/
LPOBJECT ImgFindObjectRect(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
					BOOL fExcludeBase)
/************************************************************************/
{
POINT pt;
LPOBJECT lpObject = NULL;
LPOBJECT lpBase;

if (!lpImage)
	return(NULL);
pt.x = x;
pt.y = y;
lpBase = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, fBottomUp, NO))
	{
	if (fExcludeBase && lpObject == lpBase)
		continue;
	if (lpObject->ObjectDataID != OBJECT_DATA_STOCK_ANIMATION)
		continue;
	if (PtInRect(&lpObject->rObject, pt))
		{
		return(lpObject);
		}
	}
return(NULL);
}



/************************************************************************/
int FAR EXPORT AnimateProc(HWND hWindow, LPARAM lParam, UINT msg)
/************************************************************************/
{
// This animate proc will play any animation object that is clicked on.
// Use: SetWindowLong(hCtl, GWL_IMAGE_TOOLPROC, (long)AnimateProc),
//      or call this proc if your proc doesn't handle the WM_LBUTTONDOWN message.
// Returns TRUE if it handles the message.

    int         x, y;
    LPOBJECT    lpObject;
	BOOL		bHandled = FALSE;

    x = LOWORD(lParam);
    y = HIWORD(lParam);
    switch (msg)
    {
        case WM_CREATE:	// The first mouse down message
    	case WM_LBUTTONDOWN:
            lpObject = ImgFindObjectRect(lpImage, x, y, TRUE, TRUE);
    		if (lpObject && !lpObject->fHidden)
    		{	
                if (lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
                {
    				bHandled = TRUE;			 
					ImgPlayAnimationInWindow(lpImage, hWindow, lpObject);
                }
            }
    		break;
    }
    return bHandled;
}




void ImgPlayAnimationInWindow(LPIMAGE lpImage, HWND hWindow, LPOBJECT lpObject)
{
// Play this single animation in the specified window without yeilding.
	STRING 		szString;
	FNAME 		szFileName;
	RECT		rUpdate;
	HINSTANCE 	hToy = 0;
	int 		hAObject = 0;
	LPAOBJECT 	Asession = NULL; // The session handle
	if (!lpObject) return;
	
	wsprintf( szString, "%04d", (int)(WORD)lpObject->dwObjectData );
    if ( GetToyFileName( szString, szFileName ) )
    {
    	if (!(Asession = ACreateNewAnimator()))
    		return;

    	if ((hToy = LoadLibrary(szFileName)) < (HINSTANCE)HINSTANCE_ERROR)
    		return;
			
		hAObject = LoadToy(
			Asession, 
			NULL, 
			1, 
			lpObject->rObject.left, 
			lpObject->rObject.top, 
			hToy);

		rUpdate = lpObject->rObject;
		InflateRect(&rUpdate, 2, 2);	// cover the entire object
        lpObject->fHidden = TRUE;
        lpObject->fChanged = TRUE;
		InvalidateRect(hWindow, &rUpdate, TRUE);
		UpdateWindow(hWindow); 
        AnimateToy (hWindow, lpObject->rObject.left,
                    lpObject->rObject.top,szFileName,TRUE,
                    hToy, hAObject, Asession);
        lpObject->fHidden = FALSE;
		InvalidateRect(hWindow, &rUpdate, TRUE);
		UpdateWindow(hWindow);
    }
}



