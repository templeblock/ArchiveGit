/******************************************************************************
 *
 * MOUTIL81.C - Minerva MDI utility file to DIB module
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
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "mdlg.h"
#include "mmisc.h"
#include "minerva.h" 
#include "merror.h"
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

extern OPENOP OpenDefBMP;

extern SAVEOP SaveDefBMP80;

extern SAVEOP SaveDefPCX80;

extern SAVEOP SaveDefTIF80;

extern SAVEOP SaveDefTGA80;



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void OpenInitUtility(void);
void OpenInitUtilityPlus(void);
static BOOL OpenBMP(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OpenPCX(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OpenTGA(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OpenTIF(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OpenGIF(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OpenUtility(
    LPCSTR                 pszType,
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL ExtractDibOpenUtility(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm);
static BOOL OptionsDialogOpenUtility(
     LPCSTR                 pszType,
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OptionsDialogOpenBMP(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OptionsDialogOpenPCX(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OptionsDialogOpenTGA(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OptionsDialogOpenTIF(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static BOOL OptionsDialogOpenGIF(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData);
static void PropertiesDialogOpenUtility(
    LPCSTR                 pszType,
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
     const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void PropertiesDialogOpenBMP(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void PropertiesDialogOpenPCX(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
     const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void PropertiesDialogOpenTGA(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void PropertiesDialogOpenTIF(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
     DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void PropertiesDialogOpenGIF(
    HWND                   hWnd,
    LPCSTR                 pszFilename,
    const BYTE PICHUGE*    pSrc,
     DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    const BYTE PICHUGE*    pDib,
    DWORD                  dwDibLen,
    const PIC_PARM PICFAR* pDibPicParm,
    const VOID PICFAR*     pOpenData);
static void DefaultsDialogOpenUtility(LPCSTR pszType);
static void DefaultsDialogOpenBMP(void);
static void DefaultsDialogOpenPCX(void);
static void DefaultsDialogOpenTGA(void);
static void DefaultsDialogOpenTIF(void);
static void DefaultsDialogOpenGIF(void);
static void CleanupOpenUtility(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData);
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
static void EnableColorPalette(HWND hwndDlg, LPCTRLINITDATA ar, BOOL bEnabled);
static void SetGraysToMake(HWND hwndDlg);
static void SetColorsToMake(HWND hwndDlg);
static void SetNumToMake(HWND hwndDlg, int nID, int nNum);
static void ImageGrayedCtrls(HWND hwndDlg, LPCTRLINITDATA ar, OPTIONSUSERDATA PICFAR* poud);
static BOOL ImageOptionsValid(HWND hwndDlg, OPTIONSUSERDATA PICFAR* poud);
static BOOL ImageOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
     LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);
static BOOL ExpandImageInWindow(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR*           pOpenData);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);
static BOOL PropertiesOnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate,
    void PICFAR*    pUserData);



/******************************************************************************
 *  local data
 ******************************************************************************/



/* ini section for image options, %s == "Bitmap", "Targa", "TIFF", "PCX" */
static char szOptionsSection[] = "Open %s Options";
static char szTimingOptionsSection[] = "Open Bitmap Options";	// "bitmap is longest type"

static char szBitsPerPixel[] = "Bits/Pixel";    /* ini key for bits/pixel radio buttons */
static char szColorPalette[] = "Color Palette"; /* ini key for color palette radio buttons */
static char szGIFDisplay[]   = "GIF Interlaced Display";

/* specifying image options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Make Gray",             eCheckBox, IDC_OF2DOPT_CHECK_MAKEGRAY,            FALSE },
    { "Dither",                eCheckBox, IDC_OF2DOPT_CHECK_DITHER,              TRUE  },
    { "GIF Show Transparency", eCheckBox, IDC_OF2DOPT_CHECK_GIFTRANSPARENT,      TRUE  },
    { "Image Settings",	       eCheckBox, IDC_OF2DOPT_CHECK_IMAGESETTINGS,       TRUE  },

    { szBitsPerPixel, eRadioButton, IDC_OF2DOPT_RADIO_32BITS,          FALSE, 0, "32"   },
    { szBitsPerPixel, eRadioButton, IDC_OF2DOPT_RADIO_24BITS,          TRUE,  0, "24"   },
    { szBitsPerPixel, eRadioButton, IDC_OF2DOPT_RADIO_16BITS,          FALSE, 0, "16"   },
    { szBitsPerPixel, eRadioButton, IDC_OF2DOPT_RADIO_8BITS,           FALSE, 0, "8"    },
    { szBitsPerPixel, eRadioButton, IDC_OF2DOPT_RADIO_4BITS,           FALSE, 0, "4"    },
    { szBitsPerPixel, eRadioButton, IDC_OF2DOPT_RADIO_1BITS,           FALSE, 0, "1"    },
    { szColorPalette, eRadioButton, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, TRUE,  0, "Embedded" },
    { szColorPalette, eRadioButton, IDC_OF2DOPT_RADIO_MAKEPALETTE,     FALSE, 0, "Make",    },
    { szColorPalette, eRadioButton, IDC_OF2DOPT_RADIO_EXTERNALPALETTE, FALSE, 0, "External" },

    { szGIFDisplay,   eRadioButton, IDC_OF2DOPT_RADIO_GIFPROGRESSIVE,  TRUE,  0, "Progressive" },
    { szGIFDisplay,   eRadioButton, IDC_OF2DOPT_RADIO_GIFFINAL,        FALSE, 0, "Final" },
    
    { "External Palette Filename",   eEdit, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, FALSE, _MAX_PATH },
    { "Make Gray Grays To Make",     eEdit, IDC_OF2DOPT_EDIT_GRAYSTOMAKE,     FALSE, 3, "256" },
    { "Make Palette Colors To Make", eEdit, IDC_OF2DOPT_EDIT_COLORSTOMAKE,    FALSE, 3, "256" },
	{ "ImageNumber",				 eEdit, IDC_OF2DOPT_EDIT_IMAGENUMBER,	  FALSE, 4, "0" },

    { 0, ePushButton, IDC_OF2DOPT_BUTTON_BROWSE          },
    { 0, ePushButton, IDTEST                             },
    { 0, eGroupBox,   IDC_OF2DOPT_STATIC_COLORPALETTE    },
    { 0, eGroupBox,   IDC_OF2DOPT_STATIC_GIFOPTIONS      },
    { 0, eText,       IDC_OF2DOPT_STATIC_COLORS          },
    { 0, eText,       IDC_OF2DOPT_STATIC_EXTERNALPALETTE },
    { 0, eText,       IDC_OF2DOPT_STATIC_GRAYS           },
    { 0, eCheckBox,   IDC_OF2DOPT_CHECK_SETDEFAULT       },

    /*
     n/a  nested  dialog id    n/a   dialog button      dialog init data
    */
    { 0, ePushButton, IDC_BUTTON_TIMING, FALSE, IDD_TIMING  },
    { szTimingOptionsSection, eDialog, IDD_TIMING, FALSE, IDC_BUTTON_TIMING, (LPSTR)InitTimingData },

	{ 0, eEnd, -1 }
    };
 


/* specifying image properties controls' parameters */
static CTRLINITDATA InitPropertiesData[] =
    {
    { 0, eText, IDC_F2DPROP_FILENAME,                  0, _MAX_PATH },
    { 0, eText, IDC_F2DPROP_FILESIZE,                  0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_FILETYPE,                  0, sizeof("TIFF 2-Dimensional G3") },
    { 0, eText, IDC_F2DPROP_NUMIMAGES,                 0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_IMAGEWIDTH_SOURCE,         0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_IMAGEHEIGHT_SOURCE,        0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_BITSPERPIXEL_SOURCE,       0, 2 },
    { 0, eText, IDC_F2DPROP_IMAGESIZE_SOURCE,          0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_COLORTABLESIZE_SOURCE,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_NUMBERIMPORTANT_SOURCE,    0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_IMAGEWIDTH_DISPLAYED,      0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_IMAGEHEIGHT_DISPLAYED,     0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_BITSPERPIXEL_DISPLAYED,    0, 2 },
    { 0, eText, IDC_F2DPROP_IMAGESIZE_DISPLAYED,       0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_COLORTABLESIZE_DISPLAYED,  0, sizeof("4,000,000,000") },
    { 0, eText, IDC_F2DPROP_NUMBERIMPORTANT_DISPLAYED, 0, sizeof("4,000,000,000") },

    { 0, eEnd, -1 }
    };



