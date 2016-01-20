/*	$Header: /PM8/App/CommonGroupedObjectDialog.cpp 1     3/03/99 6:04p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a generic compound graphic.
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
// $Log: /PM8/App/CommonGroupedObjectDialog.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     2/11/99 3:02p Rgrenfel
// Fixed aspect correct handling and flagged the reconstruction of the
// collection arrays.
// 
// 2     2/10/99 5:07p Rgrenfel
// Enabled the more graphics button and changed the edit path to use the
// embedded component rather than reading the object back out of the
// collection.
// 
// 1     2/01/99 5:42p Rgrenfel
// Base class for certain group component object dialogs.
*/

#include "StdAfx.h"

ASSERTNAME

#include "CommonGroupedObjectDialog.h"

#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "ComponentAttributes.h"
//#include "RenaissanceDocument.h"
//#include "RenaissanceResource.h"
//#include "Buttonbmps.h"
#include "DialogUtilities.h"
#include "WinColorDlg.h"
#include "HeadlineInterface.h"
#include "ResourceManager.h"
//#include "GrcBrowser.h"
//#include "PanelView.h"
#include "GraphicCategorySearch.h"
#include "Toolbar.h"
#include "SingleSelection.h"

#include "Pmw.h"
#include "ArtStore.h"
#include "browser.h"
#include "PmgObj.h"
#include "PmwDoc.h"
#include "CompObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const RMBCString	kDefaultGroupedObjectDialog( "NewZurica" );

// This string is just used to mark the graphic when it is saved
// as having come from the browser dialog
const RMBCString kInvalidGraphicString = "!^GraphicBrowser^!";
RMBCString kNoneSelection;

#define kDefaultBkgndColor RGB( 192, 192, 192 );
const YTint kTintIncrement = 10;

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// *********************                             **************************
// *********************  RCommonGroupedObjectDialog **************************
// *********************                             **************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::RCommonGroupedObjectDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RCommonGroupedObjectDialog::RCommonGroupedObjectDialog( RStandaloneDocument* pParentDocument,
																		  RComponentDocument* pEditedComponent,
																		  int nResourceId )
	: RGenericGroupedObjectDialog(pParentDocument,pEditedComponent,nResourceId),
	m_rFontName(""),
	m_rHeadlineText(""),
	m_fBold( FALSE ),
	m_fItalic( FALSE ),
	m_yFontAttributes(0),
	m_fInitializing( TRUE ),
	m_rColorGraphic( RColor::kSolid ),
	m_rColorEffect( RColor::kSolid ),
	m_uTimerID(kInvalidTimerID),
	m_pGraphicDoc( NULL ),
	m_pHeadlineInterface( NULL ),
	m_pHeadlineView( NULL ),
	m_uSelectedGraphicId( kInvalidGraphicId ),
	m_cGraphicColorBtn( RWinColorBtn::kInset ), 
	m_cStyleColorBtn( RWinColorBtn::kInset ),
	m_fBoundingRectPresent( FALSE ), 
	m_fFirstEdit( TRUE ),
	m_nTint( kMaxTint ),
	m_fRebuiltGraphicArrays( FALSE )
{
	kNoneSelection = ::GetResourceManager().GetResourceString( IDS_MISC_NONESELECTION );//"None"
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::~RCommonGroupedObjectDialog
//
//  Description:		Dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RCommonGroupedObjectDialog::~RCommonGroupedObjectDialog()
{
	RCollectionManager::TheCollectionManager().CloseCollectionsOfType( kCustomCollectionType );
	RCollectionManager::TheCollectionManager().CloseCollectionsOfType( kGraphicsCollectionType );

	// Kill the headline interface created...
	if (m_pHeadlineInterface != NULL)
	{
		delete m_pHeadlineInterface;
	}
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	RGenericGroupedObjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RCommonGroupedObjectDialog)
	DDX_Control( pDX, CONTROL_COMBO_COMMONGO_FONT, m_cFontCombo );
	DDX_Control(pDX, CONTROL_LISTBOX_COMMONGO_GRAPHIC, m_cGraphicsListBox);
	DDX_Control(pDX, CONTROL_LISTBOX_COMMONGO_EFFECTS, m_cEffectsListBox);

	BOOL fBold = static_cast<BOOL>(m_fBold);
	DDX_Check( pDX, CONTROL_CHKBOX_COMMONGO_BOLD, fBold );
	BOOL m_fBold = static_cast<BOOLEAN>(fBold);

	BOOL fItalic = static_cast<BOOL>(m_fItalic);
	DDX_Check( pDX, CONTROL_CHKBOX_COMMONGO_ITALIC, fItalic );
	BOOL m_fItalic = static_cast<BOOLEAN>(fItalic);

	DDX_Control( pDX, CONTROL_BUTTON_COMMONGO_COLORGRAPHIC, m_cGraphicColorBtn );
	DDX_Control( pDX, CONTROL_BUTTON_COMMONGO_COLORSTYLE,	  m_cStyleColorBtn );

	CString cText( (LPCSZ)m_rHeadlineText );
	DDX_Text( pDX, CONTROL_EDIT_COMMONGO_HEADLINETEXT, cText );
	m_rHeadlineText  = cText;

	DDX_Control( pDX, CONTROL_EDIT_COMMONGO_HEADLINETEXT, m_cHeadlineEditCtrl );

	DDX_Control( pDX, CONTROL_BUTTON_COMMONGO_TINTGRAPHIC, m_cComboTint );

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RCommonGroupedObjectDialog, RGenericGroupedObjectDialog)
	//{{AFX_MSG_MAP(RCommonGroupedObjectDialog)
	ON_BN_CLICKED(CONTROL_BUTTON_COMMONGO_COLORGRAPHIC, OnButtonGOGraphicColor)
	ON_BN_CLICKED(CONTROL_BUTTON_COMMONGO_COLORSTYLE, OnButtonGOEffectColor)
	ON_BN_CLICKED(CONTROL_CHKBOX_COMMONGO_BOLD, OnButtonGOEffectBold)
	ON_BN_CLICKED(CONTROL_CHKBOX_COMMONGO_ITALIC, OnButtonGOEffectItalic)
	ON_BN_CLICKED(CONTROL_BUTTON_COMMONGO_MOREGRAPHICS, OnButtonGOMoreGraphics)
	ON_LBN_SELCHANGE(CONTROL_LISTBOX_COMMONGO_GRAPHIC, OnSelchangeListboxGraphics)
	ON_LBN_SELCHANGE(CONTROL_LISTBOX_COMMONGO_EFFECTS, OnSelchangeListboxEffects)
	ON_CBN_SELCHANGE(CONTROL_COMBO_COMMONGO_FONT, OnSelchangeComboFont)
	ON_EN_CHANGE(CONTROL_EDIT_COMMONGO_HEADLINETEXT, OnEditChangeHeadlineText)
	ON_CBN_SELCHANGE	(CONTROL_BUTTON_COMMONGO_TINTGRAPHIC, OnCommandTint)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::IsAdditionalGraphic
