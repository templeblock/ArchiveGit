/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                    rspotdot.c

******************************************************************************
******************************************************************************

  Description:  SpotDot dialog functions.
 Date Created:  02/11/94
       Author:  Ray

*****************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "rspotdot.h"

extern HINSTANCE hInstAstral;

/********************************* Constants ********************************/
#define SPOT_DIAMETER       15
#define COLOR_ACTIVEDOT     RGB(0, 255, 0)
#define COLOR_INACTIVEDOT   RGB(255, 0, 0)
#define DEF_TIMEOUT         1500			// measured in milli-seconds
#define TM_CHANGE			1
#define TM_TIMER			2
/******************************** Private Data ******************************/

int  iTimerVal = 0;
int  iActiveDot = 1;
HWND hSpotWnd = 0;
bool bShowBackground = FALSE;
bool bDoSequential;
bool bUseTimer;
int  bPlaying;
static HFONT hNormalFont;
static BOOL bFinished;

int  SpotDotProc(HWND hWindow, LPARAM lParam, UINT msg);
void SpotDotPixelProc(LPTR lpSrc, LPTR lpDst, int dx, int iDepth);
void PaintDots(HWND hWnd);
void ShowAllSpotObjects(HWND hWnd, bool bShow);
void CalcSpotRect(LPOBJECT lpObject, RECT* rSpot);
void UpdateSpotImage(HWND hWnd, LPRECT lprUpdate);
int  GetObjectPosition(LPOBJECT lpFindObject);
bool CheckFinished();
void RemoveCodedBackground();
void ChooseRandomSpotDot();
void GetRandomDot();
int  GetNumHiddenObjects();
void ShowSpotDotControls(HWND hWnd, bool bCover);
static LPOBJECT ImgFindObjectRect(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
					BOOL fExcludeBase);
static void SetSpotDotFont(HWND hWnd);
static void UpdateTimerText(HWND hWnd);

/****************************** Public Functions ****************************/


BOOL WINPROC EXPORT DlgRoomSpotDot (HWND hWnd,UINT wMsg,WPARAM wParam,LPARAM lParam)
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
				ShowCoverAnimatedButtons(hWnd, TRUE);
				StopAnimation();
                bPlaying = FALSE;
                ShowSpotDotControls(hWnd, TRUE);
                break;
                
            case HS_SHOW_ME:
				hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
                iActiveDot = 0;
                bShowBackground = TRUE;
                ShowAllSpotObjects(hImageCtrl, TRUE);
				CheckFinished();
                UpdateSpotImage(hWnd, NULL);
				StartAnimation(hImageCtrl);
                break;
                
            case HS_COLORME_ME:
				StopAnimation();
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
					StopAnimation();
                    ShowSpotDotControls(hWnd, FALSE);
                    switch(idCurrentRoom)
                    {
                        case RM_SPACEBALL:
                            wsprintf (szTemp,GetString (IDS_SPACEBALL, NULL),wParam - HS_TAB1 + 1);
                            break;
                        case RM_EUROPE:
                            wsprintf (szTemp,GetString (IDS_EUROPE, NULL),wParam - HS_TAB1 + 1);
                            break;
                    }
                    hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
                    
                    if (hImageCtrl)
                    {
                        PathCat (szPreview,Control.PouchPath,szTemp);
                        bLoad = AstralImageLoad( 0, szPreview, TRUE, FALSE);
                        if (bLoad && lpImage)
                        {
							iTimerVal = -1;
// wait on first timer message, this made it show up too soon
//							if (bUseTimer)
//								UpdateTimerText(hWnd);
                            bPlaying = TRUE;
                            SetWindowLong(hImageCtrl, GWL_IMAGE,
                                (long) ImgGetBase (lpImage));
                            SetWindowLong(hImageCtrl, GWL_IMAGE_TOOLPROC,
                                (long) SpotDotProc);
                            bShowBackground = FALSE;
                            ImgSetPixelProc(SpotDotPixelProc);
                            ShowWindow(hImageCtrl, SW_SHOW);
                            ShowAllSpotObjects(hImageCtrl, FALSE);
                            ChooseRandomSpotDot();
                            UpdateSpotImage(hWnd, NULL);
                        }
                    }
                        
                        
                } // HS_TABS
#ifdef STUDIO			
				SoundStartID(HS_XTRA_EUROPE, FALSE, 0);
#else
				SoundStartID(HS_XTRA_SPACEBALL, FALSE, 0);
#endif			
                break;
                
                default:
                    bHandled = FALSE;
            } // switch (wParam)
            break;
                          
        case WM_INITDIALOG:
            ShowSpotDotControls(hWnd, TRUE);
            srand((unsigned)time(NULL));
            bDoSequential = (idCurrentRoom != RM_SPACEBALL);
            bUseTimer     = (idCurrentRoom != RM_SPACEBALL);
