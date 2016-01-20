/******************************************************************************
 *
 * MSAVE.C - Minerva MDI File/Save menu command handler
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
 *   SaveOnFileSave   - frame window File/Save menu command handler
 *   SaveOnFileSaveAs - frame window File/Save As menu command handler
 *   SaveGetSaveAs    - get file save as parameters using common file save dialog
 *   SaveImage        - Save the window's image
 *
 * Revision History:
 *   12-18-96  1.00.03  jrb  once-over and additional commenting
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
#include <dlgs.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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
#include "msave.h"
#include "mmisc.h"
#include "mopen.h"



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  void SaveOnFileSave(HWND hwnd)
 * 
 *  frame window File/Save menu command handler
 *
 *  parameters:
 *      hwnd - window handle of active child window or NULL of none active
 ******************************************************************************/
void SaveOnFileSave(HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    BYTE PICHUGE* pSavedImage;
    DWORD dwSavedLen;
    SAVEOP *pSaveOp;
    char szFilename[_MAX_PATH + 1];

    if ( hwnd == NULL || pInstance == NULL || !ChildIsModified(hwnd) )
        return; /* File/Save should have been grayed */
    
    pSaveOp = pInstance->pOpenOp->pSaveOp;
    lstrcpy(szFilename, pInstance->pszFilename);        
    if ( pSaveOp == 0 || pSaveOp->nFoundParmVer == 0 || pInstance->bFormatModified )
        {
        if ( !SaveGetSaveAs(szFilename, &pSaveOp) )
            return;
        }

    /* save the image, then reopen the image into the same window so the
        user can see the result of the save options */
    if ( SaveImage(hwnd, szFilename, pSaveOp, &pSavedImage, &dwSavedLen) )
        /* re-open from saved image into same window */
        OpenImage(hwnd, szFilename, pSavedImage, dwSavedLen, OP_SILENT | OP_INISETTINGS);
}



/******************************************************************************
 *  void SaveOnFileSaveAs(HWND hwnd)
 * 
 *  frame window File/Save As menu command handler
 *
 *  parameters:
 *      hwnd - window handle of active child window or NULL of none active
 *
 *  notes:
 *      uses common file save dialog
 *
 *      Determines the save opcode according to what filter the user chooses
 *      in the save dialog.  The file extension is whatever the user enters
 *      so it's possible for the user to specify a senseless extension.
 *
 *      constructs file filter list on the fly from save opcodes whose
 *      opcode dll is present.
 ******************************************************************************/
void SaveOnFileSaveAs(HWND hwnd)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    char szFilename[_MAX_PATH + 1];
    SAVEOP *pSaveOp;
    BYTE PICHUGE* pSavedImage;
    DWORD dwSavedLen;

    if ( hwnd == NULL || pInstance == NULL )
        return; /* File/Save As should have been grayed */

    lstrcpy(szFilename, pInstance->pszFilename);
    pSaveOp = pInstance->pOpenOp->pSaveOp;
    if ( !SaveGetSaveAs(szFilename, &pSaveOp) )
        return;

    /* save the image, then open the image into a new window so the
        user can see the result of the save options */
    if ( SaveImage(hwnd, szFilename, pSaveOp, &pSavedImage, &dwSavedLen) )
        /* re-open from saved image into new window */
        OpenImageWindow(szFilename, pSavedImage, dwSavedLen, OP_SILENT | OP_INISETTINGS);
}



/******************************************************************************
 *  BOOL SaveGetSaveAs(LPSTR pszFilename, SAVEOP **ppSaveOp)
 * 
 *  get file save as parameters using common file save dialog
 *
 *  parameters:
 *      pszFilename - default filename, receives selected filename
 *      ppSaveOp    - pointer to pointer to default save type def, receives selected
 *                    save type def
 *
 *  notes:
 *      uses common file save dialog
 *
 *      Determines the save opcode according to what filter the user chooses
 *      in the save dialog.  The file extension is whatever the user enters
 *      so it's possible for the user to specify a senseless extension.
 *
 *      constructs file filter list on the fly from save opcodes whose
 *      opcode dll is present.
 ******************************************************************************/

/* NOTE: SaveHook and SaveOnCommand do NOT have access to the default data segment in 16-bit
    Thus they can't access global data, or static data within the function and can't access
    string literal data (which is compiled into DGROUP) */

