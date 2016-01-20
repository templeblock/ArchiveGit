// SerialNumberDlg.cpp : implementation file
//

#define MESSAGE_ENTER_SERIAL_NUMBER "\
In order to run this application, please \r\n\
enter your serial number, located on the \r\n\
original floppy or CD package.  \r\n\r\n\
Thank you!"

#include "stdafx.h"
#include "SerialNumberDlg.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDD_SerialNumber_DIALOG				7000
#define IDC_SerialNumber_MESSAGE			7001
#define IDC_SerialNumber_APPLICATION		7002
#define IDC_SerialNumber_SERIAL_NUMBER		7003

/////////////////////////////////////////////////////////////////////////////
// CSerialNumberDlg dialog

BEGIN_MESSAGE_MAP(CSerialNumberDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialNumberDlg)
	ON_EN_CHANGE(IDC_SerialNumber_SERIAL_NUMBER, OnChangeSerialNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CSerialNumberDlg::CSerialNumberDlg(CSerialNumber* pSerialNumber, CString& szApplication, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SerialNumber_DIALOG, pParent)
{
	//{{AFX_DATA_INIT(CSerialNumberDlg)
	m_pSerialNumber = pSerialNumber;
	m_szApplication = szApplication;
	m_szSerialNumber = "";
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CSerialNumberDlg::~CSerialNumberDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialNumberDlg)
	DDX_Control(pDX, IDC_SerialNumber_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_SerialNumber_APPLICATION, m_szApplication);
	DDX_Text(pDX, IDC_SerialNumber_SERIAL_NUMBER, m_szSerialNumber);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSerialNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	m_Message.GetClientRect(rect);
	m_Message.SetRect(rect);
	m_Message.FmtLines(true);
	m_Message.SetWindowText(MESSAGE_ENTER_SERIAL_NUMBER);

	UpdateData(false);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnChangeSerialNumber() 
{
	UpdateData(true);
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnOK() 
{
	if (m_pSerialNumber->ValidateSerialNumber(m_szSerialNumber))
	{
		m_pSerialNumber->WriteSerialNumber(m_szSerialNumber);
		CDialog::OnOK();
	}
	else
		Message("That serial number is invalid");
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnCancel() 
{
	CDialog::OnCancel();
}
