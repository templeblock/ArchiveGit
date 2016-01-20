//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CatOrgnz.h 1     3/03/99 6:03p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CatOrgnz.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     7/26/98 7:21p Hforman
// changed baseclass to CPmwDialog to handle palette changes
// 
// 4     5/19/98 3:09p Hforman
// 
// 3     5/06/98 6:06p Hforman
// More functionality added -- more to come
// 
// 2     4/30/98 7:03p Hforman
// initial implementation - more to come
// 
// 1     4/16/98 10:12a Hforman
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __CATORGNZ_H__
#define __CATORGNZ_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCategoryOrganizerDlg dialog
//
// NOTE: this implementation is currently tied to the parent being a
// CBrowserDialog. Much functionality will have to be pulled out of
// the browser in order for this to be a stand-alone dialog. (hforman)
/////////////////////////////////////////////////////////////////////////////

// forward declarations
class CPMWCollection;
class CBrowserDialog;

class CCategoryOrganizerDlg : public CPmwDialog
{
// Construction/destruction
public:
	CCategoryOrganizerDlg(CPMWCollection* pCollection, CBrowserDialog* pParent);
	virtual ~CCategoryOrganizerDlg();

// Dialog Data
	//{{AFX_DATA(CCategoryOrganizerDlg)
	enum { IDD = IDD_CATEGORY_ORGANIZER };
	CButton	m_btnMove;
	CTreeCtrl	m_CategoryTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCategoryOrganizerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void					FillCategoryTree();
	BOOL					GetItemNames(DWORD dwItem, BOOL fFullPath, CString& strFileName, CString& strFriendlyName);
	void					GetCategories(CStringArray& CatArray);
	void					MoveItemToNewParent(HTREEITEM hItem, HTREEITEM hNewParent);

	CPMWCollection*	m_pCollection;
	CBrowserDialog*	m_pBrowser;		// parent browser

	CCategoryDatabase* m_pCategoryDB;
	CCategoryDir*		m_pCategoryDir;
	CFileItems*			m_pFileItems;
	BOOL					m_fProjects;			// are we looking at projects or art?
	BOOL					m_fSomethingChanged;	// has the user changed anything?

	static UINT			m_ProjIconIDs[PROJECT_TYPE_Last];

	CImageList			m_ImageList;		// images for tree items
	CUIntArray			m_ProjImageIDs;	// array of IDs for project icons

	BOOL					m_fDragging;		// currently dragging?
	HTREEITEM			m_hItemDrag;		// item being dragged
	HTREEITEM			m_hItemDrop;		// item dropped onto
	CImageList*			m_pDragImage;		// drag images

	// Generated message map functions
	//{{AFX_MSG(CCategoryOrganizerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMove();
	afx_msg void OnNew();
	afx_msg void OnRename();
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchangedCategoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMoveToCatDlg dialog

class CMoveToCatDlg : public CPmwDialog
{
// Construction
public:
	CMoveToCatDlg(const CStringArray& categories, const CString& strItemName, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CMoveToCatDlg)
	enum { IDD = IDD_CATEGORY_MOVE };
	CListBox	m_CategoryList;
	CString	m_strMessage;
	CString	m_strCategory;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMoveToCatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CStringArray	m_aryCategories;
	CString			m_strItemName;

	// Generated message map functions
	//{{AFX_MSG(CMoveToCatDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CATORGNZ_H__
