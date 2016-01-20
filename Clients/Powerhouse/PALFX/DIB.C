/*----------------------------------------------------------------------------*\
|   Routines for dealing with Device independent bitmaps                       |
|									       |
\*----------------------------------------------------------------------------*/

#include <windows.h>
#include "gmem.h"
#include "dib.h"

HANDLE CreateLogicalDib(HBITMAP hbm, WORD biBits, HPALETTE hpal);

//DWORD NEAR PASCAL lread(int fh, VOID FAR *pv, DWORD ul);
//DWORD NEAR PASCAL lwrite(int fh, VOID FAR *pv, DWORD ul);

/* flags for _lseek */
#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0

/*
 *   Open a DIB file and return a MEMORY DIB, a memory handle containing..
 *
 *   BITMAP INFO    bi
 *   palette data
 *   bits....
 *
 */
HANDLE OpenDIB(LPSTR szFile)
{
    unsigned            fh;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpbi;
    DWORD               dwLen;
    DWORD               dwBits;
    HANDLE              hdib;
    HANDLE              h;
    OFSTRUCT            of;

    if (HIWORD((DWORD)szFile) == 0)
    {
        fh = LOWORD((DWORD)szFile);
    }
    else
    {
        fh = OpenFile(szFile, &of, OF_READ);
    }

    if (fh == -1)
        return NULL;

    hdib = ReadDibBitmapInfo(fh);

    if (!hdib)
        return NULL;

    DibInfo(hdib,&bi);

    /* How much memory do we need to hold the DIB */

    dwBits = bi.biSizeImage;
    dwLen  = bi.biSize + PaletteSize(&bi) + dwBits;

    /* Can we get more memory? */

    h = GReAlloc(hdib,dwLen);

    if (!h)
    {
        GFree(hdib);
        hdib = NULL;
    }
    else
    {
        hdib = h;
    }

    if (hdib)
    {
        lpbi = GLock(hdib);

        /* read in the bits */
        _hread(fh, (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), dwBits);

        GUnlock(hdib);
    }


    if (HIWORD((DWORD)szFile) != 0)
        _lclose(fh);

    return hdib;
}

/*
 *   Write a global handle in CF_DIB format to a file.
 *
 */
BOOL WriteDIB(LPSTR szFile,HANDLE hdib)
{
    BITMAPFILEHEADER	hdr;
    LPBITMAPINFOHEADER  lpbi;
    BITMAPINFOHEADER    bi;
    int                 fh;
    OFSTRUCT            of;
    DWORD               dwSize;

    if (!hdib)
        return FALSE;

    if (HIWORD((DWORD)szFile) == 0)
    {
        fh = LOWORD((DWORD)szFile);
    }
    else
    {
        fh = OpenFile(szFile,&of,OF_CREATE|OF_READWRITE);
    }

    if (fh == -1)
        return FALSE;

    DibInfo(hdib,&bi);

    dwSize = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;

    lpbi = GLock(hdib);

    hdr.bfType          = BFT_BITMAP;
    hdr.bfSize          = dwSize + sizeof(BITMAPFILEHEADER);
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) + lpbi->biSize +
                          PaletteSize(lpbi);

    _hwrite(fh, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER));
    _hwrite(fh, (LPVOID)lpbi, dwSize);

    GUnlock(hdib);

    if (HIWORD((DWORD)szFile) != 0)
        _lclose(fh);

    return TRUE;
}

/*
 *  DibInfo(hbi, lpbi)
 *
 *  retrives the DIB info associated with a CF_DIB format memory block.
 */
BOOL DibInfo(HANDLE hbi,LPBITMAPINFOHEADER lpbi)
{
    if (hbi)
    {
        *lpbi = *(LPBITMAPINFOHEADER)GLock(hbi);
        GUnlock(hbi);

        if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        {
            BITMAPCOREHEADER bc;

            bc = *(LPBITMAPCOREHEADER)lpbi;

            lpbi->biSize               = sizeof(BITMAPINFOHEADER);
            lpbi->biWidth              = (DWORD)bc.bcWidth;
            lpbi->biHeight             = (DWORD)bc.bcHeight;
            lpbi->biPlanes             =  (WORD)bc.bcPlanes;
            lpbi->biBitCount           =  (WORD)bc.bcBitCount;
            lpbi->biCompression        = BI_RGB;
            lpbi->biSizeImage          = 0;
            lpbi->biXPelsPerMeter      = 0;
            lpbi->biYPelsPerMeter      = 0;
            lpbi->biClrUsed            = 0;
            lpbi->biClrImportant       = 0;
        }

        /*
         * fill in the default fields
         */
        if (lpbi->biSizeImage == 0L)
            lpbi->biSizeImage = lpbi->biHeight * DIBWIDTHBYTES(*lpbi);

        if (lpbi->biClrUsed == 0L)
            lpbi->biClrUsed = DibNumColors(lpbi);

        return TRUE;
    }
    return FALSE;
}

/*
 *  CreateBIPalette()
 *
 *  Given a Pointer to a BITMAPINFO struct will create a
 *  a GDI palette object from the color table.
 *
 *  works with "old" and "new" DIB's
 *
 */
