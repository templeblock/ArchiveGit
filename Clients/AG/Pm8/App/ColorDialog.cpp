// ****************************************************************************
//
//  File Name:	  ColorDialog.cpp
//
//  Project:	  Renaissance Application Framework
//
//  Author:		  Lance Wilson
//
//  Description: Implementation of the RColorDialog and the 
//               RColorDialogImp classes
//
//  Portability: Platform independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ColorDialog.cpp                                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:04p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************
#include "stdafx.h"
ASSERTNAME

#include "ColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// IColorDialogImp
//////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// Function Name:  IColorDialogImp
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
IColorDialogImp::IColorDialogImp()
{
}

// ****************************************************************************
//
//  Function Name:	IColorDialogImp::DoModal( )
//
//  Description:		Executes the dialog modally.
//
//  Returns:			An int value that specifies the value of the 
//                   nResult parameter that was passed to the 
//                   CDialog::EndDialog member function
//
//  Exceptions:		None
//
// ****************************************************************************
//
int IColorDialogImp::DoModal( CWnd* pParent, UINT uwFlags )
{
	if (uwFlags & kAllowGradients)
	{
//		BOOLEAN fShowTransparent = uwFlags & kShowTransparent;
		RColorDialog dlg( pParent, uwFlags );
		dlg.SetColor( m_crColor );

		if (IDOK == dlg.DoModal())
		{
			m_crColor = dlg.SelectedColor() ;
			return IDOK;
		}
	}
	else
	{
		return RColorDialogInterfaceImp::DoModal( pParent, uwFlags );
	}

	return IDCANCEL;
}

