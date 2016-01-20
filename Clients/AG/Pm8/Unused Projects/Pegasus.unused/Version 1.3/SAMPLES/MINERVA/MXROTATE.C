/******************************************************************************
 *
 * MXROTATE.C - Minerva MDI rotate opcode
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
#include <memory.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(default:4115 4201 4214)

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



#define REALLOCEXTRA    ( 50000L )

/* in a perfect world the following would be referenced from a .h file.  As I've
    done it instead, adding an opcode means creating the opcode implementation
    source file, and then the only other step is in minerva.c to add a pointer
    to the opcode in the appropriate opcode table (with an extern declaration).
    I expect that most of the time, it won't be referenced by another opcode, so
    most of the time the minerva.c dependency is the _only_ one outside of building
    the opcode in the first place.  So that's why this isn't a perfect world. */
extern OPENOP      OpenDefSequentialJpeg;
extern OPENOP      OpenDefEnhancedSequentialJpeg;



/******************************************************************************
 *  external functions and data referenced
 ******************************************************************************/



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void TransformInitRotateEnhanced(void);
static void TransformInitRotate(void);
static BOOL TransformRotate(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrc,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm);
static void TransformRotateDefaultsDialog(void);
static BOOL RotateOptionsDialog(
    HWND hwnd,
    PFNDLGONCOMMAND pfnOptionsOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppRorData);
static BOOL OnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arRorData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL OptionsValid(HWND hwndDlg);
static BOOL RotateImage(
    HWND                  hWnd,
    BYTE PICHUGE*         pSrc,
    DWORD                 dwSrcLen,
    BYTE PICHUGE* PICFAR* pRotate,
    DWORD PICFAR*         pdwRotateLen,
    LPCTRLINITDATA        pRorData,
	const BYTE PICFAR*	  pszKeyField);
static BOOL InitPicParm(PIC_PARM PICFAR* pPicParm, LPCTRLINITDATA ar);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL PegasusPutNeedSpace(PIC_PARM PICFAR* p);



/******************************************************************************
 *  local data
 ******************************************************************************/



