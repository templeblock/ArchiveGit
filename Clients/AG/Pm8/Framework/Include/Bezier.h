// ****************************************************************************
//
//  File Name:			Bezier.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Functions to draw bezier curves
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
//  $Logfile:: /PM8/Framework/Include/Bezier.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef float YBezierType;

void DoBezier( const RIntPoint* bezierPoints, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );
void FillBezier( const RIntPoint& b0, const RIntPoint& b1, const RIntPoint& b2, const RIntPoint& b3, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );
void FillBezier( const RRealPoint& b0, const RRealPoint& b1, const RRealPoint& b2, const RRealPoint& b3, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );
void FillBezier( const RRealPoint& b0, const RRealPoint& b1, const RRealPoint& b2, const RRealPoint& b3, RRealPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );

void SubdivideBezier( const RIntPoint& ptStart, 
							 const RIntPoint& ptControl1, 
							 const RIntPoint& ptControl2, 
							 const RIntPoint& ptEnd, 
							 RIntPoint* pOutPoints );
void SubdivideBezier( const RRealPoint& ptStart, 
							 const RRealPoint& ptControl1, 
							 const RRealPoint& ptControl2, 
							 const RRealPoint& ptEnd, 
							 RRealPoint* pOutPoints );
void SubdivideBezier( const RRealPoint& ptStart, 
							 const RRealPoint& ptControl1, 
							 const RRealPoint& ptControl2, 
							 const RRealPoint& ptEnd, 
							 RRealPoint* pOutPoints,
							 YPointCount nrSegments );

BOOLEAN CreateBezierFromArc( const RRealPoint& a0,
									  const RRealPoint& a2,
									  const RRealPoint& center,
									  YAngle angInterior,
									  RRealPoint& b0,
									  RRealPoint& b1,
									  RRealPoint& b2,
									  RRealPoint& b3 );
BOOLEAN CreatePolyBezierFromArc( const RRealPoint& a0,
											const RRealPoint& a1,
											const RRealPoint& a2,
											RArray<RRealPoint>& arPolyBezier );

// ****************************************************************************
//
//  Function Name:	::GetBezierBounds( )
//
//  Description:		Computes the bounding rectangle of the given bezier
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
void GetBezierBounds( const RPoint<T>& b0, const RPoint<T>& b1, const RPoint<T>& b2, const RPoint<T>& b3, RRect<T>& rcBounds )
{
	//
	// Compute the coefficients of the derivative of the bezier function...
	YFloatType	Ay, By, Cy;
	YFloatType	Ax, Bx, Cx;
	::ComputeBezierDerivativeCoefficients( b0.m_y, b1.m_y, b2.m_y, b3.m_y, Ay, By, Cy );
	::ComputeBezierDerivativeCoefficients( b0.m_x, b1.m_x, b2.m_x, b3.m_x, Ax, Bx, Cx );
	//
	// Compute the roots of the derivative (i.e. where the slope is 0)
	YFloatType	dRoots[4];
	int			nRoots( ::SolveQuadratic(Ay,By,Cy,dRoots) );
	nRoots += ::SolveQuadratic( Ax, Bx, Cx, dRoots+nRoots );
	//
	// sort in ascending order and remove duplicates...
	nRoots = ::UniqueFloats( dRoots, nRoots );
	//
	// setup the bounding rect...
	rcBounds.m_TopLeft		= b0;
	rcBounds.m_BottomRight	= b0;
	//
	// setup the first test point...
	RPoint<T>	ptTest( b3 );
	//
	// loop across all test points...
	do
		{
		if( ptTest.m_x < rcBounds.m_Left )		rcBounds.m_Left	= ptTest.m_x;
		if( ptTest.m_y < rcBounds.m_Top )		rcBounds.m_Top		= ptTest.m_y;
		if( ptTest.m_x > rcBounds.m_Right )		rcBounds.m_Right	= ptTest.m_x;
		if( ptTest.m_y > rcBounds.m_Bottom )	rcBounds.m_Bottom	= ptTest.m_y;
		while( nRoots>0 && (dRoots[nRoots-1]<=0.0 || dRoots[nRoots-1]>=1.0) )
			--nRoots;
		if( nRoots>0 )
			ptTest = ::PointOnBezier( b0, b1, b2, b3, dRoots[nRoots-1] );
		}
	while( nRoots-- );
}

