/********************************************************************************
	Copyright 2005 Sjaak Priester	

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

	http://www.gnu.org/licenses/
********************************************************************************/

// QGraphicsText
//
// C++ class to create a Windows GDI+ path containing string of text.
// Basically does the same as GraphicsPath::AddString().
//
// However - and this is the motivation - GDI+ GraphicsPath::AddString()
// only accepts TrueType fonts and OpenType fonts of the TrueType variety.
// QGraphicsPath also accepts PostScript Type 1 and OpenType/Type 1 fonts (under Windows 2000/XP).
//
// The resulting text path is assembled in a GDI+ GraphicsPath by calling
// the member function GetGraphicsText().
//
// See the QGraphicsTextDemo application for a demonstration.
//
// Source:
//		QGraphicsText.cpp
//
// Environment: Windows 98 and later, GDI+, Visual C++ 7.0/7.1, MFC.
// Probably compatible with Visual C++ 6.0 as well.
//
//==============
// Version 1.1, September 29, 2005.
// Major rewrite. Added caching, which resulted in a _huge_ speed up for longer strings.
//
// Version 1.0, September 21, 2005.
// (c) 2005, Sjaak Priester, Amsterdam.
// http://www.sjaakpriester.nl
//
// Use at your ownn risk. Comments welcome.

#pragma once

#include <map>
using namespace std;

class QGraphicsText
{
public:
	QGraphicsText();
	~QGraphicsText();

	void SetFont(const LOGFONT& logfont, bool bFlipY = true);
	// Set the font form a (GDI) LOGFONT structure.
	//
	// Parameter bFlipY determines how the vertical axis is interpreted.
	// If bFlipY is false, QPathText assumes that positive y-values are above the zero point.
	// If bFlipY is true, QPathText assumes that positive y-values are below the zero point.
	//
	// Note that in the default Windows mapping mode MM_TEXT, the y-axis is oriented downward,
	// so for normal operation, bFlipY should be true (default).

	int GetGraphicsText(GraphicsPath& path, LPCTSTR str, PointF pntOrigin) const;
	int GetGraphicsText(GraphicsPath& path, LPCTSTR str, Point pntOrigin) const
		{ return GetGraphicsText(path, str, PointF((REAL) pntOrigin.X, (REAL) pntOrigin.Y)); }

	// Add the string str to the GraphicsPath path.
	// The lower left of the first character will be placed at pntOrigin.
	//
	// Return: the number of processed characters.
	//
	// Basically, this function does the same as GraphicsPath::AddString(), but also with Type 1 fonts.
	// Note that GetGraphicsText will not clear the path, before it adds the string.

protected:
	void AddGlyph(TCHAR c, TCHAR cNext, PointF& pntLocation, GraphicsPath * pPath) const;
	// Add the glyph (character) c of the current font to pPath. The origin of the character cell
	// (the left point on the base line) will be placed at pntLocation.
	//
	// After return, pntLocation will be updated to the origin of the next character cell.
	//
	// cNext is the character following c; this information is used to calculate the kerning.
	//
	// pPath may be NULL. In that case, only pntLocation will be updated.

	HFONT m_hFont;
	bool m_bFlipY;

private:
	struct Glyph
	{
		Glyph(HDC hDC, TCHAR c, bool bFlipY);
		Glyph(const Glyph& g) : m_CellInc(g.m_CellInc), m_pPath(g.m_pPath->Clone())	{}
		~Glyph()	{ delete m_pPath; }

		GraphicsPath * m_pPath;
		PointF m_CellInc;
	};

	typedef map<TCHAR, Glyph> glyph_map;
	glyph_map m_Cache;

	typedef pair<TCHAR, TCHAR> kerning_key;
	typedef map<kerning_key, REAL> kerning_map;
	kerning_map m_KerningData;
};


//==========================================
// Utility functions
//
// Converions FIXED <=> REAL
// Found these partly in MSDN KB 87115.

inline FIXED FixedFromReal(REAL r)
{
    long l;

    l = (long) (r * 65536L);
    return *(FIXED *)&l;
}

inline REAL RealFromFixed(FIXED f)
{
	return f.value + (REAL) f.fract / 65536.0f;
}

inline void NegateFixed(FIXED& f)
{
	long * p((long *) &f);
	* p = - *p;
}
