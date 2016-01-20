/*
// $Workfile: DIBBRUSH.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DIBBRUSH.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:00   FRED
// Initial revision.
// 
//    Rev 1.1   09 May 1995 09:48:18   JAY
// Added UsePalette flag.
// 
//    Rev 1.0   03 May 1995 10:40:38   FRED
// Initial revision.
// 
//    Rev 1.0   02 May 1995 15:41:22   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "dibbrush.h"

/////////////////////////////////////////////////////////////////////////////
// CDibPatternBrush

CDibPatternBrush::DIBPATTERN CDibPatternBrush::DibPattern =
{
	{
		sizeof(BITMAPINFOHEADER),
		8L,
		8L,
		1,
		8,
		BI_RGB,
		8*8,
		0L,
		0L,
		256L,
		256L
	},
	{
		0, 0
	}
};

CDibPatternBrush::CDibPatternBrush()
{
	m_hData = NULL;
	m_pData = NULL;
	m_rgbColor = RGB(0,0,0);
	m_fUsePalette = TRUE;
}

CDibPatternBrush::~CDibPatternBrush()
{
	if (m_hData != NULL)
	{
		if (m_pData != NULL)
		{
			::GlobalUnlock(m_hData);
			m_pData = NULL;
		}
		
		::GlobalFree(m_hData);
		m_hData = NULL;
	}
}

void CDibPatternBrush::UsePalette(BOOL fUsePalette /*=TRUE*/)
{
	if (m_fUsePalette != fUsePalette)
	{
		m_fUsePalette = fUsePalette;
		// Invalidate object.
		if (m_DibBrush.GetSafeHandle() != NULL)
		{
			m_DibBrush.DeleteObject();
		}
	}
}

BOOL CDibPatternBrush::Color(COLORREF rgbColor)
{
	BOOL fResult = FALSE;
	
	// Allocate the memory if necessary.
	
	if (m_hData == NULL)
	{
		m_hData = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(DIBPATTERN));
			
		// Lock the memory if it exists.
		
		if (m_hData != NULL)
		{
			m_pData = (LPBYTE)::GlobalLock(m_hData);
				
			if (m_pData == NULL)
			{
				GlobalFree(m_hData);
				m_hData = NULL;
			}
			else
			{
				// Initialize the color array if not initialized yet.
			
				if (DibPattern.bmiColors[1] == 0)
				{
					SHORT *q;
					SHORT i;
			
					for (i = 0, q = DibPattern.bmiColors; i < 256; )
					{
						*q++ = i++;
					}
				}
		
				// Copy the base information over.
		
				memcpy(m_pData, &DibPattern, sizeof(DIBPATTERN));
		
				m_rgbColor = RGB(0,0,0);
			}
		}
	}
	
	if (m_pData != NULL)
	{
		// Prepare the dib pattern if it's not correct.

		if (rgbColor != m_rgbColor)
		{
			// Delete any old brush we had since it's now obsolete.

			m_DibBrush.DeleteObject();

			// Create the dither for the new color.

			Create8x8Dither(GetRValue(rgbColor), GetGValue(rgbColor), GetBValue(rgbColor), ((DIBPATTERN*)m_pData)->bmiData);

			m_rgbColor = rgbColor;
		}

		// Create the brush if we need to.

		if (m_DibBrush.GetSafeHandle() == NULL)
		{
			if (m_fUsePalette)
			{
				m_DibBrush.CreateDIBPatternBrush(m_hData, DIB_PAL_COLORS);
			}
			else
			{
				m_DibBrush.CreateSolidBrush(m_rgbColor);
			}
		}
		
		fResult = m_DibBrush.GetSafeHandle() != NULL;
	}
	
	return fResult;
}

CPalette* CDibPatternBrush::Palette(void)
{
	extern CPalette* pOurPal;
	return pOurPal;
}
