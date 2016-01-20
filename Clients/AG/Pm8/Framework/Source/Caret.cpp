// ****************************************************************************
//
//  File Name:			Caret.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RCaret class
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
//  $Logfile:: /PM8/Framework/Source/Caret.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "Caret.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ViewDrawingSurface.h"
#include "ApplicationGlobals.h"
#include "View.h"

const	int	kCaretCacheSize	= 4;

// ****************************************************************************
//
//  Function Name:	RCaret::RCaret( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCaret::RCaret( )
	: m_VisibleCount( 0 ),
	  m_Size( 0, 0 ),
	  m_Rotation( 0.0 ),
	  m_Scale( 0, 0 ),
	  m_Position( 0, 0 ),
	  m_pView( NULL ),
	  m_fCaretDirty( TRUE )
#ifdef	_WINDOWS
	, m_CaretCache( kCaretCacheSize )
#endif	//	_WINDOWS
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCaret::~RCaret( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCaret::~RCaret( )
	{
	TpsAssert( m_pView == NULL, "The caret still has an owner in the destructor" );
	}

// ****************************************************************************
//
//  Function Name:	RCaret::SetCaretOwner( )
//
//  Description:		Set the current owner of the caret for retrieving transformations
//							and clipping information.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCaret::SetCaretOwner( RView* pView )
	{
	//	account for the fact that the KeyFocus and Activate occur at separate times
	if ( pView == m_pView )
		return;

//	TpsAssert( m_VisibleCount <= 0,"SetCaretOwner called with visible count != 0" );
	m_pView			= pView;
	m_fCaretDirty	= TRUE;

	//
	//		Take ownership of the caret here...
	if ( pView )
		{
#ifdef	_WINDOWS
		::CreateCaret( (m_pView->GetCWnd()).GetSafeHwnd(), NULL, 1, 1 );
#endif
#ifdef	MAC
		//	Is anything necessary here...
#endif
		}
	}

// ****************************************************************************
//
//  Function Name:	RCaret::GetCaretOwner( )
//
//  Description:		Return the owner of the caret
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView* RCaret::GetCaretOwner( ) const
	{
		return m_pView;
	}

// ****************************************************************************
//
//  Function Name:	RCaret::ShowCaret( )
//
//  Description:		Show the caret of the counter is 0 or more. Increment
//							the counter.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCaret::ShowCaret( )
	{
	TpsAssert( (m_pView != NULL) || (m_VisibleCount<0), "ShowCaret called with owner view == NULL" );
	TpsAssert( m_VisibleCount < 10, "ShowCaret called with a visible count greater than 10" );


	if (m_VisibleCount == 0)
		{
		if (m_fCaretDirty)
			CreateCaret( );
#ifdef	_WINDOWS
		BOOLEAN	bSuccess = static_cast<BOOLEAN>( ::ShowCaret( (m_pView->GetCWnd()).GetSafeHwnd() ) );
#endif	//	_WINDOWS
#ifdef	MAC
#endif	//	MAC
		}

	++m_VisibleCount;
	}

// ****************************************************************************
//
//  Function Name:	RCaret::HideCaret( )
//
//  Description:		Hide the caret of the counter is 0 or less. Decrement
//							the counter.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCaret::HideCaret( )
	{
	TpsAssert( (m_pView != NULL)||(m_VisibleCount<=0), "HideCaret called with owner view == NULL" );
	TpsAssert( m_VisibleCount > -10, "HideCaret called with visible count less than -10" );

	--m_VisibleCount;

#ifdef	_WINDOWS
	if (m_VisibleCount == 0)
		{
		BOOLEAN	bSuccess = static_cast<BOOLEAN>( ::HideCaret( (m_pView->GetCWnd()).GetSafeHwnd() ) );
		}
#endif	//	_WINDOWS
#ifdef	MAC
#endif	//	MAC
	}


// ****************************************************************************
//
//  Function Name:	RCaret::SetCaretSize( )
//
//  Description:		Set the size of the caret to display.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCaret::SetCaretSize( RIntSize size, const R2dTransform& transform )
	{
	TpsAssert( m_pView != NULL, "SetCaretSize called with owner view == NULL" );
	TpsAssert( m_VisibleCount < 1, "SetCaretSize called when caret was visible" );

	YAngle			rotation;
	YRealDimension	xScale;
	YRealDimension	yScale;
	transform.Decompose( rotation, xScale, yScale );
	RRealSize scale( xScale, yScale );

	if ( (size != m_Size) || (scale != m_Scale) || (rotation != m_Rotation) )
		{
		m_Size			= size;
		m_Rotation		= rotation;
		m_fCaretDirty	= TRUE;
		}
	}

// ****************************************************************************
//
//  Function Name:	RCaret::SetCaretPosition( )
//
//  Description:		Set the Position of the caret (call should be in
//							logical coordinates relative to view).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCaret::SetCaretPosition( RRealPoint position )
	{
	R2dTransform	transform;

	TpsAssert( m_pView != NULL, "SetCaretPosition called with owner view == NULL" );

	RRealRect		bounds = m_pView->GetSize();
	RViewDrawingSurface	drawingSurface( m_pView );
	m_pView->GetViewTransform( transform, drawingSurface, TRUE );
	bounds.PinPointInRect( position );	//	Pin caret to view bounds (before transform)

#ifdef	_WINDOWS
	RRealVectorRect	caretBox	= RRealSize( m_Size );
	caretBox.Offset( RRealSize( position.m_x, position.m_y ) );
	caretBox *= transform;
	m_Position	= caretBox.m_TransformedBoundingRect.m_TopLeft;

	BOOLEAN	fSucessess = static_cast<BOOLEAN>( ::SetCaretPos( m_Position.m_x, m_Position.m_y ) );
#endif	//	_WINDOWS
#ifdef	MAC

#endif	//	MAC
	}

// ****************************************************************************
//
//  Function Name:	RCaret::CreateCaret( )
//
//  Description:		Create a caret to be blinked.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RCaret::CreateCaret( )
	{
	R2dTransform	transform;

	TpsAssert( m_pView != NULL, "CreateCaret called with owner view == NULL" );
	TpsAssert( m_VisibleCount <= 0, "CreateCaret called with VisibleCount > 0" );

	RViewDrawingSurface	drawingSurface( m_pView );
	RIntVectorRect			caretRect( m_Size );
	m_pView->GetViewTransform( transform, drawingSurface, TRUE );
	caretRect	*=	transform;		//	This takes into account any rotation.

#ifdef	_WINDOWS
	HWND	hwnd	= (m_pView->GetCWnd()).GetSafeHwnd();

	try
		{
		RGDICaretInfo	caretInfo( caretRect.m_TopRight, caretRect.m_BottomLeft );
		RGDICaret&		caret = m_CaretCache.GetObject( caretInfo );
		caret.SelectCaret( hwnd );
		m_CaretCache.ReleaseObject( caret );	
		}
	catch( ... )
		{
		//	If there was an error, just use plain old stupid caret
		int	height	= caretRect.m_BottomLeft.m_y - caretRect.m_TopRight.m_y;
		if (height < 0)
			height = -height;
		::CreateCaret( hwnd, NULL, 1, height );
		}

	//	Reset the caret position to insure proper placement
	::SetCaretPos( m_Position.m_x, m_Position.m_y );
#endif	//	_WINDOWS

	m_fCaretDirty	= FALSE;
	}


// ****************************************************************************
//
//  Function Name:	RAutoCaret::RAutoCaret( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoCaret::RAutoCaret( )
	{
	::GetCaret( ).HideCaret( );
	}

// ****************************************************************************
//
//  Function Name:	RAutoCaret::~RAutoCaret( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoCaret::~RAutoCaret( )
	{
	::GetCaret( ).ShowCaret( );
	}

