/******************************************************************************
 *
 * MTUTIL82.C - Minerva MDI utility opcode
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
 *  Op82CreatePalette - create an optimum palette for a DIB
 *  Op82BitDepth - create a new dib with bit depth dwBits
 *  Op82GrayScale - create a new grayscale dib with destination bit depth dwBits
 *  Op82MakePalette - create a new dib with an optimum palette
 *  Op82ExternalPalette - create a new dib in terms of a supplied palette
 *
 * Local functions:
 *
 *   ** global function helper **
 *     Op82Misc - all the work for the Op82... functions above (except Op82CreatePalette)
 *
 *   ** opcode def functions **
 *     ToolMakePalette - "Tools/Make Palette" menu
 *     ToolMakePaletteDefaultsDialog File/Default Options/Tools/Make Palette Defaults" menu
 *
 *   ** opcode helper functions **
 *     OnCommand - options dialog WM_COMMAND handler
 *     OptionsValid - validate all the dialog settings on OK or Apply
 *     AddImageToListBox - verify and add image file name to list box
 *     ImageFileColorHistogram - add color data from image file to color frequencies
 *     ImageColorHistogram - add color data from image data in memory to color frequencies
 *
 * Revision History:
 *   05-07-97     jrb    list box del wasn't working at all.
 *                       OFN_EXPLORER style screwed up if the Nth filename were shorter
 *                         than the N+1th filename
 *                       turn on the hour glass while the palette is being created
 *   11-19-96     jrb    created
 *
 ******************************************************************************/



#define USE_COMPOSITE_SUBCODES  ( FALSE )



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
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
#include "minerva.h" 
#include "mmisc.h"
#include "mchild.h"
#include "mopen.h"
#include "merror.h"
#include "mframe.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static void ToolMakePalette(void);
static void ToolMakePaletteDefaultsDialog(void);
static BOOL OnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData);
static BOOL OptionsValid(HWND hwndDlg);
static BOOL AddImageToListBox(LPCSTR pszFilename, DWORD dwParam);
static BOOL ImageFileColorHistogram(LPCSTR pszFilename, WORD PICFAR* pwHistogram, DWORD *pdwTime);
static BOOL ImageColorHistogram(
    BYTE PICHUGE* pDib,
    DWORD dwSize,
    PIC_PARM PICFAR* pp,
    WORD PICFAR* pwHistogram);
static BOOL Op82Misc(
    DWORD dwSubOpcode,
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    RGBQUAD *pColorTable,
    const BYTE PICHUGE* pDib,     DWORD         dwDibLen,   const PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm);
static RESPONSE PegasusReqInit(PIC_PARM PICFAR *p);
static RESPONSE PegasusReqExec(PIC_PARM PICFAR* p);



/******************************************************************************
 *  global data
 ******************************************************************************/



TOOLSOP ToolDefUtility =
    {
    OP_UTL,
    "Color Utility Functions",
    "&Make Palette Defaults",
    "&Make Palette",
    0,                  /* no Init needed */
    ToolMakePalette,
    ToolMakePaletteDefaultsDialog,
    0                   /* nFoundParmVer */
    };



/******************************************************************************
 *  local data
 ******************************************************************************/



/* ini section for image options */
static char szOptionsSection[] = "Make Palette Options";

/* specifying image options controls' parameters and default values */
static CTRLINITDATA InitOptionsData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Palette Filename", eEdit, IDC_TMKPAL_EDIT_FILENAME, FALSE, _MAX_PATH },
    { "Palette Colors",   eEdit, IDC_TMKPAL_EDIT_COLORS,   FALSE, 3, "256" },

    { "Display Timing",   eCheckBox, IDC_TMKPAL_CHECK_TIMING, FALSE },

    { 0, eText,       IDC_TMKPAL_STATIC_IMAGEFILES },
    { 0, eListBox,    IDC_TMKPAL_LIST_IMAGEFILES   },
    { 0, ePushButton, IDC_TMKPAL_BUTTON_ADD        },
    { 0, ePushButton, IDC_TMKPAL_BUTTON_DEL        },
    { 0, eCheckBox,   IDC_TMKPAL_CHECK_SETDEFAULT  },

    { 0, eEnd, -1 }
    };



#define EXTRACTDIB  0
#define HISTOGRAM   1
#define COLORREDUCE 2
#define FILEIO      3
#define TOTAL       4



/* specifying display timing parameters and default values, in the above order,
    following timer resolution first */
static CTRLINITDATA InitDisplayTiming[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { 0, eText, IDC_TMKPALTIME_TIMERRES,    0, sizeof("999.999") },
    { 0, eText, IDC_TMKPALTIME_EXTRACTDIB,  0, sizeof("999.999") },
    { 0, eText, IDC_TMKPALTIME_HISTOGRAM,   0, sizeof("999.999") },
    { 0, eText, IDC_TMKPALTIME_COLORREDUCE, 0, sizeof("999.999") },
    { 0, eText, IDC_TMKPALTIME_FILEIO,      0, sizeof("999.999") },
    { 0, eText, IDC_TMKPALTIME_TOTAL,       0, sizeof("999.999") },

    { 0, eEnd, -1 }
    };



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL Op82CreatePalette(dwNumColors, pDib, dwDibSize, pDibPicParm)
 * 
 *  create an optimum palette for the DIB, copying the palette to picparm.ColorTable
 *
 *  parameters:
 *      dwNumColors - number of colors to make
 *      pDib - DIBits for image
 *      dwDibSize - size of image
 *      pDibPicParm - picparm for image
 *
 *  notes:
 *      displays an error message, if applicable, but no error status is returned
 *      op82 doesn't support 16 or 32-bit depths, we don't currently convert to 24
  * 
 ******************************************************************************/
