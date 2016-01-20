#pragma once

#include "resource.h"
#include "BaseTab.h"
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
	afx_msg void OnCropApply();
	afx_msg void OnCropLandscape();
	afx_msg void OnCropPortrait();

private:
	CComboBox m_comboAspectRatio;
	CEdit m_editLeft;
	CEdit m_editTop;
	CEdit m_editRight;
	CEdit m_editBottom;
	CStatic m_staticOriginalSize;
	CStatic m_staticNewSize;
	afx_msg void OnCbnSelchangeCropAspectratio();
};
