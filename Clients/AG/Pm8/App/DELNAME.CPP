/*
// $Workfile: DELNAME.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DELNAME.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.1   01 Oct 1997 13:47:56   johno
// Removed duplicate ClassWizard (AFX_) block
// 
//    Rev 1.0   14 Aug 1997 15:20:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:04   Fred
// Initial revision.
// 
//    Rev 1.4   14 Oct 1996 11:04:24   Jay
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

#include "stdafx.h"
#include "resource.h"
#include "delname.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteNameDialog dialog

CConfirmDeleteNameDialog::CConfirmDeleteNameDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CConfirmDeleteNameDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmDeleteNameDialog)
	m_csName = _T("");
	//}}AFX_DATA_INIT
}


void CConfirmDeleteNameDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmDeleteNameDialog)
	DDX_Text(pDX, IDC_NAME, m_csName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmDeleteNameDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CConfirmDeleteNameDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteNameDialog message handlers

BOOL CConfirmDeleteNameDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteAllNamesDialog dialog

CConfirmDeleteAllNamesDialog::CConfirmDeleteAllNamesDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CConfirmDeleteAllNamesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmDeleteAllNamesDialog)
	//}}AFX_DATA_INIT
}


void CConfirmDeleteAllNamesDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmDeleteAllNamesDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmDeleteAllNamesDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CConfirmDeleteAllNamesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteAllNamesDialog message handlers

BOOL CConfirmDeleteAllNamesDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();

#ifndef WIN32
	CenterWindow();
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteSenderDialog dialog

CConfirmDeleteSenderDialog::CConfirmDeleteSenderDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CConfirmDeleteSenderDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmDeleteSenderDialog)
	m_csName = _T("");
	//}}AFX_DATA_INIT
}


void CConfirmDeleteSenderDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmDeleteSenderDialog)
	DDX_Text(pDX, IDC_NAME, m_csName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmDeleteSenderDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CConfirmDeleteSenderDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteSenderDialog message handlers

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteAddressBookDialog dialog

CConfirmDeleteAddressBookDialog::CConfirmDeleteAddressBookDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CConfirmDeleteAddressBookDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmDeleteAddressBookDialog)
	m_csName = _T("");
	//}}AFX_DATA_INIT
}

void CConfirmDeleteAddressBookDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmDeleteAddressBookDialog)
	DDX_Text(pDX, IDC_NAME, m_csName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmDeleteAddressBookDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CConfirmDeleteAddressBookDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmDeleteAddressBookDialog message handlers

BOOL CConfirmDeleteAddressBookDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();

#ifndef WIN32
	CenterWindow();
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

