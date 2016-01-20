/*
// Definitions for the TIFF handling code.
//
// $Header: /PM8/App/TIFF.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TIFF.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:16   Fred
// Initial revision.
// 
//    Rev 1.3   28 Jul 1997 14:59:40   Jay
// Support for reiniting collection graphics and other LZW TIFF fixes.
// 
//    Rev 1.2   02 Jul 1997 14:25:58   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.1   11 Apr 1997 11:23:50   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.0   14 Mar 1996 13:46:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:16   FRED
// Initial revision.
// 
//    Rev 2.1   09 Mar 1995 08:27:42   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:49:04   JAY
// Initial revision.
// 
//    Rev 1.3   04 Aug 1994 07:33:24   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   07 Jul 1994 11:28:18   JAY
// Got rid of seek and tell methods in the helper.
// 
//    Rev 1.1   25 Jan 1994 15:18:36   JAY
// More
// 
//    Rev 1.0   16 Dec 1993 16:25:54   JAY
//  
*/

#ifndef __TIFF_H__
#define __TIFF_H__

#include "tiffdrv.h"

/////////////////////////////////////////////////////////////////////////////
// TIFFHelper

class TIFFHelper : public GraphicHelper
{
private:
/*
// The pertinent data.
*/

	CTIFFInfo m_Info;

public:	

/*
// The constructor.
*/

	TIFFHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

/*
// Return our info.
*/

	CTIFFInfo& Info(void);

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

	static WORD tiff_x_resolution;
	static WORD tiff_y_resolution;

};

class TIFFDriver : public CTIFFImageDriver
{
public:
	TIFFDriver(GraphicPtr g, StorageDevice* pSource = NULL);
	virtual ERRORCODE open_image(PALETTE_INFO_PTR palette);
protected:
	BOOL m_fFailedPrep;
};

#endif							/* #ifndef __TIFF_H__ */

