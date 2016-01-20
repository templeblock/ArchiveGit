//****************************************************************************
//
// File Name:		ScaleDialog.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of RScaleDialog.   
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
//  $Logfile:: /PM8/Framework/Source/ScaleDialog.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ScaleDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Buttonbmps.h"

// #include "Resource.h"
#include "ExceptionHandling.h"

#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int kMinScaleValue = 10;
static const int kMaxScaleValue = 400;

// uncomment the below define to make the slider control range
// sensitive to the min and max permissiable scale ranges for the 
// objects being scaled.
#define USE_SMART_SCALING

/////////////////////////////////////////////////////////////////////////////
// RScaleDialog dialog


// ****************************************************************************
//
//  Function Name:	RScaleDialog::RScaleDialog( )
//
//  Description:		Constructor
//
//  Returns:			N/A
//
//  Exceptions:		None
//
// ****************************************************************************

RScaleDialog::RScaleDialog( RRealSize& minScaleFactor, RRealSize& maxScaleFactor, CWnd* pParent /*=NULL*/) : 
	CDialog(RScaleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(RScaleDialog)
	m_eScaleDirection	= kAllDirections;
	m_iScale				= kDefaultScale;
	//}}AFX_DATA_INIT

	m_minScaleFactor = minScaleFactor;
	m_maxScaleFactor = maxScaleFactor;

	m_eOldScaleDirection = kUndefined;
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::DoDataExchange( )
//
//  Description:		Standard validation routine
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RScaleDialog)
	DDX_Control(	pDX, CONTROL_STATIC_SCALE,						m_staticDisplay	);
	DDX_Control(	pDX, CONTROL_STATIC_SCALE_DOTS,				m_staticDots		);
	DDX_Control(	pDX, CONTROL_SCALE_SPIN,						m_spinScale			);
	DDX_Control(	pDX, CONTROL_SLIDER_SCALE,						m_sliderScale		);
	//}}AFX_DATA_MAP

	try
	{
		// the edit control may contain a comma so have to remove it manually...
		if ( pDX->m_bSaveAndValidate != FALSE )
		{
			CString szText;

			// save into CString
			DDX_Text( pDX, CONTROL_EDIT_SCALE, szText );

			CString szCommaFreeText;
			TCHAR tComma[2];
			
			if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, (LPTSTR) &tComma, 2 ) != 0 )
			{
				TCHAR tItem;
				for (int n = 0; n < szText.GetLength(); n++ )
				{
					tItem = szText.GetAt(n);
					if ( tItem != tComma[0] )
						szCommaFreeText+= tItem;
				}

				m_iScale = atoi( szCommaFreeText );
			}
			else
				m_iScale = m_iPrevPos;
		}
	}
	catch(...)
	{
		m_iScale = m_iPrevPos;
	}

	// validate the scale factor
	if ( pDX->m_bSaveAndValidate != FALSE )
	{	
		pDX->PrepareCtrl( CONTROL_SLIDER_SCALE );

		// validate the ranges
		DDV_MinMaxInt(	pDX, m_iScale, GetMinScale(), GetMaxScale() );
	}
}


BEGIN_MESSAGE_MAP(RScaleDialog, CDialog)
	//{{AFX_MSG_MAP(RScaleDialog)
	ON_BN_CLICKED(				CONTROL_RADIO_SCALE_ALL_DIRECTIONS,		OnRadioScaleAllDirections	)
	ON_BN_CLICKED(				CONTROL_RADIO_SCALE_HORIZONTAL_ONLY,	OnRadioScaleHorizontalOnly	)
	ON_BN_CLICKED(				CONTROL_RADIO_SCALE_VERTICAL_ONLY,		OnRadioScaleVerticalOnly	)
	ON_EN_SETFOCUS(			CONTROL_EDIT_SCALE,							OnSetfocusEditScale			)
	ON_EN_CHANGE(				CONTROL_EDIT_SCALE,							OnChangeEditScale				)
	ON_EN_KILLFOCUS(			CONTROL_EDIT_SCALE,							OnKillfocusEditScale			)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RScaleDialog message handlers

// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnInitDialog( )
//
//  Description:		Performs dialog initialization on creation.
//
//  Returns:			TRUE
//
//  Exceptions:		None
//
// ****************************************************************************

// virtual
BOOL	RScaleDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	MapDotBitmap();
	InitializeOriginalBitmap();
	InitializeScaleControls();
	InitializeRadioControls();

	m_uTimerID =	SetTimer( 
							UINT( ETimerData::kScrollEventID ), 
							UINT( ETimerData::kTimeOutMillisecs ),
							NULL
						);

	return TRUE;
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::MapDotBitmap( )
//
//  Description:		The bitmap that's a series of dots above the slider 
//							control has a background of medium grey.  This maps the
//							background color to the same color as the dialog and loads
//							the bitmap into the static control.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::MapDotBitmap()
{
	COLORMAP ColorMap;
	ColorMap.from	= RGB( 192, 192, 192 );
	ColorMap.to		= ::GetSysColor( COLOR_BTNFACE );

	VERIFY( m_bmpDots.LoadMappedBitmap( BITMAP_SCALE_DOTS, 0, &ColorMap, 1 ) );
	m_staticDots.SetBitmap( HBITMAP( m_bmpDots ) );
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::InitializeScaleControls( )
//
//  Description:		Initializes the range and default value for the spin and 
//							slider controls.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::InitializeScaleControls()
{
#ifdef USE_SMART_SCALING
	int iMin			= GetMinScale();
	int iMax			= GetMaxScale();
	//	Force to a reasonable range.
	if ( iMin < kMinScaleValue )
		iMin = kMinScaleValue;
	if ( iMax > kMaxScaleValue )
		iMax = kMaxScaleValue;
#else
	int iMin			= kMinScaleValue;
	int iMax			= kMaxScaleValue;
#endif //USE_SMART_SCALING

	int iDefault	= int( EScaleRange::kDefaultScale );

	// Initialize the spin and slider controls.  They have the same range and 
	// always initialize to 100%.
	m_spinScale.SetRange( iMin, iMax );
	m_spinScale.SetPos( iDefault );

	m_sliderScale.SetRange( iMin, iMax );
	m_sliderScale.SetPos( iDefault );

	m_iPrevPos = iDefault;

	UpdateScaleControls();
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::UpdateScaleControls()
//
//  Description:		Sets the range for the spin and slider controls.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::UpdateScaleControls()
{
#ifdef USE_SMART_SCALING
	int iMin			= GetMinScale();
	int iMax			= GetMaxScale();
	//	Force to a reasonable range.
	if ( iMin < kMinScaleValue )
		iMin = kMinScaleValue;
	if ( iMax > kMaxScaleValue )
		iMax = kMaxScaleValue;
#else
	int iMin			= kMinScaleValue;
	int iMax			= kMaxScaleValue;
#endif //USE_SMART_SCALING

	int iPos			= m_iPrevPos;

	// Initialize the spin and slider controls.  They have the same range and 
	// always initialize to 100%.
	m_spinScale.SetRange( iMin, iMax );
	m_sliderScale.SetRange( iMin, iMax );

	// ensure the current selection is still valid.
	if ( iPos > iMax )
		iPos = iMax;

	if ( iPos < iMin )
		iPos = iMin;

	// update the slider and the spin control
	m_sliderScale.SetPos( iPos );
	m_sliderScale.ClearSel( TRUE );
	m_spinScale.SetPos( iPos );

	// update the bitmap
	StretchDisplayBitmap( iPos );
}



// ****************************************************************************
//
//  Function Name:	RScaleDialog::InitializeOriginalBitmap( )
//
//  Description:		Initializes the original bitmap which will be displayed
//							in the display static control, and save information which
//							will later be handy when its scale has to be changed.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::InitializeOriginalBitmap()
{
	m_bmpDisplay.LoadBitmap( BITMAP_NO_FRAME );

	BITMAP bitmap;
	m_bmpDisplay.GetBitmap( &bitmap );

	m_sizeOrigBitmap.cx = bitmap.bmWidth;
	m_sizeOrigBitmap.cy = bitmap.bmHeight;

	m_ptBitmapCenter.x = m_sizeOrigBitmap.cx / 2;
	m_ptBitmapCenter.y = m_sizeOrigBitmap.cy / 2;

	m_staticDisplay.GetClientRect( &m_rcStaticDisplay );

	m_sizeStaticDisplay	= m_rcStaticDisplay.Size();
	m_ptStaticCenter		= m_rcStaticDisplay.CenterPoint();
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::InitializeRadioControls( )
//
//  Description:		Sets which radiobutton is checked by the value in
//							m_eScaleDirection. 
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::InitializeRadioControls()
{
	CButton* pbtnDirection;

	switch( m_eScaleDirection )
	{
		case kAllDirections:
			pbtnDirection = ( CButton* )GetDlgItem( CONTROL_RADIO_SCALE_ALL_DIRECTIONS );
			break;
			 
		case kHorizontalOnly:
			pbtnDirection = ( CButton* )GetDlgItem( CONTROL_RADIO_SCALE_HORIZONTAL_ONLY );
			break;

		case kVerticalOnly:
			pbtnDirection = ( CButton* )GetDlgItem( CONTROL_RADIO_SCALE_VERTICAL_ONLY );
			break;

		default:
			TpsAssertAlways( "Invalid value for m_eScaleDirection" );
			break;
	}

	pbtnDirection->SetCheck( 1 );
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnRadioScaleAllDirections( )
//
//  Description:		Sets the enumerated scale value to kAllDirections.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnRadioScaleAllDirections() 
{
	m_eScaleDirection = kAllDirections;
	
	UpdateScaleControls();
	StretchDisplayBitmap( m_sliderScale.GetPos() );
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnRadioScaleHorizontalOnly( )
//
//  Description:		Sets the enumerated scale value to kHorizontalOnly.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnRadioScaleHorizontalOnly() 
{
	m_eScaleDirection = kHorizontalOnly;

	UpdateScaleControls();
	StretchDisplayBitmap( m_sliderScale.GetPos() );
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnRadioScaleVerticalOnly( )
//
//  Description:		Sets the enumerated scale value to kVerticalOnly.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnRadioScaleVerticalOnly() 
{
	m_eScaleDirection = kVerticalOnly;

	UpdateScaleControls();
	StretchDisplayBitmap( m_sliderScale.GetPos() );
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnSetfocusEditScale( )
//
//  Description:		Starts the timer for updating the static control display
//							to the value in the edit control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnSetfocusEditScale() 
{
	m_uEditTimerID =	SetTimer( 
								UINT( ETimerData::kEditEventID ), 
								UINT( ETimerData::kTimeOutMillisecs ),
								NULL
							);
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnChangeEditScale( )
//
//  Description:		Sets the slider to the value in the buddy window of the
//							spin control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnChangeEditScale() 
{
	if ( ::IsWindow( m_spinScale.GetSafeHwnd() ) && m_spinScale.IsWindowVisible() )
	{
		int iPos = m_spinScale.GetPos();
		m_sliderScale.SetPos( iPos );
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnKillfocusEditScale( )
//
//  Description:		Stops the timer for updating the static control display
//							to the value in the edit control, and performs a final
//							update.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnKillfocusEditScale() 
{
	KillTimer( UINT( kEditEventID ) );	

	StretchDisplayBitmap( m_spinScale.GetPos() );
}

// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnVScroll( )
//
//  Description:		This function handles when the user clicks the spin control.
//							It synchronizes the slider control with the value in the 
//							spin control and updates the static control with the new 
//							scale factor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	UNREFERENCED_PARAMETER( nSBCode );
	UNREFERENCED_PARAMETER( nPos );

	if ( pScrollBar == ( CScrollBar* )&m_spinScale )
	{
		int iPos = m_spinScale.GetPos();
		m_sliderScale.SetPos( iPos );

		StretchDisplayBitmap( iPos );
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnHScroll( )
//
//  Description:		This is a handler for the when the user manipulates the
//							thumb on the slider control.  It synchronizes the spin 
//							control with the value in the slider control
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	UNREFERENCED_PARAMETER( nSBCode );
	UNREFERENCED_PARAMETER( nPos );

	if ( pScrollBar == ( CScrollBar* )&m_sliderScale )
	{
		// You don't want to use the position in nPos because it may not be valid
		// if nSBCode is SB_ENDSCROLL, in which case you'll lose your image when 
		// the mouse goes up.
		int iPos = m_sliderScale.GetPos();
		m_spinScale.SetPos( iPos );
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::StretchDisplayBitmap( )
//
//  Description:		Updates the image displayed to the scaled dimensions.
//
//  Returns:			Nothing
//
//  Exceptions:		CResourceException
//
// ****************************************************************************

void RScaleDialog::StretchDisplayBitmap( int iPos )
{
	if ( ( iPos == m_iPrevPos ) && ( m_eOldScaleDirection == m_eScaleDirection ) )
		return;

	m_iPrevPos = iPos;

	CRect	rcDestination;
	CRect rcSource;

	// The position will be a percentage, so multiply it by 100 to derive the
	// scale.
	DeriveSourceAndDestinationRects( float( iPos ) / 100.F, rcSource, rcDestination );

	// Because of how fast this image can update (because of the slider and spin
	// controls), it causes flicker to erase the static control before you blit.
	// Avoid having residual images show by blitting indirectly onto an offscreen 
	// bitmap, and then blitting the offscreen to the static control.
	CDC		bitmapDC;
	CDC		offscreenDC;

	CBitmap* pOldBitmap = NULL;
	CBitmap* pOldOffscreenBitmap = NULL;

	try 
	{
		CClientDC	clientDC( &m_staticDisplay );

		// Select the sample bitmap into the bitmap DC
		if ( !bitmapDC.CreateCompatibleDC( &clientDC ) )
		{
			TpsAssertAlways( "bitmapDC.CreateCompatibleDC( &clientDC ) failed" );
			ThrowException( kResource );
		}

		CBitmap*	pOldBitmap = ( CBitmap* )bitmapDC.SelectObject( &m_bmpDisplay );
		if ( pOldBitmap == NULL )
		{
//			TpsAssertAlways( "bitmapDC.SelectObject( &m_bmpDisplay ) failed" );
			ThrowException( kResource );
		}

		// Create an offscreen bitmap the size of the static control
		CBitmap		offscreenBitmap;
		if ( !offscreenBitmap.CreateCompatibleBitmap( 
					&clientDC, 
					m_sizeStaticDisplay.cx, 
					m_sizeStaticDisplay.cy ) )
		{
			TpsAssertAlways( "CreateCompatibleBitmap() failed" );
			ThrowException( kResource );
		}

		// Select the offscreen bitmap into the offscreen DC
		offscreenDC.CreateCompatibleDC( &clientDC );
		CBitmap* pOldOffscreenBitmap = ( CBitmap* )offscreenDC.SelectObject( &offscreenBitmap );
		if ( pOldOffscreenBitmap == NULL )
		{
//			TpsAssertAlways( "offscreenDC.SelectObject( &offscreenBitmap ) failed" );
			ThrowException( kResource );
		}

		// Paint the offscreen bitmap white
		if ( !offscreenDC.PatBlt( 0, 0, m_sizeStaticDisplay.cx, m_sizeStaticDisplay.cy, WHITENESS ) )
		{
//			TpsAssertAlways( "PatBlt() failed" );
			ThrowException( kResource );
		}
		
		bitmapDC.SetStretchBltMode( COLORONCOLOR );
		offscreenDC.SetStretchBltMode( COLORONCOLOR );

		// Blit the scaled sample bitmap onto the white offscreen bitmap
		if ( !offscreenDC.StretchBlt(
				rcDestination.left,		// destination upper-left 
				rcDestination.top,		// destination upper-left 
				rcDestination.Width(),	// destination rectangle width
				rcDestination.Height(),	// destination rectangle height
				&bitmapDC,
				rcSource.left,				// source upper-left
				rcSource.top,				// source upper-left
				rcSource.Width(),			// source rectangle width
				rcSource.Height(),		// source rectangle height
				SRCCOPY						// raster operation
			) )
		{
			// Try blitting at up to +/- 5% of iPos
			if ( !HackStretchBltApproximation( &offscreenDC, &bitmapDC, iPos ) )
				ThrowException( kResource );
		}

		// Blit the offscreen bitmap onto the static control
		clientDC.SetStretchBltMode( COLORONCOLOR );
		if ( !clientDC.StretchBlt(
				0, 
				0, 
				m_sizeStaticDisplay.cx, 
				m_sizeStaticDisplay.cy, 
				&offscreenDC,
				0,
				0,
				m_sizeStaticDisplay.cx, 
				m_sizeStaticDisplay.cy,
				SRCCOPY
			) )
		{
//			TpsAssertAlways( "Offscreen bitmap not blitted to static control" );
			ThrowException( kResource );
		}
				
		// Clean up
		offscreenDC.SelectObject( pOldOffscreenBitmap );
		bitmapDC.SelectObject( pOldBitmap );
	}
	catch( ... )
	{
		if ( pOldOffscreenBitmap ) 
			offscreenDC.SelectObject( pOldOffscreenBitmap );

		if ( pOldBitmap ) 
			bitmapDC.SelectObject( pOldBitmap );

//		CString strPercent;
//		strPercent.Format( "%d", iPos );
//		CString strMsg;
//		::AfxFormatString1( strMsg, STRING_ERROR_STRETCHBLT, strPercent );
//		::AfxMessageBox( strMsg );
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::HackStretchBltApproximation( )
//
//  Description:		Call this when StretchBlt() fails for the source and
//							destination rectangle sizes at iPos%.  It attempts
//							to find a percentage +/- 5% of iPos where StretchBlt()
//							works.
//
//  Returns:			TRUE:		StretchBlt() worked at some percentage and the
//										destination DC has been modified.
//							FALSE:	StretchBlt() failed at all percentages tried.
//
//  Exceptions:		None
//
// ****************************************************************************

BOOL RScaleDialog::HackStretchBltApproximation( CDC* pDestDC, CDC* pSourceDC, int iPos )
{
	CRect	rcSource;
	CRect rcDestination;
	int	i;

	for ( i = 1; i <= 5; i++ )
	{
		DeriveSourceAndDestinationRects( float( iPos + i ) / 100.F, rcSource, rcDestination );
		
		if ( pDestDC->StretchBlt(
				rcDestination.left,		// destination upper-left 
				rcDestination.top,		// destination upper-left 
				rcDestination.Width(),	// destination rectangle width
				rcDestination.Height(),	// destination rectangle height
				pSourceDC,
				rcSource.left,				// source upper-left
				rcSource.top,				// source upper-left
				rcSource.Width(),			// source rectangle width
				rcSource.Height(),		// source rectangle height
				SRCCOPY						// raster operation
			) )
			return TRUE;
	}

	for (	i = 1; i <= 5; i++ )
	{
		DeriveSourceAndDestinationRects( float( iPos - i ) / 100.F, rcSource, rcDestination );
		
		if ( pDestDC->StretchBlt(
				rcDestination.left,		// destination upper-left 
				rcDestination.top,		// destination upper-left 
				rcDestination.Width(),	// destination rectangle width
				rcDestination.Height(),	// destination rectangle height
				pSourceDC,
				rcSource.left,				// source upper-left
				rcSource.top,				// source upper-left
				rcSource.Width(),			// source rectangle width
				rcSource.Height(),		// source rectangle height
				SRCCOPY						// raster operation
			) )
			return TRUE;
	}

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::DeriveSourceAndDestinationRects( )
//
//  Description:		Calculates the dimensions and locations of the scaled 
//							source and destination rectangles.  
//
//							If the scaled bitmap dimensions are too large to fit 
//							within the static control, the source bitmap rectangle is 
//							cropped to return only the part of the scaled bitmap that 
//							will fit within the static control, and the destination 
//							rectangle is cropped so that it won't contain the entire 
//							image, but will fit within the static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::DeriveSourceAndDestinationRects( 
			float		flScale,
			CRect&	rcSource,
			CRect&	rcDestination
		)
{
	GetScaledRect( flScale, rcDestination );

	// If the scaled bitmap dimensions extend beyond the static control window,
	// only blit the area within the original bitmap that will be contained in the
	// window.  Here we do the calculations to derive the dimensions of the 
	// destination and source rectangles.
	BOOL fCropped = FALSE;

	float flHorzCropFactor = 1.F;
	float flVertCropFactor = 1.F;

	if ( rcDestination.Width() > m_sizeStaticDisplay.cx )
	{
		int	iWidth = rcDestination.Width();
		flHorzCropFactor = 1.F - float( iWidth - m_sizeStaticDisplay.cx ) / float( iWidth );
		CropRectangle( flHorzCropFactor, 1.F, rcDestination );

		fCropped = TRUE;
	}

	if ( rcDestination.Height() > m_sizeStaticDisplay.cy )
	{
		int	iHeight	= rcDestination.Height();
		flVertCropFactor	= 1.F - float( iHeight - m_sizeStaticDisplay.cy ) / float( iHeight );
		CropRectangle( 1.F, flVertCropFactor, rcDestination );

		fCropped = TRUE;
	}

	int	iHalfWidth	= int( float( rcDestination.Width()  ) / 2.F );
	int	iHalfHeight	= int( float( rcDestination.Height() ) / 2.F );

	rcDestination.OffsetRect(
		m_ptStaticCenter.x - iHalfWidth,
		m_ptStaticCenter.y - iHalfHeight
	);

	rcSource.left		= 0;
	rcSource.top		= 0;
	rcSource.right		= m_sizeOrigBitmap.cx;
	rcSource.bottom	= m_sizeOrigBitmap.cy;

	if ( fCropped )
	{
		CropRectangle( flHorzCropFactor, flVertCropFactor, rcSource );
		switch ( m_eScaleDirection )
		{
			case kAllDirections:
				rcSource.OffsetRect( 
					m_ptBitmapCenter.x - int( float( iHalfWidth )  / flScale ),	
					m_ptBitmapCenter.y - int( float( iHalfHeight ) / flScale )
				);
				break;

			case kHorizontalOnly:
				rcSource.OffsetRect( 
					m_ptBitmapCenter.x - int( float( iHalfWidth )  / flScale ),	
					m_ptBitmapCenter.y - iHalfHeight
				);
				break;

			case kVerticalOnly:
				rcSource.OffsetRect( 
					m_ptBitmapCenter.x - iHalfWidth,	
					m_ptBitmapCenter.y - int( float( iHalfHeight ) / flScale )
				);
				break;
		}
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::GetScaledRect( )
//
//  Description:		Returns (by reference) the size of the rectangle derived 
//							from the size of the original bitmap scaled in the proper 
//							directions.  This rectangle is uncropped and could easily 
//							be larger than the display static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::GetScaledRect( float flScale, CRect& rc )
{
	rc.left	= 0;
	rc.top	= 0;

	switch ( m_eScaleDirection )
	{
		case kAllDirections:
			rc.right		= int( float( m_sizeOrigBitmap.cx ) * flScale );
			rc.bottom	= int( float( m_sizeOrigBitmap.cy ) * flScale );
			break;

		case kHorizontalOnly:
			rc.right		= int( float( m_sizeOrigBitmap.cx ) * flScale );
			rc.bottom	= m_sizeOrigBitmap.cy;
			break;

		case kVerticalOnly:
			rc.right		= m_sizeOrigBitmap.cx;
			rc.bottom	= int( float( m_sizeOrigBitmap.cy ) * flScale );
			break;
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::CropRectangle( )
//
//  Description:		Crops the rectangle by the crop factor in the directions
//							specified by m_eScaleDirection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RScaleDialog::CropRectangle( 
			float flHorzCropFactor, 
			float flVertCropFactor, 
			CRect& rc )
{
	switch ( m_eScaleDirection )
	{
		case kAllDirections:
			rc.right		= int( float( rc.right  ) * flHorzCropFactor );
			rc.bottom	= int( float( rc.bottom ) * flVertCropFactor );
			break;

		case kHorizontalOnly:
			rc.right		= int( float( rc.right  ) * flHorzCropFactor );
			break;

		case kVerticalOnly:
			rc.bottom	= int( float( rc.bottom ) * flVertCropFactor );
			break;
	}
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnTimer( )
//
//  Description:		Handles WM_TIMER messages.  This function exists to space
//							out the number of times the static bitmap is updated when
//							the slider or spin move or the edit control changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnTimer( UINT nIDEvent ) 
{
	if (	( nIDEvent == m_uTimerID ) ||
			( nIDEvent == m_uEditTimerID ) )
	{
		int iPos = m_sliderScale.GetPos();
		if ( iPos != m_iPrevPos )
			StretchDisplayBitmap( iPos );
	}
	
	CDialog::OnTimer( nIDEvent );
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnDestroy( )
//
//  Description:		Override kills the timer after the window is destroyed.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	
	KillTimer( UINT( kScrollEventID	) );	
	KillTimer( UINT( kEditEventID		) );	
}

// ****************************************************************************
//
//  Function Name:	RScaleDialog::OnPaint( )
//
//  Description:		Override updates the image in the static control when a
//							WM_PAINT message comes through.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::OnPaint() 
{
	CPaintDC dc( this ); // device context for painting
	
	StretchDisplayBitmap( m_sliderScale.GetPos() );
	
	// Do not call CDialog::OnPaint() for painting messages
}

// ****************************************************************************
//
//  Function Name:	RScaleDialog::GetScaleInfo( )
//
//  Description:		Retrieves the scale information necessary to actually do
//							the scale.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RScaleDialog::GetScaleInfo( RRealSize& scaleFactor, BOOLEAN& fAspectCorrect ) const
	{
	// Initialize the out parameters
	scaleFactor.m_dx = scaleFactor.m_dy = 1;
	fAspectCorrect = FALSE;

	// Convert the scale member to a Renaissance scale factor
	YScaleFactor scale = m_iScale / 100.0;

	// Fill in the parameters based on the scale direction and scale factor
	switch( m_eScaleDirection )
		{
		case kAllDirections :
			scaleFactor.m_dx = scaleFactor.m_dy = scale;
			fAspectCorrect = TRUE;
			break;

		case kHorizontalOnly :
			scaleFactor.m_dx = scale;
			break;

		case kVerticalOnly :
			scaleFactor.m_dy = scale;
			break;

		default :
			TpsAssertAlways( "Dont know how to handle this scale value." );
		}
	}



// ****************************************************************************
//
//  Function Name:	RScaleDialog::GetMinScale()
//
//  Description:		Returns the Minimun scale permissibale based on the
//							scale mode selected by the user.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

int RScaleDialog::GetMinScale()
{
	double nScale = 1.0;

	switch( m_eScaleDirection )
	{
		case kHorizontalOnly:
			nScale = m_minScaleFactor.m_dx;
			break;
		case kVerticalOnly:
			nScale = m_minScaleFactor.m_dy;
			break;
		case kAllDirections:
			if ( m_minScaleFactor.m_dy > m_minScaleFactor.m_dx )
				nScale = m_minScaleFactor.m_dy;
			else
				nScale = m_minScaleFactor.m_dx;
			break;
		default:
			TpsAssertAlways( "No scale mode set." );
			break;
	}

	return (int) (100.0 * nScale);
}


// ****************************************************************************
//
//  Function Name:	RScaleDialog::GetMaxScale()
//
//  Description:		Returns the Minimun scale permissibale based on the
//							scale mode selected by the user.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

int RScaleDialog::GetMaxScale()
{
	double nScale = 1.0;

	switch( m_eScaleDirection )
	{
		case kHorizontalOnly:
			nScale = m_maxScaleFactor.m_dx;
			break;
		case kVerticalOnly:
			nScale = m_maxScaleFactor.m_dy;
			break;
		case kAllDirections:
			if ( m_maxScaleFactor.m_dy < m_maxScaleFactor.m_dx )
				nScale = m_maxScaleFactor.m_dy;
			else
				nScale = m_maxScaleFactor.m_dx;
			break;
		default:
			TpsAssertAlways( "No scale mode set." );
			break;
	}

	return (int) (100.0 * nScale);
}
