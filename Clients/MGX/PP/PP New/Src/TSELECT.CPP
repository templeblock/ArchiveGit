//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "pointutl.h"
#include "maskutil.h"

#define AUTOMASKWND(hDlg) (hDlg)

// Static Prototypes
LOCAL void FreeEraseLastBezier(HWND hWindow);
LOCAL void ChangeFreeMode(HWND hWindow, BOOL bPointEdit, BOOL bWasfreeBezier);
LOCAL void freemask(HWND hWindow, LPCPolyList lpPolyList);
LOCAL BOOL free_load(HWND hWnd);
LOCAL BOOL add_to_free_proc(LPCPoly lpPoly);
LOCAL BOOL close_poly_proc(LPCPoly lpPoly, LONG bClose);
LOCAL void free_save(HWND hWnd);
BOOL WINPROC DlgShapeSaveProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgShapeLoadProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
LOCAL void bounded_display2file(HWND hWindow, LPLONG x, LPLONG y);
LOCAL void bounded_display2file(HWND hWindow, LPINT x, LPINT y);
class CShapeEditDrawHook2 : public CHookHandler
{
public:
    CShapeEditDrawHook2() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CShapeEditDrawHook2 ShapeEditDrawHook;


class CMaskDrawHook : public CHookHandler
{
public:
    CMaskDrawHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CMaskDrawHook MaskDrawHook;

LOCAL void DrawFreeLine(HWND hWnd, HDC hDC, int newx, int newy, BOOL on, BOOL bBezier, BOOL bWhole = YES);
LOCAL void DrawFree(LPIMAGE lpImage, LPCPolyList lpPolyList);
LOCAL void MaskFreeBackup( HWND hWnd );
LOCAL void vector_draw( HWND hWnd, int x, int y, BOOL bCapture, BOOL bBezier);
LOCAL void MaskUndraw( HWND hWnd, HDC hDC, LPCPolyList lpPolyList, BOOL fOn);	
LOCAL void MaskDrawBezier(HWND hWnd, HDC hDC, LPPOINT lpBezier, BOOL fOn);
LOCAL void ProcessMagicMask(LPIMAGE lpImage);
LOCAL void ProcessSimilar(LPIMAGE lpImage);

// Mask tool statics
static long AspectX, AspectY;
static BOOL fConstrain, fShift;
static RECT SelectRect;

// Freehand & point edit mask stuff
static BOOL bLeft = TRUE;  						// cursor direction
static int gx, gy;								// freehand - last pos (file coords)
BOOL bFreeBezier;						// adding beziers in free mode
static LPCPolyList lpPolyList = NULL;			// the list of polys currently editing
static POSITION PolyPos = NULL;					// The position of the poly to edit
static LPCPolyList	lpSaveLoadPoly;				// Used to pass paths to shape save and load 
// the shape editing object
static CShapeEdit ShapeEdit;							

// for grow and similar
static BOOL fGotMagic;
static BOOL fUseSimilar;
static int magicX, magicY;
static int magicRange;
static int magicFade;
static int magicColorModel;
static int magiciMode;


//#define UNITPARENT(hDlg) (GetDlgItem(hDlg, IDC_CONSTRAINUNIT_WND))
//#define SIZEPARENT(hDlg) (GetDlgItem(hDlg, IDC_CONSTRAINSIZE_WND))
#define UNITPARENT(hDlg) (hDlg)
#define SIZEPARENT(hDlg) (hDlg)


// LOCAL Window Proc prototypes
LOCAL BOOL Rectangular_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Rectangular_OnDocActivated(HWND hDlg);
LOCAL void Rectangular_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Rectangular_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Rectangular_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Rectangular_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
LOCAL void Rectangular_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);
LOCAL HBRUSH Rectangular_OnCtlColor(HWND hDlg, HDC hdc, HWND hwndChild, int type);

/***********************************************************************/
BOOL WINPROC EXPORT DlgRectangularProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Rectangular_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Rectangular_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Rectangular_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Rectangular_OnControlEnable);

	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Rectangular_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Rectangular_OnDrawItem);
	HANDLE_CONTROL_COLORS(hDlg, Rectangular_OnCtlColor);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_SQUARE);
	}
}

/************************************************************************/
LOCAL BOOL Rectangular_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
int SaveUnits;

CheckComboItem( hDlg, IDC_MASKSHAPE, IDC_SQUAREMASK, IDC_CIRCLEMASK,
	IDC_SQUAREMASK + Mask.DoMaskCircle );
CheckComboItem( hDlg, IDC_ASPECTRATIO, IDC_ASPECTFIRST, IDC_ASPECTLAST,
			Mask.ConstrainRectAspect );
SaveUnits = Control.Units;
SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
Control.Units = Mask.ShapeUnits;
AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_HCONSTRAIN,
					Mask.ConstrainRectHeight, NO );
AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_WCONSTRAIN,
					Mask.ConstrainRectWidth, NO );
Control.Units = SaveUnits;
SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
CheckComboItem(UNITPARENT(hDlg), IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
	IDC_PREF_UNITLAST, Mask.ShapeUnits);
Rectangular_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Rectangular_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Rectangular_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Rectangular_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Rectangular_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Rectangular_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
LPIMAGE lpImage;

lpImage = GetActiveImage();
if (!ImgGetMask(lpImage) && Mask.iShapeModes == SHAPE_SUBTRACT)
	Mask.iShapeModes = SHAPE_DEFAULT;
CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
	IDC_MASKMODE_REVERSE + max(Mask.iShapeModes - SHAPE_REVERSE, 0) );
SetUnitResolution(0);
//ShowDlgItem(hDlg, IDC_CONSTRAINSIZE_WND,
//			Mask.ConstrainRectAspect != IDC_UNCONSTRAIN);
//ShowDlgItem(hDlg, IDC_CONSTRAINUNIT_WND,
//			Mask.ConstrainRectAspect == IDC_FIXEDSIZE);
ShowPane(hDlg, IDC_PANE0, IDC_PANE1, Mask.ConstrainRectAspect != IDC_UNCONSTRAIN);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2, Mask.ConstrainRectAspect == IDC_FIXEDSIZE);
}

/************************************************************************/
LOCAL void Rectangular_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
if (lpMeasureItem->CtlID == IDC_PREF_UNITS)
	hDlg = UNITPARENT(hDlg);
MeasureItem( hDlg, lpMeasureItem, lpMeasureItem->CtlID == IDC_MASKSHAPE );
}

/************************************************************************/
LOCAL void Rectangular_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
if (lpDrawItem->CtlID == IDC_PREF_UNITS)
	hDlg = UNITPARENT(hDlg);
DrawItem( hDlg, lpDrawItem, 0 );
}

/************************************************************************/
HBRUSH Rectangular_OnCtlColor(HWND hDlg, HDC hdc, HWND hWndChild, int type)
/************************************************************************/
{
if (hWndChild == GetDlgItem(UNITPARENT(hDlg), IDC_PREF_UNITS))
	hDlg = UNITPARENT(hDlg);
return(Dialog_OnCtlColor(hDlg, hdc, hWndChild, type));
}

/************************************************************************/
LOCAL void Rectangular_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int idUnits, SaveUnits;

switch (id)
	{
	case IDC_SQUARE:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_MASKMODE_REVERSE:
	case IDC_MASKMODE_ADD:
	case IDC_MASKMODE_SUBTRACT:
	Mask.iShapeModes = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
	CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
		id );
	SetCommonMode(Mask.iShapeModes);
	break;

	case IDC_ASPECTRATIO:
//	case IDC_UNCONSTRAIN:
//	case IDC_CONSTRAIN:
//	case IDC_FIXEDSIZE;
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.ConstrainRectAspect = id;
	SetCommonConstrain(Mask.ConstrainRectAspect);
	SEND_WM_CONTROLENABLE(hDlg, 0);
	break;

	case IDC_MASKSHAPE:
//	case IDC_SQUAREMASK:
//	case IDC_CIRCLEMASK:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.DoMaskCircle = id - IDC_SQUAREMASK;
	SetCommonDoCircle(Mask.DoMaskCircle);
	break;

	case IDC_HCONSTRAIN:
	hDlg = SIZEPARENT(hDlg);
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	SaveUnits = Control.Units;
	Control.Units = Mask.ShapeUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	Mask.ConstrainRectHeight = AstralGetDlgItemFixed( hDlg,
		id, &Bool, NO );
	Control.Units = SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetCommonHeight(Mask.ConstrainRectHeight);
	break;

	case IDC_WCONSTRAIN:
	hDlg = SIZEPARENT(hDlg);
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	SaveUnits = Control.Units;
	Control.Units = Mask.ShapeUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	Mask.ConstrainRectWidth = AstralGetDlgItemFixed( hDlg,
		id, &Bool, NO );
	Control.Units = SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetCommonWidth(Mask.ConstrainRectWidth);
	break;

	case IDC_PREF_UNITS:
	if ( !(idUnits = HandleCombo( UNITPARENT(hDlg), id, codeNotify )) )
		break;
	Mask.ShapeUnits = idUnits;
	SaveUnits = Control.Units;
	Control.Units = Mask.ShapeUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_HCONSTRAIN,
		Mask.ConstrainRectHeight, NO);
	AstralSetDlgItemFixed( SIZEPARENT(hDlg), IDC_WCONSTRAIN,
		Mask.ConstrainRectWidth, NO);
	Control.Units = SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetCommonUnits(Mask.ShapeUnits);
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Free_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Free_OnDocActivated(HWND hDlg);
LOCAL void Free_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Free_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Free_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
LOCAL void Free_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);
LOCAL void Free_HandleEnable(HWND hDlg, int fFlags, BOOL fControlEnable);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFreeProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Free_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Free_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Free_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Free_OnControlEnable);

	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Free_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Free_OnDrawItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FREE);
	}
}

