/*	$Header: /PM8/App/SignatureDialog.cpp 1     3/03/99 6:11p Gbeddow $
//
//	Definition of the RSignatureDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a signature compound graphic.
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
// $Log: /PM8/App/SignatureDialog.cpp $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     2/10/99 5:08p Rgrenfel
// Created a new DisplayGraphic call that takes the new and old component
// documents instead of performing a collection read.
// 
// 1     2/08/99 3:11p Rgrenfel
// Implementation of the signature grouped object dialog.
*/

#include "StdAfx.h"

ASSERTNAME

#include "SignatureDialog.h"
#include "HeadlineInterface.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "SolidColor.h"
#include "ComponentDocument.h"
#include "ResourceManager.h"


const YFloatType kMaxSignatureChars = 50;

const RRealSize  kSignatureHeadlineScaleSize( .67, .25 ); 
const YFloatType kSignatureHeadlineOffsetX = 0.165; // Percentage of control from top left corner
const YFloatType kSignatureHeadlineOffsetY = 0.58; // Percentage of control from top left corner

const YFloatType kGraphicOffsetDivisorY = 6.0; // The larger this value, the farther up the graphic moves
const YFloatType kGraphicScaleFactor = 0.75;

const RMBCString kSeperatorLine = "-";
const RRealSize kSeperatorHeadlineScaleSize( .80, .07 );
const YFloatType kSeperatorHeadlineOffsetX = 0.10; // Percentage of control from top left corner
const YFloatType kSeperatorHeadlineOffsetY = 0.49; // Percentage of control from top left corner

// ****************************************************************************
//
//  Function Name:	RSignatureDialog::RSignatureDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RSignatureDialog::RSignatureDialog( RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent, int nResourceId )
	: RCommonGroupedObjectDialog(pParentDocument,pEditedComponent,nResourceId),
	m_pGraphicCollectionSearcher( NULL ),
	m_pSeperatorHeadlineInterface( NULL ),
	m_pSeperatorHeadlineView( NULL )
{
	m_rHeadlineText = ::GetResourceManager().GetResourceString( IDS_DEFAULT_SIGNATURE );
}

// ****************************************************************************
//
//  Function Name:	RSignatureDialog::RSignatureDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RSignatureDialog::~RSignatureDialog()
{
	delete m_pGraphicCollectionSearcher;
	if (m_pSeperatorHeadlineInterface)
	{
		delete m_pSeperatorHeadlineInterface;
	}
}


// ****************************************************************************
//
//  Function Name:	RSignatureDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::DoDataExchange(CDataExchange* pDX)
{
	RCommonGroupedObjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RSignatureDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RSignatureDialog, RCommonGroupedObjectDialog)
	//{{AFX_MSG_MAP(RSignatureDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RSignatureDialog message handlers


// ****************************************************************************
//
//  Function Name:	RSignatureDialog::OnInitDialog
//
//  Description:		dialog initialization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL RSignatureDialog::OnInitDialog() 
{
	if( !RCommonGroupedObjectDialog::OnInitDialog() ) return FALSE;

	//
	// Create the seperator headline and set it to be unselectable..
	SetupSeperator();

	m_cHeadlineEditCtrl.SetLimitText( kMaxSignatureChars );
	// Only need to setup the seperator once..
	m_rSeperatorHeadlineData.m_rHeadlineText = kSeperatorLine; 
	m_rSeperatorHeadlineData.m_fDistort = TRUE;
	m_rSeperatorHeadlineData.m_rOutlineFillColor = m_rColorEffect;
	m_rSeperatorHeadlineData.UpdateHeadline( m_pSeperatorHeadlineInterface );

	m_fInitializing = FALSE;
	UpdatePreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// ****************************************************************************
//
//  Function Name:	RSignatureDialog::GetDefaultHeadlineRect
//
//  Description:		Setup default headline placement info
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RRealRect RSignatureDialog::GetDefaultHeadlineRect()
{	
	//
	// Need to just return top headline rect here..
	RRealSize rSignatureHeadlineSize( m_pControlView->GetSize());
	rSignatureHeadlineSize.Scale( kSignatureHeadlineScaleSize );
	RRealPoint rSignatureHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * kSignatureHeadlineOffsetX, m_pControlView->GetSize().m_dy * kSignatureHeadlineOffsetY );
	RRealRect rSignatureHeadlineRect( rSignatureHeadlineTopLeftPoint, rSignatureHeadlineSize );
	return rSignatureHeadlineRect;
}