#define EXTRACTDIB      0
#define YIELD           1
#define BITDEPTH        2
#define GRAYSCALE       3
#define MAKEPALETTE     4
#define EXTERNALPALETTE 5
#define EMBEDDEDPALETTE 6
#define OPTOTAL         7
#define PAINT           8
#define TOTAL           9



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */
    { 0, eText, IDC_OF2DTIME_TIMERRES,        0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_EXTRACTDIB,      0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_YIELD,           0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_BITDEPTH,        0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_GRAYSCALE,       0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_MAKEPALETTE,     0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_EXTERNALPALETTE, 0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_EMBEDDEDPALETTE, 0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_OPTOTAL,         0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_PAINT,           0, sizeof("999.999") },
    { 0, eText, IDC_OF2DTIME_TOTAL,           0, sizeof("999.999") },
    { 0, eEnd, -1 }
    };



/******************************************************************************
 *  global data
 ******************************************************************************/



static DWORD arBMPTypes[] = {
    BI_RGB,  BI_BMPO, BI_BMPR,
    (DWORD)-1 };

OPENOP OpenDefBMP81 =
    {
    OP_F2D,
    0,                  /* dll description in OpenDefPCX81 */
    "Bitmap Defaults",
    0,                  /* 0 so MOBMP.C version is the right one if this dll isn't present */
    arBMPTypes,
    0,
    OpenBMP,
    ExtractDibOpenUtility,
    OptionsDialogOpenBMP,
    PropertiesDialogOpenBMP,
    DefaultsDialogOpenBMP,
    CleanupOpenUtility,
    0,
    &SaveDefBMP80,
    0
    };



/* ini section for image options */
static DWORD arPCXTypes[] = {
     BI_PCX1, BI_PCX2, BI_PCX3, BI_PCX4, BI_PCX8, BI_PCXT, /* BI_DCXZ, */
     (DWORD)-1 };

OPENOP OpenDefPCX81 =
    {
    OP_F2D,
    "Bitmap, Targa, PCX, or TIFF Input",
    "PCX Defaults",
    "PCX (*.pcx)|*.PCX|",
    arPCXTypes,
    OpenInitUtility,
    OpenPCX,
    ExtractDibOpenUtility,
    OptionsDialogOpenPCX,
    PropertiesDialogOpenPCX,
    DefaultsDialogOpenPCX,
    CleanupOpenUtility,
    0,
    &SaveDefPCX80,
    0
    };



/* ini section for image options */
static DWORD arTIFTypes[] = {
     BI_TIF1, BI_TIFM, BI_TIFG, BI_TIFC, BI_TIFZ, BI_TIFK, BI_TIFu,
     (DWORD)-1 };

/* ini section for image options */
static DWORD arTIFPlusTypes[] = {
     BI_TIF1, BI_TIFM, BI_TIFG, BI_TIFC, BI_TIFZ, BI_TIFL, BI_TIFK, BI_TIFu,
     (DWORD)-1 };


OPENOP OpenDefTIF81 =
    {
    OP_F2D,
    0,
    "TIFF Defaults",
    "TIFF (*.tif)|*.TIF|",
    arTIFTypes,
    0,
    OpenTIF,
    ExtractDibOpenUtility,
    OptionsDialogOpenTIF,
    PropertiesDialogOpenTIF,
    DefaultsDialogOpenTIF,
    CleanupOpenUtility,
    0,
    &SaveDefTIF80,
    0
    };



/* ini section for image options */
static DWORD arTGATypes[] = {
     BI_TGA1, BI_TGA2, BI_TGA3, BI_TGA9, BI_TGAA, BI_TGAB,
     (DWORD)-1 };

OPENOP OpenDefTGA81 =
    {
    OP_F2D,
    0,
    "Targa Defaults",
    "Targa (*.tga)|*.TGA|",
    arTGATypes,
    0,
    OpenTGA,
    ExtractDibOpenUtility,
    OptionsDialogOpenTGA,
    PropertiesDialogOpenTGA,
    DefaultsDialogOpenTGA,
    CleanupOpenUtility,
    0,
    &SaveDefTGA80,
    0
    };



/* ini section for image options */
static DWORD arGIFTypes[] = {
     BI_GIFN, BI_GIFI, BI_GIFu,
     (DWORD)-1 };

OPENOP OpenDefGIF85 =
    {
    OP_F2DPLUS,
    "Bitmap, Targa, PCX, GIF or TIFF Input",
    "GIF Defaults",
    "GIF (*.gif)|*.GIF|",
    arGIFTypes,
    OpenInitUtilityPlus,
    OpenGIF,
    ExtractDibOpenUtility,
    OptionsDialogOpenGIF,
    PropertiesDialogOpenGIF,
    DefaultsDialogOpenGIF,
    CleanupOpenUtility,
    0,
    0,  //&SaveDefGIF80,
    0
    };



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  void OpenInitUtility()
 * 
 *  replace default open BMP open opcode table entry with this one and
 *  label BMP, TIF and TGA as found
 *
 ******************************************************************************/
void OpenInitUtility(void)
{
    OpenDefBMP81.pszFilter     = OpenDefBMP.pszFilter;
    memset(&OpenDefBMP, '\0', sizeof(OpenDefBMP));
    OpenDefBMP81.nFoundParmVer = OpenDefPCX81.nFoundParmVer;
    OpenDefTIF81.nFoundParmVer = OpenDefPCX81.nFoundParmVer;
    OpenDefTGA81.nFoundParmVer = OpenDefPCX81.nFoundParmVer;
}

