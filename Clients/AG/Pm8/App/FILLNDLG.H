/*
// $Header: /PM8/App/FILLNDLG.H 1     3/03/99 6:05p Gbeddow $
//
// fillndlg.h : header file
//
// $Log: /PM8/App/FILLNDLG.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:37:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:18   FRED
// Initial revision.
// 
//    Rev 1.2   26 Jul 1995 10:52:20   FRED
// New Address Book interface
// 
//    Rev 1.1   11 Jul 1995 14:18:56   JAY
// Fill-in fields dialog.
// 
//    Rev 1.0   11 Jul 1995 09:42:24   JAY
// Initial revision.
*/

#ifndef __FILLNDLG_H__
#define __FILLNDLG_H__

#include "macrodlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFillInDialog dialog

class CFillInDialog : public CSmartFieldsDialog
{
// Construction
public:
	CFillInDialog(CPmwView* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFillInDialog)
	enum { IDD = IDD_FILL_IN_FIELDS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual BOOL CanBeginEdit(int nItem, int nField);
	virtual CSmartFieldListBox* CreateListBox(void);

	CPmwView* m_pView;

	// Generated message map functions
	//{{AFX_MSG(CFillInDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnReceivers();
	afx_msg void OnSender();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CFillInFieldListBox : public CSmartFieldListBox
{
	INHERIT(CFillInFieldListBox, CSmartFieldListBox)

public:
	CFillInFieldListBox();
protected:
	virtual BOOL OnFinishEdit(BOOL fValidate = TRUE, BOOL fByReturn = FALSE);
	virtual BOOL CanEditItem(int nItem, int nField);
	virtual BOOL ShowMacro(CMacro* pMacro);
};

#endif
