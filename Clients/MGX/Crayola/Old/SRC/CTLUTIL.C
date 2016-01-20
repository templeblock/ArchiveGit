// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"

static HBRUSH Ctl_hShadowBrush;
static HBRUSH Ctl_hButtonBrush;
static HBRUSH Ctl_hHilightBrush;
static HPEN Ctl_hShadowPen;
static HPEN Ctl_hButtonPen;
static LPBLT Ctl_lpBltScreen;
static int iLabelFont;

/************************************************************************/
void Ctl_SetBltScreen(LPBLT lpBltScreen)
/************************************************************************/
{
Ctl_lpBltScreen = lpBltScreen;
}

/************************************************************************/
LPBLT Ctl_GetBltScreen()
/************************************************************************/
{
return(Ctl_lpBltScreen);
}

/************************************************************************/
void Ctl_SetLabelFont(int iFont)
/************************************************************************/
{
iLabelFont = iFont;
}

/************************************************************************/
int Ctl_GetLabelFont()
/************************************************************************/
{
return(iLabelFont);
}

/************************************************************************/
BOOL Ctl_CreateBrushes( )
/************************************************************************/
{
WORD Pattern[16];
HBITMAP hBitmap;
int i;

Ctl_hShadowBrush = CreateSolidBrush( Ctl_GetBtnShadowColor() );
Ctl_hButtonBrush = CreateSolidBrush( Ctl_GetBtnFaceColor() );
for ( i=0; i<16; i+=2 )
	Pattern[i] = 0xAAAA;
for ( i=1; i<16; i+=2 )
	Pattern[i] = 0x5555;
hBitmap = CreateBitmap( 16, 16, 1, 1, (LPTR)Pattern );
Ctl_hHilightBrush = CreatePatternBrush( hBitmap );
DeleteObject( hBitmap );
Ctl_hShadowPen = CreatePen( PS_SOLID, 1, Ctl_GetBtnShadowColor());
Ctl_hButtonPen = CreatePen( PS_SOLID, 1, Ctl_GetBtnFaceColor());
return(TRUE);
}

/************************************************************************/
void Ctl_DestroyBrushes( )
/************************************************************************/
{
DeleteObject(Ctl_hShadowBrush);
DeleteObject(Ctl_hButtonBrush);
DeleteObject(Ctl_hHilightBrush);
DeleteObject(Ctl_hShadowPen);
DeleteObject(Ctl_hButtonPen);
}

/************************************************************************/
HBRUSH Ctl_GetBtnShadowBrush()
/************************************************************************/
{
return(Ctl_hShadowBrush);
}

/************************************************************************/
HBRUSH Ctl_GetBtnFaceBrush()
/************************************************************************/
{
return(Ctl_hButtonBrush);
}

/************************************************************************/
HBRUSH Ctl_GetBtnHilightBrush()
/************************************************************************/
{
return(Ctl_hHilightBrush);
}

/************************************************************************/
HPEN Ctl_GetBtnShadowPen()
/************************************************************************/
{
return(Ctl_hShadowPen);
}

/************************************************************************/
HPEN Ctl_GetBtnFacePen()
/************************************************************************/
{
return(Ctl_hButtonPen);
}

/************************************************************************/
LPTR Ctl_Alloc( long lCount )
/************************************************************************/
{
return( Ctl_AllocX( lCount, 0 ) );
}

/************************************************************************/
LPTR Ctl_AllocX( long lCount, WORD wFlags )
/************************************************************************/
{
HGLOBAL  hMemory;
LPTR     lpMemory;

// Round up to the next 16 byte boundary
lCount++; // Protection against a possible C7.0 bug
lCount = ((lCount + 15) / 16) * 16;

// Allocate the memory from the global heap - NOT limited to 64K
if ( !(hMemory = GlobalAlloc(GMEM_MOVEABLE | wFlags, lCount )) )
	return( NULL );

if ( !( lpMemory = ( LPTR )GlobalLock( hMemory ) ) )
	GlobalFree( hMemory );

return( lpMemory );
}