/******************************************************************************
    SaveOnCommand
    when the Type selection changes, replace the current filename extension
    (or append) with the first extension in the list for the new type -- unless
    the current extension already occurs in the list
 ******************************************************************************/
static void SaveOnTypeSelChange(LPSTR pszType, LPSTR pszFilename)
{
    LPSTR psz;
    LPSTR pszx;
    LPSTR pszDot;
    LPSTR pszStar;
    
    /* get pointer to the place in szFilename to stuff the ".ext" string */
    psz = pszFilename;
    if ( psz[0] != '\0' && psz[1] == ':' )
        psz += 2;
    pszx = _fstrrchr(psz, '//');
    psz = _fstrrchr(psz, '\\');
    if ( psz == NULL || ( pszx != NULL && psz < pszx ) )
        psz = pszx;
    if ( psz == NULL )
        psz = pszFilename;
    pszDot = _fstrrchr(psz, '.');
    if ( pszDot == NULL )
        pszDot = psz + lstrlen(psz);

    pszStar = NULL;
    while ( ( psz = _fstrrchr(pszType, '*') ) != NULL )
        {
        *psz = '\0';
        pszStar = ++psz;
        while ( *psz > ' ' && *psz != ')' && *psz != ';' && *psz != ',' )
            psz++;
        *psz = '\0';
        if ( lstrcmpi(pszStar, pszDot) == 0 )
            return;
        }
    assert(pszStar != NULL);
    if ( pszStar == NULL )
    {
        ErrorMessage(STYLE_ERROR, IDS_SAVEINTERNAL1);        
    }
    else
        lstrcpy(pszDot, pszStar);
}

/* NOTE: SaveHook, SaveOnCommand, SaveOnTypeSelChange don't have access to the default
    data segment in 16-bit.  Thus they can't access global data, or static data within
    the function and can't access string literal data (which is compiled into DGROUP) */
static void SaveOnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT notifyCode)
{
    NOREFERENCE(hwndCtrl);
    if ( id == cmb1 && notifyCode == CBN_SELCHANGE )
    {
        char szType[128];
        char szFilename[_MAX_PATH + 1];
        int i;

        i = GetDlgItemText(hWnd, cmb1, szType, sizeof(szType));
        assert(i < sizeof(szType));
        i = GetDlgItemText(hWnd, edt1, szFilename, sizeof(szFilename));
        assert(i < sizeof(szFilename));
        
        assert(szType[0] != '\0');
        if ( i > 0 && szType[0] != '\0' )
        {
            SaveOnTypeSelChange(szType, szFilename);
            SetDlgItemText(hWnd, edt1, szFilename);
        }
        else
        {
            if ( szType[0] == '\0' )
            {
                ErrorMessage(STYLE_ERROR, IDS_SAVEINTERNAL2);
            }
            /* else a null filename and nothing to do */
        }
    }
}

static UINT CALLBACK SaveHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ( uMsg == WM_COMMAND )
        {
        HANDLE_WM_COMMAND(hWnd, wParam, lParam, SaveOnCommand);
        }
#if defined(WIN32) && defined(WINVER) && WINVER >= 0x0400
    else if ( uMsg == WM_NOTIFY )
        {
        if ( ((LPOFNOTIFY)lParam)->hdr.code == CDN_TYPECHANGE )
            {
            char szFilename[_MAX_PATH + 1];
            char szType[128];
            LPOPENFILENAME lpofn = ((LPOFNOTIFY)lParam)->lpOFN;
            LPCSTR psz;
            UINT i;

            psz = lpofn->lpstrFilter;
            for ( i = 1 /* 1-based index */ ; *psz != '\0' && i < lpofn->nFilterIndex; i++ )
                {
                psz += strlen(psz) + 1;
                psz += strlen(psz) + 1;
                }
            strcpy(szType, psz);
            assert(szType[0] != '\0');
            if ( CommDlg_OpenSave_GetSpec(
                    GetParent(hWnd),
                    szFilename,
                    sizeof(szFilename)) > 1 && szType[0] != '\0' )
                {
                SaveOnTypeSelChange(szType, szFilename);
                CommDlg_OpenSave_SetControlText(GetParent(hWnd), edt1, szFilename);
                }
            else
                {
                /* else it's the null string or an error */
                if ( szType[0] == '\0' )
                    {
                    ErrorMessage(STYLE_ERROR, IDS_SAVEINTERNAL2);
                    }
                }
            }
        }
#endif
    return ( 0 );
}

