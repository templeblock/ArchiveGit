#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "BaseSliderCtrl.h"

// CContrastBrightnessTab dialog

class CContrastBrightnessTab : public CBaseTab
{
public:
	CContrastBrightnessTab(CWnd* pParent = NULL);
	virtual ~CContrastBrightnessTab();

	enum { IDD = IDD_CONTRAST_BRIGHTNESS_TAB };

protected:
	DECLARE_DYNAMIC(CContrastBrightnessTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnAutoCorrect();
	afx_msg void OnApply();
	afx_msg void OnReset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	int m_iContrast;
	int m_iBrightness;
	CBaseSliderCtrl m_ContrastSlider;
	CBaseSliderCtrl m_BrightnessSlider;
};
