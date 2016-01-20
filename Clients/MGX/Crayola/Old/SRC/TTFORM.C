//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static BOOL TransformRefresh( LPOBJECT lpObject, int iCode );
static BOOL TransformObjectsInit( LPOBJECT lpObject, BOOL bInit );
static BOOL TransformObjectsExecute( LPOBJECT lpObject, BOOL fMoveOnly );
static BOOL TransformObject( LPOBJECT lpObject );
static BOOL IntersectCorners( int y, LPLFIXED lpX1, LPLFIXED lpX2,
	LPLFIXED lpFx1, LPLFIXED lpFy1 , LPLFIXED lpFx2, LPLFIXED lpFy2 );
static BOOL IntersectSegment( int y, POINT pa, POINT pb, LPLFIXED lpX, LPLFIXED lpPercent );

#define TF_START	1
#define TF_DURING	2
#define TF_END		3
#define TF_CANCEL	4

// Static data
static DISTORT Distort;
static BOOL fHiQuality;
static int iTransformNewObject;

static int iTool, iToolSave;
static LPOBJECT lpObjectInProcess;

extern HWND hWndAstral;

/************************************************************************/
void TransformObjectsStart(BOOL fNewObject)
/************************************************************************/
{
iTransformNewObject = fNewObject;
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgTransformerProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
BOOL Bool;

switch (msg)
	{
	case WM_INITDIALOG:
	SetControlFont (hDlg, IDC_TRANSFORMER_T11);
	SetControlFont (hDlg, IDC_TRANSFORMER_T12);
	SetControlFont (hDlg, IDC_TRANSFORMER_T13);
	RibbonInit( hDlg );
	iTool = 0;
	CheckRadioButton( hDlg, IDC_TRANSFORMER_T1, IDC_TRANSFORMER_T10,
		iTool+IDC_TRANSFORMER_T1 );
	TransformObjectsStart( NO/*fNewObject*/ );
	// fall through...

	case WM_CONTROLENABLE:
	case WM_DOCACTIVATED:
	Bool = lpImage != NULL;
	ControlEnable(hDlg, IDC_TRANSFUNCTION, Bool);
	break;

	case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_TRANSFORMER ) );

	case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

	case WM_PAINT:
	LayoutPaint( hDlg );
	break;

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
		case IDC_TRANSFORMER_T1:
		case IDC_TRANSFORMER_T2:
		case IDC_TRANSFORMER_T3:
		case IDC_TRANSFORMER_T4:
		case IDC_TRANSFORMER_T5:
		case IDC_TRANSFORMER_T6:
		case IDC_TRANSFORMER_T7:
		case IDC_TRANSFORMER_T8:
		case IDC_TRANSFORMER_T9:
		case IDC_TRANSFORMER_T10:
			CheckRadioButton( hDlg, IDC_TRANSFORMER_T1, IDC_TRANSFORMER_T10, wParam );
			iTool = wParam - IDC_TRANSFORMER_T1;
			SetWindowCursor( ID_NO_ACTION );
			break;

		case IDC_TRANSFORMER_T11:
			if( !lpImage )
				break;

			// if transform in progress, end before combining
			if (lpObjectInProcess)
				EndTransform (lpObjectInProcess);

			ImgCombineObjects( lpImage, YES/*fNoUndo*/, NO/*fCreateMask*/,
					YES/*fForceIt*/ );
			SetWindowCursor( ID_NO_ACTION );
			break;

		case IDC_TRANSFORMER_T12:
			iToolSave = iTool;
			iTool = wParam - IDC_TRANSFORMER_T1;
			SetWindowCursor( ID_CUT );
			break;

		case IDC_TRANSFORMER_T13:
			SendMessage( hWndAstral, WM_COMMAND, IDM_PASTE, 0L );
			SetWindowCursor( ID_NO_ACTION );
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
int TransformerProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;
LPOBJECT lpObject;
WORD wKey;
//static LPOBJECT lpObjectInProcess;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
	{
	case WM_CREATE:	// The first mouse down message
		Tool.bActive = YES;
		break;

	case WM_LBUTTONDOWN:
		if (!ONIMAGE(x, y))
			break;
		Display2File(&x, &y);
		if ( iTool == 11 ) // copy
			{
			if ( !(lpObject = ImgSelectObjectType( lpImage, x, y,
				YES/*bAllowBase*/, YES/*bAllowMovable*/, YES/*bAllowPlayable*/ )) )
					break;
			SetWindowCursor( ID_NO_ACTION );
			SendMessage( hWndAstral, WM_COMMAND, IDM_COPY, 0L );
			iTool = iToolSave;
			Tool.bActive = NO;
			break;
			}
		if ( !(lpObject = ImgSelectObjectType( lpImage, x, y,
			NO/*bAllowBase*/, YES/*bAllowMovable*/, NO/*bAllowPlayable*/ )) )
				break;

		if ( lpObjectInProcess != lpObject )
			{
			if ( lpObjectInProcess )
				EndTransform( lpObjectInProcess );
			lpObjectInProcess = StartTransform( lpObject );
			}

		if ( lpObjectInProcess )
			DoTransform( lpObjectInProcess, iTool );

		Tool.bActive = (lpObjectInProcess != NULL);
		break;

	case WM_SETCURSOR:
	if ( iTool == 11 ) // copy to clipboard
		return( SetToolCursor( hWindow, ID_CUT, ID_CUT ) );
	else
		return( SetToolCursor( hWindow, ID_TRANSFORMER, ID_PLAY_OBJECT ) );

	case WM_ACTIVATE:
	if (!lParam) // a deactivate
		{
		if ( lpObjectInProcess )
			EndTransform( lpObjectInProcess );
		lpObjectInProcess = NULL;
		Tool.bActive = NO;
		}
	break;

	case WM_LBUTTONUP:
		break;

	case WM_MOUSEMOVE:	// sent when ToolActive is on
		break;

	case WM_KEYDOWN:
	if ( !lpImage )
		break;
	wKey = lParam;
	switch (wKey)
		{
		case VK_DELETE:
			DeactivateTool();
			ImgDeleteSelObjects( lpImage );
			break;
		default:
			break;
		}
	break;

	case WM_KEYUP:
		break;

	case WM_LBUTTONDBLCLK:
		break;

	case WM_DESTROY:	// The cancel operation message
		if ( lpObjectInProcess )
			EndTransform( lpObjectInProcess );
		lpObjectInProcess = NULL;
		Tool.bActive = NO;
		break;
	}
