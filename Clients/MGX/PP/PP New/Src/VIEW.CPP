//®PL1¯®FD1¯®BT0¯®TP0¯®RM250¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ipframe.h"
#include <process.h>
#include "id.h"
#include "cmdlist.h"
#include "cpo.h"

// Static Prototypes
static void ScrollStart( HWND hWnd );
static void ScrollEnd( HWND hWnd );
static int ComputeZoom(HWND hWnd, int x,int y, int value, BOOL fIncremental, LFIXED *lfFileRate, int *Percentage );
static void GetDispRect( HWND hWnd, LPRECT lpRect, int iHorzMode, int iVertMode );

extern RECT rClient;

__declspec( dllimport ) LPKCMSCNCTPROC lpKCMSCnctProc;
__declspec( dllimport ) LPKCMSFREEPROC lpKCMSFreeProc;

/***********************************************************************/
LPIMAGE CreateImage(
    LPSTR       szName,         // image filename
    LPFRAME     lpFrame,        // frame - if NULL, use lpObject
    LPOBJECT    lpObject,       // object list pointer - if NULL, use lpFrame
    LPCMDLIST   lpCmdList,      // command processing list
    ITEMID      FileType,       // image file type - IDN_TIFF, etc.
    ITEMID      DataType,       // image data type - IDC_SAVECT, etc.
    int         DocumentType,   // bitwise document type, normal, fastbits, lowres
    LPSTR       ImageName)      // name used for internal use only
/***********************************************************************/
{
	CImage *pNewImage;
	BOOL fSuccess;

	ASSERT((lpFrame != NULL) || (lpObject != NULL));

	TRY
	{
		pNewImage = new CImage(DataType, szName, lpCmdList, FileType, DocumentType, ImageName);
	}
	CATCH_ALL(e)
	{
		Message(IDS_EMEMALLOC);
		return(NULL);
	}
	END_CATCH_ALL

	if (lpObject)
 		fSuccess = pNewImage->Create(lpObject);
	else
 		fSuccess = pNewImage->Create(lpFrame);

	if (!fSuccess)
	{
	 	delete pNewImage;
		pNewImage = NULL;
	}

	return( pNewImage );
}

/***********************************************************************/
void DestroyImage(LPIMAGE lpImage)
/***********************************************************************/
{
	delete lpImage;
}

/***********************************************************************/
void ImgColorMapChanged(LPIMAGE lpImage)
/***********************************************************************/
{
// free up display info that is based on colormap
if (lpImage->lpPaletteLUT)
    {
    FreeUp(lpImage->lpPaletteLUT);
    lpImage->lpPaletteLUT = NULL;
    }
}

/***********************************************************************/
void ImgInitDisplay(LPIMAGE lpImage, BOOL fGeneratePaletteLUT)
/***********************************************************************/
{
LPTR lpPaletteLUT;
RGBS RGBmap[256];
LPRGB lpRGBmap;
int nColors;
FRMTYPEINFO TypeInfo, DstTypeInfo = ColorManager.Monitor.dst;
LPOBJECT lpObject;

lpObject = lpImage->GetDisplayObject();
if (!lpObject)
	return;
FrameGetTypeInfo(ObjGetEditFrame(lpObject), &TypeInfo);

if ( lpBltScreen->BitMapInfo.bmiHeader.biBitCount == 8 &&
        !lpImage->lpPaletteLUT)
    {
    ProgressBegin(1, PROGRESS_ID(IDS_UNDOSETUPDISPLAY));
    if (TypeInfo.DataType == FDT_PALETTECOLOR)
        lpPaletteLUT = Alloc(32768L);
    else
        lpPaletteLUT = NULL;
    if (lpPaletteLUT)
        {
        if (TypeInfo.ColorMap->NumEntries > MAX8BITCOLORS)
            {
            nColors = MAX8BITCOLORS;
            lpRGBmap = RGBmap;
            if (!ReducePalette(TypeInfo.ColorMap->RGBData,  TypeInfo.ColorMap->NumEntries,
                                lpRGBmap, nColors))
                {
                FreeUp(lpPaletteLUT);
                lpPaletteLUT = NULL;
                }
            }
        else
            {
            lpRGBmap = TypeInfo.ColorMap->RGBData;
            nColors = TypeInfo.ColorMap->NumEntries;
            }
        }
    if (lpPaletteLUT)
        {
		if (fGeneratePaletteLUT)
	        CreatePaletteLut15(lpRGBmap, nColors, lpPaletteLUT, AstralClockCursor);
        lpImage->lpPaletteLUT = lpPaletteLUT;
        lpImage->PaletteType = PT_CUSTOMPALETTE;
        lpImage->hPal = CreateLogicalPalette(lpRGBmap, nColors);
		  GetObject(lpImage->hPal, sizeof(nColors), (void *)&nColors);
        lpImage->nPaletteEntries = GetPaletteEntries(lpImage->hPal,
                        0, nColors, lpImage->Palette);
        }
    else
        {
        if ( FrameType(ImgGetBaseEditFrame(lpImage)) <= FDT_GRAYSCALE)
            {
            lpImage->hPal = lpBltScreen->hGrayPal;
            lpImage->nPaletteEntries = lpBltScreen->nGrayEntries;
            lpImage->PaletteType = lpBltScreen->GrayPaletteType;
            lpImage->lpPaletteLUT = lpBltScreen->lpGrayPaletteLUT;
            copy((LPTR)lpBltScreen->GrayPalette,
                 (LPTR)lpImage->Palette,
                 sizeof(lpImage->Palette));
            }
        else
            {
            lpImage->hPal = lpBltScreen->hColorPal;
            lpImage->nPaletteEntries = lpBltScreen->nColorEntries;
            lpImage->PaletteType = lpBltScreen->ColorPaletteType;
            lpImage->lpPaletteLUT = lpBltScreen->lpColorPaletteLUT;
            copy((LPTR)lpBltScreen->ColorPalette,
                 (LPTR)lpImage->Palette,
                 sizeof(lpImage->Palette));
            }
        }
    ProgressEnd();
    }

if (lpImage->m_cmsXform)
	{
	if( Control.CMSEnabled && lpKCMSFreeProc )
		{
		if (lpImage->PtInfo.toRCS != TypeInfo.ptInfo.toRCS &&
			lpImage->PtInfo.frRCS != TypeInfo.ptInfo.frRCS )
			{
			( lpKCMSFreeProc )( lpImage->m_cmsXform );
			lpImage->m_cmsXform = NULL;
			}
		}
	else
		lpImage->m_cmsXform = NULL;
	}

	
if( Control.CMSEnabled &&
	IsSrcPTSelected( &TypeInfo.ptInfo ) &&
	IsDstPTSelected( &DstTypeInfo.ptInfo ) &&
	lpKCMSCnctProc )
	{
	lpImage->m_bDoCmsGamma = YES;
	if (!lpImage->m_cmsXform)
		{
		if( ( !FrameTypeInfoEqual( TypeInfo, DstTypeInfo ) ) &&
				ConvertCRCtoUID( &TypeInfo, &DstTypeInfo ) )
			{
			// save for later check to avoid a monitor reconnect.
			lpImage->PtInfo = TypeInfo.ptInfo;
			lpImage->m_cmsXform = ( *lpKCMSCnctProc )( &TypeInfo, &DstTypeInfo );
			}
		else
			lpImage->m_cmsXform = NULL;
		}
	}
else
	lpImage->m_cmsXform = NULL;
}


