/******************************************************************************
 *
 * MSWavlet.C - Minerva MDI Wavelet pack module
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
 *     SaveDefWavelet - opcode definition data to save wavelet image
 *
 * Local functions:
 *   ** call Pegasus to save image **
 *     SaveWavelet            - all the save wavelet interesting stuff
 *     InitPicParm           - initialize PIC_PARM from default save options settings
 *     PegasusReqInit        - Pegasus(REQ_INIT)
 *     PegasusReqExec        - Pegasus(REQ_EXEC)
 * 
 *  Pegasus(p, REQ_INIT) - handle all the REQ_INIT events
 *
 *   ** save options dialog **
 *     DefaultsDialogSaveWavelet - default save options dialog
 *     SaveOptionsDialog     - save options dialog for a specific image
 *
 *   ** save options dialog helpers **
 *     ImageOnCommand        - save image options WM_COMMAND handler
 *
 *   ** save options OnCommand helpers **
 *     ImageOptionsValid     - validate all the dialog settings on OK or Apply
 *     ImageGrayedCtrls      - enable/gray dialog controls based on current settings
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
#pragma warning(default:4115 4201 4214)
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <string.h>

/* pic includes */
#pragma warning(disable:4001)
#include "pic.h"
#include "pic2file.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h" 
#include "mdlg.h"
#include "merror.h"
#include "mmisc.h"
#include "mchild.h"
#include "mopen.h"



#define REALLOCEXTRA    ( 50000L )

extern OPENOP OpenDefWavelet;



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL SaveWavelet(
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrc,
    DWORD                    dwSrcLen,
    const PIC_PARM PICFAR*   pSrcPicParm,
    const BYTE PICHUGE*      pDib,
    DWORD                    dwDibLen,
    const PIC_PARM PICFAR*   pDibPicParm,
    BYTE PICHUGE* PICFAR*    ppSaved,
    DWORD PICFAR*            pdwLenSaved);
static void DefaultsDialogSaveWavelet(void);
static BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA pSaveData);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL PegasusPutNeedSpace(PIC_PARM PICFAR* p);
static BOOL SaveOptionsDialog(
    HWND            hwnd,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData);
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static void SelectEditComponent(HWND hwnd, int nComponent);
static BOOL NumberValid(HWND hwnd, int nCtrl, long nMin, long nMax, int nComponent);
static void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar);
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);



/******************************************************************************
 *  local data
 ******************************************************************************/



