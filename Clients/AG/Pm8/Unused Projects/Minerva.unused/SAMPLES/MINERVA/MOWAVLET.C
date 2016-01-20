/******************************************************************************
 *
 * MOWavlet.C - Minerva MDI Wavelet expand
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
 *   ** call Pegasus to open image **
 *     ExpandImageInWindow       - all the open image interesting stuff
 *     PegasusReqInit            - Pegasus(p, REQ_INIT)
 *     PegasusReqExec            - Pegasus(p, REQ_EXEC)
 *
 *   ** opcode def functions **
 *     OpenUtility - expand image to dib
 *     OpenInitUtility - replace default open BMP open opcode table entry with this one
 *     ExtractDibOpenUtility - extract full-size, full-detail dib with no interaction
 *     OptionsDialogOpenUtility - image options dialog 
 *     DefaultsDialogOpenUtility - default image options dialog
 *     PropertiesDialogOpenUtility - image properties dialog
 *     CleanupOpenUtility - image cleanup (dib, settings, picparm)
 *     DupPicParmOpenUtility - duplicate picparm for new window
 *
 *   ** misc **
 *     GetCtrlDefaults           - read default image options settings from ini file
 *
 *   ** properties helpers **
 *     PropertiesOnCommand       - properties WM_COMMAND handler
 *
 *   ** image options dialog helpers **
 *     ImageOnCommand            - image options WM_COMMAND handler
 *     DefaultsOnCommand         - default options WM_COMMAND handler
 *
 *   ** options dialog OnCommand helpers **
 *     ImageOptionsValid         - validate all the dialog settings on OK or Apply
 *     DefaultOptionsValid       - validate all the dialog settings on OK or Apply
 *     ImageGrayedCtrls          - enable/gray dialog controls based on current settings
 *     DefaultsGrayedCtrls       - enable/gray dialog controls based on current settings
 *     EnableColorPalette        - enable the controls in the color palette group box
 *     SetGraysToMake            - set grays to make if current setting is invalid
 *     SetColorsToMake           - set colors to make if current setting is invalid
 *     SetNumToMake              - helper for SetGraysToMake and SetColorsToMake -- set
 *                                 control integer value if the present value is invalid
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
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>



#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "pic2file.h"
#include "errors.h"

#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "mdlg.h"
#include "merror.h"
#include "minerva.h" 
#include "mmisc.h"
#include "mchild.h"



/******************************************************************************
 *  external functions and data referenced
 ******************************************************************************/


extern BOOL Op82BitDepth(
    DWORD dwBits,
    BOOL  bDither,
    BYTE PICHUGE* pDib,           DWORD         dwDibSize,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibSize, PIC_PARM PICFAR* pXDibPicParm);
    
extern BOOL Op82Grayscale(
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    BYTE PICHUGE* pDib,           DWORD         dwDibSize,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibSize, PIC_PARM PICFAR* pXDibPicParm);

extern BOOL Op82MakePalette(
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
     BYTE PICHUGE* pDib,           DWORD         dwDibSize,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibSize, PIC_PARM PICFAR* pXDibPicParm);

extern BOOL Op82ExternalPalette(
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    RGBQUAD *pColorTable,
    BYTE PICHUGE* pDib,           DWORD         dwDibSize,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibSize, PIC_PARM PICFAR* pXDibPicParm);

extern BOOL Op82CreatePalette(
    DWORD dwNumColors,
    BYTE PICHUGE* pDib, DWORD dwDibSize, PIC_PARM PICFAR* pDibPicParm);

extern TOOLSOP ToolDefUtility;

extern SAVEOP SaveDefWavelet;





/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL OpenWavelet(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL ExtractDibOpenWavelet(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm);
static BOOL OptionsDialogOpenWavelet(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static void PropertiesDialogOpenWavelet(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);

static void DefaultsDialogOpenWavelet(void);
static void CleanupOpenWavelet(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData);

static BOOL ExpandWaveletInWindow(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR*           pOpenData);
static BOOL ExpandImageInWindow(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR*           pOpenData);
static BOOL PropertiesOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL GetCtrlDefaults(LPCSTR pszSection, LPCTRLINITDATA PICFAR* ppOpenData);
static BOOL DefaultsOnCommand(
    HWND            hwndDlg,
     int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL DefaultOptionsValid(HWND hwndDlg);
static void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar);
static void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA PICFAR* poud);
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
     LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static void EnableColorPalette(HWND hwndDlg, LPCTRLINITDATA ar, BOOL bEnabled);
static void SetGraysToMake(HWND hwndDlg);
static void SetColorsToMake(HWND hwndDlg);
static void SetNumToMake(HWND hwndDlg, int nID, int nNum);



/******************************************************************************
 *  local data
 ******************************************************************************/



/* ini section for image options */
static char szOptionsSection[] = "Wavelet Expand Options";

static char szBitsPerPixel[] = "Bits/Pixel";    /* ini key for bits/pixel radio buttons */
static char szColorPalette[] = "Color Palette"; /* ini key for color palette radio buttons */

/* specifying image options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
     n/a  nested  dialog id    n/a   dialog button      dialog init data
    */
    { 0, ePushButton, IDC_BUTTON_TIMING, FALSE, IDD_TIMING  },
    { szOptionsSection, eDialog, IDD_TIMING, FALSE, IDC_BUTTON_TIMING, (LPSTR)InitTimingData },

    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Make Gray",             eCheckBox, IDC_OWAVEOPT_CHECK_MAKEGRAY,            FALSE },
    { "Dither",                eCheckBox, IDC_OWAVEOPT_CHECK_DITHER,              TRUE  },
    { "Image Settings",	       eCheckBox, IDC_OWAVEOPT_CHECK_IMAGESETTINGS,       TRUE  },

    { szBitsPerPixel, eRadioButton, IDC_OWAVEOPT_RADIO_32BITS,          FALSE, 0, "32"   },
    { szBitsPerPixel, eRadioButton, IDC_OWAVEOPT_RADIO_24BITS,          TRUE,  0, "24"   },
    { szBitsPerPixel, eRadioButton, IDC_OWAVEOPT_RADIO_16BITS,          FALSE, 0, "16"   },
    { szBitsPerPixel, eRadioButton, IDC_OWAVEOPT_RADIO_8BITS,           FALSE, 0, "8"    },
    { szBitsPerPixel, eRadioButton, IDC_OWAVEOPT_RADIO_4BITS,           FALSE, 0, "4"    },
    { szBitsPerPixel, eRadioButton, IDC_OWAVEOPT_RADIO_1BITS,           FALSE, 0, "1"    },
    { szColorPalette, eRadioButton, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, TRUE,  0, "Embedded" },
    { szColorPalette, eRadioButton, IDC_OWAVEOPT_RADIO_MAKEPALETTE,     FALSE, 0, "Make",    },
    { szColorPalette, eRadioButton, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE, FALSE, 0, "External" },

    { "External Palette Filename",   eEdit, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, FALSE, _MAX_PATH },
    { "Make Gray Grays To Make",     eEdit, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE,     FALSE, 3, "256" },
    { "Make Palette Colors To Make", eEdit, IDC_OWAVEOPT_EDIT_COLORSTOMAKE,    FALSE, 3, "256" },
    { "Password",                    eEdit, IDC_OWAVEOPT_EDIT_PASSWORD,        TRUE,  8 },

    { 0, ePushButton, IDC_OWAVEOPT_BUTTON_BROWSE          },
    { 0, ePushButton, IDTEST                            },
    { 0, eGroupBox,   IDC_OWAVEOPT_STATIC_COLORPALETTE    },
    { 0, eText,       IDC_OWAVEOPT_STATIC_COLORS          },
    { 0, eText,       IDC_OWAVEOPT_STATIC_EXTERNALPALETTE },
    { 0, eText,       IDC_OWAVEOPT_STATIC_GRAYS           },
    { 0, eCheckBox,   IDC_OWAVEOPT_CHECK_SETDEFAULT       },

    { 0, eEnd, -1 }
    };
 


#define REQINIT         0
#define REQEXEC         1
#define REQTERM         2
#define ALLOC           3
#define MEMCPY          4
#define YIELD           5
#define OPTOTAL         6
#define BITDEPTH        7
#define PALETTE         8
#define DISPLAYPALETTE  9
#define PAINT           10
#define TOTAL           11



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bInvisible, wLimitText, pszText or pszIniValue
    */
    { 0, eText, IDC_OWAVETIME_TIMERRES,       0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_INIT,           0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_EXEC,           0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_TERM,           0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_ALLOC,          0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_MEMCPY,         0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_YIELD,          0, sizeof("999,999") },
    { 0, eText, IDC_OWAVETIME_OPTOTAL,        0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_BITDEPTH,       0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_PALETTE,        0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_DISPLAYPALETTE, 0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_PAINT,          0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_TOTAL,          0, sizeof("999.999") },
    { 0, eText, IDC_OWAVETIME_STATIC_PALETTE, 0, 0, 0 },   
    { 0, eEnd, -1 }
    };
#define INDEX_STATIC_PALETTE    ( 13 )



/* specifying image properties controls' parameters */
static CTRLINITDATA InitPropertiesData[] =
    {
    { 0, eText, IDC_WAVEPROP_FILENAME,                  0, _MAX_PATH },
    { 0, eText, IDC_WAVEPROP_FILESIZE,                  0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_IMAGEWIDTH_SOURCE,         0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_IMAGEHEIGHT_SOURCE,        0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_BITSPERPIXEL_SOURCE,       0, 2 },
    { 0, eText, IDC_WAVEPROP_IMAGESIZE_SOURCE,          0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_COLORTABLESIZE_SOURCE,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_NUMBERIMPORTANT_SOURCE,    0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_IMAGEWIDTH_DISPLAYED,      0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_IMAGEHEIGHT_DISPLAYED,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_BITSPERPIXEL_DISPLAYED,    0, 2 },
    { 0, eText, IDC_WAVEPROP_IMAGESIZE_DISPLAYED,       0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_COLORTABLESIZE_DISPLAYED,  0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_NUMBERIMPORTANT_DISPLAYED, 0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_PICVERSION,                0, sizeof("4,000,000,000") },
    { 0, eText, IDC_WAVEPROP_VISUALORIENTATION,         0, sizeof("270* clockwise, then inverted") },
    { 0, eText, IDC_WAVEPROP_COMPRESSIONRATIO, 0, sizeof("4,000,000,000:1") },
    { 0, eEdit, IDC_WAVEPROP_COMMENT,          FALSE, MAX_COMMENT },
    { 0, eEdit, IDC_WAVEPROP_APP,              FALSE, MAX_COMMENT },

    { 0, eEnd, -1 }
    };



