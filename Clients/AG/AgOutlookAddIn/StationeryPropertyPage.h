#pragma once
#include "resource.h"

class CStationeryPropertyPage :
	public CPropertyPageImpl<CStationeryPropertyPage>,
	public CWinDataExchange<CStationeryPropertyPage>
{
public:
	enum {IDD = IDD_STATIONERY_PROPPAGE};

	CStationeryPropertyPage(void);
	virtual ~CStationeryPropertyPage(void);

	// Maps
    BEGIN_MSG_MAP(CStationeryPropertyPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CPropertyPageImpl<CStationeryPropertyPage>)
    END_MSG_MAP()
	
	BEGIN_DDX_MAP(CStationeryPropertyPage)
        DDX_CHECK(IDC_CLEARDEFAULT, m_nClearDefault)
        DDX_CHECK(IDC_CLEARCURRENT, m_nClearCurrent)
    END_DDX_MAP()

	// Overrides. 
	//Property page notification handlers
    BOOL OnApply();
	int OnSetActive();

	// Message handlers
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	void SetDefaultStatus(BOOL bVal);
	void SetCurrentStatus(BOOL bVal);
	BOOL GetDefaultStatus();
	BOOL GetCurrentStatus();

private:
	int m_nClearDefault;
	int m_nClearCurrent;
 
};
