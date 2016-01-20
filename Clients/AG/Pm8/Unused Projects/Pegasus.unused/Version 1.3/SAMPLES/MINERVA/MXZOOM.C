/******************************************************************************
 *
 * MXZOOM.C - Minerva MDI zoom opcode
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
 *   TransformDefZoom - opcode definition data to zoom image
 *
 * Local functions:
 *   TransformZoom               - all the zoom transformation interesting stuff
 *   InitPicParm                 - initialize PIC_PARM from default zoom options settings
 *   PegasusReqInit              - Pegasus(p, REQ_INIT) - handle all the REQ_INIT events
 *   PegasusReqExec              - Pegasus(p, REQ_EXEC) - handle all the REQ_EXEC events
 *   TransformZoomDefaultsDialog - default zoom options dialog
 *   ZoomOptionsDialog           - zoom options dialog for default options and for a
 *                                 specific image
 *   ImageOnCommand              - zoom image options WM_COMMAND handler
 *   ImageIsDialogDataValid      - validate all the image options dialog settings on OK
 *   SetImageDimensions          - set horizontal, vertical and percent edit fields, making
 *                                 sure they don't exceed 9999 pixels and 999 percent
 *   DefaultsOnCommand           - zoom default options WM_COMMAND handler
 *   DefaultsIsDialogDataValid   - validate all the defaults dialog settings on OK
 *
 * Revision History:
 *   12-29-96  1.00.04  jrb  implemented
 *   11-26-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
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
#include "minerva.h" 
#include "mmisc.h"
#include "merror.h"
#include "mdlg.h"
#include "mchild.h"
#include "mopen.h"



extern TOOLSOP ToolDefUtility;

extern BOOL Op82BitDepth(
    DWORD dwBits,
    BOOL  bDither,
    const BYTE PICHUGE* pDib,     DWORD         dwDibLen,   const PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm);



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL TransformZoom(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrc,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm);
static void TransformZoomDefaultsDialog(void);
static void InitPicParm(PIC_PARM PICFAR* pPicParm, LPCTRLINITDATA ar);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL ZoomOptionsDialog(
    HWND hwnd,
    PFNDLGONCOMMAND pfnOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppZoomData);
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arZoomData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL ImageIsDialogDataValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static BOOL DefaultsOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arZoomData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL DefaultsIsDialogDataValid(HWND hwndDlg);
static void SetImageDimensions(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    int imagewidth,
    int imageheight,
    int horizontal,
    int vertical,
    int percent);



/******************************************************************************
 *  local data
 ******************************************************************************/



#define UPSAMPLE 0
#define REQINIT  1
#define REQEXEC  2
#define REQTERM  3
#define ALLOC    4
#define MEMCPY   5
#define YIELD    6
#define TOTALOP  7
#define PAINT    8
#define TOTAL    9



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { 0, eText, IDC_XZOOMTIME_TIMERRES,    0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_UPSAMPLE,    0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_INIT,        0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_EXEC,        0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_TERM,        0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_ALLOC,       0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_MEMCPY,      0, sizeof("999,999") },
    { 0, eText, IDC_XZOOMTIME_YIELD,       0, sizeof("999,999") },
    { 0, eText, IDC_XZOOMTIME_OPTOTAL,     0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_PAINT,       0, sizeof("999.999") },
    { 0, eText, IDC_XZOOMTIME_TOTAL,       0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/* ini section for image options */
static char szOptionsSection[] = "Zoom Options";

static char szGray[] = "Gray";  /* ini file key for Gray radio buttons */

/* specifying zoom options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Preserve Aspect Ratio", eCheckBox, IDC_XZOOMOPT_CHECK_PRESERVEASPECT, TRUE },
    { "Use Percent",           eCheckBox, IDC_XZOOMOPT_CHECK_USEPERCENT,     TRUE },
        /* Use Percent is invisible to the user.  it exists so the dialog box can return
            whether the percent edit or horizontal/vertical pixel edit was last changed */
    
    { szGray, eRadioButton, IDC_XZOOMOPT_RADIO_NOCHANGE, TRUE,  0, "No Change" },
    { szGray, eRadioButton, IDC_XZOOMOPT_RADIO_MAKEGRAY, FALSE, 0, "Make Gray" },
    { szGray, eRadioButton, IDC_XZOOMOPT_RADIO_MAKEMONOCHROME, FALSE, 0, "Make Monochrome" },

    { "Percent",    eEdit, IDC_XZOOMOPT_EDIT_PERCENT,    FALSE, 3, "100" },
    { "Horizontal", eEdit, IDC_XZOOMOPT_EDIT_HORIZONTAL, FALSE, 5 },
    { "Vertical",   eEdit, IDC_XZOOMOPT_EDIT_VERTICAL,   FALSE, 5 },

    { 0, eCheckBox, IDC_XZOOMOPT_CHECK_SETDEFAULT },

    /*
     n/a  nested  dialog id    n/a   dialog button      dialog init data
    */
    { 0, ePushButton, IDC_BUTTON_TIMING, FALSE, IDD_TIMING  },
    { szOptionsSection, eDialog, IDD_TIMING, FALSE, IDC_BUTTON_TIMING, (LPSTR)InitTimingData },

    { 0, eEnd, -1 }
    };



/******************************************************************************
 *  global data
 ******************************************************************************/