/************************************************************************/
LOCAL BOOL Free_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
int id;
HWND hWnd;

bFreeBezier = FALSE;
hWnd = AUTOMASKWND(hDlg);
Mask.PointEdit = FALSE;
//id = (Mask.PointEdit ? IDC_POINT_WND: (Mask.AutoMask ? IDC_AUTOMASK_WND:IDC_FREEHAND_WND));
//ShowExtra(hDlg, IDC_FIRSTFREE_WND, IDC_LASTFREE_WND, id);
ShowPane(hDlg, IDC_PANE0, IDC_PANE1, Mask.AutoMask && !Mask.PointEdit);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2, Mask.PointEdit);
ShowPane(hDlg, IDC_PANE2, IDC_PANE3, !Mask.AutoMask && !Mask.PointEdit);
SetPointerFn(POINTWND(hDlg), NO);
CheckComboItem( hWnd, IDC_COLORMODEL, IDC_FIRST_MODEL,
	IDC_LAST_MODEL, Mask.idAutoColorModel + IDC_FIRST_MODEL);
InitDlgItemSpin( hWnd, IDC_AUTOSEARCHDEPTH, Mask.AutoSearchDepth, NO,
	1, 5 );
InitDlgItemSpin( hWnd, IDC_AUTOMINEDGE, Mask.AutoMinEdge, NO,
	1, 50 );
id = Mask.PointEdit ? IDC_POINTMODE:(Mask.AutoMask ? IDC_AUTOMASK : IDC_FREEHAND);
CheckComboItem( hDlg, IDC_MASKMETHOD, IDC_MASKMETHODFIRST, IDC_MASKMETHODLAST, id);
CheckRadioButton( hDlg, IDC_MASKMETHODFIRST, IDC_MASKMETHODLAST, id );
CheckDlgButton( hDlg, IDC_ANTIALIAS, Mask.FreeAntialias );
Free_HandleEnable(hDlg, 0, YES);
return(FALSE);
}

/************************************************************************/
LOCAL void Free_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Free_HandleEnable(hDlg, 0, NO);
}

/************************************************************************/
LOCAL void Free_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Free_HandleEnable(hDlg, fFlags, YES);
}

/************************************************************************/
LOCAL void Free_HandleEnable(HWND hDlg, int fFlags, BOOL fControlEnable)
/************************************************************************/
{
BOOL	 Bool;
int id;
FRMDATATYPE type = FDT_RGBCOLOR;
HWND hWnd;
LPIMAGE lpImage;

lpImage = GetActiveImage();
ImgGetInfo(lpImage,NULL,NULL, NULL, &type);

if (fControlEnable)
	{
	if (fFlags == -1)
		{
		id = Mask.PointEdit ? IDC_POINTMODE:(Mask.AutoMask ? IDC_AUTOMASK : IDC_FREEHAND);
		CheckComboItem( hDlg, IDC_MASKMETHOD, IDC_MASKMETHODFIRST, IDC_MASKMETHODLAST, id);
		CheckRadioButton( hDlg, IDC_MASKMETHODFIRST, IDC_MASKMETHODLAST, id );
//		id = (Mask.PointEdit ? IDC_POINT_WND: (Mask.AutoMask ? IDC_AUTOMASK_WND:IDC_FREEHAND_WND));
//		ShowExtra(hDlg, IDC_FIRSTFREE_WND, IDC_LASTFREE_WND, id);
		ShowPane(hDlg, IDC_PANE0, IDC_PANE1, Mask.AutoMask && !Mask.PointEdit);
		ShowPane(hDlg, IDC_PANE1, IDC_PANE2, Mask.PointEdit);
		ShowPane(hDlg, IDC_PANE2, IDC_PANE3, !Mask.AutoMask && !Mask.PointEdit);
		}
	SetPointerDlgItems(POINTWND(hDlg));
}


hWnd = AUTOMASKWND(hDlg);
ControlEnable( hWnd, IDC_AUTOSEARCHDEPTH, Mask.AutoMask );
ControlEnable( hWnd, IDC_AUTOMINEDGE, Mask.AutoMask );
Bool = Mask.AutoMask;
if (!ISCOLOR(type))
	Bool = FALSE;

ControlEnableGroup( hWnd, IDC_FIRST_MODEL, IDC_LAST_MODEL, Bool);
ControlEnable( hWnd, IDC_COLORMODEL, Bool);
ControlEnable( hWnd, IDC_IS_POINT , Mask.PointEdit);
ControlEnable( hWnd, IDC_IS_BEZIER, Mask.PointEdit);
if (fControlEnable && fFlags == 1)
	return;
if (!ImgGetMask(lpImage) && Mask.iFreeModes == SHAPE_SUBTRACT)
	Mask.iFreeModes = SHAPE_DEFAULT;
CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
	IDC_MASKMODE_REVERSE + max(Mask.iFreeModes - SHAPE_REVERSE, 0) );

id = bFreeBezier ? IDC_DRAW_BEZIER:IDC_DRAW_LINE;
CheckRadioButton( hDlg, IDC_DRAW_BEZIER,  IDC_DRAW_LINE, id);
ControlEnable( hWnd, IDC_DRAW_LINE ,  !Mask.PointEdit && !Mask.AutoMask);
ControlEnable( hWnd, IDC_DRAW_BEZIER, !Mask.PointEdit && !Mask.AutoMask);

ControlEnable( hDlg, IDC_ANTIALIAS, CANDOTRANSPARENCY(type) );
}

/************************************************************************/
LOCAL void Free_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
if (lpMeasureItem->CtlID != IDC_MASKMETHOD)
	hDlg = AUTOMASKWND(hDlg);
MeasureItem( hDlg, lpMeasureItem, NO );
}

/************************************************************************/
LOCAL void Free_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
if (lpDrawItem->CtlID != IDC_MASKMETHOD)
	hDlg = AUTOMASKWND(hDlg);
DrawItem( hDlg, lpDrawItem, 0 );
}

/************************************************************************/
LOCAL void Free_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	BOOL	 Bool;
	int newid;
	LPIMAGE lpImage;

	lpImage = GetActiveImage();

	switch (id)
		{
		case IDC_FREE:
			if ( !SaveToolPreferences( id ) )
				break;
		break;
		case IDC_MOVEPOINTS:
		case IDC_ADDPOINTS:
		case IDC_DELPOINTS:
		case IDC_IS_POINT:
		case IDC_IS_BEZIER:	
			HandlePointerFn(POINTWND(hDlg), id, codeNotify);
		break;
		case IDC_MASKMODE_REVERSE:
		case IDC_MASKMODE_ADD:
		case IDC_MASKMODE_SUBTRACT:
			Mask.iFreeModes = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
			CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE,
							IDC_MASKMODE_SUBTRACT, id );
			SetCommonMode(Mask.iFreeModes);
		break;
		
		case IDC_ANTIALIAS:
			Mask.FreeAntialias = !Mask.FreeAntialias;
			CheckDlgButton( hDlg, id, Mask.FreeAntialias );
		break;

		case IDC_DRAW_LINE:
		case IDC_DRAW_BEZIER:
			bFreeBezier = (id ==IDC_DRAW_BEZIER);
			CheckRadioButton( hDlg, IDC_DRAW_BEZIER,  IDC_DRAW_LINE, id);
			// allow Free proc to deal with the change
			MsgProc( PictPubApp.Get_hActiveWindow(), WM_CONTROLENABLE, 0, 0, 0);
		break;
		case IDC_MASKMETHOD:
			if ( !(newid = HandleCombo( hDlg, id, codeNotify )) )
				break;
			// fall through
		case IDC_FREEHAND:
		case IDC_AUTOMASK:
		case IDC_POINTMODE:
			if (id == IDC_MASKMETHOD)
				id = newid;
			else
				CheckComboItem( hDlg, IDC_MASKMETHOD, IDC_MASKMETHODFIRST, IDC_MASKMETHODLAST, id);
			CheckRadioButton( hDlg, IDC_MASKMETHODFIRST, IDC_MASKMETHODLAST, id );
			
			// check the item for the r-button menu stuff
			Mask.AutoMask = (id == IDC_AUTOMASK);
			Mask.PointEdit = (id == IDC_POINTMODE);
			// Send this to FreeProc to make sure it resets bLast
			if ( Tool.lpToolProc )
				MsgProc( PictPubApp.Get_hActiveWindow(), WM_CONTROLENABLE, 0, 0, 0);
			// activate controls
			Free_HandleEnable(hDlg, -1, YES);
		break;
		
		case IDC_COLORMODEL:
	//	case IDC_MODEL_RGB:
	//	case IDC_MODEL_HSL:
	//	case IDC_MODEL_LIGHTNESS:
	//	case IDC_MODEL_HUE:
			if ( !(id = HandleCombo(AUTOMASKWND(hDlg), id, codeNotify )) )
				break;
			Mask.idAutoColorModel = id - IDC_FIRST_MODEL;
			SetCommonColorModel(Mask.idAutoColorModel);
		break;
		
		case IDC_AUTOSEARCHDEPTH:
			if ( GetFocus() != GetDlgItem( AUTOMASKWND(hDlg), id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Mask.AutoSearchDepth = GetDlgItemSpin(AUTOMASKWND(hDlg),IDC_AUTOSEARCHDEPTH,
				&Bool, NO );
		break;
		
		case IDC_AUTOMINEDGE:
			if ( GetFocus() != GetDlgItem( AUTOMASKWND(hDlg), id ) )
				break;														  
			if ( codeNotify != EN_CHANGE )
				break;
			Mask.AutoMinEdge = GetDlgItemSpin( AUTOMASKWND(hDlg), IDC_AUTOMINEDGE,
				&Bool, NO );
		break;
	   default:
		break;
	   }
}


