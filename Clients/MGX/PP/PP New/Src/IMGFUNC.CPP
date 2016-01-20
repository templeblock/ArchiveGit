//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "objutil.h"

BOOL CImage::CombineObjects(BOOL fNoUndo, BOOL fCreateMask,
                        BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect)
{
LPOBJECT lpBase, lpObject;
int     iWidth, iHeight, depth, y, iImageWidth, iImageHeight;
LPTR    lpDataBuf, lpMaskPtr;
RECT    rFloaters, rMask, rTemp;
BOOL fNewMask;
LPMASK lpMask;
UNDO_TYPE UndoType;

AstralSetRectEmpty(lpUpdateRect);

// get rectangle for all floating objects
if (!GetFloatingObjectsRect(&rFloaters))
    return(FALSE);

// Get base object
lpBase = GetBase();

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
    {
    lpMask = GetMaskEx(OFF, Control.NoUndo, &fNewMask, &rTemp);
    AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
    }


// do it to it
ProgressBegin(lpMask != NULL ? 2 : 1, idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

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
    if (!ImgEditInit(this, ET_OBJECT, UndoType, lpBase))
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
    ImgGetLine(this, NULL, rFloaters.left, y, iWidth, lpDataBuf);
    PixmapWrite(&lpBase->Pixmap, PMT_EDIT, rFloaters.left, y, iWidth,
                lpDataBuf, iWidth);
    }
ProgressEnd();

if (lpMask && (lpObject = GetNextObject(lpBase, YES, NO)))
    {
    ProgressBegin(1, 0);
    for (y = rFloaters.top; y <= rFloaters.bottom; ++y)
        {
        AstralClockCursor(y-rFloaters.top, iHeight, NO);
        lpMaskPtr = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, rFloaters.left,
                            y, YES);
        if (!lpMaskPtr)
            continue;
        ImgMaskLoad(this, lpObject, NULL,
                rFloaters.left, y, iWidth, lpDataBuf, NO,
                CR_OR, 0, 0);
        CombineData(lpDataBuf, lpMaskPtr, iWidth, NO, CR_OR);
        }
    MaskRectUpdate(lpMask, &rMask); 
    ProgressEnd();
    }

FreeUp(lpDataBuf);

GetObjectMarqueeRect(this, &rTemp);
AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
lpObject = lpBase;
while (lpObject = GetNextObject(lpObject, YES, NO))
    {
// always let undo stuff initialize this
//  lpObject->fUndoDeleted = NO;
    lpObject->fDeleted = YES;
    }
//DeleteObjects((LPOBJECT)lpBase->lpNext, NO);

if (!fNoUndo)
    ImgEditedObject(this, lpBase, idDirty, &rFloaters);
else
    fChanged = YES;

//if ( Tool.hRibbon )
//  SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
ProgressEnd();
//SetupMiniViews(NULL, NO);
return(TRUE);
}

BOOL CImage::CombineSelObjects(BOOL fNoUndo, BOOL fCreateMask,
                        BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect)
{
LPOBJECT lpBase, lpObject;
int      iWidth, iHeight, depth, y, iImageWidth, iImageHeight, oWidth;
LPTR     lpDataBuf, lpMaskPtr;
RECT     rSelected, rMask, rTemp;
BOOL     fNewMask;
LPMASK   lpMask;
UNDO_TYPE UndoType;

AstralSetRectEmpty(lpUpdateRect);

// get rectangle for all floating objects
if (!GetSelObjectRect(&rSelected, YES))
    return(FALSE);

// Get base object
lpBase = lpObject = GetBase();

// get pixmap information
PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, &iImageWidth, &iImageHeight, &depth, NULL);
BoundRect(&rSelected, 0, 0, iImageWidth-1, iImageHeight-1);

iWidth = RectWidth(&rSelected);
iHeight = RectHeight(&rSelected);

// allocate buffer for alpha channel combining
if (!(lpDataBuf = Alloc((long)iWidth*depth)))
    {
    Message(IDS_EMEMALLOC);
    return(FALSE);
    }

lpMask = NULL;
if (fCreateMask)
    {
    lpMask = GetMaskEx(OFF, Control.NoUndo, &fNewMask, &rTemp);
    AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
    }


