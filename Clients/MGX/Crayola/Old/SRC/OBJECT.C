//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "routines.h"

static void ObjInitPrimObject(
						LPPRIMOBJECT lpObject,
						OBJECT_TYPE ObjectType,
						RENDER_TYPE RenderType,
						STORAGE_TYPE StorageType,
						LPRECT lpRect);
static void ObjInitObject(
							LPOBJECT lpObject,
							LPFRAME lpFrame,
							LPMASK lpAlpha,
							BOOL NoUndo);

/************************************************************************/
LPOBJECT CALLBACK EXPORT ObjCreateFromFrame(STORAGE_TYPE StorageType, LPFRAME lpFrame,
					LPALPHA lpAlpha, LPRECT lpRect, BOOL NoUndo)
/************************************************************************/
{
LPOBJECT lpObject;
RECT rObject;

if (!(lpObject = (LPOBJECT)Alloc(sizeof(OBJECT))))
	return(NULL);
if (lpRect)
	rObject = *lpRect;
else
	{
	if ( !lpFrame )
		return( NULL );
	rObject.left = rObject.top = 0;
	rObject.right = FrameXSize(lpFrame) - 1;
	rObject.bottom = FrameYSize(lpFrame) - 1;
	}
ObjInitPrimObject((LPPRIMOBJECT)lpObject, OT_FRAME, RT_FRAME, StorageType, &rObject);
ObjInitObject(lpObject, lpFrame, lpAlpha, NoUndo);
return(lpObject);
}

/************************************************************************/
LPOBJECT ObjCreateFromPixmap(STORAGE_TYPE StorageType, LPPIXMAP lpPixmap,
						LPMASK lpAlpha, LPRECT lpRect, BOOL NoUndo)
/************************************************************************/
{
LPOBJECT lpObject;
RECT rObject;

if (!(lpObject = (LPOBJECT)Alloc(sizeof(OBJECT))))
	return(NULL);
if (lpRect)
	rObject = *lpRect;
else
	{
	if ( !lpPixmap->EditFrame )
		return( NULL );
	rObject.left = rObject.top = 0;
	rObject.right = FrameXSize(lpPixmap->EditFrame) - 1;
	rObject.bottom = FrameYSize(lpPixmap->EditFrame) - 1;
	}
ObjInitPrimObject((LPPRIMOBJECT)lpObject, OT_FRAME, RT_FRAME, StorageType, &rObject);
ObjInitObject(lpObject, NULL, lpAlpha, NoUndo);
lpObject->Pixmap = *lpPixmap;
return(lpObject);
}

/************************************************************************/
void ObjFreeUp(LPOBJECT lpObject)
/************************************************************************/
{
if (!lpObject)
	return;
if ( lpObject->ObjectType == OT_FRAME )
	{
	PixmapFree(&lpObject->Pixmap);
	if (lpObject->lpAlpha)
		MaskClose(lpObject->lpAlpha);
	}
FreeUp((LPTR)lpObject);
}

/************************************************************************/
LPMASK ObjGetAlpha(LPOBJECT lpObject)
/************************************************************************/
{
if (!lpObject)
	return(NULL);
return(lpObject->lpAlpha);
}

/************************************************************************/
void ObjSetAlpha(LPOBJECT lpObject, LPMASK lpAlpha)
/************************************************************************/
{
if (lpObject)
	lpObject->lpAlpha = lpAlpha;
}

/************************************************************************/
LPFRAME ObjGetEditFrame(LPOBJECT lpObject)
/************************************************************************/
{
return(lpObject->Pixmap.EditFrame);
}

/************************************************************************/
LPFRAME ObjGetUndoFrame(LPOBJECT lpObject)
/************************************************************************/
{
return(lpObject->Pixmap.UndoFrame);
}

/************************************************************************/
LPOBJECT ObjDuplicateObject(LPOBJECT lpObject)
/************************************************************************/
{
LPOBJECT lpDupObject;
LPFRAME lpDataFrame;
LPFRAME lpAlphaFrame = NULL;
LPALPHA lpAlpha;

if (!lpObject)
	return(NULL);

if (!(lpDataFrame = FrameCopy(ObjGetEditFrame(lpObject), NULL)))
	return(NULL);
if (lpAlpha = ObjGetAlpha(lpObject))
	if (!(lpAlphaFrame = FrameCopy(lpAlpha->Pixmap.EditFrame, NULL)))
		{
		FrameClose(lpDataFrame);
		return(NULL);
		}
if (!(lpAlpha = MaskCreate(lpAlphaFrame,
						FrameXSize(lpDataFrame),
						FrameYSize(lpDataFrame),
						ON, Control.NoUndo)))
	{
	FrameClose(lpDataFrame);
	return(NULL);
	}
if (!(lpDupObject = ObjCreateFromFrame(lpObject->StorageType, lpDataFrame,
							lpAlpha, &lpObject->rObject, Control.NoUndo)))
	{
	FrameClose(lpDataFrame);
	MaskClose(lpAlpha);
	return(NULL);
	}
lpDupObject->Opacity = lpObject->Opacity;
lpDupObject->MergeMode = lpObject->MergeMode;
lpDupObject->wGroupID = lpObject->wGroupID;
return(lpDupObject);
}

