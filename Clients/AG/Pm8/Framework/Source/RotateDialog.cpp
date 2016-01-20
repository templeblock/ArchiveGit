//****************************************************************************
//
// File Name:		RotateDialog.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Shelah Horvitz
//
// Description:	Definition of RRotateDialog.   
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
//  $Logfile:: /PM8/Framework/Source/RotateDialog.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "RotateDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ResourceManager.h"
#include "ApplicationGlobals.h"
#include "AutoDrawingSurface.h"
#include "Utilities.h"
#include "ImageLibrary.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Buttonbmps.h"

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
// RRotateDialog dialog


// ****************************************************************************
//
//  Function Name:	RRotateDialog::RRotateDialog( )
//
//  Description:		Constructor
//
//  Returns:			N/A
//
//  Exceptions:		None
//
// ****************************************************************************

RRotateDialog::RRotateDialog( CWnd* pParent /*=NULL*/ ) : 
	CDialog( RRotateDialog::IDD, pParent ), m_dialRotate( FALSE )
{
	//{{AFX_DATA_INIT(RRotateDialog)
	//}}AFX_DATA_INIT

	m_iRotation			= kDefaultRotation;
	m_iPrevPos			= kMaxRotation + 1;
	m_bInitialised		= FALSE;
	m_dwLastUpdateTime = 0;
	m_uTimerID = 0;
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::DoDataExchange( )
//
//  Description:		Standard validation routine
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RRotateDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RRotateDialog)
	DDX_Control(	pDX,	CONTROL_SPIN_ROTATE,							m_spinRotate			);
	DDX_Control(	pDX,	CONTROL_STATIC_ROTATE,						m_staticDisplay		);
	DDX_Control(	pDX,	CONTROL_EDIT_ROTATE,							m_editRotation			);
	DDX_Control(	pDX,	CONTROL_STATIC_ROTATE_PLACEHOLDER,		m_rotatePlaceholder	);
	
	//}}AFX_DATA_MAP

	DDX_Text(		pDX,	CONTROL_EDIT_ROTATE,			m_iRotation		);
	DDV_MinMaxInt(	pDX,	m_iRotation, kMinRotation, kMaxRotation	);
}


BEGIN_MESSAGE_MAP( RRotateDialog, CDialog )

	ON_MESSAGE(	UM_ANGLE_CHANGE, OnDialAngleChanged )

	//{{AFX_MSG_MAP(RRotateDialog)
	ON_WM_PAINT()
	ON_EN_CHANGE(			CONTROL_EDIT_ROTATE,		OnTextChange)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RRotateDialog message handlers


// ****************************************************************************
//
//  Function Name:	RRotateDialog::OnInitDialog( )
//
//  Description:		Initializes the spin control
//
//  Returns:			TRUE
//
//  Exceptions:		kResource
//
// ****************************************************************************

