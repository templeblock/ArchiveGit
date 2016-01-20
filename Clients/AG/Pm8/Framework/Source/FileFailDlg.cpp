//****************************************************************************
//
// File Name:  MemoryFailDlg.cpp
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Manages routines for the Memory Failure Dialog's class.
//				
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
// Client:		   Broderbund
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
// $Header:: /PM8/Framework $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "FileFailDlg.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

/////////////////////////////////////////////////////////////////////////////
// RFileFailDlg dialog

RFileFailDlg::RFileFailDlg(CWnd* pParent /*=NULL*/)
: CDialog(RFileFailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(RFileFailDlg)
	m_unFailTime = 0;
	m_unNumTimesToFail = 1;
	//}}AFX_DATA_INIT
	m_eSelFail = kNeverFail;
}

RFileFailDlg::~RFileFailDlg()
{
	NULL;
}

void RFileFailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RFileFailDlg)
	DDX_Text(pDX, IDC_E_FAIL_TIME, m_unFailTime);
	DDV_MinMaxUInt(pDX, m_unFailTime, 0, 65535);
	DDX_Text(pDX, IDC_E_NUM_FAILURES, m_unNumTimesToFail);
	DDV_MinMaxUInt(pDX, m_unNumTimesToFail, 0, 65535);
	//}}AFX_DATA_MAP
	//
	//Set m_eSelFail to the currently selected radio button.
	int	nConvertFail = int(m_eSelFail);
	DDX_Radio( pDX, IDC_RD_NEVER_FAIL, nConvertFail );
	m_eSelFail = EFailure(nConvertFail);

	m_FileFailData.eFail          = m_eSelFail;
	m_FileFailData.uwNumAllocates = static_cast<uWORD>(m_unFailTime);
	m_FileFailData.uwNumTimesToFail = static_cast<uWORD>(m_unNumTimesToFail);
}


BEGIN_MESSAGE_MAP(RFileFailDlg, CDialog)
	//{{AFX_MSG_MAP(RFileFailDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RFileFailDlg message handlers

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a FileFailureStruct with the Memory Failure Dlg's data
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void RFileFailDlg::FillData( FileFailureStruct* pMFData )
{
	*pMFData = m_FileFailData;
}

//****************************************************************************
//
// Function Name: SetData
//
// Description:   Sets the Memory Failure Dlg's data with the FileFailureStruct  
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void RFileFailDlg::SetData( FileFailureStruct* pMFData )
{
	m_FileFailData = *pMFData;
}