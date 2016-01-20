/******************************************************************************
 *
 * MMISC.C - Minerva miscellaneous utility routines
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
 *     MiscLtoaWithCommas      - ltoa() radix 10, but with commas every 3 digits as needed
 *     MiscUltoaWithCommas     - ultoa() radix 10, but with commas every 3 digits as needed
 *     MiscPaletteColors       - evaluate color palette in file pszFilename
 *     MiscPaletteBrowse       - common file open dialog to browse for a palette file
 *     MiscGetPassword         - get password dialog
 *     MiscShortFilename       - shorten file name by inserting ...'s
 *     MiscVisualOrient        - return string corresponding to visual orientation
 *     MiscIsPaletteDisplay    - return whether or not the display is palettized
 *     MiscIsImageType         - return whether or not an image type can be handled by an opcode
 *     MiscGlobalAllocPtr      - allocate memory
 *     MiscGlobalFreePtr       - GlobalFreePtr if not null, then set pointer to null
 *     MiscAllocateComment     - (re)Allocate space for comment or application data
 *     MiscIsGrayPalette       - return TRUE if colortable only has grays
 *     MiscQuantization        - #### TODO:
 *     MiscQuantizationBrowse  - #### TODO:
 *     MiscCopyToQueue         - copy as much data as possible to the queue
 *     MiscCopyFromQueue       - copy as much data as possible from the queue
 *     MiscYield               - yield to windows
 *     MiscDisplayTimingDialog - display timings dialog
 *     MiscImageFileOpenDialog - common file open dialog for images
 *     MiscQLen                - data available in queue
 * 
 * Local functions:
 *     PasswordDlgProc     - get password dialog procedure
 *     QSpace              - space available in queue
 *     QCopyLenTo          - copy some number of bytes to queue
 *     QCopyLenFrom        - copy some number of bytes from the queue
 *
 * Revision History:
 *   12-19-96  1.00.03  jrb  once-over and additional commenting
 *   11-10-96  1.00.00  jrb  created
 * 
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <commdlg.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#pragma warning(disable:4001)
/* pic includes */
#include "pic.h"
#include "pic2file.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "mmisc.h"
#include "merror.h"
#include "mdlg.h"



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL CALLBACK PasswordDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static DWORD QSpace(QUEUE PICFAR* pQ);
static void QCopyLenTo(QUEUE PICFAR* pQ, BYTE PICHUGE* pSrc, DWORD dwCopyLen);
static void QCopyLenFrom(BYTE PICHUGE* pSrc, QUEUE PICFAR* pQ, DWORD dwCopyLen);
static P2PktGeneric PICHUGE *PIC2NextTypePacket(P2PktGeneric PICHUGE *pList, BYTE Type);
static P2PktGeneric PICHUGE *PIC2FirstTypePacket(P2PktGeneric PICHUGE *pList, BYTE Type);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  LPSTR MiscLtoaWithCommas(long lVal, LPSTR psz, UINT nSize)
 * 
 *  ltoa() radix 10, but with commas every 3 digits as needed
 *
 *  parameters:
 *      lVal  - value to write to string
 *      psz   - string buffer
 *      nSize - size of psz
 *
 *  returns:  
 *      psz
 ******************************************************************************/
LPSTR MiscLtoaWithCommas(long lVal, LPSTR psz, UINT nSize)
{
    if ( lVal < 0 )
        {
        *psz = '-';
        MiscUltoaWithCommas(-lVal, psz + 1, nSize);
        }
    else
        MiscUltoaWithCommas(lVal, psz, nSize);        
    return ( psz );
}



/******************************************************************************
 *  LPSTR MiscUltoaWithCommas(long lVal, LPSTR psz, UINT nSize)
 * 
 *  ultoa() radix 10, but with commas every 3 digits as needed
 *
 *  parameters:
 *      lVal  - value to write to string
 *      psz   - string buffer
 *      nSize - size of psz
 *
 *  returns:  
 *      psz
 ******************************************************************************/
LPSTR MiscUltoaWithCommas(DWORD dw, LPSTR psz, UINT nSize)
{
    static char sz[sizeof("4,000,000,000")];

    if ( dw < 1000 )
        wsprintf(sz, "%ld", dw);
    else if ( dw < 1000000L )
        wsprintf(sz, "%ld,%.3ld", dw / 1000, dw % 1000);
    else if ( dw < 1000000000L )
        wsprintf(sz, "%ld,%.3ld,%.3ld", dw / 1000000L, ( dw % 1000000L ) / 1000, dw % 1000);
    else
        wsprintf(
            sz,
            "%ld,%.3ld,%.3ld,%.3ld",
            dw / 1000000000L,
            ( dw % 1000000000L ) / 1000000L,
            ( dw % 1000000L ) / 1000,
            dw % 1000);

    assert(strlen(sz) < nSize);
    lstrcpyn(psz, sz, nSize);
    return ( psz );
}



/******************************************************************************
 *  int MiscPaletteColors(const char *pszFilename, RGBQUAD PICFAR* pColors, int nColors)
 * 
 *  evaluate color palette in file pszFilename
 *
 *  parameters:
 *      pszFilename - file with palette
 *      pColors     - returned palette from file or NULL and none returned
 *      nColors     - max number of colors desired in palette
 *
 *  returns:  
 *      number of colors in palette, 0 if file doesn't contain a palette
 *
 *  notes:
 *      The nColors parameter allows us to choose between primary and
 *      secondary palettes in validating the file.
 ******************************************************************************/
