// ****************************************************************************
//
//  File Name:			CollectionManager.cpp
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow, John Fleischhauer, Brad Nelson
//
//  Description:		Definition of RCollectionManager class that locates,
//							opens and closes online Renaissance collection files
//
//  Portability:		Win32 and MFC
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "FrameworkIncludes.h"	// standard Framework includes

ASSERTNAME

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>

#include "CollectionManager.h"			// header for this file
#include "RenaissanceResource.h"			// alert string defines
#include "Configuration.h"					// registry configuration declarations
#include "CollectionBuilderTypes.h"		// cbUSETYPES enum


// minimum expected collection file format version: 500=v5.0, 610=v6.1 etc.
const short kExpectedMinFormatVersion = 600;
const short kFormatVersion6 = 600;

/****************************************************************************
	RCollectionManager& RCollectionManager::TheCollectionManager()

	Return reference to the single instance of a RCollectionManager
*****************************************************************************/

RCollectionManager& RCollectionManager::TheCollectionManager()
{
	static RCollectionManager theCollectionManager;
	return theCollectionManager;
}

/****************************************************************************
	RCollectionManager::RCollectionManager()

	Constructor for RCollectionManager class
*****************************************************************************/

RCollectionManager::RCollectionManager()
	:	m_collectionIDArray(),
		m_refPathArray(),
		m_pCDDriveBuf( NULL ),
		m_nCDDrives( 0 ),
		m_emptyCollectionArray()
{
	// save path to Collections directory into dirPath
	char szAppPath[ _MAX_PATH ];
	char szDrive[ _MAX_DRIVE ];
	char szDir[ _MAX_DIR ];
	DWORD getModuleFileNameResult = ::GetModuleFileName( NULL, szAppPath, _MAX_PATH );
	ASSERT( getModuleFileNameResult );
	::_splitpath( szAppPath, szDrive, szDir, NULL, NULL );
	::strcpy( dirPath, szDrive );
	::strcat( dirPath, szDir );

#if 0 // REVIEW GCB 12/10/97 - should put these strings in resources
	loadStringResult = ::LoadString( ::AfxGetInstanceHandle(),
								CD_REFERENCE_DIR, m_CDReferenceDir, _MAX_DIR );
	ASSERT( loadStringResult );
#else
	::strcpy( m_CDReferenceDir, "Photos\\" );
#endif

	// get a reckoning of all CD drives on the system
	const int nMaxCDs = 20;						   // a more than reasonable cap
	m_pCDDriveBuf = new char[ nMaxCDs + 1 ];
	m_nCDDrives = GetCDROMDrives( m_pCDDriveBuf, nMaxCDs );	// get CD drives

	// initialize array of collection filename extensions
	extensions[ kGraphicsCollectionType ] = ".grc";
	extensions[ kBackdropsCollectionType ] = ".bkc";
	extensions[ kLayoutsCollectionType ] = ".lyc";
	extensions[ kReadyMadesCollectionType ] = ".rmc";
	extensions[ kLinesCollectionType ] = ".lnc";
	extensions[ kBordersCollectionType ] = ".brc";
	extensions[ kCustomCollectionType ] = ".cgc";
	extensions[ kQuotesAndVersesCollectionType ] = ".qvc";
	extensions[ kTexturesCollectionType ] = ".btc";
	extensions[ kEdgeMasksCollectionType ] = ".emc";
	extensions[ kFullMasksCollectionType ] = ".fmc";
	extensions[ kFramesCollectionType ] = ".frc";

	// Leapfrog-specific collection file types
	extensions[ kNavigationButtonsCollectionType ] = ".nbc";
	extensions[ kWebObjectsCollectionType ] = ".woc";
	extensions[ kAnimagedGIFCollectionType ] = ".agc";
	extensions[ kBackdropGIFCollectionType ] = ".bgc";
}

/****************************************************************************
	RCollectionManager::~RCollectionManager()

	Destructor for RCollectionManager class
*****************************************************************************/

RCollectionManager::~RCollectionManager()
{
	delete m_pCDDriveBuf;
	CloseAllCollections();
	PurgeRefPathArray();
}

/****************************************************************************
	RCollectionArray& RCollectionManager::OpenCollectionsOfType(
		CollectionType				aCollectionType
		OpenCollectionsStatus*	aStatus )

  Locates and opens all aCollectionType collection files in:
    1. The "Collections" directory in the application directory.
	 2. The registry "Collections Location" directory for this product.
	 3. The registry "Shared Collections" directories for PS family products.
    4. All local CD ROM drives in the "Collexns" directory.
	 5. The registry "Install From Root" drive.

	If aCollectionType files are already open, returns same RCollectionArray.
	Call CloseCollectionsOfType when done with files in RCollectionArray.

	If the aStatus parameter is supplied, the status of the call is
	returned as:

		kOpenCollectionsSuccess
			opened one or more collections

		kOpenCollectionsFailure
			user clicked "Cancel", no collections found or other error

		kOpenCollectionsOtherGraphics
			user clicked "Other Graphics"

	If aStatus is not kOpenCollectionsSuccess, the RCollectionArray returned
	will be empty.
*****************************************************************************/

