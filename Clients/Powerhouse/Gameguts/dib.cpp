#include <windows.h>
#include "proto.h"
#include "wing.h"
#include "memory.h"
#include "dibfx.h"

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

	Decode.x = 0;
	Decode.y = 0;
	Decode.yInBuf = -1;
	Decode.pBuf = NULL;
	Decode.pSrc = NULL;

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

	Decode.x = pdib->Decode.x;
	Decode.y = pdib->Decode.y;
	Decode.yInBuf = pdib->Decode.yInBuf;
	Decode.pBuf = NULL;
	Decode.pSrc = NULL;

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

	if ( Decode.pBuf )
		FreeUp( Decode.pBuf );
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
	if ((m_bmiHeader.biCompression == BI_RLE8) && fDecodeRLE)
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
		pDib->m_bmiHeader.biCompression = BI_RGB;
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

//************************************************************************
BOOL CDib::DibFlip()
//************************************************************************
{
	UINT cLine = GetHeight();
	UINT cFlip = cLine / 2;
	long cbLine = GetWidthBytes();
	UINT i;
	HPTR lpTop, lpBottom, lpTemp;
	BYTE temp[640];
	
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

	if ( m_bmiHeader.biCompression != BI_RLE8 )
		return( (HPTR)m_lp + ((x * m_bmiHeader.biBitCount) / 8) + ((DWORD)GetWidthBytes() * y) );
	else
		return( GetXY_RLE( x, y ) );
}

//************************************************************************
HPTR CDib::GetXY_RLE( int x, int y )
//************************************************************************
{
	// See if the caller is asking for the last completed line
	if ( y == Decode.yInBuf )
		return( Decode.pBuf + x );

	// Make sure we have a decode buffer
	if ( !Decode.pBuf )
	{
		if ( !(Decode.pBuf = Alloc( GetWidthBytes() )) )
			return( NULL );
	}

	// Make sure we have some source data to decode, and also...
	// See if we need to start over to get to the desired source line
	if ( !Decode.pSrc || y < Decode.y )
	{
		Decode.x = 0;
		Decode.y = 0;
		Decode.yInBuf = -1;
		Decode.pSrc = GetPtr();
		if ( !Decode.pSrc )
			return( NULL );
	}

	// Start decoding the current source line
	HPTR pDst = Decode.pBuf;

	while (1)
	{
		int cnt = *Decode.pSrc++;
		BYTE b = *Decode.pSrc++;

		if (cnt == RLE_ESCAPE)
		{
			switch (b)
			{
				case RLE_EOF:
				case RLE_EOL:
				{
					BOOL bDone = (y == Decode.y);
					pDst -= Decode.x;
					Decode.x = 0;
					Decode.y++;
					if ( bDone )
					{
						Decode.yInBuf = y;
						return( Decode.pBuf + x );
					}

					if (b == RLE_EOF)
						return( NULL );
					break;
				}

				case RLE_JMP:
				{
					int dx = (WORD)*Decode.pSrc++;
					int dy = (WORD)*Decode.pSrc++;
					BOOL bDone = (y == Decode.y);
					pDst += dx;
					Decode.x += dx;
					Decode.y += dy;
					if ( bDone && dy )
					{
						Decode.yInBuf = y;
						return( Decode.pBuf + x );
					}
					break;
				}

				default: // Copy the raw data that follows the codes
				{
					cnt = b;
					Decode.x += cnt;
					if (y == Decode.y)
					{
						while (--cnt >= 0)
							*pDst++ = *Decode.pSrc++;
					}
					else
					{
						pDst += cnt;
						Decode.pSrc += cnt;
					}
					if (b & 1)
						Decode.pSrc++;
					break;
				}
			}
		}
		else
		{ // Copy a solid color
			Decode.x += cnt;
			if (y == Decode.y)
			{
				while (--cnt >= 0)
					*pDst++ = b;
			}
			else
			{
				pDst += cnt;
			}
		}
	}

	return( NULL );
}