//
//  Description:		Return TRUE if the selected graphic id isn't in the member graphic array.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RCommonGroupedObjectDialog::IsAdditionalGraphic( ) 
{
	if ( m_uSelectedGraphicId == kInvalidGraphicId )
		return FALSE;	//	No graphic selected, it is not an additional graphic

	BOOLEAN fFound = FALSE;
	
	int nNumBuiltInGraphics = m_cGraphicArray.GetSize( );
	SearchResult rSelGraphic;

	//	Check to see if our graphic is in the member graphic array.
	for( int nIndex = 0; nIndex < nNumBuiltInGraphics; ++nIndex )
	{
		rSelGraphic = m_cGraphicArray[ nIndex ];
		TRACE1( "graphic id %d\n", rSelGraphic.id );
		if ( rSelGraphic.id == m_uSelectedGraphicId )
		{
			fFound = TRUE;
			break;
		}
	}

	//	If the valid graphic id isn't in the member collection then we must need to look it up.
	return !fFound;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnInitDialog
//
//  Description:		dialog initialization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RCommonGroupedObjectDialog::OnInitDialog() 
{
	BeginWaitCursor();

	// Call the base method
	RGenericGroupedObjectDialog::OnInitDialog();

	RMBCString rFontList = (RApplication::GetApplicationGlobals())->GetFontList();
	ParseAndPutInComboBox( rFontList, m_cFontCombo );
	m_rFontName = GetDefaultFont();
	int nFontIndex = m_cFontCombo.FindStringExact( -1, (LPCSZ)m_rFontName );
	if( nFontIndex == LB_ERR ) nFontIndex = 0;
	m_cFontCombo.SetCurSel( nFontIndex );
	//
	// Load color button bmps
	COLORMAP ColorMap;
	ColorMap.from	= kDefaultBkgndColor;
	ColorMap.to		= ::GetSysColor( COLOR_BTNFACE );

	//
	// Load style button bmps
	VERIFY( m_cBoldBmp.LoadMappedBitmap( BITMAP_BUTTON_BOLD_FONT, 0, &ColorMap, 1 ) );
	( ( CButton* )GetDlgItem( CONTROL_CHKBOX_COMMONGO_BOLD ) )->SetBitmap( m_cBoldBmp );
	VERIFY( m_cItalicBmp.LoadMappedBitmap( BITMAP_BUTTON_ITALIC_FONT, 0, &ColorMap, 1 ) );
	( ( CButton* )GetDlgItem( CONTROL_CHKBOX_COMMONGO_ITALIC ) )->SetBitmap( m_cItalicBmp );

	//
	// Prepare collection manager..
	m_rCollectionArray = RCollectionManager::TheCollectionManager().OpenCollectionsOfType( kCustomCollectionType );
	//
	// If we didn't find any collection files, we should generate an
	// error and dismiss the dialog
	if (m_rCollectionArray.Count() == 0)
	{
		EndDialog( IDCANCEL );
		return FALSE;
	}

	RRealRect boundingRect( GetDefaultHeadlineRect() );

	// Its not rotatable
	RComponentAttributes componentAttributes;
		//	Now we will allow rotation of the headline...
//	componentAttributes.SetRotatable( FALSE );

	m_pHeadlineView = AddComponent( kHeadlineComponent, boundingRect, componentAttributes );
	m_pHeadlineInterface = static_cast<RHeadlineInterface*>(m_pHeadlineView->GetInterface( kHeadlineInterfaceId ));


	RMBCString rGraphicsList = "";
	CreateGraphicsList( rGraphicsList );

	RMBCString rEffectsList = "";
	CreateEffectsList( rEffectsList );

	// Load listboxes 
	LoadGraphicsListBox( rGraphicsList );
	LoadEffectsListBox( rEffectsList );

	ReadUIContextData();
	
	//	Set up the base bounding rect if we didn't read it.
	if ( !m_fBoundingRectPresent )
		SetDefaultGraphicBoundingRect( );

	//
	// Set up the zeroth index of the Graphic map to be the "None" selection
	SearchResult rSelNone;
	rSelNone.namePtr = NULL;
	m_cGraphicArray[0] = rSelNone;


	SearchResult rSelGraphic;
	rSelGraphic.namePtr = NULL;


	// Display the graphic
	RCollectionSearcher<RGraphicCategorySearchCollection>*		pGraphicCollectionSearcher = NULL;
	try
	{
#if 0	// *** This process read the image out of the collection each time.
		// *** We are now attempting to copy the graphic component instead to
		// *** avoid any problems with CD switching or deleted graphics.

		//	If our graphic isn't in the member graphic collection then look it up.
		if( IsAdditionalGraphic( ) )
		{
			//
			// See if the collection array has already been initialized.
			// If it hasn't, open the collection..
			if (m_rSquareGraphicCollectionArray.Count() == 0) 
				m_rSquareGraphicCollectionArray 
				 = RCollectionManager::TheCollectionManager().OpenCollectionsOfType( kGraphicsCollectionType );
			//
			// If we didn't find any collection files, we should generate an
			// error and dismiss the dialog
			if (m_rSquareGraphicCollectionArray.Count() > 0)
			{
				//	Get the collection searcher.
				pGraphicCollectionSearcher 
					= new RCollectionSearcher<RGraphicCategorySearchCollection>(m_rSquareGraphicCollectionArray);


				// If there was context data loaded and the graphic name wasn't in the canned
				// list, the graphic must have come from the browser dialog, so see
				// if we can fetch it...
				rSelGraphic = LookupSquareGraphic( m_uSelectedGraphicId, pGraphicCollectionSearcher );
			}
			else
			// If we can't find the external square graphic, just give them the first custom graphic.
			{
				rSelGraphic = m_cGraphicArray[ 0 ];
			}
		}
		else
		{
			rSelGraphic = m_cGraphicArray[ m_cGraphicsListBox.GetCurSel() ];
		}
		DisplayGraphic( rSelGraphic, m_pGraphicDoc );
#endif
		if (m_pEditedComponent != NULL)
		{
			RComponentDocument *pLastDocument = NULL;
			TRACE( "Embedded components for %s are:\n", m_pEditedComponent->GetComponentType() );
			// Look for an embedded component that is not either 
			YComponentIterator iterator = m_pEditedComponent->GetComponentCollectionStart( );
			for( ; iterator != m_pEditedComponent->GetComponentCollectionEnd( ); ++iterator )
			{
				RComponentDocument *pEmbeddedDoc = *iterator;
				YComponentType yType = pEmbeddedDoc->GetComponentType();
				TRACE( "   %s\n", yType );
				if ((yType != kHeadlineComponent) && (yType != kSpecialHeadlineComponent) &&
					 (yType != kVerticalHeadlineComponent) && (yType != kVerticalSpecialHeadlineComponent) )
				{
					// If we have already found one non-headline document, then we
					// at the time of this writing we are dealing with a Seal which
					// has two graphics.  One of them is kept within the other, so
					// we want to keep the smaller one since it is kept at this level.
					if (m_pGraphicDoc != NULL)
					{
						RComponentView *pParentView = m_pEditedComponent->GetView( NULL );	// Doesn't have a parent.
						if (pParentView != NULL)
						{
							YComponentBoundingRect rCurrentCompRect;
							YComponentBoundingRect rNewCompRect;
							RComponentView *pView = pLastDocument->GetView( pParentView );
							if (pView != NULL)
							{
								// OK, now we can get the size of the currently assumed graphic rect.
								rCurrentCompRect = pView->GetBoundingRect();

								// Get the size of the new component we just found.
								pView = pEmbeddedDoc->GetView( pParentView );
								if (pView != NULL)
								{
									rNewCompRect = pView->GetBoundingRect();

									// OK, compare them and replace the existing component if the new one is smaller.
									if (rNewCompRect.Width() < rCurrentCompRect.Width())
									{
										RComponentDocument *pNewDocument = pEmbeddedDoc->Clone();
										if (pNewDocument != NULL)
										{
											// OK, everything is in place to allow us to replace the graphic document.
											DisplayGraphic( pNewDocument, m_pGraphicDoc );
										}
									}
								}
							}
						}
					}
					else
					{
						RComponentDocument *pNewDocument = pEmbeddedDoc->Clone();
						if (pNewDocument != NULL)
						{
							// Let's add the component we found.
							DisplayGraphic( pNewDocument, m_pGraphicDoc );
						}
					}

					// Store the current document as our previous document for sizing
					// comparisons.  We can't size the local one since the desktop
					// size and the preview sizes aren't in sync.
					pLastDocument = pEmbeddedDoc;
				}
			}
		}
		else
		{
			rSelGraphic = m_cGraphicArray[ m_cGraphicsListBox.GetCurSel() ];
			DisplayGraphic( rSelGraphic, m_pGraphicDoc );
		}
	}
	catch( ... )
	{
		delete pGraphicCollectionSearcher;
		throw;
	}

	//	Clean up the collection searcher.
	delete pGraphicCollectionSearcher;
	
	//	Set the initial button colors
	m_cGraphicColorBtn.SetColor( m_rColorGraphic );
	m_cStyleColorBtn.SetColor( m_rColorEffect );

	//	Set the doc's tint.
	m_cComboTint.SetCurSel( (kMaxTint - m_nTint ) / kTintIncrement );	
	OnCommandTint( );

	EndWaitCursor();
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnOK
//
//  Description:		Called when the user clicks OK
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::OnOK( )
{
	// If there is a timer pending, set it off.
	if ( m_uTimerID != kInvalidTimerID )
		OnTimer( m_uTimerID );

	//	Call the base.
	RGenericGroupedObjectDialog::OnOK( );
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::LookupSquareGraphic
//
//  Description:		Find the square graphic with the given id
//
//  Returns:			the graphic's SearchResult
//
//  Exceptions:		None
//
// ****************************************************************************
SearchResult RCommonGroupedObjectDialog::LookupSquareGraphic( uLONG uGraphicId
			, RCollectionSearcher<RGraphicCategorySearchCollection>*		pGraphicCollectionSearcher )
{
	// Initialize the return value..
	SearchResult rReturnResult;
	rReturnResult.namePtr = NULL;

	// Setup up the collection searcher and the search criteria..
	try
	{
		// Setup the search criteria...
		RGraphicCategorySearchCriteria rSearchCriteria;
		rSearchCriteria.theGraphicType = stSquare;
		pGraphicCollectionSearcher->SetSearchCriteria( &rSearchCriteria );

		// Find all the graphics
		RSearchResultArray* pResultArray = pGraphicCollectionSearcher->SearchCollections();

		// Now iterate through all the graphics until we find the one that
		// matches our id.
		RSearchResultIterator resultIter( pResultArray->Start() );
		RSearchResultIterator resultIterEnd( pResultArray->End() );

		// The index has to start at 1 because the None selection is at zero
		for ( ; resultIter != resultIterEnd; resultIter++ )
		{
			if( (*resultIter).id == uGraphicId )
			{
				rReturnResult = (*resultIter);
				break;
			}
		}
	}
	catch( ... )
	{
		throw;
	}

	return rReturnResult;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::GetDefaultHeadlinePlacement
//
//  Description:		Setup default headline placement info
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RRealRect RCommonGroupedObjectDialog::GetDefaultHeadlineRect()
{
	RRealSize rHeadlineSize( m_pControlView->GetSize());
	rHeadlineSize.Scale( kHeadlineScaleSize );
	RRealPoint rHeadlineTopLeftPoint( m_pControlView->GetSize().m_dx * kHeadlineOffset, ((m_pControlView->GetSize().m_dy / 2.0) - rHeadlineSize.m_dy / 2.0) );
	RRealRect rHeadlineRect( rHeadlineTopLeftPoint, rHeadlineSize );
	return rHeadlineRect;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnButtonGOEffectBold
//
//  Description:		User clicked bold button, update font attributes and preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnButtonGOEffectBold()
{
	m_yFontAttributes = YFontAttributes(0);
	m_fBold = !m_fBold;
	m_yFontAttributes |= (m_fBold) ? RFont::kBold : 0;
	m_yFontAttributes |= (m_fItalic) ? RFont::kItalic : 0;
	UpdatePreview( TRUE, FALSE);
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnButtonGOEffectItalic
//
//  Description:		User clicked italic button, update font attributes and preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnButtonGOEffectItalic()
{
	m_yFontAttributes = YFontAttributes(0);
	m_fItalic = !m_fItalic;
	m_yFontAttributes |= (m_fBold) ? RFont::kBold : 0;
	m_yFontAttributes |= (m_fItalic) ? RFont::kItalic : 0;
	UpdatePreview( TRUE, FALSE );
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnButtonGOMoreGraphics
//
//  Description:		User clicked the more graphics button, display
//							browser dialog and show graphic selection.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnButtonGOMoreGraphics()
{
	int               nDialogRetVal;
	CArtStoreDialog   *pstoreDialog = NULL;

	// Release the collection manager so we can open Print Master based collections!
	RCollectionManager::TheCollectionManager().CloseCollectionsOfType( kCustomCollectionType );
	RCollectionManager::TheCollectionManager().CloseCollectionsOfType( kGraphicsCollectionType );

	// Give ourselves a local document we can give to the art gallery, we'll
	// extract the component created for it when we are done.
	CPmwDoc* pArtDoc = GET_PMWAPP()->NewHiddenDocument();

	if (pArtDoc != NULL)
	{
		// Launch the browser.
		CArtBrowserDialog Dialog (NULL);
		Dialog.m_pDoc = pArtDoc;
		nDialogRetVal = Dialog.DoModal();

		// Did the user select art store from the browser?
		while (nDialogRetVal == ID_ONLINE_ART)
		{
			nDialogRetVal = -1;

			TRY
			{
				pstoreDialog = new CArtStoreDialog;
				// Did the user select a browser from the art store?
				if (pstoreDialog->DoModal() == IDC_ART_STORE_BROWSER_PREVIEW)
				{
					CPMWCollection    *pCollection = pstoreDialog->GetCollectionToPreview();
	 
					if (pCollection != NULL)
					{
						TRACE("Collection %s\n", (LPCSTR)pCollection->GetFriendlyName());

						CString csStartupName;
						csStartupName = pCollection->GetSuperCollection();
						if (!csStartupName.IsEmpty())
						{
							csStartupName += "/";
						}
						csStartupName += pCollection->GetFriendlyName();
						
						if (pCollection->GetType() != CPMWCollection::typeProjects)
						{
							CArtBrowserDialog Dialog;
							Dialog.SetStartupCollection(csStartupName);
							Dialog.m_pDoc = pArtDoc;

							nDialogRetVal = Dialog.DoModal();
						}
						else
						{
							// Keep them looking for art!
							nDialogRetVal = ID_ONLINE_ART;
						}
					}
					else
						ASSERT( FALSE );	// This shouldn't occur.
				}
				
				delete pstoreDialog;
			}
			CATCH_ALL(e)
			{
				ASSERT( FALSE );	// What happened?
				delete pstoreDialog;
			}
			END_CATCH_ALL
		}

		// See about adding the new object to the grouped object.
		if (nDialogRetVal == IDOK)
		{
			RComponentView* pEditedGraphicView = NULL;

			//	Get the current bounding rect.
			//	If we don't have a graphic doc then get the default
			if ( !m_pGraphicDoc )
			{
				SetDefaultGraphicBoundingRect( );
			}
			else
			{
				pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
				m_rGraphicRect = pEditedGraphicView->GetBoundingRect( );
			}

			// Get the document and add it to the dialog preview.
			PMGPageObjectPtr pSelectedObject = pArtDoc->objects();
			ComponentObject *pCompObj = dynamic_cast<ComponentObject *>(pSelectedObject);
			if (pCompObj != NULL)
			{
				RComponentDocument *pSelectedCompDoc = pCompObj->DetachComponentDocument();
				if (pSelectedCompDoc != NULL)
				{
					UpdateGraphicDocument( pSelectedCompDoc, m_pGraphicDoc );
				}
				else
					ASSERT( FALSE );	// This shouldn't occur.

				// Set listbox to have no selection.
				m_cGraphicsListBox.SetCurSel( -1 );
			}
			else
			{
				AfxMessageBox( IDS_MTO_TYPE_NOT_SUPPORTED );
			}

			//	Use the old bounding rect for the new graphic.
//			pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
//			pEditedGraphicView->SetBoundingRect( m_rGraphicRect );

			//	Apply the current tint to the new graphic.
			OnCommandTint();
		}

		pArtDoc->OnCloseDocument();
	}

	// Reopen the collection for the local list.
	m_rCollectionArray = RCollectionManager::TheCollectionManager().OpenCollectionsOfType( kCustomCollectionType );
	RMBCString rGraphicsList = "";
	m_fRebuiltGraphicArrays = TRUE;
	CreateGraphicsList( rGraphicsList );	// Recreate the graphics list, otherwise the list pointers are invalid.
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnSelchangeComboFont
//
//  Description:		User picked new font from font combo.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnSelchangeComboFont()
{
	CString cHeadlineFont;
	m_cFontCombo.GetLBText( m_cFontCombo.GetCurSel(), cHeadlineFont );
	m_rFontName = cHeadlineFont;
	UpdatePreview( TRUE, FALSE );
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::DisplayGraphic
//
//  Description:		Display selected graphic in preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc ) 
{
	DisplayGraphic( rSelGraphic, pGraphicDoc, m_uSelectedGraphicId );
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::DisplayGraphic
//
//  Description:		Display selected graphic in preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc, uLONG& uSelectedGraphicId ) 
{
	// Check to see if the "None" option is selected.  If it is, remove the component
	if( rSelGraphic.namePtr == NULL )
	{
		if( pGraphicDoc )
		{
			RemoveComponent( pGraphicDoc );
			pGraphicDoc = NULL;
		}
		//	Disable the effects we can apply to it.
		m_cGraphicColorBtn.EnableWindow( FALSE );
		m_cComboTint.EnableWindow( FALSE );
		uSelectedGraphicId = kInvalidGraphicId;
		return;
	}
	//	If we have a graphic make sure we can tint it.
	m_cComboTint.EnableWindow( TRUE );

	//
	// Get the selected component from the SearchResult
	RComponentDocument* pTempGraphicDoc = NULL;
	pTempGraphicDoc = rSelGraphic.GetComponent( m_pCompInfo, FALSE );
	//
	// Get graphic id from SearchResult
	uSelectedGraphicId = rSelGraphic.id;
	UpdateGraphicDocument( pTempGraphicDoc, pGraphicDoc );
}


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::DisplayGraphic
//
//  Description:		Display selected graphic in the preview window having
//							been given the new and old graphic documents to work with.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::DisplayGraphic( RComponentDocument *pNewGraphicDoc, RComponentDocument*& pGraphicDoc )
{
	//	If we have a graphic make sure we can tint it.
	m_cComboTint.EnableWindow( TRUE );

	UpdateGraphicDocument( pNewGraphicDoc, pGraphicDoc );
}


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::UpdateGraphicDocument
//
//  Description:		Display selected graphic in preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::UpdateGraphicDocument( RComponentDocument*& pTempGraphicDoc , RComponentDocument*& pGraphicDoc ) 
{
	TpsAssert( pTempGraphicDoc, "Null Graphic Doc" );
	//
	// Set graphic as non selectable..
	RComponentAttributes rGraphicAttrib;
	rGraphicAttrib.SetSelectable(FALSE);
	pTempGraphicDoc->SetComponentAttributes( rGraphicAttrib );
	//
	// If a graphic component already exists, replace it.  Otherwise create a new one
	if( pGraphicDoc )
		ReplaceComponent( pGraphicDoc, pTempGraphicDoc );
	else
		NewComponent( pTempGraphicDoc );
	//
	// Set member graphic document pointer and fit graphic to preview control...
	pGraphicDoc = pTempGraphicDoc;

	CheckForMonochromeGraphic( pGraphicDoc );

	RComponentView* pGraphicView = pGraphicDoc->GetView( m_pControlView );
	pGraphicView->FitInsideParent();

	UpdatePreview( FALSE, TRUE );
}
// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::CheckForMonochromeGraphic
//
//  Description:		Enables the color button if the graphic is monochrome.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::CheckForMonochromeGraphic( RComponentDocument*& pGraphicDoc )
{
	TpsAssert( pGraphicDoc, "Null graphic doc pointer" );
	RComponentView* pGraphicView = pGraphicDoc->GetView( m_pControlView );

	// Request an color interface from the view.
	RInterface* pInterface = pGraphicView->GetInterface(kColorSettingsInterfaceId);

	// Check to see if graphic is monochrome..
	if( dynamic_cast<RColorSettingsInterface*>(pInterface) )
		m_cGraphicColorBtn.EnableWindow( TRUE );
	else
		m_cGraphicColorBtn.EnableWindow( FALSE );

	// Kill the interface we created.
	delete pInterface;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnButtonGOGraphicColor
//
//  Description:		Displays color dialog for graphic color.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::OnButtonGOGraphicColor()
{
	//
	// Bring up the color dialog, and don't allow gradients
	RWinColorDlg	rColorDlg( NULL, 0 );
	//
	// Set the current graphic color into the dialog
	rColorDlg.SetColor( m_rColorGraphic );
	//
	// Pop up the color dialog
	if( rColorDlg.DoModal() == IDOK )
	{
		m_rColorGraphic = rColorDlg.SelectedSolidColor();
		m_cGraphicColorBtn.SetColor( m_rColorGraphic );
		UpdatePreview( FALSE, TRUE );
	}
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnButtonGOEffectColor
//
//  Description:		Displays color dialog for effect color.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::OnButtonGOEffectColor()
{
	//
	// Bring up the color dialog, and don't allow gradients
	RWinColorDlg	rColorDlg( NULL, 0 );
	//
	// Set the current effect color into the dialog
	rColorDlg.SetColor( m_rColorEffect );
	//
	// Pop up the color dialog
	if( rColorDlg.DoModal() == IDOK )
	{
		m_rColorEffect = rColorDlg.SelectedSolidColor();
		m_cStyleColorBtn.SetColor( m_rColorEffect );
		UpdatePreview( TRUE, FALSE );
	}
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::LoadGraphicsListBox
//
//  Description:		Load the graphics list box with the available options.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::LoadGraphicsListBox( RMBCString& rGraphics ) 
{
	RMBCString rCompleteList( kNoneSelection );
	rCompleteList += kListFieldSeperator;
	rCompleteList += rGraphics;
	ParseAndPutInListBox( rCompleteList, m_cGraphicsListBox );
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::LoadEffectsListBox
//
//  Description:		Load the graphics list box with the available options.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::LoadEffectsListBox( RMBCString& rEffects ) 
{
	ParseAndPutInListBox( rEffects, m_cEffectsListBox );
}


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::SetDefaultGraphicBoundingRect
//
//  Description:		Position the graphic in the edit area.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::SetDefaultGraphicBoundingRect( ) 
{
	//	Define the handle size.
	RRealSize rHandleSize( kRotationResizeHandleHitSize, kRotationResizeHandleHitSize );
	ScreenUnitsToLogicalUnits( rHandleSize );

	//	Use the entire control area.
	CRect tempControlRect;
	m_pControlView->GetCWnd( ).GetClientRect( &tempControlRect );
	RRealRect controlRect( tempControlRect );
	ScreenUnitsToLogicalUnits( controlRect );
	m_rGraphicRect = controlRect;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnSelchangeListboxGraphic
//
//  Description:		Background graphic selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnSelchangeListboxGraphics() 
{
	//	Save the current graphic rect.
	RComponentView* pEditedGraphicView = NULL;
	if ( m_pGraphicDoc )
	{
		pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
		m_rGraphicRect = pEditedGraphicView->GetBoundingRect( );
	}

	//	Get the current selection.
	int nCurrentSelection = m_cGraphicsListBox.GetCurSel();
	if ( kNoGraphicSelected == nCurrentSelection )
	{
		//	Remove the current selection if the user selected the "None" option.
		m_uSelectedGraphicId = kInvalidGraphicId;
		RCompositeSelection* pSelection = m_pControlView->GetSelection( );
		if ( pSelection )
			pSelection->UnselectAll( TRUE );
		//
		// Get the SearchResult object for selected graphic
		SearchResult rSelGraphic;
		rSelGraphic = m_cGraphicArray[ nCurrentSelection ];
		DisplayGraphic( rSelGraphic, m_pGraphicDoc );
	}
	else
	{
		// Get the SearchResult object for newly selected graphic
		SearchResult rSelGraphic;
		rSelGraphic = m_cGraphicArray[ nCurrentSelection ];
		DisplayGraphic( rSelGraphic, m_pGraphicDoc );

		//	Use the old bounding rect for the new graphic.
//		pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
//		pEditedGraphicView->SetBoundingRect( m_rGraphicRect );

		//	Apply the current tint to the new graphic.
		OnCommandTint( );
	}
}


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnSelchangeListboxEffects
//
//  Description:		Headline/Cap Effect selection changes.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnSelchangeListboxEffects() 
{
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnEditChangeHeadlineText
//
//  Description:		Headline text is changing, reset timer.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnEditChangeHeadlineText()
{
	// Change the text settings
	CString cHeadlineText;
	m_cHeadlineEditCtrl.GetWindowText(cHeadlineText);
	m_rHeadlineText = RMBCString(cHeadlineText);

	// Restart the timer
	BOOL fResult = KillTimer(m_uTimerID);
	m_uTimerID = SetTimer(kTimerID, kRenderTimeout, NULL);
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnTimer
//
//  Description:		Headline is no longer changing, update preview
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RCommonGroupedObjectDialog::OnTimer(UINT)
{
	// Stop the timer and update the headline attributes
	BOOL fResult	= KillTimer(m_uTimerID);
	m_uTimerID		= kInvalidTimerID;
	UpdatePreview( TRUE, FALSE );
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
void RCommonGroupedObjectDialog::UpdatePreview( BOOLEAN fUpdateHeadline, BOOLEAN fUpdateGraphic )
{
	if( m_fInitializing ) return;
	if( fUpdateHeadline )
	{
		// If the headline is nothing, set it to a space because the headline
		// engine rejects empty headlines.
		if( m_rHeadlineText == RMBCString("") )
		{
			m_rHeadlineData.m_rHeadlineText		= " ";
		}
		else
		{
			m_rHeadlineData.m_rHeadlineText		= m_rHeadlineText;
		}
		m_rHeadlineData.m_rFontName				= m_rFontName;
		m_rHeadlineData.m_rOutlineFillColor		= m_rColorEffect;
		m_rHeadlineData.m_yFontAttributes		= m_yFontAttributes;
		//
		// Invalidate the render cache to be sure headline gets updated..
		m_pHeadlineView->InvalidateRenderCache();
		m_rHeadlineData.UpdateHeadline( m_pHeadlineInterface );
	}

	if( fUpdateGraphic )
	{
		if( m_pGraphicDoc )
		{
			RComponentView*	pGraphicView	= m_pGraphicDoc->GetView( m_pControlView );
			RInterface*			pInterface		= pGraphicView->GetInterface(kColorSettingsInterfaceId);
			RColorSettingsInterface* pGraphicInterface = dynamic_cast<RColorSettingsInterface*>( pInterface );
			if( pGraphicInterface )
			{
				pGraphicInterface->SetColor( m_rColorGraphic );
				pGraphicView->InvalidateRenderCache( );
			}
			delete pInterface;
		}
	}
	TpsAssert( m_pControlDocument, "Null control document pointer" );
	m_pControlDocument->InvalidateAllViews();
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::WriteUIContextData
//
//  Description:		Writes dialogs common UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::WriteUIContextData( RUIContextData& contextData ) const
{
	// Write the graphic name
	int graphicIndex = m_cGraphicsListBox.GetCurSel( );
	CString graphicString;
	if( graphicIndex == LB_ERR )
	{
		graphicString = (LPCSZ)kInvalidGraphicString;
	}
	else
	{
		m_cGraphicsListBox.GetText( graphicIndex, graphicString );
	}
	contextData << graphicString;

	// Write the headline effect
	int effectIndex = m_cEffectsListBox.GetCurSel( );
	CString effectString;
	m_cEffectsListBox.GetText( effectIndex, effectString );
	contextData << effectString;

	// Read in the headline text and set it into the edit control
	contextData << m_rHeadlineText;

	// Write out the font
	contextData << m_rFontName;

	// Write out the color effects
	contextData << m_rColorEffect;

	// Write out the graphic color
	contextData << m_rColorGraphic;

	// Write out the font attributes
	contextData << m_yFontAttributes;

	// Write out the headline bounding rect
	contextData << m_pHeadlineView->GetBoundingRect( );

}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RCommonGroupedObjectDialog::ReadUIContextData() 
{
	// Default the the first element in each list box
	m_cGraphicsListBox.SetCurSel( 0 );
	m_cEffectsListBox.SetCurSel( 0 );

	// If we are editting a component, try to read its context data
	if( m_pEditedComponent )
	{
		try
		{
			// Get the UI context data
			RUIContextData& contextData = m_pEditedComponent->GetUIContextData( );

			// Read the graphic name and select it in the graphic listbox
			RMBCString string;
			contextData >> string;
			int nGraphicIndex = m_cGraphicsListBox.FindStringExact( -1, (LPCSZ)string );
#if 0 // GCB 12/8/97 fix bug that crashed OnInitDialog() if graphic not found
			if( nGraphicIndex == LB_ERR ) nGraphicIndex = -1;
#else
			if( nGraphicIndex == LB_ERR ) nGraphicIndex = 0;
#endif
			m_cGraphicsListBox.SetCurSel( nGraphicIndex );

			// Read the effect name and select it in the effect listbox
			contextData >> string;
#if 0 // GCB 12/8/97 fix bug that crashed OnInitDialog() if effect not found
			m_cEffectsListBox.SetCurSel(m_cEffectsListBox.FindStringExact( -1, (LPCSZ)string ) );
#else
			int nEffectIndex = m_cEffectsListBox.FindStringExact( -1, (LPCSZ)string );
			if( nEffectIndex == LB_ERR ) nEffectIndex = 0;
			m_cEffectsListBox.SetCurSel( nEffectIndex );
#endif

			// Read in the headline text
			contextData >> m_rHeadlineText;
			m_cHeadlineEditCtrl.SetWindowText( m_rHeadlineText );

			// Read in the font and set it into the combo box
			contextData >> m_rFontName;
			m_cFontCombo.SelectString( -1, m_rFontName );

			// Read in the color effects
			contextData >> m_rColorEffect;

			// Read in the graphic color 
			contextData >> m_rColorGraphic;

			// Read in the font attributes
			contextData >> m_yFontAttributes;

			m_fBold		= m_yFontAttributes & RFont::kBold;
			m_fItalic	= m_yFontAttributes & RFont::kItalic;
			UpdateData( FALSE );

			// Read in and set the headline bounding rect
			YComponentBoundingRect boundingRect;
			contextData >> boundingRect;
			VerifyBoundingRect( boundingRect );
			TpsAssert( m_pHeadlineView, "Null HeadlineView pointer" );
			m_pHeadlineView->SetBoundingRect( boundingRect );

			return &contextData;
		}
		catch( ... )
		{
			return NULL;
		}
	}
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::WriteVersion2UIContextData
//
//  Description:		Writes dialogs common UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::WriteVersion2UIContextData( RUIContextData& contextData ) const
{
	//	Prep our versioning system.
	StartContextDataVersion( contextData, kContextDataVersion2 );

	// REVIEW MWH -this would go in WriteUIContextData, but it can't
	// because then we wouldn't be backwards compatiable
	// Write out the selected graphics id
	contextData << m_uSelectedGraphicId;

	if ( m_pGraphicDoc )
	{
		//	Write a flag indicating that the bounding rect is valid.
		contextData << static_cast<uBYTE>( TRUE );

		//	Get the bounding rect of the edited component.
		RComponentView* pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
		contextData << pEditedGraphicView->GetBoundingRect(  );
	}
	else
	{
		//	Write a flag indicating that the bounding rect is not valid.
		contextData << static_cast<uBYTE>( FALSE );

		//	Write a placeholder graphic component rect.
		RRealVectorRect rTemp;
		contextData << rTemp;
	}

	//	Remember whether this is the first edit
	contextData << static_cast<uBYTE>( m_fFirstEdit );

	//	Write the tint.
	contextData << m_nTint;
}


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::ReadVersion2UIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RCommonGroupedObjectDialog::ReadVersion2UIContextData( RUIContextData& contextData ) 
{
	// If we are editting a component, try to read its context data
	if( m_pEditedComponent )
	{
		try
		{
			//	Can't proceed if our context data is miss-versioned.
			if ( kContextDataVersion2 != GetContextDataVersion( contextData ) )
				return NULL;

			// REVIEW MWH -this would go in ReadUIContextData, but it can't
			// because then we wouldn't be backwards compatiable.
			// Read in the selected graphics id
			contextData >> m_uSelectedGraphicId;

			//	Write a flag indicating that the bounding rect is present.
			contextData >> static_cast<uBYTE>( m_fBoundingRectPresent );

			//	Read it in if its there.
			if ( m_fBoundingRectPresent )
			{
				//	Read the saved graphic component rect.
				contextData >> m_rGraphicRect;
			}
			else
			{
				//	Discard the placeholder graphic component rect.
				RRealVectorRect rTemp;
				contextData >> rTemp;
			}

			//	Remember whether this is the first edit
			contextData >> static_cast<uBYTE>( m_fFirstEdit );

			//	Read the tint.
			contextData >> m_nTint;

			return &contextData;
		}
		catch( ... )
		{
			return NULL;
		}
	}
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::StartContextDataVersion
//
//  Description:		Write the context data version
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::StartContextDataVersion( RUIContextData& contextData, const YContextDataVersion& dataVersion ) const
{
	//	Write the version.
	contextData << dataVersion;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::GetContextDataVersion
//
//  Description:		Return the stream position to be used as a data version
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YContextDataVersion RCommonGroupedObjectDialog::GetContextDataVersion( RUIContextData& contextData ) const
{
	//	Get and return the data version.
	YContextDataVersion yVersion;
	contextData >> yVersion;

	return yVersion;
}

// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::VerifyBoundingRect
//
//  Description:		Make sure that the bounding rect
//							we're reading in is not somehow corrupted.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::VerifyBoundingRect( YComponentBoundingRect& rBoundingRect )
{
	//
	// REVIEW -MWH -there really shouldn't be any need for this function,
	// but somehow some of the older files have completely bogus bounding
	// rects when we read them in.  In order to remain backwards compatable,
	// we need to detect invalid bounding rects and default them.  We should
	// really get rid of this in Rev 2 if we can figure out what's going wrong.
	RRealRect rIntersectRect;
	rIntersectRect.Intersect( rBoundingRect.m_TransformedBoundingRect, RRealRect( m_pControlView->GetSize() ) );
	if( rIntersectRect.IsEmpty( ) )
	{
		TpsAssertAlways( "Corrupt bounding rect read.  Converting to default." );
		rBoundingRect = (YComponentBoundingRect)GetDefaultHeadlineRect();
	}
}


// ****************************************************************************
//
//  Function Name:	RCommonGroupedObjectDialog::OnCommandTint( )
//
//  Description:		Brings up the tint popup menu.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCommonGroupedObjectDialog::OnCommandTint()
{
	//	Set the tint.
	//	Find the selected item.
	RComponentView* pEditedGraphicView = NULL;
	if ( m_pGraphicDoc )
		pEditedGraphicView = static_cast<RComponentView*>( m_pGraphicDoc->GetActiveView( ) );
	
	if ( pEditedGraphicView )
	{
		// Get the current tint value.
		//	If it hasn't been set then use the current value.
		int nCurSel = m_cComboTint.GetCurSel( );
		if ( -1 != nCurSel )
			m_nTint = kMaxTint - (nCurSel * kTintIncrement);

		m_pGraphicDoc->SetTint(m_nTint);
		
		// Tell the view to redraw itself
		pEditedGraphicView->InvalidateRenderCache();
		pEditedGraphicView->Invalidate();	
	}
}


// ****************************************************************************
// ****************************************************************************
// ****************************************************************************
// *********************                             **************************
// *********************  RGOHeadlineData			     **************************
// *********************                             **************************
// ****************************************************************************
// ****************************************************************************
// ****************************************************************************


// ****************************************************************************
//
//  Function Name:	RGOHeadlineData::RGOHeadlineData
//
//  Description:		Ctor - default
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RGOHeadlineData::RGOHeadlineData( ) :
	m_rFontName( kDefaultGroupedObjectDialog ),
	m_eDistortEffect( kNonDistort ),     
	m_fDistort( FALSE ),
	m_eTextJustification( kCenterJust ),
	m_yFontAttributes( 0 ),
	m_uwScale1(1),
	m_uwScale2(1),
	m_uwScale3(1),
	m_nShapeIndex( 1 ),
	m_rOutlineFillColor( RColor(kBlack) ),
	m_eOutlineEffect( kSimpleFill ),
	m_eLineWeight( kMediumLine ),
	m_rStrokeColor( kBlack ),
	m_eShadowEffect( kNoShadow ),
	m_rShadowColor( kBlack ),
	m_rShadowVanishPt( kDefaultShadowVanishPt ),
	m_yShadowDepth( kDefaultShadowDepth ),
	m_eProjectionEffect( kNoProjection ),
	m_rProjectionVanishPt( kDefaultProjectionVanishPt ),
	m_yProjectionDepth( kDefaultProjectionDepth ),
	m_nProjectionStages( kDefaultProjectionStages )
{
	m_rHeadlineText = ::GetResourceManager().GetResourceString( IDS_GO_DEFAULT_HEADLINE );

	//	Check if preferences font is on machine.  If it is, use it instead.
	RMBCString	preferenceFont( GetDefaultFont() );
	if ( RFont::IsFontOnSystem( preferenceFont ) )
		m_rFontName = preferenceFont;
}

// ****************************************************************************
//
//  Function Name:	RGOHeadlineData::RGOHeadlineData
//
//  Description:		Ctor -logo effects
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
RGOHeadlineData::RGOHeadlineData(	EDistortEffects		eDistortEffect,     
												int						nShapeIndex,
												BOOLEAN					fDistort
												) :
	m_rFontName( "NewZurica" ),	//	This can't be the const since we don't know the order of initialization
	m_eDistortEffect( eDistortEffect ), 
	m_fDistort( fDistort ),
	m_eTextJustification( kCenterJust ),
	m_yFontAttributes( 0 ),
	m_uwScale1(1),
	m_uwScale2(1),
	m_uwScale3(1),
	m_nShapeIndex( nShapeIndex ),
	m_rOutlineFillColor( RColor(kBlack) ),
	m_eOutlineEffect( kSimpleFill ),
	m_eLineWeight( kMediumLine ),
	m_rStrokeColor( kBlack ),
	m_eShadowEffect( kNoShadow ),
	m_rShadowColor( kBlack ),
	m_rShadowVanishPt( kDefaultShadowVanishPt ),
	m_yShadowDepth( kDefaultShadowDepth ),
	m_eProjectionEffect( kNoProjection ),
	m_rProjectionVanishPt( kDefaultProjectionVanishPt ),
	m_yProjectionDepth( kDefaultProjectionDepth ),
	m_nProjectionStages( kDefaultProjectionStages )
{
	// Can't load resource manager here because of static table (logo)
	m_rHeadlineText = "";//::GetResourceManager().GetResourceString( STRING_GO_DEFAULT_HEADLINE );
	//	Can't load resources, or preferences so just keep the default (NewZurica)
}
// ****************************************************************************
//
//  Function Name:	RGOHeadlineData::UpdateHeadline
//
//  Description:		Update headline through given interface with current data
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RGOHeadlineData::UpdateHeadline( RHeadlineInterface* m_pInterface ) const
{
	TpsAssert( m_pInterface, "Null headline interface pointer." );

	m_pInterface->SetHeadlineText( m_rHeadlineText );
	m_pInterface->SetFontName( m_rFontName );
	m_pInterface->SetDistortEffect( m_eDistortEffect );
	m_pInterface->SetDistort( m_fDistort );
	m_pInterface->SetJustification( m_eTextJustification );
	m_pInterface->SetFontAttributes( m_yFontAttributes );
	m_pInterface->SetScale1( m_uwScale1 );
	m_pInterface->SetScale2( m_uwScale2 );
	m_pInterface->SetScale3( m_uwScale3 );
	m_pInterface->SetShapeIndex( m_nShapeIndex );
	m_pInterface->SetOutlineFillColor( m_rOutlineFillColor );
	m_pInterface->SetOutlineEffect( m_eOutlineEffect );

	m_pInterface->SetLineWeight( m_eLineWeight );
	m_pInterface->SetStrokeColor( m_rStrokeColor );
	m_pInterface->SetShadowEffect( m_eShadowEffect );
	m_pInterface->SetShadowFillColor( m_rShadowColor );
	m_pInterface->SetShadowVanishPoint( m_rShadowVanishPt );
	m_pInterface->SetShadowDepth( m_yShadowDepth );
	m_pInterface->SetProjectionEffect( m_eProjectionEffect );
	m_pInterface->SetProjectionVanishPoint( m_rProjectionVanishPt );
	m_pInterface->SetProjectionDepth( m_yProjectionDepth );
	// REVIEW MWH -temporary kludge until Houle fixes SetNumStages to take int not int&
	int nTemp = m_nProjectionStages;
	m_pInterface->SetNumStages( nTemp );

	//	No GoDialogs should ever display the Envelope.
	m_pInterface->SetNoDisplayEnvelop( TRUE );

	m_pInterface->Commit();

}
