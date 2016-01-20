#pragma once

#include "resource.h"
#include "BaseTab.h"

// CFileTab dialog

class CFileTab : public CBaseTab
{
public:
	CFileTab(CWnd* pParent = NULL);
	virtual ~CFileTab();

	enum { IDD = IDD_FILE_TAB };

protected:
	DECLARE_DYNAMIC(CFileTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnImageOpen();
	afx_msg void OnImageSave();
	afx_msg void OnImageClose();
};
