#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "BaseSliderCtrl.h"

// CColorsTab dialog

class CColorsTab : public CBaseTab
{
public:
	CColorsTab(CWnd* pParent = NULL);
	virtual ~CColorsTab();

	enum { IDD = IDD_COLORS_TAB };

protected:
	DECLARE_DYNAMIC(CColorsTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnHue();
	afx_msg void OnHueApply();
	afx_msg void OnHueCancel();
	afx_msg void OnSaturation();
	afx_msg void OnSaturationApply();
	afx_msg void OnSaturationCancel();
	afx_msg void OnMakeGray();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	int m_iLastValue;
	CBaseSliderCtrl m_AmountSlider;
	CBaseSliderCtrl m_HueSlider;
	CBaseSliderCtrl m_SaturationSlider;
};
