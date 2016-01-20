/*. Copyright (C) 1990-93 -  Xing Technology Corporation
/*.                            All Rights Reserved
/*. */
/*.**FILE ABSTRACT**********************************************************
/*.                                    Text Editor Note: NO EMBEDDED TABS
/*.  APPLICATION:  Xing (tm) VT-COMPRESS (tm) JPEG Software Development Kit
/*.  FILE:         JPEGSDK1.H
/*.  ABSTRACT:     Definitions for VT-Compress SDK Structures and Defines
/*.
/*.  REVISION LOG:
/*.    ANTECEDENT FILE: HEADER.H
/*.    Renamed to JPEGSDK1.H                    9-29-92
/*.
/*.************************************************************************/

/*---DEFINED SYMBOLS-------------------------------------------------*/

#ifdef XINGWIN
    #undef XINGDOS
    #include <windows.h>
#else
    #define XINGDOS
#endif
#define TRUE  1
#define FALSE 0

/* Developers who want to continue to used the archaic defined sysmbols
   should enable the define for KEEP_ARCHAIC_SYMBOLS */
//#define KEEP_ARCHAIC_SYMBOLS


/*******************************************************************/
/* BEGIN Defines for JPEG_CONTROL structure */
/*******************************************************************/
/* general define for structure elements as listed below */
#define JPEG_API_NULL           0

/* JPEG_CONTROL I/O Types -- .src_type and .dst_type */
/*      JPEG_API_NULL           0  App may use for dst of call to query */
/*      JPEG_MEMORY             1  CAPABILITY DELETED */
#ifdef KEEP_ARCHAIC_SYMBOLS
#define JPEG_FILE               2  /* Archaic, not recommended for new code */
#define JPEG_FUNCTION           3  /* Archaic, not recommended for new code */
#endif
        /* The archaic symbols above may continue to be used, but the */
        /* substitutes below are recommended for clarity of use */
#define JLIB_IOTYPE_NULL        0   /* App may use for dst of call to query */
#define JLIB_IOTYPE_FILE        2   /* Replaces JPEG_FILE */
#define JLIB_IOTYPE_FUNCTION    3   /* Replaces JPEG_FUNCTION */
#define JLIB_IOTYPE_MEMDIB      23  /* Future provision, not implemented */

/* JPEG_CONTROL Mode Types -- .jpeg_mode */
#ifdef KEEP_ARCHAIC_SYMBOLS
#define JPEG_COMPRESS_BASELINE  6  /* Archaic, not recommended for new code */
#define JPEG_EXPAND_BASELINE    7  /* Archaic, not recommended for new code */
#endif
        /* The archaic symbols above may continue to be used, but the */
        /* substitutes below are recommended for clarity of use */
        /* Query function will return one of the below 4 codes */
#define JLIB_MODE_NULL          0  /* App may use for call to query */
#define JLIB_MODE_COMPRESS      6  /* Replaces JPEG_COMPRESS_BASELINE */
#define JLIB_MODE_EXPAND        7  /* Replaces JPEG_EXPAND_BASELINE */
#define JLIB_MODE_LOAD          22 /* Added for Version 3.0 */

/* JPEG_CONTROL JPEG File Header Types -- .jpeg_header */
/*      JPEG_API_NULL           0  App may use for call to query */
#ifdef KEEP_ARCHAIC_SYMBOLS
#define JPEG_FULL               4
#define XING_JPEG_ABBREV        5
#define JPEG_ABBREV             5
#define JPEG_JFIF               20
#endif
        /* The archaic symbols above may continue to be used, but the */
        /* substitutes below are recommended for clarity of use */
#define JLIB_JDATA_FULL         4
#define JLIB_JDATA_ABBR_XING    5
#define JLIB_JDATA_JFIF101      20
#define JLIB_JDATA_ABBR_TABLES  21
#define JLIB_JDATA_ABBR_IMAGE   24   /* Reserved for future use */
#define JLIB_JDATA_JPEGDIB      25


/* JPEG_CONTROL Subsample Types -- .jpeg_subsample */
/*      JPEG_API_NULL           0  App may use for call to query */
#ifdef KEEP_ARCHAIC_SYMBOLS
#define JPEG_HR                 9       /* 4:4:4 subsample      */
#define JPEG_MR                 8       /* 4:2:2 subsample      */
#define JPEG_LR                 13      /* 4:1:1 subsample      */
#define JPEG_LR2                19      /* 4:2:0 subsample      */
#define JPEG_LLR                19      /* 4:2:0 subsample      */
#endif
        /* The archaic symbols above may continue to be used, but the */
        /* substitutes below are recommended for clarity of use */
