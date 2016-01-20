//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "maskutil.h"

LOCAL void OSDrawShapeDataProc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPVOID lpParam);
LOCAL void OSDrawShapeMaskProc( int y, int left, int right, LPTR lpMsk, LPVOID lpParam);
LOCAL void BrushMaskSelObj(LPIMAGE lpImage,HMGXBRUSH hBrush, BOOL bCacheInit, int dirty);
LOCAL LPFRAME OSBrushPoly(LPCPoly lpPoly, HMGXBRUSH hBrush, LPRECT lpOutRect);
LOCAL BOOL DrawEllipse(LPIMAGE lpImage, LPDRAWSHAPE_PARMS lpParms, LPRECT lpRect, HMGXBRUSH hBrush );
LOCAL BOOL DrawRectangle(LPIMAGE lpImage, LPDRAWSHAPE_PARMS lpParms, LPRECT lpRect, HMGXBRUSH hBrush );
LOCAL BOOL DrawShapeClosed( LPIMAGE lpImage, LPCPoly lpPoly, HMGXBRUSH hBrush,
					LPCOLORINFO lpBorderColor, LPCOLORINFO lpFillColor,
					BOOL	fInterior, BOOL fExterior,
					ITEMID idDirty, LPRECT lpUpdateRect);
//LOCAL BOOL DrawShapeOpen( LPIMAGE lpImage, LPSHAPE lpShape, HMGXBRUSH hBrush, int dirty, LPRECT lpUpdateRect );
LOCAL HMGXBRUSH CreateDrawBrush(LPDRAWBRUSH lpBrush, ITEMID idType, LPCOLORINFO lpColorInfo, int iSrcRes, int iDstRes, FRMTYPEINFO TypeInfo);
LOCAL count_points(LPCPoly lpPoly, LPLONG lpCount);
LOCAL BOOL close_poly(LPCPoly lpPoly, LONG bClosed);

static LPMASK lpDrawMask;
static POINT DrawMaskOffset;
static RECT SelectRect;
static LPFRAME lpExteriorFrame;
static RECT ExteriorRect;
static LPTR lpColorLuts[4];
static BOOL fShift;

extern BOOL bFreeBezier;						// adding beziers in free mode

// LOCAL prototypes
LOCAL BOOL Draw_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Draw_OnDocActivated(HWND hDlg);
LOCAL void Draw_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Draw_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Draw_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Draw_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);

/***********************************************************************/
BOOL WINPROC EXPORT DlgDrawProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Draw_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Draw_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Draw_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Draw_OnControlEnable);

	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Draw_OnMeasureItem);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, Tool.idRibbon);
	}
}

/************************************************************************/
LOCAL BOOL Draw_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
HWND hWnd;
RETOUCHDLG rd;

rd.idTool = Control.idDrawTool;
rd.idFirstTool = IDC_FIRST_DRAWTOOL;
rd.idFirstStyle = IDN_FIRST_DRAWSTYLE;
rd.idCreateDlg = Control.idDrawTool == IDC_PENCIL ? IDD_CREATEPENCILBRUSH : IDD_CREATEDRAWBRUSH;
rd.fIsRibbon = YES;
rd.StyleName = Retouch.DrawStyleName;
RetouchInit(hDlg, hWndFocus, lParam, &rd);
hWnd = hDlg;
Mask.PointEdit = FALSE;
CheckComboItem( hDlg, IDC_SQUSTYLE, IDC_SQUEDGE, IDC_CIREDGEANDFILL,
	Draw.wSquareStyle );
CheckComboItem( hDlg, IDC_FRESTYLE, IDC_FREEDGE, IDC_FREEDGEANDFILL,
	Draw.wFreeStyle + IDC_FREEDGE );
CheckRadioButton( hDlg, IDC_SQUAREMASK, IDC_CIRCLEMASK,
		IDC_SQUAREMASK + Draw.DoDrawCircle );
if (Control.idDrawTool == IDC_PENCIL && !Retouch.BrushSize)
	Retouch.BrushSize = 1;
Draw_HandleEnable(hDlg, 0);
return(FALSE);
}

/************************************************************************/
LOCAL void Draw_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Draw_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Draw_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Draw_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Draw_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
int id;
RETOUCHDLG rd;

// The following code also goes below and into the shape accelerator

SetPointerDlgItems(POINTWND(hDlg));
ControlEnable( hDlg, IDC_IS_POINT , Mask.PointEdit );
ControlEnable( hDlg, IDC_IS_BEZIER, Mask.PointEdit );
CheckDlgButton(hDlg, IDC_POINTMODE, Mask.PointEdit);
id = (Mask.PointEdit ? -1:(bFreeBezier?IDC_DRAW_BEZIER:IDC_DRAW_LINE));
CheckRadioButton( hDlg, IDC_DRAW_BEZIER, IDC_DRAW_LINE, id);

rd.idTool = Control.idDrawTool;
rd.idFirstTool = IDC_FIRST_DRAWTOOL;
rd.idFirstStyle = IDN_FIRST_DRAWSTYLE;
rd.idCreateDlg = Control.idDrawTool == IDC_PENCIL ? IDD_CREATEPENCILBRUSH : IDD_CREATEDRAWBRUSH;
rd.fIsRibbon = YES;
rd.StyleName = Retouch.DrawStyleName;
RetouchEnable(hDlg, fFlags, &rd);
}

/************************************************************************/
LOCAL void Draw_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
ITEMID id;
BOOL Bool;

id = (ITEMID)(lpMeasureItem)->CtlID;
Bool = (id == IDC_BRUSHSHAPE || id == IDC_SQUSTYLE || id == IDC_FRESTYLE);
MeasureItem( hDlg, lpMeasureItem, Bool );
}

