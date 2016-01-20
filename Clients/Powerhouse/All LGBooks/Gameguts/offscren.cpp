#include <windows.h>
#include "proto.h"
#include "wing.h"

//************************************************************************
COffScreen::COffScreen(HPALETTE hPal)
//************************************************************************
{
	m_pdibClean = NULL; // no READONLY dib yet
	m_hbmOldMonoBitmap = NULL;
	m_hbmWinG = NULL;
	m_lpSwitches = NULL;
	m_hDC = WinGCreateDC();
	m_hPal = hPal;
	m_iTransition = 0;
	m_nStepSize = 0;
	m_nTicks = -1;
	m_fCreatedPal = FALSE;

	if ( !(m_pdibStage = new CDib()) )
		return;

	//	Get WinG to recommend the fastest DIB format
	//	Orientation = ( m_pdibStage->m_bmiHeader.biHeight < 0 ? -1 : 1 ); 
	if ( !WinGRecommendDIBFormat( m_pdibStage->GetInfo() ) )
	{ // set it up ourselves
		m_pdibStage->SetPlanes(1);
		m_pdibStage->SetSizeImage(0);
		m_pdibStage->SetColorsUsed(0);
		m_pdibStage->SetColorsImportant(0);
		m_pdibStage->SetWidth(1);
		m_pdibStage->SetHeight(1);
	}
	// make sure it's 8bpp
	m_pdibStage->SetBitCount(8);
	m_pdibStage->SetCompression(BI_RGB);

	m_pdibStage->FixHeader();
}

//************************************************************************
COffScreen::~COffScreen()
//************************************************************************
{
	if ( m_hDC )
	{
		// Replace with the original bitmap
		if ( m_hbmOldMonoBitmap )
			SelectObject( m_hDC, m_hbmOldMonoBitmap );

		// delete the WinGBitmap
		if (m_hbmWinG)
			DeleteObject(m_hbmWinG);
		// delete the WinG DC
		DeleteDC(m_hDC);
	}

	// if we created a palette to be used with this
	// offscreen, then delete it
	if (m_hPal && m_fCreatedPal)
		DeleteObject(m_hPal);

	// delete the writable stage dib
	if ( m_pdibStage )
	{
		m_pdibStage->SetPtr(NULL); // They aren't our bits to delete
		delete m_pdibStage;
	}

	// delete the read-only dib
	if ( m_pdibClean )
		delete m_pdibClean;

	// delete any memory allocated for switches
	if ( m_lpSwitches )
		FreeUp( (LPTR)m_lpSwitches );
}

//************************************************************************
void COffScreen::SetTransition( int iTransition, int nTicks, int nStepSize )
//************************************************************************
{
	m_iTransition = iTransition;
	m_nStepSize = nStepSize;
	m_nTicks = nTicks;
}

