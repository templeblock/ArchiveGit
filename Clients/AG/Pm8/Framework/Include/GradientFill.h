// ****************************************************************************
//
//  File Name:			GradientFill.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Dunlap
//
//  Description:		Declaration of the RGradientFill class
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
//  $Logfile:: /PM8/Framework/Include/GradientFill.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GRADIENTFILL_H_
#define		_GRADIENTFILL_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDrawingSurface;

// ****************************************************************************
//
// @Class Class containing gradient ramp point data
class FrameworkLinkage RGradientRampPoint
{
//
// @Access Data
public:
	//
	// @CMember the color at this ramp point
	RSolidColor	m_color;
	//
	// @CMember the point between this ramp point and the next where the colors are an even mix
	YPercentage	m_pctMidPoint;
	//
	// @CMember the point on the ramp that this ramppoint is located at
	YPercentage	m_pctRampPoint;

//
// @Access Construction
public:
	//
	// @CMember default ctor
	RGradientRampPoint();
	//
	// @CMember ctor with data
	RGradientRampPoint( const RSolidColor& clr, YPercentage midpt, YPercentage ramppt );
//
// @Access Operator overloads
public:
	//
	// @CMember test for equality
	BOOLEAN operator==( const RGradientRampPoint& rhs ) const;
	//
	// @CMember test for inequality
	BOOLEAN operator!=( const RGradientRampPoint& rhs ) const;

//
// @Access Object serialization
	//
	// @CMember Read
	void						Read( RArchive& archive );
	//
	// @CMember Write
	void						Write( RArchive& archive ) const;
};


// ****************************************************************************
//
// @Type YGradientRampPointArray | <c RArray> of <c RampPoint>s specifying a gradient ramp
typedef RArray<RGradientRampPoint>	YGradientRampPointArray;


// ****************************************************************************
//
// @Type YGradientRampPointArrayIterator | Iterates through a ramp point array
typedef YGradientRampPointArray::YIterator	YGradientRampPointArrayIterator;

//
// @Enum Gradient types
enum EGradientType
{
	kUndefined,			// @EMem used by the default constructor
	kCircularBurst,	// @EMem Circular burst with center at m_ptVectorStart, outer ramp point at m_ptVectorEnd
	kSquareBurst,		// @EMem Square burst with center at m_ptVectorStart, outer ramp point at m_ptVectorEnd
	kLinear,				// @EMem Linear gradient starting at m_ptVectorStart, ending at m_ptVectorEnd
	kRadial				// @EMem Counter-clockwise radial burst with ramp point 0.0 at vector from m_ptVectorStart to m_ptVectorEnd
};


//	Parameter block for gradient creation.
class FrameworkLinkage RGradientParams
	{
	public:
		RGradientParams( );
		RGradientParams(			EGradientType			eType,
										const RIntPoint&		ptOrigin,
										YAngle					angVector,
										YRealDimension			rdlenVector,
										YPercentage				pctposHilite,
										YAngle					angHilite );

//
// @Access Object serialization
	//
	// @CMember Read
	void								Read( RArchive& archive );
	//
	// @CMember Write
	void								Write( RArchive& archive ) const;

		EGradientType				m_eType;
		RIntPoint					m_ptOrigin;
		YAngle						m_angVector;
		YRealDimension				m_rdlenVector;
		YPercentage					m_pctposHilite;
		YAngle						m_angHilite;
	};


