//****************************************************************************
//
// File Name:		DialCtrl.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of the dial control.   
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
//  $Logfile:: /PM8/Framework/Source/DialCtrl.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "DialCtrl.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DcDrawingSurface.h"
// #include "Resource.h"

#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RDialCtrl

RDialCtrl::RDialCtrl() :
	m_fDrawingDisabled( FALSE ),
	m_fInitialPaint( TRUE ),
	m_fMouseDown( FALSE ),
	m_dblAngle( 0.0 ),
	m_dbl_160Degrees( kPI * 0.89 ),
	m_dbl_90Degrees( kPI / 2.0 ),
	m_iMinRange( 0 ),
	m_iMaxRange( 0 ),
	m_iPosition( 0 ),
	m_dblTotalPositions( 0.0 )
{
	if ( !Initialize() )
		throw;
}

RDialCtrl::~RDialCtrl()
{
}


BEGIN_MESSAGE_MAP( RDialCtrl, CButton )
	//{{AFX_MSG_MAP(RDialCtrl)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RDialCtrl message handlers


// ****************************************************************************
//
//  Function Name:	RDialCtrl::Initialize( )
//
//  Description:		Creates the pens and brushes with which to paint.
//
//  Returns:			TRUE:		If successful
//							FALSE:	If any of the brushes or pens fails creation
//
//  Exceptions:		None
//
// ****************************************************************************

