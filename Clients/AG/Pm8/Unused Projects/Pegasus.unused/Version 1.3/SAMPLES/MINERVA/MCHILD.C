/******************************************************************************
 *
 * MCHILD.C - Minerva MDI child windows' window procedure
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * Global functions:
 *   ChildGetActive(void)   - return window handle of active child if any and if enabled
 *   ChildInvalidatePalette - get rid of the current palette if any so a new palette
 *                            is created the next time one is needed
 *   ChildIsModified        - return TRUE if the window is marked modified -- which
 *                            means a transform opcode was used
 *   ChildQueryClose        - if the child has been modified, ask the user about
 *                            saving, return FALSE if the user cancels the save
 *   ChildQueryCloseAll     - ChildQueryClose for all child windows
 *   ChildWndProc           - window procedure for all MDI child windows
 *   ChildSetScrollRanges   - compute scroll bars' ranges according to window size
 *   ChildWaitCursorOn      - turn hourglass cursor on
 *   ChildWaitCursorOff     - turn hourglass cursor off
 *
 * Local functions:
 *   OnDestroy              - WM_DESTROY - clean up window and image instance data
 *   OnPaint                - WM_PAINT - all image drawing
 *   OnPaletteChanged       - WM_PALETTE_CHANGED - re-realize image palette after
 *                            a system palette change
 *   OnHScroll              - WM_HSCROLL processing - horizontal scroll bar messages
 *   OnVScroll              - WM_VSCROLL processing - vertical scroll bar messages
 *   OnRClick               - processing WM_xRBUTTONUP following WM_xRBUTTONDOWN
 *                            to popup an image menu
 *   ClipWindowSize         - make sure the window isn't bigger than the image
 *   PaletteFromBitmapInfo  - create an HPALETTE from a bitmap colortable
 *   NewScrollPos           - worker function for OnVScroll and OnHScroll
 *
 * Revision History:
 *   04-21-97  1.00.21  jrb  changed paint procedure so a non-mazimized image isn't
 *                             always painted at 0,0 instead of centered if the window
 *                             is larger than some image dimension.  Else, when a border
 *                             were dragged so the centered image offset were changing,
 *                             the some repaint image segments would be offset from prior
 *                             paints.  Else, to fix it would mean saving the last tlc
 *                             point and keeping it the same while sizing.  Not hard, but
 *                             also probably not worth it.
 *                           first time through ClipWindowSize, allow it to increase the
 *                             window size up to the smaller of the image size or the
 *                             client area rectangle.  Thus most images will initially appear
 *                             in a window the same size as the image
 *   11-11-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#pragma warning(disable:4001)
/* PIC includes */
#include "pic.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "mchild.h"
#include "mmenu.h"
#include "mopen.h"
#include "merror.h"
#include "msave.h"
#include "mmisc.h"



#define MAX_LINESCROLLINCREMENT     ( 5 )
#define TIME_NEXTSCROLLINCREMENT    ( 1000 )



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static int HScrollHeight(void);
static int VScrollWidth(void);
static void OnGetMinMaxInfo(HWND hWnd, MINMAXINFO PICFAR* p, CHILDINSTANCEDATA PICFAR* pInstance);
static void OnDestroy(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance);
static void OnPaint(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance);
static void OnPaletteChanged(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance);
static void OnVScroll(
    HWND hWnd,
    HWND hwndCtrl,
    UINT uScrollCode,
    int  nPos);
static void OnHScroll(
    HWND hWnd,
    HWND hwndCtrl,
    UINT uScrollCode,
    int  nPos);
static void OnRClick(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance, int xPos, int yPos);
static HPALETTE PaletteFromBitmapInfo(const BITMAPINFO PICFAR* pbmi);
static int NewScrollPos(
    CHILDINSTANCEDATA PICFAR* pInstance,
    UINT uScrollCode,
    int nTrackPos,
    int nCurPos,
    int nTotal,
    int nPage);
static void ClipWindowSize(HWND hWnd);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  void ChildSetDibHeight(HWND hwnd, LONG lHeight)
 * 
 *  set height of dib buffer with pixels set (- from end, + from start)
 *
 *  parameters:
 *    hwnd    - window for wait cursor
 *    lHeight - height set
 ******************************************************************************/
void ChildSetDibHeight(HWND hwnd, LONG lHeight)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    LONG lOldHeight;
    RECT rect;
    int xOfs;
    int yOfs;
   
    if ( pInstance == 0 )
        return;    
    lOldHeight = pInstance->lDibHeight;
    pInstance->lDibHeight = lHeight;
    if ( lOldHeight < 0 && lHeight > 0 )
        lHeight = -lHeight;
    if ( lHeight == lOldHeight )
        return;

    if ( labs(lHeight) < labs(lOldHeight) )
        lOldHeight = 0;
    GetClientRect(hwnd, &rect);
    xOfs = 0;
    yOfs = 0;
    if ( pInstance->DibPicParm.Head.biWidth < rect.right )
        xOfs = ( rect.right - (int)pInstance->DibPicParm.Head.biWidth ) / 2;
    if ( labs(pInstance->DibPicParm.Head.biHeight) < rect.bottom )
        yOfs = ( rect.bottom - (int)labs(pInstance->DibPicParm.Head.biHeight) ) / 2;

    if ( lHeight < 0 )
        {
        rect.top    = yOfs - (int)lOldHeight;
        rect.bottom = yOfs - (int)lHeight;
        }
    else
        {
        rect.top    = yOfs + (int)labs(pInstance->DibPicParm.Head.biHeight) - (int)lHeight;
        rect.bottom = yOfs + (int)labs(pInstance->DibPicParm.Head.biHeight) - (int)lOldHeight;
        }        
    rect.left  = xOfs;
    rect.right = xOfs + (int)pInstance->DibPicParm.Head.biWidth;        

    InvalidateRect(hwnd, &rect, FALSE);
    UpdateWindow(hwnd);
}



/******************************************************************************
 *  void ChildWaitCursorOn(HWND hwnd)
 * 
 *  turn wait cursor on
 *
 *  parameters:
 *      hwnd - window for wait cursor
 ******************************************************************************/
void ChildWaitCursorOn(HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    
    if ( pInstance == 0 || pInstance->nWaitCursor++ != 0 || nWaitCursor != 0 )
        return;    
    // really turning it on
    pInstance->hOldCursor = GetCursor();
    SetCursor(hWaitCursor);
}



/******************************************************************************
 *  void ChildWaitCursorOff(HWND hwnd)
 * 
 *  turn wait cursor off
 *
 *  parameters:
 *      hwnd - window for restored cursor
 ******************************************************************************/
void ChildWaitCursorOff(HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);

    if ( pInstance == 0 ||
         pInstance->nWaitCursor == 0 ||
         --pInstance->nWaitCursor != 0 ||
         nWaitCursor != 0 )
        return;    
    // really turning it on
    SetCursor(pInstance->hOldCursor);
}



/******************************************************************************
 *  HWND ChildGetActive(void)
 * 
 *  return window handle of active child if any and if enabled
 ******************************************************************************/
HWND ChildGetActive(void)
{
    HWND hwnd = FORWARD_WM_MDIGETACTIVE(hwndMDIClient, SendMessage);
    if ( !IsWindow(hwnd) )
        hwnd = NULL;
    return ( hwnd );
}



/******************************************************************************
 *  void ChildInvalidatePalette(HWND hwnd)
 * 
 *  so each open opcode can invalidate the child's current palette when color
 *  palette image options are changed
 *
 *  parameters:
 *      hwnd - child window handle
 *
 *  notes:
 *      I'm doing it like this so opcodes still don't have to know about child
 *      window instance data
 *
 *      By deleting the old one, if any, and nulling its HPALETTE, the next
 *      time the palette is needed by a WM_PAINT or palette change, it will
 *      be recreated from the apparently new colortable
 ******************************************************************************/
void ChildInvalidatePalette(HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);

    if ( pInstance != 0 && pInstance->hPalette != 0 )
        {
        DeleteObject(pInstance->hPalette);
        pInstance->hPalette = 0;
        }   
}



/******************************************************************************
 *  BOOL ChildIsModified(void)
 * 
 *  return TRUE if the window is marked modified
 *
 *  returns:  
 *      TRUE if instance data modified flag is set
 *
 *  notes:
 *      instance data modified flag is set if a transform opcode has been applied
 *      to the image
 ******************************************************************************/
