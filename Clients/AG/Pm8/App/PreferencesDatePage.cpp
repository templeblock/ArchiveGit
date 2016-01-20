// PreferencesDatePage.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "PreferencesDatePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDatePage property page

IMPLEMENT_DYNCREATE(CPreferencesDatePage, CPropertyPage)

CPreferencesDatePage::CPreferencesDatePage() : CPropertyPage(CPreferencesDatePage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesDatePage)
	m_nDateFormat = -1;
	//}}AFX_DATA_INIT
}

CPreferencesDatePage::~CPreferencesDatePage()
{
}

void CPreferencesDatePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDatePage)
	DDX_CBIndex(pDX, IDC_DATEFORMAT, m_nDateFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesDatePage, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesDatePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDatePage message handlers

BOOL CPreferencesDatePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here

	// fill the combo box with the different date format strings
	CPmwApp*    pApp = GET_PMWAPP();
	CComboBox* pListWnd = (CComboBox *)GetDlgItem( IDC_DATEFORMAT );
	// add "System Default" first
	pListWnd->AddString( "System Default" );	
	// add the other formats using todays date
	for( int i = DT_SYSTEM_DEFAULT + 1; i < DT_LASTENTRY; i++ )
	{
		CString str;
		pApp->GetDateFormattedString( &str, (EPDateFormat)i );
		str.TrimLeft();
		pListWnd->AddString( str );
	}
	// get the current user date format
	m_nDateFormat = pApp->m_eDateFormat;
	// set it in the list box
	pListWnd->SetCurSel( m_nDateFormat );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
