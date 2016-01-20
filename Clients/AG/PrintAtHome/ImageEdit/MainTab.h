#pragma once

#include "resource.h"
#include "BaseTab.h"
#include "HyperLink.h"

// CMainTab dialog

class CMainTab : public CBaseTab
{
public:
	CMainTab(CWnd* pParent = NULL);
	virtual ~CMainTab();

	enum { IDD = IDD_MAIN_TAB };

protected:
	DECLARE_DYNAMIC(CMainTab)
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void StandardizeHyperLink(CHyperLink& HyperLink, HICON hIcon = NULL);

private:
	afx_msg void OnBorders();
	afx_msg void OnColor();
	afx_msg void OnContrastBrightness();
	afx_msg void OnCrop();
	afx_msg void OnEffects();
	afx_msg void OnFile();
	afx_msg void OnFlipRotate();
	afx_msg void OnRedEye();
	afx_msg void OnRecolor();
	afx_msg void OnResize();
	afx_msg void OnTest();

private:
	CHyperLink m_Borders;
	CHyperLink m_Color;
	CHyperLink m_ContrastBrightness;
	CHyperLink m_Crop;
	CHyperLink m_Effects;
	CHyperLink m_File;
	CHyperLink m_FlipRotate;
	CHyperLink m_RedEye;
	CHyperLink m_Recolor;
	CHyperLink m_Resize;
};