//************************************************************************
void COffScreen::DrawRect( HDC hDC, LPRECT lpRect, LPPOINT lpDstPt )
//************************************************************************
{
	RECT rRepair;
	POINT ptDst;
	HPALETTE hOldPal;
	int iTransition, nStepSize, nTicks;
	#define IS_FULL_RECT(rect,dib) ( \
		rect.left	== 0 && \
		rect.top	== 0 && \
		rect.right	== (int)(dib)->GetWidth() && \
		rect.bottom == (int)abs((dib)->GetHeight()) )

	if ( !hDC || !m_hDC || !m_pdibStage )
		return;

	// RealizePalette() doesn't need to be called for every paint if the app
	// realizes it when it gets a WM_QUERYNEWPALETTE (I think).	 I'm trying to minimize
	// the number of times the one and only application palette is realized.
	if ( m_hPal )
		{
		hOldPal = SelectPalette( hDC, m_hPal, /*bProtectPhysicalPal*/
			(BOOL)FindClassDescendent(GetApp()->GetMainWnd(), "video") );
		RealizePalette( hDC );
		}
	else
		hOldPal = NULL;

	// Don't repair more than the original DIB size
	rRepair = *lpRect;
	if ( rRepair.left < 0 )
		 rRepair.left = 0;
	if ( rRepair.top  < 0 )
		 rRepair.top  = 0;
	if ( rRepair.right	> (int)m_pdibStage->GetWidth() )
		 rRepair.right	= (int)m_pdibStage->GetWidth();
	if ( rRepair.bottom > (int)abs(m_pdibStage->GetHeight()) )
		 rRepair.bottom = (int)abs(m_pdibStage->GetHeight());
	
	if (lpDstPt)
		ptDst = *lpDstPt;
	else
	{
		ptDst.x = rRepair.left;
		ptDst.y = rRepair.top;
	}

	SpeedCheck( this, m_pdibStage );

	if (m_iTransition)
	{
		iTransition = m_iTransition;
		nStepSize = m_nStepSize;
		nTicks = m_nTicks;
	}
	else
	{
		iTransition = (int)GetSwitchValue( 't', m_lpSwitches );
		nTicks = -1;
		nStepSize = 0;
	}
	BOOL bNoWinG = (BOOL)GetSwitchValue( 'n', m_lpSwitches );

	// Only perform transition blts on a full screen draw
	if ( !IS_FULL_RECT(rRepair, m_pdibStage) )
		iTransition = 0;

	if ( bNoWinG )
	{
		if ( iTransition )
			TransitionBlt( iTransition, hDC,
				ptDst.x, ptDst.y,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, m_pdibStage, NULL,
				nTicks, nStepSize );
		else
			m_pdibStage->DCBlt( hDC,
				ptDst.x, ptDst.y,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top );
	}
	else
	{
		if ( iTransition )
			TransitionBlt( iTransition, hDC,
				ptDst.x, ptDst.y,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
				rRepair.left, rRepair.top,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top, m_pdibStage, m_hDC,
				nTicks, nStepSize );
		else
			WinGBitBlt( hDC,
				ptDst.x, ptDst.y,
				rRepair.right - rRepair.left, rRepair.bottom - rRepair.top,
				m_hDC,
				rRepair.left, rRepair.top);
	}

	if ( hOldPal )
		SelectPalette( hDC, hOldPal, TRUE/*bProtectPhysicalPal*/ );
	
	#ifdef WIN32
	GdiFlush();
	#endif
}

//************************************************************************
void COffScreen::Resize( int dx, int dy )
//************************************************************************
{
	int Orientation;
	HBITMAP hbm;
	LPTR lp;

	if ( !m_hDC || !m_pdibStage )
		return;

	if ( (dx == m_pdibStage->GetWidth()) && (abs(dy) == abs(m_pdibStage->GetHeight())) )
		return;

	// Create a new 8-bit WinGBitmap with the new size
	// while maintaining the same y orientation
	Orientation = ( m_pdibStage->GetHeight() < 0 ? -1 : 1 ); 
	m_pdibStage->SetHeight(dy * Orientation);
	m_pdibStage->SetWidth(dx);
	m_pdibStage->SetSizeImage(0);
	m_pdibStage->FixHeader();
 
	// Create a WinG Bitmap, store the data pointer, then select away
	if (m_hbmWinG)
		DeleteObject(m_hbmWinG);
	if ( !(m_hbmWinG = WinGCreateBitmap( m_hDC,
		(LPBITMAPINFO)m_pdibStage, (LPPVOID)&lp )) )
		return;
	m_pdibStage->SetPtr(lp);

	// Select it in and delete the old one
	hbm = (HBITMAP)SelectObject( m_hDC, m_hbmWinG );
	if ( !m_hbmOldMonoBitmap )
		// Store the old hbitmap to select back in before deleting
		// only save it the first time this function is called
		m_hbmOldMonoBitmap = hbm;
}

