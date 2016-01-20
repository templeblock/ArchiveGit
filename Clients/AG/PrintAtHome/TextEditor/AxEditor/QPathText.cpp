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
// Version 1.0, September 29, 2005.
// (c) 2005, Sjaak Priester, Amsterdam.
// http://www.sjaakpriester.nl

#include "StdAfx.h"
#include "QPathText.h"

#include <cmath>

//==========================================
// Utility function

// Return: parameter t of (first) intersection of line segment pntA->pntB with circle (center, radius); r2 = radius^2.
// -1: no intersection
REAL LineSegmentCircle(PointF pntA, PointF pntB, PointF center, REAL r2)
{
	// Translate to (0, 0) to get rid of center.
	pntA = pntA - center;
	pntB = pntB - center;

	// Calculate squares of distance to (0, 0)
	REAL rA2 = pntA.X * pntA.X + pntA.Y * pntA.Y;
	REAL rB2 = pntB.X * pntB.X + pntB.Y * pntB.Y;

	// If line segment is completely outside circle => no intersection, return -1
	if (rA2 > r2 && rB2 > r2) return -1;

	// If line segment is completely inside circle => no intersection, return -1
	if (rA2 < r2 && rB2 < r2) return -1;

	PointF pntDiff = pntB - pntA;

	// line segment:
	//		x = xA + t * xDiff
	//		y = yA + t * yDiff, t >= 0, t <= 1
	// circle:
	//		x^2 + y^2 = r2
	// results in standard qudratic equation:
	//		a * t^2 + b * t + c = 0
	// with:
	//		a = xDiff^2 + yDiff^2
	//		b = 2 * (xA * xDiff + yA * yDiff)
	//		c = rA2 - r2

	REAL a = pntDiff.X * pntDiff.X + pntDiff.Y * pntDiff.Y;
	REAL b = 2 * (pntA.X * pntDiff.X + pntA.Y * pntDiff.Y);
	REAL c = rA2 - r2;

	// discriminant D = b^2 - 4 * a * c
	REAL D = b * b - 4 * a * c;

	REAL t = -1;		// indicate no intersection (wouldn't be possible)

	// one intersection: t = -b / 2 * a
	if (D == 0) t = - b / (2 * a);

	// two intersections: (t = -b +/- sqrt(D)) / 2 * a
	else if (D > 0)
	{
		REAL sqrtD = sqrt(D);
		REAL t1 = (- b + sqrtD) / (2 * a);
		REAL t2 = (- b - sqrtD) / (2 * a);

		if (t1 >= 0 && t1 <= 1)
			t = (t2 >= 0 && t2 < t1) ? t2 : t1;
		else if (t2 >= 0 && t2 <= 1) t = t2;
	}
	return t;
}


//======================================
// QPathText
//
// Internally, QPathText works with 'positions' throughout.
// A 'position' is one point on the flattened guide path, expressed as a REAL number.
// The whole part determines on which segment the point is.
// The fractional part determines the exact location on the segment.
// So, a position of 2.5f means that the point is (parametrically) halfway on the third segment, in other
// words: it's exactly halfway between the third and fourth guide path point.

QPathText::QPathText(const GraphicsPath& guidePath, REAL flatness)
: m_pGuidePath(guidePath.Clone())
, m_Rotation(1)
, m_Spacing(0)
, m_IndentPosition(0)
{
	// It all works only if the guide path is flattened.
	// By expecting the guide path in the constructor, we are
	// sure that this is the case.
	if (m_pGuidePath) m_pGuidePath->Flatten(0, flatness);
}

QPathText::~QPathText()
{
	delete m_pGuidePath;
}

void QPathText::SetSpacing(REAL cs)
{
	HDC hDC = ::GetDC(NULL);
	if (hDC)
	{
		HGDIOBJ hOldFont = ::SelectObject(hDC, m_hFont);

		TEXTMETRIC tm;
		::GetTextMetrics(hDC, & tm);

		m_Spacing = ((float)tm.tmAveCharWidth/1000.0f)*cs;

		::SelectObject(hDC, hOldFont);
		::ReleaseDC(NULL, hDC);
	}
}

int QPathText::GetPathText(GraphicsPath& path, LPCTSTR str, REAL indent)
{
	// The main public interface, called to do the works.

	path.SetFillMode(FillModeWinding);

	PathPosition pos(GetPathPosition(indent));
	m_IndentPosition = pos;
	const TCHAR * p = str;

	while (* p && pos >= 0)
	{
		const TCHAR * pNext = ::CharNext(p);
		pos = AddCharacter(* p, * pNext, path, pos);
		p = pNext;
	}

	if (pos < 0) --p;

	return (int)(p - str);
}

