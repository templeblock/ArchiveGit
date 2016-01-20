#include "stdafx.h"
#include "dib.h"
//j#include "proto.h"
//j#include "wing.h"
//j#include "memory.h"
//j#include "dibfx.h"

#define BFT_ICON	0x4349	/* 'IC' */
#define BFT_BITMAP	0x4d42	/* 'BM' */
#define BFT_CURSOR	0x5450	/* 'PT' */

/* flags for _lseek */
#define	 SEEK_CUR 1
#define	 SEEK_END 2
#define	 SEEK_SET 0

// Fixed point math replacements
typedef long LFIXED;
#define fget(a,b)	((LFIXED)((65536.0 * (double)a) / (double)b))
#define fmul(n,f)	((int)(((double)n * (double)f) / 65536.0))
#define HALF		0x00008000L
#define UNITY		0x00010000L
#define ROUND(f)	((short)(((f) + HALF) >> 16))
#define WHOLE(f)	((short)HIWORD(f))
#define FRAC(f)		((unsigned short)LOWORD(f))
#define TOFIXED(n)	((n)>0 ? ((long)(n))<<16 : -(((long)-(n))<<16))

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
	memset(m_rgbQuad, 0, sizeof(m_rgbQuad));
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
CDib::CDib(PDIB pdib, HPTR lp, BOOL fCopyBits)
//************************************************************************
{
	m_bmiHeader = *pdib->GetInfoHeader();
	memcpy( m_rgbQuad, pdib->GetColors(), sizeof(m_rgbQuad));
	m_lp = lp;
	if (fCopyBits)
		memcpy(m_lp, pdib->GetPtr(), pdib->GetSizeImage());
	m_iRefCount = 0;

	Decode.x = pdib->Decode.x;
	Decode.y = pdib->Decode.y;
	Decode.yInBuf = pdib->Decode.yInBuf;
	Decode.pBuf = NULL;
	Decode.pSrc = NULL;

	AddRef();
}

//************************************************************************
CDib::CDib(LPBITMAPINFOHEADER lpbmiHeader, LPRGBQUAD lpColors, HPTR lp)
//************************************************************************
{
	m_bmiHeader = *lpbmiHeader;
	memcpy( m_rgbQuad, lpColors, sizeof(m_rgbQuad));
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
		int i = 0; //j Debug("%d references still exist for object %lx", m_iRefCount, (long)this);
	if (m_lp)
		delete [] (m_lp);

	if ( Decode.pBuf )
		delete [] ( Decode.pBuf );
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
		HPTR lp = new BYTE[dwSize];
		if (!lp)
			return(NULL);
		memset(lp, 0, dwSize);
		PDIB pDib = new CDib(this, lp, FALSE);
		if (!pDib)
		{
			delete [] (lp);
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
	HFILE fh;
	OFSTRUCT of;
	if ( (fh = OpenFile(lpFileName, &of, OF_READ)) < 0 )
		return(FALSE);
			
	if ( !(ReadBitmapInfo(fh)) )
		return FALSE;

	// Can we get enough memory to hold the DIB bits
	DWORD dwBits = GetSizeImage();
	if ( !(m_lp = new BYTE[dwBits]) )
	{
		_lclose(fh);
		return( FALSE );
	}

	memset(m_lp, 0, dwBits);

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
	HPTR lpDib;
	LPBITMAPINFO lpbmInfo; 
	DWORD dwBits;

	if ( !(hDibRes = FindResource(hInstance, szResource, RT_BITMAP)) )
		return(FALSE);
	if (! (hDib = LoadResource(hInstance, hDibRes)) )
		return(FALSE);
	lpDib = (HPTR)LockResource(hDib);
	if (!lpDib)
		return(FALSE);
	lpbmInfo = (LPBITMAPINFO)lpDib;
	m_bmiHeader = lpbmInfo->bmiHeader;
	FixHeader();
	memcpy(m_rgbQuad, lpbmInfo->bmiColors, GetNumColors() * sizeof(RGBQUAD));
	lpDib += m_bmiHeader.biSize + (GetNumColors() * sizeof(RGBQUAD));

	// Can we get enough memory to hold the DIB bits
	dwBits = GetSizeImage();
	if ( (m_lp = new BYTE[dwBits]) )
	{
		memset(m_lp, 0, dwBits);
		memcpy(m_lp, lpDib, dwBits);
	}
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
static void xlatClut8( LPBYTE ptr, DWORD dwSize, LPBYTE xlat, HPTR lpRemapped )
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
static void xlatRle8( LPBYTE ptr, DWORD dwSize, LPBYTE xlat, HPTR lpRemapped )
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
int CDib::MapToPalette( HPALETTE hpal, HPTR lpRemapped )
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
	memset(xlat, 0, sizeof(xlat));
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
	BOOL bChanged = false;
	BYTE xlat[256];
	memset(xlat, 0, sizeof(xlat));
	for ( int n=0; n<nDibColors; n++ )
	{
		int iMin = min(lpRgb->rgbRed, min(lpRgb->rgbGreen, lpRgb->rgbBlue));
		int iMax = max(lpRgb->rgbBlue, max(lpRgb->rgbGreen, lpRgb->rgbBlue));

		xlat[n] = (BYTE)((iMin + iMax)/2);
		if ( xlat[n] != n )
			bChanged = true;
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
	memcpy( m_rgbQuad, pdibSrc->GetColors(), sizeof(m_rgbQuad));
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
			if (!(lpTemp = new BYTE[cbLine]))	
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
			memcpy(lpTemp, lpTop, cbLine);
			memcpy(lpTop, lpBottom, cbLine);
			memcpy(lpBottom, lpTemp, cbLine);
	
			lpTop += cbLine;
			lpBottom -= cbLine;
		}
		
		if (lpTemp != &temp[0])
			delete [] (lpTemp);
	}
	m_bmiHeader.biHeight = -m_bmiHeader.biHeight;
	return TRUE;
}