int MiscPaletteColors(const char *pszFilename, RGBQUAD PICFAR* pColors, int nColors)
{
    HFILE hFile;
    OFSTRUCT of;
    long lColors;
    long lSecondaryColors;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    DWORD dwSize;
    
    hFile = OpenFile(pszFilename, &of, OF_READ | OF_SHARE_DENY_WRITE);
    if ( hFile == HFILE_ERROR )
        {
        if ( of.nErrCode == 2 || of.nErrCode == 3 )
            ErrorMessage(STYLE_ERROR, IDS_PALETTEFILENOTFOUND, (LPCSTR)pszFilename);
        else if ( of.nErrCode == 5 || of.nErrCode == 32 )
            ErrorMessage(STYLE_ERROR, IDS_PALETTEFILEACCESS, (LPCSTR)pszFilename);
        /* "An unexpected file access error occurred evaluating the palette file %s. "
            "Another program may be using this file." */
        else            
            ErrorMessage(STYLE_ERROR, IDS_PALETTEFILEOPENERROR, (LPCSTR)pszFilename);
        return ( 0 );
        }
        
    if ( _lread(hFile, &bmfh, sizeof(bmfh)) == (UINT)HFILE_ERROR ||
         _lread(hFile, &bmih, sizeof(bmih)) == (UINT)HFILE_ERROR )
        {
        _lclose(hFile);
        ErrorMessage(STYLE_ERROR, IDS_PALETTEFILEACCESS, (LPCSTR)pszFilename);
        /* "An unexpected file access error occurred evaluating the palette file %s. "
            "Another program may be using this file." */
        return ( 0 );
        }

    /* BM signature */
    if ( bmfh.bfType != 0x4d42 )
        {
        _lclose(hFile);
        ErrorMessage(STYLE_ERROR, IDS_INVALIDPALETTEFILE, (LPCSTR)pszFilename);
        /* "The file %s is an invalid palette file or it contains no colors." */
        return ( 0 );
        }
    if ( bmih.biClrUsed == 0 )
        {
        if ( bmih.biBitCount <= 8 )
            bmih.biClrUsed = ( 1 << bmih.biBitCount );
        else
            {
            _lclose(hFile);
            ErrorMessage(STYLE_ERROR, IDS_INVALIDPALETTEFILE, (LPCSTR)pszFilename);
            /* "The file %s is an invalid palette file or it contains no colors." */
            return ( 0 );
            }
        }            
    if ( bmih.biClrImportant == 0 )
        bmih.biClrImportant = bmih.biClrUsed;

    lColors = bmih.biClrImportant;
    lSecondaryColors = bmih.biClrUsed - bmih.biClrImportant;
    if ( lColors < 2 || lColors > nColors ) 
        {
        if ( lSecondaryColors < 2 || lSecondaryColors > nColors )
            {
            _lclose(hFile);
            ErrorMessage(STYLE_ERROR, IDS_INVALIDPALETTEFILE, (LPCSTR)pszFilename);
            /* "The file %s is an invalid palette file or it contains no colors." */
            return ( 0 );
            }
        if ( _llseek(hFile, lColors * sizeof(RGBQUAD), SEEK_CUR) == -1L )
            {
            _lclose(hFile);
            ErrorMessage(STYLE_ERROR, IDS_PALETTEFILEACCESS, (LPCSTR)pszFilename);
            /* "An unexpected file access error occurred evaluating the palette file %s. "
                "Another program may be using this file." */
            return ( 0 );
            }
        lColors = lSecondaryColors;
        }            

    /* read the color table only if requested */
    if ( pColors != 0 )
        {
        if ( _lread(hFile, pColors, (UINT)( sizeof(RGBQUAD) * lColors )) == (UINT)HFILE_ERROR )
            {
            _lclose(hFile);
            ErrorMessage(STYLE_ERROR, IDS_PALETTEFILEACCESS, (LPCSTR)pszFilename);
            /* "An unexpected file access error occurred evaluating the palette file %s. "
                "Another program may be using this file." */
            return ( 0 );
            }
        }
    if ( ( dwSize = _llseek(hFile, 0, SEEK_END) ) == (DWORD)-1L || _lclose(hFile) == HFILE_ERROR )
        {
        _lclose(hFile);
        ErrorMessage(STYLE_ERROR, IDS_PALETTEFILEACCESS, (LPCSTR)pszFilename);
        /* "An unexpected file access error occurred evaluating the palette file %s. "
            "Another program may be using this file." */
        return ( 0 );
        }


    if ( lColors < 2 || dwSize < sizeof(bmfh) + sizeof(bmih) + sizeof(RGBQUAD) * bmih.biClrUsed )
        {
        ErrorMessage(STYLE_ERROR, IDS_INVALIDPALETTEFILE, (LPCSTR)pszFilename);
        /* "The file %s is an invalid palette file or it contains no colors." */
        return ( 0 );
        }

    return ( (int)lColors );
}



/******************************************************************************
 *  MiscPaletteBrowse(HWND hwndDlg, int nEditID, int nColors)
 * 
 *  file open dialog to browse for a file containing a color palette
 *
 *  parameters:
 *      hwndDlg - window handle of dialog browsing
 *      nEditID - control id of edit field for file name
 *      nColors - max colors desired
 *
 *  returns:  
 *      FALSE if the user cancels out of the common file dialog
 *      else TRUE and nEditID control is set to the file name
 *
 *  notes:
 *      The nColors parameter allows us to choose between primary and
 *      secondary palettes in validating the file.
 ******************************************************************************/
BOOL MiscPaletteBrowse(HWND hwndDlg, int nEditID, int nColors)
{
    OPENFILENAME ofn;
    static char szFilename[_MAX_PATH + 1];
    static char szFilters[256];
    int i;
    char *psz;

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
    ofn.Flags       = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    do
        {
        if ( !GetOpenFileName(&ofn) )
            return ( FALSE );
        }
    while ( MiscPaletteColors(szFilename, 0, nColors) == 0 );

    SetDlgItemText(hwndDlg, nEditID, szFilename);
    return ( TRUE );
}



/******************************************************************************
 *  BOOL MiscGetPassword(LPSTR pszPassword)
 * 
 *  popup Get Password dialog box
 *
 *  parameters:
 *      hwndDlg     - parent window
 *      pszPassword - buffer for entered password (8 characters)
 *
 *  returns:  
 *      TRUE if user presses OK, else FALSE
 ******************************************************************************/
BOOL MiscGetPassword(HWND hwndDlg, LPSTR pszPassword)
{
    int result = DialogBoxParam(
        hinstThis,
        MAKEINTRESOURCE(IDD_GETPASSWORD),
        hwndDlg,
        (DLGPROC)PasswordDlgProc,
        (LPARAM)pszPassword);
    if ( result == -1 )
        {
        ErrorMessage(STYLE_ERROR, IDS_PASSWORDOUTOFMEMORY);
        /* "This image requires a password, but there was not enough memory to "
            "open a dialog box to enter a password." */
        return ( FALSE );
        }
    return ( result == IDOK );            
}



/******************************************************************************
 *  void MiscShortFilename(LPCSTR pszFilename, LPSTR pszShortname, UINT nShortLen
 * 
 *  shorten a file name by inserting ... in the middle
 *
 *  parameters:
 *      pszFilename  - original file name
 *      pszShortname - shortened name
 *      nShortLen    - maximum _string_ length for shortened name
 *
 *  notes:
 *      if possible, returns a name of the form C:\...\path\filename.ext where the
 *      entire filename is included and as much of the immediately prior path
 *      as fits within the length
 *
 *      if not possible, then the filename itself exceeds the length and the output
 *      is <first part of filename>...<last part of filename> ending up exactly the
 *      specified length (although this code branch hasn't been tested yet).
 ******************************************************************************/
void MiscShortFilename(LPCSTR pszFilename, LPSTR pszShortname, UINT nShortLen)
{
    char sz[_MAX_PATH + 1];
    char *pszSlash;
    char *psz;
    UINT len;

    lstrcpy(sz, pszFilename);
    len = strlen(sz);
    if ( len > nShortLen )
        {
        psz = sz + sizeof("c:\\") - 1;  /* past c:\" */
        *psz++ = '.';
        *psz++ = '.';
        *psz++ = '.';
        pszSlash = strchr(psz + 1, '\\');
            /* + 1 because we already know any slash immediately following the "..." will
                yield a name which is too long */
        while ( pszSlash != NULL && strlen(pszSlash) + sizeof("c:\\...") - 1 > nShortLen )
            pszSlash = strchr(pszSlash + 1, '\\');
        if ( pszSlash != NULL )
            strcpy(psz, pszSlash);
        else    
            {
            /* else the file name itself is too long */
            lstrcpyn(
                sz,
                pszFilename, sizeof("c:") - 1 + ( nShortLen - sizeof("c:...") - 1 ) / 2 + 1);
                    /* + 1 because lstrcypn writes a null terminator in the last
                        position indicated by the size parameter */
            psz = sz + strlen(sz);
            *psz++ = '.';
            *psz++ = '.';
            *psz++ = '.';
            lstrcpy(psz, pszFilename + len - ( nShortLen + 1 - sizeof("c:...") ) / 2);
                                                        /* + 1 to round up */
            }
        }
    lstrcpy(pszShortname, sz);
    assert(strlen(sz) <= nShortLen);
}        



