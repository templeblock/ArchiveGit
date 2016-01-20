/*[]=====================================================================[]*/
/*[]     LeadTools Run Time Library - Version 4.5                        []*/
/*[]                                                                     []*/
/*[]                                                                     []*/
/*[]     Copyright (c) 1990, 1994 by LEAD Technologies, Inc.             []*/
/*[]     All Rights Reserved.                                            []*/
/*[]=====================================================================[]*/

#if !defined(_BITMAP_H)
#define _BITMAP_H

#ifndef __BORLANDC__
#pragma pack(4)
#else
#pragma option -a4
#endif                          /* __BORLANDC__ */

#include <stdio.h>              /* It has the declartion of FILE structure */

/*=========================================================================
                   LEAD System Independent Type Definition
  =========================================================================*/

typedef TCHAR L_CHAR;           /* signed 8-bit integer */
typedef unsigned char L_UCHAR;  /* unsigned 8-bit integer */
typedef unsigned char L_BYTE;   /* unsigned 8-bit integer */

typedef short L_INT16;          /* signed 16-bit integer */
typedef unsigned short L_UINT16;/* unsigned 16-bit integer */
typedef unsigned short L_WORD;  /* unsigned 16-bit integer */

typedef long L_INT32;           /* signed 32-bit integer */
typedef unsigned long L_UINT32; /* unsigned 32-bit integer */
typedef unsigned long L_DWORD;  /* unsigned 32-bit integer */

typedef int L_INT;              /* signed integer 16-bits or larger */
typedef unsigned L_UINT;        /* unsigned integer 16-bits or larger */
typedef long L_LONG;            /* signed integer 32-bits or larger */
typedef long L_ULONG;           /* unsigned integer 32-bits or larger */

typedef void L_VOID;
typedef int L_BOOL;             /* Boolean */
typedef unsigned int L_BITFIELD;/* General bitfield type */
#define L_PASCAL
#define L_FAR
#define L_HUGE
#define L_EXPORT

#define EXT_FUNCTION WINAPI

#undef HALFTONE
#undef ERROR_FILE_NOT_FOUND
#undef ERROR_NOT_SUPPORTED
#undef ERROR_INVALID_DATA

/*=========================================================================
                           LEAD Tools definitions
  =========================================================================*/

 /* Screen locations               */
#define TOP_LEFT                  0 /* The top left part of the screen */
#define TOP_RIGHT                 1 /* The top right part of the screen */
#define BOTTOM_LEFT               2 /* The bottom left part of the screen */
#define BOTTOM_RIGHT              3 /* The bottom right part of the screen */

 /* Output Type                    */
#define OUTPUT_SCREEN             1 /* Output on EVGA, VGA, or EGA    */
#define OUTPUT_TARGA              2 /* Output to TAGRA                */
#define OUTPUT_BITMAP             4 /* Output to the BitMap           */

 /* Memory allocation mechanism    */
#define TYPE_CONV                 1 /* Use conventional memory        */
#define TYPE_NOCONV               2 /* Do not use conventional memory */
#define TYPE_EXP                  4 /* Use EMS (if possible)          */
#define TYPE_NOEXP               16 /* Do not use EMS                 */
#define TYPE_XMS                 32 /* Use XMS (if possible)          */
#define TYPE_NOXMS               64 /* Do not use XMS                 */
#define TYPE_DISK               128 /* Use Disk (if possible)         */
#define TYPE_NODISK             256 /* Do not use disk                */
#define KEEP_ON_DISK              8 /* Internal Use                   */

 /* Compression Methoed            */
#define COMP_NONE               0   /* None                           */
#define COMP_LZW                1   /* LZW Method                     */
#define COMP_CCITT              3   /* TIFF  CCITT                    */
#define COMP_CCITTG3_FAX1D      4   /* CCITT Group3 one dimensional   */
#define COMP_CCITTG3_FAX2D      5   /* CCITT Group3 two dimensional   */
#define COMP_CCITTG4_FAX        6   /* CCITT Group4 two dimensional   */

#define NONE COMP_NONE
#define LZW COMP_LZW
 /* BMP Types                      */
#define TYPE_WIN                  1 /* BMP windows version            */
#define TYPE_OS2                  2 /* BMP OS/2 version               */

#define HT_PRINT                  0
#define HT_VIEW                   1

#define NO_DITHERING              0 /* Dithering Methods              */
#define FLOYD_STEIN_DITHERING     1
#define STUCKI_DITHERING          2
#define BURKES_DITHERING          3
#define SIERRA_DITHERING          4
#define STEVENSON_ARCE_DITHERING  5
#define JARVIS_DITHERING          6
#define ORDERED_DITHERING         7
#define LAST_DITHERING          	 7

 /* Bitmap data colors order      */
#define ORDER_RGB                 0 /* RGB ordering                  */
#define ORDER_BGR                 1 /* BRG ordering                  */

 /* Video type installed         */
#define VIDEO_SVGA                1 /* Super VGA installed          */
#define VIDEO_VGA                 2 /* VGA installed                */
#define VIDEO_EGA                 3 /* EGA installed                */

 /* Video Mode supported          */
#define SIZE_320X200              0 /* 320 x 200                     */
#define SIZE_640X350              1 /* 640 x 350                     */
#define SIZE_640X480              2 /* 640 x 480                     */
#define SIZE_800X600              3 /* 800 x 600                     */
#define SIZE_1024X768             4 /* 1024 x 768                    */
#define SIZE_1024X1024            5 /* 1024 x 1024                   */
#define SIZE_1280X1024            6 /* 1280 x 1024                   */


 /* List of file type supported   */
#define FILE_PCX                  1 /* ZSoft PCX                     */
#define FILE_GIF                  2 /* CompuServe GIF                */
#define FILE_TIF                  3 /* Tagged Image File Format      */
#define FILE_TGA                  4 /* Targa                         */
#define FILE_CMP                  5 /* LEAD CMP                      */
#define FILE_BMP                  6 /* Windows BMP                   */
#define FROM_BUFFER               7 /* Internal use                  */
#define FILE_BITMAP               9 /* Internal use                  */
#define FILE_JFIF                10 /* Jpeg File Interchange Format  */
#define FILE_JTIF                11 /* Jpeg Tag Image File Format    */
#define FILE_BIN                 12 /* Internal use                  */
#define FILE_HANDLE              13 /* Internal use                  */
#define FILE_OS2                 14 /* OS/2 BMP                      */
#define FILE_WMF                 15 /* Windows Meta File             */
#define FILE_EPS                 16 /* Encapsulated Post Script      */
#define FILE_TIFLZW              17 /* Tagged Image File Format with  LZW
                                       compression */
#define FILE_CALS               50
#define FILE_MAC                51
#define FILE_IMG                52
#define FILE_MSP                53
#define FILE_WPG                54
#define FILE_RAS                55
#define FILE_PCT                56
#define FILE_PCD                57
#define FILE_DXF                58
#define FILE_AVI                59
#define FILE_WAV                60
#define FILE_FLI                61
#define FILE_CGM                62
#define FILE_EPSTIFF            63
#define FILE_EPSWMF             64
#define FILE_CMPNOLOSS          65

#define FILE_LEAD                20 /* LEAD  Proprietary             */
#define FILE_LEAD1JFIF           21 /* JPEG  4:1:1                   */
#define FILE_LEAD1JTIF           22 /* JPEG  4:1:1                   */
#define FILE_LEAD2JFIF           23 /* JPEG  4:2:2                   */
#define FILE_LEAD2JTIF           24 /* JPEG  4:2:2                   */
#define FILE_CCITT               25 /* TIFF  CCITT                   */
#define FILE_LEAD1BIT            26 /* LEAD 1 bit, lossless compression */
#define FILE_CCITT_GROUP3_1DIM   27 /* CCITT Group3 one dimension    */
#define FILE_CCITT_GROUP3_2DIM   28 /* CCITT Group3 two dimensions   */
#define FILE_CCITT_GROUP4        29 /* CCITT Group4 two dimensions   */
#define FILE_LEAD_NOLOSS         30 /* LEAD  Proprietary Lossless    */

 /* Compression Type              */
#define LEAD                      0 /* LEAD  Proprietary             */
#define JFIF                      1 /* JPEG  4:4:4                   */
#define JTIF                      2 /* JPEG  4:4:4                   */
#define LEAD1JFIF                 3 /* JPEG  4:1:1                   */
#define LEAD1JTIF                 4 /* JPEG  4:1:1                   */
#define LEAD2JFIF                 5 /* JPEG  4:2:2                   */
#define LEAD2JTIF                 6 /* JPEG  4:2:2                   */

#define LEAD_0                    1 /* LEAD 1 bit, lossless compression */
#define LEAD_1                    2 /* LEAD 1 bit, excellent compression */

#define TIFF_CCITT                3 /* TIFF  CCITT                   */
#define TIFF_CCITTG3_FAX1D        4 /* CCITT Group3 one dimensional  */
#define TIFF_CCITTG3_FAX2D        5 /* CCITT Group3 two dimensional  */
#define TIFF_CCITTG4_FAX          6 /* CCITT Group4 two dimensional  */

 /* Check flag for Escape key     */