/************************************************************************/
LOCAL void Draw_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT * lpDrawItem)
/************************************************************************/
{
DrawItem( hDlg, lpDrawItem, 0 );
}

/************************************************************************/
LOCAL void Draw_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
RETOUCHDLG rd;

rd.idTool = Control.idDrawTool;
rd.idFirstTool = IDC_FIRST_DRAWTOOL;
rd.idFirstStyle = IDN_FIRST_DRAWSTYLE;
rd.idCreateDlg = Control.idDrawTool == IDC_PENCIL ? IDD_CREATEPENCILBRUSH : IDD_CREATEDRAWBRUSH;
rd.fIsRibbon = YES;
rd.StyleName = Retouch.DrawStyleName;
if (RetouchCommand(hDlg, id, hControl, codeNotify, &rd))
	return;

switch (id)
	{
	case IDC_SQUAREMASK:
	case IDC_CIRCLEMASK:
	Draw.DoDrawCircle = id - IDC_SQUAREMASK;
	CheckRadioButton( hDlg, IDC_SQUAREMASK, IDC_CIRCLEMASK,
			IDC_SQUAREMASK + Draw.DoDrawCircle );
	SetCommonDoCircle(Draw.DoDrawCircle);
	break;

	case IDC_SQUSTYLE:
//	case IDC_SQUEDGE:
//	case IDC_SQUFILL:
//	case IDC_SQUEDGEANDFILL:
//	case IDC_CIREDGE:
//	case IDC_CIRFILL:
//	case IDC_CIREDGEANDFILL:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Draw.wSquareStyle = id;
	break;

	case IDC_FRESTYLE:
//	case IDC_FREEDGE:
//	case IDC_FREFILL:
//	case IDC_FREEDGEANDFILL:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Draw.wFreeStyle = id - IDC_FREEDGE;
	Draw.FillInterior = !(id == IDC_FREEDGE);
	Draw.DrawExterior = !(id == IDC_FREFILL);
	break;

	case IDC_DRAW_LINE:
	case IDC_DRAW_BEZIER:
		// in case this comes from menu switch to Freehand mode
		if (Mask.PointEdit)
		{
		 	Mask.PointEdit = FALSE;
			CheckDlgButton(hDlg, IDC_POINTMODE, Mask.PointEdit);
			SetPointerDlgItems(POINTWND(hDlg));
			ControlEnable( hDlg, IDC_IS_POINT , Mask.PointEdit );
			ControlEnable( hDlg, IDC_IS_BEZIER, Mask.PointEdit );
		}
		bFreeBezier = (id == IDC_DRAW_BEZIER);
		CheckRadioButton( hDlg, IDC_DRAW_BEZIER,  IDC_DRAW_LINE, id);
		// allow Free proc to deal with the change
		if ( Tool.lpToolProc )
			MsgProc( PictPubApp.Get_hActiveWindow(), WM_CONTROLENABLE, 0, 0, 0);
	break;


	case IDC_FREEHAND:
	case IDC_POINTMODE:
	 	Mask.PointEdit = (id==IDC_POINTMODE);
		CheckDlgButton(hDlg, IDC_POINTMODE, Mask.PointEdit);
		if (Mask.PointEdit)
			CheckRadioButton( hDlg, IDC_DRAW_BEZIER, IDC_DRAW_LINE, -1);
		if ( Tool.lpToolProc )
			MsgProc( PictPubApp.Get_hActiveWindow(), WM_CONTROLENABLE, 0, 0, 0);
		// ShowPointerWindow(hDlg, Mask.PointEdit);
		SetPointerDlgItems(POINTWND(hDlg));
		ControlEnable( hDlg, IDC_IS_POINT , Mask.PointEdit );
		ControlEnable( hDlg, IDC_IS_BEZIER, Mask.PointEdit );
	break;
	
	case IDC_POINTTYPE:
	case IDC_MOVEPOINTS:
	case IDC_ADDPOINTS:
	case IDC_ADDBEZIER:
	case IDC_DELPOINTS:
	case IDC_IS_POINT:
	case IDC_IS_BEZIER:	
		HandlePointerFn(POINTWND(hDlg), id, codeNotify);
	break;

   default:
	break;
   }
}

/************************************************************************/
void DrawSquareProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
POINT pt;
DRAWSHAPE_PARMS parms;
LPIMAGE lpImage;
int res;
FRMTYPEINFO TypeInfo;
static int Style;

lpImage = GetImagePtr(hWindow);
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	if (!Retouch.hBrush)
		break;
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

	case WM_ACTIVATE:
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
	fShift = NO;
	AstralSetRectEmpty( &SelectRect );
	if (Draw.wSquareStyle >= IDC_SQUEDGE &&
		Draw.wSquareStyle <= IDC_SQUEDGEANDFILL)
		Style = SL_BOX;
	else
		Style = SL_ELLIPSE;
	Draw.FillInterior = !(Draw.wSquareStyle == IDC_SQUEDGE ||
							Draw.wSquareStyle == IDC_CIREDGE);
	Draw.DrawExterior = !(Draw.wSquareStyle == IDC_SQUFILL ||
							Draw.wSquareStyle == IDC_CIRFILL);
	pt.x = x; pt.y = y;
	StartSelection( hWindow, NULL, &SelectRect, Style | SL_SPECIAL,
		pt, 0L );
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	EndSelection( hWindow, NULL, &SelectRect, Style, YES );

	lpImage = GetImagePtr(hWindow);
   ImgGetTypeInfo(lpImage, &TypeInfo);
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	parms.ShapeType = (Style == SL_ELLIPSE ? ST_ELLIPSE : ST_RECTANGLE);
	if (Draw.FillInterior && Draw.DrawExterior)
		parms.FillStyle = FS_BORDERFILL;
	else
	if (Draw.FillInterior)
		parms.FillStyle = FS_FILL;
	else
		parms.FillStyle = FS_BORDER;
	parms.iBaseRes = res;
	parms.x1 = SelectRect.left;
	parms.y1 = SelectRect.top;
	parms.x2 = SelectRect.right;
	parms.y2 = SelectRect.bottom;

	GetDrawBrushSettings(&parms.Brush);
	GetActiveColorFromTypeInfo(TypeInfo, &parms.BorderColor);
	GetAlternateColorFromTypeInfo(TypeInfo, &parms.FillColor);
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_DRAWSHAPE, &parms);
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	fShift = SHIFT;
	pt.x = x; pt.y = y;
	UpdateSelection( hWindow, NULL, &SelectRect, Style,
		pt, CONSTRAINASPECT, 1L, 1L,
		MOVEKEY||Window.fRButtonDown, FROMCENTER);
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

