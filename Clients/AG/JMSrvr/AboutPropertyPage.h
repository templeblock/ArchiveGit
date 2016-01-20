#pragma once
#include "resource.h"
#include "Label.h"
#include "Authenticate.h"


class COptionsPropertySheet;
//////////////////////////////////////////////////////////////////////////////////////
class CAboutPropertyPage :
	public CPropertyPageImpl<CAboutPropertyPage>,
	public CWinDataExchange<CAboutPropertyPage>
{
public:
	enum { IDD = IDD_ABOUT2 };

	CAboutPropertyPage(COptionsPropertySheet* psParentSheet);
	virtual ~CAboutPropertyPage(void);

	// Maps
    BEGIN_MSG_MAP(CAboutPropertyPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CAboutPropertyPage>)
		REFLECT_NOTIFICATIONS()
    END_MSG_MAP()
	
	BEGIN_DDX_MAP(CAboutPropertyPage)
    END_DDX_MAP()

	// Overrides. 
	//Property page notification handlers
    BOOL OnApply();

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	void UpdateVersion();

public:
	COptionsPropertySheet* m_psParentSheet;
	
private:
	CAuthenticate m_Auth;
	CLabel m_LicenseLabel;
	CLabel m_SalesLabel;
	CLabel m_PivacyLabel;
	CLabel m_HomeUrlLabel;
	
	
 
};
