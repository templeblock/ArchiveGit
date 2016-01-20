/*
// $Header: /PM8/App/IMAGEDRV.H 1     3/03/99 6:06p Gbeddow $
//
// Image driver definitions.
//
// $Log: /PM8/App/IMAGEDRV.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:14   Fred
// Initial revision.
// 
//    Rev 1.3   11 Apr 1997 11:23:46   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   05 Dec 1996 12:30:34   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   01 Nov 1996 17:11:52   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:45:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:10   FRED
// Initial revision.
// 
//    Rev 2.2   18 Aug 1995 15:06:00   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.1   09 Mar 1995 08:27:30   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:42   JAY
// Initial revision.
// 
//    Rev 1.1   04 Aug 1994 07:33:26   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   16 Dec 1993 16:26:14   JAY
//  
*/

#ifndef __IMAGEDRV_H__
#define __IMAGEDRV_H__

#include "file.h"
#include "imagedef.h"

class GraphicRecord;

struct PALETTE_INFO
{
	PALETTE_INFO();
	void BuildDefaultPalette(IMAGE_INFO& Info);

	RGBTRIPLE far *m_pData;
	SHORT m_nColors;
	BOOL m_fRead;
};

typedef PALETTE_INFO far *PALETTE_INFO_PTR;

/*
// A routine for converting one type of source format to 8 bpp.
*/

typedef VOID (near *SOURCE_CONVERTOR)(LPBYTE source, LPBYTE dest, int pixels);

/*
// An image driver.
*/

class ImageDriver
{
private:
	virtual ERRORCODE image_info(IMAGE_INFO_PTR info) = 0;
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette) = 0;
	virtual ERRORCODE read_lines(int line, LPBYTE pImage, int count, LPBYTE pMask) = 0;
	virtual VOID close_image(VOID) {};
	
	void FreeRows(void);

	LPBYTE 				m_pImageRow;
	SOURCE_CONVERTOR	convert;

	LPBYTE 				m_pMaskRow;

protected:

	int					source_row_bytes;			/* Not counting planes... */
	int					source_row_size;

	int					m_nMaskRowSize;

	IMAGE_INFO			our_info;

public:
	ImageDriver();
	virtual ~ImageDriver();
	ERRORCODE info(IMAGE_INFO_PTR info);
	ERRORCODE open(PALETTE_INFO_PTR palette);
	ERRORCODE read(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask = NULL);
	VOID close(VOID);
};

typedef ImageDriver far *ImageDriverPtr;

/*
// A special image driver that reads images stored for graphic records.
*/

class GraphicImageDriver : public ImageDriver
{
public:

	GraphicImageDriver(GraphicRecord* pGraphic);
	virtual ~GraphicImageDriver();
	BOOL Init(void);

protected:

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask);

	ERRORCODE ReadHeader(void);
	ERRORCODE ReadStrip(UINT uStrip);

	WORD m_wRowBytes;

	HGLOBAL m_hStrip;
	LPBYTE m_pStrip;
	int m_nStripTop;
	int m_nStripLines;

	IMAGE_INFO m_Info;
	ReadOnlyFile m_File;
	GraphicRecord* m_pGraphic;
	CGraphicImageHeader* m_pHeader;
};

#endif					// #ifndef __IMAGEDRV_H__
