//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <stdio.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "slider.h"

static int  iTool, iCategory, iTotalCategories;
static HINSTANCE hOptions;
static LPFRAME lpSplatFrame;
static BOOL bSwitchOn;
static int  iSlideSet;

int _cdecl rand(void);  // hate those compiler warnings

extern HINSTANCE hInstAstral;

/***********************************************************************/
BOOL WINPROC EXPORT DlgEffectsProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
STRING szOptions;
int swSlider, swSwitch;

switch (msg)
{
    case WM_INITDIALOG:
	SetControlFont (hDlg, IDC_EFFECTS_CATEGORY);
	RibbonInit( hDlg );
	iTotalCategories = GetExtNameCount( IDN_EFFECTS );
	hOptions = OptionLoadLibrary( hDlg, IDC_EFFECTS_CATEGORY, 17/*iCount*/,
		IDN_EFFECTS, iCategory, iTotalCategories );
	OptionGetString( hOptions, 0, szOptions );
	SetWindowText( GetDlgItem( hDlg, IDC_EFFECTS_CATEGORY ), szOptions );
	CheckRadioButton( hDlg, IDC_EFFECTS_T1, IDC_EFFECTS_T16,
		iTool+IDC_EFFECTS_T1 );
	CheckDlgButton( hDlg, IDC_EFFECTS_SW1, bSwitchOn );
#ifdef STUDIO
	iSlideSet = 1;
	InitSlide( hDlg, IDC_EFFECTS_AMOUNT, iSlideSet, 1, 10 );
	SendDlgItemMessage (hDlg, IDC_EFFECTS_AMOUNT,
	  SM_SETTICKS, 0, MAKELONG(0, 1));
#else
	iSlideSet = 1;	// set to a single
#endif
    ShowWindow (GetDlgItem(hDlg, IDC_EFFECTS_SL), SW_SHOW);
    ShowWindow (GetDlgItem(hDlg, IDC_EFFECTS_AMOUNT), SW_SHOW);
    ShowWindow (GetDlgItem(hDlg, IDC_EFFECTS_SW1), SW_HIDE);
	// Fall thru...

	case WM_CONTROLENABLE:
	case WM_DOCACTIVATED:
	if ( lpSplatFrame )
		FrameClose( lpSplatFrame );
	lpSplatFrame = NULL;
	if ( iCategory != 1 )
		lpSplatFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_EFFECTS ) );

    case WM_ERASEBKGND:
    break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaint( hDlg );
	break;

    case WM_DESTROY:
	if ( lpSplatFrame )
		FrameClose( lpSplatFrame );
	lpSplatFrame = NULL;
	if ( hOptions != hInstAstral )
		FreeLibrary( hOptions );
	return( FALSE );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_MEASUREITEM:
    case WM_DRAWITEM:
	return( OwnerDraw( hDlg, msg, lParam, NO ) );

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_EFFECTS_CATEGORY:
		if ( hOptions != hInstAstral )
			FreeLibrary( hOptions );
		if ( iTotalCategories )
			iCategory = (iCategory+1) % iTotalCategories;
		else
			iCategory = 0;
		hOptions = OptionLoadLibrary( hDlg, IDC_EFFECTS_CATEGORY, 17/*iCount*/,
			IDN_EFFECTS, iCategory, iTotalCategories );
		OptionGetString( hOptions, 0, szOptions );
		SetWindowText( GetDlgItem( hDlg, IDC_EFFECTS_CATEGORY ), szOptions );
		if ( lpSplatFrame )
			FrameClose( lpSplatFrame );
		lpSplatFrame = NULL;
		if ( iCategory != 1 )
			lpSplatFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
		break;

	    case IDC_EFFECTS_T1:
	    case IDC_EFFECTS_T2:
	    case IDC_EFFECTS_T3:
	    case IDC_EFFECTS_T4:
	    case IDC_EFFECTS_T5:
	    case IDC_EFFECTS_T6:
	    case IDC_EFFECTS_T7:
	    case IDC_EFFECTS_T8:
	    case IDC_EFFECTS_T9:
	    case IDC_EFFECTS_T10:
	    case IDC_EFFECTS_T11:
	    case IDC_EFFECTS_T12:
	    case IDC_EFFECTS_T13:
	    case IDC_EFFECTS_T14:
	    case IDC_EFFECTS_T15:
	    case IDC_EFFECTS_T16:
		CheckRadioButton( hDlg, IDC_EFFECTS_T1, IDC_EFFECTS_T16, wParam );
		iTool = wParam - IDC_EFFECTS_T1;
		if ( lpSplatFrame )
			FrameClose( lpSplatFrame );
		lpSplatFrame = NULL;
		if ( iCategory != 1 )
			lpSplatFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
