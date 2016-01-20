//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "id.h"
#include "ctxhelp.h"
#include "tablet.h"

typedef enum
	{
	BM_NORMAL,
	BM_SETSRC,
	BM_SETDST
	} BRUSH_MODE;

typedef enum
	{
	PT_NONE,
	PT_NORMAL,
	PT_OPACITY,
	PT_STYLE
	} PRESSURE_TYPE;

#define ACTIVE_IMAGE "Active Image"
#define ALL_PACKETS     0
#define KEEP_LAST       2

// Static routines
static void CloneBrush( HWND hWnd, int x, int y, int size, int on );
static void WinBrush( HWND hWnd, HDC hDC, LPRECT lpRect, int flag, int fTickMarks, int shape, LFIXED DispRate );
static BOOL CreateRetouchBrush( ITEMID idTool, ITEMID idFirstTool,
								ITEMID idFirstStyle, FNAME FAR *StyleName );
static BOOL HandleRetouchDialog(HWND hDlg, UINT msg, WPARAM wParam,
						LPARAM lParam, LPRETOUCHDLG lpRetouchDlg);
static void HandlePaintTool( HWND hWindow, UINT msg, int x, int y, UINT32 Option,
							ITEMID dirty, ITEMID idTool);	
static void AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame );
static BOOL BrushPreviewToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg );
static void UpdateBrushPreview(HWND hWnd, LPRECT lpUpdateRect, BOOL fUpdateCursor);
static void SetupBrushPreview(HWND hDlg);
static void SetupPreviewCursor(HWND hDlg, BOOL fInit);
static BOOL DisplayPreviewBrush(HWND hWnd, int x, int y, BOOL on );
static void GetCloneSrc(HWND hWnd, int x, int y, int BrushSize, LPINT lpX, LPINT lpY);
static void SetupStrokeTool(HWND hDlg, ITEMID idTool);
LOCAL void SaveRetouchMacro(LPIMAGE lpImage, ITEMID idTool);
LOCAL void SaveCloneMacro(LPIMAGE lpImage, HWND hCloneSrcWnd, POINT ptCloneSrc);
static void FillPreviewFrame(LPFRAME lpFrame);
static void InitToolBrush(LPMGXBRUSH lpBrush, ITEMID idCurTool);
static BOOL CreatePaintOnBrush(LPCOLORINFO lpColor, FRMDATATYPE Type);
LOCAL BOOL IsValidTool(LPIMAGE lpImage, ITEMID idTool);

// Retouch statics
static int cfdx = 60, cfdy = 60;
static BRUSH_MODE BrushMode;
static POINT ptCloneSrc;
static HWND hCloneSrcWnd;
static ITEMID idCurTool, idCurFirstTool, idCurFirstStyle;
static FNAME FAR *CurStyleName;
static LPFRAME lpPreviewFrame;
static PIXMAP PreviewPixmap;
static LPMGXBRUSH lpBrush;
static RETOUCHDLG rd;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaintProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
if (Control.idRetouchTool == IDC_PAINT ||
	Control.idRetouchTool == IDC_SPRAY)
	rd.idCreateDlg = IDD_CREATEBRUSHSCRATCH;
else
	rd.idCreateDlg = IDD_CREATEBRUSH;
rd.idTool = Control.idRetouchTool;
rd.idFirstTool = IDC_FIRST_RETOUCHTOOL;
rd.idFirstStyle = IDN_FIRST_RETOUCHSTYLE;
rd.StyleName = Retouch.RetouchStyleName;
rd.fIsRibbon = YES;
return(HandleRetouchDialog(hDlg, msg, wParam, lParam, &rd));
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgTextureProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
rd.idTool = Control.idRetouchTool;
rd.idFirstTool = IDC_FIRST_RETOUCHTOOL;
rd.idFirstStyle = IDN_FIRST_RETOUCHSTYLE;
rd.idCreateDlg = IDD_CREATETEXTUREBRUSH;
rd.StyleName = Retouch.RetouchStyleName;
rd.fIsRibbon = YES;
return(HandleRetouchDialog(hDlg, msg, wParam, lParam, &rd));
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgFilterProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
rd.idTool = Control.idRetouchTool;
rd.idFirstTool = IDC_FIRST_FILTERTOOL;
rd.idFirstStyle = IDN_FIRST_FILTERSTYLE;
rd.idCreateDlg = IDD_CREATEBRUSH;
rd.StyleName = Retouch.FilterStyleName;
rd.fIsRibbon = YES;
return(HandleRetouchDialog(hDlg, msg, wParam, lParam, &rd));
}

// LOCAL prototypes
LOCAL BOOL HandleRetouch_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void HandleRetouch_OnDocActivated(HWND hDlg);
LOCAL void HandleRetouch_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void HandleRetouch_HandleEnable(HWND hDlg, int fFlags);
LOCAL void HandleRetouch_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void HandleRetouch_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);

static LPRETOUCHDLG lpRD;

/***********************************************************************/
BOOL HandleRetouchDialog(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam,
						LPRETOUCHDLG lpRetouchDlg)
/***********************************************************************/
{
lpRD = lpRetouchDlg;
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, HandleRetouch_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, HandleRetouch_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, HandleRetouch_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, HandleRetouch_OnControlEnable);

	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, HandleRetouch_OnMeasureItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, 0);
	}
}

/************************************************************************/
LOCAL BOOL HandleRetouch_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
RetouchInit(hDlg, hWndFocus, lParam, lpRD);
HandleRetouch_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void HandleRetouch_OnDocActivated(HWND hDlg)
/************************************************************************/
{
if (Retouch.hBrush)
	FreeUpMgxBrushMask(Retouch.hBrush);
HandleRetouch_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void HandleRetouch_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
HandleRetouch_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void HandleRetouch_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
RetouchEnable(hDlg, fFlags, lpRD);
}

/************************************************************************/
LOCAL void HandleRetouch_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
MeasureItem( hDlg, lpMeasureItem, lpMeasureItem->CtlID == IDC_BRUSHSHAPE );
}

/************************************************************************/
LOCAL void HandleRetouch_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
RetouchCommand(hDlg, id, hControl, codeNotify, lpRD);
}

/************************************************************************/
void PaintProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
HandlePaintTool(hWindow, msg, x, y, Option,
		Control.idRetouchTool - IDC_FIRST_RETOUCHTOOL + IDS_FIRST_RETOUCHUNDO,
		Control.idRetouchTool);
}

/************************************************************************/
void FilterProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
HandlePaintTool(hWindow, msg, x, y, Option,
		Control.idRetouchTool - IDC_FIRST_RETOUCHTOOL + IDS_FIRST_RETOUCHUNDO,
		Control.idRetouchTool);
}

/************************************************************************/
void CloneProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy, sx, sy, npix, nlin;
FRMDATATYPE SrcType, DstType;
LPIMAGE lpSrcImage;
LPOBJECT lpObject, lpSrcObject;
RECT rMirror;
LPIMAGE lpImage;
MGXBRUSHPARMS mgxbrush;

lpImage = GetImagePtr(hWindow);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
    if( Control.UseTablet )
        FlushTabletQueue( KEEP_LAST );
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	if (OPTION1 && !LBUTTON)
		break;
	if (OPTION3) // not while repositioning
		break;
	if (BrushMode == BM_SETSRC)
		{
	  	DisplayBrush(0, 0, 0, OFF);
		hCloneSrcWnd = NULL;
		ptCloneSrc.x = x;
		ptCloneSrc.y = y;
		Display2File(hWindow, (LPINT)&ptCloneSrc.x, (LPINT)&ptCloneSrc.y);
		hCloneSrcWnd = hWindow;
		BrushMode = BM_SETDST;
		DisplayBrush(hWindow, x, y, ON);
		if ( Tool.hRibbon )				  
			SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		break;
		}
	if (BrushMode == BM_SETDST)
		{
	  	DisplayBrush(0, 0, 0, OFF);
		BrushMode = BM_NORMAL;
		if (!IsDoc(hCloneSrcWnd))
			hCloneSrcWnd = NULL;
		rMirror.left = rMirror.right = ptCloneSrc.x;
		rMirror.top = rMirror.bottom = ptCloneSrc.y;
		if (hCloneSrcWnd)
			lpSrcImage = GetImagePtr(hCloneSrcWnd);
		else
			lpSrcImage = lpImage;
		ImgGetInfo(lpSrcImage, &npix, &nlin, NULL, NULL);
		MgxBrushMirror(Retouch.hBrush, &rMirror,
					&rMirror, npix, nlin);
		fx = x; fy = y;
		Display2File(hWindow, &fx, &fy);
		cfdx = rMirror.left - fx;
		cfdy = rMirror.top - fy;
		DisplayBrush(hWindow, x, y, ON);
		}

	Tool.bActive = YES;

	break;

	case WM_ACTIVATE:
	BrushMode = BM_NORMAL;
	if (!Option) // a deactivate
		{
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{	// an activate ( to re-create brush)
			if ( Tool.hRibbon )
				SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		}
	break;
	
    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		{
		fx = x;
		fy = y;
		Display2File(hWindow, &fx, &fy );
		lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
		}
	if (!lpObject)
		break;
	lpSrcImage = NULL;
	if (hCloneSrcWnd)
		{
		if (!IsDoc(hCloneSrcWnd))
			{
			hCloneSrcWnd = NULL;
			break;
			}
		lpSrcImage = GetImagePtr(hCloneSrcWnd);
		lpSrcObject = ImgFindObject(lpSrcImage, ptCloneSrc.x, ptCloneSrc.y,
						NO, NO);
		if (!lpSrcObject)
			{
			hCloneSrcWnd = NULL;
			break;
			}
		// make sure source object and destination object are
		// the same data type
		PixmapGetInfo(&lpSrcObject->Pixmap, PMT_EDIT, NULL, NULL, NULL,
						 &SrcType);
		PixmapGetInfo(&lpObject->Pixmap, PMT_EDIT, NULL, NULL, NULL,
						&DstType);
		if (SrcType != DstType)
			{
			Message(IDS_BRUSHCLONETYPE);
			break;
			}
		SetMgxBrushSource(Retouch.hBrush, lpSrcObject->Pixmap.EditFrame,
						lpSrcObject->rObject.left, lpSrcObject->rObject.top);
		}
	if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		break;

	MgxBrushSetAdvanced(&mgxbrush, hWindow, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					Retouch.hBrush, YES, lpImage, NULL, GetCloneSrc);
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		mgxbrush.dwSaveStrokeInfo = SSI_DST | SSI_SRC;
	if (!MgxBrushBeginParms(&mgxbrush))
	    break;
	ImgEditedObject(lpImage, lpObject, IDS_UNDOCLONE, NULL);
	Tool.bActive = MgxBrushActive();
	if (!Tool.bActive)
		{
		if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
			SaveCloneMacro(lpImage, hCloneSrcWnd, ptCloneSrc);
		break;
		}

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x;
	fy = y;
	Display2File(hWindow, &fx, &fy );
	CloneBrush(hWindow, fx, fy, GetMgxBrushSize(Retouch.hBrush), YES);
	sx = cfdx + fx;
	sy  = cfdy + fy;
	MgxBrushStroke(sx, sy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	SetMgxBrushSource(Retouch.hBrush, NULL, 0, 0);
	Tool.bActive = NO;
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		SaveCloneMacro(lpImage, hCloneSrcWnd, ptCloneSrc);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	SetMgxBrushSource(Retouch.hBrush, NULL, 0, 0);
	Tool.bActive = NO;

	break;
    }
}

/************************************************************************/
LOCAL BOOL IsValidTool(LPIMAGE lpImage, ITEMID idTool)
/************************************************************************/
{
	ITEMID BadTypeTools[] = {IDC_SHARP, IDC_SMOOTH, IDC_LIGHTEN, IDC_DARKEN};
 	FRMTYPEINFO TypeInfo;
	int i;
	STRING szString;
	LPSTR lp;

	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (TypeInfo.DataType == FDT_LINEART ||
		TypeInfo.DataType == FDT_PALETTECOLOR)
	{
		for ( i=0; i<sizeof(BadTypeTools)/sizeof(ITEMID); i++ )
			{
			if (idTool != BadTypeTools[i])
				continue;
			if ( AstralStr( IDC_TOOLNAMESTART+idTool, &lp ) )
				lstrcpy( szString, lp );
			else
				szString[0] = '\0';
			Message(IDS_EBADTYPE, (LPSTR)szString);
			return(FALSE);
			}
	}
	return(TRUE);
}

/************************************************************************/
void HandlePaintTool( HWND hWindow, UINT msg, int x, int y, UINT32 Option,
						ITEMID dirty, ITEMID idTool)
/************************************************************************/
{
int fx, fy;
LPOBJECT lpObject;
LPIMAGE lpImage;
MGXBRUSHPARMS mgxbrush;

lpImage = GetImagePtr(hWindow);

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
    if( Control.UseTablet )
        FlushTabletQueue( KEEP_LAST );
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	if (!IsValidTool(lpImage, idTool))
		break;
	Tool.bActive = YES;
	break;

	case WM_ACTIVATE:
	BrushMode = BM_NORMAL;
	if (!Option) // a deactivate
		{
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{	// an activate ( to re-create brush)
			if ( Tool.hRibbon )
				SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		}
	break;
	
    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		{
		fx = x;
		fy = y;
		Display2File(hWindow, &fx, &fy );
		lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
		}
	if (!lpObject)
		break;
	if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		break;
	
	MgxBrushSetDefaults(&mgxbrush, hWindow, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					Retouch.hBrush);
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		{
		mgxbrush.dwSaveStrokeInfo = SSI_DST;
//		if (idTool == IDC_TEXTURE)
//			mgxbrush.dwSaveStrokeInfo |= SSI_SRC;
		}
	if (!MgxBrushBeginParms(&mgxbrush))
	    break;

	ImgEditedObject(lpImage, lpObject, dirty, NULL);
	Tool.bActive = MgxBrushActive();
	if (!Tool.bActive)
		{
		if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
			SaveRetouchMacro(lpImage, idTool);
		break;
		}

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x;
	fy = y;
	Display2File(hWindow, &fx, &fy );
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	Tool.bActive = NO;
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		SaveRetouchMacro(lpImage, idTool);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	Tool.bActive = NO;
	break;
    }
}

/************************************************************************/
void TextureProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
HandlePaintTool(hWindow, msg, x, y, Option,
		Control.idRetouchTool - IDC_FIRST_RETOUCHTOOL + IDS_FIRST_RETOUCHUNDO,
		Control.idRetouchTool);
}

/************************************************************************/
void EraserProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy;
LPOBJECT lpObject;
LPIMAGE lpImage;
MGXBRUSHPARMS mgxbrush;