return(TRUE);
}


/***********************************************************************/
static LPOBJECT StartTransform( LPOBJECT lpObject )
/***********************************************************************/
{
if ( !lpObject )
	return( NULL );
if ( lpObject == ImgGetBase(lpImage) )
	return( NULL );

AstralCursor( IDC_WAIT );
ShieldsSave();
Shields( OFF );
TrackInit( &lpObject->rObject );
TransformRefresh( lpObject, TF_START );
AstralCursor( NULL );
return( lpObject );
}


/***********************************************************************/
static void EndTransform( LPOBJECT lpObject )
/***********************************************************************/
{
if ( !lpObject )
	return;

AstralCursor( IDC_WAIT );
TransformObjectsExecute( lpObject, NO/*fMoveOnly*/ );
TransformRefresh( lpObject, TF_END );
ShieldsRestore();
AstralCursor( NULL );
}

/***********************************************************************/
static void DoTransform( LPOBJECT lpObject, int iTool )
/***********************************************************************/
{
RECT rect;
TFORM tform;
LFIXED fx, fy;

if ( !lpObject )
	return;

AstralCursor( IDC_WAIT );
TrackGetTForm( &tform );
TrackGetRect( &rect );
switch ( iTool + 1 )
	{
	case 1:         // flip left 2 right
		fx = FGET( rect.left + rect.right, 2 );
		fy = FGET( rect.top + rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, -UNITY, UNITY );
		TFMove( &tform, fx, fy );
		break;

	case 2:         // big
		fx = FGET( rect.left + rect.right, 2 );
		fy = FGET( rect.top + rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, FGET( 10, 9 ), FGET( 10, 9 ));
		TFMove( &tform, fx, fy );
		break;

	case 3:         // fat
		fx = FGET( rect.left + rect.right, 2 );
		fy = FGET( rect.top + rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, FGET( 10, 9 ), UNITY );
		TFMove( &tform, fx, fy );
		break;

	case 4:         // tall
		fx = FGET( rect.left + rect.right, 2 );
		fy = FGET( rect.top + rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, UNITY, FGET( 10, 9 ));
		TFMove( &tform, fx, fy );
		break;

	case 5:         // rotate clockwise
		fx = FGET( rect.left + rect.right, 2 );
		fy = FGET( rect.top + rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TRotate( &tform, 15, 15 );
		TFMove( &tform, fx, fy );
		break;

	case 6:        // flip top 2 bottom
		fx = FGET( rect.left +  rect.right, 2 );
		fy = FGET( rect.top +  rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, UNITY, -UNITY );
		TFMove( &tform, fx, fy );    
		break;

	case 7:        // small
		fx = FGET( rect.left +  rect.right, 2 );
		fy = FGET( rect.top +  rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, FGET( 9, 10 ), FGET( 9, 10 ));
		TFMove( &tform, fx, fy );
		break;

	case 8:        // thin
		fx = FGET( rect.left +  rect.right, 2 );
		fy = FGET( rect.top +  rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, FGET( 9, 10 ), UNITY );
		TFMove( &tform, fx, fy );
		break;

	case 9:        // short
		fx = FGET( rect.left +  rect.right, 2 );
		fy = FGET( rect.top +  rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TScale( &tform, UNITY, FGET( 9, 10 ));
		TFMove( &tform, fx, fy );
		break;

	case 10:        // rotate counter-clockwise
		fx = FGET( rect.left +  rect.right, 2 );
		fy = FGET( rect.top +  rect.bottom, 2 );
		TFMove( &tform, -fx, -fy );
		TRotate( &tform, -15, -15 );
		TFMove( &tform, fx, fy );
		break;
	}

TrackSetTForm( &tform, NO/*fDisplay*/ );
TransformRefresh( lpObject, TF_DURING );
AstralCursor( NULL );
}


/************************************************************************/
static BOOL TransformRefresh( LPOBJECT lpObject, int iCode )
/************************************************************************/
{
RECT rUpdate[2], rSect;
int nRects = 0;
static RECT rLastRefresh;
static BOOL bStarted;

#define H_SIZE 6
#define H_OFFSET (H_SIZE/2)

if ( !lpImage )
	return(FALSE);
if ( !bStarted && iCode != TF_START )
	return( FALSE );
if ( bStarted && iCode == TF_START )
	return( FALSE );

if ( iCode == TF_START )
	bStarted = YES;
if ( iCode == TF_END || iCode == TF_CANCEL )
	bStarted = NO;

AstralCursor(IDC_WAIT);
if ( iCode == TF_DURING || iCode == TF_CANCEL || iCode == TF_END )
	{ // Invalidate the LAST TRANSFORMED area
	rUpdate[nRects++] = rLastRefresh;
	}

if ( iCode == TF_START || iCode == TF_DURING )
	{
	// Get the updated tracking transform
	TrackGetRect( &rLastRefresh );

	// redisplay for color shield
	if (iCode == TF_START && ColorMask.Mask && ColorMask.On)
		rUpdate[nRects++] = rLastRefresh;

	// Update the transforms for all selected objects
	TransformObjectsInit( lpObject, YES );

	// make sure an invalidate happens first so rectangle won't draw twice
	if (iCode == TF_START)
		AstralUpdateWindow(lpImage->hWnd);
	}

if ( iCode == TF_DURING )
	{ // Invalidate the NEWLY TRANSFORMED area
	rUpdate[nRects++] = rLastRefresh;
	}

if ( iCode == TF_CANCEL )
	{ // Invalidate the ORIGINAL area
	rUpdate[nRects++] = lpObject->rObject;
	}

// The track stuff will get re-drawn by the paint engine
if (nRects)
	{
	UpdateImage(&rUpdate[0], YES);
	if (nRects > 1)
		{
		if (!AstralIntersectRect(&rSect, &rUpdate[0], &rUpdate[1]))
			AstralUpdateWindow(lpImage->hWnd);
		UpdateImage(&rUpdate[1], YES);
		}
	AstralUpdateWindow( lpImage->hWnd );
	}
AstralCursor(NULL);

return(TRUE);
}


/************************************************************************/
static BOOL TransformObjectsInit( LPOBJECT lpObject, BOOL bInit )
/************************************************************************/
{
if ( !lpObject )
	return( FALSE );

TrackGetForwardDistort( &Distort );

if ( !bInit )
	{ // Cleanup on an error condition
	if ( lpObject->fTransform )
		{
		lpObject->rObject = lpObject->rTransform;
		lpObject->fTransform = NO;
		lpObject->fPasteIntoMask = NO;
		}
	}
else
	{
	if ( !lpObject->fTransform )
		{ // If this is the first time...
		lpObject->rTransform = lpObject->rObject;
		lpObject->fTransform = YES;
		lpObject->fPasteIntoMask = NO;
		}
	TrackMapOriginalRect( &Distort, &lpObject->rTransform,
		&lpObject->rObject );
	}

return( YES );
}

/************************************************************************/
static BOOL TransformObjectsExecute( LPOBJECT lpObject, BOOL fMoveOnly )
/************************************************************************/
{
BOOL bOKtoProceed = YES;
BOOL fColorMask;

if ( !lpObject )
	return( FALSE );

ProgressBegin( 1, IDS_UNDOCHANGE-IDS_UNDOFIRST+IDS_PROGFIRST );

fColorMask = ColorMask.Mask && ColorMask.On;

if ( lpObject->fTransform )
	{
	lpObject->fTransform = NO;
	if (bOKtoProceed)
		{
		if ( !fMoveOnly || lpObject->fPasteIntoMask || fColorMask)
			{
			if ( !(bOKtoProceed = TransformObject( lpObject )) )
				MessageBeep(0); // "Error transforming an image object"
			}
		else // just a move
			{
			if (iTransformNewObject)
				{
				if ( !ImgEditInit( lpImage, ET_OBJECT,
					UT_MOVEOBJECTS|UT_OBJECTRECT, lpObject ))
						return( NULL );
				lpObject->fUndoDeleted = YES;
				ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDOCHANGE, NULL);
				}
			}
		}
	lpObject->fPasteIntoMask = NO;
	}

if ( !bOKtoProceed )
	ImgEditRecover( lpImage );
ProgressEnd();
return( bOKtoProceed );
}

/************************************************************************/
static BOOL TransformObject( LPOBJECT lpObject )
/************************************************************************/
{
int  x, y, dx, dy, iDataDepth, iAlphaDepth;
LPTR lpDataBuf, lpAlphaBuf;
LPFRAME lpFrame, lpDataFrame, lpAlphaFrame;
LPALPHA lpAlpha;
FRMDATATYPE fdt;
LPMASK	lpMask;
BOOL fOldQuality, fColorMask, fPasteInto;
int	mx, my;

if ( !lpObject )
	return( NO );

ProgressBegin(1, 0);

if ( !ImgEditInit( lpImage, ET_OBJECT,
	UT_NEWDATA|UT_NEWALPHA|UT_OBJECTRECT, lpObject ))
		return( NULL );

// How big are the newly transformed frames going to be
dx = RectWidth( &lpObject->rObject );
dy = RectHeight( &lpObject->rObject );

fColorMask = ColorMask.Mask && ColorMask.On;
if ((fPasteInto = lpObject->fPasteIntoMask) && (lpMask = ImgGetMask(lpImage)))
	{
	mx = lpObject->rObject.left;
	my = lpObject->rObject.top;
	}
else
	fPasteInto = NO;

// Create a new data frame
lpDataFrame = NULL;
if ( lpFrame = lpObject->Pixmap.EditFrame )
	{
	fdt = FrameType(lpFrame);

	if ( lpDataFrame = FrameOpen( fdt, dx, dy, FrameResolution(lpFrame) ) )
		{ // Allocate a data buffer to transform lines into
		iDataDepth = max( 1, FrameDepth(lpDataFrame) );
		}
	}

// Create a new alpha frame
lpAlphaFrame = NULL;
if ( lpAlpha = lpObject->lpAlpha )
	{
	if ( lpFrame = lpAlpha->Pixmap.EditFrame )
		{
		fdt = FrameType(lpFrame);
		if ( lpAlphaFrame = FrameOpen( fdt, dx, dy, FrameResolution(lpFrame) ) )
			{ // Allocate a data buffer to transform lines into
			iAlphaDepth = max( 1, FrameDepth(lpAlphaFrame) );
			}
		}
	}

// Install the user's desired quality setting instead of the default
fOldQuality = fHiQuality;
fHiQuality = Mask.PasteQuality;

// Create the data for the two new frames one line at a time
x = 0;

for (y = 0; y <= dy; ++y)
	{
	AstralClockCursor( y, dy, NO );
	if (lpAlphaFrame)
		lpAlphaBuf = FramePointer( lpAlphaFrame, x, y, TRUE );
	else
		lpAlphaBuf = NULL;

	if (lpDataFrame)
		lpDataBuf = FramePointer( lpDataFrame, x, y, TRUE );
	else
		lpDataBuf = NULL;

	if ( !TransformLine( lpObject, x, y, dx, lpDataBuf, iDataDepth,
		lpAlphaBuf, iAlphaDepth ) )
		continue;
	if (lpAlphaBuf)
		{
		if (fPasteInto)
			MaskLoader(lpMask, mx, my++, dx, lpAlphaBuf, NO, CR_MULTIPLY);
		if (fColorMask && lpDataBuf)
			Shield(lpAlphaBuf, lpDataBuf, (WORD)dx, iDataDepth);
		}
	}

if (iTransformNewObject)
	{
	FrameClose(lpObject->Pixmap.EditFrame);
	if (lpAlpha)
		FrameClose(lpAlpha->Pixmap.EditFrame);
	lpObject->Pixmap.EditFrame = lpDataFrame;
	lpAlpha->Pixmap.EditFrame = lpAlphaFrame;
	lpObject->fUndoDeleted = YES;
	ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDOCHANGE, NULL);
	}
else
	{
	ImgEditedObjectFrame(lpImage, lpObject, IDS_UNDOCHANGE, NULL,
		lpDataFrame, lpAlphaFrame);
	}

fHiQuality = fOldQuality; // Restore the hi quality setting
ProgressEnd();
return( YES );
}