RCollectionArray& RCollectionManager::OpenCollectionsOfType(
	CollectionType				aCollectionType,
	OpenCollectionsStatus*	aStatus /* = NULL */ )
{
	RCollectionArray* theCollectionArray = NULL;
	if (aStatus)
		*aStatus = kOpenCollectionsSuccess;

	try
	{
		ASSERT( 0 <= aCollectionType && aCollectionType < kNumberOfCollectionTypes );

		// if aCollectionType files already open, return existing RCollectionArray
		// otherwise create new one
		if (openCollectionsOfTypeArray[aCollectionType])
			return *(openCollectionsOfTypeArray[aCollectionType]->openCollections);
		else
		{
			openCollectionsOfTypeArray[aCollectionType] = new ROpenCollectionsOfType;
			openCollectionsOfTypeArray[aCollectionType]->openCollections = new RCollectionArray;
			openCollectionsOfTypeArray[aCollectionType]->openCollectionAttributes = new RCollectionAttributesArray;
		}
		theCollectionArray = openCollectionsOfTypeArray[aCollectionType]->openCollections;
		RCollectionAttributesArray* theCollectionAttributesArray = openCollectionsOfTypeArray[aCollectionType]->openCollectionAttributes;

		m_collectionIDArray.Empty();					// clear the ID array

		if ( aCollectionType == kGraphicsCollectionType )
			PurgeRefPathArray();						// and the ref path array

		char collectionDir[ _MAX_DIR ];

#if 0 // REVIEW GCB 5/14/97 - should put these strings in resources
		int loadStringResult = ::LoadString( ::AfxGetInstanceHandle(),
										COLLECTION_DIR,	collectionDir, _MAX_DIR );
		ASSERT( loadStringResult );

		loadStringResult = ::LoadString( ::AfxGetInstanceHandle(),
								CD_COLLECTION_DIR, m_CDCollectionDir, _MAX_DIR );
		ASSERT( loadStringResult );
#else
		::strcpy( collectionDir, "Collections\\" );
		::strcpy( m_CDCollectionDir, "Collexns\\" );
#endif

		char wildcardPath[ _MAX_PATH ];
		char wildcardName[ _MAX_FNAME ];

		::strcpy( wildcardName, "*" );
		::strcat( wildcardName, extensions[aCollectionType] );

		BOOLEAN applicationDirectoryCollectionsFound = FALSE;
		BOOLEAN collectionsLocationCollectionsFound = FALSE;
		BOOLEAN sharedLocationCollectionsFound = FALSE;
		BOOLEAN localCDCollectionsFound = FALSE;
		BOOLEAN installFromCollectionsFound = FALSE;

		// scan for collection files in the application directory
		::strcpy( wildcardPath, dirPath );
		::strcat( wildcardPath, collectionDir );
		ScanCollectionFiles( theCollectionArray, theCollectionAttributesArray, wildcardPath, wildcardName, FALSE, NULL, applicationDirectoryCollectionsFound );

		typedef	RArray<RMBCString*>							RMBCStringArray;
		typedef	RMBCStringArray::YIterator					RMBCStringArrayIterator;

		RMBCStringArray pathArray;

		RMBCString								szResultsKey;
		RRenaissanceConfiguration			renaissanceConfig;
		RPrintShopFamilyConfiguration		familyConfig;

		// scan for collection files in the "Collections Location" from the registry
		if (renaissanceConfig.GetStringValue( RRenaissanceConfiguration::kCollectionsLocation, szResultsKey ))
		{
			::strcpy( wildcardPath, (const char*)szResultsKey );
			::strcat( wildcardPath, "\\" );
			pathArray.InsertAtEnd( new RMBCString( wildcardPath ) );
			ScanCollectionFiles( theCollectionArray, theCollectionAttributesArray, wildcardPath, wildcardName, FALSE, NULL, collectionsLocationCollectionsFound );
		}
		
		// scan for collection files in the "Shared Collections Path" from the registry
		if (familyConfig.GetStringValue( RPrintShopFamilyConfiguration::kSharedCollectionsPath, szResultsKey ))
		{
			// for each semicolon-separated path in szResultsKey...
			char tmpSzResultsKey[ _MAX_PATH ];
			::strcpy( tmpSzResultsKey, (const char*)szResultsKey );
			for ( char* p = tmpSzResultsKey; p; )
			{
				// extract the next path from szResultsKey
				::strcpy( wildcardPath, p );
				p = ::strchr( p, ';' );
				if ( p )
				{
					char* pWildcardPath = ::strchr( wildcardPath, ';' );
					*pWildcardPath = NULL;
					p++;
				}
				::strcat( wildcardPath, "\\" );

				// if this path has not already been scanned, add it to pathArray and
				// scan for collection files in the path
				RMBCStringArrayIterator c = pathArray.Start();
				BOOL alreadyScanned = FALSE;
				RMBCString* path = new RMBCString( wildcardPath );
				for (; c != pathArray.End() && !alreadyScanned; ++c)
					if (*path == **c)
						alreadyScanned = TRUE;
				if (alreadyScanned)
					delete path;
				else
				{
					pathArray.InsertAtEnd( path );
					ScanCollectionFiles( theCollectionArray, theCollectionAttributesArray, wildcardPath, wildcardName, FALSE, NULL, sharedLocationCollectionsFound );
				}
			}
		}

		for (RMBCStringArrayIterator c = pathArray.Start(); c != pathArray.End(); ++c)
			delete *c;
			
		char installFromDirectory[_MAX_PATH];
		installFromDirectory[0] = '\0';
		BOOL needToSearchInstallFromDirectory = FALSE;
		if (renaissanceConfig.GetStringValue( RRenaissanceConfiguration::kInstallFromRoot, szResultsKey ))
		{
			::strcpy( installFromDirectory, (const char*)szResultsKey );
			needToSearchInstallFromDirectory = TRUE;
		}

		int nOK = IDOK;
		YCounter yFound = 0;

		while (nOK == IDOK && yFound <= 0)
		{
			char cdDriveRoot[_MAX_PATH];

			// scan for collection files on local CD ROM drives
			for ( int i = 0; i < m_nCDDrives; i++ )
			{
				if ( m_pCDDriveBuf[i] == '\0' )
					break;									  // shouldn't happen
				if (m_pCDDriveBuf[i] == installFromDirectory[0])
					needToSearchInstallFromDirectory = FALSE;
				wildcardPath[0] = m_pCDDriveBuf[i];
				::strcpy( wildcardPath+1, ":\\" );
				::strcpy( cdDriveRoot, wildcardPath );
				::strcat( wildcardPath, m_CDCollectionDir );
				ScanCollectionFiles( theCollectionArray, theCollectionAttributesArray, wildcardPath, wildcardName, TRUE, cdDriveRoot, localCDCollectionsFound );
			}

			// scan for collection files on install from drive
			// (might be a network CD ROM drive)
			if (needToSearchInstallFromDirectory)
			{
				::strcpy( wildcardPath, installFromDirectory );
				::strcpy( cdDriveRoot, wildcardPath );
				::strcat( wildcardPath, m_CDCollectionDir );
				ScanCollectionFiles( theCollectionArray, theCollectionAttributesArray, wildcardPath, wildcardName, TRUE, cdDriveRoot, installFromCollectionsFound );
			}

			yFound = theCollectionArray->Count();

#ifndef TPSDEBUG
			// if this is the release version and we only found collections in
			// the application directory, ask for the CD
			if (!collectionsLocationCollectionsFound && !sharedLocationCollectionsFound
				&& !localCDCollectionsFound && !installFromCollectionsFound)
				yFound = 0;
#endif

			// if no collections found, then prompt for CD
			if (yFound <= 0)
			{
				// REVIEW GCB 6/5/98
				// Workaround for bug that presented alert with Other Graphics
				// button going into custom graphic browsers (Init Cap, etc.)
				// and edit calendar cell dialogs:
				// Only present alert with Other Graphics button if caller
				// passes aStatus (currently only the graphics browser does).
				if (aStatus && aCollectionType == kGraphicsCollectionType)
				{
					YGBCDSwapAlertValues cdSwapStatus;
					cdSwapStatus = RAlert().AskUserForGraphicBrowserCdSwap();
					switch (cdSwapStatus)
					{
					case kGBAlertCancel:
						nOK = IDCANCEL;
						if (aStatus)
							*aStatus = kOpenCollectionsFailure;
						break;
					case kGBOtherGraphics:
						nOK = IDCANCEL;
						if (aStatus)
							*aStatus = kOpenCollectionsOtherGraphics;
						break;
					}
				}
				else
				{
					if (RAlert().AskUserForCdSwap() == kAlertCancel)
					{
						nOK = IDCANCEL;
						if (aStatus)
							*aStatus = kOpenCollectionsFailure;
					}
				}
				if (nOK == IDCANCEL)
				{
					CloseCollectionsOfType( aCollectionType );
					theCollectionArray = NULL;
				}
			}
		}

		RCollectionCDLockManager::TheCollectionCDLockManager().LockDrive( AnyOpenCollectionsOnCD() );
	}
	catch( YException exception )
	{
		::ReportException( exception );
		theCollectionArray = NULL;
		if (aStatus)
			*aStatus = kOpenCollectionsFailure;
	}

	if (theCollectionArray == NULL)
		theCollectionArray = &m_emptyCollectionArray;
	return *theCollectionArray;
}