void OpenInitUtilityPlus(void)
{
    if ( OpenDefBMP81.pszFilter == 0 )
        {
        OpenDefBMP81.pszFilter      = OpenDefBMP.pszFilter;
        memset(&OpenDefBMP, '\0', sizeof(OpenDefBMP));
        }
    /* else OpenInitUtility has already been called */

    if ( OpenDefPCX81.nFoundParmVer != 0 )
        PegasusUnload(OpenDefPCX81.lOpcode, CURRENT_PARMVER);

    OpenDefBMP81.nFoundParmVer  = OpenDefGIF85.nFoundParmVer;
    OpenDefBMP81.lOpcode        = OpenDefGIF85.lOpcode;

    OpenDefTIF81.nFoundParmVer  = OpenDefGIF85.nFoundParmVer;
    OpenDefTIF81.lOpcode        = OpenDefGIF85.lOpcode;
    OpenDefTIF81.pbiCompression = arTIFPlusTypes;

    OpenDefTGA81.nFoundParmVer  = OpenDefGIF85.nFoundParmVer;
    OpenDefTGA81.lOpcode        = OpenDefGIF85.lOpcode;

    OpenDefPCX81.nFoundParmVer  = OpenDefGIF85.nFoundParmVer;
    OpenDefPCX81.lOpcode        = OpenDefGIF85.lOpcode;
    OpenDefPCX81.pszAbout       = 0;
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
    PIC_PARM pp;
    RGBQUAD ColorTable[256];
    LPCTRLINITDATA par = (LPCTRLINITDATA)pOpenData;
    BYTE PICHUGE* pDib;
    DWORD dwDibLen;
    DWORD dwBits;
    BOOL bRet = TRUE;
    char sz[_MAX_PATH + 1];
    DWORD dwNumColors;
    BOOL bDisplayTiming;
    APPDATA App;
    PIC_PARM ppx = *pSrcPicParm;
	LPCTRLINITDATA pCtrl;
    BOOL bYield;
        
    NOREFERENCE(hWnd);
    
    memset(&App, 0, sizeof(App));
    *ppDib = 0;
    *pdwDibLen = 0;

    pCtrl = CtrlData(par, IDD_TIMING);
    assert(pCtrl != 0 );
    bDisplayTiming =
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_DISPLAYTIMING);
    bYield = 
        CtrlDataIsPushed((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_CHECK_YIELD);
    App.dwGetSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_INPUTK, FALSE) * 1024;
    App.dwPutSize =
        CtrlDataGetLong((LPCTRLINITDATA)pCtrl->pszText, IDC_TIMING_EDIT_OUTPUTK, FALSE) * 1024;

    App.dwTime[TOTAL] -= MiscTickCount();
    App.dwTime[OPTOTAL] = App.dwTime[TOTAL];
    App.dwTime[EXTRACTDIB] = App.dwTime[TOTAL];
    
    if ( bYield )
        ppx.u.F2D.PicFlags |= PF_YieldPut | PF_YieldGet;
    if ( CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_GIFTRANSPARENT) &&
         ( pSrcPicParm->Head.biCompression == BI_GIFI ||
           pSrcPicParm->Head.biCompression == BI_GIFN ||
			  pSrcPicParm->Head.biCompression == BI_GIFu ) )
		ppx.u.QRY.PicFlags |= PF_ApplyTransparency;
    else
		ppx.u.QRY.PicFlags &= ~PF_ApplyTransparency;
	ppx.ImageNumber = CtrlDataGetLong(par, IDC_OF2DOPT_EDIT_IMAGENUMBER, FALSE);
	if ( 1 < ppx.ImageNumber )
	{
		if ( ppx.ImageNumber > pSrcPicParm->u.QRY.NumImages )
			ppx.ImageNumber = 1;
		else
		{
			DWORD ImageNumber = ppx.ImageNumber;

		    memset(&ppx, 0, sizeof(ppx));
			ppx.ParmSize     = sizeof(ppx);
		    ppx.ParmVer      = CURRENT_PARMVER;
			ppx.ParmVerMinor = 0;
			ppx.ImageNumber  = ImageNumber;
			ppx.Get.Start    = (BYTE PICHUGE*)pSrc; /* start of input queue buffer */
			ppx.Get.End      = ppx.Get.Start + dwSrcLen; /* end of input queue buffer */
			ppx.Get.Front    = ppx.Get.Start;		/* start of input queue data */
			ppx.Get.Rear     = ppx.Get.End;			/* end of input queue data */
		    ppx.u.QRY.BitFlagsReq =					/* requested query data from image */
			    QBIT_BICOMPRESSION | QBIT_BIBITCOUNT |
				QBIT_BIHEIGHT | QBIT_BIWIDTH |
				QBIT_BICLRUSED | QBIT_BICLRIMPORTANT | QBIT_PALETTE;
		    /* PegasusQuery returns FALSE if an error occurred or if it couldn't retrieve all
			    the requested information.  In the latter case, Status == 0 (no error)
				and we just make sure we have the barest minimum information we require,
		        also make sure that quantities we have to deal with as signed integers
			    aren't (quite unexpectedly) larger than INT_MAX.  I think the expand
				and pack dll's would choke anyway for Width > INT_MAX */
		    if ( ( !PegasusQuery(&ppx) &&
		           ( ppx.Status != 0 ||
			         ( ppx.u.QRY.BitFlagsAck &
				         ( QBIT_BICOMPRESSION | QBIT_BIBITCOUNT | QBIT_BIHEIGHT | QBIT_BIWIDTH ) ) !=
                         ( QBIT_BICOMPRESSION | QBIT_BIBITCOUNT | QBIT_BIHEIGHT | QBIT_BIWIDTH )
	               )
		         ) ||
			     ppx.Head.biHeight       > INT_MAX ||
	             ppx.Head.biWidth        > INT_MAX ||
		         ppx.Head.biClrUsed      > INT_MAX ||
			     ppx.Head.biClrImportant > INT_MAX
			   )
			{
				ppx = *pSrcPicParm;
				ppx.ImageNumber = 1;
			}
		}
	}
	ppx.App = (LPARAM)(APPDATA PICHUGE*)&App;
	if ( !ExtractDibOpenUtility(pSrc, dwSrcLen, &ppx, &pDib, &dwDibLen, &pp) )
		return ( FALSE );
	App.dwTime[EXTRACTDIB] += MiscTickCount();

	/* apply image options for color reduction, etc by applying OP_UTL to the DIB */
	*ppDib = pDib;
	*pdwDibLen = dwDibLen;
	hmemcpy(pDibPicParm, &pp, sizeof(*pDibPicParm));
	if ( !CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_IMAGESETTINGS) && ToolDefUtility.nFoundParmVer != 0 )
        {        
        /* the remaining settings depend on DIB size */
        switch ( CtrlDataWhichRadio(par, IDC_OF2DOPT_RADIO_32BITS) )
            {
            case IDC_OF2DOPT_RADIO_32BITS:
                dwBits = 32;
                break;
            case IDC_OF2DOPT_RADIO_24BITS:
                dwBits = 24;
                break;
            case IDC_OF2DOPT_RADIO_16BITS:
                dwBits = 16;
                break;
            case IDC_OF2DOPT_RADIO_8BITS:
                dwBits = 8;
                break;
            case IDC_OF2DOPT_RADIO_4BITS:
                dwBits = 4;
                break;
            case IDC_OF2DOPT_RADIO_1BITS:
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
            if ( dwBits != pp.Head.biBitCount )
                {
                App.dwTime[BITDEPTH] -= MiscTickCount();
                bRet = Op82BitDepth(
                    dwBits,
                    CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_DITHER),    // could count if dwBits == 16
                    pDib,  dwDibLen,  &pp,
                    ppDib, pdwDibLen, pDibPicParm);
                App.dwTime[BITDEPTH] += MiscTickCount();
                }
            }
        else if ( CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_MAKEGRAY) )
        	{
            if ( pp.Head.biBitCount != dwBits ||
                 !MiscIsGrayPalette(pp.Head.biClrImportant, pp.ColorTable) )
            	{
	            dwNumColors = CtrlDataGetLong(par, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, FALSE);
    	        App.dwTime[GRAYSCALE] -= MiscTickCount();
        	    bRet = Op82Grayscale(
            	    dwBits,
                	CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_DITHER),
	                dwNumColors,
    	            pDib,  dwDibLen,  &pp,
        	        ppDib, pdwDibLen, pDibPicParm);
            	App.dwTime[GRAYSCALE] += MiscTickCount();
            	}
            }
        else if ( CtrlDataIsPushed(par, IDC_OF2DOPT_RADIO_MAKEPALETTE) )
            {
            dwNumColors = CtrlDataGetLong(par, IDC_OF2DOPT_EDIT_COLORSTOMAKE, FALSE);
            App.dwTime[MAKEPALETTE] -= MiscTickCount();
            bRet = Op82MakePalette(
                dwBits,
                CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_DITHER),
                dwNumColors,
                pDib,  dwDibLen,  &pp,
                ppDib, pdwDibLen, pDibPicParm);
            App.dwTime[MAKEPALETTE] += MiscTickCount();
            }
        else if ( CtrlDataIsPushed(par, IDC_OF2DOPT_RADIO_EXTERNALPALETTE) )
            {
            CtrlDataGetText(par, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
                dwNumColors = MiscPaletteColors(sz, ColorTable, 1U << (WORD)dwBits);
            assert(dwNumColors != 0);
            /* if external palette file doesn't have a suitable palette, then it's a bug */
            App.dwTime[EXTERNALPALETTE] -= MiscTickCount();
            bRet = Op82ExternalPalette(
                dwBits,
                CtrlDataIsPushed(par, IDC_OF2DOPT_CHECK_DITHER),
                dwNumColors,
                ColorTable,
                pDib,  dwDibLen,  &pp,
                ppDib, pdwDibLen, pDibPicParm);
            App.dwTime[EXTERNALPALETTE] += MiscTickCount();
            }        
        else /* EMBEDDEDPALETTE is pushed */
            {
            }
        }

    if ( *ppDib != pDib )
        MiscGlobalFreePtr(&pDib);

    if ( MiscIsPaletteDisplay(&num) && pDibPicParm->Head.biBitCount > 8 )
    {
        /* if the image is rgb on a palettized display, it will look very bad unless
            we've provided windows with a suitable palette. */
        App.dwTime[MAKEPALETTE] -= MiscTickCount();
        Op82CreatePalette(1 << num, *ppDib, *pdwDibLen, pDibPicParm);
        App.dwTime[MAKEPALETTE] -= MiscTickCount();
    }
    App.dwTime[OPTOTAL] += MiscTickCount();

    App.dwTime[PAINT] -= MiscTickCount();
    ChildSetDibHeight(hWnd, labs(pDibPicParm->Head.biHeight));
    ChildInvalidatePalette(hWnd);
    ChildSetWindowSize(hWnd);
    App.dwTime[PAINT] += MiscTickCount();
    App.dwTime[TOTAL] += MiscTickCount();
    
    if ( bDisplayTiming )
        {
        MiscDisplayTimingDialog(hWnd,
            MAKEINTRESOURCE(IDD_OF2DTIME),
            App.dwTime,
            InitDisplayTiming);
        }
    return ( bRet );
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

    response = Pegasus(p, REQ_INIT);
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

            case RES_GET_NEED_DATA:
                p->Get.QFlags |= Q_IO_ERR;
                break;
                
            default:
                assert(FALSE);  /* no others are expected */
                break;
            }
        response = Pegasus(p, REQ_CONT);
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
    RESPONSE response;
	void PICHUGE* pv;
	APPDATA PICHUGE* pApp = (APPDATA PICHUGE*)p->App;
    DWORD dw;
    
    response = Pegasus(p, REQ_EXEC);
    while ( response != RES_DONE )
        {
        switch ( response )
            {
            case RES_ERR:
                if ( p->Status == ERR_NONE || p->Status == -1 )
                    p->Status = ERR_BAD_DATA;        
                return ( RES_ERR );

            case RES_PUT_NEED_SPACE:
				if ( pApp->pPut0 == 0 )
					p->Put.QFlags |= Q_IO_ERR;
				else
	                MiscCopyFromQueue(&pApp->pPut, &pApp->dwPutLen, &p->Put, 0);
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
						pApp->dwPutLen = pApp->pPut0 + pApp->dwPutSize - pApp->pPut;
						if ( (int)pApp->dwPutLen <= 0 )
							p->Get.QFlags |= Q_IO_ERR;
					}
				}
                break;

			case RES_AUX_NEEDED:
				if ( p->u.F2D.AuxSpace == 0 )
					pv = GlobalAllocPtr(GMEM_MOVEABLE, p->u.F2D.AuxNeeded);
				else
					pv = GlobalReAllocPtr(p->u.F2D.AuxSpace, p->u.F2D.AuxSize + p->u.F2D.AuxNeeded, GMEM_MOVEABLE);
				if ( pv == 0 )
				{
			        ErrorMessage(STYLE_ERROR, IDS_IMAGEOUTOFMEMORY);
					Pegasus(p, REQ_TERM);
					p->Status = ERR_OUT_OF_SPACE;
					return ( RES_ERR );
				}
				p->u.F2D.AuxSpace = pv;
				p->u.F2D.AuxSize += p->u.F2D.AuxNeeded;
				break;

            case RES_PUT_DATA_YIELD:
            case RES_GET_DATA_YIELD:
                dw = MiscTickCount();
                pApp->dwTime[YIELD] -= dw;
                pApp->dwTime[EXTRACTDIB] += dw;
                if ( MiscYield() )
                    {
                    Pegasus(p, REQ_TERM);
                    p->Status = ERR_NONE;
                    return ( RES_ERR );
                    }
                dw = MiscTickCount();
                pApp->dwTime[YIELD] += dw;
                pApp->dwTime[EXTRACTDIB] -= dw;
                break;

            default:
                assert(FALSE);
                break;
            }
        response = Pegasus(p, REQ_CONT);
        }
    return ( response );                       
}