//bDoSequential = TRUE;
//bUseTimer     = TRUE;
            if (bUseTimer)
			{
                SetTimer(hWnd, TM_CHANGE, DEF_TIMEOUT, NULL);
                SetTimer(hWnd, TM_TIMER,  1000,        NULL);
	        	ShowWindow (GetDlgItem(hWnd, IDC_TIMER_TEXT), SW_HIDE);
			}
            
            SetSpotDotFont(hWnd);
            hSpotWnd = hWnd;
            bHandled = FALSE;           // allow DefRoomProc to be called.
            break;

        case WM_DESTROY:
			StopAnimation();
            bPlaying = FALSE;
            if (bUseTimer)
			{
                KillTimer(hWnd, TM_CHANGE);
                KillTimer(hWnd, TM_TIMER);
			}
            ImgSetPixelProc(NULL);      // make sure our proc is no longer used
            bHandled = FALSE;           // allow DefRoomProc to be called
            if (hNormalFont)
                DeleteObject(hNormalFont);
            break;

        case WM_TIMER:
            if (bUseTimer && bPlaying)
            {
				if (wParam == TM_CHANGE)
				{
                	hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);           
                	ChooseRandomSpotDot();
                	PaintDots(hImageCtrl);
				}
				else
				{
					UpdateTimerText(hWnd);
				}
            }
            else
                bHandled = FALSE;    
            break;
            
        case WM_PAINT:
            hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
            DefRoomProc (hWnd,wMsg,wParam,lParam);
            PaintDots(hImageCtrl);
            break;
            
        default:
            bHandled = FALSE;    
    } // switch (wMsg)

    if (!bHandled)
        bHandled = DefRoomProc (hWnd,wMsg,wParam,lParam);
    return bHandled;
}




int SpotDotProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
    int         x, y;
    LPOBJECT    lpObject;
    RECT        rSpot;
    HWND        hImageCtrl;

    x = LOWORD(lParam);
    y = HIWORD(lParam);

	if (!bPlaying)
		AnimateProc(hWindow, lParam, msg);

    switch (msg)
    {
        case WM_CREATE:	        // mouse down messages
    	case WM_LBUTTONDOWN:
			if ( !lpImage )
				break;
            lpObject = ImgFindObjectRect(lpImage, x, y, TRUE, TRUE);
    		if (lpObject && lpObject->fHidden)
    		{
                CalcSpotRect(lpObject, &rSpot);
                if (PtInRect(&rSpot, MAKEPOINT(lParam)))
                {
                    if (!bDoSequential ||
                        (iActiveDot == GetObjectPosition(lpObject)))
                    {
                        SoundStartResource("GOODANSWER1", FALSE, 0);
                        lpObject->fHidden = FALSE;
                        
                        ChooseRandomSpotDot();
                        hImageCtrl = GetDlgItem(hSpotWnd, IDC_HIDDEN_IMAGE);           
                        PaintDots(hImageCtrl);

                        if (!CheckFinished())
                            UpdateSpotImage(hSpotWnd, &lpObject->rObject);
                        else 
                        {
							SoundStartResource("GOODANSWER3", FALSE, 0);
                        	UpdateSpotImage(hSpotWnd, NULL);
							StartAnimation(hImageCtrl);
						}
                    }
                }
            }
			else
				SoundStartResource("WRONGANSWER2", FALSE, 0);	
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




void SpotDotPixelProc(LPTR lpSrc, LPTR lpDst, int dx, int iDepth)
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
        
        if ( !r && !g && (b == 255) )
        {
            *lpDst++ = bShowBackground ? 0 : 255;
            *lpDst++ = bShowBackground ? 0 : 255;
            *lpDst++ = bShowBackground ? 0 : 255;
        }
        else
        {
            lpDst += 3;
        }
    }
}