#pragma optimize( "", off)
//************************************************************************
LOCAL BOOL SetupDrawPolyColorLut(FRMTYPEINFO TypeInfo, LPCOLORINFO lpBorderColor,
									LPCOLORINFO lpFillColor)
//************************************************************************
{
	LPTR        lpLut;
	int         i;
	BYTE        border[4];
	BYTE        fill[4];
	int         val;
   int         depth = 1;
   FRMDATATYPE Type = TypeInfo.DataType;
	

   if (Type == FDT_NONE)
      return(FALSE);
   if (Type == FDT_RGBCOLOR)
      depth = 3;
   else if (Type == FDT_CMYKCOLOR)
      depth = 4;

	lpLut = Alloc(256*depth);
	if (!lpLut)
		return(FALSE);
	lpColorLuts[0] = lpLut;
	for(i=1;i<depth;i++) 
		lpColorLuts[i] = &lpColorLuts[i-1][256];
	if (Type == FDT_CMYKCOLOR)
	   {
		border[0]   = lpBorderColor->cmyk.c; 
		border[1]   = lpBorderColor->cmyk.m; 
		border[2]   = lpBorderColor->cmyk.y; 
		border[3]   = lpBorderColor->cmyk.k; 
      fill[0]     = lpFillColor->cmyk.c;
      fill[1]     = lpFillColor->cmyk.m;
      fill[2]     = lpFillColor->cmyk.y;
      fill[3]     = lpFillColor->cmyk.k;
	   }
	else if (Type == FDT_RGBCOLOR)
	   {
		border[0]   = lpBorderColor->rgb.red;
		border[1]   = lpBorderColor->rgb.green;
		border[2]   = lpBorderColor->rgb.blue;
      fill[0]     = lpFillColor->rgb.red;
      fill[1]     = lpFillColor->rgb.green;
      fill[2]     = lpFillColor->rgb.blue;
	   }
	else if (Type == FDT_PALETTECOLOR)
      {
		border[0]   = lpBorderColor->index; 
      fill[0]     = lpFillColor->index;
      }
	else
      {
		border[0]   = lpBorderColor->gray; 
      fill[0]     = lpFillColor->gray;
      }
	while(depth--)
		for (i=0; i<=255; i++)
		{
			val = ((long)border[depth]*i+(long)fill[depth]*(255-i))/255;
			lpColorLuts[depth][i] = bound(val,0,255);
		}
	return(TRUE);
}
#pragma optimize( "", on)

//************************************************************************
LOCAL void FreeDrawShapeColorLut(void)
//************************************************************************
{
	FreeUp(lpColorLuts[0]);
}



/************************************************************************/
LOCAL void OSDrawShapeDataProc(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth,
								LPVOID lpParam)
/************************************************************************/
{
	int dx, width, d;
	LPTR lpMaskBuf, lpInterior;
	LPTR lpExteriorLine, lpExt;
	LPTR lpSource;
	
	if (lpDrawMask && (left < DrawMaskOffset.x || y < DrawMaskOffset.y))
		return; // should NEVER happen
	
	width = right-left+1;
	lpMaskBuf = lpExteriorLine = NULL;
	lpSource = lpSrc;
	
	if (lpExteriorFrame)
		lpExteriorLine = FramePointer(lpExteriorFrame, 
							left-ExteriorRect.left, y-ExteriorRect.top,NO);

	if (!lpExteriorLine)
	{
		// pure internal color
		lpSource = lpSrc;
		for (d=0;d<depth;d++)
		{
			dx = width;
			while (--dx >= 0)
			{
				*lpSrc = lpColorLuts[d][0];
				lpSrc += depth;
			}
			lpSrc = ++lpSource;
		}
		return;
	}
	
	if (lpDrawMask)
	{
		lpMaskBuf = lpDrawMask->mbuffer;
		MaskLoad(lpDrawMask, left-DrawMaskOffset.x, y-DrawMaskOffset.y,
					width, lpMaskBuf);
	}
	
	lpInterior = lpMaskBuf;
	lpExt = lpExteriorLine;
	// do one channel at a time, combining data
	for (d=0;d<depth;d++)
	{
		dx = width;
		if (!lpMaskBuf)
			while (--dx >= 0)
			{
				*lpSrc = lpColorLuts[d][255];
				lpSrc += depth;
			}
		else
			while (--dx >= 0)
			{
				if (!(*lpInterior++))
					// pure external color
					*lpSrc = lpColorLuts[d][255];
				else
					// combine data
					*lpSrc = lpColorLuts[d][*lpExt];
				lpSrc += depth;
				lpExt++;
			}
		// begin next plane
  		lpInterior = lpMaskBuf;
		lpExt = lpExteriorLine;
		lpSrc = ++lpSource; 
	}
}

