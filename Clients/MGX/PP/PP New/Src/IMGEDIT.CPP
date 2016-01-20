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

#include "pp.h"
#include "id.h"
#include "text.h"

BOOL CImage::EditInit( EDIT_TARGET Target, UNDO_TYPE UndoType,
				LPOBJECT lpTargetObject)
{
LPOBJECT lpObject, lpBase;
int retc;
BOOL fDiscardUndo = NO;
BOOL fUndoFailed;
BOOL fMaskEdit;
EDIT_TARGET OldTarget;

lpBase = GetBase();

// the InitUndoType flag is kept separately from the
// DataUndoType and AlphaUndoType so that if the initialization
// of the undo fails we still know what was edited when
// EditedObject and EditedObjectFrame are called
lpObject = NULL;
while (lpObject = GetNextObject(lpObject, YES, NO))
	lpObject->InitUndoType = 0;

// No mask undo's are allowed - so apply any that we have now
if (!Control.UndoMasks)
	ObjEditApply(lpBase, NO, YES, NO);

// No undo's are allowed - so apply any that we have now
if (Control.NoUndo || fDisableUndos)
	{
	lpObject = NULL;
	while (lpObject = GetNextObject(lpObject, YES, NO))
		ObjEditApply(lpObject, YES, lpObject != lpBase, NO);
	}

// if we have don't have multiple objects, strip off the delete objects flag
if (!Control.MultipleObjects)
	UndoType &= ~UT_DELETEOBJECTS;

// find out whether we are in mask edit mode
fMaskEdit = (Target == ET_OBJECT) && (lpTargetObject == lpBase) &&
			((UndoType & UT_ALPHAUNDO) != 0);

// see if any objects cannot have their undos freed without
// asking the user first, if so ask the user and get the response
// This is only a manual apply thing
if (UndoType & (UT_NEWDATA|UT_NEWALPHA) && Control.UseApply)
	{
	lpObject = NULL;
	fDiscardUndo = YES;
	while (lpObject = GetNextObject(lpObject, YES, NO))
		{
		// see if this is an object we are not editing
		if ((Target == ET_SELOBJECTS &&
			!IsSelectedObject(lpObject)) ||
			(Target == ET_OBJECT && lpObject != lpTargetObject))
			continue;
		if (!ObjEditUndoDiscardable(lpObject, (UndoType & UT_NEWDATA) != 0,
											(UndoType & UT_NEWALPHA) != 0))
			{
			if (lpCmdList->fPlayback)
				{
				fDiscardUndo = fDiscardUndo;
				}
			else
				{
				DISCARDUNDO_PARMS parms;

				retc = AstralConfirm( IDS_OKTOAPPLY );
				if (retc == IDCANCEL)
					return(FALSE);
				fDiscardUndo = retc == IDYES;
				parms.fDiscardUndo = fDiscardUndo;

				PostCommand(lpCmdList, IDS_CMD_DISCARDUNDO, &parms);
				}
			break;
			}
		}
	}

OldTarget = EditTarget;
EditTarget = Target;

// now loop through the objects initializing their undos
lpObject = NULL;
while (lpObject = GetNextObject(lpObject, YES, NO))
	{
	// if our last edit involved editing all objects then
	// make sure we apply for all objects
	if (OldTarget == ET_ALLOBJECTS)
		{
		ObjEditApply(lpObject, !fMaskEdit,
 				(lpObject != lpBase) && !fMaskEdit, NO);
		}

	// see if this is an object we are not editing
	if ((Target == ET_SELOBJECTS && !IsSelectedObject(lpObject)) ||
		(Target == ET_OBJECT && lpObject != lpTargetObject))
		{
		// if no individual undo's, wack any undo buffers
		// if in auto apply or in manual apply and state is redo
		// Objects that are not the base need to also have
		// their alphas wacked if this is not a mask edit 
		if (!Control.UndoObjects && (!Control.UseApply ||
				( Control.UseApply && !lpObject->DataUndoNotRedo ) ) )
			ObjEditApply(lpObject, !fMaskEdit,
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
	if (!ObjEditInit(lpObject, UndoType))
		{
		// Undo initialization failed, so apply all objects that
		// were going to be initialized
		lpObject = NULL;
		while (lpObject = GetNextObject(lpObject, YES, NO))
			{
			// see if this is an object we are not editing
			if ((Target == ET_SELOBJECTS &&
				!IsSelectedObject(lpObject)) ||
				(Target == ET_OBJECT && lpObject != lpTargetObject))
				{
				continue;
				}
			lpObject->fUndoFailed = YES;
			lpObject->InitUndoType = 0;
			if (lpObject == lpBase)
				ObjEditApply(lpObject,
							(UndoType & UT_DATAUNDO) != 0,
							(UndoType & UT_ALPHAUNDO) != 0, NO);
			else
				ObjEditApply(lpObject, YES, YES, NO);
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

void CImage::EditApplyAll( BOOL fApplyData, BOOL fApplyAlpha )
{
LPOBJECT lpObject;

lpObject = NULL;
while ( lpObject = GetNextObject(lpObject, YES, NO) )
	ObjEditApply( lpObject, fApplyData, fApplyAlpha, NO );
}

void CImage::EditUndo( BOOL fEditUndo, BOOL fMaskUndo,
					LPUPDATE_TYPE lpUpdateType, LPRECT lpUpdateRect)
{
RECT rUndo, rUndoMask, rTemp;
LPOBJECT lpBase, lpObject, lpNext;
BOOL fNewSize, fSetupMiniViews, fRemoveMarquee, fUndoDelete, fColorMapChanged;
BOOL fUndoAlpha, fUndoData;
int nDeleted;
LPMASK lpMask;
EDIT_TARGET Target;

AstralSetRectEmpty(lpUpdateRect);

// Initialize things for updating the display
fNewSize = fSetupMiniViews = fUndoDelete = fColorMapChanged = NO;
AstralSetRectEmpty(&rUndo);
AstralSetRectEmpty(&rUndoMask);

// loop through objects doing undo
lpBase = GetBase();
lpObject = NULL;
// get the target of the last edit
Target = EditTarget;
while (lpObject = GetNextObject(lpObject, NO, NO, fEditUndo))
	{
	// See if this is an object we are undoing
	if (fEditUndo)
		{
		// check for special undo processing of a deleted object
		if (lpObject->fDeleted) 
			continue; // do no undo processing for a deleted object
//			{
//			if (Control.UndoObjects && !IsSelectedObject(lpBase))
//				continue;
//			}
		else
		// if target is not the entire image and we are in undo
		// object mode and the object is not selected then
		// skip this object
		if (Target != ET_ALLOBJECTS && Control.UndoObjects && !IsSelectedObject(lpObject))
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
			while (lpNext = GetNextObject(lpNext, YES, NO, YES))
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
			if (GetSelObject(NULL) == lpBase && !fRemoveMarquee)
				{
				if (CountObjects() - nDeleted <= 1)
					fRemoveMarquee = YES;
				}
			if (fRemoveMarquee)
				{
				GetObjectMarqueeRect(this, &rTemp);
				AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
				}
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
			if (GetMask())
				{
				GetMaskMarqueeRect(this, &rTemp);
				AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
				if (GetMaskUpdateRect(YES, NO, &rTemp))
					AstralUnionRect(&rUndoMask, &rUndoMask, &rTemp);
				}
			// if the undo is going to create the mask,
			// we need to undraw the object marquees if
			// not in mask and object marquee mode
			else
				{
				if (!Control.UseMaskAndObjects)
					{
					GetObjectMarqueeRect(this, &rTemp);
					AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
					}
				}
			}
		}

	// Actually do the undo
	ObjEditUndo(lpObject, fUndoData, fUndoAlpha);

	// do a postprocess for undoing the mask
	if ((lpMask = GetMask()) &&
		(((lpObject == lpBase) && fEditUndo && lpObject->fBothDirty) ||
		fMaskUndo))
		{
		// if the undo is going to add the mask, we need to redraw the mask
		if (lpObject->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
			{
			if (GetMaskUpdateRect(YES, NO, &rTemp))
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
			while (lpNext = GetNextObject(lpNext, YES, NO, YES))
				{
				if (lpNext->fDeleted || lpNext->fUndoDeleted)
					AstralUnionRect(&rUndo, &rUndo, &lpNext->rObject);
				}
			fSetupMiniViews = YES;
			}
		if (lpObject->DataUndoType & UT_COLORMAP)
			fColorMapChanged = YES;

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
			if( lpObject->DataUndoType & UT_OBJECTRECT )
				{
				AstralUnionRect( &rUndo, &rUndo,	&lpObject->rObject );
				AstralUnionRect( &rUndo, &rUndo,	&lpObject->rUndoObject );
				}
			}
		}
	}

// now redisplay whatever changes are necessary for the undo
if (fColorMapChanged)
	{
	ImgColorMapChanged(this);
	*lpUpdateType |= UT_DATATYPE;
	}

if (fNewSize)
	{
	*lpUpdateType |= UT_DATATYPE;
	if (lpBase->Pixmap.UndoFrame)
		{
		if ((FrameXSize(lpBase->Pixmap.UndoFrame) ==
			FrameXSize(lpBase->Pixmap.EditFrame)) &&
			(FrameYSize(lpBase->Pixmap.UndoFrame) ==
			FrameYSize(lpBase->Pixmap.EditFrame)))
			fNewSize = NO;
		}
	if (fNewSize)
		*lpUpdateType |= UT_SIZE;
	else
		{
		int dx, dy;

		GetInfo(&dx, &dy, NULL, NULL);
		SetRect(lpUpdateRect, 0, 0, dx-1, dy-1);
		}
	}
else
	{
	if (!AstralIsRectEmpty(lpUpdateRect))
		*lpUpdateType |= UT_ACTIVEAREA;
	if (rUndoMask.right >= rUndoMask.left)
		{
		*lpUpdateType |= UT_ACTIVEAREA;
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUndoMask);
		}
	if (rUndo.right >= rUndo.left)
		{
		*lpUpdateType |= UT_AREA;
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUndo);
		}
	}
}

void CImage::EditMaskUndo()
{
LPOBJECT lpBase;
LPMASK lpMask;
RECT rUndo;

lpBase = GetBase();
if (!lpBase->AlphaDirty)
	return;
if (lpBase->fBothDirty)
	return;

AstralSetRectEmpty(&rUndo);
if (GetMask() &&
	lpBase->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
	{
	RemoveMaskMarquee(this);
	GetMaskUpdateRect(YES, NO, &rUndo);
	}

ObjEditUndo(lpBase, NO, YES);

if (lpMask = GetMask())
	{
	if (lpBase->AlphaUndoType & (UT_CREATEMASK|UT_DELETEMASK) )
		{
		RemoveMaskMarquee(this);
		GetMaskUpdateRect(YES, NO, &rUndo);
		}
	else
		rUndo = lpMask->Pixmap.UndoRect;
	}
if (rUndo.right >= rUndo.left)
	{
	OffsetRect(&rUndo, lpBase->rObject.left, lpBase->rObject.top);
	File2DispRect(GetActiveDoc(), &rUndo, &rUndo);
	AstralToWindowsRect(&rUndo);
	InvalidateRect(GetActiveDoc(), NULL, NO);
	}
}

BOOL CImage::EditRecoverSelObj( BOOL fEditUndo, BOOL fMaskUndo)
{
	LPOBJECT lpObject;

	// if manual apply we can't do anything to recover
	if (Control.UseApply)
		return(FALSE);

	lpObject = NULL;
	while ( lpObject = GetSelObject(lpObject) )
	{
		if (fEditUndo)
			lpObject->DataDirty = TRUE;
		if (fMaskUndo)
			lpObject->AlphaDirty = TRUE;
		ObjEditUndo(lpObject, fEditUndo, fMaskUndo);
		ObjEditApply(lpObject, fEditUndo, fMaskUndo, FALSE);
	}
	return( TRUE );
}

BOOL CImage::EditRecoverObj( LPOBJECT lpObject,
					BOOL fEditUndo, BOOL fMaskUndo)
{
	// if manual apply we can't do anything to recover
	if (Control.UseApply)
		return(FALSE);

	if (fEditUndo)
		lpObject->DataDirty = TRUE;
	if (fMaskUndo)
		lpObject->AlphaDirty = TRUE;
	ObjEditUndo(lpObject, fEditUndo, fMaskUndo);
	ObjEditApply(lpObject, fEditUndo, fMaskUndo, FALSE);
	return( TRUE );
}

void CImage::EditedObject(LPOBJECT lpObject, int iDirty, LPRECT lpEditRect)
{
LPALPHA lpAlpha;
BOOL fEditedData, fEditedAlpha, fBase;

fBase = (lpObject == GetBase());
fEditedData = (lpObject->InitUndoType & UT_DATAUNDO) != 0;
fEditedAlpha = (lpObject->InitUndoType & UT_ALPHAUNDO) != 0;
if (lpEditRect)
	OffsetRect(lpEditRect, -lpObject->rObject.left, -lpObject->rObject.top);
lpAlpha = lpObject->lpAlpha;

	if (lpObject->lpText)
	   lpObject->lpText->Dirty = (iDirty == IDS_UNDOTEXT)? FALSE:TRUE;

if (fEditedData)
	{
    fChanged = YES;
	if (!(Control.NoUndo || fDisableUndos) && !lpObject->fUndoFailed)
		{
		lpObject->DataDirty = iDirty;
		if (lpEditRect)
			PixmapAddUndoArea(&lpObject->Pixmap, lpEditRect);
		}
	}
if (fEditedAlpha)
	{
	if (lpObject != GetBase())
        fChanged = YES;
	if (!(Control.NoUndo  || fDisableUndos) && (!fBase || Control.UndoMasks) &&
		!lpObject->fUndoFailed)
		{
		lpObject->AlphaDirty = iDirty;
		if (lpEditRect && lpAlpha)
			PixmapAddUndoArea(&lpAlpha->Pixmap, lpEditRect);
		}
	}
if (Control.NoUndo  || fDisableUndos)
	PurgeObjects();
if (!(Control.NoUndo  || fDisableUndos) && !lpObject->fUndoFailed && !lpObject->fBothDirty)
	lpObject->fBothDirty = fEditedData && fEditedAlpha;
if (lpEditRect)
	OffsetRect(lpEditRect, lpObject->rObject.left, lpObject->rObject.top);
}

void CImage::EditedObjectFrame(LPOBJECT lpObject, int iDirty,
					LPRECT lpEditRect,
					LPFRAME lpDataFrame, LPFRAME lpAlphaFrame)
{
LPALPHA lpAlpha;
BOOL fEditedData, fEditedAlpha, fBase;

fBase = (lpObject == GetBase());
fEditedData = (lpObject->InitUndoType & UT_DATAUNDO) != 0;
fEditedAlpha = (lpObject->InitUndoType & UT_ALPHAUNDO) != 0;
if (lpEditRect)
	OffsetRect(lpEditRect, -lpObject->rObject.left, -lpObject->rObject.top);
lpAlpha = lpObject->lpAlpha;

if (lpObject->lpText)
   lpObject->lpText->Dirty = (iDirty == IDS_UNDOEDITTEXT)? FALSE:TRUE;

if (fEditedData)
	{
    fChanged = YES;
	if (!(Control.NoUndo  || fDisableUndos) && !lpObject->fUndoFailed)
		lpObject->DataDirty = iDirty;
	if (lpDataFrame)
		{
		PixmapNewFrame(&lpObject->Pixmap, lpDataFrame, lpEditRect, Control.UseApply);
		if (lpObject == GetBase())
			{
			npix = FrameXSize(lpDataFrame);
			nlin = FrameYSize(lpDataFrame);
			}
		}
	}
if (fEditedAlpha)
	{
	if (lpObject != GetBase())
        fChanged = YES;
	if (!(Control.NoUndo  || fDisableUndos) && (!fBase || Control.UndoMasks) &&
		!lpObject->fUndoFailed)
		lpObject->AlphaDirty = iDirty;
	if (lpAlpha && lpAlphaFrame)
		PixmapNewFrame(&lpAlpha->Pixmap, lpAlphaFrame, lpEditRect, Control.UseApply);
	}
if (Control.NoUndo || fDisableUndos)
	PurgeObjects();
if (!(Control.NoUndo || fDisableUndos) && !lpObject->fUndoFailed && !lpObject->fBothDirty)
	lpObject->fBothDirty = fEditedData && fEditedAlpha;
if (lpEditRect)
	OffsetRect(lpEditRect, lpObject->rObject.left, lpObject->rObject.top);
}

BOOL CImage::ObjEditInit(LPOBJECT lpObject, UNDO_TYPE UndoType)
{
LPALPHA lpAlpha;
LPOBJECT lpNext;
BOOL fBase, fDataInit, fAlphaInit;
LPCOLORMAP lpColorMap;

// See if this objects the base, and get alpha channel of object
fBase = (lpObject == GetBase());
lpAlpha = lpObject->lpAlpha;

//
// HANDLE INITIALIZATION OF OBJECT DATA
//

fDataInit = (UndoType & UT_DATAUNDO) != 0;
if (fDataInit && !(Control.NoUndo || fDisableUndos))
	{
	// Apply any changes that have been made if in auto apply mode
	// or if in manual apply mode and changes have been undone
	// the init flag is set only if we are doing an edit in place change
	// to the data so we get rid of any useless undo's
	if (!Control.UseApply || ( Control.UseApply && !lpObject->DataUndoNotRedo ) )
		ObjEditApply(lpObject, YES, !fBase, (UndoType & UT_DATA) != 0);

	// Save undo information for the specific type of operation
 	if (UndoType & UT_UNDOTEXTDATA && !(lpObject->DataUndoType & UT_UNDOTEXTDATA))
		lpObject->lpUndoText = new CTextObject(*lpObject->lpText);
	if (UndoType & UT_OBJECTRECT && !(lpObject->DataUndoType & UT_OBJECTRECT))
		lpObject->rUndoObject = lpObject->rObject;
	if (UndoType & UT_RESOLUTION && !(lpObject->DataUndoType & UT_RESOLUTION))
		lpObject->UndoResolution = FrameResolution(ObjGetEditFrame(lpObject));
	if (UndoType & UT_COLORMAP && !(lpObject->DataUndoType & UT_COLORMAP))
		{
		if (lpColorMap = FrameGetColorMap(ObjGetEditFrame(lpObject)))
			lpObject->UndoColorMap = FrameCopyColorMap(lpColorMap);
		else
			lpObject->UndoColorMap = NULL;
		}
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
			UndoDataType = DataType;
		}
	if (fBase && UndoType & UT_MOVEOBJECTS &&
		!(lpObject->DataUndoType & UT_MOVEOBJECTS))
		{
		lpNext = lpObject;
		while (lpNext = GetNextObject(lpNext, YES, NO, NO))
			{
			ObjEditApply(lpNext, TRUE, TRUE, FALSE);
			lpNext->rUndoObject = lpNext->rObject;
			}
		}
	// Delete objects is a special case for the base and base only
	if (fBase && UndoType & UT_DELETEOBJECTS &&
		!(lpObject->DataUndoType & UT_DELETEOBJECTS))
		{
		lpNext = lpObject;
		while (lpNext = GetNextObject(lpNext, YES, NO, YES))
			lpNext->fUndoDeleted = NO;
		}
	if (UndoType & UT_GROUPOBJECTS && !(lpObject->DataUndoType & UT_GROUPOBJECTS))
		lpObject->UndoGroupID = lpObject->wGroupID;
	if (UndoType & UT_LOCKOBJECTS &&	!(lpObject->DataUndoType & UT_LOCKOBJECTS))
		lpObject->bUndoLocked = lpObject->bLocked;
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
if (fAlphaInit && !(Control.NoUndo  || fDisableUndos) && (!fBase || Control.UndoMasks))
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
		ObjEditApply(lpObject, fDataInit ? NO : !fBase, YES,
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

void CImage::ObjEditUndo(LPOBJECT lpObject, BOOL fUndoData, BOOL fUndoAlpha)
{
RECT rTemp;
BOOL fSelected, Resolution, fDeleted, fBase, bLocked;
LPALPHA lpAlpha, lpTemp;
LPOBJECT lpNext;
int XSize, YSize;
int tempDataType;
LPCOLORMAP lpColorMap;
int        GroupID;

lpAlpha = lpObject->lpAlpha;
fBase = lpObject == GetBase();
if (fUndoData && lpObject->DataDirty)
	{
	if (lpObject->DataUndoType & UT_RESOLUTION)
		Resolution = FrameResolution(lpObject->Pixmap.EditFrame);
	if (lpObject->DataUndoType & UT_COLORMAP)
		{
		if (lpColorMap = FrameGetColorMap(ObjGetEditFrame(lpObject)))
			lpColorMap = FrameCopyColorMap(lpColorMap);
		else
			lpColorMap = NULL;
		}
	if (lpObject->DataUndoType & (UT_DATA|UT_NEWDATA))
		{
		PixmapEditUndo(&lpObject->Pixmap);
		if (fBase)
			{
			npix = FrameXSize(ObjGetEditFrame(lpObject));
			nlin = FrameYSize(ObjGetEditFrame(lpObject));
			if (lpObject->DataUndoType & UT_NEWDATA)
				{
				tempDataType = DataType;
				DataType = UndoDataType;
				UndoDataType = tempDataType;
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
	if (lpObject->DataUndoType & UT_UNDOTEXTDATA)
		{
      	if (lpObject->lpUndoText)
      		{
      		CTextObject *pTmp    = lpObject->lpUndoText;
      		lpObject->lpUndoText = lpObject->lpText;
      		lpObject->lpText     = pTmp;
      		}
    	}
	if (lpObject->DataUndoType & UT_COLORMAP)
		{
		if (lpObject->UndoColorMap)
			{
			FrameSetColorMap( ObjGetEditFrame(lpObject), lpObject->UndoColorMap);
			FrameDestroyColorMap(lpObject->UndoColorMap);
			}
		lpObject->UndoColorMap = lpColorMap;
		}
	if (lpObject->DataUndoType & UT_SELECTION)
		{
		fSelected = lpObject->fSelected;
		lpObject->fSelected = lpObject->fUndoSelected;
		lpObject->fUndoSelected = fSelected;
		}
	if (lpObject->DataUndoType & UT_LOCKOBJECTS)
		{
		bLocked = lpObject->bLocked;
		lpObject->bLocked = lpObject->bUndoLocked;
		lpObject->bUndoLocked = bLocked;
		}
	if (lpObject->DataUndoType & UT_GROUPOBJECTS)
		{
		GroupID = lpObject->wGroupID;
		lpObject->wGroupID = lpObject->UndoGroupID;
		lpObject->UndoGroupID = GroupID;
		}
	if (lpObject->DataUndoType & UT_DELETEOBJECTS)
		{
		lpNext = lpObject;
		while (lpNext = GetNextObject(lpNext, YES, NO, YES))
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
		while (lpNext = GetNextObject(lpNext, YES, NO, NO))
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
		ObjEditApply(lpObject, NO, YES, NO);
	if (lpAlpha)
		{
		XSize = FrameXSize(lpAlpha->Pixmap.EditFrame);
		YSize = FrameYSize(lpAlpha->Pixmap.EditFrame);
		if (XSize != FrameXSize(lpObject->Pixmap.EditFrame) ||
			YSize != FrameYSize(lpObject->Pixmap.EditFrame))
			{
			ObjEditInit(lpObject, UT_DELETEMASK);
			// dont free mask if it was stored for later undo/redo
			if (!lpObject->lpUndoAlpha)
				MaskClose(lpObject->lpAlpha);
			lpObject->fBothDirty = YES;
			lpObject->AlphaDirty = lpObject->DataDirty;
			lpObject->lpAlpha = NULL;
			}
		}
	}
}

void CImage::ObjEditApply(LPOBJECT lpObject,
				BOOL fApplyData, BOOL fApplyAlpha,
				BOOL fInitFrame)
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
	if (lpObject == GetBase())
		{
		PurgeObjects();
		lpNext = lpObject;
		while (lpNext = GetNextObject(lpNext, YES, NO, NO))
			lpNext->fUndoDeleted = NO;
		}
	if (!fInitingData)
		PixmapFreeUndo(&lpObject->Pixmap);
	else
		PixmapEditApply(&lpObject->Pixmap);
   if (lpObject->lpUndoText)
      {
      delete lpObject->lpUndoText;
      lpObject->lpUndoText = NULL;
      }
	if (lpObject->UndoColorMap)
		{
		FrameDestroyColorMap(lpObject->UndoColorMap);
		lpObject->UndoColorMap = NULL;
		}
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

void CImage::ObjEditFreeUndo( LPOBJECT lpObject, BOOL fFreeData, BOOL fFreeAlpha )
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

BOOL CImage::ObjEditUndoDiscardable( LPOBJECT lpObject,
							BOOL fCheckData, BOOL fCheckAlpha )
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

BOOL CImage::ObjEditDiscardUndoCheck( LPOBJECT lpObject,
					BOOL fCheckData, BOOL fCheckAlpha )
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

// helper function
BOOL CheckAbortInPlaceEdit(LPPIXMAP lpPixmap)
{
	// see if we will be aborting a partial change that
	// we cannot back out of
	if (Control.UseApply || !lpPixmap->UndoFrame)
		return(AstralAffirm(IDS_PARTIALCHANGE));
	else
		return(TRUE);
}





