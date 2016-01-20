// MyMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "MyMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyMessageBox dialog


MyMessageBox::MyMessageBox(CString my_message,
						   int disable_which,
						   CWnd* pParent /*=NULL*/)
	: CDialog(MyMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(MyMessageBox)
	m_exclam_text = my_message;
	//}}AFX_DATA_INIT

	m_disabled = disable_which;
}


void MyMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MyMessageBox)
	DDX_Control(pDX, IDC_SAVE, m_save);
	DDX_Control(pDX, IDI_EXCLAM_ICON, m_exclamicon);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_NO_SAVE, m_no_save);
	DDX_Text(pDX, IDC_EXCLAM_TEXT, m_exclam_text);
	//}}AFX_DATA_MAP
}

void MyMessageBox::DisableButton(int which_button) // 1, 2, 3
{
	switch(which_button)
	{
		case 1:
			m_save.EnableWindow(FALSE);
			break;

		case 2:
			m_no_save.EnableWindow(FALSE);
			break;
		case 3:
			m_cancel.EnableWindow(FALSE);
	}	
}

BEGIN_MESSAGE_MAP(MyMessageBox, CDialog)
	//{{AFX_MSG_MAP(MyMessageBox)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_NO_SAVE, OnNoSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MyMessageBox message handlers

BOOL MyMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//m_exclamation.

	//load icon
	m_exclamicon.SetIcon(::LoadIcon(NULL, IDI_EXCLAMATION));

	DisableButton(m_disabled);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MyMessageBox::OnSave() 
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDYES); 
}

void MyMessageBox::OnNoSave() 
{
	// TODO: Add your control notification handler code here
	CDialog::EndDialog(IDNO); 
}
