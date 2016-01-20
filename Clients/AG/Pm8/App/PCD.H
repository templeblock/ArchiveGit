/*
// PCD.H
//
// Photo CD definitions.
//
// $Header: /PM8/App/PCD.H 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/PCD.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:02   Fred
// Initial revision.
// 
//    Rev 1.1   05 Dec 1996 12:30:40   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.0   14 Mar 1996 13:43:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:38   FRED
// Initial revision.
// 
//    Rev 1.1   21 Sep 1995 15:12:04   JAY
// 32-bit changes
// 
//    Rev 1.0   18 Aug 1995 15:04:56   JAY
// Initial revision.
*/

#ifndef __PCD_H__
#define __PCD_H__

class GraphicObject;

#include "imagedrv.h"
#include "file.h"
#ifdef WIN32
#define KPWIN32
#include "pcdlib32.h"
#else
#include "pcdlib.h"
#endif

/*
//	The helper for a Photo CD image.
*/

class PCDHelper : public GraphicHelper
{
public:	

/*
// The pertinent data.
*/

	struct
	{
		WORD				m_wResolution;		/* Resolution flag (PCDresolution). */
	} m_Record;

/*
// The constructor.
*/

	PCDHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

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
// Open the PCD file corresponding to this image.
*/

	 PCDstatus OpenPCD(PCDphotoHdl* lphPCD);

/*
// Default resolutions for draft and proof.
*/

	static WORD m_wDraftResolution;
	static WORD m_wProofResolution;
};

/*
// The PCD file driver.
*/

class PCDDriver : public ImageDriver
{
private:
	PCDphotoHdl m_hPCD;
	BOOL m_fOpen;

//	ReadOnlyFile file;
	GraphicPtr graphic;

	HGLOBAL m_hLineCache;
	LPBYTE m_pLineCache;
	int m_nCacheTop;
	int m_nCacheMax;
	int m_nCacheLines;

public:
	PCDDriver(GraphicPtr g);
	virtual ~PCDDriver();

	virtual ERRORCODE image_info(IMAGE_INFO_PTR info);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
	virtual ERRORCODE read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask);
	virtual VOID close_image(VOID);

	static WORD m_wMaxCacheSize;

};

#endif							// #ifndef __PCD_H__