/******************************************************************************
 *  global data
 ******************************************************************************/



static DWORD arTypes[]  = { BI_WAVE, (DWORD)-1 };

OPENOP OpenDefWavelet =
    {
    OP_W2D,
    "Wavelet Expand",
    "Wavelet Defaults",
    "Wavelet (*.pic)|*.PIC|",
    arTypes,
    0,
    OpenWavelet,
    ExtractDibOpenWavelet,
    OptionsDialogOpenWavelet,
    PropertiesDialogOpenWavelet,
    DefaultsDialogOpenWavelet,
    CleanupOpenWavelet,
    0,
    &SaveDefWavelet,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL ExpandWaveletInWindow(
 *      HWND                   hWnd,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      BYTE PICHUGE* PICFAR*  ppDib,
 *      DWORD PICFAR*          pdwDibLen,
 *      PIC_PARM PICFAR*       pDibPicParm,
 *      VOID PICFAR*           pOpenData)
 * 
 *  all the open sequential Wavelet interesting stuff
 *
 *  parameters:
 *      hWnd        - handle of child window to display image
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      ppDib       - receives pointer to dib
 *      pdwDibLen   - receives length of dib
 *      pDibPicParm - receives picparm for dib
 *      pOpenData   - pointer to image settings array
 *
 *  returns:
 *      FALSE if some error occurs
 *
 *  notes:
 *      currently warns the user on the second, etc. comment and second, etc.
 *      app2 field.  Only the first of each is displayed in properties
 ******************************************************************************/
static BOOL ExpandWaveletInWindow(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR*           pOpenData)
{
	char sz[_MAX_PATH + 1];
    LPCTRLINITDATA ar = (LPCTRLINITDATA)pOpenData;
    RESPONSE response;
    PIC_PARM PICFAR* pp = pDibPicParm;
    PIC_PARM SavePicParm;
    APPDATA PICFAR* pApp;
    APPDATA SaveApp;
    BOOL  bYield;
    LPCTRLINITDATA pCtrl;
    DWORD dw;
    DWORD dwX;
    DWORD dwRowSize;
    
    NOREFERENCE(pSrcPicParm);
    // make sure the window knows it has nothing valid to present yet
    *ppDib = 0;
    *pdwDibLen = 0;

    /**************************************************************************
        Initialize PIC_PARM data
    */

    /* initialize the pic_parm elements independent of image options settings */
    pApp = (APPDATA PICFAR *)pp->App;
    _fmemset(pp, 0, sizeof(*pp));     /* _VERY_ important */
    pp->ParmSize     = sizeof(*pp);
    pp->ParmVer      = CURRENT_PARMVER;  /* from PIC.H */
    pp->Op = OP_W2D;
    pp->ParmVerMinor = 2;
    pp->App = (LPARAM)(APPDATA PICFAR*)pApp;

    CtrlDataGetText(ar, IDC_OWAVEOPT_EDIT_PASSWORD, sz, sizeof(sz));
    _fmemset(pp->KeyField, '\0', sizeof(pp->KeyField));
    hmemcpy(pp->KeyField, sz, sizeof(pp->KeyField));

    pp->PIC2ListSize = -1;  /* signalling that we want PIC2List items returned */
    
    pCtrl = CtrlData(ar, IDD_TIMING);
    assert(pCtrl != 0 );
    bYield = CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);
    if ( bYield )
        pp->u.WAVE.PicFlags |= PF_YieldGet | PF_YieldPut;
                                                         
    /* initialize source image queue pointers */
    pApp->dwTime[TOTAL] -= MiscTickCount();
    pp->Get.Start  = (BYTE PICHUGE*)pSrc;
        /* cast away constness to eliminate the warning --
            note however that this is not safe in general with Pegasus, or as a general
            practice, (although it is currently safe with Wavelet expand). */
    pp->Get.End    = pp->Get.Start + dwSrcLen;
    pp->Get.Front  = pp->Get.Start;
    pp->Get.Rear   = pp->Get.End;
    pp->Get.QFlags = Q_EOF;    /* no more to read, the entire image is in the buffer */

    /**************************************************************************
        Pegasus(REQ_INIT)
    */

    /* Save the picparm before REQ_INIT.  If a password error occurs, REQ_INIT has already
        made changes to the picparm so that we can't pass that one back to REQ_INIT to restart
        with a new password. */
    SavePicParm = *pp;
    /* keep trying until the user enters the correct password or escapes out of the
        dialog to get the password */
    /* stop timing total in case a password is required */
    /* we'll add back in the time for the ReqInit call */
    dwX = -(LONG)MiscTickCount();
    pApp->dwTime[TOTAL] += -(LONG)dwX;
    SaveApp = *pApp;
    while ( ( response = PegasusReqInit(pp) ) == RES_ERR )
        {
        CleanupOpenWavelet(0, pp, 0);
        if ( pp->Status == ERR_MISSING_KEY || pp->Status == ERR_INVALID_KEY )
            {
            char sz[9];
            
            memset(sz, 0, sizeof(sz));
            if ( !MiscGetPassword(hWnd, (LPSTR)SavePicParm.KeyField) )
                return ( FALSE );
            memcpy(sz, SavePicParm.KeyField, sizeof(SavePicParm.KeyField));
            CtrlDataSetText(ar, IDC_OWAVEOPT_EDIT_PASSWORD, sz);
            }
        else
            {
            if ( pp->Status != ERR_NONE )
                ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
            /* "The image could not be expanded." */
            return ( FALSE );
            }            
        *pp = SavePicParm;
        *pApp = SaveApp;
        dwX = -(LONG)MiscTickCount();
        }
    dw = MiscTickCount();
    dwX += dw;
    pApp->dwTime[TOTAL] -= dw;
    pApp->dwTime[TOTAL] += dwX;
    pApp->dwTime[OPTOTAL] = pApp->dwTime[TOTAL];
    
    /**************************************************************************
        Allocate output DIB buffer according to the height and width
        returned from REQ_INIT
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    ChildSetDibHeight(hWnd, 0);
    dwRowSize = ( ( pp->Head.biWidth * pp->Head.biBitCount + 7 ) / 8 + 3 ) & ~3;
    *pdwDibLen = labs(pp->Head.biHeight) * dwRowSize;

    pApp->dwTime[ALLOC] -= MiscTickCount();
    *ppDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwDibLen, IDS_IMAGEOUTOFMEMORY);
    pApp->dwTime[ALLOC] += MiscTickCount();
    if ( *ppDib == 0 )
        {
        Pegasus(pp, REQ_TERM);
        CleanupOpenWavelet(0, pp, 0);
        return ( FALSE );
        }
    if ( bYield )
        {
        pApp->dwTime[PAINT] -= MiscTickCount();
        ChildInvalidatePalette(hWnd);
        ChildSetWindowSize(hWnd);
        pApp->dwTime[PAINT] += MiscTickCount();
        }

    /* initialize put queue buffer pointers */
    pp->Put.Start = *ppDib;            
    pp->Put.End = pp->Put.Start + *pdwDibLen;
    pp->Put.Front = pp->Put.Start;
    pp->Put.Rear  = pp->Put.Front;  /* initially, nothing has been put in the queue */

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */

    response = PegasusReqExec(pp);
    if ( response == RES_ERR )
        {
        if ( pp->Status != ERR_BAD_DATA || pp->Put.Rear == pp->Put.Front )
            {
            CleanupOpenWavelet(*ppDib, pp, 0);
            if ( pp->Status != ERR_NONE )
                ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
            /* "The image could not be expanded." */
            return ( FALSE );
            }
        }

    /**************************************************************************
        Pegasus(REQ_TERM)
    */
    if ( response != RES_ERR )
        {
        pApp->dwTime[REQTERM] -= MiscTickCount();
        response = Pegasus(pp, REQ_TERM);
        pApp->dwTime[REQTERM] += MiscTickCount();
        assert(response == RES_DONE);
        }
    pApp->dwTime[ALLOC] -= MiscTickCount();
    pApp->dwTime[ALLOC] += MiscTickCount();
    pApp->dwTime[OPTOTAL] += MiscTickCount();
    
    pApp->dwTime[PAINT] -= MiscTickCount();
    ChildSetDibHeight(hWnd, labs(pp->Head.biHeight));
    if ( !bYield )
        {
        ChildInvalidatePalette(hWnd);
        ChildSetWindowSize(hWnd);
        }
    pApp->dwTime[PAINT] += MiscTickCount();    

    return ( TRUE );
}


    
/******************************************************************************
 *  BOOL ExpandImageInWindow(
 *      HWND                   hWnd,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      BYTE PICHUGE* PICFAR*  ppDib,
 *      DWORD PICFAR*          pdwDibLen,
 *      PIC_PARM PICFAR*       pDibPicParm,
 *      VOID PICFAR*           pOpenData)
 * 
 *  all the open sequential Wavelet interesting stuff
 *
 *  parameters:
 *      hWnd        - handle of child window to display image
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      ppDib       - receives pointer to dib
 *      pdwDibLen   - receives length of dib
 *      pDibPicParm - receives picparm for dib
 *      pOpenData   - pointer to image settings array
 *
 *  returns:
 *      FALSE if some error occurs
 *
 *  notes:
 *      currently warns the user on the second, etc. comment and second, etc.
 *      app2 field.  Only the first of each is displayed in properties
 ******************************************************************************/
