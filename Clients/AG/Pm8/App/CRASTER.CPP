/*
// $Workfile: CRASTER.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CRASTER.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:36:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:46   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:58   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:53:38   FRED
//  
// 
//    Rev 1.0   30 Aug 1994 14:59:14   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "craster.h"

CRasterInfo::CRasterInfo()
 : m_type(CRASTER_TYPE_BILEVEL),
	m_bpp(1),
	m_planes(1),
	m_nXResolution(1),
	m_nYResolution(1)
{
}

#if 0
CRasterInfo::~CRasterInfo()
{
}
#endif

void CRasterInfo::Init(CDC* pDC)
{
	WORD colors = pDC->GetDeviceCaps(NUMCOLORS);
	WORD raster_caps = pDC->GetDeviceCaps(RASTERCAPS);

	m_bpp = pDC->GetDeviceCaps(BITSPIXEL);
	m_planes = pDC->GetDeviceCaps(PLANES);

	m_nXResolution = pDC->GetDeviceCaps(LOGPIXELSX);
	m_nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);

	m_type = CRASTER_TYPE_UNKNOWN;

	WORD color_bits = m_planes * m_bpp;

	if (color_bits == 24)
	{
		m_type = CRASTER_TYPE_24;
		m_bpp = 24;
		m_planes = 1;
	}
	else if (m_planes == 1)
	{
		if (m_bpp == 1 && colors == 2)
		{
			m_type = CRASTER_TYPE_BILEVEL;
		}
		else if (m_bpp == 8 && (raster_caps & RC_PALETTE))
		{
			m_type = CRASTER_TYPE_PALETTE;		/* 256 color palette device */
		}
	}
	else if (m_bpp == 1 && (m_planes == 3 || m_planes == 4))
	{
		m_type = CRASTER_TYPE_8;				/* 8-color primary device */
	}

	if (m_type == CRASTER_TYPE_UNKNOWN)
	{
	/* Pick something. */
		if (color_bits > 8 || colors > 256)
		{
			m_type = CRASTER_TYPE_24;
			m_bpp = 24;
		 	m_planes = 1;
		}
		else
		{
			m_type = CRASTER_TYPE_256;			/* 256-color, but no palette! */
			m_bpp = 8;
		 	m_planes = 1;
		}
	}
}