// do it to it
ProgressBegin(lpMask != NULL ? 2 : 1, idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

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
    if (!ImgEditInit(this, ET_OBJECT, UndoType, lpBase))
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
while( lpObject = GetSelObject( lpObject ) )
	{
   rTemp = lpObject->rObject;
   BoundRect(&rTemp, 0, 0, iImageWidth-1, iImageHeight-1);
	oWidth = RectWidth( &rTemp );
	for (y = rTemp.top; y <= rTemp.bottom; ++y)
    	{
    	AstralClockCursor(y-rTemp.top, rTemp.bottom, NO);
      	ImgGetLine(this, NULL, rTemp.left, y, oWidth,
                lpDataBuf, NULL, TRUE);
    	PixmapWrite(&lpBase->Pixmap, PMT_EDIT, rTemp.left, y,
			oWidth, lpDataBuf, oWidth);
    	ImgEditedObject(this, lpObject, idDirty, &rTemp);
    	}
	}
ProgressEnd();

lpObject = lpBase;

if (lpMask && (lpObject = GetSelObject(lpObject)))
    {
    ProgressBegin(1, 0);
    for (y = rSelected.top; y <= rSelected.bottom; ++y)
        {
        AstralClockCursor(y-rSelected.top, iHeight, NO);
        lpMaskPtr = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, rSelected.left,
                            y, YES);
        if (!lpMaskPtr)
            continue;
        ImgMaskLoad(this, lpObject, NULL,
                rSelected.left, y, iWidth, lpDataBuf, YES,
                CR_OR, 0, 0);
        CombineData(lpDataBuf, lpMaskPtr, iWidth, NO, CR_OR);
        }
    MaskRectUpdate(lpMask, &rMask); 
    ProgressEnd();
    }

FreeUp(lpDataBuf);

GetObjectMarqueeRect(this, &rTemp);
AstralUnionRect(lpUpdateRect, lpUpdateRect, &rTemp);
lpObject = lpBase;
while (lpObject = GetSelObject(lpObject))
    {
// always let undo stuff initialize this
//  lpObject->fUndoDeleted = NO;
    lpObject->fDeleted = YES;
    }
//DeleteObjects((LPOBJECT)lpBase->lpNext, NO);

if (!fNoUndo)
    ImgEditedObject(this, lpBase, idDirty, &rSelected);
else
    fChanged = YES;

//if ( Tool.hRibbon )
//  SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
ProgressEnd();
//SetupMiniViews(NULL, NO);
return(TRUE);
}

BOOL CImage::SetupMaskEditMode()
{
CMD_PARMS parms;
LPOBJECT lpOldBase;
COLORINFO Color;
static COLORINFO SaveActive, SaveAlternate;

if ( !(lpOldBase = (LPOBJECT)AltObjList.lpHead) )
    { // turn on mask editing mode
    GetActiveColor(&SaveActive);
    GetAlternateColor(&SaveAlternate);
    Color.gray = 255;
    SetActiveColor(&Color, CS_GRAY, NO);
    Color.gray = 0;
    SetAlternateColor(&Color, CS_GRAY, NO);
    }
else
    { // turn off mask editing mode
    SetActiveColor(&SaveActive, SaveActive.ColorSpace, NO);
    SetAlternateColor(&SaveAlternate, SaveAlternate.ColorSpace, NO);
    }
ProcessCommand(lpCmdList, IDS_CMD_ALPHAEDIT, &parms);
return(TRUE);
}

BOOL CImage::Crop( LPRECT lpCropRect )
{

if (lpCropRect)
	{
	int res;
	CROP_PARMS parms;

	GetInfo(NULL, NULL, NULL, NULL, &res);
	parms.iBaseRes = res;
	parms.CropRect = *lpCropRect;
	return(ProcessCommand(lpCmdList, IDS_CMD_CROP, &parms));
	}
else
	{
	CMD_PARMS parms;

	return(ProcessCommand(lpCmdList, IDS_CMD_CROPMASK, &parms));
	}
}

BOOL CImage::DuplicateSelObjects()
{
LPOBJECT lpDupObject;
OBJECTLIST DupList;
LPOBJECT lpObject;
RECT rUpdate;
WORD wGroupID, wOldGroupID, wLastGroupID;

if (!ImgEditInit( this, ET_OBJECT, UT_DELETEOBJECTS, GetBase() ))
    return(FALSE);

DupList.lpHead = DupList.lpTail = NULL;
lpObject = NULL;
while (lpObject = GetSelObject(lpObject))
    {
    if (!(lpDupObject = ObjDuplicateObject(lpObject)))
        {
        ObjDeleteObjects(&DupList, ST_ALL);
        return(FALSE);
        }
    ObjAddTail(&DupList, (LPPRIMOBJECT)lpDupObject);
    }
wLastGroupID = wGroupID = GetNextGroupID();
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

RemoveObjectMarquee(this);
ObjDeselectAll(&ObjList);
while (lpObject = (LPOBJECT)ObjGetNextObject(&DupList, NULL, YES))
    {
    ObjUnlinkObject(&DupList, (LPPRIMOBJECT)lpObject);
    ObjSelectObject( (LPPRIMOBJECT)lpObject, YES ); // Select the new object
    ObjAddTail( &ObjList, (LPPRIMOBJECT)lpObject ); // Add to the image's list
    lpObject->fUndoDeleted = YES;
    }
ImgEditedObject(this, GetBase(), IDS_UNDODUPLICATE, NULL);

GetSelObjectRect(&rUpdate, NO);
UpdateImage(this, &rUpdate, YES);
SetupMiniViews(NULL, NO);
return(TRUE);
}


