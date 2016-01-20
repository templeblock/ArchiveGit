// posdlg.h : header file
//

#include "layrec.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPositionDialog dialog

FARCLASS LayoutEntry : public ListElement
{
private:
	CString m_csName;
	List elements;
public:
/* Constructor. */

	LayoutEntry(LPSTR name);

/* Destructor. */

	~LayoutEntry();

/* Get the list of elements for this layout. */

	ListPtr get_elements(VOID)
		{ return &elements; }

/* Get the n'th layout element in this layout. */

	LayoutElementPtr get_element(SHORT which)
		{ return (LayoutElementPtr)elements.element_in_list(which); }

/* Add a layout element to this layout. */

	VOID add_element(LayoutElementPtr element)
		{ elements.append(element); }

/* Return the name of this layout entry. */

	LPCSTR get_name(VOID)
 		{ return m_csName; }

/* Set the name of this layout entry. */

	VOID set_name(LPCSTR n);
};
typedef LayoutEntry far *LayoutEntryPtr;

class LayoutEntryList : public List
{
public:
	LayoutEntryList();

/* Read a layout list. */

	ERRORCODE read_layout_list(LPCSTR layout_file_name);

/* Read the layout list based on a passed string id. */

	ERRORCODE read_layout_list(int nLayoutId);

/* Read the layout list based on a passed CPmwDoc. */

	ERRORCODE read_layout_list(CPmwDoc* pDoc, BOOL fDuping = FALSE);
};

class CPositionDialog : public CPmwDialog
{
private:
// Construction
public:
	CPositionDialog(CPmwDoc *pDoc, BOOL duping = FALSE, CWnd* pParent = NULL);	// standard constructor

// Dialog Data

	CPmwDoc *m_pDoc;
	LayoutEntryList m_layouts;
	PMGPageObjectPtr m_object;
	PPNT m_doc_dims;
	BOOL m_duping;

	//{{AFX_DATA(CPositionDialog)
	enum { IDD = IDD_OBJECT_POSITION };
	CButton	m_preview;
	CListBox	m_layout_list;
	int		m_layout_number;
	//}}AFX_DATA

// Implementation

	virtual int DoModal();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	VOID init_preview(VOID);
	void preview_layout(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// Generated message map functions
	//{{AFX_MSG(CPositionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkStandardPositions();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangePositions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

