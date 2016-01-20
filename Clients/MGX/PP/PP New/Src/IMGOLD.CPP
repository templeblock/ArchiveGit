//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "macros.h"
#include "id.h"
#include "cpo.h"
#include "svrview.h"

static BOOL ImgGetLineAlloc(int dx, int depth);
static LPOBJECT ImgGetSelObjectEx(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fBottomUp);
WORD ImgGetNextGroupID(LPIMAGE lpImage);
static BOOL ImgSelectObjects(LPIMAGE lpImage, LPOBJECT lpObject, LPRECT lpRect, BOOL fSelect,
                BOOL fBottomUp, BOOL fExcludeBase);
static void ImgAdjustGroupSelection(LPIMAGE lpImage, BOOL fSelect);

#define ASSERT(n) 

/************************************************************************/
void ImgGetInfo(LPIMAGE lpImage, LPINT lpXSize, LPINT lpYSize,
                    LPINT lpDepth, LPFRMDATATYPE lpType)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->GetInfo(lpXSize, lpYSize, lpDepth, lpType);
}

/************************************************************************/
void ImgGetInfoEx(LPIMAGE lpImage, LPINT lpXSize, LPINT lpYSize,
                    LPINT lpDepth, LPFRMDATATYPE lpType, LPINT lpResolution)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->GetInfo(lpXSize, lpYSize, lpDepth, lpType, lpResolution);
}

/************************************************************************/
void ImgGetTypeInfo(LPIMAGE lpImage, LPFRMTYPEINFO lpTypeInfo)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->GetTypeInfo(lpTypeInfo);
	else
    	FrameSetTypeInfo(lpTypeInfo, FDT_RGBCOLOR, NULL);   
}

/************************************************************************/
LPOBJECT ImgGetBase(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (!lpImage)
    	return(NULL);
	return(lpImage->GetBase());
}

/************************************************************************/
LPFRAME ImgGetBaseEditFrame(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (!lpImage)
    	return(NULL);
	return(lpImage->GetBaseEditFrame());
}

/************************************************************************/
LPFRAME ImgGetBaseUndoFrame(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (!lpImage)
    	return(NULL);
	return(lpImage->GetBaseUndoFrame());
}

/************************************************************************/
LPMASK ImgGetMask(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (!lpImage)
    	return(NULL);
	return(lpImage->GetMask());
}

//************************************************************************
//  Gets the current mask if any.
//  If no mask exists then create one using fOn and fNoUndo.
//  bCreated is TRUE if a new mask was created. 
//************************************************************************
LPMASK ImgGetMaskEx(LPIMAGE lpImage, BOOL fOn, BOOL fNoUndo, LPINT bCreated,
                    LPRECT lpUpdateRect)
//************************************************************************
{
	
	ASSERT(lpImage != NULL);

    AstralSetRectEmpty(lpUpdateRect);
    if (!lpImage)
        return(NULL);
	return(lpImage->GetMaskEx(fOn, fNoUndo, bCreated, lpUpdateRect));
}

/************************************************************************/
void ImgSetMask(LPIMAGE lpImage, LPMASK lpMask)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->SetMask(lpMask);
}

/************************************************************************/
BOOL ImgGetMaskRect(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetMaskRect(lpRect));
	else
		return(FALSE);
}

/************************************************************************/
void ImgGetImageRect(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->GetImageRect(lpRect);
}

/************************************************************************/
void ImgChangeSelection(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fSelect,
                        BOOL fExtendedSel)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->ChangeSelection(lpObject, fSelect, fExtendedSel);
}

/************************************************************************/
BOOL ImgSelObject(LPIMAGE lpImage, LPOBJECT lpObject, LPRECT lpRect,
                    BOOL fExtendedSel, BOOL fExcludeBase)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->SelObject(lpObject, lpRect, fExtendedSel, fExcludeBase));
	else
		return(FALSE);
}

/************************************************************************/
int ImgCountSelObjects(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CountSelObjects(lpObject));
	else
		return(0);
}

/************************************************************************/
int ImgCountObjects(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CountObjects());
	else
		return(0);
}

/************************************************************************/
LPOBJECT ImgGetSelObject(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetSelObject(lpObject));
	else
		return(0);
}

/************************************************************************/
LPOBJECT ImgGetSelObjectEx(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fBottomUp)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetSelObject(lpObject, fBottomUp));
	else
		return(NULL);
}

/************************************************************************/
void ImgInvertSelection(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->InvertSelection();
}

/************************************************************************/
BOOL ImgIsSelectedObject(LPIMAGE lpImage, LPOBJECT lpObject)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->IsSelectedObject(lpObject));
	else
		return(FALSE);
}