//************************************************************************
void CDib::DecodeRLE(HPTR pb)
//************************************************************************
{
	HPTR pSrc = GetPtr();
	if (!pSrc)
		return;

	int iWidthBytes = GetWidthBytes();

	int x = 0;
	int y = 0;

	while (1)
	{
		int cnt = *pSrc++;
		BYTE b = *pSrc++;

		if (cnt == RLE_ESCAPE)
		{
			switch (b)
			{
				case RLE_EOF:
					return;

				case RLE_EOL:
				{
					pb += (iWidthBytes - x);
					x = 0;
					y++;
					break;
				}

				case RLE_JMP:
				{
					int dx = (WORD)*pSrc++;
					int dy = (WORD)*pSrc++;
					pb += ((long)iWidthBytes * dy + dx);
					x += dx;
					y += dy;
					break;
				}

				default: // Copy raw bytes that follow
				{
					cnt = b;
					x += cnt;
					while (--cnt >= 0)
						*pb++ = *pSrc++;
					if (b & 1)
						pSrc++;
					break;
				}
			}
		}
		else
		{ // Copy a solid color
			x += cnt;
			while (--cnt >= 0)
				*pb++ = b;
		}
	}
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
	WORD selDst = HIWORD(pbitsDst);
	WORD selSrc = HIWORD(pbitsSrc);
	
	DWORD offDst, offSrc;
	long pitchDst, pitchSrc;
	
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
	WORD selDst = HIWORD(pbitsDst);
	WORD selSrc = HIWORD(pbitsSrc);
	
	DWORD offDst, offSrc;
	long pitchDst, pitchSrc;

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

//************************************************************************
BOOL CDib::DibClipRect( PDIB pdibDst, LPRECT prDst, LPRECT prDstClip,
						PDIB pdibSrc, LPRECT prSrc, LPRECT prSrcClip )
//************************************************************************
{
	if ( !pdibDst || !prDst || !pdibSrc || !prSrc )
		return( NO );

	// Set up the clip rectangles
	RECT rDstClip;
	SetRect( &rDstClip, 0, 0, pdibDst->GetWidth(), abs(pdibDst->GetHeight()) );
	if ( prDstClip )
		IntersectRect( &rDstClip, &rDstClip, prDstClip );

	RECT rSrcClip;
	SetRect( &rSrcClip, 0, 0, pdibSrc->GetWidth(), abs(pdibSrc->GetHeight()) );
	if ( prSrcClip )
		IntersectRect( &rSrcClip, &rSrcClip, prSrcClip );

	int dstLeft   = prDst->left;
	int dstTop    = prDst->top;
	int dstWidth  = prDst->right - prDst->left;
	int dstHeight = prDst->bottom - prDst->top;
	int srcLeft   = prSrc->left;
	int srcTop    = prSrc->top;
	int srcWidth  = prSrc->right - prSrc->left;
	int srcHeight = prSrc->bottom - prSrc->top;

	// Check the coordinate bounds, to avoid an out of range pointer
	int iOffset;
	int iOverrun;

	if ( (iOverrun = srcLeft - rSrcClip.left) < 0 )
	{
		if ( dstWidth == srcWidth )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(dstWidth, srcWidth));

		dstWidth += iOffset;
		dstLeft  -= iOffset;

		srcWidth += iOverrun;
		srcLeft  -= iOverrun;
	}

	if ( (iOverrun = srcTop - rSrcClip.top) < 0 )
	{
		if ( dstHeight == srcHeight )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(dstHeight, srcHeight));

		dstHeight += iOffset;
		dstTop    -= iOffset;

		srcHeight += iOverrun;
		srcTop    -= iOverrun;
	}

	if ( (iOverrun = dstLeft - rDstClip.left) < 0 )
	{
		if ( srcWidth == dstWidth )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(srcWidth, dstWidth));

		srcWidth += iOffset;
		srcLeft  -= iOffset;

		dstWidth += iOverrun;
		dstLeft  -= iOverrun;
	}

	if ( (iOverrun = dstTop - rDstClip.top) < 0 )
	{
		if ( srcHeight == dstHeight )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(srcHeight, dstHeight));

		srcHeight += iOffset;
		srcTop    -= iOffset;

		dstHeight += iOverrun;
		dstTop    -= iOverrun;
	}

	if ( srcWidth <= 0 || srcHeight <= 0 || dstWidth <= 0 || dstHeight <= 0 )
		return( NO );

	if ( srcLeft >= rSrcClip.right )
		return( NO );
	if ( srcTop	 >= rSrcClip.bottom )
		return( NO );
	if ( dstLeft >= rDstClip.right )
		return( NO );
	if ( dstTop	 >= rDstClip.bottom )
		return( NO );

	if ( (iOverrun = srcLeft + srcWidth	- rSrcClip.right) > 0 )
	{
		if ( srcWidth == dstWidth )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(dstWidth, srcWidth));

		srcWidth -= iOverrun;
		dstWidth -= iOffset;
	}

	if ( (iOverrun = srcTop	 + srcHeight - rSrcClip.bottom) > 0 )
	{
		if ( srcWidth == dstWidth )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(dstHeight, srcHeight));

		srcHeight -= iOverrun;
		dstHeight -= iOffset;
	}

	if ( (iOverrun = dstLeft + dstWidth	 - rDstClip.right) > 0 )
	{
		if ( srcWidth == dstWidth )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(srcWidth, dstWidth));

		dstWidth -= iOverrun;
		srcWidth -= iOffset;
	}

	if ( (iOverrun = dstTop	 + dstHeight - rDstClip.bottom) > 0 )
	{
		if ( srcHeight == dstHeight )
			iOffset = iOverrun;
		else
			iOffset = fmul(iOverrun, fget(srcHeight, dstHeight));

		dstHeight -= iOverrun;
		srcHeight -= iOffset;
	}

	if ( srcWidth <= 0 || srcHeight <= 0 || dstWidth <= 0 || dstHeight <= 0 )
		return( NO );

	SetRect( prDst, dstLeft, dstTop, dstLeft + dstWidth, dstTop + dstHeight );
	SetRect( prSrc, srcLeft, srcTop, srcLeft + srcWidth, srcTop + srcHeight );

	return( YES );
}

