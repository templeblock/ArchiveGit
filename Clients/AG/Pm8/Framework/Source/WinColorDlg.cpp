//****************************************************************************
//
// File Name:   WinColorDlg.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Manages RWinColorDlg and RWinColorBtn classes which 
//              are used for the popup color dialog.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//					  500 Redwood Blvd.
//					  Novato, CA 94948
//					  (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WinColorDlg.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "WinColorDlg.h"
#include "ImageColor.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DialogUtilities.h"
#include "OffscreenDrawingSurface.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct tagGradientTemplate
{
	const RColorTemplateEntry* pTemplate ;
	int						      nCount ;
} GradientTemplate ;

const uWORD kInverted   = 0x0001 ;
const uWORD kDualLinear = 0x0002 ;

const uWORD kDualLinearStyle = 4 ;

const RColorTemplateEntry kSelectedToWhite[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 1.0, -1.0, -1.0 ), 0.50, 0.95 )
} ;

const RColorTemplateEntry kSelectedToBlack[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, -1.0, -1.0 ), 0.50, 0.95 )
} ;

const RColorTemplateEntry kSelectedTo60[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.5, 0.0 ), 0.50, 0.95 )
} ;

const RColorTemplateEntry kSelectedTo60Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, -0.5, 0.0 ), 0.50, 0.95 )
} ;

const RColorTemplateEntry kSelectedTo60To120[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.46, 0.09 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.5, 0.0 ), 0.55, 0.52 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-2.0, 0.0 ), 0.50, 0.93 )
} ;

const RColorTemplateEntry kSelectedTo60To120Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.40, 0.08 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.5, 0.0 ), 0.60, 0.51 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.0, 0.0 ), 0.50, 0.90 )
} ;

const RColorTemplateEntry kSelectedTo45To90To135To180[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.000, 0.0 ), 0.50, 0.06 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.375, 0.0 ), 0.50, 0.28 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.750, 0.0 ), 0.50, 0.50 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.875, 0.0 ), 0.50, 0.72 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.500, 0.0 ), 0.50, 0.94 )
} ;

const RColorTemplateEntry kSelectedTo45To90To135To180Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.000, 0.0 ), 0.50, 0.06 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.375, 0.0 ), 0.50, 0.28 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.750, 0.0 ), 0.50, 0.50 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.125, 0.0 ), 0.50, 0.72 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.500, 0.0 ), 0.50, 0.94 )
} ;

const RColorTemplateEntry kSelectedTo60To120To180To240To300[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.5, 0.0 ), 0.50, 0.23 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-2.0, 0.0 ), 0.50, 0.41 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.5, 0.0 ), 0.50, 0.59 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.0, 0.0 ), 0.50, 0.77 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.5, 0.0 ), 0.50, 0.95 )
} ;

const RColorTemplateEntry kSelectedTo60To120To180To240To300Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.5, 0.0 ), 0.50, 0.23 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.0, 0.0 ), 0.50, 0.41 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.5, 0.0 ), 0.50, 0.59 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-2.0, 0.0 ), 0.50, 0.77 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.5, 0.0 ), 0.50, 0.95 )
} ;

//
// Dual linear templates
//
const RColorTemplateEntry kDualSelectedToWhite[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 1.0, -1.0, -1.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedToBlack[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, -1.0, -1.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo30[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( -1.0, 0.25, 1.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo30Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( -1.0, -0.25, 1.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo60[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.5, 0.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo60Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.05 ),
	RColorTemplateEntry( RWHPTriple( 0.0, -0.5, 0.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo60To120[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.06 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.5, 0.0 ), 0.50, 0.22 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-2.0, 0.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo60To120Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.0, 0.0 ), 0.50, 0.06 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.5, 0.0 ), 0.50, 0.22 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.0, 0.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo45To90To135To180[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.000, 0.0 ), 0.50, 0.06 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.375, 0.0 ), 0.50, 0.17 ),
	RColorTemplateEntry( RWHPTriple( 0.0, 0.750, 0.0 ), 0.50, 0.28 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.875, 0.0 ), 0.50, 0.39 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.500, 0.0 ), 0.50, 0.50 )
} ;

const RColorTemplateEntry kDualSelectedTo45To90To135To180Reverse[] =
{
	RColorTemplateEntry( RWHPTriple( 0.0, 0.000, 0.0 ), 0.50, 0.06 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.375, 0.0 ), 0.50, 0.17 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-0.750, 0.0 ), 0.50, 0.28 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.125, 0.0 ), 0.50, 0.39 ),
	RColorTemplateEntry( RWHPTriple( 0.0,-1.500, 0.0 ), 0.50, 0.50 )
} ;


static GradientTemplate _arrTemplateArray[] =
{
	{ kSelectedToWhite, 2 },
	{ kSelectedToBlack, 2 },
	{ kSelectedTo60, 2 },
	{ kSelectedTo60Reverse, 2 },
	{ kSelectedTo60To120, 3 },
	{ kSelectedTo60To120Reverse, 3 },
	{ kSelectedTo45To90To135To180, 5 },
	{ kSelectedTo45To90To135To180Reverse, 5 },
	{ kSelectedTo60To120To180To240To300, 6 },
	{ kSelectedTo60To120To180To240To300Reverse, 6 }
} ;

static GradientTemplate _arrDualTemplateArray[] =
{
	{ kDualSelectedToWhite, 2 },
	{ kDualSelectedToBlack, 2 },
	{ kDualSelectedTo60, 2 },
	{ kDualSelectedTo60Reverse, 2 },
	{ kDualSelectedTo60To120, 3 },
	{ kDualSelectedTo60To120Reverse, 3 },
	{ kDualSelectedTo45To90To135To180, 5 },
	{ kDualSelectedTo45To90To135To180Reverse, 5 },
	{ kDualSelectedTo30, 2 },
	{ kDualSelectedTo30Reverse, 2 }
} ;

// Define conversion array from our value to the enumerated data type
static EGradientType _arGradientTypes[] = 
{ 
	kLinear, 
	kLinear, 
	kCircularBurst, 
	kSquareBurst 
} ;

static int _invertedTable[] = { 0, 1, 0, 1, 1, 0, 1, 0 } ;

// RWinColorBtn Implementation
///////////////////////////////////////////////////////////////////////