HPALETTE CreateBIPalette(LPBITMAPINFOHEADER lpbi)
{
    LOGPALETTE          *pPal;
    HPALETTE            hpal = NULL;
    WORD                nNumColors;
    WORD                i;
    RGBQUAD        FAR *pRgb;
    BOOL                fCoreHeader;

    if (!lpbi)
        return NULL;

    fCoreHeader = (lpbi->biSize == sizeof(BITMAPCOREHEADER));

    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    nNumColors = DibNumColors(lpbi);

    if (nNumColors)
    {
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));

        if (!pPal)
            goto exit;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        for (i = 0; i < nNumColors; i++)
        {
            pPal->palPalEntry[i].peRed   = pRgb->rgbRed;
            pPal->palPalEntry[i].peGreen = pRgb->rgbGreen;
            pPal->palPalEntry[i].peBlue  = pRgb->rgbBlue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;

            if (fCoreHeader)
                ((LPSTR)pRgb) += sizeof(RGBTRIPLE) ;
            else
                pRgb++;
        }

        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    else if (lpbi->biBitCount == 24)
    {
        hpal = CreateColorPalette();
    }

exit:
    return hpal;
}


/*
 *  CreateDibPalette()
 *
 *  Given a Global HANDLE to a BITMAPINFO Struct
 *  will create a GDI palette object from the color table.
 *
 *  works with "old" and "new" DIB's
 *
 */
HPALETTE CreateDibPalette(HANDLE hbi)
{
    HPALETTE hpal;

    if (!hbi)
        return NULL;

    hpal = CreateBIPalette((LPBITMAPINFOHEADER)GLock(hbi));
    GUnlock(hbi);
    return hpal;
}

//
// create a 6*6*6 rainbow palette
//
HPALETTE CreateColorPalette()
{
    LOGPALETTE          *pPal;
    PALETTEENTRY        *ppe;
    HPALETTE            hpal = NULL;
    WORD                nNumColors;
    BYTE                r,g,b;

    nNumColors = 6*6*6;
    pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));

    if (!pPal)
	goto exit;

    pPal->palNumEntries = nNumColors;
    pPal->palVersion	= PALVERSION;

    ppe = pPal->palPalEntry;

    for (r=0; r<6; r++)
        for (g=0; g<6; g++)
            for (b=0; b<6; b++)
            {
                ppe->peRed   = (BYTE)((WORD)r * 255 / 6);
                ppe->peGreen = (BYTE)((WORD)g * 255 / 6);
                ppe->peBlue  = (BYTE)((WORD)b * 255 / 6);
                ppe->peFlags = (BYTE)0;
                ppe++;
            }

    hpal = CreatePalette(pPal);
    LocalFree((HANDLE)pPal);

exit:
    return hpal;
}

/* CreateSystemPalette()
 *
 * Return a palette which represents the system (physical) palette.
 * By selecting this palette into a screen DC and realizing the palette,
 * the exact physical mapping will be restored
 *
 * one use for this is when "snaping" the screen as a bitmap
 *
 * On error (e.g. out of memory), NULL is returned.
 */
HPALETTE CreateSystemPalette()
{
    HDC             hdc;                    // DC onto the screen
    int             iSizePalette;           // size of entire palette
    int             iFixedPalette;          // number of reserved colors
    NPLOGPALETTE    pLogPal = NULL;
    HPALETTE        hpal = NULL;
    int             i;

    hdc = GetDC(NULL);

    iSizePalette = GetDeviceCaps(hdc, SIZEPALETTE);

    //
    // determine the number of 'static' system colors that
    // are currently reserved
    //
    if (GetSystemPaletteUse(hdc) == SYSPAL_STATIC)
        iFixedPalette = GetDeviceCaps(hdc, NUMCOLORS);
    else
        iFixedPalette = 2;

    //
    // create a logical palette containing the system colors;
    // this palette has all entries except fixed (system) colors
    // flagged as PC_NOCOLLAPSE
    //
    pLogPal = (NPLOGPALETTE)LocalAlloc(LPTR, sizeof(LOGPALETTE)
            + iSizePalette * sizeof(PALETTEENTRY));

    if (pLogPal)
    {
        pLogPal->palVersion = 0x300;
        pLogPal->palNumEntries = iSizePalette;

        GetSystemPaletteEntries(hdc, 0, iSizePalette, pLogPal->palPalEntry);

        for (i = iFixedPalette/2; i < iSizePalette-iFixedPalette/2; i++)
            pLogPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;

        hpal = CreatePalette(pLogPal);
	LocalFree((HANDLE)pLogPal);
    }

    ReleaseDC(NULL,hdc);
    return hpal;
}




/*
 *  ReadDibBitmapInfo()
 *
 *  Will read a file in DIB format and return a global HANDLE to it's
 *  BITMAPINFO.  This function will work with both "old" and "new"
 *  bitmap formats, but will allways return a "new" BITMAPINFO
 *
 */