void PaintDots(HWND hWnd)
{
// Draw all the dots in the image.  Each dot is located at the center of
// each floating object in the image.
    RECT        rSpot;
    HDC         hDC;
    HBRUSH      hActiveBrush;
    HBRUSH      hInActiveBrush;
    HBRUSH      hOldBrush;
    LPOBJECT    lpObject;
    int         i;
    bool        bActive;

	if (!lpImage)
		return;

    hDC = GetDC(hWnd);    
    lpObject = ImgGetNextObject(lpImage, NULL, TRUE, FALSE);
    lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    hActiveBrush   = CreateSolidBrush(COLOR_ACTIVEDOT);
    hInActiveBrush = CreateSolidBrush(COLOR_INACTIVEDOT);
    i = 1;

    while (lpObject)
    {
        if (lpObject->fHidden)
        {
            CalcSpotRect(lpObject, &rSpot);
            bActive = !bDoSequential || (iActiveDot == i);
            hOldBrush = (HBRUSH)SelectObject(hDC, bActive ?  
                            hActiveBrush : hInActiveBrush);
            Ellipse(hDC,
                rSpot.left,    
                rSpot.top,    
                rSpot.right,    
                rSpot.bottom);    
            (HBRUSH)SelectObject(hDC, hOldBrush);
        }
        i++;
        lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    }
    ReleaseDC(hWnd, hDC);
    DeleteObject(hActiveBrush);
    DeleteObject(hInActiveBrush);
}




void ShowAllSpotObjects(HWND hWnd, bool bShow)
{
	LPOBJECT lpObject;

	if (!lpImage)
		return;
// Show or hide all the floating objects in the image.
    lpObject = ImgGetNextObject(lpImage, NULL, TRUE, FALSE);
    while (lpObject)
    {
        lpObject->fHidden = !bShow;
        lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    }
}



void CalcSpotRect(LPOBJECT lpObject, RECT* rSpot)
{
    int         Width, Height;

    Width  = lpObject->rObject.right  - lpObject->rObject.left;
    Height = lpObject->rObject.bottom - lpObject->rObject.top;
    rSpot->left   = lpObject->rObject.left + ((Width  - SPOT_DIAMETER) / 2);
    rSpot->top    = lpObject->rObject.top  + ((Height - SPOT_DIAMETER) / 2);
    rSpot->right  = rSpot->left + SPOT_DIAMETER;
    rSpot->bottom = rSpot->top  + SPOT_DIAMETER;
}


void UpdateSpotImage(HWND hWnd, LPRECT lprUpdate)
{
// Redraw the image control.  Use the update rect to update only a portion of it.
    HWND hImageCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
    
    InvalidateRect(hImageCtrl, lprUpdate, TRUE);
    UpdateWindow(hImageCtrl);
    PaintDots(hImageCtrl);
}





int GetObjectPosition(LPOBJECT lpFindObject)
{
    int i = 1;
    LPOBJECT lpObject;
    
	if (!lpImage)
		return 0;

    lpObject = ImgGetNextObject(lpImage, NULL, TRUE, FALSE);
    lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);

    while (lpObject)
    {
        if (lpObject == lpFindObject)
            return i;
        i++;
        lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    }
    return 0;
}


bool CheckFinished()
{   
    bFinished = (GetNumHiddenObjects() == 0);

    if (bFinished)
    {
        bShowBackground = TRUE;
        bPlaying = FALSE;
    }
    return bFinished;
}




void RemoveCodedBackground()
{
    LPFRAME lpFrame;
    int x, y;
    int Width;
    int Height;
    LPTR lpSrcLine;
    
    if (!lpImage) return;
	ShowAllSpotObjects(NULL, TRUE);
    ImgCombineObjects(lpImage, TRUE, FALSE, TRUE);
    lpFrame = ImgGetBaseEditFrame(lpImage);
    if (!lpFrame) return;
	if ( FrameDepth(lpFrame) != 3 )
		return;
    Width  = FrameXSize (lpFrame);
    Height = FrameYSize (lpFrame);

    for (y=0;y<Height;y++)
    {
	    AstralClockCursor( y, Height, NO );
        if ( !(lpSrcLine = FramePointer (lpFrame,0,y,TRUE)) )
			continue;

        for (x=0;x<Width;x++)
        {
            RGBS Pixel;
            FrameGetRGB (lpFrame,lpSrcLine,&Pixel,1);

			// change blue to black
            if ( !Pixel.red && !Pixel.green && (Pixel.blue == 255) )
            {
                Pixel.blue  = 0;
                FrameSetRGB (lpFrame,&Pixel,lpSrcLine,1);
            }
            lpSrcLine += 3;
            
        } // Width
    }     // Height
}




int GetNumHiddenObjects()
{
// Just return the number of objects that are still hidden.
    LPOBJECT lpObject;
    int i = 0;
    
    if (!lpImage)
		return 0;

    lpObject = ImgGetNextObject(lpImage, NULL, TRUE, FALSE);
    lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);

    while (lpObject)
    {
        if (lpObject->fHidden)
            i++;
        lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    }
    return i;
}


