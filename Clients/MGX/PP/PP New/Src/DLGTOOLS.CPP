//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"

extern CShapeEdit PointEditShape;							

// LOCAL prototypes
LOCAL BOOL Pointer_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Pointer_OnDocActivated(HWND hDlg);
LOCAL void Pointer_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Pointer_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Pointer_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPointerProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Pointer_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Pointer_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Pointer_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Pointer_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FREE);
	}
}

/************************************************************************/
LOCAL BOOL Pointer_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
Mask.PointEdit = FALSE;
Mask.PointIModes = SHAPE_REVERSE;
//	ShowExtra(hDlg, IDC_POINT_WND, IDC_FREEHAND_WND, );
//	ShowPointerWindow(hDlg, Mask.PointEdit);

// show/hide panes
ShowPane(hDlg, IDC_PANE0, IDC_PANE1, TRUE/*!Tool.bActive*/);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2, /*!Tool.bActive &&*/ Mask.MakePathType == IDC_PATH_CURVES);
ShowPane(hDlg, IDC_PANE2, IDC_PANE3, /*!Tool.bActive &&*/ Mask.MakePathType != IDC_PATH_CURVES);
ShowPane(hDlg, IDC_PANE3, IDC_PANE4, FALSE/*Tool.bActive*/);


CheckDlgButton( hDlg, IDC_ANTIALIAS, Mask.PointAntialias );

CheckComboItem( hDlg, IDC_PATHTYPE, IDC_PATH_LINES, IDC_PATH_CURVES,
	Mask.MakePathType);
InitDlgItemSpin( hDlg, IDC_PATHCONTINUITY, Mask.MakePathContinuity, NO,
	0, 20 );

InitDlgItemSpin( hDlg, IDC_PATHCURVE_TOLERANCE, Mask.MakePathCurveTolerance, NO,
	0, 99 );

InitDlgItemSpin( hDlg, IDC_PATHTOLERANCE, Mask.MakePathTolerance, NO,
	0, 99 );
	
Pointer_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Pointer_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Pointer_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Pointer_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Pointer_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Pointer_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
BOOL Bool;
int 	function;
LPIMAGE lpImage;
FRMDATATYPE Type = FDT_RGBCOLOR;

lpImage = GetActiveImage();
ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);

ShowPane(hDlg, IDC_PANE0, IDC_PANE1, !Tool.bActive);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2, !Tool.bActive && Mask.MakePathType == IDC_PATH_CURVES);
ShowPane(hDlg, IDC_PANE2, IDC_PANE3, !Tool.bActive && Mask.MakePathType != IDC_PATH_CURVES);
ShowPane(hDlg, IDC_PANE3, IDC_PANE4, Tool.bActive);

CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
IDC_MASKMODE_REVERSE + max(Mask.PointIModes - SHAPE_REVERSE, 0) );
function = PointEditShape.GetExtendedFunction();
SetPointerFn(hDlg, function);
Bool = ImgGetMask(lpImage) != NULL;
ControlEnable( hDlg, IDC_MASKMODE_ADD, Bool);
ControlEnable( hDlg, IDC_MASKMODE_SUBTRACT, Bool);
ControlEnable( hDlg, IDC_MASKMODE_REVERSE, Bool);
ControlEnable( hDlg, IDC_PATHCONTINUITY, Bool);
ControlEnable( hDlg, IDC_PATHCURVE_TOLERANCE, Bool);
ControlEnable( hDlg, IDC_PATHTOLERANCE, Bool);
ControlEnable( hDlg, IDC_PATHTYPE, Bool);
ControlEnable( hDlg, IDC_ANTIALIAS, CANDOTRANSPARENCY(Type));
}

/************************************************************************/
LOCAL void Pointer_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;

