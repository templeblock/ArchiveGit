// ****************************************************************************
//
//  File Name:			Path.h
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RPath class
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
//  $Logfile:: /PM8/Framework/Include/Path.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PATH_H_
#define		_PATH_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef int YOutlineCount;
typedef int YPathOperatorArraySize;

typedef	uWORD	YFillMethod;

const YFillMethod kStroked = 0;
const YFillMethod kFilled = 1;
const YFillMethod kFilledAndStroked = 2;
const YFillMethod	kGradFilled = 3;
const YFillMethod	kGradFilledAndStroked = 4;
const YFillMethod kStrokedAndFilled = 5;
const YFillMethod	kStrokedAndGradFilled = 6;

enum EPathOperator {				kMoveTo,
										kClose,
										kLine,
										kBezier,
										kEnd,
										kQuadraticSpline };

enum EConvertFlag {			   kHorizontalLines = 1,
										kVerticalLines   = 2 };

// Path types
typedef	RArray<YIntDimension>						YIntDimensionArray;
typedef	YIntDimensionArray::YIterator				YIntDimensionArrayIterator;
typedef	RArray<RIntPoint>								RIntPointArray;
typedef	RIntPointArray::YIterator					RIntPointArrayIterator;
typedef	RArray<RRealPoint>							RRealPointArray;
typedef	RRealPointArray::YIterator					RRealPointArrayIterator;
typedef	RArray<EPathOperator>						EPathOperatorArray;
typedef	EPathOperatorArray::YIterator				EPathOperatorArrayIterator;
typedef	RArray<YPointCount>							YPointCountArray;

class	RDrawingSurface;

// forward ref
class RSingleGraphic;
class RStorage;

// ****************************************************************************
//
// Class Name:			RPath
//
// Description:		Class to 
//
// ****************************************************************************
//
class FrameworkLinkage RPath : public RObject					 
{
	// Construction, Destruction & Initialization
	public :
												RPath( );
												RPath( uLONG grows );
												RPath( const RPath &rhs );
												RPath( const RPath &rhs, const R2dTransform& transform );
		virtual								~RPath( );

	// Operators
		RPath&								operator = ( const RPath& rhs );

	// Configure Operations
	public :
		void                          Undefine( );
		void									UndefineLastTransform( );
		BOOLEAN								Define( const RIntRect& rect );
		BOOLEAN								Define( const RRealRect& rect );
		BOOLEAN								Define( const RIntPoint* pVertices, YPointCount* pNumVertices, YPolylineCount polyCount, BOOLEAN fClosed );
		BOOLEAN								Define( EPathOperator* pOperators, const RIntPoint* pVertices );
		BOOLEAN								Define( EPathOperator* pOperators, const RRealPoint* pVertices );
		BOOLEAN								DefineFromArcs( const RIntPoint* pArcPoints, YPointCount* pNumArcPoints, YPolylineCount polyCount );
		BOOLEAN								Append( RPath& path, const R2dTransform& transform );

		void									ConvertSplinesToBeziers( );
		void									ConvertLinesToBeziers( uWORD flags );

		BOOLEAN								IsDefined( ) const;

	// Accessors
	public :
		YIntDimension						GetLength( );
		YPointCount							GetPointCount( ) const;
		RIntSize								GetPathSize( ) const;
		RIntRect 							GetBoundingRect( ) const;
		BOOLEAN								UsesSplines( ) const;
		BOOLEAN								UsesBeziers( ) const;
		YPathOperatorArraySize			GetOperatorCount( ) const;
		static YPathOperatorArraySize	GetOperatorCount( EPathOperator* pOperators );
		YOutlineCount						GetOutlineCount( ) const;
		RRealPoint							GetStartPoint( ) const;
		RRealPoint							GetEndPoint( ) const;

	// Rendering
	public :
		static YPathOperatorArraySize	RenderPathPoints( RDrawingSurface& drawingSurface,
														  				EPathOperator* pOperators,
														  				RIntPoint* pPathPoints,
														  				RRealPoint* pRealPathPoints,
														  				YFillMethod fillMethod,
														  				BOOLEAN fCacheValid,
														  				const RIntSize& offset );
	// Operations
	public :
		RRealPoint							PointOnPath( YScaleFactor fraction );
		RRealPoint							PointOnPath( YIntDimension distance );
		RRealPoint							PointOnPath( YIntDimension distance, YIntDimension cord );
		RRealPoint							PointOnPath( YIntDimension distance, YIntDimension cord, YIntDimension* seg );
		RRealPoint							PointAtX( YRealCoordinate x );
		RRealPoint							PointAtY( YRealCoordinate y );
		void									Reverse( );
		void									PrepToWarp( BOOLEAN fHorizontal );
		void									Render( RDrawingSurface& drawingSurface,
														  const R2dTransform& transform,
														  const RIntRect& rRender,
														  YFillMethod fillMethod = kFilledAndStroked );
		void									Render( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rRender,
														  const RColor& monochromeColor = RSolidColor( kBlack ),
														  BOOLEAN fImageAsMask = FALSE );
		void									Render( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& rRender,
														  YFillMethod fillMethod,
														  const RSolidColor& strokeColor,
														  const RColor& fillColor,
														  int	nPenWidth = 1);
		RIntRect								MergeWith( const R2dTransform& transform,
															  RSingleGraphic& graphic );
		BOOLEAN								MergePoints( const RIntPoint* pVertices, YPointCount* pNumVertices, YPolylineCount polyCount, BOOLEAN fClosed );
		BOOLEAN								MergePoints( const EPathOperator* pOperators, const RIntPoint* pVertices );
		void									CalcPath( const R2dTransform& transform, RIntPoint*& pPoints, YPointCountArray& countArray );
		void									SetPathSize( const RIntSize& size );

