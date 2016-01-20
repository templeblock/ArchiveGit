//****************************************************************************
//
// File Name:		ColorPalette.cpp
//
// Project:			Renaissance user interface
//
// Author:			Lance Wilson
//
// Description:	Manages RColorPalette class which is used for the 
//						popup color dialog.
//
// Portability:	Windows Specific
//
// Developed by:  Broderbund Software
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ColorPalette.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "ColorPalette.h"
#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "BitmapImage.h"
#include "FrameworkResourceIds.h"
#include "OffscreenDrawingSurface.h"

const RIntSize			kCellSize( 11, 11 );
const RIntPoint		kPtLeftCol( 19, 3 );
const RIntPoint		kPtFirstCol( 3, 3 );
const YIntDimension	kCellDistance( kCellSize.m_dx + 1 );

UINT RWinColorPalette::m_uPaletteBitmapID = BITMAP_COLOR_PALETTE;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RWinColorPaletteWell dialog

RWinColorPaletteWell::RWinColorPaletteWell() : 
	m_crColor( kBlack )
{
}

BEGIN_MESSAGE_MAP(RWinColorPaletteWell, CStatic)
	//{{AFX_MSG_MAP(RWinColorPalette)
	ON_WM_PAINT( )
	ON_WM_LBUTTONDOWN( )
	ON_WM_KEYDOWN( )
	ON_WM_GETDLGCODE( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void RWinColorPaletteWell::OnPaint( )
{
	CPaintDC dc( this );

	CRect rect;
	GetClientRect( rect );

	if (m_crColor.GetFillMethod() != RColor::kTransparent)
	{
		// The bounding rect is in logical units, so we
		// need to create a transform to scale from logical
		// units to device units.  We also divide by 2 to
		// account for the scaling in the bounding rect to
		// make the texture more visible in such a small area.
		RRealSize dpi( ::GetScreenDPI() );
		R2dTransform transform;
		transform.PostScale( 
			dpi.m_dx / kSystemDPI * 0.5, 
			dpi.m_dy / kSystemDPI * 0.5 );

		RColor crFillColor = m_crColor;
		crFillColor *= transform;

		RIntRect rRect( rect );
		RDcDrawingSurface ds;
		ds.Initialize( (HDC) dc );

		RColor oldColor = ds.GetFillColor();
		ds.SetFillColor( crFillColor );
		ds.FillRectangle( rRect );
		ds.SetFillColor( oldColor );
		ds.DetachDCs();
	}
	else
	{
		dc.FillSolidRect( rect, RGB( 255, 255, 255 ) );

		CString strText ;
		GetWindowText( strText );
		CRect rcTextRect( rect );
		rcTextRect.DeflateRect( 1, 1 );

		CFont* pFont = dc.SelectObject( GetParent()->GetFont() );
		dc.DrawText( strText, rcTextRect, DT_CENTER | DT_VCENTER );
		dc.SelectObject( pFont );
	}

	dc.MoveTo( rect.left, rect.bottom - 1 );
	dc.LineTo( rect.left, rect.top );
	dc.LineTo( rect.right, rect.top );
}

void RWinColorPaletteWell::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	CStatic::OnKeyDown( nChar, nRepCnt, nFlags );
}

UINT RWinColorPaletteWell::OnGetDlgCode( )
{
	return DLGC_WANTARROWS; 
}

void RWinColorPaletteWell::OnLButtonDown( UINT nFlags, CPoint pt )
{
	if (GetFocus() != this)
		SetFocus();

	LPARAM lParam = NULL;
	if (RColor::kSolid == m_crColor.GetFillMethod())
		lParam = (LPARAM)(YPlatformColor) m_crColor.GetSolidColor();
	else if (RColor::kTransparent == m_crColor.GetFillMethod())
		lParam = (LPARAM) -1L;

	GetParent()->SendMessage( UM_COLOR_CHANGED, GetDlgCtrlID(), lParam ) ;
	CStatic::OnLButtonDown( nFlags, pt );
}

