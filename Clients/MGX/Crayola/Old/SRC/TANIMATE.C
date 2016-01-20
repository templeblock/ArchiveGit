//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "animate.h"

static int iTool, iCategory, iTotalCategories;
static HINSTANCE hOptions;
static LPOBJECT lpPrevious;
static BOOL bInMotion;
static HWND hDlgToyWnd;
static HWND hStartAnimationWnd = 0;
static BOOL bSingleStarted = FALSE;

extern HINSTANCE hInstAstral;


#define TIFF_RES_FORMAT     "TIF_%i%c"      // TIF_1a, TIF_1b, TIF_1z ...
#define MAX_ANIMATIONS		10
//#define ALLOW_CATEGORY_0	TRUE

/***********************************************************************/
static LPOBJECT AddObjectFromToy( LPSTR lpToyFileName, int id )
/***********************************************************************/
{
LPOBJECT lpObject;
HINSTANCE hToy;

if ( (hToy = LoadLibrary( lpToyFileName )) < (HINSTANCE)HINSTANCE_ERROR )
	return( NULL );

lpObject = AddObjectFromTiffResource( hToy, "TIF_1a" , FALSE);
FreeLibrary (hToy);

if ( lpObject )
	{
	lpObject->ObjectDataID = OBJECT_DATA_STOCK_ANIMATION;
	lpObject->dwObjectData = (WORD)id;
	}

return( lpObject );
}


/***********************************************************************/
LPOBJECT AddObjectFromTiffResource( HINSTANCE hInstance, LPSTR lpName,
  BOOL fBW)
/***********************************************************************/
{
LPFRAME		lpImageFrame, lpMaskFrame;
LPMASK		lpMask;
int			xData, yData, srcpix, srclin, depth, bytes, sy;
RECT		rObject, rDisplay;
LPOBJECT	lpObject;
LPTR		lpPtr;
BYTE		bR, bG, bB;

if ( !hInstance )
	return( NULL );
if ( !(lpImageFrame = TiffResource2Frame( hInstance, lpName )) )
	return( NULL );
if ( !(lpMaskFrame = MaskCreateAlphaFrame( lpImageFrame )) )
	{
	FrameClose( lpImageFrame );
	return( NULL );
	}
if ( !(lpMask = MaskCreate( lpMaskFrame,0,0,ON,YES )) )
	{
	FrameClose( lpImageFrame );
	FrameClose( lpMaskFrame );
	return( NULL );
	}

xData = FrameXSize(lpImageFrame);
yData = FrameYSize(lpImageFrame);
rDisplay = lpImage->lpDisplay->FileRect;
rObject.left = ( rDisplay.left + rDisplay.right - xData ) / 2;
rObject.top  = ( rDisplay.top + rDisplay.bottom - yData ) / 2;
if (rObject.left < rDisplay.left)
    rObject.left = rDisplay.left;
if (rObject.top < rDisplay.top)
    rObject.top = rDisplay.top;
rObject.right = rObject.left + xData - 1;
rObject.bottom = rObject.top + yData - 1;

// black & white sticker ?
if (fBW)
{
	depth = FrameDepth (lpImageFrame);
	if (depth == 3)
	{
		srcpix = FrameXSize (lpImageFrame);
		srclin = FrameYSize (lpImageFrame);
		for (sy = 0; sy < srclin; ++sy)
		{
			lpPtr = FramePointer(lpImageFrame, 0, sy, TRUE);
			if (! lpPtr)
			{
				FrameError (IDS_EFRAMEREAD);
				return (NULL);
			}
			else
			{
				bytes = srcpix;
				while (--bytes)
				{
					bR = *lpPtr;
					bG = *(lpPtr +1);
					bB = *(lpPtr +2);
					if ( (RGB(bR,bG,bB) != RGB(0,0,0) ) &&
					  ( RGB(bR,bG,bB) != RGB(255,255,255) ) )
					{
						*lpPtr++ = 255;
						*lpPtr++ = 255;
						*lpPtr++ = 255;
					}
					else
						lpPtr += 3;
				}
			}
		}
	}
}

if ( !(lpObject = ObjCreateFromFrame( ST_TEMPORARY,
		lpImageFrame,lpMask,&rObject, Control.NoUndo)) )
	{
	FrameClose( lpImageFrame );
	MaskClose( lpMask );
	return( NULL );
	}

ImgAddNewObject( lpImage, lpObject );
return( lpObject );
}


