// ****************************************************************************
//
//  File Name:			HyperlinkNode.h
//
//  Project:			Framework
//
//  Author:				Richard Grenfell
//
//  Description:		Declaration of the RHyperlinkNode class.
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

#ifndef HYPERLINKNODE_H
#define HYPERLINKNODE_H

#include "URL.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RHyperlinkNode
//
//  Description:	Definition of a Hyperlink list node.
//						This class stores the data necessary to define a hyperlink.
//						This consists of the URL information and the location.
//						This also stores a link to the next node.
//
// ****************************************************************************
//
class FrameworkLinkage RHyperlinkNode
{
	public:
		// Construct a node with specific data defined.
		RHyperlinkNode( const RURL &rUrl, const YComponentBoundingRect &rVectRect );

		// Construct an image map string for this hyperlink.
		CString GetImageMapString() const;

		// Get a pointer to the next node.
		inline RHyperlinkNode * NextNode() const
		{
			ASSERT( this != NULL );
			return m_pNext;
		}

		// Retrieves a copy of the URL for the node.
		inline RURL GetURL() const
		{
			ASSERT( this != NULL );
			return m_URL;
		}
		// Set the URL for this node.
		inline void SetURL( const RURL &rURL )
		{
			ASSERT( this != NULL );
			m_URL = rURL;
		}
		// Set the position for this node.
		inline void SetPosition( const YComponentBoundingRect &rRect )
		{
			ASSERT( this != NULL );
			m_Rect = rRect;
		}
		// Set the node that follows this one.
		inline void SetNext( RHyperlinkNode *pNext )
		{
			ASSERT( this != NULL );
			m_pNext = pNext;
		}

	protected:
		// Construct a string with the coordinates defined.
		CString GetCoordinatesString() const;

	private:
		// Stores the URL information for the hyperlink.
		RURL m_URL;
		// The position of the hyperlink.
		YComponentBoundingRect m_Rect;
		// The next hyperlink node after this one.
		RHyperlinkNode *m_pNext;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// HYPERLINKNODE_H