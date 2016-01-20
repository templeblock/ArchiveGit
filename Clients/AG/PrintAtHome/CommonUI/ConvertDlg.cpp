#include "stdafx.h"
#include "Ctp.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "ConvertDlg.h"
#include "PaperTemplates.h"
#include "AGLayer.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define ADDCALENDAR

//////////////////////////////////////////////////////////////////////
CConvertDlg::CConvertDlg(CCtp* pCtp, CAGDoc* pAGDoc)
{
	m_pCtp = pCtp;
	m_pAGDoc = pAGDoc;
	m_fPageWidth = 0.0;
	m_fPageHeight = 0.0;
	m_iPaperType = 0;
	m_fPageWidthOrig = 0;
	m_fPageHeightOrig = 0;
	m_iPaperTypeOrig = 0;
	::SetRectEmpty(&m_SampleRect);
}

//////////////////////////////////////////////////////////////////////
CConvertDlg::~CConvertDlg()
{
}

#ifdef THEMES
	#include <uxtheme.h>
	typedef BOOL (STDAPICALLTYPE *PFNIsThemeActive)();
	typedef BOOL (STDAPICALLTYPE *PFNIsAppThemed)();
#endif THEMES

//////////////////////////////////////////////////////////////////////
LRESULT CConvertDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

#ifdef THEMES
	HMODULE hThemeLib = ::LoadLibrary("uxtheme.dll");
	if (hThemeLib)
	{
		PFNIsThemeActive pIsThemeActive = (PFNIsThemeActive)::GetProcAddress(hThemeLib, "IsThemeActive");
		bool bIsThemeActive = (pIsThemeActive ? !!pIsThemeActive() : false);

		PFNIsAppThemed pIsAppThemed = (PFNIsAppThemed)::GetProcAddress(hThemeLib, "IsAppThemed");
		bool bIsAppThemed = (pIsAppThemed ? !!pIsAppThemed() : false);

		//static HTHEME htheme = NULL;
		//htheme = OpenThemeData(m_hWnd, L"StartPanel");
		//if (htheme)
		//{
		//	// get the background region for the part we are going to
		//	// paint the container window with and apply it to the dialog.
		//	RECT rc;
		//	::GetWindowRect(m_hWnd, &rc);
		//	::OffsetRect(&rc, -rc.left, -rc.top);

		//	HRGN hrgn = NULL;
		//	if (SUCCEEDED(GetThemeBackgroundRegion(htheme, NULL, WP_DIALOG, 0, &rc, &hrgn)))
		//		::SetWindowRgn(m_hWnd, hrgn, FALSE);
		//}
		//// Visual styles are active, paint using the visual style APIs
		//RECT rc;
		//GetWindowRect(hwnd, &rc);
		//OffsetRect(&rc, -rc.left, -rc.top);
		//DrawThemeBackground(htheme, hdc, SPP_USERPANE, 0, &rc, NULL);

		::FreeLibrary(hThemeLib);
	}