lpImage = GetImagePtr(hWindow);

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
    if( Control.UseTablet )
        FlushTabletQueue( KEEP_LAST );
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	Tool.bActive = YES;
	break;

	case WM_ACTIVATE:
	BrushMode = BM_NORMAL;
	if (!Option) // a deactivate
		{
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{	// an activate ( to re-create brush)
			if ( Tool.hRibbon )
				SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		}
	break;
	
    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		{
		fx = x;
		fy = y;
		Display2File(hWindow, &fx, &fy );
		lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
		}

	if (!lpObject)
		break;

	if (GetMgxBrushSource(Retouch.hBrush) == SD_CURRENT_UNDO)
		{
		if ( !lpObject->DataDirty || !lpObject->Pixmap.UndoFrame )
			{
			MessageBeep(0);
			Message(IDS_EERASENOTHING);
			break;
			}
		if (lpObject->Pixmap.fNewFrame)
			{
			MessageBeep(0);
			Message(IDS_EERASESIZE);
			break;
			}
		}
	else
		{
		if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
			break;
		ImgEditedObject(lpImage, lpObject, IDS_UNDOERASER, NULL);
		}

	MgxBrushSetDefaults(&mgxbrush, hWindow, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					Retouch.hBrush);
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		mgxbrush.dwSaveStrokeInfo = SSI_DST;
	if (!MgxBrushBeginParms(&mgxbrush))
	    break;

	Tool.bActive = MgxBrushActive();
	if (!Tool.bActive)
		{
		if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
			SaveRetouchMacro(lpImage, IDC_ERASER);
		break;
		}

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(hWindow,&fx, &fy);
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	Tool.bActive = NO;	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		SaveRetouchMacro(lpImage, IDC_ERASER);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	Tool.bActive = NO;
	break;
    }
}

/************************************************************************/
void AltEraserProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy;
LPOBJECT lpObject;
static HMGXBRUSH hBrush;
LPIMAGE lpImage;
MGXBRUSHPARMS mgxbrush;

lpImage = GetImagePtr(hWindow);

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
    if( Control.UseTablet )
        FlushTabletQueue( KEEP_LAST );
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	Tool.bActive = YES;
	break;

    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		{
		fx = x;
		fy = y;
		Display2File(hWindow, &fx, &fy );
		lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
		}

	if (!lpObject)
		break;

	if (!(hBrush = CreateMgxEraser(0)))
		break;

	SetMgxBrushSize(hBrush, Retouch.BrushSize);
	SetMgxBrushShape(hBrush, Retouch.BrushShape, Names.CustomBrush);
	SetMgxBrushOpacity(hBrush, Retouch.Opacity);
    SetMgxBrushFeather(hBrush, Retouch.Feather);

	if ( !lpObject->DataDirty || !lpObject->Pixmap.UndoFrame )
		{
		MessageBeep(0);
		Message(IDS_EERASENOTHING);
		if (hBrush)
			{
			DestroyMgxBrush(hBrush);
			hBrush = NULL;
			}
		break;
		}
	if (lpObject->Pixmap.fNewFrame)
		{
		MessageBeep(0);
		Message(IDS_EERASESIZE);
		if (hBrush)
			{
			DestroyMgxBrush(hBrush);
			hBrush = NULL;
			}
		break;
		}
	MgxBrushSetDefaults(&mgxbrush, hWindow, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					hBrush);
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		mgxbrush.dwSaveStrokeInfo = SSI_DST;
	if (!MgxBrushBeginParms(&mgxbrush))
	    break;
	Tool.bActive = MgxBrushActive();
	if (!Tool.bActive)
		{
		if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
			SaveRetouchMacro(lpImage, IDC_ALTERASER);
		break;
		}

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(hWindow,&fx, &fy);
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	if (hBrush)
		{
		DestroyMgxBrush(hBrush);
		hBrush = NULL;
		}
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		SaveRetouchMacro(lpImage, IDC_ALTERASER);

	Tool.bActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	if (hBrush)
		{
		DestroyMgxBrush(hBrush);
		hBrush = NULL;
		}
	Tool.bActive = NO;
	break;

	default:
		return;
    }
if (!Tool.bActive)
	InstallTool(NO);
}

/************************************************************************/
void AltPaintOnProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy;
LPOBJECT lpObject;
LPALPHA lpAlpha;
static HMGXBRUSH hBrush;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWindow);

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
    if( Control.UseTablet )
        FlushTabletQueue( KEEP_LAST );
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	Tool.bActive = YES;
	break;

    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	lpObject = NULL;
	if (Mask.PaintOnObject && ImgMultipleObjects(lpImage))
		{
		if (ImgCountSelObjects(lpImage, NULL) == 1)
			lpObject = ImgGetSelObject(lpImage, NULL);
		else
			{
			fx = x;
			fy = y;
			Display2File(hWindow, &fx, &fy );
			lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
			}
		}
	else
		lpObject = ImgGetBase(lpImage);

	if (!lpObject)
		break;

	if (!(lpAlpha = lpObject->lpAlpha))
		break;

	if (!(hBrush = CreateMgxEraser(0)))
		break;

	if (GetMgxBrushSource(hBrush) == SD_CURRENT_UNDO)
		{
		if ( !lpObject->AlphaDirty || !lpAlpha->Pixmap.UndoFrame )
			{
			MessageBeep(0);
			Message(IDS_EERASENOTHING);
			if (hBrush)
				{
				DestroyMgxBrush(hBrush);
				hBrush = NULL;
				}
			break;
			}
		if (lpAlpha->Pixmap.fNewFrame)
			{
			MessageBeep(0);
			Message(IDS_EERASESIZE);
			if (hBrush)
				{
				DestroyMgxBrush(hBrush);
				hBrush = NULL;
				}
			break;
			}
		}
	else
		{
		if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpObject))
			{
			if (hBrush)
				{
				DestroyMgxBrush(hBrush);
				hBrush = NULL;
				}
			break;
			}
		ImgEditedObject(lpImage, lpObject, IDS_UNDOERASER, NULL);
		}

	if (!MgxBrushBegin(hWindow, &lpAlpha->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					hBrush))
	    break;

	Tool.bActive = MgxBrushActive();

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(hWindow,&fx, &fy);
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	if (hBrush)
		{
		DestroyMgxBrush(hBrush);
		hBrush = NULL;
		}
	Tool.bActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	if (hBrush)
		{
		DestroyMgxBrush(hBrush);
		hBrush = NULL;
		}
	Tool.bActive = NO;
	break;
    }
if (!Tool.bActive)
	InstallTool(NO);
}

// LOCAL prototypes
LOCAL BOOL PaintOn_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaintOn_OnDocActivated(HWND hDlg);
LOCAL void PaintOn_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void PaintOn_HandleEnable(HWND hDlg, int fFlags);
LOCAL void PaintOn_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaintOnProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PaintOn_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PaintOn_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, PaintOn_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, PaintOn_OnControlEnable);

	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, HandleRetouch_OnMeasureItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, Tool.idRibbon);
	}
}

/************************************************************************/
LOCAL BOOL PaintOn_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	COLORINFO ColorStruct;
	HWND hActiveWnd;

	hActiveWnd = GetActiveDoc();

	SetDlgItemText( hDlg, IDC_BRUSHCUSTOM, Names.CustomBrush );
	ControlEnable( hDlg,  IDC_BRUSHCUSTOM, Names.CustomBrush[0] != 0 );
	CheckComboItem( hDlg, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE,
		IDC_BRUSHCUSTOMNEW, Retouch.BrushShape );
	InitDlgItemSpin( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, NO,
		1, MAX_BRUSH_SIZE );
	InitDlgItemSpin( hDlg, IDC_FEATHER, Retouch.Feather, NO,
		0, 100 );
	ColorStruct.ColorSpace = CS_GRAY;
	if (GetDlgItem(hDlg, IDC_TRANSPARENT))
		InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Retouch.Opacity),
			NO, 0, 99 );
	if (Mask.iPaintOnModes == SHAPE_SUBTRACT)
		ColorStruct.gray = 0;
	else
		ColorStruct.gray = 255;
	CreatePaintOnBrush(&ColorStruct, FDT_RGBCOLOR);
	PaintOn_HandleEnable(hDlg, 0);
	return(FALSE);
}

/************************************************************************/
LOCAL void PaintOn_OnDocActivated(HWND hDlg)
/************************************************************************/
{
PaintOn_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void PaintOn_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
PaintOn_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void PaintOn_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
COLORINFO ColorStruct;
FRMDATATYPE Type = FDT_RGBCOLOR;
LPIMAGE lpImage = GetActiveImage();
BOOL fCanPaintOnAlpha;

if (lpImage)
	lpImage->GetInfo(NULL, NULL, NULL, &Type);

fCanPaintOnAlpha = ImgGetSelObject(lpImage, NULL) != ImgGetBase(lpImage);
if (!fCanPaintOnAlpha)
	Mask.PaintOnObject = FALSE;

if (GetDlgItem(hDlg, IDC_PAINTONDEST))
	{
	CheckComboItem( hDlg, IDC_PAINTONDEST, IDC_PAINTONMASK,
		IDC_PAINTONOBJECT, Mask.PaintOnObject + IDC_PAINTONMASK );
	ControlEnable(hDlg, IDC_PAINTONDEST, fCanPaintOnAlpha);
	}

if (Mask.iPaintOnModes == SHAPE_REVERSE || !Mask.iPaintOnModes)
	Mask.iPaintOnModes = SHAPE_DEFAULT;
CheckRadioButton( hDlg, IDC_MASKMODE_ADD, IDC_MASKMODE_SUBTRACT,
	IDC_MASKMODE_REVERSE + max(Mask.iPaintOnModes - SHAPE_REVERSE, 0) );
// The following code also goes below and into the shape accelerator
ControlEnable(hDlg, IDC_FEATHER, (Retouch.BrushShape == IDC_BRUSHCIRCLE ||
								Retouch.BrushShape == IDC_BRUSHSQUARE) && CANDOTRANSPARENCY(Type));
ControlEnable(hDlg, IDC_BRUSHSIZE, Retouch.BrushShape != IDC_BRUSHCUSTOM);
ControlEnable( hDlg, IDC_TRANSPARENT_SCROLL, CANDOTRANSPARENCY(Type) );
ControlEnable( hDlg, IDC_TRANSPARENT, CANDOTRANSPARENCY(Type) );
if (!Retouch.hBrush)
	{
	if (Mask.iPaintOnModes == SHAPE_SUBTRACT)
		ColorStruct.gray = 0;
	else
		ColorStruct.gray = 255;
	CreatePaintOnBrush(&ColorStruct, FDT_RGBCOLOR);
	}

}

/************************************************************************/
LOCAL void PaintOn_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int NewShape, i;
COLORINFO ColorStruct;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
	case IDC_PAINTON:
    if( !Control.Retail )
        break;
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_MASKMODE_ADD:
	case IDC_MASKMODE_SUBTRACT:
	Mask.iPaintOnModes = SHAPE_REVERSE + (id - IDC_MASKMODE_REVERSE);
	CheckRadioButton( hDlg, IDC_MASKMODE_ADD, IDC_MASKMODE_SUBTRACT,
		id );
	ColorStruct.ColorSpace = CS_GRAY;
	if (Mask.iPaintOnModes == SHAPE_SUBTRACT)
		ColorStruct.gray = 0;
	else
		ColorStruct.gray = 255;
	SetMgxBrushColor(Retouch.hBrush, &ColorStruct);
	SetCommonMode(Mask.iPaintOnModes);
	break;

	case IDC_PAINTONDEST:
//	case IDC_PAINTONMASK:
//	case IDC_PAINTONOBJECT:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.PaintOnObject = id - IDC_PAINTONMASK;
	break;

	case IDC_BRUSHSHAPE:
//	case IDC_BRUSHCIRCLE:
//	case IDC_BRUSHSQUARE:
//	case IDC_BRUSHHORIZONTAL:
//	case IDC_BRUSHVERTICAL:
//	case IDC_BRUSHSLASH:
//	case IDC_BRUSHBACKSLASH:
//	case IDC_BRUSHCUSTOM:
//	case IDC_BRUSHCUSTOMNEW:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	NewShape = id;
	if ( NewShape == IDC_BRUSHCUSTOMNEW )
		{
		NewShape = IDC_BRUSHCUSTOM;
		if (!AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_CUSTOMBRUSH,
		     DlgCustomBrushProc ))
			NewShape = Retouch.BrushShape;
		SetDlgItemText( hDlg, IDC_BRUSHCUSTOM, Names.CustomBrush );
		if (Names.CustomBrush[0] == 0)
			NewShape = Retouch.BrushShape;
		ControlEnable(hDlg, IDC_BRUSHCUSTOM, Names.CustomBrush[0] != 0);
		}
	if (hActiveWnd)
			DisplayBrush(hActiveWnd, 0, 0, OFF);
	if (SetMgxBrushShape(Retouch.hBrush, NewShape, Names.CustomBrush))
		Retouch.BrushShape = NewShape;
	SetCommonBrushShape(Retouch.BrushShape);
	CheckComboItem( hDlg, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE,
		IDC_BRUSHCUSTOMNEW, Retouch.BrushShape);
	SendMessage(hDlg, WM_CONTROLENABLE, 0, 0L);
	if (hActiveWnd && Window.hCursor == Window.hNullCursor)
		DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
	break;

	case IDC_BRUSHSIZE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	if (hActiveWnd)
		DisplayBrush(hActiveWnd, 0, 0, OFF);
	Retouch.BrushSize = GetDlgItemSpin( hDlg, IDC_BRUSHSIZE,
		&Bool, NO );
	if (Retouch.BrushSize > MAX_BRUSH_SIZE)
		Retouch.BrushSize = MAX_BRUSH_SIZE;
	if (Retouch.BrushSize < 1)
		Retouch.BrushSize = 1;
	SetMgxBrushSize(Retouch.hBrush, Retouch.BrushSize);
	SetCommonBrushSize(Retouch.BrushSize);
   if (hActiveWnd && Window.hCursor == Window.hNullCursor)
		DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
	break;

	case IDC_FEATHER:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Retouch.Feather = GetDlgItemSpin( hDlg, IDC_FEATHER,
		&Bool, NO );
	SetMgxBrushFeather(Retouch.hBrush, Retouch.Feather);
	SetCommonFeather(Retouch.Feather);
	break;

    case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Retouch.Opacity = 255-TOGRAY(i);
	SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
	SetCommonOpacity(Retouch.Opacity);
	break;

   default:
	break;
   }
}