BOOL ChildIsModified(HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    
    return ( pInstance == 0 || pInstance->bModified );    
}



/******************************************************************************
 *  BOOL ChildQueryClose(HWND hwnd)
 * 
 *  if the child has been modified, ask the user about saving, and Save if the
 *  user wants to
 *
 *  parameters:
 *      hwnd - HWND for child window being queried
 *
 *  returns:  
 *      TRUE if the child window isn't modified or if the user doesn't cancel
 *      out of the "OK to save modified data" dialog box and no error occurs
 *      saving the image.
 *
 *  notes:
 *      instance data modified flag is set if a transform opcode has been applied
 *      to the image.  A successful save will clear the flag.
 ******************************************************************************/
BOOL ChildQueryClose(HWND hwnd)
{
    int result;
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    BYTE PICHUGE* pSavedImage;
    DWORD dwSavedLen;
    int i;
    char szFilename[_MAX_PATH + 1];
    SAVEOP *pSaveOp;

    if ( pInstance == 0 )
        return ( TRUE );
    if ( pInstance->nWaitCursor != 0 || nWaitCursor != 0 )
        {
        //#### TODO it would be nice to let the user cancel the operation in
        //#### progress and let the close happen.
        ErrorMessage(STYLE_ERROR, IDS_OPCODEBUSY);
        return ( FALSE );
        }

    if ( !ChildIsModified(hwnd) )
        return ( TRUE );

    for ( i = 0; pSaveOpTable[i] != 0 && pSaveOpTable[i]->nFoundParmVer == 0; i++ )
        ;
    if ( pSaveOpTable[i] == 0 )
        return ( TRUE );    /* it's ok to close if there's no save opcode */

    result = ErrorMessage(
        MB_YESNOCANCEL | MB_ICONQUESTION,
        IDS_IMAGEMODIFIED,
        (LPCSTR)pInstance->pszFilename);
    /* "Save changes to image %s?" */        
    if ( result == IDNO )
        return ( TRUE );
    if ( result != IDYES )
        return ( FALSE );
            
    pSaveOp = pInstance->pOpenOp->pSaveOp;
    lstrcpy(szFilename, pInstance->pszFilename);        
    if ( ( pSaveOp == 0 || pSaveOp->nFoundParmVer == 0 ) && !SaveGetSaveAs(szFilename, &pSaveOp) )
        return ( FALSE );

    if ( !SaveImage(hwnd, szFilename, pSaveOp, &pSavedImage, &dwSavedLen) )
        return ( FALSE );            

    /* if TRUE, then save image will have allocated a buffer for a memory image of the saved
        image file in case we wanted to open a window for that saved image without rereading
        the file */
     MiscGlobalFreePtr(&pSavedImage);
    return ( TRUE );        
}



/******************************************************************************
 *  BOOL ChildQueryCloseAll(void)
 * 
 *  ChildClose for all child windows
 *
 *  returns:  
 *      TRUE if ChildQueryClose returns TRUE for all child windows
 *
 *  notes:
 *      I include the Enum... proc here because it's not called by anybody else
 *      and it's easier for me to understand if it's right here.
 ******************************************************************************/

/* return FALSE if any window refuses the close request (because the user
    cancelled out of the "Save changes" dialog box) */
static BOOL CALLBACK EnumChildProcQuery(HWND hwnd, LPARAM lParam)
{
    char sz[sizeof(MDICHILD_CLASSNAME) + 1];

    NOREFERENCE(lParam);
    /* some of the children of the client are not actually our mdi child windows
        For example, if a window is iconized, the icon legend is a child of the
        client.  For that reason, we compare the enum'ed window class name with
        the one we established for mdi children to make sure it's one of ours */
    GetClassName(hwnd, sz, sizeof(sz));
    if ( strcmp(sz, MDICHILD_CLASSNAME) == 0 )
        return ( ChildQueryClose(hwnd) );   /* stops the enum at the first FALSE return */
    return ( TRUE );
}

BOOL ChildQueryCloseAll(void)
{
    if ( GetWindow(hwndMDIClient, GW_CHILD) == NULL )
        {
        /* otherwise Enum... would return FALSE if there were no children
            and logically, if there are no children, then all children can be closed */
        return ( TRUE );
        }
    return ( EnumChildWindows(hwndMDIClient, EnumChildProcQuery, 0) );
}



/******************************************************************************
 *  int ChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
 * 
 *  window procedure for all MDI child windows
 *
 *  parameters:
 *      hWnd - handle to this MDI child window
 *      uMsg - message id
 *      wParam, lParam - depends on uMsg
 *
 *  returns:  
 *      depends on uMsg, ordinarily 0 if we handle the message
 *
 *  notes:
 *      ChildGetInstanceData just casts the result of GetWindowLong(hWnd, 0)
 *      to a pointer to the child instance data.  While the pointer is null, we
 *      haven't started loading an image yet and all messsages except WM_CREATE and
 *      WM_PAINT are passed to DefMDIChildProc.  WM_CREATE allows us to make sure
 *      no other child window has the same title appending ":<number>" if needed.
 *      WM_PAINT allows us to wash the background color in the window client area.
 *      OnPaint describes why the child class registration didn't define a background
 *      brush.
 *
 *      When the pointer isn't NULL, the source image data will be valid, but the
 *      DIB data still may not be if a message box happens to be displayed before
 *      the DIB is allocated.
 *
 *      note specifically that the windows doc suggests strongly that WM_SIZE
 *      (along with others we don't care about) be passed to DefMDIChildProc.
 *
 *      I include the Enum... proc here because it's not called by anybody else
 *      and it's easier for me to understand if it's right here.
 ******************************************************************************/

