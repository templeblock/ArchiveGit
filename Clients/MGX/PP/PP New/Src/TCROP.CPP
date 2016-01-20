//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// Mask tool statics
static long AspectX, AspectY;
static BOOL fConstrain, fShift;
static RECT SelectRect;

#define UNITPARENT(hDlg) (hDlg)
#define SIZEPARENT(hDlg) (hDlg)

// LOCAL prototypes
LOCAL BOOL Crop_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Crop_OnDocActivated(HWND hDlg);
LOCAL void Crop_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Crop_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Crop_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
LOCAL void Crop_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);
LOCAL HBRUSH Crop_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, WORD wType);
LOCAL void Crop_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgCropProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Crop_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Crop_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Crop_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Crop_OnControlEnable);
	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Crop_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Crop_OnDrawItem);
	HANDLE_CONTROL_COLORS(hDlg, Crop_OnCtlColor);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CROP);
	}
}

/************************************************************************/
LOCAL BOOL Crop_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
int SaveUnits;

CheckComboItem( hDlg, IDC_ASPECTRATIO, IDC_ASPECTFIRST, IDC_ASPECTLAST,
	Mask.ConstrainCropAspect );
SaveUnits = Control.Units;
Control.Units = Mask.CropUnits;
SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_HCONSTRAIN, Mask.ConstrainCropHeight,
		NO );
AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_WCONSTRAIN, Mask.ConstrainCropWidth,
		NO );
Control.Units = SaveUnits;
SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
CheckComboItem(UNITPARENT(hDlg), IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
	IDC_PREF_UNITLAST, Mask.CropUnits);
Crop_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Crop_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Crop_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Crop_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Crop_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Crop_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
SetUnitResolution(0);
ShowPane(hDlg, IDC_PANE0, IDC_PANE1, Mask.ConstrainCropAspect != IDC_UNCONSTRAIN);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2, Mask.ConstrainCropAspect == IDC_FIXEDSIZE);
//ShowDlgItem(hDlg, IDC_CONSTRAINSIZE_WND,
//			Mask.ConstrainCropAspect != IDC_UNCONSTRAIN);
//ShowDlgItem(hDlg, IDC_CONSTRAINUNIT_WND,
//			Mask.ConstrainCropAspect == IDC_FIXEDSIZE);
}

/************************************************************************/
LOCAL void Crop_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
if (lpMeasureItem->CtlID == IDC_PREF_UNITS)
	hDlg = UNITPARENT(hDlg);
MeasureItem( hDlg, lpMeasureItem, NO );
}

/************************************************************************/
LOCAL void Crop_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
if (lpDrawItem->CtlID == IDC_PREF_UNITS)
	hDlg = UNITPARENT(hDlg);
DrawItem( hDlg, lpDrawItem, 0 );
}

/***********************************************************************/
LOCAL HBRUSH Crop_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, WORD wType)
/***********************************************************************/
{
if (hControl == GetDlgItem(UNITPARENT(hDlg), IDC_PREF_UNITS))
	hDlg = UNITPARENT(hDlg);
return( Dialog_OnCtlColor( hDlg, hDC, hControl, wType ) );
}

/************************************************************************/
LOCAL void Crop_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int idUnits, SaveUnits;

switch (id)
	{
	case IDC_CROP:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_ASPECTRATIO:
//	case IDC_UNCONSTRAIN:
//	case IDC_CONSTRAIN:
//	case IDC_FIXEDSIZE;
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.ConstrainCropAspect = id;
	SetCommonConstrain(Mask.ConstrainCropAspect);
	SendMessage(hDlg, WM_CONTROLENABLE, 0, 0L);
	break;

	case IDC_HCONSTRAIN:
	hDlg = SIZEPARENT(hDlg);
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	SaveUnits = Control.Units;
	Control.Units = Mask.CropUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	Mask.ConstrainCropHeight = AstralGetDlgItemFixed( hDlg,
		id, &Bool, NO );
	Control.Units = SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetCommonHeight(Mask.ConstrainCropHeight);
	break;

	case IDC_WCONSTRAIN:
	hDlg = SIZEPARENT(hDlg);
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	SaveUnits = Control.Units;
	Control.Units = Mask.CropUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	Mask.ConstrainCropWidth = AstralGetDlgItemFixed( hDlg,
		id, &Bool, NO );
	Control.Units = SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetCommonWidth(Mask.ConstrainCropWidth);
	break;

	case IDC_PREF_UNITS:
	if ( !(idUnits = HandleCombo( UNITPARENT(hDlg), id, codeNotify )) )
		break;
	Mask.CropUnits = idUnits;
	SaveUnits = Control.Units;
	Control.Units = Mask.CropUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_HCONSTRAIN,
		Mask.ConstrainCropHeight, NO);
	AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_WCONSTRAIN,
		Mask.ConstrainCropWidth, NO);
	Control.Units = SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetCommonUnits(Mask.CropUnits);
	break;

   default:
	break;
   }
}