/******************************************************************************
 *  LPSTR MiscVisualOrient(ORIENTATION Orient)
 * 
 *  return string corresponding to orientation
 *
 *  parameters:
 *      Orient - PIC_PARM visual ORIENTATION value
 *
 *  returns:
 *      LPCSTR pointer to descriptive string
 *
 *  notes:
 *      Not too tough, but it's used by properties for most image types
 *      so it might as well be one place
 ******************************************************************************/
LPCSTR MiscVisualOrient(ORIENTATION Orient)
{
    LPCSTR psz;

#define O_normal    0
#define O_inverted  1
#define O_r90       2
#define O_r90_in    3
#define O_r180      4
#define O_r180_in   5
#define O_r270      6
#define O_r270_in   7
#define O_w_on_b    0x10
#define O_bit_rev   0x20
    switch ( Orient )
        {
        case O_normal:
            psz = "normal";
            break;
        case O_normal | O_inverted:
            psz = "inverted";
            break;
        case O_r90:
            psz = "90° clockwise";
            break;
        case O_r90 | O_inverted:
            psz = "90° clockwise, then inverted";
            break;
        case O_r180:
            psz = "180°";
            break;
        case O_r180 | O_inverted:
            psz = "180°, then inverted";
            break;
        case O_r270:
            psz = "270° clockwise";
            break;
        case O_r270 | O_inverted:
            psz = "270° clockwise, then inverted";
            break;
        default:
            psz = "n/a";
            break;
        }
    return ( psz );
}



/******************************************************************************
 *  BOOL MiscIsPaletteDisplay(int *pNumColors)
 * 
 *  return whether or not display is palettized
 *
 *  parameters:
 *      pNumColors - receives display number of colors if palettized (NULL if not desired)
 *
 *  returns:
 *      TRUE if display is palettized
 ******************************************************************************/
BOOL MiscIsPaletteDisplay(int *pNumColors)
{
    HWND hwnd;
    HDC hdc;
    int num = 0;

    hwnd = GetDesktopWindow();
    hdc = GetDC(hwnd);
    if ( ( GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE ) != 0 )
        num = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
    ReleaseDC(hwnd, hdc);
    if ( pNumColors != 0 )
        *pNumColors = num;
    return ( num != 0 );
}



/******************************************************************************
 *  BOOL MiscIsImageType(DWORD biCompression, OPENOP *pOpen)
 * 
 *  return whether or not biCompression image type can be handled by pOpen opcode
 *
 *  parameters:
 *      biCompression - image type
 *      pOpen         - opcode open data definition
 *
 *  returns:
 *      TRUE if display is palettized
 ******************************************************************************/
BOOL MiscIsImageType(DWORD biCompression, OPENOP *pOpen)
{
    int i;
    
    if ( pOpen->pbiCompression == 0 )
        return ( FALSE );
    for ( i = 0; pOpen->pbiCompression[i] != (DWORD)-1; i++ )
        if ( pOpen->pbiCompression[i] == biCompression )
            return ( TRUE );
    return ( FALSE );
}



/******************************************************************************
 *  VOID PICHUGE *MiscGlobalAllocPtr(DWORD dwSize, int nIDSError)
 * 
 *  allocate memory
 *
 *  parameters:
 *      dwSize    - number of bytes to allocate
 *      nIDSError - string resource of error message if unsucessful, 0 to
 *                  display no message
 ******************************************************************************/
VOID PICHUGE* MiscGlobalAllocPtr(DWORD dwSize, int nIDSError)
{
    VOID PICHUGE* pv = GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, dwSize);
    
    if ( pv == 0 && nIDSError != 0 )
        ErrorMessage(STYLE_ERROR, nIDSError);
    return ( pv );
}



/******************************************************************************
 *  MiscGlobalFreePtr(LPVOID pv)
 * 
 *  free pointer *pv if not null, then set to null
 *
 *  parameters:
 *      pv - pointer to pointer to free
 *
 *  notes:
 *      I couldn't figure out a way to declare the arg as a pointer to a pointer
 *      without tons of casts or tons of warnings.  With a cast, I also didn't
 *      get the warning I wanted -- that the level of indirection was wrong.
 ******************************************************************************/
void MiscGlobalFreePtr(LPVOID pv)
{
    LPVOID PICFAR* ppv = (LPVOID PICFAR*)pv;

    if ( IsBadWritePtr(ppv, sizeof(LPVOID)) )
        {
        assert(FALSE);
        return;
        }

    if ( *ppv == 0 )
        return;

    if ( IsBadReadPtr(*ppv, sizeof(BYTE)) )
        {
        assert(FALSE);
        return;
        }
    if ( GlobalFreePtr(*ppv) != 0 )
        assert(FALSE);
    *ppv = 0;
}



/******************************************************************************
 *  BOOL MiscAllocateComment(
 *      LONG dwLen,
 *      CHAR PICHUGE* PICFAR* ppComment,
 *      LONG PICFAR* pdwSize,
 *      int nIDSMultiple);
 * 
 *  allocate space for comment or application data
 *
 *  parameters:
 *      dwLen        - space required
 *      ppComment    - receive pointer to allocated space
 *      pdwSize      - current space allocated
 *      nIDSMultiple - string resource id if a comment is already allocated
 *
 *  returns:
 *      TRUE unless space can't be allocated
 *
 *  notes:
 *      allocates one more byte than requested and stuffs a null terminator there
 ******************************************************************************/
BOOL MiscAllocateComment(
    LONG dwLen,
    CHAR PICHUGE* PICFAR* ppComment,
    LONG PICFAR* pdwSize)
{
    void PICHUGE* pv;
    CHAR PICHUGE* PICHUGE* ppc;
    CHAR PICHUGE *pc;
    
    if ( *ppComment == 0 )
        {
        ppc = MiscGlobalAllocPtr(sizeof(LPSTR) + dwLen + 1, IDS_IMAGEOUTOFMEMORY);
        if ( ppc == 0 )
            return ( FALSE );
        *ppc = (CHAR PICHUGE*)ppc;
        ppc++;
        *ppComment = (CHAR PICHUGE*)ppc;
        *pdwSize = dwLen + 1;
        (*ppComment)[dwLen] = '\0';
        return ( TRUE );
        }

    ppc = (CHAR PICHUGE* PICHUGE*)*ppComment;
    ppc--;
    pc = (CHAR PICHUGE*)ppc;
    ppc = (CHAR PICHUGE* PICHUGE*)*ppc;
    *pc++ = '\r';
    *pc++ = '\n';
    *pc++ = '-';
    *pc++ = '>';
    pv = GlobalReAllocPtr(ppc, lstrlen((LPSTR)ppc) + sizeof(LPSTR) + dwLen + 1, GMEM_MOVEABLE);
                                                        // ^ for crlflf
    if ( pv == 0 )
        {
        ErrorMessage(STYLE_ERROR, IDS_IMAGEOUTOFMEMORY);
        /* "There was not enough memory to open the image." */
        return ( FALSE );
        }
    pc = (LPSTR)pv;
    pc += lstrlen((LPSTR)pc);
    ppc = (CHAR PICHUGE* PICFAR*)pc;
    *ppc++ = pv;
    *ppComment = (CHAR PICHUGE*)ppc;
    *pdwSize = dwLen + 1;
    (*ppComment)[dwLen] = '\0';
    return ( TRUE );
}



