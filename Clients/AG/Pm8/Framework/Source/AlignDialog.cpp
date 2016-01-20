//****************************************************************************
//
// File Name:		AlignDialog.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of RAlignDialog.   
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
//  $Logfile:: /PM8/Framework/Source/AlignDialog.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "AlignDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

//#include "Resource.h"

#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RAlignDialog dialog


RAlignDialog::RAlignDialog( CWnd* pParent /*=NULL*/ )
	: CDialog( RAlignDialog::IDD, pParent )
{
	//{{AFX_DATA_INIT(RAlignDialog)
	m_iAlignHorz = -1;
	m_iAlignVert = -1;
	//}}AFX_DATA_INIT

	m_eAlignHorz = RAlignSelectionAction::kAlignNoneHorz;
	m_eAlignVert = RAlignSelectionAction::kAlignNoneVert;
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::DoDataExchange( )
//
//  Description:		Standard validation routine
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RAlignDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	//{{AFX_DATA_MAP(RAlignDialog)
	DDX_Control(	pDX,	CONTROL_STATIC_ALIGN,				m_staticDisplay	);
	DDX_Radio(		pDX,	CONTROL_RADIO_ALIGN_HORZ_NONE,	m_iAlignHorz		);
	DDX_Radio(		pDX,	CONTROL_RADIO_ALIGN_VERT_NONE,	m_iAlignVert		);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( RAlignDialog, CDialog )
	//{{AFX_MSG_MAP(RAlignDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP

	ON_COMMAND_RANGE( CONTROL_RADIO_ALIGN_HORZ_NONE, CONTROL_RADIO_ALIGN_HORZ_RIGHT,		OnAlignHorizontal	)
	ON_COMMAND_RANGE( CONTROL_RADIO_ALIGN_VERT_NONE, CONTROL_RADIO_ALIGN_VERT_BOTTOM,	OnAlignVertical	)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RAlignDialog message handlers

// ****************************************************************************
//
//  Function Name:	RAlignDialog::OnInitDialog( )
//
//  Description:		Performs dialog initialization on creation.
//
//  Returns:			TRUE
//
//  Exceptions:		None
//
// ****************************************************************************

// virtual
BOOL	RAlignDialog::OnInitDialog()
{
	BOOL fInit = CDialog::OnInitDialog();

	InitializeBrushesAndPens();
	InitializeRadioControls();
	InitializeStaticControlData();

	// Don't call UpdateDisplay() here.  The window isn't painted yet so it
	// won't do anything.  The OnPaint() will handle painting the static control.

	return fInit;
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::InitializeBrushesAndPens( )
//
//  Description:		Creates the magenta, green, and blue brushes and pens used
//							by the static control to display object alignment.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RAlignDialog::InitializeBrushesAndPens()
{
	COLORREF crBlack		= RGB(   0,   0,   0 );
	COLORREF crRedBlue	= RGB( 255,   0,   255 );
	COLORREF crGreenBlue	= RGB(   0, 255,   255 );

	m_penBlack.CreatePen( PS_SOLID, 2, crBlack );

	m_brushRedBlue.CreateSolidBrush( crRedBlue );
	m_brushGreenBlue.CreateSolidBrush( crGreenBlue );
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::InitializeRadioControls( )
//
//  Description:		Selects whichever of the radio controls are indicated by
//							the value of their corresponding enumerated types.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RAlignDialog::InitializeRadioControls()
{
	CButton*	pbtnHorz = NULL;
	CButton* pbtnVert = NULL;

	switch( m_eAlignHorz )
	{
	case RAlignSelectionAction::kAlignNoneHorz:
		pbtnHorz = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_HORZ_NONE );
		break;

	case RAlignSelectionAction::kAlignLeft:
		pbtnHorz = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_HORZ_LEFT );
		break;

	case RAlignSelectionAction::kAlignCenterHorz:
		pbtnHorz = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_HORZ_CENTER );
		break;

	case RAlignSelectionAction::kAlignRight:
		pbtnHorz = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_HORZ_RIGHT );
		break;
	}

	pbtnHorz->SetCheck( 1 );

	switch( m_eAlignVert )
	{
	case RAlignSelectionAction::kAlignNoneVert:
		pbtnVert = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_VERT_NONE );
		break;

	case RAlignSelectionAction::kAlignTop:
		pbtnVert = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_VERT_TOP );
		break;

	case RAlignSelectionAction::kAlignCenterVert:
		pbtnVert = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_VERT_CENTER );
		break;

	case RAlignSelectionAction::kAlignBottom:
		pbtnVert = ( CButton* )GetDlgItem( CONTROL_RADIO_ALIGN_VERT_BOTTOM );
		break;
	}

	pbtnVert->SetCheck( 1 );
}