/***********************************************************************/
int CALLBACK EXPORT StartAnimation (HWND hWnd)
/***********************************************************************/
{
    FNAME	szFileName;
	STRING	szString;

    int NumObjects = 0;
    LPOBJECT lpObject = ImgGetBase(lpImage);
	if (lpObject)
		lpObject = ImgGetNextObject(lpImage, lpObject, NO, YES);
	hStartAnimationWnd = hWnd;

	StopAnimation ();
    ProgressBegin(1, IDS_PROGPASTE);
	SetDeferedSound(TRUE);

    while (lpObject && NumObjects < Control.iMaxAnimations)
    {
        if (!lpObject->fDeleted &&
            lpObject->ObjectDataID==OBJECT_DATA_STOCK_ANIMATION)
        {
			wsprintf( szString, "%04d", (int)(WORD)lpObject->dwObjectData );
            if ( GetToyFileName( szString, szFileName ) )
            {
                lpObject->fHidden = TRUE;
                lpObject->fChanged = TRUE;
/*				
				if (hWnd)
				{
					InvalidateRect(hWnd, &lpObject->rObject, TRUE);
					UpdateWindow(hWnd);
				}
				else
                	UpdateImage (&lpObject->rObject, YES);
*/
                if (AnimateBatchAdd (lpObject->rObject.left,
                                     lpObject->rObject.top,
                                     szFileName))
                    NumObjects++;
            }
        }
        lpObject = ImgGetNextObject(lpImage, lpObject, NO, NO);
    }
	SetDeferedSound(FALSE);
    ProgressEnd();

// Start the animations playing.
    if (NumObjects)
		if (hWnd)
        	AnimateBatchStart (hWnd);
		else
        	AnimateBatchStart (lpImage->hWnd);

    return NumObjects;
}


/***********************************************************************/
void CALLBACK EXPORT StopAnimation ()
/***********************************************************************/
{
    LPOBJECT lpObject = ImgGetBase(lpImage);
	if (!AnimateIsOn()) 
		return;

    while (lpObject)
    {
        if (lpObject->ObjectDataID==OBJECT_DATA_STOCK_ANIMATION)
        {
			if ( lpObject->fHidden )
			{
	            lpObject->fChanged = TRUE;
	            lpObject->fHidden = FALSE;
			}
        }
        lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO);
    }

    AnimateBatchEnd(hStartAnimationWnd ? hStartAnimationWnd : lpImage->hWnd);
/*
	if (hStartAnimationWnd)
	{
		InvalidateRect(hStartAnimationWnd, NULL, TRUE);
		UpdateWindow(hStartAnimationWnd);
	}
	else
    	UpdateImage(NULL, YES);
*/

	if (hDlgToyWnd)
		CheckDlgButton(hDlgToyWnd, IDC_TOY_SW1, FALSE);

#ifdef USEWAVEMIX
	if (Control.bUseWaveMix)
		mixSoundStopAll();
#endif // USEWAVEMIX
}


/***********************************************************************/
BOOL WINPROC EXPORT DlgToyProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
FNAME szFileName;
STRING szOptions, szString;
LPOBJECT lpObject;
int iRealCategory;