BOOL Op82CreatePalette(
    DWORD dwNumColors,
    BYTE PICHUGE* pDib, DWORD dwDibSize, PIC_PARM PICFAR* pDibPicParm)
{
    int i;
    int j;
    PIC_PARM pp;
    WORD PICFAR *pwHistogram;
    RESPONSE response;
    
    /* opcode 82 currently doesn't support 16 or 32 bit depths */
    if ( ToolDefUtility.nFoundParmVer == 0 ||
         pDibPicParm->Head.biBitCount == 16 ||
         pDibPicParm->Head.biBitCount == 32 )
        return ( FALSE );
        
    pwHistogram = (WORD PICFAR*)MiscGlobalAllocPtr(32768UL * sizeof(WORD), IDS_CREATEPALETTEOUTOFMEMORY);
    if ( pwHistogram == 0 )
        return ( FALSE );

    hmemcpy(&pp, pDibPicParm, sizeof(pp));
    if ( !ImageColorHistogram(pDib, dwDibSize, &pp, pwHistogram) )
        {
        ErrorPegasusMessage(pp.Status, IDS_CREATEPALETTEERROR);
        MiscGlobalFreePtr(&pwHistogram);
        return ( FALSE );
        }

    memset(&pp.u, 0, sizeof(pp.u));
    pp.u.UTL.Subcode = 2;
    pp.u.UTL.ptr1 = (BYTE PICHUGE*)pwHistogram;
     pp.u.UTL.HistogramSize = 32768UL;
    if ( dwNumColors == 256 )
        dwNumColors -= 20;
    pp.u.UTL.NumColors = dwNumColors;

    /* set to 0 so we can try to detect the number of colors made (see below) */
    _fmemset(pp.ColorTable, 0, sizeof(pp.ColorTable));
    
    response = Pegasus(&pp, REQ_INIT);
    if ( response == RES_DONE )
        response = Pegasus(&pp, REQ_EXEC);
    if ( response == RES_DONE )
        {
        response = Pegasus(&pp, REQ_TERM);
        assert(response == RES_DONE);
        hmemcpy(pDibPicParm->ColorTable, pp.ColorTable, pp.u.UTL.NumColors * sizeof(RGBQUAD));
        /*#### NOTE:
            opcode 82 subcode 2 doesn't return the number of colors made.  If the number of
            colors made is significantly less than the number of colors requested, windows
            takes a _very_ long time in SetDIBitsToDevice.  So we have to try to determine
            ourselves the number of colors.  As we've initially set all the colors to black
            for that reason, starting at the end of the palette, we find the first non-black
            color */
        for ( i = sizeof(pp.ColorTable) / sizeof(pp.ColorTable[0]) - 1; i >= 0; i-- )
            {
            if ( pp.ColorTable[i].rgbRed != 0 ||
                 pp.ColorTable[i].rgbBlue != 0 ||
                 pp.ColorTable[i].rgbGreen != 0 )
                break;
            }
        i++;
        /* see if there's a black palette entry preceding this i'th color */
        if ( i < (int)pp.u.UTL.NumColors )
            {
            for ( j = 0; j < i; j++ )
                if ( pp.ColorTable[j].rgbRed == 0 &&
                     pp.ColorTable[j].rgbBlue == 0 &&
                     pp.ColorTable[j].rgbGreen == 0 )
                    break;
            if ( j == i ) 
                /* if no black palette entry precedes i, then include the i'th palette
                    entry in the number of colors, we can't know if it was a created
                    color or not, but since we can just include it, it doesn't really matter */
                i++;                
            }
        /* i is the number of significant colors, possibly one larger than it absolutely
            has to be */
        pDibPicParm->Head.biClrImportant = i;
        pDibPicParm->Head.biClrUsed = i;
        }        
    else
        {
        ErrorPegasusMessage(pp.Status, IDS_CREATEPALETTEERROR);
        assert(response == RES_ERR);
        if ( response != RES_ERR )
            Pegasus(&pp, REQ_TERM);
        }        
    MiscGlobalFreePtr(&pwHistogram);
    return ( response == RES_DONE );
}



/******************************************************************************
 *  BOOL Op82BitDepth(
 *      dwBits,
 *      bDither,
 *      pDib, dwDibLen, pDibPicParm,
 *      ppXDib, pdwXDibLen, pXDibPicParm)
 * 
 *  create a new dib with bit depth dwBits
 *
 *  parameters:
 *      dwBits - number of bits per pixel for target
 *      bDither - dither target colors if appropriate
 *      pDib - DIBits for source image
 *      dwDibSize - size of source image
 *      pDibPicParm - picparm for source image
 *      ppXDib - receive pointer to DIBits for target image
 *      pdwXDibSize - size of target image
 *      pXDibPicParm - picparm for target image
 *
 * returns:
 *     FALSE if error
 * 
 * notes:
 *     This only works for upsampling, there's no provision for color matching
 ******************************************************************************/
BOOL Op82BitDepth(
    DWORD dwBits,
    BOOL  bDither,
    const BYTE PICHUGE* pDib,     DWORD         dwDibLen,   const PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm)
{
    return ( Op82Misc(
        0,
        dwBits,
        bDither,
        0,
        0,
        pDib,   dwDibLen,   pDibPicParm,
        ppXDib, pdwXDibLen, pXDibPicParm) );
}
    


/******************************************************************************
 *  BOOL Op82GrayScale(
 *      dwBits,
 *      bDither,
 *      dwNumColors,
 *      pDib, dwDibLen, pDibPicParm,
 *      ppXDib, pdwXDibLen, pXDibPicParm)
 * 
 *  create a new grayscale dib with destination bit depth dwBits
 *
 *  parameters:
 *      dwBits - number of bits per pixel for target
 *      bDither - TRUE to dither
 *      dwNumColors - number of grays to make
 *      pDib - DIBits for source image
 *      dwDibSize - size of source image
 *      pDibPicParm - picparm for source image
 *      ppXDib - receive pointer to DIBits for target image
 *      pdwXDibSize - size of target image
 *      pXDibPicParm - picparm for target image
 *
 * returns:
 *     FALSE if error
 * 
 ******************************************************************************/
BOOL Op82Grayscale(
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    BYTE PICHUGE* pDib,           DWORD         dwDibLen,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm)
{
    return ( Op82Misc(
        4,
        dwBits,
        bDither,
        dwNumColors,
        0,
        pDib,   dwDibLen,   pDibPicParm,
        ppXDib, pdwXDibLen, pXDibPicParm) );
}