/* return TRUE from EnumChildWindows if any window has the same title */
static BOOL CALLBACK EnumChildProcFind(HWND hwnd, LPARAM lParam)
{
    LPSTR psz = (LPSTR)lParam;
    char sz[_MAX_PATH + sizeof(":65536")];
    
    GetWindowText(hwnd, sz, sizeof(sz));
    return ( lstrcmp(sz, psz) != 0 );
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CHILDINSTANCEDATA PICFAR *pInstance = ChildGetInstanceData(hWnd);
    CREATESTRUCT PICFAR* pcs;
    MDICREATESTRUCT PICFAR *pmcs;
    char sz[_MAX_PATH + sizeof(":65536")];
    char *pszName;
    char *psz;
    int n;
    POINT pt;
    int x;
    int y;
    int xDelta;
    int yDelta;
    RECT rect;
    RECT rectClip;
    int xOfs;
    int yOfs;

    if ( pInstance == 0 )
        {
        /* the window doesn't have instance data during some period of time while
            it is being created */
        switch ( uMsg )
            {
            case WM_CREATE:
                SetScrollRange(hWnd, SB_HORZ, 0, 0, FALSE);
                SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);

                pcs = (CREATESTRUCT PICFAR*)lParam;
                assert(pcs->lpCreateParams != 0);
                pmcs = (MDICREATESTRUCT PICFAR*)pcs->lpCreateParams;                
                if ( pmcs->lParam != 0 )
                    SetWindowLong(hWnd, 0, pmcs->lParam);

                GetWindowText(hWnd, sz, sizeof(sz));

                /*#### TODO: change this so a new window gets a number larger than
                    any existing window and so the window without a number gets number 1
                    -- it would also be nice somehow (magic) to have windows have names
                        which kind of connects windows with the same path, while making it
                    clear they are different from other windows with the same name
                    but different paths */
                pszName = strrchr(sz, '\\');
                assert(pszName != NULL);
                if ( pszName != NULL )
                    pszName++;
                else
                    pszName = sz;
                psz = sz + strlen(sz);
                n = 2;
                while ( !EnumChildWindows(
                            hwndMDIClient,
                            EnumChildProcFind,
                            (LPARAM)(LPSTR)pszName) )
                    wsprintf(psz, ":%u", n++);
                SetWindowText(hWnd, pszName);
                break;

            default:
                break;
            }
        return ( DefMDIChildProc(hWnd, uMsg, wParam, lParam) );
        }
    /* else the window has instance data (although the dib may not be defined yet) */
    
    switch ( uMsg )
        {
        case WM_NCLBUTTONDOWN:
            /* it doesn't matter whether it's on a scroll bar or not so long as
                the following occurs every time it _is_ on a scroll bar */
            pInstance->nLineScrollIncrement = 1;
            pInstance->dwTimeNextLineScrollIncrement = MiscTickCount() + TIME_NEXTSCROLLINCREMENT;
            break;

        case WM_NCRBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if ( pInstance->nWaitCursor == 0 && nWaitCursor == 0 )
                {
                /* tell the MDI client window to make this child window the active one,
                    default behavior on right click is not to do anything */
                SendMessage(hwndMDIClient, WM_MDIACTIVATE, (WPARAM)hWnd, 0);

                /* folowing so RBUTTONUP knows it's a click, otherwise a button down outside
                    the window followed by a button up inside the window would act like
                    a click in the window -- I think we could save the flag by checking
                    in buttonup to see if this window had captured the mouse, but I don't
                    trust that as well as knowing for sure with the flag */
                pInstance->bRButtonDown = TRUE;
                SetCapture(hWnd);
                }
            break;
                
        case WM_NCRBUTTONUP:
        case WM_RBUTTONUP:
            if ( pInstance->bRButtonDown )
                {
                pInstance->bRButtonDown = FALSE;
                ReleaseCapture();
                OnRClick(hWnd, pInstance, (int)LOWORD(lParam), (int)HIWORD(lParam));
                }
            break;

        case WM_LBUTTONDOWN:
            if ( pInstance->nWaitCursor == 0 && nWaitCursor == 0 )
                {
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);
                GetClientRect(hWnd, &rect);
                pInstance->ptMouse = pt;
                rectClip = rect;

                xOfs = 0;
                yOfs = 0;
                if ( IsZoomed(hWnd) )
                    {
                    if ( pInstance->DibPicParm.Head.biWidth < rect.right )
                        xOfs = ( rect.right - (int)pInstance->DibPicParm.Head.biWidth ) / 2;
                    if ( labs(pInstance->DibPicParm.Head.biHeight) < rect.bottom )
                        yOfs = ( rect.bottom - (int)labs(pInstance->DibPicParm.Head.biHeight) ) / 2;
                    }
                if ( xOfs != 0 )
                    {
                    if ( pt.x < xOfs || xOfs + pInstance->DibPicParm.Head.biWidth <= pt.x )
                        break;
                    rectClip.left = pt.x;
                    rectClip.right = pt.x;
                    }
                else
                    {                    
                    if ( rect.right - pt.x >= pInstance->xWindowOrg )
                        /* the following +1 is needed because otherwise if xWindowOrg == 0, the
                            cursor will be confined such that pt.x can't be reached */
                        rectClip.right = pInstance->xWindowOrg + pt.x + 1;
                    if ( pInstance->xWindowOrg + pt.x + rect.right > pInstance->DibPicParm.Head.biWidth )
                        rectClip.left = ( pInstance->xWindowOrg + pt.x + rect.right ) -
                            (int)pInstance->DibPicParm.Head.biWidth;
                    }

                if ( yOfs != 0 )
                    {
                    if ( pt.y < yOfs || yOfs + labs(pInstance->DibPicParm.Head.biHeight) <= pt.y )
                        break;
                    rectClip.top = pt.y;
                    rectClip.bottom = pt.y;
                    }
                else
                    {
                    if ( rect.bottom - pt.y >= pInstance->yWindowOrg )
                        rectClip.bottom = pInstance->yWindowOrg + pt.y + 1;
                    if ( pInstance->yWindowOrg + pt.y + rect.bottom > labs(pInstance->DibPicParm.Head.biHeight) )
                        rectClip.top = ( pInstance->yWindowOrg + pt.y + rect.bottom ) -
                            (int)labs(pInstance->DibPicParm.Head.biHeight);
                    }

                pt.x = rectClip.left;
                pt.y = rectClip.top;
                ClientToScreen(hWnd, &pt);
                rectClip.left = pt.x;
                rectClip.top = pt.y;
            
                pt.x = rectClip.right;
                pt.y = rectClip.bottom;
                ClientToScreen(hWnd, &pt);
                rectClip.right = pt.x;
                rectClip.bottom = pt.y;

                pInstance->bLButtonDown = TRUE;
                ClipCursor(&rectClip);
                SetCursor(hHandCursor);
                SetCapture(hWnd);
                }
            break;
                
        case WM_LBUTTONUP:
            if ( pInstance->bLButtonDown )
                {
                pInstance->bLButtonDown = FALSE;
                if ( pInstance->nWaitCursor != 0 || nWaitCursor != 0 )
                    SetCursor(hWaitCursor);
                else
                    SetCursor(hArrowCursor);
                ClipCursor(NULL);
                ReleaseCapture();
                }
            break;

        case WM_DESTROY:
            OnDestroy(hWnd, pInstance);
            break;

        case WM_PAINT:
            OnPaint(hWnd, pInstance);
            return ( 0 );

        case WM_PALETTECHANGED:
            OnPaletteChanged(hWnd, pInstance);
            return ( 0 );

        case WM_MDIACTIVATE:
            /* if this window is becoming the active window, forward the event
                to the frame window so it can change the system palette to this
                window's palette */
            if ( wParam != 0 )
                SendMessage(hwndFrame, WM_MDIACTIVATE, (WPARAM)hWnd, 0);
            /* else this window is becoming inactive and the frame doesn't care */                
            break;
            
        case WM_SIZE:
            ClipWindowSize(hWnd);
            ChildSetScrollRanges(hWnd);
            break;

        case WM_SYSCOMMAND:
            switch ( wParam & 0xfff0 )
                {
                case SC_SIZE:                
                    pInstance->bSizing = TRUE;
                    ChildSetScrollRanges(hWnd);
                    DefMDIChildProc(hWnd, uMsg, wParam, lParam);
                    pInstance->bSizing = FALSE;
                    ChildSetScrollRanges(hWnd);
                    ClipWindowSize(hWnd);
                    return ( 0 );

                case SC_MAXIMIZE:
                case SC_RESTORE:
                    // since we effectively change our image origin when maximized or restored,
                    // we have to _force_ the window repaint to include the entire window --
                    // else win '95 (at least) optimizes away some of the regions
                    DefMDIChildProc(hWnd, uMsg, wParam, lParam);
                    InvalidateRect(hWnd, NULL, FALSE);
                    UpdateWindow(hWnd);
                    return ( 0 );
                
                default:
                    break;                    
                }
            break;

        case WM_GETMINMAXINFO:
            OnGetMinMaxInfo(hWnd, (MINMAXINFO FAR*)lParam, pInstance);
            break;

        case WM_VSCROLL:
            HANDLE_WM_VSCROLL(hWnd, wParam, lParam, OnVScroll);
            return ( 0 );
 
        case WM_HSCROLL:
            HANDLE_WM_HSCROLL(hWnd, wParam, lParam, OnHScroll);
            return ( 0 );

        case WM_CLOSE:
            if ( !ChildQueryClose(hWnd) )
                return ( 0 );
            break;
            
        case WM_MOUSEMOVE:
            if ( pInstance->nWaitCursor != 0 || nWaitCursor != 0 )
                SetCursor(hWaitCursor);
            else if ( pInstance->bLButtonDown )
                {
                SetCursor(hHandCursor);
                GetClipCursor(&rectClip);
                pt.x = LOWORD(lParam);
                pt.y = HIWORD(lParam);
                GetClientRect(hWnd, &rect);
                /* scroll as appropriate */
                if ( pt.x < 0 )
                    pt.x = 0;
                if ( pt.x > rect.right )
                    pt.x = rect.right;
                if ( pt.y < 0 )
                    pt.y = 0;
                if ( pt.y > rect.bottom )
                    pt.y = rect.bottom;
                xDelta = pInstance->ptMouse.x - pt.x;
                yDelta = pInstance->ptMouse.y - pt.y;

                if ( rectClip.left != rectClip.right && xDelta != 0 )
                    {
                    x = pInstance->xWindowOrg + xDelta;
                    if ( x < 0 )
                        x = 0;
                    if ( x + rect.right > pInstance->DibPicParm.Head.biWidth )
                        x = (UINT)pInstance->DibPicParm.Head.biWidth - rect.right;
                    }
                else
                    x = pInstance->xWindowOrg;

                if ( rectClip.top != rectClip.bottom && yDelta != 0 )
                    {
                    y = pInstance->yWindowOrg + yDelta;
                    if ( y < 0 )
                        y = 0;
                    if ( y + rect.bottom > labs(pInstance->DibPicParm.Head.biHeight) )
                        y = (UINT)labs(pInstance->DibPicParm.Head.biHeight) - rect.bottom;
                    }
                else
                    y = pInstance->yWindowOrg;                    

                if ( x != pInstance->xWindowOrg || y != pInstance->yWindowOrg )
                    {
                    UpdateWindow(hWnd);
                    xDelta = pInstance->xWindowOrg - x;
                    yDelta = pInstance->yWindowOrg - y;
                    pInstance->xWindowOrg = x;
                    pInstance->yWindowOrg = y;
                    ScrollWindow(hWnd, xDelta, yDelta, NULL, NULL);
                    UpdateWindow(hWnd);
                    SetScrollPos(hWnd, SB_HORZ, pInstance->xWindowOrg, TRUE);
                    SetScrollPos(hWnd, SB_VERT, pInstance->yWindowOrg, TRUE);
                    }
                pInstance->ptMouse = pt;
                
                }
            else
                SetCursor(hArrowCursor);
            break;

        default:
            break;            
        }
 
    return ( DefMDIChildProc(hWnd, uMsg, wParam, lParam) );
}



