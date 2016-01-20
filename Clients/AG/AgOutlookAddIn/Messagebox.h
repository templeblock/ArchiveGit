#pragma once
#include "resource.h"

class CJMExplorerBand;
class CMessagebox :
	public CDialogImpl<CMessagebox>
{
public:
	CMessagebox();
	virtual ~CMessagebox(void);
	
	enum { IDD = IDD_MESSAGE_DIALOG };

	int ShowMessagebox( HWND hwndParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType=MB_OK|MB_HELP); 

private:
	HWND m_hParent;
	UINT m_uType;
	LPCTSTR m_szText;
	LPCTSTR m_szCaption;
public:
	BEGIN_MSG_MAP(CMessagebox)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
		COMMAND_HANDLER(IDHELP, BN_CLICKED, OnBnClickedHelpButton)
		COMMAND_HANDLER(IDCLOSE, BN_CLICKED, OnBnClickedCloseButton)
		COMMAND_HANDLER(IDYES, BN_CLICKED, OnBnClickedYesButton)
		COMMAND_HANDLER(IDNO, BN_CLICKED, OnBnClickedNoButton)
	END_MSG_MAP()
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedHelpButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCloseButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedYesButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedNoButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