switch (msg)
    {
    case WM_INITDIALOG:
#ifdef ALLOW_CATEGORY_0
	iCategory = -1;
#endif	
	hDlgToyWnd = hDlg;
	SetControlFont (hDlg, IDC_TOY_CATEGORY);
	RibbonInit( hDlg );
	iTotalCategories = GetExtNameCount( IDN_TOY );
	hOptions = OptionLoadLibrary( hDlg, IDC_TOY_CATEGORY, 11/*iCount*/,
		IDN_TOY, iCategory, iTotalCategories );
	OptionGetString( hOptions, 0, szOptions );
	SetWindowText( GetDlgItem( hDlg, IDC_TOY_CATEGORY ), szOptions );
	bInMotion = NO;
	CheckDlgButton( hDlg, IDC_TOY_SW1, bInMotion );
	lpPrevious = NULL;
	// Select the top object
	if ( lpObject = ImgGetNextObject( lpImage, NULL/*lpObject*/, NO/*fBottomUp*/, NO/*fLoop*/ ) )
		ImgChangeSelection( lpImage, lpObject, YES/*fSelect*/, NO/*fExtendedSel*/ );
	// fall thru...

	case WM_CONTROLENABLE:
	case WM_DOCACTIVATED:
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_TOY ) );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaint( hDlg );
	break;

    case WM_DESTROY:
	if ( hOptions != hInstAstral )
		FreeLibrary( hOptions );
	StopAnimation();
	hDlgToyWnd = 0;
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
		case IDC_TOY_CATEGORY:
		if ( hOptions != hInstAstral )
			FreeLibrary( hOptions );
		if ( iTotalCategories )
			iCategory = (iCategory+1) % iTotalCategories;
		else
			iCategory = 0;
		hOptions = OptionLoadLibrary( hDlg, IDC_TOY_CATEGORY, 11/*iCount*/,
			IDN_TOY, iCategory, iTotalCategories );
		OptionGetString( hOptions, 0, szOptions );
		SetWindowText( GetDlgItem( hDlg, IDC_TOY_CATEGORY ), szOptions );
		break;

		case IDC_TOY_T1:
		case IDC_TOY_T2:
		case IDC_TOY_T3:
		case IDC_TOY_T4:
		case IDC_TOY_T5:
		case IDC_TOY_T6:
		case IDC_TOY_T7:
		case IDC_TOY_T8:
		case IDC_TOY_T9:
		case IDC_TOY_T10:
		if ( !lpImage )
			break;
		if ( lpPrevious )
			{
			DeactivateTool();
			ImgDeleteSelObjects( lpImage );
			}
		bInMotion = FALSE;
		CheckDlgButton(hDlg, IDC_TOY_SW1, FALSE);
		StopAnimation();
		SoundStopAndFree();
#ifdef USEWAVEMIX
		if (Control.bUseWaveMix)
			mixSoundStopAll();
#endif // USEWAVEMIX

		iRealCategory = OptionGetCategoryNum(iCategory, iTotalCategories, IDN_TOY);
        iTool = wParam - IDC_TOY_T1 + 1; 
		wsprintf( szString, "%02d%02d", iRealCategory, iTool );
		if ( !GetToyFileName( szString, szFileName ) )
            break;

        ProgressBegin(1, IDS_PROGPASTE);
        lpPrevious = AddObjectFromToy( szFileName, Ascii2Int(szString) );
		// Select the new object
		if ( lpPrevious )
			ImgChangeSelection( lpImage, lpPrevious, YES/*fSelect*/, NO/*fExtendedSel*/ );
		ProgressEnd();
		break;

	    case IDC_TOY_SW1:
		bInMotion = !bInMotion;
        CheckDlgButton( hDlg, wParam, bInMotion );
        if (bInMotion)
        {
            if (StartAnimation (0) == 0)
			{
                bInMotion = FALSE;
				CheckDlgButton(hDlg, wParam, bInMotion);
			}
        }
        else
		{
            StopAnimation ();
			SoundStopAndFree();
#ifdef USEWAVEMIX
			if (Control.bUseWaveMix)
				mixSoundStopAll();
#endif // USEWAVEMIX
		}
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }
return( TRUE );
}

static RECT SelectRect;
static int xOffset, yOffset;
static POINT LastPoint;
static LPDISPLAYHOOK lpLastDisplayHook;

