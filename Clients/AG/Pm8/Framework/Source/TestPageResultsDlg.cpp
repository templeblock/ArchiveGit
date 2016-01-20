//****************************************************************************
//
// File Name:  TestPageResultsDlg.cpp
//
// Project:    Renaissance application framework
//
// Author:     Eric VanHelene
//
// Description: Manages routines for the test page results Dialog's class.
//				
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//					    One Gateway Center, Suite 800
//					    Newton, Ma 02158
//					    (617)332-0202
//
// Client:		   Broderbund
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
// $Header:: /PM8/Framework $
//
//****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "TestPageResultsDlg.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "Printer.h"
#include "Document.h"
#include "PrintManager.h"

/////////////////////////////////////////////////////////////////////////////
// RTestPageResultsDlg dialog

RTestPageResultsDlg::RTestPageResultsDlg( RPrinter* pPrinter, CWnd* pParent /*=NULL*/)
: CDialog(DIALOG_TESTPAGERESULTS, pParent), m_pPrinter( pPrinter )
, m_yVerticalOffset( 0 ), m_yHorizontalOffset( 0 )
{
	//{{AFX_DATA_INIT(RTestPageResultsDlg)
	m_TestPageResultsData.yTopCalibration = 17;
	m_TestPageResultsData.yLeftCalibration = 17;
	m_TestPageResultsData.yRightCalibration = 17;
	m_TestPageResultsData.yBottomCalibration = 17;
	m_VerticalCalibration = _T("HH");
	m_HorizontalCalibration = _T("H");
	m_eTruncTest = ETruncationTest(0);
	m_eTransTest = ETransparencyTest(0);
	//}}AFX_DATA_INIT
}

RTestPageResultsDlg::~RTestPageResultsDlg()
{
	NULL;
}

void RTestPageResultsDlg::OnOK()
	{
	//	Get the printable area offset calculation. If the user's entered values
	//	are invalid then prompt them to retry.
	RAlert rDataInvalidAlert;

	BOOLEAN fDataValid = static_cast<BOOLEAN>( UpdateData( ) );

	m_VerticalCalibration.MakeUpper();
	m_HorizontalCalibration.MakeUpper();

	if ( m_VerticalCalibration	== _T("AA") )
		m_yHorizontalOffset = -7;
	else if ( m_VerticalCalibration	== _T("BB") )
		m_yHorizontalOffset = -6;
	else if ( m_VerticalCalibration	== _T("CC") )
		m_yHorizontalOffset = -5;
	else if ( m_VerticalCalibration	== _T("DD") )
		m_yHorizontalOffset = -4;
	else if ( m_VerticalCalibration	== _T("EE") )
		m_yHorizontalOffset = -3;
	else if ( m_VerticalCalibration	== _T("FF") )
		m_yHorizontalOffset = -2;
	else if ( m_VerticalCalibration	== _T("GG") )
		m_yHorizontalOffset = -1;
	else if ( m_VerticalCalibration	== _T("HH") )
		m_yHorizontalOffset = 0;
	else if ( m_VerticalCalibration	== _T("II") )
		m_yHorizontalOffset = 0;
	else if ( m_VerticalCalibration	== _T("JJ") )
		m_yHorizontalOffset = 1;
	else if ( m_VerticalCalibration	== _T("KK") )
		m_yHorizontalOffset = 2;
	else if ( m_VerticalCalibration	== _T("LL") )
		m_yHorizontalOffset = 3;
	else if ( m_VerticalCalibration	== _T("MM") )
		m_yHorizontalOffset = 4;
	else if ( m_VerticalCalibration	== _T("NN") )
		m_yHorizontalOffset = 5;
	else if ( m_VerticalCalibration	== _T("00") )
		m_yHorizontalOffset = 6;
	else if ( m_VerticalCalibration	== _T("PP") )
		m_yHorizontalOffset = 7;
	else
		{
		fDataValid = FALSE;
		//	Prompt user.
		rDataInvalidAlert.AlertUser( STRING_ERROR_DOCS_TESTPAGE_HOR_CALIBRATION_INVALID );
		return;
		}

	if ( m_HorizontalCalibration	== _T("A") )
		m_yVerticalOffset = -7;
	else if ( m_HorizontalCalibration	== _T("B") )
		m_yVerticalOffset = -6;
	else if ( m_HorizontalCalibration	== _T("C") )
		m_yVerticalOffset = -5;
	else if ( m_HorizontalCalibration	== _T("D") )
		m_yVerticalOffset = -4;
	else if ( m_HorizontalCalibration	== _T("E") )
		m_yVerticalOffset = -3;
	else if ( m_HorizontalCalibration	== _T("F") )
		m_yVerticalOffset = -2;
	else if ( m_HorizontalCalibration	== _T("G") )
		m_yVerticalOffset = -1;
	else if ( m_HorizontalCalibration	== _T("H") )
		m_yVerticalOffset = 0;
	else if ( m_HorizontalCalibration	== _T("I") )
		m_yVerticalOffset = 0;
	else if ( m_HorizontalCalibration	== _T("J") )
		m_yVerticalOffset = 1;
	else if ( m_HorizontalCalibration	== _T("K") )
		m_yVerticalOffset = 2;
	else if ( m_HorizontalCalibration	== _T("L") )
		m_yVerticalOffset = 3;
	else if ( m_HorizontalCalibration	== _T("M") )
		m_yVerticalOffset = 4;
	else if ( m_HorizontalCalibration	== _T("N") )
		m_yVerticalOffset = 5;
	else if ( m_HorizontalCalibration	== _T("O") )
		m_yVerticalOffset = 6;
	else if ( m_HorizontalCalibration	== _T("P") )
		m_yVerticalOffset = 7;
	else
		{
		fDataValid = FALSE;
		//	Prompt user.
		rDataInvalidAlert.AlertUser( STRING_ERROR_DOCS_TESTPAGE_VERT_CALIBRATION_INVALID );
		return;
		}

	if ( fDataValid )
	{
		CDialog::OnOK();
		RAlert alert;
		alert.InformUser( STRING_TEST_PAGE_DONE );
	}
	}

void RTestPageResultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RTestPageResultsDlg)
	DDX_Text(pDX, DIALOG_DEBUG_TESTPAGERESULTS_TOP_CALIBRATION, m_TestPageResultsData.yTopCalibration);
	DDV_MinMaxUInt(pDX, m_TestPageResultsData.yTopCalibration, 1, 32);
	DDX_Text(pDX, DIALOG_DEBUG_TESTPAGERESULTS_LEFT_CALIBRATION, m_TestPageResultsData.yLeftCalibration);
	DDV_MinMaxUInt(pDX, m_TestPageResultsData.yLeftCalibration, 1, 32);
	DDX_Text(pDX, DIALOG_DEBUG_TESTPAGERESULTS_RIGHT_CALIBRATION, m_TestPageResultsData.yRightCalibration);
	DDV_MinMaxUInt(pDX, m_TestPageResultsData.yRightCalibration, 1, 32);
	DDX_Text(pDX, DIALOG_DEBUG_TESTPAGERESULTS_BOTTOM_CALIBRATION, m_TestPageResultsData.yBottomCalibration);
	DDV_MinMaxUInt(pDX, m_TestPageResultsData.yBottomCalibration, 1, 32);
	DDX_Text(pDX, DIALOG_DEBUG_TESTPAGERESULTS_VERTICAL_CALIBRATION, m_VerticalCalibration);
	DDV_MaxChars(pDX, m_VerticalCalibration, 2);
	DDX_Text(pDX, DIALOG_DEBUG_TESTPAGERESULTS_HORIZONTAL_CALIBRATION, m_HorizontalCalibration);
	DDV_MaxChars(pDX, m_HorizontalCalibration, 2);

	// Set up spin control
	DDX_Control(pDX, DIALOG_DEBUG_TESTPAGERESULTS_TOP_CALIBRATION_SPIN, m_TopCalibrationSpinRotate);
	DDX_Control(pDX, DIALOG_DEBUG_TESTPAGERESULTS_LEFT_CALIBRATION_SPIN, m_LeftCalibrationSpinRotate);
	DDX_Control(pDX, DIALOG_DEBUG_TESTPAGERESULTS_RIGHT_CALIBRATION_SPIN, m_RightCalibrationSpinRotate);
	DDX_Control(pDX, DIALOG_DEBUG_TESTPAGERESULTS_BOTTOM_CALIBRATION_SPIN, m_BottomCalibrationSpinRotate);


	int	nTransTestSel = int(m_eTransTest);
	DDX_Radio( pDX, DIALOG_DEBUG_TESTPAGERESULTS_TRUNCATION_TEST_ABC, nTransTestSel );
	m_eTransTest = ETransparencyTest(nTransTestSel);

