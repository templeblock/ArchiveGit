/*
// $Workfile: PROFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROFILE.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:08   Fred
// Initial revision.
// 
//    Rev 1.2   02 Oct 1996 08:28:18   Jay
// Now TRACEs what it adds.
// 
//    Rev 1.1   16 Sep 1996 12:39:44   Fred
// Profile stuff can be disabled
// 
//    Rev 1.0   13 Sep 1996 09:54:26   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "profile.h"
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////
// CProfile

CProfile::CProfile()
{
	m_fProfiling = FALSE;
}

CProfile::~CProfile()
{
}

void CProfile::Add(LPCSTR pszLabel)
{
	Add(pszLabel, timeGetTime());
}

void CProfile::Add(LPCSTR pszLabel, DWORD dwValue)
{  
	if (m_fProfiling)
	{
		CString csItem;
		csItem.Format("%ld.%03ld   %s", dwValue/1000, dwValue%1000, pszLabel);
		TRACE("%s\n", (LPCSTR)csItem);
		m_Profile.Add(csItem);
	}
}

void CProfile::Show()
{
	CProfileShowDialog Dialog(this);
	Dialog.DoModal();
}

void CProfile::Clear()
{
	m_Profile.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// CProfileShowDialog dialog


CProfileShowDialog::CProfileShowDialog(CProfile* pProfile, CWnd* pParent /*=NULL*/)
	: CDialog(CProfileShowDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProfileShowDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pProfile = pProfile;
}


void CProfileShowDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileShowDialog)
	DDX_Control(pDX, IDC_PROFILE_LIST, m_ProfileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileShowDialog, CDialog)
	//{{AFX_MSG_MAP(CProfileShowDialog)
	ON_BN_CLICKED(IDC_PROFILE_CLEAR, OnProfileClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileShowDialog message handlers

BOOL CProfileShowDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();

	BuildProfileList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProfileShowDialog::BuildProfileList(void)
{
	m_ProfileList.SetRedraw(FALSE);
	m_ProfileList.ResetContent();

	for (int nString = 0; nString < m_pProfile->GetProfileStrings().GetSize(); nString++)
	{
		m_ProfileList.AddString(m_pProfile->GetProfileStrings().ElementAt(nString));
	}

	m_ProfileList.SetCurSel(-1);
	m_ProfileList.SetRedraw(TRUE);
	m_ProfileList.Invalidate();
}

void CProfileShowDialog::OnProfileClear() 
{
	m_pProfile->Clear();
	BuildProfileList();
}
