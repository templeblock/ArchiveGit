//****************************************************************************
//
// File Name:  CompDegree.cpp
//
// Project:    Renaissance Headline Component
//
// Author:     Mike Heydlauf
//
// Description: Methods for the compensation degree dialog
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/CompDegree.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************


#include "HeadlineIncludes.h"
ASSERTNAME

//#include "CompDegree.h"
#include "EditHeadlineDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCompDegree dialog


CCompDegree::CCompDegree(CWnd* pParent /*=NULL*/)
	: CDialog(CCompDegree::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompDegree)
	m_flCompDegree = 0.0f;
	//}}AFX_DATA_INIT
}

void CCompDegree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompDegree)
	DDX_Text(pDX, CONTROL_EDIT_COMPENSATION_DEGREE, m_flCompDegree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompDegree, CDialog)
	//{{AFX_MSG_MAP(CCompDegree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompDegree message handlers

void CCompDegree::OnOK() 
{
	UpdateData( TRUE );
	CDialog::OnOK();
}

BOOL CCompDegree::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