/************************************************************************/
void Ctl_FreeUp( LPVOID lpMemory )
/************************************************************************/
{
HGLOBAL  hMemory;

if ( !lpMemory )
	return;

#ifdef WIN32
if (!(hMemory = GlobalHandle(lpMemory)))
	return;
#else
{
long lValue;

lValue = GlobalHandle( HIWORD(lpMemory) );
if ( HIWORD(lValue) != HIWORD(lpMemory) ) /* segment portions should match */
	return;
hMemory = (HGLOBAL)LOWORD( lValue );
}
#endif
GlobalUnlock( hMemory );
GlobalFree( hMemory );
}


/************************************************************************/
void Ctl_DrawBevel (
/************************************************************************/
HDC hDC,         // Handle to DC in which to draw bevel
COLORREF rgbDark,      // Dark Color
COLORREF rgbLight,     // Light Color
RECT rRect,         // Rectangle of outer bounds of the bevel
UINT wThickness,    // Thickness, multiple of windows border thicknes
UINT wType)         // Comb of bevel types: DB_OUT,DB_IN,DB_CIRCLE,DB_RECT
                    // DB_DIAMOND.
/*
    This function draws a bevel on the given DC with the light and
    dark pens within the given rectangle. The thickness of the bevel
    must be specified as well as the type of bevel to draw. The wType 
    can be any one of the following ored together or NULL (default):
    DB_OUT | DB_IN (default) and DB_CIRCLE | DB_RECT (default).
*/
{
    int i;
    HPEN hDarkPen;
    HPEN hLightPen;
    HPEN hOldPen;
	HPEN t;
    
    if (wType & DB_CIRCLE)
    {   

        hDarkPen = CreatePen (PS_SOLID,1,rgbDark);
        hLightPen = CreatePen (PS_SOLID,1,rgbLight);
        hOldPen = (HPEN)SelectObject (hDC,hDarkPen);

        if (wType & DB_OUT)
		 	{
			t = hDarkPen;
			hDarkPen = hLightPen;
			hLightPen = t;
			 }
        for (i=0;i < (int)wThickness;i++)
        {
            SelectObject (hDC,hDarkPen);
            Arc (hDC,rRect.left,rRect.top,rRect.right,rRect.bottom,
                rRect.right,rRect.top,rRect.left,rRect.bottom);
            SelectObject (hDC,hLightPen);
            Arc (hDC,rRect.left,rRect.top,rRect.right,rRect.bottom,
                rRect.left,rRect.bottom,rRect.right,rRect.top);
            InflateRect (&rRect,-1,-1);
        }
        SelectObject (hDC,hOldPen);
        DeleteObject (hDarkPen);
        DeleteObject (hLightPen);
    }
    else if (wType & DB_DIAMOND)
    {   
        HPEN hDarkPen;
        HPEN hLightPen;
        HPEN hOldPen;
		 HPEN t;
		 UINT hWidth, hHeight;

        hDarkPen = CreatePen (PS_SOLID,1,rgbDark);
        hLightPen = CreatePen (PS_SOLID,1,rgbLight);
        hOldPen = (HPEN)SelectObject (hDC,hDarkPen);

        hWidth = (rRect.right - rRect.left)/2;
        hHeight = (rRect.bottom - rRect.top)/2;

        if (wType & DB_OUT)
		 	{
			t = hDarkPen;
			hDarkPen = hLightPen;
			hLightPen = t;
			}
        for (i=0;i < (int)wThickness;i++)
        {
            SelectObject (hDC,hLightPen);
            MoveToEx (hDC,rRect.left+hWidth,rRect.top+2*hHeight-i, NULL);
            LineTo (hDC,rRect.left+2*hWidth-i,rRect.top+hHeight);
            LineTo (hDC,rRect.left+hWidth,rRect.top+i);    
            SelectObject (hDC,hDarkPen);
            LineTo (hDC,rRect.left+i,rRect.top+hHeight);
            LineTo (hDC,rRect.left+hWidth,rRect.top+2*hHeight-i);
        }
        SelectObject (hDC,hOldPen);
        DeleteObject (hDarkPen);
        DeleteObject (hLightPen);
    }
    else //(wType & DB_RECT)
    {
        HBRUSH hDarkBrush;
        HBRUSH hLightBrush;
        HBRUSH hOldBrush;
		 HBRUSH t;
		 int cxBorder;
		 int cyBorder;
		 int cx, cy;

        hDarkBrush = CreateSolidBrush (rgbDark);
        hLightBrush = CreateSolidBrush (rgbLight);
        hOldBrush = (HBRUSH)SelectObject (hDC,hDarkBrush);
        cxBorder = GetSystemMetrics (SM_CXBORDER);
        cyBorder = GetSystemMetrics (SM_CYBORDER);

        if (wType & DB_OUT)
		 	{
			t = hDarkBrush;
			hDarkBrush = hLightBrush;
			hLightBrush = t;
			}
        for (i=0;i < (int)wThickness;i++)
        {
            cx = i * cxBorder;
            cy = i * cyBorder;
                
            SelectObject (hDC,hDarkBrush);
            PatBlt (hDC,rRect.left+cx,rRect.top+cy,
                cxBorder,(rRect.bottom-rRect.top)-2*cy,PATCOPY);
            PatBlt (hDC,rRect.left+cx,rRect.top+cy,
                (rRect.right-rRect.left)-2*cx,cyBorder,PATCOPY);
            SelectObject (hDC,hLightBrush);
            PatBlt (hDC,rRect.right-cxBorder-cx,rRect.top+cy,
                cxBorder,(rRect.bottom-rRect.top)-2*cy,PATCOPY);
            PatBlt (hDC,rRect.left+cxBorder+cx,rRect.bottom-cyBorder-cy,
                (rRect.right-rRect.left-cxBorder)-2*cx,cyBorder,PATCOPY);
        }
        SelectObject (hDC,hOldBrush);
        DeleteObject (hDarkBrush);
        DeleteObject (hLightBrush);
    }
}

