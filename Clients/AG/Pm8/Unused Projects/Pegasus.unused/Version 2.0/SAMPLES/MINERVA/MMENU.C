/******************************************************************************
 *
 * MMENU.C - Minerva MDI menu command handler
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
 *     MenuStatusFileSave     - enable/gray File/Save menu item
 *     MenuStatusFileSaveAs   - enable/gray File/Save As menu item
 *     MenuStatusImageOptions - enable/gray File/Image Options menu item
 *     MenuStatusTransform    - enable/gray Transform menu items
 *     MenuStatus             - enable/gray all menu items
 *     OnCommand              - frame window WM_COMMAND handler
 *
 * Local functions:
 *     OnFileClose            - File/Close menu item selected
 *     OnFileProperties       - File/Properties menu item selected
 *     OnFileImageOptions     - File/Image Options menu item selected
 *     OnFileDefaultOptions   - some File/Default Options menu item selected
 *     OnTransform            - some Transform menu item selected
 *     OnTools                - some Tools menu item selected
 *     OnWindowCloseAll       - Windows/Close All menu item selected
 *     OnNewWindow            - Windows/New Window menu item selected
 *
 * Revision History:
 *   04-21-97  1.00.21  jrb  set bClippedWindowSize to FALSE initially for new window
 *   11-11-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <commdlg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "mchild.h"
#include "mmenu.h"
#include "merror.h"
#include "mopen.h"
#include "msave.h"
#include "mabout.h"
#include "mmisc.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void OnFileClose(HWND hwndChild);
static void OnFileProperties(HWND hwndChild);
static void OnFileImageOptions(HWND hwndChild);
static void OnFileDefaultOptions(WPARAM wCmd);
static void OnTransform(HWND hwnd, WPARAM wCmd);
static void OnTools(WPARAM wCmd);
static void OnWindowCloseAll(void);
static void OnNewWindow(HWND hwnd);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  void MenuStatusFileSave(HMENU hmenu, HWND hwnd)
 * 
 *  enable/gray File/Save menu item
 *
 *  parameters:
 *      hmenu - menu containing the item (could be frame menu or popup)
 *      hwnd  - active child window or NULL
 *
 *  notes:
 *      grayed unless there is an active child, and it has been modified
 *      and its image's open opcode defines a save opcode for saving and
 *      that save opcode dll is loaded
 ******************************************************************************/
void MenuStatusFileSave(HMENU hmenu, HWND hwnd)
{                
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    int i;

    for ( i = 0; pSaveOpTable[i] != 0 && pSaveOpTable[i]->nFoundParmVer == 0; i++ )
        ;
    if ( hwnd != NULL && pInstance != 0 && ChildIsModified(hwnd) && pSaveOpTable[i] != 0 )
        EnableMenuItem(hmenu, IDM_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
    else                
        EnableMenuItem(hmenu, IDM_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED);
}                    

                       

/******************************************************************************
 *  void MenuStatusFileSaveAs(HMENU hmenu, HWND hwnd)
 * 
 *  enable/gray File/Save As menu item
 *
 *  parameters:
 *      hmenu - menu containing the item (could be frame menu or popup)
 *      hwnd  - active child window or NULL
 *
 *  notes:
 *      grayed unless there is an active child and there's at least one save
 *      opcode dll loaded
 ******************************************************************************/
void MenuStatusFileSaveAs(HMENU hmenu, HWND hwnd)
{
    int i;

    for ( i = 0; pSaveOpTable[i] != 0 && pSaveOpTable[i]->nFoundParmVer == 0; i++ )
        ;
    if ( hwnd != NULL && pSaveOpTable[i] != 0 )
        EnableMenuItem(hmenu, IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_ENABLED);
    else
        EnableMenuItem(hmenu, IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_GRAYED);                    
}



/******************************************************************************
 *  void MenuStatusImageOptions(HMENU hmenu, HWND hwnd)
 * 
 *  enable/gray File/Image Options menu item
 *
 *  parameters:
 *      hmenu - menu containing the item (could be frame menu or popup)
 *      hwnd  - active child window or NULL
 *
 *  notes:
 *      grayed unless there is an active child, and its image's open opcode
 *      defines an image options dialog
 ******************************************************************************/
void MenuStatusImageOptions(HMENU hmenu, HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);

    if ( hwnd != NULL &&
         pInstance != 0 &&
         pInstance->pOpenOp->pfnOptionsDialog != 0 )
        EnableMenuItem(hmenu, IDM_FILE_IMAGEOPTIONS, MF_BYCOMMAND | MF_ENABLED);
    else                
        EnableMenuItem(hmenu, IDM_FILE_IMAGEOPTIONS, MF_BYCOMMAND | MF_GRAYED);
}



