/*----------------------------------------------------------------------------*\
|   Routines for dealing with Device independent bitmaps                       |
\*----------------------------------------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include "dib.h"
#include <memory.h>             // for _fmemcpy()

#ifdef WIN32
    #define _huge
    #define hmemcpy memcpy
#endif

#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

/* flags for _lseek */
#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0

// I am quite happy that this version of dib.h did not 
// have any of the damn macros exported.  So, I include
// the necessary ones here.
#define BI_WIDTH(pBI)       ((pBI)->bmiHeader.biWidth)
#define BI_HEIGHT(pBI)      ((pBI)->bmiHeader.biHeight)


#define DIB_PCLRTAB(pDIB)   ((LPRGBQUAD)(((LPSTR)((LPBITMAPINFO)(pDIB))) \
                            + sizeof(BITMAPINFOHEADER)))
#define DIB_WIDTH(pDIB)     (BI_WIDTH((LPBITMAPINFO)(pDIB)))
#define DIB_HEIGHT(pDIB)    (BI_HEIGHT((LPBITMAPINFO)(pDIB)))
#define DIB_STORAGEWIDTH(pDIB) ((DIB_WIDTH(pDIB) + 3) & ~3)
#define DIB_BISIZE(pDIB)    (sizeof(BITMAPINFOHEADER) \
                            + DibNumColors(pDIB) * sizeof(RGBQUAD))
#define DIB_PBITS(pDIB)     (((LPSTR)((LPBITMAPINFO)(pDIB))) \
                            + DIB_BISIZE(pDIB))

/*
 *   Open a DIB file and return a MEMORY DIB, a memory handle containing..
 *
 *   BITMAP INFO    bi
 *   palette data
 *   bits....
 *
 */
PDIB WINAPI DibOpenFile(LPSTR szFile)
{
    HFILE               fh;
    DWORD               dwLen;
    DWORD               dwBits;
    PDIB                pdib;
    LPVOID              p;
    OFSTRUCT            of;

#ifdef WIN32
    #define GetCurrentInstance()    GetModuleHandle(NULL)
#else
    #define GetCurrentInstance()    (HINSTANCE)SELECTOROF((LPVOID)&of)
#endif

    fh = OpenFile(szFile, &of, OF_READ);

    if (fh == -1)
    {
        HRSRC h;

        h = FindResource(GetCurrentInstance(), szFile, RT_BITMAP);

#ifdef WIN32
        //!!! can we call GlobalFree() on this? is it the right format.
        //!!! can we write to this resource?
        if (h)
            return LockResource(LoadResource(GetCurrentInstance(), h));
#else
        if (h)
            fh = AccessResource(GetCurrentInstance(), h);
#endif
    }

    if (fh == -1)
        return NULL;

    pdib = DibReadBitmapInfo(fh);

    if (!pdib)
        return NULL;

    /* How much memory do we need to hold the DIB */

    dwBits = pdib->biSizeImage;
    dwLen  = pdib->biSize + DibPaletteSize(pdib) + dwBits;

    /* Can we get more memory? */

    p = GlobalReAllocPtr(pdib,dwLen,0);

    if (!p)
    {
        GlobalFreePtr(pdib);
        pdib = NULL;
    }
    else
    {
        pdib = (PDIB)p;
    }

    if (pdib)
    {
        /* read in the bits */
        _hread(fh, (LPBYTE)pdib + (UINT)pdib->biSize + DibPaletteSize(pdib), dwBits);
    }

    _lclose(fh);

if (pdib->biCompression == BI_RLE8)
	{
	PDIB pnewDib;
	dwLen  = pdib->biSize + DibPaletteSize(pdib) + (DWORD)DibWidthBytes(pdib) * (DWORD)(UINT)pdib->biHeight;

	pnewDib = (PDIB) GlobalAllocPtr (GMEM_MOVEABLE, dwLen);
	_fmemcpy (pnewDib, pdib, (size_t)(pdib->biSize + DibPaletteSize (pdib)));
    DecodeRle(pdib, DibPtr (pnewDib), DibPtr (pdib));
    DibFree (pdib);
    pdib = pnewDib;
    pdib->biCompression = BI_RGB;
	pdib->biSizeImage = pdib->biHeight * DibWidthBytes (pdib);
    }
    return pdib;
}