HANDLE ReadDibBitmapInfo(int fh)
{
    DWORD     off;
    HANDLE    hbi = NULL;
    int       size;
    int       i;
    WORD      nNumColors;

    RGBQUAD FAR       *pRgb;
    BITMAPINFOHEADER   bi;
    BITMAPCOREHEADER   bc;
    LPBITMAPINFOHEADER lpbi;
    BITMAPFILEHEADER   bf;

    if (fh == -1)
        return NULL;

    off = _llseek(fh,0L,SEEK_CUR);

    if (sizeof(bf) != _lread(fh,(LPSTR)&bf,sizeof(bf)))
        return FALSE;

    /*
     *  do we have a RC HEADER?
     */
    if (!ISDIB(bf.bfType))
    {
        bf.bfOffBits = 0L;
        _llseek(fh,off,SEEK_SET);
    }

    if (sizeof(bi) != _lread(fh,(LPSTR)&bi,sizeof(bi)))
        return FALSE;

    nNumColors = DibNumColors(&bi);

    /*
     *  what type of bitmap info is this?
     */
    switch (size = (int)bi.biSize)
    {
        case sizeof(BITMAPINFOHEADER):
            break;

        case sizeof(BITMAPCOREHEADER):
            bc = *(LPBITMAPCOREHEADER)&bi;
            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = (DWORD)bc.bcWidth;
            bi.biHeight             = (DWORD)bc.bcHeight;
            bi.biPlanes             =  (WORD)bc.bcPlanes;
            bi.biBitCount           =  (WORD)bc.bcBitCount;
            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;

            _llseek(fh,(LONG)sizeof(BITMAPCOREHEADER)-sizeof(BITMAPINFOHEADER),SEEK_CUR);

            break;

        default:
            return NULL;       /* not a DIB */
    }

    /*
     *	fill in some default values!
     */
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = bi.biHeight * DIBWIDTHBYTES(bi);

    }

    if (bi.biXPelsPerMeter == 0)
    {
        bi.biXPelsPerMeter = 0;         // ??????????????
    }

    if (bi.biYPelsPerMeter == 0)
    {
        bi.biYPelsPerMeter = 0;         // ??????????????
    }

    if (bi.biClrUsed == 0)
    {
        bi.biClrUsed = DibNumColors(&bi);
    }

    hbi = GAlloc((LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
    if (!hbi)
        return NULL;

    lpbi = (VOID FAR *)GLock(hbi);
    *lpbi = bi;

    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);

    if (nNumColors)
    {
        if (size == sizeof(BITMAPCOREHEADER))
        {
            /*
             * convert a old color table (3 byte entries) to a new
             * color table (4 byte entries)
             */
            _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBTRIPLE));

            for (i=nNumColors-1; i>=0; i--)
            {
                RGBQUAD rgb;

                rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
                rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
                rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
                rgb.rgbReserved = (BYTE)0;

                pRgb[i] = rgb;
            }
        }
        else
        {
            _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBQUAD));
        }
    }

    if (bf.bfOffBits != 0L)
        _llseek(fh,off + bf.bfOffBits,SEEK_SET);

    GUnlock(hbi);
    return hbi;
}

/*  How big is the palette? if bits per pel not 24
 *  no of bytes to read is 6 for 1 bit, 48 for 4 bits
 *  256*3 for 8 bits and 0 for 24 bits
 */
WORD PaletteSize(VOID FAR * pv)
{
    #define lpbi ((LPBITMAPINFOHEADER)pv)
    #define lpbc ((LPBITMAPCOREHEADER)pv)

    WORD    NumColors;

    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return NumColors * sizeof(RGBTRIPLE);
    else
        return NumColors * sizeof(RGBQUAD);

    #undef lpbi
    #undef lpbc
}

/*  How Many colors does this DIB have?
 *  this will work on both PM and Windows bitmap info structures.
 */