//************************************************************************
BOOL CDib::DibClip( PDIB pdibDst, LPINT pdstLeft, LPINT pdstTop, LPINT pdstWidth, LPINT pdstHeight,
					PDIB pdibSrc, LPINT psrcLeft, LPINT psrcTop, LPINT psrcWidth, LPINT psrcHeight )
//************************************************************************
{
	if ( !pdibDst || !pdstLeft || !pdstTop || !pdstWidth || !pdstHeight ||
		 !pdibSrc || !psrcLeft || !psrcTop || !psrcWidth || !psrcHeight )
		return( NO );

	RECT rDst;
	SetRect( &rDst, *pdstLeft, *pdstTop, *pdstLeft + *pdstWidth, *pdstTop + *pdstHeight );
	RECT rSrc;
	SetRect( &rSrc, *psrcLeft, *psrcTop, *psrcLeft + *psrcWidth, *psrcTop + *psrcHeight );

	if ( !DibClipRect( pdibDst, &rDst, NULL/*prDstClip*/,
					   pdibSrc, &rSrc, NULL/*prSrcClip*/ ) )
		return( NO );

	*pdstLeft   = rDst.left;
	*pdstTop    = rDst.top;
	*pdstWidth  = rDst.right - rDst.left;
	*pdstHeight = rDst.bottom - rDst.top;
	*psrcLeft   = rSrc.left;
	*psrcTop    = rSrc.top;
	*psrcWidth  = rSrc.right - rSrc.left;
	*psrcHeight = rSrc.bottom - rSrc.top;

	return( YES );
}

// Note we only support 8bpp DIBs here, and..
// there is no regard for color table matching between the DIBs
//************************************************************************
void CDib::DibBlt(CDib * pdibDst, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				  int srcLeft, int srcTop, int srcWidth, int srcHeight,
				  BOOL bTransparent, LPRGBTRIPLE lpRGBDummy )