#define CHECK                     1 /* Check for Escape key          */
#define NO_CHECK                  0 /* Don't Check for Escape key    */

 /* TARGA File Type               */
#define TGA8                      0 /* TARGA 8 bit                   */
#define TGA16                     1 /* TARGA 16 bit                  */
#define TGA24                     2 /* TARGA 24 bit                  */
#define TGA32                     3 /* TARGA 32 bit                  */

 /* TIFF File Type                */
#define TIF1                    130 /* TIFF 1 bit                    */
#define TIF8                      7 /* TIFF 8 bit                    */
#define TIF16                    16 /* TIFF 16 bit                   */
#define TIF24                     8 /* TIFF 24 bit                   */

 /* TIFF + LZW File Type          */
#define TIFLZW1                 131 /* TIFF LZW 1 bit                */
#define TIFLZW8                  30 /* TIFF LZW 8 bit                */
#define TIFLZW16                 31 /* TIFF LZW 16 bit               */
#define TIFLZW24                 32 /* TIFF LZW 24 bit               */

 /* 1 Bit Compressed File Types   */
#define CCITT                   140 /* CCITT                         */
#define TIF_G3_1D               141 /* CCITT Group3 one dimensional  */
#define TIF_G3_2D               142 /* CCITT Group3 two dimensional  */
#define TIF_G4                  143 /* CCITT Group4 two dimensional  */

 /* Unique 1 Bit File Types       */
#define CALS                    150 /* CALS 1 bit                    */
#define MAC                     151 /* MacPaint 1 bit                */
#define IMG1                    152 /* GEM Image 1 bit               */
#define MSP                     153 /* MicroSoft Paint 1 bit         */

 /* WPG file type                 */
#define WPG1                    160 /* WordPerfect WPG 1 bit         */
#define WPG4                    161 /* WordPerfect WPG 4 bit         */
#define WPG8                    162 /* WordPerfect WPG 8 bit         */

 /* Raster RAS file type          */
#define RAS1                    170 /* Raster RAS 1 bit              */
#define RAS4                    171 /* Raster RAS 4 bit              */
#define RAS8                    172 /* Raster RAS 8 bit              */
#define RAS24                   173 /* Raster RAS 24 bit             */
#define RAS32                   174 /* Raster RAS 32 bit             */

 /* Mac PICTURE file type         */
#define PCT1                    180 /* PCT 1 bit                     */
#define PCT4                    181 /* PCT 4 bit                     */
#define PCT8                    182 /* PCT 8 bit                     */
#define PCT24                   183 /* PCT 24 bit                    */

 /* BMP file type                 */
#define BMP1                    132 /* Windows BMP 1 bit             */
#define BMP4                      4 /* Windows BMP 4 bit             */
#define BMP8                      5 /* Windows BMP 8 bit             */
#define BMP24                    24 /* Windows BMP 24 bit            */
#define PM1                     133 /* OS/2 BMP 1 bit                */
#define PM4                     104 /* OS/2 BMP 4 bit                */
#define PM8                     108 /* OS/2 BMP 8 bit                */
#define PM24                    124 /* OS/2 BMP 24 bit               */

 /* PCX file type                 */
#define PCX1                    134 /* PCX 1 bit                     */
#define PCX4                    135 /* PCX 4 bit                     */
#define PCX8                      9 /* PCX 8 bit                     */
#define PCX24                    11 /* PCX 24 bit                    */

 /* GIF file type                 */
#define GIF8                     10 /* GIF 8 bit                     */

 /* WMF file type                 */
#define WMF1                     13 /* WMF 1 bit                     */
#define WMF8                     14 /* WMF 8 bit                     */
#define WMF24                    15 /* WMF 24 bit                    */

 /* EPS file type                 */
#define EPS                      12 /* EPS 8 bit                     */

 /* Video Board                   */
#define TARGA_BOARD               0 /* TARGA Board                   */
#define VGA_BOARD                 1 /* VGA  Board                    */
#define VGA320X200                2 /* VGA  320 x 200                */
#define VGA640X480                3 /* VGA  640 x 480                */
#define VGA800X600                4 /* VGA  800 x 600                */
#define VGA1024X768               5 /* VGA  1024 x 768               */
#define VGA1280X768               6 /* VGA  1280 x 768               */

 /* Bit information               */
#define BIT1                      1 /* 1 Bit                         */
#define BIT4                      4 /* 4 Bit                         */
#define BIT24                     24   /* 24 Bit                        */
#define BIT8                      8 /* 8 Bit                         */
#define BIT16                     16   /* 16 Bit                        */
#define BIT32                     32   /* 32 Bit                        */
#define PREFER8BIT                50   /* 8 Bit                         */

 /* Preset Q Factors              */
#define PQ1                      -1 /* Perfect quality option 1      */
#define PQ2                      -2 /* Perfect quality option 2      */
#define QFS                      -3 /* Quality far more important than size */
#define QMS                      -4 /* Quality more important than size */
#define QS                       -5 /* Quality and size are equally important */
#define SQS                      -6 /* Size more important than quality
                                       -Sharp */
#define SQT                      -7 /* Size more important than quality -
                                       Less Tilling */
#define MCQ                      -8 /* Maximum Compression, keeping quality
                                       as good as possible */
#define MC                       -9 /* Maximum compression           */

 /* Font Size                     */
#define FONT8X8                   0 /* Font Size 8 * 8               */
#define FONT8X14                  1 /* Font Size 8 * 14              */
#define FONT8X16                  2 /* Font Size 8 * 16              */

 /* Font direction                */
#define HORIZONTAL                0 /* Print characters horizontaly  */
#define VERTICAL                  1 /* Print characters horizontaly  */

 /* Text colors                   */
#define KEEPBACKGROUND           -1 /* print text transpernatly      */
#define BLACK		                0 /* Black                         */
#define BLUE		               13 /* Blue                          */
#define GREEN		               11 /* Green                         */
#define CYAN		               15 /* Cyan                          */
#define RED                       1 /* Red                           */
#define MAGENTA                  14 /* Magenta                       */
#define BROWN                    17 /* Brown                         */
#define YELLOW		               12 /* Yellow                        */
#define GRAY                      7 /* Gray                          */
#define LIGHTBLUE               252 /* Light Blue                    */
#define LIGHTGREEN              250 /* Light Green                   */
#define LIGHTCYAN               254 /* Light Cyan                    */
#define LIGHTRED                249 /* Light Red                     */
#define LIGHTMAGENTA            253 /* Light Magenta                 */
#define LIGHTYELLOW             251 /* Light Yellow                  */
#define WHITE                   255 /* White                         */

 /* TARGA and TARGA+ resolution  modes */
#define Interlaced512x400x16      0 /* Interlace 512 * 400 * 16      */
#define Noninterlaced512x400x16   1 /* Non Interlace 512 * 400 * 16  */
#define Interlaced512x400x32      2 /* Interlace 512 * 400 * 32      */
#define Noninterlaced512x400x32   3 /* Non Interlace 512 * 400 * 32  */
#define Interlaced512x486x16      4 /* Interlace 512 * 486 * 16      */
#define Noninterlaced512x486x16   5 /* Non Interlace 512 * 486 * 16  */
#define Interlaced512x486x32      6 /* Interlace 512 * 486 * 32      */
#define Noninterlaced512x486x32   7 /* Non Interlace 512 * 486 * 32  */
#define Interlaced640x480x16      8 /* Interlace 640 * 480 * 16      */
#define Noninterlaced640x480x16   9 /* Non Interlace 640 * 480 * 16  */
#define SquareInterlaced640x486x16    10  /* Interlace 640 * 486 * 16    */
#define SquareNoninterlaced640x486x16 11  /* Non Interlace 640 * 486 * 16 */
#define Interlaced720x486x16     12 /* Interlace 720 * 486 * 16      */
#define Noninterlaced720x486x16  13 /* Non Interlace 720 * 486 * 16  */
#define Interlaced756x486x16     14 /* Interlace 756 * 486 * 16      */
#define Noninterlaced756x486x16  15 /* Non Interlace 756 * 486 * 16  */
#define Interlaced800x600x16     16 /* Interlace 800 * 486 * 16      */
#define Interlaced1024x768x16    17 /* Interlace 1024 * 768 * 16     */
#define BackToVGA                18 /* Switch to VGA mode            */

 /* Video camera input options    */
#define VideoInputRGB             0 /* Input as RGB                  */
#define VideoInputComposite       1 /* Input as composite            */
#define VideoInputSVideo          2 /* Input as S video              */

 /* Printer resolution            */
#define DPI600                  600 /* 600 Dot per inch              */
#define DPI300                  300 /* 300 Dot per inch              */
#define DPI150                  150 /* 150 Dot per inch              */
#define DPI100                  100 /* 100 Dot per inch              */
#define DPI75                    75 /* 75 Dot per inch              */

 /* LEAD CMP Palette information  */
#define SaveVGAPalette            1 /* Save the palette in CMP file  */
#define NoVGAPalette              0 /* Don't Save the palette in CMP */

 /* Targa 32 alpha channel data   */
#define Save                      1 /* Save alpha channel data       */
#define NoSave                    0 /* Don't save alpha channel data */


 /* Flags for L_CopyToClipboardExt */