/******************************************************************************
 *  BOOL Op82ExternalPalette(
 *      dwBits,
 *      bDither,
 *      dwNumColors,
 *      pDib, dwDibLen, pDibPicParm,
 *      ppXDib, pdwXDibLen, pXDibPicParm)
 * 
 *  create a new dib in terms of a supplied palette
 *
 *  parameters:
 *      dwBits - number of bits per pixel for target
 *      bDither - TRUE to dither
 *      dwNumColors - number of colors to make
 *      pColorTable - colors to use
 *      pDib - DIBits for source image
 *      dwDibSize - size of source image
 *      pDibPicParm - picparm for source image
 *      ppXDib - receive pointer to DIBits for target image
 *      pdwXDibSize - size of target image
 *      pXDibPicParm - picparm for target image
 *
 * returns:
 *     FALSE if error
 * 
 ******************************************************************************/
BOOL Op82ExternalPalette(
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    RGBQUAD *pColorTable,
    BYTE PICHUGE* pDib,           DWORD         dwDibLen,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm)
{
#if !defined(USE_COMPOSITE_SUBCODES) || !USE_COMPOSITE_SUBCODES

    PIC_PARM pp = *pDibPicParm;
    BYTE PICHUGE* pMap = MiscGlobalAllocPtr(32768UL, IDS_CREATEPALETTEOUTOFMEMORY);
    RESPONSE response;

    pp.Op = OP_UTL;
    hmemcpy(pp.ColorTable, pColorTable, sizeof(RGBQUAD) * dwNumColors);
    pp.Head.biClrUsed = dwNumColors;
    pp.Head.biClrImportant = dwNumColors;
    memset(&pp.u, 0, sizeof(pp.u));
    pp.u.UTL.Subcode   = 3;
    pp.u.UTL.NumColors = dwNumColors;
    pp.u.UTL.ptr2      = pMap;
    
    response = Pegasus(&pp, REQ_INIT);
    if ( response == RES_DONE )
        {
        response = Pegasus(&pp, REQ_EXEC);
        if ( response == RES_DONE )
            response = Pegasus(&pp, REQ_TERM);
        }            
    assert(response == RES_DONE);
    if ( response != RES_DONE )
        {
        MiscGlobalFreePtr(&pMap);
        return ( FALSE );
        }

    *pXDibPicParm = *pDibPicParm;
    _fmemset(&pXDibPicParm->u, 0, sizeof(pXDibPicParm->u));
    pXDibPicParm->Op = OP_UTL;
    pXDibPicParm->u.UTL.Subcode = 4;
    pXDibPicParm->u.UTL.ptr2 = pp.u.UTL.ptr2;
    pXDibPicParm->u.UTL.ptr3 = (BYTE PICHUGE*)pp.ColorTable;
    pXDibPicParm->u.UTL.OutBpp = dwBits;
    pXDibPicParm->u.UTL.NumColors = dwNumColors;
    if ( bDither )
        pXDibPicParm->u.UTL.PicFlags |= PF_Dither;
    
    /* NOTE:
        Get.End has to be 1 byte larger than the buffer really is.  This is because
        opcode 82 implements very strict queueing and "corrects" the queue as needed.
        In this case, if End isn't one byte bigger, so Rear is set to End, 82 SETS REAR
        TO START (wrapping Rear around).  In strict queueing theory this is OK in theory
        for a synchronous queue, but questionable in practice because the opcode should
        not be changing Rear for the Get queue.  In any case, the result when 82
        corrects Rear is that it thinks the queue is empty and returns RES_GET_NEED_DATA
        -- even though we've set AllocType to 1 and even though we've set Q_EOF.

        Setting End 1 byte larger and Rear 1 byte less than End is a silly hack that
        happens to work only because 82 doesn't actually read the extra byte which isn't
        really there */
    pXDibPicParm->Get.Start = (BYTE PICHUGE*)pDib;
    pXDibPicParm->Get.End   = pXDibPicParm->Get.Start + dwDibLen + 1;
    pXDibPicParm->Get.Front = pXDibPicParm->Get.Start;
    pXDibPicParm->Get.Rear  = pXDibPicParm->Get.End - 1;
    pXDibPicParm->Get.QFlags = Q_EOF;
        
    response = PegasusReqInit(pXDibPicParm);
    if ( response == RES_ERR )
        {
        MiscGlobalFreePtr(&pMap);
        if ( pXDibPicParm->Status != ERR_NONE )
            ErrorPegasusMessage(pXDibPicParm->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        FrameWaitCursorOff();
        return ( FALSE );
        }

    /**************************************************************************
        Allocate output DIB buffer according to the height and widthpad (scan line
        width including dword padding) returned from REQ_INIT
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    *pdwXDibLen = labs(pXDibPicParm->Head.biHeight) *
        ( ( pXDibPicParm->Head.biWidth * pXDibPicParm->u.UTL.OutBpp + 31 ) & ~31 ) / 8;
    *ppXDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwXDibLen, IDS_IMAGEOUTOFMEMORY);
    if ( *ppXDib == 0 )
        {
        MiscGlobalFreePtr(&pMap);
        Pegasus(pXDibPicParm, REQ_TERM);
        FrameWaitCursorOff();
        return ( FALSE );
        }

    /* initialize put queue buffer pointers */
    pXDibPicParm->Put.Start = *ppXDib;
    pXDibPicParm->Put.End = pXDibPicParm->Put.Start + *pdwXDibLen + 1;  /* see pp->Get initialization notes */

    pXDibPicParm->Put.Front = pXDibPicParm->Put.Start;
    pXDibPicParm->Head.biHeight = labs(pXDibPicParm->Head.biHeight);
    pXDibPicParm->Put.Rear = pXDibPicParm->Put.Front;         /* initially, nothing has been put in the queue */

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */

    response = PegasusReqExec(pXDibPicParm);
    if ( response == RES_ERR )
        {
        MiscGlobalFreePtr(&pMap);
        MiscGlobalFreePtr(ppXDib);
        if ( pXDibPicParm->Status != ERR_NONE )
            ErrorPegasusMessage(pXDibPicParm->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        FrameWaitCursorOff();
        return ( FALSE );
        }

    /**************************************************************************
        Pegasus(REQ_TERM)
    */
    response = Pegasus(pXDibPicParm, REQ_TERM);
    assert(response == RES_DONE);

    hmemcpy(&pXDibPicParm->Head, &pXDibPicParm->u.UTL.BiOut, sizeof(pXDibPicParm->Head));
    hmemcpy(
        &pXDibPicParm->ColorTable,
        pXDibPicParm->u.UTL.ptr3,
        sizeof(RGBQUAD) * pXDibPicParm->Head.biClrImportant);
    MiscGlobalFreePtr(&pMap);
    FrameWaitCursorOff();
    return ( TRUE );

#else

    return ( Op82Misc(
        8,
        dwBits,
        bDither,
        dwNumColors,
        pColorTable,
        pDib,   dwDibLen,   pDibPicParm,
        ppXDib, pdwXDibLen, pXDibPicParm) );

#endif
}



/******************************************************************************
 *  BOOL Op82MakePalette(
 *      dwBits,
 *      bDither,
 *      dwNumColors,
 *      pDib, dwDibLen, pDibPicParm,
 *      ppXDib, pdwXDibLen, pXDibPicParm)
 * 
 *  create a new dib with an optimum palette
 *
 *  parameters:
 *      dwBits - number of bits per pixel for target
 *      bDither - TRUE to dither
 *      dwNumColors - number of colors to make
 *      pDib - DIBits for source image
 *      dwDibSize - size of source image
 *      pDibPicParm - picparm for source image
 *      ppXDib - receive pointer to DIBits for target image
 *      pdwXDibSize - size of target image
 *      pXDibPicParm - picparm for target image
 *
 * returns:
 *     FALSE if error
 * 
 ******************************************************************************/
BOOL Op82MakePalette(
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    BYTE PICHUGE* pDib,           DWORD         dwDibLen,   PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm)
{
#if !defined(USE_COMPOSITE_SUBCODES) || !USE_COMPOSITE_SUBCODES

    PIC_PARM pp = *pDibPicParm;

    return (
        Op82CreatePalette(dwNumColors, pDib, dwDibLen, &pp) &&
        Op82ExternalPalette(
            dwBits,
            bDither,
            pp.Head.biClrUsed,
            pp.ColorTable,
            pDib, dwDibLen, pDibPicParm,
            ppXDib, pdwXDibLen, pXDibPicParm) );

#else

    return ( Op82Misc(
        7,
        dwBits,
        bDither,
        dwNumColors,
        0,
        pDib,   dwDibLen,   pDibPicParm,
        ppXDib, pdwXDibLen, pXDibPicParm) );

#endif  // #if !defined(USE_COMPOSITE_OPCODES) || !USE_COMPOSITE_OPCODES
}



/******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL Op82Misc(
 *      dwSubOpcode
 *      dwBits,
 *      bDither,
 *      dwNumColors,
 *      pColorTable
 *      pDib, dwDibLen, pDibPicParm,
 *      ppXDib, pdwXDibLen, pXDibPicParm)
 * 
 *  all the work for the Op82... functions above (except Op82CreatePalette)
 *
 *  parameters:
 *      dwSubOpcode - opcode 82 subcode to use
 *      dwBits - number of bits per pixel for target
 *      bDither - TRUE to dither
 *      dwNumColors - number of colors to make (where applicable)
 *      pColorTable - color table to use (where applicable)
 *      pDib - DIBits for source image
 *      dwDibSize - size of source image
 *      pDibPicParm - picparm for source image
 *      ppXDib - receive pointer to DIBits for target image
 *      pdwXDibSize - size of target image
 *      pXDibPicParm - picparm for target image
 *
 * returns:
 *     FALSE if error
 * 
 ******************************************************************************/
static BOOL Op82Misc(
    DWORD dwSubOpcode,
    DWORD dwBits,
    BOOL  bDither,
    DWORD dwNumColors,
    RGBQUAD *pColorTable,
    const BYTE PICHUGE* pDib,     DWORD         dwDibLen,   const PIC_PARM PICFAR* pDibPicParm,
    BYTE PICHUGE* PICFAR* ppXDib, DWORD PICFAR* pdwXDibLen, PIC_PARM PICFAR* pXDibPicParm)
{
    PIC_PARM PICFAR* pp = pXDibPicParm;
    RESPONSE response;
    WORD i;
    
    FrameWaitCursorOn();
    hmemcpy(pp, pDibPicParm, sizeof(*pp));
    _fmemset(&pp->u, 0, sizeof(pp->u));

    pp->Op = OP_UTL;
    pp->u.UTL.Subcode = dwSubOpcode;
    if ( pp->u.UTL.Subcode == 4 )
        pp->u.UTL.PicFlags = PF_ConvertGray;        
    
    pp->u.UTL.OutBpp = dwBits;
    if ( bDither )
        pp->u.UTL.PicFlags |= PF_Dither;
    if ( pColorTable != 0 )
        pp->u.UTL.ptr3 = (BYTE PICHUGE*)pColorTable;
    pp->u.UTL.NumColors = dwNumColors;
    
    /* NOTE:
        Get.End has to be 1 byte larger than the buffer really is.  This is because
        opcode 82 implements very strict queueing and "corrects" the queue as needed.
        In this case, if End isn't one byte bigger, so Rear is set to End, 82 SETS REAR
        TO START (wrapping Rear around).  In strict queueing theory this is OK in theory
        for a synchronous queue, but questionable in practice because the opcode should
        not be changing Rear for the Get queue.  In any case, the result when 82
        corrects Rear is that it thinks the queue is empty and returns RES_GET_NEED_DATA
        -- even though we've set AllocType to 1 and even though we've set Q_EOF.

        Setting End 1 byte larger and Rear 1 byte less than End is a silly hack that
        happens to work only because 82 doesn't actually read the extra byte which isn't
        really there */
    pp->Get.Start = (BYTE PICHUGE*)pDib;
    pp->Get.End   = pp->Get.Start + dwDibLen + 1;
    pp->Get.Front = pp->Get.Start;
    pp->Get.Rear  = pp->Get.End - 1;
    pp->Get.QFlags = Q_EOF;
        
    response = PegasusReqInit(pp);
    if ( response == RES_ERR )
        {
        if ( pp->Status != ERR_NONE )
            ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        FrameWaitCursorOff();
        return ( FALSE );
        }

    /**************************************************************************
        Allocate output DIB buffer according to the height and widthpad (scan line
        width including dword padding) returned from REQ_INIT
        It wouldn't be that hard to do it before REQ_INIT, but we're guaranteed that
        REQ_INIT won't require the output buffer so this is easiest
    */

    *pdwXDibLen = labs(pp->Head.biHeight) *
        ( ( pp->Head.biWidth * pp->u.UTL.OutBpp + 31 ) & ~31 ) / 8;
    *ppXDib = (BYTE PICHUGE*)MiscGlobalAllocPtr(*pdwXDibLen, IDS_IMAGEOUTOFMEMORY);
    if ( *ppXDib == 0 )
        {
        Pegasus(pp, REQ_TERM);
        FrameWaitCursorOff();
        return ( FALSE );
        }

    /* initialize put queue buffer pointers */
    pp->Put.Start = *ppXDib;
    pp->Put.End = pp->Put.Start + *pdwXDibLen + 1;  /* see pp->Get initialization notes */

    pp->Put.Front = pp->Put.Start;
    pp->Head.biHeight = labs(pp->Head.biHeight);
    pp->Put.Rear = pp->Put.Front;         /* initially, nothing has been put in the queue */

    /**************************************************************************
        Pegasus(REQ_EXEC)
    */

    response = PegasusReqExec(pp);
    if ( response == RES_ERR )
        {
        MiscGlobalFreePtr(ppXDib);
        if ( pp->Status != ERR_NONE )
            ErrorPegasusMessage(pp->Status, IDS_IMAGEOPEN);
        /* "The image could not be expanded." */
        FrameWaitCursorOff();
        return ( FALSE );
        }

    /**************************************************************************
        Pegasus(REQ_TERM)
    */
    response = Pegasus(pp, REQ_TERM);
    assert(response == RES_DONE);

    hmemcpy(&pp->Head, &pp->u.UTL.BiOut, sizeof(pp->Head));
    if ( pp->u.UTL.PicFlags & PF_ConvertGray )
        {
        /*#### NOTE:
            unfortunately, the gray scale color table isn't returned by
            the opcode so it has to be constructed here.  Some day this
            won't be required */
        for ( i = 0; i < pp->u.UTL.NumColors; i++ )
            {
                pp->ColorTable[i].rgbRed = pp->ColorTable[i].rgbGreen = pp->ColorTable[i].rgbBlue =
                     (BYTE)( ( i * 255 ) / ( pp->u.UTL.NumColors - 1 ) );
                pp->ColorTable[i].rgbReserved = 0;
            }
        }
    else if ( pp->u.UTL.ptr3 != 0 )
        {
        hmemcpy(pp->ColorTable, pp->u.UTL.ptr3, pp->u.UTL.NumColors * sizeof(RGBQUAD));
        }        
    FrameWaitCursorOff();
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
                p->Put.QFlags |= Q_IO_ERR;
                break;

            case RES_GET_NEED_DATA:
                p->Get.QFlags |= Q_IO_ERR;
                break;
                
            case RES_SEEK:
                if ( p->SeekInfo == 0 )
                    p->Get.Front = p->Get.Start;
                break;

            default:
                //assert(FALSE);
                break;
            }
        response = Pegasus(p, REQ_CONT);
        }
    return ( response );                       
}