#define REQINIT  0
#define REQEXEC  1
#define REQTERM  2
#define ALLOC    3
#define MEMCPY   4
#define YIELD    5
#define TOTALOP  6
#define PAINT    7
#define TOTAL    8



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { 0, eText, IDC_XRORTIME_TIMERRES,    0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_INIT,        0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_EXEC,        0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_TERM,        0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_ALLOC,       0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_MEMCPY,      0, sizeof("999,999") },
    { 0, eText, IDC_XRORTIME_YIELD,       0, sizeof("999,999") },
    { 0, eText, IDC_XRORTIME_OPTOTAL,     0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_PAINT,       0, sizeof("999.999") },
    { 0, eText, IDC_XRORTIME_TOTAL,       0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/* ini section for image options */
static char szOptionsSection[] = "Rotate Options";

static char szFormat[]      = "Format"; /* ini file key for Output Format radio buttons */
static char szRotation[]    = "Rotation"; /* ini file key for Output Rotation radio buttons */
static char szPadTruncate[] = "Pad or Truncate"; /* ini file key for pad/truncate radio buttons */
static char szEntropyCode[] = "Entropy Coding"; /* ini file key for Entropy Coding radio buttons */
static char szEnhancement[] = "CompressionEnhancement"; /* ini file key for Compression Enhancement radio buttons */

/* specifying rotate options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Inverted Vertically",   eCheckBox, IDC_XROROPT_CHECK_REVERSED,        FALSE },
    { "Remove Comments",       eCheckBox, IDC_XROROPT_CHECK_REMOVECOMMENTS,  FALSE },
    { "Remove App Data",       eCheckBox, IDC_XROROPT_CHECK_REMOVEAPPDATA, FALSE },
    { "Remove Color Palette",  eCheckBox, IDC_XROROPT_CHECK_REMOVEPALETTE,   FALSE },
    
    { szFormat,      eRadioButton, IDC_XROROPT_RADIO_PIC2,           TRUE,  0, "PIC2" },
    { szFormat,      eRadioButton, IDC_XROROPT_RADIO_JFIF,           FALSE, 0, "JFIF" },
    { szRotation,    eRadioButton, IDC_XROROPT_RADIO_ROTATE0,        TRUE,  0, "0"        },
    { szRotation,    eRadioButton, IDC_XROROPT_RADIO_ROTATE180,      FALSE, 0, "180"      },
    { szRotation,    eRadioButton, IDC_XROROPT_RADIO_ROTATE270,      FALSE, 0, "270"      },
    { szRotation,    eRadioButton, IDC_XROROPT_RADIO_ROTATE90,       FALSE, 0, "90"       },
    { szPadTruncate, eRadioButton, IDC_XROROPT_RADIO_PADMCU,         FALSE, 0, "Pad" },
    { szPadTruncate, eRadioButton, IDC_XROROPT_RADIO_TRUNCATEMCU,    TRUE,  0, "Truncate" },
    { szEntropyCode, eRadioButton, IDC_XROROPT_RADIO_ELS,            TRUE,  0, "Els"      },
    { szEntropyCode, eRadioButton, IDC_XROROPT_RADIO_OPTIMUMHUFFMAN, FALSE, 0, "OptimizedHuffman" },
    { szEnhancement, eRadioButton, IDC_XROROPT_RADIO_NOREQUANT,      TRUE,  0, "None" },
    { szEnhancement, eRadioButton, IDC_XROROPT_RADIO_NICEREQUANT,    FALSE, 0, "Graceful" },
    { szEnhancement, eRadioButton, IDC_XROROPT_RADIO_RISKYREQUANT,   FALSE, 0, "Risky" },

    { "Output Password", eEdit, IDC_XROROPT_EDIT_OUTPUTPASSWORD, TRUE,  8 },
    { "Luminance",       eEdit, IDC_XROROPT_EDIT_LUMINANCE,      FALSE, 3, "24"  },
    { "Chrominance",     eEdit, IDC_XROROPT_EDIT_CHROMINANCE,    FALSE, 3, "30", },
    { "Comment",         eEdit, IDC_XROROPT_EDIT_COMMENT,        FALSE, MAX_COMMENT },

    { 0, eGroupBox,   IDC_XROROPT_STATIC_JPEGOPTIONS        },
    { 0, eGroupBox,   IDC_XROROPT_STATIC_ENTROPYCODING      },
    { 0, eGroupBox,   IDC_XROROPT_STATIC_REQUANT            },
    { 0, eText,       IDC_XROROPT_STATIC_LUMINANCE          },
    { 0, eText,       IDC_XROROPT_STATIC_CHROMINANCE        },
    { 0, eText,       IDC_XROROPT_STATIC_OUTPUTPASSWORD     },
    { 0, eText,       IDC_XROROPT_STATIC_INPUTFILESIZE,        0,  sizeof("9,999,999.999") },
    { 0, eText,       IDC_XROROPT_STATIC_OUTPUTFILESIZE,       0,  sizeof("9,999,999.999") },
    { 0, eText,       IDC_XROROPT_STATIC_INPUTFILESIZELEGEND  },
    { 0, eText,       IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND },

    { 0, ePushButton, IDTEST },
    { 0, eCheckBox,   IDC_XROROPT_CHECK_SETDEFAULT },

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



TRANSFORMOP TransformDefRotate =
    {
    OP_ROR,
    "Rotate Sequential JPEG",
    "&Rotate Defaults",
    "&Rotate",
    TransformInitRotate,
    TransformRotate,
    TransformRotateDefaultsDialog,
    0
    };



TRANSFORMOP TransformDefRotateEnhanced =
    {
    OP_RORE,
    "Rotate/Enhance Sequential JPEG",
    "&Rotate/Enhance Defaults",
    "&Rotate/Enhance",
    TransformInitRotateEnhanced,
    TransformRotate,
    TransformRotateDefaultsDialog,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void TransformInitRotate()
 * 
 *  if OP_RORE isn't present, clear out the TRANSFORMOP data structure so it
 *( doesn't show up anywhere
 *
 ******************************************************************************/
static void TransformInitRotate(void)
{
    if ( TransformDefRotateEnhanced.nFoundParmVer == 0 )
        _fmemset(&TransformDefRotateEnhanced, 0, sizeof(TransformDefRotateEnhanced));
}



/******************************************************************************
 *  void TransformInitRotateEnhanced()
 * 
 *  clear out the TransformDefRotate (OP_ROR) TRANSFORMOP data structure so it
 *( doesn't show up anywhere -- it's all replaced by this OP_RORE
 *
 ******************************************************************************/
static void TransformInitRotateEnhanced(void)
{
    _fmemset(&TransformDefRotate, 0, sizeof(TransformDefRotate));
}



/******************************************************************************
 * BOOL TransformRotate(
 *     BYTE PICHUGE* PICFAR*  ppSrc,
 *     DWORD PICFAR*          pdwSrcLen,
 *     PIC_PARM PICFAR*       pSrcPicParm,
 *     BYTE PICHUGE* PICFAR*  pDib,
 *     DWORD PICFAR*          pdwDibLen,
 *     PIC_PARM PICFAR*       pDibPicParm)
 * 
 *  all the rotate transformation interesting stuff
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
static BOOL TransformRotate(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    BYTE PICHUGE* PICFAR*  ppSrc,
    DWORD PICFAR*          pdwSrcLen,
    PIC_PARM PICFAR*       pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm)
{
    LPCTRLINITDATA pRorData;
    OPTIONSUSERDATA oud;
    int result;
    BYTE PICHUGE *pSrc = 0;
    DWORD dwSrcLen = *pdwSrcLen;
    PIC_PARM SrcPicParm = *pSrcPicParm;
    BYTE PICHUGE *pRotated;
    DWORD dwRotatedLen;

    if ( !MiscIsImageType(pSrcPicParm->Head.biCompression, &OpenDefSequentialJpeg) &&
         !MiscIsImageType(pSrcPicParm->Head.biCompression, &OpenDefEnhancedSequentialJpeg) )
        {
        ErrorMessage(STYLE_ERROR, IDS_RORNOTJPEG);
        return ( FALSE );        
        }         
    pSrc = MiscGlobalAllocPtr(dwSrcLen, IDS_ROROUTOFMEMORY);
    if ( pSrc == 0 )
        return ( FALSE );
    hmemcpy(pSrc, *ppSrc, dwSrcLen);

    /**************************************************************************
        rotate options dialog
    */
    oud.hWnd        = hWnd;
    oud.ppSrc       = &pSrc;
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = &SrcPicParm;
    oud.ppDib       = &(BYTE PICHUGE *)pDib;            /* cast away const */
    oud.pdwDibLen   = &dwDibLen;
    oud.pDibPicParm = (PIC_PARM PICFAR*)pDibPicParm;    /* cast away const */
    oud.bTested     = FALSE;
    
    result = RotateOptionsDialog(hWnd, OnCommand, &oud, &pRorData);
    if ( result == IDOK )
        {
        /* there have been changes since the test button was last pushed */
        if ( RotateImage(hWnd, pSrc, dwSrcLen, &pRotated, &dwRotatedLen, pRorData, pDibPicParm->KeyField) )
            {
            MiscGlobalFreePtr(&pSrc);
            result = IDOK_UNCHANGED;
            }
        else
            {
            if ( !OpenImage(hWnd, pszFilename, pSrc, dwSrcLen, OP_SILENT | OP_INISETTINGS) )
                {
                DestroyWindow(hWnd);
                MiscGlobalFreePtr(&pSrc);
                assert(FALSE);
                }
            }
        CtrlDataFree(&pRorData);
        return ( TRUE );
        }

    if ( result == IDOK_UNCHANGED )
        {
        MiscGlobalFreePtr(&pSrc);
        CtrlDataFree(&pRorData);
        return ( TRUE );       
        }

    /* else result == IDCANCEL || result == -1 */

    if ( oud.bTested )
        {
        /* cancel any changes and return to the original image */
        if ( !OpenImage(hWnd, pszFilename, pSrc, dwSrcLen, OP_SILENT) )
            {
            DestroyWindow(hWnd);
            MiscGlobalFreePtr(&pSrc);
            }
        }
    else
        MiscGlobalFreePtr(&pSrc);
    CtrlDataFree(&pRorData);
    return ( FALSE );
}