/****************************************************************************
	void RCollectionManager::ScanCollectionFiles(
		RCollectionArray*					theCollectionArray,
		RCollectionAttributesArray*	theCollectionAttributesArray,
		char*									wildcardPath,
		char*									wildcardName,
		BOOLEAN								isCD,
		char*									cdDriveRoot.
		BOOLEAN&								collectionsFound )

	Given a path and wildcard filename, seeks out all matching collection files
	in that location, opens them, culls image data and appends the collection
	array.
	Checks to insure that there are no duplicates.
*****************************************************************************/

void RCollectionManager::ScanCollectionFiles(
	RCollectionArray*					theCollectionArray,
	RCollectionAttributesArray*	theCollectionAttributesArray,
	char*									wildcardPath,
	char*									wildcardName,
	BOOLEAN								isCD,
	char*									cdDriveRoot,
	BOOLEAN&								collectionsFound )
{
	BOOLEAN retVal = FALSE;

	char checkPath[ _MAX_PATH ];			// construct the wildcard for search
	::strcpy( checkPath, wildcardPath );
	::strcat( checkPath, wildcardName );

	WIN32_FIND_DATA findFileData;
	HANDLE findFileHandle = ::FindFirstFile( checkPath, &findFileData );
	if (findFileHandle != INVALID_HANDLE_VALUE)
	{
		// for each file matching checkPath...
		do
		{
			// if the file is not a directory...
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char fullPath[ _MAX_PATH ];			// construct complete path
				::strcpy( fullPath, wildcardPath );
				::strcat( fullPath, findFileData.cFileName );

				RChunkStorage* aChunkStorage = NULL;

				try
				{
					// open the file as an RChunkStorage
					// throws exception if file is not chunky
					// or couldn't be opened in the specified mode
 					EAccessMode accessMode =
						(isCD ||
						(findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)) ?
						kRead : kReadWrite;
					aChunkStorage = new RChunkStorage( fullPath, accessMode, TRUE );
					aChunkStorage->PreserveFileDate( );
					BOOLEAN keepOpen = FALSE;

					// seek to the CLHD collection header chunk
					RChunkStorage::RChunkSearcher yIter = aChunkStorage->Start( 'CLHD', FALSE );
					if (yIter.End())
					{
						// no CLHD chunk: chunky, but not a collection file
						delete aChunkStorage;
						RAlert alert;
						alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
					}
					else
					{
						// read in the collection file header
						CollectionHeader header;
						*aChunkStorage >> header.yCollectionID;
						*aChunkStorage >> header.yLangLocale;
						*aChunkStorage >> header.yCurFormatVersion;
						*aChunkStorage >> header.yMinFormatVersion;
						*aChunkStorage >> header.yCharEncoding;
						*aChunkStorage >> header.yCollectionType;

						unsigned long collectionID = header.yCollectionID;
						short minFormatVersion = header.yMinFormatVersion;

						// check min format version
						if (minFormatVersion <= kExpectedMinFormatVersion &&
							minFormatVersion >= kFormatVersion6)
						{
							collectionsFound = TRUE;

							// avoid duplicates
							if (IsNewCollection( collectionID ))
							{
								keepOpen = TRUE;

								// add RChunkStorage to theCollectionArray
								theCollectionArray->InsertAtEnd( aChunkStorage );

								// add theCollectionAttributes to theCollectionAttributesArray
								RCollectionAttributes* collectionAttributes = new RCollectionAttributes;
								theCollectionAttributesArray->InsertAtEnd( collectionAttributes );

								// remember whether the open collection file is on
								// the product CD on a local drive
								// (for locking/unlocking the drive)
								if (isCD)
									collectionAttributes->cdDriveLetter = cdDriveRoot[0];

								// if collection is by-reference
								if (header.yCollectionType == utGraphicWithRef ||
									header.yCollectionType == utBackdropWithRef)
								{
									// resolve the reference path
									UpdateImageByReferenceList(
										aChunkStorage,
										wildcardPath,
										collectionID,
										isCD,
										cdDriveRoot );
								}
							}
						}
					}
					if (!keepOpen)
						delete aChunkStorage;
				}
				catch (...)
				{
					delete aChunkStorage;
				}
			}
		} while (::FindNextFile( findFileHandle, &findFileData ));
		BOOLEAN findCloseResult = ::FindClose( findFileHandle );
		ASSERT( findCloseResult );
	}
}