/******************************************************************************
 *  void ToolMakePalette()
 * 
 *  "Tools/Make Palette" menu selection
 *
 ******************************************************************************/
static void ToolMakePalette(void)
{
    HFILE hf;
    OFSTRUCT of;
    int result;
    LPCTRLINITDATA pCtrlData;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    DWORD dwColors;
    PIC_PARM pp;
    RESPONSE response;
    LPSTR pszFilenames = 0;
    LPCSTR psz;
    WORD PICFAR* pwHistogram;
    char szFilename[_MAX_PATH + 1];
    DWORD dw1;
    DWORD dw2;
    DWORD dw3;
    APPDATA App;
    BOOL bDisplayTiming;
    
    memset(&App, 0, sizeof(App));

    if ( !CtrlDataDup(InitOptionsData, &pCtrlData) )
        {
        MiscGlobalFreePtr(&pwHistogram);
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return;
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szOptionsSection, pCtrlData);

    result = DlgDoModal(
        hwndFrame,
        MAKEINTRESOURCE(IDD_TMKPAL),
        OnCommand,
        pCtrlData,
        &pszFilenames);
    if ( result != IDOK || pszFilenames == 0 )
        {
        MiscGlobalFreePtr(&pwHistogram);
        CtrlDataFree(&pCtrlData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the image options dialog." */
        return;
        }             
    FrameWaitCursorOn();
    if ( CtrlDataIsPushed(pCtrlData, IDC_TMKPAL_CHECK_SETDEFAULT) )
        CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, pCtrlData);
    bDisplayTiming = CtrlDataIsPushed(pCtrlData, IDC_TMKPAL_CHECK_TIMING);
    
    dwColors     = CtrlDataGetLong(pCtrlData, IDC_TMKPAL_EDIT_COLORS, FALSE);
    CtrlDataGetText(pCtrlData, IDC_TMKPAL_EDIT_FILENAME, szFilename, sizeof(szFilename));
    CtrlDataFree(&pCtrlData);

    App.dwTime[TOTAL] -= MiscTickCount();
    pwHistogram = (WORD PICFAR*)
        MiscGlobalAllocPtr(32768UL * sizeof(WORD), IDS_CREATEPALETTEOUTOFMEMORY);
    if ( pwHistogram == 0 )
        {
        FrameWaitCursorOff();
        return;
        }
    
    psz = pszFilenames;
    assert(*psz != '\0');
    while ( *psz != '\0' )
        {
        if ( !ImageFileColorHistogram(psz, pwHistogram, App.dwTime) )
            {
            MiscGlobalFreePtr(&pszFilenames);
            MiscGlobalFreePtr(&pwHistogram);
            FrameWaitCursorOff();
            return;
            }
        psz += lstrlen(psz) + 1;
        }    
    MiscGlobalFreePtr(&pszFilenames);

    App.dwTime[COLORREDUCE] -= MiscTickCount();
    memset(&pp, 0, sizeof(pp));
    pp.ParmSize = sizeof(pp);
    pp.ParmVer = CURRENT_PARMVER;
    pp.ParmVerMinor = 1;
    pp.Op = OP_UTL;
    pp.App = (LPARAM)(APPDATA PICFAR*)&App;
    
    pp.u.UTL.Subcode = 2;
    pp.u.UTL.ptr1 = (BYTE PICHUGE*)pwHistogram;
    pp.u.UTL.NumColors = dwColors;

    response = Pegasus(&pp, REQ_INIT);
    if ( response == RES_DONE )
        response = Pegasus(&pp, REQ_EXEC);
    if ( response == RES_DONE )
        {
        response = Pegasus(&pp, REQ_TERM);
        assert(response == RES_DONE);
        }        
    else
        {
        assert(response == RES_ERR);
        if ( response != RES_ERR )
            Pegasus(&pp, REQ_TERM);
        ErrorPegasusMessage(pp.Status, IDS_MAKEPALETTE);
        MiscGlobalFreePtr(&pwHistogram);
        FrameWaitCursorOff();
        return;
        }        
    App.dwTime[COLORREDUCE] += MiscTickCount();        

    MiscGlobalFreePtr(&pwHistogram);

    memset(&bmfh, 0, sizeof(bmfh));
    memset(&bmih, 0, sizeof(bmih));

    bmfh.bfType = 'B' + 'M' * 256;
    bmfh.bfSize = sizeof(bmfh) + sizeof(bmih) + pp.u.UTL.NumColors * sizeof(RGBQUAD);

    bmih.biSize = sizeof(bmih);
    bmih.biPlanes = 1;
    bmih.biBitCount = 8;
    bmih.biCompression = BI_RGB;
    bmih.biClrUsed = pp.u.UTL.NumColors;
    bmih.biClrImportant = pp.u.UTL.NumColors;

    // create file
    // write bmfh
    // write bmih
    // write pp.ColorTable
    // close file
    App.dwTime[FILEIO] -= MiscTickCount();
    hf = OpenFile(szFilename, &of, OF_CREATE | OF_WRITE);
    if ( hf == HFILE_ERROR )
        {
        ErrorFileMessage(IDS_PALETTECREATE, of.nErrCode, (LPCSTR)szFilename);
        /* "An error occured creating the palette file %s." */
        FrameWaitCursorOff();
        return;
        }

    dw1 = _hwrite(hf, (LPCSTR)&bmfh, sizeof(bmfh));
    dw2 = _hwrite(hf, (LPCSTR)&bmih, sizeof(bmih));
    dw3 = _hwrite(hf, (LPCSTR)&pp.ColorTable, bmih.biClrUsed * sizeof(RGBQUAD));
    hf = _lclose(hf);
    assert(hf != HFILE_ERROR);
    App.dwTime[FILEIO] += MiscTickCount();
    App.dwTime[TOTAL] += MiscTickCount();
    if ( dw1 == (DWORD)-1 || dw2 == (DWORD)-1 || dw3 == (DWORD)-1 )
        {
        remove(szFilename);
        ErrorMessage(STYLE_ERROR, IDS_PALETTEWRITE);
        /* "An unexpected error occurred writing to the palette file." */
        FrameWaitCursorOff();
        return;
        }
    if ( dw1 != sizeof(bmfh) || dw2 != sizeof(bmih) || dw3 != bmih.biClrUsed * sizeof(RGBQUAD) )
        {
        remove(szFilename);
        ErrorMessage(STYLE_ERROR, IDS_PALETTEWRITE_DISKFULL);
        /* "An error occurred writing to the palette file.  The disk appears to be full." */
        FrameWaitCursorOff();
        return;
        }
    if ( bDisplayTiming )
        {
        MiscDisplayTimingDialog(hwndFrame,
            MAKEINTRESOURCE(IDD_TMKPALTIME),
            App.dwTime,
            InitDisplayTiming);
        }
    FrameWaitCursorOff();
}



