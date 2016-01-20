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
//  $Logfile:: /PM8/PathComp/Source/PathDocument.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "PathIncludes.h"
ASSERTNAME

#include	"PathDocument.h"
#include	"PathView.h"
#include "ChunkyStorage.h"
#include "DrawingSurface.h"

const YChunkTag	kVersionChunkID  =			'VERS';
const YChunkTag	kPathChunkID     =			'PATH';
const YChunkTag	kPropertyChunkID =			'PROP';

const uWORD kVersionMajor = 1;
const uWORD kVersionMinor = 0;

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
	, m_rFillColor( RSolidColor( kBlack ) )
{
	memset( &m_sOutlineFormat, 0, sizeof( m_sOutlineFormat ) );
	m_sOutlineFormat.m_yPenColor = RSolidColor( kBlack );
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
	YFillMethod yFillMethod = (IOutlineAttrib::kNone == m_sOutlineFormat.m_uwPenStyle ? kFilled : kFilledAndStroked);
	YPenWidth yPenWidth = GetPenWidth( drawingSurface ); 

	Render( drawingSurface, transform, rcRender, size, m_rFillColor, yFillMethod, m_sOutlineFormat.m_yPenColor, yPenWidth );
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
void RPathDocument::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& size, const RColor& crFillColor, YFillMethod yFillMethod, const RSolidColor& crOutlineColor, YPenWidth yPenWidth ) const
{	
	RDrawingSurfaceState	curState( &drawingSurface );

	//	Prepare the drawing surface for imaging the headling..
	YTint	tint= GetTint();
	if (tint != kMaxTint)
		drawingSurface.SetTint(tint);

	// Adjust the path size and transform
	// to account for the outline going half 
	// out of the path.
	R2dTransform xform( transform );
	xform.PreTranslate( yPenWidth / 2.0, yPenWidth / 2.0 );
	RRealSize szAdjustedSize( size.m_dx - yPenWidth, size.m_dy - yPenWidth );

	// Convert the pen width to device units,
	// preventing it from going below 1
	RRealSize szPenWidth( yPenWidth, yPenWidth );
	szPenWidth *= transform;
	yPenWidth = Max( ::Round( szPenWidth.m_dx ), 1 );


	// Render the path
	////////////////////

	const_cast<RPathDocument *>( this )->m_rPath.Render( 
		drawingSurface, szAdjustedSize, xform, rcRender, 
		yFillMethod, crOutlineColor, crFillColor, yPenWidth );

}