switch (id)
	{
	case IDC_POINTER:
		if ( !SaveToolPreferences( id ) )
			break;
	break;
	case IDC_MOVEPOINTS:
	case IDC_ADDPOINTS:
	case IDC_DELPOINTS:
	case IDC_IS_POINT:
	case IDC_IS_BEZIER:	
		if (id == IDC_IS_POINT)
		{
			PointEditShape.MakeCorners();
			break;
		}
		if (id == IDC_IS_BEZIER)
		{
			PointEditShape.MakeCurves();
			break;
		}
		
		PointEditShape.SetBaseFunction(id);
   		SetPointerFn(hDlg, PointEditShape.GetExtendedFunction());
	break;

	case IDC_MASKMODE_REVERSE:
	case IDC_MASKMODE_ADD:
	case IDC_MASKMODE_SUBTRACT:
		Mask.PointIModes = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
		CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
			id );
		SetCommonMode(Mask.PointIModes);
	break;
	
	case IDC_ANTIALIAS:
		Mask.PointAntialias = !Mask.PointAntialias;
		CheckDlgButton( hDlg, id, Mask.PointAntialias );
	break;

	case IDC_PATHTYPE:
//	case IDC_PATH_LINES:
//	case IDC_PATH_CURVES:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.MakePathType = id;
	ShowPane(hDlg, IDC_PANE0, IDC_PANE1, !Tool.bActive);
	ShowPane(hDlg, IDC_PANE1, IDC_PANE2, !Tool.bActive && Mask.MakePathType == IDC_PATH_CURVES);
	ShowPane(hDlg, IDC_PANE2, IDC_PANE3, !Tool.bActive && Mask.MakePathType != IDC_PATH_CURVES);
	ShowPane(hDlg, IDC_PANE3, IDC_PANE4, Tool.bActive);
	break;

	case IDC_PATHTOLERANCE:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Mask.MakePathTolerance = GetDlgItemSpin( hDlg, id,
			&Bool, NO );
	break;

	case IDC_PATHCURVE_TOLERANCE:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Mask.MakePathCurveTolerance = GetDlgItemSpin( hDlg, id,
			&Bool, NO );
	break;

	case IDC_PATHCONTINUITY:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Mask.MakePathContinuity = GetDlgItemSpin( hDlg, id,
			&Bool, NO );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL View_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void View_OnDocActivated(HWND hDlg);
LOCAL void View_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void View_HandleEnable(HWND hDlg, int fFlags);
LOCAL void View_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgViewProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, View_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, View_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, View_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, View_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CUSTOMVIEW);
	}
}

/************************************************************************/
LOCAL BOOL View_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CheckComboItem( hDlg, IDC_CUSTOMFUNCTION, IDC_ZOOMIN, IDC_ZOOMOUT,
	View.ZoomOut + IDC_ZOOMIN );
CheckDlgButton( hDlg, IDC_ZOOMWINDOW, View.ZoomWindow );
View_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void View_OnDocActivated(HWND hDlg)
/************************************************************************/
{
View_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void View_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
View_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void View_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
CheckDlgButton(hDlg, IDC_QUICKZOOM, Window.fHasZoom);
}

/************************************************************************/
LOCAL void View_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_CUSTOMVIEW:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_CUSTOMFUNCTION:
//	case IDC_ZOOMIN:
//	case IDC_ZOOMOUT:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	View.ZoomOut = id - IDC_ZOOMIN;
	break;

	case IDC_ZOOMWINDOW:
	View.ZoomWindow = !View.ZoomWindow;
	CheckDlgButton( hDlg, IDC_ZOOMWINDOW, View.ZoomWindow );
	break;

	case IDC_VIEWLAST:
	if ( !GetActiveDoc() )
		break;
	RevertLastView(GetActiveDoc());
	break;

	case IDC_VIEWFULL:
	if ( !GetActiveDoc() )
		break;
	AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_VIEWFULL,
		DlgFullScreenViewProc );
	break;

	case IDC_VIEWALL:
	if ( !GetActiveDoc() )
		break;
	ViewAll(GetActiveDoc());
	break;

	case IDC_VIEW1TO1:
   View1TO1();
	break;

	case IDC_QUICKZOOM:
	SEND_WM_COMMAND(PictPubApp.Get_hWndAstral(), IDM_SHOWZOOMBOX, NULL, 0);
	CheckDlgButton(hDlg, IDC_QUICKZOOM, Window.fHasZoom);
	break;

	default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL CopyToFile_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void CopyToFile_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void CopyToFile_OnShowWindow(HWND hwnd, BOOL fShow, UINT status);

static int CopyToFile_iMode = IDN_CLIPBOARD, CopyToFile_iSaveMode;
static FNAME CopyToFile_Name;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCopyToFileProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, CopyToFile_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, CopyToFile_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_SHOWWINDOW, CopyToFile_OnShowWindow);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_COPYTOFILE);
	}
}

