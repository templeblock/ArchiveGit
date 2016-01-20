#pragma once

#include "resource.h"

class CCtp;
class CAGDoc;

class CEnvelopesDlg : public CDialogImpl<CEnvelopesDlg>
{
public:
	CEnvelopesDlg(CCtp* pCtp, int iPaperFeedCode);
	~CEnvelopesDlg();

	enum { IDD = IDD_ENVELOPES_2 };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEnvPos(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEnvFace(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEnvEdge(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEnvFeed(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckMatch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	int GetPaperFeedCode();

public:
	BEGIN_MSG_MAP(CEnvelopesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_POS_LEFT, OnCheckEnvPos)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_POS_CENTER, OnCheckEnvPos)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_POS_RIGHT, OnCheckEnvPos)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_FACE_DOWN, OnCheckEnvFace)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_FACE_UP, OnCheckEnvFace)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_EDGE_TOP, OnCheckEnvEdge)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_EDGE_LEFT, OnCheckEnvEdge)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_EDGE_RIGHT, OnCheckEnvEdge)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_BOTTOM, OnCheckEnvFeed)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_TOP, OnCheckEnvFeed)
		COMMAND_ID_HANDLER(IDC_CHECKMATCH, OnCheckMatch)
	END_MSG_MAP()

private:
	CCtp* m_pCtp;
	int m_iPaperFeedCode;
	HFONT m_hHdrFont;
	HFONT m_hChkFont;
	HFONT m_hStpFont;
};