#ifdef STUDIO
		if ( iCategory != 1 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T1 )
			{ swSwitch = SW_SHOW; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T2 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T3 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T4 )
			{ swSwitch = SW_SHOW; swSlider = SW_HIDE; } else
		if ( wParam == IDC_EFFECTS_T5 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T6 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T7 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T8 )
			{ swSwitch = SW_HIDE; swSlider = SW_HIDE; } else
		if ( wParam == IDC_EFFECTS_T9 )
			{ swSwitch = SW_HIDE; swSlider = SW_HIDE; } else
		if ( wParam == IDC_EFFECTS_T10 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T11 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T12 )
			{ swSwitch = SW_SHOW; swSlider = SW_HIDE; } else
		if ( wParam == IDC_EFFECTS_T13 )
			{ swSwitch = SW_HIDE; swSlider = SW_SHOW; } else
		if ( wParam == IDC_EFFECTS_T14 )
			{ swSwitch = SW_SHOW; swSlider = SW_HIDE; } else
		if ( wParam == IDC_EFFECTS_T15 )
			{ swSwitch = SW_HIDE; swSlider = SW_HIDE; } else
		if ( wParam == IDC_EFFECTS_T16 )
			{ swSwitch = SW_SHOW; swSlider = SW_SHOW; }
	    ShowWindow (GetDlgItem(hDlg, IDC_EFFECTS_SL), swSlider );
		ShowWindow (GetDlgItem(hDlg, IDC_EFFECTS_AMOUNT), swSlider );
		ShowWindow (GetDlgItem(hDlg, IDC_EFFECTS_SW1), swSwitch );
		UpdateControls (hDlg);
#endif
		  break;

#ifdef STUDIO
	    case IDC_EFFECTS_AMOUNT:
		  iSlideSet = HandleSlide( hDlg, (ITEMID)wParam, (UINT)lParam, NULL );
		  break;

	    case IDC_EFFECTS_SW1:
		  bSwitchOn = !bSwitchOn;
		  CheckDlgButton( hDlg, wParam, bSwitchOn );
		  break;
#endif
	    default:
		  return( FALSE );
	}
    default:
	  return( FALSE );
}

return( TRUE );
}


/************************************************************************/
static void UpdateControls (HWND hDlg)
/************************************************************************/
{
	RECT InvRect;
	HWND hParent, hControl;

	hParent = GetParent (hDlg);

	// invalidate a rect corresponding to the switch for the parent
	hControl = GetDlgItem (hDlg, IDC_EFFECTS_SW1);
	GetClientRect (hControl, &InvRect);
	MapWindowPoints (hControl, hParent, (LPPOINT)&InvRect, 2);
	InvalidateRect (hParent, &InvRect, TRUE);

	// invalidate a rect corresponding to the slider for the parent
	hControl = GetDlgItem (hDlg, IDC_EFFECTS_SL);
	GetClientRect (hControl, &InvRect);
	MapWindowPoints (hControl, hParent, (LPPOINT)&InvRect, 2);
	InvalidateRect (hParent, &InvRect, TRUE);

	UpdateWindow (hParent);
}