/***********************************************************************/
void Ctl_DrawBtnBevel (
HDC hDC,            // Handle to DC in which to draw bevel
RECT rRect,         // Rectangle of outer bounds of the bevel
WORD wThickness)    // Thickness, multiple of windows border thicknes
/***********************************************************************/
{
    int i;
    HANDLE hDarkBrush = CreateSolidBrush (GetSysColor(COLOR_BTNSHADOW));
    HANDLE hLightBrush = CreateSolidBrush (RGB(255,255,255));
    HANDLE hOldBrush = SelectObject (hDC,hDarkBrush);
    int cxBorder = GetSystemMetrics (SM_CXBORDER);
    int cyBorder = GetSystemMetrics (SM_CYBORDER);
    int cx,cy;

    for (i=0;i<(int)wThickness;i++)
    {
        cx = i * cxBorder;
        cy = i * cyBorder;
            
        SelectObject (hDC,hLightBrush);
        PatBlt (hDC,rRect.left+cx,rRect.top+cy,
            cxBorder,(rRect.bottom-rRect.top)-2*cy,PATCOPY);
        PatBlt (hDC,rRect.left+cx,rRect.top+cy,
            (rRect.right-rRect.left)-2*cx,cyBorder,PATCOPY);
        SelectObject (hDC,hDarkBrush);
        PatBlt (hDC,rRect.right-cxBorder-cx,rRect.top+cy,
            cxBorder,(rRect.bottom-rRect.top)-2*cy,PATCOPY);
        PatBlt (hDC,rRect.left+cx,rRect.bottom-cyBorder-cy,
            (rRect.right-rRect.left)-2*cx,cyBorder,PATCOPY);
    }
    SelectObject (hDC,hOldBrush);
    DeleteObject (hDarkBrush);
    DeleteObject (hLightBrush);
}