//************************************************************************
PDIB CDib::DibCopy(BOOL fCopyBits)
//************************************************************************
{
	PDIB pdibCopy;
	HPTR lp;

	if ( !(lp = new BYTE[GetSizeImage()]) )
	{
		return( NULL );
	}

	memset(lp, 0, GetSizeImage());
	if ( !(pdibCopy = new CDib(this, lp, fCopyBits)) )
	{
		delete [] (lp);
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
	if ( !(m_lp = new BYTE[dwSizeImage + 1024]) )
		return FALSE;

	memset(m_lp, 0, dwSizeImage +1024);

	#ifdef NOTUSED // We can't use this because of a problem noted in DibToDCBlt()
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
	if ( !(m_lp = new BYTE[dwSizeImage + 1024]) )
		return FALSE;
	
	memset(m_lp, 0, dwSizeImage + 1024);
	m_bmiHeader = *pdib->GetInfoHeader();
	memcpy( m_rgbQuad, pdib->GetColors(), sizeof(m_rgbQuad));
	memcpy(m_lp, pdib->GetPtr(), pdib->GetSizeImage());
	
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
		if ( !(Decode.pBuf = new BYTE[GetWidthBytes()]) )
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
		int i = 0; // m_bmiHeader.biClrUsed = 3;					   
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

	if ( !(lpLineB = new BYTE[iWidth]) )
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
		memcpy( lpLineB, lpSwap2, iWidth );
		memcpy( lpSwap2, lpSwap1, iWidth );
		memcpy( lpSwap1, lpLineB, iWidth );
		lpSwap1 += iDelta;
		lpSwap2 += iDelta;
	}

	delete [] ( lpLineB );
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
		return( false );

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
		return( false );

	if ( srcLeft >= rSrcClip.right )
		return( false );
	if ( srcTop	 >= rSrcClip.bottom )
		return( false );
	if ( dstLeft >= rDstClip.right )
		return( false );
	if ( dstTop	 >= rDstClip.bottom )
		return( false );

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
		return( false );

	SetRect( prDst, dstLeft, dstTop, dstLeft + dstWidth, dstTop + dstHeight );
	SetRect( prSrc, srcLeft, srcTop, srcLeft + srcWidth, srcTop + srcHeight );

	return( true );
}

//************************************************************************
BOOL CDib::DibClip( PDIB pdibDst, LPINT pdstLeft, LPINT pdstTop, LPINT pdstWidth, LPINT pdstHeight,
					PDIB pdibSrc, LPINT psrcLeft, LPINT psrcTop, LPINT psrcWidth, LPINT psrcHeight )
