/******************************************************************************
 *
 *  MDLG.C - Minerva dialog box utility functions
 *
 *  Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 *  This source code is provided to you as sample source code.  Pegasus Imaging
 *  Corporation grants you the right to use this source code in any way you
 *  wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 *  disavows any responsibility for any and every use you make of this source
 *  code.
 *
 * Global functions:
 *     DlgCenterInOwner - center a dialog in its owner's window rect
 *     DlgSetModified   - Set modified flag TRUE in dialog instance data
 *     DlgClearModified - Clear modified flag FALSE in dialog instance data
 *     DlgDoModal       - modal dialog from a template resource id
 *     DlgOnOK          - update client control data from dialog controls
 *
 * Local functions:
 *     OnInitDialog     - WM_INITDIALOG processing
 *     DlgProc          - dialog proc
 * 
 * Revision History:
 *   12-18-96  1.00.03  jrb  once-over and additional commenting
 *                           I decided to initialize inactive controls-- even
 *                             though they're grayed and don't matter, maybe it's
 *                             better for them to show what the defaults would have
 *                             been
 *   12-18-96  1.00.02  jrb  don't allow centering to move the dialog off screen
 *   11-30-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <assert.h>
#include <stdlib.h>

#pragma warning(disable:4001)
/* PIC includes */
#include "pic.h"

/* minerva includes */
#include "resource.h"
#include "mctrl.h"
#include "mdlg.h"
#include "mopcodes.h"
#include "mmisc.h"
#include "minerva.h" 



/******************************************************************************
 *  local data structures
 ******************************************************************************/



/* dialog instance data obtained as:
    (DLGUSERDATA PICFAR*)GetWindowLong(hwnd, DWL_USER); */
typedef struct _DlgUserData
    {
    BOOL            bModified;      /* set TRUE/FALSE by DlgSetModified, DlgClearModified */
    PFNDLGONCOMMAND pfnOnCommand;   /* client function for WM_COMMAND processing */
    LPCTRLINITDATA  arInitData;     /* client control data array for init, and OK button */
    void PICFAR*    pUserData;      /* client arbitrary data */
    LPCSTR          pszDlgTemplate; /* dialog template name or resource id */
    } DLGUSERDATA;



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void OnInitDialog(HWND hwndDlg, DLGUSERDATA PICFAR *pDud);
static BOOL CALLBACK DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL OnCommand(HWND hwndDlg, int nCtrlID, HWND hwndCtrl, UINT wNotify);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  void DlgCenterInOwner(HWND hwndDlg)
 * 
 *  center a dialog in its owner's window rect
 *
 *  parameters:
 *      hwndDlg - HWND for dialog
 ******************************************************************************/
void DlgCenterInOwner(HWND hWnd)
{
    RECT rect;
    HWND hwndOwner;
    RECT rectOwner;
    int nWidth;
    int nHeight;
    POINT pt;

    GetWindowRect(hWnd, &rect);
    hwndOwner = GetWindow(hWnd, GW_OWNER);
    if ( hwndOwner == NULL )
        hwndOwner = GetDesktopWindow();
    GetWindowRect(hwndOwner, &rectOwner);
    nWidth = rect.right - rect.left;
    nHeight = rect.bottom - rect.top;
    pt.x = rectOwner.left + ( rectOwner.right - rectOwner.left - nWidth ) / 2;
    pt.y = rectOwner.top + ( rectOwner.bottom - rectOwner.top - nHeight ) / 2;

    /* but make sure it's not off the screen */
    if ( pt.x + nWidth > GetSystemMetrics(SM_CXSCREEN) )
        pt.x = GetSystemMetrics(SM_CXSCREEN) - nWidth;
    if ( pt.y + nHeight > GetSystemMetrics(SM_CYSCREEN) )
        pt.y = GetSystemMetrics(SM_CYSCREEN) - nHeight;
    if ( pt.x < 0 )
        pt.x = 0;
    if ( pt.y < 0 )
        pt.y = 0;

    MoveWindow(hWnd, pt.x, pt.y, nWidth, nHeight, FALSE);
}



/******************************************************************************
 *  void DlgSetModified(HWND hwndDlg)
 * 
 *  Set modified flag TRUE in dialog instance data
 *
 *  parameters:
 *      hwndDlg - HWND for dialog
 *
 *  notes:
 *      Set modified flag TRUE
 *
 *      If modified flag is FALSE, then Cancel button
 *      processing is done if the OK button is pressed.
 ******************************************************************************/