/******************************************************************************
 *  void MenuStatusTransform(HMENU hmenu, HWND hwnd)
 * 
 *  enable/gray Transform menu items
 *
 *  parameters:
 *      hmenu - menu containing the Transform popup (could be frame menu or popup)
 *      hwnd  - active child window or NULL
 *
 *  notes:
 *      each transform menu item is grayed unless its corresponding opcode dll is loaded
 ******************************************************************************/
void MenuStatusTransform(HMENU hmenu, HWND hwnd)
{                
    int n = 0;
    int i;

    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        if ( pTransformOpTable[i]->pszMenuItem != 0 )
            {
            if ( hwnd == NULL || pTransformOpTable[i]->nFoundParmVer == 0 )
                EnableMenuItem(hmenu, n++, MF_BYPOSITION | MF_GRAYED);                    
            else
                EnableMenuItem(hmenu, n++, MF_BYPOSITION | MF_ENABLED);
            }
    assert(n == GetMenuItemCount(hmenu));
}        



/******************************************************************************
 *  void MenuStatus(HMENU hmenu, HWND hwnd)
 * 
 *  enable/gray all menu items
 *
 *  parameters:
 *      hmenu - menu containing the Transform popup (could be frame menu or popup)
 *      hwnd  - active child window or NULL
 *
 *  notes:
 *      some menu items are always enabled and this code doesn't look at them
 *      file/close, file/properties and window menu items are grayed unless there
 *      is an active child.  Others are enabled/grayed by calling MenuStatusFileSave,
 *      MenuStatusFileSaveAs, MenuStatusFileImageOptions or MenuStatusTransform
 *
 *      The only tricky part is to make sure it's the menu bar instead of the
 *      popup as all the popup menu item positions are different.  The popup doesn't
 *      change during its lifetime so we don't have to evaluate it here --
 *      it's evaluated once when its created.  But the various position #define's
 *      (FILE_MENU_POS) have counterparts on the popup which we don't want to confuse.
 *      So we make sure that the hmenu received is from the menu bar.  However, if
 *      the child window is maximized windows adds a system popup menu to the menu
 *      bar in position 0 and slides everything else down.  So we look at the menu
 *      bar popup at position 1.  If it's the "&File" menu, then we know the child
 *      has been maximized and adjust the position we've received so we can use our
 *      same #define's for menu bar pulldown menu positions
 ******************************************************************************/