//************************************************************************
{
	if ( !pdibDst || !pdstLeft || !pdstTop || !pdstWidth || !pdstHeight ||
		 !pdibSrc || !psrcLeft || !psrcTop || !psrcWidth || !psrcHeight )
		return( false );

	RECT rDst;
	SetRect( &rDst, *pdstLeft, *pdstTop, *pdstLeft + *pdstWidth, *pdstTop + *pdstHeight );
	RECT rSrc;
	SetRect( &rSrc, *psrcLeft, *psrcTop, *psrcLeft + *psrcWidth, *psrcTop + *psrcHeight );

	if ( !DibClipRect( pdibDst, &rDst, NULL/*prDstClip*/,
					   pdibSrc, &rSrc, NULL/*prSrcClip*/ ) )
		return( false );

	*pdstLeft   = rDst.left;
	*pdstTop    = rDst.top;
	*pdstWidth  = rDst.right - rDst.left;
	*pdstHeight = rDst.bottom - rDst.top;
	*psrcLeft   = rSrc.left;
	*psrcTop    = rSrc.top;
	*psrcWidth  = rSrc.right - rSrc.left;
	*psrcHeight = rSrc.bottom - rSrc.top;

	return( true );
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
	HPTR lp = GetXY(0,0);
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
	bLeftEdge = false;
	bRightEdge = false;
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
					bRightEdge = true;
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
					bLeftEdge = true;
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
	if ( !(m_lp = new BYTE[GetSizeImage()]) )
	{
		goto exit;
	}

	memset(m_lp, 0, GetSizeImage());

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
		HPTR lp = new BYTE[dwSize];
		if (!lp)
		{
			delete [] (m_lp);
			m_lp = NULL;
			return(FALSE);
		}

		memset(lp, 0, dwSize);
		DecodeRLE(lp);
		delete [] (m_lp);
		m_lp = lp;
		m_bmiHeader.biCompression = BI_RGB;
		SetSizeImage(dwSize);
	}
	return TRUE;
}

long const PALETTERGBFlag = 0x02000000;
long const PALETTEINDEXFlag = 0x01000000;

//***********************************************************************
// Division lookup tables.	These tables compute 0-255 divided by 51 and
// mod 51.	These tables could approximate gamma correction.
//***********************************************************************
BYTE const aDividedBy51Rounded[256] =
//***********************************************************************
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
};

//***********************************************************************
BYTE const aDividedBy51[256] =
//***********************************************************************
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5,
};

//***********************************************************************
BYTE const aMod51[256] =
//***********************************************************************
{
	 0,	 1,	 2,	 3,	 4,	 5,	 6,	 7,	 8,	 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
	38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6,
	7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
	44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
	36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0, 1, 2, 3,
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
	23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 0,
};


//***********************************************************************
// Multiplication LUTs.	 These compute 0-5 times 6 and 36.
//***********************************************************************
BYTE const aTimes6[6] =
//***********************************************************************
{
	0, 6, 12, 18, 24, 30
};

//***********************************************************************
BYTE const aTimes36[6] =
//***********************************************************************
{
	0, 36, 72, 108, 144, 180
};


//***********************************************************************
// Dither matrices for 8 bit to 2.6 bit halftones.
//***********************************************************************
BYTE const a16x16[256] =
//***********************************************************************
{
	 0, 44,	 9, 41,	 3, 46, 12, 43,	 1, 44, 10, 41,	 3, 46, 12, 43,
	34, 16, 25, 19, 37, 18, 28, 21, 35, 16, 26, 19, 37, 18, 28, 21,
	38,	 6, 47,	 3, 40,	 9, 50,	 6, 38,	 7, 47,	 4, 40,	 9, 49,	 6,
	22, 28, 13, 31, 25, 31, 15, 34, 22, 29, 13, 32, 24, 31, 15, 34,
	 2, 46, 12, 43,	 1, 45, 10, 42,	 2, 45, 11, 42,	 1, 45, 11, 42,
	37, 18, 27, 21, 35, 17, 26, 20, 36, 17, 27, 20, 36, 17, 26, 20,
	40,	 8, 49,	 5, 38,	 7, 48,	 4, 39,	 8, 48,	 5, 39,	 7, 48,	 4,
	24, 30, 15, 33, 23, 29, 13, 32, 23, 30, 14, 33, 23, 29, 14, 32,
	 2, 46, 12, 43,	 0, 44, 10, 41,	 3, 47, 12, 44,	 0, 44, 10, 41,
	37, 18, 27, 21, 35, 16, 25, 19, 37, 19, 28, 22, 35, 16, 25, 19,
	40,	 9, 49,	 5, 38,	 7, 47,	 4, 40,	 9, 50,	 6, 38,	 6, 47,	 3,
	24, 30, 15, 34, 22, 29, 13, 32, 25, 31, 15, 34, 22, 28, 13, 31,
	 1, 45, 11, 42,	 2, 46, 11, 42,	 1, 45, 10, 41,	 2, 46, 11, 43,
	36, 17, 26, 20, 36, 17, 27, 21, 35, 16, 26, 20, 36, 18, 27, 21,
	39,	 8, 48,	 4, 39,	 8, 49,	 5, 38,	 7, 48,	 4, 39,	 8, 49,	 5,
	23, 29, 14, 33, 24, 30, 14, 33, 23, 29, 13, 32, 24, 30, 14, 33,
};

