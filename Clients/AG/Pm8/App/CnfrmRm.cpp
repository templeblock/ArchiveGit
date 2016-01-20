////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CnfrmRm.cpp 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CnfrmRm.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     5/13/98 6:56p Hforman
// add CConfirmCategoryRemove
// 
// 2     5/06/98 1:39p Hforman
// 
// 1     5/06/98 1:33p Hforman
// 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "CnfrmRm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfirmFileRemove dialog


CConfirmFileRemove::CConfirmFileRemove(BOOL fProjects, const CString& csFriendlyName, const CString& csFileName, BOOL fDeleteChecked, CWnd* pParent/*=NULL*/)
	: CPmwDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmFileRemove)
	m_strFileName = csFileName;
	m_fDeleteFile = fDeleteChecked;
	//}}AFX_DATA_INIT
	
	m_fProjects = fProjects;
	m_csFriendlyName = csFriendlyName;
	m_csFileName = csFileName;
	m_fDeleteChecked = fDeleteChecked;
}

void CConfirmFileRemove::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmFileRemove)
	DDX_Control(pDX, IDC_CONFIRM_TEXT, m_staticConfirmText);
	DDX_Control(pDX, IDC_DELETE_FILE, m_chkDeleteFile);
	DDX_Text(pDX, IDC_FILE_NAME, m_strFileName);
	DDX_Check(pDX, IDC_DELETE_FILE, m_fDeleteFile);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
		m_fDeleteChecked = IsDlgButtonChecked(IDC_DELETE_FILE);
	}
}

BEGIN_MESSAGE_MAP(CConfirmFileRemove, CPmwDialog)
	//{{AFX_MSG_MAP(CConfirmFileRemove)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDYES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmFileRemove message handlers

BOOL CConfirmFileRemove::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	// set Friendly name in confirm string
	CString csText;
	m_staticConfirmText.GetWindowText(csText);
	CString csNewText;
	csNewText.Format(csText, m_csFriendlyName);
	m_staticConfirmText.SetWindowText(csNewText);

	// put the right text in "Delete file" checkbox
	CString csDeleteFile;
	csDeleteFile.LoadString(m_fProjects ? IDS_DELETE_PROJECT_FILE : IDS_DELETE_IMAGE_FILE);
	m_chkDeleteFile.SetWindowText(csDeleteFile);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CConfirmFileRemove::OnNo()
{
	EndDialog(IDNO);
}

void CConfirmFileRemove::OnYes()
{
	UpdateData(TRUE);
	EndDialog(IDYES);
}


/////////////////////////////////////////////////////////////////////////////
// CConfirmCategoryRemove dialog


CConfirmCategoryRemove::CConfirmCategoryRemove(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmCategoryRemove::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmCategoryRemove)
	m_fDeleteChecked = FALSE;
	//}}AFX_DATA_INIT
}

void CConfirmCategoryRemove::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmCategoryRemove)
	DDX_Check(pDX, IDC_DELETE_FILE, m_fDeleteChecked);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfirmCategoryRemove, CDialog)
	//{{AFX_MSG_MAP(CConfirmCategoryRemove)
	ON_BN_CLICKED(IDYES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmCategoryRemove message handlers

void CConfirmCategoryRemove::OnYes() 
{
	UpdateData(TRUE);
	EndDialog(IDYES);
}