/************************************************************************/
void PaintOnProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int fx, fy;
LPALPHA lpAlpha;
LPOBJECT lpObject;
COLORINFO ColorStruct;
RECT rUpdate;
BOOL fNewMask;
LPIMAGE lpImage;
MGXBRUSHPARMS mgxbrush;
FRMDATATYPE Type;

lpImage = GetImagePtr(hWindow);

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
    if( Control.UseTablet )
        FlushTabletQueue( KEEP_LAST );
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	Tool.bActive = YES;
	break;

	case WM_ACTIVATE:
	BrushMode = BM_NORMAL;
	if (!Option) // a deactivate
		{
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{	// an activate ( to re-create brush)
			if ( Tool.hRibbon )
				SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		}
	break;
	
    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	fNewMask = NO;
	if (Mask.PaintOnObject && ImgMultipleObjects(lpImage))
		{
		if (ImgCountSelObjects(lpImage, NULL) == 1)
			lpObject = ImgGetSelObject(lpImage, NULL);
		else
			{
			fx = x;
			fy = y;
			Display2File(hWindow, &fx, &fy );
			lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
			}
		if (!lpObject)
			break;
		if (lpObject == ImgGetBase(lpImage))
			break;
		lpAlpha = lpObject->lpAlpha;
		if (!lpAlpha)
			break;
		}
	else if (lpAlpha = ImgGetMaskEx(lpImage, Mask.iPaintOnModes == SHAPE_SUBTRACT,
						 Control.NoUndo, &fNewMask, &rUpdate))
		{
		UpdateImage(lpImage, &rUpdate, YES);
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		UpdateStatusBar( NO, NO, YES, NO );

		lpObject = ImgGetBase(lpImage);
		if (fNewMask &&
			(Mask.iPaintOnModes == SHAPE_ADD && ImgGetViewMode(lpImage) == VM_SLIME))
			{
			if (ImgGetMaskUpdateRect(lpImage, YES, NO, &rUpdate))
  				{
				File2DispRect(hWindow,&rUpdate, &rUpdate);
				++rUpdate.right; ++rUpdate.bottom;
				InvalidateRect(hWindow, &rUpdate, FALSE);
  				}
			}
		}
	else
		break;
	
	Type = FrameType(ImgGetBaseEditFrame(lpImage));
	ColorStruct.ColorSpace = CS_GRAY;
	if (Mask.iPaintOnModes == SHAPE_SUBTRACT)
		ColorStruct.gray = lpAlpha->bInvert ? 255 : 0;
	else
		ColorStruct.gray = lpAlpha->bInvert ? 0 : 255;
	CreatePaintOnBrush(&ColorStruct, Type);
	if (!Retouch.hBrush)
		{
		Message(IDS_EMEMALLOC);
		break;
		}
	if (!ImgEditInit(lpImage, ET_OBJECT,
				fNewMask ? UT_CREATEMASK : UT_ALPHA, lpObject))
		{
		break;
		}
	if (Retouch.Opacity == 255 || Type == FDT_LINEART || Type == FDT_PALETTECOLOR)
		((LPMGXBRUSH)Retouch.hBrush)->NoOverlap = NO;
	else
		((LPMGXBRUSH)Retouch.hBrush)->NoOverlap = YES;
	// make sure the resolution of the image data and mask
	// are the same so that 
	FrameSetResolution(AlphaGetEditFrame(lpAlpha),
				FrameResolution(ImgGetBaseEditFrame(lpImage)));
	MgxBrushSetDefaults(&mgxbrush, hWindow, &lpAlpha->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					Retouch.hBrush);
	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		mgxbrush.dwSaveStrokeInfo = SSI_DST;
	if (!MgxBrushBeginParms(&mgxbrush))
	    break;
	ImgEditedObject(lpImage, lpObject, IDS_UNDOPAINTMASK, NULL);
	Tool.bActive = MgxBrushActive();
	if (!Tool.bActive)
		{
		if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
			SaveRetouchMacro(lpImage, IDC_PAINTON);
		break;
		}

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(hWindow,&fx, &fy);
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	Tool.bActive = NO;

	if (IsMacroRecording() || lpImage->lpCmdList->fSaveCommands)
		SaveRetouchMacro(lpImage, IDC_PAINTON);
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	Tool.bActive = NO;
	break;
    }
}

/************************************************************************/
void DisplayBrush(HWND hWnd, int x, int y, BOOL on )
/************************************************************************/
{
DisplayBrushEx(hWnd, x, y, on, NULL);
}

/************************************************************************/
BOOL DisplayBrushEx(HWND hWnd, int x, int y, BOOL on, LPRECT lpOffRect )
/************************************************************************/
{
HDC hDC;
RECT onFRect;
LPIMAGE lpSrcImage;
LPDISPLAY lpDisplay;
int npix, nlin;
static RECT onRect, on2Rect, rSect;
static int bx, by;
static BOOL bBrushOn, bBrush2On, bLocked;
static HWND hBrushWnd, hBrush2Wnd;
static int ShapeOn;
static int SizeOn;
int HalfSize;

if (!hWnd && on)
	return(FALSE);

// make sure we are in a valid state
if ( on == bBrushOn || !Retouch.hBrush)
	return(FALSE);

if (on)
    {
	// special case to redisplay brush at same location it was turned off
    if (x == INT_MAX)
		{
		// get location that the brush was turned off at
		x = bx;
		y = by;
		}
	// save window we turned brush on in
    hBrushWnd = hWnd;
    }
else
    {
	if (lpOffRect)
		{
		if ((hWnd != hBrushWnd ||
			!AstralIntersectRect(&rSect, &onRect, lpOffRect)) &&
			(!bBrush2On || hWnd != hBrush2Wnd ||
			!AstralIntersectRect(&rSect, &on2Rect, lpOffRect)))
			return(FALSE);
		}
    hWnd = hBrushWnd;
    hBrushWnd = 0;
    }

// make sure that the window is a valid window
if (!hWnd || !IsWindow(hWnd))  
	{
	// if we are trying to turn off the brush and we can't
	// then make sure the state is set to off
	if (!on)
		bBrushOn = on;
   return (FALSE);
	}

// save last on state
bBrushOn = on;
if ( on )
	{
	// save current mouse location for turning off brush
	bx = x;
	by = y;

	// convert display to file coordinates and get brush info
	Display2File(hWnd,&x, &y);
	ShapeOn = GetMgxBrushShape(Retouch.hBrush);
	SizeOn = GetMgxBrushSize(Retouch.hBrush);
	HalfSize = SizeOn/2;

	// setup rectangle in file coordinates for brush
	if (ShapeOn == IDC_BRUSHCUSTOM ||
	    ShapeOn == IDC_BRUSHSQUARE ||
	    ShapeOn == IDC_BRUSHCIRCLE ||
	    ShapeOn == IDC_BRUSHSLASH ||
	    ShapeOn == IDC_BRUSHBACKSLASH)
		{
		onFRect.left = x - HalfSize;
		onFRect.right = onFRect.left + SizeOn - 1;
		onFRect.top = y - HalfSize;
		onFRect.bottom = onFRect.top + SizeOn - 1;
		}
	else if (ShapeOn == IDC_BRUSHHORIZONTAL)
		{
		onFRect.left = x - HalfSize;
		onFRect.right = onFRect.left + SizeOn - 1;
		onFRect.top = onFRect.bottom = y;
		}
	else if (ShapeOn == IDC_BRUSHVERTICAL)
		{
		onFRect.left = onFRect.right = x;
		onFRect.top = y - HalfSize;
		onFRect.bottom = onFRect.top + SizeOn - 1;
		}

	// convert on rectangle to display coordinates
	File2DispRect(hWnd,&onFRect, &onRect);

	// Handle source brush stuff - for clone only
	bBrush2On = ( Tool.id == IDC_CLONE );

	// adjust deltas for special key modifiers and special modes
	CloneBrush(hWnd, x, y, SizeOn, bBrush2On);

	// calculate rectangle for source brush used brush rect and deltas
	if (bBrush2On)
		{
		on2Rect.left = onFRect.left + cfdx;
		on2Rect.right = onFRect.right + cfdx;
		on2Rect.top = onFRect.top + cfdy;
		on2Rect.bottom = onFRect.bottom + cfdy;
		if (!OPTION3 && BrushMode == BM_NORMAL)
			{
			if (hCloneSrcWnd)
				lpSrcImage = GetImagePtr(hCloneSrcWnd);
			else
				lpSrcImage = GetImagePtr(hWnd);
			ImgGetInfo(lpSrcImage, &npix, &nlin, NULL, NULL);
			MgxBrushMirror(Retouch.hBrush, &on2Rect, &on2Rect, npix, nlin);
			}
		File2DispRectEx(hCloneSrcWnd ? hCloneSrcWnd : hWnd,
						&on2Rect, &on2Rect);
		}
	}
hDC = GetDC(hWnd);
lpDisplay = (LPDISPLAY)GetDisplayPtr(hWnd);
WinBrush( hWnd, hDC, &onRect, bBrushOn, BrushMode == BM_SETSRC, ShapeOn,
		lpDisplay->DispRate );
if (bBrush2On && BrushMode != BM_SETSRC)
	{
	if (hCloneSrcWnd)
		{
		if (IsDoc(hCloneSrcWnd))
			{
			ReleaseDC(hWnd, hDC);
			hWnd = hCloneSrcWnd;
			hDC = GetDC(hWnd);
			}
		else
			hCloneSrcWnd = NULL;
		}
	if (on)
		hBrush2Wnd = hWnd;
    lpDisplay = (LPDISPLAY)GetDisplayPtr(hWnd);
	WinBrush( hWnd, hDC, &on2Rect, bBrushOn, YES, ShapeOn, lpDisplay->DispRate);
	}
ReleaseDC(hWnd, hDC);
return(TRUE);
}

/************************************************************************/
static void GetCloneSrc(HWND hWnd, int x, int y, int BrushSize, LPINT lpX, LPINT lpY)
/************************************************************************/
{
CloneBrush(hWnd, x, y, BrushSize, YES);
*lpX = cfdx + x;
*lpY  = cfdy + y;
}

/************************************************************************/
static void CloneBrush(HWND hWnd, int x, int y, int size, BOOL on)
/************************************************************************/
{
static int bx, by;
static BOOL fLocked;
RECT rMirror;
int	sx, sy, npix, nlin;
LPIMAGE lpSrcImage;

if (on)
	{
	if (hCloneSrcWnd && IsDoc(hCloneSrcWnd))
		{
		if (BrushMode == BM_SETDST)
			{
			cfdx = ptCloneSrc.x - x;
			cfdy = ptCloneSrc.y - y;
			return;
			}
		lpSrcImage = GetImagePtr(hCloneSrcWnd);
		}
	else
		{
		hCloneSrcWnd = NULL;
		lpSrcImage = GetImagePtr(hWnd);
		}
	ImgGetInfo(lpSrcImage, &npix, &nlin, NULL, NULL);
//	if ( !cfdx && !cfdy )
//		cfdx = cfdy = -(2 * size);
	if (OPTION3) // always move source and dest together
		{
		// if trying to reposition, put brush back on image
		sx = bound(x + cfdx, 0, npix-1);
		sy = bound(y + cfdy, 0, nlin-1);
		cfdx = sx - x;
		cfdy = sy - y;
		if (!fLocked)
			{ // ajdust deltas to mirrored position
			rMirror.left = rMirror.right = x + cfdx;
			rMirror.top = rMirror.bottom = y + cfdy;
			MgxBrushMirror(Retouch.hBrush, &rMirror,
					&rMirror, npix, nlin);
			cfdx = rMirror.left - x;
			cfdy = rMirror.top - y;
			fLocked = YES;
			}
		}
	else
		{
		if (fLocked)
			{ // adjust deltas from mirrored position
			rMirror.left = rMirror.right = x + cfdx;
			rMirror.top = rMirror.bottom = y + cfdy;
			if (hCloneSrcWnd)
				lpSrcImage = GetImagePtr(hCloneSrcWnd);
			else
				lpSrcImage = GetImagePtr(hWnd);
			ImgGetInfo(lpSrcImage, &npix, &nlin, NULL, NULL);
			MgxBrushMirror(Retouch.hBrush, &rMirror,
					&rMirror, npix, nlin);
			cfdx = rMirror.left - x;
			cfdy = rMirror.top - y;
			fLocked = NO;
			}
		}
	if ( OPTION1 && !LBUTTON) // freeze source
		{
		// if trying to reposition, put brush back on image
		sx = bound(x + cfdx, 0, npix-1);
		sy = bound(y + cfdy, 0, nlin-1);
		cfdx = sx - x;
		cfdy = sy - y;
		cfdx += (bx - x);
		cfdy += (by - y);
		}
	}
bx = x;
by = y;
}


/************************************************************************/
static void WinBrush( HWND hWnd, HDC hDC, LPRECT lpRect, int flag,
						int fTickMarks, int shape, LFIXED DispRate )
/************************************************************************/
{
int size, size2, size3, y1, y2, x1, x2, dx, dy;
HBRUSH hOldBrush;
short OldROP;

if ( !hDC )
	return;

if (shape == IDC_BRUSHCUSTOM)
	shape = IDC_BRUSHSQUARE;

OldROP = SetROP2(hDC, R2_NOT);
x1 = lpRect->left; y1 = lpRect->top;
x2 = lpRect->right; y2 = lpRect->bottom;
if ( shape == IDC_BRUSHCIRCLE )
	{
	hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(NULL_BRUSH) );
	Ellipse( hDC, x1, y1, x2, y2 );
	SelectObject( hDC, hOldBrush );
	}
else
if ( shape == IDC_BRUSHSQUARE  ||
     shape == IDC_BRUSHHORIZONTAL ||
     shape == IDC_BRUSHVERTICAL)
	{
	MoveTo( hDC, x1, y1 );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	}
else
if ( shape == IDC_BRUSHSLASH )
	{
	size2 = FMUL(2, DispRate);
	size2 = (size2+2)/2;
	size3 = FMUL(3, DispRate);
	size3 = (size3+2)/2;
	MoveTo( hDC, x2-size2, y1 );
	LineTo( hDC, x2-size2+size3, y1+size3 );
	LineTo( hDC, x1+size2, y2+size2 );
	LineTo( hDC, x1+size2-size3, y2+size2-size3 );
	LineTo( hDC, x2-size2, y1 );
	}
else
if ( shape == IDC_BRUSHBACKSLASH )
	{
	size2 = FMUL(2, DispRate);
	size2 = (size2+2)/2;
	size3 = FMUL(3, DispRate);
	size3 = (size3+2)/2;
	MoveTo( hDC, x1+size2, y1 );
	LineTo( hDC, x1+size2-size3, y1+size3 );
	LineTo( hDC, x2-size2, y2+size2 );
	LineTo( hDC, x2-size2+size3, y2+size2-size3 );
	LineTo( hDC, x1+size2, y1 );
	}