void MenuStatus(int nPos, HMENU hmenu)
{
    char sz[sizeof("&File") + 1];   /* + 1 to get the next char if menu string is longer
                                        since note that sizeof includes the null terminator */
    HWND hwnd = ChildGetActive();
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    UINT flags = MF_BYCOMMAND;

    if ( pInstance == 0 || pInstance->nWaitCursor != 0 )
        hwnd = NULL;

    if ( hwnd != NULL )
        flags = MF_ENABLED;
    else
        flags = MF_GRAYED;
        
    if ( hmenu == GetSubMenu(GetMenu(hwndFrame), nPos) )
        {
        /* now we know its a menubar menu
            if the MDI child menu is maximized, then the MDI child system menu is
            added to the frame menu bar in position 0, sliding everything else down
            and throwing off our position #define's by 1 */
        GetMenuString(GetMenu(hwndFrame), 1, sz, sizeof(sz), MF_BYPOSITION);
        if ( stricmp(sz, "&File") == 0 )
            {
            /* child whildow is maximized */
            if ( nPos == 0 )
                return; /* initmenu for maximized child system menu is ignored */
            nPos--;     /* adjust so our #define's (FILE_MENU_POS, etc.) can be used */
            }
        switch ( nPos )
            {
            case FILE_MENU_POS:
                EnableMenuItem(hmenu, IDM_FILE_CLOSE,      flags);
                EnableMenuItem(hmenu, IDM_FILE_PROPERTIES, flags);
                MenuStatusFileSave(hmenu, hwnd);
                MenuStatusFileSaveAs(hmenu, hwnd);
                MenuStatusImageOptions(hmenu, hwnd);
                break;

            case TRANSFORM_MENU_POS:
                MenuStatusTransform(hmenu, hwnd);
                break;

            case TOOLS_MENU_POS:
                break;

            case WINDOW_MENU_POS:
                EnableMenuItem(hmenu, IDM_WINDOW_NEWWINDOW,      flags);
                EnableMenuItem(hmenu, IDM_WINDOW_CLOSEALL,       flags);
                EnableMenuItem(hmenu, IDM_WINDOW_CASCADE,        flags);
                EnableMenuItem(hmenu, IDM_WINDOW_TILEHORIZONTAL, flags);
                EnableMenuItem(hmenu, IDM_WINDOW_TILEVERTICAL,   flags);
                EnableMenuItem(hmenu, IDM_WINDOW_ARRANGEICONS,   flags);
                break;

            case HELP_MENU_POS:
                break;

            default:
                assert(FALSE);
                break;
            }
        }
}



/******************************************************************************
 *  void MenuOnCommand(HMENU hmenu, HWND hwnd)
 * 
 *  frame window WM_COMMAND handler
 *
 *  parameters:
 *      wCmd - menu item id (WM_COMMAND wParam)
 *
 *  returns:
 *      TRUE if the menu item id is recognized
 *
 *  notes:
 *      The following code needs to change if there are more than 10 mru files
 *      more than 100 default options, more than 100 tools or more than 100 transforms
 ******************************************************************************/