BOOL RRotateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	TpsAssert( ( m_iRotation >= kMinRotation ) && ( m_iRotation <= kMaxRotation ), 
		"m_iRotation out of range" );
	
	m_spinRotate.SetRange( kMinRotation, kMaxRotation );
	m_spinRotate.SetPos( m_iRotation );
	
	m_rBitmapImage.Initialize((YImageHandle) GetResourceManager().GetResourceBitmap( YResourceId( BITMAP_NO_FRAME ) ) );

	m_sizeBmpImage.m_dx	= m_rBitmapImage.GetWidthInPixels();
	m_sizeBmpImage.m_dy	= m_rBitmapImage.GetHeightInPixels();

	CRect	rcStaticDisplay;
	m_staticDisplay.GetClientRect( &rcStaticDisplay );

	m_rectStaticDisplay.m_Left		= rcStaticDisplay.left;
	m_rectStaticDisplay.m_Top		= rcStaticDisplay.top;
	m_rectStaticDisplay.m_Right	= rcStaticDisplay.right;
	m_rectStaticDisplay.m_Bottom	= rcStaticDisplay.bottom;

	m_rSizeStaticDisplay	= RIntSize( m_rectStaticDisplay.Width(), m_rectStaticDisplay.Height() );
	m_rPtStaticCenter		= m_rectStaticDisplay.GetCenterPoint();

	// Create the rotate control
	CRect rectPlaceholder;
	m_rotatePlaceholder.GetWindowRect( rectPlaceholder );
	ScreenToClient(rectPlaceholder);
	m_dialRotate.Create(NULL, "", WS_CHILD | WS_VISIBLE, rectPlaceholder, this, (UINT)-1);

	m_bInitialised = TRUE;

	m_uTimerID = SetTimer( (UINT) kTimerID, kMinUpdateInterval, NULL );

	if ( m_uTimerID == 0 )
	{
		EndDialog( IDCANCEL );
		throw kResource;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::OnPaint( )
//
//  Description:		Override updates the image in the static control when a
//							WM_PAINT message comes through.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RRotateDialog::OnPaint() 
{
	CPaintDC dc( this ); // device context for painting
	
	// RotateDisplay() won't paint if the previous position is the same as the one
	// on the spin control.  Therefore give the previous position an invalid 
	// value to ensure the paint occurs.
	m_iPrevPos = kMaxRotation + 1;

	CString strNum;
	m_editRotation.GetWindowText( strNum );
	RotateDisplay( atoi( strNum ) );
	
	// Do not call CDialog ::OnPaint() for painting messages
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::RotateDisplay( )
//
//  Description:		Rotates the sample bitmap and blits it to the static 
//							control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RRotateDialog::RotateDisplay( int iPos )
{
	if ( iPos == m_iPrevPos )
		return;

	// We need to use the RBitmap::Rotate() function, so to do that, convert to
	// Renaissance types.
	CClientDC			clientDC( &m_staticDisplay );
	HDC					hClientDC = clientDC.GetSafeHdc();

	RDcDrawingSurface	drawSurface;
	drawSurface.Initialize( hClientDC, hClientDC );

	R2dTransform transform;

	// Prepare the image on an offscreen DC
	RAutoDrawingSurface	drawSurfaceOffscreen;
	RRealRect				rRealStaticRect(m_rectStaticDisplay);
	BOOLEAN					fPrepared = drawSurfaceOffscreen.Prepare( &drawSurface, transform, rRealStaticRect );
	RDrawingSurface*		pSurface	= ( (fPrepared)? &drawSurfaceOffscreen : &drawSurface );

	// Paint the offscreen DC white
	RColor rColor = RSolidColor( kWhite );
	pSurface->SetFillColor( rColor );
	pSurface->SetPenColor( rColor );
	pSurface->FillRectangle( m_rectStaticDisplay );

	YAngle	flRadiansRotation = ::DegreesToRadians( YFloatType( iPos ) );

	// The rotation leaves a black background, so blit with a mask.  The mask
	// has to be created the same size as the original (not rotated) bitmap
	// size, so create it and then rotate it.
	RBitmapImage		bmpRotated;
	RBitmapImage		bmpMaskRotated;
	RImageLibrary rLibrary;
	rLibrary.Rotate( m_rBitmapImage, bmpRotated, bmpMaskRotated, flRadiansRotation );

	RIntSize				sizeBmpRotated;
	sizeBmpRotated.m_dx = bmpRotated.GetWidthInPixels();
	sizeBmpRotated.m_dy = bmpRotated.GetHeightInPixels();

	RIntRect				rectSource;
	RIntRect				rectDest;
	DeriveSourceAndDestinationRects( sizeBmpRotated, rectSource, rectDest );

	bmpRotated.RenderWithMask( *pSurface, bmpMaskRotated, rectSource, rectDest );

	if ( fPrepared )
		drawSurfaceOffscreen.Release();
	drawSurface.DetachDCs();

	m_iPrevPos = iPos;
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::DeriveSourceAndDestinationRects( )
//
//  Description:		Returns by reference the source and destination rectangles 
//							for the rotated image, cropped so that they will fit in 
//							the static control.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void	RRotateDialog::DeriveSourceAndDestinationRects( const RIntSize& sizeBmpRotated, RIntRect& rectSource, RIntRect& rectDest )
{
	rectSource.m_Left		= 0;
	rectSource.m_Top		= 0;
	rectSource.m_Right	= sizeBmpRotated.m_dx;
	rectSource.m_Bottom	= sizeBmpRotated.m_dy;

	RIntPoint	ptSourceCenter;
	ptSourceCenter.m_x	= sizeBmpRotated.m_dx / 2;
	ptSourceCenter.m_y	= sizeBmpRotated.m_dy / 2;

	rectDest					= rectSource;

	YFloatType flHorzCropFactor = 1.F;
	YFloatType flVertCropFactor = 1.F;

	if ( sizeBmpRotated.m_dx > m_rSizeStaticDisplay.m_dx )
	{
		flHorzCropFactor	= 1.F - YFloatType( sizeBmpRotated.m_dx - m_rSizeStaticDisplay.m_dx ) / YFloatType( sizeBmpRotated.m_dx );
		rectDest.m_Right	= YIntCoordinate( YFloatType( rectDest.m_Right ) * flHorzCropFactor ); 
	}

	if ( sizeBmpRotated.m_dy > m_rSizeStaticDisplay.m_dy )
	{
		flVertCropFactor	= 1.F - YFloatType( sizeBmpRotated.m_dy - m_rSizeStaticDisplay.m_dy ) / YFloatType( sizeBmpRotated.m_dy );
		rectDest.m_Bottom	= YIntCoordinate( YFloatType( rectDest.m_Bottom ) * flVertCropFactor );
	}

	rectDest.Offset( RIntSize(  
		m_rPtStaticCenter.m_x - YIntCoordinate( YFloatType( rectDest.Width() )  / 2.F ),
		m_rPtStaticCenter.m_y - YIntCoordinate( YFloatType( rectDest.Height() ) / 2.F ) )
	);

	if ( flHorzCropFactor < 1.F )
	{
		YIntDimension iHorzSrcCropDistance = YIntDimension( ( 1.F - flHorzCropFactor ) * YFloatType( rectSource.m_Right ) / 2.F );
		rectSource.m_Left		+= iHorzSrcCropDistance;
		rectSource.m_Right	-= iHorzSrcCropDistance;
	}

	if ( flVertCropFactor < 1.F )
	{
		YIntDimension iVertSrcCropDistance = YIntDimension( ( 1.F - flVertCropFactor ) * YFloatType( rectSource.m_Bottom ) / 2.F );
		rectSource.m_Top		+= iVertSrcCropDistance;
		rectSource.m_Bottom	-= iVertSrcCropDistance;
	}
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::GetRotationAngle( )
//
//  Description:		Accessor function returns the rotation angle
//
//  Returns:			Angle chosen for rotation.
//
//  Exceptions:		None
//
// ****************************************************************************

YAngle	RRotateDialog::GetRotationAngle() const
{
	return ::DegreesToRadians( YFloatType( m_iRotation ) );
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::SetRotationAngle( )
//
//  Description:		Accessor function sets the rotation angle
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************

void	RRotateDialog::SetRotationAngle( YAngle angle ) 
{
	int iDegreeAngle = ::Round( ::RadiansToDegrees( angle ) );

	if ( iDegreeAngle < kMinRotation )
		iDegreeAngle += ( kMaxRotation + 1 );

	if ( iDegreeAngle > kMaxRotation )
		iDegreeAngle -= ( kMaxRotation + 1 );

	m_iRotation = iDegreeAngle;
	m_dialRotate.SetAngle( angle );
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::OnTextChange()
//
//  Description:		Called when the contents of the edit control
//							changes. Updates the preview and the rotate control
//							pointer.
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************

void RRotateDialog::OnTextChange()
{
	try
	{
		if ( m_bInitialised != FALSE && GetSafeHwnd != NULL && m_editRotation.GetSafeHwnd() != NULL )
		{
			CString szValue;
			m_editRotation.GetWindowText( szValue );
			int angle = atoi( szValue );
			m_dialRotate.SetAngle( ::DegreesToRadians( angle ) );
		}
	}
	catch( ... )
	{
	}
}


// ****************************************************************************
//
//  Function Name:	RRotateDialog::OnDialAngleChanged(WPARAM, LPARAM )
//
//  Description:		Called when the position of the rotate control pointer
//							changes. Updates the edit control and the preview.
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************

LRESULT RRotateDialog::OnDialAngleChanged(WPARAM, LPARAM )
{
	try
	{
		YAngle angle = m_dialRotate.GetAngle();
		int nDegrees = ::Round( ::RadiansToDegrees( angle ) );
		
		if ( nDegrees < 0 )
			nDegrees = 360 + nDegrees;

		CString szText;
		szText.Format("%d", nDegrees );
		m_editRotation.SetWindowText( szText );
	}
	catch( ... )
	{
	}
	return (LRESULT) 1;
}

// ****************************************************************************
//
//  Function Name:	RRotateDialog::OnTimer( UINT nID )
//
//  Description:		Updates the preview based on the dit control.
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************

void RRotateDialog::OnTimer( UINT nID )
{

	try
	{
		if ( nID == kTimerID )
		{
			CString szValue;
			m_editRotation.GetWindowText( szValue );
			int angle = atoi( szValue );
			RotateDisplay( angle );
		}
	}
	catch(...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RRotateDialog::OnDestroy( )
//
//  Description:		Cleans up the timer.
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************

void RRotateDialog::OnDestroy( )
{
	if ( m_uTimerID != 0 )
		KillTimer( m_uTimerID );
}