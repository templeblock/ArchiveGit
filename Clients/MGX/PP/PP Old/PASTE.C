// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HANDLE	hInstAstral;
extern HWND	hWndAstral;

static LPSHAPE lpPasteShape;	/* The clipbaord mask */
static LPFRAME lpClip;		/* The clipboard frame buffer */
static TFORM PasteTForm;
static BOOL PasteActive;

/***********************************************************************/
BOOL FAR PASCAL DlgPasteFromFileProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	lstrcpy( Names.Saved, Names.Clipboard );
	CenterPopup( hDlg );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Clipboard,
		IDN_CLIPBOARD );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDOK:
		if (!LookupExtFile(Names.Clipboard, Names.ClipImageFile,
		    IDN_CLIPBOARD))
			break;
		if (!LookupExtFile(Names.Clipboard, Names.ClipMaskFile,
		    IDN_MASK))
			break;
		AstralDlgEnd( hDlg, TRUE);
		break;

	    case IDCANCEL:
		lstrcpy( Names.Clipboard, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Clipboard, IDN_CLIPBOARD, HIWORD(lParam) );
		break;

	    case IDC_CLIPEXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Clipboard, IDN_CLIPBOARD, wParam, YES );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/************************************************************************/
int PasteProc( hWindow, lParam, msg )
/************************************************************************/
HWND hWindow;
long lParam;
int msg;
{
int x, y;
static BOOL down;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_LBUTTONDOWN:
	down = TRUE;
        ShapeTransformMouseDown(x, y);
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
        if (down)
            ShapeTransformMouseMove(x, y);
	break;

    case WM_LBUTTONUP:
	down = FALSE;
        if (TrackMoved())
            PasteRefresh();
	break;

    case WM_LBUTTONDBLCLK:
        DonePaste(FALSE);
	break;

    case WM_DESTROY:	// The cancel operation message
	DonePaste(lParam == 1);
	break;
    }
}

/************************************************************************/
BOOL InitPaste()
/************************************************************************/
{
int tx, ty, bpp, type;
POINT point;
TFORM DispTForm;
RECT rMask;

if ( !lpImage )
	return(FALSE);
if ( lpClip )
	frame_close( lpClip );
if ( !(lpClip = AstralFrameLoad(Names.ClipImageFile, DEPTH, &bpp, &type)) )
	return( FALSE );
if ( !(lpPasteShape = ReadMask(Names.ClipMaskFile)) )
	{
	frame_close(lpClip);
	lpClip = NULL;
	return( FALSE );
	}

DisplayBrush(0, 0, 0, OFF);
SetClassWord( lpImage->hWnd, GCW_HCURSOR, LoadCursor(NULL, IDC_ARROW) );

RemoveMask();
if (!CacheInit( NO, YES ))
    {
    frame_close(lpClip);
    lpClip = NULL;
    FreeUpShapes(lpPasteShape);
    SetClassWord(lpImage->hWnd, GCW_HCURSOR, Window.hCursor);
    return(FALSE);
    }
lpImage->lpDisplay->FloatingImageProc = PasteFloatProc;
lpImage->lpDisplay->FloatingGraphicsProc = ShapeFloatProc;

Mask.IsOutside = FALSE;
UpdateStatusBar( NO, NO, YES, NO );
TInit( &DispTForm );
/* Move the object so it is over the origin */
TMove( &DispTForm,  -(int)((lpClip->Xsize)/2), -(int)((lpClip->Ysize)/2) );
/* Scale it to fit the current display scaling */
TScale( &DispTForm, FGET( DISP_DX, FILE_DX ), FGET( DISP_DY, FILE_DY ) );
/* Move it under the cursor point */
tx = DISP_DX/2;
ty = DISP_DY/2;
TMove( &DispTForm, tx, ty );

point.x = 0; point.y = 0;
Transformer( &DispTForm, &point, &tx, &ty );
if (tx < lpImage->lpDisplay->DispRect.left)
    TMove(&DispTForm, lpImage->lpDisplay->DispRect.left - tx + 5, 0);
Transformer( &DispTForm, &point, &tx, &ty );
if (ty < lpImage->lpDisplay->DispRect.top)
    TMove(&DispTForm, 0, lpImage->lpDisplay->DispRect.top - ty + 5);
/* Draw the mask */
PasteTForm = DispTForm;
Disp2FileTForm(&PasteTForm);
SetPasteMask(lpPasteShape, &PasteTForm );

ShapeTransformInit(lpPasteShape, &PasteTForm);
mask_rect(&rMask);
File2DispRect(&rMask, &rMask);
PaintImage(Window.hDC, &rMask, NULL);

Window.ModalProc = PasteProc;
PasteActive = TRUE;
return(TRUE);
}