#define COPY2CB_EMPTY 0x0001    /* Empty the clipboard */
#define COPY2CB_DIB   0x0002    /* Copy DIB data to the clipboard */
#define COPY2CB_DDB   0x0004    /* Copy DDB data to the clipboard */
#define COPY2CB_PALETTE 0x0008  /* Copy palette data to the clipboard */

 /* Flags for L_LoadFile */
#define LOADFILE_ALLOCATE     0x0001   /* Allocate image memory */
#define LOADFILE_STORE        0x0002   /* Auto-store image lines */
#define LOADFILE_FIXEDPALETTE 0x0004   /* Allow fixed palettes only */
#define LOADFILE_NOINTERLACE  0x0008   /* Don't send interlaced lines */

 /* Flags for L_SaveFile */
#define SAVEFILE_FIXEDPALETTE      1   /* Save with Fixed Palette */
#define SAVEFILE_OPTIMIZEDPALETTE  2   /* Save with Bitmap's Palette */
#define SAVEFILE_MULTIPAGE         4   /* Save as multipage */

 /* Flags for L_TwainAcquireExt */
#define TWAIN_SHOWUI               0x0001 /* Show the User Interface */
#define TWAIN_SHOWSTATUS           0x0002 /* Show the Status Indicator, if it
                                             is selectable */

 /* Flags for L_ColorResolution   */
#define CRF_FIXEDPALETTE         1  /* versus custom palette */
#define CRF_OPTIMIZEDPALETTE     2  /* versus fixed palette */
#define CRF_BYTEORDERBGR         4  /* versus RGB */
#define CRF_BYTEORDERRGB         0  /* versus BGR */
#define CRF_IDENTITYPALETTE      8  /* versus ALL 256 colors */
#define CRF_USERPALETTE         16  /* user generated palette */

 /* Operation No. used by L_ProcessBitmap */
#define ROTATE                  0   /* Rotate the bitmap           */
#define FLIP                    1   /* Flip the bitmap             */
#define RESIZE                  2   /* Resize the bitmap           */
#define REVERSE                 3   /* Reverse the bitmap          */
#define CHANGEINTENSITY         4   /* Change the bitmap intensity */
#define DITHER                  5   /* Dither the bitmap           */
#define OPTIMIZE                6   /* Optimize the bitmap         */
#define GRAYSCALE               7   /* Gray scale the bitmap       */
#define HALFTONE                8   /* Halftone the bitmap         */

/*=========================================================================
                            LEAD Window class
  =========================================================================*/

#define L_BM_LOAD       (WM_USER + 0)
#define L_BM_SAVE       (WM_USER + 1)
#define L_BM_FREE       (WM_USER + 2)
#define L_BM_FLIP       (WM_USER + 3)
#define L_BM_ROTATE     (WM_USER + 4)
#define L_BM_INTENSITY  (WM_USER + 5)
#define L_BM_REVERSE    (WM_USER + 6)
#define L_BM_INFO       (WM_USER + 7)
#define L_BM_GETPALETTE (WM_USER + 8)
#define L_BM_SETSTYLE   (WM_USER + 9)

#define L_BM_GETBITMAP  (WM_USER + 10)
#define L_BM_SETBITMAP  (WM_USER + 11)

#define L_BM_AVERAGE    (WM_USER + 12)
#define L_BM_MEDIAN     (WM_USER + 13)
#define L_BM_CONTRAST   (WM_USER + 14)
#define L_BM_HUE		   (WM_USER + 15)
#define L_BM_SATURATION (WM_USER + 16)
#define L_BM_GRAYSCALE  (WM_USER + 17)
#define L_BM_HALFTONE   (WM_USER + 18)
#define L_BM_HISTOEQ    (WM_USER + 19)
#define L_BM_INVERT     (WM_USER + 20)
#define L_BM_SPATIALFLT (WM_USER + 21)
#define L_BM_MOSAIC     (WM_USER + 22)
#define L_BM_POSTERIZE  (WM_USER + 23)
#define L_BM_SETZOOM    (WM_USER + 24)
#define L_BM_RESIZE     (WM_USER + 25)
#define L_BM_COLORRES   (WM_USER + 26)
#define L_BM_SHARPEN    (WM_USER + 27)
#define L_BM_SETIMAGERECT (WM_USER + 28)
#define L_BM_COPY       (WM_USER + 29)
#define L_BM_PASTE      (WM_USER + 30)
#define L_BM_COPYBITMAP (WM_USER + 31)
#define L_BM_TWAINSELECT (WM_USER + 32)
#define L_BM_TWAINACQUIRE (WM_USER + 33)

#define L_BN_CHANGE     0
#define L_BN_UPDATE     1
#define L_BN_HSCROLL    2
#define L_BN_VSCROLL    3
#define L_BN_SETFOCUS   4
#define L_BN_KILLFOCUS   5
#define L_BN_CLICKED    6
#define L_BN_DBLCLK     7
#define L_BN_PAINTED    8

#define L_BS_SIZEIMAGE  0x00000001  /* Specifies that the image should be
                                       resized to fit the window. */
#define L_BS_ORDERRGB   0x00000002  /* Specifies that a high color image
                                       should be loaded in reverse (RGB)
                                       video order. */
#define L_BS_THUMBTRACK 0x00000004  /* Specifies that the window tracks the
                                       thumb position when using scroll bars */
#define L_BS_SIZEWINDOW  0x00000008 /* Specifies that the window should be
                                       resized to the image. */

#define L_BS_CENTER     0x00000010  /* Specifies that the image should be
                                       centered */

#define L_BS_FIXEDPALETTE  0x00000020  /* Specifies that the image should use
                                          the fixed palette */

#define L_BS_BKGNDPALETTE  0x00000040  /* Specifies that the palette should
                                          be forced to the background */

typedef struct tagLEADBITMAPCOLORRES
{
   L_INT StructSize;
   L_INT BitsPerPixel;
   L_INT DitherType;
   L_UINT32 Flags;
} LEADBITMAPCOLORRES, far * pLEADBITMAPCOLORRES;

typedef struct tagLEADBITMAPLOAD
{
   L_INT StructSize;
   L_CHAR Name[80];
} LEADBITMAPLOAD, far * pLEADBITMAPLOAD;


typedef struct tagLEADBITMAPSAVE
{
   L_INT StructSize;
   L_CHAR Name[80];
   L_INT Type;
   L_INT QFactor;
   L_INT SavePalette;
} LEADBITMAPSAVE, far * pLEADBITMAPSAVE;


typedef struct tagLEADBITMAPINFO
{
   L_INT StructSize;
   L_INT Width;
   L_INT Height;
   L_INT Pixels;
} LEADBITMAPINFO, far * pLEADBITMAPINFO;


enum
{
   SFILE_NULL,                  /* 0    */
   SFILE_TGA8,                  /* 1    */
   SFILE_TGA16,                 /* 2    */
   SFILE_TGA24,                 /* 3    */
   SFILE_TGA32,                 /* 4    */
   SFILE_TIF1,                  /* 5    */
   SFILE_TIFLZW1,               /* 6    */
   SFILE_CCITT,                 /* 7    */
   SFILE_LEAD1BIT,              /* 8    */
   SFILE_TIF_G3_1D,             /* 9    */
   SFILE_TIF_G3_2D,             /* 10   */
   SFILE_TIF_G4,                /* 11   */
   SFILE_TIF8,                  /* 12   */
   SFILE_TIF16,                 /* 13   */
   SFILE_TIF24,                 /* 14   */
   SFILE_TIFLZW8,               /* 15   */
   SFILE_TIFLZW16,              /* 16   */
   SFILE_TIFLZW24,              /* 17   */
   SFILE_CALS_G4,               /* 18   */
   SFILE_MAC,                   /* 19   */
   SFILE_IMG1,                  /* 20   */
   SFILE_MSP,                   /* 21   */
   SFILE_WPG1,                  /* 22   */
   SFILE_WPG4,                  /* 23   */
   SFILE_WPG8,                  /* 24   */
   SFILE_RAS1,                  /* 25   */
   SFILE_RAS4,                  /* 26   */
   SFILE_RAS8,                  /* 27   */
   SFILE_RAS24,                 /* 28   */
   SFILE_RAS32,                 /* 29   */
   SFILE_PCT1,                  /* 30   */
   SFILE_PCT4,                  /* 31   */
   SFILE_PCT8,                  /* 32   */
   SFILE_PCT24,                 /* 33   */
   SFILE_WBMP1,                 /* 34   */
   SFILE_WBMP4,                 /* 35   */
   SFILE_WBMP8,                 /* 36   */
   SFILE_WBMP24,                /* 37   */
   SFILE_OBMP1,                 /* 38   */
   SFILE_OBMP4,                 /* 39   */
   SFILE_OBMP8,                 /* 40   */
   SFILE_OBMP24,                /* 41   */
   SFILE_PCX1,                  /* 42   */
   SFILE_PCX4,                  /* 43   */
   SFILE_PCX8,                  /* 44   */
   SFILE_PCX24,                 /* 45   */
   SFILE_GIF,                   /* 46   */
   SFILE_LEAD,                  /* 47   */
   SFILE_LEAD8,                 /* 48   */
   SFILE_LEAD1JFIF,             /* 49   */
   SFILE_LEAD2JFIF,             /* 50   */
   SFILE_LEAD1JTIF,             /* 51   */
   SFILE_LEAD2JTIF,             /* 52   */
   SFILE_JFIF,                  /* 53   */
   SFILE_JFIF8,                 /* 54   */
   SFILE_JTIF,                  /* 55   */
   SFILE_WMF8,                  /* 56   */
   SFILE_WMF24,                 /* 57   */
   SFILE_EPS,                   /* 58   */
   SFILE_LEADNOLOSS             /* 59   */
};



