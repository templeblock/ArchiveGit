#pragma once
#include "Generalpropertypage.h"
#include "Preferencepropertypage.h"
#include "MaintPropertypage.h"
#include "Aboutpropertypage.h"
#include "Authenticate.h"



#define UWM_CENTER_SHEET_MSG _T("UWM_CENTER_SHEET_MSG-999D99BB-7300-49f2-8726-DF71A4FD5CB2")
static const UINT UWM_CENTER_SHEET	= ::RegisterWindowMessage(UWM_CENTER_SHEET_MSG);

class CJMBrkr;
////////////////////////////////////////////////////////////////////////////////////////////////////
class COptionsPropertySheet : public CPropertySheetImpl<COptionsPropertySheet>
{
public:

	// Construction
    COptionsPropertySheet ( CJMBrkr* pBroker, _U_STRINGorID title = (LPCTSTR) NULL, 
			UINT uStartPage = 0, HWND hWndParent = NULL );

	virtual ~COptionsPropertySheet(void);

    // Maps
    BEGIN_MSG_MAP(COptionsPropertySheet)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(UWM_CENTER_SHEET, OnPageInit)
        CHAIN_MSG_MAP(CPropertySheetImpl<COptionsPropertySheet>)
    END_MSG_MAP()

	// Message handlers
	LRESULT OnPageInit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	// Property pages
    CGeneralPropertyPage		m_pgGeneral;
	CPreferencePropPage			m_pgPreference;
	CMaintPropertyPage			m_pgMaint;
	CAboutPropertyPage			m_pgAbout;


public:
	void ShowContextHelp(HWND hwndCaller, int iID, int iXPos, int iYPos);
	bool LoadPages();
	BOOL UpdateClients(DWORD dwStatus);
	BOOL HandleShow(BOOL bShow);
	BOOL HandleHelp();
	BOOL HandleHome();
	BOOL HandleUpgrade();
	BOOL HandleMyAccount();
	BOOL HandleNewMail();
	BOOL HandleCheckForUpdates();
	BOOL HandleOptimize();
	BOOL HandleAuth(DWORD dwMode);

protected:
	void PositionWindow();

private:
	CAuthenticate m_Authenticate;
	CJMBrkr * m_pBroker;

};