/* opcode definition data for minerva.c transform opcode table pTransformOpTable */
TRANSFORMOP TransformDefZoom =
    {
    OP_ZOOM,
    "Zoom/Halftone",
    "&Zoom/Halftone Defaults",
    "&Zoom/Halftone",
    0,
    TransformZoom,
    TransformZoomDefaultsDialog,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 * BOOL TransformZoom(
 *     BYTE PICHUGE* PICFAR*  ppSrc,
 *     DWORD PICFAR*          pdwSrcLen,
 *     PIC_PARM PICFAR*       pSrcPicParm,
 *     BYTE PICHUGE* PICFAR*  pDib,
 *     DWORD PICFAR*          pdwDibLen,
 *     PIC_PARM PICFAR*       pDibPicParm)
 * 
 *  all the zoom transformation interesting stuff
 *
 *  parameters:
 *      ppSrc       - pointer to pointer to source image in memory
 *      pdwSrcLen   - pointer to length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      ppDib       - pointer to pointer to dib
 *      pdwDibLen   - pointer to length of dib
 *      pDibPicParm - picparm for dib
 *
 *  returns:
 *      FALSE if some error occurs
 *      ppSrc will receive a pointer to a transformed source image, pdwSrcLen
 *      receives the new length
 ******************************************************************************/
static BOOL TransformZoom(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrc,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm)
{
    LPCTRLINITDATA pZoomData;
    OPTIONSUSERDATA oud;
    PIC_PARM pp;
    RESPONSE response;
    BYTE PICHUGE* pZoomOut = 0;
    DWORD dwZoomOutLen;
    DWORD dwZoomDibLen;
    BITMAPFILEHEADER PICFAR* pbmfi;
    BOOL  bDisplayTiming;
    BOOL  bYield;
    DWORD dwGetQSize;
    DWORD dwPutQSize;
    LPCTRLINITDATA pCtrl;
    BYTE PICHUGE *pGetQ = 0;
    BYTE PICHUGE *pPutQ = 0;
    APPDATA App;
    BYTE PICHUGE* pSaveSrc;
    DWORD dwSaveSrcLen;
    PIC_PARM ExtraPicParm;
    BYTE PICHUGE *pXDib;
    DWORD dwXDibLen;

    /**************************************************************************
        zoom only works if the source DIB is 24-bit or 8-bit
        16-bits result in error -1016 (ERR_UNIMPLEMENTED_FEATURE) and 1, 4 and 32 bits
        result in error -1409 (ERR_BAD_SRC_BITCOUNT)

        8-bit non gray scale works, but kind of accidentally in support of 8-bit gray,
        so we'll upsample to 24-bits if required and opcode 82 is available, else we'll
        forbid the zoom
    */
    if ( pDibPicParm->Head.biBitCount != 24 &&
         ( pDibPicParm->Head.biBitCount != 8 ||
           !MiscIsGrayPalette(pDibPicParm->Head.biClrImportant, pDibPicParm->ColorTable) ) )
        {
        if ( pDibPicParm->Head.biBitCount > 8 || ToolDefUtility.nFoundParmVer == 0 )
            {
            ErrorMessage(STYLE_ERROR, IDS_ZOOMSRCBITCOUNT);
            return ( FALSE );
            }
        /* else we can upsample to 24-bits to do the the zoom */
        }

    /**************************************************************************
        zoom options dialog
    */
    oud.hWnd        = hWnd;
    oud.ppSrc       = ppSrc;
    oud.pdwSrcLen   = pdwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = &(BYTE PICHUGE *)pDib;            /* cast away const */
    oud.pdwDibLen   = &dwDibLen;
    oud.pDibPicParm = (PIC_PARM PICFAR*)pDibPicParm;    /* cast away const */
    
    if ( !ZoomOptionsDialog(hWnd, ImageOnCommand, &oud, &pZoomData) )
        return ( FALSE );

    memset(&App, 0, sizeof(App));
    App.dwTime[TOTAL] -= MiscTickCount();
    if ( pDibPicParm->Head.biBitCount < 8 ||
         ( pDibPicParm->Head.biBitCount == 8 &&
           !MiscIsGrayPalette(pDibPicParm->Head.biClrImportant, pDibPicParm->ColorTable) ) )
        {
        App.dwTime[UPSAMPLE] -= MiscTickCount();
        /* upsample to 24-bit DIB if necessary */
        if ( !Op82BitDepth(
                24,
                FALSE,
                pDib,   dwDibLen,   pDibPicParm,
                &pXDib, &dwXDibLen, &ExtraPicParm) )
            {
            ErrorPegasusMessage(ExtraPicParm.Status, IDS_ZOOMIMAGE);
            return ( FALSE );
            }
        pDib = pXDib;
        dwDibLen = dwXDibLen;
        pDibPicParm = &ExtraPicParm;
        App.dwTime[UPSAMPLE] += MiscTickCount();
        }            

    /**************************************************************************
        initialize PIC_PARM settings
    */

    /* settings independent of source image type and independent of save settings */
    memset(&pp, '\0', sizeof(pp));
    pp.ParmSize     = sizeof(pp);
    pp.ParmVer      = CURRENT_PARMVER;
    pp.ParmVerMinor = 1;
    pp.Op           = OP_ZOOM;
    pp.App = (LPARAM)(APPDATA PICFAR*)&App;

    pp.Head = pDibPicParm->Head;
    hmemcpy(pp.ColorTable, pDibPicParm->ColorTable, sizeof(pDibPicParm->ColorTable));

    pCtrl = CtrlData(pZoomData, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);
    bYield = 
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);
    dwGetQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_INPUTK, FALSE) * 1024;
    dwPutQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_OUTPUTK, FALSE) * 1024;

    /* zoom options-specific initialization of the pic_parm */
    InitPicParm(&pp, pZoomData);
    if ( bYield )
        pp.u.ZOOM.PicFlags |= PF_YieldGet | PF_YieldPut;

    if ( dwGetQSize == 0 || dwGetQSize >= dwDibLen )
        {
        pp.Get.Start = (BYTE PICHUGE *)pDib;    /* cast away const */
        pp.Get.End   = pp.Get.Start + dwDibLen;
        pp.Get.Front = pp.Get.Start;
        pp.Get.Rear  = pp.Get.End;
        pp.Get.QFlags |= Q_EOF;
        }
    else
        {
        App.dwTime[ALLOC] -= MiscTickCount();
        pGetQ = MiscGlobalAllocPtr(dwGetQSize, IDS_ZOOMOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pGetQ == 0 )
            return ( FALSE );
        pp.Get.Start = pGetQ;            
        pp.Get.End   = pp.Get.Start + dwGetQSize;
        pp.Get.Front = pp.Get.Start;
        pp.Get.Rear  = pp.Get.Front;
        App.pGet     = (BYTE PICHUGE *)pDib;
        App.dwGetLen = dwDibLen;
        }

    /* finished with options data */
    CtrlDataFree(&pZoomData);

    /**************************************************************************
        Pegasus()
    */
    response = PegasusReqInit(&pp);
    if ( response != RES_DONE )
        {
        assert(response == RES_ERR);
        MiscGlobalFreePtr(&pGetQ);
        ErrorPegasusMessage(pp.Status, IDS_ZOOMIMAGE);
        return ( FALSE );
        }

    /*#### TODO: figure out how to carry comment along in zoomed image */

    /* allocate the output BMP buffer and initialize the BITMAPFILEHEADER,
        and BITMAPINFO
        note that u.ZOOM.BiOut, ZOOM's output BITMAPINFOHEADER is valid at this point */
    dwZoomDibLen = pp.u.ZOOM.BiOut.biSizeImage;
    dwZoomOutLen =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        pp.u.ZOOM.BiOut.biClrUsed * sizeof(RGBQUAD) +
        dwZoomDibLen;

    App.dwTime[ALLOC] -= MiscTickCount();
    pZoomOut = MiscGlobalAllocPtr(dwZoomOutLen + 1, IDS_ZOOMOUTOFMEMORY);
    App.dwTime[ALLOC] += MiscTickCount();
        /* + 1 to temporarily works around a zoom error.  If the output buffer
            is exactly the right size, ZOOM return RES_PUT_NEED_SPACE.  If the
            output buffer is larger than exactly the right size, ZOOM operates
            correctly */
    if ( pZoomOut == 0 )
        {
        Pegasus(&pp, REQ_TERM);
        MiscGlobalFreePtr(&pGetQ);
        return ( 0 );
        }

    /* initialize bitmapfileheader */
    pbmfi = (BITMAPFILEHEADER PICFAR*)pZoomOut;
    pbmfi->bfType      = 0x4d42; /* 'BM' */
    pbmfi->bfSize      = dwZoomOutLen;
    pbmfi->bfReserved1 = 0;
    pbmfi->bfReserved2 = 0;
    pbmfi->bfOffBits   = dwZoomOutLen - dwZoomDibLen;
        
    /* initialize bitmapinfoheader */
    hmemcpy(pZoomOut + sizeof(BITMAPFILEHEADER), &pp.u.ZOOM.BiOut, sizeof(BITMAPINFOHEADER));
    /* initialize colortable,
        note that pp.Head.biClrUsed longer applies to ColorTable */
    hmemcpy(
        pZoomOut + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
        pp.ColorTable,
        pp.u.ZOOM.BiOut.biClrUsed * sizeof(RGBQUAD));

    if ( dwPutQSize == 0 || dwPutQSize >= dwZoomDibLen ) 
        {
        pp.Put.Start = pZoomOut + pbmfi->bfOffBits;
        pp.Put.End = pp.Put.Start + dwZoomDibLen + 1;
            /* + 1 to temporarily works around a zoom error.  If the output buffer
                is exactly the right size, ZOOM return RES_PUT_NEED_SPACE.  If the
                output buffer is larger than exactly the right size, ZOOM operates
                correctly */
        }
    else
        {
        App.dwTime[ALLOC] -= MiscTickCount();
        pPutQ = MiscGlobalAllocPtr(dwPutQSize, IDS_ZOOMOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pPutQ == 0 )
            {
            Pegasus(&pp, REQ_TERM);
            MiscGlobalFreePtr(&pGetQ);
            MiscGlobalFreePtr(&pZoomOut);
            return ( FALSE );
            }
        pp.Put.Start  = pPutQ;
        pp.Put.End    = pPutQ + dwPutQSize;
        App.pPut      = pZoomOut + pbmfi->bfOffBits;;
        /* note App.dwPutLen isn't used for this opcode */
        App.dwPutSize = dwZoomDibLen;
        }
    pp.Put.Front = pp.Put.Start;
    pp.Put.Rear = pp.Put.Front;

    response = PegasusReqExec(&pp);
    if ( response != RES_DONE )
        {
        assert(response == RES_ERR);
        MiscGlobalFreePtr(&pGetQ);
        MiscGlobalFreePtr(&pPutQ);

        MiscGlobalFreePtr(&pZoomOut);
        ErrorPegasusMessage(pp.Status, IDS_ZOOMIMAGE);
        return ( FALSE );
        }
        
    App.dwTime[REQTERM] -= MiscTickCount();
    response = Pegasus(&pp, REQ_TERM);
    App.dwTime[REQTERM] += MiscTickCount();
    assert(response == RES_DONE);

    App.dwTime[ALLOC] -= MiscTickCount();
    MiscGlobalFreePtr(&pGetQ);
    MiscGlobalFreePtr(&pPutQ);
    App.dwTime[ALLOC] += MiscTickCount();

    App.dwTime[TOTALOP] = App.dwTime[TOTAL];
    App.dwTime[TOTALOP] += MiscTickCount();

    /* image was successfully transformed */
    if ( hWnd != NULL )
        {
        /* source image becomes zoomed BMP, regardless of its original type */
        pSaveSrc = *ppSrc;
        dwSaveSrcLen = *pdwSrcLen;
        ExtraPicParm = *pSrcPicParm;
        App.dwTime[PAINT] -= MiscTickCount();
        if ( !OpenImage(hWnd, pszFilename, pZoomOut, dwZoomOutLen, OP_SILENT | OP_INISETTINGS) )
            {
            *ppSrc = pSaveSrc;
            *pdwSrcLen = dwSaveSrcLen;
            *pSrcPicParm = ExtraPicParm;
            return ( FALSE );
            }
        App.dwTime[PAINT] += MiscTickCount();
        }
    App.dwTime[TOTAL] += MiscTickCount();

    if ( bDisplayTiming )
        MiscDisplayTimingDialog(
            hWnd,
            MAKEINTRESOURCE(IDD_XZOOMTIME),
            App.dwTime,
            InitDisplayTiming);
    return ( TRUE );
}



