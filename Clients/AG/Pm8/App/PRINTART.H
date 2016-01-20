/*
// $Workfile: PRINTART.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PRINTART.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 5     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 4     8/18/98 2:30p Johno
// Added GetManualPage() and SetManualPage() - allows for text in manual
// page string
// 
// 3     8/11/98 12:23p Johno
// Changed for OnManualPrintDocument()
// 
// 2     5/22/98 11:18a Fredf
// Ported 4.0 code over to print image index.
// 
//    Rev 1.2   02 Sep 1997 17:36:30   Jay
// New manual print support
// 
//    Rev 1.0   14 Aug 1997 15:24:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:28   Fred
// Initial revision.
// 
//    Rev 1.1   23 May 1997 14:12:48   doberthur
// 
// 
//    Rev 1.0   22 May 1997 10:59:54   doberthur
// Initial revision.
// 
*/

// printart.h : header file
//

#ifndef __PRINTART_H__
#define __PRINTART_H__

#define DOUBLE_NAMES					// Want double names on art printout
#define JUST_PRINT_FRIENDLY_NAME  // print friendly name but not file name

extern CString GetPrintCategory(LPCSTR pszCategory, BOOL fHeader = FALSE);

/*
// An element in the picture list.
*/

class PrintArtEntry : public ListElement
{
private:
	CString m_csName;
	CString m_csFriendlyName;
	CString m_csCategory;
public:
	PrintArtEntry(LPCSTR pszName = NULL, LPCSTR m_pszFriendlyName = NULL, LPCSTR pszCategory = NULL);
	~PrintArtEntry();
	LPCSTR GetName()
		{ return m_csName; }
	void SetName(LPCSTR pszName)
		{ m_csName = (pszName == NULL) ? "" : pszName; }

	LPCSTR GetFriendlyName()
		{ return m_csFriendlyName; }
	void SetFriendlyName(LPCSTR pszFriendlyName)
		{ m_csFriendlyName = (pszFriendlyName == NULL) ? "" : pszFriendlyName; }

	LPCSTR GetCategory(void) const
		{ return m_csCategory; }
	void SetCategory(LPCSTR pszCategory)
		{ m_csCategory = (pszCategory == NULL) ? "" : pszCategory; }
};
typedef PrintArtEntry far *PrintArtEntryPtr;


/////////////////////////////////////////////////////////////////////////////
// CArtPrintDialog dialog

class CArtPrintDialog : public CPmwPrint
{
// Construction
public:
	CArtPrintDialog(CPmwDoc *pDoc, BOOL fManualPrint = FALSE);

   int GetPrintRange() { return m_selPrintRange; }
	
	int
	GetManualPage(void);
	
	void	
	SetManualPage(int Page);
		
// Dialog Data
	//{{AFX_DATA(CArtPrintDialog)
	enum { IDD = IDD_ART_PRINT };
	int		m_per_page_index;
	int		m_x_count;
	int		m_y_count;
   int  m_selPrintRange;
	CString	m_csManualPage;
	//}}AFX_DATA
	BOOL m_fManualPrint;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void update_custom_values(void);

	// Generated message map functions
	//{{AFX_MSG(CArtPrintDialog)
	afx_msg void OnClickedCatalogCustom();
	afx_msg void OnChangeCustom();
	afx_msg void OnClickedPerPage();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CArtPrintView view

class CArtPrintView : public CPmwView
{
public:
	CArtPrintView(List &picture_list, LPSTR chosen_dir);
	virtual ~CArtPrintView();

// Attributes
public:

	List	&m_picture_list;			// The picture list.
	LPSTR m_chosen_dir;

// Operations
public:

	VOID set_document(CDocument *doc)
		{ m_pDocument = doc; }

// Implementation
protected:
//	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view

	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	SHORT OnManualPrintDocument(PrintContext& pc);
	void SetStyleFace(CTextStyle& Style, LPCSTR pszFace);

	// Generated message map functions
protected:
	//{{AFX_MSG(CArtPrintView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CArtPrintDoc document

class CArtPrintDoc : public CPmwDoc
{
	DECLARE_SERIAL(CArtPrintDoc)
public:
	CArtPrintDoc();		
	virtual ~CArtPrintDoc();
	VOID add_hidden_view(CArtPrintView *view);

// Attributes
public:

// Operations
public:

// Implementation
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CArtPrintDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
