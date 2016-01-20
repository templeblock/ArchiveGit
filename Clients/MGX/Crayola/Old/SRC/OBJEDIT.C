//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	OBJEDIT.C - Object editing setup functions

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "routines.h"
#include "id.h"

static BOOL ObjEditDiscardUndoCheck( LPOBJECT lpObject, BOOL fCheckData, BOOL fCheckAlpha);
static BOOL ObjEditInit(LPIMAGE lpImage, LPOBJECT lpObject, UNDO_TYPE UndoType);
static void ObjEditUndo(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fUndoData, BOOL fUndoAlpha);
static void ObjEditFree(LPOBJECT lpObject);
static void ObjEditFreeUndo( LPOBJECT lpObject, BOOL fFreeData, BOOL fFreeAlpha );
static BOOL ObjEditUndoDiscardable( LPOBJECT lpObject, BOOL fCheckData, BOOL fCheckAlpha );

extern HINSTANCE hInstAstral;

/***********************************************************************/
BOOL ImgEditInit( LPIMAGE lpImage, EDIT_TARGET Target, UNDO_TYPE UndoType,
				LPOBJECT lpTargetObject)
/***********************************************************************/
{
LPOBJECT lpObject, lpBase;
int retc;
BOOL fDiscardUndo = NO;
BOOL fUndoFailed;
BOOL fMaskEdit;

lpBase = ImgGetBase(lpImage);

lpObject = NULL;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	lpObject->InitUndoType = 0;

// No mask undo's are allowed - so apply any that we have now
if (!Control.UndoMasks)
	ObjEditApply(lpImage, lpBase, NO, YES, NO);

// No undo's are allowed - so apply any that we have now
if (Control.NoUndo)
	{
	lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		ObjEditApply(lpImage, lpObject, YES, lpObject != lpBase, NO);
	}

if (!Control.MultipleObjects)
	UndoType &= ~UT_DELETEOBJECTS;

fMaskEdit = (Target == ET_OBJECT) && (lpTargetObject == lpBase) &&
			((UndoType & UT_ALPHAUNDO) != 0);

// see if any objects cannot have their undos freed without
// asking the user first, if so ask the user and get the response
// This is only a manual apply thing
if (UndoType & (UT_NEWDATA|UT_NEWALPHA) && Control.UseApply)
	{
	lpObject = NULL;
	fDiscardUndo = YES;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
		{
		// see if this is an object we are not editing
		if ((Target == ET_SELOBJECTS &&
			!ImgIsSelectedObject(lpImage, lpObject)) ||
			(Target == ET_OBJECT && lpObject != lpTargetObject))
			continue;
		if (!ObjEditUndoDiscardable(lpObject, (UndoType & UT_NEWDATA) != 0,
											(UndoType & UT_NEWALPHA) != 0))
			{
			retc = AstralConfirm( IDS_OKTOAPPLY );
			if (retc == IDCANCEL)
				return(FALSE);
			fDiscardUndo = retc == IDYES;
			break;
			}
		}
	}

// now loop through the objects initializing their undos
lpObject = NULL;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	// see if this is an object we are not editing
	if ((Target == ET_SELOBJECTS && !ImgIsSelectedObject(lpImage, lpObject)) ||
		(Target == ET_OBJECT && lpObject != lpTargetObject))
		{
		// if no individual undo's, wack any undo buffers
		// if in auto apply or in manual apply and state is redo
		// Objects that are not the base need to also have
		// their alphas wacked if this is not a mask edit 
		if (!Control.UndoObjects && (!Control.UseApply ||
				( Control.UseApply && !lpObject->DataUndoNotRedo ) ) )
			ObjEditApply(lpImage, lpObject, !fMaskEdit,
 				(lpObject != lpBase) && !fMaskEdit, NO);
		continue;
		}

	// Discard it's undo if possible
	if (fDiscardUndo)
		ObjEditFreeUndo(lpObject, (UndoType & UT_NEWDATA) != 0,
						(UndoType & UT_NEWALPHA) != 0);

	// save state of undo failed flag
	fUndoFailed = lpObject->fUndoFailed;

	// Initialize the undo for this object
	if (!ObjEditInit(lpImage, lpObject, UndoType))
		{
		// Undo initialization failed, so apply all objects that
		// were going to be initialized
		lpObject = NULL;
		while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
			{
			// see if this is an object we are not editing
			if ((Target == ET_SELOBJECTS &&
				!ImgIsSelectedObject(lpImage, lpObject)) ||
				(Target == ET_OBJECT && lpObject != lpTargetObject))
				{
				continue;
				}
			lpObject->fUndoFailed = YES;
			lpObject->InitUndoType = 0;
			if (lpObject == lpBase)
				ObjEditApply(lpImage, lpObject,
							(UndoType & UT_DATAUNDO) != 0,
							(UndoType & UT_ALPHAUNDO) != 0, NO);
			else
				ObjEditApply(lpImage, lpObject, YES, YES, NO);
			}
		// if we have failed before
		if (fUndoFailed)
			return(TRUE);
		else
			{
			Message(IDS_NOUNDO);
			return(FALSE);
			}
		}
	lpObject->fUndoFailed = NO;
	lpObject->InitUndoType = UndoType;
	}