#define REQINIT  0
#define REQEXEC  1
#define REQTERM  2
#define ALLOC    3
#define MEMCPY   4
#define YIELD    5
#define TOTAL    6



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { 0, eText, IDC_SWAVETIME_TIMERRES, 0, sizeof("999.999") },
    { 0, eText, IDC_SWAVETIME_INIT,     0, sizeof("999.999") },
    { 0, eText, IDC_SWAVETIME_EXEC,     0, sizeof("999.999") },
    { 0, eText, IDC_SWAVETIME_TERM,     0, sizeof("999.999") },
    { 0, eText, IDC_SWAVETIME_ALLOC,    0, sizeof("999.999") },
    { 0, eText, IDC_SWAVETIME_MEMCPY,   0, sizeof("999.999") },
    { 0, eText, IDC_SWAVETIME_YIELD,    0, sizeof("999,999") },
    { 0, eText, IDC_SWAVETIME_TOTAL,    0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/* ini section for save options */
static char szOptionsSection[] = "Wavelet Pack Options";

/* ini keys for radio button groups in save options */
static char szOrientation[] = "Orientation-Rotation";

/* specifying save options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
     n/a  nested  dialog id    n/a   dialog button      dialog init data
    */
    /* put timing button first so we know what its index is */
    { 0, ePushButton, IDC_BUTTON_TIMING, FALSE, IDD_TIMING  },
    { szOptionsSection, eDialog, IDD_TIMING, FALSE, IDC_BUTTON_TIMING, (LPSTR)InitTimingData },

    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */
#define DEFAULT_START_IDX ( 2 )
    { "Y X Transform Filter Name", eEdit, IDC_SWAVEOPT_EDIT_Y_XFILTER,   FALSE, 8, "b09"   },
    { "Y Y Transform Filter Name", eEdit, IDC_SWAVEOPT_EDIT_Y_YFILTER,   FALSE, 8, "b09"   },
    { "Y Min Transform Level",     eEdit, IDC_SWAVEOPT_EDIT_Y_MINLEVEL,  FALSE, 2, "0"     },
    { "Y Max Transform Level",     eEdit, IDC_SWAVEOPT_EDIT_Y_MAXLEVEL,  FALSE, 2, "6"     },
    { "Y Quantization Threshold",  eEdit, IDC_SWAVEOPT_EDIT_Y_THRESHOLD, FALSE, 4, "1000"  },
    { "Y Quantization Table Name", eEdit, IDC_SWAVEOPT_EDIT_Y_TABLE,     FALSE, 5, "TESTY" },

    { "I X Transform Filter Name", eEdit, IDC_SWAVEOPT_EDIT_I_XFILTER,   FALSE, 8, "b09"   },
    { "I Y Transform Filter Name", eEdit, IDC_SWAVEOPT_EDIT_I_YFILTER,   FALSE, 8, "b09"   },
    { "I Min Transform Level",     eEdit, IDC_SWAVEOPT_EDIT_I_MINLEVEL,  FALSE, 2, "2"     },
    { "I Max Transform Level",     eEdit, IDC_SWAVEOPT_EDIT_I_MAXLEVEL,  FALSE, 2, "6"     },
    { "I Quantization Threshold",  eEdit, IDC_SWAVEOPT_EDIT_I_THRESHOLD, FALSE, 4, "1000"  },
    { "I Quantization Table Name", eEdit, IDC_SWAVEOPT_EDIT_I_TABLE,     FALSE, 5, "TESTI" },

    { "Q X Transform Filter Name", eEdit, IDC_SWAVEOPT_EDIT_Q_XFILTER,   FALSE, 8, "b09"   },
    { "Q Y Transform Filter Name", eEdit, IDC_SWAVEOPT_EDIT_Q_YFILTER,   FALSE, 8, "b09"   },
    { "Q Min Transform Level",     eEdit, IDC_SWAVEOPT_EDIT_Q_MINLEVEL,  FALSE, 2, "3"     },
    { "Q Max Transform Level",     eEdit, IDC_SWAVEOPT_EDIT_Q_MAXLEVEL,  FALSE, 2, "6"     },
    { "Q Quantization Threshold",  eEdit, IDC_SWAVEOPT_EDIT_Q_THRESHOLD, FALSE, 4, "1000"  },
    { "Q Quantization Table Name", eEdit, IDC_SWAVEOPT_EDIT_Q_TABLE,     FALSE, 5, "TESTQ" },
#define DEFAULT_END_IDX ( DEFAULT_START_IDX + 6 * 3 )

    { "Orientation-Inverted",  eCheckBox, IDC_SWAVEOPT_CHECK_REVERSED,         FALSE },
    { "CompressAsDisplayed",   eCheckBox, IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED, TRUE  },
    { "Expert Mode",           eCheckBox, IDC_SWAVEOPT_CHECK_EXPERTMODE,       FALSE },
    
    { szOrientation,  eRadioButton, IDC_SWAVEOPT_RADIO_ROTATE0,   TRUE,  0, "0"         },
    { szOrientation,  eRadioButton, IDC_SWAVEOPT_RADIO_ROTATE180, FALSE, 0, "180"       },
    { szOrientation,  eRadioButton, IDC_SWAVEOPT_RADIO_ROTATE270, FALSE, 0, "270"       },
    { szOrientation,  eRadioButton, IDC_SWAVEOPT_RADIO_ROTATE90,  FALSE, 0, "90"        },

    { "Comment",            eEdit, IDC_SWAVEOPT_EDIT_COMMENT,      FALSE, MAX_COMMENT },
    { "App Field",          eEdit, IDC_SWAVEOPT_EDIT_APPFIELD,     FALSE, MAX_COMMENT },
    { "Password",           eEdit, IDC_SWAVEOPT_EDIT_PASSWORD,     TRUE,  8 },

    { 0, ePushButton, IDC_SWAVEOPT_BUTTON_DEFAULT   },
    { 0, eText,       IDC_SWAVEOPT_STATIC_XFILTER   },
    { 0, eText,       IDC_SWAVEOPT_STATIC_YFILTER   },
    { 0, eText,       IDC_SWAVEOPT_STATIC_TABLE     },
    { 0, eCheckBox,   IDC_SWAVEOPT_CHECK_SETDEFAULT },

    { 0, eEnd, -1 }
    };
 


/******************************************************************************
 *  global data
 ******************************************************************************/



