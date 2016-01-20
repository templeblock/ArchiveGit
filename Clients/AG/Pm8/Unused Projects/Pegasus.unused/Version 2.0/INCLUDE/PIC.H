/*.T PIC.H  Include file for Pegasus Routines
 * $Header: /PM8/Pegasus/Version 2.0/INCLUDE/PIC.H 1     3/03/99 6:25p Gbeddow $
 * $Nokeywords: $
 */

/***************************************************************************\
*       Copyright (c) 1996-1998, Pegasus Imaging Corporation                *
*       All rights reserved.                                                *
*****************************************************************************
*       Revision History:                                                   *
* modified 2/16/96 -- jweber                                                *
*       -- changed Eof in QUEUE to a dword of flags                         *
*       -- changed names of OP80, 81, 82 structures to D2F, F2D, UTL        *
* modified 2/2/96 -- jweber                                                 *
*       -- added QBIT_COMMENT, QBIT_PALETTE, BI_RLE4, BI_RLE8               *
* modified 1/26/96 -- jweber                                                *
*       -- added YieldEvery to opcode 81                                    *
*       -- modified QBIT_IMAGESIZE comment                                  *
* modified 12/26/95+ -- jweber                                              *
*       -- added PicFlags to opcode 80                                      *
*       -- added BI_PCX, BI_TIF, BI_TGA (generic) definitions               *
*       -- added PF_MultiImage flag                                         *
*       -- added PicFlags to opcode 82                                      *
*       -- renamed Histogram/ColorMap to ptr/ptr2, opcode 82                *
*       -- added NumColors to opcode 82; removed references to subcode 7    *
*       -- added NumImages and ImageNum to PEGQUERY, and associated         *
*          QBIT_NUMIMAGES                                                   *
*       -- changed PEGQUERY to inidividual expansion DWORDs                 *
* modified 2/22/96 -- smann                                                 *
*       -- support reverse queue                                            *
*       -- ParmVer is now 11, ParmVerMinor is now 1                         *
* modified 3/12/96 -- jweber                                                *
*       -- opcode 82: "Reserverd3" changed to "ptr3"                        *
* modified 3/27/96 -- jweber                                                *
*       -- incorporates John's changes for opcode 15 (D2J)                  *
* modified 4/4/96 -- jweber                                                 *
*       -- more of John's changes                                           *
* modified 4/17/96 -- jweber                                                *
*       -- QUEUE structure changed to new pointers                          *
* modified 4/21/96 -- SSM                                                   *
*       -- LOSSLESS and LAYER structures added as well as support for the   *
*          LIME and LIMP functions.                                         *
* modified 4/23/96 -- jrb                                                   *
*       -- ParmVer is 13                                                    *
* modified 5/9/96 -- jweber                                                 *
*       -- Added BI_TIFJ, QBIT_SOIMARKER, and SOIMarker field to query      *
*          private u structure in support for TIFF Jpeg                     *
* modified 8/31/96 -- Els                                                   *
*       -- Added support for ZOOM operation                                 *
* modified 10/16/96 -- SSM                                                  *
*       -- Added support for cropping                                       *
* modified 3/2/96 -- jweber                                                 *
*       -- Reversing optimization                                           *
* modified 3/10/97 -- jweber                                                *
*       -- Several changes to D2F_STRUC to support Printhouse requirements  *
*          (32-bit "DIB"s; CMYK indication; DotRange support; Moto byte     *
*          order; X & Y resolutions                                         *
* modified 6/12/97 -- SSM                                                   *
*       -- Changes to allow LIP/LIE support                                 *
* modified 8/2/97  -- Jim                                                   *
*       -- John and Charles changes so pc/mac use a single set of includes  *
* modified 9/18/97 -- SSM                                                   *
*       -- Changes to allow LL3 and PIC2List support                        *
* modified 12/8/97 -- SSM                                                   *
*       -- Changes to allow Regions                                         *
* modified 4/28/98 -- SSM                                                   *
*       -- Changes to allow support for JPEG restart markers                *
* modified 9/14/98 -- SSM                                                   *
*       -- Added WSQ support                                                *
\***************************************************************************/

#if !defined(_PIC)
#define _PIC

#define CURRENT_PARMVER         20

/* DLG added to detect Windows environment from compiler directives */
  #if !defined(WINDOWS)
    #if defined(_Windows) || defined(_WINDOWS) || defined(_WINDOWS_) || \
        defined(__WINDOWS_386__) || defined(_WINDOWS_16_)
            #define WINDOWS
    #endif
  #endif

  #if !defined(__FLAT__) && defined(WIN32)
    #define __FLAT__
  #endif

#ifdef __MWERKS__
    #if PRAGMA_ALIGN_SUPPORTED
    #pragma options align=packed
    #endif
#elif defined(__BORLANDC__)
    #pragma option -a1
#else /* assume microsoft or watcom */
    #if defined(__FLAT__) || defined(__WATCOMC__)
        #pragma pack(push)
    #endif
    /* microsoft 16-bit doesn't support pack(push) */
    #pragma pack(1)
#endif

#include    <setjmp.h>
#include    "stdtypes.h"

#if defined(PIC_INTERNAL)
#include "internal.h"
#endif

/* DLG - only include if not doing windows */
    #if !defined(WINDOWS)
        #include  "bmp.h"
  #endif

/*.P*/
/***************************************************************************\
*   The following typedefs declare extensions for the file types and for    *
*   PIC1 and PIC4 compression types similar to JPEGINFOHEADER.              *
\***************************************************************************/
#define FC( ch0, ch1, ch2, ch3 )                                \
        ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
        ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

#define BI_JPEG FC('J','P','E','G') /* 'JPEG' compressed (raw ) */
#define BI_JPGE FC('J','P','G','E') /* 'JPEG-ELS' compressed  */
#define BI_JPGL FC('J','P','G','L') /* 'JPEG Lossless */
#define BI_LJPG FC('L','J','P','G') /* PIC2 JPEG Lossless */
#define BI_PJPG FC('P','J','P','G') /* 'PJPG' progressive compressed (raw) */
#define BI_TGA1 FC('T','G','A','1') /* 'TGA1' Targa type 1 (cm) uncompressed */
#define BI_TGA2 FC('T','G','A','2') /* 'TGA2' Targa type 2 (bgr) uncompressed */
#define BI_TGA3 FC('T','G','A','3') /* 'TGA3' Targa type 3 (gray) uncompressed */
#define BI_TGA9 FC('T','G','A','9') /* 'TGA9' Targa type 9 (cm) RLE */
#define BI_TGAA FC('T','G','A','A') /* 'TGAA' Targa type 10 (bgr) RLE */
#define BI_TGAB FC('T','G','A','B') /* 'TGAB' Targa type 11 (gray) RLE */
#define BI_PICJ FC('P','I','C','J') /* 'PICJ' compressed (PIC JPEG) */
#define BI_PICL FC('P','I','C','L') /* 'PICL' compressed */
#define BI_PICP FC('P','I','C','P') /* 'PICP' compressed (PIC prog. JPEG) */
#define BI_PIC1 FC('P','I','C','1') /* 'PIC1' compressed */
#define BI_PIC4 FC('P','I','C','4') /* 'PIC4' compressed */
#define BI_PIC8 FC('P','I','C','8') /* 'PIC8' compressed */
#define BI_PICG FC('P','I','C','G') /* 'PICG' compressed */
#define BI_PICX FC('P','I','C','X') /* 'PICX' compressed */
#define BI_PC2J FC('P','C','2','J') /* 'PC2J' compressed (PIC2 JPEG) */
#define BI_PC2E FC('P','C','2','E') /* 'PC2E' compressed (PIC2 JPEG Els) */
#define BI_PC2L FC('P','C','2','L') /* 'PC2L' compressed */
#define BI_PC2P FC('P','C','2','P') /* 'PC2P' compressed (PIC2 prog. JPEG) */
#define BI_PC2N FC('P','C','2','N') /* 'PC2N' compressed (PIC2 prog. JPEG Els) */
#define BI_PC21 FC('P','C','2','1') /* 'PC21' compressed */
#define BI_PC24 FC('P','C','2','4') /* 'PC24' compressed */
#define BI_PC28 FC('P','C','2','8') /* 'PC28' compressed */
#define BI_PC2G FC('P','C','2','G') /* 'PC2G' compressed */
#define BI_PC2X FC('P','C','2','X') /* 'PC2X' compressed */
/* note BI_RGB is defined in bmp.h */
#define BI_BMPO FC('B','M','P','O') /* 'BMPO' old-style BMP uncompressed */
                                    /* (OS/2 1.1 and 1.2) */
#define BI_BMPR FC('B','M','P','R') /* 'BMPR' new-style BMP RLE */
#define BI_PCX1 FC('P','C','X','1') /* 'PCX1' PCX 1-bit */
#define BI_PCX2 FC('P','C','X','2') /* 'PCX2' PCX 2-bit ('CGA') (cm) */
#define BI_PCX3 FC('P','C','X','3') /* 'PCX3' PCX 3-bit ('EGA') (cm) */
#define BI_PCX4 FC('P','C','X','4') /* 'PCX4' PCX 4-bit (cm) */
#define BI_PCX8 FC('P','C','X','8') /* 'PCX8' PCX 8-bit (cm) */
#define BI_PCXT FC('P','C','X','T') /* 'PCXT' PCX 24-bit */
#define BI_DCXZ FC('D','C','X','Z') /* 'DCXZ' DCX multi-image */
#define BI_TIF1 FC('T','I','F','1') /* 'TIF1' TIFF 1-bit */
#define BI_TIFM FC('T','I','F','M') /* 'TIFM' TIFF color-map (up to 8-bit) */
#define BI_TIFG FC('T','I','F','G') /* 'TIFG' TIFF gray-scale (up to 8-bit) */
#define BI_TIFC FC('T','I','F','C') /* 'TIFC' TIFF rgb (16,24,32-bit) */
#define BI_TIFZ FC('T','I','F','Z') /* 'TIFZ' TIFF multi-image */
#define BI_TIFJ FC('T','I','F','J') /* 'TIFJ' TIFF Jpeg */
#define BI_TIFL FC('T','I','F','L') /* 'TIFL' TIFF LZW */
#define BI_TIFK FC('T','I','F','K') /* 'TIFK' TIFF CMYK */
#define BI_TIFu FC('T','I','F','u') /* 'TIFu' TIFF unknown */
#define BI_GIFN FC('G','I','F','N') /* 'GIFN' GIF non-interlaced */
#define BI_GIFI FC('G','I','F','I') /* 'GIFI' GIF interlaced */
#define BI_GIFu FC('G','I','F','u') /* 'GIFu' GIF unknown */
#define BI_PNG  FC('P','N','G',' ') /* 'PNG ' PNG all flavors */
#define BI_OS2  FC('O','S','2','U') /* 'OS2U' OS2 2.x+ uncompressed */
#define BI_TGA  FC('T','G','A',' ') /* 'TGA ' Targa generic */
#define BI_PCX  FC('P','C','X',' ') /* 'PCX ' PCX generic */
#define BI_TIF  FC('T','I','F',' ') /* 'TIF ' TIFF generic */
#define BI_PCD  FC('P','C','D',' ') /* 'PCD ' PhotoCD generic */
#define BI_G3   FC('G','3',' ',' ') /* 'G3  ' Raw G3 fax -- 1D */
#define BI_G32D FC('G','3','2','D') /* 'G32D' Raw G3 fax -- 2D */
#define BI_UYVY FC('U','Y','V','Y') /* 'UYVY' microsoft uyvy video subtype */
#define BI_YUY2 FC('Y','U','Y','2') /* 'YUY2' microsoft yuy2 video subtype */
#define BI_CMYK FC('C','M','Y','K') /* 'CMYK' 4-byte interlaced */
#define BI_WAVE FC('W','A','V','E') /* 'WAVE' wavelet */

/***************************************************************************\
*   This structure is used to convert to 16-bit DIBs.  Only r5:g5:b5 and    *
*   r5:g6:b5 are supported.                                                 *
\***************************************************************************/

