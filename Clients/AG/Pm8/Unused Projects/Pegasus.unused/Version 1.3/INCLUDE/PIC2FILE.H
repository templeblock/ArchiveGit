/*.T PIC2FILE.H  Include file describing PIC2 File Format
 * $Header: /PM8/Pegasus/Version 1.3/INCLUDE/PIC2FILE.H 1     3/03/99 6:23p Gbeddow $
 * $Nokeywords: $
 */

/***************************************************************************\
*       Copyright (c) 1997, Pegasus Imaging Corporation                     *
*       All rights reserved.                                                *
*****************************************************************************
*       Revision History:                                                   *
* created 03/14/97 - jim                                                    *
\***************************************************************************/



#if !defined(_PIC2FILE)
#define _PIC2FILE

#include "pic.h"

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



#define P2_Signature            "PIC2"
#define P2_Version              100



/* bit flags for File Version Packet Flags field */
#define P2F_CRC                 0x0001
#define P2F_Protected           0x0002
#define P2F_VirtualImages       0x0004
#define P2F_TableOfClusters     0x0008
#define P2F_MultipleImages      0x0010
#define P2F_RecursiveImages     0x0020



/* bit flags for Cluster Header Packet Flags field */
#define P2C_Deleted     0x0001
#define P2C_Protected   0x0010
#define P2C_Virtual     0x0020
#define P2C_Relative    0x0040
#define P2C_Recursive   0x0080



/* values for Packet Type field */
#define P2P_EOF                 0
#define P2P_FileVersion         1
#define P2P_Script              2
#define P2P_FileDescription     3
#define P2P_TableOfClusters     4
#define P2P_ClusterHeader       5
#define P2P_ManifestFile        6
#define P2P_VirtualFile         7
#define P2P_RelativeFile        8
#define P2P_Copyright           9
#define P2P_AccessInformation   10
#define P2P_Promotion           11
#define P2P_RecursiveFile       12
#define P2P_TileDescription     17
#define P2P_PhysicalDimensions  18
#define P2P_Region				19
#define P2P_BMPHeader           20
#define P2P_Orientation         21
#define P2P_Comment             22
#define P2P_RawData             23
#define P2P_Watermark           24
#define P2P_ColorPalette        25
#define P2P_GammaLuminanceTable 26
#define P2P_GammaRGBTable       27
#define P2P_Filter              28
#define P2P_UserDecryption      29
#define P2P_PageInfo            30
#define P2P_DisplayInfo         31
#define P2P_ProgressiveLayers   32
#define P2P_LosslessInfo        33
#define P2P_WaveletInfo			34
#define P2P_Skip                254
#define P2P_Extended            255



/* P2DATE is used by the Access Information Packet */
typedef struct
{
    BYTE Day;
    BYTE Month;
    WORD Year;
} P2DATE;

typedef struct
{
    DWORD Width;
    DWORD Height;
    DWORD ComponentSize;
} P2LAYER;


/********************************************************************/
/* Packet data structures                                           */
/********************************************************************/

typedef struct
{
    BYTE  Type;
    DWORD Length;
    WORD  Version;
    WORD  Flags;
    DWORD PIC2Length;
} P2PktFileVersion;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  ScriptText[1];
} P2PktScript;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  FileDescription[1];
} P2PktFileDescription;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    WORD  k;
    DWORD Offset[1];
} P2PktTableOfClusters;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    DWORD Filetype;
    WORD  Version;
    WORD  Flags;
    CHAR  FileName[1];
} P2PktClusterHeader;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BYTE  ActualFileContents[1];
} P2PktManifestFile;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  PathName[1];
} P2PktVirtualFile;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    DWORD PIC2FileOffset;
} P2PktRelativeFile;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  Copyright[1];
} P2PktCopyright;

typedef struct
{
    BYTE   Type;
    DWORD  Length;
    P2DATE DateCreated;
    P2DATE DateLastModified;
    P2DATE DateLastAccessed;
    DWORD  NumberAccesses;
} P2PktAccessInformation;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  Promotion[1];
} P2PktPromotion;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BYTE  Data[1];
} P2PktRecursiveFile;

typedef struct
{
	BYTE  Type;
	DWORD Length;
	WORD  Flags;
	WORD  BoxWidth;
	WORD  BoxHeight;
	WORD  TileWidth;
	WORD  TileHeight;
} P2PktTileDescription;

typedef struct
{
    BYTE  Type;
    DWORD Length;
	BYTE  Units;	/* 0 none, 1 inches, 2 cm */
	WORD  XNum;
	WORD  XDen;
	WORD  YNum;
	WORD  YDen;
} P2PktPhysicalDimensions;