//************************************************************************
void COffScreen::Load( LPSTR lpFileName ) 
//************************************************************************
{
	// Check for optional switches
	if (!m_lpSwitches)
		m_lpSwitches = ExtractSwitches( lpFileName );

	if ( m_pdibClean )
		delete m_pdibClean;
	if (m_fCreatedPal && m_hPal)
	{
		DeleteObject(m_hPal);
		m_hPal = NULL;
	}

	m_pdibClean = CDib::LoadDibFromFile( lpFileName, m_hPal );
	if (m_pdibClean && (m_hPal == NULL))
	{
		m_hPal = CreateIdentityPalette(m_pdibClean->GetColors(),
									   m_pdibClean->GetNumColors());
		m_fCreatedPal = m_hPal != NULL;
		if (m_hPal)
			m_pdibClean->MapToPalette(m_hPal);
	}
}

//************************************************************************
void COffScreen::Load( HINSTANCE hInstance, int idResource, LPSTR lpSwitchString ) 
//************************************************************************
{
	if (lpSwitchString) // Check for optional switches
	{
		if (m_lpSwitches)
			FreeUp(m_lpSwitches);
		m_lpSwitches = ExtractSwitches( lpSwitchString );
	}

	if ( m_pdibClean )
		delete m_pdibClean;

	m_pdibClean = CDib::LoadDibFromResource( hInstance, MAKEINTRESOURCE(idResource), m_hPal );
	if (m_pdibClean && (m_hPal == NULL))
	{
		m_hPal = CreateIdentityPalette(m_pdibClean->GetColors(),
									   m_pdibClean->GetNumColors());
		m_fCreatedPal = m_hPal != NULL;
		if (m_hPal)
			m_pdibClean->MapToPalette(m_hPal);
	}
}

//************************************************************************
void COffScreen::CopyColors( void ) 
//************************************************************************
{ // Copy the clean color table to the offscreen coolor table
	if ( !m_pdibClean || !m_pdibStage )
		return;

	// copy the READONLY DIB color table to the WRITABLE DIB color table
	RGBQUAD FAR *lpSrcColors = m_pdibClean->GetColors();
	RGBQUAD FAR *lpDstColors = m_pdibStage->GetColors();
	for ( int i=0; i<256; i++ )
		lpDstColors[i] = lpSrcColors[i];
}