/************************************************************************/
LOCAL void OSDrawShapeMaskProc(int y, int left, int right, LPTR lpMsk, LPVOID lpParam )
/************************************************************************/
{
	int width;
	LPTR lpMaskBuf;
	LPTR lpExteriorLine;

	if (lpDrawMask && (left < DrawMaskOffset.x || y < DrawMaskOffset.y))
		return; // should NEVER happen
	
	width = right-left+1;
	lpMaskBuf = lpExteriorLine = NULL;
	
	if (lpExteriorFrame)
		lpExteriorLine = FramePointer(lpExteriorFrame, 
							left-ExteriorRect.left, y-ExteriorRect.top,NO);
			
	if (lpDrawMask)
	{
		lpMaskBuf = lpDrawMask->mbuffer;
		MaskLoad(lpDrawMask, left-DrawMaskOffset.x, y-DrawMaskOffset.y,
					width, lpMaskBuf);
	}
	
	if (!lpExteriorLine && !lpMaskBuf)
		return; // should NEVER happen
	if (!lpExteriorLine)
	{ 	
	 	// mask interior only
		and( lpMaskBuf, lpMsk, width );	 
		return;
	}
	
	if (!lpMaskBuf)
		// Scale lpMsk by lpExteriorLine
		while (--width >= 0)
			*lpMsk++ = (*lpMsk*(long)*(lpExteriorLine++))/255;
	else
		while (--width >= 0)
		{
			if (*lpMaskBuf++)
			{
				// pure mask
				lpMsk++;
				lpExteriorLine++;
			}
			else
				// Scale lpMsk by lpExteriorLine
				*lpMsk++ = (*lpMsk*(long)*(lpExteriorLine++))/255;
		}
}


/************************************************************************/
LOCAL void DrawShapeMaskProc(int y, int left, int right, LPTR lpMsk )
/************************************************************************/
{
int dx;
LPTR lpMaskBuf;

dx = right-left+1;
lpMaskBuf = lpDrawMask->mbuffer;
MaskLoad(lpDrawMask, left-DrawMaskOffset.x, y-DrawMaskOffset.y,
			dx, lpMaskBuf);
while (--dx >= 0)
	{
	if (!(*lpMaskBuf++))
		*lpMsk = 0;
	++lpMsk;
	}
}

//************************************************************************/
//	Draws poly.
// 	Uses a buffer.
//************************************************************************/
LOCAL STATUS_CODE DrawPolyClosed( LPIMAGE lpImage, LPCPoly lpPoly, HMGXBRUSH hBrush,
					LPCOLORINFO lpBorderColor, LPCOLORINFO lpFillColor,
					BOOL	fInterior, BOOL fExterior,
					ITEMID idDirty, LPRECT lpUpdateRect)
//************************************************************************/
{
	int	BrushFull, BrushHalf;
	RECT rArea, rShape;
	LPMASK lpMask;
	ENGINE Engine;
	TFORM tform;
	int brushOpacity, brushScale;
   FRMTYPEINFO TypeInfo;
   FRMDATATYPE FrameType;
   FRMDATATYPE MaskType = FDT_GRAYSCALE;
   STATUS_CODE StatusCode;

   ImgGetTypeInfo(lpImage, &TypeInfo);
   if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
      MaskType = FDT_LINEART;

	BrushFull = GetMgxBrushSize(hBrush);
	BrushHalf = BrushFull / 2;

	if (!fInterior && !fExterior)
		return(SC_SUCCESS);

	lpMask = lpDrawMask = NULL;
	lpExteriorFrame = NULL;
	
	lpPoly->GetBounds(&rShape);
	// expand rArea to allow for brush
	rArea= rShape;
	rArea.top -= BrushHalf; // brush engine adds this to UndoRect
	rArea.left -= BrushHalf;
	rArea.bottom += (BrushFull-BrushHalf);
	rArea.right += (BrushFull-BrushHalf);

	/* setup interior */
	if ( fInterior )
	{
	   	// create bitmap from shape
	   	lpMask = MaskCreate(NULL, RectWidth(&rArea),RectHeight(&rArea),NO,YES, MaskType);
		if (!lpMask)
			goto ExitError;
		// move poly to (rArea.left,rArea.top)
		TInit(&tform);
		TMove(&tform,-rArea.left,-rArea.top);
		PolyTForm(lpPoly, &tform);
		// add poly
		if (!MaskAddPoly(lpMask, lpPoly, SHAPE_ADD,NO /*bAntialias*/))
			goto ExitError;
		// move poly back to original origin
		TInit(&tform);
		TMove(&tform, rArea.left, rArea.top);
		PolyTForm(lpPoly, &tform);

		DrawMaskOffset.x = rArea.left;
		DrawMaskOffset.y = rArea.top;
		lpDrawMask = lpMask;
	}
	
	ProgressBegin(fExterior ? 2:1, idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);
	
	if ( fExterior )
	{
		/* setup exterior */
		lpExteriorFrame = OSBrushPoly(lpPoly, hBrush, &ExteriorRect);
		if (!lpExteriorFrame)
			goto ExitError;
	}
	else 
		ExteriorRect = rArea;

	if (!SetupDrawPolyColorLut(TypeInfo, lpBorderColor, lpFillColor))
		goto ExitError;
		
  	brushScale = GetMgxBrushOpacityScale(hBrush);
	brushOpacity = GetMgxBrushOpacity(hBrush);
	brushOpacity = ((brushOpacity * brushScale) + 50) / 100;
	SetEngineDraw(&Engine, OSDrawShapeDataProc, brushOpacity,
				GetMgxBrushMergeMode(hBrush, NULL));
	Engine.lpEditRect = &ExteriorRect;
	Engine.lpMaskProc = OSDrawShapeMaskProc;
	Engine.fThread = NO;
	if (GetMgxBrushStyle(hBrush) == IDC_BRUSHSCATTER)
		Engine.ScatterPressure = GetMgxStylePressure(hBrush);
	StatusCode = LineEngineSelObj(lpImage, &Engine, idDirty);
	FreeDrawShapeColorLut();
	if (lpMask)
		MaskClose(lpMask);
	if (lpExteriorFrame)
		FrameClose(lpExteriorFrame);
	ProgressEnd();
	*lpUpdateRect = rArea;
	
	return(StatusCode);
	
ExitError:
	Message(IDS_EMEMALLOC);
	if (lpMask)
		MaskClose(lpMask);
	if (lpExteriorFrame)
		FrameClose(lpExteriorFrame);
	ProgressEnd();
	return(SC_MEMERROR);
}


