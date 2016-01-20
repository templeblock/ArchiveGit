/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rdot2dot.c

******************************************************************************
******************************************************************************

  Description:  Dot-to-dot dialog functions.
 Date Created:  03/2/94
       Author:  Ray	

*****************************************************************************/

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;

/********************************* Constants ********************************/
#define DOT_DIAMETER       	10
#define COLOR_ACTIVEDOT     RGB(0, 255, 0)
#define COLOR_INACTIVEDOT   RGB(255, 0, 0)
#define COLOR_LINE		    RGB(0, 0, 255)
#define WIDTH_LINE		    4
#define STYLE_LINE		    PS_SOLID
#define MAX_DOTS			100
/******************************** Private Data ******************************/

static int  iActiveDot = 1;
static int  iNumDots = 0;
static bool bPlaying = FALSE;
static HWND hDot2DotWnd = 0;
static POINT DotPoints[MAX_DOTS];

static void InitPoints();
static int  Dot2DotProc(HWND hWindow, LPARAM lParam, UINT msg);
static void Dot2DotPixelProc(LPTR lpSrc, LPTR lpDst, int dx, int iDepth);
static void PaintDot2Dots(HWND hWnd);
static void RemoveCodedBackground();
static void UpdateDot2DotImage(HWND hWnd, LPRECT lprUpdate);
static bool CheckFinished();
static void ShowControls(HWND hWnd, bool bCover);
/****************************** Public Functions ****************************/


BOOL WINPROC EXPORT DlgRoomDot2Dot (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
{
    BOOL bHandled = TRUE;
    char szTemp[MAX_PATH];
    HWND hImageCtrl;

    switch (wMsg)
    {
        case WM_COMMAND:
            switch (wParam)
            {
            case HS_COVER:
				ShowControls(hWnd, TRUE);
				ShowCoverAnimatedButtons(hWnd, TRUE);
                break;
                
            case HS_SHOW_ME:
                iActiveDot = 0;
				bPlaying = FALSE;
				UpdateDot2DotImage(hWnd, NULL);
                break;
                
            case HS_COLORME_ME:
                RemoveCodedBackground();
                GoPaintApp (hWnd,szPreview);
                break;
                
            case HS_TAB1:
            case HS_TAB2:
            case HS_TAB3:
            case HS_TAB4:
            case HS_TAB5:
            case HS_TAB6:
            case HS_TAB7:
            case HS_TAB8:
                {
                    BOOL    bLoad;

					ShowCoverAnimatedButtons(hWnd, FALSE);
					ShowControls(hWnd, FALSE);
                    hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
                    
                    if (hImageCtrl)
                    {
                		wsprintf (szTemp,GetString (IDS_CIRCUS, NULL),wParam - HS_TAB1 + 1);
                        PathCat (szPreview,Control.PouchPath,szTemp);
                        bLoad = AstralImageLoad( 0, szPreview, TRUE, FALSE);
                        if (bLoad && lpImage)
                        {
							bPlaying = TRUE;
                            SetWindowLong(hImageCtrl, GWL_IMAGE,
                                (long) ImgGetBase (lpImage));
                            SetWindowLong(hImageCtrl, GWL_IMAGE_TOOLPROC,
                                (long) Dot2DotProc);
							InitPoints();
                            ImgSetPixelProc(Dot2DotPixelProc);
                            ShowWindow(hImageCtrl, SW_SHOW);
                            UpdateDot2DotImage(hWnd, NULL);
                        }
                    }
                        
                        
                } // HS_TABS
				SoundStartID(HS_XTRA_CIRCUS, FALSE, 0);
                break;
                
                default:
                    bHandled = FALSE;
            } // switch (wParam)
            break;
                          
        case WM_INITDIALOG:
            hDot2DotWnd = hWnd;
			ShowControls(hWnd, TRUE);
            bHandled = FALSE;           // allow DefRoomProc to be called.
            break;

        case WM_DESTROY:
            ImgSetPixelProc(NULL);      // make sure our proc is no longer used
            bHandled = FALSE;           // allow DefRoomProc to be called
            break;

        case WM_PAINT:
            hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
            DefRoomProc (hWnd,wMsg,wParam,lParam);
			UpdateWindow(hImageCtrl);
            PaintDot2Dots(hImageCtrl);
            break;
            
        default:
            bHandled = FALSE;    
    } // switch (wMsg)

    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}




int Dot2DotProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
    int         xPos, yPos;
	int			x, y;
    RECT        rDot;
    HWND        hImageCtrl;

    xPos = LOWORD(lParam);
    yPos = HIWORD(lParam);
    switch (msg)
    {
        case WM_CREATE:	        // mouse down messages
    	case WM_LBUTTONDOWN:
			x = DotPoints[iActiveDot].x - (DOT_DIAMETER / 2);
			y = DotPoints[iActiveDot].y - (DOT_DIAMETER / 2);
			SetRect(&rDot, x, y, x+DOT_DIAMETER, y+DOT_DIAMETER);
			if (PtInRect(&rDot, MAKEPOINT(lParam)))
			{
				SoundStartResource("ICONS", FALSE, 0);
				iActiveDot++;
				hImageCtrl = GetDlgItem(hDot2DotWnd, IDC_HIDDEN_IMAGE);
				PaintDot2Dots(hImageCtrl);
				if (!CheckFinished())
				{
					SoundStartResource("GOODANSWER3", FALSE, 0);
					UpdateDot2DotImage(hDot2DotWnd, NULL);
				}
					
			}
			else
				SoundStartResource("WRONGANSWER3", FALSE, 0);
    		break;

    	case WM_SETCURSOR:
    		return TRUE;

        case WM_LBUTTONUP:
    		break;

        case WM_MOUSEMOVE:	// sent when ToolActive is on
    		break;
    }
    return TRUE;
}