/************************************************************************/
VOID PasteRefresh()
/************************************************************************/
{
RECT maskRect;
    
if (!lpImage)
    return;
if (!PasteActive)
	return;

if (mask_rect(&maskRect))
    {
    ShapeTransformDraw(0, NULL, OFF, -1);
    lpImage->lpDisplay->FloatingImageProc = NULL;
    lpImage->lpDisplay->FloatingGraphicsProc = NULL;
    File2DispRect(&maskRect, &maskRect);
    PaintImage(Window.hDC, &maskRect, NULL);
    lpImage->lpDisplay->FloatingImageProc = PasteFloatProc;
    lpImage->lpDisplay->FloatingGraphicsProc = ShapeFloatProc;
    ShapeGetTForm(&PasteTForm);
    SetPasteMask(lpPasteShape, &PasteTForm );
    mask_rect(&maskRect);
    File2DispRect(&maskRect, &maskRect);
    PaintImage(Window.hDC, &maskRect, NULL);
    }
}

/************************************************************************/
VOID PasteRedisplay()
/************************************************************************/
{
RECT maskRect;
    
if (!lpImage)
    return;
if (!PasteActive)
	return;

if (mask_rect(&maskRect))
    {
    File2DispRect(&maskRect, &maskRect);
    InvalidateRect(lpImage->hWnd, &maskRect, TRUE);
    }
}


/************************************************************************/
VOID DonePaste(fCancel)
/************************************************************************/
BOOL fCancel;
{
RECT rect, maskRect;

Window.ModalProc = NULL;
PasteActive = FALSE;
SetClassWord(lpImage->hWnd, GCW_HCURSOR, Window.hCursor);
lpImage->lpDisplay->FloatingImageProc = NULL;
lpImage->lpDisplay->FloatingGraphicsProc = NULL;
if (mask_rect(&maskRect))
    {
    ShapeGetTForm(&PasteTForm);
    ShapeTransformDone();
    if (fCancel)
	RemoveMask();
    else
	{
    	AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &maskRect);
        ApplyPaste(&PasteTForm);
        lpImage->dirty = IDS_UNDOPASTE;
        BuildMarquee();
	UpdateImage(&maskRect, YES);
	if (Mask.PasteBlend)
	    {
	    DoBlend ();
	    lpImage->dirty = IDS_UNDOPASTE;
	    }
        }
    }
if (lpClip)
    {
    frame_close(lpClip);
    lpClip = NULL;
    }
}
  
/************************************************************************/
void SetPasteMask( lpHeadShape, lpTForm )
/************************************************************************/
LPSHAPE lpHeadShape;
LPTFORM	lpTForm;
{
LPMASK	lpMask;
LPFRAME lpFrame;
RCT maskRect;
LPSHAPE lpShape;

lpFrame = frame_set(NULL);
if (!lpFrame)
    return;

Mask.IsOutside = FALSE;
lpShape = lpHeadShape;
while (lpShape)
    {
    lpShape->tform = *lpTForm;
    lpShape = lpShape->lpNext;
    }
ShapeNewShapes(lpHeadShape);

if (lpFrame->WriteMask)
    {
    lpMask = BuildEdges(lpFrame->WriteMask, NULL);
    }
else
    {
    lpMask = BuildEdges(lpFrame->WriteMask, lpHeadShape);
    if ( lpMask )
	    mask_link( lpMask, WRITE_MASK );
    else
        MessageBeep(0);
    }
    
lpShape = lpHeadShape;
while (lpShape)
    {
    TInit(&lpShape->tform);
    lpShape = lpShape->lpNext;
    }
ShapeNewShapes(lpHeadShape);
}