//************************************************************************/
//	Draws poly.
//	Uses actual brush strokes.
//************************************************************************/
LOCAL STATUS_CODE DrawPolyOpen( LPIMAGE lpImage, LPCPoly lpPoly, HMGXBRUSH hBrush, int dirty, LPRECT lpUpdateRect )
//************************************************************************/
{
	BOOL fExterior;
	int	BrushFull, BrushHalf;
	RECT rShape;
	CPolyList PolyList;
	STATUS_CODE StatusCode;

	BrushFull = GetMgxBrushSize(Retouch.hBrush);
	BrushHalf = BrushFull / 2;
	fExterior = (BrushFull > 0) && Draw.DrawExterior;

	lpPoly->GetBounds(&rShape);

	/* Draw exterior */
	if (!PolyList.AddTail(lpPoly))
	{
		Message(IDS_EMEMALLOC);
		return(SC_MEMERROR);
	}
	StatusCode = BrushShapeObj(lpImage, &PolyList, hBrush,YES, dirty);
	PolyList.RemoveAll();
	
	rShape.top -= BrushHalf; // brush engine adds this to UndoRect
	rShape.left -= BrushHalf;
	rShape.bottom += (BrushFull-BrushHalf);
	rShape.right += (BrushFull-BrushHalf);
	*lpUpdateRect = rShape;
	return(StatusCode);
}

//************************************************************************
// 	Brushes all the shapes in the list into a frame.
//	Returns the frame, and sets lpOutRect to be the frames rectangle 
//		int the base.
//************************************************************************
LOCAL LPFRAME OSBrushPoly(LPCPoly lpPoly, HMGXBRUSH hBrush, LPRECT lpOutRect)
//************************************************************************
{
	int	BrushFull, BrushHalf;
	LPPOINT lpPoints;
	int nPoints, i;
	LPOBJECT lpObject = NULL;
	RECT rDraw;
	LPFRAME lpFrame;

	ProgressBegin(1,0);
	// get drawing area
	BrushFull = GetMgxBrushSize(hBrush);
	BrushHalf = BrushFull / 2;
	lpPoly->GetBounds(&rDraw);
	rDraw.top -= BrushHalf;
	rDraw.left -= BrushHalf;
	rDraw.bottom += (BrushFull-BrushHalf);
	rDraw.right += (BrushFull-BrushHalf);

	if (!MgxOSBrushBegin(hBrush, &rDraw, &rDraw))
		return(NULL);
		
	// get expanded point list
	lpPoly->GetPoint(-1);
	lpPoints = lpPoly->GetData(-1, NULL, &nPoints);
	MgxOSPickupBrush();
	for (i = 0; i < nPoints; i++)
	{
		AstralClockCursor( i, nPoints, NO );
		MgxOSBrushStroke(lpPoints[i].x, lpPoints[i].y);
	}
	lpFrame =  MgxOSBrushEnd(lpOutRect);
	FreeUp((LPTR)lpPoints);
	ProgressEnd();
	return(lpFrame);
}


//************************************************************************
//	brushes all shape in shape list into slected polygons
//************************************************************************
STATUS_CODE BrushShapeObj(LPIMAGE lpImage,LPCPolyList lpPolyList,HMGXBRUSH hBrush, BOOL bCacheInit, int dirty)
//************************************************************************
{
	int	BrushFull, BrushHalf;
	LPPOINT lpPoints;
	int nPoints, i, x, y, percent, nObjects;
	LPOBJECT lpObject;
	RECT rDraw, rTemp;
	POSITION pos;
	LPCPoly lpPoly;
	BOOL fCancelled;
	MGXBRUSHPARMS mgxbrush;

	ASSERT(lpImage);

	if (bCacheInit)
		if (!ImgEditInit(lpImage, ET_SELOBJECTS, UT_DATA, NULL))
		{
			return(SC_UNDOERROR);
		}
	// get drawing area
	BrushFull = GetMgxBrushSize(hBrush);
	BrushHalf = BrushFull / 2;

	AstralSetRectEmpty(&rDraw);
	PolyListGetBounds(lpPolyList, &rDraw);

	rDraw.top -= BrushHalf;
	rDraw.left -= BrushHalf;
	rDraw.bottom += (BrushFull-BrushHalf);
	rDraw.right += (BrushFull-BrushHalf);

	nObjects = 0;
	lpObject = NULL;
	while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
		if (AstralIntersectRect(&rTemp, &rDraw, &lpObject->rObject))
			++nObjects;
	} 

	fCancelled = NO;
	ProgressBegin(lpPolyList->GetCount()*ImgCountSelObjects(lpImage,lpObject), 0);
	lpObject = NULL;
	while (!fCancelled && (lpObject = ImgGetSelObject(lpImage, lpObject)))
	{
		if (!AstralIntersectRect(&rTemp, &rDraw, &lpObject->rObject))
			continue;
			
//		if (!MgxBrushBeginEx(PictPubApp.Get_hActiveWindow(), &lpObject->Pixmap,
//					lpObject->rObject.left, lpObject->rObject.top,
//	 				hBrush, NO, NO, NULL, NULL))

		MgxBrushSetAdvanced(&mgxbrush, NULL, &lpObject->Pixmap,
							lpObject->rObject.left, lpObject->rObject.top,
							hBrush, NO, lpImage, NULL, NULL);
		if (!MgxBrushBeginParms(&mgxbrush))
		{
			Message(IDS_EMEMALLOC);
			break;
		}

		pos = lpPolyList->GetHeadPosition();
		lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
		while (!fCancelled && lpPoly)
		{
			MgxOSPickupBrush();

			// get expanded point list
			lpPoly->GetPoint(-1);
			lpPoints = lpPoly->GetData(-1, NULL, &nPoints);
			ProgressBegin(1, 0);
			for (i = 0; i < nPoints; i++)
			{
				AstralClockCursor( i, nPoints, YES );
				x = lpPoints[i].x;
				y = lpPoints[i].y;
				MgxBrushStroke(x, y, x, y, 0, 0);
			}
			ProgressEnd();
			lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
		}
		MgxBrushEnd(NO);
		
		if (bCacheInit)
			ImgEditedObject(lpImage, lpObject, dirty, &rTemp);
	}
	ProgressEnd();
	return(SC_SUCCESS);
}

