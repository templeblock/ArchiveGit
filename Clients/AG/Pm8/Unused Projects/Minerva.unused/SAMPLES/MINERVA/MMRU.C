/******************************************************************************
 *
 * MMRU.C - Minerva mru file utility routines
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
 *     MruLoadAndAllocList - load mru list from application ini file
 *     MruSaveAndFreeList  - save mru list to application ini file
 *     MruGetFilename      - retrieve mru file entry corresponding to #
 *     MruDeleteFilename   - delete an mru file entry
 *     MruSetFirstFilename - set a filename as the first mru file, moving the rest down
 *
 * Local functions:
 *     SetMruMenu          - update the File menu according to the changed mru list
 *
 * Revision History:
 *   12-18-96  1.00.03  jrb  once-over and additional commenting
 *   12-09-96  1.00.01  jrb  deleting the last mru file would not mark it deleted
 *                           also wasn't taking account of maximized child window
 *                           in finding the File menu.
 *   12-08-96  1.00.00  jrb  created
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
/* pic includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "mmru.h"
#include "mmisc.h"



/******************************************************************************
 *  local data
 ******************************************************************************/



#ifdef _MT
CRITICAL_SECTION csMru;
#endif



static char arszMruFilenames[MAX_MRUFILES][_MAX_PATH + 1];



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void SetMruMenu(HWND hwnd);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL MruLoadList(HWND hwnd, LPCSTR pszIniFilename)
 * 
 *  initialize mru list in memory, loading it from an ini file
 *
 *  parameters:
 *      hwnd           - window with menu bar to which to add mru list files
 *      pszIniFilename - application ini file path/name
 *
 *  returns:  
 *      TRUE always (but if it were actually allocating anything it might have
 *      to return FALSE if the allocation failed)
 ******************************************************************************/
BOOL MruLoadList(HWND hwnd, LPCSTR pszIniFilename)
{
    char szMru[_MAX_PATH + 1];
    char sz[sizeof("File65535")];
    int n;
    int i;
    
#ifdef _MT
    InitializeCriticalSection(&csMru);
#endif
    for ( i = 0; i < MAX_MRUFILES; i++ )
        {
        wsprintf(sz, "File%u", i + 1);
        n = GetPrivateProfileString("MRU", sz, "", szMru, sizeof(szMru), pszIniFilename);
        if ( n == 0 )
            break;  /* stop at the first not present - the rest will be null
                        regardless of what's in the ini file */
        strcpy(arszMruFilenames[i], szMru);
        }
    while ( i < MAX_MRUFILES )
        {
        arszMruFilenames[i][0] = '\0';
        i++;
        }
    SetMruMenu(hwnd);
    return ( TRUE );
}



/******************************************************************************
 *  void MruSaveList(LPCSTR pszIniFilename)
 * 
 *  save mru list to ini file
 *
 *  parameters:
 *      pszIniFilename - application ini file path/name
 ******************************************************************************/
void MruSaveList(LPCSTR pszIniFilename)
{
    char sz[sizeof("File65535")];
    int i;
    
    for ( i = 0; i < MAX_MRUFILES; i++ )
        {
        wsprintf(sz, "File%u", i + 1);
        WritePrivateProfileString("MRU", sz, arszMruFilenames[i], pszIniFilename);
        }
#ifdef _MT
    DeleteCriticalSection(&csMru);
#endif
}



/******************************************************************************
 *  void MruGetFilename(LPCSTR pszIniFilename, int nMruNum, LPSTR pszMruFilename, int nSize)
 * 
 *  retrieve n'th mru file name
 *
 *  parameters:
 *      pszIniFilename - application ini file path/name
 *      nMruNum        - number to retrieve (1-based)
 *      pszMruFilename - buffer for retrieved name
 *      nSize          - size of buffer
 *
 * notes:
 *      pszIniFilename is provided in case we want to read it directly from the
 *      ini file.  Currently, the implementation reads them into memory during
 *      initialization and retrieves them from memory when requested
 ******************************************************************************/
void MruGetFilename(LPCSTR pszIniFilename, int nMruNum, LPSTR pszMruFilename, int nSize)
{
    NOREFERENCE(pszIniFilename);
    assert(nMruNum >= 1 && nMruNum <= MAX_MRUFILES);
    if ( nMruNum >= 1 && nMruNum <= MAX_MRUFILES )
        lstrcpyn(pszMruFilename, arszMruFilenames[nMruNum - 1], nSize);
    else
        *pszMruFilename = '\0';        
}



/******************************************************************************
 *  void MruDeleteFilename(HWND hwnd, LPCSTR pszIniFilename, LPCSTR pszFilename)
 * 
 *  delete a file from the mru list
 *
 *  parameters:
 *      hwnd           - window handle of window with mru menu items
 *      pszIniFilename - application ini file
 *      pszFilename    - filename to delete from mru list
 *
 * notes:
 *      pszIniFilename is provided in case we want to deal directly with the
 *      ini file.  Currently, the implementation reads them into memory during
 *      initialization and handles them in memory as needed, flushing them to
 *      the ini file only at application exit.
 ******************************************************************************/