#define JLIB_SAMPL_HR           9       /* 4:4:4 subsample      */
#define JLIB_SAMPL_MR           8       /* 4:2:2 subsample      */
#define JLIB_SAMPL_LR1          13      /* 4:1:1 subsample      */
#define JLIB_SAMPL_LR2          19      /* 4:2:0 subsample      */

/* JPEG_CONTROL JpegColor Types -- .jpeg_color */
/* App sets output color type in lower byte for compression */
/* Query returns this field for query on a jpeg file */
/*      JPEG_API_NULL           0  App may use for call to query */
#ifdef KEEP_ARCHAIC_SYMBOLS
#define JPEG_YUV                10   /* for all color compressions */
#define JPEG_Y                  14   /* for all grayscale compressions */
#endif
        /* The archaic symbols above may continue to be used, but the */
        /* substitutes below are recommended for clarity of use */
#define JLIB_COLOR_YUV          10   /* for all color compressions */
#define JLIB_COLOR_Y            14   /* for all grayscale compressions */
#define JLIB_COLOR_RGB          26   /* future provision for JPEG DIB */
/* Flag masks used on upper byte of .jpeg_color*/
#define ALT_QUANT_TABLES        0x2000  /* not implemented */
#define RESTART_ENABLED_FLAG    0x1000  /* enable insertion of 0xFFDx restart codes */

/* JPEG_CONTROL Image Types -- .image_type */
/* Defines for file_types marked "Reserved" are not currently supported */
/* Values returned from query of source file */
/*      JPEG_API_NULL           0  App may use for call to query */
#define TGA_M8  0x01        /* Truevision TARGA color mapped - Reserved */
#define TGA_16  0x02        /* Truevision TARGA 16-bit TruColor*/
#define TGA_24  0x02        /* Truevision TARGA 24-bit TruColor*/
#define TGA_32  0x02        /* Truevision TARGA 32-bit TruColor - Reserved */
#define TGA_8   0x03        /* Truevision TARGA 8-bit Grayscale */
#define TIF_B   _T('B')         /* B=66 =42h TIFF Bi-Level - Reserved */
#define TIF_G   _T('G')         /* G=71 =47h TIFF 8-bit Grayscale */
#define TIF_R   _T('R')         /* R=82 =52h TIFF 24-bit RGB */
#define JPEG_FILETYPE  _T('J')  /* J=74 =4Ah JPEG/JFIF file */
#define BMP_1   1           /* Microsoft Bitmap Bi-Level - Reserved */
#define BMP_4   4           /* Microsoft Bitmap 16-color/mapped - Reserved */
#define BMP_8   8           /* Microsoft Bitmap 256-color/mapped */
#define BMP_24  24          /* Microsoft Bitmap 24-bit TruColor */

/* JPEG_CONTROL Image Color Types -- .image_color */
/* values of color_space in lower byte */
/* Query returns this field for query on an image (not jpeg) file */
/* When compressing from a function, the App shall set this field
   to designate the format of the data in the source buffer */
/* When expanding to a function, the App shall set this field
   to designate the format of the data in the destination buffer */
/*      JPEG_API_NULL           0  App may use for call to query */
#define MONO_8      2
#define RGB_565     3
#define RGB_555     4
#define RGB_888     5
#define INVALID     7
#define UNSUPPORTED 8
#define MONO        9
#define COLOR       10
#define JPG_8       11
#define JPG_24      12
// #define JPG_FULL    13  // Ver 3.0 deleted, this define was never used
#define MONO_8_NEG  16
#define YUV_888     17
#define BGR_888     18
#define RGB_8       19
#define MONO_1      21
#define MONO_1_NEG  22
/* masks used on .image_color_space 2-byte field */
#define COLOR_SPACE_MASK        0x00FF      /* 8 bit field mask */
#define SUBSAMPLE_MASK          0xC000      /* 2 bit field mask */
#define HR_FLAG                 0x8000      /* 4:4:4 subsample on color */
#define LR_FLAG                 0x4000      /* 4:1:1 subsample on color */
#define LLR_FLAG                0xC000      /* 4:2:0 subsample on color */
#define LR2_FLAG                0xC000      /* 4:2:0 subsample on color */
#define INVERT_FLAG             0x0800      /* paint bottom to top */
#define HAS_COLOR_MAP           0x0400      /* not implemented */

    /* vtc_error flags */