/******************************************************************************
 *  BOOL ExtractDibOpenUtility(
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
static BOOL ExtractDibOpenUtility(
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm)
{
    RESPONSE response;
    PIC_PARM PICFAR* pp = pDibPicParm;
	APPDATA PICHUGE* pApp = (APPDATA PICHUGE*)pSrcPicParm->App;

     /* nothing valid yet */
     *ppDib = 0;
     *pdwDibLen = 0;
     /*#### progressive GIF display not implemented ####*/
     /*#### bProgressiveDisplay = ( pSrcPicParm->u.QRY.PicFlags & PF_YieldPut) != 0; ####*/
    
    /**************************************************************************
        Initialize PIC_PARM data
    */

    /* initialize the pic_parm elements independent of image options settings */
    _fmemset(pp, 0, sizeof(*pp));     /* _VERY_ important */
    pp->ParmSize     = sizeof(*pp);
    pp->ParmVer      = CURRENT_PARMVER;  /* from PIC.H */
    pp->ParmVerMinor = 1;
    pp->Op           = OpenDefBMP81.lOpcode;    /* Sequential JPEG to DIB */
	pp->ImageNumber  = pSrcPicParm->ImageNumber;
	pp->App          = (LPARAM)(APPDATA PICFAR*)pApp;

    hmemcpy(&pp->Head, &pSrcPicParm->Head, sizeof(pp->Head));
    pp->u.F2D.AllocType = 1;        /* full-image buffers */

    pp->u.F2D.ProgressiveMode = 2;              /* GIF - final DIB only */
    pp->u.F2D.PicFlags = pSrcPicParm->u.F2D.PicFlags & ( PF_ApplyTransparency | PF_YieldPut | PF_YieldGet );
    /* initialize source image queue pointers */
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
    if ( pApp->dwGetSize == 0 || pApp->dwGetSize >= dwSrcLen )
        {
        pp->Get.Start  = (BYTE PICHUGE*)pSrc;
            /* cast away constness to eliminate the warning --
                note however that this is not safe in general with Pegasus, or as a general
                practice, (although it is currently safe with Sequential JPEG expand). */
        pp->Get.End    = pp->Get.Start + dwSrcLen + 1;
        pp->Get.Front  = pp->Get.Start;
        pp->Get.Rear   = pp->Get.End - 1;
        pp->Get.QFlags = Q_EOF;    /* no more to read, the entire image is in the buffer */
        }
    else
        {
        BYTE PICHUGE* pGetQ = MiscGlobalAllocPtr(pApp->dwGetSize, IDS_IMAGEOUTOFMEMORY);
        if ( pGetQ == 0 )
            return ( FALSE );
        pp->Get.Start   = pGetQ;            
        pp->Get.End     = pp->Get.Start + pApp->dwGetSize;
        pp->Get.Front   = pp->Get.Start;
        pp->Get.Rear    = pp->Get.Front;
		pApp->pGet0     = (BYTE PICHUGE*)pSrc;
		pApp->pGet      = pApp->pGet0;
        pApp->dwGetSize = dwSrcLen;
		pApp->dwGetLen  = pApp->dwGetSize;
        }            

	/**************************************************************************
        Pegasus(REQ_INIT)
    */
    response = PegasusReqInit(pp);
    if ( response == RES_ERR )
        {
		if ( pApp->pGet0 != 0 )
			MiscGlobalFreePtr(&pp->Get.Start);
        CleanupOpenUtility(0, pp, 0);
        ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        return ( FALSE );
        }

    /**************************************************************************
        Allocate output DIB buffer according to the height and widthpad (scan line
        width including dword padding)
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    *pdwDibLen = labs(pSrcPicParm->Head.biHeight) *
        ( ( pSrcPicParm->Head.biWidth * pSrcPicParm->Head.biBitCount + 31 ) & ~31 ) / 8;
    *ppDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwDibLen, IDS_IMAGEOUTOFMEMORY);
    if ( *ppDib == 0 )
        {
        Pegasus(pp, REQ_TERM);
		if ( pApp->pGet0 != 0 )
			MiscGlobalFreePtr(&pp->Get.Start);
        CleanupOpenUtility(0, pp, 0);
        return ( FALSE );
        }

    pp->Head.biHeight = labs(pp->Head.biHeight);

    if ( pApp->dwPutSize == 0 || pApp->dwPutSize >= *pdwDibLen )
        {
        /* initialize put queue buffer pointers */
        pp->Put.Start = *ppDib;            
        pp->Put.End = pp->Put.Start + *pdwDibLen + 1;
        }
    else
        {
        /* note that _very_ bad stuff happens if the put q is smaller than the strip size */
		BYTE PICHUGE* pPutQ;

        pPutQ = MiscGlobalAllocPtr(pApp->dwPutSize, IDS_IMAGEOUTOFMEMORY);
        if ( pPutQ == 0 )
            {
            Pegasus(pp, REQ_TERM);
			if ( pApp->pGet0 != 0 )
				MiscGlobalFreePtr(&pp->Get.Start);
            CleanupOpenUtility(*ppDib, pp, 0);
            return ( FALSE );
            }
        pp->Put.Start   = pPutQ;
        pp->Put.End     = pPutQ + pApp->dwPutSize;
        pApp->dwPutSize = *pdwDibLen;
		pApp->dwPutLen  = pApp->dwPutSize;
        /* note App.dwPutLen isn't used for this opcode */
        pApp->pPut      = (BYTE PICHUGE *)*ppDib;
		pApp->pPut0     = pApp->pPut;
        }
	pp->Put.Front = pp->Put.Rear = pp->Put.Start;

    if ( ( pp->u.F2D.PicFlags & PF_ApplyTransparency ) != 0 )
        {
        BYTE PICHUGE *pb = pp->Put.Front;
        int row;
        int pixel;
        int black = 0;
        int white = 0;
        DWORD blacky = ULONG_MAX;
        DWORD whitey = 0;
        int i;
        DWORD dwPadding = ( ( pp->Head.biWidth + 3 ) & ~3 ) - pp->Head.biWidth;
    
        /* pick the brightest and darkest colors */
        for ( i = 0; (DWORD)i < pSrcPicParm->Head.biClrUsed; i++ )
        {
            DWORD y = pSrcPicParm->ColorTable[i].rgbRed * 30UL +
                pSrcPicParm->ColorTable[i].rgbGreen * 59UL +
                pSrcPicParm->ColorTable[i].rgbBlue * 11UL;
            if ( y > whitey )
                {
                whitey = y;
                white = i;
                }
            if ( y < blacky )
            {
                blacky = y;
                black = i;
            }
        }
        /* draw a diagonal pattern across the field */        
        for ( row = 0; row < pp->Head.biHeight; row++ )
            {
            for ( pixel = 0; pixel <  pp->Head.biWidth; pixel++ )
                {
                if ( ( pixel + ( row % 4 ) ) % 4 == 0 )
                    {
                    *pb++ = (BYTE)black;
                    }
                else
                    {
                    *pb++ = (BYTE)white;
                    }
                }                
            pb += dwPadding;
            }
        }

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */

    response = PegasusReqExec(pp);
    if ( response == RES_ERR )
        {
		if ( pApp->pGet0 != 0 )
			MiscGlobalFreePtr(&pp->Get.Start);
		if ( pApp->pPut0 != 0 )
			MiscGlobalFreePtr(&pp->Put.Start);
        CleanupOpenUtility(*ppDib, pp, 0);
        ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        return ( FALSE );
        }

    /**************************************************************************
        Pegasus(REQ_TERM)
    */
    response = Pegasus(pp, REQ_TERM);
    assert(response == RES_DONE);

	if ( pApp->pGet0 != 0 )
		MiscGlobalFreePtr(&pp->Get.Start);
	if ( pApp->pPut0 != 0 )
		MiscGlobalFreePtr(&pp->Put.Start);
    hmemcpy(&pp->Head, &pp->u.F2D.BiOut, sizeof(pp->Head));
    return ( TRUE );
}



