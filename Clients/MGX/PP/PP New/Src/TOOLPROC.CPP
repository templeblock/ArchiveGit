//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "svrdoc.h"
#include "svrview.h"
#include "id.h"

// Static prototypes
class CShapeEditDrawHook : public CHookHandler
{
public:
    CShapeEditDrawHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CShapeEditDrawHook ShapeEditDrawHook;

LOCAL void pointedit(HWND hWindow, LPCPolyList lpPolyList, int iEraseVal,WORD iMode);

// Retouch statics
static POINT StartPoint;
static RECT ClientRect, SelectRect;

// for point edit
static POINT SaveFramePos;
static BOOL fSelecting;
static BOOL bGotShape, fDisplaysInfo;
static int inout;
static LPFRAME lpSaveFrame;

// for point editing 
CShapeEdit PointEditShape;							

static BOOL bLeft = TRUE;  // cursor direction


/************************************************************************/
void CustomProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
BOOL fGotaRect;
POINT pt;
static BOOL fSelection;

switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	ClientRect = GetDisplayPtr(hWindow)->DispRect;
	StartPoint.x = x; StartPoint.y = y;
	fSelection = NO;
	break;

	case WM_LBUTTONUP:
	Tool.bActive = NO;
	if (fSelection)
		{
		EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
		if (abs(SelectRect.right-SelectRect.left) <= SMALL_MOVEMENT &&
			abs(SelectRect.bottom-SelectRect.top) <= SMALL_MOVEMENT)
				fGotaRect = NO;
		else	fGotaRect = YES;
		}
	else
		{
		fGotaRect = NO;
		Display2File(hWindow, (LPINT)&StartPoint.x, (LPINT)&StartPoint.y );
		SelectRect.left = SelectRect.right = StartPoint.x;
		SelectRect.top = SelectRect.bottom = StartPoint.y;
		}

	if ( fGotaRect )
		ViewRect(hWindow, &SelectRect, View.ZoomWindow);
	else
		{
		SaveLastView(hWindow);
		Zoom( hWindow, SelectRect.left, SelectRect.top,
			( View.ZoomOut ^ SHIFT ) ? -100 : +100, YES,
			( View.ZoomWindow ^ CONTROL ) );
		}
	Tool.bActive = NO;
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	if (!fSelection)
		{
		if (fSelection = (abs(StartPoint.x-x) > SMALL_MOVEMENT ||
			abs(StartPoint.y-y) > SMALL_MOVEMENT))
			{
			AstralUpdateWindow(hWindow);
			AstralSetRectEmpty( &SelectRect );
			StartSelection( hWindow, NULL, &SelectRect,
				SL_BOX | SL_SPECIAL, StartPoint, 0L );
			}
		}
	if (fSelection)
		{
		pt.x = x; pt.y = y;
		UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
			pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
		}
	break;

	case WM_LBUTTONDBLCLK:
	break;

	case WM_DESTROY:	// The cancel operation message
	if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	if (fSelection)
		EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	break;
	}
if (!Tool.bActive)
	InstallTool(NO);
}