/************************************************************************/
void ObjSelectObject(LPPRIMOBJECT lpObject, BOOL fSelect)
/************************************************************************/
{
lpObject->fSelected = fSelect;
}

/************************************************************************/
void ObjAddTail(LPOBJECTLIST lpObjList, LPPRIMOBJECT lpObject)
/************************************************************************/
{
if (lpObjList->lpTail)
	{
	// link the new object in
	ObjLinkObject(lpObjList, lpObjList->lpTail, lpObject, YES);
	}
else
	{
	lpObjList->lpHead = lpObject;
	lpObjList->lpTail = lpObject;
	}
}

/************************************************************************/
void ObjDeleteObjects(LPOBJECTLIST lpObjList, STORAGE_TYPE StorageType)
/************************************************************************/
{
LPPRIMOBJECT lpObject, lpNextObject;

lpNextObject = lpObjList->lpHead;
while (lpNextObject)
	{
	lpObject = lpNextObject;
	lpNextObject = lpNextObject->lpNext;
	if (StorageType == ST_ALL || lpObject->StorageType == StorageType)
		ObjDeleteObject(lpObjList, lpObject);
	}
}

/************************************************************************/
BOOL ObjDeleteObject(LPOBJECTLIST lpObjList, LPPRIMOBJECT lpObject)
/************************************************************************/
{
LPPRIMOBJECT lpNextObject;

lpNextObject = lpObjList->lpHead;
while (lpNextObject && lpNextObject != lpObject)
	lpNextObject = lpNextObject->lpNext;
if (lpNextObject) // found?
	{
	ObjUnlinkObject(lpObjList, lpObject);
	ObjFreeUp((LPOBJECT)lpObject);
	return(TRUE);
	}
else
	return(FALSE);
}

/************************************************************************/
int ObjCountObjects(LPOBJECTLIST lpObjList)
/************************************************************************/
{
LPPRIMOBJECT lpObject;
int	nCount = 0;

lpObject = lpObjList->lpHead;
while (lpObject)
	{
	++nCount;
	lpObject = lpObject->lpNext;
	}
return(nCount);
}

/************************************************************************/
BOOL ObjSelectObjects(LPOBJECTLIST lpObjList, LPRECT lpRect, BOOL fSelect,
				BOOL fBottomUp, BOOL fMultiSelect)
/************************************************************************/
{
LPPRIMOBJECT	lpObject;
BOOL 		fHaveRect, fSelected;

if (fBottomUp)
	lpObject = lpObjList->lpHead;
else
	lpObject = lpObjList->lpTail;
if (lpRect)
	fHaveRect = lpRect->left < lpRect->right;
fSelected = FALSE;
while (lpObject)
	{
	if (!lpRect)
		{
		lpObject->fSelected = fSelect;
		fSelected = TRUE;
		if (!fMultiSelect)
			break;
		}
	else if (fHaveRect)
		{
		if (lpObject->rObject.left >= lpRect->left &&
			lpObject->rObject.right <= lpRect->right &&
			lpObject->rObject.top >= lpRect->top &&
			lpObject->rObject.bottom <= lpRect->bottom)
			{
			lpObject->fSelected = fSelect;
			fSelected = TRUE;
			if (!fMultiSelect)
				break;
			}
		}
	else
		{
		if (PtInRect(&lpObject->rObject, *((LPPOINT)lpRect)))
			{
			lpObject->fSelected = fSelect;
			fSelected = TRUE;
			if (!fMultiSelect)
				break;
			}
		}
	if (fBottomUp)
		lpObject = lpObject->lpNext;
	else
		lpObject = lpObject->lpPrev;
	}
return(fSelected);
}

/************************************************************************/
BOOL ObjDeselectAll(LPOBJECTLIST lpObjList)
/************************************************************************/
{
LPPRIMOBJECT lpObject;

lpObject = lpObjList->lpHead;
while (lpObject)
	{
	lpObject->fSelected = NO;
	lpObject = lpObject->lpNext;
	}
return(YES);
}

/************************************************************************/
LPPRIMOBJECT ObjGetSelectedObject(LPOBJECTLIST lpObjList, LPPRIMOBJECT lpObject,
							BOOL fBottomUp)
