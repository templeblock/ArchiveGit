#pragma once

#include "AGDoc.h"
#include "AGMatrix.h"
#include "Shape.h"

#ifdef _DEBUG
	#include "pdflib.h"
#endif _DEBUG


typedef DWORD Fixed;
#define IntToFixed(i) (Fixed)(((DWORD)(i)) << 16)
#define FixedToInt(f) (WORD)(((DWORD)(f)) >> 16)

// These values need to be in sync with the browser body color
#define BROWSER_BACKGOUND_COLOR		RGB(255,255,255) //j::GetSysColor(COLOR_WINDOW)

class CAGDCInfo
{
public:
	CAGDCInfo();
	void Init(HDC hDC);

public:
	bool m_bRasDisplay;
	bool m_bPalette;
	int m_iResolutionX;
	int m_iResolutionY;
	SIZE m_Size;
	SIZE m_Offset;
};


class CAGDC  
{
public:
	// Each of the derived classes must provide their own constructor and destructor
	// CAGPrintDC, CAGClientDC, CAGIC, CAGDIBSectionDC, CAGPdfDC

	HDC GetHDC() const
			{ return m_hDC; }
	HDC GetRawHDC() const
			{ return m_hRawDC; }
	HWND GetWnd() const
			{ return m_hWnd; }
	HPALETTE GetPalette() const
			{ return m_hPalette; }
	const CAGDCInfo& GetDeviceInfo() const
			{ return m_Info; }
	bool GetDirty()
			{ return m_bDirty; }
	void SetDirty(bool bDirty)
			{ m_bDirty = bDirty; }
	void Polygon(const POINT* pPts, int nPoints);
	void Rectangle(const RECT& Rect);
	const CAGMatrix& GetSymbolToDeviceMatrix();
	const CAGMatrix& GetViewToDeviceMatrix();
	const CAGMatrix& GetSymbolMatrix() { return m_MatrixSymbol; }
	const CAGMatrix& GetDeviceMatrix() { return m_MatrixDevice; }
	const CAGMatrix& GetViewMatrix() { return m_MatrixView; }
	void SetDeviceMatrix(const CAGMatrix& Matrix)
			{ m_MatrixDevice = Matrix; m_bUpdateViewToDevice = true; m_bUpdateSymbolToDeviceMatrix = true; }
	void SetViewingMatrix(const CAGMatrix& Matrix)
			{ m_MatrixView = Matrix; m_bUpdateViewToDevice = true; m_bUpdateSymbolToDeviceMatrix = true; }
	const CAGMatrix& SetSymbolMatrix(const CAGMatrix& Matrix);
	void SetSymbolMatrix();
	void SetFont(const LOGFONT& Font, bool bTransform);
	HRGN SaveClipRegion();
	void RestoreClipRegion(HRGN hRegion);
	void SetClipRect(const RECT& ClipRect, bool bIncludeRawDC);
	void SetClipToView(const SIZE& ViewSize, bool bIncludeRawDC, RECT* pViewRect);

	bool GetABCWidths(UINT uFirstChar, UINT uLastChar, int* piMinA, int* piMaxB, int* piMaxC, int* piMaxW);
	bool GetTextExtent(LPCTSTR pString, int nCount, SIZE* pSize);
	bool GetTextMetrics(TEXTMETRIC* ptm) const;

	const RECT& GetViewRect()
		{ return m_ViewRect; }
	bool GetCoverAllowed()
		{ return m_bCoverAllowed; }
	void SetCoverAllowed(bool bCoverAllowed)
		{ m_bCoverAllowed = bCoverAllowed; }

	void FillLinearGradient(POINT* pPoints, COLORREF clrFrom, COLORREF clrTo, BOOL bHorizontal);
	void FillRadialGradient(POINT* pPoints, COLORREF clrFrom, COLORREF clrTo, bool bCenter);
	void FillRectangle(POINT* pt, COLORREF FillColor);