/******************************************************************************
 *  void ChildSetScrollRanges(HWND hWnd)
 * 
 *  compute scroll bars' ranges according to window size
 *
 *  parameters:
 *      hWnd      - handle to this MDI child window
 *
 *  notes:
 *      The only tricky part is when the size changes so that a scroll bar has to 
 *      be made visible or made invisible.  When that happens, the client area rectangle
 *      changes again and ChildSetScrollRanges is reentered.  For that reason, we count
 *      ChildSetScrollRanges calls in the static local variable nOnSizeCount and record
 *      the count when entered in nCountAtEntry.  If nCountAtEntry is different from
 *      nOnSizeCount, we know ChildSetScrollRanges was reentered and get out without
 *      doing any further processing.
 ******************************************************************************/
void ChildSetScrollRanges(HWND hWnd)
{
    CHILDINSTANCEDATA PICFAR *pInstance = ChildGetInstanceData(hWnd);
    RECT rect;
    int nWidth;
    int nHeight;
    int nHMin;
    int nHMax;
    int nVMin;
    int nVMax;

    if ( pInstance != 0 &&
         pInstance->DibPicParm.Head.biWidth != 0 &&
         pInstance->DibPicParm.Head.biHeight != 0 )
        {
        if ( pInstance->bDisableSetScrollRanges )
            return;
        GetClientRect(hWnd, &rect);
        if ( rect.right == 0 || rect.bottom == 0 )
            return;
        pInstance->bDisableSetScrollRanges = TRUE;
        
        GetScrollRange(hWnd, SB_HORZ, &nHMin, &nHMax);
        assert(nHMin == 0);
        GetScrollRange(hWnd, SB_VERT, &nVMin, &nVMax);
        assert(nVMin == 0);
        nWidth = rect.right;
        nHeight = rect.bottom;
        if ( nHMax != 0 )
            nHeight += HScrollHeight();
        if ( nVMax != 0 )
            nWidth += VScrollWidth();
        if ( !pInstance->bSizing && pInstance->DibPicParm.Head.biWidth <= nWidth )
            {
            if ( labs(pInstance->DibPicParm.Head.biHeight) > nHeight )
                {
                // there will be a vertical scroll bar
                nWidth -= VScrollWidth();
                if ( pInstance->DibPicParm.Head.biWidth > nWidth )
                    // there will be a horizontal scroll bar
                    nHeight -= HScrollHeight();
                }
            // else nWidth and nHeight are what they will be when we're done
            }
        else
            {
            // there will be a horizontal scrollbar
            nHeight -= HScrollHeight();
            if ( pInstance->bSizing || labs(pInstance->DibPicParm.Head.biHeight) > nHeight )
                // there will be a vertical scroll bar
                nWidth -= VScrollWidth();
            }

        /**********************************************************************
           evaluate horizontal scroll bar range
        */
        if ( pInstance->bSizing || pInstance->DibPicParm.Head.biWidth > nWidth )
            {
            /* and need a visible scroll bar */
            if ( pInstance->DibPicParm.Head.biWidth > nWidth )
                SetScrollRange(
                    hWnd,
                    SB_HORZ,
                    0,
                    (int)pInstance->DibPicParm.Head.biWidth - nWidth,
                    TRUE);
            else
                SetScrollRange(hWnd, SB_HORZ, 0, 1000, TRUE);
                                
            /* if the new scroll range means that we are less than one width from the right edge
                of the image, scroll left so the rightmost image width is exactly */
            if ( pInstance->xWindowOrg != 0 &&
                 pInstance->xWindowOrg > pInstance->DibPicParm.Head.biWidth - nWidth )
                {
                pInstance->xWindowOrg = (int)pInstance->DibPicParm.Head.biWidth - nWidth;
                SetScrollPos(hWnd, SB_HORZ, pInstance->xWindowOrg, TRUE);
                InvalidateRect(hWnd, NULL, FALSE);
                }
            }
        else
            {
             /* else make scroll bar invisible */
            SetScrollRange(hWnd, SB_HORZ, 0, 0, FALSE);
            /* set the horizontal scroll position to 0 (not scrolled) */
            SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
            if ( pInstance->xWindowOrg != 0 )
                {
                /* but only refresh if the scroll position is different now */
                pInstance->xWindowOrg = 0;
                InvalidateRect(hWnd, NULL, FALSE);
                }
            }

        /**********************************************************************
           evaluate vertical scroll bar range
        */
        if ( pInstance->bSizing || labs(pInstance->DibPicParm.Head.biHeight) > nHeight )
            {
            /* and need a visible scroll bar */
            if ( labs(pInstance->DibPicParm.Head.biHeight) > nHeight )
                SetScrollRange(
                    hWnd,
                    SB_VERT,
                    0,
                    (int)labs(pInstance->DibPicParm.Head.biHeight) - nHeight,
                    TRUE);
            else
                SetScrollRange(hWnd, SB_VERT, 0, 1000, TRUE);

            /* if the new scroll range means that we are less than one page from the bottom
                of the image, scroll up so the last image page is exactly displayed */
            if ( pInstance->yWindowOrg != 0 &&
                 pInstance->yWindowOrg > labs(pInstance->DibPicParm.Head.biHeight) - nHeight )
                {
                pInstance->yWindowOrg = (int)labs(pInstance->DibPicParm.Head.biHeight) - nHeight;
                SetScrollPos(hWnd, SB_VERT, pInstance->yWindowOrg, TRUE);
                InvalidateRect(hWnd, NULL, FALSE);
                }
            }
        else
            {
             /* else make scroll bar invisible */
            SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);
            /* set the vertical scroll position to 0 (not scrolled) */
            SetScrollPos(hWnd, SB_VERT, 0, TRUE);
            if ( pInstance->yWindowOrg != 0 )
                {
                /* but only refresh if the scroll position is different now */
                pInstance->yWindowOrg = 0;
                InvalidateRect(hWnd, NULL, FALSE);
                }
            }
        pInstance->bDisableSetScrollRanges = FALSE;
        }
}



/******************************************************************************
 *  void ChildSetWindowSize(HWND hWnd)
 * 
 *  make sure the window is the right size for the image considering the desktop size
 *
 *  parameters:
 *      hWnd - handle to this MDI child window
 ******************************************************************************/
