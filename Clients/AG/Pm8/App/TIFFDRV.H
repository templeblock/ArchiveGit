/*
// Definitions for the TIFF handling code.
//
// $Header: /PM8/App/TIFFDRV.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TIFFDRV.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 6     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 5     11/21/97 5:21p Johno
// General update
// 
// 4     11/20/97 10:37a Johno
// Use CDIBInfo::PrepareData
// 
// 3     11/19/97 2:38p Johno
// Re - sync to Source Safe
// 
// 2     11/18/97 5:50p Johno
// CDIBtoTIFFWriter
// 
//    Rev 1.0   14 Aug 1997 15:26:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:18   Fred
// Initial revision.
// 
//    Rev 1.2   05 Dec 1996 12:30:42   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   08 Nov 1996 08:57:48   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.0   14 Mar 1996 13:42:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:06   FRED
// Initial revision.
// 
//    Rev 1.3   18 Sep 1995 09:30:26   JAY
// Fixed def of PACKBTS.
// 
//    Rev 1.2   15 Sep 1995 14:04:16   JAY
// More 32-bit changes.
// 
//    Rev 1.1   20 Jun 1995 15:46:30   JAY
// Wasn't handling "missing" virtual files correctly because it wasn't
// catching the open error.
// 
//    Rev 1.0   29 Mar 1995 09:00:20   JAY
// Initial revision.
*/

#ifndef __TIFFDRV_H__
#define __TIFFDRV_H__

#include "imagedrv.h"
#include "file.h"
#include "lzw.h"
// Bits per sample 
struct BPS
{
   WORD d1;
   WORD d2;
   WORD d3;
   BPS (WORD s)
   {
      d1 = d2 = d3 = s;
   }
};
/* Byte orders */
#define BYTE_ORDER_BACKWARD   0x4949	/* Least significant first */
#define BYTE_ORDER_FORWARD    0x4d4d	/* Most significant first */
#define BYTE_ORDER_IBM        BYTE_ORDER_BACKWARD	/* PCs use the former */

/* TIFF version */
#define TIFF_VERSION	42

/* Subfile Types */
#define	SUBFILE_TYPE_NORMAL	1
#define	SUBFILE_TYPE_REDUCED	2

/* Data types */
typedef enum
{
	TIFF_BYTE		=	1,
	TIFF_ASCII		=	2,
	TIFF_SHORT		=	3,
	TIFF_LONG		=	4,
	TIFF_RATIONAL	=	5,
/* New with TIFF 6.0 */ 
	TIFF_SBYTE		=	6,
	TIFF_UNDEFINED	=	7,
	TIFF_SSHORT		=	8,
	TIFF_SLONG		=	9,
	TIFF_SRATIONAL	=	10,
	TIFF_FLOAT		=	11,
	TIFF_DOUBLE		=	12,
	MAX_TIFF_TAG_TYPE
} TIFF_TAG_TYPE;

/* Compression values. */

#define	TIFF_COMP_NONE			1
#define	TIFF_COMP_CCITT		2
#define	TIFF_COMP_LZW			5
#define	TIFF_COMP_PACKBITS	((WORD)0x8005)		/* 32773 */

typedef struct
{
	LONG	numerator;
	LONG	denominator;
} RATIONAL;

typedef union
{
	LONG Long;
	UCHAR Byte;
	SHORT Short;
	RATIONAL Rational;
	VOIDPTR Vfptr;
} TIFF_DATA;

