/****************************************************************************
 *
 * Copyright (C) 1991, 1992 TWAIN Working Group: Aldus, Caere, Eastman-Kodak,
 * Hewlett-Packard and Logitech Corporations.  All rights reserved.
 *
 * dc.h -  This is the definitive include file for applications and
 *         data sources written to the TWAIN specification.
 *         It defines constants, data structures, messages etc.
 *         for the public interface to TWAIN.
 *
 * Revision History:
 *  version 1.0, March 6, 1992.  TWAIN 1.0.
 *
 ****************************************************************************/


#ifndef TWAIN
#define TWAIN




/****************************************************************************
 * Platform Dependent Definitions and Typedefs                              *
 ****************************************************************************/

/* Define one of the following, depending on the platform */
/* #define _MAC_   */
/* #define _UNIX_  */
#define _MSWIN_

#ifdef _MSWIN_
typedef HANDLE         DC_HANDLE;
typedef LPVOID         DC_MEMREF;
#endif

#ifdef _MAC_
#define PASCAL         pascal
#define FAR
typedef Handle         DC_HANDLE;
typedef TCHAR          *DC_MEMREF;
#endif

#ifdef _UNIX_
#define PASCAL         pascal
typedef unsigned TCHAR *DC_HANDLE;
typedef unsigned TCHAR *DC_MEMREF;
#endif




/****************************************************************************
 * Type Definitions                                                         *
 ****************************************************************************/

/* String types. These include room for the strings and a NULL TCHAR,     *
 * or, on the Mac, a length byte followed by the string.                 *
 * DC_STR255 must hold less than 256 TCHARs so length fits in first byte. */
typedef TCHAR    DC_STR32[34],     FAR *pDC_STR32;
typedef TCHAR    DC_STR64[66],     FAR *pDC_STR64;
typedef TCHAR    DC_STR128[130],   FAR *pDC_STR128;
typedef TCHAR    DC_STR255[256],   FAR *pDC_STR255;

/* Numeric types. */
typedef TCHAR           DC_INT8,   FAR *pDC_INT8;
typedef short          DC_INT16,  FAR *pDC_INT16; 
typedef long           DC_INT32,  FAR *pDC_INT32;
typedef unsigned TCHAR  DC_UINT8,  FAR *pDC_UINT8;
typedef unsigned short DC_UINT16, FAR *pDC_UINT16;
typedef unsigned long  DC_UINT32, FAR *pDC_UINT32;
typedef unsigned short DC_BOOL,   FAR *pDC_BOOL; 

/* Fixed point structure type. */
typedef struct {
    DC_INT16     Whole;        /* maintains the sign */
    DC_UINT16    Frac;
} DC_FIX32,  FAR *pDC_FIX32;




/****************************************************************************
 * Structure Definitions                                                    *
 ****************************************************************************/

/* No DAT needed. */
typedef struct {
   DC_FIX32   X;
   DC_FIX32   Y;
   DC_FIX32   Z;
} DC_CIEPOINT, FAR * pDC_CIEPOINT;

/* No DAT needed. */
typedef struct {
   DC_FIX32   StartIn;
   DC_FIX32   BreakIn;
   DC_FIX32   EndIn;
   DC_FIX32   StartOut;
   DC_FIX32   BreakOut;
   DC_FIX32   EndOut;
   DC_FIX32   Gamma;
   DC_FIX32   SampleCount;  /* if =0 use the gamma */
} DC_DECODEFUNCTION, FAR * pDC_DECODEFUNCTION;

/* No DAT needed. */
typedef struct {
   DC_UINT8    Index;    /* Value used to index into the color table. */
   DC_UINT8    Channel1; /* First  tri-stimulus value (e.g Red)       */
   DC_UINT8    Channel2; /* Second tri-stimulus value (e.g Green)     */
   DC_UINT8    Channel3; /* Third  tri-stimulus value (e.g Blue)      */
} DC_ELEMENT8, FAR * pDC_ELEMENT8;


/* No DAT.  Defines a frame rectangle in ICAP_UNITS coordinates. */
typedef struct {
   DC_FIX32   Left;
   DC_FIX32   Top;
   DC_FIX32   Right;
   DC_FIX32   Bottom;
} DC_FRAME, FAR * pDC_FRAME;


/* No DAT needed.  Used to manage memory buffers. */
typedef struct {
   DC_UINT32  Flags;  /* Any combination of the DCMF_ constants.           */
   DC_UINT32  Length; /* Number of bytes stored in buffer TheMem.          */
   DC_MEMREF  TheMem; /* Pointer or handle to the allocated memory buffer. */
} DC_MEMORY, FAR * pDC_MEMORY;


/* No DAT needed. */
typedef struct {
   DC_DECODEFUNCTION   Decode[3];
   DC_FIX32            Mix[3][3];
} DC_TRANSFORMSTAGE, FAR * pDC_TRANSFORMSTAGE;


/* No DAT needed.  Describes version of software that's running. */
typedef struct {
   DC_UINT16  MajorNum;  /* Major revision number of the software. */
   DC_UINT16  MinorNum;  /* Incremental revision number of the software. */
   DC_UINT16  Language;  /* e.g. DCLG_SWISSFRENCH */
   DC_UINT16  Country;   /* e.g. DCCY_SWITZERLAND */
   DC_STR32   Info;      /* e.g. "1.0b3 Beta release" */
} _DC_VERSION, FAR * pDC_VERSION;


/* DCON_ARRAY. Container for array of values (a simplified DC_ENUMERATION) */
typedef struct {
   DC_UINT16  ItemType;
   DC_UINT32  NumItems;    /* How many items in ItemList           */
   DC_UINT8   ItemList[1]; /* Array of ItemType values starts here */
} DC_ARRAY, FAR * pDC_ARRAY;


