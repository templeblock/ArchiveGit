// ****************************************************************************
//
//  File Name:			GraphicDocument.h
//
//  Project:			Graphic Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGraphicDocument class
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
//  $Logfile:: /PM8/GraphicComp/Include/GraphicDocument.h                     $
//   $Author:: Rgrenfel                                                       $
//     $Date:: 3/08/99 1:11p                                                  $
// $Revision:: 2                                                              $
//
// ****************************************************************************

#ifndef		_GRAPHICDOCUMENT_H_
#define		_GRAPHICDOCUMENT_H_

#include		"ComponentDocument.h"
#include		"Graphic.h"

class RUndoableAction;

// ****************************************************************************
//
//  Class Name:	RGraphicDocument
//
//  Description:	The Graphic document
//
// ****************************************************************************
//
class RGraphicDocument : public RComponentDocument
{

friend class RGraphicPropertiesImp;

// Construction & Destruction
public :
												RGraphicDocument( RApplication* pApp, const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading );
	virtual									~RGraphicDocument( );

// Operations
public :
	virtual void 							Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size ) const;
	virtual void 							RenderShadow( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size, const RSolidColor& shadowColor ) const;
	virtual void							Read( RChunkStorage& storage, EReadReason reason );
	virtual void							Write( RChunkStorage& storage, EWriteReason reason ) const;
	virtual void							GetDataFormatsSupportedForCopy( YDataFormatCollection& collection ) const;
	virtual void							Copy( YDataFormat dataFormat, RDataTransferTarget& dataTransferTarget );
	virtual void							Paste( YDataFormat dataFormat, const RDataTransferSource& dataTransferSource );		
	virtual void							ResetToDefaultSize( RDocument* pParentDocument );
	virtual BOOLEAN						GetDefaultSize( const RRealSize &sizePanel, RRealSize *psizeDefault );
	virtual RComponentView*				CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView );	
	virtual RGraphic*						SetGraphic( RGraphic* pGraphic );
	RGraphic*								GetGraphic( ) const;
	virtual BOOLEAN						IsFrameable( ) const;

	virtual void							SetLinkedData( BOOLEAN fLinkData );

// Member data
protected :
	RGraphic*								m_pGraphic;	
	RSolidColor								m_rTintColor;
	BOOLEAN									m_fLinkedData;

// Debugging support
#ifdef	TPSDEBUG
public :
	virtual void							Validate( ) const;
#endif
};

#endif		// _GRAPHICDOCUMENT_H_
