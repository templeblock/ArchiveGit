/*
// $Workfile: DELNAME.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DELNAME.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:32   Fred
// Initial revision.
// 
//    Rev 1.4   14 Oct 1996 11:04:22   Jay
// New sounds.
// 
//    Rev 1.3   08 Sep 1996 09:23:16   Fred
// Confirm delete address book; New classes which center CDialog based dialogs.
// 
//    Rev 1.2   09 Aug 1996 17:23:10   Fred
// PmwDialog
// 
//    Rev 1.1   02 Jul 1996 10:56:32   Fred
// Sender Information
// 
//    Rev 1.0   01 Jul 1996 07:38:06   Fred
// Initial revision.
*/

class CConfirmDeleteNameDialog;
class CConfirmDeleteAllNamesDialog;
class CConfirmDeleteSenderDialog;
class CConfirmDeleteAddressBookDialog;

#ifndef __DELNAME_H__
#define __DELNAME_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteNameDialog dialog

class CConfirmDeleteNameDialog : public CPmwDialog
{
	INHERIT(CConfirmDeleteNameDialog, CPmwDialog)
	
// Construction
public:
	CConfirmDeleteNameDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmDeleteNameDialog)
	enum { IDD = IDD_CONFIRM_DELETE_NAME };
	CString	m_csName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmDeleteNameDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmDeleteNameDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteAllNamesDialog dialog

class CConfirmDeleteAllNamesDialog : public CPmwDialog
{
	INHERIT(CConfirmDeleteAllNamesDialog, CPmwDialog)
	
// Construction
public:
	CConfirmDeleteAllNamesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmDeleteAllNamesDialog)
	enum { IDD = IDD_CONFIRM_DELETE_ALL_NAMES };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmDeleteAllNamesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmDeleteAllNamesDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteSenderDialog dialog

class CConfirmDeleteSenderDialog : public CPmwDialog
{
	INHERIT(CConfirmDeleteSenderDialog, CPmwDialog)
	
// Construction
public:
	CConfirmDeleteSenderDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmDeleteSenderDialog)
	enum { IDD = IDD_CONFIRM_DELETE_SENDER };
	CString	m_csName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmDeleteSenderDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmDeleteSenderDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteAddressBookDialog dialog

class CConfirmDeleteAddressBookDialog : public CPmwDialog
{
	INHERIT(CConfirmDeleteAddressBookDialog, CPmwDialog)
	
// Construction
public:
	CConfirmDeleteAddressBookDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmDeleteAddressBookDialog)
	enum { IDD = IDD_CONFIRM_DELETE_ADDRESS_BOOK };
	CString	m_csName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmDeleteAddressBookDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmDeleteAddressBookDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
