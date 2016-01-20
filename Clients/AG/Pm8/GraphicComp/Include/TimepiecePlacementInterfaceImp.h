// ****************************************************************************
//
//  File Name:			TimepiecePlacementInterfaceImp.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RTimepiecePlacementInterfaceImp class
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
// ****************************************************************************

#ifndef	_TIMEPIECEPLACEMENTINTERFACEIMP_H_
#define	_TIMEPIECEPLACEMENTINTERFACEIMP_H_

#include	"TimepiecePlacementInterface.h"

class RGraphicView;
class RGraphicDocument;
class RPsd3TimepieceGraphic;

class RTimepiecePlacementInterfaceImp : public RTimepiecePlacementInterface
	{
	//	Construction & Destruction
	public :
													RTimepiecePlacementInterfaceImp( const RGraphicView* pView );
		static	RInterface*					CreateInterface( const RComponentView* pView );

	//	Operations
	public :
		virtual const RRealRect&			GetTimepiecePlacement( ) const;

	// Data members
	private:
		RGraphicDocument*						m_pGraphicDocument;
		RPsd3TimepieceGraphic*				m_pTimepiece;
	} ;

#endif	//	_GRAPHICINTERFACEIMP_H_
