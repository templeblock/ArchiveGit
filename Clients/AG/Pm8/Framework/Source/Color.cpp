// ****************************************************************************
// @Doc
// @Module Color.cpp |
//
//  File Name:			Color.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				Michael G. Dunlap
//
//  Description:		implementations of the RColor outline class members
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
//  $Logfile:: /PM8/Framework/Source/Color.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Color.h"

#undef FrameworkLinkage
#define FrameworkLinkage

#include	"DrawingSurface.h"
#include	"GradientFill.h"
#include	"ImageColor.h"

const uWORD kTaggedFormat = 'TG';
//
// Constructors requiring knowledge of the RGradientFill
//

RColor::RColor( const RSolidColor* pColors, int nColors, YAngle angle ) :
	m_eFillMethod( kTransparent ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( kBlack ),
	m_pGradient( NULL ),
	m_pImageColor( NULL )
{
	TpsAssert( pColors!=NULL && nColors>0, "No colors given for constructing a blend" );
	if( nColors == 1 )
		{
		m_rgbFill		= *pColors;
		m_eFillMethod	= kSolid;
		return;
		}
	else if( nColors>1 )
		{
		//
		// Build the color ramp...
		RGradientRamp*	pRamp				= new RGradientRamp();
		YRealDimension	nLastRampPoint	= YRealDimension(nColors-1);
		for( int iRampPoint=0 ; iRampPoint<nColors ; ++iRampPoint )
			{
			YPercentage	pctMidpoint	= (iRampPoint==(nColors-1))? 1.0 : 0.5;
			pRamp->Add( RGradientRampPoint(pColors[iRampPoint],pctMidpoint,YPercentage(iRampPoint/nLastRampPoint)) );
			}
		//
		// Create the gradient...
		m_pGradient	= new RGradientFill();
		m_pGradient->SetType( kLinear );
		m_pGradient->SetRamp( pRamp );
		m_pGradient->SetGradientAngle( angle );
		m_pGradient->SetBoundingRect( RIntRect(RIntSize(100,100)) );
		//
		// Set our internal color type...
		m_eFillMethod = kGradient;
		}
	//
	// and we're done...
}

RColor::RColor( const RGradientFill& gradient ) :
	m_eFillMethod( kGradient ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( kBlack ),
	m_pGradient( new RGradientFill(gradient) ),
	m_pImageColor( NULL )
{
	//
	// copy the ramp...
	m_pGradient->SetRamp( new RGradientRamp(*gradient.GetRamp()) );
}

RColor::RColor( const RImageColor& image ) :
	m_eFillMethod( kBitmap ),
	m_eFillEffect( kOpaque ),
	m_rgbFill( kBlack ),
	m_pGradient( NULL ),
	m_pImageColor( new RImageColor( image ) )
{
}

RColor::RColor( const RColor& fill ) :
	m_eFillMethod( fill.m_eFillMethod ),
	m_eFillEffect( fill.m_eFillEffect ),
	m_rgbFill( fill.m_rgbFill ),
	m_pGradient( NULL ),
	m_pImageColor( NULL )
{
	if( m_eFillMethod==kGradient && fill.m_pGradient )
		{
		//
		// we need to copy the ramp!
		m_pGradient = new RGradientFill( *fill.m_pGradient );
		m_pGradient->SetRamp( new RGradientRamp(*fill.m_pGradient->GetRamp()) );
		}
	else if( m_eFillMethod==kBitmap && fill.m_pImageColor )
		{
		//
		// we need to copy the bitmap!
		m_pImageColor = new RImageColor( *fill.m_pImageColor );
		}
}

//
// @MFunc destroy an RColor
//
RColor::~RColor()
{
	if( m_pGradient )
		{
		//
		// we need to delete the ramp!
		RGradientRamp*	pRamp	= const_cast<RGradientRamp*>( m_pGradient->GetRamp() );
		delete pRamp;
		delete m_pGradient;
		}
	if( m_pImageColor )
		{
		//
		// we need to delete the bitmap
		delete m_pImageColor;
		}
}

//
// @MFunc assignment operator
// @Syntax RColor newfill = oldfill
// @RDesc const reference to newfill
//
const RColor& RColor::operator=( const RColor& rhs )
{
	if( &rhs != this )
		{
		if( m_pGradient )
			{
			//
			// we need to delete the ramp!
			RGradientRamp*	pRamp	= const_cast<RGradientRamp*>( m_pGradient->GetRamp() );
			delete pRamp;
			delete m_pGradient;
			m_pGradient	= NULL;
			}
		if( m_pImageColor )
			{
			//
			// we need to delete the bitmap...
			delete m_pImageColor;
			m_pImageColor = NULL;
			}
		m_eFillMethod	= rhs.m_eFillMethod;
		m_eFillEffect	= rhs.m_eFillEffect;
		m_rgbFill		= rhs.m_rgbFill;
		if( m_eFillMethod==kGradient && rhs.m_pGradient )
			{
			//
			// we need to copy the ramp!
			m_pGradient = new RGradientFill( *rhs.m_pGradient );
			m_pGradient->SetRamp( new RGradientRamp(*rhs.m_pGradient->GetRamp()) );
			}
		else if( m_eFillMethod==kBitmap && rhs.m_pImageColor )
			{
			//
			// we need to copy the bitmap
			m_pImageColor = new RImageColor( *rhs.m_pImageColor );
			}
		}
	return *this;
}

//
// @MFunc transform this RColor
// @RDesc const reference to this RColor
//
const RColor& RColor::operator*=(
			const R2dTransform&	transform )	// @Parm the tranformation to apply
{
	if( m_pGradient )
		m_pGradient->ApplyTransform( transform );

	else if( m_pImageColor )
		m_pImageColor->ApplyTransform( transform );

	return *this;
}

BOOLEAN RColor::operator==( const RColor& rhs ) const
	{
	if( m_eFillMethod!=rhs.m_eFillMethod || m_eFillEffect!=rhs.m_eFillEffect )
		return FALSE;

	switch( m_eFillMethod )
		{
		case kTransparent:
			return TRUE;

		case kSolid:
			return m_rgbFill == rhs.m_rgbFill;

		case kGradient:
			if( m_pGradient && rhs.m_pGradient )
				return *m_pGradient == *rhs.m_pGradient;
			if( !m_pGradient && !rhs.m_pGradient )
				return BOOLEAN(TRUE);
			return BOOLEAN(FALSE);

		case kBitmap:

			if( m_pImageColor && rhs.m_pImageColor )
				return *m_pImageColor == *rhs.m_pImageColor;

			if( !m_pImageColor && !rhs.m_pImageColor )
				return BOOLEAN(TRUE);

			return BOOLEAN(FALSE);

		default:
			TpsAssertAlways( "Read an invalid fill method" );
			return FALSE;
		}
	}

//
// @MFunc obtain RImageColor aspect
// @RDesc const reference to the Image color
//
RColor::operator RImageColor() const
{
	// Note: this will throw an exception if no image color is available.
	// It is up to the user to catch this error, or validate color type
	// before calling.
	return *m_pImageColor;
}

//
// @MFunc get the solid color 
// @RDesc the solid color
//
const RSolidColor RColor::GetSolidColor( YPercentage pct ) const
	{
	//
	//	If gradient, get the gradient ramp color
	if (GetFillMethod() == RColor::kGradient)
		return GetGradientFill()->GetRamp()->GetColor( pct ) ;
	//
	// If bitmap, just return White.
//	else if ( GetFillMethod() == RColor::kBitmap )
//		return RSolidColor( kWhite );

	return m_rgbFill ;
	}

//
// @MFunc Read this fillcolor from the given storage object
//
// @RDesc nothing
//
void RColor::Read(
			RArchive&	ar )	// @Parm the archive to read from
{
	//
	// Make sure we're clean...
	if( m_pGradient )
		{
		RGradientRamp*	pRamp	= const_cast<RGradientRamp*>( m_pGradient->GetRamp() );
		delete pRamp;
		delete m_pGradient;
		m_pGradient	= NULL;
		}
	if( m_pImageColor )
		{
		delete m_pImageColor;
		m_pImageColor = NULL;
		}

	//
	// Read the fill method and effect.
	uWORD	uwFill, uwEffect;
	ar	>> uwFill;
	ar	>> uwEffect;

	m_eFillEffect	= EFillEffect( uwEffect );

	if (kTaggedFormat == uwFill)
	{
		uWORD uwTagCount;
		ar >> uwTagCount;

		for (uWORD i = 0; i < uwTagCount; i++)
		{
			uWORD uwTag;
			uLONG ulSize;
			
			ar >> uwTag;
			ar >> ulSize;

			switch (uwTag)
			{
			case kOldBitmap:	// Fall through
			case kBitmap:
				m_eFillMethod = kBitmap;
				m_pImageColor = new RImageColor;
				m_pImageColor->Read( ar );
				break;

			default:
				{
					// Because archive does not support 
					// positioning, we have to read the
					// data into a temporary buffer.
					uBYTE* pBuffer = new uBYTE[ulSize];
					ar.Read( ulSize, pBuffer );
					delete[] pBuffer; 
				}

			}	// switch

		} // for

		return;

	} // tagged?

	m_eFillMethod	= EFillMethod( uwFill );

	switch( m_eFillMethod )
		{
		case kTransparent:
			//
			// nothing to do...
			break;
		case kSolid:
			//
			// read the solid color
			ar >> m_rgbFill;
			break;
		case kGradient:
			{
			m_pGradient	= new RGradientFill;
			m_pGradient->Read( ar );
			RGradientRamp*	pRamp	= new RGradientRamp;
			pRamp->Read( ar );
			m_pGradient->SetRamp( pRamp );
			break;
			}
		default:
			TpsAssertAlways( "Read an invalid fill method" );
			break;
		}
}


//
// @MFunc Write this fillcolor to the given storage object
//
// @RDesc nothing
//
void RColor::Write(
			RArchive&	ar )	// @Parm the storage object to write to
			const
{
	uWORD uwFillFormat = kTaggedFormat;

	if (m_eFillMethod <= kGradient)
		uwFillFormat = uWORD( m_eFillMethod );

	//
	// Write the fill format and effect...
	ar	<< uwFillFormat;
	ar	<< uWORD( m_eFillEffect );

	switch( uwFillFormat )
	{
		case kTransparent:
			//
			// nothing to do...
			break;
		case kSolid:
			//
			// read the solid color
			ar << m_rgbFill;
			break;
		case kGradient:
			m_pGradient->Write( ar );
			m_pGradient->GetRamp()->Write( ar );
			break;
		
		case kTaggedFormat:

			// Serialize tag count (currently only 1 supported)
			ar << uWORD( 1 );

			switch (m_eFillMethod)
			{
			case kBitmap:
				ar << uWORD( kBitmap );						// Tag
				ar << m_pImageColor->GetDataLength();	// Data Size
				m_pImageColor->Write( ar );				// Data
				break;

			default:
				TpsAssertAlways( "Invalid fill method!" );
				break;
			}

			break;

		default:
			TpsAssertAlways( "Invalid fill method!" );
			break;
	}
}

//
// @MFunc Set the bounding rectangle for the color (ignored if color is NOT a gradient)
// @RDesc reference to this color
//
RColor& RColor::SetBoundingRect(
			const RIntRect& rcBounds )	// @Parm the new bounding rectangle
	{
	if( GetFillMethod( ) == kGradient )
		m_pGradient->SetBoundingRect( rcBounds );
	else if( GetFillMethod( ) == kBitmap )
		m_pImageColor->SetBoundingRect( rcBounds );
	return *this;
	}

#ifdef TPSDEBUG
#include	"Color.inl"
#endif
