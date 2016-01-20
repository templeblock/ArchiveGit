// ****************************************************************************
//
//  File Name:			GlyphGraphic.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Definition of the RGlyphGraphic class
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
//  $Logfile:: /PM8/Framework/Source/GlyphGraphic.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "GlyphGraphic.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::RGlyphGraphic( )
//
// Description:		Copy Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RGlyphGraphic::RGlyphGraphic( const RGlyphGraphic &rhs ) 
{
	m_fTrueType = rhs.m_fTrueType;
	m_Id = rhs.m_Id; 
}

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::Load( )
//
// Description:		
//
// Returns:				TRUE if the graphic was loaded and parsed successfuly.
//							FALSE if there was a parsing error (i.e. an ATM or TrueType error)
//
// Exceptions:			Memory exception
//
// ****************************************************************************
//
BOOLEAN RGlyphGraphic::Load( const RFont& /* font */, uWORD /* Id */ )
{





	return FALSE;
}


#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RGlyphGraphic::Validate( ) const
	{
	RSingleGraphic::Validate( );
	TpsAssertIsObject( RGlyphGraphic, this );
	}

#endif	// TPSDEBUG
