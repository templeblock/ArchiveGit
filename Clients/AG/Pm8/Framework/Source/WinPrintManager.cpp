// ****************************************************************************
//
//  File Name:			WinPrintManager.cpp
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPrinter class
//
//  Portability:		Platform independent
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
//  $Logfile:: /PM8/Framework/Source/WinPrintManager.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PrintManager.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"Document.h"
#include	"PrinterDrawingSurface.h"
#include "TestPageResultsDlg.h"
#include "FrameworkResourceIDs.h"
#include "ResourceManager.h"
#include "ApplicationGlobals.h"
#include "PrinterDatabase.h"
#include "StandaloneApplication.h"
//#include "WinPrintCalibration.h"

void	FrameSides( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RPrinter& printer );

void	FrameSides( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RIntRect& printableArea );

void	DrawCalibrationCenter( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& pageArea, RIntRect rTopTitleRect
			, RIntRect rBottomTitleRect );

void	DrawTopCalibrationString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& rTopTitleRect );

void	DrawBottomCalibrationString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& rBottomTitleRect );

void	GetActualTextOutRect( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, RIntRect& rTitleRect, int nTextResourceID );

YIntDimension	GetFontHeight( RDrawingSurface& drawingSurface );

//	Print manager constants.
const	uLONG				kEnumPrintersFlags					= MacWinDos( UNDEFINED, PRINTER_ENUM_LOCAL , UNDEFINED );
const BOOLEAN			kFrame									= TRUE;

//	Number of printer confirmation page elements per page.
const YCounter kNumPrinterConfirmationPagePrintElements = 4;