/****************************************************************************
	BOOLEAN RCollectionManager::IsCollectionTypeOpen( CollectionType aCollectionType )

	Return TRUE if aCollectionType collection files are open, else FALSE
*****************************************************************************/

BOOLEAN RCollectionManager::IsCollectionTypeOpen( CollectionType aCollectionType )
{
	ASSERT( 0 <= aCollectionType && aCollectionType < kNumberOfCollectionTypes );

	if (openCollectionsOfTypeArray[aCollectionType])
		return TRUE;
	else
		return FALSE;
}

/****************************************************************************
	void RCollectionManager::CloseCollectionsOfType( CollectionType aCollectionType )

	Close all aCollectionType collection files.
	RCollectionArray returned from OpenCollectionsOfType becomes invalid.
	Does nothing if aCollectionType collection files are not open.
*****************************************************************************/

void RCollectionManager::CloseCollectionsOfType( CollectionType aCollectionType )
{
	ASSERT( 0 <= aCollectionType && aCollectionType < kNumberOfCollectionTypes );

	if (openCollectionsOfTypeArray[aCollectionType])
	{
		// delete each element in
		// openCollectionsOfTypeArray[aCollectionType]->openCollections
		// (which closes all collection files in the array)
		// and delete the array itself
		RCollectionArray* collectionArray =
			openCollectionsOfTypeArray[aCollectionType]->openCollections;
		RCollectionArrayIterator collectionIter( collectionArray->Start() );
		RCollectionArrayIterator collectionIterEnd( collectionArray->End() );
		for (; collectionIter != collectionIterEnd; collectionIter++)
			delete *collectionIter;
		delete collectionArray;

		// delete each element in
		// openCollectionsOfTypeArray[aCollectionType]->openCollectionAttributes
		// and delete the array itself
		RCollectionAttributesArray* collectionAttributesArray =
			openCollectionsOfTypeArray[aCollectionType]->openCollectionAttributes;
		RCollectionAttributesArrayIterator attributesIter( collectionAttributesArray->Start() );
		RCollectionAttributesArrayIterator attributesIterEnd( collectionAttributesArray->End() );
		for (; attributesIter != attributesIterEnd; attributesIter++)
			delete *attributesIter;
		delete collectionAttributesArray;

		// delete and null out openCollectionsOfTypeArray[aCollectionType]
		delete openCollectionsOfTypeArray[aCollectionType];
		openCollectionsOfTypeArray[aCollectionType] = NULL;

		if (AnyOpenCollectionsOnCD() == '\0')
			RCollectionCDLockManager::TheCollectionCDLockManager().UnlockAllDrives();
	}
}

/****************************************************************************
	void RCollectionManager::CloseAllCollections()

	Close all open collection files.
	All RCollectionArray's returned from OpenCollectionsOfType become invalid.
	Does nothing if no collection files are open.
*****************************************************************************/

void RCollectionManager::CloseAllCollections()
{
	for (int i = 0; i < kNumberOfCollectionTypes; ++i)
		CloseCollectionsOfType( static_cast<CollectionType>(i) );
}