/******************************************************************************
 *  BOOL MiscIsGrayPalette(DWORD dwColors, const RGBQUAD PICFAR* pColorTable)
 * 
 *  return TRUE if colortable only has grays
 *
 *  parameters:
 *      dwColors    - number of colors to check
 *      pColorTable - pointer to color table
 *
 *  returns:
 *      TRUE if all colors are shades of gray 
 ******************************************************************************/
BOOL MiscIsGrayPalette(DWORD dwColors, const RGBQUAD PICFAR* pColorTable)
{
    UINT i;

    if ( dwColors == 0 )
        return ( FALSE );
    for ( i = 0; i < dwColors; i++ )
        {
        if ( pColorTable[i].rgbRed != pColorTable[i].rgbGreen ||
             pColorTable[i].rgbRed != pColorTable[i].rgbBlue )
            return ( FALSE );
        }
    return ( TRUE );
}



BOOL  MiscQuantization(LPCSTR pszFilename, BYTE PICHUGE* Quantization)
{
    /*#### TODO: */
    NOREFERENCE(pszFilename);
    NOREFERENCE(Quantization);
    return ( TRUE );
}



BOOL  MiscQuantizationBrowse(HWND hwndDlg, int nID)
{
    /*#### TODO: */
    NOREFERENCE(hwndDlg);
    NOREFERENCE(nID);
    return ( TRUE );
}



/******************************************************************************
 *  void MiscCopyLenToQueue(
 *      BYTE PICHUGE* PICFAR* ppSrc,
 *      DWORD PICFAR* pdwSrc,
 *      QUEUE PICFAR* pQ,
 *      DWORD dwStripLen)
 * 
 *  copy as much data as possible to the queue
 *
 *  parameters:
 *      ppSrc      - pointer to pointer to data to put in queue
 *                   after, points one byte past new data (normal queue)
 *                   or to lowest address byte of new data (reverse queue)
 *      pdwSrc     - pointer to length of data
 *                   decremented by length copied
 *      pQ         - pointer to queue structure
 *      dwStripLen - if reversed queue, length of strips to copy
 *
 *  notes:
 *      If queue is reversed, then we're copying strips one at a time in
 *      reverse order to the queue.  If the queue were empty, and big
 *      enough for 4 strips exactly, and the strips in *ppSrc are
 *      in order A B C D, then the strips will end up in the queue
 *      in order D C B A. The queue Rear pointer will point to the
 *      first byte of D and will equal the queue Start pointer.  The
 *      queue Front pointer will point to the byte following the last
 *      byte of A and will equal the queue End pointer.  The bytes
 *      within each strip are copied to the queue in the same order
 *      as they have in the buffer.  Thus the queue ends up reversed
 *      top for bottom, but not reversed left for right if you view
 *      the input buffer as a two-dimensional array of strips of pixels
 *      -- which is what it is, it's a DIB.
 *
 *      If the queue is not reversed, then we're still, in some cases
 *      copying strips, one at a time, but since they're ordered normally
 *      we don't have to care.
 *
 *      We take advantage of the fact that a special case has the
 *      Front pointer at the Start (normal) or End (reversed) of the
 *      buffer and that we can copy to all bytes of the buffer then,
 ******************************************************************************/
void MiscCopyToQueue(
    BYTE PICHUGE* PICFAR* ppSrc,
    DWORD PICFAR* pdwSrc,
    QUEUE PICFAR* pQ,
    DWORD dwStripLen)
{
    DWORD dwCopyLen;

    assert(( pQ->QFlags & Q_EOF ) == 0);
    assert(*ppSrc != 0 && *pdwSrc != 0);

    if ( ( pQ->QFlags & Q_REVERSE ) != 0 )
        {
        if ( pQ->Front == pQ->Rear )
            {
            /* note in particular that save sequential jpeg requires that there
                be at least one strip of contiguous data from Front, so we must
                reset the front (and rear) whenever the queue is empty */
            pQ->Front = pQ->End;
            pQ->Rear  = pQ->Front;
            }
        assert(dwStripLen != 0);
        dwCopyLen  = QSpace(pQ) / dwStripLen;
        dwCopyLen *= dwStripLen;
        if ( dwCopyLen > *pdwSrc )
            dwCopyLen = *pdwSrc;
        assert(dwCopyLen != 0);

        while ( dwCopyLen >= dwStripLen )
            {
            *ppSrc    -= dwStripLen;
            *pdwSrc   -= dwStripLen;
            dwCopyLen -= dwStripLen;
            QCopyLenTo(pQ, *ppSrc, dwStripLen);
            }
        if ( dwCopyLen != 0 )
            {
            *ppSrc  -= dwCopyLen;
            *pdwSrc -= dwCopyLen;
            assert(*pdwSrc == 0);   /* else dwCopyLen should have been a multiple of dwStripLen */
            QCopyLenTo(pQ, *ppSrc, dwCopyLen);
            }
        }
    else    /* not Q_REVERSE */
        {
        if ( pQ->Front == pQ->Rear )
            {
            pQ->Front = pQ->Start;
            pQ->Rear  = pQ->Front;
            }
        /* fill the queue up to at most *pdwSrc bytes */
        dwCopyLen = QSpace(pQ);
        if ( dwCopyLen > *pdwSrc )
            dwCopyLen = *pdwSrc;
        assert(dwCopyLen != 0);

        QCopyLenTo(pQ, *ppSrc, dwCopyLen);
        *ppSrc  += dwCopyLen;
        *pdwSrc -= dwCopyLen;        
        }

    if ( *pdwSrc == 0 )
        pQ->QFlags |= Q_EOF;
}



/******************************************************************************
 *  void MiscCopyLenFromQueue(
 *      BYTE PICHUGE* PICFAR* ppDest,
 *      DWORD PICFAR* pdwDest,
 *      QUEUE PICFAR* pQ,
 *      DWORD dwStripLen)
 * 
 *  copy as much data as possible from the queue
 *
 *  parameters:
 *      ppDest     - pointer to pointer to buffer for queue data,
 *                   after, points one byte past new data (normal queue)
 *                   or to lowest address byte of new data (reverse queue)
 *      pdwDest    - pointer to length available in ppDest
 *                   decremented by length copied
 *      pQ         - pointer to queue structure
 *      dwStripLen - if reversed queue, length of strips to copy
 *
 *  notes:
 *      If queue is reversed, then we're copying strips one at a time in
 *      reverse order to the buffer.  If the buffer were empty, and big
 *      enough for 4 strips exactly, and the strips in the queue are
 *      in order A B C D, then the strips will end up in the buffer
 *      in order D C B A. *ppDest will point to the first byte of D and
 *      *ppDest + *pdwDest will point past the last byte of A. The bytes
 *      within each strip are copied to the buffer in the same order
 *      as they have in the queue.  Thus the buffer ends up reversed
 *      top for bottom, but not reversed left for right if you view
 *      the queue as a two-dimensional array of strips of pixels
 *      -- which is what it is, it's a DIB.
 *
 *      If the queue is not reversed, then we're still, in some cases
 *      copying strips, one at a time, but since they're ordered normally
 *      we don't have to care.
 ******************************************************************************/