/************************************************************************/
LOCAL BOOL CopyToFile_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
ITEMID idString, idControl;
LPSTR lpString;

CenterPopup( hDlg );
lstrcpy( Names.Saved, CopyToFile_Name );
if (CopyToFile_iMode == IDN_TEXTURE)
	{
	idString = IDS_COPYTOTEXTURE;
	idControl = IDC_COPYTOTEXTURE;
	}
else
if (CopyToFile_iMode == IDN_CUSTOM)
	{
	idString = IDS_COPYTOBRUSH;
	idControl = IDC_COPYTOBRUSH;
	}
else
if (CopyToFile_iMode == IDN_CLIPBOARD)
	{
	idString = IDS_COPYTOCLIP;
	idControl = IDC_COPYTOCLIP;
	}
else
	{
	idString = IDS_COPYTOIMAGE;
	idControl = IDC_COPYTOIMAGE;
	}
if ( AstralStr( idString, &lpString ) )
	SetDlgItemText( hDlg, IDC_COPYTONAME, lpString );
CheckRadioButton( hDlg, IDC_COPYTOCLIP, IDC_COPYTOBRUSH, idControl );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, CopyToFile_Name, CopyToFile_iMode );
ShowControl( hDlg, IDC_COPYTONAME, IDC_COPYTONAME, CopyToFile_iMode, YES );
ShowControl( hDlg, IDC_EXTNAMES, IDC_EXTNAMES, CopyToFile_iMode, YES );
ShowControl( hDlg, IDC_CLIPEXTMANAGE, IDC_CLIPEXTMANAGE, CopyToFile_iMode, YES );
ShowControl( NULL, 0, 0, FALSE, FALSE );
CopyToFile_iSaveMode = CopyToFile_iMode;
return(TRUE);
}

/************************************************************************/
LOCAL void CopyToFile_OnShowWindow(HWND hDlg, BOOL fShow, UINT status)
/************************************************************************/
{
SetFocus(GetDlgItem(hDlg, IDC_EXTNAMES));
ComboBox_SetEditSel(GetDlgItem(hDlg, IDC_EXTNAMES), 0, -1);
}

/************************************************************************/
LOCAL void CopyToFile_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName, szMaskName;
LPSTR lpString;
ITEMID idString;
LPIMAGE lpImage;
COPYTOFILE_PARMS parms;