// LOCAL prototypes
LOCAL BOOL Magic_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Magic_OnDocActivated(HWND hDlg);
LOCAL void Magic_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Magic_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgMagicProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Magic_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Magic_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Magic_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Magic_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MAGICWAND);
	}
}

/************************************************************************/
LOCAL BOOL Magic_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
InitDlgItemSpin( hDlg, IDC_WANDRANGE, Mask.WandRange, NO, 0, 100 );
InitDlgItemSpin( hDlg, IDC_WANDFADE, Mask.WandFade, NO, 0, 100 );
CheckComboItem( hDlg, IDC_COLORMODEL, IDC_FIRST_MODEL,
	IDC_LAST_MODEL, Mask.idMagicColorModel + IDC_FIRST_MODEL);
InitDlgItemSpin( hDlg, IDC_GROW_SPIN, Mask.GrowAmount, NO, 0, 99 );

Magic_OnDocActivated(hDlg);
return(FALSE);
}

/************************************************************************/
LOCAL void Magic_OnDocActivated(HWND hDlg)
/************************************************************************/
{
fGotMagic = FALSE;
Magic_OnControlEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Magic_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
BOOL bGray, bLa, bPal;
FRMTYPEINFO TypeInfo;
LPIMAGE lpImage = GetActiveImage();

ImgGetTypeInfo(lpImage, &TypeInfo);

bGray  = (TypeInfo.DataType == FDT_GRAYSCALE);
bLa  = (TypeInfo.DataType == FDT_LINEART);
bPal  = (TypeInfo.DataType == FDT_PALETTECOLOR);

ControlEnableGroup( hDlg, IDC_FIRST_MODEL, IDC_LAST_MODEL, !(bLa || bGray));
ControlEnable( hDlg, IDC_COLORMODEL, !(bLa || bGray));
if (!ImgGetMask(lpImage) && Mask.iWandModes == SHAPE_SUBTRACT)
	Mask.iWandModes = SHAPE_DEFAULT;
CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
	IDC_MASKMODE_REVERSE + max(Mask.iWandModes - SHAPE_REVERSE, 0) );

ControlEnable( hDlg, IDC_GROW_SPIN, fGotMagic && !bLa);
ControlEnable( hDlg, IDC_GROW, fGotMagic && !bLa);
ControlEnable( hDlg, IDC_SIMILAR, fGotMagic);

ControlEnable( hDlg, IDC_WANDFADE, !(bLa || bPal));
ControlEnable( hDlg, IDC_WANDRANGE, !bLa);
}

/************************************************************************/
LOCAL void Magic_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
static int Range;
//RECT rUpdate;

switch (id)
	{
	case IDC_MAGICWAND:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_MASKMODE_REVERSE:
	case IDC_MASKMODE_ADD:
	case IDC_MASKMODE_SUBTRACT:
	Mask.iWandModes = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
	CheckRadioButton( hDlg, IDC_MASKMODE_REVERSE, IDC_MASKMODE_SUBTRACT,
		id );
	SetCommonMode(Mask.iWandModes);
	break;

	case IDC_COLORMODEL:
//	case IDC_MODEL_RGB:
//	case IDC_MODEL_HSL:
//	case IDC_MODEL_LIGHTNESS:
//	case IDC_MODEL_HUE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.idMagicColorModel = id - IDC_FIRST_MODEL;
	SetCommonColorModel(Mask.idMagicColorModel);
	break;

	case IDC_GROW_SPIN:
		Mask.GrowAmount = GetDlgItemSpin(hDlg, IDC_GROW_SPIN, &Bool, NO);
	break;
	case IDC_GROW:
		if (!fGotMagic)
			break;
		Range = Mask.GrowAmount;
		if (Range <= 0)
			break;
		if (!IsInFile(GetActiveDoc(), magicX, magicY))
			break;
		Range = bound(Range+magicRange, 0, 100);

		SetDlgItemSpin(hDlg,IDC_WANDRANGE, Range, NO);
		magicRange = Mask.WandRange = GetDlgItemSpin( hDlg, IDC_WANDRANGE, &Bool, NO );

		if (fUseSimilar)
			ProcessSimilar(GetActiveImage());
		else
			ProcessMagicMask(GetActiveImage());
	break;
	
	case IDC_SIMILAR:
		if (!fGotMagic)
			break;
		if (!IsInFile(GetActiveDoc(), magicX, magicY))
			break;
		magicRange = bound(magicRange, 0, 100);
		if (magicRange <= 0)
			break;
		ProcessSimilar(GetActiveImage());
		fUseSimilar = TRUE;
	break;
	
	case IDC_WANDRANGE:
	Mask.WandRange = GetDlgItemSpin( hDlg, id, &Bool, NO );
	SetCommonRange(Mask.WandRange);
	break;

	case IDC_WANDFADE:
	Mask.WandFade = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

   default:
	break;
   }
}

/************************************************************************/
void SquareProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy, pix, lin, res, XSize, YSize;
LPFRAME lpFrame;
POINT pt;
static int Style;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWindow);

switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	fConstrain = Mask.ConstrainRectAspect == IDC_CONSTRAIN;
	AspectX = FMUL (100, Mask.ConstrainRectWidth);
	AspectY = FMUL (100, Mask.ConstrainRectHeight);
	if (fConstrain)
		{
      double Max = AspectX > AspectY? AspectX : AspectY;
      AspectX = (long)((AspectX / Max) * 1000.0);
      AspectY = (long)((AspectY / Max) * 1000.0);
      }
	fShift = NO;
	Style = Mask.DoMaskCircle ? SL_ELLIPSE : SL_BOX;

	if (Mask.ConstrainRectAspect == IDC_FIXEDSIZE)
		{
		lpFrame = ImgGetBaseEditFrame(lpImage);
		res = FrameResolution(lpFrame);
		pix = FMUL( res, Mask.ConstrainRectWidth );
		lin = FMUL( res, Mask.ConstrainRectHeight );
		if (!pix || !lin)
			{
			Tool.bActive = NO;
			break;
			}
		}

	AstralSetRectEmpty( &SelectRect );
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, Style | SL_SPECIAL,
		pt, 0L );

	if (Mask.ConstrainRectAspect == IDC_FIXEDSIZE)
		{
		InvertSelection(hWindow, NULL, &SelectRect, Style);
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
		InvertSelection(hWindow, NULL, &SelectRect, Style);
		}
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, Style, YES );
	pix = RectWidth(&SelectRect);
	lin = RectHeight(&SelectRect);
	if (pix && lin && (pix > 1 || lin > 1))
		{
		SHAPEMASK_PARMS parms;
		int res;

		res = FrameResolution(ImgGetBaseEditFrame(lpImage));

		parms.Shape = Mask.DoMaskCircle;
		parms.Mode = Mask.iShapeModes;
		parms.iBaseRes = res;
		parms.x1 = SelectRect.left;
		parms.y1 = SelectRect.top;
		parms.x2 = SelectRect.right;
		parms.y2 = SelectRect.bottom;
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_SHAPEMASK, &parms);
		}
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	fShift = SHIFT;
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, Style,
		pt, fConstrain^CONSTRAINASPECT, AspectX, AspectY,
		MOVEKEY||Window.fRButtonDown||(Mask.ConstrainRectAspect == IDC_FIXEDSIZE),
		FROMCENTER);
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, Style, YES );
	break;
	}
}

#define DOFREEBEZIER \
	(bFreeBezier && !(Mask.AutoMask && !Mask.PointEdit))

