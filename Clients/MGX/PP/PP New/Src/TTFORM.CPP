//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "objutil.h"
#include "id.h"

// Static prototypes
LOCAL void TransformStartMaskProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
LOCAL void TransformStartCutProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
LOCAL void TransformStartCopyProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
LOCAL STATUS_CODE TransformEndCreateMaskFromObject(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
LOCAL STATUS_CODE TransformEndMaskProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
LOCAL STATUS_CODE TransformEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
LOCAL STATUS_CODE TransformEndCutProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
LOCAL STATUS_CODE TransformEndLoadMaskProc(LPIMAGE lpImage, BOOL fCancel,  BOOL fDelete,BOOL fMoveOnly);
LOCAL BOOL GetMaskObjectRect(LPIMAGE lpImage, LPRECT lpRect);
LOCAL BOOL TransformRefresh( HWND hWnd, int iCode );
LOCAL BOOL TransformObjectsInit( LPIMAGE lpImage, BOOL bInit, LPDISTORT lpDistort );

LOCAL STATUS_CODE TransformObjectsExecute( LPIMAGE lpImage, LPDISTORT lpDistort,
									BOOL fHiQuality, BOOL fPasteInto,
									BOOL fTransformDoUndo,
									int iTransformNewObject,
									LPRECT lpCutArea,
									ITEMID idDirty);
LOCAL STATUS_CODE TransformObject( LPIMAGE lpImage, LPOBJECT lpObject,
							LPDISTORT lpDistort, BOOL fHiQuality,
							BOOL fTransformDoUndo,
							int iTransformNewObject,
							LPRECT lpCutArea,
							ITEMID idDirty);
LOCAL STATUS_CODE TransformDataImage(LPIMAGE lpImage, LPDISTORT lpDistort, BOOL fCut,
					LPRECT lpSelectRect,
					int Opacity, MERGE_MODE MergeMode,
					BOOL fHiQuality,
					BOOL fPasteInto,
					ITEMID idDirty, LPRECT lpUpdateRect);
LOCAL STATUS_CODE TransformDataStart(LPIMAGE lpImage, LPRECT lpSelectRect,
									BOOL fCut, LPPOBJECT lppCutObject,
									LPRECT lpUpdateRect);
LOCAL STATUS_CODE TransformDataEnd(LPIMAGE lpImage,
						LPDISTORT lpDistort, BOOL fCut, LPOBJECT lpCutObject,
						BOOL fHiQuality, BOOL fPasteInto,
						BOOL fCancel,
						LPRECT lpSelectRect,
						ITEMID idDirty, LPRECT lpUpdateRect);
LOCAL STATUS_CODE TransformMaskImage(LPIMAGE lpImage, LPDISTORT lpDistort, BOOL fCut,	LPRECT lpSelectRect, int Mode, BOOL fHiQuality, BOOL fDelete, ITEMID idDirty, LPRECT lpUpdateRect);
LOCAL STATUS_CODE TransformMaskStart(LPIMAGE lpImage, LPRECT lpSelectRect, BOOL fCut, LPRECT lpUpdateRect);
LOCAL STATUS_CODE TransformMaskEnd(LPIMAGE lpImage,
						LPDISTORT lpDistort, BOOL fCut, int Mode,
						BOOL fHighQuality, 
						BOOL fCancel,
						BOOL fDelete,
						LPRECT lpSelectRect,
						ITEMID idDirty, LPRECT lpUpdateRect);
LOCAL STATUS_CODE LoadMaskEnd(LPIMAGE lpImage, BOOL fCancel, LPMASK lpOldMask,
						int Mode, ITEMID idDirty, LPDISTORT lpDistort, LPRECT lpUpdateRect);

LOCAL BOOL TransformSingleLine( LPOBJECT lpObject, int x1, int y, int dx, LPFRAME lpDataFrame, LPTR lpDataBuf, int iDataDepth, LPDISTORT lpDistort, BOOL fHiQuality );

class CObjectDrawHook : public CHookHandler
{
public:
    CObjectDrawHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CObjectDrawHook ObjectDrawHook;


LOCAL BOOL NeedToTransform(BOOL fMoveOnly, BOOL fPasteInto);
LOCAL BOOL IsMoveOnly(LPIMAGE lpImage, LPDISTORT lpDistort);
LOCAL BOOL TransformEditInit(LPIMAGE lpImage, BOOL iNewObject, BOOL fDoUndo );

LOCAL void res_convert_disort(int iBaseRes,int res, LPPOINT pDistortOut, LPPOINT pDistortIn); 

//	These are now MACROS defined at the bottom of this file.
LOCAL BOOL IntersectCorners( LPDISTORT lpDistort, int y, LPLFIXED lpX1, LPLFIXED lpX2,
	LPLFIXED lpFx1, LPLFIXED lpFy1 , LPLFIXED lpFx2, LPLFIXED lpFy2 );
LOCAL BOOL IntersectSegment( int y, POINT pa, POINT pb, LPLFIXED lpX, LPLFIXED lpPercent );

#define OPACITY Mask.TransformOpacity
#define MERGEMODE Mask.TransformMergeMode
#define MASKMODES Mask.iTransformModes

#define TF_START	1
#define TF_DURING	2
#define TF_END		3
#define TF_CANCEL	4

#define MINX		1
#define MAXX		2

#define NO_NEWOBJECT	NO
#define NEW_OBJECT		YES
#define NEW_CUTOBJECT	2

// Static data
static DISTORT Distort;
static LPTRANSFORMENDPROC lpEndProc;
static BOOL fTransformDoUndo, fPasteIntoMask;
static FNAME szTransformFileName;
static DWORD dwTransformClipOwner;
static FRMTYPEINFO TransformSrcTypeInfo;
static BOOL bTransformExtName;
static int iTransformNewObject;
static BOOL bFlipX, bFlipY;
static int idTransformDirty;

// LOCAL prototypes
LOCAL BOOL Transformer_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Transformer_OnDocActivated(HWND hDlg);
LOCAL void Transformer_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Transformer_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Transformer_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL TransformImage(LPIMAGE lpImage, LPTRANSFORMER_PARMS lpParms)
/***********************************************************************/
{
BOOL fHiQuality;
RECT SelectRect, rUpdate;
DISTORT Distort;
int res;
UPDATE_TYPE UpdateType;
FRMTYPEINFO TypeInfo;

ImgGetTypeInfo(lpImage, &TypeInfo);
fHiQuality = lpParms->fHiQuality;
if (TypeInfo.DataType == FDT_PALETTECOLOR ||
	TypeInfo.DataType == FDT_LINEART)
   fHiQuality = FALSE;
AstralSetRectEmpty(&rUpdate);
res = FrameResolution(ImgGetBaseEditFrame(lpImage));
SelectRect = lpParms->SelectRect;
ResConvertRect(lpParms->iBaseRes, res, &SelectRect);
res_convert_disort(lpParms->iBaseRes, res, Distort.p, lpParms->DistortPoints); 
switch (lpParms->Function)
	{
	case IDC_COPYMASK:
	case IDC_MOVEMASK:
		UpdateType = UT_ACTIVEAREA;
		lpParms->Common.StatusCode = TransformMaskImage(lpImage, &Distort,
						lpParms->Function == IDC_MOVEMASK,
						&SelectRect, lpParms->Mode, fHiQuality,
						lpParms->fDelete,
						lpParms->Common.idDirty, &rUpdate);
		break;
	case IDC_COPYIMAGE:
	case IDC_MOVEIMAGE:
		UpdateType = UT_AREA;
		lpParms->Common.StatusCode = TransformDataImage(lpImage, &Distort,
						lpParms->Function == IDC_MOVEIMAGE,
						&SelectRect,
						lpParms->TransformOpacity,
						lpParms->TransformMergeMode,
						fHiQuality,
						lpParms->fPasteInto,
						lpParms->Common.idDirty, &rUpdate);
		break;
	default:
		break;
	}
ImgDeleteTempObjects(lpImage);
if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UpdateType;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/***********************************************************************/
BOOL TransformObjectsImage(LPIMAGE lpImage, LPTRANSFORMOBJECTS_PARMS lpParms)
/***********************************************************************/
{
BOOL fHiQuality;
RECT rUpdate, rObjects;
DISTORT Distort;
int res;
LPOBJECT lpObject = NULL;
FRMTYPEINFO TypeInfo;

ImgGetTypeInfo(lpImage, &TypeInfo);
fHiQuality = lpParms->fHiQuality;
if (TypeInfo.DataType == FDT_PALETTECOLOR ||
	TypeInfo.DataType == FDT_LINEART)
   fHiQuality = FALSE;

AstralSetRectEmpty(&rUpdate);
res = FrameResolution(ImgGetBaseEditFrame(lpImage));
res_convert_disort(lpParms->iBaseRes, res, Distort.p, lpParms->DistortPoints); 

if (!TransformEditInit(lpImage, NO, YES ))
	return(FALSE);

ImgGetSelObjectRect(lpImage, &Distort.RectOrig, YES);
TransformObjectsInit( lpImage, YES, &Distort);
lpParms->Common.StatusCode = TransformObjectsExecute( lpImage, &Distort, fHiQuality,
						NO,	YES, NO, NULL, lpParms->Common.idDirty);
if (lpParms->Common.StatusCode == SC_SUCCESS)
	ImgKeepFrameTempObjects(lpImage);
ImgGetSelObjectRect(lpImage, &rObjects, YES);
AstralUnionRect(&rUpdate, &rUpdate, &rObjects);
AstralUnionRect(&rUpdate, &rUpdate, &Distort.RectOrig);
ImgDeleteTempObjects(lpImage);
if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/***********************************************************************/
BOOL PasteFromImage(LPIMAGE lpImage, LPPASTEFROM_PARMS lpParms)
/***********************************************************************/
{
	BOOL fHiQuality;
	RECT rUpdate, rObjects;
	DISTORT Distort;
	int res, i;
	LPOBJECT lpObject;
	FNAME szImageFile, szMaskFile;
	FRMTYPEINFO TypeInfo, SrcTypeInfo;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	fHiQuality = lpParms->fHiQuality;
	if (TypeInfo.DataType == FDT_PALETTECOLOR ||
		TypeInfo.DataType == FDT_LINEART)
   	fHiQuality = FALSE;

	if (lstrlen(lpParms->szClipboard))
	{
		if (!LookupExtFileN(lpParms->szClipboard, szImageFile, IDN_CLIPBOARD, NO))
		{
			Message(IDS_EOPEN, (LPSTR)lpParms->szClipboard);
			return(FALSE);
		}
		if (!LookupExtFileN(lpParms->szClipboard, szMaskFile, IDN_MASK, NO))
			szMaskFile[0] = '\0';
	}
	else
	if (lstrlen(lpParms->szClipFileName))
	{
		lstrcpy(szImageFile, lpParms->szClipFileName);
		szMaskFile[0] = '\0';
	}
	else
		return(FALSE);

	GetObjectMarqueeRect(lpImage, &rUpdate);

   	if ( !(lpObject = ImgCreateClipInObject( lpImage,
	   	szImageFile, szMaskFile, Control.CMSEnabled, &SrcTypeInfo, &lpParms->cmsInfo.src)) )
		{
		return(FALSE);
		}
	// set source so we don't have to ask again
	lpParms->cmsInfo.src = SrcTypeInfo;

	lpObject->StorageType = ST_TEMPORARY;
	ImgAddNewObject(lpImage, lpObject);
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	res_convert_disort(lpParms->iBaseRes, res, Distort.p, lpParms->DistortPoints); 

	if (TransformEditInit(lpImage, YES, YES ))
	{
		ImgGetSelObjectRect(lpImage, &Distort.RectOrig, YES);
		TransformObjectsInit( lpImage, YES, &Distort );
		lpParms->Common.StatusCode = TransformObjectsExecute( lpImage, &Distort, fHiQuality,
								lpParms->fPasteInto, YES, YES, NULL,
								lpParms->Common.idDirty);
		if (lpParms->Common.StatusCode == SC_SUCCESS)
		{
			ImgKeepFrameTempObjects(lpImage);
			lpObject->Opacity = lpParms->TransformOpacity;
			lpObject->MergeMode = lpParms->TransformMergeMode;
		}
		ImgGetSelObjectRect(lpImage, &rObjects, YES);
		AstralUnionRect(&rUpdate, &rUpdate, &rObjects);
		AstralUnionRect(&rUpdate, &rUpdate, &Distort.RectOrig);
	}
	else
		lpParms->Common.StatusCode = SC_MEMERROR;
	ImgDeleteTempObjects(lpImage);
	if (!AstralIsRectEmpty(&rUpdate))
	{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/***********************************************************************/
BOOL PasteImage(LPIMAGE lpImage, LPPASTE_PARMS lpParms)
/***********************************************************************/
{
BOOL fHiQuality;
RECT rUpdate, rObjects;
DISTORT Distort;
int res, i;
LPOBJECT lpObject;
FRMTYPEINFO TypeInfo;

ImgGetTypeInfo(lpImage, &TypeInfo);
fHiQuality = lpParms->fHiQuality;
if (TypeInfo.DataType == FDT_PALETTECOLOR ||
	TypeInfo.DataType == FDT_LINEART)
   fHiQuality = FALSE;

// Other formats need to create the file, therefore we stuff the path
// strings first
GetObjectMarqueeRect(lpImage, &rUpdate);
if (lpImage->lpClipObject)
	lpObject = ObjDuplicateObject(lpImage->lpClipObject, TypeInfo);
else
	lpObject = GetClipboardObject(lpImage, NULL, NULL);

if (!lpObject)
	return(FALSE);

lpObject->StorageType = ST_TEMPORARY;
ImgAddNewObject(lpImage, lpObject);
res = FrameResolution(ImgGetBaseEditFrame(lpImage));
res_convert_disort(lpParms->iBaseRes, res, Distort.p, lpParms->DistortPoints); 

if (TransformEditInit(lpImage, YES, YES ))
	{
	ImgGetSelObjectRect(lpImage, &Distort.RectOrig, YES);
	TransformObjectsInit( lpImage, YES, &Distort );
	lpParms->Common.StatusCode = TransformObjectsExecute( lpImage, &Distort, fHiQuality,
							lpParms->fPasteInto, YES, YES, NULL,
							lpParms->Common.idDirty);
	if (lpParms->Common.StatusCode == SC_SUCCESS)
		{
		ImgKeepFrameTempObjects(lpImage);
		lpObject->Opacity = lpParms->TransformOpacity;
		lpObject->MergeMode = lpParms->TransformMergeMode;
		}
	ImgGetSelObjectRect(lpImage, &rObjects, YES);
	AstralUnionRect(&rUpdate, &rUpdate, &rObjects);
	AstralUnionRect(&rUpdate, &rUpdate, &Distort.RectOrig);
	}
else
	lpParms->Common.StatusCode = SC_MEMERROR;
ImgDeleteTempObjects(lpImage);
if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/***********************************************************************/
BOOL LoadMaskImage(LPIMAGE lpImage, LPLOADMASK_PARMS lpParms)
/***********************************************************************/
{
RECT rUpdate, rObjects;
DISTORT Distort;
int res, i;
LPOBJECT lpObject = NULL;
FNAME szFileName;
LPALPHA lpAlpha;
LPMASK lpMask, lpOldMask;
BOOL fTransform;

if (lstrlen(lpParms->szMask))
	{
	if (!LookupExtFileN(lpParms->szMask, szFileName, IDN_MASK, NO))
		{
		Message(IDS_EOPEN, (LPSTR)lpParms->szMask);
		return(FALSE);
		}
	}
else
if (lstrlen(lpParms->szMaskFileName))
	{
	lstrcpy(szFileName, lpParms->szMaskFileName);
//	FixExportFileName(lpImage->CurFile, szImageFile);
	}
else
	return(FALSE);

if (!(lpAlpha = ReadMask(lpImage, szFileName, 0, 0)))
	return(FALSE);

GetObjectMarqueeRect(lpImage, &rUpdate);
if (!LoadMaskStart(lpImage, lpAlpha, lpParms->fDeleteMask, YES, &rUpdate, 
						&fTransform))
	return(FALSE);

res = FrameResolution(ImgGetBaseEditFrame(lpImage));
res_convert_disort(lpParms->iBaseRes, res, Distort.p, lpParms->DistortPoints); 

lpOldMask = NULL;
lpMask = ImgGetMask(lpImage);
if (lpParms->fDeleteMask && lpMask)
	{
	GetMaskMarqueeRect(lpImage, &rObjects);
	AstralUnionRect(&rUpdate, &rUpdate, &rObjects);
	ImgSetMask(lpImage, NULL);
	lpOldMask = lpMask;
	}

ImgGetSelObjectRect(lpImage, &Distort.RectOrig, YES);
TransformObjectsInit( lpImage, YES, &Distort );

lpParms->Common.StatusCode = LoadMaskEnd(lpImage, NO, lpOldMask, lpParms->Mode, lpParms->Common.idDirty, &Distort, &rUpdate);
ImgDeleteTempObjects(lpImage);

if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(lpParms->Common.StatusCode == SC_SUCCESS);
}

/***********************************************************************/
BOOL LoadMaskStart(LPIMAGE lpImage, LPALPHA lpAlpha, BOOL fDeleteMask,
					BOOL fTransform, LPRECT lpUpdateRect, LPBOOL lpfTransform)
/***********************************************************************/
{
LPOBJECT lpBase, lpObject;
LPFRAME lpFrame;
int iWidth, iHeight, w, h;
LPMASK lpOldMask;
RECT rAlpha;

SaveSelectedObjects(lpImage);
ImgGetInfo(lpImage, &iWidth, &iHeight, NULL, NULL);
lpOldMask = ImgGetMask(lpImage);
w = FrameXSize(lpAlpha->Pixmap.EditFrame);
h = FrameYSize(lpAlpha->Pixmap.EditFrame);
if (w == iWidth && h == iHeight &&
	(fDeleteMask || !lpOldMask) && !fTransform)
	{
	lpBase = ImgGetBase(lpImage);
	GetMaskMarqueeRect(lpImage, &rAlpha);
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rAlpha);
	if (!lpOldMask)
		ImgSetMask(lpImage, lpAlpha);

	if (!ImgEditInit(lpImage, ET_OBJECT,
				lpOldMask != NULL ? UT_NEWALPHA : UT_CREATEMASK, lpBase))
		{
		MaskClose(lpAlpha);
		return(FALSE);
		}
	if (lpOldMask)
		{
		lpFrame = lpAlpha->Pixmap.EditFrame;
		lpAlpha->Pixmap.EditFrame = NULL;
		MaskClose(lpAlpha);
		SetRect(&rAlpha, 0, 0, iWidth-1, iHeight-1);
		ImgEditedObjectFrame(lpImage, lpBase, IDS_UNDOLOADMASK,
									&rAlpha, NULL, lpFrame);
		}
	else
		ImgEditedObject(lpImage, lpBase, IDS_UNDOLOADMASK, NULL);

	MaskRectUpdate(ImgGetMask(lpImage), &rAlpha);

	// update appropriate area of the display
	if (ImgGetMaskUpdateRect(lpImage, lpOldMask == NULL, NO, &rAlpha))
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rAlpha);

	*lpfTransform = FALSE;
	}
else
	{
	rAlpha.left = rAlpha.top = 0;
	rAlpha.right = w - 1;
	rAlpha.bottom = h - 1;
	if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, NULL, lpAlpha,
				&rAlpha, Control.NoUndo )) )
		{
		Message(IDS_EMEMALLOC);
		MaskClose( lpAlpha );
		return(FALSE);
		}
	ImgAddNewObject(lpImage, lpObject);
	*lpfTransform = TRUE;
	}
return(TRUE);
}

/************************************************************************/
LOCAL STATUS_CODE LoadMaskEnd(LPIMAGE lpImage, BOOL fCancel, LPMASK lpOldMask,
						int Mode, ITEMID idDirty, LPDISTORT lpDistort,
						LPRECT lpUpdateRect)
/************************************************************************/
{
LPOBJECT lpObject = NULL, lpBase;
LPFRAME lpMaskFrame;
LPPIXMAP lpPixmap;
LPMASK lpMask;
LPALPHA lpAlpha;
LPTR lpSrc, lpDst;
int y, ox, oy, nPixels, nWidth, dx;
RECT rSect, rEdit, rUpdate;
BOOL bInvert, fNewMask, fClearMask;
STATUS_CODE StatusCode;
COMBINE_ROP ROP;

// setup mask pointer back cause we always need it
if (fClearMask = (lpOldMask != NULL))
	{
	ImgSetMask(lpImage, lpOldMask);
	lpMask = lpOldMask;
	}
else
	lpMask = ImgGetMask(lpImage);

// if cancelling, reset transform flag and delete mask object
if (fCancel)
	{
	TransformObjectsInit( lpImage, NO, NULL );
	return(SC_CANCELLED);
	}

ProgressBegin(2, idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
// transform mask object, delete if an error occurred
if ( StatusCode = TransformObjectsExecute( lpImage, lpDistort, Mask.PasteQuality,
		NO, NO, NO,	NULL, idDirty) )
	{
	ProgressEnd();
	return(StatusCode);
	}

// find an object without a data frame
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (!ObjGetEditFrame(lpObject))
		break;
	}

// no object found?
if (!lpObject)
	{
	ProgressEnd();
	return(SC_MEMERROR);
	}

// get the base
lpBase = ImgGetBase(lpImage);

// if no alpha in the object or if it doesn't intersect the image get out
lpAlpha = ObjGetAlpha(lpObject);
if (!lpAlpha ||
	!AstralIntersectRect(&rSect, &lpBase->rObject, &lpObject->rObject))
	{
	ProgressEnd();
	return(SC_MEMERROR);
	}

// get mask pointer, create a new mask if one doesn't exist
fNewMask = NO;
if (!lpMask &&
	!(lpMask = ImgGetMaskEx(lpImage, NO, Control.NoUndo, &fNewMask, &rUpdate)))
	{
	ProgressEnd();
	return(SC_MEMERROR);
	}

// initialize undos
if (!ImgEditInit(lpImage, ET_OBJECT, fNewMask ? UT_CREATEMASK:UT_ALPHA, lpBase))
  	{
	ProgressEnd();
	return(SC_MEMERROR);
  	}

bInvert = lpAlpha->bInvert;
if (Mode == SHAPE_REVERSE)
	ROP = CR_XOR;
else
if (Mode == SHAPE_SUBTRACT)
	{
	ROP = CR_AND;
	bInvert = !bInvert;
	}
else
//if (Mode == SHAPE_ADD)
	ROP = CR_OR;

if (fClearMask)
	AstralUnionRect(&rEdit, &lpOldMask->rMask, &rSect);
else
	rEdit = rSect;

ox = rSect.left-lpObject->rObject.left;
nPixels = RectWidth(&rSect);
nWidth = RectWidth(&rEdit);
dx = rSect.left - rEdit.left;

lpPixmap = &lpMask->Pixmap;
lpMaskFrame = lpAlpha->Pixmap.EditFrame;

for (y = rEdit.top; y <= rEdit.bottom; ++y)
	{
	AstralClockCursor(y-rEdit.top, RectHeight(&rEdit), NO);
	if ( !(lpDst = PixmapPtr(lpPixmap, PMT_EDIT, rEdit.left, y, YES)) )
		continue;
	if (fClearMask)
		{
		clr(lpDst, nWidth);
		lpDst += dx;
		}
	
	oy = y-lpObject->rObject.top;
	if ( lpSrc = FramePointer(lpMaskFrame, ox, oy, NO) )
		CombineData(lpSrc, lpDst, nPixels, bInvert, ROP);
	}
ImgEditedObject(lpImage, lpBase, idDirty,
		fNewMask ? (LPRECT)NULL : (LPRECT)&rEdit);
MaskRectUpdate(lpMask, &rEdit);
ProgressEnd();
return(SC_SUCCESS);
}

/***********************************************************************/
LOCAL STATUS_CODE TransformMaskImage(LPIMAGE lpImage, LPDISTORT lpDistort, BOOL fCut,
					LPRECT lpSelectRect,
					int Mode,
					BOOL fHiQuality,
					BOOL fDelete,
					ITEMID idDirty, LPRECT lpUpdateRect)
/***********************************************************************/
{
STATUS_CODE StatusCode;

StatusCode = TransformMaskStart(lpImage, lpSelectRect, fCut, lpUpdateRect);
if (StatusCode == SC_SUCCESS)
	{
	ImgGetSelObjectRect(lpImage, &lpDistort->RectOrig, YES);
	TransformObjectsInit( lpImage, YES, lpDistort );
	
	StatusCode = TransformMaskEnd(lpImage, lpDistort, fCut, Mode,
						fHiQuality, NO, fDelete, lpSelectRect, idDirty, lpUpdateRect);
	}
return(StatusCode);
}

/***********************************************************************/
LOCAL STATUS_CODE TransformDataImage(LPIMAGE lpImage, LPDISTORT lpDistort, BOOL fCut,
					LPRECT lpSelectRect,
					int Opacity, MERGE_MODE MergeMode,
					BOOL fHiQuality,
					BOOL fPasteInto,
					ITEMID idDirty, LPRECT lpUpdateRect)
/***********************************************************************/
{
STATUS_CODE StatusCode;
LPOBJECT lpObject = NULL;
RECT rUpdate;

StatusCode = TransformDataStart(lpImage, lpSelectRect, fCut, NULL, lpUpdateRect);
if (StatusCode == SC_SUCCESS)
	{
	ImgGetSelObjectRect(lpImage, &lpDistort->RectOrig, YES);
	TransformObjectsInit( lpImage, YES, lpDistort );
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		{
		lpObject->Opacity = Opacity;
		lpObject->MergeMode = MergeMode;
		}
	StatusCode = TransformDataEnd(lpImage, lpDistort, fCut, NULL,
						fHiQuality, fPasteInto, NO, lpSelectRect, idDirty, lpUpdateRect);
	if ( !Control.MultipleObjects )
		{
		ImgCombineObjects( lpImage, NO, YES, YES, idDirty, &rUpdate );
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rUpdate);
		ImgPurgeObjects( lpImage );
		}
	}
return(StatusCode);
}

/***********************************************************************/
LOCAL STATUS_CODE TransformMaskStart(LPIMAGE lpImage, LPRECT lpSelectRect,
									BOOL fCut, LPRECT lpUpdateRect)
/***********************************************************************/
{
LPTR lpDst;
LPOBJECT lpObject;
LPMASK lpMask;
LPPIXMAP lpPixmap;
int nPixels, y;
int iValue;

// save selection state in case we escape
SaveSelectedObjects(lpImage);

if (!GetMaskObjectRect(lpImage, lpSelectRect))
	{
	Message(IDS_ENOMASKAREA);
	return(SC_BADPARMS);
	}

GetObjectMarqueeRect(lpImage, lpUpdateRect);
if (!(lpObject = ImgCreateMaskObject( lpImage, lpSelectRect )))
	{
	AstralSetRectEmpty(lpUpdateRect);
	return(SC_MEMERROR);
	}

if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, ImgGetBase(lpImage)))
	{
	ImgDeleteObject(lpImage, lpObject);
	return(SC_MEMERROR);
	}