#if 0
The following will not work in 32-bit environment because UINT is 4bytes
rather than the two bytes required. This is intended to be a two-byte
field for 16-bit video modes. If we use WORD here rather than UINT, we are
not ANSI compatible.  So for now we are just going to make RGB555 and
RGB565 be simply words rather than bitfields.
typedef struct {
        UINT    blue  : 5;      /* 00000000 000bbbbb */
        UINT    green : 5;      /* 000000gg ggg00000 */
        UINT    red   : 5;      /* 0rrrrr00 00000000 */
    } RGB555;

typedef struct {
        UINT    blue  : 5;      /* 00000000 000bbbbb */
        UINT    green : 6;      /* 00000ggg ggg00000 */
        UINT    red   : 5;      /* rrrrr000 00000000 */
    } RGB565;
#endif
typedef WORD    RGB555;
typedef WORD    RGB565;
typedef struct {
        BYTE    C;
        BYTE    M;
        BYTE    Y;
        BYTE    K;
    } CMYKQUAD;

/*.P*/
/* This structure is used to reference general-purpose image regions. */
/* PixType determines physical bits per pixel.  PT_CM is 8 bits, PT_RGB */
/* is 24 bits, PT_RGBA and PT_CMYK are 32 bits, PT_GRAY and PT_GRAYM are 8 bits */
/* if Bpp <= 8 or are 16 bits otherwise.  PT_GRAYM if big-endian byte order. */
typedef BYTE    PIXEL_TYPE;
#define PT_NONE     0x00        /* undefined or compressed pixels */
#define PT_CM       0x01        /* color mapped up to 256 colors */
#define PT_GRAY     0x02        /* gray scale up to 16 bits (Intel format) */
#define PT_GRAYM    0x03        /* gray scale up to 16 bits (Motorola format */
#define PT_RGB      0x04        /* RGB 24 bit (blue is low, red is high) */
#define PT_RGBM     0x05        /* RGB 48 bit (blue is low, red is high) (Motorola) */
#define PT_RGB555   0x06        /* RGB 16 bit (xrrrrrgggggbbbbb) */
#define PT_RGB565   0x07        /* RGB 16 bit (rrrrrggggggbbbbb) */
#define PT_CMYK     0x08        /* CMYK 32 bit (cyan is low, black is high) */
#define PT_GRAYA    0x12        /* gray scale up to 16 bits with alpha (Intel) */
#define PT_GRAYAM   0x13        /* gray scale up to 9-16 bits with alpha (Motorola) */
#define PT_RGBA     0x14        /* RGBA 32 bit (blue is low, alpha is high) */
#define PT_RGBAM    0x15        /* RGBA 64 bit (blue is low, alpha is high) (Motorola) */

typedef BYTE    REGION_FLAGS;
#define RF_TopDown  0x01        /* set if image stored top line first, bottom last */
#define RF_NonInter 0x02        /* (unused for now) set if image is non-interleaved */
#define RF_MakeGray 0x04        /* set if region to be treated as gray scale */
#define RF_Channel  0x20        /* set if a single color channel is to be used */
#define RF_ChLo     0x40        /* if RF_Channel is set, ChHi,ChLo are two bits */
#define RF_ChHi     0x80        /* whose value 0..3 indicate which color channel */

/*This structure is used to reference general-purpose image regions. */
typedef struct {
        BYTE         Sig;       /* should be 0 */
        BYTE         Interlace; /* 0 => None, 1 => PNG, 2 => Gif */
        BYTE         BitErr;    /* low-order bits to be dropped or treated as 0 */
        REGION_FLAGS Flags;     /* attributes of region */
        PIXEL_TYPE   PixType;   /* type of pixel which comprise this region */
        BYTE         Bpp;       /* meaningful Bits per pixel */
        WORD         Width;     /* width of region in pixels > 0 */
        WORD         Height;    /* height of region in lines > 0 */
        SHORT        Stride;    /* width of area containing the region in bytes */
        DWORD        Offset;    /* byte offset of logical first line of region */
    } REGION;

/***************************************************************************\
*   These typedefs are used to represent non-callback states.               *
\***************************************************************************/

typedef LONG    OPERATION;
/* OPCODE offsets 0..9 are reserved for std opcodes */
/* OPCODE itself will be offset 10 */
/* MKOP allows the DLL loader to take opcode/1000 to construct a dll name */
#define STD_OPCODE_OFFSET ( 10 )
#define MKOP(x) ( (x) * 1000L + STD_OPCODE_OFFSET )

#define OP_EXP4    (MKOP(21))   /* IM4 - Expand IM4 to a DIB */
#define OP_ROR     (MKOP(22))   /* ROR - Reorient/requantize to Huffman JPEG */
#define OP_RORE    (MKOP(23))   /* RORE - Reorient/requantize to (ELS or Huffman) JPEG */

#define OP_D2S     (MKOP(10))   /* DIB to a Huffman Sequential JPEG */
#define OP_S2D     (MKOP(11))   /* Huffman Sequential JPEG to a DIB */
#define OP_D2SE    (MKOP(12))   /* DIB to Huffman or Els Sequential JPEG */
#define OP_P2D     (MKOP(13))   /* Huffman Sequential or Progressive JPEG to DIB */
#define OP_S2P     (MKOP(14))   /* Transform Seq. JPEG to Prog. JPEG */
#define OP_D2J     (MKOP(15))   /* DIB to Huffman Progressive or Sequential JPEG */
#define OP_P2S     (MKOP(16))   /* Transform Prog. JPEG to Seq. JPEG */
#define OP_D2JE    (MKOP(17))   /* DIB to Huffman or Els Progressive or Sequential JPEG */
#define OP_SE2D    (MKOP(18))   /* Huffman or Els Sequential JPEG to DIB */
#define OP_JE2D    (MKOP(19))   /* Huffman or Els Sequential or Progressive JPEG to DIB */

#define OP_LIMP    (MKOP(42))   /* Lossless IMage Pack */
#define OP_LIME    (MKOP(43))   /* Lossless IMage Expand */
#define OP_LIP     (MKOP(44))   /* Lossless IMage Pack (Ver 2)*/
#define OP_LIE     (MKOP(45))   /* Lossless IMage Expand (Ver 2)*/
#define OP_LIP3    (MKOP(46))   /* Lossless IMage Pack (Ver 3) */
#define OP_LIE3    (MKOP(47))   /* Lossless Image Expand (Ver 3) */
#define OP_PNGP    (MKOP(48))   /* Portable Network Graphics Pack */
#define OP_PNGE    (MKOP(49))   /* Portable Network Graphics Expand */
#define OP_LIP3PLUS (MKOP(62))  /* Lossless Image Pack (Ver 3) + 9-16 bit lossless JPEG */
#define OP_LIE3PLUS (MKOP(63))  /* Lossless Image Expand (Ver 3) + 9-16 bit lossless JPEG */

#define OP_D2LJ    (MKOP(62))   /* temporary pack lossless jpeg */
#define OP_LJ2D    (MKOP(63))   /* temporary expand lossless jpeg */

#define OP_D2F     (MKOP(80))   /* Convert a DIB to supported file format */
#define OP_F2D     (MKOP(81))   /* Convert a supported file format to DIB */
#define OP_UTL     (MKOP(82))   /* Utility functions (dib, palette, histogram) */
#define OP_D2FPLUS (MKOP(84))   /* OP_D2F + GIF */
#define OP_F2DPLUS (MKOP(85))   /* OP_F2D + GIF + LZW/TIFF */
#define OP_ZOOM    (MKOP(89))   /* Change image dimensions */

#define OP_D2W     (MKOP(90))   /* compress DIB to wavelet */
#define OP_W2D     (MKOP(91))   /* expand wavelet to DIB */
#define OP_WSQP    (MKOP(92))   /* Wavelet Scalar Quantization Pack */
#define OP_WSQE    (MKOP(93))   /* Wavelet Scalar Quantization Expand */

#define OP_DEBUG   (MKOP(100))  /* tracing/logging functions */

/* reserved >> */
#define OP_DISPATCH (MKOP(0))
#define OP_QRY   (MKOP( 1))     /* Query a buffer for extensive information */
#define OP_CMAP  (MKOP( 2))     /* Create a Color Map from a buffer */
#define OP_PACKJ (MKOP(10))     /* IMJ - Pack a DIB to Huffman Sequential JPEG */
#define OP_EXPJ  (MKOP(11))     /* IMJ - Expand Huffman Sequential JPEG to a DIB */
#define OP_PACK4 (MKOP(20))     /* IM4 - Pack a DIB to IM4 */
#define OP_PACK1 (MKOP(30))     /* IM1 - Pack a DIB to IM1 */
#define OP_EXP1  (MKOP(31))     /* IM1 - Expand IM1 to a DIB */
#define OP_PACK8 (MKOP(40))     /* IM8 - Pack a DIB to IM8 */
#define OP_EXP8  (MKOP(41))     /* IM8 - Expand IM8 to a DIB */
#define OP_PACKG (MKOP(50))     /* IMG - Pack a DIB to IMG */
#define OP_EXPG  (MKOP(51))     /* IMG - Expand IMG to a DIB */

/*.P*/
/***************************************************************************\
*                                                                           *
*   This structure represents the visual orientation of the image.  It      *
*   is independent of the file's storage orientation (bottom-to-top         *
*   with the bottom of the image stored first in the file or top-to-        *
*   bottom with the top of the image stored first in the file).  Let        *
*   TOP_DOWN be TRUE iff the image is stored top-to-bottom, i.e., the       *
*   biHeight of the image is < 0.  Let VO be the Visual Orientation as      *
*   represented below.  Finally let FO be the File Orientation - the        *
*   orientation field as stored within the PIC file (FO should equal        *
*   VO but for historical reasons it actually indicates whether the         *
*   file should be inverted or not).  Then have:  FO = VO ^ O_inverted.     *
*                                                                           *
*   ÚÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄÂÄÄ¿                                               *
*   ³ 0³ 0³ x³ x³ 0³ x³ x³ x³    visual ORIENTATION values                  *
*   ÀÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÁÄÄÙ                                               *
*     7  6  5  4  3  2  1  0                                                *
*           ³  ³     ³  ³  ÀÄ set if reflected across horizontal line       *
*           ³  ³     ³  ÀÄÄÄÄ set if rotated right 90ø                      *
*           ³  ³     ÀÄÄÄÄÄÄÄ set if rotated right 180ø                     *
*           ³  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄ set if appears white on black                 *
*           ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ set if bit order is reversed                  *
*   Any rotating is done before the reflection.                             *
*                                                                           *
*   If Õ is the standard image then the visual orientations are:            *
*     Õ   Ô   ·   ½   ¾   ¸   Ó   Ö   (if displayed without adjustment)     *
*                                                                           *
*   This structure is also used to indicate the visual characteristics      *
*   of 1-bit images, i.e., IMG (Group 3 and Group 4 fax).                   *
*                                                                           *
\***************************************************************************/
typedef LONG    ORIENTATION;
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


typedef LONG    SUBSAMPLING;
#define SS_111      0
#define SS_211      1   /* Cb and Cr are 2-to-1 subsamp. horiz., not vert. */
#define SS_411      2   /* Cb and Cr are 2-to-1 subsamp. horiz and vert. */
#define SS_211v     3   /* Cb and Cr are 2-to-1 subsampled vert., not horiz. */
                        /* For all other cases subsampling is defined as: */
                        /* HY<<27 | VY<<22 | HCb<<17 | VCb<<12 | HCr<<7 | VCr<<2, */
                        /* where H and V are the subsampling factors defined */
                        /* in JPEG spec. */

typedef DWORD   PICFLAGS;

#define PF_IsGray                   0x00000001L
        /* (Pack) Set if image is gray-scale */
#define PF_TopDown                  0x00000002L
        /* (Exp and Pack) Set if image is top_down (dib buffers then run backwards) */
