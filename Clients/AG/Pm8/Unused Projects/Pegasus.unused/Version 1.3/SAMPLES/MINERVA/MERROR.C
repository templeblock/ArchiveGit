/******************************************************************************
 *
 * MERROR.C - Minerva error message display
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
 *     ErrorMessage        - printf style MessageBox function
 *     ErrorPegasusMessage - MessageBox displaying a PIC opcode error message
 *     ErrorFileMessage    - MessageBox displaying a file i/o error message
 *
 * Revision History:
 *   04-24-97  1.00.21  jrb  need to parent the message box to the responsible
 *                           dialog box if applicable instead of always to hwndFrame
 *                           else the dialog isn't disabled
 *   11-11-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "merror.h"



/******************************************************************************
 *  local data
 ******************************************************************************/



/* both of the following are used only while these functions are active,
    none of the functions rely on either of them persisting between function calls
    They're declared in file scope simply to save room on the stack -- there's
    still plenty of room in the default data segment */
static char szMsg[512];
static char szFmt[512];



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  int ErrorMessage(UINT fuStyle, int idsMsg, ...)
 * 
 *  printf style MessageBox function
 *
 *  parameters:
 *      fuStyle - MB_* constants (see MessageBox() doc)
 *      idsMsg  - string resource number of printf format string
 *      ...     - args dependendent on printf format idsMsg
 *
 *  returns:  
 *      control id of button pressed (usually IDOK)
 *
 *  notes:
 *      if an application modal box can't be displayed, it tries to display
 *      a system modal box
 *
 *      merror.h defines STYLE_ERROR (MB_OK | MB_ICONEXCLAMATION) and STYLE_FATAL
 *      (MB_OK | MB_ICONSTOP)
 ******************************************************************************/
int ErrorMessage(UINT fuStyle, int idsMsg, ...)
{
    UINT i;
    int result;
    va_list marker;
    HWND hwnd = hwndFrame;
    
    if ( IsWindow(hwnd) && !IsWindowEnabled(hwnd) )
        hwnd = GetLastActivePopup(hwnd);
    if ( !IsWindow(hwnd) )
        hwnd = hwndFrame;
    
    i = LoadString(hinstThis, idsMsg, szFmt, sizeof(szFmt));
    assert(i != 0 && i < sizeof(szFmt) - 1);

    va_start(marker, idsMsg);
    i = wvsprintf(szMsg, szFmt, marker);
    assert(i != 0 && i < sizeof(szMsg) - 1);
    va_end(marker);

    result = MessageBox(hwnd, szMsg, APPLICATION_NAME, fuStyle);
    if ( result == 0 )
        {   /* not enough memory for the message box */
        fuStyle &= ~( MB_APPLMODAL | MB_TASKMODAL );
        fuStyle |= MB_SYSTEMMODAL;
        fuStyle &= ~( MB_ICONASTERISK | MB_ICONEXCLAMATION | MB_ICONQUESTION | MB_ICONSTOP );
        fuStyle |= MB_ICONHAND;
        result = MessageBox(hwnd, szMsg, APPLICATION_NAME, fuStyle);
        }
    return ( result );
}



/******************************************************************************
 *  ErrorPegasusMessage(long lStatus, int idsMsg, ...)
 * 
 *  MessageBox displaying a PIC opcode error message
 *
 *  parameters:
 *      lStatus - PIC error code
 *      idsMsg  - string resource number of string describing failed activity
 *
 *  notes:
 *      if an application modal box can't be displayed, it tries to display
 *      a system modal box
 *
 *      idsMsg will be something like "The image could not be expanded."
 *      If an explicit string resource for the lStatus error code is not
 *      supplied with a resource id of IDS_PEGASUSERROR + lStatus, then
 *      the IDS_PEGASUSERROR message "A PIC library error %d occurred." is
 *      used where lStatus is supplied as the %d arg.
 *
 *      So far I haven't supplied any of the IDS_PEGASUSERROR + lStatus
 *      messages except for BAD_DATA.  I'm kind of waiting to hear or to determine
 *      what are conceivable errors based on an invalid image or lack of memory
 *      and which are really unlikely or impossible if the minerva code
 *      is correct.
 ******************************************************************************/
