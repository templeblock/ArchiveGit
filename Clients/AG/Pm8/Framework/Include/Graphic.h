// ****************************************************************************
//
//  File Name:			Graphic.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas, Greg Brown
//
//  Description:		Declaration of the RGraphic class
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
//  $Logfile:: /PM8/Framework/Include/Graphic.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDrawingSurface;
class RPath;

// ****************************************************************************
//
// Class Name:			RGraphic
//
// Description:		Base class for graphics, headlines, etc.
//
// ****************************************************************************
//
class FrameworkLinkage RGraphic : public RObject
{			
	public:
													RGraphic( );
													RGraphic( const RGraphic &rhs );
		virtual									~RGraphic( );

		// Operations
	public :
		RGraphic&								operator=( const RGraphic& rhs );

		virtual void							Render( RDrawingSurface& drawingSurface,
															  const RRealSize& size,
															  const R2dTransform& transform,
															  const RIntRect& rRender,
															  const RColor& monochromeColor = RSolidColor( kBlack ),
															  BOOLEAN fImageAsMask = FALSE ) = 0;
		virtual BOOLEAN						GetOutlinePath(RPath& path, const RRealSize& size, const R2dTransform& transform) = 0;
		virtual void							OnXDisplayChange( );

		// Accessors
		virtual YFloatType					GetAspectRatio() const;		
		
		virtual BOOLEAN						IsMonochrome( ) const;
		RColor									GetMonochromeColor( ) const;
		void										SetMonochromeColor( const RColor& color );	

	protected:
		virtual void							Undefine( ) = 0;

	//	Members
	protected :			
		RColor									m_monochromeColor;

#ifdef TPSDEBUG
	public:
		void										Validate() const;
#endif
};


// ****************************************************************************
//
// Function Name:		RGraphic::RGraphic( )
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RGraphic::RGraphic() 
:	m_monochromeColor( kBlack )
{
}

// ****************************************************************************
//
// Function Name:		RGraphic::operator=( )
//
// Description:		assignment operator
//
// Returns:				reference to self
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RGraphic& RGraphic::operator=( const RGraphic& rhs )
{
	m_monochromeColor = rhs.m_monochromeColor;
	return *this;
}

// ****************************************************************************
//
// Function Name:		RGraphic::RGraphic( )
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RGraphic::RGraphic( const RGraphic& rhs ) :	m_monochromeColor( rhs.m_monochromeColor )
{
	*this = rhs;
}

// ****************************************************************************
//
// Function Name:		RGraphic::RGraphic( )
//
// Description:		Destructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RGraphic::~RGraphic( )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RGraphic::GetAspectRatio( )
//
// Description:		Derived classes will return the aspect ratio of the
//							graphic; this asserts and returns 1.0
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
inline YFloatType RGraphic::GetAspectRatio() const
{	
	return 1.0;
}

// ****************************************************************************
//
// Function Name:		RGraphic::IsMonochrome( )
//
// Description:		Accessor
//
// Returns:				m_monochromeColor
//
// Exceptions:			None
//
// ****************************************************************************
//
inline BOOLEAN RGraphic::IsMonochrome( ) const
{
	return FALSE;
}

// ****************************************************************************
//
// Function Name:		RGraphic::GetMonochromeColor( )
//
// Description:		Accessor
//
// Returns:				m_monochromeColor
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RColor RGraphic::GetMonochromeColor( ) const
{
	return m_monochromeColor;
}

// ****************************************************************************
//
// Function Name:		RGraphic::SetMonochromeColor( )
//
// Description:		Sets the monochrome color
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline void RGraphic::SetMonochromeColor( const RColor& color )
{
	m_monochromeColor = color;
}

// ****************************************************************************
//
//  Function Name:	RGraphic::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RGraphic::OnXDisplayChange( )
	{
	;
	}

#ifdef TPSDEBUG
// Validate()
inline void RGraphic::Validate() const
{
}
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _GRAPHIC_H_