#define PF_OptimizeHuff             0x00000004L
        /* (Pack, not Els coded) Set if huff codes are to be optimized (meaningless if ELS coded */
#define PF_IsTransparency           0x00000004L
        /* (GIF) image has a transparent color index */
#define PF_IncludeBMPHead           0x00000008L
        /* Include BITMPAINFOHEADER and palette in output stream if appropriate */
#define PF_ReturnYIfFirst           0x00000010L
        /* (Seq Exp) If image is multiscan sequential color lossy jpeg and */
        /* first scan is Y only, return gray image first and defer(RES_Y_DONE_FIRST). */
        /* App can then reset Put buffer before continuing to get color image. */
#define PF_YuvOutput                0x00000020L
        /* (Seq Expand). Causes uyvy or yuy2 output (type chosen in DIB_OUTPUT below) */
#define PF_BigEndian                0x00000040L
        /* (ROR/D2J Exif Motorola byte-ordered tags instead of Intel */
#define PF_ExpandThumbnail          0x00000080L
        /* (J2D) expand exif thumbnail instead of primary image */
#define PF_NoImageChanges           0x00000080L
        /* (ROR) changing Exif tags only -- no change to other image data */
#define PF_ConvertGray              0x00000100L
        /* (Exp)  Set if image is to be gray-scale */
#define PF_IsProtected              0x00000100L
        /* set if PegasusQuery detects that the file is protected */
#define PF_NoDibPad                 0x00000200L
        /* (Exp and Pack)  Set if DIB is not (or not to be) DWORD aligned */
#define PF_Dither                   0x00000400L
        /* (Exp)  Set if image is to be dithered */
#define PF_MakeColors               0x00000800L
        /* (Exp and Pack)  Set if palette is to be made */
#define PF_CreateDibWithMadeColors  0x00001000L
        /* (Exp)  Set if a dib is to be filled with made colors */
#define PF_OnlyUseMadeColors        0x00002000L
        /* (Exp)  Set if ONLY made colors are to be used (no dib output on "first */
        /* pass" using image or user colors) */
#define PF_App1Pal                  0x00004000L
        /* (Pack) Set if palette is to be in APP1 marker code */
#define PF_DibKnown                 0x00008000L    /* obs. name */
#define PF_WidthPadKnown            0x00008000L
        /* (exp and pack) Set if WidthPad is set by app before */
        /* init, NOT to be calc. by level 2 during init. */
#define PF_YieldGet                 0x00010000L
        /* (P&E)  Set if Yield during data get (CB only) */
#define PF_YieldPut                 0x00020000L
        /* (P&E)  Set if Yield during data put (CB only) */
#define PF_Poll                     0x00040000L
        /* (Comm) Set if return instantly when nothing received */
#define PF_NoGets                   0x00080000L
        /* (Exp prog.) Set if no data to be read - just use what is in buffer now. */
#define PF_EOIFound                 0x00100000L
        /* (Exp prog.) Set if JPEG EOI marker reached in progressive. */
#define PF_NoCrossBlockSmoothing    0x00200000L
        /* (Exp prog. and seq) */
#define PF_DecodeToDataEnd          0x00400000L
        /* (Exp prog.) */
#define PF_SynchRestart             0x00400000L
        /* (LL) If a restart error occures, resynch at next marker */
#define PF_MultiImage               0x00800000L
        /* (OP81) -- At RES_DONE, if PF_EOIFound is 0, then: 0 means next dib */
        /* is a progressive image; 1 means next dib is a new image from the */
        /* file (e.g., multi-image TIFF or GIF files) */
#define PF_ElsCoder                0x00800000L
        /* (Pack and ROR and Exp) Use ELS rather than Huffman entropy coding */
        /* For Pack and ROR, this is set by app to determine output. For Exp, it is merely returned by Pegasus describing input. */
#define PF_JPEGFileOnly             0x00800000L
        /* (LL) Do not make PIC2 file for mode J, make JPEG only */
#define PF_ApplyTransparency        0x01000000L
        /* (LL) Don't write transparent color index pixels */
#define PF_UserDelay                0x02000000L
        /* (LL) Mouse or keyboard allows image replacement */
#define PF_AllocateComment          0x04000000L
        /* (LL) RES_ALLOCATE_COMMENT_BUF on every comment */
        /* (LL) RES_ALLOCATE_APP2_BUF on every app */
        /* (LL) RES_ALLOCATE_OVERTEXT_BUF on every overtext */
#define PF_ColorTableProvided       0x08000000L
        /* (LL) Use provided ColorTable, not one in file */
#define PF_UsedMMX                  0x08000000L
        /* (SJPEG) Expand MMX was detected and used */
#define PF_ZoomToSize               0x10000000L
        /* (LL) Zoom all layers to MinLayerWanted size */
#define PF_ContextInterpolation     0x20000000L
        /* (LL) Use best guess for interpolation */
#define PF_DidPolish                0x20000000L
        /* (LL) Able to do all PIC2 forward updates */
#define PF_SkipHuffman              0x40000000L
        /* (LL) Don't calculate optimal Huffman table */
#define PF_HaveColorMap             0x40000000L
        /* (P&E) Must have ColorMap != NULL and colors in color table */
#define PF_UseYCbCrForColors        0x80000000L
        /* (Pack only) Use YCbCr space if making colors - often chooses better
             colors. If not making colors, it is ignored. This flag is not
             used by JPEG expand, as it makes colors from YCbCr always. */
#define PF_HaveWatermark            0x80000000L
        /* (LL) OverText points to watermark not CT */
#define PF_HaveBackground           0x80000000L
        /* (PNG) Background value is available */
#define PF_HaveTransparency         0x40000000L
        /* (PNG) Transparency value is available */
#define PF_BlendWithBuffer          0x20000000L
        /* (PNG) if alpha or transparency, blend color with buffer vs background color */
#define PF_SwapColors               0x10000000L
        /* (PNG) convert RGB to BGR for 24-, 32, 48-, and 64-bit colors */
#define PF_ConvertToColor           0x10000000L
        /* (ROR) convert Grayscale to Color (for lossy jpegs) */
#define PF_MakeBMP                  0x08000000L
        /* (PNG) convert output to a BMP image (set PF_SwapColors, */
        /*  do 16->8 bit conversion, DWORD padding, etc.) */
#define PF_IsBMP                    0x04000000L
        /* (PNG) the source file is BMP */
#define PF_OptimalFilter            0x02000000L
        /* (PNG) the best of filters 0..4 is used for each line */

/* It is suggested that PF2_ variables be used for returned information */
#define PF2_FoundDicom              0x00000001L
        /* (LL JPEG) file was found to be Dicom, may have multiple images */
#define PF2_IsExif                  0x00000001L
        /* (jpeg expand) file has APP1 marker with Exif signature */

typedef DWORD   THUMBNAIL;
#define THUMB_NONE      0
#define THUMB_4         1
#define THUMB_16        2
#define THUMB_64        3
#define THUMB_4F        4 /* use all dct coeffs in making 1/4 area thumbnail */

/***************************************************************************\
*  This typedef designates the currently supported input file types.        *
\***************************************************************************/
typedef LONG    JPEG_TYPE;
#define JT_PIC          0
#define JT_BMP          1
#define JT_RAW          2
#define JT_PIC2         3
#define JT_EXIF         4

typedef LONG    REQUEST;
#define REQ_INIT        1
#define REQ_EXEC        2
#define REQ_CONT        3
#define REQ_TERM        4

/*.P*/
typedef LONG    RESPONSE;
#define RES_DONE                    1
#define RES_EXEC_DONE               1
#define RES_ERR                     2
#define RES_GET_DATA_YIELD          3
#define RES_PUT_DATA_YIELD          4
#define RES_YIELD                   5
#define RES_COLORS_MADE             6
#define RES_PUT_NEED_SPACE          7
#define RES_GET_NEED_DATA           8
#define RES_NULL_PICPARM_PTR        9
#define RES_SEEK                    10
#define RES_AUX_NEEDED              11
#define RES_ALLOCATE_APP2_BUF       12
#define RES_ALLOCATE_COMMENT_BUF    13
#define RES_ALLOCATE_OVERTEXT_BUF   14
#define RES_HAVE_COMMENT            15
#define RES_POKE                    16
#define RES_EXTEND_PIC2LIST         17
#define RES_Y_DONE_FIRST            18
#define RES_PUTQ_GET_NEED_DATA      19
#define RES_INIT_DONE               20
#define RES_PIC_PARM_INIT_DONE      21



/***************************************************************************\
*   Instance specific work area used by level 1 and level 2.                *
\***************************************************************************/
#define SIZE_PIC_STACK  8192

#ifdef __cplusplus
  typedef struct {
        BYTE        reserved;
  } WORK_AREA;
#else
  typedef struct {
        jmp_buf     AppState;
        jmp_buf     PicState;
        BYTE        PicStack[SIZE_PIC_STACK];
        BYTE        InstState[564];
        BYTE        Padding[4];
        BYTE        WorkArea[1];
            /* other stuff - should never allocate sizeof(WORK_AREA) bytes! */
  } WORK_AREA;
#endif

typedef struct {
        BYTE PICHUGE *FrontEnd; /* Auxiliary, points just beyond contiguous data */
        BYTE PICHUGE *Start;    /* Points to start of queue */
        BYTE PICHUGE *Front;    /* Points to start of data in the queue */
        BYTE PICHUGE *Rear;     /* Points to start of empty space in the queue */
        BYTE PICHUGE *End;      /* Points just beyond queue buffer */
        BYTE PICHUGE *RearEnd;  /* Auxiliary, points just beyond contiguous space */
        DWORD        QFlags;    /* Flags for queue operation */
    } QUEUE;

#define Q_EOF       1           /* Set iff End-of-file reached on input */
#define Q_REVERSE   2           /* Set iff queue is reversed */
#define Q_IO_ERR    4           /* Set iff input or output error occurred */
#define Q_READ_WRAP 8           /* Set iff the Rear pointer has wrapped to Start */
#define Q_DID_WRITE 16          /* Set for Put iff RES_PUT_NEED_SPACE defer has been issued */
#define Q_DID_READ  16          /* Set for Get iff RES_GET_NEED_DATA defer has been issued */
#define Q_READING   32          /* set while level 2 is reading from Put queue, else clear */
#define Q_INIT      64          /* indicates initialization should be done in the queue */

typedef struct {
        LONG         Width;     /* Width of image in pixels for given layer */
        LONG         Height;    /* Height of image in pixels for given layer */
        LONG         Stride;    /* Width of image buffer in bytes for given layer */
        LONG         Size;      /* Compressed size of layer (BYTES) */
        BYTE PICFAR *Handle;    /* Level 2 pointer to a buffer */
    } LAYER;

/*.P*/
/***************************************************************************\
*   Variables to/from PegasusQuery                                          *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        /*NOTE: this is used by PegasusQuery -- there is NO OPCODE for query */
        BYTE PICHUGE *Reserved1;    /* Must be NULL! */
        BYTE PICHUGE *Reserved2;    /* Must be NULL! */
        BYTE PICHUGE *Reserved3;    /* Must be NULL! */
        BYTE PICHUGE *Reserved4;    /* Must be NULL! */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* Only PF_IsGray is relevant */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         NumOfPages;    /* # of pages in document (G3/G4) */
        LONG         PageNum;       /* Page # of 1st (or only) page (G3/G4) */
        DWORD        BitFlagsReq;   /* Bitmapped flags to indicate which */
                                    /*   values PegasusQuery should try to */
                                    /*   ascertain.  Set by the user. See */
                                    /* QBITxxx below. */
        DWORD        BitFlagsAck;   /* Bitmapped flags to indicate which */
                                    /*   of the requested values PegasusQuery */
                                    /*   was able to ascertain.  Note that */
                                    /*   some fields may be determined, and */
                                    /*   the resulting bits set, even if not */
                                    /*   specifically requested. */
                                    /* See QBITxxx below. */
        DWORD        ImageSize;     /* Size of image.  See QBIT_IMAGESIZE below. */
        DWORD        AuxSize;       /* Size required to hold all auxillary */
                                    /*   data blocks. */
        DWORD        ImageNum;      /* Image number we're requesting query on */
                                    /*   (1 is 1st image) -- this is ignored */
                                    /*   for files that only contain 1 image */
                                    /*   (bmp, pcx, etc and single-image */
                                    /*   tif, gif, etc) */
        DWORD        NumImages;     /* Number of images in file, if known */
        DWORD        SOIMarker;     /* Offset in file of SOI marker if tiff jpeg */
        SUBSAMPLING  SubSampling;   /* if determinable and JPEG */
        REGION       Region;
        BYTE         LumFactor;     /* if present and JPEG */
        BYTE         ChromFactor;   /* if present and JPEG */
        BYTE         AllowedBitErr; /* where significant */
    } PEGQUERY;

