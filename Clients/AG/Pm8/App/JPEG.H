/*
// JPEG.H
//
// JPEG helper and drive definitions.
//
// $Header: /PM8/App/JPEG.H 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/JPEG.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
//    Rev 1.0   14 Aug 1997 15:21:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:22   Fred
// Initial revision.
// 
//    Rev 1.3   11 Apr 1997 11:23:48   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   05 Dec 1996 12:30:36   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   08 Aug 1996 18:10:56   Jay
// New JPEG compress and decompress classes
// 
//    Rev 1.0   16 May 1996 14:24:36   Jay
// JPEG
*/

#ifndef __JPEG_H__
#define __JPEG_H__

class GraphicObject;
class CJPEGDecompressor;

#include "imagedrv.h"
#include "file.h"

/*
//	For JPEG file loading, the helper definition.
*/

class JPEGHelper : public GraphicHelper
{
private:
public:	

/*
// The pertinent data.
*/

	struct
	{
		WORD m_wColorComponents;			// How many color components?
	} m_Record;

/*
// The constructor.
*/

	JPEGHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

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

	// Global default resolution.
	static POINT m_pntResolution;
};

/*
// The JPEG file driver.
*/

class JPEGDriver : public ImageDriver
{
private:
	ReadOnlyFile file;
	GraphicPtr graphic;
public:
	JPEGDriver(GraphicPtr g, StorageDevice* pSource = NULL);
	virtual ~JPEGDriver();

	CJPEGDecompressor* m_pDecompressor;

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
	virtual void close_image(VOID);
};

#endif							// #ifndef __JPEG_H__
