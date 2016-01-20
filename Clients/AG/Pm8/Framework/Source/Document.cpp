// ****************************************************************************
//
//  File Name:			Document.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Definition of the RDocument class
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
//  $Logfile:: /PM8/Framework/Source/Document.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "Document.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"Application.h"
#include "ApplicationGlobals.h"
#include "ChunkyStorage.h"
#include	"ComponentDocument.h"
#include	"ComponentView.h"
#include "ComponentApplication.h"
#include "FrameworkResourceIDs.h"
#include "StandaloneApplication.h"
#include "DrawingSurface.h"
#include "ComponentManager.h"
#include "OffscreenDrawingSurface.h"
#include "ImageLibrary.h"
#include "BitmapImage.h"

// Static BOOLEAN to prevent us from warning the user about missing components multiple times when loading.
BOOLEAN RDocument::m_fWarnedAboutMissingComponent = FALSE;

// Command Map
RCommandMap<RDocument, RCommandTarget> RDocument::m_CommandMap;

RCommandMap<RDocument, RCommandTarget>::RCommandMap( )
	{
	RouteCommandToFunction( (RCommandTarget* (RDocument::*)( ) const) RDocument::GetApplication );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::RDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDocument::RDocument( ) 
	: m_pApplication( NULL ),
	  m_fActive( FALSE )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::~RDocument( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDocument::~RDocument( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::Initialize( )
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::Initialize( RApplication* pApp )
	{
	m_pApplication	= pApp;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetApplication( )
//
//  Description:		Accessor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RApplication* RDocument::GetApplication( ) const
	{
	return m_pApplication;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetViewCollectionStart( )
//
//  Description:		Accessor
//
//  Returns:			An iterator suitable for iterating the view list of this
//							document. The iterator returned is at the start of the list
//
//  Exceptions:		None
//
// ****************************************************************************
//
YViewIterator RDocument::GetViewCollectionStart( ) const
	{
	return m_ViewCollection.Start( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetViewCollectionEnd( )
//
//  Description:		Accessor
//
//  Returns:			An iterator suitable for iterating the view list of this
//							document. The iterator returned is at the start of the list
//
//  Exceptions:		None
//
// ****************************************************************************
//
YViewIterator RDocument::GetViewCollectionEnd( ) const
	{
	return m_ViewCollection.End( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetActiveView( )
//
//  Description:		Accessor
//
//							REVIEW: This must be changed if multiple views are ever
//									  implemented.
//
//  Returns:			This documents currently active view. As we do not currently
//							have mulitiple views, this just returns the first view.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView* RDocument::GetActiveView( ) const
	{
	if( m_ViewCollection.Count( ) )
		return *( m_ViewCollection.Start( ) );

	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetComponentCollectionStart( )
//
//  Description:		Accessor
//
//  Returns:			An iterator suitable for iterating the component list of this
//							document. The iterator returned is at the start of the list
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentIterator RDocument::GetComponentCollectionStart( ) const
	{
	return m_ComponentCollection.Start( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetComponentCollectionEnd( )
//
//  Description:		Accessor
//
//  Returns:			An iterator suitable for iterating the component list of this
//							document. The iterator returned is at the start of the list
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentIterator RDocument::GetComponentCollectionEnd( ) const
	{
	return m_ComponentCollection.End( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetComponentWithUniqueId( )
//
//  Description:		Looks for a component with the specified unique id.
//
//  Returns:			The component, if one exists, NULL if not
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RDocument::GetComponentWithUniqueId( YUniqueId uniqueId ) const
	{
	YComponentIterator iterator = m_ComponentCollection.Start( );
	for( ; iterator != m_ComponentCollection.End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentAttributes( ).GetUniqueId( ) == uniqueId )
			return *iterator;
		}

	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::Render( )
//
//  Description:		Renders this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::Render( RDrawingSurface&, const R2dTransform&, const RIntRect&, const RRealSize& ) const
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::RenderBehindSoftEffect( )
//
//  Description:		Called to render the portion of the document that should
//							appear behind its soft effect, such as the silhouette of
//							a headline
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::RenderBehindSoftEffect( RDrawingSurface& /* drawingSurface */, const R2dTransform& /* transform */, const RIntRect& /* rcRender */, const RRealSize& /* rSize */ ) const
	{
	// do nothing
	// subclasses should override if they need to display a portion of themselves
	// behind their shadow such as the silhouette of a headline
	}

// ****************************************************************************
//
//  Function Name:	RDocument::RenderShadow( )
//
//  Description:		Renders this document's shadow
//
//							This implementation relies on the drawingSurface having
//							an offscreen bitmap but will work for ANY object type.
//							However, it's not necessarily the most efficient
//							implementation so if a subclass can implement a better
//							version it should override this one. (GCB 3/25/98)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::RenderShadow( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender, const RRealSize& rSize, const RSolidColor& shadowColor ) const
{
	RComponentView* pView = dynamic_cast<RComponentView*>(GetActiveView());
	ROffscreenDrawingSurface* destinationOffscreen = dynamic_cast<ROffscreenDrawingSurface*>(&drawingSurface);
	RBitmapImage* destinationBitmap = NULL;
	if (destinationOffscreen)
		destinationBitmap = destinationOffscreen->GetBitmapImage();

	if (pView && pView->HasBackgroundColor())
	{
		// if we have an active view and it has a behind color,
		// render rectangular shadow as though cast from the behind color
		RColor rOldFill = drawingSurface.GetFillColor( );
		drawingSurface.SetFillColor( shadowColor );
		drawingSurface.FillRectangle( RRealRect( rSize ), transform );
		drawingSurface.SetFillColor( rOldFill );
	}
	else if (!destinationBitmap)
	{
		// REVIEW GCB 3/25/98
		// if drawingSurface is not an offscreen drawing surface or it doesn't
		// have an RBitmapImage just ignore the shadowColor
		// (probably should assert but don't complain for now)
		Render( drawingSurface, transform, rcRender, rSize );
	}
	else
	{
		ROffscreenDrawingSurface*	blackOffscreen = NULL;
		ROffscreenDrawingSurface*	whiteOffscreen = NULL;
		ROffscreenDrawingSurface*	alphaOffscreen = NULL;
		RBitmapImage*					blackMask = NULL;
		RBitmapImage*					whiteMask = NULL;
		RBitmapImage*					alphaMask = NULL;

		try
			{
			// Get the bounding rect of the bitmap
			RIntSize bitmapSize( destinationBitmap->GetWidthInPixels(), destinationBitmap->GetHeightInPixels() );
			RIntRect bitmapRect( bitmapSize );

			// create the black mask, white mask and alpha mask
			blackOffscreen = new ROffscreenDrawingSurface( drawingSurface.IsPrinting() );
			whiteOffscreen = new ROffscreenDrawingSurface( drawingSurface.IsPrinting() );
			alphaOffscreen = new ROffscreenDrawingSurface( drawingSurface.IsPrinting() );
			blackMask = new RBitmapImage;
			whiteMask = new RBitmapImage;
			alphaMask = new RBitmapImage;
			blackMask->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 1);
			whiteMask->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 1);
			alphaMask->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 8, kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );
			blackOffscreen->SetImage( blackMask );
			whiteOffscreen->SetImage( whiteMask );	
			alphaOffscreen->SetImage( alphaMask );

			// init black mask to black and white mask to white
			::memset( RBitmapImage::GetImageData( blackMask->GetRawData()), 0x00, RBitmapImage::GetImageDataSize( blackMask->GetRawData() ) );
			::memset( RBitmapImage::GetImageData( whiteMask->GetRawData()), 0xFF, RBitmapImage::GetImageDataSize( whiteMask->GetRawData() ) );

			// render into the black and white masks
			Render( *blackOffscreen, transform, rcRender, rSize );
			Render( *whiteOffscreen, transform, rcRender, rSize );

			// combine blackMask and whiteMask into alphaMask
			// to ensure that pure black and pure white lines are not lost
			alphaOffscreen->BlitDrawingSurface( *blackOffscreen, bitmapRect, bitmapRect, kBlitSourceCopy );
			alphaOffscreen->BlitDrawingSurface( *whiteOffscreen, bitmapRect, bitmapRect, kBlitMergePaint );

			// blend from shadowColor, through alphaMask,
			// into destinationBitmap (which belongs to drawingSurface)
			RImageLibrary().AlphaBlend( shadowColor, *destinationBitmap, bitmapRect, *alphaMask, bitmapRect );
		}
		catch (...)
		{
			TpsAssertAlways( "an unknown error occurred - can't render shadow." );
		}

		// clean up
		if (blackOffscreen)
			blackOffscreen->ReleaseImage();
		if (whiteOffscreen)
			whiteOffscreen->ReleaseImage();
		if (alphaOffscreen)
			alphaOffscreen->ReleaseImage();
		delete blackOffscreen;
		delete whiteOffscreen;
		delete alphaOffscreen;
		delete blackMask;
		delete whiteMask;
		delete alphaMask;
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
//  Function Name:	RDocument::AddComponent( )
//
//  Description:		Adds a new component to this documents component list
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::AddComponent( RComponentDocument* pComponentDocument )
	{
	TpsAssert( pComponentDocument, "NULL Component document." );
	TpsAssertValid( pComponentDocument );

	// Set the parent of the component
	pComponentDocument->SetParentDocument( this );
	
	// Add it to the list
	m_ComponentCollection.InsertAtEnd( pComponentDocument );

	// Add each component view to its parent
	YViewIterator iterator = pComponentDocument->GetViewCollectionStart( );
	for( ; iterator != pComponentDocument->GetViewCollectionEnd( ); ++iterator )
		{
		RComponentView* pComponentView = (RComponentView*)*iterator;

		// If the view does not have a parent already, use our first view
		if( !pComponentView->GetParentView( ) )
			{
			RView* pView = GetActiveView( );
			pView->AddComponentView( pComponentView );
			}
		else
			pComponentView->GetParentView( )->AddComponentView( pComponentView );
		}

	// Invalidate the component
	pComponentDocument->InvalidateAllViews( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::RemoveComponent( )
//
//  Description:		Removes a component from this documents component list
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::RemoveComponent( RComponentDocument* pComponentDocument )
	{
	TpsAssert( pComponentDocument, "NULL Component document." );
	TpsAssertValid( pComponentDocument );
	TpsAssert( pComponentDocument->GetParentDocument( ) == this, "We are not this documents parent." );

	// Make sure its in the list
	TpsAssert( m_ComponentCollection.Find( pComponentDocument ) != m_ComponentCollection.End( ), "Attempt to remove a component document that is not a child of this document." );

	// Invalidate the component
	pComponentDocument->InvalidateAllViews( );

	// Remove it from the list
	m_ComponentCollection.Remove( pComponentDocument );

	// Remove the component views from its parent view
	YViewIterator iterator = pComponentDocument->GetViewCollectionStart( );
	for( ; iterator != pComponentDocument->GetViewCollectionEnd( ); ++iterator )
		{
		RComponentView* pView = (RComponentView*)*iterator;
		pView->GetParentView( )->RemoveComponentView( pView );
		}

	// Set the parent of the component to NULL
	pComponentDocument->SetParentDocument( NULL );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::TransferComponents( )
//
//  Description:		Transfers components from the source document to this
//							document.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::TransferComponents( RDocument* pSourceDocument,
												const RComponentCollection& componentCollection,
												const RRealRect& sourceRect,
												const RRealRect& destRect )
	{
	// Compute the scale factor necessary to put the source rect into the dest rect
	RRealSize scaleFactor( destRect.Width( ) / sourceRect.Width( ), destRect.Height( ) / sourceRect.Height( ) );

	// Compute the offset necessary to position the components
	RRealSize offset = destRect.m_TopLeft - sourceRect.m_TopLeft;

	// Iterate the collection, moving the components
	YComponentIterator iterator = componentCollection.Start( );
	for( ; iterator != componentCollection.End( ); ++iterator )
		{
		// Get pointers the the component document and view
		RComponentDocument* pDocument = *iterator;
		RComponentView* pView = static_cast<RComponentView*>( pDocument->GetActiveView( ) );

		// Remove the component from its current document
		pSourceDocument->RemoveComponent( pDocument );

		// NULL out the component views parent
		pView->SetParentView( NULL );

		// Add it to this document
		AddComponent( pDocument );

		// Reset the bounding rect
		YComponentBoundingRect boundingRect = pView->GetBoundingRect( );
		boundingRect.UnrotateAndScaleAboutPoint( sourceRect.m_TopLeft, scaleFactor );
		boundingRect.Offset( offset );
		pView->SetBoundingRect( boundingRect );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDocument::DeleteComponentCollection( )
//
//  Description:		Remove all of the component documents from this document
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::DeleteComponentCollection( )
	{
	DeleteAllItems( m_ComponentCollection );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetFontList( )
//
//  Description:		Returns the document font list.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RFontList& RDocument::GetFontList( ) const
	{
	TpsAssertIsObject( RFontList, &m_FontList );
	return m_FontList;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetFontList( )
//
//  Description:		Returns the document font list.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFontList& RDocument::GetFontList( )
	{
	TpsAssertIsObject( RFontList, &m_FontList );
	return m_FontList;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetURLList( )
//
//  Description:		Returns the document URL list.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RURLList& RDocument::GetURLList( ) const
	{
	TpsAssertIsObject( RURLList, &m_URLList );
	return m_URLList;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetURLList( )
//
//  Description:		Returns the document URL list.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RURLList& RDocument::GetURLList( )
	{
	TpsAssertIsObject( RURLList, &m_URLList );
	return m_URLList;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::FreeDocumentContents( )
//
//  Description:		Free all of the document contents. Default is to call all
//							of the collected documents and free them and delete them.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::FreeDocumentContents( )
	{
	// Tell all embedded documents to free their contents
	YComponentIterator iterator = m_ComponentCollection.Start( );
	for( ; iterator != m_ComponentCollection.End( ); ++iterator )
		( *iterator )->FreeDocumentContents( );

	// Now delete them
	DeleteComponentCollection( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::AddView( )
//
//  Description:		Adds a new view to this document
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::AddRView( RView* pView )
	{
	m_ViewCollection.InsertAtStart( pView );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::AddView( )
//
//  Description:		Adds a new view to this document
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RDocument::DeleteRView( RView* pView )
	{
	TpsAssertValid( pView );

	// Make sure its in our view list
	TpsAssert( m_ViewCollection.Find( pView ) != m_ViewCollection.End( ), "Attempt to remove a view that is not in this document." );

	// Remove and delete the view
	m_ViewCollection.Remove( pView );
	delete pView;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RDocument::CreateScriptAction( const YActionId& actionId, RScript& script )
	{
	// give the app a chance.
	return GetApplication( )->CreateScriptAction( actionId, script );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::InvalidateAllViews( )
//
//  Description:		Invalidate all of the views associated with this document.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::InvalidateAllViews( BOOLEAN fErase )
	{
	YViewIterator iterator = GetViewCollectionStart( );
	for( ; iterator != GetViewCollectionEnd( ); ++iterator )
		( *iterator )->Invalidate( fErase );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetViewIndex( )
//
//  Description:		Gets the index number of the specified view
//
//  Returns:			The index
//
//  Exceptions:		None
//
// ****************************************************************************
//
YViewIndex RDocument::GetViewIndex( RView* pView ) const
	{
	return ::GetIndex( m_ViewCollection, pView );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetViewFromIndex( )
//
//  Description:		Gets the view pointer corresponding to this index
//
//  Returns:			The view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView* RDocument::GetViewByIndex( YViewIndex index ) const
	{
	RView* temp;
	::GetByIndex( m_ViewCollection, index, temp );
	return temp;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetComponentIndex( )
//
//  Description:		Gets the index number of the specified component
//
//  Returns:			The index
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentIndex RDocument::GetComponentIndex( RComponentDocument* pComponent ) const
	{
	return ::GetIndex( m_ComponentCollection, pComponent );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetComponentFromIndex( )
//
//  Description:		Gets the component pointer corresponding to this index
//
//  Returns:			The component
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RDocument::GetComponentByIndex( YComponentIndex index ) const
	{
	RComponentDocument* temp = NULL;

	if( index < m_ComponentCollection.Count( ) )
		::GetByIndex( m_ComponentCollection, index, temp );

	return temp;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::Write( )
//
//  Description:		Writes this documents data to the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RDocument::Write( RChunkStorage& storage, EWriteReason reason ) const
	{
	// Write the components
	m_ComponentCollection.Write( storage, reason );

	// Write the views
	storage.AddChunk( 'VIEW' );

	YViewIterator viewIterator = m_ViewCollection.Start( );
	for( ; viewIterator != m_ViewCollection.End( ); ++viewIterator )
		( *viewIterator )->Write( storage );

	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::Read( )
//
//  Description:		Reads from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RDocument::Read( RChunkStorage& storage, EReadReason reason )
	{
	//
	//		If there is any necessary information that needs to be saved,
	//		Search for the new chuck and read it...

	// Iterate through the components
	RChunkStorage::RChunkSearcher searcher = storage.Start( kComponent, FALSE );

	for( ;!searcher.End( ); ++searcher )
		{
		// Read in the component type
		YComponentType componentType;
		storage >> componentType;

		// Check to see if the component is available
		if( !::GetComponentManager( ).IsComponentAvailable( componentType ) )
			{
			// Only warn the user if we havent done so already
			if( !m_fWarnedAboutMissingComponent )
				{
				RAlert( ).AlertUser( STRING_ERROR_LOAD_COMPONENT_MISSING );
				m_fWarnedAboutMissingComponent = TRUE;
				}
			}

		// If the component is available, load it
		else
			{
			// Create a component of the appropriate type
			RComponentDocument* pDocument = ::GetComponentManager( ).CreateNewComponent( componentType, this, RComponentAttributes( ), TRUE );

			// Now tell that component to read
			pDocument->Read( storage, reason );

			// Add the component
			AddComponent( pDocument );
			}
		}

	storage.Start( 'VIEW', !kRecursive );

	// Read our views
	YViewIterator iterator = m_ViewCollection.Start( );
	for( ; iterator != m_ViewCollection.End( ); ++iterator )
		( *iterator )->Read( storage );

	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::CanIncorporateData( )
//
//  Description:		Determines if this document can incorporate the data
//							available from the specified data source.
//
//  Returns:			TRUE if data can be incorporated
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDocument::CanIncorporateData( const RDataTransferSource& ) const
	{
	// Default to FALSE
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::XUpdateAllViews( )
//
//  Description:		Notify all views for this document that the given update has occured.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::XUpdateAllViews( EUpdateTypes updateType, uLONG lParam )
	{
	YViewIterator	iterator		= GetViewCollectionStart( );
	YViewIterator	iteratorEnd	= GetViewCollectionEnd( );
	for ( ; iterator != iteratorEnd; ++iterator )
		(*iterator)->XUpdateAllViews( updateType, lParam );
	}

// ****************************************************************************
//
//  Function Name:	RDocument::OnXActivate( )
//
//  Description:		Default handler for the activate message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::OnXActivate( BOOLEAN fActive )
	{
	m_fActive = fActive;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::Active( )
//
//  Description:		Return Active state of document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDocument::IsActive( ) const
	{
	return m_fActive;
	}

// ****************************************************************************
//
//  Function Name:	RDocument::GetNumComponents( )
//
//  Description:		Accessor
//
//  Returns:			The number of components in this document
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RDocument::GetNumComponents( ) const
	{
	return m_ComponentCollection.Count( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDocument::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDocument::Validate( )	const
	{
	RObject::Validate( );
	TpsAssertIsObject( RDocument, this );
	}

#endif	// TPSDEBUG
