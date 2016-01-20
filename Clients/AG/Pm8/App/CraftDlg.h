//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CraftDlg.h 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/CraftDlg.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     8/26/98 8:12p Hforman
// added code for getting thumbnails out of project collection
// 
// 2     8/20/98 12:51p Hforman
// added m_CraftType, changed contents of CrTypes.ini
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __CRAFTDLG_H__
#define __CRAFTDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CraftDlg.h : header file
//
#include "labels.h"
#include "pdlgclr.h"
#include "pictprvw.h"
#include "inifile.h"
#include "project.h"

/////////////////////////////////////////////////////////////////////////////
// CCraftTypeDlg dialog

class CCraftTypeDlg : public CPmwDialogColor
{
// Construction
public:
	CCraftTypeDlg(CWnd* pParent = NULL, DWORD WizFlags = WIZBUT_OK | WIZBUT_CANCEL | WIZBUT_HELP, COLORREF cr = INVALID_DIALOG_COLOR);	// standard constructor
	virtual ~CCraftTypeDlg();

// Dialog Data
	//{{AFX_DATA(CCraftTypeDlg)
	enum { IDD = IDD_CRAFT_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString GetChosenCraft(void);
	CRAFT_TYPE	GetChosenType()		{ return m_CraftType; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCraftTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void BuildList(void);
	void UpdatePreview(void);
	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);
	BOOL GetFinalThumbnailSize(int nThumbWidth, int nThumbHeight, CSize* pSize);

	// Generated message map functions
	//{{AFX_MSG(CCraftTypeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCraftList();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDblclkCraftList();
	afx_msg void OnBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	DWORD       m_WizFlags;

	CPicturePreview	m_preview;
	LPBITMAPINFO		m_pThumbnailData;
	int					m_nPreviewWidth;		// original dims of preview window
	int					m_nPreviewHeight;

	CString				m_csSelectedProject;
	CRAFT_TYPE			m_CraftType;
	CString				m_csIniFile;
	CIniFile				m_iniFile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CRAFTDLG_H__