// ****************************************************************************
//
// @Class Class containing the gradient color ramp definition
class FrameworkLinkage RGradientRamp
{
//
// @Access Gradient Ramp Data
protected:
	//
	// @CMember Ramp type
	EGradientType				m_ePreferredType;
	//
	// @CMember Array of ramp points
	YGradientRampPointArray	m_arRampPoints;

//
// @Access Construction
public:
	//
	// @CMember Default ctor
	RGradientRamp();
	//
	// @CMember ctor with static array
	RGradientRamp( EGradientType ePreferredType, RGradientRampPoint* pRampPoints, int nRampPoints );
	//
	// @CMember add a ramp point to this ramp
	void	Add( RGradientRampPoint& ramppt );
	//
	// @CMember remove all ramp points from this ramp
	void	RemoveAllPts( );
	//
	// @CMember change the preferred type of the ramp
	void	SetType( EGradientType eType );

//
// @Access operator overloads
public:
	//
	// @CMember assignment
	const RGradientRamp& operator=( const RGradientRamp& rhs );
	//
	// @CMember test for equality
	BOOLEAN operator==( const RGradientRamp& rhs ) const;
	//
	// @CMember test for inequality
	BOOLEAN operator!=( const RGradientRamp& rhs ) const;
//
// @Access Object serialization
public:
	//
	// @CMember Read
	void						Read( RArchive& archive );
	//
	// @CMember Write
	void						Write( RArchive& archive ) const;

//
// @Access Data Retrieval
public:
	//
	// @CMember Get the preferred type of the ramp
	EGradientType			GetType() const;
	//
	// @CMember Get the number of ramp points in this gradient ramp
	int						GetRampPointCount() const;
	//
	// @CMember Get a copy of the specified ramp point
	RGradientRampPoint	GetRampPoint( int iRampPoint ) const;
	//
	// @CMember Get the color at the specified location on the ramp
	RSolidColor				GetColor( YPercentage pctRampPos, YPercentage pctPrecision=1.0 ) const;
};



// ****************************************************************************
//
// @Class Class containing the gradient fill geometry
class FrameworkLinkage RGradientFill : public RObject
{
//
// @Access Gradient Data
protected:
	//
	// @CMember Type of gradient cycling
	EGradientType			m_eGradientType;
	//
	// @CMember Gradient vector origin
	RIntPoint				m_ptGradientOrigin;
	//
	// @CMember Gradient vector angle
	YAngle					m_angGradientVector;
	//
	// @CMember Gradient vector length
	YRealDimension			m_rdlenGradientVector;
	//
	// @CMember Base transform for this gradient
	R2dTransform			m_BaseTransform;
	//
	// @CMember Final transform for this gradient
	R2dTransform			m_Transform;
	//
	// @CMember Position along hilite vector of hilite origin
	YPercentage				m_pctposHilite;
	//
	// @CMember Angle of hilite vector (vector from gradient origin)
	YAngle					m_angHilite;
	//
	// @CMember Color ramp specification
	const RGradientRamp*	m_pRamp;

//
// @Access Construction
public:
	//
	// @CMember Default ctor
	RGradientFill();
	//
	// @CMember Copy ctor
	RGradientFill( const RGradientFill& r );
	//
	// @CMember ctor with gradient params
	RGradientFill( const RGradientParams& params );
	//
	// @CMember ctor with full data
	RGradientFill( EGradientType			eType,
						const RGradientRamp* pRamp,
						const RIntPoint&		ptOrigin,
						YAngle					angVector,
						YRealDimension			rdlenVector,
						const R2dTransform&	basetransform,
						YPercentage				pctposHilite,
						YAngle					angHilite );
	//
	// @CMember Default dtor
	~RGradientFill();

//
// @Access operator overloads
public:
	//
	// @CMember assignment
	const RGradientFill& operator=( const RGradientFill& rhs );
	//
	// @CMember test for equality
	BOOLEAN operator==( const RGradientFill& rhs ) const;
	//
	// @CMember test for inequality
	BOOLEAN operator!=( const RGradientFill& rhs ) const;

//
// @Access Object serialization
	//
	// @CMember Read
	void						Read( RArchive& archive );
	//
	// @CMember Write
	void						Write( RArchive& archive ) const;

//
// @Access Gradient Data Manipulation
public:
	//
	// @CMember Set the gradient ramp
	void						SetRamp( const RGradientRamp* pRamp );
	//
	// @CMember Get a pointer to the gradient ramp
	const RGradientRamp*	GetRamp() const;
	//
	// @CMember Set the gradient type
	void						SetType( EGradientType eType );
	//
	// @CMember Get the gradient type
	EGradientType			GetType() const;
	//
	// @CMember Set the gradient vector origin
	void						SetGradientOrigin( const RIntPoint& ptOrigin );
	//
	// @CMember Get the gradient vector origin
	const RIntPoint&		GetGradientOrigin() const;
	//
	// @CMember Set the gradient vector angle
	void						SetGradientAngle( YAngle angGradient );
	//
	// @CMember Get the gradient vector angle
	YAngle					GetGradientAngle() const;
	//
	// @CMember Set the gradient vector length
	void						SetGradientLength( YRealDimension rdlenGradientVector );
	//
	// @CMember Get the gradient vector length
	YRealDimension			GetGradientLength() const;
	//
	// @CMember Set the gradients transformation matrix
	void						SetBaseTransform( const R2dTransform& basetransform );
	//
	// @CMember Get the gradients transformation matrix
	const R2dTransform&	GetBaseTransform() const;
	//
	// @CMember Apply an additional transformation to this gradient
	void						ApplyTransform( const R2dTransform& transform );
	//
	// @CMember Get the gradients output transformation matrix
	const R2dTransform&	GetOutputTransform() const;
	//
	// @CMember Set the gradients hilite position (percent position along vector)
	void						SetHilitePosition( YPercentage pctposHilite );
	//
	// @CMember Get the gradients hilite position (percent position along vector)
	YPercentage				GetHilitePosition() const;
	//
	// @CMember Get the point corresponding to the gradients hilite position
	RRealSize				GetHiliteOffset( YPercentage pctPos = 1.0 ) const;
	//
	// @CMember Set the hilite angle
	void						SetHiliteAngle( YAngle angHilite );
	//
	// @CMember Get the hilite angle
	YAngle					GetHiliteAngle() const;
	//
	// @CMember Set the target bounding rect for the gradient (resets params)
	void						SetBoundingRect( const RIntRect& rcBounds );

//
// @Access Surface Creation
public:
	//
	// @CMember Draw the gradient within the given bounds
	void	DrawGradient( RDrawingSurface& ds, const RIntRect& rcBounds, const R2dTransform& transform, YPercentage pctPrecision=1.0 ) const;
	//
	// @CMember Draw the gradient within the given edge list
	void	DrawGradient( RDrawingSurface& ds, const RIntRect& rcBounds, const RIntPoint* pptClip, const YPointCount* pcntClip, YPolygonCount nClip, const R2dTransform& transform, YPercentage pctPrecision=1.0 ) const;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
};