RWinColorBtn::RWinColorBtn(uWORD uwFlags) : 
	m_crFillColor( RSolidColor( kBlack ) ) 
{
	m_hbmSave      = NULL ;
	m_uwFlags      = uwFlags ;
}

RWinColorBtn::~RWinColorBtn()
{
}

BEGIN_MESSAGE_MAP(RWinColorBtn, CButton /*ROwnerDrawBtn*/)
	//{{AFX_MSG_MAP(RWinColorBtn)
	ON_WM_DESTROY() 
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void RWinColorBtn::GetGradientRect( RIntRect& rect )
{
	CRect rectGradient ;
	GetClientRect( rectGradient ) ;

	rectGradient.DeflateRect( 1, 1, 2, 2 ) ;	// Adjust for the border

	if (m_uwFlags && kInset)
	{
		rectGradient.DeflateRect( 1, 1 ) ;	// Adjust for inset
	}

	rect.m_Top	   = rectGradient.top ;
	rect.m_Left		= rectGradient.left ;
	rect.m_Bottom	= rectGradient.bottom ;
	rect.m_Right	= rectGradient.right ;
}

BOOLEAN RWinColorBtn::Initialize( RDcDrawingSurface& ds, RIntSize size )
{
	try
	{
		// 
		// We are only going to draw the gradient part, so 
		// shrink the size down accounting for the border.
		//
		size.m_dx -= 3 ;
		size.m_dy -= 3 ;

		if (m_uwFlags && kInset)
		{
			size.m_dx -= 2 ;	// Border of 2 pixels on each side
			size.m_dy -= 2 ;
		}

		m_biImage.Initialize( ds, size.m_dx, size.m_dy ) ;

		RIntPoint ptStart( 0, 0 ) ;
		RIntPoint ptEnd( size.m_dx - 1, size.m_dy - 1 ) ;
		RIntSize  szDelta = ptEnd - ptStart ;

		// Set the bitmap
		m_hbmSave = (HBITMAP) SetBitmap( (HBITMAP) 
			m_biImage.GetSystemHandle() ) ;

		SetBoundingRect() ;
		UpdateImage() ;
	}
	catch( YException )
	{
		return FALSE ;
	}

	return TRUE ;
}

void RWinColorBtn::UpdateImage()
{
	ROffscreenDrawingSurface dsMem ; 

	// Get the bitmap rectangle
	RIntRect rectSrc(0, 0, 
		m_biImage.GetWidthInPixels(),
		m_biImage.GetHeightInPixels()) ;

	dsMem.SetImage( &m_biImage ) ;

	if (RColor::kTransparent == m_crFillColor.GetFillMethod())
	{

		RMBCString szText = GetResourceManager().GetResourceString( 
			STRING_COLOR_TRANSPARENT ) ;
		RRealRect rcText( rectSrc.m_Left,  rectSrc.m_Top,
			rectSrc.m_Right, rectSrc.m_Bottom ) ;

		try
		{
			YFontInfo font ;
			font.attributes = 0;
			font.angle		 = 0;
			font.ulPadding	 = 0;

			LOGFONT lf ;
			GetFont()->GetLogFont( &lf ) ;
			font.height = lf.lfHeight ;
			font.width  = lf.lfWidth ;
			strcpy( font.sbName, lf.lfFaceName );

			dsMem.SetFillColor( RSolidColor( kWhite ) ) ;
			dsMem.FillRectangle( rectSrc ) ;
			dsMem.SetFont( font ) ;

			R2dTransform transform ;
			dsMem.PlaceTextInRect( rcText, szText, transform, 
				kPlaceTextVerticalPos | kPlaceTextHorizontalPos );
		}
		catch (...)
		{
		}
	}
	else
	{
		RRealSize dpi( ::GetScreenDPI() );

		// The bounding rect is in logical units, so we
		// need to create a transform to scale from logical
		// units to device units.
		R2dTransform transform;
		transform.PostScale( dpi.m_dx / kSystemDPI, dpi.m_dy / kSystemDPI );

		RColor crFillColor = m_crFillColor;
		crFillColor *= transform;

		dsMem.SetFillColor( crFillColor ) ;
		dsMem.FillRectangle( rectSrc ) ;
	}

	if (m_uwFlags & kInset)
	{
		rectSrc.m_Right--; 
		rectSrc.m_Bottom-- ;

		dsMem.Draw3dRect( rectSrc, 
			RSolidColor( GetSysColor( COLOR_BTNSHADOW ) ),
			RSolidColor( GetSysColor( COLOR_BTNHILIGHT ) ) ) ;
	}

	// Cleanup
	dsMem.RestoreDefaults() ;
	dsMem.ReleaseImage() ;
	Invalidate( FALSE ) ;
}


void	RWinColorBtn::DrawItem( LPDRAWITEMSTRUCT /*lpDrawItemStruct*/ )
{
/*	CRect rect( lpDrawItemStruct->rcItem ) ;
	CDC	dc ;

	dc.Attach( lpDrawItemStruct->hDC ) ;

	RIntRect rectDest ;
	GetGradientRect( rectDest ) ;

	if (lpDrawItemStruct->itemState & (ODS_SELECTED | ODS_CHECKED))
	{
		rectDest.Offset( RIntSize(1, 1) ) ;
	}

	RDcDrawingSurface ds ;
	ds.Initialize( (HDC) dc ) ;

	//
	// Draw the button
	//
	DrawButton (&dc, rect, lpDrawItemStruct->itemState) ;

	//
	// Draw the gradient
	//
	m_biImage.Render( ds, rectDest ) ;
	
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
//		rectDest.m_Left  -= 1 ; rectDest.m_Top    -= 1 ; 
//		rectDest.m_Right += 1 ; rectDest.m_Bottom += 1 ;

		static BITMAP   bitmap = { 0, 8, 8, 2, 1, 1 } ;
		static WORD wPatternBits[] = 
			{ 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA } ;
		
		try
		{
			CBitmap bmpPattern ;
			bitmap.bmBits = (LPSTR) wPatternBits ;
			
			if (!bmpPattern.CreateBitmapIndirect (&bitmap))
			{
				AfxThrowResourceException() ;
			}

			CRect  rect = rectDest ;
			CBrush brush( &bmpPattern ) ;

			COLORREF crOldColor = dc.SetBkColor( 
				GetSysColor( COLOR_BTNFACE ) ) ;

			dc.FrameRect( rect, &brush ) ;
			dc.SetBkColor( crOldColor ) ;
		}
		catch (CResourceException)
		{
			// REVIEW: display an error message?
		}
	}

	ds.DetachDCs( );
	dc.Detach() ;
*/
}

