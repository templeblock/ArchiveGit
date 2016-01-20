#include <windows.h>
#include "proto.h"
#include "wing.h"
#include "memory.h"
#include "dibfx.h"

#ifdef WIN32
	#define _huge		   
#endif

#define BFT_ICON	0x4349	/* 'IC' */
#define BFT_BITMAP	0x4d42	/* 'BM' */
#define BFT_CURSOR	0x5450	/* 'PT' */

/* flags for _lseek */
#define	 SEEK_CUR 1
#define	 SEEK_END 2
#define	 SEEK_SET 0

//************************************************************************
CDib::CDib()
//************************************************************************
{
	m_bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_bmiHeader.biWidth			= 1;
	m_bmiHeader.biHeight		= 1;
	m_bmiHeader.biPlanes		= 1;
	m_bmiHeader.biBitCount		= 8;
	m_bmiHeader.biCompression	= BI_RGB;
	m_bmiHeader.biSizeImage		= 0;
	m_bmiHeader.biXPelsPerMeter = 0;
	m_bmiHeader.biYPelsPerMeter = 0;
	m_bmiHeader.biClrUsed		= 0;
	m_bmiHeader.biClrImportant	= 0;
	_fmemset(m_rgbQuad, 0, sizeof(m_rgbQuad));
	m_lp = NULL;
	m_iRefCount = 0;
	AddRef();
}

//************************************************************************
CDib::CDib(PDIB pdib, LPTR lp, BOOL fCopyBits)
//************************************************************************
{
	m_bmiHeader = *pdib->GetInfoHeader();
	hmemcpy( m_rgbQuad, pdib->GetColors(), sizeof(m_rgbQuad));
	m_lp = lp;
	if (fCopyBits)
		hmemcpy(m_lp, pdib->GetPtr(), pdib->GetSizeImage());
	m_iRefCount = 0;
	AddRef();
}

//************************************************************************
CDib::CDib(LPBITMAPINFOHEADER lpbmiHeader, LPRGBQUAD lpColors, LPTR lp)
//************************************************************************
{
	m_bmiHeader = *lpbmiHeader;
	hmemcpy( m_rgbQuad, lpColors, sizeof(m_rgbQuad));
	m_lp = lp;
	m_iRefCount = 0;
	AddRef();
}

//************************************************************************
CDib::~CDib()
//************************************************************************
{
	--m_iRefCount;
	if (m_iRefCount > 0)
		Debug("%d references still exist for object %lx", m_iRefCount, (long)this);
	if (m_lp)
		FreeUp(m_lp);
}

//************************************************************************
int CDib::AddRef()
//************************************************************************
{
	++m_iRefCount;
	return(m_iRefCount);
}

//************************************************************************
int CDib::Release()
//************************************************************************
{
	if (m_iRefCount <= 1)
	{
		int iRefCount = m_iRefCount-1;
		delete this;
		return(iRefCount);
	}
	else
	{
		--m_iRefCount;
		return(m_iRefCount);
	}
}

//************************************************************************
PDIB CDib::GetDib(BOOL fDecodeRLE)
//************************************************************************
{
	if ((GetCompression() == BI_RLE8) && fDecodeRLE)
	{	
		DWORD dwSize = (long)GetWidthBytes() * (long)abs(GetHeight());
		LPTR lp = AllocX(dwSize, GMEM_ZEROINIT );
		if (!lp)
			return(NULL);
		PDIB pDib = new CDib(this, lp, FALSE);
		if (!pDib)
		{
			FreeUp(lp);
			return(NULL);
		}
		DecodeRLE(lp);
		pDib->SetCompression(BI_RGB);
		pDib->SetSizeImage(dwSize);
		return(pDib);
	}
	else
	{
		AddRef();
		return(this);
	}
}

//************************************************************************
PDIB CDib::LoadDibFromFile(LPCSTR lpFileName, HPALETTE hPal, BOOL bMapToLevels, BOOL fDecodeRLE, BOOL bBrowse)
//************************************************************************
{
	PDIB pdib = new CDib();
	if (!pdib)
		return(NULL);
	if (!pdib->LoadFromFile(lpFileName, fDecodeRLE, bBrowse))
	{
		delete pdib;
		return(NULL);
	}
	if (bMapToLevels)
	{
		pdib->ConvertToLevels();
	}
	else
	if (hPal)
	{
		PDIB pHTdib;
		if ( pHTdib = pdib->HalftoneDIB( hPal ) )
		{
			delete pdib;
			pdib = pHTdib;
		}
		else
			pdib->MapToPalette( hPal );
	}
	return(pdib);
}
																  
//************************************************************************
BOOL CDib::LoadFromFile(LPCSTR lpFileName, BOOL fDecodeRLE, BOOL bBrowse )
//************************************************************************
{
	FNAME szExpFileName;
	if ( !GetApp()->FindContent( (LPSTR)lpFileName, szExpFileName, bBrowse ) )
		return(FALSE);

	HFILE fh;
	OFSTRUCT of;
	if ( (fh = OpenFile(szExpFileName, &of, OF_READ)) < 0 )
		return(FALSE);
			
	if ( !(ReadBitmapInfo(fh)) )
		return FALSE;

	// Can we get enough memory to hold the DIB bits
	DWORD dwBits;
	if ( !(m_lp = AllocX( dwBits = GetSizeImage(), GMEM_ZEROINIT )) )
		{
		_lclose(fh);
		return( FALSE );
		}

	// read in the bits
	_hread( fh, m_lp, dwBits );
	_lclose( fh );

	return(CheckDecodeRLE(fDecodeRLE));
}

/***********************************************************************/
PDIB CDib::LoadDibFromResource(HINSTANCE hInstance, LPCSTR lpResource, HPALETTE hPal, BOOL bMapToLevels, BOOL fDecodeRLE)
/***********************************************************************/
{
	PDIB pdib = new CDib();
	if (!pdib)
		return(NULL);
	if (!pdib->LoadFromResource(hInstance, lpResource, fDecodeRLE))
	{
		delete pdib;
		return(NULL);
	}
	if (bMapToLevels)
	{
		pdib->ConvertToLevels();
	}
	else
	if (hPal)
	{
		PDIB pHTdib;
		if ( pHTdib = pdib->HalftoneDIB( hPal ) )
		{
			delete pdib;
			pdib = pHTdib;
		}
		else
			pdib->MapToPalette( hPal );
	}
	return(pdib);
}

