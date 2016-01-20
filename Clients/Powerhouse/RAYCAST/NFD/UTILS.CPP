/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// UTILS.C - useful functions for WinG Sample Apps
//

#include "stdafx.h"
#include <windowsx.h>
#include "fd.h"
#include "utils.h"

#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

/* flags for _lseek */
#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0


/*
 *  Clear the System Palette so that we can ensure an identity palette 
 *  mapping for fast performance.
 */
void ClearSystemPalette(CWnd *cwnd)
{
struct
{
	WORD			Version;
	WORD			NumberOfEntries;
	PALETTEENTRY	palEntry[256];
} LogicalPalette={0x300,256};
CPalette	CPalApp, *CPalOld;
CDC			*pDCScreen;
int			Counter;
UINT		nSet = 0, nMapped = 0;
  
//*** Reset everything in the system palette to black
for (Counter = 0; Counter < 256; Counter++)
{
	LogicalPalette.palEntry[Counter].peRed = 0;
	LogicalPalette.palEntry[Counter].peGreen = 0;
	LogicalPalette.palEntry[Counter].peBlue = 0;
	LogicalPalette.palEntry[Counter].peFlags = PC_NOCOLLAPSE;	//	PC_RESERVED;	//	PC_NOCOLLAPSE;
}

//*** Create, select, realize, deselect, and delete the palette
	if (CPalApp.CreatePalette((LOGPALETTE *)&LogicalPalette))
	{
		pDCScreen = cwnd->GetDC();
		if (CPalOld = pDCScreen->SelectPalette(&CPalApp,FALSE))
		{
			nMapped = pDCScreen->RealizePalette();
			TRACE("Number of entries mapped in palette = %d\n", nMapped);
			CPalOld = pDCScreen->SelectPalette(CPalOld,FALSE);
			CPalOld->DeleteObject();
		}
		cwnd->ReleaseDC(pDCScreen);
	}

return;
}


/*
 *   Open a DIB file and return a MEMORY DIB, a memory handle containing..
 *
 *   BITMAP INFO    bi
 *   palette data
 *   bits....
 */
PDIB DibOpenFile(LPSTR szFile)
{
HFILE		fh;
DWORD		dwLen;
DWORD		dwBits;
PDIB		pdib;
LPVOID		p;
OFSTRUCT	of;

#define GetCurrentInstance()    (HINSTANCE)SELECTOROF((LPVOID)&of)

    fh = OpenFile(szFile, &of, OF_READ);
    if (fh == -1)
    {
	HRSRC	h;

        h = FindResource(GetCurrentInstance(), szFile, RT_BITMAP);
        if (h) fh = AccessResource(GetCurrentInstance(), h);
    }
    if (fh == -1) return NULL;

    pdib = DibReadBitmapInfo(fh);
    if (!pdib) return NULL;

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
    return pdib;
}


/*
 *  ReadDibBitmapInfo()
 *
 *  Will read a file in DIB format and return a global HANDLE to its
 *  BITMAPINFO.  This function will work with both "old" and "new"
 *  bitmap formats, but will always return a "new" BITMAPINFO.
 */
PDIB DibReadBitmapInfo(HFILE fh)
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

    if (fh == -1) return NULL;

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

    FixBitmapInfo(&bi);

    pdib = (PDIB)GlobalAllocPtr(GMEM_MOVEABLE,(LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
    if (!pdib) return NULL;

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
            RGBQUAD	rgb;

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

    if (bf.bfOffBits != 0L) _llseek(fh,off + bf.bfOffBits,SEEK_SET);

    return pdib;
}

/*
 *  DibCreate(bits, dx, dy)
 *
 *  Creates a new packed DIB with the given dimensions and the
 *  given number of bits per pixel
 */
PDIB DibCreate(int bits, int dx, int dy)
{
LPBITMAPINFOHEADER lpbi ;
DWORD       dwSizeImage;
int         i;
DWORD FAR  *pdw;

    dwSizeImage = dy*(DWORD)((dx*bits/8+3)&~3);

    lpbi = (PDIB)GlobalAllocPtr(GHND,sizeof(BITMAPINFOHEADER)+dwSizeImage + 1024);

    if (lpbi == NULL) return NULL;

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

    if (bits == 4) lpbi->biClrUsed = 16;

    else if (bits == 8) lpbi->biClrUsed = 256;

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