/************************************************************************/
BOOL ApplyPaste(lpTForm)
/************************************************************************/
LPTFORM lpTForm;
{
int	x1, x2, y, y1, y2, dx;
LPTR	lpLine, lpBuffer, lpMask;
RECT	rMask;

if ( !lpClip )
	return( NO);

/* Get the mask rectangle of the pasted object */
if ( !mask_rect( &rMask ) )
	return( NO );

PasteTForm = *lpTForm;

y1 = bound( rMask.top, 0, lpImage->nlin-1 );
y2 = bound( rMask.bottom, 0, lpImage->nlin-1 );
x1 = bound( rMask.left, 0, lpImage->npix-1 );
x2 = bound( rMask.right, 0, lpImage->npix-1 );
dx = x2 - x1 + 1;
if (!AllocLines(&lpBuffer, 1, dx, 0))
	return(NO);
if (!(lpMask = Alloc((long)dx)))
	{
	FreeUp(lpBuffer);
	return(NO);
	}
dx *= DEPTH;
for (y = y1; y <= y2; ++y)
	{
	AstralClockCursor(y-y1, y2-y1+1);
	if (lpLine = CachePtr(0, x1, y, YES))
		PasteFloatProc(y, x1, x2, lpLine, lpBuffer, lpMask);
	}
FreeUp(lpBuffer);
FreeUp(lpMask);
AstralCursor(NULL);
frame_close( lpClip );
lpClip = NULL;
return( YES );
}

