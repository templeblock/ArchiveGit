/*
// GIF.H
//
// GIF header definitions.
//
// $Header: /PM8/App/GIF.H 1     3/03/99 6:05p Gbeddow $
//
// $Log: /PM8/App/GIF.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 5     1/25/99 3:44p Rgrenfell
// Added support for Netscape loop tag requesting.
// 
// 4     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 3     3/18/98 6:39p Hforman
// include "grafrec.h"
// 
// 2     1/13/98 11:15a Jayn
// Transparent GIF support.
// 
//    Rev 1.0   14 Aug 1997 15:21:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:00   Fred
// Initial revision.
// 
//    Rev 1.5   05 May 1997 08:53:44   Jay
// Added DLL. Better error handling.
// 
//    Rev 1.4   15 Apr 1997 15:44:46   Jay
// Card Server phase #1
// 
//    Rev 1.3   11 Apr 1997 11:23:44   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   05 Dec 1996 12:30:32   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   15 May 1996 14:26:54   Jay
// Now supports interlaced GIF files.
// 
//    Rev 1.0   14 Mar 1996 13:46:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:58   FRED
// Initial revision.
// 
//    Rev 2.1   19 Jan 1996 09:21:52   JAY
// 
//    Rev 2.0   07 Feb 1995 15:48:38   JAY
// Initial revision.
// 
//    Rev 1.1   23 May 1994 08:17:02   JAY
// Added a default NULL argument to GIFFile.
// 
//    Rev 1.0   16 Dec 1993 16:25:56   JAY
//  
// 
*/

#ifndef __GIF_H__
#define __GIF_H__

// The transparent color in the PrintMaster palette.
#define TRANSPARENT_INDEX	10

/*
// Interlaced GIFs.
// (Someday we'll use this)
// int RowMults = { 8, 8, 4, 2}
// int LineStarts = { 0, 4, 2, 1}
*/

#include "imagedrv.h"
#include "file.h"
#include "grafrec.h"

class LZWParser;
class ImageBuffer;

#pragma pack(1)

struct GIF_HEADER
{
	CHAR	header_id[6];					/* Usually says GIF87a. */
	SHORT	screen_width;					/* Raster width in pixels */
	SHORT	screen_height;					/* Raster height in pixels */
	UCHAR	flags;							/* M bit, CR bits, PIXEL bits */
	UCHAR	background_color;				/* background color index */
	UCHAR	zero;								/* 0 byte */
};

struct IMAGE_HEADER
{
	SHORT	left;
	SHORT	top;
	SHORT	width;
	SHORT	height;
	UCHAR flags;							/* M(0x80) bit, I(0x40) bit, pixel count if M. */
};

struct GIF_CONTROL_EXTENSION
{
	BYTE bFlags;
	WORD wDelayTime;
	BYTE bTransparentIndex;
};

struct APPLICATION_EXTENSION_BLOCK
{
	CHAR bAsciiCode[8];
	BYTE bBinaryCode[3];
};

struct NETSCAPE_LOOP_BLOCK
{
	BYTE bBlockCode;
	WORD wLoopCount;
};

#pragma pack()

class GIFFile : public ReadOnlyFile
{
private:
	BOOL reading_image;
	SHORT gif_count;

public:
/* Constructor. */

	GIFFile(LPSTR name = NULL);

/* Overloaded read method. */

	virtual ERRORCODE read(VOIDPTR ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL);

/* Overloaded seek method. */

	virtual ERRORCODE seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode);

/* Set that we are reading an image. */

	VOID set_reading_image(BOOL ri)
		{ reading_image = ri; }
};

class GIFHelper : public GraphicHelper
{
private:
	struct GifHeaderInfo
	{
		int nWidth;
		int nHeight;
		int nXRes;
		int nYRes;
		BYTE bBPP;
		BOOL fHasColorMap;
		BOOL fInterlaced;
		int nTransparentIndex;	// Transparent Index for the current image read.
		WORD wDelayTime;			// Delay for the current image read.
		WORD wLoopCount;
	};

