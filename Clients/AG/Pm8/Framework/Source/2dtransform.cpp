// ****************************************************************************
//
//  File Name:			2dTransform.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the R2dTransform class
//
//							NOTE: This class assumes the use of row vectors
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
//  $Logfile:: /PM8/Framework/Source/2dtransform.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	R2dTransform::R2dTransform( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
R2dTransform::R2dTransform( )
	{
	MakeIdentity( );
	}

// ****************************************************************************
//
//  Function Name:	R2dTransform::R2dTransform( )
//
//  Description:		Copy constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
R2dTransform::R2dTransform( const R2dTransform &rhs )
	: m_a1( rhs.m_a1 ),
	  m_a2( rhs.m_a2 ),
	  m_b1( rhs.m_b1 ),
	  m_b2( rhs.m_b2 ),
	  m_c1( rhs.m_c1 ),
	  m_c2( rhs.m_c2 )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	R2dTransform::R2dTransform( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
R2dTransform::R2dTransform( YRealDimension a1, YRealDimension a2,
									 YRealDimension b1, YRealDimension b2,
									 YRealDimension c1, YRealDimension c2 )
	: m_a1( a1 ),
	  m_a2( a2 ),
	  m_b1( b1 ),
	  m_b2( b2 ),
	  m_c1( c1 ),
	  m_c2( c2 )
	{
	;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::MakeIdentity( )
//
// Description:		Makes the transform the identity matrix
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::MakeIdentity( )
	{
	m_a1 = 1.0; m_a2 = 0.0;
	m_b1 = 0.0;	m_b2 = 1.0;
	m_c1 = 0.0; m_c2 = 0.0;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::Invert( )
//
// Description:		Inverts this matrix. The inverse of a matrix is a matrix
//							T' such that T * T' = I
//
//									|	b2 / D						-a2 / D						0 |
//							T' =	| -b1 / D						 a1 / D						0 |
//									| -( -b1c2 + b2c1 ) / D		 ( -a1c2 + a2c1 ) / D	1 |
//
//							where D = a1b2 - b1a2
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::Invert( )
	{
	YFloatType D = ( m_a1 * m_b2 ) - ( m_b1 * m_a2 );

	YFloatType temp = m_c1;

	m_c1 = ( - ( ( -m_b1 * m_c2 ) + ( m_b2 * m_c1 ) ) ) / D;
	m_c2 = ( ( -m_a1 * m_c2 ) + ( m_a2 * temp ) ) / D;

	temp = m_a1;
	m_a1 = m_b2 / D;
	m_b2 = temp / D;

	m_a2 /= -D;
	m_b1 /= -D;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreTranslate( )
//
// Description:		Pre-multiplies this transform by a translation
//
//							The following algorithm is used.
//
//	|	1	0	0	|		|	a1	a2	0	|		|	a1							a2							0	|
//	|	0	1	0	|	*	|	b1	b2	0	|	=	|	b1							b2							0	|
//	|	tx	ty	1	|		|	c1	c2	1	|		|	a1*tx + b1*ty + c1	a2*tx + b2*ty + c2	1	|
//								              
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreTranslate( YRealDimension tx, YRealDimension ty )
	{
	m_c1 += ( m_a1 * tx ) + ( m_b1 * ty );
	m_c2 += ( m_a2 * tx ) + ( m_b2 * ty );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostTranslate( )
//
// Description:		Post-multiplies this transform by a translation
//
//							The following algorithm is used.
//
//		|	a1	a2	0	|		|	1	0	0	|		|	a1				a2			0	|
//		|	b1	b2	0	|	*	|	0	1	0	|	=	|	b1				b2			0	|
//		|	c1	c2	1	|		|	tx	ty	1	|		|	c1 + tx		c2 + ty	1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostTranslate( YRealDimension tx, YRealDimension ty )
	{
	m_c1 += tx;
	m_c2 += ty;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreRotateAboutOrigin( )
//
// Description:		Pre-multiplies this transform by a rotation about the
//							origin.
//
//							angle specifies a counterclockwise rotation, in radians.
//
//							The following algorithm is used.
//
//	|	c	-s	0	|		|	a1	a2	0	|		|	 c*a1 - s*b1	 c*a2 - s*b2	0	|
//	|	s	c	0	|	*	|	b1	b2	0	|	=	|	 s*a1 + c*b1	 s*a2 + c*b2	0	|
//	|	0	0	1	|		|	c1	c2	1 	|		|	 c1				 c2				1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreRotateAboutOrigin( YAngle angle )
	{
	YFloatType sinAngle = sin( angle );
	YFloatType cosAngle = cos( angle );

	YRealCoordinate temp;

	temp = ( cosAngle * m_a1 ) - ( sinAngle * m_b1 );
	m_b1 = ( sinAngle * m_a1 ) + ( cosAngle * m_b1 );
	m_a1 = temp;

	temp = ( cosAngle * m_a2 ) - ( sinAngle * m_b2 );
	m_b2 = ( sinAngle * m_a2 ) + ( cosAngle * m_b2 );
	m_a2 = temp;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostRotateAboutOrigin( )
//
// Description:		Post-multiplies this transform by a rotation about the
//							origin
//
//							angle specifies a counterclockwise rotation, in radians.
//
//							The following algorithm is used.
//
//	|	a1	a2	0	|		|	c	-s	0	|		|	a1*c + a2*s		-a1*s + a2*c		0	|
//	|	b1	b2	0	|	*	|	s	c	0	|	=	|	b1*c + b2*s		-b1*s + b2*c		0	|
//	|	c1	c2	1 	|		|	0	0	1	|		|	c1*c + c2*s		-c1*s + c2*c		1	|
//																			
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostRotateAboutOrigin( YAngle angle )
	{
	YFloatType sinAngle = sin( angle );
	YFloatType cosAngle = cos( angle );

	YRealCoordinate temp;

	temp = ( m_a1 * cosAngle ) + ( m_a2 * sinAngle );
	m_a2 = ( -m_a1 * sinAngle ) + ( m_a2 * cosAngle );
	m_a1 = temp;

	temp = ( m_b1 * cosAngle ) + ( m_b2 * sinAngle );
	m_b2 = ( -m_b1 * sinAngle ) + ( m_b2 * cosAngle );
	m_b1 = temp;

	temp = ( m_c1 * cosAngle ) + ( m_c2 * sinAngle );
	m_c2 = ( -m_c1 * sinAngle ) + ( m_c2 * cosAngle );
	m_c1 = temp;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreRotateAboutPoint( )
//
// Description:		Pre-multiplies this transform by a rotation about an
//							arbitrary point
//
//							angle specifies a counterclockwise rotation, in radians.
//
//							A rotation about a point is a translation of that point
//							to the origin, a rotation about the origin, and a
//							translation back.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreRotateAboutPoint( YRealCoordinate x, YRealCoordinate y, YAngle angle )
	{
	PreTranslate( x, y );
	PreRotateAboutOrigin( angle );
	PreTranslate( -x, -y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostRotateAboutPoint( )
//
// Description:		Post-multiplies this transform by a rotation about an
//							arbitrary point
//
//							angle specifies a counterclockwise rotation, in radians.
//
//							A rotation about a point is a translation of that point
//							to the origin, a rotation about the origin, and a
//							translation back.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostRotateAboutPoint( YRealCoordinate x, YRealCoordinate y, YAngle angle )
	{
	PostTranslate( -x, -y );
	PostRotateAboutOrigin( angle );
	PostTranslate( x, y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreScale( )
//
// Description:		Pre-multiplies this transform by a scale operation
//
//							The following algorithm is used.
//
//	|	sx	0	0	|		|	a1	a2	0	|		|	sx*a1		sx*a2		0	|
//	|	0	sy	0	|	*	|	b1	b2	0	|	=	|	sy*b1		sy*b2		0	|
//	|	0	0	1	|		|	c1	c2	1	|		|	c1			c2			1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreScale( YScaleFactor sx, YScaleFactor sy )
	{
	m_a1 *= sx;
	m_b1 *= sy;

	m_a2 *= sx;
	m_b2 *= sy;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostScale( )
//
// Description:		Post-multiplies this transform by a scale operation
//
//							The following algorithm is used.
//
//	|	a1	a2	0	|		|	sx	0	0	|		|	sx*a1		sy*a2		0	|
//	|	b1	b2	0	|	*	|	0	sy	0	|	=	|	sx*b1		sy*b2		0	|
//	|	c1	c2	1	|		|	0	0	1	|		|	sx*c1		sy*c2		1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostScale( YScaleFactor sx, YScaleFactor sy )
	{
	m_a1 *= sx;
	m_a2 *= sy;

	m_b1 *= sx;
	m_b2 *= sy;

	m_c1 *= sx;
	m_c2 *= sy;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreScaleAboutPoint( )
//
// Description:		Pre-multiplies this transform by a scale about point op.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreScaleAboutPoint( YRealCoordinate x, YRealCoordinate y, YScaleFactor sx, YScaleFactor sy )
	{
	PreTranslate( x, y );
	PreScale( sx, sy );
	PreTranslate( -x, -y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostScaleAboutPoint( )
//
// Description:		Post-multiplies this transform by a scale about point op.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostScaleAboutPoint( YRealCoordinate x, YRealCoordinate y, YScaleFactor sx, YScaleFactor sy )
	{
	PostTranslate( -x, -y );
	PostScale( sx, sy );
	PostTranslate( x, y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreYShear( )
//
// Description:		Pre-Shear a transform along the Y Axis
//
//							The following algorithm is used.
//
//	|	1	u	0	|		|	a1	a2	0	|		|	a1 + b1*u	a2 + b2*u	0	|
//	|	0	1	0	|	*	|	b1	b2	0	|	=	|	b1				b2				0	|
//	|	0	0	1	|		|	c1	c2	1	|		|	c1				c2				1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreYShear( YRealDimension u )
	{
	m_a1	+=	m_b1 * u;
	m_a2	+= m_b2 * u;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostYShear( )
//
// Description:		Post-Shear a transform along the Y Axis
//
//							The following algorithm is used.
//
//	|	a1	a2	0	|		|	1	u	0	|		|	a1		a1*u + a2	0	|
//	|	b1	b2	0	|	*	|	0	1	0	|	=	|	b1		b1*u + b2	0	|
//	|	c1	c2	1	|		|	0	0	1	|		|	c1		c2				1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostYShear( YRealDimension u )
	{
	m_a2	+=	m_a1 * u;
	m_b2	+= m_b1 * u;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreYShearAboutPoint( )
//
// Description:		Pre-Shear a transform along the Y Axis about a point
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreYShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u )
	{
	PreTranslate( x, y );
	PreYShear( u );
	PreTranslate( -x, -y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostYShearAboutPoint( )
//
// Description:		Post-Shear a transform along the Y Axis about a point
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostYShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u )
	{
	PostTranslate( x, y );
	PostYShear( u );
	PostTranslate( -x, -y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreXShear( )
//
// Description:		Pre-Shear a transform along the X Axis
//
//							The following algorithm is used.
//
//	|	1	0	0	|		|	a1	a2	0	|		|	a1				a2				0	|
//	|	u	1	0	|	*	|	b1	b2	0	|	=	|	a1*u + b1	a2*u + b2	0	|
//	|	0	0	1	|		|	c1	c2	1	|		|	c1				c2				1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreXShear( YRealDimension u )
	{
	m_b1	+= m_a1 * u;
	m_b2	+= m_a2 * u;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostXShear( )
//
// Description:		Post-Shear a transform along the X Axis
//
//							The following algorithm is used.
//
//	|	a1	a2	0	|		|	1	u	0	|		|	a1 + a2*u	a2		0	|
//	|	b1	b2	0	|	*	|	0	1	0	|	=	|	b1 + b2*u	b2		0	|
//	|	c1	c2	1	|		|	0	0	1	|		|	c1				c2		1	|
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostXShear( YRealDimension u )
	{
	m_a1	+= m_a2 * u;
	m_b1	+= m_b2 * u;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PreXShearAboutPoint( )
//
// Description:		Pre-Shear a transform along the Y Axis about a point
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PreXShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u )
	{
	PreTranslate( x, y );
	PreXShear( u );
	PreTranslate( -x, -y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::PostXShearAboutPoint( )
//
// Description:		Post-Shear a transform along the X Axis about a point
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::PostXShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u )
	{
	PostTranslate( x, y );
	PostXShear( u );
	PostTranslate( -x, -y );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::operator*( )
//
// Description:		Multiplies one transform by another
//
// Returns:				The product transform
//
// Exceptions:			None
//
// ****************************************************************************
//
R2dTransform R2dTransform::operator*( const R2dTransform& rhs ) const
	{
	R2dTransform temp;

	temp.m_a1 = ( m_a1 * rhs.m_a1 ) + ( m_a2 * rhs.m_b1 );
	temp.m_b1 = ( m_b1 * rhs.m_a1 ) + ( m_b2 * rhs.m_b1 );
	temp.m_c1 = ( m_c1 * rhs.m_a1 ) + ( m_c2 * rhs.m_b1 ) + rhs.m_c1;

	temp.m_a2 = ( m_a1 * rhs.m_a2 ) + ( m_a2 * rhs.m_b2 );
	temp.m_b2 = ( m_b1 * rhs.m_a2 ) + ( m_b2 * rhs.m_b2 );
	temp.m_c2 = ( m_c1 * rhs.m_a2 ) + ( m_c2 * rhs.m_b2 ) + rhs.m_c2;
	return temp;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::operator*=( )
//
// Description:		Multiplies this tranform by another
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::operator*=( const R2dTransform& rhs )
	{
	*this = *this * rhs;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::operator=( )
//
// Description:		Assigns one transform to another
//
// Returns:				Reference to this transform
//
// Exceptions:			None
//
// ****************************************************************************
//
R2dTransform& R2dTransform::operator=( const R2dTransform& rhs )
	{
	m_a1 = rhs.m_a1;
	m_a2 = rhs.m_a2;

	m_b1 = rhs.m_b1;
	m_b2 = rhs.m_b2;

	m_c1 = rhs.m_c1;
	m_c2 = rhs.m_c2;

	return *this;
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::operator==( )
//
// Description:		Test for equality
//
// Returns:				TRUE if transforms are equal
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN R2dTransform::operator==( const R2dTransform& rhs ) const
	{
	return static_cast<BOOLEAN>( AreFloatsEqual( m_a1, rhs.m_a1, kTransformDelta ) &&
										  AreFloatsEqual( m_a2, rhs.m_a2, kTransformDelta ) &&
										  AreFloatsEqual( m_b1, rhs.m_b1, kTransformDelta ) &&
										  AreFloatsEqual( m_b2, rhs.m_b2, kTransformDelta ) &&
										  AreFloatsEqual( m_c1, rhs.m_c1 ) &&
										  AreFloatsEqual( m_c2, rhs.m_c2 ) );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::operator!=( )
//
// Description:		Test for inequality
//
// Returns:				TRUE if transforms are not equal
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN R2dTransform::operator!=( const R2dTransform& rhs ) const
	{
	return static_cast<BOOLEAN>( !AreFloatsEqual( m_a1, rhs.m_a1, kTransformDelta ) ||
										  !AreFloatsEqual( m_a2, rhs.m_a2, kTransformDelta ) ||
										  !AreFloatsEqual( m_b1, rhs.m_b1, kTransformDelta ) ||
										  !AreFloatsEqual( m_b2, rhs.m_b2, kTransformDelta ) ||
										  !AreFloatsEqual( m_c1, rhs.m_c1 ) ||
										  !AreFloatsEqual( m_c2, rhs.m_c2 ) );
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::AreScaleAndRotateEqual( )
//
// Description:		Determines if the scaling and rotation portion of two
//							transforms are equal.
//
// Returns:				TRUE if they are equal, FALSE otherwise.
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN R2dTransform::AreScaleAndRotateEqual( const R2dTransform& rhs )	const
	{
	return static_cast<BOOLEAN>( AreFloatsEqual( m_a1, rhs.m_a1, kTransformDelta ) &&
										  AreFloatsEqual( m_a2, rhs.m_a2, kTransformDelta ) &&
										  AreFloatsEqual( m_b1, rhs.m_b1, kTransformDelta ) &&
										  AreFloatsEqual( m_b2, rhs.m_b2, kTransformDelta ) );
	}


// ****************************************************************************
//
// Function Name:		R2dTransform::Create( )
//
// Description:		Initialize this transform with the matrix required to
//                   transform rc1 to rc2.
//							this routine should not be called needlessly.  It is expensive....
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
R2dTransform& R2dTransform::Create( const RRealVectorRect& rc1, const RRealVectorRect& rc2 )
{
	//
	// Get the transformation parameters required to transform rc1 to rc2...
	YAngle			angle;
	YScaleFactor	xScale;
	YScaleFactor	yScale;
	YRealDimension	xOffset;
	YRealDimension	yOffset;
	GetComponents( rc1, rc2, angle, xScale, yScale, xOffset, yOffset );
	//
	// Initialize ourselves with this new info...
	MakeIdentity();
	PreTranslate( xOffset, yOffset );
	PreScaleAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, xScale, yScale );
	PreRotateAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, angle );
	//
	// and we're done!
	return *this;
}
R2dTransform& R2dTransform::Create( const RRealRect& rc1, const RRealRect& rc2 )
{
	//
	// Get the transformation parameters required to transform rc1 to rc2...
//	YAngle			angle;
	YScaleFactor	xScale;
	YScaleFactor	yScale;
	YRealDimension	xOffset;
	YRealDimension	yOffset;
	GetComponents( rc1, rc2, xScale, yScale, xOffset, yOffset );
	//
	// Initialize ourselves with this new info...
	MakeIdentity();
	PreTranslate( xOffset, yOffset );
	PreScaleAboutPoint( rc1.m_Left, rc1.m_Top, xScale, yScale );
	//
	// and we're done!
	return *this;
}
R2dTransform& R2dTransform::Create( const RRealRect& rc1, const RRealVectorRect& rc2 )
{
	//
	// Get the transformation parameters required to transform rc1 to rc2...
	YAngle			angle;
	YScaleFactor	xScale;
	YScaleFactor	yScale;
	YRealDimension	xOffset;
	YRealDimension	yOffset;
	GetComponents( rc1, rc2, angle, xScale, yScale, xOffset, yOffset );
	//
	// Initialize ourselves with this new info...
	MakeIdentity();
	PreTranslate( xOffset, yOffset );
	PreScaleAboutPoint( rc1.m_Left, rc1.m_Top, xScale, yScale );
	PreRotateAboutPoint( rc1.m_Left, rc1.m_Top, angle );
	//
	// and we're done!
	return *this;
}
R2dTransform& R2dTransform::Create( const RRealVectorRect& rc1, const RRealRect& rc2 )
{
	//
	// Get the transformation parameters required to transform rc1 to rc2...
	YAngle			angle;
	YScaleFactor	xScale;
	YScaleFactor	yScale;
	YRealDimension	xOffset;
	YRealDimension	yOffset;
	GetComponents( rc1, rc2, angle, xScale, yScale, xOffset, yOffset );
	//
	// Initialize ourselves with this new info...
	MakeIdentity();
	PreTranslate( xOffset, yOffset );
	PreScaleAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, xScale, yScale );
	PreRotateAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, angle );
	//
	// and we're done!
	return *this;
}

// ****************************************************************************
//
// Function Name:		R2dTransform::GetComponents( )
//
// Description:		Get the transformation parameters required to transform rc1 to rc2
//							this routine should not be called needlessly.  It is expensive....
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN R2dTransform::GetComponents(
			const RRealVectorRect&	rc1,
			const RRealVectorRect&	rc2,
			YAngle&						angle,
			YRealDimension&			xScale,
			YRealDimension&			yScale,
			YRealDimension&			xOffset,
			YRealDimension&			yOffset )
{
	RRealSize		size1( rc1.WidthHeight() );
	RRealSize		size2( rc2.WidthHeight() );
	RRealSize		topvec1( rc1.m_TopRight - rc1.m_TopLeft );
	RRealSize		topvec2( rc2.m_TopRight - rc2.m_TopLeft );

	//
	//	Compute the required scale...
	xScale	= ( size2.m_dx / size1.m_dx );
	yScale	= ( size2.m_dy / size1.m_dy );
	//
	// Compute the rotation angles of the two rects...
	// Remember: we're in upside down cartesian coordinates, so we need to flip the Y delta...
	YAngle	angle1( atan2(-topvec1.m_dy,topvec1.m_dx) );
	YAngle	angle2( atan2(-topvec2.m_dy,topvec2.m_dx) );
	//
	// Determine if one or both is flipped, and whether or not the result is flipped
	BOOLEAN	fFlipped1( TurnsCW(rc1.m_TopLeft,rc1.m_TopRight,rc1.m_BottomRight) );
	BOOLEAN	fFlipped2( TurnsCW(rc2.m_TopLeft,rc2.m_TopRight,rc2.m_BottomRight) );
	BOOLEAN	fFlipped( BOOLEAN((fFlipped1&&!fFlipped2) || (fFlipped2&&!fFlipped1)) );
	//
	// Adjust the scale if the combined transformation requires a flip.
	// Note that we force the flip to be around the Y axis (it shouldn't matter which)
	if( fFlipped )
		{
		xScale	= -xScale;
		angle2	= kPI - angle2;
		}
	//
	// Compute the required rotation...
	angle	= YAngle( angle2 - angle1 );
	//
	// Compute the required translation...
	xOffset	= YRealDimension( rc2.m_TopLeft.m_x - rc1.m_TopLeft.m_x );
	yOffset	= YRealDimension( rc2.m_TopLeft.m_y - rc1.m_TopLeft.m_y );

#ifdef TPSDEBUG
#ifdef CHECK_DECOMPOSE
	//
	// Initialize ourselves with this new info...
	R2dTransform	test;
	test.PreTranslate( xOffset, yOffset );
	test.PreScaleAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, xScale, yScale );
	test.PreRotateAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, angle );
	RRealVectorRect	rc3( rc1 * test );
	BOOLEAN				fTLSame( rc2.m_TopLeft==rc3.m_TopLeft );
	BOOLEAN				fTRSame( rc2.m_TopRight==rc3.m_TopRight );
	BOOLEAN				fBLSame( rc2.m_BottomLeft==rc3.m_BottomLeft );
	BOOLEAN				fBRSame( rc2.m_BottomRight==rc3.m_BottomRight );
	TpsAssert( fTLSame && fTRSame && fBLSame && fBRSame, "GetComponents failed" );
#endif
#endif
	//
	// and we're done!
	return fFlipped;
}
BOOLEAN R2dTransform::GetComponents(
			const RRealRect&	rc1,
			const RRealRect&	rc2,
			YRealDimension&	xScale,
			YRealDimension&	yScale,
			YRealDimension&	xOffset,
			YRealDimension&	yOffset )
{
	RRealSize		size1( rc1.WidthHeight() );
	RRealSize		size2( rc2.WidthHeight() );

	//
	//	Compute the required scale...
	xScale	= ( size2.m_dx / size1.m_dx );
	yScale	= ( size2.m_dy / size1.m_dy );
	//
	// Determine if one or both is flipped, and whether or not the result is flipped
	BOOLEAN	fFlipped1( TurnsCW(rc1.m_TopLeft,RRealPoint(rc1.m_Right,rc1.m_Top),rc1.m_BottomRight) );
	BOOLEAN	fFlipped2( TurnsCW(rc2.m_TopLeft,RRealPoint(rc1.m_Right,rc1.m_Top),rc2.m_BottomRight) );
	BOOLEAN	fFlipped( BOOLEAN((fFlipped1&&!fFlipped2) || (fFlipped2&&!fFlipped1)) );
	//
	// Adjust the scale if the combined transformation requires a flip.
	// Note that we force the flip to be around the Y axis (it shouldn't matter which)
	if( fFlipped )
		xScale	= -xScale;
	//
	// Compute the required translation...
	xOffset	= YRealDimension( rc2.m_Left - rc1.m_Left );
	yOffset	= YRealDimension( rc2.m_Top - rc1.m_Top );

#ifdef TPSDEBUG
#ifdef CHECK_DECOMPOSE
	//
	// Initialize ourselves with this new info...
	R2dTransform	test;
	test.PreTranslate( xOffset, yOffset );
	test.PreScaleAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, xScale, yScale );
	RRealRect	rc3( rc1 * test );
	BOOLEAN		fTLSame( rc2.m_TopLeft==rc3.m_TopLeft );
	BOOLEAN		fBRSame( rc2.m_TopRight==rc3.m_TopRight );
	TpsAssert( fTLSame && fBRSame, "GetComponents failed" );
#endif
#endif
	//
	// and we're done!
	return fFlipped;
}
BOOLEAN R2dTransform::GetComponents(
			const RRealRect&			rc1,
			const RRealVectorRect&	rc2,
			YAngle&						angle,
			YRealDimension&			xScale,
			YRealDimension&			yScale,
			YRealDimension&			xOffset,
			YRealDimension&			yOffset )
{
	RRealSize		size1( rc1.WidthHeight() );
	RRealSize		size2( rc2.WidthHeight() );
	RRealSize		topvec2( rc2.m_TopRight - rc2.m_TopLeft );

	//
	//	Compute the required scale...
	xScale	= ( size2.m_dx / size1.m_dx );
	yScale	= ( size2.m_dy / size1.m_dy );
	//
	// Compute the rotation angles of the two rects...
	// Remember: we're in upside down cartesian coordinates, so we need to flip the Y delta...
	YAngle	angle1( 0.0 );
	YAngle	angle2( atan2(-topvec2.m_dy,topvec2.m_dx) );
	//
	// Determine if one or both is flipped, and whether or not the result is flipped
	BOOLEAN	fFlipped1( TurnsCW(rc1.m_TopLeft,RRealPoint(rc1.m_Right,rc1.m_Top),rc1.m_BottomRight) );
	BOOLEAN	fFlipped2( TurnsCW(rc2.m_TopLeft,rc2.m_TopRight,rc2.m_BottomRight) );
	BOOLEAN	fFlipped( BOOLEAN((fFlipped1&&!fFlipped2) || (fFlipped2&&!fFlipped1)) );
	//
	// Adjust the scale if the combined transformation requires a flip.
	// Note that we force the flip to be around the Y axis (it shouldn't matter which)
	if( fFlipped )
		{
		xScale	= -xScale;
		angle2	= kPI - angle2;
		}
	//
	// Compute the required rotation...
	angle	= YAngle( angle2 - angle1 );
	//
	// Compute the required translation...
	xOffset	= YRealDimension( rc2.m_TopLeft.m_x - rc1.m_TopLeft.m_x );
	yOffset	= YRealDimension( rc2.m_TopLeft.m_y - rc1.m_TopLeft.m_y );

#ifdef TPSDEBUG
#ifdef CHECK_DECOMPOSE
	//
	// Initialize ourselves with this new info...
	R2dTransform	test;
	test.PreTranslate( xOffset, yOffset );
	test.PreScaleAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, xScale, yScale );
	test.PreRotateAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, angle );
	RRealVectorRect	rc3( RRealVectorRect(rc1) * test );
	BOOLEAN				fTLSame( rc2.m_TopLeft==rc3.m_TopLeft );
	BOOLEAN				fTRSame( rc2.m_TopRight==rc3.m_TopRight );
	BOOLEAN				fBLSame( rc2.m_BottomLeft==rc3.m_BottomLeft );
	BOOLEAN				fBRSame( rc2.m_BottomRight==rc3.m_BottomRight );
	TpsAssert( fTLSame && fTRSame && fBLSame && fBRSame, "GetComponents failed" );
#endif
#endif
	//
	// and we're done!
	return fFlipped;
}
BOOLEAN R2dTransform::GetComponents(
			const RRealVectorRect&	rc1,
			const RRealRect&			rc2,
			YAngle&						angle,
			YRealDimension&			xScale,
			YRealDimension&			yScale,
			YRealDimension&			xOffset,
			YRealDimension&			yOffset )
{
	RRealSize		size1( rc1.WidthHeight() );
	RRealSize		size2( rc2.WidthHeight() );
	RRealSize		topvec1( rc1.m_TopRight - rc1.m_TopLeft );

	//
	//	Compute the required scale...
	xScale	= ( size2.m_dx / size1.m_dx );
	yScale	= ( size2.m_dy / size1.m_dy );
	//
	// Compute the rotation angles of the two rects...
	// Remember: we're in upside down cartesian coordinates, so we need to flip the Y delta...
	YAngle	angle1( atan2(-topvec1.m_dy,topvec1.m_dx) );
	YAngle	angle2( 0.0 );
	//
	// Determine if one or both is flipped, and whether or not the result is flipped
	BOOLEAN	fFlipped1( TurnsCW(rc1.m_TopLeft,rc1.m_TopRight,rc1.m_BottomRight) );
	BOOLEAN	fFlipped2( TurnsCW(rc2.m_TopLeft,RRealPoint(rc2.m_Right,rc2.m_Top),rc2.m_BottomRight) );
	BOOLEAN	fFlipped( BOOLEAN((fFlipped1&&!fFlipped2) || (fFlipped2&&!fFlipped1)) );
	//
	// Adjust the scale if the combined transformation requires a flip.
	// Note that we force the flip to be around the Y axis (it shouldn't matter which)
	if( fFlipped )
		{
		xScale	= -xScale;
		angle2	= kPI - angle2;
		}
	//
	// Compute the required rotation...
	angle	= YAngle( angle2 - angle1 );
	//
	// Compute the required translation...
	xOffset	= YRealDimension( rc2.m_TopLeft.m_x - rc1.m_TopLeft.m_x );
	yOffset	= YRealDimension( rc2.m_TopLeft.m_y - rc1.m_TopLeft.m_y );

#ifdef TPSDEBUG
#ifdef CHECK_DECOMPOSE
	//
	// Initialize ourselves with this new info...
	R2dTransform	test;
	test.PreTranslate( xOffset, yOffset );
	test.PreScaleAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, xScale, yScale );
	test.PreRotateAboutPoint( rc1.m_TopLeft.m_x, rc1.m_TopLeft.m_y, angle );
	RRealVectorRect	rc3( rc1 * test );
	BOOLEAN				fTLSame( rc2.m_TopLeft==rc3.m_TopLeft );
	BOOLEAN				fTRSame( RRealPoint(rc2.m_Right,rc2.m_Top)==rc3.m_TopRight );
	BOOLEAN				fBLSame( RRealPoint(rc2.m_Left,rc2.m_Bottom)==rc3.m_BottomLeft );
	BOOLEAN				fBRSame( rc2.m_BottomRight==rc3.m_BottomRight );
	TpsAssert( fTLSame && fTRSame && fBLSame && fBRSame, "GetComponents failed" );
#endif
#endif
	//
	// and we're done!
	return fFlipped;
}


// ****************************************************************************
//
// Function Name:		R2dTransform::Decompose( )
//
// Description:		Decompose the transform to retrieve the scales and rotation factors
//							this routine should not be called needlessly.  It is expensive....
//
// Returns:				TRUE if there is a flip in the scales (if the scale signs are opposite)
//
// Exceptions:			None
//
// ****************************************************************************
//
#define	USE_COMMON_CODE_WITH_SMALL_COST_IN_PERFORMANCE	TRUE

BOOLEAN R2dTransform::Decompose( YAngle& angle, YRealDimension& xScale, YRealDimension& yScale ) const
	{
	const	YRealDimension	kSideSize	= 1000.0;

#if USE_COMMON_CODE_WITH_SMALL_COST_IN_PERFORMANCE

	YRealDimension	xOffset;
	YRealDimension	yOffset;
	RRealVectorRect	rc1( RRealRect(0.0,0.0,kSideSize,kSideSize) );
	RRealVectorRect	rc2 = rc1 * (*this);
	return GetComponents( rc1, rc2, angle, xScale, yScale, xOffset, yOffset );

#else

	//	Remove the translates from the transform
	R2dTransform	xForm			= *this;
	xForm.m_c1	= xForm.m_c2	= 0.0;
	//	Take a two unit vectors and see how the transform will manipulate them
	//		describe the vectors in computer space, not cartesian

	RRealPoint		topLeft		= RRealPoint( 0.0,			0.0 )			* xForm;
	RRealPoint		topRight		= RRealPoint( kSideSize,	0.0 )			* xForm;
	RRealPoint		bottomRight	= RRealPoint( kSideSize,	kSideSize )	* xForm;
	YRealDimension	deltaX		= topRight.m_x - topLeft.m_x;
	YRealDimension	deltaY		= topRight.m_y - topLeft.m_y;

	//	Compute the scale and angle
	xScale	= topRight.Distance( topLeft ) / kSideSize;
	yScale	= topRight.Distance( bottomRight ) / kSideSize;
	angle		= atan2( deltaY, deltaX );

	BOOLEAN	fFlipped = TurnsCW( topLeft, topRight, bottomRight );

	//	Adjust for flips (negative deltas)
	if ( fFlipped)
		{
		xScale	= -xScale;
		angle		= kPI - angle;
		}

	return fFlipped;

#endif
	}

// ****************************************************************************
//
// Function Name:		R2dTransform::GetTranslation( )
//
// Description:		Return the m_c1, m_c2 values
//
// Returns:				the point compose of the translation values
//
// Exceptions:			None
//
// ****************************************************************************
//
void R2dTransform::GetTranslation( YRealDimension& xOffset, YRealDimension& yOffset ) const
	{
	xOffset = m_c1;
	yOffset = m_c2;
	}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for transforms
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator<<( RArchive& archive, const R2dTransform& transform )
	{
	archive << transform.m_a1 << transform.m_a2;
	archive << transform.m_b1 << transform.m_b2;
	archive << transform.m_c1 << transform.m_c2;
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for transforms
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator>>( RArchive& archive, R2dTransform& transform )
	{
	archive >> transform.m_a1 >> transform.m_a2;
	archive >> transform.m_b1 >> transform.m_b2;
	archive >> transform.m_c1 >> transform.m_c2;
	return archive;
	}
