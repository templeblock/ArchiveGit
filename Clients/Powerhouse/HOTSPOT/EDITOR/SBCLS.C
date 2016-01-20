/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
/*

    sbcls.c --    
		ShowSelectInfo -- Displays hotspot rect info in (modeless) SelectDlg
		DrawSelect -- Draws the selected clip rectangle with its dimensions
			on the DC/screen
		NormalizeRect -- If the rectangle coordinates are reversed, swaps
			them
		TrackMouse -- Draws a rubberbanding rectangle until mouse button up
		Overlap -- Checks for overlapping rectangles; allows overlaps
			rectangles if the frames don't overlap.
		SelectHotspot -- Draw a highlighting rectangle
		PointOnLine - Determines whether the specified point lies on the
			specified line PixelVary is the amount of space allowed on
			either side of the line.
		SizeOK -- Checks if RECT is too small (it would be silly to allow
			1x1 rects)
		PtEdgeRect -- Determines whether the specified point is on the
			edge specified by the rect, including variable distance
			(specified by PixelVary). Returns:    0 = not on edge,
			1 = left, 2 = top, 3 = right, 4 = bottom.
		MoveHotspot -- Given specified POINT (where mouse was clicked),
			will either move a side of the hotspot (thus changing its size)
                or move the entire hotspot until user releases mouse button
		OnButtonDown -- Deals with mouse button being clicked.  Determines
			whther user is starting a new hotspot, moving a hotspot, or
            selecting a hotspot (i.e. for deletion)
		HotspotFromPoint -- Return a hotspot given a POINT.  Deals with
			appropriate frames.
		OnButtonDblClk -- If double click inside a hotspot, bring up
			Hotspot dialog so user can set options for hotspot.                
		DrawRects -- Draw the hotspot rectangles appropriate for current
			frame or all hotspot rects if frame checking disabled.
*/    

#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>

#include "hotspot.h"
#include "avihed.h"            /* specific to this program          */
#include "avihede.h"            // extern variable decls
#include "resource.h"

/* Macro to swap two values */
#define SWAP(x,y)   ((x)^=(y)^=(x)^=(y))

#define MINRECTWIDTH    5
#define MINRECTHEIGHT   5

// define VIEWER for use with VIEWER (i.e. for vwrCommand() )

/*****************************************************************************************
    FUNCTION:   ShowSelectInfo(RECT FAR *)
        
    PURPOSE:    Displays hotspot rect info in (modeless) SelectDlg
*****************************************************************************************/
void ShowSelectInfo(RECT FAR *prcRect)
{
    if (hwndSelectDlg)
        {
        if (!prcRect)
            {
            SetDlgItemInt(hwndSelectDlg, ID_LEFT, 0, FALSE);
            SetDlgItemInt(hwndSelectDlg, ID_TOP, 0, FALSE);
            SetDlgItemInt(hwndSelectDlg, ID_RIGHT, 0, FALSE);
            SetDlgItemInt(hwndSelectDlg, ID_BOTTOM, 0, FALSE);
            }        
        else
            {                      
            SetDlgItemInt(hwndSelectDlg, ID_LEFT, prcRect->left, FALSE);
            SetDlgItemInt(hwndSelectDlg, ID_TOP, prcRect->top, FALSE);
            SetDlgItemInt(hwndSelectDlg, ID_RIGHT, prcRect->right, FALSE);
            SetDlgItemInt(hwndSelectDlg, ID_BOTTOM, prcRect->bottom, FALSE);
            }        
        }            
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   :  DrawSelect(HDC hdc, BOOL fDraw)                           *
 *                                                                          *
 *  PURPOSE    :  Draws the selected clip rectangle with its dimensions on  *
 *                the DC/screen                                             *
 *                                                                          *
 ****************************************************************************/
void DrawSelect( hdc, prcClip, fDraw)
HDC hdc;
RECT FAR *prcClip;
BOOL fDraw;
{    

    HBITMAP hbmp;
    HBRUSH hbr, hbrPrevious;
    
    if (!IsRectEmpty (prcClip)) {  
        hbmp = LoadBitmap(hInst, "Pattern");
        hbr = CreatePatternBrush(hbmp);
    
        UnrealizeObject(hbr);
        hbrPrevious = SelectObject(hdc, hbr);

        /* If a rectangular clip region has been selected, draw it */
        PatBlt(hdc, prcClip->left,    prcClip->top,        prcClip->right-prcClip->left, 2,  PATINVERT);
        PatBlt(hdc, prcClip->left,    prcClip->bottom, 2, -(prcClip->bottom-prcClip->top),   PATINVERT);
        PatBlt(hdc, prcClip->right-2, prcClip->top, 2,   prcClip->bottom-prcClip->top,   PATINVERT);
        PatBlt(hdc, prcClip->right,   prcClip->bottom-2, -(prcClip->right-prcClip->left), 2, PATINVERT);
    
        SelectObject(hdc, hbrPrevious);
    
        DeleteObject(hbr);
        DeleteObject(hbmp);            
    }
}
/****************************************************************************
 *                                                                          *
 *  FUNCTION   : NormalizeRect(RECT *prc)                                   *
 *                                                                          *
 *  PURPOSE    : If the rectangle coordinates are reversed, swaps them      *
 *                                                                          *
 ****************************************************************************/
void PASCAL NormalizeRect (prc)
RECT *prc;
{
    if (prc->right < prc->left)
        SWAP(prc->right,prc->left);
    if (prc->bottom < prc->top)
        SWAP(prc->bottom,prc->top);
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : TrackMouse(HWND hwnd, POINT pt)                            *
 *                                                                          *
 *  PURPOSE    : Draws a rubberbanding rectangle until mouse button up      *
 *                                                                          *
 ****************************************************************************/
void TrackMouse (hwnd, pt, prcClip)
HWND hwnd;
POINT pt;
RECT *prcClip;
{
    HDC   hdc;
    MSG   msg;
    RECT  rcClient;    

    hdc = GetDC(hwnd);
    SetCapture(hwnd);

    GetClientRect(hwnd,&rcClient);

    /* Display the coordinates */
    DrawSelect(hdc, prcClip, FALSE);

    /* Initialize clip rectangle to the point */
    
    prcClip->left   = pt.x;
    prcClip->top    = pt.y;
    prcClip->right  = pt.x;
    prcClip->bottom = pt.y;

    /* Eat mouse messages until a WM_LBUTTONUP is encountered. Meanwhile
     * continue to draw a rubberbanding rectangle and display it's dimensions
     */
    for (;;){
    if (GetMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST)){

        DrawSelect(hdc,prcClip, FALSE);

            prcClip->left   = pt.x;
            prcClip->top    = pt.y;
            prcClip->right  = LOWORD(msg.lParam);
            prcClip->bottom = HIWORD(msg.lParam);

            NormalizeRect(prcClip);
            DrawSelect(hdc, prcClip, TRUE);
            ShowSelectInfo(prcClip);

            if (msg.message == WM_LBUTTONUP)
                break;
        }
    else
        continue;
    }

    ReleaseCapture();
    ReleaseDC(hwnd,hdc);
}

/*****************************************************************************************

    FUNCTION:   Overlap(PMOVIEINFO, RECT, PHOTSPOT)
    
    PURPOSE:    Checks for overlapping rectangles; allows overlaps rectangles
                if the frames don't overlap.  i.e., rect1 at 10,10,50,50
                where BeginFrame=0, EndFrame=10 and rect2 at 12,12,40,40
                where BeginFrame=11, EndFrame=125 is ok.
    
    PARAMS:     PMOVIEINFO  - far pointer to a MOVIEINFO struct
                RECT        - rectangle to check against all others
                PHOTSPOT    - far pointer to current hotspot (e.g. in case we're moving
                              a hotspot, don't check it against itself); this can be NULL
                              
    RETURNS:    TRUE        - rect overlaps another one
                FALSE       - rect does not overlap                              
*****************************************************************************************/                                                                       
BOOL Overlap(PMOVIEINFO pMovieInfo, RECT rc, PHOTSPOT pThisHotspot)
{

    PHOTSPOT pHotspot;
    RECT rcIntersect;
    
    pHotspot = pMovieInfo->pHotspotList;
    
    while (pHotspot)
        {
        
            if (pHotspot != pThisHotspot)
                {                
                if (IntersectRect(&rcIntersect, &pHotspot->rc, &rc))
                    {                 
                    if (!pThisHotspot)
                        {
                        if (pMovieInfo->dwCurrentFrame >= pHotspot->BeginFrame && 
                                    pMovieInfo->dwCurrentFrame <= pHotspot->EndFrame)
                            return TRUE;
                        }
                    else
                        {                        
                        if (pThisHotspot->EndFrame < pHotspot->BeginFrame ||
                                pThisHotspot->BeginFrame > pHotspot->EndFrame)
                            ;                                                                            
                        else                    
                            return TRUE;
                        }
                    }
                }
             
        pHotspot = pHotspot->pNext;            
        }
    return FALSE;        
}


/*****************************************************************************************
    FUNCTION:   SelectHotspot(PMOVIEINFO, PHOTSPOT)
    
    PURPOSE:    Draw a highlighting rectangle
    
    NOTES:      Not implemented; instead the SelectDlg is updated with the
                coordinates of the current rect.
*****************************************************************************************/
BOOL SelectHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot)
{    
    return TRUE;   
}

