#pragma once

#include "Matrix.h"

#ifdef _PDF
	#include "pdflib.h"
#endif _PDF

#define SYMBOL_COMMANDPOINT(x)	x>100000
#define SYMBOL_POLYGON 100001
#define SYMBOL_BEZIER  100002
#define SYMBOL_LINE    100003
#define SYMBOL_MOVE    100004

typedef DWORD Fixed;
#define IntToFixed(i) (Fixed)(((DWORD)(i)) << 16)
#define FixedToInt(f) (WORD)(((DWORD)(f)) >> 16)

// These values need to be in sync with the browser body color
#define BROWSER_BACKGROUND_COLOR RGB(255,255,255) //j::GetSysColor(COLOR_WINDOW)

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
	const CMatrix& GetSymbolToDeviceMatrix();
	const CMatrix& GetViewToDeviceMatrix();
	const CMatrix& GetSymbolMatrix() { return m_MatrixSymbol; }
	const CMatrix& GetDeviceMatrix() { return m_MatrixDevice; }
	const CMatrix& GetViewMatrix() { return m_MatrixView; }
	void SetDeviceMatrix(const CMatrix& Matrix)
			{ m_MatrixDevice = Matrix; m_bUpdateViewToDevice = true; m_bUpdateSymbolToDeviceMatrix = true; }
	void SetViewingMatrix(const CMatrix& Matrix)
			{ m_MatrixView = Matrix; m_bUpdateViewToDevice = true; m_bUpdateSymbolToDeviceMatrix = true; }
	const CMatrix& SetSymbolMatrix(const CMatrix& Matrix);
	void SetSymbolMatrix();
	void SetFont(const LOGFONT& Font, bool bTransform);
	HRGN SaveClipRegion();
	void RestoreClipRegion(HRGN hRegion);
	bool SetClipRect(const CRect& ClipRect, bool bCombine, bool bIncludeRawDC = false);
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
	virtual bool SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, CRect* pRectAlign, bool bCombine = true);
	virtual void DrawLine(CPoint* pt, COLORREF LineColor, int LineWidth);
	virtual void DrawRectangle(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual void DrawPolygon(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor, int nPoints);
	virtual bool SetClipRectangle(CRect* pClipRect, bool bReduce = false, bool bCombine = true);
	virtual void DrawEllipse(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual bool SetClipEllipse(CPoint* pt, bool bCombine = true);
	virtual void DrawShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	virtual bool SetClipShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints, bool bCombine = true);

protected:
	void Init(bool bInitInfo = true);
	void Free();
	HPALETTE CreatePalette();
	void Dither(const BYTE* pSrcBits, BYTE* pDstBits, int nSrcBitCount, int nDstWidth, int y, Fixed fixSrcStepX, const RGBQUAD* pColors) const;
	bool StretchBlt1(const BITMAPINFOHEADER* pDIB, const BYTE* pBits, CRect DibRect, CRect DestRect, COLORREF TransparentColor);
	void StretchBlt2(const BITMAPINFOHEADER* pDIB, const BYTE* pBits, CRect DibRect, CRect DestRect, DWORD dwBltMode);
	Fixed FixedDivide(Fixed Dividend, Fixed Divisor) const;
	void ComputePosition(int& x, int& y, CRect* pRectAlign, const CSize& Size, const CMatrix& Matrix);

protected:
	bool m_bUpdateViewToDevice;
	bool m_bUpdateSymbolToDeviceMatrix;
	bool m_bDirty;
	bool m_bCoverAllowed;
	HDC m_hDC;
    HDC m_hRawDC;
	HWND m_hWnd;
	COLORREF m_clrBackground;
	HFONT m_hFont;
	HFONT m_hOldFont;
	LOGFONT m_Font;
	HPALETTE m_hPalette;
	HPALETTE m_hOldPalette;
	BYTE* m_pBltBuf;
	CRect m_ViewRect;
	CAGDCInfo m_Info;
	CMatrix m_MatrixSaved;
	CMatrix m_MatrixSymbol;
	CMatrix m_MatrixView;
	CMatrix m_MatrixDevice;
	CMatrix m_MatrixViewToDevice;		// m_MatrixView * m_MatrixDevice
	CMatrix m_MatrixSymbolToDevice;	// m_MatrixSymbol *  m_MatrixView * m_MatrixDevice
};


class CAGPrintDC : public CAGDC
{
public:
	CAGPrintDC(const char* pszDriver, const char* pszDevice, const char* pszPort, const DEVMODE* pDevMode);
	CAGPrintDC(HDC hDC = NULL, bool bPreview = true);
	~CAGPrintDC();

	virtual bool StartDoc(const char* pszDocName) const;
	virtual bool EndDoc() const;
	virtual bool AbortDoc() const;
	virtual bool StartPage();
	virtual bool EndPage() const;
	
private:
	bool m_bPreview;
	bool m_bDeleteDC;
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
	bool SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, CRect* pRectAlign, bool bCombine = true);
	void DrawLine(CPoint* pt, COLORREF LineColor, int LineWidth);
	void DrawRectangle(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipRectangle(CRect* pClipRect, bool bReduce = false, bool bCombine = true);
	void DrawEllipse(CPoint* pt, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipEllipse(CPoint* pt, bool bCombine = true);
	void DrawShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor);
	bool SetClipShape(CPoint* pCommands, int nCommands, CPoint* pPoints, int nPoints, bool bCombine = true);

protected:
	PDF* m_pPDF;
};
#endif _PDF

class CAGClientDC : public CAGDC
{
public:
	CAGClientDC(HWND hWnd, COLORREF clrBackground = CLR_NONE);
	~CAGClientDC();

	void Clear(COLORREF clrBackground = CLR_NONE);
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
	CAGDIBSectionDC(BITMAPINFO& BitmapInfo, UINT iUsage, HDC hCompatibleDC = NULL);
	CAGDIBSectionDC(BITMAPINFOHEADER* pDIB, UINT iUsage, HDC hCompatibleDC = NULL);
	~CAGDIBSectionDC();

	void Construct(BITMAPINFOHEADER* pDIB, UINT iUsage, HDC hCompatibleDC = NULL, BYTE* pBits = NULL);
	void Clear(COLORREF clrBackground = CLR_NONE);
	BITMAPINFOHEADER* ExtractDib();
	HANDLE ExtractDibGlobal();
	BITMAPINFOHEADER* GetDibHeader()
		{ return m_pDIB; }
	BYTE* GetBits()
		{ return m_pBits; }
	HBITMAP GetHBitmap()
		{ return m_hBitmap;}

	void Attach(HBITMAP hBitmap)
	{
		if(m_hBitmap != NULL && m_hBitmap != hBitmap)
			::DeleteObject(m_hBitmap);
		m_hBitmap = hBitmap;
	}
	HBITMAP Detach()
	{
		HBITMAP hBitmap = m_hBitmap;
		m_hBitmap = NULL;
		return hBitmap;
	}

protected:
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	BITMAPINFOHEADER* m_pDIB;
	BYTE* m_pBits;
};