/* DCON_ENUMERATION. Container for a collection of values. */
typedef struct {
   DC_UINT16  ItemType;
   DC_UINT32  NumItems;     /* How many items in ItemList                 */
   DC_UINT32  CurrentIndex; /* Current value is in ItemList[CurrentIndex] */
   DC_UINT32  DefaultIndex; /* Powerup value is in ItemList[DefaultIndex] */
   DC_UINT8   ItemList[1];  /* Array of ItemType values starts here       */
} DC_ENUMERATION, FAR * pDC_ENUMERATION;


/* DCON_ONEVALUE. Container for one value. */
typedef struct {
   DC_UINT16  ItemType;
   DC_UINT32  Item;
} DC_ONEVALUE, FAR * pDC_ONEVALUE;


/* DCON_RANGE. Container for a range of values. */
typedef struct {
   DC_UINT16  ItemType;
   DC_UINT32  MinValue;     /* Starting value in the range.           */
   DC_UINT32  MaxValue;     /* Final value in the range.              */
   DC_UINT32  StepSize;     /* Increment from MinValue to MaxValue.   */
   DC_UINT32  DefaultValue; /* Power-up value.                        */
   DC_UINT32  CurrentValue; /* The value that is currently in effect. */
} DC_RANGE, FAR * pDC_RANGE;


/* DAT_CAPABILITY. Used by app to get/set capability from/in a data source. */
typedef struct {
   DC_UINT16  Cap; /* id of capability to set or get, e.g. CAP_BRIGHTNESS */
   DC_UINT16  ConType; /* DCON_ONEVALUE, _RANGE, _ENUMERATION or _ARRAY   */
   DC_HANDLE  hContainer; /* Handle to container of type Dat              */
} DC_CAPABILITY, FAR * pDC_CAPABILITY;


/* DAT_CIECOLOR. */
typedef struct {
   DC_UINT16           ColorSpace;
   DC_INT16            LowEndian;
   DC_INT16            DeviceDependent;
   DC_INT32            VersionNumber;
   DC_TRANSFORMSTAGE   StageABC;
   DC_TRANSFORMSTAGE   StageLMN;
   DC_CIEPOINT         WhitePoint;
   DC_CIEPOINT         BlackPoint;
   DC_CIEPOINT         WhitePaper;
   DC_CIEPOINT         BlackInk;
   DC_FIX32            Samples[1];
} DC_CIECOLOR, FAR * pDC_CIECOLOR;


/* DAT_EVENT. For passing events down from the app to the DS. */
typedef struct {
   DC_MEMREF  pEvent;    /* Windows pMSG or Mac pEvent.                 */
   DC_UINT16  DCMessage; /* DC msg from data source, e.g. MSG_XFERREADY */
} DC_EVENT, FAR * pDC_EVENT;


/* DAT_GRAYRESPONSE */
typedef struct {
   DC_ELEMENT8         Response[1];
} DC_GRAYRESPONSE, FAR * pDC_GRAYRESPONSE;


/* DAT_IDENTITY. Identifies the program/library/code resource. */
typedef struct {
   DC_UINT32  Id;              /* Unique number.  In Windows, app hWnd      */
   _DC_VERSION Version;         /* Identifies the piece of code              */
   DC_UINT16  ProtocolMajor;   /* App and DS must set to DCON_PROTOCOLMAJOR */
   DC_UINT16  ProtocolMinor;   /* App and DS must set to DCON_PROTOCOLMINOR */
   DC_UINT32  SupportedGroups; /* Bit field OR combination of DG_ constants */
   DC_STR32   Manufacturer;    /* Manufacturer name, e.g. "Hewlett-Packard" */
   DC_STR32   ProductFamily;   /* Product family name, e.g. "ScanJet"       */
   DC_STR32   ProductName;     /* Product name, e.g. "ScanJet Plus"         */
} DC_IDENTITY, FAR * pDC_IDENTITY;


/* DAT_IMAGEINFO. App gets detailed image info from DS with this. */
typedef struct {
   DC_FIX32   XResolution;      /* Resolution in the horizontal             */
   DC_FIX32   YResolution;      /* Resolution in the vertical               */
   DC_INT32   ImageWidth;       /* Columns in the image, -1 if unknown by DS*/
   DC_INT32   ImageLength;      /* Rows in the image, -1 if unknown by DS   */
   DC_INT16   SamplesPerPixel;  /* Number of samples per pixel, 3 for RGB   */
   DC_INT16   BitsPerSample[8]; /* Number of bits for each sample           */
   DC_INT16   BitsPerPixel;     /* Number of bits for each padded pixel     */
   DC_BOOL    Planar;           /* True if Planar, False if chunky          */
   DC_INT16   PixelType;        /* How to interp data; photo interp (DCPT_) */
   DC_UINT16  Compression;      /* How the data is compressed (DCCP_xxxx)   */
} DC_IMAGEINFO, FAR * pDC_IMAGEINFO;


/* DAT_IMAGELAYOUT. Provides image layout information in current units. */
typedef struct {
   DC_FRAME   Frame;          /* Frame coords within larger document */
   DC_UINT32  DocumentNumber;
   DC_UINT32  PageNumber;     /* Reset when you go to next document  */
   DC_UINT32  FrameNumber;    /* Reset when you go to next page      */
} DC_IMAGELAYOUT, FAR * pDC_IMAGELAYOUT;


/* DAT_IMAGEMEMXFER. Used to pass image data (e.g. in strips) from DS to app.*/
typedef struct {
   DC_UINT16  Compression;  /* How the data is compressed                */
   DC_UINT32  BytesPerRow;  /* Number of bytes in a row of data          */
   DC_UINT32  Columns;      /* How many columns                          */
   DC_UINT32  Rows;         /* How many rows                             */
   DC_UINT32  XOffset;      /* How far from the side of the image        */
   DC_UINT32  YOffset;      /* How far from the top of the image         */
   DC_UINT32  BytesWritten; /* How many bytes written in Memory          */
   DC_MEMORY  Memory;       /* Mem struct used to pass actual image data */
} DC_IMAGEMEMXFER, FAR * pDC_IMAGEMEMXFER;


