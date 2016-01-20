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
//
//==============
// Version 1.1, September 29, 2005.
//
// Version 1.0, September 21, 2005.
// (c) 2005, Sjaak Priester, Amsterdam.
// http://www.sjaakpriester.nl

#include "StdAfx.h"
#include "QGraphicsText.h"

//======================================
// QGraphicsText
//

QGraphicsText::QGraphicsText()
: m_hFont(0)
, m_bFlipY(true)
{
}

QGraphicsText::~QGraphicsText()
{
	if (m_hFont) ::DeleteObject(m_hFont);
}

void QGraphicsText::SetFont(const LOGFONT& logfont, bool bFlipY)
{
	m_bFlipY = bFlipY;

	m_KerningData.clear();

	if (m_hFont) ::DeleteObject(m_hFont);

	m_hFont = ::CreateFontIndirect(& logfont);

	if (m_hFont)
	{
		HDC hDC = ::GetDC(NULL);
		if (hDC)
		{
			HGDIOBJ hOldFont = ::SelectObject(hDC, m_hFont);

			// QGraphicsText supports character kerning.
			// Note that many TrueType fonts don't have kerning information.
			// Off the standard Windows fonts, 'Garamond' is one of the few, so use
			// this for testing.

			int nKPairs = ::GetKerningPairs(hDC, 0, NULL);
			if (nKPairs > 0)
			{
				KERNINGPAIR * pKerningPairs = new KERNINGPAIR[nKPairs];
				if (pKerningPairs)
				{
					::GetKerningPairs(hDC, nKPairs, pKerningPairs);

					KERNINGPAIR * pPairs(pKerningPairs);

					for (int i = 0; i < nKPairs; ++i)
					{
						kerning_key k((TCHAR) pPairs->wFirst, (TCHAR) pPairs->wSecond);
						m_KerningData.insert(kerning_map::value_type(k, (REAL) pPairs->iKernAmount));
						++pPairs;
					}
					delete[] pKerningPairs;
				}
			}

			::SelectObject(hDC, hOldFont);
			::ReleaseDC(NULL, hDC);
		}
	}

//	TRACE1(_T("m_KerningData.size() = %d\n"), m_KerningData.size());
}

int QGraphicsText::GetGraphicsText(GraphicsPath& path, LPCTSTR str, PointF pntOrigin) const
{
	path.SetFillMode(FillModeWinding);

	HDC hDC = ::GetDC(NULL);
	if (hDC)
	{
		// Internally, QGraphicsText considers the left point on the base line of the
		// character as the origin. Windows GDI and GDI+ however see the upper
		// left point of the character cell as the origin. Compensate for compatibility.
		HGDIOBJ hOldFont = ::SelectObject(hDC, m_hFont);

		TEXTMETRIC tm;
		::GetTextMetrics(hDC, & tm);

		if (m_bFlipY) pntOrigin.Y += (REAL) tm.tmAscent;
		else pntOrigin.Y -= (REAL) tm.tmAscent;

		::SelectObject(hDC, hOldFont);
		::ReleaseDC(NULL, hDC);
	}

	const TCHAR * p = str;

	Status stat(Ok);
	while (* p && stat == Ok)
	{
		const TCHAR * pNext = ::CharNext(p);
		AddGlyph(* p, * pNext, pntOrigin, & path);

		p = pNext;
	}
	if (stat != Ok) --p;
	return (int)(p - str);
}