void RWinColorBtn::PreSubclassWindow( )
{
	CRect rect ;
	GetClientRect( rect ) ;

	CClientDC dc(this) ;
	RDcDrawingSurface ds;
	ds.Initialize( (HDC) dc );

	Initialize( ds, RIntSize(rect.Width(), rect.Height()) ) ;
	ds.DetachDCs( );
}

void RWinColorBtn::OnDestroy( )
{
	// Return the old bitmap
	SetBitmap( (HBITMAP) m_hbmSave ) ;

	CWnd::OnDestroy() ;
}

void RWinColorBtn::SetColor( RColor crFillColor )
{
	// Copy the color
	m_crFillColor = crFillColor ;

	if (IsWindow( m_hWnd ))
	{
		// Update the image
		SetBoundingRect();
		UpdateImage() ;
	}
}

void RWinColorBtn::SetBoundingRect()
{
	RIntRect	rc;
	GetGradientRect( rc );

	::ScreenUnitsToLogicalUnits( rc );
	m_crFillColor.SetBoundingRect( rc );
}

void RWinColorBtn::SetGradientAngle( YAngle angle )
{
	RGradientFill* pGradientFill = m_crFillColor.m_pGradient;	// Asserts it's a gradient

	if (pGradientFill)
	{
		pGradientFill->SetGradientAngle( /*kPI+*/angle ) ;	//	All angles must be reflected about y-axis
		SetBoundingRect() ;
		UpdateImage() ;
	}
}

/*void RWinColorBtn::SetGradientOrigin()
{
	RGradientFill* pGradientFill = m_crFillColor.m_pGradient;	// Asserts it's a gradient

	if (pGradientFill)
	{
		RIntRect	rc;
		GetGradientRect( rc );
		pGradientFill->SetBoundingRect( rc );
	}
}*/

void RWinColorBtn::SetGradientType( EGradientType eType )
{
	RGradientFill* pGradientFill = m_crFillColor.m_pGradient;	// Asserts it's a gradient

	if (pGradientFill && pGradientFill->GetType() != eType)
	{
		pGradientFill->SetType( eType ) ;
		SetBoundingRect() ;
		UpdateImage() ;
	}
}

void RWinColorBtn::SetGradientParams( EGradientType eType, RGradientRamp& ramp, YAngle angle )
{
	RGradientFill gradient ;

	gradient.SetType( eType ) ;
	gradient.SetGradientAngle( /*kPI+*/angle ) ;		//	All Angles must be reflected about y-axis
	gradient.SetRamp( &ramp ) ;

	m_crFillColor = RColor( gradient ) ;

	// Update the image
	SetBoundingRect() ;
	UpdateImage() ;
}

// RWinMultiColorBtn Implementation
///////////////////////////////////////////////////////////////////////

RWinMultiColorBtn::RWinMultiColorBtn(uWORD uwFlags) : 
	RWinColorBtn(uwFlags),
	m_yMultiColorBitmapId( BITMAP_BUTTON_COLOR_UP ),
	m_yMultiGrayscaleBitmapId( BITMAP_BUTTON_TINT_UP ),
	m_eMultiColorState( kSingleColor )
	{
	NULL;
	}


BOOLEAN RWinMultiColorBtn::Initialize( RDcDrawingSurface& ds, RIntSize size )
{
	//	Make sure we are given a useful size or we will fail
	//	to initialize the bitmap image below.
	//	Use the size of the color bitmaps.
	if( 0 >= size.m_dx )
		size.m_dx = 46;

	if( 0 >= size.m_dy )
		size.m_dy = 22;

	//	Do our inherited initialization.
	return RWinColorBtn::Initialize( ds, size );
}

void RWinMultiColorBtn::UpdateImage()
{
	RSolidColor buttonHilight( GetSysColor( COLOR_BTNFACE ) ) ;

	//	If we are drawing the multiple selection states then show the
	//	appropriate bitmaps.
	if ( kSingleColor != m_eMultiColorState )
	{
		try
		{
			ROffscreenDrawingSurface dsMem; 

			//	Move the multi color bitmap to account for inset borders.
			// Get the bitmap rectangle
			RIntRect rectSrc(0, 0, 
				m_biImage.GetWidthInPixels(),
				m_biImage.GetHeightInPixels());
			RIntRect rectDst( rectSrc );
			RIntRect rectInset;
			
			//	Get the inset border rect.
			GetGradientRect( rectInset );
			
			//	Offset the destination rect.
			rectDst.Offset( RIntSize( -rectInset.m_Left, -rectInset.m_Top ) );
		
			//	Prep the image for use.
			dsMem.SetImage( &m_biImage );

			//	Fill the button with the hilite color to erase any prior
			CRect cButtonRect;
			GetClientRect( &cButtonRect );
			dsMem.SetFillColor( buttonHilight ) ;
			dsMem.FillRectangle( cButtonRect ) ;
	
			//	Get the appropriate bitmap.
			DWORD style = GetStyle( );
			RBitmapImage rBitmap( FALSE );
			YResourceId	yBitmapId;
			if ( kMultiColor == m_eMultiColorState )
			{
				if ( WS_DISABLED & style )
					yBitmapId = BITMAP_BUTTON_COLOR_DISABLED;
				else
					yBitmapId = m_yMultiColorBitmapId;
			}
			else
			{
				if ( WS_DISABLED & style )
					yBitmapId = BITMAP_BUTTON_TINT_DISABLED;
				else
					yBitmapId = m_yMultiGrayscaleBitmapId;
			}

			//	Prep it for use.
			rBitmap.Initialize( yBitmapId );

			//	Draw the appropriate bitmap.
			rBitmap.Render( dsMem, rectSrc, rectDst );

			if (m_uwFlags & kInset)
			{
				rectSrc.m_Right--; 
				rectSrc.m_Bottom-- ;

				dsMem.Draw3dRect( rectSrc, 
					RSolidColor( GetSysColor( COLOR_BTNSHADOW ) ),
					RSolidColor( GetSysColor( COLOR_BTNHILIGHT ) ) ) ;
			}

			// Cleanup
			dsMem.RestoreDefaults() ;
			dsMem.ReleaseImage() ;
			Invalidate( FALSE ) ;
		}
		catch( ... )
		{
			//	Review ESV - what else should we do here?
			NULL;
		}
	}
	else
		RWinColorBtn::UpdateImage();
}