/***********************************************************************/
void Ctl_DrawButton (
HWND hWindow,
HDC hDC,
LPRECT lpRect,
WORD wStyle,
WORD wState)
/***********************************************************************/
{
    int cxBorder = GetSystemMetrics (SM_CXBORDER);
    int cyBorder = GetSystemMetrics (SM_CYBORDER);
    HBRUSH hBrush, hOldBrush;
    RECT rFrame = *lpRect;

    if (!(wStyle & BMS_NOBORDER))
    {
        hBrush = CreateSolidBrush (RGB(0,0,0));
        hOldBrush = (HBRUSH)SelectObject (hDC,hBrush);
        FrameRect (hDC,&rFrame,hBrush);
        if (wStyle & BMS_ROUNDCORNERS)
        {
            HBRUSH hBkgd = Ctl_GetBackgroundBrush( hWindow, hDC, CTLCOLOR_BTN );
        
            SelectObject (hDC,hBkgd);
            PatBlt (hDC,rFrame.left,rFrame.top,cxBorder,cyBorder,PATCOPY);
            PatBlt (hDC,rFrame.left,rFrame.bottom,cxBorder,-cyBorder,PATCOPY);
            PatBlt (hDC,rFrame.right,rFrame.top,-cxBorder,cyBorder,PATCOPY);
            PatBlt (hDC,rFrame.right,rFrame.bottom,-cxBorder,-cyBorder,PATCOPY);
            SelectObject (hDC,hBrush);
        }
        InflateRect (&rFrame,-cxBorder,-cyBorder);
        if (wStyle & BMS_DBLBORDER)
        {
            FrameRect (hDC,&rFrame,hBrush);
            InflateRect (&rFrame,-cxBorder,-cyBorder);
        }
        SelectObject (hDC,hOldBrush);
        DeleteObject (hBrush);
    }
    if (wState == BMS_UP)
    {   
        Ctl_DrawBtnBevel (hDC,
            rFrame,(WORD)(wStyle & BMS_THINBEVEL ? 1 : 2));
        InflateRect (&rFrame,(wStyle & BMS_THINBEVEL ? -1 : -2)*cxBorder,
            (wStyle & BMS_THINBEVEL ? -1 : -2)*cyBorder);
        if (wStyle & BMS_WINDOWSBEVEL && !(wStyle & BMS_THINBEVEL))
        {
            rFrame.left--;
            rFrame.top--;
        }
    }
    else if (wState == BMS_DOWN || wState == BMS_SELECTED)
    {
		 if (wState == BMS_SELECTED)
   	 	hBrush = CreateSolidBrush (RGB(0,0,0));
	 	else
			hBrush = CreateSolidBrush (GetSysColor(COLOR_BTNSHADOW));
        hOldBrush = (HBRUSH)SelectObject (hDC,hBrush);
        PatBlt (hDC,rFrame.left,rFrame.top,cxBorder,
            (rFrame.bottom-rFrame.top),PATCOPY);
        PatBlt (hDC,rFrame.left,rFrame.top,(rFrame.right-rFrame.left),
            cyBorder,PATCOPY);
        SelectObject (hDC,hOldBrush);
        DeleteObject (hBrush);
        rFrame.left += cxBorder;
        rFrame.top += cyBorder;
    }
	 if (wState == BMS_SELECTED)
    	hBrush = CreateSolidBrush (GetSysColor(COLOR_BTNSHADOW));
	 else
		hBrush = CreateSolidBrush (GetSysColor(COLOR_BTNFACE));
    hOldBrush = (HBRUSH)SelectObject (hDC,hBrush);
    PatBlt (hDC,rFrame.left,rFrame.top,rFrame.right - rFrame.left,
        rFrame.bottom - rFrame.top,PATCOPY);
    SelectObject (hDC,hOldBrush);
    DeleteObject (hBrush);
}