QPathText::PathPosition QPathText::AddCharacter(TCHAR c, TCHAR cNext, GraphicsPath& path, PathPosition position) const
{
	// Add one character to path. The origin of the character cell sits on the
	// guide curve, at position.

	// Get glyph from base class.
	PointF pntOrigin(0, 0);
	GraphicsPath pathGlyph;
	AddGlyph(c, cNext, pntOrigin, &pathGlyph);

	// pntOrigin will be updated to the lower right of the character cell.
	REAL wCell = pntOrigin.X + pntOrigin.Y;		// cell width; Y-value will probably be 0.
	if (wCell == 0) return -1;				// Can't work with this.

	// Use this information to find the right position.
	PathPosition posRight = FindRightPosition(position, wCell);
	if (posRight < 0) return posRight;		// Some error, ignore this glyph and return start position.

	// Convert lower left and lower right to Euclidian PointF's.
	PointF pntLeft = GetPathPoint(position);
	PointF pntRight = GetPathPoint(posRight);

	// Use these to calculate the cosine and sine of the rotation.
	// Note that we don't need the value of the angle itself.
	PointF pntDiff = pntRight - pntLeft;

	REAL cosPhi = pntDiff.X / wCell;		// wCell != 0.
	REAL sinPhi = pntDiff.Y / wCell;

	// Fill in a transformation matrix, employing some clever
	// math for the rotation amount.
	Matrix mx(
		cosPhi, sinPhi,
		- m_Rotation * sinPhi, 1 + m_Rotation * (cosPhi - 1),
		pntLeft.X, pntLeft.Y);

	// Transform glyph.
	pathGlyph.Transform(& mx);

	// Add to path.
	path.AddPath(& pathGlyph, FALSE);

	posRight += m_Spacing;
	posRight = (posRight < m_IndentPosition) ? m_IndentPosition : posRight;

	return posRight;
}

PointF QPathText::GetPathPoint(PathPosition position) const
{
	// Convert position on (flattened) guide path to Euclidian PointF.
	PointF r(0, 0);
	if (position < 0) return r;

	int iSegment = (int) floor(position);

	if (iSegment < m_pGuidePath->GetPointCount() - 1)
	{
		PathData pd;

		Status stat = m_pGuidePath->GetPathData(& pd);
		if (stat == Ok)
		{
			r = pd.Points[iSegment];
			PointF diff = pd.Points[iSegment +1] - pd.Points[iSegment];

			REAL t = position - iSegment;
			r.X += t * diff.X;
			r.Y += t * diff.Y;
		}
	}
	return r;
}

QPathText::PathPosition QPathText::FindRightPosition(PathPosition posCenter, REAL radius) const
{
	// Find the first position to the right where the (flattened) guide path intersects with circle.

	if (posCenter < 0) return -1;

	int iStartSegment = (int) floor(posCenter);

	if (iStartSegment >= m_pGuidePath->GetPointCount() - 1) return -1;

	PathData pd;

	Status stat = m_pGuidePath->GetPathData(& pd);
	if (stat == Ok)
	{
		PointF pntStart = GetPathPoint(posCenter);

		REAL r2 = radius * radius;

		// First, see if the intersection point is on the first segment.
		// I.o.w. see whether the radius is closer than the right point of the first segment.
		PointF diffRight = pntStart - pd.Points[iStartSegment + 1];
		REAL r2Right = diffRight.X * diffRight.X + diffRight.Y * diffRight.Y;
		if (r2 < r2Right)
		{
			// Yes, it is, so intersection point is on first segment.
			PointF diffLeft = pntStart - pd.Points[iStartSegment];
			REAL r2Left = diffLeft.X * diffLeft.X + diffLeft.Y * diffLeft.Y;

			REAL rLeft = sqrt(r2Left);		// Distance from start point to left point of segment.
			REAL rRight = sqrt(r2Right);	// Distance from start point to right point of segment.

			return iStartSegment + 1.0f - (rRight - radius) / (rRight + rLeft);
		}

		// No, the searched point is not on the first segment.
		// So search in next segments.
		for (int i = iStartSegment + 1; i < m_pGuidePath->GetPointCount() - 1; ++i)
		{
			REAL t = LineSegmentCircle(pd.Points[i], pd.Points[i + 1], pntStart, r2);

			if (t >= 0) return i + t;	// Found.
		}
	}
	return -1;							// Not found.
}

QPathText::PathPosition QPathText::GetPathPosition(REAL indent) const
{
	// Transform indent in logical co-ordinates in position.
	PathData pd;

	Status stat = m_pGuidePath->GetPathData(& pd);
	if (stat == Ok)
	{
		for (int i = 0; i < m_pGuidePath->GetPointCount() - 1; ++i)
		{
			PointF pntDiff = pd.Points[i] - pd.Points[i + 1];
			REAL length = sqrt(pntDiff.X * pntDiff.X + pntDiff.Y * pntDiff.Y);
			if (indent < length) return i + indent / length;

			indent -= length;
		}
	}
	return -1;
}
