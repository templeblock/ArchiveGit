// ****************************************************************************
//
//  File Name:			WinPrinterStatus.cpp
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPrinterStatus class
//
//  Portability:		Windows dependent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WinPrinterStatus.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PrinterStatus.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "PrintStatusDialog.h"
#include "ApplicationGlobals.h"
#include "FrameworkResourceIDs.h"
#include "DrawingSurface.h"

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::RPrinterStatus( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterStatus::RPrinterStatus() :
	m_pStatusDialog( NULL ),
	m_yCurrentElementIndex( 0 ),
	m_yCurrentTotalElements( 0 ),
	m_fWaitCursorOn( FALSE )
{
	;
}
// ****************************************************************************
//
//  Function Name:	RPrinterStatus::~RPrinterStatus( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterStatus::~RPrinterStatus() 
{
	EndStatus();
}

// ****************************************************************************
//
//  Function Name:	staticPrintAbortProc	
//
//  Description:		Call back to allow print loop go on while status dlg is up
//
//  Returns:			CALLBACK
//
//  Exceptions:		NONE
//
// ****************************************************************************
//
BOOL CALLBACK staticPrintAbortProc(HDC, int)
{
	MSG msg;
	while (!RPrintStatusDialog::m_fCancelPrint && ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return FALSE;   // terminate if WM_QUIT received
	}
	return !RPrintStatusDialog::m_fCancelPrint;
}

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::StartStatus( )
//
//  Description:		Start the print status dialog
//
//  Returns:			VOID
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RPrinterStatus::StartStatus( RDrawingSurface* pDrawingSurface, const RMBCString& rPrinter, const RMBCString& rDocName, RCancelPrintingSignal* pCancelPrintingSignal, int nNumElements, int nNumPages ) 
{
	m_pStatusDialog = new RPrintStatusDialog( pDrawingSurface, pCancelPrintingSignal );
	//
	// Set abort proc for status dialog...
	int nResult = ::SetAbortProc( (HDC)pDrawingSurface->GetSurface(), staticPrintAbortProc );//rStatusDlg.PrintAbortProc );
	TpsAssert( SP_ERROR != nResult, "Setabortproc failed" );
	m_pStatusDialog->SetPrintStatusData( rPrinter, rDocName );
	m_pStatusDialog->Create( DIALOG_PRINT_STATUS );
	//
	// Disable entire app with exception to the status dialog, so the user can't change anything
	// while we're printing.
	AfxGetMainWnd()->EnableWindow(FALSE);
	m_pStatusDialog->EnableWindow(TRUE);

	// Save the number of elements, and initialize the current element to 0
	m_nNumElements = nNumElements;
	m_nCurrentElement = 0;

	// Save the number of pages, and initialize the current page to 0
	m_nNumPages = nNumPages;
	m_nCurrentPage = 0;

	// 
	// Start the wait cursor while print data is being initialized...
	::GetCursorManager().SetCursor( IDC_WAIT );
	m_fWaitCursorOn = TRUE;
	m_pStatusDialog->SetProgress( 1, m_nNumPages, 0, 0 );

}

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::EndStatus( )
//
//  Description:		Ends the print status dialog
//
//  Returns:			VOID
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterStatus::EndStatus() 
{
	//
	// Reenable application...
	if ( m_pStatusDialog )
	{
		AfxGetMainWnd()->EnableWindow(TRUE);
	}

	if ( m_pStatusDialog ) 
		m_pStatusDialog->DestroyWindow( );
	delete m_pStatusDialog;
	m_pStatusDialog = NULL;
	RPrintStatusDialog::m_fCancelPrint = FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::PrintElement( )
//
//  Description:		Updates the status indicator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterStatus::PrintElement( )
	{
	if (RPrintStatusDialog::m_fCancelPrint)
		return;

	// Increment the current element count
	++m_nCurrentElement;
	TpsAssert( m_nCurrentElement <= m_nNumElements, "Printed too many elements." );

	SetProgress( m_nCurrentPage, m_nNumPages, m_nCurrentElement, m_nNumElements );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::PrintPage( )
//
//  Description:		Updates the status indicator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterStatus::PrintPage( )
	{
	if (RPrintStatusDialog::m_fCancelPrint)
		return;

	++m_nCurrentPage;
	TpsAssert( m_nCurrentPage <= m_nNumPages, "Printed too many pages." );
	
	SetProgress( m_nCurrentPage, m_nNumPages, m_nCurrentElement, m_nNumElements );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::Reset( )
//
//  Description:		Reset the element counters
//
//  Returns:			TRUE to continue, FALSE otherwise
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
BOOLEAN RPrinterStatus::Reset( )
{
	//	Reset the base values.
	m_nNumElements	= 0;
	m_nCurrentElement	= 0;
	m_nNumPages= 0;
	m_nCurrentPage= 0;

	//	Update the dialog and reset the page values.
	return SetProgress( 0, 0, 0, 0 );
}

// ****************************************************************************
//
//  Function Name:	RPrinterStatus::SetProgress( )
//
//  Description:		Ends the print status dialog
//
//  Returns:			TRUE to continue, FALSE otherwise
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
BOOLEAN RPrinterStatus::SetProgress( YCounter yCurrentPage, YCounter yTotalPages
								, YCounter yCurrentElement, YCounter yTotalElements )
{
	m_yCurrentElementIndex = yCurrentElement;
	m_yCurrentTotalElements	= yTotalElements;
														
	//
	// The dialog is now showing feedback, so disable the wait cursor...
	if ( m_fWaitCursorOn )
	{
		::GetCursorManager().ClearCursor();
		m_fWaitCursorOn = FALSE;
	}

	HDC hdcDummy	= NULL;
	int nDummy		= 0;
	staticPrintAbortProc( hdcDummy, nDummy );
	if (RPrintStatusDialog::m_fCancelPrint)
	{
		return FALSE;
	}

	//	Use a 1 based indexing scheme to match user expectations.
	if ( yCurrentPage < yTotalPages )
		++yCurrentPage;

	//	If we actually have a status dialog update its display.
	if( m_pStatusDialog )
		m_pStatusDialog->SetProgress( yCurrentPage, yTotalPages, m_yCurrentElementIndex, m_yCurrentTotalElements );
	return TRUE;
}