	ERRORCODE ReadGifInfo(StorageDevicePtr file, GifHeaderInfo& Info);
public:	

/*
// The pertinent data.
*/

	struct
	{
		UCHAR		m_bBPP;					// number of bits per pixel
		WORD		m_fHasColorMap;		// has a global color map
		WORD		m_fInterlaced;			// Are we interlaced?
		// New in 5.0.
		// We have both a boolean variable to control transparency as well
		// as a transparent color. This is redundant but it solves a problem:
		// record members will be zero if loading an old format. We want such
		// a case to indicate no transparency, and the boolean solves this.
		BOOL		m_fHasTransparency;	// Do we have transparency?
		SHORT		m_nTransparentIndex;	// What is the transparent index?
	} record;

/*
// The constructor.
*/

	GIFHelper(GraphicPtr graphic);

/*
// GIF block traversing functions.
*/

	BYTE next_gif_block(StorageDevice* pFile);
	BOOL find_image_block(StorageDevice* pFile, GifHeaderInfo* pInfo = NULL);

/*
// Overridden methods (for storage purposes).
*/

	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	ST_DEV_IO_SIZE size(StorageDevicePtr device);

/*
// The init function.
*/

	ERRORCODE init(GRAPHIC_CREATE_STRUCT_PTR gcs);

/*
// Global default resolutions.
*/

	static POINT m_pntResolution;
};

typedef GIFHelper far *GIFHelperPtr;

/*
// The GIF file driver.
*/

typedef class GIFDriver far *GIFDriverPtr;

class GIFDriver : public ImageDriver
{
private:
	GIFFile file;
	GraphicPtr graphic;
	LZWParser* lzw;

	ST_DEV_POSITION image_start;

	SHORT current_line;
	BYTE bits;
	SHORT bytes;

	BOOL m_fDeInterlacing;
	ImageBuffer* m_pBuffer;
	int m_nPassStart[4];

	int m_nTransparentIndex;		// -1 if none

	int PassSize(int nRows, int nPass);
	int InterlacedLineOffset(int nLine, int nPass);
	int InterlacedLine(int nLine);

public:
	GIFDriver(GraphicPtr g, StorageDevice* pSource = NULL);
	~GIFDriver(VOID);

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
	virtual VOID close_image(VOID);
};

// The following two abstract classes are assumed to throw exceptions
// when necessary.

// Data source (input).
class CGIFDataSource
{
public:
	CGIFDataSource() {}
	virtual ~CGIFDataSource() {}

	virtual int ReadByte(void) = 0;
};

// Data sink (output).
class CGIFDataSink
{
public:
	CGIFDataSink() {}
	virtual ~CGIFDataSink() {}

	virtual void WriteData(LPBYTE pData, int nCount) = 0;
};

/*
// The GIF writer.
*/

class CGIFWriter
{
public:
	CGIFWriter();
	virtual ~CGIFWriter();

	// Create a GIF file.
	void CreateGIF(CGIFDataSource* pSource,/* ptr to caller's pixel reader */
						CGIFDataSink* pSink,		/* ptr to caller's data writer */
						short nWidth,				/* width of image in pixels */
						short nHeight,				/* height of image in pixels */
						short nColorResolution,	/* bits of color resolution (1..8) */
						BOOL fInterlaced,			/* interlaced flag */
						short nBitsPerPixel,		/* bits per pixel (1..8) */
						BOOL fNormalize,			/* normalize the color map */
						RGBQUAD* pColorMap,		/* ptr to color map */
						int nTransparentIndex = -1);
protected:

	// Write signature and screen description.
	void WriteHeader(short nWidth,
						  short nHeight,
						  short nColorResolution,
						  short nFillColor,
						  short nBitsPerPixel,
						  BOOL fNormalize,
						  RGBQUAD* pColorMap,
						  int nTransparentIndex = -1);

	// Write a graphic control extension block.
	void WriteGraphicControlExtension(int nTransparentIndex);

	// Write the color map.
	void WriteColorMap(short nColorResolution,
							 short nBitsPerPixel,
							 BOOL fNormalize,
							 RGBQUAD* pColorMap,
							 int nTransparentIndex = -1);