//************************************************************************
BOOL CDib::LoadFromResource(HINSTANCE hInstance, LPCSTR szResource, BOOL fDecodeRLE)
//************************************************************************
{
	HRSRC hDibRes;
	HGLOBAL hDib;
	LPTR lpDib;
	LPBITMAPINFO lpbmInfo; 
	DWORD dwBits;

	if ( !(hDibRes = FindResource(hInstance, szResource, RT_BITMAP)) )
		return(FALSE);
	if (! (hDib = LoadResource(hInstance, hDibRes)) )
		return(FALSE);
	lpDib = (LPTR)LockResource(hDib);
	if (!lpDib)
		return(FALSE);
	lpbmInfo = (LPBITMAPINFO)lpDib;
	m_bmiHeader = lpbmInfo->bmiHeader;
	FixHeader();
	hmemcpy(m_rgbQuad, lpbmInfo->bmiColors, GetNumColors() * sizeof(RGBQUAD));
	lpDib += m_bmiHeader.biSize + (GetNumColors() * sizeof(RGBQUAD));

	// Can we get enough memory to hold the DIB bits
	if ( (m_lp = AllocX( dwBits = GetSizeImage(), GMEM_ZEROINIT )) != NULL )
		hmemcpy(m_lp, lpDib, dwBits);
	UnlockResource(hDib);
	FreeResource(hDib);

	return(CheckDecodeRLE(fDecodeRLE));
}

//************************************************************************
BOOL CDib::WriteFile( LPCSTR lpFileName )
//************************************************************************
{
	BITMAPFILEHEADER	hdr;
	HFILE				fh;
	OFSTRUCT			of;
	DWORD				dwSize, dwDibSize;

	if ( (fh = OpenFile( lpFileName, &of, OF_CREATE|OF_READWRITE )) < 0 )
		return FALSE;

	dwSize = GetSizeImage();

	dwDibSize = sizeof(BITMAPINFOHEADER) + (256*sizeof(RGBQUAD));
	hdr.bfType		= BFT_BITMAP;
	hdr.bfSize		= (DWORD)sizeof(BITMAPFILEHEADER) + dwDibSize + dwSize;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits	= (DWORD)sizeof(BITMAPFILEHEADER) + dwDibSize;

	_lwrite( fh, (LPCSTR)(LPVOID)&hdr, sizeof(BITMAPFILEHEADER) );
	_hwrite( fh, (LPCSTR)(LPVOID)&m_bmiHeader, sizeof(m_bmiHeader) );
	_hwrite( fh, (LPCSTR)(LPVOID)&m_rgbQuad, sizeof(m_rgbQuad) );
	_hwrite( fh, (LPCSTR)(LPVOID)m_lp, dwSize );
	_lclose( fh );
	return TRUE;
}

//	Will read a file in DIB format and return a global HANDLE to its
//	BITMAPINFO.	 This function will work with both "old" and "new"
//	bitmap formats, but will always return a "new" BITMAPINFO
//************************************************************************
BOOL CDib::ReadBitmapInfo( HFILE fh )
//************************************************************************
{
	DWORD off, size;
	HANDLE hbi = NULL;
	int i, nNumColors;
	LPRGBQUAD pRgb;
	BITMAPINFOHEADER bi;
	BITMAPCOREHEADER bc;
	BITMAPFILEHEADER bf;

	if ( fh < 0 )
		return FALSE;

	off = _llseek(fh,0L,SEEK_CUR);

	if (sizeof(bf) != _lread(fh,(LPSTR)&bf,sizeof(bf)))
		return FALSE;

	// do we have a RC HEADER?
	if (bf.bfType != BFT_BITMAP)
		{
		bf.bfOffBits = 0L;
		_llseek(fh,off,SEEK_SET);
		}

	if (sizeof(bi) != _lread(fh,(LPSTR)&bi,sizeof(bi)))
		return FALSE;

	// what type of bitmap info is this?
	if ( (size = bi.biSize) == sizeof(BITMAPCOREHEADER) )
		{
			bc = *(LPBITMAPCOREHEADER)&bi;
			bi.biSize				= sizeof(BITMAPINFOHEADER);
			bi.biWidth				= (DWORD)bc.bcWidth;
			bi.biHeight				= (DWORD)bc.bcHeight;
			bi.biPlanes				= (UINT)bc.bcPlanes;
			bi.biBitCount			= (UINT)bc.bcBitCount;
			bi.biCompression		= BI_RGB;
			bi.biSizeImage			= 0;
			bi.biXPelsPerMeter		= 0;
			bi.biYPelsPerMeter		= 0;
			bi.biClrUsed			= 0;
			bi.biClrImportant		= 0;
			_llseek(fh,(long)sizeof(BITMAPCOREHEADER)-sizeof(BITMAPINFOHEADER),SEEK_CUR);
		}

	//!!! hack for BI_BITFIELDS
	if ( bi.biCompression == BI_BITFIELDS && !bi.biClrUsed )
		bi.biClrUsed = 3;

	// Copy in the bitmap info header
	m_bmiHeader = bi;
	FixHeader();
	nNumColors = GetNumColors();
	pRgb = GetColors();

	if (nNumColors)
		{
		if (size == sizeof(BITMAPCOREHEADER))
			{
			// convert old color table (3 byte entries) to new (4 byte entries)
			_lread(fh,(LPVOID)pRgb,nNumColors * sizeof(RGBTRIPLE));

			for (i=nNumColors-1; i>=0; i--)
				{
				RGBQUAD rgb;

				rgb.rgbRed		= ((LPRGBTRIPLE)pRgb)[i].rgbtRed;
				rgb.rgbBlue		= ((LPRGBTRIPLE)pRgb)[i].rgbtBlue;
				rgb.rgbGreen	= ((LPRGBTRIPLE)pRgb)[i].rgbtGreen;
				rgb.rgbReserved = (BYTE)0;

				pRgb[i] = rgb;
				}
			}
		else
			{
			_lread(fh,(LPVOID)pRgb,nNumColors * sizeof(RGBQUAD));
			}
		}

	if (bf.bfOffBits)
		_llseek(fh,off + bf.bfOffBits,SEEK_SET);

	return TRUE;
}

/***********************************************************************/
static void xlatClut8( LPBYTE ptr, DWORD dwSize, LPBYTE xlat, LPTR lpRemapped )
/***********************************************************************/
{
	BYTE b;
	HPTR hptr;
	long lSize;

	hptr = ptr;
	lSize = (long)dwSize;
	while ( --lSize >= 0 )
	{
		b = *hptr;
		if (lpRemapped && (*hptr != xlat[b]))
			lpRemapped[b] = TRUE;
		*hptr++ = xlat[b];
	}
}

#define RLE_ESCAPE	0
#define RLE_EOL		0
#define RLE_EOF		1
#define RLE_JMP		2

