// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "pp.h"
#include "ppafx.h"
#include "svrview.h"
#include "dataitem.h"
#include "id.h"
#include "objfloat.h"

#define TITLE_HEIGHT		   9
#define BORDER_WIDTH		   1
#define SYS_MENU_WIDTH	   12
#define SYS_COOKIE_OFFSET	3
#define SYS_COOKIE_WIDTH	7
#define SYS_COOKIE_HEIGHT	4
#define COLOR_SYSMENU		RGB(192, 192, 192)
#define COLOR_SYSSHADOW		RGB(128, 128, 128)
#define COLOR_COOKIEFILL	RGB(255, 255, 255)
#define COLOR_BORDER		   RGB(0,   0,   0)

LOCAL DROPEFFECT DoDragDrop(HWND hWindow, int MouseX, int MouseY, 
  DWORD dwEffects = DROPEFFECT_COPY, BOOL bStartImmediate = FALSE, BOOL DragObj = FALSE);
LOCAL BOOL ObjectToolsEnabled(LPIMAGE lpImage);
LOCAL BOOL SetObjectOpacity(LPIMAGE lpImage, BYTE Opacity);
LOCAL BOOL SetObjectMergeMode(LPIMAGE lpImage, MERGE_MODE MergeMode);
LOCAL BOOL SetObjectLayering(LPIMAGE lpImage, BOOL fToTop, BOOL fOneLevel);
LOCAL BOOL MoveObjects(LPIMAGE lpImage, int xorg, int yorg, int xnew, int ynew);
LOCAL BOOL SelectObjects(LPIMAGE lpImage, LPRECT lpRect, BOOL fExtendedSel);
LOCAL BOOL DuplicateObjects(LPIMAGE lpImage);
LOCAL void SelectorProc_OnCreate( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnSetCursor( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnLButtonDown( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnLButtonUp( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnMouseMove( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnKeyDown( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnTimer( HWND hWindow, int x, int y, UINT32 Option );
LOCAL void SelectorProc_OnDestroy( HWND hWindow, int x, int y, UINT32 Option );


LOCAL DROPEFFECT DoDragDrop(HWND hWindow, int MouseX, int MouseY, 
	DWORD dwEffects,BOOL bStartImmediate, BOOL DragObj)
{
	LPIMAGE lpImage = GetImagePtr(hWindow);
	ASSERT(lpImage);
	CServerDoc *pDoc = PictPubApp.GetDocument(lpImage);
	ASSERT(pDoc);

	CPoint point(MouseX,MouseY);
	RECT rMask;
	LPCMDPKT lpCmdPkt;
	LPOBJECT lpObject;
	LPIMAGE lpNewImage = NULL;
	CDataItem *pDataItem = NULL;
	
	// Note: the code below is almost identical to the code
	// in doccmds.cpp funtion CServerDoc::OnCopy()
	AstralSetRectEmpty((LPRECT)&rMask);
	if (!ImgGetSelObjectRect( lpImage, &rMask, TRUE ))
		ImgGetMaskRect(lpImage, &rMask);

	// Make sure that ImgCreate drops into the first case
	if (!(lpObject = ImgCreateClipOutObject(lpImage, DragObj)))
		AfxThrowMemoryException();

	if (!(lpNewImage = CreateImage(NULL,NULL,lpObject,NULL,      
		IDN_PP,lpImage->DataType,lpImage->DocumentType,NULL))) 
		AfxThrowMemoryException();
	
	lpNewImage->lpCmdList->dwImageID = lpImage->lpCmdList->dwImageID;

	// only need to save command list if this image was loaded low-res
	// or PhotoCD
	lpCmdPkt = (LPCMDPKT)ListGetHead(&lpImage->lpCmdList->UndoPacketList);
	if (lpCmdPkt && IsVariableResLoadCmd(lpCmdPkt))
	{
		CMD_PARMS parms;
	
		if (!CopyPacketList(&lpImage->lpCmdList->UndoPacketList,
							&lpNewImage->lpCmdList->UndoPacketList))
			AfxThrowMemoryException();
	
		if (!(lpCmdPkt = CreatePacket(IDS_CMD_CREATECLIPOBJECT, &parms, YES)))
			AfxThrowMemoryException();
		ListAddTail(&lpNewImage->lpCmdList->UndoPacketList, lpCmdPkt);
	}
	
	if (lpNewImage)
	    pDataItem = new CDataItem (pDoc, lpNewImage, &rMask, TRUE);
	
	ASSERT(pDataItem);
	if (!pDataItem)
		return DROPEFFECT_NONE;
	
	CRect rStartDrag;
	::ClientToScreen(hWindow,(LPPOINT)&rMask);        // must be in screen co-ordinates
	::ClientToScreen(hWindow,(LPPOINT)&rMask.right);  // must be in screen co-ordinates
	point.x = (rMask.right-rMask.left)/2;
	point.y = (rMask.bottom-rMask.top)/2;
	if (!bStartImmediate)
	{
		CWnd *pWnd = CWnd::FromHandle(hWindow);
	    if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))
			pWnd->GetParentFrame()->GetWindowRect(&rStartDrag);
		else
			::GetWindowRect(hWindow,&rStartDrag);
	}
	else
		rStartDrag.SetRect(point.x,point.y,point.x+1,point.y+1);

	// NOTE!!the pDataItem will be internally destroyed once the drag/drop 
	// operation is completed therefore we do not need to delete the pDataItem
	return pDataItem->DoDragDrop(&rMask, point, FALSE, dwEffects, 
		&rStartDrag);
}

/***********************************************************************/
BOOL SelectObjectsImage(LPIMAGE lpImage, LPSELECTOBJECTS_PARMS lpParms)
/***********************************************************************/
{
RECT SelectRect;
int res;
RECT rUpdate;
LPOBJECT lpObject;
int x, y;
BOOL fRet;

if (lpParms->fSelectAll)
	{
	GetObjectMarqueeRect(lpImage, &rUpdate);
	fRet = ImgSelObject(lpImage, NULL, NULL, NO, YES);
	}
else
	{
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	SelectRect = lpParms->SelectRect;
	// one point rect is not a rect
	if (SelectRect.left == SelectRect.right && SelectRect.top == SelectRect.bottom)
	{
		ResConvertUL(lpParms->iBaseRes, res, &SelectRect.left, &SelectRect.top);
		ResConvertUL(lpParms->iBaseRes, res, &SelectRect.right, &SelectRect.bottom);
	}
	else
		ResConvertRect(lpParms->iBaseRes, res, &SelectRect);
	
	AstralSetRectEmpty(&rUpdate);
	if (SelectRect.right > SelectRect.left) // rectangular selection
		{
		GetObjectMarqueeRect(lpImage, &rUpdate);
		fRet = ImgSelObject(lpImage, NULL, &SelectRect, lpParms->fExtendedSel, NO);
		}
	else
		{
		x = SelectRect.left;
		y = SelectRect.top;
		lpObject = ImgFindSelObject(lpImage, x, y, YES);
		GetObjectMarqueeRect(lpImage, &rUpdate);
		SetRect(&SelectRect, x, y, x, y);
		fRet = ImgSelObject(lpImage, lpObject, &SelectRect, lpParms->fExtendedSel,
					lpObject != NULL ? YES : NO);
		}
	}
if (fRet)
	{
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}
return(TRUE);
}

extern BOOL IsGroupLocked(WORD wGroupID);
/***********************************************************************/
BOOL MoveObjectsImage(LPIMAGE lpImage, LPMOVEOBJECTS_PARMS lpParms)
/***********************************************************************/
{
LPOBJECT lpBase, lpObject;
RECT rOrigObject, rUpdate;
int dx, dy, res, x, y;
BOOL  Locked;

res = FrameResolution(ImgGetBaseEditFrame(lpImage));
x = lpParms->x;
y = lpParms->y;
ResConvertUL(lpParms->iBaseRes, res, &x, &y);

ImgGetSelObjectRect(lpImage, &rOrigObject, YES);
dx = x - rOrigObject.left;
dy = y - rOrigObject.top;

if (!dx && !dy)
	return(TRUE);

AstralSetRectEmpty(&rUpdate);
lpObject = NULL;
lpBase = ImgGetBase(lpImage);

while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
   if (lpObject == lpBase)
      continue;
   Locked = (lpObject->wGroupID && IsGroupLocked(lpObject->wGroupID)) ||
             lpObject->bLocked;
   if (Locked)
      continue;
	rOrigObject = lpObject->rObject;
	OffsetRect(&lpObject->rObject, dx, dy);
	if (!lpObject->Pixmap.fNewFrame &&
			EqualRect(&rOrigObject, &lpObject->rUndoObject))
		lpObject->rUndoObject = lpObject->rObject;
	AstralUnionRect(&rUpdate, &rUpdate, &rOrigObject);
	AstralUnionRect(&rUpdate, &rUpdate, &lpObject->rObject);
	}

lpParms->Common.UpdateType = UT_AREA;
lpParms->Common.rUpdateArea = rUpdate;

return(TRUE);
}

/***********************************************************************/
BOOL SetAttrObjectsImage(LPIMAGE lpImage, LPSETATTROBJECTS_PARMS lpParms)
/***********************************************************************/
{
LPOBJECT lpObject;
RECT rUpdate;

AstralSetRectEmpty(&rUpdate);
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	switch (lpParms->AttributeType)
		{
		case AT_OPACITY:
			if (lpObject->Opacity != lpParms->Opacity)
				{
				lpObject->Opacity = lpParms->Opacity;
				AstralUnionRect(&rUpdate, &rUpdate, &lpObject->rObject);
				}
			break;
		case AT_MERGEMODE:
			if (lpObject->MergeMode != lpParms->MergeMode)
				{
				lpObject->MergeMode = lpParms->MergeMode;
				AstralUnionRect(&rUpdate, &rUpdate, &lpObject->rObject);
				}
			break;
		default:
			break;
		}
	}

if (!AstralIsRectEmpty(&rUpdate))
	{
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = rUpdate;
	}

return(TRUE);
}

/***********************************************************************/
BOOL LayerObjectsImage(LPIMAGE lpImage, LPLAYEROBJECTS_PARMS lpParms)
/***********************************************************************/
{
RECT rUpdate;

ImgAdjustLayerSelObj(lpImage, lpParms->fToTop, lpParms->fOneLevel);
ImgGetSelObjectRect(lpImage, &rUpdate, YES/*bExcludeBase*/);
lpParms->Common.UpdateType = UT_AREA;
lpParms->Common.rUpdateArea = rUpdate;
return(TRUE);
}

/***********************************************************************/
BOOL DuplicateObjectsImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/***********************************************************************/
{
if (ImgDuplicateSelObjects(lpImage))
	{
	ImgGetSelObjectRect(lpImage, &lpParms->Common.rUpdateArea, YES);
	lpParms->Common.UpdateType = UT_AREA;
	}
return(TRUE);
}

/***********************************************************************/
BOOL DeleteObjectsImage(LPIMAGE lpImage, LPCMD_PARMS lpParms)
/***********************************************************************/
{
	RECT rSelect;
	LPOBJECT lpObject;

    if (ImgGetSelObject(lpImage, NULL) == ImgGetBase(lpImage))
        return(TRUE); // not really an error
    if (!ImgGetSelObjectRect(lpImage, &rSelect, YES/*bExcludeBase*/))
        return(TRUE); // not really an error
    if (!ImgEditInit(lpImage, ET_OBJECT, UT_DELETEOBJECTS, ImgGetBase(lpImage)))
        return(FALSE);
    lpObject = NULL;
    while (lpObject = ImgGetSelObject(lpImage, lpObject))
    {
        lpObject->fDeleted = YES;
    }
    ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDODELETE, NULL);

	lpParms->Common.rUpdateArea = rSelect;
	lpParms->Common.UpdateType = UT_AREA;

	return(TRUE);
}

// LOCAL prototypes
LOCAL BOOL Selector_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Selector_OnDocActivated(HWND hDlg);
LOCAL void Selector_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Selector_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Selector_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgSelectorProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Selector_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Selector_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Selector_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Selector_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_SELECTOR);
	}
}