BOOL MenuOnCommand(WPARAM wCmd)
{
    HWND hwnd = ChildGetActive();
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);

    if ( pInstance == 0 || pInstance->nWaitCursor != 0 )
        hwnd = NULL;
    
    switch ( wCmd )
        {
        case IDM_FILE_OPEN:
            OpenOnFileOpen();
            break;
        case IDM_FILE_CLOSE:
            OnFileClose(hwnd);
            break;
        case IDM_FILE_SAVE:
            SaveOnFileSave(hwnd);
            break;
        case IDM_FILE_SAVEAS:
            SaveOnFileSaveAs(hwnd);
            break;
        case IDM_FILE_PROPERTIES:
            OnFileProperties(hwnd);
            break;
        case IDM_FILE_IMAGEOPTIONS:
            OnFileImageOptions(hwnd);
            break;
        case IDM_FILE_EXIT:
            SendMessage(hwndFrame, WM_CLOSE, 0, 0);
            break;
        case IDM_FILE_MRUFILE1:
        case IDM_FILE_MRUFILE2:
        case IDM_FILE_MRUFILE3:
        case IDM_FILE_MRUFILE4:
        case IDM_FILE_MRUFILE5:
        case IDM_FILE_MRUFILE6:
        case IDM_FILE_MRUFILE7:
        case IDM_FILE_MRUFILE8:
        case IDM_FILE_MRUFILE9:
            OpenOnFileMruOpen(wCmd - IDM_FILE_MRUFILE1 + 1);
            break;
        
        case IDM_WINDOW_NEWWINDOW:
            OnNewWindow(hwnd);
            break;
        case IDM_WINDOW_CASCADE:
            SendMessage(hwndMDIClient, WM_MDICASCADE, 0, 0);
            break;
        case IDM_WINDOW_TILEHORIZONTAL:
            SendMessage(hwndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
            break;
        case IDM_WINDOW_TILEVERTICAL:
            SendMessage(hwndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);
            break;
        case IDM_WINDOW_ARRANGEICONS:
            SendMessage(hwndMDIClient, WM_MDIICONARRANGE, 0, 0);
            break;
        case IDM_WINDOW_CLOSEALL:
            OnWindowCloseAll();
            break;

        case IDM_HELP_CONTENTS:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, 0);
            break;

        case IDM_HELP_ABOUT:
            AboutOnHelpAbout();
            break;

        default:
            if ( wCmd >= IDM_FILE_DEFAULTOPTIONS && wCmd < IDM_FILE_DEFAULTOPTIONS + 100 )
                OnFileDefaultOptions(wCmd);
            else if ( wCmd >= IDM_TRANSFORM && wCmd < IDM_TRANSFORM + 100 )
                OnTransform(hwnd, wCmd);
            else if ( wCmd >= IDM_TOOLS && wCmd < IDM_TOOLS + 100 )
                OnTools(wCmd);
            else
                return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void OnFileClose(HWND hwndChild)
 * 
 *  File/Close selected
 *
 *  parameters:
 *      hwndChild - HWND of active child or NULL if none active
 ******************************************************************************/
static void OnFileClose(HWND hwndChild)
{
    if ( hwndChild == NULL )
        return; /* File/Close should have been grayed */

    /* allow the user an opportunity to save modified data or cancel the close */
    if ( ChildQueryClose(hwndChild) )
        /* MDI client is supposed to close it */
        SendMessage(hwndMDIClient, WM_MDIDESTROY, (WPARAM)hwndChild, 0);
}



/******************************************************************************
 *  void OnFileProperties(HWND hwndChild)
 * 
 *  File/Properties selected
 *
 *  parameters:
 *      hwndChild - HWND of active child or NULL if none active
 ******************************************************************************/
static void OnFileProperties(HWND hwndChild)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwndChild);

    if ( pInstance == 0 || pInstance->pOpenOp->pfnPropertiesDialog == 0 )
        return; /* File/Properties should have been grayed */

    (*pInstance->pOpenOp->pfnPropertiesDialog)(
        hwndChild,
        pInstance->pszFilename,
        pInstance->pSrc,
        pInstance->dwSrcLen,
        &pInstance->SrcPicParm,
        pInstance->pDib,
        pInstance->dwDibLen,
        &pInstance->DibPicParm,
        pInstance->pOpenData);
}



/******************************************************************************
 *  void OnFileImageOptions(HWND hwndChild)
 * 
 *  File/Image Options selected
 *
 *  parameters:
 *      hwndChild - HWND of active child or NULL if none active
 ******************************************************************************/
static void OnFileImageOptions(HWND hwndChild)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwndChild);

    if ( pInstance == 0 || pInstance->pOpenOp->pfnOptionsDialog == 0 )
        return; /* File/Image Options should have been grayed */

    (*pInstance->pOpenOp->pfnOptionsDialog)(
        hwndChild,
        pInstance->pSrc,
        pInstance->dwSrcLen,
        &pInstance->SrcPicParm,
        &pInstance->pDib,
        &pInstance->dwDibLen,
        &pInstance->DibPicParm,
        &pInstance->pOpenData);
}



/******************************************************************************
 *  void OnWindowCloseAll()
 * 
 *  Window/Close All selected
 *
 *  notes:
 *      I include the Enum... proc here because it's not called by anybody else
 *      and it's easier for me to understand if it's right here.
 ******************************************************************************/