/*****************************************************************************************
    FUNCTION:   PointOnLine(POINT, int, int, int int)
    
    PURPOSE:    Determines whether the specified point lies on the specified line
    
    NOTES:      PixelVary is the amount of space allowed on either side of the line.
*****************************************************************************************/
BOOL PointOnLine(POINT pt, int x1, int y1, int x2, int y2)
{

//      old way of checking: doesn't allow variable space
//    if (pt.x <= x2 && pt.x >= x1 && pt.y <= y2 && pt.y >= y1)

    if ((pt.x >= x1 - PixelVary && pt.x <= x1 + PixelVary) ||
        (pt.x >= x2 - PixelVary && pt.x <= x2 + PixelVary) ||
        (pt.y >= y1 - PixelVary && pt.y <= y1 + PixelVary) ||
        (pt.y >= y2 - PixelVary && pt.y <= y2 + PixelVary))
        return (TRUE);
    else
        return (FALSE);        
}


/*****************************************************************************************
    FUNCTION:   SizeOK(RECT)
    
    PURPOSE:    Checks if RECT is too small (it would be silly to allow 1x1 rects)
*****************************************************************************************/
BOOL SizeOK(RECT rc)
{               
    if (rc.right - rc.left < MINRECTWIDTH)
        return FALSE;
    if (rc.bottom - rc.top < MINRECTHEIGHT)
        return FALSE;

    return TRUE;
}

/*****************************************************************************************
    FUNCTION:   PtEdgeRect(RECT, POINT)
    
    PURPOSE:    Determines whether the specified point is on the edge specified by
                the rect, including variable distance (specified by PixelVary).
                Returns:    0 = not on edge
                            1 = left
                            2 = top
                            3 = right
                            4 = bottom            
*****************************************************************************************/
int PtEdgeRect(RECT rc, POINT pt)
{
    int PtOnLine = 0;
    RECT rcTemp;

    SetRect(&rcTemp, rc.left - PixelVary, rc.top, rc.left + PixelVary, rc.bottom);
    if (PtInRect(&rcTemp, pt))        
        PtOnLine = 1;        
    SetRect(&rcTemp, rc.left, rc.top - PixelVary, rc.right, rc.top + PixelVary);
    if (PtInRect(&rcTemp, pt))
        PtOnLine = 2;
    SetRect(&rcTemp, rc.right - PixelVary, rc.top, rc.right + PixelVary, rc.bottom);
    if (PtInRect(&rcTemp, pt))
        PtOnLine = 3;
    SetRect(&rcTemp, rc.left, rc.bottom - PixelVary, rc.right, rc.bottom + PixelVary);
    if (PtInRect(&rcTemp, pt))
        PtOnLine = 4;        

    return (PtOnLine);    
}

/*****************************************************************************************
    FUNCTION:   MoveHotspot(PMOVIEINFO, PHOTSPOT, POINT)
    
    PURPOSE:    Given specified POINT (where mouse was clicked), will either
                move a side of the hotspot (thus changing its size)
                or move the entire hotspot until user releases mouse button
                
    RETURNS:    TRUE                            
*****************************************************************************************/
BOOL MoveHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot, POINT pt)
{
    MSG msg;
    RECT rc;
    HDC hDC;
    int PtOnLine = 0; // 0 = not on line, 1 = left, 2 = top, 3= right, 4= bottom
    //HCURSOR hcurSave;
    //RECT rcTemp;
    RECT rcOld;
    
    CopyRect(&rc, &pHotspot->rc);
    
    PtOnLine = PtEdgeRect(rc, pt);
    hDC = GetDC(pMovieInfo->hwndMovie);

    for (;;)
        {
        if (GetMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST))
            {            
            DrawSelect(hDC,&rc, FALSE);
            CopyRect(&rcOld, &rc);
                switch (PtOnLine)
                    {
                    case 0:                
                        rc.left   = rc.left + LOWORD(msg.lParam) - pt.x;
                        rc.top    = rc.top + HIWORD(msg.lParam) - pt.y;
                        rc.right  = rc.right + LOWORD(msg.lParam) - pt.x;
                        rc.bottom = rc.bottom + HIWORD(msg.lParam) - pt.y;
                        break;
                    
                    case 1:                    
                        rc.left   = rc.left + LOWORD(msg.lParam) - pt.x; 
                        break;
                
                    case 2:
                        rc.top    = rc.top + HIWORD(msg.lParam) - pt.y;
                        break;

                    case 3:
                        rc.right  = rc.right + LOWORD(msg.lParam) - pt.x;
                        break;
                    
                    case 4:                    
                        rc.bottom = rc.bottom + HIWORD(msg.lParam) - pt.y;
                        break;                                    
                    
                    }                
                pt.x = LOWORD(msg.lParam);
                pt.y = HIWORD(msg.lParam);
                NormalizeRect(&rc);
                DrawSelect(hDC, &rc, TRUE);            
                ShowSelectInfo(&rc);
                if (!EqualRect(&rcOld, &rc))
                    bModified = TRUE;
            if (msg.message == WM_LBUTTONUP)
                break;
            } // getmessage
        else
            continue;
        }
        
    
    if (!Overlap(pMovieInfo, rc, pHotspot) && SizeOK(rc))
        CopyRect(&pHotspot->rc, &rc);
    else
        {
        DrawSelect(hDC, &rc, TRUE);
        DrawSelect(hDC, &pHotspot->rc, TRUE);        
        }        
    
    ReleaseDC(pMovieInfo->hwndMovie, hDC);    
