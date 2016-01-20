// DibData.cpp

#include "stdafx.h"
#include "DibData.h"
#include "Dib.h"
#include "HSL.h"
#include "Utility.h"
#include <math.h>

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
CDibData::CDibData()
{
	m_pDib             = NULL;
	m_pBits            = NULL;
	m_pLine            = NULL;
	m_pColorTable      = NULL;
	m_pBufferDib	   = NULL;
	m_wBitsPerPixel    = 0;
	m_dwColorTableSize = 0;
	m_fTolerance	   = 0.0;
	m_SelectedTool	   = TOOL_OFF;
//j	m_nNumPixels	   = 0;
//j	m_dwAccumRed	   = 0;
//j	m_dwAccumGreen	   = 0;
//j	m_dwAccumBlue	   = 0; 
//j	m_fAverageColors   = false;

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
bool CDibData::DeleteObject()
{
	DibDataFree(m_pDib);

	delete [] m_pLine;

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
//Call this member to detach m_pDib from the CDibData object.
LPBYTE CDibData::Detach()
{
	LPBYTE pDib = m_pDib;
	if( m_pDib )
	{
		m_pDib = NULL;
		DeleteObject();
	}
	return m_pDib;
}

/////////////////////////////////////////////////////////////////////////////
void CDibData::SetTolerance(int nValue)
{
	m_fTolerance = (double)nValue * 2.55;
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
bool CDibData::InitDIB(BITMAPINFOHEADER* pbmpSrc, bool bCompress)
{
	// Always use bit-field compression for 16 & 32 bit DIBs
	CDibDataVars ddv(pbmpSrc);
	if( 16 == ddv.m_wBpp || ddv.m_wBpp == 32 )
		ddv.m_dwCompression = BI_BITFIELDS;

	SetDefaultBitfields(ddv.m_wBpp);

	// Allocate storage
	LPBYTE pDib = DibDataAlloc(ddv.m_dwMaxDibSize);
	if (!pDib)
		return FALSE;
	
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
		::SetLastError(ERROR_OUTOFMEMORY);
		return FALSE;
	}

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
		case 24: 
			return 0x00FFFFFF & *(LPDWORD)pPixel;
		case 32: 
			return *(LPDWORD)pPixel;
	}

	return CLR_INVALID;
}

/////////////////////////////////////////////////////////////////////////////
bool CDibData::SetPixelValue(int x, int y, DWORD dwValue, SelectedTool selectedTool)
{
	if( !m_pLine )
		return FALSE;

	if( x < 0 || GetWidth()  <= x || y < 0 || abs(GetHeight()) <= y )
		return FALSE;

	LPBYTE pPixel = m_pLine[y] + ((x * m_wBitsPerPixel) >> 3);

	//Must be doing a gradient
	if (GetBufferDib())
	{
		if (TOOL_REDEYE != selectedTool)
		{
			dwValue = GetBufferDib()->GetPixel(x, y); //dwFromValue;
		}
		else
		{
			dwValue = GetPixel(x, y); 
			RedEyeRemove(dwValue);
			*(RGBTRIPLE*)pPixel = *(RGBTRIPLE*)&dwValue;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CDibData::GetPixel(int x, int y) const
{
	return ValueToRGB(GetPixelValue(x, y));
}

/////////////////////////////////////////////////////////////////////////////
bool CDibData::SetPixel(int x, int y, COLORREF clPixel, SelectedTool selectedTool)
{
	if( clPixel == CLR_INVALID)
		return FALSE;

	DWORD dwValue = RGBtoValue(clPixel);

	if( dwValue == CLR_INVALID)
		return FALSE;

	return SetPixelValue(x, y, dwValue, selectedTool);
}

/////////////////////////////////////////////////////////////////////////////
bool CDibData::DrawLineH(int x1, int x2, int y, DWORD dwFromValue, DWORD dwToValue, SelectedTool selectedTool, RECT* pFloodRect)
{
	if( !m_pLine || ((dwFromValue == CLR_INVALID) && (TOOL_REDEYE != selectedTool)))
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

	int nCurrentXPos = x1;
	LPBYTE pStart = m_pLine[y] + ((x1 * m_wBitsPerPixel) >> 3);
	LPBYTE pEnd   = m_pLine[y] + ((x2 * m_wBitsPerPixel) >> 3);
	DWORD dwColor = 0;

	for( ; pStart <= pEnd; pStart += sizeof(RGBTRIPLE) )
	{
		if (GetBufferDib())
		{
			if (TOOL_REDEYE != selectedTool)
			{
				dwColor = GetBufferDib()->GetPixel(nCurrentXPos++, y); //dwFromValue;
			}
			else
			{
				dwColor = GetPixel(nCurrentXPos++, y); 
				RedEyeRemove(dwColor);
			}
		}
		else
			dwColor = dwFromValue; 

		*(RGBTRIPLE*)pStart = *(RGBTRIPLE*)&dwColor;

	}

	return TRUE;
}

#ifdef NOTUSED //j
void CDibData::SetColorAverageState(bool fAverage)
{
	m_dwAccumRed = 0;
	m_dwAccumGreen = 0;
	m_dwAccumBlue = 0;
	m_fAverageColors = fAverage;
}

void CDibData::SetColorForAverage(DWORD dwRed, DWORD dwGreen, DWORD dwBlue)
{
	m_nNumPixels++;
	m_dwAccumRed += dwRed;
	m_dwAccumGreen += dwGreen;
	m_dwAccumBlue += dwBlue;
}

void CDibData::GetColorAverage(RGBTRIPLE* pRGB)
{
	if (m_nNumPixels)
	{
		pRGB->rgbtRed	= (BYTE)(m_dwAccumRed / m_nNumPixels);
		pRGB->rgbtGreen = (BYTE)(m_dwAccumGreen / m_nNumPixels);
		pRGB->rgbtBlue	= (BYTE)(m_dwAccumBlue / m_nNumPixels);
	}
}
#endif NOTUSED //j

void CDibData::RedEyeRemove(COLORREF& RedEyePixel)
{
	int nWhiteIndex = 0;
	RGBTRIPLE rgbq;
	rgbq.rgbtRed   = GetRValue(RedEyePixel);
	rgbq.rgbtGreen = GetGValue(RedEyePixel);
	rgbq.rgbtBlue  = GetBValue(RedEyePixel);

	//Try to adjust the white pixels
	if (rgbq.rgbtRed >= 200)
		nWhiteIndex++;

	if (rgbq.rgbtGreen >= 200)
		nWhiteIndex++;

	if (rgbq.rgbtBlue >= 200)
		nWhiteIndex++;

	//Assume that the pixel was meant to be white
	if (nWhiteIndex > 1)
	{
		int nWhiteLevel = max(rgbq.rgbtRed, max(rgbq.rgbtGreen, rgbq.rgbtBlue));
		RedEyePixel = RGB(nWhiteLevel, nWhiteLevel, nWhiteLevel);
	}

//#define ZHANG
#ifdef ZHANG //Zhang recolor
	else
	{
		RGBTRIPLE RGBAve;
		GetColorAverage(&RGBAve);
		//CColor Color;
		//Color.SetRGB(RGBAve.rgbtRed, RGBAve.rgbtGreen, RGBAve.rgbtBlue);
		float fFactor = Factor(RGBAve.rgbtRed, RGBAve.rgbtGreen, RGBAve.rgbtBlue);
		BYTE r = AdjustBrightness(rgbq.rgbtRed, -fFactor);
		BYTE g = AdjustBrightness(rgbq.rgbtGreen, fFactor / 3);
		BYTE b = AdjustBrightness(rgbq.rgbtBlue, fFactor / 3);

		BYTE rr = AdjustContrast(r, -10, lL/*iMeanLight*/);
		BYTE gg = AdjustContrast(g, -10, lL/*iMeanLight*/);
		BYTE bb = AdjustContrast(b, -10, lL/*iMeanLight*/);
		RedEyePixel = RGB(r, g, b);
	}
#else //MDJ recolor
	else
	{
		rgbq.rgbtRed = max(rgbq.rgbtGreen,rgbq.rgbtBlue);
		RedEyePixel = RGB(min(rgbq.rgbtRed, 255), min(rgbq.rgbtGreen * 1.20, 255), min(rgbq.rgbtBlue * 1.20, 255));
	}
#endif
}

////////////////////////////////////////////////////////////////////////////
int CDibData::Convert(int nPos)
{
	return ((nPos * m_wBitsPerPixel) >> 3);
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
				if( !IsColorToBeFilled(dwValue, (0x00FFFFFF & *(LPDWORD)pPixel)))
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
				if( IsColorToBeFilled(dwValue, (0x00FFFFFF & *(LPDWORD)pPixel)) )
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
				if( !IsColorToBeFilled(dwValue, (0x00FFFFFF & *(LPDWORD)pPixel)) )
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
				if( IsColorToBeFilled(dwValue, (0x00FFFFFF & *(LPDWORD)pPixel)) )
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

bool CDibData::IsColorToBeFilled(DWORD dwColorToFill, DWORD dwPixelColor)
{
	bool fRet = false;

	BYTE PixelColorRed, PixelColorGreen, PixelColorBlue;
	RGBTRIPLE PixelColorrgbt = *(RGBTRIPLE*)&dwPixelColor;
	PixelColorRed = PixelColorrgbt.rgbtRed, PixelColorGreen = PixelColorrgbt.rgbtGreen, PixelColorBlue = PixelColorrgbt.rgbtBlue;

	if (TOOL_REDEYE == m_SelectedTool)
	{
		float fColorRed = ((float)PixelColorRed / (float)(PixelColorRed + PixelColorGreen + PixelColorBlue));
		float fColorGreen = ((float)PixelColorGreen /(float)(PixelColorRed + PixelColorGreen + PixelColorBlue));
		float fColorBlue = ((float)PixelColorBlue /(float)(PixelColorRed + PixelColorGreen + PixelColorBlue));

		HSL hsl;
		RGBtoHSL(PixelColorRed, PixelColorGreen, PixelColorBlue, &hsl);

		//Try to adjust the white pixels if the user clicks directly on a white part of an eye.
		//I need to account for this and continue. This may not be pure white in the pict. 
		int nWhiteIndex = 0;
		if (PixelColorRed >= 200)
			nWhiteIndex++;

		if (PixelColorGreen >= 200)
			nWhiteIndex++;

		if (PixelColorBlue >= 200)
			nWhiteIndex++;

		//Assume that the pixel was meant to be white
		if (PixelColorRed && nWhiteIndex > 1)
		{
			return true;
		}
			
		if ((hsl.hue >= 0 && hsl.hue <= 10) || hsl.hue >= 200)
		{
			if (hsl.sat < 98 && hsl.lum < 98)
				return false;

			if ((PixelColorRed > 50) && (fColorRed > 0.40) && (fColorGreen < 0.31) && (fColorBlue < 0.36))
			{
				if ((4 * PixelColorRed -(PixelColorGreen + PixelColorBlue) - min(PixelColorGreen,PixelColorBlue) - max(PixelColorGreen,PixelColorBlue))/PixelColorRed)
				{
#ifdef NOTUSED //j
					//For red eye, we need to take an average value for each color
					if (m_fAverageColors)
					{
						SetColorForAverage(PixelColorRed, PixelColorGreen, PixelColorBlue);
					}
#endif NOTUSED //j

					return true;
				}
			}
		}
	}
	else
	{
		BYTE ColorToFillRed, ColorToFillGreen, ColorToFillBlue;
		RGBTRIPLE ColorToFillrgbt = *(RGBTRIPLE*)&dwColorToFill;
		ColorToFillRed = ColorToFillrgbt.rgbtRed, ColorToFillGreen = ColorToFillrgbt.rgbtGreen, ColorToFillBlue = ColorToFillrgbt.rgbtBlue;

		if (dwColorToFill == dwPixelColor)
		{
			return true;
		}

		int nMaxDiff = max(max(abs(ColorToFillRed - PixelColorRed), abs(ColorToFillGreen - PixelColorGreen)), abs(ColorToFillBlue - PixelColorBlue));
		if (nMaxDiff <= m_fTolerance)
			fRet = true;
	}

	return fRet;
}

float CDibData::Factor(float fAvgR, float fAvgG, float fAvgB)
{
	float fMax = max(fAvgG, fAvgB);
	float fVal = ((fAvgR -  fMax) / (float)(255 * 3.2));
	return fVal;
}

BYTE CDibData::AdjustBrightness(BYTE bColor, float fFactor)
{
	return (BYTE)(0.4 * fFactor * 255 + bColor);
}

BYTE CDibData::AdjustContrast(BYTE bColor, float fFactor, BYTE bMeanL)
{
	return (BYTE)(bColor + (float)(bColor - bMeanL) * fFactor);
}

void CDibData::DrawRedEyeCircle(LPRECT pRect, CAGClientDC* pDC /*, COLORREF cLeft, COLORREF cRight*/)
{
	//First lets draw a circle in the bitmap with a radius about a third smaller
	//  then the smallest dimension of the bitmap
	CRect Rect(pRect);
	int		nDiameter = (min(Rect.Height(), Rect.Width()) * 2) / 3;
	CRect	rcCircle(0, 0, nDiameter, nDiameter);
	rcCircle.OffsetRect((Rect.Width() - nDiameter) / 2, (Rect.Height() - nDiameter)/2);


	//For each pixel inside the circle...
	CPoint	ptCenter   = rcCircle.CenterPoint();
	int		nSqrRadius = nDiameter * nDiameter / 4;
	for(int x=rcCircle.left; x<rcCircle.right; x++)
	{
		int nDX = x - ptCenter.x;
		for(int y=rcCircle.top; y<rcCircle.bottom; y++)
		{
			int nDY = y - ptCenter.y;
			if( (nDX * nDX + nDY * nDY) < nSqrRadius)
			{
				COLORREF Color = 0;
				GetPixelColor(rcCircle.left + x, rcCircle.top + y, Color);

				//DWORD dwValue = RGBtoValue(Color);
				LPBYTE pStart = m_pLine[rcCircle.top + y] + (((rcCircle.left + x) * m_wBitsPerPixel) >> 3);
				*(RGBTRIPLE*)pStart = *(RGBTRIPLE*)&Color; //dwValue;
			}
		}
	}

	//////CRect Rect(pRect);
	//////int iWidth = Rect.Width(); //dtoi(sqrt((double)((px * px) + (py * py))));
	//////int iHeight = Rect.Height(); //dtoi((double)(sqrt((double)((px * px) + (py * py)))));

	//////int cx = Rect.Width() / 2;
	//////int cy = Rect.Height() / 2;

	//////for (int x = 0; x < Rect.Width(); x++) 
	//////{
	//////	for (int y = 0; y < Rect.Height(); y++) 
	//////	{
	//////		COLORREF Color = 0;
	//////		GetPixelColor(Rect.left + x, Rect.top + y, Color);

	//////		DWORD dwValue = RGBtoValue(Color);
	//////		LPBYTE pStart = m_pLine[Rect.top + y] + (((Rect.left + x) * m_wBitsPerPixel) >> 3);
	//////		*(RGBTRIPLE*)pStart = *(RGBTRIPLE*)&dwValue;
	//////	}
	//////}
}

void CDibData::GetPixelColor(int x, int y, COLORREF &Color)
{
	DWORD ThisColorValue = GetPixel(x,y);
	RGBTRIPLE rgbq = *(RGBTRIPLE*)&ThisColorValue;

	int nWhiteIndex = 0;
	rgbq.rgbtRed   = GetRValue(ThisColorValue);
	rgbq.rgbtGreen = GetGValue(ThisColorValue);
	rgbq.rgbtBlue  = GetBValue(ThisColorValue);

	//Try to adjust the white pixels
	if (rgbq.rgbtRed >= 200)
		nWhiteIndex++;

	if (rgbq.rgbtGreen >= 200)
		nWhiteIndex++;

	if (rgbq.rgbtBlue >= 200)
		nWhiteIndex++;

	//Assume that the pixel was meant to be white
	if (nWhiteIndex > 1)
	{
		int nWhiteLevel = max(rgbq.rgbtRed, max(rgbq.rgbtGreen, rgbq.rgbtBlue));
		Color = RGB(nWhiteLevel, nWhiteLevel, nWhiteLevel);
	}
	else
	{
		rgbq.rgbtRed = min(rgbq.rgbtGreen,rgbq.rgbtBlue);
		Color = RGB(rgbq.rgbtRed, rgbq.rgbtGreen, rgbq.rgbtBlue);
	}

	//else
	//{
	//	RGBTRIPLE RGBAve;
	//	GetColorAverage(&RGBAve);
	//	float fFactor = Factor(RGBAve.rgbtRed, RGBAve.rgbtGreen, RGBAve.rgbtBlue);
	//	BYTE r = AdjustBrightness(rgbq.rgbtRed, -fFactor);
	//	BYTE g = AdjustBrightness(rgbq.rgbtGreen, fFactor / 3);
	//	BYTE b = AdjustBrightness(rgbq.rgbtBlue, fFactor / 3);

	//	BYTE rr = AdjustContrast(r, -10, lL/*iMeanLight*/);
	//	BYTE gg = AdjustContrast(g, -10, lL/*iMeanLight*/);
	//	BYTE bb = AdjustContrast(b, -10, lL/*iMeanLight*/);
	//	Color = RGB(rr, gg, bb);
	//}
}

/////////////////////////////////////////////////////////////////////////////
bool CDibData::FillRectBand(DWORD dwValue, RECT* pFloodRect)
{
	for (int y = pFloodRect->top; y < pFloodRect->bottom; y++)
	{
		LPBYTE pStart = m_pLine[y] + Convert(pFloodRect->left);
		LPBYTE pEnd   = m_pLine[y] + Convert(pFloodRect->right);

		for( ; pStart < pEnd; pStart += sizeof(RGBTRIPLE) )
		{
			*(RGBTRIPLE*)pStart = *(RGBTRIPLE*)&dwValue;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CDibData::DrawGradRect(LPRECT Rect, COLORREF cLeft, COLORREF cRight, bool a_bVertical)
{
	COLORREF color;				// color for the bands
	float fStep;
	RECT stepR;
	CRect r(Rect);
	
	if(a_bVertical)
		fStep = ((float)r.Height())/255.0f;	
	else
		fStep = ((float)r.Width())/255.0f;	// width of color's band
	
	for (int iOnBand = 0; iOnBand < 255; iOnBand++) 
	{
		// set current band
		if(a_bVertical)
		{
			SetRect(&stepR,
				r.left, 
				r.top+(int)(iOnBand * fStep),
				r.right, 
				r.top+(int)((iOnBand+1)* fStep));	
		}
		else
		{
			SetRect(&stepR,
				r.left+(int)(iOnBand * fStep), 
				r.top,
				r.left+(int)((iOnBand+1)* fStep), 
				r.bottom);	
		}

		// set current color
		color = RGB((GetRValue(cRight)-GetRValue(cLeft))*((float)iOnBand)/255.0f+GetRValue(cLeft),
			(GetGValue(cRight)-GetGValue(cLeft))*((float)iOnBand)/255.0f+GetGValue(cLeft),
			(GetBValue(cRight)-GetBValue(cLeft))*((float)iOnBand)/255.0f+GetBValue(cLeft));

		// fill current band
		FillRectBand(color, &stepR);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDibData::DrawRadialGradient(CRect rect, COLORREF clrFrom, COLORREF clrTo, bool bCenter)
{
	int iWidth = rect.Width(); 
	int iHeight = rect.Height(); 

	int iRadius = min(iWidth, iHeight);
	if (bCenter)
		iRadius /= 2;

	int cx = (bCenter ? rect.Width() / 2 : 0);
	int cy = (bCenter ? rect.Height() / 2 : 0);

	int r1 = GetRValue(clrFrom);
	int g1 = GetGValue(clrFrom);
	int b1 = GetBValue(clrFrom);
	int r2 = GetRValue(clrTo);
	int g2 = GetGValue(clrTo);
	int b2 = GetBValue(clrTo);

	int iRadius2 = iRadius * iRadius;
	double dr = (double)(r1 - r2) / iRadius2;
	double dg = (double)(g1 - g2) / iRadius2;
	double db = (double)(b1 - b2) / iRadius2;

	for (int x = 0; x <= rect.Width(); x++) 
	{
		int x2 = x - cx; x2 *= x2;
		for (int y = 0; y < rect.Height(); y++) 
		{
			int y2 = y - cy; y2 *= y2;
			int x2y2 = x2 + y2;

			COLORREF Color = 0;
			int iMultiplier = iRadius2 - x2y2;
			if (iMultiplier <= 0)
				Color = RGB(r2,g2,b2);
			else
			{
				int r = r2 + (int)(dr * iMultiplier);
				int g = g2 + (int)(dg * iMultiplier);
				int b = b2 + (int)(db * iMultiplier);
				Color = RGB(r,g,b);
			}

			LPBYTE pStart = m_pLine[rect.top + y] + (((rect.left + x) * m_wBitsPerPixel) >> 3);
			*(RGBTRIPLE*)pStart = *(RGBTRIPLE*)&Color;
		}
	}
}

#ifdef NOTUSED
void CDibData::DrawGradRect(RECT* pRect, COLORREF cLeft, COLORREF cRight, bool bVertical)
{
	BYTE R = GetRValue(cLeft);
	BYTE G = GetGValue(cLeft);
	BYTE B = GetBValue(cLeft);
	int dR = (int)GetRValue(cRight) - R;
	int dG = (int)GetGValue(cRight) - G;
	int dB = (int)GetBValue(cRight) - B;

	CRect Rect(pRect);
	int dx = Rect.Width() + 1;
	int dy = Rect.Height() + 1;
	int nColors = (bVertical ? dy : dx);
	RGBTRIPLE* pColors = new RGBTRIPLE[nColors];
	for (int i = 0; i < nColors; i++) 
	{
		double fScale = (double)i / (nColors-1);
		RGBTRIPLE* pColor = &pColors[i];
		pColor->rgbtBlue  = R + (int)(fScale * dR); //j This is wrong but there is a bug somewhere else
		pColor->rgbtGreen = G + (int)(fScale * dG);
		pColor->rgbtRed   = B + (int)(fScale * dB);
	}
		
	for (int y = 0; y < dy; y++) 
	{
		RGBTRIPLE* pPixel = (RGBTRIPLE*)m_pLine[y];
		RGBTRIPLE yColor = (bVertical ? pColors[y] : pColors[0]);
		for (int x = 0; x < dx; x++) 
		{
			*(RGBTRIPLE*)pPixel = (bVertical ? yColor : *(RGBTRIPLE*)&pColors[x]);
			pPixel++;
		}
	}

	delete pColors;
}
#endif NOTUSED
