//****************************************************************************
//
// File Name:  WindowImageList.cpp
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description: Implements the RWindowsImageList class
//
// Portability: Windows Specific
//
// Developed by:  Broderbund Software, Inc.
//						500 Redwood Blvd.
//						Novato, CA 94948
//						(415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/WindowsImageList.cpp                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:14p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "WindowsImageList.h"
#include "BitmapLoad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RWindowsImageList::RWindowsImageList() : 
	m_szImage(0, 0)
{
	m_hbmImageList = NULL ;
	m_hbmImageMask = NULL ;

	m_hdcImageList = NULL ;
	m_hdcImageMask = NULL ;

	m_hdcImageDisabled = NULL ;
	m_hbmImageDisabled = NULL ;

	m_hbmOldImageList = NULL ;
	m_hbmOldImageMask	= NULL ;
	m_hbmOldImageDisabled = NULL ;

	m_cxImageCount = 0 ;
	m_nImageRow    = 0 ;
}

RWindowsImageList::~RWindowsImageList()
{
	DeleteObject() ;
}

void RWindowsImageList::CreateMask( int nWidth, int nHeight, COLORREF crMask )
{
	// Create the image mask bitmap and screen compatible device
	HDC     hdcImageMask = ::CreateCompatibleDC( m_hdcImageList ) ;
	HBITMAP hbmImageMask = ::CreateBitmap( nWidth, nHeight, 1, 1, NULL ) ;

	if (!hdcImageMask || !hdcImageMask)
	{
		if (hdcImageMask)
		{
			::DeleteDC( hdcImageMask ) ;
		}

		if (hbmImageMask)
		{
			::DeleteObject( hbmImageMask ) ;
		}

		AfxThrowResourceException( ) ;
	}

	m_hdcImageMask = hdcImageMask ;
	m_hbmImageMask = hbmImageMask ;

	// Select the bitmaps into the screen compatible dc
	m_hbmOldImageMask = (HBITMAP) ::SelectObject( m_hdcImageMask, m_hbmImageMask ) ;

	COLORREF crBkColor = ::SetBkColor( m_hdcImageList, crMask ) ;

	::BitBlt( m_hdcImageMask, 0, 0, nWidth, nHeight,
		m_hdcImageList, 0, 0, SRCCOPY ) ;

	::SetBkColor( m_hdcImageList, crBkColor ) ;
}

void RWindowsImageList::DeleteObject() 
{
	if (m_hdcImageList)
	{
		::SelectObject( m_hdcImageList, m_hbmOldImageList ) ;
		::DeleteObject( m_hbmImageList ) ;
		::DeleteDC( m_hdcImageList ) ;

		m_hbmOldImageList = NULL ;
		m_hbmImageList = NULL ;
		m_hdcImageList = NULL ;
	}

	if (m_hdcImageMask)
	{
		::SelectObject( m_hdcImageMask, m_hbmOldImageMask ) ;
		::DeleteObject( m_hbmImageMask ) ;
		::DeleteDC( m_hdcImageMask ) ;

		m_hbmOldImageMask = NULL ;
		m_hbmImageMask = NULL ;
		m_hdcImageMask = NULL ;
	}

	if (m_hdcImageDisabled)
	{
		::SelectObject( m_hdcImageDisabled, m_hbmOldImageDisabled ) ;
		::DeleteObject( m_hbmImageDisabled ) ;
		::DeleteDC( m_hdcImageDisabled ) ;

		m_hbmOldImageDisabled = NULL ;
		m_hbmImageDisabled = NULL ;
		m_hdcImageDisabled = NULL ;
	}

	m_szImage.cx = 0 ;
	m_szImage.cy = 0 ;
}

BOOL RWindowsImageList::LoadBitmap( UINT nID, COLORREF crMask )
{
	return LoadBitmap( MAKEINTRESOURCE (nID), crMask ) ;
}

