/******************************************************************************
 *
 * MOSEQJPG.C - Minerva MDI sequential JPEG expand module
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
 *     OpenDefSequentialJpeg - opcode definition data to open sequential jpeg image
 *
 * Local functions:
 *
 *   ** call Pegasus to open image **
 *     ExpandImageInWindow       - all the open sequential jpeg interesting stuff
 *     InitPicParm               - initialize pic_parm from options settings
 *     PegasusReqInit            - Pegasus(p, REQ_INIT)
 *     PegasusReqExec            - Pegasus(p, REQ_EXEC)
 *
 *   ** opcode def functions **
 *     OpenSequentialJpeg        - expand image to dib
 *     ExtractDibOpenSequentialJpeg - extract full-size, full-detail dib with no interaction
 *     OptionsDialogOpenSequentialJpeg - image options dialog 
 *     DefaultsDialogOpenSequentialJpeg - default image options dialog
 *     PropertiesDialogOpenSequentialJpeg - image properties dialog
 *     CleanupOpenSequentialJpeg - image cleanup (dib, settings, picparm)
 *     DupPicParmOpenSequentialJpeg - duplicate picparm for new window
 *
 *   ** misc **
 *     GetCtrlDefaults           - read default image options settings from ini file
 *     DisplayTimingDialog       - display timing dialog
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
 *
 * Revision History:
 *   04-21-97  1.00.21  jrb  added extract procedure implementation
 *                           set child window size before setting scroll range so the
 *                             scroll bar doesn't flash
 *   11-13-96  1.00.00  jrb  created
 *
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "mdlg.h"
#include "merror.h"
#include "minerva.h" 
#include "mmisc.h"
#include "mchild.h"

/* in a perfect world the following would be referenced from a .h file.  As I've
    done it instead, adding an opcode means creating the opcode implementation
    source file, and then the only other step is in minerva.c to add a pointer
    to the opcode in the appropriate opcode table (with an extern declaration).
    I expect that most of the time, it won't be referenced by another opcode, so
    most of the time the minerva.c dependency is the _only_ one outside of building
    the opcode in the first place.  So that's why this isn't a perfect world. */
extern SAVEOP SaveDefSequentialJpeg;

extern TOOLSOP ToolDefUtility;

extern BOOL Op82CreatePalette(
    DWORD dwNumColors,
    BYTE PICHUGE* pDib, DWORD dwDibSize, PIC_PARM PICFAR* pDibPicParm);



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void InitOpenEnhancedSequentialJpeg(void);
static BOOL ExpandImageInWindow(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR*               pOpenData);
static BOOL OpenSequentialJpeg(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData);
static BOOL ExtractDibOpenSequentialJpeg(
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm);
static BOOL OptionsDialogOpenSequentialJpeg(
    HWND                       hwnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       pOpenData);
static void PropertiesDialogOpenSequentialJpeg(
    HWND                   hwnd,
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
static void DefaultsDialogOpenSequentialJpeg(void);
static BOOL DupPicParmOpenSequentialJpeg(const PIC_PARM PICFAR* pSrc, PIC_PARM PICFAR* pDup);
static void CleanupOpenSequentialJpeg(
    BYTE PICHUGE *pDib,
    PIC_PARM PICFAR *pDibPicParm,
    VOID PICFAR* pOpenData);
static BOOL GetCtrlDefaults(LPCTRLINITDATA PICFAR* ppOpenData);
static void InitPicParm(PIC_PARM PICFAR* p, const PIC_PARM PICFAR* pSrcPicParm, LPCTRLINITDATA ar);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR* p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA PICFAR* poud);
static BOOL DefaultsOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL DefaultOptionsValid(HWND hwndDlg);
static void DefaultsGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar);
static void EnableColorPalette(HWND hwndDlg, LPCTRLINITDATA ar, BOOL bEnabled);
static void SetGraysToMake(HWND hwndDlg);
static void SetColorsToMake(HWND hwndDlg);
static void SetNumToMake(HWND hwndDlg, int nID, int nNum);



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
#define DISPLAYPALETTE 7
#define PAINT    8
#define TOTAL    9



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { 0, eText, IDC_OSEQJPGTIME_TIMERRES, 0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_INIT,     0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_EXEC,     0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_TERM,     0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_ALLOC,    0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_MEMCPY,   0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_YIELD,    0, sizeof("999,999") },
    { 0, eText, IDC_OSEQJPGTIME_OPTOTAL,  0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_DISPLAYPALETTE, 0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_PAINT,    0, sizeof("999.999") },
    { 0, eText, IDC_OSEQJPGTIME_TOTAL,    0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/* ini section for image options */
static char szOptionsSection[] = "Sequential JPEG Expand Options";

static char szImageSize[]    = "Image Size";    /* ini key for image size radio buttons */
static char szBitsPerPixel[] = "Bits/Pixel";    /* ini key for bits/pixel radio buttons */
static char szColorPalette[] = "Color Palette"; /* ini key for color palette radio buttons */

/* specifying image options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Make Gray",             eCheckBox, IDC_OSEQJPGOPT_CHECK_MAKEGRAY,            FALSE },
    { "Dither",                eCheckBox, IDC_OSEQJPGOPT_CHECK_DITHER,              TRUE  },
    { "Cross-Block Smoothing", eCheckBox, IDC_OSEQJPGOPT_CHECK_ENHANCEDDECOMPRESS,  TRUE  },

    { szImageSize,    eRadioButton, IDC_OSEQJPGOPT_RADIO_FULLSIZE,        TRUE,  0, "100%" },
    { szImageSize,    eRadioButton, IDC_OSEQJPGOPT_RADIO_FOURTH,          FALSE, 0, "25%"  },
    { szImageSize,    eRadioButton, IDC_OSEQJPGOPT_RADIO_SIXTEENTH,       FALSE, 0, "1/16" },
    { szImageSize,    eRadioButton, IDC_OSEQJPGOPT_RADIO_SIXTYFOURTH,     FALSE, 0, "1/64" },
    { szBitsPerPixel, eRadioButton, IDC_OSEQJPGOPT_RADIO_32BITS,          FALSE, 0, "32"   },
    { szBitsPerPixel, eRadioButton, IDC_OSEQJPGOPT_RADIO_24BITS,          TRUE,  0, "24"   },
    { szBitsPerPixel, eRadioButton, IDC_OSEQJPGOPT_RADIO_16BITS,          FALSE, 0, "16"   },
    { szBitsPerPixel, eRadioButton, IDC_OSEQJPGOPT_RADIO_8BITS,           FALSE, 0, "8"    },
    { szBitsPerPixel, eRadioButton, IDC_OSEQJPGOPT_RADIO_4BITS,           FALSE, 0, "4"    },
    { szColorPalette, eRadioButton, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE, TRUE,  0, "Embedded" },
    { szColorPalette, eRadioButton, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE,     FALSE, 0, "Make",    },
    { szColorPalette, eRadioButton, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE, FALSE, 0, "External" },

    { "External Palette Filename",   eEdit, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, FALSE, _MAX_PATH },
    { "Make Gray Grays To Make",     eEdit, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE,     FALSE, 3, "256" },
    { "Make Palette Colors To Make", eEdit, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE,    FALSE, 3, "256" },
    { "Password",                    eEdit, IDC_OSEQJPGOPT_EDIT_PASSWORD,        TRUE,  8 },
    { "Image Left",                  eEdit, IDC_OSEQJPGOPT_EDIT_IMAGELEFT,       FALSE, 4 },
    { "Image Top",                   eEdit, IDC_OSEQJPGOPT_EDIT_IMAGETOP,        FALSE, 4 },
    { "Image Width",                 eEdit, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH,      FALSE, 4 },
    { "Image Height",                eEdit, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT,     FALSE, 4 },
    { "Window Left",                 eEdit, IDC_OSEQJPGOPT_EDIT_WINDOWLEFT,      FALSE, 4 },
    { "Window Top",                  eEdit, IDC_OSEQJPGOPT_EDIT_WINDOWTOP,       FALSE, 4 },

    { 0, ePushButton, IDC_OSEQJPGOPT_BUTTON_BROWSE          },
    { 0, ePushButton, IDTEST                                },
    { 0, eGroupBox,   IDC_OSEQJPGOPT_STATIC_COLORPALETTE    },
    { 0, eText,       IDC_OSEQJPGOPT_STATIC_COLORS          },
    { 0, eText,       IDC_OSEQJPGOPT_STATIC_EXTERNALPALETTE },
    { 0, eText,       IDC_OSEQJPGOPT_STATIC_GRAYS           },
    { 0, eCheckBox,   IDC_OSEQJPGOPT_CHECK_SETDEFAULT       },

    /*
     n/a  nested  dialog id    n/a   dialog button      dialog init data
    */
    { 0, ePushButton, IDC_BUTTON_TIMING, FALSE, IDD_TIMING  },
    { szOptionsSection, eDialog, IDD_TIMING, FALSE, IDC_BUTTON_TIMING, (LPSTR)InitTimingData },

    { 0, eEnd, -1 }
    };
 


