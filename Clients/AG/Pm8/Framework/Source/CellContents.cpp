// ****************************************************************************
// @Doc
// @Module CellContents.cpp |
//
//  File Name:			CellContents.cpp
//
//  Project:			Calendar Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Implementation of the RCellContents and supporting classes
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
//  $Logfile:: /PM8/Framework/Source/CellContents.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"CellContents.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"Grid.h"
#include "DrawingSurface.h"
#include "BufferStream.h"
#include "ComponentManager.h"

//
// static template member initialization
YComponentType			RCellText::m_gComponentType( kTextComponent );
YInterfaceId			RCellText::m_gInterfaceId( kTextInterfaceId );
uLONG						RCellText::m_gNumItems			= 0;
RComponentView*		RCellText::m_gpView				= NULL;
RComponentDocument*	RCellText::m_gpDocument			= NULL;
BOOLEAN					RCellText::m_gfDocViewInUse	= FALSE;
RCriticalSection		RCellText::m_gCriticalSection;

const YChunkTag	kTempTag	= 'TemP';

//
// @MFunc Construct an RCellContents
// @Syntax	RCellContents()
// @Syntax	RCellContents( const RCellContents& rhs );
// @Parm	const RCellContents& | rhs | const reference to the RCellContents to copy
// @RDesc nothing
//
RCellContents::RCellContents() :
	m_nCellIndex( 0 ),
	m_ulFlags( 0 ),
	m_Text(),
	m_pSquareView( NULL ),
	m_SquareComponentId( 0 )
{
 ;
}

RCellContents::RCellContents( const RCellContents& rhs ) :
	m_nCellIndex( 0 ),
	m_ulFlags( 0 ),
	m_Text(),
	m_pSquareView( NULL ),
	m_SquareComponentId( 0 )
{
	*this = rhs;
}

RCellContents::~RCellContents( )
{
	if ( m_pSquareView )
	{
		RComponentDocument*	pDocument	= m_pSquareView->GetComponentDocument( );
		if ( pDocument->GetParentDocument( ) )
			pDocument->GetParentDocument( )->RemoveComponent( pDocument );
		delete pDocument;
	}
}

//
// @MFunc operator=
// @RDesc const ref to this object
//
const RCellContents& RCellContents::operator=(
			const RCellContents&	rhs )	// @Parm const reference to the object to copy
{
	if( &rhs != this )
		{
		RBufferStream		stream;
		RStorage				parentStorage( &stream );
		RChunkStorage		storage( &parentStorage );
		//
		//		Start the write
		storage.AddChunk( kTempTag );
		//
		//	Write out the component data
		rhs.Write( storage );
		//
		//		Back to our parent
		storage.SelectParentChunk( );
		//
		//	Set the position back to 0
		storage.SeekAbsolute( 0 );
		//
		// Read the chunk
		storage.Start( kTempTag, !kRecursive );
		//
		//	Read from the storage
		Read( storage );
		//
		//		Back to our parent
		storage.SelectParentChunk( );
		}
	return *this;
}

//
// @MFunc Read the contents from the given storage
// @RDesc nothing
//
void RCellContents::Read(
			RChunkStorage&	storage )	// @Parm the storage to read from
{
	uWORD	uwTemp;
	storage	>> uwTemp
				>> m_ulFlags;
	m_nCellIndex	= int(uwTemp);
	m_Text.Read( storage );
	//
	//	Write out if we have a component or not.
	uBYTE	ubHaveComponent;
	storage >> ubHaveComponent;

	storage >> m_SquareComponentId;

	uLONG	ulPad		= 0;
	storage >> ulPad;
	//
	// If we have a component, write it out
	if( ubHaveComponent == 1 )
	{
		//
		// Write out the component type
		YComponentType	yComponentType;
		storage >> yComponentType;
		//
		//	Create the appropriate component
		//
		RComponentDocument* pDocument = ::GetComponentManager().CreateNewComponent( yComponentType, NULL, RComponentAttributes(), FALSE );
		//
		//	Let it read itself
		pDocument->Read( storage, kLoading );
		//
		// Assign the view
		m_pSquareView = static_cast<RComponentView*>( pDocument->GetActiveView( ) );
	}
}