if ( !fTickMarks )
	{
	SetROP2(hDC, OldROP);
	return;
	}

dx = 1;
dy = 1;
if (shape == IDC_BRUSHCIRCLE)
	{
	size = x2 - x1 + 1;
	dx = size - ((size * 100) / 141);
	size = y2 - y1 + 1;
	dy = size - ((size * 100) / 141);
	if (dx & 1)
		++dx;
	if (dy & 1)
		++dy;
	dx /= 2;
	dy /= 2;
	}
x1 += dy; y1 += dy;
x2 -= dy; y2 -= dy;
MoveTo( hDC, x1, y1 );
LineTo( hDC, x2, y2 );
MoveTo( hDC, x2, y1 );
LineTo( hDC, x1, y2 );

SetROP2(hDC, OldROP);
}

/************************************************************************/
static BOOL CreateRetouchBrush( ITEMID idTool, ITEMID idFirstTool,
								ITEMID idFirstStyle, FNAME FAR *StyleName )
/************************************************************************/
{
FRMTYPEINFO TypeInfo;
int index;
BOOL fRet = FALSE;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

if (lpImage)
	FrameGetTypeInfo(ImgGetBaseEditFrame(lpImage), &TypeInfo);
else
	FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
if (hActiveWnd)
	DisplayBrush(hActiveWnd, 0, 0, OFF);
DestroyMgxBrush(Retouch.hBrush);
index = idTool - idFirstTool;

if (Retouch.hBrush = CreateMgxBrush(StyleName[index],
	index + idFirstStyle, TypeInfo))
	{
	SetMgxBrushSize(Retouch.hBrush, Retouch.BrushSize);
	SetMgxBrushShape(Retouch.hBrush, Retouch.BrushShape, Names.CustomBrush);
	SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
	SetMgxBrushPressure(Retouch.hBrush, Retouch.Pressure);
	SetMgxBrushMergeMode(Retouch.hBrush, Retouch.MergeMode);
//	Retouch.MergeMode = GetMgxBrushMergeMode(Retouch.hBrush, NULL);
    SetMgxBrushFeather(Retouch.hBrush, Retouch.Feather);
	fRet = TRUE;
	}
if (hActiveWnd && Window.hCursor == Window.hNullCursor)
	DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
return(fRet);
}

/************************************************************************/
static BOOL CreatePaintOnBrush(LPCOLORINFO lpColor, FRMDATATYPE Type)
/************************************************************************/
{
	BOOL fRet = FALSE;

	HWND hActiveWnd = GetActiveDoc();
	if (hActiveWnd)
		DisplayBrush(0, 0, 0, OFF);
	DestroyMgxBrush(Retouch.hBrush);
	Retouch.hBrush = CreateMgxColorBrush(lpColor, Retouch.Feather);
	if (Retouch.hBrush)
	{
		SetMgxBrushSize(Retouch.hBrush, Retouch.BrushSize);
		SetMgxBrushShape(Retouch.hBrush, Retouch.BrushShape, Names.CustomBrush);
		if (Type != FDT_LINEART && Type != FDT_PALETTECOLOR)
			SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
		fRet = TRUE;
	}

	if (hActiveWnd && Window.hCursor == Window.hNullCursor)
		DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
	return(fRet);
}

// LOCAL prototypes
LOCAL BOOL CreateBrush_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void CreateBrush_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
BOOL CreateBrush_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest, UINT msg);

static int CreateBrush_index;
static WORD CreateBrush_wType;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCreateBrushProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, CreateBrush_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, CreateBrush_OnCommand);

	HANDLE_DLGRET(hDlg, WM_SETCURSOR, CreateBrush_OnSetCursor);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CREATEBRUSH);
	}
}

/************************************************************************/
LOCAL BOOL CreateBrush_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	BOOL fUntitled;
	FRMTYPEINFO TypeInfo;
	LPIMAGE lpImage = GetActiveImage();

	CenterPopup( hDlg );

	CreateBrush_index = idCurTool - idCurFirstTool;
	CreateBrush_wType = CreateBrush_index + idCurFirstStyle;
	lstrcpy(Names.BrushStyle, CurStyleName[CreateBrush_index]);

	fUntitled = NO;
	if (lpImage)
		ImgGetTypeInfo(lpImage, &TypeInfo);
	else
		FrameSetTypeInfo(&TypeInfo, FDT_NONE);
	lpBrush = (LPMGXBRUSH)CreateMgxBrush(Names.BrushStyle, CreateBrush_wType, TypeInfo);
	if (!lpBrush)
	{
		lpBrush = CreateDefaultBrush();
		if (lpBrush)
		{
			fUntitled = YES;
			InitToolBrush(lpBrush, idCurTool);
		}
	}

//	lpBrush = CopyMgxBrush(Retouch.hBrush);
	if (!lpBrush)
	{
		AstralDlgEnd( hDlg, FALSE );
		return( TRUE );
	}

	SetMgxBrushSize((HMGXBRUSH)lpBrush, Retouch.BrushSize);
	SetMgxBrushShape((HMGXBRUSH)lpBrush, Retouch.BrushShape, Names.CustomBrush);
	SetMgxBrushOpacity((HMGXBRUSH)lpBrush, Retouch.Opacity);
	SetMgxBrushPressure((HMGXBRUSH)lpBrush, Retouch.Pressure);
	SetMgxBrushMergeMode((HMGXBRUSH)lpBrush, Retouch.MergeMode);
	SetMgxBrushFeather((HMGXBRUSH)lpBrush, Retouch.Feather);

	InitExtName( hDlg, IDC_STYLES, IDC_FILENAME, Names.BrushStyle, CreateBrush_wType );

	InitDlgItemSpin( hDlg, IDC_SPACING, lpBrush->Spacing, NO,
		0, 999 );
	InitDlgItemSpin( hDlg, IDC_FADESIZE, lpBrush->FadeSize, NO,
		0, 100 );
	CheckDlgButton( hDlg, IDC_BRUSHSCATTER, lpBrush->Style == IDC_BRUSHSCATTER);
	CheckDlgButton( hDlg, IDC_BRUSHSOLID, lpBrush->Style == IDC_BRUSHSOLID);
	InitDlgItemSpin( hDlg, IDC_STYLEPRESSURE, TOPERCENT(lpBrush->StylePressure),
				 	NO, 0, 100 );
	ControlEnable(hDlg, IDC_STYLEPRESSURE, lpBrush->Style == IDC_BRUSHSCATTER);
	CheckDlgButton( hDlg, IDC_OVERLAP, !lpBrush->NoOverlap);
	ControlEnable(hDlg, IDC_OVERLAP,
			lpBrush->SourceFunction == SF_SOLID_COLOR ||
			lpBrush->SourceFunction == SF_COPY_IMAGE);
	InitDlgItemSpin( hDlg, IDC_SMOOTHNESS, lpBrush->Smoothness, NO,
		0, 100 );
	HandleMergeComboEx(hDlg, IDC_MERGEMODE, TRUE, lpBrush->MergeMode, 0L, TRUE);
	if (GetDlgItem(hDlg, IDC_EXTNAMES))
	{
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpBrush->szSourceExtName,
					IDN_TEXTURE );
		lstrcpy( Names.Saved, lpBrush->szSourceExtName );
		CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, lpBrush->SourceFlipHorz);
		CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, lpBrush->SourceFlipVert);
		lpBrush->SourceFlipMode = SFM_TILE;
	}
	SetupBrushPreview(hDlg);
	SetupPreviewCursor(hDlg, YES);
	return(TRUE);
}

/************************************************************************/
BOOL CreateBrush_OnSetCursor(HWND hDlg, HWND hWndCursor, UINT codeHitTest,
							UINT msg)
/************************************************************************/
{
    HWND hControl;
    CPoint pt;
    HWND hChild;

    // check for disabled control
    GetCursorPos( &pt );
    if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
    {
        hWndCursor = hChild;
    }

    if (hControl = GetDlgItem(hDlg, IDC_BRUSHPREVIEW))
    {
	    if (hControl != hWndCursor)
		    DisplayPreviewBrush(hControl, 0, 0, OFF);
    }
    return( SetupCursor( hWndCursor, codeHitTest, IDD_CREATEBRUSH ));
}

/************************************************************************/
LOCAL void CreateBrush_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int i;
MERGE_MODE MergeMode;
FNAME szFileName;

switch (id)
	{
	case IDC_STYLES:
	GetExtName( hDlg, id, IDC_FILENAME,
		Names.BrushStyle, CreateBrush_wType, codeNotify );
	break;

	case IDC_SPACING:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	lpBrush->Spacing = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_FADESIZE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	lpBrush->FadeSize = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_FADEOPACITY:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	lpBrush->FadeOpacity = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_MINTRANSPARENCY:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	lpBrush->OpacityScale = 100-GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

//	case IDC_BRUSHSTYLE:
	case IDC_BRUSHSOLID:
	case IDC_BRUSHSCATTER:
//	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
//		break;
	lpBrush->Style = id;
	CheckDlgButton( hDlg, IDC_BRUSHSCATTER,
			lpBrush->Style == IDC_BRUSHSCATTER);
	CheckDlgButton( hDlg, IDC_BRUSHSOLID,
			lpBrush->Style == IDC_BRUSHSOLID);
	ControlEnable(hDlg, IDC_STYLEPRESSURE, lpBrush->Style == IDC_BRUSHSCATTER);
	break;

	case IDC_STYLEPRESSURE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	lpBrush->StylePressure = TOGRAY(i);
	break;

	case IDC_OVERLAP:
	lpBrush->NoOverlap = !lpBrush->NoOverlap;
	CheckDlgButton( hDlg, IDC_OVERLAP, !lpBrush->NoOverlap);
	break;

	case IDC_SMOOTHNESS:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	lpBrush->Smoothness = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeComboEx( hDlg, id, NO, MM_NORMAL, codeNotify, YES )) < 0)
		break;
	lpBrush->MergeMode = MergeMode;
	break;

	case IDC_TEXTUREFLIPX:
	lpBrush->SourceFlipHorz = !lpBrush->SourceFlipHorz;
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, lpBrush->SourceFlipHorz);
	break;

	case IDC_TEXTUREFLIPY:
	lpBrush->SourceFlipVert = !lpBrush->SourceFlipVert;
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, lpBrush->SourceFlipVert);
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpBrush->szSourceExtName,
		IDN_TEXTURE, codeNotify );
	if (lpBrush->SourceFrame)
		{
		FrameClose(lpBrush->SourceFrame);
		lpBrush->SourceFrame = NULL;
		}
	lpBrush->szSource[0] = '\0';
	LookupExtFile(lpBrush->szSourceExtName, lpBrush->szSource,
				 IDN_TEXTURE);
	lstrcpy(Names.BrushStyle, lpBrush->szSourceExtName);
	SetDlgItemText(hDlg, IDC_STYLES, Names.BrushStyle);
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_DELETEEXT:
	case IDC_ADDEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				lpBrush->szSourceExtName,
				IDN_TEXTURE, id, NO );
	break;

	case IDC_PREVIEWRESET:
	if (!lpPreviewFrame)
		break;
	FrameClose(lpPreviewFrame);
	SetupBrushPreview(hDlg);
	DisplayPreviewBrush(GetDlgItem(hDlg, IDC_BRUSHPREVIEW), 0, 0, OFF);
	AstralControlRepaint(hDlg, id);
	UpdateWindow(GetDlgItem(hDlg, IDC_BRUSHPREVIEW));
	DisplayPreviewBrush(GetDlgItem(hDlg, IDC_BRUSHPREVIEW), INT_MAX, INT_MAX, ON);
	break;

	case IDOK:
	if ( LookupExtFile(Names.BrushStyle, szFileName, CreateBrush_wType) )
		{
		if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			(LPTR)Names.BrushStyle) == IDCANCEL)
			break;
		}
	else
		{
		if ( !CreateFileName( Names.BrushStyle, CreateBrush_wType, szFileName ) )
			break;
		}
	if (!WriteMgxBrush((HMGXBRUSH)lpBrush, szFileName))
		break;
	if (!AddExtFile(Names.BrushStyle, szFileName, CreateBrush_wType))
		{
		FileDelete(szFileName);
		break;
		}
	SetupPreviewCursor(hDlg, NO);
	DestroyMgxBrush((HMGXBRUSH)lpBrush);
	if (lpPreviewFrame)
		{
		AttachPreviewFrame(hDlg, IDC_BRUSHPREVIEW, NULL);
		FrameClose(lpPreviewFrame);
		}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	SetupPreviewCursor(hDlg, NO);
	DestroyMgxBrush((HMGXBRUSH)lpBrush);
	if (lpPreviewFrame)
		{
		AttachPreviewFrame(hDlg, IDC_BRUSHPREVIEW, NULL);
		FrameClose(lpPreviewFrame);
		}
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

/***********************************************************************/
static void InitToolBrush(LPMGXBRUSH lpBrush, ITEMID idCurTool)
/***********************************************************************/
{
switch (idCurTool)
	{
	case IDC_PAINT:
	break;

	case IDC_SPRAY:
	lpBrush->Timer = 100;
	break;
	
	case IDC_CLONE:
	lpBrush->SourceFunction = SF_COPY_IMAGE;
	break;

	case IDC_TEXTURE:
	lpBrush->SourceData = SD_FILE;
	lpBrush->SourceFunction = SF_COPY_IMAGE;
	break;

	case IDC_SMEAR:
	lpBrush->SourceFunction = SF_SMEAR_IMAGE;
	break;

	case IDC_SHARP:
	lpBrush->SourceFunction = SF_SHARPEN_IMAGE;
	break;

	case IDC_SMOOTH:
	lpBrush->SourceFunction = SF_SMOOTH_IMAGE;
	break;

	case IDC_LIGHTEN:
	lpBrush->SourceFunction = SF_LIGHTEN_IMAGE;
	break;

	case IDC_DARKEN:
	lpBrush->SourceFunction = SF_DARKEN_IMAGE;
	break;

	}
}

static ITEMID idStrokeTool, idStrokeFirstTool, idStrokeFirstStyle;
static FNAME *StrokeStyleName;
static int idStrokeAmount;

// LOCAL prototypes
LOCAL BOOL MaskStroke_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void MaskStroke_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static int SaveBrushCurrent;
static HMGXBRUSH SaveBrushHandle;