BOOL SaveGetSaveAs(LPSTR pszFilename, SAVEOP **ppSaveOp)
{
    char szOpFilters[512];
    int len;
    int i;
    UINT ndx;
    char *psz;
    OPENFILENAME ofn;
    int FoundIndex;

    /* construct the file filter list */
    len = 0;
    ndx = 1;
    FoundIndex = 0;
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        {
        if ( pSaveOpTable[i]->nFoundParmVer != 0 && pSaveOpTable[i]->pszFilter != 0 )
            {
            assert(*pSaveOpTable[i]->pszFilter != '\0');
            lstrcpy(&szOpFilters[len], pSaveOpTable[i]->pszFilter);
            assert(sizeof(szOpFilters) > strlen(szOpFilters));
            len += strlen(&szOpFilters[len]);
            if ( *ppSaveOp == pSaveOpTable[i] )
                FoundIndex = ndx;
            ndx++;
            }
        }
    if ( len == 0 )
        return ( FALSE );   /* no file save opcodes present (File/Save As should have been grayed) */

    /* common file save dialog wants filter segments separated by '\0' */
    psz = szOpFilters;
    while ( ( psz = strchr(psz, '|') ) != NULL )
        *psz++ = '\0';

    /* popup connon file save dialog */
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = hwndFrame;
    ofn.lpstrFilter  = szOpFilters;
    ofn.lpstrFile    = pszFilename;
    ofn.nFilterIndex = FoundIndex;
    ofn.nMaxFile     = _MAX_PATH + 1;
    ofn.Flags        = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK | OFN_PATHMUSTEXIST;
    ofn.lpfnHook     = (UINT (CALLBACK*)(HWND, UINT, WPARAM, LPARAM))SaveHook;

#if defined(WIN32) && defined(WINVER) && WINVER >= 0x0400
    /* otherwise OFN_ENABLEHOOK will turn off the explorer style dialog */
    ofn.Flags |= OFN_EXPLORER;
#endif
    if ( !GetSaveFileName(&ofn) )
        return ( FALSE );

    /* find the opcode corresponding to the filter (we have to search because
        some opcodes in the table may not have filters)
        note that nFilterIndex is 1-based */
    for ( ndx = 0, i = 0; pSaveOpTable[i] != 0; i++ )
        {
        if ( pSaveOpTable[i]->nFoundParmVer != 0 && pSaveOpTable[i]->pszFilter != 0 )
            {
            if ( ++ndx == ofn.nFilterIndex )    /* ++ndx since nFilterIndex is 1-based */
                break;
            }
        }
    assert(pSaveOpTable[i] != 0);

    *ppSaveOp = pSaveOpTable[i];
    return ( TRUE );
}



/******************************************************************************
 *  void SaveImage(
 *      HWND                  hwnd,
 *      LPCSTR                pszFilename,
 *      SAVEOP               *pSaveOp,
 *      BYTE PICHUGE* PICFAR* ppSavedImage,
 *      DWORD PICFAR*         pdwSavedLen)
 * 
 *  Save the window's image
 *
 *  parameters:
 *      hwnd         - window handle of active child window or NULL of none active
 *      pszFilename  - file name to save to
 *      pSaveOp      - pointer to save opcode definition data
 *      ppSavedImage - receive pointer to saved file image in memory
 *      pdwSavedLen  - receive length of saved file image
 *
 *  notes:
 *      saves the image, then writes the saved image to a temporary file in the
 *      same directory as the target file.  If the save is successful, deletes
 *      the target file (in case it previously existed) and renames the temporary
 *      file to the target file name.
 ******************************************************************************/
