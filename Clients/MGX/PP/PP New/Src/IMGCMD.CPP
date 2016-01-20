//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "imgcmd.h"
#include "id.h"
#include "dataitem.h"

LOCAL BOOL CropToRect( LPIMAGE lpImage, LPRECT lpCropRect, int idDirty );

/************************************************************************/
BOOL SaveMaskImage(LPIMAGE lpImage, LPSAVEMASK_PARMS lpParms)
/************************************************************************/
{
	FNAME szFileName;
	LPMASK lpMask;

	ProgressBegin(1, PROGRESS_ID(IDS_UNDOSAVEMASK));

	if ( !(lpMask = ImgGetMask(lpImage)) )
	{
		ProgressEnd();
   	    Message( IDS_ENOMASK );
		return(FALSE);
	}

	if ( !CreateFileName( lpParms->szName, IDN_MASK, szFileName ) )
	{
		ProgressEnd();
		return(FALSE);
	}

	FrameSetResolution(lpMask->Pixmap.EditFrame,
					FrameResolution(ImgGetBaseEditFrame(lpImage)) );
	if (!WriteMask(szFileName, lpMask, FALSE))
	{
		ProgressEnd();
		return(FALSE);
	}

	if (!AddExtFile(lpParms->szName, szFileName, IDN_MASK))
		{
		FileDelete(szFileName);
		ProgressEnd();
		return(FALSE);
		}
	ProgressEnd();
	return(TRUE);
}

/************************************************************************/
BOOL CopyToClipboard(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
	if (lpImage->lpClipObject)
		ObjFreeUp(lpImage->lpClipObject);
	lpImage->lpClipObject = ImgCreateClipOutObject(lpImage);
	return(lpImage->lpClipObject != NULL);
}

/************************************************************************/
BOOL DoCloseImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
lpParms->Common.UpdateType = UT_CLOSE;
return(TRUE);
}

/************************************************************************/
BOOL CutImage(LPIMAGE lpImage, LPTINTFILL_PARMS lpParms)
/************************************************************************/
{
	if (ImgGetMask(lpImage))
	{
		return(TintFillImage(lpImage, lpParms));
	}
	else
	{
		CMD_PARMS parms;

		parms.Common = lpParms->Common;
		BOOL fRet = DeleteObjectsImage(lpImage, &parms);
		lpParms->Common = parms.Common;
		return(fRet);
	}
}

/************************************************************************/
BOOL AlphaEditImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
LPOBJECT lpNewBase, lpOldBase;
LPMASK lpMask, lpNewMask;
RECT rUpdate, rTemp;
BOOL fMaskDeleted;
int dirty, dx, dy;

if (!lpImage)
	return(FALSE);

