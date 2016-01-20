/*
// $Workfile: CRASTER.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CRASTER.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:38   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:18   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:14   FRED
//  
// 
//    Rev 1.0   30 Aug 1994 14:59:12   JAY
// Initial revision.
*/

#ifndef __CRASTER_H__
#define __CRASTER_H__

typedef enum
{
	CRASTER_TYPE_UNKNOWN = -1,			/* Unknown type */
	CRASTER_TYPE_BILEVEL,				/* Black and white (or 2-color) */
	CRASTER_TYPE_8,						/* 8-color primary device */
	CRASTER_TYPE_PALETTE,				/* 256-color palette device */
	CRASTER_TYPE_256,						/* 256-color non-palette device */
	CRASTER_TYPE_24						/* 24-bit color device */
} CRASTER_TYPE;

/*
// Raster information.
*/

class CRasterInfo
{
public:
	CRASTER_TYPE	m_type;
	int				m_bpp;
	int				m_planes;

	int				m_nXResolution;
	int				m_nYResolution;

	CRasterInfo();
	void Init(CDC* pDC);
};

#endif			// __CRASTER_H__
