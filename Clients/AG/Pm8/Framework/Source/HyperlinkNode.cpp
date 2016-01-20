// ****************************************************************************
//
//  File Name:			HyperlinkNode.cpp
//
//  Project:			Framework
//
//  Author:				Richard Grenfell
//
//  Description:		Implementation of the RHyperlinkNode class.
//							This class stores the data necessary to define a hyperlink.
//							This consists of the URL information and the location.
//
//  Portability:		Platform independent.
//
//  Developed by:		Broderbund Software, Inc.
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

#include "FrameworkIncludes.h"
#include "HyperlinkNode.h"


// ****************************************************************************
//
//  Function Name:	RHyperlinkNode::RHyperlinkNode()
//
//  Description:		Construct a node with specific data defined.
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RHyperlinkNode::RHyperlinkNode( const RURL &rUrl, const YComponentBoundingRect &rVectRect )
{
	m_pNext = NULL;
	m_URL = rUrl;
	m_Rect = rVectRect;
}


// ****************************************************************************
//
//  Function Name:	RHyperlinkNode::GetImageMapString()
//
//  Description:		Construct an image map string for this hyperlink.
//
//  Returns:			A string with the complete URL text for an image map entry.
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
CString RHyperlinkNode::GetImageMapString() const
{
	CString strCoords = GetCoordinatesString();
	RURL rUrl = m_URL;	// Copy to a local to retain const status.
	CString strURL = rUrl.GetURL();
	CString strImageMap = "<area shape=\"poly\" coords=\"" + strCoords + "\" href=\"" + strURL + "\">";

	return strImageMap;
}


// ****************************************************************************
//
//  Function Name:	RHyperlinkNode::GetCoordinatesString()
//
//  Description:		Construct a string with the coordinates defined.
//
//  Returns:			A string specifying the coordinates for use in an image map.
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
CString RHyperlinkNode::GetCoordinatesString() const
{
	CString strCoord;
	strCoord.Format( "%d,%d,%d,%d,%d,%d,%d,%d",
							(int)m_Rect.m_TopLeft.m_x, (int)m_Rect.m_TopLeft.m_y,
							(int)m_Rect.m_TopRight.m_x, (int)m_Rect.m_TopRight.m_y,
							(int)m_Rect.m_BottomRight.m_x, (int)m_Rect.m_BottomRight.m_y,
							(int)m_Rect.m_BottomLeft.m_x, (int)m_Rect.m_BottomLeft.m_y );
	return strCoord;
}
