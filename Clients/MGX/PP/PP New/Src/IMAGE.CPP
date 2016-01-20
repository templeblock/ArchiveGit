//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "macros.h"
#include "cpo.h"
#include "suminfo.h" 
#include "svrview.h"
#include "ppmodal.h"
#include "id.h"

__declspec( dllimport ) LPKCMSFREEPROC lpKCMSFreeProc;

CImage::CImage(	ITEMID idDataType,
				LPSTR lpFileName,
				LPCMDLIST lpCommandList,
				ITEMID idFileType,
				int DocType,
				LPSTR lpImageName)
{
	fOwnedByClient = FALSE;
	fRelease = FALSE;
	// setup name of image, if no name passed in, setup an untitled image
	if ( !lpFileName )
    	{
    	fChanged  = TRUE;
    	fUntitled = TRUE;
    	GetUntitledName(CurFile);
    	}
	else
		{
		fChanged = FALSE;
		fUntitled = FALSE;
    	lstrcpy( CurFile, lpFileName );
		}
	if (lpImageName)
	    lstrcpy(ImageName, lpImageName);
	else
		ImageName[0] = '\0';
	npix = 0;
	nlin = 0;
	FileType = GetPPFileType(CurFile);
	if (!FileType)
		FileType = Control.DefaultFileType;
	DataType = idDataType;
	UndoDataType = 0;
	DocumentType = DocType;
	Element = 0;
	Compressed = FALSE;
	UseObjectMarquee = Control.UseObjectMarquee;
	MarqueeStatus = 0;
	ObjList.lpHead = ObjList.lpTail = NULL;
	AltObjList.lpHead = AltObjList.lpTail = NULL;
	m_ViewMode = VM_NORMAL;
	lpFastbitsPreview = NULL;
	AstralSetRectEmpty(&rFastbitsGrid);
	hPal = NULL;        
	nPaletteEntries = 0;
	fNoSize = FALSE;
	lpCmdList = lpCommandList;
	iUndoCreateMaskDirty = 0;
	fSaveNoSize = FALSE;
	lpPaletteLUT = NULL; 
	PaletteType = PT_COLORPALETTE;
	EditTarget = ET_OBJECT;
	fDisableUndos = FALSE;
	hLastBrush = NULL;
	szLastBrushStyle[0] = '\0';
	idLastBrush = 0;
	dwObjectID = 0;
	m_cmsXform = NULL;
	SaveInfo.toRCS = SaveInfo.frRCS = 0;
	lpClipObject = NULL;
	m_bDoCmsGamma = FALSE;
	fMaskCreate = FALSE;  
	fDiscardUndo = FALSE;
	PtInfo.toRCS = PtInfo.frRCS = 0;
	m_lpDispObject = NULL;

	if (lpCmdList)
		pServerDoc = lpCmdList->pDocument;
	else
		pServerDoc = NULL;
}

CImage::~CImage()
{
	CMD_PARMS parms;

    // notify the Image Forum that this image has been destroyed.
    ImageForum.NotifyImageChange(this, IMAGE_DESTROYED);

	if (AltObjList.lpHead)
	   	AlphaEditImage(this, &parms);

	// free up memory occupied by the image
	if (lpCmdList)
    	DestroyCommandList(lpCmdList);

	// delete the image's object list
	if (ObjList.lpHead) // do we have any objects yet?
		DeleteObjects(NULL, NO);

	// free up memory occupied by images palette look-up-table
	if (lpPaletteLUT)
    	FreeUp((LPTR)lpPaletteLUT);

	// free up the palette GDI object
	if (hPal)
   	{
    	// see if this is a palette that belongs to the image
    	if (hPal != lpBltScreen->hGrayPal &&
        	hPal != lpBltScreen->hColorPal)
        	::DeleteObject(hPal);
   	}

	// free up the brush cache
	if (hLastBrush)
    	DestroyMgxBrush((HMGXBRUSH)hLastBrush);

	if( Control.CMSEnabled && m_cmsXform && lpKCMSFreeProc )
		( *lpKCMSFreeProc )( m_cmsXform );

	if (lpClipObject)
		ObjFreeUp(lpClipObject);

}

