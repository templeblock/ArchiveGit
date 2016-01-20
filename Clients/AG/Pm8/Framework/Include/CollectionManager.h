// ****************************************************************************
//
//  File Name:			CollectionManager.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of RCollectionManager class that locates,
//							opens and closes online Renaissance collection files
//
//  Portability:		Win32
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _CollectionManager_H_
#define _CollectionManager_H_

#include "ChunkyStorage.h"			// RChunkStorage class
#include "Array.h"					// RArray template class

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

enum CollectionType
{
	kGraphicsCollectionType = 0,
	kBackdropsCollectionType,
	kLayoutsCollectionType,
	kReadyMadesCollectionType,
	kLinesCollectionType,
	kBordersCollectionType,
	kCustomCollectionType,
	kQuotesAndVersesCollectionType,
	kTexturesCollectionType,
	kEdgeMasksCollectionType,
	kFullMasksCollectionType,
	kFramesCollectionType,

	// Leapfrog-specific collection file types
	kNavigationButtonsCollectionType,
	kWebObjectsCollectionType,
	kAnimagedGIFCollectionType,
	kBackdropGIFCollectionType,

	kNumberOfCollectionTypes			// not a collection type, just # of types
};

enum OpenCollectionsStatus
{
	kOpenCollectionsSuccess = 0,
	kOpenCollectionsFailure,
	kOpenCollectionsOtherGraphics
};

// array of open collection files
typedef	RArray<RChunkStorage*>						RCollectionArray;
typedef	RArray<RChunkStorage*>::YIterator		RCollectionArrayIterator;


// collection file header structure in CLHD chunk
typedef struct CollectionHeader
{
	unsigned long 	yCollectionID;
	unsigned long 	yLangLocale;
	short		 		yCurFormatVersion;
	short		 		yMinFormatVersion;
	short		 		yCharEncoding;
	short		 		yCollectionType;
	unsigned long	yNumberOfEntries;
	char				yFirstUseDateTimeStamp[12];
	long		  		yUnused;
} CollectionHeader;


class FrameworkLinkage RCollectionManager : public RObject
{
	friend class RCollectionCDLockManager;

public:
	static RCollectionManager&		TheCollectionManager();
	// return reference to the single instance of a RCollectionManager

	~RCollectionManager();

	RCollectionArray& OpenCollectionsOfType(
		CollectionType				aCollectionType,
		OpenCollectionsStatus*	aStatus = NULL );
	// open all aCollectionType collection files in the "standard" directories
	// if aCollectionType files are already open, returns same RCollectionArray
	// call CloseCollectionsOfType when done with files in RCollectionArray
	// if the aStatus parameter is supplied, the status of the call is
	// returned as:
	//	kOpenCollectionsSuccess
	//		opened one or more collections
	//	kOpenCollectionsFailure
	//		user clicked "Cancel", no collections found or other error
	//	kOpenCollectionsOtherGraphics
	//		user clicked "Other Graphics"
	// if aStatus is not kOpenCollectionsSuccess, the RCollectionArray returned
	// will be empty

	BOOLEAN	IsCollectionTypeOpen( CollectionType aCollectionType );
	// return TRUE if aCollectionType collection files are open, else FALSE

	void	CloseCollectionsOfType( CollectionType aCollectionType );
	// close all aCollectionType collection files
	// RCollectionArray returned from OpenCollectionsOfType becomes invalid
	// does nothing if aCollectionType collection files are not open

	void	CloseAllCollections();
	// close all open collection files
	// all RCollectionArray's returned from OpenCollectionsOfType become invalid
	// does nothing if no collection files are open

	BOOLEAN GetImageByReferencePathPrefix( RChunkStorage* aStorage, RMBCString &aPath );
	// checks to see if aStorage is an image by ref type; if so
	// returns true, and location path prefix is returned via the aPath arg