// ****************************************************************************
//
//  Function Name:	RPrintManager::GetAttachedPrinters( )
//
//  Description:		Populate list of installed printers.
//
//  Returns:			return TRUE if printers were added to the attached list
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintManager::GetAttachedPrinters( )
	{
	BOOLEAN fSucceeded = FALSE;
	YCounter yOriginalNumPrinters = m_AttachedPrinters.Count( );

	typedef PRINTER_INFO_1 PrinterStructType;
   PrinterStructType* pPrinterEnum = NULL;// pointer to buffer to receive printer info structures 
	int	nStructureType = 1;				//	type of structure for above ptr
	DWORD cbBuf = 0;							// size, in bytes, of array 
	DWORD cbNeeded = 0;						// variable with no. of bytes copied (or required) 
	DWORD cReturned = 0;						// variable with no. of printer info. structures copied 

	//	Get size of array to allocate.
	//	Ignore return value when getting the buffer size since EnumPrinters LIES.
	::EnumPrinters( 
		kEnumPrintersFlags
		, NULL								//	no name used
		, nStructureType					//	type of structure to fill
		, NULL								//	no buffer used
		, cbBuf								//	no bytes in buffer
		, &cbNeeded							//	get the size of the buffer
		, &cReturned );					//	number of structures
	
	//	Get printer info into array.
	DWORD cbUsed;							// size, in bytes, of array actually used

	try
		{
		//	Allocate the printer array
		pPrinterEnum = (PrinterStructType*)new BYTE[ cbNeeded ];
		::memset( pPrinterEnum, 0, cbNeeded );
		}
	catch( ... )
		{
		fSucceeded = FALSE;
		cReturned = 0;
		}

	if ( pPrinterEnum )	
		{
		//	Get the array of attached printers.
		fSucceeded = static_cast<BOOLEAN>(::EnumPrinters( 
			kEnumPrintersFlags
			, NULL								//	no name used
			, nStructureType					//	type of structure to fill
			, (PUBYTE)pPrinterEnum			//	printer info buffer
			, cbNeeded							//	bytes in buffer
			, &cbUsed							//	get the size of the buffer used
			, &cReturned ));					//	number of structures

		TpsAssert( cbUsed <= cbNeeded, "Wrong buffer size passed to EnumPrinters" );

		//	Add info for each printer to the list of attached printers.
		PSBYTE pubPort = NULL;
		PSBYTE pubDriverName = NULL;
	   PrinterStructType* pPrinterInfo = NULL;	// pointer to current printer info structures
		
		//	Only set the default printer if the print manager hasn't been initialized yet.
		BOOLEAN fFoundDefault = m_fPrintManagerInitialized;
		for( DWORD i = 0; i < cReturned; ++i )
			{
			//	Get the port and driver names from the description info.
			//	Rely on the c libs (goofy) internal statics for the string ptr location in the
			//	second call to strtok.
			pPrinterInfo = pPrinterEnum + i;

			//	Make a new printer
			//	Use the following code for PRINTER_INFO_1 structures.
			pubPort = strtok( pPrinterInfo->pDescription, "," );
			pubDriverName = strtok( NULL, "," );
			RPrinter newPrinter = RPrinter(	pPrinterInfo->pName, pubDriverName, pubPort );

			//	Talk to the driver to get valid values for all name fields so
			//	that we may compare this printer with others reliably.
			
			//	We do a full update here instead of just using values from the 
			//	registry so that we may compare printers with each other correctly.
			//	The full update is more time consuming than reading the registry but
			// we have to do it.
			//	We cannot just use the printers name and driver name since the
			//	same printer may be attached to several ports and we might
			//	choose the wrong one if we did not talk to the driver to get 
			//	accurate values.
			// Dont update the paper area values yet
			newPrinter.Update( FALSE );

			//	If the printer isn't already on the list of attached printers then add it.
			//	Don't check the attached list for the printer if we are in the middle of initializing
			//	the print manager.
			RPrinter* pExistingPrinter = NULL;
			if ( m_fPrintManagerInitialized )
				pExistingPrinter = FindPrinter( &newPrinter, kCompareAllNames | kCompareRequireTotalConfidence );

			if ( !m_fPrintManagerInitialized || !pExistingPrinter )
				{
				// Do a complete update
				newPrinter.Update( );				
				m_AttachedPrinters.InsertAtEnd( newPrinter );
				}

			else
				//	Make sure the printer is enabled even if it was previously deleted and is 
				//	now being reattached.
				if ( pExistingPrinter )
					pExistingPrinter->Enable( );		

			//	Use the following code for PRINTER_INFO_2 structures.
			//			if ( pPrinterInfo->Attributes & PRINTER_ATTRIBUTE_DEFAULT )
//				{
//				m_CurrentPrinter = i;
//				TpsAssert( !fFoundDefault, "Found two different default printers" );
//				fFoundDefault = TRUE;
//				}
//			m_AttachedPrinters.InsertAtEnd( RPrinter(	pPrinterInfo->pPrinterName, pPrinterInfo->pDriverName, pPrinterInfo->pPortName ) );
			}

		//	If we didn't find the default printer based on the attribute flags look again.
		//	This can happen under nt 4 or when we are not using PRINTER_INFO_2 structures.
		if ( !fFoundDefault )
			{
			//	Get its name and index in the printer list.
			RMBCString rDriverName;
			if( GetSystemDefaultPrinterName( rDriverName ) )
				fFoundDefault = SetDefaultPrinter( rDriverName );
			}

		//	Clean up the array of print info structures.
		delete [] pPrinterEnum;
		}
		
	//	If we found no printers above then
	//	add a template printer for use as the default.
	if ( 0 == m_AttachedPrinters.Count( ) )
		m_AttachedPrinters.InsertAtEnd( RPrinter( kTemplatePrinter ) );

	//	REVIEW ESV 10/1/96- Should we warn the user here if ( !fSucceeded )? Probably not.

	//	Return TRUE if we added printers to the attached list.
	return static_cast<BOOLEAN>( yOriginalNumPrinters < m_AttachedPrinters.Count( ) );
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::GetSystemDefaultPrinterName( RMBCString& rDriverName )
//
//  Description:		Gets the default printer name from the os.
//
//  Returns:			TRUE if the default printer was found.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintManager::GetSystemDefaultPrinterName( RMBCString& rDriverName )
	{
	BOOLEAN fSuccess = FALSE;

#if 0
	//	This code is much slower than the code below which uses the mfc and ::PrintDlg.
	//	Keep this version around in case it does a better job since the docs say ::PageSetupDlg 
	// is the api to use with win32.
	PAGESETUPDLG psd;
	::memset( &psd, 0, sizeof( PAGESETUPDLG ) );
	psd.lStructSize = sizeof( PAGESETUPDLG );
	psd.Flags = PSD_RETURNDEFAULT | PSD_NOWARNING;
	if ( ::PageSetupDlg(&psd) )
		{
		LPDEVMODE pDevMode = (LPDEVMODE)::GlobalLock( psd.hDevMode );
		rDriverName = RMBCString( pDevMode->dmDeviceName );
		BOOLEAN fUnlocked = static_cast<BOOLEAN>( ::GlobalUnlock( psd.hDevMode ) );
		fSuccess = TRUE;
		}
#endif
	PRINTDLG printDialog;
	if( AfxGetApp( )->GetPrinterDeviceDefaults( &printDialog ) )
		{
		//	Get driver name of current printer
		LPDEVMODE pDevMode = (LPDEVMODE)::GlobalLock( printDialog.hDevMode );
		rDriverName = RMBCString( pDevMode->dmDeviceName );
		BOOLEAN fUnlocked = static_cast<BOOLEAN>( ::GlobalUnlock( printDialog.hDevMode ) );
		
		fSuccess = TRUE;
		}
	
	return fSuccess;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::TestPrinter( )
//
//  Description:		Print a test page and record the user's response in the
//							user printer db.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::TestPrinter( )
{
	UnimplementedCode( );
}

// ****************************************************************************
//
//  Function Name:	RPrintManager::CalibratePrinter( )
//
//  Description:		Print a test page.
//
//  Returns:			TRUE: the user did not cancel printing the test page.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintManager::CalibratePrinter( RPrinter* pPrinter, BOOLEAN fShowStatus /*= !kShowStatus*/
													 , YCounter numCalibrationPages /*= 1*/ )
{
	// DCS (6/21/97)
	// throw away the previous calibration results so that we get a completely
	// uncalibrated test page, using the printable area as specified by the printer driver.
	pPrinter->Update( TRUE, FALSE );

	RCancelPrintingSignal cancelPrintingSignal;
	RPrinterDrawingSurface* pDrawingSurface = pPrinter->CreateDrawingSurface( &cancelPrintingSignal );

	//	Send the test page to the printer.
	RMBCString rDocTitle;
	rDocTitle = GetResourceManager( ).GetResourceString( STRING_TEST_PAGE_DOC_TITLE );

	//	Show a printer status dialog if requested.
	//	Don't show it if we're on a later page of a multi-page calibration.
	RPrinterStatus* pPrinterStatus = RPrintManager::GetPrintManager( ).GetStatus( );
	if ( fShowStatus && 0 == pPrinterStatus->m_nCurrentPage )
		pPrinterStatus->StartStatus( pDrawingSurface, pPrinter->GetPrinterName( ), rDocTitle
			, &cancelPrintingSignal, kNumPrinterTesPagePrintElements * kNumPrinterTesPagePrintElements
			, numCalibrationPages );

	pDrawingSurface->GetDocInfo( ).Initialize( rDocTitle, pPrinter->GetPortName( ) );

	try
		{
		//	Start the doc.
		if ( pDrawingSurface && !pDrawingSurface->DocStarted( ) )
			pDrawingSurface->BeginDoc( );
		
		//	Start the page.
		if ( pDrawingSurface && !pDrawingSurface->PageStarted( ) )
			pDrawingSurface->BeginPage( );

		//	Print the test page.
		PrintTestPage( *pDrawingSurface, R2dTransform( ), *pPrinter, RIntRect( ), pPrinterStatus );
		}

	catch( ... )
		{
		//	REVIEW ESV - what else should we do here?
		NULL;
		}

	//	Finish the page.
	if ( pDrawingSurface && pDrawingSurface->PageStarted( ) )
		pDrawingSurface->EndPage( );

	//	Finish the doc.
	if ( pDrawingSurface && pDrawingSurface->DocStarted( ) )
		pDrawingSurface->EndDoc( );

	//	Hide the status dialog if we're done.
	if ( pPrinterStatus && pPrinterStatus->m_nCurrentPage >= pPrinterStatus->m_nNumPages )
		pPrinterStatus->EndStatus( );

	BOOLEAN fCanceled = cancelPrintingSignal.IsPrintingCanceled( );
	
	//	Clean up.
	pPrinter->DestroyDrawingSurface( pDrawingSurface );

	return !fCanceled;
}

// ****************************************************************************
//
//  Function Name:	RPrintManager::ConfirmPrinter( )
//
//  Description:		Print a confirmation page.
//
//  Returns:			TRUE: the user did not cancel printing the confirmation page.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintManager::ConfirmPrinter( RPrinter* pPrinter
												  , const RIntRect& printableArea 
												  , BOOLEAN fShowStatus /*= !kShowStatus*/
												  , YCounter numConfirmationPages /*= 1*/ )
{
	// DCS (6/21/97)
	// throw away the previous calibration results so that we get a completely
	// uncalibrated test page, using the printable area as specified by the printer driver.
	pPrinter->Update( TRUE, FALSE );

	RCancelPrintingSignal cancelPrintingSignal;
	RPrinterDrawingSurface* pDrawingSurface = pPrinter->CreateDrawingSurface( &cancelPrintingSignal );

	//	Send the test page to the printer.
	RMBCString rDocTitle;
	rDocTitle = GetResourceManager( ).GetResourceString( STRING_TEST_PAGE_DOC_TITLE );

	//	Show a printer status dialog if requested.
	//	Don't show it if we're on a later page of a multi-page calibration.
	RPrinterStatus* pPrinterStatus = RPrintManager::GetPrintManager( ).GetStatus( );
	if ( fShowStatus && 0 == pPrinterStatus->m_nCurrentPage )
		pPrinterStatus->StartStatus( pDrawingSurface, pPrinter->GetPrinterName( ), rDocTitle
			, &cancelPrintingSignal
			, kNumPrinterConfirmationPagePrintElements * kNumPrinterConfirmationPagePrintElements
			, numConfirmationPages );

	pDrawingSurface->GetDocInfo( ).Initialize( rDocTitle, pPrinter->GetPortName( ) );

	try
		{
		//	Start the doc.
		if ( pDrawingSurface && !pDrawingSurface->DocStarted( ) )
			pDrawingSurface->BeginDoc( );
		
		//	Start the page.
		if ( pDrawingSurface && !pDrawingSurface->PageStarted( ) )
			pDrawingSurface->BeginPage( );

		//	Print the test page.
		PrintConfirmationPage( *pDrawingSurface, R2dTransform( ), *pPrinter, RIntRect( )
			, printableArea, pPrinterStatus );
		}

	catch( ... )
		{
		//	REVIEW ESV - what else should we do here?
		NULL;
		}

	//	Finish the page.
	if ( pDrawingSurface && pDrawingSurface->PageStarted( ) )
		pDrawingSurface->EndPage( );

	//	Finish the doc.
	if ( pDrawingSurface && pDrawingSurface->DocStarted( ) )
		pDrawingSurface->EndDoc( );

	//	Hide the status dialog if we're done.
	if ( pPrinterStatus && pPrinterStatus->m_nCurrentPage >= pPrinterStatus->m_nNumPages )
		pPrinterStatus->EndStatus( );

	BOOLEAN fCanceled = cancelPrintingSignal.IsPrintingCanceled( );
	
	//	Clean up.
	pPrinter->DestroyDrawingSurface( pDrawingSurface );

	return !fCanceled;
}