/************************************************************************/
void FreeProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
	WORD wKey;
	int function;
	LPCPoly lpPoly;
	BOOL bAutoMask;
	static BOOL bLastMode;
	static BOOL bLastFreeBezier;
	
	
	// Need to change modes?
	if (Tool.bActive && bLastMode != Mask.PointEdit)
	{  
		bLastMode = Mask.PointEdit;
		ChangeFreeMode(hWindow, Mask.PointEdit, bLastFreeBezier);
		bLastFreeBezier = DOFREEBEZIER;
	}
	// change free bezier mode
	if (Tool.bActive && bLastFreeBezier != DOFREEBEZIER)
	{  
		if (!bLastMode)
		{
				DrawFreeLine( hWindow, 0, gx, gy, OFF, bLastFreeBezier, NO);
				if (DOFREEBEZIER)
					FreeEraseLastBezier(hWindow);
				DrawFreeLine( hWindow, 0, gx, gy, OFF, DOFREEBEZIER);
		}
		bLastFreeBezier = DOFREEBEZIER;
	}
	
	switch (msg)
	{
		case WM_CREATE:	
			// The first mouse down message
			bLastMode = Mask.PointEdit;
			bLastFreeBezier = DOFREEBEZIER;
			if (!IsOnImage(hWindow, x, y))
				break;
			// be sure we have a brush if we need one
			if (ISDRAWTOOL(Tool.id))
				if (!Retouch.hBrush)
					{Message(IDS_ENOBRUSH);break;}
			// setup structures, install hooks etc
			ChangeFreeMode(hWindow, Mask.PointEdit, bLastFreeBezier);
			Tool.bActive = YES;
			// simulate the click
			gx = x;
			gy = y;
			FreeProc(hWindow, WM_LBUTTONDOWN, x, y, Option);
			SendRibbonEnable(1);
		break;

		case WM_ACTIVATE:
			if (!Option)
			{
				if (ISDRAWTOOL(Tool.id))
				{
					if (Retouch.hBrush)
						DestroyMgxBrush(Retouch.hBrush);
					Retouch.hBrush = NULL;
				}
			}
			else
			{	// an activate ( to re-create brush)
				if (ISDRAWTOOL(Tool.id))
					SendRibbonEnable(1);
			}
		break;

		case WM_LBUTTONDOWN:
			if (bLastMode)
			{
				ShapeEdit.MouseDown(x,y);
				SendRibbonEnable(1);
			}
			else //  freehand mode 
			{
				bounded_display2file(hWindow, &x, &y);
				bAutoMask = ( Mask.AutoMask | SHIFT ) && Tool.id == IDC_FREE;
				if ( bAutoMask)
				{
					if (!PolyPos || !lpPolyList)
						break;
					lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos);
					if (!lpPoly)
						break;
					// Undraw the guide line ...
					DrawFreeLine( hWindow, 0, gx, gy, OFF,NO/* bBezier*/);
					automask(hWindow, x, y, lpPoly, Mask.idAutoColorModel, YES);
					// draw the new guide line
					DrawFreeLine( hWindow, 0, gx, gy, ON, NO /*bBezier*/);
				}
				else
					vector_draw( hWindow, x, y, YES, DOFREEBEZIER);
			}
		break;

		case WM_LBUTTONUP:
			if (!bLastMode)
				break;
			ShapeEdit.MouseUp(x,y);
			SendRibbonEnable(1);
		break;

		case WM_MOUSEMOVE:	// sent when ToolActive is on
			if (bLastMode)	  // point mode
			  	ShapeEdit.MouseMove(x,y);
			else // freehand mode
			{
				// cant do free drawing in bezier mode!
				if (DOFREEBEZIER && Window.fLButtonDown)
					break;
				bounded_display2file(hWindow, &x, &y);
				vector_draw( hWindow, x, y, Window.fLButtonDown, DOFREEBEZIER );
			}
		break;
		
		case WM_SETCURSOR:
			if (bLastMode)
			{
				function = ShapeEdit.GetExtendedFunction();
				SetPointerCursor(function, bLeft);
			}
			else if (Tool.Prim.idCursor == ID_FREE && !bLeft)
				SetWindowCursor(ID_FREEALT);
			else
				SetWindowCursor(Tool.Prim.idCursor);
		break;
		
		case WM_KEYDOWN:
			wKey = (WORD)Option;
			if (wKey=='P')
			{
			   	Mask.PointEdit = !Mask.PointEdit;
				// notify myself
				SEND_WM_CONTROLENABLE(hWindow, 0);
				// notify ribbon
				SendRibbonEnable(-1);
				CursorProc(hWindow, 0L);
			}
			if (wKey=='D')
			{
				bLeft = !bLeft;
				CursorProc(hWindow, 0L);
			}
			if (wKey=='B' && !bLastMode && !Mask.AutoMask)
			{
				DrawFreeLine( hWindow, 0, gx, gy, OFF, bFreeBezier, NO);
				bFreeBezier = !bFreeBezier;
				bLastFreeBezier = bFreeBezier;
				// if there is a preceding bezier then erase it
				if (bFreeBezier)
					FreeEraseLastBezier(hWindow);
				DrawFreeLine( hWindow, 0, gx, gy, ON, bFreeBezier);
				SendRibbonEnable(-1);
			}
	
			if (wKey=='O')
			{
				FreeProc(hWindow, WM_COMMAND, x, y, IDM_LOADSHAPE);
				break;
			}
	
			if (wKey=='S')
			{
				free_save(hWindow);	
				break;
			}
			
			if (bLastMode)	// point edit
			{
			  	ShapeEdit.KeyDown(wKey);
				SendRibbonEnable(1);
				CursorProc(hWindow, 0L);
				break;
			}
			if (wKey==VK_BACK)
				MaskFreeBackup(hWindow);
		break;
		
		case WM_KEYUP:
			wKey = (WORD)Option;
			if (bLastMode)	// point edit
			{
				ShapeEdit.KeyUp(wKey);
				SendRibbonEnable(1);
				CursorProc(hWindow, 0L);
			}
		break;

		case WM_DESTROY:	// The cancel operation message
			if (bLastMode)	// point edit
				ChangeFreeMode(hWindow,bLastMode = Mask.PointEdit = NO, bLastFreeBezier); // chane to freehand
			if (Option) /* cancel function */
			{
				Tool.bActive = NO;
				SetDisplayHook(hWindow, NULL);
				/* Undraw the guide line */
				DrawFreeLine( hWindow, 0, gx, gy, OFF, DOFREEBEZIER, NO);
				/* Undraw the unclosed shape */
				MaskUndraw(hWindow, NULL, lpPolyList, NO);
				FreePolyList(lpPolyList);
				lpPolyList = NULL;
				PolyPos = NULL;
				SendRibbonEnable(-1);
				break;
			}
		/* fall through to ending function */
		case WM_LBUTTONDBLCLK:
			if (bLastMode && msg == WM_LBUTTONDBLCLK)	// point edit
				break;
			else if (ISCLOSED(Tool.id) && msg == WM_LBUTTONDBLCLK)
				MaskFreeBackup(hWindow);
			bounded_display2file(hWindow, &x, &y);
			Tool.bActive = NO;
			SetDisplayHook(hWindow, NULL);
			lpPoly = NULL;
			if (PolyPos && !lpPolyList->IsEmpty())
				if (lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos))
					DrawFreeLine( hWindow, 0, gx, gy, OFF, DOFREEBEZIER, NO);
			MaskUndraw(hWindow, NULL, lpPolyList, NO);
			if (ISDRAWTOOL(Tool.id))
				DrawFree(GetImagePtr(hWindow), lpPolyList);
			else
				freemask(hWindow, lpPolyList);
			FreePolyList(lpPolyList);
			lpPolyList = NULL;
			PolyPos = NULL;
			SendRibbonEnable(-1);
		break;

		case WM_COMMAND:
			switch (Option)
			{
				case IDM_LOADSHAPE:
					if (!Tool.bActive)
					{
						// need to activate tool first
						bLastMode = Mask.PointEdit;
						bLastFreeBezier = DOFREEBEZIER;
						// be sure we have a brush if we need one
						if (ISDRAWTOOL(Tool.id))
							if (!Retouch.hBrush)
								{Message(IDS_ENOBRUSH);break;}
						// setup structures, install hooks etc
						ChangeFreeMode(hWindow, Mask.PointEdit, bLastFreeBezier);
					}
					if (!free_load(hWindow))
						break;
					
					if (!Tool.bActive)
						Tool.bActive = YES;
					SendRibbonEnable(1);
				break;
				case IDM_SAVESHAPE:
					free_save(hWindow);	
				break;
				case IDM_DELETEITEM:
					if (!bLastMode)	// freehand mode
						break;
					Tool.bActive = NO;
					SetDisplayHook(hWindow, NULL);
					lpPolyList = ShapeEdit.Done();
					FreePolyList(lpPolyList);
					lpPolyList = NULL;
					PolyPos = NULL;
					UpdateStatusBar( YES, NO, YES, NO );
					SendRibbonEnable(-1);
				break;
			}
		break;

	}
}

//************************************************************************/
//	 Changes modes from freehand to point editing & vice versa
//************************************************************************/
LOCAL void ChangeFreeMode(HWND hWindow, BOOL bPointEdit, BOOL bWasfreeBezier)
//************************************************************************/
{
	RECT maskRect;
	POINT pt;
	BOOL bStarted;
	LPCPoly lpPoly;
	
	if (bPointEdit)	// point editor
	{
		SetDisplayHook(hWindow, NULL);
		bStarted = (lpPolyList && !lpPolyList->IsEmpty());
		if (bStarted)
		{
			// undraw freehand line
			DrawFreeLine( hWindow, 0, gx, gy, OFF, bWasfreeBezier);
			AstralUpdateWindow(hWindow);
			// make ShapeEdit from the freehand so far
			lpPolyList->ForEach((LPLISTPROC)PolySqueeze);
		}
		if (!ShapeEdit.Init(hWindow, lpPolyList)) 
			goto ExitMemory;
		FreePolyList(lpPolyList);
		lpPolyList = NULL;
		PolyPos = NULL;
		lpPoly = NULL;
		//ShapeEdit.CloseShape(NO);
		ShapeEdit.GetBounds(&maskRect);
		File2DispRect(hWindow, &maskRect, &maskRect);
		LPIMAGE lpImage = GetImagePtr(hWindow);
		lpImage->Paint(hWindow, Window.hDC, &maskRect, NULL);
		// lpImage->Paint will set the Clip Rgn
		SelectClipRgn(Window.hDC, NULL);
		ShapeEdit.Draw(Window.hDC, ON);
		SetDisplayHook(hWindow, &ShapeEditDrawHook);
	}
	else // freehand
	{
		SetDisplayHook(hWindow, NULL);
		// undraw any old shape
		ShapeEdit.Draw(NULL, OFF);
		if (!(lpPolyList = ShapeEdit.Done()))
		{
			// create polygonlist to work on
			if (!(lpPolyList = new CPolyList)) goto ExitMemory;
		}
		if (!(PolyPos = lpPolyList->GetHeadPosition()))
		{
			// create polygon to work on
			if (!(lpPoly = new CPoly))  goto ExitMemory;
			if (!(PolyPos = lpPolyList->AddTail(lpPoly)))
			{
				delete(lpPoly);
				goto ExitMemory;
			}
			lpPoly->ClosePoly(NO);
		}
		else
		{
			// setup freehand vars
//			lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos);
//			lpPoly->LastPoint(&pt);
			// re-draw unclosed mask
			MaskUndraw(hWindow, NULL, lpPolyList, YES);
			GetCursorPos(&pt);
			ScreenToClient(hWindow, &pt);
			bounded_display2file(hWindow, &pt.x, &pt.y);
			gx = pt.x;
			gy = pt.y;
//			// Draw a guide line from the last point to the cursor
			if (DOFREEBEZIER)
				FreeEraseLastBezier(hWindow);
			DrawFreeLine(hWindow, 0, gx, gy, ON, DOFREEBEZIER);
		}
		SetDisplayHook(hWindow, &MaskDrawHook);
	}
	return;

ExitMemory:
	if (lpPolyList)
		FreePolyList(lpPolyList);
	lpPolyList = ShapeEdit.Done();
	if (lpPolyList)
		FreePolyList(lpPolyList);
	lpPolyList = NULL;
	PolyPos = NULL;
	Message(IDS_EMEMALLOC);
}



