// ****************************************************************************
//
//  File Name:			RMessageDispatch.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMessageDispatch class
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
//  $Logfile:: /PM8/Framework/Include/MessageDispatch.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MESSAGEDISPATCH_H_
#define		_MESSAGEDISPATCH_H_

#include "DcDrawingSurface.h"
#include "ApplicationGlobals.h"
#include "Cursor.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RWindowView;

// ****************************************************************************
//
//  Class Name:		RMessageDispatch
//
//  Description:		Handles message dispatching
//
// ****************************************************************************
//
template <class Base> class RMessageDispatch : public Base
	{
	// Initialization
	protected :
		void									SetView( RWindowView* pView );

	// MFC Overrides
	public :
		virtual BOOL						OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );

	// Message handlers
	public :
		afx_msg void						OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg int							OnCreate( LPCREATESTRUCT lpCreateStruct );
		afx_msg void						OnContextMenu( CWnd* pWnd, CPoint pos );
		afx_msg BOOL						OnEraseBkgnd(CDC* pDC);
		afx_msg void						OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void						OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void						OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void						OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void						OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void						OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void						OnRButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void						OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void						OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void						OnMButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void						OnMButtonDown(UINT nFlags, CPoint point);
		afx_msg void						OnMButtonUp(UINT nFlags, CPoint point);
		afx_msg LRESULT					OnDisplayChange( WPARAM wParam, LPARAM lParam );
		afx_msg void						OnPaint( );
		afx_msg void						OnSize( UINT nType, int cx, int cy );
		afx_msg void						OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
		afx_msg void						OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
		afx_msg BOOL						OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
		afx_msg void						OnKillFocus(CWnd* pNewWnd);
		afx_msg void						OnSetFocus(CWnd* pOldWnd);

		DECLARE_MESSAGE_MAP( );

	// Implementation
	private :
		void									VectorMouseMessage( EMouseMessages eMessage,
																		  const RIntPoint& devicePoint,
																		  YModifierKey modifierKeys );

	// Members
	private :
		RWindowView*						m_pView;
	};

// ****************************************************************************
//
// The follwing is a template version of BEGIN_MESSAGE_MAP / END_MESSAGE_MAP
//
// ****************************************************************************
//
template <class Base> const AFX_MSGMAP* PASCAL RMessageDispatch<Base>:: _GetBaseMessageMap( )
	{
	return &Base::messageMap;
	}
	
template <class Base> const AFX_MSGMAP* RMessageDispatch<Base>::GetMessageMap( ) const
	{
	return &RMessageDispatch<Base>::messageMap;
	}

template <class Base> AFX_DATADEF const AFX_MSGMAP RMessageDispatch<Base>::messageMap =
	{
	&RMessageDispatch<Base>::_GetBaseMessageMap, &RMessageDispatch<Base>::_messageEntries[0]
	};