static BOOL RotateImage(
    HWND                  hWnd,
    BYTE PICHUGE*         pSrc,
    DWORD                 dwSrcLen,
    BYTE PICHUGE* PICFAR* ppRotate,
    DWORD PICFAR*         pdwRotateLen,
    LPCTRLINITDATA        pRorData,
	const BYTE PICFAR*	  pszKeyField)
{
    PIC_PARM pp;
    RESPONSE response;
    BOOL  bDisplayTiming;
    BOOL  bYield;
    DWORD dwGetQSize;
    DWORD dwPutQSize;
    LPCTRLINITDATA pCtrl;
    BYTE PICHUGE *pGetQ = 0;
    BYTE PICHUGE *pPutQ = 0;
    APPDATA App;
    DWORD dwRotateSize;
    BYTE PICHUGE *p;

    memset(&App, 0, sizeof(App));
    App.dwTime[TOTAL] -= MiscTickCount();

    /**************************************************************************
        initialize PIC_PARM settings
    */

    /* settings independent of source image type and independent of save settings */
    memset(&pp, '\0', sizeof(pp));
    pp.ParmSize     = sizeof(pp);
    pp.ParmVer      = CURRENT_PARMVER;
    pp.ParmVerMinor = 1;
    pp.Op           = OP_ROR;
    if ( TransformDefRotateEnhanced.nFoundParmVer != 0 )
        pp.Op = OP_RORE;
        
    pp.App = (LPARAM)(APPDATA PICFAR*)&App;

    pCtrl = CtrlData(pRorData, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);
    bYield = 
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);
    dwGetQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_INPUTK, FALSE) * 1024;
    dwPutQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_OUTPUTK, FALSE) * 1024;

    /* rotate options-specific initialization of the pic_parm */
    if ( !InitPicParm(&pp, pRorData) )
        return ( FALSE );
	hmemcpy(pp.KeyField, pszKeyField, sizeof(pp.KeyField));
    
    if ( bYield )
        pp.u.ZOOM.PicFlags |= PF_YieldGet | PF_YieldPut;

    if ( dwGetQSize == 0 || dwGetQSize >= dwSrcLen )
        {
        pp.Get.Start = (BYTE PICHUGE *)pSrc;    /* cast away const */
        pp.Get.End   = pp.Get.Start + dwSrcLen;
        pp.Get.Front = pp.Get.Start;
        pp.Get.Rear  = pp.Get.End;
        pp.Get.QFlags |= Q_EOF;
        }
    else
        {
        App.dwTime[ALLOC] -= MiscTickCount();
        pGetQ = MiscGlobalAllocPtr(dwGetQSize, IDS_ROROUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pGetQ == 0 )
            return ( FALSE );
        pp.Get.Start = pGetQ;            
        pp.Get.End   = pp.Get.Start + dwGetQSize;
        pp.Get.Front = pp.Get.Start;
        pp.Get.Rear  = pp.Get.Front;
        App.pGet     = (BYTE PICHUGE *)pSrc;
        App.dwGetLen = dwSrcLen;
        }

    /* We could allocate more tightly than the following in guessing how big the
        output buffer needs to be.  Also, it's probably true that there is some
        horribly degenerate case where the output JPEG image is larger than the
        input JPEG so we still have to handle reallocating the output buffer
        even in the following simplistic case */
    /* note there must be a put buffer before req_init. if not then it will immediately
        RES_PUT_NEED_SPACE so we might as well do it here */
    *pdwRotateLen = 0;
    App.dwTime[ALLOC] -= MiscTickCount();
    dwRotateSize = dwSrcLen + min(4096, dwSrcLen / 4);

    *ppRotate = MiscGlobalAllocPtr(dwRotateSize, IDS_ROROUTOFMEMORY);
    App.dwTime[ALLOC] += MiscTickCount();
        /* "There was not enough memory to rotate the image." */
    if ( *ppRotate == 0 )
        {
        MiscGlobalFreePtr(&pGetQ);
        MiscGlobalFreePtr(&pp.Comment);
        return ( FALSE );
        }

    if ( dwPutQSize == 0 || dwPutQSize >= dwRotateSize )
        {
        pp.Put.Start = *ppRotate;
        pp.Put.End = pp.Put.Start + dwRotateSize;
        }
    else
        {
        // note that minimium put buffer size is 1024 bytes, as we are only accepting
        // K, it would be a wierd error to be less than 1024 here
        assert(dwPutQSize >= 1024);

        App.dwTime[ALLOC] -= MiscTickCount();
        pPutQ = MiscGlobalAllocPtr(dwPutQSize, IDS_ROROUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pPutQ == 0 )
            {
            MiscGlobalFreePtr(pGetQ);
            MiscGlobalFreePtr(&pp.Comment);
            return ( FALSE );
            }
        pp.Put.Start = pPutQ;
        pp.Put.End = pp.Put.Start + dwPutQSize;
        App.dwPutSize = dwRotateSize;
        App.dwPutLen  = 0;
        App.pPut      = *ppRotate;
        App.pPut0     = App.pPut;
        }        
    pp.Put.Front = pp.Put.Start;
    pp.Put.Rear  = pp.Put.Front;
    pp.Put.QFlags = 0;

    /**************************************************************************
        Pegasus()
    */
    response = PegasusReqInit(&pp);
    if ( App.pPut0 != 0 && App.pPut0 != *ppRotate )
        *ppRotate = App.pPut0;
    if ( response != RES_DONE )
        {
        assert(response == RES_ERR);
        MiscGlobalFreePtr(&pGetQ);
        MiscGlobalFreePtr(&pPutQ);
        MiscGlobalFreePtr(ppRotate);
        MiscGlobalFreePtr(&pp.Comment);
        ErrorPegasusMessage(pp.Status, IDS_RORIMAGE);
        return ( FALSE );
        }

    response = PegasusReqExec(&pp);
    if ( App.pPut0 != 0 && App.pPut0 != *ppRotate )
        *ppRotate = App.pPut0;
    if ( response != RES_DONE )
        {
        assert(response == RES_ERR);
        MiscGlobalFreePtr(&pGetQ);
        MiscGlobalFreePtr(&pPutQ);
        MiscGlobalFreePtr(ppRotate);
        MiscGlobalFreePtr(&pp.Comment);
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
    MiscGlobalFreePtr(&pp.Comment);
    App.dwTime[ALLOC] += MiscTickCount();

    if ( App.pPut != 0 )
        *pdwRotateLen = App.dwPutLen;
    else
        *pdwRotateLen += pp.Put.Rear - pp.Put.Front;
    App.dwTime[ALLOC] -= MiscTickCount();
    p = GlobalReAllocPtr(*ppRotate, *pdwRotateLen, GMEM_MOVEABLE);
    App.dwTime[ALLOC] += MiscTickCount();
    if ( p == 0 )
        {
        MiscGlobalFreePtr(ppRotate);
        ErrorMessage(STYLE_ERROR, IDS_ROROUTOFMEMORY);
        return ( FALSE );
        }
    *ppRotate = p;

    App.dwTime[TOTALOP] = App.dwTime[TOTAL];
    App.dwTime[TOTALOP] += MiscTickCount();

    /* image was successfully transformed */
    if ( hWnd != NULL )
        {
        CHILDINSTANCEDATA PICFAR* p = ChildGetInstanceData(hWnd);

        App.dwTime[PAINT] -= MiscTickCount();
        if ( !OpenImage(hWnd, p->pszFilename, *ppRotate, *pdwRotateLen, OP_SILENT | OP_INISETTINGS) )
            return ( FALSE );
        App.dwTime[PAINT] += MiscTickCount();
        }
    App.dwTime[TOTAL] += MiscTickCount();

    if ( bDisplayTiming )
        if ( TransformDefRotateEnhanced.nFoundParmVer != 0 )
            MiscDisplayTimingDialog(
                hWnd,
                MAKEINTRESOURCE(IDD_XRORETIME),
                App.dwTime,
                InitDisplayTiming);
        else
            MiscDisplayTimingDialog(
                hWnd,
                MAKEINTRESOURCE(IDD_XRORTIME),
                App.dwTime,
                InitDisplayTiming);
    return ( TRUE );
}