// Pass the baton to IDC_OBJECTTRANSFORM tool if the object is not NULL
if ( fCut )
	{
	lpMask = ImgGetMask(lpImage);
	lpPixmap = &lpMask->Pixmap;
	nPixels = RectWidth(lpSelectRect);
	iValue = lpMask->bInvert ? 255 : 0;
	for (y = lpSelectRect->top; y <= lpSelectRect->bottom; ++y)
		{
		lpDst = PixmapPtr(lpPixmap, PMT_EDIT, lpSelectRect->left, y, YES);
		if (lpDst)
			set(lpDst, nPixels, iValue);
		}
	}
return(SC_SUCCESS);
}

/************************************************************************/
LOCAL STATUS_CODE TransformMaskEnd(LPIMAGE lpImage,
						LPDISTORT lpDistort, BOOL fCut, int Mode,
						BOOL fHighQuality, 
						BOOL fCancel,
						BOOL fDelete,
						LPRECT lpSelectRect,
						ITEMID idDirty, LPRECT lpUpdateRect)
/************************************************************************/
{
LPOBJECT lpBase;
LPFRAME lpMaskFrame;
LPPIXMAP lpPixmap;
LPMASK lpMask;
LPALPHA lpAlpha;
LPTR lpSrc, lpDst;
int y, ox, oy, nPixels;
RECT rSect;
BOOL bInvert;
COMBINE_ROP ROP;
LPOBJECT lpObject = NULL;
STATUS_CODE StatusCode;

while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (!ObjGetEditFrame(lpObject))
		break;
	}
if (!lpObject)
	return(SC_MEMERROR);

if (!(lpMask = ImgGetMask(lpImage)))
	{
	ObjFreeUp(lpObject);
	return(SC_MEMERROR);
	}
if (fCancel)
	{
	TransformObjectsInit( lpImage, NO, lpDistort );
	StatusCode = SC_CANCELLED;
	}
else
	{
	StatusCode = TransformObjectsExecute( lpImage, lpDistort, Mask.PasteQuality, NO, NO, NO, NULL, idDirty);
	if (StatusCode != SC_SUCCESS)
		fCancel = TRUE;
	}