//************************************************************************
//			free_load
// DESCRIPTION:
//			Handles loading new shape into point editor
//************************************************************************
LOCAL BOOL free_load(HWND hWindow)
//************************************************************************
{			
	LPCPoly lpPoly;
	POINT pt;
	LPCPolyList lpTempList;
	BOOL bRet = FALSE;
	
	lpSaveLoadPoly = NULL;
	if (!AstralDlg(NO, PictPubApp.GetResourceHandle(), hWindow, IDD_SHAPELOAD, DlgShapeLoadProc))
		goto Done;
	if (!lpSaveLoadPoly || lpSaveLoadPoly->IsEmpty())
		goto Done;
	// make all polygons closed
	lpSaveLoadPoly->ForEach1((LPLISTPROC1)close_poly_proc, YES);	

	// add it to the existing poly
	if (Mask.PointEdit)
	{
		lpTempList = ShapeEdit.PeekPolyList();
		if (!lpTempList || !lpTempList->GetHeadPtr())
		{
			// open the first shape
			if (lpPoly = (LPCPoly)lpSaveLoadPoly->GetHeadPtr())
				lpPoly->ClosePoly(FALSE);
		}
		bRet = ShapeEdit.AddPolyList(lpSaveLoadPoly);
	}
	else // freehand mode
	{
		// check if we have a 'bogus' polygon
		if (lpPolyList)
		{
			if (lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos))
				if (!lpPoly->NumPoints())
				{
					// kill it and replace with new poly list
					FreePolyList(lpPolyList);
					lpPolyList = NULL;
				}
		}
		if (!lpPolyList)
		{
			// start a new poly
			if (!(PolyPos = lpSaveLoadPoly->GetHeadPosition()))
				goto Done;
			if (!(lpPoly = (LPCPoly)lpSaveLoadPoly->GetAtPtr(PolyPos)))
				goto Done;
			lpPolyList = lpSaveLoadPoly; 
			lpSaveLoadPoly = NULL;
			lpPoly->ClosePoly(FALSE);
			// re-draw unclosed mask
			MaskUndraw(hWindow, NULL, lpPolyList, YES);
			lpPoly->GetPoint(0, &pt);
			lpPoly->PreviousPoint(&pt);
			gx = pt.x;
			gy = pt.y;
			// Draw a guide line from the last point to the cursor
			DrawFreeLine(hWindow, 0, gx, gy, ON, DOFREEBEZIER);
			// setup hook
			SetDisplayHook(hWindow, &MaskDrawHook);
			bRet = TRUE;
		}
		else
		{
			// undraw freehand line
			DrawFreeLine( hWindow, 0, gx, gy, OFF, DOFREEBEZIER);
			// un-draw mask
			MaskUndraw(hWindow, NULL, lpPolyList, NO);
			
			// add the polys
			if (!lpSaveLoadPoly->ForEach((LPLISTPROC)add_to_free_proc))
				goto Done;
			bRet = TRUE;
			// free up poly list (not polys)
			delete lpSaveLoadPoly;
			lpSaveLoadPoly = NULL;
			
			// Draw a guide line from the last point to the cursor
			DrawFreeLine(hWindow, 0, gx, gy, ON, DOFREEBEZIER);
			// re-draw mask
			MaskUndraw(hWindow, NULL, lpPolyList, YES);
		}
	}
	
Done:
	// free up poly list and its polys
	if (lpSaveLoadPoly != NULL)
		FreePolyList(lpSaveLoadPoly);
	lpSaveLoadPoly = NULL;
	return(bRet);
}


//************************************************************************
//	For PolyList.ForEach
//	Adds given poly to end of lpPolyList.
//************************************************************************
LOCAL BOOL add_to_free_proc(LPCPoly lpPoly)
//************************************************************************
{
	return((BOOL)(lpPolyList->AddTail(lpPoly) != NULL));
}

//************************************************************************
//	For PolyList.ForEach1
//	Makes it closed or open.
//************************************************************************
LOCAL BOOL close_poly_proc(LPCPoly lpPoly, LONG bClose)
//************************************************************************
{
	lpPoly->ClosePoly(bClose);
	return(TRUE);
}


//************************************************************************
//			free_save
// DESCRIPTION:
//			Handles saving new shape into point editor
//************************************************************************
LOCAL void free_save(HWND hWnd)
//************************************************************************
{
	if (!Mask.PointEdit)
		DoShapeSaveDlg(hWnd,lpPolyList);
	else
		DoShapeSaveDlg(hWnd,ShapeEdit.PeekPolyList());
}


//************************************************************************
//			DoShapeSaveDlg
// DESCRIPTION:
//			Handles saving goven shape
//************************************************************************
VOID DoShapeSaveDlg(HWND hWnd, LPCPolyList lpList)
//************************************************************************
{
	lpSaveLoadPoly = lpList;
	AstralDlg(NO, PictPubApp.GetResourceHandle(), hWnd, IDD_SHAPESAVE, DlgShapeSaveProc);
}


//************************************************************************
//		DoShapeLoadDlg
// DESCRIPTION:
//		Brings up the shape load dialog.
//		Loads the shape that the user selects.
// RETURNS:
//		A poly list or null.  
//		NOTE: It is up to the caller to free the list and all polygons!!
//************************************************************************
LPCPolyList DoShapeLoadDlg(HWND hWnd)
//************************************************************************
{
	LPCPolyList lpList;
	
	lpSaveLoadPoly = NULL;
	if (!AstralDlg(NO, PictPubApp.GetResourceHandle(), hWnd, IDD_SHAPELOAD, DlgShapeLoadProc))
		return(NULL);
	lpList = lpSaveLoadPoly;
	lpSaveLoadPoly = NULL;
	return(lpList);
}



//************************************************************************/
//************************************************************************/
LOCAL void freemask(HWND hWindow, LPCPolyList lpPolyList)
//************************************************************************/
{
	FREEMASK_PARMS parms;
	int res;
	LPIMAGE lpImage;

	lpImage = GetImagePtr(hWindow);
	if (!lpImage || !lpPolyList || lpPolyList->IsEmpty() || !PolyPos)
		return;
	lpPolyList->ForEach((LPLISTPROC)PolySqueeze);
	ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
	parms.iBaseRes = res;
	parms.lpPoints = PolyList2IntPoints(lpPolyList, &parms.nPoints);
	if (!parms.lpPoints)
		return;
	parms.Mode = Mask.iFreeModes;
	parms.Antialias = Mask.FreeAntialias;
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_FREEMASK, &parms);
}



//************************************************************************/
//************************************************************************/
LOCAL void bounded_display2file(HWND hWindow, LPLONG x, LPLONG y)
//************************************************************************/
{
bounded_display2file( hWindow, ( LPINT )x, ( LPINT )y );
}

//************************************************************************/
//************************************************************************/
LOCAL void bounded_display2file(HWND hWindow, LPINT x, LPINT y)
//************************************************************************/
{
	LPRECT lpRect;
	
	lpRect = &GetDisplayPtr(hWindow)->DispRect;
	*x = bound( *x, lpRect->left, lpRect->right );
	*y = bound( *y, lpRect->top, lpRect->bottom );
	Display2File(hWindow, x, y);
}

// LOCAL prototypes
LOCAL BOOL ShapeLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ShapeLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgShapeLoadProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, ShapeLoad_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, ShapeLoad_OnCommand);

		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_SHAPELOAD);
	}
}


/************************************************************************/
LOCAL BOOL ShapeLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	CenterPopup( hDlg );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Shape, IDN_SHAPE );
	lstrcpy( Names.Saved, Names.Shape );
	Mask.iShapeScale = 100;
	InitDlgItemSpin( hDlg, IDC_SHAPE_SCALE, Mask.iShapeScale, NO,
	1, 1000);
	return(TRUE);
}

/************************************************************************/
LOCAL void ShapeLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	FNAME szFileName;
	int res;
	LPIMAGE lpImage;
	BOOL Bool;
	int i;

	switch (id)
	{
	
		case IDC_SHAPE_SCALE:
			if ( GetFocus() != GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			i = GetDlgItemSpin(hDlg,id,&Bool, NO );
			if (Bool)
				Mask.iShapeScale = i;
		break;
	
		case IDOK:
		if ( !LookupExtFile(Names.Shape, szFileName, IDN_SHAPE) )
		{
			InitExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Shape, IDN_SHAPE, NO, NO );
			break;
		}
		lpImage = GetActiveImage();
		if (lpImage)
			res = FrameResolution(ImgGetBaseEditFrame(lpImage));
		else
			res = 75;
		res = (res * (long)Mask.iShapeScale)/100;
		lpSaveLoadPoly = LoadShape(szFileName, res);
		if ( !lpSaveLoadPoly)
			break;
		AstralDlgEnd( hDlg, TRUE );
		break;

		case IDCANCEL:
		lstrcpy( Names.Shape, Names.Saved );
		lpSaveLoadPoly = NULL;
		AstralDlgEnd( hDlg, FALSE );
		break;

		case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Shape, IDN_SHAPE, codeNotify );
		break;

		case IDC_EXTMANAGE:
		PopupMenu( hDlg, id, IDC_FILENAME );
		break;

		case IDC_ADDEXT:
		case IDC_DELETEEXT:
		case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Shape, IDN_SHAPE, id, NO );
		break;

	   default:
		break;
	}
}