return( TRUE );
}

/************************************************************************/
void ImgEditApplyAll( LPIMAGE lpImage, BOOL fApplyData, BOOL fApplyAlpha )
/************************************************************************/
{
LPOBJECT lpObject;

lpObject = NULL;
while ( lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO) )
	ObjEditApply( lpImage, lpObject, fApplyData, fApplyAlpha, NO );
}

/************************************************************************/
void ImgEditApply( LPIMAGE lpImage )
/************************************************************************/
{
LPOBJECT lpObject;

lpObject = NULL;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (Control.UndoObjects && !ImgIsSelectedObject(lpImage, lpObject))
		continue;
	ObjEditApply( lpImage, lpObject, YES, YES, NO );
	}
}

/***********************************************************************/
void ImgEditUndo( LPIMAGE lpImage, BOOL fEditUndo, BOOL fMaskUndo )
/***********************************************************************/
{
RECT rUndo, rUndoMask, rTemp;
LPOBJECT lpBase, lpObject, lpNext;
BOOL fNewSize, fSetupMiniViews, fRemoveMarquee, fUndoDelete;
BOOL fUndoAlpha, fUndoData;
int nDeleted;
LPMASK lpMask;

if (!lpImage)
	return;

// Initialize things for updating the display
fNewSize = fSetupMiniViews = fUndoDelete = NO;
AstralSetRectEmpty(&rUndo);
AstralSetRectEmpty(&rUndoMask);

// loop through objects doing undo
lpBase = ImgGetBase(lpImage);
lpObject = NULL;
while (lpObject = ImgGetNextObjectEx(lpImage, lpObject, NO, NO, fEditUndo))
	{
	// See if this is an object we are undoing
	if (fEditUndo)
		{
		// check for special undo processing of a deleted object
		if (lpObject->fDeleted) 
			{
			if (Control.UndoObjects && !ImgIsSelectedObject(lpImage, lpBase))
				continue;
			}
		else
		if (Control.UndoObjects && !ImgIsSelectedObject(lpImage, lpObject))
			continue;
		}
	// Only handle mask undo for the base object
	if (fMaskUndo)
		{
		if (lpObject != lpBase || lpObject->fBothDirty ||
			!lpObject->AlphaDirty)
		continue;
		}

	fUndoData = fUndoAlpha = NO;

	// Do preprocess for doing a data undo
	if (fEditUndo)
		{
		if (lpObject->DataDirty)
			fUndoData = YES;
		// See if we need to undo the alpha for this object
		if ((lpObject != lpBase) && lpObject->lpAlpha && lpObject->AlphaDirty)
			fUndoAlpha = YES;
		if (!fUndoData && !fUndoAlpha)
			continue;
	
		// check to see and undoing/redoing deleted objects will change
		// the select state, if so undraw the object marquee
		if (lpObject == lpBase &&
			lpObject->DataUndoType & UT_DELETEOBJECTS)
			{
			fUndoDelete = YES;
			fRemoveMarquee = NO;
			nDeleted = 0;
			lpNext = lpObject;
			while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, YES))
				{
				if (lpNext->fUndoDeleted)
					++nDeleted;
				else
				if (lpNext->fDeleted && lpNext->fSelected)
					{
					fRemoveMarquee = YES;
					break;
					}
				}
			if (ImgGetSelObject(lpImage, NULL) == lpBase && !fRemoveMarquee)
				{
				if (ImgCountObjects(lpImage) - nDeleted <= 1)
					fRemoveMarquee = YES;
				}
			if (fRemoveMarquee)
				RemoveObjectMarqueeEx(lpImage, NO);
			}
		}
	else // fMaskUndo
	if (!lpObject->AlphaDirty)
		continue;
	else
		fUndoAlpha = YES;

	// do a preprocess for undoing the mask caused either by
	// a mask undo or by an edit function that also edits the mask
	if (((lpObject == lpBase) && fEditUndo && lpObject->fBothDirty) ||
		fMaskUndo)
		{
		if ( lpObject->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
			{
			// if the undo is going to delete the mask,
			// we need to undraw the mask
			if (ImgGetMask(lpImage))
				{
				RemoveMaskMarquee(lpImage);
				if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rTemp))
					AstralUnionRect(&rUndoMask, &rUndoMask, &rTemp);
				}
			// if the undo is going to create the mask,
			// we need to undraw the object marquees if
			// not in mask and object marquee mode
			else
				{
				if (!Control.UseMaskAndObjects)
 					RemoveObjectMarqueeEx(lpImage, NO);
				}
			}
		}

	// Actually do the undo
	ObjEditUndo(lpImage, lpObject, fUndoData, fUndoAlpha);

	// do a postprocess for undoing the mask
	if ((lpMask = ImgGetMask(lpImage)) &&
		(((lpObject == lpBase) && fEditUndo && lpObject->fBothDirty) ||
		fMaskUndo))
		{
		// if the undo is going to add the mask, we need to redraw the mask
		if (lpObject->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
			{
			if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rTemp))
				AstralUnionRect(&rUndoMask, &rUndoMask, &rTemp);
			}
		else
			// just redraw the undo area for the mask
			AstralUnionRect(&rUndoMask, &rUndoMask, &lpMask->Pixmap.UndoRect);
		}

	// Setup rectangle for undoing deletion of objects
	// Handled specially so that moved objects will still undo
	// and redo properly
	if (fEditUndo)
		{
		if (lpObject == lpBase &&
			lpObject->DataUndoType & UT_DELETEOBJECTS)
			{
			lpNext = lpObject;
			while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, YES))
				{
				if (lpNext->fDeleted || lpNext->fUndoDeleted)
					AstralUnionRect(&rUndo, &rUndo, &lpNext->rObject);
				}
			fSetupMiniViews = YES;
			}

		if (lpObject->Pixmap.fNewFrame)
			{
			/* if new frame, cause window to be redisplayed */
			if (lpObject == lpBase)
				fNewSize = YES;
			else
				{
				if (!fNewSize)
					{
					AstralUnionRect(&rUndo, &rUndo, &lpObject->rObject);
					AstralUnionRect(&rUndo, &rUndo, &lpObject->rUndoObject);
					}
				}
			}
		else
			{
			AstralSetRectEmpty(&rTemp);
			if (fUndoData)
				AstralUnionRect(&rTemp, &rTemp, &lpObject->Pixmap.UndoRect);
			if (fUndoAlpha)
				AstralUnionRect(&rTemp, &rTemp,
						&lpObject->lpAlpha->Pixmap.UndoRect);
			if (rTemp.right >= rTemp.left)
				{
				if (!fNewSize)
					{
					OffsetRect(&rTemp, lpObject->rObject.left,
							lpObject->rObject.top);
					AstralUnionRect(&rUndo, &rUndo, &rTemp);
					}
				}
			}
		}
	}
