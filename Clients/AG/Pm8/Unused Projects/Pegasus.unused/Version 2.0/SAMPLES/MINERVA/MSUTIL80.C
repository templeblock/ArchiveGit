/******************************************************************************
 *
 * MSUTIL80.C - Minerva MDI utility DIB to file module
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
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#pragma warning(default:4115 4201 4214)
#include <memory.h>
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
#include "mdlg.h"
#include "minerva.h" 
#include "merror.h"
#include "mmisc.h"
#include "mchild.h"
#include "mopen.h"



extern OPENOP OpenDefTIF81;



/* if the estimated size of the output buffer isn't large enough, this is
    the extra amount reallocated above the current output data length */
#define REALLOCEXTRA    ( 50000L )



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void SaveInitUtility(void);
static void SaveInitUtilityPlus(void);
static BOOL SaveBMP(
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
static BOOL SaveTGA(
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
static BOOL SaveTIF(
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
static BOOL SaveGIF(
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
static BOOL SavePCX(
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
static BOOL SaveUtility(
    DWORD                    dwImageType,
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrcImage,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    D2F_STRUC*               pd2f,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL PegasusPutNeedSpace(PIC_PARM PICFAR* p);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);



static void DefaultsDialogSaveGIF(void);
static BOOL SaveGIFOptionsDialog(
    HWND            hwnd,
    PFNDLGONCOMMAND pfnOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData);
static BOOL GIFImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static BOOL GIFImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static BOOL GIFDefaultsOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static void GIFGrayedCtrls(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    OPTIONSUSERDATA PICFAR* poud);
static BOOL GIFDefaultsOptionsValid(HWND hwndDlg);



static void DefaultsDialogSaveTIF(void);
static BOOL SaveTIFOptionsDialog(
    HWND            hwnd,
    PFNDLGONCOMMAND pfnOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData);
static BOOL TIFImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static BOOL TIFImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static void TIFGrayedCtrls(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    OPTIONSUSERDATA PICFAR* poud);



/******************************************************************************
 *  global data
 ******************************************************************************/



SAVEOP SaveDefBMP80 =
    {
    OP_D2F,
    "Bitmap, Targa, PCX or TIFF Output",
    0,
    "Bitmap (*.bmp)|*.BMP|",
    SaveInitUtility,
    SaveBMP,
    0,
    0
    };

SAVEOP SaveDefPCX80 =
    {
    OP_D2F,
    0,
    0,
    "PCX (*.pcx)|*.PCX|",
    0,
    SavePCX,
    0,
    0
    };

SAVEOP SaveDefTIF80 =
    {
    OP_D2F,
    0,
    "TIFF Defaults",
    "TIFF (*.tif)|*.TIF|",
    0,
    SaveTIF,
    DefaultsDialogSaveTIF,
    0
    };

SAVEOP SaveDefGIF84 =
    {
    OP_D2FPLUS,
    "Bitmap, Targa, PCX, GIF or TIFF Output",
    "GIF Defaults",
    "GIF (*.gif)|*.GIF|",
    SaveInitUtilityPlus,
    SaveGIF,
    DefaultsDialogSaveGIF,
    0
    };

SAVEOP SaveDefTGA80 =
    {
    OP_D2F,
    0,
    0,
    "Targa (*.tga)|*.TGA|",
    0,
    SaveTGA,
    0,
    0
    };



/******************************************************************************
 *  local data
 ******************************************************************************/



#define REQINIT  0
#define REQEXEC  1
#define REQTERM  2
#define ALLOC    3
#define YIELD    4
#define TOTAL    5



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA TIFInitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { 0, eText, IDC_SSEQJPGTIME_TIMERRES, 0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_INIT,     0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_EXEC,     0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_TERM,     0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_ALLOC,    0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_YIELD,    0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_TOTAL,    0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void SaveInitUtility()
 * 
 *  mark PCX, TIF and TGA found or not according to BMP found or not
 *
 ******************************************************************************/
static void SaveInitUtility(void)
{
    SaveDefPCX80.nFoundParmVer = SaveDefBMP80.nFoundParmVer;
    SaveDefTIF80.nFoundParmVer = SaveDefBMP80.nFoundParmVer;
    SaveDefTGA80.nFoundParmVer = SaveDefBMP80.nFoundParmVer;
}

static void SaveInitUtilityPlus(void)
{
    SaveDefBMP80.pszAbout      = 0;

    if ( SaveDefBMP80.nFoundParmVer != 0 )
        PegasusUnload(SaveDefBMP80.lOpcode, CURRENT_PARMVER);
    SaveDefBMP80.lOpcode       = SaveDefGIF84.lOpcode;
    SaveDefBMP80.nFoundParmVer = SaveDefGIF84.nFoundParmVer;
    SaveDefPCX80.lOpcode       = SaveDefGIF84.lOpcode;
    SaveDefPCX80.nFoundParmVer = SaveDefGIF84.nFoundParmVer;
    SaveDefTIF80.lOpcode       = SaveDefGIF84.lOpcode;
    SaveDefTIF80.nFoundParmVer = SaveDefGIF84.nFoundParmVer;
    SaveDefTGA80.lOpcode       = SaveDefGIF84.lOpcode;
    SaveDefTGA80.nFoundParmVer = SaveDefGIF84.nFoundParmVer;
}



/******************************************************************************
 * BOOL SaveUtility(
 *     DWORD                  dwImageType,
 *     const BYTE PICHUGE*    pSrc,
 *     DWORD                  dwSrcLen,
 *     const PIC_PARM PICFAR* pSrcPicParm,
 *     const BYTE PICHUGE*    pDib,
 *     DWORD                  dwDibLen,
 *     const PIC_PARM PICFAR* pDibPicParm,
 *     BYTE PICHUGE* PICFAR*  ppSaved,
 *     DWORD PICFAR*          pdwLenSaved)
 * 
 *  all the save utility intersting stuff
 *
 *  parameters:
 *      dwImageType - biCompression value appropriate to output format type
 *                    BI_RGB, BI_TGA, BI_TIF or BI_PCX
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      pDib        - pointer to dib
 *      dwDibLen    - length of dib
 *      pDibPicParm - picparm for dib
 *      ppSaved     - receive pointer to saved image in memory
 *      pdwLenSaved - receive length of saved image
 *
 *  returns:
 *      FALSE if some error occurs
 ******************************************************************************/
static BOOL SaveBMP(
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
    if ( pDibPicParm->Head.biBitCount == 16 || pDibPicParm->Head.biBitCount == 32 )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVEBMPBITDEPTH);
        return ( FALSE );
        }
    return ( SaveUtility(
        BI_RGB,
        hWnd,
        pszFilename,
        pSrcImage, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        0,
        ppSaved, pdwLenSaved) );
}

static BOOL SaveTGA(
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
    if ( pDibPicParm->Head.biBitCount != 8 && pDibPicParm->Head.biBitCount != 24 )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVETGABITDEPTH);
        return ( FALSE );
        }
    return ( SaveUtility(
        BI_TGA,
        hWnd,
        pszFilename,
        pSrcImage, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        0,
        ppSaved, pdwLenSaved) );
}
    
static BOOL SaveGIF(
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
    LPCTRLINITDATA pSaveData;
    OPTIONSUSERDATA oud;
    D2F_STRUC d2f;

    if ( pDibPicParm->Head.biBitCount != 1 &&
         pDibPicParm->Head.biBitCount != 4 &&
         pDibPicParm->Head.biBitCount != 8 )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVEGIFBITDEPTH);
        return ( FALSE );
        }
    memset(&d2f, 0, sizeof(d2f));
    oud.hWnd        = hWnd;
    oud.pszFilename = pszFilename,
    oud.ppSrc       = (BYTE PICHUGE* PICFAR*)&pSrcImage;
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = (BYTE PICHUGE * PICFAR*)&pDib;    /* cast away constness */
    oud.pdwDibLen   = &dwDibLen;
    oud.pDibPicParm = (PIC_PARM PICFAR*)pDibPicParm;    /* cast away constness */
    if ( !SaveGIFOptionsDialog(hWnd, GIFImageOnCommand, &oud, &pSaveData) )
        return ( FALSE );
    if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTGIF_CHECK_TRANSPARENT) )
        {
        d2f.PicFlags |= PF_ApplyTransparency;
        d2f.TransparentColorIndex = (BYTE)
            CtrlDataGetLong(pSaveData, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX, FALSE);
        if ( d2f.TransparentColorIndex >= pSrcPicParm->Head.biClrUsed )
            d2f.TransparentColorIndex = 0;
        }
    if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTGIF_CHECK_INTERLACED) )
        d2f.ImageType = BI_GIFI;            
    else
        d2f.ImageType = BI_GIFN;
    CtrlDataFree(&pSaveData);
    return ( SaveUtility(
        BI_GIFu,
        hWnd,
        pszFilename,
        pSrcImage, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        &d2f,
        ppSaved, pdwLenSaved) );
}
    
