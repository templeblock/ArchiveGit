/******************************************************************************
 *
 * MSIMStar.C - Minerva MDI IMStar pack module
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
 *     SaveDefIMStar - opcode definition data to save imstar image
 *
 * Local functions:
 *   ** call Pegasus to save image **
 *     SaveIMStar            - all the save imstar interesting stuff
 *     InitPicParm           - initialize PIC_PARM from default save options settings
 *     PegasusReqInit        - Pegasus(REQ_INIT)
 *     PegasusReqExec        - Pegasus(REQ_EXEC)
 * 
 *  Pegasus(p, REQ_INIT) - handle all the REQ_INIT events
 *
 *   ** save options dialog **
 *     DefaultsDialogSaveIMStar - default save options dialog
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

extern OPENOP OpenDefIMStar;



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void InitSaveIMStarPlus(void);
static BOOL SaveIMStar(
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
static void DefaultsDialogSaveIMStar(void);
static BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA pSaveData);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL PegasusPutNeedSpace(PIC_PARM PICFAR* p);
static BOOL SaveOptionsDialog(
    HWND            hwnd,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData);
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static BOOL NumberValid(HWND hwnd, int nCtrl, long nMin, long nMax);
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

    { 0, eText, IDC_SLLTIME_TIMERRES, 0, sizeof("999.999") },
    { 0, eText, IDC_SLLTIME_INIT,     0, sizeof("999.999") },
    { 0, eText, IDC_SLLTIME_EXEC,     0, sizeof("999.999") },
    { 0, eText, IDC_SLLTIME_TERM,     0, sizeof("999.999") },
    { 0, eText, IDC_SLLTIME_ALLOC,    0, sizeof("999.999") },
    { 0, eText, IDC_SLLTIME_MEMCPY,   0, sizeof("999.999") },
    { 0, eText, IDC_SLLTIME_YIELD,    0, sizeof("999,999") },
    { 0, eText, IDC_SLLTIME_TOTAL,    0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/* ini section for save options */
static char szOptionsSection[] = "IMStar Pack Options";

/* ini keys for radio button groups in save options */
static char szCompMethod[]  = "Compression Method";
static char szOrientation[] = "Orientation-Rotation";