#endif THEMES

	SIZE PageSize = {0,0};
	CAGPage* pPage = m_pAGDoc->GetPage(0);
	if (pPage)
		pPage->GetPageSize(PageSize);
	m_fPageWidth = DINCHES(PageSize.cx);
	m_fPageHeight = DINCHES(PageSize.cy);
	m_fPageWidthOrig = m_fPageWidth;
	m_fPageHeightOrig = m_fPageHeight;

	CString strDocTypeName = CAGDocTypes::Name(m_pAGDoc->GetDocType());
	SetDlgItemText(IDC_DOCTYPE, strDocTypeName);

	CPaperTemplates::StuffCombo(m_pAGDoc->GetDocType(), 0, NULL, GetDlgItem(IDC_PAPERTYPE), 0, 0);

	CString strValue;

	strValue.Format("%0.5G", m_fPageWidth);
	SetDlgItemText(IDC_WIDTH, strValue);

	strValue.Format("%0.5G", m_fPageHeight);
	SetDlgItemText(IDC_HEIGHT, strValue);

	UpdateSample(IDC_SAMPLE);

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CConvertDlg::OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode == CBN_SELCHANGE)
	{
		m_iPaperType = SendDlgItemMessage(IDC_PAPERTYPE, CB_GETCURSEL, 0, 0);
		const PaperTemplate* pPaper = NULL;
		int nTemplates = CPaperTemplates::GetTemplate(m_pAGDoc->GetDocType(), m_iPaperType, &pPaper);
		if (pPaper)
		{
			bool bPortrait = (m_fPageWidthOrig <= m_fPageHeightOrig);
			bool bPaperPortrait = (pPaper->fWidth <= pPaper->fHeight);

			int iPaperWidth = 0;
			int iPaperHeight = 0;
			if (bPaperPortrait == bPortrait)
			{
				m_fPageWidth = pPaper->fWidth;
				m_fPageHeight = pPaper->fHeight;
			}
			else
			{
				m_fPageWidth = pPaper->fHeight;
				m_fPageHeight = pPaper->fWidth;
			}
		}

		CString strValue;

		strValue.Format("%0.5G", m_fPageWidth);
		SetDlgItemText(IDC_WIDTH, strValue);

		strValue.Format("%0.5G", m_fPageHeight);
		SetDlgItemText(IDC_HEIGHT, strValue);

		UpdateSample(IDC_SAMPLE);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CConvertDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bool bModified = false;
	if (m_iPaperTypeOrig  != m_iPaperType ||
		m_fPageHeightOrig != m_fPageHeight ||
		m_fPageWidthOrig  != m_fPageWidth)
	{
		bModified = true;
	}

	if (bModified)
	{
		if (CMessageBox::Message("Are you sure you want to convert the project?", MB_YESNO) == IDNO)
			return S_OK;

		SIZE NewPageSize = {0,0};
		NewPageSize.cx = INCHES(m_fPageWidth);
		NewPageSize.cy = INCHES(m_fPageHeight);

		for (int i = 0; i < m_pAGDoc->GetNumPages(); i++)
		{
			CAGPage* pPage = m_pAGDoc->GetPage(i);
			if (!pPage)
				continue;

			SIZE PageSize = {0,0};
			pPage->GetPageSize(PageSize);
			if (PageSize.cx == NewPageSize.cx &&
				PageSize.cy == NewPageSize.cy)
				continue;

			pPage->SetPageSize(NewPageSize);
			RECT PageRect = {0, 0, PageSize.cx, PageSize.cy};
			RECT NewPageRect = {0, 0, NewPageSize.cx, NewPageSize.cy};
			CAGMatrix NewMatrix;
			NewMatrix.StretchToFit(NewPageRect, PageRect);

			int nLayers = pPage->GetNumLayers();
			for (int l = 0; l < nLayers; l++)
			{
				CAGLayer* pLayer = pPage->GetLayer(l);
				if (!pLayer)
					continue;

				int nSymbols = pLayer->GetNumSymbols();
				for (int i = 0; i < nSymbols; i++)
				{
					CAGSym* pSym = pLayer->GetSymbol(i);
					if (!pSym)
						continue;

					CAGMatrix Matrix = pSym->GetMatrix();
					bool bNonRotatedText = ((pSym->IsText() || pSym->IsAddAPhoto()) && !Matrix.IsRotated());
					bool bDoSetRect = (pSym->IsLine() || bNonRotatedText);
					Matrix *= NewMatrix;

					// Scale the text point size
					if (bNonRotatedText)
					{
						CAGSymText* pSymText = (CAGSymText*)pSym;
						pSymText->SelectAll();
						double fScale = min(Matrix.m_ax, Matrix.m_by);
						pSymText->SetTextSize(0/*TextSize*/, fScale);
					}

					// Change the symbol, either by applying the matrix, or by changing the rectangle
					if (bDoSetRect)
					{
						RECT DestRect = pSym->GetDestRect();
						Matrix.Transform(DestRect);
						Matrix.Unity();
						// Don't let the rect be empty in either dimension
						::InflateRect(&DestRect, !WIDTH(DestRect), !HEIGHT(DestRect));
						pSym->SetDestRect(DestRect);
					}

					pSym->SetMatrix(Matrix);
				}
			}
		}
	}

	EndDialog(IDOK);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CConvertDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CConvertDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pCtp)
		return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
void CConvertDlg::UpdateSample(int idSample)
{
	HWND hWnd = GetDlgItem(idSample);
	if (!hWnd)
		return;

	// If it is our first time in, initialize the sample rectangle
	if (::IsRectEmpty(&m_SampleRect))
	{
		::GetClientRect(hWnd, &m_SampleRect);
		if (::GetParent(hWnd))
			::MapWindowPoints(hWnd, ::GetParent(hWnd), (LPPOINT)&m_SampleRect.left, 2);
	}

	SIZE DocSize = {(int)(m_fPageWidth * 1000), (int)(m_fPageHeight * 1000)};
	RECT NewRect;
	::SetRect(&NewRect, 0, 0, DocSize.cx, DocSize.cy);
	CAGMatrix Matrix;
	Matrix.ScaleToFit(m_SampleRect, NewRect);
	Matrix.Transform(NewRect);
	::MoveWindow(hWnd, NewRect.left, NewRect.top, WIDTH(NewRect), HEIGHT(NewRect), true/*bRepaint*/);
}