/************************************************************************/
int ToyProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y, dx, dy;
RECT rOrigObject, rBase;
LPOBJECT lpObject;
WORD wKey;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
		if (!ONIMAGE(x, y))
			break;
		Display2File( &x, &y );
		if ( !ImgSelectObjectType( lpImage, x, y,
			NO/*bAllowBase*/, YES/*bAllowMovable*/, YES/*bAllowPlayable*/ ) )
				break;
		Tool.bActive = YES;
		break;

	case WM_SETCURSOR:
		return( SetToolCursor( hWindow, ID_MOVE_OBJECT, ID_PLAY_OBJECT ) );

	case WM_LBUTTONDOWN:
		Display2File( &x, &y );
		LastPoint.x = x;
		LastPoint.y = y;
		xOffset = yOffset = 0;
		ImgGetSelObjectRect(lpImage, &SelectRect, YES);
		AstralUpdateWindow(hWindow);
		lpLastDisplayHook = SetDisplayHook(hWindow, SelectorHook);
		SelectorHook(0, NULL);
		break;

    case WM_LBUTTONUP:
        Tool.bActive = NO;
		AstralCursor(IDC_WAIT);
		SelectorHook(0, NULL);
		SetDisplayHook(hWindow, lpLastDisplayHook);
		lpLastDisplayHook = NULL;
		ImgGetSelObjectRect(lpImage, &rOrigObject, YES);
		dx = SelectRect.left - rOrigObject.left;
		dy = SelectRect.top - rOrigObject.top;
		lpObject = NULL;
		while (lpObject = ImgGetSelObject(lpImage, lpObject))
			{
			rOrigObject = lpObject->rObject;
            if (dx || dy)
            {
				OffsetRect(&lpObject->rObject, dx, dy);
				if (!lpObject->Pixmap.fNewFrame &&
						EqualRect(&rOrigObject, &lpObject->rUndoObject))
					lpObject->rUndoObject = lpObject->rObject;
				UpdateImage(&rOrigObject, TRUE);
				UpdateImage(&lpObject->rObject, TRUE);
            }
            else
            {
                if (lpImage)
                {
                    if (lpObject &&
                        lpObject->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)
                    {
						CheckDlgButton(hDlgToyWnd, IDC_TOY_SW1, TRUE);
                        ImgPlaySingleAnimation (lpImage,lpObject,FALSE);
						CheckDlgButton(hDlgToyWnd, IDC_TOY_SW1, FALSE);
						bSingleStarted = TRUE;
                    }
                }

            }

            }
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
		AstralCursor(NULL);
		break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
		lpPrevious = NULL; // since an object is being moved, it will be kept
		Display2File(&x, &y);
		dx = x - LastPoint.x;
		dy = y - LastPoint.y;
		if (!dx && !dy)
			break;
		LastPoint.x = x;
		LastPoint.y = y;
		SelectorHook(0, NULL);
		OffsetRect(&SelectRect, dx, dy);
		xOffset += dx;
		yOffset += dy;
		rBase = ImgGetBase(lpImage)->rObject;
		dx = dy = 0;
		if (SelectRect.left > rBase.right)
			dx = rBase.right - SelectRect.left;
		if (SelectRect.right < rBase.left)
			dx = rBase.left - SelectRect.right;
		if (SelectRect.top > rBase.bottom)
			dy = rBase.bottom - SelectRect.top;
		if (SelectRect.bottom < rBase.top)
			dy = rBase.top - SelectRect.bottom;
		OffsetRect(&SelectRect, dx, dy);
		xOffset += dx;
		yOffset += dy;
		SelectorHook(0, NULL);
		break;

	case WM_KEYDOWN:
	if (!lpImage || Tool.bActive)
		break;
	wKey = lParam;
	switch (wKey)
		{
		case VK_DELETE:
			DeactivateTool();
			ImgDeleteSelObjects( lpImage );
			// Select the top object
			if ( lpObject = ImgGetNextObject( lpImage, NULL/*lpObject*/, NO/*fBottomUp*/, NO/*fLoop*/ ) )
				ImgChangeSelection( lpImage, lpObject, YES/*fSelect*/, NO/*fExtendedSel*/ );
			break;
		default:
			break;
		}
	break;

	case WM_KEYUP:
		break;

    case WM_LBUTTONDBLCLK:
		if ( !lpImage )
			break;
		break;

    case WM_DESTROY:	// The cancel operation message
        if ( !Tool.bActive )
			break;
		SelectorHook(0, NULL);
		SetDisplayHook(hWindow, lpLastDisplayHook);
		lpLastDisplayHook = NULL;
		Tool.bActive = NO;
		break;
    }
return(TRUE);
}


/************************************************************************/
BOOL GetToyFileName( LPSTR lpString, LPSTR lpFileName )
/************************************************************************/
{
if ( !lpFileName )
	return( FALSE );
*lpFileName = '\0';
if ( !lpString )
	return( FALSE );
lstrcpy( lpFileName, Control.PouchPath );
#ifdef WIN32
wsprintf( lpFileName + lstrlen(lpFileName), "toy%hs.toy", lpString );
#else
wsprintf( lpFileName + lstrlen(lpFileName), "toy%ls.toy", lpString );
#endif
return( TRUE );
}