/************************************************************************/
void ViewXY( HWND hWnd, int x, int y )
/************************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
ComputeFileRect(hWnd, x, y, lpDisplay->FileRate);
SetupRulers( hWnd );
InvalidateRect( hWnd, NULL, TRUE );
}

/************************************************************************/
void ViewRect(HWND hWnd, LPRECT lpRect,  BOOL fResizeWindow)
/************************************************************************/
{
int x, y, left, top;
RECT rMax;
LPDISPLAY lpDisplay;

HWND hMDIWindow = hWnd;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

lpDisplay = GetDisplayPtr(hWnd);

if (fResizeWindow) // if allowed to change size use a max size
    GetClientRect(PictPubApp.Get_hClientAstral(), &rMax);
else // if window size will not change use current DispRect
    rMax = lpDisplay->DispRect;
if (!CanZoom(hWnd, 0L, lpRect, &rMax))
    return;
SaveLastView(hWnd);
lpDisplay->FileRect = *lpRect;
lpDisplay->FileRate = 0;
lpDisplay->ViewPercentage = 0;
x = (lpDisplay->FileRect.left 
    + lpDisplay->FileRect.right+1) / 2;
y = ( lpDisplay->FileRect.top 
    + lpDisplay->FileRect.bottom+1) / 2;
if (fResizeWindow)
    {
    GetWinXY(hMDIWindow, &left, &top);
    FullViewWindow(hWnd, left, top, x, y, NO);
    }
else
    {
    ComputeDispRect(hWnd);
    ComputeFileRect(hWnd, x, y, lpDisplay->FileRate);
    SetupRulers(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
    }
ZoomMainViewChanged(hWnd);
}

/************************************************************************/
void FullViewWindow( HWND hWindow, int x, int y, int cx, int cy,
                     BOOL fClipToImage )
/************************************************************************/
{
RECT NewWindowRect;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

HWND hMDIWindow = hWindow;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

// if window is iconic, do nothing here
if (IsIconic(hMDIWindow))
    return;

lpImage = GetImagePtr(hWindow);
lpDisplay = GetDisplayPtr(hWindow);

// if window is maximized, do not change the size of the window
if ( !IsZoomed(hMDIWindow) )
    {
    // get the difference between the client area and the window area
    // not including scrollbars (i.e. - pretend scrollbars aren't there)
    // this is so we know how much extra size is needed for caption,
    // borders, etc. when we size the window
    CalcFullViewWindowSize( &NewWindowRect, &lpDisplay->FileRect,
        lpDisplay->FileRate, lpImage->npix, lpImage->nlin,
        x, y, lpDisplay->HasRulers, fClipToImage );
    // now actually size the window, it's possible nothing will happen here
    ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CServerView)));        
    pWnd->GetParentFrame()->MoveWindow(NewWindowRect.left, NewWindowRect.top,
        RectWidth(&NewWindowRect), RectHeight(&NewWindowRect), TRUE);
    }

// recompute DispRect and FileRect for cx and cy
// and also it's possible there was no WM_SIZE
ComputeDispRect(hWindow);
ComputeFileRect(hWindow, cx, cy, lpDisplay->FileRate );
SetupRulers(hWindow);
InvalidateRect(hWindow, NULL, TRUE);
}

/************************************************************************/
void GetDocumentPosition(LPINT lpX, LPINT lpY)
/************************************************************************/
{
	CMDIFrame* pMDIFrame = (CMDIFrame*)PictPubApp.m_pMainWnd;
	int nDocs = pMDIFrame->GetNumMDIChildWindows() - 1; // don't count ourself
    int stagger = GetSystemMetrics(SM_CYCAPTION) + 
		GetSystemMetrics(SM_CYFRAME) - 2;
	*lpY = Control.yImage;
	if (nDocs)	
	    *lpY = *lpY + GetSystemMetrics(SM_CYFRAME) + (((nDocs)%5)*stagger);
    *lpX = Control.xImage + (((nDocs)%5)*stagger) + (((nDocs)/5)*stagger);
}

/************************************************************************/
void CalcFullViewWindowSize( LPRECT lpWindowRect, LPRECT lpFileRect,
                              LFIXED FileRate, int npix, int nlin, int x,
                              int y, BOOL fHasRulers, BOOL fClipToImage )
