//****************************************************************************
//
// File Name:		FramesDialog.cpp
//
// Project:			Renaissance Text Component
//
// Author:			Mike Heydlauf, 
//						Shelah Horvitz
//
// Description:	Definition of RFramesDialog.   
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
//  $Logfile:: /PM8/Framework/Source/FramesDialog.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "FramesDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "WinColorDlg.h"
#include "AutoDrawingSurface.h"
#include "ResourceManager.h"
#include "ApplicationGlobals.h"

#include "Buttonbmps.h"

// #include "Resource.h"

#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

/////////////////////////////////////////////////////////////////////////////
// RFramesDialog dialog

// ****************************************************************************
//
//  Function Name:	RFramesDialog::RFramesDialog( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

RFramesDialog::RFramesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(RFramesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(RFramesDialog)
	//}}AFX_DATA_INIT

	m_bMultipleFrameTypes = FALSE;
	m_eFrameType = kNoFrame;
}

// ****************************************************************************
//
//  Function Name:	RFramesDialog::DoDataExchange
//
//  Description:		Effects data exchange between the controls and the values
//							associated with them.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RFramesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RFramesDialog)
	DDX_Control( pDX, CONTROL_BUTTON_THIN_FRAME,				m_btnThinFrame				);
	DDX_Control( pDX, CONTROL_BUTTON_MEDIUM_FRAME,			m_btnMediumFrame			);
	DDX_Control( pDX, CONTROL_BUTTON_THICK_FRAME,			m_btnThickFrame			);
	
	DDX_Control( pDX, CONTROL_BUTTON_DBL_THIN_FRAME,		m_btnDblThinFrame			);
	DDX_Control( pDX, CONTROL_BUTTON_DBL_MEDIUM_FRAME,		m_btnDblMediumFrame		);
	DDX_Control( pDX, CONTROL_BUTTON_DBL_THICK_FRAME,		m_btnDblThickFrame		);
	
	DDX_Control( pDX, CONTROL_BUTTON_ROUND_CORNER_FRAME,	m_btnRoundCornerFrame	);
	DDX_Control( pDX, CONTROL_BUTTON_PICTURE_FRAME,			m_btnPictureFrame			);
	DDX_Control( pDX, CONTROL_BUTTON_DROP_SHADOW_FRAME,	m_btnDropShadowFrame		);

	DDX_Control( pDX, CONTROL_STATIC_FRAME_DISPLAY,			m_ctrlFrameDisplay		);

	DDX_Control( pDX, CONTROL_BUTTON_COLOR_FRAME,			m_ColorBtn					);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RFramesDialog, CDialog)
	//{{AFX_MSG_MAP(RFramesDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(CONTROL_BUTTON_THIN_FRAME,				OnButtonThinFrame			)
	ON_BN_CLICKED(CONTROL_BUTTON_MEDIUM_FRAME,			OnButtonMediumFrame		)
	ON_BN_CLICKED(CONTROL_BUTTON_THICK_FRAME,				OnButtonThickFrame		)

	ON_BN_CLICKED(CONTROL_BUTTON_DBL_THIN_FRAME,			OnButtonDblThinFrame		)
	ON_BN_CLICKED(CONTROL_BUTTON_DBL_MEDIUM_FRAME,		OnButtonDblMediumFrame	)
	ON_BN_CLICKED(CONTROL_BUTTON_DBL_THICK_FRAME,		OnButtonDblThickFrame	)

	ON_BN_CLICKED(CONTROL_BUTTON_ROUND_CORNER_FRAME,	OnButtonRoundCornerFrame)
	ON_BN_CLICKED(CONTROL_BUTTON_PICTURE_FRAME,			OnButtonPictureFrame		)
	ON_BN_CLICKED(CONTROL_BUTTON_DROP_SHADOW_FRAME,		OnButtonDropShadowFrame	)
	ON_BN_CLICKED(CONTROL_BUTTON_REMOVE_FRAME,			OnButtonRemoveFrame		)

	ON_BN_CLICKED(CONTROL_BUTTON_COLOR_FRAME,				OnButtonSetFrameColor	)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// ****************************************************************************