typedef struct
{
    BYTE  Type;
    DWORD Length;
	REGION Region;
} P2PktRegion;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BITMAPINFOHEADER bmih;
} P2PktBMPHeader;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    WORD  Orientation;
} P2PktOrientation;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  Comment[1];
} P2PktComment;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  RawDescription[4];
    DWORD RawLength;
    BYTE  RawData[1];
} P2PktRawData;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    CHAR  Watermark[1];
} P2PktWatermark;

typedef struct
{
    BYTE      Type;
    DWORD     Length;
    WORD      FirstColor;
    WORD      NumColors;
    RGBTRIPLE Colors[1];
} P2PktColorPalette;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BYTE  GammaLuminanceTable[256];
    CHAR  Description[1];    /* (optional) */
} P2PktGammaLuminanceTable;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BYTE  BlueGammaRGBTable[256];
    BYTE  GreenGammaRGBTable[256];
    BYTE  RedGammaRGBTable[256];
    CHAR  Description[1];    /* (optional) */
} P2PktGammaRGBTable;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    WORD  k;
    WORD  WeightTable[1];    /* index is actually k * k */
/*  WORD  Divisor; */        /* == &WeightTable[k * k]  */
/*  CHAR  Description; */    /* (optional) == &WeightTable[k * k + 1] */
} P2PktFilter;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    WORD  Method;
    WORD  Version;
    BYTE  RelevantData[1];
} P2PktUserDecryption;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    DWORD PageNum;
    DWORD NumOfPages;
} P2PktPageInfo;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    WORD  TransparentColorIndex;
    WORD  DispositionMethod;
    WORD  Delay;
    DWORD XOffset;
    DWORD YOffset;
} P2PktDisplayInfo;

typedef struct
{
    BYTE    Type;
    DWORD   Length;
    BYTE    NumLayers;
    P2LAYER LayerDescription[1];
} P2PktProgressiveLayers;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BYTE  CompressionMethod;
    BYTE  CompressionOrder;
    BYTE  HashMethod;
    BYTE  AllowedBitErr;    
} P2PktLosslessInfo;

typedef struct
{
	BYTE  Type;
	DWORD Length;
	WORD  Quality;
	WORD  MinThreshold;
	WORD  MaxThreshold;
} P2PktWaveletInfo;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    BYTE  DataToSkip[1];
} P2PktSkip;

typedef struct
{
    BYTE  Type;
    DWORD Length;
    DWORD ExtendedType;
    BYTE  Data[1];
} P2PktExtended;



typedef struct
{
    BYTE Type;
    DWORD Length;
} P2PktNull;

typedef struct
{
    BYTE Type;
    DWORD Length;
    BYTE Data[1];
} P2PktGeneric;



typedef union
{
    P2PktNull                NullPkt;
    P2PktGeneric             Generic;
    P2PktFileVersion         FileVersion;
    P2PktScript              Script;
    P2PktFileDescription     FileDescription;
    P2PktTableOfClusters     TableOfClusters;
    P2PktClusterHeader       ClusterHeader;
    P2PktManifestFile        ManifestFile;
    P2PktVirtualFile         VirtualFile;
    P2PktRelativeFile        RelativeFile;
    P2PktCopyright           Copyright;
    P2PktAccessInformation   AccessInformation;
    P2PktPromotion           Promotion;
	P2PktRecursiveFile		 RecursiveFile;
	P2PktTileDescription	 TileDescription;
	P2PktPhysicalDimensions  PhysicalDimensions;
	P2PktRegion				 Region;
    P2PktBMPHeader           BMPHeader;
    P2PktOrientation         Orientation;
    P2PktComment             Comment;
    P2PktRawData             RawData;
    P2PktWatermark           Watermark;
    P2PktColorPalette        ColorPalette;
    P2PktGammaLuminanceTable GammaLuminanceTable;
    P2PktGammaRGBTable       GammaRGBTable;
    P2PktFilter              Filter;
    P2PktUserDecryption      UserDecryption;
    P2PktPageInfo            PageInfo;
    P2PktDisplayInfo         DisplayInfo;
    P2PktProgressiveLayers   ProgressiveLayers;
    P2PktLosslessInfo        LosslessInfo;
	P2PktWaveletInfo		 WaveletInfo;
    P2PktSkip                Skip;
    P2PktExtended            Extended;
} P2Pkt;



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

#endif  /* #if !defined(_PIC2FILE) */