lpImage = GetActiveImage();
switch (id)
	{
	case IDOK:
	if ( !lpImage )
		break;
	if ( !CopyToFile_iMode )
		{
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_COPYTOIMAGE, &parms);
		AstralDlgEnd( hDlg, TRUE);
		break;
		}
	if ( LookupExtFile(CopyToFile_Name, szFileName, CopyToFile_iMode) ||
		(CopyToFile_iMode == IDN_MASK &&
		LookupExtFile(CopyToFile_Name, szMaskName, IDN_MASK)) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			(LPTR)CopyToFile_Name) == IDCANCEL)
			break;
		DeleteExtFile(CopyToFile_Name, CopyToFile_iMode);
		if (CopyToFile_iMode == IDN_CLIPBOARD)
			DeleteExtFile(CopyToFile_Name, IDN_MASK);
		}
	lstrcpy(parms.szName, CopyToFile_Name);
	lstrcpy( Names.Clipboard, CopyToFile_Name );
	if (CopyToFile_iMode == IDN_TEXTURE)
		parms.Type = 1;
	else
	if (CopyToFile_iMode == IDN_CUSTOM)
		parms.Type = 2;
	else
		parms.Type = 0;
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_COPYTOFILE, &parms);
	AstralDlgEnd( hDlg, TRUE);
	break;

	case IDCANCEL:
	lstrcpy( CopyToFile_Name, Names.Saved );
	CopyToFile_iMode = CopyToFile_iSaveMode;
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		CopyToFile_Name, CopyToFile_iMode, codeNotify );
	break;

	case IDC_CLIPEXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		CopyToFile_Name, CopyToFile_iMode, id, NO );
	break;

	case IDC_COPYTOCLIP:
	case IDC_COPYTOIMAGE:
	case IDC_COPYTOTEXTURE:
	case IDC_COPYTOBRUSH:
	if (id == IDC_COPYTOTEXTURE)
		{
		CopyToFile_iMode = IDN_TEXTURE;
		idString = IDS_COPYTOTEXTURE;
		}
	else
	if (id == IDC_COPYTOBRUSH)
		{
		CopyToFile_iMode = IDN_CUSTOM;
		idString = IDS_COPYTOBRUSH;
		}
	else
	if (id == IDC_COPYTOCLIP)
		{
		CopyToFile_iMode = IDN_CLIPBOARD;
		idString = IDS_COPYTOCLIP;
		}
	else
		{ // Copy to new image
		CopyToFile_iMode = 0;
		idString = IDS_COPYTOIMAGE;
		}
	if ( AstralStr( idString, &lpString ) )
		SetDlgItemText( hDlg, IDC_COPYTONAME, lpString );
	CheckRadioButton( hDlg, IDC_COPYTOCLIP, IDC_COPYTOBRUSH,
		id );
	if ( CopyToFile_iMode )
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, CopyToFile_Name, CopyToFile_iMode);
	ShowControl( hDlg, IDC_COPYTONAME, IDC_COPYTONAME, CopyToFile_iMode, YES );
	ShowControl( hDlg, IDC_EXTNAMES, IDC_EXTNAMES, CopyToFile_iMode, YES );
	ShowControl( hDlg, IDC_CLIPEXTMANAGE, IDC_CLIPEXTMANAGE, CopyToFile_iMode, YES );
	ShowControl( NULL, 0, 0, FALSE, FALSE );
	SetFocus(GetDlgItem(hDlg, IDC_EXTNAMES));
	ComboBox_SetEditSel(GetDlgItem(hDlg, IDC_EXTNAMES), 0, -1);
	break;

   default:
	break;
   }
}

/***********************************************************************/
BOOL CopyToNewImage( LPIMAGE lpImage, LPCMD_PARMS lpParms )
/***********************************************************************/
{
LPIMAGE lpNewImage;
LPOBJECT lpObject;

ProgressBegin(1, 0);

LPNEWIMAGEINFO lpInfo;

lpInfo = (LPNEWIMAGEINFO)Alloc(sizeof(NEWIMAGEINFO));
if (!lpInfo)
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
clr((LPTR)lpInfo, sizeof(NEWIMAGEINFO));

// copy the Object from the image
if (!(lpObject = ImgCreateClipOutObject(lpImage)))
	{
	FreeUp(lpInfo);
	ProgressEnd();
	Message(IDS_EMEMALLOC);
	return( FALSE );
	}

// Setup the new image
lpNewImage = CreateImage(
	NULL,					// Name
	NULL,					// lpNewFrame
	lpObject,				// lpBase
	NULL,					// lpCmdList
	lpImage->FileType,		// lpImage->FileType
	lpImage->DataType,		// lpImage->DataType
	lpImage->DocumentType,	// lpImage->DocumentType
	NULL);					// lpImage->ImageName

ProgressEnd();
if (lpNewImage)
	{
	lpNewImage->fChanged = YES;
	lpInfo->lpImage = lpNewImage;
	ListAddTail(&lpParms->Common.NewImageList, lpInfo);
	lpParms->Common.UpdateType = UT_NEWIMAGE;
	return(TRUE);
	}
else
	{
	FreeUp(lpInfo);
	return(FALSE);
	}
}

// LOCAL prototypes
LOCAL BOOL MaskLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MaskLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMaskLoadProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MaskLoad_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MaskLoad_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MASKLOAD);
	}
}

/************************************************************************/
LOCAL BOOL MaskLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask, IDN_MASK );
lstrcpy( Names.Saved, Names.Mask );
CheckDlgButton(hDlg, IDC_DELETEMASK, Mask.DeleteMask);
ControlEnable(hDlg, IDC_DELETEMASK, ImgGetMask(GetActiveImage()) != NULL);
return(TRUE);
}

/************************************************************************/
LOCAL void MaskLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;
LPALPHA lpAlpha;
LPIMAGE lpImage = NULL;
HWND hActiveWnd;
RECT rUpdate;
BOOL fTransform;

hActiveWnd = GetActiveDoc();
if (hActiveWnd)
	lpImage = GetImagePtr(hActiveWnd);