#define VTC_SUCCESS             0  /* App should use for call to query */
#define VTC_INVALID_SRC_FILE    1
#define VTC_INVALID_DST_FILE    2
#define VTC_PREMATURE_EOF       3
#define VTC_INVALID_HEADER      4
#define VTC_INVALID_DATA        5
#define VTC_BUFFER_OVERFLOW     6
#define VTC_FILE_ACCESS_ERROR   7
#define VTC_UNSUPPORTED         8
#define VTC_SYSTEM_ERROR        9
#define VTC_USER_BREAK          10  /* App should use to force an abort
                                       of expand/compress via function
                                       callbacks */

/*---STRUCTURE DEFINITONS--------------------------------------------*/

    /* control structure for JPEG compression/expansion */
struct JPEG_CONTROL {
    int     src_type;       /* from JPEG_FILE, JPEG_FUNCTION */
    int     dst_type;       /* to JPEG_FILE, JPEG_FUNCTION   */
    void FAR *source;        /* pointer to source filename or function       */
    void FAR *destination;   /* pointer to destination filename or function  */
    void FAR *src_buf;       /* buffer for source data                       */
    void FAR *dst_buf;       /* buffer for destination data                  */
    long    src_buf_size;   /* source buffer size                           */
    long    dst_buf_size;   /* destination buffer size                      */
    int     jpeg_mode;      /* JPEG_COMPRESS_BASELINE, JPEG_EXPAND_BASELINE */
    int     jpeg_header;    /* JPEG_JFIF, or JPEG_FULL                     */
    int     jpeg_subsample; /* subsampling JPEG_LR, JPEG_MR, JPEG_HR        */
    int     jpeg_color;     /* used for restart interval signaling          */
    int     image_type;     /* JPG_0, TIF_R, BMP_24, etc        */
    int     image_pixel;    /* bytes per pixel == 1, 2 or 3                 */
    int     image_color;    /* MONO_8, RGB_565, RGB_555, RGB_888 etc*/
    int     image_width;    /* in pixels, not bytes                         */
    int     image_height;   /* in pixels, not bytes                         */
    int     yquant;         /* luminance quantization                       */
    int     cquant;         /* chrominance quantization                     */
    int     jpeg_error;     /* error return value                           */
    TCHAR    user_space[64]; /* user defined storage                         */
    void FAR *JFIFHeader;   /* pointer to a JFIFHeader struct, if allocated */
};
typedef struct JPEG_CONTROL JPEGCTRLSTRUCT;
typedef JPEGCTRLSTRUCT FAR *LPJPEGCTRL;

/* an App may allocate this and stuff pointer in jpeg_control.JFIFHeader */
struct JFIFHEADER {
    unsigned short  units;          /* extracted from JFIF APPO data */
    unsigned short  Xdensity;       /* extracted from JFIF APPO data */
    unsigned short  Ydensity;       /* extracted from JFIF APPO data */
    unsigned short  Xthumbnail;     /* extracted from JFIF APPO data */
    unsigned short  Ythumbnail;     /* extracted from JFIF APPO data */
    LPTSTR lpthumbnail;    			/* optional ptr to App allocation */
    LPTSTR lpcomment;      			/* optional ptr to App allocation */
    LPTSTR lpReserved1;    			/* USAGE NOT IMPLEMENTED BY LIBRARY */
    LPTSTR lpReserved2;    			/* USAGE NOT IMPLEMENTED BY LIBRARY */
    int iSizeofGrowth;  			/* USAGE NOT IMPLEMENTED BY LIBRARY */
#ifndef JFIFHEADER_SHORT_ALLOC
    unsigned char   growth[80];      /* proposed growth room */
    /* JPEG Library version 2.0 does not include this growth, so
       the JPEG_DLL.DLL build for Version 2.0 is compatible with
       the version 1.6 allocation for this structure.
       Later versions of the DLL may enventually incorporate the growth.
       All new Applications should allocate the growth immediately so that
       future DLLs will not be incompatible. */
#endif
};
typedef struct JFIFHEADER JFIFHDRSTRUCT;
typedef JFIFHDRSTRUCT FAR *LPJFIFHDR;

