/******************************************************************************
 *
 * MXP2S.C - Minerva MDI progressive to sequential conversion
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



BOOL TransformP2S(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrc,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm);

TRANSFORMOP TransformDefP2S =
    {
    OP_P2S,
    "Convert Progressive JPEG to Sequential JPEG",
    0,
    0,
    0,
    TransformP2S,
    0,
    0
    };



BOOL TransformP2S(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrc,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm)
{
    NOREFERENCE(hWnd);
    NOREFERENCE(pszFilename);
    NOREFERENCE(ppSrc);
    NOREFERENCE(pdwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(pDib);
    NOREFERENCE(dwDibLen);
    NOREFERENCE(pDibPicParm);

    MessageBox(hwndFrame, "P2S is not implemented.", APPLICATION_NAME, MB_OK);
    return ( FALSE );
}
