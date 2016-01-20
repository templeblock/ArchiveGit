#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "afxcmn.h"
#include "afxwin.h"

// CResizeTab dialog

class CResizeTab : public CBaseTab
{
public:
	CResizeTab(CWnd* pParent = NULL);
	virtual ~CResizeTab();

	enum { IDD = IDD_RESIZE_TAB };

protected:
	DECLARE_DYNAMIC(CResizeTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnApply();
	afx_msg void OnReset();
	afx_msg void OnResizeOriginal();
	afx_msg void OnResizePredefined();
	afx_msg void OnResizePredefinedChanged();
	afx_msg void OnResizeCustom();
	afx_msg void OnResizeCustomSizeChanged();
	afx_msg void OnResizePercentage();
	afx_msg void OnResizePercentageChanged();

private:
	void UpdateDimensions();

private:
	CSize m_SizeImage;
	CSize m_SizeLimits;
	CSize m_Size;
	CComboBox m_comboPredefinedSizes;
	CEdit m_editCustomWidth;
	CEdit m_editCustomHeight;
	CEdit m_editPercentage;
	CButton m_radioOriginalSize;
	CButton m_radioPredefinedSize;
	CButton m_radioCustomSize;
	CButton m_radioPercetageofOriginal;
	CStatic m_staticOriginalDimensions;
	CStatic m_staticNewDimensions;
	CButton m_btnApply;
	CButton m_btnReset;
};