#define QBIT_BISIZE 0x1
        /* Head.biSize -- this is the size of the bitmap info header.  It */
        /* will generally be "sizeof (BITMAPINFOHEADER)", except in the */
        /* case of .BMP files, where it may be larger. */
#define QBIT_BIWIDTH 0x2
        /* Head.biWidth -- this is the width of the image in pixels. */
#define QBIT_BIHEIGHT 0x4
        /* Head.biHeight -- this is the height of the image in pixels. */
        /* This value may be negative.  If so, it indicates that if the */
        /* image is converted to a DIB via opcode 81, it will be upside */
        /* down.  (Opcode 81 may generate upside-down DIBS.) */
#define QBIT_BIPLANES 0x8
        /* Head.biPlanes -- this will always be 1. */
#define QBIT_BIBITCOUNT 0x10
        /* Head.biBitCount -- total number of bits per pixel for the */
        /* image.  May be wierd (e.g., 7).  When opcode 81 generates a */
        /* DIB, it will create only 1, 4, 8, 16, or 24 bpp images. */
#define QBIT_BICOMPRESSION 0x20
        /* Head.biCompression -- this is the basic file type.  See the */
        /* BI_???? manifest constants near the top of this file. */
#define QBIT_BISIZEIMAGE 0x40
        /* Head.biSizeImage -- this is the size of the image data if */
        /* converted to a DIB.  Pixels will be padded out to 1, 4, 8, */
        /* 16, or 24 bpp, and scanlines will be padded out to a 4-byte */
        /* boundary. */
#define QBIT_BIXPELSPERMETER 0x80
        /* Head.biXPelsPerMeter -- generally of no use */
#define QBIT_BIYPELSPERMETER 0x100
        /*Head.biYPelsPerMeter -- generally of no use */
#define QBIT_BICLRUSED 0x200
        /* Head.biClrUsed -- a RGB image may have any value here, or */
        /* 0 (if non-zero, this is presumably some sort of "suggested" */
        /* palette).  For palettized images, this is the number of colors */
        /* in the palette and MUST be >=2 and <= (1 << bpp). */
#define QBIT_BICLRIMPORTANT 0x400
        /* Head.biClrImportant -- generally of no use. */
#define QBIT_IMAGESIZE 0x800
        /* ImageSize -- size of the input "file".  This is the number of */
        /* bytes, starting from the beginning of the "file", which must be */
        /* accessed in order to get all header information and image data */
        /* in order to display an image.  Therefore, other auxillary */
        /* information that may be present in the file, but not required */
        /* for image display (e.g., author, comments, etc) will not be */
        /* included in this size.  Note that for 'random' formats such as */
        /* TIFF, it is possible that this space could contain information */
        /* for other images. */
#define QBIT_AUXSIZE 0x1000
        /* AuxSize -- size of the buffer required to read in all */
        /* 'auxillary' information (comments, gamma curves, etc.) in */
        /* preparation for an opcode 81 call.  Note that opcode 81 does */
        /* not require that this be known in advance, but things are more */
        /* efficient if it is known. */
#define QBIT_NUMIMAGES 0x2000
        /* NumImages -- number of images in the file.  Most often, 1. */
        /* This count may not be obtainable with from a small amout of data. */
#define QBIT_COMMENT 0x4000
        /* Iff Comment field set in PIC_PARM */
#define QBIT_PALETTE 0x08000L
        /* Set this if you want the palette returned in ColorTable */
        /* if possible. */
#define QBIT_SOIMARKER 0x10000L
                /* Set if we found SOIMarker in a tiff jpeg file */
#define QBIT_PICJPEG 0x20000L
#define QBIT_REGION  0x40000L
        /* set if u.QRY.Region is filled in */

/* for progressive JPEG */
typedef struct {
        /* NOTE: Co = 3 (interleaved yCbCr) is allowed ONLY for dc scans. */
        /* Also, for now, the dc scans MUST be interleaved for color image) */
        LONG   Co; /* Component indicator(0=y, 1=Cb, 2=Cr, 3=interleaved YCbCr) */
        LONG   Ss; /* dct coeff. index of start of scan */
        LONG   Se; /* dct coeff. index of end of scan ( 0 indicates dc) */
        LONG   Ah; /* (see JPEG spec) */
        LONG   Al; /* (see JPEG spec) */
        } SCAN_PARM;


/***************************************************************************\
*   Variables for reading DIBS (Operation D2S)                              *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        /*  Information prior to REQ_INIT */
        BYTE PICHUGE *AppField;     /* points to user info of AppLen bytes */
        BYTE PICHUGE *QTable;       /* NULL or points to Q-table values;
                                     Not in zig_zag order! There only need be   64 bytes for
                                     grayscale input.  For color,   first 64 bytes are for
                                     luminance(Y) and the   next 64 for Cb. If this is
                                     followed by a zero byte, the same 64 are used for Cr
                                     as were for Cb.  If not, it is assumed that the 64
                                     bytes following the 64 Cr quant.   bytes are used for Cb.
                                     Thus in the color case, this table must be at least 129
                                     bytes, and 192 bytes if there are 3 different quant. tables. */
        SCAN_PARM PICHUGE *ScanParms;    /* For progressive JPEG */
        BYTE PICHUGE *ExifThumbnail; /* pointer to Jfif-format thumbnail image for Exif output if
                                     desired.  Error unless unless JpegType == JT_EXIF &&
                                     ExifThumbnailToMake == THUMB_NONE */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* PF_IsGray, PF_IsNotDibPad, PF_OptimizeHuff */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         PrimClrToMake; /* Desired number of colors in primary palette */
        LONG         SecClrToMake;  /* Desired number of colors in secondary palette */
        LONG         LumFactor;     /* Luminance compression factor 0..255.
                                    Ignored if QTable above is not NULL. */
        LONG         ChromFactor;   /* Chrominance compression factor 0..255 */
        SUBSAMPLING  SubSampling;   /* SS_field, 111/211/211v/411 subsampling */
        JPEG_TYPE    JpegType;      /* JT_ fields, desired PIC/BMP/RAW/PIC2 JPEG file type. JT_BMP not supported. */
        LONG         AppFieldSize;  /* Size of AppField in bytes. */
        LONG         AppFieldLen;   /* 0 or length of data within AppField */
        LONG         NumOfPages;    /* Number of pages in document */
        LONG         PageNum;       /* Page Number of first (or only) page */
        LONG         Context;       /* number of context pts (IM1, IM4, & IM8) */
        /*  Derived information about the Uncompressed Image after REQ_INIT */
        LONG         StripSize;     /* Minimum size of buffer to hold a strip */
        LONG         WidthPad;      /* Actual byte size of UI line (with any pad) */
        LONG         NumProgScans;  /* for progressive JPEG */
        LONG         ExifAppsToKeep;
            /* bit 0 to output JFIF App0 marker
               bit 1 to output PIC App1 maker (not recommended) */
        DWORD     ExifThumbnailLen;
            /* length of jiff thumbnail provided at ExifThumbail.  Error unless
                JpegType == JT_EXIF && ExifThumbnailToMake == THUMB_NONE */
        THUMBNAIL ExifThumbnailToMake;
            /* != THUMB_NONE to create a thumbnail in Exif output.
                (requires and uses OP_S2D or OP_SE2D and OP_D2S or OP_D2SE)
                error unless JpegType == JT_EXIF && ExifThumbnailTomake == THUMB_NONE */
        } DIB_INPUT;

/*.P*/
/***************************************************************************\
*   Variables for writing DIBs (Operations S2D, P2D)                   *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        /*  Information prior to REQ_INIT */
        BYTE PICHUGE *AppField;     /* points to user info of AppLen bytes */
        BYTE PICHUGE *MadeColorTable;
        BYTE PICHUGE *PrecisionReq; /* PJPEG - 192 byte array of precision requested */
        BYTE PICHUGE *ColorMap;     /* points to pre-allocated 32K area for ColorMap */
        BYTE PICHUGE *ExifThumbnail;/* exif thumbnail buffer of ExifThumbnailLen */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* PF_MakeGray, PF_NoDibPad, PF_Dither etc*/
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         DibSize;       /* 1/4/8/16/24/32, may change if gray image */
        THUMBNAIL    Thumbnail;     /* 0/1/2/3 => none / 1/4 / 1/16 / 1/64 thumbnail */
        LONG         NumScansReq;   /* PJPEG User's requested number of scans */
                                    /*   (0 means do all) */
        LONG         NumScansDone;  /* PJPEG Scans actually done upon return */
        LONG         NumBytesDone;  /* PJPEG Bytes comprising the scans returned */
        LONG         GraysToMake;   /* if > 0 then number of gray levels to make */
        LONG         PrimClrToMake; /* Desired number of colors in primary palette */
        LONG         SecClrToMake;  /* Desired number of colors in secondary palette */
        /*  Derived information about the Uncompressed Image after REQ_INIT */
        LONG         StripSize;     /* Minimum size of buffer to hold a strip */
        LONG         WidthPad;      /* Actual byte size of UI line (with any pad) */
        LONG         LumFactor;     /* Luminance compression factor 0..255 */
        LONG         ChromFactor;   /* Chrominance compression factor 0..255 */
        SUBSAMPLING  SubSampling;   /* SS_field, 111/211/211v/411 subsampling */
                                    /*   (returned by Pegasus) */
        JPEG_TYPE    JpegType;      /* (returned by Pegasus). JT_ fields, PIC/BMP/RAW/PIC2 JPEG file type (JT_BMP not supported)*/
        LONG         AppFieldSize;  /* Size of AppField in bytes. */
        LONG         AppFieldLen;   /* 0 or logical length of Application data */
        LONG         NumOfPages;    /* Number of pages in document */
        LONG         PageNum;       /* Page Number of first (or only) page */
        LONG         Context;       /* number of context pts (IM1, IM4, & IM8) */
        LONG         DitherType;    /* 0 = Floyd-Steinberg, 1 = fast-error-diffusion */
        LONG         YuvOutputType; /* BI_UYVY or BI_YUY2.  Ignored unless PF_YuvOutput is set. */
        LONG         ExifThumbnailLen; /* size of ExifThumbnail buffer */
        } DIB_OUTPUT;


/******************  For Wizard ***********************************/

#define MAX_REGIONS 16
/*This structure is used for regions in wizard (see REORIENT structure). */
typedef struct {
                WORD         LumFactor; /* 0-255 gives quality as in compress, can go beyond 255 here. */
                WORD         ChromFactor;
                LONG         NumTbls; /* Number of quant. tables to be supplied, or a code.
                                         255-> do not change quantization from existing,
                                         0->use luminance and chrominance values instead of supplying q-tables,
                                         1->grayscale,2 or 3 -> color. */
                WORD         QTbls[1]; /* If NumTbls != 0 or 255, there should actually be
                                         64*NumTbls words here. The 1 is a dummy. */
        } REGION_INFO;