// ****************************************************************************
// 					Inlines
// ****************************************************************************


//****************************************************************************
//
// RGradientParams::RGradientParams
//
// @MFunc default ctor
//
// @RDesc nothing
//
inline RGradientParams::RGradientParams(
										EGradientType			eType,
										const RIntPoint&		ptOrigin,
										YAngle					angVector,
										YRealDimension			rdlenVector,
										YPercentage				pctposHilite,
										YAngle					angHilite ):
	m_eType( eType ), m_ptOrigin( ptOrigin ), m_angVector( angVector ), m_rdlenVector( rdlenVector )
		, m_pctposHilite( pctposHilite ), m_angHilite( angHilite )
	{
	NULL;
	}


//****************************************************************************
//
// RGradientParams::RGradientParams
//
// @MFunc default ctor
//
// @RDesc nothing
//
inline RGradientParams::RGradientParams( ):
	m_eType( kUndefined ), m_ptOrigin( 0,0 ), m_angVector( 0.0 ), m_rdlenVector( 0.0 )
		, m_pctposHilite( 0.0 ), m_angHilite( 0.0 )
	{
	NULL;
	}


//****************************************************************************
//
// RGradientRampPoint::RGradientRampPoint
//
// @MFunc default ctor
//
// @RDesc nothing
//
inline RGradientRampPoint::RGradientRampPoint() :
	m_color( kBlack ),
	m_pctMidPoint( YPercentage(0.5) ),
	m_pctRampPoint( YPercentage(0.0) )
	{
	}

//****************************************************************************
//
// RGradientRampPoint::RGradientRampPoint
//
// @MFunc ctor with data
//
// @RDesc nothing
//
inline RGradientRampPoint::RGradientRampPoint( const RSolidColor& clr, YPercentage midpt, YPercentage ramppt ) :
		m_color( clr ),
		m_pctMidPoint( midpt ),
		m_pctRampPoint( ramppt )
	{
	}

//****************************************************************************
//
// RGradientRampPoint::operator!=
//
// @MFunc test for inequality
//
// @RDesc TRUE if they are different else FALSE
//
inline BOOLEAN RGradientRampPoint::operator!=( 
			const RGradientRampPoint&	rhs )	// @Parm reference to the ramp point to compare this one with
			const
	{
	return BOOLEAN(!operator==(rhs));
	}

