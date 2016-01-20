#include "stdafx.h"
#include "PaperFeedDlg.h"
#include "PaperFeed.h"
#include "Ctp.h"
#include "AGDoc.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CPaperFeedDlg::CPaperFeedDlg(CCtp* pCtp, int iPaperFeedCode)
{
	m_pCtp = pCtp;
	m_iPaperFeedCode = iPaperFeedCode;
}

//////////////////////////////////////////////////////////////////////
CPaperFeedDlg::~CPaperFeedDlg()
{
	DeleteObject(m_hHdrFont);
	DeleteObject(m_hChkFont);
	DeleteObject(m_hStpFont);
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

	{
		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(lf));
		::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

		strcpy_s(lf.lfFaceName, "Microsoft Sans Serif");
		lf.lfHeight = -13;
		lf.lfWeight = FW_BOLD;
		m_hHdrFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_HDR).SetFont(m_hHdrFont);

		lf.lfHeight = -12;
		lf.lfWeight = FW_SEMIBOLD;
		m_hChkFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_CHECKMATCH).SetFont(m_hChkFont);

		lf.lfHeight = -13;
		lf.lfWeight = FW_BOLD;
		lf.lfUnderline = true;
		m_hStpFont = ::CreateFontIndirect(&lf);
		GetDlgItem(IDC_STEP1).SetFont(m_hStpFont);
		GetDlgItem(IDC_STEP2).SetFont(m_hStpFont);
	}

	CheckRadioButton(IDC_PAPERFEED_BOTTOM, IDC_PAPERFEED_TOP, CPaperFeed::GetFeed(m_iPaperFeedCode));
	CheckRadioButton(IDC_PAPERFEED_POS_LEFT, IDC_PAPERFEED_POS_RIGHT, CPaperFeed::GetPos(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetPositionIcon(m_iPaperFeedCode))));
	CheckDlgButton(IDC_CHECKMATCH, BST_UNCHECKED);
	::EnableWindow(GetDlgItem(IDOK).m_hWnd, false);

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pCtp)
		return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDOK);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnCheckMatch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::EnableWindow(GetDlgItem(IDOK).m_hWnd, (IsDlgButtonChecked(IDC_CHECKMATCH)==BST_CHECKED));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnCheckEnvFeed(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CPaperFeed::SetFeed(m_iPaperFeedCode, wID);
	CheckRadioButton(IDC_PAPERFEED_BOTTOM, IDC_PAPERFEED_TOP, CPaperFeed::GetFeed(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetPositionIcon(m_iPaperFeedCode))));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPaperFeedDlg::OnCheckEnvPos(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CPaperFeed::SetPos(m_iPaperFeedCode, wID);
	CheckRadioButton(IDC_PAPERFEED_POS_LEFT, IDC_PAPERFEED_POS_RIGHT, CPaperFeed::GetPos(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetPositionIcon(m_iPaperFeedCode))));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
int CPaperFeedDlg::GetPaperFeedCode()
{
	return m_iPaperFeedCode;
}