template <class Base> const AFX_MSGMAP_ENTRY RMessageDispatch<Base>::_messageEntries[] =
	{
	ON_WM_MDIACTIVATE( )
	ON_WM_ACTIVATEAPP( )
	ON_WM_CHAR( )
	ON_WM_CREATE( )
	ON_WM_ERASEBKGND( )
	ON_WM_LBUTTONDOWN( )
	ON_WM_LBUTTONUP( )
	ON_WM_KEYDOWN( )
	ON_WM_KEYUP( )
	ON_WM_CONTEXTMENU( )
	ON_WM_LBUTTONDBLCLK( )
	ON_WM_MOUSEMOVE( )
	ON_WM_RBUTTONDBLCLK( )
	ON_WM_RBUTTONDOWN( )
	ON_WM_RBUTTONUP( )
	ON_WM_MBUTTONDBLCLK( )
	ON_WM_MBUTTONDOWN( )
	ON_WM_MBUTTONUP( )
	ON_WM_PAINT( )
	ON_WM_SETCURSOR( )
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_MESSAGE( WM_DISPLAYCHANGE, OnDisplayChange )
	{ 0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
	};

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::SetView
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::SetView( RWindowView* pView )
	{
	m_pView = pView;
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnChar( )
//
//  Description:		Handles the WM_CHAR message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnChar(UINT nChar, UINT, UINT ) 
	{
	try
		{
		// Get the current modifier keys
		YModifierKey modifierKeys = m_pView->GetModifierKeyState( );

		// Call into the framework
		m_pView->OnXCharacter( nChar, modifierKeys );
		}

	catch( RForcedMemoryException& Debug( memException ) )
		{
		Debug( RForcedMemoryException tmp = memException; )
		::ReportException( kMemory );
		}
	catch( YException exception )
		{
		Debug( YException tmp = exception; )
		::ReportException( exception );
		}
	catch( CException* Debug( mfcException ) )
		{
		::ReportException( kUnknownError );
		Debug( CException* tmp = mfcException; )
		TpsAssertAlways( "An Exception was caught at the top level OnChar." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnEraseBkgnd( )
//
//  Description:		Handles the WM_ERASEBKGND message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> BOOL RMessageDispatch<Base>::OnEraseBkgnd( CDC* )
	{
	// Swallow this message, nobody wants it
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnCreate( )
//
//  Description:		Handles the WM_CREATE message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> int RMessageDispatch<Base>::OnCreate( LPCREATESTRUCT lpCreateStruct )
	{
	// Register the view as a drop target
	m_pView->m_OleDropTarget.Register( this );

	return Base::OnCreate( lpCreateStruct );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnContextMenu( )
//
//  Description:		Handles the WM_CONTEXTMENU message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnContextMenu( CWnd*, CPoint pos )
	{
	ScreenToClient( &pos );
	VectorMouseMessage( kContextMenu, RIntPoint( pos.x, pos.y ), 0 );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnLButtonDblClk( )
//
//  Description:		Handles the WM_LBUTTONDBLCLK message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnLButtonDblClk( UINT nFlags, CPoint point )
	{	
	VectorMouseMessage( kLeftButtonDoubleClick, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnLButtonDown( )
//
//  Description:		Handles the WM_LBUTTONDOWN message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnLButtonDown( UINT nFlags, CPoint point )
	{
	VectorMouseMessage( kLeftButtonDown, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnLButtonUp( )
//
//  Description:		Called on a WM_LBUTTONUP
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnLButtonUp( UINT nFlags, CPoint point) 
	{
	VectorMouseMessage( kLeftButtonUp, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnMouseMove( )
//
//  Description:		Called on WM_MOUSEMOVE
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnMouseMove(UINT nFlags, CPoint point) 
	{
	VectorMouseMessage( kMouseMove, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnRButtonDblClk( )
//
//  Description:		Called on WM_RBUTTONDOWN
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnRButtonDblClk(UINT nFlags, CPoint point) 
	{
	VectorMouseMessage( kRightButtonDoubleClick, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnRButtonDown( )
//
//  Description:		Called on WM_RBUTTONDOWN
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnRButtonDown(UINT nFlags, CPoint point) 
	{
	// Ignore this message if the mouse is captured; we dont want any context menus
	if( !m_pView->IsMouseCaptured( ) )
		VectorMouseMessage( kRightButtonDown, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnRButtonUp( )
//
//  Description:		Called on WM_RBUTTONUP
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnRButtonUp(UINT nFlags, CPoint point) 
	{
	// Ignore this message if the mouse is captured; we dont want any context menus
	if( !m_pView->IsMouseCaptured( ) )
		VectorMouseMessage( kRightButtonUp, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnMButtonDblClk( )
//
//  Description:		Called on WM_MBUTTONDBLCLK
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnMButtonDblClk(UINT nFlags, CPoint point) 
	{
	VectorMouseMessage( kMiddleButtonDoubleClick, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnMButtonDown( )
//
//  Description:		Called on WM_MBUTTONDOWN
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnMButtonDown(UINT nFlags, CPoint point) 
	{
	VectorMouseMessage( kMiddleButtonDown, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnMButtonUp( )
//
//  Description:		Called on WM_MBUTTONUP
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnMButtonUp(UINT nFlags, CPoint point) 
	{
	VectorMouseMessage( kMiddleButtonUp, RIntPoint( point.x, point.y ), nFlags );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch<Base>::OnKeyDown( )
//
//  Description:		Handles the WM_KEYDOWN message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	try
		{
		// Call into the framework
		m_pView->OnXKeyDown( static_cast<EVirtualCharacter>( nChar ), m_pView->GetModifierKeyState( ) );

		// Call the base method
		Base::OnKeyDown( nChar, nRepCnt, nFlags );
		}

	catch( RForcedMemoryException& Debug( memException ) )
		{
		Debug( RForcedMemoryException tmp = memException; )
		::ReportException( kMemory );
		}
	catch( YException exception )
		{
		Debug( YException tmp = exception; )
		::ReportException( exception );
		}
	catch( CException* Debug( mfcException ) )
		{
		::ReportException( kUnknownError );
		Debug( CException* tmp = mfcException; )
		TpsAssertAlways( "An Exception was caught at the top level OnKeyDown." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch<Base>::OnKeyUp( )
//
//  Description:		Called on WM_KEYUP
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	try
		{
		// Call into the framework
		m_pView->OnXKeyUp( static_cast<EVirtualCharacter>( nChar ), m_pView->GetModifierKeyState( ) );

		// Call the base method
		Base::OnKeyDown( nChar, nRepCnt, nFlags );
		}

	catch( RForcedMemoryException& Debug( memException ) )
		{
		Debug( RForcedMemoryException tmp = memException; )
		::ReportException( kMemory );
		}
	catch( YException exception )
		{
		Debug( YException tmp = exception; )
		::ReportException( exception );
		}
	catch( CException* Debug( mfcException ) )
		{
		::ReportException( kUnknownError );
		Debug( CException* tmp = mfcException; )
		TpsAssertAlways( "An Exception was caught at the top level OnKeyUp." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnPaint( )
//
//  Description:		Handles the WM_PAINT message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::OnPaint( ) 
	{
	// Create a dc
	CPaintDC dc( this );

	// Get the clip box and initialize rcRender with it
	CRect	rcClip;
	CRect	rcClient;
	GetClientRect( rcClient );
	dc.GetClipBox( rcClip );

	CRect	rcPaint;
	if( rcPaint.IntersectRect(rcClient,rcClip) )
		{
		RIntRect	rcRender( rcPaint.left, rcPaint.top, rcPaint.right + 1, rcPaint.bottom + 1 );
		
		// Create a drawing surface and intitialize it with the dc
		RDcDrawingSurface drawingSurface;
		drawingSurface.Initialize( dc.GetSafeHdc( ), dc.GetSafeHdc( ) );

		// Tell the view to render
		m_pView->OnXRender( drawingSurface, rcRender );

		POINT	pt;
		::GetCursorPos( &pt );
		ScreenToClient( &pt );
		RIntPoint devicePoint( pt.x, pt.y );
		m_pView->OnXMouseMessage( kSetCursor, devicePoint, 0 );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnDisplayChange( )
//
//  Description:		Called when this window receives a WM_DISPLAYCHANGE message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> afx_msg LRESULT RMessageDispatch<Base>::OnDisplayChange( WPARAM wParam, LPARAM lParam )
	{
	// Call the cross platform method
	m_pView->OnXDisplayChange( );

	return CWnd::OnDisplayChange( wParam, lParam );
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch<Base>::VectorMouseMessage
//
//  Description:		Vectors mouse messages to our common handler, and handles
//							any errors resulting
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> void RMessageDispatch<Base>::VectorMouseMessage( EMouseMessages eMessage,
																							  const RIntPoint& devicePoint,
																							  YModifierKey modifierKeys )
	{
	try
		{
		// Call the common mouse message handler
		m_pView->OnXMouseMessage( eMessage, devicePoint, modifierKeys );

		// We still want the default processing to happen for mouse messages
		Default( );
		}

	// Catch forced memory exceptions
	catch( RForcedMemoryException& Debug( memException ) )
		{
		Debug( RForcedMemoryException tmp = memException; )
		::ReportException( kMemory );
		}

	// Catch Renaissance exceptions
	catch( YException exception )
		{
		::ReportException( exception );
		}

	// Catch MFC Exceptions
	catch( CException* Debug( mfcException ) )
		{
		::ReportException( kUnknownError );
		Debug( CException* tmp = mfcException; )
		TpsAssertAlways( "An Exception was caught at the top level mouse handler." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneViewMessageDispatch::OnSetCursor( )
//
//  Description:		Handles the WM_SETCURSOR message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> BOOL RMessageDispatch<Base>::OnSetCursor(CWnd*, UINT, UINT ) 
	{
	//	Do nothing since we will call SetCursor on a MouseMove message
	GetCursorManager( ).ClearCursor( );

	//	If the cursor position is NOT in the clientRect of the window,
	//	just set the cursor to the arrow...
	POINT	pt;
	if( ::GetCursorPos( &pt ) )
		{
		// Convert to client coordinates
		m_pView->GetCWnd( ).ScreenToClient( &pt );

		// Get the client rect
		RECT clientRect;
		m_pView->GetCWnd( ).GetClientRect( &clientRect );

		// If the point is not in the client rect, set the cursor
		if ( !PtInRect( &clientRect, pt ) )
			GetCursorManager( ).SetCursor( IDC_ARROW );
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneViewMessageDispatch::OnSize( )
//
//  Description:		Handle WM_SIZE processing
//
//  Returns:			Default
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> afx_msg void RMessageDispatch<Base>::OnSize( UINT, int cx, int cy )
	{
	m_pView->OnXResize( RRealSize( cx, cy ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneViewMessageDispatch::OnHScroll( )
//
//  Description:		Handler for WM_HSCROLL
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> afx_msg void RMessageDispatch<Base>::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* )
	{
	m_pView->OnHScroll( nSBCode, static_cast<int>( nPos ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneViewMessageDispatch::OnVScroll( )
//
//  Description:		Handler for WM_VSCROLL
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> afx_msg void RMessageDispatch<Base>::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* ) 
	{
	m_pView->OnVScroll( nSBCode, static_cast<int>( nPos ) );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneViewMessageDispatch::OnSetFocus( )
//
//  Description:		Handles the WM_SETFOCUS message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> afx_msg void RMessageDispatch<Base>::OnSetFocus(CWnd* pOldWnd) 
	{
	try
		{
		// Send to the framework
		m_pView->OnXSetKeyFocus( );

		// Call the base method
		Base::OnSetFocus( pOldWnd );
		}

	catch( RForcedMemoryException& Debug( memException ) )
		{
		Debug( RForcedMemoryException tmp = memException; )
		::ReportException( kMemory );
		}
	catch( YException exception )
		{
		Debug( YException tmp = exception; )
		::ReportException( exception );
		}
	catch( CException* Debug( mfcException ) )
		{
		::ReportException( kUnknownError );
		Debug( CException* tmp = mfcException; )
		TpsAssertAlways( "An Exception was caught at the top level OnSetFocus." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneViewMessageDispatch::OnKillFocus( )
//
//  Description:		Handles the WM_KILLFOCUS message
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> afx_msg void RMessageDispatch<Base>::OnKillFocus(CWnd* pNewWnd) 
	{
	try
		{
		// Call into the framework
		m_pView->OnXKillKeyFocus();

		// Call the base method
		Base::OnKillFocus(pNewWnd);
		}

	catch( RForcedMemoryException& Debug( memException ) )
		{
		Debug( RForcedMemoryException tmp = memException; )
		::ReportException( kMemory );
		}
	catch( YException exception )
		{
		Debug( YException tmp = exception; )
		::ReportException( exception );
		}
	catch( CException* Debug( mfcException ) )
		{
		::ReportException( kUnknownError );
		Debug( CException* tmp = mfcException; )
		TpsAssertAlways( "An Exception was caught at the top level OnKillFocus." );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMessageDispatch::OnCmdMsg( )
//
//  Description:		Override of the standard MFC command handler. Windows
//							commands are dispatched as Command xEvents. Any command not
//							handled is passed back to CView::OnCmdMsg( ).
//
//  Returns:			TRUE:		The command was handled.
//							FALSE:	The command was not handled.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class Base> BOOL RMessageDispatch<Base>::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
	{
	BOOLEAN fReturn = FALSE;

	try
		{
		// Handle command UI
		if( nCode == CN_UPDATE_COMMAND_UI )
			{
			// Get and validate a pointer to the MFC command UI object
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			TpsAssertIsObject( CCmdUI, pCmdUI );

			// Create a Renaissance command UI object from it
			RMFCCommandUI cmdUI( pCmdUI );

			// Route it through our command map
			fReturn = m_pView->GetRCommandMap( )->OnCommandUI( cmdUI, m_pView );

			// If it wasn't handled, let the base class try
			if( !fReturn )
				fReturn = (BOOLEAN)Base::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
			}

		// Let MFC handle help
		else if( nID == ID_HELP )
			fReturn = (BOOLEAN)Base::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );

		// Route commands through our command maps
		else if( m_pView->GetRCommandMap( )->OnCommand( nID, m_pView, static_cast<BOOLEAN>( pHandlerInfo == NULL ) ) )
			fReturn = TRUE;

		// If we couldn't handle it, let the base class try
		else
			fReturn = (BOOLEAN)Base::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
		}

	catch( YException exception )
		{
		::ReportException( exception );
		}

	catch( RForcedMemoryException& )
		{
		::ReportException( kMemory );
		}

	return fReturn;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _MESSAGEDISPATCH_H_
