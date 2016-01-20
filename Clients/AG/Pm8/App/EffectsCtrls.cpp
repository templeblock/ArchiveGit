//****************************************************************************
//
// File Name:		EffectsCtrls.cpp
//
// Project:			Renaissance Application Component
//
// Author:			Lisa Wu
//
// Description:		Implementation for custom controls related to image effects 
//					typed dialog
//
// Portability:		Win32
//
// Developed by:	Broderbund Software
//						500 Redwood Blvd
//						Novato, CA 94948
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//****************************************************************************


//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "EffectsCtrls.h"
#include "BitmapLoad.h"


#include "ScratchBitmapImage.h"
#include "DcDrawingSurface.h"
#include "OffscreenDrawingSurface.h"
#include "ComponentView.h"
#include "ComponentDocument.h"
#include "Cursor.h"
#include "FrameworkResourceIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//****************************************************************************
//
// Function Name: RBmpCtrl::RBmpCtrl
//
// Description:   Constructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RBmpCtrl::RBmpCtrl():
	m_pRBitmap	(NULL)
{
}

//****************************************************************************
//
// Function Name: RBmpCtrl::~RBmpCtrl
//
// Description:   Destructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RBmpCtrl::~RBmpCtrl()
{
	if (m_pRBitmap)
		delete m_pRBitmap;
}


BEGIN_MESSAGE_MAP(RBmpCtrl, CStatic)
	//{{AFX_MSG_MAP(RBmpCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//****************************************************************************
//
// Function Name: RBmpCtrl::SetResourceBmp
//
// Description:   Creates a bitmap if necessary and initialize using resource id
//
// Returns:       BOOL 
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RBmpCtrl::SetResourceBmp( int resID )
{
	if (!m_pRBitmap)
		m_pRBitmap = new RBitmapImage;
	
	m_pRBitmap->Initialize(static_cast<YResourceId>(resID));	

	return TRUE;
}


