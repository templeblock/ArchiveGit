/******************************************************************************
 *
 * MSSEQJPG.C - Minerva MDI sequential JPEG pack module
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * #### TODO: quantization stuff in MMISC.C
 *
 * Global data:
 *     SaveDefSequentialJpeg - opcode definition data to save sequential jpeg image
 *
 * Local functions:
 *   ** call Pegasus to save image **
 *     SaveSequentialJpeg    - all the save sequential jpeg interesting stuff
 *     InitPicParm           - initialize PIC_PARM from default save options settings
 *     PegasusReqInit        - Pegasus(REQ_INIT)
 *     PegasusReqExec        - Pegasus(REQ_EXEC)
 * 
 *  Pegasus(p, REQ_INIT) - handle all the REQ_INIT events
 *
 *   ** save options dialog **
 *     DefaultsDialogSaveSequentialJpeg - default save options dialog
 *     SaveOptionsDialog     - save options dialog for a specific image
 *
 *   ** save options dialog helpers **
 *     ImageOnCommand        - save image options WM_COMMAND handler
 *     DefaultsOnCommand     - save default options WM_COMMAND handler
 *
 *   ** save options OnCommand helpers **
 *     ImageOptionsValid     - validate all the dialog settings on OK or Apply
 *     ImageGrayedCtrls      - enable/gray dialog controls based on current settings
 *     DefaultsOptionsValid  - validate all the dialog settings on OK or Apply
 *     DefaultsGrayedCtrls   - enable/gray dialog controls based on current settings
 *
 * Revision History:
 *   11-26-96           jrb  created
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

#define JFIFOFFSET_APP1COLORPALLETE ( 40 )
    /* 40 is a magic number for the offset into our JFIF image
        where the portion of the App1 field which we've defined
        to hold the color palette data */

/* in a perfect world the following would be referenced from a .h file.  As I've
    done it instead, adding an opcode means creating the opcode implementation
    source file, and then the only other step is in minerva.c to add a pointer
    to the opcode in the appropriate opcode table (with an extern declaration).
    I expect that most of the time, it won't be referenced by another opcode, so
    most of the time the minerva.c dependency is the _only_ one outside of building
    the opcode in the first place.  So that's why this isn't a perfect world. */
extern OPENOP      OpenDefSequentialJpeg;
extern OPENOP      OpenDefEnhancedSequentialJpeg;
extern OPENOP      OpenDefProgressiveJpeg;
extern TRANSFORMOP TransformDefP2S;



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



