//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// LOCAL prototypes
LOCAL BOOL Flood_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Flood_OnDocActivated(HWND hDlg);
LOCAL void Flood_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Flood_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Flood_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFloodProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Flood_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Flood_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Flood_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Flood_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_FLOOD);
	}
}

/************************************************************************/
LOCAL BOOL Flood_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
InitDlgItemSpin( hDlg, IDC_FLOODRANGE, Fill.FloodRange, NO, 0, 100 );
InitDlgItemSpin( hDlg, IDC_FLOODFADE, Fill.FloodFade, NO, 0, 100 );
InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Fill.FloodOpacity), NO,
	0, 99 );
CheckComboItem( hDlg, IDC_COLORMODEL, IDC_FIRST_MODEL,
	IDC_LAST_MODEL, Fill.idFillColorModel + IDC_FIRST_MODEL);
Flood_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Flood_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Flood_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Flood_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Flood_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Flood_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
FRMDATATYPE Type = FDT_RGBCOLOR;

ImgGetInfo(GetActiveImage(), NULL, NULL, NULL, &Type);

HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, Fill.FloodMergeMode, 0L);

ControlEnableGroup( hDlg, IDC_FIRST_MODEL, IDC_LAST_MODEL, ISCOLOR(Type));
ControlEnable( hDlg, IDC_COLORMODEL, ISCOLOR(Type));
ControlEnable( hDlg, IDC_FLOODRANGE, Type != FDT_LINEART);
ControlEnable( hDlg, IDC_TRANSPARENT,  CANDOTRANSPARENCY(Type));
ControlEnable( hDlg, IDC_FLOODFADE, CANDOTRANSPARENCY(Type));
}

/************************************************************************/
LOCAL void Flood_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
MERGE_MODE MergeMode;
int i;

switch (id)
	{
	case IDC_FLOOD:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Fill.FloodOpacity = 255-TOGRAY(i);
	SetCommonOpacity(Fill.FloodOpacity);
	break;

	case IDC_COLORMODEL:
//	case IDC_MODEL_RGB:
//	case IDC_MODEL_HSL:
//	case IDC_MODEL_LIGHTNESS:
//	case IDC_MODEL_HUE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Fill.idFillColorModel = id - IDC_FIRST_MODEL;
	SetCommonColorModel(Fill.idFillColorModel);
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
		break;
	Fill.FloodMergeMode = MergeMode;
	SetCommonMergeMode(Fill.FloodMergeMode);
	break;

	case IDOK:
//	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
//	AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_FLOODRANGE:
	Fill.FloodRange = GetDlgItemSpin( hDlg, id, &Bool, NO );
	SetCommonRange(Fill.FloodRange);
	break;

	case IDC_FLOODFADE:
	Fill.FloodFade = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL TintFill_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TintFill_OnDocActivated(HWND hDlg);
LOCAL void TintFill_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void TintFill_HandleEnable(HWND hDlg, int fFlags);
LOCAL void TintFill_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgTintFillProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, TintFill_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, TintFill_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, TintFill_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, TintFill_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TINTFILL);
	}
}

/************************************************************************/
LOCAL BOOL TintFill_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Fill.FillOpacity), NO,
	0, 99 );
TintFill_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void TintFill_OnDocActivated(HWND hDlg)
/************************************************************************/
{
TintFill_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void TintFill_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
TintFill_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void TintFill_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
FRMDATATYPE Type = FDT_RGBCOLOR;

ImgGetInfo(GetActiveImage(), NULL, NULL, NULL, &Type);

HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, Fill.FillMergeMode, 0L);
// The following code also goes below and into the shape accelerator
ControlEnable( hDlg, IDC_TRANSPARENT_SCROLL, CANDOTRANSPARENCY(Type) );
ControlEnable( hDlg, IDC_TRANSPARENT, CANDOTRANSPARENCY(Type) );
}

/************************************************************************/
LOCAL void TintFill_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
BOOL Bool;
MERGE_MODE MergeMode;