/***************************************************************************\
*   Variables for re-orienting JPEG files (Operation ROR)                   *
\***************************************************************************/
typedef struct {
        DWORD         Reserved0;
		BYTE PICHUGE *RegionInfo; /* Null unless doing regions.
									 Put in data according to REGION_INFO structure above,
									 for each of NumRegions regions (concatenated). After that, put in
									 RegionMap, at offset RegionMapOffset from beginning of RegionInfo.
									 RegionMap is an array of bytes, corresponding to the 8x8 blocks in
									 the image.  The value in each byte,in row scan order, tells
									 which region number the corresponding 8x8 block is in.  The
									 region numbers are from 0 through NumRegions - 1. RegionMap
									 has width and height given below in RegionMapWidth and
									 RegionMapHeight. */
		BYTE PICHUGE *QTableReq;    /* (optional) Ignored unless Requantize
									 below is set to 1, or JoinFlags & JF_DoJoin != 0 and
									 JoinFlags & JF_UseRequestedQuantization != 0. Allocated
									 and set by user.	Points to Q-table values, if used.
									 Not in zig_zag order! There only need be	64 bytes for
									 grayscale input.  For color,	first 64 bytes are for
									 luminance(Y) and the	next 64 for Cb. If this is followed
									 by a	zero byte, the same 64 are used for Cr as	were for Cb.
									 If not, it is assumed that the 64 bytes following the 64
									 Cr quant. bytes are used for Cb.  Thus in the color case,
									 this table must be at least 129 bytes, and 192 bytes if
									 there are 3 different quant. tables. */
		BYTE PICHUGE *ExifThumbnail; /* pointer to Jfif-format thumbnail image for Exif output if
                                     desired.  Error unless unless JpegType == JT_EXIF &&
                                     ExifThumbnailToMake == THUMB_NONE */
		BYTE PICHUGE *Reserved4;    /* Must be NULL! */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
		PICFLAGS     PicFlags;
		PICFLAGS     PicFlags2;
        LONG         Pad;           /* 1-> pad to fill mcu if needed, 0->trim ,
                                     2-> pad width only, 3-> pad height only(in case of rotation,
                                     width and height here refer to original image). */
		LONG         KeepColors;    /* 1->put colors from pic file in APP1,0->discard */
		LONG         LumFactorReq;  /* (optional) Ignored unless QTableReq is
									 NULL and: Requantize	below is set to 1, or	 JoinFlags &
									 JF_DoJoin != 0 and	JoinFlags & JF_UseRequestedQuantization
									 != 0. Luminance compression factor 0..255 */
		LONG         ChromFactorReq; /* Same comment as above. Chrominance compression factor 0..255 */
		LONG         Requantize;    /* Set by User. 1-> change the quantization interval size
									for each DCT coefficient to the odd multiple
									of the existing value which is nearest to
									the value in QTableReq if this is present,
									or to the one implied by LumFactorReq and
									ChromFactorReq otherwise. This produces the
									same quantized DCT coefficients that would
									have been obtained starting from the raw
									image using this quantization.
									2-> for each DCT coefficient, use requested
									quantization value when larger than one in
									input file, otherwise use one in input file.
									NOTE: this does not produce same jpeg file
									as if we had started with the raw image
									using this quantiztion! Use at your own risk.
									3-> for each DCT coefficient, use requested
									quantization value, period.
									0-> do not requantize  */
		LONG         RequantizationDone;  /* set by Pegasus if Requantize is
									1. 1->some change was made to quantization,
									0-> no change was made. */
		LONG         AppsToKeep;    /* The default is to remove all App data segments
									except JFIF and PIC. The bit positions on in
									AppsToKeep, numbered from least significant to
									most, tell which App data segments to keep,
									other than JFIF and PIC, for positions 0,1,...15.
									For example, if AppsToKeep = 0xC (1100 binary),
									this means keep App2 and App3 data segments.
									JFIF is removed only if bit position 16 is ON,
									and PIC is removed only if bit position 17 is ON. */
		LONG         RemoveComments; /* if the 0th bit is on, remove the existing
									comments (if not, keep all comments).
									If the 1st bit is on, do NOT add the comment
									Pegasus Imaging Corp (if not, add this
									comment, even if bit 0 is on, unless a comment
									whose first 15 characters are Pegasus Imaging
									already remains in the file). */
		JPEG_TYPE    JpegType;      /* Output type(set by application). Only JT_RAW and JT_PIC2 are supported */
		BYTE         OutputKeyField[8];
				/* ^ Output encode key -- currently only for JpegType == JT_PIC2 &&
								if OutputKeyField[0] != 0.  Eventually also for JpegType ==
								JT_RAW && OutputKeyField[N] != 0 for any N = 0, .., 7 */
		LONG    JoinOffset; /* offset from Get.Start to the start of
					a second image in Get buffer to be joined to first (see JoinFlags
					below). If this is 0,	Pegasus will assume offset is unknown and
					will scan to attempt to find it. The Get buffer must be large
					enough to hold ALL of the	first image and up through the first scan
					header of the second image. */
		DWORD   JoinFlags;  /* These flags are set to cause two images to be joined
					 in manner desired. See the defined values for JoinFlags below. */
		DWORD   InsertTransparencyLum; /* A value between 0 and 256, inclusive, which
					determines how much of the luminance comes from the pixels of the original
					image when inserting an image.  A value of 0 means the INSERTED
					image determines the luminance in its	region.  A value of 256 means the
					the original image completely determines the luminance.
					This has meaning only when JF_Insert is set (see below). */
		DWORD   InsertTransparencyChrom; /* A value between 0 and 256, inclusive, which
					determines how much of the chrominance comes from the pixels of the original
					image when inserting an image. See above.
					This has meaning only when JF_Insert is set (see below). */
		LONG	NumRegions; /* 1 or greater to have effect; <= MAX_REGIONS */
		DWORD	RegionMapOffset; /* offset from RegionInfo to RegionMap. */
		WORD	RegionMapWidth; /* Must be >= (image_width + 7)/8.  Any extra columns skipped over */
		WORD	RegionMapHeight; /* Must be >= (image_height + 7)/8. Any extra rows ignored */
		/* The following color space transforms are not allowed if doing requantize,join, or regions. */
		SHORT   YShift; /* 0 if not transforming luminance (Y); -256 ... 255 otherwise. */
		/* YShift represents increase(decrease if < 0) in brightness. */
		SHORT	YScale; /* 0 if not transforming luminance (Y); -128 ... 127 otherwise. */
		/* YScale represents increase(decrease if <0) in contrast.  Ususally the
		contrast needs to be increased if the brightness is increased, so these two
		are often used together. */
		SHORT   CbShift; /* 0 if not transforming chrominance; -256 ... 255 otherwise. */
		SHORT	CbScale; /* 0 if not transforming chrominance; -128 ... 127 otherwise. */
		SHORT   CrShift; /* 0 if not transforming chrominance; -256 ... 255 otherwise. */
		SHORT	CrScale; /* 0 if not transforming chrominance; -128 ... 127 otherwise. */
        DWORD     ExifThumbnailLen;
            /* length of jiff thumbnail provided at ExifThumbail.  Error unless
                JpegType == JT_EXIF && ExifThumbnailToMake == THUMB_NONE */
        THUMBNAIL ExifThumbnailToMake;
            /* != THUMB_NONE to create a thumbnail in Exif output.
                (requires and uses OP_S2D or OP_SE2D and OP_D2S or OP_D2SE)
                error unless JpegType == JT_EXIF && ExifThumbnailTomake == THUMB_NONE */
		} REORIENT;

		/* settings for JoinFlags for join operation above( these are "or"ed with JoinFlags). */
#define JF_DoJoin    0x00000001  /* Set if two images are to be joined. All
		other join flags are ignored if this is not set.  This is NOT allowed
		unless: Requantize is zero, F_Crop is NOT set in PicParm Flags, and
		VisualOrient is zero in PicParm. */
#define JF_LeftRight 0x00000002  /* Set if images are to be joined left-right
		rather than top-bottom(the default is top-bottom) */
#define JF_SecondOnTopLeftInsert 0x00000004  /* Set if second image in Get buffer
	 goes on top or left or inserted; default is first is top or left or inserted. */
#define JF_UseSecondSubsampling 0x00000008  /* Set if subsampling is to be
		taken from second image in Get buffer; default is from first image.
		WARNING: If the two images do not	have the same subsampling, some inverse
		and forward DCT operations are required	on the chrominance components. */
#define JF_UseRequestedQuantization 0x00000010  /* Set if quantization of
		joined image is to be input by app, either by supplying a non-null
		pointer for	QTableReq above; if that is null, Pegasus will use
		LumFactorReq and ChromFactorReq above. */
#define JF_UseSecondQuantization 0x00000020  /* This is ignored if
		JoinFlags & JF_UseRequestedQuantization != 0.  Set this if quantization
		is to be taken from second image in get buffer; the default is to take
		quantization from the first. */

		/* WARNING: if the joined image quantization is different from the input
		quantization for either image, some new error may be introduced, larger
		than would have been expected from the indicated quantization table.
		Even when an output quantization interval is larger than the input one,
		the error in a coefficient is no longer guaranteed to be no more than half
		of the output quantization interval size.	This has to do with rounding. */

#define JF_Insert    0x00000040  /* Set if one image is to be inserted into
		the other rather than placed side-by-side. NOTE: This is considered a
		version of joining two images, so JF_DoJoin must also be set in order for
		this to have meaning.  The first image in the Get	buffer will be
		inserted in the second image unless the flag JF_SecondOnTopLeftInsert
		is set.  If JF_Insert is set, then CropXoff and CropYoff in the PIC_PARM
		stucture must be set;  this gives the location of where to put the upper
		left corner of the inserted image.  This will be rounded down to the nearest
		multiple of MCU width and height of the surrounding image.  The width and
		height of the inserted image will be rounded down to a multiple of MCU width
		and height, and made to fit in the surrounding image if too large. Also
		if JF_Insert is set, InsertTransparencyLum and InsertTransparencyChrom
		must be given a value between 0 and 256 inclusive (see above). */

		/* Settings for PicFlags2 above */
#define PF2_ClipDc	0x00000001  /* not operational at this time */
#define PF2_ClipAc	0x00000002  /* not operational at this time */

/***************************************************************************\
*   Variables for converting Seq to Progressive JPEG (Operations S2P)       *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        /*  Buffer information */
        SCAN_PARM PICHUGE *ScanParms; /* For progressive JPEG */
        BYTE PICHUGE *Reserved2;    /* Must be NULL */
        BYTE PICHUGE *Reserved3;    /* Must be NULL */
        BYTE PICHUGE *Reserved4;    /* Must be NULL */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         NumProgScans;  /* for progressive JPEG */
        } TRANS2P;

/***************************************************************************\
*   Variables for converting Progressive to Seq JPEG (Operation P2S)        *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        /*  Buffer information */
        BYTE PICHUGE *Reserved1;    /* Must be NULL! */
        BYTE PICHUGE *Reserved2;    /* Must be NULL */
        BYTE PICHUGE *Reserved3;    /* Must be NULL */
        BYTE PICHUGE *Reserved4;    /* Must be NULL */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         NumScansReq;   /* num progressive JPEG scans to decode
                                        (0 means do all) */
        LONG         NumScansDone;  /* PJPEG Scans actually done upon return */
        LONG         NumBytesDone;  /* PJPEG Bytes comprising the scans returned */
        SUBSAMPLING  SubSampling;   /* SS_field, 111/211/211v/411 subsampling
                                        (returned by Pegasus) */
    } TRANP2S;

/*.P*/
/***************************************************************************\
*   Info is already in the PIC_PARM structure                               *
\***************************************************************************/
/* INTERNAL must be the largest union variant */
typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE *ReservedPtrs[8]; /* Must be NULL! */
        PICFLAGS     PicFlags;
        PICFLAGS     PicFlags2;
        DWORD        Reserveds[64];
    } INTERNAL;

/*.P*/
/***************************************************************************\
*   Converting supported file formats to DIBs (F2D) (opcode 81)             *
\***************************************************************************/