/******************************************************************************
 *  void InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA ar)
 * 
 *  initialize PIC_PARM from default zoom options settings
 *
 *  parameters:
 *      pPicParm    - PIC_PARM to init
 *      pSaveData   - default zoom options settings array
 ******************************************************************************/
static void InitPicParm(PIC_PARM PICFAR* pPicParm, LPCTRLINITDATA ar)
{
    if ( CtrlDataIsPushed(ar, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) &&
         CtrlDataIsPushed(ar, IDC_XZOOMOPT_CHECK_USEPERCENT) )
        {
        /* use zoom ratio feature if percent was the last edit changed */
        pPicParm->u.ZOOM.Mode = 1;
        pPicParm->u.ZOOM.NewWidth = CtrlDataGetLong(ar, IDC_XZOOMOPT_EDIT_PERCENT, FALSE);
        pPicParm->u.ZOOM.NewHeight = 100;
        }
    else
        {
        /* else zoom to new horizontal and vertical dimensions */
        pPicParm->u.ZOOM.Mode = 0;
        pPicParm->u.ZOOM.NewWidth = CtrlDataGetLong(ar, IDC_XZOOMOPT_EDIT_HORIZONTAL, FALSE);
        pPicParm->u.ZOOM.NewHeight = CtrlDataGetLong(ar, IDC_XZOOMOPT_EDIT_VERTICAL, FALSE);
        }

    switch ( CtrlDataWhichRadio(ar, IDC_XZOOMOPT_RADIO_NOCHANGE) )
        {
        case IDC_XZOOMOPT_RADIO_NOCHANGE:
            if ( pPicParm->Head.biBitCount == 8 )
                {
                /* if the image is grayscale, we have to still tell zoom to make
                    it grayscale or we'll end up with a 24-bit dib */
                pPicParm->u.ZOOM.PicFlags |= PF_ConvertGray;
                pPicParm->u.ZOOM.NewBitCount = 8;
                }
            else
                pPicParm->u.ZOOM.NewBitCount = 24;
            break;
            
        case IDC_XZOOMOPT_RADIO_MAKEGRAY:
            pPicParm->u.ZOOM.PicFlags |= PF_ConvertGray;
            pPicParm->u.ZOOM.NewBitCount = 8;
            break;

        case IDC_XZOOMOPT_RADIO_MAKEMONOCHROME:
            pPicParm->u.ZOOM.PicFlags |= PF_ConvertGray;
            /* PF_ConvertGray is actually redundant and ignored if
                NewBitCount == 1, but it seems more consistent to me */
            pPicParm->u.ZOOM.NewBitCount = 1;
            break;

        default:
            assert(FALSE);
            break;
        }
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
    APPDATA PICFAR* pApp = (APPDATA PICFAR*)p->App;
    RESPONSE response;
    
    pApp->dwTime[REQINIT] -= MiscTickCount();
    response = Pegasus(p, REQ_INIT);
    pApp->dwTime[REQINIT] += MiscTickCount();
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                return ( response );

            case RES_GET_DATA_YIELD:
            case RES_PUT_DATA_YIELD:
            case RES_YIELD:
                ////pApp->dwTime[YIELD] -= MiscTickCount();
                ////if ( MiscYield() )
                ////    {
                ////    Pegasus(p, REQ_TERM);
                ////    p->Status = ERR_NONE;
                ////    return ( RES_ERR );
                ////    }
                ////pApp->dwTime[YIELD] += MiscTickCount();
                break;

            case RES_GET_NEED_DATA:
                pApp->dwTime[MEMCPY] -= MiscTickCount();
                MiscCopyToQueue(&pApp->pGet, &pApp->dwGetLen, &p->Get, 0);
                pApp->dwTime[MEMCPY] += MiscTickCount();
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

    pApp->dwTime[REQEXEC] -= MiscTickCount();
    response = Pegasus(p, REQ_EXEC);
    pApp->dwTime[REQEXEC] += MiscTickCount();
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                return ( response );

            case RES_PUT_DATA_YIELD:
                /*#### note ZOOM actually shouldn't be returning this
                    unless PF_YieldPut is set.  Since ZOOM does, however,
                    for the time being, we tolerate it until ZOOM changes */
                if ( ( p->u.ZOOM.PicFlags & PF_YieldPut ) == 0 )
                    break;
            case RES_GET_DATA_YIELD:
            case RES_YIELD:
                ////if ( MiscYield() )
                ////    {
                ////    Pegasus(p, REQ_TERM);
                ////    p->Status = ERR_NONE;
                ////    return ( RES_ERR );
                ////    }
                break;

            case RES_GET_NEED_DATA:
                pApp->dwTime[MEMCPY] -= MiscTickCount();
                MiscCopyToQueue(&pApp->pGet, &pApp->dwGetLen, &p->Get, 0);
                pApp->dwTime[MEMCPY] += MiscTickCount();
                break;
                
            case RES_PUT_NEED_SPACE:
                pApp->dwTime[MEMCPY] -= MiscTickCount();
                MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, 0);
                pApp->dwTime[MEMCPY] += MiscTickCount();
                break;

            default:
                assert(FALSE);
                break;
            }
        pApp->dwTime[REQEXEC] -= MiscTickCount();
        response = Pegasus(p, REQ_CONT);
        pApp->dwTime[REQEXEC] += MiscTickCount();
        }
    if ( response == RES_DONE && pApp->pPut != 0 && p->Put.Front != p->Put.Rear )
        {
        pApp->dwTime[MEMCPY] -= MiscTickCount();
        MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, 0);
        pApp->dwTime[MEMCPY] += MiscTickCount();
        }
    return ( response );        
}