void ChildSetWindowSize(HWND hWnd)
{
    CHILDINSTANCEDATA PICFAR *pInstance = ChildGetInstanceData(hWnd);
    RECT rect;
    int nHAdjust = 0;
    int nVAdjust = 0;
    int nHeight;
    int nWidth;
    POINT pt;

    if ( pInstance == 0 || pInstance->bSizing )
        return;
    if ( IsZoomed(hWnd) )
        {
        ChildSetScrollRanges(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        ShowWindow(hWnd, SW_SHOW);
        return;        
        }
    
    LockWindowUpdate(hwndMDIClient);
    ShowWindow(hWnd, SW_HIDE);

    SetScrollRange(hWnd, SB_HORZ, 0, 1000, TRUE);
    SetScrollRange(hWnd, SB_VERT, 0, 1000, TRUE);

    GetClientRect(hWnd, &rect);
    if ( rect.right != pInstance->DibPicParm.Head.biWidth )
        nHAdjust = rect.right - (int)pInstance->DibPicParm.Head.biWidth;
    if ( rect.bottom != labs(pInstance->DibPicParm.Head.biHeight) )
        nVAdjust = rect.bottom - (int)labs(pInstance->DibPicParm.Head.biHeight);
    if ( nHAdjust != 0 || nVAdjust != 0 )
        {
        GetWindowRect(hWnd, &rect);
        nWidth = rect.right - rect.left - nHAdjust;
        nHeight = rect.bottom - rect.top - nVAdjust;
        pt.x = rect.left;
        pt.y = rect.top;
        ScreenToClient(hwndMDIClient, &pt);
        GetClientRect(hwndMDIClient, &rect);
        if ( nWidth > rect.right )
            nWidth = rect.right;
        if ( nHeight > rect.bottom )
            nHeight = rect.bottom;
        MoveWindow(hWnd, pt.x, pt.y, nWidth, nHeight, TRUE);
        }

    ChildSetScrollRanges(hWnd);
    ShowWindow(hWnd, SW_SHOW);
    LockWindowUpdate(NULL);
    InvalidateRect(hwndMDIClient, NULL, FALSE);
    UpdateWindow(hwndMDIClient);
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



static int HScrollHeight(void)
{
#ifdef WIN32
    return ( GetSystemMetrics(SM_CYHSCROLL) );
#else
    return ( GetSystemMetrics(SM_CYHSCROLL) - 1 );
#endif
}



static int VScrollWidth(void)
{
#ifdef WIN32
    return ( GetSystemMetrics(SM_CXVSCROLL) );
#else
    return ( GetSystemMetrics(SM_CXVSCROLL) - 1 );
#endif
}



/******************************************************************************
 *  void ClipWindowSize(HWND hWnd)
 * 
 *  make sure the window isn't bigger than the image
 *
 *  parameters:
 *      hWnd - handle to this MDI child window
 ******************************************************************************/
static void ClipWindowSize(HWND hWnd)
{
    CHILDINSTANCEDATA PICFAR *pInstance = ChildGetInstanceData(hWnd);
    RECT rect;
    int nHAdjust = 0;
    int nVAdjust = 0;
    int nHeight;
    int nWidth;
    POINT pt;

    if ( pInstance == 0 || pInstance->bSizing || IsZoomed(hWnd) )
        return;

    GetClientRect(hWnd, &rect);
    if ( rect.right != pInstance->DibPicParm.Head.biWidth )
        {
        nHAdjust = rect.right - (int)pInstance->DibPicParm.Head.biWidth;
        if ( nHAdjust < 0 )
            nHAdjust = 0;
        }
    if ( rect.bottom != labs(pInstance->DibPicParm.Head.biHeight) )
        {
        nVAdjust = rect.bottom - (int)labs(pInstance->DibPicParm.Head.biHeight);
        if ( nVAdjust < 0 )
            nVAdjust = 0;
        }            
    if ( nHAdjust != 0 || nVAdjust != 0 )
        {
        GetWindowRect(hWnd, &rect);
        nWidth = rect.right - rect.left - nHAdjust;
        nHeight = rect.bottom - rect.top - nVAdjust;
        pt.x = rect.left;
        pt.y = rect.top;
        ScreenToClient(hwndMDIClient, &pt);
        GetClientRect(hwndMDIClient, &rect);
        if ( nWidth > rect.right )
            nWidth = rect.right;
        if ( nHeight > rect.bottom )
            nHeight = rect.bottom;
        MoveWindow(hWnd, pt.x, pt.y, nWidth, nHeight, TRUE);
        }
}



/******************************************************************************
 * void OnGetMinMaxInfo(HWND hWnd, MINMAXINFO PICFAR* p, CHILDINSTANCEDATA PICFAR* pInstance)
 * 
 *  WM_GETMINMAXINFO - if not maximized, set the window's maximum size to the image dimensions
 * 
 *  parameters:
 *      hWnd      - handle to this MDI child window
 *      p         - pointer to MINMAXINFO data for windows
 *      pInstance - child window and image instance data
 *
 *  notes:
 *      it's miserable to try to determine (cross-platform) if the window is iconized
 *      or not.  If it is, we don't want to set min/max info according to the current
 *      (iconized) parameters.  Below works in NT 4.0, Win '95 and Win 3.11 -- although
 *      IsIconic turns out never to be TRUE when WM_GETMINMAXINFO is received while restoring.
 ******************************************************************************/
static void OnGetMinMaxInfo(HWND hWnd, MINMAXINFO PICFAR* p, CHILDINSTANCEDATA PICFAR* pInstance)
{
    RECT wrect;
    RECT crect;
    int horz;
    int vert;

    if ( !IsZoomed(hWnd) )
        {
        GetWindowRect(hWnd, &wrect);
        GetClientRect(hWnd, &crect);
        horz = wrect.right - wrect.left - crect.right;
        vert = wrect.bottom - wrect.top - crect.bottom;
        if ( IsIconic(hWnd) || crect.right == 0 || crect.bottom == 0 || horz == 0 || vert == 0 )
            return;
        p->ptMaxTrackSize.x = (UINT)pInstance->DibPicParm.Head.biWidth + horz;
        p->ptMaxTrackSize.y = (UINT)labs(pInstance->DibPicParm.Head.biHeight) + vert;
        }
}



/******************************************************************************
 *  void OnDestroy(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance);
 * 
 *  WM_DESTROY - clean up window and image instance data
 * 
 *  parameters:
 *      hWnd      - handle to this MDI child window
 *      pInstance - child window and image instance data
 *
 *  notes:
 *      mopen.c allocated the child instance data and source image data so it
 *      frees that stuff.  The opcode DLL allocated the DIB, so mopen.c calls
 *      the opcode dll cleanup function to free the DIB.
 ******************************************************************************/
static void OnDestroy(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance)
{
    SetWindowLong(hWnd, 0, 0L); /* WM_DESTROY is the last message so this shouldn't help or hurt,
                                    but it comforts me */
    OpenCleanup(pInstance);
}



/******************************************************************************
 *  void OnPaint(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance);
 * 
 *  WM_PAINT - all image drawing
 *
 *  parameters:
 *      hWnd - handle to this MDI child window
 *      pInstance - child window and image instance data
 *
 *  notes:
 *      pInstance is NULL or pInstance->pDib is NULL while the image is being
 *      opened.  In those cases, the frame background olor (COLOR_APPWORKSPACE) is displayed.
 *      We haven't specified a background brush for the window class because that
 *      causes a background flash behind the image when we do things like sizing
 *      and scrolling and I don't like that.
 *
 *      Note: we use biHeight to tell us how much of the DIB is valid so a progressive
 *      image open can display progressively.  This may make things difficult for the
 *      open code since it shouldn't screw around with the open PIC_PARM data.  Thus
 *      the open PIC_PARM has to be slightly different from the DIB PIC_PARM.
 *
 *      Though the windows doc recommends stretchdibbitstodevice because setdib...
 *      is anachronistic and hard to use, we _never_ want to use windows to stretch
 *      the image (it doesn't do as good a job as our zoom opcode) and we only had
 *      to figure out how to use setdib... once.  In theory we should probably convert
 *      the DIB to a DDB and blit the DDB during paint.  In practice, for static
 *      images there doesn't seem to be much of a performance hit and, since we
 *      have to hold onto the DIB bits anyway for transformations and save as, I
 *      didn't want to use up the extra DDB memory.  Besides, the first time
 *      display is probably just as fast and hopefully windows is smart about its
 *      internal conversion to DDB because we've tried to be smart about the update
 *      region.
 *
 *      #### TODO: if the screen is palettized we should set a foreground palette
 *      even for bit depths > 8, since otherwise our image looks pretty bad.
 ******************************************************************************/
static void DrawHorizontalLine(HDC hdc, HBRUSH hbr, int nWidth, int y, int left, int right)
{
    HRGN hrgn = CreateRectRgn(left, y, right, y + nWidth);
    FillRgn(hdc, hrgn, hbr);
    DeleteObject(hrgn);
}

static void DrawVerticalLine(HDC hdc, HBRUSH hbr, int nWidth, int x, int top, int bottom)
{
    HRGN hrgn = CreateRectRgn(x, top, x + nWidth, bottom);
    FillRgn(hdc, hrgn, hbr);
    DeleteObject(hrgn);
}

static void Draw3DRectangle(HDC hdc, int nWidth, int left, int top, int right, int bottom)
{
    HBRUSH hbr;
    HRGN hrgn;

    // white lines
    hbr = GetStockObject(WHITE_BRUSH);
    // top
    DrawHorizontalLine(hdc, hbr, 1, top, left, right - 1);
    // left
    DrawVerticalLine(hdc, hbr, 1, left, top + 1, bottom - 1);
    // right
    DrawVerticalLine(hdc, hbr, 1, right - nWidth, top + nWidth, bottom - nWidth + 1);
    // bottom
    DrawHorizontalLine(hdc, hbr, 1, bottom - nWidth, left + nWidth, right - nWidth);

    // gray lines
    hbr = CreateSolidBrush(RGB(128, 128, 128));
    hrgn = CreateRectRgn(left + 1, top + 1, right - 1, bottom - 1);
    FrameRgn(hdc, hrgn, hbr, nWidth - 2, nWidth - 2);
    DeleteObject(hrgn);
    DeleteObject(hbr);

    // black lines
    hbr = GetStockObject(BLACK_BRUSH);
    // top
    DrawHorizontalLine(hdc, hbr, 1, top + nWidth - 1, left + nWidth - 1, right - nWidth + 1);
    // left
    DrawVerticalLine(hdc, hbr, 1, left + nWidth - 1, top + nWidth, bottom - nWidth + 1);
    // right
    DrawVerticalLine(hdc, hbr, 1, right - 1, top, bottom - 1);
    // bottom
    DrawHorizontalLine(hdc, hbr, 1, bottom - 1, left, right);
}

static void OnPaint(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HPALETTE hOldPalette = NULL;
    int xOfs;
    int yOfs;
    RECT rect;
    RECT xrect;
    HRGN hrgn;
    HBRUSH hbr;
    int dleft;
    int dtop;
    int width;
    int height;
    int ileft;
    int itop;
    WORD wDibHeight;

    hdc = BeginPaint(hWnd, &ps);
    assert(hdc != NULL);
    if ( hdc == NULL )
        return;
    if ( pInstance == 0 ||
         pInstance->pDib == 0 ||
         pInstance->DibPicParm.Head.biWidth == 0 ||
         pInstance->DibPicParm.Head.biHeight == 0 )
        {
        /* apparently the image isn't opened enough yet so just paint the update
            region with a COLOR_APPWORKSPACE brush */
        hbr = CreateSolidBrush(GetSysColor(COLOR_APPWORKSPACE));
        hrgn = CreateRectRgnIndirect(&ps.rcPaint);
        FillRgn(hdc, hrgn, hbr);
        DeleteObject(hrgn);
        DeleteObject(hbr);
        }
    else
        {
        if ( pInstance->DibPicParm.Head.biBitCount <= 8 || MiscIsPaletteDisplay(0) )
            {
            /* The palette is initialized from the colormap here (or in OnPaletteChanged).
                The only other place that knows enough to do it before the first time
                it's needed is in the open opcode code.  I didn't want any opcode code
                to have to know about the child window instance data -- although I found
                I still had to give the open opcode a way of destroying the palette
                so a new colortable could be applied to the image */
            if ( pInstance->hPalette == NULL )
                {
                pInstance->hPalette =
                    PaletteFromBitmapInfo((LPBITMAPINFO)&pInstance->DibPicParm.Head);
                /* if this is the active child, then the frame has to set this palette
                    as the system palette since the palette wasn't available or was
                    different when the child originally became active and previously
                    sent the frame an activate message */
                if ( hWnd == ChildGetActive() )
                    SendMessage(hwndFrame, WM_MDIACTIVATE, (WPARAM)hWnd, 0);
                }
            if ( pInstance->hPalette != NULL )
                {
                /* realize this window's palette in terms of the current system palette,
                    (which is the same as this palette if this is the active window) */
                hOldPalette = SelectPalette(hdc, pInstance->hPalette, TRUE);
                assert(hOldPalette != NULL);    /* there's got to be _some_ palette? */
                RealizePalette(hdc);
                }
            }

        /* only paint if the update rectangle isn't empty */
        if ( ps.rcPaint.right > ps.rcPaint.left && ps.rcPaint.bottom > ps.rcPaint.top )
            {
            GetClientRect(hWnd, &rect);
            xOfs = 0;
            yOfs = 0;
            if ( IsZoomed(hWnd) )
                {
                if ( pInstance->DibPicParm.Head.biWidth < rect.right )
                    xOfs = ( rect.right - (int)pInstance->DibPicParm.Head.biWidth ) / 2;
                if ( labs(pInstance->DibPicParm.Head.biHeight) < rect.bottom )
                    yOfs = ( rect.bottom - (int)labs(pInstance->DibPicParm.Head.biHeight) ) / 2;
                }

            dleft  = xOfs;
            dtop   = yOfs;
            width  = (int)pInstance->DibPicParm.Head.biWidth - pInstance->xWindowOrg;
            height = (int)labs(pInstance->DibPicParm.Head.biHeight) - pInstance->yWindowOrg;
            wDibHeight = (WORD)labs(pInstance->lDibHeight);
            if ( pInstance->lDibHeight > 0 )
                dtop += (int)labs(pInstance->DibPicParm.Head.biHeight) - wDibHeight;
            height -= (int)labs(pInstance->DibPicParm.Head.biHeight) - wDibHeight;
            ileft = pInstance->xWindowOrg; 
            if ( pInstance->lDibHeight > 0 )
                itop = 0;
            else
                itop = (int)labs(pInstance->DibPicParm.Head.biHeight) - wDibHeight;                

            if ( height > 0 )
                SetDIBitsToDevice(
                    hdc,
                    dleft, dtop, width, height,
                    ileft, itop,
                    0,
                    (UINT)labs(pInstance->DibPicParm.Head.biHeight),
                    pInstance->pDib,
                    (LPBITMAPINFO)&pInstance->DibPicParm.Head,
                    DIB_RGB_COLORS);
            
            /******************************************************************
                paint the background color over any part of the window outside the image
                and any part inside which isn't available yet
            */
            hbr = CreateSolidBrush(GetSysColor(COLOR_APPWORKSPACE));

            if ( wDibHeight != (WORD)labs(pInstance->DibPicParm.Head.biHeight) )
                {
                xrect.left = dleft;
                xrect.right = dleft + width;
                xrect.top = yOfs;
                xrect.bottom = xrect.top + (int)labs(pInstance->DibPicParm.Head.biHeight);
                if ( dtop == yOfs )
                    xrect.top = dtop + height;
                else
                    xrect.bottom = dtop;
                hrgn = CreateRectRgnIndirect(&xrect);
                FillRgn(hdc, hrgn, hbr);
                DeleteObject(hrgn);
                }

            /******************************************************************
                paint the full height of the window rectangle to the left and right of the 
                image, if any
            */
            // left of image
            xrect.left = 0;
            xrect.right = xOfs - 3;
            xrect.top = 0;
            xrect.bottom = rect.bottom;
            if ( xrect.left < xrect.right )
                {
                hrgn = CreateRectRgnIndirect(&xrect);
                FillRgn(hdc, hrgn, hbr);
                DeleteObject(hrgn);
                }

            // right of image
            xrect.left = xOfs + (int)pInstance->DibPicParm.Head.biWidth + 3;
            xrect.right = rect.right;
            if ( xrect.left < xrect.right )
                {
                hrgn = CreateRectRgnIndirect(&xrect);
                FillRgn(hdc, hrgn, hbr);
                DeleteObject(hrgn);
                }

            // above image
            xrect.left = xOfs - 3;
            xrect.right = xOfs + (int)pInstance->DibPicParm.Head.biWidth + 3;
            xrect.top = 0;
            xrect.bottom = yOfs - 3;
            if ( xrect.top < xrect.bottom )
                {
                hrgn = CreateRectRgnIndirect(&xrect);
                FillRgn(hdc, hrgn, hbr);
                DeleteObject(hrgn);
                }

            // below image
            xrect.top = yOfs + (int)labs(pInstance->DibPicParm.Head.biHeight) + 3;
            xrect.bottom = rect.bottom;
            if ( xrect.top < xrect.bottom )
                {
                hrgn = CreateRectRgnIndirect(&xrect);
                FillRgn(hdc, hrgn, hbr);
                DeleteObject(hrgn);
                }
            DeleteObject(hbr);

            Draw3DRectangle(
                hdc,
                4,
                xOfs - 4,
                yOfs - 4,
                xOfs + (int)pInstance->DibPicParm.Head.biWidth + 4,
                yOfs + (int)labs(pInstance->DibPicParm.Head.biHeight) + 4);

            }

        /* else the update rectangle was empty when tested to see if we needed to paint */
        
        if ( hOldPalette != NULL )
            SelectPalette(hdc, hOldPalette, FALSE);
        }
    EndPaint(hWnd, &ps);
}



/******************************************************************************
 *  void OnPaletteChanged(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance);
 * 
 *  WM_PALETTE_CHANGED - re-realize image palette after
 *  a system palette change
 * 
 *  parameters:
 *      hWnd      - handle to this MDI child window
 *      pInstance - child window and image instance data
 *
 *  notes:
 *      When the system palette changes, each window which cares has to realize
 *      its palette in terms of the system palette.
 ******************************************************************************/
void OnPaletteChanged(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance)
{
    HDC hdc;
    HPALETTE hOldPalette = NULL;

    if ( pInstance->pDib != 0 )
        {
        if ( pInstance->DibPicParm.Head.biBitCount <= 8 || MiscIsPaletteDisplay(0) )
            {
            /* The palette is initialized from the colormap here (or in OnPaint).
                The only other place that knows to do it before the first time it's
                needed is in the open opcode code.  I didn't want any opcode code
                to have to know about the child window instance data */
            if ( pInstance->hPalette == NULL )
                {
                pInstance->hPalette =
                    PaletteFromBitmapInfo((BITMAPINFO PICFAR*)&pInstance->DibPicParm.Head);
                /* if this is the active child, then the frame has to set this palette
                    as the system palette since the palette wasn't available or was
                    different when the child originally became active and previously
                    sent the frame an activate message */
                if ( hWnd == ChildGetActive() )
                    SendMessage(hwndFrame, WM_MDIACTIVATE, (WPARAM)hWnd, 0);
                }
            if ( pInstance->hPalette != NULL )
                {
                hdc = GetDC(hWnd);
                hOldPalette = SelectPalette(hdc, pInstance->hPalette, TRUE);
                /* realize this window's palette in terms of the current system palette,
                    (which is the same as this palette if this is the active window) */
                RealizePalette(hdc);
                if ( hOldPalette != NULL )
                    SelectPalette(hdc, hOldPalette, FALSE);
                ReleaseDC(hWnd, hdc);
                InvalidateRect(hWnd, NULL, FALSE);
                UpdateWindow(hWnd);
                }
            }
        }
}



/******************************************************************************
 *  void OnHScroll(
 *    HWND hWnd,
 *    HWND hwndCtrl,
 *    UINT uScrollCode,
 *    int  nTrackPos)
 *
 *  WM_HSCROLL processing - horizontal scroll bar messages
 *
 *  parameters:
 *      hWnd        - handle to this MDI child window
 *      hwndCtrl    - not used
 *      uScrollCode - SB_* code (see WM_HSCROLL doc)
 *      nTrackPos   - thumb pos if SB_THUMBTRACK or SB_THUMBPOSITION
 *
 *  notes:
 *      much of the work happens in NewScrollPos()
 ******************************************************************************/
static void OnHScroll(
    HWND hWnd,
    HWND hwndCtrl,
    UINT uScrollCode,
    int  nTrackPos)
{
    RECT rect;
    int nCurPos;
    int nNewPos;
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hWnd);

    NOREFERENCE(hwndCtrl);
    if ( pInstance->pDib != 0 )
        {
        GetClientRect(hWnd, &rect);
        nCurPos = GetScrollPos(hWnd, SB_HORZ);
        nNewPos = NewScrollPos(                         /* returns position after scroll event */
            pInstance,
            uScrollCode,                                /* SB_* codes */
            nTrackPos,                                  /* thumb pos for SB_THUMB* */
            nCurPos,                                    /* position before scroll event */
            (int)pInstance->DibPicParm.Head.biWidth,    /* width of image */
            rect.right);                                /* width of window client area */

        if ( nNewPos != nCurPos )
            {
            /* the thumb position changed */
            SetScrollPos(hWnd, SB_HORZ, nNewPos, TRUE);
            if ( abs(nNewPos - nCurPos) >= rect.right )
                {
                /* no part of the previous window area is visible at the new thumb position */
                pInstance->xWindowOrg = nNewPos;
                InvalidateRect(hWnd, NULL, FALSE);
                }
            else
                {
                /* some part of the previous window area is visible at the new thumb position */

                /* update window a necessity before ScrollWindow - note also that new xWindowOrg
                    _must_ follow UpdateWindow  and must precede ScrollWindow (or at least
                    must precede any UpdateWindow following ScrollWindow -- ScrollWindow
                    invalidates the necessary region */
                UpdateWindow(hWnd);
                pInstance->xWindowOrg = nNewPos;
                ScrollWindow(hWnd, nCurPos - nNewPos, 0, NULL, NULL);
                }
            /* following not strictly necessary, but I like the way it looks better as
                you're thumbtrack'ing, to have the window presentation updated in real time */
            UpdateWindow(hWnd);
            }
        /* else the thumb position didn't change */
        }
}



/******************************************************************************
 *  void OnVScroll(
 *    HWND hWnd,
 *    HWND hwndCtrl,
 *    UINT uScrollCode,
 *    int  nTrackPos)
 *
 *  WM_VSCROLL processing - vertical scroll bar messages
 *
 *  parameters:
 *      hWnd        - handle to this MDI child window
 *      hwndCtrl    - not used
 *      uScrollCode - SB_* code (see WM_VSCROLL doc)
 *      nTrackPos   - thumb pos if SB_THUMBTRACK or SB_THUMBPOSITION
 *
 *  notes:
 *      much of the work happens in NewScrollPos()
 ******************************************************************************/
static void OnVScroll(
    HWND hWnd,
    HWND hwndCtrl,
    UINT uScrollCode,
    int  nTrackPos)
{
    RECT rect;
    int nCurPos;
    int nNewPos;
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hWnd);

    NOREFERENCE(hwndCtrl);
    if ( pInstance->pDib != 0 )
        {
        GetClientRect(hWnd, &rect);
        nCurPos = GetScrollPos(hWnd, SB_VERT);
        nNewPos = NewScrollPos(                         /* returns position after scroll event */
            pInstance,
            uScrollCode,                                /* SB_* codes */
            nTrackPos,                                  /* thumb pos for SB_THUMB* */
            nCurPos,                                    /* position before scroll event */
            (int)labs(pInstance->DibPicParm.Head.biHeight),   /* height of image */
            rect.bottom);                               /* height of window client area */

        if ( nNewPos != nCurPos )
            {
            /* the thumb position changed */
            SetScrollPos(hWnd, SB_VERT, nNewPos, TRUE);
            if ( abs(nNewPos - nCurPos) >= rect.bottom )
                {
                /* no part of the previous window area is visible at the new thumb position */
                pInstance->yWindowOrg = nNewPos;
                InvalidateRect(hWnd, NULL, FALSE);
                }
            else
                {
                /* some part of the previous window area is visible at the new thumb position */

                /* update window a necessity before ScrollWindow - note also that new yWindowOrg
                    _must_ follow UpdateWindow  and must precede ScrollWindow (or at least
                    must precede any UpdateWindow following ScrollWindow -- ScrollWindow
                    invalidates the necessary region */
                UpdateWindow(hWnd);
                pInstance->yWindowOrg = nNewPos;
                ScrollWindow(hWnd, 0, nCurPos - nNewPos, NULL, NULL);
                }
            /* following not strictly necessary, but I like the way it looks better as
                you're thumbtrack'ing, to have the window presentation updated in real time */
            UpdateWindow(hWnd);
            }
        /* else the thumb position didn't change */
        }
}