//****************************************************************************
//
// int RGradientRamp::GetRampPointCount() const
//
// @MFunc Get the number of ramp points in this ramp
//
// @RDesc the number of ramp points
//
inline int RGradientRamp::GetRampPointCount() const
	{
	return m_arRampPoints.Count();
	}

//****************************************************************************
//
// RGradientRampPoint RGradientRamp::GetRampPoint( int iRampPoint ) const
//
// @MFunc Get a copy of the given ramp point
//
// @RDesc a copy of the given ramp point
//
inline RGradientRampPoint RGradientRamp::GetRampPoint( int iRampPoint ) const
	{
	return m_arRampPoints.Start()[iRampPoint];
	}

//****************************************************************************
//
// RGradientRamp::GetType() const
//
// @MFunc get the preferred type of the ramp
//
// @RDesc the preferred type for this ramp
//
inline EGradientType RGradientRamp::GetType() const
	{
	return m_ePreferredType;
	}

//****************************************************************************
//
// RGradientRamp::SetType( EGradientType eType )
//
// @MFunc change the preferred type of the ramp
//
// @RDesc nothing
//
inline void RGradientRamp::SetType( EGradientType eType )
	{
	m_ePreferredType	= eType;
	}

//****************************************************************************
//
// RGradientRamp::operator!=
//
// @MFunc test for inequality
//
// @RDesc TRUE if they are different else FALSE
//
inline BOOLEAN RGradientRamp::operator!=( 
			const RGradientRamp&	rhs )	// @Parm reference to the ramp to compare this one with
			const
	{
	return BOOLEAN(!operator==(rhs));
	}

//****************************************************************************
//
// void RGradientFill::SetRamp( const RGradientRamp* pRamp )
//
// @MFunc Set the color ramp for this gradient
//
// @RDesc nothing
//
inline void RGradientFill::SetRamp(
		const RGradientRamp* pRamp )	// @Parm pointer to the color ramp to be used by this gradient
	{
	m_pRamp	= pRamp;
	}

//****************************************************************************
//
// const RGradientRamp* RGradientFill::GetRamp() const
//
// @MFunc Get the color ramp for this gradient
//
// @RDesc a pointer to the gradient color ramp
//
inline const RGradientRamp* RGradientFill::GetRamp() const
	{
	return m_pRamp;
	}

//****************************************************************************
//
// void RGradientFill::SetType( EGradientType eType )
//
// @MFunc Set this gradients type
//
// @RDesc nothing
//
inline void RGradientFill::SetType(
		EGradientType eType )	// @Parm the gradient type
	{
	m_eGradientType	= eType;
	}

//****************************************************************************
//
// EGradientType RGradientFill::GetType() const
//
// @MFunc Get this gradients type
//
// @RDesc the type of gradient
//
inline EGradientType RGradientFill::GetType() const
	{
	return m_eGradientType;
	}

//****************************************************************************
//
// void RGradientFill::SetGradientOrigin( const RIntPoint& ptOrigin )
//
// @MFunc Set the gradient vector origin
//
// @RDesc nothing
//
inline void RGradientFill::SetGradientOrigin(
		const RIntPoint& ptOrigin )	// @Parm the origin for this gradient
	{
	m_ptGradientOrigin	= ptOrigin;
	}

//****************************************************************************
//
// const RIntPoint& RGradientFill::GetGradientOrigin() const
//
// @MFunc Get the gradient vector origin
//
// @RDesc the gradient vector origin
//
inline const RIntPoint& RGradientFill::GetGradientOrigin() const
	{
	return m_ptGradientOrigin;
	}

//****************************************************************************
//
// void RGradientFill::SetGradientAngle( YAngle angGradient )
//
// @MFunc Set the gradient vector angle
//
// @RDesc nothing
//
inline void RGradientFill::SetGradientAngle(
		YAngle angGradient )	// @Parm the angle for the gradient vector
	{
	m_angGradientVector	= angGradient;
	}

//****************************************************************************
//
// YAngle RGradientFill::GetGradientAngle() const
//
// @MFunc Get the gradient vector angle
//
// @RDesc the gradient vector angle
//
inline YAngle RGradientFill::GetGradientAngle() const
	{
	return m_angGradientVector;
	}