//
//  Function Name:	RFramesDialog::RenderPreview( )
//
//  Description:		Paints the bitmap with its frame on the preview
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RFramesDialog::RenderPreview()
{
	RFrame* pFrame = NULL;

	try
	{
		//
		// Get the preview's drawing surface
		CClientDC				clientDC( &m_ctrlFrameDisplay );
		HDC						hClientDC = clientDC.GetSafeHdc();
		RDcDrawingSurface		drawSurface;
		drawSurface.Initialize( hClientDC, hClientDC );
		//
		// Get an identity transform
		R2dTransform			transform;
		//
		// Prepare the image on an offscreen DC
		RAutoDrawingSurface	drawSurfaceOffscreen;
		RRealRect				rRealStaticRect(m_rRealRectPreview);
		BOOLEAN					fPrepared	= drawSurfaceOffscreen.Prepare( &drawSurface, transform, m_rRealRectPreview );
		RDrawingSurface*		pSurface		= ((fPrepared)? &drawSurfaceOffscreen : &drawSurface );
		//
		// Paint the offscreen DC white
		RColor rColor = RSolidColor( kWhite );
		pSurface->SetFillColor( rColor );
		pSurface->SetPenColor( rColor );
		pSurface->FillRectangle( m_rRealRectPreview );
		//
		// Scale the transform to the device DPI
		transform.PreScale( YScaleFactor(YFloatType(m_rIntRectImage.Width())/m_rIntRectLogicalImage.Width() ),
								  YScaleFactor(YFloatType(m_rIntRectImage.Height())/m_rIntRectLogicalImage.Height() ) );

		// Get the frame object
		pFrame = RFrame::GetFrame( m_eFrameType, NULL );

		//
		// Get the rect inside the frame to which the bitmap should be blitted.
		// First we need to get the inset (how much space the frame extends into its 
		// interior), which will be in logical units.
		RRealSize sizeTopLeft;
		RRealSize sizeBottomRight;
		pFrame->GetInset( sizeTopLeft, sizeBottomRight );
		//
		// Shrink the bitmap destination rect by the thickness of the frame
		RRealRect rIntRectBitmapDest( m_rIntRectLogicalImage );
		rIntRectBitmapDest.Inset( sizeTopLeft, sizeBottomRight );
		//
		// Convert the bitmap destination rect to device units for the render call
		::LogicalUnitsToScreenUnits( rIntRectBitmapDest );
		//
		// Render the bitmap to the offscreen drawing surface
		m_rBitmapImage.Render( *pSurface, rIntRectBitmapDest );

		// Render the frame to the offscreen drawing surface
		pFrame->Render( *pSurface, transform, m_rIntRectLogicalImage, m_rColor, RColor( ) );
		
		// Render the image to the preview
		if ( fPrepared )
			drawSurfaceOffscreen.Release();
		drawSurface.DetachDCs();
	}
	catch( ... )
	{
		;
	}

	delete pFrame;
}

/////////////////////////////////////////////////////////////////////////////
// RFramesDialog message handlers

// ****************************************************************************
//
//  Function Name:	RFramesDialog::OnInitDialog
//
//  Description:		Handler for WM_INITDIALOG message initializes the dialog
//							when it's created.
//
//  Returns:			TRUE always
//
//  Exceptions:		None
//
// ****************************************************************************

BOOL RFramesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//
	// Set the bitmaps onto their buttons
	LoadFrameButtonBitmaps();
	
	//	Set the current color into the color button
	m_ColorBtn.SetColor( m_rColor );
	//
	// Get the preview's (constant) client rect for rendering
	CRect rcPreview;
	m_ctrlFrameDisplay.GetClientRect( rcPreview );
	m_rRealRectPreview = RRealRect( rcPreview );
	//
	// Load the image bitmap
	m_rBitmapImage.Initialize((YImageHandle) GetResourceManager().GetResourceBitmap( YResourceId( BITMAP_NO_FRAME ) ) );
	//
	// Get the rect the bitmap will take up within the preview
	RRealRect	rRealRectImage( 
						0.F,
						0.F,
						YRealCoordinate( m_rBitmapImage.GetWidthInPixels() ),
						YRealCoordinate( m_rBitmapImage.GetHeightInPixels() )
					);
	//
	// Center it within the preview.  It has to be an RIntRect to avoid roundoff 
	// errors when rendering the frame.
	m_rIntRectImage = RIntRect( rRealRectImage.CenterRectInRect( m_rRealRectPreview, TRUE, TRUE ) );
	m_rIntRectLogicalImage	= m_rIntRectImage;
	::ScreenUnitsToLogicalUnits( m_rIntRectLogicalImage );
	
	return !CheckActiveFrameButton();	// return TRUE unless you set the focus to a control
													// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL RFramesDialog::CheckActiveFrameButton()
{
	// set the focus to the correct button
	CButton *pButton = MapFrameToButton( m_eFrameType );
	BOOL bSetFocus = FALSE;

	if ( pButton != NULL && m_bMultipleFrameTypes == FALSE )
	{
		pButton->SetFocus();
		pButton->SetCheck( 1 );
		bSetFocus = TRUE;
	}
	else
	{
		PushFrameButton( -1 );
		pButton = (CButton*) GetDlgItem( CONTROL_BUTTON_REMOVE_FRAME );
		ASSERT_VALID( pButton );
		pButton->SetCheck( 1 );
		pButton->SetFocus();
		bSetFocus = TRUE;
	}

	return bSetFocus;
}