void MiscCopyFromQueue(
    BYTE PICHUGE* PICFAR* ppDest,
    DWORD PICFAR* pdwDest,
    QUEUE PICFAR* pQ,
    DWORD dwStripLen)
{
    DWORD dwCopyLen;

    assert(*ppDest != 0 && *pdwDest != 0);

    if ( ( pQ->QFlags & Q_REVERSE ) != 0 )
        {
        assert(dwStripLen != 0);
        dwCopyLen = MiscQLen(pQ);
        if ( dwCopyLen >= dwStripLen )
            {
            dwCopyLen  = MiscQLen(pQ) / dwStripLen;
            dwCopyLen *= dwStripLen;
            }
        if ( dwCopyLen > *pdwDest )
            dwCopyLen = *pdwDest;

        while ( dwCopyLen >= dwStripLen )
            {
            *ppDest   -= dwStripLen;
            *pdwDest  -= dwStripLen;
            dwCopyLen -= dwStripLen;
            QCopyLenFrom(*ppDest, pQ, dwStripLen);
            }
        if ( dwCopyLen != 0 )
            {
            *ppDest  -= dwCopyLen;
            *pdwDest -= dwCopyLen;
            QCopyLenFrom(*ppDest, pQ, dwCopyLen);
            }
        }
    else    /* not Q_REVERSE */
        {
        dwCopyLen = MiscQLen(pQ);
        if ( dwCopyLen > *pdwDest )
            dwCopyLen = *pdwDest;
        assert(dwCopyLen != 0);

        QCopyLenFrom(*ppDest, pQ, dwCopyLen);
        *ppDest  += dwCopyLen;
        *pdwDest -= dwCopyLen;        
        }
}



/******************************************************************************
 *  DWORD MiscQLen(QUEUE PICFAR* pQ)
 * 
 *  data available in queue
 *
 *  parameters:
 *      pQ         - pointer to queue structure
 ******************************************************************************/
DWORD MiscQLen(QUEUE PICFAR* pQ)
{
    if ( ( pQ->QFlags & Q_REVERSE) != 0 )
        {
        if ( pQ->Front == pQ->Rear )
            return ( 0 );
        if ( pQ->Rear < pQ->Front )
            return ( pQ->Front - pQ->Rear );
        /* else pQ->Rear > pQ->Front */
        return ( pQ->Front - pQ->Start + pQ->Rear - pQ->End );
        }
    /* else not Q_REVERSE */

    if ( pQ->Front == pQ->Rear )
        return ( 0 );
    if ( pQ->Rear > pQ->Front )
        return ( pQ->Rear - pQ->Front );
    /* else pQ->Rear < pQ->Front */
    return ( pQ->End - pQ->Front + pQ->Rear - pQ->Start );
}



/******************************************************************************
 *  BOOL MiscYield(void)
 * 
 *  yield to windows
 *
 *  returns:
 *      TRUE if WM_QUIT message encountered
 ******************************************************************************/
BOOL MiscYield(void)
{
    MSG msg;

    while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
        if ( msg.message == WM_QUIT )
            {
            PostQuitMessage(msg.wParam);
            return ( TRUE );
            }

        if ( !TranslateMDISysAccel(hwndMDIClient, &msg) &&
             !TranslateAccelerator(hwndFrame, hAcceleratorTable, &msg) )
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }
    return ( FALSE );
}




/******************************************************************************
 *  DWORD MiscTickCount()
 *
 *  intercepting GetTickCount calls from minerva
 ******************************************************************************/
DWORD MiscTickCount(void)
{
#if defined(__BORLANDC__) || !defined(__FLAT__)
    return ( GetTickCount() );
#else
    static int Calibrated = 0;
    static DWORD divlo;
    DWORD dw;

    if ( Calibrated == 0 )
    {
        SYSTEM_INFO si;

        GetSystemInfo(&si);
        if ( si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL ||
             si.dwProcessorType != PROCESSOR_INTEL_PENTIUM ||
             bDisableRDTSC )
        {
            Calibrated = -1;
        }
        else
        {
            DWORD i;
            DWORD dwStart;
            DWORD tsclo;
            DWORD tschi;
            DWORD dwLast;
            DWORD ticks;
            DWORD divhi;

            i = 0;
            dwStart = GetTickCount();
            while (  ( dw = GetTickCount() ) == dwStart )
                ;
            while (  ( dwStart = GetTickCount() ) == dw )
                ;
            _asm
            {
                _emit   0x0f
                _emit   0x31
                not     eax
                not     edx
                add     eax,1
                adc     edx,0
                mov     tsclo,eax
                mov     tschi,edx
            }
            do
            {
                dwLast = dw;
                while ( ( dw = GetTickCount() ) == dwLast )
                    ;
                i++;
            }
            while ( ( dw = GetTickCount() ) - dwStart < 1000 );
            _asm
            {
                _emit   0x0f
                _emit   0x31
                add     tsclo,eax
                adc     tschi,edx
            }
            ticks = dw - dwStart;
            _asm
            {
                mov     edx,0
                mov     eax,tschi
                div     ticks
                mov     divhi,eax
                mov     eax,tsclo
                div     ticks
                mov     divlo,eax
                mov     edx,tschi
                mov     eax,tsclo
                div     divlo
                mov     ecx,eax
                sub     ecx,ticks
                inc     divlo
                mov     edx,tschi
                mov     eax,tsclo
                div     divlo
                mov     edx,ticks
                sub     edx,eax
                cmp     edx,ecx
                jb      rounded
                dec     divlo
rounded:
            }
            assert(divhi == 0); // until we reach 4GHz * 1000 CPU's
            Calibrated = 1;
        }
    }

    if ( Calibrated < 0 )
        return ( GetTickCount() );
    _asm
    {
        _emit   0x0f
        _emit   0x31
        div     divlo
        mov     [dw],eax
    }
    return ( dw );
#endif
}



/******************************************************************************
 *  void MiscDisplayTimingDialog(
 *      HWND hwnd,
 *      LPCSTR pszDlg,
 *      DWORD *pdwTimes,
 *      LPCTRLINITDATA pTimingDef)
 *
 *  display timing dialog
 *
 *  parameters:
 *    pszDlg     - dialog template name/id
 *    pdwTimes   - array of millisecond times
 *    pTimingDef - control data for timing static text  
 ******************************************************************************/
