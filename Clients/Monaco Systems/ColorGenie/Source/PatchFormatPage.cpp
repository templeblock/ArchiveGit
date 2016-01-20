// PatchFormatPage.cpp : implementation file
//

#include "stdafx.h"
#include "PatchFormatPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchFormatPage property page

IMPLEMENT_DYNCREATE(CPatchFormatPage, CPropertyPage)

CPatchFormatPage::CPatchFormatPage() : CPropertyPage(CPatchFormatPage::IDD)
{
	//{{AFX_DATA_INIT(CPatchFormatPage)
	m_calFrequency = -1;
	//}}AFX_DATA_INIT
}

CPatchFormatPage::~CPatchFormatPage()
{
}

void CPatchFormatPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchFormatPage)
	DDX_CBIndex(pDX, IDC_PATCHFORMAT, m_calFrequency);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatchFormatPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPatchFormatPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchFormatPage message handlers

BOOL CPatchFormatPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateData(FALSE);	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