//****************************************************************************
//
// Function Name: RBmpCtrl::OnPaint
//
// Description:   Handles the paint message
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
void RBmpCtrl::OnPaint()
{
	
	// Render the image into the device rect:
	CPaintDC cDC(this);

	// Erase the area behind the image
	CRect cRect;
	GetClientRect(&cRect);
	cRect.InflateRect(1, 1);
	
	// Initialize the drawing surface
	RDcDrawingSurface rDS;
	if (!rDS.Initialize(cDC.m_hDC, cDC.m_hDC)) return;
	
	HRGN hClip = NULL;
	ROffscreenDrawingSurface rODS;
	try
	{
		// Create an RBitmapImage compatible with the drawing surface
		RScratchBitmapImage rTempBitmap;
		rTempBitmap.Initialize(rDS, cRect.Width(), cRect.Height());		
		
		// Set the bitmap into an offscreen DS		
		rODS.SetImage(&rTempBitmap);		
				
		// Clear the background
		rODS.SetFillColor(RSolidColor(::GetSysColor(COLOR_3DFACE)));
		rODS.FillRectangle(RIntRect(cRect.left, cRect.top, cRect.right + 1, cRect.bottom + 1));

		// If we have an image, render it
		if (m_pRBitmap)
		{
			TpsAssertValid( m_pRBitmap );

			// Set a clip region around the image rect
			hClip = ::CreateRectRgn(cRect.left, cRect.top, cRect.right, cRect.bottom);
			if (hClip) VERIFY(::SelectClipRgn((HDC)rODS.GetSurface(), hClip));

			// Draw a white rect in case we have a metafile
			rODS.SetFillColor(RSolidColor(kWhite));
			rODS.FillRectangle(RIntRect(cRect));

			// Render the image
			m_pRBitmap->Render(rODS, RIntRect(cRect));

		}

		// Clean up the clip region
		if (rODS.GetSurface()) ::SelectClipRgn((HDC)rODS.GetSurface(), NULL);
		if (hClip) VERIFY(::DeleteObject(hClip));
		hClip = NULL;

		// Release the offscreen bitmap
		rODS.ReleaseImage();

		// Render the image
		rTempBitmap.Render(rDS, RIntRect(cRect));
	}
	catch(YException)
	{	
		// Clean up the clip region
		if (rODS.GetSurface()) ::SelectClipRgn((HDC)rODS.GetSurface(), NULL);
		if (hClip) VERIFY(::DeleteObject(hClip));
		hClip = NULL;

		// Release the offscreen bitmap
		rODS.ReleaseImage();

		// For some reason, we couldn't use an offscreen, so just draw directly to the screen			
		// Clear the background
		rDS.SetFillColor(RSolidColor(::GetSysColor(COLOR_3DFACE)));
		rDS.FillRectangle(RIntRect(cRect.left, cRect.top, cRect.right + 1, cRect.bottom + 1));
		if (m_pRBitmap)
		{
			// Render the image
			m_pRBitmap->Render(rDS, RIntRect(cRect));		
		}
	}

	rDS.DetachDCs();	
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// RPresetGridCtrl 

//****************************************************************************
//
// Function Name: RPresetGridCtrl::RPresetGridCtrl
//
// Description:   Constructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RPresetGridCtrl::RPresetGridCtrl() :
		RGridCtrl<HBITMAP>	(kIntegral),
		m_bHasNames			(FALSE)
{
}

//****************************************************************************
//
// Function Name: RPresetGridCtrl::~RPresetGridCtrl
//
// Description:   Destructor 
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
RPresetGridCtrl::~RPresetGridCtrl()
{
	Unload();
}

//****************************************************************************
//
// Function Name: RPresetGridCtrl::Unload
//
// Description:   Clear out the bitmaps and strings
//
// Returns:       Nothing 
//
// Exceptions:	  None
//
//****************************************************************************
void RPresetGridCtrl::Unload()
{
	HBITMAP hBitmap ;

	for (int i = 0; i < GetCount(); i++)
	{
		GetItemData( i, hBitmap ) ;
		::DeleteObject( hBitmap ) ;
		
	}

	ResetContent();

	if (m_nameStrArray.GetSize())
		m_nameStrArray.RemoveAll();

}
//****************************************************************************
//
// Function Name: RPresetGridCtrl::LoadBitmaps
//
// Description:   Loads the ReadyMade bitmaps into the control 
//
// Returns:       TRUE if successful; otherwise FALSE. 
//
// Exceptions:	   None
//
//****************************************************************************
BOOLEAN RPresetGridCtrl::LoadBitmaps( int nBeginID, int nEndID )
{
	HBITMAP hBitmap  ;
	SetWindowLong( m_hWnd, GWL_STYLE, GetStyle() & ~LBS_SORT ) ;

	for (int i = nBeginID; i <= nEndID; i++)
	{
		hBitmap = ::Load256Bitmap( i ) ;

		if (!hBitmap)
		{
			return FALSE ;
		}
		
		AddItem( hBitmap ) ;
	}

	return TRUE ;
}

//****************************************************************************
//
// Function Name: RPresetGridCtrl::SetData
//
// Description:   Loads the string resource into string array
//
// Returns:       Nothing
//
// Exceptions:	   None
//
//****************************************************************************
void RPresetGridCtrl::SetData(int nBeginID, int nEndID)
{
	//
	// Setup the font information
	//
	LOGFONT lf ;
	CFont* pFont = GetFont() ;
	pFont->GetLogFont( &lf ) ;

	m_font.info.attributes = 0;
	m_font.info.height     = lf.lfHeight ;
	m_font.info.width      = abs( lf.lfWidth ) ;
	m_font.info.angle		  = 0;
	m_font.info.ulPadding  = 0;
	strcpy( m_font.info.sbName, lf.lfFaceName );

	// clear out everything first
	if (m_nameStrArray.GetSize())
		m_nameStrArray.RemoveAll();

	for (int i = nBeginID; i <= nEndID; i++)
	{	
		CString	resStr;
		// get the string out of the resource
		resStr.LoadString(i);
		// add it into the array
		m_nameStrArray.Add(resStr);
	}
	m_bHasNames = TRUE;
}

//****************************************************************************
//
// Function Name: RPresetGridCtrl::DrawItem
//
// Description:   Draws the specified bitmap, in the specified
//                state, in the specified area.
//
// Returns:       Nothing 
//
// Exceptions:	   None
//
//****************************************************************************
void RPresetGridCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// what item are we drawing?
	int nIndex = lpDrawItemStruct->itemID;

	// don't do anything
	if (LB_ERR == nIndex)
		return ;

	// build some rects
	CRect rcCellRect( lpDrawItemStruct->rcItem ) ;
	RIntRect rcTextRect( rcCellRect ) ;

	CDC dc ;
	dc.Attach( lpDrawItemStruct->hDC ) ;

	CDC dcMem ;
	dcMem.CreateCompatibleDC( &dc ) ;

	BITMAP bm ;
	HBITMAP hBitmap ;
	GetItemData( lpDrawItemStruct->itemID, hBitmap ) ;
	GetObject( hBitmap, sizeof( bm ), &bm ) ;
	
	int nFontHeight = abs((int)m_font.info.height);

	RIntRect rcDestRect( 0, 0, bm.bmWidth - 1, bm.bmHeight - 1 ) ;
	rcDestRect.CenterRectInRect( RIntRect( rcCellRect ) ) ;
	if (m_bHasNames)
	{
		rcDestRect.Offset(RRealSize(0, -nFontHeight));
	}

	COLORREF  crFillColor = RGB( 0xFF, 0xFF, 0xFF );// always white //GetSysColor( COLOR_WINDOW ) ;
	COLORREF  crFontColor = GetSysColor( COLOR_WINDOWTEXT ) ;

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		crFillColor = GetSysColor( COLOR_HIGHLIGHT ) ;
		crFontColor = GetSysColor( COLOR_HIGHLIGHTTEXT ) ;
	}

	switch (lpDrawItemStruct->itemAction)
	{
		case ODA_DRAWENTIRE:


			hBitmap = (HBITMAP) dcMem.SelectObject( hBitmap ) ;
			
			dc.BitBlt( rcDestRect.m_Left, rcDestRect.m_Top, bm.bmWidth, bm.bmHeight,
				&dcMem, 0, 0, SRCCOPY ) ;
			
			rcCellRect = rcDestRect ;
			rcCellRect.InflateRect( 1, 1 ) ;

			// Clean up.
			dcMem.SelectObject( hBitmap ) ;

			if (!(lpDrawItemStruct->itemState & ODS_SELECTED))
			{
				break ;
			}

			// Fall through

		case ODA_SELECT:
			// draw the selection rect
			rcDestRect.Inflate( RIntSize(2, 2) ) ;

			CPen pen(PS_SOLID, 2, crFillColor) ;
			CPen *pOldPen = dc.SelectObject( &pen ) ;

			dc.MoveTo( rcDestRect.m_Left,	rcDestRect.m_Bottom	) ;
			dc.LineTo( rcDestRect.m_Left,	rcDestRect.m_Top	) ;
			dc.LineTo( rcDestRect.m_Right,	rcDestRect.m_Top 	) ;
			dc.LineTo( rcDestRect.m_Right,	rcDestRect.m_Bottom	) ;
			dc.LineTo( rcDestRect.m_Left,	rcDestRect.m_Bottom	) ;

			dc.SelectObject( pOldPen ) ;

			break;

	}
	dc.Detach() ;

	if (m_bHasNames && m_nameStrArray.GetSize() > 0)
	{
		RDcDrawingSurface ds;
		ds.Initialize(lpDrawItemStruct->hDC, lpDrawItemStruct->hDC);
		R2dTransform transform ;

		ds.SetFont( m_font.info ) ;	

		RColor crOldFill = ds.GetFillColor() ;

		// get the name out of the member array
		RMBCString	 strText(m_nameStrArray[nIndex]) ;
		uWORD uwTextOptions = kPlaceTextCenterV ;

		//
		// Determine the text rectangle as this is used for
		// all states of the drawing code. 
		//
		RIntSize	 szExtent = ds.GetActualTextSize( 
			strText.GetStringLength(), (uBYTE*)(LPCSZ) strText ) ;

		RRealSize szOffset( 
			rcTextRect.m_Left + rcTextRect.Width() / 2 - szExtent.m_dx / 2,
			rcTextRect.m_Bottom - (nFontHeight * 1.5)) ;

		// Update the text rect to display the text 
		// at the bottom of the cell.
		rcTextRect.m_Left   = -2 ;
		rcTextRect.m_Top    = -2 ;
		rcTextRect.m_Right  = szExtent.m_dx + 2 ;
		rcTextRect.m_Bottom = szExtent.m_dy + 2 ;
		rcTextRect.Offset( RIntSize( szOffset ) );

		uwTextOptions = kPlaceTextCenterV | kPlaceTextCenterH ;

/*		if (lpDrawItemStruct->itemAction != ODA_FOCUS)
		{
			rcDestRect.Inflate( RIntSize( -(int) nFontHeight, -(int) nFontHeight ) ) ;
			rcDestRect.m_Bottom -= nFontHeight + 1 ;
		}
*/
		if (lpDrawItemStruct->itemAction != ODA_FOCUS)
		{
			//
			// Draw the text...
			//
			RColor crOldFont = ds.GetFontColor() ;
			RRealRect rcRect( (CRect) rcTextRect ) ;

			ds.SetFillColor( crFillColor ) ;
			ds.SetFontColor( crFontColor ) ;
			ds.FillRectangle( rcTextRect ) ;

			ds.PlaceTextInRect( rcRect, strText, transform, uwTextOptions ) ;
			ds.SetFontColor( crOldFont ) ;
		}
		
/*		if (lpDrawItemStruct->itemAction == ODA_FOCUS ||
			 lpDrawItemStruct->itemState   & ODS_FOCUS)
		{
			ds.SetFillColor( crFillColor ) ;
			ds.DrawFocusRect( rcTextRect ) ;
		}
*/
		ds.SetFillColor( crOldFill ) ;
		ds.DetachDCs() ;
	}
}

