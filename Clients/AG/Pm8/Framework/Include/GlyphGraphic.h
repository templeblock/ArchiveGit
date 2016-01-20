// ****************************************************************************
//
//  File Name:			GlyphGraphic.h
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RGlyphGraphic class
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
//  $Logfile:: /PM8/Framework/Include/GlyphGraphic.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GLYPHGRAPHIC_H_
#define		_GLYPHGRAPHIC_H_

#ifndef		_SINGLEGRAPHIC_H_
#include		"SingleGraphic.h"
#endif

// ****************************************************************************
//
// Class Name:			RGlyphGraphic
//
// Description:		Class to 
//
// ****************************************************************************
//
class FrameworkLinkage RGlyphGraphic : public RSingleGraphic					 
{
	// Construction, Destruction & Initialization
	public :
												RGlyphGraphic( );
												RGlyphGraphic( const RGlyphGraphic &rhs );
		virtual								~RGlyphGraphic( );

	// Operations
	public :
		BOOLEAN								Load( const RFont& font, uWORD Id );

	// Accessors
	public :
		uWORD									GetId( ) const;
		BOOLEAN								IsTrueType( ) const;

	// Member data
	private :
		BOOLEAN								m_fTrueType;
		uWORD									m_Id;											// character code

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::RGlyphGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RGlyphGraphic::RGlyphGraphic( ) : m_fTrueType( FALSE ),
	  												  m_Id( 0 )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::~RGlyphGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RGlyphGraphic::~RGlyphGraphic( )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::IsTrueType( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
inline BOOLEAN RGlyphGraphic::IsTrueType( ) const
{
	return m_fTrueType;
}

// ****************************************************************************
//
// Function Name:		RGlyphGraphic::GetId( )
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
inline uWORD RGlyphGraphic::GetId( ) const
{
	return m_Id;
}

#endif		// _GLYPHGRAPHIC_H_