/***********************************************************************/
static void xlatRle8( LPBYTE ptr, DWORD dwSize, LPBYTE xlat, LPTR lpRemapped )
/***********************************************************************/
{
	BYTE b, c;
	HPTR hptr;
	char cnt;

	hptr = ptr;
	while (1)
		{
		cnt = *hptr++;
		b	= *hptr;

		if (cnt != RLE_ESCAPE)
			{
			if (lpRemapped && (*hptr != xlat[b]))
				lpRemapped[b] = TRUE;
			*hptr++ = xlat[b];
			}
		else
			{
			hptr++;

			switch (b)
				{
				case RLE_EOF:
					return;

				case RLE_EOL:
					break;

				case RLE_JMP:
					hptr++;		// skip dX
					hptr++;		// skip dY
					break;

				default:
					cnt = b;
					while ( --cnt >= 0 )
						{
						c = *hptr;
						if (lpRemapped && (*hptr != xlat[c]))
							lpRemapped[c] = TRUE;
						*hptr++ = xlat[c];
						}

					if (b & 1)
						hptr++;

					break;
				}
			}
		}
}

// Map the colors of the DIB (using GetNearestPaletteIndex) to a palette
//************************************************************************
int CDib::MapToPalette( HPALETTE hpal, LPTR lpRemapped )
//************************************************************************
{
	if ( !hpal )
		return( 0 );

	if ( m_bmiHeader.biBitCount != 8 )
		return( 0 );

	if ( GetColorTableType() == DIB_PAL_COLORS )
		return( 0 );

	//	build a xlat table. from the current DIB colors to the given palette
	int nDibColors = GetNumColors();
	LPRGBQUAD lpRgb = GetColors();
	int iChanged = 0;
	BYTE xlat[256];
	_fmemset(xlat, 0, sizeof(xlat));
	for ( int n=0; n<nDibColors; n++ )
	{
		// get this particular palette entry and compare
		PALETTEENTRY Entry;
		if ( GetPaletteEntries(hpal, n, 1, &Entry) && 
			 Entry.peRed   == lpRgb->rgbRed        &&
			 Entry.peGreen == lpRgb->rgbGreen      &&
			 Entry.peBlue  == lpRgb->rgbBlue )
			xlat[n] = n;
		else
		{
			xlat[n] = (BYTE)GetNearestPaletteIndex( hpal,
				RGB(lpRgb->rgbRed, lpRgb->rgbGreen, lpRgb->rgbBlue) );
			if ( xlat[n] != n )
				iChanged++;
		}
		lpRgb++;
	}

	if ( !iChanged )
	{
		// Now reset the DIB color table to match the palette
		SetColorTable( hpal, DIB_RGB_COLORS );
		return( 0 );
	}

	// translate the DIB bits
	LPBYTE lpBits = (LPBYTE)GetPtr();
	DWORD SizeImage = GetSizeImage();
	switch (m_bmiHeader.biCompression)
	{
		case BI_RLE8:
			xlatRle8(lpBits, SizeImage, xlat, lpRemapped);
			break;

		case BI_RGB:
			xlatClut8(lpBits, SizeImage, xlat, lpRemapped);
			break;
	}

	// Now reset the DIB color table to match the palette
	SetColorTable( hpal, DIB_RGB_COLORS );
	return( iChanged );
}

// Map the colors of the DIB to grayscale levels (0-255)
//************************************************************************
void CDib::ConvertToLevels()
//************************************************************************
{
	if ( m_bmiHeader.biBitCount != 8 )
		return;

	if ( GetColorTableType() == DIB_PAL_COLORS )
		return;

	//	build a xlat table. from the current DIB colors to the given palette
	int nDibColors = GetNumColors();
	LPRGBQUAD lpRgb = GetColors();
	BOOL bChanged = NO;
	BYTE xlat[256];
	_fmemset(xlat, 0, sizeof(xlat));
	for ( int n=0; n<nDibColors; n++ )
		{
		int iMin = min(lpRgb->rgbRed, min(lpRgb->rgbGreen, lpRgb->rgbBlue));
		int iMax = max(lpRgb->rgbBlue, max(lpRgb->rgbGreen, lpRgb->rgbBlue));

		xlat[n] = (BYTE)((iMin + iMax)/2);
		if ( xlat[n] != n )
			bChanged = YES;
		lpRgb++;
		}

	if ( !bChanged )
		{
		// Now reset the DIB color table to match the palette
		//SetColorTable( hpal, DIB_RGB_COLORS );
		return;
		}

	// translate the DIB bits
	LPBYTE lpBits = (LPBYTE)GetPtr();
	DWORD SizeImage = GetSizeImage();
	switch (m_bmiHeader.biCompression)
		{
		case BI_RLE8:
			xlatRle8(lpBits, SizeImage, xlat, NULL);
			break;

		case BI_RGB:
			xlatClut8(lpBits, SizeImage, xlat, NULL);
			break;
		}
}

//	Modifies the color table of the passed DIB for use with the wUsage
//	parameter specifed.
//
//	if wUsage is DIB_PAL_COLORS the DIB color table is set to 0-256
//	if wUsage is DIB_RGB_COLORS the DIB color table is set to the RGB values
//		in the passed palette
//************************************************************************
BOOL CDib::SetColorTable( HPALETTE hpal, UINT wUsage )
//************************************************************************
{
	int n;

	switch (wUsage)
	{
		// Set the DIB color table to palette indexes
		case DIB_PAL_COLORS:
		{
			LPWORD pw = (LPWORD)GetColors();
			for ( n=0; n<256; n++ )
				*pw++ = n;
			break;
		}

		// Set the DIB color table to RGBQUADS
		default:
		case DIB_RGB_COLORS:
		{
			if (!hpal)
				hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

			WORD wColors;
			GetObject( hpal, sizeof(WORD), (LPSTR)&wColors );
			int nColors = min(wColors,256);
			PALETTEENTRY ape[256];
			GetPaletteEntries( hpal, 0, nColors, ape );
							
			LPRGBQUAD pRgb = GetColors();
//			m_bmiHeader.biClrUsed = nColors;
			for ( n=0; n<nColors; n++ )
				{
				int m = min(n, 255);
				pRgb[m].rgbRed		= ape[n].peRed;
				pRgb[m].rgbGreen	= ape[n].peGreen;
				pRgb[m].rgbBlue		= ape[n].peBlue;
				pRgb[m].rgbReserved = 0;
				}
			break;
		}
	}

	return TRUE;
}

//************************************************************************
void CDib::CopyColorTable( PDIB pdibSrc )
//************************************************************************
{
	m_bmiHeader.biClrUsed		= pdibSrc->m_bmiHeader.biClrUsed;
	m_bmiHeader.biClrImportant	= pdibSrc->m_bmiHeader.biClrImportant;
	hmemcpy( m_rgbQuad, pdibSrc->GetColors(), sizeof(m_rgbQuad));
}