//****************************************************************************
//
// Function Name: RPresetGridCtrl::MeasureItem
//
// Description:   Determines the size of the specified item.
//
// Returns:       Nothing
//
// Exceptions:	   None
//
//****************************************************************************
void RPresetGridCtrl::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	HBITMAP hBitmap ;
	GetItemData( lpMeasureItemStruct->itemID, hBitmap ) ;

	BITMAP bm ;
	GetObject( hBitmap, sizeof( bm ), &bm ) ;
   lpMeasureItemStruct->itemWidth  = bm.bmWidth ;    // width of menu item, in pixels 
   lpMeasureItemStruct->itemHeight = bm.bmHeight ;   // height of single item in list box, in pixels 
}

// ****************************************************************************
//
//  Function Name:	RPresetGridCtrl::GetGridBackgroundColor()
//
//  Description:	Accessor
//
//  Returns:		The background color of this grid
//
//  Exceptions:		None
//
// ****************************************************************************
//
COLORREF RPresetGridCtrl::GetGridBackgroundColor() const
{
	// use white always
	return RGB( 0xFF, 0xFF, 0xFF );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// RShadowGlowControlDoc 

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlDoc::RShadowGlowControlDoc( )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RShadowGlowControlDoc::RShadowGlowControlDoc( )
{
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlDoc::~RShadowGlowControlDoc( )
//
//  Description:	Destructor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RShadowGlowControlDoc::~RShadowGlowControlDoc( )
{
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlDoc::CreateView( )
//
//  Description:	create a control view for this document, subclassing the
//                   given window...
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView* RShadowGlowControlDoc::CreateView( CDialog* pDialog, int nControlId, int theType )
{
	return new RShadowGlowControlView( pDialog, nControlId, this, theType );
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlDoc::GetMinObjectSize( )
//
//  Description:	Accessor
//
//  Returns:		Min object size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RShadowGlowControlDoc::GetMinObjectSize( const YComponentType&, BOOLEAN )
{
	// Lets limit the user to 1/4 inch
	const RRealSize kMinGroupedObjectSize( ::InchesToLogicalUnits( 0.25 ), ::InchesToLogicalUnits( 0.25 ) );

	return kMinGroupedObjectSize;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// RShadowGlowControlView 


// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::RShadowGlowControlView( CDialog* pdlg, int nctlid )
//
//  Description:	Constructor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RShadowGlowControlView::RShadowGlowControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument, int type )
	: RControlView( pDialog, nControlId, pDocument ),
	  m_nControlViewType( type ), 
	  m_pDialog			( pDialog ),
	  m_bMovingShadow	( FALSE ),
	  m_bEnableMouse	( FALSE ),
	  m_pImageView		( NULL ),
	  m_minOffset		( 0 ),
	  m_maxOffset		( 0 )
{
	
	// find the center of this view
	CRect tempControlRect;
	GetCWnd( ).GetClientRect( &tempControlRect );
	RRealRect controlRect( tempControlRect );
	ScreenUnitsToLogicalUnits( controlRect );
	m_centerPoint = controlRect.GetCenterPoint();
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::~RShadowGlowControlView( )
//
//  Description:	Destructor
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RShadowGlowControlView::~RShadowGlowControlView( )
{
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::EnableMouseEvents( )
//
//  Description:	Allow mouse drag and stuff here?
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RShadowGlowControlView::EnableMouseEvents( BOOLEAN enableMouse)
{
	m_bEnableMouse = enableMouse;
}

//*****************************************************************************
//
// Function Name:  RShadowGlowControlView::Render
//
// Description:    Render our own little drag rect if necessary
//
// Returns:        Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
//
void RShadowGlowControlView::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	RControlView::Render(drawingSurface, transform, rcRender);
	
}
// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::CalcAngle( )
//
//  Description:	Calculates the angle in radians in the Cartesian system,
//					e.g., 0 degrees points to the right.
//
//  Returns:		YAngle
//
//  Exceptions:		None
//
// ****************************************************************************

YAngle	RShadowGlowControlView::CalcShadowAngle( const RRealPoint& pt )
{
	YAngle angle = ::atan2( ( m_centerPoint.m_y - pt.m_y ), ( pt.m_x - m_centerPoint.m_x ) );

	// Ensure the angle is between 0 and 2PI.  Can't use the modulus operator
	// because the type is double.
	while ( angle > k2PI )
		angle -= k2PI;
	
	// angle is less than 0, then 2 pi - (-the angle)
	if ( angle < 0.F )
		angle += k2PI;

	int nDegrees = ::Round( ::RadiansToDegrees( angle ) );

	angle = k2PI - angle;	// flip it over to shadow setting's way of life
	return angle;
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::CalcShadowOffset( )
//
//  Description:	calculate the percentage offset of the shadow
//
//  Returns:		YAngle
//
//  Exceptions:		None
//
// ****************************************************************************
YPercentage RShadowGlowControlView::CalcShadowOffset( const RRealPoint& pt )
{
	YRealDimension		length = m_centerPoint.Distance( pt );

	// anything less or equal to min return 0 percent
	if ( length <= m_minOffset)
		return 0;

	// anything greater than or equal to max, return 100 percent
	else if (length >= m_maxOffset)
		return 1.0;

	// somewhere within range?
	else if ( length > m_minOffset && length < m_maxOffset)
	{
		// subtract to get the distance from min
		length -= m_minOffset;

		// ratio of that over the difference should be the percentage
		return ( length / (m_maxOffset - m_minOffset) );
	}
	else
		return 0;	// shouldn't hit here
}
// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::OnXLButtonDown( )
//
//  Description:	hande mouse down
//
//					set up drag rect
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RShadowGlowControlView::OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	if (!m_bEnableMouse)
		return;

	// get the control document
	RShadowGlowControlDoc* pControlDoc =  static_cast<RShadowGlowControlDoc*>( GetRDocument( ) );
	TpsAssertValid( pControlDoc );
	
	// get the component document
	RComponentDocument* pComponentDocument = pControlDoc->GetComponentByIndex( 0 );
	TpsAssertValid( pComponentDocument );

	m_pImageView = static_cast<RComponentView*>(pComponentDocument->GetActiveView( ));
	TpsAssertValid( m_pImageView );

	m_pImageView->GetShadowSettings(m_curShadowSetting);
	// make the shadow visible
	m_curShadowSetting.m_fShadowOn = TRUE;

	if (m_minOffset == 0)	// only need to calculate this once
	{
		// get the minimum and max offset based on min dimention of component's dimension
		YComponentBoundingRect rBoundsRect = m_pImageView->GetBoundingRect();
		YRealDimension minDimension	= ::Min( rBoundsRect.Height(), rBoundsRect.Width() );
		m_minOffset = kMinShadowOffset * minDimension;
		m_maxOffset = kMaxShadowOffset * minDimension;
	}

	// Set the mouse capture
	SetMouseCapture( );
	m_bMovingShadow = TRUE;

}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::OnXMouseMove( )
//
//  Description:	handle move, move the dragging rect
//
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RShadowGlowControlView::OnXMouseMove( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	if (!m_bEnableMouse)
		return;

	::GetCursorManager( ).SetCursor( CURSOR_RESOURCE_MOVE );

			// are we dragging the shadow around, is there a component view?
	if (m_bMovingShadow && m_pImageView)
	{		
		m_pImageView->Invalidate();

		// get the angle and offset based on current mouse point
		m_curShadowSetting.m_fShadowAngle = CalcShadowAngle(mousePoint);
		m_curShadowSetting.m_fShadowOffset = CalcShadowOffset(mousePoint);
		
		// set it into the component
		m_pImageView->SetShadowSettings( m_curShadowSetting );

		m_pImageView->Invalidate();

	}

}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::OnXLButtonUp( )
//
//  Description:	handle mouse up, remove drag rect
//
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RShadowGlowControlView::OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	if (!m_bEnableMouse)
		return;

			// are we dragging the shadow around, is there a component view?
	if (m_bMovingShadow && m_pImageView)
	{		
		m_pImageView->Invalidate();

		// get the angle and offset based on current mouse point
		m_curShadowSetting.m_fShadowAngle = CalcShadowAngle(mousePoint);
		m_curShadowSetting.m_fShadowOffset = CalcShadowOffset(mousePoint);
		
		// set it into the component
		m_pImageView->SetShadowSettings( m_curShadowSetting );

		m_pImageView->Invalidate();

	}

	m_bMovingShadow = FALSE;
	m_pImageView = NULL;

	// Release the mouse capture
	ReleaseMouseCapture( );

}
// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::OnXRButtonDown( )
//
//  Description:	Default handler for the right button down XEvent.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RShadowGlowControlView::OnXRButtonDown( const RRealPoint& , YModifierKey )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::GetBackgroundColor( )
//
//  Description:		Accessor
//
//  Returns:			The background color of this view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RColor RShadowGlowControlView::GetBackgroundColor( ) const
{
	// default color for this control view is always white
	RColor backgroundColor = RSolidColor( kWhite );

	RShadowGlowControlDoc* pControlDoc =  static_cast<RShadowGlowControlDoc*>( GetRDocument( ) );
	RComponentDocument* pComponentDocument = pControlDoc->GetComponentByIndex( 0 );
	RComponentView* pImageView = static_cast<RComponentView*>(pComponentDocument->GetActiveView( ));
	if (pImageView == NULL)
		return backgroundColor;
	
	RSoftShadowSettings shadowSetting;
	RSoftGlowSettings	glowSetting;
	RSolidColor			curEffectColor;
	YPercentage			opacityPercentage = 0;

	if (m_nControlViewType == kAllPurposeType)
		return backgroundColor;
	
	else if (m_nControlViewType == kShadowType)
	{
		pImageView->GetShadowSettings( shadowSetting );

		if (shadowSetting.m_fShadowOn)
		{
			curEffectColor = shadowSetting.m_fShadowColor;
			opacityPercentage = shadowSetting.m_fShadowOpacity;
		}
	}
	else if (m_nControlViewType == kGlowType)
	{
		pImageView->GetGlowSettings( glowSetting );
		
		if (glowSetting.m_fGlowOn)
		{
			curEffectColor = glowSetting.m_fGlowColor;
			opacityPercentage = glowSetting.m_fGlowOpacity;
		}
	}

	// if the effect is white too, then change to 
	// background color to black
	if (backgroundColor.GetSolidColor() == curEffectColor)
	{
		return RColor( kBlack );
	}

	return backgroundColor;
}

// ****************************************************************************
//
//  Function Name:	RShadowGlowControlView::RenderSurfaceAdornments( )
//
//  Description:		Render any "surface adornments" such as desktop, panel
//							surface, background color/gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RShadowGlowControlView::RenderSurfaceAdornments( RComponentView* /* pView */, RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	RenderBackground( drawingSurface, transform, rcRender );
}