void QGraphicsText::AddGlyph(TCHAR c, TCHAR cNext, PointF& pntLocation, GraphicsPath * pPath) const
{
	if (m_Cache.find(c) == m_Cache.end())
	{
//		TRACE2(_T("Caching \'%c\' (0x%02x)\n"), c, c);

		HDC hDC = ::GetDC(NULL);
		if (hDC)
		{
			HGDIOBJ hOldFont = ::SelectObject(hDC, m_hFont);

			// This ugly cast is needed because this is a const function.
			// Of course, the function could have been non-const, but I sort of feel
			// that it has const semantics. But maybe, this example doesn't deserve
			// to be followed...
			(const_cast<QGraphicsText*>(this))->m_Cache.insert(glyph_map::value_type(c, Glyph(hDC, c, m_bFlipY)));

			::SelectObject(hDC, hOldFont);
			::ReleaseDC(NULL, hDC);
		}
	}

	glyph_map::const_iterator it = m_Cache.find(c);

	if (pPath)
	{
		GraphicsPath * pGlyph = it->second.m_pPath->Clone();

		// Translate to pntLocation.
		Matrix mx;
		mx.Translate(pntLocation.X, pntLocation.Y);
		pGlyph->Transform(& mx);

		pPath->AddPath(pGlyph, FALSE);

		delete pGlyph;
	}

	// Update pntLocation.
	pntLocation = pntLocation + it->second.m_CellInc;

	// If available, add kerning amount (probably, but not necessarily, a negative number).
	kerning_key k(c, cNext);
	kerning_map::const_iterator kit = m_KerningData.find(k);
	if (kit != m_KerningData.end())
	{
		REAL amount = kit->second;

//		TRACE3(_T("Kerning amount \'%c\' - \'%c\': %f\n"), c, cNext, amount);
		pntLocation.X += amount;
	}
}

