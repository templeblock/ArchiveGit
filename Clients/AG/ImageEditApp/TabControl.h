#pragma once
#include "ResizeDlg.h"
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

class CTabControl : public CTabCtrl
{
public:
	CTabControl();
	virtual ~CTabControl();
	void CreateTabs();
	bool CreateTab(int iIndex, UINT nId);
	void ResizeDialogs();
	bool SetCurrentTab(UINT nId);
	int SetCurSel(int nItem); // Override

protected:
	virtual void PreSubclassWindow();
	afx_msg void OnEnable(BOOL bEnable);
public:
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
protected:
	afx_msg void OnDestroy();
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	DECLARE_MESSAGE_MAP()

private:
	CResizeDlg* m_pTabDialogs[20];

	CBordersTab m_BordersTab;
	CColorsTab m_ColorsTab;
	CContrastBrightnessTab m_ContrastBrightnessTab;
	CCropTab m_CropTab;
	CEffectsTab m_EffectsTab;
	CFileTab m_FileTab;
	CLayoutTab m_LayoutTab;
	CMainTab m_MainTab;
	CRecolorTab m_RecolorTab;
	CRedEyeTab m_RedEyeTab;
	CResizeTab m_ResizeTab;

	bool m_bSetMaxSize;
	int m_iPreviousTab;
	int m_iMaxWidth;
	int m_iMaxHeight;
	int m_iTabCount;
};