static BOOL ExpandImageInWindow(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR*           pOpenData)
{
    int num;
    PIC_PARM pp;
    RGBQUAD ColorTable[256];
    LPCTRLINITDATA par = (LPCTRLINITDATA)pOpenData;
    BYTE PICHUGE* pDib;
    DWORD dwDibLen;
    DWORD dwBits;
    BOOL bRet = TRUE;
    char sz[_MAX_PATH + 1];
    //####DWORD dwSecondary;
    //####DWORD dwPrimary;
    DWORD dwNumColors;
    LPCTRLINITDATA pCtrl;
    BOOL bDisplayTiming;
    APPDATA App;
    BOOL bRefresh;
        
    NOREFERENCE(hWnd);
    
    memset(&App, 0, sizeof(App));
    *ppDib = 0;
    *pdwDibLen = 0;

    pCtrl = CtrlData(par, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);

    pDibPicParm->App = (LPARAM)(APPDATA PICFAR*)&App;
    App.hWnd = hWnd;
    if ( !ExpandWaveletInWindow(
			hWnd,
            pSrc,
            dwSrcLen,
            pSrcPicParm,
            ppDib,
            pdwDibLen,
            pDibPicParm,
            pOpenData) )
        return ( FALSE );

    bRefresh = FALSE;   /* assume that ExpandWaveletInWindow has left image current */
    /* apply image options for color reduction, etc by applying OP_UTL to the DIB */
    if ( !CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_IMAGESETTINGS) && ToolDefUtility.nFoundParmVer != 0 )
        {        
        /* the remaining settings depend on DIB size */
        switch ( CtrlDataWhichRadio(par, IDC_OWAVEOPT_RADIO_32BITS) )
            {
            case IDC_OWAVEOPT_RADIO_32BITS:
					 dwBits = 32;
                break;
            case IDC_OWAVEOPT_RADIO_24BITS:
                dwBits = 24;
                break;
            case IDC_OWAVEOPT_RADIO_16BITS:
                dwBits = 16;
                break;
            case IDC_OWAVEOPT_RADIO_8BITS:
                dwBits = 8;
                break;
            case IDC_OWAVEOPT_RADIO_4BITS:
                dwBits = 4;
                break;
            case IDC_OWAVEOPT_RADIO_1BITS:
                dwBits = 1;
                break;
            default:
					 assert(FALSE);
                dwBits = pDibPicParm->Head.biBitCount;
                break;            
            }

        if ( dwBits > 8 )
            {
            /* since it's not palettized, we only care if it's different from the original
                bit depth */
            if ( dwBits != pDibPicParm->Head.biBitCount )
                {
                App.dwTime[BITDEPTH] -= MiscTickCount();
                bRet = Op82BitDepth(
                    dwBits,
                    CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_DITHER),    // could count if dwBits == 16
                    *ppDib, *pdwDibLen, pDibPicParm,
                    &pDib,  &dwDibLen,  &pp);
                App.dwTime[BITDEPTH] += MiscTickCount();
					 bRefresh |= bRet;
                }
            }
        else if ( CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_MAKEGRAY) )
            {
            if ( pDibPicParm->Head.biBitCount != dwBits ||
                 !MiscIsGrayPalette(pDibPicParm->Head.biClrImportant, pDibPicParm->ColorTable) )
                {
                dwNumColors = CtrlDataGetLong(par, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, FALSE);
                App.dwTime[PALETTE] -= MiscTickCount();
                bRet = Op82Grayscale(
                    dwBits,
                    CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_DITHER),
                    dwNumColors,
                    *ppDib, *pdwDibLen, pDibPicParm,
                    &pDib,  &dwDibLen,  &pp);
                App.dwTime[PALETTE] += MiscTickCount();
                InitDisplayTiming[INDEX_STATIC_PALETTE].pszText = "Make Gray Scale";
                bRefresh |= bRet;
                }
            }
        else if ( CtrlDataIsPushed(par, IDC_OWAVEOPT_RADIO_MAKEPALETTE) )
			{
            dwNumColors = CtrlDataGetLong(par, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, FALSE);
            App.dwTime[PALETTE] -= MiscTickCount();
            bRet = Op82MakePalette(
                dwBits,
                CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_DITHER),
                dwNumColors,
                *ppDib, *pdwDibLen, pDibPicParm,
                &pDib,  &dwDibLen,  &pp);
            App.dwTime[PALETTE] += MiscTickCount();
            InitDisplayTiming[INDEX_STATIC_PALETTE].pszText = "Make Palette";
            bRefresh |= bRet;
            }
        else if ( CtrlDataIsPushed(par, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE) )
            {
            CtrlDataGetText(par, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
                dwNumColors = MiscPaletteColors(sz, ColorTable, 1U << (WORD)dwBits);
            assert(dwNumColors != 0);
				/* if external palette file doesn't have a suitable palette, then it's a bug */
            App.dwTime[PALETTE] -= MiscTickCount();
            bRet = Op82ExternalPalette(
                dwBits,
                CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_DITHER),
                dwNumColors,
                ColorTable,
                *ppDib, *pdwDibLen, pDibPicParm,
                &pDib,  &dwDibLen,  &pp);
            App.dwTime[PALETTE] += MiscTickCount();
            InitDisplayTiming[INDEX_STATIC_PALETTE].pszText = "External Palette";
            bRefresh |= bRet;
            }        
        else /* EMBEDDEDPALETTE is pushed */
            {
            /* we only care if it's different from the original bit depth,
                (else the embedded palette was already applied in creating the dib) */
#if 0   //####
				if ( dwBits < pDibPicParm->Head.biBitCount )
                {
                dwPrimary = pSrcPicParm->Head.biClrImportant;
                dwSecondary = ( pSrcPicParm->Head.biClrUsed - pSrcPicParm->Head.biClrImportant );
                if ( dwPrimary < 2 || dwPrimary > ( 1U << (WORD)dwBits ) )
                    {
                    assert(dwSecondary >= 2 && dwSecondary <= ( 1U << (WORD)dwBits ));
                    dwNumColors = dwSecondary;
                    hmemcpy(ColorTable, &pSrcPicParm->ColorTable[(WORD)dwSecondary], dwNumColors);
                    }
                else
                    {
                    dwNumColors = dwPrimary;
                    hmemcpy(ColorTable, pSrcPicParm->ColorTable, dwNumColors);
                    }
                App.dwTime[PALETTE] -= MiscTickCount();
                bRet = Op82ExternalPalette(
                    dwBits,
						  CtrlDataIsPushed(par, IDC_OWAVEOPT_CHECK_DITHER),
                    dwNumColors,
                    ColorTable,
                    *ppDib, *pdwDibLen, pDibPicParm,
                    &pDib,  &dwDibLen,  &pp);
                App.dwTime[PALETTE] += MiscTickCount();
                InitDisplayTiming[INDEX_STATIC_PALETTE].pszText = "Embedded Palette";
                bRefresh |= bRet;
                }
#endif  //####
            }
        if ( bRefresh )
            {
            MiscGlobalFreePtr(ppDib);
            *ppDib = pDib;
            *pdwDibLen = dwDibLen;
            hmemcpy(&pp.u, &pDibPicParm->u, sizeof(pp.u));
            pp.Comment = pDibPicParm->Comment;
				pp.CommentLen = pDibPicParm->CommentLen;
            pp.CommentSize = pDibPicParm->CommentSize;
            hmemcpy(pDibPicParm, &pp, sizeof(*pDibPicParm));
            }
        }
    
    if ( MiscIsPaletteDisplay(&num) && pDibPicParm->Head.biBitCount > 8 )
        {
        /* if the image is rgb on a palettized display, it will look very bad unless
            we've provided windows with a suitable palette. */
        App.dwTime[DISPLAYPALETTE] -= MiscTickCount();
        Op82CreatePalette(1 << num, *ppDib, *pdwDibLen, pDibPicParm);
        App.dwTime[DISPLAYPALETTE] -= MiscTickCount();
        bRefresh = TRUE;
        }

    if ( bRefresh )
        {
		  App.dwTime[PAINT] -= MiscTickCount();
        ChildInvalidatePalette(hWnd);
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
        App.dwTime[PAINT] += MiscTickCount();    
        }

    App.dwTime[TOTAL] += MiscTickCount();
    
    if ( bDisplayTiming )
        {
        MiscDisplayTimingDialog(hWnd,
            MAKEINTRESOURCE(IDD_OWAVETIME),
            App.dwTime,
            InitDisplayTiming);
        }

    return ( bRet );
}



/******************************************************************************
 *  BOOL OpenWavelet(
 *      HWND                   hWnd,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      BYTE PICHUGE* PICFAR*  ppDib,
 *      DWORD PICFAR*          pdwDibLen,
 *      PIC_PARM PICFAR*       pDibPicParm,
 *      VOID PICFAR* PICFAR*   ppOpenData)
 * 
 *  opcode def data function to expand image to dib
 *
 *  parameters:
 *      hWnd        - handle of child window to display image
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      ppDib       - receives pointer to dib
 *      pdwDibLen   - receives length of dib
 *      pDibPicParm - receives picparm for dib
 *      ppOpenData  - input:  pdwOpFlags (cast as LPDWORD)
 *                    output: receives pointer to image settings array
 *
 *  returns:
 *      FALSE if some error occurs
 ******************************************************************************/
