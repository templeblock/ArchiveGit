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

// QPathText
//
// C++ class to create a Windows GDI+ path text, of which the characters follow
// a random (curved or otherwise) guide path.
//
// The guide path is presented as a GDI+ GraphicsPath to QPathText's constructor.
// The resulting text path is assembled in another GDI+ GraphicsPath by calling
// the member function GetPathText.
//
// See the QPathTextDemo application for a demonstration.
//
// Source:
//		QPathText.cpp
//
// Other files needed:
//		QGraphicsText.h
//		QGraphicsText.cpp
//
// Environment: Windows 98 and later, GDI+, Visual C++ 7.0/7.1, MFC.
// Probably compatible with Visual C++ 6.0 as well.
//
//==============
// Version 1.0, September 29, 2005.
// (c) 2005, Sjaak Priester, Amsterdam.
// http://www.sjaakpriester.nl
//
// Use at your ownn risk. Comments welcome.

#pragma once

#include "QGraphicsText.h"

class QPathText : public QGraphicsText
{
public:
	QPathText(const GraphicsPath& guidePath, REAL flatness = 10.0f * FlatnessDefault);
	// Create a QPathText with guidePath as guide.
	// Parameter flatness determines the accuracy of the 'flattening'. The default
	// will be appropriate for almost any case.

	~QPathText();

	int GetPathText(GraphicsPath& path, LPCTSTR str, REAL indent = 0);
	// Add the string str to the GraphicsPath path.
	// The string will follow the guide path.
	//
	// Parameter indent defines the starting point of the string path,
	// measured along the the guide path.
	//
	// If str is too long to fit alongside the guide path, it will be truncated.
	//
	// Return: number of characters processed (may be less than length of str).
	//
	// Note that GetPathText will not clear the path, before it adds the string.
	// It will set the path's fill mode to FillModeWinding.

	void SetSpacing(REAL cs);
	void SetRotation(REAL rot)		{ m_Rotation = rot; }
	REAL GetRotation() const		{ return m_Rotation; }
	// Set or get the amount of rotation.
	//
	// If rot equals 1.0f, QTextPath operates normally. That is, the characters rotate fully
	// according to the direction of the guide path.
	// If rot equals 0.0f, the characters aren't rotated at all, just 'skewed' to follow
	// the guide path.
	// Values between 0.0f and 1.0f let QTextPath operate in an intermediate mode. Characters
	// will be partly rotated and partly skewed.
	//
	// The parameter really should be between 0.0f and 1.0f. Other values do work, but yield a very
	// distorted result.

protected:
	typedef REAL PathPosition;

	PathPosition AddCharacter(TCHAR c, TCHAR cNext, GraphicsPath& path, PathPosition position) const;
	PointF GetPathPoint(PathPosition position) const;
	PathPosition GetPathPosition(REAL indent) const;
	PathPosition FindRightPosition(PathPosition posCenter, REAL radius) const;

	GraphicsPath * m_pGuidePath;
	REAL m_Rotation;
	REAL m_Spacing;
	REAL m_IndentPosition;

};