void Dot2DotPixelProc(LPTR lpSrc, LPTR lpDst, int dx, int iDepth)
{
    BYTE r, g, b;

    if ( !iDepth ) iDepth = 1;
    copy( lpSrc, lpDst, dx * iDepth );
    if (iDepth != 3)  return;
    
    while (--dx >= 0)
    {
    	r = *lpSrc++;
    	g = *lpSrc++;
    	b = *lpSrc++;
        
        if ( (bPlaying && !(r + g + b)) // if playing, hide all black
        	|| (r >= 100 && !g && !b) ) // always hide dots
        {
            // This is a coded pixel that contains:
            //  1) green - No coding.  MUST be 0.
            //  2) red   - Sequence that the dot is found.
            //  3) blue  - No coding.  MUST be 0.
            
			*lpDst++ = 255;
			*lpDst++ = 255;
			*lpDst++ = 255;
        }
		else
        if ( !r && !g && b==255 ) // always make blue lines black
		{
			*lpDst++ = 0;
			*lpDst++ = 0;
			*lpDst++ = 0;
		}
		else
			lpDst += 3;
    }
}




void PaintDot2Dots(HWND hWnd)
{
// Draw all the dots in the image.  Each dot is located at the center of
// each floating object in the image.
	RECT        rDot;
	HDC         hDC;
	HBRUSH      hActiveBrush;
	HBRUSH      hInActiveBrush;
	HBRUSH      hOldBrush;
	HPEN		hPen;
	HPEN		hOldPen;
	int         i;
	int			x, y;
	POINT		ptPrev;
	
	if (!bPlaying) return;

	hDC = GetDC(hWnd);    
	hActiveBrush   = CreateSolidBrush(COLOR_ACTIVEDOT);
	hInActiveBrush = CreateSolidBrush(COLOR_INACTIVEDOT);
	hPen = CreatePen(STYLE_LINE, WIDTH_LINE, COLOR_LINE);

// Draw each of the Dots.  An array of points indicates where each dot is
// placed.
	for (i=0; i<iNumDots; i++)
	{	
		if (DotPoints[i].y != 0)
		{
// Draw the Ellipse		
			x = DotPoints[i].x - (DOT_DIAMETER / 2);
			y = DotPoints[i].y - (DOT_DIAMETER / 2);
			SetRect(&rDot, x, y, x+DOT_DIAMETER, y+DOT_DIAMETER);

			hOldBrush = (HBRUSH)SelectObject(hDC, i==iActiveDot ?  
			            hActiveBrush : hInActiveBrush);
			Ellipse(hDC,
				rDot.left,    
				rDot.top,    
				rDot.right,    
				rDot.bottom);    
			SelectObject(hDC, hOldBrush);

// Draw the connecting lines
			if (i > 0 && i < iActiveDot)
			{			
				hOldPen = (HPEN)SelectObject(hDC, hPen);
				MoveToEx(hDC, DotPoints[i].x, DotPoints[i].y, NULL);
				LineTo(hDC, ptPrev.x, ptPrev.y);
				SelectObject(hDC, hOldPen);
			}
			ptPrev.x = DotPoints[i].x;
			ptPrev.y = DotPoints[i].y;
		}
	}

	ReleaseDC(hWnd, hDC);
	DeleteObject(hActiveBrush);
	DeleteObject(hInActiveBrush);
	DeleteObject(hPen);
}