/*=========================================================================
                       Helpful structure definitons
  =========================================================================*/

/*-----------------(Structure to hold 16 bit RGB value)--------------------*/

typedef struct Struct_Pixel16_Data
{
   L_BITFIELD BlueBits:5;
   L_BITFIELD GreenBits:5;
   L_BITFIELD RedBits:5;
   L_BITFIELD ChannelBits:1;
} PIXEL16DATA, L_FAR * pPIXEL16DATA;
/*=========================================================================
                       Definitions for Internal Use
  =========================================================================*/

typedef L_INT ((L_FAR * STARTDIRECTFUNCTION) (L_INT, L_VOID L_FAR *));
typedef L_INT ((L_FAR * READDIRECTFUNCTION) (L_INT, L_INT32, L_UCHAR L_FAR *, L_INT, L_VOID L_FAR *));
typedef L_INT ((L_FAR * WRITEDIRECTFUNCTION) (L_INT, L_INT32, L_UCHAR L_FAR *, L_INT, L_VOID L_FAR *));
typedef L_INT ((L_FAR * ENDDIRECTFUNCTION) (L_INT, L_VOID L_FAR *));
/*=========================================================================
               Structure to be used for Resizing the data
  =========================================================================*/

typedef struct Struct_Resize_Data
{
   L_INT OldWidth;
   L_INT OldHeight;
   L_INT NewWidth;
   L_INT NewHeight;
   L_INT XIncrease;
   L_INT YIncrease;
   L_CHAR XFactor[256];
   L_CHAR YFactor[256];
} RESIZEDATA, L_FAR * pRESIZEDATA;
/*=========================================================================
                          LEAD Bitmap Structure
  =========================================================================*/

typedef struct struct_LEAD_Bitmap
{
   union
   {
      struct struct_window_memory   /* Structure for tracking    */
      {                         /* bitmap memory under windows. */
         HANDLE hData;          /* Handle to the bitmap memory */
         L_UCHAR L_HUGE *pData; /* Huge pointer to locked memory */
      } Windows;

      struct struct_file_memory /* Structure to track the    */
      {                         /* bitmap allocated in disk  */
         L_INT fd;              /* File handle               */
         L_CHAR L_FAR *Filename;/* Pointer to the temp file name */
         L_INT BuffDirty;       /* Flag that indicate the internal buffer has
                                   data */
         HANDLE hDiskBuffer;    /* Handle to the internal file buffer */
         L_UCHAR L_FAR *pDiskBuffer;   /* Pointer to the internal file buffer */
         L_INT32 BufferPos;     /* Pointer to the current location of the
                                   buffer */
         L_UINT BufferSize;     /* The size of the internal buffer */
         L_UINT BufferBytes;
         L_UINT BufferCount;
         L_INT32 BufferStart;
      } File;
   } Addr;
   L_INT Type;                  /* Memory type           */
   L_INT Width;                 /* Image width           */
   L_INT Height;                /* Image height          */
   L_INT BitsPerPixel;          /* Number of bits per pixel */
   L_UINT BytesPerLine;         /* Bytes per line, this is rounded by four */
   L_UINT32 CurrentPos;         /* Internal use          */
   L_INT CurrentRow;            /* Used to track the rows inserted in the
                                   bitmap */
   L_INT XOffset;               /* The X offset in the bitmap */
   L_INT YOffset;               /* The Y offset in the bitmap */
   struct struct_bitmap_flags
   {
      L_BITFIELD Allocated:1;   /* Is the bitmap allocated */
      L_BITFIELD ConventionalMemory:1; /* Is it allocated in conventional
                                          memory */
      L_BITFIELD ExpandedMemory:1;  /* Is it allocated in expanded memory */
      L_BITFIELD ExtendedMemory:1;  /* Is it allocated in extended memory */
      L_BITFIELD DiskMemory:1;  /* Is it allocated in the disk */
      L_BITFIELD ReadOffDisk:1; /* Internal use       */
      L_BITFIELD LockedMemory:1;/* Is memory locked in windows */
      L_BITFIELD Interlaced:1;  /* Internal use */
      L_BITFIELD GrayScale:1;   /* Is the bitmap gray scale */
      L_BITFIELD WasOptimized:1;/* Is it optimized */
      L_BITFIELD FixedPalette:1;/* Do we use fixed palette */
      L_BITFIELD ImageHasTruePalette:1;   /* Is the image has true palette */
      L_BITFIELD Resize:1;      /* Internal use */
      L_BITFIELD ReadWriteDirect:1; /* Internal use */
      L_BITFIELD PaletteIndexes:1;  /* Internal use */
      L_BITFIELD Dither24:1;    /* if this is set to TRUE, and the image to
                                   be decompressed is requested as 24 bit,
                                   the data will be error diffused, for a
                                   smoother display. */
   } Flags;
   L_INT nColors;               /* No. of colors the bitmap has */

   BITMAPINFOHEADER BitmapInfo; /* Independent Bitmap Info */
   RGBQUAD pPalette[256];       /* Array of the RGB Palette */
   HPALETTE hPalette;           /* Handle to a palette */
   HPALETTE hFixedPalette;      /* Handle to fixed palette */
   RECT BitmapRect;             /* Internal use */

   L_INT ViewPerspective;       /* Is TOP_LEFT, BOTTOM_RIGHT, .. */
   L_INT Order;                 /* Is it ORDER_RGB, ORDER_BGR */
   L_UINT32 Size;               /* Size of the bitmap  */
   L_CHAR L_FAR *pName;

   L_VOID L_FAR *pDitherData;   /* Internal use        */
   L_INT DitheringMethod;       /* Which dithering method used */
   L_INT ErrorLines[8];         /* Internal use        */
   L_INT DitherRow;             /* Internal use        */

   STARTDIRECTFUNCTION StartImageRead; /* Internal use        */
   READDIRECTFUNCTION ReadImageBuffer; /* Internal use        */
   WRITEDIRECTFUNCTION WriteImageBuffer;  /* Internal use        */
   ENDDIRECTFUNCTION EndImageRead;  /* Internal use        */
   L_VOID L_FAR *pDirectPacket; /* Internal use        */
   L_INT ReadBytes;             /* Internal use        */
   L_INT XResolution;
   L_INT YResolution;
   L_INT ResolutionUnit;
   L_INT AspectRatio;
   L_INT GifVirtualWidth;
   L_INT GifVirtualHeight;
} BITMAPHANDLE, L_FAR * pBITMAPHANDLE;
/*=========================================================================
                    File Info Structure used by L_FileInfo
  =========================================================================*/

typedef struct struct_file_information
{
   L_CHAR Format;               /* File format: FILE_PCX, .. */
   L_CHAR Name[13];             /* File name include the Ext */
   L_INT Width;                 /* Image Width              */
   L_INT Height;                /* Image height             */
   L_INT BitsPerPixel;          /* Bits per pixel           */
   L_INT32 SizeDisk;            /* Available free disk size */
   L_INT32 SizeMem;             /* Available free memory size */
   L_CHAR Compression[20];      /* Compression name method  */
   L_INT ViewPerspective;       /* Image view prespective   */
   L_INT Order;                 /* RGB order                */
   L_INT TotalPage;
   L_INT XResolution;
   L_INT YResolution;
   L_INT ResolutionUnit;
   L_INT AspectRatio;
   L_INT32 HeaderSize;          /* Size of the file header */
} FILEINFO, L_FAR * pFILEINFO;


/*=========================================================================
                  Process Structure used by L_ProcessBitmap
  =========================================================================*/

typedef struct tagPROCESSSTRUCT
{
   L_INT nDegrees;              /* Rotate nDegrees */
   L_INT nNewWidth;             /* Resize to nNewWidth */
   L_INT nNewHeight;            /* Resize to nNewHeight */
   L_INT nIntensityChange;      /* Lignten or darken the bitmap */
   L_INT nResizeWindowToFitBitmap;  /* When rotate clip or not the image */
   L_INT nDitherToBits;         /* Dither or optimize to nBits */
} PROCESSSTRUCT;
typedef PROCESSSTRUCT L_FAR *LPPROCESSSTRUCT;
/*=========================================================================
                  Lossless callback prototype
  =========================================================================*/
typedef L_INT (L_FAR * CDATACALLBACK)
 (
   L_UCHAR L_FAR * lpBufferForData,
   L_UINT nNumOfBytes,
   L_VOID L_FAR * lpUserData
);
/*=========================================================================
                  Photo CD info structure
  =========================================================================*/
