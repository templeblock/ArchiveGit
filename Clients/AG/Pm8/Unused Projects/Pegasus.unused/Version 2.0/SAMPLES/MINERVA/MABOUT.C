/*****************************************************************************
 *
 * MABOUT.C - Minerva MDI about box
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
 *     AboutOnHelpAbout - Help/About
 *
 * Local functions:
 *     OnInitAbout      - WM_INITDIALOG handler for about box
 *     AboutProc        - dialog procedure for about box
 *
 * Revision History:
 *   12-19-96  1.00.03  jrb  take out lOpcode != 0 test, pszAbout != 0 subsumes it
 *   12-18-96  1.00.03  jrb  once-over and some more comments
 *                           display an error message in the unlikely event the
 *                             about box can't be created or initialized
 *                           enlarge dialog to hold opcode list by the same amount
 *                             the list was enlarged rather than hard-coding an
 *                             offset between the bottom of the dialog and the
 *                             bottom of the list
 *   11-26-96  1.00.00  jrb  created
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
#ifdef WIN32
#include <winver.h>
#else
#include <ver.h>
#endif

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
#include "mabout.h"
#include "mmisc.h"
#include "mdlg.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL OnInitAbout(HWND hwndDlg);
static BOOL CALLBACK AboutProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  void OnHelpAbout(void)
 * 
 *  Help/About
 ******************************************************************************/