BOOL CImage::Create(LPFRAME lpFrame)
{
    if (!AddFrameObject(lpFrame, NULL, NULL))
		return(FALSE);
	return(Create());
}

BOOL CImage::Create(LPOBJECT lpObject)
{
	ObjAddListTail(&this->ObjList, (LPPRIMOBJECT)lpObject);
	return(Create());
}

BOOL CImage::Create()
{
	// if a command list was not passed in then create one
	if (!lpCmdList)
    {
    	// if we don't have a command list, create one
 		lpCmdList = CreateCommandList();
    	if ( !lpCmdList)
        	return(FALSE);
    }
	// link command list back to image
	lpCmdList->ThreadData.lpImage = this;

	// setup image information for easy access

	LPFRAME lpFrame = GetBaseEditFrame();

	npix      = FrameXSize(lpFrame);
	nlin      = FrameYSize(lpFrame);

	// if using threading and no command list already existence
	// create the command thread
	#ifdef WIN32
	if (Control.UseThreading)
    	{
    	lstrcpy(lpCmdList->ThreadData.ThreadInfo.szThreadName,
            	filename(CurFile));
    	}
	#endif
	return(TRUE);
} 

void CImage::GetInfo(LPINT lpXSize,
					 LPINT lpYSize,
                     LPINT lpDepth,
					 LPFRMDATATYPE lpType,
					 LPINT lpResolution)
{
	LPOBJECT lpBase;

	lpBase = GetBase();
	PixmapGetInfoEx(&lpBase->Pixmap, PMT_EDIT, lpXSize, lpYSize, lpDepth, lpType, lpResolution);
}

void CImage::GetTypeInfo(LPFRMTYPEINFO lpTypeInfo)
{
    FrameGetTypeInfo(GetBaseEditFrame(), lpTypeInfo);
}

LPFRAME CImage::GetBaseEditFrame()
{
	return(ObjGetEditFrame(GetBase()));
}

LPFRAME CImage::GetBaseUndoFrame()
{
	return(ObjGetUndoFrame(GetBase()));
}

LPMASK CImage::GetMask()
{
	return(ObjGetAlpha(GetBase()));
}

//  Gets the current mask if any.
//  If no mask exists then create one using fOn and fNoUndo.
//  bCreated is TRUE if a new mask was created. 
LPMASK CImage::GetMaskEx(BOOL fOn, BOOL fNoUndo, LPINT bCreated, LPRECT lpUpdateRect)
{
    LPMASK lpMask;
    LPFRAME lpFrame;
    FRMTYPEINFO TypeInfo;
    FRMDATATYPE FrameType = FDT_GRAYSCALE;

    AstralSetRectEmpty(lpUpdateRect);
    if (lpMask = GetMask())
    {
        if (bCreated)
            *bCreated = FALSE;
        return(lpMask);
    }
    lpFrame = GetBaseEditFrame();
    FrameGetTypeInfo(lpFrame, &TypeInfo);
    if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
       FrameType = FDT_LINEART;
    if (!(lpMask = MaskCreate(NULL, FrameXSize(lpFrame),
                                FrameYSize(lpFrame),
                                fOn, 
                                fNoUndo, FrameType)))
    {
        Message( IDS_EMEMALLOC );
        return(NULL);
    }
	// sync up the resolution of the image data and the mask
	// don't ever rely on the mask's resolution being correct
	// cause other size can change the image data resolution
	// without changing the mask's resolution.  Also, there are
	// other places the mask gets created without syncing up
	// the resolution.  I am doing it here so we can begin
	// to track it correctly (Ted)
	FrameSetResolution(AlphaGetEditFrame(lpMask), FrameResolution(lpFrame));
    if (bCreated)
        *bCreated = TRUE;
    if (!Control.UseMaskAndObjects)
        GetObjectMarqueeRect(this, lpUpdateRect);
    SetMask(lpMask);
    return(lpMask);
}

void CImage::SetMask(LPMASK lpMask)
{
	GetBase()->lpAlpha = lpMask;
}