switch (id)
	{
	case IDC_TINTFILL:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
		break;
	Fill.FillMergeMode = MergeMode;
	SetCommonMergeMode(Fill.FillMergeMode);
	break;

	case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Fill.FillOpacity = 255-TOGRAY(i);
	SetCommonOpacity(Fill.FillOpacity);
	break;

	case IDOK:
//	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
//	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Vignette_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Vignette_OnDocActivated(HWND hDlg);
LOCAL void Vignette_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Vignette_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Vignette_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgVignetteProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Vignette_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Vignette_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Vignette_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Vignette_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_VIGNETTE);
	}
}

/************************************************************************/
LOCAL BOOL Vignette_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CheckComboItem( hDlg, IDC_VIGNETTETYPE, IDC_VIGLINEAR, IDC_VIGRECTANGLE,
	Vignette.Gradient );
CheckComboItem( hDlg, IDC_COLORMODEL, IDC_FIRST_MODEL, IDC_LAST_MODEL,
	Vignette.VigColorModel+IDC_FIRST_MODEL );
InitDlgItemSpin( hDlg, IDC_VIGREPEAT, Vignette.RepeatCount, NO,
	1, 99 );
CheckComboItem( hDlg, IDC_VIGTRANSITION,
	IDC_VIGHARDTRANSITION, IDC_VIGSOFTTRANSITION,
	IDC_VIGHARDTRANSITION + Vignette.SoftTransition );
InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Vignette.VigOpacity),
	NO, 0, 99 );
InitDlgItemSpin( hDlg, IDC_MIDPOINT, TOPERCENT(Vignette.Midpoint),
	NO, 0, 99 );
ControlEnable( hDlg, IDC_VIGTRANSITION, Vignette.RepeatCount > 1 );
Vignette_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Vignette_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Vignette_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Vignette_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Vignette_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Vignette_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
FRMDATATYPE Type = FDT_RGBCOLOR;

ImgGetInfo(GetActiveImage(), NULL, NULL, NULL, &Type);

HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, Vignette.VigMergeMode, 0L);
// The following code also goes below and into the shape accelerator
ControlEnableGroup( hDlg, IDC_FIRST_MODEL, IDC_LAST_MODEL, ISCOLOR(Type));
ControlEnable( hDlg, IDC_COLORMODEL, ISCOLOR(Type));
ControlEnable( hDlg, IDC_TRANSPARENT, CANDOTRANSPARENCY(Type) );
}

/************************************************************************/
LOCAL void Vignette_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
BOOL Bool;
MERGE_MODE MergeMode;

switch (id)
	{
	case IDC_VIGNETTE:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_VIGNETTETYPE:
//	case IDC_VIGLINEAR:
//	case IDC_VIGRADIAL:
//	case IDC_VIGCIRCLE:
//	case IDC_VIGELLIPSE:
//	case IDC_VIGSQUARE:
//	case IDC_VIGRECTANGLE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Vignette.Gradient = id;
	break;

	case IDC_COLORMODEL:
//	case IDC_MODEL_RGB:
//	case IDC_MODEL_HL:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Vignette.VigColorModel = id-IDC_FIRST_MODEL;
	SetCommonColorModel(Vignette.VigColorModel);
	break;

	case IDC_VIGREPEAT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Vignette.RepeatCount = GetDlgItemSpin( hDlg, id, &Bool, NO);
	ControlEnable( hDlg, IDC_VIGTRANSITION,
		Vignette.RepeatCount > 1 );
	break;

	case IDC_VIGTRANSITION:
//	case IDC_VIGSOFTTRANSITION:
//	case IDC_VIGHARDTRANSITION:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Vignette.SoftTransition = id - IDC_VIGHARDTRANSITION;
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
		break;
	Vignette.VigMergeMode = MergeMode;
	SetCommonMergeMode(Vignette.VigMergeMode);
	break;

	case IDC_MIDPOINT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Vignette.Midpoint = (i * 255L) / 100L;
	break;

	case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Vignette.VigOpacity = 255-TOGRAY(i);
	SetCommonOpacity(Vignette.VigOpacity);
	break;

	case IDOK:
//	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
//	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL TextureFill_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TextureFill_OnDocActivated(HWND hDlg);
LOCAL void TextureFill_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void TextureFill_HandleEnable(HWND hDlg, int fFlags);
LOCAL void TextureFill_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgTextureFillProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, TextureFill_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, TextureFill_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, TextureFill_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, TextureFill_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TEXTUREFILL);
	}
}

