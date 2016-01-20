// ****************************************************************************
//
//  File Name:			PathDocument.h
//
//  Project:			Path Component
//
//  Author:				Lance Wilson
//
//  Description:		Declaration of the RPathDocument class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PathComp/Include/PathDocument.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PathDocument_H_
#define		_PathDocument_H_

#include "Path.h"
#include "PathInterface.h"
#include "PropertyInterface.h"
#include "ComponentDocument.h"

// ****************************************************************************
//
//  Class Name:	RPathDocument
//
//  Description:	The Path document.
//
// ****************************************************************************
//
class RPathDocument : public RComponentDocument
{
// Construction & Destruction
public :
									RPathDocument( RApplication* pApp,
														 const RComponentAttributes& componentAttributes,
														 const YComponentType& componentType,
														 BOOLEAN fLoading );
	virtual						~RPathDocument( );

// Operations
public :

	virtual RComponentView*	CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView );

	virtual void				ResetToDefaultSize( RDocument* pParentDocument );
	virtual void 				Render( RDrawingSurface& drawingSurface,
											  const R2dTransform& transform,
											  const RIntRect& rcRender,
											  const RRealSize& size ) const;
	void 							Render( RDrawingSurface& drawingSurface,
											  const R2dTransform& transform,
											  const RIntRect& rcRender,
											  const RRealSize& size,
											  const RColor& crFillColor, 
											  YFillMethod yFillMethod,
											  const RSolidColor& crOutlineColor,
											  YPenWidth yPenWidth ) const;
	virtual void 				RenderShadow( RDrawingSurface& drawingSurface,
											  const R2dTransform& transform,
											  const RIntRect& rcRender,
											  const RRealSize& size,
											  const RSolidColor& shadowColor ) const;
	virtual void				Read( RChunkStorage& storage, EReadReason reason );
	virtual void				Write( RChunkStorage& storage, EWriteReason reason ) const;

protected:

	YPenWidth					GetPenWidth( RDrawingSurface& drawingSurface ) const;

	void							ResizeView( RRealSize rNewViewSize, BOOLEAN fFitInsidePanel );

// Member data
private:

	RPath							m_rPath;	
	OUTLINE_FORMAT				m_sOutlineFormat;
	RColor						m_rFillColor;

	friend class RPathInterfaceImp;
	friend class RPathObjectPropertiesImp;

// Debugging support
#ifdef	TPSDEBUG
public :
	virtual void				Validate( ) const;
#endif
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _PathDocument_H_
