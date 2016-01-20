#include "stdafx.h"
#include "EnvelopesDlg.h"
#include "PaperFeed.h"
#include "Ctp.h"
#include "AGDoc.h"

//////////////////////////////////////////////////////////////////////
CEnvelopesDlg::CEnvelopesDlg(CCtp* pCtp, int iPaperFeedCode)
{
	m_pCtp = pCtp;
	m_iPaperFeedCode = iPaperFeedCode;
}

//////////////////////////////////////////////////////////////////////
CEnvelopesDlg::~CEnvelopesDlg()
{
	DeleteObject(m_hHdrFont);
	DeleteObject(m_hChkFont);
	DeleteObject(m_hStpFont);
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

	{
		LOGFONT lf;

		//GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(lf), (LPVOID)&lf);

		::ZeroMemory(&lf, sizeof(lf));
		::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

		strcpy(lf.lfFaceName, "Microsoft Sans Serif");
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
		GetDlgItem(IDC_STEP3).SetFont(m_hStpFont);
		GetDlgItem(IDC_STEP4).SetFont(m_hStpFont);
	}

	CheckRadioButton(IDC_PAPERFEED_BOTTOM, IDC_PAPERFEED_TOP, CPaperFeed::GetFeed(m_iPaperFeedCode));
	CheckRadioButton(IDC_PAPERFEED_POS_LEFT, IDC_PAPERFEED_POS_RIGHT, CPaperFeed::GetPos(m_iPaperFeedCode));
	CheckRadioButton(IDC_PAPERFEED_FACE_DOWN, IDC_PAPERFEED_FACE_UP, CPaperFeed::GetFace(m_iPaperFeedCode));
	CheckRadioButton(IDC_PAPERFEED_EDGE_LEFT, IDC_PAPERFEED_EDGE_TOP, CPaperFeed::GetEdge(m_iPaperFeedCode));
	CheckDlgButton(IDC_CHECKMATCH, BST_UNCHECKED);
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode))));
	::EnableWindow(GetDlgItem(IDOK).m_hWnd, false);

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pCtp)
		return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDOK);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCheckMatch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	::EnableWindow(GetDlgItem(IDOK).m_hWnd, (IsDlgButtonChecked(IDC_CHECKMATCH)==BST_CHECKED));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCheckEnvFeed(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CPaperFeed::SetFeed(m_iPaperFeedCode, wID);
	CheckRadioButton(IDC_PAPERFEED_BOTTOM, IDC_PAPERFEED_TOP, CPaperFeed::GetFeed(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode))));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCheckEnvPos(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CPaperFeed::SetPos(m_iPaperFeedCode, wID);
	CheckRadioButton(IDC_PAPERFEED_POS_LEFT, IDC_PAPERFEED_POS_RIGHT, CPaperFeed::GetPos(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode))));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCheckEnvFace(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CPaperFeed::SetFace(m_iPaperFeedCode, wID);
	CheckRadioButton(IDC_PAPERFEED_FACE_DOWN, IDC_PAPERFEED_FACE_UP, CPaperFeed::GetFace(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode))));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopesDlg::OnCheckEnvEdge(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CPaperFeed::SetEdge(m_iPaperFeedCode, wID);
	CheckRadioButton(IDC_PAPERFEED_EDGE_LEFT, IDC_PAPERFEED_EDGE_TOP, CPaperFeed::GetEdge(m_iPaperFeedCode));
	SendDlgItemMessage(IDC_PAPERFEED_ACTIVE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(CPaperFeed::GetEnvelopeIconEx(m_iPaperFeedCode))));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
int CEnvelopesDlg::GetPaperFeedCode()
{
	return m_iPaperFeedCode;
}