void RWinMultiColorBtn::SetMultiColor( const BOOLEAN fUpdate /*= TRUE*/ )
{
	//	Set the current button state.
	m_eMultiColorState = kMultiColor;

	// Update the image
	if ( fUpdate )
		UpdateImage() ;
}

void RWinMultiColorBtn::SetMultiGrayscale( const BOOLEAN fUpdate /*= TRUE*/ )
{
	//	Set the current button state.
	m_eMultiColorState = kMultiGrayscale;

	// Update the image
	if ( fUpdate )
		UpdateImage() ;
}

void RWinMultiColorBtn::SetColor( RColor crFillColor )
{
	//	Set the current button state.
	m_eMultiColorState = kSingleColor;
	
	//	Show the color.
	RWinColorBtn::SetColor( crFillColor );
}




/////////////////////////////////////////////////////////////////////////////
// RWinColorPaletteDlg dialog

YPlatformColor RWinColorPaletteDlg::mruColors[6] = 
{ 
	RGB(255,  0,  0),
   RGB(  0,255,  0),
	RGB(  0,  0,255),
	RGB(255,255,  0),
	RGB(  0,255,255),
	RGB(255,  0,255) 
} ;


//*****************************************************************************
//
// Function Name:  RWinColorPaletteDlg
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RWinColorPaletteDlg::RWinColorPaletteDlg( CWnd* pParent, BOOLEAN fShowTransparent, UINT nIDTemplate )
	: CDialog( nIDTemplate, pParent )
{
	m_fShowTransparent = fShowTransparent ;
}


//*****************************************************************************
//
// Function Name: DoDataExchange
//
// Description:   Method for transferning the contents of the child
//				      controls to/from the data member variables.  As
//						well as any subclassing.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RWinColorPaletteDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RWinColorPaletteDlg)
	DDX_Control( pDX, CONTROL_COLOR_MRU1, m_scMRUList[0] ) ;
	DDX_Control( pDX, CONTROL_COLOR_MRU2, m_scMRUList[1] ) ;
	DDX_Control( pDX, CONTROL_COLOR_MRU3, m_scMRUList[2] ) ;
	DDX_Control( pDX, CONTROL_COLOR_MRU4, m_scMRUList[3] ) ;
	DDX_Control( pDX, CONTROL_COLOR_MRU5, m_scMRUList[4] ) ;
	DDX_Control( pDX, CONTROL_COLOR_MRU6, m_scMRUList[5] ) ;
	DDX_Control( pDX, CONTROL_COLOR_NONE, m_scTransparent ) ;
	DDX_Control( pDX, CONTROL_COLOR_PALETTE, m_scPalette ) ;
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(RWinColorPaletteDlg, CDialog)
	//{{AFX_MSG_MAP(RWinColorPaletteDlg)
	ON_COMMAND_RANGE( CONTROL_COLOR_MRU1, CONTROL_COLOR_PALETTE, OnColorClicked )
	ON_BN_CLICKED( CONTROL_COLOR_MORE, OnColorMore )
	ON_MESSAGE( UM_COLOR_CHANGED, OnColorChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: OnInitDialog
//
// Description:   Initializes dialog
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RWinColorPaletteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_fShowTransparent)
		m_scTransparent.ShowWindow( SW_HIDE );
	else
	{
		RColor crTransparent;
		m_scTransparent.SetColor( crTransparent );
	}

	for (int i = 0; i < NumElements( mruColors ); i++)
	{
		RSolidColor rSolid( mruColors[i] );
		RColor crColor( rSolid );
		m_scMRUList[i].SetColor( crColor );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RWinColorPaletteDlg::OnColorClicked( UINT /*nID*/ )
{
	AddSelectedToMRU();
	EndDialog( IDOK );
}

void RWinColorPaletteDlg::OnColorMore()
{
	EndDialog( CONTROL_COLOR_MORE );
}

LRESULT RWinColorPaletteDlg::OnColorChange( WPARAM nID, LPARAM lParam )
{
	RSolidColor rSolid( (YPlatformColor) lParam );
	m_crColor = RColor( rSolid ) ;

	if (lParam < 0) 
		m_crColor = RColor();

	// Update the color palette with
	// the new color if necessary.
	if (nID != CONTROL_COLOR_PALETTE)
		m_scPalette.SetColor( (YPlatformColor) lParam );

	return 0L;
}

void RWinColorPaletteDlg::SetColor( RColor crColor )
{
	switch (crColor.GetFillMethod())
	{
	case RColor::kSolid:
		m_scPalette.SetColor( crColor.GetSolidColor() );
		break ;

	case RColor::kTransparent:
		m_scPalette.SetColor( RSolidColor( (COLORREF) -1L ) );
		break;

	default:
		TpsAssertAlways( "Invalid color fill method!" );
	}

	m_crColor = crColor;
}

void RWinColorPaletteDlg::AddSelectedToMRU()
{
	if (m_crColor.GetFillMethod() != RColor::kTransparent)
	{
		YPlatformColor crColor = m_crColor.GetSolidColor();

		// First see if it is in the list
		for (int i = 0; i < NumElements( mruColors ); i++)
			if (crColor == mruColors[i])
				break;

		for (i = i - 1; i >= 0; i--)
			if (i < NumElements( mruColors ) - 1)
				mruColors[i + 1] = mruColors[i];

		mruColors[0] = crColor;
	}
}




/////////////////////////////////////////////////////////////////////////////
// RWinColorPaletteDlgEx dialog

//*****************************************************************************
//
// Function Name:  RWinColorPaletteDlgEx
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RWinColorPaletteDlgEx::RWinColorPaletteDlgEx( CWnd* pParent, BOOLEAN fShowTransparent ): 
	RWinColorPaletteDlg( pParent, fShowTransparent, RWinColorPaletteDlgEx::IDD ),
	m_crColor( kBlack )
{
	//{{AFX_DATA_INIT(RWinColorPaletteDlgEx)
	m_nGradientType  = 0 ;
	m_nGradientSel   = 0 ;
	//}}AFX_DATA_INIT
}

//*****************************************************************************
//
// Function Name: DoDataExchange
//
// Description:   Method for transferning the contents of the child
//				      controls to/from the data member variables.  As
//						well as any subclassing.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RWinColorPaletteDlgEx::DoDataExchange(CDataExchange* pDX)
{
	RWinColorPaletteDlg::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RWinColorPaletteDlgEx)
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT1,  m_rbGradientArray[0] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT2,  m_rbGradientArray[1] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT3,  m_rbGradientArray[2] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT4,  m_rbGradientArray[3] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT5,  m_rbGradientArray[4] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT6,  m_rbGradientArray[5] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT7,  m_rbGradientArray[6] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT8,  m_rbGradientArray[7] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT9,  m_rbGradientArray[8] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT10, m_rbGradientArray[9] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_GRADIENT11, m_rbGradientArray[10] ) ;

	DDX_Control( pDX, CONTROL_RADIO_COLOR_LINEAR,      m_rbStyleArray[0] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_LINEAR2,     m_rbStyleArray[2] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_CIRCLEBURST, m_rbStyleArray[4] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_SQAUREBURST, m_rbStyleArray[6] ) ;

	DDX_Control( pDX, CONTROL_RADIO_COLOR_INVERSE_LINEAR,      m_rbStyleArray[1] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_INVERSE_LINEAR2,     m_rbStyleArray[3] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_INVERSE_CIRCLEBURST, m_rbStyleArray[5] ) ;
	DDX_Control( pDX, CONTROL_RADIO_COLOR_INVERSE_SQAUREBURST, m_rbStyleArray[7] ) ;

	DDX_Radio( pDX, CONTROL_RADIO_COLOR_LINEAR, m_nGradientType ) ;
	DDX_Radio( pDX, CONTROL_RADIO_COLOR_GRADIENT1, m_nGradientSel ) ;
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RWinColorPaletteDlgEx, CDialog)
	//{{AFX_MSG_MAP(RWinColorPaletteDlgEx)
	ON_WM_DESTROY( )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_LINEAR, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_LINEAR2, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_CIRCLEBURST, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_SQAUREBURST, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_INVERSE_LINEAR, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_INVERSE_LINEAR2, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_INVERSE_CIRCLEBURST, OnGradientType )
	ON_BN_CLICKED( CONTROL_RADIO_COLOR_INVERSE_SQAUREBURST, OnGradientType )
	ON_MESSAGE( UM_ANGLE_CHANGE, OnAngleChange )
	ON_MESSAGE( UM_COLOR_CHANGED, OnColorChange )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: OnInitDialog
