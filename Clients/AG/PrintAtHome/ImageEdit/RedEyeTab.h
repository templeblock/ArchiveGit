#pragma once

#include "resource.h"
#include "BaseTab.h"

// CRedEyeTab dialog

class CRedEyeTab : public CBaseTab
{
public:
	CRedEyeTab(CWnd* pParent = NULL);
	virtual ~CRedEyeTab();

	enum { IDD = IDD_REDEYE_TAB };

protected:
	DECLARE_DYNAMIC(CRedEyeTab)
	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnRedEyeFix();
	afx_msg void OnRedEyeReset();
};
