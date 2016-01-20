// ****************************************************************************
//
//  File Name:			RasterFont.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RRasterFont class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/RasterFont.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"RasterFont.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RRasterFont::RRasterFont( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RRasterFont::RRasterFont( const YFontInfo& fontInfo, BOOLEAN fInitialize )
	: RFont( fontInfo, fInitialize )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RRasterFont::GetGlyphPath( )
//
//  Description:		Return the path that constitues the given character data
//
//  Returns:			Return the path data for the character
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
RPath*	RRasterFont::GetGlyphPath( const RCharacter& /* character */ )
	{
	TpsAssertAlways( "The RasterFont should never be called to retrieve its glyph path data" );
	return NULL;
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RRasterFont::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRasterFont::Validate( ) const
	{
	RFont::Validate( );
	TpsAssertIsObject( RRasterFont, this );
	}

#endif	//	TPSDEBUG