/******************************************************************************
 *  BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA ar)
 * 
 *  initialize PIC_PARM from default rotate options settings
 *
 *  parameters:
 *      pPicParm    - PIC_PARM to init
 *      ar          - default rotate options settings array
 ******************************************************************************/
static BOOL InitPicParm(PIC_PARM PICFAR* pPicParm, LPCTRLINITDATA ar)
{
    char sz[_MAX_PATH + 1];

    switch ( CtrlDataWhichRadio(ar, IDC_XROROPT_RADIO_PIC2) )
        {
        case IDC_XROROPT_RADIO_PIC2:
            pPicParm->u.ROR.JpegType = JT_PIC2;
            break;
        case IDC_XROROPT_RADIO_JFIF:
            pPicParm->u.ROR.JpegType = JT_RAW;
            break;
        default:
            assert(FALSE);
            break;
        }
    
    switch ( CtrlDataWhichRadio(ar, IDC_XROROPT_RADIO_ROTATE0) )
        {
        case IDC_XROROPT_RADIO_ROTATE0:
            pPicParm->VisualOrient = O_normal;
            break;
        case IDC_XROROPT_RADIO_ROTATE90:
            pPicParm->VisualOrient = O_r90;
            break;
        case IDC_XROROPT_RADIO_ROTATE180:
            pPicParm->VisualOrient = O_r180;
            break;
        case IDC_XROROPT_RADIO_ROTATE270:
            pPicParm->VisualOrient = O_r270;
            break;
        default:
            assert(FALSE);
            break;
        }
    if ( CtrlDataIsPushed(ar, IDC_XROROPT_CHECK_REVERSED) )
        pPicParm->VisualOrient |= O_inverted;
    
    switch ( CtrlDataWhichRadio(ar, IDC_XROROPT_RADIO_PADMCU) )
        {
        case IDC_XROROPT_RADIO_PADMCU:
            pPicParm->u.ROR.Pad = 1;
            break;
        case IDC_XROROPT_RADIO_TRUNCATEMCU:
            pPicParm->u.ROR.Pad = 0;
            break;
        default:
            assert(FALSE);
            break;
        }

    switch ( CtrlDataWhichRadio(ar, IDC_XROROPT_RADIO_ELS) )
        {
        case IDC_XROROPT_RADIO_ELS:
            pPicParm->u.ROR.PicFlags |= PF_ElsCoder;
            break;
        case IDC_XROROPT_RADIO_OPTIMUMHUFFMAN:
            pPicParm->u.ROR.PicFlags |= PF_OptimizeHuff;
            break;
        default:
            assert(FALSE);
            break;
        }

    switch ( CtrlDataWhichRadio(ar, IDC_XROROPT_RADIO_NOREQUANT) )
        {
        case IDC_XROROPT_RADIO_NOREQUANT:
            pPicParm->u.ROR.Requantize = 0;
            break;
        case IDC_XROROPT_RADIO_NICEREQUANT:
            pPicParm->u.ROR.Requantize = 1;
            break;
        case IDC_XROROPT_RADIO_RISKYREQUANT:
            pPicParm->u.ROR.Requantize = 2;
            break;
        default:
            assert(FALSE);
            break;
        }

    pPicParm->Comment = MiscGlobalAllocPtr(MAX_COMMENT + 1, IDS_ROROUTOFMEMORY);
    if ( pPicParm->Comment == 0 )
        return ( FALSE );
    CtrlDataGetText(ar, IDC_XROROPT_EDIT_COMMENT, (LPSTR)pPicParm->Comment, MAX_COMMENT + 1);
    pPicParm->CommentLen = lstrlen((LPCSTR)pPicParm->Comment);
    pPicParm->CommentSize = pPicParm->CommentLen;
    if ( pPicParm->CommentLen == 0 )
        MiscGlobalFreePtr(&pPicParm->Comment);

    if ( CtrlDataIsPushed(ar, IDC_XROROPT_CHECK_REMOVECOMMENTS) )
        pPicParm->u.ROR.RemoveComments = 1;     /* 3 so "Pegasus Imaging Corp" comment isn't added */
    if ( !CtrlDataIsPushed(ar, IDC_XROROPT_CHECK_REMOVEAPPDATA) )
        pPicParm->u.ROR.AppsToKeep = 0xffff;    /* keep any and all app markers (app0 - appF) */
    /* else pPicParm->u.AppsToKeep = 0x20000 is "pure" jfif (pic app1 is removed)
        and AppsToKeep = 0x30000 even removes jfif app0 marker */
    if ( !CtrlDataIsPushed(ar, IDC_XROROPT_CHECK_REMOVEPALETTE) )
        pPicParm->u.ROR.KeepColors = 1;
        
    pPicParm->u.ROR.LumFactorReq =
        CtrlDataGetLong(ar, IDC_XROROPT_EDIT_LUMINANCE, FALSE);
    pPicParm->u.ROR.ChromFactorReq =
        CtrlDataGetLong(ar, IDC_XROROPT_EDIT_CHROMINANCE, FALSE);

    CtrlDataGetText(ar, IDC_XROROPT_EDIT_OUTPUTPASSWORD, sz, sizeof(sz));
    _fmemset(pPicParm->u.ROR.OutputKeyField, '\0', sizeof(pPicParm->u.ROR.OutputKeyField));
    lstrcpy((LPSTR)pPicParm->u.ROR.OutputKeyField, sz);
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
                        MiscCopyToQueue(&pApp->pGet, &pApp->dwGetLen, &p->Get, p->u.D2S.StripSize);
                        pApp->dwTime[MEMCPY] += MiscTickCount();
                        }
                    }
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
            p->Put.Rear - p->Put.Front + dwLen + REALLOCEXTRA,
            IDS_ROROUTOFMEMORY);
        pApp->dwTime[ALLOC] += MiscTickCount();
        if ( pApp->pPut == 0 )
            {
            Pegasus(p, REQ_TERM);
            p->Status = ERR_NONE;
            return ( FALSE );
            }
        pApp->pPut0 = pApp->pPut;
        pApp->dwPutSize = p->Put.Rear - p->Put.Front + dwLen + REALLOCEXTRA;
        pApp->dwPutLen = 0;
        }
    else
        {
        if ( dwLen > pApp->dwPutSize )
            {
            /* unexpectedly have to reallocate the output buffer */
            pApp->dwTime[ALLOC] -= MiscTickCount();
            pRealloc = GlobalReAllocPtr(pApp->pPut0, pApp->dwPutLen + dwLen + REALLOCEXTRA, GMEM_MOVEABLE);
            pApp->dwTime[ALLOC] += MiscTickCount();
            if ( pRealloc == 0 )
                {
                Pegasus(p, REQ_TERM);
                ErrorMessage(STYLE_ERROR, IDS_ROROUTOFMEMORY);
                p->Status = ERR_NONE;
                return ( FALSE );
                }
			pApp->pPut = ( pApp->pPut - pApp->pPut0 ) + pRealloc;
            pApp->pPut0 = pRealloc;
            pApp->dwPutSize = dwLen + REALLOCEXTRA;
            }
        }
    pApp->dwTime[MEMCPY] -= MiscTickCount();
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
                hmemcpy(
                    pApp->pPut0 + p->SeekInfo,
                    p->Put.FrontEnd,
                    p->Put.RearEnd - p->Put.RearEnd);
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
                        MiscCopyToQueue(&pApp->pGet, &pApp->dwGetLen, &p->Get, p->u.D2S.StripSize);
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
 *  void TransformRotateDefaultsDialog(void)
 * 
 *  default rotate options dialog
 * 
 *  notes:
 *      The processing specific to the rotate transform options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static void TransformRotateDefaultsDialog(void)
{
    LPCTRLINITDATA pRorData;
    
    int result = RotateOptionsDialog(hwndFrame, OnCommand, NULL, &pRorData);
    if ( result == IDOK || result == IDOK_UNCHANGED )
        CtrlDataFree(&pRorData);
}