/************************************************************************/
{
int dx, dy, sx, sy, rs, iImageWidth, iImageHeight, iWidth, iHeight;
LFIXED DispRate;
int minsize;

// normal window - to goal is to size the window to fit the view
// If FileRate == 0 or 1, then we make the window as big as possible
// for the FileRect provided.  If FileRate != 0, then we make the
// the window as big as possible to fit the image at that FileRate.
// If the fClipToImage flag and FileRate == 0, then we do not let
// the FileRate go over 100%.  This is used for a caller who wants
// to display the image pixel for pixel, if possible.
if (x < 0 || y < 0)
	GetDocumentPosition(&x, &y);

dx = 2 * GetSystemMetrics(SM_CXFRAME);
dy = GetSystemMetrics(SM_CYCAPTION) + (2*GetSystemMetrics(SM_CYFRAME)) - 1;

// get size of scrollbars and rulers for use later
GetScrollBarSize(&sx, &sy);
rs = GetRulerSize();

// determine maximum area for window to occupy
if (IsIconic(PictPubApp.Get_hWndAstral()))
    *lpWindowRect = rClient;
else
    GetClientRect(PictPubApp.Get_hClientAstral(), lpWindowRect);
lpWindowRect->top = y;
lpWindowRect->left = x;
lpWindowRect->right -= dx;
lpWindowRect->bottom -= (dy + GetSystemMetrics(SM_CYFRAME));

// start with this maximum Client Area size
minsize = MIN_WINDOW_SIZE+rs;
iWidth = RectWidth(lpWindowRect);
if ( iWidth < minsize )
    iWidth = minsize;
iHeight = RectHeight(lpWindowRect);
if ( iHeight < minsize )
    iHeight = minsize;

// If we already have a FileRate, calculate new size of window
// based on the image size
if (FileRate > TINYRATE)
    {
    // calculate size of area for total image at this
    // FileRate
    DispRate = FGET(FUNITY, FileRate);
    iImageWidth = FMUL(npix, DispRate);
    iImageHeight = FMUL(nlin, DispRate);

    // add in additional space for rulers if necessary
    if (fHasRulers)
        {
        iImageWidth += rs;
        iImageHeight += rs;
        }
    if (iImageWidth > iWidth && iImageHeight < iHeight)
        iImageHeight += sy;
    if (iImageHeight > iHeight && iImageWidth < iWidth)
        iImageWidth += sx;

    // see if we don't need the total width
    // otherwise, use the maximum
    if (iImageWidth < iWidth)
        iWidth = iImageWidth;
    // see if we don't need the total height
    // otherwise, use the maximum
    if (iImageHeight < iHeight)
        iHeight = iImageHeight;
    }
// if we have no FileRate, then somebody wants a specific
// FileRect, figure out how big the window needs to be and
// whether we need rulers
else
    {
    // get width and height of area to be displayed
    iImageWidth = RectWidth(lpFileRect);
    iImageHeight = RectHeight(lpFileRect);

    // reduce the maximum area if we have rulers
    if (fHasRulers)
        {
        iWidth -= rs;
        iHeight -= rs;
        }

    // reduce the maximum area if we need scrollbars
    if (lpFileRect->left != 0 ||
        lpFileRect->right != npix-1 ||
        lpFileRect->top != 0 ||
        lpFileRect->bottom != nlin-1)
        {
        iWidth -= sx;
        iHeight -= sy;
        }

    // scale maximum client area size (not including rulers and
    // scrollbars) to fit aspect ratio of FileRect
    DispRate = ScaleToFit(&iWidth, &iHeight, iImageWidth, iImageHeight);

    // if caller wants to clip to hires to achieve 100% view,
    // clip.  But only if hires is smaller
    if (iImageWidth <= iWidth && iImageHeight <= iHeight && fClipToImage)
        {
        iWidth = iImageWidth;
        iHeight = iImageHeight;
        }

    // add ruler size back into client area
    if (fHasRulers)
        {
        iWidth += rs;
        iHeight += rs;
        }

    // add scrollbar size back into client area
    if (lpFileRect->left != 0 ||
        lpFileRect->right != npix-1 ||
        lpFileRect->top != 0 ||
        lpFileRect->bottom != nlin-1)
        {
        iWidth += sx;
        iHeight += sy;
        }
    }
// Calculate new window size based on iWidth and iHeight */
lpWindowRect->right = lpWindowRect->left + iWidth + dx - 1;
lpWindowRect->bottom = lpWindowRect->top + iHeight + dy - 1;
}

/************************************************************************/
BOOL CanZoom( HWND hWnd, LFIXED FileRate, LPRECT lpFileRect,
               LPRECT lpDispRect )
/************************************************************************/
{
LFIXED DispRate;
long lFileWidth, lFileHeight;
int iDispWidth, iDispHeight;
LPIMAGE lpImage;

lpImage = GetImagePtr(hWnd);
if (FileRate != 0)  // FileRate passed in
    DispRate = FGET(FUNITY, FileRate);
else        // Displaying a FileRect
    {
    // calculate DispRate using this FileRect and
    // DispRect.
    iDispWidth = RectWidth(lpDispRect);
    iDispHeight = RectHeight(lpDispRect);
    DispRate = ScaleToFit(&iDispWidth, &iDispHeight,
                RectWidth(lpFileRect),
                RectHeight(lpFileRect));
    }
// Calculate eventual size in File coordinates using
// this DispRate.  This calculation is done by most
// of the viewing routines and has the possibility
// of overflow.  Usually in these routines when the
// calculation is made it is too late to turn back
// so those routines call CanZoom() before doing
// anything that is unreversible.
lFileWidth = FMUL(lpImage->npix, DispRate);
lFileHeight = FMUL(lpImage->nlin, DispRate);
// Although MAXINT is 32767, I decided to use 30000 just
// for some slop and my own paranoia
return(lFileWidth > 0L && lFileWidth <= MAX_IMAGE_WIDTH &&
       lFileHeight > 0L && lFileHeight <= MAX_IMAGE_HEIGHT);
}