WORD DibNumColors(VOID FAR * pv)
{
    #define lpbi ((LPBITMAPINFOHEADER)pv)
    #define lpbc ((LPBITMAPCOREHEADER)pv)

    int bits;

    /*
     *  with the new format headers, the size of the palette is in biClrUsed
     *  else is dependent on bits per pixel
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
    {
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;

        bits = lpbi->biBitCount;
    }
    else
    {
        bits = lpbc->bcBitCount;
    }

    switch (bits)
    {
    case 1:
            return 2;
    case 4:
            return 16;
    case 8:
            return 256;
    default:
            return 0;
    }

    #undef lpbi
    #undef lpbc
}

/*
 *  DibFromBitmap()
 *
 *  Will create a global memory block in DIB format that represents the DDB
 *  passed in
 *
 */
HANDLE DibFromBitmap(HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal, WORD wUsage)
{
    BITMAP               bm;
    BITMAPINFOHEADER     bi;
    BITMAPINFOHEADER FAR *lpbi;
    DWORD                dwLen;
    int                  nColors;
    HANDLE               hdib;
    HANDLE               h;
    HDC                  hdc;

    if (wUsage == 0)
        wUsage = DIB_RGB_COLORS;

    if (!hbm)
        return NULL;

    if (biStyle == BI_RGB && wUsage == DIB_RGB_COLORS)
        return CreateLogicalDib(hbm,biBits,hpal);

    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    GetObject(hbm,sizeof(bm),(LPSTR)&bm);
    GetObject(hpal,sizeof(nColors),(LPSTR)&nColors);

    if (biBits == 0)
        biBits = bm.bmPlanes * bm.bmBitsPixel;

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = biStyle;
    bi.biSizeImage          = 0;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = 0;
    bi.biClrImportant       = 0;

    dwLen  = bi.biSize + PaletteSize(&bi);

    hdc = CreateCompatibleDC(NULL);
    hpal = SelectPalette(hdc,hpal,FALSE);
    RealizePalette(hdc);  // why is this needed on a MEMORY DC? GDI bug??

    hdib = GAlloc(dwLen);

    if (!hdib)
        goto exit;

    lpbi = GLock(hdib);

    *lpbi = bi;

    /*
     *  call GetDIBits with a NULL lpBits param, so it will calculate the
     *  biSizeImage field for us
     */
    GetDIBits(hdc, hbm, 0, (WORD)bi.biHeight,
        NULL, (LPBITMAPINFO)lpbi, wUsage);

    bi = *lpbi;
    GUnlock(hdib);

    /*
     * HACK! if the driver did not fill in the biSizeImage field, make one up
     */
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = (DWORD)WIDTHBYTES(bm.bmWidth * biBits) * bm.bmHeight;

        if (biStyle != BI_RGB)
            bi.biSizeImage = (bi.biSizeImage * 3) / 2;
    }

    /*
     *  realloc the buffer big enough to hold all the bits
     */
    dwLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;
    if (h = GReAlloc(hdib,dwLen))
    {
        hdib = h;
    }
    else
    {
        GFree(hdib);
        hdib = NULL;
        goto exit;
    }

    /*
     *  call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
    lpbi = GLock(hdib);

    GetDIBits(hdc, hbm, 0, (WORD)bi.biHeight,
        (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi),
        (LPBITMAPINFO)lpbi, wUsage);

    bi = *lpbi;
    GUnlock(hdib);

exit:
    SelectPalette(hdc,hpal,FALSE);
    DeleteDC(hdc);
    return hdib;
}

/*
 *  BitmapFromDib()
 *
 *  Will create a DDB (Device Dependent Bitmap) given a global handle to
 *  a memory block in CF_DIB format
 *
 */
HBITMAP BitmapFromDib(HANDLE hdib, HPALETTE hpal, WORD wUsage)
{
    LPBITMAPINFOHEADER lpbi;
    HPALETTE    hpalT;
    HDC         hdc;
    HBITMAP     hbm;

    if (!hdib)
        return NULL;

    if (wUsage == 0)
        wUsage = DIB_RGB_COLORS;

    lpbi = GLock(hdib);

    if (!lpbi)
        return NULL;

    hdc = GetDC(NULL);
//  hdc = CreateCompatibleDC(NULL);

    if (hpal)
    {
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);  // why is this needed on a MEMORY DC? GDI bug??
    }

#if 0
    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
    {
        dx   = ((LPBITMAPCOREHEADER)lpbi)->bcWidth;
        dy   = ((LPBITMAPCOREHEADER)lpbi)->bcHeight;
        bits = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
    }
    else
    {
        dx   = (WORD)lpbi->biWidth;
        dy   = (WORD)lpbi->biHeight;
        bits = (WORD)lpbi->biBitCount;
    }

    if (bMonoBitmap /* || bits == 1 */)
    {
        hbm = CreateBitmap(dx,dy,1,1,NULL);
    }
    else
    {
        HDC hdcScreen = GetDC(NULL);
        hbm = CreateCompatibleBitmap(hdcScreen,dx,dy);
        ReleaseDC(NULL,hdcScreen);
    }

    if (hbm)
    {
        if (fErrProp)
            SetDIBitsErrProp(hdc,hbm,0,dy,
               (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
               (LPBITMAPINFO)lpbi,wUsage);
        else
            SetDIBits(hdc,hbm,0,dy,
               (LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
               (LPBITMAPINFO)lpbi,wUsage);
    }

#else
    hbm = CreateDIBitmap(hdc,
                (LPBITMAPINFOHEADER)lpbi,
                (LONG)CBM_INIT,
                (LPSTR)lpbi + (int)lpbi->biSize + PaletteSize(lpbi),
                (LPBITMAPINFO)lpbi,
                wUsage );
#endif

    if (hpal && hpalT)
        SelectPalette(hdc,hpalT,FALSE);

//  DeleteDC(hdc);
    ReleaseDC(NULL,hdc);

    GUnlock(hdib);
    return hbm;
}

/*
 *  DibFromDib()
 *
 *  Will convert a DIB in 1 format to a DIB in the specifed format
 *
 */
HANDLE DibFromDib(HANDLE hdib, DWORD biStyle, WORD biBits, HPALETTE hpal, WORD wUsage)
{
    BITMAPINFOHEADER bi;
    HBITMAP     hbm;
    BOOL        fKillPalette=FALSE;

    if (!hdib)
        return NULL;

    DibInfo(hdib,&bi);

    /*
     *  do we have the requested format already?
     */
    if (bi.biCompression == biStyle && bi.biBitCount == biBits)
        return hdib;

    if (hpal == NULL)
    {
        hpal = CreateDibPalette(hdib);
        fKillPalette++;
    }

    hbm = BitmapFromDib(hdib,hpal,wUsage);

    if (hbm == NULL)
    {
        hdib = NULL;
    }
    else
    {
        hdib = DibFromBitmap(hbm,biStyle,biBits,hpal,wUsage);
        DeleteObject(hbm);
    }

    if (fKillPalette && hpal)
        DeleteObject(hpal);

    return hdib;
}

/*
 *  CreateLogicalDib
 *
 *  Given a DDB and a HPALETTE create a "logical" DIB
 *
 *  if the HBITMAP is NULL create a DIB from the system "stock" bitmap
 *      This is used to save a logical palette to a disk file as a DIB
 *
 *  if the HPALETTE is NULL use the system "stock" palette (ie the
 *      system palette)
 *
 *  a "logical" DIB is a DIB where the DIB color table *exactly* matches
 *  the passed logical palette.  There will be no system colors in the DIB
 *  block, and a pixel value of <n> in the DIB will correspond to logical
 *  palette index <n>.
 *
 *  This is accomplished by doing a GetDIBits() with the DIB_PAL_COLORS
 *  option then converting the palindexes returned in the color table
 *  from palette indexes to logical RGB values.  The entire passed logical
 *  palette is always copied to the DIB color table.
 *
 *  The DIB color table will have exactly the same number of entries as
 *  the logical palette.  Normaly GetDIBits() will always set biClrUsed to
 *  the maximum colors supported by the device regardless of the number of
 *  colors in the logical palette
 *
 *  Why would you want to do this?  The major reason for a "logical" DIB
 *  is so when the DIB is written to a disk file then reloaded the logical
 *  palette created from the DIB color table will be the same as one used
 *  originaly to create the bitmap.  It also will prevent GDI from doing
 *  nearest color matching on PC_RESERVED palettes.
 *
 *  ** What do we do if the logical palette has more than 256 entries!!!!!
 *  ** GetDIBits() may return logical palette index's that are greater than
 *  ** 256, we cant represent these colors in the "logical" DIB
 *  **
 *  ** for now hose the caller?????
 *
 */

HANDLE CreateLogicalDib(HBITMAP hbm, WORD biBits, HPALETTE hpal)
{
    BITMAP              bm;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpDib;      // pointer to DIB
    LPBITMAPINFOHEADER  lpbi;       // temp pointer to BITMAPINFO
    DWORD               dwLen;
    DWORD               dw;
    int                 n;
    int                 nColors;
    HANDLE              hdib;
    HDC                 hdc;
    BYTE FAR *          lpBits;
    WORD FAR *          lpCT;
    RGBQUAD FAR *       lpRgb;
    PALETTEENTRY        peT;
    HPALETTE            hpalT;

    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    if (hbm == NULL)
        hbm = NULL; // ????GetStockObject(STOCK_BITMAP);

    GetObject(hpal,sizeof(nColors),(LPSTR)&nColors);
    GetObject(hbm,sizeof(bm),(LPSTR)&bm);

    if (biBits == 0)
        biBits = nColors > 16 ? 8 : 4;

    if (nColors > 256)      // ACK!
        ;                   // How do we handle this????

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = BI_RGB;
    bi.biSizeImage          = (DWORD)WIDTHBYTES(bm.bmWidth * biBits) * bm.bmHeight;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = nColors;
    bi.biClrImportant       = 0;

    dwLen = bi.biSize + PaletteSize(&bi) + bi.biSizeImage;

    hdib = GAlloc(dwLen);

    if (!hdib)
        return NULL;

    lpbi = GAllocPtr(bi.biSize + 256 * sizeof(RGBQUAD));

    if (!lpbi)
    {
        GFree(hdib);
        return NULL;
    }

    hdc = GetDC(NULL);
    hpalT = SelectPalette(hdc,hpal,FALSE);
    RealizePalette(hdc);  // why is this needed on a MEMORY DC? GDI bug??

    lpDib = GLock(hdib);

    *lpbi  = bi;
    *lpDib = bi;
    lpCT   = (WORD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    lpRgb  = (RGBQUAD FAR *)((LPSTR)lpDib + (WORD)lpDib->biSize);
    lpBits = (LPSTR)lpDib + (WORD)lpDib->biSize + PaletteSize(lpDib);

    /*
     *  call GetDIBits to get the DIB bits and fill the color table with
     *  logical palette index's
     */
    GetDIBits(hdc, hbm, 0, (WORD)bi.biHeight,
        lpBits,(LPBITMAPINFO)lpbi, DIB_PAL_COLORS);

    /*
     *  Now convert the DIB bits into "real" logical palette index's
     *
     *  lpCT        points to the DIB color table wich is a WORD array of
     *              logical palette index's
     *
     *  lpBits      points to the DIB bits, each DIB pixel is a index into
     *              the DIB color table.
     *
     */

    if (biBits == 8)
    {
        for (dw = 0; dw < bi.biSizeImage; dw++, ((BYTE huge *)lpBits)++)
            *lpBits = (BYTE)lpCT[*lpBits];
    }
    else // biBits == 4
    {
        for (dw = 0; dw < bi.biSizeImage; dw++, ((BYTE huge *)lpBits)++)
            *lpBits = lpCT[*lpBits & 0x0F] | (lpCT[(*lpBits >> 4) & 0x0F] << 4);
    }

    /*
     *  Now copy the RGBs in the logical palette to the dib color table
     */
    for (n=0; n<nColors; n++,lpRgb++)
    {
        GetPaletteEntries(hpal,n,1,&peT);

        lpRgb->rgbRed      = peT.peRed;
        lpRgb->rgbGreen    = peT.peGreen;
        lpRgb->rgbBlue     = peT.peBlue;
        lpRgb->rgbReserved = (BYTE)0;
    }

    GUnlock(hdib);
    GFreePtr(lpbi);

    SelectPalette(hdc,hpalT,FALSE);
    ReleaseDC(NULL,hdc);

    return hdib;
}

void xlatClut8(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
    DWORD dw;

    for (dw = 0; dw < dwSize; dw++, ((BYTE huge *)pb)++)
        *pb = xlat[*pb];
}

void xlatClut4(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
    DWORD dw;

    for (dw = 0; dw < dwSize; dw++, ((BYTE huge *)pb)++)
        *pb = xlat[*pb & 0x0F] | (xlat[(*pb >> 4) & 0x0F] << 4);
}

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2

void xlatRle8(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
    BYTE    cnt;
    BYTE    b;
    BYTE huge *prle = pb;

    for(;;)
    {
        cnt = *prle++;
        b   = *prle;

        if (cnt == RLE_ESCAPE)
        {
            prle++;

            switch (b)
            {
                case RLE_EOF:
                    return;

                case RLE_EOL:
                    break;

                case RLE_JMP:
                    prle++;     // skip dX
                    prle++;     // skip dY
                    break;

                default:
                    cnt = b;
                    for (b=0; b<cnt; b++,prle++)
                        *prle = xlat[*prle];

                    if (cnt & 1)
                        prle++;

                    break;
            }
        }
        else
        {
            *prle++ = xlat[b];
        }
    }
}

void xlatRle4(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
}

//
// MapDib - map the given DIB so it matches the specifed palette
//
BOOL MapDib(HANDLE hdib, HPALETTE hpal)
{
    LPBITMAPINFOHEADER  lpbi;
    PALETTEENTRY        pe;
    int                 n;
    int                 nDibColors;
    int                 nPalColors;
    BYTE FAR *          lpBits;
    RGBQUAD FAR *       lpRgb;
    BYTE                xlat[256];
    DWORD               dwSize;

    if (!hpal || !hdib)
        return FALSE;

    lpbi   = GLock(hdib);
    lpRgb  = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    lpBits = DibXY(lpbi,0,0);

    GetObject(hpal,sizeof(int),(LPSTR)&nPalColors);
    nDibColors = DibNumColors(lpbi);

    if (nPalColors > nDibColors)
    {
        //
        // This is bad, we need to grow the dib!  punt for now
        //
        nPalColors = nDibColors;
    }

    //
    //  build a xlat table. from the current DIB colors to the given
    //  palette.
    //
    for (n=0; n<nDibColors; n++)
        xlat[n] = (BYTE)GetNearestPaletteIndex(hpal,RGB(lpRgb[n].rgbRed,lpRgb[n].rgbGreen,lpRgb[n].rgbBlue));

    //
    // translate the DIB bits
    //
    if ((dwSize = lpbi->biSizeImage) == 0)
        dwSize = lpbi->biHeight * DIBWIDTHBYTES(*lpbi);

    switch ((WORD)lpbi->biCompression)
    {
        case BI_RLE8:
            xlatRle8(lpBits, dwSize, xlat);
            break;

        case BI_RLE4:
            xlatRle4(lpBits, dwSize, xlat);
            break;

        case BI_RGB:
            if (lpbi->biBitCount == 8)
                xlatClut8(lpBits, dwSize, xlat);
            else
                xlatClut4(lpBits, dwSize, xlat);
            break;
    }

    //
    //  Now copy the RGBs in the logical palette to the dib color table
    //
    for (n=0; n<nPalColors; n++)
    {
        GetPaletteEntries(hpal,n,1,&pe);

        lpRgb[n].rgbRed      = pe.peRed;
        lpRgb[n].rgbGreen    = pe.peGreen;
        lpRgb[n].rgbBlue     = pe.peBlue;
        lpRgb[n].rgbReserved = (BYTE)0;
    }

    for (n=nPalColors; n<nDibColors; n++)
    {
        lpRgb[n].rgbRed      = (BYTE)0;
        lpRgb[n].rgbGreen    = (BYTE)0;
        lpRgb[n].rgbBlue     = (BYTE)0;
        lpRgb[n].rgbReserved = (BYTE)0;
    }

    GUnlock(hdib);
    return TRUE;
}


/*
 *  Draws bitmap <hbm> at the specifed position in DC <hdc>
 *
 */
BOOL StretchBitmap(HDC hdc, int x, int y, int dx, int dy, HBITMAP hbm, int x0, int y0, int dx0, int dy0, DWORD rop)
{
    HDC hdcBits;

    HPALETTE hpal, hpalT;
    BOOL f;

    if (!hdc || !hbm)
        return FALSE;

    hpal = SelectPalette(hdc,GetStockObject(DEFAULT_PALETTE),FALSE);
    SelectPalette(hdc,hpal,FALSE);

    hdcBits = CreateCompatibleDC(hdc);
    SelectObject(hdcBits,hbm);
    hpalT = SelectPalette(hdcBits,hpal,FALSE);
    RealizePalette(hdcBits);
    f = StretchBlt(hdc,x,y,dx,dy,hdcBits,x0,y0,dx0,dy0,rop);
    SelectPalette(hdcBits,hpalT,FALSE);
    DeleteDC(hdcBits);

    return f;
}

/*
 *  Draws bitmap <hbm> at the specifed position in DC <hdc>
 *
 */
BOOL DrawBitmap(HDC hdc, int x, int y, HBITMAP hbm, DWORD rop)
{
    HDC hdcBits;
    BITMAP bm;
    BOOL f;

    if (!hdc || !hbm)
        return FALSE;

    hdcBits = CreateCompatibleDC(hdc);
    GetObject(hbm,sizeof(BITMAP),(LPSTR)&bm);
    SelectObject(hdcBits,hbm);
    f = BitBlt(hdc,x,y,bm.bmWidth,bm.bmHeight,hdcBits,0,0,rop);
    DeleteDC(hdcBits);

    return f;
}

/*
 *  Draws HDIB <hdib> at the specifed position in DC <hdc>
 *
 */
BOOL DrawDib(HDC hdc, int x, int y, HANDLE hdib, HPALETTE hpal, WORD wUsage)
{
    HPALETTE hpalT;

    if (hpal)
    {
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);
    }

    DibBlt(hdc,x,y,-1,-1,hdib,0,0,SRCCOPY,wUsage);

    if (hpal)
    {
        SelectPalette(hdc,hpalT,FALSE);
    }
    return TRUE;
}


/*
 *  SetDibUsage(hdib,hpal,wUsage)
 *
 *  Modifies the color table of the passed DIB for use with the wUsage
 *  parameter specifed.
 *
 *  if wUsage is DIB_PAL_COLORS the DIB color table is set to 0-256
 *  if wUsage is DIB_RGB_COLORS the DIB color table is set to the RGB values
 *      in the passed palette
 *
 */
BOOL SetDibUsage(HANDLE hdib, HPALETTE hpal,WORD wUsage)
{
    LPBITMAPINFOHEADER lpbi;
    PALETTEENTRY       ape[MAXPALETTE];
    RGBQUAD FAR *      pRgb;
    WORD FAR *         pw;
    int                nColors;
    int                n;

    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    if (!hdib)
        return FALSE;

    lpbi = GLock(hdib);

    if (!lpbi)
        return FALSE;

    nColors = DibNumColors(lpbi);

    if (nColors > 0)
    {
        pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);

        switch (wUsage)
        {
            //
            // Set the DIB color table to palette indexes
            //
            case DIB_PAL_COLORS:
                for (pw = (WORD FAR*)pRgb,n=0; n<nColors; n++,pw++)
                    *pw = n;
                break;

            //
            // Set the DIB color table to RGBQUADS
            //
            default:
            case DIB_RGB_COLORS:
                nColors = min(nColors,MAXPALETTE);

                GetPaletteEntries(hpal,0,nColors,ape);

                for (n=0; n<nColors; n++)
                {
                    pRgb[n].rgbRed      = ape[n].peRed;
                    pRgb[n].rgbGreen    = ape[n].peGreen;
                    pRgb[n].rgbBlue     = ape[n].peBlue;
                    pRgb[n].rgbReserved = 0;
                }
                break;
        }
    }
    GUnlock(hdib);
    return TRUE;
}

/*
 *  SetPalFlags(hpal,iIndex, cnt, wFlags)
 *
 *  Modifies the palette flags of all indexs in the range (iIndex - nIndex+cnt)
 *  to the parameter specifed.
 *
 */
BOOL SetPalFlags(HPALETTE hpal, int iIndex, int cntEntries, WORD wFlags)
{
    int     i;
    BOOL    f;
    PALETTEENTRY FAR *lppe;

    if (hpal == NULL)
        return FALSE;

    if (cntEntries < 0)
        GetObject(hpal,sizeof(int),(LPSTR)&cntEntries);

    lppe = GAllocPtr((LONG)cntEntries * sizeof(PALETTEENTRY));

    if (!lppe)
        return FALSE;

    GetPaletteEntries(hpal, iIndex, cntEntries, lppe);

    for (i=0; i<cntEntries; i++)
    {
        lppe[i].peFlags = (BYTE)wFlags;
    }

    f = SetPaletteEntries(hpal, iIndex, cntEntries, lppe);

    GFreePtr(lppe);
    return f;
}

/*
 *  PalEq(hpal1,hpal2)
 *
 *  return TRUE if the palette's are the same
 *
 */
BOOL PalEq(HPALETTE hpal1, HPALETTE hpal2)
{
    BOOL    f;
    int     i;
    int     nPal1,nPal2;
    PALETTEENTRY FAR *ppe;

    if (hpal1 == hpal2)
        return TRUE;

    if (!hpal1 || !hpal2)
        return FALSE;

    GetObject(hpal1,sizeof(int),(LPSTR)&nPal1);
    GetObject(hpal2,sizeof(int),(LPSTR)&nPal2);

    if (nPal1 != nPal2)
        return FALSE;

    ppe = GAllocPtr(nPal1 * 2 * sizeof(PALETTEENTRY));

    if (!ppe)
        return FALSE;

    GetPaletteEntries(hpal1, 0, nPal1, ppe);
    GetPaletteEntries(hpal2, 0, nPal2, ppe+nPal1);

    for (f=TRUE,i=0; f && i<nPal1; i++)
    {
        f &= (ppe[i].peRed   == ppe[i+nPal1].peRed   &&
              ppe[i].peBlue  == ppe[i+nPal1].peBlue  &&
              ppe[i].peGreen == ppe[i+nPal1].peGreen);
    }

    GFreePtr(ppe);
    return f;
}

/*
 *  StretchDibBlt()
 *
 *  draws a bitmap in CF_DIB format, using StretchDIBits()
 *
 *  takes the same parameters as StretchBlt()
 */
BOOL StretchDibBlt(HDC hdc, int x, int y, int dx, int dy, HANDLE hdib, int x0, int y0, int dx0, int dy0, LONG rop, WORD wUsage)
{
    LPBITMAPINFOHEADER lpbi;
    LPSTR        pBuf;
    BOOL         f;

    if (!hdib)
        return PatBlt(hdc,x,y,dx,dy,rop);

    if (wUsage == 0)
        wUsage = DIB_RGB_COLORS;

    lpbi = GLock(hdib);

    if (!lpbi)
        return FALSE;

    if (dx0 == -1 && dy0 == -1)
    {
        if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        {
            dx0 = ((LPBITMAPCOREHEADER)lpbi)->bcWidth;
            dy0 = ((LPBITMAPCOREHEADER)lpbi)->bcHeight;
        }
        else
        {
            dx0 = (int)lpbi->biWidth;
            dy0 = (int)lpbi->biHeight;
        }
    }

    if (dx < 0 && dy < 0)
    {
        dx = dx0 * -dx;
        dy = dy0 * -dy;
    }

    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);

    f = StretchDIBits (
        hdc,
        x,y,
        dx,dy,
        x0,y0,
        dx0,dy0,
        pBuf, (LPBITMAPINFO)lpbi,
        wUsage,
        rop);

    GUnlock(hdib);
    return f;
}

