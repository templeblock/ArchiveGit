#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "BaseSliderCtrl.h"

// CEffectsTab dialog

class CEffectsTab : public CBaseTab
{
public:
	CEffectsTab(CWnd* pParent = NULL);
	virtual ~CEffectsTab();

	enum { IDD = IDD_EFFECTS_TAB };

protected:
	DECLARE_DYNAMIC(CEffectsTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnSharpen();
	afx_msg void OnSharpenApply();
	afx_msg void OnSharpenCancel();
	afx_msg void OnSmooth();
	afx_msg void OnSmoothApply();
	afx_msg void OnSmoothCancel();
	afx_msg void OnEdges();
	afx_msg void OnEdgesApply();
	afx_msg void OnEdgesCancel();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	int m_iLastValue;
	CBaseSliderCtrl m_SharpenSlider;
	CBaseSliderCtrl m_SmoothSlider;
	CBaseSliderCtrl m_EdgesSlider;
};