/* specifying image properties controls' parameters */
static CTRLINITDATA InitPropertiesData[] =
    {
    { 0, eText, IDC_SEQJPGPROP_FILENAME,        0, _MAX_PATH },
    { 0, eText, IDC_SEQJPGPROP_FILESIZE,        0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_IMAGEWIDTH_SOURCE,         0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_IMAGEHEIGHT_SOURCE,        0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_BITSPERPIXEL_SOURCE,       0, 2 },
    { 0, eText, IDC_SEQJPGPROP_IMAGESIZE_SOURCE,          0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_COLORTABLESIZE_SOURCE,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_NUMBERIMPORTANT_SOURCE,    0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_IMAGEWIDTH_DISPLAYED,      0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_IMAGEHEIGHT_DISPLAYED,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_BITSPERPIXEL_DISPLAYED,    0, 2 },
    { 0, eText, IDC_SEQJPGPROP_IMAGESIZE_DISPLAYED,       0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_COLORTABLESIZE_DISPLAYED,  0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_NUMBERIMPORTANT_DISPLAYED, 0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_PICVERSION,                0, sizeof("4,000,000,000") },
    { 0, eText, IDC_SEQJPGPROP_VISUALORIENTATION,         0, sizeof("270* clockwise, then inverted") },
    { 0, eText, IDC_SEQJPGPROP_COMPRESSIONRATIO, 0, sizeof("4,000,000,000:1") },
    { 0, eText, IDC_SEQJPGPROP_LUMINANCE,       0, 3 },
    { 0, eText, IDC_SEQJPGPROP_CHROMINANCE,     0, 3 },
    { 0, eText, IDC_SEQJPGPROP_SUBSAMPLING,     0, sizeof("unknown") },
    { 0, eEdit, IDC_SEQJPGPROP_COMMENT,         FALSE, MAX_COMMENT },
    { 0, eEdit, IDC_SEQJPGPROP_APP2,            FALSE, MAX_COMMENT },
    { 0, eText, IDC_SEQJPGPROP_ENTROPYCODING,   0, sizeof("Els Arithmetic Entropy Coding") },
    
    { 0, eEnd, -1 }
    };



/* PegasusQuery biCompression types supported by this opcode */
static DWORD arEnhancedTypes[] = { BI_JPEG, BI_PICJ, BI_JPGE, BI_PC2J, BI_PC2E, (DWORD)-1 };
/* PegasusQuery biCompression types supported by this opcode */
static DWORD arTypes[] = { BI_JPEG, BI_PICJ, BI_PC2J, (DWORD)-1 };



/******************************************************************************
 *  global data
 ******************************************************************************/



/* opcode definition data for minerva.c open opcode table pOpenOpTable */
OPENOP OpenDefSequentialJpeg =
    {
    OP_S2D,
    "Sequential JPEG Expand",
    "Sequential JPEG Defaults",
    "Sequential JPEG (*.pic,*.jpg)|*.PIC;*.JPG|",
    arTypes,
    0,
    OpenSequentialJpeg,
    ExtractDibOpenSequentialJpeg,
    OptionsDialogOpenSequentialJpeg,
    PropertiesDialogOpenSequentialJpeg,
    DefaultsDialogOpenSequentialJpeg,
    CleanupOpenSequentialJpeg,
    DupPicParmOpenSequentialJpeg,
    &SaveDefSequentialJpeg,
    0
    };



