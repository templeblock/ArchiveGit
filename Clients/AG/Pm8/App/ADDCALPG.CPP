// AddCalendarPageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "Addcalpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddCalendarPageDialog dialog


CAddCalendarPageDialog::CAddCalendarPageDialog(short iPages, BOOL bPicture, CWnd* pParent /*=NULL*/)
	: CDialog(CAddCalendarPageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddCalendarPageDialog)
	m_iAddNumber = 1; // add 1 page
	
	m_iContents = 0; // default
	m_iWhere = 1;  // at end
	m_strPageNumber = _T("");
	//}}AFX_DATA_INIT
	
	m_iCopyPage = 1; // use this page as a template
	m_iPages = iPages;
	m_bPicture = bPicture;
}


void CAddCalendarPageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddCalendarPageDialog)
	DDX_Control(pDX, IDC_COPYPAGE_CMBO, m_ctlCopyPage);
	DDX_Text(pDX, IDC_EDIT_NUMBER_CAL, m_iAddNumber);
//	DDX_Text(pDX, IDC_PAGE_NUM, m_iCopyPage);
	DDX_Radio(pDX, IDC_RADIO_CONTENTS1, m_iContents);
	DDX_Radio(pDX, IDC_RADIO_WHERE1, m_iWhere);
	DDX_CBString(pDX, IDC_COPYPAGE_CMBO, m_strPageNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddCalendarPageDialog, CDialog)
	//{{AFX_MSG_MAP(CAddCalendarPageDialog)
	ON_CBN_SELCHANGE(IDC_COPYPAGE_CMBO, OnSelchangeCopypageCmbo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddCalendarPageDialog message handlers

BOOL CAddCalendarPageDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	update_page_control();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddCalendarPageDialog::OnOK() 
{
	UpdateData(TRUE);

	if (!m_bPicture) 
		m_iCopyPage= m_ctlCopyPage.GetCurSel(); // no picture
	else  
		m_iCopyPage = m_ctlCopyPage.GetCurSel() + 1; // picture

	CDialog::OnOK();
}

void CAddCalendarPageDialog::update_page_control()
{
	m_ctlCopyPage.ResetContent();
	CString strFmt;
	
	
	// if picture is present, do not include cover pages in the list
	if (m_bPicture)
	{
		for (int i = 2; i < m_iPages-1; i+=2)
		{
			char pBuf[40];
			strFmt = "pages %d - %d";
			sprintf(pBuf, strFmt, i, i+1);
			m_ctlCopyPage.AddString(pBuf);
		}
	}
	else
	{
		for (int i = 1; i <=m_iPages; i++)
		{
			char pBuf[40];
			strFmt = "page %d";
			sprintf(pBuf, strFmt, i);
			m_ctlCopyPage.AddString(pBuf);
		}

	}
	m_ctlCopyPage.SetCurSel(0);

}

void CAddCalendarPageDialog::OnSelchangeCopypageCmbo() 
{
	/*UpdateData(TRUE);
	
	if (!m_bPicture) 
		m_iCopyPage= m_ctlCopyPage.GetCurSel(); // no picture
	else  
		m_iCopyPage = m_ctlCopyPage.GetCurSel() + 1; // picture
*/
}
