#pragma once
#include "resource.h"
#include "Authenticate.h"

class COptionsPropertySheet;
//////////////////////////////////////////////////////////////////////////////////////
class CMaintPropertyPage :
	public CPropertyPageImpl<CMaintPropertyPage>,
	public CWinDataExchange<CMaintPropertyPage>
{
public:
	enum { IDD = IDD_MAINT_PROPPAGE };

	CMaintPropertyPage(COptionsPropertySheet* psParentSheet);
	virtual ~CMaintPropertyPage(void);

	// Maps
    BEGIN_MSG_MAP(CMaintPropertyPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_HELP, OnHelpMsg)
		COMMAND_HANDLER(IDC_CHECKUPDATES_BTN, BN_CLICKED, OnCheckUpdates)
		COMMAND_HANDLER(IDC_OPTIMIZE_BTN, BN_CLICKED, OnOptimize)
		CHAIN_MSG_MAP(CPropertyPageImpl<CMaintPropertyPage>)
    END_MSG_MAP()
	
	BEGIN_DDX_MAP(CMaintPropertyPage)
    END_DDX_MAP()

	// Overrides. 
	//Property page notification handlers
    BOOL OnApply();

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHelpMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCheckUpdates(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOptimize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	
	

public:
	COptionsPropertySheet* m_psParentSheet;
	
private:
	CAuthenticate m_Auth; 
};