static BOOL SavePCX(
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
    if ( pDibPicParm->Head.biBitCount != 8 && pDibPicParm->Head.biBitCount != 24 )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVEPCXBITDEPTH);
        return ( FALSE );
        }
    return ( SaveUtility(
        BI_PCX,
        hWnd,
        pszFilename,
        pSrcImage, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        0,
        ppSaved, pdwLenSaved) );
}



static BOOL SaveUtility(
    DWORD                    dwImageType,
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrcImage,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    D2F_STRUC*               pd2f,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved)
{
    PIC_PARM pp;
    RESPONSE response;
    APPDATA App;

    NOREFERENCE(hWnd);
    NOREFERENCE(pszFilename);
    NOREFERENCE(pSrcImage);
    NOREFERENCE(dwSrcLen);
    NOREFERENCE(pSrcPicParm);
    
    /**************************************************************************
        initialize PIC_PARM settings
    */


    pp = *pDibPicParm;
    pp.Op = SaveDefBMP80.lOpcode;

    memset(&App, 0, sizeof(App));
    pp.App = (LPARAM)(BYTE PICHUGE*)&App;

    memset(&pp.u, 0, sizeof(pp.u));
    if ( pd2f != 0 )
        pp.u.D2F = *pd2f;
    else
        pp.u.D2F.ImageType = dwImageType;
    pp.u.D2F.OutBpp = pp.Head.biBitCount;
    pp.u.D2F.AllocType = 1;

    /* NOTE:
        Get.End has to be 1 byte larger than the buffer really is.  This is because
        opcode 81 implements very strict queueing and "corrects" the queue as needed.
        In this case, if End isn't one byte bigger, so Rear is set to End, 82 SETS REAR
        TO START (wrapping Rear around).  In strict queueing theory this is OK in theory
        for a synchronous queue, but questionable in practice because the opcode should
        not be changing Rear for the Get queue.  In any case, the result when 82
        corrects Rear is that it thinks the queue is empty and returns RES_GET_NEED_DATA
        -- even though we've set AllocType to 1 and even though we've set Q_EOF.
        
        Setting End 1 byte larger and Rear 1 byte less than End is a silly hack that
        happens to work only because 82 doesn't actually read the extra byte which isn't
        really there */
    pp.Get.Start = (BYTE PICHUGE*)pDib;
        /* cast away constness to eliminate the warning */
    pp.Get.End = pp.Get.Start + dwDibLen + 1;
    pp.Get.Front = pp.Get.Start;
    pp.Get.Rear = pp.Get.End - 1;
    pp.Get.QFlags = Q_EOF;

    /* try to guess the output buffer size,
        dib length + 272*sizeof(RGBQUAD) for color table + 960 for header and any other stuff */
    *pdwLenSaved = dwDibLen + 2048;
    *ppSaved = MiscGlobalAllocPtr(*pdwLenSaved, IDS_SAVEOUTOFMEMORY);
        /* "There was not enough memory to save the image." */
    if ( *ppSaved == 0 )
        return ( FALSE );

    pp.Put.Start = *ppSaved;
    pp.Put.End   = *ppSaved + *pdwLenSaved;
    pp.Put.Front = pp.Put.Start;
    pp.Put.Rear  = pp.Put.Start;
    pp.Put.QFlags = 0;

    /**************************************************************************
        Pegasus()
    */
    response = PegasusReqInit(&pp);
    if ( response == RES_DONE )
        {
        response = PegasusReqExec(&pp);
        if ( response == RES_DONE )
            {
            response = Pegasus(&pp, REQ_TERM);
            assert(response == RES_DONE);
            }
        }
    if ( response != RES_DONE )
        {
        if ( response != RES_ERR )
            Pegasus(&pp, REQ_TERM);
        ErrorPegasusMessage(pp.Status, IDS_SAVEIMAGE);
        /* else an out of memory error was already reported in PegasusPutNeedSpace() */
        if ( App.pGet0 != 0 )
            MiscGlobalFreePtr(&pp.Get.Start);
        MiscGlobalFreePtr(&pp.Put.Start);
        MiscGlobalFreePtr(&App.pPut0);
        *ppSaved = 0;
        *pdwLenSaved = 0;
        return ( FALSE );
        }

    if ( App.pPut0 == 0 )
    {
        *pdwLenSaved = pp.Put.Rear - pp.Put.Front;
        *ppSaved = GlobalReAllocPtr(pp.Put.Start, *pdwLenSaved, GMEM_MOVEABLE);
    }
    else
    {
        *pdwLenSaved = App.pPut - App.pPut0;
        *ppSaved = GlobalReAllocPtr(App.pPut0, *pdwLenSaved, GMEM_MOVEABLE);
    }
    if ( *ppSaved == 0 )
        {
        /* "There was not enough memory to save the image." */
        ErrorMessage(STYLE_ERROR, IDS_SAVEOUTOFMEMORY);
        if ( App.pGet0 != 0 )
            MiscGlobalFreePtr(&pp.Get.Start);
        MiscGlobalFreePtr(&pp.Put.Start);
        MiscGlobalFreePtr(&App.pPut0);
        *pdwLenSaved = 0;
        return ( FALSE );
        }

    if ( App.pGet0 != 0 )
        MiscGlobalFreePtr(&pp.Get.Start);
    if ( App.pPut0 != 0 )
        MiscGlobalFreePtr(&pp.Put.Start);
    return ( TRUE );
}