	// Write the image description.
	void WriteImageDescription(short nLeftEdge,
										short nTopEdge,
										short nWidth,
										short nHeight,
										short nColorResolution,
										BOOL fInterlaced,
										int nBitsPerPixel,
										BOOL fNormalize,
										RGBQUAD* pColorMap,
										int nTransparentIndex = -1);

	// Write a separator (',').
	void WriteSeparator(void);

	// Write a terminator (';').
	void WriteTerminator(void);

	// Write an extension block marker (';').
	void WriteExtensionBlock(void);

	// Easy write routine.
	void WriteData(LPBYTE pData, int nCount)
		{ m_pSink->WriteData(pData, nCount); }

	// The guy to write data to.
	CGIFDataSink* m_pSink;
};

/*
// The GIF (LZW) compressor.
*/

class CGIFCompressor
{
public:
	CGIFCompressor();
	~CGIFCompressor();

	void CompressData(short nMinCodeSize, CGIFDataSource* pSource, CGIFDataSink* pSink);

	// Cleanup for exceptions, etc.
	void FreeAllData(void);

protected:
	// Compression stuff
	struct CCodeEntry
	{
		short m_nPriorCode;
		short m_nCodeId;
		BYTE m_bAddedChar;
	};

	short m_nCodeSize;
	short m_nClearCode;
	short m_nEofCode;
	short m_nMinCode;
	short m_nBitOffset;
	short m_nByteOffset;
	short m_nBitsLeft;
	short m_nMaxCode;
	short m_nFreeCode;
	short m_nPrefixCode;
	short m_nSuffixChar;

	BYTE m_CodeBuffer[256+3];
	CCodeEntry* m_pCodeTable;

	void InitTable(short nMinCodeSize);
	void FreeTable(void);
	void Flush(short n);
	void WriteCode(short nCode);

	void WriteData(LPBYTE pData, int nCount)
		{ m_pSink->WriteData(pData, nCount); }

	// The guy to write data to.
	CGIFDataSink* m_pSink;
};

/*
// Another abstract GIF source.
// This gets lines and feeds them to the caller. It handles interlacing.
*/

class CGIFDataSourceLines : public CGIFDataSource
{
public:
	CGIFDataSourceLines(int nWidth, int nHeight, BOOL fInterlaced = FALSE);
	virtual ~CGIFDataSourceLines();
	virtual int ReadByte(void);
protected:
	virtual void ReadLine(int nLine, LPBYTE pBuffer) = 0;
	// Variables.
	BOOL m_fInterlaced;
	int m_nWidth;
	int m_nHeight;
	int m_nLine;
	int m_nByteInLine;
	LPBYTE m_pLine;
};

/*
// A standard GIF source.
// This takes the bits from a 256-color DIB, with all its usual weirdness like
// padded rows and flipped lines.
*/

class CGIFDataSourceDIB : public CGIFDataSourceLines
{
public:
	CGIFDataSourceDIB(int nWidth, int nHeight, LPBYTE pData, BOOL fInterlaced = FALSE);
	void FreeAll(void);
protected:
	virtual void ReadLine(int nLine, LPBYTE pBuffer);
	int m_nRowBytes;
	LPBYTE m_pData;
};

/*
// A standard GIF sink based on a StorageDevice.
*/

class CGIFDataSinkDevice : public CGIFDataSink
{
public:
	CGIFDataSinkDevice(StorageDevice* pDevice);
	virtual void WriteData(LPBYTE pData, int nCount);
protected:
	StorageDevice* m_pDevice;
};

/*
// GIF writer that takes a DIB as source.
*/

class CDIBtoGIFWriter : public CGIFWriter
{
public:
	CDIBtoGIFWriter();

	void CreateGIF(BITMAPINFO* pbi, LPBYTE pData, CGIFDataSink* pSink, BOOL fInterlaced = FALSE, int nTransparentIndex = -1);
};

#endif							// #ifndef __GIF_H__