//****************************************************************************
//
// void RGradientFill::SetGradientLength( YRealDimension rdlenGradient )
//
// @MFunc Set the gradient vector length
//
// @RDesc nothing
//
inline void RGradientFill::SetGradientLength(
		YRealDimension rdlenGradient )	// @Parm the length of the gradient vector
	{
	m_rdlenGradientVector	= rdlenGradient;
	}

//****************************************************************************
//
// YRealDimension RGradientFill::GetGradientLength() const
//
// @MFunc Get the gradient vector length
//
// @RDesc the gradient vector length
//
inline YRealDimension RGradientFill::GetGradientLength() const
	{
	return m_rdlenGradientVector;
	}

//****************************************************************************
//
// void RGradientFill::SetHilitePosition( YPercentage pctposHilite )
//
// @MFunc Set the position of the hilite as a percentage of the vector length
//
// @RDesc nothing
//
inline void RGradientFill::SetHilitePosition(
		YPercentage pctposHilite )	// @Parm the position of the hilite center
	{
	m_pctposHilite	= pctposHilite;
	}

//****************************************************************************
//
// YPercentage RGradientFill::GetHilitePosition() const
//
// @MFunc Get the hilite position as a percentage of the vector length
//
// @RDesc the hilite position
//
inline YPercentage RGradientFill::GetHilitePosition() const
	{
	return m_pctposHilite;
	}

//****************************************************************************
//
// void RGradientFill::SetHiliteAngle( YAngle angHilite )
//
// @MFunc Set the angle of the hilite vector
//
// @RDesc nothing
//
inline void RGradientFill::SetHiliteAngle(
		YAngle angHilite )	// @Parm the angle of the hilite vector
	{
	m_angHilite	= angHilite;
	}

//****************************************************************************
//
// YAngle RGradientFill::GetHiliteAngle() const
//
// @MFunc Get the angle of the hilite vector
//
// @RDesc the hilite vector angle
//
inline YAngle RGradientFill::GetHiliteAngle() const
	{
	return m_angHilite;
	}

//****************************************************************************
//
// void RGradientFill::SetBaseTransform( const R2dTransform& basetransform )
//
// @MFunc Set the gradient's base transformation matrix
//
// @RDesc nothing
//
inline void RGradientFill::SetBaseTransform(
		const R2dTransform& basetransform )	// @Parm the transformation matrix
	{
	m_BaseTransform	= basetransform;
	m_Transform			= basetransform;
	}

//****************************************************************************
//
// const R2dTransform& RGradientFill::GetBaseTransform() const
//
// @MFunc Get the base transformation matrix
//
// @RDesc the base transformation matrix
//
inline const R2dTransform& RGradientFill::GetBaseTransform() const
	{
	return m_BaseTransform;
	}

//****************************************************************************
//
// void RGradientFill::ApplyTransform( const R2dTransform& transform )
//
// @MFunc Apply the given transformation to the current gradient transformation
//
// @RDesc nothing
//
inline void RGradientFill::ApplyTransform(
		const R2dTransform& transform )	// @Parm the transformation matrix
	{
	m_Transform	= m_BaseTransform * transform;
	}

//****************************************************************************
//
// const R2dTransform& RGradientFill::GetOutputTransform() const
//
// @MFunc Get the output transformation matrix for this gradient
//
// @RDesc nothing
//
inline const R2dTransform& RGradientFill::GetOutputTransform() const
	{
	return m_Transform;
	}

//****************************************************************************
//
// RRealSize RGradientFill::GetHiliteOffset() const
//
// @MFunc Get the offset from the gradient origin of the hilite center
//
// @RDesc the hilite offset
//
inline RRealSize RGradientFill::GetHiliteOffset(
		YPercentage pctpos ) const	// @Parm percent of hilite vector to get position at
	{
	R2dTransform	xform;
	RRealSize		szOffset( m_rdlenGradientVector*m_pctposHilite*pctpos, 0.0 );
	xform.PreRotateAboutOrigin( m_angHilite );
	return szOffset * xform;
	}

//****************************************************************************
//
// RGradientFill::operator!=
//
// @MFunc test for inequality
//
// @RDesc TRUE if they are different else FALSE
//
inline BOOLEAN RGradientFill::operator!=( 
			const RGradientFill&	rhs )	// @Parm reference to the gradient to compare this one with
			const
	{
	return BOOLEAN(!operator==(rhs));
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _GRADIENTFILL_H_