// now redisplay whatever changes are necessary for the undo
if (fNewSize)
	{
	SetupImagePalette(lpImage, NO, YES);
	if (lpBase->Pixmap.UndoFrame)
		{
		if ((FrameXSize(lpBase->Pixmap.UndoFrame) ==
			FrameXSize(lpBase->Pixmap.EditFrame)) &&
			(FrameYSize(lpBase->Pixmap.UndoFrame) ==
			FrameYSize(lpBase->Pixmap.EditFrame)))
			fNewSize = NO;
		}
	if (fNewSize)
		UpdateImageSize();
	else
		UpdateImageEx(lpImage, NULL, YES, YES, YES);
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	}
else
	{
	if (rUndoMask.right >= rUndoMask.left)
		{
		File2DispRect(&rUndoMask, &rUndoMask);
		AstralToWindowsRect(&rUndoMask);
		InvalidateRect(lpImage->hWnd, NULL, NO);
		}
	if (rUndo.right >= rUndo.left)
		AnimateUndo( &rUndo );
	}
if (fUndoDelete)
	if ( Tool.hRibbon )
		SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
	
if (fSetupMiniViews)
	SetupMiniViews(NULL, NO);
}

/***********************************************************************/
static void AnimateUndo( LPRECT lpUndoRect )
/***********************************************************************/
{
RECT rect;
STRING szString;
ITEMID id1, id2;
int iHeight, iWidth, x1, x2, y;
#define DXANIMATION 71
#define DYANIMATION 76

SoundStartID( IDC_AMBULANCE1, YES/*bLoop*/, NULL/*hInstance*/ );
GetClientRect( lpImage->hWnd, &rect );
iHeight = RectHeight( &rect );
iWidth = RectWidth( &rect );
File2DispRect( lpUndoRect, &rect );
x1 = (rect.left + rect.right) / 2;
if ( x1 < iWidth - x1 )
	{ // closer to the left; come in from the left
	x1 = -DXANIMATION;
	x2 = rect.right;
	id1 = IDC_AMBULANCE2;
	id2 = IDC_AMBULANCE1;
	}
else
	{ // closer to the right; come in from the right
	x1 = iWidth - 1;
	x2 = rect.left - DXANIMATION;
	id1 = IDC_AMBULANCE1;
	id2 = IDC_AMBULANCE2;
	}
y = (rect.top + rect.bottom - DYANIMATION) / 2;
y = bound( y, 0, iHeight - DYANIMATION );

wsprintf( szString, "SETSPEED 35\rJUMP %d,%d\rMOVE %d,%d\r", x1, y, x2, y );
AnimateDibID( lpImage->hWnd, NULL/*&rect*/, hInstAstral, id1, NULL, szString );

UpdateImage( lpUndoRect, YES );
AstralUpdateWindow( lpImage->hWnd );

wsprintf( szString, "SETSPEED 35\rJUMP %d,%d\rMOVE %d,%d\r", x2, y, x1, y );
AnimateDibID( lpImage->hWnd, NULL/*&rect*/, hInstAstral, id2, NULL, szString );
SoundStop();
}