/* DAT_JPEGCOMPRESSION. Based on JPEG Draft International Std, ver 10918-1. */
typedef struct {
   DC_UINT16   ColorSpace;       /* One of the DCPT_xxxx values                */
   DC_UINT32   SubSampling;      /* Two word "array" for subsampling values    */
   DC_UINT16   NumComponents;    /* Number of color components in image        */
   DC_UINT16   RestartFrequency; /* Frequency of restart marker codes in MDU's */
   DC_UINT16   QuantMap[4];      /* Mapping of components to QuantTables       */
   DC_MEMREF   QuantTable[4];    /* Quantization tables                        */
   DC_UINT16   HuffmanMap[4];    /* Mapping of components to Huffman tables    */
   DC_MEMREF   HuffmanDC[2];     /* DC Huffman tables                          */
   DC_MEMREF   HuffmanAC[2];     /* AC Huffman tables                          */
} DC_JPEGCOMPRESSION, FAR * pDC_JPEGCOMPRESSION;


/* DAT_PALETTE8. Color palette when DCPT_PALETTE pixels xfer'd in mem buf. */
typedef struct {
   DC_UINT16    NumColors;   /* Number of colors in the color table.  */
   DC_UINT16    PaletteType; /* DCPA_xxxx, specifies type of palette. */
   DC_ELEMENT8  Colors[256]; /* Array of palette values starts here.  */
} DC_PALETTE8, FAR * pDC_PALETTE8;


/* DAT_PENDINGXFERS. Used with MSG_ENDXFER to indicate additional data. */
typedef struct {
   DC_UINT16 Count;    /* Number of additional "images" pending.      */
   DC_UINT32 Reserved;
} DC_PENDINGXFERS, FAR * pDC_PENDINGXFERS;


/* DAT_RGBRESPONSE */
typedef struct {
   DC_ELEMENT8         Response[1];
} DC_RGBRESPONSE, FAR * pRGBRESPONSE;


/* DAT_SETUPFILEXFER. Sets up DS to app data transfer via a file. */
typedef struct {
   DC_STR255 FileName;
   DC_UINT16 Format;   /* Any DCFF_ constant */
   DC_INT16  VRefNum;  /* Used for Mac only  */
} DC_SETUPFILEXFER, FAR * pDC_SETUPFILEXFER;


/* DAT_SETUPMEMXFER. Sets up DS to app data transfer via a memory buffer. */
typedef struct {
   DC_UINT32 MinBufSize;
   DC_UINT32 MaxBufSize;
   DC_UINT32 Preferred;
} DC_SETUPMEMXFER, FAR * pDC_SETUPMEMXFER;


/* DAT_STATUS. App gets detailed status info from a data source with this. */
typedef struct {
   DC_UINT16  ConditionCode; /* Any DCCC_ constant     */
   DC_UINT16  Reserved;      /* Future expansion space */
} DC_STATUS, FAR * pDC_STATUS;


/* DAT_USERINTERFACE. Coordinates UI between app and data source. */
typedef struct {
   DC_BOOL    ShowUI;  /* TRUE if DS should bring up its UI           */
   DC_BOOL    ModalUI; /* For Mac only - true if the DS's UI is modal */
   DC_HANDLE  hParent; /* For windows only - App window handle        */
} DC_USERINTERFACE, FAR * pDC_USERINTERFACE;




/****************************************************************************
 * Generic Constants                                                        *
 ****************************************************************************/

#define DCON_PROTOCOLMINOR   0
#define DCON_PROTOCOLMAJOR   1

#define DCON_ARRAY           3 /* indicates DC_ARRAY container       */
#define DCON_ENUMERATION     4 /* indicates DC_ENUMERATION container */
#define DCON_ONEVALUE        5 /* indicates DC_ONEVALUE container    */
#define DCON_RANGE           6 /* indicates DC_RANGE container       */

#define DCON_ICONID          962 /* res Id of icon used in USERSELECT lbox */
#define DCON_DSMID           461 /* res Id of the DSM version num resource */
#define DCON_DSMCODEID       63  /* res Id of the Mac SM Code resource     */

#define DCON_DONTCARE8       0xff
#define DCON_DONTCARE16      0xffff
#define DCON_DONTCARE32      0xffffffff


/* Flags used in DC_MEMORY structure. */
#define DCMF_APPOWNS     0x1
#define DCMF_DSMOWNS     0x2
#define DCMF_DSOWNS      0x4
#define DCMF_POINTER     0x8
#define DCMF_HANDLE      0x10


/* Palette types for DC_PALETTE8 */
#define DCPA_RGB         0
#define DCPA_GRAY        1
#define DCPA_CMY         2


/* There are four containers used for capabilities negotiation:
 *    DCON_ONEVALUE, DCON_RANGE, DCON_ENUMERATION, DCON_ARRAY
 * In each container structure ItemType can be DCTY_INT8, DCTY_INT16, etc.
 * The kind of data stored in the container can be determined by doing
 * DCItemSize[ItemType] where the following is defined in TWAIN glue code:
 *          DCItemSize[]= { sizeof(DC_INT8),
 *                          sizeof(DC_INT16),
 *                          etc.
 *                          sizeof(DC_UINT32) };
 *
 */

#define DCTY_INT8        0x0000    /* Means Item is a DC_INT8   */
#define DCTY_INT16       0x0001    /* Means Item is a DC_INT16  */
#define DCTY_INT32       0x0002    /* Means Item is a DC_INT32  */

