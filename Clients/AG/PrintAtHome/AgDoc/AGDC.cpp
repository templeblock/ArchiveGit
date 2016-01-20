#include "stdafx.h"
#include "AGDC.h"
#include "AGDib.h"
#include "Dib.h"
#include <math.h>
//j #include "Version.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CAGDCInfo::CAGDCInfo()
{
	m_bRasDisplay = false;
	m_bPalette = false;
	m_iResolutionX = 0;
	m_iResolutionY = 0;
	m_Size.cx = m_Size.cy = 0;
	m_Offset.cx = m_Offset.cy = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGDCInfo::Init(HDC hDC)
{
	if (!hDC)
		return;

	m_bRasDisplay = (::GetDeviceCaps(hDC, TECHNOLOGY) == DT_RASDISPLAY);
	m_bPalette = ((::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE) != 0);

	m_iResolutionX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	m_iResolutionY = ::GetDeviceCaps(hDC, LOGPIXELSY);

	// Initialize the size for printer DC's
	m_Size.cx = ::GetDeviceCaps(hDC, PHYSICALWIDTH);
	m_Size.cy = ::GetDeviceCaps(hDC, PHYSICALHEIGHT);
	if (::Escape(hDC, GETPHYSPAGESIZE, NULL, NULL, &m_Size) <= 0)
		int i = 0;

	// If size is still 0...
	if (!m_Size.cx && !m_Size.cy)
	{
		m_Size.cx = ::GetDeviceCaps(hDC, HORZRES);
		m_Size.cy = ::GetDeviceCaps(hDC, VERTRES);
	}

	// Initialize the offset for printer DC's
	m_Offset.cx = ::GetDeviceCaps(hDC, PHYSICALOFFSETX);
	m_Offset.cy = ::GetDeviceCaps(hDC, PHYSICALOFFSETY);
	if (::Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &m_Offset) <= 0)
		int i = 0;
}


//////////////////////////////////////////////////////////////////////
// Each of the derived classes must provide their own constructor and destructor
// CAGPrintDC, CAGClientDC, CAGIC, CAGDIBSectionDC, CAGPdfDC

//////////////////////////////////////////////////////////////////////
CAGPrintDC::CAGPrintDC(const char* pszDriver, const char* pszDevice, const char* pszPort, const DEVMODE* pDevMode)
{
	m_hDC = ::CreateDC(pszDriver, pszDevice, pszPort, pDevMode);
	m_hRawDC = NULL;
	m_hWnd = NULL;
	Init();
}

//////////////////////////////////////////////////////////////////////
CAGPrintDC::CAGPrintDC()
{
	m_hDC = NULL;
	m_hRawDC = NULL;
	m_hWnd = NULL;
	Init();
}

//////////////////////////////////////////////////////////////////////
CAGPrintDC::CAGPrintDC(HDC hDC)
{
	m_hDC = hDC;
	m_hRawDC = NULL;
	m_hWnd = NULL;
	Init();
}

//////////////////////////////////////////////////////////////////////
CAGPrintDC::~CAGPrintDC()
{
	Free();

	if (m_hDC)
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	m_hRawDC = NULL;
}

//////////////////////////////////////////////////////////////////////
bool CAGPrintDC::StartDoc(const char* pszDocName) const
{
	DOCINFO di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	di.lpszDocName = pszDocName;
	return (::StartDoc(m_hDC, &di) > 0);
}

//////////////////////////////////////////////////////////////////////
bool CAGPrintDC::EndDoc() const
{
	return (::EndDoc(m_hDC) >= 0);
}

//////////////////////////////////////////////////////////////////////
bool CAGPrintDC::AbortDoc() const
{
	return (::AbortDoc(m_hDC) >= 0);
}