/* opcode definition data for minerva.c open opcode table pOpenOpTable */
OPENOP OpenDefEnhancedSequentialJpeg =
    {
    OP_SE2D,
    "Enhanced Sequential JPEG Expand",
    0,
    0,
    arEnhancedTypes,
    InitOpenEnhancedSequentialJpeg,
    OpenSequentialJpeg,
    ExtractDibOpenSequentialJpeg,
    OptionsDialogOpenSequentialJpeg,
    PropertiesDialogOpenSequentialJpeg,
    0,
    CleanupOpenSequentialJpeg,
    DupPicParmOpenSequentialJpeg,
    &SaveDefSequentialJpeg,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



static void InitOpenEnhancedSequentialJpeg(void)
{
    OpenDefEnhancedSequentialJpeg.pszFilter = OpenDefSequentialJpeg.pszFilter;
    OpenDefEnhancedSequentialJpeg.pszDefaultsMenu = OpenDefSequentialJpeg.pszDefaultsMenu;
    OpenDefEnhancedSequentialJpeg.pfnDefaultsDialog = OpenDefSequentialJpeg.pfnDefaultsDialog;
    if ( OpenDefSequentialJpeg.nFoundParmVer != 0 )
        PegasusUnload(OpenDefSequentialJpeg.lOpcode, CURRENT_PARMVER);
    memset(&OpenDefSequentialJpeg, '\0', sizeof(OpenDefSequentialJpeg));
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
 *  all the open sequential jpeg interesting stuff
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
    LPCTRLINITDATA ar = (LPCTRLINITDATA)pOpenData;
    RESPONSE response;
    PIC_PARM PICFAR* pp = pDibPicParm;
    PIC_PARM SavePicParm;
    BYTE PICHUGE *pGetQ = 0;
    BYTE PICHUGE *pPutQ = 0;
    APPDATA App;
    APPDATA SaveApp;
    DWORD dwSize;
    BOOL  bDisplayTiming;
    BOOL  bYield;
    DWORD dwGetQSize;
    DWORD dwPutQSize;
    LPCTRLINITDATA pCtrl;
    DWORD dw;
    DWORD dwX;
    CHAR PICHUGE* PICHUGE* ppc;
    CHAR PICHUGE* pc;
        
    
    // make sure the window knows it has nothing valid to present yet
    *ppDib = 0;
    *pdwDibLen = 0;

    /**************************************************************************
        Initialize PIC_PARM data
    */

    /* initialize the pic_parm elements independent of image options settings */
    _fmemset(pp, 0, sizeof(*pp));     /* _VERY_ important */
    pp->ParmSize     = sizeof(*pp);
    pp->ParmVer      = CURRENT_PARMVER;  /* from PIC.H */
    pp->ParmVerMinor = 1;
    pp->PIC2ListSize = -1;  /* obtain exif tags for exif input */
    ////pp->u.J2D.PicFlags |= PF_ExpandThumbnail;
    pp->LoadResInstance = (unsigned int)hinstThis; /* load from resource if present */
        /*  ^ casting to unsigned int eliminates a 16-bit warning about casting a near pointer
            to a long (if it were cast to long or dword instead) and is effectively
            the same in 32 bit as casting to dword */
    pp->LoadPath = "";                      /* else load from disk */
    if ( OpenDefSequentialJpeg.nFoundParmVer == 0 )
        pp->Op = OP_SE2D;       /* Enhanced Sequential JPEG to DIB */
    else
        pp->Op = OP_S2D;        /* Sequential JPEG to DIB */

    memset(&App, 0, sizeof(App));
    App.hWnd = hWnd;
    pp->App = (LPARAM)(APPDATA PICFAR*)&App;

    /* apply Image Options settings to pic_parm */
    InitPicParm(pp, pSrcPicParm, ar);

    pp->u.J2D.PicFlags |= PF_AllocateComment;
        /* PF_AllocateComment allows us to be notified if there is a comment (or
            application data) and, only at that time and after we know how big the
            comment is, allocate space for the comment */
    pp->u.J2D.AppFieldSize = -1;
        /* NOTE: this is required in order to be notified that there is application data
            so we can allocate space knowing exactly how much is required.  Currently,
            Pegasus ignores PF_AllocateComment for the AppField in spite of PIC.H's comments.
            I expect that Pegasus eventually will not require this, but also that this
            will be allowed without hurting anything */

    pCtrl = CtrlData(ar, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);
    bYield = 
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);
    dwGetQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_INPUTK, FALSE) * 1024;
    dwPutQSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_OUTPUTK, FALSE) * 1024;

    if ( bYield )
        pp->u.J2D.PicFlags |= PF_YieldGet | PF_YieldPut;
                                                         
    /* initialize source image queue pointers */
    App.dwTime[TOTAL] -= MiscTickCount();
    if ( dwGetQSize == 0 || dwGetQSize >= dwSrcLen )
        {
        pp->Get.Start  = (BYTE PICHUGE*)pSrc;
            /* cast away constness to eliminate the warning --
                note however that this is not safe in general with Pegasus, or as a general
                practice, (although it is currently safe with Sequential JPEG expand). */
        pp->Get.End    = pp->Get.Start + dwSrcLen;
        pp->Get.Front  = pp->Get.Start;
        pp->Get.Rear   = pp->Get.End;
        pp->Get.QFlags = Q_EOF;    /* no more to read, the entire image is in the buffer */
        }
    else
        {
        App.dwTime[ALLOC] -= MiscTickCount();
        pGetQ = MiscGlobalAllocPtr(dwGetQSize, IDS_IMAGEOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pGetQ == 0 )
            return ( FALSE );
        pp->Get.Start = pGetQ;            
        pp->Get.End   = pp->Get.Start + dwGetQSize;
        pp->Get.Front = pp->Get.Start;
        pp->Get.Rear  = pp->Get.Front;
        App.pGet     = (BYTE PICHUGE *)pSrc;
        App.dwGetLen = dwSrcLen;
        }            

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
    App.dwTime[TOTAL] += -(LONG)dwX;
    SaveApp = App;
    while ( ( response = PegasusReqInit(pp) ) == RES_ERR )
        {
        if ( pp->Comment != 0 )
            {
            ppc = (CHAR PICHUGE* PICHUGE*)pp->Comment;
            ppc--;
            pc = (CHAR PICHUGE*)ppc;
            pp->Comment = *ppc;
            *pc++ = '\r';
            *pc++ = '\n';
            *pc++ = '-';
            *pc++ = '>';
                pp->CommentLen = lstrlen((LPSTR)pp->Comment) + 1;
                lstrcpy((LPSTR)pp->Comment, (LPSTR)pp->Comment + 2);
            }
        if ( pp->u.J2D.AppField != 0 )
            {
            ppc = (CHAR PICHUGE* PICHUGE*)pp->u.J2D.AppField;
            ppc--;
            pc = (CHAR PICHUGE*)ppc;
            pp->u.J2D.AppField = (BYTE PICHUGE *)*ppc;
            *pc++ = '\r';
            *pc++ = '\n';
            *pc++ = '-';
            *pc++ = '>';
            pp->u.J2D.AppFieldLen = lstrlen((LPSTR)pp->u.J2D.AppField) + 1;
            lstrcpy((LPSTR)pp->u.J2D.AppField, (LPSTR)pp->u.J2D.AppField + 2);
            }
        CleanupOpenSequentialJpeg(0, pp, 0);
        if ( pp->Status == ERR_MISSING_KEY || pp->Status == ERR_INVALID_KEY )
            {
            char sz[9];
            
            memset(sz, 0, sizeof(sz));
            if ( !MiscGetPassword(hWnd, (LPSTR)SavePicParm.KeyField) )
                {
                MiscGlobalFreePtr(&pGetQ);
                return ( FALSE );
                }
            memcpy(sz, SavePicParm.KeyField, sizeof(SavePicParm.KeyField));
            CtrlDataSetText(ar, IDC_OSEQJPGOPT_EDIT_PASSWORD, sz);
            }
        else
            {
            MiscGlobalFreePtr(&pGetQ);
            if ( pp->Status != ERR_NONE )
                ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
            /* "The image could not be expanded." */
            return ( FALSE );
            }            
        *pp = SavePicParm;
        App = SaveApp;
        dwX = -(LONG)MiscTickCount();
        }
    dw = MiscTickCount();
    dwX += dw;
    App.dwTime[TOTAL] -= dw;
    App.dwTime[TOTAL] += dwX;

    /**************************************************************************
        Allocate output DIB buffer according to the height and widthpad (scan line
        width including dword padding) returned from REQ_INIT
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    ChildSetDibHeight(hWnd, 0);
    *pdwDibLen = pp->Head.biHeight * pp->u.J2D.WidthPad;
    App.dwTime[ALLOC] -= MiscTickCount();
    *ppDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwDibLen, IDS_IMAGEOUTOFMEMORY);
    App.dwTime[ALLOC] += MiscTickCount();
    if ( *ppDib == 0 )
        {
        Pegasus(pp, REQ_TERM);
        MiscGlobalFreePtr(&pGetQ);
        CleanupOpenSequentialJpeg(0, pp, 0);
        return ( FALSE );
        }
    if ( bYield )
        {
        App.dwTime[PAINT] -= MiscTickCount();
        ChildInvalidatePalette(hWnd);
        ChildSetWindowSize(hWnd);
        App.dwTime[PAINT] += MiscTickCount();
        }

    if ( dwPutQSize == 0 || dwPutQSize >= *pdwDibLen )
        {
        /* initialize put queue buffer pointers */
        pp->Put.Start = *ppDib;            
        pp->Put.End = pp->Put.Start + *pdwDibLen;
        }
    else
        {
        /* note that _very_ bad stuff happens if the put q is smaller than the strip size */
        if ( dwPutQSize < (DWORD)pp->u.J2D.StripSize )
            dwSize = pp->u.J2D.StripSize;
        else
            dwSize = dwPutQSize;            
        App.dwTime[ALLOC] -= MiscTickCount();            
        pPutQ = MiscGlobalAllocPtr(dwSize, IDS_IMAGEOUTOFMEMORY);
        App.dwTime[ALLOC] += MiscTickCount();
        if ( pPutQ == 0 )
            {
            Pegasus(pp, REQ_TERM);
            MiscGlobalFreePtr(&pGetQ);
            CleanupOpenSequentialJpeg(*ppDib, pp, 0);
            return ( FALSE );
            }
        pp->Put.Start  = pPutQ;
        pp->Put.End    = pPutQ + dwSize;
        App.dwPutSize = *pdwDibLen;
        /* note App.dwPutLen isn't used for this opcode */
        App.pPut      = (BYTE PICHUGE *)*ppDib;
        }

    if ( pSrcPicParm->Head.biHeight < 0 )
        {
        pp->Put.Front = pp->Put.End;
        pp->Put.QFlags |= Q_REVERSE; /* this is the usual case */
        /* ordinarily the dib scan line highest on the screen is the final scan line
            in the dib buffer.  PegasusQuery returns biHeight < 0 for that dib and
            we have to tell Pegasus that sense of the buffer is reversed (top is last).
            Thus the Q front (which is the first data returned by Pegasus, which is the first
            scan line since that's the scan line earliest in a JPEG file) is set to
            the End of the dib buffer and Q_REVERSE is set so Pegasus knows to put
            data in the buffer moving from the Front towards the start of the buffer */
        App.pPut += App.dwPutSize;
        }
    else
        {
        /* apparently the JPEG image is reversed from what it's supposed to be so that
            the first scan line processed will be the bottom scan line in the resulting
            dib image */
        pp->Put.Front = pp->Put.Start;
        }        
    pp->Put.Rear = pp->Put.Front;         /* initially, nothing has been put in the queue */

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */
    response = PegasusReqExec(pp);
    if ( pp->Comment != 0 )
        {
        ppc = (CHAR PICHUGE* PICHUGE*)pp->Comment;
        ppc--;
        pc = (CHAR PICHUGE*)ppc;
        pp->Comment = *ppc;
        *pc++ = '\r';
        *pc++ = '\n';
        *pc++ = '-';
        *pc++ = '>';
        pp->CommentLen = lstrlen((LPSTR)pp->Comment) + 1;
        lstrcpy((LPSTR)pp->Comment, (LPSTR)pp->Comment + 2);
        }
    if ( pp->u.J2D.AppField != 0 )
        {
        ppc = (CHAR PICHUGE* PICHUGE*)pp->u.J2D.AppField;
        ppc--;
        pc = (CHAR PICHUGE*)ppc;
        pp->u.J2D.AppField = (BYTE PICHUGE *)*ppc;
        *pc++ = '\r';
        *pc++ = '\n';
        *pc++ = '-';
        *pc++ = '>';
        pp->u.J2D.AppFieldLen = lstrlen((LPSTR)pp->u.J2D.AppField) + 1;
        lstrcpy((LPSTR)pp->u.J2D.AppField, (LPSTR)pp->u.J2D.AppField + 2);
        }
    if ( response == RES_ERR )
        {
        if ( ( pp->Put.Start == *ppDib && pp->Put.Rear == pp->Put.Front ) ||
             ( pp->Put.Start != *ppDib && *pdwDibLen == App.dwPutSize ) )
            {
            MiscGlobalFreePtr(&pGetQ);
            MiscGlobalFreePtr(&pPutQ);
            CleanupOpenSequentialJpeg(*ppDib, pp, 0);
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
        App.dwTime[REQTERM] -= MiscTickCount();
        response = Pegasus(pp, REQ_TERM);
        App.dwTime[REQTERM] += MiscTickCount();
        assert(response == RES_DONE);
        }
    App.dwTime[ALLOC] -= MiscTickCount();
    MiscGlobalFreePtr(&pGetQ);
    MiscGlobalFreePtr(&pPutQ);
    App.dwTime[ALLOC] += MiscTickCount();

    App.dwTime[TOTALOP] = App.dwTime[TOTAL];
    App.dwTime[TOTALOP] += MiscTickCount();

    App.dwTime[DISPLAYPALETTE] -= MiscTickCount();
    if ( MiscIsPaletteDisplay(&num) && pDibPicParm->Head.biBitCount > 8 )
    {
        /* if the image is rgb on a palettized display, it will look very bad unless
            we've provided windows with a suitable palette. */
        Op82CreatePalette(1 << num, *ppDib, *pdwDibLen, pDibPicParm);
        ChildInvalidatePalette(hWnd);
    }
    App.dwTime[DISPLAYPALETTE] += MiscTickCount();
    App.dwTime[PAINT] -= MiscTickCount();    
    ChildSetDibHeight(hWnd, pp->Head.biHeight);
    if ( !bYield )
        {
        ChildInvalidatePalette(hWnd);
        ChildSetWindowSize(hWnd);
        }
    App.dwTime[PAINT] += MiscTickCount();    

    App.dwTime[TOTAL] += MiscTickCount();

    if ( pp->Status != 0 )
        ErrorMessage(STYLE_ERROR, IDS_IMAGEBADDATA);
    if ( bDisplayTiming )
        MiscDisplayTimingDialog(hWnd,
            MAKEINTRESOURCE(IDD_OSEQJPGTIME),
            App.dwTime,
            InitDisplayTiming);

    return ( TRUE );
}



/******************************************************************************
 *  void InitPicParm(PIC_PARM PICFAR* p, const PIC_PARM *pSrcPicParm, LPCTRLINITDATA ar)
 *
 *  initialize PIC_PARM from default options settings
 *
 *  parameters:
 *      p           - PIC_PARM to init
 *      pSrcPicParm - pointer to PegasusQuery picparm for source image
 *      ar          - image default options settings array
 ******************************************************************************/
