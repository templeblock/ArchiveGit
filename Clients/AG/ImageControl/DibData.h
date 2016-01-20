#pragma once

/////////////////////////////////////////////////////////////////////////////
//! class CDibData

#include "DocCommands.h"

class CDibData
{
	LPBYTE m_pDib;				//!< Pointer to DIB
	LPBYTE m_pBits;				//!< Pointer to image bits in DIB
	LPBYTE* m_pLine;			//!< DIB line pointer array
	LPRGBQUAD m_pColorTable;	//!< Pointer to color table in DIB
	WORD  m_wBitsPerPixel;		//!< Number of bits-per-pixel
	DWORD m_dwColorTableSize;	//!< Number of color table entries
	DWORD m_dwBitfields[3];		//!< DIB bitfields (16 & 24 bit images)
	WORD  m_wRShift[3];			//!< Right shift values (16 & 24 bit images)
	WORD  m_wLShift[3];			//!< Left  shift values (16 & 24 bit images)
	double  m_fTolerance;

//j	bool  m_fAverageColors;
//j	int	  m_nNumPixels;
//j	DWORD m_dwAccumRed;
//j	DWORD m_dwAccumGreen;
//j	DWORD m_dwAccumBlue; 

	CDibData* m_pBufferDib;
	SelectedTool m_SelectedTool;
protected:
	void SetDefaultBitfields(WORD wBitsColor);

public:
	CDibData();
	~CDibData();

	// Attach/Detach
	LPBYTE Detach();

	// DIB deletion
	bool DeleteObject();

	// Pixel set/get
	DWORD GetPixelValue(int x, int y) const;
	bool SetPixelValue(int x, int y, DWORD dwValue, SelectedTool selectedTool = TOOL_OFF);

	COLORREF GetPixel(int x, int y) const;
	bool SetPixel(int x, int y, COLORREF clPixel, SelectedTool selectedTool = TOOL_OFF);

	bool CopyPixelValue(int xDest, int yDest,
		CDibData& dibSrc, int xSrc, int ySrc);
	bool CopyPixel(int xDest, int yDest,
		CDibData& dibSrc, int xSrc, int ySrc);

	void SetTolerance(int nValue);

	int Convert(int nPos);
	void SetBufferDib(CDibData* pBufferDib){m_pBufferDib = pBufferDib;}
	CDibData* GetBufferDib(){ return m_pBufferDib;}

	// DIB creation/initialization
	bool InitDIB(BITMAPINFOHEADER* pbmpSrc, bool bCompress = FALSE);

	// Create converted copy of DIB
	CDibData* GetConvertedDIB(WORD wBitsPerPixel, bool bOptimize = FALSE);

	bool IsColorToBeFilled(DWORD dwColorToFill, DWORD dwPixelColor);
	void SetSelectedTool(SelectedTool selectedTool) { m_SelectedTool = selectedTool; };
	/////////////////////////////////////////////////////////////////////////
	// Direct DIB access
	LPBYTE GetDibPtr() const;			//!< Pointer to DIB
	DWORD GetDibSize() const;			//!< Total size of DIB
	LPBITMAPINFOHEADER GetHeaderPtr() const;//!< Pointer to DIB header
	DWORD GetHeaderSize() const;		//!< Size of header
	LONG  GetWidth() const;				//!< Width of image
	LONG  GetHeight() const;			//!< Height of image (may be negative)
	WORD  GetPlanes() const;			//!< Number of planes (=1)
	WORD  GetBitCount() const;			//!< Bits per plane
	DWORD GetCompression() const;		//!< BI_RGB, BI_RLE8, BI_RLE4, BI_BITFIELDS
	DWORD GetSizeImage() const;			//!< Image size (may be 0)
	LONG  GetXPelsPerMeter() const;
	LONG  GetYPelsPerMeter() const;
	DWORD GetClrUsed() const;			//!< Colors used, 0 to 256
	DWORD GetClrImportant() const;		//!< Important colors, 0 to 256

	WORD  GetBitsPerPixel() const;		//!< 1, 2, 4, 8, 16, 24, 32
	DWORD GetByteWidth() const;			//!< Bytes per scan line
	DWORD GetMaxImageSize() const;		//!< Maximum possible image size

	WORD  GetSizeOfBitFields() const;	//!< Bit field size or 0
	LPDWORD GetBitFeildsPtr();			//!< Pointer to bit fields or NULL

	WORD  GetMaxColors() const;			//!< Maximum colors, 0 to 256
	DWORD GetUsedColors() const;		//!< Used or maximum colors, 0 to 256

	DWORD GetSizeOfColorTable() const;	//!< Size of color table in bytes
	DWORD GetColorTableOffset() const;	//!< Offset to color table or 0
	LPRGBQUAD GetColorTablePtr() const;	//!< Pointer to color table or NULL

	DWORD GetImageOffset() const;		//!< Offset to image data
	LPBYTE GetImagePtr() const;			//!< Pointer to image data

	/////////////////////////////////////////////////////////////////////////////
	// Special Functions
	/////////////////////////////////////////////////////////////////////////////

	// Color conversion
	DWORD RGBtoValue(COLORREF ColorRef) const;
	COLORREF ValueToRGB(DWORD dwValue) const;

	// Line drawing
	bool DrawLineH(int x1, int x2, int y, DWORD dwFromValue, DWORD dwToValue, SelectedTool selectedTool, RECT* pFloodRect);