/************************************************************************/
void PointerProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
	int function;
	WORD iMode;
	LPCPolyList lpPolyList, lpNewPolyList;
	LPCPoly lpNewPoly, lpOldPoly;
	LPFRAME lpFrame;
	LPMASK lpMask;
	RECT rArea;
	BOOL fCancel = FALSE;
	WORD wKey;
	LPOBJECT lpBase;
	POINT pt;
	LPIMAGE lpImage;

	
	lpImage = GetImagePtr(hWindow);
	switch (msg)
	{
		case WM_CREATE:	// The first mouse down message
			if (!IsOnImage(hWindow, x, y))
				break;
			Tool.bActive = YES;
			bGotShape = NO;
			lpSaveFrame = NULL;
			fSelecting = NO;
			SetDisplayHook(hWindow, NULL);
			AstralUpdateWindow(hWindow);
//			SendRibbonEnable(0);
		break;

		case WM_LBUTTONDOWN:
			if (bGotShape)
			{
				PointEditShape.MouseDown(x,y);
				SendRibbonEnable(1);
				break;
			}
			if (fSelecting)
				break;
			ClientRect = GetDisplayPtr(hWindow)->DispRect;
			AstralSetRectEmpty( &SelectRect );
			fDisplaysInfo = Tool.fDisplaysInfo;
			Tool.fDisplaysInfo = YES;
			pt.x = x; pt.y = y;
			StartSelection( hWindow, NULL, &SelectRect, SL_BOX | SL_SPECIAL,
				pt, 0L );
			fSelecting = YES;
		break;

		case WM_LBUTTONUP:
			if (bGotShape)
			{
				PointEditShape.MouseUp(x,y);
				SendRibbonEnable(1);
				break;
			}
			if (!fSelecting)
				break;
			// get rect
			EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
			fSelecting = NO;
			Tool.fDisplaysInfo = fDisplaysInfo;
//			File2DispRect(hWindow, &SelectRect, &SelectRect );
			// get and install shape
			if (!(lpMask = ImgGetMask(lpImage)))
				break;
			if (!(lpFrame = lpMask->Pixmap.EditFrame))
				break;
			rArea.top = rArea.left = 0;
			rArea.right = FrameXSize(lpFrame) - 1;
			rArea.bottom = FrameYSize(lpFrame) - 1;
			if ( abs( SelectRect.right - SelectRect.left ) > SMALL_MOVEMENT ||
				 abs( SelectRect.bottom - SelectRect.top ) > SMALL_MOVEMENT )
					AstralIntersectRect(&rArea, &SelectRect, &rArea);
			lpBase = ImgGetBase(lpImage);
			if (!ImgEditInit(lpImage, ET_OBJECT, UT_ALPHA, lpBase))
				break;
			lpPolyList = GetPath(&rArea, lpFrame, 127, &inout);
			if (!lpPolyList)
				break;
			// compress the poly list
			lpNewPolyList = new CPolyList;
			if (!lpNewPolyList)
			{
				FreePolyList(lpPolyList);
				break;
			}
			while(!lpPolyList->IsEmpty())
			{
				lpOldPoly = (LPCPoly)lpPolyList->RemoveHeadPtr();
				if (lpOldPoly)
				{
					if (Mask.MakePathType == IDC_PATH_LINES)
					{
						lpOldPoly->Squeeze(Mask.MakePathTolerance);
						lpNewPolyList->AddHead(lpOldPoly);
					}
					else
					{
						lpOldPoly->Squeeze(1);
						lpNewPoly = BezCompressPoly(lpOldPoly, Mask.MakePathContinuity, Mask.MakePathCurveTolerance);
						if (!lpNewPoly)
							lpNewPolyList->AddHead(lpOldPoly);
						else
						{
							lpNewPolyList->AddHead(lpNewPoly);
							delete lpOldPoly;
						}
					}
												
				}
			}
			delete lpPolyList;
			lpPolyList = lpNewPolyList;
			
//			CompressShapes(lpPolyList, Mask.MakePathMinEdge, Mask.MakePathTolerance);
			// if we are in subtract mode then must invert in/out
			// cache frame and erase the area
			lpSaveFrame = ClearMaskRect(lpImage, &rArea, (BYTE)(inout ? 255:0), YES);
			SaveFramePos.x = rArea.left;
			SaveFramePos.y = rArea.top;
			bGotShape = TRUE;
			AstralUpdateWindow(hWindow);
			PointEditShape.Init(hWindow, lpPolyList); 
			FreePolyList(lpPolyList);
			PointEditShape.CloseShape(YES);
			PointEditShape.Draw(Window.hDC, ON);
			SetDisplayHook(hWindow, &ShapeEditDrawHook);
			SendRibbonEnable(0);
		break;

		case WM_MOUSEMOVE:	// sent when ToolActive is on
			if (bGotShape)
			{
				PointEditShape.MouseMove(x,y);
				break;
			}
			if (!fSelecting)
				break;
			pt.x = x; pt.y = y;
			UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
				pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
		break;
		
		case WM_SETCURSOR:
			if (bGotShape)
			{
				function = PointEditShape.GetExtendedFunction();
				SetPointerCursor(function, bLeft);
			}
			else
			   SetWindowCursor(Tool.Prim.idCursor);
		break;
		
		case WM_KEYDOWN:
			wKey = Option;
			if (wKey == 'O')
				PointerProc(hWindow, WM_COMMAND, x, y, IDM_LOADSHAPE);
			
			if (wKey == 'S')
				PointerProc(hWindow, WM_COMMAND, x, y, IDM_SAVESHAPE);
			
			if (!bGotShape)
				break;
			if (wKey=='D')
			{
				bLeft = !bLeft;
				CursorProc(hWindow, 0L);
			}
			PointEditShape.KeyDown(wKey);
			SendRibbonEnable(1);
			CursorProc(hWindow, 0L);
		break;
		
		case WM_KEYUP:
			if (!bGotShape)
				break;
			PointEditShape.KeyUp(wKey);
			SendRibbonEnable(1);
			CursorProc(hWindow, 0L);
		break;
		
		case WM_DESTROY:	// The cancel operation message
			fCancel = Option == 1L;      
			if ( !Tool.bActive )
				break;
			Tool.bActive = NO;
			if ( fSelecting )
			{
				Tool.fDisplaysInfo = fDisplaysInfo;
				EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
			}
			if ( !bGotShape)
				break;
			// undraw shape
			PointEditShape.Draw(NULL, OFF);
			SetDisplayHook(hWindow, NULL);
			lpPolyList = PointEditShape.Done();
			if (!lpPolyList)
				fCancel = TRUE;
			if (lpSaveFrame && fCancel)
			{
				rArea.left = SaveFramePos.x;
				rArea.top = SaveFramePos.y;
				rArea.right = rArea.left+FrameXSize(lpSaveFrame)-1;
				rArea.bottom = rArea.top+FrameYSize(lpSaveFrame)-1;
				RestoreMaskRect(&rArea, lpSaveFrame);
				// 	ImgEditInit done in WM_LBUTTONUP
				lpBase = ImgGetBase(lpImage);
				ImgEditedObject(lpImage, lpBase, IDS_UNDOPOINTEDIT, &rArea);
			} 
			else if (!fCancel)
			{
				lpBase = ImgGetBase(lpImage);
				// set mask merge mode
				iMode = Mask.PointIModes;
//				if (inout)
//				{
//					if (iMode == SHAPE_ADD)
//						iMode = SHAPE_SUBTRACT;
//					else if (iMode == SHAPE_SUBTRACT)
//						iMode = SHAPE_ADD;
//				}
				if (!inout && (iMode == SHAPE_SUBTRACT))
		  				iMode = SHAPE_ADD;

				// pointedit will do ImgEditedObject
				pointedit(hWindow, lpPolyList, (inout ? 255:0), iMode);
			}
			if (lpSaveFrame)
				FrameClose(lpSaveFrame);
			lpSaveFrame = NULL;
			FreePolyList(lpPolyList);
			bGotShape = FALSE;
			SendRibbonEnable(0);
			UpdateStatusBar( YES, NO, YES, NO );
		break;

		case WM_COMMAND:
			switch (Option)
			{
				case IDM_LOADSHAPE:
					if (Tool.bActive && fSelecting)
						break;
					if (!(lpPolyList = DoShapeLoadDlg(hWindow)))
						break;
					if( lpPolyList->IsEmpty())
					{
						FreePolyList(lpPolyList);
						break;
					}
					if (!Tool.bActive || (Tool.bActive && !bGotShape))
					{
						// new shape
						Tool.bActive = TRUE;
						lpSaveFrame = NULL;
						fSelecting = NO;
						bGotShape = TRUE;
						PointEditShape.Init(hWindow, lpPolyList); 
						PointEditShape.CloseShape(YES);
						PointEditShape.Draw(Window.hDC, ON);
						SetDisplayHook(hWindow, &ShapeEditDrawHook);
						SendRibbonEnable(0);
					}
					else
					{
						// add to existing shape
						PointEditShape.AddPolyList(lpPolyList);
					}
					FreePolyList(lpPolyList);
				break;
				case IDM_SAVESHAPE:
					DoShapeSaveDlg(hWindow, PointEditShape.PeekPolyList());
				break;
				case IDM_DELETEITEM:
					if (!bGotShape)
						break;
					Tool.bActive = NO;
					SetDisplayHook(hWindow, NULL);
					lpPolyList = PointEditShape.Done();
					FreePolyList(lpPolyList);
					SendRibbonEnable(0);
					UpdateStatusBar( YES, NO, YES, NO );
				break;
			}
		break;
	}
	if (!Tool.bActive)
		InstallTool(NO);
}