#define L_PCD_BASE_OVER_64   0  /* 64 x   96 */
#define L_PCD_BASE_OVER_16   1  /* 128 x  192 */
#define L_PCD_BASE_OVER_4    2  /* 256 x  384 */
#define L_PCD_BASE           3  /* 512 x  768 */
#define L_PCD_4BASE          4  /* 1024 x 1536 */
#define L_PCD_16BASE         5  /* 2048 x 3072 */

typedef struct pcdinfo
{
   L_INT resolution[6];
} PCDINFO, L_FAR * pPCDINFO;
/*=========================================================================
                  SetTIFFOption structure
  =========================================================================*/
typedef struct Struct_TIF_TAG
{
   L_UINT16 tag_id;
   L_UINT16 type;
   L_UINT32 count;
   L_INT32 lData;
} TIFF_TAG, L_FAR * pTIFF_TAG;

typedef struct _EXTFILEOPTION
{
   L_INT XResolution;
   L_INT YResolution;
   L_INT ResolutionUnit;
   L_INT AspectRatio;
   L_INT PageNumber;
   L_INT ExtraTagNumber;
   TIFF_TAG ExtraTag[10];
} EXTFILEOPTION, L_FAR * LPEXTFILEOPTION;
#define Struct_TIF_Option _EXTFILEOPTION
#define TIFOPTION EXTFILEOPTION
#define LPTIFOPTION LPEXTFILEOPTION

#define BFLT_DONTCARE   256

typedef struct tagBINARYFLT
{
   L_BOOL bfltMax;              /* select maximimum value */
   L_UINT bfltDim;              /* filter dimensions */
   L_INT bfltMatrix[1];         /* array of filter coefficients */
} BINARYFLT, L_FAR * LPBINARYFLT;
#define BINARYFLTSIZE(dim) (sizeof(BINARYFLT) + (sizeof(L_INT) * (((L_INT) (dim) * (L_INT) (dim)) - 1)))
#define MAKEINTBFLT(i) ((LPBINARYFLT)((L_UINT32)((L_UINT16) (i))))

#define BFLT_EROSION_OMNI    MAKEINTBFLT(0)
#define BFLT_EROSION_HORZ    MAKEINTBFLT(1)
#define BFLT_EROSION_VERT    MAKEINTBFLT(2)
#define BFLT_EROSION_DIAG    MAKEINTBFLT(3)
#define BFLT_DILATION_OMNI    MAKEINTBFLT(4)
#define BFLT_DILATION_HORZ    MAKEINTBFLT(5)
#define BFLT_DILATION_VERT    MAKEINTBFLT(6)
#define BFLT_DILATION_DIAG    MAKEINTBFLT(7)

typedef struct tagSPATIALFLT
{
   L_INT fltDivisor;            /* sum divisor */
   L_INT fltBias;               /* sum bias */
   L_UINT fltDim;               /* filter dimensions */
   L_INT fltMatrix[1];          /* array of filter coefficients */
} SPATIALFLT, L_FAR * LPSPATIALFLT;
#define SPATIALFLTSIZE(dim) (sizeof(SPATIALFLT) + (sizeof(L_INT) * (((L_INT) (dim) * (L_INT) (dim)) - 1)))
#define MAKEINTFLT(i) ((LPSPATIALFLT)((L_UINT32)((L_UINT16) (i))))


#define FLT_EMBOSS_N    MAKEINTFLT(0)
#define FLT_EMBOSS_NE   MAKEINTFLT(1)
#define FLT_EMBOSS_E    MAKEINTFLT(2)
#define FLT_EMBOSS_SE   MAKEINTFLT(3)
#define FLT_EMBOSS_S    MAKEINTFLT(4)
#define FLT_EMBOSS_SW   MAKEINTFLT(5)
#define FLT_EMBOSS_W    MAKEINTFLT(6)
#define FLT_EMBOSS_NW   MAKEINTFLT(7)

#define FLT_GRADIENT_N    MAKEINTFLT(8)
#define FLT_GRADIENT_NE   MAKEINTFLT(9)
#define FLT_GRADIENT_E    MAKEINTFLT(10)
#define FLT_GRADIENT_SE   MAKEINTFLT(11)
#define FLT_GRADIENT_S    MAKEINTFLT(12)
#define FLT_GRADIENT_SW   MAKEINTFLT(13)
#define FLT_GRADIENT_W    MAKEINTFLT(14)
#define FLT_GRADIENT_NW   MAKEINTFLT(15)

#define FLT_LAPLACE_1     MAKEINTFLT(16)
#define FLT_LAPLACE_2     MAKEINTFLT(17)
#define FLT_LAPLACE_3     MAKEINTFLT(18)
#define FLT_LAPLACE_DIAG  MAKEINTFLT(19)
#define FLT_LAPLACE_HORZ  MAKEINTFLT(20)
#define FLT_LAPLACE_VERT  MAKEINTFLT(21)

#define FLT_SOBEL_HORZ    MAKEINTFLT(22)
#define FLT_SOBEL_VERT    MAKEINTFLT(23)

#define FLT_PREWITT_HORZ  MAKEINTFLT(24)
#define FLT_PREWITT_VERT  MAKEINTFLT(25)

#define FLT_SHIFTDIFF_DIAG  MAKEINTFLT(26)
#define FLT_SHIFTDIFF_HORZ  MAKEINTFLT(27)
#define FLT_SHIFTDIFF_VERT  MAKEINTFLT(28)

#define FLT_LINESEG_HORZ  MAKEINTFLT(29)
#define FLT_LINESEG_VERT  MAKEINTFLT(30)
#define FLT_LINESEG_LTOR  MAKEINTFLT(31)
#define FLT_LINESEG_RTOL  MAKEINTFLT(32)

#define EFFECT_NORMAL               0
#define EFFECT_WIPE_DOWN            1
#define EFFECT_WIPE_UP              2
#define EFFECT_WIPE_RIGHT           3
#define EFFECT_WIPE_LEFT            4

#define EFFECT_RANDOM_PIXEL         5
#define EFFECT_RANDOM_SMALLBOX      6
#define EFFECT_RANDOM_MEDIUMBOX     7
#define EFFECT_RANDOM_LARGEBOX      8

#define EFFECT_RANDOM_HLINE         9
#define EFFECT_RANDOM_VLINE        10

#define EFFECT_VMIDDLE             11
#define EFFECT_HMIDDLE             12

#define EFFECT_HINTERLACE          13
#define EFFECT_VINTERLACE          14

#define EFFECT_OUTTOIN             15
#define EFFECT_INTOOUT             16

#define EFFECT_OUTTOIN_INTERLACE   17
#define EFFECT_INTOOUT_INTERLACE   18

#define EFFECT_SPIRAL_IN           19
#define EFFECT_SPIRAL_OUT          20

#define EFFECT_LAST                20


/*=========================================================================
                     LEAD Tools functions prototype
  =========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------(I/O redirect functions)--------------------------*/

   typedef L_INT ((L_FAR * REDIRECTOPEN) (const L_CHAR L_FAR *, L_INT, L_INT));
   typedef L_UINT ((L_FAR * REDIRECTREAD) (L_INT, L_CHAR L_FAR *, L_INT));
   typedef L_UINT ((L_FAR * REDIRECTWRITE) (L_INT, L_CHAR L_FAR *, L_INT));
   typedef L_INT32 ((L_FAR * REDIRECTSEEK) (L_INT, L_INT32, L_INT));
   typedef L_INT ((L_FAR * REDIRECTCLOSE) (L_INT));

/*----------------------(Call back functions)------------------------------*/

   typedef L_INT ((L_FAR * PAINTCALLBACK) (HWND hWnd, L_VOID L_FAR * lpBuffer));

   typedef L_INT ((L_FAR * STATUSCALLBACK) (L_INT));

   typedef L_INT ((L_FAR * FILEREADCALLBACK) (pBITMAPHANDLE lpBitmap,
                                               L_UCHAR L_FAR * lpBuffer,
                                               L_INT nLines,
                                               L_VOID L_FAR * lpUserData));

   typedef L_INT ((L_FAR * FILESAVECALLBACK) (pBITMAPHANDLE lpBitmap,
                                               L_UCHAR L_FAR * lpBuffer,
                                               L_UINT uStartRow,
                                               L_UINT uLines,
                                               L_VOID L_FAR * lpUserData));

   typedef L_INT ((L_FAR * TWAINCALLBACK) (pBITMAPHANDLE lpBitmap,
                                            L_VOID L_FAR * lpUserData));