/*
 *  DibBlt()
 *
 *  draws a bitmap in CF_DIB format, using SetDIBits to device.
 *
 *  takes the same parameters as BitBlt()
 */
BOOL DibBlt(HDC hdc, int x0, int y0, int dx, int dy, HANDLE hdib, int x1, int y1, LONG rop, WORD wUsage)
{
    LPBITMAPINFOHEADER lpbi;
    LPSTR       pBuf;
    BOOL        f;

    if (!hdib)
        return PatBlt(hdc,x0,y0,dx,dy,rop);

    if (wUsage == 0)
        wUsage = DIB_RGB_COLORS;

    lpbi = GLock(hdib);

    if (!lpbi)
        return FALSE;

    if (dx == -1 && dy == -1)
    {
        if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        {
            dx = ((LPBITMAPCOREHEADER)lpbi)->bcWidth;
            dy = ((LPBITMAPCOREHEADER)lpbi)->bcHeight;
        }
        else
        {
            dx = (int)lpbi->biWidth;
            dy = (int)lpbi->biHeight;
        }
    }

    pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);

#if 0
    f = SetDIBitsToDevice(hdc, x0, y0, dx, dy,
        x1,y1,
        x1,
        dy,
        pBuf, (LPBITMAPINFO)lpbi,
        wUsage );