void  Ctl_DrawPushButton (
HDC hDC,                 // Handle to DC in which to draw bevel
LPRECT lpRect,           // rectangle of pushbutton (shrunk to inner rect)
WORD wStyle,                // style - DP_DBLBORDER, DP_CHAMFERFRAME, 
                            //         DP_THINBEVEL, DP_THICKBEVEL, DP_CALCRECT
WORD wState,                // state - PBSTATE_UP, PBSSTATE_DOWN, 
                            //         PBSTATE_SELECTED
HBRUSH hBkground)
/*  This function draws a push button frame with borders and bevels for
    the specific state, fills the button face and updates the lpRect to
    encompase the area inside the button (including inner spacing) for
    the button text or picture. If the wStyle includes (ored in) the 
    DP_CALCRECT style, hDC, hBkground, lpBtnColors are ignored and
    the lpRect is changed to indictae the inner area based on the wState.
    This can is used to determine control sizes. 
*/
{
	HBRUSH hOldBrush;
	HBRUSH hBrush;
    RECT rInside;
    int cxBorder = GetSystemMetrics (SM_CXBORDER);
    int cyBorder = GetSystemMetrics (SM_CYBORDER);
    int width = 3;

    CopyRect (&rInside,lpRect);
    if (wStyle & DP_DBLBORDER)
       width++;
    if (wStyle & DP_THICKBEVEL)
       width++;
    InflateRect (lpRect,-width*cxBorder,-width*cyBorder);
    if (wState == PBSTATE_DOWN || wState == PBSTATE_SELECTED)
        OffsetRect (lpRect,cxBorder,cyBorder);
    if (!(wStyle & DP_CALCRECT))
    {
		 COLOR rgbBtnFace;
		 COLOR rgbBtnShadow;
		 COLOR rgbBtnHighlight;
		 COLOR rgbBtnBorder;

		 rgbBtnFace = GetSysColor (COLOR_BTNFACE);
        rgbBtnShadow = GetSysColor (COLOR_BTNSHADOW);
        rgbBtnHighlight = WHITE;
        rgbBtnBorder = GetSysColor (COLOR_WINDOWFRAME);

        if (rgbBtnShadow == rgbBtnHighlight)
        {   
            rgbBtnFace = WHITE;
            rgbBtnShadow = LIGHT_GRAY;
            rgbBtnHighlight = WHITE;
            rgbBtnBorder = BLACK;
        }
        hBrush = CreateSolidBrush (rgbBtnBorder);
        FrameRect (hDC,&rInside,hBrush);
        if (wStyle & DP_CHAMFERFRAME)
        {
            PatBlt (hDC,rInside.left,rInside.top,cxBorder,cyBorder,PATCOPY);
            PatBlt (hDC,rInside.left,rInside.bottom,cxBorder,-cyBorder,PATCOPY);
            PatBlt (hDC,rInside.right,rInside.top,-cxBorder,cyBorder,PATCOPY);
            PatBlt (hDC,rInside.right,rInside.bottom,-cxBorder,-cyBorder,PATCOPY);
        }
        InflateRect (&rInside,-cxBorder,-cyBorder);
        if (wStyle & DP_DBLBORDER)
        {
            FrameRect (hDC,&rInside,hBrush);
            InflateRect (&rInside,-cxBorder,-cyBorder);
        }
        DeleteObject (hBrush);
        if (wState == PBSTATE_UP)
        {   
            Ctl_DrawBevel (hDC,rgbBtnShadow,rgbBtnHighlight,
                rInside,(wStyle & DP_THICKBEVEL ? 2 : 1),DB_RECT | DB_OUT);
            InflateRect (&rInside,(wStyle & DP_THICKBEVEL ? -2 : -1)*cxBorder,
                (wStyle & DP_THICKBEVEL ? -2 : -1)*cyBorder);
        }
        else if (wState == PBSTATE_DOWN || wState == PBSTATE_SELECTED)
        {
            hBrush = CreateSolidBrush (wState == PBSTATE_SELECTED ? 
                rgbBtnBorder : rgbBtnShadow);
            SelectObject (hDC,hBrush);
            PatBlt (hDC,rInside.left,rInside.top,cxBorder,
                (rInside.bottom-rInside.top),PATCOPY);
            PatBlt (hDC,rInside.left,rInside.top,(rInside.right-rInside.left),
                cyBorder,PATCOPY);
            SelectObject (hDC,hOldBrush);
            DeleteObject (hBrush);
            rInside.left += cxBorder;
            rInside.top += cyBorder;
        }
        hBrush = CreateSolidBrush (wState == PBSTATE_SELECTED ? 
            rgbBtnShadow : rgbBtnFace);
        SelectObject (hDC,hBrush);
        PatBlt (hDC,rInside.left,rInside.top,rInside.right - rInside.left,
            rInside.bottom - rInside.top,PATCOPY);
        SelectObject (hDC,hOldBrush);
        DeleteObject (hBrush);
    }
}

