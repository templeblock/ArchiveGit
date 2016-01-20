//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CATALOG.H 1     3/03/99 6:03p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CATALOG.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     4/17/98 6:09p Hforman
// finishing up Win95-style open dialog
// 
//////////////////////////////////////////////////////////////////////////////

//#define SPECIAL_CATALOG

#include "OpenDlgs.h"

#ifndef SPECIAL_CATALOG
#define DOUBLE_NAMES					// Want double names on picture catalog
#endif


/////////////////////////////////////////////////////////////////////////////
// CPictureCatalogDialog dialog

class CPictureCatalogDialog : public CCustomOpenDlg
{
public:
	CPictureCatalogDialog(CPmwDoc *pDoc, CWnd* pParent, LPCSTR filters);
	~CPictureCatalogDialog();

	void do_print();		// Do the catalog printout

// Dialog Data
	//{{AFX_DATA(CPictureCatalogDialog)
	enum { IDD = IDD_OPEN_PREVIEW_OPTS };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPictureCatalogDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPmwDoc* m_pDoc;
};

/////////////////////////////////////////////////////////////////////////////
// CCatalogPrint dialog

class CCatalogPrint : public CPmwPrint
{
// Construction
public:
	CCatalogPrint(CPmwDoc *pDoc);

// Dialog Data
	//{{AFX_DATA(CCatalogPrint)
	enum { IDD = IDD_CATALOG_PRINT };
	int		m_per_page_index;
	int		m_x_count;
	int		m_y_count;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void update_custom_values(void);

	// Generated message map functions
	//{{AFX_MSG(CCatalogPrint)
	afx_msg void OnClickedCatalogCustom();
	afx_msg void OnChangeCustom();
	afx_msg void OnClickedPerPage();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CCatalogView view

class CCatalogView : public CPmwView
{
public:
	CCatalogView(const CStringArray& picture_list, LPCSTR chosen_dir);
	virtual ~CCatalogView();

// Attributes
public:
	const CStringArray& m_picture_list;
	LPCSTR m_chosen_dir;

// Operations
public:

	VOID set_document(CDocument *doc)
		{ m_pDocument = doc; }

// Implementation
protected:
//	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view

	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	// Generated message map functions
protected:
	//{{AFX_MSG(CCatalogView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCatalogDoc document

class CCatalogDoc : public CPmwDoc
{
	DECLARE_SERIAL(CCatalogDoc)
public:
	CCatalogDoc();			// protected constructor used by dynamic creation
	virtual ~CCatalogDoc();
	VOID add_hidden_view(CCatalogView *view);

// Attributes
public:

// Operations
public:

// Implementation
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CCatalogDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