/***********************************************************************/
void ImgEditMaskUndo( LPIMAGE lpImage )
/***********************************************************************/
{
LPOBJECT lpBase;
LPMASK lpMask;
RECT rUndo;

lpBase = ImgGetBase(lpImage);
if (!lpBase->AlphaDirty)
	return;
if (lpBase->fBothDirty)
	return;

AstralSetRectEmpty(&rUndo);
if (ImgGetMask(lpImage) &&
	lpBase->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
	{
	RemoveMaskMarquee(lpImage);
	ImgGetMaskUpdateRect(lpImage, YES, NO, &rUndo);
	}

ObjEditUndo(lpImage, lpBase, NO, YES);

if (lpMask = ImgGetMask(lpImage))
	{
	if (lpBase->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
		{
		RemoveMaskMarquee(lpImage);
		ImgGetMaskUpdateRect(lpImage, YES, NO, &rUndo);
		}
	else
		rUndo = lpMask->Pixmap.UndoRect;
	}
if (rUndo.right >= rUndo.left)
	{
	OffsetRect(&rUndo, lpBase->rObject.left, lpBase->rObject.top);
	File2DispRect(&rUndo, &rUndo);
	AstralToWindowsRect(&rUndo);
	InvalidateRect(lpImage->hWnd, NULL, NO);
	}
}

/***********************************************************************/
BOOL ImgEditRecover( LPIMAGE lpImage )
/***********************************************************************/
{
LPOBJECT lpObject;

lpObject = NULL;
while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
	{
	}

return( TRUE );
}

/***********************************************************************/
void ImgEditedObject(LPIMAGE lpImage, LPOBJECT lpObject, int iDirty, LPRECT lpEditRect)
/***********************************************************************/
{
LPALPHA lpAlpha;
BOOL fEditedData, fEditedAlpha, fBase;

fBase = (lpObject == ImgGetBase(lpImage));
fEditedData = (lpObject->InitUndoType & UT_DATAUNDO) != 0;
fEditedAlpha = (lpObject->InitUndoType & UT_ALPHAUNDO) != 0;
if (lpEditRect)
	OffsetRect(lpEditRect, -lpObject->rObject.left, -lpObject->rObject.top);
lpAlpha = lpObject->lpAlpha;
if (fEditedData)
	{
	lpObject->fChanged = YES;
	if (!Control.NoUndo && !lpObject->fUndoFailed)
		{
		lpObject->DataDirty = iDirty;
		if (lpEditRect)
			PixmapAddUndoArea(&lpObject->Pixmap, lpEditRect);
		}
	}
if (fEditedAlpha)
	{
	if (lpObject != ImgGetBase(lpImage))
		lpObject->fChanged = YES;
	if (!Control.NoUndo && (!fBase || Control.UndoMasks) &&
		!lpObject->fUndoFailed)
		{
		lpObject->AlphaDirty = iDirty;
		if (lpEditRect && lpAlpha)
			PixmapAddUndoArea(&lpAlpha->Pixmap, lpEditRect);
		}
	}
if (!Control.NoUndo && !lpObject->fUndoFailed && !lpObject->fBothDirty)
	lpObject->fBothDirty = fEditedData && fEditedAlpha;
if (lpEditRect)
	OffsetRect(lpEditRect, lpObject->rObject.left, lpObject->rObject.top);
}

/***********************************************************************/
void ImgEditedObjectFrame(LPIMAGE lpImage, LPOBJECT lpObject, int iDirty,
					LPRECT lpEditRect,
					LPFRAME lpDataFrame, LPFRAME lpAlphaFrame)
/***********************************************************************/
{
LPALPHA lpAlpha;
BOOL fEditedData, fEditedAlpha, fBase;

fBase = (lpObject == ImgGetBase(lpImage));
fEditedData = (lpObject->InitUndoType & UT_DATAUNDO) != 0;
fEditedAlpha = (lpObject->InitUndoType & UT_ALPHAUNDO) != 0;
if (lpEditRect)
	OffsetRect(lpEditRect, -lpObject->rObject.left, -lpObject->rObject.top);
lpAlpha = lpObject->lpAlpha;
if (fEditedData)
	{
	lpObject->fChanged = YES;
	if (!Control.NoUndo && !lpObject->fUndoFailed)
		lpObject->DataDirty = iDirty;
	if (lpDataFrame)
		{
		PixmapNewFrame(&lpObject->Pixmap, lpDataFrame, lpEditRect, Control.UseApply);
		if (lpObject == ImgGetBase(lpImage))
			{
			lpImage->npix = FrameXSize(lpDataFrame);
			lpImage->nlin = FrameYSize(lpDataFrame);
			}
		}
	}
if (fEditedAlpha)
	{
	if (lpObject != ImgGetBase(lpImage))
		lpObject->fChanged = YES;
	if (!Control.NoUndo && (!fBase || Control.UndoMasks) &&
		!lpObject->fUndoFailed)
		lpObject->AlphaDirty = iDirty;
	if (lpAlpha && lpAlphaFrame)
		PixmapNewFrame(&lpAlpha->Pixmap, lpAlphaFrame, lpEditRect, Control.UseApply);
	}
if (!Control.NoUndo && !lpObject->fUndoFailed && !lpObject->fBothDirty)
	lpObject->fBothDirty = fEditedData && fEditedAlpha;
if (lpEditRect)
	OffsetRect(lpEditRect, lpObject->rObject.left, lpObject->rObject.top);
}

/***********************************************************************/
static BOOL ObjEditInit(LPIMAGE lpImage, LPOBJECT lpObject, UNDO_TYPE UndoType)
/***********************************************************************/
{
LPALPHA lpAlpha;
LPOBJECT lpNext;
BOOL fBase, fDataInit, fAlphaInit;

// See if this objects the base, and get alpha channel of object
fBase = (lpObject == ImgGetBase(lpImage));
lpAlpha = lpObject->lpAlpha;

//
// HANDLE INITIALIZATION OF OBJECT DATA
//

fDataInit = (UndoType & UT_DATAUNDO) != 0;
if (fDataInit && !Control.NoUndo)
	{
	// Apply any changes that have been made if in auto apply mode
	// or if in manual apply mode and changes have been undone
	// the init flag is set only if we are doing an edit in place change
	// to the data so we get rid of any useless undo's
	if (!Control.UseApply || ( Control.UseApply && !lpObject->DataUndoNotRedo ) )
		ObjEditApply(lpImage, lpObject, YES, !fBase, (UndoType & UT_DATA) != 0);

	// Save undo information for the specific type of operation
	if (UndoType & UT_OBJECTRECT && !(lpObject->DataUndoType & UT_OBJECTRECT))
		lpObject->rUndoObject = lpObject->rObject;
	if (UndoType & UT_RESOLUTION && !(lpObject->DataUndoType & UT_RESOLUTION))
		lpObject->UndoResolution = FrameResolution(ObjGetEditFrame(lpObject));
	if (UndoType & UT_SELECTION && !(lpObject->DataUndoType & UT_SELECTION))
		lpObject->fUndoSelected = lpObject->fSelected;
	// Size base is a special case for the base and base only
	if (UndoType & UT_DATA)
		{
		// init the data pixmap for editing
		if (!lpObject->Pixmap.UndoFrame)
			if (!PixmapInitUndo(&lpObject->Pixmap))
				return(FALSE);
		}
	if (UndoType & UT_NEWDATA)
		{
		if (fBase)
			lpImage->UndoDataType = lpImage->DataType;
		}
	if (fBase && UndoType & UT_MOVEOBJECTS &&
		!(lpObject->DataUndoType & UT_MOVEOBJECTS))
		{
		lpNext = lpObject;
		while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, NO))
			lpNext->rUndoObject = lpNext->rObject;
		}
	// Delete objects is a special case for the base and base only
	if (fBase && UndoType & UT_DELETEOBJECTS &&
		!(lpObject->DataUndoType & UT_DELETEOBJECTS))
		{
		lpNext = lpObject;
		while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, YES))
			lpNext->fUndoDeleted = NO;
		}
	}