//	int	nTruncTestSel = int(m_eTruncTest);
//	DDX_Radio( pDX, DIALOG_DEBUG_TESTPAGERESULTS_TRANSPARENCY_TEST_CARD, nTruncTestSel );
//	m_eTruncTest = ETruncationTest(nTruncTestSel);
	//}}AFX_DATA_MAP
	//


}


BEGIN_MESSAGE_MAP(RTestPageResultsDlg, CDialog)
	//{{AFX_MSG_MAP(RTestPageResultsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RTestPageResultsDlg message handlers

//****************************************************************************
//
// Function Name: FillData
//
// Description:   Fills a TestPageResultsureStruct with the label info Dlg's data
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void RTestPageResultsDlg::FillData( TestPageResultsStruct* pMFData )
{
	*pMFData = m_TestPageResultsData;
}

//****************************************************************************
//
// Function Name: SetData
//
// Description:   Sets the Memory Failure Dlg's data with the TestPageResultsureStruct  
//
// Returns:       VOID
//
// Exceptions:	  None
//
//****************************************************************************
void RTestPageResultsDlg::SetData( TestPageResultsStruct* pMFData )
{
	m_TestPageResultsData = *pMFData;
}

//****************************************************************************
//
// Function Name: GetPrintableArea( )
//
// Description:   Return the printable area specified by the user. 
//
// Returns:       ^
//
// Exceptions:	  None
//
//****************************************************************************
RIntRect RTestPageResultsDlg::GetPrintableArea( )
	{
	//	Get the adjusted printable area from the test page panel.
	//	Note that there is only one panel in the test page project
	//	so index 0 is always defined.	
	
	return RPrintManager::GetPrintManager( ).CompensatePrintableArea( 
		RIntRect( m_TestPageResultsData.yLeftCalibration
			, m_TestPageResultsData.yTopCalibration
			, m_TestPageResultsData.yRightCalibration
			, m_TestPageResultsData.yBottomCalibration )
		, RIntSize( m_yHorizontalOffset, m_yVerticalOffset )
		, m_pPrinter );
	}

// ****************************************************************************
//
// Function Name:		RTestPageResultsDlg::OnInitDialog( )
//
// Description:		Initializes the dialog
//
// Returns:				See CDialog::OnInitDialog
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOL RTestPageResultsDlg::OnInitDialog( )
	{
	CDialog::OnInitDialog( );

	// Init the spin controls
	m_TopCalibrationSpinRotate.SetRange( kMinCalRange, kMaxCalRange );
	m_TopCalibrationSpinRotate.SetPos( kDefaultRotation );

	m_LeftCalibrationSpinRotate.SetRange( kMinCalRange, kMaxCalRange );
	m_LeftCalibrationSpinRotate.SetPos( kDefaultRotation );

	m_RightCalibrationSpinRotate.SetRange( kMinCalRange, kMaxCalRange );
	m_RightCalibrationSpinRotate.SetPos( kDefaultRotation );

	m_BottomCalibrationSpinRotate.SetRange( kMinCalRange, kMaxCalRange );
	m_BottomCalibrationSpinRotate.SetPos( kDefaultRotation );

	// Call the base method
	//BOOL fReturn = CDialog::OnInitDialog( );
	//return fReturn;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

// ****************************************************************************
//
//  Function Name:	RTestPageResultsDlg::OnTextChange()
//
//  Description:		
//							
//							
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************

void RTestPageResultsDlg::OnTextChange()
{
//	try
//	{
//		if ( m_bInitialised != FALSE && GetSafeHwnd != NULL && m_editRotation.GetSafeHwnd() != NULL )
//		{
//			CString szValue;
//			m_editRotation.GetWindowText( szValue );
//			int angle = atoi( szValue );
//			m_dialRotate.SetAngle( ::DegreesToRadians( angle ) );
//		}
//	}
//	catch( ... )
//	{
//	}
}