void ErrorPegasusMessage(long lStatus, int idsMsg, ...)
{
    UINT i;
    char *psz;
    va_list marker;
    HWND hwnd = hwndFrame;
    
    if ( IsWindow(hwnd) && !IsWindowEnabled(hwnd) )
        hwnd = GetLastActivePopup(hwnd);
    if ( !IsWindow(hwnd) )
        hwnd = hwndFrame;
    
    i = LoadString(hinstThis, idsMsg, szFmt, sizeof(szFmt));
    assert(i != 0 && i < sizeof(szFmt) - 1);
    
    va_start(marker, idsMsg);
    i = wvsprintf(szMsg, szFmt, marker);
    assert(i != 0 && i < sizeof(szMsg) - 1);
    va_end(marker);

    psz = szMsg + strlen(szMsg);
    *psz++ = ' ';   /* add a space between the task message and the pegasus message */

    if ( lStatus < 0 && IDS_PEGASUSERROR - lStatus <= INT_MAX )
        i = LoadString(hinstThis, (int)( IDS_PEGASUSERROR - lStatus ), szFmt, sizeof(szFmt));
    else
        i = 0;  /* no message defined if out of range */

    if ( i != 0 )
        {
        /* some message defined for this status code */
        assert(i < sizeof(szFmt) - 1);
        strcpy(psz, szFmt);
        }
    else
        {
        /* no message defined for this status code */
        i = LoadString(hinstThis, IDS_PEGASUSERROR, szFmt, sizeof(szFmt));
        assert(i != 0 && i < sizeof(szFmt) - 1);
        wsprintf(psz, szFmt, lStatus);
        }
    assert(strlen(szMsg) < sizeof(szMsg));

    if ( MessageBox(hwnd, szMsg, APPLICATION_NAME, STYLE_ERROR) == 0 )
        {   /* not enough memory for the message box */
        MessageBox(hwnd, szMsg, APPLICATION_NAME, MB_OK | MB_SYSTEMMODAL | MB_ICONHAND);
        }
}



/******************************************************************************
 *  ErrorFileMessage(int idsMsg, UINT nErrCode, LPCSTR pszFilename)
 * 
 *  MessageBox displaying a file i/o error message
 *
 *  parameters:
 *      idsMsg      - string resource number of string describing failed activity
 *      nErrCode    - file i/o error code returned from OpenFile
 *      pszFilename - file name accessed
 *
 *  notes:
 *      if an application modal box can't be displayed, it tries to display
 *      a system modal box
 *
 *      idsMsg will be something like "The image could not be expanded."
 *      If an explicit string resource for the nErrCode error code is not
 *      supplied with a resource id of IDS_FILEERROR + lStatus, then
 *      the IDS_FILERROR message "A file error %d occurred." is
 *      used where nErrCode is supplied as the %d arg.
 *
 *      So far I haven't supplied many of the IDS_FILEERROR + nErrCode
 *      messages.  File not found, path not found, too many files open
 *      and sharing errors are the only messages.
 ******************************************************************************/
void ErrorFileMessage(int idsMsg, UINT nErrCode, LPCSTR pszFilename)
{
    UINT i;
    char *psz;
    
    HWND hwnd = hwndFrame;
    
    if ( IsWindow(hwnd) && !IsWindowEnabled(hwnd) )
        hwnd = GetLastActivePopup(hwnd);
    if ( !IsWindow(hwnd) )
        hwnd = hwndFrame;
    
    i = LoadString(hinstThis, idsMsg, szFmt, sizeof(szFmt));
    assert(i != 0 && i < sizeof(szFmt) - 1);
    wsprintf(szMsg, szFmt, pszFilename);
    psz = szMsg + strlen(szMsg);
    *psz++ = ' ';   /* add a space between the task message and the i/o error message */
    
    i = LoadString(hinstThis, IDS_FILEERROR + nErrCode, szFmt, sizeof(szFmt));
    if ( i != 0 )
        {
        assert(i < sizeof(szFmt) - 1);
        strcpy(psz, szFmt);
        }
    else
        {
        i = LoadString(hinstThis, IDS_FILEERROR, szFmt, sizeof(szFmt));
        assert(i != 0 && i < sizeof(szFmt) - 1);
        wsprintf(psz, szFmt, nErrCode);
        }
    assert(strlen(szMsg) < sizeof(szMsg));

    if ( MessageBox(hwnd, szMsg, APPLICATION_NAME, STYLE_ERROR) == 0 )
        {   /* not enough memory for the message box */
        MessageBox(hwnd, szMsg, APPLICATION_NAME, MB_OK | MB_SYSTEMMODAL | MB_ICONHAND);
        }
}