// ****************************************************************************
//
//  Function Name:	RAlignDialog::InitializeStaticControlData( )
//
//  Description:		Initializes data used to draw the alignment sample in the
//							static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RAlignDialog::InitializeStaticControlData()
{
	m_staticDisplay.GetClientRect( &m_rcDisplay );
	m_sizeStatic.cx	= m_rcDisplay.Width();
	m_sizeStatic.cy	= m_rcDisplay.Height();
	m_ptStaticCenter	= m_rcDisplay.CenterPoint();

	// The static control has a border outside of which we don't paint.
	// The display rect is the area inside this border.
	int iBorder = int( EStaticDimensions::kBorder );
	m_rcDisplay.InflateRect( CSize( -iBorder, -iBorder ) );

	int	iStaticWidth = m_rcDisplay.Width();
 	int	iStaticHeight = m_rcDisplay.Height();

	// Make the circle half the width of the static control
	m_sizeCircle.cx		= iStaticWidth / 3;
	m_sizeCircle.cy		= iStaticHeight / 2;

	// Make the rectangle 1/4 x 3/4 the width of the static control
	m_sizeRectangle.cx	= iStaticWidth / 2;
	m_sizeRectangle.cy	= iStaticHeight / 3;
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::OnAlignHorizontal( )
//
//  Description:		Command handler for horizontal alignment radio controls.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RAlignDialog::OnAlignHorizontal( UINT nID )
{
	switch ( nID )
	{
	case CONTROL_RADIO_ALIGN_HORZ_NONE:
		m_eAlignHorz = RAlignSelectionAction::kAlignNoneHorz;
		break;

	case CONTROL_RADIO_ALIGN_HORZ_LEFT:
		m_eAlignHorz = RAlignSelectionAction::kAlignLeft;
		break;

	case CONTROL_RADIO_ALIGN_HORZ_CENTER:
		m_eAlignHorz = RAlignSelectionAction::kAlignCenterHorz;
		break;

	case CONTROL_RADIO_ALIGN_HORZ_RIGHT:
		m_eAlignHorz = RAlignSelectionAction::kAlignRight;
		break;
	}

	UpdateDisplay();
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::OnAlignVertical( )
//
//  Description:		Command handler for vertical alignment radio controls.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RAlignDialog::OnAlignVertical( UINT nID )
{
	switch ( nID )
	{
	case CONTROL_RADIO_ALIGN_VERT_NONE:
		m_eAlignVert = RAlignSelectionAction::kAlignNoneVert;
		break;

	case CONTROL_RADIO_ALIGN_VERT_TOP:
		m_eAlignVert = RAlignSelectionAction::kAlignTop;
		break;

	case CONTROL_RADIO_ALIGN_VERT_CENTER:
		m_eAlignVert = RAlignSelectionAction::kAlignCenterVert;
		break;

	case CONTROL_RADIO_ALIGN_VERT_BOTTOM:
		m_eAlignVert = RAlignSelectionAction::kAlignBottom;
		break;
	}

	UpdateDisplay();
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::UpdateDisplay( )
//
//  Description:		Updates the arrangement of the objects in the static 
//							control display.
//
//  Returns:			Nothing
//
//  Exceptions:		CResourceException
//
// ****************************************************************************

void	RAlignDialog::UpdateDisplay()
{
	CRect rcCircle;
	CRect	rcRectangle;

	DeriveCircleRect( rcCircle );
	DeriveRectangleRect( rcRectangle );

	CBitmap*		pOldBitmap	= NULL;
	CPen*			pOldPen		= NULL;
	CBrush*		pOldBrush	= NULL;
	CDC			offscreenDC;

	try
	{
		CClientDC	clientDC( &m_staticDisplay );

		CBitmap		offscreenBitmap;
		offscreenBitmap.CreateCompatibleBitmap( &clientDC, m_sizeStatic.cx, m_sizeStatic.cy );

		offscreenDC.CreateCompatibleDC( &clientDC );
		pOldBitmap = ( CBitmap* )offscreenDC.SelectObject( &offscreenBitmap );

		// Paint the offscreen bitmap white
		offscreenDC.PatBlt( 0, 0, m_sizeStatic.cx, m_sizeStatic.cy, WHITENESS );

		// The shapes will all be outlined in black
		pOldPen		= offscreenDC.SelectObject( &m_penBlack );

		// Draw a blue rectangle (always on the bottom if there's overlap)
		pOldBrush	= offscreenDC.SelectObject( &m_brushGreenBlue );
		offscreenDC.Rectangle( rcRectangle );

		// Draw a red circle (always in the middle if there's overlap)
		offscreenDC.SelectObject( &m_brushRedBlue );
		offscreenDC.Rectangle( rcCircle );

		// Draw a left, right or center horizontal alignment line
		switch (m_eAlignHorz)
		{
		case RAlignSelectionAction::kAlignLeft:
			offscreenDC.MoveTo(rcRectangle.TopLeft().x, m_rcDisplay.top);
			offscreenDC.LineTo(rcRectangle.TopLeft().x, m_rcDisplay.bottom);
			break;
		case RAlignSelectionAction::kAlignRight:
			offscreenDC.MoveTo(rcRectangle.BottomRight().x, m_rcDisplay.top);
			offscreenDC.LineTo(rcRectangle.BottomRight().x, m_rcDisplay.bottom);
			break;
		case RAlignSelectionAction::kAlignCenterHorz:
			offscreenDC.MoveTo(rcRectangle.TopLeft().x + (rcRectangle.Width() / 2), m_rcDisplay.top);
			offscreenDC.LineTo(rcRectangle.TopLeft().x + (rcRectangle.Width() / 2), m_rcDisplay.bottom);
			break;
		case RAlignSelectionAction::kAlignNoneHorz:
		default:
			break;
		}

		// Draw a top, bottom or center vertical alignment line
		switch (m_eAlignVert)
		{
		case RAlignSelectionAction::kAlignTop:
			offscreenDC.MoveTo(m_rcDisplay.left, rcRectangle.TopLeft().y);
			offscreenDC.LineTo(m_rcDisplay.right, rcRectangle.TopLeft().y);
			break;
		case RAlignSelectionAction::kAlignBottom:
			offscreenDC.MoveTo(m_rcDisplay.left, rcRectangle.BottomRight().y);
			offscreenDC.LineTo(m_rcDisplay.right, rcRectangle.BottomRight().y);
			break;
		case RAlignSelectionAction::kAlignCenterVert:
			offscreenDC.MoveTo(m_rcDisplay.left, rcRectangle.TopLeft().y + (rcRectangle.Height() / 2));
			offscreenDC.LineTo(m_rcDisplay.right, rcRectangle.TopLeft().y + (rcRectangle.Height() / 2));
			break;
		case RAlignSelectionAction::kAlignNoneVert:
		default:
			break;
		}

		// Blit the offscreen bitmap onto the static control
		clientDC.SetStretchBltMode( COLORONCOLOR );
		clientDC.StretchBlt(
			0,
			0,
			m_sizeStatic.cx,
			m_sizeStatic.cy,
			&offscreenDC,
			0,
			0,
			m_sizeStatic.cx,
			m_sizeStatic.cy,
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

		if ( pOldPen )
			offscreenDC.SelectObject( pOldPen );

		if ( pOldBrush )
			offscreenDC.SelectObject( pOldBrush );

		e->Delete();
	}
}

// ****************************************************************************
//
//  Function Name:	RAlignDialog::DeriveCircleRect( )
//
//  Description:		Determines the aligned coordinates of the rect that 
//							encloses the circle to be displayed in the static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RAlignDialog::DeriveCircleRect( CRect& rcCircle )
{
	switch( m_eAlignHorz )
	{
	case RAlignSelectionAction::kAlignNoneHorz:
	case RAlignSelectionAction::kAlignLeft:
		rcCircle.left = m_rcDisplay.left + (m_rcDisplay.Width() / 7);
		break;

	case RAlignSelectionAction::kAlignRight:
		rcCircle.left = m_rcDisplay.right - (m_rcDisplay.Width() / 7) - m_sizeCircle.cx;
		break;

	case RAlignSelectionAction::kAlignCenterHorz:
		rcCircle.left = m_ptStaticCenter.x - ( m_sizeCircle.cx / 2 );
		break;
	}

	rcCircle.right = rcCircle.left + m_sizeCircle.cx;

	switch( m_eAlignVert )
	{
	case RAlignSelectionAction::kAlignNoneVert:
	case RAlignSelectionAction::kAlignTop:
		rcCircle.top = m_rcDisplay.top + (m_rcDisplay.Height() / 7);
		break;

	case RAlignSelectionAction::kAlignCenterVert:
		rcCircle.top = m_ptStaticCenter.y - ( m_sizeCircle.cy / 2 );
		break;

	case RAlignSelectionAction::kAlignBottom:
		rcCircle.top = m_rcDisplay.bottom - (m_rcDisplay.Height() / 7) - m_sizeCircle.cy;
		break;
	}

	rcCircle.bottom = rcCircle.top + m_sizeCircle.cy;
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::GetHorizontalAlignment( )
//
//  Description:		Returns the type of horizontal alignment. 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************

RAlignSelectionAction::EAlignHorizontal	RAlignDialog::GetHorizontalAlignment( )
	{
	return m_eAlignHorz;
	}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::GetVerticalAlignment( )
//
//  Description:		Returns the type of vertical alignment. 
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************

RAlignSelectionAction::EAlignVertical	RAlignDialog::GetVerticalAlignment( )
	{
	return m_eAlignVert;
	}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::DeriveRectangleRect( )
//
//  Description:		Determines the aligned coordinates of the rectangle rect 
//							to be displayed in the static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RAlignDialog::DeriveRectangleRect( CRect& rcRectangle )
{
	switch( m_eAlignHorz )
	{
	case RAlignSelectionAction::kAlignNoneHorz:
	case RAlignSelectionAction::kAlignRight:
		rcRectangle.left = m_rcDisplay.right - (m_rcDisplay.Width() / 7) - m_sizeRectangle.cx;
 		break;

	case RAlignSelectionAction::kAlignLeft:
		rcRectangle.left = m_rcDisplay.left + (m_rcDisplay.Width() / 7);
		break;

	case RAlignSelectionAction::kAlignCenterHorz:
		rcRectangle.left = m_ptStaticCenter.x - ( m_sizeRectangle.cx / 2 );
		break;
	}

	rcRectangle.right	= rcRectangle.left + m_sizeRectangle.cx;

	switch( m_eAlignVert )
	{
	case RAlignSelectionAction::kAlignNoneVert:
	case RAlignSelectionAction::kAlignBottom:
		rcRectangle.top = m_rcDisplay.bottom - (m_rcDisplay.Height() / 7) - m_sizeRectangle.cy;
 		break;

	case RAlignSelectionAction::kAlignTop:
		rcRectangle.top = m_rcDisplay.top + (m_rcDisplay.Height() / 7);
		break;

	case RAlignSelectionAction::kAlignCenterVert:
		rcRectangle.top = m_ptStaticCenter.y - ( m_sizeRectangle.cy / 2 );
		break;
	}

	rcRectangle.bottom = rcRectangle.top + m_sizeRectangle.cy;
}


// ****************************************************************************
//
//  Function Name:	RAlignDialog::OnPaint( )
//
//  Description:		Paints the display in the static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RAlignDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	UpdateDisplay();

	// Do not call CDialog ::OnPaint() for painting messages
}