/* Some field types */
typedef enum
{
	TIFF_NewSubfileType			= 254,
	TIFF_SubfileType			   = 255,
	TIFF_ImageWidth				= 256,
	TIFF_ImageLength				= 257,
	TIFF_BitsPerSample			= 258,
	TIFF_Compression				= 259,
	TIFF_PhotometricInterp		= 262,
	TIFF_Threshholding			= 263,
	TIFF_CellWidth					= 264,
	TIFF_CellLength				= 265,
	TIFF_FillOrder					= 266,
	TIFF_DocumentName				= 269,
	TIFF_ImageDescription		= 270,
	TIFF_Make						= 271,
	TIFF_Model						= 272,
	TIFF_StripOffsets				= 273,
	TIFF_Orientation				= 274,
	TIFF_SamplesPerPixel			= 277,
	TIFF_RowsPerStrip				= 278,
	TIFF_StripByteCounts			= 279,
	TIFF_MinSampleValue			= 280,
	TIFF_MaxSampleValue			= 281,
	TIFF_XResolution				= 282,
	TIFF_YResolution				= 283,
	TIFF_PlanarConfiguration	= 284,
	TIFF_PageName					= 285,
	TIFF_XPosition					= 286,
	TIFF_YPosition					= 287,
	TIFF_GrayResponseUnit		= 290,
	TIFF_GrayResponseCurve		= 291,
   TIFF_ResolutionUnit        = 296,
   TIFF_Software              = 305,
	TIFF_Predictor					= 317,
	TIFF_ColorMap					= 320
} TIFF_TAG_TAG;
/* Some field indexes. */

#ifdef EXTENSIVE_TIFF

/* Remember many fields in TIFF file. */

#define Tindex_SubfileType			0
#define Tindex_ImageWidth			1
#define Tindex_ImageLength			2
#define Tindex_BitsPerSample		3
#define Tindex_Compression			4
#define Tindex_PhotometricInterp	5
#define Tindex_Threshholding		6
#define Tindex_CellWidth			7
#define Tindex_CellLength			8
#define Tindex_FillOrder			9
#define Tindex_DocumentName		10
#define Tindex_ImageDescription	11
#define Tindex_Make					12
#define Tindex_Model					13
#define Tindex_StripOffsets		14
#define Tindex_Orientation			15
#define Tindex_SamplesPerPixel	16
#define Tindex_RowsPerStrip		17
#define Tindex_StripByteCounts	18
#define Tindex_MinSampleValue		19
#define Tindex_MaxSampleValue		20
#define Tindex_XResolution			21
#define Tindex_YResolution			22
#define Tindex_PlanarConfiguration 23
#define Tindex_PageName				24
#define Tindex_XPosition			25
#define Tindex_YPosition			26
#define Tindex_GrayResponseUnit	27
#define Tindex_GrayResponseCurve	28

#else

/* Remember a smaller subset of TIFF fields. */

#define Tindex_SubfileType				0
#define Tindex_ImageWidth				1
#define Tindex_ImageLength				2
#define Tindex_BitsPerSample			3
#define Tindex_Compression				4
#define Tindex_PhotometricInterp		5
#define Tindex_StripOffsets			6
#define Tindex_SamplesPerPixel		7
#define Tindex_RowsPerStrip			8
#define Tindex_XResolution				9
#define Tindex_YResolution				10
#define Tindex_PlanarConfiguration	11
#define Tindex_GrayResponseUnit		12
#define Tindex_GrayResponseCurve		13
#define Tindex_Predictor				14
#define Tindex_ColorMap					15
#endif

/* Header, directory and entry structures */

typedef struct
{
	SHORT	tag;				// TIFF_TAG_TAG
	SHORT	type;				// TIFF_TAG_TYPE
	LONG	length;			/* must be 32 bits */
	LONG	value_offset;
} TIFF_FILE_ENTRY;

typedef struct
{
	BOOL entry_valid;		/* Is this entry read? */
	BOOL default_valid;		/* Does this entry have a default? */
	TIFF_DATA default_value;	/* If so, what is the default value? */
	TIFF_FILE_ENTRY entry;		/* The entry read, if one was found. */
	LONG entry_offset;		/* Offset of this entry in the file. */
} TIFF_ENTRY;

typedef TIFF_ENTRY *TIFF_ENTRY_PTR;
#define	NULL_TIFF_ENTRY	((TIFF_ENTRY_PTR)0)

typedef struct
{
	WORD entry_count;
/*	ENTRY entries[0];*/
} TIFF_IFD;

typedef struct
{
	WORD byte_order;
	WORD version;
	LONG ifd0_address;
} TIFF_HEADER;

/////////////////////////////////////////////////////////////////////////////
// CTIFFInfo

