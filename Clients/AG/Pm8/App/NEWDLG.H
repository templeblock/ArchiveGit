// newdlg.h : header file
//

#include "pdlgclr.h"
#include "ownerlb.h"

class CNewDlgListbox : public COwnerDrawListBox
{
	INHERIT (CNewDlgListbox, COwnerDrawListBox)

public:
   CNewDlgListbox ();
   virtual ~CNewDlgListbox ();

   BOOL 
   SubclassDlgItem(UINT nID, CWnd* pParent);

   BOOL        m_AllItemDataPtrsValid;

protected:

	// Generated message map functions
	//{{AFX_MSG(CNewDlgListbox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
	virtual void   MeasureItem (LPMEASUREITEMSTRUCT lpMIS);
	virtual void   DrawItem (LPDRAWITEMSTRUCT lpDIS);
	virtual int    CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
   virtual void   DeleteItem (LPDELETEITEMSTRUCT pDeleteItemStruct);

   TEXTMETRIC  m_tm;
};

/////////////////////////////////////////////////////////////////////////////
// CNewDialog dialog

class CNewDialog : public CPmwDialogColor
{
// Construction
private:
	CPtrList*   m_pList;        // actually a list of doc templates
	CDocTemplate *m_template;

public:
// standard constructor
	CNewDialog(CPtrList* pList, NEWINFO_REF info, UINT id = 0, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CNewDialog)
	enum { IDD = IDD_NEW_DIALOG };
	int		m_nIndex;
	int		m_subtype;
	int		m_orientation;
	int		m_nPanels;
	//}}AFX_DATA

	int m_nType;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

   CNewDlgListbox m_NewListbox;
	void
	SizeToListboxContent(void);

	// Generated message map functions
	//{{AFX_MSG(CNewDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeNewList();
	afx_msg void OnFinish();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	VOID get_exit_info(NEWINFO_PTR info);
};