#define DCTY_UINT8       0x0003    /* Means Item is a DC_UINT8  */
#define DCTY_UINT16      0x0004    /* Means Item is a DC_UINT16 */
#define DCTY_UINT32      0x0005    /* Means Item is a DC_UINT32 */

#define DCTY_BOOL        0x0006    /* Means Item is a DC_BOOL   */

#define DCTY_FIX32       0x0007    /* Means Item is a DC_FIX32  */

#define DCTY_FRAME       0x0008    /* Means Item is a DC_FRAME  */

#define DCTY_STR32       0x0009    /* Means Item is a DC_STR32  */
#define DCTY_STR64       0x000a    /* Means Item is a DC_STR64  */
#define DCTY_STR128      0x000b    /* Means Item is a DC_STR128 */
#define DCTY_STR255      0x000c    /* Means Item is a DC_STR255 */




/****************************************************************************
 * Capability Constants                                                     *
 ****************************************************************************/

/* ICAP_BITORDER values (BO_ means Bit Order) */
#define DCBO_LSBFIRST    0
#define DCBO_MSBFIRST    1


/* ICAP_COMPRESSION values (CP_ means ComPression ) */
#define DCCP_NONE        0
#define DCCP_PACKBITS    1
#define DCCP_GROUP31D    2 /* Follows CCITT spec (no End Of Line)          */
#define DCCP_GROUP31DEOL 3 /* Follows CCITT spec (has End Of Line)         */
#define DCCP_GROUP32D    4 /* Follows CCITT spec (use cap for K Factor)    */
#define DCCP_GROUP4      5 /* Follows CCITT spec                           */
#define DCCP_JPEG        6 /* Use capability for more info                 */
#define DCCP_LZW         7 /* Must license from Unisys and IBM to use      */


/* ICAP_IMAGEFILEFORMAT values (FF_means File Format)   */
#define DCFF_TIFF        0    /* Tagged Image File Format     */
#define DCFF_PICT        1    /* Macintosh PICT               */
#define DCFF_BMP         2    /* Windows Bitmap               */
#define DCFF_XBM         3    /* X-Windows Bitmap             */
#define DCFF_JFIF        4    /* JPEG File Interchange Format */


/* ICAP_FILTER values (FT_ means Filter Type) */
#define DCFT_RED         0
#define DCFT_GREEN       1
#define DCFT_BLUE        2
#define DCFT_NONE        3
#define DCFT_WHITE       4
#define DCFT_CYAN        5
#define DCFT_MAGENTA     6
#define DCFT_YELLOW      7
#define DCFT_BLACK       8


/* ICAP_LIGHTPATH values (LP_ means Light Path) */
#define DCLP_REFLECTIVE   0
#define DCLP_TRANSMISSIVE 1


/* ICAP_LIGHTSOURCE values (LS_ means Light Source) */
#define DCLS_RED         0
#define DCLS_GREEN       1
#define DCLS_BLUE        2
#define DCLS_NONE        3
#define DCLS_WHITE       4
#define DCLS_UV          5
#define DCLS_IR          6


/* ICAP_ORIENTATION values (OR_ means ORientation) */
#define DCOR_ROT0        0
#define DCOR_ROT90       1
#define DCOR_ROT180      2
#define DCOR_ROT270      3
#define DCOR_PORTRAIT    DCOR_ROT0
#define DCOR_LANDSCAPE   DCOR_ROT270


/* ICAP_PLANARCHUNKY values (PC_ means Planar/Chunky ) */
#define DCPC_CHUNKY      0
#define DCPC_PLANAR      1


/* ICAP_PIXELFLAVOR values (PF_ means Pixel Flavor) */
#define DCPF_CHOCOLATE   0  /* zero pixel represents darkest shade  */
#define DCPF_VANILLA     1  /* zero pixel represents lightest shade */


/* ICAP_PIXELTYPE values (PT_ means Pixel Type) */
#define DCPT_BW          0 /* Black and White */
#define DCPT_GRAY        1
#define DCPT_RGB         2
#define DCPT_PALETTE     3
#define DCPT_CMY         4
#define DCPT_CMYK        5
#define DCPT_YUV         6
#define DCPT_YUVK        7
#define DCPT_CIEXYZ      8


/* ICAP_SUPPORTEDSIZES values (SS_ means Supported Sizes) */
#define DCSS_NONE        0
#define DCSS_A4LETTER    1
#define DCSS_B5LETTER    2
#define DCSS_USLETTER    3
#define DCSS_USLEGAL     4


/* ICAP_XFERMECH values (SX_ means Setup XFer) */
#define DCSX_NATIVE      0
#define DCSX_FILE        1
#define DCSX_MEMORY      2


/* ICAP_UNITS values (UN_ means UNits) */
#define DCUN_INCHES      0
#define DCUN_CENTIMETERS 1
#define DCUN_PICAS       2
#define DCUN_POINTS      3
#define DCUN_TWIPS       4
#define DCUN_PIXELS      5




/****************************************************************************
 * Country Constants                                                        *
 ****************************************************************************/