SAVEOP SaveDefWavelet =
    {
    OP_D2W,
    "Wavelet Compress",
    "Wavelet Defaults",
    "Wavelet (*.pic)|*.PIC|",
    0,
    SaveWavelet,
    DefaultsDialogSaveWavelet,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 * BOOL SaveWavelet(
 *     const BYTE PICHUGE*    pSrc,
 *     LPCSTR                 pszFilename,
 *     DWORD                  dwSrcLen,
 *     const PIC_PARM PICFAR* pSrcPicParm,
 *     const BYTE PICHUGE*    pDib,
 *     DWORD                  dwDibLen,
 *     const PIC_PARM PICFAR* pDibPicParm,
 *     BYTE PICHUGE* PICFAR*  ppSaved,
 *     DWORD PICFAR*          pdwLenSaved)
 * 
 *  all the save interesting stuff
 *
 *  parameters:
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
static BOOL SaveWavelet(
    HWND                     hWnd,
    LPCSTR                   pszFilename,
    const BYTE PICHUGE*      pSrc,
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
    BYTE PICHUGE *pSaveSrc;
    DWORD dwSaveSrcLen;
    DWORD dwPutLen;
    PIC_PARM SaveSrcPicParm;
    DWORD biCompressionSrc = pSrcPicParm->Head.biCompression;
    RESPONSE response;
    BITMAPFILEHEADER PICFAR* pbmfh;
    LPCTRLINITDATA pCtrl;
    BOOL bDisplayTiming;
    BOOL bYield;
    APPDATA App;
    BYTE PICHUGE *p;

    /**************************************************************************
        save options dialog
    */
    oud.hWnd        = hWnd;
    oud.pszFilename = pszFilename;
    oud.ppSrc       = (BYTE PICHUGE* PICFAR*)&pSrc;
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = (BYTE PICHUGE * PICFAR*)&pDib;    /* cast away constness */
    oud.pdwDibLen   = &dwDibLen;
    oud.pDibPicParm = (PIC_PARM PICFAR*)pDibPicParm;    /* cast away constness */
    
    if ( !SaveOptionsDialog(hWnd, &oud, &pSaveData) )
        return ( FALSE );

    /**************************************************************************
        initialize PIC_PARM settings
    */

    /* settings independent of source image type and independent of save settings */
    memset(&SaveSrcPicParm, '\0', sizeof(SaveSrcPicParm));
    SaveSrcPicParm.ParmSize     = sizeof(SaveSrcPicParm);
    SaveSrcPicParm.ParmVer      = CURRENT_PARMVER;
    SaveSrcPicParm.ParmVerMinor = 2;    /* 2 since it uses PIC2List features */
    SaveSrcPicParm.Op = OP_D2W;
    memset(&App, 0, sizeof(App));
    SaveSrcPicParm.App = (LPARAM)(APPDATA PICFAR*)&App;

    /* settings dependent on source image type */
    if ( CtrlDataIsPushed(pSaveData, IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED) )
        {
        if ( pDibPicParm->Head.biBitCount != 24 &&
             ( pDibPicParm->Head.biBitCount != 8 ||
               !MiscIsGrayPalette(pDibPicParm->Head.biClrUsed, pDibPicParm->ColorTable) ) )
            {
            ErrorMessage(STYLE_ERROR, IDS_SAVEWAVELETBITDEPTH);
            return ( FALSE );
            }

        /* save image as displayed is pretty simple,
            since the source image is the dib.  The only tricky part is that,
            if we need a color table, we want to use the dib's color table
            if it has one (bit depth <= 8), else we want to use the source
            image's color table
        */
        pSaveSrc = (BYTE PICHUGE*)pDib;
        dwSaveSrcLen = dwDibLen;
        SaveSrcPicParm.Head = pDibPicParm->Head;
        if ( SaveSrcPicParm.Head.biClrUsed != 0 )
            hmemcpy(
                SaveSrcPicParm.ColorTable,
                pDibPicParm->ColorTable,
                sizeof(SaveSrcPicParm.ColorTable));
        else
            {
            SaveSrcPicParm.Head.biClrUsed      = pSrcPicParm->Head.biClrUsed;
            SaveSrcPicParm.Head.biClrImportant = pSrcPicParm->Head.biClrImportant;
            hmemcpy(
                SaveSrcPicParm.ColorTable,
                pSrcPicParm->ColorTable,
                sizeof(SaveSrcPicParm.ColorTable));
            }            
        }
    else
        {
        /* save from source image is heavily dependent, of course, on the source image type.
            For source image wavelet, we can just take the source image as the
                saved image.
            For source image a bitmap, we can easily extract the original dib from
                the source image buffer.
            Else we don't yet handle the case
        */
        if ( MiscIsImageType(biCompressionSrc, &OpenDefWavelet) )
            {
            /* source image is wavelet */
            CtrlDataFree(&pSaveData);
            *ppSaved = (BYTE PICHUGE*)MiscGlobalAllocPtr(dwSrcLen, IDS_SAVEOUTOFMEMORY);
            if ( *ppSaved == 0 )
                return ( FALSE );
            hmemcpy(*ppSaved, pSrc, dwSrcLen);
            *pdwLenSaved = dwSrcLen;
            return ( TRUE );
            }
        if ( biCompressionSrc == BI_RGB )
            {
            /* best detail is dib in Src BMP
                (can't use MiscImageType on OpenDefBMP81 because it supports other
                than simple RGB which the code below requires, and can't use OpenDefBMP
                because it will have been zero'ed by the op81 init entry point) */
            if ( pSrcPicParm->Head.biBitCount != 24 &&
                 ( pSrcPicParm->Head.biBitCount != 8 ||
                   !MiscIsGrayPalette(pSrcPicParm->Head.biClrUsed, pSrcPicParm->ColorTable) ) )
                {
                ErrorMessage(STYLE_ERROR, IDS_SAVEWAVELETBITDEPTH);
                return ( FALSE );
                }
            pbmfh = (BITMAPFILEHEADER PICFAR*)pSrc;
            dwSaveSrcLen = pSrcPicParm->Head.biSizeImage;
            pSaveSrc = (BYTE PICHUGE*)pSrc + pbmfh->bfOffBits;
            SaveSrcPicParm.Head = pSrcPicParm->Head;
            hmemcpy(
                SaveSrcPicParm.ColorTable,
                pSrcPicParm->ColorTable,
                sizeof(SaveSrcPicParm.ColorTable));
            }
        else
            {        
            /* re-expand TGA, PJpeg, wavelet, IM4, GIF, TIF or PCX to DIB */
            /*#### TODO: TGA should have its own case, since pack supports it as a native
                input format and then Q_REVERSE, biHeight, etc. presumably are affected
                in the following */
            /*####*/
            MessageBox(
                hwndFrame,
                "This would re-expand from the SJPEG, PJPEG, IM4, GIF, TIF or PCX source "
                    "image to a DIB, then compress to wavelet format but it's not implemented yet.",
                APPLICATION_NAME,
                MB_OK);
            CtrlDataFree(&pSaveData);
            return ( FALSE );
            }
        }

    /* clear out any remnants from expanding to DIB */
    SaveSrcPicParm.Head.biHeight = labs(SaveSrcPicParm.Head.biHeight);

    pCtrl = CtrlData(pSaveData, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);
    bYield = 
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);

    /* save options-specific initialization of the pic_parm */
    if ( !InitPicParm(&SaveSrcPicParm, pSaveData) )
        {
        CtrlDataFree(&pSaveData);
        MiscGlobalFreePtr(&SaveSrcPicParm.u.WAVE.QlutlibPath);
        MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
        return ( FALSE );
        }
    if ( bYield )
        SaveSrcPicParm.u.WAVE.PicFlags |= PF_YieldGet | PF_YieldPut;

    App.dwTime[TOTAL] -= MiscTickCount();
    /* We could allocate more tightly than the following in guessing how big the
        output buffer needs to be.  Also, it's probably true that there is some
        horribly degenerate case where the output wavelet image is larger than the
        input dib so we still have to handle reallocating the output buffer
        even in the following simplistic case */
    /* note there must be a put buffer before req_init, there doesn't have to be a get buffer */
    *pdwLenSaved = 0;
    App.dwTime[ALLOC] -= MiscTickCount();
    dwPutLen = dwSaveSrcLen;
    if ( dwPutLen < 32 * 1024UL )
        dwPutLen = 32 * 1024UL;
	dwPutLen += 1024;
    *ppSaved = MiscGlobalAllocPtr(dwPutLen, IDS_SAVEOUTOFMEMORY);
    App.dwTime[ALLOC] += MiscTickCount();
        /* "There was not enough memory to save the image." */
    if ( *ppSaved == 0 )
        {
        MiscGlobalFreePtr(&SaveSrcPicParm.u.WAVE.QlutlibPath);
        MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
        CtrlDataFree(&pSaveData);
        return ( FALSE );
        }

    // currently, the put buffer size must be the same size as the dib, but not less than 8k
    SaveSrcPicParm.Put.Start = *ppSaved;
    SaveSrcPicParm.Put.End = SaveSrcPicParm.Put.Start + dwPutLen;
    SaveSrcPicParm.Put.Front = SaveSrcPicParm.Put.Start;
    SaveSrcPicParm.Put.Rear  = SaveSrcPicParm.Put.Front;
    SaveSrcPicParm.Put.QFlags = 0;

    /**************************************************************************
        Pegasus()
    */
    response = PegasusReqInit(&SaveSrcPicParm);
    if ( response == RES_DONE )
        {
        /* currently the get queue must be large enough for the entire input dib */
        SaveSrcPicParm.Get.Start = pSaveSrc;
        SaveSrcPicParm.Get.End   = SaveSrcPicParm.Get.Start + dwSaveSrcLen;
        SaveSrcPicParm.Get.Front = SaveSrcPicParm.Get.Start;
        SaveSrcPicParm.Get.Rear  = SaveSrcPicParm.Get.End;
        SaveSrcPicParm.Get.QFlags |= Q_EOF;

        response = PegasusReqExec(&SaveSrcPicParm);
        if ( response == RES_DONE )
            {
            App.dwTime[REQTERM] -= MiscTickCount();
            response = Pegasus(&SaveSrcPicParm, REQ_TERM);
            App.dwTime[REQTERM] += MiscTickCount();
            assert(response == RES_DONE);
            }
        }

    if ( response != RES_DONE )
        ErrorPegasusMessage(SaveSrcPicParm.Status, IDS_SAVEIMAGE);
    else
        {
        /* for PIC and JFIF output format, now have to update file size and/or color table */
        *pdwLenSaved += SaveSrcPicParm.Put.Rear - SaveSrcPicParm.Put.Front;
        App.dwTime[ALLOC] -= MiscTickCount();
        p = GlobalReAllocPtr(*ppSaved, *pdwLenSaved, GMEM_MOVEABLE);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( p == 0 )
            {
            ErrorMessage(STYLE_ERROR, IDS_SAVEOUTOFMEMORY);
            MiscGlobalFreePtr(ppSaved);
            *ppSaved = 0;            
            }
        else
            {        
            *ppSaved = p;
            }
        }        

    /* free output buffer, comment, appdata and quantization if present
        from save source DIB PIC_PARM */
    CtrlDataFree(&pSaveData);
    MiscGlobalFreePtr(&SaveSrcPicParm.u.WAVE.QlutlibPath);
    MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);

    App.dwTime[TOTAL] += MiscTickCount();

    if ( response == RES_DONE && *ppSaved != 0 && bDisplayTiming )
        MiscDisplayTimingDialog(
            hwndFrame,
            MAKEINTRESOURCE(IDD_SWAVETIME),
            App.dwTime,
            InitDisplayTiming);

    return ( response == RES_DONE && *ppSaved != 0 );
}



