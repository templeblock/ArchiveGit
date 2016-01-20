#pragma once
#include "resource.h"
#include "BordersTab.h"
#include "ColorsTab.h"
#include "ContrastBrightnessTab.h"
#include "CropTab.h"
#include "EffectsTab.h"
#include "FileTab.h"
#include "LayoutTab.h"
#include "MainTab.h"
#include "RecolorTab.h"
#include "RedEyeTab.h"
#include "ResizeTab.h"

class CTabControl : public CWindowImpl<CTabControl, CTabCtrl>
{
public:
	CTabControl();
	virtual ~CTabControl();

	BEGIN_MSG_MAP_EX(CTabControl)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ENABLE(OnEnable)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER_EX(WM_USER, OnGetObject);
		REFLECTED_NOTIFY_CODE_HANDLER(TCN_SELCHANGE, OnSelChange)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	void Init(UINT idTab);
	bool CanApply();
	bool CanReset();
	bool Apply();
	bool Reset();
	UINT GetCurrentTab();
	bool SetCurrentTab(UINT nId);

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

	CMainTab m_MainTab;
	CFileTab m_FileTab;
	CCropTab m_CropTab;
	CContrastBrightnessTab m_ContrastBrightnessTab;
	CEffectsTab m_EffectsTab;
	CColorsTab m_ColorsTab;
	CResizeTab m_ResizeTab;
	CFlipRotateTab m_FlipRotateTab;
	CRedEyeTab m_RedEyeTab;
	CBordersTab m_BordersTab;
	CRecolorTab m_RecolorTab;

	bool m_bSetMaxSize;
	int m_iPreviousTab;
	int m_iMaxWidth;
	int m_iMaxHeight;
	int m_iTabCount;
};