BOOL SaveImage(
    HWND                  hwnd,
    LPCSTR                pszFilename,
    SAVEOP               *pSaveOp,
    BYTE PICHUGE* PICFAR* ppSavedImage,
    DWORD PICFAR*         pdwSavedLen)
{
    CHILDINSTANCEDATA PICFAR* pInstance = ChildGetInstanceData(hwnd);
    char sz[_MAX_PATH + 1];
    char szTemp[_MAX_PATH + 1];
    OFSTRUCT of;
    HFILE hf;
    LPSTR psz;
    LPSTR pszx;
    LPCSTR pszBaseName;
    DWORD dw;

    /* this opcode function creates a memory buffer holding a file image of the saved image */
    if ( IsWindow(hwnd) )
        ChildWaitCursorOn(hwnd);
    pszBaseName = pszFilename;
    if ( pszFilename[0] != '\0' && pszFilename[1] == ':' )
        pszBaseName = pszFilename + 2;
    psz = _fstrrchr(pszBaseName, '\\');
    pszx = _fstrrchr(pszBaseName, '//');
    if ( psz == NULL )
    {
        if ( pszx != NULL )
            pszBaseName = pszx + 1;
    }
    else if ( pszx == NULL || pszx < psz )
    {
        pszBaseName = psz + 1;
    }
    else /* pszx > psz */
    {
        pszBaseName = pszx + 1;
    }

    if ( pSaveOp->nFoundParmVer == 0 || !(*pSaveOp->pfnSave)(
            hwnd,
            pszBaseName,
            pInstance->pSrc,
            pInstance->dwSrcLen,
            &pInstance->SrcPicParm,
            pInstance->pDib,
            pInstance->dwDibLen,
            &pInstance->DibPicParm,
            ppSavedImage,
            pdwSavedLen) )
        {
        if ( IsWindow(hwnd) )
            ChildWaitCursorOff(hwnd);
        return ( FALSE );
        }
    if ( IsWindow(hwnd) )
        ChildWaitCursorOff(hwnd);

    /* create a temporary file in the same directory as pszFilename.  The temporary
        file name is derived from this minerva instance's instance handle and the
        current tick count so it is _highly_ likely to be unique.  In case it's not,
        a unique filename is generated. */
    lstrcpy(sz, pszFilename);          
    lstrcpy(szTemp, pszFilename);
    psz = strrchr(szTemp, '\\');
    assert(psz != NULL);
    psz++;
    dw = MiscTickCount();
    of.cBytes = sizeof(of);
    do
        {
        wsprintf(psz, "PICM%.4X.$%.2X", LOWORD((DWORD)(void PICFAR *)hinstThis), (BYTE)dw++);
        }
    while ( OpenFile(szTemp, &of, OF_EXIST) != HFILE_ERROR );

    /* there's a window here where a conflicting file name could have been created,
        and we are now truncating it to 0 length. So I prefixed it with Pic in the
        hopes that it would be another PIC application's file and added an M in the
        so it would be another minerva instance (and presume that the instance handle
        mod 4K makes that impossible)
        The problem with using the C functions to do the same thing is, either they
        aren't as portable (UNIX, not ASCII) or they have the same window, or we
        can't specify the directory for the file and we'd have to copy the result
        to this directory. */
    hf = OpenFile(szTemp, &of, OF_CREATE | OF_WRITE);
    if ( hf == HFILE_ERROR )
        {
        MiscGlobalFreePtr(ppSavedImage);
        ErrorFileMessage(IDS_IMAGECREATE, of.nErrCode, (LPCSTR)szTemp);
        /* "An error occured creating a temporary image file %s." */
        return ( FALSE );
        }
    dw = _hwrite(hf, (LPCSTR)*ppSavedImage, *pdwSavedLen);
    if ( dw == (DWORD)-1 )
        {
        _lclose(hf);
        remove(szTemp);
        MiscGlobalFreePtr(ppSavedImage);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEWRITE);
        /* "An unexpected error occurred writing to a temporary image file." */
        return ( FALSE );
        }
    else if ( dw != *pdwSavedLen )
        {
        _lclose(hf);
        remove(szTemp);
        MiscGlobalFreePtr(ppSavedImage);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEWRITE_DISKFULL, (LPCSTR)sz);
        /* "An error occurred writing to a temporary image file.  The disk appears to be full." */
        return ( FALSE );
        }
    hf = _lclose(hf);
    assert(hf != HFILE_ERROR );
    if ( remove(sz) != 0 && errno != ENOENT )
        { /* for any error except file not found */
        MiscGlobalFreePtr(ppSavedImage);
        ErrorMessage(STYLE_ERROR, IDS_IMAGEREMOVE, (LPCSTR)sz);
        /* "An unexpected error occurred renaming a temporary image file to %s."
            The file %s may be a read-only file or it may be in use by another application." */
        return ( FALSE );
        }
    if ( rename(szTemp, sz) != 0 )
        {
        MiscGlobalFreePtr(ppSavedImage);
        ErrorMessage(STYLE_ERROR, IDS_IMAGERENAME, (LPCSTR)sz);
        /* "An unexpected error occured renaming a temporary image file to %s." */
        return ( FALSE );
        }

    pInstance->bModified = FALSE;
    pInstance->bFormatModified = FALSE;
    return ( TRUE );
}