/******************************************************************************
 *  BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA pSaveData)
 * 
 *  initialize PIC_PARM from default save options settings
 *
 *  parameters:
 *      pPicParm    - PIC_PARM to init
 *      pSaveData   - default save options settings array
 *
 *  returns:
 *      FALSE if some error occurs
 ******************************************************************************/
static BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA ar)
{
    char sz[_MAX_PATH + 1];
    P2PktGeneric PICHUGE *pList;
    P2PktRawData PICHUGE *pRawDataPtr;

    CtrlDataGetText(ar, IDC_SWAVEOPT_EDIT_PASSWORD, sz, sizeof(sz));
    memset(pPicParm->KeyField, '\0', sizeof(pPicParm->KeyField));
    strncpy((char *)pPicParm->KeyField, sz, sizeof(pPicParm->KeyField));

    /* note that VisualOrient is saved with the image and can be retrieved
        using PegasusQuery or Pegasus(REQ_INIT) when expanding -- but
        it has 0 effect on packing or expanding otherwise */
    switch ( CtrlDataWhichRadio(ar, IDC_SWAVEOPT_RADIO_ROTATE0) )
        {
        case IDC_SWAVEOPT_RADIO_ROTATE0:
            pPicParm->VisualOrient = O_normal;
            break;
        case IDC_SWAVEOPT_RADIO_ROTATE90:
            pPicParm->VisualOrient = O_r90;
            break;
        case IDC_SWAVEOPT_RADIO_ROTATE180:
            pPicParm->VisualOrient = O_r180;
            break;
        case IDC_SWAVEOPT_RADIO_ROTATE270:
            pPicParm->VisualOrient = O_r270;
            break;
        default:
            assert(FALSE);
            break;
        }
    if ( CtrlDataIsPushed(ar, IDC_SWAVEOPT_CHECK_REVERSED) )
        pPicParm->VisualOrient |= O_inverted;

    /* wavelet-specific options */
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_Y_XFILTER,
        (LPSTR)pPicParm->u.WAVE.Y.XTransformFilter,
        sizeof(pPicParm->u.WAVE.Y.XTransformFilter) - 1);
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_Y_YFILTER,
        (LPSTR)pPicParm->u.WAVE.Y.YTransformFilter,
        sizeof(pPicParm->u.WAVE.Y.YTransformFilter) - 1);
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_Y_TABLE,
        (LPSTR)pPicParm->u.WAVE.Y.Qthr,
        sizeof(pPicParm->u.WAVE.Y.Qthr) - 1);
    pPicParm->u.WAVE.Y.MinTransformLevel =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_Y_MINLEVEL, FALSE);
    pPicParm->u.WAVE.Y.MaxTransformLevel =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_Y_MAXLEVEL, FALSE);
    pPicParm->u.WAVE.Y.QuantizationThreshold =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_Y_THRESHOLD, FALSE);
    
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_I_XFILTER,
        (LPSTR)pPicParm->u.WAVE.I.XTransformFilter,
        sizeof(pPicParm->u.WAVE.I.XTransformFilter) - 1);
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_I_YFILTER,
        (LPSTR)pPicParm->u.WAVE.I.YTransformFilter,
        sizeof(pPicParm->u.WAVE.I.YTransformFilter) - 1);
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_I_TABLE,
        (LPSTR)pPicParm->u.WAVE.I.Qthr,
        sizeof(pPicParm->u.WAVE.I.Qthr) - 1);
    pPicParm->u.WAVE.I.MinTransformLevel =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_I_MINLEVEL, FALSE);
    pPicParm->u.WAVE.I.MaxTransformLevel =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_I_MAXLEVEL, FALSE);
    pPicParm->u.WAVE.I.QuantizationThreshold =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_I_THRESHOLD, FALSE);
    
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_Q_XFILTER,
        (LPSTR)pPicParm->u.WAVE.Q.XTransformFilter,
        sizeof(pPicParm->u.WAVE.Q.XTransformFilter) - 1);
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_Q_YFILTER,
        (LPSTR)pPicParm->u.WAVE.Q.YTransformFilter,
        sizeof(pPicParm->u.WAVE.Q.YTransformFilter) - 1);
    CtrlDataGetText(
        ar,
        IDC_SWAVEOPT_EDIT_Q_TABLE,
        (LPSTR)pPicParm->u.WAVE.Q.Qthr,
        sizeof(pPicParm->u.WAVE.Q.Qthr) - 1);
    pPicParm->u.WAVE.Q.MinTransformLevel =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_Q_MINLEVEL, FALSE);
    pPicParm->u.WAVE.Q.MaxTransformLevel =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_Q_MAXLEVEL, FALSE);
    pPicParm->u.WAVE.Q.QuantizationThreshold =
        (BYTE)CtrlDataGetLong(ar, IDC_SWAVEOPT_EDIT_Q_THRESHOLD, FALSE);
    
    pList = MiscGlobalAllocPtr(
        ( MAX_COMMENT + 1 + sizeof(P2PktNull) ) * 4, IDS_SAVEOUTOFMEMORY);
        /*              ^ null terminator         ^ Comment, AppField, Watermark, Script */
    if ( pList == 0 )
        return ( FALSE );
    pPicParm->PIC2List = (CHAR PICHUGE *)pList;

    CtrlDataGetText(ar, IDC_SWAVEOPT_EDIT_COMMENT, (LPSTR)pList->Data, MAX_COMMENT + 1);
    pList->Type   = P2P_Comment;
    pList->Length = lstrlen((LPSTR)pList->Data);
    if ( pList->Length != 0 )
    {
        pList->Length++;    /* the comment should be stored null-terminated */
        pPicParm->PIC2ListLen += sizeof(P2PktNull) + pList->Length;
        pList = (P2PktGeneric PICHUGE*)( (BYTE PICHUGE*)pList + sizeof(P2PktNull) + pList->Length );
    }

    pRawDataPtr = (P2PktRawData PICHUGE*)pList;
    CtrlDataGetText(ar, IDC_SWAVEOPT_EDIT_APPFIELD, (LPSTR)pRawDataPtr->RawData, MAX_COMMENT + 1);
    pList->Type   = P2P_RawData;
    _fmemset(pRawDataPtr->RawDescription, 0, sizeof(pRawDataPtr->RawDescription));
    pRawDataPtr->RawLength = lstrlen((LPSTR)pRawDataPtr->RawData);
    if ( pRawDataPtr->RawLength != 0 )
    {
        /* appfield need not be null-terminated, but we do here anyway */
        pRawDataPtr->RawLength++;
        pList->Length = pRawDataPtr->RawLength + sizeof(pRawDataPtr->RawDescription) +
            sizeof(pRawDataPtr->RawLength);
        pPicParm->PIC2ListLen += sizeof(P2PktNull) + pList->Length;
        pList = (P2PktGeneric PICHUGE*)( (BYTE PICHUGE*)pList + sizeof(P2PktNull) + pList->Length );
    }

    if ( pPicParm->PIC2ListLen == 0 )
        MiscGlobalFreePtr(&pPicParm->PIC2List);
    else
    {
        pPicParm->PIC2List[pPicParm->PIC2ListLen++] = P2P_EOF;
        pPicParm->PIC2ListSize = pPicParm->PIC2ListLen;
    }
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

            case RES_PUT_DATA_YIELD:
            case RES_YIELD:
                pApp->dwTime[YIELD] -= MiscTickCount();
                if ( MiscYield() )
                    {
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[YIELD] += MiscTickCount();
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

    dwLen = MiscQLen(&p->Put);
    if ( pApp->pPut == 0 )
        {
        pApp->dwTime[ALLOC] -= MiscTickCount();
        pApp->pPut = MiscGlobalAllocPtr(
            p->Put.End - p->Put.Start + REALLOCEXTRA,
            IDS_SAVEOUTOFMEMORY);
        pApp->dwTime[ALLOC] += MiscTickCount();
        if ( pApp->pPut == 0 )
            {
            Pegasus(p, REQ_TERM);
            p->Status = ERR_NONE;
            return ( FALSE );
            }
        pApp->pPut0 = pApp->pPut;
        pApp->dwPutSize = p->Put.End - p->Put.Start + REALLOCEXTRA;
        pApp->dwPutLen = 0;
        }
    else if ( dwLen > pApp->dwPutSize )
        {
        /* unexpectedly have to reallocate the output buffer */
        pApp->dwTime[ALLOC] -= MiscTickCount();
        pRealloc = GlobalReAllocPtr(pApp->pPut0, pApp->dwPutLen + dwLen + REALLOCEXTRA, GMEM_MOVEABLE);
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
        pApp->dwPutSize = dwLen + REALLOCEXTRA;
        }
    pApp->dwTime[MEMCPY] -= MiscTickCount();
    if ( MiscQLen(&p->Put) != 0 )
        MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, 0);
    pApp->dwTime[MEMCPY] += MiscTickCount();
    pApp->dwPutLen += dwLen - MiscQLen(&p->Put);
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

    pApp->dwTime[REQEXEC] -= MiscTickCount();
    response = Pegasus(p, REQ_EXEC);
    pApp->dwTime[REQEXEC] += MiscTickCount();
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                return ( response );

            case RES_GET_DATA_YIELD:
            case RES_PUT_DATA_YIELD:
            case RES_YIELD:
                pApp->dwTime[YIELD] -= MiscTickCount();
                if ( MiscYield() )
                    {
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[YIELD] += MiscTickCount();
                break;

            case RES_PUT_NEED_SPACE:
                if ( !PegasusPutNeedSpace(p) )
                    return ( RES_ERR );
                break;
                
            case RES_POKE:
                assert(pApp->pPut0 != 0);
                hmemcpy(
                    pApp->pPut0 + p->SeekInfo,
                    p->Put.FrontEnd,
                    p->Put.RearEnd - p->Put.FrontEnd);
                break;
                
            case RES_GET_NEED_DATA:
                if ( ( p->Get.QFlags & Q_EOF ) != 0 )
                    p->Get.QFlags |= Q_IO_ERR;
                else
                    {                    
                    if ( pApp->dwGetLen == 0 )
                        p->Get.QFlags |= Q_EOF;
                    else
                        {
                        pApp->dwTime[MEMCPY] -= MiscTickCount();
                        MiscCopyToQueue(&pApp->pGet, &pApp->dwGetLen, &p->Get, 0);
                        pApp->dwTime[MEMCPY] += MiscTickCount();
                        }
                    }
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
        if ( !PegasusPutNeedSpace(p) )
            return ( RES_ERR );
        }
    return ( response );        
}