//************************************************************************
UINT CDib::GetColorTableType()
//************************************************************************
{
LPDWORD lpDWORD;

lpDWORD = (LPDWORD)GetColors();
if ( *lpDWORD == 0x00010000 ) // its looks like indices 0, 1, ...
	return( DIB_PAL_COLORS );
else 
	return( DIB_RGB_COLORS );
}

BOOL CDib::DibFlip()
{
	UINT cLine = GetHeight();
	UINT cFlip = cLine / 2;
	long cbLine = GetWidthBytes();
	UINT i;
	HPTR lpTop, lpBottom, lpTemp;
	static BYTE temp[640];
	
	if (cLine >= 2) 
	{
		if (cbLine > 640)
		{
			if ((lpTemp = (HPTR)GlobalAllocPtr(GHND, cbLine)) == NULL)	
				return FALSE;
		}
		else
		{
			lpTemp = &temp[0];
		}
	
		lpTop = GetPtr();
		lpBottom = lpTop + (cLine - 1) * cbLine;
		for (i = 0 ; i < cFlip ; i++)
		{
			if (lpBottom < lpTop)
				return FALSE;
			hmemcpy(lpTemp, lpTop, cbLine);
			hmemcpy(lpTop, lpBottom, cbLine);
			hmemcpy(lpBottom, lpTemp, cbLine);
	
			lpTop += cbLine;
			lpBottom -= cbLine;
		}
		
		if (lpTemp != &temp[0])
			GlobalFreePtr(lpTemp);
	}
	m_bmiHeader.biHeight = -m_bmiHeader.biHeight;
	return TRUE;
}

//************************************************************************
PDIB CDib::DibCopy(BOOL fCopyBits)
//************************************************************************
{
	PDIB pdibCopy;
	LPTR lp;

	if ( !(lp = AllocX( GetSizeImage(), GMEM_ZEROINIT )) )
	{
		return( NULL );
	}
	if ( !(pdibCopy = new CDib(this, lp, fCopyBits)) )
	{
		FreeUp(lp);
		return( NULL );
	}

	return pdibCopy;
}

//************************************************************************
BOOL CDib::Create( int bits, int dx, int dy )
//************************************************************************
{
	DWORD dwSizeImage;
	int i;
	LPDWORD pdw;

	dwSizeImage = dy * (DWORD)((dx * bits/8+3)&~3);
	if ( !(m_lp = AllocX( dwSizeImage + 1024, GMEM_ZEROINIT )) )
		return FALSE;

#ifdef UNUSED // We can't use this because of a problem noted in DibToDCBlt()
	//	Get WinG to recommend the fastest DIB format
	//	Orientation = ( m_bmiHeader.biHeight < 0 ? -1 : 1 ); 
	if ( WinGRecommendDIBFormat( (LPBITMAPINFO)&m_bmiHeader ) )
		{ // make sure it's 8bpp
		m_bmiHeader.biWidth			*= dx;
		m_bmiHeader.biHeight		*= dy;
		m_bmiHeader.biBitCount		= bits;
		m_bmiHeader.biCompression	= BI_RGB;
		}
	else
#endif
		{ // set it up ourselves
		m_bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		m_bmiHeader.biPlanes		= 1;
		m_bmiHeader.biSizeImage		= dwSizeImage;
		m_bmiHeader.biXPelsPerMeter = 0 ;
		m_bmiHeader.biYPelsPerMeter = 0 ;
		m_bmiHeader.biClrUsed		= 0 ;
		m_bmiHeader.biClrImportant	= 0 ;
		m_bmiHeader.biWidth			= dx;
		m_bmiHeader.biHeight		= dy;
		m_bmiHeader.biBitCount		= bits;
		m_bmiHeader.biCompression	= BI_RGB;
		}

	if (bits == 4)
		m_bmiHeader.biClrUsed = 16;
	else
	if (bits == 8)
		m_bmiHeader.biClrUsed = 256;

	pdw = (LPDWORD)m_rgbQuad;
	for (i=0; i<(int)m_bmiHeader.biClrUsed/16; i++)
	{
		*pdw++ = 0x00000000;	// 0000	 black
		*pdw++ = 0x00800000;	// 0001	 dark red
		*pdw++ = 0x00008000;	// 0010	 dark green
		*pdw++ = 0x00808000;	// 0011	 mustard
		*pdw++ = 0x00000080;	// 0100	 dark blue
		*pdw++ = 0x00800080;	// 0101	 purple
		*pdw++ = 0x00008080;	// 0110	 dark turquoise
		*pdw++ = 0x00C0C0C0;	// 1000	 gray
		*pdw++ = 0x00808080;	// 0111	 dark gray
		*pdw++ = 0x00FF0000;	// 1001	 red
		*pdw++ = 0x0000FF00;	// 1010	 green
		*pdw++ = 0x00FFFF00;	// 1011	 yellow
		*pdw++ = 0x000000FF;	// 1100	 blue
		*pdw++ = 0x00FF00FF;	// 1101	 pink (magenta)
		*pdw++ = 0x0000FFFF;	// 1110	 cyan
		*pdw++ = 0x00FFFFFF;	// 1111	 white
	}

	FixHeader();
	return TRUE;
}

//************************************************************************
BOOL CDib::Create( PDIB pdib )
//************************************************************************
{
	DWORD dwSizeImage = pdib->GetSizeImage();
	if ( !(m_lp = AllocX( dwSizeImage + 1024, GMEM_ZEROINIT )) )
		return FALSE;
	
	m_bmiHeader = *pdib->GetInfoHeader();
	hmemcpy( m_rgbQuad, pdib->GetColors(), sizeof(m_rgbQuad));
	hmemcpy(m_lp, pdib->GetPtr(), pdib->GetSizeImage());
	
	return TRUE;
}

//************************************************************************
UINT CDib::GetWidthBytes()
//************************************************************************
{ // ULONG aligned!
	return( ( ( (m_bmiHeader.biBitCount * (int)m_bmiHeader.biWidth) + 31) & (~31) ) / 8 );
}

//************************************************************************
DWORD CDib::GetSizeImage()
//************************************************************************
{
if ( m_bmiHeader.biSizeImage )
	return( m_bmiHeader.biSizeImage );
return( GetWidthBytes() * abs(m_bmiHeader.biHeight) );
}

//************************************************************************
UINT CDib::GetNumColors()
//************************************************************************
{
if ( m_bmiHeader.biClrUsed )
	return( (UINT)m_bmiHeader.biClrUsed );
if ( m_bmiHeader.biBitCount <= 8 )
	return( (UINT)1 << m_bmiHeader.biBitCount );
return( 0 );
}

