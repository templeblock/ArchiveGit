//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PhPrjDlg.h 1     3/03/99 6:09p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/PhPrjDlg.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 3     2/24/99 9:14p Psasse
// fixed a memory leak
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     12/24/98 4:58p Psasse
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __PHPRJDLG_H__
#define __PHPRJDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PhPrjDlg.h : header file
//
#include "photoprj.h"
#include "pdlgclr.h"

//class CPhotoProjectPreview;

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectsTypeDlg dialog

class CPhotoProjectsTypeDlg : public CPmwDialogColor
{
// Construction
public:
	CPhotoProjectsTypeDlg(CPaperInfo* pOldInfo, CWnd* pParent = NULL, DWORD WizFlags = WIZBUT_OK | WIZBUT_CANCEL | WIZBUT_HELP, COLORREF cr = INVALID_DIALOG_COLOR);	// standard constructor
	virtual ~CPhotoProjectsTypeDlg();

// Dialog Data
	//{{AFX_DATA(CPhotoProjectsTypeDlg)
	enum { IDD = IDD_PHOTO_PROJECT_TYPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CPhotoLabelData* GetChosenPhotoProject(void);
	
	PHOTO_PROJECT_TYPE	GetChosenType()		{ return m_PhotoProjectType; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhotoProjectsTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD       m_WizFlags;
	PHOTO_PROJECT_TYPE			m_PhotoProjectType;

	void BuildList(void);

	// Generated message map functions
	//{{AFX_MSG(CPhotoProjectsTypeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePhotoProjectsList();
	afx_msg void OnPhotoProjectTypeChanged();
	virtual void OnOK();
	afx_msg void OnDblclkPhotoProjectsList();
	afx_msg void OnBack();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	void UpdatePreview(void);
	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);

	CPhotoPrjList m_List;
	//CPhotoProjectPreview* m_pPreview;
	CPaperInfo* m_pOldInfo;

	CPhotoPrjData* m_pChosenPhotoProject;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __PHPRJDLG_H__