static BOOL CALLBACK EnumChildProcClose(HWND hwnd, LPARAM lParam)
{
    char sz[sizeof(MDICHILD_CLASSNAME) + 1];

    NOREFERENCE(lParam);
    /* some of the children of the client are not actually our mdi child windows
        For example, if a window is iconized, the icon legend is a child of the
        client.  For that reason, we compare the enum'ed window class name with
        the one we established for mdi children to make sure it's one of ours */
    GetClassName(hwnd, sz, sizeof(sz));
    if ( lstrcmp(sz, MDICHILD_CLASSNAME) == 0 )
        SendMessage(hwndMDIClient, WM_MDIDESTROY, (WPARAM)hwnd, 0);
    return ( TRUE );
}

static void OnWindowCloseAll(void)
{
    /* give user an opportunity for each child to save modified data or cancel
        the closeall */
    if ( ChildQueryCloseAll() )
        {
        /* ShowWindow is suggested in one of MS's examples to reduce flashing */
        ShowWindow(hwndMDIClient, SW_HIDE);
        EnumChildWindows(hwndMDIClient, (BOOL (CALLBACK*)(HWND, LPARAM))EnumChildProcClose, 0);
        ShowWindow(hwndMDIClient, SW_SHOW);
        }
}



/******************************************************************************
 *  void OnFileDefaultOptions(WPARAM wCmd)
 * 
 *  some File/Default Options menu item selected
 *
 *  parameters:
 *      wCmd - menu id of selected menu item
 *
 *  notes:
 *      Looks through all the opcode tables to find the entry with a matching
 *      menu id.  We look to make sure the dll is present before calling
 *      for the options dialog, even though the menu item would be grayed unless
 *      the dll is present.  Accelerator keys cause WM_COMMAND events for the
 *      menu item even if grayed, so I try to always check even if (for now) the menu
 *      item doesn't have an accelerator.
 ******************************************************************************/
static void OnFileDefaultOptions(WPARAM wCmd)
{
    int i;
    WPARAM nMenuCmd = IDM_FILE_DEFAULTOPTIONS;

    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        if ( pOpenOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( wCmd == nMenuCmd )
                {
                if ( pOpenOpTable[i]->nFoundParmVer != 0 )
                    (*pOpenOpTable[i]->pfnDefaultsDialog)();
                return;
                }
            nMenuCmd++;
            }
    
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        if ( pSaveOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( wCmd == nMenuCmd )
                {
                if ( pSaveOpTable[i]->nFoundParmVer != 0 )
                    (*pSaveOpTable[i]->pfnDefaultsDialog)();
                return;
                }
            nMenuCmd++;
            }            

    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        if ( pTransformOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( wCmd == nMenuCmd )
                {
                if ( pTransformOpTable[i]->nFoundParmVer != 0 )
                    (*pTransformOpTable[i]->pfnDefaultsDialog)();
                return;
                }
            nMenuCmd++;
            }            

    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        if ( pToolsOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( wCmd == nMenuCmd )
                {
                if ( pToolsOpTable[i]->nFoundParmVer != 0 )
                    (*pToolsOpTable[i]->pfnDefaultsDialog)();
                return;
                }
            nMenuCmd++;
            }            

    assert(FALSE);
}



/******************************************************************************
 *  void OnTransform(HWND hwnd, WPARAM wCmd)
 * 
 *  some Transform menu item selected
 *
 *  parameters:
 *      hwnd - window handle of active child, or NULL if none active
 *      wCmd - menu id of selected menu item
 ******************************************************************************/
