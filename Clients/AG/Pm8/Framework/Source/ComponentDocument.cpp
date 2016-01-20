// ****************************************************************************
//
//  File Name:			CommonComponentDocument.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Definition of the RComponentDocument class
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
//  $Logfile:: /PM8/Framework/Source/ComponentDocument.cpp                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentDocument.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"ComponentView.h"
#include	"UndoableAction.h"
#include "DrawingSurface.h"
#include "ComponentAttributes.h"
#include "ComponentApplication.h"
#include "ChunkyStorage.h"
#include "DataTransfer.h"
#include "ComponentManager.h"
#include "ApplicationGlobals.h"

//	chunk tags
const uLONG kExtendedAttributesTag = 'XATT';
const uLONG kExtendedURLTag = 'XURL';


// ****************************************************************************
//
//  Function Name:	RComponentDocument::RComponentDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument::RComponentDocument( RApplication* pApp,
													 const RComponentAttributes& componentAttributes,
													 const YComponentType& componentType,
													 BOOLEAN )
	: m_pParentDocument( NULL ),
	  m_ComponentAttributes( componentAttributes ),
	  m_Tint(kMaxTint),
	  m_ComponentType( componentType )
	{
	// Initialize the base class
	RDocument::Initialize( pApp );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::~RComponentDocument( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument::~RComponentDocument( )
	{
	// Delete the views. Only done for a component document, as MFC/Powerplant
	// will take care of them for a standalone view
	YViewIterator iterator = GetViewCollectionStart( );
	for( ; iterator != GetViewCollectionEnd( ); ++iterator )
		delete *iterator;
	}


// ****************************************************************************
//
//  Function Name:	RComponentDocument::Clone()
//
//  Description:		Clones the component creating a complete copy of it.
//
//							It is the callers responsibility to free the new document
//							provided.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument * RComponentDocument::Clone()
{
	RComponentDocument *pNewDoc = NULL;

	// Create a memory stream which we can use to duplicate the component.
	RBufferStream stream;
	RStorage	parentStorage( &stream );
	RChunkStorage storage( &parentStorage );

	// Create a chunk to drop ourselves in.
	storage.AddChunk( kComponent );

	// Write ourselves into the storage.
	Write( storage, kSaving );

	// Seek back to the start of the storage
	storage.SeekAbsolute( 0 );

	// Read a new document out of the storage!
	if( ::GetComponentManager( ).IsComponentAvailable( GetComponentType() ) )
	{
		// Create a component of the appropriate type
		pNewDoc = ::GetComponentManager( ).CreateNewComponent( GetComponentType(), m_pParentDocument, RComponentAttributes( ), TRUE );

		// Now tell that component to read
		pNewDoc->Read( storage, kPasting );

		// NULL out the parent of the component view; its pretty much useless
		TpsAssertIsObject( RComponentView, pNewDoc->GetActiveView( ) );
		static_cast<RComponentView*>( pNewDoc->GetActiveView( ) )->SetParentView( NULL );
	}

	return pNewDoc;
}


// ****************************************************************************
//
//  Function Name:	RComponentDocument::IsFrameable( )
//
//  Description:		Return TRUE if this document can be framed
//
//  Returns:			TRUE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentDocument::IsFrameable( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetTopLevelDocument( )
//
//  Description:		Return the Top Level Standalone document
//
//  Returns:			the parents, topLevel Document
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RComponentDocument::GetTopLevelDocument( ) const
	{
	if ( GetParentDocument( ) )
		return GetParentDocument()->GetTopLevelDocument();
	else
		return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetParentDocument( )
//
//  Description:		Accessor
//
//  Returns:			Parent document
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDocument* RComponentDocument::GetParentDocument( ) const
	{
	return m_pParentDocument;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::SetParentDocument( )
//
//  Description:		Sets the parent document of this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::SetParentDocument( RDocument* pParentDocument )
	{
	m_pParentDocument = pParentDocument;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetMinObjectSize( const YComponentType& componentType )
//
//  Description:		Return the minimum size for the given object
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
RRealSize RComponentDocument::GetMinObjectSize( const YComponentType& componentType, BOOLEAN /* fMaintainAspect */ )
	{
	TpsAssert(m_pParentDocument, "Component has no parent document.");
	return m_pParentDocument->GetMinObjectSize( componentType );
	}
// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetMaxObjectSize( const YComponentType& componentType )
//
//  Description:		Return the maximum size for the given object
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
RRealSize RComponentDocument::GetMaxObjectSize( const YComponentType& componentType )
	{
	TpsAssert(m_pParentDocument, "Component has no parent document.");
	return m_pParentDocument->GetMaxObjectSize( componentType );
	}



// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetDefaultObjectSize( const YComponentType& componentType )
//
//  Description:		Return the default size for the given object
//
//  Returns:			^
//
//  Exceptions:		none
//
// ****************************************************************************
//
RRealSize RComponentDocument::GetDefaultObjectSize( const YComponentType& componentType )
	{
	TpsAssert(m_pParentDocument, "Component has no parent document.");
	return m_pParentDocument->GetDefaultObjectSize( componentType );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetView( )
//
//  Description:		Gets the view whose parent is the given view
//
//  Returns:			Component view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RComponentDocument::GetView( const RView* pParentView ) const
	{
	RView* pView = NULL;

	YViewIterator iterator = GetViewCollectionStart( );
	for( ; iterator != GetViewCollectionEnd( ); ++iterator )
		{
		TpsAssertIsObject( RComponentView, *iterator );
		RComponentView* pTempView = static_cast<RComponentView*>( *iterator );
		if( pTempView->GetParentView( ) == pParentView )
			{
			pView = *iterator;
			break;
			}
		}

	TpsAssert( pView, "There are no views on this document with the given parent view." );
	TpsAssertIsObject( RComponentView, pView );
	return static_cast<RComponentView*>( pView );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::DeleteView( )
//
//  Description:		Adds a new view to this document
//
//  Returns:			Nothing
//
//  Exceptions:		Memory exception
//
// ****************************************************************************
//
void RComponentDocument::DeleteRView( RView* pView )
	{
	// Call the base method to delete the view
	RDocument::DeleteRView( pView );

	// If this was our last view, remove ourself from our parent, and commit suicide
	if( m_ViewCollection.Count( ) == 0 )
		{
		FreeDocumentContents( );
		m_pParentDocument->RemoveComponent( this );
		delete this;
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::SendAction( )
//
//  Description:		Component API function to send an action to a Component
//
//  Returns:			TODO: What does this return?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentDocument::SendAction( RAction* pAction )
	{
	BOOLEAN	fRetVal;
	if ( m_pParentDocument )
		{
		TpsAssertValid( m_pParentDocument );
		TpsAssertValid( pAction );
		fRetVal = m_pParentDocument->SendAction( pAction );
		}
	else
		{
		try
			{
			fRetVal = pAction->Do( );
			}
		catch( YException )
			{
			fRetVal = FALSE;
			}
		delete pAction;
		}
	return fRetVal;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::BeginMultiUndo( )
//
//  Description:		Start a MultiUndo chain of actions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::BeginMultiUndo( )
	{
	TpsAssert( m_pParentDocument != NULL, "The Component document does not have a parent document");
	TpsAssertValid( m_pParentDocument );
	m_pParentDocument->BeginMultiUndo( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::CancelMultiUndo( )
//
//  Description:		Cancel a MultiUndo chain of actions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::CancelMultiUndo( )
	{
	TpsAssert( m_pParentDocument != NULL, "The Component document does not have a parent document");
	TpsAssertValid( m_pParentDocument );
	m_pParentDocument->CancelMultiUndo( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::EndMultiUndo( )
//
//  Description:		End a MultiUndo chain of actions
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::EndMultiUndo( uWORD uwUndoStringId, uWORD uwRedoStringId )
	{
	TpsAssert( m_pParentDocument != NULL, "The Component document does not have a parent document");
	TpsAssertValid( m_pParentDocument );
	m_pParentDocument->EndMultiUndo( uwUndoStringId, uwRedoStringId );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetNextActionId( )
//
//  Description:		Get the id of the action that will be undone next
//
//  Returns:			The id
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YActionId& RComponentDocument::GetNextActionId( ) const
	{
	TpsAssert( m_pParentDocument != NULL, "The Component document does not have a parent document");
	TpsAssertValid( m_pParentDocument );
	return m_pParentDocument->GetNextActionId( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RComponentDocument::CreateScriptAction( const YActionId& actionId, RScript& script )
	{
	// Apparently no one wants this command. Return FALSE.
	return RDocument::CreateScriptAction( actionId, script );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetComponentAttributes( )
//
//  Description:		Gets the component attributes struct for this component
//							
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RComponentAttributes& RComponentDocument::GetComponentAttributes( ) const
	{
	return m_ComponentAttributes;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::SetComponentAttributes( )
//
//  Description:		Sets the component attributes struct for this component
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::SetComponentAttributes( const RComponentAttributes& componentAttributes )
	{
	m_ComponentAttributes = componentAttributes;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetComponentType( )
//
//  Description:		Get the type of this component
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType& RComponentDocument::GetComponentType( ) const
	{
	return m_ComponentType;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::SetComponentType( )
//
//  Description:		Set the type of this component
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::SetComponentType( const YComponentType & componentType )
	{
	m_ComponentType = componentType;
	}


// ****************************************************************************
//
//  Function Name:	RComponentDocument::Write( )
//
//  Description:		Writes this documents data to the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentDocument::Write( RChunkStorage& storage, EWriteReason reason ) const
	{
	storage.AddChunk( 'CDOC' );

	// Write the component attributes
	m_ComponentAttributes.Write( storage );

	// Write out the number of views
	storage << static_cast< uLONG >( m_ViewCollection.Count( ) );

	// Write out the tint value
	storage << (uLONG)m_Tint;

	// Write out the information required to recreate our views
	YViewIterator iterator = GetViewCollectionStart( );
	for( ; iterator != GetViewCollectionEnd( ); ++iterator )
		{
		// Write the index of its parent view
		TpsAssertIsObject( RComponentView, *iterator );
		RComponentView* pComponentView = static_cast<RComponentView*>( *iterator );
		uLONG	ulViewIndex = 0;
		if( m_pParentDocument && pComponentView->GetParentView( ) )
			ulViewIndex	= static_cast<uLONG>( m_pParentDocument->GetViewIndex( pComponentView->GetParentView( ) ) );
		storage << ulViewIndex;
		}

	//	Write out any UI context object information
	m_UIContextData.Write( storage );

	// PS 6.0 and later: write a nested extended component attributes chunk
	// ignored by Print Shop 5.x since it's at the end of the CDOC chunk
	storage.AddChunk( kExtendedAttributesTag );
	m_ComponentAttributes.WriteExtendedAttributes( storage );
	storage.SelectParentChunk( );

	// PS 7/10 and later: write a nested URL chunk to be ignored by earlier versions.
	storage.AddChunk( kExtendedURLTag );
	m_URL.Write( storage );
	storage.SelectParentChunk( );

	//	Go back to writing at the prior level.
	storage.SelectParentChunk( );

	// Call the base method
	RDocument::Write( storage, reason );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::Read( )
//
//  Description:		Reads from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentDocument::Read( RChunkStorage& storage, EReadReason reason )
	{
	storage.Start( 'CDOC', !kRecursive );

	// Read the component attributes
	m_ComponentAttributes.Read( storage );

	// Read in the number of views
	uLONG numViews;
	storage >> numViews;

	uLONG uTint;
	storage >> uTint;
	m_Tint = (YTint)uTint;

	// For each view, create a new one
	for( uLONG i = 0; i < numViews; ++i )
		{
		// Read the parent view index
		uLONG parentViewIndex;
		storage >> parentViewIndex;

		RView* pParentView = NULL;

		if( reason == kPasting )
			{
			// If we are pasting, then the parent view is meaningless, so just use NULL
			pParentView = NULL;
			}
		else
			{
			// Get the view pointer corresponding to this index
			if ( m_pParentDocument )
				pParentView = m_pParentDocument->GetViewByIndex( parentViewIndex );
			}

		// Create a view. Give it a large size, because some components (ie text) need it
		RComponentView* pComponentView = CreateView( YComponentBoundingRect( 0, 0, 32000.0, 32000.0), pParentView );

		// Add the view to ourself
		AddRView( pComponentView );
		}

	//	Read in any UI context object information
	m_UIContextData.Read( storage );

	// Read or set the extended component attributes
	RChunkStorage::RChunkSearcher yIter = storage.Start( kExtendedAttributesTag, FALSE );
	if (!yIter.End())
	{
		// PS 6.0 and later file: read the nested extended component attributes chunk
		m_ComponentAttributes.ReadExtendedAttributes( storage );
		storage.SelectParentChunk();
	}
	else
	{
		// Pre-6.0 file: use default extended component attributes
		m_ComponentAttributes.SetDefaultExtendedComponentAttributes( m_ComponentType );
	}
	// more component types are shadowable in 6.0 and later than in pre-6.0
	// so use the default shadowable attribute, ignoring what was written
	RComponentAttributes defaultComponentAttributes( RComponentAttributes::GetDefaultComponentAttributes( m_ComponentType ) );
	m_ComponentAttributes.SetShadowable( defaultComponentAttributes.IsShadowable() );

	// PS 7/10 and later: Read in nested URL chunk if present.
	yIter = storage.Start( kExtendedURLTag, FALSE );
	if (!yIter.End())
	{
		m_URL.Read( storage );
		storage.SelectParentChunk();
	}

	//	Go back to reading at the prior level.
	storage.SelectParentChunk( );

	// Call the base method to continue reading
	RDocument::Read( storage, reason );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetDataFormatsSupportedForCopy( )
//
//  Description:		Retrieves the data formats that this document can copy to.
//							Formats should be added to the end of the collection in
//							decreasing order of priority.
//
//							Default behavior is to support picture and bitmap formats.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::GetDataFormatsSupportedForCopy( YDataFormatCollection& collection ) const
	{
	collection.InsertAtEnd( kEnhancedMetafileFormat );
	collection.InsertAtEnd( kPictureFormat );
	collection.InsertAtEnd( kDibFormat );
	collection.InsertAtEnd( kBitmapFormat );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::Copy( )
//
//  Description:		Requests data in the specified format.
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RComponentDocument::Copy( YDataFormat dataFormat, RDataTransferTarget& dataTransferTarget )
	{
	switch( dataFormat )
		{
		// Default handling for various formats
		case kDibFormat :
		case kBitmapFormat :
		case kPictureFormat :
		case kEnhancedMetafileFormat :
			{
			// Create a component collection of only this component
			RComponentCollection componentCollection;
			componentCollection.InsertAtStart( this );

			// Copy the collection
			componentCollection.Copy( dataTransferTarget, dataFormat );

			break;
			}

		default :
			TpsAssertAlways( "Unexpected data format." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::Paste( )
//
//  Description:		Retrieves data in the specified format from the clipboard
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RComponentDocument::Paste( YDataFormat, const RDataTransferSource& )
	{
	TpsAssertAlways( "Why are we here?" );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::SetTint()
//
//  Description:		Sets the tint of this object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RComponentDocument::SetTint(YTint tint, BOOLEAN fApplyToChildren /*= kApplyToChildren*/)
	{
	// Make sure the tint is in range
	TpsAssert((tint >= kMinTint) && (tint <= kMaxTint), "Invalid tint value.");

	// Set the tint of this component
	YTint		oldTint	= m_Tint;
	if ( m_Tint != tint )
		{
		m_Tint = tint;	

		//	If there was a change, apply it to the children if desired
		if ( fApplyToChildren )
			{
			// Set the tint of all embedded components
			YComponentIterator iterator = GetComponentCollectionStart( );
			for( ; iterator != GetComponentCollectionEnd( ); ++iterator )
				{
				YTint	componentTint	= ( *iterator )->GetTint( );
				//	Adjust the tint to remove the old tint percentage (retrieve the component actual tint)
				componentTint	= ::Round( (YRealDimension)componentTint * (YRealDimension)kMaxTint / (YRealDimension)oldTint );
				//	Adjust the tint to put in the new tint
				componentTint	= ::Round( (YRealDimension)componentTint * (YRealDimension)tint / (YRealDimension)kMaxTint );
				//	Make sure it is not 0
				if ( componentTint == 0 )
					componentTint = 1;
				( *iterator )->SetTint( componentTint);
				}
			}

		//	Invalidate all of the views
		InvalidateAllViews();
		//	Invalidate all of the render caches...
		YViewIterator	iterator		= GetViewCollectionStart( );
		YViewIterator	iteratorEnd	= GetViewCollectionEnd( );
		for ( ; iterator != iteratorEnd; ++iterator )
			{
			RComponentView* pView = static_cast<RComponentView*>( *iterator );
			pView->InvalidateRenderCache( );
 			}
		}
 	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetTint()
//
//  Description:		Returns the tint of this object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
YTint RComponentDocument::GetTint() const
	{
	return m_Tint;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetUIContextData()
//
//  Description:		Gets the objects UI persistant data
//
//  Returns:			Reference to the UI persistant data
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData& RComponentDocument::GetUIContextData( )
	{
	// Seek to the start of the persistant data
	m_UIContextData.SeekAbsolute( 0 );

	// Return it
	return m_UIContextData;
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::ResetToDefaultSize()
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
void RComponentDocument::ResetToDefaultSize( RDocument* pParentDocument )
	{
	// Get the view
	RComponentView* pView = static_cast<RComponentView*>( GetActiveView( ) );

	// Get the default size for the component
	RRealSize defaultSize = pParentDocument->GetDefaultObjectSize( GetComponentType( ) );

	// Create a new bounding rect using that size
	YComponentBoundingRect boundingRect( defaultSize );

	// Offset to put it back at the old center
	boundingRect.Offset( pView->GetBoundingRect( ).GetCenterPoint( ) - boundingRect.GetCenterPoint( ) );

	// Set the new bounding rect
	pView->SetBoundingRect( boundingRect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::CanInsertComponent()
//
//  Description:		Determines if a component of the specified type may be
//							inserted into this document.
//
//  Returns:			TRUE if the component can be inserted
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentDocument::CanInsertComponent( const YComponentType& componentType ) const
	{
	// Just ask our parent
	return m_pParentDocument->CanInsertComponent( componentType );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::InvalidateAllViews( )
//
//  Description:		Invalidate all of the views associated with this document.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::InvalidateAllViews( BOOLEAN fErase )
	{
	// Dont do if we dont have a parent
	if( m_pParentDocument )
		RDocument::InvalidateAllViews( fErase );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::GetDataSize( )
//
//  Description:		Gets the natural size of the data contained by this
//							document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentDocument::GetDataSize( ) const
	{
	UnimplementedCode( );
	return RRealSize( 0, 0 );
	}

// ****************************************************************************
//
//  Function Name:	RComponentDocument::ReleaseResources()
//
//  Description:		Free up all GDI resources that can be reacquired later
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RComponentDocument::ReleaseResources( ) 
	{
	//	Nothing to release.
	NULL;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RComponentDocument::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentDocument::Validate( ) const
	{
	RDocument::Validate( );
	TpsAssertIsObject( RComponentDocument, this );
	}

#endif	//	TPSDEBUG