// now that we have successfully initialized everything set UndoTypes
if (fDataInit)
	{
	lpObject->DataUndoNotRedo = YES;
	lpObject->DataUndoType |= (UndoType & UT_DATAUNDO);
	}

//
// HANDLE INITIALIZATION OF OBJECT ALPHA
//
fAlphaInit = (UndoType & UT_ALPHAUNDO) != 0;
if (fAlphaInit && !Control.NoUndo && (!fBase || Control.UndoMasks))
	{
	// Apply any changes that have been made if in auto apply mode
	// or if in manual apply mode and changes have been undone or
	// apply any changes only to the mask
	// or if it is a change to both data and mask and
	// both were not dirty before
	// Initialize the data undo if the object is not the base and we didn't
	// already do a data init
	// the init flag is set only if we are doing an edit in place change
	// to the alpha so we get rid of any useless undo's
	if (!Control.UseApply ||
		(Control.UseApply && !lpObject->AlphaUndoNotRedo) ||
		(fBase && (!fDataInit || !lpObject->fBothDirty)))
		ObjEditApply(lpImage, lpObject, fDataInit ? NO : !fBase, YES,
					(UndoType & UT_ALPHA) != 0);

	// Save undo information for the specific type of operation
	if (lpAlpha)
		{
		if (!lpObject->AlphaUndoType)
			{
			if (UndoType & UT_ALPHA)
				{
				// init the data pixmap for editing
				if (!lpAlpha->Pixmap.UndoFrame)
					if (!PixmapInitUndo(&lpAlpha->Pixmap))
						return(FALSE);
				lpAlpha->rUndoMask = lpAlpha->rMask;
				}
			if (UndoType & UT_NEWALPHA)
				lpAlpha->rUndoMask = lpAlpha->rMask;
			if (fBase && UndoType & UT_CREATEMASK)
				lpObject->lpUndoAlpha = NULL;
			if (fBase && UndoType & UT_DELETEMASK)
				lpObject->lpUndoAlpha = lpAlpha;
			if (UndoType & UT_ALPHAUNDO)
				{
				lpObject->AlphaUndoNotRedo = YES;
				lpObject->AlphaUndoType |= (UndoType & UT_ALPHAUNDO);
				}
			}
		}
	}