/****************************************************************************
	void RCollectionManager::UpdateImageByReferenceList(
													RChunkStorage* aChunkStorage,
													char *wildcardName,
													unsigned long id,
													BOOLEAN isCD,
													char* cdDriveRoot )

	If aChunkStorage is NULL, resolve location of the photos refered to in a
	save-by-ref QSL; inserts the location into a single item in the list used
	for later lookups from GetImageByReferencePathPrefix().

	Otherwise if aChunkStorage is not NULL, resolve location of the photos
	refered to by the image by reference collection specified by aChunkStorage;
	adds the location to a list for later lookups from
	GetImageByReferencePathPrefix().
*****************************************************************************/

void RCollectionManager::UpdateImageByReferenceList(
													RChunkStorage* aChunkStorage,
													char * /* wildcardName */,
													unsigned long /* id */,
													BOOLEAN isCD,
													char* cdDriveRoot )
{
	if (!aChunkStorage)
		PurgeRefPathArray();

	BOOL found = FALSE;
	RMBCString s;
	char refPath[ _MAX_PATH ];

	if ( aChunkStorage && isCD )	// if we found the collection on CD
	{										// then assume the photos are there, too
		::strcpy( refPath, cdDriveRoot );
		::strcat( refPath, m_CDReferenceDir );

		s = refPath;
		found = TRUE;
	}
	else								// otherwise, we started on a hard drive
	{
		char temp[_MAX_PATH];
		_getcwd( temp, _MAX_PATH );
  
		// see if we have a photos directory in the application directory
		::strcpy( refPath, dirPath );
		::strcat( refPath, m_CDReferenceDir );

		if ( _chdir( refPath ) == 0 )				// ok, that's it
		{
			s = refPath;
			_chdir( temp );							// restore cwd
			found = TRUE;
		}				

		if ( !found )								// look now on CDs
		{
			RMBCString szResultsKey;
			char installFromDirectory[_MAX_PATH];
			installFromDirectory[0] = '\0';
			BOOL needToSearchInstallFromDirectory = FALSE;
			RRenaissanceConfiguration config;
			if (config.GetStringValue( RRenaissanceConfiguration::kInstallFromRoot, szResultsKey ))
			{
				::strcpy( installFromDirectory, (const char*)szResultsKey );
				needToSearchInstallFromDirectory = TRUE;
			}

			for ( int i = 0; i < m_nCDDrives; i++ )
			{
				if ( m_pCDDriveBuf[i] == '\0' )
				{
					break;
				}

				if (m_pCDDriveBuf[i] == installFromDirectory[0])
					needToSearchInstallFromDirectory = FALSE;
				refPath[0] = m_pCDDriveBuf[i];
				::strcpy( refPath+1, ":\\" );

				::strcat( refPath, m_CDReferenceDir );
				if ( _chdir( refPath ) == 0 )				// ok, that's it
				{
					s = refPath;
					_chdir( temp );							// restore cwd
					found = TRUE;
				}				
			}

			// look for matching collection file on install from drive
			// (might be a network CD ROM drive)
			if (needToSearchInstallFromDirectory)
			{
				::strcpy( refPath, installFromDirectory );
				::strcat( refPath, m_CDReferenceDir );
				if ( _chdir( refPath ) == 0 )				// ok, that's it
				{
					s = refPath;
					_chdir( temp );							// restore cwd
					found = TRUE;
				}				
			}
		}
	}

	if ( found )					// we found the photos directory somewhere
	{
		RImageRefPath *rp = new RImageRefPath;		// new ref table entry
		ASSERT ( rp );

		rp->storage = aChunkStorage;				// maps chunky storage ptr
		rp->path = s;								// to the photos path

		m_refPathArray.InsertAtEnd( rp );
	}
}	

/****************************************************************************
	BOOLEAN RCollectionManager::FindMatchingContentFile( unsigned long id,
													char drive,
													char *wildcardName )

	Searches the files in a CD collection subdirectory for a matching
	image by reference collection; the assumption is that if a match is found,
	the refered-to content will also exist on the same CD
*****************************************************************************/

BOOLEAN RCollectionManager::FindMatchingContentFile( unsigned long id,
													char* wildcardPath,
													char *wildcardName )
{
	BOOLEAN matchFound = FALSE;

	char checkPath[ _MAX_PATH ];				// construct full wildcard name
	::strcpy( checkPath, wildcardPath );
	::strcat( checkPath, m_CDCollectionDir );
	::strcat( checkPath, wildcardName );

	WIN32_FIND_DATA findFileData;
	HANDLE findFileHandle = ::FindFirstFile( checkPath, &findFileData );
	if ( findFileHandle != INVALID_HANDLE_VALUE )
	{
		do							// for each file matching checkPath...
		{
			// if the file is not a directory...
			if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char fullPath[ _MAX_PATH ];
				::strcpy( fullPath, wildcardPath );
				::strcat( fullPath, m_CDCollectionDir );
				::strcat( fullPath, findFileData.cFileName );

				RChunkStorage* aChunkStorage = NULL;

				try
				{
					// open the file as an RChunkStorage
					// throws exception if file is not chunky
					RChunkStorage* aChunkStorage = new RChunkStorage( fullPath, kRead );

					// seek to the CLHD collection header chunk
					RChunkStorage::RChunkSearcher yIter = aChunkStorage->Start( 'CLHD', FALSE );
					if (yIter.End())
					{
						// no CLHD chunk: chunky, but not a collection file
						delete aChunkStorage;
						RAlert alert;
						alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
					}
					else
					{
						// read in the collection file header
						CollectionHeader header;
						*aChunkStorage >> header.yCollectionID;
						*aChunkStorage >> header.yLangLocale;
						*aChunkStorage >> header.yCurFormatVersion;
						*aChunkStorage >> header.yMinFormatVersion;

						unsigned long collectionID = header.yCollectionID;
						short minFormatVersion = header.yMinFormatVersion;

						delete aChunkStorage;

						// check min format version and id
						if ( minFormatVersion <= kExpectedMinFormatVersion &&
							collectionID == id )
						{
							matchFound = TRUE;
							break;
						}
					}
				}
				catch (...)
				{
					delete aChunkStorage;
				}
			}
		} while (::FindNextFile( findFileHandle, &findFileData ));

		BOOLEAN findCloseResult = ::FindClose( findFileHandle );
		ASSERT( findCloseResult );
	}

	return matchFound;									// return the result
}