/***********************************************************************/
BOOL WINPROC EXPORT DlgMaskStrokeProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, MaskStroke_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, MaskStroke_OnCommand);

	default:
	rd.idTool = idStrokeTool;
	rd.idFirstTool = idStrokeFirstTool;
	rd.idFirstStyle = idStrokeFirstStyle;
	rd.idCreateDlg = 0;
	rd.fIsRibbon = NO;
	rd.StyleName = StrokeStyleName;
	return( HandleRetouchDialog( hDlg, msg, wParam, lParam, &rd));
	}
}

/************************************************************************/
LOCAL BOOL MaskStroke_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();

CenterPopup( hDlg );

CheckComboItem( hDlg, IDC_STROKETOOLS, IDC_STROKEFIRST,
	IDC_STROKELAST, Mask.StrokeTool );
if (hActiveWnd)
	DisplayBrush(hActiveWnd, 0, 0, OFF);
SaveBrushHandle = Retouch.hBrush;
Retouch.hBrush = NULL;
SaveBrushCurrent = RetouchBrushCurrent;
SelectRetouchBrush(STROKEMASK_BRUSH);
SetupStrokeTool(hDlg, Mask.StrokeTool);
rd.idTool = idStrokeTool;
rd.idFirstTool = idStrokeFirstTool;
rd.idFirstStyle = idStrokeFirstStyle;
rd.idCreateDlg = 0;
rd.fIsRibbon = NO;
rd.StyleName = StrokeStyleName;
RetouchInit(hDlg, hWndFocus, lParam, &rd);
HandleRetouch_HandleEnable(hDlg, 0);
return( TRUE );
}

/************************************************************************/
LOCAL void MaskStroke_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int i;
BOOL Bool;
LPMASK lpMask;
LPFRAME lpFrame;
HWND hActiveWnd;
STROKEMASK_PARMS parms;
LPIMAGE lpImage = NULL;


if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);

switch (id)
	{
	case IDC_STROKETOOLS:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Mask.StrokeTool = id;
	SetupStrokeTool(hDlg, Mask.StrokeTool);
	rd.idTool = idStrokeTool;
	rd.idFirstTool = idStrokeFirstTool;
	rd.idFirstStyle = idStrokeFirstStyle;
	rd.idCreateDlg = 0;
	rd.fIsRibbon = NO;
	rd.StyleName = StrokeStyleName;
	RetouchInit(hDlg, NULL, 0, &rd);
	break;
 
	case IDC_STROKEAMOUNT:
	switch (idStrokeAmount)
		{	
 		case 0:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, id, &Bool, NO );
		Retouch.Opacity = 255-TOGRAY(i);
		SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
		SetCommonOpacity(Retouch.Opacity);
		break;

   	case 1:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, id, &Bool, NO );
		Retouch.Opacity = TOGRAY(i);
		SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
		SetCommonPressure(Retouch.Opacity);
		break;

   	case 2:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, id, &Bool, NO );
		Retouch.Pressure = TOGRAY(i);
		SetMgxBrushPressure(Retouch.hBrush, Retouch.Pressure);
		SetCommonPressure(Retouch.Pressure);
		break;
		}
	break;

	case IDOK:
	if (!lpImage)
		break;
	if (!(lpMask = ImgGetMask(lpImage)))
		break;
	if (!(lpFrame = lpMask->Pixmap.EditFrame))
		break;
	if (!Retouch.hBrush)
		break;

	parms.idBrush = Mask.StrokeTool;
	i = idStrokeTool - idStrokeFirstTool;
	lstrcpy(parms.StyleName, StrokeStyleName[i]);
	parms.BrushShape = Retouch.BrushShape;
	if (Retouch.BrushShape == IDC_BRUSHCUSTOM)
		lstrcpy(parms.ShapeName, Names.CustomBrush);
	else
		parms.ShapeName[0] = '\0';
	parms.iBaseRes = FrameResolution(ImgGetBaseEditFrame(lpImage));		
	parms.Size = Retouch.BrushSize;
	parms.Feather = Retouch.Feather;
	parms.Opacity = Retouch.Opacity;
	parms.MergeMode = Retouch.MergeMode;
	parms.Pressure = Retouch.Pressure;
	GetActiveColor(&parms.Color);

	SelectRetouchBrush(SaveBrushCurrent);
	if (Retouch.hBrush)
		DestroyMgxBrush(Retouch.hBrush);
	Retouch.hBrush = SaveBrushHandle;
	SaveToolPreferences(IDC_STROKEMASK);

	ProcessCommand(lpImage->lpCmdList, IDS_CMD_STROKEMASK, &parms);

	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	SelectRetouchBrush(SaveBrushCurrent);
	if (Retouch.hBrush)
		DestroyMgxBrush(Retouch.hBrush);
	Retouch.hBrush = SaveBrushHandle;
	AstralDlgEnd( hDlg, FALSE );
	break;
	
	default:
	rd.idTool = idStrokeTool;
	rd.idFirstTool = idStrokeFirstTool;
	rd.idFirstStyle = idStrokeFirstStyle;
	rd.idCreateDlg = 0;
	rd.fIsRibbon = NO;
	rd.StyleName = StrokeStyleName;
	RetouchCommand(hDlg, id, hControl, codeNotify, &rd);
	break;
   }
}

/***********************************************************************/
static void SetupStrokeTool(HWND hDlg, ITEMID idTool)
/***********************************************************************/
{
STRING szString;

switch (idTool)
	{
	case IDC_SPRAYTOOL:
	idStrokeTool = IDC_SPRAY;
 	idStrokeFirstTool = IDC_FIRST_RETOUCHTOOL;
	idStrokeFirstStyle = IDN_FIRST_RETOUCHSTYLE;
	StrokeStyleName = Retouch.RetouchStyleName;
	idStrokeAmount = 1;
	break;

	case IDC_TEXTURETOOL:
	idStrokeTool = IDC_TEXTURE;
 	idStrokeFirstTool = IDC_FIRST_RETOUCHTOOL;
	idStrokeFirstStyle = IDN_FIRST_RETOUCHSTYLE;
	StrokeStyleName = Retouch.RetouchStyleName;
	idStrokeAmount = 0;
	break;

	case IDC_SMEARTOOL:
	idStrokeTool = IDC_SMEAR;
 	idStrokeFirstTool = IDC_FIRST_RETOUCHTOOL;
	idStrokeFirstStyle = IDN_FIRST_RETOUCHSTYLE;
	StrokeStyleName = Retouch.RetouchStyleName;
	idStrokeAmount = 1;
	break;

	case IDC_SHARPENTOOL:
	idStrokeTool = IDC_SHARP;
 	idStrokeFirstTool = IDC_FIRST_FILTERTOOL;
	idStrokeFirstStyle = IDN_FIRST_FILTERSTYLE;
	StrokeStyleName = Retouch.FilterStyleName;
	idStrokeAmount = 2;
	break;

	case IDC_SMOOTHTOOL:
	idStrokeTool = IDC_SMOOTH;
 	idStrokeFirstTool = IDC_FIRST_FILTERTOOL;
	idStrokeFirstStyle = IDN_FIRST_FILTERSTYLE;
	StrokeStyleName = Retouch.FilterStyleName;
	idStrokeAmount = 2;
	break;

	case IDC_LIGHTENTOOL:
	idStrokeTool = IDC_LIGHTEN;
 	idStrokeFirstTool = IDC_FIRST_FILTERTOOL;
	idStrokeFirstStyle = IDN_FIRST_FILTERSTYLE;
	StrokeStyleName = Retouch.FilterStyleName;
	idStrokeAmount = 2;
	break;
 
	case IDC_DARKENTOOL:
	idStrokeTool = IDC_DARKEN;
 	idStrokeFirstTool = IDC_FIRST_FILTERTOOL;
	idStrokeFirstStyle = IDN_FIRST_FILTERSTYLE;
	StrokeStyleName = Retouch.FilterStyleName;
	idStrokeAmount = 2;
	break;

	case IDC_PAINTTOOL:
	default:
	idStrokeTool = IDC_PAINT;
 	idStrokeFirstTool = IDC_FIRST_RETOUCHTOOL;
	idStrokeFirstStyle = IDN_FIRST_RETOUCHSTYLE;
	StrokeStyleName = Retouch.RetouchStyleName;
	idStrokeAmount = 0;
	break;
	}
switch (idStrokeAmount)
	{
	case 0:
	GetDlgItemText(hDlg, IDC_TRANSPARENCYLBL, szString, sizeof(szString));
	InitDlgItemSpin( hDlg, IDC_STROKEAMOUNT, TOPERCENT(255-Retouch.Opacity),
		NO, 0, 99 );
	break;
	case 1:
	GetDlgItemText(hDlg, IDC_PRESSURELBL, szString, sizeof(szString));
	InitDlgItemSpin( hDlg, IDC_STROKEAMOUNT, TOPERCENT(Retouch.Opacity),
		NO, 0, 100 );
	break;
	case 2:
	GetDlgItemText(hDlg, IDC_PRESSURELBL, szString, sizeof(szString));
	InitDlgItemSpin( hDlg, IDC_STROKEAMOUNT, TOPERCENT(Retouch.Pressure),
		NO, 0, 100 );
	break;
	}
SetDlgItemText(hDlg, IDC_STROKEAMOUNTLBL, szString);
}

/***********************************************************************/
void RetouchInit(HWND hDlg, HWND hWndFocus, LPARAM lParam, LPRETOUCHDLG lpRD)
/***********************************************************************/
{
int index;

idCurTool = lpRD->idTool;
idCurFirstTool = lpRD->idFirstTool;
idCurFirstStyle = lpRD->idFirstStyle;
CurStyleName = lpRD->StyleName;

BrushMode = BM_NORMAL;
if (GetDlgItem(hDlg, IDC_STYLES))
	{
	index = lpRD->idTool - lpRD->idFirstTool;
	InitExtName( hDlg, IDC_STYLES, IDC_FILENAME,
		lpRD->StyleName[index], index + lpRD->idFirstStyle );
	}

SetDlgItemText( hDlg, IDC_BRUSHCUSTOM, Names.CustomBrush );
ControlEnable( hDlg,  IDC_BRUSHCUSTOM, Names.CustomBrush[0] != 0 );
CheckComboItem( hDlg, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE,
	IDC_BRUSHCUSTOMNEW, Retouch.BrushShape );

InitDlgItemSpin( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, NO,
	1, MAX_BRUSH_SIZE );
InitDlgItemSpin( hDlg, IDC_FEATHER, Retouch.Feather, NO,
	0, 100 );
if (GetDlgItem(hDlg, IDC_PRESSURE))
	{
	InitDlgItemSpin( hDlg, IDC_PRESSURE, TOPERCENT(Retouch.Pressure),
		NO, 0, 100 );
	}
if (GetDlgItem(hDlg, IDC_OPACITYPRESSURE))
	InitDlgItemSpin( hDlg, IDC_OPACITYPRESSURE, TOPERCENT(Retouch.Opacity),
		NO, 0, 100 );
if (GetDlgItem(hDlg, IDC_TRANSPARENT))
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Retouch.Opacity),
		NO, 0, 99 );
CreateRetouchBrush( lpRD->idTool, lpRD->idFirstTool, lpRD->idFirstStyle, lpRD->StyleName );
}

/***********************************************************************/
void RetouchEnable(HWND hDlg, int fFlags, LPRETOUCHDLG lpRD)
/***********************************************************************/
{
FRMDATATYPE Type = FDT_RGBCOLOR;
LPIMAGE lpImage = GetActiveImage();
BOOL fReadOnly;
MERGE_MODE MergeMode;

if (lpImage)
	lpImage->GetInfo(NULL, NULL, NULL, &Type);

if (GetDlgItem(hDlg, IDC_CLONESOURCE))
	CheckDlgButton(hDlg, IDC_CLONESOURCE, BrushMode == BM_SETSRC);
MergeMode = GetMgxBrushMergeMode(Retouch.hBrush, &fReadOnly);
HandleMergeCombo(hDlg, IDC_MERGEMODE, TRUE, MergeMode, 0L);
ControlEnable(hDlg, IDC_MERGEMODE, !fReadOnly && CANDOMERGEMODE(Type));
if (GetDlgItem(hDlg, IDC_TRANSPARENT))
	ControlEnable( hDlg, IDC_TRANSPARENT, CANDOTRANSPARENCY(Type) );
if (GetDlgItem(hDlg, IDC_OPACITYPRESSURE))
	ControlEnable( hDlg, IDC_OPACITYPRESSURE, CANDOTRANSPARENCY(Type) );
if (GetDlgItem(hDlg, IDC_STROKEAMOUNT))
	ControlEnable( hDlg, IDC_STROKEAMOUNT, CANDOTRANSPARENCY(Type) );
ControlEnable(hDlg, IDC_FEATHER, (Retouch.BrushShape == IDC_BRUSHCIRCLE ||
								Retouch.BrushShape == IDC_BRUSHSQUARE) && CANDOTRANSPARENCY(Type));
ControlEnable(hDlg, IDC_BRUSHSIZE, Retouch.BrushShape != IDC_BRUSHCUSTOM);
if (!Retouch.hBrush)
	CreateRetouchBrush( lpRD->idTool, lpRD->idFirstTool, lpRD->idFirstStyle, lpRD->StyleName );
}

/***********************************************************************/
BOOL RetouchCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify,
						LPRETOUCHDLG lpRD)