/************************************************************************/
void ComputeDispRect(HWND hWnd)
/************************************************************************/
{
int w, h, dx, dy;
int iWidth, iHeight, iImageWidth, iImageHeight;
LFIXED DispRate;
RECT DispRect;
BOOL bZoomed, bIconic;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

lpImage = GetImagePtr(hWnd);
lpDisplay = GetDisplayPtr(hWnd);

BOOL bInPlaceFrameType = FALSE;
HWND hMDIWindow = hWnd;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
{
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();
	bInPlaceFrameType = ((CServerView*)pWnd)->GetDocument()->IsInPlaceActive();
}

// get max size of client area with no scrollbars
GetDispRect(hWnd, &DispRect, 1, 1);
w = RectWidth(&DispRect);
h = RectHeight(&DispRect);

// If we have no FileRate to this point, it means somebody wants
// to display a specific FileRect.  Calculate a FileRate so that
// we can calculate a DispRect.  This is necessary because it is
// possible (when the window is maximized for example) that the
// FileRect requested will not fill up the window, so it would
// need to be recalculated.  It is assumed that scrollbars would
// not be needrf and should not be needed if the calculation is
// done correctly, which hopefully it is.
if (!lpDisplay->FileRate)
    {
    iWidth = w; iHeight = h;
    DispRate = ScaleToFit(&iWidth, &iHeight, RectWidth(&lpDisplay->FileRect), RectHeight(&lpDisplay->FileRect));
    lpDisplay->FileRate = FGET(FUNITY, DispRate);
    }
if (lpDisplay->FileRate == TINYRATE) // Iconic
    {
    iWidth = w;
    iHeight = h;

    iImageWidth = lpImage->npix;
    iImageHeight = lpImage->nlin;
    DispRate = ScaleToFit(&iWidth, &iHeight, iImageWidth, iImageHeight);
	//---------------------------------------------------------------------
	// NOTE!! added bInPlaceFrameType here because Insitu needs to be able 
	// to scale over 100%.
	//---------------------------------------------------------------------
	if (!bInPlaceFrameType && (DispRate > FGET(1, 1))) 
        {
        DispRate = FGET(1, 1);
        iWidth = FMUL(iImageWidth, DispRate);
        iHeight = FMUL(iImageHeight, DispRate);
        }
    }
else // if (lpDisplay->FileRate) // Specific FileRate
    {
    // Calulate how much space is needed at this FileRate
    DispRate = FGET(FUNITY, lpDisplay->FileRate);
    iImageWidth = FMUL(lpImage->npix, DispRate);
    iImageHeight = FMUL(lpImage->nlin, DispRate);

    // See if scrollbars will be necessary and get the
    // appropriate DispRect if so
    if (iImageWidth > w && iImageHeight > h)
        {
        GetDispRect(hWnd, &DispRect, 2, 2);
        }
    else
    if (iImageWidth > w)
        {
        GetDispRect(hWnd, &DispRect, 2, 1);
        h = RectHeight(&DispRect);
        if (iImageHeight > h)
            {
            GetDispRect(hWnd, &DispRect, 2, 2);
            }
        }
    else
    if (iImageHeight > h)
        {
        GetDispRect(hWnd, &DispRect, 1, 2);
        w = RectWidth(&DispRect);
        if (iImageWidth > w)
            {
            GetDispRect(hWnd, &DispRect, 2, 2);
            }
        }
    w = RectWidth(&DispRect);
    h = RectHeight(&DispRect);
    iWidth = w;
    iHeight = h;

    // Clip the DispRect size to the amount needed for image
    if (iImageWidth < iWidth)
        iWidth = iImageWidth;
    if (iImageHeight < iHeight)
        iHeight = iImageHeight;
    }

bIconic = IsIconic(hMDIWindow);
bZoomed = IsZoomed(hMDIWindow);
if (bZoomed || bIconic)
    {
    dy = RectHeight(&DispRect) - iHeight;
    dy /= 2;
    dx = RectWidth(&DispRect) - iWidth;
    dx /= 2;
    lpDisplay->DispRect.top = DispRect.top + dy;
    lpDisplay->DispRect.left = DispRect.left + dx;
    }
else
    {
    lpDisplay->DispRect.top = DispRect.top;
    lpDisplay->DispRect.left = DispRect.left;
    }
lpDisplay->DispRect.bottom = lpDisplay->DispRect.top + iHeight - 1;
lpDisplay->DispRect.right = lpDisplay->DispRect.left + iWidth - 1;
}


/*************************************************************************/
static void GetDispRect( HWND hWnd, LPRECT lpRect, int iHorzMode, int iVertMode )
/************************************************************************/
// int iHorzMode; /* 0 = GetClientRect 1 = No Scrollbars 2 = With Scrollbars */
// int iVertMode; /* 0 = GetClientRect 1 = No Scrollbars 2 = With Scrollbars */
{
RECT rHorz, rVert;
int cy, cx;
LPDISPLAY lpDisplay;

GetScrollBarSize(&cx, &cy);
if (iHorzMode == 2)         // with horizontal scrollbar
    {
    GetRealClientRect(hWnd, &rHorz);
    rHorz.bottom -= cy;
    }
else
if (iHorzMode == 1)     // without horizontal scrollbar
    {
    GetRealClientRect(hWnd, &rHorz);
    }
else
    {           // actual client size
    GetClientRect(hWnd, &rHorz);
    }
if (iVertMode == 2)     // with vertical scrollbar
    {
    GetRealClientRect(hWnd, &rVert);
    rVert.right -= cx;
    }
else
if (iVertMode == 1) // without horizontal scrollbar
    {
    GetRealClientRect(hWnd, &rVert);
    }
else                // actual client size
    {
    GetClientRect(hWnd, &rVert);
    }

lpRect->top = rHorz.top;
lpRect->bottom = rHorz.bottom;
lpRect->left = rVert.left;
lpRect->right = rVert.right;


lpDisplay = GetDisplayPtr(hWnd);
if (lpDisplay->HasRulers && !IsIconic(hWnd)) 
    {
    lpRect->top += GetRulerSize();
    lpRect->left += GetRulerSize();
    }

// adjust because our rects include right and bottom
lpRect->bottom -= 1;
lpRect->right -= 1;
}

/************************************************************************/
void ComputeFileRect( HWND hWnd, int x, int y, LFIXED FileRate )
/************************************************************************/
{
int fx, fy, DispWidth, DispHeight, dx, dy;
LFIXED DispRate;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

// A NULL FileRate is a signal that FileRect was just initialized
// and doesn't need recalculating
if ( !FileRate)
    return;

lpImage = GetImagePtr(hWnd);
lpDisplay = GetDisplayPtr(hWnd);

// FileRect will be recomputed from FileRate and a center point (x,y)
DispWidth = RectWidth( &lpDisplay->DispRect );
DispHeight = RectHeight( &lpDisplay->DispRect );

DispRate = FGET(FUNITY, lpDisplay->FileRate);
dx = FMUL(lpImage->npix, DispRate);
dy = FMUL(lpImage->nlin, DispRate);
// FileRate == 1 is a special case for handling iconic state
// otherwise handle normally
if ( FileRate == TINYRATE) // Display all of image - iconic state
    {
    fx = lpImage->npix;
    fy = lpImage->nlin;
    lpDisplay->ViewPercentage = 0;
    }
else 
    {
    // if image fits or is smaller than display surface
    // use maximum FileRect, otherwise use current FileRate
    if (dx <= DispWidth)
        fx = lpImage->npix;
    else
        fx = FMUL( DispWidth, FileRate );
    if (dy <= DispHeight)
        fy = lpImage->nlin;
    else
        fy = FMUL( DispHeight, FileRate );
    }

if (x == -1 && y == -1) // special signal to use old XY
    {
    x = lpDisplay->FileRect.left;
    y = lpDisplay->FileRect.top;
    }
else
    {
    x -= ( fx / 2 );
    y -= ( fy / 2 );
    }
if ( x < 0 ) x = 0;
if ( y < 0 ) y = 0;
if ( x + fx - 1 >= lpImage->npix )  x = lpImage->npix - fx;
if ( y + fy - 1 >= lpImage->nlin )  y = lpImage->nlin - fy;
lpDisplay->FileRect.left   = x;
lpDisplay->FileRect.top = y;
lpDisplay->FileRect.right  = x + fx - 1;
lpDisplay->FileRect.bottom = y + fy - 1;
BoundRect( &lpDisplay->FileRect, 0, 0, lpImage->npix-1, lpImage->nlin-1 );
}