//************************************************************************
int CDib::FlipY( int y )
//************************************************************************
{
return( (int)m_bmiHeader.biHeight - y - 1 );
}

//************************************************************************
HPTR CDib::GetXY( int x, int y )
//************************************************************************
{
if ( m_bmiHeader.biHeight > 0 )
	y = FlipY( y );
return( (HPTR)m_lp + ((x * m_bmiHeader.biBitCount) / 8) + ((DWORD)GetWidthBytes() * y) );
}

//************************************************************************
void CDib::FixHeader()
//************************************************************************
{
m_bmiHeader.biSizeImage = GetSizeImage();
m_bmiHeader.biClrUsed = GetNumColors();
if ( !m_bmiHeader.biClrUsed && m_bmiHeader.biCompression == BI_BITFIELDS )
	; // m_bmiHeader.biClrUsed = 3;					   
}

//************************************************************************
void CDib::SwapLines( int y1, int y2, int iCount )
//************************************************************************
{
HPTR lpLineB, lpSwap1, lpSwap2;
int iHeight = abs( GetHeight() );
int iWidth = GetWidthBytes();

if ( m_bmiHeader.biCompression != BI_RGB )
	return;

if ( !(lpSwap1 = GetXY( 0, y1 )) )
	return;

if ( !(lpSwap2 = GetXY( 0, y2 )) )
	return;

if ( !(lpLineB = Alloc( iWidth )) )
	return;

if ( y1 < 0 )
	y1 = -y1;
if ( y1 >= iHeight )
	y1 = iHeight-1;

if ( y2 < 0 )
	y2 = -y2;
if ( y2 >= iHeight )
	y2 = iHeight-1;

if ( iCount < 0 )
	iCount = -iCount;
if ( y1 + iCount > iHeight )
	iCount = iHeight - y1;
if ( y2 + iCount > iHeight )
	iCount = iHeight - y2;

int iDelta = ( GetHeight() < 0 ? iWidth : -iWidth ); // if its an upside down DIB...
while ( --iCount >= 0 )
	{
	hmemcpy( lpLineB, lpSwap2, iWidth );
	hmemcpy( lpSwap2, lpSwap1, iWidth );
	hmemcpy( lpSwap1, lpLineB, iWidth );
	lpSwap1 += iDelta;
	lpSwap2 += iDelta;
	}

FreeUp( lpLineB );
}

#define DN(v) ((WORD)(v)>>3)
#define RGB3toMiniRGB(r, g, b) \
	(((WORD) ( DN((b)) << 5 | DN((g)) ) << 5) | DN((r)) )
	
#ifdef USE_DIBFX
extern "C"
{
#define DibPitch(pdib)	((((pdib->biBitCount * pdib->biWidth) + 31) & ~31) / 8)
#define DibHeight(pdib)	(abs(pdib->biHeight))
#define DibWidth(pdib)	(pdib->biWidth)
void FAR PASCAL CopyTransparentBits(WORD selDst, DWORD offDst, DWORD pitchDst, WORD selSrc, DWORD offSrc, DWORD pitchSrc, DWORD cx, DWORD cy, BYTE transparent);
void FAR PASCAL CopyBits(WORD selDst, DWORD offDst, DWORD pitchDst, WORD selSrc, DWORD offSrc, DWORD pitchSrc, DWORD cx, DWORD cy);

//************************************************************************
BOOL WINAPI MyDibTransparentBlt(PDIBFX pdibDst, LPBYTE pbitsDst, int xDst, int yDst, PDIBFX pdibSrc, LPBYTE pbitsSrc, int xSrc, int ySrc, int cx, int cy, BYTE transparent)
//************************************************************************
{
	DWORD offDst, offSrc;
	WORD selDst, selSrc;
	long pitchDst, pitchSrc;
	
	selDst = HIWORD(pbitsDst);
	selSrc = HIWORD(pbitsSrc);
	
	if (pdibDst->biHeight < 0)
	{
		pitchDst = (long)DibPitch(pdibDst);
		offDst = LOWORD(pbitsDst) + DibPitch(pdibDst) * yDst + xDst;
	}
	else
	{
		pitchDst = - (long)DibPitch(pdibDst);
		offDst = LOWORD(pbitsDst) + DibPitch(pdibDst) * (DibHeight(pdibDst) - yDst - 1) + xDst;
	}

	if (pdibSrc->biHeight < 0)
	{
		pitchSrc = (long)DibPitch(pdibSrc);
		offSrc = LOWORD(pbitsSrc) + DibPitch(pdibSrc) * ySrc + xSrc;
	}
	else
	{
		pitchSrc = - (long)DibPitch(pdibSrc);
		offSrc = LOWORD(pbitsSrc) + DibPitch(pdibSrc) * (DibHeight(pdibSrc) - ySrc - 1) + xSrc;
	}
	CopyTransparentBits(selDst, offDst, pitchDst, selSrc, offSrc, pitchSrc, cx, cy, transparent);
	return TRUE;
}

//************************************************************************
BOOL WINAPI MyDibBlt(PDIBFX pdibDst, LPBYTE pbitsDst, int xDst, int yDst, PDIBFX pdibSrc, LPBYTE pbitsSrc, int xSrc, int ySrc, int cx, int cy)
//************************************************************************
{
	DWORD offDst, offSrc;
	WORD selDst, selSrc;
	long pitchDst, pitchSrc;
	
	selDst = HIWORD(pbitsDst);
	selSrc = HIWORD(pbitsSrc);
	
	if (pdibDst->biHeight < 0)
	{
		pitchDst = (long)DibPitch(pdibDst);
		offDst = LOWORD(pbitsDst) + DibPitch(pdibDst) * yDst + xDst;
	}
	else
	{
		pitchDst = - (long)DibPitch(pdibDst);
		offDst = LOWORD(pbitsDst) + DibPitch(pdibDst) * (DibHeight(pdibDst) - yDst - 1) + xDst;
	}

	if (pdibSrc->biHeight < 0)
	{
		pitchSrc = (long)DibPitch(pdibSrc);
		offSrc = LOWORD(pbitsSrc) + DibPitch(pdibSrc) * ySrc + xSrc;
	}
	else
	{
		pitchSrc = - (long)DibPitch(pdibSrc);
		offSrc = LOWORD(pbitsSrc) + DibPitch(pdibSrc) * (DibHeight(pdibSrc) - ySrc - 1) + xSrc;
	}
	CopyBits(selDst, offDst, pitchDst, selSrc, offSrc, pitchSrc, cx, cy);
	return TRUE;
}

}
#endif // USE_DIBFX

