#pragma once
#include "resource.h"
#include "Authenticate.h"

class COptionsPropertySheet;
//////////////////////////////////////////////////////////////////////////////////
class CPreferencePropPage :
	public CPropertyPageImpl<CPreferencePropPage>,
	public CWinDataExchange<CPreferencePropPage>
{
public:
	enum {IDD = IDD_PREFERENCE_PROPPAGE};

	CPreferencePropPage(COptionsPropertySheet* psParentSheet);
	virtual ~CPreferencePropPage(void);

	// Maps
    BEGIN_MSG_MAP(CPreferencePropPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_HELP, OnHelpMsg)
		COMMAND_HANDLER(IDC_MYACCOUNT_BTN, BN_CLICKED, OnMyAccount)
		COMMAND_HANDLER(IDC_UNLOCK_BTN, BN_CLICKED, OnUnlock)
		CHAIN_MSG_MAP(CPropertyPageImpl<CPreferencePropPage>)
    END_MSG_MAP()
	
	BEGIN_DDX_MAP(CPreferencePropPage)
		DDX_CHECK(IDC_PROMO_CHECK, m_nPromoStatus)
		DDX_TEXT(IDC_REGISTEREDEMAIL, m_szRegEmailAddr)
    END_DDX_MAP()

	// Overrides. 
	//Property page notification handlers
    BOOL OnApply();
	int OnSetActive();

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHelpMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMyAccount(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnUnlock(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	
	void SetPromoStatus(BOOL bVal){m_nPromoStatus = bVal;}
	BOOL GetPromoStatus(){return m_nPromoStatus;}
	
	COptionsPropertySheet* m_psParentSheet;

private:
	CAuthenticate m_Auth;
	CString m_szRegEmailAddr;
	int m_nPromoStatus;
};