// ****************************************************************************
//
//  Function Name:	RSignatureDialog::SetupSeperator
//
//  Description:		Setup the seperator headline
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::SetupSeperator()
{
	RComponentAttributes componentAttributes;
	// Seperator headline are not selectatable
	componentAttributes.SetSelectable( FALSE );

	RRealSize rSepearatorHeadlineSize( m_pControlView->GetSize());
	rSepearatorHeadlineSize.Scale( kSeperatorHeadlineScaleSize );
	RRealPoint rSeperatorHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * kSeperatorHeadlineOffsetX, m_pControlView->GetSize().m_dy * kSeperatorHeadlineOffsetY );
	RRealRect rSeperatorHeadlineRect( rSeperatorHeadlineTopLeftPoint, rSepearatorHeadlineSize );
	// You can enable the following code(and comment out the above code) to easily
	// find your offsets for headline placement in the debugger (ie. you can change
	// tempx and tempy in the debugger).
//	YFloatType tempx, tempy;
//	tempx = kSeperatorHeadlineOffsetX;
//	tempy = kSeperatorHeadlineOffsetY;
//	RRealPoint rSeperatorHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * tempx, m_pControlView->GetSize().m_dy * tempy );
//	RRealRect rSeperatorHeadlineRect( rSeperatorHeadlineTopLeftPoint, rSepearatorHeadlineSize );

	// Add seperator headline and save interface pointer
	m_pSeperatorHeadlineView = AddComponent( kHeadlineComponent, rSeperatorHeadlineRect, componentAttributes );
	// Ensure we are not overwriting an existing interface without cleaning it up first.
	if (m_pSeperatorHeadlineInterface != NULL)
	{
		delete m_pSeperatorHeadlineInterface;
	}
	m_pSeperatorHeadlineInterface = static_cast<RHeadlineInterface*>(m_pSeperatorHeadlineView->GetInterface( kHeadlineInterfaceId ));
	if ( m_pSeperatorHeadlineInterface )
		m_pSeperatorHeadlineInterface->SetNoDisplayEnvelop( TRUE );
}