// ****************************************************************************
//
//  Function Name:	::GetBezierBounds( )
//
//  Description:		Computes the bounding rectangle of the given bezier
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
void GetBezierBounds( const RPoint<T>* pBez, RRect<T>& rcBounds )
{
	::GetBezierBounds( pBez[0], pBez[1], pBez[2], pBez[3], rcBounds );
}

// ****************************************************************************
//
//  Function Name:	::SplitBezier( )
//
//  Description:		Subdivide the given bezier at the given parameter
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
void SplitBezier( const RPoint<T>* pBez, RPoint<T>* pLeft, RPoint<T>* pRight, YFloatType t )
{
	//--------------------------
	//      b1----b----b2      |
	//     / _--/   \--_ \     |
	//   a--/ d---f---e \-c    |
	// /                   \   |
	// b0                   b3 |
	//--------------------------
	pLeft[0]		= pBez[0];
	pRight[3]	= pBez[3];
	pLeft[1]		= Interpolate( pBez[0], pBez[1], t );		// a
	RPoint<T>	b( Interpolate(pBez[1],pBez[2],t) );		// b
	pRight[2]	= Interpolate( pBez[2], pBez[3], t );		// c
	pLeft[2]		= Interpolate( pLeft[1], b, t );				// d
	pRight[1]	= Interpolate( b, pRight[2], t );			// e
	pLeft[3]		= Interpolate( pLeft[2], pRight[1], t );	// f
	pRight[0]	= pLeft[3];
}

//
// ComputeBezierCoefficients
// Compute A,B,C, and D for the cubic equation Axxx + Bxx + Cx + D
inline void ComputeBezierCoefficients( YFloatType b0, YFloatType b1, YFloatType b2, YFloatType b3, YFloatType& A, YFloatType& B, YFloatType& C, YFloatType& D )
{
	A = 3.0 * (b1 - b2) + b3 - b0;
	B = 3.0 * (b0 + b2 - 2.0*b1);
	C = 3.0 * (b1 - b0);
	D = b0;
}

//
// ComputeBezierDerivativeCoefficients
// Compute A,B, and C for the derivative of the bezier
inline void ComputeBezierDerivativeCoefficients( YFloatType b0, YFloatType b1, YFloatType b2, YFloatType b3, YFloatType& A, YFloatType& B, YFloatType& C )
{
	YFloatType	D;
	::ComputeBezierCoefficients( b0, b1, b2, b3, A, B, C, D );
	A *= 3.0;
	B *= 2.0;
}

// ****************************************************************************
//
//  Function Name:	::PointOnBezier( )
//
//  Description:		Compute the point on the given bezier at the given parameter
//
//  Returns:			the point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
RPoint<T> PointOnBezier( const RPoint<T>& b0, const RPoint<T>& b1, const RPoint<T>& b2, const RPoint<T>& b3, YFloatType t )
{
	YFloatType	t1( 1-t );
	YFloatType	tt1( t1*t1 );
	YFloatType	ttt1( tt1*t1 );
	YFloatType	tt( t*t );
	YFloatType	ttt( tt*t );
	YFloatType	tt1t3( tt1*t*3.0 );
	YFloatType	t1tt3( t1*tt*3.0 );
	RRealPoint	rpt;
	rpt.m_x	= YRealDimension( ttt1*b0.m_x + tt1t3*b1.m_x + t1tt3*b2.m_x + ttt*b3.m_x );
	rpt.m_y	= YRealDimension( ttt1*b0.m_y + tt1t3*b1.m_y + t1tt3*b2.m_y + ttt*b3.m_y );
	return RPoint<T>(rpt.m_x, rpt.m_y);
}

// ****************************************************************************
//
//  Function Name:	::PointOnBezier( )
//
//  Description:		Compute the point on the given bezier at the given parameter
//
//  Returns:			the point
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class T>
RPoint<T> PointOnBezier( const RPoint<T>* pBez, YFloatType t )
{
	return ::PointOnBezier( pBez[0], pBez[1], pBez[2], pBez[3], t );
}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif
