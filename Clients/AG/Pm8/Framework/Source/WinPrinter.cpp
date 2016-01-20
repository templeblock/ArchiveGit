// ****************************************************************************
//
//  File Name:			WinPrinter.cpp
//
//  Project:			Renaissance Application
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPrinter class
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
//  $Logfile:: /PM8/Framework/Source/WinPrinter.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Printer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"PrintManager.h"
#include "FrameworkResourceIDs.h"
#include	"PrinterDrawingSurface.h"
#include "Document.h"
#include "ApplicationGlobals.h"

//	The largest amount by which the printer's point size can
//	differ from the expected paper size when examining the 
//	printer's paper sizes.
const YFloatType kMaximumPointSizeDifference = 1.;

// ****************************************************************************
//
//  Function Name:	RPrinter::Update( )
//
//  Description:		Get current info for this printer from the driver.
//
//  Returns:			TRUE: the update was successful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::Update( BOOLEAN fUpdatePaperAreas /*= TRUE*/, BOOLEAN fUsePrinterDatabase /*= TRUE*/ )
	{
	BOOLEAN fSuccess = TRUE;

	//	Try to update the printer even if we previously failed.
	HANDLE hPrinter;
	PRINTER_INFO_2*	pPrinterInfo = NULL;

	TpsAssert( !m_rPrinterName.IsEmpty(), "Updating a printer with no name." );

	//	Get current info about this printer from the driver
	//	Clear parameter block.
	hPrinter = (HANDLE)OpenPrinter( );

	fSuccess = static_cast<BOOLEAN>(NULL != hPrinter);
	
	if ( fSuccess )
		{
		DWORD dwRequestedBuffSize = 0;
		DWORD dwUsedBuffSize = 0;

		//	Get the size of the buffer. Ignore the return value since GetPrinter() LIES.
		::GetPrinter( hPrinter, 2, NULL, 0, &dwRequestedBuffSize );
		
		try
			{
			pPrinterInfo = (PRINTER_INFO_2*)new uBYTE[ dwRequestedBuffSize ];
			::memset( pPrinterInfo, 0, dwRequestedBuffSize );
			}
		catch( ... )
			{
			m_fSpecificationComplete = FALSE;
			fSuccess = FALSE;
			}

		if ( fSuccess )
			{
			fSuccess = static_cast<BOOLEAN>( ::GetPrinter( hPrinter, 2, (PUBYTE)pPrinterInfo, dwRequestedBuffSize, &dwUsedBuffSize ) );
			TpsAssert( dwRequestedBuffSize == dwUsedBuffSize, "Incorrect buffer size passed to GetPrinter." );
			}
		
		if ( fSuccess )
			fSuccess = static_cast<BOOLEAN>( dwRequestedBuffSize == dwUsedBuffSize );

		}

	if ( fSuccess )
		{
		//	Update these values with more accurate versions obtained when ::GetPrinter
		//	actually talks to the driver instead of reading from the registry.
		m_rPrinterName = pPrinterInfo->pPrinterName;
		m_rDriverName = pPrinterInfo->pDriverName;
		m_rPortName = pPrinterInfo->pPortName;

		//	Populate the paper areas collection. Dont actually get the info yet. Mark the cache as dirty and
		// retreive as necessary.
		if( fUpdatePaperAreas )
			InvalidatePaperInfo( );

		//	Get properties (paper size and orientation) that match the ones used to create the above DC.
		//	Use the local user specified properties in m_pDriverInfo if they exist otherwise
		//	use the properties from the driver returned in pPrinterInfo->pDevMode.
		//	If no property info is available
		//	(as can happen if an incompletely installed driver is selected) then make this 
		//	a template printer below by setting fSuccess FALSE here.
		LPDEVMODE pDevMode = (LPDEVMODE)m_pDriverInfo;
		if ( !pDevMode )
			{
			GetProperties( (uLONG)hPrinter, kPrinterPropertiesNoFlags );
			pDevMode = (LPDEVMODE)m_pDriverInfo;
			}

		fSuccess = static_cast<BOOLEAN>( NULL != pDevMode );

		// Get the paper type
		if ( fSuccess )
			{
			SetPaperSize( pDevMode->dmPaperSize );

			// Get the orientation
			if( pDevMode->dmOrientation == DMORIENT_PORTRAIT )
				m_Orientation = kPortrait;
			else
				m_Orientation = kLandscape;
			}

		//	Remember printable area that the driver returns for our orientation 
		//	before we update it from the database. We will use this value in
		//	comparisons.
		if( fUpdatePaperAreas )
			m_PrintableAreaFromDriver = GetPrintableArea( kUSLetter, kPortrait );
		}
	
	//	Clean up.
	if( hPrinter )
		ClosePrinter( (uLONG)hPrinter );

	delete [] pPrinterInfo;

	if ( fSuccess )
		{
		//	If the printable area is in the printer database then use the database value.
		if ( RPrintManager::m_fPrintManagerInitialized && (fUsePrinterDatabase != FALSE) )
			RPrintManager::m_PrintManager.UpdatePrinterFromDatabase( this );
		}
	else
		{
		//	The printer was deleted or is otherwise unavailable. Mark it and return failure.
		if ( !m_fTemplatePrinter )
			Delete( );

		//	Make sure the printer has valid information. Some printers may become invalid
		//	while Renaissance is running. Keep the old previously valid values if this has
		// happened.
		if ( !m_fSpecificationComplete )
			SpecifyTemplateValues( );
		}

	//	Remember whether our settings have been initialized yet.
	m_fSpecificationComplete = fSuccess;

	// REVIEW ESV 5/8/98 - This is a better way to handle a printer that does not support
	// its paper. I cannot install this method now because we are too near the end of a 
	// development cycle. kPrinterPaperNotSupported needs to be added to ETemplateReason. 
	// Please install this code at a better time.

	// Update the paper info.
//	if ( fUpdatePaperAreas )	
//		UpdatePaperInfo( m_PaperSize, m_Orientation );

	// If our paper size is not supported then we are using template values for m_PaperSize.
//	if ( !IsPaperSupported( m_PaperSize ) )
//		MakeTemplate( kPrinterPaperNotSupported );

	return fSuccess;
}