// ****************************************************************************
//
//  Function Name:	RPrintManager::TestPrinter( )
//
//  Description:		Print a test page and record the user's response in the
//							user printer db.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::TestPrinter( RPrinter* pPrinter )
	{
	//	Print the test page and 
	// get the user's response if the user didn't cancel the test page.
	if( CalibratePrinter( pPrinter ) )
		{
		RTestPageResultsDlg rTestPageResultsDlg( pPrinter );

		if ( IDOK == rTestPageResultsDlg.DoModal( ) )
			{	
			RIntRect rNewPrintableArea = rTestPageResultsDlg.GetPrintableArea( );
			
			if ( m_pUserDatabase )
				{
				//	Update the printer in the printer database.
				m_pUserDatabase->UpdatePrinter( pPrinter, rNewPrintableArea );
				
				//	Update the printer in the open print manager.
				RPrintManager::GetPrintManager( ).UpdatePrinterFromDatabase( pPrinter );
			
				//	Save changed database.
				m_pUserDatabase->Update( );
				}
			}
			else
				{
				// DCS (6/21/97)
				// because we had to blow away the previous calibration results
				// before we printed, if the user cancels the test print
				// we have to re-apply the printer's calibration from the printer database.
				RPrintManager::GetPrintManager( ).UpdatePrinterFromDatabase( pPrinter );
				}
			}

	// DCS 6/17/97 - this next call will force all documents which
	// use the printer which we have just calibrated to update (including the preview).
	::GetApplication( ).OnXPrinterChanged( pPrinter->GetPrinterName( ) );
	}