/******************************************************************************
 *  void TransformZoomDefaultsDialog(void)
 * 
 *  default zoom options dialog
 * 
 *  notes:
 *      The processing specific to the Zoom transform options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static void TransformZoomDefaultsDialog(void)
{
    LPCTRLINITDATA pZoomData;
    
    if ( ZoomOptionsDialog(hwndFrame, DefaultsOnCommand, NULL, &pZoomData) )
        CtrlDataFree(&pZoomData);
}



/******************************************************************************
 *  BOOL ZoomOptionsDialog(HWND hwnd, OPTIONSUSERDATA PICFAR* poud, LPCTRLINITDATA PICFAR* ppZoomData)
 * 
 *  zoom options dialog for default options and for a specific image
 *
 *  parameters:
 *      pfnOptionsOnCommand - WM_COMMAND handler for dialog
 *      poud                - pointers to source and dib image data (see mopcodes.h)
 *      ppZoomData          - receives pointer to zoom options settings
 * 
 *  notes:
 *      The processing specific to the Zoom transform options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static BOOL ZoomOptionsDialog(
    HWND hwnd,
    PFNDLGONCOMMAND pfnOptionsOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppZoomData)
{
    int result;

    /* read default settings from the ini file -- apply InitOptions settings
        where ini settings aren't present */
    if ( !CtrlDataDup(InitOptionsData, ppZoomData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_ZOOMOPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the zoom options dialog." */
        return ( FALSE );
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppZoomData);
    if ( poud == 0 )
        {
        CtrlDataSetPushed(*ppZoomData, IDC_XZOOMOPT_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppZoomData, IDC_XZOOMOPT_CHECK_SETDEFAULT);
        }

    result = DlgDoModal(hwnd, MAKEINTRESOURCE(IDD_XZOOMOPT), pfnOptionsOnCommand, *ppZoomData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppZoomData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEZOOMOPTIONSDIALOG);
            /* "An unexpected error occurred opening the zoom options dialog." */
        return ( FALSE );
        }             

    if ( CtrlDataIsPushed(*ppZoomData, IDC_XZOOMOPT_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppZoomData);
        
    return ( TRUE );
}