// put new data in
lpBase = ImgGetBase(lpImage);
lpPixmap = &lpMask->Pixmap;
lpAlpha = ObjGetAlpha(lpObject);
if (!fDelete && lpAlpha &&
	AstralIntersectRect(&rSect, &lpBase->rObject, &lpObject->rObject))
	{
	bInvert = lpAlpha->bInvert;
	if (fCancel)
		ROP = CR_OR;
	else
		{
		if (Mode == SHAPE_REVERSE)
			ROP = CR_XOR;
		else if (Mode == SHAPE_SUBTRACT)
			{
			ROP = CR_AND;
			bInvert = !bInvert;
			}
		else // if (Mode == SHAPE_ADD)
			ROP = CR_OR;
		}
	ox = rSect.left-lpObject->rObject.left;
	nPixels = RectWidth(&rSect);
	lpMaskFrame = lpAlpha->Pixmap.EditFrame;
	for (y = rSect.top; y <= rSect.bottom; ++y)
		{
		oy = y-lpObject->rObject.top;
		lpSrc = FramePointer(lpMaskFrame, ox, oy, NO);
		lpDst = PixmapPtr(lpPixmap, PMT_EDIT, rSect.left, y, YES);
		if (lpSrc && lpDst)
			CombineData(lpSrc, lpDst, nPixels, bInvert, ROP);
		}
	}
ImgDeleteObject(lpImage, lpObject);
if (!fCancel)
	{
	if (Mask.TransformImage == IDC_MOVEMASK)
		AstralUnionRect(&rSect, &rSect, lpSelectRect);
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rSect);
	ImgEditedObject(lpImage, lpBase, idTransformDirty, &rSect);
	MaskRectUpdate(lpMask, &rSect);
	}
if (fCancel && fCut)
	ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, ImgGetBase(lpImage));
return(StatusCode);
}

/***********************************************************************/
LOCAL STATUS_CODE TransformDataStart(LPIMAGE lpImage, LPRECT lpSelectRect,
									BOOL fCut, LPPOBJECT lppCutObject,
									LPRECT lpUpdateRect)
/***********************************************************************/
{
LPOBJECT lpObject;
LPFRAME lpFrame;
LPTR lpDst;
long lPixel;
int XSize, YSize;
int y;
COLORINFO Color;

// save selection state in case we escape
SaveSelectedObjects(lpImage);

if (ImgGetMask(lpImage) && !GetMaskObjectRect(lpImage, lpSelectRect))
	{
	Message(IDS_ENOMASKAREA);
	return(SC_BADPARMS);
	}
// get rectangle of current object marquee
GetObjectMarqueeRect(lpImage, lpUpdateRect);
if (fCut && lppCutObject)
	{
	*lppCutObject = ImgCreateCutoutObject( lpImage, lpSelectRect );
	if (!*lppCutObject)
		{
		AstralSetRectEmpty(lpUpdateRect);
		return(SC_MEMERROR);
		}
	}

lpObject = ImgCreateCutoutObject( lpImage, lpSelectRect);
if (!lpObject)
	{
	if (fCut && lppCutObject)
		ImgDeleteObject(lpImage, *lppCutObject);
	else
		AstralSetRectEmpty(lpUpdateRect);
	return(SC_MEMERROR);
	}

if (fCut && lppCutObject)
	{
	lpFrame = ObjGetEditFrame(*lppCutObject);
	Color.gray = 255;
	SetColorInfo(&Color, &Color, CS_GRAY);
	lPixel = GetFrameColor(&Color, FrameTypeInfo(lpFrame));
	XSize = FrameXSize(lpFrame);
	YSize = FrameYSize(lpFrame);
	for (y = 0; y < YSize; ++y)
		{
		if ( lpDst = FramePointer(lpFrame, 0, y, YES) )
			FrameSetPixel(lpFrame, lPixel, lpDst, XSize);
		}
	}
return(SC_SUCCESS);
}

/************************************************************************/
LOCAL STATUS_CODE TransformDataEnd(LPIMAGE lpImage,
						LPDISTORT lpDistort, BOOL fCut, LPOBJECT lpCutObject,
						BOOL fHiQuality, BOOL fPasteInto,
						BOOL fCancel,
						LPRECT lpSelectRect,
						ITEMID idDirty, LPRECT lpUpdateRect)
/************************************************************************/
{
ENGINE Engine;
RECT rCutArea, rObjects;
COLORINFO Color;
STATUS_CODE StatusCode;

if (fCancel)
	{
	if (fCut && lpCutObject)
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &lpCutObject->rObject);
	ImgDeleteTempObjects(lpImage);
	return(SC_CANCELLED);
	}

if (lpCutObject)
   {
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &lpCutObject->rObject);
	ImgDeleteObject(lpImage, lpCutObject);
   }
if (fCut)
	{
	rCutArea = *lpSelectRect;
	ImgInvertSelection(lpImage);
	SetEngineDef(&Engine);
	Color.gray = 255;
	SetColorInfo(&Color, &Color, CS_GRAY);
	Engine.lpColor = &Color;
	Engine.lpEditRect = &rCutArea;
	Engine.fNoUndo = YES;
	Engine.fEscapable = FALSE;
	StatusCode = LineEngineSelObj(lpImage, &Engine, idDirty);
	ImgInvertSelection(lpImage);
	}

StatusCode = TransformObjectsExecute( lpImage, lpDistort, fHiQuality, fPasteInto,
						YES, fCut ? NEW_CUTOBJECT : YES, &rCutArea,
						idDirty);
if (StatusCode == SC_SUCCESS)
	ImgKeepFrameTempObjects(lpImage);
ImgGetSelObjectRect(lpImage, &rObjects, YES);
AstralUnionRect(lpUpdateRect, lpUpdateRect, &rObjects);
if (fCut)
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rCutArea);
return(StatusCode);
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgTransformerProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Transformer_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Transformer_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Transformer_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Transformer_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TRANSFORMER);
	}
}

/************************************************************************/
LOCAL BOOL Transformer_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CheckComboItem( hDlg, IDC_TRANSFUNCTION, IDC_TRANSFUNCTIONFIRST,
	IDC_TRANSFUNCTIONLAST, Mask.TransformImage );
Transformer_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Transformer_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Transformer_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Transformer_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Transformer_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Transformer_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
BOOL Bool;

Bool = GetActiveDoc() != NULL;
ControlEnable(hDlg, IDC_TRANSFUNCTION, Bool);
}

/************************************************************************/
LOCAL void Transformer_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_TRANSFORMER:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_TRANSFUNCTION:
//	case IDC_COPYMASK:
//	case IDC_MOVEMASK:
//	case IDC_COPYIMAGE:
//	case IDC_MOVEIMAGE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.TransformImage = id;
	SendMessage(hDlg, WM_CONTROLENABLE, 0, 0L);
	break;

   default:
	break;
   }
}

static int iRotateMode;

// LOCAL prototypes
LOCAL BOOL ObjectTransform_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ObjectTransform_OnDocActivated(HWND hDlg);
LOCAL void ObjectTransform_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void ObjectTransform_HandleEnable(HWND hDlg, int fFlags);
LOCAL void ObjectTransform_OnShieldChange(HWND hDlg);
LOCAL void ObjectTransform_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgObjectTransformProc( HWND hDlg, UINT msg, WPARAM wParam,
                                       LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, ObjectTransform_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, ObjectTransform_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, ObjectTransform_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, ObjectTransform_OnControlEnable);
	HANDLE_DLGMSG(hDlg, WM_SHIELDCHANGE, ObjectTransform_OnShieldChange);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_OBJECTTRANSFORM);
	}
}

/************************************************************************/
LOCAL BOOL ObjectTransform_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPOBJECT lpObject;
BOOL fEnable, UseMaskAndObjects;
LPIMAGE lpImage;
FRMDATATYPE Type = FDT_RGBCOLOR;

lpImage = GetActiveImage();
ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);
fPasteIntoMask = NO;
if (GetDlgItem(hDlg, IDC_PASTEINTOMASK))
	CheckDlgButton(hDlg, IDC_PASTEINTOMASK, fPasteIntoMask);

OPACITY = 255;
MERGEMODE= MM_NORMAL;
if (GetDlgItem(hDlg, IDC_MASKMODE_REVERSE))
	{
	MASKMODES = SHAPE_ADD;
	CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
		IDC_MASKMODE_REVERSE + max(MASKMODES - SHAPE_REVERSE, 0) );
	}

UseMaskAndObjects = Control.UseMaskAndObjects;
Control.UseMaskAndObjects = YES;
lpObject = ImgGetSelObject(lpImage, NULL);
Control.UseMaskAndObjects = UseMaskAndObjects;
fEnable = lpObject && lpObject->StorageType == ST_TEMPORARY;

CheckDlgButton( hDlg, IDC_SMARTPASTE, Mask.PasteQuality );
CheckComboItem( hDlg, IDC_TRANSMODE, IDC_TRANSSCALE,
	IDC_TRANSDISTORT, Mask.TransMode );
iRotateMode = IDC_TRANSROTATEXY;
CheckRadioButton( hDlg, IDC_TRANSROTATEXY, IDC_TRANSROTATEY, iRotateMode );
CheckDlgButton( hDlg, IDC_TRANSFLIPX, (bFlipX = NO) );
CheckDlgButton( hDlg, IDC_TRANSFLIPY, (bFlipY = NO) );

ControlEnable(hDlg, IDC_SMARTPASTE, CANDOTRANSPARENCY(Type));
if (GetDlgItem(hDlg, IDC_TRANSPARENT))
	{
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-OPACITY), NO,
		0, 99 );
	InitSlide( hDlg, IDC_TRANSPARENT_SCROLL, TOPERCENT(255-OPACITY),
		0, 99 );
	ControlEnable(hDlg, IDC_TRANSPARENT_SCROLL, fEnable && CANDOTRANSPARENCY(Type));
	ControlEnable(hDlg, IDC_TRANSPARENT, fEnable && CANDOTRANSPARENCY(Type));
	}
if (GetDlgItem(hDlg, IDC_MERGEMODE))
	{
	HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, MERGEMODE, 0L);
	ControlEnable(hDlg, IDC_MERGEMODE, fEnable && CANDOTRANSPARENCY(Type));
	}
ControlEnable( hDlg, IDC_PASTEINTOMASK,
	( lpImage ? ImgGetMask(lpImage) != NULL : NO ) );
return(FALSE);
}

/************************************************************************/
LOCAL void ObjectTransform_OnDocActivated(HWND hDlg)
/************************************************************************/
{
ObjectTransform_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void ObjectTransform_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
ObjectTransform_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void ObjectTransform_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
}

/************************************************************************/
LOCAL void ObjectTransform_OnShieldChange(HWND hDlg)
/************************************************************************/
{
LPOBJECT lpObject = NULL;
LPIMAGE lpImage;

lpImage = GetActiveImage();

while (lpObject = ImgGetSelObject(lpImage, lpObject))
	UpdateImage(lpImage, &lpObject->rObject, YES);
}

/************************************************************************/
LOCAL void ObjectTransform_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
LPOBJECT lpObject;
BOOL fOldHiQuality, Bool, fHiQuality;
LPIMAGE lpImage;
MERGE_MODE MergeMode;
FRMDATATYPE Type;

lpImage = GetActiveImage();

switch (id)
	{
	case IDC_OBJECTTRANSFORM:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_PASTEINTOMASK:
	if (!lpImage)
		break;
	fPasteIntoMask = !fPasteIntoMask;
	CheckDlgButton(hDlg, id, fPasteIntoMask);
	lpObject = NULL;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		{
		if (fPasteIntoMask != lpObject->fPasteIntoMask)
			{
			lpObject->fPasteIntoMask = fPasteIntoMask;
			UpdateImage(lpImage, &lpObject->rObject, YES);
			}
		}
	break;

	case IDC_MASKMODE_REVERSE:
	case IDC_MASKMODE_ADD:
	case IDC_MASKMODE_SUBTRACT:
	MASKMODES = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
	CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
		id );
	SetCommonMode(MASKMODES);
	break;

	case IDC_TRANSMODE:
	case IDC_TRANSSCALE:
	case IDC_TRANSSKEW:
	case IDC_TRANSPERSPECTIVE:
	case IDC_TRANSDISTORT:
	if (!lpImage)
		break;
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.TransMode = id;
	if (Tool.bActive)
		TrackMode( GetActiveDoc(), Mask.TransMode, YES );
	break;

	case IDC_TRANSROTATEXY:
	case IDC_TRANSROTATEX:
	case IDC_TRANSROTATEY:
	CheckRadioButton( hDlg, IDC_TRANSROTATEXY, IDC_TRANSROTATEY, id );
	iRotateMode = id;
	TrackRotateConstrain(
		iRotateMode == IDC_TRANSROTATEX,
		iRotateMode == IDC_TRANSROTATEY );
	break;

	case IDC_TRANSFLIPX:
	bFlipX = !bFlipX;
	CheckDlgButton( hDlg, id, bFlipX );
	TrackFlip(GetActiveDoc(), YES, NO, NO);
	if (Tool.bActive)
		TransformRefresh( GetActiveDoc(), TF_DURING );
	break;

	case IDC_TRANSFLIPY:
	bFlipY = !bFlipY;
	CheckDlgButton( hDlg, id, bFlipY );
	TrackFlip(GetActiveDoc(), NO, YES, NO);
	if (Tool.bActive)
		TransformRefresh( GetActiveDoc(), TF_DURING );
	break;

	case IDC_SMARTPASTE:
	if (!lpImage)
		break;
	ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);

	fOldHiQuality = Mask.PasteQuality && Mask.DisplayQuality;
	if (!CANDOTRANSPARENCY(Type))
		fOldHiQuality = FALSE;

	Mask.PasteQuality = !Mask.PasteQuality;
	CheckDlgButton( hDlg, id, Mask.PasteQuality );

	fHiQuality = Mask.PasteQuality && Mask.DisplayQuality;
	if (!CANDOTRANSPARENCY(Type))
		fHiQuality = FALSE;

	if ( Tool.bActive && (fHiQuality != fOldHiQuality) )
		TransformRefresh( GetActiveDoc(), TF_DURING );
	break;

	case IDC_MERGEMODE:
	if (!lpImage)
		break;
	if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
		break;
	MERGEMODE = MergeMode;
	lpObject = NULL;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		{
		if (MERGEMODE != lpObject->MergeMode)
			{
			lpObject->MergeMode = MERGEMODE;
			UpdateImage(lpImage, &lpObject->rObject, YES);
			}
		}
	SetCommonMergeMode(MERGEMODE);
	break;

	case IDC_TRANSPARENT:
		if (!lpImage)
			break;
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, id, &Bool, NO );
		SetSlide( hDlg, IDC_TRANSPARENT_SCROLL, i );
		OPACITY = 255-TOGRAY(i);
		lpObject = NULL;
		while (lpObject = ImgGetSelObject(lpImage, lpObject))
			{
			if (OPACITY != lpObject->Opacity)
				{
				lpObject->Opacity = OPACITY;
				UpdateImage(lpImage, &lpObject->rObject, YES);
				}
			}
		SetCommonOpacity(OPACITY);
		break;

	case IDC_TRANSPARENT_SCROLL:
		if (!lpImage)
			break;
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		i = HandleSlide( hDlg, id, codeNotify, NULL );
		SetDlgItemInt( hDlg, IDC_TRANSPARENT, i, FALSE );
		OPACITY = 255-TOGRAY(i);
		lpObject = NULL;
		while (lpObject = ImgGetSelObject(lpImage, lpObject))
			{
			if (OPACITY != lpObject->Opacity)
				{
				lpObject->Opacity = OPACITY;
				UpdateImage(lpImage, &lpObject->rObject, YES);
				}
			}
		SetCommonOpacity(OPACITY);
		break;

   default:
	break;
   }
}