	// Serialization
	virtual void							Read( RArchive& storage );
	virtual void							Write( RArchive& storage ) const;

	// Static Operations
	public :
		static BOOLEAN						IsClosed( const EPathOperator* pOperators );
		static BOOLEAN						IsEmpty( const EPathOperator* pOperators, const RIntPoint* pVertices );
		static BOOLEAN						IsEmpty( const EPathOperator* pOperators, const RRealPoint* pVertices );
		static RIntRect					CalcBoundingRect( const EPathOperator* pOperators,
														  				const RIntPoint* pPathPoints );
		static RIntRect					CalcBoundingRect( const EPathOperator* pOperators,
														  				const RRealPoint* pPathPoints );
		static YIntDimension				CalcBezierLength( const RIntPoint& ptStart, 
																		const RIntPoint& ptControl1, 
																		const RIntPoint& ptControl2, 
																		const RIntPoint& ptEnd );
		static YRealDimension			CalcBezierLength( const RRealPoint& ptStart, 
																		const RRealPoint& ptControl1, 
																		const RRealPoint& ptControl2, 
																		const RRealPoint& ptEnd );
		static YIntDimension				CalcQuadraticSplineLength( const RIntPoint& ptStart, 
																					const RIntPoint& ptControl, 
																					const RIntPoint& ptEnd );
		static YRealDimension			CalcQuadraticSplineLength( const RRealPoint& ptStart, 
																					const RRealPoint& ptControl, 
																					const RRealPoint& ptEnd );
		static RIntPoint 					PointOnLine( const RIntPoint& ptStart, const RIntPoint& ptEnd, YIntDimension distance );
		static RRealPoint 				PointOnLine( const RRealPoint& ptStart, const RRealPoint& ptEnd, YIntDimension distance );
		static RIntPoint 					PointOnBezier( const RIntPoint* points, YScaleFactor fraction );
		static RRealPoint 				PointOnBezier( const RRealPoint* points, YScaleFactor fraction );
		static RIntPoint 					PointOnQuadraticSpline( const RIntPoint* points, YScaleFactor fraction );
		static RRealPoint 				PointOnQuadraticSpline( const RRealPoint* points, YScaleFactor fraction );

	// Implementation
	private :
		void									ApplyScale( YScaleFactor scale );
		void									ApplyTransform( const R2dTransform& transform );
		void 									CalcSegmentLengths( );
		EPathOperator						GetSegmentOfPath( short segment, RRealPoint* points ) const;
		void		 							CalcBoundingRect( );

	// Helper
	private :
		EPathOperator						FindXSegment( YRealCoordinate x, RRealPoint* ppts );
		EPathOperator						FindYSegment( YRealCoordinate y, RRealPoint* ppts );
		static void							CopyPointArray( const RRealPointArray& src, RIntPointArray& dst );
		static void							CopyPointArray( const RIntPointArray& src, RRealPointArray& dst );
		static YIntDimension				ApproxBezierLength( const RRealPoint& ptStart, 
																		  const RRealPoint& ptControl1, 
																		  const RRealPoint& ptControl2, 
																		  const RRealPoint& ptEnd );
		static YIntDimension				ApproxQuadraticSplineLength( const RRealPoint& ptStart, 
																					  const RRealPoint& ptControl, 
																					  const RRealPoint& ptEnd );
		static YIntDimension				SubDivideLineSegment( const RRealPoint& ptStart, 
																			 const RRealPoint& ptEnd, 
																			 BOOLEAN fHorizontal,
																			 RRealPoint* points );
		static YIntDimension				SubDivideBezierSegment( const RRealPoint& ptStart, 
																				const RRealPoint& ptControl1, 
																				const RRealPoint& ptControl2, 
																				const RRealPoint& ptEnd, 
																				RRealPoint* points );
		static YIntDimension				SubDivideQuadraticSplineSegment( const RRealPoint& ptStart, 
																							const RRealPoint& ptControl, 
																							const RRealPoint& ptEnd, 
																							RRealPoint* points );