/*
 *   Write a global handle in CF_DIB format to a file.
 *
 */
BOOL WINAPI DibWriteFile(PDIB pdib, LPSTR szFile)
{
    BITMAPFILEHEADER    hdr;
    HFILE               fh;
    OFSTRUCT            of;
    DWORD               dwSize;
	LPVOID				poutput;
	BOOL				bFreeOutput;
	DWORD				dwImageSize, dwCompression;
	
	
    if (!pdib)
        return FALSE;

    fh = OpenFile(szFile,&of,OF_CREATE|OF_READWRITE);

    if (fh == -1)
        return FALSE;

	dwImageSize = pdib->biSizeImage;
	dwCompression = pdib->biCompression;
	
    if (pdib->biCompression == BI_RLE8)
    {
    	bFreeOutput = FALSE;
    	poutput = DibPtr (pdib);
    }
    else
    {
    	poutput = GlobalAllocPtr (GMEM_MOVEABLE, DibSizeImage (pdib));
    	bFreeOutput = TRUE;
    	DeltaFrame386 (pdib, NULL, DibPtr (pdib), poutput);
		pdib->biCompression = BI_RLE8;
	}

    dwSize = DibSize(pdib);

    hdr.bfType          = BFT_BITMAP;
    hdr.bfSize          = dwSize + sizeof(BITMAPFILEHEADER);
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) + pdib->biSize +
                          DibPaletteSize(pdib);

    _lwrite(fh,(LPCSTR)(LPVOID)&hdr,sizeof(BITMAPFILEHEADER));
	//Write the header
	_lwrite (fh, (LPCSTR) pdib, (UINT)(pdib->biSize + DibPaletteSize(pdib)));
	//write the bits
	_hwrite (fh, (LPCSTR) poutput, pdib->biSizeImage);

#if 0
    _hwrite(fh,(LPCSTR)(LPVOID)pdib,dwSize);
#endif
    _lclose(fh);

if (bFreeOutput)
	GlobalFreePtr (poutput);
	
	pdib->biSizeImage = dwImageSize;
	pdib->biCompression = dwCompression;
    return TRUE;
}

/*
 *  CreateBIPalette()
 *
 *  Given a Pointer to a BITMAPINFO struct will create a
 *  a GDI palette object from the color table.
 *
 */
HPALETTE WINAPI DibCreatePalette(PDIB pdib)
{
    NPLOGPALETTE         pPal;
    HPALETTE            hpal = NULL;
    int                 nNumColors;
    int                 i;
    RGBQUAD FAR *       pRgb;

    if (!pdib)
        return NULL;

    nNumColors = DibNumColors(pdib);
    
    if (nNumColors == 3 && DibCompression(pdib) == BI_BITFIELDS)
        nNumColors = 0;

    if (nNumColors > 0)
    {
        pRgb = DibColors(pdib);
        pPal = (NPLOGPALETTE)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));

        if (!pPal)
            goto exit;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = 0x300;

        for (i = 0; i < nNumColors; i++)
        {
            pPal->palPalEntry[i].peRed   = pRgb->rgbRed;
            pPal->palPalEntry[i].peGreen = pRgb->rgbGreen;
            pPal->palPalEntry[i].peBlue  = pRgb->rgbBlue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;

            pRgb++;
        }

        hpal = CreatePalette(pPal);
        LocalFree((HLOCAL)pPal);
    }
    else
    {
#ifdef WIN32                 
        HDC hdc = GetDC(NULL);
        hpal = CreateHalftonePalette(hdc);      
        ReleaseDC(NULL, hdc);
#endif          
    }