// Note we only support 8bpp DIBs here, and..
// there is no regard for color table matching between the DIBs
//************************************************************************
void CDib::DibBlt(PDIB pdibDst, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				  int srcLeft, int srcTop, int srcWidth, int srcHeight,
				  BOOL bTransparent, LPRGBTRIPLE lpRGB, LPTR lpLut, HPALETTE hPal )
//************************************************************************
{
HPTR pSrc, pDst;
int iScanS, iScanD, iOverrun;

if ( !pdibDst )
	return;
if ( GetBitCount() != 8 )
	return;
if ( pdibDst->GetBitCount() != 8 )
	return;

// Check the coordinate bounds, to avoid an out of range pointer
if ( srcLeft < 0 )	{ dstLeft -= srcLeft; srcWidth	+= srcLeft; dstWidth  += srcLeft; srcLeft = 0; }
if ( srcTop	 < 0 )	{ dstTop  -= srcTop;  srcHeight += srcTop;	dstHeight += srcTop;  srcTop  = 0; }
if ( dstLeft < 0 )	{ srcLeft -= dstLeft; srcWidth	+= dstLeft; dstWidth  += dstLeft; dstLeft = 0; }
if ( dstTop	 < 0 )	{ srcTop  -= dstTop;  srcHeight += dstTop;	dstHeight += dstTop;  dstTop  = 0; }

if ( srcWidth <= 0 || srcHeight <= 0 || dstWidth <= 0 || dstHeight <= 0 )
	return;

if ( srcLeft - GetWidth()	   >= 0 )	return;
if ( srcTop	 - abs(GetHeight()) >= 0 )	return;
if ( dstLeft - pdibDst->GetWidth()		   >= 0 )	return;
if ( dstTop	 - abs(pdibDst->GetHeight()) >= 0 ) return;

if ( (iOverrun = srcLeft + srcWidth	 - GetWidth())		> 0 )	{ srcWidth	-= iOverrun; dstWidth  -= iOverrun; }
if ( (iOverrun = srcTop	 + srcHeight - abs(GetHeight())) > 0 )	{ srcHeight -= iOverrun; dstHeight -= iOverrun; }
if ( (iOverrun = dstLeft + dstWidth	 - pdibDst->GetWidth())		> 0 )	{ dstWidth	-= iOverrun; srcWidth  -= iOverrun; }
if ( (iOverrun = dstTop	 + dstHeight - abs(pdibDst->GetHeight())) > 0 ) { dstHeight -= iOverrun; srcHeight -= iOverrun; }

if ( srcWidth <= 0 || srcHeight <= 0 || dstWidth <= 0 || dstHeight <= 0 )
	return;

if ( !bTransparent )
	{ // Copy the lines.
	#ifdef USE_DIBFX
	MyDibBlt(	pdibDst->GetInfoHeader(), pdibDst->GetPtr(), dstLeft, dstTop,
				GetInfoHeader(), GetPtr(), srcLeft, srcTop,
				srcWidth, srcHeight);
	#else
	// Get pointers to the start points in the source and destination
	if ( !(pSrc = GetXY( srcLeft, srcTop )) )
		return;
	if ( !(pDst = pdibDst->GetXY( dstLeft, dstTop )) )
		return;
	
	// Get the scan line widths of each DIB.
	iScanS = GetWidthBytes();
	iScanD = pdibDst->GetWidthBytes();
	
	// Upside down DIBs have to move backwards
	if ( GetHeight() > 0 )
		 iScanS = -iScanS;
	if ( pdibDst->GetHeight() > 0 )
		 iScanD = -iScanD;
	
	while ( --srcHeight >= 0 )
		{
		hmemcpy( pDst, pSrc, srcWidth );
		pSrc += iScanS;
		pDst += iScanD;
		}
	#endif
	}
else
if (lpRGB)
	{ // Copy lines with transparency mask
	WORD src, dst, wMini;
	BYTE dstPixel, srcColor, red, green, blue;
	int iSinc, iDinc, iCount;
						 
	// Get pointers to the start points in the source and destination
	if ( !(pSrc = GetXY( srcLeft, srcTop )) )
		return;
	if ( !(pDst = pdibDst->GetXY( dstLeft, dstTop )) )
		return;

	// Get the scan line widths of each DIB.
	iScanS = GetWidthBytes();
	iScanD = pdibDst->GetWidthBytes();

	// Upside down DIBs have to move backwards
	if ( GetHeight() > 0 )
		 iScanS = -iScanS;
	if ( pdibDst->GetHeight() > 0 )
		 iScanD = -iScanD;

	LPRGBQUAD pTable = GetColors();
	if (lpLut)
		{
		wMini = RGB3toMiniRGB(lpRGB->rgbtRed, lpRGB->rgbtGreen, lpRGB->rgbtBlue);
		srcColor = lpLut[wMini];				
		}
	else
	if (hPal)
		{
		srcColor = (BYTE)GetNearestPaletteIndex( hPal, RGB(lpRGB->rgbtRed, lpRGB->rgbtGreen, lpRGB->rgbtBlue) );
		}
	else
		srcColor = 0;

	iSinc = iScanS - srcWidth; // Source increment value
	iDinc = iScanD - srcWidth; // Destination increment value
	while ( --srcHeight >= 0 )
		{
		iCount = srcWidth;	  // Number of pixels to scan.
		while ( --iCount >= 0 )
			{
			src = (WORD)(*pSrc++);
			// Copy pixel only if it isn't transparent.
			if (src == 0)
				pDst++;
			else
			if (src == 255)
				*pDst++ = srcColor;
			else
				{
				if (src > 127)
					++src;
				dst = 256-src;
				dstPixel = *pDst;
				red = (BYTE)((((WORD)lpRGB->rgbtRed * src) + ((WORD)pTable[dstPixel].rgbRed * dst)) >> 8);
				green = (BYTE)((((WORD)lpRGB->rgbtGreen * src) + ((WORD)pTable[dstPixel].rgbGreen * dst)) >> 8);
				blue = (BYTE)((((WORD)lpRGB->rgbtBlue * src) + ((WORD)pTable[dstPixel].rgbBlue * dst)) >> 8);
				if (lpLut)
					{
					wMini = RGB3toMiniRGB(red, green, blue);
					*pDst++ = lpLut[wMini];					
					}
				else
				if (hPal)
					{
					*pDst++ = (BYTE)GetNearestPaletteIndex( hPal, RGB(red, green, blue) );
					}
				else
					{
					pDst++;
					}
				}
			}

		pSrc += iSinc;
		pDst += iDinc;
		}
	}
else
	{ // Copy lines with transparency.
	BYTE cTransparent = *GetXY(0, 0);
	#ifdef USE_DIBFX
	MyDibTransparentBlt(	pdibDst->GetInfoHeader(), pdibDst->GetPtr(), dstLeft, dstTop,
						GetInfoHeader(), GetPtr(), srcLeft, srcTop,
						srcWidth, srcHeight, cTransparent);
	#else
	BYTE pixel;
	int iSinc, iDinc, iCount;

	// Get pointers to the start points in the source and destination
	if ( !(pSrc = GetXY( srcLeft, srcTop )) )
		return;
	if ( !(pDst = pdibDst->GetXY( dstLeft, dstTop )) )
		return;

	// Get the scan line widths of each DIB.
	iScanS = GetWidthBytes();
	iScanD = pdibDst->GetWidthBytes();

	// Upside down DIBs have to move backwards
	if ( GetHeight() > 0 )
		 iScanS = -iScanS;
	if ( pdibDst->GetHeight() > 0 )
		 iScanD = -iScanD;

	iSinc = iScanS - srcWidth; // Source increment value
	iDinc = iScanD - srcWidth; // Destination increment value
	while ( --srcHeight >= 0 )
		{
		iCount = srcWidth;	  // Number of pixels to scan.
		while ( --iCount >= 0 )
			{
			pixel = *pSrc++;
			// Copy pixel only if it isn't transparent.
			if (pixel != cTransparent)
				*pDst++ = pixel;
			else
				pDst++;
			}
	
		pSrc += iSinc;
		pDst += iDinc;
		}
	#endif
	}
}