static void InitPicParm(PIC_PARM PICFAR* p, const PIC_PARM PICFAR* pSrcPicParm, LPCTRLINITDATA ar)
{
    char sz[_MAX_PATH + 1];
    WORD wSecondary;
    WORD wPrimary;
    WORD wImageLeft;
    WORD wImageTop;
    WORD wImageWidth;
    WORD wImageHeight;

    /* image password -- if one is required and this is null or wrong, we'll get an
        error and an opportunity to correct it.  If one is not required and this is
        not null, we don't get an error from Pegasus() and we don't try to detect it
        on our own */
    CtrlDataGetText(ar, IDC_OSEQJPGOPT_EDIT_PASSWORD, sz, sizeof(sz));
    _fmemset(p->KeyField, '\0', sizeof(p->KeyField));
    hmemcpy(p->KeyField, sz, sizeof(p->KeyField));

    /* image size is independent of the other settings */
    switch ( CtrlDataWhichRadio(ar, IDC_OSEQJPGOPT_RADIO_FULLSIZE) )
        {
        case IDC_OSEQJPGOPT_RADIO_FULLSIZE:
            p->u.J2D.Thumbnail = THUMB_NONE;
            break;
        case IDC_OSEQJPGOPT_RADIO_FOURTH:
            p->u.J2D.Thumbnail = THUMB_4;
            break;
        case IDC_OSEQJPGOPT_RADIO_SIXTEENTH:
            p->u.J2D.Thumbnail = THUMB_16;
            break;
        case IDC_OSEQJPGOPT_RADIO_SIXTYFOURTH:
            p->u.J2D.Thumbnail = THUMB_64;
            break;
        default:
            assert(FALSE);
            break;
        }
        
    /* cross-block smoothing is also independent of the other settings */
    if ( !CtrlDataIsPushed(ar, IDC_OSEQJPGOPT_CHECK_ENHANCEDDECOMPRESS) )
        p->u.J2D.PicFlags |= PF_NoCrossBlockSmoothing;

    /* the remaining settings depend on DIB size */
    switch ( CtrlDataWhichRadio(ar, IDC_OSEQJPGOPT_RADIO_32BITS) )
        {
        case IDC_OSEQJPGOPT_RADIO_32BITS:
            p->u.J2D.DibSize = 32;
            break;
        case IDC_OSEQJPGOPT_RADIO_24BITS:
            p->u.J2D.DibSize = 24;
            break;
        case IDC_OSEQJPGOPT_RADIO_16BITS:
            p->u.J2D.DibSize = 16;
            break;
        case IDC_OSEQJPGOPT_RADIO_8BITS:
            p->u.J2D.DibSize = 8;
            break;
        case IDC_OSEQJPGOPT_RADIO_4BITS:
            p->u.J2D.DibSize = 4;
            break;
        default:
            assert(FALSE);
            break;            
        }

    wImageLeft   = (WORD)CtrlDataGetLong(ar, IDC_OSEQJPGOPT_EDIT_IMAGELEFT, FALSE);
    wImageTop    = (WORD)CtrlDataGetLong(ar, IDC_OSEQJPGOPT_EDIT_IMAGETOP, FALSE);
    wImageWidth  = (WORD)CtrlDataGetLong(ar, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH, FALSE);
    wImageHeight = (WORD)CtrlDataGetLong(ar, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT, FALSE);

    if ( wImageLeft != 0 || wImageTop != 0 || wImageWidth != 0 || wImageHeight != 0 )
        {
        if ( wImageLeft >= (WORD)pSrcPicParm->Head.biWidth )
            wImageLeft = (WORD)( pSrcPicParm->Head.biWidth - 1 );
        if ( wImageTop >= (WORD)labs(pSrcPicParm->Head.biHeight) )
            wImageTop = (WORD)( labs(pSrcPicParm->Head.biHeight) - 1 );
        if ( wImageWidth == 0 )
            wImageWidth = (WORD)pSrcPicParm->Head.biWidth;
        if ( wImageHeight == 0 )
            wImageHeight = (WORD)labs(pSrcPicParm->Head.biHeight);
        p->CropXoff   = wImageLeft;
        p->CropYoff   = wImageTop;
        p->CropWidth  = wImageWidth;
        p->CropHeight = wImageHeight;
        p->Flags |= F_Crop;
        }

    if ( p->u.J2D.DibSize > 8 )
        {
        if ( p->u.J2D.DibSize == 16 && CtrlDataIsPushed(ar, IDC_OSEQJPGOPT_CHECK_DITHER) )
            // dither makes sense for 16-bit also
            p->u.J2D.PicFlags |= PF_Dither;

        /* in theory our image options box has prevented this.  If there isn't a bug,
            then the user's been messing with the ini file */
        assert(!CtrlDataIsPushed(ar, IDC_OSEQJPGOPT_CHECK_MAKEGRAY));
        }
    else /* p->u.J2D.DibSize <= 8 */
        {
        /* dither is independent of gray or color choice */
        if ( CtrlDataIsPushed(ar, IDC_OSEQJPGOPT_CHECK_DITHER) )
            p->u.J2D.PicFlags |= PF_Dither;

        if ( CtrlDataIsPushed(ar, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) )
            {
            /* the dib will be grayscale */
            /*#### TODO: use palette buttons to determine whether to make grays, use
                the embedded palette or use an external palette.  Even though it's a
                grayscale image, all the palette considerations apply. */
            p->u.J2D.PicFlags |= PF_ConvertGray;
            p->u.J2D.GraysToMake = CtrlDataGetLong(ar, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, FALSE);
            }
        else if ( pSrcPicParm->Head.biBitCount > 8 )
            {
            /* color choices only matter for a color source image and if we
                aren't making a grayscale dib */
            switch ( CtrlDataWhichRadio(ar, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) )
                {
                case IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE:
                    wPrimary = (WORD)pSrcPicParm->Head.biClrImportant;
                    wSecondary = (WORD)( pSrcPicParm->Head.biClrUsed - pSrcPicParm->Head.biClrImportant );
                    if ( wPrimary < 2 || wPrimary > ( 1U << (WORD)p->u.J2D.DibSize ) )
                        {
                        assert(wSecondary >= 2 && wSecondary <= ( 1U << (WORD)p->u.J2D.DibSize ));
                        p->Head.biClrUsed = wSecondary;
                        p->Head.biClrImportant = wSecondary;
                        hmemcpy(
                            p->ColorTable,
                            &pSrcPicParm->ColorTable[wSecondary],
                            wSecondary * sizeof(RGBQUAD));
                        }
                    else
                        {
                        p->Head.biClrUsed = wPrimary;
                        p->Head.biClrImportant = wPrimary;
                        hmemcpy(
                            p->ColorTable,
                            pSrcPicParm->ColorTable,
                            wPrimary * sizeof(RGBQUAD));
                        }
                    /* if source doesn't have a suitable palette, then we're here because of a bug */
                    break;

                case IDC_OSEQJPGOPT_RADIO_MAKEPALETTE:
                    p->u.J2D.PicFlags |=
                        PF_MakeColors | PF_CreateDibWithMadeColors | PF_OnlyUseMadeColors;
                    /* PF_CreateDibWithMadeColors, else Pegasus won't return a dib
                        PF_OnlyUseMadeColors, else Pegasus will return two dibs -- the first
                        with the supplied palette (PIC_PARM.ColorTable), and then starting
                        after RES_COLORS_MADE, the second with the optimal palette */
                    p->u.J2D.PrimClrToMake =
                        CtrlDataGetLong(ar, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE, FALSE);
                    break;

                case IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE:
                    CtrlDataGetText(ar, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
                    p->Head.biClrUsed =
                        MiscPaletteColors(sz, p->ColorTable, ( 1 << (unsigned)p->u.J2D.DibSize) );
                    p->Head.biClrImportant = p->Head.biClrUsed;
                    assert(p->Head.biClrUsed != 0);
                    /* if external palette file doesn't have a suitable palette, then we're here
                        because of a bug */
                    break;

                default:
                    assert(FALSE);
                    break;                    
                }
            }
        /* else pSrcPicParm->Head.biBitCount <= 8, the source jpeg is gray and color choices
            don't matter */
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
    RESPONSE response;
    APPDATA PICFAR* pApp = (APPDATA PICFAR*)p->App;
    CHAR PICHUGE* pb;
    
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
                        if ( ( p->u.J2D.PicFlags & PF_YieldGet ) != 0 )
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
                
            case RES_ALLOCATE_APP2_BUF:
                /* Pegasus() encountered an app2 field,
                    currently this only occurs during REQ_INIT -- since I don't know
                    that it _has_ to, I've included the code in REQ_EXEC also */
                pApp->dwTime[ALLOC] -= MiscTickCount();
                if ( !MiscAllocateComment(
                        p->u.J2D.AppFieldLen,
                        (CHAR PICHUGE* PICFAR*)&p->u.J2D.AppField,
                        &p->u.J2D.AppFieldSize) )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[ALLOC] += MiscTickCount();
                break;

            case RES_HAVE_COMMENT:
                break;

            case RES_ALLOCATE_COMMENT_BUF:
                /* Pegasus() encountered a comment
                    currently this only occurs during REQ_INIT -- since I don't know
                    that it _has_ to, I've included the code in REQ_EXEC also */
                pApp->dwTime[ALLOC] -= MiscTickCount();
                if ( !MiscAllocateComment(
                        p->CommentLen,
                        &p->Comment,
                        &p->CommentSize ) )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[ALLOC] += MiscTickCount();
                break;

            case RES_EXTEND_PIC2LIST:
                if ( p->PIC2List == 0 )
                    pb = GlobalAllocPtr(GMEM_MOVEABLE, p->PIC2ListLen);
                else
                    pb = GlobalReAllocPtr(p->PIC2List, p->PIC2ListLen, GMEM_MOVEABLE);
                if ( pb == 0 )
                {
                    ErrorMessage(STYLE_ERROR, IDS_SAVEOUTOFMEMORY);
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                }
                p->PIC2List = pb;
                p->PIC2ListSize = p->PIC2ListLen;
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
                MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, p->u.J2D.StripSize);
                pApp->dwTime[MEMCPY] += MiscTickCount();
                if ( ( p->u.J2D.PicFlags & PF_YieldPut ) != 0 )
                    {
                    pApp->dwTime[PAINT] -= MiscTickCount();
                    lHeight = p->Head.biHeight * p->u.J2D.WidthPad - pApp->dwPutSize;
                    lHeight /= p->u.J2D.WidthPad;
                    if ( ( p->Put.QFlags & Q_REVERSE ) != 0 )
                        lHeight = -lHeight;
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
                        if ( ( p->u.J2D.PicFlags & PF_YieldGet ) != 0 )
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
                
            case RES_COLORS_MADE:
                /* at this point, if we are making colors, we have the color table
                    we need to create the image palette and can start displaying
                    the image as we receive dib strips */
                ChildInvalidatePalette(pApp->hWnd);
                break;

            case RES_ALLOCATE_APP2_BUF:
                /* Pegasus() encountered an app2 field,
                    currently this only occurs during REQ_INIT -- since I don't know
                    that it _has_ to, I've included the code in REQ_EXEC also */
                pApp->dwTime[ALLOC] -= MiscTickCount();
                if ( !MiscAllocateComment(
                        p->u.J2D.AppFieldLen,
                        (CHAR PICHUGE* PICFAR*)&p->u.J2D.AppField,
                        &p->u.J2D.AppFieldSize) )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[ALLOC] += MiscTickCount();
                break;

            case RES_ALLOCATE_COMMENT_BUF:
                /* Pegasus() encountered a comment
                    currently this only occurs during REQ_INIT -- since I don't know
                    that it _has_ to, I've included the code in REQ_EXEC also */
                pApp->dwTime[ALLOC] -= MiscTickCount();
                if ( !MiscAllocateComment(
                        p->CommentLen,
                        &p->Comment,
                        &p->CommentSize) )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                pApp->dwTime[ALLOC] += MiscTickCount();
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
        MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutSize, &p->Put, p->u.J2D.StripSize);
        pApp->dwTime[MEMCPY] += MiscTickCount();
        }

    return ( response );                       
}