// LOCAL prototypes
LOCAL BOOL ShapeSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ShapeSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgShapeSaveProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, ShapeSave_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, ShapeSave_OnCommand);

		default:
		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_SHAPESAVE);
	}
}

/************************************************************************/
LOCAL BOOL ShapeSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	CenterPopup( hDlg );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Shape, IDN_SHAPE );
	lstrcpy( Names.Saved, Names.Shape );
	return(TRUE);
}

/************************************************************************/
LOCAL void ShapeSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	FNAME szFileName;
	LFIXED res;
	LPIMAGE lpImage;

	switch (id)
	{
		case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Shape, IDN_SHAPE, codeNotify );
		break;

		case IDC_CLIPEXTMANAGE:
		PopupMenu( hDlg, id, IDC_FILENAME );
		break;

		case IDC_ADDEXT:
		case IDC_DELETEEXT:
		case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Shape,
			IDN_SHAPE, id, NO );
		break;

		case IDOK:
		if ( LookupExtFile(Names.Shape, szFileName, IDN_SHAPE) )
		{
			if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
				(LPTR)Names.Shape) == IDCANCEL)
				break;
		}
		else
		{
			if ( !CreateFileName( Names.Shape, IDN_SHAPE, szFileName ) )
				break;
		}
		lpImage = GetActiveImage();
		if (lpImage)
			res = FrameResolution(ImgGetBaseEditFrame(lpImage));
		else
			res = 75;
		if ( !SaveShape(lpSaveLoadPoly,  szFileName, res) )
			break;
		if ( !AddExtFile( Names.Shape, szFileName, IDN_SHAPE ) )
		{
			FileDelete( szFileName );
			break;
		}
		AstralDlgEnd( hDlg, TRUE );
		break;

		case IDCANCEL:
		lstrcpy( Names.Shape, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	   default:
		break;
	   }
}


/************************************************************************/
void MagicProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
LPIMAGE lpImage;

lpImage = GetImagePtr(hWindow);
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	AstralUpdateWindow(hWindow);

	Display2File(hWindow, &x, &y);
	if ( IsInFile(hWindow, x, y) )
	{
		// setup values for grow & similar
		fGotMagic = TRUE;
		fUseSimilar = FALSE;
	 	magicX=x;
	 	magicY=y;
	 	magicRange=Mask.WandRange;
	 	magicFade=Mask.WandFade;
	 	magicColorModel=Mask.idMagicColorModel;
	 	magiciMode=Mask.iWandModes;
		ProcessMagicMask(lpImage);
		SendRibbonEnable(1);
	}
	break;

	case WM_LBUTTONDOWN:
	break;

	case WM_LBUTTONUP:
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	Tool.bActive = NO;
	break;
	}
}

//************************************************************************
void DrawGuideLine(HWND hWnd, HDC hDC, int x1, int y1, int x2, int y2, BOOL on)
//************************************************************************
{
TFORM TForm;
POINT pt;

TInit(&TForm);
File2DispTFormEx(hWnd, &TForm, ISDRAWTOOL(Tool.id)/*YES*/);
pt.x = x1; pt.y = y1;
Transformer(&TForm, &pt, &x1, &y1);
pt.x = x2; pt.y = y2;
Transformer(&TForm, &pt, &x2, &y2);
//File2DisplayEx(hWnd, &x1, &y1, ISDRAWTOOL(Tool.id)/*YES*/);
//File2DisplayEx(hWnd, &x2, &y2, ISDRAWTOOL(Tool.id)/*YES*/);
Dline( hDC, x1, y1, x2, y2, on );
}

//************************************************************************
//	if bWhole then it will update and draw the last segment (if bBezier)
//	assumes lpPolyList and PolyPos are set!
//	changes lpPolyList!
// 	newx & newy are in file coords.
//************************************************************************
LOCAL void DrawFreeLine(HWND hWnd, HDC hDC, int newx, int newy, BOOL on, BOOL bBezier, BOOL bWhole)
//************************************************************************
{
	POINT line[3];
	POINT handles[4];
	POINT bezier[BEZIER_IN];
	BOOL bLeadingBezier;
	LPCPoly lpPoly;
	TFORM TForm;
	POINT pt;
	
	line[1].x = line[2].x = newx;
	line[1].y = line[2].y = newy;
	// get polygon
	if (!PolyPos || !lpPolyList)
		return;
	lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos);
	if (!lpPoly)
		return;
	// get last point
	if (!lpPoly->LastPoint(&line[1]))
		goto DrawLine;
	// if not bezier then go ahead and draw
	if (!bBezier)
		goto DrawLine;
	// get next to last real point
	if (!PreviousControlPoint(lpPoly, &line[0]))
		goto DrawLine;
	bLeadingBezier = (lpPoly->IsCurveEndPoint()	& POLY_CURVE_BEGIN);
	// compute bezier handles
	ComputeBezierHandles(line, &handles[0], &handles[2]);
	if (bLeadingBezier && bWhole)
	{
		// change existing bezier
		bezier[0] = line[0];
		lpPoly->NextPoint(&bezier[1]);
		lpPoly->NextPoint();
		lpPoly->SetCurrentPoint(&handles[1]);
		bezier[2] = handles[1];
		bezier[3] = line[1];
		// draw first curve
		MaskDrawBezier(hWnd, hDC, bezier, on); 
	}	
	// draw last line
	if (!PtEq(line[1], line[2]))
	{
		bezier[0] = line[1];
		bezier[1] = handles[2];
		bezier[2] = handles[3];
		bezier[3] = line[2];
		MaskDrawBezier(hWnd, hDC, bezier, on); 
	}
	return;
	
DrawLine:
	TInit(&TForm);
	File2DispTFormEx(hWnd, &TForm, ISDRAWTOOL(Tool.id)/*YES*/);
	pt.x = line[1].x; pt.y = line[1].y;
	Transformer(&TForm, &pt, &line[1].x, &line[1].y);
	pt.x = line[2].x; pt.y = line[2].y;
	Transformer(&TForm, &pt, &line[2].x, &line[2].y);
//	File2DisplayEx(hWnd, &line[1].x, &line[1].y, ISDRAWTOOL(Tool.id)/*YES*/);
//	File2DisplayEx(hWnd, &line[2].x, &line[2].y, ISDRAWTOOL(Tool.id)/*YES*/);
	Dline(hDC, line[1].x, line[1].y, line[2].x, line[2].y, on );
	return;
}

/************************************************************************/
void CShapeEditDrawHook2::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect)
/************************************************************************/
{
	ShapeEdit.Draw(hDC, NO);
}

/************************************************************************/
void CMaskDrawHook::DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect)
/************************************************************************/
{
	// draw the unclosed shape
	MaskUndraw( hWnd, hDC, lpPolyList, ON);
	// draw the guide line (only the last part)
	DrawFreeLine( hWnd, hDC, gx, gy, ON, DOFREEBEZIER, NO);
}

/************************************************************************/
// x & y in file coords.
/************************************************************************/
LOCAL void vector_draw( HWND hWnd, int x, int y, BOOL bCapture, BOOL bBezier)
/************************************************************************/
{
	POINT pt;
	LPCPoly lpPoly;
	POINT handles[4];
	POINT line[3];
	
	if ( !bCapture )
	{
		/* if the cursor didn't move... */
		if ( x == gx && y == gy )
			return;
		/* Undraw the guide line ... */
		DrawFreeLine( hWnd, 0, gx, gy, OFF, bBezier);
		/* .. and draw a new one */
		gx = x; gy = y;
		goto Done;
	}
	
	// Undraw the guide line ...
	DrawFreeLine( hWnd, 0, gx, gy, OFF, bBezier);
	gx = x; gy = y;
	// draw the new guide line
	DrawFreeLine( hWnd, 0, gx, gy, ON, bBezier);
	pt.x = x;
	pt.y = y;
	lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos);
	if (!lpPoly)
		return;
	if (!bBezier)
	{
		lpPoly->AddPoint(&pt);
	}
	else
	{
		// cumpute bezier
		line[2] = pt;
		if (!lpPoly->LastPoint(&line[1]))
		{
			// no points
			lpPoly->AddPoint(&pt);
			goto Done;
		}
		if (!PreviousControlPoint(lpPoly, &line[0]))
		{
			// one point
			// un draw the new guide line (again)
			DrawFreeLine( hWnd, 0, gx, gy, ON, bBezier);
			SplitLine(&line[1], &line[2], &handles[2], &handles[3]);
			lpPoly->AddBezierLine(&handles[2], &handles[3], &line[2]);
			// draw the new guide line (again)
			DrawFreeLine( hWnd, 0, gx, gy, ON, bBezier);
			return;
		}
		ComputeBezierHandles(line, &handles[0], &handles[2]);
		// add bezier
		lpPoly->AddBezierLine(&handles[2], &handles[3], &line[2]);
	}
	return;
Done:
	// draw the new guide line
	DrawFreeLine( hWnd, 0, gx, gy, ON, bBezier);
}