/************************************************************************/
// This routines take NEW frame coordiantes x1, y, dx,
// and stuffs the passed data and alpha buffers
// given a transform BACK to the passed object's frame data
/************************************************************************/

/************************************************************************/
BOOL TransformLine( LPOBJECT lpObject, int x1, int y, int dx, LPTR lpDataBuf,
	int iDataDepth, LPTR lpAlphaBuf, int iAlphaDepth )
/************************************************************************/
{
int     i, x2, nx, ny, right, bottom, iCount;
LPTR    lpAlphaSrc, lpAlphaDst;
LPTR    lpDataSrc,  lpDataDst;
LPRGB   lpRGBSrc,   lpRGBDst;
LPCMYK  lpCMYKSrc,  lpCMYKDst;
LPFRAME lpDataFrame, lpAlphaFrame;
LPALPHA lpAlpha;
LFIXED  f, fnx, fny, lnx, lny, fMin, fMax, fxRate, fyRate;
LFIXED	fnegone, fright, fbottom;
BYTE    ClearByte;
RGBS    ClearRGB;
CMYKS   ClearCMYK;

if ( !lpObject || !dx )
	return( NO );

lpDataFrame = ( lpDataBuf ? lpObject->Pixmap.EditFrame : NULL );
if ( lpAlpha = lpObject->lpAlpha )
	lpAlphaFrame = ( lpAlphaBuf ? lpAlpha->Pixmap.EditFrame : NULL );
else
	lpAlphaFrame = NULL;

// If nothing to do
if ( !lpDataFrame && !lpAlphaFrame )
	return( NO );

// See if it's alpha only
if ( !lpDataFrame )
	return( TransformSingleLine( lpObject, x1, y, dx,
			lpAlphaFrame, lpAlphaBuf, iAlphaDepth ) );

// See if it's data only
if ( !lpAlphaFrame )
	return( TransformSingleLine( lpObject, x1, y, dx,
		lpDataFrame, lpDataBuf, iDataDepth ) );

// It's data and alpha both

// WARNING: THIS ROUTINE IS ALMOST A DUPLICATE OF THE ONE BELOW
// PLEASE MAINTAIN BOTH WHEN MAKING BUG FIXES AND UPDATES

// Make sure the line intersects the transformed object in some way
nx = RectWidth(&lpObject->rObject);
ny = RectHeight(&lpObject->rObject);

x2 = x1 + dx - 1;
if ( y < 0 || y >= ny || x2 < 0 || x1 >= nx )
	return( NO );

// Clear the output buffers since we may not write to each location
if ( iDataDepth == 4 )
		set( lpDataBuf, dx * iDataDepth, 255 );
else	clr( lpDataBuf, dx * iDataDepth );
if ( iAlphaDepth == 4 )
		set( lpAlphaBuf, dx * iAlphaDepth, 255 );
else	clr( lpAlphaBuf, dx * iAlphaDepth );

// If the x1 coordinate is too far left, adjust dx and the data pointers
if ( x1 < 0 )
	{
	lpDataBuf -= (x1 * iDataDepth);
	lpAlphaBuf -= (x1 * iAlphaDepth);
	dx += x1;
	}

// If the x2 coordinate is too far right, adjust dx
if ( x2 >= nx )
	{
	dx -= (x2 - nx + 1);
	}

// Map a y scan line (in the new output distorted space)...
// back to the original bounding rect
y += lpObject->rObject.top;

// The output span between fMin and fMax is the number of
// pixels to read between input points (fnx,fny) and (lnx,lny)
IntersectCorners( y, &fMin, &fMax, &fnx, &fny, &lnx, &lny );
f = lpObject->rTransform.top; f = TOFIXED( f );
fny -= f;
lny -= f;
f = lpObject->rTransform.left; f = TOFIXED( f );
fnx -= f;
lnx -= f;

// Compute the pixel count for the rate calculations
f = fMax - fMin;
iCount = ROUND((long)fMax) - ROUND((long)fMin);

// Compute the x and y rates to from the first input point to the last
if ( !iCount )
	{
	fxRate = UNITY;
	fyRate = -UNITY;
	}
else
	{
	fxRate = (lnx - fnx) / iCount;
	fyRate = (lny - fny) / iCount;
	}

iCount++; // Now its a loop count

// Compute the buffer indent
i = ROUND((long)fMin) - lpObject->rObject.left - x1;
if ( i >= dx )
	return( YES ); // Nothing to do

// Handle an off-image start
if ( i < 0 )
	{
	fnx -= (fxRate * i);
	fny -= (fyRate * i);
	iCount += i;
	i = 0;
	}
if ( iCount + i > dx )
	iCount = dx - i; // Don't go past the end of the buffers
if ( iCount <= 0 )
	return( YES ); // Nothing to do

// Indent the buffers
lpDataDst  = lpDataBuf + (i * iDataDepth);
lpAlphaDst = lpAlphaBuf + (i * iAlphaDepth);

// Make this a frame size check
right  = FrameXSize( lpDataFrame ) - 1;
bottom = FrameYSize( lpDataFrame ) - 1;

ClearByte = 0;
ClearRGB.red = ClearRGB.green = ClearRGB.blue  = 0;
ClearCMYK.c = ClearCMYK.m = ClearCMYK.y = ClearCMYK.k = 255;

if ( fHiQuality )
	{
	fright  = TOFIXED(right + 1);
	fbottom = TOFIXED(bottom + 1);
	fnegone = TOFIXED(-1);

	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					fnx >= fright  || fny >= fbottom )
					{
					*lpDataDst++  = ClearByte;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					average_pixel8( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpDataDst++ );
					average_pixel8( lpAlphaFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpAlphaDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 3 :
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					fnx >= fright  || fny >= fbottom )
					{
					*lpRGBDst++   = ClearRGB;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					average_pixel24( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearRGB, lpRGBDst++ );
					average_pixel8( lpAlphaFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpAlphaDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					fnx >= fright  || fny >= fbottom )
					{
					*lpCMYKDst++  = ClearCMYK;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					average_pixel32( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearCMYK, lpCMYKDst++ );
					average_pixel8( lpAlphaFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpAlphaDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}
else
	{
	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			{
			while ( --iCount >= 0 )
				{
				nx = ROUND((long)fnx);
				ny = ROUND((long)fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpDataSrc = FramePointer( lpDataFrame, nx, ny, NO )) ||
					!(lpAlphaSrc = FramePointer( lpAlphaFrame, nx, ny, NO )) )
					{
					*lpDataDst++  = ClearByte;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					*lpDataDst++  = *lpDataSrc;
					*lpAlphaDst++ = *lpAlphaSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;

		case 3 :
			{
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = ROUND((long)fnx);
				ny = ROUND((long)fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpRGBSrc = (LPRGB)FramePointer( lpDataFrame, nx, ny, NO )) ||
					!(lpAlphaSrc = FramePointer( lpAlphaFrame, nx, ny, NO )) )
					{
					*lpRGBDst++   = ClearRGB;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					*lpRGBDst++   = *lpRGBSrc;
					*lpAlphaDst++ = *lpAlphaSrc;
					}
				fnx += fxRate;
				fny += fyRate;
			}
		}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = ROUND((long)fnx);
				ny = ROUND((long)fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpCMYKSrc = (LPCMYK)FramePointer( lpDataFrame, nx, ny, NO )) ||
					!(lpAlphaSrc = FramePointer( lpAlphaFrame, nx, ny, NO )) )
					{
					*lpCMYKDst++  = ClearCMYK;
					*lpAlphaDst++ = ClearByte;
					}
				else
					{
					*lpCMYKDst++  = *lpCMYKSrc;
					*lpAlphaDst++ = *lpAlphaSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}

return( YES );
}

/************************************************************************/
// This routines take NEW frame coordiantes x1, y, dx,
// and stuffs the passed data OR alpha buffers
// given a transform BACK to the passed object's frame data
/************************************************************************/

/************************************************************************/
BOOL TransformSingleLine( LPOBJECT lpObject, int x1, int y, int dx,
	LPFRAME lpDataFrame, LPTR lpDataBuf, int iDataDepth )
/************************************************************************/
{
int    i, x2, nx, ny, right, bottom, iCount;
LPTR   lpDataSrc, lpDataDst;
LPRGB  lpRGBSrc,  lpRGBDst;
LPCMYK lpCMYKSrc, lpCMYKDst;
LFIXED f, fnx, fny, lnx, lny, fMin, fMax, fxRate, fyRate;
LFIXED fnegone, fright, fbottom;
BYTE   ClearByte;
RGBS   ClearRGB;
CMYKS  ClearCMYK;

// WARNING: THIS ROUTINE IS ALMOST A DUPLICATE OF THE ONE ABOVE
// PLEASE MAINTAIN BOTH WHEN MAKING BUG FIXES AND UPDATES

if ( !lpObject || !lpDataFrame || !lpDataBuf || !dx )
	return( NO );

// Make sure the line intersects the transformed object in some way
nx = RectWidth(&lpObject->rObject);
ny = RectHeight(&lpObject->rObject);

x2 = x1 + dx - 1;
if ( y < 0 || y >= ny || x2 < 0 || x1 >= nx )
	return( NO );

// Clear the output buffers since we may not write to each location
if ( iDataDepth == 4 )
		set( lpDataBuf, dx * iDataDepth, 255 );
else	clr( lpDataBuf, dx * iDataDepth );

// If the x1 coordinate is too far left, adjust dx and the data pointers
if ( x1 < 0 )
	{
	lpDataBuf -= (x1 * iDataDepth);
	dx += x1;
	}

// If the x2 coordinate is too far right, adjust dx
if ( x2 >= nx )
	{
	dx -= (x2 - nx + 1);
	}

// Map a y scan line (in the new output distorted space)...
// back to the original bounding rect
y += lpObject->rObject.top;

// The output span between fMin and fMax is the number of
// pixels to read between input points (fnx,fny) and (lnx,lny)
IntersectCorners( y, &fMin, &fMax, &fnx, &fny, &lnx, &lny );
f = lpObject->rTransform.top; f = TOFIXED( f );
fny -= f;
lny -= f;
f = lpObject->rTransform.left; f = TOFIXED( f );
fnx -= f;
lnx -= f;

// Compute the pixel count for the rate calculations
f = fMax - fMin;
iCount = ROUND((long)fMax) - ROUND((long)fMin);

// Compute the x and y rates to from the first input point to the last
if ( !iCount )
	{
	fxRate = UNITY;
	fyRate = -UNITY;
	}
else
	{
	fxRate = (lnx - fnx) / iCount;
	fyRate = (lny - fny) / iCount;
	}

iCount++; // Now its a loop count

// Compute the buffer indent
i = ROUND((long)fMin) - lpObject->rObject.left - x1;
if ( i >= dx )
	return( YES ); // Nothing to do

// Handle an off-image start
if ( i < 0 )
	{
	fnx -= (fxRate * i);
	fny -= (fyRate * i);
	iCount += i;
	i = 0;
	}
if ( iCount + i > dx )
	iCount = dx - i; // Don't go past the end of the buffers
if ( iCount <= 0 )
	return( YES ); // Nothing to do

// Indent the buffers
lpDataDst  = lpDataBuf + (i * iDataDepth);

// Make this a frame size check
right  = FrameXSize( lpDataFrame ) - 1;
bottom = FrameYSize( lpDataFrame ) - 1;

ClearByte = 0;
ClearRGB.red = ClearRGB.green = ClearRGB.blue  = 0;
ClearCMYK.c = ClearCMYK.m = ClearCMYK.y = ClearCMYK.k = 255;

if ( fHiQuality )
	{
	fright  = TOFIXED(right + 1);
	fbottom = TOFIXED(bottom + 1);
	fnegone = TOFIXED(-1);

	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					fnx >= fright  || fny >= fbottom )
					{
					*lpDataDst++  = ClearByte;
					}
				else
					{
					average_pixel8( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearByte, lpDataDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 3 :
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					fnx >= fright  || fny >= fbottom )
					{
					*lpRGBDst++   = ClearRGB;
					}
				else
					{
					average_pixel24( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearRGB, lpRGBDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				if ( fnx <= fnegone || fny <= fnegone ||
					fnx >= fright  || fny >= fbottom )
					{
					*lpCMYKDst++  = ClearCMYK;
					}
				else
					{
					average_pixel32( lpDataFrame, fnx, fny, 0, 0,
						bottom, right, FALSE, ClearCMYK, lpCMYKDst++ );
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}
else
	{
	switch( iDataDepth )
		{
		case 0 :
		case 1 :
			{
			while ( --iCount >= 0 )
				{
				nx = ROUND((long)fnx);
				ny = ROUND((long)fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpDataSrc = FramePointer( lpDataFrame, nx, ny, NO )) )
					{
					*lpDataDst++  = ClearByte;
					}
				else
					{
					*lpDataDst++  = *lpDataSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;

		case 3 :
			{
			lpRGBDst = (LPRGB)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = ROUND((long)fnx);
				ny = ROUND((long)fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpRGBSrc = (LPRGB)FramePointer( lpDataFrame, nx, ny, NO )) )
					{
					*lpRGBDst++   = ClearRGB;
					}
				else
					{
					*lpRGBDst++   = *lpRGBSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;

		case 4 :
			{
			lpCMYKDst = (LPCMYK)lpDataDst;

			while ( --iCount >= 0 )
				{
				nx = ROUND((long)fnx);
				ny = ROUND((long)fny);

				if ( nx < 0 || nx > right || ny < 0 || ny > bottom ||
					!(lpCMYKSrc = (LPCMYK)FramePointer( lpDataFrame, nx, ny, NO )) )
					{
					*lpCMYKDst++  = ClearCMYK;
					}
				else
					{
					*lpCMYKDst++  = *lpCMYKSrc;
					}
				fnx += fxRate;
				fny += fyRate;
				}
			}
		break;
		}
	}

return( YES );
}

/************************************************************************/
static BOOL IntersectCorners( int y, LPLFIXED lpX1, LPLFIXED lpX2,
	LPLFIXED lpFx1, LPLFIXED lpFy1, LPLFIXED lpFx2, LPLFIXED lpFy2 )
/************************************************************************/
{
LFIXED fPercent, lx, lxx[4], lpx[4], lpy[4], lxmin, lxmax;
int i, one, two;
#define P1 Distort.p[0]
#define P2 Distort.p[1]
#define P3 Distort.p[2]
#define P4 Distort.p[3]
#define DR Distort.RectOrig

i = 0;
if ( IntersectSegment( y, P1, P2, &lx, &fPercent ) )
	{ // iEdgeType = t0V;
	lxx[i] = lx;
	lpx[i] = TOFIXED( DR.left ) + fPercent * (DR.right - DR.left);
	lpy[i] = TOFIXED( DR.top );
	i++;
	}

if ( IntersectSegment( y, P2, P3, &lx, &fPercent ) )
	{ // iEdgeType = tU1;
	lxx[i] = lx;
	lpx[i] = TOFIXED( DR.right );
	lpy[i] = TOFIXED( DR.top ) + fPercent * (DR.bottom - DR.top);
	i++;
	}

if ( IntersectSegment( y, P3, P4, &lx, &fPercent ) )
	{ // iEdgeType = t1V;
	lxx[i] = lx;
	lpx[i] = TOFIXED( DR.right ) - fPercent * (DR.right - DR.left);
	lpy[i] = TOFIXED( DR.bottom );
	i++;
	}

if ( IntersectSegment( y, P4, P1, &lx, &fPercent ) )
	{ // iEdgeType = tU0;
	lxx[i] = lx;
	lpx[i] = TOFIXED( DR.left );
	lpy[i] = TOFIXED( DR.bottom ) - fPercent * (DR.bottom - DR.top);
	i++;
	}

switch ( i )
	{ // how many intersections did we get?
	case 2: // Perrrrfect!
	one = 0;
	two = 1;
	break;

	case 1: // Huh?
	one = 0;
	two = 0;
	break;

	case 3: // There must be a duplicate
	one = 0;
	two = ( lxx[1] != lxx[0] ? 1 : 2 );
	break;

	case 4: // Find the smallest and largest x
	lxmin = min( min( min( lxx[0], lxx[1] ), lxx[2] ), lxx[3] );
	lxmax = max( max( max( lxx[0], lxx[1] ), lxx[2] ), lxx[3] );
	one = ( lxx[0] == lxmin ? 0 :
		( lxx[1] == lxmin ? 1 :
		( lxx[2] == lxmin ? 2 : 3 ) ) );
	two = ( lxx[0] == lxmax ? 0 :
		( lxx[1] == lxmax ? 1 :
		( lxx[2] == lxmax ? 2 : 3 ) ) );
	break;

	case 0:
	// Didn't get any; bail out with null values
	*lpX1 = *lpX2 = *lpFx1 = *lpFx2 = *lpFy1 = *lpFy2 = 0;
	return( NO );
	}

// Check to see if we need to swap
if ( lxx[two] < lxx[one] )
	{ i = one; one = two; two = i; }

*lpX1  = lxx[one];
*lpX2  = lxx[two];
*lpFx1 = lpx[one];
*lpFx2 = lpx[two];
*lpFy1 = lpy[one];
*lpFy2 = lpy[two];

return( YES );
}


/************************************************************************/
// Does y intersect this segment? If so, at what x and percentage complete?
/************************************************************************/
static BOOL IntersectSegment( int y, POINT pa, POINT pb, LPLFIXED lpX,
	LPLFIXED lpPercent )
/************************************************************************/
{ // Does y intersect this segment? If so, at what x and percentage complete?
long ldy;

if ( (y < pa.y && y < pb.y) || (y > pa.y && y > pb.y) )
	return( NO );

/* if the segment is horizontal, forget about it */
if ( !(ldy = pb.y - pa.y) )
	return( NO );

if ( ldy > 0 )
		*lpPercent = FGET( y - pa.y, ldy );
else	*lpPercent = UNITY - FGET( y - pb.y, -ldy );

*lpX = TOFIXED( pa.x ) + *lpPercent * (pb.x - pa.x);
return( YES );
}