static void OnTransform(HWND hwnd, WPARAM wCmd)
{                
    UINT i;
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    DWORD biCompression;

    if ( hwnd == 0 || pInstance == 0 )
        return;

    /* make sure that the wCmd entry is a valid transform entry */
    for ( i = 0; pTransformOpTable[i] != 0 && i != wCmd - IDM_TRANSFORM; i++ )
        ;
    assert(pTransformOpTable[i] != 0);
    if ( pTransformOpTable[i] != 0 &&   /* no need to gp when assert is not compiled in release */
         pTransformOpTable[i]->pfnTransform != 0 )
        {
        ChildWaitCursorOn(hwnd);
        biCompression = pInstance->SrcPicParm.Head.biCompression;
        if ( (*pTransformOpTable[i]->pfnTransform)(
                hwnd,
                pInstance->pszFilename,
                &pInstance->pSrc,
                &pInstance->dwSrcLen,
                &pInstance->SrcPicParm,
                pInstance->pDib,
                pInstance->dwDibLen,
                &pInstance->DibPicParm) )
            {            
            ChildWaitCursorOff(hwnd);
            pInstance = ChildGetInstanceData(hwnd);
            assert(pInstance != 0 );
            pInstance->bModified = TRUE;
            pInstance->bFormatModified =
                pInstance->SrcPicParm.Head.biCompression != biCompression;
            }
        ChildWaitCursorOff(hwnd);
        }
}



/******************************************************************************
 *  void OnTools(HWND hwnd, WPARAM wCmd)
 * 
 *  some Tools menu item selected
 *
 *  parameters:
 *      wCmd - menu id of selected menu item
 ******************************************************************************/
static void OnTools(WPARAM wCmd)
{
    int i;

    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        if ( wCmd == (WPARAM)( IDM_TOOLS + i ) )
            {
            if ( pToolsOpTable[i]->pfnTool != 0 )
                (*pToolsOpTable[i]->pfnTool)();
            break;
            }
    assert(pToolsOpTable[i] != 0);
}



/******************************************************************************
 *  void OnNewWindow(HWND hwnd)
 * 
 *  Window/New Window menu item selected
 *
 *  parameters:
 *      hwnd - window handle of active child window or NULL if none active
 ******************************************************************************/