/************************************************************************/
LOCAL BOOL Selector_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
Selector_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Selector_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Selector_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Selector_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Selector_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL BOOL  ImgIfSelObjectsLocked(LPIMAGE lpImage)
/************************************************************************/
{
LPOBJECT lpObject;
BOOL        AllLocked = TRUE;

if (lpObject = ImgGetSelObject(lpImage, NULL))
   {
   if (!lpObject->bLocked)
      AllLocked = FALSE;
   while (lpObject = ImgGetSelObject(lpImage, lpObject))
      if (!lpObject->bLocked)
         AllLocked = FALSE;
   }
else
   AllLocked = FALSE;
return(AllLocked);
}

static BOOL RibbonGenerated = FALSE;
/************************************************************************/
LOCAL void Selector_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
LPOBJECT lpObject;
int		Opacity, iNumSel;
BOOL	Bool, bEnabled;
MERGE_MODE MergeMode;
LPIMAGE lpImage;
FRMTYPEINFO TypeInfo;
BOOL        AllLocked;
BOOL        NotLocked = TRUE;

lpImage = GetActiveImage();
ImgGetTypeInfo(lpImage, &TypeInfo);
CheckDlgButton(hDlg, IDC_OBJECTGROUP, ImgIfSelObjectsGrouped(lpImage));
AllLocked = ImgIfSelObjectsLocked(lpImage);