//************************************************************************
void CDib::DCBlt(  HDC hDC, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				int srcLeft, int srcTop, int srcWidth, int srcHeight, DWORD rop )
//************************************************************************
{ // a substiture for WinGBitBlt(), or to load a WinG DIB(DC) with bits
int y, iSign;

if ( !hDC || !GetPtr() )
	return;

if ( m_bmiHeader.biHeight > 0 )
	{ // need to start at the last y (which is first in dib memory)
	iSign = 1;
	y = FlipY( srcTop + srcHeight - 1 );
	}
else
	{ // need to fix the dib header's height, and tell stretch to mirror data
	iSign = -1;
	y = srcTop + srcHeight + 1;
	}

#ifdef WIN32
	if ( m_bmiHeader.biBitCount > 8 )
		SetStretchBltMode( hDC, HALFTONE );
	else
		SetStretchBltMode( hDC, COLORONCOLOR );
#else
	SetStretchBltMode( hDC, COLORONCOLOR );
#endif

	// My lame attempts at transparent blt's with a DC blt
	// Maybe it doesn't work cause its a Wing DC?
//	SetBkMode( hDC, 3/*NEWTRANSPARENT*/ );
//	COLORREF cColor = GetPixel( hDC, 0, 0 );
//	COLORREF cColor = PALETTERGB(0,0,0);	// (0x02000000L | RGB(r,g,b))
//	COLORREF cColor = PALETTEINDEX(*GetPtr());	// ((COLORREF)(0x01000000L | (DWORD)(WORD)(i)))
//	COLORREF cOldColor = SetBkColor( hDC, cColor );

// to try...
// trans white, other black, then SRCAND
// trans black, other orig, then SRCPAINT

// Be warned that if the destination DC is a WING DC, and the sign
// is negative, the bits don't mirror correctly (if at all).
// This code will work without any problems, as long as either...
// 1. the source DIB is a bottom up (old style) dib as loaded from
//	  disk, or created when halftoned, or,
// 2. the source DIB is a top down (new style) dib, and the DC is the screen
m_bmiHeader.biHeight *= iSign;
StretchDIBits( hDC, dstLeft, dstTop, dstWidth, dstHeight,
	srcLeft, y, srcWidth, srcHeight * iSign,
	GetPtr() , GetInfo(), GetColorTableType(), rop );
m_bmiHeader.biHeight *= iSign;
}

//************************************************************************
BOOL CDib::Dwindle()
//************************************************************************
{
long lx, lx1, lx2, lCount;
int x1, x2, y, y1, y2, dx, dy, depth;
BOOL bLeftEdge, bRightEdge;
HPTR lp;
RECT rRect;
LPRECT lpRect;

if ( !GetPtr() )
	return( FALSE );

if ( GetBitCount() != 8 )
	return( FALSE );

depth = 1;
dx = GetWidth();
dy = abs(GetHeight());
lpRect = &rRect;
SetRect( lpRect, 0, 0, dx-1, dy-1 );

for ( y1 = lpRect->top; y1 <= lpRect->bottom; y1++ )
	{
	if ( !(lp = GetXY( lpRect->left, y1 )) )
		break;
	lCount = (long)dx * depth;
	while ( --lCount >= 0 )
		{
		if ( *lp++ )
			goto FoundTop;
		}
	}

FoundTop:

if ( y1 > lpRect->bottom ) // If the mask is empty, ...
	return(FALSE);

for ( y2 = lpRect->bottom; y2 > y1; y2-- )
	{
	if ( !(lp = GetXY( lpRect->left, y2 )) )
		break;
	lCount = (long)dx * depth;
	while ( --lCount >= 0 )
		{
		if ( *lp++ )
			goto FoundBottom;
		}
	}

FoundBottom:

lx1 = +999999999;
lx2 = -999999999;
lCount = (long)dx * depth;
bLeftEdge = NO;
bRightEdge = NO;
for ( y = y1; y <= y2; y++ )
	{
	if ( !(lp = GetXY( lpRect->left, y )) )
		break;
	if ( !bRightEdge )
		{ // If its worth doing a search for the right most...
		for ( lx = lCount-1; lx > 0; lx-- )
			{ // Do a byte search
			if ( *(lp+lx) )
				break;
			}
		if ( lx > lx2 )
			{
			lx2 = lx;
			if ( lCount - lx2 <= depth )
				bRightEdge = YES;
			}
		}
	if ( !bLeftEdge )
		{ // If its worth doing a search for the left most...
		for ( lx = 0; lx < lCount-1; lx++ )
			{ // Do a byte search
			if ( *(lp+lx) )
				break;
			}
		if ( lx < lx1 )
			{
			lx1 = lx;
			if ( lx1 < depth )
				bLeftEdge = YES;
			}
		}
	if ( bLeftEdge && bRightEdge )
		break;
	}

// Convert byte counts to pixel counts
x1 = lpRect->left + (int)(lx1 / depth);
x2 = lpRect->left + (int)(lx2 / depth);

// Set the mask rectangle
SetRect( lpRect, x1, y1, x2, y2 );
return(TRUE);
}

