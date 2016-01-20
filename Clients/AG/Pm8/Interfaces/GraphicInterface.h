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
//  $Logfile:: /PM8/Interfaces/GraphicInterface.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GRAPHICINTERFACE_H_
#define		_GRAPHICINTERFACE_H_

#ifndef		_COMPONENTDATAINTERFACE_H_
#include		"ComponentDataInterface.h"
#endif

const	YInterfaceId	kGraphicInterfaceId			= 60;
const	YInterfaceId	kGraphicColorInterfaceId	= 61;

class IBufferInterface;

// ****************************************************************************
//
//  Class Name:	RGraphicHolder
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RGraphicHolder : public RComponentDataHolder
	{
	//	Construction and Destruction
	public :
													RGraphicHolder( ) { ; };
		virtual									~RGraphicHolder( ) { ; };
	} ;

// ****************************************************************************
//
//  Class Name:	RGraphicInterface
//
//  Description:	A pure virtual class interfacing with a text component
//
// ****************************************************************************
//
class RGraphicInterface : public RComponentDataInterface
	{
	// Public types
	public :
		enum EGraphicType { kSquare, kRow, kColumn, kSeal, kBorder, kLine, kBackdrop, 
			kHorzBannerBackdrop, kVertBannerBackdrop, kTiledBannerBackdrop, kTimepiece,
			kNavButtonSet, kIconButton, kBullet, kPageDivider, kAnimation, kWebBackground, 	//KDM 10/22/97 LeapFrog
			kCGM };

	//	Construction & Destruction
	public :
													RGraphicInterface( const RComponentView* pView ) : RComponentDataInterface( pView ) { ; }

	//	Operations
	public :
		virtual RComponentDataHolder*		GetData( ) = 0;
		virtual RComponentDataHolder*		GetData( BOOLEAN fRemove ) = 0;
		virtual void							EditData( ) = 0;
		virtual void							SetData( RComponentDataHolder* ) = 0;
		virtual void							ClearData( ) = 0;
		virtual void							Load( RStorage& storage, EGraphicType graphicType ) = 0;
		virtual void							Load( RStorage& storage, uLONG uLength, EGraphicType graphicType ) = 0;
		virtual void							Load( IBufferInterface* pBufferedData, EGraphicType graphicType, BOOLEAN fLinkData = FALSE ) = 0;
		virtual void							LoadFromPretzelCollection( RStorage& storage, EGraphicType graphicType ) = 0;
		virtual void							Read( RChunkStorage& ) = 0;
		virtual void							Write( RChunkStorage& ) = 0;
		virtual void							Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& ) = 0;
	} ;

// ****************************************************************************
//
//  Class Name:	RGraphicColorInterface
//
//  Description:	A pure virtual class interfacing with a graphic component
//
// ****************************************************************************
//
class RGraphicColorInterface : public RInterface
	{
	//	Construction & Destruction
	public :
													RGraphicColorInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Operations
	public :
		virtual BOOLEAN						IsMonochrome( ) const = 0;
		virtual RColor							GetColor( ) const = 0;
		virtual void							SetColor( const RColor& ) = 0;
	} ;


#endif	//	_GRAPHICINTERFACE_H_