void MiscDisplayTimingDialog(HWND hwnd, LPCSTR pszDlg, DWORD *pdwTimes, LPCTRLINITDATA pTimingDef)
{
    int result;
    LPCTRLINITDATA pData;
    DWORD dwTimerRes;
    DWORD dw;
    DWORD dwLast;
    DWORD dwStart;
    int i;
    int nScale;
    int nDiv;
    int nRnd;
        
    /**************************************************************************
        Create a writeable version of the property dialog control definitions
    */
    if ( !CtrlDataDup(pTimingDef, &pData) )
        {
        ErrorMessage(STYLE_ERROR, IDS_TIMINGOUTOFMEMORY);
        /* "There was not enough memory to open the timing dialog." */
        return;
        }

    /* attempt to measure the timer resolution */
    dwStart = MiscTickCount();
    while (  ( dw = MiscTickCount() ) == dwStart )
        ;
    dwStart = dw;
    i = 0;
    do
        {
        dwLast = dw;
        while ( ( dw = MiscTickCount() ) == dwLast )
            ;
        i++;
        }
    while ( ( dw = MiscTickCount() ) - dwStart < 500 );
    dwTimerRes = ( ( dw - dwStart ) * 10 + i / 2 ) / i;
    if ( dwTimerRes < 100 )
        {
        nScale = 3;
        nDiv = 1;
        nRnd = 0;
        }
    else
        {
        nScale = 2;
        nDiv = 10;
        nRnd = 5;
        }
    
    /**************************************************************************
        Initialize all the times
    */
    /* first control is timer resolution */
    CtrlDataSetDecFixed(pData, pData[0].nID, ( dwTimerRes + nRnd ) / nDiv, nScale + 1);
    
    for ( i = 1; pData[i].eType != eEnd; i++ )
        {
        if ( pData[i].eType == eText && pData[i].wLimitText != 0 )
            CtrlDataSetDecFixed(pData, pData[i].nID, ( pdwTimes[i - 1] + nRnd ) / nDiv, nScale);
        }

    /**************************************************************************
        Display the timing dialog
    */
    result = DlgDoModal(hwnd, pszDlg, 0, pData, NULL);
    CtrlDataFree(&pData);
    if ( result == -1 )
        {
        ErrorMessage(STYLE_ERROR, IDS_CREATETIMINGDIALOG);
        /* "An unexpected error occurred opening the timing dialog." */
        }             
}



/******************************************************************************
 *  BOOL MiscImageFileOpenDialog(BOOL (*pfn)(LPCSTR))
 *
 *  display multiple-select common file open dialog for images.  call pfn function
 *  for each file or until pfn function returns FALSE
 *
 *  parameters:
 *    pfn - function to call for each file
 *    dwParam - parameter to pass to function
 *
 *  returns:
 *    FALSE - user cancelled out of file open box or pfn returned false for some file
 *    else TRUE
 ******************************************************************************/
BOOL MiscImageFileOpenDialog(HWND hwnd, BOOL (*pfn)(LPCSTR, DWORD), DWORD dwParam)
{
    OPENFILENAME ofn;
    static char szFilename[(_MAX_PATH + 1) * 50];
    char szOpFilters[512];
    char szAllFilters[512];
    int i;
    char * psz;
    UINT len;
    char * pszNext;
    HCURSOR hOldCursor;
            
    /* file filter strings are constructed from present opcodes
        The first filter string will be "All images" and we will here collect
        all the file specs for present opcodes to include in "All images".
        That will be followed by a spec for each supported file type with
        a present opcode.  Those will be followed by "All files (*.*)" */
    i = LoadString(hinstThis, IDS_ALLIMAGES, szAllFilters, sizeof(szAllFilters));
    assert(i != 0 && i < sizeof(szAllFilters));
    len = 0;
    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {
        if ( pOpenOpTable[i]->nFoundParmVer!= 0 &&
             pOpenOpTable[i]->pszFilter != 0 &&
             *pOpenOpTable[i]->pszFilter != '\0' )
            {
            lstrcpy(&szOpFilters[len], pOpenOpTable[i]->pszFilter);
            assert(sizeof(szOpFilters) > strlen(szOpFilters));
            psz = strchr(&szOpFilters[len], '|');
            assert(psz != NULL);
            strcat(szAllFilters, psz + 1);
            szAllFilters[strlen(szAllFilters) - 1] = ';';
            len += strlen(&szOpFilters[len]);
            }
        }
    strcat(szAllFilters, "*.KQP;");
    szAllFilters[strlen(szAllFilters) - 1] = '|';
    strcat(szAllFilters, szOpFilters);
    i = LoadString(hinstThis, IDS_ALLFILES, szOpFilters, sizeof(szOpFilters));
    assert(i != 0 && i < sizeof(szOpFilters));
    strcat(szAllFilters, szOpFilters);
    assert(sizeof(szAllFilters) > strlen(szAllFilters));

    /* filter strings' separators are '|' so they can easily be entered and parsed.
        the file common dialog requires the separator to be '\0' (what a pain) */
    psz = szAllFilters;
    while ( ( psz = strchr(psz, '|') ) != NULL )
        *psz++ = '\0';

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = hwnd;
    ofn.lpstrFilter = szAllFilters;
    ofn.lpstrFile   = szFilename;
    ofn.nMaxFile    = sizeof(szFilename);
    szFilename[0] = '\0';
    ofn.Flags       = OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
#if defined(WIN32) && defined(WINVER) && WINVER >= 0x0400
    /* otherwise OFN_ALLOWMULTISELECT will turn off the explorer style dialog */
    ofn.Flags |= OFN_EXPLORER;
#endif
    if ( !GetOpenFileName(&ofn) )
        return ( FALSE );
    
    psz = &szFilename[ofn.nFileOffset];
    psz[-1] = '\\';
    hOldCursor = SetCursor(hWaitCursor);
    if ( psz[-2] == '\\' )
    {
        // apparently multiple files selected in the root
        memcpy(psz - 1, psz, (size_t)( szFilename + sizeof(szFilename) - psz ));
        psz--;
    }
#if defined(WIN32) && defined(WINVER) && WINVER >= 0x0400
    if ( ofn.Flags & OFN_EXPLORER )
        {
        pszNext = psz;
        /* OFN_EXPLORER has changed the way it returns multiple file names slightly */
        do
            {
            SetCursor(hWaitCursor); /* setting it once each loop forces it back to a wait
                                    cursor if the user has moved the mouse */
            if ( !(*pfn)(szFilename, dwParam) )
                {
                SetCursor(hOldCursor);
                return ( FALSE );
                }
            pszNext += strlen(pszNext) + 1;
            strcpy(psz, pszNext);
            /* if the Nth file name is shorter than the N+1th, then the strcpy is
                an overlapping copy a null has been copied at or past pszNext, screwing
                us up as we look for the start of the next file name without the
                following (unpleasant) correction */
            if ( pszNext <= psz + strlen(psz) )
                pszNext = psz + strlen(psz) + 1;
            }
        while ( *psz != 0 );
        }
    else
#endif
        {
        while ( ( pszNext = strchr(psz, ' ') ) != 0 )
            {
            *pszNext++ = '\0';
            SetCursor(hWaitCursor); /* setting it once each loop forces it back to a wait
                                        cursor if the user has moved the mouse */
            if ( !(*pfn)(szFilename, dwParam) )
                {
                SetCursor(hOldCursor);
                return ( FALSE );
                }
            strcpy(psz, pszNext);
            }
        if ( !(*pfn)(szFilename, dwParam) )
            {
            SetCursor(hOldCursor);
            return ( FALSE );
            }
        }
    SetCursor(hOldCursor);
    return ( TRUE );    
}



/******************************************************************************
 *  LPSTR MiscAllocatePIC2Comment(CHAR PICHUGE *pPIC2List, BYTE Type)
 * 
 *  allocate space for comment(s) or other ASCIIZ application data and copy the
 *  data into the space
 *
 *  parameters:
 *      PIC2List - pointer to PIC2List data
 *      Type     - PIC2 packet type to be returned
 *
 *  returns:
 *      pointer to allocated space or 0
 *
 *  notes:
 *      Type must have the same form as a P2PktGeneric packet where the
 *      PIC2 Packet type and length are followed by an ASCIIZ string
 ******************************************************************************/
