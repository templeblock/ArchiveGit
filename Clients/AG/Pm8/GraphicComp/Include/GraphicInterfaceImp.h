// ****************************************************************************
//
//  File Name:			GraphicInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RGraphicInterface class
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
//  $Logfile:: /PM8/GraphicComp/Include/GraphicInterfaceImp.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GRAPHICINTERFACEIMP_H_
#define		_GRAPHICINTERFACEIMP_H_

#include	"GraphicInterface.h"
#include "PropertyInterface.h"
#include "DateTimeInterface.h"
#include "DateTime.h"

class RGraphicDocument;
class RGraphicView;
class RGraphic;
class RPsd3SingleGraphic;
class RPsd3TimepieceGraphic;

// ****************************************************************************
//
//  Class Name:	RGraphicHolderImp
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RGraphicHolderImp : public RGraphicHolder
	{
	//	Construction and Destruction
	public :
													RGraphicHolderImp( RGraphic* pGraphic, BOOLEAN fOwnData = FALSE  );
		virtual									~RGraphicHolderImp( );

	//	Operations
	public :
		RPsd3SingleGraphic*					GetGraphic( ) const;
		const RColor&							GetColor( ) const;

	//	Member data
	private :
		RPsd3SingleGraphic*					m_pGraphic;
		RColor									m_Color;


#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const;
#endif
	} ;

// ****************************************************************************
//
//  Class Name:	RGraphicInterface
//
//  Description:	A pure virtual class interfacing with a Graphic component
//
// ****************************************************************************
//
class RGraphicInterfaceImp : public RGraphicInterface
	{
	//	Construction & Destruction
	public :
													RGraphicInterfaceImp( const RGraphicView* pView );
		static	RInterface*					CreateInterface( const RComponentView* pView );

	//	Operations
	public :
		virtual RComponentDataHolder*		GetData( );
		virtual RComponentDataHolder*		GetData( BOOLEAN fRemove );
		virtual void							EditData( );
		virtual void							SetData( RComponentDataHolder* pHolder );
		virtual void							ClearData( );
		virtual void							Load( RStorage& storage, uLONG uLength, EGraphicType graphicType );
		virtual void							Load( RStorage& storage, EGraphicType graphicType );
		virtual void							Load( IBufferInterface* pBufferedData, EGraphicType graphicType, BOOLEAN fLinkData = FALSE );
		virtual void							LoadFromPretzelCollection( RStorage& storage, EGraphicType graphicType );
		virtual void							Read( RChunkStorage& pStorage );
		virtual void							Write( RChunkStorage& pStorage );
		virtual void							Render( RDrawingSurface& drawingSurface, 
															  const R2dTransform& transform, 
															  const RIntRect& rcRender, 
															  const RIntRect& rcLocation );

	//	Private Members
	private :
		RGraphicDocument*						m_pGraphicDocument;
		RPsd3SingleGraphic*					m_pGraphic;
	} ;

// ****************************************************************************
//
//  Class Name:	RGraphicColorInterface
//
//  Description:	A pure virtual class interfacing with a graphic component
//
// ****************************************************************************
//
class RGraphicColorInterfaceImp : public RGraphicColorInterface
	{
	//	Construction & Destruction
	public :
													RGraphicColorInterfaceImp( const RGraphicView* pView );
		static	RInterface*					CreateInterface( const RComponentView* pView );

	//	Operations
	public :
		virtual BOOLEAN						IsMonochrome( ) const;
		virtual RColor							GetColor( ) const;
		virtual void							SetColor( const RColor& color );

	//	Data Members
	private :
		RGraphicView*							m_pView;

	};

// ****************************************************************************
//
//  Class Name:	RGraphicDateTimeInterfaceImp
//
//  Description:	
//
// ****************************************************************************
//
class RGraphicDateTimeInterfaceImp : public RDateTimeInterface
	{
	//	Construction & Destruction
	public :
													RGraphicDateTimeInterfaceImp( const RGraphicView* pView );
		static	RInterface*					CreateInterface( const RComponentView* pView );

	//	Operations
	public :
		virtual const RDateTime&			GetDateTime( ) const;
		virtual void							SetDateTime( const RDateTime&, BOOLEAN );
		virtual YResourceId					GetDateFormatId( ) const;

	//	Data Members
	private :
		const RGraphicView*					m_pView;
		RPsd3TimepieceGraphic*				m_pTimepiece;
		RDateTime								m_rDateTime;
	};

// ****************************************************************************
//
//  Class Name:	RGraphicPropertiesImp
//
//  Description:	Definition of the RGraphicPropertiesImp class.
//
// ****************************************************************************
//
class RGraphicPropertiesImp : public IObjectProperties
	{
	//	Construction & Destruction
	public :
										RGraphicPropertiesImp( const RComponentView* pView );
		static RInterface*		CreateInterface( const RComponentView* pView );

	public:

		virtual BOOLEAN			FindProperty( YInterfaceId iid, RInterface** ppInterface );
	//	virtual void				EnumProperties(  );

	private:

		RGraphicDocument*			m_pDocument;
	};

#endif	//	_GRAPHICINTERFACEIMP_H_
