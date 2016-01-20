// ServerAccountDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "srvacdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerAccountDlg dialog


CServerAccountDlg::CServerAccountDlg(int nType, CWnd* pParent /*=NULL*/)
	: CDialog(CServerAccountDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerAccountDlg)
	m_strCity = _T("");
	m_strCountry = _T("");
	m_strEMail = _T("");
	m_strFirstName = _T("");
	m_strLastName = _T("");
	m_strPhone = _T("");
	m_strState = _T("");
	m_strZipCode = _T("");
	m_strPassword = _T("");
	m_strVerifyPassword = _T("");
	m_strAccountID = _T("");
	m_strStreet1 = _T("");
	m_strStreet2 = _T("");
	//}}AFX_DATA_INIT
	m_eType = nType;
}


void CServerAccountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerAccountDlg)
	DDX_Control(pDX, IDC_STREET2EDIT, m_ecStreet2);
	DDX_Control(pDX, IDC_STREET1EDIT, m_ecStreet1);
	DDX_Control(pDX, IDC_LASTNAMESTATIC, m_stLastName);
	DDX_Control(pDX, IDC_EMAILSTATIC, m_stEMail);
	DDX_Control(pDX, IDC_ZIPCODESTATIC, m_stZipCode);
	DDX_Control(pDX, IDC_VERIFYPASSWORDSTATIC, m_stVerifyPassword);
	DDX_Control(pDX, IDC_STREETSTATIC, m_stStreet);
	DDX_Control(pDX, IDC_STATESTATIC, m_stState);
	DDX_Control(pDX, IDC_PHONESTATIC, m_stPhone);
	DDX_Control(pDX, IDC_PASSWORDSTATIC, m_stPassword);
	DDX_Control(pDX, IDC_FIRSTNAMESTATIC, m_stFirstName);
	DDX_Control(pDX, IDC_COUNTRYSTATIC, m_stCountry);
	DDX_Control(pDX, IDC_CITYSTATIC, m_stCity);
	DDX_Control(pDX, IDC_ACCOUNTIDSTATIC, m_stAccount);
	DDX_Control(pDX, IDC_ACCOUNTID, m_ecAccountID);
	DDX_Control(pDX, IDC_ZIPCODEEDIT, m_ecZipCode);
	DDX_Control(pDX, IDC_STATEEDIT, m_ecState);
	DDX_Control(pDX, IDC_PHONEEDIT, m_ecPhone);
	DDX_Control(pDX, IDC_LASTNAMEEDIT, m_ecLastName);
	DDX_Control(pDX, IDC_FIRSTNAMEEDIT, m_ecFirstName);
	DDX_Control(pDX, IDC_EMAILEDIT, m_ecEMail);
	DDX_Control(pDX, IDC_COUNTRYEDIT, m_ecCountry);
	DDX_Control(pDX, IDC_CITYEDIT, m_ecCity);
	DDX_Control(pDX, IDC_PASSWORDEDIT, m_ecPassword);
	DDX_Control(pDX, IDC_VERIFYPASSWORDEDIT, m_ecVerifyPassword);
	DDX_Text(pDX, IDC_CITYEDIT, m_strCity);
	DDV_MaxChars(pDX, m_strCity, 255);
	DDX_Text(pDX, IDC_COUNTRYEDIT, m_strCountry);
	DDV_MaxChars(pDX, m_strCountry, 255);
	DDX_Text(pDX, IDC_EMAILEDIT, m_strEMail);
	DDV_MaxChars(pDX, m_strEMail, 255);
	DDX_Text(pDX, IDC_FIRSTNAMEEDIT, m_strFirstName);
	DDV_MaxChars(pDX, m_strFirstName, 255);
	DDX_Text(pDX, IDC_LASTNAMEEDIT, m_strLastName);
	DDV_MaxChars(pDX, m_strLastName, 255);
	DDX_Text(pDX, IDC_PHONEEDIT, m_strPhone);
	DDV_MaxChars(pDX, m_strPhone, 50);
	DDX_Text(pDX, IDC_STATEEDIT, m_strState);
	DDV_MaxChars(pDX, m_strState, 2);
	DDX_Text(pDX, IDC_ZIPCODEEDIT, m_strZipCode);
	DDV_MaxChars(pDX, m_strZipCode, 15);
	DDX_Text(pDX, IDC_PASSWORDEDIT, m_strPassword);
	DDX_Text(pDX, IDC_VERIFYPASSWORDEDIT, m_strVerifyPassword);
	DDX_Text(pDX, IDC_ACCOUNTID, m_strAccountID);
	DDV_MaxChars(pDX, m_strAccountID, 255);
	DDX_Text(pDX, IDC_STREET1EDIT, m_strStreet1);
	DDX_Text(pDX, IDC_STREET2EDIT, m_strStreet2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerAccountDlg, CDialog)
	//{{AFX_MSG_MAP(CServerAccountDlg)
	ON_EN_CHANGE(IDC_CITYEDIT, OnChangeCity)
	ON_EN_CHANGE(IDC_COUNTRYEDIT, OnChangeCountry)
	ON_EN_CHANGE(IDC_EMAILEDIT, OnChangeEmail)
	ON_EN_CHANGE(IDC_FIRSTNAMEEDIT, OnChangeFirstName)
	ON_EN_CHANGE(IDC_LASTNAMEEDIT, OnChangeLastName)
	ON_EN_CHANGE(IDC_PASSWORDEDIT, OnChangePassword)
	ON_EN_CHANGE(IDC_PHONEEDIT, OnChangePhone)
	ON_EN_CHANGE(IDC_STATEEDIT, OnChangeState)
	ON_EN_CHANGE(IDC_VERIFYPASSWORDEDIT, OnChangeVerifyPassword)
	ON_EN_CHANGE(IDC_ZIPCODEEDIT, OnChangeZipcode)
	ON_EN_CHANGE(IDC_ACCOUNTID, OnChangeAccountID)
	ON_EN_CHANGE(IDC_STREET1EDIT, OnChangeStreet1Edit)
	ON_EN_CHANGE(IDC_STREET2EDIT, OnChangeStreet2Edit)
	ON_EN_KILLFOCUS(IDC_PASSWORDEDIT, OnKillFocusPasswordEdit)
	ON_EN_KILLFOCUS(IDC_VERIFYPASSWORDEDIT, OnKillFocusVerifyPasswordEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CServerAccountDlg::FromAccountInfo(const CServerAccountInfo& AccountInfo)
{
	m_strFirstName = AccountInfo.m_csCustFirstName;
	m_strLastName =  AccountInfo.m_csCustLastName;
	m_strStreet1 = AccountInfo.m_csStreet1;
	m_strStreet2 = AccountInfo.m_csStreet2;
	m_strCity = AccountInfo.m_csCity;
	m_strZipCode = AccountInfo.m_csZip;
	m_strCountry = AccountInfo.m_csCountry;
	m_strState = AccountInfo.m_csState.Left(2);
	m_strEMail = AccountInfo.m_csEMailAddr;
	m_strPhone = AccountInfo.m_csPhone;
}

void CServerAccountDlg::ToAccountInfo(CServerAccountInfo& AccountInfo) const
{
	AccountInfo.m_csCustFirstName = m_strFirstName;
	AccountInfo.m_csCustLastName = m_strLastName ;
	AccountInfo.m_csStreet1 = m_strStreet1;
	AccountInfo.m_csStreet2 = m_strStreet2;
	AccountInfo.m_csCity = m_strCity;
	AccountInfo.m_csZip = m_strZipCode;
	AccountInfo.m_csCountry = m_strCountry;
	AccountInfo.m_csState = m_strState;
	AccountInfo.m_csEMailAddr = m_strEMail;
	AccountInfo.m_csPhone = m_strPhone;
}

/////////////////////////////////////////////////////////////////////////////
// CServerAccountDlg message handlers

BOOL CServerAccountDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ecPassword.SetPasswordChar('*');
	m_ecVerifyPassword.SetPasswordChar('*');

	if(m_eType == ExistingAcct){
		SetWindowText("Select an Existing Account");
		m_ecZipCode.ShowWindow(SW_HIDE);
		m_stZipCode.ShowWindow(SW_HIDE);
		m_ecStreet1.ShowWindow(SW_HIDE);
		m_ecStreet2.ShowWindow(SW_HIDE);
		m_stStreet.ShowWindow(SW_HIDE);
		m_ecState.ShowWindow(SW_HIDE);
		m_stState.ShowWindow(SW_HIDE);
		m_ecPhone.ShowWindow(SW_HIDE);
		m_stPhone.ShowWindow(SW_HIDE);
		m_ecLastName.ShowWindow(SW_HIDE);
		m_stLastName.ShowWindow(SW_HIDE);
		m_ecFirstName.ShowWindow(SW_HIDE);
		m_stFirstName.ShowWindow(SW_HIDE);
		m_ecEMail.ShowWindow(SW_HIDE);
		m_stEMail.ShowWindow(SW_HIDE);
		m_ecCountry.ShowWindow(SW_HIDE);
		m_stCountry.ShowWindow(SW_HIDE);
		m_ecCity.ShowWindow(SW_HIDE);
		m_stCity.ShowWindow(SW_HIDE);

		m_ecVerifyPassword.ShowWindow(SW_HIDE);
		m_stVerifyPassword.ShowWindow(SW_HIDE);

		CRect EMailrect;
		m_ecEMail.GetWindowRect(&EMailrect);
		ScreenToClient(&EMailrect);

		CRect Passrect;
		m_ecPassword.GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);

		CPoint OffsetPt;
		OffsetPt.x = 0;
		OffsetPt.y = EMailrect.top - Passrect.top;

		m_ecPassword.GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);
		Passrect.OffsetRect(OffsetPt);
		m_ecPassword.MoveWindow(&Passrect, TRUE);

		m_stPassword.GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);
		Passrect.OffsetRect(OffsetPt);
		m_stPassword.MoveWindow(&Passrect, TRUE);

		m_ecPassword.GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);

		CRect Phonerect;
		m_ecPhone.GetWindowRect(&Phonerect);
		ScreenToClient(&Phonerect);

		OffsetPt.x = 0;
		OffsetPt.y = Passrect.top - Phonerect.top;

		CWnd* pOK = GetDlgItem(IDOK);
		pOK->GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);
		Passrect.OffsetRect(OffsetPt);
		pOK->MoveWindow(&Passrect, TRUE);

		CWnd* pCancel = GetDlgItem(IDCANCEL);
		pCancel->GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);
		Passrect.OffsetRect(OffsetPt);
		pCancel->MoveWindow(&Passrect, TRUE);

		GetWindowRect(&Passrect);
		Passrect.bottom += OffsetPt.y;
		MoveWindow(&Passrect, TRUE);

		m_ecAccountID.SetFocus();

	}
	if(m_eType == NewAcct){
		SetWindowText("Create a New Account");
		m_ecZipCode.ShowWindow(SW_HIDE);
		m_stZipCode.ShowWindow(SW_HIDE);
		m_ecStreet1.ShowWindow(SW_HIDE);
		m_ecStreet2.ShowWindow(SW_HIDE);
		m_stStreet.ShowWindow(SW_HIDE);
		m_ecState.ShowWindow(SW_HIDE);
		m_stState.ShowWindow(SW_HIDE);
		m_ecPhone.ShowWindow(SW_HIDE);
		m_stPhone.ShowWindow(SW_HIDE);
		m_ecLastName.ShowWindow(SW_HIDE);
		m_stLastName.ShowWindow(SW_HIDE);
		m_ecFirstName.ShowWindow(SW_HIDE);
		m_stFirstName.ShowWindow(SW_HIDE);
		m_ecCountry.ShowWindow(SW_HIDE);
		m_stCountry.ShowWindow(SW_HIDE);
		m_ecCity.ShowWindow(SW_HIDE);
		m_stCity.ShowWindow(SW_HIDE);

		CRect Passrect;
		m_ecVerifyPassword.GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);

		CRect Phonerect;
		m_ecPhone.GetWindowRect(&Phonerect);
		ScreenToClient(&Phonerect);

		CPoint OffsetPt;
		OffsetPt.x = 0;
		OffsetPt.y = Passrect.top - Phonerect.top;

		CWnd* pOK = GetDlgItem(IDOK);
		pOK->GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);
		Passrect.OffsetRect(OffsetPt);
		pOK->MoveWindow(&Passrect, TRUE);

		CWnd* pCancel = GetDlgItem(IDCANCEL);
		pCancel->GetWindowRect(&Passrect);
		ScreenToClient(&Passrect);
		Passrect.OffsetRect(OffsetPt);
		pCancel->MoveWindow(&Passrect, TRUE);

		GetWindowRect(&Passrect);
		Passrect.bottom += OffsetPt.y;
		MoveWindow(&Passrect, TRUE);
	
		m_ecAccountID.SetFocus();
	}
	if (m_eType == UpdateAcct){
		m_ecAccountID.SetReadOnly(TRUE);
		SetWindowText("Edit Account Information");
		
		m_ecEMail.SetFocus();
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServerAccountDlg::OnOK() 
{

	if(!VerifyAccountFormat()){
		m_ecAccountID.SetFocus();
		m_ecAccountID.SetSel(0,-1);
		return;
	}

	if(!VerifyPasswordFormat()){
		m_ecPassword.SetFocus();
		m_ecPassword.SetSel(0,-1);
		return;
	}

	if(!VerifyPassword()){
		m_ecVerifyPassword.SetFocus();
		m_ecVerifyPassword.SetSel(0,-1);
		return;
	}

	//make sure all fields are filled in
	if( (m_eType            == NewAcct  &&
		(m_strAccountID		== "" ||
		m_strPassword		== "" ||
		m_strVerifyPassword	== ""))  ||
		(m_eType            == ExistingAcct      &&
		(m_strAccountID		== "" ||
		m_strPassword		== "")))
	{
		AfxMessageBox(IDS_PGS_FORCE_ACCOUNT_INFO);
		return;
	}

	CDialog::OnOK();
}



void CServerAccountDlg::OnChangeCity() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecCity.GetLine(0, Buf, 255);
	m_strCity = Buf;

}