#define DCCY_AFGHANISTAN   1001
#define DCCY_ALGERIA        213
#define DCCY_AMERICANSAMOA  684
#define DCCY_ANDORRA        033
#define DCCY_ANGOLA        1002
#define DCCY_ANGUILLA      8090
#define DCCY_ANTIGUA       8091
#define DCCY_ARGENTINA       54
#define DCCY_ARUBA          297
#define DCCY_ASCENSIONI     247
#define DCCY_AUSTRALIA       61
#define DCCY_AUSTRIA         43
#define DCCY_BAHAMAS       8092
#define DCCY_BAHRAIN        973
#define DCCY_BANGLADESH     880
#define DCCY_BARBADOS      8093
#define DCCY_BELGIUM         32
#define DCCY_BELIZE         501
#define DCCY_BENIN          229
#define DCCY_BERMUDA       8094
#define DCCY_BHUTAN        1003
#define DCCY_BOLIVIA        591
#define DCCY_BOTSWANA       267
#define DCCY_BRITAIN          6
#define DCCY_BRITVIRGINIS  8095
#define DCCY_BRAZIL          55
#define DCCY_BRUNEI         673
#define DCCY_BULGARIA       359
#define DCCY_BURKINAFASO   1004
#define DCCY_BURMA         1005
#define DCCY_BURUNDI       1006
#define DCCY_CAMAROON       237
#define DCCY_CANADA           2
#define DCCY_CAPEVERDEIS    238
#define DCCY_CAYMANIS      8096
#define DCCY_CENTRALAFREP  1007
#define DCCY_CHAD          1008
#define DCCY_CHILE           56
#define DCCY_CHINA           86
#define DCCY_CHRISTMASIS   1009
#define DCCY_COCOSIS       1009
#define DCCY_COLOMBIA        57
#define DCCY_COMOROS       1010
#define DCCY_CONGO         1011
#define DCCY_COOKIS        1012
#define DCCY_COSTA RICA     506
#define DCCY_CUBA           005
#define DCCY_CYPRUS         357
#define DCCY_CZECHOSLOVAKIA  42
#define DCCY_DENMARK         45
#define DCCY_DJIBOUTI      1013
#define DCCY_DOMINICA      8097
#define DCCY_DOMINCANREP   8098
#define DCCY_EASTERIS      1014
#define DCCY_ECUADOR        593
#define DCCY_EGYPT           20
#define DCCY_ELSALVADOR     503
#define DCCY_EQGUINEA      1015
#define DCCY_ETHIOPIA       251
#define DCCY_FALKLANDIS    1016
#define DCCY_FAEROEIS       298
#define DCCY_FIJIISLANDS    679
#define DCCY_FINLAND        358
#define DCCY_FRANCE          33
#define DCCY_FRANTILLES     596
#define DCCY_FRGUIANA       594
#define DCCY_FRPOLYNEISA    689
#define DCCY_FUTANAIS      1043
#define DCCY_GABON          241
#define DCCY_GAMBIA         220
#define DCCY_GERMANY         49
#define DCCY_GHANA          233
#define DCCY_GIBRALTER      350
#define DCCY_GREECE          30
#define DCCY_GREENLAND      299
#define DCCY_GRENADA       8099
#define DCCY_GRENEDINES    8015
#define DCCY_GUADELOUPE     590
#define DCCY_GUAM           671
#define DCCY_GUANTANAMOBAY 5399
#define DCCY_GUATEMALA      502
#define DCCY_GUINEA         224
#define DCCY_GUINEABISSAU  1017
#define DCCY_GUYANA         592
#define DCCY_HAITI          509
#define DCCY_HONDURAS       504
#define DCCY_HONG KONG      852
#define DCCY_HUNGARY         36
#define DCCY_ICELAND        354
#define DCCY_INDIA           91
#define DCCY_INDONESIA       62
#define DCCY_IRAN            98
#define DCCY_IRAQ           964
#define DCCY_IRELAND        353
#define DCCY_ISRAEL         972
#define DCCY_ITALY           39
#define DCCY_IVORY COAST    225
#define DCCY_JAMAICA       8010
#define DCCY_JAPAN           81
#define DCCY_JORDAN         962
#define DCCY_KENYA          254
#define DCCY_KIRIBATI      1018
#define DCCY_KOREA           82
#define DCCY_KUWAIT         965
#define DCCY_LAOS          1019
#define DCCY_LEBANON       1020
#define DCCY_LIBERIA        231
#define DCCY_LIBYA          218
#define DCCY_LIECHTENSTEIN   41
#define DCCY_LUXENBOURG     352
#define DCCY_MACAO          853
#define DCCY_MADAGASCAR    1021
#define DCCY_MALAWI         265
#define DCCY_MALAYSIA        60
#define DCCY_MALDIVES       960
#define DCCY_MALI          1022
#define DCCY_MALTA          356
#define DCCY_MARSHALLIS     692
#define DCCY_MAURITANIA    1023
#define DCCY_MAURITIUS      230
#define DCCY_MEXICO           3
#define DCCY_MICRONESIA     691
#define DCCY_MIQUELON       508
#define DCCY_MONACO          33
#define DCCY_MONGOLIA      1024
#define DCCY_MONTSERRAT    8011
#define DCCY_MOROCCO        212
#define DCCY_MOZAMBIQUE    1025
#define DCCY_NAMIBIA        264
#define DCCY_NAURU         1026
#define DCCY_NEPAL          977
#define DCCY_NETHERLANDS     31
#define DCCY_NETHANTILLES   599
#define DCCY_NEVIS         8012
#define DCCY_NEWCALEDONIA   687
#define DCCY_NEWZEALAND      64
#define DCCY_NICARAGUA      505
#define DCCY_NIGER          227
#define DCCY_NIGERIA        234
#define DCCY_NIUE          1027
#define DCCY_NORFOLKI      1028
#define DCCY_NORWAY          47
#define DCCY_OMAN           968
#define DCCY_PAKISTAN        92
#define DCCY_PALAU         1029
#define DCCY_PANAMA         507
#define DCCY_PARAGUAY       595
#define DCCY_PERU            51
#define DCCY_PHILLIPPINES    63
#define DCCY_PITCAIRNIS    1030
#define DCCY_PNEWGUINEA     675
#define DCCY_POLAND          48
#define DCCY_PORTUGAL       351
#define DCCY_QATAR          974
#define DCCY_REUNIONI      1031
#define DCCY_ROMANIA         40
#define DCCY_RWANDA         250
#define DCCY_SAIPAN         670
#define DCCY_SANMARINO      039
#define DCCY_SAOTOME       1033
#define DCCY_SAUDIARABIA    966
#define DCCY_SENEGAL        221
#define DCCY_SEYCHELLESIS  1034
#define DCCY_SIERRALEONE   1035
#define DCCY_SINGAPORE       65
#define DCCY_SOLOMONIS     1036
#define DCCY_SOMALI        1037
#define DCCY_SOUTH AFRICA    27
#define DCCY_SPAIN           34
#define DCCY_SRILANKA        94
#define DCCY_STHELENA      1032
#define DCCY_STKITTS       8013
#define DCCY_STLUCIA       8014
#define DCCY_STPIERRE       508
#define DCCY_STVINCENT     8015
#define DCCY_SUDAN         1038
#define DCCY_SURINAME       597
#define DCCY_SWAZILAND      268
#define DCCY_SWEDEN          46
#define DCCY_SWITZERLAND     41
#define DCCY_SYRIA         1039
#define DCCY_TAIWAN         886
#define DCCY_TANZANIA       255
#define DCCY_THAILAND        66
#define DCCY_TOBAGO        8016
#define DCCY_TOGO           228
#define DCCY_TONGAIS        676
#define DCCY_TRINIDAD      8016
#define DCCY_TUNISIA        216
#define DCCY_TURKEY          90
#define DCCY_TURKSCAICOS   8017
#define DCCY_TUVALU        1040
#define DCCY_UGANDA         256
#define DCCY_USSR             7
#define DCCY_UAEMIRATES     971
#define DCCY_UNITEDKINGDOM   44
#define DCCY_USA              1
#define DCCY_URUGUAY        598
#define DCCY_VANUATU       1041
#define DCCY_VATICANCITY     39
#define DCCY_VENEZUELA       58
#define DCCY_WAKE          1042
#define DCCY_WALLISIS      1043
#define DCCY_WESTERNSAHARA 1044
#define DCCY_WESTERNSAMOA  1045
#define DCCY_YEMEN         1046
#define DCCY_YUGOSLAVIA      38
#define DCCY_ZAIRE          243
#define DCCY_ZAMBIA         260
#define DCCY_ZIMBABWE       263

 
  