if (lpObject = ImgGetSelObject(lpImage, NULL))
	{
	MergeMode = lpObject->MergeMode;
	Opacity = lpObject->Opacity;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
		{
		if (lpObject->Opacity != Opacity)
			Opacity = -1;
		if (lpObject->MergeMode != MergeMode)
			MergeMode = (MERGE_MODE)-1;
		}
	if (Opacity < 0)
		Opacity = 255;
	}
else
	{
	Opacity = 255;
	MergeMode = (MERGE_MODE)-1;
	}

CheckDlgButton(hDlg, IDC_OBJECTLOCK, AllLocked);
HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, MergeMode, 0L);
if (!RibbonGenerated)
   InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Opacity), NO, 0, 99);
InitSlide( hDlg, IDC_TRANSPARENT_SCROLL, TOPERCENT(255-Opacity),
	0, 99 );
Bool = ObjectToolsEnabled(lpImage);
iNumSel = ImgCountSelObjects(lpImage, NULL);
ControlEnable(hDlg, IDC_OBJECTGROUP, Bool && iNumSel > 1);
if (!iNumSel || ImgGetSelObject(lpImage, NULL) == ImgGetBase(lpImage))
	Bool = FALSE;

lpObject = ImgGetBase(lpImage);
while (lpObject = ImgGetSelObject(lpImage, lpObject))
   {
   if ((lpObject->wGroupID && IsGroupLocked(lpObject->wGroupID)) ||
       lpObject->bLocked)
      {
      NotLocked = FALSE;
      break;
      }
   }

ControlEnable(hDlg, IDC_OBJECTTRANSFORM, NotLocked && Bool);
ControlEnable(hDlg, IDC_MERGEMODE, Bool && CANDOMERGEMODE(TypeInfo.DataType));
ControlEnable(hDlg, IDC_TRANSPARENT, Bool && CANDOTRANSPARENCY(TypeInfo.DataType));
ControlEnable(hDlg, IDC_TRANSPARENT_SCROLL, Bool && CANDOTRANSPARENCY(TypeInfo.DataType));
ControlEnable(hDlg, IDC_OBJECTLOCK, Bool);

bEnabled = ImgCountObjects(lpImage) > 2;
ControlEnable(hDlg, IDC_TOTOP, Bool && bEnabled);
ControlEnable(hDlg, IDC_TOBOTTOM, Bool && bEnabled);
}

/************************************************************************/
LOCAL void Selector_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
    int		    Opacity, i;
    BOOL	    Bool;
    MERGE_MODE  MergeMode;
    LPIMAGE     lpImage;
    CMD_PARMS   parms;

    if( !Control.Retail )
        return;

    lpImage = GetActiveImage();

    switch (id)
    {
        case IDC_SELECTOR:
	        if ( !SaveToolPreferences( id ))
		        break;

	    case IDC_TOTOP:
	    case IDC_TOBOTTOM:
		    if (!lpImage)
			    break;
		    SetObjectLayering(lpImage, id == IDC_TOTOP, !SHIFT);
		    break;

        case IDC_OBJECTLOCK:
		    if (!lpImage)
			    break;
            if (ImgIfSelObjectsLocked(lpImage))
                ProcessCommand(lpImage->lpCmdList, IDS_CMD_OBJUNLOCK, &parms);
            else
                ProcessCommand(lpImage->lpCmdList, IDS_CMD_OBJLOCK, &parms);
            break;

	    case IDC_OBJECTGROUP:
		    if (!lpImage)
			    break;

            if (!ImgIfSelObjectsGrouped(lpImage))
                ProcessCommand(lpImage->lpCmdList, IDS_CMD_GROUPOBJECTS, &parms);
            else
                ProcessCommand(lpImage->lpCmdList, IDS_CMD_UNGROUPOBJECTS, &parms);
		    CheckDlgButton(hDlg, IDC_OBJECTGROUP, ImgIfSelObjectsGrouped(lpImage));
		    break;

	    case IDC_OBJECTTRANSFORM:
		    if (!lpImage)
			    break;
    	    RemoveObjectMarquee(lpImage);
	        TransformObjectsStart(lpImage, TransformObjectsEndProc, IDC_SELECTORTRANSFORM, NO, 
		    			    IDS_UNDOTRANSFORMER, YES);
		    break;

	    case IDC_MERGEMODE:
		    if (!lpImage)
			    break;
		    if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
			    break;
		    SetObjectMergeMode(lpImage, MergeMode);
		    break;

	    case IDC_TRANSPARENT:
		    if (!lpImage || RibbonGenerated)
			    break;
		    if ( GetFocus() != GetDlgItem( hDlg, id ) )
			    break;
		    if ( codeNotify != EN_CHANGE )
			    break;
		    i = GetDlgItemSpin( hDlg, id, &Bool, NO );
		    SetSlide( hDlg, IDC_TRANSPARENT_SCROLL, i );
		    Opacity = 255-TOGRAY(i);
            RibbonGenerated = TRUE;
		    SetObjectOpacity(lpImage, Opacity);
            RibbonGenerated = FALSE;
		    break;

	    case IDC_TRANSPARENT_SCROLL:
		    if (!lpImage)
			    break;
		    if ( GetFocus() != GetDlgItem( hDlg, id ) )
			    break;
		    i = HandleSlide( hDlg, id, codeNotify, NULL );
		    SetSlide( hDlg, id, i );
		    SetDlgItemIntNow( hDlg, IDC_TRANSPARENT, i, FALSE );
		    Opacity = 255-TOGRAY(i);
		    SetObjectOpacity(lpImage, Opacity);
		    break;

        default:
	        break;
    }
}