if ( !(lpOldBase = (LPOBJECT)lpImage->AltObjList.lpHead) )
	{ // turn on mask editing mode
	lpOldBase = ImgGetBase(lpImage);
	lpMask = ImgGetMaskEx(lpImage,OFF,Control.NoUndo,&lpImage->fMaskCreate, &rUpdate);
	if (!lpMask)
		return(FALSE);

	// if a both are dirty apply alpha changes
	if (lpOldBase->fBothDirty)
		lpImage->ObjEditApply(lpOldBase, NO, YES, NO);

	// Create a new base object
	if ( !(lpNewBase = (LPOBJECT)ObjCreateFromPixmap(ST_PERMANENT,
		&lpMask->Pixmap, NULL, NULL, Control.NoUndo)) )
		{
		Message(IDS_EMEMALLOC);
		if (lpImage->fMaskCreate)
			MaskClose(lpMask);
		return(FALSE);
		}

	GetObjectMarqueeRect(lpImage, &rTemp);
	AstralUnionRect(&rUpdate, &rUpdate, &rTemp);

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
	lpImage->iUndoCreateMaskDirty = NO;
	if ((lpNewBase->DataUndoType == 0) &&
		(lpOldBase->AlphaUndoType & UT_CREATEMASK))
		lpImage->iUndoCreateMaskDirty = lpOldBase->AlphaDirty;
	lpNewBase->DataUndoNotRedo = lpOldBase->AlphaUndoNotRedo;
	lpOldBase->fBothDirty = NO;

	// make sure the mask channel has same resolution as image
	FrameSetResolution(ObjGetEditFrame(lpNewBase),
						FrameResolution(ObjGetEditFrame(lpOldBase)));
	if (ObjGetUndoFrame(lpNewBase))
		FrameSetResolution(ObjGetUndoFrame(lpNewBase),
						FrameResolution(ObjGetEditFrame(lpOldBase)));

	// Swap the two object list pointers
	lpImage->AltObjList = lpImage->ObjList;
	lpImage->ObjList.lpHead = lpImage->ObjList.lpTail = (LPPRIMOBJECT)lpNewBase;
	// Redisplay if not in slime view mode
	
	lpImage->fSaveNoSize = lpImage->fNoSize;
	lpImage->fNoSize = YES;

	if (ImgGetViewMode(lpImage) != VM_SLIME || lpImage->fMaskCreate)
		{
		ImgGetInfo(lpImage, &dx, &dy, NULL, NULL);
		SetRect(&rTemp, 0, 0, dx-1, dy-1);
		AstralUnionRect(&rUpdate, &rTemp, &rTemp);
//		lpParms->Common.UpdateType = UT_DATATYPE;
		}
	lpParms->Common.UpdateType = UT_DATATYPE;
	if (!AstralIsRectEmpty(&rUpdate))
		{
		lpParms->Common.UpdateType |= UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
		}
	return(TRUE);
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
	ImgCombineObjects( lpImage, NO, NO, YES, IDS_UNDOCOMBINE, &rUpdate );
	ImgPurgeObjects(lpImage);

	// Copy the new base Pixmap structure over to the the old mask Pixmap
	// and recompute the mask rectangle
	fMaskDeleted = NO;
	if ( lpMask = lpOldBase->lpAlpha )
		{
		lpMask->Pixmap = lpNewBase->Pixmap;
        if ( (lpImage->fMaskCreate && !lpImage->fChanged) ||
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
				dirty = lpImage->iUndoCreateMaskDirty;
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

	lpImage->fNoSize = lpImage->fSaveNoSize;

	// Redisplay if not in slime view mode (or we had a mask)
	if (ImgGetViewMode(lpImage) != VM_SLIME || lpNewMask /*had a new mask*/ ||
			fMaskDeleted)
		{
//		if (ImgGetViewMode(lpImage) != VM_SLIME)
//			lpParms->Common.UpdateType = UT_DATATYPE;
		ImgGetInfo(lpImage, &dx, &dy, NULL, NULL);
		SetRect(&rTemp, 0, 0, dx-1, dy-1);
		AstralUnionRect(&rUpdate, &rTemp, &rTemp);
		}
	lpParms->Common.UpdateType = UT_DATATYPE;
	if (!AstralIsRectEmpty(&rUpdate))
		{
		lpParms->Common.UpdateType |= UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
		}
	return(TRUE);
	}
}

/************************************************************************/
BOOL CombineObjects(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
BOOL fRet;
RECT rUpdate;

fRet = ImgCombineObjects(lpImage, NO, NO, NO, lpParms->Common.idDirty,
						&rUpdate);
if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(fRet);
}

/************************************************************************/
BOOL CombineSelObjects(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
BOOL fRet;
RECT rUpdate;

fRet = ImgCombineSelObjects(lpImage, NO, NO, NO, lpParms->Common.idDirty,
						&rUpdate);
if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(fRet);
}

/************************************************************************/
BOOL CopyToFile(LPIMAGE lpImage, LPCOPYTOFILE_PARMS lpParms)
/************************************************************************/
{
ITEMID Type, DataType;
FNAME szFileName;
FILE_HANDLE fh;
FNAME szPathName;
BOOL bUpdateThumb = NO;
LPSTR lpPathName = NULL;
int   FileType = IDN_PP;

ProgressBegin(1, PROGRESS_ID(IDS_UNDOCOPYTOFILE));
if (lpParms->Type == 1)
   {
	Type = IDN_TEXTURE;
   FileType = IDN_TIFF;
   }
else if (lpParms->Type == 2)
   {
	Type = IDN_CUSTOM;
   FileType = IDN_TIFF;
   }
else
	{
	Type = IDN_CLIPBOARD;
	bUpdateThumb = ExtBrowser.PasteThumbCreate;
	if (Control.UseProjectDirectory && !lpImage->fUntitled)
		{
		lstrcpy(szPathName, lpImage->CurFile);
		stripfile(szPathName);
		lpPathName = szPathName;
		}
	}

if ( !CreateFileName( lpParms->szName, Type, szFileName, lpPathName) )
	{
	ProgressEnd();
	return(FALSE);
	}

if (Type == IDN_CUSTOM)
	DataType = IDC_SAVECT;
else
	DataType = lpImage->DataType;

if ( !ImgWriteClipOut(lpImage, szFileName, DataType, FileType, bUpdateThumb ))
	{
	ProgressEnd();
	return(FALSE);
	}
if (!AddExtFile(lpParms->szName, szFileName, Type))
	{
	FileDelete(szFileName);
	ProgressEnd();
	return(FALSE);
	}
if (Type == IDN_CLIPBOARD)
{
	UpdatePasteFromFile(NULL);
}

ProgressEnd();
return(TRUE);
}

/************************************************************************/
BOOL CropMaskImage( LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
LPMASK lpMask;
RECT rCrop;

if (!(lpMask = ImgGetMask( lpImage )))
	return(TRUE); // no need to crop - but don't consider it an error
rCrop = lpMask->rMask;

if (CropToRect(lpImage, &rCrop, lpParms->Common.idDirty))
	{
	lpParms->Common.UpdateType = UT_SIZE;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL CropImage( LPIMAGE lpImage, LPCROP_PARMS lpParms )
/************************************************************************/
{
int res;
RECT rCrop, rImage, rSect;
LPFRAME lpFrame;

lpFrame = ImgGetBaseEditFrame(lpImage);
res = FrameResolution(lpFrame);
rCrop = lpParms->CropRect;
ResConvertRect(lpParms->iBaseRes, res, &rCrop);

rImage.left = rImage.top = 0;
rImage.right = FrameXSize(lpFrame)-1;
rImage.bottom = FrameYSize(lpFrame)-1;
if (!AstralIntersectRect(&rSect, &rCrop, &rImage))
	return(TRUE);
if (CropToRect(lpImage, &rCrop, lpParms->Common.idDirty))
	{
	lpParms->Common.UpdateType = UT_SIZE;
	return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
LOCAL BOOL CropToRect( LPIMAGE lpImage, LPRECT lpCropRect, int idDirty )
/************************************************************************/
{
RECT rCrop, rMask, rSect, rImage;
LPOBJECT lpBase, lpObject;
LPMASK lpMask;
LPFRAME lpDataFrame, lpMaskFrame;
UNDO_TYPE UndoType;

if ( !lpImage )
    return( FALSE );

if ( !(lpBase = ImgGetBase( lpImage )) )
    return( FALSE );
rCrop = *lpCropRect;
lpMask = ImgGetMask( lpImage );

ProgressBegin(lpMask != NULL ? 2 : 1, PROGRESS_ID(idDirty));

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
// Copy out the new data frame
if ( !(lpDataFrame = FrameCopy( ImgGetBaseEditFrame(lpImage), &rCrop )) )
    {
    ProgressEnd();
    Message(IDS_EMEMALLOC);
    return( FALSE );
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
        return( FALSE );
        }
    }

if ( !ImgEditInit(lpImage, ET_OBJECT, UndoType, lpBase))
    {
    FrameClose( lpDataFrame );
    FrameClose( lpMaskFrame );
    ProgressEnd();
    return(FALSE);
    }

lpObject = lpBase;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
    {
    if (!AstralIntersectRect(&rSect, &rCrop, &lpObject->rObject))
        {
// always let undo stuff initialize this
//      lpObject->fUndoDeleted = NO;
        lpObject->fDeleted = YES;
        }
    else
        OffsetRect(&lpObject->rObject, -rCrop.left, -rCrop.top);
    }

if (UndoType & UT_DELETEMASK)
    ImgSetMask(lpImage, NULL);

ImgEditedObjectFrame(lpImage, lpBase, idDirty, NULL, lpDataFrame, lpMaskFrame);
// Setup the new image and bring up the new image window
lpBase->rObject.right = FrameXSize(lpDataFrame)-1;
lpBase->rObject.bottom = FrameYSize(lpDataFrame)-1;
if (lpMask)
    {
    lpMask->rMask = rMask;
    OffsetRect(&lpMask->rMask, -rCrop.left, -rCrop.top);
    }
ProgressEnd();
return( TRUE );
}

/************************************************************************/
BOOL CreateClipImage( LPIMAGE lpImage, LPCMD_PARMS lpParms)
/************************************************************************/
{
    LPOBJECT lpObject = ImgCreateClipOutObject(lpImage);
	if (lpObject)
	{		  
		RECT rAlpha;

		ImgDeleteObjects(lpImage, NULL, NO);
		ObjAddTail(&lpImage->ObjList, lpObject);
		lpImage->npix = FrameXSize(ObjGetEditFrame(lpObject));
		lpImage->nlin = FrameYSize(ObjGetEditFrame(lpObject));
		if (lpObject->lpAlpha)
			MaskRectUpdate(lpObject->lpAlpha, &rAlpha);
		lpParms->Common.UpdateType = UT_SIZE;
		return(TRUE);
	}
	return(FALSE);
}

/************************************************************************/
BOOL ImgEditApply( LPIMAGE lpImage, LPCMD_PARMS lpParms )
/************************************************************************/
{
	LPOBJECT lpObject;

	lpObject = NULL;
	while (lpObject = lpImage->GetNextObject(lpObject, YES, NO))
	{
		if (Control.UndoObjects && !lpImage->IsSelectedObject(lpObject))
			continue;
		lpImage->ObjEditApply( lpObject, YES, YES, NO );
	}
	return(TRUE);
}

/***********************************************************************/
BOOL EditUndoImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/***********************************************************************/
{
	lpImage->EditUndo(YES, NO, &lpParms->Common.UpdateType, 
								&lpParms->Common.rUpdateArea);

	return(TRUE);
}

/***********************************************************************/
BOOL MaskUndoImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/***********************************************************************/
{
	lpImage->EditUndo(NO, YES, &lpParms->Common.UpdateType, 
								&lpParms->Common.rUpdateArea);
	return(TRUE);
}

/***********************************************************************/
BOOL DiscardUndo(LPIMAGE lpImage, LPDISCARDUNDO_PARMS lpParms)
/***********************************************************************/
{
	lpImage->fDiscardUndo = lpParms->fDiscardUndo;
	return(TRUE);
}