static void OnNewWindow(HWND hwnd)
{                
    char sz[_MAX_PATH + 1];
    MDICREATESTRUCT mcs;
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    CHILDINSTANCEDATA PICFAR* pNew;

    if ( pInstance == 0 )
        return; /* Window/New Window should have been grayed */

    pNew = (CHILDINSTANCEDATA PICFAR*)MiscGlobalAllocPtr(
        sizeof(*pNew) + lstrlen(pInstance->pszFilename) + 1, IDS_NEWWINDOWOUTOFMEMORY);
        /* "There was not enough memory to create the new window." */
    if ( pNew == 0 )
        return;
        
    *pNew = *pInstance; /* this has the effect of duplicating the modified state
                            of the window as well as of duplicating the src picparm
                            Duplicating the src picparm this way works only because
                            we don't allocate comments, appfields, etc in the 
                            pegasusquery picparm, so it's self-contained */
    pNew->pszFilename  = (LPSTR)pNew + sizeof(*pNew);
    lstrcpy(pNew->pszFilename, pInstance->pszFilename);                            
    pNew->xWindowOrg   = 0;     /* not scrolled to start out */
    pNew->yWindowOrg   = 0;    
    pNew->bRButtonDown = FALSE;             /* should be FALSE anyway */
    assert(!pInstance->bRButtonDown);
    pNew->bLButtonDown = FALSE;             /* should be FALSE anyway */
    assert(!pInstance->bLButtonDown);
    pNew->bDisableSetScrollRanges = FALSE;  /* should be FALSE anyway */
    assert(!pInstance->bDisableSetScrollRanges);
    pNew->hPalette     = NULL;  /* this window will need its own hpalette */
    pNew->nWaitCursor  = 0;
    assert(pInstance->nWaitCursor == 0);

    /* duplicate the source image */
    pNew->pSrc = MiscGlobalAllocPtr(pNew->dwSrcLen, IDS_NEWWINDOWOUTOFMEMORY);
        /* "There was not enough memory to create the new window." */
    if ( pNew->pSrc == 0 )
        {
        MiscGlobalFreePtr(&pInstance);
        return;
        }
    hmemcpy(pNew->pSrc, pInstance->pSrc, pNew->dwSrcLen);

    /* duplicate the dib unless it's a pointer into the source image */
    if ( pInstance->pDib < pInstance->pSrc ||
         pInstance->pDib >= pInstance->pSrc + pInstance->dwSrcLen )
        {
        pNew->pDib = MiscGlobalAllocPtr(pNew->dwDibLen, IDS_NEWWINDOWOUTOFMEMORY);
            /* "There was not enough memory to create the new window." */
        if ( pNew->pDib == 0 )
            {
            MiscGlobalFreePtr(&pNew->pSrc);
            MiscGlobalFreePtr(&pInstance);
            return;
            }
        hmemcpy(pNew->pDib, pInstance->pDib, pNew->dwDibLen);
        }
    else
        pNew->pDib = pNew->pSrc + ( pInstance->pDib - pInstance->pSrc );

    /* duplicate the dib picparm - through the opcode because there may be comments
        or appfields or overtext or some such which need to be separately allocated,
        and which only the opcode knows enough to allocate
        if not implemented, then old is copied over new above when old instance is
        copied to new instance */
    if ( pNew->pOpenOp->pfnDupPicParm != 0 &&
         !(*pNew->pOpenOp->pfnDupPicParm)(&pInstance->DibPicParm, &pNew->DibPicParm) )
        {
        (*pNew->pOpenOp->pfnCleanup)(pNew->pDib, 0, 0);
        MiscGlobalFreePtr(&pNew->pSrc);
        MiscGlobalFreePtr(&pInstance);
        ErrorMessage(STYLE_ERROR, IDS_NEWWINDOWOUTOFMEMORY);
        /* "There was not enough memory to create the new window." */
        return;
        }

    /* duplicate image options settings */
    if ( !CtrlDataDup(
            (LPCTRLINITDATA)pInstance->pOpenData,
            (LPCTRLINITDATA PICFAR *)&pNew->pOpenData) )
        {
        (*pNew->pOpenOp->pfnCleanup)(pNew->pDib, &pInstance->DibPicParm, 0);
        MiscGlobalFreePtr(&pNew->pSrc);
        MiscGlobalFreePtr(&pInstance);
        ErrorMessage(STYLE_ERROR, IDS_NEWWINDOWOUTOFMEMORY);
        /* "There was not enough memory to create the new window." */
        return;
        }

    /* Create MDI child window */
    GetWindowText(hwnd, sz, sizeof(sz));
    mcs.szClass    = MDICHILD_CLASSNAME;
    mcs.szTitle    = pInstance->pszFilename;
    mcs.hOwner     = hinstThis;
    mcs.x          = CW_USEDEFAULT;
    mcs.y          = CW_USEDEFAULT;
    mcs.cx         = CW_USEDEFAULT;
    mcs.cy         = CW_USEDEFAULT;
    mcs.style      = WS_HSCROLL | WS_VSCROLL;
    mcs.lParam     = (LPARAM)pNew;
    if ( ChildGetActive() != NULL && IsZoomed(ChildGetActive()) )
        mcs.style |= WS_MAXIMIZE;
    // tell the MDI Client to create the child 
    LockWindowUpdate(hwndMDIClient);
    hwnd = FORWARD_WM_MDICREATE(hwndMDIClient, (LPMDICREATESTRUCT)&mcs, SendMessage);
    if ( hwnd == NULL )
        {
        LockWindowUpdate(NULL);
        (*pNew->pOpenOp->pfnCleanup)(pNew->pDib, &pInstance->DibPicParm, pNew->pOpenData);
        MiscGlobalFreePtr(&pNew->pSrc);
        MiscGlobalFreePtr(&pInstance);
        ErrorMessage(STYLE_ERROR, IDS_NEWWINDOWCREATE);
        /* "An unexpected error occurred creating the new window." */
        return;
        }
    ShowWindow(hwnd, SW_HIDE);
    LockWindowUpdate(NULL);
    UpdateWindow(hwndMDIClient);

    /* make sure the window isn't bigger than the image */
    ChildSetWindowSize(hwnd);
}
