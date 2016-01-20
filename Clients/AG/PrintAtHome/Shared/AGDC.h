#pragma once

#include "AGMatrix.h"

#ifdef _PDF
	#include "pdflib.h"
#endif _PDF


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
	CSize m_Size;
	CSize m_Offset;
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
	void Polygon(const CPoint* pPts, int nPoints);
	void Rectangle(const CRect& Rect);
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
	void SetClipRect(const CRect& ClipRect, bool bIncludeRawDC = false);
	void SetClipToView(const CSize& ViewSize, CRect* pViewRect, bool bIncludeRawDC = false);

	bool GetABCWidths(UINT uFirstChar, UINT uLastChar, int* piMinA, int* piMaxB, int* piMaxC, int* piMaxW);
	bool GetTextExtent(LPCTSTR pString, int nCount, CSize* pSize);
	bool GetTextMetrics(TEXTMETRIC* ptm) const;

	const CRect& GetViewRect()
		{ return m_ViewRect; }
	bool GetCoverAllowed()
		{ return m_bCoverAllowed; }
	void SetCoverAllowed(bool bCoverAllowed)
		{ m_bCoverAllowed = bCoverAllowed; }

	void FillLinearGradient(CPoint* pPoints, COLORREF clrFrom, COLORREF clrTo, BOOL bHorizontal);
	void FillRadialGradient(CPoint* pPoints, COLORREF clrFrom, COLORREF clrTo, bool bCenter);
	void FillRectangle(CPoint* pt, COLORREF FillColor);

	// Drawing and clipping methods
	virtual void StretchBlt(const BITMAPINFOHEADER* pDIB, CRect CropRect, CRect DestRect, COLORREF TransparentColor);
	virtual bool GetClipRect(CRect* pClipRect);
	virtual void DrawPath(COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual void DrawText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, COLORREF LineColor, int LineWidth, COLORREF FillColor, int x, int y, CRect* pRectAlign = NULL);
	virtual bool SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, CRect* pRectAlign = NULL);
	virtual void DrawLine(CPoint* pt, COLORREF LineColor, int LineWidth);
	virtual void DrawRectangle(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual void DrawPolygon(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor, int nPoints);
	virtual bool SetClipRectangle(CPoint* pt);
	virtual void DrawEllipse(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual bool SetClipEllipse(CPoint* pt);
	virtual void DrawShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual bool SetClipShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints);

protected:
	void Init(bool bInitInfo = true);
	void Free();
	HPALETTE CreatePalette();
	void Dither(const BYTE* pSrcBits, BYTE* pDstBits, int nSrcBitCount, int nDstWidth, int y, Fixed fixSrcStepX, const RGBQUAD* pColors) const;
	void StretchBlt2(const BITMAPINFOHEADER* pDIB, const BYTE* pBits, CRect DibRect, CRect DestRect, DWORD dwBltMode);
	Fixed FixedDivide(Fixed Dividend, Fixed Divisor) const;
	void ComputePosition(int& x, int& y, CRect* pRectAlign, const CSize& Size, const CAGMatrix& Matrix);

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
	CRect m_ViewRect;
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

#ifdef _PDF
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

	void PDF_polygon(PDF* pPDF, CPoint* pPoints, int nPoints);
	void PDF_polybezier(PDF* pPDF, CPoint* pPoints, int nPoints);
	void StretchBlt(const BITMAPINFOHEADER* pDIB, CRect CropRect, CRect DestRect, COLORREF TransparentColor);
	bool GetClipRect(CRect* pClipRect);
	void DrawPath(COLORREF LineColor, int LineWidth, COLORREF FillColor);
	void DrawText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, COLORREF LineColor, int LineWidth, COLORREF FillColor, int x, int y, CRect* pRectAlign = NULL);
	bool SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, CRect* pRectAlign = NULL);
	void DrawLine(CPoint* pt, COLORREF LineColor, int LineWidth);
	void DrawRectangle(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipRectangle(CPoint* pt);
	void DrawEllipse(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipEllipse(CPoint* pt);
	void DrawShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints);

protected:
	PDF* m_pPDF;
};
#endif _PDF

class CAGClientDC : public CAGDC
{
public:
	CAGClientDC(HWND hWnd);
	~CAGClientDC();

	void Clear();
	void Blt(HDC hScreenDC, const CRect& rect);

protected:
    HBITMAP m_hBitmap;		// Offscreen bitmap
    HBITMAP m_hOldBitmap;	// Original DC bitmap 
    CRect m_rect;			// Rectangle of drawing area.
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