//////////////////////////////////////////////////////////////////////
bool CAGPrintDC::StartPage()
{
	if (m_hOldFont)
	{
		::SelectObject(m_hDC, m_hOldFont);
		m_hOldFont = NULL;
	}

	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	bool bReturn = (::StartPage(m_hDC) > 0);

	::SetMapMode(m_hDC, MM_TEXT);
	::SetTextAlign(m_hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
	::SetBkMode(m_hDC, TRANSPARENT);

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGPrintDC::EndPage() const
{
	return (::EndPage(m_hDC) >= 0);
}

//////////////////////////////////////////////////////////////////////
CAGPrintPreviewDC::CAGPrintPreviewDC(HDC hDC)
{
	m_hDC = hDC;
	m_hRawDC = NULL;
	m_hWnd = NULL;
	Init();
}

//////////////////////////////////////////////////////////////////////
CAGPrintPreviewDC::~CAGPrintPreviewDC()
{

}

//////////////////////////////////////////////////////////////////////
bool CAGPrintPreviewDC::StartDoc(const char* pszDocName) const
{
	DOCINFO di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	di.lpszDocName = pszDocName;

	return true;
}


#ifdef _DEBUG

#define PDF_UNIT_CONVERSION ((float)72.0 / APP_RESOLUTION)
#define PDF_COLOR_CONVERSION ((float)1.0 / 256.0)

//////////////////////////////////////////////////////////////////////
CAGPdfDC::CAGPdfDC(LPCSTR strDocFileName, LPCSTR strFileName, LPCSTR strUser, double fPageWidth, double fPageHeight)
{
	m_hDC = ::CreateCompatibleDC(NULL); //j temporary NULL;
	m_hRawDC = NULL;
	m_hWnd = NULL;

	// The device info needs to be setup special for this kind of a DC
	m_Info.m_bPalette	  = false;
	m_Info.m_iResolutionX = APP_RESOLUTION;
	m_Info.m_iResolutionY = APP_RESOLUTION;
	m_Info.m_Size.cx	  = dtoi(fPageWidth * APP_RESOLUTION);
	m_Info.m_Size.cy	  = dtoi(fPageHeight * APP_RESOLUTION);
	m_Info.m_Offset.cx	  = 0;
	m_Info.m_Offset.cy	  = 0;
	Init(false/*bInitInfo*/);

	PDF_boot();
	m_pPDF = PDF_new();
	if (!m_pPDF)
	{
		PDF_shutdown();
		m_pPDF = NULL;
		return;
	}

	if (PDF_open_file(m_pPDF, strFileName) == -1)
	{
		PDF_delete(m_pPDF);
		PDF_shutdown();
		m_pPDF = NULL;
		return;
	}

	PDF_set_info(m_pPDF, "Title", "Create & Print Project PDF");
	PDF_set_info(m_pPDF, "Subject", strDocFileName);
	PDF_set_info(m_pPDF, "Keywords",	"");
	PDF_set_info(m_pPDF, "Author", strUser);
//j	PDF_set_info(m_pPDF, "Creator", CString("Create && Print version ") + CString(VER_PRODUCT_VERSION_STR));
//j	PDF_set_info(m_pPDF, "Producer", VER_COPYRIGHT);
}

//////////////////////////////////////////////////////////////////////
CAGPdfDC::~CAGPdfDC()
{
	Free();

	m_hRawDC = NULL;

	if (m_hDC)
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if (m_pPDF)
	{
		PDF_close(m_pPDF);
		PDF_delete(m_pPDF);
		PDF_shutdown();
		m_pPDF = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::StartDoc(const char* pszDocName) const
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::EndDoc() const
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::AbortDoc() const
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::StartPage()
{
	float fUnitConversion = PDF_UNIT_CONVERSION;
	float fWidth  = fUnitConversion * m_Info.m_Size.cx;
	float fHeight = fUnitConversion * m_Info.m_Size.cy;
	PDF_begin_page(m_pPDF, fWidth, fHeight);
	PDF_add_bookmark(m_pPDF, "Page", 0, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::EndPage() const
{
	PDF_end_page(m_pPDF);
	return true;
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::DrawPath(COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::GetClipRect(RECT* pClipRect)
{
	return false;
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::DrawText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, COLORREF LineColor, int LineWidth, COLORREF FillColor, int x, int y, RECT* pRectAlign)
{
	CAGMatrix Matrix = GetSymbolToDeviceMatrix();
	POINT pt = {x, y};
	Matrix.Transform(pt);

	LPCTSTR strFontName = m_Font.lfFaceName;
	int font = PDF_findfont(m_pPDF, strFontName, "host"/*encoding*/, true/*embed*/);
	PDF_setfont(m_pPDF, font, (float)POINTSIZE(abs(m_Font.lfHeight)));

	PDF_set_parameter(m_pPDF, "underline", (m_Font.lfUnderline ? "true" : "false"));

	float fColorConversion = PDF_COLOR_CONVERSION;
	float r = fColorConversion * GetRValue(FillColor);
	float g = fColorConversion * GetGValue(FillColor);
	float b = fColorConversion * GetBValue(FillColor);
	PDF_setcolor(m_pPDF, "fill", "rgb", r, g, b, 0.0);

	float fUnitConversion = PDF_UNIT_CONVERSION;
	float x1 = fUnitConversion * pt.x;
	float y1 = fUnitConversion * (m_Info.m_Size.cy - pt.y); // Flipped

//	PDF_setmatrix(m_pPDF, float a, float b, float c, float d, float e, float f)
	float fAngle = (float)Matrix.GetAngle();
	if (fAngle)
	{
		PDF_translate(m_pPDF, -x1, -y1);
		PDF_rotate(m_pPDF, fAngle);
		PDF_translate(m_pPDF, x1, y1);
	}

	PDF_show_xy2(m_pPDF, pszString, nCount, x1, y1);

	if (fAngle)
	{
		PDF_translate(m_pPDF, -x1, -y1);
		PDF_rotate(m_pPDF, -fAngle);
		PDF_translate(m_pPDF, x1, y1);
	}
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, RECT* pRectAlign)
{
	return false;
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::PDF_polygon(PDF* pPDF, POINT* pPoints, int nPoints)
{
	float fUnitConversion = PDF_UNIT_CONVERSION;
	for (int i = 0; i < nPoints; i++)
	{
		float x = fUnitConversion * pPoints->x;
		float y = fUnitConversion * (m_Info.m_Size.cy - pPoints->y); // Flipped
		pPoints++;
		if (!i)
			PDF_moveto(pPDF, x, y);
		else
			PDF_lineto(pPDF, x, y);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::PDF_polybezier(PDF* pPDF, POINT* pPoints, int nPoints)
{
	float fUnitConversion = PDF_UNIT_CONVERSION;
	for (int i = 0; i < nPoints; i += 3)
	{
		float x1 = fUnitConversion * pPoints->x;
		float y1 = fUnitConversion * (m_Info.m_Size.cy - pPoints->y); // Flipped
		pPoints++;
		float x2 = fUnitConversion * pPoints->x;
		float y2 = fUnitConversion * (m_Info.m_Size.cy - pPoints->y); // Flipped
		pPoints++;
		float x3 = fUnitConversion * pPoints->x;
		float y3 = fUnitConversion * (m_Info.m_Size.cy - pPoints->y); // Flipped
		pPoints++;
		PDF_curveto(pPDF, x1, y1, x2, y2, x3, y3);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::DrawLine(POINT* pPoints, COLORREF LineColor, int LineWidth)
{
	float fColorConversion = PDF_COLOR_CONVERSION;
	float r = fColorConversion * GetRValue(LineColor);
	float g = fColorConversion * GetGValue(LineColor);
	float b = fColorConversion * GetBValue(LineColor);
	PDF_setcolor(m_pPDF, "stroke", "rgb", r, g, b, 0.0);

	float fUnitConversion = PDF_UNIT_CONVERSION;
	float x = fUnitConversion * pPoints[0].x;
	float y = fUnitConversion * (m_Info.m_Size.cy - pPoints[0].y); // Flipped
	PDF_moveto(m_pPDF, x, y);
	x = fUnitConversion * pPoints[1].x;
	y = fUnitConversion * (m_Info.m_Size.cy - pPoints[1].y); // Flipped
	PDF_lineto(m_pPDF, x, y);

	PDF_stroke(m_pPDF);
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::DrawRectangle(POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	float fColorConversion = PDF_COLOR_CONVERSION;
	float r = fColorConversion * GetRValue(FillColor);
	float g = fColorConversion * GetGValue(FillColor);
	float b = fColorConversion * GetBValue(FillColor);
	PDF_setcolor(m_pPDF, "fill", "rgb", r, g, b, 0.0);

	if (LineColor != CLR_NONE)
	{
		r = fColorConversion * GetRValue(LineColor);
		g = fColorConversion * GetGValue(LineColor);
		b = fColorConversion * GetBValue(LineColor);
		PDF_setcolor(m_pPDF, "stroke", "rgb", r, g, b, 0.0);
	}

	PDF_polygon(m_pPDF, pPoints, 4);
    PDF_closepath(m_pPDF);
	if (LineColor != CLR_NONE)
		PDF_fill_stroke(m_pPDF);
	else
		PDF_fill(m_pPDF);
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::DrawEllipse(POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	float fColorConversion = PDF_COLOR_CONVERSION;
	float r = fColorConversion * GetRValue(FillColor);
	float g = fColorConversion * GetGValue(FillColor);
	float b = fColorConversion * GetBValue(FillColor);
	PDF_setcolor(m_pPDF, "fill", "rgb", r, g, b, 0.0);

	if (LineColor != CLR_NONE)
	{
		r = fColorConversion * GetRValue(LineColor);
		g = fColorConversion * GetGValue(LineColor);
		b = fColorConversion * GetBValue(LineColor);
		PDF_setcolor(m_pPDF, "stroke", "rgb", r, g, b, 0.0);
	}

	float fUnitConversion = PDF_UNIT_CONVERSION;
	float x = fUnitConversion * pPoints[0].x;
	float y = fUnitConversion * (m_Info.m_Size.cy - pPoints[0].y); // Flipped
	PDF_moveto(m_pPDF, x, y);
	PDF_polybezier(m_pPDF, &pPoints[1], 12);

    PDF_closepath(m_pPDF);
	if (LineColor != CLR_NONE)
		PDF_fill_stroke(m_pPDF);
	else
		PDF_fill(m_pPDF);
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::DrawShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	float fColorConversion = PDF_COLOR_CONVERSION;
	float r = fColorConversion * GetRValue(FillColor);
	float g = fColorConversion * GetGValue(FillColor);
	float b = fColorConversion * GetBValue(FillColor);
	PDF_setcolor(m_pPDF, "fill", "rgb", r, g, b, 0.0);

	if (LineColor != CLR_NONE)
	{
		r = fColorConversion * GetRValue(LineColor);
		g = fColorConversion * GetGValue(LineColor);
		b = fColorConversion * GetBValue(LineColor);
		PDF_setcolor(m_pPDF, "stroke", "rgb", r, g, b, 0.0);
	}

	float fUnitConversion = PDF_UNIT_CONVERSION;
	if (!nCommands)
	{
		// Support the old way of doing things
		PDF_polygon(m_pPDF, pPoints, nPoints);
	}
	else
	{
		int nCurrentPoint = 0;
		for (int i = 0; i < nCommands; i++)
		{
			if (pCommands->x == SYMBOL_MOVE)
			{
				float x = fUnitConversion * pPoints[nCurrentPoint].x;
				float y = fUnitConversion * (m_Info.m_Size.cy - pPoints[nCurrentPoint].y); // Flipped
				PDF_moveto(m_pPDF, x, y);
			}
			else
			if (pCommands->x == SYMBOL_POLYGON)
				PDF_polygon(m_pPDF, &pPoints[nCurrentPoint], pCommands->y);
			else 
			if (pCommands->x == SYMBOL_BEZIER)
				PDF_polybezier(m_pPDF, &pPoints[nCurrentPoint], pCommands->y);
			else
			if (pCommands->x == SYMBOL_LINE)
			{
				float x = fUnitConversion * pPoints[nCurrentPoint].x;
				float y = fUnitConversion * (m_Info.m_Size.cy - pPoints[nCurrentPoint].y); // Flipped
				PDF_lineto(m_pPDF, x, y);
			}

			nCurrentPoint += pCommands->y;
			pCommands++;
		}
	}

    PDF_closepath(m_pPDF);
	if (LineColor != CLR_NONE)
		PDF_fill_stroke(m_pPDF);
	else
		PDF_fill(m_pPDF);
}

//////////////////////////////////////////////////////////////////////
void CAGPdfDC::StretchBlt(const BITMAPINFOHEADER* pDIB, RECT CropRect, RECT DestRect, COLORREF TransparentColor)
{
	POINT pPoints[4];
	pPoints[0].x = DestRect.left;
	pPoints[0].y = DestRect.top;
	pPoints[1].x = DestRect.right;
	pPoints[1].y = DestRect.top;
	pPoints[2].x = DestRect.right;
	pPoints[2].y = DestRect.bottom;
	pPoints[3].x = DestRect.left;
	pPoints[3].y = DestRect.bottom;
	DrawRectangle(pPoints, RGB(255,0,0), 0, RGB(0,0,0));
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::SetClipRectangle(POINT* pPoints)
{
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::SetClipEllipse(POINT* pPoints)
{
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CAGPdfDC::SetClipShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints)
{
	return false;
}
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CAGClientDC::CAGClientDC(HWND hWnd)
{
	m_hRawDC = ::GetDC(hWnd);
	m_hDC = ::CreateCompatibleDC(m_hRawDC); // Create the offscreen memory DC
	m_hWnd = hWnd;

	// Create the offscreen bitmap
	::GetClientRect(::GetDesktopWindow(), &m_rect);
	m_hBitmap = ::CreateCompatibleBitmap(m_hRawDC, WIDTH(m_rect), HEIGHT(m_rect));
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_hBitmap);
	Init();
	Clear();
}

//////////////////////////////////////////////////////////////////////
CAGClientDC::~CAGClientDC()
{
	Free();

	if (m_hDC)
	{
		::SelectObject(m_hDC, m_hOldBitmap);
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if (m_hBitmap)
		::DeleteObject(m_hBitmap);

	if (m_hRawDC)
	{
		::ReleaseDC(m_hWnd, m_hRawDC);
		m_hRawDC = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGClientDC::Clear()
{
	if (!m_hDC)
		return;

	// Restore the full clip rectangle
	HRGN hClipRegion = ::CreateRectRgnIndirect(&m_rect);
	::SelectClipRgn(m_hDC, hClipRegion);
	::DeleteObject(hClipRegion);

	// Fill background 
	::FillRect(m_hDC, &m_rect, CAGBrush(BROWSER_BACKGOUND_COLOR));
}

//////////////////////////////////////////////////////////////////////
void CAGClientDC::Blt(HDC hScreenDC, const RECT& rect)
{
	if (!hScreenDC)
		if (!(hScreenDC = m_hRawDC))
			return;

	if (::IsRectEmpty(&rect))
		int i = 0;

	if (m_hPalette)
	{
		m_hOldPalette = ::SelectPalette(hScreenDC, m_hPalette, true);
		::RealizePalette(hScreenDC);
	}

	// Copy the offscreen bitmap onto the screen
	::BitBlt(hScreenDC, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), m_hDC, rect.left, rect.top, SRCCOPY);
}

//////////////////////////////////////////////////////////////////////
CAGIC::CAGIC(const char* pszDriver)
{
	m_hDC = ::CreateIC(pszDriver, NULL/*pszDevice*/, NULL/*pszPort*/, NULL/*pDevMode*/);
	m_hRawDC = NULL;
	m_hWnd = NULL;
	Init();
}

//////////////////////////////////////////////////////////////////////
CAGIC::~CAGIC()
{
	Free();

	if (m_hDC)
	{
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	m_hRawDC = NULL;
}

//////////////////////////////////////////////////////////////////////
CAGDIBSectionDC::CAGDIBSectionDC(const BITMAPINFO& BitmapInfo, UINT iUsage, HDC hCompatibleDC)
{
	m_hDC = ::CreateCompatibleDC(hCompatibleDC);
	m_hRawDC = NULL;
	m_hWnd = NULL;
	m_BitmapInfo = BitmapInfo;
	m_pBits = NULL;

	if (m_hDC)
	{
		// When using CreateDIBSection() with DIB_PAL_COLORS, the GDI will use the color indices in the
		// bmiColors to populate the color table of the DIB section by resolving the indices to the RGB
		// values from the palette selected into the device context passed into CreateDIBSection().
		// Consequently, after a program calls CreateDIBSection(), the color table will contain RGB
		// values rather than palette indices. Specifying DIB_PAL_COLORS affects only how the color
		// table is initialized, not how it is subsequently used.
		m_hBitmap = ::CreateDIBSection(m_hDC, &m_BitmapInfo, iUsage, (void**)&m_pBits, NULL, 0);
		m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_hBitmap);

	}
	else
	{
		m_hBitmap = NULL;
		m_hOldBitmap = NULL;
	}

	// The device info needs to be setup special for this kind of a DC
	m_Info.m_bPalette	  = (m_BitmapInfo.bmiHeader.biClrUsed != 0);
	m_Info.m_iResolutionX = dtoi((double)m_BitmapInfo.bmiHeader.biXPelsPerMeter / 39.37);
	m_Info.m_iResolutionY = dtoi((double)m_BitmapInfo.bmiHeader.biYPelsPerMeter / 39.37);
	m_Info.m_Size.cx	  = m_BitmapInfo.bmiHeader.biWidth;
	m_Info.m_Size.cy	  = m_BitmapInfo.bmiHeader.biHeight;
	m_Info.m_Offset.cx	  = 0;
	m_Info.m_Offset.cy	  = 0;
	Init(false/*bInitInfo*/);
	Clear();
}

//////////////////////////////////////////////////////////////////////
CAGDIBSectionDC::~CAGDIBSectionDC()
{
	Free();

	if (m_hDC)
	{
		::SelectObject(m_hDC, m_hOldBitmap);
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if (m_hBitmap)
		::DeleteObject(m_hBitmap);

	m_hRawDC = NULL;
}

//////////////////////////////////////////////////////////////////////
void CAGDIBSectionDC::Clear()
{
	if (!m_hDC)
		return;

	// Fill background 
	RECT DibRect = {0, 0, m_BitmapInfo.bmiHeader.biWidth, m_BitmapInfo.bmiHeader.biHeight};
	::FillRect(m_hDC, &DibRect, CAGBrush(WHITE_BRUSH, eStock));
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CAGDIBSectionDC::ExtractDib()
{
	if (!m_pBits)
		return NULL;

	return DibCopy(&m_BitmapInfo.bmiHeader, m_pBits);
}

//////////////////////////////////////////////////////////////////////
HANDLE CAGDIBSectionDC::ExtractDibGlobal()
{
	if (!m_pBits)
		return NULL;

	return DibCopyGlobal(&m_BitmapInfo.bmiHeader, m_pBits);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::Init(bool bInitInfo)
{
	if (bInitInfo)
		m_Info.Init(m_hDC);

	m_pBltBuf = NULL;
	m_hOldFont = NULL;
	m_hFont = NULL;
	::ZeroMemory(&m_Font, sizeof(m_Font));
	m_hOldPalette = NULL;
	m_hPalette = (m_Info.m_bPalette ? CreatePalette() : NULL);
	if (m_hDC)
	{
		if (m_hPalette)
		{
			m_hOldPalette = ::SelectPalette(m_hDC, m_hPalette, true);
			::RealizePalette(m_hDC);
		}

		::SetMapMode(m_hDC, MM_TEXT);
		::SetTextAlign(m_hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
		::SetBkMode(m_hDC, TRANSPARENT);
		::SetStretchBltMode(m_hDC, COLORONCOLOR);
		::SetPolyFillMode(m_hDC, WINDING); // vs. the default ALTERNATE
	}

	if (m_hRawDC)
	{
		::SetMapMode(m_hRawDC, MM_TEXT);
		::SetTextAlign(m_hRawDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
		::SetBkMode(m_hRawDC, TRANSPARENT);
		::SetStretchBltMode(m_hRawDC, COLORONCOLOR);
		::SetPolyFillMode(m_hRawDC, WINDING); // vs. the default ALTERNATE
	}

	m_MatrixDevice.SetMatrix((double)m_Info.m_iResolutionX / (double)APP_RESOLUTION, 0, 0, (double)m_Info.m_iResolutionY / (double)APP_RESOLUTION, -m_Info.m_Offset.cx, -m_Info.m_Offset.cy);

	m_bUpdateViewToDevice = true;
	m_bUpdateSymbolToDeviceMatrix = true;
	m_bDirty = false;
	m_bCoverAllowed = false;
	::SetRectEmpty(&m_ViewRect);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::Free()
{
	if (m_hOldFont)
	{
		::SelectObject(m_hDC, m_hOldFont);
		m_hOldFont = NULL;
	}

	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	if (m_hOldPalette)
	{
		::SelectPalette(m_hDC, m_hOldPalette, true);
		m_hOldPalette = NULL;
	}

	if (m_hPalette)
	{
		::DeleteObject(m_hPalette);
		m_hPalette = NULL;
	}

	if (m_pBltBuf)
	{
		free(m_pBltBuf);
		m_pBltBuf = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
HRGN CAGDC::SaveClipRegion()
{
	if (!m_hDC)
		return NULL;

	HRGN hClipRegion = ::CreateRectRgn(0,0,1,1);
	if (::GetClipRgn(m_hDC, hClipRegion) <= 0)
	{ // No clip region
		::DeleteObject(hClipRegion);
		return NULL;
	}

	return hClipRegion;
}

//////////////////////////////////////////////////////////////////////
void CAGDC::RestoreClipRegion(HRGN hClipRegion)
{
	if (m_hDC) // Make the select call even if hClipRegion is NULL
		::SelectClipRgn(m_hDC, hClipRegion);

	if (hClipRegion)
		::DeleteObject(hClipRegion);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::SetClipRect(const RECT& ClipRect, bool bIncludeRawDC)
{
	if (!m_hDC)
		return;

	HRGN hClipRegion = ::CreateRectRgnIndirect(&ClipRect);
	::SelectClipRgn(m_hDC, hClipRegion);
	if (bIncludeRawDC)
		::SelectClipRgn(m_hRawDC, hClipRegion);
	::DeleteObject(hClipRegion);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::SetClipToView(const SIZE& ViewSize, bool bIncludeRawDC, RECT* pViewRect)
{
	if (!m_hDC)
		return;

	::SetRect(&m_ViewRect, 0, 0, ViewSize.cx, ViewSize.cy);

	RECT ViewRect = m_ViewRect;
	GetViewToDeviceMatrix().Transform(ViewRect);
	SetClipRect(ViewRect, bIncludeRawDC);

	if (pViewRect)
		*pViewRect = ViewRect;
}

//////////////////////////////////////////////////////////////////////
HPALETTE CAGDC::CreatePalette()
{
	int nPaletteSize = sizeof(LOGPALETTE) + (216 * sizeof(PALETTEENTRY));
	LPLOGPALETTE lpLogPalette;
	if (!(lpLogPalette = (LPLOGPALETTE)malloc(nPaletteSize)))
		return NULL;

	memset(lpLogPalette, 0, nPaletteSize);
	lpLogPalette->palVersion = 0x300;
	lpLogPalette->palNumEntries = 216;
	for (int r = 0, n = 0; r < 6; r++)
	{
		for (int g = 0; g < 6; g++)
		{
			for (int b = 0; b < 6; b++, n++)
			{
				lpLogPalette->palPalEntry[n].peRed = (BYTE)(r * 51);
				lpLogPalette->palPalEntry[n].peGreen = (BYTE)(g * 51);
				lpLogPalette->palPalEntry[n].peBlue = (BYTE)(b * 51);
			}
		}
	}

	HPALETTE hPalette = ::CreatePalette(lpLogPalette);
	free(lpLogPalette);
	return hPalette;
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawPath(COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	HPEN hPen = NULL;
	HPEN hOldPen = NULL;
	if (LineColor != CLR_NONE)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = LineColor;
		lb.lbHatch = 0 ;
		DWORD dwPenStyle = PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_ROUND;
		hPen = ::ExtCreatePen(dwPenStyle, LineWidth, &lb, 0, NULL);
		hOldPen = (HPEN)::SelectObject(m_hDC, hPen);
	}

	CAGBrush Brush;
	HBRUSH hOldBrush = NULL;
	if (FillColor != CLR_NONE)
		hOldBrush = (HBRUSH)::SelectObject(m_hDC, Brush.Create(FillColor));

	if (FillColor != CLR_NONE && LineColor != CLR_NONE)
		::StrokeAndFillPath(m_hDC);
	else
	if (FillColor != CLR_NONE)
		::FillPath(m_hDC);
	else
	if (LineColor != CLR_NONE)
		::StrokePath(m_hDC);

	if (hOldBrush)
		::SelectObject(m_hDC, hOldBrush);
	if (hOldPen)
		::SelectObject(m_hDC, hOldPen);
	if (hPen)
		::DeleteObject(hPen);
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::GetClipRect(RECT* pClipRect)
{
	if (!pClipRect)
		return false;

	int iRet = ::GetClipBox(m_hDC, pClipRect);
	return (iRet == ERROR ? false : true);
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::GetABCWidths(UINT uFirstChar, UINT uLastChar, int* piMinA, int* piMaxB, int* piMaxC, int* piMaxW)
{
	int nChars = (int)uLastChar - uFirstChar + 1;
	if (nChars <= 0)
		return false;

	ABC* pABC = new ABC[nChars];

// The total width of a character (A + B + C) is equal to the Size.cx returned by ::GetTextExtentPoint32(m_hDC, uChar, 1, Size)
// Either the A or the C space can be negative to indicate underhangs or overhangs.
// The A spacing is the distance to add to the current position before drawing the character glyph. 
// The B spacing is the width of the drawn portion of the character glyph. 
// The C spacing is the distance to add to the current position to provide white space to the right of the character glyph. 
	int iMinA =  99999;
	int iMaxB = -99999;
	int iMaxC = -99999;
	int iMaxW = -99999;

	bool bOK = !!::GetCharABCWidths(m_hDC, uFirstChar, uLastChar, pABC);
	if (bOK)
	{
		for (int i = 0; i < nChars; i++)
		{
			int iWidth = max(pABC[i].abcA,0) + pABC[i].abcB + max(pABC[i].abcC,0);
			if (iMaxW < iWidth)
				iMaxW = iWidth;

			if (iMinA > pABC[i].abcA)
				iMinA = pABC[i].abcA;

			if (iMaxB < ((int)pABC[i].abcB))
				iMaxB = ((int)pABC[i].abcB);

			if (iMaxC < pABC[i].abcC)
				iMaxC = pABC[i].abcC;
		}
	}

	delete [] pABC;

	if (bOK)
	{
		if (piMinA)	*piMinA = iMinA;
		if (piMaxB)	*piMaxB = iMaxB;
		if (piMaxC)	*piMaxC = iMaxC;
		if (piMaxW)	*piMaxW = iMaxW;
	}

	return bOK;
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::GetTextExtent(LPCTSTR pString, int nCount, SIZE* pSize)
{
	return !!::GetTextExtentPoint32(m_hDC, pString, nCount, pSize);
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::GetTextMetrics(TEXTMETRIC* ptm) const
{
	::ZeroMemory(ptm, sizeof(TEXTMETRIC));
	return !!::GetTextMetrics(m_hDC, ptm);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::ComputePosition(int& x, int& y, RECT* pRectAlign, const SIZE& Size, const CAGMatrix& Matrix)
{
	if (!pRectAlign)
		return;

	TEXTMETRIC tm;
	GetTextMetrics(&tm);

	int dx = Size.cx;
	int dy = Size.cy;
	int yBaseLine = tm.tmAscent;
	int yPad = tm.tmDescent;
	int xPad = yPad / 2;

	CAGMatrix MatrixInverse = Matrix.Inverse();
	dx = dtoi(MatrixInverse.TransformWidth(dx));
	dy = dtoi(MatrixInverse.TransformHeight(dy));
	yBaseLine = dtoi(MatrixInverse.TransformHeight(yBaseLine));
	yPad = dtoi(MatrixInverse.TransformHeight(yPad));
	xPad = dtoi(MatrixInverse.TransformHeight(xPad));

	int iHorzAlign = x;
	int iVertAlign = y;

	if (iHorzAlign <  0) /*Left*/	x = pRectAlign->left + xPad;		else
	if (iHorzAlign >  0) /*Right*/	x = pRectAlign->right - xPad - dx;	else
	if (iHorzAlign == 0) /*Center*/	x = pRectAlign->left + (WIDTH(*pRectAlign) - dx)/2;

	if (iVertAlign <  0) /*Top*/	y = pRectAlign->top + yBaseLine;	else
	if (iVertAlign >  0) /*Bottom*/	y = pRectAlign->bottom - yPad;		else
	if (iVertAlign == 0) /*Center*/	y = pRectAlign->top + yBaseLine + (HEIGHT(*pRectAlign) - dy)/2;
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawText(LPCSTR pszString, UINT nCount, const int* pDxWidths, COLORREF LineColor, int LineWidth, COLORREF FillColor, int x, int y, RECT* pRectAlign)
{
	if (nCount <= 0)
		return;

	CAGMatrix Matrix = GetSymbolToDeviceMatrix();

	if (pRectAlign)
	{
		SIZE Size;
		GetTextExtent(pszString, nCount, &Size);
		ComputePosition(x, y, pRectAlign, Size, Matrix);
	}

	int* pWidths = NULL;
	if (pDxWidths)
	{
		if (!(pWidths = new int[nCount]))
			pWidths = (int*)pDxWidths;
		else
		{
			int iDistance = 0;
			int xLast = 0;
			for (UINT i = 0; i < nCount; i++)
			{
				iDistance += pDxWidths[i];
				int xCurr = dtoi(Matrix.TransformDistance(iDistance));
				pWidths[i] = xCurr - xLast;
				xLast = xCurr;
			}
		}
	}

	int iMinA = 0;

#ifdef NOTUSED //j
	int iMaxB = 0;
	int iMaxC = 0;
	int iMaxW = 0;
	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(m_Font, false/*bTransform*/);
	BYTE bFirstChar = *pszString;
	TextIC.GetABCWidths(bFirstChar, bFirstChar, &iMinA, &iMaxB, &iMaxC, &iMaxW);
#endif NOTUSED //j

	POINT pt = {x - iMinA, y};
	Matrix.Transform(pt);

	if (FillColor != CLR_NONE && LineWidth == LT_None)
	{
		::SetTextColor(m_hDC, FillColor);
		::ExtTextOut(m_hDC, pt.x, pt.y, NULL/*nRectType*/, NULL/*pRect*/, pszString, nCount, pWidths);
	}
	else
	{
		::BeginPath(m_hDC);
		::ExtTextOut(m_hDC, pt.x, pt.y, NULL/*nRectType*/, NULL/*pRect*/, pszString, nCount, pWidths);
		::CloseFigure(m_hDC);
		::EndPath(m_hDC);

		DrawPath(LineColor, LineWidth, FillColor);
	}

	if (pWidths && (pWidths != pDxWidths))
		delete [] pWidths;
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::SetClipText(LPCTSTR pszString, UINT nCount, const int* pDxWidths, int x, int y, RECT* pRectAlign)
{
	if (!m_hDC)
		return false;

	if (nCount <= 0)
		return false;

	CAGMatrix Matrix = GetSymbolToDeviceMatrix();

	if (pRectAlign)
	{
		SIZE Size;
		GetTextExtent(pszString, nCount, &Size);
		ComputePosition(x, y, pRectAlign, Size, Matrix);
	}

	int* pWidths = NULL;
	if (pDxWidths)
	{
		if (!(pWidths = new int[nCount]))
			pWidths = (int*)pDxWidths;
		else
		{
			int iDistance = 0;
			int xLast = 0;
			for (UINT i = 0; i < nCount; i++)
			{
				iDistance += pDxWidths[i];
				int xCurr = dtoi(Matrix.TransformDistance(iDistance));
				pWidths[i] = xCurr - xLast;
				xLast = xCurr;
			}
		}
	}

	int iMinA = 0;

#ifdef NOTUSED //j
	int iMaxB = 0;
	int iMaxC = 0;
	int iMaxW = 0;
	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(m_Font, false/*bTransform*/);
	BYTE bFirstChar = *pszString;
	TextIC.GetABCWidths(bFirstChar, bFirstChar, &iMinA, &iMaxB, &iMaxC, &iMaxW);
#endif NOTUSED //j

	POINT pt = {x - iMinA, y};
	Matrix.Transform(pt);

	::BeginPath(m_hDC);
	::ExtTextOut(m_hDC, pt.x, pt.y, NULL/*nRectType*/, NULL/*pRect*/, pszString, nCount, pWidths);
	::CloseFigure(m_hDC);
	::EndPath(m_hDC);

	if (pWidths && (pWidths != pDxWidths))
		delete [] pWidths;

	// Create a new clip region from the shape (path) combined with the existing clip region
	return !!::SelectClipPath(m_hDC, RGN_AND);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawLine(POINT* pPoints, COLORREF LineColor, int LineWidth)
{
	::BeginPath(m_hDC);
	::MoveToEx(m_hDC, pPoints[0].x, pPoints[0].y, NULL);
	::LineTo(m_hDC, pPoints[1].x, pPoints[1].y);
	::EndPath(m_hDC);

	DrawPath(LineColor, LineWidth, CLR_NONE/*FillColor*/);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawPolygon(POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor, int nPoints)
{
	if (!m_hDC)
		return;

	::BeginPath(m_hDC);
	::Polygon(m_hDC, pPoints, nPoints);
	::EndPath(m_hDC);

	DrawPath(LineColor, LineWidth, FillColor);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawRectangle(POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	if (!m_hDC)
		return;

	::BeginPath(m_hDC);
	::Polygon(m_hDC, pPoints, 4);
	::EndPath(m_hDC);

	DrawPath(LineColor, LineWidth, FillColor);
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::SetClipRectangle(POINT* pPoints)
{
	if (!m_hDC)
		return false;

	::BeginPath(m_hDC);
	::Polygon(m_hDC, pPoints, 4);
	::EndPath(m_hDC);

	// Create a new clip region from the shape (path) combined with the existing clip region
	return !!::SelectClipPath(m_hDC, RGN_AND);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawEllipse(POINT* pPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	if (!m_hDC)
		return;

	::BeginPath(m_hDC);
	::MoveToEx(m_hDC, pPoints[0].x, pPoints[0].y, NULL);
	::PolyBezierTo(m_hDC, &pPoints[1], 12);
	::EndPath(m_hDC);

	DrawPath(LineColor, LineWidth, FillColor);
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::SetClipEllipse(POINT* pPoints)
{
	if (!m_hDC)
		return false;

	::BeginPath(m_hDC);
	::MoveToEx(m_hDC, pPoints[0].x, pPoints[0].y, NULL);
	::PolyBezierTo(m_hDC, &pPoints[1], 12);
	::EndPath(m_hDC);

	// Create a new clip region from the shape (path) combined with the existing clip region
	return !!::SelectClipPath(m_hDC, RGN_AND);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::DrawShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints, COLORREF LineColor, int LineWidth, COLORREF FillColor)
{
	if (!m_hDC)
		return;

	::BeginPath(m_hDC);

	if (!nCommands)
	{	// Support the old way of doing things
		::Polygon(m_hDC, pPoints, nPoints);
	}
	else
	{
		int nCurrentPoint = 0;
		for (int i = 0; i < nCommands; i++)
		{
			if (pCommands->x == SYMBOL_MOVE)
			{
				::CloseFigure(m_hDC);
				::MoveToEx(m_hDC, pPoints[nCurrentPoint].x, pPoints[nCurrentPoint].y, NULL);
			}
			else
			if (pCommands->x == SYMBOL_POLYGON)
				::Polygon(m_hDC, &pPoints[nCurrentPoint], pCommands->y);
			else 
			if (pCommands->x == SYMBOL_BEZIER)
				::PolyBezierTo(m_hDC, &pPoints[nCurrentPoint], pCommands->y);
			else
			if (pCommands->x == SYMBOL_LINE)
				::LineTo(m_hDC, pPoints[nCurrentPoint].x, pPoints[nCurrentPoint].y);

			nCurrentPoint += pCommands->y;
			pCommands++;
		}
	}

	::CloseFigure(m_hDC);
	::EndPath(m_hDC);

	DrawPath(LineColor, LineWidth, FillColor);
}

//////////////////////////////////////////////////////////////////////
bool CAGDC::SetClipShape(POINT* pCommands, int nCommands, POINT* pPoints, int nPoints)
{
	if (!m_hDC)
		return false;

	::BeginPath(m_hDC);

	if (!nCommands)
	{	// Support the old way of doing things
		::Polygon(m_hDC, pPoints, nPoints);
	}
	else
	{
		int nCurrentPoint = 0;
		for (int i = 0; i < nCommands; i++)
		{
			if (pCommands->x == SYMBOL_MOVE)
			{
				::CloseFigure(m_hDC);
				::MoveToEx(m_hDC, pPoints[nCurrentPoint].x, pPoints[nCurrentPoint].y, NULL);
			}
			else
			if (pCommands->x == SYMBOL_POLYGON)
				::Polygon(m_hDC, &pPoints[nCurrentPoint], pCommands->y);
			else 
			if (pCommands->x == SYMBOL_BEZIER)
				::PolyBezierTo(m_hDC, &pPoints[nCurrentPoint], pCommands->y);
			else
			if (pCommands->x == SYMBOL_LINE)
				::LineTo(m_hDC, pPoints[nCurrentPoint].x, pPoints[nCurrentPoint].y);

			nCurrentPoint += pCommands->y;
			pCommands++;
		}
	}

	::CloseFigure(m_hDC);
	::EndPath(m_hDC);

	// Create a new clip region from the shape (path) combined with the existing clip region
	return !!::SelectClipPath(m_hDC, RGN_AND);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::FillRectangle(POINT* pPoints, COLORREF FillColor)
{
	if (FillColor != CLR_NONE)
		DrawRectangle(pPoints, CLR_NONE/*LineColor*/, 0/*LineWidth*/, FillColor);
}

#define FRAC_BITS 16
#define FRAC_MASK 0xFFFF
#define STEP_PRECISION 2

//////////////////////////////////////////////////////////////////////
void CAGDC::FillLinearGradient(POINT* pPoints, COLORREF clrFrom, COLORREF clrTo, BOOL bHorizontal)
{
	if (!m_hDC)
		return;

	if (clrFrom == clrTo)
	{
		FillRectangle(pPoints, clrFrom);
		return;
	}
	
	// Compute polygon step increments using fixed point arithmetic: (16.16), (whole, fractional)
	int dx = 0;
	int dy = 0;

	if (bHorizontal)
	{
		dx = pPoints[1].x - pPoints[0].x;
		dy = pPoints[1].y - pPoints[0].y;
	}
	else
	{
		dx = pPoints[2].x - pPoints[1].x;
		dy = pPoints[2].y - pPoints[1].y;
	}
	
	long nSpan = (long)sqrt((double)((dx * dx) + (dy * dy)));
	long nSteps = nSpan / STEP_PRECISION;
	if (!nSteps)
	{
		FillRectangle(pPoints, clrFrom);
		return;
	}

	CPoint ptStep;
	ptStep.x = (dx << FRAC_BITS) / nSteps;
	ptStep.y = (dy << FRAC_BITS) / nSteps;

	// Setup color increments
	DWORD r1 = GetRValue(clrFrom) << FRAC_BITS;
	DWORD g1 = GetGValue(clrFrom) << FRAC_BITS;
	DWORD b1 = GetBValue(clrFrom) << FRAC_BITS;
	DWORD r2 = GetRValue(clrTo) << FRAC_BITS;
	DWORD g2 = GetGValue(clrTo) << FRAC_BITS;
	DWORD b2 = GetBValue(clrTo) << FRAC_BITS;
	
	long dwRStep = ((long)r2 - (long)r1) / nSteps;
	long dwGStep = ((long)g2 - (long)g1) / nSteps;
	long dwBStep = ((long)b2 - (long)b1) / nSteps;

	// Setup the initial color
	DWORD r = r1;
	DWORD g = g1;
	DWORD b = b1;

	// Setup the initial color bar
	CPoint ptBarHorz[] = {pPoints[0], pPoints[0], pPoints[3], pPoints[3]};
	CPoint ptBarVert[] = {pPoints[1], pPoints[1], pPoints[0], pPoints[0]};
	CPoint* ptBar = (bHorizontal ? ptBarHorz : ptBarVert);
	
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, ::GetStockObject(NULL_PEN));
	CAGBrush Brush;
	CPoint ptFraction(0,0);

	// Draw bars at each increment
	for (int i = 0; i < nSteps; i++)
	{
		// Increment a fractional step
		ptFraction += ptStep;
		long xStep = ptFraction.x >> FRAC_BITS;
		long yStep = ptFraction.y >> FRAC_BITS;
		ptFraction.x &= FRAC_MASK;
		ptFraction.y &= FRAC_MASK;

		// Bump up the bar
		ptBar[1].x += xStep;
		ptBar[1].y += yStep;
		ptBar[2].x += xStep;
		ptBar[2].y += yStep;

		// Bump the color value
		r += dwRStep;
		g += dwGStep;
		b += dwBStep;

		// If the integer part of the color has changed, draw the next bar
		if ((r >> FRAC_BITS) != (r1 >> FRAC_BITS) ||
			(g >> FRAC_BITS) != (g1 >> FRAC_BITS) ||
			(b >> FRAC_BITS) != (b1 >> FRAC_BITS))
		{			
			// Draw the filled bar
			Brush.Create((BYTE)(r >> FRAC_BITS), (BYTE)(g >> FRAC_BITS), (BYTE)(b >> FRAC_BITS));
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDC, Brush);
			::Polygon(m_hDC, ptBar, 4);
			::SelectObject(m_hDC, hOldBrush);

			// Reset the color for next cycle
			r1 = r;
			g1 = g;
			b1 = b;

			// Inch the bar along
			ptBar[0] = ptBar[1];
			ptBar[3] = ptBar[2];
		}
	}

	// Draw final bar
	Brush.Create((BYTE)(r >> FRAC_BITS), (BYTE)(g >> FRAC_BITS), (BYTE)(b >> FRAC_BITS));
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDC, Brush);
	ptBar[1] = pPoints[1];
	ptBar[2] = pPoints[2];
	::Polygon(m_hDC, ptBar, 4);
	::SelectObject(m_hDC, hOldBrush);
	::SelectObject(m_hDC, hOldPen);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::FillRadialGradient(POINT* pPoints, COLORREF clrFrom, COLORREF clrTo, bool bCenter)
{
	if (!m_hDC)
		return;

	if (clrFrom == clrTo)
	{
		FillRectangle(pPoints, clrFrom);
		return;
	}
	
	int px = pPoints[1].x - pPoints[0].x;
	int py = pPoints[1].y - pPoints[0].y;
	int iWidth = dtoi(sqrt((double)((px * px) + (py * py))));

	px = pPoints[2].x - pPoints[1].x;
	py = pPoints[2].y - pPoints[1].y;
	int iHeight = dtoi((double)(sqrt((double)((px * px) + (py * py)))));

	int iRadius = min(iWidth, iHeight);
	if (bCenter)
		iRadius /= 2;
	if (!iRadius)
	{
		FillRectangle(pPoints, clrFrom);
		return;
	}

	CRect rect;
	rect.left   = min(min(min(pPoints[0].x, pPoints[1].x), pPoints[2].x), pPoints[3].x);
	rect.right  = max(max(max(pPoints[0].x, pPoints[1].x), pPoints[2].x), pPoints[3].x);
	rect.top    = min(min(min(pPoints[0].y, pPoints[1].y), pPoints[2].y), pPoints[3].y);
	rect.bottom = max(max(max(pPoints[0].y, pPoints[1].y), pPoints[2].y), pPoints[3].y);

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

	for (int x = 0; x < rect.Width(); x++) 
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

			::SetPixel(m_hDC, rect.left + x, rect.top + y, Color);
		}
	}
}

//////////////////////////////////////////////////////////////////////
const CAGMatrix& CAGDC::GetSymbolToDeviceMatrix()
{
	if (m_bUpdateSymbolToDeviceMatrix)
	{
		m_MatrixSymbolToDevice = m_MatrixSymbol * GetViewToDeviceMatrix();
		m_bUpdateSymbolToDeviceMatrix = false;
	}

	return m_MatrixSymbolToDevice;
}

//////////////////////////////////////////////////////////////////////
const CAGMatrix& CAGDC::GetViewToDeviceMatrix()
{
	if (m_bUpdateViewToDevice)
	{
		m_MatrixViewToDevice = m_MatrixView * m_MatrixDevice;
		m_bUpdateViewToDevice = false;
	}
	return m_MatrixViewToDevice;
}

//////////////////////////////////////////////////////////////////////
const CAGMatrix& CAGDC::SetSymbolMatrix(const CAGMatrix& Matrix)
{
	m_MatrixSaved = m_MatrixSymbol;
	m_MatrixSymbol = Matrix;
	m_bUpdateSymbolToDeviceMatrix = true;
	return m_MatrixSaved;
}

//////////////////////////////////////////////////////////////////////
void CAGDC::SetSymbolMatrix()
{
	m_MatrixSymbol.Unity();
	m_bUpdateSymbolToDeviceMatrix = true;
}

//////////////////////////////////////////////////////////////////////
void CAGDC::Polygon(const POINT* pPts, int nPoints)
{
	POINT* pPoints = new POINT[nPoints];
	memcpy(pPoints, pPts, nPoints * sizeof(POINT));

	GetSymbolToDeviceMatrix().Transform(pPoints, nPoints);

	::Polygon(m_hDC, pPoints, nPoints);

	delete [] pPoints;
}

//////////////////////////////////////////////////////////////////////
void CAGDC::Rectangle(const RECT& Rect)
{
	POINT pt[5];
	pt[0].x = Rect.left;	pt[0].y = Rect.top;
	pt[1].x = Rect.right;	pt[1].y = Rect.top;
	pt[2].x = Rect.right;	pt[2].y = Rect.bottom;
	pt[3].x = Rect.left;	pt[3].y = Rect.bottom;
	pt[4].x = Rect.left;	pt[4].y = Rect.top;
	Polygon(pt, 5);
}

//////////////////////////////////////////////////////////////////////
void CAGDC::SetFont(const LOGFONT& theFont, bool bTransform)
{
	m_Font = theFont;

	if (bTransform)
	{
		CAGMatrix Matrix = GetSymbolToDeviceMatrix();
		double Angle = Matrix.GetAngle();
		if (Angle)
			m_Font.lfEscapement = dtoi(Angle * 10);

		if (m_Font.lfWidth)
			m_Font.lfWidth = dtoi(Matrix.TransformDistance(m_Font.lfWidth));

		double fHeight = Matrix.TransformHeight(m_Font.lfHeight);
		m_Font.lfHeight = dtoi((m_Font.lfHeight < 0) ? -fHeight : fHeight);
	}

	if (m_hOldFont)
	{
		::SelectObject(m_hDC, m_hOldFont);
		m_hOldFont = NULL;
	}

	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	m_hFont = ::CreateFontIndirect(&m_Font);
	m_hOldFont = (HFONT)::SelectObject(m_hDC, m_hFont);
}

//////////////////////////////////////////////////////////////////////
void SetMaskBits(const BITMAPINFOHEADER* pDIB, const BYTE* pSrc, BYTE* pNewBits, COLORREF TransparentColor)
{
	if (!pSrc || !pNewBits || !pDIB)
		return;

	if (pDIB->biBitCount == 8)
	{
		RGBQUAD* pRGB = DibColors(pDIB);
		if (!pRGB)
			return;
		DWORD nColors = DibNumColors(pDIB);
		BYTE* pDest = pNewBits;
		for (int y = 0; y < pDIB->biHeight; y++)
		{
			for (int x = 0; x < pDIB->biWidth; x++)
			{
				BYTE cIndex = pSrc[x];
				COLORREF SrcColor = CLR_NONE;
				if (cIndex < nColors)
				{
					RGBQUAD* p = &pRGB[cIndex];
					SrcColor = RGB(p->rgbRed, p->rgbGreen, p->rgbBlue);
				}
				BYTE cValue = (SrcColor == TransparentColor ? 1 : 0);
				pDest[x] = cValue; // Remember, this is a palette entry
			}

			pSrc += DibWidthBytes(pDIB);
			pDest += DibWidthBytes(pDIB);
		}
	}
	else
	if (pDIB->biBitCount == 24)
	{
		BYTE* pDest = pNewBits;
		for (int y = 0; y < pDIB->biHeight; y++)
		{
			for (int x = 0; x < pDIB->biWidth; x++)
			{
				int n = x * 3;
				COLORREF SrcColor = RGB(pSrc[n+2], pSrc[n+1], pSrc[n+0]);
				BYTE cValue = (SrcColor == TransparentColor ? 255 : 0);
				pDest[n+0] = cValue;
				pDest[n+1] = cValue;
				pDest[n+2] = cValue;
			}

			pSrc += DibWidthBytes(pDIB);
			pDest += DibWidthBytes(pDIB);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CAGDC::StretchBlt(const BITMAPINFOHEADER* pDIB, RECT CropRect, RECT DestRect, COLORREF TransparentColor)
{
	if (!pDIB)
		return;

	RECT DibRect;
	::IntersectRect(&CropRect, &CropRect, &DestRect);
	if (::IsRectEmpty(&CropRect))
	{
		::SetRect(&DibRect, 0, 0, pDIB->biWidth, pDIB->biHeight);
	}
	else
	{
		double CropL = (double)(CropRect.left   - DestRect.left) / WIDTH(DestRect);
		double CropR = (double)(CropRect.right  - DestRect.left) / WIDTH(DestRect);
		double CropT = (double)(CropRect.top    - DestRect.top)  / HEIGHT(DestRect);
		double CropB = (double)(CropRect.bottom - DestRect.top)  / HEIGHT(DestRect);

		double fx = pDIB->biWidth;
		double fy = pDIB->biHeight;
		DibRect.left   = dtoi(fx * CropL);
		DibRect.right  = dtoi(fx * CropR);
		DibRect.top    = dtoi(fy * CropT);
		DibRect.bottom = dtoi(fy * CropB);

		DestRect = CropRect;
	}

	CAGMatrix Matrix = GetSymbolToDeviceMatrix();
	Matrix.Transform((POINT&)DestRect.left); // Won't get the correct result by calling Matrix.Transform(DestRect)
	Matrix.Transform((POINT&)DestRect.right); // This approach maintains a negative width or height if flipped
	if (!WIDTH(DestRect) || !HEIGHT(DestRect))
		return;

	bool bDeleteDib = false;
	bool bFlipX = (WIDTH(DestRect) < 0);
	bool bFlipY = (HEIGHT(DestRect) < 0);
	if (bFlipX || bFlipY)
	{
		BITMAPINFOHEADER* pDIBNew = CDib::Orient(pDIB, 0/*iRotateDirection*/, bFlipX, bFlipY);
		if (pDIBNew)
		{
			pDIB = pDIBNew;
			bDeleteDib = true;
			if (bFlipX)
				SWAP(DestRect.left, DestRect.right);
			if (bFlipY)
				SWAP(DestRect.top, DestRect.bottom);
		}
	}

	if (TransparentColor == CLR_NONE)
	{
		StretchBlt2(pDIB, DibPtr(pDIB), DibRect, DestRect, SRCCOPY);
	}
	else
	{
		StretchBlt2(pDIB, DibPtr(pDIB), DibRect, DestRect, SRCINVERT);

		if (pDIB->biBitCount == 8 && m_Info.m_bRasDisplay)
		{
			// Set in a "mask" palette that will retain transparent colors and remove non-transparent colors
			// Save the original palette entries as we go
			RGBQUAD SavedRGB[256];
			RGBQUAD* pRGB = DibColors(pDIB);
			DWORD nColors = DibNumColors(pDIB);
			for (DWORD i = 0; i < nColors; i++)
			{
				SavedRGB[i] = pRGB[i];
				COLORREF SrcColor = RGB(pRGB[i].rgbRed, pRGB[i].rgbGreen, pRGB[i].rgbBlue);
				pRGB[i].rgbRed = pRGB[i].rgbGreen = pRGB[i].rgbBlue = (SrcColor == TransparentColor ? 255 : 0);
			}

			StretchBlt2(pDIB, DibPtr(pDIB), DibRect, DestRect, SRCAND);

			// Restore the original palette entries
			for (DWORD i = 0; i < nColors; i++)
				pRGB[i] = SavedRGB[i];
		}
		else
		{ // everything else
			// Create the mask bitmap
			BYTE* pMaskBits = (BYTE*)malloc(DibSizeImage(pDIB));
			if (pMaskBits)
			{
				SetMaskBits(pDIB, DibPtr(pDIB), pMaskBits, TransparentColor);

				if (pDIB->biBitCount == 8)
				{
					RGBQUAD* pRGB = DibColors(pDIB);
					RGBQUAD SavedRGB0 = pRGB[0];
					pRGB[0].rgbRed = pRGB[0].rgbGreen = pRGB[0].rgbBlue = 0;
					RGBQUAD SavedRGB1 = pRGB[1];
					pRGB[1].rgbRed = pRGB[1].rgbGreen = pRGB[1].rgbBlue = 255;

					StretchBlt2(pDIB, pMaskBits, DibRect, DestRect, SRCAND);

					pRGB[0] = SavedRGB0;
					pRGB[1] = SavedRGB1;
				}
				else
				{
					StretchBlt2(pDIB, pMaskBits, DibRect, DestRect, SRCAND);
				}

				free ((void*)pMaskBits);
			}
		}

		StretchBlt2(pDIB, DibPtr(pDIB), DibRect, DestRect, SRCINVERT);
	}

	if (bDeleteDib)
		free((void*)pDIB);
}

//////////////////////////////////////////////////////////////////////
// Fixed point math for sampling images

#define IntToFixed(i) (Fixed)(((DWORD)(i)) << 16)
#define FixedToInt(f) (WORD)(((DWORD)(f)) >> 16)

//////////////////////////////////////////////////////////////////////
Fixed CAGDC::FixedDivide(Fixed Dividend, Fixed Divisor) const
{
	Fixed fixResult = 0;
	_asm
	{
		mov 	eax,Dividend
		mov 	ecx,Divisor

		rol 	eax,10h
		movsx	edx,ax
		xor 	ax,ax

		idiv	ecx
		shld	edx,eax,16
		mov 	[fixResult], eax
	};

	return fixResult;
}

#define BLTBUFSIZE (128L * 1024L)

//////////////////////////////////////////////////////////////////////
void CAGDC::StretchBlt2(const BITMAPINFOHEADER* pDIB, const BYTE* pBits, RECT DibRect, RECT DestRect, DWORD dwBltMode)
{
	if (!pDIB || !pBits)
		return;

	bool bWindowsBlt = false;

	if (!m_Info.m_bRasDisplay) // If Printing...
		bWindowsBlt = true; // At print time, use the Windows blt'er - it is faster for printing, especially when transparent
	else
	{
		if (!m_pBltBuf)
		{
			m_pBltBuf = (BYTE*)malloc(BLTBUFSIZE);
			if (!m_pBltBuf)
				bWindowsBlt = true;  // Use the Windows blt'er instead
		}
	}

	int nDstX = DestRect.left;
	int nDstY = DestRect.top;
	int nDstWidth = WIDTH(DestRect);
	int nDstHeight = HEIGHT(DestRect);

	if (bWindowsBlt)
	{
		int y = pDIB->biHeight - DibRect.bottom;
		int nLines = ::StretchDIBits(m_hDC, nDstX, nDstY, nDstWidth, nDstHeight,
			DibRect.left, y, WIDTH(DibRect), HEIGHT(DibRect), pBits, (BITMAPINFO*)pDIB, DIB_RGB_COLORS, dwBltMode);
		return;
	}

	// Not Printing - This is a much faster display blt'er because of faster sampling (it also dithers if necessary)
	if (!m_pBltBuf)
	{
		m_pBltBuf = (BYTE*)malloc(BLTBUFSIZE);
		if (!m_pBltBuf)
			return;
	}

	BITMAPINFOEX bi;
	::ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = nDstWidth;
	bi.bmiHeader.biHeight = 0;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biXPelsPerMeter = 3937; // 100 pixels/inch
	bi.bmiHeader.biYPelsPerMeter = 3937;

	UINT iUsage = DIB_RGB_COLORS; 

	if (m_Info.m_bPalette)
	{
		bi.bmiHeader.biBitCount = 8;
		bi.bmiHeader.biClrUsed = 256;
		for (WORD i = 0; i < 256; i++)
			((WORD*)bi.bmiColors)[i] = i;
		iUsage = DIB_PAL_COLORS;
	}
	else
	{
		bi.bmiHeader.biBitCount = (pDIB->biBitCount == 1 || pDIB->biBitCount == 4 ? 8 : pDIB->biBitCount);
		bi.bmiHeader.biClrUsed = pDIB->biClrUsed;
		memcpy(bi.bmiColors, ((const BITMAPINFO*)pDIB)->bmiColors, pDIB->biClrUsed * sizeof(RGBQUAD));
	}

	DWORD dwSrcWidthBytes = DibWidthBytes(pDIB);
	DWORD dwDstWidthBytes = DibWidthBytes(&bi.bmiHeader);
	int nMaxLinesPerBlt = BLTBUFSIZE / dwDstWidthBytes;
	bi.bmiHeader.biHeight = nMaxLinesPerBlt;

	DWORD dwSrcCropBytes = 0;
	if (pDIB->biBitCount == 24)
		dwSrcCropBytes = DibRect.left * 3;
	else
	if (pDIB->biBitCount == 8)
		dwSrcCropBytes = DibRect.left;
	else
	if (pDIB->biBitCount == 4)
		dwSrcCropBytes = DibRect.left / 2;
	else
	if (pDIB->biBitCount == 1)
		dwSrcCropBytes = DibRect.left / 8;

	BYTE* pSrcBitsStart = ((BYTE*)pBits) + ((pDIB->biHeight - 1) * dwSrcWidthBytes) + dwSrcCropBytes;
	BYTE* pSrcBits = NULL;
	BYTE* pDstBits = m_pBltBuf + ((nMaxLinesPerBlt - 1) * dwDstWidthBytes);
	BYTE* pPrevDstBits = NULL;

	bool bXStretch = (nDstWidth != WIDTH(DibRect) || pDIB->biBitCount == 4 || pDIB->biBitCount == 1);
	int nLinesToBlt = 0;

	Fixed fixSrcStepX = FixedDivide(IntToFixed(WIDTH(DibRect)), IntToFixed(nDstWidth));
	Fixed fixSrcStepY = FixedDivide(IntToFixed(HEIGHT(DibRect)), IntToFixed(nDstHeight));
	Fixed fixPosX = 0;
	Fixed fixPosY = IntToFixed(DibRect.top) + (fixSrcStepY >> 1);
	WORD wSrcPosX = 0;
	WORD wSrcPosY = 0;
	WORD wPrevPosY = 0xFFFF;

	for (int y = 0; y < nDstHeight; y++)
	{
		wSrcPosY = FixedToInt(fixPosY);
		fixPosY += fixSrcStepY;
		pSrcBits = pSrcBitsStart - (wSrcPosY * dwSrcWidthBytes);

		if (m_Info.m_bPalette)
			Dither(pSrcBits, pDstBits, pDIB->biBitCount, nDstWidth, y, fixSrcStepX, ((const BITMAPINFO*)pDIB)->bmiColors);
		else
		if (wSrcPosY == wPrevPosY)
			memcpy(pDstBits, pPrevDstBits, dwDstWidthBytes);
		else
		if (!bXStretch)
			memcpy(pDstBits, pSrcBits, dwDstWidthBytes);
		else
		{
			fixPosX = (fixSrcStepX >> 1);

			if (pDIB->biBitCount == 8)
			{
				for (int x = 0; x < nDstWidth; )
				{
					wSrcPosX = FixedToInt(fixPosX);
					fixPosX += fixSrcStepX;
					pDstBits[x++] = pSrcBits[wSrcPosX];
				}
			}
			else
			if (pDIB->biBitCount == 24)
			{
				for (int x = 0; x < nDstWidth * 3; )
				{
					wSrcPosX = FixedToInt(fixPosX) * 3;
					fixPosX += fixSrcStepX;
					pDstBits[x++] = pSrcBits[wSrcPosX];
					pDstBits[x++] = pSrcBits[wSrcPosX + 1];
					pDstBits[x++] = pSrcBits[wSrcPosX + 2];
				}
			}
			else
			if (pDIB->biBitCount == 4)
			{
				for (int x = 0; x < nDstWidth; )
				{
					wSrcPosX = FixedToInt(fixPosX);
					fixPosX += fixSrcStepX;
					if (wSrcPosX % 2)
						pDstBits[x++] = pSrcBits[wSrcPosX >> 1] & 0x0F;
					else
						pDstBits[x++] = pSrcBits[wSrcPosX >> 1] >> 4;
				}
			}
			else
			if (pDIB->biBitCount == 1)
			{
				for (int x = 0; x < nDstWidth; )
				{
					wSrcPosX = FixedToInt(fixPosX);
					fixPosX += fixSrcStepX;
					int nBitPos = wSrcPosX % 8;
					pDstBits[x++] = pSrcBits[wSrcPosX / 8] >> (7 - nBitPos) & 0x01;
				}
			}
		}

		wPrevPosY = wSrcPosY;
		pPrevDstBits = pDstBits;

		if (++nLinesToBlt >= nMaxLinesPerBlt)
		{
			::StretchDIBits(m_hDC, nDstX, nDstY, nDstWidth, nLinesToBlt,
				0, 0, nDstWidth, nLinesToBlt, m_pBltBuf, (BITMAPINFO*)&bi.bmiHeader, iUsage, dwBltMode);

			nDstY += nLinesToBlt;
			nLinesToBlt = 0;
			pDstBits = m_pBltBuf + ((nMaxLinesPerBlt - 1) * dwDstWidthBytes);
		}
		else
			pDstBits -= dwDstWidthBytes;
	}
	
	if (nLinesToBlt)
	{
		bi.bmiHeader.biHeight = nLinesToBlt;
		pDstBits = m_pBltBuf + ((nMaxLinesPerBlt - nLinesToBlt) * dwDstWidthBytes);

		::StretchDIBits(m_hDC, nDstX, nDstY, nDstWidth, nLinesToBlt,
			0, 0, nDstWidth, nLinesToBlt, pDstBits, (BITMAPINFO*)&bi.bmiHeader, iUsage, dwBltMode);
	}
}

static const BYTE aHalftone16x16[16][16] =
{
	 0, 44,  9, 41,  3, 46, 12, 43,  1, 44, 10, 41,  3, 46, 12, 43,
	34, 16, 25, 19, 37, 18, 28, 21, 35, 16, 26, 19, 37, 18, 28, 21,
	38,  6, 47,  3, 40,  9, 50,  6, 38,  7, 47,  4, 40,  9, 49,  6,
	22, 28, 13, 31, 25, 31, 15, 34, 22, 29, 13, 32, 24, 31, 15, 34,
	 2, 46, 12, 43,  1, 45, 10, 42,  2, 45, 11, 42,  1, 45, 11, 42,
	37, 18, 27, 21, 35, 17, 26, 20, 36, 17, 27, 20, 36, 17, 26, 20,
	40,  8, 49,  5, 38,  7, 48,  4, 39,  8, 48,  5, 39,  7, 48,  4,
	24, 30, 15, 33, 23, 29, 13, 32, 23, 30, 14, 33, 23, 29, 14, 32,
	 2, 46, 12, 43,  0, 44, 10, 41,  3, 47, 12, 44,  0, 44, 10, 41,
	37, 18, 27, 21, 35, 16, 25, 19, 37, 19, 28, 22, 35, 16, 25, 19,
	40,  9, 49,  5, 38,  7, 47,  4, 40,  9, 50,  6, 38,  6, 47,  3,
	24, 30, 15, 34, 22, 29, 13, 32, 25, 31, 15, 34, 22, 28, 13, 31,
	 1, 45, 11, 42,  2, 46, 11, 42,  1, 45, 10, 41,  2, 46, 11, 43,
	36, 17, 26, 20, 36, 17, 27, 21, 35, 16, 26, 20, 36, 18, 27, 21,
	39,  8, 48,  4, 39,  8, 49,  5, 38,  7, 48,  4, 39,  8, 49,  5,
	23, 29, 14, 33, 24, 30, 14, 33, 23, 29, 13, 32, 24, 30, 14, 33
};

static const BYTE aModulo51[256] =
{
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0
};

static const BYTE aDividedBy51[256] =
{
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 5
};

static const BYTE aTimes6[6] =
{
	 0, 6, 12, 18, 24, 30
};

static const BYTE aTimes36[6] =
{
	 0, 36, 72, 108, 144, 180
};

//////////////////////////////////////////////////////////////////////
void CAGDC::Dither(const BYTE* pSrcBits, BYTE* pDstBits, int nSrcBitCount, int nDstWidth, int y, Fixed fixSrcStepX, const RGBQUAD* pColors) const
{
	Fixed fixPosX = (fixSrcStepX >> 1);
	WORD wSrcPosX = 0;
	int Red = 0;
	int Green = 0;
	int Blue = 0;
	BYTE bIndex, bTemp, bRedTemp, bGreenTemp, bBlueTemp;

	for (int x = 0; x < nDstWidth; x++)
	{
		wSrcPosX = FixedToInt(fixPosX);
	
		if (nSrcBitCount == 8)
		{
			bIndex = pSrcBits[wSrcPosX];
			Red = pColors[bIndex].rgbRed;
			Green = pColors[bIndex].rgbGreen;
			Blue = pColors[bIndex].rgbBlue;
		}
		else
		if (nSrcBitCount == 24)
		{
			wSrcPosX *= 3;
			Blue = pSrcBits[wSrcPosX];
			Green = pSrcBits[wSrcPosX + 1];
			Red = pSrcBits[wSrcPosX + 2];
		}
		else
		if (nSrcBitCount == 4)
		{
			if (wSrcPosX % 2)
				bIndex = (BYTE)(pSrcBits[wSrcPosX >> 1] & 0x0F);
			else
				bIndex = (BYTE)(pSrcBits[wSrcPosX >> 1] >> 4);
			Red = pColors[bIndex].rgbRed;
			Green = pColors[bIndex].rgbGreen;
			Blue = pColors[bIndex].rgbBlue;
		}

		fixPosX += fixSrcStepX;

		bTemp = aHalftone16x16[(x & 0x0F)][(y & 0x0F)];
		bRedTemp = aDividedBy51[Red];
		bGreenTemp = aDividedBy51[Green];
		bBlueTemp = aDividedBy51[Blue];

		if (aModulo51[Red] > bTemp)
			bRedTemp++;
		if (aModulo51[Green] > bTemp)
			bGreenTemp++;
		if (aModulo51[Blue] > bTemp)
			bBlueTemp++;

		pDstBits[x] = (BYTE)(aTimes36[bRedTemp] + aTimes6[bGreenTemp] + bBlueTemp);
	}
}
