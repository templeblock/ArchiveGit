#pragma once
#include "resource.h"
#include "Authenticate.h"
#include "ComboBoxSuper.h"

class COptionsPropertySheet;
/////////////////////////////////////////////////////////////////////////////////
class CGeneralPropertyPage :
	public CPropertyPageImpl<CGeneralPropertyPage>,
	public CWinDataExchange<CGeneralPropertyPage>
{
public:
	enum { IDD = IDD_GENERAL_PROPPAGE };

	CGeneralPropertyPage(COptionsPropertySheet* psParentSheet);
	virtual ~CGeneralPropertyPage(void);

	// Maps
    BEGIN_MSG_MAP(CGeneralPropertyPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_HELP, OnHelpMsg)
		COMMAND_HANDLER(IDC_WEBMAIL_CHECK, BN_CLICKED, OnIESelect)
		COMMAND_HANDLER(IDC_OUTLOOK_CHECK, BN_CLICKED, OnOLSelect)
		COMMAND_HANDLER(IDC_EXPRESS_CHECK, BN_CLICKED, OnOESelect)
		COMMAND_HANDLER(IDC_AOL_CHECK, BN_CLICKED, OnAOLSelect)
		COMMAND_HANDLER(IDC_OL_CLEAR_BUTTON, BN_CLICKED, OnOLClear)
		COMMAND_HANDLER(IDC_OE_CLEAR_BUTTON, BN_CLICKED, OnOEClear)
		COMMAND_HANDLER(IDC_IE_CLEAR_BUTTON, BN_CLICKED, OnIEClear)
		COMMAND_HANDLER(IDC_AOL_CLEAR_BUTTON, BN_CLICKED, OnAOLClear)
		COMMAND_CODE_HANDLER(CBN_SELENDOK, OnSelEndOk)
		CHAIN_MSG_MAP(CPropertyPageImpl<CGeneralPropertyPage>)
    END_MSG_MAP()
	
	BEGIN_DDX_MAP(CGeneralPropertyPage)
		DDX_CHECK(IDC_SMARTCACHE_CHECK, m_nSmartCacheStatus)
        DDX_CHECK(IDC_TRAYICON_CHECK, m_nTrayIconStatus)
		DDX_CHECK(IDC_ALWAYSONTOP_CHECK, m_nAlwaysOnTop)
		DDX_CHECK(IDC_OUTLOOK_CHECK, m_nOutlookStatus)
        DDX_CHECK(IDC_EXPRESS_CHECK, m_nExpressStatus)
		DDX_CHECK(IDC_WEBMAIL_CHECK, m_nWebMailStatus)
		DDX_CHECK(IDC_AOL_CHECK, m_nAOLStatus)
		DDX_CHECK(IDC_OTLK_USEDEFAULT_CHECK, m_nUseOutlookDefaultSta)
		DDX_CHECK(IDC_EXPRESS_USEDEFAULT_CHECK, m_nUseExpressDefaultSta)
		DDX_CHECK(IDC_IE_USEDEFAULT_CHECK, m_nUseIEDefaultSta)
		DDX_CHECK(IDC_AOL_USEDEFAULT_CHECK, m_nUseAOLDefaultSta)
    END_DDX_MAP()

	// Overrides. 
	//Property page notification handlers
    BOOL OnApply();

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHelpMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSelEndOk(WORD uMsg, WORD idCtrl, HWND hWndControl, BOOL& bHandled);
	LRESULT OnIESelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOLSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOESelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAOLSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOLClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOEClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnIEClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnAOLClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	// Access methods
	BOOL GetTrayIconStatus(){return m_nTrayIconStatus;}
	BOOL GetSmartCacheStatus(){return m_nSmartCacheStatus;}
	BOOL GetAlwaysOnTopStatus(){return m_nAlwaysOnTop;}
	BOOL GetOutlookStatus(){return m_nOutlookStatus;}
	BOOL GetExpressStatus(){return m_nExpressStatus;}
	BOOL GetWebMailStatus(){return m_nWebMailStatus;}
	void SetSmartCacheStatus(BOOL bVal){m_nSmartCacheStatus = bVal;}
	void SetTrayIconStatus(BOOL bVal){m_nTrayIconStatus = bVal;}
	void SetAlwaysOnTopStatus(BOOL bVal){m_nAlwaysOnTop = bVal;}
	void SetOutlookStatus(BOOL bVal){m_nOutlookStatus = bVal;}
	void SetExpressStatus(BOOL bVal){m_nExpressStatus = bVal;}
	void SetWebMailStatus(BOOL bVal){m_nWebMailStatus = bVal;}

protected:
	BOOL InitCombo();
	int AddComboImage(DWORD dwID);

private:
	COptionsPropertySheet* m_psParentSheet;
	CAuthenticate m_Auth;
	CComboBoxSuper m_ctlIECatCombo;
	CImageList m_ImageList;
	int m_nSmartCacheStatus;
	int m_nTrayIconStatus;
	int m_nAlwaysOnTop;
	int m_nOutlookStatus;
	int m_nExpressStatus;
	int m_nWebMailStatus;
	int m_nAOLStatus;
	int m_nIECat;
	int m_nUseOutlookDefaultSta;
	int m_nUseExpressDefaultSta;
	int m_nUseIEDefaultSta;
	int m_nUseAOLDefaultSta;
};
