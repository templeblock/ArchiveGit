/******************************************************************************
 *
 * MOBMP.C - Minerva MDI last resourt BMP loader
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * Global data:
 *     OpenDefBMP        - opcode definition data to open BMP if opcode 81 isn't present 
 *
 * Local functions:
 *     OpenBmp           - allocate dib and pic_parm from source bmp and pic_parm
 *     OpenBmpExtractDib - extract full-size, full-detail dib with no interaction
 *     OpenBmpProperties - display bmp properties
 *     OpenBmpCleanup    - free dib and dib pic_parm
 *     PropertiesOnCommand - WM_COMMAND handling for properties dialog
 *
 * Revision History:
 *   04-21-97  1.00.21  jrb  added extract procedure implementation
 *   11-13-96  1,00.00  jrb  created
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
#include <string.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h" 
#include "merror.h"
#include "mmisc.h"
#include "mdlg.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL OpenBmp(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OpenBmpExtractDib(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm);
static void OpenBmpProperties(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static BOOL PropertiesOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static void OpenBmpCleanup(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData);



/******************************************************************************
 *  local data
 ******************************************************************************/



/* define variable control data for Properties dialog */
static CTRLINITDATA InitPropertiesData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */
    { 0, eText, IDC_BMPPROP_FILENAME,        0, _MAX_PATH },
    { 0, eText, IDC_BMPPROP_FILESIZE,        0, sizeof("4,000,000,000") },
    { 0, eText, IDC_BMPPROP_IMAGEWIDTH,      0, sizeof("4,000,000,000") },
    { 0, eText, IDC_BMPPROP_IMAGEHEIGHT,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_BMPPROP_BITSPERPIXEL,    0, 2 },
    { 0, eText, IDC_BMPPROP_IMAGESIZE,       0, sizeof("4,000,000,000") },
    { 0, eText, IDC_BMPPROP_COLORTABLESIZE,  0, sizeof("4,000,000,000") },
    { 0, eText, IDC_BMPPROP_NUMBERIMPORTANT, 0, sizeof("4,000,000,000") },
    { 0, eEnd, -1 }
    };



/* PegasusQuery biCompression types supported by this "opcode" */
static DWORD arTypes[] = { BI_RGB, (DWORD)-1 };



/******************************************************************************
 *  global data
 ******************************************************************************/



extern SAVEOP SaveDefBMP80;