/************************************************************************/
int EffectsProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;
RECT rChange;
FNAME szFileName;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Display2File(&x, &y);
	if ( !ImgSelectObjectType( lpImage, x, y,
		YES/*bAllowBase*/,
		(iCategory != 1 ? NO : YES)/*bAllowMovable*/, NO/*bAllowPlayable*/ ) )
			break;
	SoundStartResource( "magic01", NO/*bLoop*/, NULL/*hInstance*/ );
	if ( iCategory != 1 )
		{ // a splat effect
		if ( !lpSplatFrame )
			lpSplatFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
		if ( !lpSplatFrame )
			break;
		RandomPaint( lpImage->hWnd, lpSplatFrame, iSlideSet/*1-10*/, x, y );
		break;
		}
	wsprintf( szFileName, "%lseffect%02d.dfx", (LPTR)Control.PouchPath, iTool+1 );
	if ( !DoEffects( lpImage, szFileName, &rChange, bSwitchOn, iSlideSet*10 ) )
		break;
	UpdateImage( &rChange, YES );
	break;

	case WM_SETCURSOR:
	return( SetToolCursor( hWindow,
		(iCategory != 1 ? ID_NO_ACTION : NULL), ID_PLAY_OBJECT ) );

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
return(TRUE);
}

static int		iLastY, dstY, dstX;
static int		iBltFrameDepth;
static int		iSrcWidth;
static LPOBJECT lpObject;
/************************************************************************/
/* RandomPaint                                                          */
/*   Parameters -                                                       */
/*     HWND                                                             */
/*     TotalPicts - Total number of pictures to randomly draw           */
/************************************************************************/
void RandomPaint (HWND hWnd, LPFRAME lpSrcFrame, int TotalPicts, int x, int y )
{
LPFRAME  lpDstFrame;
LPTR     lpSrcData, lpDstData, lpMask;
int      dxSrc, dySrc, iSrcDepth, iDstDepth;
int      bufsize, iNumPicts;
RECT     rDest, rSource, rOutput;
long	 lSize;
BOOL	 bUseXY;
PIXMAP   SrcPixmap;
LPPROCESSPROC lpProcessProc;	// Proc to process image data

if ( !lpSrcFrame )
	return;

if ( TotalPicts <= 0 )
  return;

bUseXY = (TotalPicts <= 1);

if ( !(lpObject = ImgGetBase(lpImage)) )
	return;

AstralCursor( IDC_WAIT );
ImgPaintSetup (lpImage, NO);

// get the number of pictures in the source file
dxSrc = FrameXSize(lpSrcFrame);
dySrc = FrameYSize(lpSrcFrame);
iNumPicts = dySrc / dxSrc;

// compute destination info.
lpDstFrame = ImgGetBaseEditFrame (lpImage);
iDstDepth = FrameDepth (lpDstFrame);
if (iDstDepth == 0) iDstDepth = 1;

// compute source info.
iSrcDepth = lpSrcFrame->Depth /* 3 */;
iBltFrameDepth = iSrcDepth;
if (iSrcDepth == 0) iSrcDepth = 1;
rSource.top = rSource.left = 0;
rSource.right = dxSrc - 1;
rSource.bottom = dySrc - 1;
iSrcWidth = dxSrc;
PixmapSetup (&SrcPixmap, lpSrcFrame, YES);

rDest.top = rDest.left = 0;
rDest.right = rDest.bottom = dxSrc - 1;

// get pointer to the merge process proc
lpProcessProc = GetProcessProc (MM_NORMAL, iBltFrameDepth);

// allocate memory for data buffers
lpSrcData = lpDstData = lpMask = NULL;
bufsize = dxSrc * dxSrc;
lSize = iBltFrameDepth * bufsize;
if (!(lpSrcData = Alloc( lSize )))
  goto Error;
if (!(lpDstData = Alloc( lSize )))
  goto Error;
if (!(lpMask = Alloc( (long)bufsize)))
  goto Error;

if (! ImgEditInit (lpImage, ET_OBJECT, UT_DATA, lpObject) )
  goto Error;

iLastY = rand() % iNumPicts;    // random starting point
while (TotalPicts--)
{
  if ( bUseXY )
	{ dstX = x - (dxSrc / 2); dstY = y - (dxSrc / 2); }
  else
	{ dstX = rand() % (480-dxSrc); dstY = rand() % (360-dxSrc); }
  OffsetRect (&rDest, dstX, dstY);
  if (! LoadSourceData (&lpObject->Pixmap, PMT_EDIT, dstX, dstY,
    dxSrc, dxSrc, lpDstData, &rDest, NO, NO))
	    continue;

  if (! LoadMirroredData (&SrcPixmap, PMT_EDIT, 0, GetSourceY(iNumPicts),
    dxSrc, dxSrc, lpSrcData, &rSource, NO, NO))
	    continue;

  LoadDropoutMask (lpSrcData, dxSrc, dxSrc, lpMask);
  (*lpProcessProc)(lpDstData, lpSrcData, lpMask, bufsize);
  rOutput = rDest;
  BlitToFrame (dstX, dstY, dxSrc, dxSrc, lpDstData, &rOutput);
  // so that each bitmap shows on image when output
  BlitToDisplay (&rDest);
}

ImgEditedObject (lpImage, lpObject, IDS_UNDOCHANGE, NULL);

goto Exit;

Error:
Message (IDS_EMEMALLOC);

Exit:
// free memory
if ( lpSrcData )
	FreeUp (lpSrcData);
if ( lpDstData )
	FreeUp (lpDstData);
if ( lpMask )
	FreeUp (lpMask);
AstralCursor( NULL );
}