void InitSaveEnhancedSequentialJpeg(void);
static BOOL SaveSequentialJpeg(
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
static BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA pSaveData);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL PegasusPutNeedSpace(PIC_PARM PICFAR* p);
static void DefaultsDialogSaveSequentialJpeg(void);
static BOOL SaveOptionsDialog(
    HWND            hwnd,
    PFNDLGONCOMMAND pfnOnCommand,
    OPTIONSUSERDATA PICFAR* poud,
    LPCTRLINITDATA PICFAR* ppSaveData);
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static void ImageGrayedCtrls(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    OPTIONSUSERDATA PICFAR* poud);
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static BOOL DefaultsOptionsValid(HWND hwndDlg);
static void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar);
static BOOL DefaultsOnCommand(
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

    { 0, eText, IDC_SSEQJPGTIME_TIMERRES, 0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_INIT,     0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_EXEC,     0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_TERM,     0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_ALLOC,    0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_MEMCPY,   0, sizeof("999.999") },
    { 0, eText, IDC_SSEQJPGTIME_YIELD,    0, sizeof("999,999") },
    { 0, eText, IDC_SSEQJPGTIME_TOTAL,    0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/* ini section for save options */
static char szOptionsSection[] = "Sequential JPEG Pack Options";

/* ini keys for radio button groups in save options */
static char szEntropyCode[]  = "Entropy Coding";
static char szOutputType[]   = "Output Type";
static char szColorPalette[] = "Color Palette";
static char szColorSpace[]   = "Make Palette Color Space";
static char szOrientation[]  = "Orientation-Rotation";

/* specifying save options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Orientation-Inverted",  eCheckBox, IDC_SSEQJPGOPT_CHECK_REVERSED,            FALSE },
    { "CompressAsDisplayed",   eCheckBox, IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED,    FALSE },
    { "HorizontalSubsampling", eCheckBox, IDC_SSEQJPGOPT_CHECK_HSUBSAMPLING,        TRUE  },
    { "VerticalSubsampling",   eCheckBox, IDC_SSEQJPGOPT_CHECK_VSUBSAMPLING,        TRUE  },
    
    { szEntropyCode,  eRadioButton, IDC_SSEQJPGOPT_RADIO_ELS,             TRUE,  0, "Els"      },
    { szEntropyCode,  eRadioButton, IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN,  FALSE, 0, "OptimizedHuffman" },
    { szOutputType,   eRadioButton, IDC_SSEQJPGOPT_RADIO_PIC2,            TRUE,  0, "PIC2"     },
    { szOutputType,   eRadioButton, IDC_SSEQJPGOPT_RADIO_JFIF,            FALSE, 0, "JFIF"     },
    { szColorPalette, eRadioButton, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, TRUE,  0, "Embedded" },
    { szColorPalette, eRadioButton, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE, FALSE, 0, "External" },
    { szColorPalette, eRadioButton, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE,     FALSE, 0, "Make"     },
    { szColorPalette, eRadioButton, IDC_SSEQJPGOPT_RADIO_NOPALETTE,       FALSE, 0, "None"     },
    { szColorSpace,   eRadioButton, IDC_SSEQJPGOPT_RADIO_OPTIMALRGB,      TRUE,  0, "RGB"      },
    { szColorSpace,   eRadioButton, IDC_SSEQJPGOPT_RADIO_OPTIMALYCBCR,    FALSE, 0, "YcbCr"    },
    { szOrientation,  eRadioButton, IDC_SSEQJPGOPT_RADIO_ROTATE0,         TRUE,  0, "0"        },
    { szOrientation,  eRadioButton, IDC_SSEQJPGOPT_RADIO_ROTATE180,       FALSE, 0, "180"      },
    { szOrientation,  eRadioButton, IDC_SSEQJPGOPT_RADIO_ROTATE270,       FALSE, 0, "270"      },
    { szOrientation,  eRadioButton, IDC_SSEQJPGOPT_RADIO_ROTATE90,        FALSE, 0, "90"       },

    { "Comment",                     eEdit, IDC_SSEQJPGOPT_EDIT_COMMENT,         FALSE, MAX_COMMENT },
    { "App Field",                   eEdit, IDC_SSEQJPGOPT_EDIT_APPFIELD,        FALSE, MAX_COMMENT },
    { "Password",                    eEdit, IDC_SSEQJPGOPT_EDIT_PASSWORD,        TRUE,  8 },
    { "Primary Colors",              eEdit, IDC_SSEQJPGOPT_EDIT_PRIMARY,         FALSE, 3, "236" },
    { "Secondary Colors",            eEdit, IDC_SSEQJPGOPT_EDIT_SECONDARY,       FALSE, 2, "16"  },
    { "External Palette Filename",   eEdit, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE, FALSE, _MAX_PATH },
    { "Luminance",                   eEdit, IDC_SSEQJPGOPT_EDIT_LUMINANCE,       FALSE, 3, "24"  },
    { "Chrominance",                 eEdit, IDC_SSEQJPGOPT_EDIT_CHROMINANCE,     FALSE, 3, "30", },
    { "Quantization Filename",       eEdit, IDC_SSEQJPGOPT_EDIT_QUANTIZATION,    FALSE, _MAX_PATH },

    { 0, eCheckBox, IDC_SSEQJPGOPT_CHECK_SETDEFAULT },

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



/* opcode definition data for minerva.c save opcode table pSaveOpTable */
SAVEOP SaveDefSequentialJpeg =
    {
    OP_D2S,
    "Sequential JPEG Compress",
    "Sequential JPEG Defaults",
    "Sequential JPEG (*.pic,*.jpg)|*.PIC;*.JPG|",
    0,
    SaveSequentialJpeg,
    DefaultsDialogSaveSequentialJpeg,
    0
    };



/* opcode definition data for minerva.c save opcode table pSaveOpTable */
SAVEOP SaveDefEnhancedSequentialJpeg =
    {
    OP_D2SE,
    "Enhanced Sequential JPEG Compress",
    0,
    0,
    InitSaveEnhancedSequentialJpeg,
    SaveSequentialJpeg,
    0,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



void InitSaveEnhancedSequentialJpeg(void)
{
    SaveDefEnhancedSequentialJpeg.pszFilter = SaveDefSequentialJpeg.pszFilter;
    SaveDefEnhancedSequentialJpeg.pszDefaultsMenu = SaveDefSequentialJpeg.pszDefaultsMenu;
    SaveDefEnhancedSequentialJpeg.pfnDefaultsDialog = SaveDefSequentialJpeg.pfnDefaultsDialog;
    memset(&SaveDefSequentialJpeg, '\0', sizeof(SaveDefSequentialJpeg));
}



/******************************************************************************
 * BOOL SaveSequentialJpeg(
 *     const BYTE PICHUGE*    pSrc,
 *     LPCSTR                   pszFilename,
 *     DWORD                  dwSrcLen,
 *     const PIC_PARM PICFAR* pSrcPicParm,
 *     const BYTE PICHUGE*    pDib,
 *     DWORD                  dwDibLen,
 *     const PIC_PARM PICFAR* pDibPicParm,
 *     BYTE PICHUGE* PICFAR*  ppSaved,
 *     DWORD PICFAR*          pdwLenSaved)
 * 
 *  all the save sequential jpeg interesting stuff
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
static BOOL SaveSequentialJpeg(
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
    BITMAPINFOHEADER PICFAR* pbmih;
    DWORD dwColorTableOffset;
    DWORD dwColorTableLen;
    WORD PICHUGE* pApp1;
    RGBTRIPLE PICHUGE* pRgbTriple;
    UINT i;
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
    
    if ( !SaveOptionsDialog(hWnd, ImageOnCommand, &oud, &pSaveData) )
        return ( FALSE );

    /**************************************************************************
        initialize PIC_PARM settings
    */

    /* settings independent of source image type and independent of save settings */
    memset(&SaveSrcPicParm, '\0', sizeof(SaveSrcPicParm));
    SaveSrcPicParm.ParmSize     = sizeof(SaveSrcPicParm);
    SaveSrcPicParm.ParmVer      = CURRENT_PARMVER;
    SaveSrcPicParm.ParmVerMinor = 1;
    if ( SaveDefSequentialJpeg.nFoundParmVer == 0 )
        SaveSrcPicParm.Op = OP_D2SE;
    else
        SaveSrcPicParm.Op = OP_D2S;
    memset(&App, 0, sizeof(App));
    SaveSrcPicParm.App = (LPARAM)(APPDATA PICFAR*)&App;

    /* settings dependent on source image type */
    if ( CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED) )
        {
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
            For source image sequential JPEG, we can just take the source image as the
                saved image.
            For source image progressive JPEG, if the Progressive to Sequential
                opcode is available, we can use it.
            For source image a bitmap, we can easily extract the original dib from
                the source image buffer.
            Else we don't yet handle the case becuase we have no other source image types yet.
                #### TODO: However, I think for each open opcode, there needs to be a set of
                control init values which can be used to extract a full-size, full-detail dib.
                Then there can be an opcode table entry point (yech!) or an OpenImage parameter
                setting to use those control init values without interacting with the user.
                Then we compress the dib */
        if ( MiscIsImageType(biCompressionSrc, &OpenDefSequentialJpeg) ||
             MiscIsImageType(biCompressionSrc, &OpenDefEnhancedSequentialJpeg) )
            {
            /* source image is sequential jpeg */
            CtrlDataFree(&pSaveData);
            *ppSaved = (BYTE PICHUGE*)MiscGlobalAllocPtr(dwSrcLen, IDS_SAVEOUTOFMEMORY);
            if ( *ppSaved == 0 )
                return ( FALSE );
            hmemcpy(*ppSaved, pSrc, dwSrcLen);
            *pdwLenSaved = dwSrcLen;
            return ( TRUE );
            }
        else if ( MiscIsImageType(biCompressionSrc, &OpenDefProgressiveJpeg) )
            {
            /* source image is progressive jpeg */
            if ( TransformDefP2S.nFoundParmVer != 0 )
                {
                /* progressive to sequential opcode is available */
                CtrlDataFree(&pSaveData);
                *ppSaved = (BYTE PICHUGE *)pSrc;    /* cast away constness */
                *pdwLenSaved = dwSrcLen;
                return ( (*TransformDefP2S.pfnTransform)(
                        NULL,
                        NULL,
                        ppSaved,
                        pdwLenSaved,
                        (PIC_PARM PICFAR *)pSrcPicParm,
                        pDib,
                        dwDibLen,
                        pDibPicParm) );
                }
            else  /* transform p2s opcode not present */
                {
                /* "The Pegasus Imaging Corp. DLL for loss-less conversion from progressive JPEG "
                    "format to sequential JPEG format is not present.  Do you want to save in "
                    "sequential JPEG format anyway?" */
                if ( ErrorMessage(
                        MB_YESNO | MB_ICONEXCLAMATION,
                        IDS_NOTRANSFORMP2S,
                        (LPCSTR)TransformDefP2S.pszAbout) != IDYES )
                    {
                    CtrlDataFree(&pSaveData);
                    return ( FALSE );
                    }
                }
            /* else fall through to re-expand to dib full size and without color-reduction
                and smoothing, before compressing from dib to jpeg */
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
                "This would re-expand from the PJPEG, IMStar, IM4, GIF, TIF or PCX source "
                    "image to a DIB, then compress to SJPEG format but it's not implemented yet.",
                APPLICATION_NAME,
                MB_OK);
            CtrlDataFree(&pSaveData);
            return ( FALSE );
            }
        }

    /* clear out any remnants from expanding to DIB */
    SaveSrcPicParm.Head.biHeight = labs(SaveSrcPicParm.Head.biHeight);

    /* see if the image to be saved is gray scale
        only if bit depth <= 8 and all color palette colors are gray (red==green==blue) */
    if ( SaveSrcPicParm.Head.biBitCount <= 8 &&
         MiscIsGrayPalette(SaveSrcPicParm.Head.biClrImportant, SaveSrcPicParm.ColorTable) )
        SaveSrcPicParm.u.D2S.PicFlags |= PF_IsGray;

    /*#### TODO: note that -1013 occurs if you point the queue at the bmp header
        so it's sort of a bad data error.  This is probably worth a string resource
        error message
        
        also not to forget -- I've seen a gp in PegasusQuery during my mistake where I
        was overwriting the app1pal colortable 4-bytes too early. */

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
        CtrlDataFree(&pSaveData);
        return ( FALSE );
        }
    if ( bYield )
        SaveSrcPicParm.u.D2S.PicFlags |= PF_YieldGet | PF_YieldPut;

    App.dwTime[TOTAL] -= MiscTickCount();
    /* We could allocate more tightly than the following in guessing how big the
        output buffer needs to be.  Also, it's probably true that there is some
        horribly degenerate case where the output JPEG image is larger than the
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
            SaveSrcPicParm.Get.Front = SaveSrcPicParm.Get.End;
            SaveSrcPicParm.Get.Rear  = SaveSrcPicParm.Get.Start;
            SaveSrcPicParm.Get.QFlags |= Q_EOF | Q_REVERSE; /* reverse for all Dib's */
            }
        else
            {
            /* note get queue length must be at least one strip */
            if ( dwGetQSize > (DWORD)SaveSrcPicParm.u.D2S.StripSize )
                dwSize = dwGetQSize;
            else
                dwSize = SaveSrcPicParm.u.D2S.StripSize;
            App.dwTime[ALLOC] -= MiscTickCount();
            pGetQ = MiscGlobalAllocPtr(dwSize, IDS_SAVEOUTOFMEMORY);
            App.dwTime[ALLOC] += MiscTickCount();
            if ( pGetQ == 0 )
                {
                Pegasus(&SaveSrcPicParm, REQ_TERM);
                MiscGlobalFreePtr(&pAllocSaveSrc);
                CtrlDataFree(&pSaveData);
                return ( FALSE );
                }        
            SaveSrcPicParm.Get.Start = pGetQ;
            SaveSrcPicParm.Get.End   = pGetQ + dwSize;
            SaveSrcPicParm.Get.Front = SaveSrcPicParm.Get.End;
            SaveSrcPicParm.Get.Rear  = SaveSrcPicParm.Get.Front;
            SaveSrcPicParm.Get.QFlags |= Q_REVERSE; /* reverse for all Dib's */
            App.dwGetLen = dwSaveSrcLen;
            App.pGet  = pSaveSrc + dwSaveSrcLen;
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
            dwColorTableLen = SaveSrcPicParm.Head.biClrUsed * sizeof(RGBQUAD);
            if ( SaveSrcPicParm.u.D2S.JpegType == JT_PIC )
                {
                /* thus == JT_PIC */
                pbmfh = (BITMAPFILEHEADER PICFAR*)*ppSaved;
                pbmfh->bfSize = *pdwLenSaved;
                pbmih = (BITMAPINFOHEADER PICFAR*)( *ppSaved + sizeof(BITMAPFILEHEADER) );
                pbmih->biSizeImage = *pdwLenSaved - pbmfh->bfOffBits;
                if ( dwColorTableLen != 0 )
                    {
                    dwColorTableOffset = ( (BYTE PICHUGE*)pbmih + pbmih->biSize ) - *ppSaved;
                    if ( !CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_RADIO_NOPALETTE) )
                        {
                        /* copy the color table into the compressed image */
                        hmemcpy(
                            *ppSaved + dwColorTableOffset,
                            SaveSrcPicParm.ColorTable,
                            dwColorTableLen);
                        }                            
                    }
                }
            else if ( SaveSrcPicParm.u.D2S.JpegType == JT_RAW )
                {
                if ( dwColorTableLen != 0 && ( SaveSrcPicParm.u.D2S.PicFlags & PF_App1Pal ) != 0 )
                    {
                    pApp1 = (WORD PICHUGE*)( *ppSaved + JFIFOFFSET_APP1COLORPALLETE );
                    /* the offset into our JFIF image where the portion of the App1
                         field which we've defined to hold the color palette data */
                    *pApp1++ = (WORD)SaveSrcPicParm.Head.biClrUsed;
                    *pApp1++ = (WORD)SaveSrcPicParm.Head.biClrImportant;
                    pRgbTriple = (RGBTRIPLE PICHUGE*)pApp1;
                    for ( i = 0; i < SaveSrcPicParm.Head.biClrUsed; i++ )
                        pRgbTriple[i] = *(RGBTRIPLE *)&SaveSrcPicParm.ColorTable[i];
                    }
                }                    
            }
        }        

    /* free output buffer, comment, appdata and quantization if present
        from save source DIB PIC_PARM */
    MiscGlobalFreePtr(&SaveSrcPicParm.Comment);
    MiscGlobalFreePtr(&SaveSrcPicParm.u.D2S.AppField);
    MiscGlobalFreePtr(&SaveSrcPicParm.u.D2S.QTable);
    CtrlDataFree(&pSaveData);

    App.dwTime[TOTAL] += MiscTickCount();

    if ( bDisplayTiming )
        MiscDisplayTimingDialog(
            hwndFrame,
            MAKEINTRESOURCE(IDD_SSEQJPGTIME),
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
static BOOL InitPicParm(PIC_PARM *pPicParm, LPCTRLINITDATA pSaveData)
{
    char sz[_MAX_PATH + 1];

    switch ( CtrlDataWhichRadio(pSaveData, IDC_SSEQJPGOPT_RADIO_PIC2) )
        {
        case IDC_SSEQJPGOPT_RADIO_PIC2:
            pPicParm->u.D2S.JpegType = JT_PIC2;
            break;
        case IDC_SSEQJPGOPT_RADIO_JFIF:
            pPicParm->u.D2S.JpegType = JT_RAW;
            break;
        default:
            assert(FALSE);
            break;
        }
       
    CtrlDataGetText(pSaveData, IDC_SSEQJPGOPT_EDIT_PASSWORD, sz, sizeof(sz));
    memset(pPicParm->KeyField, '\0', sizeof(pPicParm->KeyField));
    strncpy((char *)pPicParm->KeyField, sz, sizeof(pPicParm->KeyField));

    /* note that VisualOrient is saved with the image and can be retrieved
        using PegasusQuery or Pegasus(REQ_INIT) when expanding -- but
        it has 0 effect on packing or expanding otherwise */
    switch ( CtrlDataWhichRadio(pSaveData, IDC_SSEQJPGOPT_RADIO_ROTATE0) )
        {
        case IDC_SSEQJPGOPT_RADIO_ROTATE0:
            pPicParm->VisualOrient = O_normal;
            break;
        case IDC_SSEQJPGOPT_RADIO_ROTATE90:
            pPicParm->VisualOrient = O_r90;
            break;
        case IDC_SSEQJPGOPT_RADIO_ROTATE180:
            pPicParm->VisualOrient = O_r180;
            break;
        case IDC_SSEQJPGOPT_RADIO_ROTATE270:
            pPicParm->VisualOrient = O_r270;
            break;
        default:
            assert(FALSE);
            break;
        }
    if ( CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_CHECK_REVERSED) )
        pPicParm->VisualOrient |= O_inverted;

    switch ( CtrlDataWhichRadio(pSaveData, IDC_SSEQJPGOPT_RADIO_NOPALETTE) )
        {
        case IDC_SSEQJPGOPT_RADIO_NOPALETTE:
            pPicParm->Head.biClrUsed = 0;
            pPicParm->Head.biClrImportant = 0;
            /* note in the case where the dib is <= 8 bits, we need the
                color table to compress the image -- but the color table
                itself is already in place.  Setting these to 0 has the
                effect of telling the compressor not to output a color
                table.  For input, it assumes that each index it encounters
                is a valid index into a valid color table.  Thus it doesn't
                use biClrused and biClrImportant on input, it just uses
                them to decide whether or not to output a color table. */
            break;

        case IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE:
            if ( pPicParm->u.D2S.JpegType == JT_RAW )
                pPicParm->u.D2S.PicFlags |= PF_App1Pal;
            break;

        case IDC_SSEQJPGOPT_RADIO_MAKEPALETTE:
            if ( pPicParm->u.D2S.JpegType == JT_RAW )
                pPicParm->u.D2S.PicFlags |= PF_App1Pal;

            pPicParm->u.D2S.PicFlags |= PF_MakeColors;
            pPicParm->u.D2S.PrimClrToMake =
                CtrlDataGetLong(pSaveData, IDC_SSEQJPGOPT_EDIT_PRIMARY, FALSE);
            pPicParm->u.D2S.SecClrToMake =
                CtrlDataGetLong(pSaveData, IDC_SSEQJPGOPT_EDIT_SECONDARY, FALSE);
            if ( CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_RADIO_OPTIMALYCBCR) )
                pPicParm->u.D2S.PicFlags |= PF_UseYCbCrForColors;
            break;

        case IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE:
            if ( pPicParm->u.D2S.JpegType == JT_RAW )
                pPicParm->u.D2S.PicFlags |= PF_App1Pal;

            CtrlDataGetText(pSaveData, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            pPicParm->Head.biClrUsed = MiscPaletteColors(sz, pPicParm->ColorTable, 256);
            pPicParm->Head.biClrImportant = pPicParm->Head.biClrUsed;
            assert(pPicParm->Head.biClrUsed != 0);
            break;

        default:
            assert(FALSE);
            break;
        }

    switch ( CtrlDataWhichRadio(pSaveData, IDC_SSEQJPGOPT_RADIO_ELS) )
        {
        case IDC_SSEQJPGOPT_RADIO_ELS:
            pPicParm->u.D2S.PicFlags |= PF_ElsCoder;
            break;
        case IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN:
            pPicParm->u.D2S.PicFlags |= PF_OptimizeHuff;
            break;
        default:
            assert(FALSE);
            break;
        }

    if ( CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_CHECK_HSUBSAMPLING) )
        {
        if ( CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_CHECK_VSUBSAMPLING) )
            pPicParm->u.D2S.SubSampling = SS_411;
        else
            pPicParm->u.D2S.SubSampling = SS_211;
        }
    else
        {
        if ( CtrlDataIsPushed(pSaveData, IDC_SSEQJPGOPT_CHECK_VSUBSAMPLING) )
            pPicParm->u.D2S.SubSampling = SS_211v;
        else
            pPicParm->u.D2S.SubSampling = SS_111;
        }
        
    pPicParm->u.D2S.LumFactor =
        CtrlDataGetLong(pSaveData, IDC_SSEQJPGOPT_EDIT_LUMINANCE, FALSE);

    pPicParm->u.D2S.ChromFactor =
        CtrlDataGetLong(pSaveData, IDC_SSEQJPGOPT_EDIT_CHROMINANCE, FALSE);
    
    pPicParm->Comment = MiscGlobalAllocPtr(MAX_COMMENT + 1, IDS_SAVEOUTOFMEMORY);
    if ( pPicParm->Comment == 0 )
        return ( FALSE );
    CtrlDataGetText(pSaveData, IDC_SSEQJPGOPT_EDIT_COMMENT, (LPSTR)pPicParm->Comment, MAX_COMMENT + 1);
    pPicParm->CommentLen = lstrlen((LPCSTR)pPicParm->Comment);
    pPicParm->CommentSize = pPicParm->CommentLen;
    if ( pPicParm->CommentLen == 0 )
        MiscGlobalFreePtr(&pPicParm->Comment);

    pPicParm->u.D2S.AppField = (BYTE PICHUGE*)MiscGlobalAllocPtr(MAX_COMMENT + 1, IDS_SAVEOUTOFMEMORY);
    if ( pPicParm->u.D2S.AppField == 0 )
        {
        MiscGlobalFreePtr(&pPicParm->Comment);
        return ( FALSE );
        }
    CtrlDataGetText(
        pSaveData,
        IDC_SSEQJPGOPT_EDIT_APPFIELD,
        (LPSTR)pPicParm->u.D2S.AppField,
        MAX_COMMENT + 1);
    pPicParm->u.D2S.AppFieldLen = lstrlen((LPSTR)pPicParm->u.D2S.AppField);
    pPicParm->u.D2S.AppFieldSize = pPicParm->u.D2S.AppFieldLen;
    if ( pPicParm->u.D2S.AppFieldLen == 0 )
        MiscGlobalFreePtr(&pPicParm->u.D2S.AppField);

    CtrlDataGetText(pSaveData, IDC_SSEQJPGOPT_EDIT_QUANTIZATION, sz, sizeof(sz));
    if ( sz[0] != '\0' )
        {
        pPicParm->u.D2S.QTable = (BYTE PICHUGE *)MiscGlobalAllocPtr(128, IDS_SAVEOUTOFMEMORY);
        if ( pPicParm->u.D2S.QTable == 0 )
            {
            MiscGlobalFreePtr(&pPicParm->Comment);
            MiscGlobalFreePtr(&pPicParm->u.D2S.AppField);
            return ( FALSE );
            }
        if ( !MiscQuantization(sz, pPicParm->u.D2S.QTable) )
            {
            assert(FALSE);
            MiscGlobalFreePtr(&pPicParm->u.D2S.QTable);
            MiscGlobalFreePtr(&pPicParm->Comment);
            MiscGlobalFreePtr(&pPicParm->u.D2S.AppField);
            return ( FALSE );
            }
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
                ErrorMessage(STYLE_ERROR, IDS_SAVEOUTOFMEMORY);
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
 *  void DefaultsDialogSaveSequentialJpeg(void)
 * 
 *  default save options dialog
 * 
 *  notes:
 *      The processing specific to the Save Sequential Jpeg save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static void DefaultsDialogSaveSequentialJpeg(void)
{
    LPCTRLINITDATA pSaveData;
    
    if ( SaveOptionsDialog(hwndFrame, DefaultsOnCommand, NULL, &pSaveData) )
        CtrlDataFree(&pSaveData);
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
 *      The processing specific to the Save Sequential Jpeg save options is contained
 *      in the WM_COMMAND handler OptionsOnCmd.  See also mdlg.c and mdlgctrl.c.
 *
 *      IDOK is returned by DialogDoModal only if the OK button is pressed and some
 *      image options settings have changed since the dialog was opened.  Otherwise,
 *      if the OK button is pushed, then IDOK_UNCHANGED is returned and we know we don't
 *      have to save the settings to the ini file.
 ******************************************************************************/
static BOOL SaveOptionsDialog(
    HWND hWnd,
    PFNDLGONCOMMAND pfnOnCommand,
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
        CtrlDataSetPushed(*ppSaveData, IDC_SSEQJPGOPT_CHECK_SETDEFAULT, TRUE);
        CtrlDataSetInactive(*ppSaveData, IDC_SSEQJPGOPT_CHECK_SETDEFAULT);
        }
    if ( SaveDefEnhancedSequentialJpeg.nFoundParmVer == 0 )
        {
        CtrlDataSetInactive(*ppSaveData, IDC_SSEQJPGOPT_RADIO_ELS);
        if ( CtrlDataIsPushed(*ppSaveData, IDC_SSEQJPGOPT_RADIO_ELS) )
            CtrlDataSetPushed(*ppSaveData, IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN, TRUE);
        }
    
    result = DlgDoModal(hWnd, MAKEINTRESOURCE(IDD_SSEQJPGOPT), pfnOnCommand, *ppSaveData, poud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        CtrlDataFree(ppSaveData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATESAVEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the save options dialog." */
        return ( FALSE );
        }             

    if ( CtrlDataIsPushed(*ppSaveData, IDC_SSEQJPGOPT_CHECK_SETDEFAULT) )
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
    LPCTRLINITDATA  arSaveData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    char sz[_MAX_PATH + sizeof(" Save JPEG Options")];
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
            lstrcpy(sz, poud->pszFilename);
            lstrcat(sz, " Save JPEG Options");
            SetWindowText(hwndDlg, sz);
            ImageGrayedCtrls(hwndDlg, arSaveData, poud);
            break;

        case IDC_SSEQJPGOPT_RADIO_JFIF:
            if ( wNotifyCode == BN_CLICKED )
                {
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_ELS, FALSE);
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN, TRUE);
                ImageGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SSEQJPGOPT_RADIO_PIC2:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_SSEQJPGOPT_RADIO_ELS:
            if ( wNotifyCode == BN_CLICKED )
                {
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_JFIF, FALSE);
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_PIC2, TRUE);
                ImageGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
                            
        case IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SSEQJPGOPT_RADIO_NOPALETTE:
        case IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_SSEQJPGOPT_RADIO_MAKEPALETTE:
        case IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arSaveData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SSEQJPGOPT_BUTTON_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( MiscPaletteBrowse(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;
            
        case IDC_SSEQJPGOPT_BUTTON_QUANTIZATION:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( MiscQuantizationBrowse(hwndDlg, IDC_SSEQJPGOPT_EDIT_QUANTIZATION) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SSEQJPGOPT);
            break;

        case IDOK:
            return ( ImageOptionsValid(hwndDlg, poud) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL DefaultsOnCommand(
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
 *  notes:
 *      as the oncommand handler is called for the dialog and its
 *      children, pszDlgTemplate is used to tell which dialog is active
 ******************************************************************************/
static BOOL DefaultsOnCommand(
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
    NOREFERENCE(pUserData);
    
    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:
            DefaultsGrayedCtrls(hwndDlg, arSaveData);
            break;

        case IDC_SSEQJPGOPT_RADIO_JFIF:
            if ( wNotifyCode == BN_CLICKED )
                {
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_ELS, FALSE);
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN, TRUE);
                DefaultsGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SSEQJPGOPT_RADIO_PIC2:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_SSEQJPGOPT_RADIO_ELS:
            if ( wNotifyCode == BN_CLICKED )
                {
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_JFIF, FALSE);
                CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_PIC2, TRUE);
                DefaultsGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;
                            
        case IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SSEQJPGOPT_RADIO_NOPALETTE:
        case IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_SSEQJPGOPT_RADIO_MAKEPALETTE:
        case IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arSaveData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_SSEQJPGOPT_BUTTON_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( MiscPaletteBrowse(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;
            
        case IDC_SSEQJPGOPT_BUTTON_QUANTIZATION:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( MiscQuantizationBrowse(hwndDlg, IDC_SSEQJPGOPT_EDIT_QUANTIZATION) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_SSEQJPGOPT);
            break;

        case IDOK:
            return ( DefaultsOptionsValid(hwndDlg) );

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
    char sz[_MAX_PATH + 1];
    int nPrimary;
    int nSecondary;
    int swap;
    BOOL fPrimaryValid;
    BOOL fSecondaryValid;
    int n;
    BOOL fValid;
    int nLuminance;
    int nChrominance;

    if ( !IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED) )
        {
        if ( MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefSequentialJpeg) ||
             MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefEnhancedSequentialJpeg) )
            return ( TRUE );    /* source image is sequential jpeg */
        if ( TransformDefP2S.nFoundParmVer != 0 &&
             MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefProgressiveJpeg) )
            return ( TRUE );    /* source image is progressive jpeg and the
                                    progressive to sequential opcode is available */
        }
    /* otherwise, we'll be compressing from a dib and will have to look at the other
        save options */
 
    if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE) )
        {
        assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE));

        nPrimary = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY, &fPrimaryValid, FALSE);
        nSecondary = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY, &fSecondaryValid, FALSE);

        /* make sure that nPrimary >= nSecondary */
        if ( nSecondary > nPrimary )
            {
            swap = nSecondary;
            nSecondary = nPrimary;
            nPrimary = swap;
            SetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY,   nPrimary,   FALSE);
            SetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY, nSecondary, FALSE);
            }

        /* validate primary colors to make */
        if ( !fPrimaryValid || nPrimary < 2 || nPrimary > 256 )
            {
            ErrorMessage(STYLE_ERROR, IDS_PRIMARYCOLORS);
            SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY), 0, -1);
            return ( FALSE );
            }

        /* validate secondary colors to make */
        GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY, sz, sizeof(sz));
        if ( sz[0] != '\0' &&
             nSecondary > 0 &&
             ( !fSecondaryValid ||
               nSecondary < 2 ||
               nSecondary > 16 ||
               nPrimary + nSecondary > 256 ) )
            {
            if ( nPrimary >= 256 - 2 )
                ErrorMessage(STYLE_ERROR, IDS_NOSECONDARYCOLORS);
            else                 
                ErrorMessage(STYLE_ERROR, IDS_SECONDARYCOLORS, max(16, 256 - nPrimary));
            SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY), 0, -1);
            return ( FALSE );
            }        
        }
    else if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE) )
        {
        assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE));

        GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
        n = MiscPaletteColors(sz, 0, 256);
        if ( n == 0 )
            {
            SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE), 0, -1);
            return ( FALSE );
            }
        }

    assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE));
    nLuminance = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE, &fValid, FALSE);
    if ( !fValid || nLuminance > 255 )
        {
        ErrorMessage(STYLE_ERROR, IDS_LUMINANCECHROMINANCE);
        SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE), 0, -1);
        return ( FALSE );
        }        

    assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE));
    nChrominance = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE, &fValid, FALSE);
    if ( !fValid || nChrominance > 255 )
        {
        ErrorMessage(STYLE_ERROR, IDS_LUMINANCECHROMINANCE);
        SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE), 0, -1);
        return ( FALSE );
        }

    //#### TODO: uncomment when quantization is implemented
    //####assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_EDIT_QUANTIZATION));
    GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_EDIT_QUANTIZATION, sz, sizeof(sz));
    if ( sz[0] != '\0' && !MiscQuantization(sz, 0) )
        return ( FALSE );

    return ( TRUE );
}