// ****************************************************************************
//
//  Function Name:	RTestPagePanel::PrintTestPage( )
//
//  Description:		Renders the test page into the give output target
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::PrintTestPage( RDrawingSurface& drawingSurface,
									  const R2dTransform& /*transform*/,
									  const RPrinter& printer,
									  const RIntRect& /* rcRender */,
									  RPrinterStatus* pPrinterStatus /*=NULL*/ ) const
	{
	RIntRect printableArea = printer.GetPrintableArea( );
	RIntRect pageArea = printer.GetPaperRect( );

	//	We must use our own transform since we short circuit many project printing 
	//	conventions.
	R2dTransform paperOrientedTransform;
	printer.GetBaseTransform( paperOrientedTransform, &drawingSurface );
	
	//	Compensate for the offset from the distance from the physical page side to 
	//	the begininng of the printable area.
	paperOrientedTransform.PreTranslate( -printableArea.m_Left, -printableArea.m_Top );	

	//	prep drawing
	drawingSurface.RestoreDefaults( );
	drawingSurface.SetPenColor( RSolidColor( kBlack ) );
	drawingSurface.SetPenWidth( 1 );
	drawingSurface.SetPenStyle( kSolidPen );
	drawingSurface.SetClipRect( pageArea );

	//	Draw the side and center markers.
	::DrawSideMarkers( drawingSurface, paperOrientedTransform, printer );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	::DrawCenterMarkers( drawingSurface, paperOrientedTransform, pageArea );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	//	Draw the drawing test strings.
	
	//	Get the string locations
	//	Put the test strings above and to the right of the page center calibrations.
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );
	RIntRect rTopTitleRect( rPageCenter.m_x, rPageCenter.m_y - printableArea.Height( )/4, printableArea.m_Right, rPageCenter.m_y );
	RIntRect rBottomTitleRect( rTopTitleRect );
	rTopTitleRect.m_Bottom = ::InchesToLogicalUnits( kCalibrationTextOffset );
	rTopTitleRect.m_Top = rTopTitleRect.m_Bottom - ::InchesToLogicalUnits( kCalibrationFontHeight );
	rBottomTitleRect.m_Top = rBottomTitleRect.m_Bottom - ::InchesToLogicalUnits( kCalibrationTextOffset );
	rBottomTitleRect.m_Bottom = rBottomTitleRect.m_Top + ::InchesToLogicalUnits( kCalibrationFontHeight );

	//	Draw the drawing test string one.
	//	String one defines the title rect we use below.
	RIntRect rTitleRect;
	::DrawTestStringOne( drawingSurface, paperOrientedTransform, rTopTitleRect, rBottomTitleRect, rTitleRect );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	//	Draw title string.
	::DrawTitleString( drawingSurface, paperOrientedTransform, pageArea, rTitleRect );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	//	Draw the driver name string.
	::DrawDriverNameString( drawingSurface, paperOrientedTransform, printer, pageArea, rTitleRect );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	//	Draw the paper name string.
	::DrawPaperNameString( drawingSurface, paperOrientedTransform, printer, pageArea, rTitleRect );
	if ( pPrinterStatus )
	{
		pPrinterStatus->PrintElement( );
		pPrinterStatus->PrintPage( );
	}
}


// ****************************************************************************
//
//  Function Name:	RTestPagePanel::PrintConfirmationPage( )
//
//  Description:		Renders the test page into the give output target
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::PrintConfirmationPage( RDrawingSurface& drawingSurface,
									  const R2dTransform& /*transform*/,
									  const RPrinter& printer,
									  const RIntRect& /* rcRender */,
									  const RIntRect& printableArea, 
									  RPrinterStatus* pPrinterStatus /*=NULL*/
									  ) const
	{
	RIntRect pageArea = printer.GetPaperRect( );

	//	We must use our own transform since we short circuit many project printing 
	//	conventions.
	R2dTransform paperOrientedTransform;
	printer.GetBaseTransform( paperOrientedTransform, &drawingSurface );
	
	//	Compensate for the offset from the distance from the physical page side to 
	//	the begininng of the printable area.
	paperOrientedTransform.PreTranslate( -printableArea.m_Left, -printableArea.m_Top );	

	//	Use the mark increment as the line width. Our calibration can be off
	//	by as much as the mark increment without our being able to record this fact.
	//	Make the indicators as wide as the mark increment so the user will still see
	//	the sides and fold within the center indicator even if the measurements
	// are off as much as reasonable.
	RIntSize	yLineWidth( ::InchesToLogicalUnits( kMarkIncrement ), ::InchesToLogicalUnits( kMarkIncrement ) );
	::LogicalUnitsToDeviceUnits( yLineWidth, drawingSurface );

	//	prep drawing
	drawingSurface.RestoreDefaults( );
	drawingSurface.SetPenColor( RSolidColor( kBlack ) );
	drawingSurface.SetPenWidth( yLineWidth.m_dx );
	drawingSurface.SetPenStyle( kSolidPen );
	drawingSurface.SetClipRect( pageArea );

	//	Draw the side and center markers.
	::FrameSides( drawingSurface, paperOrientedTransform, printableArea );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	//	Draw the drawing test strings.
	
	//	Get the string locations.
	//	Start with a guess of the output rect then use them to 
	// calculate the actual values.
	//	Center the confirmation title in the top of the confirmation page.
	// Make it 13 lines high.
	YIntDimension	yMarginWidth = ::InchesToLogicalUnits( kCenterMarkWidth );
	YIntDimension yLineHeight = ::GetFontHeight( drawingSurface );
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );
	RIntRect rHalfAvailableTitleRect( pageArea.m_Left, pageArea.m_Top + yMarginWidth
		, pageArea.m_Right, rPageCenter.m_y );
	RIntSize rHalfAvailableSize( rHalfAvailableTitleRect.Width( ), yLineHeight * 15 );
	RIntRect rTopTitleRect( RIntSize( rHalfAvailableSize.m_dx / 2, rHalfAvailableSize.m_dy ) );
	rTopTitleRect.CenterRectInRect( rHalfAvailableTitleRect );
//	::GetActualTextOutRect( drawingSurface, paperOrientedTransform, rTopTitleRect
//		, STRING_PRINT_TEST_CONFIRMATION_TEXT1 );

	//	Center the confirmation title in the bottom of the confirmation page.
	// Make it 3 lines high.
	rHalfAvailableSize = RIntSize( rHalfAvailableTitleRect.Width( ), yLineHeight * 5 );
	RIntRect rBottomTitleRect = RIntRect( RIntSize( rHalfAvailableSize.m_dx / 2, rHalfAvailableSize.m_dy ) );
	rHalfAvailableTitleRect.m_Top = rPageCenter.m_y;
	rHalfAvailableTitleRect.m_Bottom = pageArea.m_Bottom - yMarginWidth;
	rBottomTitleRect.CenterRectInRect( rHalfAvailableTitleRect );