// ****************************************************************************
//
//  Function Name:	RPathDocument::RenderShadow( )
//
//  Description:		Called to render the documents shadow
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathDocument::RenderShadow( RDrawingSurface& drawingSurface,
														const R2dTransform& transform,
														const RIntRect& rcRender,
														const RRealSize& size,
														const RSolidColor& shadowColor ) const
{
	RComponentView* pView = dynamic_cast<RComponentView*>(GetActiveView());
	if (pView && pView->HasBackgroundColor())
	{
		// if we have an active view and it has a behind color,
		// render rectangular shadow as though cast from the behind color
		RColor rOldFill = drawingSurface.GetFillColor( );
		drawingSurface.SetFillColor( shadowColor );
		drawingSurface.FillRectangle( RRealRect( size ), transform );
		drawingSurface.SetFillColor( rOldFill );
	}
	else
	{
		RColor rColor( shadowColor );
		YPenWidth yPenWidth = GetPenWidth( drawingSurface ); 
		YFillMethod yFillMethod = (IOutlineAttrib::kNone == m_sOutlineFormat.m_uwPenStyle ? kFilled : kFilledAndStroked);
	
		Render( drawingSurface, transform, rcRender, size, rColor, yFillMethod, shadowColor, yPenWidth );
	}

	if (pView && pView->GetFrameType() != kNoFrame && !pView->HasBackgroundColor())
	{
		// if we have an active view and it has a frame,
		// render shadow as though cast from the frame
		pView->RenderFrame( drawingSurface, transform, shadowColor );
	}
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

	// Search for version information
	//
	RChunkStorage::RChunkSearcher rSearcher;
	rSearcher = rStorage.Start( kVersionChunkID, !kRecursive );

	uWORD uwMajor = kVersionMajor, uwMinor = kVersionMinor;

	if (!rSearcher.End())
	{
		rStorage >> uwMajor;
		rStorage >> uwMinor;

		rStorage.SelectParentChunk();
	}

	// Search for path information	
	//
	rSearcher = rStorage.Start(kPathChunkID, !kRecursive);

	if (!rSearcher.End())
	{
		// Read the path
		m_rPath.Read( rStorage );

		rStorage.SelectParentChunk();
	}	

	// Search for property information	
	//
	rSearcher = rStorage.Start(kPropertyChunkID, !kRecursive);

	if (!rSearcher.End())
	{
		rStorage >> m_rFillColor;

		rStorage >> m_sOutlineFormat.m_uwPenStyle;
		rStorage >> m_sOutlineFormat.m_uwLineWeight;
		rStorage >> m_sOutlineFormat.m_yPenColor;

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

	rStorage.AddChunk( kVersionChunkID );
	rStorage << kVersionMajor;
	rStorage << kVersionMinor;
	rStorage.SelectParentChunk();

	rStorage.AddChunk(kPathChunkID);
	m_rPath.Write( rStorage );
	rStorage.SelectParentChunk();

	rStorage.AddChunk( kPropertyChunkID );
	rStorage << m_rFillColor;

	rStorage << m_sOutlineFormat.m_uwPenStyle;
	rStorage << m_sOutlineFormat.m_uwLineWeight;
	rStorage << m_sOutlineFormat.m_yPenColor;

	rStorage.SelectParentChunk();
}

// ****************************************************************************
//
//  Function Name:	RPathDocument::ResizeView()
//
//  Description:		Make sure that the image fits on the panel
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RPathDocument::ResizeView( RRealSize rNewViewSize, BOOLEAN fFitInsidePanel )
{
	RComponentView* pComponentView = static_cast<RComponentView*>(GetActiveView());	

	if( pComponentView->GetParentView( ) && fFitInsidePanel )
	{
		// Compare object size to panel size
		RRealSize rPanelSize = pComponentView->GetParentView( )->GetSize();
		if (rNewViewSize.m_dx > rPanelSize.m_dx || rNewViewSize.m_dy > rPanelSize.m_dy) rNewViewSize.FitInside(rPanelSize);	
	}

	// Resize the view
	pComponentView->Resize( rNewViewSize );
}

/*
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
*/

// ****************************************************************************
//
//  Function Name:	RPathDocument::GetPenWidth()
//
//  Description:		Determines the pen width to use for rendering the outline
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPenWidth RPathDocument::GetPenWidth( RDrawingSurface& drawingSurface ) const
{
	const int oneLogicalPoint = 20;

	// Compute the pen width
	/////////////////////////////////

	// Get one device unit into logical units
	RIntSize oneUnit( 1, 1 );
	DeviceUnitsToLogicalUnits( oneUnit, drawingSurface );

	YPenWidth yPenWidth = 0; 
	
	switch (m_sOutlineFormat.m_uwPenStyle)
	{
		case IOutlineAttrib::kNone:
			yPenWidth = 0;
			break;

		case IOutlineAttrib::kHairLine:
			yPenWidth = oneUnit.m_dx;
			break;

		case IOutlineAttrib::kThin: 
		case IOutlineAttrib::kMedium:
		case IOutlineAttrib::kThick:
		case IOutlineAttrib::kExtraThick:

			// TODO: add predefined line weight support.
			break;

		case IOutlineAttrib::kFixed:

			// Set the pen width in logical units
			yPenWidth = m_sOutlineFormat.m_uwLineWeight * oneLogicalPoint;
			break;
	}

	return yPenWidth;
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