/*-----------------------(General Functions)-------------------------------*/

   L_INT EXT_FUNCTION L_CompressALL (L_CHAR L_FAR * lpszImageFile, L_CHAR L_FAR * lpszCompFile,
                                      L_INT nOutputType, L_INT nImageQfactor,
                                      L_INT nFilePalette);

   L_INT EXT_FUNCTION L_CompressTGA (L_CHAR L_FAR * lpszImageFile, L_CHAR L_FAR * lpszCompFile,
                                      L_INT nOutputType, L_INT nImageQfactor,
                                      L_INT nFilePalette, L_INT nALF);

   L_INT EXT_FUNCTION L_CompressPCX (L_CHAR L_FAR * lpszImageFile, L_CHAR L_FAR * lpszCompFile,
                                      L_INT nOutputType, L_INT nImageQfactor,
                                      L_INT nFilePalette);

   L_INT EXT_FUNCTION L_CompressTIF (L_CHAR L_FAR * lpszImageFile, L_CHAR L_FAR * lpszCompFile,
                                      L_INT nOutputType, L_INT nImageQfactor,
                                      L_INT nFilePalette);

   L_INT EXT_FUNCTION L_CompressGIF (L_CHAR L_FAR * lpszImageFile, L_CHAR L_FAR * lpszCompFile,
                                      L_INT nOutputType, L_INT nImageQfactor,
                                      L_INT nFilePalette);

   L_INT EXT_FUNCTION L_CompressBMP (L_CHAR L_FAR * lpszImageFile, L_CHAR L_FAR * lpszCompFile,
                                      L_INT nOutputType, L_INT nImageQfactor,
                                      L_INT nFilePalette);

   L_INT EXT_FUNCTION L_CompressBitmap (pBITMAPHANDLE lpBitmap, L_CHAR L_FAR * lpszCompFile,
                                     L_INT nOutputType, L_INT nImageQfactor,
                                         L_INT nFilePalette);

   L_INT EXT_FUNCTION L_DecompressBitmap (L_CHAR L_FAR * lpszCompFile, pBITMAPHANDLE lpBitmap,
                                           L_INT nBitPerPixel);

   L_INT EXT_FUNCTION L_DecompressALL (L_CHAR L_FAR * lpszCompFile, L_CHAR L_FAR * lpszImageFile,
                                        L_INT nOutputType);

   L_INT EXT_FUNCTION L_DecompressTGA (L_CHAR L_FAR * lpszCompFile, L_CHAR L_FAR * lpszImageFile,
                                        L_INT nOutputType);

   L_INT EXT_FUNCTION L_DecompressPCX (L_CHAR L_FAR * lpszCompFile, L_CHAR L_FAR * lpszImageFile,
                                        L_INT nOutputType);

   L_INT EXT_FUNCTION L_DecompressTIF (L_CHAR L_FAR * lpszCompFile, L_CHAR L_FAR * lpszImageFile,
                                        L_INT nOutputType);

   L_INT EXT_FUNCTION L_DecompressGIF (L_CHAR L_FAR * lpszCompFile, L_CHAR L_FAR * lpszImageFile,
                                        L_INT nOutputType);

   L_INT EXT_FUNCTION L_DecompressBMP (L_CHAR L_FAR * lpszCompFile, L_CHAR L_FAR * lpszImageFile,
                                        L_INT nOutputType);

   L_INT EXT_FUNCTION L_DecompressFile (L_CHAR L_FAR * lpszCompFile, pBITMAPHANDLE lpBitmap,
                 FILEREADCALLBACK lpYourFunction, L_INT nBitsPerPixelWanted,
                         L_VOID L_FAR * lpYourData, L_INT nExtraBytesToPad);

   L_INT EXT_FUNCTION L_SetCallbackLine (L_INT nline);

   L_INT EXT_FUNCTION L_LoadTGAFile (L_CHAR L_FAR * lpszFilename, pBITMAPHANDLE lpBitmap,
                 FILEREADCALLBACK lpYourFunction, L_INT nBitsPerPixelWanted,
                                      L_VOID L_FAR * lpUserData);

   L_INT EXT_FUNCTION L_LoadTIFFile (L_CHAR L_FAR * lpszFilename, pBITMAPHANDLE lpBitmap,
                 FILEREADCALLBACK lpYourFunction, L_INT nBitsPerPixelWanted,
                                      L_VOID L_FAR * lpUserData);

   L_INT EXT_FUNCTION L_LoadBMPFile (L_CHAR L_FAR * lpszFilename, pBITMAPHANDLE lpBitmap,
                 FILEREADCALLBACK lpYourFunction, L_INT nBitsPerPixelWanted,
                                      L_VOID L_FAR * lpUserData);

   L_INT EXT_FUNCTION L_LoadPCXFile (L_CHAR L_FAR * lpszFilename, pBITMAPHANDLE lpBitmap,
                 FILEREADCALLBACK lpYourFunction, L_INT nBitsPerPixelWanted,
                                      L_VOID L_FAR * lpUSerData);

   L_INT EXT_FUNCTION L_LoadGIFFile (L_CHAR L_FAR * lpszFilename, pBITMAPHANDLE lpBitmap,
                 FILEREADCALLBACK lpYourFunction, L_INT nBitsPerPixelWanted,
                                      L_VOID L_FAR * lpUserData);

   L_INT EXT_FUNCTION L_LoadCALSFile (L_CHAR L_FAR * Filename,
                                       pBITMAPHANDLE pBitmap,
                                       FILEREADCALLBACK pYourFunction,
                                       L_INT BitsPerPixelWanted,
                                       L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadMACFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadIMGFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadMSPFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadWPGFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadRASFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadEPSFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadPCTFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);

   L_INT EXT_FUNCTION L_LoadPCDFile (L_CHAR L_FAR * Filename,
                                      pBITMAPHANDLE pBitmap,
                                      FILEREADCALLBACK pYourFunction,
                                      L_INT BitsPerPixelWanted,
                                      L_VOID L_FAR * pYourData);


   L_INT EXT_FUNCTION L_StartCompressBuffer (pBITMAPHANDLE lpBitmap, FILEREADCALLBACK lpYourCallbackFunction,
                                  L_UINT32 nInputBytes, L_UINT nOutputBytes,
                         L_CHAR L_FAR * lpszOutputBuffer, L_INT nOutputType,
                                L_INT nQFactor, L_VOID L_FAR * lpYourParms);

   L_INT EXT_FUNCTION L_CompressBuffer (L_UCHAR L_FAR * lpszbuffer);

   L_INT EXT_FUNCTION L_EndCompressBuffer (L_VOID);

   L_INT EXT_FUNCTION L_DitherBitmap (pBITMAPHANDLE lpBitmap, L_INT nDitheringMethod,
                                       L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_StartResize (L_INT nOldWidth, L_INT nOldHeight,
                                      L_INT nNewWidth, L_INT nNewHeight,
                                      L_VOID L_FAR * L_FAR * lpResizeData);

   L_INT EXT_FUNCTION L_Resize (L_UCHAR L_FAR * lpLineBuf,
                                 L_INT nCurrentRow, L_INT nBitsPerPixel,
                               L_INT L_FAR * lpXSize, L_INT L_FAR * lpYSize,
                                 pRESIZEDATA lpResizeData);

   L_INT EXT_FUNCTION L_StopResize (pRESIZEDATA lpResizeData);

   L_VOID EXT_FUNCTION L_InitBitmap (pBITMAPHANDLE lpBitmap, L_INT nWidth,
                                      L_INT nHeight, L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_AllocateBitmap (pBITMAPHANDLE lpBitmap,
                                         L_INT nTypeOfMemoryIWant);

   L_VOID EXT_FUNCTION L_FreeBitmap (pBITMAPHANDLE lpBitmap);

   L_INT32 EXT_FUNCTION L_GetBitmapRow (pBITMAPHANDLE lpBitmap,
                                         L_UCHAR L_FAR * lpBuffer,
                                         L_INT nRow,
                                         L_UINT32 nNumberOfBytes);

   L_INT32 EXT_FUNCTION L_PutBitmapRow (pBITMAPHANDLE lpBitmap,
                                         L_UCHAR L_FAR * lpBuffer,
                                       L_INT nRow, L_UINT32 nNumberOfBytes);

   L_INT32 EXT_FUNCTION L_GetBitmapRowCol (pBITMAPHANDLE lpBitmap,
                                            L_UCHAR L_FAR * lpBuffer,
                                            L_INT nRow, L_INT nCol,
                                            L_UINT32 nNumberOfBytes);

   L_INT32 EXT_FUNCTION L_PutBitmapRowCol (pBITMAPHANDLE lpBitmap,
                                            L_UCHAR L_FAR * lpBuffer,
                                            L_INT nRow, L_INT nCol,
                                            L_UINT32 nNumberOfBytes);

   L_INT EXT_FUNCTION L_FlipBitmap (pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_ReverseBitmap (pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_RotateBitmap (pBITMAPHANDLE lpBitmap,
                                       L_INT nNumberOfDegrees,
                                     L_INT nResizeWindowToFitRotatedBitmap);

   L_INT EXT_FUNCTION L_ChangeBitmapIntensity (pBITMAPHANDLE lpBitmap,
                                                L_INT nIntensityChange);

   L_INT EXT_FUNCTION L_ChangeBitmapContrast (pBITMAPHANDLE pBitmap,
                                               L_INT nContrastChange);

   L_INT EXT_FUNCTION L_InvertBitmap (pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_ClearBitmap (pBITMAPHANDLE lpBitmap);
   L_INT EXT_FUNCTION L_SizeBitmap (pBITMAPHANDLE lpBitmap, L_INT nNewWidth,
                                     L_INT nNewHeight);

   L_INT EXT_FUNCTION L_ResizeBitmap (pBITMAPHANDLE lpSourceBitmap,
                                       pBITMAPHANDLE lpDestBitmap);

   L_INT EXT_FUNCTION L_OptimizeBitmap (pBITMAPHANDLE lpSourceBitmap,
                                         pBITMAPHANDLE lpDestBitmap,
                                         L_INT nDitheringMethod,
                                         L_INT nNumberColorsOut);

   L_INT EXT_FUNCTION L_FileInfo (L_CHAR L_FAR * lpszFileName,
                                   pFILEINFO lpFileInformation);

   L_INT EXT_FUNCTION L_SaveTGABitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_SaveTIFBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel,
                                        L_INT nCompressionOption);

   L_INT EXT_FUNCTION L_SaveGIFBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_SavePCXBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_SaveBMPBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel,
                                        L_INT nWin_Or_OS2);

   L_INT EXT_FUNCTION L_SaveWMFBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_SaveEPSBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap,
                                        L_INT nBitsPerPixel);

   L_INT EXT_FUNCTION L_SaveCALSBitmap (L_CHAR L_FAR * pFileName,
                                         pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_SaveMACBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_SaveMSPBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_SaveWPGBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap,
                                        L_INT BitsPerPixel);

   L_INT EXT_FUNCTION L_SaveRASBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap,
                                        L_INT BitsPerPixel);

   L_INT EXT_FUNCTION L_SavePCTBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap,
                                        L_INT BitsPerPixel);

   L_INT EXT_FUNCTION L_SaveIMGBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_FileConvert (L_CHAR L_FAR * pFrom,
                                      L_CHAR L_FAR * pTo,
                                      L_INT ToType,
                                      L_INT NewWidth,
                                      L_INT NewHeight,
                                      L_INT Pixel,
                                      L_INT QFactor);

   L_INT EXT_FUNCTION L_CompressBitmap1Bit (pBITMAPHANDLE lpBitmap,
                                             L_CHAR L_FAR * lpszFileName,
                                             L_INT nCompressionTypeFor1Bit);

   L_INT EXT_FUNCTION L_DecompressBitmap1Bit (L_CHAR L_FAR * lpszFileName,
                                               pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_HalfToneBitmap (pBITMAPHANDLE pBitmap, L_UINT uType);

   L_INT EXT_FUNCTION L_GrayScaleBitmap (pBITMAPHANDLE lpBitmap);

   L_VOID EXT_FUNCTION L_SetGrayFixedPalette (L_INT nflag);

   L_INT EXT_FUNCTION L_GetCompressFileStamp (L_CHAR L_FAR * lpszFileName,
                                               pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_CompressBitmapWithStamp (pBITMAPHANDLE lpBitmap,
                                                L_CHAR L_FAR * lpszFileName,
                                                  L_INT nformat,
                                                  L_INT nQFactor,
                                                  L_INT nStampWidth,
                                                  L_INT nStampHeight,
                                                  L_INT nStampBits,
                                                  L_INT nDitherFlag);

   L_INT EXT_FUNCTION L_CopyBitmap (pBITMAPHANDLE lpToBitmap,
                                     pBITMAPHANDLE lpFromBitmap);

   L_INT EXT_FUNCTION L_CopyBitmapData (pBITMAPHANDLE lpToBitmap,
                                         pBITMAPHANDLE lpFromBitmap);

   L_VOID EXT_FUNCTION L_CopyBitmapHandle (pBITMAPHANDLE lpToBitmap,
                                            pBITMAPHANDLE lpFromBitmap);

   L_INT EXT_FUNCTION L_Create1BitTable (L_VOID);

   L_VOID EXT_FUNCTION L_Free1BitTable (L_VOID);

   L_VOID EXT_FUNCTION L_SetTIFFXYResolution (L_INT nX, L_INT nY);

   L_INT EXT_FUNCTION L_SetTIFFOption (TIFOPTION L_FAR * pOption);
#define L_SetExtFileOption L_SetTIFFOption

   L_VOID EXT_FUNCTION L_RedirectIO (REDIRECTOPEN lpOpenFile, REDIRECTREAD lpReadFile,
                         REDIRECTWRITE lpWriteFile, REDIRECTSEEK lpSeekFile,
                                      REDIRECTCLOSE lpCloseFile);

   L_INT EXT_FUNCTION L_SaveBitmap (LPTSTR lpszFile, pBITMAPHANDLE pBitmap,
                                L_INT nFormat, L_INT nBits, L_INT nQFactor);

   L_INT EXT_FUNCTION L_LoadBitmap (LPTSTR lpszFile, pBITMAPHANDLE pBitmap,
                                     L_INT nBitsTo, L_INT nColorOrder);

   L_INT EXT_FUNCTION L_LoadBitmapMemory (L_CHAR L_HUGE * pMemHandle,
                                           pBITMAPHANDLE pBitmap,
                                           L_INT BitsPerPixel,
                                           L_INT nOrder,
                                           L_INT32 nSizeOfFile);


   L_INT EXT_FUNCTION L_LoadMemory (L_CHAR L_HUGE * pMemHandle,
                                     pBITMAPHANDLE pBitmap,
                                     L_INT BitsPerPixel,
                                     L_INT nColorOrder,
                                     L_UINT uFlags,
                                     FILEREADCALLBACK lpfnCallBack,
                                     L_VOID L_FAR * lpUserData,
                                     L_INT32 nSizeOfFile);

   L_INT EXT_FUNCTION L_ConvertFile (LPTSTR lpszFrom, LPTSTR lpszTo, L_INT nFormat,
                                  L_INT nBits, L_INT nQFactor, L_INT nWidth,
                                      L_INT nHeight);

   L_INT EXT_FUNCTION L_ProcessBitmap (pBITMAPHANDLE lpBitmap, L_INT nOperation,
                                        LPPROCESSSTRUCT lpProcessStruct);

/*----------------------(DOS Environment Functions)------------------------*/


/*----------------------(Windows Environment Functions)--------------------*/


   L_VOID EXT_FUNCTION L_AccessBitmap (pBITMAPHANDLE lpBitmap);

   L_VOID EXT_FUNCTION L_ReleaseBitmap (pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_LoadPCXBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_LoadGIFBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadBMPBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_LoadTGABitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_LoadTIFBitmap (L_CHAR L_FAR * lpszFileName,
                                        pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_LoadCALSBitmap (L_CHAR L_FAR * pFileName,
                                         pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadMACBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadIMGBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadMSPBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadWPGBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadRASBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadEPSBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadPCTBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadPCDBitmap (L_CHAR L_FAR * pFileName,
                                        pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_LoadWMFBitmap (L_CHAR L_FAR * pFileName,
                          pBITMAPHANDLE pBitmap, L_INT width, L_INT height);

   L_INT EXT_FUNCTION L_PaintDevice (HWND hWnd, HDC hDC,
                                      pBITMAPHANDLE lpBitmap,
                                  LPRECT lpRect, PAINTCALLBACK lpCallBackFn,
                                      L_VOID L_FAR * lpUserData);

   L_INT EXT_FUNCTION L_PaintBuffer (HWND hWnd, HDC hDC, pBITMAPHANDLE lpBitmap,
                                    L_UCHAR L_FAR * lpBuffer, LPRECT lpRect,
                                      PAINTCALLBACK lpCallBackFn,
                                      L_VOID L_FAR * lpUserData);

   HANDLE EXT_FUNCTION L_ConvertToDIB (pBITMAPHANDLE lpBitmap);

   L_INT EXT_FUNCTION L_ConvertFromDIB (pBITMAPHANDLE lpBitmap,
                                         BITMAPINFO L_FAR * lpbmiBitmap,
                                         L_UCHAR L_HUGE * lpData);

   L_INT EXT_FUNCTION L_ColorResolution (pBITMAPHANDLE lpBitmapToConvert,
                                          L_INT nBitsPerPixelWanted,
                                          L_UINT32 nOptionFlags,
                                          FILEREADCALLBACK lpYourFunction,
                                          L_VOID L_FAR * lpUserData);

   L_INT EXT_FUNCTION L_CopyBitmapRect (pBITMAPHANDLE lpToBitmap,
                                         pBITMAPHANDLE lpFromBitmap,
                                         L_UINT nXPosition,
                                         L_UINT nYPosition,
                                         L_UINT nWidth, L_UINT nHeight);

   BOOL EXT_FUNCTION L_CopyFromClipboard (HWND hWnd, pBITMAPHANDLE lpBitmap);

   BOOL EXT_FUNCTION L_CopyToClipboard (HWND hWnd, pBITMAPHANDLE lpBitmap);

   HDC EXT_FUNCTION L_PrintBitmap (BITMAPHANDLE L_FAR * Bitmap, L_INT nXPos,
                                    L_INT nYPos, L_INT nXSize,
                                    L_INT nYSize, BOOL bEndDoc);

   L_VOID EXT_FUNCTION L_LoadCustomQTable (L_INT L_FAR * lpnQTable);

   L_VOID EXT_FUNCTION L_LoadCustomQTable (L_INT L_FAR * lpnQTable);


   HPALETTE EXT_FUNCTION L_CreateScreenPalette (L_VOID);
   L_INT EXT_FUNCTION L_SaveMultiTIFBitmap (LPTSTR lpszFileName,
                                             pBITMAPHANDLE pBitmap,
                                             L_INT nPixel,
                                             L_INT nCompressOption);
   HANDLE EXT_FUNCTION L_ChangeToDIB (pBITMAPHANDLE lpBitmap);
   HBITMAP EXT_FUNCTION L_ChangeToDDB (HDC hDC, pBITMAPHANDLE lpBitmap);
   HPALETTE EXT_FUNCTION L_DupPalette (HPALETTE hPalette);
   L_INT EXT_FUNCTION L_DecompressMemory (L_CHAR L_HUGE * pBuf,
                                           pBITMAPHANDLE pBitmapHandle,
                                           FILEREADCALLBACK lpfnCallBack,
                                           L_INT nBitsPerPixel,
                                           L_VOID L_FAR * lpYourData,
                                           L_INT nExtraBytesToPad,
                                           L_INT32 nSizeOfBuffer);

   L_INT EXT_FUNCTION L_DecompressBitmapMemory (L_CHAR L_HUGE * pBuf,
                                                 pBITMAPHANDLE pBitmapHandle,
                                                 L_INT nBitsPerPixel,
                                                 L_INT32 nSizeOfFile);



   L_INT EXT_FUNCTION L_GetPCDResolution (L_CHAR L_FAR * pFilename, pPCDINFO pPCDInfo);
   L_INT EXT_FUNCTION L_SetPCDResolution (L_INT nResolution);


   L_INT EXT_FUNCTION L_StartCompressData (pBITMAPHANDLE pBitmap,
                                            CDATACALLBACK pYourFunction,
                                            L_UINT nInBufferSize,
                                            L_UINT nOutBufferSize,
                                            L_UCHAR L_FAR * pOutBuffer,
                                            L_UINT nHeaderSize,
                                            L_UCHAR L_FAR * pHeaderBuffer,
                                            L_VOID L_FAR * pParms);

   L_INT EXT_FUNCTION L_CompressData (L_UCHAR L_FAR * pInBuffer);
   L_VOID EXT_FUNCTION L_EndCompressData (L_VOID);


   L_INT EXT_FUNCTION L_StartDecompressData (pBITMAPHANDLE pBitmap,
                                              CDATACALLBACK pYourFunction,
                                              L_UINT nInBufferSize,
                                              L_UCHAR L_FAR * pInBuffer,
                                              L_UINT nOutBufferSize,
                                              L_UINT L_FAR * pHeaderSize,
                                              L_UCHAR L_FAR * pHeaderBuffer,
                                              L_VOID L_FAR * pParms);

   L_INT EXT_FUNCTION L_DecompressData (L_UCHAR L_FAR * pOutBuffer);
   L_VOID EXT_FUNCTION L_EndDecompressData (L_VOID);
   L_INT EXT_FUNCTION L_FileInfoMemory (L_CHAR L_HUGE * pMemHandle,
                                         pFILEINFO pFileInfo,
                                         L_INT32 nSizeOfFile);

   HPALETTE EXT_FUNCTION L_CreatePaintPalette (HDC hDC, pBITMAPHANDLE pBitmap);

   L_INT EXT_FUNCTION L_AverageFilterBitmap (pBITMAPHANDLE pBitmap, L_UINT uDim);
   L_INT EXT_FUNCTION L_MedianFilterBitmap (pBITMAPHANDLE pBitmap, L_UINT uDim);
   L_INT EXT_FUNCTION L_ChangeBitmapHue (pBITMAPHANDLE pBitmap, L_INT nAngle);
   L_INT EXT_FUNCTION L_ChangeBitmapSaturation (pBITMAPHANDLE pBitmap, L_INT nChange);
   HDC EXT_FUNCTION L_CreateLeadDC (pBITMAPHANDLE pBitmap);
   L_INT EXT_FUNCTION L_PosterizeBitmap (pBITMAPHANDLE pBitmap, L_UINT uLevels);
   L_INT EXT_FUNCTION L_MosaicBitmap (pBITMAPHANDLE pBitmap, L_UINT uDim);
   L_INT EXT_FUNCTION L_SpatialFilterBitmap (pBITMAPHANDLE pBitmap, LPSPATIALFLT pFilter);
   L_INT EXT_FUNCTION L_SharpenBitmap (pBITMAPHANDLE pBitmap, L_INT nSharpness);
   L_INT EXT_FUNCTION L_ChangeFromDIB (pBITMAPHANDLE pBitmap, HANDLE hDIB);
   L_INT EXT_FUNCTION L_TwainAcquire (HWND hWnd, pBITMAPHANDLE pBitmap);
   L_INT EXT_FUNCTION L_TwainSelect (HWND hWnd);
   L_BOOL EXT_FUNCTION L_EnableFastPaint (L_BOOL);
   L_INT EXT_FUNCTION L_PaintEffect (HDC hDC, pBITMAPHANDLE pBitmap, LPRECT pRect, L_UINT uEffect);
   L_INT EXT_FUNCTION L_ConvertBuffer (L_UCHAR L_FAR * pBuf, L_INT Width,
                                L_INT BitsInPerPixel, L_INT BitsOutPerPixel,
                                        L_INT OrderIn, L_INT OrderOut,
                               LPRGBQUAD pInPalette, LPRGBQUAD pOutPalette);

   L_INT EXT_FUNCTION L_StartDithering (pBITMAPHANDLE pBitmap, LPRGBQUAD pPalette, L_UINT uColors);

   L_INT EXT_FUNCTION L_DitherLine (pBITMAPHANDLE pBitmap, L_UCHAR L_FAR * pIn,
                                     L_UCHAR L_FAR * pOut);

   L_INT EXT_FUNCTION L_StopDithering (pBITMAPHANDLE pBitmap);
   L_INT EXT_FUNCTION L_HistoEqualizeBitmap (pBITMAPHANDLE pBitmap);
   L_BOOL EXT_FUNCTION L_EnableDitheredPaint (L_BOOL);
   L_BOOL EXT_FUNCTION L_EnableIndexedPaint (L_BOOL);
   HDC EXT_FUNCTION L_PrintBitmapExt (HDC hDC, pBITMAPHANDLE pBitmap, L_INT nXPos,
                                       L_INT nYPos, L_INT nXSize,
                                       L_INT nYSize, BOOL fEndDoc);
   L_INT EXT_FUNCTION L_LoadFile (L_CHAR L_FAR * lpszFile, pBITMAPHANDLE pBitmap,
                                   L_INT nBitsTo, L_INT nColorOrder,
                                   L_UINT uFlags,
                                   FILEREADCALLBACK lpYourFunction,
                                   L_VOID L_FAR * lpUserData);


   L_INT EXT_FUNCTION L_BinaryFilterBitmap (pBITMAPHANDLE pBitmap, LPBINARYFLT pFilter);
   L_INT EXT_FUNCTION L_RotateBitmapFine (pBITMAPHANDLE pBitmap, L_INT32 nAngle, L_BOOL fResize, COLORREF crFill);
   L_BOOL EXT_FUNCTION L_EnableOrderedDither (L_BOOL fEnable);
   HBITMAP EXT_FUNCTION L_ConvertToDDB (HDC hdc, pBITMAPHANDLE pBitmap);
   L_INT EXT_FUNCTION L_ConvertFromDDB (HDC hdc, pBITMAPHANDLE lpBitmap, HBITMAP hBitmap, HPALETTE hPalette);
   L_BOOL EXT_FUNCTION L_ClipboardReady (L_VOID);
   L_INT EXT_FUNCTION L_ChangeFromDDB (HDC hdc, pBITMAPHANDLE pBitmap, HBITMAP hBitmap, HPALETTE hPalette);
   L_INT EXT_FUNCTION L_CopyToClipboardExt (HWND hWnd, pBITMAPHANDLE pBitmap, L_UINT uFlags);
   L_BOOL EXT_FUNCTION L_EnableScaleToGray (L_BOOL fEnable);

   L_INT EXT_FUNCTION L_SaveFile (L_CHAR L_FAR * lpszFilename,
                                   pBITMAPHANDLE pInBitmap,
                                   L_INT nFormat,
                                   L_INT nBitsTo,
                                   L_INT nQFactor,
                                   L_UINT uFlags,
                                   FILESAVECALLBACK pFunction,
                                   L_VOID L_FAR * lpUserData);
   L_INT EXT_FUNCTION L_DespeckleBitmap (pBITMAPHANDLE pBitmap);
   L_INT EXT_FUNCTION L_DefaultDithering (L_UINT DefaultDitheringMethod);

   STATUSCALLBACK EXT_FUNCTION L_SetStatusCallBack (STATUSCALLBACK pFunction);
   L_INT EXT_FUNCTION L_TwainAcquireExt (HWND hWnd,
                                          pBITMAPHANDLE pBitmap,
                                          L_UINT uFlags,
                                          TWAINCALLBACK pFunction,
                                          L_VOID L_FAR * lpUserData);
#ifdef __cplusplus
}
#endif

#ifndef __BORLANDC__
#pragma pack()
#else
#pragma option -a.
#endif                          /* __BORLANDC__ */
#endif                          /* !defined(_BITMAP_H) */
