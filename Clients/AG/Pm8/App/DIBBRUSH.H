/*
// $Workfile: DIBBRUSH.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DIBBRUSH.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:52   FRED
// Initial revision.
// 
//    Rev 1.1   09 May 1995 09:48:18   JAY
// Added UsePalette flag.
// 
//    Rev 1.0   03 May 1995 10:40:20   FRED
// Initial revision.
// 
//    Rev 1.0   02 May 1995 15:41:22   FRED
// Initial revision.
*/ 

class CDibPatternBrush;

#ifndef __DIBBRUSH_H__
#define __DIBBRUSH_H__

#include "dither.h"

class CDibPatternBrush
{
protected:
	struct DIBPATTERN
	{
		BITMAPINFOHEADER	bmiHeader;
		SHORT					bmiColors[256];			/* 256 palette entries */
		BYTE					bmiData[8*8];				/* bitmap data */
	};
	static DIBPATTERN DibPattern;
public:
	CDibPatternBrush();
	~CDibPatternBrush();
public:
	COLORREF Color(void)
		{	return m_rgbColor; }
	void UsePalette(BOOL fUsePalette = TRUE);
	BOOL Color(COLORREF rgbColor);
	CBrush* Brush(void)
		{ return &m_DibBrush; }
	CPalette* Palette(void);
protected:
	HGLOBAL m_hData;
	LPBYTE m_pData;
	COLORREF m_rgbColor;
	CBrush m_DibBrush;
	BOOL m_fUsePalette;
};

#endif
