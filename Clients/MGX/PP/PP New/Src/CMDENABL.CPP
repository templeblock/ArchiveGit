//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "cmdenabl.h"
#include "svrview.h"
#include "svrdoc.h"

BOOL AlwaysEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return(TRUE);
}

BOOL IsObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return(pImage->MultipleObjects());
}

BOOL IsNonBaseObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return(pImage->GetSelObject() != pImage->GetBase());
}

BOOL IsSizeEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return(pImage->IsSizable());
}

BOOL IsInvertEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;
	pImage->GetTypeInfo(&TypeInfo);
	return( (TypeInfo.DataType != FDT_PALETTECOLOR ||
			(!pImage->GetMask() &&
			(pImage->CountObjects() == pImage->CountSelObjects()))) );
}

BOOL IsConvertEnabled(CImage *pImage, CServerView *pView, LPCONVERT_PARMS lpParms)
{
	if (pImage->InMaskEditMode())
		return(FALSE);

	if (lpParms)
	{
		FRMTYPEINFO TypeInfo;
		
		pImage->GetTypeInfo(&TypeInfo);
		if (FrameTypeInfoEqual(TypeInfo, lpParms->cmsInfo.dst) && !IsDstPTSelected(&lpParms->cmsInfo.dst.ptInfo))
			return(FALSE);
		if (lpParms->cmsInfo.src.DataType != FDT_NONE &&
			TypeInfo.DataType != lpParms->cmsInfo.src.DataType) 
			return(FALSE);
	}
	return(TRUE);
}

BOOL IsCutEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return (pImage->GetMask() || !pImage->IsSelectedObject(pImage->GetBase()));
}
 
BOOL IsMappingEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;

	pImage->GetTypeInfo(&TypeInfo);
	return(	TypeInfo.DataType != FDT_LINEART &&
			(TypeInfo.DataType != FDT_PALETTECOLOR ||
			(!pImage->GetMask() &&
			(pImage->CountObjects() == pImage->CountSelObjects()))) );
}

BOOL IsTransformObjectsEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	if (!pImage->MultipleObjects())
		return(FALSE);
	if (pImage->GetMask() && !Control.UseMaskAndObjects)
		return(FALSE);
	return(TRUE);
}

BOOL IsMergeMaskEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return (pImage->GetMask() &&
			pImage->GetSelObject() != pImage->GetBase() );
}

BOOL IsPasteEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	BOOL fEnabled = FALSE;

	if (pImage->lpClipObject)
		return(TRUE);
	else
	{
		if (!::OpenClipboard(PictPubApp.m_pMainWnd->GetSafeHwnd()))
			return(fEnabled);
		fEnabled = (::IsClipboardFormatAvailable(CF_METAFILEPICT) || // new jtm
        			::IsClipboardFormatAvailable(CF_DIB) ||
	    			::IsClipboardFormatAvailable(CF_BITMAP) ||
	    			::IsClipboardFormatAvailable(Control.cfImage) ||
	    			::IsClipboardFormatAvailable(Control.cfImagePP4));
		::CloseClipboard();			
		return(fEnabled);
	}
}

BOOL IsEffectEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;

	pImage->GetTypeInfo(&TypeInfo);
	return(	TypeInfo.DataType != FDT_PALETTECOLOR &&
			TypeInfo.DataType != FDT_LINEART );
}

BOOL IsMaskFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return(	pImage->GetMask() != NULL );
}

BOOL IsFeatherMaskEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;

	pImage->GetTypeInfo(&TypeInfo);
	return(	pImage->GetMask() != NULL &&
			CANDOTRANSPARENCY(TypeInfo.DataType) );
}

BOOL IsFeatherObjectEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;

	pImage->GetTypeInfo(&TypeInfo);
	return(	pImage->GetMask() != NULL &&
			CANDOTRANSPARENCY(TypeInfo.DataType) &&
			pImage->GetSelObject() != pImage->GetBase() );
}					 

BOOL IsEditUndoEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	return(pImage->GetDirtyState() != 0);
}

BOOL IsMaskUndoEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	LPOBJECT lpBase = pImage->GetBase();
	return(lpBase->AlphaDirty && !lpBase->fBothDirty);
}

BOOL IsPaletteEditEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;

	pImage->GetTypeInfo(&TypeInfo);
	return(	TypeInfo.DataType == FDT_PALETTECOLOR );
}

BOOL IsUnlockedObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{		  
	BOOL     f = FALSE;
	LPOBJECT lpObject = pImage->GetBase();

 	while (lpObject = pImage->GetSelObject(lpObject))
	{
		if ((lpObject->wGroupID && !IsGroupLocked(lpObject->wGroupID)) ||
          !lpObject->bLocked)
		{
        	f = TRUE;
         	break;
	    }
	}
	return(f);
}

BOOL IsLockedObjectFunctionEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{		  
	BOOL     f = FALSE;
	LPOBJECT lpObject = pImage->GetBase();

 	while (lpObject = pImage->GetSelObject(lpObject))
	{
		if ((lpObject->wGroupID && IsGroupLocked(lpObject->wGroupID)) ||
          lpObject->bLocked)
		{
        	f = TRUE;
         	break;
	    }
	}
	return(f);
}
 
BOOL IsPSFilterEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;

	pImage->GetTypeInfo(&TypeInfo);
	return(	TypeInfo.DataType != FDT_PALETTECOLOR );
}

BOOL IsCropMaskEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
    return(pImage->GetMask() &&	pImage->IsSizable());
}

BOOL IsChromaEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	FRMTYPEINFO TypeInfo;
	
	pImage->GetTypeInfo(&TypeInfo);
	return(TypeInfo.DataType != FDT_LINEART && !pImage->InMaskEditMode());
}

BOOL IsGroupEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	BOOL     f;

	f = (pImage->CountSelObjects(pImage->GetBase()) > 1);
	return(f && !pImage->IfSelObjectsGrouped());
}

BOOL IsUngroupEnabled(CImage *pImage, CServerView *pView, LPCMD_PARMS lpParms)
{
	LPOBJECT	lpObject;
	BOOL     f = FALSE;

	if( !( lpObject = pImage->GetBase()))
		return(FALSE);

	while (lpObject = pImage->GetSelObject(lpObject))
		if (lpObject->wGroupID)
		{
        	f = TRUE;
         	break;
	    }
	return(f);
}

BOOL IsSelectObjectsRelativeEnabled(CImage *pImage, CServerView *pView, LPSELECTOBJRELATIVE_PARMS lpParms)
{
	if (lpParms)
		return( lpParms->Pos < ImgCountFloaters(pImage) );
	else
		return( pImage->MultipleObjects() );
}
		
 
