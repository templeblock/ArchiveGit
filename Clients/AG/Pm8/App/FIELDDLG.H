/*
// $Workfile: FIELDDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FIELDDLG.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
//    Rev 1.0   14 Aug 1997 15:20:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:37:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:14   FRED
// Initial revision.
// 
//    Rev 1.4   31 Aug 1995 10:04:18   FRED
// Miscellaneous fixes.
// 
//    Rev 1.3   30 Aug 1995 11:41:16   FRED
// Field name validation
// 
//    Rev 1.2   25 Aug 1995 18:38:10   JAY
// Receiver and sender address blocks.
// 
//    Rev 1.1   25 Aug 1995 15:00:48   JAY
// Insert fields dialog
// 
//    Rev 1.0   25 Aug 1995 09:18:34   JAY
// Initial revision.
*/

#ifndef __FIELDDLG_H__
#define __FIELDDLG_H__

#include "pmwdlg.h"
#include "macrorec.h"

class CPmwView;

/////////////////////////////////////////////////////////////////////////////
// CInsertFieldHelper class
// This is an abstract class used by the Insert Field dialog to interface to
// the rest of the world.

class CInsertFieldHelper
{
public:
	virtual ~CInsertFieldHelper() {}

	virtual void InsertMacro(int nMacro) = 0;
	virtual void InsertText(CHARACTER* pText, int nSize) = 0;
	virtual void ReflowMacros(void) = 0;
	virtual CMacroList& MacroList(void) = 0;
	virtual void GetMacrosInUse(CWordArray& cwaMacrosInUse) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CInsertFieldViewHelper class
// This is a standard class used to interface a CPmwView to the Insert Field
// dialog.

class CInsertFieldViewHelper : public CInsertFieldHelper
{
public:
	CInsertFieldViewHelper(CPmwView* pView);

	virtual void InsertMacro(int nMacro);
	virtual void InsertText(CHARACTER* pText, int nSize);
	virtual void ReflowMacros(void);
	virtual CMacroList& MacroList(void);
	virtual void GetMacrosInUse(CWordArray& cwaMacrosInUse);
protected:
	CPmwView* m_pView;
};

/////////////////////////////////////////////////////////////////////////////
// CInsertFieldDialog dialog

class CInsertFieldDialog : public CPmwDialog
{
// Construction
public:
	CInsertFieldDialog(CInsertFieldHelper* pHelper, BOOL fMergeFieldsOnly = FALSE, UINT uID = (UINT)-1, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CInsertFieldDialog)
	enum { IDD = IDD_INSERT_FIELD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInsertFieldDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddField();
	afx_msg void OnSelchangeFieldList();
	afx_msg void OnDblclkFieldList();
	afx_msg void OnInsertField();
	afx_msg void OnInsertReceiver();
	afx_msg void OnInsertSender();
	afx_msg void OnRemoveField();
	afx_msg void OnRenameField();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void BuildList(int nCurSel = -1);
	void UpdateButtons(void);
	BOOL MacroInUse(CMacro* pMacro);
	int FindMacro(LPCSTR pName, CMacro* pSkipMacro = NULL);
	BOOL MacroNameIsUnique(LPCSTR pName, CMacro* pSkipMacro = NULL);
	void UpdateFields(void);
	void InsertBlock(LPCSTR* pStrings);

	CInsertFieldHelper* m_pHelper;
	CMacroList m_MacroList;
	CWordArray m_cwaMacrosInUse;
	BOOL m_fChangedMacroList;
	BOOL m_fMergeFieldsOnly;
	
	static CString m_csLastFieldName;
};

/////////////////////////////////////////////////////////////////////////////
// CFieldNameDialog dialog

class CFieldNameDialog : public CPmwDialog
{
// Construction
public:
	CFieldNameDialog(WORD wIDD, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFieldNameDialog)
	enum { IDD = IDD_FIELD_ADD };
	CString	m_csName;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFieldNameDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFieldName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	void UpdateButtons(void);
	BOOL ValidateFieldName(CString& csName);
};
#endif
