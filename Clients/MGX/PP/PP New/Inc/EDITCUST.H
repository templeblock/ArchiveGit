//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
// editcust.h : header file
//

/***************************************************************************/
/*                                  CToolList                              */
/***************************************************************************/
#ifndef _EDITCUST_H_
#define _EDITCUST_H_   
class CCustToolBox;
class CCustTool;

class CToolList : public CListBox
{
// Construction
public:
	CToolList();	// standard constructor
   BOOL FillIt(HWND hDlg, int ID);
};

/***************************************************************************/
/*                                  CCategoryList                          */
/***************************************************************************/
   
class CCategoryList : public CComboBox
{
// Construction
public:
	CCategoryList();	// standard constructor
   BOOL FillIt();
};

/***************************************************************************/
/*                            CEditCustTool dialog                         */
/***************************************************************************/

class CEditCustTool : public CDialog
{
// Construction
public:
	CEditCustTool(CCustToolBox *ptb, CWnd* pParent = NULL);	// standard constructor
	~CEditCustTool();

// Dialog Data
	//{{AFX_DATA(CEditCustTool)
	enum { IDD = IDD_EDITCUSTOOL };
	CEdit	         m_ToolName;
	CEdit	         m_Width;
	CToolList      m_Tools;
	CButton	      m_Delete;
	CCategoryList	m_Category;
	CButton	      m_Add;
	int	         m_CatVal;
	int	         m_BoxVal;
	int	         m_ListVal;
	CString	      m_NameVal;
	int    	      m_WidthVal;
   BOOL           m_ColorPatch;
   int            m_LastIcon;
   BOOL           m_LastSelectedIcon;
   CCustToolBox *m_pToolBox;
   CCustToolBox *m_Original;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
   CCustTool *CreateNewTool();

	// Generated message map functions
	//{{AFX_MSG(CEditCustTool)
           void SortList(BOOL);
           void UpdateEditName(CWnd *pListBox);
   afx_msg void OnMoveItemTools();
	afx_msg void OnAdd();
	afx_msg void OnInsert();
	afx_msg void OnSelchangeCategory();
	afx_msg void OnDel();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   afx_msg void OnMoveItem();
	afx_msg void OnSelchangeToolboxTools();
	afx_msg void OnSelchangeToolbox();
	afx_msg void OnSelchangeToollist();
	afx_msg void OnKillfocusToolname();
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDraw);
   afx_msg void OnSpin();
   afx_msg void OnColorPatch();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
   BOOL    FillToolBox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CRenameCustTool : public CDialog
{
// Construction
public:
	CRenameCustTool(CString *Name);	// standard constructor
	~CRenameCustTool();

// Dialog Data
	//{{AFX_DATA(CEditCustTool)
	enum { IDD = IDD_RENAMECUSTOOL };
	CEdit	         m_ToolName;
	CString	     *m_pNameVal;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CEditCustTool)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