//    if (PtOnLine != 0)
//        SetCursor(hcurSave);
    
    return TRUE;

}

/*****************************************************************************************
    FUNCTION:   OnButtonDown(PMOVIEINFO, USHORT, UHSORT, LONG)
    
    PURPOSE:    Deals with mouse button being clicked.
                Posibilities:   - starting a new hotspot
                                - moving a hotspot
                                - selecting a hotspot (i.e. for deletion)
                                
    RETURNS:    0 = nothing happened (e.g. there was an overlap) or unable to alloc memory
                1 = no longer used
                2 = hotspot selected/drawn etc. (in any case, make the hotspot selected
                    in case of double click)                
*****************************************************************************************/
int OnButtonDown(PMOVIEINFO pMovieInfo, USHORT msg, USHORT wp, LONG lp)
{
    PHOTSPOT pHotspot, pHotspotSelected = NULL;
    DWORD dwFrame;                      
    POINT pt;    
    RECT rc;
    
    if (!pMovieInfo)
        return FALSE;
    
    pHotspot = pMovieInfo->pHotspotList;
    pt = MAKEPOINT(lp);
    dwFrame = GetMovieFrame(pMovieInfo);
    SetRectEmpty(&rc);

    if (!pHotspot)                          // none selected yet...
        {
        TrackMouse(pMovieInfo->hwndMovie, pt, &rc);
        if ((pHotspot = (PHOTSPOT)ALLOCATE(sizeof(HOTSPOT))))
             {            
            CopyRect(&pHotspot->rc, &rc);
            pHotspot->BeginFrame = pMovieInfo->dwCurrentFrame;
            pHotspot->EndFrame = pMovieInfo->dwMovieLength;        
            pHotspot->pszCommand = NULL;
            pHotspot->pszHotspotID = NULL;
            pHotspot->OnClick = ID_CONTINUE;
            AddHotspot(pMovieInfo, pHotspot);        
            bModified = TRUE;
            return (2);
            }
        }
    else
        {
        while (pHotspot)
        {        
        if (pMovieInfo->dwCurrentFrame >= pHotspot->BeginFrame && 
                    pMovieInfo->dwCurrentFrame <= pHotspot->EndFrame)        
            {
//            if (PtInRect(&pHotspot->rc, pt) != 0)
              if (PtEdgeRect(pHotspot->rc, pt) || PtInRect(&pHotspot->rc, pt))
                // if the point's in the rect, select the rect
                // otherwise, draw a new rect.
                // need more complicated code for good handling of overlapping rects
                {                                                
                pHotspotSelected = pHotspot;
                }                            
            }
        pHotspot = pHotspot->pNext;       
        } // while
        if (pHotspotSelected)
            {
            // we can move the rect, resize it, delete it, or if dlbclick, bring up dlg                        
            SelectHotspot(pMovieInfo, pHotspotSelected);
            MoveHotspot(pMovieInfo, pHotspotSelected, pt);
            ShowSelectInfo(&pHotspotSelected->rc);
                        
            return (2);            
            }
        else
            {
            TrackMouse(pMovieInfo->hwndMovie, pt, &rc);        
            if (Overlap(pMovieInfo, rc, NULL) || !SizeOK(rc))
                {
                HDC hDC;
            
                hDC = GetDC(pMovieInfo->hwndMovie);
                DrawSelect(hDC, &rc, TRUE);                 // un-invert the rect
                ReleaseDC(pMovieInfo->hwndMovie, hDC);
                ShowSelectInfo(NULL);
                return FALSE;
                }
            if (! (pHotspot = (PHOTSPOT)ALLOCATE(sizeof(HOTSPOT))))
                {
                return (FALSE);
                }
            else
                {
                CopyRect(&pHotspot->rc, &rc);
                pHotspot->BeginFrame = pMovieInfo->dwCurrentFrame;
                pHotspot->EndFrame = pMovieInfo->dwMovieLength;            
                pHotspot->pszCommand = NULL;
                pHotspot->pszHotspotID = NULL;
                pHotspot->OnClick = ID_CONTINUE;
                AddHotspot(pMovieInfo, pHotspot);        
                bModified = TRUE;
                return (2);
                }            
            }
        
        } // else
        
    return TRUE;        
}