return(TRUE);
}

/***********************************************************************/
static void ObjEditUndo(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fUndoData, BOOL fUndoAlpha)
/***********************************************************************/
{
RECT rTemp;
BOOL fSelected, Resolution, fDeleted, fBase;
LPALPHA lpAlpha, lpTemp;
LPOBJECT lpNext;
int XSize, YSize;
int tempDataType;

lpAlpha = lpObject->lpAlpha;
fBase = lpObject == ImgGetBase(lpImage);
if (fUndoData && lpObject->DataDirty)
	{
	if (lpObject->DataUndoType & UT_RESOLUTION)
		Resolution = FrameResolution(lpObject->Pixmap.EditFrame);
	if (lpObject->DataUndoType & (UT_DATA|UT_NEWDATA))
		{
		PixmapEditUndo(&lpObject->Pixmap);
		if (fBase)
			{
			lpImage->npix = FrameXSize(ObjGetEditFrame(lpObject));
			lpImage->nlin = FrameYSize(ObjGetEditFrame(lpObject));
			if (lpObject->DataUndoType & UT_NEWDATA)
				{
				tempDataType = lpImage->DataType;
				lpImage->DataType = lpImage->UndoDataType;
				lpImage->UndoDataType = tempDataType;
				}
			}
		}
	if (lpObject->DataUndoType & UT_OBJECTRECT)
		{
		rTemp = lpObject->rObject;
		lpObject->rObject = lpObject->rUndoObject;
		lpObject->rUndoObject = rTemp;
		}
	if (lpObject->DataUndoType & UT_RESOLUTION)
		{
		FrameSetResolution( lpObject->Pixmap.EditFrame,
							lpObject->UndoResolution);
		lpObject->UndoResolution = Resolution;
		}
	if (lpObject->DataUndoType & UT_SELECTION)
		{
		fSelected = lpObject->fSelected;
		lpObject->fSelected = lpObject->fUndoSelected;
		lpObject->fUndoSelected = fSelected;
		}
	if (lpObject->DataUndoType & UT_DELETEOBJECTS)
		{
		lpNext = lpObject;
		while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, YES))
			{
			if (lpNext->fUndoDeleted || lpNext->fDeleted)
				{
				fDeleted = lpNext->fDeleted;
				lpNext->fDeleted = lpNext->fUndoDeleted;
				lpNext->fUndoDeleted = fDeleted;
				}
			}
		}
	if (fBase && lpObject->DataUndoType & UT_MOVEOBJECTS)
		{
		lpNext = lpObject;
		while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, NO))
			{
			rTemp = lpNext->rObject;
			lpNext->rObject = lpNext->rUndoObject;
			lpNext->rUndoObject = rTemp;
			}
		}
	lpObject->DataUndoNotRedo = !lpObject->DataUndoNotRedo;
	if (!fUndoAlpha)
		fUndoAlpha = lpObject->fBothDirty;
	}
else if (fUndoAlpha && lpObject->fBothDirty)
	fUndoAlpha = NO;

if (fUndoAlpha && lpObject->AlphaDirty)
	{
	if (lpAlpha)
		{
		if (lpObject->AlphaUndoType & (UT_ALPHA|UT_NEWALPHA))
			{
			PixmapEditUndo(&lpAlpha->Pixmap);
			rTemp = lpAlpha->rMask;
			lpAlpha->rMask = lpAlpha->rUndoMask;
			lpAlpha->rUndoMask = rTemp;
			}
		}
	if (lpObject->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK))
		{
		lpTemp = lpObject->lpAlpha;
		lpObject->lpAlpha = lpObject->lpUndoAlpha;
		lpObject->lpUndoAlpha = lpTemp;
		}
	lpObject->AlphaUndoNotRedo = !lpObject->AlphaUndoNotRedo;
	}