/************************************************************************/
BOOL SetToolCursor( HWND hWindow, ITEMID idMoveObject, ITEMID idPlayObject )
/************************************************************************/
{
POINT pos;
LPOBJECT lpObject;

if (!lpImage)
	return( FALSE );

GetCursorPos(&pos);
ScreenToClient(hWindow, &pos);
Display2File((LPINT)&pos.x, (LPINT)&pos.y);
if ( !(lpObject = ImgFindObject( lpImage, pos.x, pos.y,
	NO/*fBottomUp*/, YES /*fExcludeBase*/ )) )
	{ // no object
	SetWindowCursor( Tool.Prim.idCursor );
	}
else
if ( IS_STOCK_ANIMATION(lpObject) )
	{ // a playable object
	if ( !idPlayObject ) idPlayObject = Tool.Prim.idCursor;
	SetWindowCursor( idPlayObject );
	}
else
if ( lpObject != ImgGetBase(lpImage) )
	{ // a movable object
	if ( !idMoveObject ) idMoveObject = Tool.Prim.idCursor;
	SetWindowCursor( idMoveObject );
	}
else
	{ // the base object
	SetWindowCursor( Tool.Prim.idCursor );
	}
return( TRUE );
}


/************************************************************************/
LPOBJECT ImgSelectObjectType( LPIMAGE lpImage, int x, int y, BOOL bAllowBaseObject, BOOL bAllowMoveObject, BOOL bAllowPlayObject )
/************************************************************************/
{
LPOBJECT lpObject;
RECT rect;

if ( !(lpObject = ImgFindObject( lpImage, x, y, NO/*fBottomUp*/, !bAllowBaseObject )) )
	return( NULL );
if ( lpObject->fHidden )
	return( NULL );
if ( IS_STOCK_ANIMATION(lpObject) )
	{ // a playable object
	if ( !bAllowPlayObject )
		{
		ImgPlaySingleAnimation( lpImage, lpObject, TRUE );
		return( NULL );
		}
	}
else
if ( lpObject != ImgGetBase(lpImage) )
	{ // a movable object
	if ( !bAllowMoveObject )
		return( NULL );
	}
else
	{ // the base object
	if ( !bAllowBaseObject )
		return( NULL );
	}

// Select the object and return it
RemoveObjectMarquee( lpImage );
SetRect( &rect, x, y, x, y );
ImgSelObject( lpImage, NULL, &rect, NO/*fExtendedSel*/, !bAllowBaseObject );
ImgAdjustLayerSelObj( lpImage, YES/*fToTop*/, NO/*fOneLevel*/ );
return( ImgGetSelObject( lpImage, NULL ) );
}

/************************************************************************/
void ImgDeleteSelObjects( LPIMAGE lpImage )
/************************************************************************/
{
LPOBJECT lpObject;
RECT rDisplay;

if (ImgGetSelObject(lpImage, NULL) == ImgGetBase(lpImage))
	return;
if (!ImgGetSelObjectRect(lpImage, &rDisplay, YES/*bExcludeBase*/))
	return;
if (!ImgEditInit(lpImage, ET_OBJECT, UT_DELETEOBJECTS, ImgGetBase(lpImage)))
	return;
lpObject = NULL;
while (lpObject = ImgGetSelObject(lpImage, lpObject))
	lpObject->fDeleted = YES;
ImgEditedObject(lpImage, ImgGetBase(lpImage), IDS_UNDODELETE, NULL);
UpdateImage(&rDisplay, YES);
if ( Tool.hRibbon )
	SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 0, 0L );
}


/************************************************************************/
void SelectorHook(HDC hDC, LPRECT lpRect)
/************************************************************************/
{
RECT rUpdate, rDisplay;
LPOBJECT lpObject;
if (!hDC) // our internal call
	hDC = Window.hDC;
else
	{ // an image redisplay call
	if ( lpLastDisplayHook )
		(*lpLastDisplayHook)( (HDC)hDC, (LPRECT)lpRect );
	}

File2DispRectExact(lpImage->hWnd, &SelectRect, &rDisplay);
lpObject = ImgGetSelObject(lpImage, NULL);
ImgGetSelObjectRect(lpImage, &rUpdate, YES);
OffsetRect(&rUpdate, xOffset, yOffset);
InflateRect(&rUpdate, 1, 1);
File2DispRect(&rUpdate, &rUpdate);
PaintMarquee(lpImage, hDC, lpObject, NULL, YES, &rUpdate,
	0, 0, xOffset, yOffset);
}

