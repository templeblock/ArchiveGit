// ****************************************************************************
//
//  File Name:			HeadlineView.cpp
//
//  Project:			Headline Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RHeadlineView class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineView.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include	"HeadlineView.h"
#include	"HeadlineDocument.h"
#include "EditHeadlineAction.h"
// #include "RenaissanceResource.h"
#include "Menu.h"
#include "HLSpellCheckInterface.h"
#include "HLFindReplaceInterface.h"
#include "HeadlineInterfaceImp.h"
#include "Frame.h"

// ****************************************************************************
//
//  Function Name:	RHeadlineView::RHeadlineView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RHeadlineView::RHeadlineView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RComponentView( boundingRect, pDocument, pParentView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::OnXEditComponent( )
//
//  Description:		Process an Edit command on the component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineView::OnXEditComponent( EActivationMethod, const RRealPoint& )
	{
	REditHeadlineAction*	pAction = new REditHeadlineAction( GetHeadlineDocument() );
	GetRDocument()->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::Scale( )
//
//  Description:		Resize the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineView::Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio )
{
	//	Let the inherited view process the scale
#ifndef	DONT_CALL_INHERITED

	// Resize
	if( GetParentView( ) )
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint( centerOfScaling, scaleFactor, GetMinimumSize( ), GetMaximumSize( ), fMaintainAspectRatio );
		SetBoundingRect( tmp );
		}
	else
		{
		YComponentBoundingRect tmp( GetBoundingRect() );
		tmp.UnrotateAndResizeAboutPoint( centerOfScaling, scaleFactor );
		SetBoundingRect( tmp );
		}

#else
	RComponentView::Scale( centerOfScaling, scaleFactor, fMaintainAspectRatio );
#endif	//	DONT_CALL_INHERITED


	//	Tell the document to reconstruct itself
	RHeadlinePersistantObject&	headlineObject	= GetHeadlineDocument()->HeadlineObject();
	//	lock the data object
	RDataObjectLocker	locker( headlineObject );
	headlineObject.QueueReconstruct( );

	//	Flush any Outline paths
	UndefineOutlinePath( );
}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::SetBoundingRect( )
//
//  Description:		Sets the bounding rect of this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineView::SetBoundingRect( const YComponentBoundingRect& boundingRect )
	{
	RComponentView::SetBoundingRect( boundingRect );

	//	Tell the document to reconstruct itself
	RHeadlinePersistantObject&	headlineObject	= GetHeadlineDocument()->HeadlineObject();
	//	lock the data object
	RDataObjectLocker	locker( headlineObject );
	headlineObject.QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::SetFrame( RFrame::EFrameType eNewFrame )
//
//  Description:		Install the given frame in this component
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineView::SetFrame( EFrameType eNewFrame )
	{
	// Call the base method
	RComponentView::SetFrame( eNewFrame );

	//	Tell the document to reconstruct itself
	RHeadlinePersistantObject&	headlineObject	= GetHeadlineDocument()->HeadlineObject();
	//	lock the data object
	RDataObjectLocker	locker( headlineObject );
	headlineObject.QueueReconstruct( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::RenderInternalTrackingFeedback( )
//
//  Description:		Renders the tracking feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineView::RenderInternalTrackingFeedback( RDrawingSurface& drawingSurface,
														  const R2dTransform& transform,
														  const RView& targetView ) const
	{
	RHeadlineDocument*	pDoc = GetHeadlineDocument();
	RHeadlineGraphic*		pGraphic = static_cast<RHeadlineGraphic*>( pDoc->HeadlineObject().GetGraphic() );

	TpsAssertIsObject( RHeadlineGraphic, pGraphic );

	// Draw interior feedback
	if ( pDoc != NULL && pGraphic != NULL )
		pGraphic->RenderFeedback( drawingSurface, GetBoundingRect( ).GetUntransformedSize( ), transform );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::MaintainAspectRatioIsDefault( )
//
//  Description:		Returns TRUE if the default behavior when resizing is to
//							maintain the components aspect ratio. The opposite
//							behavior is obtained by holding down the control key.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RHeadlineView::MaintainAspectRatioIsDefault( ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::WantsGlowAndShadowInsideBounds() const
//
//  Description:		Determines if the component view wants its glow and
//							shadow effects to render inside its bounds.
//
//  Returns:			TRUE if the component view wants its glow and shadow
//							effects to render inside its bounds, else FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RHeadlineView::WantsGlowAndShadowInsideBounds() const
{
	// REVIEW GCB 4/19/98 - spec'd to return TRUE but scaling and positioning
	// relationships still aren't quite working right - try this way for now
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	YRealDimension RHeadlineView::SoftEffectDimension(
//								const RRealSize& size ) const
//
//  Description:		Returns the YRealDimension that should be used for
//							determining the x and y offsets of a soft effect on this
//							component.
//
//							If the headline component does NOT have a background color,
//							this implementation returns, in system units, the component
//							height (component width in the case of a vertical headline)
//							divided by the number of lines in the headline, or
//							the minimum dimension of the component, whichever is
//							smaller -- a "good enough" approximation to an average
//							line height of the headline.
//
//							If the headline has a background color, the minimum
//							dimension of the component is returned instead.
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RHeadlineView::SoftEffectDimension( const RRealSize& size ) const
{
	YRealDimension softEffectDimension( RComponentView::SoftEffectDimension( size ) );
	RHeadlineInterface* pHeadlineInterface = NULL;

	if (!HasBackgroundColor())
	{
		pHeadlineInterface =
			dynamic_cast<RHeadlineInterface*>( GetInterface( kHeadlineInterfaceId ) );
		TpsAssert( pHeadlineInterface != NULL, "Can't create headline interface" );
	}
	if (pHeadlineInterface)
	{
		uWORD numLines = 1;
		pHeadlineInterface->GetNumLines( numLines );
		TpsAssert( numLines != 0, "Zero headline text lines!" );
		if (numLines != 0)
		{
			ETextEscapement eEscapement;
			pHeadlineInterface->GetEscapement( eEscapement );
			YRealDimension lineHeight;
			if (eEscapement < kTop2BottomLeft2Right)
				lineHeight = ::Abs( size.m_dy ) / numLines;	// horizontal headline
			else
				lineHeight = ::Abs( size.m_dx ) / numLines;	// vertical headline
			softEffectDimension = ::Min( softEffectDimension, lineHeight );
		}
	}
	delete pHeadlineInterface;

	return softEffectDimension;
}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::CalculateOutlinePath( )
//
//  Description:		Calculate the outline path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RHeadlineView::CalculateOutlinePath( )
	{
	RHeadlineDocument*			pDoc				= GetHeadlineDocument( );
	RGraphic*						pGraphic			= pDoc->HeadlineObject().GetGraphic();

	if( pGraphic && (GetFrameType( )==kNoFrame) && !HasBackgroundColor( ) )
		{
		TpsAssert( m_pOutlinePath, "CalculateOutlinePath called but path is null" );

		//	First, make sure the outline is undefined
		m_pOutlinePath->Undefine( );
		if ( !pGraphic->GetOutlinePath(*m_pOutlinePath, GetReferenceSize( ), GetBoundingRect().GetTransform( )) )
			RComponentView::CalculateOutlinePath( );
		}
	else
		RComponentView::CalculateOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::GetInterface( )
//
//  Description:		Return The Interface pointer if the given component can support
//							the desired interface.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RInterface* RHeadlineView::GetInterface( YInterfaceId interfaceId ) const
	{
	//	Special case for certain interfaces...
	if ( interfaceId == kSpellCheckInterfaceId )
		{
		RMBCString	text;
		GetHeadlineDocument()->GetHeadlineText( text );
		if ( text.IsEmpty( ) )
			return NULL;
		}

	static	InterfaceToCreateFunction interfaceCreationMap[] =
		{
			{ kSpellCheckInterfaceId, RHLSpellCheckInterface::CreateInterface },
			{ kFindReplaceInterfaceId, RHLFindReplaceInterface::CreateInterface },
			{ kCalendarFindReplaceInterfaceId, RHLFindReplaceInterface::CreateInterface },
			{ kHeadlineInterfaceId, RHeadlineInterfaceImp::CreateInterface }
		} ;

	TpsAssert( interfaceId != kInvalidInterfaceId, "Invalid InterfaceId passed to GetInterface" )
	for (int i = 0; i < sizeof(interfaceCreationMap)/sizeof(InterfaceToCreateFunction); ++i )
		{
		if ( interfaceId == interfaceCreationMap[i].interfaceId )
			{
			return interfaceCreationMap[i].creator( this );
			}
		}

	return RComponentView::GetInterface( interfaceId );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineView::HasSoftEffects( )
//
//  Description:		Determines if the component uses soft effects
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RHeadlineView::HasSoftEffects( ) const
{
	RHeadlineDocument* pDoc = (RHeadlineDocument*) GetComponentDocument( );

	if (pDoc->HeadlineObject( ).GetGraphic()->HasSoftEffects())
		return TRUE;

	return RComponentView::HasSoftEffects();
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RHeadlineView::Validate( )
//
//  Description:		Validate the object.
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineView::Validate( ) const
	{
	RComponentView::Validate( );
	TpsAssertIsObject( RHeadlineView, this );
	}

#endif	//	TPSDEBUG
