/*
// HALO.H
//
// HALO header definitions.
//
// $Header: /PM8/App/HALO.H 1     3/03/99 6:05p Gbeddow $
//
// $Log: /PM8/App/HALO.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:04   Fred
// Initial revision.
// 
//    Rev 1.2   11 Apr 1997 11:23:46   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.1   05 Dec 1996 12:30:32   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.0   14 Mar 1996 13:47:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:02   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:40   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:10   JAY
//  
// 
*/

#ifndef __HALO_H__
#define __HALO_H__

#include "imagedrv.h"
#include "file.h"

typedef struct
{
	WORD	xsize;
	WORD	ysize;
	WORD	word0;
} HALO_HEADER;

FARCLASS HALOHelper : public GraphicHelper
{
private:
ERRORCODE near read_halo_header(StorageDevicePtr file,
					LPWORD dots, LPWORD lines,
					LPWORD x_res, LPWORD y_res);
public:	

/*
// The pertinent data.
*/

	struct
	{
		BOOL				has_pal_file;		/* the pal file exists */
		LINE_LOCATOR	lines[8+1];			/* start of lines */
	} record;

/*
// The constructor.
*/

	HALOHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

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

	static WORD halo_x_resolution;
	static WORD halo_y_resolution;
};

typedef HALOHelper far *HALOHelperPtr;

/*
// The HALO file driver.
*/

FARCLASS HALODriver : public ImageDriver
{
private:
	ReadOnlyFile file;
	GraphicPtr graphic;

	BOOL near read_halo_header(StorageDevicePtr file,
					LPWORD dots, LPWORD lines,
					LPWORD x_res, LPWORD y_res, LPBYTE bpp);

	LINE_LOCATOR_PTR locator;
	SHORT current_line;
	SHORT bytes;

public:
	HALODriver(GraphicPtr g, StorageDevice* pSource = NULL);

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
};

typedef HALODriver far *HALODriverPtr;

#endif							// #ifndef __HALO_H__