exit:
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
PDIB WINAPI DibReadBitmapInfo(HFILE fh)
{
    DWORD     off;
    HANDLE    hbi = NULL;
    int       size;
    int       i;
    int       nNumColors;

    RGBQUAD FAR       *pRgb;
    BITMAPINFOHEADER   bi;
    BITMAPCOREHEADER   bc;
    BITMAPFILEHEADER   bf;
    PDIB               pdib;

    if (fh == -1)
        return NULL;

    off = _llseek(fh,0L,SEEK_CUR);

    if (sizeof(bf) != _lread(fh,(LPSTR)&bf,sizeof(bf)))
        return FALSE;

    /*
     *  do we have a RC HEADER?
     */
    if (bf.bfType != BFT_BITMAP)
    {
        bf.bfOffBits = 0L;
        _llseek(fh,off,SEEK_SET);
    }

    if (sizeof(bi) != _lread(fh,(LPSTR)&bi,sizeof(bi)))
        return FALSE;

    /*
     *  what type of bitmap info is this?
     */
    switch (size = (int)bi.biSize)
    {
        default:
        case sizeof(BITMAPINFOHEADER):
            break;

        case sizeof(BITMAPCOREHEADER):
            bc = *(BITMAPCOREHEADER*)&bi;
            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = (DWORD)bc.bcWidth;
            bi.biHeight             = (DWORD)bc.bcHeight;
            bi.biPlanes             =  (UINT)bc.bcPlanes;
            bi.biBitCount           =  (UINT)bc.bcBitCount;
            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = 0;
            bi.biClrImportant       = 0;

            _llseek(fh,(LONG)sizeof(BITMAPCOREHEADER)-sizeof(BITMAPINFOHEADER),SEEK_CUR);

            break;
    }

    nNumColors = DibNumColors(&bi);

#if 0
    if (bi.biSizeImage == 0)
        bi.biSizeImage = DibSizeImage(&bi);

    if (bi.biClrUsed == 0)
        bi.biClrUsed = DibNumColors(&bi);
#else
    FixBitmapInfo(&bi);
#endif

    pdib = (PDIB)GlobalAllocPtr(GMEM_MOVEABLE,(LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));

    if (!pdib)
        return NULL;

    *pdib = bi;

    pRgb = DibColors(pdib);

    if (nNumColors)
    {
        if (size == sizeof(BITMAPCOREHEADER))
        {
            /*
             * convert a old color table (3 byte entries) to a new
             * color table (4 byte entries)
             */
            _lread(fh,(LPVOID)pRgb,nNumColors * sizeof(RGBTRIPLE));

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
            _lread(fh,(LPVOID)pRgb,nNumColors * sizeof(RGBQUAD));
        }
    }

    if (bf.bfOffBits != 0L)
        _llseek(fh,off + bf.bfOffBits,SEEK_SET);

    return pdib;
}

/*
 *  DibFromBitmap()
 *
 *  Will create a global memory block in DIB format that represents the DDB
 *  passed in
 *
 */