lpAlpha = lpObject->lpAlpha;
// check to make sure the mask is still the same size
if (fUndoData && fBase && lpObject->Pixmap.fNewFrame)
	{
	// we just undid to a different size frame bag mask delete undo
	if (lpObject->lpUndoAlpha && !lpObject->fBothDirty)
		ObjEditApply(lpImage, lpObject, NO, YES, NO);
	if (lpAlpha)
		{
		XSize = FrameXSize(lpAlpha->Pixmap.EditFrame);
		YSize = FrameYSize(lpAlpha->Pixmap.EditFrame);
		if (XSize != FrameXSize(lpObject->Pixmap.EditFrame) ||
			YSize != FrameYSize(lpObject->Pixmap.EditFrame))
			{
			ObjEditInit(lpImage, lpObject, UT_DELETEMASK);
			lpObject->fBothDirty = YES;
			lpObject->AlphaDirty = lpObject->DataDirty;
			lpObject->lpAlpha = NULL;
			}
		}
	}
}

#ifdef UNUSED
/***********************************************************************/
void ObjEditUndo(LPOBJECT lpObject, BOOL fUndoData, BOOL fUndoAlpha)
/***********************************************************************/
{
RECT rTemp;
LPALPHA lpAlpha;

lpAlpha = lpObject->lpAlpha;
if (fUndoData)
	{
	fUndoAlpha = NO;
	if (lpObject->DataDirty)
		{
		PixmapEditUndo(&lpObject->Pixmap);
		rTemp = lpObject->rObject;
		lpObject->rObject = lpObject->rUndoObject;
		lpObject->rUndoObject = rTemp;
		fUndoAlpha = lpObject->fBothDirty && lpAlpha;
		}
	}
else if (fUndoAlpha && lpObject->fBothDirty)
	fUndoAlpha = NO;

if (fUndoAlpha && lpAlpha)
	{
	if (lpObject->AlphaDirty)
		{
		PixmapEditUndo(&lpAlpha->Pixmap);
		rTemp = lpAlpha->rMask;
		lpAlpha->rMask = lpAlpha->rUndoMask;
		lpAlpha->rUndoMask = rTemp;
		}
	}
}
#endif

/***********************************************************************/
void ObjEditApply(LPIMAGE lpImage, LPOBJECT lpObject,
				BOOL fApplyData, BOOL fApplyAlpha,
				BOOL fInitFrame)
/***********************************************************************/
{
LPALPHA lpAlpha;
LPOBJECT lpNext;
BOOL fInitingData, fInitingAlpha;

if (!fApplyData && !fApplyAlpha)
	return;

lpObject->InitUndoType = 0;
fInitingAlpha = fInitFrame && fApplyAlpha;
fInitingData = fInitFrame && fApplyData;
lpAlpha = lpObject->lpAlpha;
if (fApplyData)
	{
	if (lpObject->fBothDirty)
		fApplyAlpha = YES;
	if (lpObject == ImgGetBase(lpImage))
		{
		ImgPurgeObjects(lpImage);
		lpNext = lpObject;
		while (lpNext = ImgGetNextObjectEx(lpImage, lpNext, YES, NO, NO))
			lpNext->fUndoDeleted = NO;
		}
	if (!fInitingData)
		PixmapFreeUndo(&lpObject->Pixmap);
	else
		PixmapEditApply(&lpObject->Pixmap);
	lpObject->DataUndoType = 0;
	lpObject->DataDirty = NO;
	}
if (fApplyAlpha)
	{
	if (lpObject->lpUndoAlpha)
		{
		MaskClose(lpObject->lpUndoAlpha);
		lpObject->lpUndoAlpha = NULL;
		}
	if (lpAlpha)
		{
		if (!fInitingAlpha)
			PixmapFreeUndo(&lpAlpha->Pixmap);
		else
			PixmapEditApply(&lpAlpha->Pixmap);
		}
	lpObject->AlphaUndoType = 0;
	lpObject->AlphaDirty = NO;
	}
lpObject->fBothDirty = NO;
}

#ifdef UNUSED
/***********************************************************************/
void ObjEdited(LPOBJECT lpObject, int iDirty, BOOL fEditedData, BOOL fEditedAlpha,
				LPRECT lpEditRect)
/***********************************************************************/
{
LPALPHA lpAlpha;

if (lpEditRect)
	OffsetRect(lpEditRect, -lpObject->rObject.left, -lpObject->rObject.top);
lpAlpha = lpObject->lpAlpha;
if (fEditedData)
	{
	lpObject->fChanged = YES;
	lpObject->DataDirty = iDirty;
	if (lpEditRect)
		PixmapAddUndoArea(&lpObject->Pixmap, lpEditRect);
	}
if (fEditedAlpha && lpAlpha)
	{
	lpObject->AlphaDirty = iDirty;
	if (lpEditRect)
		PixmapAddUndoArea(&lpAlpha->Pixmap, lpEditRect);
	}
lpObject->fBothDirty = fEditedData && fEditedAlpha;
if (lpEditRect)
	OffsetRect(lpEditRect, lpObject->rObject.left, lpObject->rObject.top);
}

