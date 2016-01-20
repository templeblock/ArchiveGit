/******************************************************************************
 *
 * MSPRGJPG.C - Minerva MDI progressive JPEG pack module
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
#include <memory.h>



#pragma warning(disable:4001)
#include "pic.h"
#include "errors.h"

#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h" 



void SaveInitProgressiveJpeg(void);
BOOL SaveProgressiveJpeg(
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrcImage,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved);
void DefaultsDialogSaveProgressiveJpeg(void);
BOOL SaveD2JSequentialJpeg(
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrcImage,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved);
void DefaultsDialogSaveD2JSequentialJpeg(void);



extern SAVEOP SaveDefSequentialJpeg;
extern SAVEOP SaveDefEnhancedSequentialJpeg;

SAVEOP SaveDefProgressiveJpeg =
    {
    OP_D2J,
    "Progressive and Sequential JPEG Compress",
    "Progressive JPEG Defaults",
    "Progressive JPEG (*.pic,*.jpg)|*.PIC;*.JPG|",
    SaveInitProgressiveJpeg,
    SaveProgressiveJpeg,
    DefaultsDialogSaveProgressiveJpeg,
    0
    };

SAVEOP SaveDefD2JSequentialJpeg =
    {
    0,
    0,
    "Sequential JPEG Defaults",
    "Sequential JPEG (*.pic,*.jpg)|*.PIC;*.JPG|",
    0,
    SaveD2JSequentialJpeg,
    DefaultsDialogSaveD2JSequentialJpeg,
    0
    };



void SaveInitProgressiveJpeg(void)
{
    if ( SaveDefEnhancedSequentialJpeg.nFoundParmVer == 0 &&
         SaveDefSequentialJpeg.nFoundParmVer == 0 )
        {
        SaveDefD2JSequentialJpeg.lOpcode = SaveDefProgressiveJpeg.lOpcode;
        SaveDefSequentialJpeg.nFoundParmVer = SaveDefProgressiveJpeg.nFoundParmVer;
        }
}



BOOL SaveProgressiveJpeg(
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrcImage,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved)
{
    /*####*/
    NOREFERENCE(hWnd);
    NOREFERENCE(pszFilename);
    NOREFERENCE(pSrcImage);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(pDib);
    NOREFERENCE(dwDibLen);
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(ppSaved);
    NOREFERENCE(pdwLenSaved);

    MessageBox(
        hwndFrame,
        "Save Progressive JPEG is not implemented.",
        APPLICATION_NAME,
        MB_OK);
    return ( FALSE );
}



void DefaultsDialogSaveProgressiveJpeg(void)
{
    /*####*/
    MessageBox(
        hwndFrame,
        "The Save Progressive JPEG Default Options dialog is not implemented.",
        APPLICATION_NAME,
        MB_OK);
}



BOOL SaveD2JSequentialJpeg(
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrcImage,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved)
{
    /*####*/
    NOREFERENCE(hWnd);
    NOREFERENCE(pszFilename);
    NOREFERENCE(pSrcImage);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(pDib);
    NOREFERENCE(dwDibLen);
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(ppSaved);
    NOREFERENCE(pdwLenSaved);

    MessageBox(
        hwndFrame,
        "D2J Save Sequential JPEG is not implemented.",
        APPLICATION_NAME,
        MB_OK);
    return ( FALSE );
}



void DefaultsDialogSaveD2JSequentialJpeg(void)
{
    /*####*/
    MessageBox(
        hwndFrame,
        "The D2J Save Sequential JPEG Default Options dialog is not implemented.",
        APPLICATION_NAME,
        MB_OK);
}