//	::GetActualTextOutRect( drawingSurface, paperOrientedTransform, rBottomTitleRect
//		, STRING_PRINT_TEST_CONFIRMATION_TEXT2 );

	//	Draw the top calibration text.
	::DrawTopCalibrationString( drawingSurface, paperOrientedTransform, rTopTitleRect );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	//	Draw the bottom calibration text.
	::DrawBottomCalibrationString( drawingSurface, paperOrientedTransform, rBottomTitleRect );
	if ( pPrinterStatus )
		pPrinterStatus->PrintElement( );

	// Indicate the page center.
	// Leave more space around the calibration text.
	rTopTitleRect.Inset( RIntSize( 0, -yLineHeight ) );
	rBottomTitleRect.Inset( RIntSize( 0, -yLineHeight ) );
	::DrawCalibrationCenter( drawingSurface, paperOrientedTransform
			, pageArea, rTopTitleRect, rBottomTitleRect );
	if ( pPrinterStatus )
	{
		pPrinterStatus->PrintElement( );
		pPrinterStatus->PrintPage( );
	}
}

// ****************************************************************************
//
//  Function Name:	GetFontHeight
//
//  Description:		Return the height of a line of text in the drawing surface's context.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension	GetFontHeight( RDrawingSurface& drawingSurface )
{
	//	get the font size and output string.
	RIntSize rFontSize( ::InchesToLogicalUnits( kCalibrationFontWidth ), ::InchesToLogicalUnits( kCalibrationFontHeight ) );

	//	draw the top title
	RIntSize rLastCharSize(0, 0);
	SIZE deviceFontSize;
	BOOLEAN fGotSize = static_cast<BOOLEAN>( ::GetTextExtentPoint32( (HDC)drawingSurface.GetSurface( ), "M"
			, 1, &deviceFontSize ) );
	if ( fGotSize )
		{
		rFontSize = RIntSize ( deviceFontSize.cx, deviceFontSize.cy );
		::DeviceUnitsToLogicalUnits( rFontSize, drawingSurface );
		}
	
	return rFontSize.m_dy;
}

// ****************************************************************************
//
//  Function Name:	DrawTestStringOne
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawTestStringOne( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RIntRect& rTopTitleRect, const RIntRect& rBottomTitleRect, RIntRect& rTitleRect )
{
	//	get the font size and output string.
	RMBCString strTestText = RMBCString( ::GetResourceManager( ).GetResourceString(STRING_PRINT_TEST_PRINT_STRING_1) );
	RIntSize rFontSize( ::InchesToLogicalUnits( kCalibrationFontWidth ), ::InchesToLogicalUnits( kCalibrationFontHeight ) );

	//	draw the top title
	RIntSize rLastCharSize(0, 0);
	SIZE deviceFontSize;
	BOOLEAN fGotSize = static_cast<BOOLEAN>( ::GetTextExtentPoint32( (HDC)drawingSurface.GetSurface( ), strTestText
			, strTestText.GetStringLength( ), &deviceFontSize ) );
	if ( fGotSize )
		{
		rFontSize = RIntSize ( deviceFontSize.cx, deviceFontSize.cy );
		::DeviceUnitsToLogicalUnits( rFontSize, drawingSurface );
		}
	else
		rFontSize = rTopTitleRect.WidthHeight( );

	RMBCString rLastCharString = strTestText.Tail(1);
	fGotSize = static_cast<BOOLEAN>( ::GetTextExtentPoint32( (HDC)drawingSurface.GetSurface( ), rLastCharString
		, rLastCharString.GetStringLength( ), &deviceFontSize ) );
	if ( fGotSize )
		{
		rLastCharSize = RIntSize ( deviceFontSize.cx, deviceFontSize.cy );
		::DeviceUnitsToLogicalUnits( rLastCharSize, drawingSurface );
		}
	else
		rLastCharSize = rBottomTitleRect.WidthHeight( );

	rTitleRect = RIntRect( rFontSize );
	RIntRect	tempTopTitleRect( rTopTitleRect );
	rTitleRect.CenterRectInRect( tempTopTitleRect );
	rTitleRect.Offset( RIntSize(0, 2*rTopTitleRect.Height( )) );

	//	Draw test string 1
	::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTitleRect.m_TopLeft, rTitleRect.WidthHeight( ), strTestText, strTestText.GetStringLength( ), !kFrame, kMarkerDrawTextMode );

	//	Test the transparency charecteristics of the printer driver by drawing white over the last character of the string.
	RIntRect rLastCharRect( rTitleRect );
	rLastCharRect.m_Left = rLastCharRect.m_Right - rLastCharSize.m_dx;
	drawingSurface.SetFillColor( RSolidColor( kWhite ) );

	//	REVIEW ESV 12/6/96 - Draw text is offsetting the text by its height. Why?
	rLastCharRect.Offset( RIntSize( 0, -rLastCharRect.Height( ) ) );
	
	//	Try to compensate for mysterious positioning differences between drivers by exanding
	//	white fill rect.
	rLastCharRect.Inset( RIntSize(-2, -2) );
	drawingSurface.FillRectangle( rLastCharRect, paperOrientedTransform );
}

// ****************************************************************************
//
//  Function Name:	FrameSides( ... ) const
//
//  Description:		Indicate the printable area by framing it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	FrameSides( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RPrinter& printer )
{
	//	Indicate the printable area by framing it.
	RIntRect printableArea = printer.GetPrintableArea( );
	drawingSurface.FrameRectangle( printableArea, paperOrientedTransform );
}