	// Drawing and clipping methods
	virtual void StretchBlt(const BITMAPINFOHEADER* pDIB, RECT CropRect, RECT DestRect, COLORREF TransparentColor);
	virtual bool GetClipRect(RECT* pClipRect);
	virtual void DrawPath(COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual void DrawText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, COLORREF LineColor, int LineWidth, COLORREF FillColor, int x, int y, RECT* pRectAlign = NULL);
	virtual bool SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, RECT* pRectAlign = NULL);
	virtual void DrawLine(POINT* pt, COLORREF LineColor, int LineWidth);
	virtual void DrawRectangle(POINT* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual void DrawPolygon(POINT* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor, int nPoints);
	virtual bool SetClipRectangle(POINT* pt);
	virtual void DrawEllipse(POINT* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual bool SetClipEllipse(POINT* pt);
	virtual void DrawShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual bool SetClipShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints);

protected:
	void Init(bool bInitInfo = true);
	void Free();
	HPALETTE CreatePalette();
	void Dither(const BYTE* pSrcBits, BYTE* pDstBits, int nSrcBitCount, int nDstWidth, int y, Fixed fixSrcStepX, const RGBQUAD* pColors) const;
	void StretchBlt2(const BITMAPINFOHEADER* pDIB, const BYTE* pBits, RECT DibRect, RECT DestRect, DWORD dwBltMode);
	Fixed FixedDivide(Fixed Dividend, Fixed Divisor) const;
	void ComputePosition(int& x, int& y, RECT* pRectAlign, const SIZE& Size, const CAGMatrix& Matrix);

protected:
	bool m_bUpdateViewToDevice;
	bool m_bUpdateSymbolToDeviceMatrix;
	bool m_bDirty;
	bool m_bCoverAllowed;
	HDC m_hDC;
    HDC m_hRawDC;
	HWND m_hWnd;
	HFONT m_hFont;
	HFONT m_hOldFont;
	LOGFONT m_Font;
	HPALETTE m_hPalette;
	HPALETTE m_hOldPalette;
	BYTE* m_pBltBuf;
	RECT m_ViewRect;
	CAGDCInfo m_Info;
	CAGMatrix m_MatrixSaved;
	CAGMatrix m_MatrixSymbol;
	CAGMatrix m_MatrixView;
	CAGMatrix m_MatrixDevice;
	CAGMatrix m_MatrixViewToDevice;		// m_MatrixView * m_MatrixDevice
	CAGMatrix m_MatrixSymbolToDevice;	// m_MatrixSymbol *  m_MatrixView * m_MatrixDevice
};


class CAGPrintDC : public CAGDC
{
public:
	CAGPrintDC(const char* pszDriver, const char* pszDevice, const char* pszPort, const DEVMODE* pDevMode);
	CAGPrintDC();
	~CAGPrintDC();

	virtual bool StartDoc(const char* pszDocName) const;
	virtual bool EndDoc() const;
	virtual bool AbortDoc() const;
	virtual bool StartPage();
	virtual bool EndPage() const;
};

#ifdef _DEBUG
class CAGPdfDC : public CAGPrintDC
{
public:
	CAGPdfDC(LPCSTR strDocFileName, LPCSTR strFileName, LPCSTR strUser, double fPageWidth, double fPageHeight);
	~CAGPdfDC();

	bool StartDoc(const char* pszDocName) const;
	bool EndDoc() const;
	bool AbortDoc() const;
	bool StartPage();
	bool EndPage() const;

	void PDF_polygon(PDF* pPDF, POINT* pPoints, int nPoints);
	void PDF_polybezier(PDF* pPDF, POINT* pPoints, int nPoints);
	void StretchBlt(const BITMAPINFOHEADER* pDIB, RECT CropRect, RECT DestRect, COLORREF TransparentColor);
	bool GetClipRect(RECT* pClipRect);
	void DrawPath(COLORREF LineColor, int LineWidth, COLORREF FillColor);
	void DrawText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, COLORREF LineColor, int LineWidth, COLORREF FillColor, int x, int y, RECT* pRectAlign = NULL);
	bool SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, RECT* pRectAlign = NULL);
	void DrawLine(POINT* pt, COLORREF LineColor, int LineWidth);
	void DrawRectangle(POINT* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipRectangle(POINT* pt);
	void DrawEllipse(POINT* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipEllipse(POINT* pt);
	void DrawShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints);

protected:
	PDF* m_pPDF;
};
#endif _DEBUG

class CAGClientDC : public CAGDC
{
public:
	CAGClientDC(HWND hWnd);
	~CAGClientDC();

	void Clear();
	void Blt(HDC hScreenDC, const RECT& rect);

protected:
    HBITMAP m_hBitmap;		// Offscreen bitmap
    HBITMAP m_hOldBitmap;	// Original DC bitmap 
    RECT m_rect;			// Rectangle of drawing area.
};


class CAGIC : public CAGDC
{
public:
	CAGIC(const char* pszDriver);
	~CAGIC();
};


class CAGDIBSectionDC : public CAGDC
{
public:
	CAGDIBSectionDC(const BITMAPINFO& BitmapInfo, UINT iUsage, HDC hCompatibleDC = NULL);
	~CAGDIBSectionDC();

	void Clear();
	BITMAPINFOHEADER* ExtractDib();
	HANDLE ExtractDibGlobal();
	BITMAPINFOHEADER* GetDibHeader()
		{ return &m_BitmapInfo.bmiHeader; }
	BYTE* GetBits()
		{ return m_pBits; }
	HBITMAP GetHBitmap()
		{ return m_hBitmap;}

protected:
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	BITMAPINFO m_BitmapInfo;
	BYTE* m_pBits;
};