/************************************************************************/
BOOL ScrollImage( HWND hWnd, BOOL fVScroll, UINT code, int pos )
/************************************************************************/
{
int fy, dy, fx, dx;
RECT rScroll;
BOOL bScrollWindow;
#define SCROLLPIXELS 10
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

lpImage = GetImagePtr(hWnd);
lpDisplay = GetDisplayPtr(hWnd);

if (IsIconic(hWnd))
    return( NO );

bScrollWindow = NO;
rScroll = lpDisplay->DispRect;
rScroll.bottom++; rScroll.right++;
if (fVScroll &&
    ( lpImage->nlin - RectHeight( &lpDisplay->FileRect ) > 0 ) )
    {
    switch ( code )
        {
        case SB_ENDSCROLL:
        ScrollEnd( hWnd );
        break;

        case SB_TOP:
        case SB_BOTTOM:
        MessageBeep(0);
        break;

        case SB_PAGEUP:
        case SB_PAGEDOWN:
        case SB_LINEUP:
        case SB_LINEDOWN:
        fy = SCROLLPIXELS;
        if ( code == SB_PAGEUP || code == SB_PAGEDOWN )
            fy *= 10;
        if ( !(fy = FMUL( fy, lpDisplay->FileRate )) )
            fy = 1;
        if ( code == SB_LINEDOWN || code == SB_PAGEDOWN )
            {
            fy = -fy;
            dy = lpDisplay->FileRect.bottom - lpImage->nlin + 1;
            }
        else
        // ( code == SB_LINEUP || code == SB_PAGEUP )
            dy = lpDisplay->FileRect.top;
        if ( !dy )
            break;
        if ( abs(fy) > abs(dy) )
            fy = dy;
        if ( !(dy = FMUL( fy, lpDisplay->DispRate )) )
            break;
        ScrollStart( hWnd );
        lpDisplay->FileRect.top -= fy;
        lpDisplay->FileRect.bottom -= fy;
        lpDisplay->yDiva -= dy;
        ScrollWindow( hWnd, 0 /*x*/, dy, &rScroll, &rScroll );
        AstralUpdateWindow(hWnd);
        bScrollWindow = YES;
        break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
        if ( !(fy = GetDisplayScrollDelta( hWnd, lpDisplay, SB_VERT, pos) ) )
            break;
        if ( !(dy = FMUL( fy, lpDisplay->DispRate )) )
            break;
        ScrollStart( hWnd );
        lpDisplay->FileRect.top -= fy;
        lpDisplay->FileRect.bottom -= fy;
        lpDisplay->yDiva -= dy;
        ScrollWindow( hWnd, 0 /*x*/, dy, &rScroll, &rScroll );
        AstralUpdateWindow(hWnd);
        bScrollWindow = YES;
        break;
        }
    }

if (!fVScroll &&
    ( lpImage->npix - RectWidth( &lpDisplay->FileRect ) > 0 ) )
    {
    switch ( code )
        {
        case SB_ENDSCROLL:
        ScrollEnd( hWnd );
        break;

        case SB_TOP:
        case SB_BOTTOM:
        MessageBeep(0);
        break;

        case SB_PAGEUP:
        case SB_PAGEDOWN:
        case SB_LINEUP:
        case SB_LINEDOWN:
        fx = SCROLLPIXELS;
        if ( code == SB_PAGEUP || code == SB_PAGEDOWN )
            fx *= 10;
        if ( !(fx = FMUL( fx, lpDisplay->FileRate )) )
            fx = 1;
        if ( code == SB_LINEDOWN || code == SB_PAGEDOWN )
            {
            fx = -fx;
            dx = lpDisplay->FileRect.right - lpImage->npix + 1;
            }
        else
        // ( code == SB_LINEUP || code == SB_PAGEUP )
            dx = lpDisplay->FileRect.left;
        if ( !dx )
            break;
        if ( abs(fx) > abs(dx) )
            fx = dx;
        if ( !(dx = FMUL( fx, lpDisplay->DispRate )) )
            break;
        ScrollStart( hWnd );
        lpDisplay->FileRect.left -= fx;
        lpDisplay->FileRect.right -= fx;
        lpDisplay->xDiva -= dx;
        ScrollWindow( hWnd, dx, 0 /*y*/, &rScroll, &rScroll );
        AstralUpdateWindow(hWnd);
        bScrollWindow = YES;
        break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
        if ( !(fx = GetDisplayScrollDelta( hWnd, lpDisplay, SB_HORZ, pos )) )
            break;
        if ( !(dx = FMUL( fx, lpDisplay->DispRate )) )
            break;
        ScrollStart( hWnd );
        lpDisplay->FileRect.left -= fx;
        lpDisplay->FileRect.right -= fx;
        lpDisplay->xDiva -= dx;
        ScrollWindow( hWnd, dx, 0 /*y*/, &rScroll, &rScroll );
        AstralUpdateWindow(hWnd);
        bScrollWindow = YES;
        break;
        }
    }

return( bScrollWindow );
}

static BOOL bScrolling;
static CHookHandler *lpProc;
/************************************************************************/
static void ScrollStart( HWND hWnd )
/************************************************************************/
{
if ( bScrolling )
    return;
// Rely on the fact that we always get an SB_ENDSCROLL when done
if ( hWnd == GetActiveDoc() )
    DisplayBrush(0, 0, 0, OFF);
if ( lpProc = GetDisplayHook(hWnd) )
    { // Call the display hook and turn it off
    lpProc->DisplayHookProc( hWnd, Window.hDC, NULL );
    SetDisplayHook( hWnd, NULL );
    }
EnableMarquee( NO );
RemoveMarquee();
AstralUpdateWindow( hWnd );
bScrolling = YES;
}


/************************************************************************/
static void ScrollEnd( HWND hWnd )
/************************************************************************/
{
if ( !bScrolling )
    return;
if ( lpProc )
    { // Call the display hook and turn it back on
    lpProc->DisplayHookProc( hWnd, Window.hDC, NULL );
    SetDisplayHook( hWnd, lpProc );
    lpProc = NULL;
    }
EnableMarquee( YES );
SetupRulers(hWnd);
ZoomMainViewChanged( hWnd );
bScrolling = NO;
}


