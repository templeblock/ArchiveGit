//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/SvBorder.h 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/SvBorder.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 3     4/16/98 7:06p Hforman
// add categories
// 
// 2     2/11/98 5:54p Hforman
// finished implementing
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __SVBORDER_H__
#define __SVBORDER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "UsrCtgry.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveBorderDlg dialog

class CSaveBorderDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CSaveBorderDlg)

public:
	CSaveBorderDlg(LPCTSTR szDefExt, LPCTSTR szFileName, LPCTSTR szFilter,
						LPCTSTR szInitDir, CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CSaveBorderDlg)
	enum { IDD = IDD_SAVE_BORDER_OPTS };
	CComboBox	m_CategoryCombo;
	CButton	m_btnNewCategory;
	CString	m_strCategory;
	BOOL	m_fAddToGallery;
	//}}AFX_DATA

protected:
	CUserCategoryManager		m_CategoryManager;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnFileNameOK();
   //{{AFX_MSG(CSaveBorderDlg)
	afx_msg void OnAddToGallery();
	virtual BOOL OnInitDialog();
	afx_msg void OnNewCategory();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SVBORDER_H__