/******************************************************************************
 *  BOOL OpenUtility(
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
static BOOL OpenBMP(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    return ( OpenUtility(
        "Bitmap",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OpenPCX(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    return ( OpenUtility(
        "PCX",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OpenTIF(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    return ( OpenUtility(
        "TIFF",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OpenTGA(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    return ( OpenUtility(
        "Targa",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OpenGIF(
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    return ( OpenUtility(
        "GIF",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OpenUtility(
    LPCSTR                     pszType,
    HWND                       hWnd,
    const BYTE PICHUGE*        pSrc,
    DWORD                      dwSrcLen,
    const PIC_PARM PICFAR*     pSrcPicParm,
    BYTE PICHUGE* PICFAR*      ppDib,
    DWORD PICFAR*              pdwDibLen,
    PIC_PARM PICFAR*           pDibPicParm,
    VOID PICFAR* PICFAR*       ppOpenData)
{
    char szSection[64];
    char sz[_MAX_PATH + 1];
    int nColors;
    long lPrimaryColors;
    long lSecondaryColors;
    
    if ( ToolDefUtility.nFoundParmVer == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPCODE82MISSINGOPTIONS, (LPCSTR)ToolDefUtility.pszAbout);
        }
    /* read default ini image options settings */
    wsprintf(szSection, szOptionsSection, pszType);
    assert(strlen(szSection) < sizeof(szSection));
	strcpy(szTimingOptionsSection, szSection);
    if ( !GetCtrlDefaults(szSection, (LPCTRLINITDATA PICFAR*)ppOpenData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_IMAGEOUTOFMEMORY);
        /* "There was not enough memory to open the image." */
        return ( FALSE );
        }        

    if ( ToolDefUtility.nFoundParmVer != 0 )
        {
        /*#### F2D doesn't yet deal with 32-bit or 16-bit DIB's (and UTL doesn't convert) */
        CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_32BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_16BITS);
    
        /* if source image is grayscale, don't let the user change color palette choices
            in the image options dialog box */
        if ( ( pSrcPicParm->u.F2D.PicFlags & PF_IsGray ) != 0 )
            {
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_STATIC_COLORPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_32BITS);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_24BITS);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_16BITS);

            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_MAKEPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_EXTERNALPALETTE);
        
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_EDIT_COLORSTOMAKE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_STATIC_COLORS);
        
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_STATIC_EXTERNALPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_EDIT_EXTERNALPALETTE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_BUTTON_BROWSE);

            /* and make sure either 4-bit or 8-bit bit depth is pushed and
                that make gray is pushed */
            if ( !CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_4BITS) &&
                 !CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_1BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_8BITS, TRUE);
            CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_CHECK_MAKEGRAY, TRUE);
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_CHECK_MAKEGRAY);
            }
        else
            {
            /* if it's a color source image, but there's no embedded palette,
                don't let the user select "Embedded palette" in color palette image options */
            if ( pSrcPicParm->Head.biClrUsed == 0 )
                CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE);

            if ( ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_8BITS) ||
                   CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_4BITS) ||
                   CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_1BITS) ) &&
                 !CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_CHECK_MAKEGRAY) )
                {
                if ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_8BITS) )
                    nColors = 256;
                else if ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_4BITS) )
                    nColors = 16;
                else
                    nColors = 2;
                /* if the image needs a color palette
                    and embedded palette is selected, but there is no suitable embedded palette
                    or external palette is selected, but the file doesn't have a suitable palette,
                    then report the error to the user and set make palette for this image */
                CtrlDataGetText(*ppOpenData, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
                lPrimaryColors = pSrcPicParm->Head.biClrImportant;
                lSecondaryColors = pSrcPicParm->Head.biClrUsed - pSrcPicParm->Head.biClrImportant;
                if ( ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE) &&
                       ( lPrimaryColors < 2 || lPrimaryColors > nColors ) &&
                       ( lSecondaryColors < 2 || lSecondaryColors > nColors ) ) ||
                     ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_EXTERNALPALETTE) &&
                       MiscPaletteColors(sz, 0, nColors) == 0 ) )
                    {
                    ErrorMessage(STYLE_ERROR, IDS_IMAGECOLORPALETTE);
                    /* "The image default Color Palette option is not suitable. "
                        "An optimal color palette will be made." */
                    CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_MAKEPALETTE, TRUE);
                    CtrlDataSetLong(*ppOpenData, IDC_OF2DOPT_EDIT_COLORSTOMAKE, nColors, FALSE);
                    }
                }
            if ( CtrlDataIsInactive(*ppOpenData, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE) &&
                 CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE) )
                CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_MAKEPALETTE, TRUE);
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
 *  BOOL OptionsDialogOpen???(
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
static BOOL OptionsDialogOpenBMP(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    return ( OptionsDialogOpenUtility(
        "Bitmap",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OptionsDialogOpenPCX(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    return ( OptionsDialogOpenUtility(
        "PCX",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OptionsDialogOpenTGA(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    return ( OptionsDialogOpenUtility(
        "Targa",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OptionsDialogOpenTIF(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    return ( OptionsDialogOpenUtility(
        "TIFF",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OptionsDialogOpenGIF(
    HWND                   hWnd,
    const BYTE PICHUGE*    pSrc,
    DWORD                  dwSrcLen,
    const PIC_PARM PICFAR* pSrcPicParm,
    BYTE PICHUGE* PICFAR*  ppDib,
    DWORD PICFAR*          pdwDibLen,
    PIC_PARM PICFAR*       pDibPicParm,
    VOID PICFAR* PICFAR*   ppOpenData)
{
    return ( OptionsDialogOpenUtility(
        "GIF",
        hWnd,
        pSrc,
        dwSrcLen,
        pSrcPicParm,
        ppDib,
        pdwDibLen,
        pDibPicParm,
        ppOpenData) );
}

static BOOL OptionsDialogOpenUtility(
    LPCSTR                 pszType,
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
    char szSection[64];
    
    if ( ToolDefUtility.nFoundParmVer == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPCODE82MISSINGOPEN, (LPCSTR)ToolDefUtility.pszAbout);
        return ( FALSE );
        }
        
    wsprintf(szSection, szOptionsSection, pszType);
    assert(strlen(szSection) < sizeof(szSection));
    if ( !CtrlDataDup(*ppData, &pOldData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return ( FALSE );
        }

	if ( ( pDibPicParm->u.F2D.PicFlags & PF_IsGray ) != 0 )
	{
        /* and make sure either 4-bit or 8-bit bit depth is pushed and
            that make gray is pushed */
        if ( !CtrlDataIsPushed(*ppData, IDC_OF2DOPT_RADIO_4BITS) &&
             !CtrlDataIsPushed(*ppData, IDC_OF2DOPT_RADIO_1BITS) )
            CtrlDataSetPushed(*ppData, IDC_OF2DOPT_RADIO_8BITS, TRUE);
        CtrlDataSetPushed(*ppData, IDC_OF2DOPT_CHECK_MAKEGRAY, TRUE);
        CtrlDataSetInactive(*ppData, IDC_OF2DOPT_CHECK_MAKEGRAY);
	}
	if ( pSrcPicParm->u.QRY.NumImages <= 1 )
	{
		CtrlDataSetInactive(*ppData, IDC_OF2DOPT_EDIT_IMAGENUMBER);
	    CtrlDataSetLong(*ppData, IDC_OF2DOPT_EDIT_IMAGENUMBER, pSrcPicParm->u.QRY.NumImages, FALSE);
	}
	
    memset(&oud, 0, sizeof(oud));
    oud.hWnd        = hWnd;
    oud.pszType     = pszType;
    oud.ppSrc       = (BYTE PICHUGE* PICFAR*)&pSrc,
    oud.pdwSrcLen   = &dwSrcLen;
    oud.pSrcPicParm = (PIC_PARM PICFAR*)pSrcPicParm;
    oud.ppDib       = ppDib;
    oud.pdwDibLen   = pdwDibLen;
    oud.pDibPicParm = pDibPicParm;
    oud.bTested     = FALSE;

    result = DlgDoModal(hwndFrame, MAKEINTRESOURCE(IDD_OF2DOPT), ImageOnCommand, *ppData, &oud);
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
                CleanupOpenUtility(pOldDib, &OldDibPicParm, 0);
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
    if ( CtrlDataIsPushed(*ppData, IDC_OF2DOPT_CHECK_SETDEFAULT) )
	{
		strcpy(szTimingOptionsSection, szSection);
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szSection, *ppData);
	}

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
        CleanupOpenUtility(pOldDib, &OldDibPicParm, 0);
        }
    return ( TRUE );
}



/******************************************************************************
 *  void PropertiesDialogOpenUtility(
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
static void PropertiesDialogOpenBMP(
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
    PropertiesDialogOpenUtility(
        "Bitmap",
        hWnd,
        pszFilename,
        pSrc, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        pOpenData);
}    

static void PropertiesDialogOpenPCX(
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
    PropertiesDialogOpenUtility(
        "PCX",
        hWnd,
        pszFilename,
        pSrc, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        pOpenData);
}    

static void PropertiesDialogOpenTIF(
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
    PropertiesDialogOpenUtility(
        "TIFF",
        hWnd,
        pszFilename,
        pSrc, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        pOpenData);
}    

static void PropertiesDialogOpenTGA(
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
    PropertiesDialogOpenUtility(
        "Targa",
        hWnd,
        pszFilename,
        pSrc, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        pOpenData);
}    

static void PropertiesDialogOpenGIF(
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
    PropertiesDialogOpenUtility(
        "GIF",
        hWnd,
        pszFilename,
        pSrc, dwSrcLen, pSrcPicParm,
        pDib, dwDibLen, pDibPicParm,
        pOpenData);
}    

static void PropertiesDialogOpenUtility(
    LPCSTR                 pszType,
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
    char sz[_MAX_PATH + sizeof(" Bitmap Properties")];
    int result;
    LPCTRLINITDATA pInitData;
    DWORD dwSrcWidthPad;
    
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
    CtrlDataSetText(pInitData, IDC_F2DPROP_FILENAME, sz);
    CtrlDataSetLong(pInitData, IDC_F2DPROP_FILESIZE, dwSrcLen, FALSE);
	if ( lstrcmp(pszType, "TIFF") == 0 )
	{
		switch ( pDibPicParm->u.F2D.Compression )
		{
		case 1:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF Uncompressed");
			break;
		case 2:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF Modified G3");
			break;
		case 3:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF 1-dimensional G3");
			break;
		case -3:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF 2-dimensional G3");
			break;
		case 4:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF G4");
			break;
		case 5:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF LZW");
			break;
		case 32773U:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, "TIFF Packbits");
			break;
		case 0:
		default:
			CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, pszType);
			break;
		}
	}
	else
		CtrlDataSetText(pInitData, IDC_F2DPROP_FILETYPE, pszType);
    if ( pSrcPicParm->u.QRY.NumImages != 0 )
	    CtrlDataSetLong(pInitData, IDC_F2DPROP_NUMIMAGES, pSrcPicParm->u.QRY.NumImages, FALSE);
	else
		CtrlDataSetInactive(pInitData, IDC_F2DPROP_NUMIMAGES);
    CtrlDataSetLong(pInitData, IDC_F2DPROP_IMAGEWIDTH_SOURCE, pSrcPicParm->Head.biWidth, FALSE);
    CtrlDataSetLong(pInitData, IDC_F2DPROP_IMAGEHEIGHT_SOURCE, labs(pSrcPicParm->Head.biHeight), FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_BITSPERPIXEL_SOURCE,
        pSrcPicParm->Head.biBitCount,
        FALSE);
    dwSrcWidthPad = ( pSrcPicParm->Head.biWidth * pSrcPicParm->Head.biBitCount + 7 ) / 8;
    dwSrcWidthPad = ( ( dwSrcWidthPad + sizeof(DWORD) - 1 ) / sizeof(DWORD) ) * sizeof(DWORD);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_IMAGESIZE_SOURCE,
        dwSrcWidthPad * labs(pSrcPicParm->Head.biHeight),
        FALSE);
    CtrlDataSetLong(pInitData, IDC_F2DPROP_COLORTABLESIZE_SOURCE, pSrcPicParm->Head.biClrUsed, FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_NUMBERIMPORTANT_SOURCE,
        pSrcPicParm->Head.biClrImportant,
        FALSE);

    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_IMAGEWIDTH_DISPLAYED,
        pDibPicParm->Head.biWidth,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_IMAGEHEIGHT_DISPLAYED,
        pDibPicParm->Head.biHeight,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_BITSPERPIXEL_DISPLAYED,
        pDibPicParm->Head.biBitCount,
        FALSE);
    CtrlDataSetLong(pInitData, IDC_F2DPROP_IMAGESIZE_DISPLAYED, dwDibLen, FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_COLORTABLESIZE_DISPLAYED,
        pDibPicParm->Head.biClrUsed,
        FALSE);
    CtrlDataSetLong(
        pInitData,
        IDC_F2DPROP_NUMBERIMPORTANT_DISPLAYED,
        pDibPicParm->Head.biClrImportant,
        FALSE);

    /**************************************************************************
        Display the properties dialog
    */
    GetWindowText(hWnd, sz, sizeof(sz) - ( sizeof(" Bitmap Properties") - 1 ));
    strcat(sz, " ");
    _fstrcat(sz, pszType);
    strcat(sz, " Properties");
    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_F2DPROP),
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
 *  void DefaultsDialogOpen???(void)
 *
 *  default image options dialog
 * 
 ******************************************************************************/
static void DefaultsDialogOpenBMP(void)
{
    DefaultsDialogOpenUtility("Bitmap");
}

static void DefaultsDialogOpenPCX(void)
{
    DefaultsDialogOpenUtility("PCX");
}

static void DefaultsDialogOpenTGA(void)
{
    DefaultsDialogOpenUtility("Targa");
}

static void DefaultsDialogOpenTIF(void)
{
    DefaultsDialogOpenUtility("TIFF");
}

static void DefaultsDialogOpenGIF(void)
{
    DefaultsDialogOpenUtility("GIF");
}

static void DefaultsDialogOpenUtility(LPCSTR pszType)
{
    char szSection[64];
    int result;
    LPCTRLINITDATA pOpenData;
    OPTIONSUSERDATA oud;
    
    wsprintf(szSection, szOptionsSection, pszType);
    assert(strlen(szSection) < sizeof(szSection));
	strcpy(szTimingOptionsSection, szSection);
    memset(&oud, 0, sizeof(oud));
    oud.pszType = szSection;
    
    if ( !GetCtrlDefaults(szSection, &pOpenData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return;
        }
    /*#### F2D doesn't yet deal with 32-bit or 16-bit DIB's (and UTL doesn't convert) */
    CtrlDataSetInactive(pOpenData, IDC_OF2DOPT_RADIO_32BITS);
    CtrlDataSetInactive(pOpenData, IDC_OF2DOPT_RADIO_16BITS);

    CtrlDataSetPushed(pOpenData, IDC_OF2DOPT_CHECK_SETDEFAULT, TRUE);
    CtrlDataSetInactive(pOpenData, IDC_OF2DOPT_CHECK_SETDEFAULT);
    CtrlDataSetInactive(pOpenData, IDTEST);
    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_OF2DOPT),
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

    CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szSection, pOpenData);
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
static void CleanupOpenUtility(
    BYTE PICHUGE*    pDib,
    PIC_PARM PICFAR* pDibPicParm,
    VOID PICFAR*     pOpenData)
{
    MiscGlobalFreePtr(&pDib);
    if ( pDibPicParm != 0 )
        {
		MiscGlobalFreePtr(&pDibPicParm->u.F2D.AuxSpace);
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

    /* if screen bit depth is <= 8, make sure default bits/pixel isn't greater */
    if ( MiscIsPaletteDisplay(&num) )
        {
        /* opcode 82 won't deal with 32 bits or 16 bits so this prevents them
            on a palettized display */
        CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_32BITS);
        CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_16BITS);
        if ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_32BITS) ||
             CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_16BITS) )
            CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_24BITS, TRUE);
        if ( ToolDefUtility.nFoundParmVer == 0 )
            {
            CtrlDataSetInactive(*ppOpenData, IDC_OF2DOPT_RADIO_24BITS);
            if ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_24BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_8BITS, TRUE);
            if ( num < 8 )
                CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_4BITS, TRUE);
            }
        else if ( num < 8 )
            {
            if ( CtrlDataIsPushed(*ppOpenData, IDC_OF2DOPT_RADIO_8BITS) )
                CtrlDataSetPushed(*ppOpenData, IDC_OF2DOPT_RADIO_4BITS, TRUE);
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
    char sz[_MAX_PATH + sizeof(" Bitmap Open Options")];
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
            if ( poud->pSrcPicParm->Head.biCompression == BI_GIFI ||
                 poud->pSrcPicParm->Head.biCompression == BI_GIFN ||
                 poud->pSrcPicParm->Head.biCompression == BI_GIFu )
                {
                if ( poud->pSrcPicParm->Head.biCompression != BI_GIFI )
                    {
                    EnableWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_RADIO_GIFPROGRESSIVE), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_RADIO_GIFFINAL), FALSE);
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OF2DOPT_RADIO_GIFFINAL,
                        IDC_OF2DOPT_RADIO_GIFPROGRESSIVE,
                        IDC_OF2DOPT_RADIO_GIFFINAL);
                    }
                if ( ( poud->pSrcPicParm->u.QRY.PicFlags & PF_IsTransparency ) == 0 )
                    EnableWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_CHECK_GIFTRANSPARENT), FALSE);
                }                    
            else
                {
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_STATIC_GIFOPTIONS), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_CHECK_GIFTRANSPARENT), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_RADIO_GIFPROGRESSIVE), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_RADIO_GIFFINAL), SW_HIDE);
                }               
            GetWindowText(poud->hWnd, sz, sizeof(sz) - ( sizeof(" Bitmap Open Options") - 1 ));
            strcat(sz, " ");
            _fstrcat(sz, poud->pszType);
            strcat(sz, " Open Options");
            SetWindowText(hwndDlg, sz);
            ImageGrayedCtrls(hwndDlg, arOpenData, poud);
            break;
            
		case IDC_OF2DOPT_CHECK_IMAGESETTINGS:
            if ( wNotifyCode == BN_CLICKED )
            {
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
            }
            break;
            
        case IDC_OF2DOPT_CHECK_MAKEGRAY:
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
                        IDC_OF2DOPT_CHECK_MAKEGRAY,
                        !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY));
                    }                    

                /* make sure the bit depth is 8 bits or 4 bits */
                if ( !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) &&
                     !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OF2DOPT_RADIO_32BITS,
                        IDC_OF2DOPT_RADIO_1BITS,
                        IDC_OF2DOPT_RADIO_8BITS);

                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;                

        case IDC_OF2DOPT_RADIO_32BITS:
        case IDC_OF2DOPT_RADIO_24BITS:
        case IDC_OF2DOPT_RADIO_16BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* uncheck make gray button for bit depths > 8 */
                CheckDlgButton(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY, FALSE);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OF2DOPT_RADIO_8BITS:
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

        case IDC_OF2DOPT_RADIO_4BITS:
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
        
        case IDC_OF2DOPT_RADIO_1BITS:
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
        
        case IDC_OF2DOPT_RADIO_MAKEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;


        case IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_OF2DOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                ImageGrayedCtrls(hwndDlg, arOpenData, poud);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OF2DOPT_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* browse for a suitable external palette file.  If the file doesn't have
                    a 16-color palette, and we click 4-bits, then IDOK will report the
                    error */
                if ( MiscPaletteBrowse(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OF2DOPT_CHECK_SETDEFAULT:
            // this prevents modification of this control from setting the modified
            // flag which would a repaint of the image if OK were pushed
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OF2DOPT);
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
                    CleanupOpenUtility(pOldDib, &OldDibPicParm, 0);
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
    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) )
        maxColors = 256;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) )
        maxColors = 16;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
        maxColors = 2;
    else
        {
        /* unless the # bits/pixel <= 8, there can't be an inconsistency in the settings
            unless there's a bug */
        assert(!IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY) &&
               ( poud->pSrcPicParm->u.F2D.PicFlags & PF_IsGray ) == 0 );
        return ( TRUE );
        }

	if ( GetDlgItemInt(hwndDlg, IDC_OF2DOPT_EDIT_IMAGENUMBER, &fValid, FALSE) > poud->pSrcPicParm->u.QRY.NumImages )
	{
        ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 1, poud->pSrcPicParm->u.QRY.NumImages);
        /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_IMAGENUMBER));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_IMAGENUMBER), 0, -1);
        return ( FALSE );
	}

    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY) )
        {
        /* validate number of grays to make */
        n = GetDlgItemInt(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, &fValid, FALSE);
        if ( !fValid || n < 2 || n > maxColors )
            {
            ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
            /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
            SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE), 0, -1);
            return ( FALSE );
            }
        }
    else if ( ( poud->pSrcPicParm->u.F2D.PicFlags & PF_IsGray ) == 0 )
        {
        /* not making grayscale and source image isn't grayscale
            #### TODO: allow grayscale palette choices for gray scale and make gray */
        if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE) )
            {
            assert(( poud->pSrcPicParm->Head.biClrImportant >= 2 &&
                     poud->pSrcPicParm->Head.biClrImportant <= (unsigned)maxColors ) ||
                   ( poud->pSrcPicParm->Head.biClrUsed -
                        poud->pSrcPicParm->Head.biClrImportant >= 2 &&
                     poud->pSrcPicParm->Head.biClrUsed -
                        poud->pSrcPicParm->Head.biClrImportant <= (unsigned)maxColors ));
            }
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE) )
            {
            /* validate number of colors to make */
            n = GetDlgItemInt(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE, &fValid, FALSE);
            if ( !fValid || n < 2 || n > maxColors )
                {
                ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
                /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
                SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE), 0, -1);
                return ( FALSE );
                }
            }
        else
            {
            /* make sure external palette actually has a suitable palette */
            assert(IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EXTERNALPALETTE));
            GetDlgItemText(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            n = MiscPaletteColors(sz, 0, maxColors);
            if ( n == 0 )
                {
                SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE), 0, -1);
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
	BOOL fImageSettings = IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_IMAGESETTINGS);
    BOOL fMakeGray = IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY);
    BOOL fEnableColorPalette = !fMakeGray;

    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_DITHER,        !fImageSettings);
	CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_MAKEGRAY,      !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_EDIT_GRAYSTOMAKE,    !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_GRAYS,        !fImageSettings);

	CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_BITSPERPIXEL, !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_32BITS,        !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_24BITS,        !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_16BITS,        !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS,         !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS,         !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS,         !fImageSettings);

	if ( fImageSettings )
	{
	    EnableColorPalette(hwndDlg, ar, FALSE);
	    return;
	}

	CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_MAKEGRAY, TRUE);
    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
        {
        /* target bit depth <= 8 */

        /* enable make gray options */
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_DITHER, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_GRAYS, fMakeGray);

        /* disable bit depths > 8 if makegray is checked,
            if the src image is grayscale, then the openimage initialization
            will have inactivated these settings and the following won't matter
            (or hurt) */
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_32BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_24BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_16BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS, fMakeGray);
        }
    else
        {
        /* these don't make sense for bit depths > 8 */
        CtrlEnable(
            hwndDlg,
            ar,
            IDC_OF2DOPT_CHECK_DITHER,
            IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_16BITS));
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_GRAYS,     FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_32BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_24BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_16BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS, TRUE);
        fEnableColorPalette = FALSE;
        }
    
    EnableColorPalette(hwndDlg, ar, fEnableColorPalette);
	if ( !fEnableColorPalette )
		return;

    /* for image options dialog for a displayed image,
        make sure embedded palette choices are consistent with the image's palette.
        if the source image doesn't have any palette, then the openimage init will
        have inactivated the embeddedpalette choice so none of this will matter
        (or hurt) */
    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE) )
        {
        switch ( poud->pSrcPicParm->Head.biBitCount )
        {
            case 8:
                if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) ||
                     IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE, TRUE);
                    }
                else
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS, TRUE);
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS, FALSE);
                    }
                break;
            case 4:
                if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) ||
                     IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE, TRUE);
                    }
                else
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS, TRUE);
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS, FALSE);
                    }
                break;
            case 1:
                if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) ||
                     IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) )
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
                    CheckDlgButton(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE, TRUE);
                    }
                else
                    {
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS, FALSE);
                    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS, TRUE);
                    }
                break;
        }
        }
    else
        {
        if ( ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) &&
               poud->pSrcPicParm->Head.biBitCount == 8 ) ||
             ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) &&
               poud->pSrcPicParm->Head.biBitCount == 4 ) ||
             ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) &&
               poud->pSrcPicParm->Head.biBitCount == 1 ) )
            CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, TRUE);
        else
            CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS, TRUE);
        }
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
    OPTIONSUSERDATA PICFAR* poud = (OPTIONSUSERDATA PICFAR*)pUserData;
    
    if ( pszDlgTemplate == MAKEINTRESOURCE(IDD_TIMING) )
        return ( nCtrlID == IDOK );

    switch ( nCtrlID )
        {
        case -1:    /* for WM_INITDIALOG - the virtue is it saves us having to have a separarte
                        function passed to our dialog box handler */
            if ( _fstrstr(poud->pszType, "GIF") == 0 )
                {
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_STATIC_GIFOPTIONS), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_CHECK_GIFTRANSPARENT), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_RADIO_GIFPROGRESSIVE), SW_HIDE);
                ShowWindow(GetDlgItem(hwndDlg, IDC_OF2DOPT_RADIO_GIFFINAL), SW_HIDE);
                }
            SetWindowText(hwndDlg, poud->pszType);
            DefaultsGrayedCtrls(hwndDlg, arOpenData);
            break;
            
		case IDC_OF2DOPT_CHECK_IMAGESETTINGS:
            if ( wNotifyCode == BN_CLICKED )
            {
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
            }
            break;
            
        case IDC_OF2DOPT_CHECK_MAKEGRAY:
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
                    IDC_OF2DOPT_CHECK_MAKEGRAY,
                    !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY));

                /* make sure the bit depth is 8 bits or 4 bits */
                if ( !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) &&
                     !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
                    CheckRadioButton(
                        hwndDlg,
                        IDC_OF2DOPT_RADIO_32BITS,
                        IDC_OF2DOPT_RADIO_1BITS,
                        IDC_OF2DOPT_RADIO_8BITS);

                /* set default grays to make according to bit depth, if not yet set */
                SetGraysToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;                

        case IDC_OF2DOPT_RADIO_32BITS:
        case IDC_OF2DOPT_RADIO_24BITS:
        case IDC_OF2DOPT_RADIO_16BITS:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* uncheck make gray button for bit depths > 8 */
                CheckDlgButton(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY, FALSE);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OF2DOPT_RADIO_8BITS:
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

        case IDC_OF2DOPT_RADIO_4BITS:
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
        
        case IDC_OF2DOPT_RADIO_1BITS:
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
        
        case IDC_OF2DOPT_RADIO_MAKEPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* set default colors to make according to bit depth, if not yet set */
                SetColorsToMake(hwndDlg);
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;


        case IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE:
        case IDC_OF2DOPT_RADIO_EXTERNALPALETTE:
            if ( wNotifyCode == BN_CLICKED )
                {
                DefaultsGrayedCtrls(hwndDlg, arOpenData);
                DlgSetModified(hwndDlg);
                }
            break;

        case IDC_OF2DOPT_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                /* browse for a suitable external palette file.  If the file doesn't have
                    a 16-color palette, and we click 4-bits, then IDOK will report the
                    error */
                if ( MiscPaletteBrowse(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, 256) )
                    DlgSetModified(hwndDlg);
                }
            break;

        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OF2DOPT);
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
    
    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) )
        maxColors = 256;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) )
        maxColors = 16;
    else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
        maxColors = 2;
    else
        return ( TRUE );

    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY) )
        {
        /* validate number of grays to make */
        n = GetDlgItemInt(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, &fValid, FALSE);
        if ( !fValid || n < 2 || n > maxColors )
            {
            ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
            /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
            SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE));
            Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE), 0, -1);
            return ( FALSE );
            }
        }
    else
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE) )
            {
            /* validate number of colors to make */
            n = GetDlgItemInt(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE, &fValid, FALSE);
            if ( !fValid || n < 2 || n > maxColors )
                {
                ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, maxColors);
                /* "The number is invalid or out of range.  Please enter a number from 2 to %d." */
                SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE), 0, -1);
                return ( FALSE );
                }
            }
        else if ( !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE) )
            {
            /* make sure external palette actually has a suitable palette */
            assert(IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EXTERNALPALETTE));
            GetDlgItemText(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE, sz, sizeof(sz));
            n = MiscPaletteColors(sz, 0, maxColors);
            if ( n == 0 )
                {
                SetFocus(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE));
                Edit_SetSel(GetDlgItem(hwndDlg, IDC_OF2DOPT_EDIT_EXTERNALPALETTE), 0, -1);
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
	BOOL fImageSettings = IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_IMAGESETTINGS);
    BOOL fMakeGray = IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY);
    BOOL fEnableColorPalette = !fMakeGray;

    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_DITHER,        !fImageSettings);
	CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_MAKEGRAY,      !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_EDIT_GRAYSTOMAKE,    !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_GRAYS,        !fImageSettings);

	CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_BITSPERPIXEL, !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_32BITS,        !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_24BITS,        !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_16BITS,        !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_8BITS,         !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_4BITS,         !fImageSettings);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_1BITS,         !fImageSettings);

	if ( fImageSettings )
	{
	    EnableColorPalette(hwndDlg, ar, FALSE);
	    return;
	}

    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) ||
         IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
        {
        /* target bit depth <= 8 */

        /* enable make gray options */
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_CHECK_DITHER, TRUE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_GRAYS, fMakeGray);

        /* disable bit depths > 8 if makegray is checked,
            if the src image is grayscale, then the openimage initialization
            will have inactivated these settings and the following won't matter
            (or hurt) */
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_32BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_24BITS, !fMakeGray);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_16BITS, !fMakeGray);
        }
    else
        {
        /* these don't make sense for bit depths > 8 */
        CtrlEnable(
            hwndDlg,
            ar,
            IDC_OF2DOPT_CHECK_DITHER,
            IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_16BITS));
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, FALSE);
        CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_GRAYS,     FALSE);

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
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_STATIC_COLORPALETTE,   bEnabled);

    /* radio buttons */
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EMBEDDEDPALETTE, bEnabled);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_MAKEPALETTE,     bEnabled);
    CtrlEnable(hwndDlg, ar, IDC_OF2DOPT_RADIO_EXTERNALPALETTE, bEnabled);

    /* make palette options */
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OF2DOPT_EDIT_COLORSTOMAKE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OF2DOPT_STATIC_COLORS,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE));

    /* external palette options */
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OF2DOPT_STATIC_EXTERNALPALETTE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EXTERNALPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OF2DOPT_EDIT_EXTERNALPALETTE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EXTERNALPALETTE));
    CtrlEnable(
        hwndDlg,
        ar,
        IDC_OF2DOPT_BUTTON_BROWSE,
        bEnabled && IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_EXTERNALPALETTE));
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
    if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY) )
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) )
            SetNumToMake(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, 256);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) )
            SetNumToMake(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, 4);
        else
            {
            assert(IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS));
            SetNumToMake(hwndDlg, IDC_OF2DOPT_EDIT_GRAYSTOMAKE, 2);
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
    if ( !IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_CHECK_MAKEGRAY) &&
         IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_MAKEPALETTE) )
        {
        if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_8BITS) )
            SetNumToMake(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE, 256);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_4BITS) )
            SetNumToMake(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE, 16);
        else if ( IsDlgButtonChecked(hwndDlg, IDC_OF2DOPT_RADIO_1BITS) )
            SetNumToMake(hwndDlg, IDC_OF2DOPT_EDIT_COLORSTOMAKE, 2);
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