/******************************************************************************
 *  void DefaultsDialogSaveWavelet(void)
 * 
 *  default save options dialog
 * 
 *  notes:
 *      The processing specific to the wavelet save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
void DefaultsDialogSaveWavelet(void)
{
    LPCTRLINITDATA ar;
    
    if ( SaveOptionsDialog(hwndFrame, NULL, &ar) )
        CtrlDataFree(&ar);
}



/******************************************************************************
 *  BOOL SaveOptionsDialog(
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
 *      The processing specific to the Save wavelet save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static BOOL SaveOptionsDialog(
    HWND hWnd,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData)
{
    int result;

    /* read default settings from the ini file -- apply InitOptions settings
        where ini settings aren't present */
    if ( !CtrlDataDup(InitOptionsData, ppSaveData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_SAVEOPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the save options dialog." */
        return ( FALSE );
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppSaveData);
    if ( poud == 0 )
        {
        CtrlDataSetPushed(*ppSaveData, IDC_SWAVEOPT_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_CHECK_SETDEFAULT);
        }

    //#### following temporarily
    CtrlDataSetPushed(*ppSaveData, IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED, TRUE);
    CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED);

    CtrlDataSetInactive((LPCTRLINITDATA)(*ppSaveData)[1].pszText, IDC_TIMING_STATIC_INPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppSaveData)[1].pszText, IDC_TIMING_EDIT_INPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppSaveData)[1].pszText, IDC_TIMING_STATIC_OUTPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppSaveData)[1].pszText, IDC_TIMING_EDIT_OUTPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppSaveData)[1].pszText, IDC_TIMING_CHECK_YIELD);

    if ( !CtrlDataIsPushed(*ppSaveData, IDC_SWAVEOPT_CHECK_EXPERTMODE) )
    {
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_STATIC_XFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_STATIC_YFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_STATIC_TABLE);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Y_YFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Y_TABLE);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Y_XFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Y_YFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Y_TABLE);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_I_XFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_I_YFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_I_TABLE);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Q_XFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Q_YFILTER);
        CtrlDataSetInactive(*ppSaveData, IDC_SWAVEOPT_EDIT_Q_TABLE);
    }

    result = DlgDoModal(hWnd, MAKEINTRESOURCE(IDD_SWAVEOPT), ImageOnCommand, *ppSaveData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppSaveData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATESAVEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the save options dialog." */
        return ( FALSE );
        }             

    if ( CtrlDataIsPushed(*ppSaveData, IDC_SWAVEOPT_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppSaveData);

    return ( TRUE );
}



