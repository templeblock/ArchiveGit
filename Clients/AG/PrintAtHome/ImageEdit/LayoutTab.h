#pragma once

#include "resource.h"
#include "BaseTab.h"

// CLayoutTab dialog

class CLayoutTab : public CBaseTab
{
public:
	CLayoutTab(CWnd* pParent = NULL);
	virtual ~CLayoutTab();

	enum { IDD = IDD_FLIP_ROTATE_TAB };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnFlipHorizontal();
	afx_msg void OnFlipVertical();
	afx_msg void OnRotateLeft();
	afx_msg void OnRotateRight();

protected:
	DECLARE_DYNAMIC(CLayoutTab)
	DECLARE_MESSAGE_MAP()
};
