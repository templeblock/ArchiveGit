// ****************************************************************************
//
//  File Name:			RPolyPolygon.h
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
//  $Logfile:: /PM8/Framework/Include/PolyPolygon.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _RPolyPolygon_h_
#define _RPolyPolygon_h_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef	RArray<RIntPoint>						YPointCollection;
typedef	YPointCollection::YIterator		YPointIterator;

typedef	RArray<int>								YPointIndexCollection;
typedef	YPointIndexCollection::YIterator YPointIndexIterator;


class FrameworkLinkage RPolyPolygon
	{
		//	Construction
	public :
										RPolyPolygon( );

		//	Implementation
	public:
		void							AddPoint( const RIntPoint& point );
		void							NewPolygon();
		void							EndPolygon();
		RIntPoint*					GetPointCollection();
		int*							GetPointIndexCollection() ;
		uLONG							GetPointIndexCount() const;

		void							Empty( );
		void							ApplyTransform( const R2dTransform& transform );
	
		//	Data members
	private:
		YPointCollection			m_pointCollection;
		YPointIndexCollection	m_pointIndexCollection;
		int							m_iLastCount;

#ifdef	TPSDEBUG
	private :
		BOOLEAN						m_fPolyStarted;
#endif
	} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_RPolyPolygon_h_