/***********************************************************************/
{
BOOL Bool;
int i, index, NewShape;
MERGE_MODE MergeMode;
HWND hActiveWnd;

hActiveWnd = GetActiveDoc();

switch (id)
    {
	case IDC_PAINT:
	case IDC_SPRAY:
	case IDC_CLONE:
	case IDC_TEXTURE:
	case IDC_SMEAR:
	case IDC_SHARP:
	case IDC_SMOOTH:
	case IDC_LIGHTEN:
	case IDC_DARKEN:
	case IDC_ERASER:
	case IDC_DRAWSQUARE:
	case IDC_DRAWFREE:
	case IDC_PENCIL:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

   case IDC_STYLES:
	index = lpRD->idTool - lpRD->idFirstTool;
	if (GetExtName( hDlg, id, IDC_FILENAME, lpRD->StyleName[index],
		index + lpRD->idFirstStyle, codeNotify ) >= 0)
			{
			CreateRetouchBrush( lpRD->idTool, lpRD->idFirstTool, lpRD->idFirstStyle, lpRD->StyleName );
			SendMessage(hDlg, WM_CONTROLENABLE, 0, 0L);
			}
	break;

    case IDC_BRUSHSHAPE:
//	    case IDC_BRUSHCIRCLE:
//	    case IDC_BRUSHSQUARE:
//	    case IDC_BRUSHHORIZONTAL:
//	    case IDC_BRUSHVERTICAL:
//	    case IDC_BRUSHSLASH:
//	    case IDC_BRUSHBACKSLASH:
//	    case IDC_BRUSHCUSTOM:
//	    case IDC_BRUSHCUSTOMNEW:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	NewShape = id;
	if ( NewShape == IDC_BRUSHCUSTOMNEW )
		{
		NewShape = IDC_BRUSHCUSTOM;
		if (!AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_CUSTOMBRUSH,
		     DlgCustomBrushProc ))
			NewShape = Retouch.BrushShape;
		SetDlgItemText( hDlg, IDC_BRUSHCUSTOM, Names.CustomBrush );
		if (Names.CustomBrush[0] == 0)
			NewShape = Retouch.BrushShape;
		ControlEnable(hDlg, IDC_BRUSHCUSTOM, Names.CustomBrush[0] != 0);
		}
	if (hActiveWnd)
    	DisplayBrush(hActiveWnd, 0, 0, OFF);
	if (SetMgxBrushShape(Retouch.hBrush, NewShape, Names.CustomBrush))
		Retouch.BrushShape = NewShape;
	SetCommonBrushShape(Retouch.BrushShape);
	CheckComboItem( hDlg, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE,
		IDC_BRUSHCUSTOMNEW, Retouch.BrushShape);
	Retouch.BrushSize = GetMgxBrushSize(Retouch.hBrush);
	SetDlgItemSpin( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, NO);
	SendMessage(hDlg, WM_CONTROLENABLE, 0, 0L);
    if (hActiveWnd && Window.hCursor == Window.hNullCursor)
		DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
	break;

    case IDC_BRUSHSIZE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	if (hActiveWnd)
   		DisplayBrush(hActiveWnd, 0, 0, OFF);
	Retouch.BrushSize = GetDlgItemSpin( hDlg, IDC_BRUSHSIZE,
		&Bool, NO );
	if (Retouch.BrushSize > MAX_BRUSH_SIZE)
		Retouch.BrushSize = MAX_BRUSH_SIZE;
	if (Retouch.BrushSize < 1)
		Retouch.BrushSize = 1;
	SetMgxBrushSize(Retouch.hBrush, Retouch.BrushSize);
	SetCommonBrushSize(Retouch.BrushSize);
	if (hActiveWnd && Window.hCursor == Window.hNullCursor)
		DisplayBrush(hActiveWnd, INT_MAX, INT_MAX, ON);
	break;

    case IDC_FEATHER:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Retouch.Feather = GetDlgItemSpin( hDlg, IDC_FEATHER,
		&Bool, NO );
	SetMgxBrushFeather(Retouch.hBrush, Retouch.Feather);
	SetCommonFeather(Retouch.Feather);
	break;

	case IDC_MERGEMODE:
	if ((MergeMode = HandleMergeCombo( hDlg, id, NO, MM_NORMAL, codeNotify )) < 0)
		break;
	Retouch.MergeMode = MergeMode;
	SetMgxBrushMergeMode(Retouch.hBrush, Retouch.MergeMode);
	SetCommonMergeMode(Retouch.MergeMode);
	break;

    case IDC_TRANSPARENT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Retouch.Opacity = 255-TOGRAY(i);
	SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
	SetCommonOpacity(Retouch.Opacity);
	break;

    case IDC_PRESSURE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Retouch.Pressure = TOGRAY(i);
	SetMgxBrushPressure(Retouch.hBrush, Retouch.Pressure);
	SetCommonPressure(Retouch.Pressure);
	break;

    case IDC_OPACITYPRESSURE:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	i = GetDlgItemSpin( hDlg, id, &Bool, NO );
	Retouch.Opacity = TOGRAY(i);
	SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
	SetCommonPressure(Retouch.Opacity);
	break;

    case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.TextureName,
		IDN_TEXTURE, codeNotify );
	break;

	case IDC_EXTMANAGE:
    case IDC_BRUSHEXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_DELETEEXT:
	if (Retouch.hBrush &&
		((LPMGXBRUSH)Retouch.hBrush)->Protection & (PS_READONLY|PS_NODELETE))
		{
		Message(IDS_BRUSH_NODELETE);
		break;
		}
   case IDC_ADDEXT:
   case IDC_RENAMEEXT:
	index = lpRD->idTool - lpRD->idFirstTool;
	if (id == IDC_ADDEXT && lpRD->idCreateDlg)
		{
		AstralDlg( NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), lpRD->idCreateDlg, DlgCreateBrushProc );
		index = lpRD->idTool - lpRD->idFirstTool;
		lstrcpy(lpRD->StyleName[index], Names.BrushStyle);
		InitExtName( hDlg, IDC_STYLES, IDC_FILENAME,
				lpRD->StyleName[index], index + lpRD->idFirstStyle );
		CreateRetouchBrush( lpRD->idTool, lpRD->idFirstTool, lpRD->idFirstStyle,
				 lpRD->StyleName );
		SendMessage(hDlg, WM_CONTROLENABLE, 0, 0L);
		}
	else
		{
		ExtNameManager( hDlg, IDC_STYLES, IDC_FILENAME,
			lpRD->StyleName[index], index + lpRD->idFirstStyle, id, NO );
		CreateRetouchBrush( lpRD->idTool, lpRD->idFirstTool, lpRD->idFirstStyle,
				 lpRD->StyleName );
		}
	break;

	case IDC_CLONESOURCE:
	if (BrushMode == BM_NORMAL)
		BrushMode = BM_SETSRC;
	else
		BrushMode = BM_NORMAL;
	CheckDlgButton(hDlg, id, BrushMode != BM_NORMAL);
	break;

    default:
	return( FALSE );
    }
return(TRUE);
}

/************************************************************************/
static void AttachPreviewFrame( HWND hDlg, ITEMID id, LPFRAME lpFrame )
/************************************************************************/
{
HWND hControl;

// Get the handle to the preview image control
if ( !(hControl = GetDlgItem( hDlg, id )) )
	return;

// Link the passed in frame to the sample image control
SetWindowLong( hControl, GWL_IMAGE, (long)lpFrame );
SetWindowLong( hControl, GWL_IMAGE_TOOLPROC, (long)BrushPreviewToolProc );
InvalidateRect( hControl, NULL, NO );
}

/************************************************************************/
static BOOL BrushPreviewToolProc( HWND hWindow, HDC hDC, int x, int y, UINT msg )
/************************************************************************/
{
static BOOL fActive;

switch (msg)
	{
	case WM_LBUTTONDOWN:
	if (fActive)
		break;
	UpdateWindow( hWindow );
	fActive = NO;
	if (!lpPreviewFrame)
		break;
	PixmapSetup(&PreviewPixmap, lpPreviewFrame, YES);
	if (!MgxBrushBeginEx(hWindow, &PreviewPixmap, 0, 0, 
					(HMGXBRUSH)lpBrush, YES, YES, UpdateBrushPreview, NULL))
	    break;
	fActive = MgxBrushActive();

	/* fall through to WM_MOUSEMOVE */
    case WM_MOUSEMOVE:	// sent when ToolActive is on
	DisplayPreviewBrush(hWindow, x, y, OFF);
	if (fActive)
		MgxBrushStroke(x, y, x, y, x, y);
	DisplayPreviewBrush(hWindow, x, y, ON);
	break;
	
	case WM_LBUTTONUP:
	if (!fActive)
		break;
	MgxBrushEnd(NO);
    if( Control.UseTablet )
        FlushTabletQueue( ALL_PACKETS );
	fActive = NO;
	break;
	}
return(TRUE);
}

/************************************************************************/
static void UpdateBrushPreview(HWND hWnd, LPRECT lpUpdateRect, BOOL fUpdateCursor)
/************************************************************************/
{
LPFRAME lpFrame;
HDC hDC;
RECT rClient;
int x, y;
BOOL fTurnOn;

if (fUpdateCursor)
	{
	DisplayPreviewBrush(hWnd, 0, 0, OFF);
	x = lpUpdateRect->left;
	y = lpUpdateRect->top;
	DisplayPreviewBrush(hWnd, x, y, ON);
	}
else
	{
	fTurnOn = DisplayPreviewBrush(hWnd, 0, 0, OFF);
	hDC = GetDC(hWnd);
	lpFrame = (LPFRAME)GetWindowLong( hWnd, GWL_IMAGE );
	GetClientRect(hWnd, &rClient);
	WindowsToAstralRect(&rClient);
	DrawFrameEx(lpFrame, hDC, &rClient, &rClient, lpUpdateRect);
	ReleaseDC(hWnd, hDC);
	if (fTurnOn)
		DisplayPreviewBrush(hWnd, INT_MAX, INT_MAX, ON);
	}
}

/************************************************************************/
static void FillPreviewFrame(LPFRAME lpFrame)
/************************************************************************/
{
int dx, dy, y, iCount, dxCount, xleft;
RGBS rgb, c1, c2, c3;
LPTR lp;

FrameSetBackground(lpFrame, 255);
if (!Control.UseBrushEditColor)
	return;
dy = FrameYSize(lpFrame);
dx = FrameXSize(lpFrame);
dy /= 2;
y = dy;
iCount = dy / 3;
dy -= iCount;
c1.red = 255; c1.green = 0; c1.blue = 0;
c2.red = 0; c2.green = 255; c2.blue = 0;
c3.red = 0; c3.green = 0; c3.blue = 255;
while (--iCount >= 0)
	{
	lp = FramePointer(lpFrame, 0, y, YES);
	if (lp)
		{
		xleft = dx;
		dxCount = xleft / 3;
		FrameSetRGB(lpFrame, &c1, lp, dxCount);
		lp += (dxCount * 3);
		xleft -= dxCount;
		dxCount = xleft / 2;
		FrameSetRGB(lpFrame, &c2, lp, dxCount);
		lp += (dxCount * 3);
		xleft -= dxCount;
		dxCount = xleft / 1;
		FrameSetRGB(lpFrame, &c3, lp, dxCount);
		}
	++y;
	}
iCount = dy / 2;
dy -= iCount;
c1.red = 0; c1.green = 255; c1.blue = 255;
c2.red = 255; c2.green = 0; c2.blue = 255;
c3.red = 255; c3.green = 255; c3.blue = 0;
while (--iCount >= 0)
	{
	lp = FramePointer(lpFrame, 0, y, YES);
	if (lp)
		{
		xleft = dx;
		dxCount = xleft / 3;
		FrameSetRGB(lpFrame, &c1, lp, dxCount);
		lp += (dxCount * 3);
		xleft -= dxCount;
		dxCount = xleft / 2;
		FrameSetRGB(lpFrame, &c2, lp, dxCount);
		lp += (dxCount * 3);
		xleft -= dxCount;
		dxCount = xleft / 1;
		FrameSetRGB(lpFrame, &c3, lp, dxCount);
		}
	++y;
	}
iCount = dy / 1;
rgb.red = rgb.green = rgb.blue = 0;
while (--iCount >= 0)
	{
	lp = FramePointer(lpFrame, 0, y, YES);
	if (lp)
		FrameSetRGB(lpFrame, &rgb, lp, dx);
	++y;
	}
}

/************************************************************************/
static void SetupBrushPreview(HWND hDlg)
/************************************************************************/
{
HWND hControl;
RECT rPreview;

lpPreviewFrame = NULL;
if (hControl = GetDlgItem(hDlg, IDC_BRUSHPREVIEW))
	{
	GetClientRect(hControl, &rPreview);
	lpPreviewFrame = FrameOpen(FDT_RGBCOLOR, rPreview.right-rPreview.left,
						rPreview.bottom-rPreview.top, 100);
	if (lpPreviewFrame)
		FillPreviewFrame(lpPreviewFrame);
	AttachPreviewFrame(hDlg, IDC_BRUSHPREVIEW, lpPreviewFrame);
	}
}

/************************************************************************/
static void SetupPreviewCursor(HWND hDlg, BOOL fInit)
/************************************************************************/
{
HWND hControl;
static HCURSOR hOldCursor;

if (!(hControl = GetDlgItem(hDlg, IDC_BRUSHPREVIEW)))
	return;

if (fInit)
	hOldCursor = SetClassCursor( hControl, Window.hNullCursor );
else
	SetClassCursor( hControl, hOldCursor );
}

/************************************************************************/
static BOOL DisplayPreviewBrush(HWND hWnd, int x, int y, BOOL on )
/************************************************************************/
{
HDC hDC;
static RECT onRect, on2Rect;
static int bx, by;
static BOOL bBrushOn;
static HWND hBrushWnd;
static int ShapeOn;
static int SizeOn;
int HalfSize;

// make sure we are in a valid state
if ( on == bBrushOn || !lpBrush)
	return(FALSE);

if (on)
    {
	// special case to redisplay brush at same location it was turned off
    if (x == INT_MAX)
		{
		// get location that the brush was turned off at
		x = bx;
		y = by;
		}
	// save window we turned brush on in
    hBrushWnd = hWnd;
    }
else
    {
    hWnd = hBrushWnd;
    hBrushWnd = 0;
    }
// do we have a window to use?
if (!hWnd)
    return(FALSE);

// save last on state
bBrushOn = on;
if ( on )
	{
	// save current mouse location for turning off brush
	bx = x;
	by = y;

	ShapeOn = GetMgxBrushShape((HMGXBRUSH)lpBrush);
	SizeOn = GetMgxBrushSize((HMGXBRUSH)lpBrush);
	HalfSize = SizeOn/2;

	// setup rectangle in file coordinates for brush
	if (ShapeOn == IDC_BRUSHCUSTOM ||
	    ShapeOn == IDC_BRUSHSQUARE ||
	    ShapeOn == IDC_BRUSHCIRCLE ||
	    ShapeOn == IDC_BRUSHSLASH ||
	    ShapeOn == IDC_BRUSHBACKSLASH)
		{
		onRect.left = x - HalfSize;
		onRect.right = onRect.left + SizeOn - 1;
		onRect.top = y - HalfSize;
		onRect.bottom = onRect.top + SizeOn - 1;
		}
	else if (ShapeOn == IDC_BRUSHHORIZONTAL)
		{
		onRect.left = x - HalfSize;
		onRect.right = onRect.left + SizeOn - 1;
		onRect.top = onRect.bottom = y;
		}
	else if (ShapeOn == IDC_BRUSHVERTICAL)
		{
		onRect.left = onRect.right = x;
		onRect.top = y - HalfSize;
		onRect.bottom = onRect.top + SizeOn - 1;
		}
	}
hDC = GetDC(hWnd);
WinBrush( hWnd, hDC, &onRect, bBrushOn, NO, ShapeOn, FUNITY );
ReleaseDC(hWnd, hDC);
return(TRUE);
}