static BOOL OpenWavelet(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    char sz[_MAX_PATH + 1];
    int nColors;
    long lPrimaryColors;
    long lSecondaryColors;
    
    if ( ToolDefUtility.nFoundParmVer == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPCODE82MISSINGOPTIONS, (LPCSTR)ToolDefUtility.pszAbout);
        }
    /* read default ini image options settings */
    if ( !GetCtrlDefaults(szOptionsSection, (LPCTRLINITDATA PICFAR*)ppOpenData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_IMAGEOUTOFMEMORY);
        /* "There was not enough memory to open the image." */
        return ( FALSE );
        }        

    if ( !CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_CHECK_IMAGESETTINGS) && ToolDefUtility.nFoundParmVer != 0 )
        {
        /*#### Wavelet doesn't deal with 32-bit or 16-bit DIB's (and UTL doesn't convert) */
        CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_32BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_16BITS);
    
        /* if source image is grayscale, don't let the user change color palette choices
            in the image options dialog box */
        if ( pSrcPicParm->Head.biBitCount < 16 &&
             MiscIsGrayPalette(pSrcPicParm->Head.biClrImportant, pSrcPicParm->ColorTable) )
            {
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_STATIC_COLORPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_32BITS);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_24BITS);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_16BITS);

            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_MAKEPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE);
        
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_EDIT_COLORSTOMAKE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_STATIC_COLORS);
        
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_STATIC_EXTERNALPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_BUTTON_BROWSE);

            /* and make sure either 4-bit or 8-bit bit depth is pushed and
                that make gray is pushed */
            if ( !CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_4BITS) &&
                 !CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_1BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_8BITS, TRUE);
            CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_CHECK_MAKEGRAY, TRUE);
            CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_CHECK_MAKEGRAY);
            }
        else
            {
            /* if it's a color source image, but there's no embedded palette,
                don't let the user select "Embedded palette" in color palette image options */
            if ( pSrcPicParm->Head.biClrUsed == 0 )
                CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE);

            if ( ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_8BITS) ||
                   CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_4BITS) ||
                   CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_1BITS) ) &&
                 !CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_CHECK_MAKEGRAY) )
                {
                if ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_8BITS) )
                    nColors = 256;
                else if ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_4BITS) )
                    nColors = 16;
                else
                    nColors = 2;
                /* if the image needs a color palette
                    and embedded palette is selected, but there is no suitable embedded palette
                    or external palette is selected, but the file doesn't have a suitable palette,
                    then report the error to the user and set make palette for this image */
                CtrlDataGetText(*ppOpenData, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
                lPrimaryColors = pSrcPicParm->Head.biClrImportant;
                lSecondaryColors = pSrcPicParm->Head.biClrUsed - pSrcPicParm->Head.biClrImportant;
                if ( ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) &&
                       ( lPrimaryColors < 2 || lPrimaryColors > nColors ) &&
                       ( lSecondaryColors < 2 || lSecondaryColors > nColors ) ) ||
                     ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE) &&
                       MiscPaletteColors(sz, 0, nColors) == 0 ) )
                    {
                    ErrorMessage(STYLE_ERROR, IDS_IMAGECOLORPALETTE);
                    /* "The image default Color Palette option is not suitable. "
                        "An optimal color palette will be made." */
                    CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
                    CtrlDataSetLong(*ppOpenData, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, nColors, FALSE);
                    }
                }
            if ( CtrlDataIsInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) &&
                 CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) )
                CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
            }
        }
    /* else opcode 82 isn't available */        

    if ( !ExpandImageInWindow(
            hWnd,
            pSrc,
            dwSrcLen,
            pSrcPicParm,
            ppDib,
            pdwDibLen,
            pDibPicParm,
            *ppOpenData) )
        {
        CtrlDataFree((LPCTRLINITDATA PICFAR*)ppOpenData);
        return ( FALSE );
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
    CHAR PICHUGE *pList;
    
    pApp->dwTime[REQINIT] = -(LONG)MiscTickCount();
    response = Pegasus(p, REQ_INIT);
    pApp->dwTime[REQINIT] += MiscTickCount();
    /* continue with REQ_INIT until RES_ERR (error) or RES_DONE (init complete)
        the only thing which _should_ be happening here is Pegasus notifying us
        that it's encountered a comment or app2 field so we can allocate a buffer */
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                if ( p->Status == ERR_NONE || p->Status == -1 )
                    p->Status = ERR_BAD_DATA;        
                return ( RES_ERR );

            case RES_EXTEND_PIC2LIST:
                if ( p->PIC2List == 0 )
                    pList = GlobalAllocPtr(GMEM_MOVEABLE, p->PIC2ListLen);
                else
                    pList = GlobalReAllocPtr(p->PIC2List, p->PIC2ListLen, GMEM_MOVEABLE);
                if ( pList == 0 )
                {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_OUT_OF_SPACE;
                    return ( RES_ERR );
                }
                p->PIC2List = pList;
                p->PIC2ListSize = p->PIC2ListLen;
                break;

            case RES_PUT_DATA_YIELD:
            case RES_GET_DATA_YIELD:
            case RES_YIELD:
                pApp->dwTime[YIELD] -= MiscTickCount();
                if ( MiscYield() )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[YIELD] += MiscTickCount();
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
                        if ( ( p->u.WAVE.PicFlags & PF_YieldGet ) != 0 )
                            {                
                            pApp->dwTime[YIELD] -= MiscTickCount();
                            if ( MiscYield() )
                                {
                                Pegasus(p, REQ_TERM);
                                p->Status = ERR_NONE;
                                return ( RES_ERR );
                                }
                            pApp->dwTime[YIELD] += MiscTickCount();
                            }
                        }
                    }                        
                break;
                
            default:
                assert(FALSE);  /* no others are expected */
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
    LONG lHeight;
    int num;
    DWORD dwRowSize = ( ( p->Head.biWidth * p->Head.biBitCount + 7 ) / 8 + 3 ) & ~3;
    CHAR PICHUGE *pList;
    
    pApp->dwTime[REQEXEC] = -(LONG)MiscTickCount();
    response = Pegasus(p, REQ_EXEC);
    pApp->dwTime[REQEXEC] += MiscTickCount();
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                if ( p->Status == ERR_NONE || p->Status == -1 )
                    p->Status = ERR_BAD_DATA;        
                return ( RES_ERR );

            case RES_EXTEND_PIC2LIST:
                if ( p->PIC2List == 0 )
                    pList = GlobalAllocPtr(GMEM_MOVEABLE, p->PIC2ListLen);
                else
                    pList = GlobalReAllocPtr(p->PIC2List, p->PIC2ListLen, GMEM_MOVEABLE);
                if ( pList == 0 )
                {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_OUT_OF_SPACE;
                    return ( RES_ERR );
                }
                p->PIC2List = pList;
                p->PIC2ListSize = p->PIC2ListLen;
                break;

            case RES_PUT_DATA_YIELD:
            case RES_GET_DATA_YIELD:
            case RES_YIELD:
                pApp->dwTime[YIELD] -= MiscTickCount();
                if ( MiscYield() )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[YIELD] += MiscTickCount();
                break;

            case RES_PUT_NEED_SPACE:
                pApp->dwTime[MEMCPY] -= MiscTickCount();
                MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, 0);
                pApp->dwTime[MEMCPY] += MiscTickCount();
                if ( ( p->u.WAVE.PicFlags & PF_YieldPut ) != 0 )
                    {
                    pApp->dwTime[PAINT] -= MiscTickCount();
                    lHeight = labs(p->Head.biHeight) * dwRowSize - pApp->dwPutSize;
                    lHeight /= dwRowSize;
                    if ( !MiscIsPaletteDisplay(&num) || p->Head.biBitCount <= 8 )
                        ChildSetDibHeight(pApp->hWnd, lHeight);
                    /* else the display is palettized and it's a 16, 24 or 32-bit image
                        until a palette can be created, the image looks bad, so we forbid
                        any display until we have the entire image */
                    pApp->dwTime[PAINT] += MiscTickCount();
                    pApp->dwTime[YIELD] -= MiscTickCount();
                    if ( MiscYield() )
                        {
                        Pegasus(p, REQ_TERM);
                        p->Status = ERR_NONE;
                        return ( RES_ERR );
                        }
                    pApp->dwTime[YIELD] += MiscTickCount();
                    }
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
                        if ( ( p->u.WAVE.PicFlags & PF_YieldGet ) != 0 )
                            {
                            pApp->dwTime[YIELD] -= MiscTickCount();
                            if ( MiscYield() )
                                {
                                Pegasus(p, REQ_TERM);
                                p->Status = ERR_NONE;
                                return ( RES_ERR );
                                }
                            pApp->dwTime[YIELD] += MiscTickCount();
                            }
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
        pApp->dwTime[MEMCPY] -= MiscTickCount();
        MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, 0);
        pApp->dwTime[MEMCPY] += MiscTickCount();
        if ( ( p->u.WAVE.PicFlags & PF_YieldPut ) != 0 )
            {
            pApp->dwTime[PAINT] -= MiscTickCount();
            lHeight = labs(p->Head.biHeight) * dwRowSize - pApp->dwPutSize;
            lHeight /= dwRowSize;
            if ( !MiscIsPaletteDisplay(&num) || p->Head.biBitCount <= 8 )
                ChildSetDibHeight(pApp->hWnd, lHeight);
            /* else the display is palettized and it's a 16, 24 or 32-bit image
                until a palette can be created, the image looks bad, so we forbid
                any display until we have the entire image */
            pApp->dwTime[PAINT] += MiscTickCount();
            pApp->dwTime[YIELD] -= MiscTickCount();
            if ( MiscYield() )
                {
                Pegasus(p, REQ_TERM);
                p->Status = ERR_NONE;
                return ( RES_ERR );
                }
            pApp->dwTime[YIELD] += MiscTickCount();
            }
        }

    return ( response );                       
}



