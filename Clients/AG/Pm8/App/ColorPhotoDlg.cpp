//****************************************************************************
//
// File Name:   ColorPhotoDlg.cpp
//
// Project:     Pretzel user interface
//
// Author:      Lance Wilson
//
// Description: Implementation of the CPColorPhotoDlg dialog object
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//					  500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1999 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ColorPhotoDlg.cpp                                     $
//   $Author:: Jfleischhauer                                                  $
//     $Date:: 3/08/99 2:41p                                                  $
// $Revision:: 2                                                              $
//
//****************************************************************************

#include "stdafx.h"
ASSERTNAME

#include "Pmw.h"
#include "PmwDoc.h"
#include "PmwCfg.h"

#include "ColorPhotoDlg.h"
#include "OpenDlgs.h"
#include "CompObj.h"
#include "TabbedEditImage.h"

// Interfaces
#include "ImageInterface.h"

// Framework support
#include "ImageColor.h"
#include "BitmapImage.h"
#include "DcDrawingSurface.h"
#include "ComponentTypes.h"
#include "ComponentDocument.h"
#include "ComponentView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*****************************************************************************
//
// Function Name:  CPColorPhotoDlg
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
CPColorPhotoDlg::CPColorPhotoDlg( CWnd* pParent ): 
	CDialog( IDD, pParent )
{
	//{{AFX_DATA_INIT(CPColorPhotoDlg)
	m_nTint = 0;
	//}}AFX_DATA_INIT
}

CPColorPhotoDlg::~CPColorPhotoDlg()
{
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
void CPColorPhotoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPColorPhotoDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CPColorPhotoDlg, CDialog )
	//{{AFX_MSG_MAP(CPColorPhotoDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED( IDC_BUTTON_IMPORT_PHOTO, OnPhotoImport )
	ON_BN_CLICKED( IDC_BUTTON_EDIT_PHOTO, OnPhotoEdit )
	ON_CBN_SELCHANGE( IDC_LIST_PHOTOS_TINT, OnSelChangeTint )
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
BOOL CPColorPhotoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* pWnd = GetDlgItem( IDC_STATIC_PHOTO_PREVIEW );
	pWnd->GetWindowText( m_strErrorText );
	pWnd->SetWindowText( "" );

	CComboBox* pComboBox = (CComboBox *) GetDlgItem( IDC_LIST_PHOTOS_TINT );
	pComboBox->SetCurSel( m_nTint );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

RColor CPColorPhotoDlg::SelectedColor() const
{
	return m_crColor;
}

void CPColorPhotoDlg::SetColor( RColor crColor )
{
	try
	{
		// Type casting to image color can throw
		// an exception, so this needs to be
		// done inside a try-catch block.
		RImageColor rImageColor = crColor;
		
		// Save off the color and tint
		m_crColor = crColor;
		m_nTint   = 10 - (rImageColor.GetTint() / 10);
	}
	catch (...)
	{
	}
}

BOOL CPColorPhotoDlg::IsColorPhoto( const RColor crColor ) const
{
	try
	{
		// Type casting to image color can throw
		// an exception, so this needs to be
		// done inside a try-catch block.
		RImageColor rImageColor = crColor;
		
		if (rImageColor.GetFlags() & RImageColor::kPhoto)
			return TRUE;
	}
	catch (...)
	{
	}

	return FALSE;
}

void CPColorPhotoDlg::OnPhotoImport()
{
	CString strFullDir;
	CString strFullName = GET_PMWAPP()->get_last_picture_name();
	
	if (!strFullName.IsEmpty())
	{
		strFullName = GetGlobalPathManager()->ExpandPath( strFullName, TRUE );
		int nPos = strFullName.ReverseFind( '\\' );

		if (nPos)
			strFullDir = strFullName.Left( nPos );
	}

	CString filters;
	GetConfiguration()->PictureFilter(filters);

	// Create the dialog.
	COpenPictureDlg picdlg( "", strFullDir.GetBuffer(0), filters, this );

	if (IDOK == picdlg.DoModal())
	{
		RImageColor rImageColor( RImageColor::kCentered | RImageColor::kPhoto );

		if (rImageColor.Initialize( picdlg.GetPathName() ))
			m_crColor = RColor( rImageColor );

		GetDlgItem( IDC_STATIC_PHOTO_PREVIEW )->Invalidate( TRUE );

		GET_PMWAPP()->set_last_picture_name( picdlg.GetPathName() );
	}
}