CHAR PICHUGE *MiscAllocatePIC2Comment(CHAR PICHUGE *pPIC2List, BYTE Type)
{
    P2PktGeneric PICHUGE *pList;
    LPSTR psz;
    size_t strlenpsz;

    if ( pPIC2List == 0 ||
         ( pList = PIC2FirstTypePacket((P2PktGeneric PICHUGE*)pPIC2List, Type) ) == 0 )
        return ( 0 );
    psz = (LPSTR)MiscGlobalAllocPtr(pList->Length + sizeof("->"), IDS_PROPERTIESOUTOFMEMORY);
    if ( psz == 0 )
        return ( 0 );
    lstrcpy(psz, "->");
    strlenpsz = sizeof("->") - 1;
     hmemcpy(&psz[strlenpsz], (LPCSTR)pList->Data, pList->Length);
     psz[strlenpsz + (size_t)pList->Length] = '\0';
    while ( ( pList = PIC2NextTypePacket(pList, Type) ) != 0 )
    {
        LPSTR pszRe = GlobalReAllocPtr(
            psz,
            strlenpsz + sizeof("\r\n->") + pList->Length,
            GMEM_MOVEABLE);
        if ( pszRe == 0 )
        {
            MiscGlobalFreePtr(&psz);
            ErrorMessage(STYLE_ERROR, IDS_PROPERTIESOUTOFMEMORY);
            return ( 0 );
        }
        psz = pszRe;
        lstrcat(psz, "\r\n->");
        strlenpsz = lstrlen(psz);
        hmemcpy(&psz[strlenpsz], (LPCSTR)pList->Data, pList->Length);
        psz[strlenpsz + (size_t)pList->Length] = '\0';
    }
    return ( psz );
}



/******************************************************************************
 *  LPSTR MiscAllocatePIC2RawData(CHAR PICHUGE *pPIC2List, DWORD RawDesc)
 * 
 *  allocate space for raw data packet(s) and copy the data into the space
 *
 *  parameters:
 *      PIC2List - pointer to PIC2List data
 *      RawDesc - FOURCC version of description
 *
 *  returns:
 *      pointer to allocated space or 0
 ******************************************************************************/
CHAR PICHUGE *MiscAllocatePIC2RawData(CHAR PICHUGE *pPIC2List, CHAR *RawDesc)
{
    P2PktRawData PICHUGE *pList;
    LPSTR psz;
    int strlenpsz;
    DWORD dwLength;

    if ( pPIC2List == 0 ||
         ( pList = (P2PktRawData PICHUGE *)
             PIC2FirstTypePacket((P2PktGeneric PICHUGE*)pPIC2List, P2P_RawData) ) == 0 )
        return ( 0 );
    while ( *(DWORD PICHUGE*)pList->RawDescription == *(DWORD *)RawDesc )
    {
        pList = (P2PktRawData PICHUGE *)
            PIC2NextTypePacket((P2PktGeneric PICHUGE*)pList, P2P_RawData);
        if ( pList == 0 )
            return ( 0 );
    }

    /* note that Minerva allocates space for the full raw data packet, but if it
        were allocating less space, Length would be adjusted for the truncated
        packet, but RawLength would not be adjusted -- so it's adjusted here */
    dwLength = pList->RawLength;
    if ( dwLength > pList->Length - sizeof(pList->RawDescription) - sizeof(pList->RawLength) )
        dwLength = pList->Length - sizeof(pList->RawDescription) - sizeof(pList->RawLength);

    psz = (LPSTR)MiscGlobalAllocPtr(dwLength + sizeof("->"), IDS_PROPERTIESOUTOFMEMORY);
    if ( psz == 0 )
        return ( 0 );
    lstrcpy(psz, "->");
    strlenpsz = sizeof("->") - 1;
    hmemcpy(&psz[strlenpsz], (LPCSTR)pList->RawData, dwLength);
     psz[strlenpsz + (size_t)dwLength] = '\0';
    while ( ( pList = (P2PktRawData PICHUGE *)
                PIC2NextTypePacket((P2PktGeneric PICHUGE *)pList, P2P_RawData) ) != 0 )
    {
        if ( *(DWORD PICHUGE*)pList->RawDescription == *(DWORD *)RawDesc )
        {
            LPSTR pszRe;

            dwLength = pList->RawLength;
            if ( dwLength > pList->Length - sizeof(pList->RawDescription) - sizeof(pList->RawLength) )
                dwLength = pList->Length - sizeof(pList->RawDescription) - sizeof(pList->RawLength);

            pszRe = GlobalReAllocPtr(
                psz,
                lstrlen(psz) + sizeof("\r\n->") + dwLength,
                GMEM_MOVEABLE);
            if ( pszRe == 0 )
            {
                MiscGlobalFreePtr(&psz);
                ErrorMessage(STYLE_ERROR, IDS_PROPERTIESOUTOFMEMORY);
                return ( 0 );
            }
            psz = pszRe;
            lstrcat(psz, "\r\n->");
            strlenpsz = lstrlen(psz);
            hmemcpy(&psz[strlenpsz], (LPCSTR)pList->RawData, dwLength);
            psz[strlenpsz + (size_t)dwLength] = '\0';
        }
    }
    return ( psz );
}



 /******************************************************************************
 *  local functions
 ******************************************************************************/



/******************************************************************************
 *  PasswordDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
 * 
 *  dialog proc for Get Password dialog
 *
 *  parameters:
 *      hwndDlg - window handle for about box
 *      uMsg    - message id
 *      wParam, lParam - depends on uMsg
 *
 *  returns:
 *      TRUE if it has handled the message, else FALSE, except WM_INITDIALOG
 *      FALSE if SetFocus has been used (get password doesn't)
 ******************************************************************************/
static BOOL CALLBACK PasswordDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PIC_PARM *p = 0;    /* avoid use before init warning */
    char sz[sizeof(p->KeyField) + 1];
    LPSTR psz;
    int i;

    NOREFERENCE(p);
    switch ( msg )
        {
        case WM_INITDIALOG:
            SetWindowLong(hwndDlg, DWL_USER, lParam);
            SendMessage(
                GetDlgItem(hwndDlg, IDC_PASSWORD_EDIT),
                EM_LIMITTEXT,
                sizeof(p->KeyField),
                0);
            return ( TRUE );

        case WM_COMMAND:
            if ( wParam == IDOK )
                {
                psz = (LPSTR)GetWindowLong(hwndDlg, DWL_USER);
                GetDlgItemText(hwndDlg, IDC_PASSWORD_EDIT, sz, sizeof(sz));
                for ( i = lstrlen(sz) + 1; i < sizeof(p->KeyField); i++ )
                    sz[i] = '\0';
                hmemcpy(psz, sz, sizeof(p->KeyField));
                EndDialog(hwndDlg, wParam);
                return ( TRUE );
                }
            else if ( wParam == IDCANCEL )
                {
                EndDialog(hwndDlg, wParam);
                return ( TRUE );
                }
            break;

        default:
            break;
        }

    return ( FALSE );
}