//***********************************************************************
BYTE const a8x8[64] =
//***********************************************************************
{
	 0, 38,	 9, 47,	 2, 40, 11, 50,
	25, 12, 35, 22, 27, 15, 37, 24,
	 6, 44,	 3, 41,	 8, 47,	 5, 43,
	31, 19, 28, 15, 34, 21, 31, 18,
	 1, 39, 11, 49,	 0, 39, 10, 48,
	27, 14, 36, 23, 26, 13, 35, 23,
	 7, 46,	 4, 43,	 7, 45,	 3, 42,
	33, 20, 30, 17, 32, 19, 29, 16,
};

//***********************************************************************
BYTE const a4x4[16] =
//***********************************************************************
{
	 0, 25,	 6, 31,
	38, 12, 44, 19,
	 9, 35,	 3, 28,
	47, 22, 41, 15
};

//***********************************************************************
// aWinGHalftoneTranslation

//	Translates a 2.6 bit-per-pixel halftoned representation into the
//	slightly rearranged WinG Halftone Palette.
//***********************************************************************
BYTE const aWinGHalftoneTranslation[216] =
//***********************************************************************
{
	  0, 29, 30, 31, 32,249, 33, 34, 35, 36, 37, 38,
	 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 51, 52, 53, 54, 55, 56,250,250, 57, 58, 59,251,
	 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
	 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
	 84, 85, 86, 87, 88, 89,250, 90, 91, 92, 93, 94,
	 95, 96, 97, 98, 99,100,101,102,103,104,105,106,
	107,108,109,110,111,227,112,113,114,115,116,117,
	118,119,151,120,121,122,123,124,228,125,126,229,
	133,162,135,131,132,137,166,134,140,130,136,143,
	138,139,174,141,142,177,129,144,145,146,147,148,
	149,150,157,152,153,154,155,156,192,158,159,160,
	161,196,163,164,165,127,199,167,168,169,170,171,
	172,173,207,175,176,210,178,179,180,181,182,183,
	184,185,186,187,188,189,190,191,224,193,194,195,
	252,252,197,198,128,253,252,200,201,202,203,204,
	205,206,230,208,209,231,211,212,213,214,215,216,
	217,218,219,220,221,222,254,223,232,225,226,255,
};

//***********************************************************************
//	DibHalftoneDIB
//	Halftone a 24-bit DIB to the 8-bit WinG Halftone Palette
//***********************************************************************
PDIB CDib::HalftoneDIB( HPALETTE hPal )
//***********************************************************************
{
	int x, y;
	BYTE r, g, b, i;
	PDIB pDestination;
	HPTR pSourceScanline, pDestinationScanline;

	// Only work on 24-bit sources
	if ( GetBitCount() != 24 )
		return( NULL );
		
	// Create an 8-bit DIB to halftone to
	if ( !(pDestination = new CDib()) )
		return( NULL );

	if ( !(pDestination->Create(8,GetWidth(),abs(GetHeight()))) )
	{
		delete pDestination;
		return( NULL );
	}

	// Fill in the DIB color table with the halftone palette
	pDestination->SetColorTable( hPal, DIB_RGB_COLORS );

	// Step through, converting each pixel in each scan line
	// to the nearest halftone match
	for ( y = 0; y < (int)abs(GetHeight()); y++ )
		{
		pSourceScanline		 = (HPTR)GetXY( 0, y );
		pDestinationScanline = (HPTR)pDestination->GetXY( 0, y );

		for ( x = 0; x < (int)GetWidth(); x++ )
			{
			// This is the meat of the halftoning algorithm:
			// Convert an RGB into an index into the halftone palette.

			// First, extract the raw RGB information
			b = *pSourceScanline++;
			g = *pSourceScanline++;
			r = *pSourceScanline++;

			// Now, look up each value in the halftone matrix
			// using an 8x8 ordered dither.
	//		i = a4x4[	( ((x & 3)<<2) + (y & 3)) ];
	//		i = a8x8[	( ((x & 7)<<3) + (y & 7)) ];
			i = a16x16[ ( ((x &15)<<4) + (y &15)) ];
			r = aDividedBy51[r] + (aMod51[r] > i);
			g = aDividedBy51[g] + (aMod51[g] > i);
			b = aDividedBy51[b] + (aMod51[b] > i);

			// Recombine the halftoned RGB values into a palette index
			i = r + aTimes6[g] + aTimes36[b];

			// And translate through the WinG Halftone Palette
			// translation vector to give the correct value.
			*pDestinationScanline++ = aWinGHalftoneTranslation[i];
			}
		}

	return( pDestination );
}
