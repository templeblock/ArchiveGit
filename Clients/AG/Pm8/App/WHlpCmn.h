//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHlpCmn.h 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/WHlpCmn.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 15    9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 14    9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 13    6/04/98 2:12p Rlovejoy
// Added flag to allow user to avoid updating the description string.
// 
// 12    6/03/98 9:46p Hforman
// add label previewing
// 
// 11    6/03/98 7:07p Rlovejoy
// Code to display description string.
// 
// 10    6/01/98 11:31p Hforman
// fixed control IDs, etc.
// 
// 9     6/01/98 8:22p Hforman
// moved preview window to CWHelpPropertyPage
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __WHLPCMN_H__
#define __WHLPCMN_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "pmw.h"
#include "oriendlg.h"
#include "WHlpWiz.h"
#include "pictprvw.h"
#include "labels.h"

/////////////////////////////////////////////////////////////////////////////
// CWithHelpPreview
class CWithHelpPreview : public CProjectMakePreview
{
public:
	enum PreviewTypes {
		EMPTY_PREVIEW,
		CBITMAP_PREVIEW,
		TEMPLATE_PREVIEW,
		LABEL_PREVIEW
	};
	PreviewTypes m_PreviewType;
	CPicturePreview m_ppDocPict;
	CLabelData* m_pLabelData;

// Construction
public:
	CWithHelpPreview();
	virtual ~CWithHelpPreview();
	
	void SetType(enum PreviewTypes type)
	{
		m_PreviewType = type;
	}
	BOOL VerifyUpdateCBitmap(const char *cnName);

public:
	BOOL InitializeDocPreview(CWnd* pDialog, UINT uPrevID, UINT uDescID = 0);
	void UpdateDocPreview(CPmwDoc* pDoc);
	void SizePreview();
	void ForcePreviewToFront(BOOL fForce);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWithHelpPreview)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


/////////////////////////////////////////////////////////////////////////////
// CWHelpPropertyPage

class CWHelpPropertyPage : public CPropertyPage
// All property pages in the With Help wizard are derived from this class.
// Pages with list selection and preview are derived from CWHelpCommonPropertyPage
{
// Construction
public:
	CWHelpPropertyPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem, UINT nIDTemplate);
	~CWHelpPropertyPage();

// Dialog Data
	//{{AFX_DATA(CWHelpPropertyPage)
	enum { IDD = IDD_WH_BASE_PAGE };
	CListBox	m_listChoices;
	CString	m_strTopText;
	CString	m_strLowerText;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWHelpPropertyPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Attributes
public:

	void UpdatePreview(BOOL fShowDescription = TRUE);
	void SetCurSelection(int nSelection)	{ m_nCurSelection = nSelection; }

	CWHelpWizPropertySheet* GetSheet() { return (CWHelpWizPropertySheet*)GetParent(); }

// Implementation
protected:
	CWHelpPageContainer* m_pContainer;
	UINT					m_nIDContainerItem;
	CPmwDoc*				m_pDoc;					// document being worked on
	CWithHelpPreview	m_preview;				// preview box

	static int			m_nCurSelection;		// determines which preview to show

	BOOL					m_fRedrawPreview;
	BOOL					m_fEliminateProjectTypes;

	void OpenDocument(CString& csDocName, BOOL fExternal = FALSE);
	void CloseDocument();
	void DrawFigure(CDC* pDC);

protected:
	// Generated message map functions
	//{{AFX_MSG(CWHelpPropertyPage)
   afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CWHelpCommonPropertyPage dialog

class CWHelpCommonPropertyPage : public CWHelpPropertyPage
{
// Construction
public:
	CWHelpCommonPropertyPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem, UINT nIDTemplate = CWHelpCommonPropertyPage::IDD);
	~CWHelpCommonPropertyPage();

// Dialog Data
	//{{AFX_DATA(CWHelpCommonPropertyPage)
	enum { IDD = IDD_WH_PROPERTY_PAGE };
	CListBox	m_listChoices;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWHelpCommonPropertyPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Attributes
public:
	void EliminateProjectTypes() { m_fEliminateProjectTypes = TRUE; }

protected:

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWHelpCommonPropertyPage)
	afx_msg void OnSelchangeChoiceList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __WHLPCMN_H__