#define OP8X_NPOINTERS  20          /*way more than enough */
#define OP8X_NVARS      20          /*way more than enough */
typedef struct {
        BYTE PICFAR *ptr8X[OP8X_NPOINTERS];
        DWORD vars[OP8X_NVARS];
        } OP8X_WORK_AREA;
/* This is the work area (in p->Reserved) for opcodes 80,81,82.  It basically */
/* consists of a number of malloc'ed pointers that all get released */
/* at PegasusTerm time.  Currently, vars[0] is a flag which, if set, */
/* tells REQ_EXEC to do a REQ_CONT (but only for opcode 81) -- this is so */
/* the app that calls Pegasus can call REQ_EXEC for multiple dibs, but */
/* REQ_CONT will actually be executed.  The area must obviously initially */
/* be nulled out. */

typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE *AuxSpace;
                /* If auxillary data (e.g., Author, etc) is to be retained, this is a */
                /* pointer to the area to contain these chunks.  If the application */
                /* is not interested in these chunks, this pointer must be NULL.  The */
                /* application is responsible for allocating this space.  If there is */
                /* not enough room to hold the data, the response RES_SPACE_NEEDED */
                /* will result, which means that AuxNeeded (below) additional bytes */
                /* need to be allocated.  The current allocated size of this buffer */
                /* must be set in AuxSize below, and maintained each time it is */
                /* increased by a response to RES_SPACE_NEEDED. */
        BYTE PICHUGE *Ptr2;    /* Must be NULL!, except for PhotoCD, in which */
                /* case it must point to the filename for the PhotoCD file */
        BYTE PICHUGE *Reserved3;    /* Must be NULL! */
        BYTE PICHUGE *Reserved4;    /* Must be NULL! */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
                /*NOTE: if any of the following fields are not applicable, */
                /* they should be set to 0 */
        PICFLAGS     PicFlags;      /*PF_EOIFound, PF_IsGray, PF_IncludeBMPHead */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        DWORD        AllocType;
                /* 0 if input is to be processed in chunks, 1 if input will be */
                /* available in its entirety (avoids possible need for seeking) */
        DWORD        AuxSize;
                /* current allocated size of AuxSpace; set by the application */
        DWORD        AuxUsed;
                /* how many bytes of AuxSpace are used up.  Should be set to 0 by */
                /* the application before the opcode is "called". */
        DWORD        AuxNeeded;
                /* needed additional space; set by opcode before returning with */
                /* RES_AUX_NEEDED.  If the application continues (REQ_CONT) and */
                /* AuxSize is unchanged, it is assumed that the application requests */
                /* that any other auxillary chunks be tossed. */
        DWORD        ApplyResponse;
                /* ignored unless PNG or TIFF.  0=don't apply gamma correction or */
                /* gray response to resulting DIB; 1=do */
        DWORD        ProgressiveMode;
                /* only applies if input file is progressive: */
                /* 0=create increasingly larger DIBs */
                /* 1=create same size DIBS, increasing clarity (just pixel */
                /*   replication) -- with only data from current pass */
                /* 2=return only final DIB */
                /* 3=create same size DIBS, increasing clarity (just pixel replica- */
                /*   tion) -- merging data from each pass.  Seeking of the output */
                /*   queue may be required if the output queue is not large enough */
                /*   to hold the entire image. */
                /* 4=create same size DIBS, smoothing, with only data from current pass */
                /* 5 = create same size DIBS, smoothing, merging data from each pass. */
                /*   Seeking of the output queue may be required if the output queue */
                /*   is not large enough to hold the entire image. */
        BITMAPINFOHEADER BiOut;     /* this is the bi header of the output dib */
        DWORD        YieldEvery;    /* If PF_Yield is set, yield after putting this many scanlines in the
                                        output queue */
        DWORD        PhotoCDResolution; /* unused and should be 0, except for PhotoCD,
                                        in which case it is: 0: 64x96; 1: 128x192; 2: 256x384; 3: 512x768;
                                        4: 1024x1536; 5: 2048x3072; 6: 4096x6144 */
        LONG         Compression;   /* TIFF (0, 1-none, 2-Modified G3, 3-G31D, -3-G32D, 4-G4, 5-LZW, 32773-PackBits) */
        BYTE         TransparentColorIndex;/* if GIF && PF_IsTransparency */
        BYTE         RawG3FillOrder; /* != 1 for raw fax TIFF byte FillOrder == 2, otherwise FillOrder == 1 */
        BYTE         RawG3PhotometricInterpretation; /* != 0 for raw fax BlackIsZero, otherwise WhiteIsZero */
        BYTE         Expansion3d;   /* unused and should be 0 */
        DWORD        Expansion4;    /* unused and should be 0 */
        DWORD        Expansion5;    /* unused and should be 0 */
        DWORD        Expansion6;    /* unused and should be 0 */
        DWORD        Expansion7;    /* unused and should be 0 */
        DWORD        Expansion8;    /* unused and should be 0 */
        DWORD        Expansion9;    /* unused and should be 0 */
        DWORD        Expansion10;   /* unused and should be 0 */
    } F2D_STRUC;

/*.P*/
/***************************************************************************\
*   Converting DIBs to supported file formats (D2F) (opcode 80)             *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE *AuxSpace;
                /* If auxillary data (e.g., Author, etc) has been retained from an */
                /* opcode 81 call, this is a pointer to the area that contains these */
                /* chunks.  The size of this buffer must be set in AuxSize below. */
        BYTE PICHUGE *Reserved2;    /* Must be NULL! */
        BYTE PICHUGE *Reserved3;    /* Must be NULL! */
        BYTE PICHUGE *Reserved4;    /* Must be NULL! */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
                /* NOTE: if any of the following fields are not applicable, they */
                /* should be set to 0 */
        PICFLAGS     PicFlags;      /*only PF_IncludeBMPHead */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        DWORD        AllocType;
                /* 0 if input is to be processed in chunks, 1 if input will be */
                /* available in its entirety (avoids possible need for seeking) */
        DWORD        AuxSize;
                /* allocated size of AuxSpace; set by the application */
        DWORD        ImageType;
                /* output image type requested: must be in one of these families: */
                /* BI_BMPx, BI_TGAx, BI_PCXx, BI_TIFx, BI_PNGx */
        DWORD        Compression;
                /* type of compression:
                    0=none (BMP, TGA, TIFF)
                    1=rle (BMP, PCX) none TIFF)
                    2=lzw (PNG), Modified G3 (TIFF)
                    3=G3 (TIFF) 1-D & 2-D
                    4= G4 (TIFF) */
        DWORD        PNG_Progressive;
                /* PNG only: 0=sequential, 1=adam7 progressive */
        DWORD        PNG_Filter;
                /* PNG filter option: 0=none, 1=sub, 2=up, 3=average, 4=paeth, */
                /* 1000="optimal" (computed optimally for each scanline) */
        DWORD        ApplyAux;
                /* 0=discard AuxSpace data; 1=map chunks only if they map exactly to */
                /* the output file; 2=map chunks to most reasonable output chunks */
        DWORD        OutBpp;
                /* only used when creating Targa files and the input dib size is 24; */
                /* in that case, OutBpp may be either 16 or 24 */
        DWORD        Expansion1;    /* unused and should be 0, except for TIF
                                        ( Expansion1 & 0xff000000 ) == 0 for intel byte order output
                                        ( Expansion1 & 0xff000000 ) == 1 for motoroloa byte order output
                                        if the input is 32-bpp CMYK:
                                            ( Expansion1 & 0xff ) == 0 for CMYK output
                                            ( Expansion1 & 0xff ) == 1 for K output only from input CMYK
                                        if the input is 32-bpp CMYK and ( Expansion1 & 0xff ) == 0
                                            ( Expansion1 & 0x0000ff00 ) is output as DotRange[0]
                                            ( Expansion1 & 0x00ff0000 ) is output as DotRange[1] */
        DWORD        Expansion2;     /* unused and should be 0, except for TIF
                                        ( Expansion & 0xff ) == 0 don't output X/Y resolution tags
                                                             == 1 output in inches
                                                             == 2 output in centimeters */
        DWORD        MultiImageSize; /* for TIF, optional input image size for appending to an existing image */
        BYTE         TransparentColorIndex; /* if GIF && PF_ApplyTransparency */
        BYTE         NegateImage;   /* for TIF, G3 or G4, output complement of image if != 0 */
        BYTE         Dimension;     /* for TIF with Compression=3, 0=1-D, 2=1-D */
        BYTE         Expansion4d;   /* unused and should be 0 */
        DWORD        Expansion5;    /* unused and should be 0 */
        DWORD        Expansion6;    /* unused and should be 0 */
        DWORD        Expansion7;    /* unused and should be 0 */
        DWORD        Expansion8;    /* unused and should be 0 */
        DWORD        Expansion9;    /* unused and should be 0 */
        DWORD        Expansion10;   /* unused and should be 0 */
    } D2F_STRUC;

/*.P*/
/***************************************************************************\
*   Utiltiy functions (DIBs, colormaps, histograms) (opcode 82)             *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE *ptr1;
                /* subcode 0,3,4,5,6 -- unreferenced */
                /* subcode 1,2 -- pointer to 32k/64K WORD histogram buffer */
        BYTE PICHUGE *ptr2;
                /* subcode 3,4 -- pointer to 32K BYTE colormap buffer */
                /* subcode 0,1,2,5,6 -- unreferenced */
        BYTE PICHUGE *ptr3;         /* subcode 8 -- external LOGPALETTE */
        BYTE PICHUGE *Reserved4;    /* must be null */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
                /*NOTE: if a subcode does not use one of the following members, */
                /* it should be set to 0. */
        PICFLAGS     PicFlags;      /* PF_IncludeBMPHead, PF_Dither */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        DWORD        AllocType;
                /* 0 if input is to be processed in chunks, 1 if input will be */
                /* available in its entirety (avoids possible need for seeking) */
        DWORD        Subcode;       /* 0,1,2,3,4,5,6 */
        DWORD        HistogramSize;
                /* subcode 0,4,5,6 -- unused */
                /* subcode 1,2 -- size of histogram buffer (Histogram) in WORDS -- */
                /* must be either 32768 or 65536 */
        BITMAPINFOHEADER BiOut;     /* bi header for output dibs */
        DWORD        RetainPalette;
                /* subcode 0 -- 1 to retain palette info, 0 to discard */
                /* subcodes 1,2,3,4,5,6 -- unused */
        DWORD        Orient;
                /* subcode 0,1,2,3,4,6 -- unused */
                /* subcode 5 -- 3 lsbs indicate: */
                /*   bit 0: reflect horizontally */
                /*   bit 1: rotate right 90 */
                /*   bit 2: rotate 180 */
                /* all other bits should be 0 rotations are performed first */
        DWORD        OutBpp;
                /* subcode 0,4: output bits per pixel */
                /* subcode 1,2,3,5,6: unused */
        DWORD        NumColors;
                /* subcode 2,3: number of colors */
                /* subcode 0,1,4,5,6: unused */
        DWORD        OutWidthPad;   /* used to pass in output width on Mac */
        DWORD        Expansion2;    /* unused and should be 0 */
        DWORD        Expansion3;    /* unused and should be 0 */
        DWORD        Expansion4;    /* unused and should be 0 */
        DWORD        Expansion5;    /* unused and should be 0 */
        DWORD        Expansion6;    /* unused and should be 0 */
        DWORD        Expansion7;    /* unused and should be 0 */
        DWORD        Expansion8;    /* unused and should be 0 */
        DWORD        Expansion9;    /* unused and should be 0 */
        DWORD        Expansion10;   /* unused and should be 0 */
    } UTL_STRUC;