/* specifying save options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Orientation-Inverted",  eCheckBox, IDC_SLLOPT_CHECK_REVERSED,         FALSE },
    { "CompressAsDisplayed",   eCheckBox, IDC_SLLOPT_CHECK_IMAGEASDISPLAYED, FALSE },
    { "TransparentColorIndex", eCheckBox, IDC_SLLOPT_CHECK_TRANSPARENT,      FALSE },
    
    { szOrientation,  eRadioButton, IDC_SLLOPT_RADIO_ROTATE0,   TRUE,  0, "0"         },
    { szOrientation,  eRadioButton, IDC_SLLOPT_RADIO_ROTATE180, FALSE, 0, "180"       },
    { szOrientation,  eRadioButton, IDC_SLLOPT_RADIO_ROTATE270, FALSE, 0, "270"       },
    { szOrientation,  eRadioButton, IDC_SLLOPT_RADIO_ROTATE90,  FALSE, 0, "90"        },
    { szCompMethod,   eRadioButton, IDC_SLLOPT_RADIO_PPMD,      TRUE,  0, "PPMD"      },
    { szCompMethod,   eRadioButton, IDC_SLLOPT_RADIO_JPEG,      FALSE, 0, "JPEG"      },

    { "Comment",            eEdit, IDC_SLLOPT_EDIT_COMMENT,      FALSE, MAX_COMMENT },
    { "App Field",          eEdit, IDC_SLLOPT_EDIT_APPFIELD,     FALSE, MAX_COMMENT },
    { "Watermark",          eEdit, IDC_SLLOPT_EDIT_WATERMARK,    FALSE, MAX_COMMENT },
    { "Script",             eEdit, IDC_SLLOPT_EDIT_SCRIPT,       FALSE, MAX_COMMENT },
    { "Password",           eEdit, IDC_SLLOPT_EDIT_PASSWORD,     TRUE,  8 },
    
    { "Page Number",        eEdit, IDC_SLLOPT_EDIT_PAGENUM,      FALSE, 5 },
    { "Total Pages",        eEdit, IDC_SLLOPT_EDIT_TOTALPAGES,   FALSE, 5 },
    { "Transparent Index",  eEdit, IDC_SLLOPT_EDIT_TRANSPARENTINDEX, FALSE, 3 },
    { "Left Offset",        eEdit, IDC_SLLOPT_EDIT_LEFT,         FALSE, 5 },
    { "Top Offset",         eEdit, IDC_SLLOPT_EDIT_TOP,          FALSE, 5 },
    { "Delay",              eEdit, IDC_SLLOPT_EDIT_DELAY,        FALSE, 5 },
    { "Disposition",        eEdit, IDC_SLLOPT_EDIT_DISP,         FALSE, 3 },
    { "Error Limit",        eEdit, IDC_SLLOPT_EDIT_ERRORLIMIT,   FALSE, 2 },
    { "Order",              eEdit, IDC_SLLOPT_EDIT_ORDER,        FALSE, 1 },
    
    { 0, eCheckBox, IDC_SLLOPT_CHECK_SETDEFAULT },

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



SAVEOP SaveDefIMStar =
    {
    OP_LIP3,
    "IMStar Compress",
    "IMStar Defaults",
    "IMStar (*.pic)|*.PIC|",
    0,
    SaveIMStar,
    DefaultsDialogSaveIMStar,
    0
    };

SAVEOP SaveDefIMStarPlus =
    {
    OP_LIP3PLUS,
    "Enhanced IMStar Compress",
    0,
    0,
    InitSaveIMStarPlus,
    SaveIMStar,
    0,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



void InitSaveIMStarPlus(void)
{
    SaveDefIMStarPlus.pszFilter = SaveDefIMStar.pszFilter;
    SaveDefIMStarPlus.pszDefaultsMenu = SaveDefIMStar.pszDefaultsMenu;
    SaveDefIMStarPlus.pfnDefaultsDialog = SaveDefIMStar.pfnDefaultsDialog;
    if ( SaveDefIMStar.nFoundParmVer != 0 )
        PegasusUnload(SaveDefIMStar.lOpcode, CURRENT_PARMVER);
	memcpy(&SaveDefIMStar, &SaveDefIMStarPlus, sizeof(SaveDefIMStar));
    memset(&SaveDefIMStarPlus, '\0', sizeof(SaveDefIMStarPlus));
}



/******************************************************************************
 * BOOL SaveIMStar(
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
static BOOL SaveIMStar(
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
    BYTE PICHUGE *pAllocSaveSrc = 0;
    DWORD dwSaveSrcLen;
    DWORD dwPutLen;
    PIC_PARM SaveSrcPicParm;
    DWORD biCompressionSrc = pSrcPicParm->Head.biCompression;
    RESPONSE response;
    BITMAPFILEHEADER PICFAR* pbmfh;
    LPCTRLINITDATA pCtrl;
    BOOL bDisplayTiming;
    BOOL bYield;
    DWORD dwGetQSize;
    DWORD dwPutQSize;
    BYTE PICHUGE* pGetQ = 0;
    BYTE PICHUGE* pPutQ = 0;
    APPDATA App;
    DWORD dwSize;
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
	SaveSrcPicParm.Op = SaveDefIMStar.lOpcode;
    memset(&App, 0, sizeof(App));
    SaveSrcPicParm.App = (LPARAM)(APPDATA PICFAR*)&App;

    /* settings dependent on source image type */
    if ( CtrlDataIsPushed(pSaveData, IDC_SLLOPT_CHECK_IMAGEASDISPLAYED) )
        {
        if ( pDibPicParm->Head.biBitCount > 8 )
            {
            CtrlDataFree(&pSaveData);
            ErrorMessage(STYLE_ERROR, IDS_SAVEIMSTARBITDEPTH);
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
            For source image IMStar, we can just take the source image as the
                saved image.
            For source image a bitmap, we can easily extract the original dib from
                the source image buffer.
            Else we don't yet handle the case
        */
        if ( MiscIsImageType(biCompressionSrc, &OpenDefIMStar) )
            {
            /* source image is IMStar */
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
            /* re-expand TGA, PJpeg, IMStar, IM4, GIF, TIF or PCX to DIB */
            /*#### TODO: TGA should have its own case, since pack supports it as a native
                input format and then Q_REVERSE, biHeight, etc. presumably are affected
                in the following */
            /*####*/
            MessageBox(
                hwndFrame,
                "This would re-expand from the SJPEG, PJPEG, IM4, GIF, TIF or PCX source "
                    "image to a DIB, then compress to IMStar format but it's not implemented yet.",
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
    dwGetQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_INPUTK, FALSE) * 1024;
    dwPutQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_OUTPUTK, FALSE) * 1024;

    /* save options-specific initialization of the pic_parm */
    if ( !InitPicParm(&SaveSrcPicParm, pSaveData) )
        {
        MiscGlobalFreePtr(&pAllocSaveSrc);
        MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
        CtrlDataFree(&pSaveData);
        return ( FALSE );
        }
    if ( bYield )
        SaveSrcPicParm.u.D2S.PicFlags |= PF_YieldGet | PF_YieldPut;

    App.dwTime[TOTAL] -= MiscTickCount();
    /* We could allocate more tightly than the following in guessing how big the
        output buffer needs to be.  Also, it's probably true that there is some
        horribly degenerate case where the output imstar image is larger than the
        input dib so we still have to handle reallocating the output buffer
        even in the following simplistic case */
    /* note there must be a put buffer before req_init, there doesn't have to be a get buffer */
    *pdwLenSaved = 0;
    App.dwTime[ALLOC] -= MiscTickCount();
    dwPutLen = dwSaveSrcLen;
    if ( dwPutLen < 4096 )
        dwPutLen = 4096;
    *ppSaved = MiscGlobalAllocPtr(dwPutLen, IDS_SAVEOUTOFMEMORY);
    App.dwTime[ALLOC] += MiscTickCount();
        /* "There was not enough memory to save the image." */
    if ( *ppSaved == 0 )
        {
        MiscGlobalFreePtr(&pAllocSaveSrc);
        CtrlDataFree(&pSaveData);
        MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
        return ( FALSE );
        }

    if ( dwPutQSize == 0 || dwPutQSize >= dwPutLen )
        {
        SaveSrcPicParm.Put.Start = *ppSaved;
        SaveSrcPicParm.Put.End = SaveSrcPicParm.Put.Start + dwPutLen;
        }
    else
        {
        // note that minimium put buffer size is 1024 bytes, as we are only accepting
        // K, it would be a wierd error to be less than 1024 here
        assert(dwPutQSize >= 1024);

        App.dwTime[ALLOC] -= MiscTickCount();
        pPutQ = MiscGlobalAllocPtr(dwPutQSize, IDS_SAVEOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pPutQ == 0 )
            {
            MiscGlobalFreePtr(&pAllocSaveSrc);
            CtrlDataFree(&pSaveData);
            MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
            return ( FALSE );
            }
        SaveSrcPicParm.Put.Start = pPutQ;
        SaveSrcPicParm.Put.End = SaveSrcPicParm.Put.Start + dwPutQSize;
        App.dwPutSize = dwPutLen;
        App.dwPutLen  = 0;
        App.pPut      = *ppSaved;
        App.pPut0     = App.pPut;
        }        
    SaveSrcPicParm.Put.Front = SaveSrcPicParm.Put.Start;
    SaveSrcPicParm.Put.Rear  = SaveSrcPicParm.Put.Front;
    SaveSrcPicParm.Put.QFlags = 0;

    /**************************************************************************
        Pegasus()
    */
    response = PegasusReqInit(&SaveSrcPicParm);
    if ( App.pPut0 != 0 && App.pPut0 != *ppSaved )
        *ppSaved = App.pPut0;
    if ( response == RES_DONE )
        {
        if ( dwGetQSize == 0 || dwGetQSize >= dwSaveSrcLen )
            {
            SaveSrcPicParm.Get.Start = pSaveSrc;
            SaveSrcPicParm.Get.End   = SaveSrcPicParm.Get.Start + dwSaveSrcLen;
            SaveSrcPicParm.Get.Front = SaveSrcPicParm.Get.Start;
            SaveSrcPicParm.Get.Rear  = SaveSrcPicParm.Get.End;
            SaveSrcPicParm.Get.QFlags |= Q_EOF;
            }
        else
            {
            /* note get queue length must be at least StripSize */
            if ( dwGetQSize > (DWORD)SaveSrcPicParm.u.LL3.StripSize )
                dwSize = dwGetQSize;
            else
                dwSize = SaveSrcPicParm.u.LL3.StripSize;
            App.dwTime[ALLOC] -= MiscTickCount();
            pGetQ = MiscGlobalAllocPtr(dwSize, IDS_SAVEOUTOFMEMORY);
            App.dwTime[ALLOC] += MiscTickCount();
            if ( pGetQ == 0 )
                {
                Pegasus(&SaveSrcPicParm, REQ_TERM);
                MiscGlobalFreePtr(&pAllocSaveSrc);
                CtrlDataFree(&pSaveData);
                MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
                return ( FALSE );
                }        
            SaveSrcPicParm.Get.Start = pGetQ;
            SaveSrcPicParm.Get.End   = pGetQ + dwSize;
            SaveSrcPicParm.Get.Front = SaveSrcPicParm.Get.Start;
            SaveSrcPicParm.Get.Rear  = SaveSrcPicParm.Get.Front;
            App.dwGetLen = dwSaveSrcLen;
            App.pGet  = pSaveSrc;
            }

        response = PegasusReqExec(&SaveSrcPicParm);
        if ( App.pPut0 != 0 && App.pPut0 != *ppSaved )
            *ppSaved = App.pPut0;
        if ( response == RES_DONE )
            {
            App.dwTime[REQTERM] -= MiscTickCount();
            response = Pegasus(&SaveSrcPicParm, REQ_TERM);
            App.dwTime[REQTERM] += MiscTickCount();
            assert(response == RES_DONE);
            }
        }
    App.dwTime[ALLOC] -= MiscTickCount();
    MiscGlobalFreePtr(&pGetQ);
    MiscGlobalFreePtr(&pPutQ);
    App.dwTime[ALLOC] += MiscTickCount();
    MiscGlobalFreePtr(&pAllocSaveSrc);

    if ( response != RES_DONE )
        ErrorPegasusMessage(SaveSrcPicParm.Status, IDS_SAVEIMAGE);
    else
        {
        /* for PIC and JFIF output format, now have to update file size and/or color table */
        if ( App.pPut != 0 )
            *pdwLenSaved = App.dwPutLen;
        else
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
    MiscGlobalFreePtr(&SaveSrcPicParm.PIC2List);
    CtrlDataFree(&pSaveData);

    App.dwTime[TOTAL] += MiscTickCount();

    if ( response == RES_DONE && *ppSaved != 0 && bDisplayTiming )
        MiscDisplayTimingDialog(
            hwndFrame,
            MAKEINTRESOURCE(IDD_SLLTIME),
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

    switch ( CtrlDataWhichRadio(ar, IDC_SLLOPT_RADIO_PPMD) )
        {
        case IDC_SLLOPT_RADIO_PPMD:
            pPicParm->u.LL3.CompMethod = 'P';
            break;
        case IDC_SLLOPT_RADIO_JPEG:
            pPicParm->u.LL3.CompMethod = 'J';
            break;
        default:
            assert(FALSE);
            break;
        }
       
    CtrlDataGetText(ar, IDC_SLLOPT_EDIT_PASSWORD, sz, sizeof(sz));
    memset(pPicParm->KeyField, '\0', sizeof(pPicParm->KeyField));
    strncpy((char *)pPicParm->KeyField, sz, sizeof(pPicParm->KeyField));

    /* note that VisualOrient is saved with the image and can be retrieved
        using PegasusQuery or Pegasus(REQ_INIT) when expanding -- but
        it has 0 effect on packing or expanding otherwise */
    switch ( CtrlDataWhichRadio(ar, IDC_SLLOPT_RADIO_ROTATE0) )
        {
        case IDC_SLLOPT_RADIO_ROTATE0:
            pPicParm->VisualOrient = O_normal;
            break;
        case IDC_SLLOPT_RADIO_ROTATE90:
            pPicParm->VisualOrient = O_r90;
            break;
        case IDC_SLLOPT_RADIO_ROTATE180:
            pPicParm->VisualOrient = O_r180;
            break;
        case IDC_SLLOPT_RADIO_ROTATE270:
            pPicParm->VisualOrient = O_r270;
            break;
        default:
            assert(FALSE);
            break;
        }
    if ( CtrlDataIsPushed(ar, IDC_SLLOPT_CHECK_REVERSED) )
        pPicParm->VisualOrient |= O_inverted;

    pPicParm->u.LL3.PageNum = (SHORT)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_PAGENUM, FALSE);
    pPicParm->u.LL3.NumOfPages = (SHORT)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_TOTALPAGES, FALSE);
    if ( CtrlDataIsPushed(ar, IDC_SLLOPT_CHECK_TRANSPARENT) )
        pPicParm->u.LL3.Transparent = (SHORT)
            CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_TRANSPARENTINDEX, FALSE);
    else
        pPicParm->u.LL3.Transparent = -1;
    pPicParm->u.LL3.XOff = (SHORT)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_LEFT, TRUE);
    pPicParm->u.LL3.YOff = (SHORT)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_TOP, TRUE);
    pPicParm->u.LL3.UserDelay = (SHORT)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_DELAY, FALSE);
    pPicParm->u.LL3.DispMethod = (BYTE)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_DISP, FALSE);

    pPicParm->u.LL3.AllowedBitErr = (BYTE)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_ERRORLIMIT, FALSE);
    if ( pPicParm->u.LL3.CompMethod == 'P' )
        pPicParm->u.LL3.CompOrder = (BYTE)CtrlDataGetLong(ar, IDC_SLLOPT_EDIT_ORDER, FALSE);

    pPicParm->u.LL3.PTuning = 0;

    pList = MiscGlobalAllocPtr(
        ( MAX_COMMENT + 1 + sizeof(P2PktNull) ) * 4, IDS_SAVEOUTOFMEMORY);
        /*              ^ null terminator         ^ Comment, AppField, Watermark, Script */
    if ( pList == 0 )
        return ( FALSE );
    pPicParm->PIC2List = (CHAR PICHUGE *)pList;

    CtrlDataGetText(ar, IDC_SLLOPT_EDIT_COMMENT, (LPSTR)pList->Data, MAX_COMMENT + 1);
    pList->Type   = P2P_Comment;
    pList->Length = lstrlen((LPSTR)pList->Data);
    if ( pList->Length != 0 )
    {
        pList->Length++;    /* the comment should be stored null-terminated */
        pPicParm->PIC2ListLen += sizeof(P2PktNull) + pList->Length;
        pList = (P2PktGeneric PICHUGE*)( (BYTE PICHUGE*)pList + sizeof(P2PktNull) + pList->Length );
    }

    pRawDataPtr = (P2PktRawData PICHUGE*)pList;
    CtrlDataGetText(ar, IDC_SLLOPT_EDIT_APPFIELD, (LPSTR)pRawDataPtr->RawData, MAX_COMMENT + 1);
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

    CtrlDataGetText(ar, IDC_SLLOPT_EDIT_WATERMARK, (LPSTR)pList->Data, MAX_COMMENT + 1);
    pList->Type   = P2P_Watermark;
    pList->Length = lstrlen((LPSTR)pList->Data);
    if ( pList->Length != 0 )
    {
        pList->Length++;    /* the comment should be stored null-terminated */
        pPicParm->PIC2ListLen += sizeof(P2PktNull) + pList->Length;
        pList = (P2PktGeneric PICHUGE*)( (BYTE PICHUGE*)pList + sizeof(P2PktNull) + pList->Length );
        pPicParm->u.LL.PicFlags |= PF_HaveWatermark;
    }

    CtrlDataGetText(ar, IDC_SLLOPT_EDIT_SCRIPT, (LPSTR)pList->Data, MAX_COMMENT + 1);
    pList->Type   = P2P_Script;
    pList->Length = lstrlen((LPSTR)pList->Data);
    if ( pList->Length != 0 )
    {
        pList->Length++;    /* the comment should be stored null-terminated */
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
    /* note that OP_LIP requires that the Front and Rear pointers be
        reset to Start -- actually it probably requires that there
        always be contiguous space of at least dwStripSize following
        p->Put.Front */
    ////####p->Put.Front = p->Put.Start;
    ////####p->Put.Rear = p->Put.Start;
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
 *  void DefaultsDialogSaveIMStar(void)
 * 
 *  default save options dialog
 * 
 *  notes:
 *      The processing specific to the IMStar save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
void DefaultsDialogSaveIMStar(void)
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
 *      The processing specific to the Save Imstar save options is contained
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
        CtrlDataSetPushed(*ppSaveData, IDC_SLLOPT_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppSaveData, IDC_SLLOPT_CHECK_SETDEFAULT);
        }
    if ( poud != 0 && !MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefIMStar) )
	{	/* source image is not imstar */
        CtrlDataSetPushed(*ppSaveData, IDC_SLLOPT_CHECK_IMAGEASDISPLAYED, TRUE);
        CtrlDataSetInactive(*ppSaveData, IDC_SLLOPT_CHECK_IMAGEASDISPLAYED);
	}

    result = DlgDoModal(hWnd, MAKEINTRESOURCE(IDD_SLLOPT), ImageOnCommand, *ppSaveData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppSaveData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATESAVEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the save options dialog." */
        return ( FALSE );
        }             

    if ( CtrlDataIsPushed(*ppSaveData, IDC_SLLOPT_CHECK_SETDEFAULT) )
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
 *      pUserData   - as defined by SaveSequentialIMstar
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
    char sz[_MAX_PATH + sizeof(" Save IMStar Options")];
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
                lstrcat(sz, " Save IMStar Options");
                SetWindowText(hwndDlg, sz);
                }
            ImageGrayedCtrls(hwndDlg, arSaveData);
            break;

        case IDC_SLLOPT_RADIO_JPEG:
        case IDC_SLLOPT_RADIO_PPMD:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SLLOPT_CHECK_IMAGEASDISPLAYED:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_SLLOPT_CHECK_TRANSPARENT:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SLLOPT);
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
    if ( poud != 0 && !IsDlgButtonChecked(hwndDlg, IDC_SLLOPT_CHECK_IMAGEASDISPLAYED) )
        {
        assert(MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefIMStar));
        return ( TRUE );    /* source image is imstar */
        }

    /* otherwise, it's default options or we're compressing from a dib and have to look
        at the other save options */
    if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_ERRORLIMIT, 0, 7) )
        return ( FALSE );    
    if ( IsDlgButtonChecked(hwndDlg, IDC_SLLOPT_RADIO_PPMD) )
        {
        if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_ORDER, 0, 4) )
            return ( FALSE );
        }

     if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_PAGENUM, 0, 65535L) )
          return ( FALSE );
     if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_TOTALPAGES, 0, 65535L) )
          return ( FALSE );
    if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_TRANSPARENTINDEX, 0, 255) )
        return ( FALSE );
     if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_LEFT, -32768L, 32767) )
          return ( FALSE );
     if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_TOP, -32768L, 32767) )
          return ( FALSE );
     if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_DELAY, 0, 65535L) )
          return ( FALSE );
     if ( !NumberValid(hwndDlg, IDC_SLLOPT_EDIT_DISP, 0, 255) )
        return ( FALSE );        

    return ( TRUE );
}