/******************************************************************************
 *  BOOL RotateOptionsDialog(HWND hwnd, OPTIONSUSERDATA PICFAR* poud, LPCTRLINITDATA PICFAR* ppRorData)
 * 
 *  Rotate options dialog for default options and for a specific image
 *
 *  parameters:
 *      pfnOptionsOnCommand - WM_COMMAND handler for dialog
 *      poud                - pointers to source and dib image data (see mopcodes.h)
 *      ppRorData           - receives pointer to Rotate options settings
 * 
 *  notes:
 *      The processing specific to the Rotate transform options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static int RotateOptionsDialog(
    HWND hwnd,
    PFNDLGONCOMMAND pfnOptionsOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppRorData)
{
    int result;

    /* read default settings from the ini file -- apply InitOptions settings
        where ini settings aren't present */
    if ( !CtrlDataDup(InitOptionsData, ppRorData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_ROROPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the Rotate options dialog." */
        return ( -1 );
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppRorData);
    if ( poud == 0 )
        {
        CtrlDataSetPushed(*ppRorData, IDC_XROROPT_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_CHECK_SETDEFAULT);
        CtrlDataSetInactive(*ppRorData, IDTEST);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_INPUTFILESIZE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_INPUTFILESIZELEGEND);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND);
        }

    if ( TransformDefRotateEnhanced.nFoundParmVer == 0 )
        {
        CtrlDataSetPushed(*ppRorData, IDC_XROROPT_RADIO_JFIF, TRUE);
        CtrlDataSetPushed(*ppRorData, IDC_XROROPT_RADIO_OPTIMUMHUFFMAN, TRUE);

        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_RADIO_PIC2);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_JPEGOPTIONS);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_ENTROPYCODING);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_REQUANT);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_LUMINANCE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_CHROMINANCE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_STATIC_OUTPUTPASSWORD);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_RADIO_ELS);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_RADIO_OPTIMUMHUFFMAN);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_RADIO_NOREQUANT);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_RADIO_NICEREQUANT);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_RADIO_RISKYREQUANT);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_EDIT_LUMINANCE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_EDIT_CHROMINANCE);
        CtrlDataSetInactive(*ppRorData, IDC_XROROPT_EDIT_OUTPUTPASSWORD);
        }

    result = DlgDoModal(hwnd, MAKEINTRESOURCE(IDD_XROROPT), pfnOptionsOnCommand, *ppRorData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppRorData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEROROPTIONSDIALOG);
            /* "An unexpected error occurred opening the Rotate options dialog." */
        return ( result );
        }             

    if ( CtrlDataIsPushed(*ppRorData, IDC_XROROPT_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppRorData);
        
    return ( result );
}