/************************************************************************/
void TransformerProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
switch (Mask.TransformImage)
	{
	case IDC_MOVEMASK:
	case IDC_COPYMASK:
		TransformStartMaskProc( hWindow, msg, x, y, Option );
		break;
	case IDC_COPYIMAGE:
		TransformStartCopyProc( hWindow, msg, x, y, Option );
		break;
	case IDC_MOVEIMAGE:
		TransformStartCutProc( hWindow, msg, x, y, Option );
		break;
	}
}


static RECT SelectRect;

/************************************************************************/
LOCAL void TransformStartMaskProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
POINT pt;
LPIMAGE lpImage;
STATUS_CODE StatusCode;
RECT rUpdate;

lpImage = GetImagePtr(hWindow);
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if ( !IsOnImage(hWindow,  x, y ) )
		break;
	if (!ImgGetMask(lpImage))
		break;
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, SL_BOX | SL_SPECIAL,
		pt, 0L );
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );

	AstralCursor(IDC_WAIT);
	RemoveObjectMarquee(lpImage);
	StatusCode = TransformMaskStart(lpImage, &SelectRect,
				Mask.TransformImage == IDC_MOVEMASK, &rUpdate);
	StatusOfRectangle( &SelectRect, 0L,
				FrameResolution(ImgGetBaseEditFrame(lpImage)) );
	AstralCursor(NULL);
	if (StatusCode == SC_SUCCESS)
		TransformObjectsStart(lpImage, TransformEndMaskProc, IDC_MASKTRANSFORM, NO,
			IDS_UNDOTRANSFORMER, NO);
	else
		SetupMiniViews(NULL, NO);
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
		pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER );
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	break;

	case WM_ACTIVATE:
	return;
	}
}

/************************************************************************/
LOCAL STATUS_CODE TransformEndMaskProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
STATUS_CODE StatusCode;
int res, i;
RECT rUpdate;
TRANSFORMER_PARMS parms;

StatusCode = TransformMaskEnd(lpImage, &Distort,
			Mask.TransformImage == IDC_MOVEMASK,
			MASKMODES, Mask.PasteQuality, fCancel, NO,
	 		&SelectRect, idTransformDirty, &rUpdate);
if (fDelete)
	StatusCode = SC_SUCCESS;
if (StatusCode == SC_SUCCESS)
	{
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.iBaseRes = res;
	parms.SelectRect = SelectRect;
	parms.Function = Mask.TransformImage;
	parms.Mode = MASKMODES;
	parms.TransformOpacity = 255;
	parms.TransformMergeMode = MM_NORMAL;
	parms.fHiQuality = Mask.PasteQuality;
	parms.fPasteInto = NO;
	parms.fDelete = fDelete;
	for (i = 0; i < 4; ++i)
		{
		parms.DistortPoints[i] = Distort.p[i];
		}
	PostCommand(lpImage->lpCmdList, IDS_CMD_TRANSFORMMASK, &parms);
	}
return(StatusCode);
}

static LPMASK lpOldMask;

/************************************************************************/
void TransformStartLoadMaskProc(LPIMAGE lpImage, BOOL fDeleteMask)
/************************************************************************/
{
LPMASK lpMask;

lpOldMask = NULL;
lpMask = ImgGetMask(lpImage);
if (fDeleteMask && lpMask)
	{
	RemoveMaskMarquee(lpImage);
	ImgSetMask(lpImage, NULL);
	lpOldMask = lpMask;
	}
TransformObjectsStart(lpImage, TransformEndLoadMaskProc, IDC_MASKTRANSFORM, NO,
	IDS_UNDOLOADMASK, NO, Names.Mask);
}

/************************************************************************/
LOCAL STATUS_CODE TransformEndLoadMaskProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
int i, res;
LOADMASK_PARMS parms;
RECT rUpdate;
STATUS_CODE StatusCode;

StatusCode = LoadMaskEnd(lpImage, fCancel, lpOldMask, MASKMODES, idTransformDirty, &Distort, &rUpdate);
if (StatusCode == SC_SUCCESS)
	{
	if (!AstralIsRectEmpty(&rUpdate))
		UpdateImage(lpImage, &rUpdate, YES);

	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.fHiQuality = Mask.PasteQuality;
	parms.Mode = MASKMODES;
	parms.iBaseRes = res;
	for (i = 0; i < 4; ++i)
		parms.DistortPoints[i] = Distort.p[i];
	lstrcpy(parms.szMask, szTransformFileName);
	parms.fDeleteMask = Mask.DeleteMask;
	LookupExtFileN(parms.szMask, parms.szMaskFileName, IDN_MASK, NO);
	PostCommand(lpImage->lpCmdList, IDS_CMD_LOADMASK, &parms);
	}
SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
return(StatusCode);
}


static LPOBJECT lpCutObject;
static RECT rCutArea;
static COLORINFO Color;

/************************************************************************/
LOCAL void TransformStartCutProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
POINT pt;
LPIMAGE lpImage;
STATUS_CODE StatusCode;
RECT rUpdate;

lpImage = GetImagePtr(hWindow);

switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if ( !IsOnImage(hWindow,  x, y ) )
		break;
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, SL_BOX | SL_SPECIAL,
		pt, 0L );
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );

	AstralCursor(IDC_WAIT);
	RemoveObjectMarquee(lpImage);
	StatusCode = TransformDataStart(lpImage, &SelectRect, YES, &lpCutObject, &rUpdate);
	AstralCursor( NULL );

	// Pass the baton to IDC_OBJECTTRANSFORM tool if the object is not NULL
	if ( StatusCode == SC_SUCCESS )
		TransformObjectsStart(lpImage, TransformEndCutProc,
						IDC_OBJECTTRANSFORM, NEW_CUTOBJECT,
						IDS_UNDOTRANSFORMER, YES);
	else
		SetupMiniViews(NULL, NO);
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
		pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	break;

	case WM_ACTIVATE:
	return;
	}
}

/************************************************************************/
LOCAL STATUS_CODE TransformEndCutProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
STATUS_CODE StatusCode;
RECT rUpdate;
int i, res;
TRANSFORMER_PARMS parms;

StatusCode = TransformDataEnd(lpImage, &Distort,
			Mask.TransformImage == IDC_MOVEIMAGE, lpCutObject,
			Mask.PasteQuality, fPasteIntoMask, fCancel,
	 		&SelectRect, idTransformDirty, &rUpdate);
if (StatusCode != SC_SUCCESS || fCancel)
	UpdateImage(lpImage, &rUpdate, YES);
else
if (StatusCode == SC_SUCCESS)
	{
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.iBaseRes = res;
	parms.SelectRect = SelectRect;
	parms.Function = Mask.TransformImage;
	parms.Mode = MASKMODES;
	parms.TransformOpacity = OPACITY;
	parms.TransformMergeMode = MERGEMODE;
	parms.fHiQuality = Mask.PasteQuality;
	parms.fPasteInto = fPasteIntoMask;
	parms.fDelete = NO;
	for (i = 0; i < 4; ++i)
		{
		parms.DistortPoints[i] = Distort.p[i];
		}
	PostCommand(lpImage->lpCmdList, IDS_CMD_TRANSFORMIMAGE, &parms);
	}
return(StatusCode);
}

/************************************************************************/
STATUS_CODE TransformObjectsEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
int i, res;
STATUS_CODE StatusCode;
TRANSFORMOBJECTS_PARMS parms;

StatusCode = TransformEndProc(lpImage, fCancel, fDelete, fMoveOnly);

if (!fCancel && StatusCode == SC_SUCCESS)
	{
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.fHiQuality = Mask.PasteQuality;
	parms.iBaseRes = res;
	for (i = 0; i < 4; ++i)
		{
		parms.DistortPoints[i] = Distort.p[i];
		}
	PostCommand(lpImage->lpCmdList, IDS_CMD_TRANSFORMOBJECTS, &parms);
	}
return(StatusCode);
}

/************************************************************************/
STATUS_CODE PasteFromEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
int i, res;
PASTEFROM_PARMS parms;
STATUS_CODE StatusCode;
FRMTYPEINFO TypeInfo;

StatusCode = TransformEndProc(lpImage, fCancel, fDelete, fMoveOnly);

if (!fCancel && StatusCode == SC_SUCCESS)
	{
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.TransformOpacity = OPACITY;
	parms.TransformMergeMode = MERGEMODE;
	parms.fHiQuality = Mask.PasteQuality;
	parms.fPasteInto = fPasteIntoMask;
	parms.iBaseRes = res;
	for (i = 0; i < 4; ++i)
		{
		parms.DistortPoints[i] = Distort.p[i];
		}
	if (bTransformExtName)
	{
		lstrcpy(parms.szClipboard, szTransformFileName);
		LookupExtFileN(parms.szClipboard, parms.szClipFileName, IDN_CLIPBOARD, NO);
		parms.fTemporary = NO;
	}
	else
	{
		parms.szClipboard[0] = '\0';
		lstrcpy(parms.szClipFileName, szTransformFileName);
		parms.fTemporary = YES;
	}
	parms.cmsInfo.src = TransformSrcTypeInfo;
	ImgGetTypeInfo(lpImage, &parms.cmsInfo.dst);
	PostCommand(lpImage->lpCmdList, IDS_CMD_PASTEFROM, &parms);
	}
return(StatusCode);
}

/************************************************************************/
STATUS_CODE PasteEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
	int i, res;
	BOOL fDoPasteFrom = FALSE;
	FNAME	szFileName;
	STATUS_CODE StatusCode;

	StatusCode = TransformEndProc(lpImage, fCancel, fDelete, fMoveOnly);

	if (!fCancel && StatusCode == SC_SUCCESS)
	{
		if (dwTransformClipOwner != lpImage->lpCmdList->dwImageID &&
			Control.DoCommandList &&
			Control.WriteFileOnPaste &&
			PictPubApp.GetClipboardFileName(szFileName))
		{
			PASTEFROM_PARMS parms;

			res = FrameResolution(ImgGetBaseEditFrame(lpImage));
			parms.TransformOpacity = OPACITY;
			parms.TransformMergeMode = MERGEMODE;
			parms.fHiQuality = Mask.PasteQuality;
			parms.fPasteInto = fPasteIntoMask;
			parms.iBaseRes = res;
			for (i = 0; i < 4; ++i)
				parms.DistortPoints[i] = Distort.p[i];
			lstrcpy(parms.szClipboard, "");
			lstrcpy(parms.szClipFileName, szFileName);
			parms.fTemporary = YES;
			parms.cmsInfo.src = TransformSrcTypeInfo;
			ImgGetTypeInfo(lpImage, &parms.cmsInfo.dst);
			PostCommand(lpImage->lpCmdList, IDS_CMD_PASTEFROM, &parms);
		}
		else
		{
			PASTE_PARMS parms;

			res = FrameResolution(ImgGetBaseEditFrame(lpImage));
			parms.TransformOpacity = OPACITY;
			parms.TransformMergeMode = MERGEMODE;
			parms.fHiQuality = Mask.PasteQuality;
			parms.fPasteInto = fPasteIntoMask;
			parms.iBaseRes = res;
			for (i = 0; i < 4; ++i)
				parms.DistortPoints[i] = Distort.p[i];
			PostCommand(lpImage->lpCmdList, IDS_CMD_PASTE, &parms);
		}
	}
	return(StatusCode);
}

/************************************************************************/
LOCAL STATUS_CODE TransformEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
/************************************************************************/
{
STATUS_CODE StatusCode = SC_CANCELLED;

if (fCancel)
	TransformObjectsInit( lpImage, NO, NULL );
else
	{
	StatusCode = TransformObjectsExecute( lpImage, &Distort, Mask.PasteQuality,
				fPasteIntoMask, fTransformDoUndo, iTransformNewObject, NULL,
				idTransformDirty);
	fCancel = StatusCode != SC_SUCCESS;
	}
if (fCancel)
	ImgDeleteTempObjects(lpImage);
else
	ImgKeepFrameTempObjects(lpImage);
	
return(StatusCode);
}

/************************************************************************/
LOCAL void TransformStartCopyProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
POINT pt;
LPIMAGE lpImage;
STATUS_CODE StatusCode;
RECT rUpdate;

lpImage = GetImagePtr(hWindow);

switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if ( !IsOnImage(hWindow,  x, y ) )
		break;
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, SL_BOX | SL_SPECIAL,
		pt, 0L );
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );

	AstralCursor(IDC_WAIT);
	RemoveObjectMarquee(lpImage);
	StatusCode = TransformDataStart(lpImage, &SelectRect, NO, NULL, &rUpdate);
	AstralCursor( NULL );

	// Pass the baton to IDC_OBJECTTRANSFORM tool if the object is not NULL
	if ( StatusCode == SC_SUCCESS )
		TransformObjectsStart(lpImage, TransformEndCutProc, IDC_OBJECTTRANSFORM, YES,
				IDS_UNDOTRANSFORMER, YES);
	else
		SetupMiniViews(NULL, NO);
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
		pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	break;

	case WM_ACTIVATE:
	return;
	}
}

/************************************************************************/
LOCAL BOOL GetMaskObjectRect(LPIMAGE lpImage, LPRECT lpRect)
/************************************************************************/
{
BOOL fGotaRect;
LPMASK lpMask;
RECT rMask;

// Setup a file rectangle to cut out, objectify, and transform
if ( abs( lpRect->right - lpRect->left ) <= SMALL_MOVEMENT &&
	 abs( lpRect->bottom - lpRect->top ) <= SMALL_MOVEMENT )
		fGotaRect = NO;
else	fGotaRect = YES;

if (!(lpMask = ImgGetMask(lpImage)))
	return(FALSE);

ImgGetMaskRect(lpImage, &rMask);
if (!fGotaRect)
	*lpRect = rMask;
else
if (!AstralIntersectRect(lpRect, lpRect, &rMask))
	return(FALSE);

return(MaskRectDwindle(lpMask, lpRect, NULL));
}

