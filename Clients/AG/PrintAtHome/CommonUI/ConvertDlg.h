#pragma once

#include "resource.h"

#define MAX_ITEMS 40

/////////////////////////////////////////////////////////////////////////////
// CConvertDlg

class CCtp;

class CAGDoc;

class CConvertDlg : public CDialogImpl<CConvertDlg>
{
public:
	CConvertDlg(CCtp* pCtp, CAGDoc* pAGDoc);
	~CConvertDlg();

	enum { IDD = IDD_CONVERT };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void UpdateSample(int idSample);

public:
	BEGIN_MSG_MAP(CConvertDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDC_PAPERTYPE, OnPaperType)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

private:
	CCtp* m_pCtp;
	CAGDoc* m_pAGDoc;
	AGDOCTYPE m_idDocType;
	int m_iPaperType;
	int m_iPaperTypeOrig;
	double m_fPageWidthOrig;
	double m_fPageWidth;
	double m_fPageHeightOrig;
	double m_fPageHeight;
	RECT m_SampleRect;
};