/******************************************************************************
 * BOOL SaveTIF(
 *     HWND                   hWnd,
 *     LPCSTR                 pszFilename,
 *     DWORD                  dwImageType,
 *     const BYTE PICHUGE*    pSrc,
 *     DWORD                  dwSrcLen,
 *     const PIC_PARM PICFAR* pSrcPicParm,
 *     const BYTE PICHUGE*    pDib,
 *     DWORD                  dwDibLen,
 *     const PIC_PARM PICFAR* pDibPicParm,
 *     BYTE PICHUGE* PICFAR*  ppSaved,
 *     DWORD PICFAR*          pdwLenSaved)
 * 
 *  all the save utility intersting stuff
 *
 *  parameters:
 *      dwImageType - biCompression value appropriate to output format type
 *                    BI_RGB, BI_TGA, BI_TIF or BI_PCX
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      pDib        - pointer to dib
 *      dwDibLen    - length of dib
 *      pDibPicParm - picparm for dib
 *      ppSaved     - receive pointer to saved image in memory
 *      pdwLenSaved - receive length of saved image
 *
 *  returns:
 *      FALSE if some error occurs
 ******************************************************************************/
static BOOL SaveTIF(
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
    PIC_PARM pp;
    RESPONSE response;
    LPCTRLINITDATA pSaveData;
    OPTIONSUSERDATA oud;
    D2F_STRUC d2f;
    BOOL bDisplayTiming = FALSE;
    APPDATA App;
    BYTE PICHUGE* pImage = 0;
    DWORD dwImageLen;
    char szFilename[_MAX_PATH + 1];
    OPENOP *pOpenOp;
    LPCTRLINITDATA pCtrl;
    BOOL bYield;

    if ( pDibPicParm->Head.biBitCount != 1 && pDibPicParm->Head.biBitCount != 8 && pDibPicParm->Head.biBitCount != 24 )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVETIFBITDEPTH);
        return ( FALSE );
        }
    memset(&d2f, 0, sizeof(d2f));
    oud.hWnd        = hWnd;
    oud.pszFilename = pszFilename,
    oud.ppSrc       = (BYTE PICHUGE* PICFAR*)&pSrcImage;
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = (BYTE PICHUGE * PICFAR*)&pDib;    /* cast away constness */
    oud.pdwDibLen   = &dwDibLen;
    oud.pDibPicParm = (PIC_PARM PICFAR*)pDibPicParm;    /* cast away constness */
    if ( !SaveTIFOptionsDialog(hWnd, TIFImageOnCommand, &oud, &pSaveData) )
        return ( FALSE );

    if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_RADIO_MOTOROLA) )
        d2f.Expansion1 |= 0xff000000UL;
    if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_RADIO_MODIFIEDG3) )
        d2f.Compression = 2;
    else if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_RADIO_G4) )
        d2f.Compression = 4;
    else if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_RADIO_G31D) )
    {
        d2f.Compression = 3;
        d2f.Dimension = 0;
    }
    else if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_RADIO_G32D) )
    {
        d2f.Compression = 3;
        d2f.Dimension = 1;
    }

    if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_CHECK_NEGATIVE) )
        d2f.NegateImage = 1;

    if ( CtrlDataIsPushed(pSaveData, IDC_SD2FOPTTIF_CHECK_APPEND) )
    {
        CtrlDataGetText(pSaveData, IDC_SD2FOPTTIF_EDIT_FILENAME, szFilename, sizeof(szFilename));
        if ( !OpenFileQuery(szFilename, &pImage, &dwImageLen, &pOpenOp, &pp) )
            return ( FALSE );
        d2f.PicFlags |= PF_MultiImage;
    }               

    memset(&App, 0, sizeof(App));
    pCtrl = CtrlData(pSaveData, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);
    bYield = 
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);
    App.dwGetSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_INPUTK, FALSE) * 1024;
    App.dwPutSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_OUTPUTK, FALSE) * 1024;

    CtrlDataFree(&pSaveData);

    /**************************************************************************
        initialize PIC_PARM settings
    */

    pp = *pDibPicParm;
    pp.Op = SaveDefBMP80.lOpcode;

    memset(&pp.u, 0, sizeof(pp.u));
    pp.u.D2F = d2f;
    pp.u.D2F.ImageType = BI_TIF;
    pp.u.D2F.OutBpp = pp.Head.biBitCount;
    pp.u.D2F.AllocType = 1;
    if ( bYield )
        pp.u.D2F.PicFlags |= PF_YieldGet | PF_YieldPut;
    pp.App = (LPARAM)(APPDATA PICFAR*)&App;
    App.dwTime[TOTAL] -= MiscTickCount();

    /* NOTE:
        Get.End has to be 1 byte larger than the buffer really is.  This is because
        opcode 81 implements very strict queueing and "corrects" the queue as needed.
        In this case, if End isn't one byte bigger, so Rear is set to End, 82 SETS REAR
        TO START (wrapping Rear around).  In strict queueing theory this is OK in theory
        for a synchronous queue, but questionable in practice because the opcode should
        not be changing Rear for the Get queue.  In any case, the result when 82
        corrects Rear is that it thinks the queue is empty and returns RES_GET_NEED_DATA
        -- even though we've set AllocType to 1 and even though we've set Q_EOF.
        
        Setting End 1 byte larger and Rear 1 byte less than End is a silly hack that
        happens to work only because 82 doesn't actually read the extra byte which isn't
        really there */
    if ( App.dwGetSize == 0 || App.dwGetSize >= dwSrcLen )
        {
        pp.Get.Start  = (BYTE PICHUGE*)pDib;
            /* cast away constness to eliminate the warning --
                note however that this is not safe in general with Pegasus, or as a general
                practice, (although it is currently safe with Sequential JPEG expand). */
        pp.Get.End    = pp.Get.Start + dwDibLen + 1;
        pp.Get.Front  = pp.Get.Start;
        pp.Get.Rear   = pp.Get.End - 1;
        pp.Get.QFlags = Q_EOF;    /* no more to read, the entire image is in the buffer */
        }
    else
        {
        BYTE PICHUGE* pGetQ;

        App.dwTime[ALLOC] -= MiscTickCount();
        pGetQ = MiscGlobalAllocPtr(App.dwGetSize, IDS_IMAGEOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pGetQ == 0 )
            return ( FALSE );
        pp.Get.Start   = pGetQ;            
        pp.Get.End     = pp.Get.Start + App.dwGetSize;
        pp.Get.Front   = pp.Get.Start;
        pp.Get.Rear    = pp.Get.Front;
        pp.Get.QFlags  = 0;
        App.pGet0      = (BYTE PICHUGE*)pDib;
        App.pGet       = App.pGet0;
        App.dwGetSize  = dwDibLen;
        App.dwGetLen   = App.dwGetSize;
        }            

    /* try to guess the output buffer size,
        dib length + 272*sizeof(RGBQUAD) for color table + 960 for header and any other stuff */
    *pdwLenSaved = dwDibLen + 2048;
    if ( ( pp.u.D2F.PicFlags & PF_MultiImage ) != 0 )
        *pdwLenSaved += dwImageLen;
    App.dwTime[ALLOC] -= MiscTickCount();
    *ppSaved = MiscGlobalAllocPtr(*pdwLenSaved, IDS_SAVEOUTOFMEMORY);
    App.dwTime[ALLOC] += MiscTickCount();
        /* "There was not enough memory to save the image." */
    if ( *ppSaved == 0 )
    {
        if ( App.pGet0 != 0 )
            MiscGlobalFreePtr(&pp.Get.Start);
        MiscGlobalFreePtr(&pImage);
        return ( FALSE );
    }

    if ( App.dwPutSize == 0 || App.dwPutSize >= *pdwLenSaved )
    {
        /* initialize put queue buffer pointers */
        pp.Put.Start = *ppSaved;
        pp.Put.End = *ppSaved + *pdwLenSaved;
        pp.Put.Front = pp.Put.Start;
        pp.Put.Rear  = pp.Put.Start;
        pp.Put.QFlags = 0;
        if ( ( pp.u.D2F.PicFlags & PF_MultiImage ) != 0 )
        {
            hmemcpy(pp.Put.Start, pImage, dwImageLen);
            pp.Put.Rear = pp.Put.Start + dwImageLen;
            pp.Put.QFlags |= Q_EOF;
            MiscGlobalFreePtr(&pImage);
        }
    }
    else
        {
        BYTE PICHUGE* pPutQ;

        App.dwTime[ALLOC] -= MiscTickCount();
        pPutQ = MiscGlobalAllocPtr(App.dwPutSize, IDS_IMAGEOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pPutQ == 0 )
            {
            if ( App.pGet0 != 0 )
                MiscGlobalFreePtr(&pp.Get.Start);
            MiscGlobalFreePtr(&pImage);
            return ( FALSE );
            }
        pp.Put.Start  = pPutQ;
        pp.Put.End    = pPutQ + App.dwPutSize;
        App.dwPutSize = *pdwLenSaved;
        App.dwPutLen  = *pdwLenSaved;
        App.pPut      = (BYTE PICHUGE *)*ppSaved;
        App.pPut0     = App.pPut;
        pp.Put.Front  = pp.Put.Start;
        pp.Put.Rear   = pp.Put.Start;
        pp.Put.QFlags = 0;
        if ( ( pp.u.D2F.PicFlags & PF_MultiImage ) != 0 )
        {
            hmemcpy(App.pPut0, pImage, dwImageLen);
            App.dwPutLen = dwImageLen;
            MiscGlobalFreePtr(&pImage);
        }
    }

    /**************************************************************************
        Pegasus()
    */
    response = PegasusReqInit(&pp);
    if ( response == RES_DONE )
        {
        response = PegasusReqExec(&pp);
        if ( response == RES_DONE )
            {
            App.dwTime[REQTERM] -= MiscTickCount();
            response = Pegasus(&pp, REQ_TERM);
            App.dwTime[REQTERM] += MiscTickCount();
            assert(response == RES_DONE);
            }
        }
    if ( response != RES_DONE )
        {
        if ( response != RES_ERR )
            Pegasus(&pp, REQ_TERM);
        ErrorPegasusMessage(pp.Status, IDS_SAVEIMAGE);
        /* else an out of memory error was already reported in PegasusPutNeedSpace() */
        if ( App.pGet0 != 0 )
            MiscGlobalFreePtr(&pp.Get.Start);
        MiscGlobalFreePtr(&pp.Put.Start);
        MiscGlobalFreePtr(&App.pPut0);
        *ppSaved = 0;
        *pdwLenSaved = 0;
        return ( FALSE );
        }

    if ( App.pPut0 == 0 )
    {
        *pdwLenSaved = pp.Put.Rear - pp.Put.Front;
        App.dwTime[ALLOC] -= MiscTickCount();
        *ppSaved = GlobalReAllocPtr(pp.Put.Start, *pdwLenSaved, GMEM_MOVEABLE);
        App.dwTime[ALLOC] += MiscTickCount();
    }
    else
    {
        *pdwLenSaved = App.pPut - App.pPut0;
        App.dwTime[ALLOC] -= MiscTickCount();
        *ppSaved = GlobalReAllocPtr(App.pPut0, *pdwLenSaved, GMEM_MOVEABLE);
        App.dwTime[ALLOC] += MiscTickCount();
    }
    if ( *ppSaved == 0 )
        {
        /* "There was not enough memory to save the image." */
        ErrorMessage(STYLE_ERROR, IDS_SAVEOUTOFMEMORY);
        if ( App.pGet0 != 0 )
            MiscGlobalFreePtr(&pp.Get.Start);
        MiscGlobalFreePtr(&pp.Put.Start);
        MiscGlobalFreePtr(&App.pPut0);
        *pdwLenSaved = 0;
        return ( FALSE );
        }
 
    App.dwTime[ALLOC] -= MiscTickCount();
    if ( App.pGet0 != 0 )
        MiscGlobalFreePtr(&pp.Get.Start);
    if ( App.pPut0 != 0 )
        MiscGlobalFreePtr(&pp.Put.Start);
    App.dwTime[ALLOC] += MiscTickCount();

    App.dwTime[TOTAL] += MiscTickCount();

    if ( bDisplayTiming )
        MiscDisplayTimingDialog(
            hwndFrame,
            MAKEINTRESOURCE(IDD_SD2FTIME),
            App.dwTime,
            TIFInitDisplayTiming);

    return ( TRUE );
}
    



