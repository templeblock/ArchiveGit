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
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnApply();
	afx_msg void OnReset();
	afx_msg void OnMakeGray();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	int m_iAmount;
	int m_iHue;
	int m_iSaturation;
	CBaseSliderCtrl m_AmountSlider;
	CBaseSliderCtrl m_HueSlider;
	CBaseSliderCtrl m_SaturationSlider;
};