/******************************************************************************
 *  BOOL ImageOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arZoomData,
 *      LPCSTR          pszDlgTemplate, 
 *      void PICFAR*    pUserData)
 *
 *  zoom image options WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arZoomData  - zoom options settings
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
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arZoomData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData)
{
    char sz[_MAX_PATH + sizeof(" Zoom/Halftone Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */
    int percent;
    int vertical;
    int horizontal;
    BOOL fValid;
    
    NOREFERENCE(wNotifyCode);
    
    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:
            /* WM_INITDIALOG - make sure the ini defaults are reasonable (valid
                numbers != 0 */
             GetWindowText(poud->hWnd, sz, sizeof(sz) - ( sizeof(" Zoom/Halftone Options") - 1 ));
             strcat(sz, " Zoom/Halftone Options");
             SetWindowText(hwndDlg, sz);
             if ( poud->pDibPicParm->Head.biBitCount == 8 )
                {
                /* then it's gray scale because we made sure of that on entry to TransformZoom */
                EnableWindow(GetDlgItem(hwndDlg, IDC_XZOOMOPT_RADIO_MAKEGRAY), FALSE);
                /* force the user to think of this as no change and that it doesn't
                    make any sense to make it gray when it's already gray
                    in fact, we have to tell the ZOOM opcode to make it gray or it
                    will make it 24-bt */
                if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_RADIO_MAKEGRAY) )
                    CheckDlgButton(hwndDlg, IDC_XZOOMOPT_RADIO_NOCHANGE, TRUE);
                }
            if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) )
                {
                percent = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, &fValid, FALSE);
                if ( !fValid || percent == 0 || percent > 999 )
                    percent = 100;
                SetImageDimensions(
                    hwndDlg,
                    arZoomData,
                    (int)poud->pDibPicParm->Head.biWidth,
                    (int)poud->pDibPicParm->Head.biHeight,
                    MulDiv(percent, (int)poud->pDibPicParm->Head.biWidth, 100),
                    MulDiv(percent, (int)poud->pDibPicParm->Head.biHeight, 100),
                    percent);
                return ( TRUE );
                }
            horizontal = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, &fValid, FALSE);
            if ( !fValid || horizontal == 0 || horizontal > 9999 )
                SetDlgItemInt(
                    hwndDlg,
                    IDC_XZOOMOPT_EDIT_HORIZONTAL,
                    (int)poud->pDibPicParm->Head.biWidth,
                    FALSE);
            vertical = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, &fValid, FALSE);
            if ( !fValid || vertical == 0 || vertical > 9999 )
                SetDlgItemInt(
                    hwndDlg,
                    IDC_XZOOMOPT_EDIT_VERTICAL,
                    (int)poud->pDibPicParm->Head.biHeight,
                    FALSE);
            break;

        case IDC_XZOOMOPT_CHECK_PRESERVEASPECT:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) )
                    {
                    horizontal = GetDlgItemInt(
                        hwndDlg,
                        IDC_XZOOMOPT_EDIT_HORIZONTAL,
                        &fValid,
                        FALSE);
                    vertical = MulDiv(
                        horizontal,
                        (int)poud->pDibPicParm->Head.biHeight,
                        (int)poud->pDibPicParm->Head.biWidth);
                    SetImageDimensions(
                        hwndDlg,
                        arZoomData,
                        (int)poud->pDibPicParm->Head.biWidth,
                        (int)poud->pDibPicParm->Head.biHeight,
                        horizontal,
                        MulDiv(
                            horizontal,
                            (int)poud->pDibPicParm->Head.biHeight,
                            (int)poud->pDibPicParm->Head.biWidth),
                        MulDiv(horizontal, 100, (int)poud->pDibPicParm->Head.biWidth));
                    }
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_XZOOMOPT_EDIT_PERCENT:
            if ( wNotifyCode == EN_CHANGE )
                {
                GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, sz, sizeof(sz));
                CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_USEPERCENT, sz[0] != '\0');
                /* check usepercent (invisible to user) to return from the dialog
                    box the fact that the percent field was last changed */
                DlgSetModified(hwndDlg);
                }
            else if ( wNotifyCode == EN_KILLFOCUS )
                {
                percent = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, &fValid, FALSE);
                if ( fValid &&
                     percent != CtrlDataGetLong(arZoomData, IDC_XZOOMOPT_EDIT_PERCENT, FALSE) )
                    {
                    SetImageDimensions(
                        hwndDlg,
                        arZoomData,
                        (int)poud->pDibPicParm->Head.biWidth,
                        (int)poud->pDibPicParm->Head.biHeight,
                        MulDiv(percent, (int)poud->pDibPicParm->Head.biWidth, 100),
                        MulDiv(percent, (int)poud->pDibPicParm->Head.biHeight, 100),
                        percent);
                    CtrlDataSetLong(arZoomData, IDC_XZOOMOPT_EDIT_PERCENT, percent, FALSE);                        
                    }
                }
            break;

        case IDC_XZOOMOPT_EDIT_HORIZONTAL:
            if ( wNotifyCode == EN_CHANGE )
                {
                GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, sz, sizeof(sz));
                if ( sz[0] != '\0' )
                    {
                    CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_USEPERCENT, FALSE);
                    /* clear usepercent (invisible to user) to return from the dialog
                        box the fact that the percent field was not last changed */
                    if ( !IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) )
                        SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, "");
                    }
                DlgSetModified(hwndDlg);
                }
            else if ( wNotifyCode == EN_KILLFOCUS )
                {
                /* if zoom options for a particular image and preserving aspect,
                    then adjust vertical so aspect ratio is correct */
                horizontal = GetDlgItemInt(
                    hwndDlg,
                    IDC_XZOOMOPT_EDIT_HORIZONTAL,
                    &fValid,
                    FALSE);
                if ( fValid &&
                     horizontal != CtrlDataGetLong(arZoomData, IDC_XZOOMOPT_EDIT_HORIZONTAL, FALSE) )
                    {
                    if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) ||
                         ( GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, &fValid, FALSE) ==
                               (UINT)MulDiv(
                                   horizontal,
                                   (int)poud->pDibPicParm->Head.biHeight,
                                   (int)poud->pDibPicParm->Head.biWidth) &&
                           fValid ) )
                        SetImageDimensions(
                            hwndDlg,
                            arZoomData,
                            (int)poud->pDibPicParm->Head.biWidth,
                            (int)poud->pDibPicParm->Head.biHeight,
                            horizontal,
                            MulDiv(
                                horizontal,
                                (int)poud->pDibPicParm->Head.biHeight,
                                (int)poud->pDibPicParm->Head.biWidth),
                            MulDiv(horizontal, 100, (int)poud->pDibPicParm->Head.biWidth));
                    CtrlDataSetLong(arZoomData, IDC_XZOOMOPT_EDIT_HORIZONTAL, horizontal, FALSE);                        
                    }
                }
            break;
                
        case IDC_XZOOMOPT_EDIT_VERTICAL:
            if ( wNotifyCode == EN_CHANGE )
                {
                GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, sz, sizeof(sz));
                if ( sz[0] != '\0' )
                    {
                    /* clear usepercent (invisible to user) to return from the dialog
                        box the fact that the percent field was not last changed */
                    CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_USEPERCENT, FALSE);
                    if ( !IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) )
                        SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, "");
                    }
                DlgSetModified(hwndDlg);
                }
            else if ( wNotifyCode == EN_KILLFOCUS )
                {
                /* if zoom options for a particular image and preserving aspect,
                    then adjust horizontal so aspect ratio is correct */
                vertical = GetDlgItemInt(
                    hwndDlg,
                    IDC_XZOOMOPT_EDIT_VERTICAL,
                    &fValid,
                    FALSE);
                if ( fValid &&
                     vertical != CtrlDataGetLong(arZoomData, IDC_XZOOMOPT_EDIT_VERTICAL, FALSE) )
                    {
                    if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) ||
                         ( GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, &fValid, FALSE) ==
                               (UINT)MulDiv(
                                   vertical,
                                   (int)poud->pDibPicParm->Head.biWidth,
                                   (int)poud->pDibPicParm->Head.biHeight) &&
                           fValid ) )
                        SetImageDimensions(
                            hwndDlg,
                            arZoomData,
                            (int)poud->pDibPicParm->Head.biWidth,
                            (int)poud->pDibPicParm->Head.biHeight,
                            MulDiv(
                                vertical,
                                (int)poud->pDibPicParm->Head.biWidth,
                                (int)poud->pDibPicParm->Head.biHeight),
                            vertical,
                            MulDiv(vertical, 100, (int)poud->pDibPicParm->Head.biHeight));
                    CtrlDataSetLong(arZoomData, IDC_XZOOMOPT_EDIT_VERTICAL, vertical, FALSE);                        
                    }
                }
            break;
                
        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_XZOOMOPT);
            break;

        case IDOK:
            return ( ImageIsDialogDataValid(hwndDlg, poud) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL ImageIsDialogDataValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
 *
 *  validate all the image options dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      poud    - pointer to src/dib image data structure
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL ImageIsDialogDataValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
{
    int percent;
    int vertical;
    int horizontal;
    BOOL fValid;
    char sz[sizeof("9999")];

    NOREFERENCE(poud);

    GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, sz, sizeof(sz));
    if ( sz[0] != '\0' )
        {
        percent = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, &fValid, FALSE);
        if ( !fValid || percent == 0 )
            {
            ErrorMessage(STYLE_ERROR, IDS_ZOOMNUM);
            SetFocus(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT), 0, -1);
            return ( FALSE );
            }
        }                    

    horizontal = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, &fValid, FALSE);
    if ( !fValid || horizontal == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_ZOOMNUM);
        SetFocus(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL), 0, -1);
        return ( FALSE );
        }
    vertical = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, &fValid, FALSE);
    if ( !fValid || vertical == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_ZOOMNUM);
        SetFocus(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL), 0, -1);
        return ( FALSE );
        }
                            
    return ( TRUE );
}