//************************************************************************
PDIB CDib::Convert( int BitCount, DWORD biCompression )
//************************************************************************
{
	PDIB pdib = new CDib( this, NULL, FALSE );
	if ( !pdib )
		return( FALSE );

	if ( pdib->ConvertData( this->GetPtr(), BitCount, biCompression, DIB_RGB_COLORS ) )
		return( pdib );

	delete pdib;
	return( FALSE );
}

//************************************************************************
BOOL CDib::ConvertData( LPTR lpData, UINT biBits, DWORD biCompression, UINT wUsage )
//************************************************************************
{
	if ( !lpData )
		return( FALSE );

	HPALETTE hpal;
	if (GetNumColors() > 3)
		hpal = CreateIdentityPalette( GetColors(), GetNumColors(), NO/*bMakeIdentity*/ );
	else
		hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	HBITMAP hbm = CreateBitmap(hpal, lpData);
	if ( !hbm )
	{
		DeletePalette(hpal);
		return( FALSE );
	}

	if ( biBits )
		m_bmiHeader.biBitCount	= biBits;
	m_bmiHeader.biCompression	= biCompression;
	m_bmiHeader.biSizeImage		= 0;
	m_bmiHeader.biPlanes		= 1;

	// this is a silly hack to test Win32/NT and BI_BITFIELDS
	// convert to a 565 (for 16 bit) or a RGB DIB (for 32/24 bit)
	if (biCompression == BI_BITFIELDS)
	{
		LPDWORD pdw = (LPDWORD)GetColors();

		switch (biBits)
		{
			case 16:
				pdw[0] = 0xF800;		// masks for RGB 565
				pdw[1] = 0x07E0;
				pdw[2] = 0x001F;
				break;

			case 24:
			case 32:
				pdw[0] = 0x0000FF;		// masks for RGB (not normal BGR)
				pdw[1] = 0x00FF00;
				pdw[2] = 0xFF0000;
				break;
		}
	}

	HDC hdc = CreateCompatibleDC(NULL);
	HPALETTE hOldPal = SelectPalette(hdc, hpal, FALSE/*bProtectPhysicalPal*/);
	RealizePalette(hdc);

#ifdef WIN32
	SetStretchBltMode(hdc, HALFTONE);
#endif

	// call GetDIBits with a NULL lpBits param, to calculate the biSizeImage field
	GetDIBits(hdc, hbm, 0, (int)abs(m_bmiHeader.biHeight), NULL, (LPBITMAPINFO)&m_bmiHeader, wUsage);

	m_bmiHeader.biClrUsed = GetNumColors();
	m_bmiHeader.biSizeImage = GetSizeImage();
									   
	// Alloc the buffer big enough to hold all the bits
	UINT u;
	if ( !(m_lp = AllocX( GetSizeImage(), GMEM_ZEROINIT )) )
		u = FALSE;
	else
	{
		//	call GetDIBits with a NON-NULL lpBits param, and actually get the
		//	bits this time
		u = GetDIBits(hdc, hbm, 0, (int)abs(m_bmiHeader.biHeight),
			m_lp, (LPBITMAPINFO)&m_bmiHeader, wUsage);
		
		m_bmiHeader.biClrUsed = GetNumColors();
		m_bmiHeader.biSizeImage = GetSizeImage();
	}

	if (hpal)
	{
		SelectPalette(hdc, hOldPal, FALSE/*bProtectPhysicalPal*/);
		DeletePalette(hpal);
	}

	if ( hdc)
		DeleteDC(hdc);

	if (hbm)
		DeleteObject(hbm);

	return( (BOOL)u );
}

//************************************************************************
HBITMAP CDib::CreateBitmap( HPALETTE hpal, LPTR lpData )
//************************************************************************
{
	HDC hdc;
	if ( !(hdc = GetDC(NULL)) )
		return( NULL );

	HPALETTE hOldPal;
	if (hpal)
	{
		hOldPal = SelectPalette(hdc, hpal, FALSE/*bProtectPhysicalPal*/);
		RealizePalette(hdc);
	}

	DWORD dwFlags = CBM_INIT;
#ifdef WIN32
	SetStretchBltMode(hdc, HALFTONE);		//??? does this do anything?
	if (!m_bmiHeader.biCompression || m_bmiHeader.biCompression == BI_BITFIELDS)
		dwFlags = CBM_INIT;
#endif

	HBITMAP hBitmap = CreateDIBitmap(hdc, &m_bmiHeader, dwFlags, lpData, (LPBITMAPINFO)&m_bmiHeader, DIB_RGB_COLORS);

	if (hpal && hOldPal)
		SelectPalette(hdc, hOldPal, FALSE/*bProtectPhysicalPal*/);

	ReleaseDC(NULL,hdc);

	return( hBitmap );
}

//************************************************************************
BOOL CDib::CheckDecodeRLE(BOOL fDecodeRLE)
//************************************************************************
{
	if (!m_lp)
		return(FALSE);

	if ((GetCompression() == BI_RLE8) && fDecodeRLE)
	{
		DWORD dwSize = (long)GetWidthBytes() * (long)abs(GetHeight());
		LPTR lp = AllocX(dwSize, GMEM_ZEROINIT );
		if (!lp)
		{
			FreeUp(m_lp);
			m_lp = NULL;
			return(FALSE);
		}
		DecodeRLE(lp);
		FreeUp(m_lp);
		m_lp = lp;
		SetCompression(BI_RGB);
		SetSizeImage(dwSize);
	}
	return TRUE;
}

//
//	DecodeRle	- 'C' version
//
//	Play back a RLE buffer into a DIB buffer
//
//	returns
//		none
//
//************************************************************************
void CDib::DecodeRLE(HPTR pb)
//************************************************************************
{
	BYTE	cnt;
	BYTE	b;
	WORD	x;
	WORD	dx,dy;
	WORD	wWidthBytes;
	HPTR	prle = GetPtr();

	wWidthBytes = (WORD)GetWidthBytes();

	x = 0;

	for(;;)
	{
		cnt = *prle++;
		b	= *prle++;

		if (cnt == RLE_ESCAPE)
		{
			switch (b)
			{
				case RLE_EOF:
					return;

				case RLE_EOL:
					pb += wWidthBytes - x;
					x = 0;
					break;

				case RLE_JMP:
					dx = (WORD)*prle++;
					dy = (WORD)*prle++;

					pb += (DWORD)wWidthBytes * dy + dx;
					x  += dx;

					break;

				default:
					cnt = b;
					x  += cnt;
					while (cnt-- > 0)
						*pb++ = *prle++;

					if (b & 1)
						prle++;

					break;
			}
		}
		else
		{
			x += cnt;

			while (cnt-- > 0)
				*pb++ = b;
		}
	}
}
