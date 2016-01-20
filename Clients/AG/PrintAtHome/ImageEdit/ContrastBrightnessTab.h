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
	afx_msg void OnAutoCorrect();
	afx_msg void OnContrast();
	afx_msg void OnContrastApply();
	afx_msg void OnContrastCancel();
	afx_msg void OnBrightness();
	afx_msg void OnBrightnessApply();
	afx_msg void OnBrightnessCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	int m_iLastValue;
	CBaseSliderCtrl m_ContrastSlider;
	CBaseSliderCtrl m_BrightnessSlider;
};