/******************************************************************************
 *  BOOL ExtractDibOpenWavelet(
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
static BOOL ExtractDibOpenWavelet(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm)
{
    RESPONSE response;
    PIC_PARM PICFAR* pp = pDibPicParm;

     /* nothing valid yet */
     *ppDib = 0;
     *pdwDibLen = 0;
    
    /**************************************************************************
        Initialize PIC_PARM data
    */

    /* initialize the pic_parm elements independent of image options settings */
    _fmemset(pp, 0, sizeof(*pp));     /* _VERY_ important */
    pp->ParmSize     = sizeof(*pp);
    pp->ParmVer      = CURRENT_PARMVER;  /* from PIC.H */
    pp->ParmVerMinor = 2;
    pp->Op = OP_W2D;

    hmemcpy(&pp->Head, &pSrcPicParm->Head, sizeof(pp->Head));

    /* initialize source image queue pointers */
    pp->Get.Start  = (BYTE PICHUGE*)pSrc;
        /* cast away constness to eliminate the warning --
            note however that this is not safe in general with Pegasus, or as a general
            practice, (although it is currently safe with Wavelet expand). */
    pp->Get.End    = pp->Get.Start + dwSrcLen;
    pp->Get.Front  = pp->Get.Start;
    pp->Get.Rear   = pp->Get.End;
    pp->Get.QFlags = Q_EOF;    /* no more to read, the entire image is in the buffer */

    /**************************************************************************
        Pegasus(REQ_INIT)
    */
    response = PegasusReqInit(pp);
    if ( response == RES_ERR )
        {
        CleanupOpenWavelet(0, pp, 0);
        if ( pp->Status != ERR_NONE )
            ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        return ( FALSE );
        }

    /**************************************************************************
        Allocate output DIB buffer according to the height and widthpad (scan line
        width including dword padding) returned from REQ_INIT
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    *pdwDibLen = labs(pSrcPicParm->Head.biHeight) *
        ( ( pSrcPicParm->Head.biWidth * pSrcPicParm->Head.biBitCount + 31 ) & ~31 ) / 8;
    *ppDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwDibLen, IDS_IMAGEOUTOFMEMORY);
    if ( *ppDib == 0 )
        {
        Pegasus(pp, REQ_TERM);
        CleanupOpenWavelet(0, pp, 0);
        return ( FALSE );
        }

    /* initialize put queue buffer pointers */
    pp->Put.Start = *ppDib;            
    pp->Put.End = pp->Put.Start + *pdwDibLen;

    if ( pSrcPicParm->Head.biHeight > 0 )
        pp->Put.Front = pp->Put.Start;
    else
    {
        pp->Put.Front = pp->Put.End;
        pp->Put.QFlags |= Q_REVERSE;
    }   
    pp->Put.Front = pp->Put.Start;
    pp->Head.biHeight = labs(pp->Head.biHeight);
    pp->Put.Rear = pp->Put.Front;         /* initially, nothing has been put in the queue */

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */

    response = PegasusReqExec(pp);
    if ( response == RES_ERR )
        {
        CleanupOpenWavelet(*ppDib, pp, 0);
        if ( pp->Status != ERR_NONE )
            ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        return ( FALSE );
        }

    /**************************************************************************
        Pegasus(REQ_TERM)
    */
    response = Pegasus(pp, REQ_TERM);
    assert(response == RES_DONE);

    return ( TRUE );
}



/******************************************************************************
 *  BOOL OptionsDialogOpenWavelet(
 *      HWND                   hWnd,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      BYTE PICHUGE* PICFAR*  ppDib,
 *      DWORD PICFAR*          pdwDibLen,
 *      PIC_PARM PICFAR*       pDibPicParm,
 *      VOID PICFAR* PICFAR*   pOpenData)
 * 
 *  image options dialog 
 *
 *  parameters:
 *      hWnd        - handle of child window to display image
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      ppDib       - receives pointer to dib
 *      pdwDibLen   - receives length of dib
 *      pDibPicParm - receives picparm for dib
 *      pOpenData   - pointer to image settings array
 *
 *  returns:
 *      FALSE if some error occurs
 ******************************************************************************/
static BOOL OptionsDialogOpenWavelet(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    int result;
    LPCTRLINITDATA PICFAR* ppData = (LPCTRLINITDATA PICFAR *)ppOpenData;
    LPCTRLINITDATA pOldData;
    OPTIONSUSERDATA oud;    /* OPTIONSUSERDATA defined in MOPCODES.H */
    BYTE PICHUGE *pOldDib;
    DWORD dwOldDibLen;
    PIC_PARM OldDibPicParm;

    if ( !CtrlDataDup(*ppData, &pOldData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return ( FALSE );
        }

    CtrlDataSetInactive((LPCTRLINITDATA)(*ppData)[1].pszText, IDC_TIMING_STATIC_INPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppData)[1].pszText, IDC_TIMING_EDIT_INPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppData)[1].pszText, IDC_TIMING_STATIC_OUTPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppData)[1].pszText, IDC_TIMING_EDIT_OUTPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)(*ppData)[1].pszText, IDC_TIMING_CHECK_YIELD);

    memset(&oud, 0, sizeof(oud));
    oud.hWnd        = hWnd;
    oud.ppSrc       = (BYTE PICHUGE* PICFAR*)&pSrc,
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = ppDib;
    oud.pdwDibLen   = pdwDibLen;
    oud.pDibPicParm = pDibPicParm;
    oud.bTested     = FALSE;

    result = DlgDoModal(hwndFrame, MAKEINTRESOURCE(IDD_OWAVEOPT), ImageOnCommand, *ppData, &oud);
    if ( result != IDOK && result != IDOK_UNCHANGED )
        {
        if ( oud.bTested )
            {
            /* paint over the dialog box with the last displayed version so we don't
                have to wait until expand is finished */
            UpdateWindow(hWnd);
            CtrlDataFree(ppData);
            *ppData = pOldData;
            pOldDib = *ppDib;
            OldDibPicParm = *pDibPicParm;
            ChildWaitCursorOn(hWnd);
            if ( ExpandImageInWindow(     
                    hWnd,
                    pSrc,
                    dwSrcLen,
                    pSrcPicParm,
                    ppDib,
                    pdwDibLen,
                    pDibPicParm,
                    *ppData) )
                CleanupOpenWavelet(pOldDib, &OldDibPicParm, 0);
            else                
                assert(FALSE);
            ChildWaitCursorOff(hWnd);
            }                
        else
            CtrlDataFree(&pOldData);
        
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the image options dialog." */
        return ( FALSE );
        }             

    /* else OK button was pushed */
    if ( CtrlDataIsPushed(*ppData, IDC_OWAVEOPT_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppData);

    CtrlDataFree(&pOldData);
    if ( result == IDOK )
        {
        /* OK button was pressed and there've been changes since the last time TEST was pushed */
        UpdateWindow(hWnd);
        pOldDib = *ppDib;
        dwOldDibLen = *pdwDibLen;
        OldDibPicParm = *pDibPicParm;
        ChildWaitCursorOn(hWnd);
        if ( !ExpandImageInWindow(     
                hWnd,
                pSrc,
                dwSrcLen,
                pSrcPicParm,
                ppDib,
                pdwDibLen,
                pDibPicParm,
                *ppData) )
            {
            ChildWaitCursorOff(hWnd);
            *ppDib = pOldDib;
            *pdwDibLen = dwOldDibLen;
            *pDibPicParm = OldDibPicParm;
            InvalidateRect(hWnd, NULL, FALSE);
            UpdateWindow(hWnd);
            return ( FALSE );
            }
        ChildWaitCursorOff(hWnd);
        CleanupOpenWavelet(pOldDib, &OldDibPicParm, 0);
        }
    return ( TRUE );
}



/******************************************************************************
 *  void PropertiesDialogOpenWavelet
 *      LPCSTR                 pszFilename,
 *      const BYTE PICHUGE*    pSrc,
 *      DWORD                  dwSrcLen,
 *      const PIC_PARM PICFAR* pSrcPicParm,
 *      const BYTE PICHUGE*    pDib,
 *      DWORD                  dwDibLen,
 *      const PIC_PARM PICFAR* pDibPicParm,
 *      const VOID PICFAR*     pOpenData)
 *
 *  image properties dialog
 *
 *  parameters:
 *      pszFilename - filename for image
 *      pSrc        - pointer to source image in memory
 *      dwSrcLen    - length of source image
 *      pSrcPicParm - PegasusQuery picparm for source image
 *      pDib        - pointer to dib
 *      dwDibLen    - length of dib
 *      pDibPicParm - picparm for dib
 *      pOpenData   - pointer to image settings array
 ******************************************************************************/