/******************************************************************************
 *  BOOL OnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arRorData,
 *      LPCSTR          pszDlgTemplate, 
 *      void PICFAR*    pUserData)
 *
 *  rotate default options WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arRorData   - rotate options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData      - 0
 *
 *  returns:
 *      all but IDOK: FALSE if the command isn't handled, else TRUE
 *      IDOK: FALSE if dialog data is invalid (don't end dialog), else TRUE
 *
 *  notes:
 *      as the oncommand handler is called for the dialog and its
 *      children, pszDlgTemplate is used to tell which dialog is active
 ******************************************************************************/
static BOOL OnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arRorData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData)
{
    char sz[_MAX_PATH + sizeof(" Rotate/Enhance Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */
    BYTE PICHUGE *pRotated;
    DWORD dwRotatedLen;

    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:
            /* WM_INITDIALOG - make sure the ini defaults are reasonable (valid
                numbers != 0 */
            if ( pUserData != 0 )
                DlgSetModified(hwndDlg);
            if ( poud != 0 )
                {
                GetWindowText(poud->hWnd, sz, sizeof(sz) - ( sizeof(" Rotate/Enhance Options") - 1 ));
                if ( TransformDefRotateEnhanced.nFoundParmVer != 0 )
                    strcat(sz, " Rotate/Enhance Options");
                else
                    strcat(sz, " Rotate Options");
                SetWindowText(hwndDlg, sz);
                SetDlgItemText(
                    hwndDlg,
                    IDC_XROROPT_STATIC_INPUTFILESIZE,
                    MiscUltoaWithCommas(*poud->pdwSrcLen, sz, sizeof(sz)));
                SetDlgItemText(
                    hwndDlg,
                    IDC_XROROPT_STATIC_OUTPUTFILESIZE,
                    MiscUltoaWithCommas(*poud->pdwSrcLen, sz, sizeof(sz)));
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, FALSE);
                }
            break;

        case IDC_XROROPT_RADIO_JFIF:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( IsDlgButtonChecked(hwndDlg, IDC_XROROPT_RADIO_ELS) )
                    {
                    CheckDlgButton(hwndDlg, IDC_XROROPT_RADIO_ELS, FALSE);
                    CheckDlgButton(hwndDlg, IDC_XROROPT_RADIO_OPTIMUMHUFFMAN, TRUE);
                    }
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTPASSWORD, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_EDIT_OUTPUTPASSWORD, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, FALSE);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_XROROPT_RADIO_PIC2:
            if ( wNotifyCode == BN_CLICKED )
                {
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTPASSWORD, TRUE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_EDIT_OUTPUTPASSWORD, TRUE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, FALSE);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_XROROPT_RADIO_ELS:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( IsDlgButtonChecked(hwndDlg, IDC_XROROPT_RADIO_JFIF) )
                    {
                    CheckDlgButton(hwndDlg, IDC_XROROPT_RADIO_JFIF, FALSE);
                    CheckDlgButton(hwndDlg, IDC_XROROPT_RADIO_PIC2, TRUE);
                    }
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, FALSE);
                DlgSetModified(hwndDlg);
                }
            break;

                
        case IDC_XROROPT_RADIO_ROTATE0:
        case IDC_XROROPT_RADIO_ROTATE180:
        case IDC_XROROPT_RADIO_ROTATE270:
        case IDC_XROROPT_RADIO_ROTATE90:
        case IDC_XROROPT_RADIO_PADMCU:
        case IDC_XROROPT_RADIO_TRUNCATEMCU:
        case IDC_XROROPT_RADIO_OPTIMUMHUFFMAN:
        case IDC_XROROPT_RADIO_NOREQUANT:
        case IDC_XROROPT_RADIO_NICEREQUANT:
        case IDC_XROROPT_RADIO_RISKYREQUANT:
        case IDC_XROROPT_CHECK_REVERSED:
        case IDC_XROROPT_CHECK_REMOVECOMMENTS:
        case IDC_XROROPT_CHECK_REMOVEAPPDATA:
        case IDC_XROROPT_CHECK_REMOVEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, FALSE);
                DlgSetModified(hwndDlg);
                }
            break;
    
        case IDC_XROROPT_EDIT_OUTPUTPASSWORD:
        case IDC_XROROPT_EDIT_LUMINANCE:
        case IDC_XROROPT_EDIT_CHROMINANCE:
            if ( wNotifyCode == EN_CHANGE )
                {
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, FALSE);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_XROROPT);
            break;

        case IDOK:
            return ( OptionsValid(hwndDlg) );

        case IDTEST:
            if ( OptionsValid(hwndDlg) )
                {
                DlgOnOK(hwndDlg);
                /* save the old dib and picparm in case an expand error occurs */
                CtrlEnable(hwndDlg, arRorData, IDOK,     FALSE);
                CtrlEnable(hwndDlg, arRorData, IDCANCEL, FALSE);
                CtrlEnable(hwndDlg, arRorData, IDTEST,   FALSE);
                EnableWindow(hwndDlg, FALSE);
                if ( RotateImage(
                        poud->hWnd,
                        *poud->ppSrc,
                        *poud->pdwSrcLen,
                        &pRotated,
                        &dwRotatedLen,
                        arRorData,
						poud->pDibPicParm->KeyField) )
                    {
                    DlgClearModified(hwndDlg);
                    poud->bTested = TRUE;
                    SetDlgItemText(
                        hwndDlg,
                        IDC_XROROPT_STATIC_OUTPUTFILESIZE,
                        MiscUltoaWithCommas(dwRotatedLen, sz, sizeof(sz)));
                    CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZELEGEND, TRUE);
                    CtrlEnable(hwndDlg, arRorData, IDC_XROROPT_STATIC_OUTPUTFILESIZE, TRUE);
                    }                        
                EnableWindow(hwndDlg, TRUE);
                CtrlEnable(hwndDlg, arRorData, IDOK,     TRUE);
                CtrlEnable(hwndDlg, arRorData, IDCANCEL, TRUE);
                CtrlEnable(hwndDlg, arRorData, IDTEST,   TRUE);
                }
            break;

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL OptionsValid(HWND hwndDlg)
 *
 *  validate all the dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL OptionsValid(HWND hwndDlg)
{
    BOOL fValid;
    int nLuminance;
    int nChrominance;

    if ( !CtrlIsEnabled(hwndDlg, IDC_XROROPT_EDIT_LUMINANCE) )
        {
        assert(!CtrlIsEnabled(hwndDlg, IDC_XROROPT_EDIT_CHROMINANCE));
        return ( TRUE );
        }

    nLuminance = GetDlgItemInt(hwndDlg, IDC_XROROPT_EDIT_LUMINANCE, &fValid, FALSE);
    if ( !fValid || nLuminance > 255 )
        {
        ErrorMessage(STYLE_ERROR, IDS_LUMINANCECHROMINANCE);
        SetFocus(GetDlgItem(hwndDlg, IDC_XROROPT_EDIT_LUMINANCE));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_XROROPT_EDIT_LUMINANCE), 0, -1);
        return ( FALSE );
        }

    assert(CtrlIsEnabled(hwndDlg, IDC_XROROPT_EDIT_CHROMINANCE));
    nChrominance = GetDlgItemInt(hwndDlg, IDC_XROROPT_EDIT_CHROMINANCE, &fValid, FALSE);
    if ( !fValid || nChrominance > 255 )
        {
        ErrorMessage(STYLE_ERROR, IDS_LUMINANCECHROMINANCE);
        SetFocus(GetDlgItem(hwndDlg, IDC_XROROPT_EDIT_CHROMINANCE));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_XROROPT_EDIT_CHROMINANCE), 0, -1);
        return ( FALSE );
        }
    return ( TRUE );
}