//************************************************************************
//	hDC can be NULL
//************************************************************************
LOCAL void MaskUndraw( HWND hWnd, HDC hDC, LPCPolyList lpPolyList, BOOL fOn)
//************************************************************************
{
	int count, idone, total;
	LPPOINT lpPoint;
	TFORM TForm;
	LPCPoly lpPoly;
	POSITION pos;
	BOOL bOldClip = FALSE;
	
	if (!lpPolyList || lpPolyList->IsEmpty())
		return;
	// get tform to convert to display  
	TInit(&TForm);
	File2DispTFormEx(hWnd, &TForm, ISDRAWTOOL(Tool.id)/*YES*/);
	pos = lpPolyList->GetHeadPosition();
	// be sure we have a hDC to save time
	if (!hDC)
	{
		hDC = Window.hDC;
		bOldClip = TRUE;
		if (GetActiveDoc())
			SelectClipRect(hDC, &GetActiveDisplay()->DispRect, NULL);
	}
	// go through every shape
	while (lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos))
	{
		lpPoly->GetPoint(-1);
		total = lpPoly->NumPoints();
		while(total > 0)
		{
			lpPoint = (LPPOINT)LineBuffer[0];
			// get the data
			idone = 16384; 	// storage size
			lpPoly->GetData(-1, &TForm, &count, lpPoint, &idone);
			total -= idone;	// data points used
			if (!count)
				break;
			// draw the data
			if (count == 1)
			{
				Dline(hDC, lpPoint->x, lpPoint->y, lpPoint->x, lpPoint->y, fOn);
				break;
			}	
			while(--count)
			{
				Dline(hDC, lpPoint->x, lpPoint->y, lpPoint[1].x, lpPoint[1].y, fOn);
				lpPoint++;
			}	// draw points
		}	// draw poly 
	} // get each poly
	if (bOldClip)
		SelectClipRgn(hDC, NULL);
}

//************************************************************************
//		ProcessMagicMask
// DESCRIPTION:
//		Uses global vars to fill MAGICMASK_PARMS & sends a process.
//************************************************************************
LOCAL void ProcessMagicMask(LPIMAGE lpImage)
//************************************************************************
{
		MAGICMASK_PARMS parms;
		int res;
		RECT rArea;
		
		res = FrameResolution(ImgGetBaseEditFrame(lpImage));
		parms.iBaseRes = res;
		parms.x = magicX;
		parms.y = magicY;
		parms.Mode = magiciMode;
		parms.WandRange = magicRange;
		parms.WandFade = magicFade;
		parms.idMagicColorModel = magicColorModel;
		rArea.left = rArea.right = magicX;
		rArea.top = rArea.bottom = magicY;
		Measure(lpImage, &rArea, &parms.Color);
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_MAGICMASK, &parms);
}

//************************************************************************
//		ProcessSimilar
// DESCRIPTION:
//		Uses global vars to fill SIMILAR_PARMS & sends a process.
//************************************************************************
LOCAL void ProcessSimilar(LPIMAGE lpImage)
//************************************************************************
{
		SIMILAR_PARMS parms;
		int res;
		RECT rArea;
		
		res = FrameResolution(ImgGetBaseEditFrame(lpImage));
		parms.iBaseRes = res;
		parms.x = magicX;
		parms.y = magicY;
		parms.Mode = magiciMode;
		parms.WandRange = magicRange;
		parms.WandFade = magicFade;
		parms.idMagicColorModel = magicColorModel;
		rArea.left = rArea.right = magicX;
		rArea.top = rArea.bottom = magicY;
		Measure(lpImage, &rArea, &parms.Color);
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_SIMILAR, &parms);
}

//************************************************************************
// For frehand stuff.
// If the current polygon ends in a bezier then erase it.
//************************************************************************
LOCAL void FreeEraseLastBezier(HWND hWindow)
//************************************************************************
{
	POINT bezier[BEZIER_IN];
	LPCPoly lpPoly;
	
	lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos);
	if (!lpPoly)
		return;
	lpPoly->LastPoint(&bezier[3]);
	if (lpPoly->IsCurveEndPoint() == POLY_CURVE_END)
	{
		PreviousControlPoint(lpPoly);
		lpPoly->NextPoint(&bezier[1]);
		lpPoly->NextPoint(&bezier[2]);
		PreviousControlPoint(lpPoly, &bezier[0]);
		MaskDrawBezier(hWindow, NULL, bezier, OFF); 
	}
}

//************************************************************************
// bezier should be in file coords!
//************************************************************************
LOCAL void MaskDrawBezier(HWND hWnd, HDC hDC, LPPOINT lpBezier, BOOL fOn)
//************************************************************************
{
	LPPOINT lpPoint;
	POINT bezier[BEZIER_IN];
	TFORM TForm;
	BOOL bOldClip = FALSE;
	int count;
	
	TInit(&TForm);
	File2DispTFormEx(hWnd, &TForm, ISDRAWTOOL(Tool.id)/*NO*/);
	if (!hDC)
	{
		hDC = Window.hDC;
		bOldClip = TRUE;
		if (GetActiveDoc())
			SelectClipRect(hDC, &GetActiveDisplay()->DispRect, NULL);
	}
	for (count=0; count<BEZIER_IN;count++)
		Transformer(&TForm, &lpBezier[count], &bezier[count].x, &bezier[count].y);
		
	
	lpPoint = (LPPOINT)LineBuffer[0];
	count = CurveToPoints(bezier, BEZIER_MARKER, lpPoint);
	
	if (count == 1)
		Dline(hDC, lpPoint->x, lpPoint->y, lpPoint->x, lpPoint->y, fOn);
	else
	{
		while(--count)
		{
			Dline(hDC, lpPoint->x, lpPoint->y, lpPoint[1].x, lpPoint[1].y, fOn);
			lpPoint++;
		}
	}

	if (bOldClip)
		SelectClipRgn(hDC, NULL);
}

//************************************************************************
LOCAL void MaskFreeBackup(HWND hWnd)
//************************************************************************
{
	int num, x2, y2;
	POINT pt;
	LPCPoly lpPoly;
	POINT bezier[BEZIER_IN];
	BOOL bUndrawBezier;
	
	if (!Tool.bActive)
		return;
	
	lpPoly = (LPCPoly)lpPolyList->GetAtPtr(PolyPos);
	if (!lpPoly)
		return;
	num=lpPoly->NumPoints();
	if (num < 1)
		return;
		
	// dont delete last point/bezier
	if (num <= 1)
		return;
	/* Undraw the guide line*/
	DrawFreeLine( hWnd, 0, gx, gy, OFF, DOFREEBEZIER, NO);
	lpPoly->LastPoint();
	bUndrawBezier = (lpPoly->IsCurveEndPoint() == POLY_CURVE_END);
	// erase last point
	/* Undraw the vector */
	// if it is a bezier then need to make it a line before deleting
	if (bUndrawBezier)
	{
	   	// undraw & remove last bezier
		FreeEraseLastBezier(hWnd);
		lpPoly->LastPoint();
		PreviousControlPoint(lpPoly);
		lpPoly->BezierToLine();
	}
	lpPoly->LastPoint(&pt);
	lpPoly->DeletePoint();
	x2 = pt.x; y2 = pt.y;
	if (!bUndrawBezier)
	{
		// preceding line was not curve,
		// undraw the line 
		DrawFreeLine( hWnd, 0, x2, y2, OFF, bUndrawBezier, NO);
		// so just draw new guide line
	}
	// if we are in bezier draw mode then undraw the preceding bezier also
	if (DOFREEBEZIER)
		FreeEraseLastBezier(hWnd);
	/* Draw the new guide line */
	DrawFreeLine( hWnd, 0, gx, gy, ON, DOFREEBEZIER);
}


//************************************************************************
// 	Uses brush engine & line engine to draw the current shape.
//	Must be in freehand (not point edit) node
//************************************************************************
static void DrawFree(LPIMAGE lpImage, LPCPolyList lpList)
//************************************************************************
{
	DRAWFREE_PARMS parms;
	int res;
	FRMDATATYPE type;
	
	ImgGetInfoEx(lpImage, NULL, NULL, NULL, &type, &res);
	
	if (Draw.FillInterior && Draw.DrawExterior)
		parms.FillStyle = FS_BORDERFILL;
	else
	if (Draw.FillInterior)
		parms.FillStyle = FS_FILL;
	else
		parms.FillStyle = FS_BORDER;
	
	GetDrawBrushSettings(&parms.Brush);
	GetActiveColorFromType(type, &parms.BorderColor);
	GetAlternateColorFromType(type, &parms.FillColor);
	
	ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
	
	parms.iBaseRes = res;
	parms.lpPoints = PolyList2IntPoints(lpPolyList, &parms.nPoints);
	if (!parms.lpPoints)
		return;
	
	if (Tool.id == IDC_DRAWFREE)
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_DRAWFREE, &parms);
	else
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_PENCIL, &parms);
}


//************************************************************************
//	Handles the array of point edit mode buttons.
//	Sets the current mode to mode & updates all mode buttons.
//	If then !Mask.PointEdit all are disabled.
//************************************************************************
void SetPointerDlgItems(HWND hDlg)
//************************************************************************
{
	int function;
	
	if (Mask.PointEdit)
		function = ShapeEdit.GetExtendedFunction();
	else
		function = NO;
	SetPointerFn(hDlg, function);
}

//************************************************************************
//	Handles the array of point edit mode buttons.
//	Sets the current mode to mode & updates all mode buttons.
//	If mode == then all are disabled.
//************************************************************************
void SetPointerFn(HWND hWnd, int mode)
//************************************************************************
{
	ControlEnable(hWnd,IDC_MOVEPOINTS, mode); 
	SendDlgItemMessage(hWnd, IDC_MOVEPOINTS, BM_SETSTATE, 
		mode == IDC_MOVEPOINTS,0L);
	ControlEnable(hWnd,IDC_ADDPOINTS, mode); 
	SendDlgItemMessage(hWnd, IDC_ADDPOINTS, BM_SETSTATE, 
		(mode == IDC_ADDPOINTS) || mode == IDC_ADDBEZIER,0L);
	ControlEnable(hWnd,IDC_DELPOINTS, mode); 
	SendDlgItemMessage(hWnd, IDC_DELPOINTS, BM_SETSTATE, 
		mode == IDC_DELPOINTS,0L);
}