// ****************************************************************************
//
//  Function Name:	RSignatureDialog::CreateEffectsList
//
//  Description:		Create '|' delimited string of effects names(in this case, nothing)
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::CreateEffectsList( RMBCString& rList )
{
	// We don't have any effects for signatures, but CreateEffectsList is
	// a pure virtual function in the base class, so we must define it.
	rList = " ";
}
// ****************************************************************************
//
//  Function Name:	RSignatureDialog::CreateGraphicsList
//
//  Description:		Create '|' delimited string of graphics names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::CreateGraphicsList( RMBCString& rList )
{
	//
	// Only allocate memory for the collection searcher once..
	if( m_pGraphicCollectionSearcher == NULL )
	{
		m_pGraphicCollectionSearcher = new 	RCollectionSearcher<RSignatureGraphicSearchCollection>(m_rCollectionArray);
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
//  Function Name:	RSignatureDialog::OnSelchangeListboxGraphic
//
//  Description:		Background graphic selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::OnSelchangeListboxGraphics() 
{
	//
	// Get the SearchResult object for selected graphic
	int nCurSel = m_cGraphicsListBox.GetCurSel();

	//	Reset the default signature if we select the NONE option.
	if ( 0 == nCurSel )
	{
		m_rHeadlineText = ::GetResourceManager().GetResourceString( IDS_DEFAULT_SIGNATURE );
		UpdateData( FALSE );
		UpdatePreview( TRUE, FALSE );
	}

	SearchResult rSelGraphic;
	rSelGraphic = m_cGraphicArray[ nCurSel ];
	DisplayGraphic( rSelGraphic, m_pGraphicDoc );
}

// ****************************************************************************
//
//  Function Name:	RSignatureDialog::DisplayGraphic
//
//  Description:		Display selected graphic in preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc ) 
{
	RCommonGroupedObjectDialog::DisplayGraphic( rSelGraphic, pGraphicDoc );
	// Make sure the graphic document isn't NULL
	if( pGraphicDoc == NULL ) return;

	RComponentView* pGraphicView = pGraphicDoc->GetView( m_pControlView );

	// Scale the signature 
	pGraphicView->Scale( pGraphicView->GetBoundingRect().GetCenterPoint(), RRealSize( kGraphicScaleFactor, kGraphicScaleFactor ), FALSE);

	// Offset down so that the signature baseline is on the control view center
	YRealDimension graphicHeight = pGraphicView->GetBoundingRect( ).Height( );
	pGraphicView->Offset( RRealSize( 0, ( m_pControlView->GetSize( ).m_dy / kGraphicOffsetDivisorY ) - graphicHeight ) );
	// Only set the graphic text to the signature name if we are not initializing,
	// otherwise we may overwrite user-defined text...
	if( !m_fInitializing ) m_rHeadlineText = RMBCString( (LPSTR)rSelGraphic.namePtr );
	UpdateData( FALSE );
	UpdatePreview( TRUE, FALSE );
}


// ****************************************************************************
//
//  Function Name:	RSignatureDialog::DisplayGraphic
//
//  Description:		Display selected graphic in the preview given the new and
//							old documents to work with.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::DisplayGraphic( RComponentDocument *pNewGraphicDoc, RComponentDocument*& pGraphicDoc )
{
	RCommonGroupedObjectDialog::DisplayGraphic( pNewGraphicDoc, pGraphicDoc );
	// Make sure the graphic document isn't NULL
	if( pGraphicDoc == NULL ) return;

	RComponentView* pGraphicView = pGraphicDoc->GetView( m_pControlView );

	// Scale the signature 
	pGraphicView->Scale( pGraphicView->GetBoundingRect().GetCenterPoint(), RRealSize( kGraphicScaleFactor, kGraphicScaleFactor ), FALSE);

	// Offset down so that the signature baseline is on the control view center
	YRealDimension graphicHeight = pGraphicView->GetBoundingRect( ).Height( );
	pGraphicView->Offset( RRealSize( 0, ( m_pControlView->GetSize( ).m_dy / kGraphicOffsetDivisorY ) - graphicHeight ) );

	// Unlike the above DisplayGraphic call, this one does not perform an
	// initialization so the headline text is not set.  This version should only
	// be used to establish an editing session for an existing object.

	UpdatePreview( TRUE, FALSE );
}


// ****************************************************************************
//
//  Function Name:	RSignatureDialog::OnSelchangeListboxEffects
//
//  Description:		Headline/Cap Effect selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::OnSelchangeListboxEffects() 
{
	TpsAssertAlways( "How the hell did you end up here?!!!" );
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::UpdatePreview
//
//  Description:		Something effecting the preview has changed, so get
//							all pertaining data and redraw it.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSignatureDialog::UpdatePreview( BOOLEAN fUpdateHeadline, BOOLEAN fUpdateGraphic )
{
	if( m_fInitializing ) return;
	RCommonGroupedObjectDialog::UpdatePreview( fUpdateHeadline, fUpdateGraphic );
	if( fUpdateHeadline )
	{
		m_rSeperatorHeadlineData.m_rOutlineFillColor	= m_rColorEffect;
		m_rSeperatorHeadlineData.UpdateHeadline( m_pSeperatorHeadlineInterface );
	}
}
// ****************************************************************************
//
//  Function Name:	RSignatureDialog::GetComponentType
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
const YComponentType& RSignatureDialog::GetComponentType( ) const
	{
	static YComponentType componentType = kSignatureComponent;

	return componentType;
	}


// ****************************************************************************
//
//  Function Name:	RSignatureDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSignatureDialog::WriteUIContextData( RUIContextData& contextData ) const
{
	// Write common data..
	RCommonGroupedObjectDialog::WriteUIContextData( contextData );

	//	Write extra data at the current end of the context data.
	RCommonGroupedObjectDialog::WriteVersion2UIContextData( contextData );
}

// ****************************************************************************
//
//  Function Name:	RSignatureDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RSignatureDialog::ReadUIContextData( ) 
{
	RUIContextData* pContextData = NULL;
	pContextData = RCommonGroupedObjectDialog::ReadUIContextData();

	// Read common data..
	if ( pContextData )
		pContextData 
		 = RCommonGroupedObjectDialog::ReadVersion2UIContextData( *pContextData );

	return pContextData;
}