void CServerAccountDlg::OnChangeCountry() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecCountry.GetLine(0, Buf, 255);
	m_strCountry = Buf;

}

void CServerAccountDlg::OnChangeEmail() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecEMail.GetLine(0, Buf, 255);
	m_strEMail = Buf;

}

void CServerAccountDlg::OnChangeFirstName() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecFirstName.GetLine(0, Buf, 255);
	m_strFirstName = Buf;

}

void CServerAccountDlg::OnChangeLastName() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecLastName.GetLine(0, Buf, 255);
	m_strLastName = Buf;

}

void CServerAccountDlg::OnChangePassword() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecPassword.GetLine(0, Buf, 255);
	m_strPassword = Buf;

}

void CServerAccountDlg::OnChangePhone() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecPhone.GetLine(0, Buf, 255);
	m_strPhone = Buf;

}

void CServerAccountDlg::OnChangeState() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecState.GetLine(0, Buf, 255);
	m_strState = Buf;

}

void CServerAccountDlg::OnChangeVerifyPassword() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecVerifyPassword.GetLine(0, Buf, 255);
	m_strVerifyPassword = Buf;

}

void CServerAccountDlg::OnChangeZipcode() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecZipCode.GetLine(0, Buf, 255);
	m_strZipCode = Buf;

}

