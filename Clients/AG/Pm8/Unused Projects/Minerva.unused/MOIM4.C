/******************************************************************************
 *
 * MOIM4 - Minerva MDI IM4 expand
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
 *
 * Local functions:
 *
 * Revision History:
 *   11-19-96     jrb    created
 *
 ******************************************************************************/



#define STRICT
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)



#pragma warning(disable:4001)
#include "pic.h"
#include "errors.h"

#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h" 



static BOOL OpenIM4(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL ExtractDibOpenIM4(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm);
static BOOL OptionsDialogOpenIM4(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static void PropertiesDialogOpenIM4(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void DefaultsDialogOpenIM4(void);
static void CleanupOpenIM4(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData);



static DWORD arTypes[] = { BI_PIC4, (DWORD)-1 };

OPENOP OpenDefIM4 =
    {
    OP_EXP4,
    "IM4 Expand",
    "IM4 Defaults",
    "IM4 (*.pic,*.im4)|*.PIC;*.IM4|",
    arTypes,
    0,
    OpenIM4,
    ExtractDibOpenIM4,
    OptionsDialogOpenIM4,
    PropertiesDialogOpenIM4,
    DefaultsDialogOpenIM4,
    CleanupOpenIM4,
    0,
    0,
    0
    };



static BOOL OpenIM4(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    /*####*/
    NOREFERENCE(hWnd);
    NOREFERENCE(pSrc);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(ppDib);
    NOREFERENCE(pdwDibLen);
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(ppOpenData);

    MessageBox(
        hwndFrame,
        "Open IM4 is not implemented.",
        APPLICATION_NAME,
        MB_OK);
    return ( FALSE );
}



static BOOL ExtractDibOpenIM4(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm)
{
    /*####*/
    NOREFERENCE(pSrc);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(ppDib);
    NOREFERENCE(pdwDibLen);
    NOREFERENCE(pDibPicParm);
    return ( FALSE );
}



static BOOL OptionsDialogOpenIM4(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    /*####*/
    NOREFERENCE(hWnd);
    NOREFERENCE(pSrc);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(ppDib);
    NOREFERENCE(pdwDibLen);
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(ppOpenData);

    MessageBox(
        hwndFrame,
        "The Open IM4 Options dialog is not implemented.",
        APPLICATION_NAME,
        MB_OK);
    return ( ERR_NONE );
}



static void PropertiesDialogOpenIM4(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData)
{
    /*####*/
    NOREFERENCE(hWnd);
    NOREFERENCE(pszFilename);
    NOREFERENCE(pSrc);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(pDib);
    NOREFERENCE(dwDibLen);
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(pOpenData);

    MessageBox(
        hwndFrame,
        "The IM4 Properties dialog is not implemented.",
        APPLICATION_NAME,
        MB_OK);
}



static void DefaultsDialogOpenIM4(void)
{
    /*####*/
    MessageBox(
        hwndFrame,
        "The Open IM4 Default Options dialog is not implemented.",
        APPLICATION_NAME,
        MB_OK);
}



static void CleanupOpenIM4(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData)
{
    /*####*/
    NOREFERENCE(pDib);
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(pOpenData);
}