PDIB WINAPI DibFromBitmap(HBITMAP hbm, DWORD biStyle, UINT biBits, HPALETTE hpal, UINT wUsage)
{   
    PDIB                 pdib;
    BITMAP               bm;
    int                  nColors=0;
    HDC                  hdc;
    LPVOID               p;
    UINT                 u;     

    if (!hbm)
        return NULL;

    if (hpal == NULL)
        hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

    GetObject(hbm,sizeof(bm),(LPVOID)&bm);
    GetObject(hpal,sizeof(nColors),(LPVOID)&nColors);

    if (biBits == 0)
        biBits = bm.bmPlanes * bm.bmBitsPixel;

    pdib = (PDIB)GlobalAllocPtr(GMEM_MOVEABLE,sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

    if (!pdib)
        return NULL;

    pdib->biSize               = sizeof(BITMAPINFOHEADER);
    pdib->biWidth              = bm.bmWidth;
    pdib->biHeight             = bm.bmHeight;
    pdib->biPlanes             = 1;
    pdib->biBitCount           = biBits;
    pdib->biCompression        = biStyle;
    pdib->biSizeImage          = 0;
    pdib->biXPelsPerMeter      = 0;
    pdib->biYPelsPerMeter      = 0;
    pdib->biClrUsed            = 0;
    pdib->biClrImportant       = 0;
    pdib->biClrUsed            = DibNumColors(pdib);

    // this is a silly hack to test Win32/NT and BI_BITFIELDS
    // convert to a 565 (for 16 bit) or a RGB DIB (for 32/24 bit)
    if (biStyle == BI_BITFIELDS)
    {            
        LPDWORD pdw = (LPDWORD)DibColors(pdib);
                      
        switch (biBits)
        {
            case 16:            
                pdw[0] = 0xF800;        // masks for RGB 565
                pdw[1] = 0x07E0;
                pdw[2] = 0x001F;
                break;
                                
            case 24:
            case 32:
                pdw[0] = 0x0000FF;      // masks for RGB (not normal BGR)
                pdw[1] = 0x00FF00;
                pdw[2] = 0xFF0000;
                break;    
        }
    }   

    hdc = CreateCompatibleDC(NULL);
    hpal = SelectPalette(hdc,hpal,FALSE);
    RealizePalette(hdc);
    
#ifdef WIN32                                   
    SetStretchBltMode(hdc, HALFTONE);
#endif    

    /*
     *  call GetDIBits with a NULL lpBits param, so it will calculate the
     *  biSizeImage field for us
     */
    GetDIBits(hdc, hbm, 0, (UINT)pdib->biHeight,
        NULL, (LPBITMAPINFO)pdib, wUsage);

    pdib->biClrUsed = DibNumColors(pdib);
    pdib->biSizeImage = DibSizeImage(pdib);

    /*
     * HACK! if the driver did not fill in the biSizeImage field, make one up
     */
    if (pdib->biSizeImage == 0)
    {
        pdib->biSizeImage = DibSizeImage(pdib);

        if (biStyle != BI_RGB)
            pdib->biSizeImage = (pdib->biSizeImage * 3) / 2;
    }

    /*
     *  realloc the buffer big enough to hold all the bits
     */
    if (p = GlobalReAllocPtr(pdib,DibSize(pdib),0))
    {
        pdib = (PDIB)p;
    }
    else
    {
        GlobalFreePtr(pdib);
        pdib = NULL;
        goto exit;
    }

    /*
     *  call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
    u = GetDIBits(hdc, hbm, 0, (UINT)pdib->biHeight,
        DibPtr(pdib),(LPBITMAPINFO)pdib, wUsage);

    pdib->biClrUsed = DibNumColors(pdib);
    pdib->biSizeImage = DibSizeImage(pdib);
    
    // error getting bits.
    
    if (u == 0)
    {
        DibFree(pdib);
        pdib = NULL;
    }   

exit:
    SelectPalette(hdc,hpal,FALSE);
    DeleteDC(hdc);
    return pdib;
}

/*
 *  BitmapFromDib()
 *
 *  Will create a DDB (Device Dependent Bitmap) given a global handle to
 *  a memory block in CF_DIB format
 *
 */
HBITMAP WINAPI BitmapFromDib(PDIB pdib, HPALETTE hpal, UINT wUsage)
{
    HPALETTE    hpalT;
    HDC         hdc;
    HBITMAP     hbm;
    DWORD       dwFlags = CBM_INIT;

    if (!pdib)
        return NULL;

#ifdef WIN32
    if (wUsage == (UINT)-1)	 // silly hack for ConvertDib
        if (pdib->biCompression == 0 || 
            pdib->biCompression == BI_BITFIELDS)
	    dwFlags |= CBM_CREATEDIB;	
#endif
    if (wUsage == (UINT)-1)
	wUsage = DIB_RGB_COLORS;	// silly hack for ConvertDib

    hdc = GetDC(NULL);

    if (hpal)
    {
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);
    }
                    
#ifdef WIN32                                   
    SetStretchBltMode(hdc, HALFTONE);	   //??? does this do anything?
#endif    

    hbm = CreateDIBitmap(hdc,
                pdib,dwFlags,
                DibPtr(pdib),
                (LPBITMAPINFO)pdib,
                wUsage);

    if (hpal && hpalT)
        SelectPalette(hdc,hpalT,FALSE);

    ReleaseDC(NULL,hdc);

    return hbm;
}

/*
 *  DibSetUsage(hdib,hpal,wUsage)
 *
 *  Modifies the color table of the passed DIB for use with the wUsage
 *  parameter specifed.
 *
 *  if wUsage is DIB_PAL_COLORS the DIB color table is set to 0-256
 *  if wUsage is DIB_RGB_COLORS the DIB color table is set to the RGB values
 *      in the passed palette
 *
 */
BOOL WINAPI DibSetUsage(PDIB pdib, HPALETTE hpal,UINT wUsage)
{
    PALETTEENTRY       ape[256];
    RGBQUAD FAR *      pRgb;
    WORD FAR *         pw;
    int                nColors;
    int                n;

    if (hpal == NULL)
        hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

    if (!pdib)
        return FALSE;

    nColors = DibNumColors(pdib);
    
    if (nColors == 3 && DibCompression(pdib) == BI_BITFIELDS)
        nColors = 0;

    if (nColors > 0)
    {
        pRgb = DibColors(pdib);

        switch (wUsage)
        {
#ifdef WIN32
            case DIB_PAL_INDICES:
                // hack!!! fall through  
#endif
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
                nColors = min(nColors,256);

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
    return TRUE;
}

/*
 *  DibDraw()
 */
BOOL WINAPI DibDraw(HDC hdc,  int x,  int y,  int dx,  int dy,
            PDIB pdib, int x0, int y0, int dx0, int dy0,
            LONG rop, UINT wUsage)
{
    BOOL f;
    LPVOID lpBits;

    if (!pdib)
        return FALSE;

    if (dx0 == -1 && dy0 == -1)
    {
        dx0 = DibWidth(pdib);
        dy0 = DibHeight(pdib);
    }

    if (dx == -1 && dy == -1)
    {
	dx = dx0;
	dy = dy0;
    }          
        
#if 0                                   
#ifdef WIN32
    if (pdib->biBitCount > 8)
        SetStretchBltMode(hdc, HALFTONE);
    else
        SetStretchBltMode(hdc, COLORONCOLOR);
#else    
    SetStretchBltMode(hdc, COLORONCOLOR);
#endif    
#endif

    lpBits = DibPtr(pdib);
    
    f = StretchDIBits(
        hdc,
        x,y,
        dx,dy,
        x0,y0,
        dx0,dy0,
        lpBits, (LPBITMAPINFO)pdib,
        wUsage,
        rop);

    return f;
}

PDIB WINAPI DibCopy(PDIB pdib)
{
    PDIB pdibCopy;

    if (pdib == NULL)
        return NULL;

    if (pdibCopy = (PDIB)GlobalAllocPtr(GMEM_MOVEABLE,DibSize(pdib)))
        hmemcpy(pdibCopy, pdib, DibSize(pdib));

    return pdibCopy;
}

PDIB WINAPI DibCreate(int bits, int dx, int dy)
{
    LPBITMAPINFOHEADER lpbi ;
    DWORD       dwSizeImage;
    int         i;
    DWORD FAR  *pdw;

    dwSizeImage = dy*(DWORD)((dx*bits/8+3)&~3);

    lpbi = (PDIB)GlobalAllocPtr(GHND,sizeof(BITMAPINFOHEADER)+dwSizeImage + 1024);

    if (lpbi == NULL)
        return NULL;

    lpbi->biSize            = sizeof(BITMAPINFOHEADER) ;
    lpbi->biWidth           = dx;
    lpbi->biHeight          = dy;
    lpbi->biPlanes          = 1;
    lpbi->biBitCount        = bits ;
    lpbi->biCompression     = BI_RGB ;
    lpbi->biSizeImage       = dwSizeImage;
    lpbi->biXPelsPerMeter   = 0 ;
    lpbi->biYPelsPerMeter   = 0 ;
    lpbi->biClrUsed         = 0 ;
    lpbi->biClrImportant    = 0 ;

    if (bits == 4)
        lpbi->biClrUsed = 16;

    else if (bits == 8)
        lpbi->biClrUsed = 256;

    pdw = (DWORD FAR *)((LPBYTE)lpbi+(int)lpbi->biSize);

    for (i=0; i<(int)lpbi->biClrUsed/16; i++)
    {
        *pdw++ = 0x00000000;    // 0000  black
        *pdw++ = 0x00800000;    // 0001  dark red
        *pdw++ = 0x00008000;    // 0010  dark green
        *pdw++ = 0x00808000;    // 0011  mustard
        *pdw++ = 0x00000080;    // 0100  dark blue
        *pdw++ = 0x00800080;    // 0101  purple
        *pdw++ = 0x00008080;    // 0110  dark turquoise
        *pdw++ = 0x00C0C0C0;    // 1000  gray
        *pdw++ = 0x00808080;    // 0111  dark gray
        *pdw++ = 0x00FF0000;    // 1001  red
        *pdw++ = 0x0000FF00;    // 1010  green
        *pdw++ = 0x00FFFF00;    // 1011  yellow
        *pdw++ = 0x000000FF;    // 1100  blue
        *pdw++ = 0x00FF00FF;    // 1101  pink (magenta)
        *pdw++ = 0x0000FFFF;    // 1110  cyan
        *pdw++ = 0x00FFFFFF;    // 1111  white
    }

    return (PDIB)lpbi;
}

PDIB WINAPI DibConvert(PDIB pdib, int BitCount, DWORD biCompression)
{
    HBITMAP hbm;            
    HPALETTE hpal = NULL;
    
    if (pdib == NULL)
        return NULL;

    if (DibNumColors(pdib) > 3)
        hpal = DibCreatePalette(pdib);
    // pass -1 as wUsage so we dont get a device bitmap, see
    // BitmapFromDib for the Hack. NOTE only in Win32
    hbm = BitmapFromDib(pdib, hpal, (UINT)-1 /*DIB_RGB_COLORS*/); 
        
    if (hbm)
        pdib = DibFromBitmap(hbm, biCompression, BitCount, hpal, DIB_RGB_COLORS);
    else
        pdib = NULL;
        
    if (hbm)
        DeleteObject(hbm);
        
    if (hpal)
        DeleteObject(hpal);
        
    return pdib;
}


static void xlatClut8(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
    DWORD dw;

#ifdef __cplusplus
    for (dw = 0; dw < dwSize; dw++, ((BYTE _huge *&)pb)++)
#else
    for (dw = 0; dw < dwSize; dw++, ((BYTE _huge *)pb)++)
#endif
        *pb = xlat[*pb];
}

static void xlatClut4(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
    DWORD dw;

#ifdef __cplusplus
    for (dw = 0; dw < dwSize; dw++, ((BYTE _huge *&)pb)++)
#else
    for (dw = 0; dw < dwSize; dw++, ((BYTE _huge *)pb)++)
#endif
        *pb = (BYTE)(xlat[*pb & 0x0F] | (xlat[(*pb >> 4) & 0x0F] << 4));
}

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2

static void xlatRle8(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
    BYTE    cnt;
    BYTE    b;
    BYTE _huge *prle = pb;

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

static void xlatRle4(BYTE FAR *pb, DWORD dwSize, BYTE FAR *xlat)
{
}

static void memmove(BYTE _huge *d, BYTE _huge *s, LONG len)
{
    d += len-1;
    s += len-1;

    while (len--)
        *d-- = *s--;
}

//
// MapDib - map the given DIB so it matches the specifed palette
//
void WINAPI DibMapToPalette(PDIB pdib, HPALETTE hpal)
{
    LPBITMAPINFOHEADER  lpbi;
    PALETTEENTRY        pe;
    int                 n;
    int                 nDibColors;
    int                 nPalColors=0;
    BYTE FAR *          lpBits;
    RGBQUAD FAR *       lpRgb;
    BYTE                xlat[256];
    DWORD               SizeImage;

    if (!hpal || !pdib)
        return;

    lpbi   = (LPBITMAPINFOHEADER)pdib;
    lpRgb  = DibColors(pdib);

    GetObject(hpal,sizeof(int),(LPSTR)&nPalColors);
    nDibColors = DibNumColors(pdib);

    if ((SizeImage = lpbi->biSizeImage) == 0)
        SizeImage = DibSizeImage(lpbi);

    //
    //  build a xlat table. from the current DIB colors to the given
    //  palette.
    //
    for (n=0; n<nDibColors; n++)
        xlat[n] = (BYTE)GetNearestPaletteIndex(hpal,RGB(lpRgb[n].rgbRed,lpRgb[n].rgbGreen,lpRgb[n].rgbBlue));

    lpBits = (LPBYTE)DibPtr(lpbi);
    lpbi->biClrUsed = nPalColors;

    //
    // re-size the DIB
    //
    if (nPalColors > nDibColors)
    {
        GlobalReAllocPtr(lpbi, lpbi->biSize + nPalColors*sizeof(RGBQUAD) + SizeImage, 0);
        memmove((BYTE _huge *)DibPtr(lpbi), (BYTE _huge *)lpBits, SizeImage);
        lpBits = (LPBYTE)DibPtr(lpbi);
    }
    else if (nPalColors < nDibColors)
    {
        hmemcpy(DibPtr(lpbi), lpBits, SizeImage);
        GlobalReAllocPtr(lpbi, lpbi->biSize + nPalColors*sizeof(RGBQUAD) + SizeImage, 0);
        lpBits = (LPBYTE)DibPtr(lpbi);
    }

    //
    // translate the DIB bits
    //
    switch (lpbi->biCompression)
    {
        case BI_RLE8:
            xlatRle8(lpBits, SizeImage, xlat);
            break;

        case BI_RLE4:
            xlatRle4(lpBits, SizeImage, xlat);
            break;

        case BI_RGB:
            if (lpbi->biBitCount == 8)
                xlatClut8(lpBits, SizeImage, xlat);
            else
                xlatClut4(lpBits, SizeImage, xlat);
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

    return;
}

void WINAPI MakeIdentityPalette(HPALETTE hpal)
{
    HDC hdc;
    PALETTEENTRY ape[256];
    HPALETTE hpalT;
    int nColors;
    int nCosmic;
    int i;
    HWND hwnd = GetActiveWindow();

    hdc = GetDC(hwnd);

    nColors = GetDeviceCaps(hdc, SIZEPALETTE);
    nCosmic = GetDeviceCaps(hdc, NUMCOLORS);

    if ((GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) && nColors <= 256)
    {
        SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
        SetSystemPaletteUse(hdc, SYSPAL_STATIC);

        hpalT = SelectPalette(hdc, hpal, FALSE);
        RealizePalette(hdc);
        SelectPalette(hdc, hpalT, FALSE);

        GetSystemPaletteEntries(hdc, 0, nColors, ape);

        for (i=0; i<nCosmic/2; i++)
            ape[i].peFlags = 0;

        for (; i<nColors-nCosmic/2; i++)
            ape[i].peFlags = PC_NOCOLLAPSE;

        for (; i<nColors-nCosmic/2; i++)
            ape[i].peFlags = 0;

        ResizePalette(hpal, nColors);
        SetPaletteEntries(hpal, 0, nColors, ape);
    }

    ReleaseDC(hwnd,hdc);
}

/*
 * CopyPalette, makes a copy of a GDI logical palette
 */
HPALETTE WINAPI CopyPalette(HPALETTE hpal)
{
    NPLOGPALETTE ppal;
    int         nNumEntries=0;

    if (!hpal)
        return NULL;

    GetObject(hpal,sizeof(int),(LPSTR)&nNumEntries);

    if (nNumEntries == 0)
        return NULL;

    ppal = (NPLOGPALETTE)LocalAlloc(LPTR,sizeof(LOGPALETTE) +
                nNumEntries * sizeof(PALETTEENTRY));

    if (!ppal)
        return NULL;

    ppal->palVersion    = 0x0300;
    ppal->palNumEntries = nNumEntries;

    GetPaletteEntries(hpal,0,nNumEntries,ppal->palPalEntry);

    hpal = CreatePalette(ppal);

    LocalFree((HANDLE)ppal);
    return hpal;
}


HPBYTE WINAPI GetDIBPixelAddress(PDIB pDIB, UINT x, UINT y)
{
    HPBYTE p;
    long lWidth;

    //
    // make sure it's in range and if not return zero
    //

    if ((x >= (UINT)DIB_WIDTH(pDIB))
    ||  (y >= (UINT)DIB_HEIGHT(pDIB)))
        return NULL;

    //
    // Calculate the scan line storage width
    //

    lWidth = DIB_STORAGEWIDTH(pDIB);

    p = (HPBYTE) DIB_PBITS(pDIB);
    p += (long)(DIB_HEIGHT(pDIB)-y-1) * lWidth + (long)x;

    return p;
}

//
// The the value (color index) of a DIB pixel
//

BYTE WINAPI GetDIBPixelValue(PDIB pDIB, UINT x, UINT y)
{
    HPBYTE p;

    p = GetDIBPixelAddress(pDIB, x, y);

    if (!p) {

        //dprintf1("Out of range pixel coords");
        return 0;
    }

    return *p;
}

//
// Return the Color of a DIB pixel
//

COLORREF WINAPI GetDIBPixelColor(PDIB pDIB, UINT x, UINT y)
{
    LPRGBQUAD pquad = DIB_PCLRTAB(pDIB);
    RGBQUAD quad = pquad[GetDIBPixelValue(pDIB, x, y)];
    
    return RGB(quad.rgbRed, quad.rgbGreen, quad.rgbBlue);
}

int WINAPI DibFindColor(PDIB pdib, COLORREF color)
{
	LPRGBQUAD pColors = DibColors(pdib);
	UINT cColors = DibNumColors(pdib);
	UINT iColor;

	for (iColor = 0 ; iColor < cColors ; iColor++)
	{
		if ((pColors[iColor].rgbRed == GetRValue(color))
		&&  (pColors[iColor].rgbGreen == GetGValue(color))
		&&  (pColors[iColor].rgbBlue == GetBValue(color)))
		{
			return iColor;
		}
	}
	return -1;
}

BOOL WINAPI DibIsRectEmpty(PDIB pdib, LPRECT pRect, BYTE bTransparent)
{
	return (DibCountRectPixels (pdib, pRect, bTransparent) == 0);
}  

BOOL WINAPI DibIsRectFull(PDIB pdib, LPRECT pRect, BYTE bTransparent)
{
	return ((DWORD)(pRect->right - pRect->left) * (DWORD)(pRect->bottom - pRect->top)) == DibCountRectPixels(pdib, pRect, bTransparent);
}

DWORD WINAPI DibCountRectPixels(PDIB pdib, LPRECT pRect, BYTE bTransparent)
{
	LONG count = 0;
	int y;

	for (y = pRect->top ; y < pRect->bottom ; y++)
	{
		LPBYTE p = GetDIBPixelAddress(pdib, pRect->left, y);
		LPBYTE pEnd = p + pRect->right - pRect->left;
		
		while (p < pEnd)
		{
			if (*p++ != bTransparent)
				count++;
		}
	}
	return count;
}