void Ctl_DrawLed (
HDC hDC,         // Handle to DC in which to draw bevel
COLOR rgbColor,     // Color of diamond
RECT rRect,         // Rectangle of outer bounds of the bevel
BOOL bHilight)      // With hilight   
{
    HPEN hOldPen, hPen;
    HBRUSH hOldBrush, hBrush;
    
    hPen = CreatePen (PS_SOLID,1,rgbColor);
    hOldPen = (HPEN)SelectObject (hDC,hPen);
    hBrush = CreateSolidBrush (rgbColor);
    hOldBrush = (HBRUSH)SelectObject (hDC,hBrush);
    Ellipse (hDC,rRect.left,rRect.top,rRect.right,rRect.bottom);
    if (bHilight)
    {
        RECT rTemp = rRect;

        InflateRect (&rTemp,10,10);
        InflateRect (&rRect,-2,-2);
        SelectObject (hDC,hOldPen);
        DeleteObject (hPen);
        hPen = CreatePen (PS_SOLID,1,WHITE);
        SelectObject (hDC,hPen);
        Arc (hDC,rRect.left,rRect.top,rRect.right,rRect.bottom,
            (rTemp.left+rTemp.right)/2,rTemp.top,
            rTemp.left,(rTemp.top+rTemp.bottom)/2);
    }
    SelectObject (hDC,hOldPen);
    SelectObject (hDC,hOldBrush);
    DeleteObject (hPen);
    DeleteObject (hBrush);
}

void Ctl_DrawDiamond (
HDC hDC,         // Handle to DC in which to draw diamond
COLOR rgbColor,     // Color of diamond
RECT rRect)         // Rectangle of outer bounds of the bevel
{
    POINT ptsDiamond[4];
    HPEN hOldPen, hPen;
    HBRUSH hOldBrush, hBrush;
    int OldPolyMode = SetPolyFillMode (hDC,WINDING);
    int hWidth = max ((rRect.right - rRect.left)/2,1);
    int hHeight = max ((rRect.bottom - rRect.top)/2,1);
    
    hPen = CreatePen (PS_SOLID,1,rgbColor);
    hOldPen = (HPEN)SelectObject (hDC,hPen);
    hBrush = CreateSolidBrush (rgbColor);
    hOldBrush = (HBRUSH)SelectObject (hDC,hBrush);
    ptsDiamond[0].x = rRect.left+hWidth;
    ptsDiamond[0].y = rRect.top;
    ptsDiamond[1].x = rRect.left+2*hWidth;
    ptsDiamond[1].y = rRect.top+hHeight;
    ptsDiamond[2].x = rRect.left+hWidth;
    ptsDiamond[2].y = rRect.top+2*hHeight;
    ptsDiamond[3].x = rRect.left;
    ptsDiamond[3].y = rRect.top+hHeight;
    Polygon (hDC,ptsDiamond,4);
    DeleteObject (SelectObject (hDC,hOldPen));
    DeleteObject (SelectObject (hDC,hOldBrush));
    SetPolyFillMode (hDC,OldPolyMode);
}

void Ctl_GetAvgCharSize (
HFONT hFont,           // handle to font of interest.
LPWORD lpwCharW,     // returned average character width.
LPWORD lpwCharH)     // returned average character height.
{
    HFONT hOldFont;
    TEXTMETRIC tm;
    HDC hDisplayDC = GetDC ((HWND)NULL);
    
    hOldFont = (HFONT)(hFont ? SelectObject (hDisplayDC,hFont) : (HFONT)NULL);
    GetTextMetrics (hDisplayDC,&tm);
    if (hOldFont)
        SelectObject (hDisplayDC,hOldFont);
    ReleaseDC ((HWND)NULL,hDisplayDC);
    if (lpwCharW)
        *lpwCharW = tm.tmAveCharWidth;
    if (lpwCharH)
        *lpwCharH = tm.tmHeight+tm.tmExternalLeading;
}

