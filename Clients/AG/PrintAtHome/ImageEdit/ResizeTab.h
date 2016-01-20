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
	afx_msg void OnResize();
	afx_msg void OnResizeOriginal();
	afx_msg void OnResizePredefine();
	afx_msg void OnResizeCustom();
	afx_msg void OnResizePercentage();

private:
	CComboBox m_comboPredefinedSizes;
	CEdit m_editCustomWidth;
	CEdit m_editCustomHeight;
	CEdit m_editPercentage;
	CButton m_btnResize;
};