	// Render Helper
	private :
		static YPathOperatorArraySize	DrawPath( RDrawingSurface& drawingSurface,
														  	 EPathOperator* pOperators,
														  	 RIntPoint* pPathPoints,
														  	 YFillMethod fillMethod );
		static YPathOperatorArraySize	DoPath( RDrawingSurface& drawingSurface,
														  EPathOperator* pOperators,
														  RIntPoint* pPathPoints );
		static YPathOperatorArraySize	CalcPathPoints( EPathOperator* pOperators,
																	 RRealPoint* pPathPoints,
																	 RIntPoint*& pPolygonPoints,
																	 YPointCountArray& pointCountArray,
																	 BOOLEAN fTryToClip,
																	 const RIntRect& rClip );
		static void							DrawPathPoints( RDrawingSurface& drawingSurface,
																	 YFillMethod fillMethod,
																	 RIntPoint* pPolygonPoints,
																	 YPointCountArray& pointCountArray );
		static void							MarkPathPoints( RDrawingSurface& drawingSurface,
															  		 EPathOperator* pOperators,
																    RIntPoint* pPathPoints );

	// Member data
	private :
		EPathOperatorArray				m_PathOperators;
		RRealPointArray					m_PathPoints;
		RIntPointArray						m_TransformedIntPoints;
		RRealPointArray					m_TransformedRealPoints;
		YOutlineCount						m_OutlineCount;
		RIntSize								m_PathSize;
		RIntRect								m_BoundingRect;
		YIntDimensionArray				m_SegmentLengths;
		R2dTransform						m_LastTransform;

#ifdef	TPSDEBUG
	// Debugging support  
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
};

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RPath& path );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RPath& path );

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
// Function Name:		RPath::RPath( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RPath::RPath( ) : m_OutlineCount( 0 ),
								 m_BoundingRect( 0, 0, 0, 0 ),
								 m_PathSize( 0, 0 )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RPath::RPath( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RPath::RPath( uLONG grows ) : m_OutlineCount( 0 ),
	  							 				 m_PathPoints( grows ),
	  							 				 m_TransformedIntPoints( grows ),
	  							 				 m_TransformedRealPoints( grows ),
								 				 m_BoundingRect( 0, 0, 0, 0 ),
								 				 m_PathSize( 0, 0 )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RPath::~RPath( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RPath::~RPath( )
{
	NULL;
}

// ****************************************************************************
//
// Function Name:		RPath::GetPathSize( )
//
// Description:		
//
// Returns:				RIntSize
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RIntSize RPath::GetPathSize( ) const
{
	return m_PathSize;
}

// ****************************************************************************
//
// Function Name:		RPath::GetBoundingRect( )
//
// Description:		
//
// Returns:				RIntRect
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RIntRect RPath::GetBoundingRect( ) const
{
	return m_BoundingRect;
}

// ****************************************************************************
//
// Function Name:		RPath::GetPointCount( )
//
// Description:		
//
// Returns:				YPointCount
//
// Exceptions:			None
//
// ****************************************************************************
//
inline YPointCount RPath::GetPointCount( ) const
{
	return m_PathPoints.Count();
}

// ****************************************************************************
//
// Function Name:		RPath::GetOutlineCount( )
//
// Description:		
//
// Returns:				YOutlineCount
//
// Exceptions:			None
//
// ****************************************************************************
//
inline YOutlineCount RPath::GetOutlineCount( ) const
{
	return m_OutlineCount;
}


// ****************************************************************************
//
// Class Name:			RClippingPath
//
// Description:		Class to 
//
// ****************************************************************************
//
class FrameworkLinkage RClippingPath : public RPath					 
{
	// Construction, Destruction & Initialization
public :
											RClippingPath( BOOLEAN fPathFromImport = FALSE );
											RClippingPath( uLONG grows, BOOLEAN fPathFromImport );
											RClippingPath( const RClippingPath &rhs );
											RClippingPath( const RClippingPath &rhs, const R2dTransform& transform );
											RClippingPath( const RPath &rhs, const R2dTransform& transform, BOOLEAN fPathFromImport  );

	// Accesors
	BOOLEAN								PathFromImport() 
											{ return m_fPathFromImport; }

	// Operators
	RClippingPath&						operator= ( const RClippingPath& rhs );
//	RClippingPath&						operator= ( const RPath& rhs );

	// Serialization
	virtual void						Read( RArchive& storage );
	virtual void						Write( RArchive& storage ) const;

private:

	BOOLEAN								m_fPathFromImport;
};

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RClippingPath& path );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RClippingPath& path );

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _PATH_H_