/******************************************************************************
 *  int NewScrollPos(
 *      CHILDINSTANCEDATA PICFAR* pInstance,
 *      UINT uScrollCode,
 *      int nTrackPos,
 *      int nCurPos,
 *      int nTotal,
 *      int nPage)
 * 
 *  worker function for OnVScroll and OnHScroll
 *
 *  parameters:
 *      uScrollCode - SB_* code (see WM_HSCROLL, WM_VSCROLL doc)
 *      nTrackPos   - new thumb position if SB_THUMBTRACK or SB_THUMBPOSITION
 *      nCurPos     - position of the thumb before this scroll event
 *      nTotal      - range of scroll bar is 0 .. nTotal - 1
 *      nPage       - size of one page (note we page by a full page not a page - 1 pixel line)
 *
 *  returns:
 *      possibly new scroll thumb position (it's not new for example if SB_PAGEDOWN
 *      happens and the image bottom is already displayed)
 ******************************************************************************/
static int NewScrollPos(
    CHILDINSTANCEDATA PICFAR* pInstance,
    UINT uScrollCode,
    int nTrackPos,
    int nCurPos,
    int nTotal,
    int nPage)
{
    int nNewPos = nCurPos;

    switch ( uScrollCode )
        {
        case SB_PAGEDOWN:
            nNewPos = nCurPos + nPage;
            break;

        case SB_BOTTOM:
            nNewPos = nTotal - nPage;
            break;

        case SB_LINEDOWN:
            if ( pInstance->nLineScrollIncrement < MAX_LINESCROLLINCREMENT )
                {
                if ( (LONG)( MiscTickCount() - pInstance->dwTimeNextLineScrollIncrement ) >= 0 )
                    {
                    pInstance->nLineScrollIncrement++;
                    pInstance->dwTimeNextLineScrollIncrement = MiscTickCount() +
                        TIME_NEXTSCROLLINCREMENT;
                    }
                }
            nNewPos = nCurPos + pInstance->nLineScrollIncrement;
            break;                

        case SB_LINEUP:
            if ( pInstance->nLineScrollIncrement < MAX_LINESCROLLINCREMENT )
                {
                if ( (LONG)( MiscTickCount() - pInstance->dwTimeNextLineScrollIncrement ) >= 0 )
                    {
                    pInstance->nLineScrollIncrement++;
                    pInstance->dwTimeNextLineScrollIncrement = MiscTickCount() +
                        TIME_NEXTSCROLLINCREMENT;
                    }
                }
            nNewPos = nCurPos - pInstance->nLineScrollIncrement;
            break;

        case SB_PAGEUP:
            nNewPos = nCurPos - nPage;
            break;

        case SB_TOP:
            nNewPos = 0;
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            nNewPos = nTrackPos;
            break;
        };

    /* normalize the new position so we don't scroll past the top or bottom of the image */
    if ( nNewPos < 0 )
        nNewPos = 0;
    else if ( nNewPos > nTotal - nPage )
        nNewPos = nTotal - nPage;

    return ( nNewPos );
}