static void PropertiesDialogOpenWavelet(
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
    char sz[_MAX_PATH + sizeof(" Wavelet Properties")];
    int result;
    LPCTRLINITDATA pInitData;
    DWORD dwSrcWidthPad;
    DWORD dwSrcBmpSize;
    
    NOREFERENCE(pSrc);
    NOREFERENCE(pDib);
    NOREFERENCE(pOpenData);

    /**************************************************************************
        Create a writeable version of the property dialog control definitions
    */
    if ( !CtrlDataDup(InitPropertiesData, &pInitData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_PROPERTIESOUTOFMEMORY);
        /* "There was not enough memory to open the image properties dialog." */
        return;
        }

    /**************************************************************************
        Initialize all the properties' values
    */
    
    MiscShortFilename(pszFilename, sz, sizeof("C:\\...\\FILENAME.EXE") - 1);
    CtrlDataSetText(pInitData, IDC_WAVEPROP_FILENAME, sz);
    CtrlDataSetLong(pInitData, IDC_WAVEPROP_FILESIZE, dwSrcLen, FALSE);
    CtrlDataSetLong(pInitData, IDC_WAVEPROP_IMAGEWIDTH_SOURCE, pSrcPicParm->Head.biWidth, FALSE);
    CtrlDataSetLong(pInitData, IDC_WAVEPROP_IMAGEHEIGHT_SOURCE, labs(pSrcPicParm->Head.biHeight), FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_BITSPERPIXEL_SOURCE,
        pSrcPicParm->Head.biBitCount,
        FALSE);
    dwSrcWidthPad = ( pSrcPicParm->Head.biWidth * pSrcPicParm->Head.biBitCount + 7 ) / 8;
    dwSrcWidthPad = ( ( dwSrcWidthPad + sizeof(DWORD) - 1 ) / sizeof(DWORD) ) * sizeof(DWORD);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_IMAGESIZE_SOURCE,
        dwSrcWidthPad * labs(pSrcPicParm->Head.biHeight),
        FALSE);
    CtrlDataSetLong(pInitData, IDC_WAVEPROP_COLORTABLESIZE_SOURCE, pSrcPicParm->Head.biClrUsed, FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_NUMBERIMPORTANT_SOURCE,
        pSrcPicParm->Head.biClrImportant,
        FALSE);

    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_IMAGEWIDTH_DISPLAYED,
        pDibPicParm->Head.biWidth,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_IMAGEHEIGHT_DISPLAYED,
        labs(pDibPicParm->Head.biHeight),
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_BITSPERPIXEL_DISPLAYED,
        pDibPicParm->Head.biBitCount,
        FALSE);
    CtrlDataSetLong(pInitData, IDC_WAVEPROP_IMAGESIZE_DISPLAYED, dwDibLen, FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_COLORTABLESIZE_DISPLAYED,
        pDibPicParm->Head.biClrUsed,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_NUMBERIMPORTANT_DISPLAYED,
        pDibPicParm->Head.biClrImportant,
        FALSE);

    CtrlDataSetLong(pInitData, IDC_WAVEPROP_PICVERSION, pSrcPicParm->PicVer, FALSE);
    CtrlDataSetText(
        pInitData,
        IDC_WAVEPROP_VISUALORIENTATION,
        MiscVisualOrient(pSrcPicParm->VisualOrient));

    dwSrcBmpSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pSrcPicParm->CommentLen +
        dwSrcWidthPad * labs(pSrcPicParm->Head.biHeight),
    
    CtrlDataSetLong(
        pInitData,
        IDC_WAVEPROP_COMPRESSIONRATIO,
        ( dwSrcBmpSize + dwSrcLen / 2 ) / dwSrcLen,
        FALSE);
    CtrlDataGetText(pInitData, IDC_WAVEPROP_COMPRESSIONRATIO, sz, sizeof(sz));
    strcat(sz, ":1");
    CtrlDataSetText(pInitData, IDC_WAVEPROP_COMPRESSIONRATIO, sz);

    if ( pDibPicParm->PIC2List != 0 )
    {   /* version 3 comments, etc. */
		  LPSTR psz;
		  psz = (LPSTR)MiscAllocatePIC2Comment(pDibPicParm->PIC2List, P2P_Comment);
		  if ( psz != 0 )
				CtrlDataSetText(pInitData, IDC_WAVEPROP_COMMENT, psz);
		  MiscGlobalFreePtr(&psz);
		  psz = (LPSTR)MiscAllocatePIC2RawData(pDibPicParm->PIC2List, "APPL");
		  if ( psz != 0 )
				CtrlDataSetText(pInitData, IDC_WAVEPROP_APP, psz);
		  MiscGlobalFreePtr(&psz);
	 }

    /**************************************************************************
        Display the properties dialog
    */
    GetWindowText(hWnd, sz, sizeof(sz) - ( sizeof(" Wavelet Properties") - 1 ));
    strcat(sz, " Wavelet Properties");
    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_WAVEPROP),
        PropertiesOnCommand,
        pInitData,
        sz);
    CtrlDataFree(&pInitData);
    if ( result == -1 )
        {
        ErrorMessage(STYLE_ERROR, IDS_CREATEPROPERTIESDIALOG);
        /* "An unexpected error occurred opening the image properties dialog." */
        }             
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
 *      pUserData   - as defined by OpenWavelet
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
 *  void DefaultsDialogOpenWavelet(void)
 *
 *  default image options dialog
 * 
 ******************************************************************************/
void DefaultsDialogOpenWavelet(void)
{
    int result;
    LPCTRLINITDATA pOpenData;
    OPTIONSUSERDATA oud;
    
    memset(&oud, 0, sizeof(oud));
    if ( !GetCtrlDefaults(szOptionsSection, &pOpenData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return;
        }
    /* UTL doesn't yet convert to 32-bit or 16-bit DIB's */
    CtrlDataSetInactive(pOpenData, IDC_OWAVEOPT_RADIO_32BITS);
    CtrlDataSetInactive(pOpenData, IDC_OWAVEOPT_RADIO_16BITS);

    CtrlDataSetInactive((LPCTRLINITDATA)pOpenData[1].pszText, IDC_TIMING_STATIC_INPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)pOpenData[1].pszText, IDC_TIMING_EDIT_INPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)pOpenData[1].pszText, IDC_TIMING_STATIC_OUTPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)pOpenData[1].pszText, IDC_TIMING_EDIT_OUTPUTK);
    CtrlDataSetInactive((LPCTRLINITDATA)pOpenData[1].pszText, IDC_TIMING_CHECK_YIELD);

    CtrlDataSetPushed(pOpenData, IDC_OWAVEOPT_CHECK_SETDEFAULT, TRUE);
    CtrlDataSetInactive(pOpenData, IDC_OWAVEOPT_CHECK_SETDEFAULT);
    CtrlDataSetInactive(pOpenData, IDTEST);
    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_OWAVEOPT),
        DefaultsOnCommand,
        pOpenData,
        &oud);
    if ( result != IDOK )
        {
        CtrlDataFree(&pOpenData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the image options dialog." */
        return;
        }             

    CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, pOpenData);
    CtrlDataFree(&pOpenData);
}



/******************************************************************************
 * void CleanupOpenUtility(
 *      BYTE PICHUGE *pDib,
 *      PIC_PARM PICFAR *pDibPicParm,
 *      VOID PICFAR* pOpenData)
 *
 *  image cleanup (dib, settings, picparm)
 *
 *  parameters:
 *      pDib        - pointer to dib
 *      pDibPicParm - pointer to dib's picparm
 *      pOpenData   - pointer to image settings array
 *
 *  notes:
 *      dib and pOpenData have to be GlobalFree'd
 *
 *      picparm must not be GlobalFree'd, but any comment, app data or anything
 *      else allocated in this module whose pointer is in the picparm _must_
 *      be freed here
 ******************************************************************************/
static void CleanupOpenWavelet(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData)
{
    MiscGlobalFreePtr(&pDib);
    if ( pDibPicParm != 0 )
    {
        MiscGlobalFreePtr(&pDibPicParm->PIC2List);
    }
    CtrlDataFree((LPCTRLINITDATA PICFAR*)&pOpenData);
}



/******************************************************************************
 *  BOOL GetCtrlDefaults(LPCSTR pszSection, LPCTRLINITDATA PICFAR* ppOpenData)
 *
 *  read default image options settings from ini file
 *
 *  parameters:
 *      ppOpenData - receives pointer to image settings array
 *
 *  returns:
 *      FALSE if a image settings array can't be allocated
 *
 *  notes:
 *      the default settings before the ini file exists are in InitOptionsData at
 *      the top of this file
 *
 *      regardless of the default settings, if the screen is palettized, then
 *      this sets the default bitcount setting to 4 or 8 as appropriate
 ******************************************************************************/
static BOOL GetCtrlDefaults(LPCSTR pszSection, LPCTRLINITDATA PICFAR* ppOpenData)
{
    int num;

    if ( !CtrlDataDup(InitOptionsData, ppOpenData) )
        return ( FALSE );
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, pszSection, *ppOpenData);

    if ( ToolDefUtility.nFoundParmVer == 0 )
    {
    	CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_CHECK_IMAGESETTINGS, TRUE);
    	CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_CHECK_IMAGESETTINGS);
    }
    
    /* if screen bit depth is <= 8, make sure default bits/pixel isn't greater */
    else if ( MiscIsPaletteDisplay(&num) )
        {
        /* opcode 82 won't deal with 32 bits or 16 bits so this prevents them
            on a palettized display */
        CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_32BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OWAVEOPT_RADIO_16BITS);
        if ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_32BITS) ||
             CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_16BITS) )
            CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_24BITS, TRUE);
        if ( num < 8 )
            {
            if ( CtrlDataIsPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_8BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OWAVEOPT_RADIO_4BITS, TRUE);
            }
        }
    return ( TRUE );
}


