#pragma once
#include "resource.h"
#include "EMAddressTab.h"
#include "EMDesignTab.h"

class CEMDlg;

///////////////////////////////////////////////////////////////////////////////
class CEMTabControl : public CWindowImpl<CEMTabControl, CTabCtrl>
{
public:
	CEMTabControl(CEMDlg * pEMDlg);
	virtual ~CEMTabControl();

	BEGIN_MSG_MAP_EX(CEMTabControl)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ENABLE(OnEnable)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER_EX(WM_USER, OnGetObject);
		REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnSelChange)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	void Init(UINT iTabID);
	bool CanApply();
	bool CanReset();
	bool Apply();
	bool Reset();
	UINT GetCurrentTab();
	bool SetCurrentTab(UINT nId);

	CEMAddressTab * GetAddressTab()
		{return &m_EMAddressTab;}

	CEMDesignTab * GetDesignTab()
		{return &m_EMDesignTab;}



protected:
	LRESULT OnSelChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	void OnEnable(BOOL bEnable);
	void OnDestroy();
	void OnSize(UINT nType, CSize Size);
	LRESULT OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void CreateTabs();
	bool CreateTab(int iIndex, UINT nId);
	void ResizeDialogs();
	int SetCurSel(int nItem); // Override
	CWindow& GetBaseTab();

private:
	CDialogImplBase* m_pTabDialogs[20];

	CEMAddressTab m_EMAddressTab;
	CEMDesignTab m_EMDesignTab;
	CEMDlg * m_pEMDlg;
	
	bool m_bSetMaxSize;
	int m_iPreviousTab;
	int m_iMaxWidth;
	int m_iMaxHeight;
	int m_iTabCount;
};