void CServerAccountDlg::OnChangeAccountID() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecAccountID.GetLine(0, Buf, 255);
	m_strAccountID = Buf;
}

void CServerAccountDlg::OnChangeStreet1Edit() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecStreet1.GetLine(0, Buf, 255);
	m_strStreet1 = Buf;
	
}

void CServerAccountDlg::OnChangeStreet2Edit() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecStreet2.GetLine(0, Buf, 255);
	m_strStreet2 = Buf;
	
}

void CServerAccountDlg::OnKillFocusPasswordEdit() 
{
}

UINT CServerAccountDlg::VerifyPasswordFormat() 
{
	char Buf[255];
	UINT uRet = 1;

	memset(Buf, '\0', 255);
	m_ecPassword.GetLine(0, Buf, 255);
	m_strPassword = Buf;

	int len = m_strPassword.GetLength();

	if(len <= 3)
	{
			AfxMessageBox(IDS_PGS_PASSWORD_MIN_LENGTH);
			uRet = 0;
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			char c = m_strPassword[i];

			if ((c >= 'A' && c <= 'Z') ||
				(c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9'))
			{
				//cool
			}
			else
			{
				AfxMessageBox(IDS_PGS_VALID_PASSWORD_FORMAT);
				uRet = 0;
				break;
			}
		}
	}


	return uRet;
}

UINT CServerAccountDlg::VerifyAccountFormat() 
{
	char Buf[255];
	UINT uRet = 1;

	memset(Buf, '\0', 255);
	m_ecAccountID.GetLine(0, Buf, 255);
	m_strAccountID = Buf;

	int len = m_strAccountID.GetLength();

	if(len <= 3)
	{
			AfxMessageBox(IDS_PGS_ACCOUNT_MIN_LENGTH);
			uRet = 0;
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			char c = m_strAccountID[i];

			if ((c >= 'A' && c <= 'Z') ||
				(c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9'))
			{
				//cool
			}
			else
			{
				AfxMessageBox(IDS_PGS_VALID_ACCOUNT_FORMAT);
				uRet = 0;
				break;
			}
		}
	}


	return uRet;
}

void CServerAccountDlg::OnKillFocusVerifyPasswordEdit() 
{
}

UINT CServerAccountDlg::VerifyPassword() 
{
	UINT uRet = 1;

	if (m_eType != ExistingAcct)
	{
		char Buf[255];
		memset(Buf, '\0', 255);
		m_ecVerifyPassword.GetLine(0, Buf, 255);
		m_strVerifyPassword = Buf;

		if(m_strPassword != m_strVerifyPassword)
		{
			AfxMessageBox(IDS_PGS_INVALID_PASSWORD_VERIFICATION);
			uRet = 0;
		}
	}
	return uRet;
}