	// Line scanning functions
	int ScanRight(int x, int y, int xmax, DWORD dwValue);
	int SearchRight(int x, int y, int xmax, DWORD dwValue);
	int ScanLeft(int x, int y, int xmin, DWORD dwValue);
	int SearchLeft(int x, int y, int xmin, DWORD dwValue);

	bool FillRectBand(DWORD dwValue, RECT* pFloodRect);
	void DrawGradRect(RECT* Rect, COLORREF cLeft, COLORREF cRight, bool bVertical);
	void DrawRedEyeCircle(LPRECT pRect, CAGClientDC* pDC /*, COLORREF cLeft, COLORREF cRight*/);
	void DrawRadialGradient(CRect Rect, COLORREF clrFrom, COLORREF clrTo, bool bCenter);
	void GetPixelColor(int x, int y, COLORREF &Color);

	//RedEye helpers
	float Factor(float fAvgR, float fAvgG, float fAvgB);
	BYTE AdjustBrightness(BYTE bColor, float fFactor);
	BYTE AdjustContrast(BYTE bColor, float fFactor, BYTE bMeanL);
	void RedEyeRemove(COLORREF& RedEyePixel);
//j	void SetColorAverageState(bool fAverage);
//j	void SetColorForAverage(DWORD dwRed, DWORD dwGreen, DWORD dwBlue);
//j	void GetColorAverage(RGBTRIPLE* pRGB);
};

/////////////////////////////////////////////////////////////////////////////
// class CDibData inline functions

//! Copy pixel value from source bitmap to this bitmap.
inline bool CDibData::
CopyPixelValue(int xDest, int yDest,
	CDibData& dibSrc, int xSrc, int ySrc)
{ return SetPixelValue(xDest, yDest, dibSrc.GetPixelValue(xSrc, ySrc)); }

//! Copy pixel color from source bitmap to this bitmap.
inline bool CDibData::
CopyPixel(int xDest, int yDest,
	CDibData& dibSrc, int xSrc, int ySrc)
{ return SetPixel(xDest, yDest, dibSrc.GetPixel(xSrc, ySrc)); }

inline LPBYTE CDibData::GetDibPtr() const
{ return m_pDib; }
inline LPBITMAPINFOHEADER CDibData::GetHeaderPtr() const
{ return((LPBITMAPINFOHEADER)m_pDib); }
inline DWORD CDibData::GetHeaderSize() const
{ return(GetHeaderPtr()->biSize); }
inline LONG  CDibData::GetWidth() const
{ return(GetHeaderPtr()->biWidth); }
inline LONG  CDibData::GetHeight() const
{ return(GetHeaderPtr()->biHeight); }
inline WORD  CDibData::GetPlanes() const
{ return(GetHeaderPtr()->biPlanes); }
inline WORD  CDibData::GetBitCount() const
{ return(GetHeaderPtr()->biBitCount); }
inline DWORD CDibData::GetCompression() const
{ return(GetHeaderPtr()->biCompression); }
inline DWORD CDibData::GetSizeImage() const
{ return(GetHeaderPtr()->biSizeImage); }
inline LONG  CDibData::GetXPelsPerMeter() const
{ return(GetHeaderPtr()->biXPelsPerMeter); }
inline LONG  CDibData::GetYPelsPerMeter() const
{ return(GetHeaderPtr()->biYPelsPerMeter); }
inline DWORD CDibData::GetClrUsed() const
{ return(GetHeaderPtr()->biClrUsed); }
inline DWORD CDibData::GetClrImportant() const
{ return(GetHeaderPtr()->biClrImportant); }

inline WORD  CDibData::GetBitsPerPixel() const
{ return WORD(GetPlanes() * GetBitCount()); }

inline DWORD CDibData::GetByteWidth() const
{ return(((GetWidth() * GetBitsPerPixel() + 31) & ~31) >> 3); }

inline DWORD CDibData::GetMaxImageSize() const
{ return(GetByteWidth() * abs(GetHeight())); }

inline WORD  CDibData::GetSizeOfBitFields() const
{ return WORD((GetCompression() == BI_BITFIELDS) ? sizeof(m_dwBitfields) : 0); }

inline LPDWORD CDibData::GetBitFeildsPtr()
{ return(LPDWORD)(GetSizeOfBitFields() ? (m_pDib + GetHeaderSize()) : NULL); }

inline WORD CDibData::GetMaxColors() const
{ return WORD((GetBitsPerPixel() <= 8) ? (1 << GetBitsPerPixel()) : 0); }

inline DWORD CDibData::GetUsedColors() const
{ return(GetClrUsed() ? GetClrUsed() : GetMaxColors()); }

inline DWORD CDibData::GetSizeOfColorTable() const
{ return(GetUsedColors() * sizeof(RGBQUAD)); }

inline DWORD CDibData::GetColorTableOffset() const
{ return(GetHeaderSize() + GetSizeOfBitFields()); }

inline LPRGBQUAD CDibData::GetColorTablePtr() const
{ return(LPRGBQUAD)(GetUsedColors() ? (m_pDib + GetColorTableOffset()) : NULL); }

inline DWORD CDibData::GetImageOffset() const
{ return(GetColorTableOffset() + GetSizeOfColorTable()); }

inline LPBYTE CDibData::GetImagePtr() const
{ return(m_pDib + GetImageOffset()); }

inline DWORD CDibData::GetDibSize() const
{
	if( GetSizeImage() )
		return GetImageOffset() + GetSizeImage();
	return GetImageOffset() + GetMaxImageSize();
}

// Allocation helpers
#define DibDataAlloc(dwLen) (BYTE*)malloc(dwLen)
#define DibDataFree(pDib)   free(pDib)