/************************************************************************/
{
lpObject = ObjGetNextObject(lpObjList, lpObject, fBottomUp);
while (lpObject && !lpObject->fSelected)
	lpObject = ObjGetNextObject(lpObjList, lpObject, fBottomUp);
return(lpObject);
}

/************************************************************************/
LPPRIMOBJECT ObjGetNextObject(LPOBJECTLIST lpObjList, LPPRIMOBJECT lpObject,
							BOOL fBottomUp)
/************************************************************************/
{
if (lpObject)
	{
	if (fBottomUp)
		lpObject = lpObject->lpNext;
	else
		lpObject = lpObject->lpPrev;
	}
else
	{
	if (fBottomUp)
		lpObject = lpObjList->lpHead;
	else
		lpObject = lpObjList->lpTail;
	}
return(lpObject);
}

/************************************************************************/
void CALLBACK EXPORT ObjUnlinkObject(LPOBJECTLIST lpObjList,
									LPPRIMOBJECT lpObject)
/************************************************************************/
{
// if a next object in list, link to this object's previous
if (lpObject->lpNext)
	lpObject->lpNext->lpPrev = lpObject->lpPrev;
// if a previous object in list, link to this object's next
if (lpObject->lpPrev)
	lpObject->lpPrev->lpNext = lpObject->lpNext;
// fix head and tail pointers if necessary
if (lpObjList->lpHead == lpObject)
	lpObjList->lpHead = lpObject->lpNext;
if (lpObjList->lpTail == lpObject)
	lpObjList->lpTail = lpObject->lpPrev;
lpObject->lpPrev = NULL;
lpObject->lpNext = NULL;
}

/************************************************************************/
void ObjLinkObject(LPOBJECTLIST lpObjList, LPPRIMOBJECT lpLinkObject,
					LPPRIMOBJECT lpObject, BOOL fAfter)
/************************************************************************/
{
if (fAfter)
	{
	lpObject->lpNext = lpLinkObject->lpNext;
	lpObject->lpPrev = lpLinkObject;
	lpLinkObject->lpNext = lpObject;
	if (lpObject->lpNext)
		lpObject->lpNext->lpPrev = lpObject;
	else
		lpObjList->lpTail = lpObject;
	}
else
	{
	lpObject->lpNext = lpLinkObject;
	lpObject->lpPrev = lpLinkObject->lpPrev;
	lpLinkObject->lpPrev = lpObject;
	if (lpObject->lpPrev)
		lpObject->lpPrev->lpNext = lpObject;
	else
		lpObjList->lpHead = lpObject;
	}
}

/************************************************************************/
static void ObjInitPrimObject(LPPRIMOBJECT lpObject,
						OBJECT_TYPE ObjectType,
						RENDER_TYPE RenderType,
						STORAGE_TYPE StorageType,
						LPRECT lpRect)
/************************************************************************/
{
lpObject->lpNext = NULL;
lpObject->lpPrev = NULL;
lpObject->ObjectType = ObjectType;
lpObject->RenderType = RenderType;
lpObject->StorageType = StorageType;
lpObject->rObject = *lpRect;
lpObject->Opacity = 255;
lpObject->MergeMode = MM_NORMAL;
lpObject->fSelected = NO;
lpObject->fChanged = NO;
lpObject->fDeleted = NO;
lpObject->fHidden  = NO;
lpObject->wGroupID = 0;
lpObject->ObjectDataID = OBJECT_DATA_NONE;
lpObject->dwObjectData = 0;
}

/************************************************************************/
static void ObjInitObject(LPOBJECT lpObject,
							LPFRAME lpFrame,
							LPMASK lpAlpha,
							BOOL NoUndo)
/************************************************************************/
{
PixmapSetup(&lpObject->Pixmap, lpFrame, NoUndo);
lpObject->lpAlpha = lpAlpha;

lpObject->DataUndoType = 0;
lpObject->DataUndoNotRedo = YES;
lpObject->DataDirty = NO;
lpObject->AlphaUndoType = 0;
lpObject->AlphaUndoNotRedo = YES;
lpObject->AlphaDirty = NO;
lpObject->fBothDirty = NO;

lpObject->lpUndoAlpha = NULL;
AstralSetRectEmpty(&lpObject->rUndoObject);
lpObject->UndoResolution = 0;
lpObject->fUndoSelected = NO;

lpObject->fTransform = NO;
AstralSetRectEmpty(&lpObject->rTransform);
lpObject->fPasteIntoMask = NO;
lpObject->fUndoDeleted = NO;
lpObject->fUndoFailed = NoUndo;
lpObject->InitUndoType = 0;
}