/************************************************************************/
LOCAL void SaveRetouchMacro(LPIMAGE lpImage, ITEMID idTool)
/************************************************************************/
{
RETOUCH_PARMS parms;
ITEMID idCommand;

parms.hpStrokeInfo = MgxBrushGetStrokeInfo(&parms.nPoints, &parms.fUsePressure, &parms.iDstRes, &parms.iSrcRes);
if (!parms.hpStrokeInfo)
	return;

switch (idTool)
	{
	case IDC_PAINT:
		idCommand = IDS_CMD_PAINT;
		lstrcpy(parms.StyleName, Retouch.RetouchStyleName[idTool-IDC_FIRST_RETOUCHTOOL]);
		break;
	case IDC_SPRAY:
		idCommand = IDS_CMD_SPRAY;
		lstrcpy(parms.StyleName, Retouch.RetouchStyleName[idTool-IDC_FIRST_RETOUCHTOOL]);
		break;
	case IDC_CLONE:
		idCommand = IDS_CMD_CLONE;
		lstrcpy(parms.StyleName, Retouch.RetouchStyleName[idTool-IDC_FIRST_RETOUCHTOOL]);
		break;
	case IDC_TEXTURE:
		idCommand = IDS_CMD_TEXTURE;
		lstrcpy(parms.StyleName, Retouch.RetouchStyleName[idTool-IDC_FIRST_RETOUCHTOOL]);
		break;
	case IDC_SMEAR:
		idCommand = IDS_CMD_SMEAR;
		lstrcpy(parms.StyleName, Retouch.RetouchStyleName[idTool-IDC_FIRST_RETOUCHTOOL]);
		break;
	case IDC_ERASER:
		idCommand = IDS_CMD_ERASER;
		lstrcpy(parms.StyleName, Retouch.RetouchStyleName[idTool-IDC_FIRST_RETOUCHTOOL]);
		break;
	case IDC_ALTERASER:
		idCommand = IDS_CMD_UNDOERASE;
		lstrcpy(parms.StyleName, "");
		break;
	case IDC_SHARP:
		idCommand = IDS_CMD_SHARP;
		lstrcpy(parms.StyleName, Retouch.FilterStyleName[idTool-IDC_FIRST_FILTERTOOL]);
		break;
	case IDC_SMOOTH:
		idCommand = IDS_CMD_SMOOTH;
		lstrcpy(parms.StyleName, Retouch.FilterStyleName[idTool-IDC_FIRST_FILTERTOOL]);
		break;
	case IDC_LIGHTEN:
		idCommand = IDS_CMD_LIGHTEN;
		lstrcpy(parms.StyleName, Retouch.FilterStyleName[idTool-IDC_FIRST_FILTERTOOL]);
		break;
	case IDC_DARKEN:
		idCommand = IDS_CMD_DARKEN;
		lstrcpy(parms.StyleName, Retouch.FilterStyleName[idTool-IDC_FIRST_FILTERTOOL]);
		break;
	case IDC_PAINTON:
        if( Control.Retail )
    		idCommand = IDS_CMD_PAINTON;
		break;
	}
parms.BrushShape = Retouch.BrushShape;
if (Retouch.BrushShape == IDC_BRUSHCUSTOM)
	lstrcpy(parms.ShapeName, Names.CustomBrush);
else
	parms.ShapeName[0] = '\0';
parms.Size = Retouch.BrushSize;
parms.Feather = Retouch.Feather;
parms.Opacity = Retouch.Opacity;
parms.MergeMode = Retouch.MergeMode;
parms.Pressure = Retouch.Pressure;
parms.Mode = Mask.iPaintOnModes;
parms.fPaintOnObject = Mask.PaintOnObject;
GetMgxBrushSourceColor(Retouch.hBrush, &parms.Color);
PostCommand(lpImage->lpCmdList, idCommand, &parms);
}

/************************************************************************/
LOCAL void SaveCloneMacro(LPIMAGE lpImage, HWND hCloneSrcWnd, POINT ptCloneSrc)
/************************************************************************/
{
RETOUCH_PARMS parms;
int res;
LPIMAGE lpSrcImage;

parms.hpStrokeInfo = MgxBrushGetStrokeInfo(&parms.nPoints, &parms.fUsePressure, &parms.iDstRes, &parms.iSrcRes);
if (!parms.hpStrokeInfo)
	return;
lstrcpy(parms.StyleName, Retouch.RetouchStyleName[IDC_CLONE-IDC_FIRST_RETOUCHTOOL]);
parms.BrushShape = Retouch.BrushShape;
if (Retouch.BrushShape == IDC_BRUSHCUSTOM)
	lstrcpy(parms.ShapeName, Names.CustomBrush);
else
	parms.ShapeName[0] = '\0';
parms.Size = Retouch.BrushSize;
parms.Feather = Retouch.Feather;
parms.Opacity = Retouch.Opacity;
parms.MergeMode = Retouch.MergeMode;
parms.Pressure = Retouch.Pressure;
parms.szSourceImage[0] = '\0';
parms.ptSource.x = parms.ptSource.y = 0;
if (hCloneSrcWnd)
	{
	lpSrcImage = GetImagePtr(hCloneSrcWnd);
	if (lpSrcImage)
		{
		if (lpSrcImage == lpImage)
			lstrcpy(parms.szSourceImage, ACTIVE_IMAGE);
		else
			lstrcpy(parms.szSourceImage, lpSrcImage->CurFile);
		parms.ptSource = ptCloneSrc;
		}
	}
PostCommand(lpImage->lpCmdList, IDS_CMD_CLONE, &parms);
}

/************************************************************************/
BOOL RetouchImage(LPIMAGE lpImage, LPRETOUCH_PARMS lpParms)
/************************************************************************/
{
MGXBRUSHPARMS mgxbrush;
HMGXBRUSH hBrush;
int Size, res, dx, dy, sx, sy, nPoints, i, srcRes;
int last_dx, last_dy, Pressure;
LPOBJECT lpObject, lpSrcObject;
HPSTROKEINFO hpStrokeInfo;
RECT rEdit;
POINT ptCloneSrc;
LPIMAGE lpSrcImage;
FRMDATATYPE SrcType, DstType;
LPFRAME lpSrcFrame;
ITEMID idType, idTool;
FRMTYPEINFO TypeInfo;

if (!(hpStrokeInfo = lpParms->hpStrokeInfo))
	return(FALSE);
FrameGetTypeInfo(ImgGetBaseEditFrame(lpImage), &TypeInfo);
switch (lpParms->BrushType)
	{
	case BT_PAINT:
		idType = IDN_PAINTSTYLE;
		idTool = IDC_PAINT;
		break;
	case BT_SPRAY:
		idType = IDN_SPRAYSTYLE;
		idTool = IDC_SPRAY;
		break;
	case BT_CLONE:
		idType = IDN_CLONESTYLE;
		idTool = IDC_CLONE;
		break;
	case BT_TEXTURE:
		idType = IDN_TEXTURESTYLE;
		idTool = IDC_TEXTURE;
		break;
	case BT_SMEAR:
		idType = IDN_SMEARSTYLE;
		idTool = IDC_SMEAR;
		break;
	case BT_ERASER:
		idType = IDN_ERASESTYLE;
		idTool = IDC_ERASER;
		break;
	case BT_SHARP:
		idType = IDN_SHARPSTYLE;
		idTool = IDC_SHARP;
		break;
	case BT_SMOOTH:
		idType = IDN_SMOOTHSTYLE;
		idTool = IDC_SMOOTH;
		break;
	case BT_LIGHTEN:
		idType = IDN_LIGHTENSTYLE;
		idTool = IDC_LIGHTEN;
		break;
	case BT_DARKEN:
		idType = IDN_DARKENSTYLE;
		idTool = IDC_DARKEN;
		break;
	default:
		idType = 0;
		idTool = 0;
		break;
	}

if (!idType)
	return(TRUE);

if (!IsValidTool(lpImage, idTool))
	return(FALSE);

if (lpImage->hLastBrush)
	{
	if (!StringsEqual(lpParms->StyleName, lpImage->szLastBrushStyle) ||
		(idType != lpImage->idLastBrush))
		{
		DestroyMgxBrush((HMGXBRUSH)lpImage->hLastBrush);
		lpImage->hLastBrush = NULL;
		}
	}
if (lpImage->hLastBrush)
	hBrush = (HMGXBRUSH)lpImage->hLastBrush;
else
	hBrush = CreateMgxBrush(lpParms->StyleName, idType, TypeInfo);
if (!hBrush)
	return(FALSE);
srcRes = res = FrameResolution(ImgGetBaseEditFrame(lpImage));
Size = lpParms->Size;
ResConvertDist(lpParms->iDstRes, res, &Size);

SetMgxBrushSize(hBrush, Size);
SetMgxBrushShape(hBrush, lpParms->BrushShape, lpParms->ShapeName);
SetMgxBrushOpacity(hBrush, lpParms->Opacity);
SetMgxBrushPressure(hBrush, lpParms->Pressure);
SetMgxBrushMergeMode(hBrush, lpParms->MergeMode);
SetMgxBrushFeather(hBrush, lpParms->Feather);
SetMgxBrushColor(hBrush, &lpParms->Color);

HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
dx = hpStrokePtr->dx;
dy = hpStrokePtr->dy;
ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);

if (ImgCountSelObjects(lpImage, NULL) == 1)
	lpObject = ImgGetSelObject(lpImage, NULL);
else
	lpObject = ImgFindSelObject(lpImage, dx, dy, NO);
if (!lpObject)
	{
	if (hBrush != lpImage->hLastBrush)
		DestroyMgxBrush(hBrush);
	return(FALSE);
	}

// if we have an image find it by filename
if (lpParms->BrushType == BT_CLONE && lstrlen(lpParms->szSourceImage))
	{
	if (StringsEqual(lpParms->szSourceImage, ACTIVE_IMAGE))
		lpSrcImage = lpImage;
	else
	{
		CServerDoc *pDoc = PictPubApp.GetDocument(lpParms->szSourceImage);    
		if (!pDoc)
			{
			Message(IDS_NOCLONESOURCE);
			if (hBrush != lpImage->hLastBrush)
				DestroyMgxBrush(hBrush);
			return(FALSE);
			}
		lpSrcImage = pDoc->m_lpImage;
	}
	srcRes = FrameResolution(ImgGetBaseEditFrame(lpSrcImage));

	ptCloneSrc = lpParms->ptSource;
	ResConvertCenter(lpParms->iSrcRes, srcRes, &ptCloneSrc.x, &ptCloneSrc.y);
	lpSrcObject = ImgFindObject(lpSrcImage, ptCloneSrc.x, ptCloneSrc.y,
						NO, NO);
	if (!lpSrcObject)
		{
		Message(IDS_NOCLONESOURCE);
		if (hBrush != lpImage->hLastBrush)
			DestroyMgxBrush(hBrush);
		return(FALSE);
		}
	// make sure source object and destination object are
	// the same data type
	PixmapGetInfo(&lpSrcObject->Pixmap, PMT_EDIT, NULL, NULL, NULL,
					 &SrcType);
	PixmapGetInfo(&lpObject->Pixmap, PMT_EDIT, NULL, NULL, NULL,
					&DstType);
	if (SrcType != DstType)
		{
		Message(IDS_BRUSHCLONETYPE);
		if (hBrush != lpImage->hLastBrush)
			DestroyMgxBrush(hBrush);
		return(FALSE);
		}
	SetMgxBrushSource(hBrush, lpSrcObject->Pixmap.EditFrame,
					lpSrcObject->rObject.left, lpSrcObject->rObject.top);
	}
else
if (lpParms->BrushType == BT_TEXTURE)
	{
	lpSrcFrame = GetMgxBrushSourceFrame(hBrush);
//	if (lpSrcFrame)
//		srcRes = FrameResolution(lpSrcFrame);
	}

if (GetMgxBrushSource(hBrush) == SD_CURRENT_UNDO)
	{
	if (!lpObject->DataDirty ||
		!lpObject->Pixmap.UndoFrame ||
		lpObject->Pixmap.fNewFrame)
		{
		if (hBrush != lpImage->hLastBrush)
			DestroyMgxBrush(hBrush);
		return(TRUE);
		}
	}

if (lpParms->BrushType != BT_ERASER ||
	GetMgxBrushSource(hBrush) != SD_CURRENT_UNDO)
	{
	if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		{
		if (hBrush != lpImage->hLastBrush)
			DestroyMgxBrush(hBrush);
		return(FALSE);
		}
	}
MgxBrushSetAdvanced(&mgxbrush, NULL, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					hBrush, NO, lpImage, NULL, NULL);
ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty));
if (!MgxBrushBeginParms(&mgxbrush))
	{
	ProgressEnd();
	if (hBrush != lpImage->hLastBrush)
		DestroyMgxBrush(hBrush);
   return(FALSE);
	}
nPoints = lpParms->nPoints;
last_dx = INT_MIN;
last_dy = INT_MIN;
for (i = 0; i < nPoints; ++i)
	{
	AstralClockCursor(i, nPoints, NO);
	if (lpParms->BrushType == BT_CLONE)
		// || lpParms->BrushType == BT_TEXTURE)
		{
		if (lpParms->fUsePressure)
			{
			HPSTROKEINFO4 hpStrokePtr = (HPSTROKEINFO4)hpStrokeInfo;
			dx = hpStrokePtr->dx;
			dy = hpStrokePtr->dy;
			ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
			sx = hpStrokePtr->sx;
			sy = hpStrokePtr->sy;
			ResConvertCenter(lpParms->iSrcRes, srcRes, &sx, &sy);
			Pressure = hpStrokePtr->Pressure;
			++hpStrokePtr;
			hpStrokeInfo = hpStrokePtr;
			}
		else
			{
			HPSTROKEINFO3 hpStrokePtr = (HPSTROKEINFO3)hpStrokeInfo;
			dx = hpStrokePtr->dx;
			dy = hpStrokePtr->dy;
			ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
			sx = hpStrokePtr->sx;
			sy = hpStrokePtr->sy;
			ResConvertCenter(lpParms->iSrcRes, srcRes, &sx, &sy);
			++hpStrokePtr;
			hpStrokeInfo = hpStrokePtr;
			}
		}
	else
		{
		if (lpParms->fUsePressure)
			{
			HPSTROKEINFO2 hpStrokePtr = (HPSTROKEINFO2)hpStrokeInfo;
			dx = hpStrokePtr->dx;
			dy = hpStrokePtr->dy;
			ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
			sx = dx;
			sy = dy;
			Pressure = hpStrokePtr->Pressure;
			++hpStrokePtr;
			hpStrokeInfo = hpStrokePtr;
			}
		else
			{
			HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
			dx = hpStrokePtr->dx;
			dy = hpStrokePtr->dy;
			ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
			sx = dx;
			sy = dy;
			++hpStrokePtr;
			hpStrokeInfo = hpStrokePtr;
			}
		}
	if (dx == last_dx && dy == last_dy)
		MgxOSPickupBrush();
	last_dx = dx;
	last_dy = dy;	
	MgxBrushStroke(sx, sy, dx, dy, 0, 0);
	}