	void UpdateImageByReferencePathPrefix( RChunkStorage* aStorage );
	// attempt to resolve location of files referenced by the image by reference
	// collection specified by aStorage and add the location to internal list for
	// access from GetImageByReferencePathPrefix(); useful for locating the files
	// on a newly inserted CD referenced by an image by reference collection.

	virtual void		Validate() const {}

private:
	RCollectionManager();	// should be exactly one instance of RCollectionManager

	// collection ID array used to avoid loading duplicates
	typedef	RArray<unsigned long>				CollectionIDArray;
	typedef	CollectionIDArray::YIterator		CollectionIDArrayIterator;

	// a means to associate "image by reference" collections with paths
	typedef struct
	{
		RChunkStorage*	storage;
		RMBCString		path;

	} RImageRefPath;

	typedef	RArray<RImageRefPath*>						RImageRefPathArray;
	typedef	RArray<RImageRefPath*>::YIterator		RImageRefPathArrayIterator;

	// attributes of an open collection file
	class RCollectionAttributes : public RObject
	{
	public:
		RCollectionAttributes()
		:	cdDriveLetter( 0 )
		{}

		char					cdDriveLetter;

		virtual void		Validate() const {}
	};

	typedef	RArray<RCollectionAttributes*>					RCollectionAttributesArray;
	typedef	RArray<RCollectionAttributes*>::YIterator		RCollectionAttributesArrayIterator;

	// all open collection files of a certain type (graphics, backdrops, etc.)
	class ROpenCollectionsOfType : public RObject
	{
	public:
		ROpenCollectionsOfType()
		:	openCollections( NULL ),
			openCollectionAttributes( NULL )
		{}

		RCollectionArray*						openCollections;
		RCollectionAttributesArray*		openCollectionAttributes;

		virtual void		Validate() const {}
	};

	ROpenCollectionsOfType*	openCollectionsOfTypeArray[ kNumberOfCollectionTypes ];

	char dirPath[ _MAX_PATH ];

	char* extensions[ kNumberOfCollectionTypes ];

	void ScanCollectionFiles(
		RCollectionArray*					theCollectionArray,
		RCollectionAttributesArray*	theCollectionAttributesArray,
		char*									wildcardPath,
		char*									wildcardName,
		BOOLEAN								isCD,
		char*									cdDriveRoot,
		BOOLEAN&								collectionsFound );

	int GetCDROMDrives( char *szBuff, int nMaxCDs );

	void UpdateImageByReferenceList(
		RChunkStorage* aChunkStorage,
		char *wildcardName,
		unsigned long id,
		BOOLEAN isCD,
		char* cdDriveRoot );

	BOOLEAN FindMatchingContentFile( unsigned long id, char *wildcardPath, char *wildcardName );

	RImageRefPathArray m_refPathArray;
	void PurgeRefPathArray();
	
	char *m_pCDDriveBuf;
	int   m_nCDDrives;
	char  m_CDCollectionDir[ _MAX_DIR ];
	char  m_CDReferenceDir[ _MAX_DIR ];


	CollectionIDArray   m_collectionIDArray;

	BOOLEAN IsNewCollection( unsigned long id );

	char AnyOpenCollectionsOnCD();

	RCollectionArray m_emptyCollectionArray;
};

// utility class for locking/unlocking the CD drive
// except for RCollectionManager, should be used only on app shartup and shutdown
class FrameworkLinkage RCollectionCDLockManager
{
	friend class RCollectionManager;

public:
	static RCollectionCDLockManager&		TheCollectionCDLockManager();

	void											UnlockAllDrives();

	void											LockDrive( char aDriveLetter );

private:
	RCollectionCDLockManager()	// should be exactly one instance of RCollectionCDLockManager
		:	driveLetter( '\0' )
	{}

	void											UnlockDrive();

	void											UnlockDriveInternal( char aDriveLetter );

	char											driveLetter;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _CollectionManager_H_
