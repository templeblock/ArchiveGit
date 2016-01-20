// ****************************************************************************
//
//  File Name:			HeadlineColor.cpp
//
//  Project:			Headline Component
//
//  Author:				M. Dunlap
//
//  Description:		Declaration of the RHeadlineColor class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineColor.cpp          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include "GradientFill.h"
#include "ImageColor.h"

const YAngle	kCompassAngle[] =
{
	kPI/2,		// kNorth
	kPI/4,		// kNorthEast
	0.0,			// kEast
	-kPI/4,		// kSouthEast
	-kPI/2,		// kSouth
	-3*kPI/2,	// kSouthWest
	kPI,			// kWest
	3*kPI/2,		// kNorthWest
};

RHeadlineColor::RHeadlineColor() :
	m_Color()
{
}


RHeadlineColor::RHeadlineColor( const RSolidColor& color ) :
	m_Color( color )
{
}

RHeadlineColor::RHeadlineColor( const RSolidColor& startColor, const RSolidColor& endColor, EMajorCompassDirections eDirection )
{
	TpsAssert( eDirection>=kNorth && eDirection<=kNorthWest, "Invalid Direction" );
	YAngle		angGradient( kCompassAngle[eDirection-kNorth] );
	RSolidColor	arColors[2];
	arColors[0]	= startColor;
	arColors[1]	= endColor;
	m_Color	= RColor( arColors, sizeof(arColors)/sizeof(arColors[0]), angGradient );
}

RHeadlineColor::RHeadlineColor( const RColor& color ) :
	m_Color( color )
{
}

RHeadlineColor::RHeadlineColor( const RHeadlineColor& rhs ) :
	m_Color( rhs.m_Color )
{
}

const RHeadlineColor& RHeadlineColor::operator=( const RHeadlineColor& rhs )
{
	if( this != &rhs )
		m_Color = rhs.m_Color;
	return *this;
}

BOOLEAN RHeadlineColor::UsesGradient() const
{
	return BOOLEAN(m_Color.GetFillMethod()==RColor::kGradient);
}

RSolidColor RHeadlineColor::GetSolidColor( YPercentage pct ) const
{
	if( m_Color.GetFillMethod()==RColor::kGradient )
		return m_Color.GetGradientFill()->GetRamp()->GetColor( pct );
	return m_Color.GetSolidColor();
}

const RColor& RHeadlineColor::GetColor() const
{
	return m_Color;
}

YAngle	RHeadlineColor::GetAngle() const
{
	return UsesGradient()? m_Color.GetGradientFill()->GetGradientAngle() : 0;
}

void RHeadlineColor::SetAngle( YAngle angle )
{
	if ( UsesGradient() )
	{
		m_Color.m_pGradient->SetGradientAngle( angle );
	}
}

BOOLEAN RHeadlineColor::IsTransparent() const
{
	return BOOLEAN( m_Color.GetFillMethod()==RColor::kTransparent );
}

BOOLEAN RHeadlineColor::IsTranslucent() const
{
	return BOOLEAN( m_Color.GetFillEffect()==RColor::kTranslucent );
}

BOOLEAN RHeadlineColor::IsBitmap() const
{
	return BOOLEAN( m_Color.GetFillMethod()==RColor::kBitmap );
}

void RHeadlineColor::SetBoundingRect( const RIntRect& rcBounds )
{
	m_Color.SetBoundingRect( rcBounds );
}

void RHeadlineColor::InvertBaseTransform()
{
	if (IsBitmap())
	{
		RImageColor* pImageColor = m_Color.m_pImageColor;
		R2dTransform xform( pImageColor->GetBaseTransform() );
		xform.PreScale( 1, -1 );

		pImageColor->SetBaseTransform( xform );
	}
}

void RHeadlineColor::Read( RArchive& ar )
{
	ar	>> m_Color;
}

void RHeadlineColor::Write( RArchive& ar ) const
{
	ar	<< m_Color;
}

#ifdef TPSDEBUG
BOOLEAN RHeadlineColor::IsValid() const
{
	//
	// currently no way to validate color, so we assume it is...
	return TRUE;
}
#endif // defined(TPSDEBUG)