MgxBrushEnd(YES);
MgxGetEditRect(&rEdit);
ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, NULL);
lpParms->Common.UpdateType = UT_AREA;
lpParms->Common.rUpdateArea = rEdit;

// save the brush for next time
lpImage->hLastBrush = hBrush;
lstrcpy(lpImage->szLastBrushStyle, lpParms->StyleName);
lpImage->idLastBrush = idType;

ProgressEnd();
return(TRUE);
}

/************************************************************************/
BOOL AltEraserImage(LPIMAGE lpImage, LPRETOUCH_PARMS lpParms)
/************************************************************************/
{
MGXBRUSHPARMS mgxbrush;
HMGXBRUSH hBrush;
int Size, res, dx, dy, sx, sy, nPoints, i;
int last_dx, last_dy, Pressure;
LPOBJECT lpObject;
HPSTROKEINFO hpStrokeInfo;
RECT rEdit;
FRMTYPEINFO TypeInfo;

if (!(hpStrokeInfo = lpParms->hpStrokeInfo))
	return(FALSE);
FrameGetTypeInfo(ImgGetBaseEditFrame(lpImage), &TypeInfo);

if (!(hBrush = CreateMgxEraser(0)))
	return(FALSE);

res = FrameResolution(ImgGetBaseEditFrame(lpImage));
Size = lpParms->Size;
ResConvertDist(lpParms->iDstRes, res, &Size);

SetMgxBrushSize(hBrush, Size);
SetMgxBrushShape(hBrush, lpParms->BrushShape, lpParms->ShapeName);
SetMgxBrushOpacity(hBrush, lpParms->Opacity);
SetMgxBrushFeather(hBrush, lpParms->Feather);

HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
dx = hpStrokePtr->dx;
dy = hpStrokePtr->dy;
ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
if (ImgCountSelObjects(lpImage, NULL) == 1)
	lpObject = ImgGetSelObject(lpImage, NULL);
else
	lpObject = ImgFindSelObject(lpImage, dx, dy, NO);
if (!lpObject)
	{
	DestroyMgxBrush(hBrush);
	return(FALSE);
	}

if ( !lpObject->DataDirty || !lpObject->Pixmap.UndoFrame )
	{
	DestroyMgxBrush(hBrush);
	return(TRUE);
	}
if (lpObject->Pixmap.fNewFrame)
	{
	DestroyMgxBrush(hBrush);
	return(TRUE);
	}
MgxBrushSetAdvanced(&mgxbrush, NULL, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					hBrush, NO, lpImage, NULL, NULL);
ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty));
if (!MgxBrushBeginParms(&mgxbrush))
	{
	ProgressEnd();
	DestroyMgxBrush(hBrush);
	return(FALSE);
	}
nPoints = lpParms->nPoints;
last_dx = INT_MIN;
last_dy = INT_MIN;
for (i = 0; i < nPoints; ++i)
	{
	AstralClockCursor(i, nPoints, NO);
	if (lpParms->fUsePressure)
		{
		HPSTROKEINFO2 hpStrokePtr = (HPSTROKEINFO2)hpStrokeInfo;
		dx = hpStrokePtr->dx;
		dy = hpStrokePtr->dy;
		ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
		sx = dx;
		sy = dy;
		Pressure = hpStrokePtr->Pressure;
		++hpStrokePtr;
		hpStrokeInfo = hpStrokePtr;
		}
	else
		{
		HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
		dx = hpStrokePtr->dx;
		dy = hpStrokePtr->dy;
		ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
		sx = dx;
		sy = dy;
		++hpStrokePtr;
		hpStrokeInfo = hpStrokePtr;
		}
	if (dx == last_dx && dy == last_dy)
		MgxOSPickupBrush();
	last_dx = dx;
	last_dy = dy;	
	MgxBrushStroke(sx, sy, dx, dy, 0, 0);
	}
MgxBrushEnd(YES);
MgxGetEditRect(&rEdit);
lpParms->Common.UpdateType = UT_AREA;
lpParms->Common.rUpdateArea = rEdit;

DestroyMgxBrush(hBrush);

ProgressEnd();
return(TRUE);
}

	
/************************************************************************/
BOOL PaintMaskImage(LPIMAGE lpImage, LPRETOUCH_PARMS lpParms)
/************************************************************************/
{
MGXBRUSHPARMS mgxbrush;
HMGXBRUSH hBrush;
int Size, res, dx, dy, sx, sy, nPoints, i;
int last_dx, last_dy, Pressure;
LPOBJECT lpObject;
HPSTROKEINFO hpStrokeInfo;
RECT rEdit;
COLORINFO ColorStruct;
BOOL fNewMask;
LPALPHA lpAlpha;
RECT rUpdate;

AstralSetRectEmpty(&rUpdate);
if (!(hpStrokeInfo = lpParms->hpStrokeInfo))
	return(FALSE);

HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
res = FrameResolution(ImgGetBaseEditFrame(lpImage));
dx = hpStrokePtr->dx;
dy = hpStrokePtr->dy;
ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);

fNewMask = NO;
if (lpParms->fPaintOnObject && ImgMultipleObjects(lpImage))
	{
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		lpObject = ImgFindSelObject(lpImage, dx, dy, NO);
	if (!lpObject)
		return(FALSE);
	if (lpObject == ImgGetBase(lpImage))
		return(FALSE);
	lpAlpha = lpObject->lpAlpha;
	if (!lpAlpha)
		return(FALSE);
	}
else if (lpAlpha = ImgGetMaskEx(lpImage, lpParms->Mode == SHAPE_SUBTRACT,
					 Control.NoUndo, &fNewMask, &rUpdate))
	{
	lpObject = ImgGetBase(lpImage);
	if (fNewMask &&
		(lpParms->Mode == SHAPE_ADD && ImgGetViewMode(lpImage) == VM_SLIME))
		{
		ImgGetMaskUpdateRect(lpImage, YES, NO, &rUpdate);
		}
	}
else
	return(FALSE);

ColorStruct.ColorSpace = CS_GRAY;
if (lpParms->Mode == SHAPE_SUBTRACT)
	ColorStruct.gray = lpAlpha->bInvert ? 255 : 0;
else
	ColorStruct.gray = lpAlpha->bInvert ? 0 : 255;

hBrush = CreateMgxColorBrush(&ColorStruct, lpParms->Feather);
if (!hBrush)
	{
	if (fNewMask)
		{
		ImgSetMask(lpImage, NULL);
		MaskClose(lpAlpha);
		}
	return(FALSE);
	}

Size = lpParms->Size;
ResConvertDist(lpParms->iDstRes, res, &Size);
SetMgxBrushSize(hBrush, Size);
SetMgxBrushShape(hBrush, lpParms->BrushShape, lpParms->ShapeName);
SetMgxBrushOpacity(hBrush, lpParms->Opacity);
SetMgxBrushFeather(hBrush, lpParms->Feather);

if (!ImgEditInit(lpImage, ET_OBJECT,
			fNewMask ? UT_CREATEMASK : UT_ALPHA, lpObject))
	{
	if (fNewMask)
		{
		ImgSetMask(lpImage, NULL);
		MaskClose(lpAlpha);
		}
	DestroyMgxBrush(hBrush);
	return(FALSE);
	}

if (lpParms->Opacity == 255)
	((LPMGXBRUSH)hBrush)->NoOverlap = NO;
else
	((LPMGXBRUSH)hBrush)->NoOverlap = YES;

MgxBrushSetAdvanced(&mgxbrush, NULL, &lpAlpha->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					hBrush, NO, lpImage, NULL, NULL);
ProgressBegin(1, PROGRESS_ID(lpParms->Common.idDirty));
if (!MgxBrushBeginParms(&mgxbrush))
	{
	ProgressEnd();
	if (fNewMask)
		{
		ImgSetMask(lpImage, NULL);
		MaskClose(lpAlpha);
		}
	DestroyMgxBrush(hBrush);
	return(FALSE);
	}
nPoints = lpParms->nPoints;
last_dx = INT_MIN;
last_dy = INT_MIN;
for (i = 0; i < nPoints; ++i)
	{
	AstralClockCursor(i, nPoints, NO);
	if (lpParms->fUsePressure)
		{
		HPSTROKEINFO2 hpStrokePtr = (HPSTROKEINFO2)hpStrokeInfo;
		dx = hpStrokePtr->dx;
		dy = hpStrokePtr->dy;
		ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
		sx = dx;
		sy = dy;
		Pressure = hpStrokePtr->Pressure;
		++hpStrokePtr;
		hpStrokeInfo = hpStrokePtr;
		}
	else
		{
		HPSTROKEINFO1 hpStrokePtr = (HPSTROKEINFO1)hpStrokeInfo;
		dx = hpStrokePtr->dx;
		dy = hpStrokePtr->dy;
		ResConvertCenter(lpParms->iDstRes, res, &dx, &dy);
		sx = dx;
		sy = dy;
		++hpStrokePtr;
		hpStrokeInfo = hpStrokePtr;
		}
	if (dx == last_dx && dy == last_dy)
		MgxOSPickupBrush();
	last_dx = dx;
	last_dy = dy;	
	MgxBrushStroke(sx, sy, dx, dy, 0, 0);
	}
MgxBrushEnd(YES);
MgxGetEditRect(&rEdit);
AstralUnionRect(&rEdit, &rEdit, &rUpdate);
ImgEditedObject(lpImage, lpObject, lpParms->Common.idDirty, NULL);
if (lpParms->fPaintOnObject)
	lpParms->Common.UpdateType = UT_AREA;
else
	lpParms->Common.UpdateType = UT_ACTIVEAREA;
lpParms->Common.rUpdateArea = rEdit;

DestroyMgxBrush(hBrush);

ProgressEnd();
return(TRUE);
}

/************************************************************************/
BOOL StrokeMaskImage(LPIMAGE lpImage, LPSTROKEMASK_PARMS lpParms)
/************************************************************************/
{
HMGXBRUSH hBrush;
int Size, res;
ITEMID idType;
FRMTYPEINFO TypeInfo;
int inout, BrushHalf, BrushFull;
LPMASK lpMask;
LPFRAME lpFrame;
LPCPolyList lpPolyList;
RECT rShape, rArea;

if (!(lpMask = ImgGetMask(lpImage)))
	return(TRUE); // not really an error
if (!(lpFrame = lpMask->Pixmap.EditFrame))
	return(FALSE); // this is an error

FrameGetTypeInfo(ImgGetBaseEditFrame(lpImage), &TypeInfo);
switch (lpParms->idBrush)
	{
	case IDC_PAINTTOOL:
		idType = IDN_PAINTSTYLE;
		break;
	case IDC_SPRAYTOOL:
		idType = IDN_SPRAYSTYLE;
		break;
	case IDC_TEXTURETOOL:
		idType = IDN_TEXTURESTYLE;
		break;
	case IDC_SMEARTOOL:
		idType = IDN_SMEARSTYLE;
		break;
	case IDC_SHARPENTOOL:
		idType = IDN_SHARPSTYLE;
		break;
	case IDC_SMOOTHTOOL:
		idType = IDN_SMOOTHSTYLE;
		break;
	case IDC_LIGHTENTOOL:
		idType = IDN_LIGHTENSTYLE;
		break;
	case IDC_DARKENTOOL:
		idType = IDN_DARKENSTYLE;
		break;
	default:
		idType = 0;
		break;
	}

if (lpImage->hLastBrush)
	{
	if (!StringsEqual(lpParms->StyleName, lpImage->szLastBrushStyle) ||
		(idType != lpImage->idLastBrush))
		{
		DestroyMgxBrush((HMGXBRUSH)lpImage->hLastBrush);
		lpImage->hLastBrush = NULL;
		}
	}
if (lpImage->hLastBrush)
	hBrush = (HMGXBRUSH)lpImage->hLastBrush;
else
	hBrush = CreateMgxBrush(lpParms->StyleName, idType, TypeInfo);
if (!hBrush)
	return(FALSE);

res = FrameResolution(ImgGetBaseEditFrame(lpImage));
Size = lpParms->Size;
ResConvertDist(lpParms->iBaseRes, res, &Size);

SetMgxBrushSize(hBrush, Size);
SetMgxBrushShape(hBrush, lpParms->BrushShape, lpParms->ShapeName);
SetMgxBrushOpacity(hBrush, lpParms->Opacity);
SetMgxBrushPressure(hBrush, lpParms->Pressure);
SetMgxBrushMergeMode(hBrush, lpParms->MergeMode);
SetMgxBrushFeather(hBrush, lpParms->Feather);
SetMgxBrushColor(hBrush, &lpParms->Color);

ProgressBegin(2, PROGRESS_ID(IDS_UNDOSTROKEMASK));
rArea.top = rArea.left = 0;
rArea.right = FrameXSize(lpFrame) - 1;
rArea.bottom = FrameYSize(lpFrame) - 1;
if ( !(lpPolyList = GetPath(&rArea, lpFrame, 127, &inout)) )
	{
	if (hBrush != lpImage->hLastBrush)
		DestroyMgxBrush(hBrush);
	ProgressEnd();
	return(FALSE);
	}

EnableMarquee( NO );
// remove mask temporarily
ImgSetMask(lpImage, NULL);
BrushShapeObj( lpImage, lpPolyList, hBrush, YES/*bCacheInit*/, IDS_UNDOSTROKEMASK );
BrushFull = GetMgxBrushSize(hBrush);
BrushHalf = BrushFull / 2;
PolyListGetBounds(lpPolyList, &rShape);
FreePolyList(lpPolyList);
ImgSetMask(lpImage, lpMask);
EnableMarquee( YES );

rShape.top -= BrushHalf; // brush engine adds this to UndoRect
rShape.left -= BrushHalf;
rShape.bottom += (BrushFull-BrushHalf);
rShape.right += (BrushFull-BrushHalf);

lpParms->Common.UpdateType = UT_AREA;
lpParms->Common.rUpdateArea = rShape;

// save the brush for next time
lpImage->hLastBrush = hBrush;
lstrcpy(lpImage->szLastBrushStyle, lpParms->StyleName);
lpImage->idLastBrush = idType;

ProgressEnd();
return(TRUE);
}









