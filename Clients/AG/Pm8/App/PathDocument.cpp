// ****************************************************************************
//
//  File Name:			PathDocument.cpp
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
//  $Logfile:: /PM8/App/PathDocument.cpp                                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:08p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include	"PathDocument.h"
#include	"PathView.h"
#include "ChunkyStorage.h"
#include "DrawingSurface.h"

const YChunkTag	kPathChunkID =			'PATH';

// ****************************************************************************
//
//  Function Name:	RPathDocument::RPathDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPathDocument::RPathDocument( RApplication* pApp,
										  const RComponentAttributes& componentAttributes,
										  const YComponentType& componentType,
										  BOOLEAN fLoading )
	: RComponentDocument( pApp, componentAttributes, componentType, fLoading )
{
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::~RPathDocument()
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RPathDocument::~RPathDocument()
{
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::CreateView( )
//
//  Description:		Creates a new view on this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RPathDocument::CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView )
	{
	return new RPathView( boundingRect, this, pParentView );
	}

// ****************************************************************************
//
//  Function Name:	RPathDocument::Render( )
//
//  Description:		Called to render the documents data into an output target
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RPathDocument::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size ) const
{	
	// Lack of foresight in the path component requires us to 
	// remove the const from the this pointer before calling
	// the paths render function.
	const_cast<RPathDocument *>( this )->m_rPath.Render( 
		drawingSurface, size, transform, rcRender );

	YDrawMode yOldDrawMode = drawingSurface.GetForegroundMode( );
	YPenStyle yOldPenStyle = drawingSurface.GetPenStyle( );
	drawingSurface.SetForegroundMode( kXNOR );
	drawingSurface.SetPenStyle( kDotPen );

	// Lack of foresight in the path component requires us to 
	// remove the const from the this pointer before calling
	// the paths render function.
	const_cast<RPathDocument *>( this )->m_rPath.Render( 
		drawingSurface, size, transform, rcRender );

	drawingSurface.SetForegroundMode( yOldDrawMode );
	drawingSurface.SetPenStyle( yOldPenStyle );
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::Read()
//
//  Description:		Read in a path from storage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RPathDocument::Read( RChunkStorage& rStorage, EReadReason eReason )
{	
	RComponentDocument::Read( rStorage, eReason );

	// Search for path chunk and instantiate appropriately	
	RChunkStorage::RChunkSearcher rSearcher;
	rSearcher = rStorage.Start(kPathChunkID, !kRecursive);

	if (!rSearcher.End())
	{
		// Read the path
		m_rPath.Read( rStorage );

		rStorage.SelectParentChunk();
	}	
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::Write()
//
//  Description:		Write out a path to storage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RPathDocument::Write(RChunkStorage& rStorage, EWriteReason eReason) const
{
	RComponentDocument::Write( rStorage, eReason );

	rStorage.AddChunk(kPathChunkID);
	m_rPath.Write( rStorage );
	rStorage.SelectParentChunk();
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::SetPath()
//
//  Description:		Sets a COPY of the path in rPath into the component.   
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RPathDocument::SetPath( RPath& rPath, R2dTransform& transform, BOOLEAN fResize )
{
//	R2dTransform identity;

	m_rPath.Undefine();
	m_rPath.Append( rPath, transform );

	if (fResize)
	{
		RIntSize rNewViewSize = m_rPath.GetPathSize();
		RComponentView* pComponentView = static_cast<RComponentView*>(
			GetActiveView());	

		// Resize the view
		pComponentView->Resize( rNewViewSize );
	}
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::GetPath()
//
//  Description:		Returns a pointer to the path
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RPath* RPathDocument::GetPath( )
{
	return &m_rPath;
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::ResetToDefaultSize()
//
//  Description:		Resets this component to be the default size for the
//							specified parent document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathDocument::ResetToDefaultSize(RDocument*)
{
	RIntSize rNewViewSize = m_rPath.GetPathSize();
	RComponentView* pComponentView = static_cast<RComponentView*>(
		GetActiveView());	

	// Resize the view
	pComponentView->Resize( rNewViewSize );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPathDocument::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathDocument::Validate( ) const
	{
	RComponentDocument::Validate( );
	TpsAssertIsObject( RPathDocument, this );
	}
									
#endif	//	TPSDEBUG