//************************************************************************
//	Handles the array of point edit mode buttons, when one is hit.
//	Returns the new base mode or 0 for no change. 
//************************************************************************
WORD HandlePointerFn(HWND hWnd, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	if (wParam == IDC_IS_POINT)
	{
		ShapeEdit.MakeCorners();
		return(0);
	}
	if (wParam == IDC_IS_BEZIER)
	{
		ShapeEdit.MakeCurves();
		return(0);
	}
	
	ShapeEdit.SetBaseFunction(wParam);
   	SetPointerFn(hWnd, ShapeEdit.GetExtendedFunction());

	return(wParam);
}

//************************************************************************
//	Sets point edit cursor
//************************************************************************
void SetPointerCursor(int PointerFunction, BOOL bLeft)
//************************************************************************
{
	int add;
	
	add = bLeft ? 0:1;
	switch (PointerFunction)
	{
		case IDC_ADDPOINTS:
 			SetWindowCursor(ID_ADDLEFT+add);
		break;
		case IDC_DELPOINTS:
 			SetWindowCursor(ID_DELETELEFT+add);
		break;
		case IDC_MOVEPOINTS:
 			SetWindowCursor(ID_MOVELEFT+add);
		break;
		default:
 			SetWindowCursor(Tool.Alt.idCursor);
		break;
	}
}



//************************************************************************
//	Converts polygon list to fixed point list.
//	lpiPoints is set to the number of points used.
//	Returns the point list or NULL for empty or failure.
//	Note: caller must free the list.
//************************************************************************
LPFPOINT Int2FixedPoints(LPCPolyList lpList, LPINT lpiPoints, int res)
//************************************************************************
{
	LPCPoly lpPoly;
	long num, i;
	long len;
	LPFPOINT lpPoints;
	POINT pt;
	POSITION pos;
	
	*lpiPoints = 0;
	                
	if (!lpList)
		return(NULL);
	        
	num = 0;
	pos = lpList->GetHeadPosition();
	// go through every polygon and detrmine the size needed
	while (lpPoly = (LPCPoly)lpList->GetNextPtr(pos))
	{
		num += lpPoly->NumPoints();
		num++;	// for the new shape marker
	} // get each poly
	if (num < 1)
		return(NULL);
	// adjust for possible beziers
	len = sizeof(FPOINT) *( 2 + (num * 4) / 3);
#ifdef WIN32
	if (len > 65400)
	{
		Message(IDS_ETOOCOMPLEX);
		return(NULL);
	}
#endif
	lpPoints = (LPFPOINT)Alloc(len);
	if (!lpPoints)
	{
		Message(IDS_EMEMALLOC);
		return(NULL);
	}
	i = 0;                
	pos = lpList->GetHeadPosition();
	// go through every polygon and detrmine the size needed
	while (lpPoly = (LPCPoly)lpList->GetNextPtr(pos))
	{
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
		{           
			if (lpPoly->IsCurveEndPoint() & POLY_CURVE_BEGIN)
				lpPoints[i++].fx = FBEZIER_MARKER;
			lpPoints[i].fx = FGET(pt.x, res);
			lpPoints[i].fy = FGET(pt.y, res);
			i++;
		}
		// add end of poly marker
		lpPoints[i++].fx = FNEWPOLY_MARKER;
	} // get each poly
	// back of last new poly marker
	i--;
	*lpiPoints = i;
	return(lpPoints);
}


//************************************************************************
//	Converts fixed point list to polygon list.
//	Returns TRUE if the polygon was converted.
//	Note: caller must free the polygons.
//************************************************************************
int Fixed2IntPoints(LPCPolyList lpOutList, LPFPOINT lpFPoints, int nPoints, int res)
//************************************************************************
{
	int i;
	POINT pt1, pt2, pt3;
	LPCPoly lpPoly;
	
	if (!nPoints)
		return(TRUE);
	
	lpPoly = new CPoly;
	if (!lpPoly)
		goto MemErr;
	if (!lpOutList->AddTail(lpPoly))
	{
		delete(lpPoly);
		goto MemErr;
	}
	i = nPoints;
	while (--i>=0)
	{
		if (lpFPoints->fx == FNEWPOLY_MARKER)
		{
			lpPoly = new CPoly;
			if (!lpPoly)
				goto MemErr;
			if (!lpOutList->AddTail(lpPoly))
			{
				delete(lpPoly);
				goto MemErr;
			}
		}
		else if (lpFPoints->fx == FBEZIER_MARKER)
		{
			++lpFPoints;
			--i;
			pt1.x = FMUL(res, lpFPoints->fx);
			pt1.y = FMUL(res, lpFPoints->fy);
			++lpFPoints;
			--i;
			pt2.x = FMUL(res, lpFPoints->fx);
			pt2.y = FMUL(res, lpFPoints->fy);
			++lpFPoints;
			--i;
			pt3.x = FMUL(res, lpFPoints->fx);
			pt3.y = FMUL(res, lpFPoints->fy);
			if (!lpPoly->AddBezierLine2(&pt1, &pt2, &pt3))
				goto MemErr;
		}
		else 
		{
			pt1.x = FMUL(res, lpFPoints->fx);
			pt1.y = FMUL(res, lpFPoints->fy);
			if (!lpPoly->AddPoint(&pt1))
				goto MemErr;
		}
		++lpFPoints;
	}
	return(TRUE);

MemErr:
	Message(IDS_EMEMALLOC);
	FreePolys(lpOutList);
	return(FALSE);
}


//************************************************************************
//	Converts polygon list to point list.
//	lpiPoints is set to the number of points used.
//	Returns the point list or NULL for empty or failure.
//	Note: caller must free the list.
//************************************************************************
LPPOINT PolyList2IntPoints(LPCPolyList lpList, LPINT lpiPoints)
//************************************************************************
{
	LPCPoly lpPoly;
	long num, i;
	long len;
	LPPOINT lpPoints;
	POINT pt;
	POSITION pos;
	
	*lpiPoints = 0;
	                
	if (!lpList)
		return(NULL);
	        
	num = 0;
	pos = lpList->GetHeadPosition();
	// go through every polygon and detrmine the size needed
	while (lpPoly = (LPCPoly)lpList->GetNextPtr(pos))
	{
		num += lpPoly->NumPoints();
		num++;	// for the new shape marker
	} // get each poly
	if (num < 1)
		return(NULL);
	// adjust for possible beziers
	len = sizeof(POINT) *( 2 + (num * 4) / 3);
#ifdef WIN32
	if (len > 65400)
	{
		Message(IDS_ETOOCOMPLEX);
		return(NULL);
	}
#endif
	lpPoints = (LPPOINT)Alloc(len);
	if (!lpPoints)
	{
		Message(IDS_EMEMALLOC);
		return(NULL);
	}
	i = 0;                
	pos = lpList->GetHeadPosition();
	// go through every polygon and detrmine the size needed
	while (lpPoly = (LPCPoly)lpList->GetNextPtr(pos))
	{
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
		{           
			if (lpPoly->IsCurveEndPoint() & POLY_CURVE_BEGIN)
				lpPoints[i++].x = BEZIER_MARKER;
			lpPoints[i] = pt;
			i++;
		}
		// add end of poly marker
		lpPoints[i++].x = NEWPOLY_MARKER;
	} // get each poly
	// back of last new poly marker
	i--;
	*lpiPoints = i;
	return(lpPoints);
}


//************************************************************************
//	Converts point list to polygon list.
//	Returns TRUE if the polygon was converted.
//	USES UPPER  LEFT RES CONVERSION
//	Note: caller must free the polygons.
//************************************************************************
int IntPoints2PolyList(LPCPolyList lpOutList, LPPOINT lpPoints, int nPoints, int iSrcRes, int iDstRes)
//************************************************************************
{
	int i;
	POINT pt1, pt2, pt3;
	LPCPoly lpPoly;
	
	if (!nPoints)
		return(TRUE);
	
	lpPoly = new CPoly;
	if (!lpPoly)
		goto MemErr;
	if (!lpOutList->AddTail(lpPoly))
	{
		delete(lpPoly);
		goto MemErr;
	}
	i = nPoints;
	while (--i>=0)
	{
		if (lpPoints->x == NEWPOLY_MARKER)
		{
			lpPoly = new CPoly;
			if (!lpPoly)
				goto MemErr;
			if (!lpOutList->AddTail(lpPoly))
			{
				delete(lpPoly);
				goto MemErr;
			}
		}
		else if (lpPoints->x == BEZIER_MARKER)
		{
			--i;
			pt1 = *++lpPoints;
			ResConvertUL(iSrcRes, iDstRes, &pt1.x, &pt1.y);
			--i;
			pt2 = *++lpPoints;
			ResConvertUL(iSrcRes, iDstRes, &pt2.x, &pt2.y);
			--i;
			pt3 = *++lpPoints;
			ResConvertUL(iSrcRes, iDstRes, &pt3.x, &pt3.y);
			if (!lpPoly->AddBezierLine2(&pt1, &pt2, &pt3))
				goto MemErr;
		}
		else 
		{
			pt1 =  *lpPoints;
			ResConvertUL(iSrcRes, iDstRes, &pt1.x, &pt1.y);
			if (!lpPoly->AddPoint(&pt1))
				goto MemErr;
		}
		++lpPoints;
	}
	return(TRUE);

MemErr:
	Message(IDS_EMEMALLOC);
	FreePolys(lpOutList);
	return(FALSE);
}