void UpdateDot2DotImage(HWND hWnd, LPRECT lprUpdate)
{
// Redraw the image control.  Use the update rect to update only a portion of it.
    HWND hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
    
    InvalidateRect(hImageCtrl, lprUpdate, TRUE);
    UpdateWindow(hImageCtrl);
    PaintDot2Dots(hImageCtrl);
}




bool CheckFinished()
{   
	bPlaying = (iActiveDot < iNumDots);
    return bPlaying;
}




void RemoveCodedBackground()
{
    LPFRAME lpFrame;
    int x, y;
    int Width;
    int Height;
    LPTR lpSrcLine;
    BYTE r, g, b;
    
    if (!lpImage) return;
    ImgCombineObjects(lpImage, TRUE, FALSE, TRUE);
    lpFrame = ImgGetBaseEditFrame(lpImage);
    if (!lpFrame) return;
    Width  = FrameXSize (lpFrame);
    Height = FrameYSize (lpFrame);

    for (y=0;y<Height;y++)
    {
	    AstralClockCursor( y, Height, NO );
        lpSrcLine = FramePointer (lpFrame,0,y,TRUE);

        for (x=0;x<Width;x++)
        {
            RGBS Pixel;
            FrameGetRGB (lpFrame,lpSrcLine,&Pixel,1);
            r = Pixel.red;
            g = Pixel.green;
            b = Pixel.blue;

            if (r >= 100 && !g && !b)
            { // a dot
                Pixel.red   = 255;
                Pixel.green = 255;
                Pixel.blue  = 255;
                FrameSetRGB (lpFrame,&Pixel,lpSrcLine,1);
            }
			else
            if ( (r != g) || (g != b) )
            { // not white or black
                Pixel.red   = 0;
                Pixel.green = 0;
                Pixel.blue  = 0;
                FrameSetRGB (lpFrame,&Pixel,lpSrcLine,1);
            }
            lpSrcLine += 3;
            
        } // Width
    }     // Height
}


void ShowControls(HWND hWnd, bool bCover)
{
    HWND hCtrl;
    
    hCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_LOGO_CIRCUS);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_SHOW : SW_HIDE);

    hCtrl = GetDlgItem(hWnd, HS_XTRA_CIRCUS);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_COLORME);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_SHOWME);
    if (hCtrl)
    	ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
}


void InitPoints()
{
	int 		i;
	LPFRAME 	lpFrame;
	int 		Width;
	int 		Height;
	int 		x,y;
	LPTR 		lpSrcLine;
	BYTE 		r, g, b;

	iActiveDot = 0;
	iNumDots = 0;

// Clear out the array	
	for (i=0; i<MAX_DOTS; i++)
	{
		DotPoints[i].x = 0;
		DotPoints[i].y = 0;
	}

    if (!lpImage) return;
    lpFrame = ImgGetBaseEditFrame(lpImage);
    if (!lpFrame) return;
    Width  = FrameXSize (lpFrame);
    Height = FrameYSize (lpFrame);

// Find all the points in the image.
// Search for coded dots that indicate a dot-to-dot point.
// These dot points will be drawn as circles for the user to click in.
// An array of points - DotPoints[] is maintained to remember these dots.
    for (y=0;y<Height;y++)
    {
	    AstralClockCursor( y, Height, NO );
        lpSrcLine = FramePointer (lpFrame,0,y,FALSE);

        for (x=0;x<Width;x++)
        {
            RGBS Pixel;
            FrameGetRGB (lpFrame,lpSrcLine,&Pixel,1);
            r = Pixel.red;
            g = Pixel.green;
            b = Pixel.blue;
            if (r >= 100 && g == 0 && b == 0)
            {
            	// This is a coded pixel that contains:
            	//  1) green - No coding.  MUST be 0.
            	//  2) red   - Sequence that the dot is found.
            	//  3) blue  - No coding.  MUST be 0.
				r -= 100;
				if (r >= 0 && r <= MAX_DOTS)
				{
					// Save the Dot position.
					// Do not allow the dot to go off the screen.
					DotPoints[r].x = min(x, Width  - ((DOT_DIAMETER / 2) + 2));
					DotPoints[r].y = min(y, Height - ((DOT_DIAMETER / 2) + 2));
					iNumDots = max(iNumDots, r);
				}
            }
            lpSrcLine += 3;
        } // Width
    }     // Height
}




/****************************************************************************/

