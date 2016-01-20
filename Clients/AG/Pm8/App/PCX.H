/*
// PCX.H
//
// PCX header definitions.
//
// $Header: /PM8/App/PCX.H 1     3/03/99 6:09p Gbeddow $
//
// $Log: /PM8/App/PCX.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 4     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 3     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 2     11/21/97 5:21p Johno
// General update
// 
//    Rev 1.0   14 Aug 1997 15:23:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:04   Fred
// Initial revision.
// 
//    Rev 1.2   11 Apr 1997 11:23:48   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.1   05 Dec 1996 12:30:40   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.0   14 Mar 1996 13:45:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:12   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:52   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:25:54   JAY
//  
// 
*/

#ifndef __PCX_H__
#define __PCX_H__

#include "imagedrv.h"
#include "file.h"

/*
//	For PCX file loading, the header definition.
*/

typedef struct
{
	BYTE	d_Manuf;
	BYTE	d_Hard;			/* version info */
	BYTE	d_Encod;		/* encoding method */
	BYTE	d_Bitpx;		/* bits per pixels */
	WORD	d_X1;
	WORD	d_Y1;
	WORD	d_X2;
	WORD	d_Y2;
	WORD	d_Hres;
	WORD	d_Vres;
	BYTE	d_clrmap[48];
	BYTE	d_Vmode;
	BYTE	d_Nplanes;
	WORD	d_Bplin;
} PCX_HEADER;

FARCLASS PCXHelper : public GraphicHelper
{
private:
public:	

/*
// The pertinent data.
*/

	struct
	{
		SHORT				row_bytes;			/* bytes in a row */
		UCHAR				nplanes;				/* number of planes */
		UCHAR				bits_per_pixel;	/* number of bits per pixel */
		LINE_LOCATOR	lines[8+1];			/* start of lines */
	} record;

/*
// The constructor.
*/

	PCXHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

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

typedef PCXHelper far *PCXHelperPtr;

/*
// The PCX file driver.
*/

FARCLASS PCXDriver : public ImageDriver
{
private:
	ReadOnlyFile file;
	GraphicPtr graphic;

	ERRORCODE rewind(VOID);
	ERRORCODE near skip_pcx_lines(SHORT row_bytes, SHORT nplanes, SHORT lines);
	ERRORCODE near read_pcx_line(SHORT bytes, SHORT row_bytes, BYTE nplanes, LPBYTE current_byte, BOOL full_color);

	SHORT current_line;
	SHORT pcx_row_bytes;
	LINE_LOCATOR_PTR locator;					/* Current line locator. */

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);

public:
	PCXDriver(GraphicPtr g, StorageDevice* pSource = NULL);
};

typedef PCXDriver far *PCXDriverPtr;

// CDIBtoPCXWriter

#include "pmwdoc.h"

class CDIBtoPCXWriter
{
public:
   CDIBtoPCXWriter (void)
   {
      ErrorCode = ERRORCODE_None;
   }

   ERRORCODE
   GetError (void)
   {
      return ErrorCode;
   }

   void
   CreatePCX (StorageDevice *pdev, CDIBInfo *pInfo, long dpi);
protected:
   ERRORCODE   ErrorCode;

};

#endif							// #ifndef __PCX_H__