/******************************************************************************
 *  RESPONSE PegasusReqInit(PIC_PARM PICFAR *p)
 * 
 *  Pegasus(p, REQ_INIT) - handle all the REQ_INIT events
 *
 *  parameters:
 *      p  - pointer to initialized PIC_PARM
 *
 *  returns:
 *      RES_ERR or RES_DONE only
 ******************************************************************************/
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p)
{
    RESPONSE response;
    APPDATA PICFAR* pApp = (APPDATA PICFAR*)p->App;

    pApp->dwTime[REQINIT] -= MiscTickCount();
    response = Pegasus(p, REQ_INIT);
    pApp->dwTime[REQINIT] += MiscTickCount();
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                return ( response );

            case RES_SEEK:
                assert(( p->SeekInfo & SEEK_DIRECTION ) == 0 );
                if ( ( p->SeekInfo & SEEK_FILE ) == 0 )
                    p->Get.Front = p->Get.Start + ( p->SeekInfo & SEEK_OFFSET );
                else
                    p->Put.Front = p->Put.Rear = p->Put.Start + ( p->SeekInfo & SEEK_OFFSET );
                break;
                
            case RES_PUT_NEED_SPACE:
                if ( !PegasusPutNeedSpace(p) )
                    return ( RES_ERR );
                break;
                
            default:
                assert(FALSE);
                break;
            }
        pApp->dwTime[REQINIT] -= MiscTickCount();
        response = Pegasus(p, REQ_CONT);
        pApp->dwTime[REQINIT] += MiscTickCount();
        }
    return ( response );        
}