void CPColorPhotoDlg::OnPhotoEdit()
{
	try
	{
		// Type casting to image color can throw
		// an exception, so this needs to be
		// done inside a try-catch block.
		RImageColor rImageColor = m_crColor;
		
		// Create a new image component to hold our image.  Note, when we
		// set the image, the component will be resized, so a default size
		// of one inch is used for creation purposes only.
		RComponentDocument *pComponent = ComponentObject::CreateNewComponent(
			kImageComponent, RRealRect( 0, 0, kSystemDPI, kSystemDPI ) );

		RComponentView* pComponentView = (RComponentView *) pComponent->GetActiveView();
		RImageInterface* pInterface = (RImageInterface *) pComponentView->GetInterface( kImageInterfaceId );
		pInterface->SetImage( *(rImageColor.GetImage()) );
		delete pInterface;

		// invoke the image edit dialog
		RTabbedEditImage dlg( pComponent, kAdjustColorTab, NULL, this );

		if (dlg.DoModal() == IDOK)
		{
			// create a component with edited features
			RImageColor rImageColor( RImageColor::kCentered | RImageColor::kPhoto );

			if (rImageColor.Initialize( dlg.GetImage() ))
				m_crColor = RColor( rImageColor );

			GetDlgItem( IDC_STATIC_PHOTO_PREVIEW )->Invalidate( TRUE );
		}

		delete pComponent;
	}
	catch (...)
	{
	}
}

//****************************************************************************
//
// Function Name: OnSelChangeTint
//
// Description:   Selection change handler for the tint
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void CPColorPhotoDlg::OnSelChangeTint() 
{
	CComboBox* pComboBox = (CComboBox *) GetDlgItem( IDC_LIST_PHOTOS_TINT );
	int nSel = pComboBox->GetCurSel();

	if (LB_ERR != nSel)
	{
		try
		{
			// Type casting to image color can throw
			// an exception, so this needs to be
			// done inside a try-catch block.
			RImageColor rImageColor = m_crColor;
			rImageColor.SetTint( (10 - nSel) * 10 );
			m_crColor = RColor( rImageColor );

			GetDlgItem( IDC_STATIC_PHOTO_PREVIEW )->Invalidate( TRUE );
		}
		catch (...)
		{
		}
	}
}

HBRUSH CPColorPhotoDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	if (CTLCOLOR_STATIC == nCtlColor && IDC_STATIC_PHOTO_PREVIEW == pWnd->GetDlgCtrlID())
	{
		CRect rect;
		pWnd->GetClientRect( rect );

		if (m_crColor == RColor())
		{
			pDC->FillSolidRect( rect, ::GetSysColor( COLOR_WINDOW ) );
			pDC->DrawText( m_strErrorText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		}
		else
		{
			RDcDrawingSurface ds;
			ds.Initialize( pDC->GetSafeHdc() );

			RColor crFillColor = m_crColor;
			RIntRect rRect( rect );
			::ScreenUnitsToLogicalUnits( rRect );
			crFillColor.SetBoundingRect( rRect );

			RRealSize dpi( ::GetScreenDPI() );

			// The bounding rect is in logical units, so we
			// need to create a transform to scale from logical
			// units to device units.
			R2dTransform transform;
			transform.PostScale( dpi.m_dx / kSystemDPI, dpi.m_dy / kSystemDPI );
			crFillColor *= transform;

			ds.SetFillColor( crFillColor ) ;
			ds.FillRectangle( rect ) ;
			ds.DetachDCs();
		}

		return (HBRUSH) ::GetStockObject( NULL_BRUSH );
	}

	return CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}