switch (id)
	{
	case IDOK:
	if (!lpImage)
		break;

	if ( !LookupExtFile( Names.Mask, szFileName, IDN_MASK ) )
		break;
	if (!(lpAlpha = ReadMask(lpImage, szFileName, 0, 0)))
		break;
	AstralDlgEnd( hDlg, TRUE );
	AstralUpdateWindow(hActiveWnd);
	AstralSetRectEmpty(&rUpdate);
	if (!LoadMaskStart(lpImage, lpAlpha, Mask.DeleteMask, NO, &rUpdate, 
						&fTransform))
		break;
	if (fTransform)
		TransformStartLoadMaskProc(lpImage, Mask.DeleteMask);
	else
		{
		int res, dx, dy;
		LOADMASK_PARMS parms;

		ImgGetInfo(lpImage, &dx, &dy, NULL, NULL);
		res = FrameResolution(ImgGetBaseEditFrame(lpImage));
		parms.fHiQuality = Mask.PasteQuality;
		parms.Mode = SHAPE_ADD;
		parms.iBaseRes = res;
		parms.DistortPoints[0].x = 0;
		parms.DistortPoints[0].y = 0;
		parms.DistortPoints[1].x = dx-1;
		parms.DistortPoints[1].y = 0;
		parms.DistortPoints[2].x = dx-1;
		parms.DistortPoints[2].y = dy-1;
		parms.DistortPoints[3].x = 0;
		parms.DistortPoints[3].y = dy-1;
		lstrcpy(parms.szMask, Names.Mask);
		LookupExtFileN(parms.szMask, parms.szMaskFileName, IDN_MASK, NO);
		parms.fDeleteMask = Mask.DeleteMask;
		PostCommand(lpImage->lpCmdList, IDS_CMD_LOADMASK, &parms);
		if (!AstralIsRectEmpty(&rUpdate))
			{
			File2DispRect(hActiveWnd, &rUpdate, &rUpdate);
			InvalidateRect(hActiveWnd, AstralToWindowsRect(&rUpdate), FALSE);
			}
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		}
	break;

	case IDCANCEL:
	lstrcpy( Names.Mask, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_DELETEMASK:
	Mask.DeleteMask = !Mask.DeleteMask;
	CheckDlgButton(hDlg, id, Mask.DeleteMask);
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.Mask, IDN_MASK, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask,
		IDN_MASK, id, NO );
	break;

	default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL MaskSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MaskSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMaskSaveProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MaskSave_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MaskSave_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MASKSAVE);
	}
}

/************************************************************************/
LOCAL BOOL MaskSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask, IDN_MASK );
lstrcpy( Names.Saved, Names.Mask );
return(TRUE);
}

/************************************************************************/
LOCAL void MaskSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;
LPMASK lpMask;
LPIMAGE lpImage;
SAVEMASK_PARMS parms;

lpImage = GetActiveImage();

switch (id)
	{
	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.Mask, IDN_MASK, codeNotify );
	break;

	case IDC_CLIPEXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask,
		IDN_MASK, id, NO );
	break;

	case IDOK:
	if ( !(lpMask = ImgGetMask(lpImage)) )
		break;
	if ( LookupExtFile(Names.Mask, szFileName, IDN_MASK) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE, (LPTR)Names.Mask) == IDCANCEL)
			break;
		DeleteExtFile(Names.Mask, IDN_MASK);
		}
	lstrcpy(parms.szName, Names.Mask);
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_SAVEMASK, &parms);
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.Mask, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL ApplyUndo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ApplyUndo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgApplyUndoProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, ApplyUndo_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, ApplyUndo_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_APPLYORUNDO);
	}
}

/************************************************************************/
LOCAL BOOL ApplyUndo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
return(TRUE);
}

/************************************************************************/
LOCAL void ApplyUndo_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_APPLY:
	SEND_WM_COMMAND(PictPubApp.Get_hWndAstral(), IDM_APPLY, NULL, 0);
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDC_UNDO:
	SEND_WM_COMMAND(PictPubApp.Get_hWndAstral(), IDM_UNDO, NULL, 0);
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL CustomBrush_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void CustomBrush_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