/******************************************************************************
 *  BOOL DefaultsOptionsValid(HWND hwndDlg)
 *
 *  validate all the dialog settings on OK
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL DefaultsOptionsValid(HWND hwndDlg)
{
    char sz[_MAX_PATH + 1];
    int nPrimary;
    int nSecondary;
    int swap;
    BOOL fPrimaryValid;
    BOOL fSecondaryValid;
    int n;
    BOOL fValid;
    int nLuminance;
    int nChrominance;

    if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE) )
        {
        assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE));

        nPrimary = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY, &fPrimaryValid, FALSE);
        nSecondary = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY, &fSecondaryValid, FALSE);

        /* make sure that nPrimary >= nSecondary */
        if ( nSecondary > nPrimary )
            {
            swap = nSecondary;
            nSecondary = nPrimary;
            nPrimary = swap;
            SetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY,   nPrimary,   FALSE);
            SetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY, nSecondary, FALSE);
            }

        /* validate primary colors to make */
        if ( !fPrimaryValid || nPrimary < 2 || nPrimary > 256 )
            {
            ErrorMessage(STYLE_ERROR, IDS_PRIMARYCOLORS);
            SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_PRIMARY), 0, -1);
            return ( FALSE );
            }

        /* validate secondary colors to make */
        GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY, sz, sizeof(sz));
        if ( sz[0] != '\0' &&
             nSecondary > 0 &&
             ( !fSecondaryValid ||
               nSecondary < 2 ||
               nSecondary > 16 ||
               nPrimary + nSecondary > 256 ) )
            {
            if ( nPrimary >= 256 - 2 )
                ErrorMessage(STYLE_ERROR, IDS_NOSECONDARYCOLORS);
            else                 
                ErrorMessage(STYLE_ERROR, IDS_SECONDARYCOLORS, max(16, 256 - nPrimary));
            SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_SECONDARY), 0, -1);
            return ( FALSE );
            }        
        }
    else if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE) )
        {
        assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE));

        GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
        n = MiscPaletteColors(sz, 0, 256);
        if ( n == 0 )
            {
            SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE), 0, -1);
            return ( FALSE );
            }
        }

    assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE));
    nLuminance = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE, &fValid, FALSE);
    if ( !fValid || nLuminance > 255 )
        {
        ErrorMessage(STYLE_ERROR, IDS_LUMINANCECHROMINANCE);
        SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_LUMINANCE), 0, -1);
        return ( FALSE );
        }        

    assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE));
    nChrominance = GetDlgItemInt(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE, &fValid, FALSE);
    if ( !fValid || nChrominance > 255 )
        {
        ErrorMessage(STYLE_ERROR, IDS_LUMINANCECHROMINANCE);
        SetFocus(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_SSEQJPGOPT_EDIT_CHROMINANCE), 0, -1);
        return ( FALSE );
        }

    //#### TODO: uncomment when quantization is implemented
    //####assert(CtrlIsEnabled(hwndDlg, IDC_SSEQJPGOPT_EDIT_QUANTIZATION));
    GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_EDIT_QUANTIZATION, sz, sizeof(sz));
    if ( sz[0] != '\0' && !MiscQuantization(sz, 0) )
        return ( FALSE );

    return ( TRUE );
}



