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
//  $Logfile:: /PM8/Framework/Source/PrintManager.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PrintManager.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"ChunkyStorage.h"
#include	"PrinterDatabase.h"
#include "ResourceManager.h"
#include "FrameworkResourceIDs.h"
#include "ApplicationGlobals.h"
#include "Document.h"
#include "StandaloneApplication.h"

//	Declare static global print manager.
RPrintManager RPrintManager::m_PrintManager;
REvent RPrintManager::m_InitializedEvent( TRUE, FALSE );
BOOLEAN RPrintManager::m_fPrintManagerInitialized = FALSE;
const YThreadResult kNormalTerminate = 2000;

//	print manager constants
const YVersion			kMinPrinterExtraInfoVersion		= kDefaultDataVersion + (YVersion)1;


// ****************************************************************************
//
//  Function Name:	RPrintManager::RPrintManager( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrintManager::RPrintManager( )
	:m_CurrentPrinter(0), m_pUserDatabase( NULL )
	, m_pVendorDatabase( NULL ), m_pOpenDocuments( new RDocumentCollection( ) )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::~RPrintManager( )
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrintManager::~RPrintManager( )
	{
	delete m_pOpenDocuments;
	delete m_pUserDatabase;
	delete m_pVendorDatabase;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::Startup
//
//  Description:		Intitializes the print manager
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::Startup( )
	{
	// Start a thread to do the rest of the initialization
	::CreateThread( InitializePrintManagerThreadFunction, &m_PrintManager, THREAD_PRIORITY_LOWEST );
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::InitializePrintManagerThreadFunction( )
//
//  Description:		Thread to initialize the print manager. This can take a
//							while, so we do it in the background.
//
//  Returns:			kNormalTerminate
//
//  Exceptions:		None
//
// ****************************************************************************
//
YThreadResult RPrintManager::InitializePrintManagerThreadFunction( void* pThreadParam )
	{
	// Get the print manager
	RPrintManager* pPrintManager = static_cast<RPrintManager*>( pThreadParam );

	//	Get the location of the printer db files in rPrinterDBPath.
	RMBCString rAppPath( ::GetApplication( ).GetApplicationPath( ) );
	RMBCString rUserPrinterDBPath( rAppPath );
	RMBCString rVendorPrinterDBPath( rAppPath );
	rUserPrinterDBPath += GetResourceManager().GetResourceString( STRING_PRINTER_USER_DB_NAME );
	rVendorPrinterDBPath += GetResourceManager().GetResourceString( STRING_PRINTER_VENDOR_DB_NAME );

	//	Start up printer databases.
	if ( !pPrintManager->m_pUserDatabase )
		pPrintManager->m_pUserDatabase = RPrinterDatabase::Startup( rUserPrinterDBPath );

	if ( !pPrintManager->m_pVendorDatabase )
		pPrintManager->m_pVendorDatabase = RPrinterDatabase::Startup( rVendorPrinterDBPath );

	// Get the attached printers. This can take a while.
	pPrintManager->GetAttachedPrinters( );

	//	Remember that we are ready to go.
	pPrintManager->m_fPrintManagerInitialized = TRUE;

	// Signal that we are done
	m_InitializedEvent.Set( );

	return kNormalTerminate;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::Shutdown( )
//
//  Description:		Release printer databases.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::Shutdown( )
	{
	//	Clean up printer databases.
	if ( m_pUserDatabase )
		m_pUserDatabase->Shutdown();
	delete m_pUserDatabase;
	m_pUserDatabase = NULL;

	if ( m_pVendorDatabase )
		m_pVendorDatabase->Shutdown();
	delete m_pVendorDatabase;
	m_pVendorDatabase = NULL;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::GetPrintManager( )
//
//  Description:		The global list of available printers.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrintManager& RPrintManager::GetPrintManager( )
	{
	// Wait on our initialized event
	m_InitializedEvent.Wait( );

	// Return the singleton
	return m_PrintManager;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::SetDefaultPrinter( const YCounter yNewDefault )
//
//  Description:		Set the default printer. Update it's values.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::SetDefaultPrinter( const YCounter yNewDefault )
	{
	RPrinterCollection::YIterator rIter = m_AttachedPrinters.Start();

	m_CurrentPrinter = yNewDefault;
	rIter[ m_CurrentPrinter ].Update( );
	}


// ****************************************************************************
//
//  Function Name:	RPrintManager::GetDefaultPrinterIndex( )
//
//  Description:		Return default printer index in the printer list.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter RPrintManager::GetDefaultPrinterIndex( )
	{
	return m_CurrentPrinter;
	}


// ****************************************************************************
//
//  Function Name:	RPrintManager::GetPrinterIndex( )
//
//  Description:		Return index of the given printer in the printer list.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter RPrintManager::GetPrinterIndex( const RPrinter* pPrinter )
	{
	YCounter yIndex = 0;
	RPrinterCollection::YIterator rIter = m_AttachedPrinters.Start();
	for( ;( rIter != m_AttachedPrinters.End() ) && ( pPrinter != &(*rIter) ); ++rIter, ++yIndex )
		NULL;

	//	If we didn't find the printer return the default printer index.
	if ( m_AttachedPrinters.Count() == yIndex )
		yIndex = m_CurrentPrinter;
	
	return yIndex;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::GetDefaultPrinter( )
//
//  Description:		Return default printer. Could be the template printer if none are installed.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter* RPrintManager::GetDefaultPrinter( )
	{
	//	Return the current selected printer. Note that this printer may have been
	//	deleted. A deleted printer can still be returned if there are no undeleted printers.
	RPrinterCollection::YIterator rIter = m_AttachedPrinters.Start();
	return &(rIter[ m_CurrentPrinter ]);
	}


// ****************************************************************************
//
//  Function Name:	RPrintManager::FindPrinter( const rDriverName, const uLONG ulCompareFlags /*= kCompareStandard*/ )
//
//  Description:		Return a matching printer in the printer collection or NULL.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter* RPrintManager::FindPrinter( const RMBCString& rDriverName, const uLONG ulCompareFlags /*= kCompareStandard*/ )
	{
	//	Find the given printer in the list of attached printers
	RPrinter printer( rDriverName );
	RPrinter* pPrinter = RPrintManager::GetPrintManager( ).FindPrinter( &printer, kComparePortOrDriverName );
	
	return FindPrinter( pPrinter, ulCompareFlags );
	}


// ****************************************************************************
//
//  Function Name:	RPrintManager::FindPrinter( RPrinter* pPrinter )
//
//  Description:		Return a matching printer in the printer collection or NULL.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter* RPrintManager::FindPrinter( const RPrinter* pPrinter, const uLONG ulCompareFlags /*= kCompareStandard*/ )
	{
	TpsAssert( pPrinter, "Null input printer" );

	//	Find the printer in the collection of attached printers.
	RPrinter* pPrinterBestMatch = NULL;

	RPrinterCollection::YIterator rIter = m_AttachedPrinters.Start();
	YConfidence yBestMatch = kNoConfidence;
	YConfidence yCurrentMatch = kNoConfidence;
	for(; rIter != m_AttachedPrinters.End(); ++rIter )
		{
		yCurrentMatch = pPrinter->IsSimilar( &(*rIter), ulCompareFlags );

		//	Get collection printer which is the best match.
		if ( yCurrentMatch > yBestMatch )
			{
			yBestMatch = yCurrentMatch;
			pPrinterBestMatch = &(*rIter);
			}
		}
	
	return pPrinterBestMatch;
	}


// ****************************************************************************
//
//  Function Name:	RPrintManager::AddPrinter( const RPrinter* pPrinter )
//
//  Description:		Add the given printer to the attached collection and return its new address
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter* RPrintManager::AddPrinter( const RPrinter* pPrinter )
	{
	TpsAssert( pPrinter, "Null input printer" );
	
	//	Add the printer.
	m_AttachedPrinters.InsertAtEnd( *pPrinter );
	
	//	Return its new home in the printer collection.
	RPrinterCollection::YIterator rIter = m_AttachedPrinters.End();
	
	return &( *(--rIter) );
	}


// ****************************************************************************
//
//  Function Name:	RPrintManager::DeletePrinter( const RPrinter* pPrinter )
//
//  Description:		Remove the given printer to the attached collection and return its old address
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinter* RPrintManager::DeletePrinter( const RPrinter* pPrinter )
	{
	TpsAssert( pPrinter, "Null input printer" );
	
	//	Mark the printer as deleted.
	RPrinter* pAttachedPrinter = FindPrinter( pPrinter );
	TpsAssert( pAttachedPrinter, "Deleting a printer that was never attached." );

	pAttachedPrinter->Delete( );
	
	//	Return its old home in the printer collection.	
	return pAttachedPrinter;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::AttachedPrintersChanged( const RMBCString rInfo )
//
//  Description:		Called when the default printer has been changed outside of Renaissance.
//
//  Returns:			the reason the printer changed; see flags at top of printer.h
//
//  Exceptions:		None
//
// ****************************************************************************
//
uLONG RPrintManager::AttachedPrintersChanged( const RMBCString& /*rInfo*/ )
	{
	RWaitCursor waitCursor;

	RMBCString rSystemDefaultPrinterName;

	//	Examine the system to see if:
	//	printers were added, removed or the default printer was changed.
	//	Collect flags indicating these events in ulReason.
	
	//	Always assume the printer has changed since we can't be
	//	sure the driver didn't change its internal info.
	uLONG ulReason = kChangePrinterChanged;

	//	Get the default printer from the system to see if its changed.
	if( GetSystemDefaultPrinterName( rSystemDefaultPrinterName ) )
		{
		//	Check to see if printers were deleted.
		//	If the printer fails to update it was deleted.
		//	Also get new paper sizes from the driver when we update
		// in case they've changed.
		RPrinterCollection::YIterator rIter = m_AttachedPrinters.Start();
		for(; rIter != m_AttachedPrinters.End(); ++rIter )
			if ( !(*rIter).Update( ) )
				ulReason |= kChangePrinterDeleted;

		//	Check to see if printers were added.
		if ( GetAttachedPrinters( ) )
			ulReason |= kChangePrinterAdded;

		//	Compare the Renaissance default printer to the system default printer.
		//	We only care about the driver name because we will install
		//	our paper size and orientation.
		RPrinter SystemDefault( rSystemDefaultPrinterName );
			if ( !GetDefaultPrinter( )->IsSimilar( &SystemDefault, kCompareDriverName ) )
			{
			ulReason |= kChangeDefaultChanged;
			SetDefaultPrinter( rSystemDefaultPrinterName );
			}
		}
	else
		{
		//	No printers are attached. There can only be no system default printer when there are no printers.
		ulReason = kChangeNoPrintersAttached;
		}
	
	return ulReason;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::SetDefaultPrinter( const RMBCString& rDriverName )
//
//  Description:		Gets the default printer name from the os.
//
//  Returns:			TRUE if the default printer was found.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintManager::SetDefaultPrinter( const RMBCString& rDriverName )
	{
	BOOLEAN fSuccess = FALSE;

	//	Search for the current printer in the list of attached printers.
	RPrinterCollection::YIterator rIter = m_AttachedPrinters.Start();
	YCounter i = 0;
	RMBCString rAttachedDriverName;
	RMBCString rAttachedPrinterName;
	for(; rIter != m_AttachedPrinters.End(); ++rIter, ++i )
		{
		//	REVIEW ESV 4/23/97 - Must use a real printer not a driver name for this
		//	so we are not confused when there are two printers on the system with the same
		// name connected to different ports and one of them is the default.

		//	Truncate the printer and driver name to the length of the given string
		//	since we are temporarily using driver names from a routine which
		//	truncates to 32 chars.
		rAttachedDriverName = (*rIter).GetDriverName().Head( Min( rDriverName.GetDataLength( ), (*rIter).GetDriverName().GetDataLength( ) ) );
		rAttachedPrinterName = (*rIter).GetPrinterName().Head( Min( rDriverName.GetDataLength( ), (*rIter).GetPrinterName().GetDataLength( ) ) );
		if ( ( rDriverName == rAttachedDriverName )
		|| ( rDriverName == rAttachedPrinterName ) )
			{
			//	Save index and get current settings.
			m_CurrentPrinter = i;
			fSuccess = TRUE;
			break;
			}
		}
	TpsAssert( i < m_AttachedPrinters.Count(), "Current printer not found on list of available printers." );
	
	return fSuccess;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::CanTestDefaultPrinter( )
//
//  Description:		Return TRUE if the user can test the current default printer.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrintManager::CanTestDefaultPrinter( )
	{
	//	We can test everything except the template printer.
	return static_cast<BOOLEAN>( !GetDefaultPrinter( )->IsTemplate() );
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::UpdatePrinterFromDatabase( RPrinter* pPrinter )
//
//  Description:		Set the given printer's printable area to one found in either
//							the user printer db or the vendor printer db or the original
//							value if it is not found.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::UpdatePrinterFromDatabase( RPrinter* pPrinter )
	{
	//	Update the given printer. Do nothing if we have no databases or the print manager
	//	isn't available yet.
	if ( m_fPrintManagerInitialized && m_pUserDatabase && m_pVendorDatabase && !m_pUserDatabase->UpdatePrinter( pPrinter ) )
		m_pVendorDatabase->UpdatePrinter( pPrinter );
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::UpdatePrinterFromDatabase( ... )
//
//  Description:		Change the given printer's printable area in
//							the user printer db.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::UpdatePrinterFromDatabase( RPrinter* pPrinter, const RIntRect& rNewPrintableArea )
	{
	//	Update the given printer. Do nothing if we have no databases or the print manager
	//	isn't available yet.
	if ( m_fPrintManagerInitialized && m_pUserDatabase )
		{
		m_pUserDatabase->UpdatePrinter( pPrinter, rNewPrintableArea );
		
		//	Update the printer in the open print manager.
		RPrintManager::GetPrintManager( ).UpdatePrinterFromDatabase( pPrinter );

		//	Save changed database.
		m_pUserDatabase->Update( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::GetPrinterCollection( )
//
//  Description:		Return the printer collection.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterCollection* RPrintManager::GetPrinterCollection( )
	{
	return &m_AttachedPrinters;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::GetStatus( )
//
//  Description:		Get reference to status member
//
//  Returns:			reference to status member
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterStatus* RPrintManager::GetStatus()
	{
	return &m_Status;
	}

// ****************************************************************************
//
//  Function Name:	RPrintManager::CompensatePrintableArea( const RIntRect rSideCalibration, const RIntSize rCenterOffset  )
//
//  Description:		return the printers printable area after applying the given compensation values to it.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RPrintManager::CompensatePrintableArea( const RIntRect rSideCalibration, const RIntSize rCenterOffset, RPrinter* pPrinter )
	{
	//	Adjust printable area per given calibrations.
	//	Use the center calibration markes to offset the printable area.
	RIntRect rPrintableArea = pPrinter->GetPrintableArea();
	RIntRect rPaperArea = pPrinter->GetPaperArea();
	RIntSize	yIncrementSize( ::InchesToLogicalUnits( kMarkIncrement ), ::InchesToLogicalUnits( kMarkIncrement ) );
	RIntPoint rTopLeft = rPrintableArea.m_TopLeft;
	rTopLeft.m_x -= rCenterOffset.m_dx * yIncrementSize.m_dx;
	rTopLeft.m_y -= rCenterOffset.m_dy * yIncrementSize.m_dy;

	//	Figure out how much larger or smaller the printable area is than is reported by the driver.
	//	The test page places the calibration mark 16 right at the edge of the printable area. Record
	//	in rPrintableAreaInset the distance to shrink the printable area based on the user's calibration 
	// response.

	RIntRect rPrintableAreaInset;
	rPrintableAreaInset.m_Left		= (rSideCalibration.m_Left - 17) * yIncrementSize.m_dx;
	rPrintableAreaInset.m_Top		= (rSideCalibration.m_Top - 17) * yIncrementSize.m_dy;
	rPrintableAreaInset.m_Right	= (rSideCalibration.m_Right - 17) * yIncrementSize.m_dx;
	rPrintableAreaInset.m_Bottom	= (rSideCalibration.m_Bottom - 17) * yIncrementSize.m_dy;
	RIntSize rPrintableAreaSize = rPrintableArea.WidthHeight();
	rPrintableAreaSize.m_dx -= rPrintableAreaInset.m_Left + rPrintableAreaInset.m_Right;
	rPrintableAreaSize.m_dy -= rPrintableAreaInset.m_Top + rPrintableAreaInset.m_Bottom;

	//	Return printable area.
	rPrintableArea = RIntRect( rTopLeft, rPrintableAreaSize );

	// ensure the printable area is not larger than the current paper size
	rPrintableArea.Intersect( rPrintableArea, rPaperArea );

	return rPrintableArea;
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPrintManager::Validate( )
//
//  Description:		Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrintManager::Validate( ) const
{
	RObject::Validate();
	TpsAssertIsObject( RPrintManager, this );
}

#endif	//	TPSDEBUG