/************************************************************************/
void SaveLastView(HWND hWnd)
/************************************************************************/
{
LPDISPLAY lpDisplay;

lpDisplay = GetDisplayPtr(hWnd);
lpDisplay->LastWindowRect = lpDisplay->WindowRect;
lpDisplay->LastFileRect = lpDisplay->FileRect;
lpDisplay->LastFileRate = lpDisplay->FileRate;
lpDisplay->LastViewPercentage = lpDisplay->ViewPercentage;
}

/************************************************************************/
void RevertLastView(HWND hWnd)
/************************************************************************/
{
int iWidth, iHeight, i;
RECT FileRect, WindowRect,r;
LFIXED FileRate;
int ViewPercentage;
POINT UpperLeft;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;
HWND hMDIWindow = hWnd;
HWND  hClient;

lpImage = GetImagePtr(hWnd);
lpDisplay = GetDisplayPtr(hWnd);

CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

if (IsIconic(hMDIWindow))
    return;

FileRect = lpDisplay->FileRect;
lpDisplay->FileRect = lpDisplay->LastFileRect;
lpDisplay->LastFileRect = FileRect;
FileRate = lpDisplay->FileRate;
lpDisplay->FileRate = lpDisplay->LastFileRate;
lpDisplay->LastFileRate = FileRate;
ViewPercentage = lpDisplay->ViewPercentage;
lpDisplay->ViewPercentage = lpDisplay->LastViewPercentage;
lpDisplay->LastViewPercentage = ViewPercentage;
ComputeDispRect(hWnd);
ComputeFileRect(hWnd, -1, -1, lpDisplay->FileRate);
GetWindowRect(hWnd, &WindowRect);
hClient = GetParent(hMDIWindow);
ScreenToClient(hClient, (LPPOINT)&WindowRect);
ScreenToClient(hClient, (LPPOINT)&WindowRect.right);
if (!IsZoomed(hMDIWindow) && !EqualRect(&lpDisplay->LastWindowRect, &WindowRect))
    {
    GetWindowRect(hMDIWindow, &r);
    ScreenToClient(hClient, (LPPOINT)&r);
    ScreenToClient(hClient, (LPPOINT)&r.right);
    iWidth   = RectWidth(&lpDisplay->LastWindowRect)-1;
    iWidth  += r.right - WindowRect.right;
    iHeight  = RectHeight(&lpDisplay->LastWindowRect)-1;
    iHeight += r.bottom - WindowRect.bottom;
    UpperLeft.x  = lpDisplay->LastWindowRect.left;
    i            = (WindowRect.left - r.left);
    UpperLeft.x -= i;
    iWidth      += i;
    UpperLeft.y  = lpDisplay->LastWindowRect.top;
    i            = (WindowRect.top - r.top);
    UpperLeft.y -= i;
    iHeight     += i;
    MoveWindow(hMDIWindow,UpperLeft.x, UpperLeft.y, iWidth, iHeight, TRUE);

    lpDisplay->LastWindowRect = WindowRect;
    }
else
    {
    SetupRulers(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
    }
ZoomMainViewChanged(hWnd);
}


#ifndef ID_MAGIC
/************************************************************************/
BOOL ZoomEnable( HWND hWnd, int value, BOOL fIncremental)
/************************************************************************/
{
LFIXED FileRate;
int Percentage;

HWND hMDIWindow = hWnd;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

if (IsIconic(hMDIWindow))
    return(FALSE);
		 
return(ComputeZoom(hWnd,0,0,value,fIncremental, &FileRate, &Percentage));
}

/************************************************************************/
void Zoom( HWND hWnd, int x, int y, int value, BOOL fIncremental,
            BOOL fAllowSizeChange )
/************************************************************************/
{
LFIXED FileRate;
int Percentage, left, top;
LPDISPLAY lpDisplay;

HWND hMDIWindow = hWnd;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

if (IsIconic(hMDIWindow))
    return;

if (!ComputeZoom(hWnd,x,y,value,fIncremental, &FileRate, &Percentage))
    {
    Message( IDS_BADZOOM );
    return;
    }

SaveLastView(hWnd);
lpDisplay = GetDisplayPtr(hWnd);
lpDisplay->ViewPercentage = Percentage;
lpDisplay->FileRate = FileRate;
if (fAllowSizeChange)
    {
	 GetWinXY(hMDIWindow, &left, &top);
    FullViewWindow(hWnd, left, top, x, y, YES);
    }
else
    {
    ComputeDispRect(hWnd);
    // Change the FileRect to accomodate
    ComputeFileRect( hWnd, x, y, FileRate);
    SetupRulers(hWnd);
    InvalidateRect( hWnd, NULL, TRUE );
    }
ZoomMainViewChanged(hWnd);      // update the 'zoom window'
}


/***********************************************************************
    ComputeZoom
        Computes Percentage and FileRate given the same inputs as Zoom
        Returns:
                TRUE if the reusult is valid.
                FALSE if the result would be illegal (Percent & FileRate are NOT valid) 
************************************************************************/
/************************************************************************/
static int ComputeZoom( HWND hWnd, int x, int y, int value,
                        BOOL fIncremental, LFIXED *lfFileRate,
                        int *Percentage )
/************************************************************************/
{
LFIXED DispRate;
int OldPercentage, iImageWidth, iImageHeight;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

HWND hMDIWindow = hWnd;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

if (IsIconic(hMDIWindow))
    return(FALSE);

lpImage = GetImagePtr(hWnd);
lpDisplay = GetDisplayPtr(hWnd);

// Compute the viewing percentage, if necessary
if ( !(*Percentage = lpDisplay->ViewPercentage) )
    *Percentage = FMUL( 100, lpDisplay->DispRate );

// Compute the NEW viewing percentage, and DispRate
if ( fIncremental )
    {
    if (value < 0)
        {
        if (*Percentage > 100)
            {
            if (*Percentage % 100 != 0)
                *Percentage -= *Percentage % 100;
            else
                *Percentage -= 100;
            }
        else
        if (*Percentage <= 1)
            *Percentage = 0;
        else
            {
            OldPercentage = *Percentage;
            *Percentage = 100;
            while (*Percentage >= OldPercentage)
                *Percentage = (*Percentage+1)/2;
            }
        }
    else
    if (value > 0)
        {
        if (*Percentage > 32600)
            *Percentage = 0;
        else
        if (*Percentage >= 50)
            *Percentage = ((*Percentage+100)/100) * 100;
        else
        if (*Percentage)
            {
            OldPercentage = (*Percentage * 2) + 1;
            *Percentage = 100;
            while (*Percentage > OldPercentage)
                *Percentage = (*Percentage+1)/2;
            }
        else
            *Percentage = 1;
        }
    }
else
if (value > 0 && value < 32700)
    *Percentage  = value;
else
    *Percentage = 0;

// do we have a valid zoom percentage?
if (!*Percentage)
    return(FALSE);

// can we mathematically support this percentage?
DispRate = FGET( *Percentage, 100 );  /* DispRate */
*lfFileRate = FGET( 100, *Percentage );  /* FileRate */
if (!CanZoom(hWnd, *lfFileRate, &lpDisplay->FileRect,
        &lpDisplay->DispRect))
    {
    return(FALSE);
    }

// is our display surface too small?
iImageWidth = FMUL(lpImage->npix, DispRate);
iImageHeight = FMUL(lpImage->nlin, DispRate);
if (iImageHeight < 1 || iImageWidth < 1)
    {
    return(FALSE);
    }
return(TRUE);
}


void View1TO1()
   {
   int x, y, res, ratio;
   LFIXED fixed;
   HDC hDC;
   HWND hWnd;
   LPDISPLAY lpDisplay;
   LPIMAGE lpImage;

	if (hWnd = GetActiveDoc())
      {
	   lpImage = GetImagePtr(hWnd);
	   lpDisplay = GetDisplayPtr(hWnd);
	   hDC = GetDC( PictPubApp.Get_hWndAstral() );
	   x = GetDeviceCaps( hDC, HORZRES ); // in pixels
	   ReleaseDC( PictPubApp.Get_hWndAstral(), hDC );
	   fixed = FGET( FUNITY, Control.ScreenWidth );
	   if (res = FMUL( x, fixed)) // in pixels/inch
         {
	      ratio = ( 100L * res ) / FrameResolution(ImgGetBaseEditFrame(lpImage));
	      x = ( lpDisplay->FileRect.left 
		   + lpDisplay->FileRect.right ) / 2;
	      y = ( lpDisplay->FileRect.top 
		   + lpDisplay->FileRect.bottom ) / 2;
	      Zoom( hWnd, x, y, ratio, NO, View.ZoomWindow );
         }
      }
   }

/************************************************************************/
void ViewAll(HWND hWnd)
/************************************************************************/
{
int x, y, width, height, rs, left, top;
RECT rClient;
LPIMAGE lpImage;
LPDISPLAY lpDisplay;

HWND hMDIWindow = hWnd;
CWnd *pWnd = CWnd::FromHandle(hMDIWindow);
if (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)))        
	hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();

