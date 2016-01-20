// ****************************************************************************
//
//  File Name:			PrinterDatabase.cpp
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
//  $Logfile:: /PM8/Framework/Source/PrinterDatabase.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"PrinterDatabase.h"
#include	"FileStream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::RPrinterDatabase( )
//
//  Description:		ctor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDatabase::RPrinterDatabase( )
: m_fChanged( FALSE ), m_pFile( NULL ), m_yVersion( kVersionCurrentPrinterDB )
	{
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::~RPrinterDatabase( )
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDatabase::~RPrinterDatabase( )
	{
	TpsAssert( !m_fChanged, "Destroying unsaved printer database data." );

	//	Close and release the database file.
	delete m_pFile;
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::GetVersion( ) const
//
//  Description:		Return the version of the printer database.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
uBYTE RPrinterDatabase::GetVersion( ) const
	{
	return m_yVersion;
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::SetVersion( ) const
//
//  Description:		Install the given version of the printer database.
//
//  Returns:			VOID
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDatabase::SetVersion( const uBYTE version )
	{
	m_yVersion = version;
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::GetPrinterCompensationCollection( )
//
//  Description:		Return the printer compensation collection.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterCompensationCollection& RPrinterDatabase::GetPrinterCompensationCollection( )
	{
	return m_ActualPrinterInfo;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::GetDatabaseFile( )
//
//  Description:		Return the database file.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFileStream*& RPrinterDatabase::GetDatabaseFile( )
	{
	if ( NULL == m_pFile )
		m_pFile = new RFileStream();

	return m_pFile;
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::Startup( const RMBCString& rDatabaseFilename )
//
//  Description:		Load printer database.
//
//  Returns:			A new printer database with entries from file.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterDatabase* RPrinterDatabase::Startup( const RMBCString& rDatabaseFilename )
	{
	RPrinterDatabase* pDB = NULL;
	RFileStream* pDatabaseFile = NULL;

	try
		{
		//	Make a new printer database.
		pDB = new RPrinterDatabase;

		//	Load its entires from file.
		pDatabaseFile = pDB->GetDatabaseFile( );
		pDatabaseFile->Open( rDatabaseFilename, kReadWrite );

		//	If the database has entries then read them in.
		if ( pDatabaseFile->GetSize() )
			{
			//	Get the number of printer information records in the file.
			//	Load each entry into the database.
			YCounter i;
			YCounter yNumRecords;
			
			//	Make sure we have a printer database file. They contain our header.
			//	Read in the header info which defines our version.
			RMBCString rDbHeader;
			*pDatabaseFile >> rDbHeader;
			if ( rDbHeader == kDbInitialFileHeader )
				pDB->SetVersion( kVersionInitialPrinterDB );
			else if ( rDbHeader == kDbDriverVersionFileHeader )
				pDB->SetVersion( kVersionDriverPrintableAreaPrinterDB );
			else
				::ThrowException( kDataFormatInvalid );

			uLONG ulTemp;
			*pDatabaseFile >> ulTemp;
			yNumRecords = (YCounter)ulTemp;
			RPrinterCompensationCollection& rDB = pDB->GetPrinterCompensationCollection( );

			for( i = 0; i < yNumRecords; ++i )
				rDB.InsertAtEnd( RPrinterCompensation( pDatabaseFile, pDB->GetVersion( ) ) );
			}
		}
	catch( YException& exception )
		{
		// REVIEW: What else do we do here?
		if ( kDataFormatInvalid != exception )
			::ReportException( exception );
		}
	catch( ... )
		{
		// REVIEW: What else do we do here?
		::ReportException( kUnknownError );
		}

	//	Shut down the database file. We don't leave it open because during 
	//	development we abort the app frequently and the file doesn't get resized.
	if ( pDatabaseFile )
		pDatabaseFile->Close();

	return pDB;
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::UpdatePrinter( const RPrinter* pPrinter )
//
//  Description:		Set the given printer's printable area to one found in the database
//							or leave it alone if it was not found.
//
//  Returns:			TRUE: the printable area was changed.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPrinterDatabase::UpdatePrinter( RPrinter* pPrinter )
	{
	// ensure that the correct orientation is also returned for printer database entries.
	RPrinterCompensation* pPrinterInDb = static_cast<RPrinterCompensation*>( FindPrinter( pPrinter ) );
	if ( pPrinterInDb )
		{
		pPrinter->SetPrintableArea( pPrinterInDb->GetActualPrintableArea(), pPrinter->GetPaperSize( ), pPrinter->GetOrientation( ) );
		return TRUE;
		}
	else
		return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::UpdatePrinter( const RPrinter* pPrinter, const RIntRect& rNewPrintableArea )
//
//  Description:		Record the given printable area in a printer compensation record in the database.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDatabase::UpdatePrinter( RPrinter* pPrinter, const RIntRect& rNewPrintableArea )
	{
	//	Adjust the printable area in the database.
	RPrinterCompensation* pPrinterInDb = FindPrinter( pPrinter );
	if ( pPrinterInDb )
		{
		pPrinterInDb->Update( rNewPrintableArea );
		}
	else
		{
		//	The printer isn't there. Add a new one.
		m_ActualPrinterInfo.InsertAtEnd( RPrinterCompensation( *pPrinter, rNewPrintableArea ) );
		}

	//	Remember to save the database.
	m_fChanged = TRUE;
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::FindPrinter( const RPrinter* pPrinter )
//
//  Description:		Return a matching printer in the database or NULL.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPrinterCompensation* RPrinterDatabase::FindPrinter( RPrinter* pPrinter )
	{
	TpsAssert( pPrinter, "Null input printer" );

	//	Find the printer in the database.
	RPrinterCompensation* pPrinterBestMatch = NULL;

	RPrinterCompensationCollection::YIterator rIter = m_ActualPrinterInfo.Start();
	YConfidence yBestMatch = kNoConfidence;
	YConfidence yCurrentMatch = kNoConfidence;
	for(; rIter != m_ActualPrinterInfo.End(); ++rIter )
		{
		yCurrentMatch = pPrinter->IsSimilar( &(*rIter), kCompareInPrinterDB );

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
//  Function Name:	RPrinterDatabase::Shutdown( )
//
//  Description:		Release printer database.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDatabase::Shutdown( )
	{
	Update( );
	}


// ****************************************************************************
//
//  Function Name:	RPrinterDatabase::Update( )
//
//  Description:		Save printer database.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDatabase::Update( )
	{
	try
		{
		//	Rewrite database if it has changed.
		//	Do a safe save.
		if ( m_fChanged /*|| (0 == m_pFile->GetSize())*/ )
			{
			m_pFile->Open( kReadWrite );
			m_pFile->SeekAbsolute( 0 );
			*m_pFile << kDbDriverVersionFileHeader;
			*m_pFile << (uLONG)m_ActualPrinterInfo.Count();

			RPrinterCompensationCollection::YIterator rIter = m_ActualPrinterInfo.Start();
			for(; rIter != m_ActualPrinterInfo.End(); ++rIter )
				(*rIter).Write( m_pFile );
			m_pFile->Close( );
			}

		//	Remember that we saved our data.
		m_fChanged = FALSE;
		}
	catch( YException& exception )
		{
		// REVIEW: What else do we do here?
		::ReportException( exception );
		}
	catch( ... )
		{
		// REVIEW: What else do we do here?
		::ReportException( kUnknownError );
		}
	}

