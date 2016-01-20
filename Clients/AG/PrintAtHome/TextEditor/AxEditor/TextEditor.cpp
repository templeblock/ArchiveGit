// TextEditor.cpp : Implementation of CTextEditor
#include "stdafx.h"
#include "TextEditor.h"
#include ".\texteditor.h"


#define MyR(rgb) ((BYTE)(rgb))
#define MyG(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define MyB(rgb) ((BYTE)((rgb) >> 16))

// CTextEditor
static InitGDIPlus GDI_Plus_Controler;


// CTextEditor
/////////////////////////////////////////////////////////////////////////////
LRESULT CTextEditor::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	GDI_Plus_Controler.Initialize(); //GDI_Plus_Controler is a static global

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_CurrPrvBkColor = new Color(255, 190, 200, 200);
	m_pDraw = new CStringDraw();
	m_pDraw->Init(this->m_hWnd);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTextEditor::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_CurrPrvBkColor)
		delete m_CurrPrvBkColor;

	if (m_pDraw)
		delete m_pDraw;

	m_CurrPrvBkColor = NULL;
	m_pDraw = NULL;

	GDI_Plus_Controler.Deinitialize(); //GDI_Plus_Controler is a static global

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTextEditor::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	RECT Rect;
	GetClientRect(&Rect);
	RectF rectF((float)Rect.left, (float)Rect.top, (float)Rect.right-Rect.left, (float)Rect.bottom-Rect.top);

	Graphics g(hDC);

	SolidBrush dlgBrush(*m_CurrPrvBkColor); //Color(255, 190, 200, 200));
	g.FillRectangle(&dlgBrush, rectF);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CTextEditor::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;

	HDC hPaintDC = ::BeginPaint(m_hWnd, &ps);
	if (!hPaintDC)
		return E_FAIL;

	// draw the outer border here
	CRect Rect;
	GetClientRect(Rect);

	HBRUSH hbr = ::CreateSolidBrush(RGB(0,0,0));
	::FrameRect(hPaintDC, Rect, hbr);

	m_pDraw->DoDraw(hPaintDC);

	::EndPaint(m_hWnd, &ps);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety
HRESULT CTextEditor::GetInterfaceSafetyOptions(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions)
{
	if (!pdwSupportedOptions || !pdwEnabledOptions)
		return E_POINTER;

	HRESULT hr = S_OK;
	if (riid == IID_IDispatch)
	{
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
		*pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
	}
	else
	{
		*pdwSupportedOptions = 0;
		*pdwEnabledOptions = 0;
		hr = E_NOINTERFACE;
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety
HRESULT CTextEditor::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IViewObjectEx
//HRESULT CTextEditor::GetViewStatus(DWORD* pdwStatus)
//{
//	ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
//	*pdwStatus = 0;
//	return S_OK;
//}
//

STDMETHODIMP CTextEditor::IncrementLineHeight(void)
{
	// TODO: Add your implementation code here
	m_pDraw->IncrementLineHeight();
	return S_OK;
}

STDMETHODIMP CTextEditor::DecrementLineHeight(void)
{
	// TODO: Add your implementation code here
	m_pDraw->DecrementLineHeight();
	return S_OK;
}

STDMETHODIMP CTextEditor::IncrementCharWidth(void)
{
	// TODO: Add your implementation code here
	m_pDraw->IncrementCharWidth();
	return S_OK;
}

STDMETHODIMP CTextEditor::DecrementCharWidth(void)
{
	// TODO: Add your implementation code here
	m_pDraw->DecrementCharWidth();
	return S_OK;
}

STDMETHODIMP CTextEditor::SetFontSize(USHORT FontSize)
{
	// TODO: Add your implementation code here
	m_pDraw->SetFontSize(FontSize);
	return S_OK;
}

STDMETHODIMP CTextEditor::SetCurveType(USHORT CurveType)
{
	// TODO: Add your implementation code here
	if ((CurveType >= STR_BEZIER) && 
		(CurveType <= STR_SPIRAL))
		m_pDraw->SetCurveType((STR_TYPES)CurveType);
	return S_OK;
}

STDMETHODIMP CTextEditor::SetImageSize(FLOAT Width, FLOAT Height)
{
	// TODO: Add your implementation code here
	SizeF size(Width, Height);
	m_pDraw->SetImageSize(size);
	return S_OK;
}

STDMETHODIMP CTextEditor::SetString(BSTR LineString, USHORT Index)
{
	// TODO: Add your implementation code here
	m_pDraw->SetString(LineString, Index);
	return S_OK;
}

STDMETHODIMP CTextEditor::SaveImage(BSTR FileName, USHORT ImageType)
{
	// TODO: Add your implementation code here
	if ((ImageType >= BMP) && (ImageType <= PNG))
		m_pDraw->SaveImage(FileName, (IMAGE_TYPES)ImageType);
	return S_OK;
}

STDMETHODIMP CTextEditor::RefreshPreviewWindow(void)
{
	// TODO: Add your implementation code here
	m_pDraw->RefreshPreviewWindow();
	return S_OK;
}

STDMETHODIMP CTextEditor::SetColor1(LONG Color1)
{
	// TODO: Add your implementation code here
	Color *BkColor = new Color(255, MyR(Color1), MyG(Color1), MyB(Color1));
	m_pDraw->SetColor1(*BkColor);
	delete BkColor;

	return S_OK;
}

STDMETHODIMP CTextEditor::SetColor2(LONG Color2)
{
	// TODO: Add your implementation code here
	Color *BkColor = new Color(255, MyR(Color2), MyG(Color2), MyB(Color2));
	m_pDraw->SetColor2(*BkColor);
	delete BkColor;

	return S_OK;
}

STDMETHODIMP CTextEditor::SetPreviewBkColor(LONG Clr)
{
	// TODO: Add your implementation code here
	if (m_CurrPrvBkColor)
		delete m_CurrPrvBkColor;
	m_CurrPrvBkColor = NULL;
	m_CurrPrvBkColor = new Color(255, MyR(Clr), MyG(Clr), MyB(Clr));

	return S_OK;
}

STDMETHODIMP CTextEditor::DeleteString(USHORT Index)
{
	// TODO: Add your implementation code here
	m_pDraw->DeleteString(Index);
	return S_OK;
}

STDMETHODIMP CTextEditor::SetImageBkColor(LONG Clr)
{
	// TODO: Add your implementation code here
	Color *BkColor = new Color(255, MyR(Clr), MyG(Clr), MyB(Clr));
	m_pDraw->SetImageBkColor(*BkColor);
	delete BkColor;

	return S_OK;
}

STDMETHODIMP CTextEditor::SetColorGradientMode(USHORT SweepMode)
{
	// TODO: Add your implementation code here
	m_pDraw->SetColorGradientMode((LINEAR_MODES)SweepMode);
	return S_OK;
}

STDMETHODIMP CTextEditor::SetFont(BSTR FontName)
{
	// TODO: Add your implementation code here
	m_pDraw->SetFont(FontName);
	return S_OK;
}

STDMETHODIMP CTextEditor::SetWhiteBkgdTransparent(VARIANT_BOOL bkgdTransparent)
{
	// TODO: Add your implementation code here
	bool btrans = (bkgdTransparent == VARIANT_TRUE);
	m_pDraw->SetWhiteBkgdTransparent(btrans);
	return S_OK;
}
