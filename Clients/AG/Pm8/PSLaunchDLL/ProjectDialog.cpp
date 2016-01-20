// ProjectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LaunchResource.h"

#include "ProjectDialog.h"
#include "CommonTypes.h"
#include "PSButton.h"

#include "ProjectCalendar.h"
#include "ProjectSetupDlg.h"
#include "ProjectQSLDlg.h"

// Renaissance Support
#include "ProjectTypes.h"
#include "Printer.h"

using RPrinter::kUSLetter;
using RPrinter::kUSLegal;
using RPrinter::kA4Letter;
using RPrinter::kB5Letter;
using RPrinter::kA3Tabloid;
using RPrinter::kSpecialPaperSize;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

STATEINFO CProjectDialog::StateInfo = { 0 };

/////////////////////////////////////////////////////////////////////////////
// CProjectDialog dialog


CProjectDialog::CProjectDialog( UINT nDialogID, CWnd* pParent /*=NULL*/, UINT nNextDlg /*= 0*/)
	: CPSBaseDialog(nDialogID, pParent),
	m_nNextDlg( nNextDlg ),
	m_nNextNextDlg( 0 )
{
	//{{AFX_DATA_INIT(CProjectDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CPSBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectDialog, CPSBaseDialog)
	//{{AFX_MSG_MAP(CProjectDialog)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_LBN_SELCHANGE(IDC_FORMAT_LIST, OnSelChangeFormatList)
	ON_BN_CLICKED(IDC_FORMAT_TALL, OnFormatTall)
	ON_BN_CLICKED(IDC_FORMAT_WIDE, OnFormatWide)
	ON_LBN_SELCHANGE(IDC_PAPER_LIST, OnSelchangePaperList)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_START_CUSTOMIZE, OnStartCustomize)
	ON_BN_CLICKED(IDC_START_SCRATCH, OnStartScratch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectDialog message handlers

BOOL CProjectDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (BN_CLICKED == HIWORD( wParam ))
	{
		CWnd* pWnd = GetDlgItem( LOWORD( wParam ) );

		if (dynamic_cast<CPSButton*>( pWnd ))
		{
			DWORD dwData = ((CPSButton *) pWnd)->GetUserData();

			// Get the preview
			if (HIWORD( dwData ))
			{
				CStatic* pWnd = (CStatic *) GetDlgItem( IDC_PREVIEW );
				ASSERT( pWnd );

				UINT uiResID = HIWORD( dwData );
				HBITMAP hBitmap = ::LoadBitmap( AfxGetResourceHandle(), 
					MAKEINTRESOURCE( uiResID ) );

				hBitmap = pWnd->SetBitmap( hBitmap );
				pWnd->UpdateWindow( );

				if (hBitmap) 
					::DeleteObject( hBitmap );
			}

			if (LOWORD( dwData ))
			{
				// Get the next dialog
				m_nNextDlg = LOWORD( dwData );
			}

			return CPSBaseDialog::OnCommand(wParam, lParam);
		}

#if 0
		// Search through table to find matching command.
		for (int i = 0; i < NumElements( kProjectWizardTransitions ); i++)
		{
			if (kProjectWizardTransitions[i][0] == LOWORD( wParam ))
			{
				m_nNextDlg     = LOWORD( wParam );
				m_nNextNextDlg = kProjectWizardTransitions[i][1];

				return TRUE;
			}
		}
#endif
	}

	return CPSBaseDialog::OnCommand(wParam, lParam);
}

void CProjectDialog::OnNext() 
{
	if (m_nNextDlg)
	{
		CPSBaseDialog* pDialog = NULL;
		
		if (IDD_PROJECT_FORMAT == m_nNextDlg)
		{
			SProjectMapEntry mapEntry;

			// Remap the format dialog ID if necessary.
			if (m_cProjectMap.Lookup( _ProjectInfo.nProjectType, mapEntry ) && mapEntry.m_wFormatDlgID)
				m_nNextDlg = mapEntry.m_wFormatDlgID;
		}

		switch (m_nNextDlg)
		{
		case IDD_PROJECT_CALENDAR_TYPE:
			pDialog = new CProjectCalendarDlg( GetParent() );
			break;

		case IDD_PROJECT_QSL_PRESSWRITER:
			pDialog = new CProjectQSLDlg( GetParent() );
			break;

		case IDD_PROJECT_SETUP:
			pDialog = new CProjectSetupDlg( GetParent() );
			break;

		default:
			pDialog = new CProjectDialog( m_nNextDlg, GetParent(), m_nNextNextDlg );
		};

		GetParent()->SendMessage( UM_PUSH_DIALOG, GetBaseDialogID( m_nNextDlg ), (LPARAM) pDialog );
	}
}

