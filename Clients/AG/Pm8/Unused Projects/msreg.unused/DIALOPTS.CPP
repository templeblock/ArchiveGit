/*
// $Workfile: DIALOPTS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/DIALOPTS.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.3   04 Oct 1996 15:29:52   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.2   26 Jun 1996 07:03:26   FRED
// International Registration
// 
//    Rev 1.1   05 Dec 1995 13:02:36   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:06   FRED
// Initial revision.
// 
//    Rev 1.6   16 Nov 1995 10:31:18   FRED
//  
// 
//    Rev 1.5   31 Oct 1995 15:09:12   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.4   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.3   26 Oct 1995 10:27:54   FRED
//  
*/ 

// dialopts.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "msregini.h"
#include "dialopts.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialingOptionsDialog dialog


CDialingOptionsDialog::CDialingOptionsDialog(CWnd* pParent /*=NULL*/)
	: CBaseDialog(CDialingOptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialingOptionsDialog)
	m_csPrefix = _T("");
	m_fDial1 = FALSE;
	//}}AFX_DATA_INIT
}


void CDialingOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialingOptionsDialog)
	DDX_Text(pDX, IDC_PREFIX, m_csPrefix);
	DDX_Check(pDX, IDC_DIAL1, m_fDial1);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_DialingOptions, "Prefix", m_csPrefix);
		GetApp()->UserIniFile().WriteInteger(MSREGUSRINI_Section_DialingOptions, "DialOne", m_fDial1 ? 1 : 0);
	}
}


BEGIN_MESSAGE_MAP(CDialingOptionsDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CDialingOptionsDialog)
	ON_EN_CHANGE(IDC_PREFIX, OnChangePrefix)
	ON_BN_CLICKED(IDC_DIAL1, OnDial1)
	ON_BN_DOUBLECLICKED(IDC_DIAL1, OnDoubleclickedDial1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDialingOptionsDialog message handlers

BOOL CDialingOptionsDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);

	CenterWindow();
	
	((CEdit*)GetDlgItem(IDC_PREFIX))->LimitText(10);
	
	((CEdit*)GetDlgItem(IDC_PREFIX))->SetWindowText(GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_DialingOptions, MSREGUSRINI_DialingOptions_Prefix, DEFAULT_PREFIX));
	CheckDlgButton(IDC_DIAL1, GetApp()->UserIniFile().GetInteger(MSREGUSRINI_Section_DialingOptions, MSREGUSRINI_DialingOptions_DialOne, DEFAULT_DIAL1));

	CInternationalData* pInternationalData = GetApp()->GetInternationalData();
	if (pInternationalData != NULL)
	{
		if (!pInternationalData->m_fUnitedStatesLocal)
		{
			GetDlgItem(IDC_DIAL1)->EnableWindow(FALSE);
		}
	}

	RedrawDialingNumber();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialingOptionsDialog::RedrawDialingNumber(void)
{
	CString csNumber;
	csNumber.Empty();

	CWnd* pDialingNumber = GetDlgItem(IDC_DIALING_NUMBER);
	if (pDialingNumber != NULL)
	{
		CInternationalData* pInternationalData = GetApp()->GetInternationalData();
		if (pInternationalData != NULL)
		{
			TRY
			{
				GetDlgItem(IDC_PREFIX)->GetWindowText(csNumber);
				int nLength = csNumber.GetLength();
				if (nLength > 0 && csNumber[nLength-1] != ',')
				{
					csNumber += ',';
				}

				csNumber += pInternationalData->m_Registration.m_Modem.m_szPrefix;

				if ((pInternationalData->m_Registration.m_Modem.m_fUnitedStatesLocal)
				 && (pInternationalData->m_fUnitedStatesLocal))
				{
					if (IsDlgButtonChecked(IDC_DIAL1))
					{
						csNumber += "1-";
					}
				}

				csNumber += pInternationalData->m_Registration.m_Modem.m_szNumber;
			}
			END_TRY
		}
	}

	pDialingNumber->SetWindowText(csNumber);
}

void CDialingOptionsDialog::OnChangePrefix() 
{
	RedrawDialingNumber();
}

void CDialingOptionsDialog::OnDial1() 
{
	RedrawDialingNumber();
}

void CDialingOptionsDialog::OnDoubleclickedDial1() 
{
	RedrawDialingNumber();
}