//************************************************************************/
LOCAL void pointedit(HWND hWindow, LPCPolyList lpPolyList, int iEraseVal,WORD iMode)
//************************************************************************/
{
	POINTEDIT_PARMS parms;
	int res;
	LPIMAGE lpImage;

	lpImage = GetImagePtr(hWindow);
	if (!lpImage || !lpPolyList || lpPolyList->IsEmpty())
		return;
	res = FrameResolution(ImgGetBaseEditFrame(lpImage));
	lpPolyList->ForEach((LPLISTPROC)PolySqueeze);
	ImgGetInfoEx(lpImage, NULL, NULL, NULL, NULL, &res);
	parms.lpPoints = PolyList2IntPoints(lpPolyList, &parms.nPoints);
	if (!parms.lpPoints)
		return;
	parms.Mode = iMode;
	parms.Antialias = Mask.PointAntialias;
	parms.iBaseRes = res;
	// set erase rect empty because we already erased
	parms.rErase.left 	= 100;
	parms.rErase.top 	= 100;
	parms.rErase.right 	= -100;
	parms.rErase.bottom = -100;
	parms.iEraseVal = iEraseVal;
	parms.Common.idDirty = IDS_UNDOPOINTEDIT; 
	PointEditImageEx(lpImage,&parms,NO);
	if (lpSaveFrame)
	{
		parms.rErase.left 	= SaveFramePos.x;
		parms.rErase.top 	= SaveFramePos.y;
		parms.rErase.right 	= SaveFramePos.x + FrameXSize(lpSaveFrame) - 1;
		parms.rErase.bottom = SaveFramePos.y + FrameYSize(lpSaveFrame) - 1;
	}
	
	// could be smarter here 
	UpdateImage(lpImage, NULL, YES);
	
	PostCommand(lpImage->lpCmdList, IDS_CMD_POINTEDIT, &parms);
}