BOOL	RDialCtrl::Initialize()
{
	m_crFace = ::GetSysColor( COLOR_3DFACE );

	COLORREF crBtnHiLight(	::GetSysColor( COLOR_3DHILIGHT	) );
	COLORREF crBtnShadow(	::GetSysColor( COLOR_3DSHADOW		) );
	COLORREF crBtnDkShadow(	::GetSysColor( COLOR_3DDKSHADOW	) );

	if ( !m_penHighlight.CreatePen(	PS_SOLID, 1, crBtnHiLight	) )
		return FALSE;

	if ( !m_penFace.CreatePen(			PS_SOLID, 1, m_crFace		) )
		return FALSE;

	if ( !m_penShadow.CreatePen(		PS_SOLID, 1, crBtnShadow	) )
		return FALSE;

	if ( !m_penDkShadow.CreatePen(	PS_SOLID, 1, crBtnDkShadow	) )
		return FALSE;

	if ( !m_brushFace.CreateSolidBrush(			m_crFace		) )
		return FALSE;

	if ( !m_brushHighlight.CreateSolidBrush(	crBtnHiLight	) )
		return FALSE;

	if ( !m_brushShadow.CreateSolidBrush(		crBtnShadow		) )
		return FALSE;

	if ( !m_brushDkShadow.CreateSolidBrush(	crBtnDkShadow	) )
		return FALSE;

	return TRUE;
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::InitializeRects( )
//
//  Description:		Initializes the client and bounds rects.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RDialCtrl::InitializeRects()
{
	// Get the control's dimensions
	GetClientRect( &m_rcClient );
	TpsAssert( ( m_rcClient.right > 0 ) && ( m_rcClient.bottom > 0 ), "Client rect is empty" );

	int	iDiameter = min( m_rcClient.Width(), m_rcClient.Height() );

	// Create a rectangle for the dial circle that's 3 pixels smaller than the
	// window (e.g., leaves 1 pixel to the top and left for the highlight,
	// 1 pixel to the bottom and left for the shadow, 1 pixel bottom and left for
	// the dark shadow)
	m_rcBounds.left	= 1;
	m_rcBounds.top		= 1;
	m_rcBounds.right	= iDiameter - 2;
	m_rcBounds.bottom	= iDiameter - 2;

	m_ptDialCtr		= m_rcBounds.CenterPoint();
	m_dblRadius		= double( m_rcBounds.Width() ) / 2.0;
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::DrawItem( )
//
//  Description:		WM_DRAWITEM message handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDialCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
	if ( m_fInitialPaint )
	{
		InitializeRects();
		m_fInitialPaint = FALSE;
	}
	
	//	We may be asked to draw when we are in the middle of composing the bitmap.
	//	Wait for another draw message.	
	if ( m_fDrawingDisabled )
		return;
		
	DrawDial( CDC::FromHandle( lpDrawItemStruct->hDC ) );
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::OnPaint( )
//
//  Description:		Handler for WM_PAINT messages
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDialCtrl::OnPaint() 
{
	CPaintDC dc( this ); // device context for painting

	if ( m_fInitialPaint )
	{
		InitializeRects();
		m_fInitialPaint = FALSE;
	}
	
	DrawDial( &dc );
	
	// Do not call CButton ::OnPaint() for painting messages
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::OnLButtonDown( )
//
//  Description:		Handler for WM_LBUTTONDOWN messages.  Dial only spins when
//							the button is down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDialCtrl::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	if ( IsPointOnFace( point ) )
	{
		m_fMouseDown = TRUE;

		CalcAngle( point );
		Invalidate( FALSE );

		GetParentOwner()->SendMessage( WM_USER_DIAL_LMOUSEDOWN, WPARAM( m_iPosition ), 0 );
	}
	
	CButton::OnLButtonDown(nFlags, point);
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::OnLButtonUp( )
//
//  Description:		Handler for WM_LBUTTONUP messages
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDialCtrl::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	if ( m_fMouseDown && IsPointOnFace( point ) )
	{
		CalcAngle( point );
		CalcPosition();
		Invalidate( FALSE );

		GetParentOwner()->SendMessage( WM_USER_DIAL_LMOUSEUP, WPARAM( m_iPosition ), 0 );

		m_fMouseDown = FALSE;
	}
	
	CButton::OnLButtonUp(nFlags, point);
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::OnMouseMove( )
//
//  Description:		Handler for WM_MOUSEMOVE messages.  Dial only spins when
//							mouse button is down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDialCtrl::OnMouseMove( UINT nFlags, CPoint point ) 
{
	if ( m_fMouseDown && IsPointOnFace( point ) )
	{
		CalcAngle( point );
		CalcPosition();
		Invalidate( FALSE );

		GetParentOwner()->SendMessage( WM_USER_DIAL_MOVED, WPARAM( m_iPosition ), 0 );
	}
	
	CButton ::OnMouseMove(nFlags, point);
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::IsPointOnFace( )
//
//  Description:		Tests whether point is on the dial face
//
//  Returns:			TRUE:		If point is on dial face
//							FALSE:	If point is outside the dial face
//
//  Exceptions:		None
//
// ****************************************************************************

BOOL	RDialCtrl::IsPointOnFace( const CPoint& pt )	const
{
	int	iXLength = pt.x - m_ptDialCtr.x;
	int	iYLength = pt.y - m_ptDialCtr.y;
	
	return( _hypot( double( iXLength ), double( iYLength ) ) <= m_dblRadius );	
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::DrawDial( )
//
//  Description:		Draws the dial and its pointer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDialCtrl::DrawDial( CDC* pDC ) 
{
	CBitmap*	pOldBitmap	= NULL;
	CBrush*	pOldBrush	= NULL;
	CPen*		pOldPen		= NULL;	

	CDC	offscreenDC;

	try
	{
		CBitmap	offscreenBitmap;
		offscreenBitmap.CreateCompatibleBitmap( pDC, m_rcClient.Width(), m_rcClient.Height() );

		offscreenDC.CreateCompatibleDC( pDC );

		pOldBitmap	= ( CBitmap* )offscreenDC.SelectObject( &offscreenBitmap );

		// Paint the window rect the system button face color
		pOldBrush	= ( CBrush* )offscreenDC.SelectObject( &m_brushFace );
		BOOL fPainted = offscreenDC.PatBlt( 0, 0, m_rcClient.Width(), m_rcClient.Height(), PATCOPY );

		// Paint the dial shadows to the offscreen bitmap
		pOldPen		= ( CPen* )offscreenDC.SelectObject( &m_penDkShadow );
		offscreenDC.SelectObject( &m_brushDkShadow );

		offscreenDC.Ellipse( 
			m_rcBounds.left	+ 2,
			m_rcBounds.top		+ 2,
			m_rcBounds.right	+ 2,
			m_rcBounds.bottom	+ 2 );

		offscreenDC.SelectObject( &m_penShadow );
		offscreenDC.SelectObject( &m_brushShadow );

		offscreenDC.Ellipse( 
			m_rcBounds.left	+ 1,
			m_rcBounds.top		+ 1,
			m_rcBounds.right	+ 1,
			m_rcBounds.bottom	+ 1 );

		// Paint the dial highlight to the offscreen bitmap
		offscreenDC.SelectObject( &m_penHighlight );
		offscreenDC.SelectObject( &m_brushHighlight );

		offscreenDC.Ellipse( 
			m_rcBounds.left	- 1,
			m_rcBounds.top		- 1,
			m_rcBounds.right	- 1,
			m_rcBounds.bottom	- 1 );

		// Paint the dial itself to the offscreen bitmap
		offscreenDC.SelectObject( &m_penFace );
		offscreenDC.SelectObject( &m_brushFace );

 		offscreenDC.Ellipse( &m_rcBounds );

		// Draw the indicator triangle.  The arrow tip is at pts[ 1 ].
		double dblAngle0 = m_dblAngle + m_dbl_160Degrees;
		double dblAngle2 = m_dblAngle - m_dbl_160Degrees;

		CPoint	pts[ 3 ];
		pts[ 0 ] = CPoint(	m_ptDialCtr.x + int( m_dblRadius * cos( dblAngle0 ) ),
									m_ptDialCtr.y - int( m_dblRadius * sin( dblAngle0 ) ) );

		pts[ 1 ] = CPoint(	m_ptDialCtr.x + int( m_dblRadius * cos( m_dblAngle ) ),
									m_ptDialCtr.y - int( m_dblRadius * sin( m_dblAngle ) ) );

		pts[ 2 ] = CPoint(	m_ptDialCtr.x + int( m_dblRadius * cos( dblAngle2 ) ),
									m_ptDialCtr.y - int( m_dblRadius * sin( dblAngle2 ) ) );

		offscreenDC.SelectObject( &m_penShadow );
		offscreenDC.SelectObject( &m_brushShadow );
		offscreenDC.Polygon( pts, sizeof( pts ) / sizeof( pts[ 0 ] ) );

		// Blit from the offscreen DC to the screen
		pDC->SetStretchBltMode( COLORONCOLOR );
		pDC->StretchBlt(
			0,
			0,
			m_rcClient.Width(),
			m_rcClient.Height(),
			&offscreenDC,
			0, 
			0,
			m_rcClient.Width(),
			m_rcClient.Height(),
			SRCCOPY
		);

		offscreenDC.SelectObject( pOldBitmap );
		offscreenDC.SelectObject( pOldPen );
		offscreenDC.SelectObject( pOldBrush );
	}
	catch( CResourceException* e )
	{
		if ( pOldBitmap )
			offscreenDC.SelectObject( pOldBitmap );

		if ( pOldBrush )
			offscreenDC.SelectObject( pOldBrush );

		if ( pOldPen )
			offscreenDC.SelectObject( pOldPen );

		e->Delete();
	}
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::CalcAngle( )
//
//  Description:		Calculates the angle in radians in the Cartesian system,
//							e.g., 0 degrees points to the right.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RDialCtrl::CalcAngle( const CPoint& pt )
{
	int	iAdjacent = pt.x - m_ptDialCtr.x;
	int	iOpposite = m_ptDialCtr.y - pt.y;
	m_dblAngle = atan2( double( iOpposite ), double( iAdjacent ) );

	// Ensure the angle is between 0 and 2PI.  Can't use the modulus operator
	// because the type is double.
	while ( m_dblAngle > k2PI )
		m_dblAngle -= k2PI;

	while ( m_dblAngle < 0.F )
		m_dblAngle += k2PI;
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::SetRange( )
//
//  Description:		Sets the extrema for values to be returned by the dial 
//							control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RDialCtrl::SetRange( int iMin, int iMax )
{
	TpsAssert( iMin >= UD_MINVAL, "iMin out of range" );
	TpsAssert( iMax <= UD_MAXVAL, "iMax out of range" );
	TpsAssert( iMin <= iMax, "iMin must be <= iMax" );

	m_iMinRange = iMin;
	m_iMaxRange = iMax;

	m_dblTotalPositions = double( m_iMaxRange - m_iMinRange );
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::SetPosition( )
//
//  Description:		Positions the dial control at a given value.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RDialCtrl::SetPosition( int iPosition )
{
	m_iPosition = iPosition;

	double dblAmountAround = double( m_iPosition - m_iMinRange ) / m_dblTotalPositions;

	m_dblAngle = dblAmountAround * k2PI;

	Invalidate( FALSE );
}


// ****************************************************************************
//
//  Function Name:	RDialCtrl::CalcPosition( )
//
//  Description:		Derives the position from the current angle.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RDialCtrl::CalcPosition()
{
	double dblAmountAround = m_dblAngle / k2PI;

	m_iPosition = int( dblAmountAround * m_dblTotalPositions ) + m_iMinRange;

	if ( m_iPosition > m_iMaxRange )
	{
		m_iPosition %= ( int( m_dblTotalPositions ) / 2 );

		if ( m_iPosition == 0 )
			m_iPosition = m_iMaxRange;
	}

	if ( m_iPosition < m_iMinRange )
	{
		m_iPosition %= ( int( m_dblTotalPositions ) / 2 );

		if ( m_iPosition == 0 )
			m_iPosition = m_iMinRange;
	}
}