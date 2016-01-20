// ****************************************************************************
//
//  File Name:		TabbedEditImage.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Definition of the RTabbedEditImage class
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					500 Redwood Blvd
//					Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "ComponentView.h"
#include "ComponentDocument.h"
#include "ComponentManager.h"
#include "ComponentTypes.h"
#include "TabbedEditImage.h"
#include "BitmapImage.h"
#include "Path.h"
#include "OffscreenDrawingSurface.h"
#include "FrameworkResourceIDs.h"
#include "UndoManager.h"
#include "ExportGraphicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// a color for the background of clipped images created via the Save As command
static const RSolidColor kDefaultClippedBkgColor( kWhite );

BEGIN_MESSAGE_MAP(RTabbedEditImage, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_EDIT_TAB, OnSelChangeTabs)
	ON_BN_CLICKED(IDC_EDIT_RESET, OnReset)
	ON_BN_CLICKED(IDC_EDIT_SAVEAS, OnSaveAs)
	ON_WM_PARENTNOTIFY()
END_MESSAGE_MAP()

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::RTabbedEditImage()
//
//  Description:	Constructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RTabbedEditImage::RTabbedEditImage(RComponentDocument* pDesignDeskComponent, int selTab, RMBCString *pSaveAsDir /*=NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(RTabbedEditImage::IDD, pParent),
	m_pDesignDeskComponent(pDesignDeskComponent),
	m_nCurSelPage( selTab ),
	m_pSaveAsDir( pSaveAsDir ),
	m_pLocalComponent(NULL),
	m_pImage( NULL ),
	m_bImageHasFramePath( FALSE ),
	m_bButtonsEnabledAndLocked( FALSE )
{
	m_pCurrentPage = NULL ;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::~RTabbedEditImage()
//
//  Description:	Destructor
//
//  Returns:		nothing
//
// ****************************************************************************
//
RTabbedEditImage::~RTabbedEditImage()
{	
	delete m_pImage;
	delete m_pLocalComponent;
}


// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::DoDataExchange(CDataExchange* pDX)
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control (pDX, IDC_EDIT_TAB, m_ctlTabCtrl) ;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::PreTranslateMessage()
//
//  Description:		Handles various keystrokes for this dialog
//
//  Returns:			TRUE if the message is processed here
//
// ****************************************************************************
//
BOOL RTabbedEditImage::PreTranslateMessage( MSG* pMsg )
{
	BOOL bRet = FALSE;

	// handle character messages
	if ( pMsg->message == WM_CHAR )
	{
		// Ctrl-Z = undo
		if (( ::GetAsyncKeyState( VK_CONTROL ) < 0 ) && pMsg->wParam == 26 )
		{
			if ( m_pCurrentPage )
			{
				m_pCurrentPage->SendMessage( WM_USER_UNDO_FROM_IED, WPARAM(0), LPARAM(0) );
			}
			bRet = TRUE;
		}
		// zoom in
		else if ( pMsg->wParam == '+' )
		{
			if ( m_pCurrentPage )
			{
				m_pCurrentPage->SendMessage( WM_USER_ZOOMIN_FROM_IED, WPARAM(0), LPARAM(0) );
			}
			bRet = TRUE;
		}
		// zoom out
		else if ( pMsg->wParam == '-' )
		{
			m_pCurrentPage->SendMessage( WM_USER_ZOOMOUT_FROM_IED, WPARAM(0), LPARAM(0) );
		}
		bRet = TRUE;
	}

	if ( ! bRet )
	{
		bRet = CDialog::PreTranslateMessage( pMsg );
	}

	return bRet;
}
	
// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::OnInitDialog() 
//
//  Description:	Sets up the tab control
//
//  Returns:		BOOL
//
// ****************************************************************************
//
BOOL RTabbedEditImage::OnInitDialog() 
{
	RWaitCursor waitCursor;
	CDialog::OnInitDialog();

	// set up our local component copy
	if ( ! InitializeLocalComponent( TRUE ) )
	{
		EndDialog( IDCANCEL );
		return TRUE;
	}

	// OK and Revert should be disabled to start with
	EnableRevertAndOkButtons( FALSE );

	// Add in the tabs
	TC_ITEM tabItem;
	tabItem.iImage  = -1;
	tabItem.mask    = TCIF_TEXT;

	CString strTabText ;
	
	try
	{
		strTabText.LoadString( STRING_IED_CROP ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (0, &tabItem) ;

		strTabText.LoadString( STRING_IED_ADJUSTCOLOR ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (1, &tabItem) ;

		strTabText.LoadString( STRING_IED_BRIGHTFOCUS ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (2, &tabItem) ;

		strTabText.LoadString( STRING_IED_FIXFLAW ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (3, &tabItem) ;

		strTabText.LoadString( STRING_IED_ARTEFFECTS ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem (4, &tabItem) ;

	}

	catch (CMemoryException& e)
	{
		e.ReportError() ;
		EndDialog( IDCANCEL ) ;

		return TRUE ;
	}

	try
	{
		CRect rect ;
		m_ctlTabCtrl.GetWindowRect( rect ) ;
		m_ctlTabCtrl.SetItemSize( CSize( (rect.Width() - 5) / 5, 0 ) ) ;
	}
	catch (...)
	{
		// Let the user know there is a reason 
		// the dialog didn't come it.
		RAlert rAlert ;
		rAlert.AlertUser( STRING_ERROR_ACC_OUT_OF_MEMORY ) ;
		EndDialog( IDCANCEL ) ;
	}

	ShowPage( m_nCurSelPage );
	m_ctlTabCtrl.SetCurSel( m_nCurSelPage ) ;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::InitializeLocalComponent()
//
//  Description:	creates a fresh local copy of the design desk component
//
//  Returns:		BOOLEAN
//
// ****************************************************************************
//
BOOLEAN RTabbedEditImage::InitializeLocalComponent( BOOLEAN bFirstTime )
{
	try
	{
		RComponentView* pComponentView = (RComponentView *)
			m_pDesignDeskComponent->GetActiveView();

		// Get an image interface to the component to be edited
		RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(
			pComponentView->GetInterface( kImageInterfaceId ));
		TpsAssert(pImageInterface, "Component does not support an image interface.");

		// If the image is not editable, we must convert it to editable form BEFORE copying
		// it into the preview component.  Otherwise, when we go to apply the effects to the
		// full-size component later, we will not be able to.  This may be time consuming, but
		// it is probably the best way to do it.
		if ( ! pImageInterface->QueryEditImage() )
		{
			// The image is not editable
			if ( bFirstTime )
			{
				// Ask the user if they would like to make it editable
				RAlert rAlert;		
				{
					if ( rAlert.QueryUser(STRING_PROMPT_CONVERT_METAFILE_TO_BITMAP) == kAlertNo )
					{
						// The image was not or could not be made editable
						delete pImageInterface;
						return FALSE;
					}
				}					
				if ( ! pImageInterface->MakeImageEditable() )
				{
					// The image was not or could not be made editable
					delete pImageInterface;
					return FALSE;
				}
			}
		}

		// note if the image has a frame path; we will use this info to
		//	disable some tools (Rotate 90 in the Crop tab, for now)
		RBitmapImage* pImage = (RBitmapImage*) pImageInterface->GetImage();
		if ( pImage->GetFrameRPath() )
		{
			m_bImageHasFramePath = TRUE;
		}

		// Create a local copy of the component
		YComponentBoundingRect rBoundingRect = RRealRect( pImage->GetSizeInLogicalUnits() );
		RComponentAttributes rAttr = m_pDesignDeskComponent->GetComponentAttributes();
		rAttr.SetSelectable( FALSE );

		delete m_pLocalComponent;
		m_pLocalComponent = ::GetComponentManager().CreateNewComponent(
			kImageComponent, NULL, NULL, rAttr,	rBoundingRect,	FALSE );

		// Get a copy of the source image from the component
		delete m_pImage;
		m_pImage = dynamic_cast<RBitmapImage *>(
			pImageInterface->CopySourceImage() );

		// make sure image is 24 bits, or many of the effects won't work
		if ( m_pImage->GetBitDepth() != 24 )
		{
			RImageLibrary().ChangeBitDepth( *m_pImage, 24 );
		}

		// Clean up the interfaces
		delete pImageInterface;

		// Set the image into the local component
		ApplyImageEffects( pComponentView );
	}

	catch( YException exception )
	{
		::ReportException( exception );
		TpsAssertAlways( "Hit exception handling in REditImageDialog::OnInitDialog" );
		return FALSE;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::CreateComponentWithEffects()
//
//  Description:	   Returns the local copy of the component that 
//                   has all the effects applied to it.  By asserting
//                   that the dialog has been destroyed, we can
//                   safely assume we don't need it anymore.
//
//  Returns:		   
//
// ****************************************************************************
//
RComponentDocument* RTabbedEditImage::CreateComponentWithEffects()
{
	TpsAssert( !m_hWnd, "Function only available after window is destroyed" );
	RComponentDocument* pComponent = m_pLocalComponent;

	// So it doesn't get deleted
	m_pLocalComponent = NULL; 

	return pComponent;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::GetFullComponentCopy()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
RComponentDocument* RTabbedEditImage::GetFullComponentCopy()
{
	RComponentDocument *pNewComponent = NULL;
	
	// if we don't have a local document, we need go no further
	if ( m_pLocalComponent )
	{
		// get a view and an image interface to our document
		RComponentView *pView = (RComponentView *)m_pLocalComponent->GetActiveView();
		if ( pView )
		{
			RImageInterface* pImageInterface;
			pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));
			RImage *pImage = pImageInterface->GetImage();
			if ( pImage )
			{
				RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(pImage);				
				if ( pBitmapImage )
				{
					// get the preview image dimensions and get a full size copy
					CSize cSize( pBitmapImage->GetWidthInPixels(), pBitmapImage->GetHeightInPixels() );
					pNewComponent = GetComponentCopy( m_pLocalComponent, cSize );
				}
			}

			delete pImageInterface;
		}
	}
	
	return pNewComponent;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::GetPreviewCopyFromIEDComponent()
//
//  Description:	Creates a copy of the local RImageDocument with an image
//					scaled for a preview control.
//
//  Returns:		pointer to the new RImageDocument
//
// ****************************************************************************
//
RComponentDocument* RTabbedEditImage::GetPreviewCopyFromIEDComponent(RImagePreviewCtrl *pCtrl)
{
	RComponentDocument *pNewComponent = NULL;
	
	// if we don't have a local document, we need go no further
	if ( m_pLocalComponent )
	{
		pNewComponent = GetPreviewComponentCopy( m_pLocalComponent, pCtrl );
	}
	
	return pNewComponent;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::GetPreviewComponentCopy()
//
//  Description:	Creates a copy of the passed image component with an image
//					scaled for a preview control.
//
//  Returns:		pointer to the new RImageDocument
//
// ****************************************************************************
//
RComponentDocument* RTabbedEditImage::GetPreviewComponentCopy(RComponentDocument *pComponent, RImagePreviewCtrl *pCtrl)
{
	// if we don't have a document, we need go no further
	if ( ! pComponent || ! pCtrl )
	{
		return NULL;
	}

	// get a view and an image interface to our document
	RComponentView *pView = (RComponentView *)pComponent->GetActiveView();
	if (! pView)
	{
		return NULL;
	}

	RImageInterface* pImageInterface;
	pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));
	RImage *pImage = pImageInterface->GetImage();
	if (! pImage)
	{
		delete pImageInterface;
		return NULL;
	}

	// get the preview control's vital statistics
	CRect cRect;
	pCtrl->GetClientRect( cRect );
	
	// get the preview image dimensions
	CSize cDisplaySize = GetPreviewImageSize( pImage, cRect );

	// clean up the interface
	delete pImageInterface;

	// get the preview-sized copy
	return GetComponentCopy( pComponent, cDisplaySize );
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::GetComponentCopy()
//
//  Description:	Creates a copy of the passed image component
//
//  Returns:		pointer to the new RImageDocument
//
// ****************************************************************************
//
RComponentDocument* RTabbedEditImage::GetComponentCopy( RComponentDocument *pComponent, CSize& cNewSize )
{
	// if we don't have a document, we need go no further
	if (! pComponent)
	{
		return NULL;
	}

	// get a view and an image interface to our document
	RComponentView *pView = (RComponentView *)pComponent->GetActiveView();
	if (! pView)
	{
		return NULL;
	}

	RImageInterface* pImageInterface;
	pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));
	RImage *pImage = pImageInterface->GetImage();
	if (! pImage)
	{
		delete pImageInterface;
		return NULL;
	}

	// create an RBitmapImage compatible with the drawing surface
	RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
	RBitmapImage rTempBitmap;
	rTempBitmap.Initialize( cNewSize.cx, cNewSize.cy, pBitmap->GetBitDepth() );		

	// set the bitmap into an offscreen DS		
	ROffscreenDrawingSurface rODS;
	rODS.SetImage(&rTempBitmap);		
				
	// create a transform to scale image to the preview dimensions
	RRealSize rImageSize( pBitmap->GetWidthInPixels(), pBitmap->GetHeightInPixels() );
	YScaleFactor sX = (YFloatType)cNewSize.cx / (YFloatType)rImageSize.m_dx;		
	YScaleFactor sY = (YFloatType)cNewSize.cy / (YFloatType)rImageSize.m_dy;		

	R2dTransform transform;
	transform.PreScale( sX, sY );

	// copy over the clipping path
	RClippingPath *pPath = pBitmap->GetClippingRPath();

	if( pPath && pPath->IsDefined() )
	{
		RClippingPath rNewPath( *pPath, transform );
		rTempBitmap.SetRPath( &rNewPath );
	}

	// render into our offscreen
	RColor crOldColor  = rODS.GetFillColor();
	RColor crFillColor = RSolidColor( GetSysColor( COLOR_BTNFACE ) ); 
	RIntRect rImageRect(0, 0, cNewSize.cx, cNewSize.cy);

	rODS.SetFillColor( crFillColor );
	rODS.FillRectangle( rImageRect );
	rODS.SetFillColor( crOldColor );

	pView->Render(rODS, rImageRect );

	// create a new component document
	RComponentAttributes rAttr = pComponent->GetComponentAttributes();
	YComponentBoundingRect rBoundingRect(0, 0, 1, 1);

	RComponentDocument *pNewComponent = ::GetComponentManager().CreateNewComponent(kImageComponent,
											NULL,
											NULL,
											rAttr,
											rBoundingRect,
											FALSE);
	if (pNewComponent)
	{
		// get the new document's view and a pointer to the Image interface
		RComponentView *pNewView = static_cast<RComponentView*>(pNewComponent->GetActiveView());
		RImageInterface *pNewInterface = dynamic_cast<RImageInterface*>(pNewView->GetInterface(kImageInterfaceId));
		TpsAssert(pNewInterface, "Image component does not support image interface.");

		// and set its image to be our scaled preview bitmap
		pNewInterface->SetImage( rTempBitmap );
 		delete pNewInterface;
	}

	// release the offscreen bitmap and clean up
	rODS.ReleaseImage();

	// clean up the interface
	delete pImageInterface;

	return pNewComponent;
}	

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::GetPreviewImageSize()
//
//  Description:	Calculates the dimensions for an image scaled to fit within
//					the passed rectangle.
//
//  Returns:		the dimensions for the scaled image
//
// ****************************************************************************
//
CSize RTabbedEditImage::GetPreviewImageSize( RImage *pImage, CRect cRect )
{		
	CSize cDisplaySize;

	if (pImage)
	{
		// calculate the size to fit the image within the rectangle
		RRealSize rImageSize = pImage->GetSizeInLogicalUnits();
		YFloatType nARi = rImageSize.m_dx / rImageSize.m_dy;		
		YFloatType nARp = (YFloatType)cRect.Width() / (YFloatType)cRect.Height();
		if (nARi > nARp)
		{
			cDisplaySize.cx = cRect.Width();
			cDisplaySize.cy = (YIntDimension)((YFloatType)cRect.Width() / nARi);
		}
		else
		{
			cDisplaySize.cx = (YIntDimension)((YFloatType)cRect.Height() * nARi);
			cDisplaySize.cy = cRect.Height();
		}

		// if it is a bitmap, we don't want to stretch it
		RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(pImage);				
		if (pBitmapImage)
		{
			int sWidth = (int)pBitmapImage->GetWidthInPixels();
			int sHeight = (int)pBitmapImage->GetHeightInPixels();
			if (cDisplaySize.cx > sWidth || cDisplaySize.cy > sHeight)
			{
				cDisplaySize.cx = sWidth;
				cDisplaySize.cy = sHeight;
			}
		}
	}

	// to avoid zero sized objects
	cDisplaySize.cx = max( cDisplaySize.cx, 1 );
	cDisplaySize.cy = max( cDisplaySize.cy, 1 );

	return cDisplaySize;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::ShowPage(int nPage)
//
//  Description:	Shows the tab page that's passed in
//
//  Returns:		Nothing
//
// ****************************************************************************
//
void RTabbedEditImage::ShowPage(int nPage)
{
	CDialog* pageArray[] = 
	{ 
		&m_pageCrop,
		&m_pageAdjustColor,
		&m_pageBrightFocus,
		&m_pageFixFlaw,
		&m_pageArtEffect
	} ;

	static int idArray[] =
	{
		m_pageCrop.IDD,
		m_pageAdjustColor.IDD,
		m_pageBrightFocus.IDD,
		m_pageFixFlaw.IDD,
		m_pageArtEffect.IDD
	} ;

	// Save a pointer to the current page,
	// so if the creation process fails,
	// we can reset to the previous page.
	CDialog* pPrevPage = m_pCurrentPage ;

	if (m_pCurrentPage != pageArray[nPage] /*&& m_ctlTabCtrl.IsWindowVisible()*/)
	{

		if (m_pCurrentPage && IsWindow (m_pCurrentPage->m_hWnd))
			m_pCurrentPage->ShowWindow(SW_HIDE) ;

		CRect   rectAdjust(0,0,0,0);
		CRect   rectWindow(0,0,0,0);

		m_ctlTabCtrl.AdjustRect( TRUE, &rectAdjust );
		m_ctlTabCtrl.GetWindowRect( &rectWindow );

		rectWindow.left -= rectAdjust.left;
		rectWindow.top  -= rectAdjust.top;
		ScreenToClient( &rectWindow );

		m_pCurrentPage = pageArray[nPage] ;
		m_nCurSelPage = nPage;

		if (!IsWindow (m_pCurrentPage->m_hWnd))
		{
			if (!m_pCurrentPage->Create(idArray[nPage], this))
			{
				// Let the user know there is a reason 
				// they're staring at a blank page.
				RAlert rAlert ;
				rAlert.AlertUser( STRING_ERROR_ACC_OUT_OF_MEMORY ) ;
				if (!pPrevPage) return ;

				// Restore the current page pointer to the previous one.
				m_pCurrentPage = pPrevPage ;

				// Find the index of the previous page. (For setting the tab)
				for (int i = 0; i < m_ctlTabCtrl.GetItemCount(); i++)
				{
					if (m_pCurrentPage == pageArray[nPage])
					{
						m_ctlTabCtrl.SetCurSel( i ) ;
						break ;
					}
				}
			}
			
			m_pCurrentPage->SetWindowPos (&wndTop, rectWindow.left,
				rectWindow.top, 0, 0, SWP_NOSIZE) ;
		}
		
		m_pCurrentPage->ShowWindow (SW_SHOW) ;
	}

}

// ****************************************************************************
//
//  Function Name:	OnSelChangeTabs(NMHDR*, LRESULT* pResult) 
//
//  Description:	Sets the selected page to the top
//
//  Returns:		Nothing
//
// ****************************************************************************
//
void RTabbedEditImage::OnSelChangeTabs(NMHDR*, LRESULT* pResult) 
{
	ShowPage (m_ctlTabCtrl.GetCurSel()) ;
	*pResult = 0;

	if (GetFocus() != (CWnd *) &m_ctlTabCtrl)
	{
		m_pCurrentPage->GetTopWindow()->SetFocus() ;
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::OnOK()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
void RTabbedEditImage::OnOK()
{
	// call this to force tab to update m_pLocalComponent with latest edits
//	m_pCurrentPage->SendMessage( WM_COMMAND, IDOK );

	if ( m_pCurrentPage->SendMessage( WM_USER_ON_OK_FROM_IED, WPARAM(0), LPARAM(0) ) )
	{
		// Call the base member
		CDialog::OnOK();
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::SetImage()
//
//  Description:	Sets the local copy of the image.
//
//					NOTE: passed image MUST have been allocated on the heap, and
//						  should not be deleted, as we simply stash the pointer here
//						  to avoid an extra copy.
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::SetImage( RBitmapImage* pImage )
{
	//	caller may or may not have done an in place edit
	//	if not, replace the old image with the new one
	if ( m_pImage && m_pImage != pImage )
	{
		delete m_pImage;
		m_pImage = pImage;

		if (m_pCurrentPage)
			m_pCurrentPage->SendMessage( WM_USER_SCRATCH_IMAGE_CHANGED );
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::ApplyImageEffects()
//
//  Description:	Updates the local component with all the 
//                changes made to the local image.
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::ApplyImageEffects( RComponentView* pSourceView, BOOLEAN fResize )
{
	RComponentView* pLocalComponentView = (RComponentView *)
		m_pLocalComponent->GetActiveView();

	if (!pSourceView)
		pSourceView = pLocalComponentView;

	try
	{
		// Set the new image
		RImageInterface* pLocalImageInterface = static_cast<RImageInterface*>(
			pLocalComponentView->GetInterface( kImageInterfaceId ));

		pLocalImageInterface->SetImage( *m_pImage, fResize );

		delete pLocalImageInterface;
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::ResetImageToLastSavedState()
//
//  Description:	Resets the working image to the last saved state
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::ResetImageToLastSavedState()
{
	try
	{
		RComponentView* pLocalComponentView = (RComponentView *)
			m_pLocalComponent->GetActiveView();

		RImageInterface* pImageInterface = static_cast<RImageInterface*>(
			pLocalComponentView->GetInterface( kImageInterfaceId ));

		delete m_pImage;

		m_pImage = static_cast<RBitmapImage *>( pImageInterface->CopySourceImage() );

		delete pImageInterface;
	}
	catch (...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::GetCropRect()
//
//  Description:	calculates the rectangle that represents the cropped area of the image
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::GetCropRect( RIntRect *pRect )
{
	//	get the RImageEffects member of the  local component, so we can access the crop rect
	//	the crop rect has been set to fit the the bounds of the clipping path
	RComponentView *pView = (RComponentView *)m_pLocalComponent->GetActiveView();
	REditImageInterface* pEditInterface = static_cast<REditImageInterface*>(pView->GetInterface( kEditImageInterfaceId ) );
	RImageEffects rEffects = pEditInterface->GetImageEffects();
	delete pEditInterface;

	// determine the area of our intermediate bitmap that corresponds to the cropped area
	RIntRect rImageRect( RIntSize( m_pImage->GetWidthInPixels(), m_pImage->GetHeightInPixels() ) );
 	RIntRect rRect( 0, 0, rImageRect.Width() * rEffects.m_rCropArea.m_Right, rImageRect.Height() * rEffects.m_rCropArea.m_Bottom );

	rRect.Offset( RIntSize( rImageRect.m_Left + rImageRect.Width()  * rEffects.m_rCropArea.m_Left,
	 						rImageRect.m_Top  + rImageRect.Height() * rEffects.m_rCropArea.m_Top ) );
	*pRect = rRect;
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::OnParentNotify()
//
//  Description:	handles WM_PARENTNOTIFY messages sent from controls in child tabs
//
//					Since some controls (e.g., CStatic) in child tab dialogs
//					send their WM_PARENTNOTIFY messages here, we must forward them to the
//					current child tab dialog
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::OnParentNotify( UINT message, LPARAM lParam )
{
	//	if this is a mouse message, we need to offset the point to the
	//	tab's coordinate space and pass it on to the tab child dialog
	if ( LOWORD(message) != WM_CREATE && LOWORD(message) != WM_DESTROY )
	{
		// forward the message to the current tab
		if ( m_pCurrentPage )
		{
			// get tab's position in our client area
			CRect crTabRect;
			m_pCurrentPage->GetWindowRect( &crTabRect );
			ScreenToClient( &crTabRect );

			// get our client rect
			CRect crClientRect;
			GetClientRect( &crClientRect );

			// adjust incoming mouse point to tab's coordinate space
			POINT point;
			point.x = LOWORD(lParam) - (crTabRect.left - crClientRect.left);
			point.y = HIWORD(lParam) - (crTabRect.top  - crClientRect.top);

			m_pCurrentPage->SendMessage( WM_PARENTNOTIFY, (WPARAM)message, MAKELPARAM( point.x, point.y ) );
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::OnReset()
//
//  Description:	resets the IED document to its original incoming state
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::OnReset()
{
	if ( m_pCurrentPage )
	{
		RWaitCursor rWaitCursor;

		// refresh the current tab
		m_pCurrentPage->SendMessage( WM_USER_CLEANUP_IED_TAB_FOR_RESET );

		// recreate our local component copy
		InitializeLocalComponent();

		// refresh the current tab
		m_pCurrentPage->SendMessage( WM_USER_RESET_IED_TAB );

		// reset the Revert and OK buttons to disabled
		m_bButtonsEnabledAndLocked = FALSE;
		EnableRevertAndOkButtons( FALSE );
	}
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::OnSaveAs()
//
//  Description:	invokes the Save As dialog to save the current image to file
//
//  Returns:		nothing
//
// ****************************************************************************
//
void RTabbedEditImage::OnSaveAs()
{
	// this is necessary if image was tinted for crop display
	ResetImageToLastSavedState();
	
	if ( m_pCurrentPage )
	{
		m_pCurrentPage->SendMessage( WM_USER_APPLY_EDITS_TO_IED_BITMAP ) ;
	}
	
	// get a view and an image interface to our document
	RComponentView *pView = (RComponentView *)m_pLocalComponent->GetActiveView();
	RImageInterface* pImageInterface = dynamic_cast<RImageInterface*>(pView->GetInterface(kImageInterfaceId));
	RImage* pImage = pImageInterface->GetImage();
	RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>( pImage );

	// if this is a clipped / cropped photo, then we want to save the clipped area only
	RPath *pPath = pBitmap->GetClippingRPath();
	if ( pPath && pPath->IsDefined() )
	{
		// get the clipped image size
		RIntRect rDestRect( RIntSize( pBitmap->GetWidthInPixels(), pBitmap->GetHeightInPixels() ) );

		// create a bitmap the size of the clipping path's bounding rect and set it in an offscreen DS
		RBitmapImage rTempBitmap;
		rTempBitmap.Initialize( rDestRect.Width(), rDestRect.Height(), pBitmap->GetBitDepth() );		

		ROffscreenDrawingSurface rODS;
		rODS.SetImage( &rTempBitmap );		
				
		// get the cropping rectangle
	 	RIntRect rSrcRect;
		GetCropRect( &rSrcRect );

		// render the image section into the offscreen
		m_pImage->Render( rODS, rSrcRect, rDestRect );

		// release the offscreen bitmap
		rODS.ReleaseImage();

		// create a second bitmap for the final clipped output and set it into the DS
		RBitmapImage rTempBitmap2;
		rTempBitmap2.Initialize( rDestRect.Width(), rDestRect.Height(), pBitmap->GetBitDepth() );		
		rODS.SetImage( &rTempBitmap2 );		
				
		// fill with default background color
		rODS.SetFillColor( RSolidColor( kDefaultClippedBkgColor ) );
		rODS.FillRectangle( RIntRect( rDestRect ) );
		
		// render as object requires a pre-allocated mask buffer
		RBitmapImage rMaskBitmap;
		rMaskBitmap.Initialize( rDestRect.Width(), rDestRect.Height(), 1 );

		// create a transform to scale image the path from the original image to the image section 
		RRealSize rImageSize = pView->GetBoundingRect().WidthHeight();
		YScaleFactor sX = (YFloatType)rDestRect.Width() / (YFloatType)rImageSize.m_dx;		
		YScaleFactor sY = (YFloatType)rDestRect.Height() / (YFloatType)rImageSize.m_dy;		

		R2dTransform rTransform;
		rTransform.PreScale( sX, sY );

		// now render our image section with the clipping rect
		rTempBitmap.RenderAsObject( rODS, rMaskBitmap, rDestRect, rImageSize, rTransform, pPath );

		// invoke the Export Image dialog to do the save
		ExportGraphicImage( &rTempBitmap2, m_pSaveAsDir );
		// release the offscreen bitmap
		rODS.ReleaseImage();
	}
	else						// otherwise, no clipping path, just export the local bitmap
	{
		// bring up the export image dialog
		ExportGraphicImage( m_pImage, m_pSaveAsDir );
	}

	// after the first time into the ExportGraphicDialog,
	//	we will default to the last directory the user chose 
	m_pSaveAsDir = NULL;

	// clean up the interface
	delete pImageInterface;

	//	reset our bitmap, since the user has not implied a "burn-in" by saving
	//	the image
	ResetImageToLastSavedState();
}

// ****************************************************************************
//
//  Function Name:	RTabbedEditImage::EnableRevertAndOkButtons()
//
//  Description:		handles enabling / disabling of the Revert and OK buttons
//
//  Returns:			nothing
//
// ****************************************************************************
//
void RTabbedEditImage::EnableRevertAndOkButtons( BOOLEAN bEnable, BOOLEAN bLock )
{
	// if an edit has taken place, the buttons stay enabled
	if ( ! m_bButtonsEnabledAndLocked )
	{
		CWnd* pWnd = GetDlgItem( IDOK );
		pWnd->EnableWindow( bEnable );
		
		pWnd = GetDlgItem( IDC_EDIT_RESET );
		pWnd->EnableWindow( bEnable );

		if ( bEnable && bLock )
		{
			m_bButtonsEnabledAndLocked = TRUE;
		}
	}
}


////////////////////////////////////////////////////////////////////////

YActionId RModifyImageAction::m_ActionId( "RModifyImageAction" );

// ****************************************************************************
//
//  Function Name:	RModifyImageAction::RModifyImageAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		kUnknownError
//
// ****************************************************************************
//
RModifyImageAction::RModifyImageAction( RComponentDocument* pComponent, RIntRect* pRect, RBitmapImage* pImage ) :
	RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_SPECIAL_EFFECT, STRING_UNDO_CHANGE_SPECIAL_EFFECT ),
	m_pComponent( pComponent ),
	m_ptImageOffset( 0, 0 ),
	m_fReplaceWhole( TRUE )
{

	try
	{
		m_pFullImage = pImage;
		if ( ! m_pFullImage )
		{
			RComponentView* pComponentView = static_cast<RComponentView *>(
				pComponent->GetActiveView() );

			RImageInterface* pInterface = static_cast<RImageInterface *>(
				pComponentView->GetInterface( kImageInterfaceId ) );
			TpsAssert( pInterface, "Invalid component type!" );

			m_pFullImage = static_cast<RBitmapImage *>(
				pInterface->GetImage() );

			delete pInterface;
		}

		if (pRect)
		{
			m_fReplaceWhole = FALSE;
			m_ptImageOffset = pRect->m_TopLeft;

			m_biImage.Initialize( pRect->Width(), pRect->Height(), 
				m_pFullImage->GetBitDepth(), m_pFullImage->GetXDpi(), m_pFullImage->GetYDpi() );

			RIntRect rDestRect( *pRect );
			rDestRect.Offset( RIntSize( -pRect->m_Left, -pRect->m_Top ) );

			ROffscreenDrawingSurface dsMem1, dsMem2;
			dsMem1.SetImage( m_pFullImage );
			dsMem2.SetImage( &m_biImage );

			dsMem2.BlitDrawingSurface( dsMem1, *pRect, rDestRect, kBlitSourceCopy );
			
			dsMem1.ReleaseImage();
			dsMem2.ReleaseImage();

			return;
		}

		m_biImage.Initialize( m_pFullImage->GetRawData() );

	}
	catch(...)
	{
		ThrowException( kUnknownError );
	}
}


// ****************************************************************************
//
//  Function Name:	RModifyImageAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ******************************************************************************
//
void RModifyImageAction::Undo( )
{
	RUndoableAction::Undo( );

	RImageInterface* pInterface = NULL;

	try
	{
		RBitmapImage* pNewImage = &m_biImage;

		if (!m_fReplaceWhole)
		{
			pNewImage = m_pFullImage;

			RIntRect rDestRect( 
				m_ptImageOffset.m_x,
				m_ptImageOffset.m_y,
				m_ptImageOffset.m_x + m_biImage.GetWidthInPixels(),
				m_ptImageOffset.m_y + m_biImage.GetHeightInPixels() );

			ROffscreenDrawingSurface dsMem;
			dsMem.SetImage( pNewImage );
			m_biImage.Render( dsMem, rDestRect );
			dsMem.ReleaseImage();
		}

		RComponentView* pComponentView = static_cast<RComponentView *>(
			m_pComponent->GetActiveView() );

		pInterface = static_cast<RImageInterface *>(
			pComponentView->GetInterface( kImageInterfaceId ) );
		TpsAssert( pInterface, "Invalid component type!" );

		pInterface->SetImage( *pNewImage, FALSE );
		m_pComponent->InvalidateAllViews();
	}
	catch(...)
	{
	}

	if (pInterface)
		delete pInterface;
}

// ****************************************************************************
//
//  Function Name:	RModifyImageAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RModifyImageAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RModifyImageAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RModifyImageAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RModifyImageAction, this );
}

#endif	//	TPSDEBUG

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//  Class:	REditUndoManager
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// ****************************************************************************
//
//  Function Name:	REditUndoManager::SendAction( )
//
//  Description:	Component API function to send an action to a Component
//
//  Returns:		TODO: What does this return?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditUndoManager::SendAction( RAction* pAction )
{
	BOOLEAN	fReturnValue	= FALSE;

	if( pAction->Do( ) )
	{
		fReturnValue = TRUE;
			//	Action was Done and the script was written, we succeded.
			//	If action is undoable, give it to the undo mananger...
			//	If it is not undoable, we are done with it so delete it...
		if( pAction->IsUndoable( ) )
		{
			RUndoableAction*	pUndoAction = dynamic_cast<RUndoableAction*>( pAction );
			TpsAssert( pUndoAction, "The given undoable action is not derived from RUndoableAction");
			AddAction( pUndoAction );

			return TRUE;
		}
	}
	else
	{
		;///xxx	TODO: Do processing Failed... Alert user and throw an error...
	}

	delete pAction;

	return fReturnValue;
}

