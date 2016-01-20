//////////////////////////////////////////////////////////////////////////////
// $Workfile: SAVEPROJ.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/SAVEPROJ.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     4/16/98 6:57p Hforman
// add categories to dialog
// 
// 2     4/13/98 2:37p Hforman
// using Common Dialog, added categories, etc.
// 
//    Rev 1.0   14 Aug 1997 15:25:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:50   Fred
// Initial revision.
// 
//    Rev 1.2   07 May 1997 17:21:54   Jay
// More save project tweaks.
// 
//    Rev 1.1   06 May 1997 10:30:10   Jay
// New browser stuff.
// 
//    Rev 1.0   28 Mar 1997 17:11:38   Jay
// Ripped out albums, sound manager, side bar, etc.
//////////////////////////////////////////////////////////////////////////////

#ifndef __SAVEPROJ_H__
#define __SAVEPROJ_H__

#include "UsrCtgry.h"

class CSaveProjectDialog : public CFileDialog
{
public:
	CSaveProjectDialog(LPCTSTR szExt, LPCTSTR szFileName, LPCTSTR szFilter,
							 LPCTSTR szInitialDir, CPmwDoc* pDoc);

	virtual ~CSaveProjectDialog();
	//{{AFX_DATA(CSaveProjectDialog)
	enum { IDD = IDD_SAVE_PROJECT_OPTS };
	CButton	m_btnNewCategory;
	CComboBox	m_CategoryCombo;
	BOOL	m_fAddToGallery;
	BOOL	m_fEmbedPictures;
	CString	m_csCategory;
	//}}AFX_DATA

protected:
	CUserCategoryManager		m_CategoryManager;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnFileNameOK();
	// Generated message map functions
	//{{AFX_MSG(CSaveProjectDialog)
	afx_msg void OnAddToGallery();
	afx_msg void OnNewCategory();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