/******************************************************************************
 *  void ToolMakePaletteDefaultsDialog()
 * 
 *  "File/Default Options/Tools/Make Palette Defaults" menu selection
 *
 ******************************************************************************/
static void ToolMakePaletteDefaultsDialog(void)
{
    int result;
    LPCTRLINITDATA pCtrlData;
    
    if ( !CtrlDataDup(InitOptionsData, &pCtrlData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_OPTIONSOUTOFMEMORY);
        /* "There was not enough memory to open the image options dialog." */
        return;
        }
    CtrlDataReadDefaults(APPLICATION_INIFILENAME, szOptionsSection, pCtrlData);
    CtrlDataSetPushed(pCtrlData, IDC_TMKPAL_CHECK_SETDEFAULT, TRUE);
    CtrlDataSetInactive(pCtrlData, IDC_TMKPAL_CHECK_SETDEFAULT);
    CtrlDataSetInactive(pCtrlData, IDC_TMKPAL_STATIC_IMAGEFILES);
    CtrlDataSetInactive(pCtrlData, IDC_TMKPAL_LIST_IMAGEFILES);
    CtrlDataSetInactive(pCtrlData, IDC_TMKPAL_BUTTON_ADD);
    CtrlDataSetInactive(pCtrlData, IDC_TMKPAL_BUTTON_DEL);
        
    result = DlgDoModal(hwndFrame, MAKEINTRESOURCE(IDD_TMKPAL), OnCommand, pCtrlData, NULL);
    if ( result != IDOK )
        {
        CtrlDataFree(&pCtrlData);
        if ( result == -1 )
            ErrorMessage(STYLE_ERROR, IDS_CREATEOPTIONSDIALOG);
            /* "An unexpected error occurred opening the image options dialog." */
        return;
        }             

    CtrlDataWriteDefaults(APPLICATION_INIFILENAME, szOptionsSection, pCtrlData);
    CtrlDataFree(&pCtrlData);
}



