/*
// $Workfile: DIBBRUSH.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BORDER/DIBBRUSH.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     4/22/98 11:44a Fredf
*/ 

class CDibPatternBrush;

#ifndef __DIBBRUSH_H__
#define __DIBBRUSH_H__

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
protected:
	HGLOBAL m_hData;
	LPBYTE m_pData;
	COLORREF m_rgbColor;
	CBrush m_DibBrush;
	BOOL m_fUsePalette;
};

#endif