/****************************************************************************
	int	RCollectionManager::GetCDROMDrives( char *szBuff )

	Fills string buffer with system's CD ROM drive letters.
	Returns count of CD drives found.
*****************************************************************************/

int RCollectionManager::GetCDROMDrives( char *szBuff, int nMaxCDs )
{
	int  nDrives = 0;
	char szDrives[202];
	char   szRoot[5];

	// obtain drive letters for all drives in system (50 max)
	DWORD nSize = ::GetLogicalDriveStrings( 200, szDrives );
	
	if ( nSize > 0 )
	{
		DWORD i = 0;
		char *srcptr = szDrives;
		
		// examine all drives
		while ( i < nSize )
		{
			// get next drive
			char *dstptr = szRoot;
			while ( *srcptr != '\0' )
			{
				*dstptr++ = *srcptr++;
				i++;
			}
			*dstptr = *srcptr++;
			i++;

			if ( *szRoot != '\0' )
			{
				// determine drive type
				UINT nType = ::GetDriveType(szRoot);

				if ( nType == DRIVE_CDROM )
				{
					// add drive letter to output buffer
					if ( nDrives < nMaxCDs )
					{
						*szBuff++ = *szRoot;
					}
					nDrives++;
				}
			}
		}
	}
	*szBuff = '\0';									// terminate buffer

	return nDrives;
}

/****************************************************************************
	BOOLEAN RCollectionManager::IsNewCollection( long id )

	Maintains array of collection IDs already loaded to avoid duplicates.
*****************************************************************************/

BOOLEAN RCollectionManager::IsNewCollection( unsigned long id )
{
	BOOLEAN isNew = TRUE;

	CollectionIDArrayIterator iterator = m_collectionIDArray.Start();
	for ( ; iterator != m_collectionIDArray.End(); ++iterator )
	{
		if ( *iterator == id )			  // this collection already loaded
		{
			isNew = FALSE;
			break;
		}
	}

	if ( isNew )
	{
		m_collectionIDArray.InsertAtEnd( id );
	}

	return isNew;
}	

/****************************************************************************
	void RCollectionManager::PurgeRefPathArray()

	Empties the image by reference lookup table.
*****************************************************************************/

void RCollectionManager::PurgeRefPathArray()
{
	RImageRefPathArrayIterator iter( m_refPathArray.Start() );
	for ( ; iter != m_refPathArray.End(); iter++)
	{
		delete *iter;
	}
	
	m_refPathArray.Empty();
}

/****************************************************************************
	BOOLEAN RCollectionManager::GetImageByReferencePathPrefix(
									RChunkStorage* aStorage, RMBCString &aPath )

	This is the public method used to fetch the path prefix for the location
	of referred-to images.
*****************************************************************************/

BOOLEAN RCollectionManager::GetImageByReferencePathPrefix( RChunkStorage* aStorage, RMBCString &aPath )
{
	BOOL found = FALSE;

	RImageRefPathArrayIterator iter( m_refPathArray.Start() );
	if (iter != m_refPathArray.End())
	{
		if (!aStorage)
		{
			// return the first (and only) partial path prefix in m_refPathArray
			// for resolving a QSL photo save-by-ref partial path
			aPath = (*iter)->path;
			found = TRUE;
		}
		else
		{
			// look for matching aStorage pointer and return corresponding
			// partial path prefix for resolving collection file image-by-ref
			for ( ; iter != m_refPathArray.End(); iter++)
			{
				if ( (*iter)->storage == aStorage )
				{
					aPath = (*iter)->path;
					found = TRUE;
					break;
				}
			}
		}
	}
	
//	TpsAssert( found, "Couldn't find image by reference path prefix" );
	return found;
}

/****************************************************************************
	void RCollectionManager::UpdateImageByReferencePathPrefix(
									RChunkStorage* aStorage )

	If aStorage is NULL, attempt to resolve location of files referenced by
	a QSL photo save-by-ref image and insert a single entry into the internal
	list for access from GetImageByReferencePathPrefix(); useful for locating
	the files on a newly inserted CD referenced by a QSL photo save-by-ref
	image.

	Otherwise if aStorage is not NULL, attempt to resolve location of files
	referenced by the image by reference collection specified by aStorage and
	add the location to internal list for access from
	GetImageByReferencePathPrefix(); useful for locating the files on a newly
	inserted CD referenced by an image by reference collection.
*****************************************************************************/