if (IsIconic(hMDIWindow))
    return;

lpImage = GetImagePtr(hWnd);
lpDisplay = GetDisplayPtr(hWnd);

GetClientRect(hWnd, &rClient);
width = rClient.right - rClient.left;
height = rClient.bottom - rClient.top;
if (lpDisplay->HasRulers)
    {
    rs = GetRulerSize();
    width -= rs;
    height -= rs;
    }
if ((width <= 0 || height <= 0) && !View.ZoomWindow)
    return;

SaveLastView(hWnd);
lpDisplay->FileRect.top = 0;
lpDisplay->FileRect.left = 0;
lpDisplay->FileRect.bottom = lpImage->nlin - 1;
lpDisplay->FileRect.right = lpImage->npix - 1;
lpDisplay->FileRate = TINYRATE;
lpDisplay->ViewPercentage = 0;
x = ( lpDisplay->FileRect.left 
    + lpDisplay->FileRect.right+1 ) / 2;
y = ( lpDisplay->FileRect.top 
    + lpDisplay->FileRect.bottom+1 ) / 2;
if (View.ZoomWindow)
    {
	GetWinXY(hMDIWindow, &left, &top);
    FullViewWindow(hWnd, left, top, x, y, YES);
    }
else
    {
    ComputeDispRect(hWnd);
    ComputeFileRect(hWnd, x, y, lpDisplay->FileRate);
    SetupRulers(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
    }
}

// LOCAL prototypes
LOCAL BOOL FullScreenView_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void FullScreenView_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL BOOL FullScreenView_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void FullScreenView_OnPaint(HWND hWindow);
LOCAL void FullScreenView_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);

/***********************************************************************/
BOOL WINPROC EXPORT DlgFullScreenViewProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
    {
    HANDLE_DLGRET(hDlg, WM_INITDIALOG, FullScreenView_OnInitDialog);  
    HANDLE_DLGMSG(hDlg, WM_COMMAND, FullScreenView_OnCommand);
    HANDLE_DLGRET(hDlg, WM_ERASEBKGND, FullScreenView_OnEraseBkgnd);
    HANDLE_DLGMSG(hDlg, WM_PAINT, FullScreenView_OnPaint);
    HANDLE_DLGMSG(hDlg, WM_KEYDOWN, FullScreenView_OnKeyDown);

    HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
    HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Dialog_OnPaletteChanged);

    default:
    return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_VIEWFULL);
    }
}

/***********************************************************************/
LOCAL BOOL FullScreenView_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
return(TRUE);
}

/***********************************************************************/
LOCAL void FullScreenView_OnPaint(HWND hDlg)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect;
HDC hDC;

hDC = BeginPaint( hDlg, &ps );
//  if ( ps.fErase )
    { // Just draw the background
    ClientRect = ps.rcPaint;
    DrawColorPatch( hDC, &ClientRect, GetActiveRGB(NULL), YES );
    }
EndPaint( hDlg, &ps );
}

/***********************************************************************/
LOCAL void FullScreenView_OnKeyDown(HWND hDlg, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
AstralDlgEnd( hDlg, FALSE );
}

/************************************************************************/
LOCAL void FullScreenView_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
AstralDlgEnd( hDlg, FALSE );
}

#endif // #ifndef ID_MAGIC



