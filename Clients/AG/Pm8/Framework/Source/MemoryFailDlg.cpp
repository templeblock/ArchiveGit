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

#include "MemoryFailDlg.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

/////////////////////////////////////////////////////////////////////////////
// RMemoryFailDlg dialog

RMemoryFailDlg::RMemoryFailDlg(CWnd* pParent /*=NULL*/)
: CDialog(RMemoryFailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(RMemoryFailDlg)
	m_unFailTime = 0;
	m_unNumFailures = 0;
	//}}AFX_DATA_INIT
	m_eSelFail = kNeverFail;
}

RMemoryFailDlg::~RMemoryFailDlg()
{
	NULL;
}

void RMemoryFailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RMemoryFailDlg)
	DDX_Text(pDX, IDC_E_FAIL_TIME, m_unFailTime);
	DDX_Text(pDX, IDC_E_NUM_FAILURES, m_unNumFailures);
	DDV_MinMaxUInt(pDX, m_unFailTime, 0, 65535);
	DDV_MinMaxUInt(pDX, m_unNumFailures, 0, 65535);
	//}}AFX_DATA_MAP
	//
	//Set m_eSelFail to the currently selected radio button.
	int	nConvertFail = int(m_eSelFail);
	DDX_Radio( pDX, IDC_RD_NEVER_FAIL, nConvertFail );
	m_eSelFail = EFailure(nConvertFail);

	m_MemFailData.eFail            = m_eSelFail;
	m_MemFailData.uwNumAllocates   = static_cast<uWORD>(m_unFailTime);
	m_MemFailData.uwNumFailures    = static_cast<uWORD>(m_unNumFailures);


}


BEGIN_MESSAGE_MAP(RMemoryFailDlg, CDialog)
	//{{AFX_MSG_MAP(RMemoryFailDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RMemoryFailDlg message handlers

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a MemoryFailureStruct with the Memory Failure Dlg's data
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void RMemoryFailDlg::FillData( MemoryFailureStruct* pMFData )
{
	*pMFData = m_MemFailData;
}

//****************************************************************************
//
// Function Name: SetData
//
// Description:   Sets the Memory Failure Dlg's data with the MemoryFailureStruct  
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void RMemoryFailDlg::SetData( MemoryFailureStruct* pMFData )
{
	m_MemFailData = *pMFData;
}