void AboutOnHelpAbout(void)
{
    if ( DialogBoxParam(hinstThis, MAKEINTRESOURCE(IDD_ABOUT), hwndFrame, (DLGPROC)AboutProc, 0) == -1 )
        ErrorMessage(STYLE_ERROR, IDS_ABOUTERROR);
        /* "An unexpected error occurred opening the about box." */
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL OnInitAbout(HWND hwndDlg)
 * 
 *  WM_INITDIALOG handler for about box
 *
 *  parameters:
 *      hwndDlg - window handle for about box
 *
 *  returns:
 *      FALSE if an error occurs initializing the about box controls, else TRUE
 *
 *  notes:
 *      Version string is constructed from the FileVersion entry in the version
 *      resource just so we only have to change it one place (in the .RC file)
 *
 *      Executable description and copyright also come from the version resource
 *
 *      List box has one line for each loaded opcode.  The legend for the opcode
 *      comes from the respective opcode table entry (e.g., pOpenOpTable[2]).
 *      The construction of the DLL filename is hard-coded here (PICN%.2lu%.2u)
 *      In so doing, we're also hard-coding the dependency between the opcode
 *      number in the DLL name and the actual opcode stuffed in PIC_PARM.Op.
 *
 *      After the list box is loaded, the height to display all the items without
 *      scrolling is computed and the list box and dialog are sized as needed, then
 *      the dialog is centered in the frame
 ******************************************************************************/
static BOOL OnInitAbout(HWND hwndDlg)
{
    char sz[_MAX_PATH + 1];
    char szFmt[256];
    char szVersion[256];
    LPVOID pvVersionData;
    DWORD  handle;
    DWORD  dwSize;
    LPBYTE p;
    char *psz;
    UINT uSize;
    int i;
    HWND hwndList;
    int nHeight;
    int nCount;
    RECT rectW;
    RECT rectC;
    POINT ptTlc;
    int incrHeight;
    /* following because fully-optimized, large-model MSC puts a literal string
        compiled into the call to VerQueryValue into the code segment which GP's
        because, even though the function is prototyped with LPCSTR for that parameter,
        MS actually modifies, then restores the string in the bowels of VerQueryValue */        
    static char szVarFileInfoTranslation[] = "\\VarFileInfo\\Translation";

    /**************************************************************************
        read version, copyright, and description from the version resource
    */

    i = GetModuleFileName(hinstThis, sz, sizeof(sz));
    assert(i < sizeof(sz) - 1);
    dwSize = GetFileVersionInfoSize(sz, &handle);
    if ( dwSize == 0 )
        return ( FALSE );
    pvVersionData = GlobalAllocPtr(GMEM_MOVEABLE, dwSize);
    if ( pvVersionData == 0 )
        return ( FALSE );
    if ( GetFileVersionInfo(sz, handle, dwSize, pvVersionData) &&
            VerQueryValue(pvVersionData, szVarFileInfoTranslation, &(LPVOID)p, &uSize) )
          {
          /* following constructs the language id for the version resource -- this
            is very poorly documented in the SDK and the SDK help example is incorrect. */
        wsprintf(sz, "\\StringFileInfo\\%.4x%.4x\\", ((LPWORD)p)[0], ((LPWORD)p)[1]);
        psz = sz + strlen(sz);
        /* now psz -> following trailing \\ in "\\StringFieldInfo\\%4.x%.4x\\" so we
            can copy partial keys to that location and use sz as a full key path for
            whatever we're looking for */
        
        strcpy(psz, "FileDescription");
          VerQueryValue(pvVersionData, sz, &(LPVOID)p, &uSize);
        SetDlgItemText(hwndDlg, IDC_ABOUT_FILEDESCRIPTION, (LPCSTR)p);
        
        strcpy(psz, "FileVersion");
          VerQueryValue(pvVersionData, sz, &(LPVOID)p, &uSize);
        /* This is a little wierd.  The static text in the dialog resource is
            a printf format string so we an put the FileVersion and ParmVer
            numbers in the correct places.  Otherwise we'd have to have a bunch
            of separate static strings */
        GetDlgItemText(hwndDlg, IDC_ABOUT_VERSION, szFmt, sizeof(szFmt));
#ifndef NDEBUG
        strcpy(szVersion, "Debug ");
#else
        szVersion[0] = '\0';
#endif
        wsprintf(szVersion + strlen(szVersion), szFmt, p, CURRENT_PARMVER / 10, CURRENT_PARMVER % 10);
        SetDlgItemText(hwndDlg, IDC_ABOUT_VERSION, szVersion);

        strcpy(psz, "LegalCopyright");
          VerQueryValue(pvVersionData, sz, &(LPVOID)p, &uSize);
        SetDlgItemText(hwndDlg, IDC_ABOUT_LEGALCOPYRIGHT, (LPCSTR)p);
          MiscGlobalFreePtr(&pvVersionData);
          }
     else
          {
          /* some error with the version resource */
          MiscGlobalFreePtr(&pvVersionData);
          return ( FALSE );
        }

    /* initialize szFmt with memory model and executable format */
#if defined(WIN32)
    i = LoadString(hinstThis, IDS_WIN32, szFmt, sizeof(szFmt));
    assert(i != 0 && i < sizeof(szFmt) - 1);
#elif defined(__MEDIUM__) || defined(_M_I86MM)
    i = LoadString(hinstThis, IDS_WIN16MEDIUM, szFmt, sizeof(szFmt));
    assert(i != 0 && i < sizeof(szFmt) - 1);
#elif defined(__LARGE__) || defined(_M_I86LM)
    i = LoadString(hinstThis, IDS_WIN16LARGE, szFmt, sizeof(szFmt));
    assert(i != 0 && i < sizeof(szFmt) - 1);
#else
    assert(FALSE);
#endif

    /* initialize szVersion with a description of the compiler */
#if   defined(_MSC_VER)
    i = LoadString(hinstThis, IDS_MSC, szVersion, sizeof(szVersion));
    assert(i != 0 && i < sizeof(szVersion) - 1);
#elif defined(__BORLANDC__)
    i = LoadString(hinstThis, IDS_BORLANDC, szVersion, sizeof(szVersion));
    assert(i != 0 && i < sizeof(szVersion) - 1);
#elif defined(__WATCOMC__)
    i = LoadString(hinstThis, IDS_WATCOMC, szVersion, sizeof(szVersion));
    assert(i != 0 && i < sizeof(szVersion) - 1);
#else
    assert(FALSE);
    i = LoadString(hinstThis, IDS_UNKNOWNC, szVersion, sizeof(szVersion));
    assert(i != 0 && i < sizeof(szVersion) - 1);
#endif

    wsprintf(sz, szFmt, (LPCSTR)szVersion);
    assert(strlen(sz) < sizeof(sz));
    SetDlgItemText(hwndDlg, IDC_ABOUT_COMPILERMODEL, sz);

    /**************************************************************************
        tally up opcode dll's available and add each to the list box
    */
            
    hwndList = GetDlgItem(hwndDlg, IDC_ABOUT_LIBRARIES);
    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {
        if ( pOpenOpTable[i]->pszAbout != 0 && pOpenOpTable[i]->nFoundParmVer != 0 )
            {
            wsprintf(
                sz,
                "PICN%.2lu%.2u: %s",
                pOpenOpTable[i]->lOpcode / 1000,
                CURRENT_PARMVER,
                (LPSTR)pOpenOpTable[i]->pszAbout);
            assert(strlen(sz) < sizeof(sz));
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)sz);
            }
        }
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        {
        if ( pSaveOpTable[i]->pszAbout != 0 && pSaveOpTable[i]->nFoundParmVer != 0 )
            {
            wsprintf(
                sz,
                "PICN%.2lu%.2u: %s",
                pSaveOpTable[i]->lOpcode / 1000,
                CURRENT_PARMVER,
                (LPSTR)pSaveOpTable[i]->pszAbout); 
            assert(strlen(sz) < sizeof(sz));
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)sz);
            }
        }
    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        if ( pTransformOpTable[i]->pszAbout != 0 && pTransformOpTable[i]->nFoundParmVer != 0 )
            {
            wsprintf(
                sz,
                "PICN%.2lu%.2u: %s",
                pTransformOpTable[i]->lOpcode / 1000,
                CURRENT_PARMVER,
                (LPSTR)pTransformOpTable[i]->pszAbout); 
            assert(strlen(sz) < sizeof(sz));
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)sz);
            }
        }
    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        {
        if ( pToolsOpTable[i]->pszAbout != 0 && pToolsOpTable[i]->nFoundParmVer != 0 )
            {
            wsprintf(
                sz,
                "PICN%.2lu%.2u: %s",
                pToolsOpTable[i]->lOpcode / 1000,
                CURRENT_PARMVER,
                (LPSTR)pToolsOpTable[i]->pszAbout); 
            assert(strlen(sz) < sizeof(sz));
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)sz);
            }
        }
    
    /**************************************************************************
        compute list box height and dialog height with all present opcode dll's
        so that scrolling isn't necessary.  Clearly at some number of opcodes,
        this isn't practical but today it is
    */

    nHeight = (int)SendMessage(hwndList, LB_GETITEMHEIGHT, 0, 0);
    nCount =  (int)SendMessage(hwndList, LB_GETCOUNT, 0, 0);
    GetWindowRect(hwndList, &rectW);
    GetClientRect(hwndList, &rectC);
    incrHeight = nHeight * nCount - rectC.bottom;
    rectW.bottom += incrHeight;
 
    /* make the list box that much bigger */
    ptTlc.x = rectW.left;
    ptTlc.y = rectW.top;
    ScreenToClient(hwndDlg, &ptTlc);    /* movewindow of child window is in parent coordinates */
    MoveWindow(
        hwndList,
        ptTlc.x,
        ptTlc.y,
        rectW.right - rectW.left,
        rectW.bottom - rectW.top,
        FALSE);

    /* make the dialog box that much bigger */
    GetWindowRect(hwndDlg, &rectW);
    GetClientRect(hwndDlg, &rectC);
    rectW.bottom += incrHeight;
    MoveWindow(                         /* movewindow of popup is in screen coordinates */
        hwndDlg,
        rectW.left,
        rectW.top,
        rectW.right - rectW.left,
        rectW.bottom - rectW.top,
        FALSE);

    /* center dialog on Minerva's main window */
    DlgCenterInOwner(hwndDlg);
    return ( TRUE );
}



/******************************************************************************
 *  BOOL AboutProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
 * 
 *  dialog procedure for about box
 *
 *  parameters:
 *      hwndDlg - window handle for about box
 *      uMsg    - message id
 *      wParam, lParam - depends on uMsg
 *
 *  returns:
 *      TRUE if it has handled the message, else FALSE, except WM_INITDIALOG
 *      FALSE if SetFocus has been used (about doesn't)
 ******************************************************************************/
static BOOL CALLBACK AboutProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NOREFERENCE(lParam);

    switch ( msg )
        {
        case WM_INITDIALOG:
            if ( !OnInitAbout(hwndDlg) )
                EndDialog(hwndDlg, -1);     /* returning error status to caller */
            return ( TRUE );

        case WM_COMMAND:
            if ( wParam == IDOK || wParam == IDCANCEL )
                {
                EndDialog(hwndDlg, wParam);
                return ( TRUE );
                }
            break;

        default:
            break;
        }
    return ( FALSE );
}
