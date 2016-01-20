//****************************************************************************
//
// File Name:		ControlContainer.cpp
//
// Project:			Renaissance Application Framework
//
// Author:			S Athanas
//
// Description:	Definition of RControlDocument and RControlView.
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ControlContainer.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ControlContainer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "StandaloneApplication.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "StandaloneView.h"

//
// Message map
BEGIN_MESSAGE_MAP( RControlView, RMessageDispatch<CWnd> )
	ON_WM_GETDLGCODE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//
// Window message handlers
UINT RControlView::OnGetDlgCode()
{
//
// Return the input we want: a combination of the following...
//		DLGC_BUTTON				Button (generic).
//		DLGC_DEFPUSHBUTTON	Default pushbutton.
//		DLGC_HASSETSEL			EM_SETSEL messages.
//		DLGC_UNDEFPUSHBUTTON	No default pushbutton processing. (An application can use this flag with DLGC_BUTTON to indicate that it processes button input but relies on the system for default pushbutton processing.)
//		DLGC_RADIOBUTTON		Radio button.
//		DLGC_STATIC				Static control.
//		DLGC_WANTALLKEYS		All keyboard input.
//		DLGC_WANTARROWS		Arrow keys.
//		DLGC_WANTCHARS			WM_CHAR messages.
//		DLGC_WANTMESSAGE		All keyboard input. The application passes this message on to the control.
//		DLGC_WANTTAB			TAB key.
	return DLGC_WANTALLKEYS|DLGC_WANTMESSAGE;
}

void RControlView::OnContextMenu( CWnd*, CPoint )
{
	//
	// Ignore it...
	return;
}

// ****************************************************************************
//
//  Function Name:	RControlDocument::RControlDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlDocument::RControlDocument( )
	{
	Initialize( &::GetApplication( ), 0 );
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::~RControlDocument( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlDocument::~RControlDocument( )
	{
	//
	// Delete the contents...
	FreeDocumentContents();
	//
	// Delete the views...
	YViewIterator iterator = GetViewCollectionStart( );
	for( ; iterator != GetViewCollectionEnd( ); ++iterator )
		{
		RControlView*	pView	= dynamic_cast<RControlView*>( *iterator );
		delete pView;
		}
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::CreateView( )
//
//  Description:		create a control view for this document, subclassing the
//                   given window...
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView* RControlDocument::CreateView( CDialog* pDialog, int nControlId )
{
	return new RControlView( pDialog, nControlId, this );
}

// ****************************************************************************
//
//  Function Name:	RControlDocument::GetMinObjectSize( )
//
//  Description:		Pure virtual in RStandaloneDocument; stubbed out.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RControlDocument::GetMinObjectSize( const YComponentType&, BOOLEAN /* fMaintainAspect */ )
	{
	return RRealSize( 1, 1 );
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::GetMaxObjectSize( )
//
//  Description:		Pure virtual in RStandaloneDocument; stubbed out.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RControlDocument::GetMaxObjectSize( const YComponentType& )
	{
	return RRealSize( INT_MAX, INT_MAX );
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::GetDefaultObjectSize( )
//
//  Description:		Pure virtual in RStandaloneDocument; stubbed out.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RControlDocument::GetDefaultObjectSize( const YComponentType& )
	{
	UnimplementedCode( );
	return RRealSize( );
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::GetFontSizeInfo( )
//
//  Description:		Pure virtual in RStandaloneDocument; stubbed out.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RFontSizeInfo* RControlDocument::GetFontSizeInfo( ) const
	{
	return &m_FontInfo;
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::GetSaveFileFormats( )
//
//  Description:		Pure virtual in RStandaloneDocument; stubbed out.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RControlDocument::GetSaveFileFormats( RFileFormatCollection& ) const
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RControlDocument::GetMinObjectSize( )
//
//  Description:		Pure virtual in RStandaloneDocument; stubbed out.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RControlDocument::SetDefaultFileFormat( )
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RControlView::RControlView( CDialog* pdlg, int nctlid )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView::RControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument )
	{
	//
	// subclass the window...
	SubclassControl( pDialog, nControlId );

	Initialize( pDocument );
	}

// ****************************************************************************
//
//  Function Name:	RControlView::SubclassControl( CDialog* pdlg, int nctlid )
//
//  Description:		subclass the given control
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RControlView::SubclassControl( CDialog* pDialog, int nControlId )
	{
	SetView( this );
	//
	// Get the control we're stealing...
	CWnd*	pCtl	= pDialog->GetDlgItem( nControlId );
	CWnd* pPrev = pCtl->GetNextWindow( GW_HWNDPREV ) ;
	DWORD dwStyle = pCtl->GetStyle() ;
	DWORD dwExStyle = GetWindowLong( pCtl->m_hWnd, GWL_EXSTYLE ) ;

	//
	// Get its bounding rect in dialog client coordinates
	CRect	rcCtl;
	pCtl->GetWindowRect( rcCtl );
	pDialog->ScreenToClient( rcCtl );
	//
	// Destroy it
	pCtl->DestroyWindow();
	//
	// Create a new window...
	BOOLEAN fResult = BOOLEAN( CWnd::CreateEx( dwExStyle, NULL, "", dwStyle, rcCtl.left,
		rcCtl.top, rcCtl.Width(), rcCtl.Height(), pDialog->m_hWnd, (HMENU) nControlId ) );

	if (fResult)
		{
		CWnd::SetWindowPos( pPrev, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE ) ;
		}

	return fResult ;
	}

// ****************************************************************************
//
//  Function Name:	RControlView::UpdateScrollBars( )
//
//  Description:		Stub
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RControlView::UpdateScrollBars( EScrollUpdateReason )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RControlView::SetClipRegion( )
//
//  Description:		Creates a new clip region that is the intersection of the
//							current clip region and a region that will clip all output
//							to the bounds of this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RControlView::SetClipRegion( RDrawingSurface& drawingSurface, const R2dTransform& ) const
	{
	//
	// Get our window bounds...
	CRect	rc;
	GetCWnd().GetClientRect( rc );
	//
	// Intersect our bounds with the current clip region...
	drawingSurface.IntersectClipRect( RIntRect(rc) );
	}

// ****************************************************************************
//
//  Function Name:	RControlView::GetCWnd( )
//
//  Description:		Gets the CWnd
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
CWnd& RControlView::GetCWnd( ) const
	{
	return *(CWnd*)this;
	}

// ****************************************************************************
//
//  Function Name:	RControlView::GetBackgroundColor( )
//
//  Description:		Accessor
//
//  Returns:			The background color of this view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RColor RControlView::GetBackgroundColor( ) const
{
	RColor backgroundColor;

#ifdef _WINDOWS
	// always use white
	backgroundColor = RSolidColor( kWhite );
#endif

	return backgroundColor;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RControlDocument::Validate( )
//
//  Description:		Validate the Document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RControlDocument::Validate( ) const
	{
	RStandaloneDocument::Validate( );
	}

// ****************************************************************************
//
//  Function Name:	RControlView::Validate( )
//
//  Description:		Validate the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RControlView::Validate( ) const
	{
	RWindowView::Validate( );
	}

#endif	// TPSDEBUG