// ****************************************************************************
//
//  Function Name:	PreferPaper( const EPaperSize paper, const EOrientation orientation, EPaperSize& oldPaper, EOrientation& oldOrientation )
//
//  Description:		Set the given paper and orientation so that calculations will take place using them.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::PreferPaper( const EPaperSize paper, const EOrientation orientation, EPaperSize& oldPaper, EOrientation& oldOrientation )
	{
	//	Return the current printer charecteristics so we may restore them later.
	oldPaper = m_PaperSize;
	oldOrientation = m_Orientation;

	//	Set the printers device properties to the requested page and orientation.
	m_PaperSize = paper;
	m_Orientation = orientation;

	if ( !m_fTemplatePrinter )
		{
		if( !m_pDriverInfo )
			GetProperties( kPrinterPropertiesForceUpdate );

		LPDEVMODE pDevMode = (LPDEVMODE)m_pDriverInfo;

		if ( pDevMode )
			{
			pDevMode->dmFields |= DM_ORIENTATION | DM_PAPERSIZE; 
			pDevMode->dmOrientation = (short)((kPortrait == orientation)?DMORIENT_PORTRAIT:DMORIENT_LANDSCAPE); 
			pDevMode->dmPaperSize = (m_PaperAreas.Start())[paper].m_swPlatformPaperId;//  RPrinter::GetPlatformPaperId( paper ); 
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	GetPaperDimensions
//
//  Description:		Return the dimensions of the given paper size.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize GetPaperDimensions( RPrinter::EPaperSize paper, RPrinter::EOrientation eOrientation )
	{
	RRealSize rPaperSize( 0., 0. );

	//	Get the size of the given paper in portrait orientation and 10 x millimeters
	//	so we can compare to the given POINT dimensions.
	switch( paper )
		{
		case RPrinter::kUSLetter:
			rPaperSize = RRealSize( ::InchesToLogicalUnits( 8.50 ), ::InchesToLogicalUnits( 11. ) );
			break;

		case RPrinter::kUSLegal:
			rPaperSize = RRealSize( ::InchesToLogicalUnits( 8.50 ), ::InchesToLogicalUnits( 14. ) );
			break;

		case RPrinter::kTabloid:
			rPaperSize = RRealSize( ::InchesToLogicalUnits( 11. ), ::InchesToLogicalUnits( 17. ) );
			break;

		case RPrinter::kA4Letter:
			rPaperSize = RRealSize( ::InchesToLogicalUnits( ::MillimetersToInches( 210. ) ), ::InchesToLogicalUnits( ::MillimetersToInches( 297. ) ) );
			break;

		case RPrinter::kB5Letter:
			rPaperSize = RRealSize( ::InchesToLogicalUnits( ::MillimetersToInches( 182. ) ), ::InchesToLogicalUnits( ::MillimetersToInches( 257. ) ) );
			break;

		case RPrinter::kA3Tabloid:
			rPaperSize = RRealSize( ::InchesToLogicalUnits( ::MillimetersToInches( 297. ) ), ::InchesToLogicalUnits( ::MillimetersToInches( 420. ) ) );
			break;

		default:
			TpsAssertAlways( "Invalid paper size" );
		};
	
	//	We specified the above dimensions in portrait orientation.
	//	Swap to landscape if that is the orientation of the given dimensions.
	if ( RPrinter::kPortrait != eOrientation )
		Swap( rPaperSize );

	return rPaperSize;
	}

// ****************************************************************************
//
//  Function Name:	AreFloatsSimiliar
//
//  Description:		Returns TRUE iff the given floats differ by the given 
//							yMaximumDistance.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN AreFloatsSimiliar( YFloatType yFloat1, YFloatType yFloat2, YFloatType yMaximumDistance )
	{
	return ::Abs( yFloat1 - yFloat2 ) < yMaximumDistance;
	}

// ****************************************************************************
//
//  Function Name:	Swap
//
//  Description:		Swap the given points values.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
static void Swap( POINT& givenPoint )
{
	int nTemp = givenPoint.x;
	givenPoint.x = givenPoint.y;
	givenPoint.y = nTemp;
}

// ****************************************************************************
//
//  Function Name:	PaperAreaMatchesDimensions
//
//  Description:		Determines if this printer supports the specified paper
//							size.
//
//  Returns:			TRUE if the paper size is supported
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN PaperAreaMatchesDimensions( POINT dimensions, RPrinter::EPaperSize paper, RPrinter::EOrientation eOrientation )
	{
	//	Get the size of the given paper size.
	RRealSize rPaperSize( GetPaperDimensions( paper, eOrientation ) );
	
	//	Get the size of the paper (given in 10 x millimeters) in logical units
	//	from the given POINT dimensions.
	RRealSize rPaperSizeToMatch = RRealSize( ::InchesToLogicalUnits( ::MillimetersToInches( dimensions.x ) ) / 10.
		, ::InchesToLogicalUnits( ::MillimetersToInches( dimensions.y ) ) / 10. );

	return ::AreFloatsSimiliar( rPaperSizeToMatch.m_dx, rPaperSize.m_dx, kMaximumPointSizeDifference ) 
		&& ::AreFloatsSimiliar( rPaperSizeToMatch.m_dy, rPaperSize.m_dy, kMaximumPointSizeDifference );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::IsPaperSupported
//
//  Description:		Determines if this printer supports the specified paper
//							size.
//
//  Returns:			TRUE if the paper size is supported
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::IsPaperSupported( EPaperSize paper ) const
	{
	// If it is a template printer, let the user do whatever they want
	if( m_fTemplatePrinter )
		return TRUE;

	if ( m_PaperAreas.Count() > 1 )
	{
		//	First, check the array of paper sizes that this printer supports.
		//		if the values are marked as valid, we must support this paper.
		RPaperAreaCollection::YIterator Iter = m_PaperAreas.Start( );
		if ( ((m_Orientation == kPortrait) && Iter[paper].m_fPortraitSizeValid) ||
			 ((m_Orientation == kLandscape) && Iter[paper].m_fLandscapeSizeValid) )
		 return TRUE;
	}

	WORD* pPapers = NULL;
	POINT* pPaperSizes = NULL;
	BOOLEAN fPaperSupported = FALSE;

	try
		{
		// Get the number of papers this driver supports
		sLONG numPapers = ::DeviceCapabilities( m_rPrinterName, m_rPortName
			, DC_PAPERS, NULL, (PDEVMODE)m_pDriverInfo );

		// Check for failure
		if( numPapers == -1 )
			throw FALSE;

		// Allocate an array of WORDs and retrieve the paper sizes
		pPapers = new WORD[ numPapers ];
		memset( pPapers, 0, sizeof( WORD ) * numPapers );

		// Retrieve the supported paper sizes
		sLONG result = ::DeviceCapabilities( m_rPrinterName, m_rPortName
			, DC_PAPERS, reinterpret_cast<char*>( pPapers ), (PDEVMODE)m_pDriverInfo );

		// Check for failure
		if( result == -1 )
			throw FALSE;

		// Get the windows paper id
		WORD windowsPaper = GetPlatformPaperId( paper );

		// Look for the paper
		for( sLONG i = 0; i < numPapers; ++i )
			{
			if( pPapers[ i ] == windowsPaper )
				{
				fPaperSupported = TRUE;
				break;
				}
			}

		// The HP PhotoSmart printer does not correctly report support for DMPAPER_LETTER. Rather, it 
		// uses a custom paper with size 8.5 x 11. In addition, calling DeviceCapabilities to query 
		// for DC_PAPERSIZES fails with this printer. I see no option other than to retreive the paper
		// names, and look for the string "Letter". This is HPs bug, and if they ever fix it, this
		// hack can (and should) be removed.
			//	MDH - 1/20/98: This test is being allowed for app paper types so that
			//		the EPSON Stylus 3000 can use its ledger size for Tabloid.
		if( fPaperSupported == FALSE )//&& (paper == kUSLetter || paper == kA4Letter) )
			{
			BOOLEAN fPhotoSmartAssumed = FALSE;

			// Get the number of paper sizes this driver supports.
			sLONG numPaperSizes = ::DeviceCapabilities( m_rPrinterName, m_rPortName, DC_PAPERSIZE, NULL
				, (PDEVMODE)m_pDriverInfo );

			//	The HP Photosmart driver incorrectly returns -1 == numPaperSizes. Use numPapers instead.
			if( numPaperSizes == -1 )
			{
				numPaperSizes = numPapers;
				fPhotoSmartAssumed = TRUE;
			}

			pPaperSizes = new POINT[ numPaperSizes ];
			memset( pPaperSizes, 0, sizeof( POINT ) * numPaperSizes );

			// Retrieve the supported paper sizes in tenths of millimeters.
			result = ::DeviceCapabilities( m_rPrinterName, m_rPortName, DC_PAPERSIZE
				, reinterpret_cast<char*>( pPaperSizes ), (PDEVMODE)m_pDriverInfo );

			// Check for failure
			if( result == -1 )
				throw FALSE;

			// See if the paper size we're testing is in the driver's list of supported paper dimensions.
			for( sLONG j = 0; j < numPaperSizes; ++j )
				{
				// Believe it or don't. The photosmart printer always returns portrait paper sizes.
				//	Swap the page orientation if we are dealing with the hp photosmart printer.
				if ( fPhotoSmartAssumed && (kPortrait != m_Orientation) )
					Swap( pPaperSizes[ j ] );

				//	Use m_Orientation since we must match the orientation specified in the
				// ::DeviceCapabilities call thru m_pDriverInfo.
				if( PaperAreaMatchesDimensions( pPaperSizes[ j ], paper, m_Orientation ) )
					{
					fPaperSupported = TRUE;
					(m_PaperAreas.Start())[paper].m_swPlatformPaperId = pPapers[j];
					break;
					}
				}
			}
		}

	catch( ... )
		{
		;
		}

	// Clean up and return
	delete [] pPaperSizes;
	delete [] pPapers;
	return fPaperSupported;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPlatformPaperId( const EPaperSize eSize  ) const
//
//  Description:		Convert the given platform specific paper size into a renaissance paper size
//							and save it in this object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
sWORD RPrinter::GetPlatformPaperId( const EPaperSize eSize )
	{
	switch( eSize )
		{
		case kUSLetter: 				//	Letter, 8 1/2- by 11-inches
			return DMPAPER_LETTER;
		case kUSLegal:					//	Legal, 8 1/2- by 14-inches
			return DMPAPER_LEGAL;

		case kA4Letter:				//	A4 Sheet, 210- by 297-millimeters
			return DMPAPER_A4;
		case kB5Letter:				//	B5 sheet, 182- by 257-millimeter paper
			return DMPAPER_B5;

		case kTabloid:					//	Tabloid, 11- by 17-inches
			return DMPAPER_TABLOID;
		case kSpecialPaperSize:		//	Most of our envelope paper sizes are unsupported so use expect the user size
			return DMPAPER_USER;

		case kA3Tabloid:				//	A3 sheet, 297- by 420-millimeters
			return DMPAPER_A3;

		default:
			TpsAssertAlways( "Unknown paper size." );
			return DMPAPER_USER;
		};

	return 0;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::SetPaperSize( const sWORD swPaperID  )
//
//  Description:		Convert the given platform specific paper size into a renaissance paper size
//							and save it in this object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::SetPaperSize( const sWORD swPaperID  )
{
	BOOLEAN fKnownPaperSize = TRUE;

	switch( swPaperID )
		{
		case DMPAPER_LETTER:			//	Letter, 8 1/2- by 11-inches
			m_PaperSize = kUSLetter;
			break;
		case DMPAPER_LEGAL:			//	Legal, 8 1/2- by 14-inches
			m_PaperSize = kUSLegal;
			break;

		case DMPAPER_A4:				//	A4 Sheet, 210- by 297-millimeters
			m_PaperSize = kA4Letter;
			break;
		case DMPAPER_B5:				//	B5 sheet, 182- by 257-millimeter paper
			m_PaperSize = kB5Letter;
			break;

		case DMPAPER_TABLOID:		//	Tabloid, 11- by 17-inches
			m_PaperSize = kTabloid;
			break;
		case DMPAPER_USER:			//	Most of our envelope paper sizes are unsupported so use expect the user size
			m_PaperSize = kSpecialPaperSize;
			break;

		case DMPAPER_A3:				//	A3 sheet, 297- by 420-millimeters
			m_PaperSize = kA3Tabloid;
			break;

		case DMPAPER_CSHEET:			//	C Sheet, 17- by 22-inches
		case DMPAPER_DSHEET:			//	D Sheet, 22- by 34-inches
		case DMPAPER_ESHEET:			//	E Sheet, 34- by 44-inches
		case DMPAPER_LETTERSMALL:	//	Letter Small, 8 1/2- by 11-inches
		case DMPAPER_LEDGER:			//	Ledger, 17- by 11-inches
		case DMPAPER_STATEMENT:		//	Statement, 5 1/2- by 8 1/2-inches
		case DMPAPER_EXECUTIVE:		//	Executive, 7 1/4- by 10 1/2-inches
		case DMPAPER_A4SMALL:		//	A4 small sheet, 210- by 297-millimeters
		case DMPAPER_A5:				//	A5 sheet, 148- by 210-millimeters
		case DMPAPER_B4:				//	B4 sheet, 250- by 354-millimeters
		case DMPAPER_FOLIO:			//	Folio, 8 1/2- by 13-inch paper
		case DMPAPER_QUARTO:			//	Quarto, 215- by 275-millimeter paper
		case DMPAPER_10X14:			//	10- by 14-inch sheet
		case DMPAPER_11X17:			//	11- by 17-inch sheet
		case DMPAPER_NOTE:			//	Note, 8 1/2- by 11-inches
		case DMPAPER_ENV_9:			//	#9 Envelope, 3 7/8- by 8 7/8-inches
		case DMPAPER_ENV_10:			//	#10 Envelope, 4 1/8- by 9 1/2-inches
		case DMPAPER_ENV_11:			//	#11 Envelope, 4 1/2- by 10 3/8-inches
		case DMPAPER_ENV_12:			//	#12 Envelope, 4 3/4- by 11-inches
		case DMPAPER_ENV_14:			//	#14 Envelope, 5- by 11 1/2-inches
		case DMPAPER_ENV_DL:			//	DL Envelope, 110- by 220-millimeters
		case DMPAPER_ENV_C5:			//	C5 Envelope, 162- by 229-millimeters
		case DMPAPER_ENV_C3:			//	C3 Envelope,  324- by 458-millimeters
		case DMPAPER_ENV_C4:			//	C4 Envelope,  229- by 324-millimeters
		case DMPAPER_ENV_C6:			//	C6 Envelope,  114- by 162-millimeters
		case DMPAPER_ENV_C65:		//	C65 Envelope, 114- by 229-millimeters
		case DMPAPER_ENV_B4:			//	B4 Envelope,  250- by 353-millimeters
		case DMPAPER_ENV_B5:			//	B5 Envelope,  176- by 250-millimeters
		case DMPAPER_ENV_B6:			//	B6 Envelope,  176- by 125-millimeters
		case DMPAPER_ENV_ITALY:		//	Italy Envelope, 110- by 230-millimeters
		case DMPAPER_ENV_MONARCH:	//	Monarch Envelope, 3 7/8- by 7 1/2-inches
		case DMPAPER_ENV_PERSONAL:	//	6 3/4 Envelope, 3 5/8- by 6 1/2-inches
		case DMPAPER_FANFOLD_US:	//	US Std Fanfold, 14 7/8- by 11-inches
		case DMPAPER_FANFOLD_STD_GERMAN:	//	German Std Fanfold, 8 1/2- by 12-inches
		case DMPAPER_FANFOLD_LGL_GERMAN:	//	German Legal Fanfold, 8 1/2- by 13-inches
			
			//	Use the variable size envelope entry to represent unsupported paper sizes.
			m_PaperSize = kSpecialPaperSize;
			fKnownPaperSize = FALSE;
			break;
		
		default:
			fKnownPaperSize = FALSE;
			break;
		}

	return fKnownPaperSize;	
}

// ****************************************************************************
//
//  Function Name:	RPrinter::OpenPrinter( )
//
//  Description:		Open the printer for use.
//
//  Returns:			the open printer's HANDLE
//
//  Exceptions:		none
//
// ****************************************************************************
//
uLONG RPrinter::OpenPrinter(  ) 
	{
	//	Specify our current settings.
	PRINTER_DEFAULTS printerDefaults;
	printerDefaults.pDatatype = NULL;
	printerDefaults.pDevMode = (PDEVMODE)m_pDriverInfo;
	printerDefaults.DesiredAccess = PRINTER_ACCESS_USE;

	HANDLE hPrinter( NULL );

	//	Open the printer. Use our current settings if any. 
	BOOLEAN	fOpened;
	LPCSZ		pszPrinter	= m_rPrinterName;
	if ( m_pDriverInfo )
		fOpened = static_cast<BOOLEAN>( ::OpenPrinter( (LPSZ)pszPrinter, &hPrinter, &printerDefaults ) );
	else
		fOpened = static_cast<BOOLEAN>( ::OpenPrinter( (LPSZ)pszPrinter, &hPrinter, NULL ) );

	return (uLONG)hPrinter;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::ClosePrinter( const uLONG ulHandle ) 
//
//  Description:		Closes the given printer handle.
//
//  Returns:			nothing
//
//  Exceptions:		none
//
// ****************************************************************************
//
void RPrinter::ClosePrinter( const uLONG ulHandle ) 
	{
	TpsAssert( ulHandle, "ClosePrinter: NULL input handle." );

	BOOLEAN fClosed = static_cast<BOOLEAN>( ::ClosePrinter( (HANDLE)ulHandle ) );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetProperties( )
//
//  Description:		Get the printers properties into m_pDriverInfo.
//
//  Returns:			TRUE if OK was chosen form the properties dialog
//
//  Exceptions:		memory
//
// ****************************************************************************
//
BOOLEAN RPrinter::GetProperties( const uLONG ulPrinterHandle, const uLONG ulFlags ) 
	{
	BOOLEAN fPropertiesModified = FALSE;

	//	If we are a template printer, we have no properties to retrieve...
	if ( m_fTemplatePrinter )
		return FALSE;

	//	If we're just making sure there is something in m_pDriverInfo then we're done.
	if ( m_pDriverInfo && !( (ulFlags & kPrinterPropertiesPromptUser) || (ulFlags & kPrinterPropertiesForceUpdate) ) )
		return fPropertiesModified;

	//	The devmode properties live either in the printer driver or in this printer object if the
	//	user has changed them with the properties dialog. If m_pDriverInfo is NULL then the
	//	default info will be retrieved from the driver else the user's preferences (stored
	//	in m_pDriverInfo) will be used.
	HANDLE hPrinter = (HANDLE)ulPrinterHandle;
	DWORD dwfMode = m_pDriverInfo?DM_IN_BUFFER:0;
	long lResult = -1;
	PUBYTE lpNewDriverInfo = NULL;
	sLONG slNewDriverInfoSize = 0;
	
	//	Get the properties.
	if ( hPrinter )
		{
		//	Get the size of the m_pDriverInfo.
		LPCSZ		pszPrinterName	= m_rPrinterName;
		lResult = ::DocumentProperties( 
				AfxGetMainWnd( )->GetSafeHwnd()
				, hPrinter
				, (LPSZ)pszPrinterName
				, NULL
				, (PDEVMODE)m_pDriverInfo
				, 0
				);
		
		if( 0 < lResult )
			{
			//	Get a buffer for the device info. Remember the size of the device info.
				slNewDriverInfoSize = lResult;
				try
					{
					//	Allocate the devmode buffer.
					lpNewDriverInfo = new uBYTE[ slNewDriverInfoSize ];

					//	Disable app's window.
					//AfxGetMainWnd()->EnableWindow(FALSE);

					//	Get the device info.
					//	Prompt the user if requested.
					if (ulFlags & kPrinterPropertiesPromptUser)
						dwfMode |= DM_IN_PROMPT;

					LPCSZ		pszPrinter	= m_rPrinterName;
					lResult = ::DocumentProperties( 
							AfxGetMainWnd( )->GetSafeHwnd()
							, hPrinter
							, (LPSZ)pszPrinter
							, (PDEVMODE)lpNewDriverInfo
							, (PDEVMODE)m_pDriverInfo
							, dwfMode | DM_OUT_BUFFER 
							);
	
					//	Re-enable app's window.
					//AfxGetMainWnd()->EnableWindow(TRUE);

					if( IDOK == lResult )
						{
						//	Use the new info; delete the old.
						delete [] m_pDriverInfo;
						m_pDriverInfo = lpNewDriverInfo;
						m_yDriverInfoSize = slNewDriverInfoSize;
						fPropertiesModified = TRUE;
						}
					else
						{
						delete [] lpNewDriverInfo;
						lpNewDriverInfo = NULL;
						}
					}
				catch( YException& exception )
					{
					lResult = -1;
					delete [] lpNewDriverInfo;
					lpNewDriverInfo = NULL;
					::ReportException( exception );
					}
				catch( ... )
					{
					lResult = -1;
					delete [] lpNewDriverInfo;
					lpNewDriverInfo = NULL;
					// REVIEW: What else do we do here?
					::ReportException( kUnknownError );
					}
				}
			}

	//	Notify user of failure.
	if( (0 > lResult) && (kPrinterPropertiesPromptUser & ulFlags) )
		{
		RAlert rNoPropertiesAlert;
		rNoPropertiesAlert.AlertUser( STRING_ERROR_PRINTER_COULD_NOT_MODIFY_PROPERTIES );
		}
	else
		if ( fPropertiesModified && (kPrinterPropertiesForceUpdate & ulFlags) )
			//	Use new values.
			Update();

	return fPropertiesModified;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::CreateDrawingSurface( )
//
//  Description:		Returns a drawing surface for this device.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDrawingSurface* RPrinter::CreateDrawingSurface( RCancelPrintingSignal* pCancelPrintingSignal )
	{
	RPrinterDrawingSurface* pDrawingSurface = NULL;	
	try
		{
		HDC hdc = ::CreateDC( m_rDriverName, m_rPrinterName, m_rPortName, (DEVMODE*)m_pDriverInfo );

		if ( hdc )
			{
			//	Create the drawing surface. Do not use path support. We are printing.
			pDrawingSurface = new RPrinterDrawingSurface( pCancelPrintingSignal );
			if ( !pDrawingSurface->Initialize( hdc, hdc ) )
				{
				delete pDrawingSurface;
				pDrawingSurface = NULL;
				}
			}
		}
	catch( ... )
		{
		//	REVIEW ESV 10/9/96 - Should we warn the user here? What else should we do here?
		}
	
	return pDrawingSurface;
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::DestroyDrawingSurface( )
//
//  Description:		Deletes the given drawing surface.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::DestroyDrawingSurface( RPrinterDrawingSurface*& pDrawingSurface  )
	{
	//	Delete the printer dc then the drawing surface.
	if ( pDrawingSurface )
		{
		// Get the dc before we detach
		HDC hdc = reinterpret_cast<HDC>(pDrawingSurface->GetSurface( ));

		// Delete the drawing surface
		delete pDrawingSurface;
		pDrawingSurface = NULL;	
		
		// Delete the DC
		VERIFY( ::DeleteDC( hdc ) );
		}
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::TracePaperRequest( const uLONG ulPrinterHandle, const EPaperSize paper, const EOrientation orientation, RIntSize &rPaperSize, RIntSize &rPrintableArea ) const
//
//  Description:		Get the paper area and printable area for the given page size and orientation from the driver.
//
//  Returns:			TRUE: we successfully interogated the driver.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void TracePaperRequest( const RMBCString &rDriverName, const RMBCString& rPrinterName, const RMBCString& rPortName, RPrinter::EOrientation orientation, RPrinter::EPaperSize paper )
	{
	OutputDebugString( "Getting paper size: " );
	OutputDebugString( rPrinterName );
	OutputDebugString( " " );
	OutputDebugString( rDriverName );
	OutputDebugString( " " );
	OutputDebugString( rPortName );
	OutputDebugString( (RPrinter::kPortrait == orientation)?" portrait ":" landscape " );

	switch( paper )
		{
		case RPrinter::kUSLetter:
			OutputDebugString( "US Letter " );
			break;
		case RPrinter::kUSLegal:
			OutputDebugString( "US Legal " );
			break;
		case RPrinter::kTabloid:
			OutputDebugString( "US Tabloid " );
			break;
		case RPrinter::kA4Letter:
			OutputDebugString( "A4 Letter " );
			break;
		case RPrinter::kA3Tabloid:
			OutputDebugString( "A3 Tabloid " );
			break;
		case RPrinter::kB5Letter:
			OutputDebugString( "B5 Letter " );
			break;
		case RPrinter::kSpecialPaperSize:
			OutputDebugString( "Special paper " );
			break;
		default:
			TpsAssertAlways( "Invalid paper size " );
		}
		
	OutputDebugString( "\n" );
	}

// ****************************************************************************
//
//  Function Name:	RPrinter::GetPaperAreasFromDriver( const uLONG ulPrinterHandle, const EPaperSize paper, const EOrientation orientation, RIntSize &rPaperSize, RIntSize &rPrintableArea ) const
//
//  Description:		Get the paper area and printable area for the given page size and orientation from the driver.
//
//  Returns:			TRUE: we successfully interogated the driver.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinter::GetPaperAreasFromDriver( const uLONG ulPrinterInfo, const EPaperSize paper, const EOrientation orientation, RIntSize &rPaperSize, RIntRect &rPrintableArea )
{
	// If paper is not supported, we can't get the values from the driver
	if( !IsPaperSupported( paper ) )
		return FALSE;

	BOOLEAN fSuccess = TRUE;
	
	if ( !m_fTemplatePrinter )
		{
		CDC printerDC;

		// Get the page area with the given orientation from the driver.
		//	If no property info is available
		//	(as can happen if an incompletely installed driver is selected) then make this 
		//	a template printer below by setting fSuccess FALSE here.
		PRINTER_INFO_2* pPrinterInfo = (PRINTER_INFO_2*)ulPrinterInfo;

		LPDEVMODE pDevMode = (LPDEVMODE)m_pDriverInfo;
		if ( !pDevMode )
			pDevMode = pPrinterInfo->pDevMode;

		fSuccess = static_cast<BOOLEAN>( NULL != pDevMode );

		// Get the paper type
		if ( fSuccess )
			{		
			//	Save the driver info we need to modify to get the page area.
			DWORD		dmFields = pDevMode->dmFields; 
			short		dmOrientation = pDevMode->dmOrientation; 
			short		dmPaperSize = pDevMode->dmPaperSize; 

			//	Slam the desired page oriention and page size in the driver info so that it will give us
			//	the right info.
			pDevMode->dmFields |= DM_ORIENTATION | DM_PAPERSIZE; 
			pDevMode->dmOrientation = (short)((kPortrait == orientation)?DMORIENT_PORTRAIT:DMORIENT_LANDSCAPE); 
			pDevMode->dmPaperSize = (m_PaperAreas.Start())[paper].m_swPlatformPaperId;//  RPrinter::GetPlatformPaperId( paper ); 

			//	Get the page info from the driver.
			BOOLEAN fSuccess = static_cast<BOOLEAN>( printerDC.CreateIC( m_rDriverName, m_rPrinterName, m_rPortName, pDevMode ) );
			if ( fSuccess )
				{
				//	Scale values from the driver to our system resolution.
				YFloatType scaleX = (YFloatType)kSystemDPI / printerDC.GetDeviceCaps( LOGPIXELSX );
				YFloatType scaleY = (YFloatType)kSystemDPI / printerDC.GetDeviceCaps( LOGPIXELSY );

				rPaperSize.m_dx = ::Round( printerDC.GetDeviceCaps( PHYSICALWIDTH ) * scaleX );
				rPaperSize.m_dy = ::Round( printerDC.GetDeviceCaps( PHYSICALHEIGHT ) * scaleY ); 
				
				rPrintableArea.m_Left = ::Round( printerDC.GetDeviceCaps( PHYSICALOFFSETX ) * scaleX );
				rPrintableArea.m_Top = ::Round( printerDC.GetDeviceCaps( PHYSICALOFFSETY ) * scaleY );
				rPrintableArea.m_Right = rPrintableArea.m_Left + ::Round( printerDC.GetDeviceCaps( HORZRES ) * scaleX );
				rPrintableArea.m_Bottom = rPrintableArea.m_Top + ::Round( printerDC.GetDeviceCaps( VERTRES ) * scaleY ); 

				//	Clean up the allocated dc.
				BOOLEAN fDCDeleted = static_cast<BOOLEAN>( printerDC.DeleteDC() );
				VERIFY( fDCDeleted );
				}
			else
				{
				//	Could not create an IC - so this can't be used as a valid printer
				//		mark it as a template for now...
				MakeTemplate( kPrinterDriverFailure );
				//	Return the current values.
				rPaperSize = const_cast<RPrinter*>(this)->GetPaperArea( m_PaperSize, m_Orientation, !kUpdatePaperAreas );
				rPrintableArea = const_cast<RPrinter*>(this)->CalcPrintableAreaRect( m_PaperSize, m_Orientation, !kUpdatePaperAreas );
				//
				//	We now have values that are valid
				fSuccess = TRUE;
				}

			//	Restore the info fields we changed when we created the dc.
			pDevMode->dmFields = dmFields; 
			pDevMode->dmOrientation = dmOrientation; 
			pDevMode->dmPaperSize = dmPaperSize; 
			}
		}
	else
		{
		//	Return the current values.
		rPaperSize = const_cast<RPrinter*>(this)->GetPaperArea( m_PaperSize, m_Orientation, !kUpdatePaperAreas );
		rPrintableArea = const_cast<RPrinter*>(this)->CalcPrintableAreaRect( m_PaperSize, m_Orientation, !kUpdatePaperAreas );
		}

	TpsAssert( rPrintableArea.Width() <= rPaperSize.m_dx, "Printable area wider than page area!" );
	TpsAssert( rPrintableArea.Height() <= rPaperSize.m_dy, "Printable area taller than page area!" );

	return fSuccess;
}

// ****************************************************************************
//
//  Function Name:	RPrinter::UpdatePaperInfo
//
//  Description:		Updates the cached paper info if it is dirty
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinter::UpdatePaperInfo( const EPaperSize paper, const EOrientation orientation )
	{
	// If paper is not supported, we have nothing to update
	if( !IsPaperSupported( paper ) )
		return ;

	RPaperAreaCollection::YIterator Iter = m_PaperAreas.Start( );

	// If the cached info is not valid, retreive it
	if( ( orientation == kPortrait	&& ( !Iter[paper].m_fPortraitSizeValid		|| !Iter[paper].m_fPortraitPrintableAreaValid ) ) ||
		 ( orientation == kLandscape	&& ( !Iter[paper].m_fLandscapeSizeValid	|| !Iter[paper].m_fLandscapePrintableAreaValid ) ) )
		{
		PRINTER_INFO_2* pPrinterInfo = NULL;

		// Open the printer
		HANDLE hPrinter = (HANDLE)OpenPrinter( );

		// Get the printer info
		if( hPrinter )
			{
			DWORD dwRequestedBuffSize = 0;
			DWORD dwUsedBuffSize = 0;

			//	Get the size of the buffer. Ignore the return value since GetPrinter() LIES.
			::GetPrinter( hPrinter, 2, NULL, 0, &dwRequestedBuffSize );
			
			try
				{
				pPrinterInfo = (PRINTER_INFO_2*)new uBYTE[ dwRequestedBuffSize ];
				::memset( pPrinterInfo, 0, dwRequestedBuffSize );
				::GetPrinter( hPrinter, 2, (PUBYTE)pPrinterInfo, dwRequestedBuffSize, &dwUsedBuffSize );
				}

			catch( ... )
				{
				;
				}

			// Now we have the printer info. Get the paper info
			RIntSize paperSize;
			RIntRect printableArea;
			if( GetPaperAreasFromDriver( (uLONG)pPrinterInfo, paper, orientation, paperSize, printableArea ) )
				{
				// Fill in the cached info
				if( orientation == kPortrait )
					{
					if( !Iter[paper].m_fPortraitSizeValid )
						Iter[paper].rPortraitSize = paperSize;

					if( !Iter[paper].m_fPortraitPrintableAreaValid )
						Iter[paper].rPortraitPrintableArea = printableArea;

					Iter[paper].m_fPortraitSizeValid = TRUE;
					Iter[paper].m_fPortraitPrintableAreaValid = TRUE;
					}

				else
					{
					if( !Iter[paper].m_fLandscapeSizeValid )
						Iter[paper].rLandscapeSize = paperSize;

					if( !Iter[paper].m_fLandscapePrintableAreaValid )
						Iter[paper].rLandscapePrintableArea = printableArea;

					Iter[paper].m_fLandscapeSizeValid = TRUE;
					Iter[paper].m_fLandscapePrintableAreaValid = TRUE;
					}
				}
			}
	
		//	Clean up.
		if( hPrinter )
			ClosePrinter( (uLONG)hPrinter );

		delete [] pPrinterInfo;
		}
	}