typedef enum
    {
	SM_START,
	SM_MOVE,
	SM_SELECT
	} SELECT_MODE;

#define TIMER_ID 100
#define BOX_DELAY 400
#define OUTLINE_DELAY 600

class CSelectorHook : public CHookHandler
{
public:
    CSelectorHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CSelectorHook SelectorHook;


static BOOL fDrawOutline;
static SELECT_MODE Mode;
static RECT SelectRect;
static int xOffset, yOffset;
static HPEN hPen;
static POINT LastPoint;
static WORD		idTimer;	// timer indentifier
static BOOL fShift;

/************************************************************************/
void SelectorProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	 	SelectorProc_OnCreate(hWindow, x, y, Option);
		break;

	case WM_SETCURSOR:
	 	SelectorProc_OnSetCursor(hWindow, x, y, Option);
		break;
	
	case WM_LBUTTONDOWN:
	 	SelectorProc_OnLButtonDown(hWindow, x, y, Option);
		break;

    case WM_LBUTTONUP:
	 	SelectorProc_OnLButtonUp(hWindow, x, y, Option);
		break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	 	SelectorProc_OnMouseMove(hWindow, x, y, Option);
		break;

	case WM_KEYDOWN:
	 	SelectorProc_OnKeyDown(hWindow, x, y, Option);
		break;

	case WM_TIMER:
	 	SelectorProc_OnTimer(hWindow, x, y, Option);
		break;

	case WM_DESTROY:	// The cancel operation message
	 	SelectorProc_OnDestroy(hWindow, x, y, Option);
		break;
    }
}