LPOBJECT CImage::GetDisplayObject()
{
	LPOBJECT lpObject;

	if (GetViewMode() == VM_NORMAL)
    	lpObject = GetBase();
	else if (AltObjList.lpHead) // mask edit mode
    	lpObject = (LPOBJECT)AltObjList.lpHead;
	else
    	lpObject = (LPOBJECT)ObjList.lpHead;
	if (lpObject->RenderType != RT_FRAME)
    	return(NULL);
	else
    	return(lpObject);
}

LPOBJECT CImage::GetSlimeObject()
{
	// slime only displayed when slime is on
	if (GetViewMode() != VM_SLIME)
     	return(NULL);
	// Always use object we are editing for slime
	return(GetBase());
}

void CImage::GetGroupRect(LPOBJECT lpObject, LPRECT lpRect)
{
	LPOBJECT	lpNextObject;

	AstralSetRectEmpty( lpRect );

	lpNextObject = GetBase();
	while( lpNextObject = GetSelObject( lpNextObject ) )
	{
		if( lpObject->wGroupID == lpNextObject->wGroupID )
    	   	AstralUnionRect( lpRect, lpRect, &lpNextObject->rObject );
	}
}
  
BOOL CImage::GetMaskRect(LPRECT lpRect)
{
	LPMASK lpMask;

	if (lpMask = GetMask())
   	{
    	MaskRect(lpMask, lpRect);
    	return(TRUE);
   	}
	else
   	{
    	GetImageRect(lpRect);
    	return(FALSE);
   	}
}