/************************************************************************/
void CShapeEditDrawHook::DisplayHookProc( HWND hWnd, HDC hDC, LPRECT lpRect )
/************************************************************************/
{
	PointEditShape.Draw(hDC, ON);
}

/************************************************************************/
void GrabberProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
int dx, dy, fx, fy, fxleft, fxright, fytop, fybottom;
RECT rScroll, FileRect;
LPDISPLAY lpDisplay;
LPIMAGE lpImage;
static POINT Anchor;

switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	Tool.bActive = YES;
	break;

	case WM_LBUTTONDOWN:
	Anchor.x = x;
	Anchor.y = y;
	Display2File(hWindow, (LPINT)&Anchor.x, (LPINT)&Anchor.y);
	EnableMarquee( NO );
	RemoveMarquee();
	break;

	case WM_LBUTTONUP:
	EnableMarquee( YES );
	Tool.bActive = NO;
	break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
	if ( !Window.fLButtonDown )
		break;
	lpImage = GetImagePtr(hWindow);
	lpDisplay = GetDisplayPtr(hWindow);
	Display2File(hWindow,&x, &y);
	fx = x - Anchor.x;
	fy = y - Anchor.y;
	FileRect = lpDisplay->FileRect;
	fxleft = FileRect.left;
	fxright = FileRect.right - lpImage->npix + 1; 
	fytop = FileRect.top;
	fybottom = FileRect.bottom - lpImage->nlin + 1;
	if (fx > 0 && fx > fxleft)
		fx = fxleft;
	if (fx < 0 && fx < fxright)
		fx = fxright;
	if (fy > 0 && fy > fytop)
		fy = fytop;
	if (fy < 0 && fy < fybottom)
		fy = fybottom;
	dx = FMUL( fx, lpDisplay->DispRate );
	dy = FMUL( fy, lpDisplay->DispRate );
	if ( !dx && !dy )
		break;
	rScroll = lpDisplay->DispRect;
	rScroll.bottom++; rScroll.right++;
	if (dx)
		{
		lpDisplay->FileRect.left -= fx;
		lpDisplay->FileRect.right -= fx;
		lpDisplay->xDiva -= dx;
		ScrollWindow( hWindow, dx, 0, &rScroll, &rScroll);
		}
	if (dy)
		{
		lpDisplay->FileRect.top -= fy;
		lpDisplay->FileRect.bottom -= fy;
		lpDisplay->yDiva -= dy;
		ScrollWindow( hWindow, 0, dy, &rScroll, &rScroll);
		}
	{
	CWnd *pView = CWnd::FromHandle(hWindow);
	if (pView && pView->IsKindOf(RUNTIME_CLASS(CServerView)))
		((CServerView*)pView)->UpdateWindowNow();
	else
		AstralUpdateWindow( hWindow );
	}
	SetupRulers(hWindow);
	ZoomMainViewChanged( hWindow );
	Anchor.x = x-fx;
	Anchor.y = y-fy;
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