/******************************************************************************
 *  DWORD QSpace(QUEUE PICFAR* pQ)
 * 
 *  space available in queue
 *
 *  parameters:
 *      pQ         - pointer to queue structure
 ******************************************************************************/
static DWORD QSpace(QUEUE PICFAR* pQ)
{
    if ( ( pQ->QFlags & Q_REVERSE) != 0 )
        {
        if ( pQ->Front == pQ->End )
            return ( pQ->Rear - pQ->Start );
        if ( pQ->Rear > pQ->Front )
            return ( pQ->Rear - pQ->Front - 1 );
        /* else pQ->Rear < pQ->Front */
        return ( pQ->Rear - pQ->Start + pQ->End - pQ->Front - 1 );
        }
    /* else not Q_REVERSE */

    if ( pQ->Front == pQ->Start )
        return ( pQ->End - pQ->Rear );
    if ( pQ->Rear < pQ->Front )
        return ( pQ->Front - pQ->Rear );
    /* else pQ->Rear > pQ->Front */
    return ( pQ->End - pQ->Rear + pQ->Front - pQ->Start - 1 );
}



/******************************************************************************
 *  void QCopyLenTo(QUEUE PICFAR* pQ, BYTE PICHUGE* pSrc, DWORD dwCopyLen)
 * 
 *  copy some number of bytes to the queue
 *
 *  parameters:
 *      pQ         - pointer to queue structure
 *      pSrc       - pointer to data to put in queue
 *      dwCopyLen  - length of data to copy
 *
 *  notes:
 *      there must be space in the queue for dwCopyLen bytes
 ******************************************************************************/
static void QCopyLenTo(QUEUE PICFAR* pQ, BYTE PICHUGE* pSrc, DWORD dwCopyLen)
{
    DWORD dw;

    assert(dwCopyLen <= QSpace(pQ));
    assert(dwCopyLen != 0);
    if ( ( pQ->QFlags & Q_REVERSE ) != 0 )
        {
        if ( pQ->Rear == pQ->Front )
            {
            pQ->Front = pQ->End;
            pQ->Rear  = pQ->Front;
            }
        if ( pQ->Rear <= pQ->Front && (DWORD)( pQ->Rear - pQ->Start ) < dwCopyLen )
            {
            dw = pQ->Rear - pQ->Start;
            dwCopyLen -= dw;
            hmemcpy(pQ->End - dwCopyLen, pSrc, dwCopyLen);
            hmemcpy(pQ->Start, pSrc + dwCopyLen, dw);
            pQ->Rear = pQ->End - dwCopyLen;
            }
        else
            {
            hmemcpy(pQ->Rear - dwCopyLen, pSrc, dwCopyLen);
            pQ->Rear -= dwCopyLen;
            if ( pQ->Rear == pQ->Start && pQ->Front != pQ->End )
                pQ->Rear = pQ->End;
            }
        }
    else
        {
        if ( pQ->Rear == pQ->Front )
            {
            pQ->Front = pQ->Start;
            pQ->Rear  = pQ->Front;
            }
        if ( pQ->Rear >= pQ->Front && (DWORD)( pQ->End - pQ->Rear ) < dwCopyLen )
            {
            dw = pQ->End - pQ->Rear;
            dwCopyLen -= dw;
            hmemcpy(pQ->Rear, pSrc, dw);
            hmemcpy(pQ->Start, pSrc + dw, dwCopyLen);
            pQ->Rear = pQ->Start + dwCopyLen;
            }
        else
            {
            hmemcpy(pQ->Rear, pSrc, dwCopyLen);
            pQ->Rear += dwCopyLen;
            if ( pQ->Rear == pQ->End && pQ->Front != pQ->Start )
                pQ->Rear = pQ->Start;
            }
        }
}



/******************************************************************************
 *  void QCopyLenFrom(BYTE PICHUGE* pDest, QUEUE PICFAR* pQ, DWORD dwCopyLen)
 * 
 *  copy some number of bytes from the queue
 *
 *  parameters:
 *      pDest      - pointer to buffer for queue data
 *      pQ         - pointer to queue structure
 *      dwCopyLen  - length of data to copy
 *
 *  notes:
 *      there must be dwCopyLen bytes of data in the queue
 ******************************************************************************/
static void QCopyLenFrom(BYTE PICHUGE* pDest, QUEUE PICFAR* pQ, DWORD dwCopyLen)
{
    DWORD dw;

    assert(dwCopyLen <= MiscQLen(pQ));
    assert(dwCopyLen != 0);

    if ( ( pQ->QFlags & Q_REVERSE ) != 0 )
        {
        if ( pQ->Rear > pQ->Front && (DWORD)( pQ->Front - pQ->Start ) < dwCopyLen )
            {
            dw = pQ->Front - pQ->Start;
            dwCopyLen -= dw;
            hmemcpy(pDest, pQ->End - dwCopyLen, dwCopyLen);
            hmemcpy(pDest + dwCopyLen, pQ->Start, dw);
            pQ->Front = pQ->End - dwCopyLen;
            }
        else
            {
            hmemcpy(pDest, pQ->Front - dwCopyLen, dwCopyLen);
            pQ->Front -= dwCopyLen;
            if ( pQ->Front == pQ->Start && pQ->Front != pQ->Rear )
                pQ->Front = pQ->End;
            }
        }
    else
        {
        if ( pQ->Rear < pQ->Front && (DWORD)( pQ->End - pQ->Front ) < dwCopyLen )
            {
            dw = pQ->End - pQ->Front;
            dwCopyLen -= dw;
            hmemcpy(pDest, pQ->Front, dw);
            hmemcpy(pDest + dw, pQ->Start, dwCopyLen);
            pQ->Front = pQ->Start + dwCopyLen;
            }
        else
            {
            hmemcpy(pDest, pQ->Front, dwCopyLen);
            pQ->Front += dwCopyLen;
            if ( pQ->Front == pQ->End && pQ->Front != pQ->Rear )
                pQ->Front = pQ->Start;
            }
        }
}



/******************************************************************************
 *  PIC2NextTypePacket(pList, Type)
 * 
 *  advance to the next PIC2 packet of Type
 *
 *  parameters:
 *      pList - pointer to pic2 packet after which to start the search
 *      Type  - type to find
 ******************************************************************************/
static P2PktGeneric PICHUGE *PIC2NextTypePacket(P2PktGeneric PICHUGE *pList, BYTE Type)
{
    while ( pList->Type != P2P_EOF )
    {
        pList = (P2PktGeneric PICHUGE *)
            ( (BYTE PICHUGE*)pList + sizeof(P2PktNull) + pList->Length );
        if ( pList->Type == Type )
            return ( pList );
    }
    return ( 0 );
}




/******************************************************************************
 *  PIC2FirstTypePacket(pList, Type)
 * 
 *  advance to the first PIC2 packet of Type
 *
 *  parameters:
 *      pList - pointer to pic2 packet to start search
 *      Type  - type to find
 ******************************************************************************/
static P2PktGeneric PICHUGE *PIC2FirstTypePacket(P2PktGeneric PICHUGE *pList, BYTE Type)
{
    if ( pList->Type == Type )
        return ( pList );
    return ( PIC2NextTypePacket(pList, Type) );
}