void Ctl_DrawCheck (
HDC hDC,         // Handle to DC in which to draw bevel
COLOR rgbColor,     // Color of check
RECT rRect)         // Rectangle of outer bounds of the bevel
{
    POINT ptsCheck[7];
    HPEN hOldPen = (HPEN)SelectObject (hDC,GetStockObject (NULL_PEN));
    HBRUSH hOldBrush = (HBRUSH)SelectObject (hDC,CreateSolidBrush (rgbColor));
    int OldPolyMode = SetPolyFillMode (hDC,WINDING);
    int Width = (rRect.right - rRect.left);
    int Height = (rRect.bottom - rRect.top);

    ptsCheck[0].x = rRect.left;
    ptsCheck[0].y = rRect.top+Height;
    ptsCheck[1].x = rRect.left;
    ptsCheck[1].y = rRect.top+Height/2;
    ptsCheck[2].x = rRect.left+(2*Width+5)/10;
    ptsCheck[2].y = rRect.top+(3*Height+5)/10;
    ptsCheck[3].x = rRect.left+(2*Width+5)/10;
    ptsCheck[3].y = rRect.top+(8*Height+5)/10;
    ptsCheck[4].x = rRect.left+Width;
    ptsCheck[4].y = rRect.top;
    ptsCheck[5].x = rRect.left+Width;
    ptsCheck[5].y = rRect.top+Height/5;
    ptsCheck[6].x = rRect.left+Width/5;
    ptsCheck[6].y = rRect.top+Height;
    Polygon (hDC,ptsCheck,7);
    SelectObject (hDC,hOldPen);
    DeleteObject (SelectObject (hDC,hOldBrush));
    SetPolyFillMode (hDC,OldPolyMode);
}

/***********************************************************************/
void Ctl_EraseBackground( HWND hWnd, HDC hDC, LPRECT lpRect, WORD wType )
/***********************************************************************/
{
HBRUSH hBrush;

hBrush = Ctl_GetBackgroundBrush( hWnd, hDC, wType );
FillRect( hDC, lpRect, hBrush );
}

HBRUSH Ctl_GetBackgroundBrush( HWND hWnd, HDC hDC, WORD wType )
{
#ifdef DLGEDIT
return((HBRUSH)GetStockObject(LTGRAY_BRUSH));	
#else
HBRUSH  hBrush;

#ifdef WIN32
if (wType == CTLCOLOR_MSGBOX)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORMSGBOX, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_EDIT)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOREDIT, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_LISTBOX)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORLISTBOX, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_BTN)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_DLG)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORDLG, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_SCROLLBAR)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORSCROLLBAR, (WPARAM)hDC, (LPARAM)hWnd);
else
if (wType == CTLCOLOR_STATIC)
	hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)hWnd);
else
	hBrush = NULL;
#else
hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOR, (WPARAM)hDC, MAKELONG (hWnd,wType));
#endif
if (!hBrush)
	{
	hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	SetBkColor (hDC,GetSysColor (COLOR_WINDOW));
        SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
	}
return( ( HBRUSH )hBrush );
#endif
}

COLOR Ctl_GetDisabledBtnTextColor()
{
#ifdef CHICAGO
return(GetSysColor(COLOR_GRAYTEXT));
#else
return(GetSysColor(COLOR_BTNSHADOW));
#endif
}

COLOR Ctl_GetBtnTextColor()
{
return(GetSysColor(COLOR_BTNTEXT));
}

COLOR Ctl_GetBtnFaceColor()
{
return(GetSysColor(COLOR_BTNFACE));
}

COLOR Ctl_GetTextColor()
{
return(GetSysColor(COLOR_WINDOWTEXT));
}

COLOR Ctl_GetBtnShadowColor()
{
return(GetSysColor(COLOR_BTNSHADOW));
}

COLOR Ctl_GetBtnHilightColor()
{
return(GetSysColor(COLOR_BTNHIGHLIGHT));
}

COLOR Ctl_GetBtnBorderColor()
{
return(RGB(0, 0, 0));
}