typedef struct
{
	SHORT		valid;				// BOOL
	WORD		byte_order;
	SHORT		width;
	SHORT		height;
	SHORT		byte_width;
	SHORT		bits_per_sample;
	SHORT		rows_per_strip;
	SHORT		compression;
	SHORT		photometric;
	LONG		strip_offsets;
	SHORT		last_row_read;
	LONG		last_strip_read;
	LONG		next_line_offset;
	LONG		palette_offset;
	SHORT		gray_response_length;
	SHORT		gray_response_unit;
	SHORT		samples_per_pixel;
	SHORT		planar_configuration;
	SHORT		predictor;
	ULONG		unused_offset;

} TIFF_INFO;

class CTIFFInfo
{
public:
	CTIFFInfo();
	~CTIFFInfo();

	// The info structure.
	TIFF_INFO m_Info;

	// A pointer to our entries.
	TIFF_ENTRY* m_pEntries;

	// The file we use for reading tags, etc.
	StorageDevicePtr m_pDevice;

	// For persistence purposes.
	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	ST_DEV_IO_SIZE size(StorageDevicePtr device);

	// Operations.

	ERRORCODE Open(StorageDevicePtr pDevice, BOOL fForceTagRead, \
					LPINT pWidth, LPINT pHeight, \
					LPINT pXResolution, LPINT pYResolution);

	void Close(void);

protected:
	// Helper functions.
   void near SetDefaults(void);
	void near SwapData(TIFF_DATA *, SHORT nType);
	TIFF_ENTRY_PTR near FindEntry(SHORT nTag);
	ERRORCODE near GetData(TIFF_TAG_TAG nTag, LONG lNumber, TIFF_DATA* pData);
	ERRORCODE near ReadTags(void);
	ERRORCODE near ReadInfo(LPINT pWidth, LPINT pHeight, LPINT pXRes, LPINT pYRes);
};

/*
// The TIFF file driver.
*/

class CTIFFImageDriver : public ImageDriver
{
protected:
	ReadOnlyFile file;

	CTIFFInfo* m_pInfo;

	LZWParserPtr lzw;
	SHORT current_line;

/* Old tiff context. */

	BOOL predictor;
	BOOL invert;
	SHORT byte_width;
	UCHAR last_mask;
	LPBYTE last_byte;
	SHORT row_in_strip;
	SHORT rows_per_strip;
	LONG in_strip;
	SHORT compression;

/* Where we wanna' be. */

	LONG seek_strip;
	SHORT seek_row;

/* LZW source accumulation variables. */

	SHORT row_accumulated;

	VOID near defaults_for_tiff(VOID);
	ERRORCODE near initialize_palette(PALETTE_INFO_PTR palette);
	ERRORCODE near seek_tiff_line(VOID);
	ERRORCODE near read_tiff_line(LPBYTE buffer);

/* A routine passed to the LZW code, */

	static CTIFFImageDriver* me;
	static BOOL tiff_next_byte(LPBYTE byte);

public:
	CTIFFImageDriver(CTIFFInfo* pInfo);
	~CTIFFImageDriver();

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
	virtual VOID close_image(VOID);
};

#include "pmwdoc.h"

class CDIBtoTIFFWriter
{
public:
   CDIBtoTIFFWriter (void)
   {
      ErrorCode = ERRORCODE_None;      
      pStripOffsetArray = NULL;
      StripOffsetArraySize = 0;
   }
   
   ~CDIBtoTIFFWriter (void)
   {
      KillStripOffsetArray ();
   }

   ERRORCODE
   ERRORCODESave (ERRORCODE ec)
   {
      ErrorCode = ec;
      return ec;
   }

   ERRORCODE
   GetError (void)
   {
      return ErrorCode;
   }
   
   void
   CreateTIFF (StorageDevice *pdev, CDIBInfo *pInfo, long dpi);
protected:
   long        *pStripOffsetArray, StripOffsetArraySize;
   ERRORCODE   ErrorCode;

   void
   CreateStripOffsetArray (long Size)
   {
      KillStripOffsetArray ();
      pStripOffsetArray = new long [Size];
      StripOffsetArraySize = Size;

   }

   void
   KillStripOffsetArray (void)
   {
      if (pStripOffsetArray != NULL)
      {
         delete [] pStripOffsetArray;
         pStripOffsetArray = NULL;
      }
      StripOffsetArraySize = 0;
   }
};

#endif							/* #ifndef __TIFFDRV_H__ */