/******************************************************************************
 *  BOOL OnCommand(
 *      HWND            hwndDlg,
 *      int             nCtrlID,
 *      WORD            wNotifyCode,
 *      LPCTRLINITDATA  arOpenData,
 *      LPCSTR          pszDlgTemplate, 
 *      void PICFAR*    pUserData)
 *
 *  options dialog WM_COMMAND handler
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
static BOOL OnCommand(
    HWND            hwndDlg,
    int             nCtrlID,
    WORD            wNotifyCode,
    LPCTRLINITDATA  arOpenData,
    LPCSTR          pszDlgTemplate, 
    void PICFAR*    pUserData)
{
    HWND hwndList = GetDlgItem(hwndDlg, IDC_TMKPAL_LIST_IMAGEFILES);
    int n;
    OPENFILENAME ofn;
    static char szFilename[_MAX_PATH + 1];
    static char szFilters[256];
    int i;
    char *psz;
    LPSTR PICFAR* ppsz;
    LPSTR lpsz;

    NOREFERENCE(pUserData);
    NOREFERENCE(pszDlgTemplate);

    switch ( nCtrlID )
        {
        case -1:    /* for WM_INITDIALOG - the virtue is it saves us having to have a separarte
                        function passed to our dialog box handler */
            break;
            
        case IDC_TMKPAL_BUTTON_BROWSE:
            if ( wNotifyCode == BN_CLICKED )
                {
                i = LoadString(hinstThis, IDS_PALETTEFILTERS, szFilters, sizeof(szFilters));
                    /* "All Palettes (*.pic,*.bmp)|*.PIC;*.BMP|All Files (*.*)|*.*|" */
                assert(i != 0 && i < sizeof(szFilters) - 1);

                /* open file dialog wants the filter segments separated by '\0' */
                psz = szFilters;
                while ( ( psz = strchr(psz, '|') ) != NULL )
                    *psz++ = '\0';

                memset(&ofn, 0, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner   = hwndDlg;
                ofn.lpstrFilter = szFilters;
                ofn.lpstrFile   = szFilename;
                ofn.nMaxFile    = sizeof(szFilename);
                szFilename[0] = '\0';
                ofn.Flags       = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
                if ( GetSaveFileName(&ofn) )
                    {
                    SetDlgItemText(hwndDlg, IDC_TMKPAL_EDIT_FILENAME, szFilename);
                    DlgSetModified(hwndDlg);
                    }
                }
            break;

        case IDC_TMKPAL_LIST_IMAGEFILES:
            CtrlEnable(
                hwndDlg,
                arOpenData,
                IDC_TMKPAL_BUTTON_DEL,
                ListBox_GetSelCount(hwndList) != 0);
            break;
        
        case IDC_TMKPAL_BUTTON_ADD:
            if ( wNotifyCode == BN_CLICKED )
                {
                if ( MiscImageFileOpenDialog(hwndDlg, AddImageToListBox, (DWORD)(void PICFAR*)hwndList) &&
                     ListBox_GetCount(hwndList) != 0 )
                    {
                    DlgSetModified(hwndDlg);    /* if browse is accepted */
                    CtrlEnable(
                        hwndDlg,
                        arOpenData,
                        IDC_TMKPAL_BUTTON_DEL,
                        ListBox_GetSelCount(hwndList) != 0);
                    }
                }
            break;                
            
        case IDC_TMKPAL_BUTTON_DEL:
            if ( wNotifyCode == BN_CLICKED && ListBox_GetSelCount(hwndList) != 0 )
                {
                i = 0;
                while ( i < ListBox_GetCount(hwndList) )
                    {
                    assert(ListBox_GetSel(hwndList, i) != LB_ERR);
                    if ( ListBox_GetSel(hwndList, i) > 0 )
                        {
                        ListBox_DeleteString(hwndList, i);
                        /* as we've deleted the i'th item, there's a new i'th item to
                            be examined so we can't yet advance i */
                        }
                    else
                        {
                        i++;
                        }
                    }                        
                CtrlEnable(hwndDlg, arOpenData, IDC_TMKPAL_BUTTON_DEL, FALSE);
                DlgSetModified(hwndDlg);    /* if some list item was deleted */
                }
            break;                
            
        case IDHELP:
            WinHelp(hwndFrame, APPLICATION_HELPFILENAME, HELP_CONTENTS, IDD_OSEQJPGOPT);
            break;

        case IDOK:
            if ( !OptionsValid(hwndDlg) )
                return ( FALSE );
            if ( pUserData != NULL )
            {
                ppsz = pUserData;
                n = 0;
                for ( i = 0; i < ListBox_GetCount(hwndList); i++ )
                    {
                    n += ListBox_GetTextLen(hwndList, i) + 1;
                    }  
                n++;
                     *ppsz = (LPSTR)MiscGlobalAllocPtr(n, IDS_CREATEPALETTEOUTOFMEMORY);
                if ( *ppsz == 0 )
                    return ( TRUE );
                lpsz = *ppsz;
                for ( i = 0; i < ListBox_GetCount(hwndList); i++ )
                    {
                    ListBox_GetText(hwndList, i, lpsz);
                    lpsz += lstrlen(lpsz) + 1;
                    }  
                *lpsz++;
            }
            return ( TRUE );

        default:
            return ( FALSE );
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL OptionsValid(HWND hwndDlg)
 *
 *  validate all the dialog settings on OK or Apply
 *
 *  parameters:
 *      hwndDlg - dialog window handle
 *
 *  returns:
 *      TRUE if the data is OK
 ******************************************************************************/
static BOOL OptionsValid(HWND hwndDlg)
{
    char sz[_MAX_PATH + 1];
    int wColors;
    BOOL fValid;
    OFSTRUCT of;
    HFILE h = HFILE_ERROR;
    BOOL bIsDefaultsDialog = !IsWindowEnabled(GetDlgItem(hwndDlg, IDC_TMKPAL_LIST_IMAGEFILES));

    wColors = (WORD)GetDlgItemInt(hwndDlg, IDC_TMKPAL_EDIT_COLORS, &fValid, FALSE);
    if ( !fValid || wColors < 2 || wColors > 256 )
        {
        ErrorMessage(STYLE_ERROR, IDS_INVALIDNUM, 2, 256);
        /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_TMKPAL_EDIT_COLORS));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_TMKPAL_EDIT_COLORS), 0, -1);
        return ( FALSE );
        }

    GetDlgItemText(hwndDlg, IDC_TMKPAL_EDIT_FILENAME, sz, sizeof(sz));
    if ( sz[0] != 0 )
        {
        h = OpenFile(sz, &of, OF_READWRITE | OF_EXIST);
        if ( h != HFILE_ERROR )
            _lclose(h);
        else if ( of.nErrCode == 2 )    /* file not found */
            {
            h = OpenFile(sz, &of, OF_READWRITE | OF_CREATE);
            if ( h != HFILE_ERROR )
                {
                _lclose(h);
                remove(sz);
                }
            }
        }
    if ( ( !bIsDefaultsDialog && sz[0] == 0 ) || h == HFILE_ERROR )
        {
        if ( sz[0] == 0 )
            ErrorMessage(STYLE_ERROR, IDS_NOFILENAME);
        else
            ErrorMessage(STYLE_ERROR, IDS_INVALIDFILENAME, (LPCSTR)sz);
                    
        /* "The number is invalid or out of range.  Please enter a number from %d to %d." */
        SetFocus(GetDlgItem(hwndDlg, IDC_TMKPAL_EDIT_FILENAME));
        Edit_SetSel(GetDlgItem(hwndDlg, IDC_TMKPAL_EDIT_FILENAME), 0, -1);
        return ( FALSE );
        }
    
    /* make sure the list has at least one file */
    if ( !bIsDefaultsDialog &&
         ListBox_GetCount(GetDlgItem(hwndDlg, IDC_TMKPAL_LIST_IMAGEFILES)) == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_NOIMAGEFILES);
        SetFocus(GetDlgItem(hwndDlg, IDC_TMKPAL_BUTTON_ADD));
        return ( FALSE );
        }
    return ( TRUE );        
}