/************************************************************************/
void TransformObjectsStart(LPIMAGE lpImage, LPTRANSFORMENDPROC lpProc,
			int idTool, BOOL fNewObject, int idDirty, BOOL fDoUndo,
			LPSTR lpFileName, DWORD dwClipOwner, BOOL bExtName,
			LPFRMTYPEINFO lpSrcTypeInfo)
/************************************************************************/
{
ITEMID idLastTool;

lpEndProc = lpProc;
iTransformNewObject = fNewObject;
idTransformDirty = idDirty;
fTransformDoUndo = fDoUndo;
idLastTool = ActivateTool(idTool);
dwTransformClipOwner = dwClipOwner;
bTransformExtName = bExtName;
if (lpSrcTypeInfo)
	TransformSrcTypeInfo = *lpSrcTypeInfo;
else
	FrameSetTypeInfo(&TransformSrcTypeInfo, FDT_NONE);
if (lpFileName)
	lstrcpy(szTransformFileName, lpFileName);
CreateProc( GetActiveDoc(), 0, 0, idLastTool);
}

/************************************************************************/
void TransformObjectsProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
RECT rOrig, rEdit, rUpdate;
BOOL fCancel, fMoveOnly, fDelete, fProgress;
WORD wKey;
static BOOL UseMaskAndObjects;
LPIMAGE lpImage;
LPOBJECT lpObject;
LFIXED fScale;
STATUS_CODE StatusCode;

lpImage = GetImagePtr(hWindow);

fCancel = NO;
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	UseMaskAndObjects = Control.UseMaskAndObjects;
	Control.UseMaskAndObjects = YES;
	if ( !ImgGetSelObjectRect( lpImage, &rOrig, YES/*bExcludeBase*/ ) )
		{ // If no floating objects are selected, get out
		Control.UseMaskAndObjects = UseMaskAndObjects;
		ActivateTool( Tool.idLast );
		SetupMiniViews(NULL, NO);
		break;
		}
	// if we are not working with a temporary object then we should 
	// save selection state in case the user escapes
	if ((lpObject = ImgGetSelObject(lpImage, NULL)) && lpObject->StorageType != ST_TEMPORARY)
		SaveSelectedObjects(lpImage);
	
	if (!TransformEditInit(lpImage, iTransformNewObject, fTransformDoUndo ))
		{
		ActivateTool( Tool.idLast );
		Control.UseMaskAndObjects = UseMaskAndObjects;
		SetupMiniViews(NULL, NO);
		break;
		}
	AstralCursor( IDC_WAIT );

	// if it is a new object, look at the paste option to
	// see if we should paste it at real size
	fScale = FUNITY;
	if (iTransformNewObject)
		{
		int ObjectRes, ImageRes;
		LPOBJECT lpObject;

		lpObject = ImgGetSelObject(lpImage, NULL);
		ImageRes = FrameResolution(ImgGetBaseEditFrame(lpImage));
		ObjectRes = FrameResolution(ObjGetEditFrame(lpObject));
		if (Mask.PasteAtRealSize)
			fScale = FGET(ImageRes, ObjectRes);
		FrameSetResolution(ObjGetEditFrame(lpObject), ImageRes);
		if (lpObject->lpAlpha)
			FrameSetResolution(AlphaGetEditFrame(lpObject->lpAlpha), ImageRes);
		}

	// Initialize the tracking with the selected object rectangle
	TrackInit( hWindow, Mask.TransMode, NULL, &rOrig, fScale );

	// Initialize the transform display stuff
	TransformRefresh( hWindow, TF_START );

	// Draw the tracking rectangle
	ObjectDrawHook.DisplayHookProc( hWindow, Window.hDC, NULL );

	// Setup our display hook for object rectangle
	SetDisplayHook(hWindow, &ObjectDrawHook);

	// need to check error condition here, when this gets redone
	// Setup display for repaints
	AstralCursor( NULL );
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	TrackGrab(hWindow, x, y);
	break;

	case WM_LBUTTONUP:
	TrackUp(hWindow, x, y);
	if ( !TrackMoved() )
		break;
	TransformRefresh( hWindow, TF_DURING );
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	if (!Window.fLButtonDown)
		TrackSetCursor(hWindow, x, y);
	else
		TrackMove(hWindow, x, y);
	break;

	case WM_DESTROY:	// The cancel operation message
	fCancel = (Option == 1L) || (Option == 2L);
	fDelete = (Option == 2L);
	// Fall through...

	case WM_LBUTTONDBLCLK:
	if ( !Tool.bActive )
		break;
	if (msg == WM_LBUTTONDBLCLK)
		fCancel = fDelete = FALSE;
	AstralUpdateWindow(hWindow);
	SetDisplayHook(hWindow, NULL);
	fMoveOnly = TrackDone(hWindow, YES);
	fProgress = (!fCancel && !Control.MultipleObjects);
	if (fProgress)
		ProgressBegin(NeedToTransform(fMoveOnly, fPasteIntoMask) ? 2 : 1,
					idTransformDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

	if (lpEndProc)
		{
		StatusCode = (*lpEndProc)(lpImage, fCancel, fDelete, fMoveOnly);
		ImgDeleteTempObjects(lpImage);
		}
	else
		StatusCode = TransformEndProc(lpImage, fCancel, fDelete, fMoveOnly);

	fCancel = StatusCode != SC_SUCCESS;

	TransformRefresh( hWindow, fCancel ? TF_CANCEL : TF_END );
	if ( !Control.MultipleObjects &&
		ImgGetFloatingObjectsRect(lpImage, &rEdit))
		{
		ImgCombineObjects( lpImage, NO, YES, YES, idTransformDirty, &rUpdate );
		UpdateImage(lpImage, &rUpdate, YES);

		ImgPurgeObjects( lpImage );
//		ImgEditedObject(lpImage, ImgGetBase(lpImage), idTransformDirty, &rEdit);
		}

	// retore selection state of all objects
	if (fCancel && !fDelete)
	{
		RestoreSelectedObjects(lpImage);
     	UpdateImage(lpImage, NULL, YES);
	}
	if (fProgress)
		ProgressEnd();

	Control.UseMaskAndObjects = UseMaskAndObjects;
	UpdateStatusBar( YES, NO, YES, NO );
	SetupMiniViews(NULL, NO);
	Tool.bActive = NO;
	ActivateTool( Tool.idLast );
	break;

	case WM_KEYDOWN:
	if (!Tool.bActive)
		break;
	wKey = Option;
	switch (wKey)
		{
		case VK_UP:
			TrackDelta(hWindow, 0, -1);
			break;
		case VK_DOWN:
			TrackDelta(hWindow, 0, 1);
			break;
		case VK_LEFT:
			TrackDelta(hWindow, -1, 0);
			break;
		case VK_RIGHT:
			TrackDelta(hWindow, 1, 0);
			break;
		case VK_DELETE:
			if (Tool.id == IDC_SELECTORTRANSFORM)
				break;
			if (Tool.id == IDC_MASKTRANSFORM)
			{
				ImgDeleteObjects( lpImage, NULL, YES/*bOnlyIfSelected*/ );
				TransformObjectsProc( hWindow, WM_DESTROY, 0, 0, 2 );
				break;
			}
			ImgDeleteObjects( lpImage, NULL, YES/*bOnlyIfSelected*/ );
			TransformObjectsProc( hWindow, WM_DESTROY, 0, 0, 1 );
			break;
		default:
			break;
		}
	break;

	case WM_KEYUP:
	wKey = Option;
	if (wKey != VK_UP && wKey != VK_DOWN && wKey != VK_LEFT && wKey != VK_RIGHT)
		break;
	TransformRefresh( hWindow, TF_DURING );
	break;

	case WM_ACTIVATE:
	return;
	}
}

/************************************************************************/
LOCAL BOOL TransformEditInit(LPIMAGE lpImage, BOOL iNewObject, BOOL fDoUndo )
/************************************************************************/
{
UNDO_TYPE UndoType;

if (iNewObject && fDoUndo)
	{
	if (Control.MultipleObjects)
		{
		UndoType = UT_DELETEOBJECTS;
		if (iNewObject == NEW_CUTOBJECT)
			UndoType |= UT_DATA;
		if (!ImgEditInit( lpImage, ET_OBJECT, UndoType,
						 	ImgGetBase(lpImage) ))
			return(FALSE);
		}
	else
		{
		// this undo initialization will happen again
		// in ImgCombineObjects() because it cannot the createmask
		// undo cannot be set up without having a mask first
		// and ImgCombineObjects() is the guy that creates
		// the mask
		UndoType = UT_DATA;
		if (ImgGetMask(lpImage))
			UndoType |= UT_ALPHA;
		if (!ImgEditInit( lpImage, ET_OBJECT, UndoType,
		 	ImgGetBase(lpImage) ))
			return(FALSE);
		}
	}
else if (fDoUndo)
	{
	if (!ImgEditInit( lpImage, ET_SELOBJECTS,
					UT_NEWDATA|UT_NEWALPHA|UT_OBJECTRECT, NULL ))
		return(FALSE);
	}
return(TRUE);
}

/************************************************************************/
LOCAL BOOL TransformRefresh( HWND hWnd, int iCode )
/************************************************************************/
{
RECT rUpdate[2], rSect;
int nRects = 0;
static RECT rLastRefresh;
static BOOL bStarted;

#define H_SIZE 6
#define H_OFFSET (H_SIZE/2)

if ( !hWnd )
	return(FALSE);
if ( !bStarted && iCode != TF_START )
	return( FALSE );
if ( bStarted && iCode == TF_START )
	return( FALSE );

if ( iCode == TF_START )
	bStarted = YES;
if ( iCode == TF_END || iCode == TF_CANCEL )
	bStarted = NO;

AstralCursor(IDC_WAIT);
if ( iCode == TF_DURING || iCode == TF_CANCEL || iCode == TF_END )
	{ // Invalidate the LAST TRANSFORMED area
	rUpdate[nRects++] = rLastRefresh;
	}

if ( iCode == TF_START || iCode == TF_DURING )
	{
	// Get the updated tracking transform
	TrackGetRect( &rLastRefresh );

	if (iCode == TF_START &&
		( (ColorMask.Mask && ColorMask.On) ||
		  (iTransformNewObject && Mask.PasteAtRealSize) ) )
		rUpdate[nRects++] = rLastRefresh;

	// Update the transforms for all selected objects
	TransformObjectsInit( GetImagePtr(hWnd), YES, NULL );

	// make sure an invalidate happens first so rectangle won't draw twice
	if (iCode == TF_START)
		AstralUpdateWindow(hWnd);
	}

if ( iCode == TF_DURING )
	{ // Invalidate the NEWLY TRANSFORMED area
	rUpdate[nRects++] = rLastRefresh;
	}

if ( iCode == TF_CANCEL )
	{ // Invalidate the ORIGINAL area
	ImgGetSelObjectRect( GetImagePtr(hWnd), &rUpdate[nRects++], YES/*bExcludeBase*/ );
	}

// The track stuff will get re-drawn by the paint engine
if (nRects)
	{
	UpdateImage(GetImagePtr(hWnd), &rUpdate[0], YES);
	if (nRects > 1)
		{
		if (!AstralIntersectRect(&rSect, &rUpdate[0], &rUpdate[1]))
			AstralUpdateWindow(hWnd);
		UpdateImage(GetImagePtr(hWnd), &rUpdate[1], YES);
		}
	AstralUpdateWindow( hWnd );
	}
AstralCursor(NULL);

return(TRUE);
}


/************************************************************************/
LOCAL BOOL TransformObjectsInit( LPIMAGE lpImage, BOOL bInit,
								LPDISTORT lpDistort)
/************************************************************************/
{
LPOBJECT lpObject;

if (!lpDistort)
	TrackGetForwardDistort( &Distort );

lpObject = ImgGetBase(lpImage); // Start with objects after the base
while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
	{
	if ( !bInit )
		{ // Cleanup on an error condition
		if ( lpObject->fTransform )
			{
			lpObject->rObject = lpObject->rTransform;
			lpObject->fTransform = NO;
			lpObject->fPasteIntoMask = NO;
			}
		}
	else
		{
		if ( !lpObject->fTransform )
			{ // If this is the first time...
			lpObject->rTransform = lpObject->rObject;
			lpObject->fTransform = YES;
			lpObject->fPasteIntoMask = fPasteIntoMask;
			}
		if (!lpDistort)
			lpDistort = &Distort;
		TrackMapOriginalRect( lpDistort, &lpObject->rTransform,
			&lpObject->rObject );
		}
	}

return( YES );
}

/************************************************************************/
LOCAL BOOL NeedToTransform(BOOL fMoveOnly, BOOL fPasteInto)
/************************************************************************/
{
return(!fMoveOnly || fPasteInto ||
	   	(ColorMask.Mask && ColorMask.On));
}

/************************************************************************/
LOCAL BOOL IsMoveOnly(LPIMAGE lpImage, LPDISTORT lpDistort)
/************************************************************************/
{
RECT rOrig;
LPOBJECT lpObject = NULL;
POINT pts[4];
int dx, dy, i;

AstralSetRectEmpty(&rOrig);
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	AstralUnionRect(&rOrig, &rOrig, &lpObject->rTransform);
dx = rOrig.left - lpDistort->p[0].x;
dy = rOrig.top - lpDistort->p[0].y;

pts[0].x = rOrig.left - dx;
pts[0].y = rOrig.top - dy;
pts[1].x = rOrig.right - dx;
pts[1].y = rOrig.top - dy;
pts[2].x = rOrig.right - dx;
pts[2].y = rOrig.bottom - dy;
pts[3].x = rOrig.left - dx;
pts[3].y = rOrig.bottom - dy;
for (i = 0; i < 4; ++i)
	{
	if (pts[i].x != lpDistort->p[i].x)
		return(FALSE);
	if (pts[i].y != lpDistort->p[i].y)
		return(FALSE);
	}
return(TRUE);
}

/************************************************************************/
LOCAL STATUS_CODE TransformObjectsExecute( LPIMAGE lpImage, LPDISTORT lpDistort,
									BOOL fHiQuality, BOOL fPasteInto,
									BOOL fTransformDoUndo,
									int iTransformNewObject,
									LPRECT lpCutArea,
									ITEMID idDirty)
/************************************************************************/
{
STATUS_CODE StatusCode = SC_SUCCESS;
LPOBJECT lpObject;
BOOL fTransformObject;
BOOL fMoveOnly;
FRMDATATYPE Type;

ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);
if (!CANDOTRANSPARENCY(Type))
	fHiQuality = FALSE;