void CImage::GetImageRect(LPRECT lpRect)
{				  
	LPFRAME lpFrame;

	lpFrame = GetBaseEditFrame();
	SetRect(lpRect, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
}

BOOL CImage::GetSelObjectRect( LPRECT lpRect, BOOL bExcludeBase )
{
	LPOBJECT lpObject;
	BOOL bFoundOne;

	AstralSetRectEmpty( lpRect );

	bFoundOne = NO;
	lpObject = ( bExcludeBase ? GetBase() : NULL );
	while ( lpObject = GetSelObject(lpObject) )
   	{
    	AstralUnionRect( lpRect, lpRect, &lpObject->rObject );
    	bFoundOne = YES;
   	}

	return( bFoundOne );
}

BOOL CImage::GetFloatingObjectsRect( LPRECT lpRect )
{
	LPOBJECT lpObject;

	AstralSetRectEmpty( lpRect );

	if ( !(lpObject = GetNextObject(GetBase(), YES, NO) ))
    	return( NO );

	while ( lpObject )
   	{
    	AstralUnionRect( lpRect, lpRect, &lpObject->rObject );
    	lpObject = GetNextObject(lpObject, YES, NO);
   	}
	return( YES );
}

void CImage::GroupSelObjects(BOOL fGroup)
{
WORD wGroupID;
LPOBJECT lpObject = NULL;

if (fGroup)
    wGroupID = GetNextGroupID();
while (lpObject = GetSelObject(lpObject))
    {
    if (fGroup)
        lpObject->wGroupID = wGroupID;
    else
        lpObject->wGroupID = 0;
    }
}

BOOL CImage::IfSelObjectsGrouped()
{
LPOBJECT lpObject;
WORD wGroupID;

lpObject = GetSelObject(NULL);
wGroupID = lpObject->wGroupID;
while (lpObject = GetSelObject(lpObject))
    {
    if (lpObject->wGroupID != wGroupID)
        return(FALSE);
    }
return(wGroupID != 0);
}

BOOL CImage::DisplayMarquees()
{
LPOBJECT lpObject;

// displaying normal marquee
if (IsBusy())
	return(FALSE);
if (GetMarqueeMask())
    return(TRUE);
if (lpObject = GetObjectMarqueeObject())
    return(TRUE);
return(FALSE);
}

LPOBJECT CImage::GetMaskMarqueeObject()
{
LPOBJECT lpObject = NULL;

if (!IsMarqueeVisible(this))
    return(NULL);
if (GetViewMode() == VM_NORMAL)
    {
    // Use mask of object currently being edited
    lpObject = GetBase();
    }
else if (GetViewMode() == VM_SLIME)
    {
    // If in slime mode and we are editing a different object
    // than we are displaying, use object we are editing for marquee
    if (GetBase() != GetDisplayObject())
        lpObject = GetBase();
    }
return(lpObject);
}

LPOBJECT CImage::GetObjectMarqueeObject()
{
LPOBJECT lpObject = NULL;

if (!UseObjectMarquee)
    return(NULL);

if ((Control.UseMaskAndObjects || !GetMask()) &&
    MultipleObjects())
    {
    lpObject = GetSelObject(NULL);
    }
return(lpObject);
}

LPMASK CImage::GetMarqueeMask()
{
LPOBJECT lpObject = NULL;

lpObject = GetMaskMarqueeObject();
if (lpObject)
    return(ObjGetAlpha(lpObject));
else
    return(NULL);
}

BOOL CImage::GetMaskUpdateRect(BOOL fNewMask, BOOL fInvert, LPRECT lpRect)
{
LPMASK lpMask;
LPOBJECT lpObject;
int dx, dy;

if (!(lpObject = GetDisplayObject()))
    return(FALSE);
if (!(lpMask = lpObject->lpAlpha))
    return(FALSE);

if (fNewMask || fInvert)
    {
    if (GetViewMode() == VM_NORMAL)
        return(FALSE);
    if (!fInvert && lpMask->bInvert)
        *lpRect = lpMask->rMask;
    else
        {
        GetInfo(&dx, &dy, NULL, NULL);
        SetRect(lpRect, 0, 0, dx-1, dy-1);
        }
    }
return(TRUE);
}

BOOL CImage::GetUndoState()
{
LPOBJECT lpObject = NULL;
BOOL ObjUndoNotRedo;
int UndoNotRedo = -1;

while (lpObject = GetNextObject(lpObject, YES, NO))
    {
    if (Control.UndoObjects && !IsSelectedObject(lpObject))
        continue;
    if (lpObject->DataDirty)
        ObjUndoNotRedo = lpObject->DataUndoNotRedo;
    else if ((lpObject != GetBase()) && lpObject->lpAlpha &&
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

int CImage::GetDirtyState()
{
LPOBJECT lpObject = NULL;
int dirty = NO;
int ObjDirty;

while (lpObject = GetNextObject(lpObject, YES, NO))
    {
    if (Control.UndoObjects && !IsSelectedObject(lpObject))
        continue;
    if (lpObject->DataDirty)
        ObjDirty = lpObject->DataDirty;
    else if ((lpObject != GetBase()) && lpObject->lpAlpha &&
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

BOOL CImage::Changed()
{
    return( fChanged );
}

void CImage::SetChanged(BOOL fChangedIn)
{
	fChanged = fChangedIn;
}

void CImage::DeleteObjects(LPOBJECT lpObject, BOOL bOnlyIfSelected)
{
LPOBJECT lpNext;
BOOL fDeleteAll;

fDeleteAll = (lpObject == NULL) && !bOnlyIfSelected;

if (!lpObject)
    lpObject = GetBase();

while (lpObject)
    {
    lpNext = (LPOBJECT)lpObject->lpNext;
    if ( !bOnlyIfSelected || lpObject->fSelected )
        DeleteObject( lpObject );
    lpObject = lpNext;
    }
if (AltObjList.lpHead && fDeleteAll)
    {
	lpObject = AltObjList.lpHead;
    while (lpObject)
        {
        lpNext = (LPOBJECT)lpObject->lpNext;
		ObjDeleteObject(&AltObjList, lpObject);
        lpObject = lpNext;
        }
    }
}

void CImage::PurgeObjects()
{
LPOBJECT lpNext, lpObject;

lpObject = GetBase();
while (lpObject)
    {
    lpNext = (LPOBJECT)lpObject->lpNext;
    if ( lpObject->fDeleted )
        DeleteObject( lpObject );
    lpObject = lpNext;
    }
}

void CImage::DeleteObject(LPOBJECT lpObject )
{
	if ( lpObject )
	{
		ObjDeleteObject(&ObjList, (LPPRIMOBJECT)lpObject);
		SetupMiniViews(NULL, NO);
	}
}

BOOL CImage::InMaskEditMode()
{
return(AltObjList.lpHead != NULL);
}

void CImage::SetViewMode(VIEW_MODE ViewMode)
{
if (m_ViewMode == ViewMode)
    return;
m_ViewMode = ViewMode;
if (InMaskEditMode())
    {
    ImgColorMapChanged(this);
	ImgInitDisplay(this);
	CServerDoc *pDoc = PictPubApp.GetDocument(this);
	if (pDoc)
		{
		CServerView *pView = pDoc->GetActiveView();
		if (pView)
	    	pView->SetupImageDisplayPalette();
		}
   }
UpdateImage(this, NULL, YES);
}

LPOBJECT CImage::AddFrameObject(LPFRAME lpFrame, LPMASK lpAlpha, LPRECT lpRect)
{
LPOBJECT lpObject, lpBase;
RECT    rObject;

lpBase = GetBase();
if (lpRect)
    rObject = *lpRect;
else
    SetRect(&rObject, 0, 0, FrameXSize(lpFrame)-1, FrameYSize(lpFrame)-1);
lpObject = ObjCreateFromFrame(ST_PERMANENT, lpFrame, lpAlpha,
                            &rObject, Control.NoUndo);
if (lpObject)
    ObjAddTail(&ObjList, (LPPRIMOBJECT)lpObject);
if (lpBase)
	{
	int res = FrameResolution(ObjGetEditFrame(lpBase));
	FrameSetResolution(ObjGetEditFrame(lpObject), res);
	if (lpObject->lpAlpha)
		FrameSetResolution(AlphaGetEditFrame(lpObject->lpAlpha), res);
	}
return(lpObject);
}

void CImage::AddNewObject(LPOBJECT lpObject, BOOL fSetObjRes)
{
LPOBJECT lpBase = GetBase();
lpObject->dwObjectID = ++dwObjectID;
ObjAddTail( &ObjList, (LPPRIMOBJECT)lpObject ); // Add to the image's list
ObjDeselectAll(&ObjList); // Deselect anything currently selected
ObjSelectObject( (LPPRIMOBJECT)lpObject, YES ); // Select the new object
if (lpBase)
	{
	if(fSetObjRes)
		{
		int res = FrameResolution(ObjGetEditFrame(lpBase));
		FrameSetResolution(ObjGetEditFrame(lpObject), res);
		if (lpObject->lpAlpha)
			FrameSetResolution(AlphaGetEditFrame(lpObject->lpAlpha), res);
		}
	}
}

void CImage::DeleteTempObjects()
{
ObjDeleteObjects(&ObjList, ST_TEMPORARY);
SetupMiniViews(NULL, NO);
}

void CImage::KeepFrameTempObjects()
{
LPOBJECT lpObject = NULL;

while (lpObject = GetNextObject(lpObject, YES, NO))
    {
    if ( lpObject->StorageType == ST_TEMPORARY &&
         lpObject->ObjectType == OT_FRAME )
            lpObject->StorageType = ST_PERMANENT;
    }
}

WORD CImage::GetNextGroupID()
{
LPOBJECT lpObject = NULL;
WORD wGroupID = 0;

while (lpObject = GetNextObject(lpObject, YES, NO))
    {
    if (lpObject->wGroupID > wGroupID)
        wGroupID = lpObject->wGroupID;
    }
return(wGroupID+1);
}

BOOL CImage::IsBusy()
{
	#ifdef WIN32
	// if no command list, image must be busy
	if (!lpCmdList)
    	return(TRUE);
	// if no command event, image is never busy (no threading)
	if (!lpCmdList->ThreadData.ThreadInfo.hCmdEvent) // no event obj
    	return(FALSE);
	// see if event object is signalled or not
	return(WaitForSingleObject(lpCmdList->ThreadData.ThreadInfo.hCmdEvent, 0) == WAIT_OBJECT_0);
	#else
	return(FALSE);
	#endif
}

LPSTORAGE CImage::GetRootCompoundStorage(BOOL *pbTempStg) 
{
	if (pServerDoc)
		return(pServerDoc->GetRootCompoundStorage(pbTempStg));
	else
		return(NULL);
}

BOOL CImage::WriteSummaryInfo()
{
	if (pServerDoc)
		return(pServerDoc->WriteSummaryInfo());
	else
		return(FALSE);
}

