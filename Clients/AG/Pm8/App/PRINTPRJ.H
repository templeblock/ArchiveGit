/*
// $Workfile: PRINTPRJ.H $
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
// $Log: /PM8/App/PRINTPRJ.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 4     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     8/19/98 12:15p Johno
// Changes to PrintProjectEntry for thumbnail extraction
// 
// 2     8/10/98 6:29p Johno
// Changes for OnManualPrintDocument()
// 
//    Rev 1.0   14 Aug 1997 15:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:28   Fred
// Initial revision.
// 
//    Rev 1.1   01 Jun 1997 16:29:16   doberthur
// 
// 
//    Rev 1.0   30 May 1997 18:35:44   doberthur
// Initial revision.
// 
*/

#ifndef __PRINTPROJECT_H__
#define __PRINTPROJECT_H__

#define DOUBLE_NAMES					// Want double names on art printout
#define JUST_PRINT_FRIENDLY_NAME  // print friendly name but not file name

/*
// An element in the picture list.
*/

FARCLASS PrintProjectEntry : public ListElement
{
private:
	CString	m_csName;
	CString	m_csFriendlyName;
	CString	m_csCategory;
	USHORT	m_uCollectionID;
	DWORD		m_dwItemNumber;
public:
	PrintProjectEntry(LPCSTR pszName = NULL, LPCSTR m_pszFriendlyName = NULL, LPCSTR pszCategory = NULL, USHORT	CollectionID = -1, DWORD dwItemNumber = -1);
	~PrintProjectEntry();
	LPCSTR GetName(void) const
		{ return m_csName; }
	void SetName(LPCSTR pszName)
		{ m_csName = (pszName == NULL) ? "" : pszName; }

	LPCSTR GetFriendlyName(void) const
		{ return m_csFriendlyName; }
	void SetFriendlyName(LPCSTR pszFriendlyName)
		{ m_csFriendlyName = (pszFriendlyName == NULL) ? "" : pszFriendlyName; }

	LPCSTR GetCategory(void) const
		{ return m_csCategory; }
	void SetCategory(LPCSTR pszCategory)
		{ m_csCategory = (pszCategory == NULL) ? "" : pszCategory; }

	USHORT GetCollectionID(void) const
		{ return m_uCollectionID; }

	DWORD	GetItemNumber(void) const
		{ return m_dwItemNumber; }
};
typedef PrintProjectEntry far *PrintProjectEntryPtr;


/////////////////////////////////////////////////////////////////////////////
// CProjectPrintDialog dialog
/*
class CProjectPrintDialog : public CPmwPrint
{
// Construction
public:
	CProjectPrintDialog(CPmwDoc *pDoc);

   int GetPrintRange() { return m_selPrintRange; }

// Dialog Data
	//{{AFX_DATA(CProjectPrintDialog)
	enum { IDD = IDD_PROJECT_PRINT };
	int		m_per_page_index;
	int		m_x_count;
	int		m_y_count;
   int  m_selPrintRange;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void update_custom_values(void);

	// Generated message map functions
	//{{AFX_MSG(CProjectPrintDialog)
	afx_msg void OnClickedCatalogCustom();
	afx_msg void OnChangeCustom();
	afx_msg void OnClickedPerPage();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
*/
/////////////////////////////////////////////////////////////////////////////
// CProjectPrintView view

class CProjectPrintView : public CPmwView
{
public:
	CProjectPrintView(List &picture_list, LPSTR chosen_dir);
	virtual ~CProjectPrintView();

// Attributes
public:
	List	&m_picture_list;			// The picture list.
	LPSTR m_chosen_dir;

// Operations
public:
	VOID set_document(CDocument *doc)
		{ m_pDocument = doc; }
   VOID clear_document()
		{ m_pDocument = NULL; }

// Implementation
protected:
	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);
	SHORT OnManualPrintDocument(PrintContext& pc);
	void SetStyleFace(CTextStyle& Style, LPCSTR pszFace);
	
		// Generated message map functions
protected:
	//{{AFX_MSG(CProjectPrintView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