OPENOP OpenDefBMP =
    {
    -1,                                 /* not really an opcode
                                            (but minerva.c asserts if == 0 and since
                                            this is the _only_ instance justifiable
                                            as 0, so setting it to -1 seemed better) */
    0,                                  /* no about box opcode to load or report */
    0,                                  /* no default image options menu item */
    "Windows Bitmap (*.bmp)|*.BMP|",    /* common file open filter */
    arTypes,                            /* array of biCompression types supported */
    0,                                  /* no init proc */
    OpenBmp,                            /* open proc */
    OpenBmpExtractDib,                  /* extract to full-detail dib */
    0,                                  /* no image options proc */
    OpenBmpProperties,                  /* properties proc */
    0,                                  /* no default options proc */
    OpenBmpCleanup,                     /* cleanup proc */
    0,                                  /* no DupPicParm proc */
    &SaveDefBMP80,                      /* File/Save opcode def (if present) */
    CURRENT_PARMVER                     /* mark it available and current version */
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL OpenBmp(
 *      HWND                   hWnd,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      BYTE PICHUGE* PICFAR*  ppDib,
 *      DWORD PICFAR*          pdwDibLen,
 *      PIC_PARM PICFAR*       pDibPicParm,
 *      VOID PICFAR* PICFAR*   ppOpenData)
 * 
 *  open a bmp image
 *
 *  parameters:
 *      hwnd        - HWND for child window to show window
 *      pSrc        - pointer to source BMP
 *      dwSrcLen    - length of source BMP
 *      pSrcPicParm - PicParm from PegasusQuery of source BMP
 *      ppDib       - to receive pointer to DIB
 *      pdwDibLen   - to receive length of DIB
 *      pDibPicParm - to received opened PIC_PARM (same as source for this open)
 *      ppOpenData  - n/a for this open
 *
 *  returns:  
 *      TRUE if it can allocate memory for the DIB, else FALSE
 *
 *  notes:
 *      So that progressive images can be displayed while the open opcode is
 *      expanding, the caller doesn't make assumptions about the image display.
 *      In particular, the caller doesn't cause a display refresh.  So it's here.
 *
 *      It's up to the open opcode to display messages about whatever errors
 *      occur.  This because the opcode knows most about what is expected and
 *      how to describe it.
 ******************************************************************************/
static BOOL OpenBmp(
	 HWND                   hWnd,
	 const BYTE PICHUGE*    pSrc,
	 DWORD                  dwSrcLen,
	 const PIC_PARM PICFAR* pSrcPicParm,
	 BYTE PICHUGE* PICFAR*  ppDib,
	 DWORD PICFAR*          pdwDibLen,
	 PIC_PARM PICFAR*       pDibPicParm,
	 VOID PICFAR* PICFAR*   ppOpenData)
{
	 NOREFERENCE(hWnd);      /* in release mode */
    assert(hWnd == NULL || IsWindow(hWnd));
    assert(ppOpenData != 0);
    *ppOpenData = 0;    /* since there aren't any user options for this function */
    
    return ( OpenBmpExtractDib(pSrc, dwSrcLen, pSrcPicParm, ppDib, pdwDibLen, pDibPicParm) );
}



/******************************************************************************
 *  BOOL OpenBmpExtractDib(
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      BYTE PICHUGE* PICFAR*  ppDib,
 *      DWORD PICFAR*          pdwDibLen,
 *      PIC_PARM PICFAR*       pDibPicParm)
 * 
 *  extract a full-size, full-detail dib from the image
 *
 *  parameters:
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      ppDib       - receives pointer to dib
 *      pdwDibLen   - receives length of dib
 *      pDibPicParm - receives picparm for dib
 *
 *  returns:
 *      FALSE if some error occurs
 ******************************************************************************/
static BOOL OpenBmpExtractDib(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm)
{
    BITMAPFILEHEADER PICHUGE* pbmfi = (BITMAPFILEHEADER PICHUGE*)pSrc;

    NOREFERENCE(dwSrcLen);  /* in release mode */
    
    assert(pSrc != 0);
    assert(dwSrcLen != 0);
    assert(pSrcPicParm != 0);
    assert(ppDib != 0);
    assert(pdwDibLen != 0);
    assert(pDibPicParm != 0);

    hmemcpy(pDibPicParm, pSrcPicParm, sizeof(*pDibPicParm));
    pDibPicParm->Head.biHeight = labs(pSrcPicParm->Head.biHeight);

    /* # bytes per scan line */
    *pdwDibLen = pSrcPicParm->Head.biSizeImage;
    
    *ppDib = (BYTE PICHUGE *)pSrc + pbmfi->bfOffBits;
    return ( TRUE );
}



/******************************************************************************
 *  void OpenBmpProperties(
 *      LPCSTR                 pszFilename,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      const BYTE PICHUGE*    pDib,
 *      DWORD                  dwDibLen,
 *      const PIC_PARM PICFAR* pDibPicParm,
 *      const VOID PICFAR*     pOpenData)
 * 
 *  show a dialog with bmp properties
 *
 *  parameters:
 *      pszFilename - filename containing source BMP
 *      pSrc        - pointer to source BMP
 *      dwSrcLen    - length of source BMP
 *      pSrcPicParm - PicParm from PegasusQuery of source BMP
 *      pDib        - pointer to DIB
 *      dwDibLen    - length of DIB
 *      pDibPicParm - opened PIC_PARM (same as source for this open)
 *      ppOpenData  - n/a for this open
 ******************************************************************************/
static void OpenBmpProperties(
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
    char sz[_MAX_PATH + sizeof(" Properties")];
    int result;
    LPCTRLINITDATA pInitData;
    
    NOREFERENCE(pSrc);
    NOREFERENCE(pSrcPicParm);
    NOREFERENCE(pDib);
    NOREFERENCE(pOpenData);
    
    if ( !CtrlDataDup(InitPropertiesData, &pInitData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_PROPERTIESOUTOFMEMORY);
        /* "There was not enough memory to open the image properties dialog." */
        return;
        }
    MiscShortFilename(pszFilename, sz, sizeof("C:\\...\\FILENAME.EXT") - 1);
    CtrlDataSetText(pInitData, IDC_BMPPROP_FILENAME, sz);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_FILESIZE, dwSrcLen, FALSE);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_IMAGEWIDTH, pDibPicParm->Head.biWidth, FALSE);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_IMAGEHEIGHT, pDibPicParm->Head.biHeight, FALSE);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_BITSPERPIXEL, pDibPicParm->Head.biBitCount, FALSE);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_IMAGESIZE, dwDibLen, FALSE);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_COLORTABLESIZE, pDibPicParm->Head.biClrUsed, FALSE);
    CtrlDataSetLong(pInitData, IDC_BMPPROP_NUMBERIMPORTANT, pDibPicParm->Head.biClrImportant, FALSE);

    GetWindowText(hWnd, sz, sizeof(sz) - ( sizeof(" Properties") - 1 ));
    strcat(sz, " Properties");
    result = DlgDoModal(hWnd, MAKEINTRESOURCE(IDD_BMPPROP), PropertiesOnCommand, pInitData, sz);
    if ( result != IDOK )
        {
        CtrlDataFree(&pInitData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEPROPERTIESDIALOG);
            /* "An unexpected error occurred opening the image properties dialog." */
        return;
        }             
    CtrlDataFree(&pInitData);
}



/******************************************************************************
 *  BOOL PropertiesOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arOpenData,
 *      LPCSTR          pszDlgTemplate,
 *      void PICFAR*    pUserData)
 *
 *  allow properties dialog init after the window is created
 *
 *  parameters:
 *      hwndDlg     - dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arOpenData  - image options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - as defined by OpenSequentialJpeg
 *
 *  returns:
 *      all but IDOK: FALSE if the command isn't handled, else TRUE
 *      IDOK: FALSE if dialog data is invalid (don't end dialog), else TRUE
 ******************************************************************************/
static BOOL PropertiesOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    NOREFERENCE(wNotifyCode);
    NOREFERENCE(arOpenData);
    NOREFERENCE(pszDlgTemplate);
    if ( nCtrlID == -1 )
        SetWindowText(hwndDlg, (LPCSTR)pUserData);
    return ( nCtrlID == IDOK );
}



/******************************************************************************
 *  void OpenBmpCleanup(
 *      const BYTE PICHUGE*    pDib,
 *      const PIC_PARM PICFAR* pDibPicParm,
 *      const VOID PICFAR*     pOpenData)
 * 
 *  whatever deallocation and cleanup needs to be done
 *
 *  parameters:
 *      pDib        - pointer to DIB
 *      pDibPicParm - opened PIC_PARM (same as source for this open)
 *      ppOpenData  - n/a for this open
 ******************************************************************************/
static void OpenBmpCleanup(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData)
{
    NOREFERENCE(pDibPicParm);
    NOREFERENCE(pOpenData);
    NOREFERENCE(pDib);
}
