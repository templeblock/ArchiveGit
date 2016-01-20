/*
// BMP.H
//
// BMP header definitions.
//
// $Header: /PM8/App/BMP.H 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/BMP.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     8/20/98 11:02a Jayn
// Resolution changes.
// 
//    Rev 1.0   14 Aug 1997 15:18:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:44   Fred
// Initial revision.
// 
//    Rev 1.3   11 Apr 1997 11:23:44   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   05 Dec 1996 12:30:30   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   20 Sep 1996 13:41:18   Jay
// Support for Temp DIB graphic records.
// 
//    Rev 1.0   14 Mar 1996 13:48:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:40   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:20   JAY
// Initial revision.
// 
//    Rev 1.1   09 Jan 1995 16:26:42   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.0   16 Dec 1993 16:25:58   JAY
//  
// 
*/

#ifndef __BMP_H__
#define __BMP_H__

class GraphicObject;

#include "imagedrv.h"
#include "file.h"

/*
//	For BMP file loading, the header definition.
*/

FARCLASS BMPHelper : public GraphicHelper
{
private:
	ERRORCODE near read_bmp_header(StorageDevicePtr file,
					LPWORD dots, LPWORD lines,	LPBYTE bpp,
					LPBYTE compression, LPDWORD header_size,
					LPDWORD bitmap_offset);
public:	

/*
// The pertinent data.
*/

	struct
	{
		DWORD				header_size;		/* size of BMP header */
		UCHAR				bits_per_pixel;	/* number of bits per pixel */
		UCHAR				compression;		/* type of compression */
		DWORD				bitmap_offset;		/* offset to the bitmap */
	} record;

/*
// The constructor.
*/

	BMPHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

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

typedef BMPHelper far *BMPHelperPtr;

/*
// The BMP file driver.
*/

class BMPDriver : public ImageDriver
{
private:
	ReadOnlyFile file;
	GraphicPtr graphic;

	SHORT current_line;
	SHORT source_byte_width;
	SHORT buffer_byte_width;
	SHORT source_compression;
public:
	BMPDriver(GraphicPtr g, StorageDevice* pSource = NULL);

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
};

typedef BMPDriver far *BMPDriverPtr;

/////////////////////////////////////////////////////////////////////////////
// An image driver based on a DIB.

class DIBImageDriver : public ImageDriver
{
private:
	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);

	LPBITMAPINFO m_pDib;
#ifdef WIN32
	LPBYTE m_pCurrentByte;
#else
	BYTE __huge* m_pCurrentByte;
#endif

	SHORT current_line;
	SHORT source_byte_width;
	SHORT buffer_byte_width;

public:
	DIBImageDriver(LPBITMAPINFO m_pDib);
	virtual ~DIBImageDriver();
};

/*
// Create a BMP graphic from clipboard data.
*/

extern ERRORCODE CreateBMPFromDIB(LPCSTR pName,
											 PMGDatabase* pDatabase,
											 BITMAPINFO* pBmi,
											 GraphicObject*& pObject);

extern ERRORCODE CreateBMPFromBitmap(LPCSTR pName,
												 PMGDatabase* pDatabase,
												 HBITMAP hBitmap,
												 GraphicObject*& pObject,
												 HPALETTE hPalette = NULL);

extern ERRORCODE BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal, HGLOBAL* phDIB);

extern LPSTR FindDIBBits(LPVOID lpbi);
extern WORD DIBNumColors(LPVOID lpbi);
extern WORD DIBPaletteSize(LPVOID lpbi);

extern void InitBitmapInfoHeader(LPBITMAPINFOHEADER lpBmInfoHdr,
											DWORD dwWidth,
											DWORD dwHeight,
											int nBPP);

#endif							// #ifndef __BMP_H__