void RWinColorPaletteWell::SetColor( RColor& crColor )
{
	if (crColor != m_crColor)
	{
		m_crColor = crColor;

		CRect rect;
		GetClientRect( rect );
		RIntRect	rRect( rect );

		// By making the bounding rect
		// twice as large, we will make
		// textures more visible.
		R2dTransform transform;
		transform.PostScale( 2, 2 );
		rRect *= transform;

		::ScreenUnitsToLogicalUnits( rRect );
		m_crColor.SetBoundingRect( rRect );

		if (m_hWnd)
			Invalidate( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// RWinColorPalette dialog


RBitmapImage&	RWinColorPalette::GetPaletteBitmapImage( )
{
	static BOOLEAN			m_fPaletteInitialized = FALSE;
	static RBitmapImage	m_biPalette;

	if ( !m_fPaletteInitialized )
	{
		// find the resource in the resource file
		HRSRC hRsrc = FindResource( AfxGetResourceHandle(), 
			MAKEINTRESOURCE( m_uPaletteBitmapID ), RT_BITMAP );

		if ( hRsrc != NULL )
		{
			// get a handle to the resource data
			HGLOBAL hTemp = LoadResource( AfxGetResourceHandle(), hRsrc );

			if ( hTemp != NULL )
			{
				// Initlize the palette bitmap with the resource data
				m_biPalette.Initialize( LockResource( hTemp ) );

				// unlock and free the resource
				UnlockResource( hTemp );
				FreeResource( hTemp );
			}
			else
				AfxThrowResourceException( );
		}
		else
			AfxThrowResourceException( );

		m_fPaletteInitialized = TRUE;

		COLORMAP crColorMap[] =
		{
			{ RGB( 255, 255, 255 ), GetSysColor( COLOR_BTNHIGHLIGHT ) },
			{ RGB( 192, 192, 192 ), GetSysColor( COLOR_BTNFACE )      },
			{ RGB( 128, 128, 128 ), GetSysColor( COLOR_BTNSHADOW )    }
		};

		RIntPoint ptCells[] = 
		{
			FindColor( crColorMap[0].from ),
			FindColor( crColorMap[1].from ),
			FindColor( crColorMap[2].from )
		};

		void*     pRawData   = m_biPalette.GetRawData();
		RGBQUAD*  pColorData = (RGBQUAD *) RBitmapImage::GetColorData( pRawData );
		LPBYTE    pImageData = (LPBYTE) RBitmapImage::GetImageData( pRawData );

		for (int j = 0; (LPVOID) pColorData < (LPVOID) pImageData; j++, pColorData++)
		{
			for (int i = 0; i < NumElements( crColorMap ); i++)
			{
				if (crColorMap[i].from == RGB( pColorData->rgbRed, pColorData->rgbGreen, pColorData->rgbBlue ))
				{
					pColorData->rgbBlue  = GetBValue( crColorMap[i].to );
					pColorData->rgbRed   = GetRValue( crColorMap[i].to );
					pColorData->rgbGreen = GetGValue( crColorMap[i].to );
					pColorData->rgbReserved = 0;
				}
			}

			if (j == 9)
			{
				// We only need to look at the system colors, so
				// jump to the last 10 entries in the palette.
				pColorData += 235;
			}
		}

		m_biPalette.UpdatePalette();

		//
		// Relace the cells that got remapped
		//
		ROffscreenDrawingSurface dsMem;
		dsMem.SetImage( &m_biPalette );

		RSolidColor rSolid;

		for (int i = 0; i < NumElements( ptCells ); i++)
		{
			if (ptCells[i].m_x >= 0 && ptCells[i].m_y >= 0)
			{
				RIntRect rcCell( RIntSize( kCellSize.m_dx - 2, kCellSize.m_dy - 2 ) );
				rcCell.Offset( RIntSize( ptCells[i].m_x + 1, ptCells[i].m_y + 1 ) );

				rSolid = crColorMap[i].from;

				RColor rColor( rSolid );
				dsMem.SetFillColor( rColor );
				dsMem.FillRectangle( rcCell );
			}
		}

		dsMem.ReleaseImage();
	}

	return m_biPalette;
}

COLORREF RWinColorPalette::ColorFromPoint( RIntPoint ptCell )
{
	RBitmapImage&	biPalette	= GetPaletteBitmapImage();
//	TpsAssert( biPalette.GetSystemHandle(), "Uninitialized palette!" );
	TpsAssert( biPalette.GetBitDepth() == 8, "Invalid bit-depth for color palette!" );
	TpsAssert( ptCell.m_x < (int) biPalette.GetWidthInPixels(), "Invalid coordinate!" );
	TpsAssert( ptCell.m_y < (int) biPalette.GetHeightInPixels(), "Invalid coordinate!" );

	void*     pRawData   = biPalette.GetRawData();
	RGBQUAD*  pColorData = (RGBQUAD *) RBitmapImage::GetColorData( pRawData );
	LPBYTE    pImageData = (LPBYTE) RBitmapImage::GetImageData( pRawData );

	uLONG     ulBytesPerRow = RBitmapImage::GetBytesPerRow( 
					biPalette.GetWidthInPixels(), 
					biPalette.GetBitDepth() );

	YIntDimension biHeight = ((BITMAPINFOHEADER *) pRawData)->biHeight;

	if (biHeight >= 0)
	{
		// The bitmap is bottom up.
		ptCell.m_y = (biHeight - 1) - ptCell.m_y ;
	}

	uLONG nIndex = (ptCell.m_y) * ulBytesPerRow + ptCell.m_x;
	RGBQUAD rgb  = pColorData[ pImageData[nIndex] ];
	
	return RGB( rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue );
}

RWinColorPalette::RWinColorPalette() : 
	m_crHilited( (COLORREF) -1L ),
	m_crSelected( (COLORREF) -1L ),
	m_ptSelected( kPtFirstCol ),
	m_ptHilited( -1, -1 ),
	m_fFlyoversEnabled( TRUE ),
	m_rcColorChip( 0, 0, 1, 1 )
{
	RBitmapImage& biPalette	= GetPaletteBitmapImage();
	biPalette.LockImage( );
}


RWinColorPalette::~RWinColorPalette( )
{
	RBitmapImage& biPalette	= GetPaletteBitmapImage();
	biPalette.UnlockImage( );
}

BEGIN_MESSAGE_MAP(RWinColorPalette, CStatic)
	//{{AFX_MSG_MAP(RWinColorPalette)
	ON_WM_LBUTTONDOWN( )
	ON_WM_MOUSEMOVE( )
	ON_WM_PAINT( )
	ON_WM_KEYDOWN( )
	ON_WM_GETDLGCODE( )
	ON_WM_KILLFOCUS( )
	ON_WM_SETFOCUS( )
	ON_WM_DESTROY( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void RWinColorPalette::OnPaint( )
{
	CPaintDC dc( this );
	RDcDrawingSurface ds;
	ds.Initialize( (HDC) dc ) ;

	CRect rcWindow;
	GetClientRect( rcWindow );
	RIntRect rcClient( rcWindow );

	RBitmapImage&	biPalette	= GetPaletteBitmapImage();
	biPalette.LockImage( );
	biPalette.Render( ds, rcClient );
	biPalette.UnlockImage( );
	ds.DetachDCs();

	CClientDC dcClient( this );
	ds.Initialize( (HDC) dcClient ) ;

	RColor crOldFill = ds.GetFillColor();
	RColor crOldPen  = ds.GetPenColor();

	if (m_ptSelected.m_x >= 0 && m_ptSelected.m_y >= 0)
	{
		//
		// Draw selected cell

		RIntRect rcSelected( 
			m_ptSelected.m_x + 1,
			m_ptSelected.m_y + 1,
			m_ptSelected.m_x + kCellSize.m_dx - 1,
			m_ptSelected.m_y + kCellSize.m_dy - 1 );

		RSolidColor crBlack( kBlack );
		RSolidColor crHilight( GetSysColor( COLOR_BTNHILIGHT ) );
		ds.Draw3dRect( rcSelected, crBlack, crHilight );

		RSolidColor crShadow( GetSysColor( COLOR_BTNSHADOW ) );
		rcSelected.m_Top -= 1; rcSelected.m_Left -= 1;
		ds.Draw3dRect( rcSelected, crShadow, crHilight );

		if (GetFocus() == this)
		{
			RSolidColor rSolid = GetSysColor( COLOR_HIGHLIGHT );
			RColor rSelected( rSolid );

			rcSelected.Inflate( RIntSize( 1, 1 ), RIntSize( 2, 2 ) );
			ds.SetPenColor( RColor( rSelected ) );
			ds.FrameRectangle( rcSelected );
		}
	}

	if (m_crHilited != (COLORREF) -1L)
	{
		// Draw color chip
		m_rcColorChip = RIntRect( 
			m_ptHilited.m_x,
			m_ptHilited.m_y,
			m_ptHilited.m_x + 2 * kCellSize.m_dx,
			m_ptHilited.m_y + 2 * kCellSize.m_dy );

		YIntDimension xOffset( 2 * kCellSize.m_dx );
		YIntDimension yOffset( 2 * kCellSize.m_dy );

		if (m_rcColorChip.m_Right + xOffset > rcWindow.right)
		{
			xOffset = -3 * kCellSize.m_dx;
		}

		if (m_rcColorChip.m_Bottom + yOffset > rcWindow.bottom)
		{
			yOffset = -3 * kCellSize.m_dy;
		}

		m_rcColorChip.Offset( RIntSize( xOffset, yOffset ) );

		RColor crFill = RSolidColor( m_crHilited );
		RColor crPen = RSolidColor( kWhite );

		ds.SetPenColor( crPen );
		ds.SetFillColor( crFill );
		ds.FillRectangle( m_rcColorChip );
		ds.FrameRectangle( m_rcColorChip );
	}
	else
	{
		m_rcColorChip = RIntRect( 0, 0, 1, 1 );
	}

	ds.SetPenColor( crOldPen );
	ds.SetFillColor( crOldFill );
	ds.DetachDCs();
}

void RWinColorPalette::OnLButtonDown( UINT nFlags, CPoint pt )
{
	if (GetFocus() != this)
		SetFocus();

	RIntPoint ptHit = CellFromPoint( pt );

	if (ptHit.m_x >= 0 && ptHit.m_y >= 0)
	{
		CRect rect( -1, -1, kCellSize.m_dx + 1, kCellSize.m_dy + 1 );
		rect.OffsetRect( m_ptSelected.m_x, m_ptSelected.m_y );
		InvalidateRect( rect, FALSE );

		RIntPoint ptTest( ptHit.m_x + 1, ptHit.m_y + 1 );
		m_crSelected = ColorFromPoint( ptTest );
		m_ptSelected = ptHit;

		GetParent()->SendMessage( UM_COLOR_CHANGED, GetDlgCtrlID(), (LPARAM) m_crSelected ) ;
		CStatic::OnLButtonDown( nFlags, pt );
	}
}

void RWinColorPalette::OnMouseMove( UINT, CPoint pt )
{
	if (!m_fFlyoversEnabled)
		return;

	CRect rcClient;
	GetClientRect( rcClient );

	COLORREF crColor = (COLORREF) -1L;

	if (rcClient.PtInRect( pt ))
	{
		if (GetCapture() != this)
			SetCapture();

		RIntPoint ptHit = CellFromPoint( pt );

		if (ptHit.m_x >= 0 && ptHit.m_y >= 0)
		{
			RIntPoint pt( ptHit.m_x + 1, ptHit.m_y + 1 );
			crColor = ColorFromPoint( pt );
			m_ptHilited = ptHit;
		}
	}
	else
	{
		ReleaseCapture();
	}

	if (crColor != m_crHilited)
	{
		m_crHilited = crColor;

		if (m_rcColorChip.Width() && m_rcColorChip.Height())
		{
			CRect rcInvalid = m_rcColorChip;
			InvalidateRect( rcInvalid, FALSE );
		}
	}
}

void RWinColorPalette::OnDestroy( )
{
	SetBitmap( NULL );
}

RIntPoint RWinColorPalette::CellFromPoint( CPoint pt )
{
	RIntPoint ptTopLeft( (pt.x < kPtLeftCol.m_x ? kPtFirstCol : kPtLeftCol) ) ;
	RIntPoint ptPoint( pt.x - ptTopLeft.m_x, pt.y - ptTopLeft.m_y );

	RIntPoint ptCell;
	ptCell.m_x = ptPoint.m_x / kCellDistance * kCellDistance + ptTopLeft.m_x;
	ptCell.m_y = ptPoint.m_y / kCellDistance * kCellDistance + ptTopLeft.m_y;

	if ((pt.x % kCellDistance > kCellSize.m_dx) ||
		 (pt.y % kCellDistance > kCellSize.m_dy))
	{
		// Point wasn't over a cell
		ptCell.m_x = ptCell.m_y = -1;
	}
	else if ((ptCell.m_x > kPtFirstCol.m_x + kCellSize.m_dx) && (ptCell.m_x < kPtLeftCol.m_x))
	{
		// Point wasn't over a cell
		ptCell.m_x = ptCell.m_y = -1;
	}

	return ptCell;
}

void RWinColorPalette::SetColor( RSolidColor crColor )
{
	m_crSelected = (COLORREF) crColor;

	if (m_hWnd && m_ptSelected.m_x >= 0 && m_ptSelected.m_y >= 0)
	{
		CRect rect( 0, 0, kCellSize.m_dx, kCellSize.m_dy );
		rect.OffsetRect( m_ptSelected.m_x, m_ptSelected.m_y );
		InvalidateRect( rect, FALSE );

		m_ptSelected = RWinColorPalette::FindColor( m_crSelected, FALSE );
	}
}

void RWinColorPalette::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	RIntPoint ptNew( kPtFirstCol );

	if (m_ptSelected.m_x >= 0 && m_ptSelected.m_y >= 0)
	{
		RIntPoint ptTopLeft( (m_ptSelected.m_x < kPtLeftCol.m_x ? kPtFirstCol : kPtLeftCol) ) ;
		RIntPoint ptPoint( m_ptSelected.m_x - ptTopLeft.m_x, m_ptSelected.m_y - ptTopLeft.m_y );

		if (m_ptSelected.m_x >= kPtLeftCol.m_x)
			ptPoint.m_x += kCellDistance;

		int nCurRow = ptPoint.m_y / kCellDistance;
		int nCurCol = ptPoint.m_x / kCellDistance;

		switch (nChar)
		{
		case VK_UP:
			nCurRow -= nRepCnt;
			break;

		case VK_DOWN:
			nCurRow += nRepCnt;
			break;

		case VK_LEFT:
			nCurCol -= nRepCnt;
			break;

		case VK_RIGHT:
			nCurCol += nRepCnt;
			break;
		}

		RBitmapImage& biPalette	= GetPaletteBitmapImage();
		int cxCols = (biPalette.GetWidthInPixels() - 
			kPtLeftCol.m_x) / kCellDistance + 1;  // One is added for first column

		int cxRows = (biPalette.GetHeightInPixels() -
			kPtLeftCol.m_y) / kCellDistance;

		nCurRow %= cxRows;
		nCurCol %= cxCols;

		if (nCurCol < 0) nCurCol += cxCols;
		if (nCurRow < 0) nCurRow += cxRows;

		ptNew.m_x = (nCurCol ? (nCurCol - 1) * kCellDistance + kPtLeftCol.m_x : kPtFirstCol.m_x);
		ptNew.m_y = nCurRow * kCellDistance + kPtLeftCol.m_y;
	}

	CRect rcInvalid( -1, -1, kCellSize.m_dx + 1, kCellSize.m_dy + 1 );
	rcInvalid.OffsetRect( m_ptSelected.m_x, m_ptSelected.m_y );
	InvalidateRect( rcInvalid, FALSE );

	RIntPoint pt( ptNew.m_x + 1, ptNew.m_y + 1 );
	m_crSelected = ColorFromPoint( pt );
	m_ptSelected = ptNew;

	GetParent()->SendMessage( UM_COLOR_CHANGED, GetDlgCtrlID(), (LPARAM) m_crSelected ) ;

	CStatic::OnKeyDown( nChar, nRepCnt, nFlags );
}

UINT RWinColorPalette::OnGetDlgCode( )
{
	return DLGC_WANTARROWS; 
}

void RWinColorPalette::OnKillFocus( CWnd* )
{
	CRect rcInvalid( -1, -1, kCellSize.m_dx + 1, kCellSize.m_dy + 1 );
	rcInvalid.OffsetRect( m_ptSelected.m_x, m_ptSelected.m_y );
	InvalidateRect( rcInvalid, FALSE );
}

void RWinColorPalette::OnSetFocus( CWnd* )
{
	CRect rcInvalid( -1, -1, kCellSize.m_dx + 1, kCellSize.m_dy + 1 );
	rcInvalid.OffsetRect( m_ptSelected.m_x, m_ptSelected.m_y );
	InvalidateRect( rcInvalid, FALSE );
}

void RWinColorPalette::PreSubclassWindow( )
{
	CStatic::PreSubclassWindow();


	// Set the new bitmap into the control
	RBitmapImage&	biPalette	= GetPaletteBitmapImage();
	HBITMAP hOldBitmap = SetBitmap( (HBITMAP) biPalette.GetSystemHandle() );
	
	if (hOldBitmap) 
		::DeleteObject( hOldBitmap );

	m_ptSelected = RWinColorPalette::FindColor( m_crSelected, FALSE );
}

RIntPoint RWinColorPalette::FindColor( YPlatformColor crColor, BOOL /*fExact*/ )
{
	RIntPoint ptCell( kPtFirstCol );
	RBitmapImage&	biPalette	= GetPaletteBitmapImage();
	RIntPoint ptMaxPt( 
		biPalette.GetWidthInPixels()  - kCellSize.m_dx,
		biPalette.GetHeightInPixels() - kCellSize.m_dy );

	while (ptCell.m_x <= ptMaxPt.m_x)
	{
		while (ptCell.m_y <= ptMaxPt.m_y)
		{
			RIntPoint pt( ptCell.m_x + 1, ptCell.m_y + 1 );

			if (crColor == ColorFromPoint( pt ))
				return ptCell;

			ptCell.m_y += kCellDistance;
		}

		ptCell.m_y = kPtLeftCol.m_y;

		if (ptCell.m_x != kPtFirstCol.m_x)
			ptCell.m_x += kCellDistance;  
		else
			ptCell.m_x = kPtLeftCol.m_x;
	}

	return RIntPoint( -1, -1 );
}

void RWinColorPalette::EnableFlyovers( BOOLEAN fEnabled )
{
	CRect rcInvalid( m_rcColorChip );
	InvalidateRect( rcInvalid, FALSE );

	m_crHilited = (COLORREF) -1L;
	m_fFlyoversEnabled = fEnabled;
}