/******************************************************************************
 *  BOOL PegasusPutNeedSpace(PIC_PARM PICFAR *p)
 * 
 *  handle a response == RES_PUT_NEED_SPACE
 *
 *  parameters:
 *      p  - pointer to initialized PIC_PARM
 *
 *  returns:
 *      FALSE - memory allocation failure
 ******************************************************************************/
static BOOL PegasusPutNeedSpace(PIC_PARM PICFAR* p)
{
    APPDATA PICFAR* pApp = (APPDATA PICFAR*)p->App;
    DWORD dwLen;
    BYTE PICHUGE* pRealloc;
    DWORD dw;

    dwLen = MiscQLen(&p->Put);
    if ( pApp->pPut0 == 0 )
        {
        pApp->dwTime[ALLOC] -= MiscTickCount();
        pApp->dwPutSize = p->Put.End - p->Put.Start + REALLOCEXTRA; 
        pApp->pPut0 = MiscGlobalAllocPtr(pApp->dwPutSize, IDS_SAVEOUTOFMEMORY);
        pApp->dwTime[ALLOC] += MiscTickCount();
        if ( pApp->pPut0 == 0 )
            {
            Pegasus(p, REQ_TERM);
            p->Status = ERR_NONE;
            return ( FALSE );
            }
        pApp->pPut = pApp->pPut0;
        pApp->dwPutLen = 0;
        }
    else
        {
        if ( dwLen > pApp->dwPutSize - pApp->dwPutLen )
            {
            /* unexpectedly have to reallocate the output buffer */
            pApp->dwTime[ALLOC] -= MiscTickCount();
            pApp->dwPutSize = pApp->pPut + dwLen + REALLOCEXTRA - pApp->pPut0;
            pRealloc = GlobalReAllocPtr(pApp->pPut0, pApp->dwPutSize, GMEM_MOVEABLE);
            pApp->dwTime[ALLOC] += MiscTickCount();
            if ( pRealloc == 0 )
                {
                Pegasus(p, REQ_TERM);
                ErrorMessage(STYLE_ERROR, IDS_SAVEOUTOFMEMORY);
                p->Status = ERR_NONE;
                return ( FALSE );
                }
            pApp->pPut = ( pApp->pPut - pApp->pPut0 ) + pRealloc;
            pApp->pPut0 = pRealloc;
            }
        }
    dw = pApp->dwPutSize - pApp->dwPutLen;
    MiscCopyFromQueue(&pApp->pPut, &dw, &p->Put, 0);
    pApp->dwPutLen = pApp->dwPutSize - dw;
    return ( TRUE );
}

    
    