//
// Description:   Initializes dialog, gradient button controls and creates and 
//                positions the angle control.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RWinColorPaletteDlgEx::OnInitDialog() 
{
	RWinColorPaletteDlg::OnInitDialog();

	SubclassControl( this, CONTROL_STATIC_COLOR_ANGLE, m_wndBlendAngle ) ;
	InitStyleButtons() ;
	SetGradientColors() ;
	
	// Disable angle control for square and circular burst 
	// blend options; Enable for the others if disabled.
	m_wndBlendAngle.EnableWindow( m_nGradientType < 4 ) ;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//****************************************************************************
//
// Function Name: OnOK
//
// Description:   Initializes dialog, gradient button controls and creates and 
//                positions the angle control.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorPaletteDlgEx::OnOK()
{
	CDialog::OnOK();
	AddSelectedToMRU();
}

//****************************************************************************
//
// Function Name: BuildRamp
//
// Description:   Builds a gradient ramp from using the specified start color
//                and color template.  The ramp is returned in the specified
//                ramp arguement.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorPaletteDlgEx::BuildRamp( RGradientRamp& ramp, RWHPTriple crStartColor, const RColorTemplateEntry* pTemplate, sWORD nCount, uWORD uwFlags )
{
	RGradientRampPoint      ramppt ;
	YGradientRampPointArray	arRampPoints;

	if (uwFlags & kInverted)
	{
		//
		// Build inverted ramp point arry
		//
		for (int j = 0, i = nCount - 1; i >= 0; i--, j++)
		{
			ramppt.m_color        = crStartColor + pTemplate[i].m_whpColor ; // Use the opposite color, but
			ramppt.m_pctMidPoint  = pTemplate[j].m_pctMidPoint ;				  // use the normal template mid-
			ramppt.m_pctRampPoint = pTemplate[j].m_pctRampPoint ;				  // point and ramp point.
			arRampPoints.InsertAtEnd( ramppt ) ;
		}
	}
	else
	{
		//
		// Build ramp point arry
		//
		for (int i = 0; i < nCount; i++)
		{
			ramppt.m_color        = crStartColor + pTemplate[i].m_whpColor ;
			ramppt.m_pctMidPoint  = pTemplate[i].m_pctMidPoint ;
			ramppt.m_pctRampPoint = pTemplate[i].m_pctRampPoint ;
			arRampPoints.InsertAtEnd( ramppt ) ;
		}
	}

	if (uwFlags & kDualLinear)
	{
		YGradientRampPointArrayIterator iterator = arRampPoints.Start() ;

		// Don't duplicate the last entry, as this will cause an
		// assert due to overlapping ramp points.
		for (int i = nCount - 2; i >= 0; i--)
		{
			// Add the reversed point
			ramppt = iterator[i] ;
			ramppt.m_pctRampPoint = 1.0 - ramppt.m_pctRampPoint ;
			arRampPoints.InsertAtEnd( ramppt ) ;
		}
	}

	//
	// Build the ramp
	//

	ramp.RemoveAllPts() ;

	YGradientRampPointArrayIterator iterator	= arRampPoints.Start() ;
	YGradientRampPointArrayIterator iterEnd	= arRampPoints.End() ;

	for ( ; iterator != iterEnd; ++iterator )
	{
		ramp.Add( *iterator ) ;
	}
}

//****************************************************************************
//
// Function Name: InitStyleButtons
//
// Description:   Initializes the gradient style radio button options.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorPaletteDlgEx::InitStyleButtons()
{
	RSolidColor   crColor( kWhite ) ;
	RWHPTriple    crStartColor( crColor ) ;
	RGradientRamp ramp ;

	BuildRamp( ramp, crStartColor, kSelectedToBlack, 2, 0 ) ;
	m_rbStyleArray[0].SetGradientParams( kLinear, ramp, -kPI ) ;
	m_rbStyleArray[4].SetGradientParams( kCircularBurst, ramp, 0.0 ) ;
	m_rbStyleArray[6].SetGradientParams( kSquareBurst, ramp, 0.0 ) ;

	// Now do the inverse options
	//////////////////////////////
	BuildRamp( ramp, crStartColor, kSelectedToBlack, 2, kInverted ) ;
	m_rbStyleArray[1].SetGradientParams( kLinear, ramp, -kPI ) ;
	m_rbStyleArray[5].SetGradientParams( kCircularBurst, ramp, 0.0 ) ;
	m_rbStyleArray[7].SetGradientParams( kSquareBurst, ramp, 0.0 ) ;

	// Now do the dual linear (Black to White to Black)
	///////////////////////////////////////////////////
	BuildRamp( ramp, crStartColor, kDualSelectedToBlack, 2, kDualLinear | kInverted ) ;
	m_rbStyleArray[2].SetGradientParams( kLinear, ramp, 0.0 ) ;

	// Now do the inverse dual linear (White to Black to White)
	///////////////////////////////////////////////////////////
	BuildRamp( ramp, crStartColor, kDualSelectedToBlack, 2, kDualLinear ) ;
	m_rbStyleArray[3].SetGradientParams( kLinear, ramp, 0.0 ) ;
}

//****************************************************************************
//
// Function Name: SelectedColor
//
// Description:   Returns the currently selected color.
//
// Returns:       RColor
//
// Exceptions:	   None
//
//****************************************************************************
RColor RWinColorPaletteDlgEx::SelectedColor() const
{
	return m_rbGradientArray[m_nGradientSel].GetColor();
}

//****************************************************************************
//
// Function Name: SetColor
//
// Description:   Stores the current color for use in gradient option 
//						determination, and then updates the gradient selections.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorPaletteDlgEx::SetColor( RColor crColor )
{
	if (RColor::kTransparent == crColor.GetFillMethod())
	{
		m_crColor = RSolidColor( (YPlatformColor) -1L ) ;
		RWinColorPaletteDlg::SetColor( RColor() ) ; 
	}

	else if (RColor::kGradient == crColor.GetFillMethod())
	{
		static int _arGradientIndexes[] = { -1, 2, 3, 0 } ;

		const RGradientFill* pGradientFill = crColor.GetGradientFill() ;
		const RGradientRamp*	pRamp = pGradientFill->GetRamp() ;

		// Convert the gradient type to a radio button index
		EGradientType eType = pGradientFill->GetType() ;
		m_nGradientType = _arGradientIndexes[eType] ;
		m_nGradientSel = GetTemplateIndex( crColor ) + 1 ;

		RSolidColor crStart = pRamp->GetColor( 0.0 ) ;
		RSolidColor crEnd = pRamp->GetColor( 100.0 ) ;

		if	(eType != kLinear)
		{
			RSolidColor crStart = pRamp->GetColor( 100.0 ) ;
			RSolidColor crEnd = pRamp->GetColor( 0.0 ) ;
		}

		// Determine if it is dual linear
		if (crStart == crEnd && pRamp->GetRampPointCount() > 2)
			m_nGradientType = 1 ;

		// Determine gradient type accounting for inverted options
		m_nGradientType *= 2 ;

		//
		// Determine if it is inverted.
		//
		if	(crColor.m_rgbFill != crStart)
		{
			// The inverted options for circular burst and square
			// burst are the first option, where as for linear it
			// is the second.
			if (kLinear == eType)
				m_nGradientType += 1 ;
		}
		else if (kLinear != eType)
		{
			// The normal options for circular burst and square
			// burst are the second option, where as for linear it
			// is the first.
			m_nGradientType += 1 ;
		}

		// Set the angle control
		m_wndBlendAngle.SetAngle( /*kPI*/ + pGradientFill->GetGradientAngle() ) ;	//	All angles must be reflected about y-axis

		m_crColor = crColor.m_rgbFill;
		RWinColorPaletteDlg::SetColor( RColor( m_crColor ) ) ; 
	}
	else
	{
		m_crColor = crColor.m_rgbFill;
		RWinColorPaletteDlg::SetColor( RColor( m_crColor ) ) ; 
	}

	if (GetSafeHwnd())
	{
		SetGradientColors() ;

		// Disable angle control for square and circular burst 
		// blend options; Enable for the others if disabled.
		m_wndBlendAngle.EnableWindow( m_nGradientType < 4 ) ;
	}
}

//****************************************************************************
//
// Function Name: SetGradientColors
//
// Description:   Initializes the gradient selection radio buttons, with
//                the specified color ;
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorPaletteDlgEx::SetGradientColors() 
{
	RSolidColor   crSolidColor( m_crColor ) ;
	RWHPTriple    crStartColor( crSolidColor ) ;

	if ((YPlatformColor) crSolidColor == (YPlatformColor) -1L)
	{
		//
		// If the current color is transparent, disable
		// all but the solid gradient selection.
		//
		for (int i = 1; i < 11; i++)
		{
			m_rbGradientArray[i].SetColor( RSolidColor( kDarkGray ) ) ;
			m_rbGradientArray[i].EnableWindow( FALSE ) ;
		}

		// Set the first entry with the transparent color
		m_rbGradientArray[0].SetColor( RColor() ) ;
		CheckRadioButton( CONTROL_RADIO_COLOR_GRADIENT1, CONTROL_RADIO_COLOR_GRADIENT11,
			CONTROL_RADIO_COLOR_GRADIENT1 ) ;

		return ;
	}

	//
	// Build the gradients
	//
	RGradientFill gradient ;
	RGradientRamp ramp ;

	gradient.SetType( _arGradientTypes[m_nGradientType / 2] ) ;
	gradient.SetGradientAngle( /*kPI+*/m_wndBlendAngle.GetAngle() ) ;	//	All angles  must be reflected about y-axis
	gradient.SetRamp( &ramp ) ;

	GradientTemplate* pTemplateArray = _arrTemplateArray ;

	uWORD uwFlags = 0 ;

	if (_invertedTable[m_nGradientType])
	{
		uwFlags |= kInverted ;
	}

	if (m_nGradientType / 2 == 1) 
	{
		uwFlags |= kDualLinear ;
		pTemplateArray = _arrDualTemplateArray ;
	}

	// Set the first entry, as this is always a solid
	m_rbGradientArray[0].SetColor( crSolidColor ) ;

	for (int i = 1; i < 11; i++)
	{
		BuildRamp( ramp, crStartColor, pTemplateArray[i-1].pTemplate, 
			(short) pTemplateArray[i-1].nCount, uwFlags ) ;

		m_rbGradientArray[i].SetColor( RColor( gradient ) ) ;
	}

	if (IsWindow( m_hWnd ) && !m_rbGradientArray[1].IsWindowEnabled())
	{
		//
		// Re-enable all the gradient selections
		//
		for (int i = 1; i < 11; i++)
		{
			m_rbGradientArray[i].EnableWindow( TRUE ) ;
		}
	}
}

//****************************************************************************
//
// Function Name: OnGradientType
//
// Description:   BN_CLICKED message handler for all the gradient style
//                radio buttons.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorPaletteDlgEx::OnGradientType()
{
	int nGradientType = m_nGradientType ;
	UpdateData( TRUE ) ;

	if (m_nGradientType != nGradientType)
	{
		if ((YPlatformColor) -1L != (YPlatformColor) m_crColor)
		{
			// Need to update colors if moving from or to a dual linear blend.
			if ((nGradientType / 2) == 1 || (m_nGradientType / 2) == 1)
			{
				SetGradientColors() ;
			}
			// Need to update colors if changing from inverse to normal or vice versa.
			else if (_invertedTable[m_nGradientType] != _invertedTable[nGradientType])
			{
				SetGradientColors() ;
			}
			else	// Just change the gradient type
			{
				EGradientType eType = _arGradientTypes[m_nGradientType / 2] ;

				for (int i = 1; i < 11; i++)
				{
					m_rbGradientArray[i].SetGradientType( eType ) ;
				}
			}

		} // if (!transparent)

		// Disable angle control for square and circular burst 
		// blend options; Enable for the others if disabled.
		m_wndBlendAngle.EnableWindow( m_nGradientType < 4 ) ;

	} // if (data change)
}

//****************************************************************************
//
// Function Name: OnAngleChange
//
// Description:   UM_ANGLE_CHANGE message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RWinColorPaletteDlgEx::OnAngleChange( WPARAM, LPARAM )
{
	if ((YPlatformColor) -1L != (YPlatformColor) m_crColor)
	{
		YAngle angle = m_wndBlendAngle.GetAngle() ;

		for (int i = 1; i < 11; i++)
		{
			m_rbGradientArray[i].SetGradientAngle( angle ) ;
		}
	}

	return TRUE ;
}

int RWinColorPaletteDlgEx::GetTemplateIndex( RColor& crColor )
{
	RSolidColor crBlack( kBlack ) ;

	//
	// We need to check for an unitialized m_rgbFill member, as this
	// is used to determine the start color of the gradient.  To determine if 
	// the value is unitialized, we must first see if it is used in the
	// ramp.  By being the first color, or the center color.
	//
	const RGradientFill* pGradientFill = crColor.GetGradientFill() ;
	const RGradientRamp*	pRamp = pGradientFill->GetRamp() ;
	EGradientType eType = pGradientFill->GetType() ;

	int nCount = pRamp->GetRampPointCount() ;
	RGradientRampPoint rampStart  = pRamp->GetRampPoint( 0 ) ;
	RGradientRampPoint rampEnd    = pRamp->GetRampPoint( nCount - 1 ) ;
	RGradientRampPoint rampCenter = pRamp->GetRampPoint( nCount / 2 ) ;

	uWORD uwFlags = 0 ;

	if (eType != kLinear)
	{
		// Reverse start and end colors for non-linear blends
		rampStart = pRamp->GetRampPoint( nCount - 1 ) ;
		rampEnd   = pRamp->GetRampPoint( 0 ) ;
		uwFlags  |= kInverted ;
	}

	// Determine which template array to use, and which
	// style of ramps to create for matching ramps with.
	GradientTemplate* pTemplateArray = _arrTemplateArray ;

	// The start color is really unknown, so,
	// default it to the first color in the ramp.
	crColor.m_rgbFill = rampStart.m_color ;

	// Find out if it is a dual linear blend, if it is,
	// set the dual linear blend style, and change the
	// template array to be used for generating ramps.
	if (rampStart.m_color == rampEnd.m_color && pRamp->GetRampPointCount() > 2)
	{
		pTemplateArray = _arrDualTemplateArray ;
		uwFlags |= kDualLinear ;
	}

	//
	// Try to find the template that created the color.
	//
	RWHPTriple crStartColor( crColor.m_rgbFill ) ;
	RGradientRamp ramp ;

	for ( int i = 0; i < 10; i++)
	{
		RWinColorPaletteDlgEx::BuildRamp( ramp, crStartColor, pTemplateArray[i].pTemplate, 
			(short) pTemplateArray[i].nCount, uwFlags ) ;

		if (ramp == *pRamp)
		{
			return i ;
		}
	}

	//
	// Ok, was unable to find a matching ramp, 
	// so try the inverted ramp options, with
	// the end or middle color depending on
	// wheter or not it's a dual blend.
	//
	crColor.m_rgbFill = (uwFlags & kDualLinear ? rampCenter.m_color : rampEnd.m_color) ;
	crStartColor = crColor.m_rgbFill ;
	uwFlags ^= kInverted ;

	for ( i = 0; i < 10; i++)
	{
		RWinColorPaletteDlgEx::BuildRamp( ramp, crStartColor, pTemplateArray[i].pTemplate, 
			(short) pTemplateArray[i].nCount, uwFlags ) ;

		if (ramp == *pRamp)
		{
			return i ;
		}
	}

	TpsAssertAlways( "Unable to locate template!" ) ;
	return -1 ;
}

LRESULT RWinColorPaletteDlgEx::OnColorChange( WPARAM nID, LPARAM yColor )
{
	RWinColorPaletteDlg::OnColorChange( nID, yColor );

	m_crColor = (YPlatformColor) yColor ;
	SetGradientColors();

	return 0L;
}


/////////////////////////////////////////////////////////////////////////////
// RWinColorDlg dialog

//*****************************************************************************
//
// Function Name:  RWinColorDlg
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RWinColorDlg::RWinColorDlg(CWnd* pParent, uWORD uwFlags )
{
	//{{AFX_DATA_INIT(RWinColorDlg)
	//}}AFX_DATA_INIT

	m_uwFlags = uwFlags ;
	m_pParent = pParent ;

	(RApplication::GetApplicationGlobals())->CreateColorDialog( 
		kColorDialogInterfaceId, (void **) &m_pColorInterface );
}

//*****************************************************************************
//
// Function Name:  RWinColorDlg
//
// Description:    Destructor
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RWinColorDlg::~RWinColorDlg()
{
	if (m_pColorInterface)
		m_pColorInterface->Release();
}

//****************************************************************************
//
// Function Name: ColorToWhite
//
// Description:   Builds an RColor gradient from the specified color
//                to white.
//
// Returns:       gradient from crStart to white
//
// Exceptions:	   None
//
//****************************************************************************
RColor RWinColorDlg::ColorToWhite( RSolidColor crStart, uWORD nGradientType, YAngle angle, BOOLEAN fInvert )
{
	RWHPTriple    crStartColor( crStart ) ;
	RGradientFill fill ;
	RGradientRamp ramp ;

	uWORD uwFlags = 0 ;
	EGradientType eType = (EGradientType) nGradientType ;
	const RColorTemplateEntry* pTemplate = kSelectedToWhite ;

	if (kDualLinearStyle == nGradientType)
	{
		eType = kLinear ;
		uwFlags |= kDualLinear ;
		pTemplate = kDualSelectedToWhite ;
	}

	if (fInvert)
	{
		uwFlags |= kInverted ;
	}

	RWinColorPaletteDlgEx::BuildRamp( ramp, crStartColor, pTemplate, 2, uwFlags ) ;

	fill.SetRamp( &ramp ) ;
	fill.SetType( eType ) ;
	fill.SetGradientAngle( /*kPI+*/ angle ) ;	//	this angles reflect about x-axis

	RColor crResult( fill ) ;
	crResult.m_rgbFill = crStart ;

	return crResult ;
}


//****************************************************************************
//
// Function Name: GetGradientInfo
//
// Description:   Determines the gradient information from the
//                specified color object
//
// Returns:       Nothing
//
// Exceptions:	   None
//
//****************************************************************************
void RWinColorDlg::GetGradientInfo( const RColor& crColor, uWORD& nGradientType, YAngle& nGradientAngle )
{
	// Note: GetGradientFill will cause an assert if 
	// crColor is not a gradient fill color
	const RGradientFill* fill = crColor.GetGradientFill() ;

	nGradientType  = (uWORD) fill->GetType() ;
	nGradientAngle = fill->GetGradientAngle() ;

	const RGradientRamp*	pRamp = fill->GetRamp() ;

	RSolidColor crStart = pRamp->GetColor( 0.0 ) ;
	RSolidColor crEnd   = pRamp->GetColor( 100.0 ) ;

	if (crStart == crEnd && pRamp->GetRampPointCount() > 2)
	{
		// Gradient is dual linear
		nGradientType = kDualLinearStyle ;
	}
}

void RWinColorDlg::SetGradientInfo( RColor& crColor, uWORD& nGradientType, YAngle& nGradientAngle, BOOLEAN fInvert ) 
{
	uWORD  nCurGradientType ;
	YAngle nCurGradientAngle ;

	try
	{
		RGradientFill* pGradientFill = const_cast<RGradientFill*>(crColor.GetGradientFill()) ;
		GetGradientInfo( crColor, nCurGradientType, nCurGradientAngle ) ;

		if (!fInvert && nCurGradientType == nGradientType)
		{
			// Just set the angle
			pGradientFill->SetGradientAngle( /*kPI+*/nGradientAngle ) ;
		}
		else if (!fInvert && nGradientType != kDualLinearStyle && nCurGradientType != kDualLinearStyle)
		{
			// Set style and angle
			pGradientFill->SetType( (EGradientType) nGradientType ) ;
			pGradientFill->SetGradientAngle( /*kPI+*/nGradientAngle ) ;
		}
		else
		{
			// Find out which template was used to create the color
			int  nTemplate = RWinColorPaletteDlgEx::GetTemplateIndex( crColor ) ;

			const RGradientRamp*	pRamp = pGradientFill->GetRamp() ;
			GradientTemplate* pTemplateArray = _arrTemplateArray ;
			uWORD uwFlags = 0 ;

			//
			// Find out if the gradient should be inverted.
			//
			if (crColor.m_rgbFill != pRamp->GetColor( 0.0 ))
			{
				uwFlags |= kInverted ;
			}

			//
			// Find out if the gradient should be dual linear.
			//
			if (nGradientType == kDualLinearStyle)
			{
				pTemplateArray = _arrDualTemplateArray ;
				uwFlags |= kDualLinear ;
				nGradientType = kLinear ;
			}

			//
			// Build the new color
			//
			RWHPTriple crStartColor( crColor.m_rgbFill ) ;
			RGradientFill gradient ;
			RGradientRamp ramp ;

			gradient.SetType( (EGradientType) nGradientType ) ;
			gradient.SetGradientAngle( /*kPI+*/nGradientAngle  ) ;	//	All angles  must be reflected about y-axis
			gradient.SetRamp( &ramp ) ;

			if (fInvert)
			{
				// Invert the "inverted" flag
				uwFlags ^= kInverted ;
			}

			RWinColorPaletteDlgEx::BuildRamp( ramp, crStartColor, pTemplateArray[nTemplate].pTemplate, 
				(short) pTemplateArray[nTemplate].nCount, uwFlags ) ;

			RSolidColor crStart = crColor.m_rgbFill ;
			crColor = RColor( gradient ) ;
			crColor.m_rgbFill = crStart ;

			return ;

		} 


	} // try
	catch (...)
	{
	}
}