/************************************************************************/
int PasteFloatProc(yline, xleft, xright, lpDst, lpSrc, lpMask)
/************************************************************************/
int yline, xleft, xright;
LPTR lpDst, lpSrc, lpMask;
{
int	dx, offmask, nx, ny, right, bottom, lastline, x;
LPTR	lp1, lpCLine, lpDst1, lpSrc1;
RECT	rMask;
LPFRAME	lpFrame;
POINT Point;
FIXED fnx, fny, fnegone, fright, fbottom;
int mode, depth, width, cx, cy;
long value, color;
TFORM tform;

if ( !lpClip )
	return( NO );
if ( !mask_rect( &rMask ) )
	return( NO );
if (!(lpFrame = frame_set(NULL)))
    	return(NO);
if (!lpMask)
	return(NO);
depth = lpFrame->Depth;
dx = xright-xleft+1;
if (yline < rMask.top || yline > rMask.bottom)
	return(NO);
offmask = min(rMask.left - xleft, dx);
if (offmask > 0)
	{
	xleft += offmask;
	if (xleft > xright)
		return(NO);
	offmask *= depth;
	copy(lpDst, lpSrc, offmask);
	lpDst += offmask;
	lpSrc += offmask;
	}
dx = xright-xleft+1;
offmask = min(xright - rMask.right, dx);
if (offmask > 0)
	{
	xright -= offmask;
	if (xright < xleft)
		return(NO);
	offmask *= depth;
	width = (xright-xleft+1)*depth;
	lpDst1 = lpDst + width;
	lpSrc1 = lpSrc + width;
	copy(lpDst1, lpSrc1, offmask);
	}
dx = xright-xleft+1;
width = dx*depth;

tform = PasteTForm;
TInvert( &tform );
mode = 0;
if (!Control.SmartPaste && tform.bx == 0 && tform.ay == 0)
    {
    if (tform.ax == FONE && tform.by == FONE)
	{
	cx = FMUL(1, tform.cx);
	cy = FMUL(1, tform.cy);
        mode = 2;
	}
    else
        {
        mode = 1;
        }
    }
//sprintf(msg, "ax = %lf by = %lf cx = %ld cy = %ld",
//	(double)tform.ax/65536., (double)tform.by/65536.,
//	tform.cx, tform.cy);
//dbg(msg);

right = lpClip->Xsize - 1;
bottom = lpClip->Ysize - 1;
if (Control.SmartPaste)
	{
	fright = FGET(right + 1, 1);
	fbottom = FGET(bottom + 1, 1);
	fnegone = FGET(-1, 1);
	}
lastline = -30000;
frame_set(lpClip);
if (mode == 2)  /* translating transform */
	{
	/* get a line from the clipboard frame */
	ny = yline+cy;
	if (ny < 0 || ny > bottom)
		{
		copy(lpDst, lpSrc, width);
		frame_set(lpFrame);
		return(YES);
		}

	lpCLine = frame_ptr(0, 0, ny, NO);
	if (!lpCLine)
		{
		copy(lpDst, lpSrc, width);
		frame_set(lpFrame);
		return(YES);
		}
	lpSrc1 = lpSrc;
	lpDst1 = lpDst;
	for ( x=xleft; x<=xright; x++, lpDst1 += depth, lpSrc1 += depth)
		{
	    	nx = x + cx;
	   	if (nx < 0 || nx > right)
			{
			frame_copypixel(lpDst1, lpSrc1, depth);
			}
		else
			{
	    		lp1 = lpCLine + (nx*depth);
			frame_copypixel(lp1, lpSrc1, depth);
			}
		}
	}
else if (mode == 1)  /* scaling transform */
	{
	/* get a line from the clipboard frame */
	fny = ((long)yline * tform.by) + tform.cy;
	value = fny + HALF;
	ny = HIWORD(value);
	//sprintf(msg, "fny = %lf value = %lf ny = %d",
	//	(double)fny/65536., (double)value/65536., ny);
	//dbg(msg);
	if (ny < 0 || ny > bottom)
		{
		copy(lpDst, lpSrc, width);
		frame_set(lpFrame);
		return(YES);
		}
	lpCLine = frame_ptr(0, 0, ny, NO);
	if (!lpCLine)
		{
		copy(lpDst, lpSrc, width);
		frame_set(lpFrame);
		return(YES);
		}
	lpSrc1 = lpSrc;
	lpDst1 = lpDst;
	for ( x=xleft; x<=xright; x++, lpDst1 += depth, lpSrc1 += depth )
		{
	    	fnx = ((long)x * tform.ax) + tform.cx;
		value = fnx + HALF;
		nx = HIWORD(value);
		//sprintf(msg, "fnx = %lf value = %lf nx = %d",
		//	(double)fnx/65536., (double)value/65536., nx);
		//dbg(msg);
	   	if (nx < 0 || nx > right)
			{
			frame_copypixel(lpDst1, lpSrc1, depth);
			}
		else
			{
	    		lp1 = lpCLine + (nx*depth);
			frame_copypixel(lp1, lpSrc1, depth);
			}
		}
	}
else    /* "complicated" transform */
	{
	lpSrc1 = lpSrc;
	lpDst1 = lpDst;
	for ( x=xleft; x<=xright; x++, lpDst1 += depth, lpSrc1 += depth )
		{
		Point.x = x;
		Point.y = yline;
		if (Control.SmartPaste)
			{
		    	FTransformer(&tform, &Point, &fnx, &fny);
		    	if (fnx <= fnegone || fny <= fnegone || fnx >= fright || fny >= fbottom)
				{
				frame_copypixel(lpDst1, lpSrc1, depth);
				continue;
				}
		        color = average_pixel(fnx, fny, 0, 0, bottom, right, FALSE);
			frame_putpixel(lpSrc1, color);
			}
		else
			{
	    		Transformer( &tform, &Point, &nx, &ny );
	   		if (nx < 0 || nx > right || ny < 0 || ny > bottom)
				{
				frame_copypixel(lpDst1, lpSrc1, depth);
				continue;
				}
			if (!(lp1 = frame_ptr(0, nx, ny, NO)))
				{
				frame_copypixel(lpDst1, lpSrc1, depth);
				continue;
				}
			frame_copypixel(lp1, lpSrc1, depth);
			}
		}
	}
frame_set(lpFrame);
mload(xleft, yline, dx, 1, lpMask, lpSrc, lpFrame);
setifset(lpMask, dx, Mask.Opacity);
ProcessData(lpDst, lpSrc, lpMask, dx, depth);
return( YES );
}