BOOL RWindowsImageList::LoadBitmap( LPCTSTR lpszBitmapID, COLORREF crMask )
{
	if (m_hbmImageList)
	{
		DeleteObject() ;
	}

	try
	{

		HDC      hdc = ::GetWindowDC( NULL ) ;
		BITMAP   bm ;

		// Create the image list bitmap and screen compatible device
		m_hdcImageList = ::CreateCompatibleDC( hdc ) ;
		m_hbmImageList = ::LoadBitmap( AfxGetResourceHandle(), lpszBitmapID ) ;
		::ReleaseDC( NULL, hdc ) ;

		if (!m_hdcImageList || !m_hbmImageList)
		{
			AfxThrowResourceException( ) ;
		}

		// Select the bitmap into the screen compatible dc
		m_hbmOldImageList = (HBITMAP) ::SelectObject( m_hdcImageList, m_hbmImageList ) ;

		// Determine the bitmaps dimensions
		::GetObject( m_hbmImageList, sizeof( bm ), &bm ) ;

		try
		{

			if (crMask != CLR_DEFAULT)
			{
				CreateMask( bm.bmWidth, bm.bmHeight, crMask ) ;
			}

		}
		catch( CResourceException& )
		{

			// Restore the dc so all objects can be deleted
			::SelectObject( m_hdcImageList, m_hbmOldImageList ) ;
			m_hbmOldImageList = NULL ;

			throw ;
		}

		// Save the bitmaps dimensions
		m_szImage.cx   = bm.bmWidth ;
		m_szImage.cy   = bm.bmHeight ;

		// Save the mask color
		m_crMask = crMask ;

	}
	catch( CResourceException& )
	{
		
		if (m_hbmImageList)
		{
			::DeleteObject( m_hbmImageList ) ;
			m_hbmImageList = NULL ;
		}

		if (m_hdcImageList)
		{
			::DeleteDC( m_hdcImageList ) ;
			m_hdcImageList = NULL ;
		}

		return FALSE ;
	}

	return TRUE ;
}

BOOL RWindowsImageList::LoadImageList( UINT nBitmapID, int cx, COLORREF crMask ) 
{
	return LoadImageList( MAKEINTRESOURCE( nBitmapID ), cx, crMask ) ;
}

BOOL RWindowsImageList::LoadImageList( LPCTSTR lpszBitmapID, int cx, COLORREF crMask ) 
{
	if (!LoadBitmap( lpszBitmapID, crMask ))
	{
		return FALSE ;
	}

	m_cxImageCount = WORD( m_szImage.cx / cx ) ;
	m_szImage.cx   = cx ;

	return TRUE ;
}

BOOL RWindowsImageList::LoadImageList( UINT nBitmapID, int cx, int cy, COLORREF crMask ) 
{
	return LoadImageList( MAKEINTRESOURCE( nBitmapID ), cx, cy, crMask ) ;
}

BOOL RWindowsImageList::LoadImageList( LPCTSTR lpszBitmapID, int cx, int cy, COLORREF crMask ) 
{
	if (!LoadBitmap( lpszBitmapID, crMask ))
	{
		return FALSE ;
	}

	m_cxImageCount = WORD( (m_szImage.cx / cx) * (m_szImage.cy / cy) ) ;
	m_szImage.cx   = cx ;
	m_szImage.cy   = cy ;

	return TRUE ;
}