/************************************************************************/
LOCAL BOOL TextureFill_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.TextureName,
	IDN_TEXTURE );
CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, Texture.fHorzFlip);
CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, Texture.fVertFlip);
InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Texture.TextureOpacity),
	NO, 0, 99 );
lstrcpy( Names.Saved, Texture.TextureName );
TextureFill_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void TextureFill_OnDocActivated(HWND hDlg)
/************************************************************************/
{
TextureFill_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void TextureFill_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
TextureFill_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void TextureFill_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
FRMDATATYPE Type = FDT_RGBCOLOR;

ImgGetInfo(GetActiveImage(), NULL, NULL, NULL, &Type);

HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, Texture.TextureMergeMode, 0L);
// The following code also goes below and into the shape accelerator
ControlEnable( hDlg, IDC_TRANSPARENT_SCROLL, CANDOTRANSPARENCY(Type) );
ControlEnable( hDlg, IDC_TRANSPARENT, CANDOTRANSPARENCY(Type) );
}

/************************************************************************/
LOCAL void TextureFill_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
BOOL Bool;
MERGE_MODE MergeMode;

switch (id)
	{
	case IDC_TEXTUREFILL:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_TEXTUREFLIPX:
	Texture.fHorzFlip = !Texture.fHorzFlip;
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, Texture.fHorzFlip);
	break;

	case IDC_TEXTUREFLIPY:
	Texture.fVertFlip = !Texture.fVertFlip;
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, Texture.fVertFlip);
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.TextureName,
		IDN_TEXTURE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.TextureName,
		IDN_TEXTURE, id, NO );
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeCombo( hDlg, id, FALSE, MM_NORMAL, codeNotify )) < 0)
		break;
	Texture.TextureMergeMode = MergeMode;
	SetCommonMergeMode(Texture.TextureMergeMode);
	break;

	case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Texture.TextureOpacity = 255-TOGRAY(i);
	SetCommonOpacity(Texture.TextureOpacity);
	break;

   default:
	break;
   }
}

/************************************************************************/
void FloodProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
FRMDATATYPE type;
int res;
FLOOD_PARMS parms;
LPIMAGE lpImage;
RECT rArea;

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	Display2File(hWindow,&x, &y);
	lpImage = (LPIMAGE)GetImagePtr(hWindow);
	type = FrameType(ImgGetBaseEditFrame(lpImage));
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.FloodOpacity = Fill.FloodOpacity;
	parms.FloodMergeMode = Fill.FloodMergeMode;
	GetActiveColorFromType(type, &parms.Color);
	parms.iBaseRes = res;
	parms.x = x;
	parms.y = y;
	rArea.left = rArea.right = x;
	rArea.top = rArea.bottom = y;
	Measure(lpImage, &rArea, &parms.InColor);
	parms.FloodRange = Fill.FloodRange;
	parms.FloodFade = Fill.FloodFade;
	parms.idFloodColorModel = Fill.idFillColorModel;
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_FLOOD, &parms);
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


/************************************************************************/
void TintFillProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
LPIMAGE lpImage;
FRMDATATYPE type;
TINTFILL_PARMS parms;

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
    lpImage = (LPIMAGE)GetImagePtr ( hWindow );
	type = FrameType(ImgGetBaseEditFrame(lpImage));

	parms.FillOpacity = Fill.FillOpacity;
	parms.FillMergeMode = Fill.FillMergeMode;
	GetActiveColorFromType(type, &parms.Color);

	ProcessCommand(lpImage->lpCmdList, IDS_CMD_TINTFILL, &parms);
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