#else
    f = StretchDIBits (
        hdc,
        x0,y0,
        dx,dy,
        x1,y1,
        dx,dy,
        pBuf, (LPBITMAPINFO)lpbi,
        wUsage,
        rop);
#endif

    GUnlock(hdib);
    return f;
}

LPVOID DibLock(HANDLE hdib,int x, int y)
{
    return DibXY((LPBITMAPINFOHEADER)GLock(hdib),x,y);
}

VOID DibUnlock(HANDLE hdib)
{
    GUnlock(hdib);
}

LPVOID DibXY(LPBITMAPINFOHEADER lpbi,int x, int y)
{
    BYTE huge *pBits;
    DWORD ulWidthBytes;

    pBits = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);

    ulWidthBytes = DIBWIDTHBYTES(*lpbi);

    pBits += (ulWidthBytes * (long)y) + (x * (int)lpbi->biBitCount / 8);

    return (LPVOID)pBits;
}

HANDLE CreateDib(int bits, int dx, int dy)
{
    HANDLE              hdib;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpbi;
    DWORD FAR *         pRgb;
    WORD                i;

    //
    // These are the standard VGA colors, we will be stuck with until the
    // end of time!
    //
    static DWORD CosmicColors[16] = {
         0x00000000        // 0000  black
        ,0x00800000        // 0001  dark red
        ,0x00008000        // 0010  dark green
        ,0x00808000        // 0011  mustard
        ,0x00000080        // 0100  dark blue
        ,0x00800080        // 0101  purple
        ,0x00008080        // 0110  dark turquoise
        ,0x00C0C0C0        // 1000  gray
        ,0x00808080        // 0111  dark gray
        ,0x00FF0000        // 1001  red
        ,0x0000FF00        // 1010  green
        ,0x00FFFF00        // 1011  yellow
        ,0x000000FF        // 1100  blue
        ,0x00FF00FF        // 1101  pink (magenta)
        ,0x0000FFFF        // 1110  cyan
        ,0x00FFFFFF        // 1111  white
        };

    if (bits <= 0)
        bits = 8;

    bi.biSize           = sizeof(BITMAPINFOHEADER);
    bi.biPlanes         = 1;
    bi.biBitCount       = bits;
    bi.biWidth          = dx;
    bi.biHeight         = dy;
    bi.biCompression    = BI_RGB;
    bi.biSizeImage      = (long) dy * DIBWIDTHBYTES(bi);
    bi.biXPelsPerMeter  = 0;
    bi.biYPelsPerMeter  = 0;
    bi.biClrUsed	= 0;
    bi.biClrImportant   = 0;
    bi.biClrUsed	= DibNumColors(&bi);

    hdib = GAllocF(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(BITMAPINFOHEADER)
        + (long)bi.biClrUsed * sizeof(RGBQUAD)
        + (long)dy * DIBWIDTHBYTES(bi));

    if (hdib)
    {
        lpbi  = GLock(hdib);
        *lpbi = bi;

        pRgb  = (LPVOID)((LPSTR)lpbi + (int)lpbi->biSize);

        //
        //  setup the color table
        //
        if (bits == 1)
        {
            pRgb[0] = CosmicColors[0];
            pRgb[1] = CosmicColors[15];
        }
        else
        {
            for (i=0; i<bi.biClrUsed; i++)
                pRgb[i] = CosmicColors[i % 16];
        }

        GUnlock(hdib);
    }

    return hdib;
}

HANDLE CopyDib (HANDLE hdib)
{
    BYTE huge *ps;
    BYTE huge *pd;
    HANDLE h;
    DWORD cnt;

    if (h = GAlloc(cnt = GSize(hdib)))
    {
        ps = GLock(hdib);
        pd = GLock(h);
	hmemcpy(pd, ps, cnt);

        GUnlock(hdib);
        GUnlock(h);
    }
    return h;
}