BOOL RWindowsImageList::Draw( HDC hdc, CPoint pt, int nIndex, UINT uiFlags )
{
	if (uiFlags & kDisabled)
	{
		return DrawDisabled( hdc, pt, nIndex ) ;
	}

	CDC   dc ;
	dc.Attach( hdc ) ;

	if (uiFlags & kTransparent)
	{
		CDC   dcMem ;
		dcMem.CreateCompatibleDC( &dc ) ;

		CBitmap bitmap ;
		bitmap.CreateCompatibleBitmap( &dc, m_szImage.cx, m_szImage.cy ) ;

		CBitmap* pBitmap = dcMem.SelectObject( &bitmap ) ;
		
		::BitBlt( dcMem, 0, 0, m_szImage.cx, m_szImage.cy,
			m_hdcImageList, nIndex * m_szImage.cx, 
			m_nImageRow * m_szImage.cy, SRCCOPY ) ;

		COLORREF crBkColor = dcMem.SetBkColor( RGB( 0, 0, 0 ) ) ;
		COLORREF crTxColor = dcMem.SetTextColor( RGB( 255, 255, 255 ) ) ;

		::BitBlt( dcMem, 0, 0, m_szImage.cx, m_szImage.cy,
			m_hdcImageMask, nIndex * m_szImage.cx, 
			m_nImageRow * m_szImage.cy, SRCAND ) ;

		dcMem.SetBkColor( crBkColor ) ;
		dcMem.SetTextColor( crTxColor ) ;

		crBkColor = dc.SetBkColor( RGB( 255, 255, 255 ) ) ;
		crTxColor = dc.SetTextColor( RGB( 0, 0, 0 ) ) ;

		::BitBlt( dc, pt.x, pt.y, m_szImage.cx, m_szImage.cy,
			m_hdcImageMask, nIndex * m_szImage.cx, 
			m_nImageRow * m_szImage.cy, SRCAND ) ;

		dc.SetBkColor( crBkColor ) ;
		dc.SetTextColor( crTxColor ) ;

		dc.BitBlt( pt.x, pt.y, m_szImage.cx, m_szImage.cy,
			&dcMem, 0, 0, SRCINVERT ) ;

		dcMem.SelectObject( pBitmap ) ;
	}
	else
	{
		::BitBlt( dc, pt.x, pt.y, m_szImage.cx, m_szImage.cy,
			m_hdcImageList, nIndex * m_szImage.cx, 
			m_nImageRow * m_szImage.cy, SRCCOPY ) ;
	}

	if (uiFlags & kFocused)
	{
		// Determine and draw the focus rect.
		CRect rect(pt.x + 1, pt.y + 2, pt.x + m_szImage.cx - 2, pt.y + m_szImage.cy - 1) ;
		dc.DrawFocusRect( rect ) ;
	}

	dc.Detach() ;

	return TRUE ;
}

#pragma warning(disable:4100)
BOOL RWindowsImageList::DrawDisabled( HDC hdc, CPoint pt, int nIndex )
{
	if (!m_hdcImageDisabled)
	{
		BITMAP bm ;
		::GetObject( m_hbmImageList, sizeof( bm ), &bm ) ;

		// Create the image mask bitmap and screen compatible device
		HDC     hdcImageDisabled = ::CreateCompatibleDC( hdc ) ;
		HBITMAP hbmImageDisabled = ::CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL ) ;

		HDC     hdcTemp = ::CreateCompatibleDC( hdc ) ;
		HBITMAP hbmTemp = ::CreateBitmap( bm.bmWidth, bm.bmHeight, bm.bmPlanes, 
			bm.bmBitsPixel, NULL ) ;

		if (!hdcImageDisabled || !hbmImageDisabled || !hdcTemp || !hbmTemp)
		{
			if (hdcImageDisabled) ::DeleteDC( hdcImageDisabled ) ;
			if (hbmImageDisabled) ::DeleteObject( hbmImageDisabled ) ;
			if (hdcTemp) ::DeleteDC( hdcTemp ) ;
			if (hbmTemp) ::DeleteObject( hbmTemp ) ;

			return FALSE ;
		}

		HBITMAP hbmOldTemp = (HBITMAP) ::SelectObject( hdcTemp, hbmTemp ) ;

		::BitBlt( hdcTemp, 0, 0, bm.bmWidth, bm.bmWidth,
			m_hdcImageList, 0, 0, SRCCOPY ) ;

		COLORREF crBkColor = SetBkColor( hdcTemp, RGB( 255, 255, 255 ) ) ;
		COLORREF crTxColor = SetTextColor( hdcTemp, RGB( 0, 0, 0 ) ) ;

		::BitBlt( hdcTemp, 0, 0, bm.bmWidth, bm.bmWidth,
			m_hdcImageMask, 0, 0, SRCPAINT ) ;

		m_hdcImageDisabled = hdcImageDisabled ;
		m_hbmImageDisabled = hbmImageDisabled ;

		m_hbmOldImageDisabled = (HBITMAP) ::SelectObject( m_hdcImageDisabled, 
			m_hbmImageDisabled ) ;

		COLORREF crBkColor2 = SetBkColor( hdcImageDisabled, RGB( 0, 0, 0 ) ) ;
		COLORREF crTxColor2 = SetTextColor( hdcImageDisabled, RGB( 255, 255, 255 ) ) ;

		::BitBlt( m_hdcImageDisabled, 0, 0, bm.bmWidth, bm.bmWidth,
			hdcTemp, 0, 0, SRCCOPY ) ;

		SetBkColor( m_hdcImageDisabled, crBkColor2 ) ;
		SetTextColor( m_hdcImageDisabled, crTxColor2 ) ;

		SetBkColor( hdcTemp, crBkColor ) ;
		SetTextColor( hdcTemp, crTxColor ) ;

		// Clean up temporary bitmap and dc
		::SelectObject( hdcTemp, hbmOldTemp ) ;
		::DeleteObject( hbmTemp ) ;
		::DeleteDC( hdcTemp ) ;
	}

	HBRUSH hBrush = ::CreateSolidBrush( GetSysColor( COLOR_BTNHILIGHT ) ) ;
	HBRUSH hOldBrush = (HBRUSH) ::SelectObject( hdc, hBrush ) ;

	COLORREF crBkColor = SetBkColor( hdc, RGB( 0, 0, 0 ) ) ;
	COLORREF crTxColor = SetTextColor( hdc, RGB( 255, 255, 255 ) ) ;

	::BitBlt( hdc, pt.x + 1, pt.y + 1, m_szImage.cx, m_szImage.cy,
		m_hdcImageDisabled, nIndex * m_szImage.cx, 
		m_nImageRow * m_szImage.cy, 0xE20746L ) ;

	::SelectObject( hdc, hOldBrush ) ;
	::DeleteObject( hBrush ) ;

	hBrush = ::CreateSolidBrush( GetSysColor( COLOR_BTNSHADOW ) ) ;
	hOldBrush = (HBRUSH) ::SelectObject( hdc, hBrush ) ;

	::BitBlt( hdc, pt.x, pt.y, m_szImage.cx, m_szImage.cy,
		m_hdcImageDisabled, nIndex * m_szImage.cx, 
		m_nImageRow * m_szImage.cy, 0xE20746L ) ;

	SetBkColor( hdc, crBkColor ) ;
	SetTextColor( hdc, crTxColor ) ;

	::SelectObject( hdc, hOldBrush ) ;
	::DeleteObject( hBrush ) ;

	return TRUE ;
}

