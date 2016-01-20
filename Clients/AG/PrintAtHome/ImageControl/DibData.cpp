// DibData.cpp

#include "stdafx.h"
#include "DibData.h"
#include "Dib.h"

class CDibDataVars
{
	CDibDataVars::CDibDataVars() {}	// No default constructor
public:
	CDibDataVars(const LPBITMAPINFOHEADER pInfo);
	~CDibDataVars() {}

	void SetDataVars(const LPBITMAPINFOHEADER pInfo);

	DWORD m_dwCompression;		//!< Compression type
	WORD  m_wBpp;				//!< Number of bits-per-pixel
	DWORD m_dwByteWidth;		//!< Number of bytes-per-line
	DWORD m_dwImageSize;		//!< Size of image
	DWORD m_dwDibSize;			//!< Size of DIB
	DWORD m_dwFieldSize;		//!< Size of bit-fields section
	DWORD m_dwNumColors;		//!< Number of colors used
	DWORD m_dwSizeColorTable;	//!< Size of color table
	DWORD m_dwImageOffset;		//!< Offset to image data
	DWORD m_dwFieldOffset;		//!< Offset to bit-fields
	DWORD m_dwColorOffset;		//!< Offset to color-table
	DWORD m_dwMaxDibSize;		//!< Maximum uncompressed DIB size
	DWORD m_dwMaxColor;			//!< Maximum number of possible colors
};

