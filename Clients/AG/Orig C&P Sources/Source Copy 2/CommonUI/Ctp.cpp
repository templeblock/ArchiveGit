//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "Ctp.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGDC.h"
#include "Font.h"

#include <strstrea.h>
#include <scselect.h>

#define TIMER_TEXT	1

static HHOOK g_hHook=NULL;
static CCtp *g_pThis;


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CALLBACK CtpKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || !g_pThis->HasFocus())
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);

	switch (wParam)
	{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		{
			int bHandled;
			if (lParam & 0x80000000)
				g_pThis->OnKeyUp(0, wParam, 0, bHandled);
			else
				g_pThis->OnKeyDown(0, wParam, 0, bHandled);
			return (1);
			break;
		}

		case VK_ESCAPE:
		case VK_TAB:
		case VK_BACK:
		{
			int bHandled;
			if ((lParam & 0x80000000) == 0)
				g_pThis->OnChar(0, wParam, 0, bHandled);
			return (1);
			break;
		}
	}

	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::CreateBackPage()
{
	CAGPage *pAGPage = m_pAGDoc->GetPage(4);
	SIZE sizePage;
	pAGPage->GetPageSize(&sizePage);

	if (pAGPage->GetNumLayers() != 2)
	{
		CAGLayer *pAGLayer = new CAGLayer();
		pAGPage->AppendLayer(pAGLayer);
	}

	CAGSymImage *pAGLogo = GetImage(IDR_AGLOGO);
	CAGSymImage *pCPLogo = GetImage(IDR_CPLOGO);

	RECT AGLogoRect; 
	RECT ImageRect = pAGLogo->GetDestRect();
	AGLogoRect.left = (sizePage.cx - WIDTH(ImageRect)) / 2;
	AGLogoRect.top = (sizePage.cy - HEIGHT(ImageRect)) / 2;
	AGLogoRect.right = AGLogoRect.left + WIDTH(ImageRect);
	AGLogoRect.bottom = AGLogoRect.top + HEIGHT(ImageRect);

	RECT CPLogoRect;
	ImageRect = pCPLogo->GetDestRect();
	CPLogoRect.left = (APP_RESOLUTION / 16);
	CPLogoRect.bottom = sizePage.cy -(APP_RESOLUTION / 16);
	CPLogoRect.top = CPLogoRect.bottom - HEIGHT(ImageRect);
	CPLogoRect.right = CPLogoRect.left + WIDTH(ImageRect);

	char szMsg[] = "Created\njust for you\nby\nsender's name";
	char szCopyright[] = "©AGC, Inc.";

	RECT MsgRect;
	RECT CopyrightRect;
	CopyrightRect.left = sizePage.cx -((APP_RESOLUTION * 15) / 10);
	CopyrightRect.top = sizePage.cy -(APP_RESOLUTION / 2);
	CopyrightRect.right = sizePage.cx -(APP_RESOLUTION / 16);
	CopyrightRect.bottom = sizePage.cy -(APP_RESOLUTION / 16);

	LOGFONT MsgFont;
	memset(&MsgFont, 0, sizeof(MsgFont));
	MsgFont.lfWeight = 400;
	lstrcpy(MsgFont.lfFaceName, "CAC Futura Casual");

	LOGFONT CopyrightFont;
	memset(&CopyrightFont, 0, sizeof(CopyrightFont));
	CopyrightFont.lfWeight = 400;
	lstrcpy(CopyrightFont.lfFaceName, "Arial");

	switch (m_pAGDoc->GetDocType())
	{
		case DOC_CARD_SINGLEFOLD_PORTRAIT:
		{
			MsgFont.lfHeight = -(14 * APP_RESOLUTION / 72);
			MsgRect.left = APP_RESOLUTION / 4;
			MsgRect.top = APP_RESOLUTION / 2;
			MsgRect.right = sizePage.cx -(APP_RESOLUTION / 4);
			MsgRect.bottom = APP_RESOLUTION * 2;
			
			CopyrightFont.lfHeight = -(12 * APP_RESOLUTION / 72);
			break;
		}

		case DOC_CARD_SINGLEFOLD_LANDSCAPE:
		{
			MsgFont.lfHeight = -(14 * APP_RESOLUTION / 72);
			MsgRect.left = APP_RESOLUTION / 4;
			MsgRect.top = APP_RESOLUTION / 4;
			MsgRect.right = sizePage.cx -(APP_RESOLUTION / 4);
			MsgRect.bottom = (APP_RESOLUTION * 175) / 100;

			CopyrightFont.lfHeight = -(12 * APP_RESOLUTION / 72);

			AGLogoRect.top += (APP_RESOLUTION / 2);
			AGLogoRect.bottom += (APP_RESOLUTION / 2);
			break;
		}

		case DOC_CARD_QUARTERFOLD_PORTRAIT:
		{
			MsgFont.lfHeight = -(11 * APP_RESOLUTION / 72);
			MsgRect.left = APP_RESOLUTION / 8;
			MsgRect.top = APP_RESOLUTION / 4; 
			MsgRect.right = sizePage.cx -(APP_RESOLUTION / 8);
			MsgRect.bottom = (APP_RESOLUTION * 15) / 10;  

			CopyrightFont.lfHeight = -(8 * APP_RESOLUTION / 72);

			AGLogoRect.top += (APP_RESOLUTION / 8);
			AGLogoRect.bottom += (APP_RESOLUTION / 8);

			CAGMatrix Matrix;
			Matrix.Scale(.75, .75,
						((AGLogoRect.left + AGLogoRect.right) / 2),
						((AGLogoRect.top + AGLogoRect.bottom) / 2));
			pAGLogo->SetMatrix(Matrix);

			Matrix.Unity();
			Matrix.Scale(.70, .70, CPLogoRect.left, CPLogoRect.bottom);
			pCPLogo->SetMatrix(Matrix);
			break;
		}

		case DOC_CARD_QUARTERFOLD_LANDSCAPE:
		{
			MsgFont.lfHeight = -(11 * APP_RESOLUTION / 72);
			MsgRect.left = APP_RESOLUTION / 8;
			MsgRect.top = APP_RESOLUTION / 16; 
			MsgRect.right = sizePage.cx -(APP_RESOLUTION / 8);
			MsgRect.bottom = (APP_RESOLUTION * 125) / 100;	
			
			CopyrightFont.lfHeight = -(8 * APP_RESOLUTION / 72);

			AGLogoRect.top += (APP_RESOLUTION / 5);
			AGLogoRect.bottom += (APP_RESOLUTION / 5);

			CAGMatrix Matrix;
			Matrix.Scale(.75, .75,
						((AGLogoRect.left + AGLogoRect.right) / 2),
						((AGLogoRect.top + AGLogoRect.bottom) / 2));
			pAGLogo->SetMatrix(Matrix);

			Matrix.Unity();
			Matrix.Scale(.70, .70, CPLogoRect.left, CPLogoRect.bottom);
			pCPLogo->SetMatrix(Matrix);
			break;
		}
	}

	CAGSpec MsgSpec(MsgFont, RGB(0, 0, 0), eRagCentered);
	CAGSpec CopyrightSpec(CopyrightFont, RGB(0, 0, 0), eRagLeft);

	int nSpecOffset = 0;

	CAGSymText *pAGSymText = new CAGSymText();
	pAGSymText->Create(MsgRect);
	pAGSymText->SetVertJust(eVertCentered);
	pAGSymText->SetText(szMsg, lstrlen(szMsg), 1, &MsgSpec, &nSpecOffset);

	CAGLayer *pAGLayer = pAGPage->GetLayer(2);
	pAGLayer->AppendSymbol(pAGSymText);

	pAGLayer = pAGPage->GetLayer(1);

	pAGLogo->SetDestRect(AGLogoRect);
	pAGLayer->AppendSymbol(pAGLogo);

	pCPLogo->SetDestRect(CPLogoRect);
	pAGLayer->AppendSymbol(pCPLogo);

	pAGSymText = new CAGSymText();
	pAGSymText->Create(CopyrightRect);
	pAGSymText->SetVertJust(eVertBottom);
	pAGSymText->SetText(szCopyright, lstrlen(szCopyright), 1, &CopyrightSpec,
						&nSpecOffset);
	pAGLayer->AppendSymbol(pAGSymText);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::DrawEditRect(CAGDC *pDC)
{
	if (m_pText)
	{
		RECT DestRect = m_pText->GetDestRect();
		RECT Offset = {-1, -1, 1, 1};
		pDC->DPAtoLPA(&Offset);
		DestRect.left += Offset.left;
		DestRect.right += Offset.right;
		DestRect.top += Offset.top;
		DestRect.bottom += Offset.bottom;

		POINT Pts[5];
		Pts[0].x = Pts[1].x = Pts[4].x = DestRect.left;
		Pts[2].x = Pts[3].x = DestRect.right;
		Pts[0].y = Pts[3].y = Pts[4].y = DestRect.top;
		Pts[1].y = Pts[2].y = DestRect.bottom;
		pDC->LPtoDP(Pts, 5);

		int PrevROP = ::SetROP2(pDC->GetHDC(), R2_NOTXORPEN);
		HBRUSH hOldBrush = (HBRUSH) ::SelectObject(pDC->GetHDC(),
												::GetStockObject(NULL_BRUSH));
		HPEN hOldPen = (HPEN) ::SelectObject(pDC->GetHDC(),
												::GetStockObject(BLACK_PEN));
		::Polyline(pDC->GetHDC(), Pts, 5);
		::SelectObject(pDC->GetHDC(), hOldBrush);
		::SelectObject(pDC->GetHDC(), hOldPen);
		::SetROP2(pDC->GetHDC(), PrevROP);
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::FileData(BYTE *pBytes, DWORD dwLen)
{
	if (m_pDownloadData == NULL)
		m_pDownloadData = (BYTE *)malloc(dwLen);
	else
		m_pDownloadData = (BYTE *)realloc(m_pDownloadData, m_dwDownloadSize + dwLen);

	memcpy(m_pDownloadData + m_dwDownloadSize, pBytes, dwLen);
	m_dwDownloadSize += dwLen;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::FileEnd()
{
	if (m_pAGDoc)
	{
		delete m_pAGDoc;
		m_pAGDoc = NULL;
	}

	istrstream input((char *)m_pDownloadData, m_dwDownloadSize);

	m_pAGDoc = new CAGDoc();
	if (! m_pAGDoc->Read(input))
	{
		delete m_pAGDoc;
		m_pAGDoc = NULL;
		free(m_pDownloadData);
		m_pDownloadData = NULL;
		m_dwDownloadSize = 0;
	}
	else
	{
		LOGFONTARRAY m_DocFonts;
		m_pAGDoc->GetFonts(m_DocFonts);
		m_FontList.CheckFonts(m_DocFonts, m_FontDownloadArray);
		if (m_FontDownloadArray.empty())
		{
			CreateBackPage();
			free(m_pDownloadData);
			m_pDownloadData = NULL;
			m_dwDownloadSize = 0;
		}
		else
		{
			delete m_pAGDoc;
			m_pAGDoc = NULL;

#if 0
			int nDownload = m_FontDownloadArray.size();
			for (int i = 0; i < nDownload; i++)
			{
				StartDownloadFont(m_FontDownloadArray[i].szFontFile);
			}
#else
			if (! m_FontDownloadArray.empty())
				StartDownloadFont(m_FontDownloadArray[0].szFontFile);
#endif
		}
	}

	if (m_hWnd && m_pAGDoc)
	{
		m_pCtlPanel->SetDoc(m_pAGDoc);
		m_pCtlPanel->ShowWindow(SW_SHOW);
		NewPage();
		::InvalidateRect(GetParent(), NULL, TRUE);
		Invalidate();
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::FileStart()
{
	if (m_pDownloadData)
		free(m_pDownloadData);
	m_pDownloadData = NULL;
	m_dwDownloadSize = 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::FontData(const char *pszFontFile, BYTE *pBytes, DWORD dwLen)
{
	int nDownload = m_FontDownloadArray.size();
	for (int i = 0; i < nDownload; i++)
	{
		if (lstrcmpi(pszFontFile, m_FontDownloadArray[i].szFontFile) == 0)
		{
			if (m_FontDownloadArray[i].pDownloadData == NULL)
				m_FontDownloadArray[i].pDownloadData = (BYTE *)malloc(dwLen);
			else
			{
				m_FontDownloadArray[i].pDownloadData = (BYTE *)realloc(
											m_FontDownloadArray[i].pDownloadData,
											m_FontDownloadArray[i].dwDownloadSize + dwLen);
			}

			memcpy(m_FontDownloadArray[i].pDownloadData +
					m_FontDownloadArray[i].dwDownloadSize, pBytes, dwLen);
			m_FontDownloadArray[i].dwDownloadSize += dwLen;
			break;
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::FontEnd(const char *pszFontFile)
{
	int nDownload = m_FontDownloadArray.size();
	for (int i = 0; i < nDownload; i++)
	{
		if (lstrcmpi(pszFontFile, m_FontDownloadArray[i].szFontFile) == 0)
		{
			if (m_FontDownloadArray[i].pDownloadData)
			{
				m_FontList.InstallFont(m_FontDownloadArray[i].pDownloadData,
				  m_FontDownloadArray[i].dwDownloadSize,
				  m_FontDownloadArray[i].szFullName, pszFontFile);

				free(m_FontDownloadArray[i].pDownloadData);
			}
			m_FontDownloadArray[i].pDownloadData = NULL;
			m_FontDownloadArray[i].dwDownloadSize = 0;

			m_FontDownloadArray.erase(m_FontDownloadArray.begin() + i);

			if (m_FontDownloadArray.empty() && m_pDownloadData)
			{
				m_FontList.InitFontArray();
				if (m_pCtlPanel)
					m_pCtlPanel->SetFonts();

				istrstream input((char *)m_pDownloadData, m_dwDownloadSize);
				m_pAGDoc = new CAGDoc();
				if (! m_pAGDoc->Read(input))
				{
					delete m_pAGDoc;
					m_pAGDoc = NULL;
					free(m_pDownloadData);
					m_pDownloadData = NULL;
					m_dwDownloadSize = 0;
				}
				else
				{
					CreateBackPage();
					free(m_pDownloadData);
					m_pDownloadData = NULL;
					m_dwDownloadSize = 0;
				}

				if (m_hWnd && m_pAGDoc)
				{
					m_pCtlPanel->SetDoc(m_pAGDoc);
					m_pCtlPanel->ShowWindow(SW_SHOW);
					NewPage();
					::InvalidateRect(GetParent(), NULL, TRUE);
					Invalidate();
				}
			}
#if 1
			else if (! m_FontDownloadArray.empty())
			{
				StartDownloadFont(m_FontDownloadArray[0].szFontFile);
			}
#endif
			break;
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::FontStart(const char *pszFontFile)
{
	int nDownload = m_FontDownloadArray.size();
	for (int i = 0; i < nDownload; i++)
	{
		if (lstrcmpi(pszFontFile, m_FontDownloadArray[i].szFontFile) == 0)
		{
			if (m_FontDownloadArray[i].pDownloadData)
				free(m_FontDownloadArray[i].pDownloadData);
			m_FontDownloadArray[i].pDownloadData = NULL;
			m_FontDownloadArray[i].dwDownloadSize = 0;
			break;
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSymImage *CCtp::GetImage(int nID)
{
	CAGSymImage *pAGSymImage = NULL;
	HGLOBAL 	hSymImage = NULL;
	HRSRC		hResource;

	if ((hResource = ::FindResource(_Module.GetResourceInstance(),
		MAKEINTRESOURCE(nID), "AGIMAGE")) != NULL)
	{
		hSymImage = ::LoadResource(_Module.GetResourceInstance(), hResource);
	}

	if (hSymImage)
	{
		istrstream input((char *)::LockResource(hSymImage),
		  ::SizeofResource(_Module.GetResourceInstance(), hResource));
		CAGDocIO DocIO(&input);

		pAGSymImage = new CAGSymImage();
		if (! pAGSymImage->Read(&DocIO))
		{
			delete pAGSymImage;
			pAGSymImage = NULL;
		}

		DocIO.Close();
	}
	return (pAGSymImage);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::NewPage()
{
	if (m_pText && m_pText->IsEditing())
		StopEdit();

	RECT RepaintRect;
	::UnionRect(&RepaintRect, &m_PageRect, &m_ShadowRect);

	SIZE sizeShadow = {(APP_RESOLUTION * 4) / 100,
						(APP_RESOLUTION * 4) / 100 };

	RECT WndRect, DlgRect;
	GetClientRect(&WndRect);
	m_pCtlPanel->GetClientRect(&DlgRect);
	WndRect.left += 10;
	WndRect.top += 2;
	WndRect.right -= (WIDTH(DlgRect) + 10);
	WndRect.bottom -= 2;

	m_pClientDC->DPtoVP(&WndRect);
	WndRect.right -= sizeShadow.cx;
	WndRect.bottom -= sizeShadow.cy;

	SIZE sizePage;
	m_pAGDoc->GetCurrentPage()->GetPageSize(&sizePage);

	::SetRect(&m_PageRect, 0, 0, sizePage.cx, sizePage.cy);
	m_ViewMatrix.ScaleAndCenter(WndRect, m_PageRect);

	m_ViewMatrix.m_31 = WndRect.left;
	m_ViewMatrix.m_32 = WndRect.top;

	m_pClientDC->SetViewingMatrix(m_ViewMatrix);
	m_pClientDC->MPtoDP(&m_PageRect);
	
	m_pCtlPanel->SetWindowPos(NULL, m_PageRect.right + 10, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);


	m_ShadowRect = m_PageRect;
	m_pClientDC->VPAtoDPA((POINT *)&sizeShadow, 1);
	::OffsetRect(&m_ShadowRect, sizeShadow.cx, sizeShadow.cy);

	CAGMatrix TempMatrix((double)WIDTH(m_PageRect) /(double)sizePage.cx,
						0, 0, (double)HEIGHT(m_PageRect) /(double)sizePage.cy, 1, 1);


	::InflateRect(&m_PageRect, 1, 1);
	::UnionRect(&RepaintRect, &RepaintRect, &m_PageRect);
	::UnionRect(&RepaintRect, &RepaintRect, &m_ShadowRect);

	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	
	HDC hMemDC = ::CreateCompatibleDC(m_pClientDC->GetHDC());
	m_hBitmap = ::CreateCompatibleBitmap(m_pClientDC->GetHDC(), WIDTH(m_PageRect), HEIGHT(m_PageRect));
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, m_hBitmap);

	HBRUSH hbrOld = (HBRUSH)::SelectObject(hMemDC, GetStockObject(WHITE_BRUSH));
	HPEN hpenOld = (HPEN)::SelectObject(hMemDC, GetStockObject(BLACK_PEN));
	::Rectangle(hMemDC, 0, 0, WIDTH(m_PageRect), HEIGHT(m_PageRect));
	::SelectObject(hMemDC, hbrOld);
	::SelectObject(hMemDC, hpenOld);

	CAGDC dc(hMemDC);
	dc.SetDeviceMatrix(TempMatrix);
	m_pAGDoc->GetCurrentPage()->GetLayer(1)->Draw(dc);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);


	InvalidateRect(&RepaintRect);
	UpdateWindow();

	CAGLayer *pLayer = m_pAGDoc->GetCurrentPage()->GetLayer(2);
	CAGSymText *pText = (CAGSymText *)pLayer->FindFirstSymbolByType(ST_TEXT);
	if (pText)
	{

		const RECT &DestRect = pText->GetDestRect();
		POINT pt = {DestRect.left, DestRect.top};
		StartEdit(pText, pt, false);
		SetFocus();
	}

	m_pCtlPanel->UpdateControls(m_pText);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
						BOOL & /*bHandled*/)
{
	DWORD ClassStyle = ::GetClassLong(m_hWnd, GCL_STYLE);
	::SetClassLong(m_hWnd, GCL_STYLE, ClassStyle | CS_DBLCLKS);

	m_pClientDC = new CAGClientDC(m_hWnd);
	m_pCtlPanel = new CCtlPanel(this);
	m_pCtlPanel->Create(m_hWnd);

	if (m_pAGDoc)
	{
		NewPage();
		m_pCtlPanel->SetDoc(m_pAGDoc);
		m_pCtlPanel->ShowWindow(SW_SHOW);
	}

	g_hHook = ::SetWindowsHookEx(WH_KEYBOARD,
	  reinterpret_cast<HOOKPROC> (CtpKeyboardProc), NULL, GetCurrentThreadId());
	g_pThis = this;

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
						 BOOL & /*bHandled*/)
{
	::UnhookWindowsHookEx(g_hHook);

	if (m_pCtlPanel)
	{
		m_pCtlPanel->DestroyWindow();
		delete m_pCtlPanel;
		m_pCtlPanel = NULL;
	}
	if (m_pAGDoc)
	{
		delete m_pAGDoc;
		m_pAGDoc = NULL;
	}
	if (m_pClientDC)
	{
		delete m_pClientDC;
		m_pClientDC = NULL;
	}

	if (m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
HRESULT CCtp::OnDraw(ATL_DRAWINFO &di)
{
	BOOL bUserMode;
	GetAmbientUserMode(bUserMode);
	if (! bUserMode)
	{
		RECT &rc = *(RECT*)di.prcBounds;

		::FillRect(di.hdcDraw, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		BSTR bstr;
		if (SUCCEEDED(GetAmbientDisplayName(bstr)))
		{
			USES_CONVERSION;
			::DrawText(di.hdcDraw, OLE2A(bstr), -1, &rc, DT_TOP | DT_SINGLELINE);
		}
	}
	else if (m_pAGDoc)
	{
		CAGDC dc(di.hdcDraw);
		RECT r = m_ShadowRect;
		r.left = m_PageRect.right;
		::FillRect(di.hdcDraw, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
		r.left = m_ShadowRect.left;
		r.top = m_PageRect.bottom;
		::FillRect(di.hdcDraw, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));

			
		HDC hMemDC = ::CreateCompatibleDC(di.hdcDraw);
		CAGDC dcTemp(hMemDC);
		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hMemDC, m_hBitmap);
		::BitBlt(di.hdcDraw, m_PageRect.left, m_PageRect.top,
					WIDTH(m_PageRect), HEIGHT(m_PageRect), hMemDC, 0, 0, SRCCOPY);
		::SelectObject(hMemDC, hOldBitmap);
		::DeleteDC(hMemDC);

		dc.SetViewingMatrix(m_ViewMatrix);
		m_pAGDoc->GetCurrentPage()->GetLayer(2)->Draw(dc);


		if (m_pText && m_pText->IsEditing())
		{
			dc.PushModelingMatrix(m_pText->GetMatrix());
			m_pText->DrawSelection(dc);
			DrawEditRect(&dc);
			dc.PopModelingMatrix();
		}

	}
	else
	{
		RECT &rc = *(RECT*)di.prcBounds;

		::SetTextColor(di.hdcDraw, RGB(0, 0, 0));
		::SetBkMode(di.hdcDraw, TRANSPARENT);

		RECT rp;
		HWND hParent = GetParent();
		::GetClientRect(hParent, &rp);
		::MapWindowPoints(hParent, m_hWnd, (POINT *)&rp, 2);
		RECT r;
		::IntersectRect(&r, &rc, &rp);

		::DrawText(di.hdcDraw, "Preparing to edit card.  Please Wait...", -1,
					&r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	return S_OK;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/,
						BOOL & /*bHandled*/)
{
	UINT nChar = wParam;

	if (m_pText && m_pText->IsEditing())
	{
		if (nChar == VK_ESCAPE)
			StopEdit();
		else
			m_pText->OnChar(nChar);
	}

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/,
						BOOL & /*bHandled*/)
{
	UINT nChar = wParam;

	if (m_pText && m_pText->IsEditing())
	{
		m_pText->OnKeyDown(nChar);
		m_pCtlPanel->UpdateControls(m_pText);
	}

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/,
						BOOL & /*bHandled*/)
{
	UINT nChar = wParam;

	if (m_pText && m_pText->IsEditing())
		m_pText->OnKeyUp(nChar);

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam,
								BOOL & /*bHandled*/)
{
	if (m_pText && m_pText->IsEditing())
	{
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		m_pClientDC->DPtoLP(&pt, 1);
		m_pText->OnLButtonDblClk(pt);
		m_pCtlPanel->UpdateControls(m_pText);
	}
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam,
							BOOL & /*bHandled*/)
{
	if (m_pAGDoc)
	{
		SetCapture();

		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		m_pClientDC->DPtoMP(&pt, 1);
		CAGSym *pSym = m_pAGDoc->GetCurrentPage()->GetLayer(2)->FindSymbolByPoint(pt, ST_TEXT);
		if (pSym)
		{
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			m_pClientDC->DPtoLP(&pt, 1);

			if (pSym != m_pText)
			{
				if (m_pText && m_pText->IsEditing())
					StopEdit();

				StartEdit((CAGSymText *)pSym, pt, true);
			}
			else if (m_pText->IsEditing())
				m_pText->OnLButtonDown(pt, (wParam & MK_SHIFT) != 0);

			SetFocus();
			m_pCtlPanel->UpdateControls(m_pText);
		}
		else if (m_pText)
		{
			if (m_pText->IsEditing())
				StopEdit();
			m_pText = NULL;
		}
	}
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam,
							BOOL & /*bHandled*/)
{
	if (m_pText && m_pText->IsEditing())
	{
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		m_pClientDC->DPtoLP(&pt, 1);
		m_pText->OnLButtonUp(pt);
		if (! m_pText->GetSelection()->IsSliverCursor())
			m_pCtlPanel->UpdateControls(m_pText);
	}

	ReleaseCapture();

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam,
							BOOL & /*bHandled*/)
{
	if (m_pText && m_pText->IsEditing())
	{
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		m_pClientDC->DPtoLP(&pt, 1);
		m_pText->OnMouseMove(pt);
	}
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /* lParam */,
							BOOL & /*bHandled*/)
{
	if (m_pText && m_pText->IsEditing())
	{
		KillTimer(TIMER_TEXT);
		
		if (m_pText->GetSelection()->IsSliverCursor())
			m_pText->ShowSelection(false);
	}
	m_bHasFocus = false;

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /* lParam */,
							BOOL & /*bHandled*/)
{
	if (m_pText && m_pText->IsEditing())
	{
		if (m_pText->GetSelection()->IsSliverCursor())
			m_pText->ShowSelection(true);
//		SetTimer(TIMER_TEXT, 500, NULL);
		SetTimer(TIMER_TEXT, 500);
		m_bHasFocus = true;
	}

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
LRESULT CCtp::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /* lParam */,
						BOOL & /*bHandled*/)
{
	if (wParam == TIMER_TEXT)
	{
		if (m_pText && m_pText->IsEditing())
			m_pText->BlinkCursor();
		else
			KillTimer(TIMER_TEXT);
	}

	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::StartEdit(CAGSymText *pText, POINT Pt, bool bClick)
{
	m_pText = pText;

	m_pClientDC->PushModelingMatrix(m_pText->GetMatrix());

	m_pText->Edit(m_pClientDC, Pt.x, Pt.y, bClick);
	DrawEditRect(m_pClientDC);
//	SetTimer(TIMER_TEXT, 500, NULL);
	SetTimer(TIMER_TEXT, 500);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CCtp::StopEdit()
{
	if (m_pText && m_pText->IsEditing())
	{
		KillTimer(TIMER_TEXT);
		m_pText->EndEdit();
		DrawEditRect(m_pClientDC);
		m_pClientDC->PopModelingMatrix();
	}
	m_pText = NULL;
	m_pCtlPanel->UpdateControls(NULL);
}