/***********************************************************************/
void ObjEditedFrame(LPOBJECT lpObject, int iDirty,
					BOOL fEditedData, BOOL fEditedAlpha,
					LPRECT lpEditRect,
					LPFRAME lpDataFrame, LPFRAME lpAlphaFrame)
/***********************************************************************/
{
LPALPHA lpAlpha;

if (lpEditRect)
	OffsetRect(lpEditRect, -lpObject->rObject.left, -lpObject->rObject.top);
lpAlpha = lpObject->lpAlpha;
if (fEditedData && lpDataFrame)
	{
	lpObject->fChanged = YES;
	lpObject->DataDirty = iDirty;
	PixmapNewFrame(&lpObject->Pixmap, lpDataFrame, lpEditRect, Control.UseApply);
	}
if (fEditedAlpha && lpAlpha && lpAlphaFrame)
	{
	lpObject->AlphaDirty = iDirty;
	PixmapNewFrame(&lpAlpha->Pixmap, lpAlphaFrame, lpEditRect, Control.UseApply);
	}
lpObject->fBothDirty = fEditedData && fEditedAlpha;
if (lpEditRect)
	OffsetRect(lpEditRect, lpObject->rObject.left, lpObject->rObject.top);
}
#endif

/***********************************************************************/
void ObjEditFree( LPOBJECT lpObject )
/***********************************************************************/
{
LPALPHA lpAlpha;

lpAlpha = lpObject->lpAlpha;
PixmapFree(&lpObject->Pixmap);
if (lpAlpha)
	PixmapFree(&lpAlpha->Pixmap);
if (lpObject->lpUndoAlpha)
	{
	MaskClose(lpObject->lpUndoAlpha);
	lpObject->lpUndoAlpha = NULL;
	}
}

/***********************************************************************/
void ObjEditFreeUndo( LPOBJECT lpObject, BOOL fFreeData, BOOL fFreeAlpha )
/***********************************************************************/
{
LPALPHA lpAlpha;

lpAlpha = lpObject->lpAlpha;
if (fFreeData)
	{
	PixmapFreeUndo(&lpObject->Pixmap);
	if (lpObject->fBothDirty && lpAlpha)
		PixmapFreeUndo(&lpAlpha->Pixmap);
	lpObject->DataDirty = NO;
	lpObject->fBothDirty = NO;
	}
if (fFreeAlpha && lpAlpha)
	{
	PixmapFreeUndo(&lpAlpha->Pixmap);
	if (lpObject->fBothDirty)
		{
		PixmapFreeUndo(&lpObject->Pixmap);
		lpObject->DataDirty = NO;
		}
	lpObject->AlphaDirty = NO;
	lpObject->fBothDirty = NO;
	}
if (lpObject->lpUndoAlpha)
	{
	MaskClose(lpObject->lpUndoAlpha);
	lpObject->lpUndoAlpha = NULL;
	}
}

/************************************************************************/
static BOOL ObjEditUndoDiscardable( LPOBJECT lpObject,
							BOOL fCheckData, BOOL fCheckAlpha )
/************************************************************************/
{
int dirty = 0;
LPALPHA lpAlpha;

if ( !lpObject )
	return(NO);
if (!(lpAlpha = lpObject->lpAlpha))
	fCheckAlpha = NO;
if ((!fCheckData || !lpObject->Pixmap.UndoFrame) &&
	(!fCheckAlpha || !lpAlpha->Pixmap.UndoFrame))
	return(YES);

if (Control.UseApply)
	{
	if (fCheckData && lpObject->DataUndoNotRedo)
		dirty |= lpObject->DataDirty;
	if (fCheckAlpha && lpAlpha && lpObject->AlphaUndoNotRedo)
		dirty |= lpObject->AlphaDirty;
	return(dirty == 0);
	}
else
	return(YES);
}

/************************************************************************/
static BOOL ObjEditDiscardUndoCheck( LPOBJECT lpObject,
					BOOL fCheckData, BOOL fCheckAlpha )
/************************************************************************/
{
LPALPHA lpAlpha;
int retc;

if ( !lpObject )
	return(FALSE);
if (!ObjEditUndoDiscardable(lpObject, fCheckData, fCheckAlpha))
	retc = AstralConfirm( IDS_OKTOAPPLY );
else
	retc = IDYES;
if (retc == IDYES)
	{
	lpAlpha = lpObject->lpAlpha;
	if (fCheckData)
		PixmapFreeUndo(&lpObject->Pixmap);
	if (fCheckAlpha && lpAlpha)
		PixmapFreeUndo(&lpAlpha->Pixmap);
	}
return( retc != IDCANCEL );
}

/************************************************************************/
void SetupMiniViews(HWND hDlg, BOOL fSelectChange)
/************************************************************************/
{
}

