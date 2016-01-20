/*	$Header: /PM8/App/SealDialog.cpp 1     3/03/99 6:11p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a Seal compound graphic.
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
// $Log: /PM8/App/SealDialog.cpp $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     2/10/99 5:08p Rgrenfel
// Created a new DisplayGraphic call that takes the new and old component
// documents instead of performing a collection read.
// 
// 1     2/01/99 5:39p Rgrenfel
// Constructs a new Seal component object.
*/

#include "StdAfx.h"

ASSERTNAME

#include "SealDialog.h"
#include "ComponentTypes.h"
#include "ComponentView.h"
#include "ComponentDocument.h"
#include "ResourceManager.h"
#include "WinColorDlg.h"

const int kMaxTopTextChars = 65;
const int kMaxBottomTextChars = 65;

const int kTopTextWarpPathId		= kWarpPath120;
const int kBottomTextWarpPathId  = kWarpPath130;

const YFloatType kPercentToShrinkInside = .60;

const RRealSize  kTopHeadlineScaleSize( 1, .312 ); 
const YFloatType kTopHeadlineOffsetX = (1.0 - kTopHeadlineScaleSize.m_dx) / 2.0; // Percentage of control from top left corner
const YFloatType kTopHeadlineOffsetY = 0.15;	// Percentage of control from top left corner

const RRealSize  kBottomHeadlineScaleSize( kTopHeadlineScaleSize ); 
const YFloatType kBottomHeadlineOffsetX = kTopHeadlineOffsetX; // Percentage of control from top left corner
const YFloatType kBottomHeadlineOffsetY = 0.54;	// Percentage of control from top left corner


const RRealSize kBulletScaleSize( .06, .06 ); 
const YFloatType kLeftBulletOffset			= .134;
const YFloatType kRightBulletOffset			= .802;
const RMBCString kBulletText					= "\267"; //the bullet char in symbol font

RMBCString kBulletFont;

// ****************************************************************************
//
//  Function Name:	RSealDialog::RSealDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RSealDialog::RSealDialog( RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent, int nResourceId )
	: RCommonGroupedObjectDialog(pParentDocument,pEditedComponent,nResourceId),
	m_pInnerGraphicCollectionSearcher( NULL ),
	m_pOuterGraphicCollectionSearcher( NULL ),
	m_pBottomHeadlineInterface( NULL ),
	m_pBottomHeadlineView( NULL ),
	m_pOuterGraphicDoc( NULL ),
	m_pOuterGraphicInterface( NULL ),
	m_fShowBullets( FALSE ),
	m_pLeftBulletView( NULL ),
	m_rColorEdgeGraphic( RColor::kTransparent ),
	m_pRightBulletView( NULL ),
	m_cEdgeGraphicColorBtn( RWinColorBtn::kInset ),
	m_uSelectedEdgeGraphicId( kInvalidGraphicId )
{
	kBulletFont = ::GetResourceManager().GetResourceString( IDS_MISC_BULLETFONTNAME );//"Symbol";
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::~RSealDialog
//
//  Description:		Dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RSealDialog::~RSealDialog()
{
	delete m_pInnerGraphicCollectionSearcher;
	delete m_pOuterGraphicCollectionSearcher;
	if (m_pBottomHeadlineInterface != NULL)
	{
		delete m_pBottomHeadlineInterface;
	}
}


// ****************************************************************************
//
//  Function Name:	RSealDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::DoDataExchange(CDataExchange* pDX)
{
	RCommonGroupedObjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RSealDialog)
	CString cBottomText( (LPCSZ)m_rBottomText );
	DDX_Text( pDX, CONTROL_EDIT_SEAL_BOTTOMTEXT, cBottomText );
	m_rBottomText = cBottomText;

	DDX_Control( pDX, CONTROL_EDIT_SEAL_BOTTOMTEXT, m_cBottomTextEditCtrl );
	DDX_Control( pDX, CONTROL_BUTTON_SEAL_COLORGRAPHIC_EDGE, m_cEdgeGraphicColorBtn );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RSealDialog, RCommonGroupedObjectDialog)
	//{{AFX_MSG_MAP(RSealDialog)
	ON_BN_CLICKED(CONTROL_CHKBOX_SEAL_SHOWBULLETS, OnButtonShowBullets)
	ON_BN_CLICKED(CONTROL_BUTTON_SEAL_COLORGRAPHIC_EDGE, OnButtonColorEdge)
	ON_EN_CHANGE(CONTROL_EDIT_SEAL_BOTTOMTEXT, OnEditChangeBottomText)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// RSealDialog message handlers