//************************************************************************
LOCAL BOOL count_points(LPCPoly lpPoly, LPLONG lpCount)
//************************************************************************
{
	*lpCount+=lpPoly->NumPoints();
	return(TRUE);
}

#ifdef UNUSED
//************************************************************************
//	brushes all shape in shape list  NOT USED.. EditInit needs work
//************************************************************************
LOCAL void BrushMaskSelObj(LPIMAGE lpImage,HMGXBRUSH hBrush, BOOL bCacheInit, int dirty)
//************************************************************************
{
	int	BrushFull, BrushHalf;
	LPOBJECT lpObject = NULL;
	RECT rDraw, rTemp;
	LPTR lpMaskBuf[3];
	BYTE p, n, c;
	LPTR lpP, lpC, lpN;
	int prev, curr, next, temp;
	int width, height, x, y, i;
	
	if (!lpImage)
		return;
	if (bCacheInit)
		if (!ImgEditInitSelObj(lpImage, YES, NO, NO))
			return;
	ProgressBegin(1,0);
	// get drawing area
	BrushFull = GetMgxBrushSize(hBrush);
	BrushHalf = BrushFull / 2;

	rDraw = lpDrawMask->rMask;
	width = RectWidth(&rDraw);
	height = RectHeight(&rDraw);
	OffsetRect(&rDraw, DrawMaskOffset.x, DrawMaskOffset.y);
	rDraw.top -= BrushHalf;
	rDraw.left -= BrushHalf;
	rDraw.bottom += (BrushFull-BrushHalf);
	rDraw.right += (BrushFull-BrushHalf);

	for (i = 0; i < 3; ++i)
		if (!(lpMaskBuf[i] = Alloc((long)width)))
		{
			if (i>=2)
				FreeUp(lpMaskBuf[1]);
			if (i>=1)
				FreeUp(lpMaskBuf[0]);
			ProgressEnd();
			return;
		}

	while (lpObject = ImgGetSelObject(lpImage, lpObject))
	{
		if (!AstralIntersectRect(&rTemp, &rDraw, &lpObject->rObject))
			continue;
		if (!MgxBrushBeginEx(PictPubApp.Get_hActiveWindow(), &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					hBrush, NO, NO, NULL, NULL))
			continue;
		// find edges and brush
		prev = 0;
		curr = 1;
		next = 2;
		// get mask for previous line
		clr( lpMaskBuf[prev], width);
		// get mask for first line
		y=0;
		if ( !(MaskLoader(lpDrawMask,0, y, width, lpMaskBuf[curr], NO, CR_COPY )) )
			clr( lpMaskBuf[curr], width );
		while ( y <= height)
		{
			if (!(MaskLoader(lpDrawMask, 0, y+1, width, lpMaskBuf[next], NO, CR_COPY )) )
				clr( lpMaskBuf[next], width );
			lpP = lpMaskBuf[prev];
			lpC = lpMaskBuf[curr];
			lpN = lpMaskBuf[next];
			// special case first pixel
			if (*lpC)
				MgxBrushStroke(DrawMaskOffset.x,DrawMaskOffset.y+y,0,0,0,0);
			p = *lpC++;
			++lpP;
			++lpN;
			c = *lpC++;
			x = 1;
			while(x < width)
			{
				n = *lpC++;
				if (c)  // a pixel is on
					if (!(p && n && *lpP && *lpN)) // is any neighbor off?
						MgxBrushStroke(0,0,DrawMaskOffset.x+x,DrawMaskOffset.y+y,0,0);
				p = c;
				c = n;
				++lpP; ++lpN;
				++x;
			}
			// special case las pixel
			if (c)
				MgxBrushStroke(DrawMaskOffset.x,DrawMaskOffset.y+y,0,0,0,0);
			temp = prev;
			prev = curr;
			curr = next;
			next = temp;
			++y;
		}
		MgxBrushEnd(NO);
//		lpObject->Pixmap.dirty = dirty;
//		lpObject->fChanged = YES;
	}
	for (i = 0; i < 3; ++i)
		FreeUp(lpMaskBuf[i]);
	ProgressEnd();
}

#endif

/************************************************************************/
BOOL DrawShapeImage(LPIMAGE lpImage, LPDRAWSHAPE_PARMS lpParms)
/************************************************************************/
{
int res;
RECT rRect;
BOOL fRet;
HMGXBRUSH hBrush;
FRMTYPEINFO TypeInfo;

ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
ImgGetTypeInfo(lpImage, &TypeInfo);
lpParms->BorderColor.index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpParms->BorderColor.rgb);
lpParms->FillColor.index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpParms->FillColor.rgb);
hBrush = CreateDrawBrush(&lpParms->Brush, IDN_DRAWSHAPESTYLE, &lpParms->BorderColor, lpParms->iBaseRes, res, TypeInfo);
if (!hBrush)
	return(FALSE);

