/*
 * PPCNTLS -- PP CUSTOM CONTROLS
 *
 */

#include <windows.h>
#include <custcntl.h>
#include "types.h"
#include "dlgedit.h"

extern "C" {
/* global static variables */
LOCAL HANDLE		 hLibInstance;

/*
 * LibMain( hInstance, wDataSegment, wHeapSize, lpszCmdLine ) : WORD
 *
 *    hInstance      library instance handle
 *    wDataSegment   library data segment
 *    wHeapSize      default heap size
 *    lpszCmdLine    command line arguments
 *
 * LibMain is called by LibEntry, which is called by Windows when 
 * the DLL is loaded.  The LibEntry routine is provided 
 * in the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The source 
 * LIBENTRY.ASM is also provided.)  
 *
 * LibEntry initializes the DLL's heap, if a HEAPSIZE value is
 * specified in the DLL's DEF file.  Then LibEntry calls
 * LibMain.  The LibMain function below satisfies that call.
 *
 * LibMain performs all the initialization necessary to use the
 * ppcntls user control.  Included in this initialization is the
 * registration of the Spin window class.
 *
*/
 
int FAR PASCAL LibMain(
   HINSTANCE      hInstance,
   WORD        wDataSegment,
   WORD        wHeapSize,
   LPTSTR       lpszCmdLine )
{
/* register ppcntls window if necessary */
if ( hLibInstance == NULL )
	{
	if (Control_RegisterClass(hInstance, 0))
		hLibInstance = hInstance;
	}
/* return result 1 = success; 0 = fail */
return( hLibInstance? 1:0 );
}

/**/


/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded
              (no remaining tasks still have the DLL loaded).  It is
              strongly recommended that a DLL have a WEP() function,
              even if it does nothing but return, as in this example.

*******************************************************************************/
VOID FAR PASCAL WEP (int bSystemExit)
{
    return;
}


/*
 * DlgEdit_Info() : HANDLE
 *
 * This function returns a handle to a global block of memory that
 * contains various information about the kinds of controls the library
 * is capable of supporting.  This data block can, for example, be used
 * by the dialog editor when determining the capabilities of a particular
 * control library.
 *
 * Note that this handle becomes the property of the caller once this
 * function returns.  This implies that the caller must call GlobalFree
 * once it is finished with the data.
 *
 */

HANDLE FAR PASCAL DlgEdit_Info()
{
    HANDLE        hCtlInfo;
    LPCTLINFO    lpCtlInfo;
	 WORD 		wWidth;
	 WORD		wHeight;
	 DWORD		dwStyle;
	 TCHAR		szClassName[MAX_STR_LEN];

    /* allocate space for information structure */
    hCtlInfo = GlobalAlloc( GHND, (DWORD)sizeof(CTLINFO) * 1);
    if ( hCtlInfo ) {

        /* attempt to lock it down */
        lpCtlInfo = (LPCTLINFO)GlobalLock( hCtlInfo );
        if ( lpCtlInfo ) {

            Control_GetClassInfo(szClassName, &wWidth, &wHeight, &dwStyle );
            /* define the fixed portion of the structure */
            lpCtlInfo->wVersion = 100;
            lpCtlInfo->wCtlTypes = 1;
            lstrcpy(lpCtlInfo->szClass, szClassName);
            lstrcpy( lpCtlInfo->szTitle, _T(""));

            /* define the variable portion of the structure */
			 lstrcpy(lpCtlInfo->Type[0].szDescr, szClassName);
            lpCtlInfo->Type[0].wWidth = wWidth;
            lpCtlInfo->Type[0].wHeight = wHeight;
            lpCtlInfo->Type[0].dwStyle = WS_CHILD|dwStyle;

            /* unlock it */
            GlobalUnlock( hCtlInfo );
        
        } else {
            GlobalFree( hCtlInfo );
            hCtlInfo = NULL;
        }
    
    }

    /* return result */
    return( hCtlInfo );

}
 
/**/

/*
 * DlgEdit_Flags( wFlags, lpszString, wMaxString ) : WORD;
 *
 *    wFlags         class style flags
 *    lpszString     class style string
 *    wMaxString     maximum size of class style string
 *
 * This function translates the class style flags provided into a
 * corresponding text string for output to an RC file.  The general
 * windows flags (contained in the low byte) are not interpreted,
 * only those in the high byte.
 *
 * The value returned by this function is the library instance
 * handle when sucessful, and NULL otherwise.
 *
 */

WORD FAR PASCAL DlgEdit_Flags(
    WORD        wFlags,
    LPTSTR       lpszString,
    WORD        wMaxString )
{
    lpszString[0] = NULL;
    return( 0 );
}

/**/


BOOL FAR PASCAL DlgEdit_Style(
    HWND        hWnd,
    HANDLE      hCtlStyle,
    LPFNSTRTOID    lpfnVerifyId,
    LPFNIDTOSTR    lpfnGetIdStr )
{
return(NULL);
}

BOOL FAR PASCAL DlgEdit_DlgFn(
    HWND        hDlg,
    WORD        wMessage,
    WORD        wParam,
    LONG        lParam )
{
return(FALSE);
}

}
