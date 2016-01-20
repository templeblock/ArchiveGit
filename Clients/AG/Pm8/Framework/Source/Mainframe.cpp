// ****************************************************************************
//
//  File Name:			Mainframe.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RMainframe class
//
//  Portability:		Windows, MFC specific
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
//  $Logfile:: /PM8/Framework/Source/Mainframe.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_WINDOWS
	#error "This file should only be compiled under Windows"
#endif

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "Mainframe.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "AfxPriv.h"
#include	"FrameworkResourceIDs.h"
#include "StandaloneApplication.h"
#include	"ApplicationGlobals.h"
#include	"View.h"
#include "Menu.h"
#include "StandaloneView.h"
#include "StandaloneDocument.h"
#include "BitmapImage.h"
#include "ComponentTypes.h"
#include "ComponentAttributes.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ImageInterface.h"
#include "NewComponentAction.h"
#include "ComponentManager.h"
#include "SplashScreen.h"
#include "ResourceManager.h"
#include "Configuration.h"
#include "VersionInfo.h"
#include "Alert.h"
#include <direct.h>

#ifdef RENAISSANCE_TWAIN
#include "twainWrapper.h"
#endif

IMPLEMENT_DYNAMIC( RMainframe, CMDIFrameWnd )

BEGIN_MESSAGE_MAP( RMainframe, CMDIFrameWnd )
	//{{AFX_MSG_MAP(RMainframe)
		ON_WM_CREATE( )
		ON_WM_WININICHANGE()
		ON_WM_DEVMODECHANGE()
		ON_WM_ACTIVATEAPP( )
		ON_WM_INITMENUPOPUP( )
		ON_COMMAND_RANGE( COMMAND_MENU_FILE_NEW, COMMAND_MENU_FILE_QUIT, OnMfcCommand )
		ON_MESSAGE( WM_SETTEXT, OnSetText )
		ON_UPDATE_COMMAND_UI(COMMAND_MENU_INSERT_ACQUIRE, OnUpdateAcquire)
		ON_COMMAND(COMMAND_MENU_INSERT_ACQUIRE, OnAcquire)
		ON_UPDATE_COMMAND_UI(COMMAND_MENU_INSERT_SELECT_SOURCE, OnUpdateSelectSource)
		ON_COMMAND(COMMAND_MENU_INSERT_SELECT_SOURCE, OnSelectSource)
		ON_WM_DESTROY()
#ifdef RENAISSANCE_TWAIN
		ON_REGISTERED_MESSAGE( gWM_BTWAIN_IMAGEXFER_ID, OnBTwainImageXfer )
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

