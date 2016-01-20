// ProjectCalendar.cpp : implementation file
//

#include "stdafx.h"
#include "LaunchResource.h"
#include "ProjectCalendar.h"
#include "CommonTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectCalendarDlg dialog


CProjectCalendarDlg::CProjectCalendarDlg(CWnd* pParent /*=NULL*/)
	: CProjectDialog(CProjectCalendarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectCalendarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjectCalendarDlg::DoDataExchange(CDataExchange* pDX)
{
	CProjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectCalendarDlg)
	DDX_Control(pDX, IDC_CALENDAR, m_cCalendar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectCalendarDlg, CProjectDialog)
	//{{AFX_MSG_MAP(CProjectCalendarDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectCalendarDlg message handlers

BOOL CProjectCalendarDlg::OnInitDialog() 
{
	CProjectDialog::OnInitDialog();

	// Load the background bitmap
	m_cBitmap.LoadBitmap( IDB_PROJECT_WORKSPACE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CProjectCalendarDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect( rect );
	pDC->FillSolidRect( rect, RGB( 255, 255, 255 ) );

	if (m_cBitmap.GetSafeHandle())
	{
		BITMAP bm;
		m_cBitmap.GetObject( sizeof( bm ), &bm );

		CDC dcMem;
		dcMem.CreateCompatibleDC( pDC );
		dcMem.SelectObject( &m_cBitmap );
		pDC->BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY );
	}

	return TRUE;
}

void CProjectCalendarDlg::OnNext() 
{
	// Get the project type and the first dialog ID of the path to project. 
	// Note, a value of 1 is subtraced from m_nNextDlg because it is 
	// 1 - based and the project types  are 0 - based.
	_ProjectInfo.nProjectType = m_nNextDlg - 1;
	m_nNextDlg = IDD_PROJECT_FORMAT;

	// TODO: Get Date information

	CProjectDialog::OnNext();
}