/****************************************************************************
 * Language Constants                                                       *
 ****************************************************************************/

#define DCLG_DAN              0 /* Danish                 */
#define DCLG_DUT              1 /* Dutch                  */
#define DCLG_ENG              2 /* International English  */
#define DCLG_FCF              3 /* French Canadian        */
#define DCLG_FIN              4 /* Finnish                */
#define DCLG_FRN              5 /* French                 */
#define DCLG_GER              6 /* German                 */
#define DCLG_ICE              7 /* Icelandic              */
#define DCLG_ITN              8 /* Italian                */
#define DCLG_NOR              9 /* Norwegian              */
#define DCLG_POR             10 /* Portuguese             */
#define DCLG_SPA             11 /* Spanish                */
#define DCLG_SWE             12 /* Swedish                */
#define DCLG_USA             13 /* U.S. English           */




/****************************************************************************
 * Data Groups                                                              *
 ****************************************************************************/

/* More Data Groups may be added in the future.
 * Possible candidates include text, vector graphics, sound, etc.
 * NOTE: Data Group constants must be powers of 2 as they are used
 *       as bitflags when App asks DSM to present a list of DSs.
 */

#define DG_CONTROL          0x0001L /* data pertaining to control       */
#define DG_IMAGE            0x0002L /* data pertaining to raster images */




/****************************************************************************
 * Data Argument Types                                                      *
 ****************************************************************************/

#define DAT_NULL            0x0000 /* No data or structure. */
#define DAT_CUSTOMBASE      0x8000 /* Base of custom DATs. */


/* Data Argument Types for the DG_CONTROL Data Group. */
#define DAT_CAPABILITY      0x0001 /* DC_CAPABILITY                        */
#define DAT_EVENT           0x0002 /* DC_EVENT                             */
#define DAT_IDENTITY        0x0003 /* DC_IDENTITY                          */
#define DAT_PARENT          0x0004 /* DC_HANDLE, app win handle in Windows */
#define DAT_PENDINGXFERS    0x0005 /* DC_PENDINGXFERS                      */
#define DAT_SETUPMEMXFER    0x0006 /* DC_SETUPMEMXFER                      */
#define DAT_SETUPFILEXFER   0x0007 /* DC_SETUPFILEXFER                     */
#define DAT_STATUS          0x0008 /* DC_STATUS                            */
#define DAT_USERINTERFACE   0x0009 /* DC_USERINTERFACE                     */
#define DAT_XFERGROUP       0x000a /* DC_UINT32                            */


/* Data Argument Types for the DG_IMAGE Data Group. */
#define DAT_IMAGEINFO       0x0101 /* DC_IMAGEINFO                         */
#define DAT_IMAGELAYOUT     0x0102 /* DC_IMAGELAYOUT                       */
#define DAT_IMAGEMEMXFER    0x0103 /* DC_IMAGEMEMXFER                      */
#define DAT_IMAGENATIVEXFER 0x0104 /* DC_UINT32 loword is hDIB, PICHandle  */
#define DAT_IMAGEFILEXFER   0x0105 /* Null data                            */
#define DAT_CIECOLOR        0x0106 /* DC_CIECOLOR                          */
#define DAT_GRAYRESPONSE    0x0107 /* DC_GRAYRESPONSE                      */
#define DAT_RGBRESPONSE     0x0108 /* DC_RGBRESPONSE                       */
#define DAT_JPEGCOMPRESSION 0x0109 /* DC_JPEGCOMPRESSION                   */
#define DAT_PALETTE8        0x010a /* DC_PALETTE8                          */




/****************************************************************************
 * Messages                                                                 *
 ****************************************************************************/

/* All message constants are unique.
 * Messages are grouped according to which DATs they are used with.*/