/******************************************************************************
 *  BOOL AddImageToListBox(pszFilename, DWORD dwParam)
 *
 *  verify and add image file name to list box
 * 
 *  parameters:
 *      pszFilename - file containing image
 *      dwParam     - listbox hwnd
 *
 *  returns:
 *      TRUE if MiscImageFileOpenDialog should continue to next file
 *      FALSE to abort MiscImageFileOpenDialog file processing
 *
 ******************************************************************************/
static BOOL AddImageToListBox(LPCSTR pszFilename, DWORD dwParam)
{
    HWND hwndList;
    BYTE PICHUGE *pDib;
    DWORD dwSize;
    PIC_PARM pp;
    OPENOP *pOpenOp;
    
#ifdef WIN32
    hwndList = (HWND)dwParam;
#else
    hwndList = (HWND)(WORD)dwParam;
#endif

    if ( ListBox_FindString(hwndList, -1, pszFilename) != LB_ERR )
        return ( TRUE );
    if ( OpenFileQuery(pszFilename, &pDib, &dwSize, &pOpenOp, &pp) )
        {
        MiscGlobalFreePtr(&pDib);
        ListBox_AddString(hwndList, pszFilename);    
        }
    return ( TRUE );
}



/******************************************************************************
 *  BOOL ImageFileColorHistogram(pszFilename, pHistogram)
 * 
 *  add color data from image pszFilename to color frequencies (32K word) in dwParam
 *
 *  parameters:
 *      pszFilename - file containing image for histogram
 *      pHistogram - pointer to histogram buffer
 *
 *  returns:
 *      FALSE if error
 *
 ******************************************************************************/
