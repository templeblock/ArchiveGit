#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "afxwin.h"
//j#include "EditEx.h"

// CCropTab dialog

class CCropTab : public CBaseTab
{
public:
	CCropTab(CWnd* pParent = NULL);
	virtual ~CCropTab();

	enum { IDD = IDD_CROP_TAB };

protected:
	DECLARE_DYNAMIC(CCropTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	afx_msg void OnEnterTab();
	afx_msg void OnLeaveTab();
	afx_msg void OnApply();
	afx_msg void OnReset();
	afx_msg void OnCropLandscape();
	afx_msg void OnCropPortrait();
	afx_msg void OnCropAspectRatio();
	afx_msg void OnTimer(UINT_PTR idTimer);

private:
	void SetCropAspect();
	void UpdateDimensions();

private:
	CSize m_SizeImage;
	CComboBox m_comboAspectRatio;
	CEdit m_editLeft;
	CEdit m_editTop;
	CEdit m_editRight;
	CEdit m_editBottom;
	CStatic m_staticOriginalDimensions;
	CStatic m_staticNewDimensions;
	CRect m_CropRect;
	CButton m_radioPortrait;
	CButton m_radioLandscape;
};