//************************************************************************
void COffScreen::CopyBits( LPRECT lpRect  ) 
//************************************************************************
{ // Copy the clean bits to the offscreen
	if ( !m_hDC || !m_pdibStage ) // no destination
		return;

	RECT ClipRect;
	SetRect( &ClipRect, 0, 0, m_pdibStage->GetWidth(), abs(m_pdibStage->GetHeight()) );
	if ( lpRect )
		IntersectRect( &ClipRect/*dst*/, lpRect, &ClipRect );

	if ( !m_pdibClean ) // no source
	{ // so a solid color into the offscreen before leaving
		FillRect( m_hDC, &ClipRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
		return;
	}

	BOOL bScaled   = (BOOL)GetSwitchValue( 's', m_lpSwitches );
	BOOL bCentered = (BOOL)GetSwitchValue( 'c', m_lpSwitches );

	// Setup the coordinates for the copy
	RECT srcRect, dstRect;
	if ( bScaled )
	{ // if we want to scale the DIB (up or down)
		srcRect.left = 0; // centering is meaningless, with scaling
		dstRect.left = 0;
		dstRect.right = m_pdibStage->GetWidth();
		srcRect.right = m_pdibClean->GetWidth();
		srcRect.top = 0;
		dstRect.top = 0;
		dstRect.bottom = abs(m_pdibStage->GetHeight());
		srcRect.bottom = abs(m_pdibClean->GetHeight());
	}
	else
	{
		if ( !bCentered )
			{ // drawn top-left
			srcRect.left = 0;
			dstRect.left = 0;
			srcRect.top = 0;
			dstRect.top = 0;
			}
		else
			{ // drawn centered
			srcRect.left = ( m_pdibClean->GetWidth() - m_pdibStage->GetWidth() ) / 2;
			if ( srcRect.left < 0 )
				{ // source is smaller than the destination
				dstRect.left = -srcRect.left;
				srcRect.left = 0;
				}
			else
				dstRect.left = 0;
			srcRect.top = ( abs(m_pdibClean->GetHeight()) - abs(m_pdibStage->GetHeight()) ) / 2;
			if ( srcRect.top < 0 )
				{ // source is smaller than the destination
				dstRect.top = -srcRect.top;
				srcRect.top = 0;
				}
			else
				dstRect.top = 0;
			}

		dstRect.right = dstRect.left + min( m_pdibStage->GetWidth(), m_pdibClean->GetWidth() );
		srcRect.right = dstRect.right;
		dstRect.bottom = dstRect.top + min( abs(m_pdibStage->GetHeight()), abs(m_pdibClean->GetHeight()) );
		srcRect.bottom = dstRect.bottom;
	}

	// Draw a solid color outside of the blt
	// top
	RECT rect;
	SetRect( &rect, 0, 0, m_pdibStage->GetWidth(), dstRect.top );
	IntersectRect( &rect/*dst*/, &rect, &ClipRect );
	FillRect( m_hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	// bottom
	SetRect( &rect, 0, dstRect.right, m_pdibStage->GetWidth(), abs(m_pdibStage->GetHeight()) );
	IntersectRect( &rect/*dst*/, &rect, &ClipRect );
	FillRect( m_hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	// left
	SetRect( &rect, 0, dstRect.top, dstRect.left, dstRect.bottom );
	IntersectRect( &rect/*dst*/, &rect, &ClipRect );
	FillRect( m_hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	// right
	SetRect( &rect, dstRect.right, dstRect.top, m_pdibStage->GetWidth(), dstRect.bottom );
	IntersectRect( &rect/*dst*/, &rect, &ClipRect );
	FillRect( m_hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );

	// since GDI drawing happens in a different thread we must make sure that
	// the FillRect happens before we change the bits in the WinG dib.	If this
	// is not done, the FillRect() happens after we change the WinG dib so our
	// bits always get overwritten by GDI.
	#ifdef WIN32
	GdiFlush();
	#endif

	IntersectRect( &srcRect/*dst*/, &srcRect, &ClipRect );
	IntersectRect( &dstRect/*dst*/, &dstRect, &ClipRect );

	if ( bScaled ||
		 m_pdibClean->GetCompression() == BI_RLE8 ||
		   m_pdibStage->GetCompression() == BI_RLE8 )
	{ // compressed DIBs must use GDI copying (lose transparency ability)
		m_pdibClean->DCBlt( m_hDC,	/* dst */
			dstRect.left, dstRect.top, dstRect.right, dstRect.bottom,
			srcRect.left, srcRect.top, srcRect.right, srcRect.bottom );
	}
	else
	{
		m_pdibClean->DibBlt( m_pdibStage, /* dst */
			dstRect.left, dstRect.top, dstRect.right, dstRect.bottom,
			srcRect.left, srcRect.top, srcRect.right, srcRect.bottom,
			NO /*bTransparent*/ );
	}

	if ( !GetSwitchValue( 'a', m_lpSwitches ) )
	{ // if we don't need a clean bitmap around anymore (kind of a copy once flag)
		delete m_pdibClean;
		m_pdibClean = NULL;
	}
}


//************************************************************************
PDIB COffScreen::GetReadOnlyDIB(void) 
//************************************************************************
{
return( m_pdibClean );
}

//************************************************************************
PDIB COffScreen::GetWritableDIB(void) 
//************************************************************************
{
return( m_pdibStage );
}

//************************************************************************
HDC COffScreen::GetDC(void) 
//************************************************************************
{
return( m_hDC );
}