//
// @MFunc Write the contents to the given storage
// @RDesc nothing
//
void RCellContents::Write(
			RChunkStorage&	storage )	// @Parm the storage to write to
			const
{
	storage	<< uWORD(m_nCellIndex)
				<< m_ulFlags;
	//
	//	Write out the text information
	m_Text.Write( storage );
	//
	//	Write out if we have a component or not.
	uBYTE	ubHaveComponent = (m_pSquareView)? 1 : 0;
	storage << ubHaveComponent;

	storage << m_SquareComponentId;

	uLONG	ulPad		= 0;
	storage << ulPad;
	//
	// If we have a component, write it out
	if ( ubHaveComponent )
	{
		//
		// Write out the component type
		YComponentType	yComponentType = m_pSquareView->GetComponentDocument( )->GetComponentType( );
		storage << yComponentType;
		//
		//	Write out the comonent data
		m_pSquareView->GetComponentDocument( )->Write( storage, kSaving );
	}
}


//
// @MFunc Render the contents to the given drawing surface
// @RDesc nothing
//
void RCellContents::Render(
			RDrawingSurface&		ds,					// @Parm the drawing surface
			const R2dTransform&	transform,			// @Parm the base transform
			const RGrid*			pGrid,						// @Parm the grid the cell is from
			const RRealSize&		nominalGridSize,	// @Parm the nominal size of the grid
			const RRealRect&		finalCellRect )	// @Parm the final cell rect
			const
{
	//
	// Compute the final grid size based on the given cell rectangle's dimensions
	RRealSize		finalGridSize( pGrid->ComputeGridDimensions(finalCellRect.WidthHeight()) );
	//
	// Get the rectangle the cell would normally be in given the final grid size
	RRealRect		defaultCellRect( pGrid->GetItemRect(finalGridSize,kGridCellInterior,m_nCellIndex) );
	//
	// Compute the final output bounding rectangle
	RRealRect		rcRender( (RRealVectorRect(finalGridSize)*transform).m_TransformedBoundingRect );
	//
	// Create a transformation to get us from the default cell rectangle to the target rectangle
	R2dTransform	xform( transform );
	xform.PreScale( finalCellRect.Width()/defaultCellRect.Width(), finalCellRect.Height()/defaultCellRect.Height() );
	xform.PreTranslate( finalCellRect.m_Left-defaultCellRect.m_Left, finalCellRect.m_Top-defaultCellRect.m_Top );
	//
	//	If we have a pComponentView, let it render
	if( m_pSquareView )
		{
		//
		//	Get the Graphic Rect
		RRealRect	rcGraphicPosition( pGrid->GetItemRect(nominalGridSize,kGridCellContentSquare,m_nCellIndex) );
		//
		// Compute the nominal size of the graphic
		RRealSize	sizeNominalGraphic( rcGraphicPosition.WidthHeight( ) );
		//
		// Get the final output rectangle for the graphic
		RRealRect	rcGraphic( pGrid->GetItemRect(finalGridSize,kGridCellContentSquare,m_nCellIndex) );
		//
		//	Set the views position
		m_pSquareView->SetBoundingRect( rcGraphicPosition );
		//
		//	Get a New transform
		R2dTransform	squareXform( transform );
		squareXform.PreTranslate( rcGraphic.m_Left, rcGraphic.m_Top );
		//
		// Render the graphic - I am not sure about this...
		m_pSquareView->Render( ds, squareXform, rcRender );
		}

	//
	// Compute the nominal size of the text
	RRealSize	sizeNominalText( pGrid->GetItemRect(nominalGridSize,kGridCellContentText,m_nCellIndex).WidthHeight() );
	//
	// Get the final output rectangle for the text
	RRealRect	rcText( pGrid->GetItemRect(finalGridSize,kGridCellContentText,m_nCellIndex) );
	//
	// Render the text
	if( ds.IsVisible(rcText,xform) )
		m_Text.Render( ds, xform, rcRender, sizeNominalText, rcText );
}