/******************************************************************************
 *  RESPONSE PegasusReqExec(PIC_PARM PICFAR *p)
 * 
 *  Pegasus(p, REQ_EXEC) - handle all the REQ_EXEC events
 *
 *  parameters:
 *      p  - pointer to initialized PIC_PARM
 *
 *  returns:
 *      RES_ERR or RES_DONE only
 ******************************************************************************/
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p)
{    
    APPDATA PICFAR* pApp = (APPDATA PICFAR*)p->App;
    RESPONSE response;
    DWORD dw;

    pApp->dwTime[REQEXEC] -= MiscTickCount();
    response = Pegasus(p, REQ_EXEC);
    pApp->dwTime[REQEXEC] += MiscTickCount();
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                return ( response );

            case RES_SEEK:
                assert(( p->SeekInfo & SEEK_DIRECTION ) == 0 );
                if ( ( p->SeekInfo & SEEK_FILE ) == 0 )
                {
                    assert(p->Get.Front == p->Get.Rear);
                    if ( pApp->pGet0 == 0 )
                        p->Get.Front = p->Get.Start + ( p->SeekInfo & SEEK_OFFSET );
                    else
                    {
                        p->Get.Front = p->Get.Rear = p->Get.Start;
                        pApp->pGet = pApp->pGet0 + ( p->SeekInfo & SEEK_OFFSET );
                        assert(pApp->pGet >= pApp->pGet0 && (DWORD)( pApp->pGet - pApp->pGet0 ) <= pApp->dwGetSize);
                        pApp->dwGetLen = pApp->pGet0 + pApp->dwGetSize - pApp->pGet;
                        if ( (int)pApp->dwGetLen < 0 )
                            p->Get.QFlags |= Q_IO_ERR;
                        else if ( pApp->dwGetLen == 0 )
                            p->Get.QFlags |= Q_EOF;
                        else
                            p->Get.QFlags &= ~Q_EOF;
                    }
                }
                else
                {
                    assert(p->Put.Front == p->Put.Rear);
                    if ( pApp->pPut0 == 0 )
                        p->Put.Front = p->Put.Rear = p->Put.Start + ( p->SeekInfo & SEEK_OFFSET );
                    else
                    {
                        p->Put.Front = p->Put.Rear = p->Put.Start;
                        pApp->pPut = pApp->pPut0 + ( p->SeekInfo & SEEK_OFFSET );
                        assert(pApp->pPut >= pApp->pPut0 && (DWORD)( pApp->pPut - pApp->pPut0 ) <= pApp->dwPutSize);
                        if ( pApp->dwPutLen < ( p->SeekInfo & SEEK_OFFSET ) )
                            p->Put.QFlags |= Q_IO_ERR;
                        else if ( pApp->dwPutLen == ( p->SeekInfo & SEEK_OFFSET ) )
                            p->Put.QFlags |= Q_EOF;
                        else
                            p->Put.QFlags &= ~Q_EOF;
                    }
                }
                break;
                
            case RES_PUTQ_GET_NEED_DATA:
                if ( ( p->Put.QFlags & Q_EOF ) != 0 )
                    p->Put.QFlags |= Q_IO_ERR;
                else
                {                    
                    if ( pApp->pPut0 == 0 )
                        p->Put.QFlags |= Q_EOF;
                    else
                    {
                        pApp->pPut = pApp->pPut0 + ( p->SeekInfo & SEEK_OFFSET );
                        dw = pApp->dwPutLen - ( p->SeekInfo & SEEK_OFFSET );
                        MiscCopyToQueue(&pApp->pPut, &dw, &p->Put, 0);
                        if ( p->Put.Front == p->Put.Start && p->Put.Rear == p->Put.End )
                            p->Put.Rear--;
                        pApp->pPut = pApp->pPut0 + ( p->SeekInfo & SEEK_OFFSET );
                    }
                }                            
                break;

            case RES_GET_NEED_DATA:
                if ( ( p->Get.QFlags & Q_EOF ) != 0 )
                    p->Get.QFlags |= Q_IO_ERR;
                else
                {                    
                    if ( pApp->pGet0 == 0 )
                        p->Get.QFlags |= Q_EOF;
                    else
                    {
                        MiscCopyToQueue(&pApp->pGet, &pApp->dwGetLen, &p->Get, 0);
                        if ( p->Get.Front == p->Get.Start && p->Get.Rear == p->Get.End )
                        {
                            pApp->dwGetLen++;
                            pApp->pGet--;
                            p->Get.Rear--;
                        }
                    }
                }                            
                break;

            case RES_PUT_NEED_SPACE:
                if ( ( p->Put.QFlags & Q_READING ) != 0 )
                {
                    p->Put.Front = p->Put.Rear = p->Put.Start;
                }
                else 
                {
                    if ( !PegasusPutNeedSpace(p) )
                        return ( RES_ERR );
                }
                break;
                
            case RES_PUT_DATA_YIELD:
            case RES_GET_DATA_YIELD:
                dw = MiscTickCount();
                pApp->dwTime[YIELD] -= dw;
                if ( MiscYield() )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                dw = MiscTickCount();
                pApp->dwTime[YIELD] += dw;
                break;

            default:
                assert(FALSE);
                break;
            }
        pApp->dwTime[REQEXEC] -= MiscTickCount();
        response = Pegasus(p, REQ_CONT);
        pApp->dwTime[REQEXEC] += MiscTickCount();
        }
    if ( response == RES_DONE && pApp->pPut0 != 0 && p->Put.Front != p->Put.Rear )
        {
        if ( !PegasusPutNeedSpace(p) )
            return ( RES_ERR );
        }
    return ( response );        
}



/******************************************************************************
 *  GIF Options dialog
 ******************************************************************************/

 
 
/******************************************************************************
 *  local data for GIF
 ******************************************************************************/



/* ini section for save options */
static char szGIFOptionsSection[] = "GIF Save Options";