/******************************************************************************
 *  BOOL ImageOnCommand(
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
 *      pUserData   - as defined by SaveSequentialWavelet
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
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    int i;
    char sz[_MAX_PATH + sizeof(" Save Wavelet Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */

    NOREFERENCE(hwndDlg);
    NOREFERENCE(wNotifyCode);
    NOREFERENCE(arSaveData);
    
    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:
            if ( poud != 0 )
                {
                lstrcpy(sz, poud->pszFilename);
                lstrcat(sz, " Save Wavelet Options");
                SetWindowText(hwndDlg, sz);
                }
            ComboBox_AddString(GetDlgItem(hwndDlg, IDC_SWAVEOPT_COMBO_COMPONENT), "Y Component");
            ComboBox_AddString(GetDlgItem(hwndDlg, IDC_SWAVEOPT_COMBO_COMPONENT), "I Component");
            ComboBox_AddString(GetDlgItem(hwndDlg, IDC_SWAVEOPT_COMBO_COMPONENT), "Q Component");
            ComboBox_SetCurSel(GetDlgItem(hwndDlg, IDC_SWAVEOPT_COMBO_COMPONENT), 0);
            ImageGrayedCtrls(hwndDlg, arSaveData);
            break;

        case IDC_SWAVEOPT_COMBO_COMPONENT:
            if ( wNotifyCode == CBN_SELCHANGE )
            {
                SelectEditComponent(hwndDlg, ComboBox_GetCurSel(GetDlgItem(
                    hwndDlg,
                    IDC_SWAVEOPT_COMBO_COMPONENT)));
            }
            break;

        case IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_SWAVEOPT_BUTTON_DEFAULT:
            for ( i = DEFAULT_START_IDX; i <= DEFAULT_END_IDX; i++ )
                SetDlgItemText(hwndDlg, arSaveData[i].nID, arSaveData[i].pszText);
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SWAVEOPT);
            break;

        case IDOK:
            return ( ImageOptionsValid(hwndDlg, poud) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
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
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
{
    if ( poud != 0 && !IsDlgButtonChecked(hwndDlg, IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED) )
        {
        assert(MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefWavelet));
        return ( TRUE );    /* source image is wavelet */
        }

    if ( !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_Y_MINLEVEL, 0, 10, 0) ||
         !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_Y_MAXLEVEL, 0, 10, 0) ||
         !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_Y_THRESHOLD, 0, 1000, 0) )
        return ( FALSE );

    if ( !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_I_MINLEVEL, 0, 10, 1) ||
         !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_I_MAXLEVEL, 0, 10, 1) ||
         !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_I_THRESHOLD, 0, 1000, 0) )
        return ( FALSE );

    if ( !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_Q_MINLEVEL, 0, 10, 2) ||
         !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_Q_MAXLEVEL, 0, 10, 2) ||
         !NumberValid(hwndDlg, IDC_SWAVEOPT_EDIT_Q_THRESHOLD, 0, 1000, 2) )
        return ( FALSE );

    /* otherwise, it's default options or we're compressing from a dib and have to look
        at the other save options */
    return ( TRUE );
}



