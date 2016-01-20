//****************************************************************************
//
// File Name:  ExportGraphicDlg.cpp
//
// Project:    Renaissance user interface
//
// Author:     Lance Wilson
//
// Description: Implementation of the RExportGraphicDlg class
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software, Inc.
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ExportGraphicDlg.cpp                                  $
//   $Author:: Jfleischhauer                                                  $
//     $Date:: 3/08/99 2:43p                                                  $
// $Revision:: 2                                                              $
//
//****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "GraphicCompResource.h"
#include "ExportGraphicDlg.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

#include "BitmapImage.h"
#include "VectorImage.h"
#include "ImageInterface.h"
#include "OffscreenDrawingSurface.h"
#include "PictureDrawingSurface.h"
#include "ImageLibrary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kMinImageWidth   = 10 ;
const int kMinImageHeight  = 10 ;
const int kMaxImageWidth   = 1024 ;
const int kMaxImageHeight  = 768 ;
const int kDefaultWidth    = 160 ;

const int kNumFormats = 6 ;
const int kImageExportFilters[kNumFormats][2] = 
{
	{ kImageFormatBMP, STRING_FILTER_BMP },
	{ kImageFormatJPG, STRING_FILTER_JPG },
	{ kImageFormatTIF, STRING_FILTER_TIF },
	{ kImageFormatFPX, STRING_FILTER_FPX },
	{ kImageFormatGIF, STRING_FILTER_GIF },
	{ kImageFormatXRX, STRING_FILTER_WMF }
} ;

static int _nFilterIndex = 1 ;		// Session persistent filter index

