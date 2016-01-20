#pragma once

#include "resource.h"

#define MAX_ITEMS 40

/////////////////////////////////////////////////////////////////////////////
// CPropertiesDlg

class CCtp;

class CAGDoc;

class CPropertiesDlg : public CDialogImpl<CPropertiesDlg>
{
public:
	CPropertiesDlg(CCtp* pCtp, CAGDoc* pAGDoc);
	~CPropertiesDlg();

	enum { IDD = IDD_PROPERTIESDLG };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void AddDocType(AGDOCTYPE idDocType);
	int FindDocType(AGDOCTYPE idDocType);
	LRESULT OnDocType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnPageNames(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CPropertiesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDC_DOCTYPE, OnDocType)
		COMMAND_ID_HANDLER(IDC_PAGENAMES, OnPageNames)
		COMMAND_ID_HANDLER(IDC_WIDTH, OnWidth)
		COMMAND_ID_HANDLER(IDC_HEIGHT, OnHeight)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

private:
	CCtp* m_pCtp;
	CAGDoc* m_pAGDoc;
	int m_nSelectedPage;
	int m_nDocTypes;
	AGDOCTYPE m_idDocTypes[MAX_ITEMS];
	AGDOCTYPE m_idDocType;
	CString m_strPageNamesOrig[MAX_AGPAGE];
	CString m_strPageNames[MAX_AGPAGE];
	double m_fPageWidthsOrig[MAX_AGPAGE];
	double m_fPageWidths[MAX_AGPAGE];
	double m_fPageHeightsOrig[MAX_AGPAGE];
	double m_fPageHeights[MAX_AGPAGE];
};