static BOOL ImageFileColorHistogram(LPCSTR pszFilename, WORD PICFAR* pwHistogram, DWORD *pdwTime)
{
    BYTE PICHUGE *pDib;
    DWORD dwSize;
    PIC_PARM pp;
    BOOL bRet;
    
    pdwTime[EXTRACTDIB] -= MiscTickCount();
    if ( !OpenFileExtractDib(pszFilename, &pDib, &dwSize, &pp) )
        return ( FALSE );
    pdwTime[EXTRACTDIB] += MiscTickCount();

    pdwTime[HISTOGRAM] -= MiscTickCount();
    bRet = ImageColorHistogram(pDib, dwSize, &pp, pwHistogram);
    MiscGlobalFreePtr(&pDib);
    if ( !bRet )
        ErrorPegasusMessage(pp.Status, IDS_IMAGEHISTOGRAM, pszFilename);
    pdwTime[HISTOGRAM] += MiscTickCount();
    return ( bRet );
}



/******************************************************************************
 *  BOOL ImageColorHistogram(pDib, dwSize, pp, pwHistogram)
 * 
 *  add color data from image to color frequencies (32K word) in dwParam
 *
 *  parameters:
 *      pDib - DIBits for image
 *      dwSize - size of image
 *      pp - picparm for image
 *
 *  returns:
 *      FALSE if error
 *
 ******************************************************************************/
static BOOL ImageColorHistogram(
    BYTE PICHUGE* pDib,
    DWORD dwSize,
    PIC_PARM PICFAR* pp,
    WORD PICFAR* pwHistogram)
{
    RESPONSE response;

    pp->Op = OP_UTL;
    _fmemset(&pp->u.UTL, 0, sizeof(pp->u.UTL));
    pp->Get.Start = pDib;
    /* NOTE:
        Get.End has to be 1 byte larger than the buffer really is.  This is because
        opcode 82 implements very strict queueing and "corrects" the queue as needed.
        In this case, if End isn't one byte bigger, so Rear is set to End, 82 SETS REAR
        TO START (wrapping Rear around).  In strict queueing theory this is OK in theory
        for a synchronous queue, but questionable in practice because the opcode should
        not be changing Rear for the Get queue.  In any case, the result when 82
        corrects Rear is that it thinks the queue is empty and returns RES_GET_NEED_DATA
        -- even though we've set AllocType to 1 and even though we've set Q_EOF.
        
        Setting End 1 byte larger and Rear 1 byte less than End is a silly hack that
        happens to work only because 82 doesn't actually read the extra byte which isn't
        really there */
    pp->Get.End   = pDib + dwSize + 1;
    pp->Get.Front = pp->Get.Start;
    pp->Get.Rear  = pp->Get.End - 1;
    pp->Get.QFlags = Q_EOF;

    pp->u.UTL.Subcode = 1;
    pp->u.UTL.ptr1 = (BYTE PICHUGE*)pwHistogram;
    pp->u.UTL.AllocType = 1;
    pp->u.UTL.HistogramSize = 32768UL;

    response = Pegasus(pp, REQ_INIT);
    if ( response == RES_DONE )
        response = Pegasus(pp, REQ_EXEC);
    if ( response == RES_DONE )
        {
        response = Pegasus(pp, REQ_TERM);
        assert(response == RES_DONE);
        }        
    else
        {
        assert(response == RES_ERR);
        if ( response != RES_ERR )
            Pegasus(pp, REQ_TERM);
        }        
        
    return ( response == RES_DONE );
}