static void SelectEditComponent(HWND hwnd, int nComponent)
{
    if ( nComponent != 0 )
    {
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_XFILTER),   SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_YFILTER),   SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_MINLEVEL),  SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_MAXLEVEL),  SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_THRESHOLD), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_TABLE),     SW_HIDE);
    }
    if ( nComponent != 1 )
    {
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_XFILTER),   SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_YFILTER),   SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_MINLEVEL),  SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_MAXLEVEL),  SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_THRESHOLD), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_TABLE),     SW_HIDE);
    }
    if ( nComponent != 2 )
    {
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_XFILTER),   SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_YFILTER),   SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_MINLEVEL),  SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_MAXLEVEL),  SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_THRESHOLD), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_TABLE),     SW_HIDE);
    }
    switch ( nComponent )
    {
    case 0:
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_XFILTER),   SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_YFILTER),   SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_MINLEVEL),  SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_MAXLEVEL),  SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_THRESHOLD), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Y_TABLE),     SW_SHOW);
        break;
    case 1:
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_XFILTER),   SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_YFILTER),   SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_MINLEVEL),  SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_MAXLEVEL),  SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_THRESHOLD), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_I_TABLE),     SW_SHOW);
        break;
    case 2:
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_XFILTER),   SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_YFILTER),   SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_MINLEVEL),  SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_MAXLEVEL),  SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_THRESHOLD), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_SWAVEOPT_EDIT_Q_TABLE),     SW_SHOW);
        break;
    }
}