// ****************************************************************************
//
//  Function Name:	IColorDialogImp::CreateInterface( )
//
//  Description:		Instantiates an object of this type
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
BOOLEAN IColorDialogImp::CreateInstance( YInterfaceId id, void** ppInterface )
{
	IColorDialogImp* pObject = new IColorDialogImp();

	if (pObject && !pObject->QueryInterface( id, ppInterface ))
	{
		delete pObject;
		return FALSE;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// RColorDialog
//////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// Function Name:  RColorDialog
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RColorDialog::RColorDialog( CWnd* pParent, UINT uwFlags ) : 
	CDialog( RColorDialog::IDD, pParent ),
	m_dlgColorPalette( NULL, BOOLEAN( uwFlags & kShowTransparent ) ),
	m_pCurrentPage( NULL ),
	m_uwFlags( uwFlags ),
	m_nStartPage( 0 )
{
	//{{AFX_DATA_INIT(RColorDialog)
	//}}AFX_DATA_INIT
}

//*****************************************************************************
//
// Function Name: DoDataExchange
//
// Description:   Method for transferning the contents of the child
//				      controls to/from the data member variables.  As
//						well as any subclassing.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RColorDialog)
	DDX_Control (pDX, CONTROL_COLOR_TAB, m_ctlTabCtrl) ;
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RColorDialog, CDialog)
	//{{AFX_MSG_MAP(RColorDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED( IDC_COLOR_MORE, OnColorMore )
	ON_NOTIFY(TCN_SELCHANGE, CONTROL_COLOR_TAB, OnSelChangeTabs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: OnInitDialog
//
// Description:   Initializes dialog, gradient button controls and creates and 
//                positions the angle control.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RColorDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Add in the tabs
	TC_ITEM tabItem;
	tabItem.iImage  = -1;
	tabItem.mask    = TCIF_TEXT;

	CString strTabText ;
	
	try
	{
		strTabText.LoadString( STRING_MISC_COLORPAL_COLOR ) ;
		tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
		m_ctlTabCtrl.InsertItem( 0, &tabItem ) ;

		if (!(m_uwFlags & kExcludeTextures))
		{
			strTabText.LoadString( STRING_MISC_COLORPAL_TEXTURE ) ;
			tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
			m_ctlTabCtrl.InsertItem( 1, &tabItem ) ;

			strTabText.LoadString( STRING_MISC_COLORPAL_PHOTO ) ;
			tabItem.pszText = (LPSTR)(LPCSTR) strTabText ;
			m_ctlTabCtrl.InsertItem( 2, &tabItem ) ;
		}
	}
	catch (CMemoryException& e)
	{
		e.ReportError() ;
		EndDialog( IDCANCEL ) ;

		return TRUE ;
	}

	m_ctlTabCtrl.SetCurSel( m_nStartPage );
	ShowPage( m_nStartPage );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RColorDialog::OnColorMore()
{
	if (m_pCurrentPage == (CDialog *) &m_dlgColorPalette)
	{
		CColorDialog dlg( (YPlatformColor) m_dlgColorPalette.SelectedColor().GetSolidColor() );

		if (IDOK == dlg.DoModal())
			m_dlgColorPalette.SetColor( RSolidColor( dlg.GetColor() ) );
	}
}

//****************************************************************************
//
// Function Name: OnOK
//
// Description:   OK message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RColorDialog::OnOK()
{
	if (m_pCurrentPage == (CDialog *) &m_dlgColorTexture)
	{
		m_dlgColorTexture.UpdateData( TRUE );
		m_dlgColorTexture.AddSelectedToMRU();
	}
	else if (m_pCurrentPage == (CDialog *) &m_dlgColorPalette)
	{
		m_dlgColorPalette.UpdateData( TRUE );
		m_dlgColorPalette.AddSelectedToMRU();
	}

	CDialog::OnOK();
}

//*****************************************************************************
//
// Function Name:  OnDestroy
//
// Description:    WM_DESTORY message handler
//
// Returns:        Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RColorDialog::OnDestroy( )
{
	CDialog::OnDestroy();
}

//*****************************************************************************
//
// Function Name:  RColorDialog::OnSelChangeTabs
//
// Description:    ON_CBN_SELCHANGE handler for the tab control
//
// Returns:        None
//
// Exceptions:	    None
//
//*****************************************************************************
void RColorDialog::OnSelChangeTabs( NMHDR*, LRESULT* pResult ) 
{
	ShowPage (m_ctlTabCtrl.GetCurSel()) ;
	*pResult = 0;

	if (GetFocus() != (CWnd *) &m_ctlTabCtrl)
	{
		m_pCurrentPage->GetTopWindow()->SetFocus() ;
	}
}

//*****************************************************************************
//
// Function Name:  RColorDialog::ShowPage
//
// Description:    Helper function for creating/displaying the correct
//						 property sheet in the tab control.
//
// Returns:        VOID
//
// Exceptions:	    None
//
//*****************************************************************************
void RColorDialog::ShowPage( int nPage )
{
	CDialog* pPrevPage  = m_pCurrentPage;
	CDialog* pPages[]   = { &m_dlgColorPalette, &m_dlgColorTexture, &m_dlgColorPhoto };
	int		pPageIDs[] = { DIALOG_COLOR_DIALOG_GRADIENTS, m_dlgColorTexture.IDD, m_dlgColorPhoto.IDD };

	TpsAssert( nPage < NumElements( pPages ), "Invalid page number!" );

	if (m_pCurrentPage != pPages[nPage])
	{
		m_pCurrentPage = pPages[nPage] ;

		if (!IsWindow( m_pCurrentPage->m_hWnd ))
		{
			CRect   rectAdjust(0,0,0,0);
			CRect   rectWindow(0,0,0,0);

			m_ctlTabCtrl.AdjustRect( TRUE, &rectAdjust );
			m_ctlTabCtrl.GetWindowRect( &rectWindow );

			rectWindow.left -= rectAdjust.left;
			rectWindow.top  -= rectAdjust.top;
			ScreenToClient( &rectWindow );

			if (!m_pCurrentPage->Create( pPageIDs[nPage], this ))
			{
				if (!pPrevPage) return ;

				// Restore the current page pointer to the previous one.
				m_pCurrentPage = pPrevPage ;
				pPrevPage = NULL;

				// Find the index of the previous page. (For setting the tab)
				for (int i = 0; i < NumElements( pPages ); i++)
				{
					if (m_pCurrentPage == pPages[i])
					{
						m_ctlTabCtrl.SetCurSel( i ) ;
						break ;
					}
				}
			}

			m_pCurrentPage->SetWindowPos( &wndTop, rectWindow.left,
				rectWindow.top, 0, 0, SWP_NOSIZE ) ;

		} // if (IsWindow())
			
	} // if (new page)

	if (pPrevPage && IsWindow( pPrevPage->m_hWnd ))
		pPrevPage->ShowWindow( SW_HIDE ) ;

	m_pCurrentPage->ShowWindow( SW_SHOW ) ;

	GetDlgItem( IDC_COLOR_MORE )->EnableWindow( m_pCurrentPage == &m_dlgColorPalette );
}

//****************************************************************************
//
// Function Name: SetColor
//
// Description:   Stores the current color for use in gradient option 
//						determination, and then updates the gradient selections.
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RColorDialog::SetColor( const RColor& crColor )
{
	if (RColor::kBitmap == crColor.GetFillMethod())
	{
		if (m_dlgColorPhoto.IsColorPhoto( crColor ))
		{
			m_dlgColorPhoto.SetColor( crColor );
			m_nStartPage = 2;
		}
		else
		{
			m_dlgColorTexture.SetColor( crColor );
			m_nStartPage = 1;
		}
	}
	else
	{
		m_dlgColorPalette.SetColor( crColor );
		m_nStartPage = 0;
	}
}

RColor RColorDialog::SelectedColor() const
{
	if (m_pCurrentPage == (CDialog *) &m_dlgColorTexture)
		return m_dlgColorTexture.SelectedColor();
	else if (m_pCurrentPage == (CDialog *) &m_dlgColorPhoto)
		return m_dlgColorPhoto.SelectedColor();

	return m_dlgColorPalette.SelectedColor();
}