/******************************************************************************
 *  BOOL DefaultsOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arOpenData,
 *      LPCSTR          pszDlgTemplate, 
 *      void PICFAR*    pUserData)
 *
 *  default options dialog WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arOpenData  - image options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - as defined by OpenWavelet
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
	 LPCTRLINITDATA  arOpenData,
	 LPCSTR          pszDlgTemplate,
	 void PICFAR*    pUserData)
{
	 NOREFERENCE(pUserData);

	 if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
		  return ( nCtrlID == IDOK );

	 switch ( nCtrlID )
		  {
		  case -1:    /* for WM_INITDIALOG - the virtue is it saves us having to have a separarte
								function passed to our dialog box handler */
            DefaultsGrayedCtrls(hwndDlg, arOpenData);
            break;
            
		case IDC_OWAVEOPT_CHECK_IMAGESETTINGS:
            if ( wNotifyCode == BN_CLICKED )
            {
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
            }
            break;
            
        case IDC_OWAVEOPT_CHECK_MAKEGRAY:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* MakeGray is not an auto button (we have to check and uncheck it,
                    because we don't want it to be uncheckable when the source image
                    is gray scale -- but we don't want to gray it because we want
                    the number of grays choice to be enabled. */
                /* #### TODO: we need to change the color palette legends so that
                        the options apply to a gray image.  Then we can also lose
                        the # grays box */
                /* Toggle the button state */
                CheckDlgButton(
                    hwndDlg,
                    IDC_OWAVEOPT_CHECK_MAKEGRAY,
						  !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY));

                /* make sure the bit depth is 8 bits or 4 bits */
                if ( !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) &&
                     !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OWAVEOPT_RADIO_32BITS,
                        IDC_OWAVEOPT_RADIO_1BITS,
                        IDC_OWAVEOPT_RADIO_8BITS);

                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;                

        case IDC_OWAVEOPT_RADIO_32BITS:
        case IDC_OWAVEOPT_RADIO_24BITS:
        case IDC_OWAVEOPT_RADIO_16BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* uncheck make gray button for bit depths > 8 */
                CheckDlgButton(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY, FALSE);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_RADIO_8BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                SetColorsToMake(hwndDlg);
					 /* set default colors to make according to bit depth, if not yet set */
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_RADIO_4BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;
        
		  case IDC_OWAVEOPT_RADIO_1BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_OWAVEOPT_RADIO_MAKEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
					 DlgSetModified(hwndDlg);
                }
            break;


        case IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_OWAVEOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* browse for a suitable external palette file.  If the file doesn't have
						  a 16-color palette, and we click 4-bits, then IDOK will report the
                    error */
                if ( MiscPaletteBrowse(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OWAVEOPT);
            break;

        case IDOK:
            return ( DefaultOptionsValid(hwndDlg) );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL DefaultOptionsValid(HWND hwndDlg)
 *
 *  validate all the dialog settings on OK or Apply
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL DefaultOptionsValid(HWND hwndDlg)
{
    char sz[_MAX_PATH + 1];
    int maxColors;
    int n;
    BOOL fValid;
    
    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) )
        maxColors = 256;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) )
        maxColors = 16;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
        maxColors = 2;
    else
        return ( TRUE );

    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY) )
        {
        /* validate number of grays to make */
        n = GetDlgItemInt(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, &fValid, FALSE);
        if ( !fValid || n < 2 || n > maxColors )
            {
            ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
            /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
            SetFocus(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE), 0, -1);
            return ( FALSE );
            }
        }
    else
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE) )
            {
            /* validate number of colors to make */
            n = GetDlgItemInt(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, &fValid, FALSE);
            if ( !fValid || n < 2 || n > maxColors )
                {
                ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
                /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
                SetFocus(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE), 0, -1);
                return ( FALSE );
                }
            }
        else if ( !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) )
            {
            /* make sure external palette actually has a suitable palette */
            assert(IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE));
            GetDlgItemText(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            n = MiscPaletteColors(sz, 0, maxColors);
            if ( n == 0 )
                {
                SetFocus(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE), 0, -1);
                return ( FALSE );
                }
            }
        }
    return ( TRUE );        
}



/******************************************************************************
 *  void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar)
 *
 *  enable/gray default options controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 ******************************************************************************/
static void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar)
{
	BOOL fImageSettings = IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_IMAGESETTINGS);
    BOOL fMakeGray = IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY);
    BOOL fEnableColorPalette;

	if ( fImageSettings )
	{
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_CHECK_DITHER,        FALSE);
		CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_CHECK_MAKEGRAY,      FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE,    FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_GRAYS,        FALSE);

		CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_BITSPERPIXEL, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_32BITS,        FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_24BITS,        FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_16BITS,        FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_8BITS,         FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS,         FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS,         FALSE);

	    EnableColorPalette(hwndDlg, ar, FALSE);
	    return;
	}

    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
        {
        /* target bit depth <= 8 */

        /* enable make gray options */
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_CHECK_DITHER, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_GRAYS, fMakeGray);

        /* disable bit depths > 8 if makegray is checked,
            if the src image is grayscale, then the openimage initialization
            will have inactivated these settings and the following won't matter
            (or hurt) */
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_32BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_24BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_16BITS, !fMakeGray);

        fEnableColorPalette = !fMakeGray;
        }
    else
        {
        /* these don't make sense for bit depths > 8 */
        CtrlEnable(
            hwndDlg,
            ar,
            IDC_OWAVEOPT_CHECK_DITHER,
            IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_16BITS));
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_GRAYS,     FALSE);

        fEnableColorPalette = FALSE;
        }
    
    EnableColorPalette(hwndDlg, ar, fEnableColorPalette);
}



/******************************************************************************
 *  BOOL ImageOnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arOpenData,
 *      LPCSTR          pszDlgTemplate,
 *      void PICFAR*    pUserData)
 *
 *  image options dialog WM_COMMAND handler
 *
 *  parameters:
 *      hwndDlg     - options dialog window handle
 *      nCtrlID     - control id
 *      wNotifyCode - WM_COMMAND notify code
 *      arOpenData  - image options settings
 *      pszDlgTemplate - dialog template name or resource id
 *      pUserData   - as defined by OpenWavelet
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
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData)
{
    char sz[_MAX_PATH + sizeof(" Wavelet Open Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */
    BYTE PICHUGE *pOldDib;
    DWORD dwOldDibLen;
    PIC_PARM OldDibPicParm;

    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:    /* for WM_INITDIALOG - the virtue is it saves us having to have a separate
                        function passed to our dialog box handler */
            GetWindowText(poud->hWnd, sz, sizeof(sz) - ( sizeof(" Wavelet Open Options") - 1 ));
            strcat(sz, " Wavelet Open Options");
            SetWindowText(hwndDlg, sz);
            ImageGrayedCtrls(hwndDlg, arOpenData, poud);
            break;
            
		case IDC_OWAVEOPT_CHECK_IMAGESETTINGS:
            if ( wNotifyCode == BN_CLICKED )
            {
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
            }
            break;
            
        case IDC_OWAVEOPT_CHECK_MAKEGRAY:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* MakeGray is not an auto button (we have to check and uncheck it,
                    because we don't want it to be uncheckable when the source image
                    is gray scale -- but we don't want to gray it because we want
                    the number of grays choice to be enabled.
                    #### TODO: we need to change the color palette legends so that
                        the options apply to a gray image.  Then we can also lose
                        the # grays box */
                if ( ( poud->pDibPicParm->u.F2D.PicFlags & PF_IsGray ) == 0 )
                    {
                    /* Toggle the button state unless it's a gray scale source image */
                    CheckDlgButton(
                        hwndDlg,
                        IDC_OWAVEOPT_CHECK_MAKEGRAY,
                        !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY));
                    }                    

                /* make sure the bit depth is 8 bits or 4 bits */
                if ( !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) &&
                     !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OWAVEOPT_RADIO_32BITS,
                        IDC_OWAVEOPT_RADIO_1BITS,
                        IDC_OWAVEOPT_RADIO_8BITS);

                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;                

        case IDC_OWAVEOPT_RADIO_32BITS:
        case IDC_OWAVEOPT_RADIO_24BITS:
        case IDC_OWAVEOPT_RADIO_16BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* uncheck make gray button for bit depths > 8 */
                CheckDlgButton(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY, FALSE);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_RADIO_8BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                SetColorsToMake(hwndDlg);
                /* set default colors to make according to bit depth, if not yet set */
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_RADIO_4BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_OWAVEOPT_RADIO_1BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;
        
        case IDC_OWAVEOPT_RADIO_MAKEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;


        case IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_OWAVEOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* browse for a suitable external palette file.  If the file doesn't have
                    a 16-color palette, and we click 4-bits, then IDOK will report the
                    error */
                if ( MiscPaletteBrowse(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OWAVEOPT_CHECK_SETDEFAULT:
            // this prevents modification of this control from setting the modified
            // flag which would a repaint of the image if OK were pushed
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OWAVEOPT);
            break;

        case IDOK:
            /* this could do all the apply stuff, but then the dialog box would hang
                around until the image reprocessing were finished */
            return ( ImageOptionsValid(hwndDlg, poud) );

        case IDTEST:
            if ( ImageOptionsValid(hwndDlg, poud) )
                {
                DlgOnOK(hwndDlg);
                /* save the old dib and picparm in case an expand error occurs */
                pOldDib = *poud->ppDib;
                dwOldDibLen = *poud->pdwDibLen;
                OldDibPicParm = *poud->pDibPicParm;
                CtrlEnable(hwndDlg, arOpenData, IDOK,     FALSE);
                CtrlEnable(hwndDlg, arOpenData, IDCANCEL, FALSE);
                CtrlEnable(hwndDlg, arOpenData, IDTEST,   FALSE);
                EnableWindow(hwndDlg, FALSE);
                if ( ExpandImageInWindow(
                         poud->hWnd,
                         *poud->ppSrc,
                         *poud->pdwSrcLen,
                         poud->pSrcPicParm,
                         poud->ppDib,
                         poud->pdwDibLen,
                         poud->pDibPicParm,
                         arOpenData) )
                    {
                    CleanupOpenWavelet(pOldDib, &OldDibPicParm, 0);
                    DlgClearModified(hwndDlg);
                    poud->bTested = TRUE;
                    }
                else                         
                    {
                    *poud->ppDib = pOldDib;
                    *poud->pdwDibLen = dwOldDibLen;
                    *poud->pDibPicParm = OldDibPicParm;
                    }
                EnableWindow(hwndDlg, TRUE);
                CtrlEnable(hwndDlg, arOpenData, IDOK,     TRUE);
                CtrlEnable(hwndDlg, arOpenData, IDCANCEL, TRUE);
                CtrlEnable(hwndDlg, arOpenData, IDTEST,   TRUE);
                }
            break;

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud)
 *
 *  validate all the image options dialog settings on OK or Apply
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
    int maxColors;
    int n;
    BOOL fValid;
    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) )
        maxColors = 256;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) )
        maxColors = 16;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
        maxColors = 2;
    else
        {
        /* unless the # bits/pixel <= 8, there can't be an inconsistency in the settings
            unless there's a bug */
        assert(!IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY) &&
               ( poud->pSrcPicParm->u.F2D.PicFlags & PF_IsGray ) == 0 );
        return ( TRUE );
        }

    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY) )
        {
        /* validate number of grays to make */
        n = GetDlgItemInt(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, &fValid, FALSE);
        if ( !fValid || n < 2 || n > maxColors )
            {
            ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
            /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
            SetFocus(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE), 0, -1);
            return ( FALSE );
            }
        }
    else if ( ( poud->pSrcPicParm->u.F2D.PicFlags & PF_IsGray ) == 0 )
        {
        /* not making grayscale and source image isn't grayscale
            #### TODO: allow grayscale palette choices for gray scale and make gray */
        if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) )
            {
            assert(( poud->pSrcPicParm->Head.biClrImportant >= 2 &&
                     poud->pSrcPicParm->Head.biClrImportant <= (unsigned)maxColors ) ||
                   ( poud->pSrcPicParm->Head.biClrUsed -
                        poud->pSrcPicParm->Head.biClrImportant >= 2 &&
                     poud->pSrcPicParm->Head.biClrUsed -
                        poud->pSrcPicParm->Head.biClrImportant <= (unsigned)maxColors ));
            }
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE) )
            {
            /* validate number of colors to make */
            n = GetDlgItemInt(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, &fValid, FALSE);
            if ( !fValid || n < 2 || n > maxColors )
                {
                ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
                /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
                SetFocus(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE), 0, -1);
                return ( FALSE );
                }
            }
        else
            {
            /* make sure external palette actually has a suitable palette */
            assert(IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE));
            GetDlgItemText(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            n = MiscPaletteColors(sz, 0, maxColors);
            if ( n == 0 )
                {
                SetFocus(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OWAVEOPT_EDIT_EXTERNALPALETTE), 0, -1);
                return ( FALSE );
                }
            }
        }
    /* else source image is grayscale */
    return ( TRUE );        
}