LOCAL BOOL FullScreenView_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
{
	CPPMDIChildWnd *pMDIChild;
	HWND hMDIWnd;
	RECT ClientRect;
	int npix, nlin, cx, cy, dx, dy;
	HWND hControl, hMDIChild, hView;
	LPOBJECT lpBase;
	LPFRAME lpFrame;

	// Make the window as big as the screen
	dx = GetSystemMetrics( SM_CXSCREEN );
	dy = GetSystemMetrics( SM_CYSCREEN );
	MoveWindow( hDlg, 0, 0, dx, dy, NO );

	// Compute the center of the window
	GetClientRect( hDlg, &ClientRect );
	cx = ( ClientRect.right + ClientRect.left ) / 2;
	cy = ( ClientRect.bottom + ClientRect.top ) / 2;

	// Link all of the images to image controls
	hControl = GetDlgItem( hDlg, IDC_VIEWFULL );

	CServerView *pView = PictPubApp.GetActiveView();
	BOOL bInplaceActive = (pView && pView->GetDocument()->IsInPlaceActive());
	if (bInplaceActive)
	{
		hMDIChild = pView->GetParentFrame()->GetSafeHwnd();
		hView = pView->GetSafeHwnd();
	}
	else
	{
		pMDIChild = (CPPMDIChildWnd*)((CMDIFrame*)PictPubApp.m_pMainWnd)->MDIGetActive();
		hMDIWnd = pMDIChild->GetSafeHwnd();
		if (hMDIWnd)
			hView = pMDIChild->GetActiveView()->GetSafeHwnd();
		else
			hView = NULL;
	}

	while ( hView )
 	{
		lpBase = ImgGetBase((LPIMAGE)GetImagePtr(hView));
		if ( !lpBase || !hControl )
			break;
		lpFrame = ObjGetEditFrame(lpBase);

		// Link the frame to the image control
		SetWindowLong( hControl, GWL_IMAGE, (long)lpBase );
			
		// The destination size can be no bigger the the screen
		npix = FrameXSize(lpFrame);
		nlin = FrameYSize(lpFrame);
		if ( npix > dx || nlin > dy )
 		{
			npix = dx;
			nlin = dy;
			ScaleToFit( &npix, &nlin,
				FrameXSize(lpFrame), FrameYSize(lpFrame));
 		}

		// Position the control in the center of the window
		ClientRect.left = cx - npix/2;
		ClientRect.right = ClientRect.left + npix;
		ClientRect.top = cy - nlin/2;
		ClientRect.bottom = ClientRect.top + nlin;
		MoveWindow( hControl,
			ClientRect.left, ClientRect.top, // New position
			RectWidth( &ClientRect ),
			RectHeight( &ClientRect ), // New size
			NO /* No repaint*/ );

		if (bInplaceActive)
			break;

		// Check to see if these get destroyed when the window does
		do	
		{
			hView = NULL;
			if (hMDIWnd = GetWindow( hMDIWnd, GW_HWNDNEXT ))
			{
				if ((CWnd::FromHandle(hMDIWnd))->
					IsKindOf(RUNTIME_CLASS(CPPMDIChildWnd)))
					hView = ((CPPMDIChildWnd*)CWnd::FromHandle(hMDIWnd))->
						GetActiveView()->GetSafeHwnd();
			}

		} while (hMDIWnd && !hView);

		if (!hView)
			break;

		// Get another image control				 
		hControl = CopyWindow( hControl );
	}

	return TRUE;
}

CZoomForum		ZoomForum;

CZoomForum::CZoomForum()
{
	m_iPercentage = 0;
}

void CZoomForum::SetViewPercentage(int iPercentage, CAttendee* pSkip)
{
	m_iPercentage = iPercentage;
	MakeRemark (pSkip);
}

#define NUM_PERCENTAGES 8
#define NUM_COMMANDS	5
#define NUM_ITEMS (NUM_PERCENTAGES+NUM_COMMANDS)
void CZoomForum::FillCombo(HWND hWnd)
{
	STRING szContents[NUM_ITEMS] = {
							"1600%", "800%", "400%", "200%", "100%", "75%",
							"50%", "25%", "Area Zoom", "Previous View",
							"View 1:1", "Entire Image", "Full Screen" };
							
   	SendMessage(hWnd, CB_RESETCONTENT, 0, 0L );
	for (int i=0; i < NUM_ITEMS; i++)
		ComboBox_AddString(hWnd, szContents[i] );
}

CZoomCombo		ZoomCombo;

CZoomCombo::CZoomCombo()
{
	m_hWnd = NULL;
}

CZoomCombo::~CZoomCombo()
{
}

void CZoomCombo::NotifyChanged(int code)
{
	int		nIndex;
	int 	iPercentages[NUM_PERCENTAGES] = {1600, 800, 400, 200, 100, 75, 50, 25};
	int		idCommands[NUM_COMMANDS] = {IDC_CUSTOMVIEW, IDC_VIEWLAST, IDC_VIEW1TO1, IDC_VIEWALL, IDC_VIEWFULL};
	int		iPercentage = 0;

	CServerView *pView = PictPubApp.GetActiveView();
	if (pView == NULL)
		return;

	if (code == CBN_KILLFOCUS)
	{
		BOOL 	bTranslated;
		iPercentage = GetDlgItemInt(GetParent(m_hWnd), GetDlgCtrlID(m_hWnd), &bTranslated, FALSE);
		if (!iPercentage)
			return;
	}
	else
	{
		if ( code != CBN_SELCHANGE )
			return;
		nIndex = ComboBox_GetCurSel(m_hWnd);
	}
	if (iPercentage || nIndex < NUM_PERCENTAGES)
	{
		if (!iPercentage)
			iPercentage = iPercentages[nIndex];
		if (iPercentage != ZoomForum.GetViewPercentage())
		{
			if (!ZoomEnable( pView->GetSafeHwnd(), iPercentage, FALSE))
			{
			    Message( IDS_BADZOOM );
				SetFocus(m_hWnd);
				return;
			}
    		int x = ( pView->m_lpDisplay->FileRect.left 
            		+ pView->m_lpDisplay->FileRect.right ) / 2;
    		int y = ( pView->m_lpDisplay->FileRect.top 
            		+ pView->m_lpDisplay->FileRect.bottom ) / 2;
    		SaveLastView(pView->GetSafeHwnd());
    		Zoom(pView->GetSafeHwnd(), x, y, iPercentage, NO, CONTROL );
		}
	}
	else
	{
		nIndex -= NUM_PERCENTAGES;
		int idCommand = idCommands[nIndex];
		HWND hWnd = PictPubApp.m_pMainWnd->GetSafeHwnd();
		FORWARD_WM_COMMAND(hWnd, idCommand, m_hWnd, 0, SendMessage);
	}
}							  

void CZoomCombo::Init(HWND hWnd)
{
	if (hWnd)
	{
		ArriveAt(ZoomForum);
		m_hWnd = hWnd;
		ZoomForum.FillCombo(hWnd);
		Assimilate(&ZoomForum);
	}
	else
	{
		// only depart from forum if we ever joined forum
		if (m_hWnd)
		{
			m_hWnd = NULL;
			DepartFrom(ZoomForum);
		}
	}
}

void CZoomCombo::Assimilate (CForum * pForum)
{
	if (pForum == &ZoomForum)
	{
		STRING szPercentage;
		wsprintf(szPercentage, "%d%%", ZoomForum.GetViewPercentage());
   		ComboBox_SetText(m_hWnd, szPercentage);
	}
}