LOCAL BOOL CustomBrush_fEdit;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCustomBrushProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, CustomBrush_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, CustomBrush_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CUSTOMBRUSH);
	}
}

/************************************************************************/
LOCAL BOOL CustomBrush_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.CustomBrush,
	IDN_CUSTOM );
CheckDlgButton( hDlg, IDC_EDIT_BRUSH, CustomBrush_fEdit = FALSE);
lstrcpy( Names.Saved, Names.CustomBrush);
return(TRUE);
}

/************************************************************************/
LOCAL void CustomBrush_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;
LPFRAME lpFrame;
LPTR lpLine;
long black;
int y;
RGBS blackRGB;

switch (id)
	{
	case IDOK:
	if ( LookupExtFile( Names.CustomBrush, szFileName, IDN_CUSTOM ))
		{
		if ( CustomBrush_fEdit )
			{
			FRMTYPEINFO TypeInfo;

			FrameSetTypeInfo(&TypeInfo, FDT_GRAYSCALE, NULL);
			CReadImage read(szFileName);
			if (!(lpFrame = read.ReadFrame(TypeInfo) ))
				break;
            LPIMAGE lpNewImage = CreateImage(szFileName, lpFrame, NULL, 
               NULL, IDN_TIFF, IDC_SAVECT, IMG_BRUSH, Names.CustomBrush);
            if (lpNewImage)
                {
                if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
                   lpNewImage))         
                    {
                    /* only version of image is in the cache */
                	/* so insure user is asked about saving when done */
                    lpNewImage->fChanged = TRUE;
                    }
                else
                    DestroyImage(lpNewImage);
                }
            }
		}
	else
		{
		if ( !Names.CustomBrush[0] )
			{
			MessageBeep(0);
			break;
			}
		if (AstralOKCancel(IDS_NEWBRUSH,
			(LPTR)Names.CustomBrush) != IDOK)
			break;
		if (!(lpFrame = FrameOpen( FDT_GRAYSCALE, MAX_BRUSH_SIZE,
			MAX_BRUSH_SIZE, 100)))
			break;
		blackRGB.red = blackRGB.green = blackRGB.blue = 0;
		FrameRGB2Pixel(lpFrame, &blackRGB, &black);
		for (y = 0; y < FrameYSize(lpFrame); ++y)
			{
			if ((lpLine = FramePointer(lpFrame, 0, y, YES)))
				FrameSetPixel(lpFrame, black, lpLine, FrameXSize(lpFrame));
			}
		if ( !CreateFileName( Names.CustomBrush, IDN_CUSTOM, szFileName ) )
			{
			FrameClose(lpFrame);
			break;
			}
		CWriteImage write(szFileName, IDN_TIFF);
		if (!write.WriteFrame(lpFrame, IDC_SAVECT))
			{
			FrameClose(lpFrame);
			break;
			}
		if (!AddExtFile(Names.CustomBrush, szFileName, IDN_CUSTOM))
			{
			FrameClose(lpFrame);
			FileDelete(szFileName);
			break;
			}
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.CustomBrush, IDN_CUSTOM );
		if ( CustomBrush_fEdit )
        {
            LPIMAGE lpNewImage = CreateImage(szFileName, lpFrame, NULL, 
               NULL, IDN_TIFF, IDC_SAVECT, IMG_BRUSH, Names.CustomBrush);
            if (lpNewImage)
            {
                if (PictPubApp.OpenDocumentFile((LPSTR)lpNewImage->CurFile,
                   lpNewImage))         
                {
                   	/* only version of image is in the cache */
                	/* so insure user is asked about saving when done */
                    lpNewImage->fChanged = TRUE;
                }
                else
                    DestroyImage(lpNewImage);
            }
        }
		else
			FrameClose(lpFrame);
		}

	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	lstrcpy( Names.CustomBrush, Names.Saved );
	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.CustomBrush, IDN_CUSTOM, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		Names.CustomBrush, IDN_CUSTOM, id, NO );
	lstrcpy( Names.Saved, Names.CustomBrush);
	break;

	case IDC_EDIT_BRUSH:
	CustomBrush_fEdit = !CustomBrush_fEdit;
	CheckDlgButton( hDlg, IDC_EDIT_BRUSH, CustomBrush_fEdit);
	break;

   default:
	break;
   }
}


