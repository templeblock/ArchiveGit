// ****************************************************************************
//
//  File Name:			PolyPolygon.cpp
//
//  Project:			TPS Libraries
//
//  Author:				Greg Brown, Bruce Rosenblum
//
//  Description:		Class to encapsulate a poly-polygon
//
//  Portability:		Platform independent, 32 bit systems only
//
//  Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Turning Point Software         
//
//  Copyright (C) 1996 Turning Point Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/PolyPolygon.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "PolyPolygon.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

const	int kPointGrowSize	= 1024;
const int kIndexGrowSize	= 64;

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::RPolyPolygon( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
RPolyPolygon::RPolyPolygon( )
	: m_pointCollection( kPointGrowSize ),
	  m_pointIndexCollection( kIndexGrowSize ),
	  m_iLastCount( 0 )
	{
	Debug( m_fPolyStarted = FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::AddPoint( )
//
//  Description:		Called to add a point to a polypolygon
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void	RPolyPolygon::AddPoint( const RIntPoint& point )
{
	TpsAssert( m_fPolyStarted, "Point added, but poly not started" );
	m_pointCollection.InsertAtEnd( point );
}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::NewPolygon( )
//
//  Description:		Called to start a new polygon in a polypolygon
//
//  Returns:			Nothing
//
//  Exceptions:		none
//
// ****************************************************************************
//
void	RPolyPolygon::NewPolygon()
{
	Debug( m_fPolyStarted = TRUE );
	m_iLastCount = m_pointCollection.Count();
}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::EndPolygon( )
//
//  Description:		Called to end a polygon in a polypolygon
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void	RPolyPolygon::EndPolygon()
{
	TpsAssert( m_fPolyStarted, "Polygon ended, but not started" );
	//	add current total count minus total last time we added
	m_pointIndexCollection.InsertAtEnd( m_pointCollection.Count() - m_iLastCount );
}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::GetPointCollection( )
//
//  Description:		Get pointer to the points in a polypolygon
//
//  Returns:			Pointer to list of points (platform-specific type)
//
//  Exceptions:		none
//
// ****************************************************************************
//
RIntPoint* RPolyPolygon::GetPointCollection()
{
	return static_cast<RIntPoint*>( &(*m_pointCollection.Start()) );
}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::GetPointIndexCollection( )
//
//  Description:		Get pointer to the array of polygon starts in a polypolygon
//
//  Returns:			Pointer to list of ints
//
//  Exceptions:		none
//
// ****************************************************************************
//
int* RPolyPolygon::GetPointIndexCollection()
{
	return static_cast<int*>(  &(*m_pointIndexCollection.Start()) );
}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::GetPointIndexCollection( )
//
//  Description:		Get number of polygon starts in a polypolygon
//
//  Returns:			long (Windows should cast to an int); return is 1-based
//
//  Exceptions:		none
//
// ****************************************************************************
//
uLONG	RPolyPolygon::GetPointIndexCount() const
{
	return m_pointIndexCollection.Count();
}


// ****************************************************************************
//
//  Function Name:	RPolyPolygon::Empty( )
//
//  Description:		Empth all of the collections
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RPolyPolygon::Empty( )
{
	m_pointCollection.Empty( );
	m_pointIndexCollection.Empty( );
}

// ****************************************************************************
//
//  Function Name:	RPolyPolygon::ApplyTransform( )
//
//  Description:		Apply a transform to all of the points in the polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RPolyPolygon::ApplyTransform( const R2dTransform& transform )
{
	YPointIterator	iterator	= m_pointCollection.Start( );
	YPointIterator iterEnd	= m_pointCollection.End( );
	for ( ; iterator != iterEnd; ++iterator )
		(*iterator) *= transform;
}