/////////////////////////////////////////////////////////////////////////////
CDibDataVars::CDibDataVars(const LPBITMAPINFOHEADER pInfo)
{ 
	SetDataVars(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
void CDibDataVars::SetDataVars(const LPBITMAPINFOHEADER pInfo)
{
	m_dwCompression = pInfo->biCompression;
	m_wBpp          = WORD(pInfo->biPlanes * pInfo->biBitCount);
	m_dwByteWidth   = (((pInfo->biWidth * m_wBpp + 31) & ~31) >> 3);
	m_dwImageSize   = m_dwByteWidth * abs(pInfo->biHeight);
	m_dwMaxDibSize  = pInfo->biSize + m_dwImageSize;
	m_dwDibSize     = pInfo->biSize;
	m_dwFieldSize   = 0L;
	m_dwMaxColor    = 0L;
	m_dwNumColors   = 0L;
	m_dwImageOffset = 0L;
	m_dwFieldOffset = 0L;
	m_dwColorOffset = 0L;
	m_dwSizeColorTable = 0L;

	if( m_wBpp <= 1 )
		m_wBpp = 1;
	else if( m_wBpp <= 2 )
		m_wBpp = 2;
	else if( m_wBpp <= 4 )
		m_wBpp = 4;
	else if( m_wBpp <= 8 )
		m_wBpp = 8;
	else if( m_wBpp <= 16 )
		m_wBpp = 16;
	else if( m_wBpp <= 24 )
		m_wBpp = 24;
	else
		m_wBpp = 32;

	if( pInfo->biSizeImage )
		m_dwImageSize = pInfo->biSizeImage;

	if( BI_BITFIELDS == m_dwCompression )
	{
		m_dwFieldOffset = m_dwDibSize;
		m_dwFieldSize   = 3 * sizeof(DWORD);
		m_dwDibSize    += m_dwFieldSize;

		m_dwMaxDibSize += m_dwFieldSize;
	}

	m_dwImageOffset = m_dwDibSize;
	m_dwDibSize    += m_dwImageSize;
}

/////////////////////////////////////////////////////////////////////////////
CDibData::CDibData(const CDibData& dibSrc)
{
	m_hDib             = NULL;
	m_pDib             = NULL;
	m_pBits            = NULL;
	m_pLine            = NULL;
	m_pColorTable      = NULL;
	m_wBitsPerPixel    = 0;
	m_dwColorTableSize = 0;

	::ZeroMemory(m_dwBitfields, sizeof(m_dwBitfields));
	::ZeroMemory(m_wRShift    , sizeof(m_wRShift));
	::ZeroMemory(m_wLShift    , sizeof(m_wLShift));
}

/////////////////////////////////////////////////////////////////////////////
CDibData::CDibData() : m_hDib(NULL), m_pDib(NULL), m_pBits(NULL),
					   m_pColorTable(NULL), m_pLine(NULL),
					   m_wBitsPerPixel(0), m_dwColorTableSize(0)
{
	::ZeroMemory(m_dwBitfields, sizeof(m_dwBitfields));
	::ZeroMemory(m_wRShift    , sizeof(m_wRShift));
	::ZeroMemory(m_wLShift    , sizeof(m_wLShift));
}

/////////////////////////////////////////////////////////////////////////////
CDibData::~CDibData()
{
	DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDibData::DeleteObject()
{
	if( m_hDib && !DibUnlock(m_hDib) )
		DibFree(m_hDib);

	delete [] m_pLine;

	m_hDib             = NULL;
	m_pDib             = NULL;
	m_pBits            = NULL;
	m_pLine            = NULL;
	m_pColorTable      = NULL;
	m_wBitsPerPixel    = 0;
	m_dwColorTableSize = 0;

	::ZeroMemory(m_dwBitfields, sizeof(m_dwBitfields));
	::ZeroMemory(m_wRShift    , sizeof(m_wRShift));
	::ZeroMemory(m_wLShift    , sizeof(m_wLShift));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//Call this member to detach m_hDib from the CDibData object.
HANDLE CDibData::Detach()
{
	HGLOBAL hDib = m_hDib;
	if( m_hDib )
	{
		DibUnlock(m_hDib);
		m_hDib = NULL;
		DeleteObject();
	}
	return hDib;
}

/////////////////////////////////////////////////////////////////////////////
void CDibData::SetDefaultBitfields(WORD wBitsColor)
{
	if( wBitsColor == 16 )
	{
		m_dwBitfields[0] = 0x00007C00;
		m_dwBitfields[1] = 0x000003E0;
		m_dwBitfields[2] = 0x0000001F;

		m_wRShift[0] = 10;
		m_wRShift[1] =  5;
		m_wRShift[2] =  0;

		m_wLShift[0] = m_wLShift[1] = m_wLShift[2] = 3;
	}
	else if(wBitsColor == 24 || wBitsColor == 32 )
	{
		m_dwBitfields[0] = 0x00FF0000;
		m_dwBitfields[1] = 0x0000FF00;
		m_dwBitfields[2] = 0x000000FF;

		m_wRShift[0] = 16;
		m_wRShift[1] =  8;
		m_wRShift[2] =  0;

		m_wLShift[0] = m_wLShift[1] = m_wLShift[2] = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDibData::InitDIB(BITMAPINFOHEADER* pbmpSrc, BOOL bCompress)
{
	// Always use bit-field compression for 16 & 32 bit DIBs
	CDibDataVars ddv(pbmpSrc);
	if( 16 == ddv.m_wBpp || ddv.m_wBpp == 32 )
		ddv.m_dwCompression = BI_BITFIELDS;

	SetDefaultBitfields(ddv.m_wBpp);

	// Allocate storage
	HANDLE hDib = DibAlloc(ddv.m_dwMaxDibSize);
	LPBYTE pDib = DibLock(hDib);
	if( !hDib || !pDib )
	{
		if( hDib )
			DibFree(hDib);
		return FALSE;
	}
	
	// Fill in Information Header
	::CopyMemory(pDib, pbmpSrc, sizeof(BITMAPINFOHEADER) );
	((LPBITMAPINFOHEADER)pDib)->biCompression = ddv.m_dwCompression;
	if( ddv.m_dwFieldOffset )
	{
		::CopyMemory(LPDWORD(pDib + ddv.m_dwFieldOffset),
			m_dwBitfields, sizeof(m_dwBitfields));
	}

	LPBITMAPINFO pBmpInfo             = (LPBITMAPINFO)pDib;
	LPBITMAPINFOHEADER pBmpInfoHeader = (LPBITMAPINFOHEADER)pDib;

	LPBYTE pBits = pDib + ddv.m_dwImageOffset;
	memcpy(pBits, DibPtr(pbmpSrc), ddv.m_dwImageSize);

	m_pLine = new LPBYTE[abs(pBmpInfoHeader->biHeight)];
	if( !m_pLine )
	{
		DibUnlock(hDib);
		::SetLastError(ERROR_OUTOFMEMORY);
		return FALSE;
	}

	m_hDib             = hDib;
	m_pDib             = pDib;
	m_pBits            = pDib + ddv.m_dwImageOffset;
	m_wBitsPerPixel    = ddv.m_wBpp;
	m_dwColorTableSize = ddv.m_dwNumColors;

	if( ddv.m_dwColorOffset )
		m_pColorTable = LPRGBQUAD(pDib + ddv.m_dwColorOffset);

	if( pBmpInfoHeader->biHeight > 0 )
	{
		LONG y = pBmpInfoHeader->biHeight - 1;
		for( LONG i = 0; y >= 0; --y, ++i )
		{
			m_pLine[i] = m_pBits + y * ddv.m_dwByteWidth;
		}
	}
	else
	{
		LONG lHeight = abs(pBmpInfoHeader->biHeight);
		for( LONG i = 0; i < lHeight; ++i )
		{
			m_pLine[i] = m_pBits + i * ddv.m_dwByteWidth;
		}
	}
	DibUnlock(hDib);

	return TRUE; 
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDibData::RGBtoValue(COLORREF ColorRef) const
{
	if( ColorRef == CLR_INVALID )
		return CLR_INVALID;

	RGBQUAD rgbq;
	rgbq.rgbRed   = GetRValue(ColorRef);
	rgbq.rgbGreen = GetGValue(ColorRef);
	rgbq.rgbBlue  = GetBValue(ColorRef);
	rgbq.rgbReserved = 0;

	DWORD dwValue;
	if( m_wBitsPerPixel == 24 )
	{
		*(RGBTRIPLE*)&dwValue = *(RGBTRIPLE*)&rgbq;
		dwValue &= 0x00FFFFFF;
	}
	else if( m_wBitsPerPixel == 32 &&
		GetCompression() == BI_RGB )
	{
		*(LPRGBQUAD)&dwValue = rgbq;
	}
	else
	{
		dwValue  = (((DWORD)rgbq.rgbRed   >> m_wLShift[0]) << m_wRShift[0]);
		dwValue |= (((DWORD)rgbq.rgbGreen >> m_wLShift[1]) << m_wRShift[1]);
		dwValue |= (((DWORD)rgbq.rgbBlue  >> m_wLShift[2]) << m_wRShift[2]);
	}

	return dwValue;
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CDibData::ValueToRGB(DWORD dwValue) const
{
	if( dwValue == CLR_INVALID )
		return CLR_INVALID;

	BYTE Red, Green, Blue;
	if( m_wBitsPerPixel == 24 )
	{
		RGBTRIPLE rgbt = *(RGBTRIPLE*)&dwValue;
		Red = rgbt.rgbtRed, Green = rgbt.rgbtGreen, Blue = rgbt.rgbtBlue;
	}
	else if( m_wBitsPerPixel == 32 &&
		GetCompression() == BI_RGB )
	{
		RGBQUAD rgbq = *(LPRGBQUAD)&dwValue;
		Red = rgbq.rgbRed, Green = rgbq.rgbGreen, Blue = rgbq.rgbBlue;
	}
	else
	{
		Red   = (BYTE)(((m_dwBitfields[0] & dwValue) 
			>> m_wRShift[0]) << m_wLShift[0]);

		Green = (BYTE)(((m_dwBitfields[1] & dwValue)
			>> m_wRShift[1]) << m_wLShift[1]);

		Blue  = (BYTE)(((m_dwBitfields[2] & dwValue)
			>> m_wRShift[2]) << m_wLShift[2]);
	}

	return RGB(Red, Green, Blue);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDibData::GetPixelValue(int x, int y) const
{
	if( !m_pLine )
		return CLR_INVALID;

	if( x < 0 || GetWidth()  <= x || y < 0 || abs(GetHeight()) <= y )
		return CLR_INVALID;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	switch( m_wBitsPerPixel )
	{
		case  1: 
			return 0x01 & (*pPixel >> (7 - (x & 7)));
		case  2: // WinCE ?
			return 0x03 & (*pPixel >> ((3 - (x & 3)) << 1));
		case  4:
			return 0x0F & (*pPixel >> (x & 1 ? 0 : 4));
		case  8: 
			return *pPixel;
		case 16: 
			return *(LPWORD)pPixel;
		case 24: 
			return 0x00FFFFFF & *(LPDWORD)pPixel;
		case 32: 
			return *(LPDWORD)pPixel;
	}

	return CLR_INVALID;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDibData::SetPixelValue(int x, int y, DWORD dwValue)
{
	if( !m_pLine )
		return FALSE;

	if( x < 0 || GetWidth()  <= x || y < 0 || abs(GetHeight()) <= y )
		return FALSE;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	switch( m_wBitsPerPixel )
	{
		case 24:
			*(RGBTRIPLE*)pPixel = *(RGBTRIPLE*)&dwValue;
			break;

		case 32:
			*(LPDWORD)pPixel = dwValue;
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CDibData::GetPixel(int x, int y) const
{
	return ValueToRGB(GetPixelValue(x, y));
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDibData::SetPixel(int x, int y, COLORREF clPixel)
{
	if( clPixel == CLR_INVALID)
		return FALSE;

	DWORD dwValue = RGBtoValue(clPixel);

	if( dwValue == CLR_INVALID)
		return FALSE;

	return SetPixelValue(x, y, dwValue);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDibData::DrawLineH(int x1, int x2, int y, DWORD dwValue)
{
	if( !m_pLine || dwValue == CLR_INVALID )
		return FALSE;

	if( x1 > x2 )
		x1^=x2, x2^=x1, x1^=x2;

	if( x1 < 0 && x2 < 0 )
		return FALSE;

	LONG nWidth = GetWidth();

	if( x1 >= nWidth && x2 >= nWidth )
		return FALSE;

	if( x1 < 0 )
		x1 = 0;

	if( x2 >= nWidth )
		x2 = nWidth - 1;

	LPBYTE pStart = m_pLine[y] + ((x1 * m_wBitsPerPixel) >> 3);
	LPBYTE pEnd   = m_pLine[y] + ((x2 * m_wBitsPerPixel) >> 3);
	
	switch( m_wBitsPerPixel )
	{
		case 24: 
			for( ; pStart <= pEnd; pStart += sizeof(RGBTRIPLE) )
				*(RGBTRIPLE*)pStart = *(RGBTRIPLE*)&dwValue;
			break;
		case 32: 
			for( ; pStart <= pEnd; pStart += sizeof(DWORD) )
				*(LPDWORD)pStart = dwValue;
			break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CDibData::ScanRight(int x, int y, int xmax, DWORD dwValue)
{
	if( !m_pLine || xmax < 0 )
		return ++xmax;

	LONG nWidth = GetWidth();

	if( x < 0 || nWidth  <= x || y < 0 || abs(GetHeight()) <= y )
		return ++xmax;

	if( dwValue == CLR_INVALID || nWidth <= xmax )
		return ++xmax;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	switch( m_wBitsPerPixel )
	{
		case 24: 
			for( ; x <= xmax; ++x, pPixel += 3 )
			{
				if( dwValue != (0x00FFFFFF & *(LPDWORD)pPixel) )
					break;
			}
			break;
		case 32: 
			for( ; x <= xmax; ++x, pPixel += 4 )
			{
				if( dwValue != *(LPDWORD)pPixel )
					break;
			}
			break;
	}

	return x;
}

/////////////////////////////////////////////////////////////////////////////
int CDibData::SearchRight(int x, int y, int xmax, DWORD dwValue)
{
	if( !m_pLine || xmax < 0 )
		return ++xmax;

	LONG nWidth = GetWidth();

	if( x < 0 || nWidth  <= x || y < 0 || abs(GetHeight()) <= y )
		return ++xmax;

	if( dwValue == CLR_INVALID || nWidth <= xmax )
		return ++xmax;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	switch( m_wBitsPerPixel )
	{
		case 24: 
			for( ; x <= xmax; ++x, pPixel += 3 )
			{
				if( dwValue == (0x00FFFFFF & *(LPDWORD)pPixel) )
					break;
			}
			break;
		case 32: 
			for( ; x <= xmax; ++x, pPixel += 4 )
			{
				if( dwValue == *(LPDWORD)pPixel )
					break;
			}
			break;
	}

	return x;
}

/////////////////////////////////////////////////////////////////////////////
int CDibData::ScanLeft(int x, int y, int xmin, DWORD dwValue)
{
	if( !m_pLine || xmin < 0 )
		return --xmin;

	LONG nWidth = GetWidth();

	if( x < 0 || nWidth  <= x || y < 0 || abs(GetHeight()) <= y )
		return --xmin;

	if( dwValue == CLR_INVALID || nWidth <= xmin )
		return --xmin;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	switch( m_wBitsPerPixel )
	{
		case 24: 
			for( ; x >= xmin; --x, pPixel -= 3 )
			{
				if( dwValue != (0x00FFFFFF & *(LPDWORD)pPixel) )
					break;
			}
			break;
		case 32: 
			for( ; x >= xmin; --x, pPixel -= 4 )
			{
				if( dwValue != *(LPDWORD)pPixel )
					break;
			}
			break;
	}

	return x;
}

/////////////////////////////////////////////////////////////////////////////
int CDibData::SearchLeft(int x, int y, int xmin, DWORD dwValue)
{
	if( !m_pLine || xmin < 0 )
		return --xmin;

	LONG nWidth = GetWidth();

	if( x < 0 || nWidth  <= x || y < 0 || abs(GetHeight()) <= y )
		return --xmin;

	if( dwValue == CLR_INVALID || nWidth <= xmin )
		return --xmin;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	switch( m_wBitsPerPixel )
	{
		case 24: 
			for( ; x >= xmin; --x, pPixel -= 3 )
			{
				if( dwValue == (0x00FFFFFF & *(LPDWORD)pPixel) )
					break;
			}
			break;
		case 32: 
			for( ; x >= xmin; --x, pPixel -= 4 )
			{
				if( dwValue == *(LPDWORD)pPixel )
					break;
			}
			break;
	}

	return x;
}