void RCollectionManager::UpdateImageByReferencePathPrefix(
	RChunkStorage* aChunkStorage )
{
	if (!aChunkStorage)
		UpdateImageByReferenceList( NULL, NULL, 0L, FALSE, NULL );
	else
	{
		// save position in chunky storage to restore later
		YChunkStorageId currentStorageId = aChunkStorage->GetSelectedChunk();
		YStreamLength chunkyPos	= aChunkStorage->GetPosition();
		RStorage* storage = aChunkStorage->GetDataParent();
		YStreamLength pos	= storage->GetPosition();

		// seek to the CLHD collection header chunk
		aChunkStorage->SelectRootChunk();
		RChunkStorage::RChunkSearcher yIter = aChunkStorage->Start( 'CLHD', FALSE );
		if (yIter.End())
		{
			RAlert alert;
			alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
		}
		else
		{
			// read in the collection file header
			CollectionHeader header;
			*aChunkStorage >> header.yCollectionID;
			*aChunkStorage >> header.yLangLocale;
			*aChunkStorage >> header.yCurFormatVersion;
			*aChunkStorage >> header.yMinFormatVersion;
			*aChunkStorage >> header.yCharEncoding;
			*aChunkStorage >> header.yCollectionType;

			unsigned long collectionID = header.yCollectionID;
			short collectionType = header.yCollectionType;
			short minFormatVersion = header.yMinFormatVersion;

			// check min format version and whether this is a by-reference collection
			if (minFormatVersion <= kExpectedMinFormatVersion &&
				(collectionType == utGraphicWithRef ||
				collectionType == utBackdropWithRef))
			{
				// resolve the reference path
				char wildcardName[ _MAX_FNAME ];
				::strcpy( wildcardName, "*" );
				::strcat( wildcardName, extensions[kGraphicsCollectionType] );

				UpdateImageByReferenceList( aChunkStorage, wildcardName, collectionID, FALSE, NULL );
			}
		}

		// restore position in chunky storage
		aChunkStorage->SetSelectedChunk( currentStorageId );
		aChunkStorage->SeekAbsolute( chunkyPos );
		storage->SeekAbsolute( pos );
	}
}

/****************************************************************************
	char RCollectionManager::AnyOpenCollectionsOnCD()

	If any open collection files are on the Product CD, returns the drive
	letter of the CD-ROM drive, else returns zero.
*****************************************************************************/

char RCollectionManager::AnyOpenCollectionsOnCD()
{
	for (int i = 0; i < kNumberOfCollectionTypes; ++i)
	{
		if (openCollectionsOfTypeArray[i])
		{
			RCollectionAttributesArray* collectionAttributesArray =
				openCollectionsOfTypeArray[i]->openCollectionAttributes;
			RCollectionAttributesArrayIterator attributesIter( collectionAttributesArray->Start() );
			RCollectionAttributesArrayIterator attributesIterEnd( collectionAttributesArray->End() );
			for (; attributesIter != attributesIterEnd; attributesIter++)
				if ((*attributesIter)->cdDriveLetter)
					return (*attributesIter)->cdDriveLetter;
		}
	}
	return '\0';
}


/////////////////////////////////////////////////////////////////////////////


#define CARRY_FLAG              0x1
#define VWIN32_DIOC_DOS_IOCTL   1

#pragma pack(1)
typedef struct _DIOC_REGISTERS
{
    DWORD reg_EBX;
    DWORD reg_EDX;
    DWORD reg_ECX;
    DWORD reg_EAX;
    DWORD reg_EDI;
    DWORD reg_ESI;
    DWORD reg_Flags;
} DIOC_REGISTERS, *PDIOC_REGISTERS;
#pragma pack()

#define LOCK_MEDIA   0
#define UNLOCK_MEDIA 1
#define STATUS_LOCK  2

#pragma pack(1)
typedef struct _PARAMBLOCK
{
   BYTE Operation;
   BYTE NumLocks;
} PARAMBLOCK, *PPARAMBLOCK;
#pragma pack()

static PARAMBLOCK     pb      = {0, 0};

/****************************************************************************
	RCollectionCDLockManager& RCollectionCDLockManager::TheCollectionCDLockManager()

	Return reference to the single instance of a RCollectionCDLockManager
*****************************************************************************/

RCollectionCDLockManager& RCollectionCDLockManager::TheCollectionCDLockManager()
{
	static RCollectionCDLockManager theCollectionCDLockManager;
	return theCollectionCDLockManager;
}

/****************************************************************************
	void RCollectionCDLockManager::LockDrive( char aDriveLetter )

	Lock the drive specified by aDriveLetter
*****************************************************************************/

#pragma warning(disable:4100)