// ****************************************************************************
//
//  Function Name:	RSealDialog::OnInitDialog
//
//  Description:		dialog initialization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL RSealDialog::OnInitDialog() 
{
	// Setup up base members..
	m_rHeadlineData = RGOHeadlineData( kFollowPath, kTopTextWarpPathId, FALSE );
	m_rHeadlineText = ::GetResourceManager().GetResourceString( IDS_GO_DEFAULT_SEALTOP );
	m_rBottomHeadlineData = RGOHeadlineData( kFollowPath, kBottomTextWarpPathId, FALSE );
	m_rBottomText	= ::GetResourceManager().GetResourceString( IDS_GO_DEFAULT_SEALBOTTOM );

	// call base init..
	if( !RCommonGroupedObjectDialog::OnInitDialog() ) return FALSE;
	//
	// Set the top headline to be unselectable and create and do the same
	// for the bottom headline...
	SetupHeadlines();

	//
	// now that we're setup, do the rest of our initialization...
	m_cHeadlineEditCtrl.SetLimitText( kMaxTopTextChars );
	m_cBottomTextEditCtrl.SetLimitText( kMaxBottomTextChars );

	//
	// Set up the zeroth index of the Graphic map to be the "None" selection
	SearchResult rSelNone;
	rSelNone.namePtr = NULL;
	m_cOuterGraphicArray[0] = rSelNone;

	SearchResult rSelGraphic;
	rSelGraphic = m_cOuterGraphicArray[ m_cEffectsListBox.GetCurSel() ];
	m_cEdgeGraphicColorBtn.EnableWindow( FALSE );
	DisplayGraphic( rSelGraphic, m_pOuterGraphicDoc, m_uSelectedEdgeGraphicId );

	ShowOrHideBullets( m_fShowBullets );
	if( m_fShowBullets )
	{
		CButton* pCheckBtn = (CButton*)GetDlgItem( CONTROL_CHKBOX_SEAL_SHOWBULLETS );
		pCheckBtn->SetCheck( 1 );
	}
	
	//	Show the current color in the color button
	m_cEdgeGraphicColorBtn.SetColor( m_rColorEdgeGraphic );

	m_fInitializing = FALSE;
	UpdatePreview();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::GetDefaultHeadlinePlacement
//
//  Description:		Setup default headline placement info
//
//  Returns:			Headline's rect
//
//  Exceptions:		None
//
// ****************************************************************************
RRealRect RSealDialog::GetDefaultHeadlineRect()
{
	//
	// Need to just return top headline rect here..
	RRealSize rTopHeadlineSize( m_pControlView->GetSize());
	rTopHeadlineSize.Scale( kTopHeadlineScaleSize );
	RRealPoint rHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * kTopHeadlineOffsetX, m_pControlView->GetSize().m_dy * kTopHeadlineOffsetY );
	RRealRect rHeadlineRect( rHeadlineTopLeftPoint, rTopHeadlineSize );
	// You can enable the following code(and comment out the above code) to easily
	// find your offsets for headline placement in the debugger (ie. you can change
	// tempx and tempy in the debugger).
	//RRealSize rTopHeadlineSize( m_pControlView->GetSize());
	//RRealSize rTempSize( kTopHeadlineScaleSize );
	//rTopHeadlineSize.Scale( rTempSize );
	//YFloatType tempx, tempy;
	//tempx = kTopHeadlineOffsetX;
	//tempy = kTopHeadlineOffsetY;
	//RRealPoint rHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * tempx, m_pControlView->GetSize().m_dy * tempy );
	//RRealRect rHeadlineRect( rHeadlineTopLeftPoint, rTopHeadlineSize );
	return rHeadlineRect;
}
// ****************************************************************************
//
//  Function Name:	RSealDialog::SetupHeadlines
//
//  Description:		Setup the bottom seal headline
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::SetupHeadlines()
{
	RComponentAttributes componentAttributes;
	componentAttributes = m_pHeadlineView->GetComponentAttributes();
	// Seal headlines are not selectatable
	componentAttributes.SetSelectable( FALSE );
	m_pHeadlineView->GetComponentDocument()->SetComponentAttributes( componentAttributes );
	// We never want to use the headline rect read in in ReadUIContextData for Seals
	// because there have been cases where the rect read in has been invalid and the
	// the Seal headline rects should always be the same anyway.
	m_pHeadlineView->SetBoundingRect( GetDefaultHeadlineRect() );

	// Setup and position the bottom headline
	// Create bottom headline bounding rect
	RRealSize rBottomHeadlineSize( m_pControlView->GetSize());
	rBottomHeadlineSize.Scale( kBottomHeadlineScaleSize );
	RRealPoint rBottomHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * kBottomHeadlineOffsetX, m_pControlView->GetSize().m_dy * kBottomHeadlineOffsetY );
	RRealRect rBottomHeadlineRect( rBottomHeadlineTopLeftPoint, rBottomHeadlineSize );
	// You can enable the following code(and comment out the above code) to easily
	// find your offsets for headline placement in the debugger (ie. you can change
	// tempx and tempy in the debugger).
	//RRealSize rBottomHeadlineSize( m_pControlView->GetSize());
	//RRealSize rTempSize( kBottomHeadlineScaleSize );
	//rBottomHeadlineSize.Scale( rTempSize );
	//YFloatType tempx, tempy;
	//tempx = kBottomHeadlineOffsetX;
	//tempy = kBottomHeadlineOffsetY;
	//RRealPoint rBottomHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * tempx, m_pControlView->GetSize().m_dy * tempy );
	//RRealRect rBottomHeadlineRect( rBottomHeadlineTopLeftPoint, rBottomHeadlineSize );

	// Add bottom headline and save interface pointer
	m_pBottomHeadlineView = AddComponent( kHeadlineComponent, rBottomHeadlineRect, componentAttributes );
	m_pBottomHeadlineInterface = static_cast<RHeadlineInterface*>(m_pBottomHeadlineView->GetInterface( kHeadlineInterfaceId ));
}
// ****************************************************************************
//
//  Function Name:	RSealDialog::CreateGraphicsList
//
//  Description:		Create '|' delimited string of inner graphics names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::CreateGraphicsList( RMBCString& rList )
{
	//
	// Only allocate memory for the collection searcher once..
	if( m_pInnerGraphicCollectionSearcher == NULL )
	{
		m_pInnerGraphicCollectionSearcher = new 	RCollectionSearcher<RSealInnerGraphicSearchCollection>(m_rCollectionArray);
	}
	// Find all the graphics
	RSearchResultArray* pResultArray = m_pInnerGraphicCollectionSearcher->SearchCollections();

	// Now iterate through all the graphics and insert them into
	// the string
	RSearchResultIterator resultIter( pResultArray->Start() );
	RSearchResultIterator resultIterEnd( pResultArray->End() );

	// The index has to start at 1 because the None selection is at zero
	for (int nIdx = 1; resultIter != resultIterEnd; resultIter++, nIdx++ )
	{
		rList += RMBCString( (LPSTR)(*resultIter).namePtr );
		rList += RMBCString( kListFieldSeperator );

		m_cGraphicArray.SetAtGrow( nIdx, (*resultIter) );
   }
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::CreateEffectsList
//
//  Description:		Create '|' delimited string of inner graphics names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::CreateEffectsList( RMBCString& rList )
{
	//
	// Only allocate memory for the collection searcher once..
	if( m_pOuterGraphicCollectionSearcher == NULL )
	{
		m_pOuterGraphicCollectionSearcher = new 	RCollectionSearcher<RSealOuterGraphicSearchCollection>(m_rCollectionArray);
	} 
	// Find all the graphics
	RSearchResultArray* pResultArray = m_pOuterGraphicCollectionSearcher->SearchCollections();

	// Now iterate through all the graphics and insert them into
	// the string
	RSearchResultIterator resultIter( pResultArray->Start() );
	RSearchResultIterator resultIterEnd( pResultArray->End() );

	rList = kNoneSelection;
	rList += kListFieldSeperator;
	// The index has to start at 1 because the None selection is at zero
	for (int nIdx = 1; resultIter != resultIterEnd; resultIter++, nIdx++ )
	{
		rList += RMBCString( (LPSTR)(*resultIter).namePtr );
		rList += kListFieldSeperator;

		m_cOuterGraphicArray.SetAtGrow( nIdx, (*resultIter) );
   }
}
// ****************************************************************************
//
//  Function Name:	RSealDialog::OnSelchangeListboxGraphic
//
//  Description:		Background graphic selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::OnSelchangeListboxGraphics() 
{
	// Load selected inner graphic
	SearchResult rSelGraphic;
	rSelGraphic = m_cGraphicArray[ m_cGraphicsListBox.GetCurSel() ];
	DisplayGraphic( rSelGraphic, m_pGraphicDoc, m_uSelectedGraphicId );
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::OnButtonGOOtherGraphics
//
//  Description:		Headline/Cap Effect selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::OnSelchangeListboxEffects() 
{
	// Load selected outer graphic
	SearchResult rSelGraphic;
	rSelGraphic = m_cOuterGraphicArray[ m_cEffectsListBox.GetCurSel() ];
	
	//	Remember the interior graphic id since the base method modifies
	//	it incorrectly. Remember the new edge graphic id.
	DisplayGraphic( rSelGraphic, m_pOuterGraphicDoc, m_uSelectedEdgeGraphicId );
}
// ****************************************************************************
//
//  Function Name:	RSealDialog::DisplayGraphic
//
//  Description:		Display selected graphic in preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc, uLONG& uSelectedGraphicId ) 
{
	RCommonGroupedObjectDialog::DisplayGraphic( rSelGraphic, pGraphicDoc, uSelectedGraphicId );

	// Make sure the current graphic document being displayed isn't NULL
	if( pGraphicDoc == NULL )
	{
		// The graphic doc being displayed is NULL, but the other graphic in
		// a seal may not be null, so we check to see if the color button should
		// be enabled/disabled. NOTE: the graphic doc parameter is not used...
		CheckForMonochromeGraphic( pGraphicDoc );
		return;
	}

	if( pGraphicDoc == m_pGraphicDoc ) // see if its the inside graphic
	{
		//
		// Shrink inside graphic so it fits properly...
		RComponentView* pGraphicView = pGraphicDoc->GetView( m_pControlView );
		pGraphicView->FitInsideParent();
		pGraphicView->Scale( pGraphicView->GetBoundingRect().GetCenterPoint(), RRealSize( kPercentToShrinkInside, kPercentToShrinkInside ), FALSE);
	}
	else
	{
		// We don't need to resize the outer graphic.
		TpsAssert( pGraphicDoc == m_pOuterGraphicDoc, "Invalid graphic doc" );
	}

	m_pControlDocument->InvalidateAllViews();
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::DisplayGraphic
//
//  Description:		Display selected graphic in the preview given the new and
//							old documents to work with.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::DisplayGraphic( RComponentDocument *pNewGraphicDoc, RComponentDocument*& pGraphicDoc )
{
	RCommonGroupedObjectDialog::DisplayGraphic( pNewGraphicDoc, pGraphicDoc );

	// Make sure the current graphic document being displayed isn't NULL
	if( pGraphicDoc == NULL )
	{
		// The graphic doc being displayed is NULL, but the other graphic in
		// a seal may not be null, so we check to see if the color button should
		// be enabled/disabled. NOTE: the graphic doc parameter is not used...
		CheckForMonochromeGraphic( pGraphicDoc );
		return;
	}

	if( pGraphicDoc == m_pGraphicDoc ) // see if its the inside graphic
	{
		//
		// Shrink inside graphic so it fits properly...
		RComponentView* pGraphicView = pGraphicDoc->GetView( m_pControlView );
		pGraphicView->FitInsideParent();
		pGraphicView->Scale( pGraphicView->GetBoundingRect().GetCenterPoint(), RRealSize( kPercentToShrinkInside, kPercentToShrinkInside ), FALSE);
	}
	else
	{
		// We don't need to resize the outer graphic.
		TpsAssert( pGraphicDoc == m_pOuterGraphicDoc, "Invalid graphic doc" );
	}

	m_pControlDocument->InvalidateAllViews();
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::CheckForMonochromeGraphic
//
//  Description:		Enables the color button if the either graphic is monochrome.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSealDialog::CheckForMonochromeGraphic( RComponentDocument*& /*pGraphicDoc*/ )
{
	BOOLEAN rInnerGraphicIsMonochrome = FALSE, rOuterGraphicIsMonochrome = FALSE;
	if( m_pGraphicDoc ) 
	{
		RComponentView* pGraphicView = m_pGraphicDoc->GetView( m_pControlView );
		//
		// Check to see if graphic is monochrome..
		RInterface *pColorSettingsInterface = dynamic_cast<RColorSettingsInterface*>( pGraphicView->GetInterface(kColorSettingsInterfaceId) );
		if ( pColorSettingsInterface )
		{
			rInnerGraphicIsMonochrome = TRUE;
			m_cGraphicColorBtn.EnableWindow( TRUE );
			// If the dialog is initializing, its initializing with a monochrome graphic
			// so we want to preserve its last color, therefore don't reset the graphic color.
			if( !m_fInitializing )
			{
				//m_rColorGraphic = RColor( RColor::kTransparent );
			}
			delete pColorSettingsInterface;
		}
		else
		{
			m_cGraphicColorBtn.EnableWindow( FALSE );
		}
	}
	if( m_pOuterGraphicDoc ) 
	{
		RComponentView* pGraphicView = m_pOuterGraphicDoc->GetView( m_pControlView );
		//
		// Check to see if graphic is monochrome..
		RInterface *pColorSettingsInterface = dynamic_cast<RColorSettingsInterface*>( pGraphicView->GetInterface(kColorSettingsInterfaceId) );
		if ( pColorSettingsInterface )
		{
			rOuterGraphicIsMonochrome = TRUE;
			m_cEdgeGraphicColorBtn.EnableWindow( TRUE );
			// If the dialog is initializing, its initializing with a monochrome graphic
			// so we want to preserve its last color, therefore don't reset the graphic color.
			if( !m_fInitializing )
			{
				//m_rColorEdgeGraphic = RColor( RColor::kTransparent );
			}
			delete pColorSettingsInterface;
		}
		else
		{
			m_cEdgeGraphicColorBtn.EnableWindow( FALSE );
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::OnEditChangeBottomText
//
//  Description:		Bottom text is changing, reset timer.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::OnEditChangeBottomText()
{
	// Change the text settings
	CString cBottomText;
	m_cBottomTextEditCtrl.GetWindowText(cBottomText);
	m_rBottomText = RMBCString(cBottomText);

	// Restart the timer
	BOOL fResult = KillTimer(m_uTimerID);
	m_uTimerID = SetTimer(kTimerID, kRenderTimeout, NULL);
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::OnButtonShowBullets
//
//  Description:		Hide/Show bullets
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::OnButtonShowBullets()
{
	m_fShowBullets = !m_fShowBullets;
	ShowOrHideBullets( m_fShowBullets );
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::ShowOrHideBullets
//
//  Description:		Hide or show seal bullets based on passed in boolean
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::ShowOrHideBullets( BOOLEAN fShowBullets )
{
	// If we're initializing and fShowBullets is false, we don't need to do 
	// anything so just return..
	if( m_fInitializing && !fShowBullets ) return;

	if( fShowBullets )
	{
		// Data structure for bullets..
		RGOHeadlineData rBulletData;
		rBulletData.m_rHeadlineText = kBulletText;
		rBulletData.m_rFontName = kBulletFont;
		rBulletData.m_rOutlineFillColor	= m_rColorEffect;
		// Create and position the left bullet headline..
		RComponentAttributes componentAttributes;
		componentAttributes.SetRotatable( FALSE );
		componentAttributes.SetSelectable( FALSE );
		// Create left bullet bounding rect
		RRealSize rBulletSize( m_pControlView->GetSize());
		rBulletSize.Scale( kBulletScaleSize );
		RRealPoint rBulletTopLeftPoint( m_pControlView->GetSize().m_dx * kLeftBulletOffset, ((m_pControlView->GetSize().m_dy / 2.0) - rBulletSize.m_dy / 2.0) );
		RRealRect leftBulletRect( rBulletTopLeftPoint, rBulletSize );

		// Add left headline and save interface pointer
		m_pLeftBulletView = AddComponent( kHeadlineComponent, leftBulletRect, componentAttributes );
		RHeadlineInterface *pInterface = static_cast<RHeadlineInterface*>(m_pLeftBulletView->GetInterface( kHeadlineInterfaceId ));
	   rBulletData.UpdateHeadline(pInterface);
		delete pInterface;

		// Create and position the right bullet headline..
		rBulletSize = m_pControlView->GetSize();
		rBulletSize.Scale( kBulletScaleSize );
		rBulletTopLeftPoint = RRealPoint( m_pControlView->GetSize().m_dx * kRightBulletOffset, ((m_pControlView->GetSize().m_dy / 2.0) - rBulletSize.m_dy / 2.0) ) ;
		RRealRect rightBulletRect( rBulletTopLeftPoint, rBulletSize );

		// Add left headline and save interface pointer
		m_pRightBulletView = AddComponent( kHeadlineComponent, rightBulletRect, componentAttributes );
		pInterface = static_cast<RHeadlineInterface*>(m_pRightBulletView->GetInterface( kHeadlineInterfaceId ));
	   rBulletData.UpdateHeadline(pInterface);
		delete pInterface;
	}
	else
	{
		TpsAssert( m_pRightBulletView && m_pLeftBulletView, "Null bullet view pointer" );
		RemoveComponent( m_pLeftBulletView->GetComponentDocument() );
		RemoveComponent( m_pRightBulletView->GetComponentDocument() );
		m_pLeftBulletView = m_pRightBulletView = NULL;
	}
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::OnButtonColorEdge
//
//  Description:		Displays color dialog for edge graphic color.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSealDialog::OnButtonColorEdge()
{
	//
	// Bring up the color dialog, and don't allow gradients
	RWinColorDlg	rColorDlg( NULL, 0 );
	//
	// Set the current graphic color into the dialog
	rColorDlg.SetColor( m_rColorEdgeGraphic );
	//
	// Pop up the color dialog
	if( rColorDlg.DoModal() == IDOK )
	{
		m_rColorEdgeGraphic = rColorDlg.SelectedSolidColor();
		
		//	Show the current color in the color button
		m_cEdgeGraphicColorBtn.SetColor( m_rColorEdgeGraphic );

		UpdatePreview( FALSE, TRUE );
	}
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::UpdatePreview
//
//  Description:		Something effecting the preview has changed, so get
//							all pertaining data and redraw it.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RSealDialog::UpdatePreview( BOOLEAN fUpdateHeadline, BOOLEAN fUpdateGraphic )
{
	if( m_fInitializing ) return;

	if( fUpdateHeadline )
	{
		// If the headline is nothing, set it to a space because the headline
		// engine rejects empty headlines.

		if( m_rHeadlineText == RMBCString("") )
		{
			m_rHeadlineData.m_rHeadlineText		= " ";
	 		m_pHeadlineInterface->SetNoDisplayEnvelop( TRUE );
		}
		else
		{
			m_rHeadlineData.m_rHeadlineText		= m_rHeadlineText;
		}
		m_rHeadlineData.m_rFontName			= m_rFontName;
		m_rHeadlineData.m_rOutlineFillColor	= m_rColorEffect;
		m_rHeadlineData.m_yFontAttributes	= m_yFontAttributes;
		m_pHeadlineInterface->SetVerticalPlacement( kPin2Bottom );
		//
		// Invalidate the render cache to be sure headline gets updated..
		m_pHeadlineView->InvalidateRenderCache();
		//
		// Apply settings to interface to preserve headline size in seals..
		m_rHeadlineData.UpdateHeadline( m_pHeadlineInterface );

		// If the headline is nothing, set it to a space because the headline
		// engine rejects empty headlines.
		if( m_rBottomText == RMBCString("") )
		{
			m_rBottomHeadlineData.m_rHeadlineText		= " ";
	 		m_pBottomHeadlineInterface->SetNoDisplayEnvelop( TRUE );
		}
		else
		{
			m_rBottomHeadlineData.m_rHeadlineText		= m_rBottomText;
		}
		m_rBottomHeadlineData.m_rFontName			= m_rFontName;
		m_rBottomHeadlineData.m_rOutlineFillColor	= m_rColorEffect;
		m_rBottomHeadlineData.m_yFontAttributes	= m_yFontAttributes;

		m_pBottomHeadlineInterface->SetVerticalPlacement( kPin2Bottom );
		//
		// Invalidate the render cache to be sure headline gets updated..
		m_pBottomHeadlineView->InvalidateRenderCache();
		m_rBottomHeadlineData.UpdateHeadline( m_pBottomHeadlineInterface );

		// If the bullets exist, update them as well (just their color really)
		if( m_fShowBullets )
		{
			TpsAssert( m_pLeftBulletView && m_pRightBulletView, "Null Bullet views" );
			// Data structure for bullets..
			RGOHeadlineData rBulletData;
			rBulletData.m_rHeadlineText		= kBulletText;
			rBulletData.m_rFontName				= kBulletFont;
			rBulletData.m_rOutlineFillColor	= m_rColorEffect;
			m_pLeftBulletView->InvalidateRenderCache();
			m_pRightBulletView->InvalidateRenderCache();
			RHeadlineInterface *pInterface = static_cast<RHeadlineInterface*>(m_pRightBulletView->GetInterface( kHeadlineInterfaceId ));
		   rBulletData.UpdateHeadline(pInterface);
			delete pInterface;
			pInterface = static_cast<RHeadlineInterface*>(m_pLeftBulletView->GetInterface( kHeadlineInterfaceId ));
		   rBulletData.UpdateHeadline(pInterface);
			delete pInterface;
		}


	}

	if( fUpdateGraphic )
	{
		if( m_pGraphicDoc )
		{
			RComponentView* pGraphicView = m_pGraphicDoc->GetView( m_pControlView );
			RColorSettingsInterface* pGraphicInterface = dynamic_cast<RColorSettingsInterface*>( pGraphicView->GetInterface(kColorSettingsInterfaceId) );
			if( pGraphicInterface )
			{
				pGraphicInterface->SetColor( m_rColorGraphic );
				delete pGraphicInterface;
			}
		}
		if( m_pOuterGraphicDoc )
		{
			RComponentView* pGraphicView = m_pOuterGraphicDoc->GetView( m_pControlView );
			RColorSettingsInterface* pGraphicInterface = dynamic_cast<RColorSettingsInterface*>( pGraphicView->GetInterface(kColorSettingsInterfaceId) );
			if( pGraphicInterface )
			{
				pGraphicInterface->SetColor( m_rColorEdgeGraphic );
				delete pGraphicInterface;
			}
		}
	}

	TpsAssert( m_pControlDocument, "Null control document pointer" );
	m_pControlDocument->InvalidateAllViews();
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::GetComponentType
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
const YComponentType& RSealDialog::GetComponentType( ) const
	{
	static YComponentType componentType = kSealComponent;

	return componentType;
	}

// ****************************************************************************
//
//  Function Name:	RSealDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSealDialog::WriteUIContextData( RUIContextData& contextData ) const
{
	// Write common data..
	RCommonGroupedObjectDialog::WriteUIContextData( contextData );

	// Write out the text
	contextData << m_rBottomText;

	// Write out the show bullet check state
	contextData << m_fShowBullets;

	// Write out the edge graphic color
	contextData << m_rColorEdgeGraphic;

	//	Write extra data at the current end of the context data.
	WriteVersion2UIContextData( contextData );
}

// ****************************************************************************
//
//  Function Name:	RSealDialog::WriteVersion2UIContextData
//
//  Description:		Writes dialogs common UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSealDialog::WriteVersion2UIContextData( RUIContextData& contextData ) const
{
		//	Write extra data at the current end of the context data.
	RCommonGroupedObjectDialog::WriteVersion2UIContextData( contextData );

	//	Prep our versioning system.
	StartContextDataVersion( contextData, kContextDataVersion3 );

	// REVIEW MWH -this would go in WriteUIContextData, but it can't
	// because then we wouldn't be backwards compatiable
	// Write out the selected graphics id
	contextData << m_uSelectedEdgeGraphicId;

}

// ****************************************************************************
//
//  Function Name:	RSealDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RSealDialog::ReadUIContextData( ) 
{
	RUIContextData* pContextData = NULL;
	pContextData = RCommonGroupedObjectDialog::ReadUIContextData();
	// Write common data..
	if ( pContextData )
	{
		try
		{
			// Read in the Bottom headline text and set it into the edit control
			(*pContextData) >> m_rBottomText;
			m_cBottomTextEditCtrl.SetWindowText( m_rBottomText );

			// Read in the show bullet check state
			(*pContextData) >> m_fShowBullets;

			// Read in the edge graphic color
			(*pContextData) >> m_rColorEdgeGraphic;

			// Read common data..
			pContextData 
				 = ReadVersion2UIContextData( *pContextData );
		}
		catch( ... )
		{
		}
	}
	return pContextData;
}


// ****************************************************************************
//
//  Function Name:	RSealDialog::ReadVersion2UIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RSealDialog::ReadVersion2UIContextData( RUIContextData& contextData ) 
{
	RUIContextData* pContextData = NULL;

	// If we are editting a component, try to read its context data
	if( m_pEditedComponent )
	{
		try
		{
			// Read common data..
			pContextData 
				 = RCommonGroupedObjectDialog::ReadVersion2UIContextData( contextData );

			//	Can't proceed if our context data is miss-versioned.
			if ( kContextDataVersion3 != GetContextDataVersion( contextData ) )
				return NULL;

			// REVIEW ESV -this would go in ReadUIContextData, but it can't
			// because then we wouldn't be backwards compatiable.
			// Read in the selected graphics id
			contextData >> m_uSelectedEdgeGraphicId;
		}
		catch( ... )
		{
		}
	}
	return pContextData;
}