/******************************************************************************
 *  void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA *poud)
 *
 *  enable/gray dialog controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 *      poud    - pointer to src/dib image data structure
 ******************************************************************************/
static void ImageGrayedCtrls(
    HWND hwndDlg,
    LPCTRLINITDATA ar,
    OPTIONSUSERDATA PICFAR* poud)
{
    char sz[64];
    BOOL fEnableOptions   = TRUE;
    BOOL fIsGray          = FALSE;
    BOOL fMakePalette     = IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE);
    BOOL fExternalPalette = IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE);
    BOOL fEmbeddedPaletteOK;
    BOOL fAsDisplayed;
    DWORD nPrimary;
    DWORD nSecondary;
    int n;

    if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED) )
        {
        /* compress from image as displayed */
        n = GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_STATIC_IMAGEEMBEDDED, sz, sizeof(sz));
        assert(n < sizeof(sz) - 1);
        SetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, sz);
        fEmbeddedPaletteOK = ( poud->pDibPicParm->Head.biClrUsed != 0 );

        fEnableOptions = TRUE;
        if ( poud->pDibPicParm->Head.biBitCount <= 8 )
            fIsGray = MiscIsGrayPalette(
                poud->pDibPicParm->Head.biClrImportant,
                poud->pDibPicParm->ColorTable);
        /* else fIsGray FALSE from its initialization */
        fAsDisplayed = TRUE;
        }
    else
        {
        fAsDisplayed = FALSE;
        /* else compressing from source image */
        n = GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_STATIC_SOURCEEMBEDDED, sz, sizeof(sz));
        assert(n < sizeof(sz) - 1);
        SetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, sz);
        fEmbeddedPaletteOK = ( poud->pSrcPicParm->Head.biClrUsed != 0 );

        /* decide whether the source image is gray scale */
        if ( poud->pSrcPicParm->Head.biBitCount <= 8 )
            {
            nPrimary = poud->pSrcPicParm->Head.biClrImportant;
            nSecondary = poud->pSrcPicParm->Head.biClrUsed - nPrimary;
            if ( nPrimary > (DWORD)( 1 << poud->pSrcPicParm->Head.biBitCount ) )
                fIsGray = MiscIsGrayPalette(nPrimary, poud->pSrcPicParm->ColorTable);
            else
                {
                assert(nSecondary <= (DWORD)( 1 << poud->pSrcPicParm->Head.biBitCount ));
                fIsGray = MiscIsGrayPalette(
                    nSecondary,
                    &poud->pSrcPicParm->ColorTable[(UINT)nPrimary]);
                }
            }

        /* no compress options if the source image is sequential jpeg and none if
            progressive jpeg and the progressive to sequential jpeg opcode is available */
        if ( MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefSequentialJpeg) ||
             MiscIsImageType(poud->pSrcPicParm->Head.biCompression, &OpenDefEnhancedSequentialJpeg) )
            fEnableOptions = FALSE;
        else if ( MiscIsImageType(
                    poud->pSrcPicParm->Head.biCompression,
                    &OpenDefProgressiveJpeg) &&
                  TransformDefP2S.nFoundParmVer != 0 )
            fEnableOptions = FALSE;
        else
            fEnableOptions = TRUE;
        }

    /* disable or enable _all_ other save options (other than the image as displayed
        checkbox) as appropriate */
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_OUTPUTFORMAT,      fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_PIC2,               fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_JFIF,               fEnableOptions);

    /* els-coding only supports pic2 output */
    if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_ELS) )
        {
        CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_JFIF, FALSE);
        CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_PIC2, TRUE);
        }

    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_PASSWORD,          fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_PASSWORD,            fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_VISUALORIENTATION, fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_ROTATE0,            fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_ROTATE90,           fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_ROTATE180,          fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_ROTATE270,          fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_CHECK_REVERSED,           fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_COLORPALETTE,      fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_NOPALETTE,          fEnableOptions);

    /* only enable make palette if compressing && not gray */
    if ( fEnableOptions && !fIsGray )
        CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE, TRUE);
    else
        {
        /* if disabled, make sure it's not pushed */
        CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE, FALSE);
        if ( fEnableOptions && IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE) )
            {
            CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE, FALSE);
            CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_NOPALETTE, TRUE);
            ImageGrayedCtrls(hwndDlg, ar, poud);
            return;
            }
        }

    /* only enable embedded palette if there is one */
    if ( fEnableOptions &&
         fEmbeddedPaletteOK &&
         ( !fIsGray || !IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_JFIF) ) )
        CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, TRUE);
    else        
        {
        /* if disabled, make sure it's not pushed */
        CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, FALSE);

        if ( fEnableOptions && IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE) )
            {
            CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, FALSE);
            CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_NOPALETTE, TRUE);
            ImageGrayedCtrls(hwndDlg, ar, poud);
            return;
            }
        }
    
    /* don't allow external palette if the compressed image needs the palette in
        PIC_PARM.Head */
    if ( fEnableOptions && ( !fAsDisplayed || poud->pDibPicParm->Head.biBitCount > 8 ) )
        CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE, TRUE);
    else
        {
        /* if disabled, make sure it's not pushed */
        CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE, FALSE);
        if ( fEnableOptions && IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE) )
            {
            CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE, FALSE);
            CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_NOPALETTE, TRUE);
            ImageGrayedCtrls(hwndDlg, ar, poud);
            return;
            }
        }
        
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_JPEG,              fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_ENTROPYCODING,     fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_ELS,                fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_OPTIMUMHUFFMAN,     fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_SUBSAMPLING,       fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_CHECK_HSUBSAMPLING,       fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_CHECK_VSUBSAMPLING,       fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_LUMINANCE,         fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_LUMINANCE,           fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_CHROMINANCE,       fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_CHROMINANCE,         fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_QUANTIZATION,      fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_QUANTIZATION,        fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_BUTTON_QUANTIZATION,      fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_COMMENT,           fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_COMMENT,             fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_APPFIELD,          fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_APPFIELD,            fEnableOptions);

    CtrlEnable(hwndDlg, ar, IDC_BUTTON_TIMING,                       fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_CHECK_SETDEFAULT,         fEnableOptions);

    /* gray makepalette options unless make palette is set */
    fMakePalette     = ( fMakePalette && fEnableOptions );
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_OPTIMALRGB,       fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_OPTIMALYCBCR,     fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_PRIMARY,         fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_PRIMARY,           fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_PRIMARYCOLORS,   fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_SECONDARY,       fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_SECONDARY,         fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_SECONDARYCOLORS, fMakePalette);
    
    /* gray external palette options unless external palette is set */
    fExternalPalette = ( fExternalPalette && fEnableOptions );
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_EXTERNALPALETTE, fExternalPalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE,   fExternalPalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_BUTTON_EXTERNALPALETTE, fExternalPalette);
}