// ****************************************************************************
//
//  Function Name:	FrameSides( ... ) const
//
//  Description:		Indicate the printable area by framing it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	FrameSides( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RIntRect& printableArea )
{
	//	Indicate the printable area by framing it.
	drawingSurface.FrameRectangle( printableArea, paperOrientedTransform );
}

// ****************************************************************************
//
//  Function Name:	DrawSideMarkers( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntPoint& rTopLeft, const RIntSize& rMarkSize, const char* pMarkerText, const YCounter yOutStringLength, const BOOLEAN fFrame, const int nDrawTextMode ) const
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawSideMarkers( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
		   , const RPrinter& printer )
{

	//	Line the page sides with boxed calibration marks. 
	//	draw calibration rects
	YIntDimension	yIncrement = ::InchesToLogicalUnits( kMarkIncrement );
	YIntDimension	yCornerOffset = ::InchesToLogicalUnits( kSideMarkOffset );
	RIntPoint		rTopLeft;
	char				pMarkerText[3];
	RIntRect printableArea = printer.GetPrintableArea( );
	RIntRect&		rCalibrationBounds = printableArea;

	for( int i = 0; i < kNumSideMarkers; ++i )
		{
		WinCode( ::wsprintf( pMarkerText, "%2d", i+1 ) );

		//	Draw side calibration marks that will allow us to determine the size of the printable area

		//	Draw top markers down and left from the top right corner. Center the 16th marker at the edge of the printable area.
		rTopLeft = RIntPoint( rCalibrationBounds.m_Right - yMarkWidth - i*yMarkWidth, rCalibrationBounds.m_Top + i*yIncrement - 16 * yIncrement );
		::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopLeft, kMarkSize, pMarkerText, 2, kFrame, kMarkerDrawTextMode );

		//	Draw left markers right and down from the top left corner. Center the 16th marker at the edge of the printable area.
		rTopLeft = RIntPoint( rCalibrationBounds.m_Left + i*yIncrement - 16 * yIncrement, rCalibrationBounds.m_Top + yCornerOffset + i*yMarkWidth );
		::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopLeft, kMarkSize, pMarkerText, 2, kFrame, kMarkerDrawTextMode );

		//	Draw bottom markers up and right from the bottom left corner. Center the 16th marker at the edge of the printable area.
		rTopLeft = RIntPoint( rCalibrationBounds.m_Left + i*yMarkWidth, rCalibrationBounds.m_Bottom - yMarkWidth - i*yIncrement + 16 * yIncrement );
		::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopLeft, kMarkSize, pMarkerText, 2, kFrame, kMarkerDrawTextMode );

		//	Draw right markers up and left from the lower right hand corner. Center the 16th marker at the edge of the printable area.
		rTopLeft = RIntPoint( rCalibrationBounds.m_Right - yMarkWidth - i*yIncrement + 16 * yIncrement, rCalibrationBounds.m_Bottom - yMarkWidth - yCornerOffset - i*yMarkWidth );
		::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopLeft, kMarkSize, pMarkerText, 2, kFrame, kMarkerDrawTextMode );
		}
}

// ****************************************************************************
//
//  Function Name:	DrawTitleString( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntPoint& rTopLeft, const RIntSize& rMarkSize, const char* pMarkerText, const YCounter yOutStringLength, const BOOLEAN fFrame, const int nDrawTextMode ) const
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawTitleString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& pageArea, const RIntRect& rTitleRect )
{
	RMBCString strTestText = RMBCString( ::GetResourceManager( ).GetResourceString(STRING_PRINT_TEST_TITLE) );
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );
	YIntDimension	yIncrement = ::InchesToLogicalUnits( kMarkIncrement );

	//	Position the title string one marker height below the lowest center marker box (labeled PP).
	//	Make the output box as high as the output area for test string 2 (rTitleRect.Height( ).
	//	Let the os centering logic position the string between the left and right boundary of the page area.
	RIntRect rBottomTitleRect = RIntRect( pageArea.m_Left
		, rPageCenter.m_y + (kNumSideMarkers/2 + 3) * yIncrement
		, pageArea.m_Right
		, rPageCenter.m_y + (kNumCenterMarkers/2 + 3) * yMarkWidth + rTitleRect.Height( ) );

	//	Draw title string.
	::DrawCalibrationText( drawingSurface, paperOrientedTransform, rBottomTitleRect.m_TopLeft, rBottomTitleRect.WidthHeight( ), strTestText, strTestText.GetStringLength( ), !kFrame, kMarkerDrawTextMode );
}

// ****************************************************************************
//
//  Function Name:	GetActualTextOutRect( ... ) 
//
//  Description:		call DrawText to get the rect used by the given text
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	GetActualTextOutRect( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, RIntRect& rTitleRect, int nTextResourceID )
{
	RMBCString strTestText = RMBCString( ::GetResourceManager( )
		.GetResourceString(nTextResourceID) );

	//	Convert the given output rect to device units.
	CDC* pdc = CDC::FromHandle( (HDC)drawingSurface.GetSurface(  ) );
	rTitleRect *= paperOrientedTransform;

	//	Get the output rect.
	RECT rNative;
	rNative.left = rTitleRect.m_Left;
	rNative.top = rTitleRect.m_Top;
	rNative.right = rTitleRect.m_Right;
	rNative.bottom = rTitleRect.m_Bottom;
	pdc->DrawText( strTestText, strTestText.GetStringLength( ), &rNative
		, DT_CALCRECT | DT_CENTER | DT_NOCLIP | DT_NOPREFIX | DT_WORDBREAK );

	//	Convert the output rect back to logical units.
	rTitleRect = RIntRect( rNative );
	::DeviceUnitsToLogicalUnits( rTitleRect, drawingSurface );
}