fMoveOnly = IsMoveOnly(lpImage, lpDistort);
fTransformObject = NeedToTransform(fMoveOnly, fPasteInto);

if (fTransformObject)
	ProgressBegin(ImgCountSelObjects(lpImage, NULL),
		idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

lpObject = ImgGetBase(lpImage); // Start with objects after the base
while ( lpObject = ImgGetSelObject(lpImage, lpObject) )
	{
	if ( !lpObject->fTransform || lpObject->bLocked)
		continue;
	lpObject->fPasteIntoMask = fPasteInto;
	if (!lpObject->Pixmap.EditFrame && ObjGetAlpha(lpObject))
	{
		Type = FrameType(AlphaGetEditFrame(ObjGetAlpha(lpObject)));
		if (!CANDOTRANSPARENCY(Type))
			fHiQuality = FALSE;
	}
	if (StatusCode == SC_SUCCESS)
		{
		if ( fTransformObject )
			{
			StatusCode = TransformObject( lpImage, lpObject,
							lpDistort, fHiQuality, fTransformDoUndo,
							iTransformNewObject, lpCutArea, idDirty);
			}
		else // just a move
			{
			if (iTransformNewObject && fTransformDoUndo)
				{
				lpObject->fUndoDeleted = YES;
				if (iTransformNewObject == NEW_CUTOBJECT)
					ImgEditedObject(lpImage, ImgGetBase(lpImage), idDirty,
							lpCutArea);
				else
					ImgEditedObject(lpImage, ImgGetBase(lpImage), idDirty, NULL);
				}
			}
		}
	if (StatusCode == SC_SUCCESS)
		{
		lpObject->fTransform = NO;
		lpObject->fPasteIntoMask = NO;
		}

	}

// reset info for any objects that have not been transformed
TransformObjectsInit( lpImage, FALSE, NULL);

if ( StatusCode != SC_SUCCESS && fTransformDoUndo)
	ImgEditRecoverSelObj( lpImage, TRUE, TRUE );
if (fTransformObject)
	ProgressEnd();
return( StatusCode );
}

/************************************************************************/
LOCAL STATUS_CODE TransformObject( LPIMAGE lpImage, LPOBJECT lpObject,
							LPDISTORT lpDistort, BOOL fHiQuality,
							BOOL fTransformDoUndo,
							int iTransformNewObject,
							LPRECT lpCutArea,
							ITEMID idDirty)
/************************************************************************/
{
FRMTYPEINFO DataTypeInfo, AlphaTypeInfo;
int  x, y, dx, dy, iDataDepth, iAlphaDepth;
LPTR lpDataBuf, lpAlphaBuf;
LPFRAME lpFrame, lpDataFrame, lpAlphaFrame;
LPALPHA lpAlpha;
LPMASK	lpMask;
BOOL fColorMask, fPasteInto;
int	mx, my;

ASSERT(lpObject);

ProgressBegin(1, 0);
// How big are the newly transformed frames going to be
dx = RectWidth( &lpObject->rObject );
dy = RectHeight( &lpObject->rObject );

fColorMask = ColorMask.Mask && ColorMask.On;
if ((fPasteInto = lpObject->fPasteIntoMask) && (lpMask = ImgGetMask(lpImage)))
	{
	mx = lpObject->rObject.left;
	my = lpObject->rObject.top;
	}
else
	fPasteInto = NO;

// Create a new data frame
lpDataFrame = NULL;
if ( lpFrame = lpObject->Pixmap.EditFrame )
	{
	FrameGetTypeInfo(lpFrame, &DataTypeInfo);
	if ( lpDataFrame = FrameOpen( DataTypeInfo, dx, dy, FrameResolution(lpFrame) ) )
		{ // Allocate a data buffer to transform lines into
		iDataDepth = max( 1, FrameDepth(lpDataFrame) );
		}
	else
		{
		Message(IDS_EMEMALLOC);
		return(SC_MEMERROR);
		}
	}

// Create a new alpha frame
lpAlphaFrame = NULL;
if ( lpAlpha = lpObject->lpAlpha )
	{
	if ( lpFrame = lpAlpha->Pixmap.EditFrame )
		{
		FrameGetTypeInfo(lpFrame, &AlphaTypeInfo);
		if ( lpAlphaFrame = FrameOpen( AlphaTypeInfo, dx, dy, FrameResolution(lpFrame) ) )
			{ // Allocate a data buffer to transform lines into
			iAlphaDepth = max( 1, FrameDepth(lpAlphaFrame) );
			}
		else
			{
			Message(IDS_EMEMALLOC);
			if (lpDataFrame)
				FrameClose(lpDataFrame);
			return(SC_MEMERROR);
			}
		}
	}

// Create the data for the two new frames one line at a time
x = 0;

for (y = 0; y <= dy; ++y)
	{
	if (AstralClockCursor( y, dy, YES ))
		{
		ProgressEnd();
		if (lpDataFrame)
			FrameClose(lpDataFrame);
		if (lpAlphaFrame)
			FrameClose(lpAlphaFrame);
		return(SC_CANCELLED);
		}

	if (lpAlphaFrame)
		lpAlphaBuf = FramePointer( lpAlphaFrame, x, y, TRUE );
	else
		lpAlphaBuf = NULL;

	if (lpDataFrame)
		lpDataBuf = FramePointer( lpDataFrame, x, y, TRUE );
	else
		lpDataBuf = NULL;

	if ( !TransformLine( lpObject, x, y, dx, lpDataBuf, iDataDepth,
		lpAlphaBuf, iAlphaDepth, lpDistort, fHiQuality ) )
		continue;
	if (lpAlphaBuf)
		{
		if (fPasteInto)
			MaskLoader(lpMask, mx, my++, dx, lpAlphaBuf, NO, CR_MULTIPLY);
		if (fColorMask && lpDataBuf)
			Shield(lpAlphaBuf, lpDataBuf, (WORD)dx, DataTypeInfo);
		}
	}

if (iTransformNewObject && fTransformDoUndo)
	{
	FrameClose(lpObject->Pixmap.EditFrame);
	if (lpAlpha)
		FrameClose(lpAlpha->Pixmap.EditFrame);
	lpObject->Pixmap.EditFrame = lpDataFrame;
	lpAlpha->Pixmap.EditFrame = lpAlphaFrame;
	lpObject->fUndoDeleted = YES;
	if (iTransformNewObject == NEW_CUTOBJECT)
		ImgEditedObject(lpImage, ImgGetBase(lpImage), idDirty,
							lpCutArea);
	else
		ImgEditedObject(lpImage, ImgGetBase(lpImage), idDirty, NULL);
	}
else
if (fTransformDoUndo)
	{
	ImgEditedObjectFrame(lpImage, lpObject, idDirty, NULL,
		lpDataFrame, lpAlphaFrame);
	}
else
	{ // just plug in the newly created frames
	if (lpDataFrame)
		{
		FrameClose(lpObject->Pixmap.EditFrame);
		lpObject->Pixmap.EditFrame = lpDataFrame;
		}
	if (lpAlphaFrame)
		{
		FrameClose(lpAlpha->Pixmap.EditFrame);
		lpAlpha->Pixmap.EditFrame = lpAlphaFrame;
		}
	}

ProgressEnd();
return( SC_SUCCESS );
}

/************************************************************************/
// This routines take NEW frame coordiantes x1, y, dx,
// and stuffs the passed data and alpha buffers
// given a transform BACK to the passed object's frame data
/************************************************************************/

/************************************************************************/
BOOL TransformLine( LPOBJECT lpObject, int x1, int y, int dx, LPTR lpDataBuf,
	int iDataDepth, LPTR lpAlphaBuf, int iAlphaDepth,
	LPDISTORT lpDistort, BOOL fHiQuality)
