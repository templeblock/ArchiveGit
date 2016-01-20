// ****************************************************************************
//
//  File Name:			2dTransform.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the R2dTransform class
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
//  $Logfile:: /PM8/Framework/Include/2dtransform.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_2DTRANSFORM_H_
#define		_2DTRANSFORM_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RArchive;
template<class T> class RSize;
template<class T> class RPoint;
template<class T> class RRect;
template<class T> class RVectorRect;

// ****************************************************************************
//
// Class Name:			R2dTransform
//
// Description:		2d transform class
//
// ****************************************************************************
//
class FrameworkLinkage R2dTransform
	{
	// Construction, Destruction & Initialization
	public :
												R2dTransform( );
												R2dTransform( const R2dTransform& rhs );
												R2dTransform( YRealDimension a1, YRealDimension a2,
																  YRealDimension b1, YRealDimension b2,
																  YRealDimension c1, YRealDimension c2 );

	// Operations
	public :
		void									MakeIdentity( );
		void									Invert( );
		R2dTransform&						Create( const RVectorRect<YRealCoordinate>& rc1, const RVectorRect<YRealCoordinate>& rc2 );
		R2dTransform&						Create( const RRect<YRealCoordinate>& rc1, const RRect<YRealCoordinate>& rc2 );
		R2dTransform&						Create( const RRect<YRealCoordinate>& rc1, const RVectorRect<YRealCoordinate>& rc2 );
		R2dTransform&						Create( const RVectorRect<YRealCoordinate>& rc1, const RRect<YRealCoordinate>& rc2 );

		// Translation
		void									PreTranslate( YRealDimension dx, YRealDimension dy );
		void									PostTranslate( YRealDimension dx, YRealDimension dy );

		// Rotation
		void									PreRotateAboutOrigin( YAngle angle );
		void									PostRotateAboutOrigin( YAngle angle );

		void									PostRotateAboutPoint( YRealCoordinate x, YRealCoordinate y, YAngle angle );
		void									PreRotateAboutPoint( YRealCoordinate x, YRealCoordinate y, YAngle angle );
		
		// Scaling
		void									PreScale( YScaleFactor sx, YScaleFactor sy );
		void									PostScale( YScaleFactor sx, YScaleFactor sy );

		void									PreScaleAboutPoint( YRealCoordinate x, YRealCoordinate y, YScaleFactor sx, YScaleFactor sy );
		void									PostScaleAboutPoint( YRealCoordinate x, YRealCoordinate y, YScaleFactor sx, YScaleFactor sy );

		// Shearing
		void									PreXShear( YRealDimension u );
		void									PostXShear( YRealDimension u );

		void									PreXShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u );
		void									PostXShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u );

		void									PreYShear( YRealDimension u );
		void									PostYShear( YRealDimension u );

		void									PreYShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u );
		void									PostYShearAboutPoint( YRealCoordinate x, YRealCoordinate y, YRealDimension u );

		// operators
		R2dTransform						operator*( const R2dTransform& rhs ) const;
		void									operator*=( const R2dTransform& rhs );
		R2dTransform&						operator=( const R2dTransform& rhs );

		BOOLEAN								operator==( const R2dTransform& rhs ) const;
		BOOLEAN								operator!=( const R2dTransform& rhs ) const;

		BOOLEAN								AreScaleAndRotateEqual( const R2dTransform& rhs ) const;

		//	Retrieval
		BOOLEAN								Decompose( YAngle& angle, YRealDimension&	xScale, YRealDimension&	yScale ) const;
		void									GetTranslation( YRealDimension& xOffset, YRealDimension& yOffset ) const;

		static BOOLEAN						GetComponents( const RVectorRect<YRealCoordinate>&	rc1,
																	const RVectorRect<YRealCoordinate>&	rc2,
																	YAngle&										angle,
																	YRealDimension&							xScale,
																	YRealDimension&							yScale,
																	YRealDimension&							xOffset,
																	YRealDimension&							yOffset );
		static BOOLEAN						GetComponents( const RRect<YRealCoordinate>&	rc1,
																	const RRect<YRealCoordinate>&	rc2,
																	YRealDimension&					xScale,
																	YRealDimension&					yScale,
																	YRealDimension&					xOffset,
																	YRealDimension&					yOffset );
		static BOOLEAN						GetComponents( const RRect<YRealCoordinate>&			rc1,
																	const RVectorRect<YRealCoordinate>&	rc2,
																	YAngle&										angle,
																	YRealDimension&							xScale,
																	YRealDimension&							yScale,
																	YRealDimension&							xOffset,
																	YRealDimension&							yOffset );
		static BOOLEAN						GetComponents( const RVectorRect<YRealCoordinate>&	rc1,
																	const RRect<YRealCoordinate>&			rc2,
																	YAngle&										angle,
																	YRealDimension&							xScale,
																	YRealDimension&							yScale,
																	YRealDimension&							xOffset,
																	YRealDimension&							yOffset );

	// member data
	private :
		YRealCoordinate					m_a1;
		YRealCoordinate					m_a2;
		YRealCoordinate					m_b1;
		YRealCoordinate					m_b2;
		YRealCoordinate					m_c1;
		YRealCoordinate					m_c2;

	//
	// Friends
	public :
		friend FrameworkLinkage RArchive& operator<<( RArchive& archive, const R2dTransform& color );
		friend FrameworkLinkage RArchive& operator>>( RArchive& archive, R2dTransform& color );
		friend class RSize<YIntDimension>;
		friend class RSize<YRealDimension>;
		friend class RPoint<YIntDimension>;
		friend class RPoint<YRealDimension>;
		friend class RRect<YIntDimension>;
		friend class RRect<YRealDimension>;
		friend class RVectorRect<YIntDimension>;
		friend class RVectorRect<YRealDimension>;
		friend class T3dTransform;
		};

FrameworkLinkage RArchive& operator<<( RArchive& archive, const R2dTransform& transform );
FrameworkLinkage RArchive& operator>>( RArchive& archive, R2dTransform& transform );
	
// ****************************************************************************
// 					Inlines
// ****************************************************************************


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _2DTRANSFORM_H_