QGraphicsText::Glyph::Glyph(HDC hDC, TCHAR c, bool bFlipY)
{
	static MAT2 matUnity =
	{
		FixedFromReal(1), FixedFromReal(0),
		FixedFromReal(0), FixedFromReal(1)
	};

	static MAT2 matFlipped =
	{
		FixedFromReal(1), FixedFromReal(0),
		FixedFromReal(0), FixedFromReal(-1)
	};

	MAT2 * pMat = bFlipY ? &matFlipped : &matUnity;

	m_pPath = new GraphicsPath;
	if (! m_pPath) return;

	GLYPHMETRICS gm;

	UINT uFormat = GGO_NATIVE;
#if (_WIN32_WINNT >= 0x0500)
	uFormat |= GGO_UNHINTED;	// Gives a result which seems closer to GraphicsPath::AddString
#endif

	// Ask Windows GDI how big the buffer must be.
	DWORD nBytes = ::GetGlyphOutline(hDC, c, uFormat, & gm, 0, NULL, pMat);
	
	if (nBytes > 0)	// if 0: no glyph (space character).
	{
		BYTE * pBuf = new BYTE[nBytes];
		if (pBuf)
		{
			Status stat(Ok);

			// Get the glyph outline from Window's depths.
			::GetGlyphOutline(hDC, c, uFormat, & gm, nBytes, pBuf, pMat);

			BYTE * pData(pBuf);

			// Transform the outline data from GetGlyphOutline, which are in a very contrived
			// format with FIXED real numbers, in a form GDI+ will understand and add them to path.
			// Refer to MSDN KB 243285.
			while (nBytes && stat == Ok)
			{
				// Data consists of one or more TTPOLYGONHEADERs...
				TTPOLYGONHEADER * pPH = (TTPOLYGONHEADER *) pData;
				if (pPH->dwType != TT_POLYGON_TYPE) break;

				PointF pntStart(RealFromFixed(pPH->pfxStart.x), RealFromFixed(pPH->pfxStart.y));

				pData += sizeof(TTPOLYGONHEADER);
				int nCurveBytes = pPH->cb - sizeof(TTPOLYGONHEADER);

				while (nCurveBytes && stat == Ok)
				{
					// ... each followed by one or more TTPOLYCURVEs...
					TTPOLYCURVE * pPC = (TTPOLYCURVE *) pData;

					POINTFX * pFX = pPC->apfx;

					// ... which can be of several types.
					switch (pPC->wType)
					{
					case TT_PRIM_LINE:
						{
							// Multiple lines: store as line in path.
							for (int i = 0; i < pPC->cpfx; ++i)
							{
								PointF pntEnd(RealFromFixed(pFX->x), RealFromFixed(pFX->y));
								++pFX;

								stat = m_pPath->AddLine(pntStart, pntEnd);
								if (stat != Ok) break;

								pntStart = pntEnd;
							}
						}
						break;
					case TT_PRIM_QSPLINE:
						{
							// Multiple quadratic B-splines: transform to cubic Bézier curves and store in path.
							for (int i = 0; i < pPC->cpfx;)
							{
								PointF pntB(RealFromFixed(pFX->x), RealFromFixed(pFX->y));		// Quadratic control point.
								++pFX;
								++i;

								PointF pntEnd(RealFromFixed(pFX->x), RealFromFixed(pFX->y));	// Quadratic end point.

								if (i == (pPC->cpfx - 1))
								{
									// Last end point is given explicitly, so do not modify pntEnd...
									++pFX;	// ...but do update pointer...
									++i;	// ...and counter.
								}
								else
								{
									// End point is midpoint of control point (pntB) and next point (pntEnd).
									pntEnd = pntB + pntEnd;
									pntEnd.X *= 0.5f;
									pntEnd.Y *= 0.5f;
								}

								// Convert quadratic B-spline to cubic Bézier.
								// Start and end points are unmodified.
								PointF pnt1(pntStart);		// First Bézier control point.
								pnt1.X += (2.0f / 3.0f) * (pntB.X - pntStart.X);
								pnt1.Y += (2.0f / 3.0f) * (pntB.Y - pntStart.Y);

								PointF pnt2(pntB);			// Second Bézier control point.
								pnt2.X += (pntEnd.X - pntB.X) / 3.0f;
								pnt2.Y += (pntEnd.Y - pntB.Y) / 3.0f;

								stat = m_pPath->AddBezier(pntStart, pnt1, pnt2, pntEnd);
								if (stat != Ok) break;

								pntStart = pntEnd;
							}
						}

						break;

					// The following code is never executed, and therefore commented out.
					// Under Windows 2000/XP, it is possible to get the glyph outline
					// directly in cubic Bézier format (call GetGlyphOutline with data format GGO_BEZIER).
					// This would make this function slightly less complex.
					// However, because the code wouldn't run under older Windows versions, we use the
					// 'classic' way. There isn't much gain in speed anyway.
/*
					case TT_PRIM_CSPLINE:
						{
							// Multiple cubic Bézier curve: store directly in path.
							for (int i = 0; i < pPC->cpfx; i += 3)
							{
								PointF pnt1(RealFromFixed(pFX->x), RealFromFixed(pFX->y));		// First Bézier control point.
								++pFX;

								PointF pnt2(RealFromFixed(pFX->x), RealFromFixed(pFX->y));		// Second Bézier control point.
								++pFX;

								PointF pntEnd(RealFromFixed(pFX->x), RealFromFixed(pFX->y));	// Bézier end point.
								++pFX;

								stat = m_pPath->AddBezier(pntStart, pnt1, pnt2, pntEnd);
								if (stat != Ok) break;

								pntStart = pntEnd;
							}
						}
						break;
*/
					default:
						break;
					}

					int nBytesProcessed = sizeof(TTPOLYCURVE) + (pPC->cpfx - 1) * sizeof(POINTFX);

					pData += nBytesProcessed;
					nCurveBytes -= nBytesProcessed;
				}

				// Can't find documentation for this, but some PostScript fonts
				// obviously need this.
				stat = m_pPath->CloseFigure();

				nBytes -= pPH->cb;
			}
		}
		delete[] pBuf;
	}

	// Remember the cell dimensions. Y value will be 0 for western fonts.
	m_CellInc.X = (REAL) gm.gmCellIncX;
	m_CellInc.Y = (REAL) gm.gmCellIncY;
}