// ****************************************************************************
//
//  Function Name:	RFramesDialog::MapFrameToButton( EFrameType eFrameType )
//
//  Description:		Returns the buttons for a frame.
//
//  Returns:			CButton*
//
//  Exceptions:		None
//
// ****************************************************************************

CButton* RFramesDialog::MapFrameToButton( EFrameType eFrameType )
{
	CButton *pReturn = NULL;

	if ( eFrameType == kNoFrame )
		return pReturn;

	switch( eFrameType )
	{
	case kThinLineFrame:				pReturn = &m_btnThinFrame; 			break;
	case kMediumLineFrame:			pReturn = &m_btnMediumFrame;			break;
	case kThickLineFrame:			pReturn = &m_btnThickFrame;			break;
	case kDoubleLineFrame:			pReturn = &m_btnDblThinFrame;			break;
	case kMediumDoubleLineFrame:	pReturn = &m_btnDblMediumFrame;		break;
	case kThickDoubleLineFrame:	pReturn = &m_btnDblThickFrame;		break;
	case kRoundCornerFrame:			pReturn = &m_btnRoundCornerFrame;	break;
	case kPictureFrame:				pReturn = &m_btnPictureFrame;			break;
	case kDropShadowFrame:			pReturn = &m_btnDropShadowFrame;		break;
	default:
		TpsAssertAlways( "Bad Frame id!" );
	}

	ASSERT_VALID( pReturn );
	return pReturn;
}


// ****************************************************************************
//
//  Function Name:	RFramesDialog::LoadFrameButtonBitmaps( )
//
//  Description:		Initialization function loads the bitmaps for the frame
//							button faces.
//
//  Returns:			Nothing
//
//  Exceptions:		Ends the dialog if a CMemoryException occurs.
//
// ****************************************************************************

void RFramesDialog::LoadFrameButtonBitmaps()
{
	try
	{
		m_aButtonBitmaps.SetSize(	kNumFrameTypes );
	}
	catch( CMemoryException* e )
	{
		EndDialog( IDABORT );
		e->Delete();
	}

	COLORMAP ColorMap;
	ColorMap.from	= RGB( 192, 192, 192 );
	ColorMap.to		= ::GetSysColor( COLOR_BTNFACE );

	int iIndex = int( kThinLineFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_THIN_FRAME, 0, &ColorMap, 1 ) )
		m_btnThinFrame.SetBitmap( HBITMAP( m_aButtonBitmaps[ iIndex ] ) );

	iIndex = int( kMediumLineFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_MEDIUM_FRAME, 0, &ColorMap, 1 ) )
		m_btnMediumFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kThickLineFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_THICK_FRAME, 0, &ColorMap, 1 ) )
		m_btnThickFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kDoubleLineFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_DBL_THIN_FRAME, 0, &ColorMap, 1 ) )
		m_btnDblThinFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kMediumDoubleLineFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_DBL_MEDIUM_FRAME, 0, &ColorMap, 1 ) )
		m_btnDblMediumFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kThickDoubleLineFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_DBL_THICK_FRAME, 0, &ColorMap, 1 ) )
		m_btnDblThickFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kRoundCornerFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_ROUND_CORNER_FRAME, 0, &ColorMap, 1 ) )
		m_btnRoundCornerFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kPictureFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_PICTURE_FRAME, 0, &ColorMap, 1 ) )
		m_btnPictureFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));

	iIndex = int( kDropShadowFrame );
	if ( m_aButtonBitmaps[ iIndex ].LoadMappedBitmap( BITMAP_BUTTON_DROP_SHADOW_FRAME, 0, &ColorMap, 1 ) )
		m_btnDropShadowFrame.SetBitmap( HBITMAP(m_aButtonBitmaps[ iIndex ]));
}