/******************************************************************************
 *  void OnRClick(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance, int xPos, int yPos);
 *
 *  processing WM_xRBUTTONUP following WM_xRBUTTONDOWN
 *  to popup an image menu
 *
 *  parameters:
 *      hWnd        - handle to this MDI child window
 *      pInstance   - child window and image instance data
 *      xPos        - x position of RBUTTONUP
 *      yPos        - y position of RBUTTONUP
 *
 *  notes:
 *      The transform cascade menu items are created on the fly based on the transform
 *      opcodes built into pTransformOpTable (minerva.c).  Save, Save As, Image Options
 *      and Properties are enabled/disabled just like menu bar File menu items - but we
 *      can't just look at the File menu items because their enabled/disabled state
 *      isn't evaluated unless the user looks at the menu, so it may not be correct at
 *      this instant.
 *
 *      Creating a dummy menubar in the resource file with a single popup allows us
 *      to load and maintain this popup using resource editors rather than completely
 *      constructing it on the fly each time
 *
 *      Some resource editors (16-bit App Studio at least) don't allow you to define
 *      an empty popup so the Transform popup has a single separator in the resource
 *      definition which is deleted after the Transform menu items are added below
 ******************************************************************************/
static void OnRClick(HWND hWnd, CHILDINSTANCEDATA PICFAR* pInstance, int xPos, int yPos)
{
    HMENU hMenu;
    HMENU hPopup;
    HMENU hTransform;
    POINT pt;
    int i;

    if ( pInstance == 0 || pInstance->nWaitCursor != 0 || nWaitCursor != 0 )
        return;
    hMenu = LoadMenu(hinstThis, "MinervaPopup");
    assert(hMenu != 0);
    if ( hMenu == 0 )
        return;

    hPopup = GetSubMenu(hMenu, 0);

    /* add transform menu selections on the fly based on transform opcode table
        thus this code doesn't need to change if we add another transform opcode */
    hTransform = GetSubMenu(hPopup, TRANSFORM_POPUP_MENU_POS);
    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        if ( pTransformOpTable[i]->pszMenuItem != 0 )
            AppendMenu(
                hTransform,
                MF_STRING | MF_ENABLED,
                IDM_TRANSFORM + i,
                pTransformOpTable[i]->pszMenuItem);
        }
    /* delete the Transform menu separator.  It's there in the first place only because
        App Studio changes the popup style if there are no cascaded menu items */
    DeleteMenu(hTransform, 0, MF_BYPOSITION);

    /* enable/disabled status for following */
    MenuStatusFileSave(hPopup, hWnd);
    MenuStatusFileSaveAs(hPopup, hWnd);
    MenuStatusImageOptions(hPopup, hWnd);
    MenuStatusTransform(hTransform, hWnd);
    
    /* following finally pops up the menu */
    pt.x = xPos;
    pt.y = yPos;
    ClientToScreen(hWnd, &pt);  /* has to be in screen coordinates */
    TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndFrame, NULL);
    /* note you can't be sure whether the WM_COMMAND event was processed before Track...
        returns or after it returns.  The behavior is different in Win16 and Win32, although
        Track... doesn't return while the menu is still visible and no selection has been made */
    DestroyMenu(hMenu);
}