/************************************************************************/
static void BlitToFrame( int x, int y, int dh, int dv, LPTR lpData,
		LPRECT lpRect)
/************************************************************************/
{
int sy;
WORD bwidth, width;
LPTR lp;
RECT StrokeRect;

StrokeRect = *lpRect;

// get width of buffer in bytes
bwidth = dh * iBltFrameDepth;

// advance in the number of lines and pixels
lpData += ((WORD)StrokeRect.top * bwidth) +
		  ((WORD)StrokeRect.left * iBltFrameDepth);

// get width of area to be blt'ed
width = (StrokeRect.right - StrokeRect.left + 1) * iBltFrameDepth;

// offset stroked area from buffer to frame
StrokeRect.left += x;
StrokeRect.right += x;
StrokeRect.top += y;
StrokeRect.bottom += y;

for (sy = StrokeRect.top; sy <= StrokeRect.bottom; ++sy)
{
  if ((lp = PixmapPtr (&lpObject->Pixmap, PMT_EDIT, StrokeRect.left, sy, YES)))
    copy (lpData, lp, width);
  lpData += bwidth;
}

PixmapAddUndoArea (&lpObject->Pixmap, &StrokeRect);
}


/************************************************************************/
static int GetSourceY( int iNumPicts )
/************************************************************************/
{
  if (iLastY + 1 > iNumPicts)
 	   iLastY = 0;
  else
 	   iLastY++;
  return( iLastY * iSrcWidth );
}

/************************************************************************/
static void LoadDropoutMask( LPTR lpSrc, int dx, int dy, LPTR lpMask )
/************************************************************************/
{
#define BRMASK(lpM,Size,x,y) lpM[ (Size * (y)) + (x) ]
  int iSize, r, g, b, i, x, y, rTarget, gTarget, bTarget;

  iSize = max (dx, dy);

  rTarget = *lpSrc++;
  gTarget = *lpSrc++;
  bTarget = *lpSrc++;
  lpSrc -= 3;
  for (y = 0; y < dy; ++y)
  {
    for (x = 0; x < dx; ++x)
    {
      r = *lpSrc++;
      g = *lpSrc++;
      b = *lpSrc++;
      if ( r == rTarget && g == gTarget && b == bTarget )
      {
	    i = 0; // white is the dropout
      }
      else
      {
        i = 255;
      }
      BRMASK( lpMask, iSize, x, y ) = i;
    }
  }
}

/************************************************************************/
static void BlitToDisplay(LPRECT lpRect)
/************************************************************************/
{
  RECT StrokeRect, DispRect, rUpdate, rInvalid;

  StrokeRect = *lpRect;
  OffsetRect (&StrokeRect, dstX, dstY);
  AstralUnionRect (&rUpdate, &rUpdate, &StrokeRect);
  File2DispRect (&StrokeRect, &DispRect);
  ImgPaintRaster (lpImage, Window.hDC, &DispRect, &rInvalid);
}