static BOOL NumberValid(HWND hwnd, int nCtrl, long nMin, long nMax)
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
     BOOL fEnableOptions = IsDlgButtonChecked(hwndDlg, IDC_SLLOPT_CHECK_IMAGEASDISPLAYED);
     BOOL fEnablePPMD    = fEnableOptions && IsDlgButtonChecked(hwndDlg, IDC_SLLOPT_RADIO_PPMD);

     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_CHECK_REVERSED,    fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_CHECK_TRANSPARENT, fEnableOptions);

     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_RADIO_ROTATE0,     fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_RADIO_ROTATE180,   fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_RADIO_ROTATE270,   fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_RADIO_ROTATE90,    fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_RADIO_PPMD,        fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_RADIO_JPEG,        fEnableOptions);

     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_COMMENT,      fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_APPFIELD,     fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_WATERMARK,    fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_SCRIPT,       fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_PASSWORD,     fEnableOptions);

     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_PAGENUM,      fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_TOTALPAGES,   fEnableOptions);
     CtrlEnable(
          hwndDlg,
          ar,
          IDC_SLLOPT_EDIT_TRANSPARENTINDEX,
          fEnableOptions && IsDlgButtonChecked(hwndDlg, IDC_SLLOPT_CHECK_TRANSPARENT));
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_LEFT,         fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_TOP,          fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_DELAY,        fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_DISP,         fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_ERRORLIMIT,   fEnableOptions);

     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_TOTALPAGES, fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_DELAY,      fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_DISP,       fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_IMSTAR,     fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_COMMENT,    fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_APPFIELD,   fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_WATERMARK,  fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_SCRIPT,     fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_VISUALORIENTATION, fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_PASSWORD,   fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_ERRORLIMIT, fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_OTHER,      fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_PAGENUM,    fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_OFFSET,     fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_LEFT,       fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_TOP,        fEnableOptions);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_DELAY2,     fEnableOptions);

     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_EDIT_ORDER,        fEnablePPMD);
     CtrlEnable(hwndDlg, ar, IDC_SLLOPT_STATIC_ORDER,      fEnablePPMD);
}
