// CalProgD.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "CalProgD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarProgressDlg dialog


CCalendarProgressDlg::CCalendarProgressDlg(int steps, CString strTitle, COLORREF cr, CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CCalendarProgressDlg::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CCalendarProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_steps = steps;
	m_current = 0;
	m_strTitle = strTitle;
}


void CCalendarProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProgress);
	DDX_Control(pDX, IDC_PERCENT_DONE, m_ctlPercent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalendarProgressDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CCalendarProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarProgressDlg message handlers

BOOL CCalendarProgressDlg::OnInitDialog() 
{
	CPmwDialogColor::OnInitDialog();
	SetWindowText(m_strTitle);
	m_ctlProgress.SetRange (0, m_steps);
	m_ctlProgress.SetPos(m_current);
	m_ctlProgress.SetStep(1);
	m_ctlPercent.SetWindowText("");
	UpdateData(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCalendarProgressDlg::Step()
{
	
	m_current++;
	// calc percent done
	float percent = (float)m_current / (float)m_steps * 100;

	//CString fmt = "%d Percent complete.";
   CString fmt;
   fmt.LoadString (IDS_PERCENT_COMPLETE);
	char pBuf[50];
	// format text and update 
	sprintf(pBuf, fmt, (int)percent);
	m_ctlPercent.SetWindowText(pBuf);

	m_ctlProgress.StepIt();
	UpdateData(TRUE);
}
