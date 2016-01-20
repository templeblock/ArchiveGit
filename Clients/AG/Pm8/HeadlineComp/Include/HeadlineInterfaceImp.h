// ****************************************************************************
//
//  File Name:			HeadlineInterfaceImp.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RHeadlineInterfaceImp class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineInterfaceImp.h               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEINTERFACEIMP_H_
#define		_HEADLINEINTERFACEIMP_H_

#ifndef		_HEADLINEINTERFACE_H_
#include		"HeadlineInterface.h"
#endif

#ifndef		_HEADLINEPERSISTANTOBJECT_H_
#include		"HeadlinePersistantObject.h"
#endif

#include "SoftShadowSettings.h"

class RHeadlineView;
class RHeadlineDocument;

// ****************************************************************************
//
//  Class Name:	RGraphicHolderImp
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RHeadlineHolderImp : public RHeadlineHolder
	{
	//	Construction and Destruction
	public :
													RHeadlineHolderImp( RHeadlinePersistantObject& persistantObject );

	//	Operations
	public :

		virtual void							Read( RStorage& storage ) ;
		virtual void							Write( RStorage& storage ) const ;

		HeadlineDataStruct&					GetHeadlineData( );	//	Non-Const...

	//	Member data
	private :
		HeadlineDataStruct					m_HeadlineData;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const;
#endif
	} ;