/************************************************************************/
{
int     i, x2, nx, ny, right, bottom, iCount;
LPTR    lpAlphaSrc, lpAlphaDst;
LPTR    lpDataSrc,  lpDataDst;
LPRGB   lpRGBSrc,   lpRGBDst;
LPCMYK  lpCMYKSrc,  lpCMYKDst;
LPFRAME lpDataFrame, lpAlphaFrame;
LPALPHA lpAlpha;
LFIXED  f, fnx, fny, lnx, lny, fMin, fMax, fxRate, fyRate;
LFIXED	fnegone, fright, fbottom;
BYTE    ClearByte;
RGBS    ClearRGB;
CMYKS   ClearCMYK;
FRMDATATYPE Type;

if ( !lpObject || !dx )
	return( NO );

lpDataFrame = ( lpDataBuf ? lpObject->Pixmap.EditFrame : NULL );
if ( lpAlpha = lpObject->lpAlpha )
	lpAlphaFrame = ( lpAlphaBuf ? lpAlpha->Pixmap.EditFrame : NULL );
else
	lpAlphaFrame = NULL;

// If nothing to do
if ( !lpDataFrame && !lpAlphaFrame )
	return( NO );

// See if it's alpha only
if ( !lpDataFrame )
	return( TransformSingleLine( lpObject, x1, y, dx,
			lpAlphaFrame, lpAlphaBuf, iAlphaDepth, lpDistort, fHiQuality ) );

// See if it's data only
if ( !lpAlphaFrame )
	return( TransformSingleLine( lpObject, x1, y, dx,
		lpDataFrame, lpDataBuf, iDataDepth, lpDistort, fHiQuality ) );

// It's data and alpha both

// WARNING: THIS ROUTINE IS ALMOST A DUPLICATE OF THE ONE BELOW
// PLEASE MAINTAIN BOTH WHEN MAKING BUG FIXES AND UPDATES
if (fHiQuality)
	{
	Type = FrameType(lpDataFrame);
	fHiQuality = CANDOTRANSPARENCY(Type);
	}

// Make sure the line intersects the transformed object in some way
nx = RectWidth(&lpObject->rObject);
ny = RectHeight(&lpObject->rObject);

x2 = x1 + dx - 1;
if ( y < 0 || y >= ny || x2 < 0 || x1 >= nx )
	return( NO );

// Clear the output buffers since we may not write to each location
if ( iDataDepth == 4 )
		set( lpDataBuf, dx * iDataDepth, 255 );
else	clr( lpDataBuf, dx * iDataDepth );
if ( iAlphaDepth == 4 )
		set( lpAlphaBuf, dx * iAlphaDepth, 255 );
else	clr( lpAlphaBuf, dx * iAlphaDepth );

// If the x1 coordinate is too far left, adjust dx and the data pointers
if ( x1 < 0 )
	{
	lpDataBuf -= (x1 * iDataDepth);
	lpAlphaBuf -= (x1 * iAlphaDepth);
	dx += x1;
	}

// If the x2 coordinate is too far right, adjust dx
if ( x2 >= nx )
	{
	dx -= (x2 - nx + 1);
	}

// Map a y scan line (in the new output distorted space)...
// back to the original bounding rect
y += lpObject->rObject.top;

// The output span between fMin and fMax is the number of
// pixels to read between input points (fnx,fny) and (lnx,lny)
if (!lpDistort)
	lpDistort = &Distort;
IntersectCorners( lpDistort, y, &fMin, &fMax, &fnx, &fny, &lnx, &lny );
f = MAKEFIXED(lpObject->rTransform.top);
fny -= f;
lny -= f;
f = MAKEFIXED(lpObject->rTransform.left); 
fnx -= f;
lnx -= f;

// Compute the pixel count for the rate calculations
f = fMax - fMin;
iCount = FROUND(fMax) - FROUND(fMin);

// Compute the x and y rates to from the first input point to the last
if ( !iCount )
	{
	fxRate = FUNITY;
	fyRate = -FUNITY;
	}
else
	{
	fxRate = (lnx - fnx) / iCount;
	fyRate = (lny - fny) / iCount;
	}

iCount++; // Now its a loop count

// Compute the buffer indent
i = FROUND(fMin) - lpObject->rObject.left - x1;
if ( i >= dx )
	return( YES ); // Nothing to do

// Handle an off-image start
if ( i < 0 )
	{
	fnx -= (fxRate * i);
	fny -= (fyRate * i);
	iCount += i;
	i = 0;
	}
if ( iCount + i > dx )
	iCount = dx - i; // Don't go past the end of the buffers
if ( iCount <= 0 )
	return( YES ); // Nothing to do

// Indent the buffers
lpDataDst  = lpDataBuf + (i * iDataDepth);
lpAlphaDst = lpAlphaBuf + (i * iAlphaDepth);

// Make this a frame size check
right  = FrameXSize( lpDataFrame ) - 1;
bottom = FrameYSize( lpDataFrame ) - 1;

ClearByte = 0;
ClearRGB.red = ClearRGB.green = ClearRGB.blue  = 0;
ClearCMYK.c = ClearCMYK.m = ClearCMYK.y = ClearCMYK.k = 255;

if ( fHiQuality )
	{
	fright  = MAKEFIXED(right + 1);
	fbottom = MAKEFIXED(bottom + 1);
	fnegone = MAKEFIXED(-1);

	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					 fnx >= fright  || fny >= fbottom )
					{
					*lpDataDst++  = ClearByte;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					average_pixel8( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpDataDst++ );
					average_pixel8( lpAlphaFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpAlphaDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 3 :
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					 fnx >= fright  || fny >= fbottom )
					{
					*lpRGBDst++   = ClearRGB;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					average_pixel24( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearRGB, lpRGBDst++ );
					average_pixel8( lpAlphaFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpAlphaDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					 fnx >= fright  || fny >= fbottom )
					{
					*lpCMYKDst++  = ClearCMYK;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					average_pixel32( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearCMYK, lpCMYKDst++ );
					average_pixel8( lpAlphaFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpAlphaDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}
else
	{
	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			{
			while ( --iCount >= 0 )
				{
				nx = FROUND(fnx);
				ny = FROUND(fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpDataSrc = FramePointer( lpDataFrame, nx, ny, NO, 1 )) ||
					!(lpAlphaSrc = FramePointer( lpAlphaFrame, nx, ny, NO, 1 )) )
					{
					*lpDataDst++  = ClearByte;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					*lpDataDst++  = *lpDataSrc;
					*lpAlphaDst++ = *lpAlphaSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;

		case 3 :
			{
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = FROUND(fnx);
				ny = FROUND(fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpRGBSrc = (LPRGB)FramePointer( lpDataFrame, nx, ny, NO, 1 )) ||
					!(lpAlphaSrc = FramePointer( lpAlphaFrame, nx, ny, NO, 1 )) )
					{
					*lpRGBDst++   = ClearRGB;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					*lpRGBDst++   = *lpRGBSrc;
					*lpAlphaDst++ = *lpAlphaSrc;
					}
				fnx += fxRate;
				fny += fyRate;
			}
		}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = FROUND(fnx);
				ny = FROUND(fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpCMYKSrc = (LPCMYK)FramePointer( lpDataFrame, nx, ny, NO, 1 )) ||
					!(lpAlphaSrc = FramePointer( lpAlphaFrame, nx, ny, NO, 1 )) )
					{
					*lpCMYKDst++  = ClearCMYK;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					*lpCMYKDst++  = *lpCMYKSrc;
					*lpAlphaDst++ = *lpAlphaSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}

return( YES );
}

/************************************************************************/
// This routines take NEW frame coordiantes x1, y, dx,
// and stuffs the passed data OR alpha buffers
// given a transform BACK to the passed object's frame data
/************************************************************************/

/************************************************************************/
LOCAL BOOL TransformSingleLine( LPOBJECT lpObject, int x1, int y, int dx,
	LPFRAME lpDataFrame, LPTR lpDataBuf, int iDataDepth, LPDISTORT lpDistort,
	BOOL fHiQuality )
/************************************************************************/
{
int    i, x2, nx, ny, right, bottom, iCount;
LPTR   lpDataSrc, lpDataDst;
LPRGB  lpRGBSrc,  lpRGBDst;
LPCMYK lpCMYKSrc, lpCMYKDst;
LFIXED f, fnx, fny, lnx, lny, fMin, fMax, fxRate, fyRate;
LFIXED fnegone, fright, fbottom;
BYTE   ClearByte;
RGBS   ClearRGB;
CMYKS  ClearCMYK;
FRMDATATYPE Type;

// WARNING: THIS ROUTINE IS ALMOST A DUPLICATE OF THE ONE ABOVE
// PLEASE MAINTAIN BOTH WHEN MAKING BUG FIXES AND UPDATES
if (fHiQuality)
	{
	Type = FrameType(lpDataFrame);
	fHiQuality = CANDOTRANSPARENCY(Type);
	}

if ( !lpObject || !lpDataFrame || !lpDataBuf || !dx )
	return( NO );

// Make sure the line intersects the transformed object in some way
nx = RectWidth(&lpObject->rObject);
ny = RectHeight(&lpObject->rObject);

x2 = x1 + dx - 1;
if ( y < 0 || y >= ny || x2 < 0 || x1 >= nx )
	return( NO );

// Clear the output buffers since we may not write to each location
if ( iDataDepth == 4 )
		set( lpDataBuf, dx * iDataDepth, 255 );
else	clr( lpDataBuf, dx * iDataDepth );

// If the x1 coordinate is too far left, adjust dx and the data pointers
if ( x1 < 0 )
	{
	lpDataBuf -= (x1 * iDataDepth);
	dx += x1;
	}

// If the x2 coordinate is too far right, adjust dx
if ( x2 >= nx )
	{
	dx -= (x2 - nx + 1);
	}

// Map a y scan line (in the new output distorted space)...
// back to the original bounding rect
y += lpObject->rObject.top;

// The output span between fMin and fMax is the number of
// pixels to read between input points (fnx,fny) and (lnx,lny)
if (!lpDistort)
	lpDistort = &Distort;
IntersectCorners( lpDistort, y, &fMin, &fMax, &fnx, &fny, &lnx, &lny );
f = MAKEFIXED(lpObject->rTransform.top); 
fny -= f;
lny -= f;
f = MAKEFIXED(lpObject->rTransform.left);
fnx -= f;
lnx -= f;

// Compute the pixel count for the rate calculations
f = fMax - fMin;
iCount = FROUND(fMax) - FROUND(fMin);

// Compute the x and y rates to from the first input point to the last
if ( !iCount )
	{
	fxRate = FUNITY;
	fyRate = -FUNITY;
	}
else
	{
	fxRate = (lnx - fnx) / iCount;
	fyRate = (lny - fny) / iCount;
	}

iCount++; // Now its a loop count

// Compute the buffer indent
i = FROUND(fMin) - lpObject->rObject.left - x1;
if ( i >= dx )
	return( YES ); // Nothing to do

// Handle an off-image start
if ( i < 0 )
	{
	fnx -= (fxRate * i);
	fny -= (fyRate * i);
	iCount += i;
	i = 0;
	}
if ( iCount + i > dx )
	iCount = dx - i; // Don't go past the end of the buffers
if ( iCount <= 0 )
	return( YES ); // Nothing to do

// Indent the buffers
lpDataDst  = lpDataBuf + (i * iDataDepth);

// Make this a frame size check
right  = FrameXSize( lpDataFrame ) - 1;
bottom = FrameYSize( lpDataFrame ) - 1;

ClearByte = 0;
ClearRGB.red = ClearRGB.green = ClearRGB.blue  = 0;
ClearCMYK.c = ClearCMYK.m = ClearCMYK.y = ClearCMYK.k = 255;

if ( fHiQuality )
	{
	fright  = MAKEFIXED(right + 1);
	fbottom = MAKEFIXED(bottom + 1);
	fnegone = MAKEFIXED(-1);

	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					 fnx >= fright  || fny >= fbottom )
					{
					*lpDataDst++  = ClearByte;
					}
				else
					{
					average_pixel8( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpDataDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 3 :
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					 fnx >= fright  || fny >= fbottom )
					{
					*lpRGBDst++   = ClearRGB;
					}
				else
					{
					average_pixel24( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearRGB, lpRGBDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					 fnx >= fright  || fny >= fbottom )
					{
					*lpCMYKDst++  = ClearCMYK;
					}
				else
					{
					average_pixel32( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearCMYK, lpCMYKDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}
else
	{
	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			{
			while ( --iCount >= 0 )
				{
				nx = FROUND(fnx);
				ny = FROUND(fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpDataSrc = FramePointer( lpDataFrame, nx, ny, NO, 1 )) )
					{
					*lpDataDst++  = ClearByte;
					}
				else
					{
					*lpDataDst++  = *lpDataSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;

		case 3 :
			{
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = FROUND(fnx);
				ny = FROUND(fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpRGBSrc = (LPRGB)FramePointer( lpDataFrame, nx, ny, NO, 1 )) )
					{
					*lpRGBDst++   = ClearRGB;
					}
				else
					{
					*lpRGBDst++   = *lpRGBSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = FROUND(fnx);
				ny = FROUND(fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpCMYKSrc = (LPCMYK)FramePointer( lpDataFrame, nx, ny, NO, 1 )) )
					{
					*lpCMYKDst++  = ClearCMYK;
					}
				else
					{
					*lpCMYKDst++  = *lpCMYKSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}

return( YES );
}

/************************************************************************/
void CObjectDrawHook::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect)
/************************************************************************/
{
LPOBJECT lpObject;
RECT rUpdate;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
lpObject = ImgGetSelObject(lpImage, NULL);
if (lpObject && !ObjGetEditFrame(lpObject))
	{
	ImgGetSelObjectRect(lpImage, &rUpdate, YES);
	InflateRect(&rUpdate, 1, 1);
	File2DispRect(hWnd, &rUpdate, &rUpdate);
	if (lpRect)
		{
		if (AstralIntersectRect(&rUpdate, &rUpdate, lpRect))
			PaintMarquee(hWnd, hDC, lpObject, NULL, YES, &rUpdate, 0, 0, 0, 0);
		}
	else
		PaintMarquee(hWnd, hDC, lpObject, NULL, YES, &rUpdate, 0, 0, 0, 0);
	}
TrackDraw( hWnd, hDC, ON );
}


/************************************************************************/
LOCAL BOOL IntersectCorners( LPDISTORT lpDistort, int y, LPLFIXED lpX1, LPLFIXED lpX2,
	LPLFIXED lpFx1, LPLFIXED lpFy1, LPLFIXED lpFx2, LPLFIXED lpFy2 )
/************************************************************************/
{
LFIXED fPercent, lx, lxx[4], lpx[4], lpy[4], lxmin, lxmax;
int i, one, two;
#define P1 lpDistort->p[0]
#define P2 lpDistort->p[1]
#define P3 lpDistort->p[2]
#define P4 lpDistort->p[3]
#define DR lpDistort->RectOrig

i = 0;
if ( IntersectSegment( y, P1, P2, &lx, &fPercent ) )
	{ // iEdgeType = t0V;
	lxx[i] = lx;
	lpx[i] = (fPercent * (DR.right - DR.left)) + MAKEFIXED(DR.left);
	lpy[i] = MAKEFIXED(DR.top);
	i++;
	}

if ( IntersectSegment( y, P2, P3, &lx, &fPercent ) )
	{ // iEdgeType = tU1;
	lxx[i] = lx;
	lpx[i] = MAKEFIXED(DR.right);
	lpy[i] = (fPercent * (DR.bottom - DR.top)) + MAKEFIXED(DR.top);
	i++;
	}

if ( IntersectSegment( y, P3, P4, &lx, &fPercent ) )
	{ // iEdgeType = t1V;
	lxx[i] = lx;
	lpx[i] =  -(fPercent * (DR.right - DR.left)) + MAKEFIXED(DR.right);
	lpy[i] = MAKEFIXED(DR.bottom);
	i++;
	}

if ( IntersectSegment( y, P4, P1, &lx, &fPercent ) )
	{ // iEdgeType = tU0;
	lxx[i] = lx;
	lpx[i] = MAKEFIXED(DR.left);
	lpy[i] = -(fPercent * (DR.bottom - DR.top)) + MAKEFIXED(DR.bottom);
	i++;
	}

switch ( i )
	{ // how many intersections did we get?
	case 2: // Perrrrfect!
	one = 0;
	two = 1;
	break;

	case 1: // Huh?
	one = 0;
	two = 0;
	break;

	case 3: // There must be a duplicate
	one = 0;
	two = ( lxx[1] != lxx[0] ? 1 : 2 );
	break;

	case 4: // Find the smallest and largest x
	lxmin = min( min( min( lxx[0], lxx[1] ), lxx[2] ), lxx[3] );
	lxmax = max( max( max( lxx[0], lxx[1] ), lxx[2] ), lxx[3] );
	one = ( lxx[0] == lxmin ? 0 :
		  ( lxx[1] == lxmin ? 1 :
		  ( lxx[2] == lxmin ? 2 : 3 ) ) );
	two = ( lxx[0] == lxmax ? 0 :
		  ( lxx[1] == lxmax ? 1 :
		  ( lxx[2] == lxmax ? 2 : 3 ) ) );
	break;

	case 0:
	// Didn't get any; bail out with null values
	*lpX1 = *lpX2 = *lpFx1 = *lpFx2 = *lpFy1 = *lpFy2 = 0;
	return( NO );
	}

// Check to see if we need to swap
if ( lxx[two] < lxx[one] )
	{ i = one; one = two; two = i; }

*lpX1  = lxx[one];
*lpX2  = lxx[two];
*lpFx1 = lpx[one];
*lpFx2 = lpx[two];
*lpFy1 = lpy[one];
*lpFy2 = lpy[two];

return( YES );
}


/************************************************************************/
// Does y intersect this segment? If so, at what x and percentage complete?
/************************************************************************/
LOCAL BOOL IntersectSegment( int y, POINT pa, POINT pb, LPLFIXED lpX,
	LPLFIXED lpPercent )
/************************************************************************/
{ // Does y intersect this segment? If so, at what x and percentage complete?
long     ldy;

if ( (y < pa.y && y < pb.y) || (y > pa.y && y > pb.y) )
	return( NO );

/* if the segment is horizontal, forget about it */
if ( !(ldy = pb.y - pa.y) )
	return( NO );

if ( ldy > 0 )
		*lpPercent = FGET( y - pa.y, ldy );
else	*lpPercent = FUNITY - FGET( y - pb.y, -ldy );

*lpX = MAKEFIXED(pa.x) + (*lpPercent * (pb.x - pa.x));
return( YES );
}


LOCAL STATUS_CODE TransformEndCreateMaskFromObject(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly)
   {
   LPOBJECT lpBase, lpObject;
   LOADMASK_PARMS parms;
   RECT rUpdate;
   int i;
   STATUS_CODE StatusCode;

   StatusCode = LoadMaskEnd(lpImage, fCancel, lpOldMask, MASKMODES, idTransformDirty, &Distort, &rUpdate);
	if (StatusCode == SC_SUCCESS)
   	{
      lpBase   = ImgGetBase(lpImage);
      lpObject = lpBase;
      lpBase->fSelected = FALSE;
      while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
         if (lpObject->fUndoSelected)
            {
            lpObject->fUndoSelected = FALSE;
            lpObject->fSelected = TRUE;
            }
      UpdateImage(lpImage, NULL, YES);
	  parms.Mode = MASKMODES;
	  parms.iBaseRes = FrameResolution(ImgGetBaseEditFrame(lpImage));
	  for (i = 0; i < 4; ++i)
	  	parms.DistortPoints[i] = Distort.p[i];
	  PostCommand(lpImage->lpCmdList, IDS_CMD_MASKFROMOBJECT, &parms);
	}
	return(StatusCode);
   }

void TransformStartCreateMaskFromObjectProc(LPIMAGE lpImage)
   {
   lpOldMask = NULL;
   TransformObjectsStart(lpImage, TransformEndCreateMaskFromObject, IDC_MASKTRANSFORM, NO,
	   IDS_UNDOMASKFROMOBJECT, NO, NULL);
   }


BOOL MaskFromObjectWithMask(LPIMAGE lpImage)
   {
   LPTR     lpDataBuf, lpMaskPtr;
   LPOBJECT lpBase, lpObject;
   BOOL     fTransform = FALSE;
   BOOL     fNewMask = FALSE;
   LPMASK   lpMask;
   RECT     rSel;
   int      y,s,e,l,iWidth,iHeight;
   
   // get rectangle for all floating objects
   if (!ImgGetSelObjectRect(lpImage, &rSel, YES))
       return(FALSE);
   
   iWidth   = RectWidth(&rSel);
   iHeight  = RectHeight(&rSel);
   lpMask   = ImgGetMask(lpImage);
   lpBase   = ImgGetBase(lpImage);
   
   SaveSelectedObjects(lpImage);
   lpObject = lpBase;

   FRMTYPEINFO TypeInfo;
   FRMDATATYPE FrameType = FDT_GRAYSCALE;

	FrameGetTypeInfo(ObjGetEditFrame(lpBase), &TypeInfo);
   if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
      FrameType = FDT_LINEART;

   if (!(lpMask = MaskCreate(NULL, iWidth, iHeight, OFF, TRUE, FrameType)))
      return(FALSE);
   fTransform = TRUE;
   s = rSel.top;
   e = rSel.bottom;
   l = rSel.left;
   
   if (lpObject = ImgGetSelObject(lpImage, lpObject))
      {
      // allocate buffer for alpha channel combining
      if (!(lpDataBuf = Alloc((long)iWidth)))
         {
         MaskClose(lpMask);
         Message(IDS_EMEMALLOC);
         return(FALSE);
         }
   
      ProgressBegin(1, 0);
      for (y = s; y <= e; y++)
         {
         AstralClockCursor(y-s, iHeight, NO);
         lpMaskPtr = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, 0, y - s, YES);
         if (!lpMaskPtr)
            continue;
         ImgMaskLoad(lpImage, lpObject, NULL, rSel.left, y, iWidth, 
            lpDataBuf, YES, CR_OR, 0, 0);
         CombineData(lpDataBuf, lpMaskPtr, iWidth, NO, CR_OR);
         }
      ProgressEnd();
      FreeUp(lpDataBuf);

	   if (!(lpObject = ObjCreateFromFrame(ST_TEMPORARY, NULL, lpMask,
		   &rSel, Control.NoUndo)))
		   {
		   Message(IDS_EMEMALLOC);
		   MaskClose( lpMask);
		   return(FALSE);
		   }
	   ImgAddNewObject(lpImage, lpObject);
  	   TransformStartCreateMaskFromObjectProc(lpImage);
      }
   return(TRUE);
   }

/***********************************************************************/
BOOL MaskFromObjStart(LPIMAGE lpImage, LPALPHA lpAlpha,
					BOOL fTransform, LPRECT lpUpdateRect, LPBOOL lpfTransform)
/***********************************************************************/
{
LPOBJECT lpBase, lpObject;
LPFRAME lpFrame;
int iWidth, iHeight, w, h;
LPMASK lpOldMask;
RECT rAlpha;

SaveSelectedObjects(lpImage);
ImgGetInfo(lpImage, &iWidth, &iHeight, NULL, NULL);
lpOldMask = ImgGetMask(lpImage);
w = FrameXSize(lpAlpha->Pixmap.EditFrame);
h = FrameYSize(lpAlpha->Pixmap.EditFrame);
if (w == iWidth && h == iHeight && !lpOldMask && !fTransform)
	{
	lpBase = ImgGetBase(lpImage);
	GetMaskMarqueeRect(lpImage, &rAlpha);
	AstralUnionRect(lpUpdateRect, lpUpdateRect, &rAlpha);
	if (!lpOldMask)
		ImgSetMask(lpImage, lpAlpha);

	if (!ImgEditInit(lpImage, ET_OBJECT,
				lpOldMask != NULL ? UT_NEWALPHA : UT_CREATEMASK, lpBase))
		{
		MaskClose(lpAlpha);
		return(FALSE);
		}
	if (lpOldMask)
		{
		lpFrame = lpAlpha->Pixmap.EditFrame;
		lpAlpha->Pixmap.EditFrame = NULL;
		MaskClose(lpAlpha);
		SetRect(&rAlpha, 0, 0, iWidth-1, iHeight-1);
		ImgEditedObjectFrame(lpImage, lpBase, IDS_UNDOLOADMASK,
									&rAlpha, NULL, lpFrame);
		}
	else
		ImgEditedObject(lpImage, lpBase, IDS_UNDOLOADMASK, NULL);

	MaskRectUpdate(ImgGetMask(lpImage), &rAlpha);

	// update appropriate area of the display
	if (ImgGetMaskUpdateRect(lpImage, lpOldMask == NULL, NO, &rAlpha))
		AstralUnionRect(lpUpdateRect, lpUpdateRect, &rAlpha);

	*lpfTransform = FALSE;
	}
else
	{
	rAlpha.left = rAlpha.top = 0;
	rAlpha.right = w - 1;
	rAlpha.bottom = h - 1;
	if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY, NULL, lpAlpha,
				&rAlpha, Control.NoUndo )) )
		{
		Message(IDS_EMEMALLOC);
		MaskClose( lpAlpha );
		return(FALSE);
		}
	ImgAddNewObject(lpImage, lpObject);
	*lpfTransform = TRUE;
	}
return(TRUE);
}

BOOL MaskFromObjectWithoutMask(LPIMAGE lpImage, LPMASKFROMOBJECT_PARMS lpParms)
   {
   LPTR     lpDataBuf, lpMaskPtr;
   LPOBJECT lpBase, lpObject;
   BOOL     fTransform = FALSE;
   BOOL     fNewMask = FALSE;
   RECT     rUpdate;
   LPMASK   lpMask;
   RECT     rSel;
   int      y,s,e,l,iWidth,iHeight, dx, dy;
   COMBINE_ROP    ROP;
   BOOL     bInvert = FALSE;
   
   if (ImgGetMask(lpImage))
	   	return(FALSE);

   // get rectangle for all floating objects
   if (!ImgGetSelObjectRect(lpImage, &rSel, YES))
       return(FALSE);

   ImgGetInfo(lpImage, &dx, &dy, NULL, NULL);
   BoundRect(&rSel, 0, 0, dx-1, dy-1);

   if (AstralIsRectEmpty(&rSel))
   		return(FALSE);

   ROP = CR_OR;
   iWidth   = RectWidth(&rSel);
   iHeight  = RectHeight(&rSel);
   lpMask   = ImgGetMask(lpImage);
   lpBase   = lpObject = ImgGetBase(lpImage);
   
   if (!(lpMask = ImgGetMaskEx(lpImage, OFF, NO, &fNewMask, &rUpdate)))
      return(FALSE);
	lpMask->rMask = rSel;
   s = rSel.top;
   e = rSel.bottom;
   l = rSel.left;
   if (!ImgEditInit(lpImage, ET_OBJECT, fNewMask ? UT_CREATEMASK:UT_ALPHA, lpBase))
  	   {
      MaskClose(lpMask);
      Message(IDS_EMEMALLOC);
	   return(FALSE);
  	   }

   if (lpObject = ImgGetSelObject(lpImage, lpObject))
      {
      if (!(lpDataBuf = Alloc((long)iWidth)))
         {
         MaskClose(lpMask);
         Message(IDS_EMEMALLOC);
         return(FALSE);
         }
   
      ProgressBegin(1, 0);
      for (y = s; y <= e; y++)
         {
         AstralClockCursor(y-s, iHeight, NO);
         lpMaskPtr = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, l, y, YES);
         if (!lpMaskPtr)
            continue;
         ImgMaskLoad(lpImage, lpObject, NULL, rSel.left, y, iWidth, 
            lpDataBuf, YES, CR_OR, 0, 0);
         CombineData(lpDataBuf, lpMaskPtr, iWidth, bInvert, ROP);
         }
      ProgressEnd();
      FreeUp(lpDataBuf);

	   ImgEditedObject(lpImage, lpBase, IDS_UNDOMASKFROMOBJECT, (LPRECT)NULL);

	   // add selected area to area created by ImgGetMaskEx()
	   AstralUnionRect(&rUpdate, &rUpdate, &rSel);
	   if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rSel))
		   AstralUnionRect(&rUpdate, &rUpdate, &rSel);

		lpParms->Common.UpdateType = UT_ACTIVEAREA;
		lpParms->Common.rUpdateArea = rUpdate;

      }
   return(TRUE);
   }

BOOL MaskFromObject(LPIMAGE lpImage, LPMASKFROMOBJECT_PARMS lpParms)
   {
   LPTR     lpDataBuf, lpMaskPtr;
   LPOBJECT lpBase, lpObject;
   BOOL     fTransform = FALSE;
   BOOL     fNewMask = FALSE;
   RECT     rUpdate;
   LPMASK   lpMask;
   RECT     rSel;
   int      res, y,s,e,l,iWidth,iHeight;
   COMBINE_ROP    ROP;
   int      i, Mode = lpParms->Mode;
   BOOL     bInvert = FALSE;
   DISTORT  Distort;

   if (!ImgGetMask(lpImage))
   		return(MaskFromObjectWithoutMask(lpImage, lpParms));

   // get rectangle for all floating objects
   if (!ImgGetSelObjectRect(lpImage, &rSel, YES))
       return(FALSE);
   
   if (Mode == SHAPE_REVERSE)
	   ROP = CR_XOR;
   else if (Mode == SHAPE_SUBTRACT)
	   {
	   ROP = CR_AND;
	   bInvert = !bInvert;
	   }
   else
	   ROP = CR_OR;
   iWidth   = RectWidth(&rSel);
   iHeight  = RectHeight(&rSel);
   lpBase   = lpObject = ImgGetBase(lpImage);
   
   FRMTYPEINFO TypeInfo;
   FRMDATATYPE FrameType = FDT_GRAYSCALE;

	FrameGetTypeInfo(ObjGetEditFrame(lpBase), &TypeInfo);
   if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
      FrameType = FDT_LINEART;

   if (!(lpMask = MaskCreate(NULL, iWidth, iHeight, ON, TRUE, FrameType)))
      return(FALSE);
   s = rSel.top;
   e = rSel.bottom;
   l = rSel.left;

   SaveSelectedObjects(lpImage);
   if (lpObject = ImgGetSelObject(lpImage, lpObject))
      {
      if (!(lpDataBuf = Alloc((long)iWidth)))
         {
         MaskClose(lpMask);
         Message(IDS_EMEMALLOC);
         return(FALSE);
         }
   
      ProgressBegin(1, 0);
      for (y = s; y <= e; y++)
         {
         AstralClockCursor(y-s, iHeight, NO);
         lpMaskPtr = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, 0, y-s, YES);
         if (!lpMaskPtr)
            continue;
         ImgMaskLoad(lpImage, lpObject, NULL, rSel.left, y, iWidth, 
            lpMaskPtr, YES, CR_OR, 0, 0);
//            lpDataBuf, YES, CR_OR, 0, 0);
//         CombineData(lpDataBuf, lpMaskPtr, iWidth, bInvert, ROP);
         }
      ProgressEnd();
      FreeUp(lpDataBuf);

      }

   GetObjectMarqueeRect(lpImage, &rUpdate);
   if (!MaskFromObjStart(lpImage, lpMask, YES, &rUpdate, &fTransform))
	   return(FALSE);

   res = FrameResolution(ImgGetBaseEditFrame(lpImage));
   res_convert_disort(lpParms->iBaseRes, res, Distort.p, lpParms->DistortPoints); 

   ImgGetSelObjectRect(lpImage, &Distort.RectOrig, YES);
   TransformObjectsInit(lpImage, YES, &Distort);

   lpParms->Common.StatusCode = LoadMaskEnd(lpImage, NO, NULL, Mode, lpParms->Common.idDirty, &Distort, &rUpdate);
   ImgDeleteTempObjects(lpImage);

	RestoreSelectedObjects(lpImage);
   if (!AstralIsRectEmpty(&rUpdate))
	   {
	   lpParms->Common.UpdateType = UT_AREA;
	   lpParms->Common.rUpdateArea = rUpdate;
	   }
   return(lpParms->Common.StatusCode == SC_SUCCESS);
   }