#define MSG_NULL         0x0000 /* Used in DC_EVENT structure               */
#define MSG_CUSTOMBASE   0x8000 /* Base of custom messages                  */


/* Generic messages may be used with any of several DATs.                   */
#define MSG_GET          0x0001 /* Get one or more values                   */
#define MSG_GETCURRENT   0x0002 /* Get current value                        */
#define MSG_GETDEFAULT   0x0003 /* Get default (e.g. power up) value        */
#define MSG_GETFIRST     0x0004 /* Get first of a series of items, e.g. DSs */
#define MSG_GETNEXT      0x0005 /* Iterate through a series of items.       */
#define MSG_SET          0x0006 /* Set one or more values                   */
#define MSG_RESET        0x0007 /* Set current value to default value       */


/* Messages used with DAT_NULL                                              */
#define MSG_XFERREADY    0x0101 /* The data source has data ready           */
#define MSG_CLOSEDSREQ   0x0102 /* Request for App. to close DS             */


/* Messages used with a pointer to a DAT_STATUS structure                   */
#define MSG_CHECKSTATUS  0x0201 /* Get status information                   */


/* Messages used with a pointer to DAT_PARENT data                          */
#define MSG_OPENDSM      0x0301 /* Open the DSM                             */
#define MSG_CLOSEDSM     0x0302 /* Close the DSM                            */


/* Messages used with a pointer to a DAT_IDENTITY structure                 */
#define MSG_OPENDS       0x0401 /* Open a data source                       */
#define MSG_CLOSEDS      0x0402 /* Close a data source                      */
#define MSG_USERSELECT   0x0403 /* Put up a dialog of all DS                */


/* Messages used with a pointer to a DAT_USERINTERFACE structure            */
#define MSG_DISABLEDS    0x0501 /* Disable data transfer in the DS          */
#define MSG_ENABLEDS     0x0502 /* Enable data transfer in the DS           */


/* Messages used with a pointer to a DAT_EVENT structure                    */
#define MSG_PROCESSEVENT 0x0601


/* Messages used with a pointer to a DAT_PENDINGXFERS structure             */
#define MSG_ENDXFER      0x0701




/****************************************************************************
 * Capabilities                                                             *
 ****************************************************************************/

#define CAP_CUSTOMBASE         0x8000 /* Base of custom capabilities */


/* all data sources are REQUIRED to support these caps */
#define CAP_XFERCOUNT          0x0001


/* image data sources are REQUIRED to support these caps */
#define ICAP_COMPRESSION       0x0100
#define ICAP_PIXELTYPE         0x0101
#define ICAP_UNITS             0x0102 /* default is DCON_INCHES */
#define ICAP_XFERMECH          0x0103


/* all data sources MAY support these caps */
#define CAP_AUTHOR             0x1000
#define CAP_CAPTION            0x1001
#define CAP_FEEDERENABLED      0x1002
#define CAP_FEEDERLOADED       0x1003
#define CAP_TIMEDATE           0x1004
#define CAP_SUPPORTEDCAPS      0x1005
#define CAP_EXTENDEDCAPS       0x1006
#define CAP_AUTOFEED           0x1007
#define CAP_CLEARPAGE          0x1008
#define CAP_FEEDPAGE           0x1009
#define CAP_REWINDPAGE         0x100a


/* image data sources MAY support these caps */
#define ICAP_AUTOBRIGHT        0x1100
#define ICAP_BRIGHTNESS        0x1101
#define ICAP_CONTRAST          0x1103
#define ICAP_CUSTHALFTONE      0x1104
#define ICAP_EXPOSURETIME      0x1105
#define ICAP_FILTER            0x1106
#define ICAP_FLASHUSED         0x1107
#define ICAP_GAMMA             0x1108
#define ICAP_HALFTONES         0x1109
#define ICAP_HIGHLIGHT         0x110a
#define ICAP_IMAGEFILEFORMAT   0x110c
#define ICAP_LAMPSTATE         0x110d
#define ICAP_LIGHTSOURCE       0x110e
#define ICAP_ORIENTATION       0x1110
#define ICAP_PHYSICALWIDTH     0x1111
#define ICAP_PHYSICALHEIGHT    0x1112
#define ICAP_SHADOW            0x1113
#define ICAP_FRAMES            0x1114
#define ICAP_XNATIVERESOLUTION 0x1116
#define ICAP_YNATIVERESOLUTION 0x1117
#define ICAP_XRESOLUTION       0x1118
#define ICAP_YRESOLUTION       0x1119
#define ICAP_MAXFRAMES         0x111a
#define ICAP_TILES             0x111b
#define ICAP_BITORDER          0x111c
#define ICAP_CCITTKFACTOR      0x111d
#define ICAP_LIGHTPATH         0x111e
#define ICAP_PIXELFLAVOR       0x111f
#define ICAP_PLANARCHUNKY      0x1120
#define ICAP_ROTATION          0x1121
#define ICAP_SUPPORTEDSIZES    0x1122
#define ICAP_THRESHOLD         0x1123
#define ICAP_XSCALING          0x1124
#define ICAP_YSCALING          0x1125
#define ICAP_BITORDERCODES     0x1126
#define ICAP_PIXELFLAVORCODES  0x1127
#define ICAP_JPEGPIXELTYPE     0x1128
#define ICAP_TIMEFILL          0x112a
#define ICAP_BITDEPTH          0x112b




/***************************************************************************
 *            Return Codes and Condition Codes section                     *
 ***************************************************************************/

/* Return Codes: DSM_Entry and DS_Entry may return any one of these values. */
#define DCRC_CUSTOMBASE     0x8000

#define DCRC_SUCCESS         0
#define DCRC_FAILURE         1 /* App may get DC_STATUS for info on failure */
#define DCRC_CHECKSTATUS     2 /* "tried hard"; get status                  */
#define DCRC_CANCEL          3
#define DCRC_DSEVENT         4
#define DCRC_NOTDSEVENT      5
#define DCRC_XFERDONE        6
#define DCRC_ENDOFLIST       7 /* After MSG_GETNEXT if nothing left         */