/******************************************************************************
 *  BOOL ExtractDibOpenSequentialJpeg(
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
static BOOL ExtractDibOpenSequentialJpeg(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm)
{
    RESPONSE response;
    PIC_PARM PICFAR* pp = pDibPicParm;
    PIC_PARM SavePicParm;
    APPDATA App;
    
    // nothing valid yet
    *ppDib = 0;
    *pdwDibLen = 0;

    /**************************************************************************
        Initialize PIC_PARM data
    */

    /* initialize the pic_parm elements independent of image options settings */
    _fmemset(pp, 0, sizeof(*pp));     /* _VERY_ important */
    pp->ParmSize     = sizeof(*pp);
    pp->ParmVer      = CURRENT_PARMVER;  /* from PIC.H */
    pp->ParmVerMinor = 1;
    if ( OpenDefSequentialJpeg.nFoundParmVer == 0 )
        pp->Op = OP_SE2D;       /* Enhanced Sequential JPEG to DIB */
    else
        pp->Op = OP_S2D;        /* Sequential JPEG to DIB */

    memset(&App, 0, sizeof(App));
    pp->App = (LPARAM)(APPDATA PICFAR*)&App;

    /* the only image parameter we need to set is the desired DibSize.  Everything
        else is ok as 0'ed above */
    pp->u.J2D.DibSize = pSrcPicParm->Head.biBitCount;

    hmemcpy(pp->KeyField, pSrcPicParm->KeyField, sizeof(pp->KeyField));
    pp->u.J2D.PicFlags |= PF_AllocateComment;
        /* PF_AllocateComment allows us to be notified if there is a comment (or
            application data) and, only at that time and after we know how big the
            comment is, allocate space for the comment */
    pp->u.J2D.AppFieldSize = -1;
        /* NOTE: this is required in order to be notified that there is application data
            so we can allocate space knowing exactly how much is required.  Currently,
            Pegasus ignores PF_AllocateComment for the AppField in spite of PIC.H's comments.
            I expect that Pegasus eventually will not require this, but also that this
            will be allowed without hurting anything */

    pp->u.J2D.PicFlags |= PF_YieldGet | PF_YieldPut;    /* good windows behavior */
                                                         
    /* initialize source image queue pointers */
    pp->Get.Start  = (BYTE PICHUGE*)pSrc;
        /* cast away constness to eliminate the warning --
            note however that this is not safe in general with Pegasus, or as a general
            practice, (although it is currently safe with Sequential JPEG expand). */
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
    while ( ( response = PegasusReqInit(pp) ) == RES_ERR )
        {
        CleanupOpenSequentialJpeg(0, pp, 0);
        if ( pp->Status == ERR_MISSING_KEY || pp->Status == ERR_INVALID_KEY )
            {
            if ( !MiscGetPassword(hwndFrame, (LPSTR)SavePicParm.KeyField) )
                return ( FALSE );
            }
        else
            {
            if ( pp->Status != ERR_NONE )
                ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
            /* "The image could not be expanded." */
            return ( FALSE );
            }            
        *pp = SavePicParm;
        }

    /**************************************************************************
        Allocate output DIB buffer according to the height and widthpad (scan line
        width including dword padding) returned from REQ_INIT
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    *pdwDibLen = pp->Head.biHeight * pp->u.J2D.WidthPad;
    *ppDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwDibLen, IDS_IMAGEOUTOFMEMORY);
    if ( *ppDib == 0 )
        {
        Pegasus(pp, REQ_TERM);
        CleanupOpenSequentialJpeg(0, pp, 0);
        return ( FALSE );
        }

    /* initialize put queue buffer pointers */
    pp->Put.Start = *ppDib;            
    pp->Put.End = pp->Put.Start + *pdwDibLen;

    if ( pSrcPicParm->Head.biHeight < 0 )
        {
        pp->Put.Front = pp->Put.End;
        pp->Put.QFlags |= Q_REVERSE; /* this is the usual case */
        /* ordinarily the dib scan line highest on the screen is the final scan line
            in the dib buffer.  PegasusQuery returns biHeight < 0 for that dib and
            we have to tell Pegasus that sense of the buffer is reversed (top is last).
            Thus the Q front (which is the first data returned by Pegasus, which is the first
            scan line since that's the scan line earliest in a JPEG file) is set to
            the End of the dib buffer and Q_REVERSE is set so Pegasus knows to put
            data in the buffer moving from the Front towards the start of the buffer */
        }
    else
        {
        /* apparently the JPEG image is reversed from what it's supposed to be so that
            the first scan line processed will be the bottom scan line in the resulting
            dib image */
        pp->Put.Front = pp->Put.Start;
        }        
    pp->Put.Rear = pp->Put.Front;         /* initially, nothing has been put in the queue */

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */

    response = PegasusReqExec(pp);
    if ( response == RES_ERR )
        {
        CleanupOpenSequentialJpeg(*ppDib, pp, 0);
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
 *  BOOL OpenSequentialJpeg(
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
static BOOL OpenSequentialJpeg(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    char sz[_MAX_PATH + 1];
    int nColors;
    long lPrimaryColors;
    long lSecondaryColors;
    DWORD dwOpFlags = *(LPDWORD)ppOpenData; // see OpenImageIntoWindow() in mopen.c
    LPCTRLINITDATA pCtrl;

    /* read default ini image options settings */
    if ( !GetCtrlDefaults((LPCTRLINITDATA PICFAR*)ppOpenData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_IMAGEOUTOFMEMORY);
        /* "There was not enough memory to open the image." */
        return ( FALSE );
        }        

    // if running silent, override the ini settings for display of timing data
    if ( ( dwOpFlags & OP_SILENT ) != 0 )
        {
        pCtrl = CtrlData(*ppOpenData, IDD_TIMING);
        assert(pCtrl != 0 );
        if ( pCtrl != 0 )        
            CtrlDataSetPushed(
                (LPCTRLINITDATA)pCtrl->pszText,
                IDC_TIMING_CHECK_DISPLAYTIMING,
                FALSE);
        }

    /* if source image is grayscale, don't let the user change color palette choices
        in the image options dialog box */
    if ( pSrcPicParm->Head.biBitCount == 8 )
        {
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_STATIC_COLORPALETTE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_32BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_24BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_16BITS);

        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE);
        
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_STATIC_COLORS);
        
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_STATIC_EXTERNALPALETTE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_BUTTON_BROWSE);

        /* and make sure either 4-bit or 8-bit bit depth is pushed and
            that make gray is pushed */
        if ( !CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_4BITS) )
            CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_8BITS, TRUE);
        CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_CHECK_MAKEGRAY, TRUE);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_CHECK_MAKEGRAY);
        }
    else
        {
        /* if it's a color source image, but there's no embedded palette,
            don't let the user select "Embedded palette" in color palette image options */
        if ( pSrcPicParm->Head.biClrUsed == 0 )
            CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE);

        if ( ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_8BITS) ||
               CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_4BITS) ) &&
             !CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) )
            {
            if ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_8BITS) )
                nColors = 256;
            else
                nColors = 16;
            /* if the image needs a color palette
                and embedded palette is selected, but there is no suitable embedded palette
                or external palette is selected, but the file doesn't have a suitable palette,
                then report the error to the user and set make palette for this image */
            CtrlDataGetText(*ppOpenData, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            lPrimaryColors = pSrcPicParm->Head.biClrImportant;
            lSecondaryColors = pSrcPicParm->Head.biClrUsed - pSrcPicParm->Head.biClrImportant;
            if ( ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) &&
                   ( lPrimaryColors < 2 || lPrimaryColors > nColors ) &&
                   ( lSecondaryColors < 2 || lSecondaryColors > nColors ) ) ||
                 ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE) &&
                   MiscPaletteColors(sz, 0, nColors) == 0 ) )
                {
                if ( ( dwOpFlags & OP_SILENT ) == 0 )
                    ErrorMessage(STYLE_ERROR, IDS_IMAGECOLORPALETTE);
                /* "The image default Color Palette option is not suitable. "
                    "An optimal color palette will be made." */
                CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE, TRUE);
                CtrlDataSetLong(*ppOpenData, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE, nColors, FALSE);
                }
            }
        if ( CtrlDataIsInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) &&
             CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) )
            CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE, TRUE);
        }                            

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
 *  BOOL OptionsDialogOpenSequentialJpeg(
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
 *
 *  notes:
 *      The processing specific to the Open Sequential Jpeg image options is contained
 *      in the WM_COMMAND handler OnCmdOSeqJpgOpt.  See also mdlg.c and mdlgctrl.c.
 *
 *      The Apply button may have refreshed the image before DialogDoModal returns.
 *      Apply'ed changes can't be cancelled.  IDOK is returned by DialogDoModal only
 *      if the OK button is pressed and some image options settings have changed since
 *      the last Apply or since the dialog was opened.  Otherwise, if the OK button
 *      is pushed, then IDOK_UNCHANGED is returned and we know we don't have to
 *      refresh the image.
 ******************************************************************************/
static BOOL OptionsDialogOpenSequentialJpeg(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
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

    oud.hWnd        = hWnd;
    oud.ppSrc       = (BYTE PICHUGE* PICFAR*)&pSrc,
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = ppDib;
    oud.pdwDibLen   = pdwDibLen;
    oud.pDibPicParm = pDibPicParm;
    oud.bTested     = FALSE;

    result = DlgDoModal(hwndFrame, MAKEINTRESOURCE(IDD_OSEQJPGOPT), ImageOnCommand, *ppData, &oud);
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
                CleanupOpenSequentialJpeg(pOldDib, &OldDibPicParm, 0);
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
    if ( CtrlDataIsPushed(*ppData, IDC_OSEQJPGOPT_CHECK_SETDEFAULT) )
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
        CleanupOpenSequentialJpeg(pOldDib, &OldDibPicParm, 0);
        }
    return ( TRUE );
}



