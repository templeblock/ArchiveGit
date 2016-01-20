// ****************************************************************************
//
//  File Name:			HeadlineColor.h
//
//  Project:			Headine Component
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineColor.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINECOLOR_H_
#define		_HEADLINECOLOR_H_
//
// enums

enum EMajorCompassDirections
{
	kNorth,
	kNorthEast,
	kEast,
	kSouthEast,
	kSouth,
	kSouthWest,
	kWest,
	kNorthWest
};

class RHeadlineColor
{
//
// Data
private:
	RColor						m_Color;
//
// Construction
public:
	RHeadlineColor();
	RHeadlineColor( const RSolidColor& color );
	RHeadlineColor( const RSolidColor& startcolor, const RSolidColor& endcolor, EMajorCompassDirections eDirection );
	RHeadlineColor( const RColor& color );
	RHeadlineColor( const RHeadlineColor& rhs );
	const RHeadlineColor& operator=( const RHeadlineColor& rhs );
//
// Accessors
public:
	BOOLEAN			UsesGradient() const;
	RSolidColor		GetSolidColor( YPercentage pct=0.0 ) const;
	const RColor&	GetColor() const;
	YAngle			GetAngle() const;
	void				SetAngle( YAngle angle );
	BOOLEAN			IsTransparent() const;
	BOOLEAN			IsTranslucent() const;
	BOOLEAN			IsBitmap() const;
//
// Operations
public:
	void				SetBoundingRect( const RIntRect& rcBounds );
	void				InvertBaseTransform();

//
// Serialization
public:
	void				Read( RArchive& ar );
	void				Write( RArchive& ar ) const;
#ifdef TPSDEBUG
//
// Debug support
public:
	BOOLEAN IsValid() const;
#endif
};

//
// Inlines...
//

//
// Serialization read helper
inline RArchive& operator>>( RArchive& ar, RHeadlineColor& hclr )
{
	hclr.Read( ar );
	return ar;
}

//
// Serialization write helper
inline RArchive& operator<<( RArchive& ar, const RHeadlineColor& hclr )
{
	hclr.Write( ar );
	return ar;
}

#endif // !defined(_HEADLINECOLOR_H_)
