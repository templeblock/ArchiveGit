/*	$Header: /PM8/App/LogoMakerDialog.cpp 1     3/03/99 6:07p Gbeddow $
//
//	Definition of the RLogoMakerDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a logo compound graphic.
//	
// Author:     Mike Heydlauf
//
// Portability: Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
// $Log: /PM8/App/LogoMakerDialog.cpp $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     2/11/99 3:01p Rgrenfel
// Forced reconstruction of the graphics collection searcher when
// necessary.
// 
// 1     2/08/99 3:10p Rgrenfel
// Implementation of the logo grouped object dialog.
*/

#include "StdAfx.h"

ASSERTNAME

#include "LogoMakerDialog.h"
#include "HeadlineInterface.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "SolidColor.h"
#include "ComponentDocument.h"
#include "ComponentTypes.h"
#include "ResourceManager.h"
#include "CompositeSelection.h"


const YFloatType kMaxLogoChars = 30;

const YFloatType kPercentToShrinkLogoX = .75;
const YFloatType kPercentToShrinkLogoY = .25;

const RGOHeadlineData RLogoMakerDialog::m_rCannedHeadlineEffects[] = 
{
/*Rectangle*/	RGOHeadlineData( kNonDistort,		 /*kNoWarpShape,*/30, TRUE ),
/*Squeeze*/		RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape13,*/38, TRUE ),
/*Balloon*/		RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape1,*/	42, TRUE ),
/*Bottom Arch*/RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape3,*/	63, TRUE ),
/*Top Arch*/	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape120,*/67, TRUE ),
/*Angle Up*/	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape9,*/	39, TRUE ),
/*Angle Down*/	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape10,*/43, TRUE ),
/*Top Arc*/		RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape14,*/64, TRUE ),
/*Bottom Arc*/	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape14,*/68, TRUE ),
/*Scroll Up*/	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape9,*/	71, TRUE ),
/*Scroll Down*/RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape10,*/75, TRUE ),
/*Drop Cap*///	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape110,*/78, FALSE ),
/*Pop Cap*/	//	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape110,*/80, FALSE ),
/*Drop Pop*///	RGOHeadlineData( kWarpToEnvelop,	 /*kWarpShape110,*/79, FALSE ),
/*Top Semi*/	RGOHeadlineData( kFollowPath,		 /*kWarpPath20,*/	4, FALSE ),
/*Bottom Semi*/RGOHeadlineData( kFollowPath,		 /*kWarpPath30,*/	5, FALSE ),
/*Line*/			RGOHeadlineData( kNonDistort,	 /*Doesn't matter what path*/	30, FALSE )
};


// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::RLogoMakerDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RLogoMakerDialog::RLogoMakerDialog( RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent, int nResourceId )
	: RCommonGroupedObjectDialog(pParentDocument,pEditedComponent,nResourceId),
	m_pGraphicCollectionSearcher( NULL )
{
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::RLogoMakerDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RLogoMakerDialog::~RLogoMakerDialog()
{
	delete m_pGraphicCollectionSearcher;
}


// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::DoDataExchange(CDataExchange* pDX)
{
	RCommonGroupedObjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RLogoMakerDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RLogoMakerDialog, RCommonGroupedObjectDialog)
	//{{AFX_MSG_MAP(RLogoMakerDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RLogoMakerDialog message handlers

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::OnInitDialog
//
//  Description:		dialog initialization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL RLogoMakerDialog::OnInitDialog() 
{
	m_rHeadlineText = ::GetResourceManager().GetResourceString( IDS_DEFAULT_LOGO );

	//	Do default initialization.
	if( !RCommonGroupedObjectDialog::OnInitDialog() ) 
		return FALSE;

	//	If we are editing the logo for the first time
	//	then start with a graphic selected.
	if ( m_fFirstEdit )
	{
		m_fFirstEdit = FALSE;
		m_cGraphicsListBox.SetCurSel( 1 );
		OnSelchangeListboxGraphics( );
	}

	//	Install the graphic component's bounding rect if it was written.
	//	Use the default if it wasn't.
	if ( m_fBoundingRectPresent && m_pGraphicDoc )
	{
		RComponentView* pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
		pEditedGraphicView->SetBoundingRect( m_rGraphicRect );
	}

	//	Remove the current graphic selection.
	RCompositeSelection* pSelection = m_pControlView->GetSelection( );
	if ( pSelection )
		pSelection->UnselectAll( TRUE );

	//	Limit the number of characters we can edit.
	m_cHeadlineEditCtrl.SetLimitText( kMaxLogoChars );

	ApplyHeadlineEffect();

	m_fInitializing = FALSE;
	UpdatePreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::SetDefaultGraphicBoundingRect
//
//  Description:		Position the graphic in the edit area.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::SetDefaultGraphicBoundingRect( ) 
{
	RCommonGroupedObjectDialog::SetDefaultGraphicBoundingRect( );
	
	//	Define the handle size.
	RRealSize rHandleSize( kRotationResizeHandleHitSize, kRotationResizeHandleHitSize );
	ScreenUnitsToLogicalUnits( rHandleSize );

	//	Reduce the size of the graphic so that its control points are all visible.
	m_rGraphicRect.Inset( rHandleSize, rHandleSize );

	//	Move it up so that the graphic doesn't overlay the headline.
	m_rGraphicRect.Offset( RRealSize( 0, -rHandleSize.m_dy / 2. ) );
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::DisplayGraphic
//
//  Description:		Display selected graphic in preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::UpdateGraphicDocument( RComponentDocument*& pTempGraphicDoc , RComponentDocument*& pGraphicDoc ) 
{
	TpsAssert( pTempGraphicDoc, "Null Graphic Doc" );
	//
	// If a graphic component already exists, replace it.  Otherwise create a new one
	if( pGraphicDoc )
	{
		ReplaceComponent( pGraphicDoc, pTempGraphicDoc );
	}
	else
	{
		NewComponent( pTempGraphicDoc );
	}
	//
	// Set member graphic document pointer and fit graphic to preview control...
	pGraphicDoc = pTempGraphicDoc;

	CheckForMonochromeGraphic( pGraphicDoc );

	UpdatePreview( FALSE, TRUE );
}


// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::GetDefaultHeadlinePlacement
//
//  Description:		Setup default headline placement info
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RRealRect RLogoMakerDialog::GetDefaultHeadlineRect()
{
	//	Move the headline to the bottom of the control window.
	RRealSize rHeadlineSize( m_pControlView->GetSize());

	//	The headline will be 1/4 the height of the control area
	//	and its full width.
	rHeadlineSize.m_dy /= 4.;

	//	Position the headline the bottom of the control area.
	RRealPoint rHeadlineTopLeftPoint( 0., 3. * rHeadlineSize.m_dy );

	//	Define the base headline rect.
	RRealRect rHeadlineRect( rHeadlineTopLeftPoint, rHeadlineSize );

	//	Reduce the headline rect so that the resize handles are inside the control
	//	area when it is selected. Don't reduce the vertical size since this
	//	will make the headline too small. Instead move the headline up to get the
	//	bottom control handles inside the control.
	//	Define the handle size.
	RRealSize rHandleSize( kResizeHandleHitSize, kResizeHandleHitSize );
	ScreenUnitsToLogicalUnits( rHandleSize );
	rHeadlineRect.Inset( RRealSize( rHandleSize.m_dx, 0. ) );
	rHeadlineRect.Offset( RRealSize( 0., - rHandleSize.m_dy / 2. ) );
	
	return rHeadlineRect;
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::CreateEffectsList
//
//  Description:		Create '|' delimited string of effects names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::CreateEffectsList( RMBCString& rList )
{
	rList = ::GetResourceManager().GetResourceString( IDS_LOGO_EFFECTS );
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::CreateGraphicsList
//
//  Description:		Create '|' delimited string of graphics names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::CreateGraphicsList( RMBCString& rList )
{
	//
	// Only allocate memory for the collection searcher once..
	if( m_pGraphicCollectionSearcher == NULL || m_fRebuiltGraphicArrays )
	{
		// We rebuilt the graphic arrays, so we have to recreate the searcher.
		if (m_pGraphicCollectionSearcher != NULL)
		{
			delete m_pGraphicCollectionSearcher;
		}
		m_pGraphicCollectionSearcher = new 	RCollectionSearcher<RLogoGraphicSearchCollection>(m_rCollectionArray);
	}
	// Find all the graphics
	RSearchResultArray* pResultArray = m_pGraphicCollectionSearcher->SearchCollections();

	// Now iterate through all the graphics and insert them into
	// the string
	RSearchResultIterator resultIter( pResultArray->Start() );
	RSearchResultIterator resultIterEnd( pResultArray->End() );

	// The index has to start at 1 because the None selection is at zero
	for (int nIdx = 1; resultIter != resultIterEnd; resultIter++, nIdx++ )
	{
		rList += RMBCString( (LPSTR)(*resultIter).namePtr );
		rList += RMBCString( kListFieldSeperator );

		//
		// Keep track of graphics and their indexes...
		m_cGraphicMap.SetAt( (*resultIter).id, (*resultIter) );
		m_cGraphicArray.SetAtGrow( nIdx, (*resultIter) );
   }
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::OnButtonGOOtherGraphics
//
//  Description:		Headline/Cap Effect selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::OnSelchangeListboxEffects() 
{
	//
	//		Apply the effect
	ApplyHeadlineEffect();
	//
	//		Select the headline
	RCompositeSelection* pSelection = m_pControlView->GetSelection( );
	pSelection->UnselectAll( TRUE );
	pSelection->Select( m_pHeadlineView, TRUE );
	//
	//		Update preview
	UpdatePreview( TRUE, FALSE );
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::ApplyHeadlineEffect
//
//  Description:		Display new headline effect
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RLogoMakerDialog::ApplyHeadlineEffect() 
{
	int nTextStyleSelection = m_cEffectsListBox.GetCurSel();
	//
	// Make sure current selection isn't out of canned effects array bounds..
	TpsAssert( nTextStyleSelection < NumElements( m_rCannedHeadlineEffects ) , "Selection out of array bounds" );
	//
	// Apply canned headline effect..
	m_rHeadlineData = m_rCannedHeadlineEffects[nTextStyleSelection];
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RLogoMakerDialog::WriteUIContextData( RUIContextData& contextData ) const
{
	// Write common data..
	RCommonGroupedObjectDialog::WriteUIContextData( contextData );

	//	Write extra data at the current end of the context data.
	RCommonGroupedObjectDialog::WriteVersion2UIContextData( contextData );
}

// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RLogoMakerDialog::ReadUIContextData( ) 
{
	RUIContextData* pContextData = NULL;
	pContextData = RCommonGroupedObjectDialog::ReadUIContextData();

	// Write common data..
	if ( pContextData )
		pContextData 
		 = RCommonGroupedObjectDialog::ReadVersion2UIContextData( *pContextData );

	return pContextData;
}


// ****************************************************************************
//
//  Function Name:	RLogoMakerDialog::GetComponentType
//
//  Description:		Gets the type of component that this dialog creates and
//							edits
//
//  Returns:			Component type
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType& RLogoMakerDialog::GetComponentType( ) const
	{
	static YComponentType componentType = kLogoComponent;

	return componentType;
	}