/*.P*/
/***************************************************************************\
*   Variables for Lossless DIBS (Operation LIMP, LIME)                      *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        LAYER PICHUGE *Layer;       /* points to an 8-element array of layers */
        BYTE PICHUGE *AppField;     /* points to user info of AppFieldLen bytes */
        BYTE PICHUGE *OverText;     /* NULL or points to overlaying text */
        RGBQUAD PICHUGE *UniversalCT;  /* Universal Color Table, replaces CT in image */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* See the PF_ flags above */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         AppFieldSize;  /* Allocated size of AppField in bytes. */
        LONG         AppFieldLen;   /* 0 or length of data within AppField */
        LONG         OverTextSize;  /* Allocated size of OverText in bytes. */
        LONG         OverTextLen;   /* 0 or length of data within OverText */
        LONG         StripSize;     /* Minimum size of buffer to hold a strip - after */
                                    /* INIT it is the size of the largest strip, upon */
                                    /* RES_PUT_NEED_SPACE it is the size for the */
                                    /* current layer */
        SHORT        NumUC;         /* Number of Universal Colors in UniversalCT */
        SBYTE        MinLayerWanted;/* When expanding, smallest layer needed */
        SBYTE        CurrentLayer;  /* Current layer being packed/expanded */
        BYTE         RestartLo;     /* Low range of restart intervals to be expanded */
        BYTE         RestartHi;     /* High range of restart intervals to be expanded */
        /* the next 15 items (ProgHeight..SymbolSize) should remain in the same order */
        BYTE         ProgHeight;    /* Progression height */
        BYTE         NumRestarts;   /* Number of restart intervals (for level 0) */
        DWORD        RestartOff[4]; /* Offset of restart interval from layer 0 */
        SHORT        NumOfPages;    /* Number of pages/images in document/file */
        SHORT        PageNum;       /* Page/image number of current page/image */
        SHORT        Transparent;   /* Transparent color index, -1 means none */
        SHORT        UserDelay;     /* User delay (seconds/100) before image replacement */
        SHORT        XOff;          /* Image X-offset relative to logical screen */
        SHORT        YOff;          /* Image Y-offset relative to logical screen */
        BYTE         DispMethod;    /* Disposition method */
        BYTE         ErrLimit;      /* Last ErrLimit bits can be in error */
        BYTE         CompMethod;    /* Lossless compression 'L', or 'P', 0 => automatic */
        BYTE         CompOrder;     /* Order of method if method if 'P' (1..4), 0=>3 */
        /* The remaining should be invisible to the app. */
        LONG         TableSize;     /* Hash table size */
        LONG         SymbolSize;    /* Size in bits of Symbol */
        DWORD        Symbol;        /* Symbol to be encoded/decoded */
        DWORD        HashedContext; /* Context for Symbol to be encoded/decoded */
        DWORD        HashStep;      /* Aid to help Els-coder traverse hash table */
                                    /* must be < TableSize */
        DWORD        TotalWritten;  /* Accumulative count of bytes written so far */
        WORD         internal_buffer;   /* for internal use by Els coder only */
        SHORT        jot_count;                     /* " */
        BYTE PICFAR  *context_table;                /* " */
        BYTE PICFAR  *backup_table;                 /* " */
        BYTE PICFAR  *table_end;                    /* " */
        DWORD        minimum;                       /* " */
        LONG         backlog;                       /* " */
    } LOSSLESS;


/*.P*/
/***************************************************************************\
*   Variables for Lossless DIBS (Operation LIP3, LIE3), have ParmVerMin>=2  *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        LAYER PICHUGE *Layer;       /* points to an 8-element array of layers */
        BYTE PICHUGE *AHT;          /* points to optional Abbreviated Huffman Table */
        BYTE PICHUGE *Reserved3;    /* must be null */
        RGBQUAD PICHUGE *UniversalCT;  /* Universal Color Table, replaces CT in image */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* See the PF_ flags above */
        PICFLAGS     PicFlags2;     /* See the PF2_ flags above */
        REGION       Region;        /* Source or Destination region (replaces Head) */
        LONG         StripSize;     /* Minimum size of buffer to hold a strip - after */
                                    /* INIT it is the size of the largest strip, upon */
                                    /* RES_PUT_NEED_SPACE it is the size for the */
                                    /* current layer */
        SHORT        NumUC;         /* Number of Universal Colors in UniversalCT */
        SBYTE        MinLayerWanted;/* When expanding, smallest layer needed */
        SBYTE        CurrentLayer;  /* Current layer being packed/expanded */
        BYTE         RestartLo;     /* Low range of restart intervals to be expanded */
        BYTE         RestartHi;     /* High range of restart intervals to be expanded */
        /* the next 13 items (ProgHeight..SymbolSize) should remain in the same order */
        BYTE         ProgHeight;    /* Progression height */
        BYTE         NumRestarts;   /* Number of restart intervals (for level 0) */
        DWORD        RestartOff[4]; /* Offset of restart interval from layer 0 */
        SHORT        NumOfPages;    /* Number of pages/images in document/file */
        SHORT        PageNum;       /* Page/image number of current page/image */
        SHORT        Transparent;   /* Transparent color index, -1 means none */
        SHORT        UserDelay;     /* User delay (seconds/100) before image replacement */
        SHORT        XOff;          /* Image X-offset relative to logical screen */
        SHORT        YOff;          /* Image Y-offset relative to logical screen */
        BYTE         DispMethod;    /* Disposition method */
        BYTE         AllowedBitErr; /* Last AllowedBitErr bits can be in error */
        BYTE         CompMethod;    /* Lossless compression (see #define METHOD_xxx below) */
        /* Configuration parameters for PPMD or JPEG method: */
        BYTE         CompOrder;     /* Order of method PPMD=>0..4, JPEG=>1..7 */
        BYTE         PTuning;       /* Index to set of tuned parameters for PPMD method */
        SBYTE        Channel;       /* Color channel to be treated as gray (-1 if composit) */
        PIXEL_TYPE   IOPixType;     /* Convert from/to this pixel type , 0 = default */
        BYTE         NativeBpp;     /* Bpp prior to any conversion from/to IOPixType */
        DWORD        RestartCount;  /* Number of pixels between restart markers */
        DWORD        SetBits;       /* (internal) After UnpkLine this has pixel's used bits set */
        DWORD        *PixelMap;     /* (internal) Maps 2^EffectiveBpp values to 2^RegionBpp values, */
                                    /* includes UniversalCT corrections and any byte swap */
        BYTE         EffectiveBpp;  /* (internal) NativeBpp - AllowedBitErr after INIT */
        BYTE         PrecisionUCT;  /* (internal) 0 <= Each element in UniversalCT < 2^PrecisionUCT */
        BYTE         Reserved9[2];
        DWORD        AHTLen;        /* Byte length of AHT buffer (if any) */
    } LOSSLESS3;

#define METHOD_AUTO 0
#define METHOD_LOCO 'J'
#define METHOD_PPMD 'P'
#define METHOD_JPEG 'S'


/***************************************************************************\
*   Variables for Portable Network Graphics, have ParmVerMin>=2             *
\***************************************************************************/
#define rgbAlpha     rgbReserved    /* replaces 4th component of RGBQUAD array */
typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE *Reserved1;    /* must be null */
        BYTE PICHUGE *Reserved2;    /* must be null */
        BYTE PICHUGE *Reserved3;    /* must be null */
        BYTE PICHUGE *Reserved4;    /* must be null */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* See the PF_ flags above */
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        REGION       Region;        /* Source or Destination region (replaces Head) */
        LONG         StripSize;     /* Minimum size of buffer to hold a strip - after */
                                    /* INIT it is the size of the largest strip */
        DWORD        FilterSampleSize; /* Byte size for filter offset */
        DWORD        AlphaBpp;      /* Bytes per pixel including any alpha channel */
        DWORD        AlphaStride;   /* Stride including any alpha channel */       
        DWORD        BytesRem;      /* Number of image bytes to be (de)compressed */
        BYTE         Background[6]; /* index, gray value, or color value of background color */
                                    /* colors are BGR, each component is a WORD */
                                    /* valid iff PF_HaveBackground */
        BYTE         Trans[6];      /* gray value, or color value of transparent color */
                                    /* For index trans. the 4th RGBQUAD value is used */
        BYTE         LastTrans;     /* ColorTable indices 0..LastTrans are transparent */
        BYTE         Action;        /* internal variable - action to be performed */
        BYTE         Filter;        /* valid if PF_OptimalFilter is not set. Use filter 0..4 */
        BYTE         BitDepth;      /* number of bits per component */
        DWORD        MinReadData;   /* minimum bytes required to satisfy ReadCode request */
    } PNG_UNION;



/***************************************************************************\
*   Variables for Zooming (Operation ZOOM)                                  *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE *Reserved1;    /* Must be NULL! */
        BYTE PICHUGE *Reserved2;    /* Must be NULL! */
        BYTE PICHUGE *Reserved3;    /* Must be NULL! */
        BYTE PICHUGE *Reserved4;    /* Must be NULL! */
        BYTE PICHUGE *Reserved5;    /* Must be NULL! */
        BYTE PICHUGE *Reserved6;    /* Must be NULL! */
        BYTE PICHUGE *Reserved7;    /* Must be NULL! */
        BYTE PICHUGE *Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;      /* See the PF_ flags above */
        PICFLAGS     PicFlags2;     /* See the PF_ flags above */
        LONG         NewWidth;      /* width of new image */
        LONG         NewHeight;     /* height of new image */
        WORD         Mode;          /* if 1, then scaling factor is NewWidth/NewHeight */
        WORD         NewBitCount;   /* bitcount for new image */
        void PICFAR  *WorkArea;     /* For internal use */
        BITMAPINFOHEADER BiOut;     /* Header for output image */
        DWORD        Reserved;      /* Should be zero */
    } ZOOM_PARMS;



/***************************************************************************\
*   Variables for wavelet expand/compress (Operations W2D, D2W)             *
\***************************************************************************/
typedef struct {
        WORD          QuantizationThreshold;    /* 0 .. 1000  */
        CHAR          XTransformFilter[9];      /* ASCIIZ file name */
        CHAR          YTransformFilter[9];      /* ASCIIZ file name */
        BYTE          MinTransformLevel;        /* 0 .. 10   */
        BYTE          MaxTransformLevel;        /* 0 .. 10   */
        CHAR          Qthr[6];                  /* QLUTLIB ASCIIZ Quantization table name */
        } WAVECOMPONENT;

typedef struct {
        DWORD        Reserved0;
        /* Information prior to REQ_INIT */
        BYTE PICHUGE* QlutlibPath;
        BYTE PICHUGE* Reserved2;
        BYTE PICHUGE* Reserved3;
        BYTE PICHUGE* Reserved4;
        BYTE PICHUGE* Reserved5;    /* Must be NULL! */
        BYTE PICHUGE* Reserved6;    /* Must be NULL! */
        BYTE PICHUGE* Reserved7;    /* Must be NULL! */
        BYTE PICHUGE* Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         Status;
        /* file names below specify files with extension .ASC in the same
            directory (QlutlibPath) as QLUTLIB.ASC */
        WAVECOMPONENT Y;    /* also gray scale */
        WAVECOMPONENT I;
        WAVECOMPONENT Q;
        REGION       Region;
        /* ChunkSize .. ChunkHeight must appear in this order and adjacent
            to each other so op91.c/op90.c can define a pic_parm offset to them */            
        BYTE         ChunkSize;     /* if ChunkType != 3, 0 for no chunks,
                                        else ignored if ChunkType == 3 */
        BYTE         ChunkType;     /* 0 horizontal strips, 1 vertical strips, 2 rectangles,
                                        3 to use ChunkWidth/ChunkHeight and ignore
                                        ChunkSize */
        WORD         ChunkWidth;    /* chunk width for ChunkType == 3 */
        WORD         ChunkHeight;   /* chunk height for ChunkType == 3 */
        } WAVELET;