//////////////////////////////////////////////////////////////////////////////
//
//	void ExportGraphicImage( RBitmapImage* pImage, RMBCString *pInitialDir ) 
//
//	export a graphic to file from a bitmap image
//
//////////////////////////////////////////////////////////////////////////////
void ExportGraphicImage( RBitmapImage* pImage, RMBCString *pInitialDir /*=NULL*/) 
{
	RImageLibrary rLibrary ;
	CArray<int, int> arrFileFormats ;

	//
	// Build the export file filter list...
	//
	CString strFilter, str ;

	for (int i = 0; i < kNumFormats - 1; i++)
	{
		if (rLibrary.IsFormatSupported( (EImageFormat) kImageExportFilters[i][0] ))
		{
			str.LoadString( kImageExportFilters[i][1] ) ;
			strFilter += str ;

			arrFileFormats.Add( kImageExportFilters[i][0] ) ;
		}
	}

	TpsAssert( kImageExportFilters[i][0] == kImageFormatXRX, "Invalid export format!" ) ;
	str.LoadString( kImageExportFilters[i][1] ) ;  
	strFilter += str ;
	strFilter += "|" ;

	arrFileFormats.Add( kImageExportFilters[i][0] ) ;

	//
	// Create and execute the dialog...
	//
	RExportGraphicDlg dlg( pImage, strFilter ) ;

	// Load in the dialog title string
	CString strTitle ;
	strTitle.LoadString( STRING_EXPORT_IMAGE_DIALOG_TITLE ) ;
	dlg.m_ofn.lpstrTitle = (LPCTSTR) strTitle ;
	if( pInitialDir )
	{
		dlg.m_ofn.lpstrInitialDir = (LPCTSTR)*pInitialDir;
	}

	if (IDOK == dlg.DoModal())
	{
		RMBCString   strPathName = dlg.GetPathName() ;
		EImageFormat eFormat = (EImageFormat) arrFileFormats[dlg.m_ofn.nFilterIndex - 1] ;

		try
		{
			if (kImageFormatXRX == eFormat)		// exporting as WMF
			{
				// get our output image size
				RRealSize rRealOutputSize( dlg.GetSize() );
				RRealSize screenDPI = ::GetScreenDPI();

				// create a vector image
				RVectorImage vectorImage;
 				vectorImage.SetSuggestedWidthInInches( ::PixelsToInches( rRealOutputSize.m_dx, (uLONG) screenDPI.m_dx ) );
				vectorImage.SetSuggestedHeightInInches( ::PixelsToInches( rRealOutputSize.m_dy, (uLONG) screenDPI.m_dy ) );

				// Create an offscreen drawing surface and set the picture
				ROffscreenDrawingSurface rODS;
				rODS.SetImage( &vectorImage );

		  		// Scale to the device DPI
				RRealSize rRealImageSize = pImage->GetSizeInLogicalUnits();
				R2dTransform transform ;
				RRealSize deviceDPI = rODS.GetDPI();

				transform.PreScale( (YFloatType) deviceDPI.m_dx / screenDPI.m_dy, 
									(YFloatType) deviceDPI.m_dy / screenDPI.m_dy );

				transform.PreScale( rRealOutputSize.m_dx / rRealImageSize.m_dx,
									rRealOutputSize.m_dy / rRealImageSize.m_dy ) ;

				// calculate the output rect
				RRealRect outputRect( rRealOutputSize ) ;
				::ScreenUnitsToLogicalUnits( outputRect );
				outputRect *= transform;

				// render to the offscreen
				pImage->Render( rODS, outputRect ) ;

				// clean up
				rODS.ReleaseImage();

				// export
				rLibrary.ExportImage( vectorImage, strPathName, kImageFormatXRX ) ;
				if (rLibrary.GetLastException() != kNoError)
				{
					throw rLibrary.GetLastException() ;
				}
			}
			else					// all other formats
			{
				_nFilterIndex = dlg.m_ofn.nFilterIndex ;
				
				EImageFormat eFormat = (EImageFormat) arrFileFormats[dlg.m_ofn.nFilterIndex - 1] ;
				rLibrary.ExportImage( *pImage, strPathName, eFormat );
				if (rLibrary.GetLastException() != kNoError)
				{
					throw rLibrary.GetLastException() ;
				}
	
			}
		}
		catch( YException e)
		{
			ReportException( e ) ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//	void ExportGraphicComponent( RComponentView* pComponentView, RMBCString *pInitialDir ) 
//
//	export a graphic to file from a component view
//
//////////////////////////////////////////////////////////////////////////////
void ExportGraphicComponent( RComponentView* pComponentView, RMBCString *pInitialDir /*=NULL*/) 
{
	RImageLibrary rLibrary ;
	CArray<int, int> arrFileFormats ;

	//
	// Build the export file filter list...
	//
	CString strFilter, str ;

	for (int i = 0; i < kNumFormats - 1; i++)
	{
		if (rLibrary.IsFormatSupported( (EImageFormat) kImageExportFilters[i][0] ))
		{
			str.LoadString( kImageExportFilters[i][1] ) ;
			strFilter += str ;

			arrFileFormats.Add( kImageExportFilters[i][0] ) ;
		}
	}

	TpsAssert( kImageExportFilters[i][0] == kImageFormatXRX, "Invalid export format!" ) ;
	str.LoadString( kImageExportFilters[i][1] ) ;  
	strFilter += str ;
	strFilter += "|" ;

	arrFileFormats.Add( kImageExportFilters[i][0] ) ;

	//
	// Create and execute the dialog...
	//
	RExportGraphicDlg dlg( pComponentView, strFilter ) ;

	// Load in the dialog title string
	CString strTitle ;
	strTitle.LoadString( STRING_EXPORT_IMAGE_DIALOG_TITLE ) ;
	dlg.m_ofn.lpstrTitle = (LPCTSTR) strTitle ;
	if( pInitialDir )
	{
		dlg.m_ofn.lpstrInitialDir = (LPCTSTR)*pInitialDir;
	}

	if (IDOK == dlg.DoModal())
	{
		RMBCString   strPathName = dlg.GetPathName() ;
		RIntSize     szImageSize = dlg.GetSize() ;

		_nFilterIndex = dlg.m_ofn.nFilterIndex ;
		EImageFormat eFormat = (EImageFormat) arrFileFormats[dlg.m_ofn.nFilterIndex - 1] ;
		RRealSize pictureSize  = pComponentView->GetReferenceSize() ;

		try
		{
			if (kImageFormatXRX == eFormat)
			{
				RRealSize outputSize( szImageSize.m_dx, szImageSize.m_dy ) ; //   = pictureSize ;
				RRealSize screenDPI    = ::GetScreenDPI() ;

				RVectorImage vectorImage ;
				vectorImage.SetSuggestedWidthInInches( ::PixelsToInches( outputSize.m_dx, (uLONG) screenDPI.m_dx ) ) ;
				vectorImage.SetSuggestedHeightInInches( ::PixelsToInches( outputSize.m_dy, (uLONG) screenDPI.m_dy ) ) ;

				// Create an offscreen drawing surface and set the picture
				ROffscreenDrawingSurface drawingSurface;
				drawingSurface.SetImage( &vectorImage );

				R2dTransform transform ;

				// Scale to the device DPI
				RRealSize deviceDPI = drawingSurface.GetDPI( );
				transform.PreScale( (YFloatType) deviceDPI.m_dx / screenDPI.m_dy, 
					(YFloatType) deviceDPI.m_dy / screenDPI.m_dy );

				transform.PreScale( 
					outputSize.m_dx / pictureSize.m_dx,
					outputSize.m_dy / pictureSize.m_dy ) ;

				// Render the component
				RRealRect outputRect( pictureSize ) ;
				pComponentView->Render( drawingSurface, transform, outputRect ) ;

				drawingSurface.ReleaseImage( );

				rLibrary.ExportImage( vectorImage, strPathName, kImageFormatXRX ) ;
				if (rLibrary.GetLastException() != kNoError)
				{
					throw rLibrary.GetLastException() ;
				}
			} // if (XRX)
			else
			{
				// Check for a image interface
				RImageInterface* pInterface = (RImageInterface *) 
					pComponentView->GetInterface( kImageInterfaceId ) ;

				if (pInterface)
				{
					pInterface->Export( strPathName, eFormat ) ;
					delete pInterface ;
				}
				else
				{
					// Initialize the new bitmap at a bit depth of 24
					RBitmapImage image ;
					image.Initialize( szImageSize.m_dx, szImageSize.m_dy, 24 ) ;

					ROffscreenDrawingSurface dsMem ; 
					dsMem.SetImage( &image ) ;

					R2dTransform transform ;
					transform.PreScale( 
						szImageSize.m_dx / pictureSize.m_dx,
						szImageSize.m_dy / pictureSize.m_dy ) ;

					// Render the component
					dsMem.SetFillColor( RSolidColor( kWhite ) ) ;
					dsMem.FillRectangle( RRealRect( pictureSize ), transform ) ;
					pComponentView->Render( dsMem, transform, RIntRect( 
						RIntSize( pictureSize.m_dx, pictureSize.m_dy ) ) ) ;

					dsMem.ReleaseImage() ;
					
					rLibrary.ExportImage( image, strPathName, eFormat ) ;
				}

				if (rLibrary.GetLastException() != kNoError)
				{
					throw rLibrary.GetLastException() ;
				}
			
			} // else

		} // try

		catch( YException e)
		{
			ReportException( e ) ;
		}

	} // if (IDOK)
}

/////////////////////////////////////////////////////////////////////////////
// RExportGraphicDlg

IMPLEMENT_DYNAMIC(RExportGraphicDlg, CFileDialog)

//////////////////////////////////////////////////////////////////////////////
//
//	RExportGraphicDlg::RExportGraphicDlg( RComponentView* pComponentView, CString& strFilter ) 
//
//	dialog constructor using a component view
//
//////////////////////////////////////////////////////////////////////////////
RExportGraphicDlg::RExportGraphicDlg( RComponentView* pComponentView, CString& strFilter ) :
	CFileDialog( FALSE, strFilter, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_PATHMUSTEXIST | OFN_EXPLORER, strFilter, NULL ) 
{
	m_ofn.hInstance         = AfxGetResourceHandle() ;
	m_ofn.lpTemplateName    = MAKEINTRESOURCE( DIALOG_EXPORT_IMAGE_DATA ) ;
	m_ofn.nFilterIndex      = _nFilterIndex ;
	m_ofn.Flags            |= OFN_ENABLETEMPLATE ;

	SetHelpID( DIALOG_EXPORT_IMAGE_DATA );

	// REVIEW: Setup initial directory

	// Obtain the current size from the component view.
	TpsAssert( pComponentView, "No component provided!" ) ;

	// REVIEW 5/1/97 GKB Eliminated call to GetPreferredAspectRatio() function and replaced
	//					  with computation based on the size of the view.  Since this code is
	//					  not being called from anywhere in the application, there is no way to test
	//					  it.  If the actual aspect ratio of the graphic is needed instead of the 
	//					  aspect ratio of the view, it can be obtained via the graphic interface.
	//					  
	RRealSize viewSize = pComponentView->GetReferenceSize();
	m_flScaleFactor     = viewSize.m_dx / viewSize.m_dy;
	m_fMaintainAspect   = TRUE ;
	m_fExportingImage   = FALSE ;

	RImageInterface* pInterface = (RImageInterface*)	
		pComponentView->GetInterface( kImageInterfaceId ) ;

	if (pInterface)
	{
		m_szExportSize.m_dx = viewSize.m_dx ;
		m_szExportSize.m_dy = viewSize.m_dy ;
		m_fExportingImage   = TRUE ;

		::LogicalUnitsToScreenUnits( m_szExportSize ) ;
		delete pInterface ;
	}
	else if (viewSize.m_dx > viewSize.m_dy)
	{
		m_szExportSize.m_dx = kDefaultWidth ;
		m_szExportSize.m_dy = kDefaultWidth / m_flScaleFactor ;
	}
	else
	{
		m_szExportSize.m_dy = kDefaultWidth ;
		m_szExportSize.m_dx = kDefaultWidth * m_flScaleFactor ;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//	RExportGraphicDlg::RExportGraphicDlg( RBitmapImage* pImage, CString& strFilter ) 
//
//	dialog constructor using a bitmap image
//
//////////////////////////////////////////////////////////////////////////////
RExportGraphicDlg::RExportGraphicDlg( RBitmapImage* pImage, CString& strFilter ) :
	CFileDialog( FALSE, strFilter, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_PATHMUSTEXIST | OFN_EXPLORER, strFilter, NULL ) 
{
	m_ofn.hInstance         = AfxGetResourceHandle() ;
	m_ofn.lpTemplateName    = MAKEINTRESOURCE( DIALOG_EXPORT_IMAGE_DATA ) ;
	m_ofn.nFilterIndex      = _nFilterIndex ;
	m_ofn.Flags            |= OFN_ENABLETEMPLATE ;

	SetHelpID( DIALOG_EXPORT_IMAGE_DATA );

	m_szExportSize = pImage->GetSizeInLogicalUnits() ;
	::LogicalUnitsToScreenUnits( m_szExportSize ) ;
	
	m_fExportingImage   = TRUE ;

}

RExportGraphicDlg::~RExportGraphicDlg()
{
}

BEGIN_MESSAGE_MAP(RExportGraphicDlg, CFileDialog)
	//{{AFX_MSG_MAP(RExportGraphicDlg)
	ON_BN_CLICKED( ID_HELP, OnHelp )
	ON_BN_CLICKED( CONTROL_CHKBOX_EXPORT_ASPECT, OnMaintainAspectChange )
	ON_EN_KILLFOCUS( CONTROL_EDIT_EXPORT_WIDTH, OnKillFocusExportWidth )
	ON_EN_KILLFOCUS( CONTROL_EDIT_EXPORT_HEIGHT, OnKillFocusExportHeight )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void RExportGraphicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RBackdropBrowserDlg)
	DDX_Text( pDX, CONTROL_EDIT_EXPORT_WIDTH, m_szExportSize.m_dx ) ;
	DDX_Text( pDX, CONTROL_EDIT_EXPORT_HEIGHT, m_szExportSize.m_dy );
	//}}AFX_DATA_MAP
}

BOOL RExportGraphicDlg::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	CheckDlgButton( CONTROL_CHKBOX_EXPORT_ASPECT, m_fMaintainAspect ) ;

	// Disable sizing controls 
	// when exporting images
	if (m_fExportingImage)
	{
		CWnd* pWnd ;

		pWnd = GetDlgItem( CONTROL_CHKBOX_EXPORT_ASPECT ) ;
		pWnd->EnableWindow( FALSE ) ;

		pWnd = GetDlgItem( CONTROL_EDIT_EXPORT_HEIGHT ) ;
		pWnd->EnableWindow( FALSE ) ;

		pWnd = GetDlgItem( CONTROL_EDIT_EXPORT_WIDTH ) ;
		pWnd->EnableWindow( FALSE ) ;
	}
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RExportGraphicDlg::OnMaintainAspectChange() 
{
	m_fMaintainAspect = !m_fMaintainAspect ;

	if (m_fMaintainAspect)
	{
		m_szExportSize.m_dy = m_szExportSize.m_dx / m_flScaleFactor + 0.5 ;
		SetDlgItemInt( CONTROL_EDIT_EXPORT_HEIGHT, m_szExportSize.m_dy ) ;
	}
}

void RExportGraphicDlg::OnKillFocusExportWidth()
{
	if (UpdateData( TRUE ) && m_fMaintainAspect)
	{
		m_szExportSize.m_dy = m_szExportSize.m_dx / m_flScaleFactor + 0.5 ;
		SetDlgItemInt( CONTROL_EDIT_EXPORT_HEIGHT, m_szExportSize.m_dy ) ;
	}
}

void RExportGraphicDlg::OnKillFocusExportHeight()
{
	if (UpdateData( TRUE ) && m_fMaintainAspect)
	{
		m_szExportSize.m_dx = m_szExportSize.m_dy * m_flScaleFactor ;
		SetDlgItemInt( CONTROL_EDIT_EXPORT_WIDTH, m_szExportSize.m_dx ) ;
	}
}

void RExportGraphicDlg::OnHelp()
{
   AfxGetApp()->WinHelp( 0x20000 + DIALOG_EXPORT_IMAGE_DATA ) ;
}