/******************************************************************************
 *  void SetImageDimensions(
 *      HWND hwndDlg,
 *      LPCTRLINITDATA ar,
 *      int imagewidth,
 *      int imageheight,
 *      int horizontal,
 *      int vertical,
 *      int percent)
 *
 *  set horizontal, vertical and percent edit fields, making sure they don't
 *  exceed 9999 pixels and 999 percent
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      imagewidth
 *      imageheight
 *      horizontal  - (possibly) new horizontal
 *      vertical    - (possibly) new vertical
 *      percent     - (possibly) new percent
 ******************************************************************************/
static void SetImageDimensions(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    int imagewidth,
    int imageheight,
    int horizontal,
    int vertical,
    int percent)
{
    if ( horizontal > 9999 )
        {
        /* if horizontal > max, adjust percent and vertical so
            horizontal == max */
        horizontal = 9999;
        percent = MulDiv(100, horizontal, imagewidth);
        vertical = MulDiv(imageheight, horizontal, imagewidth);        
        }

    if ( vertical > 9999 )
        {
        /* if vertical > max, adjust percent and horizontal so
            vertical == max */
        vertical = 9999;
        percent = MulDiv(100, vertical, imageheight);
        horizontal = MulDiv(imagewidth, vertical, imageheight);
        }

    if ( percent > 999 )
        {
        /* if percent > max, adjust horizontal and vertical so
            percent == max */
        percent = 999;
        horizontal = MulDiv(imagewidth, percent, 100);
        vertical = MulDiv(imageheight, percent, 100);
        }
                
    SetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT,    percent,    FALSE);
    SetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, horizontal, FALSE);
    SetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL,   vertical,   FALSE);
    CtrlDataSetLong(ar,    IDC_XZOOMOPT_EDIT_PERCENT,    percent,    FALSE);                        
    CtrlDataSetLong(ar,    IDC_XZOOMOPT_EDIT_HORIZONTAL, horizontal, FALSE);                        
    CtrlDataSetLong(ar,    IDC_XZOOMOPT_EDIT_VERTICAL,   vertical,   FALSE);                        
}