void MruDeleteFilename(HWND hwnd, LPCSTR pszIniFilename, LPCSTR pszFilename)
{
    int i;
    
    NOREFERENCE(pszIniFilename);
#ifdef _MT
    EnterCriticalSection(&csMru);
#endif
    for ( i = 0; i < MAX_MRUFILES; i++ )
        {
        if ( lstrcmpi(pszFilename, arszMruFilenames[i]) == 0 )
            break;
        }    
    if ( i != MAX_MRUFILES )
    {
        while ( ++i < MAX_MRUFILES )
            strcpy(arszMruFilenames[i - 1], arszMruFilenames[i]);
        arszMruFilenames[MAX_MRUFILES - 1][0] = '\0';
        SetMruMenu(hwnd);
    }
#ifdef _MT
    LeaveCriticalSection(&csMru);
#endif
}



/******************************************************************************
 *  void MruSetFirstFilename(HWND hwnd, LPCSTR pszIniFilename, LPCSTR pszFilename)
 * 
 *  set a new first file in mru list, moving the others down, deleting the last
 *
 *  parameters:
 *      hwnd           - window handle of window with mru menu items
 *      pszIniFilename - application ini file
 *      pszFilename    - filename to delete from mru list
 *
 * notes:
 *      pszIniFilename is provided in case we want to deal directly with the
 *      ini file.  Currently, the implementation reads them into memory during
 *      initialization and handles them in memory as needed, flushing them to
 *      the ini file only at application exit.
 ******************************************************************************/
void MruSetFirstFilename(HWND hwnd, LPCSTR pszIniFilename, LPCSTR pszFilename)
{
    int i;

#ifdef _MT
    EnterCriticalSection(&csMru);
#endif
    MruDeleteFilename(hwnd, pszIniFilename, pszFilename);
    for ( i = MAX_MRUFILES - 1; i > 0; i-- )
        strcpy(arszMruFilenames[i], arszMruFilenames[i - 1]);
    lstrcpy(arszMruFilenames[0], pszFilename);
    SetMruMenu(hwnd);
#ifdef _MT
    LeaveCriticalSection(&csMru);
#endif
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void SetMruMenu(HWND hwnd)
 * 
 *  update the mru menu items according to the changed mru list
 *
 *  parameters:
 *      hwnd           - window handle of window with mru menu items
 *
 * notes:
 *      The mru list appears in the File menu for the window's menu bar.
 *      The file menu can be the first (0th) or second (1th) by_position
 *      depending on whether the mdi child is maximized (see mmenu.c MenuStatus()).
 *
 *      Assumes the file menu last two items initially are a separator followed
 *      by a menu item (exit).  Inserts mru items following the separator followed
 *      by a new separator.
 ******************************************************************************/
static void SetMruMenu(HWND hwnd)
{
    char sz[sizeof("&1 ") + _MAX_PATH];
    char *psz;
    HMENU hMenu = GetMenu(hwnd);
    int i;
    int nExit;

    /* find the File menu */
    assert(hMenu != 0);
    if ( hMenu == 0 )
        return;
    GetMenuString(hMenu, 1, sz, sizeof(sz), MF_BYPOSITION);
    if ( stricmp(sz, "&File") == 0 )
        hMenu = GetSubMenu(hMenu, 1);
    else
        hMenu = GetSubMenu(hMenu, 0);        
    assert(hMenu != 0);
    if ( hMenu == 0 )
        return;

    /* delete any mru items presently in the file menu */
    for ( i = 0; i < MAX_MRUFILES; i++ )
        DeleteMenu(hMenu, IDM_FILE_MRUFILE1 + i, MF_BYCOMMAND);

    /* if the 3rd entry from the bottom is a separator, then we assume it's
        the one we inserted for the mru section, and delete it (to be reinserted below) */
    i = GetMenuItemCount(hMenu);
    nExit = GetMenuItemID(hMenu, i - 1);
    if ( GetMenuItemID(hMenu, i - 3) == 0 )
        DeleteMenu(hMenu, i - 3, MF_BYPOSITION);

    if ( arszMruFilenames[0][0] != '\0' )
        {
        /* insert mru file names */
        for ( i = 0; i < MAX_MRUFILES && *arszMruFilenames[i] != '\0'; i++ )
            {
            wsprintf(sz, "&%d ", i + 1);
            psz = sz + strlen(sz);
            MiscShortFilename(
                arszMruFilenames[i],
                psz,
                sizeof("C:\\...\\PATHNAME.EXT\\FILENAME.EXT") - 1);
            InsertMenu(
                hMenu,
                nExit,
                MF_BYCOMMAND | MF_STRING,
                IDM_FILE_MRUFILE1 + i,
                sz
                );
            }

        /* (re)insert separator preceding File/Exit */
        InsertMenu(hMenu, nExit, MF_BYCOMMAND | MF_SEPARATOR, 0, 0);
        }
}
