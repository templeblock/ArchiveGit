// ****************************************************************************
//
//  File Name:			HeadlineDocument.cpp
//
//  Project:			Headline Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RHeadlineDocument class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineDocument.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include	"HeadlineDocument.h"
#include	"HeadlineApplication.h"
#include	"HeadlineView.h"
#include "HeadlinesCanned.h"
#include "DrawingSurface.h"
#include "ChunkyBackstore.h"
#include "EditHeadlineAction.h"
#include "SetHeadlineTextAction.h"
#include "ComponentTypes.h"

#include "SoftShadowSettings.h"

const YChunkTag kHeadlineDataTag	= 'Head';

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::RHeadlineDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineDocument::RHeadlineDocument( RApplication* pApp,
												  const RComponentAttributes& componentAttributes,
												  const YComponentType& componentType,
												  BOOLEAN fLoading )
	: RComponentDocument( pApp, componentAttributes, componentType, fLoading )
	{
	m_HeadlineObject.SetDocument( this, fLoading );
	m_HeadlineObject.Initialize( );

	//	Setup the backstore object pointer here...
	RChunkStorage*	pBackstore	= RChunkStorage::GetRootBackstore();
	pBackstore->AddChunk( kHeadlineDataTag );
	m_HeadlineObject.Associate( pBackstore );
	pBackstore->SelectParentChunk();
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::~RHeadlineDocument( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineDocument::~RHeadlineDocument( )
	{
	//	Deallocate the text data object - first deassociate it to not bother writting anything
	///	REVIEW	MDH - I need a way to tell the backstore that it can reuse this YChunkStorageId
	m_HeadlineObject.PurgeData( );
	m_HeadlineObject.Associate( NULL );
	if (m_HeadlineObject.IsInMemory())
		m_HeadlineObject.Deallocate();
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::Write( )
//
//  Description:		Writes this documents data to the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RHeadlineDocument::Write( RChunkStorage& storage, EWriteReason reason ) const
{
	//	lock the data object
	RHeadlineDocument*		pThisDoc	= const_cast<RHeadlineDocument*>( this );
	RDataObjectLocker			locker( pThisDoc->HeadlineObject() );

	RComponentDocument::Write( storage, reason );

	storage.AddChunk( kHeadlineDataTag );

	// Convert new shadow parameters into old
	RSoftShadowSettings shadowSettings;
	RComponentView* pView = (RComponentView *) GetActiveView();
	if (pView) pView->GetShadowSettings( shadowSettings );

	if (pView && shadowSettings.m_fShadowOn && FALSE)
	{
		// We need a non-const pointer to make these changes
		RHeadlineDocument* pDoc = const_cast<RHeadlineDocument *>( this );

		EffectsDataStruct sEffects, sOldEffects;
		pDoc->m_HeadlineObject.GetEffectData( sEffects );
		pDoc->m_HeadlineObject.GetEffectData( sOldEffects );
		
		sEffects.shadowFillDesc   = RHeadlineColor( shadowSettings.m_fShadowColor );
		sEffects.eShadowEffect    = kDropShadow;

		YAngle angle = -shadowSettings.m_fShadowAngle;
		sEffects.nShadowVanishPtX = int( cos( angle ) * 1414 + 0.5 );
		sEffects.nShadowVanishPtY = int( sin( angle ) * 1061 + 0.5 );

		// Determine shadow depth
		if (shadowSettings.m_fShadowEdgeSoftness > 0.0)
		{
			sEffects.eShadowEffect = kSoftDropShadow;
			sEffects.flShadowDepth = 0.125;
		}
		else if (shadowSettings.m_fShadowOffset > 0.666)
			sEffects.flShadowDepth  = 0.25;
		else if (shadowSettings.m_fShadowOffset > 0.333)
			sEffects.flShadowDepth  = 0.125;
		else
			sEffects.flShadowDepth  = 0.0625;

		pDoc->m_HeadlineObject.SetEffectData( sEffects, FALSE );

		m_HeadlineObject.Write( storage );
		storage.SelectParentChunk( );

		// Reset things to the way they were.
		pDoc->m_HeadlineObject.SetEffectData( sOldEffects, TRUE );

		return;
	}

	m_HeadlineObject.Write( storage );
	
	storage.SelectParentChunk( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::Read( )
//
//  Description:		Reads from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RHeadlineDocument::Read( RChunkStorage& storage, EReadReason reason )
	{
	//	lock the data object
	RDataObjectLocker	locker( m_HeadlineObject );

	//	The state is not valid until we are done reading...
	m_HeadlineObject.SetStateValid( FALSE );

	RComponentDocument::Read( storage, reason );

	// Iterate through the components
	RChunkStorage::RChunkSearcher searcher = storage.Start( kHeadlineDataTag, FALSE );

	TpsAssert( !searcher.End(), "No Stonehand information found in this chunky section" );
	if ( searcher.End() )
		throw kGenericFile;

	m_HeadlineObject.Read( storage );

	EffectsDataStruct sEffects;
	m_HeadlineObject.GetEffectData( sEffects );

	if (sEffects.eShadowEffect > kNoShadow && FALSE)
	{
		// Convert old shadow parameters into new
		RSoftShadowSettings shadowSettings;
		RComponentView* pView = (RComponentView *) GetActiveView();
		if (pView) pView->GetShadowSettings( shadowSettings );

		// Make sure there not set already
		if (pView && !shadowSettings.m_fShadowOn)
		{
			shadowSettings.m_fShadowOn           = TRUE;
			shadowSettings.m_fShadowOpacity      = 1.0;
			shadowSettings.m_fShadowEdgeSoftness = 0.0;
			shadowSettings.m_fShadowOffset       = 0.0; // REVIEW: calculate from old settings;

			if (kSoftDropShadow == sEffects.eShadowEffect)
			{
				shadowSettings.m_fShadowOpacity      = 0.65;
				shadowSettings.m_fShadowEdgeSoftness = 0.60;
			}

			shadowSettings.m_fShadowColor = 
				sEffects.shadowFillDesc.GetSolidColor();
			shadowSettings.m_fShadowAngle = 
				-(kPI + atan2( sEffects.nShadowVanishPtX, sEffects.nShadowVanishPtY ));

			pView->SetShadowSettings( shadowSettings );
		}

		sEffects.eShadowEffect = kNoShadow;
		m_HeadlineObject.SetEffectData( sEffects, FALSE );
	}

	//	We are now all read in, so its valid and reconstruct it.
	m_HeadlineObject.SetStateValid( TRUE );
	m_HeadlineObject.QueueReconstruct( );

	// Vertical headline types were added late, so check for them here.
	if( m_HeadlineObject.GetGraphic( FALSE )->GetTextEscapement( ) == kTop2BottomLeft2Right )
		if( m_ComponentType == kHeadlineComponent )
			m_ComponentType = kVerticalHeadlineComponent;
		else if( m_ComponentType == kSpecialHeadlineComponent )
			m_ComponentType = kVerticalSpecialHeadlineComponent;

		// The banner was creating the wrong headline type for a while. Check here.
		else if( m_ComponentType == kVerticalHeadlineComponent && GetComponentAttributes( ).GetUniqueId( ) != kNotUnique )
			m_ComponentType = kVerticalSpecialHeadlineComponent;

	// Select the parent storages
	storage.SelectParentChunk( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::ResetToDefaultSize()
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
void RHeadlineDocument::ResetToDefaultSize( RDocument* pParentDocument )
	{
	// Get the default size for the component
	RRealSize defaultSize = pParentDocument->GetDefaultObjectSize( GetComponentType( ) );

	// Get the view
	RComponentView* pView = static_cast<RComponentView*>( GetActiveView( ) );
	RHeadlineInterface* pInterface = (RHeadlineInterface *)
		pView->GetInterface( kHeadlineInterfaceId );

	if (pInterface)
	{
		EDistortEffects eEffect;
		ETextEscapement eEscapement;

		pInterface->GetDistortEffect( eEffect );
		pInterface->GetEscapement( eEscapement );
		delete pInterface;

		if (eEffect == kFollowPath && eEscapement < kTop2BottomLeft2Right)
			defaultSize.m_dy *= 2;
	}

	// Create a new bounding rect using that size
	YComponentBoundingRect boundingRect( defaultSize );

	// Offset to put it back at the old center
	boundingRect.Offset( pView->GetBoundingRect( ).GetCenterPoint( ) - boundingRect.GetCenterPoint( ) );

	// Set the new bounding rect
	pView->SetBoundingRect( boundingRect );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::GetHeadlineData( )
//
//  Description:		Retrieve the headline data from the HeadlineGraphic
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::GetHeadlineData( HeadlineDataStruct* pData )
	{
	//	lock the data object
	RDataObjectLocker	locker( m_HeadlineObject );
	m_HeadlineObject.GetHeadlineData( pData );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::SetHeadlineData( )
//
//  Description:		Stores the headline data into the HeadlineGraphic
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::SetHeadlineData( HeadlineDataStruct* pData )
	{
	//	lock the data object
	RDataObjectLocker	locker( m_HeadlineObject );
	m_HeadlineObject.SetHeadlineData( pData );

	//	Invalidate the views to cause a render and recalculation
	InvalidateAllViews( );

	//	Invalidate the Outlinepath of any views of this data
	YViewIterator	iterator		= GetViewCollectionStart( );
	YViewIterator	iteratorEnd	= GetViewCollectionEnd( );
	for ( ; iterator != iteratorEnd; ++iterator )
		{
		TpsAssertIsObject( RComponentView, *iterator );
		RComponentView*	pView	= static_cast<RComponentView*>( *iterator );
		pView->UndefineOutlinePath( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::GetHeadlineText( )
//
//  Description:		Retrieve the headline Text from the HeadlinePersistantObject
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::GetHeadlineText( RMBCString& text )
	{
	//	lock the data object
	RDataObjectLocker	locker( m_HeadlineObject );
	HeadlineObject( ).GetText( text );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::SetHeadlineText( )
//
//  Description:		Stores the headline text into the HeadlinePersistantObject
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::SetHeadlineText( const RMBCString& text )
	{
	RSetHeadlineTextAction*	pAction	= new RSetHeadlineTextAction( this, text );
	SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::FreeDocumentContents( )
//
//  Description:		Free the document data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::FreeDocumentContents( )
	{
	m_HeadlineObject.PurgeData( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::Render( )
//
//  Description:		Called to render the documents data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::Render( RDrawingSurface& drawingSurface,
														const R2dTransform& transform,
														const RIntRect& rcRender,
														const RRealSize& size ) const
	{
	//	Prepare the drawing surface for imaging the headling..
	drawingSurface.RestoreDefaults( );
	YTint		tint	= GetTint();
	if (tint != kMaxTint)
		drawingSurface.SetTint(tint);

	//	lock the data object
	RHeadlineDocument*	pThisDoc	= const_cast< RHeadlineDocument* >( this );
	RDataObjectLocker	locker( pThisDoc->HeadlineObject() );
	RHeadlineGraphic*		pGraphic		= pThisDoc->HeadlineObject().GetGraphic( );
	pGraphic->Render( drawingSurface, size, transform, rcRender );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::RenderBehindSoftEffect( )
//
//  Description:		Called to render the portion of the document that should
//							appear behind its soft effect such as the silhouette of
//							a headline
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::RenderBehindSoftEffect( RDrawingSurface& /*drawingSurface*/,
														const R2dTransform& /*transform*/,
														const RIntRect& /*rcRender*/,
														const RRealSize& /*size*/ ) const
	{
#if 0
	drawingSurface.RestoreDefaults( );
	YTint		tint	= GetTint();
	if (tint != kMaxTint)
		drawingSurface.SetTint(tint);

	//	lock the data object
	RHeadlineDocument*	pThisDoc	= const_cast< RHeadlineDocument* >( this );
	RDataObjectLocker	locker( pThisDoc->HeadlineObject() );
	RHeadlineGraphic*		pGraphic		= pThisDoc->HeadlineObject().GetGraphic( );
	pGraphic->RenderBehindSoftEffect( drawingSurface, size, transform, rcRender );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::RenderShadow( )
//
//  Description:		Called to render the documents shadow
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::RenderShadow( RDrawingSurface& drawingSurface,
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
		RColor monochromeColor = shadowColor;

		//	Prepare the drawing surface for imaging the headling..
		drawingSurface.RestoreDefaults( );
		YTint		tint	= GetTint();
		if (tint != kMaxTint)
			drawingSurface.SetTint(tint);

		//	lock the data object
		RHeadlineDocument*	pThisDoc	= const_cast< RHeadlineDocument* >( this );
		RDataObjectLocker	locker( pThisDoc->HeadlineObject() );
		RHeadlineGraphic*		pGraphic		= pThisDoc->HeadlineObject().GetGraphic( );
		pGraphic->Render( drawingSurface, size, transform, rcRender, monochromeColor, TRUE );
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
//  Function Name:	RHeadlineDocument::CreateView( )
//
//  Description:		Creates a new view on this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RHeadlineDocument::CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView )
	{
	return new RHeadlineView( boundingRect, this, pParentView );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::GetDataSize( )
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
RRealSize RHeadlineDocument::GetDataSize( ) const
	{
	RHeadlineGraphic*		pGraphic = const_cast<RHeadlineDocument*>( this )->HeadlineObject().GetGraphic( FALSE );
	return pGraphic->CalcNominalBoundingSize( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RHeadlineDocument::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineDocument::Validate( ) const
	{
	RComponentDocument::Validate( );
	TpsAssertIsObject( RHeadlineDocument, this );
	}
									
#endif	//	TPSDEBUG
