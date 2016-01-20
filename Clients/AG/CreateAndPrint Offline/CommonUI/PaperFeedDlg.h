#pragma once

#include "resource.h"

class CCtp;
class CAGDoc;

class CPaperFeedDlg : public CDialogImpl<CPaperFeedDlg>
{
public:
	CPaperFeedDlg(CCtp* pCtp, int iPaperFeedCode);
	~CPaperFeedDlg();

	enum { IDD = IDD_PAPERFEED_2 };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEnvPos(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckMatch(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCheckEnvFeed(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	int GetPaperFeedCode();

public:
	BEGIN_MSG_MAP(CPaperFeedDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_POS_LEFT, OnCheckEnvPos)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_POS_CENTER, OnCheckEnvPos)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_POS_RIGHT, OnCheckEnvPos)
		COMMAND_ID_HANDLER(IDC_CHECKMATCH, OnCheckMatch)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_BOTTOM, OnCheckEnvFeed)
		COMMAND_ID_HANDLER(IDC_PAPERFEED_TOP, OnCheckEnvFeed)
	END_MSG_MAP()

private:
	CCtp* m_pCtp;
	int m_iPaperFeedCode;
	HFONT m_hHdrFont;
	HFONT m_hChkFont;
	HFONT m_hStpFont;
};