void RCollectionCDLockManager::LockDrive( char aDriveLetter )
{
	//	check if a PowerUser decided to disable CD drive locking	
	RMBCString rValue;
	RPowerUserConfiguration rConfig;
	if (rConfig.GetStringValue(RPowerUserConfiguration::kUnlockCD, rValue))
		return;

	// if we have a valid drive letter and we haven't already locked the drive...
	if (aDriveLetter != '\0' && pb.NumLocks == 0)
	{
		driveLetter = ::tolower( aDriveLetter );

		HANDLE driveHandle = INVALID_HANDLE_VALUE;
		BOOL deviceIoControlRetVal = TRUE;

		OSVERSIONINFO verInfo;
		verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		BOOL getVersionExResult = GetVersionEx( &verInfo );
		if (!getVersionExResult || verInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
		{
			// Win95

			DIOC_REGISTERS regs    = {0, 0, 0, 0, 0, 0, 0};
			DWORD          cb;

			pb.Operation = LOCK_MEDIA;
			regs.reg_EAX = 0x440D;
			regs.reg_EBX = driveLetter - 'a' + 1;
			regs.reg_ECX = 0x0848; // Lock/unlock media
			regs.reg_EDX = (DWORD)&pb;

			driveHandle = ::CreateFile( "\\\\.\\vwin32", 0, 0, NULL, OPEN_ALWAYS,
				FILE_FLAG_DELETE_ON_CLOSE, NULL );

//			TpsAssert( driveHandle != INVALID_HANDLE_VALUE, "Couldn't open CD-ROM drive for locking" );
			if (driveHandle != INVALID_HANDLE_VALUE)
			{
				BOOL deviceIoControlRetVal = ::DeviceIoControl( driveHandle,
					VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0 );
				// pb.NumLocks is incremented by DeviceIoControl call above

				if (deviceIoControlRetVal)
					deviceIoControlRetVal = !(regs.reg_Flags & CARRY_FLAG);
			}
		}
		else
		{
			// WinNT

			char driveName[30];
			::sprintf( driveName, "\\\\.\\%c:", driveLetter );

			driveHandle = ::CreateFile( driveName, GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );

//			TpsAssert( driveHandle != INVALID_HANDLE_VALUE, "Couldn't open CD-ROM drive for locking" );
			if (driveHandle != INVALID_HANDLE_VALUE)
			{
				PREVENT_MEDIA_REMOVAL pmr;
				pmr.PreventMediaRemoval = TRUE;
				DWORD bytesReturned;

				deviceIoControlRetVal = ::DeviceIoControl( driveHandle,
					IOCTL_DISK_MEDIA_REMOVAL, &pmr, sizeof(PREVENT_MEDIA_REMOVAL), NULL,
					0, &bytesReturned, NULL );

				pb.NumLocks = 1;
			}
		}

//		TpsAssert( deviceIoControlRetVal, "Couldn't lock CD-ROM drive" );

		if (driveHandle != INVALID_HANDLE_VALUE)
			::CloseHandle( driveHandle );
	}
}

/****************************************************************************
	void RCollectionCDLockManager::UnlockDrive()

	Unlock the drive locked by a prior call to LockDrive
*****************************************************************************/

void RCollectionCDLockManager::UnlockDrive()
{
	if (driveLetter != '\0')
		UnlockDriveInternal( driveLetter );
}

/****************************************************************************
	void RCollectionCDLockManager::UnlockDriveInternal( char aDriveLetter )

	Unlock the drive specified by aDriveLetter
*****************************************************************************/

#pragma warning(disable:4100)

void RCollectionCDLockManager::UnlockDriveInternal( char aDriveLetter )
{
	aDriveLetter = ::tolower( aDriveLetter );

	HANDLE driveHandle = INVALID_HANDLE_VALUE;
	BOOL deviceIoControlRetVal = TRUE;

	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	BOOL getVersionExResult = GetVersionEx( &verInfo );
	if (!getVersionExResult || verInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		// Win95

		DIOC_REGISTERS regs    = {0, 0, 0, 0, 0, 0, 0};
		DWORD          cb;

		pb.Operation = UNLOCK_MEDIA;
		regs.reg_EAX = 0x440D;
		regs.reg_EBX = aDriveLetter - 'a' + 1;
		regs.reg_ECX = 0x0848;	// Lock/unlock media
		regs.reg_EDX = (DWORD)&pb;

		driveHandle = ::CreateFile( "\\\\.\\vwin32", 0, 0, NULL, OPEN_ALWAYS,
			FILE_FLAG_DELETE_ON_CLOSE, NULL );

		if (driveHandle != INVALID_HANDLE_VALUE)
		{
			BOOL deviceIoControlRetVal = ::DeviceIoControl( driveHandle,
				VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0 );
			// pb.NumLocks is decremented by DeviceIoControl call above

			if (deviceIoControlRetVal)
				deviceIoControlRetVal = !(regs.reg_Flags & CARRY_FLAG);
		}
	}
	else
	{
		// WinNT

		PREVENT_MEDIA_REMOVAL pmr;
		pmr.PreventMediaRemoval = FALSE;
		DWORD bytesReturned;

		char driveName[30];
		::sprintf( driveName, "\\\\.\\%c:", aDriveLetter );

		driveHandle = ::CreateFile( driveName, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );

		if (driveHandle != INVALID_HANDLE_VALUE)
		{
			deviceIoControlRetVal = ::DeviceIoControl( driveHandle,
				IOCTL_DISK_MEDIA_REMOVAL, &pmr, sizeof(PREVENT_MEDIA_REMOVAL), NULL,
				0, &bytesReturned, NULL );

			pb.NumLocks = 0;
		}
	}

	TpsAssert( driveHandle != INVALID_HANDLE_VALUE, "Couldn't open CD-ROM drive for unlocking" );
	TpsAssert( deviceIoControlRetVal, "Couldn't unlock CD-ROM drive" );

	if (driveHandle != INVALID_HANDLE_VALUE)
		::CloseHandle( driveHandle );
}

/****************************************************************************
	void RCollectionCDLockManager::UnlockAllDrives()

	Unlock all CD drives
*****************************************************************************/

void RCollectionCDLockManager::UnlockAllDrives()
{
	const int nMaxCDs = 20;						   // a more than reasonable cap
	char cDDriveBuf[nMaxCDs + 1];
	int nCDDrives = RCollectionManager::TheCollectionManager().GetCDROMDrives( cDDriveBuf, nMaxCDs );
	for (int i = 0; i < nCDDrives; ++i)
	{
		if (cDDriveBuf[i] == '\0')
			break;									  // shouldn't happen
		UnlockDriveInternal( cDDriveBuf[i] );
	}
}