/*****************************************************************************************
    FUNCTION:   HotspotFromPoint(PMOVIEINFO, POINT)
    
    PURPOSE:    Return a hotspot given a POINT.  Deals with appropriate frames.
*****************************************************************************************/
PHOTSPOT HotspotFromPoint(PMOVIEINFO pMovieInfo, POINT pt)
{
    PHOTSPOT pHotspot, pHotspotSelected = (PHOTSPOT)NULL;

    pHotspot = pMovieInfo->pHotspotList;
    
    while (pHotspot)
        {        
        if (bShowValidRectsOnly)
            {            
            if (pHotspot->BeginFrame <= pMovieInfo->dwCurrentFrame && 
                        pMovieInfo->dwCurrentFrame <= pHotspot->EndFrame)
                {
                if (PtInRect(&pHotspot->rc, pt) != 0)
                    pHotspotSelected = pHotspot;
                }
            }
        else
            {
            if (PtInRect(&pHotspot->rc, pt) != 0)
                pHotspotSelected = pHotspot;                
            }            
        pHotspot = pHotspot->pNext;
        } // while
        
    return (pHotspotSelected);
}

/*****************************************************************************************
    FUNCTION:   OnButtonDblClk(PMOVIEINFO, USHORT, unsigned, LONG)
    
    PURPOSE:    If double click inside a hotspot, bring up Hotspot dialog so
                user can set options for hotspot.                
*****************************************************************************************/
int OnButtonDblClk(PMOVIEINFO pMovieInfo, USHORT msg, unsigned wp, LONG lp)
{
    static DLGPROC dlgproc;
    POINT pt;
    PHOTSPOT pHotspotSelected = NULL, pHotspot;

    pt = MAKEPOINT(lp); 
    
    pHotspot = pMovieInfo->pHotspotList;
    
    while (pHotspot)
        {        
        if (bShowValidRectsOnly)
            {            
            if (pHotspot->BeginFrame <= pMovieInfo->dwCurrentFrame && 
                        pMovieInfo->dwCurrentFrame <= pHotspot->EndFrame)
                {
                if (PtInRect(&pHotspot->rc, pt) != 0)
                    pHotspotSelected = pHotspot;
                }
            }
        else
            {
            if (PtInRect(&pHotspot->rc, pt) != 0)
                pHotspotSelected = pHotspot;                
            }            
        pHotspot = pHotspot->pNext;
        } // while
    if (pHotspotSelected)
        {
        dlgproc = (DLGPROC) MakeProcInstance(HotspotDlg, hInst);
        DialogBoxParam(hInst, MAKEINTRESOURCE(HOTSPOTDLG), pMovieInfo->hwndParent, dlgproc,
                (LPARAM)pHotspotSelected);
        FreeProcInstance((FARPROC)dlgproc);
        InvalidateRect(pMovieInfo->hwndMovie, NULL, TRUE);
        UpdateWindow(pMovieInfo->hwndMovie);
        }
    return TRUE;
}

/*****************************************************************************************
    FUNCTION:   DrawRects(HDC, PMOVIEINFO)
    
    PURPOSE:    Draw the hotspot rectangles appropriate for current frame
                or all hotspot rects if frame checking disabled.
*****************************************************************************************/
void DrawRects(HDC hDC, PMOVIEINFO pMovieInfo)
{
    PHOTSPOT pHotspot;
    
    pHotspot = pMovieInfo->pHotspotList;
    
    while (pHotspot)
        {
        if (bShowValidRectsOnly)
            {
            if (pHotspot->BeginFrame <= pMovieInfo->dwCurrentFrame &&
                    pHotspot->EndFrame >= pMovieInfo->dwCurrentFrame)
                DrawSelect(hDC, &pHotspot->rc, TRUE);        
            }
        else
            DrawSelect(hDC, &pHotspot->rc, TRUE);
                                    
        pHotspot = pHotspot->pNext;
        }
}
