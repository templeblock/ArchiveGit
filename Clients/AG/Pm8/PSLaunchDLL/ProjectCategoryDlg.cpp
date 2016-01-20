// ****************************************************************************
//
//  File Name:		ProjectCategoryDlg.cpp
//
//  Project:		PrintShop Launcher Application
//
//  Author:			Lance Wilson
//
//  Description:	Implementation file
//
//  Developed by:	Broderbund Software
//				    500 Redwood Blvd.
//				    Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PSLaunchDLL/ProjectCategoryDlg.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "LaunchResource.h"

#include "ProjectCategoryDlg.h"
#include "ProjectDialog.h"
#include "CommonTypes.h"
#include "PSButton.h"

// Renaissance Support
#include "ProjectTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0
// The ProjectCategoryTransition table maps a project type to the start of it's
// path to project.  Note, whenever 1 is subtracted from a number it is because
// it is a 1-based value where all the others are 0 - based.
UINT	kProjectCategoryTransitions[][3] =
{
	//	Project ID,			Project Dialog						Header ID
	//
	{	kBlankPage - 1,		IDD_PROJECT_FORMAT,					IDB_TEXT_SIGN		  },	
	{	kSign,				IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kCertificate,		IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kLetterHead,		IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kGreetingCard - 1,	IDD_PROJECT_GREETINGCARD_METHOD,	IDB_TEXT_GREETINGCARD },	
	{	kCalendar - 1,		IDD_PROJECT_CALENDAR_METHOD,		IDB_TEXT_CALENDAR	  },  
	{	kPamphlet,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kNameTag - 1,		IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kPostCard,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kBanner,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kBusinessCard,		IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kEnvelope - 1,		IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kPostItNote,		IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },  
	{	kLabels - 1,		IDD_PROJECT_LABEL_METHOD,			IDB_TEXT_LABEL		  },  
	{	kNewsletter,		IDD_PROJECT_METHOD,					IDB_TEXT_NEWSLETTER   },	
	{	kBrocures,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kLetters,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kReports,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kFlyers,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kBooklets,			IDD_PROJECT_METHOD,					IDB_TEXT_SIGN		  },	
	{	kWebsite,			IDD_PROJECT_WEBSITE_METHOD,			IDB_TEXT_WEBSITE      },
//	{	kPhotoAlbum,		IDD_PROJECT_METHOD,					IDB_TEXT_PHOTOPROJECT },  QSL only
//	{	kPhotoCollage,		IDD_PROJECT_METHOD,					IDB_TEXT_PHOTOPROJECT },  QSL only
//	{	kPhotoNovelty,		IDD_PROJECT_METHOD,					IDB_TEXT_PHOTOPROJECT }   QSL only
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectCategoryDlg dialog


CProjectCategoryDlg::CProjectCategoryDlg(UINT nDialogID, CWnd* pParent /*=NULL*/) : 
	CPSBaseDialog(nDialogID, pParent),
	m_uiNextDlg( 0 )
{
	//{{AFX_DATA_INIT(CProjectCategoryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjectCategoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CPSBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectCategoryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectCategoryDlg, CPSBaseDialog)
	//{{AFX_MSG_MAP(CProjectCategoryDlg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT CProjectCategoryDlg::MapProjectToDialog( UINT nProjType )
{
	SProjectMapEntry mapEntry;
	if (m_cProjectMap.Lookup( nProjType, mapEntry ))
	{
		CProjectDialog::StateInfo.uiHeaderID = mapEntry.m_wHeaderID;
		return mapEntry.m_wStartDlgID;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectCategoryDlg message handlers

BOOL CProjectCategoryDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect( rect );
	rect.InflateRect( 0, 0, 1, 1 );
	pDC->FillSolidRect( rect, RGB( 255, 255, 255 ) );
	
	return TRUE;
}

BOOL CProjectCategoryDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
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
				// Get the project type and the first dialog ID of the path to project. 
				// Note, a value of 1 is subtraced from the low word of dwData because it 
				// is 1 - based and the project types  are 0 - based.
				m_nProjectType = LOWORD( dwData ) - 1;
				m_uiNextDlg = MapProjectToDialog( m_nProjectType );

				// A project type of kBlankPage is really a sign.  We have to have
				// the seperate entry in our table though as their path to project
				// is different.
				if (LOWORD( dwData ) == kBlankPage)
					m_nProjectType = kSign;

				if (m_uiNextDlg)
				{
					OnNext();
					//CDialog* pDialog = new CProjectDialog( m_uiNextDlg, GetParent() );
					//GetParent()->SendMessage( UM_PUSH_DIALOG, GetBaseDialogID( m_uiNextDlg ), (LPARAM) pDialog );
				}
			}

			return TRUE;
		}
	}
	else if (LBN_SELCHANGE == HIWORD( wParam ))
	{
		CListBox* pWnd = (CListBox *) GetDlgItem( LOWORD( wParam ) );
		DWORD dwData = pWnd->GetItemData( pWnd->GetCurSel() );

		if (dwData)
		{
			// Get the project type and the first dialog ID of the path to project. 
			// Note, a value of 1 is subtraced from the low word of dwData because it 
			// is 1 - based and the project types  are 0 - based.
			m_nProjectType = LOWORD( dwData ) - 1;
			m_uiNextDlg = MapProjectToDialog( m_nProjectType );

			// A project type of kBlankPage is really a sign.  We have to have
			// the seperate entry in our table though as their path to project
			// is different.
			if (LOWORD( dwData ) == kBlankPage)
				m_nProjectType = kSign;

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

			return TRUE;
		}
	}
	
	return CPSBaseDialog::OnCommand(wParam, lParam);
}

void CProjectCategoryDlg::OnNext() 
{
	if (m_uiNextDlg)
	{
		if (m_nProjectType != _ProjectInfo.nProjectType)
		{
			_ProjectInfo.nProjectType = m_nProjectType;
			GetParent()->SendMessage( UM_REMOVE_UNUSED );
		}

		CDialog* pDialog = new CProjectDialog( m_uiNextDlg, GetParent() );
		GetParent()->SendMessage( UM_PUSH_DIALOG, GetBaseDialogID( m_uiNextDlg ), (LPARAM) pDialog );
	}
}

HBRUSH CProjectCategoryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return CPSBaseDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}