/******************************************************************************
 *  BOOL DefaultsOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arZoomData,
 *      LPCSTR          pszDlgTemplate, 
 *      void PICFAR*    pUserData)
 *
 *  zoom default options WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arZoomData  - zoom options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - 0
 *
 *  returns:
 *      all but IDOK: FALSE if the command isn't handled, else TRUE
 *      IDOK: FALSE if dialog data is invalid (don't end dialog), else TRUE
 *
 *  notes:
 *      as the oncommand handler is called for the dialog and its
 *      children, pszDlgTemplate is used to tell which dialog is active
 ******************************************************************************/
static BOOL DefaultsOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arZoomData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData)
{
    int percent;
    int vertical;
    int horizontal;
    BOOL fValid;
    char sz[sizeof("9999")];
    
    NOREFERENCE(arZoomData);
    NOREFERENCE(pUserData);
    
    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:
            /* WM_INITDIALOG - make sure the ini defaults are reasonable (valid
                numbers != 0 */
            if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) )
                {
                percent = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, &fValid, FALSE);
                if ( !fValid || percent == 0 || percent > 999 )
                    percent = 100;
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, "");
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, "");
                return ( TRUE );
                }
            /* else !PreserveAspect */
            SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, "");            
            horizontal = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, &fValid, FALSE);
            if ( !fValid || horizontal == 0 || horizontal > 9999 )
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, "");
            vertical = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, &fValid, FALSE);
            if ( !fValid || vertical == 0 || vertical > 9999 )
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, "");
            break;

        case IDC_XZOOMOPT_CHECK_PRESERVEASPECT:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( IsDlgButtonChecked(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT) )
                    {
                    SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, "");
                    SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, "");
                    }
                else
                    SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, "");
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_XZOOMOPT_EDIT_PERCENT:
            if ( wNotifyCode == EN_CHANGE )
                {
                GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, sz, sizeof(sz));
                CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_USEPERCENT, sz[0] != '\0');
                /* check usepercent (invisible to user) to return from the dialog
                    box the fact that the percent field was last changed */
                DlgSetModified(hwndDlg);
                }
            else if ( wNotifyCode == EN_KILLFOCUS )
                {
                CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_PRESERVEASPECT, TRUE);
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, "");
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, "");
                }
            break;

        case IDC_XZOOMOPT_EDIT_HORIZONTAL:
            if ( wNotifyCode == EN_CHANGE )
                {
                GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, sz, sizeof(sz));
                if ( sz[0] != '\0' )
                    CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_USEPERCENT, FALSE);
                    /* clear usepercent (invisible to user) to return from the dialog
                        box the fact that the percent field was not last changed */
                DlgSetModified(hwndDlg);
                }
            else if ( wNotifyCode == EN_KILLFOCUS )
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, "");
            break;
                
        case IDC_XZOOMOPT_EDIT_VERTICAL:
            if ( wNotifyCode == EN_CHANGE )
                {
                GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, sz, sizeof(sz));
                if ( sz[0] != '\0' )
                    /* clear usepercent (invisible to user) to return from the dialog
                        box the fact that the percent field was not last changed */
                    CheckDlgButton(hwndDlg, IDC_XZOOMOPT_CHECK_USEPERCENT, FALSE);
                }
            else if ( wNotifyCode == EN_KILLFOCUS )
                SetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, "");
            break;
                
        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_XZOOMOPT);
            break;

        case IDOK:
            return ( DefaultsIsDialogDataValid(hwndDlg) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL DefaultsIsDialogDataValid(HWND hwndDlg)
 *
 *  validate all the defaults dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL DefaultsIsDialogDataValid(HWND hwndDlg)
{
    int percent;
    int vertical;
    int horizontal;
    BOOL fValid;
    char sz[sizeof("9999")];

    GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, sz, sizeof(sz));
    if ( sz[0] != '\0' )
        {
        horizontal = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL, &fValid, FALSE);
        if ( !fValid || horizontal == 0 )
            {
            ErrorMessage(STYLE_ERROR, IDS_ZOOMNUM);
            SetFocus(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_HORIZONTAL), 0, -1);
            return ( FALSE );
            }
        }

    GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, sz, sizeof(sz));
    if ( sz[0] != '\0' )
        {
        vertical = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL, &fValid, FALSE);
        if ( !fValid || vertical == 0 )
            {
            ErrorMessage(STYLE_ERROR, IDS_ZOOMNUM);
            SetFocus(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_VERTICAL), 0, -1);
            return ( FALSE );
            }
        }
                            
    GetDlgItemText(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, sz, sizeof(sz));
    if ( sz[0] != '\0' )
        {
        percent = GetDlgItemInt(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT, &fValid, FALSE);
        if ( !fValid || percent == 0 )
            {
            ErrorMessage(STYLE_ERROR, IDS_ZOOMNUM);
            SetFocus(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_XZOOMOPT_EDIT_PERCENT), 0, -1);
            return ( FALSE );
            }
        }                    

    return ( TRUE );
}