/************************************************************************/
void VignetteProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
RECT Rect;
BOOL fShift;
POINT pt;
static int Type, Style;
static BOOL fConstrain;
static RECT SelectRect;
int res;
FRMDATATYPE type;
LPIMAGE lpImage;
STRING szString;
GRADIENT_PARMS parms;

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	ImgGetInfo(GetActiveImage(), NULL, NULL, NULL, &type);
	if (type == FDT_LINEART)
		{
		AstralStrEx( IDC_TOOLNAMESTART+IDC_VIGNETTE, szString, sizeof(szString) );
		Message(IDS_NOTWITHLINEART, (LPSTR)szString);
		break;
		}
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
//	SetMaskingState( x, y );
	break;

    case WM_LBUTTONDOWN:
	Type = Vignette.Gradient;
	fConstrain = ( Type == IDC_VIGSQUARE || Type == IDC_VIGCIRCLE );
	AstralSetRectEmpty( &SelectRect );
	if ( Type == IDC_VIGCIRCLE || Type == IDC_VIGELLIPSE )
		Style = SL_ELLIPSE;
	else
	if ( Type == IDC_VIGRADIAL || Type == IDC_VIGLINEAR )
		Style = SL_LINE;
	else
		Style = SL_BOX;
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, Style | SL_SPECIAL,
		pt, 0L );
	break;

    case WM_LBUTTONUP:
	Tool.bActive = NO;
	Rect = SelectRect; // make a copy before it gets ordered
	EndSelection( hWindow, NULL, &SelectRect, Style, YES );
    lpImage = (LPIMAGE)GetImagePtr ( hWindow );
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	type = FrameType(ImgGetBaseEditFrame(lpImage));

	parms.VigOpacity = Vignette.VigOpacity;
	parms.VigMergeMode = Vignette.VigMergeMode;
	parms.iBaseRes = res;
	parms.x1 = Rect.left;
	parms.y1 = Rect.top;
	parms.x2 = Rect.right;
	parms.y2 = Rect.bottom;
	GetActiveColorFromType(type, &parms.StartColor);
	GetAlternateColorFromType(type, &parms.EndColor);
	parms.Gradient = Vignette.Gradient;
	parms.RepeatCount = Vignette.RepeatCount;
	parms.SoftTransition = Vignette.SoftTransition;
	parms.Midpoint = Vignette.Midpoint;
	parms.VigColorModel = Vignette.VigColorModel;
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_GRADIENT, &parms);
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fShift = SHIFT;
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, Style,
		pt, fConstrain^CONSTRAINASPECT, 1L, 1L,
		MOVEKEY||Window.fRButtonDown, FROMCENTER);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	if (!Tool.bActive)
		break;
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, Style, YES );
	break;
    }
}


static FNAME szTextureName;
static LPFRAME lpTextureFrame;

/************************************************************************/
void TextureFillProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
FNAME szFileName;
LPIMAGE lpImage;
TEXTUREFILL_PARMS parms;

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	// Load the texture
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!LookupExtFile(Texture.TextureName, szFileName, IDN_TEXTURE))
		break;
	if (!FileExists(szFileName))
		{
		Message(IDS_EOPEN, (LPSTR)szFileName);
		break;
		}
    lpImage = (LPIMAGE)GetImagePtr ( hWindow );
	parms.TextureOpacity = Texture.TextureOpacity;
	parms.TextureMergeMode = Texture.TextureMergeMode;
	lstrcpy(parms.TextureName, Texture.TextureName);
	parms.fHorzFlip = Texture.fHorzFlip;
	parms.fVertFlip = Texture.fVertFlip;
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_TEXTUREFILL, &parms);
	break;

	case WM_ACTIVATE:
	if (!Option) // a deactivate
		{
		FrameClose( lpTextureFrame );
		lpTextureFrame = NULL;
		}
	return;
		
    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	Tool.bActive = NO;
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



