//****************************************************************************
//
// File Name:   WinAngleCtrl.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Implementation of RWinAngleCtrl object
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WinAngleCtrl.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "WinAngleCtrl.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "OffscreenDrawingSurface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const YPlatformColor kArrowColor  = RGB( 0, 0, 128 ) ;
const YAngle			kFirstEdge   = -kPI * 0.875 ;
const YAngle         kSecondEdge  = -kPI * 0.125 ;
const YAngle			kThirdEdge   = -kPI * 0.375 ;
const YAngle			kFourthEdge  = -kPI * 0.625 ;

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::RWinAngleCtrl
//
// Description:    Constructor (Default).
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
RWinAngleCtrl::RWinAngleCtrl( BOOLEAN fSnapToGuides )
{
	m_fSnapToGuides  = fSnapToGuides;
	m_fMouseCaptured = FALSE ;
	m_flAngle        = 0.0 ;
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::RWinAngleCtrl
//
// Description:    Destructor
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
RWinAngleCtrl::~RWinAngleCtrl()
{
}


BEGIN_MESSAGE_MAP(RWinAngleCtrl, CWnd)
	//{{AFX_MSG_MAP(RWinAngleCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnPaint
//
// Description:    WM_PAINT message handler
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::OnPaint() 
{
	CWnd::OnPaint() ;

	CClientDC dc(this); // device context for painting

	RDcDrawingSurface ds ;
	ds.Initialize( (HDC) dc ) ;

//	m_biImage.Render( ds, m_rcDestRect ) ;

	RColor crOldFill = ds.GetFillColor() ;
	RColor crOldPen  = ds.GetPenColor() ;

	RIntPoint* ptArray = static_cast<RIntPoint*>( m_pointCollection ) ;

	RSolidColor crHilight = GetSysColor( COLOR_BTNHILIGHT ) ;
	RSolidColor crShadow  = GetSysColor( COLOR_BTNSHADOW ) ;
	RSolidColor crFill    = RSolidColor( kArrowColor ) ; 

	if (!IsWindowEnabled())
	{
		crFill    = crShadow ;
		crShadow  = GetSysColor( COLOR_BTNHILIGHT ) ;
		crHilight = GetSysColor( COLOR_BTNSHADOW ) ;
	}

	ds.SetFillColor( crFill ) ;
	ds.FillPolygon( ptArray, m_pointCollection.Count() ) ;

	// Draw the hilights and shadows.  Note, A little 
	// fudging is used for determining the hilite, 
	// because the angle on the arrow is not 45 degrees.
	////////////////////////////////////////////////////

	//
	// Handle first edge
	//
	if (m_flAngle > kFirstEdge  && m_flAngle < kFirstEdge + kPI)
	{
		ds.SetPenColor( crHilight ) ;
	}
	else 
	{
		ds.SetPenColor( crShadow ) ;
	}

	ds.MoveTo( ptArray[0] ) ;
	ds.LineTo( ptArray[1] ) ;

	//
	// Handle the third edge
	//
	if (m_flAngle > kSecondEdge && m_flAngle < kSecondEdge + kPI)
	{
		ds.SetPenColor( crHilight ) ;
	}
	else 
	{
		ds.SetPenColor( crShadow ) ;
	}

	ds.LineTo( ptArray[2] ) ;

	//
	// Handle third edge.  Note, due to angles going from
	// -PI to PI, with the hilite for this edge starting
	// positive.  It is necessary to use a negative
	// start edge constant to determine the shadow instead
	// of the hilight.
	//
	if (m_flAngle > kThirdEdge  && m_flAngle < kThirdEdge + kPI)
	{
		ds.SetPenColor( crShadow ) ;
	}
	else 
	{
		ds.SetPenColor( crHilight ) ;
	}

	ds.LineTo( ptArray[3] ) ;

	//
	// Handle the fourth edge
	//
	if (m_flAngle > kFourthEdge  && m_flAngle < kFourthEdge + kPI)
	{
		ds.SetPenColor( crHilight ) ;
	}
	else 
	{
		ds.SetPenColor( crShadow ) ;
	}

	ds.LineTo( ptArray[0] ) ;

	//
	// Restore the original settings
	//
	ds.SetFillColor( crOldFill ) ;
	ds.SetPenColor( crOldPen ) ;
	ds.DetachDCs() ;
}

//void RWinAngleCtrl::PreSubclassWindow() 
//{
//	Initialize() ;
//	UpdateBitmap() ;
//
//	CWnd::PreSubclassWindow();
//}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::GetAngle
//
// Description:    Determines the currently displayed angle
//
// Returns:		    The current angle in PI to -PI radians
//
// Exceptions:	    None
//
//*****************************************************************************
YAngle RWinAngleCtrl::GetAngle( )
{
	return m_flAngle ;
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::SetAngle
//
// Description:    Sets the indicator to the specified angle
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::SetAngle( YAngle angle )
{
	// Normalize the angle to -PI to PI ;
	if (angle > kPI || angle < -kPI)
	{
		angle = atan2( sin(angle), cos( angle ) ) ;
	}

	if (m_fSnapToGuides)
	{
		YAngle flRound = ( angle < 0.0 ? -0.5 : 0.5 ) ;
		int    nAngle  = (int)( angle / (kPI / 4) + flRound ) ;

		// Recompute the new angle
		angle = nAngle * (kPI / 4) ;
	}

	m_flAngle = angle ; 

	// Set up the data points for the angle specifier
	m_pointCollection.Empty() ;
	m_pointCollection.InsertAtEnd( RIntPoint( -3,   0 ) ) ;
	m_pointCollection.InsertAtEnd( RIntPoint( -7, -10 ) ) ;
	m_pointCollection.InsertAtEnd( RIntPoint( 12,   0 ) ) ;
	m_pointCollection.InsertAtEnd( RIntPoint( -7,  10 ) ) ;

	//
	// Scale, rotate and translate appropriately
	//
	RIntPoint    ptCenter = m_rcDestRect.GetCenterPoint() ;
	R2dTransform transform ;

	transform.PostScale( m_rcDestRect.Width() / 69.0, m_rcDestRect.Height() / 72.0 ) ;
	transform.PostRotateAboutOrigin( m_flAngle ) ;
	transform.PostTranslate( ptCenter.m_x, ptCenter.m_y ) ;

	YPointIterator	iterator	= m_pointCollection.Start( );
	YPointIterator iterEnd	= m_pointCollection.End( );

	for ( ; iterator != iterEnd; ++iterator )
	{
		(*iterator) *= transform;
	}

	if (m_hWnd)
	{
		Invalidate( TRUE ) ;
	}
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::Initialize
//
// Description:    Initializes the background bitmap, as well as
//						 sets the angle to the currently specified 
//                 value.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
BOOLEAN RWinAngleCtrl::Initialize()
{
	TpsAssert( m_hWnd, "Control must first be created!" ) ;

//	RDcDrawingSurface ds ;
	BOOLEAN fRetVal = TRUE ;

	try
	{

//		CClientDC dc(this) ;
//		ds.Initialize( (HDC) dc ) ;

		CRect rect ;
		GetClientRect( rect ) ;

		// Determine bitmap coordinates
		m_rcDestRect.m_Top    = 0 ;
		m_rcDestRect.m_Left   = 0 ;
		m_rcDestRect.m_Right  = rect.Width() - 1 ;
		m_rcDestRect.m_Bottom = m_rcDestRect.m_Right ;

		// Initialize bitmap
//		m_biImage.Initialize( 
//			m_rcDestRect.m_Right, 
//			m_rcDestRect.m_Bottom,
//			GetScreenColorDepth() ) ;

		// Center the destination rect ;
		m_rcDestRect.Offset( RIntSize( 
			(rect.Width() / 2)  - (m_rcDestRect.Width() / 2),  
			(rect.Height() / 2) - (m_rcDestRect.Height() / 2) ) ) ;

		SetAngle( m_flAngle ) ;
//		UpdateBitmap() ;
	}
	catch( YException )
	{
		fRetVal = FALSE ;
	}

//	ds.DetachDCs( );

	return fRetVal ;
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnEraseBkgnd
//
// Description:    WM_ERASEBKGND message handler which clears the background
//                 and draws the tick marks
//
// Returns:		    TRUE
//
// Exceptions:	    None
//
//*****************************************************************************
BOOL RWinAngleCtrl::OnEraseBkgnd( CDC *pDC )
{
	BOOL fResult = CWnd::OnEraseBkgnd( pDC ) ;

	// Get the rectangle
	RRealRect rectSrc( m_rcDestRect ) ;

	RDcDrawingSurface ds ;
	ds.Initialize( (HDC) *pDC ) ;
	RColor crOldPen  = ds.GetPenColor() ;
	RColor crOldFill = ds.GetFillColor() ;

	ds.SetPenColor( RSolidColor( kBlack ) ) ;

	if (!IsWindowEnabled())
	{
		ds.SetPenColor( RSolidColor( GetSysColor( COLOR_BTNSHADOW ) ) ) ;
	}

	ds.SetFillColor( RSolidColor( GetSysColor( COLOR_BTNFACE ) ) ) ;
	ds.FillRectangle( rectSrc ) ;

	// Do the drawing?

	RRealPoint pt1( 0, 19 ) ;
	RRealPoint pt2( 0, 28 ) ;
	RRealPoint ptCenter = rectSrc.GetCenterPoint() ;

	R2dTransform transform ;
	transform.PostScale( rectSrc.Width() / 69.0, rectSrc.Height() / 72.0 ) ;
	transform.PostTranslate( ptCenter.m_x, ptCenter.m_y ) ;

	for (int i = 0; i < 8; i++)
	{
		pt1 *= transform ;
		pt2 *= transform ;

		ds.MoveTo( pt1 ) ;
		ds.LineTo( pt2 ) ;

		transform.MakeIdentity() ;
		transform.PostRotateAboutPoint( ptCenter.m_x, 
			ptCenter.m_y, kPI / 4 ) ;
	}

	ds.SetPenColor( crOldPen ) ;
	ds.SetFillColor( crOldFill ) ;
	ds.DetachDCs() ;

	return fResult ;
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::UpdateBitmap
//
// Description:    Updates the background bitmap.  This is only
//                 necessary when the control is created, or when
//                 the system button color changes.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
/*void RWinAngleCtrl::UpdateBitmap()
{
	TpsAssert( m_hWnd, "Control must first be created!" ) ;

	ROffscreenDrawingSurface dsMem ; 
	dsMem.SetImage( &m_biImage ) ;

	// Get the bitmap rectangle
	RRealRect rectSrc(0, 0, 
		m_biImage.GetWidthInPixels(),
		m_biImage.GetHeightInPixels()) ;

	RColor crOldPen  = dsMem.GetPenColor() ;
	RColor crOldFill = dsMem.GetFillColor() ;

	dsMem.SetPenColor( RSolidColor( kBlack ) ) ;

	if (!IsWindowEnabled())
	{
		dsMem.SetPenColor( RSolidColor( GetSysColor( COLOR_BTNFACE ) ) ) ;
	}

	dsMem.SetFillColor( RSolidColor( GetSysColor( COLOR_BTNFACE ) ) ) ;
	dsMem.FillRectangle( rectSrc ) ;

	// Do the drawing?

	RRealPoint pt1( 0, 19 ) ;
	RRealPoint pt2( 0, 28 ) ;
	RRealPoint ptCenter = rectSrc.GetCenterPoint() ;

	R2dTransform transform ;
	transform.PostScale( rectSrc.Width() / 69.0, rectSrc.Height() / 72.0 ) ;
	transform.PostTranslate( ptCenter.m_x, ptCenter.m_y ) ;

	for (int i = 0; i < 8; i++)
	{
		pt1 *= transform ;
		pt2 *= transform ;

		dsMem.MoveTo( pt1 ) ;
		dsMem.LineTo( pt2 ) ;

		transform.MakeIdentity() ;
		transform.PostRotateAboutPoint( ptCenter.m_x, 
			ptCenter.m_y, kPI / 4 ) ;
	}

	dsMem.SetPenColor( crOldPen ) ;
	dsMem.SetFillColor( crOldFill ) ;
	dsMem.ReleaseImage() ;
}*/

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::ComputeAngle
//
// Description:    Comuptes and sets the angle from a given poit.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::ComputeAngle( CPoint point )
{
	// Normalize the mouse point to our center 
	// point origin, and then compute the angle.
	RIntPoint pt = m_rcDestRect.GetCenterPoint() ;
	YAngle angle = atan2( YRealDimension(pt.m_y - point.y) , YRealDimension(point.x - pt.m_x) ) ;

	if (m_fSnapToGuides)
	{
		YAngle flRound = ( angle < 0.0 ? -0.5 : 0.5 ) ;
		int    nAngle  = (int)( angle / (kPI / 4) + flRound ) ;

		// Recompute the new angle
		angle = nAngle * (kPI / 4) ;
	}

	if (angle != m_flAngle)
	{
		SetAngle( angle ) ;
	}
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnSize
//
// Description:    WM_SIZE handler.  (Re)Initializes the background
//                 bitmap
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	Initialize() ;
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnLButtonDown
//
// Description:    WM_LBUTTONDOWN message handler.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Store the old angle, as we will 
	// reset with this anytime they
	// go out of our client area.
	m_flOldAngle     = m_flAngle ;
	m_fMouseCaptured = TRUE ;

	SetCapture() ;
	ComputeAngle( point ) ;

	CWnd::OnLButtonDown(nFlags, point);
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnLButtonUp
//
// Description:    WM_LBUTTONUP message handler.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture() ;
	m_fMouseCaptured = FALSE ;

	// Send notification to parent that the angle changed
	GetParent()->PostMessage( UM_ANGLE_CHANGE ) ;

	CWnd::OnLButtonUp(nFlags, point);
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnMouseMove
//
// Description:    WM_MOUSEMOVE message handler.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_fMouseCaptured)
	{
		ComputeAngle( point ) ;
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

//*****************************************************************************
//
// Function Name:  RWinAngleCtrl::OnEnable
//
// Description:    WM_ENABLE message handler.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
void RWinAngleCtrl::OnEnable( BOOL fEnable )
{
	CWnd::OnEnable( fEnable ) ;
	Invalidate( TRUE ) ;	
}