void DlgSetModified(HWND hwndDlg)
{
    DLGUSERDATA PICFAR* pDud = (DLGUSERDATA PICFAR*)GetWindowLong(hwndDlg, DWL_USER);
    pDud->bModified = TRUE;
}



/******************************************************************************
 *  void DlgClearModified(HWND hwndDlg)
 * 
 *  Clear modified flag FALSE in dialog instance data
 *
 *  parameters:
 *      hwndDlg - HWND for dialog
 *
 *  notes:
 *      set modified flag FALSE.
 * 
 *      If modified flag is FALSE, then Cancel button
 *      processing is done if the OK button is pressed.
 ******************************************************************************/
void DlgClearModified(HWND hwndDlg)
{
    DLGUSERDATA PICFAR* pDud = (DLGUSERDATA PICFAR*)GetWindowLong(hwndDlg, DWL_USER);
    pDud->bModified = FALSE;
}



/******************************************************************************
 *  int DlgDoModal(
 *      HWND hwnd,
 *      LPCSTR pszDlgName,
 *      PFNDLGONCOMMAND pfnOnCommand,
 *      LPCTRLINITDATA arInitData,
 *      void PICFAR* pUserData)
 * 
 *  modal dialog from a template resource id
 *
 *  parameters:
 *      hwnd         - parent window
 *      pszDlgName   - resource id or name for dialog
 *      pfnOnCommand - client WM_COMMAND processing
 *      arInitData   - client control init data (see mdlgctrl.c)
 *      pUserData    - client user data
 *
 *  returns:
 *      the exit code parameter to EndDialog.  Ordinarily IDOK or IDCANCEL.  It
 *      can also be -1 if an error occurs creating the dialog (as returned by
 *      DialogBoxParam)
 ******************************************************************************/
int DlgDoModal(
    HWND            hwnd,
    LPCSTR          pszDlgName,
    PFNDLGONCOMMAND pfnOnCommand,
    LPCTRLINITDATA  arInitData,
    void PICFAR*    pUserData)
{
    DLGUSERDATA dud;
    
    dud.bModified      = FALSE;
    dud.pfnOnCommand   = pfnOnCommand;
    dud.arInitData     = arInitData;
    dud.pUserData      = pUserData;
    dud.pszDlgTemplate = pszDlgName;
    
    return ( DialogBoxParam(
        hinstThis,
        pszDlgName,
        hwnd,
        DlgProc,
        (LPARAM)(DLGUSERDATA PICFAR *)&dud) );
}



/******************************************************************************
 *  void DlgOnOK(HWND hwndDlg)
 * 
 *  update client control data from dialog controls
 *
 *  parameters:
 *      hwndDlg - HWND of dialog window
 *
 *  notes:
 *      copies control values to the client's control data array so they're
 *      available when the dialog is closed
 *
 *      This function is visible globally so the client can process Apply button
 *      presses similary to OK button presses without closing the dialog.  It would
 *      be nice if the processing could be identical, but I like the appearance
 *      better if the OK button doesn't reopen the image until after the dialog
 *      goes away and calling EndDialog in this module instead of in the client procedure
 *      gives us better control of dialog box stuff lifetimes
 ******************************************************************************/