//************************************************************************
{
	if ( !pdibDst )
		return;
	if ( GetBitCount() != 8 )
		return;
	if ( pdibDst->GetBitCount() != 8 )
		return;
	if ( pdibDst->GetCompression() != BI_RGB )// the destination can't be compressed...
		return;								// but the source can

	// Check the coordinate bounds, to avoid an out of range pointer
	if ( !DibClip( pdibDst, &dstLeft, &dstTop, &dstWidth, &dstHeight,
					this,	&srcLeft, &srcTop, &srcWidth, &srcHeight ) )
		return;

	// Get the first pixel in the image as the transparent color
	LPTR lp = GetXY(0,0);
	if (!lp)
		return;
	BYTE cTransparent = *lp;

	#ifdef USE_DIBFX

	if ( !bTransparent )
	{
		MyDibBlt(
			pdibDst->GetInfoHeader(), pdibDst->GetPtr(), dstLeft, dstTop,
					 GetInfoHeader(),          GetPtr(), srcLeft, srcTop,
					 srcWidth, srcHeight);
	}
	else
	{
		MyDibTransparentBlt(
			pdibDst->GetInfoHeader(), pdibDst->GetPtr(), dstLeft, dstTop,
					 GetInfoHeader(),          GetPtr(), srcLeft, srcTop,
					 srcWidth, srcHeight, cTransparent);
	}

	#else // USE_DIBFX

	// Compute the sampling scale factors
	LFIXED fy = fget(srcHeight,dstHeight);
	LFIXED fx = fget(srcWidth, dstWidth);

	// Get the scan line widths of the destination dib
	int dstWidthBytes = pdibDst->GetWidthBytes();
	if ( pdibDst->GetHeight() > 0 )
		dstWidthBytes = -dstWidthBytes;
	
	// Get the destination pointer for the first line
	HPTR pDst = pdibDst->GetXY( dstLeft, dstTop );
	if ( !pDst )
		return;

	LFIXED fsy = 0;
	while ( --dstHeight >= 0 )
	{
		// Get the source pointer for each line
		HPTR pSrc = GetXY( srcLeft, srcTop + WHOLE(fsy) );
		if ( !pSrc )
			break;

		fsy += fy;

		HPTR p = pDst;
		int iWidth = dstWidth;

		LFIXED fsx = 0;
		while ( --iWidth >= 0 )
		{
			BYTE pixel = *(pSrc + WHOLE(fsx));
			fsx += fx;

			if (!bTransparent || pixel != cTransparent)
				*p++ = pixel;
			else
				 p++;
		}

		pDst += dstWidthBytes;
	}

	#endif // USE_DIBFX
}

//************************************************************************
void CDib::DCBlt(  HDC hDC, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				int srcLeft, int srcTop, int srcWidth, int srcHeight, DWORD rop )
//************************************************************************
{ // a substiture for WinGBitBlt(), or to load a WinG DIB(DC) with bits

	if ( !hDC || !GetPtr() )
		return;

	int y, iSign;
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
	HBITMAP hbm;
	HPALETTE hpal = NULL;
	PDIB pdib;

	if (GetNumColors() > 3)
		hpal = CreateIdentityPalette( GetColors(), GetNumColors(), NO/*bMakeIdentity*/ );
	// pass -1 as wUsage so we dont get a device bitmap, see
	// BitmapFromDib for the Hack. NOTE only in Win32
	hbm = CreateBitmap(hpal, (UINT)-1 /*DIB_RGB_COLORS*/);

	if (hbm)
	{
		pdib = new CDib();
		if (pdib)
			pdib->Create(hbm, biCompression, BitCount, hpal, DIB_RGB_COLORS);
	}
	else
		pdib = NULL;

	if (hbm)
		DeleteObject(hbm);

	if (hpal)
		DeleteObject(hpal);

	return pdib;
}