/************************************************************************/
void CropProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy, pix, lin, res, XSize, YSize;
BOOL fCancel = NO;
LPFRAME lpFrame;
POINT pt;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWindow);
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	if (lpImage->fNoSize)
		{
		Message(IDS_NOTINMASKEDIT);
		break;
		}
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	fConstrain = Mask.ConstrainCropAspect == IDC_CONSTRAIN;
	if (fConstrain)
		{
		AspectX = FMUL (100, Mask.ConstrainCropWidth);
		AspectY = FMUL (100, Mask.ConstrainCropHeight);
      double Max = AspectX > AspectY? AspectX : AspectY;
      AspectX = (AspectX / Max) * 1000.0;
      AspectY = (AspectY / Max) * 1000.0;
		}
	else
		AspectX = AspectY = 1L;
	fShift = NO;
	
	if (Mask.ConstrainCropAspect == IDC_FIXEDSIZE)
		{
		lpFrame = ImgGetBaseEditFrame(lpImage);
		res = FrameResolution(lpFrame);
		pix = FMUL( res, Mask.ConstrainCropWidth );
		lin = FMUL( res, Mask.ConstrainCropHeight );
		if (!pix || !lin)
			{
			Tool.bActive = NO;
			break;
			}
		}

	AstralSetRectEmpty( &SelectRect );
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, SL_BOX|SL_SPECIAL,
				pt, 0L );

	if (Mask.ConstrainCropAspect == IDC_FIXEDSIZE)
		{
		InvertSelection(hWindow, NULL, &SelectRect, SL_BOX);
		XSize = FrameXSize(lpFrame);
		YSize = FrameYSize(lpFrame);
		if (pix > XSize)
			pix = XSize;
		if (lin > YSize)
			lin = YSize;
		fx = x; fy = y;
		Display2File(hWindow, &fx, &fy);
		SelectRect.left = fx - (pix/2);
		if (SelectRect.left < 0) 
			SelectRect.left = 0;
		else
		if ((SelectRect.left + pix) > XSize)
			SelectRect.left = XSize - pix;
		SelectRect.top = fy - (lin/2);
		if (SelectRect.top < 0)
			SelectRect.top = 0;
		else
 		if ((SelectRect.top + lin) > YSize)
			SelectRect.top = YSize - lin;
		SelectRect.right = SelectRect.left + pix - 1;
		SelectRect.bottom = SelectRect.top + lin - 1;
		InvertSelection(hWindow, NULL, &SelectRect, SL_BOX);
		}
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	pix = RectWidth(&SelectRect);
	lin = RectHeight(&SelectRect);
	if (pix < 1 || lin < 1)
		break;
	lpFrame = ImgGetBaseEditFrame(lpImage);
	XSize = FrameXSize(lpFrame);
	YSize = FrameYSize(lpFrame);
	if (pix < XSize || lin < YSize)
		ImgCrop(lpImage, &SelectRect);
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	fShift = SHIFT;
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
		pt, fConstrain^CONSTRAINASPECT, AspectX, AspectY,
		MOVEKEY||Window.fRButtonDown||(Mask.ConstrainCropAspect == IDC_FIXEDSIZE),
		FROMCENTER);
	break;

	case WM_DESTROY:	// The cancel operation message
	fCancel = (Option == 1);
	case WM_LBUTTONDBLCLK:
	if ( !Tool.bActive )
		break;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	Tool.bActive = NO;
	break;
	}
}