// ****************************************************************************
//
//  Function Name:	DrawTopCalibrationString( ... ) const
//
//  Description:		call DrawText to render the given text to the given 
//								drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawTopCalibrationString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& rTopTitleRect )
{
	RMBCString strTestText = RMBCString( ::GetResourceManager( )
		.GetResourceString(STRING_PRINT_TEST_CONFIRMATION_TEXT1) );

	//	Draw title string.
	::DrawCalibrationText( drawingSurface, paperOrientedTransform
		, rTopTitleRect.m_TopLeft, rTopTitleRect.WidthHeight( )
		, strTestText, strTestText.GetStringLength( ), !kFrame
		, DT_CENTER | DT_NOCLIP | DT_NOPREFIX | DT_WORDBREAK );
}

// ****************************************************************************
//
//  Function Name:	DrawBottomCalibrationString( ... ) const
//
//  Description:		call DrawText to render the given text to the given 
//								drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawBottomCalibrationString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& rBottomTitleRect )
{
	RMBCString strTestText = RMBCString( ::GetResourceManager( )
		.GetResourceString(STRING_PRINT_TEST_CONFIRMATION_TEXT2) );

	//	Draw title string.
	::DrawCalibrationText( drawingSurface, paperOrientedTransform
		, rBottomTitleRect.m_TopLeft, rBottomTitleRect.WidthHeight( )
		, strTestText, strTestText.GetStringLength( ), !kFrame
		, DT_CENTER | DT_NOCLIP | DT_NOPREFIX | DT_WORDBREAK );
}

// ****************************************************************************
//
//  Function Name:	DrawTitleString( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntPoint& rTopLeft, const RIntSize& rMarkSize, const char* pMarkerText, const YCounter yOutStringLength, const BOOLEAN fFrame, const int nDrawTextMode ) const
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawDriverNameString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RPrinter& printer, const RIntRect& pageArea, const RIntRect& rTitleRect )
{
	RMBCString strTestText( printer.GetDriverName( ) );
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );

	//	Position the title string one marker height below the lowest center marker box (labeled PP).
	//	Make the output box as high as the output area for test string 2 (rTitleRect.Height( ).
	//	Let the gdi centering logic position the string between the left and right boundary of the page area.
	RIntRect rBottomTitleRect = RIntRect( pageArea.m_Left
		, rPageCenter.m_y + (kNumCenterMarkers/2 + 3) * yMarkWidth
		, pageArea.m_Right
		, rPageCenter.m_y + (kNumCenterMarkers/2 + 3) * yMarkWidth + rTitleRect.Height( ) );

	//	Offset the driver name string from the title rect by its height.
	rBottomTitleRect.Offset( RIntSize( 0, rTitleRect.Height( ) ) );

	//	Draw title string.
	::DrawCalibrationText( drawingSurface, paperOrientedTransform, rBottomTitleRect.m_TopLeft, rBottomTitleRect.WidthHeight( ), strTestText, strTestText.GetStringLength( ), !kFrame, kMarkerDrawTextMode );
}

// ****************************************************************************
//
//  Function Name:	DrawCalibrationCenter( ... ) const
//
//  Description:		Indicate the center of the printable area.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawCalibrationCenter( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& pageArea, RIntRect rTopTitleRect
			, RIntRect rBottomTitleRect )
{
	YIntDimension	yMarginWidth = ::InchesToLogicalUnits( kCenterMarkWidth );
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );

	//	Draw the center offset markers to determine the location of the printable area.
	//	Draw cross hairs at the center of the page without obscuring top or bottom text:
	//    |
	// Top Text
	//    |
	//---------
	//    |
	// Bottom Text
	//    |

	//	Adjust the top and bottom rects so they are proper end points for the calibration
	// lines by making sure they don't start outside the margins.
	if ( pageArea.m_Top + yMarginWidth > rTopTitleRect.m_Top )
		rTopTitleRect.m_Top = pageArea.m_Top + yMarginWidth;
	if ( pageArea.m_Bottom - yMarginWidth < rBottomTitleRect.m_Bottom )
		rBottomTitleRect.m_Bottom = pageArea.m_Top - yMarginWidth;

	// Draw the vertical marker.
	drawingSurface.MoveTo( RIntPoint( rPageCenter.m_x, pageArea.m_Top + yMarginWidth )	, paperOrientedTransform );
	drawingSurface.LineTo( RIntPoint( rPageCenter.m_x, rTopTitleRect.m_Top )				, paperOrientedTransform );
	drawingSurface.MoveTo( RIntPoint( rPageCenter.m_x, rTopTitleRect.m_Bottom )			, paperOrientedTransform );
	drawingSurface.LineTo( RIntPoint( rPageCenter.m_x, rBottomTitleRect.m_Top )			, paperOrientedTransform );
	drawingSurface.MoveTo( RIntPoint( rPageCenter.m_x, rBottomTitleRect.m_Bottom )		, paperOrientedTransform );
	drawingSurface.LineTo( RIntPoint( rPageCenter.m_x, pageArea.m_Bottom - yMarginWidth ), paperOrientedTransform );
	
	// Draw the horizontal marker.
	drawingSurface.MoveTo( RIntPoint( pageArea.m_Left + yMarginWidth, rPageCenter.m_y ), paperOrientedTransform );
	drawingSurface.LineTo( RIntPoint( pageArea.m_Right - yMarginWidth, rPageCenter.m_y ), paperOrientedTransform );
}

