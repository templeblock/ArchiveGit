/*
// $Workfile: IREGISTR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IREGISTR.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:32:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:00   FRED
// Initial revision.
// 
//    Rev 2.4   24 Jan 1996 08:43:44   JAY
//  
// 
//    Rev 2.3   14 Aug 1995 14:36:32   FRED
// New Serial Number
// iregistr.cpp -> L:\PMW2\IREGISTR.CPV
// Enter description of changes made.
// 
//    Rev 2.2   07 Aug 1995 17:11:28   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:37:40   JAY
// Reverted. New series.
// 
//    Rev 1.3   04 Oct 1994 14:20:30   JAY
// Now calls FriendlyString on the serial number to correct any user errors.
// 
//    Rev 1.2   21 Jun 1994 15:37:36   FRED
// Confirms Cancel.
// 
// 
//    Rev 1.1   16 Jun 1994 13:44:56   FRED
// Removed calls to ls().
// 
// 
//    Rev 1.0   15 Jun 1994 19:01:36   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "install.h"
#include "instdat.h"
#include "iconfirm.h"
#include "util.h"
#include "pmwdlg.h"
#include "iregistr.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

VOID FriendlySerialNumber(LPSTR lpszSerialNumber);
BOOL ValidateSerialNumber(LPSTR lpszSerialNumber);

InstallationInfo UserInstallationInfo;

/////////////////////////////////////////////////////////////////////////////
// CInstallRegistrationDialog dialog


CInstallRegistrationDialog::CInstallRegistrationDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallRegistrationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallRegistrationDialog)
	m_csSerialNumber = "PMW20-59YFD-3DE5RD";
	m_csUserName = "";
	m_csCompanyName = "";
	//}}AFX_DATA_INIT
}

void CInstallRegistrationDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallRegistrationDialog)
//	DDX_Text(pDX, IDC_SERIAL_NUMBER, m_csSerialNumber);
	DDX_Text(pDX, IDC_USER_NAME, m_csUserName);
	DDX_Text(pDX, IDC_COMPANY_NAME, m_csCompanyName);
	//}}AFX_DATA_MAP
	
	int nFailID = 0;

	if (pDX->m_bSaveAndValidate)
	{
		BOOL fIsValid = TRUE;
		
		if (fIsValid)
		{
			/*
			// Validate the serial number.
			*/
		
			fIsValid = FALSE;
			
			if (m_csSerialNumber.GetLength() < sizeof(UserInstallationInfo.szSerialNumber))
			{
				lstrcpy(UserInstallationInfo.szSerialNumber, m_csSerialNumber);
				FriendlySerialNumber(UserInstallationInfo.szSerialNumber);
				// Copy it back in case FriendlySerialNumber() changed it.
				m_csSerialNumber = UserInstallationInfo.szSerialNumber;
					
				if (UserInstallationInfo.szSerialNumber[0] != 0)
				{
					if (ValidateSerialNumber(UserInstallationInfo.szSerialNumber))
					{
						fIsValid = TRUE;
					}
				}
			}
			
			if (!fIsValid)
			{
				GetConfiguration()->MessageBox(IDS_ErrInvalidSerialNumber, 0, MB_OK);
				nFailID = IDC_SERIAL_NUMBER;
			}
		}
		
		if (fIsValid)
		{
	      /*
	      // Validate the user name.
	      */
		
			fIsValid = FALSE;
			
			lstrcpyn(UserInstallationInfo.szUserName, m_csUserName, sizeof(UserInstallationInfo.szUserName)-1);
					
			if (UserInstallationInfo.szUserName[0] != 0)
			{
				fIsValid = TRUE;
			}
			
			if (!fIsValid)
			{
				GetConfiguration()->MessageBox(IDS_ErrInvalidUserName, 0, MB_OK);
				nFailID = IDC_USER_NAME;
			}
		}
		
		if (fIsValid)
		{
			/*
			// Validate the company name.
			*/
			
			fIsValid = FALSE;
			
			lstrcpyn(UserInstallationInfo.szCompanyName, m_csCompanyName, sizeof(UserInstallationInfo.szCompanyName)-1);
		
			if ((lstrlen(UserInstallationInfo.szCompanyName) != 0)
			 || (GetFocus() != GetDlgItem(IDC_USER_NAME)))
			{
				fIsValid = TRUE;
			}
			
			if (!fIsValid)
			{
				nFailID = IDC_COMPANY_NAME;
			}
		}
		
		if (fIsValid)
		{
			/*
			// Ask the user to validate his choices.
			*/
			
			fIsValid = FALSE;
			
			CInstallConfirmDialog confirmDlg(this);
			confirmDlg.m_csUserName = UserInstallationInfo.szUserName;
			confirmDlg.m_csCompanyName = UserInstallationInfo.szCompanyName;
			
			if (confirmDlg.DoModal() == IDOK)
			{
				fIsValid = TRUE;
			}
			
			if (!fIsValid)
			{
				nFailID = IDC_USER_NAME;
			}
		}
	}
	
	if (nFailID != 0)
	{
		pDX->PrepareEditCtrl(nFailID);
		pDX->Fail();
	}
}

BEGIN_MESSAGE_MAP(CInstallRegistrationDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallRegistrationDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstallRegistrationDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csSerialNumber.Empty();
	m_csUserName.Empty();
	m_csCompanyName.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CInstallRegistrationDialog message handlers

BOOL CInstallRegistrationDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
//	((CEdit *)GetDlgItem(IDC_SERIAL_NUMBER))->LimitText(sizeof(UserInstallationInfo.szSerialNumber)-1);
	((CEdit *)GetDlgItem(IDC_USER_NAME))->LimitText(sizeof(UserInstallationInfo.szUserName)-1);
	((CEdit *)GetDlgItem(IDC_COMPANY_NAME))->LimitText(sizeof(UserInstallationInfo.szCompanyName)-1);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallRegistrationDialog::OnCancel()
{
	/*
	// Confirm that the user really does want to cancel.
	*/
	
	if (GetConfiguration()->MessageBox(IDS_ConfirmExitInstallation, 0, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		CPmwDialog::OnCancel();
	}
}
