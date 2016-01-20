// CalTypeD.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "CalTypeD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarTypeDlg dialog


CCalendarTypeDlg::CCalendarTypeDlg(COLORREF cr, CALENDAR_INFO * pInfo, CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CCalendarTypeDlg::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CCalendarTypeDlg)
	//}}AFX_DATA_INIT
	if (pInfo)
	{
		switch (pInfo->style.type)
		{
		case YEARLY:
			m_iCalType = 0;
			break;
		case MONTHLY:
			m_iCalType = 1;
			break;
		case WEEKLY:
			m_iCalType = 2;
			break;
		};

	}
	else
		m_iCalType = 1;
	
	m_WizFlags = WIZBUT_BACK | WIZBUT_NEXT | WIZBUT_CANCEL | WIZBUT_HELP;
}


void CCalendarTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarTypeDlg)
	DDX_Radio(pDX, IDC_YEARLY, m_iCalType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalendarTypeDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CCalendarTypeDlg)
		ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_YEARLY, OnYearly)
	ON_BN_CLICKED(IDC_MONTHLY, OnMonthly)
	ON_BN_CLICKED(IDC_WEEKLY, OnWeekly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarTypeDlg message handlers

BOOL CCalendarTypeDlg::OnInitDialog() 
{
	VERIFY (m_Preview.SubclassDlgItem (IDC_MAKE_PREVIEW, this));

	CPmwDialogColor::OnInitDialog();
	
   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);
	 UpdateCBitmap();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CCalendarTypeDlg::get_exit_info (CALENDAR_INFO_PTR pInfo)
{
	BOOL rv = TRUE;
	switch (m_iCalType)
		{
		case 0:
				pInfo->set_default_style(YEARLY);
				break;
		case 1:
				pInfo->set_default_style(MONTHLY);
				break;
		case 2:
				pInfo->set_default_style(WEEKLY);
				break;
		default:
			rv = FALSE;
			break;
		};
	return rv;
}


void CCalendarTypeDlg::OnBack() 
{
   EndDialog (IDC_BACK);
}


void CCalendarTypeDlg::OnYearly() 
{
	UpdateCBitmap();
	
}

void CCalendarTypeDlg::OnMonthly() 
{
	UpdateCBitmap();
	
}

void CCalendarTypeDlg::OnWeekly() 
{
	UpdateCBitmap();
	
}

void CCalendarTypeDlg::UpdateCBitmap()
{
	UpdateData(TRUE);

	CString MetaName;

	switch (m_iCalType)
	{
		
		case 0:
			{
				MetaName = "CALENDARYNP_BM";
				break;
			}
		case 1:
			{
				MetaName = "CALENDARMNP_BM";
				break;
			}
		case 2:
			{
				MetaName = "CALENDARWNP_BM";
				break;
			}
	};
	
	m_Preview.UpdateCBitmap (MetaName);
	m_Preview.Invalidate();
}
