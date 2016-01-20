// ****************************************************************************
//
//  File Name:			VectorRect.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RVectorRect class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/VectorRect.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "VectorRect.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef	_WINDOWS

// ****************************************************************************
//
// Function Name:		::RotateRect( )
//
// Description:		Function for external applications (ie Park Row) so that
//							they can rotate rects. Renaissance should not use
//
//							The angle is a counterclockwise rotation in degrees
//
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RotateRect( CRect& rect, YAngle angle )
	{
	// Create a vector rect
	RIntVectorRect vectorRect( rect.left, rect.top, rect.right, rect.bottom );

	// Rotate it
	vectorRect.RotateAboutCenter( ::DegreesToRadians( angle ) );

	// Copy the rotated bounding rect
	rect.left = vectorRect.m_TransformedBoundingRect.m_Left;
	rect.top = vectorRect.m_TransformedBoundingRect.m_Top;
	rect.right = vectorRect.m_TransformedBoundingRect.m_Right;
	rect.bottom = vectorRect.m_TransformedBoundingRect.m_Bottom;
	}

#endif	//	_WINDOWS