/* specifying save options controls' parameters and default values */
static CTRLINITDATA GIFInitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Interlaced",  eCheckBox, IDC_SD2FOPTGIF_CHECK_INTERLACED,  TRUE  },
    { "Transparent", eCheckBox, IDC_SD2FOPTGIF_CHECK_TRANSPARENT, FALSE },

    { "Transparent Color Index", eEdit, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX, FALSE, 3, "0" },

    { 0, eCheckBox, IDC_SD2FOPTGIF_CHECK_SETDEFAULT },

    { 0, eEnd, -1 }
    };
 


 /******************************************************************************
 *  void DefaultsDialogSaveGIF(void)
 * 
 *  default save options dialog
 * 
 *  notes:
 *      The processing specific to the Save GIF options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static void DefaultsDialogSaveGIF(void)
{
    LPCTRLINITDATA pSaveData;
    
    if ( SaveGIFOptionsDialog(hwndFrame, GIFDefaultsOnCommand, NULL, &pSaveData) )
        CtrlDataFree(&pSaveData);
}



/******************************************************************************
 *  BOOL SaveGIFOptionsDialog(
 *      PFNDLGONCOMMAND *pfnOnCommand,
 *      OPTIONSUSERDATA PICFAR* poud,
 *      LPCTRLINITDATA PICFAR* ppSaveData)
 * 
 *  save options dialog default options and for a specific image
 *
 *  parameters:
 *      pfnOnCommand - WM_COMMAND handler for dialog
 *      poud         - pointers to source and dib image data (see mopcodes.h)
 *      ppSaveData   - receives pointer to save options settings
 * 
 *  notes:
 *      The processing specific to the Save Sequential GIF save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static BOOL SaveGIFOptionsDialog(
    HWND hWnd,
    PFNDLGONCOMMAND pfnOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData)
{
    int result;

    /* read default settings from the ini file -- apply InitOptions settings
        where ini settings aren't present */
    if ( !CtrlDataDup(GIFInitOptionsData, ppSaveData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVEOPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the save options dialog." */
        return ( FALSE );
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szGIFOptionsSection, *ppSaveData);
    if ( poud == 0 )
        {
        CtrlDataSetPushed(*ppSaveData, IDC_SD2FOPTGIF_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTGIF_CHECK_SETDEFAULT);
        }

    result = DlgDoModal(hWnd, MAKEINTRESOURCE(IDD_SD2FOPTGIF), pfnOnCommand, *ppSaveData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppSaveData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATESAVEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the save options dialog." */
        return ( FALSE );
        }             

    if ( CtrlDataIsPushed(*ppSaveData, IDC_SD2FOPTGIF_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szGIFOptionsSection, *ppSaveData);

    return ( TRUE );
}



/******************************************************************************
 *  BOOL GIFImageOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arSaveData,
 *      LPCSTR          pszDlgTemplate,
 *      void PICFAR*    pUserData)
 *
 *  save options WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arSaveData  - save options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - as defined by SaveSequentialJpeg
 *
 *  returns:
 *      all but IDOK: FALSE if the command isn't handled, else TRUE
 *      IDOK: FALSE if dialog data is invalid (don't end dialog), else TRUE
 *
 *  notes:
 *      as the oncommand handler is called for the dialog and its
 *      children, pszDlgTemplate is used to tell which dialog is active
 ******************************************************************************/
static BOOL GIFImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    char sz[_MAX_PATH + sizeof(" Save GIF Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */

    NOREFERENCE(hwndDlg);
    NOREFERENCE(wNotifyCode);
    NOREFERENCE(arSaveData);
    NOREFERENCE(pszDlgTemplate);
    
    switch ( nCtrlID )
        {
        case -1:
            lstrcpy(sz, poud->pszFilename);
            lstrcat(sz, " Save GIF Options");
            SetWindowText(hwndDlg, sz);
            GIFGrayedCtrls(hwndDlg, arSaveData, poud);
            break;

        case IDC_SD2FOPTGIF_CHECK_TRANSPARENT:
            if ( wNotifyCode == BN_CLICKED )
                {
                GIFGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SD2FOPTGIF);
            break;

        case IDOK:
            return ( GIFImageOptionsValid(hwndDlg, poud) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL GIFImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
 *
 *  validate all the dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      poud    - pointer to src/dib image data structure
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL GIFImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
{
    DWORD nIndex;
    BOOL fValid;

    if ( IsDlgButtonChecked(hwndDlg, IDC_SD2FOPTGIF_CHECK_TRANSPARENT) )
        {
        nIndex = GetDlgItemInt(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX, &fValid, FALSE);
        if ( !fValid || nIndex >= poud->pSrcPicParm->Head.biClrUsed )
            {
            ErrorMessage(
                STYLE_ERROR,
                IDS_GIFTRANSPARENTINDEX,
                poud->pSrcPicParm->Head.biClrUsed - 1);
            SetFocus(GetDlgItem(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX), 0, -1);
            return ( FALSE );
            }
        }
    return ( TRUE );
}



/******************************************************************************
 *  void GIFGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA *poud)
 *
 *  enable/gray dialog controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 *      poud    - pointer to src/dib image data structure
 ******************************************************************************/
static void GIFGrayedCtrls(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    OPTIONSUSERDATA PICFAR* poud)
{
    BOOL fTransparent = IsDlgButtonChecked(hwndDlg, IDC_SD2FOPTGIF_CHECK_TRANSPARENT);
    
    NOREFERENCE(ar);
    NOREFERENCE(poud);
    
    EnableWindow(GetDlgItem(hwndDlg, IDC_SD2FOPTGIF_STATIC_TRANSPARENTINDEX), fTransparent);
    EnableWindow(GetDlgItem(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX), fTransparent);
}



/******************************************************************************
 *  BOOL GIFDefaultsOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arSaveData,
 *      LPCSTR          pszDlgTemplate,
 *      void PICFAR*    pUserData)
 *
 *  save options WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arSaveData  - save options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - as defined by SaveSequentialJpeg
 *
 *  returns:
 *      all but IDOK: FALSE if the command isn't handled, else TRUE
 *      IDOK: FALSE if dialog data is invalid (don't end dialog), else TRUE
 *
 *  notes:
 *      as the oncommand handler is called for the dialog and its
 *      children, pszDlgTemplate is used to tell which dialog is active
 ******************************************************************************/
static BOOL GIFDefaultsOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    NOREFERENCE(hwndDlg);
    NOREFERENCE(wNotifyCode);
    NOREFERENCE(arSaveData);
    NOREFERENCE(pszDlgTemplate);
    NOREFERENCE(pUserData);
    
    switch ( nCtrlID )
        {
        case -1:
            GIFGrayedCtrls(hwndDlg, arSaveData, 0);
            break;

        case IDC_SD2FOPTGIF_CHECK_TRANSPARENT:
            if ( wNotifyCode == BN_CLICKED )
                {
                GIFGrayedCtrls(hwndDlg, arSaveData, 0);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SD2FOPTGIF);
            break;

        case IDOK:
            return ( GIFDefaultsOptionsValid(hwndDlg) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL GIFDefaultsOptionsValid(HWND hwndDlg)
 *
 *  validate all the dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL GIFDefaultsOptionsValid(HWND hwndDlg)
{
    DWORD nIndex;
    BOOL fValid;

    if ( IsDlgButtonChecked(hwndDlg, IDC_SD2FOPTGIF_CHECK_TRANSPARENT) )
        {
        nIndex = GetDlgItemInt(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX, &fValid, FALSE);
        if ( !fValid || nIndex >= 256 )
            {
            ErrorMessage(STYLE_ERROR, IDS_GIFTRANSPARENTINDEX, 256);
            SetFocus(GetDlgItem(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SD2FOPTGIF_EDIT_TRANSPARENTINDEX), 0, -1);
            return ( FALSE );
            }
        }
    return ( TRUE );
}



/******************************************************************************
 *  TIFF Options dialog
 ******************************************************************************/

 
 
/******************************************************************************
 *  local data for TIF
 ******************************************************************************/



/* ini section for save options */
static char szTIFOptionsSection[] = "TIF Save Options";

static char szCompression[] = "Compression";
static char szByteOrder[] = "ByteOrder";

/* specifying save options controls' parameters and default values */
static CTRLINITDATA TIFInitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */
    { szCompression,   eRadioButton, IDC_SD2FOPTTIF_RADIO_UNCOMPRESSED,  TRUE,  0, "None" },
    { szCompression,   eRadioButton, IDC_SD2FOPTTIF_RADIO_MODIFIEDG3,    FALSE, 0, "Modified G3"  },
    { szCompression,   eRadioButton, IDC_SD2FOPTTIF_RADIO_G31D,          FALSE, 0, "G31D" },
    { szCompression,   eRadioButton, IDC_SD2FOPTTIF_RADIO_G32D,          FALSE, 0, "G32D" },
    { szCompression,   eRadioButton, IDC_SD2FOPTTIF_RADIO_G4,            FALSE, 0, "G4" },
    { szByteOrder,     eRadioButton, IDC_SD2FOPTTIF_RADIO_INTEL,         TRUE,  0, "Intel" },
    { szByteOrder,     eRadioButton, IDC_SD2FOPTTIF_RADIO_MOTOROLA,      FALSE, 0, "Motorola"  },

    { "NegativeImage", eCheckBox,    IDC_SD2FOPTTIF_CHECK_NEGATIVE,      FALSE },
    { "AppendToFile",  eCheckBox,    IDC_SD2FOPTTIF_CHECK_APPEND,        FALSE },

    { "AppendFilename", eEdit, IDC_SD2FOPTTIF_EDIT_FILENAME, FALSE, _MAX_PATH },

    { 0, eCheckBox, IDC_SD2FOPTTIF_CHECK_SETDEFAULT },

    { 0, eGroupBox, IDC_SD2FOPTTIF_STATIC_COMPRESSION },

    /*
     n/a  nested  dialog id    n/a   dialog button      dialog init data
    */
    { 0, ePushButton, IDC_BUTTON_TIMING, FALSE, IDD_TIMING  },
    { szTIFOptionsSection, eDialog, IDD_TIMING, FALSE, IDC_BUTTON_TIMING, (LPSTR)InitTimingData },

    { 0, eEnd, -1 }
    };
 


 /******************************************************************************
 *  void DefaultsDialogSaveTIF(void)
 * 
 *  default save options dialog
 * 
 *  notes:
 *      The processing specific to the Save TIFF options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static void DefaultsDialogSaveTIF(void)
{
    LPCTRLINITDATA pSaveData;
    
    if ( SaveTIFOptionsDialog(hwndFrame, TIFImageOnCommand, NULL, &pSaveData) )
        CtrlDataFree(&pSaveData);
}



/******************************************************************************
 *  BOOL SaveTIFOptionsDialog(
 *      PFNDLGONCOMMAND *pfnOnCommand,
 *      OPTIONSUSERDATA PICFAR* poud,
 *      LPCTRLINITDATA PICFAR* ppSaveData)
 * 
 *  save options dialog default options and for a specific image
 *
 *  parameters:
 *      pfnOnCommand - WM_COMMAND handler for dialog
 *      poud         - pointers to source and dib image data (see mopcodes.h)
 *      ppSaveData   - receives pointer to save options settings
 * 
 *  notes:
 *      The processing specific to the Save Sequential GIF save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static BOOL SaveTIFOptionsDialog(
    HWND hWnd,
    PFNDLGONCOMMAND pfnOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData)
{
    int result;

    /* read default settings from the ini file -- apply InitOptions settings
        where ini settings aren't present */
    if ( !CtrlDataDup(TIFInitOptionsData, ppSaveData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVEOPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the save options dialog." */
        return ( FALSE );
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szTIFOptionsSection, *ppSaveData);
    if ( poud == 0 )
        {
        CtrlDataSetPushed(*ppSaveData, IDC_SD2FOPTTIF_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_CHECK_SETDEFAULT);
        }
    else if ( poud->pDibPicParm->Head.biBitCount != 1 )
    {
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_STATIC_COMPRESSION);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_RADIO_UNCOMPRESSED);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_RADIO_MODIFIEDG3);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_RADIO_G31D);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_RADIO_G32D);
        CtrlDataSetInactive(*ppSaveData, IDC_SD2FOPTTIF_RADIO_G4);
        CtrlDataSetPushed(*ppSaveData, IDC_SD2FOPTTIF_RADIO_UNCOMPRESSED, TRUE);
    }

    result = DlgDoModal(hWnd, MAKEINTRESOURCE(IDD_SD2FOPTTIF), pfnOnCommand, *ppSaveData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppSaveData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATESAVEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the save options dialog." */
        return ( FALSE );
        }             

    if ( CtrlDataIsPushed(*ppSaveData, IDC_SD2FOPTTIF_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szTIFOptionsSection, *ppSaveData);

    return ( TRUE );
}



/******************************************************************************
 *  BOOL TIFImageOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arSaveData,
 *      LPCSTR          pszDlgTemplate,
 *      void PICFAR*    pUserData)
 *
 *  save options WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arSaveData  - save options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - as defined by SaveSequentialJpeg
 *
 *  returns:
 *      all but IDOK: FALSE if the command isn't handled, else TRUE
 *      IDOK: FALSE if dialog data is invalid (don't end dialog), else TRUE
 *
 *  notes:
 *      as the oncommand handler is called for the dialog and its
 *      children, pszDlgTemplate is used to tell which dialog is active
 ******************************************************************************/
static BOOL TIFImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    char sz[_MAX_PATH + sizeof(" Save TIF Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */

    NOREFERENCE(hwndDlg);
    NOREFERENCE(wNotifyCode);
    NOREFERENCE(arSaveData);
    NOREFERENCE(pszDlgTemplate);
    
    switch ( nCtrlID )
        {
        case -1:
            if ( poud != 0 )
            {
                lstrcpy(sz, poud->pszFilename);
                lstrcat(sz, " Save TIF Options");
                SetWindowText(hwndDlg, sz);
            }
            TIFGrayedCtrls(hwndDlg, arSaveData, poud);
            break;

        case IDC_SD2FOPTTIF_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
            {
                OPENFILENAME ofn;
                static char szFilename[_MAX_PATH + 1];

                memset(&ofn, 0, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner   = hwndDlg;
                ofn.lpstrFilter = "TIF files (*.TIF)\0*.tif\0All files (*.*)\0*.*\0";
                ofn.lpstrFile   = szFilename;
                ofn.nMaxFile    = sizeof(szFilename);
                szFilename[0] = '\0';
                ofn.Flags       = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                if ( GetOpenFileName(&ofn) )
                    SetDlgItemText(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME, szFilename);
            }
            break;

        case IDC_SD2FOPTTIF_CHECK_APPEND:
            if ( wNotifyCode == BN_CLICKED )
                {
                TIFGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SD2FOPTGIF);
            break;

        case IDOK:
            return ( TIFImageOptionsValid(hwndDlg, poud) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL TIFImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
 *
 *  validate all the dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      poud    - pointer to src/dib image data structure
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL TIFImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
{
    static char szFilename[_MAX_PATH + 1];
    OPENOP *pOpenOp;
    static PIC_PARM pp;
    BYTE PICHUGE* pImage = 0;
    DWORD dwImageLen;

    NOREFERENCE(poud);

    if ( IsDlgButtonChecked(hwndDlg, IDC_SD2FOPTTIF_CHECK_APPEND) )
    {
        GetDlgItemText(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME, szFilename, sizeof(szFilename));
        if ( !OpenFileQuery(szFilename, &pImage, &dwImageLen, &pOpenOp, &pp) )
        {
            MiscGlobalFreePtr(&pImage);
            SetFocus(GetDlgItem(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME), 0, -1);
            return ( FALSE );
        }
        MiscGlobalFreePtr(&pImage);
        if ( pOpenOp != &OpenDefTIF81 )
        {
            ErrorMessage(STYLE_ERROR, IDS_TIFAPPENDFILENAME, szFilename);
            SetFocus(GetDlgItem(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME), 0, -1);
            return ( FALSE );
        }
    }
    return ( TRUE );
}



/******************************************************************************
 *  void TIFGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA *poud)
 *
 *  enable/gray dialog controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 *      poud    - pointer to src/dib image data structure
 ******************************************************************************/
static void TIFGrayedCtrls(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    OPTIONSUSERDATA PICFAR* poud)
{
    NOREFERENCE(ar);
    NOREFERENCE(poud);
   
    EnableWindow(
        GetDlgItem(hwndDlg, IDC_SD2FOPTTIF_EDIT_FILENAME),
        IsDlgButtonChecked(hwndDlg, IDC_SD2FOPTTIF_CHECK_APPEND));
    EnableWindow(
        GetDlgItem(hwndDlg, IDC_SD2FOPTTIF_BUTTON_BROWSE),
        IsDlgButtonChecked(hwndDlg, IDC_SD2FOPTTIF_CHECK_APPEND));
}
