// CrPGSAcc.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "CrPGSAcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreatePGSAccount dialog


CCreatePGSAccount::CCreatePGSAccount(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatePGSAccount::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreatePGSAccount)
	m_strAccountID = _T("");
	m_strEMail = _T("");
	m_strPassword = _T("");
	m_strVerifyPassword = _T("");
	//}}AFX_DATA_INIT
}


void CCreatePGSAccount::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreatePGSAccount)
	DDX_Control(pDX, IDC_VERIFYPASSWORDEDIT, m_ecVerifyPassword);
	DDX_Control(pDX, IDC_PASSWORDEDIT, m_ecPassword);
	DDX_Control(pDX, IDC_EMAILEDIT, m_ecEMail);
	DDX_Control(pDX, IDC_ACCOUNTIDEDIT, m_ecAccountID);
	DDX_Text(pDX, IDC_ACCOUNTIDEDIT, m_strAccountID);
	DDX_Text(pDX, IDC_EMAILEDIT, m_strEMail);
	DDX_Text(pDX, IDC_PASSWORDEDIT, m_strPassword);
	DDX_Text(pDX, IDC_VERIFYPASSWORDEDIT, m_strVerifyPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreatePGSAccount, CDialog)
	//{{AFX_MSG_MAP(CCreatePGSAccount)
	ON_EN_CHANGE(IDC_ACCOUNTIDEDIT, OnChangeAccountIDEdit)
	ON_EN_CHANGE(IDC_EMAILEDIT, OnChangeEmailEdit)
	ON_EN_CHANGE(IDC_PASSWORDEDIT, OnChangePasswordEdit)
	ON_EN_CHANGE(IDC_VERIFYPASSWORDEDIT, OnChangeVerifypasswordEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreatePGSAccount message handlers

void CCreatePGSAccount::OnChangeAccountIDEdit() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecAccountID.GetLine(0, Buf, 255);
	m_strAccountID = Buf;
}

void CCreatePGSAccount::OnChangeEmailEdit() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecEMail.GetLine(0, Buf, 255);
	m_strEMail = Buf;
	
}

void CCreatePGSAccount::OnChangePasswordEdit() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecPassword.GetLine(0, Buf, 255);
	m_strPassword = Buf;
}

void CCreatePGSAccount::OnChangeVerifypasswordEdit() 
{
	char Buf[255];

	memset(Buf, '\0', 255);
	m_ecVerifyPassword.GetLine(0, Buf, 255);
	m_strVerifyPassword = Buf;

}

BOOL CCreatePGSAccount::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ecPassword.SetPasswordChar('*');
	m_ecVerifyPassword.SetPasswordChar('*');

	SetWindowText("Create a New Account");
	m_ecAccountID.SetFocus();
	//m_ecAccountID.SetSel(0,-1);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

UINT CCreatePGSAccount::VerifyPasswordFormat() 
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

UINT CCreatePGSAccount::VerifyAccountFormat() 
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

UINT CCreatePGSAccount::VerifyPassword() 
{
	char Buf[255];
	UINT uRet = 1;

	memset(Buf, '\0', 255);
	m_ecVerifyPassword.GetLine(0, Buf, 255);
	m_strVerifyPassword = Buf;

	if(m_strPassword != m_strVerifyPassword)
	{
		AfxMessageBox(IDS_PGS_INVALID_PASSWORD_VERIFICATION);
		uRet = 0;
	}

	return uRet;
}

void CCreatePGSAccount::OnOK() 
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

	CDialog::OnOK();
}