/***************************************************************************\
*   Variables for Wavelet Scalar Quantization (Operations WSQP, WSQE)       *
\***************************************************************************/
typedef struct {
        DWORD        Reserved0;
        BYTE PICHUGE* Reserved1; 
        BYTE PICHUGE* Reserved2;
        BYTE PICHUGE* Reserved3;
        BYTE PICHUGE* Reserved4;
        BYTE PICHUGE* Reserved5;    /* Must be NULL! */
        BYTE PICHUGE* Reserved6;    /* Must be NULL! */
        BYTE PICHUGE* Reserved7;    /* Must be NULL! */
        BYTE PICHUGE* Reserved8;    /* Must be NULL! */
        PICFLAGS     PicFlags;
        PICFLAGS     PicFlags2;     /* PF2_ flags above when implemented */
        LONG         Status;
        REGION       Region;
    } WSQ_UNION;



/*.P*/
/***************************************************************************\
*  This structure is allocated by the user and its address passed to        *
*  the Pegasus routines and to any callback routines.                       *
*  The values which need to be specified in the PIC_PARM structure vary     *
*  depending on which operation is performed, and on whether the            *
*  REQ_INIT or REQ_EXEC request is made.  See the documentation for         *
*  specific details.                                                        *
\***************************************************************************/

typedef struct PIC_PARM_TAG {
        DWORD        Reserved0;
        /*  Elements shared by most operations */
        LONG         ParmSize;      /* Size of this structure (bytes) */
        BYTE         ParmVer;       /* Version of parameters for given Op (11..99 ) */
        BYTE         ParmVerMinor;  /* Minor version number of parameter structure */
        WORD         Reserver1;     /* should be 0 for now */
        LONG         Status;        /* 0 or error code (see ERRORS.H) */
        OPERATION    Op;            /* OP_ fields, specific operation to be performed */
        BITMAPINFOHEADER Head;      /* Uncompressed Image (UI) Width, Height, etc. */
        RGBQUAD      ColorTable[272]; /* Holds primary (ó256) & secondary (ó16) palettes */
#if defined(MACINTOSH)
        CTabHandle  MacCTable;      /* Color Table used on the Macintosh */
#endif
        LONG         PicVer;        /* PIC image version */
        ORIENTATION  VisualOrient;  /* O_ fields, displayed orientation of UI */
        LONG         CommentSize;   /* Size of comment buffer (bytes) */
        LONG         CommentLen;    /* Length of comment (0 if no comment) */
        CHAR PICHUGE *Comment;      /* Points to comment up to CommentSize bytes */
        LPARAM       App;           /* any user info needed by the callbacks */
        LONG         PercentDone;   /* Strip progress monitor, 100 iff done */
        WORK_AREA PICHUGE *Reserved;/* Allocated by Pegasus routines, init NULL */
        /*  Buffer information. Can be used as circular buffers. */
        QUEUE        Get;
        QUEUE        Put;
        BYTE         KeyField[8];   /* Used to hold encode/decode key */
        /* The following function is called in the non-coroutine case by Pegasus */
        /* to ask for more data, space, etc. from the application.  DeferFn is an */
        /* application function that returns non-zero if Pegasus is to terminate */
        LONG         (PICFAR *DeferFn)(struct PIC_PARM_TAG PICFAR *, RESPONSE);
        DWORD        Flags;         /* See F_ defines */
        DWORD        Flags2;        /* See F2_ defines when implemented */
        WORD         CropWidth;     /* These next four fields have meaning only */
        WORD         CropHeight;    /* if the Flags F_Crop bit is set.  The */
        WORD         CropXoff;      /* pixel position are relative to the */
        WORD         CropYoff;      /* image's logical upper-left corner */
        DWORD        ImageNumber;   /* Which image of a multi-image file (0 = next) */
        DWORD        PacketType;    /* Type of PIC2 packet for which space is to be reserved */
        DWORD        SeekInfo;      /* see components of SeekInfo below */

        CHAR PICHUGE* PIC2List;
        LONG         PIC2ListSize;
        LONG         PIC2ListLen;
        REGION       RegionIn;
        REGION       RegionOut;
                                    /* OpVersion? returned by some Op's: */
        WORD         OpVersion0;    /*     lsw of 64-bit FileVersion */
        WORD         OpVersion1;    /*     word 1 of 64-bit FileVersion */
        WORD         OpVersion2;    /*     word 2 of 64-bit FileVersion */
        WORD         OpVersion3;    /*     msw of 64-bit FileVersion */

        BYTE PICHUGE* ReservedPtr1;
        BYTE PICHUGE* ReservedPtr2;
        BYTE PICHUGE* ReservedPtr3;
        BYTE PICHUGE* ReservedPtr4;
        BYTE PICHUGE* ReservedPtr5;
        BYTE PICHUGE* ReservedPtr6;
        BYTE PICHUGE* ReservedPtr7;
        BYTE PICHUGE* ReservedPtr8;

        CHAR PICHUGE* LoadPath;     /* Path to directory with opcode DLL */
        DWORD         LoadResInstance; /* Module instance handle of an
                                       EXE or DLL from whose resource data
                                       the opcode DLL is to be loaded */
            /* if LoadPath == 0 && LoadResInstance == 0
                    then the opcode DLL is loaded from the same directory as
                    the dispatcher DLL.  If not found there then the opcode
                    DLL is loaded using the directory order Windows uses
                    when loading DLLs
               if LoadPath != 0 && LoadResInstance == 0,
                    then the opcode DLL is loaded from the specified directory
               if LoadPath == 0 && LoadResInstance != 0
                    then the opcode DLL is loaded from the specified module's
                    resource data
               if LoadPath != 0 && LoadResInstance != 0
                    then the opcode DLL is loaded from the specified module's
                    resource data.  If not found there, then the opcode DLL
                    is loaded from the specified path. If LoadPath is "",
                    and the opcode DLL is not found in the specified module's
                    resource data, then the opcode DLL is loaded as though
                    LoadPath == 0 && LoadResInstance == 0
            */
        struct PIC_PARM_TAG PICHUGE* NestPP;    /* reserved */
        REQUEST       NestReq;                  /* reserved */

        DWORD         Reserveds[108];
        /* see also: Internal Use Only comments in internal.h */
        /*  Operator specific information */
        union {
            PEGQUERY    QRY;        /* PegasusQuery (NOT opcode) */
            DIB_INPUT   D2J;        /* OP_D2J (progressive or sequential) */
            DIB_OUTPUT  J2D;        /* OP_J2D, OP_JE2D, OP_S2D, OP_SE2D */
            TRANS2P     S2P;        /* OP_S2P */
            TRANP2S     P2S;        /* OP_P2S */
            D2F_STRUC   D2F;        /* OP_D2F */
            F2D_STRUC   F2D;        /* OP_F2D */
            UTL_STRUC   UTL;        /* OP_UTL */
            LOSSLESS    LL;         /* OP_LIMP, OP_LIME */
            LOSSLESS3   LL3;        /* OP_LIP3, OP_LIE3 */
            REORIENT    ROR;        /* OP_ROR */
            ZOOM_PARMS  ZOOM;       /* OP_ZOOM */
            WAVELET     WAVE;       /* OP_W2D, OP_D2W */
            DIB_INPUT   D2S;        /* please use D2J instead */
            DIB_OUTPUT  S2D;        /* please use J2D instead */
            DIB_OUTPUT  P2D;        /* please use J2D instead */
            PNG_UNION   PNG;        /* Portable Network Graphics */
            WSQ_UNION   WSQ;        /* Wavelet Scalar Quantization */
            INTERNAL    Reserved;   /*Always reserved as the largest variant */
        } u;
    } PIC_PARM;

        /*the components of SeekInfo */
#define SEEK_OFFSET     0x3FFFFFFFL  /* bits 0-29: unsigned offset in file */
#define SEEK_FILE       0x40000000L  /* bit 30: 0/1 => seek input/output queue */
#define SEEK_DIRECTION  0x80000000L  /* bit 31: 0/1 => seek from beg./end of file */

        /* Flag bit fields for general image information */
#define F_Crop          0x00000001  /* Set if image is to be cropped */
#define F_Raw           0x00000002  /* Set if pack source or unpack dest. is RAW */
#define F_Bmp           0x00000004  /* Set if pack source or unpack dest. is BMP */
#define F_XOut          0x00000008  /* reserved, for internal use only */
#define F_XIn           0x00000010  /* reserved, for internal use only */
#define F_DoReqExecNext 0x00000020  /* used by KSAPI */
#define F_DidReqExec    0x00000040  /* used by KSAPI */
#define F_AsyncAbort    0x00000080  /* used by MTAPI */



/*.P*/
/***************************************************************************\
*   void  Verify (BOOL x, PIC_PARM *p, LONG err);                           *
*                                                                           *
*   This is a macro which tests the value of x.  Iff x is false, Verify     *
*   will return control back to the application from any nested depth.      *
*   p->status is set to err, the operation is terminated, the Pegasus       *
*   routine is terminated, and the Reserved is freed.                       *
*                                                                           *
*   void  ChkDefer (BOOL x, PIC_PARM *p);                                   *
*                                                                           *
*   This is a macro which tests the value of x.  Iff x is false, it         *
*   will return control back to the application from any nested depth.      *
*   The Pegasus routine is terminated, and the Reserved is freed.           *
*                                                                           *
*   void  Defer (PIC_PARM *p, RESPONSE res);                                *
*                                                                           *
*   This is a macro which will pass control back to the application         *
*   with the result res.  It is assumed that control will resume after      *
*   the Defer statement when the application continues with the next        *
*   call to the Pegasus routine. Note that res should not equal RES_ERR.    *
\***************************************************************************/
#define Verify(x,p,err)     while (!(x)) (p)->Status = (err), \
                                longjmp((p)->Reserved->AppState, RES_ERR)

#define ChkDefer(x,p)       while (!(x)) \
                                longjmp((p)->Reserved->AppState, RES_ERR)

#define Defer(p,res)        while (setjmp((p)->Reserved->PicState) == 0) \
                                longjmp((p)->Reserved->AppState, (res))


/***************************************************************************\
*  Prototypes for Pegasus functions.                                        *
\***************************************************************************/
#ifdef __cplusplus
    extern "C" {
#endif

PUBLIC  BOOL DLLEXPORTWINAPI PegasusQuery (PIC_PARM PICHUGE *p);

#if !defined(MACINTOSH)
PUBLIC  BOOL DLLEXPORTWINAPI PegasusCB (PIC_PARM PICHUGE *p,
                 void (PICFAR *GetData) (PIC_PARM PICHUGE *p),
                 void (PICFAR *PutData) (PIC_PARM PICHUGE *p) );

PUBLIC  RESPONSE DLLEXPORTWINAPI Pegasus (PIC_PARM PICHUGE *p, REQUEST req);

PUBLIC  LONG DLLEXPORTWINAPI PegasusLoad(
    OPERATION Op,
    LONG ParmVer,
    char PICFAR *Path);

PUBLIC  LONG DLLEXPORTWINAPI PegasusLoadFromRes(
    OPERATION Op,
    LONG ParmVer,
    char PICFAR *Path,
    DWORD hInstance);

PUBLIC  void DLLEXPORTWINAPI PegasusUnload (OPERATION Op, LONG ParmVer);

PUBLIC  void DLLEXPORTWINAPI PegasusTrace (const char PICFAR *pszMsg);
#endif

PUBLIC  BOOL DLLEXPORTWINAPI PegasusLibInit (DWORD hInstance);

PUBLIC  void DLLEXPORTWINAPI PegasusLibTerm (void);

PUBLIC  void DLLEXPORTWINAPI PegasusLibThreadInit (void);

PUBLIC  void DLLEXPORTWINAPI PegasusLibThreadTerm (void);

#ifdef __cplusplus
    }
#endif

#ifdef __MWERKS__
    /*#### find out how to save/restore the alignment */
#elif defined(__BORLANDC__)
    /*#### find out how to save/restore the alignment
            "#pragma option -a" doesn't do it in BC 4.5 */
#else /* assume microsoft or watcom */
    #if defined(__FLAT__) || defined(__WATCOMC__)
        #pragma pack(pop)
    #else
        /* in MS 16-bit, best we can do is to restore to the command-line state */
        #pragma pack()
    #endif
#endif

#endif  /* #if !defined(_PIC) */