// ****************************************************************************
//
//  Function Name:	DrawCenterMarkers( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntPoint& rTopLeft, const RIntSize& rMarkSize, const char* pMarkerText, const YCounter yOutStringLength, const BOOLEAN fFrame, const int nDrawTextMode ) const
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawCenterMarkers( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RIntRect& pageArea )
{
	char				pMarkerText[3];
	RIntPoint		rTopLeft;
	YIntDimension	yIncrement = ::InchesToLogicalUnits( kMarkIncrement );

	//	Draw the center offset markers to determine the location of the printable area
	//	Draw cross hairs at the center of the page
	YIntDimension	yCrossHairWidth = ::InchesToLogicalUnits( kCenterMarkWidth );
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );
	drawingSurface.MoveTo( RIntPoint( rPageCenter.m_x, rPageCenter.m_y - yCrossHairWidth/2 ), paperOrientedTransform );
	drawingSurface.LineTo( RIntPoint( rPageCenter.m_x, rPageCenter.m_y + yCrossHairWidth/2 ), paperOrientedTransform );
	drawingSurface.MoveTo( RIntPoint( rPageCenter.m_x - yCrossHairWidth/2, rPageCenter.m_y ), paperOrientedTransform );
	drawingSurface.LineTo( RIntPoint( rPageCenter.m_x + yCrossHairWidth/2, rPageCenter.m_y ), paperOrientedTransform );

	//	Draw the center markers
	//	Define increment start values
	char cMarkerStart = 'A';
	YIntDimension yMarkerXStartOffset = -((kNumCenterMarkers/2 - 1) * yMarkWidth + yCrossHairWidth/2 + yIncrement);
	YIntDimension yMarkerYStartOffset = -((kNumCenterMarkers/2 - 1) * yIncrement);
	
	//	Split the markers across the center by drawing half of them at a time
	for( int j = 0; j < 2; ++j )
		{
		//	Draw the horizontal markers
		for( int i = 0; i < kNumCenterMarkers/2; ++i )
			{
			WinCode( ::wsprintf( pMarkerText, "%2c", cMarkerStart + i ) );

			//	Draw horizontal markers
			rTopLeft = RIntPoint( rPageCenter.m_x + yMarkerXStartOffset + i*yMarkWidth, rPageCenter.m_y - yMarkWidth + yMarkerYStartOffset + i*yIncrement );
			::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopLeft, kMarkSize, pMarkerText, 2, kFrame, kMarkerDrawTextMode );
	
			//	Draw vertical markers
			//	Get text markers whose values do not overlap with the horizontal markers
			pMarkerText[0] = pMarkerText[1];

			rTopLeft = RIntPoint( rPageCenter.m_x - yMarkWidth + yMarkerYStartOffset + i*yIncrement, rPageCenter.m_y + yMarkerXStartOffset + i*yMarkWidth );
			::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopLeft, kMarkSize, pMarkerText, 2, kFrame, kMarkerDrawTextMode );
			}
		
		//	go on to next set of marker labels
		cMarkerStart += kNumCenterMarkers/2;							
		
		//	switch to other side of center cross hairs
		yMarkerXStartOffset += (yCrossHairWidth + (kNumCenterMarkers/2 - 2) * yMarkWidth + 2*yIncrement);
		yMarkerYStartOffset = -yMarkerYStartOffset + yIncrement;	
		}
}

// ****************************************************************************
//
//  Function Name:	DrawCalibrationText( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntPoint& rTopLeft, const RIntSize& rMarkSize, const char* pMarkerText, const YCounter yOutStringLength, const BOOLEAN fFrame, const int nDrawTextMode ) const
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawCalibrationText( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntPoint& rTopLeft
			, const RIntSize& rMarkSize, const char* pMarkerText, const YCounter yOutStringLength, const BOOLEAN fFrame
			, const int nDrawTextMode )
	{
	RIntRect			rMarker;
	RIntRect			rMarkerText;
	WinCode(			CDC* pdc = CDC::FromHandle( (HDC)drawingSurface.GetSurface(  ) ) );
	WinCode(			RECT rNative );

	rMarker = RIntRect( rTopLeft, rMarkSize );
	rMarker *= transform;
	
	//	Offset the text so that it fits within the given rect even for built in fonts which windows
	//	seems to position incorrectly when we're framing text.
	rMarkerText = rMarker;
	if ( fFrame )
		rMarkerText.Offset( RIntSize(0,rMarkerText.Height( )/4) );
	
	//	draw the marker text
	WinCode( (rNative.left = rMarkerText.m_Left, rNative.top = rMarkerText.m_Top, rNative.right = rMarkerText.m_Right, rNative.bottom = rMarkerText.m_Bottom) );
	WinCode( pdc->DrawText( pMarkerText, yOutStringLength, &rNative, nDrawTextMode ) );

	if ( fFrame )
		drawingSurface.FrameRectangle( rMarker );
	}

// ****************************************************************************
//
//  Function Name:	DrawPaperNameString
//
//  Description:		call DrawText to render the given text to the given drawingSurface in the given location
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	DrawPaperNameString( RDrawingSurface& drawingSurface, const R2dTransform& paperOrientedTransform
			, const RPrinter& printer, const RIntRect& pageArea, const RIntRect& rTitleRect )
{
	RMBCString strTestText( printer.GetPaperNameAndOrientation( ) );
	RIntPoint rPageCenter = pageArea.GetCenterPoint( );
	YIntDimension	yIncrement = ::InchesToLogicalUnits( kMarkIncrement );

	//	Position the title string between the highest center marker box and the lowest top marker box.
	//	Make the output box as high as the output area for test string 2 (rTitleRect.Height( ).
	//	Let the gdi centering logic position the string between the left and right boundary of the page area.
	RIntRect rTopTitleRectAvailableArea = RIntRect( pageArea.m_Left
		, pageArea.m_Top + (kNumCenterMarkers + 1) * yIncrement
		, pageArea.m_Right
		, rPageCenter.m_y - (kNumCenterMarkers/2 + 1) * yMarkWidth );
	
	//	The text prints at the bottom of the available area so center a title rect
	//	which has the text height in the available area.
	RIntRect rTopTitleRect( rTopTitleRectAvailableArea );
	rTopTitleRect.m_Bottom = rTopTitleRect.m_Top + rTitleRect.Height( );
	rTopTitleRect.CenterRectInRect( rTopTitleRectAvailableArea );

	//	Draw test string 1
	::DrawCalibrationText( drawingSurface, paperOrientedTransform, rTopTitleRect.m_TopLeft, rTopTitleRect.WidthHeight( ), strTestText
		, strTestText.GetStringLength( ), !kFrame, kMarkerDrawTextMode );
}