static BOOL NumberValid(HWND hwnd, int nCtrl, long nMin, long nMax, int nComponent)
{
    char sz[sizeof("4000000000")];
    long n;
    char *psz;
        
    n = GetDlgItemText(hwnd, nCtrl, sz, sizeof(sz));
    if ( n == 0 && nMin <= 0 && nMax >= 0 )
        return ( TRUE );

    n = strtol(sz, &psz, 10);
    if ( *psz == '\0' && nMin <= n && n <= nMax )
        return ( TRUE );
    ErrorMessage(STYLE_ERROR, IDS_OUTOFRANGE, nMin, nMax);
    ComboBox_SetCurSel(GetDlgItem(hwnd, IDC_SWAVEOPT_COMBO_COMPONENT), nComponent);
    SelectEditComponent(hwnd, nComponent);
    SetFocus(GetDlgItem(hwnd, nCtrl));
    Edit_SetSel(GetDlgItem(hwnd, nCtrl), 0, -1);
    return ( FALSE );
}



/******************************************************************************
 *  void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar)
 *
 *  enable/gray dialog controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 ******************************************************************************/
static void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar)
{
    BOOL fEnableOptions = IsDlgButtonChecked(hwndDlg, IDC_SWAVEOPT_CHECK_IMAGEASDISPLAYED);

    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_CHECK_REVERSED,    fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_RADIO_ROTATE0,     fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_RADIO_ROTATE180,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_RADIO_ROTATE270,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_RADIO_ROTATE90,    fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_COMMENT,      fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_APPFIELD,     fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_PASSWORD,     fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_WAVELET,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_COMMENT,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_APPFIELD,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_VISUALORIENTATION, fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_PASSWORD,   fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_WAVELET,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_COMBO_COMPONENT,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_XFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_YFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_MINLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_MAXLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_THRESHOLD,  fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_STATIC_TABLE,      fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Y_XFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Y_YFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Y_MINLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Y_MAXLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Y_THRESHOLD,  fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Y_TABLE,      fEnableOptions);
    
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_I_XFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_I_YFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_I_MINLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_I_MAXLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_I_THRESHOLD,  fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_I_TABLE,      fEnableOptions);
 
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Q_XFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Q_YFILTER,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Q_MINLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Q_MAXLEVEL,   fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Q_THRESHOLD,  fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SWAVEOPT_EDIT_Q_TABLE,      fEnableOptions);
}
