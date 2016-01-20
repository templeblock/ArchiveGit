// ****************************************************************************
//
//  File Name:			ComponentCollection.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RComponentCollection class
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
//  $Logfile:: /PM8/Framework/Source/ComponentCollection.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ComponentCollection.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "OffscreenDrawingSurface.h"
#include "BitmapImage.h"
#include "VectorImage.h"
#include "DataTransfer.h"
#include "ChunkyStorage.h"
#include "StandaloneApplication.h"
#include "CompositeSelection.h"
#include "BufferStream.h"
#include "ComponentManager.h"
#include "ComponentTypes.h"

// ****************************************************************************
//
//  Function Name:	RComponentCollection::RComponentCollection( )
//
//  Description:		Default constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentCollection::RComponentCollection( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::RComponentCollection( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentCollection::RComponentCollection( const RCompositeSelection& selection )
	{
	// Initialize this component collection with the components in the specified selection
	YSelectionIterator iterator = selection.Start( );
	for( ; iterator != selection.End( ); ++iterator )
		InsertAtEnd( static_cast<RComponentDocument*>( ( *iterator )->GetRDocument( ) ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::RComponentCollection( )
//
//  Description:		Copy constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentCollection::RComponentCollection( const RComponentCollection& rhs, RDocument* pParentDocument, BOOLEAN& fObjectConverted )
	{
	// Create a memory storage
	RBufferStream stream;
	RStorage	parentStorage( &stream );
	RChunkStorage storage( &parentStorage );

	// Write the components to the storage
	rhs.Write( storage, kSaving );

	// Seek back to the start of the storage
	storage.SeekAbsolute( 0 );

	// Read the components out of the storage
	fObjectConverted = Read( storage, pParentDocument );
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::GetBoundingRect( )
//
//  Description:		Calculates the bounding rect of this component collection
//
//  Returns:			The bounding rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RComponentCollection::GetBoundingRect( ) const
	{
	TpsAssert( Count( ) > 0, "No components" );

	YComponentIterator iterator = Start( );

	// Use the first rectangle as the starting point, and union from there
	TpsAssertIsObject( RComponentView, ( *iterator )->GetActiveView( ) );
	RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );
	RRealRect boundingRect = pComponentView->GetFullSoftEffectsBoundingRect( ).m_TransformedBoundingRect;
	++iterator;

	for( ; iterator != End( ); ++iterator )
		{
		// Get the active view of the document
		TpsAssertIsObject( RComponentView, ( *iterator )->GetActiveView( ) );
		RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

		// Intersect with the current bounding rect
		boundingRect.Union( boundingRect, pComponentView->GetFullSoftEffectsBoundingRect( ).m_TransformedBoundingRect );
		}

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::GetTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rectangle within which tracking feedback
//							will be drawn.
//
//  Returns:			Bounding rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RComponentCollection::GetTrackingFeedbackBoundingRect( const R2dTransform& transform )
	{
	TpsAssert( Count( ) > 0, "No components" );

	YComponentIterator iterator = Start( );

	// Use the first rectangle as the starting point, and union from there
	TpsAssertIsObject( RComponentView, ( *iterator )->GetActiveView( ) );
	RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );
	R2dTransform tempTransform = pComponentView->GetFullSoftEffectsBoundingRect( ).GetTransform( ) * transform;
	RRealRect boundingRect = pComponentView->GetTrackingFeedbackBoundingRect( tempTransform );
	++iterator;

	for( ; iterator != End( ); ++iterator )
		{
		// Get the active view of the document
		TpsAssertIsObject( RComponentView, ( *iterator )->GetActiveView( ) );
		pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

		tempTransform = pComponentView->GetFullSoftEffectsBoundingRect( ).GetTransform( ) * transform;

		// Intersect with the current bounding rect
		boundingRect.Union( boundingRect, pComponentView->GetTrackingFeedbackBoundingRect( tempTransform ) );
		}

	return boundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::GetSizeLimits( )
//
//  Description:		Computes the maximum and minimum scale factors that may
//							be applied to this collection
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::GetSizeLimits( RRealSize& minScaleFactor, RRealSize& maxScaleFactor ) const
	{
	// Initialize the scale factors
	minScaleFactor.m_dx = minScaleFactor.m_dy = 0;
	maxScaleFactor.m_dx = maxScaleFactor.m_dy = kFloatMax;

	// Iterator the collection
	for( YComponentIterator iterator = Start( ); iterator != End( ); ++iterator )
		{
		// Skip the component if it is not resizable
		if( ( *iterator )->GetComponentAttributes( ).IsResizable( ) )
			{
			// Get the active view of the document
			TpsAssertIsObject( RComponentView, ( *iterator )->GetActiveView( ) );
			RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

			// Get the current size of the component
			RRealSize currentSize = pComponentView->GetBoundingRect( ).WidthHeight( );

			// Get the min and max sizes of the component
			RRealSize minSize = pComponentView->GetMinimumSize( );
			RRealSize maxSize = pComponentView->GetMaximumSize( );

			// Calculate the minimum and maximum scale factors that may be applyed to the component
			RRealSize localMinScaleFactor( minSize.m_dx / currentSize.m_dx, minSize.m_dy / currentSize.m_dy );
			RRealSize localMaxScaleFactor( maxSize.m_dx / currentSize.m_dx, maxSize.m_dy / currentSize.m_dy );

			// If this components min scale factor is greater than the current min, use it
			if( localMinScaleFactor.m_dx > minScaleFactor.m_dx )
				minScaleFactor.m_dx = localMinScaleFactor.m_dx;

			if( localMinScaleFactor.m_dy > minScaleFactor.m_dy )
				minScaleFactor.m_dy = localMinScaleFactor.m_dy;

			// If this components max scale factor is less than the current max, use it
			if( localMaxScaleFactor.m_dx < maxScaleFactor.m_dx )
				maxScaleFactor.m_dx = localMaxScaleFactor.m_dx;

			if( localMaxScaleFactor.m_dy < maxScaleFactor.m_dy )
				maxScaleFactor.m_dy = localMaxScaleFactor.m_dy;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::CombineAttributes( )
//
//  Description:		Ands the attributes of the components in this collection
//							with the specified attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::CombineAttributes( RComponentAttributes& attributes ) const
	{
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		attributes &= ( *iterator )->GetComponentAttributes( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::GetBitmapRepresentation( )
//
//  Description:		Images all components in this collection to a bitmap and
//							returns it. The caller adopts the bitmap.
//
//  Returns:			The bitmap
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RBitmapImage* RComponentCollection::GetBitmapRepresentation( ) const
	{
	TpsAssert( Count( ) > 0, "No components." );

	// Get the bounding rect of the components
	RIntRect boundingRect = GetBoundingRect( );

	// Create a bitmap big enough to hold the components
	RIntSize bitmapSize = boundingRect.WidthHeight( );
	::LogicalUnitsToScreenUnits( bitmapSize );

	RBitmapImage* pBitmapImage = NULL;
	ROffscreenDrawingSurface drawingSurface;

	try
		{
		pBitmapImage = new RBitmapImage;
		pBitmapImage->Initialize(bitmapSize.m_dx, bitmapSize.m_dy, 24);

		// Set the bitmap
		drawingSurface.SetImage(pBitmapImage);

		// Fill in the background
		drawingSurface.SetFillColor( RColor( kWhite ) );
		drawingSurface.FillRectangle( RIntSize( bitmapSize.m_dx + 1, bitmapSize.m_dy + 1 ) );

		// Render the collection
		Render( drawingSurface );
		}

	catch( ... )
		{
		drawingSurface.ReleaseImage( );
		delete pBitmapImage;
		throw;
		}

	drawingSurface.ReleaseImage();
	return pBitmapImage;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::GetPictureRepresentation( )
//
//  Description:		Images all components in this collection to a picture and
//							returns it. The caller adopts the picture.
//
//  Returns:			The bitmap
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RVectorImage* RComponentCollection::GetPictureRepresentation( ) const
	{
	TpsAssert( Count( ) > 0, "No components." );

	// Get the bounding rect of the components
	RIntRect boundingRect = GetBoundingRect( );

	// Create a picture big enough to hold the components
	RIntSize pictureSize = boundingRect.WidthHeight( );
	RVectorImage* pVectorImage = NULL;
	ROffscreenDrawingSurface drawingSurface( TRUE, FALSE );

	try
		{
		pVectorImage = new RVectorImage;
		pVectorImage->SetSuggestedWidthInInches(::LogicalUnitsToInches(pictureSize.m_dx));
		pVectorImage->SetSuggestedHeightInInches(::LogicalUnitsToInches(pictureSize.m_dy));
		
		// Set the image
		drawingSurface.SetImage(pVectorImage);

		// Render the collection
		Render( drawingSurface );
		}

	catch( ... )
		{
		drawingSurface.ReleaseImage( );
		delete pVectorImage;
		throw;
		}

	drawingSurface.ReleaseImage( );
	return pVectorImage;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::Render( )
//
//  Description:		Renders this collection to a drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RComponentCollection::Render( RDrawingSurface& drawingSurface ) const
	{
	RIntRect boundingRect = GetBoundingRect( );

	// Get a transform
	R2dTransform transform;

	// Multiply in a translation operation to position the group
	RIntSize offset( boundingRect.m_TopLeft.m_x, boundingRect.m_TopLeft.m_y );
	::LogicalUnitsToDeviceUnits( offset, drawingSurface );
	transform.PreTranslate( -offset.m_dx, -offset.m_dy );
								
	// Iterate the components, drawing them
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		// Get the component and its view
		RComponentDocument* pDocument = *iterator;
		TpsAssertIsObject( RComponentView, pDocument->GetActiveView( ) );
		RComponentView* pView = static_cast<RComponentView*>( pDocument->GetActiveView( ) );
	
		// Make a copy of the transform and add this components transform to it
		R2dTransform tempTransform = transform;
		pView->GetViewTransform( tempTransform, drawingSurface, FALSE );

		// position the data correctly within the collection bounds
#if GREG_REVIEW_HERE
		RRealVectorRect collectionBoundingRect( GetBoundingRect() );
		collectionBoundingRect *= tempTransform;
		tempTransform.PostTranslate(
			-collectionBoundingRect.m_TransformedBoundingRect.m_Left,
			-collectionBoundingRect.m_TransformedBoundingRect.m_Top );
#else
		//	Since the render process is going to multiply in the component transform,
		//		we need to invert the bounding rect and multiply it so that it does not
		//		get accounted for 2 times (1 by get view and the other by pView->RenderComponent)
		R2dTransform	rCompXForm	= pView->GetBoundingRect().GetTransform( );
		rCompXForm.Invert( );
		tempTransform	= rCompXForm * tempTransform;
#endif
		// Render the component
		pView->RenderComponentWithSoftEffects( pView, drawingSurface, tempTransform, RIntRect( boundingRect.WidthHeight( ) ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::DrawDragTrackingRect( )
//
//  Description:		Draws dragging feedback for all components in this
//							collection
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::DrawDragTrackingRect( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RView& targetView ) const
	{
	// Iterate the component collection, drawing the "active" view of each
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		if( ( *iterator )->GetComponentAttributes( ).IsMovable( ) )
			{
			RComponentView* pComponentView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );
			R2dTransform tempTransform = pComponentView->GetBoundingRect( ).GetTransform( ) * transform;
			pComponentView->RenderTrackingFeedback( drawingSurface, tempTransform, targetView );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::DeleteAllComponents( )
//
//  Description:		Deletes all component in this collection
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::DeleteAllComponents( )
	{
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		(*iterator)->FreeDocumentContents( );
		delete *iterator;
		}

	Empty( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::Copy( )
//
//  Description:		Copies this collections data to the specified data target
//							in the specified format
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::Copy( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const
	{
	RWaitCursor waitCursor;

	if( dataFormat == kComponentFormat )
		CopyComponents( dataTarget );

	else if( dataFormat == kBitmapFormat || dataFormat == kDibFormat )
		CopyBitmap( dataTarget, dataFormat );

	else if( dataFormat == kPictureFormat || dataFormat == kEnhancedMetafileFormat )
		CopyPicture( dataTarget, dataFormat );

	else if( dataFormat == kTextFormat )
		CopyText( dataTarget );

	else if( dataFormat == kRTFFormat )
		CopyRTF( dataTarget );

	else
		TpsAssertAlways( "Unsupported data format." );
	}

// ****************************************************************************
//																												 
//  Function Name:	RComponentCollection::CopyComponents( )
//
//  Description:		Copies this compoment collection to the data target, in
//							component form
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::CopyComponents( RDataTransferTarget& dataTarget ) const
	{
	// Create a memory storage
	RBufferStream stream;
	RStorage	parentStorage( &stream );
	RChunkStorage storage( &parentStorage );

	// Write the components to the storage
	Write( storage, kSaving );

	// Get the size and data of the storage
	YStreamLength size;
	PUBYTE pubBuffer = storage.GetCurrentChunkBuffer( size );

	// Set the data
	dataTarget.SetData( kComponentFormat, pubBuffer, size );
	storage.ReleaseBuffer( pubBuffer );
	}

// ****************************************************************************
//																												 
//  Function Name:	RComponentCollection::Write( )
//
//  Description:		Writes this compoment collection to a storage
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::Write( RChunkStorage& storage, EWriteReason reason ) const
	{
	// Iterator the collection, writing each component to the stream
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		// Create a new chunk for the component
		storage.AddChunk( kComponent );

		// Write out the component type
		YComponentType	componentType = ( *iterator )->GetComponentType();

		if (componentType == kPhotoFrameComponent)
		 	componentType = kPSDPhotoComponent;

		storage << componentType;

		// Write the component itself
		( *iterator )->Write( storage, reason );

		// Get back to the top level
		storage.SelectParentChunk( );
		}
	}

// ****************************************************************************
//																												 
//  Function Name:	RComponentCollection::Read( )
//
//  Description:		Reads this compoment collection from a storage
//
//  Returns:			TRUE if any components were converted to metafiles
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentCollection::Read( RChunkStorage& storage, RDocument* pParentDocument )
	{
	BOOLEAN fObjectConverted = FALSE;

	// Iterate through the components, creating new ones and adding them to a list
	RChunkStorage::RChunkSearcher searcher = storage.Start( kComponent, FALSE );

	for( ;!searcher.End( ); ++searcher )
		{
		// Read in the component type
		YComponentType componentType;										
		storage >> componentType;

		// Only read the component if its component server is available
		if( ::GetComponentManager( ).IsComponentAvailable( componentType ) )
			{
			// Create a component of the appropriate type
			RComponentDocument* pDocument = ::GetComponentManager( ).CreateNewComponent( componentType, pParentDocument, RComponentAttributes( ), TRUE );

			// Now tell that component to read
			pDocument->Read( storage, kPasting );

			// NULL out the parent of the component view; its pretty much useless
			TpsAssertIsObject( RComponentView, pDocument->GetActiveView( ) );
			static_cast<RComponentView*>( pDocument->GetActiveView( ) )->SetParentView( NULL );

			// If the parent document can insert a component of this type, reset the attributes and insert it
			if( !pParentDocument || pParentDocument->CanInsertComponent( componentType ) )
				{
				// Pasted components always come in unlocked
				RComponentAttributes componentAttributes = pDocument->GetComponentAttributes( );
				componentAttributes.SetCanChangeLockState( TRUE );
				componentAttributes.SetLocked( FALSE );
				pDocument->SetComponentAttributes( componentAttributes );

				// Add the component to our list
				InsertAtEnd( pDocument );
				}

			// Otherwise, convert the component into a metafile and use it instead
			else
				{
				// Create a memory transfer
				RMemoryDataTransfer dataTransfer;

				// Copy the component to the memory transfer as a metafile
				pDocument->Copy( kEnhancedMetafileFormat, dataTransfer );

				// Paste it into a component
				if( PasteDataIntoComponent( dataTransfer, pParentDocument, kEnhancedMetafileFormat ) )
					{
					// Get the view of the original component and the new component
					RComponentView*		pOldView = static_cast<RComponentView*>( pDocument->GetActiveView( ) );
					YComponentIterator	iterator	= End( );
					RComponentView*		pNewView = static_cast<RComponentView*>( ( *(--iterator) )->GetActiveView( ) );

					// We need to maintain the original spacial relationship between the components, so move
					// the new view so that its center point is the same as the old view
					pNewView->Offset( pOldView->GetBoundingRect( ).GetCenterPoint( ) - pNewView->GetBoundingRect( ).GetCenterPoint( ) );

					// Now resize so we are the same size
					pNewView->Resize( pOldView->GetBoundingRect( ).m_TransformedBoundingRect.WidthHeight( ) );

					// Free and delete the original component
					pDocument->FreeDocumentContents( );
					delete pDocument;

					// Remember that we converted an object
					fObjectConverted = TRUE;
					}
				}
			}
		}

	return fObjectConverted;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::CopyText( )
//
//  Description:		Retrieves text data from all components which support
//							text format, and concatinates the data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::CopyText( RDataTransferTarget& dataTarget ) const
	{
	RMBCString string;

	// Iterate the collection, looking for components which support text
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		// Get the data formats supported by this component
		YDataFormatCollection dataFormatCollection;
		( *iterator )->GetDataFormatsSupportedForCopy( dataFormatCollection );

		// Look for kTextFormat
		if( dataFormatCollection.Find( kTextFormat ) != dataFormatCollection.End( ) )
			{
			// This component supports text. Create a data target and get the text
			RMemoryDataTransfer dataTransfer;
			( *iterator )->Copy( kTextFormat, dataTransfer );

			// Get the data
			uBYTE* pData;
			YDataLength dataLength;
			dataTransfer.GetData( kTextFormat, pData, dataLength );

			// Make sure the last character is NULL
			TpsAssert( *( pData + dataLength - 1 ) == '\0', "Text must be NULL terminated." );

			// Append this data to the string
			string += RMBCString( pData );

			// Delete the data
			delete [] pData;
			}
		}

	// Copy the string
	string.Copy( dataTarget, kTextFormat );
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::CopyRTF( )
//
//  Description:		Retrieves RTF data from all components which support
//							RTF format, and concatinates the data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::CopyRTF( RDataTransferTarget& dataTarget ) const
	{
#ifdef _WINDOWS
	RRTFConcatinator concatinator;

	// Iterate the collection, looking for components which support RTF
	YComponentIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		{
		// Get the data formats supported by this component
		YDataFormatCollection dataFormatCollection;
		( *iterator )->GetDataFormatsSupportedForCopy( dataFormatCollection );

		// Look for kRTFFormat
		if( dataFormatCollection.Find( kRTFFormat ) != dataFormatCollection.End( ) )
			{
			// This component supports text. Create a data target and get the text
			RMemoryDataTransfer dataTransfer;
			( *iterator )->Copy( kRTFFormat, dataTransfer );

			// Paste it into the concatinator
			concatinator.Paste( dataTransfer );
			}
		}

	concatinator.Copy( dataTarget );
#else
	UnimplementedCode( );
#endif	// _WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::CopyBitmap( )
//
//  Description:		Copies this collection to the specified data target as
//							a bitmap
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::CopyBitmap( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const
	{
	RBitmapImage* pBitmapImage = NULL;

	try
		{
		pBitmapImage = GetBitmapRepresentation( );
		pBitmapImage->Copy( dataTarget, dataFormat );
		}

	catch( YException exception )
		{
		::ReportException( exception );
		}

	delete pBitmapImage;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::CopyPicture( )
//
//  Description:		Copies this collection to the specified data target as
//							a picture
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::CopyPicture( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const
	{
	RVectorImage* pVectorImage = NULL;

	try
		{
		pVectorImage = GetPictureRepresentation( );
		pVectorImage->Copy( dataTarget, dataFormat );
		}

	catch( YException exception )
		{
		::ReportException( exception );
		}

	delete pVectorImage;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::PromiseFormats( )
//
//  Description:		Promises the data formats that this collection can support
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::PromiseFormats( RDataTransferTarget& dataTarget ) const
	{
	YDataFormatCollection dataFormatCollection;

	// If there is only one component, just use its supported data formats
	if( Count( ) == 1 )
		( *Start( ) )->GetDataFormatsSupportedForCopy( dataFormatCollection );

	// If there are multiple components, we support picture, bitmap, and possible text
	else
		{
		dataFormatCollection.InsertAtEnd( kEnhancedMetafileFormat );
		dataFormatCollection.InsertAtEnd( kPictureFormat );
		dataFormatCollection.InsertAtEnd( kDibFormat );
		dataFormatCollection.InsertAtEnd( kBitmapFormat );

		BOOLEAN fNonTextFound = FALSE;
		BOOLEAN fTextFound = FALSE;
		BOOLEAN fNonRTFFound = FALSE;
		BOOLEAN fRTFFound = FALSE;

		YComponentIterator iterator = Start( );
		for( ; iterator != End( ); ++iterator )
			{
			// Ask the component for its supported data formats
			YDataFormatCollection tempFormatCollection;
			( *iterator )->GetDataFormatsSupportedForCopy( tempFormatCollection );

			// Look for text format
			if( tempFormatCollection.Find( kTextFormat ) != tempFormatCollection.End( ) )
				fTextFound = TRUE;
			else
				fNonTextFound = TRUE;

			// Look for RTF format
			if( tempFormatCollection.Find( kRTFFormat ) != tempFormatCollection.End( ) )
				fRTFFound = TRUE;
			else
				fNonRTFFound = TRUE;
			}

		if( fRTFFound )
			if( !fNonRTFFound )
				{
				if( fTextFound )
					dataFormatCollection.InsertAtStart( kTextFormat );
				dataFormatCollection.InsertAtStart( kRTFFormat );
				}

			else
				{
				dataFormatCollection.InsertAtEnd( kRTFFormat );
				if( fTextFound )
					dataFormatCollection.InsertAtEnd( kTextFormat );
				}

		else
			if( fTextFound )
				if( !fNonTextFound )
					dataFormatCollection.InsertAtStart( kTextFormat );
				else
					dataFormatCollection.InsertAtEnd( kTextFormat );
		}

	// For each supported format, promise the data
	YDataFormatIterator dataFormatIterator = dataFormatCollection.Start( );
	for( ; dataFormatIterator != dataFormatCollection.End( ); ++dataFormatIterator )
		dataTarget.PromiseData( *dataFormatIterator );
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::Paste( )
//
//  Description:		Retrieves data from a data transfer source
//
//  Returns:			TRUE if any components were converted to metafiles
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentCollection::Paste( const RDataTransferSource& dataSource, RDocument* pParentDocument )
	{
	RWaitCursor waitCursor;

	YDataFormat dataFormat;
	BOOLEAN fFormatHandled = FALSE;

	// Enumerate the formats available and take the first one we can handle.
	dataSource.BeginEnumFormats( );

	while( fFormatHandled == FALSE && dataSource.GetNextFormat( dataFormat ) == TRUE )
		{
		// If our internal component format is available, create new components from it
		if( dataFormat == kComponentFormat )
			{
			BOOLEAN fComponentConverted = CreateComponents( dataSource, pParentDocument );
			if( Count( ) > 0 )
				return fComponentConverted;
			}

		// Otherwise, see if we can create a component to handle the format
		else
			fFormatHandled = PasteDataIntoComponent( dataSource, pParentDocument, dataFormat );
		}

	return FALSE;
	}

// ****************************************************************************
//																												 
//  Function Name:	RComponentCollection::CreateComponents( )
//
//  Description:		Reads a list of components in our internal component format
//
//  Returns:			TRUE if any components were converted to metafiles
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentCollection::CreateComponents( const RDataTransferSource& dataSource, RDocument* pParentDocument )
	{
	// Make sure there really is component data available
	TpsAssert( dataSource.IsFormatAvailable( kComponentFormat ), "Data not available in component format." );

	uBYTE* pData = NULL;
	BOOLEAN fReturn = FALSE;

	try
		{
		// Get the component data
		YDataLength dataLength;
		dataSource.GetData( kComponentFormat, pData, dataLength );

		// Make a storage out of this memory
		RChunkStorage storage( dataLength, pData );

		// Read the components from the storage
		fReturn = Read( storage, pParentDocument );
		}

	catch( YException )
		{
		delete [] pData;
		throw;
		}																																

	delete [] pData;
	return fReturn;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::PasteDataIntoComponent( )
//
//  Description:		Creates a component of the appropriate type, and pastes
//							data into it.
//
//  Returns:			TRUE if the paste operation was successful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentCollection::PasteDataIntoComponent( const RDataTransferSource& dataSource, RDocument* pParentDocument, YDataFormat format )
	{
	const RRealSize kDefaultObjectSize( ::InchesToLogicalUnits( 2 ), ::InchesToLogicalUnits( 2 ) );

	// Get a component type which can handle this data
	YComponentType componentType;
	if( ::GetComponentManager( ).MapDataFormatToComponentType( format, componentType ) )
		{
		RComponentDocument* pDocument = NULL;

		try
			{
			// Found a component type. Create one and tell it to paste.
			pDocument = ::GetComponentManager( ).CreateNewComponent( componentType,	pParentDocument, RComponentAttributes( ), TRUE );

			if( pDocument )
				{
				// Get a size and parent view for the component
				RRealSize componentSize;
				RView* pParentView;

				if( pParentDocument )
					{
					componentSize = pParentDocument->GetDefaultObjectSize( componentType );
					pParentView	= pParentDocument->GetActiveView( );
					}

				else
					{
					componentSize = kDefaultObjectSize;
					pParentView = NULL;
					}

				// Create and add a view for the component
				RComponentView* pComponentView = pDocument->CreateView( componentSize, pParentView );
				pDocument->AddRView( pComponentView );

				// Do the paste
				pDocument->Paste( format, dataSource );

				// NULL out the parent of the component view; its pretty much useless
				pComponentView->SetParentView( NULL );

				// Add the component to our list
				InsertAtEnd( pDocument );

				return TRUE;
				}
			}

		catch( YException exception )
			{
			delete pDocument;

			if( exception != kNoTextError )
				::ReportException( exception );
			}
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::ScaleToFitInView
//
//  Description:		Scales the components in the collection to fit within the
//							specified view.
//
//  Returns:			The scale factor that was applied
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentCollection::ScaleToFitInView( RView* pTargetView )
	{
	RRealSize scaleFactor( 1.0, 1.0 );

	// Get the size of the view
	RRealSize size = pTargetView->GetSize( );

	// Get the bounding rect of this collection
	RRealRect boundingRect = GetBoundingRect( );

	// Get the size of this collection
	RRealSize collectionSize = boundingRect.WidthHeight( );

	// Get the center point of this collection
	RRealPoint centerPoint = boundingRect.GetCenterPoint( );

	// If the size of the collection is larger than the specified size in any
	// dimension, we have to scale.
	if( collectionSize.m_dx > size.m_dx || collectionSize.m_dy > size.m_dy )
		{
		scaleFactor.m_dx = scaleFactor.m_dy = ::Min( size.m_dx / collectionSize.m_dx, size.m_dy / collectionSize.m_dy );

		// Iterate the component collection scaling each components view
		for( YComponentIterator iterator = Start( ); iterator != End( ); ++iterator )
			{
			// Get the view
			RComponentView* pView = static_cast<RComponentView*>( ( *iterator )->GetActiveView( ) );

			// Ask the target view to scale the component
			pTargetView->ScaleComponentToFit( pView, centerPoint, scaleFactor );
			}
		}

	return scaleFactor;
	}

// ****************************************************************************
//
//  Function Name:	::CompareComponents( )
//
//  Description:		Compares two components for z-order. Used by the
//							sort routine below.
//
//  Returns:			1 if component 1 is is higher in z-order than component 2
//							-1 if component 1 is is lower in z-order than component 2
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
int CompareComponents( RComponentDocument* pComponent1, RComponentDocument* pComponent2 )
	{
	// Get the component views
	RComponentView* pComponentView1 = static_cast<RComponentView*>( pComponent1->GetActiveView( ) );
	RComponentView* pComponentView2 = static_cast<RComponentView*>( pComponent2->GetActiveView( ) );

	// Make sure they have the same non NULL parent
	TpsAssert( pComponentView1->GetParentView( ) == pComponentView2->GetParentView( ), "Parent views must be equal." );
	TpsAssert( pComponentView1->GetParentView( ) != NULL, "And NULL doesnt count!" );

	// Get the z-order position of each
	RView* pParentView = pComponentView1->GetParentView( );
	YComponentZPosition zPosition1 = pParentView->GetComponentZPosition( pComponentView1 );
	YComponentZPosition zPosition2 = pParentView->GetComponentZPosition( pComponentView2 );

	return zPosition1 > zPosition2;
	}

// ****************************************************************************
//
//  Function Name:	RComponentCollection::SortByZOrder
//
//  Description:		Sorts this component collection by z-order. Each component
//							must have only one view, and all views must have the same
//							parent.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentCollection::SortByZOrder( )
	{
	Sort( Start( ), End( ), CompareComponents );
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RRTFConcatinator::RRTFConcatinator
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRTFConcatinator::RRTFConcatinator( )
	{
	m_RichEditControl.Create( ES_MULTILINE, CRect( 0, 0, 200, 200 ), CWnd::GetDesktopWindow( ), 0 );
	}

// ****************************************************************************
//
//  Function Name:	RRTFConcatinator::Paste( )
//
//  Description:		Callback to stream data into an RTF control
//
//  Returns:			The number of bytes copied to the buffer, or zero if the
//							transfer is complete. 
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRTFConcatinator::Paste( const RDataTransferSource& dataSource )
	{
	// Get the data
	dataSource.GetData( kRTFFormat, m_pData, m_DataLength );
	m_CurrentPosition = 0;

	// Stream the data into the RTF control
	EDITSTREAM editStream;
	editStream.dwCookie = reinterpret_cast<DWORD>( this );
	editStream.pfnCallback = StreamInCallback;

	m_RichEditControl.SetSel( 0, 0 );

	m_RichEditControl.StreamIn( SF_RTF | SFF_SELECTION, editStream );
	}

// ****************************************************************************
//
//  Function Name:	RRTFConcatinator::Paste( )
//
//  Description:		Callback to stream data into an RTF control
//
//  Returns:			The number of bytes copied to the buffer, or zero if the
//							transfer is complete. 
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRTFConcatinator::Copy( RDataTransferTarget& dataTarget )
	{
	// Stream the data out of the RTF control
	EDITSTREAM editStream;
	editStream.dwCookie = reinterpret_cast<DWORD>( this );
	editStream.pfnCallback = StreamOutCallback;

	m_String.Empty( );

	m_RichEditControl.StreamOut( SF_RTFNOOBJS, editStream );

	// Copy the CString to the data target
	dataTarget.SetData( kRTFFormat, (uBYTE*)(LPCTSTR)m_String, m_String.GetLength( ) + 1 );
	}

// ****************************************************************************
//
//  Function Name:	RRTFConcatinator::StreamInCallback( )
//
//  Description:		Callback to stream data into an RTF control
//
//  Returns:			The number of bytes copied to the buffer, or zero if the
//							transfer is complete. 
//
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD CALLBACK RRTFConcatinator::StreamInCallback( DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb )
	{
	// Get the pointer to the concatinator that started the transfer
	RRTFConcatinator* pConcatinator = reinterpret_cast<RRTFConcatinator*>( dwCookie );

	// Copy the requested amount of data into the buffer
	*pcb = min( cb, (LONG)( pConcatinator->m_DataLength - pConcatinator->m_CurrentPosition ) );
	memcpy( pbBuff, pConcatinator->m_pData, *pcb );

	// Update the current position
	pConcatinator->m_CurrentPosition += *pcb;

	// If the transfer is done, return 0
	if( pConcatinator->m_CurrentPosition == pConcatinator->m_DataLength )
		return 0;

	// Otherwise, return the number of bytes read
	return *pcb;
	}

// ****************************************************************************
//
//  Function Name:	RRTFConcatinator::StreamOutCallback( )
//
//  Description:		Callback to stream data out if an RTF control
//
//  Returns:			The number of bytes copied to the buffer, or zero if the
//							transfer is complete. 
//
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD CALLBACK RRTFConcatinator::StreamOutCallback( DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb )
	{
	// Get the pointer to the concatinator that started the transfer
	RRTFConcatinator* pConcatinator = reinterpret_cast<RRTFConcatinator*>( dwCookie );

	// Copy the requested amount of data to the CString object
	CString string( (LPCTSTR)pbBuff, cb );
	pConcatinator->m_String += string;

	*pcb = cb;

	return 1;
	}

#endif	// _WINDOWS