void ChooseRandomSpotDot()
{
    int     iOldActiveDot;
    int     numHidden;
    int     i = 0;
    
    numHidden = GetNumHiddenObjects();
    if (numHidden <= 1)
    {
        GetRandomDot();
        return;
    }

// Make sure we get another dot, not just the same one again and again.    
    iOldActiveDot = iActiveDot;
    while ((iOldActiveDot == iActiveDot) && (i++ < 100))
        GetRandomDot();    
}

void GetRandomDot()
{
// Choose one of the available hidden objects to use as the Active Dot.
// Choose a random value from 0 to the number of hidden objects.
// Find the number of that hidden object and use it as the active dot.

    LPOBJECT lpObject;
    int i, n, nPos;
    int numHidden;

    if (!lpImage)
		return;

// Avoid divide-by-zero
    numHidden = GetNumHiddenObjects();
    if (numHidden == 0)
    {
        iActiveDot = 0;
        return;
    }
    
// Choose the random number
    i = rand();
    i %= numHidden;
    n = 0;
    nPos = 1;

// Find the nth hidden object in the list. 
// Use that object number.        
    lpObject = ImgGetNextObject(lpImage, NULL, TRUE, FALSE);
    lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);

    while (lpObject)
    {
        if (lpObject->fHidden)
        {
            if (n == i)
            {
                iActiveDot = nPos;
                break;
            }
            n++;        // another hidden object
        }
        nPos++;
        lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE);
    }
}


void ShowSpotDotControls(HWND hWnd, bool bCover)
{
    HWND hCtrl;
    
    switch(idCurrentRoom)
    {
    case RM_EUROPE:
        hCtrl = GetDlgItem(hWnd,HS_LOGO_EUROPE);
	    if (hCtrl)
	        ShowWindow(hCtrl, !bCover ? SW_HIDE : SW_SHOW);
        hCtrl = GetDlgItem(hWnd,HS_XTRA_EUROPE);
	    if (hCtrl)
	        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
        hCtrl = GetDlgItem(hWnd,IDC_TIMER_BLK);
	    if (hCtrl)
	        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
        hCtrl = GetDlgItem(hWnd,IDC_TIMER_TEXT);
	    if (hCtrl)
	        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
    	break;
    case RM_SPACEBALL:
        hCtrl = GetDlgItem(hWnd,HS_LOGO_SPACEBALL);
	    if (hCtrl)
	        ShowWindow(hCtrl, !bCover ? SW_HIDE : SW_SHOW);
        hCtrl = GetDlgItem(hWnd,HS_XTRA_SPACEBALL);
	    if (hCtrl)
	        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
		break;
    }
    hCtrl = GetDlgItem(hWnd, IDC_HIDDEN_IMAGE);
    if (hCtrl)
        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_COLORME_ME);
    if (hCtrl)
        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);

    hCtrl = GetDlgItem(hWnd, HS_SHOW_ME);
    if (hCtrl)
        ShowWindow(hCtrl, bCover ? SW_HIDE : SW_SHOW);
}

/************************************************************************/
LPOBJECT ImgFindObjectRect(LPIMAGE lpImage, int x, int y, BOOL fBottomUp,
					BOOL fExcludeBase)
/************************************************************************/
{
POINT pt;
LPOBJECT lpObject = NULL;
LPOBJECT lpBase;
RECT rSpot;

if (!lpImage)
	return(NULL);
pt.x = x;
pt.y = y;
lpBase = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, fBottomUp, NO))
	{
	if (fExcludeBase && lpObject == lpBase)
		continue;
		
		CalcSpotRect(lpObject, &rSpot);
		if (lpObject->fHidden && PtInRect(&rSpot, pt))
		{
			if (!bDoSequential ||
				(iActiveDot == GetObjectPosition(lpObject)))
			{
				return(lpObject);
			}
		}
	}
return(NULL);
}



void SetSpotDotFont(HWND hWnd)
{
// Set the text controls with the proper fonts.
    char szTmp[MAX_PATH];
	if (LoadString(hInstAstral, IDS_FONT_NORMAL, szTmp, sizeof(szTmp)))
	{
	    hNormalFont = CreateFont(-36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  							(PSTR)szTmp);	// Typeface Name
		if (hNormalFont != 0)
			SendDlgItemMessage(hWnd, IDC_TIMER_TEXT, WM_SETFONT, (WPARAM)hNormalFont, 0L);
    }
}


void UpdateTimerText(HWND hWnd)
{
	int i = iTimerVal++;
	if (i < 2)
		i = 1;
	
	SetDlgItemInt(hWnd, IDC_TIMER_TEXT, i, FALSE);

//	hCtrl = GetDlgItem(hWnd,IDC_TIMER_BLK);
//	if (hCtrl)
//	{
//		InvalidateRect(hCtrl, NULL, TRUE);
//		UpdateWindow(hCtrl);
//	}
}