SetRect(&rRect, lpParms->x1, lpParms->y1, lpParms->x2, lpParms->y2);
ResConvertRect(lpParms->iBaseRes, res, &rRect);

if (lpParms->ShapeType == ST_ELLIPSE)
	fRet = DrawEllipse( lpImage, lpParms, &rRect, hBrush );
else
	fRet = DrawRectangle( lpImage, lpParms, &rRect, hBrush );
DestroyMgxBrush(hBrush);
return(fRet);
}

/************************************************************************/
BOOL DrawFreeImage(LPIMAGE lpImage, LPDRAWFREE_PARMS lpParms)
/************************************************************************/
{
	int res;
	RECT rUpdate, rTotal;
	BOOL fInterior, fExterior;
	HMGXBRUSH hBrush;
	FRMTYPEINFO TypeInfo;
	CPolyList PolyList;
	POSITION pos;
	LPCPoly lpPoly;
	
	ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
	ImgGetTypeInfo(lpImage, &TypeInfo);
   lpParms->BorderColor.index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpParms->BorderColor.rgb);
   lpParms->FillColor.index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpParms->FillColor.rgb);
	hBrush = CreateDrawBrush(&lpParms->Brush,
					lpParms->fClosed ? IDN_DRAWFREESTYLE : IDN_PENCILSTYLE,
					&lpParms->BorderColor, lpParms->iBaseRes, res, TypeInfo);
	if (!hBrush)
		return(FALSE);
	
	// convert back to a polygon
	if (!IntPoints2PolyList(&PolyList, lpParms->lpPoints,lpParms->nPoints, lpParms->iBaseRes, res))
	{
		DestroyMgxBrush(hBrush);
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	
	// make closed or opened shapes
	PolyList.ForEach1((LPLISTPROC1)close_poly, (LONG)lpParms->fClosed);
	
	// render each polygon
	ProgressBegin(PolyList.GetCount(), 0);
	
	AstralSetRectEmpty(&lpParms->Common.rUpdateArea);
	AstralSetRectEmpty(&rTotal);
	pos = PolyList.GetHeadPosition();
	while (lpPoly = (LPCPoly)PolyList.GetNextPtr(pos))
	{
		if (lpParms->fClosed)
		{
			fInterior = lpParms->FillStyle == FS_BORDERFILL || lpParms->FillStyle == FS_FILL;
			fExterior = (GetMgxBrushSize(hBrush) > 0) && 
				(lpParms->FillStyle == FS_BORDERFILL || lpParms->FillStyle == FS_BORDER);
			lpParms->Common.StatusCode = DrawPolyClosed( lpImage, lpPoly, hBrush,
						&lpParms->BorderColor, &lpParms->FillColor,
						fInterior, fExterior, lpParms->Common.idDirty, &rUpdate);
		}
		else
			lpParms->Common.StatusCode = DrawPolyOpen( lpImage, lpPoly, hBrush, lpParms->Common.idDirty, &rUpdate );
		AstralUnionRect(&rTotal, &rTotal, &rUpdate);
		if (!AstralIsRectEmpty(&rTotal))
		{
			lpParms->Common.UpdateType = UT_AREA;
			lpParms->Common.rUpdateArea = rTotal;
		}
		if (lpParms->Common.StatusCode)
			break;
	}
	DestroyMgxBrush(hBrush);
	ProgressEnd();
	FreePolys(&PolyList);
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//=======================================================================
LOCAL BOOL close_poly(LPCPoly lpPoly, LONG bClosed)
//=======================================================================
{
	lpPoly->ClosePoly(bClosed);
	return(TRUE);
}

//************************************************************************
// Renders an ellipse -- mirrors DrawShape
//************************************************************************
LOCAL BOOL DrawEllipse(LPIMAGE lpImage, LPDRAWSHAPE_PARMS lpParms,
						LPRECT lpRect, HMGXBRUSH hBrush)
//************************************************************************
{
	BOOL fInterior, fExterior;
	int	BrushFull, BrushHalf;
	LPMASK lpMask;
	ENGINE Engine;
	RECT rArea, rEllipse;
	int brushOpacity, brushScale;
   FRMTYPEINFO TypeInfo;

	fInterior = lpParms->FillStyle == FS_BORDERFILL || lpParms->FillStyle == FS_FILL;
	BrushFull = GetMgxBrushSize(hBrush);
	BrushHalf = BrushFull / 2;
	fExterior = (BrushFull > 0) && 
		(lpParms->FillStyle == FS_BORDERFILL || lpParms->FillStyle == FS_BORDER);

	if (!fInterior && !fExterior)
		return(FALSE);

	lpMask = lpDrawMask = NULL;
	lpExteriorFrame = NULL;
	
	// expand rShape to allow for brush
	rEllipse = *lpRect;
	
	ProgressBegin(fExterior ? 2:1, lpParms->Common.idDirty-IDS_UNDOFIRST+IDS_PROGFIRST);

    ImgGetTypeInfo(lpImage, &TypeInfo);
	if (fInterior)
	{
      FRMDATATYPE FrameType = FDT_GRAYSCALE;

		rArea = *lpRect;
		rArea.top -= BrushHalf; // brush engine adds this to UndoRect
		rArea.left -= BrushHalf;
		rArea.bottom += (BrushFull-BrushHalf);
		rArea.right += (BrushFull-BrushHalf);
      	if (TypeInfo.DataType == FDT_LINEART && Mask.OneBitMask)
       		FrameType = FDT_LINEART;

		/* Draw interior */
		// create bitmap from shape
		lpMask = MaskCreate(NULL, RectWidth(&rArea),RectHeight(&rArea),NO,YES, FrameType);
		if (!lpMask)
			goto ExitError;
		
		// setup interior 
		// move rect to origin
		OffsetRect(&rEllipse, -rArea.left, -rArea.top);
		MaskAddEllipse(lpMask, &rEllipse, SHAPE_ADD);
		// move shapes back to original origin
		DrawMaskOffset.x = rArea.left;
		DrawMaskOffset.y = rArea.top;
		lpDrawMask = lpMask;
	}
	
	rEllipse = *lpRect;
	
	/* setup exterior */
	if ( fExterior )
	{
		// stroke the interior
		lpExteriorFrame = StrokeEllipse(hBrush, &rEllipse, &ExteriorRect);
		if (!lpExteriorFrame)
			goto ExitError;
	}
	else 
		ExteriorRect = rArea;
	
	// draw it
	if (!SetupDrawPolyColorLut(TypeInfo, &lpParms->BorderColor, &lpParms->FillColor))
		goto ExitError;
  	brushScale = GetMgxBrushOpacityScale(hBrush);
	brushOpacity = GetMgxBrushOpacity(hBrush);
	brushOpacity = ((brushOpacity * brushScale) + 50) / 100;

	SetEngineDraw(&Engine, OSDrawShapeDataProc, brushOpacity, GetMgxBrushMergeMode(hBrush, NULL));
	Engine.lpEditRect = &ExteriorRect;
	Engine.lpMaskProc = OSDrawShapeMaskProc;
	Engine.fThread = NO;
	if (GetMgxBrushStyle(hBrush) == IDC_BRUSHSCATTER)
		Engine.ScatterPressure = GetMgxStylePressure(hBrush);
	lpParms->Common.StatusCode = LineEngineSelObj(lpImage, &Engine, lpParms->Common.idDirty);
	FreeDrawShapeColorLut();
	if (lpMask)
		MaskClose(lpMask);
	if (lpExteriorFrame)
		FrameClose(lpExteriorFrame);
	ProgressEnd();
	AstralUnionRect(&rArea, &rArea, &Engine.rUpdate);
	lpParms->Common.UpdateType = UT_AREA;
	lpParms->Common.rUpdateArea = rArea;
	return(lpParms->Common.StatusCode == SC_SUCCESS);
	
ExitError:
	Message(IDS_EMEMALLOC);
	if (lpMask)
		MaskClose(lpMask);
	if (lpExteriorFrame)
		FrameClose(lpExteriorFrame);
	ProgressEnd();
	return(FALSE);
}

//************************************************************************
LOCAL BOOL DrawRectangle(LPIMAGE lpImage, LPDRAWSHAPE_PARMS lpParms,
						LPRECT lpRect, HMGXBRUSH hBrush)
//************************************************************************
{
	CPoly Poly;
	BOOL fInterior, fExterior, fRet;
	RECT rUpdate;
	
	if (!MakeRectPoly(&Poly, lpRect))
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	fInterior = lpParms->FillStyle == FS_BORDERFILL || lpParms->FillStyle == FS_FILL;
	fExterior = (GetMgxBrushSize(hBrush) > 0) && 
		(lpParms->FillStyle == FS_BORDERFILL || lpParms->FillStyle == FS_BORDER);
	lpParms->Common.StatusCode = DrawPolyClosed( lpImage, &Poly, hBrush,
					&lpParms->BorderColor, &lpParms->FillColor,
					fInterior, fExterior, lpParms->Common.idDirty, &rUpdate);
	if (lpParms->Common.StatusCode == SC_SUCCESS)
	{
		lpParms->Common.UpdateType = UT_AREA;
		lpParms->Common.rUpdateArea = rUpdate;
	}
	return(lpParms->Common.StatusCode == SC_SUCCESS);
}

//************************************************************************
void GetDrawBrushSettings(LPDRAWBRUSH lpBrush)
//************************************************************************
{
lstrcpy(lpBrush->StyleName, Retouch.DrawStyleName[Control.idDrawTool-IDC_FIRST_DRAWTOOL]);
lpBrush->BrushShape = Retouch.BrushShape;
if (Retouch.BrushShape == IDC_BRUSHCUSTOM)
	lstrcpy(lpBrush->ShapeName, Names.CustomBrush);
else
	lpBrush->ShapeName[0] = '\0';
lpBrush->Size = Retouch.BrushSize;
lpBrush->Feather = Retouch.Feather;
lpBrush->Opacity = Retouch.Opacity;
lpBrush->MergeMode = Retouch.MergeMode;
}

//************************************************************************
LOCAL HMGXBRUSH CreateDrawBrush(LPDRAWBRUSH lpBrush, ITEMID idType, LPCOLORINFO lpColorInfo,
								 int iSrcRes, int iDstRes, FRMTYPEINFO TypeInfo)
//************************************************************************
{
HMGXBRUSH hBrush;
int Size;

hBrush = CreateMgxBrush(lpBrush->StyleName, idType, TypeInfo);
if (!hBrush)
	return(NULL);
Size = lpBrush->Size;
ResConvertDist(iSrcRes, iDstRes, &Size);
SetMgxBrushSize(hBrush, Size);
SetMgxBrushShape(hBrush, lpBrush->BrushShape, lpBrush->ShapeName);
SetMgxBrushOpacity(hBrush, lpBrush->Opacity);
SetMgxBrushMergeMode(hBrush, lpBrush->MergeMode);
if (TypeInfo.DataType == FDT_PALETTECOLOR)
   SetMgxBrushFeather(hBrush, 0);
else
   SetMgxBrushFeather(hBrush, lpBrush->Feather);
SetMgxBrushColor(hBrush, lpColorInfo);
return(hBrush);
}