//************************************************************************
BOOL CDib::Create( HBITMAP hbm, DWORD biStyle, UINT biBits, HPALETTE hpal, UINT wUsage )
//************************************************************************
{
	BITMAP bm;
	int nColors=0;
	UINT u;
	BOOL fRet = FALSE;

	if ( !hbm )
		return FALSE;

	if ( !hpal )
		hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	GetObject( hbm, sizeof(bm), (LPVOID)&bm );
	GetObject( hpal, sizeof(nColors), (LPVOID)&nColors );

	if ( !biBits )
		biBits = bm.bmPlanes * bm.bmBitsPixel;

	m_bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	m_bmiHeader.biWidth				= bm.bmWidth;
	m_bmiHeader.biHeight			= bm.bmHeight;
	m_bmiHeader.biPlanes			= 1;
	m_bmiHeader.biBitCount			= biBits;
	m_bmiHeader.biCompression		= biStyle;
	m_bmiHeader.biSizeImage			= 0;
	m_bmiHeader.biXPelsPerMeter		= 0;
	m_bmiHeader.biYPelsPerMeter		= 0;
	m_bmiHeader.biClrImportant		= 0;
	m_bmiHeader.biClrUsed			= 0;
	m_bmiHeader.biClrUsed			= GetNumColors();

	// this is a silly hack to test Win32/NT and BI_BITFIELDS
	// convert to a 565 (for 16 bit) or a RGB DIB (for 32/24 bit)
	if (biStyle == BI_BITFIELDS)
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
									   
	// HACK! if the driver did not fill in the biSizeImage field, make one up
	if ( !m_bmiHeader.biSizeImage )
	{
		m_bmiHeader.biSizeImage = GetSizeImage();

		if (biStyle != BI_RGB)
			m_bmiHeader.biSizeImage = (m_bmiHeader.biSizeImage * 3) / 2;
	}

	// Alloc the buffer big enough to hold all the bits
	if ( !(m_lp = AllocX( GetSizeImage(), GMEM_ZEROINIT )) )
	{
		goto exit;
	}

	//	call GetDIBits with a NON-NULL lpBits param, and actually get the
	//	bits this time
	u = GetDIBits(hdc, hbm, 0, (int)abs(m_bmiHeader.biHeight),
		GetPtr(),(LPBITMAPINFO)GetInfo(), wUsage);

	m_bmiHeader.biClrUsed = GetNumColors();
	m_bmiHeader.biSizeImage = GetSizeImage();

	// error getting bits.
	if (u)
	{
		fRet = TRUE;
	}

exit:
	SelectPalette(hdc,hpal,FALSE/*bProtectPhysicalPal*/);
	DeleteDC(hdc);
	return fRet;
}

//************************************************************************
HBITMAP CDib::CreateBitmap( HPALETTE hpal, UINT wUsage )
//************************************************************************
{
	HPALETTE	hpalT;
	HDC			hdc;
	HBITMAP		hbm;
	DWORD		dwFlags = CBM_INIT;

	if ( !(hdc = GetDC(NULL)) )
		return( NULL );

	if (hpal)
	{
		hpalT = SelectPalette(hdc,hpal,FALSE/*bProtectPhysicalPal*/);
		RealizePalette(hdc);
	}

	#ifdef WIN32
		SetStretchBltMode(hdc, HALFTONE);		//??? does this do anything?
		if (wUsage == (UINT)-1)	 // silly hack for ConvertDib
			if (!m_bmiHeader.biCompression ||
				m_bmiHeader.biCompression == BI_BITFIELDS)
				dwFlags |= CBM_INIT;
	#endif

	if (wUsage == (UINT)-1)
		wUsage = DIB_RGB_COLORS;	// silly hack for ConvertDib

	hbm = CreateDIBitmap(hdc, &m_bmiHeader, dwFlags, GetPtr(), (LPBITMAPINFO)&m_bmiHeader, wUsage);

	if (hpal && hpalT)
		SelectPalette(hdc,hpalT,FALSE/*bProtectPhysicalPal*/);

	ReleaseDC(NULL,hdc);

	return hbm;
}

//************************************************************************
BOOL CDib::CheckDecodeRLE(BOOL fDecodeRLE)
//************************************************************************
{
	if (!m_lp)
		return(FALSE);

	if ((m_bmiHeader.biCompression == BI_RLE8) && fDecodeRLE)
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
		m_bmiHeader.biCompression = BI_RGB;
		SetSizeImage(dwSize);
	}
	return TRUE;
}
