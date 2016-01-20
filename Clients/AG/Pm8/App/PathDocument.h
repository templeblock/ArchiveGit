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
//  $Logfile:: /PM8/App/PathDocument.h                                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:08p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PathDocument_H_
#define		_PathDocument_H_

#include "Path.h"
#include "ComponentDocument.h"

const YComponentType kPathComponent( "Path" );

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
	virtual								~RPathDocument( );

// Operations
public :

	virtual RComponentView*			CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView );

	virtual void						ResetToDefaultSize( RDocument* pParentDocument );
	virtual void 						Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size ) const;
	virtual void						Read( RChunkStorage& storage, EReadReason reason );
	virtual void						Write( RChunkStorage& storage, EWriteReason reason ) const;
	virtual void						SetPath( RPath& rPath, R2dTransform& transform, BOOLEAN fResize = TRUE );
	RPath*								GetPath( );

// Member data
protected :
	RPath									m_rPath;	
	
// Debugging support
#ifdef	TPSDEBUG
public :
	virtual void						Validate( ) const;
#endif
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _PathDocument_H_