void DlgOnOK(HWND hwndDlg)
{
    DLGUSERDATA PICFAR* pDud = (DLGUSERDATA PICFAR*)GetWindowLong(hwndDlg, DWL_USER);
    LPCTRLINITDATA ar = (LPCTRLINITDATA)pDud->arInitData;
    char sz[_MAX_PATH + 1];
    int i;

    for ( i = 0; ar[i].eType != eEnd; i++ )
        {
        if ( !ar[i].bInactive )
            {
            switch ( ar[i].eType )
                {
                case eDialog:
                    /* nothing to do */
                    break;
                case eText:
                    /* nothing to do */
                    break;
                case eGroupBox:
                    /* nothing to do */
                    break;
                case ePushButton:
                    /* nothing to do */
                    break;
                case eListBox:
                    /* nothing to do yet */
                    break;
                            
                case eRadioButton:
                    CtrlDataSetPushed(ar, ar[i].nID, IsDlgButtonChecked(hwndDlg, ar[i].nID));
                    break;

                case eCheckBox:
                    CtrlDataSetPushed(ar, ar[i].nID, IsDlgButtonChecked(hwndDlg, ar[i].nID));
                    break;

                case eEdit:
                    GetDlgItemText(hwndDlg, ar[i].nID, sz, sizeof(sz));
                    CtrlDataSetText(ar, ar[i].nID, sz);
                    break;
            
                case eScrollBar:        /* not implemented */
                case eComboSimple:      /* not implemented */
                case eComboDropDown:    /* not implemented */
                case eComboDropList:    /* not implemented */
                default:
                    assert(FALSE);
                    break;
                }
            }
        }
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void OnInitDialog(HWND hwndDlg, DLGUSERDATA PICFAR *pDud)
 * 
 *  WM_INITDIALOG processing
 *
 *  parameters:
 *      hwndDlg - HWND of dialog window
 *
 *  notes:
 *      WM_INITDIALOG processing
 *
 *      copies client's control data array values to the dialog box controls
 *      and centers the dialog in the frame window
 ******************************************************************************/
static void OnInitDialog(HWND hwndDlg, DLGUSERDATA PICFAR *pDud)
{
    LPCTRLINITDATA ar = (LPCTRLINITDATA)pDud->arInitData;
    int i;
    HWND hwndCtrl = 0;

    for ( i = 0; ar[i].eType != eEnd; i++ )
        {
        if ( ar[i].eType != eDialog )
            hwndCtrl = GetDlgItem(hwndDlg, ar[i].nID);

        if ( ar[i].bInactive )
             EnableWindow(hwndCtrl, FALSE);

        switch ( ar[i].eType )
            {
            case eText:
                if ( ar[i].pszText != 0 )
                    SetDlgItemText(hwndDlg, ar[i].nID, ar[i].pszText);
                break;

            case eDialog:
                /* nothing to do */
                break;
            case eGroupBox:
                /* nothing to do */
                break;
            case ePushButton:
                /* nothing to do */
                break;
            case eListBox:
                /* nothing to do */
                break;

            case eRadioButton:
                CheckDlgButton(hwndDlg, ar[i].nID, ar[i].u.bPushed);
                break;

            case eCheckBox:
                CheckDlgButton(hwndDlg, ar[i].nID, ar[i].u.bPushed);
                break;

            case eEdit:
                if ( ar[i].wLimitText != 0 )
                    SendMessage(
                        GetDlgItem(hwndDlg, ar[i].nID),
                        EM_LIMITTEXT,
                        ar[i].wLimitText,
                        0);
                SetDlgItemText(hwndDlg, ar[i].nID, ar[i].pszText);
                break;
            
            case eScrollBar:        /* not implemented */
            case eComboSimple:      /* not implemented */
            case eComboDropDown:    /* not implemented */
            case eComboDropList:    /* not implemented */
            default:
                assert(FALSE);
                break;            

            }
        }
    DlgClearModified(hwndDlg);
    DlgCenterInOwner(hwndDlg);
}



/******************************************************************************
 *  BOOL CALLBACK DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
 * 
 *  dialog proc
 *
 *  parameters:
 *      hwndDlg - HWND of dialog window
 *      msg     - windows WM_* message
 *      wParam, lParam - depend on msg
 *
 *  return:
 *      FALSE if we don't want the dialog manager to do anything about the message
 *      (we've handled it).  _except_ for WM_INITDIALOG where FALSE means the 
 *      dialog manager shouldn't set focus to the first control in the tab order
 *      because we've set focus (which we don't)
 *
 *  notes:
 *      OK button press returns a different code if the dialog
 *      data hasn't been modified or has been Apply'ed.  This allows
 *      Image Options to decide not to refresh the image, although
 *      most others (e.g. Save Options) want to to the same thing
 *      for OK_UNCHANGED as for OK.
 *
 *      WM_INITDIALOG is passed to the OnCommand handler as though a
 *      WM_COMMAND event occured with a control id of -1.  Kludgey, but
 *      I really didn't want to pass two function pointers to DlgDoModal().
 *      And I ran into something, not totally unexpected in hindsight, where
 *      the dialog had to evaluate the gray state of the controls at init time.
 ******************************************************************************/
static BOOL CALLBACK DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DLGUSERDATA PICFAR* pDud;

    switch ( msg )
        {
        case WM_INITDIALOG:
            SetWindowLong(hwndDlg, DWL_USER, lParam);
            pDud = (DLGUSERDATA PICFAR*)lParam;
            OnInitDialog(hwndDlg, pDud);
            if ( pDud->pfnOnCommand != 0 )
                (*pDud->pfnOnCommand)(hwndDlg, -1, 0, pDud->arInitData, pDud->pszDlgTemplate, pDud->pUserData);
            return ( TRUE );    /* to set focus to wParam control HWND */

        case WM_COMMAND:
            return ( (BOOL)HANDLE_WM_COMMAND(hwndDlg, wParam, lParam, OnCommand) );

        default:
            break;
        }
    return ( FALSE );
}


