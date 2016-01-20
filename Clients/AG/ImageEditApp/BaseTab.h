#pragma once

#include "resource.h"
#include "ResizeDlg.h"
#include "ImageControlWrapper.h"
#include "ImageControlConstants.h"

// CBaseTab dialog

class CBaseTab : public CResizeDlg
{
public:
	CBaseTab(const UINT resID, CWnd* pParent);
	virtual ~CBaseTab();
	bool EnableControl(UINT idControl, bool bEnable);
	bool ShowControl(UINT idControl, UINT nCmdShow);
	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue);
	long GetCommand(LPCTSTR strCommand);
	CImageControl& GetImageControl();

protected:
	virtual void PreSubclassWindow();

private:
	virtual afx_msg void OnEnterTab() {};
	virtual afx_msg void OnLeaveTab() {};
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnBack();

protected:
	DECLARE_DYNAMIC(CBaseTab)
	DECLARE_MESSAGE_MAP()
};