/* Condition Codes: App gets these by doing DG_CONTROL DAT_STATUS MSG_GET.  */
#define DCCC_CUSTOMBASE     0x8000

#define DCCC_SUCCESS         0 /* It worked!                                */
#define DCCC_BUMMER          1 /* Failure due to unknown causes             */
#define DCCC_LOWMEMORY       2 /* Not enough memory to perform operation    */
#define DCCC_NODS            3 /* No Data Source                            */
#define DCCC_MAXCONNECTIONS  4 /* DS is connected to max possible apps      */
#define DCCC_OPERATIONERROR  5 /* DS or DSM reported error, app shouldn't   */
#define DCCC_BADCAP          6 /* Unknown capability                        */
#define DCCC_BADPROTOCOL     9 /* Unrecognized MSG DG DAT combination       */
#define DCCC_BADVALUE       10 /* Data parameter out of range               */
#define DCCC_SEQERROR       11 /* DG DAT MSG out of expected sequence       */
#define DCCC_BADDEST        12 /* Unknown destination App/Src in DSM_Entry  */




/****************************************************************************
 * Entry Points                                                             *
 ****************************************************************************/


/**********************************************************************
 * Function: DSM_Entry, the only entry point into the Data Source Manager.
 *
 * Parameters:
 *  pOrigin Identifies the source module of the message. This could
 *          identify an Application, a Source, or the Source Manager.
 *
 *  pDest   Identifies the destination module for the message.
 *          This could identify an application or a data source.
 *          If this is NULL, the message goes to the Source Manager.
 *
 *  DG      The Data Group. 
 *          Example: DG_IMAGE.
 *
 *  DAT     The Data Attribute Type.
 *          Example: DAT_IMAGEMEMXFER.
 *    
 *  MSG     The message.  Messages are interpreted by the destination module
 *          with respect to the Data Group and the Data Attribute Type.  
 *          Example: MSG_GET.
 *
 *  pData   A pointer to the data structure or variable identified 
 *          by the Data Attribute Type.
 *          Example: (DC_MEMREF)&ImageMemXfer
 *                   where ImageMemXfer is a DC_IMAGEMEMXFER structure.
 *                    
 * Returns:
 *  ReturnCode
 *         Example: DCRC_SUCCESS.
 *
 ********************************************************************/

/* Don't mangle the name "DSM_Entry" if we're compiling in C++! */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSWIN_
DC_UINT16 FAR PASCAL DSM_Entry( pDC_IDENTITY pOrigin,
                                pDC_IDENTITY pDest,
                                DC_UINT32    DG,
                                DC_UINT16    DAT,
                                DC_UINT16    MSG,
                                DC_MEMREF    pData);

typedef DC_UINT16 (FAR PASCAL *DSMENTRYPROC)(pDC_IDENTITY, pDC_IDENTITY,
                                             DC_UINT32,    DC_UINT16,
                                             DC_UINT16,    DC_MEMREF);
#else

FAR PASCAL DC_UINT16 DSM_Entry( pDC_IDENTITY pOrigin,
                                pDC_IDENTITY pDest,
                                DC_UINT32    DG,
                                DC_UINT16    DAT,
                                DC_UINT16    MSG,
                                DC_MEMREF    pData);

typedef DC_UINT16 (*DSMENTRYPROC)(pDC_IDENTITY, pDC_IDENTITY,
                                  DC_UINT32,    DC_UINT16,
                                  DC_UINT16,    DC_MEMREF);
#endif

#ifdef __cplusplus
}
#endif


/**********************************************************************
 * Function: DS_Entry, the entry point provided by a Data Source.
 *
 * Parameters:
 *  pOrigin Identifies the source module of the message. This could
 *          identify an application or the Data Source Manager.
 *
 *  DG      The Data Group. 
 *          Example: DG_IMAGE.
 *           
 *  DAT     The Data Attribute Type.
 *          Example: DAT_IMAGEMEMXFER.
 *    
 *  MSG     The message.  Messages are interpreted by the data source
 *          with respect to the Data Group and the Data Attribute Type.
 *          Example: MSG_GET.
 *
 *  pData   A pointer to the data structure or variable identified 
 *          by the Data Attribute Type.
 *          Example: (DC_MEMREF)&ImageMemXfer
 *                   where ImageMemXfer is a DC_IMAGEMEMXFER structure.
 *                    
 * Returns:
 *  ReturnCode
 *          Example: DCRC_SUCCESS.
 *
 * Note:
 *  The DSPROC type is only used by an application when it calls
 *  a Data Source directly, bypassing the Data Source Manager.
 *
 ********************************************************************/
#ifdef _MSWIN_
DC_UINT16 FAR PASCAL DS_Entry( pDC_IDENTITY pOrigin,
                               DC_UINT32    DG, 
                               DC_UINT16    DAT, 
                               DC_UINT16    MSG, 
                               DC_MEMREF    pData);

typedef DC_UINT16 (FAR PASCAL *DSENTRYPROC)(pDC_IDENTITY,
                                             DC_UINT32,    DC_UINT16,
                                             DC_UINT16,    DC_MEMREF);

#else
FAR PASCAL DC_UINT16 DS_Entry( pDC_IDENTITY pOrigin, 
                               DC_UINT32    DG, 
                               DC_UINT16    DAT, 
                               DC_UINT16    MSG, 
                               DC_MEMREF    pData);

typedef DC_UINT16 (*DSENTRYPROC)(pDC_IDENTITY,
                                  DC_UINT32,    DC_UINT16,
                                  DC_UINT16,    DC_MEMREF);
#endif




#endif /* TWAIN */