CSize RWindowsImageList::GetBitmapDimensions( ) const
{
	return m_szImage ;
}

WORD RWindowsImageList::GetImageCount() const
{
	return m_cxImageCount ;
}

WORD RWindowsImageList::SetImageRow( WORD nNewRow ) 
{
	WORD nOldRow = m_nImageRow ;
	m_nImageRow  = nNewRow ;

	return nOldRow ;
}


///////////////////////////////////////////////////////////////////

#include "OffscreenDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RBitmapImage()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImageList::RBitmapImageList() : RBitmapImage(),
	m_szImage( 0, 0 ) 
{
	m_fMask        = FALSE ;
	m_uwGrow       = 5 ;
	m_ulImageCount = 0 ;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RBitmapImage()
//
//  Description:		Destructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImageList::~RBitmapImageList()
{
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::Create()
//
//  Description:		(Re)creates the bitmap list with the given 
//							attributes
//
//  Returns:			TRUE if successful; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RBitmapImageList::Create( int cx, int cy, uLONG ulBitDepth, BOOLEAN fMask, uWORD nInitial, uWORD nGrow )
{
	try
	{
		Initialize( cx * nInitial, cy, ulBitDepth ) ;

		if (fMask)
		{
			m_bmMask.Initialize( cx * nInitial, cy, 1 ) ;
		}

		m_fMask = fMask ;
		m_uwGrow = nGrow ;
		m_szImage.m_dx = cx ;
		m_szImage.m_dy = cy ;
	}
	catch (YException)
	{
		return FALSE ;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::Create()
//
//  Description:		Initializes the RBitmapImageList to be compatible with 
//							the YImageHandle.  Does not take ownership of the 
//                   resource	bitmap.
//
//  Returns:			TRUE if successful; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RBitmapImageList::Create( YResourceId yBitmapID, int cx, uWORD nGrow, YPlatformColor crMask ) 
{
	// FIX: change to use the resource manager
	HBITMAP hBitmap = ::Load256Bitmap( yBitmapID ) ;

	if (!hBitmap)
	{
		return FALSE ;
	}

	try
	{
		Initialize( (YImageHandle) hBitmap ) ;

		if (CLR_NONE != crMask)
		{
			uLONG ulWidth  = GetWidthInPixels() ;
			uLONG ulHeight = GetHeightInPixels() ;

			// Initialize temporary device-dependent bitmap
			HBITMAP hbmMask =	CreateBitmap( ulWidth, ulHeight, 1, 1, NULL) ;
			
			if (!hbmMask)
			{
				throw kMemory ;
			}

			HDC hdc = GetDC( NULL ) ;
			HDC hdcSrc = CreateCompatibleDC( hdc ) ;
			HDC hdcDst = CreateCompatibleDC( hdc ) ;
			ReleaseDC( NULL, hdc ) ;

			HBITMAP hOldSrc = (HBITMAP) SelectObject( hdcSrc, hBitmap ) ;
			HBITMAP hOldDst = (HBITMAP) SelectObject( hdcDst, hbmMask ) ;

			COLORREF crBkColor = SetBkColor( hdcSrc, crMask ) ;
			BitBlt( hdcDst, 0, 0, ulWidth, ulHeight, hdcSrc, 0, 0, NOTSRCCOPY ) ;
			SetBkColor( hdcSrc, RGB( 0, 0, 0 ) ) ;

			SelectObject( hdcSrc, hOldSrc ) ;
			SelectObject( hdcDst, hOldDst ) ;
			DeleteDC( hdcSrc ) ;
			DeleteDC( hdcDst ) ;

			// Initialize the mask bitmap image
			m_bmMask.Initialize( (YImageHandle) hbmMask ) ;
			DeleteObject( hbmMask ) ;

			m_fMask = TRUE ;
		}

		m_szImage.m_dx = cx ;
		m_szImage.m_dy = GetHeightInPixels() ;

		// Determine the image count
		m_ulImageCount = GetWidthInPixels() / cx ;

	}
	catch( YException )
	{
		// FIX: change to use the resource manager
		DeleteObject( hBitmap ) ;

		return FALSE ;
	}

	// FIX: change to use the resource manager
	DeleteObject( hBitmap ) ;

	return TRUE ;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::Create()
//
//  Description:		Initializes the RBitmapImageList to be compatible with 
//							the YImageHandle.  Does not take ownership of the 
//                   resource	bitmap.
//
//  Returns:			TRUE if successful; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RBitmapImageList::Create( YResourceId yBitmapID, int cx, int cy, uWORD nGrow, YPlatformColor crMask )
{
	if (Create( yBitmapID, cx, nGrow, crMask ))
	{
		// Update the image height
		m_szImage.m_dy = cy ;

		// Determine the image count
		m_ulImageCount *= GetHeightInPixels() / cy ;

		return TRUE ;
	}

	return FALSE ;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::Add()
//
//  Description:		Adds one or more images or an icon to the image 
//                   list.
//
//  Returns:			Zero-based index of the first new image if 
//                   successful; otherwise -1.
//
//  Exceptions:		None
//
// ****************************************************************************
int RBitmapImageList::Add( RBitmapImage* pbmImage, RBitmapImage* pbmMask )
{
	int nImageIndex = m_ulImageCount ;

	if (!m_ulImageCount)
	{
		// Copy the palette of the incoming image
	}

	RIntRect rcDestRect(0, 0, 
		pbmImage->GetWidthInPixels(), 
		pbmImage->GetHeightInPixels()) ;

	RIntSize szOffset((m_ulImageCount - 1) * m_szImage.m_dx, 0) ;
	rcDestRect.Offset( szOffset ) ;

	while (GetWidthInPixels() < (uLONG) rcDestRect.m_Right)
	{
		uLONG cx = GetWidthInPixels() + m_uwGrow * m_szImage.m_dx ;
		uLONG cy = GetHeightInPixels() ;

		// Resize the bitmaps
//		Resize( cx, cy ) ;
		
		if (m_fMask)
		{
//			m_bmMask.Resize( cx, cy ) ;
		}
	}

	ROffscreenDrawingSurface dsMem ;

	dsMem.SetImage( (RBitmapImage*) this ) ;
	pbmImage->Render( dsMem, rcDestRect ) ;
	dsMem.ReleaseImage() ;

	if (m_fMask)
	{
		dsMem.SetImage( &m_bmMask ) ;
		pbmMask->Render( dsMem, rcDestRect ) ;
		dsMem.ReleaseImage() ;
	}

	// Update the image count
	m_ulImageCount += pbmImage->GetWidthInPixels() / m_szImage.m_dx ;

	return nImageIndex ;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::Add()
//
//  Description:		Adds one or more images or an icon to the image 
//                   list.
//
//  Returns:			Zero-based index of the first new image if 
//                   successful; otherwise -1.
//
//  Exceptions:		None
//
// ****************************************************************************
int RBitmapImageList::Add( RBitmapImage* pbmImage, YPlatformColor crMask )
{
	if (m_fMask)
	{
		RBitmapImage bmMask ;

		try
		{
			uLONG ulWidth  = pbmImage->GetWidthInPixels() ;
			uLONG ulHeight = pbmImage->GetHeightInPixels() ;

			bmMask.Initialize( ulWidth, ulHeight, 1 ) ;

			ROffscreenDrawingSurface dsDest ;
			ROffscreenDrawingSurface dsSrc;

			dsDest.SetImage( &bmMask ) ;

			dsSrc.SetImage( pbmImage ) ;
			dsSrc.SetFontColor( RSolidColor( !crMask ) ) ;
			dsSrc.SetFillColor( RSolidColor( crMask ) ) ;

			RIntRect rcRect( 0, 0, ulWidth, ulHeight ) ;
			dsDest.BlitDrawingSurface( dsSrc, rcRect, rcRect, kBlitSourceCopy ) ;

			dsSrc.ReleaseImage() ;
			dsDest.ReleaseImage() ;

			return Add( pbmImage, &bmMask ) ;
		}
		catch( YException )
		{
			return -1 ;
		}
	}

	return Add( pbmImage, (RBitmapImage*) NULL ) ;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::Render()
//
//  Description:		Renders the specified image onto the drawing
//							surface int the specified style.
//
//  Returns:			TRUE if successful; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RBitmapImageList::Render( RDrawingSurface& ds, int nImage, RIntRect rcDestRect, uLONG ulStyle )
{
	if ((uLONG) nImage < 0 || (uLONG) nImage > m_ulImageCount)
	{
		return FALSE ;
	}

	int nCols = GetWidthInPixels() / m_szImage.m_dx ;

	RIntSize szOffset( 
		nImage % nCols * m_szImage.m_dx, 
		nImage / nCols * m_szImage.m_dy ) ;

	RIntRect rcSrcRect( 0, 0, m_szImage.m_dx, m_szImage.m_dy ) ;
	rcSrcRect.Offset( szOffset ) ;

	if (!m_fMask)
	{
		// If there is no mask, the bitmap will always be drawn normal
		RBitmapImage::Render( ds, rcSrcRect, rcDestRect ) ;
	}
	else
	{
		//
		// Render the bitmap in the appropriate style
		//
		switch (ulStyle)
		{
		case kNormal:
			RBitmapImage::Render( ds, rcSrcRect, rcDestRect ) ;
			break ;

		case kTransparent:
			RBitmapImage::RenderWithMask( ds, m_bmMask, rcSrcRect, rcDestRect ) ;
			break ;

		default:
			TpsAssertAlways( "Not yet implemented!" ) ;
		}
	}

	return TRUE ;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::GetImageSize()
//
//  Description:		Determines the dimensions of the images in the list.
//
//  Returns:			Dimensions of images.
//
//  Exceptions:		None
//
// ****************************************************************************
RIntSize	RBitmapImageList::GetImageSize()
{
	return m_szImage ;
}

// ****************************************************************************
//
//  Function Name:	BitmapImageList::GetImageCount()
//
//  Description:		Determines the number of images in the list.
//
//  Returns:			Number of images.
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG	RBitmapImageList::GetImageCount()
{
	return m_ulImageCount ;
}