/******************************************************************************
 *  void DefaultsDialogOpenSequentialJpeg(void)
 *
 *  default image options dialog
 * 
 *  notes:
 *      The processing specific to the Open Sequential Jpeg image options is contained
 *      in the WM_COMMAND handler OnCmdOSeqJpgOpt.  See also mdlg.c and mdlgctrl.c.
 *
 *      The Apply button may have saved ini settings before DialogDoModal returns.
 *      Apply'ed changes can't be cancelled.  IDOK is returned by DialogDoModal only
 *      if the OK button is pressed and some image options settings have changed since
 *      the last Apply or since the dialog was opened.  Otherwise, if the OK button
 *      is pushed, then IDOK_UNCHANGED is returned and we know we don't have to
 *      save the settings to the ini file.
 ******************************************************************************/
static void DefaultsDialogOpenSequentialJpeg(void)
{
    int result;
    LPCTRLINITDATA pOpenData;
    
    if ( !GetCtrlDefaults(&pOpenData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return;
        }
    CtrlDataSetPushed(pOpenData, IDC_OSEQJPGOPT_CHECK_SETDEFAULT, TRUE);
    CtrlDataSetInactive(pOpenData, IDC_OSEQJPGOPT_CHECK_SETDEFAULT);
    CtrlDataSetInactive(pOpenData, IDTEST);
    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_OSEQJPGOPT),
        DefaultsOnCommand,
        pOpenData,
        NULL);
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
 *  void PropertiesDialogOpenSequentialJpeg
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
static void PropertiesDialogOpenSequentialJpeg(
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
    char sz[_MAX_PATH + sizeof(" Sequential JPEG Properties")];
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
    CtrlDataSetText(pInitData, IDC_SEQJPGPROP_FILENAME, sz);
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_FILESIZE, dwSrcLen, FALSE);
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_IMAGEWIDTH_SOURCE, pSrcPicParm->Head.biWidth, FALSE);
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_IMAGEHEIGHT_SOURCE, labs(pSrcPicParm->Head.biHeight), FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_BITSPERPIXEL_SOURCE,
        pSrcPicParm->Head.biBitCount,
        FALSE);
    dwSrcWidthPad = ( pSrcPicParm->Head.biWidth * pSrcPicParm->Head.biBitCount + 7 ) / 8;
    dwSrcWidthPad = ( ( dwSrcWidthPad + sizeof(DWORD) - 1 ) / sizeof(DWORD) ) * sizeof(DWORD);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_IMAGESIZE_SOURCE,
        dwSrcWidthPad * labs(pSrcPicParm->Head.biHeight),
        FALSE);
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_COLORTABLESIZE_SOURCE, pSrcPicParm->Head.biClrUsed, FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_NUMBERIMPORTANT_SOURCE,
        pSrcPicParm->Head.biClrImportant,
        FALSE);

    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_IMAGEWIDTH_DISPLAYED,
        pDibPicParm->Head.biWidth,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_IMAGEHEIGHT_DISPLAYED,
        pDibPicParm->Head.biHeight,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_BITSPERPIXEL_DISPLAYED,
        pDibPicParm->Head.biBitCount,
        FALSE);
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_IMAGESIZE_DISPLAYED, dwDibLen, FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_COLORTABLESIZE_DISPLAYED,
        pDibPicParm->Head.biClrUsed,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_NUMBERIMPORTANT_DISPLAYED,
        pDibPicParm->Head.biClrImportant,
        FALSE);

    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_PICVERSION, pSrcPicParm->PicVer, FALSE);
    CtrlDataSetText(
        pInitData,
        IDC_SEQJPGPROP_VISUALORIENTATION,
        MiscVisualOrient(pSrcPicParm->VisualOrient));

    dwSrcBmpSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pSrcPicParm->CommentLen +
        dwSrcWidthPad * labs(pSrcPicParm->Head.biHeight),
    
    CtrlDataSetLong(
        pInitData,
        IDC_SEQJPGPROP_COMPRESSIONRATIO,
        ( dwSrcBmpSize + dwSrcLen / 2 ) / dwSrcLen,
        FALSE);
    CtrlDataGetText(pInitData, IDC_SEQJPGPROP_COMPRESSIONRATIO, sz, sizeof(sz));
    strcat(sz, ":1");
    CtrlDataSetText(pInitData, IDC_SEQJPGPROP_COMPRESSIONRATIO, sz);

    if ( pSrcPicParm->Head.biCompression == BI_JPGE ||
         pSrcPicParm->Head.biCompression == BI_PC2E )
        CtrlDataSetText(pInitData, IDC_SEQJPGPROP_ENTROPYCODING, "Els Arithmetic Entropy Coding");
    else
        CtrlDataSetText(pInitData, IDC_SEQJPGPROP_ENTROPYCODING, "Huffman");
        
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_LUMINANCE, pDibPicParm->u.J2D.LumFactor, FALSE);
    CtrlDataSetLong(pInitData, IDC_SEQJPGPROP_CHROMINANCE, pDibPicParm->u.J2D.ChromFactor, FALSE);
    switch ( pDibPicParm->u.J2D.SubSampling )
        {
        case 0:  lstrcpy(sz, "1/1/1");   break;
        case 1:  lstrcpy(sz, "2/1/1");   break;
        case 2:  lstrcpy(sz, "4/1/1");   break;
        default: lstrcpy(sz, "unknown"); break;
        }
    CtrlDataSetText(pInitData, IDC_SEQJPGPROP_SUBSAMPLING, sz);
    if ( pDibPicParm->CommentLen != 0 )
        {
          LPSTR psz = (LPSTR)MiscGlobalAllocPtr(pDibPicParm->CommentLen, IDS_PROPERTIESOUTOFMEMORY);
        if ( psz == 0 )
            {
            CtrlDataFree(&pInitData);
            return;
            }
        lstrcpyn(psz, (LPCSTR)pDibPicParm->Comment, MAX_COMMENT + 1);
        CtrlDataSetText(pInitData, IDC_SEQJPGPROP_COMMENT, psz);
        MiscGlobalFreePtr(&psz);
        }
    if ( pDibPicParm->u.J2D.AppFieldLen != 0 )
        {
        LPSTR psz = (LPSTR)MiscGlobalAllocPtr(pDibPicParm->u.J2D.AppFieldLen, IDS_PROPERTIESOUTOFMEMORY);
        if ( psz == 0 )
            {
            CtrlDataFree(&pInitData);
            return;
            }
        lstrcpyn(psz, (LPSTR)pDibPicParm->u.J2D.AppField, MAX_COMMENT + 1);
        CtrlDataSetText(pInitData, IDC_SEQJPGPROP_APP2, psz);
        MiscGlobalFreePtr(&psz);
        }

    /**************************************************************************
        Display the properties dialog
    */
    GetWindowText(hWnd, sz, sizeof(sz) - ( sizeof(" Sequential JPEG Properties") - 1 ));
    strcat(sz, " Sequential JPEG Properties");
    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_SEQJPGPROP),
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
 * void CleanupOpenSequentialJpeg(
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
static void CleanupOpenSequentialJpeg(
    BYTE PICHUGE *pDib,
    PIC_PARM PICFAR *pDibPicParm,
    VOID PICFAR* pOpenData)
{
    MiscGlobalFreePtr(&pDib);
    if ( pDibPicParm != 0 )
        {
        MiscGlobalFreePtr(&pDibPicParm->Comment);
        MiscGlobalFreePtr(&pDibPicParm->PIC2List);
        MiscGlobalFreePtr(&pDibPicParm->u.J2D.AppField);
        }
    CtrlDataFree((LPCTRLINITDATA PICFAR*)&pOpenData);
}



/******************************************************************************
 *  BOOL DupPicParmOpenSequentialJpeg(const PIC_PARM PICFAR* pSrc, PIC_PARM PICFAR* pDup);
 *
 *  duplicate picparm for new window
 *
 *  parameters:
 *      pSrc - picparm to be duped
 *      pDup - resulting duplicate picparm
 *
 *  notes:
 *      This function exists because nobody but the responsible opcode knows
 *      how to duplicate any picparm fields allocated as the image is expanded.
 *      App2 data, for example, is part of the dib_output union variant.
 ******************************************************************************/
static BOOL DupPicParmOpenSequentialJpeg(const PIC_PARM PICFAR* pSrc, PIC_PARM PICFAR* pDup)
{
    *pDup = *pSrc;
    if ( pSrc->PIC2List != 0 )
        {
        pDup->PIC2List = MiscGlobalAllocPtr(pDup->PIC2ListLen, 0);
        if ( pDup->PIC2List == 0 )
            return ( FALSE );
        hmemcpy(pDup->PIC2List, pSrc->PIC2List, pDup->PIC2ListLen);
        }
    if ( pSrc->Comment != 0 )
        {
        pDup->Comment = MiscGlobalAllocPtr(pDup->CommentSize, 0);
        if ( pDup->Comment == 0 )
            return ( FALSE );
        hmemcpy(pDup->Comment, pSrc->Comment, pDup->CommentSize);
        }
    if ( pSrc->u.J2D.AppField != 0 )
        {
        pDup->u.J2D.AppField = (BYTE PICHUGE*)
            MiscGlobalAllocPtr(pDup->u.J2D.AppFieldSize, 0);
        if ( pDup->u.J2D.AppField == 0 )
            {
            MiscGlobalFreePtr(&pDup->Comment);
            return ( FALSE );
            }
        hmemcpy(pDup->u.J2D.AppField, pSrc->u.J2D.AppField, pDup->u.J2D.AppFieldSize);
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL GetCtrlDefaults(LPCTRLINITDATA PICFAR* ppOpenData)
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
static BOOL GetCtrlDefaults(LPCTRLINITDATA PICFAR* ppOpenData)
{
    int num;

    if ( !CtrlDataDup(InitOptionsData, ppOpenData) )
        return ( FALSE );
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szOptionsSection, *ppOpenData);

    /* if screen bit depth is <= 8, make sure default bits/pixel isn't greater */
    if ( MiscIsPaletteDisplay(&num) )
        {
        /* opcode 82 won't deal with 32 bits or 16 bits so this prevents them
            on a palettized display */
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_32BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_16BITS);
        if ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_32BITS) ||
             CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_16BITS) )
            CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_24BITS, TRUE);
        if ( ToolDefUtility.nFoundParmVer == 0 )
            {
            CtrlDataSetInactive(*ppOpenData, IDC_OSEQJPGOPT_RADIO_24BITS);
            if ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_24BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_4BITS, TRUE);
            if ( num < 8 )                
                CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_4BITS, TRUE);
            }
        else if ( num < 8 )
            {
            if ( CtrlDataIsPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_8BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OSEQJPGOPT_RADIO_4BITS, TRUE);
            }
        }
    return ( TRUE );
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
 *      pUserData   - as defined by OpenSequentialJpeg
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
    char sz[_MAX_PATH + sizeof(" Sequential JPEG Open Options")];
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
        /* OPTIONSUSERDATA defined in MOPCODES.H */
    BYTE PICHUGE *pOldDib;
    DWORD dwOldDibLen;
    PIC_PARM OldDibPicParm;

    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:    /* for WM_INITDIALOG - the virtue is it saves us having to have a separarte
                        function passed to our dialog box handler */
            GetWindowText(poud->hWnd, sz, sizeof(sz) - ( sizeof(" Sequential JPEG Open Options") - 1 ));
            strcat(sz, " Sequential JPEG Open Options");
            SetWindowText(hwndDlg, sz);
            ImageGrayedCtrls(hwndDlg, arOpenData, poud);
            break;
            
        case IDC_OSEQJPGOPT_CHECK_MAKEGRAY:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* MakeGray is not an auto button (we have to check and uncheck it,
                    because we don't want it to be uncheckable when the source image
                    is gray scale -- but we don't want to gray it because we want
                    the number of grays choice to be enabled.
                    #### TODO: we need to change the color palette legends so that
                        the options apply to a gray image.  Then we can also lose
                        the # grays box */
                if ( poud->pSrcPicParm->Head.biBitCount != 8 )
                    {
                    /* Toggle the button state unless it's a gray scale source image */
                    CheckDlgButton(
                        hwndDlg,
                        IDC_OSEQJPGOPT_CHECK_MAKEGRAY,
                        !IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY));
                    }                    

                /* make sure the bit depth is 8 bits or 4 bits */
                if ( !IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OSEQJPGOPT_RADIO_32BITS,
                        IDC_OSEQJPGOPT_RADIO_4BITS,
                        IDC_OSEQJPGOPT_RADIO_8BITS);

                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;                

        case IDC_OSEQJPGOPT_RADIO_32BITS:
        case IDC_OSEQJPGOPT_RADIO_24BITS:
        case IDC_OSEQJPGOPT_RADIO_16BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* uncheck make gray button for bit depths > 8 */
                CheckDlgButton(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY, FALSE);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OSEQJPGOPT_RADIO_8BITS:
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

        case IDC_OSEQJPGOPT_RADIO_4BITS:
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
        
        case IDC_OSEQJPGOPT_RADIO_MAKEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;


        case IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OSEQJPGOPT_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* browse for a suitable external palette file.  If the file doesn't have
                    a 16-color palette, and we click 4-bits, then IDOK will report the
                    error */
                if ( MiscPaletteBrowse(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OSEQJPGOPT_CHECK_SETDEFAULT:
            // this prevents modification of this control from setting the modified
            // flag which would a repaint of the image if OK were pushed
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OSEQJPGOPT);
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
                    CleanupOpenSequentialJpeg(pOldDib, &OldDibPicParm, 0);
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
 *      pUserData   - as defined by OpenSequentialJpeg
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
            
        case IDC_OSEQJPGOPT_CHECK_MAKEGRAY:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* MakeGray is not an auto button (we have to check and uncheck it,
                    because we don't want it to be uncheckable when the source image
                    is gray scale -- but we don't want to gray it because we want
                    the number of grays choice to be enabled.
                    #### TODO: we need to change the color palette legends so that
                        the options apply to a gray image.  Then we can also lose
                        the # grays box */
                /* Toggle the button state */
                CheckDlgButton(
                    hwndDlg,
                    IDC_OSEQJPGOPT_CHECK_MAKEGRAY,
                    !IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY));

                /* make sure the bit depth is 8 bits or 4 bits */
                if ( !IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OSEQJPGOPT_RADIO_32BITS,
                        IDC_OSEQJPGOPT_RADIO_4BITS,
                        IDC_OSEQJPGOPT_RADIO_8BITS);

                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;                

        case IDC_OSEQJPGOPT_RADIO_32BITS:
        case IDC_OSEQJPGOPT_RADIO_24BITS:
        case IDC_OSEQJPGOPT_RADIO_16BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* uncheck make gray button for bit depths > 8 */
                CheckDlgButton(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY, FALSE);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OSEQJPGOPT_RADIO_8BITS:
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

        case IDC_OSEQJPGOPT_RADIO_4BITS:
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
        
        case IDC_OSEQJPGOPT_RADIO_MAKEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;


        case IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OSEQJPGOPT_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* browse for a suitable external palette file.  If the file doesn't have
                    a 16-color palette, and we click 4-bits, then IDOK will report the
                    error */
                if ( MiscPaletteBrowse(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OSEQJPGOPT);
            break;

        case IDOK:
            return ( DefaultOptionsValid(hwndDlg) );

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
     WORD wImageLeft;
     WORD wImageTop;
         
    wImageLeft = (WORD)GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT, &fValid, FALSE);
    if ( ( !fValid &&
           GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT, sz, sizeof(sz)) != 0 ) ||
         ( fValid && wImageLeft >= (WORD)poud->pSrcPicParm->Head.biWidth ) )
        {
        ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 0, poud->pSrcPicParm->Head.biWidth - 1);
        /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT), 0, -1);
        return ( FALSE );
        }
    wImageTop  = (WORD)GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP,  &fValid, FALSE);
    if ( ( !fValid &&
           GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP, sz, sizeof(sz)) != 0 ) ||
         ( fValid && wImageTop >= (WORD)labs(poud->pSrcPicParm->Head.biHeight) ) )
        {
        ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 0, labs(poud->pSrcPicParm->Head.biHeight) - 1);
        /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP), 0, -1);
        return ( FALSE );
          }
     GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH,  &fValid, FALSE);
     if ( !fValid && GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH, sz, sizeof(sz)) != 0 )
          {
          ErrorMessage(
                STYLE_ERROR,
                IDS_INVALIDNUM,
                1,
                poud->pSrcPicParm->Head.biWidth - wImageLeft);
        /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH), 0, -1);
        return ( FALSE );
        }
     GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT,  &fValid, FALSE);
     if ( !fValid && GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT, sz, sizeof(sz)) != 0 )
        {
        ErrorMessage(
            STYLE_ERROR,
            IDS_INVALIDNUM,
            1,
            labs(poud->pSrcPicParm->Head.biHeight) - wImageTop);
          /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT), 0, -1);
        return ( FALSE );
        }

    if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_8BITS) )
        maxColors = 256;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
        maxColors = 16;
    else
        {
        /* unless the # bits/pixel <= 8, there can't be an inconsistency in the settings
            unless there's a bug */
          assert(!IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) &&
               poud->pSrcPicParm->Head.biBitCount > 8);
        return ( TRUE );
        }

    if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) )
        {
        /* validate number of grays to make */
        n = GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, &fValid, FALSE);
        if ( !fValid || n < 2 || n > maxColors )
            {
            ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
            /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
            SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE), 0, -1);
            return ( FALSE );
            }
        }
    else if ( poud->pSrcPicParm->Head.biBitCount > 8 )
        {
        /* not making grayscale and source image isn't grayscale
            #### TODO: allow grayscale palette choices for gray scale and make gray */
        if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) )
            {
            assert(( poud->pSrcPicParm->Head.biClrImportant >= 2 &&
                     poud->pSrcPicParm->Head.biClrImportant <= (unsigned)maxColors ) ||
                   ( poud->pSrcPicParm->Head.biClrUsed -
                        poud->pSrcPicParm->Head.biClrImportant >= 2 &&
                     poud->pSrcPicParm->Head.biClrUsed -
                        poud->pSrcPicParm->Head.biClrImportant <= (unsigned)maxColors ));
            }
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE) )
                {
            /* validate number of colors to make */
            n = GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE, &fValid, FALSE);
            if ( !fValid || n < 2 || n > maxColors )
                {
                ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
                /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
                SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE), 0, -1);
                return ( FALSE );
                }
            }
        else
            {
                /* make sure external palette actually has a suitable palette */
            assert(IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE));
            GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            n = MiscPaletteColors(sz, 0, maxColors);
            if ( n == 0 )
                {
                SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE), 0, -1);
                return ( FALSE );
                }
            }
        }
    /* else source image is grayscale */
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

     GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT, &fValid, FALSE);
     if ( !fValid && GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT, sz, sizeof(sz)) != 0 )
          {
          ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 0, 9999);
          /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
          SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT));
          Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGELEFT), 0, -1);
          return ( FALSE );
          }
     GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP,  &fValid, FALSE);
     if ( !fValid && GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP, sz, sizeof(sz)) != 0 )
          {
          ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 0, 9999);
          /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
          SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP));
          Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGETOP), 0, -1);
          return ( FALSE );
          }
     GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH,  &fValid, FALSE);
     if ( !fValid && GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH, sz, sizeof(sz)) != 0 )
          {
          ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 1, 9999);
          /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
          SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH));
          Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEWIDTH), 0, -1);
          return ( FALSE );
          }
     GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT,  &fValid, FALSE);
     if ( !fValid && GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT, sz, sizeof(sz)) != 0 )
          {
          ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 1, 9999);
          /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
          SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT));
          Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_IMAGEHEIGHT), 0, -1);
          return ( FALSE );
          }

     if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_8BITS) )
          maxColors = 256;
     else if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
          maxColors = 16;
     else
          return ( TRUE );

    if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) )
        {
        /* validate number of grays to make */
        n = GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, &fValid, FALSE);
        if ( !fValid || n < 2 || n > maxColors )
            {
            ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
            /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
            SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE), 0, -1);
            return ( FALSE );
            }
        }
    else
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE) )
            {
            /* validate number of colors to make */
            n = GetDlgItemInt(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE, &fValid, FALSE);
            if ( !fValid || n < 2 || n > maxColors )
                {
                ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
                /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
                SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE), 0, -1);
                return ( FALSE );
                }
            }
        else if ( !IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) )
            {
            /* make sure external palette actually has a suitable palette */
            assert(IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE));
            GetDlgItemText(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            n = MiscPaletteColors(sz, 0, maxColors);
            if ( n == 0 )
                {
                SetFocus(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE), 0, -1);
                return ( FALSE );
                }
            }
        }
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
    long lColors;
    long lSecondaryColors;
    BOOL fMakeGray = IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY);

    if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_8BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
        {
        /* target bit depth <= 8 */

        /* enable make gray options */
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_CHECK_DITHER, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_STATIC_GRAYS, fMakeGray);

        /* disable bit depths > 8 if makegray is checked,
            if the src image is grayscale, then the openimage initialization
            will have inactivated these settings and the following won't matter
            (or hurt) */
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_32BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_24BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_16BITS, !fMakeGray);

        if ( fMakeGray )
            {
            EnableColorPalette(hwndDlg, ar, FALSE);
            CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_4BITS, TRUE);
            return;
            }
        }
    else
        {
        /* these don't make sense for bit depths > 8 */
        CtrlEnable(
            hwndDlg,
            ar,
            IDC_OSEQJPGOPT_CHECK_DITHER,
            IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_16BITS));
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_STATIC_GRAYS,     FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_4BITS, TRUE);

        EnableColorPalette(hwndDlg, ar, FALSE);
        return;
        }
    
    EnableColorPalette(hwndDlg, ar, TRUE);

    /* for image options dialog for a displayed image,
        make sure embedded palette choices are consistent with the image's palette.
        if the source image doesn't have any palette, then the openimage init will
        have inactivated the embeddedpalette choice so none of this will matter
        (or hurt) */
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
        if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE) )
            {
            if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
                {
                CheckDlgButton(hwndDlg, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                CheckDlgButton(hwndDlg, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE, TRUE);
                SetGraysToMake(hwndDlg);
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, ar, poud);
                return;
                }
            else                
                CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_4BITS, FALSE);
            }
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
            CtrlEnable(hwndDlg, 
                ar,
                IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE,
                FALSE);
        else
            {
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE,
                TRUE);
            CtrlEnable(
                hwndDlg,
                ar,
                IDC_OSEQJPGOPT_RADIO_4BITS,
                TRUE);
            }
        }
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
    BOOL fMakeGray = IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY);
    BOOL fEnableColorPalette;

    if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_8BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
        {
        /* target bit depth <= 8 */

        /* enable make gray options */
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_CHECK_DITHER, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_STATIC_GRAYS, fMakeGray);

        /* disable bit depths > 8 if makegray is checked,
            if the src image is grayscale, then the openimage initialization
            will have inactivated these settings and the following won't matter
            (or hurt) */
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_32BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_24BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_16BITS, !fMakeGray);

        fEnableColorPalette = !fMakeGray;
        }
    else
        {
        /* these don't make sense for bit depths > 8 */
        CtrlEnable(
            hwndDlg,
            ar,
            IDC_OSEQJPGOPT_CHECK_DITHER,
            IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_16BITS));
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_STATIC_GRAYS,     FALSE);

        fEnableColorPalette = FALSE;
        }
    
    EnableColorPalette(hwndDlg, ar, fEnableColorPalette);
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
    CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_STATIC_COLORPALETTE,   bEnabled);

    /* radio buttons */
    CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_EMBEDDEDPALETTE, bEnabled);
    CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE,     bEnabled);
    CtrlEnable(hwndDlg, ar, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE, bEnabled);

    /* make palette options */
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OSEQJPGOPT_STATIC_COLORS,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE));

    /* external palette options */
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OSEQJPGOPT_STATIC_EXTERNALPALETTE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OSEQJPGOPT_EDIT_EXTERNALPALETTE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OSEQJPGOPT_BUTTON_BROWSE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_EXTERNALPALETTE));
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
    if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) )
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_8BITS) )
            SetNumToMake(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, 256);
        else
            {
            assert(IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS));
            SetNumToMake(hwndDlg, IDC_OSEQJPGOPT_EDIT_GRAYSTOMAKE, 16);
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
    if ( !IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_CHECK_MAKEGRAY) &&
         IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_MAKEPALETTE) )
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_8BITS) )
            SetNumToMake(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE, 256);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OSEQJPGOPT_RADIO_4BITS) )
            SetNumToMake(hwndDlg, IDC_OSEQJPGOPT_EDIT_COLORSTOMAKE, 16);
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
