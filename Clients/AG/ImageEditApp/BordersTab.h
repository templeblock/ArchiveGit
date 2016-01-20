#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "HyperLink.h"
#include "afxwin.h"

// CBordersTab dialog

class CBordersTab : public CBaseTab
{
public:
	CBordersTab(CWnd* pParent = NULL);
	virtual ~CBordersTab();

	enum { IDD = IDD_BORDERS_TAB };

protected:
	DECLARE_DYNAMIC(CBordersTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnApply();
	afx_msg void OnReset();
	afx_msg void OnBorderNone();
	afx_msg void OnBorderImage();
	afx_msg void OnBorderFade();
	afx_msg void OnBorderImageChoose();

private:
	CHyperLink m_btnChoose;
	CEdit m_editBorderImageFileName;
	CComboBox m_comboFadedEdgeShape;
	CComboBox m_comboFadedEdgeDirection;
	CEdit m_editFadedEdgeWidth;
	CEdit m_editFadedEdgeOpacity;
};
