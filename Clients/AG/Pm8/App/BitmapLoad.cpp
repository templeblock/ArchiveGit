#include "stdafx.h"
ASSERTNAME

#include "BitmapLoad.h"

/////////////////////////////////////////////////////////////////////////////
// HBITMAP Load256Bitmap( LPCTSTR )
//
// @func This function accepts a LPCTSTR identifier to load a bitmap from
//       the current application. In order to munge the palette, the
//       LoadBitmap API can not be used. This function loads the resource
//       with the LoadResource API and locks the resource in memory. 
//       A RT_BITMAP locked from the resource file is accessible as a DIB
//       which has palette information available. A copy of the locked
//       DIB is made in the application's stack space.
//
//        Note that this function only works for bitmap resources
//        with a color depth of 4 or 8 bpp.
//
HBITMAP Load256Bitmap( UINT nID )
{
	return Load256Bitmap( MAKEINTRESOURCE(nID) ); 
}

HBITMAP Load256Bitmap( LPCTSTR lpBitmapName )
{
	HBITMAP		hBitmap;

	BITMAPINFOHEADER    * lpbih;    // pointer to original bitmap
	HRSRC                 hrsrc;    // handle returned from FindResource
	HGLOBAL               hglobal;  // handle returned from LoadResource
	void                * lpbits;
	int						 nPalSize;

	if (lpBitmapName == NULL) return NULL;

	hrsrc   = FindResource(AfxGetResourceHandle(), lpBitmapName, RT_BITMAP);
	if (hrsrc == NULL) return NULL;

	hglobal = LoadResource(AfxGetResourceHandle(), hrsrc );
	if (hglobal == NULL) return NULL;

	lpbih   = (LPBITMAPINFOHEADER) LockResource( hglobal );

	if (lpbih == NULL)
	{
		FreeResource(hglobal);
		throw(kResource);
		return NULL;
	}

	nPalSize = (lpbih->biClrUsed == 0) ? 256 : lpbih->biClrUsed;
	lpbits   = (BYTE *)lpbih + nPalSize * sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER); 
	
	// Get a compatible DC to create an HBITMAP
	HWND    hWnd = ::GetDesktopWindow();
	HDC     hdcDeskTop = ::GetDC(hWnd);

	HPALETTE		hPalette = (HPALETTE)tbitGetScreenPalette();
	HPALETTE		hPalOld	= ::SelectPalette(hdcDeskTop, hPalette, FALSE);
	::RealizePalette(hdcDeskTop);

	try
	{
		 // Create the HBITMAP
		 hBitmap = CreateCompatibleBitmap( hdcDeskTop, 
													  lpbih->biWidth, 
													  lpbih->biHeight );

		 if (!hBitmap)
			 throw(kResource);

		 // Populate the HBITMAP with the bits from the "new" DIB
		 SetDIBits(  hdcDeskTop,
						 hBitmap,
						 0,
						 lpbih->biHeight,
						 lpbits,
	(BITMAPINFO *)  lpbih,
						 DIB_RGB_COLORS );
	}
	catch( ... )
	{
		;
	}

	if ( hPalOld )
		::SelectPalette(hdcDeskTop, hPalOld, FALSE);
	 // Clean up other allocated resources
	 ReleaseDC(hWnd, hdcDeskTop);
	 FreeResource( hglobal );

    return hBitmap;
}

/////////////////////////////////////////////////////////////////////////////
//
//	void SetButtonBitmap()
//
// 	This function loads the bitmap resource designated by the passed resource ID
//	and sets it as the bitmap for a button.
//
void SetButtonBitmap( CWnd* pParent, UINT nCtlID, UINT nBitmapID )
{
	CWnd* pWnd = pParent->GetDlgItem( nCtlID );

	if (pWnd)
	{
		HBITMAP hBitmap = ::Load256Bitmap( nBitmapID );
		if ( hBitmap )
		{
			((CButton *) pWnd)->SetBitmap( hBitmap );

			// resize button to bitmap size, leaving a 2-pixel margin
			BITMAP bm;
			::GetObject( hBitmap, sizeof( bm ), &bm );
//			CRect rect( 0, 0, bm.bmWidth - 1, bm.bmHeight - 1 );
			CRect rect( 0, 0, bm.bmWidth + 4, bm.bmHeight + 4 );

			::AdjustWindowRect( rect, pWnd->GetStyle(), FALSE );

			pWnd->SetWindowPos( NULL, 0, 0, rect.Width(), rect.Height(),
				SWP_NOZORDER | SWP_NOMOVE );
		}
	}
}