/******************************************************************************
 *  void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA *poud)
 *
 *  enable/gray image options controls based on current settings
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *      ar      - image options settings array
 *      poud    - pointer to src/dib image data structure
 ******************************************************************************/
static void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA PICFAR* poud)
{
//####    long lColors;
//####    long lSecondaryColors;
	BOOL fImageSettings = IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_IMAGESETTINGS);
    BOOL fMakeGray = IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY);
    BOOL fEnableColorPalette = TRUE;

	if ( fImageSettings )
	{
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_CHECK_DITHER,        FALSE);
		CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_CHECK_MAKEGRAY,      FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE,    FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_GRAYS,        FALSE);

		CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_BITSPERPIXEL, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_32BITS,        FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_24BITS,        FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_16BITS,        FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_8BITS,         FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS,         FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS,         FALSE);

	    EnableColorPalette(hwndDlg, ar, FALSE);
	    return;
	}

    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
        {
        /* target bit depth <= 8 */

        /* enable make gray options */
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_CHECK_DITHER, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_GRAYS, fMakeGray);

        /* disable bit depths > 8 if makegray is checked,
            if the src image is grayscale, then the openimage initialization
            will have inactivated these settings and the following won't matter
            (or hurt) */
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_32BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_24BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_16BITS, !fMakeGray);
        if ( fMakeGray )
            {
            CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, TRUE);
            CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, TRUE);
            fEnableColorPalette = FALSE;
            }
        }
    else
        {
        /* these don't make sense for bit depths > 8 */
        CtrlEnable(
            hwndDlg,
            ar,
            IDC_OWAVEOPT_CHECK_DITHER,
            IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_16BITS));
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_GRAYS,     FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, TRUE);
        fEnableColorPalette = FALSE;
        }
    
    EnableColorPalette(hwndDlg, ar, fEnableColorPalette);

    /* for image options dialog for a displayed image,
        make sure embedded palette choices are consistent with the image's palette.
        if the source image doesn't have any palette, then the openimage init will
        have inactivated the embeddedpalette choice so none of this will matter
        (or hurt) */
//####
    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) )
        {
        switch ( poud->pSrcPicParm->Head.biBitCount )
        {
            case 8:
                if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) ||
                     IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
                    }
                else
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_8BITS, TRUE);
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, FALSE);
                    }
                break;
            case 4:
                if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) ||
                     IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
                    }
                else
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_8BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, TRUE);
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, FALSE);
                    }
                break;
            case 1:
                if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) ||
                     IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) )
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
                    }
                else
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_8BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, TRUE);
                    }
                break;
        }
        }
    else
        {
        if ( ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) &&
               poud->pSrcPicParm->Head.biBitCount == 8 ) ||
             ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) &&
               poud->pSrcPicParm->Head.biBitCount == 4 ) ||
             ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) &&
               poud->pSrcPicParm->Head.biBitCount == 1 ) )
            CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, TRUE);
        else
            CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_8BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, TRUE);
        }
//####
#if 0   //####
    lColors = poud->pSrcPicParm->Head.biClrImportant;
    lSecondaryColors = poud->pSrcPicParm->Head.biClrUsed - lColors;
    if ( lColors < 2 || ( lSecondaryColors >= 2 && lSecondaryColors < lColors ) )
        lColors = lSecondaryColors;
    if ( lColors > 16 )
        {
        /* no suitable 4-bit palette, if embedded palette is pushed, disable
            4-bit bit depth.  Else if 4-bit is pushed, disable embedded palette,
            Else enable 4-bit, and enable/disable embedded palette according
            to whether all the other palette stuff is enabled/disabled */
        if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) )
            {
            if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) ||
                 IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
                {
                CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
                SetGraysToMake(hwndDlg);
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, ar, poud);
                return;
                }
            else
                {
                CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_4BITS, FALSE);
                CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, FALSE);
                }
            }
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) ||
                  IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
            CtrlEnable(hwndDlg, 
                ar,
                IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE,
                FALSE);
        else
            {
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE,
                TRUE);
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OWAVEOPT_RADIO_4BITS,
                TRUE);
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OWAVEOPT_RADIO_1BITS,
                TRUE);
            }
        }
    else if ( lColors > 2 )
        {
        /* no suitable 1-bit palette, if embedded palette is pushed, disable
            1-bit bit depth.  Else if 1-bit is pushed, disable embedded palette,
            Else enable 1-bit, and enable/disable embedded palette according
            to whether all the other palette stuff is enabled/disabled */
        if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE) )
            {
            if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
                {
                CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                CheckDlgButton(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE, TRUE);
                SetGraysToMake(hwndDlg);
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, ar, poud);
                return;
                }
            else                
                CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_1BITS, FALSE);
            }
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
            CtrlEnable(hwndDlg, 
                ar,
                IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE,
                FALSE);
        else
            {
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE,
                TRUE);
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OWAVEOPT_RADIO_1BITS,
                TRUE);
            }
        }
#endif  //####
}



/******************************************************************************
 *  void EnableColorPalette(HWND hwndDlg, LPCTRLINITDATA ar, BOOL bEnabled)
 *
 *  enable the controls in the color palette group box
 *
 *  parameters:
 *      hwndDlg  - dialog window handle
 *      ar       - current image options settings array
 *      bEnabled - TRUE to enable, FALSE to gray
 *
 *  notes:
 *      no big deal, but since makepalette and externalpalette have other things
 *      to enable/gray, it's all here
 ******************************************************************************/
static void EnableColorPalette(HWND hwndDlg, LPCTRLINITDATA ar, BOOL bEnabled)
{
    /* group box */
    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_STATIC_COLORPALETTE,   bEnabled);

    /* radio buttons */
    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EMBEDDEDPALETTE, bEnabled);
    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_MAKEPALETTE,     bEnabled);
    CtrlEnable(hwndDlg, ar, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE, bEnabled);

    /* make palette options */
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OWAVEOPT_EDIT_COLORSTOMAKE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OWAVEOPT_STATIC_COLORS,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE));

    /* external palette options */
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OWAVEOPT_STATIC_EXTERNALPALETTE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OWAVEOPT_EDIT_EXTERNALPALETTE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OWAVEOPT_BUTTON_BROWSE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_EXTERNALPALETTE));
}



/******************************************************************************
 *  void SetGraysToMake(HWND hwndDlg)
 *
 *  set grays to make if current setting is invalid
 *
 *  parameters:
 *      hwndDlg  - dialog window handle
 *
 *  notes:
 *      making sure that grays to make setting is valid
 ******************************************************************************/
static void SetGraysToMake(HWND hwndDlg)
{
    if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY) )
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) )
            SetNumToMake(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, 256);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) )
            SetNumToMake(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, 4);
        else
            {
            assert(IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS));
            SetNumToMake(hwndDlg, IDC_OWAVEOPT_EDIT_GRAYSTOMAKE, 2);
            }
        }
}



/******************************************************************************
 *  void SetColorsToMake(HWND hwndDlg)
 *
 *  set colors to make if current setting is invalid
 *
 *  parameters:
 *      hwndDlg  - dialog window handle
 *
 *  notes:
 *      making sure that colors to make setting is valid
 ******************************************************************************/
static void SetColorsToMake(HWND hwndDlg)
{
    if ( !IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_CHECK_MAKEGRAY) &&
         IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_MAKEPALETTE) )
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_8BITS) )
            SetNumToMake(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, 256);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_4BITS) )
            SetNumToMake(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, 16);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OWAVEOPT_RADIO_1BITS) )
            SetNumToMake(hwndDlg, IDC_OWAVEOPT_EDIT_COLORSTOMAKE, 2);
        }
}



/******************************************************************************
 *  void SetNumToMake(HWND hwndDlg, int nID, int nNum)
 *
 *  helper for SetGraysToMake and SetColorsToMake -- set control integer value
 *  if the present value is invalid
 *
 *  parameters:
 *      hwndDlg  - dialog window handle
 *      nID      - edit control id
 *      nNum     - max valid number in edit
 *
 *  notes:
 *      if number in edit is not a number or is < 2 or > nNum, set it to nNum
 ******************************************************************************/
static void SetNumToMake(HWND hwndDlg, int nID, int nNum)
{
    int n;
    BOOL fValid;

    n = GetDlgItemInt(hwndDlg, nID, &fValid, FALSE);
    if ( !fValid || n < 2 || n > nNum )
        SetDlgItemInt(hwndDlg, nID, nNum, FALSE);
}