// ****************************************************************************
//
//  Function Name:	RMainframe::RMainframe( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMainframe::RMainframe( )
	{
#ifdef RENAISSANCE_TWAIN
	m_pTwainHandler = NULL;
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::~RMainframe( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMainframe::~RMainframe()
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnActivateApp
//
//  Description:		Handles the WM_ACTIVATEAPP message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
afx_msg void RMainframe::OnActivateApp( BOOL bActive, HTASK hTask )
	{
	CMDIFrameWnd::OnActivateApp( bActive, hTask );

	if( ::GetActiveView( ) )
		::GetActiveView( )->OnXActivateApp( static_cast<BOOLEAN>( bActive ) );
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnCreate( )
//
//  Description:		Called during window creation
//
//  Returns:			0:		Continue window creation
//							-1:	Terminate the window
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RMainframe::OnCreate( LPCREATESTRUCT lpCreateStruct )
	{
	// Call the base class
	if( CMDIFrameWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	//show the splashscreen
	CSplashWnd::EnableSplashScreen(TRUE);
	CSplashWnd::ShowSplashScreen(this);

#ifdef RENAISSANCE_TWAIN
	// Create a TWAIN handler for image acquisition
	m_pTwainHandler = new BTwainWrapper;		
	if ( m_pTwainHandler )						
		{
		// fill the TWAIN info struct
		BTwainAppInfo appID;

		// Get version info
		RVersionInfo version;
		version.GetVersionInfo( );

		// Fill in the version fields in the TWAIN info struct
		appID.m_nVersionMajor  = version.m_MajorVersion;
		appID.m_nVersionMinor  = version.m_MinorVersion;

		// Fill in the version as string
		char temp[ 40 ];
	   strcpy( temp, "Ver. " );
		wsprintf( temp, "Ver. %d.%d.%d.%d", version.m_MajorVersion, version.m_MinorVersion, version.m_RevisionVersion, version.m_InternalVersion );
      appID.m_sVersionInfo = temp;

		// Fill in the company and application name
		CString s1, s2;
		appID.m_sManufacturer   = "";
		if ( s1.LoadString( STRING_COMPANY_NAME ) )
			appID.m_sManufacturer = s1.GetBuffer(32);

		appID.m_sProductFamily = "";
		appID.m_sProductName   = "";
		if ( s2.LoadString( STRING_APPLICATION_NAME ) )
			appID.m_sProductName   = s2.GetBuffer(32);

		// ok, now initialize TWAIN
		BTwainResult result = m_pTwainHandler->Initialize( m_hWnd, &appID );
		if ( result != BTR_OK )
			{
			// there are legitimate reasons that the initialization might fail,
			// like the user never installed the dirvers and has no scanner;
			// therefore, don't put up unnecessary and alarming messages
			if ( result == BTR_NO_WINDOWS_MESSAGE )
				{
				RAlert().AlertUser( m_pTwainHandler->ErrorMessage( result ) );
//				AfxMessageBox( );
				}
			delete m_pTwainHandler;
			m_pTwainHandler = NULL;
			}
		}
#endif // RENAISSANCE_TWAIN

	return 0;
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnMfcCommand( )
//
//  Description:		Called when commands are recieved from MFC command routing
//							architecture.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
afx_msg void RMainframe::OnMfcCommand( UINT nID )
	{
	// Dispatch the command
	GetApplication( ).GetRCommandMap( )->OnCommand( static_cast<YCommandID>( nID ), &GetApplication( ), TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnCommand( WPARAM wParam, LPARAM lParam )
//
//  Description:		Called when commands are recieved from window command routing
//							architecture.
//
//  Returns:			TRUE: we handled the command
//
//  Exceptions:		None
//
// ****************************************************************************
//
afx_msg BOOL RMainframe::OnCommand( WPARAM wParam, LPARAM lParam )
	{
	BOOL fHandled = CMDIFrameWnd::OnCommand( wParam,  lParam );
	
	//	if the greater mfc framework does not handle this command
	//		then we need a chance at it
	if ( !fHandled )
		{
		UINT nID = LOWORD(wParam);
		OnMfcCommand( nID );
		fHandled = TRUE;
		}
	
	return fHandled;
	}
	
// ****************************************************************************
//
//  Function Name:	RMainframe::OnSetText( WPARAM wParam, LPARAM lParam )
//
//  Description:		Called when the name of the title bar is needed.
//
//  Returns:			Result of DefWndProc
//
//  Exceptions:		kMemory (CMemoryException*)
//
// ****************************************************************************
//
LRESULT RMainframe::OnSetText(WPARAM wParam, LPARAM lParam)
	{
#ifdef _DEBUG
	static char szNewTitle[ 256 ];

	// Get the version info
	RVersionInfo version;
	if( version.GetVersionInfo( ) )
		{
		// Format the title string
		wsprintf( szNewTitle, "%s - Build %d", (char*)lParam, version.m_InternalVersion );
		lParam = (LPARAM)szNewTitle;
		}
#endif //_DEBUG

	return DefWindowProc( WM_SETTEXT, wParam, lParam );
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnWinIniChange( LPSCTR lpszSection )
//
//  Description:		Called on WM_INICHANGE; update print manager's list of printers
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnWinIniChange( LPCTSTR lpszSection ) 
	{
	CMDIFrameWnd::OnWinIniChange(lpszSection);
	HNATIVE	hnColors = NULL;

	try
		{
		YResourceType		trt;

		//	reset color palette in TPS lib in case user changed color scheme
		tbitShutdown( );							
		SetResourceType(trt, 'P','L','T','T');
		if ( NULL == (hnColors = GetResourceManager().GetResource(trt, PALETTE_APPLICATION_256)) )
			throw kResource;

		if (!tbitStartup (3, 0, hnColors))		//	initialize with 3 ports for any depth
			throw kMemory;

		GetResourceManager().FreeResource(hnColors);
		hnColors = NULL;								//	in case of exception		 
		::GetApplication( ).OnXPreferencesChanged( RMBCString( lpszSection ) );

		//	Tell the application globals to delete the current default printer dc
		::DeleteDefaultPrinterDC( );
		}
	catch( ... )
		{
		::ReportException( kUnknownError );
		if(hnColors != NULL)
			GetResourceManager().FreeResource(hnColors);
		}
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnDevModeChange( LPSCTR lpszSection )
//
//  Description:		Called on WM_DEVMODECHANGE; update projects using the changed printer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnDevModeChange( LPTSTR lpDeviceName ) 
	{
	CMDIFrameWnd::OnDevModeChange(lpDeviceName);
	
	try
		{
		::GetApplication( ).OnXPrinterChanged( RMBCString( lpDeviceName ) );
		}
	catch( ... )
		{
		::ReportException( kUnknownError );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::ForceIdle( )
//
//  Description:		Posts a WM_KICKIDLE message to force idle processing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMainframe::ForceIdle( )
	{
	PostMessage( WM_KICKIDLE );
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnInitMenuPopup(  )
//
//  Description:		Called when a popup menu is about to be displayed
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnInitMenuPopup( CMenu* pMenu, UINT, BOOL bSysMenu )
	{
	// We do not handle the system menu
	if( !bSysMenu )
		{
		// Create an RMenu from the CMenu
		RMenu menu( pMenu->m_hMenu );

		// Update the menu
		menu.Update( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::PreTranslateMessage(  )
//
//  Description:		Intercepts messages before they are dispatched; used here
//							to send messages to TWAIN Source Manager for analysis and
//							possible consumption.
//
//  Returns:			TRUE if message is processed here
//							FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RMainframe::PreTranslateMessage( MSG* pMsg )
	{
	BOOL retcode = FALSE;

#ifdef RENAISSANCE_TWAIN
	// TWAIN message?
	if( m_pTwainHandler )
		retcode = m_pTwainHandler->ProcessMessage( pMsg );
#endif

	// if not, send it on
	if( !retcode )
		retcode = CMDIFrameWnd::PreTranslateMessage( pMsg );

	return retcode;
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::Destroy(  )
//
//  Description:		Called when the frame is about to be destroyed
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnDestroy() 
	{
	if( !::GetApplication( ).WasLaunchedForPrinting( ) )
		{
		// Get the Window placement information 
		WINDOWPLACEMENT windowPlacement;
		windowPlacement.length = sizeof( windowPlacement );
		GetWindowPlacement( &windowPlacement );

		// Write the window placement structure to the registry
		RMainframeConfiguration rConfig;
		rConfig.SetBinaryValue(RMainframeConfiguration::kPosition, &windowPlacement, sizeof(windowPlacement));
		}

#ifdef RENAISSANCE_TWAIN
	if( m_pTwainHandler )
		{
		// Clean up twain handler
		m_pTwainHandler->Close( );
		delete m_pTwainHandler;
		m_pTwainHandler = NULL;
		}
#endif

	CMDIFrameWnd::OnDestroy();
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnUpdateSelectSource(  )
//
//  Description:		Update the Select Image Source (TWAIN) menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnUpdateSelectSource(CCmdUI* pCmdUI) 
	{
#ifdef RENAISSANCE_TWAIN
	// For some reason this function resets the working directory. Save it here and
	// restore it before we leave.
   char buffer[_MAX_PATH + 1];
	_getcwd( buffer, _MAX_PATH );

	if ( m_pTwainHandler )
		{
		//  enable menu item only if a source is detectable, and there is
		//  no acquisition already in progress
		pCmdUI->Enable( m_pTwainHandler->ReadyToAcquire() );
		}
	else
		{
		pCmdUI->Enable( FALSE );
		}

	_chdir( buffer );
#else
	pCmdUI->Enable( FALSE );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnSelectSource(  )
//
//  Description:		Process the Select Image Source (TWAIN) menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnSelectSource() 
	{
#ifdef RENAISSANCE_TWAIN
	if( m_pTwainHandler )
		{
		m_pTwainHandler->SelectSource();
		}
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnUpdateAcquire(  )
//
//  Description:		Update the Acquire Image (TWAIN) menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnUpdateAcquire(CCmdUI* pCmdUI) 
	{
	// Get the active document and view
	RStandaloneDocument* pDocument = GetActiveStandaloneDocument( );
	RView* pActiveView = pDocument->GetActiveView( );

	// Enable the menu item if we have an active view, and there are no components active in it
	if( pActiveView && pActiveView->GetActiveComponent( ) == NULL )
		{
		// uses same criteria as the Select Source command
		OnUpdateSelectSource( pCmdUI );
		}

	else
		{
		pCmdUI->Enable( FALSE );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnAcquire(  )
//
//  Description:		Process the Acquire Image (TWAIN) menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnAcquire() 
	{
#ifdef RENAISSANCE_TWAIN
	// Acquire the image
	if( m_pTwainHandler )
		BTwainResult err = m_pTwainHandler->Acquire();
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnBTwainImageXfer(  )
//
//  Description:		Handle the special message from TWAIN that an image has
//							been acquired
//
//  Returns:			0 to indicate completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
LONG RMainframe::OnBTwainImageXfer( WPARAM wParam, LPARAM )
	{
#ifdef RENAISSANCE_TWAIN
	// Get the global memory block that contains the DIB
	HGLOBAL hDib = reinterpret_cast<HGLOBAL>( wParam );

 	// Get the active document
 	RStandaloneDocument* pTwainDocument = GetActiveStandaloneDocument( );

	if( hDib && pTwainDocument )
		{
		// Get the active view
 		RView* pTwainView = pTwainDocument->GetActiveView( );

 		// Save the current insertion point
 		RRealPoint twainInsertionPoint = pTwainView->GetInsertionPoint( RRealSize( 0, 0 ) );

		// Lock down the DIB
		BITMAPINFOHEADER* pBitmapInfoHeader = reinterpret_cast<BITMAPINFOHEADER*>( ::GlobalLock( hDib ) );

		//  Accusoft bug - 24-bit DIBs that also have lookup table entries are not handled properly
		BYTE *pNewBmpData = NULL;
		if ( pBitmapInfoHeader )
			{
			if ( pBitmapInfoHeader->biBitCount >= 16 && pBitmapInfoHeader->biClrUsed > 0 )
				{
				int size = pBitmapInfoHeader->biSize + pBitmapInfoHeader->biSizeImage;

				pNewBmpData = new BYTE[size];		// so create a new DIB
				if ( pNewBmpData )
					{								// without the table entries
					memcpy( pNewBmpData, pBitmapInfoHeader, pBitmapInfoHeader->biSize );
					memcpy( pNewBmpData + pBitmapInfoHeader->biSize,
							(BYTE *)pBitmapInfoHeader + pBitmapInfoHeader->biSize + (pBitmapInfoHeader->biClrUsed * sizeof(RGBQUAD)),
							pBitmapInfoHeader->biSizeImage );

					pBitmapInfoHeader = (BITMAPINFOHEADER *)pNewBmpData;
					pBitmapInfoHeader->biClrUsed = 0;
					}
				}
			}

		// Create and initialize a new bitmap
		RBitmapImage bitmapImage;
		bitmapImage.Initialize( pBitmapInfoHeader );

		// Get the image size, in logical units
		RRealSize imageSize( pBitmapInfoHeader->biWidth, pBitmapInfoHeader->biHeight );
		::ScreenUnitsToLogicalUnits( imageSize );

		// Calculate the bounding rect of the new component
		RRealRect boundingRect( twainInsertionPoint, imageSize );
		boundingRect.Offset( RRealSize( -imageSize.m_dx / 2, -imageSize.m_dy / 2 ) );

		// Create a new image component
		RComponentDocument* pImageComponent = ::GetComponentManager( ).CreateNewComponent( kImageComponent,
																													  pTwainDocument,
																													  pTwainView,
																													  RComponentAttributes( ),
																													  boundingRect,
																													  FALSE );

		// Get the image component view
		RComponentView* pImageView = static_cast<RComponentView*>( pImageComponent->GetActiveView( ) );

		// Get the image interface
		RImageInterface* pImageInterface = static_cast<RImageInterface*>( pImageView->GetInterface( kImageInterfaceId ) );
		TpsAssert( pImageInterface, "No image interface." );

		// Set the image of the image component through the interface
		pImageInterface->SetImage( bitmapImage );

		// Delete the interface once we are done with it.
		delete pImageInterface;

		// Activate the view we are inserting into
		pTwainView->ActivateView( );

		// Cancel tracking in the view
		pTwainView->CancelTracking( );

		// Create and send a new component action
		RNewComponentAction* pAction = new RNewComponentAction( pImageComponent,
																				  pTwainDocument,
																				  pTwainView->GetSelection( ),
																				  STRING_UNDO_AQUIRE_IMAGE );
		pTwainDocument->SendAction( pAction );

		// Clean up
		::GlobalUnlock( hDib );
		delete [] pNewBmpData;
		}

	// TWAIN allocated the DIB, so we need to free it
	if( hDib )
		::GlobalFree( hDib );
#endif
	return 0;
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnIdle( )
//
//  Description:		Idle handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMainframe::OnIdle( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMainframe::OnIdle( )
//
//  Description:		Accessor
//
//  Returns:			The active standalone document, NULL if there is none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument*	RMainframe::GetActiveStandaloneDocument( ) const
	{
	RStandaloneDocument* pDocument = NULL;

	// Get the active CView
	if( MDIGetActive( ) )
		{
		CView* pActiveCView = MDIGetActive( )->GetActiveView( );

		// Convert this to a Renaissance standalone view
		if( pActiveCView )
			{
			RStandaloneView* pActiveStandaloneView = dynamic_cast<RStandaloneView*>( pActiveCView );
			TpsAssert( pActiveStandaloneView, "Active CView is not an RStandaloneView." );

			// Get the document attached to the standalone view
			TpsAssertIsObject( RStandaloneDocument, pActiveStandaloneView->GetRDocument( ) );
			pDocument = static_cast<RStandaloneDocument*>( pActiveStandaloneView->GetRDocument( ) );
			}
		}

	return pDocument;
	}