// ****************************************************************************
//
//  Class Name:	RHeadlineInterfaceImp
//
//  Description:	A pure virtual class interfacing with a headline component
//
// ****************************************************************************
//
class RHeadlineInterfaceImp : public RHeadlineInterface
	{
	//	Construction & Destruction
	public :
													RHeadlineInterfaceImp( const RHeadlineView* pView );
		static RInterface*					CreateInterface( const RComponentView* pView );

	//	Operations
	public :
		virtual RComponentDataHolder*		GetData( );
		virtual void							EditData( );
		virtual void							SetData( RComponentDataHolder* );
		virtual void							ClearData( );
		virtual void							Read( RChunkStorage& );
		virtual void							Write( RChunkStorage& );
		virtual void							Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& );

		virtual void							Commit( );

		//	Attributes.
		virtual void							GetFontName( RMBCString& fontName );
		virtual void							GetEscapement( ETextEscapement& escapement );
		virtual void							GetFontAttributes( YFontAttributes& attributes );
		virtual void							GetJustification( ETextJustification& justification );
		virtual void							GetCompensation( ETextCompensation& compensation );
		virtual void							GetCompensationDegree( float& flCompensation );
		virtual void							GetKerning( BOOLEAN& fKerning );
		virtual void							GetLeading( BOOLEAN& fLeading );
		virtual void							GetScale1( uWORD& uwScale );
		virtual void							GetScale2( uWORD& uwScale );
		virtual void							GetScale3( uWORD& uwScale );
		virtual void							GetNoDisplayEnvelop( BOOLEAN& fNotDisplay );
		virtual void							GetNumLines( uWORD& numLines );

		virtual void							SetFontName( const RMBCString& fontName );
		virtual void							SetEscapement( ETextEscapement escapement );
		virtual void							SetFontAttributes( YFontAttributes attributes );
		virtual void							SetJustification( ETextJustification justification );
		virtual void							SetCompensation( ETextCompensation compensation );
		virtual void							SetCompensationDegree( float flCompensation );
		virtual void							SetKerning( BOOLEAN fKerning );
		virtual void							SetLeading( BOOLEAN fLeading );
		virtual void							SetScale1( uWORD uwScale );
		virtual void							SetScale2( uWORD uwScale );
		virtual void							SetScale3( uWORD uwScale );
		virtual void							SetNoDisplayEnvelop( BOOLEAN fNotDisplay );

		//	Shape 
		virtual void							GetDistortEffect( EDistortEffects& effect );
		virtual void							GetShearAngle( int& shearAngle );
		virtual void							GetRotation( int& rotation );
		virtual void							GetAltPercent( float& percent );
		virtual void							GetDistort( BOOLEAN& fDistort );
		virtual void							GetShapeIndex( int& shapeIndex );
		virtual void							GetVerticalPlacement( EVerticalPlacement& placement );

		virtual void							SetDistortEffect( EDistortEffects effect );
		virtual void							SetShearAngle( int shearAngle );
		virtual void							SetRotation( int rotation );
		virtual void							SetAltPercent( float percent );
		virtual void							SetDistort( BOOLEAN fDistort );
		virtual void							SetShapeIndex( int shapeIndex );
		virtual void							SetVerticalPlacement( EVerticalPlacement placement );

		//	Outline
		virtual void							GetOutlineFillColor( RColor& color );
		virtual void							GetOutlineEffect( EOutlineEffects& effect );
		virtual void							GetLineWeight( ELineWeight& weight );		
		virtual void							GetStrokeColor( RSolidColor& color );
		virtual void							GetShadowColor( RSolidColor& color );
		virtual void							GetHiliteColor( RSolidColor& color );
		virtual void							GetBlur2Color( RSolidColor& color );

		virtual void							SetOutlineFillColor( const RColor& color );
		virtual void							SetOutlineEffect( EOutlineEffects effect );
		virtual void							SetLineWeight( ELineWeight weight );		
		virtual void							SetStrokeColor( const RSolidColor& color );
		virtual void							SetShadowColor( const RSolidColor& color );
		virtual void							SetHiliteColor( const RSolidColor& color );
		virtual void							SetBlur2Color( const RSolidColor& color );

		//	Effects
		virtual void							GetNumStages( int& stages );
		virtual void							GetProjectionFillColor( RColor& color );
		virtual void							GetProjectionEffect( EProjectionEffects& effect );
		virtual void							GetProjectionVanishPoint( RIntPoint& vanishPt );
		virtual void							GetProjectionDepth( float& flDepth );
		virtual void							GetShadowFillColor( RColor& color );
		virtual void							GetShadowEffect( EShadowEffects& effect );
		virtual void							GetShadowVanishPoint( RIntPoint& vanishPt );
		virtual void							GetShadowDepth( float& flDepth );
		virtual void							GetShadowColor2( RSolidColor& color );

		virtual void							SetNumStages( int stages );
		virtual void							SetProjectionFillColor( const RColor& color );
		virtual void							SetProjectionEffect( EProjectionEffects effect );
		virtual void							SetProjectionVanishPoint( const RIntPoint& vanishPt );
		virtual void							SetProjectionDepth( float flDepth );
		virtual void							SetShadowFillColor( const RColor& color );
		virtual void							SetShadowEffect( EShadowEffects effect );
		virtual void							SetShadowVanishPoint( const RIntPoint& vanishPt );
		virtual void							SetShadowDepth( float flDepth );
		virtual void							SetShadowColor2( const RSolidColor& color );

		//	Behinds
		virtual void							GetBehindColor( RColor& color );
		virtual void							GetBehindEffect( EFramedEffects& effect );
		virtual void							GetBehindShadowColor( RSolidColor& color );
		virtual void							GetBehindHilite( RSolidColor& color );

		virtual void							SetBehindColor( const RColor& color );
		virtual void							SetBehindEffect( EFramedEffects effect );
		virtual void							SetBehindShadowColor( const RSolidColor& color );
		virtual void							SetBehindHilite( const RSolidColor& color );

		// Interface Data
		virtual void							GetInterfaceData( uLONG& ulData );
		virtual void							SetInterfaceData( uLONG ulData );

		//	Text Data
		virtual void							GetHeadlineText( RMBCString& text );
		virtual void							SetHeadlineText( const RMBCString& text );

	//	Private Members
	private :

		RHeadlineDocument*					m_pDocument;
		RHeadlinePersistantObject&			m_PersistantObject;
		HeadlineDataStruct					m_HeadlineData;

		RSoftShadowSettings					m_ShadowSettings;
	};

#endif	//	_HEADLINEINTERFACE_H_