// Applications developers might allocate spare space at the end of the
// structure to pre-allocate for a (future) larger structure
// Currently, at least one known app will operate with errors if we
// enlarge this structure.
//   The enlarged structure may include the following, or adaptations
//   of the following, plus other TBD data
//    unsigned long   jfif_filelength; /* JFIF data length, EOI or file end*/
//    unsigned long   thumboffset;     /* provision for future use */
//    unsigned short  thumblength;     /* provision for future use */
//    unsigned long   commentoffset;   /* provision for future use */
//    unsigned short  commentlength;   /* provision for future use */
//   Also offsets to custom huffman and quantization tables will
//    probably be implemented.

/* VT-Compress Query function will extract a comment from JFIF files up to */
/* a default 145 TCHARs + a null if the app provides a pointer to a */
/* JFIF header structure and then embedds the lpcomment pointer */
/* The App must ensure the lpcomment allocation is [146] or greater */
/* Only the first comment encountered will be extracted */
#define XING_DEFAULT_COMMENT_ALLOC  146

/* VT-Compress Query function will extract thumbnails from JFIF files up to */
/* a default 100 x 100 x 3 if the app provides a pointer to a */
/* JFIF header structure and then embeds the lpthumbnail pointer */
/* The App must ensure the lpthumbnail allocation is [30000] or greater */
#define XING_DEFAULT_THUMBNAIL_ALLOC  30000

#define PALVERSION  0x300
#define MAXPALETTE  256   /* max. # supported palette entries */
//  VT-Compress SDK supports retrieval of the Xing Default Palette
//  for those Apps which convert images to 256 colors
//  The procedure GetXingDefPal requires a Logical Palette allocated
//  as follows:
//    dwLen = (LONG)sizeof(LOGPALETTE) +
//        (MAXPALETTE) * sizeof(PALETTEENTRY);/* see MAXPALETTE define */
//    hDefPal = GlobalAlloc (GHND, dwLen);
//  The procedure GetDefPalIndex can be used to convert any RGB data
//    to indexes mapped to the Xing Default Palette.
//  These added functions require DOS programs to first call the
//    procedure init_jpeg_library once at program initialization.
//  For WINDOWS programs, the DLL performs the Library initialization
//    so the individual App has no call to init_jpeg_library
//  Although the LOGPALETTE structure is a WINDOWS structure, Xing uses
//    the identical structure for DOS Apps as well

#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

/*---BEGIN -- DEFINE SYMBOLS FOR DOS-----------------------------------*/
/* these are defined in WINDOWS.H for WINDOWS Programs */

//#define LONG                long
//#define FAR                 _far

#ifdef XINGDOS
//#define HANDLE unsigned short
//#define BYTE unsigned char
//#define DWORD unsigned long
//#define LONG unsigned long
//#define WORD unsigned short
typedef unsigned char	BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef unsigned int    UINT;
typedef UINT            HANDLE;
typedef TCHAR FAR*      LPTSTR;

typedef struct tagPALETTEENTRY {
    BYTE    peRed;
    BYTE    peGreen;
    BYTE    peBlue;
    BYTE    peFlags;
} PALETTEENTRY;
typedef PALETTEENTRY FAR* LPPALETTEENTRY;

  /* Logical Palette */
typedef struct tagLOGPALETTE {
    WORD    palVersion;
    WORD    palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE;
typedef LOGPALETTE FAR*  LPLOGPALETTE;
#endif
/*---END -- DEFINE SYMBOLS FOR DOS-----------------------------------*/

    /* structure used by JPEG SDK color space conversion routine */
    /* see function prototype for GetDefPalIndex */
typedef struct tagRGBIXSTRUCT {
    int   R[64];      /* 8-bit Red Component of pixel */
    int   G[64];      /* 8-bit Grn Component of pixel */
    int   B[64];      /* 8-bit Blu Component of pixel */
    int   RGBIX[64];  /* Return Index correlated to Xing Default Palette*/
}RGBIXSTRUCT;
typedef RGBIXSTRUCT FAR *LPRGBIXSTRUCT;


/*---EXTERNAL DATA DECLARATIONS--------------------------------------*/
extern int vtc_error;

/*.**SOURCE FILE END************* JPEGSDK1.H ******************************/
/*. XING:CFGID: JLVTC160 - KEM           --Mon Jul 06 14:19:19 1992 */
/*. XING:CFGID: JLVTC200 - KEM           --Sun Oct 18 10:18:12 1992 */
/*. XING:CFGID: JLVTC201 - KEM           --Wed Oct 28 10:28:09 1992 */
/*. XING:CFGID: JLVT300 - KEM (prelim)   --Fri May 14 17:40:17 1993 */