/************************************************************************/
LPOBJECT ImgGetNextObject(LPIMAGE lpImage, LPOBJECT lpObject,
                        BOOL fBottomUp, BOOL fLoop)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetNextObject(lpObject, fBottomUp, fLoop));
	else
		return(NULL);
}

/************************************************************************/
LPOBJECT ImgGetNextObjectEx(LPIMAGE lpImage, LPOBJECT lpObject,
                        BOOL fBottomUp, BOOL fLoop, BOOL fGetDeletedObjects)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetNextObject(lpObject, fBottomUp, fLoop, fGetDeletedObjects));
	else
		return(NULL);
}

/************************************************************************/
LPOBJECT ImgFindSelObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->FindSelObject(x, y, fBottomUp));
	else
		return(NULL);
}

/************************************************************************/
LPOBJECT ImgFindObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
                    BOOL fExcludeBase)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->FindObject(x, y, fBottomUp, fExcludeBase));
	else
		return(NULL);
}

/************************************************************************/
BOOL ImgGetSelObjectRect( LPIMAGE lpImage, LPRECT lpRect, BOOL bExcludeBase )
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetSelObjectRect(lpRect, bExcludeBase));
	else
		return(FALSE);

}

/************************************************************************/
BOOL ImgGetFloatingObjectsRect( LPIMAGE lpImage, LPRECT lpRect )
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetFloatingObjectsRect(lpRect));
	else
		return(FALSE);
}

/************************************************************************/
void ImgAdjustLayerSelObj(LPIMAGE lpImage, BOOL fToTop, BOOL fOneLevel)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->AdjustLayerSelObj(fToTop, fOneLevel);
}

/************************************************************************/
void ImgAdjustLayerObject(LPIMAGE lpImage, LPOBJECT lpObject,
                        LPOBJECT lpToObject, BOOL fAfter)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->AdjustLayerObject(lpObject, lpToObject, fAfter);
}

/************************************************************************/
void ImgGroupSelObjects(LPIMAGE lpImage, BOOL fGroup)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->GroupSelObjects(fGroup);
}

/************************************************************************/
BOOL ImgIfSelObjectsGrouped(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->IfSelObjectsGrouped());
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgMultipleObjects(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->MultipleObjects());
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgDisplayMarquees(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->DisplayMarquees());
	else
		return(FALSE);
}

/************************************************************************/
LPOBJECT ImgGetMaskMarqueeObject(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetMaskMarqueeObject());
	else
		return(NULL);
}

/************************************************************************/
LPOBJECT ImgGetObjectMarqueeObject(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetObjectMarqueeObject());
	else
		return(NULL);
}

/************************************************************************/
LPMASK ImgGetMarqueeMask(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetMarqueeMask());
	else
		return(NULL);
}

/************************************************************************/
BOOL ImgGetMaskUpdateRect(LPIMAGE lpImage, BOOL fNewMask, BOOL fInvert, LPRECT lpRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetMaskUpdateRect(fNewMask, fInvert, lpRect));
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgGetUndoState(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetUndoState());
	else
		return(FALSE);
}

/************************************************************************/
int ImgGetDirtyState(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetDirtyState());
	else
		return(0);
}

/************************************************************************/
BOOL ImgChanged(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->Changed());
	else
		return(FALSE);
}

/************************************************************************/
void SetImgChanged(LPIMAGE lpImage, BOOL fChanged)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->SetChanged(fChanged);
}

/************************************************************************/
BOOL ImgCombineObjects(LPIMAGE lpImage, BOOL fNoUndo, BOOL fCreateMask,
                        BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CombineObjects(fNoUndo, fCreateMask, fForceIt, idDirty, lpUpdateRect));
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgCombineSelObjects(LPIMAGE lpImage, BOOL fNoUndo, BOOL fCreateMask,
                        BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CombineSelObjects(fNoUndo, fCreateMask, fForceIt, idDirty, lpUpdateRect));
	else
		return(FALSE);
}

/************************************************************************/
void ImgDeleteObjects(LPIMAGE lpImage, LPOBJECT lpObject, BOOL bOnlyIfSelected)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->DeleteObjects(lpObject, bOnlyIfSelected);
}

/************************************************************************/
void ImgPurgeObjects(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->PurgeObjects();
}

/************************************************************************/
void ImgDeleteObject(LPIMAGE lpImage, LPOBJECT lpObject )
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->DeleteObject(lpObject);
}

/************************************************************************/
void ImgSetViewMode(LPIMAGE lpImage, VIEW_MODE ViewMode)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->SetViewMode(ViewMode);
}

/************************************************************************/
VIEW_MODE ImgGetViewMode(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetViewMode());
	else
		return(VM_NORMAL);
}