/******************************************************************************
 *  HPALETTE PaletteFromBitmapInfo(const BITMAPINFO PICFAR* pbmi);
 * 
 *  create an HPALETTE from a bitmap colortable
 *
 *  parameters:
 *      pbmi - far pointer to BITMAPINFO (BITMAPINFOHEADER followed by color table)
 *
 *  returns:
 *      HPALETTE of a palette corresponding to the color table
 ******************************************************************************/
static HPALETTE PaletteFromBitmapInfo(const BITMAPINFO PICFAR* pbmi)
{
    HPALETTE hPalette;
    LOGPALETTE PICFAR* ppal;
    int nColors;
    int i;
        
    /* following if biClrUsed == 0 according to the windows doc -- of course the
        PIC libraries handle this for you, but we might have loaded the bitmap
        ourselves in mobmp.c */
    nColors = (int)pbmi->bmiHeader.biClrUsed;
    if ( nColors == 0 )
        {
        if ( pbmi->bmiHeader.biBitCount == 8 )
            nColors = 256;
        else if ( pbmi->bmiHeader.biBitCount == 4 )
            nColors = 16;
        else if ( pbmi->bmiHeader.biBitCount == 1 )
            nColors = 2;
        else
            return ( NULL );
        }
    if ( pbmi->bmiHeader.biClrImportant != 0 )
        nColors = (int)pbmi->bmiHeader.biClrImportant;

    ppal = GlobalAllocPtr(
        GMEM_MOVEABLE,
        sizeof(LOGPALETTE) + ( nColors - 1 ) * sizeof(PALETTEENTRY));
    assert(ppal != 0);
    if ( ppal == 0 )
        {
        /* certainly wouldn't expect this */
        return ( NULL );
        }

    ppal->palVersion = 0x300;
    ppal->palNumEntries = (WORD)nColors;
    
    for ( i = 0; i < nColors; i++ )
        {
        ppal->palPalEntry[i].peRed   = pbmi->bmiColors[i].rgbRed;
        ppal->palPalEntry[i].peGreen = pbmi->bmiColors[i].rgbGreen;
        ppal->palPalEntry[i].peBlue  = pbmi->bmiColors[i].rgbBlue;
        ppal->palPalEntry[i].peFlags = 0;        
        }                
    hPalette = CreatePalette(ppal);
    /* hPalette is deleted in mopen.c/OpenCleanup -- ordinarily during
        WM_DESTROY processing for the child, and also when the palette
        is invalidated by an open opcode if the colortable changes. */

    MiscGlobalFreePtr(&ppal);
    return ( hPalette );
}