void RFramesDialog::OnButtonThinFrame() 
{
	if ( m_eFrameType != kThinLineFrame )
	{
		m_eFrameType = kThinLineFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonMediumFrame() 
{
	if ( m_eFrameType != kMediumLineFrame )
	{
		m_eFrameType = kMediumLineFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonThickFrame() 
{
	if ( m_eFrameType != kThickLineFrame )
	{
		m_eFrameType = kThickLineFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonDblThinFrame() 
{
	if ( m_eFrameType != kDoubleLineFrame )
	{
		m_eFrameType = kDoubleLineFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonDblMediumFrame() 
{
	if ( m_eFrameType != kMediumDoubleLineFrame )
	{
		m_eFrameType = kMediumDoubleLineFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonDblThickFrame() 
{
	if ( m_eFrameType != kThickDoubleLineFrame )
	{
		m_eFrameType = kThickDoubleLineFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonRoundCornerFrame() 
{
	if ( m_eFrameType != kRoundCornerFrame )
	{
		m_eFrameType = kRoundCornerFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonPictureFrame() 
{
	if ( m_eFrameType != kPictureFrame )
	{
		m_eFrameType = kPictureFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonDropShadowFrame() 
{
	if ( m_eFrameType != kDropShadowFrame )
	{
		m_eFrameType = kDropShadowFrame;
		RenderPreview();
	}
}

void RFramesDialog::OnButtonRemoveFrame() 
{
	// clear this flag to show that the user
	// *really* doesn't want any frames - even if the selection
	// contains object with different frame types.
	m_bMultipleFrameTypes = FALSE;

	if ( m_eFrameType != kNoFrame )
	{
		m_eFrameType = kNoFrame;
		RenderPreview();
	}
}

// ****************************************************************************
//
//  Function Name:	RFramesDialog::OnButtonSetFrameColor( )
//
//  Description:		Brings up the color dialog, and if the user OKs out of it,
//							it sets the frame color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RFramesDialog::OnButtonSetFrameColor()
{
	//
	// Don't allow gradients or transparents
	RWinColorDlg dlg( NULL, 0 );	
	//
	// Set the current color into the dialog
	dlg.SetColor( m_rColor );
	//
	// Pop up the color dialog
	if( dlg.DoModal() == IDOK )
	{
		//
		// Get the selected color
		m_rColor = dlg.SelectedSolidColor();
	
		//	Set the current color into the color button
		m_ColorBtn.SetColor( m_rColor );
		//
		// Render the frame
		RenderPreview();
	}
}

// ****************************************************************************
//
//  Function Name:	RFramesDialog::OnPaint( )
//
//  Description:		Override updates the preview as well as the rest of the 
//							dialog.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RFramesDialog::OnPaint()
{
	//
	// Paint the dialog
	CPaintDC dc( this );
	//
	// Paint the preview
	RenderPreview();
}


// ****************************************************************************
//
//  Function Name:	RFramesDialog::PreTranslateMessage( MSG* pMsg )
//
//  Description:		Override to allow the user to use the arrow keys to 
//							logically move around the grid of buttons for frames.
//
//							Tab Order of Buttons:
//							1 2 3
//							4 5 6
//							7 8 9
//
//							1 = CONTROL_BUTTON_THIN_FRAME
//							9 = CONTROL_BUTTON_DROP_SHADOW_FRAME
//
//							Assumes that the buttons are defined with resource IDs 
//							in the same	order as the TAB order.
//
//  Returns:			TRUE if message was handled
//
//  Exceptions:		None
//
// ****************************************************************************

BOOL RFramesDialog::PreTranslateMessage( MSG* pMsg )
{
	if (pMsg->message == WM_KEYDOWN)
	{
		CWnd *pCurButton = GetFocus();
		pCurButton = dynamic_cast<CButton*> (pCurButton);

		if (pCurButton != NULL)
		{
			ASSERT_VALID( pCurButton );
			int nButtonID = pCurButton->GetDlgCtrlID();

			// is the focus set to one of the frames buttons
			if ((nButtonID >= CONTROL_BUTTON_THIN_FRAME) && 
				(nButtonID <= CONTROL_BUTTON_DROP_SHADOW_FRAME) )
			{
				ASSERT_VALID( pCurButton );

				switch(pMsg->wParam)
				{
				case VK_UP:
					// if we can move up, do so
					if ( nButtonID >= CONTROL_BUTTON_THIN_FRAME + 3 )
						PushFrameButton( nButtonID - 3 );
					return TRUE;
					break;

				case VK_DOWN:
					// if we can move down, do so
					if ( nButtonID <= CONTROL_BUTTON_DROP_SHADOW_FRAME - 3 )
						PushFrameButton( nButtonID + 3 );
					return TRUE;
					break;
				}
			}
		}
	}

	return CDialog::PreTranslateMessage( pMsg );
}


void RFramesDialog::PushFrameButton( int nID )
{
	CButton *pButton = NULL;

	for ( int nButtonID = CONTROL_BUTTON_THIN_FRAME; nButtonID <= CONTROL_BUTTON_DROP_SHADOW_FRAME; nButtonID++)
	{
		pButton = (CButton*) GetDlgItem( nID );

		if ( pButton != NULL )
		{
			ASSERT_VALID( pButton );

			if ( nID == nButtonID )
				pButton->SendMessage( BM_CLICK );
//			else
//				pButton->SetCheck( 0 );
		}
	}
}