/******************************************************************************
 *  void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar)
 *
 *  enable/gray dialog controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 ******************************************************************************/
static void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar)
{
    char sz[64];
    int n;
    BOOL fEnableOptions   = TRUE;
    BOOL fMakePalette     = IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_MAKEPALETTE);
    BOOL fExternalPalette = IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_EXTERNALPALETTE);

    if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_CHECK_IMAGEASDISPLAYED) )
        {
        /* compress from image as displayed */
        n = GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_STATIC_IMAGEEMBEDDED, sz, sizeof(sz));
        assert(n < sizeof(sz) - 1);
        SetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, sz);
        }
    else
        {
        /* else compressing from source image */
        n = GetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_STATIC_SOURCEEMBEDDED, sz, sizeof(sz));
        assert(n < sizeof(sz) - 1);
        SetDlgItemText(hwndDlg, IDC_SSEQJPGOPT_RADIO_EMBEDDEDPALETTE, sz);
        }

    /* els-coding only supports pic2 output */
    if ( IsDlgButtonChecked(hwndDlg, IDC_SSEQJPGOPT_RADIO_ELS) )
        {
        CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_JFIF, FALSE);
        CheckDlgButton(hwndDlg, IDC_SSEQJPGOPT_RADIO_PIC2, TRUE);
        }

    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_OUTPUTFORMAT,    fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_PIC2,             fEnableOptions);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_JFIF,             fEnableOptions);

    /* gray makepalette options unless make palette is set */
    fMakePalette     = ( fMakePalette && fEnableOptions );
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_OPTIMALRGB,       fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_RADIO_OPTIMALYCBCR,     fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_PRIMARY,         fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_PRIMARY,           fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_PRIMARYCOLORS,   fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_SECONDARY,       fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_SECONDARY,         fMakePalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_SECONDARYCOLORS, fMakePalette);
    
    /* gray external palette options unless external palette is set */
    fExternalPalette = ( fExternalPalette && fEnableOptions );
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_STATIC_EXTERNALPALETTE, fExternalPalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_EDIT_EXTERNALPALETTE,   fExternalPalette);
    CtrlEnable(hwndDlg, ar, IDC_SSEQJPGOPT_BUTTON_EXTERNALPALETTE, fExternalPalette);
}