BOOL ObjectFromMask(LPIMAGE lpImage, LPOBJECTFROMMASK_PARMS lpParms)
   {
   LPOBJECT lpBase, lpObject;
   RECT     rUpdate;
   RECT     rSel, rSelObj, rMarquee, rMaskUpdate;
   int      depth,iImageWidth, iImageHeight;
   
   // if no mask, get the fuck out!!!
   if (!ImgGetMaskRect(lpImage, &rSel))
	   return(FALSE);

   lpBase = ImgGetBase(lpImage);
   PixmapGetInfo(&lpBase->Pixmap, PMT_EDIT, &iImageWidth, &iImageHeight, &depth, NULL);
   BoundRect(&rSel, 0, 0, iImageWidth-1, iImageHeight-1);
   ImgGetSelObjectRect(lpImage, &rSelObj, YES);
   
   lpObject = ImgCreateCutoutObject(lpImage, &rSel);
   if (!lpObject) // not enough memory
        return(FALSE);
	// need to call this cause ImgCreateCutoutObject creates
	// temporary objects
	ImgKeepFrameTempObjects(lpImage);

    // ImgEditInit cannot fail here, so don't worry about cleanup
    // of lpObject
   if (!ImgEditInit(lpImage, ET_OBJECT, UT_DELETEMASK | UT_DELETEOBJECTS,lpBase))
	   return(FALSE);

	GetMaskMarqueeRect(lpImage, &rMarquee);
	if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rMaskUpdate))
		AstralUnionRect(&rMarquee, &rMarquee, &rMaskUpdate);
   ImgSetMask(lpImage, NULL);
   if (!Control.MultipleObjects)
	   {
	   ImgCombineObjects(lpImage, NO, YES, YES, IDS_UNDOOBJECTFROMMASK, &rUpdate);
	   AstralUnionRect(&rSel, &rSel, &rUpdate);
	   ImgPurgeObjects(lpImage);
	   }
   else
      {
	   lpObject->fUndoDeleted = YES;
	   ImgEditedObject(lpImage, lpBase, IDS_UNDOOBJECTFROMMASK, NULL);
      }
   AstralUnionRect(&rSel, &rSel, &rSelObj);
   AstralUnionRect(&rSel, &rSel, &rMarquee);
   if (!AstralIsRectEmpty(&rSel))
	   {
	   lpParms->Common.UpdateType =  UT_AREA;
	   lpParms->Common.rUpdateArea = rSel;
	   }
   return(TRUE);
   }




//************************************************************************
//	converts Distort.p from one res to another
//************************************************************************
LOCAL void res_convert_disort(int iBaseRes,int res, LPPOINT pDistortOut, LPPOINT pDistortIn)
//************************************************************************
{
	int i, dummy;
	
	for (i = 0; i < 4; ++i)
		pDistortOut[i] = pDistortIn[i];
	
	// upper left
	ResConvertUL(iBaseRes, res, &pDistortOut[0].x, &pDistortOut[0].y);
	// upper right
	ResConvertUL(iBaseRes, res, (LPLONG)&dummy, &pDistortOut[1].y);
	ResConvertLR(iBaseRes, res, &pDistortOut[1].x, (LPLONG)&dummy);
	// lower right
	ResConvertLR(iBaseRes, res, &pDistortOut[2].x, &pDistortOut[2].y);
	// lower left
	ResConvertLR(iBaseRes, res, (LPLONG)&dummy, &pDistortOut[3].y);
	ResConvertUL(iBaseRes, res, &pDistortOut[3].x, (LPLONG)&dummy);
}


//************************************************************************
// 	Saves the selection state of all objects in fUndoSelect
//************************************************************************
void SaveSelectedObjects(LPIMAGE lpImage)
//************************************************************************
{
	LPOBJECT lpObject = NULL;
    while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
    {
    	lpObject->fUndoSelected = lpObject->fSelected;
	}
}

//************************************************************************
// 	 restores the selection state of all objects from  fUndoSelect
//************************************************************************
void RestoreSelectedObjects(LPIMAGE lpImage)
//************************************************************************
{
	LPOBJECT lpObject = NULL;
    while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
    {
    	 lpObject->fSelected = lpObject->fUndoSelected;
		 lpObject->fUndoSelected = FALSE;
	}
}