/************************************************************************/
LPOBJECT ImgAddFrameObject(LPIMAGE lpImage, LPFRAME lpFrame, LPMASK lpAlpha, LPRECT lpRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->AddFrameObject(lpFrame, lpAlpha, lpRect));
	else
		return(NULL);
}

/************************************************************************/
LPOBJECT ImgCreateMaskObject(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CreateMaskObject(lpRect));
	else
		return(NULL);
}

/************************************************************************/
BOOL ImgCrop( LPIMAGE lpImage, LPRECT lpCropRect )
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->Crop(lpCropRect));
	else
		return(FALSE);
}

/************************************************************************/
LPOBJECT ImgCreateCutoutObject(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CreateCutoutObject(lpRect));
	else
		return(NULL);
}

/************************************************************************/
void ImgAddNewObject(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fSetObjRes)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->AddNewObject(lpObject, fSetObjRes);
}

/************************************************************************/
void ImgDeleteTempObjects(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->DeleteTempObjects();
}

/************************************************************************/
BOOL ImgKeepFrameTempObjects(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->KeepFrameTempObjects();
	return(TRUE);
}

/************************************************************************/
WORD ImgGetNextGroupID(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->GetNextGroupID());
	else
		return(0);
}

/************************************************************************/
BOOL ImgWritePP4ClipOut(LPIMAGE lpImage, LPTR lpFileName, LPTR lpMaskName,
					LPRECT lpRect, int DataType )
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->WritePP4ClipOut(lpFileName, lpMaskName, lpRect, DataType));
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgWriteClipOut(LPIMAGE lpImage, LPSTR lpFileName, int DataType ,ITEMID Type, BOOL bUpdateThumb)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
	  	return(lpImage->WriteClipOut(lpFileName, DataType, Type, bUpdateThumb));
	else
		return(FALSE);
}

/************************************************************************/
LPOBJECT ImgCreateClipOutObject(LPIMAGE lpImage, BOOL CopyObj)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->CreateClipOutObject(CopyObj));
	else
		return(NULL);
}


/************************************************************************/
BOOL ImgDuplicateSelObjects(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->DuplicateSelObjects());
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgSetupMaskEditMode(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->SetupMaskEditMode());
	else
		return(FALSE);
}

/************************************************************************/
BOOL ImgInMaskEditMode(LPIMAGE lpImage)
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->InMaskEditMode());
	else
		return(FALSE);
}

BOOL ImgEditInit( LPIMAGE lpImage, EDIT_TARGET Target, UNDO_TYPE UndoType,
				LPOBJECT lpTargetObject)
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->EditInit(Target, UndoType, lpTargetObject));
	else
		return(FALSE);
}

/************************************************************************/
void ImgEditApplyAll( LPIMAGE lpImage, BOOL fApplyData, BOOL fApplyAlpha )
/************************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->EditApplyAll(fApplyData, fApplyAlpha);
}

/***********************************************************************/
LOCAL void ImgEditUndo( LPIMAGE lpImage, BOOL fEditUndo, BOOL fMaskUndo,
					LPUPDATE_TYPE lpUpdateType, LPRECT lpUpdateRect)
/***********************************************************************/
{
	AstralSetRectEmpty(lpUpdateRect);
	if (lpImage)
		lpImage->EditUndo(fEditUndo, fMaskUndo, lpUpdateType, lpUpdateRect);
}

/***********************************************************************/
void ImgEditMaskUndo( LPIMAGE lpImage )
/***********************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->EditMaskUndo();
}

/***********************************************************************/
BOOL ImgEditRecoverSelObj( LPIMAGE lpImage, BOOL fEditUndo, BOOL fMaskUndo)
/***********************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->EditRecoverSelObj(fEditUndo, fMaskUndo));
	else
		return(FALSE);
}

/***********************************************************************/
BOOL ImgEditRecoverObj( LPIMAGE lpImage, LPOBJECT lpObject,
					BOOL fEditUndo, BOOL fMaskUndo)
/***********************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		return(lpImage->EditRecoverObj(lpObject, fEditUndo, fMaskUndo));
	else
		return(FALSE);
}

/***********************************************************************/
void ImgEditedObject(LPIMAGE lpImage, LPOBJECT lpObject, int iDirty, LPRECT lpEditRect)
/***********************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->EditedObject(lpObject, iDirty, lpEditRect);
}

/***********************************************************************/
void ImgEditedObjectFrame(LPIMAGE lpImage, LPOBJECT lpObject, int iDirty,
					LPRECT lpEditRect,
					LPFRAME lpDataFrame, LPFRAME lpAlphaFrame)
/***********************************************************************/
{
	ASSERT(lpImage != NULL);
	if (lpImage)
		lpImage->EditedObjectFrame(lpObject, iDirty, lpEditRect, lpDataFrame, lpAlphaFrame);
}