static BOOL OnCommand(HWND hwndDlg, int nCtrlID, HWND hwndCtrl, UINT wNotify)
{
    DLGUSERDATA PICFAR* pDud;
    BOOL bRet = FALSE;
    LPCTRLINITDATA pCtrl;
    LPCTRLINITDATA pDlg;
    DLGUSERDATA dud;

    NOREFERENCE(hwndCtrl);

    pDud = (DLGUSERDATA PICFAR*)GetWindowLong(hwndDlg, DWL_USER);
 
    /* then pass event to client's WM_COMMAND handler */
    if ( pDud->pfnOnCommand != 0 )
        {
        if ( nCtrlID == IDOK && wNotify == BN_CLICKED )
            /* if the OK button is pressed as the default button, then
                the focus changes needed to do some dialog processing
                hasn't necessarily occurred yet -- make it so */
            SetFocus(GetDlgItem(hwndDlg, IDOK));
                    
        bRet = (*pDud->pfnOnCommand)(
            hwndDlg,
            nCtrlID,
            (WORD)wNotify,
            pDud->arInitData,
            pDud->pszDlgTemplate,
            pDud->pUserData);
        /* if oncommand handler doesn't handle it, and it's a bn_clicked event
            for a button or an en_change event for an edit, and the control is
            in the arInitData array, then mark the dialog modified.  Thus
            changeable controls need to be in the ctrl array -- but don't need
            to be explicitly handled */
        if ( !bRet )
            {
            pCtrl = CtrlData(pDud->arInitData, nCtrlID);
            if ( pCtrl != NULL )
                {
                switch ( pCtrl->eType )
                    {
                    case eEdit:
                        if ( wNotify == EN_CHANGE )
                            DlgSetModified(hwndDlg);
                        break;
                    case ePushButton:
                        if ( wNotify == BN_CLICKED )
                            {
                            if ( pCtrl->wLimitText == 0 )
                                DlgSetModified(hwndDlg);
                            else
                                {
                                pDlg = CtrlData(pDud->arInitData, pCtrl->wLimitText);
                                assert(pDlg != 0 &&
                                       pDlg->eType == eDialog &&
                                       pDlg->wLimitText == (UINT)nCtrlID);
    
                                dud.bModified      = FALSE;
                                dud.pfnOnCommand   = pDud->pfnOnCommand;
                                dud.arInitData     = (LPCTRLINITDATA)pDlg->pszText;
                                dud.pUserData      = pDud->pUserData;
                                dud.pszDlgTemplate = MAKEINTRESOURCE(pDlg->nID);
                                if ( DialogBoxParam(
                                        hinstThis,
                                        MAKEINTRESOURCE(pDlg->nID),
                                        hwndDlg,
                                        DlgProc,
                                        (LPARAM)(DLGUSERDATA PICFAR*)&dud) == IDOK &&
                                     dud.bModified )
                                    DlgSetModified(hwndDlg);
                                }
                            }
                        break;
                    case eCheckBox:
                    case eRadioButton:
                        if ( wNotify == BN_CLICKED )
                            DlgSetModified(hwndDlg);
                        break;
                    default:
                        assert(FALSE);
                        break;
                    }
               }
           }
        }                
                                
    if ( nCtrlID == IDOK )
        {
        if ( !pDud->bModified )
            nCtrlID = IDOK_UNCHANGED;
        /* if we didn't define an OnCommand handler (like a properties dialog)
            we still want the OK button to close the box.
            IDOK and bRet == FALSE with an OnCommand handler means we failed
            a validation check and we don't want to close the box */
        if ( pDud->pfnOnCommand == 0 || bRet )
            {
            DlgOnOK(hwndDlg);
            EndDialog(hwndDlg, nCtrlID);
            }
        return ( TRUE );
        }

    if ( nCtrlID == IDCANCEL )
        {
        EndDialog(hwndDlg, nCtrlID);
        return ( TRUE );
        }

    return ( bRet );
}