/************************************************************************/
LOCAL void SelectorProc_OnCreate( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{
LPIMAGE lpImage;

lpImage = GetImagePtr(hWindow);
if (!IsOnImage(hWindow, x, y))
	return;

Tool.bActive = YES;
hPen = CreatePen(PS_DOT, 1, RGB(0,0,0));
}

/************************************************************************/
LOCAL void SelectorProc_OnSetCursor( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{
LPIMAGE lpImage;
LPOBJECT lpObject;
POINT    pos;
BOOL     Locked = FALSE;

lpImage = GetImagePtr(hWindow);
GetCursorPos(&pos);
ScreenToClient(hWindow, &pos);
Display2File(hWindow,(LPINT)&pos.x, (LPINT)&pos.y);
lpObject = ImgFindSelObject(lpImage, pos.x, pos.y, NO);

if (lpObject)
   Locked = (lpObject->wGroupID && IsGroupLocked(lpObject->wGroupID)) ||
             lpObject->bLocked;
if (lpObject && 
	 lpObject != ImgGetBase(lpImage) && !Locked)
	SetWindowCursor(ID_SELECTORALT);
else
	SetWindowCursor(Tool.Prim.idCursor);
}

/************************************************************************/
LOCAL void SelectorProc_OnLButtonDown( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{
	LPIMAGE lpImage;
	int fx, fy;

	lpImage = GetImagePtr(hWindow);
	fShift = SHIFT;
	Mode = SM_START;
	fDrawOutline = NO;
	LastPoint.x = x;
	LastPoint.y = y;
	fx = LastPoint.x;
	fy = LastPoint.y;
	Display2File(hWindow,&fx, &fy);

	// if we landed in an object that is not the base
	// set a timer, to change mode to select and move
	// instead of dragging a rectangle
	if (ImgFindObject(lpImage, fx, fy, NO, YES))
		SetTimer( hWindow, idTimer = TIMER_ID, BOX_DELAY, NULL );
	else
	if (ALT)
	{
		DeactivateTool();
		DoDragDrop(hWindow,x,y,DROPEFFECT_COPY,TRUE);
	 	SelectorProc_OnDestroy(hWindow, x, y, Option);
		FORWARD_WM_LBUTTONUP(hWindow, x, y, Option, SendMessage);
		return;
	}
}

/************************************************************************/
LOCAL void SelectorProc_OnLButtonUp( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{
LPIMAGE lpImage;
LPOBJECT lpObject;
RECT rOrigObject;
int dx, dy, fx, fy;

lpImage = GetImagePtr(hWindow);
Tool.bActive = NO;
AstralCursor(IDC_WAIT);
if (idTimer)
	{
	KillTimer( hWindow, idTimer );
	idTimer = 0;
	}
if (Mode == SM_MOVE)
	{
	DisplayInfo(x, y, NULL);
	SelectorHook.DisplayHookProc(hWindow, 0, NULL);
	SetDisplayHook(hWindow, NULL);
	ImgGetSelObjectRect(lpImage, &rOrigObject, YES);
	dx = SelectRect.left - rOrigObject.left;
	dy = SelectRect.top - rOrigObject.top;
	if (dx || dy)
		MoveObjects(lpImage, rOrigObject.left, rOrigObject.top, SelectRect.left, SelectRect.top);
	}
else 
	{
	BOOL fRet;

	if (Mode == SM_SELECT)
		{
		EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
		if (abs(SelectRect.right-SelectRect.left) <= SMALL_MOVEMENT &&
			abs(SelectRect.bottom-SelectRect.top) <= SMALL_MOVEMENT)
			{
			SelectRect.right = SelectRect.left;
			SelectRect.bottom = SelectRect.top;
			}
		}
	else
		{
		fx = LastPoint.x;
		fy = LastPoint.y;
		Display2File(hWindow,&fx, &fy);
		SetRect(&SelectRect, fx, fy, fx, fy);
		}
	RemoveObjectMarquee(lpImage);
	if (SelectRect.right > SelectRect.left)
		fRet = ImgSelObject(lpImage, NULL, &SelectRect, fShift, NO);
	else
		{
		fx = SelectRect.left;
		fy = SelectRect.top;
		lpObject = ImgFindSelObject(lpImage, fx, fy, YES);
		fRet = ImgSelObject(lpImage, lpObject, &SelectRect, fShift,
					lpObject != NULL ? YES : NO);

		}
	if (fRet)
		{
		SelectObjects(lpImage, &SelectRect, fShift);
		}
	}
if ( Tool.hRibbon )
	SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
AstralCursor(NULL);
if (hPen)
	DeleteObject(hPen);
}

/************************************************************************/
LOCAL void SelectorProc_OnMouseMove( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{
POINT pt;
int dx, dy, fx, fy;
LPIMAGE lpImage;
LPOBJECT lpObject;
RECT rDisplay, rBase;
BOOL     Locked = FALSE;

lpImage = GetImagePtr(hWindow);
if (Mode == SM_START)
	{
	if (abs(LastPoint.x-x) <= SMALL_MOVEMENT &&
		abs(LastPoint.y-y) <= SMALL_MOVEMENT)
		return;
	if (idTimer)
		{
		KillTimer( hWindow, idTimer );
		idTimer = 0;
		}
	AstralUpdateWindow(hWindow);
	fx = LastPoint.x;
	fy = LastPoint.y;
	Display2File(hWindow,&fx, &fy);
	lpObject = ImgFindSelObject(lpImage, fx, fy, YES);
   if (lpObject)
      Locked = (lpObject->wGroupID && IsGroupLocked(lpObject->wGroupID)) ||
                lpObject->bLocked;
	if (lpObject && lpObject != ImgGetBase(lpImage) && !Locked)
		Mode = SM_MOVE;
	else
		Mode = SM_SELECT;
	if (Mode == SM_MOVE)
		{
		if (fShift)
			{
			if (!DuplicateObjects(lpImage))
				{
				if (hPen)
					DeleteObject(hPen);
				Tool.bActive = NO;
				return;
				}
			}
		AstralUpdateWindow(hWindow);
		ImgGetSelObjectRect(lpImage, &SelectRect, YES);
		rDisplay = SelectRect;
		File2DispRectExact(hWindow, &rDisplay, &rDisplay);
		DrawRect(0, &rDisplay, hPen);
		Display2File(hWindow,(LPINT)&LastPoint.x, (LPINT)&LastPoint.y);
		SetDisplayHook(hWindow, &SelectorHook);
		}
	else
		{
		AstralSetRectEmpty( &SelectRect );
		StartSelection( hWindow, NULL, &SelectRect,
			SL_BOX | SL_SPECIAL, LastPoint, 0L );
		}
	}
if (idTimer)
	{
	KillTimer( hWindow, idTimer );
	idTimer = 0;
	}
if (Mode == SM_MOVE)
	{
	DisplayInfo(x, y, &SelectRect);
	Display2File(hWindow,&x, &y);
	dx = x - LastPoint.x;
	dy = y - LastPoint.y;
	if (!dx && !dy)
		return;
	LastPoint.x = x;
	LastPoint.y = y;
	SelectorHook.DisplayHookProc(hWindow, 0, NULL);
	OffsetRect(&SelectRect, dx, dy);
	xOffset += dx;
	yOffset += dy;
	rBase = ImgGetBase(lpImage)->rObject;

	dx = dy = 0;
	if (SelectRect.left > rBase.right)
		dx = rBase.right - SelectRect.left;
	if (SelectRect.right < rBase.left)
		dx = rBase.left - SelectRect.right;
	if (SelectRect.top > rBase.bottom)
		dy = rBase.bottom - SelectRect.top;
	if (SelectRect.bottom < rBase.top)
		dy = rBase.top - SelectRect.bottom;

	OffsetRect(&SelectRect, dx, dy);
	xOffset += dx;
	yOffset += dy;
	SelectorHook.DisplayHookProc(hWindow, 0, NULL);

	CRect rWindow;
	::GetWindowRect(hWindow,&rWindow);
	::ScreenToClient(hWindow,(LPPOINT)&rWindow.left);
	::ScreenToClient(hWindow,(LPPOINT)&rWindow.right);
	Display2File(hWindow,(LPINT)&rWindow.left,(LPINT)&rWindow.top);
	Display2File(hWindow,(LPINT)&rWindow.right,(LPINT)&rWindow.bottom);
#define  DRAGZONE  10

   BOOL DoDrag = !fShift && ((x > (rWindow.right + DRAGZONE)) || 
                             (x < (rWindow.left  - DRAGZONE)) ||
                             (y > (rWindow.bottom+ DRAGZONE)) ||
                             (y < (rWindow.top   - DRAGZONE)));

	if (DoDrag)
		{
		Tool.bActive = NO;
		DisplayInfo(x, y, NULL);
		SelectorHook.DisplayHookProc(hWindow, 0, NULL);
		SetDisplayHook(hWindow, NULL);
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		AstralCursor(NULL);
		if (hPen)
			DeleteObject(hPen);
		DeactivateTool();

		DoDragDrop(hWindow,x,y,DROPEFFECT_COPY, FALSE, TRUE);
	 	SelectorProc_OnDestroy(hWindow, x, y, Option);
		FORWARD_WM_LBUTTONUP(hWindow, x, y, Option, SendMessage);
		}
	}
else if (Mode == SM_SELECT)
	{
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
		pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
	}
}

/************************************************************************/
LOCAL void SelectorProc_OnKeyDown( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{
WORD wKey;

if (Tool.bActive)
	return;
wKey = (WORD)Option;
switch (wKey)
	{
	case VK_DELETE:
		SendMessage(PictPubApp.Get_hWndAstral(), WM_COMMAND, IDM_OBJ_DELETE, 0L);
		break;
	default:
		break;
	}
}

/************************************************************************/
LOCAL void SelectorProc_OnTimer( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{		
int fx, fy;
RECT rDisplay, rUpdate;
LPIMAGE lpImage;
LPOBJECT lpObject;

lpImage = GetImagePtr(hWindow);

if (idTimer)
	{
	KillTimer( hWindow, idTimer );
	idTimer = 0;
	}
if (Mode != SM_START)
	{
	if (lpObject = ImgGetSelObject(lpImage, NULL))
		{
		xOffset = yOffset = 0;
		ImgGetSelObjectRect(lpImage, &rUpdate, YES);
		OffsetRect(&rUpdate, xOffset, yOffset);
		InflateRect(&rUpdate, 1, 1);
		File2DispRect(hWindow, &rUpdate, &rUpdate);
		PaintMarquee(hWindow, Window.hDC, lpObject, NULL, YES, &rUpdate,
						0, 0, xOffset, yOffset);
		fDrawOutline = YES;
		}
	return;
	}
fx = LastPoint.x;
fy = LastPoint.y;
Display2File(hWindow,&fx, &fy);
// did the user click in an already selected object
// that's not the base
lpObject = ImgFindSelObject(lpImage, fx, fy, YES);
if (lpObject && lpObject != ImgGetBase(lpImage))
	{
	if (fShift)
		{
		if (!DuplicateObjects(lpImage))
			{
			if (hPen)
				DeleteObject(hPen);
			Tool.bActive = NO;
			return;
			}
		}
	}
else	// user clicked in a unselected object
	{
	BOOL fRet;

	RemoveObjectMarquee(lpImage);
	SetRect(&SelectRect, fx, fy, fx, fy);
	fRet = ImgSelObject(lpImage, NULL, &SelectRect, fShift, NO);
	if (fRet)
		SelectObjects(lpImage, &SelectRect, fShift);
	}
if (ImgGetSelObject(lpImage, NULL) != ImgGetBase(lpImage))
	{
	AstralUpdateWindow(hWindow);
	ImgGetSelObjectRect(lpImage, &SelectRect, YES);
	rDisplay = SelectRect;
	File2DispRectExact(hWindow, &rDisplay, &rDisplay);
	DrawRect(0, &rDisplay, hPen);
	Display2File(hWindow,(LPINT)&LastPoint.x, (LPINT)&LastPoint.y);
	Mode = SM_MOVE;
	SetDisplayHook(hWindow, &SelectorHook);
	SetTimer( hWindow, idTimer = TIMER_ID, OUTLINE_DELAY, NULL );
	}
}

/************************************************************************/
LOCAL void SelectorProc_OnDestroy( HWND hWindow, int x, int y, UINT32 Option )
/************************************************************************/
{		
LPIMAGE     lpImage;
RECT        rDisplay;
LPOBJECT    lpObject;

lpImage = GetImagePtr(hWindow);
if ( !Tool.bActive )
	return;
if (idTimer)
	{
	KillTimer( hWindow, idTimer );
	idTimer = 0;
	}
if (Mode == SM_MOVE)
	{
	if (fShift)
      {
      lpObject = ImgGetBase(lpImage);
      while (lpObject = ImgGetSelObject(lpImage, lpObject))
         lpObject->fDeleted = TRUE;
	   SetDisplayHook(hWindow, NULL);
      }
	rDisplay = SelectRect;
	File2DispRectExact(hWindow, &rDisplay, &rDisplay);
	DrawRect(0, &rDisplay, hPen);
   SetupMiniViews(NULL, YES);
	}
else if (Mode == SM_SELECT)
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
if (hPen)
	DeleteObject(hPen);
Tool.bActive = NO;
}

/************************************************************************/
void CSelectorHook::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect)
/************************************************************************/
{
RECT rUpdate, rDisplay;
LPOBJECT lpObject;

if (!hDC)
	hDC = Window.hDC;

if (Mode != SM_MOVE)
	return;

File2DispRectExact(hWnd, &SelectRect, &rDisplay);
DrawRect(hDC, &rDisplay, hPen);
if (fDrawOutline)
	{
	LPIMAGE lpImage;

	lpImage = GetImagePtr(hWnd);
	lpObject = ImgGetSelObject(lpImage, NULL);
	ImgGetSelObjectRect(lpImage, &rUpdate, YES);
	OffsetRect(&rUpdate, xOffset, yOffset);
	InflateRect(&rUpdate, 1, 1);
	File2DispRect(hWnd, &rUpdate, &rUpdate);
	PaintMarquee(hWnd, hDC, lpObject, NULL, YES, &rUpdate,
					0, 0, xOffset, yOffset);
	}
}

/************************************************************************/
MERGE_MODE HandleMergeCombo(HWND hDlg, ITEMID idCombo, BOOL fInit,
					MERGE_MODE ModeSelected, UINT codeNotify)
/************************************************************************/
{
return(HandleMergeComboEx(hDlg, idCombo, fInit, ModeSelected, codeNotify, NO));
}

/************************************************************************/
MERGE_MODE HandleMergeComboEx(HWND hDlg, ITEMID idCombo, BOOL fInit,
					MERGE_MODE ModeSelected, UINT codeNotify,
					BOOL fAllModes)
/************************************************************************/
{
int LATable[] = {	IDS_MODENORMAL, IDS_MODEINVERT};
int GrayTable[] = {	IDS_MODENORMAL, IDS_MODEADDITIVE, IDS_MODESUBTRACTIVE,
					IDS_MODEIFLIGHTER, IDS_MODEIFDARKER, IDS_MODESCREEN,
					IDS_MODEMULTIPLY, IDS_MODEDIFFERENCE, IDS_MODETEXTURIZE,
					IDS_MODEINVERT};
int RGBTable[] = {	IDS_MODENORMAL, IDS_MODEADDITIVE, IDS_MODESUBTRACTIVE,
					IDS_MODEIFLIGHTER, IDS_MODEIFDARKER, IDS_MODESCREEN,
					IDS_MODEMULTIPLY, IDS_MODEDIFFERENCE, IDS_MODETEXTURIZE,
					IDS_MODEHUESAT, IDS_MODEHUE, IDS_MODESAT,
					IDS_MODELUM, IDS_MODERED, IDS_MODEGREEN,
					IDS_MODEBLUE, IDS_MODEINVERT};
int CMYKTable[] = {	IDS_MODENORMAL, IDS_MODEADDITIVE, IDS_MODESUBTRACTIVE,
					IDS_MODEIFLIGHTER, IDS_MODEIFDARKER, IDS_MODESCREEN,
					IDS_MODEMULTIPLY, IDS_MODEDIFFERENCE, IDS_MODETEXTURIZE,
					IDS_MODEHUESAT, IDS_MODEHUE, IDS_MODESAT,
					IDS_MODELUM, IDS_MODECYAN, IDS_MODEMAGENTA,
					IDS_MODEYELLOW, IDS_MODEBLACK, IDS_MODEINVERT};
int AllTable[] = {	IDS_MODENORMAL, IDS_MODEADDITIVE, IDS_MODESUBTRACTIVE,
					IDS_MODEIFLIGHTER, IDS_MODEIFDARKER, IDS_MODESCREEN,
					IDS_MODEMULTIPLY, IDS_MODEDIFFERENCE, IDS_MODETEXTURIZE,
					IDS_MODEHUESAT, IDS_MODEHUE, IDS_MODESAT,
					IDS_MODELUM, IDS_MODERED, IDS_MODEGREEN,
					IDS_MODEBLUE,
					IDS_MODECYAN, IDS_MODEMAGENTA,
					IDS_MODEYELLOW, IDS_MODEBLACK, IDS_MODEINVERT};
int nItems, idItem;
LPINT lpItemID;
FRMDATATYPE Type;

if (!GetDlgItem(hDlg, idCombo))
	return((MERGE_MODE)-1);

if (fAllModes)
	{
	nItems = sizeof(AllTable)/sizeof(int);
	lpItemID = AllTable;
	}
else
	{
	if (GetActiveImage())
		Type = FrameType(ImgGetBaseEditFrame(GetActiveImage()));
	else
		Type = FDT_RGBCOLOR;
	switch (Type)
		{
		case FDT_GRAYSCALE:
			nItems = sizeof(GrayTable)/sizeof(int);
			lpItemID = GrayTable;
			break;
		case FDT_RGBCOLOR:
			nItems = sizeof(RGBTable)/sizeof(int);
			lpItemID = RGBTable;
			break;
		case FDT_CMYKCOLOR:
			nItems = sizeof(CMYKTable)/sizeof(int);
			lpItemID = CMYKTable;
			break;
		case FDT_PALETTECOLOR:
		case FDT_LINEART:
			nItems = sizeof(LATable)/sizeof(int);
			lpItemID = LATable;
			break;
		default:
			nItems = 0;
			lpItemID = NULL;
			break;
		}
	}
if (fInit)
	{
	TableCheckComboItem(hDlg, idCombo, lpItemID, nItems,
					ModeSelected + IDS_MODEFIRST);
	ControlEnable(hDlg, idCombo, nItems > 1);
	return(MM_NORMAL);
	}
else
	{
	idItem = TableHandleCombo(hDlg, lpItemID, nItems, idCombo, codeNotify);
	if (!idItem)
		return((MERGE_MODE)-1);
	return((MERGE_MODE)(idItem - IDS_MODEFIRST));
	}
}

/***********************************************************************/
static BOOL ObjectToolsEnabled(LPIMAGE lpImage)
/***********************************************************************/
{
if (!lpImage)
	return(FALSE);
if (!ImgMultipleObjects(lpImage))
	return(FALSE);
if (ImgGetMask(lpImage) && !Control.UseMaskAndObjects)
	return(FALSE);
return(TRUE);
}


// Static Prototypes
static int CalculateMaxMinis( HWND hDlg );
int nTopObject;
int nMaxMinis = -1;

/************************************************************************/
static int CalculateMaxMinis( HWND hDlg )
/************************************************************************/
{
HWND hWnd;
int y, iHeightAllMinis, iHeightMini;
RECT rRect;

// Get the bottom of the dialog box
GetWindowRect( hDlg, &rRect );
y = rRect.bottom;

// Get the bottom of the last icon, and use the smaller of the two
if ( !(hWnd = GetDlgItem( hDlg, IDC_VIEWOBJLAST )) )
	return( 0 );
GetWindowRect( hWnd, &rRect );
y = min( y, rRect.bottom );

// Get the top of the first icon, and compute the height
if ( !(hWnd = GetDlgItem( hDlg, IDC_VIEWOBJFIRST )) )
	return( 0 );
GetWindowRect( hWnd, &rRect );
iHeightAllMinis = y - rRect.top;
y = rRect.top;

// Get the top of the second icon to compute the spacing for each icon
if ( !(hWnd = GetDlgItem( hDlg, IDC_VIEWOBJFIRST+1 )) )
	return( 0 );
GetWindowRect( hWnd, &rRect );
iHeightMini = rRect.top - y;

// Compute the number of icons that will fit
if ( !iHeightMini )
	return( 0 );
return( iHeightAllMinis / iHeightMini );
}


/************************************************************************/
void SetupMiniViews(HWND hDlg, BOOL fSelectChange)
/************************************************************************/
{
HWND hControl;
LPOBJECT lpObject;
ITEMID id, idActiveObject;
int i, nObjects, iObject;
BOOL bShow, bOffScreen;
LPIMAGE lpImage;

if (!hDlg)
	if ( !(hDlg = ObjFloat.GetSafeHwnd()) )
		return;

lpImage = GetActiveImage();
if ( nMaxMinis < 0 )
	nMaxMinis = CalculateMaxMinis( hDlg );

nObjects = ImgCountFloaters(lpImage);

// Compute the last image to scroll to
if ( (i = nObjects - nMaxMinis) < 0 )
	i = 0;

// Correct nTopObject since images may have closed
if ( nTopObject > i )
	nTopObject = i;

// Show or hide the scroll buttons
bShow = ( nMaxMinis && (nObjects > nMaxMinis) );
bShow = YES; // always show them
ShowDlgItem( hDlg, IDC_VIEWOBJUP, bShow );
ShowDlgItem( hDlg, IDC_VIEWOBJDOWN, bShow );

// Enable or disable scroll buttons
if (bShow)
	{
	ControlEnable(hDlg, IDC_VIEWOBJUP, nTopObject > 0);
	ControlEnable(hDlg, IDC_VIEWOBJDOWN, (nObjects-nTopObject) > nMaxMinis);
	}
idActiveObject = 0;
for ( id = IDC_VIEWOBJFIRST; id <= IDC_VIEWOBJLAST; id++ )
	{
	if ( !(hControl = GetDlgItem( hDlg, id )) )
		continue;
	iObject = id - IDC_VIEWOBJFIRST;
	bOffScreen = ( iObject >= nMaxMinis );
	iObject += nTopObject;
	if ( !bOffScreen && (lpObject = ImgGetObject(lpImage, iObject)) )
		{
		CheckDlgButton(hDlg, id, lpObject->fSelected);
		SetWindowLong( hControl, GWL_IMAGE, (long)(1 + iObject) );
		ShowDlgItem( hDlg, id, YES );
		if ( lpObject && (lpObject->fSelected) )
			idActiveObject = id;
		}
	else	{
		SetWindowLong( hControl, GWL_IMAGE, 0L );
		ShowDlgItem( hDlg, id, NO );
		}
	if (!fSelectChange)
		AstralControlRepaint( hDlg, id );
	}

//CheckRadioButton( hDlg, IDC_VIEWOBJFIRST, IDC_VIEWOBJLAST, idActiveObject );
}

/**************************************************************************/
int ImgCountFloaters(LPIMAGE lpImage)
/**************************************************************************/
{
int nFloaters;

nFloaters = ImgCountObjects(lpImage)-1;
if (nFloaters < 0)
	nFloaters = 0;
return(nFloaters);
}

/**************************************************************************/
LPOBJECT ImgGetObject(LPIMAGE lpImage, int nObject)
/**************************************************************************/
{
int i = 0;
LPOBJECT lpObject = NULL;

while (lpObject = ImgGetNextObject(lpImage, lpObject, NO, NO))
	{
	if (i == nObject)
		break;
	++i;
	}
if (lpObject == ImgGetBase(lpImage))
	lpObject = NULL;
return(lpObject);
}

/**************************************************************************/
LOCAL BOOL SetObjectOpacity(LPIMAGE lpImage, BYTE Opacity)
/**************************************************************************/
{
BOOL fChanged;
LPOBJECT lpObject;

fChanged = NO;
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (Opacity != lpObject->Opacity)
		{
		fChanged = YES;
		lpObject->Opacity = Opacity;
		UpdateImage(lpImage, &lpObject->rObject, YES);
		}
	}
if (fChanged)
	{
	SETATTROBJECTS_PARMS parms;

	parms.Opacity = Opacity;
	PostCommand(lpImage->lpCmdList, IDS_CMD_SETOPACITY, &parms);
	}
return(fChanged);
}

/**************************************************************************/
LOCAL BOOL SetObjectMergeMode(LPIMAGE lpImage, MERGE_MODE MergeMode)
/**************************************************************************/
{
BOOL fChanged;
LPOBJECT lpObject;

fChanged = NO;
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
	if (MergeMode != lpObject->MergeMode)
		{
		fChanged = YES;
		lpObject->MergeMode = MergeMode;
		UpdateImage(lpImage, &lpObject->rObject, YES);
		}
	}
if (fChanged)
	{
	SETATTROBJECTS_PARMS parms;

	parms.MergeMode = MergeMode;
	PostCommand(lpImage->lpCmdList, IDS_CMD_SETMERGEMODE, &parms);
	}
return(fChanged);
}

/**************************************************************************/
LOCAL BOOL SetObjectLayering(LPIMAGE lpImage, BOOL fToTop, BOOL fOneLevel)
/**************************************************************************/
{
RECT rDisplay;
LAYEROBJECTS_PARMS parms;
ITEMID idCmd;

ImgAdjustLayerSelObj(lpImage, fToTop, fOneLevel);
ImgGetSelObjectRect(lpImage, &rDisplay, YES/*bExcludeBase*/);
UpdateImage(lpImage, &rDisplay, YES);

if (fOneLevel)
	{
	if (fToTop)
		idCmd = IDS_CMD_ORDERUP;
	else
		idCmd = IDS_CMD_ORDERDOWN;
	}
else
	{
	if (fToTop)
		idCmd = IDS_CMD_ORDERTOTOP;
	else
		idCmd = IDS_CMD_ORDERTOBOTTOM;
	}
PostCommand(lpImage->lpCmdList, idCmd, &parms);
SetupMiniViews(NULL, NO);
return(TRUE);
}

/**************************************************************************/
LOCAL BOOL MoveObjects(LPIMAGE lpImage, int xorg, int yorg, int xnew, int ynew)
/**************************************************************************/
{
LPOBJECT lpObject;
RECT rOrigObject;
BOOL  Locked;
int dx, dy;

dx = xnew-xorg;
dy = ynew-yorg;

lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
   Locked = (lpObject->wGroupID && IsGroupLocked(lpObject->wGroupID)) ||
             lpObject->bLocked;
   if (Locked)
      continue;
	rOrigObject = lpObject->rObject;
	OffsetRect(&lpObject->rObject, dx, dy);
	if (!lpObject->Pixmap.fNewFrame &&
			EqualRect(&rOrigObject, &lpObject->rUndoObject))
		lpObject->rUndoObject = lpObject->rObject;
	UpdateImage(lpImage, &rOrigObject, TRUE);
	UpdateImage(lpImage, &lpObject->rObject, TRUE);
	}
if (dx || dy)
	{
	MOVEOBJECTS_PARMS parms;
	int res;

	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.iBaseRes = res;
	parms.x = xnew;
	parms.y = ynew;
	PostCommand(lpImage->lpCmdList, IDS_CMD_MOVEOBJECTS, &parms);
	}
return(TRUE);
}

/**************************************************************************/
LOCAL BOOL SelectObjects(LPIMAGE lpImage, LPRECT lpRect, BOOL fExtendedSel)
/**************************************************************************/
{
SELECTOBJECTS_PARMS parms;
int res;

res = FrameResolution(ImgGetBaseEditFrame(lpImage));

parms.iBaseRes = res;
parms.SelectRect.left = lpRect->left;
parms.SelectRect.top = lpRect->top;
parms.SelectRect.right = lpRect->right;
parms.SelectRect.bottom = lpRect->bottom;
parms.fExtendedSel = fExtendedSel;
parms.fSelectAll = NO;
PostCommand(lpImage->lpCmdList, IDS_CMD_SELECTOBJECTS, &parms);
SetupMiniViews(NULL, YES);
return(TRUE);
}

/**************************************************************************/
LOCAL BOOL DuplicateObjects(LPIMAGE lpImage)
/**************************************************************************/
{
CMD_PARMS parms;

AstralCursor(IDC_WAIT);
if (!ImgDuplicateSelObjects(lpImage))
	{
	AstralCursor(NULL);
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
else
	PostCommand(lpImage->lpCmdList, IDS_CMD_DUPLICATEOBJECTS, &parms);
AstralCursor(NULL);
return(TRUE);
}