BEGIN_EVENTSINK_MAP(CProjectDialog, CPSBaseDialog)
    //{{AFX_EVENTSINK_MAP(CProjectDialog)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CProjectDialog::OnInitDialog() 
{
	CPSBaseDialog::OnInitDialog();

	if (StateInfo.uiHeaderID)
	{
		CStatic* pStatic = (CStatic *) GetDlgItem( IDC_TEXT_HEADER );

		if (pStatic)
		{
			HBITMAP hBitmap = ::LoadBitmap( AfxGetResourceHandle(), 
				MAKEINTRESOURCE( StateInfo.uiHeaderID ) );

			if (hBitmap)
			{
				hBitmap = pStatic->SetBitmap( hBitmap );
				::DeleteObject( hBitmap );
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CProjectDialog::OnSelChangeFormatList() 
{
	CListBox* pWnd = (CListBox *) GetDlgItem( IDC_FORMAT_LIST );
	DWORD dwData = pWnd->GetItemData( pWnd->GetCurSel() );

	if (dwData)
	{
//			m_uiNextDlg = LOWORD( dwData );

		if (HIWORD( dwData ))
		{
			CStatic* pWnd = (CStatic *) GetDlgItem( IDC_PREVIEW );
			ASSERT( pWnd );

			UINT uiResID = HIWORD( dwData );
			HBITMAP hBitmap = ::LoadBitmap( AfxGetResourceHandle(), 
				MAKEINTRESOURCE( uiResID ) );

			hBitmap = pWnd->SetBitmap( hBitmap );

			if (hBitmap) 
				::DeleteObject( hBitmap );
		}

		if (LOWORD( dwData ))
		{
			// Save state data.  Remember project values obtained 
			// from the resource file are 1 - based, where the
			// actual type we want to store is 0 - based.  
			_ProjectInfo.nProjectType	= LOWORD( dwData ) - 1;

			switch (_ProjectInfo.nProjectType)
			{
			case kTrifoldBrochureTall:
				_ProjectInfo.nProjectType	= kTrifoldBrochure;
				_ProjectInfo.nProjectFormat = kWide; 
				break;

			case kTrifoldBrochureWide:
				_ProjectInfo.nProjectType	= kTrifoldBrochure;
				_ProjectInfo.nProjectFormat = kTall; 
				break;

			default:
				_ProjectInfo.nProjectFormat	= 0; // Defined by project type
			};
		}

		LoadPaperList();
	}
}

void CProjectDialog::OnOK() 
{
	// TODO: validate state
	GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM( IDOK, BN_CLICKED ) );
	
//	CPSBaseDialog::OnOK();
}

void CProjectDialog::OnFormatTall() 
{
	_ProjectInfo.nProjectFormat	= kTall;
}

void CProjectDialog::OnFormatWide() 
{
	_ProjectInfo.nProjectFormat	= kWide;
}

void CProjectDialog::OnSelchangePaperList() 
{
	CListBox* pWnd = (CListBox *) GetDlgItem( IDC_PAPER_LIST );
	DWORD dwData = pWnd->GetItemData( pWnd->GetCurSel() );

	// Save state data
	_ProjectInfo.nPaperType = LOWORD( dwData );
}

BOOL CProjectDialog::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect( rect );
	rect.InflateRect( 0, 0, 1, 1 );
	pDC->FillSolidRect( rect, RGB( 255, 255, 255 ) );
	
	return TRUE;
}


void CProjectDialog::OnStartCustomize() 
{
	_ProjectInfo.eProjectMethod = kMethodCustomize;
}

void CProjectDialog::OnStartScratch() 
{
	_ProjectInfo.eProjectMethod = kMethodScratch;
}
