// ****************************************************************************
//
//  File Name:			FileFormat.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RFileFormat and RFileFormatCollection class
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
//  $Logfile:: /PM8/Framework/Source/FileFormat.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"FileFormat.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FileType.h"
#include "StandaloneApplication.h"
#include "ApplicationGlobals.h"
#include "StandaloneDocument.h"
#include "StandaloneView.h"
#include "ChunkyStorage.h"
#include "UndoManager.h"
#include "FileStream.h"
#include "FrameworkResourceIds.h"

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::RNativeFileFormat( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RNativeFileFormat::RNativeFileFormat( RStandaloneDocument* pDocument )
	: m_pDocument( pDocument )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::Load( )
//
//  Description:		Loads the specified file as this file format
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RNativeFileFormat::Load( const RMBCString& filename )
	{
	RWaitCursor waitCursor;

	RStandaloneDocument* pDocument = NULL;

	// Check to see if the file is already open in another copy of PrintShop.
	// If so, we will still load the file, but the user will not be able to
	// save it. They can still do a save-as.
	BOOLEAN fAlreadyOpen = FALSE;
	try
		{
		RFileStream::TestFileWritable( filename );
		}

	catch( YException exception )
		{
		// If we failed because of a sharing violation, the file is already open
		if( exception == kShareViolation )
			fAlreadyOpen = TRUE;
		}

	try
		{
		// Create a disk storage
		RChunkStorage* pStorage = new RChunkStorage( filename, kRead );

		// Find the document type chunk
		pStorage->Start( kDocumentTypeTag );

		// Load the document type
		YDocumentType documentType;
		*pStorage >> documentType;

		// Delete the storage to close it
		delete pStorage;

		// Create a new document
		pDocument = ::GetApplication( ).CreateNewDocument( documentType, FALSE, filename );

		// Create a disk storage
		pStorage = new RChunkStorage( filename, kRead );

		// Read the document
		ReadDocument( pDocument, *pStorage, kLoading );

		// delete the storage. This closes it
		delete pStorage;

		// If the file is already open, dont set the default file format or open a placeholder. This
		// makes save do a save as (because we wont have a file format), and does not lock the file
		// in the other app
		if( !fAlreadyOpen )
			{
			// Set the default file format
			pDocument->SetDefaultFileFormat( );

			//	Open a placeholder document to prevent the user from deleting this file from the explorer
			pDocument->OpenPlaceholder( filename );
			}

		// Warn the user that the file is already open
		else
			RAlert( ).InformUser( STRING_ERROR_FILE_ALREADY_OPEN );

		// Initial update
		pDocument->InitialUpdate( );
		}

	catch( ... )
		{
		// We failed for some reason, get the frame and kill it
		if( pDocument )
			{
			POSITION pos = pDocument->GetFirstViewPosition( );
			CFrameWnd* pFrame = pDocument->GetNextView( pos )->GetParentFrame( );
			pFrame->DestroyWindow( );
			}

		throw;
		}

	return pDocument;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::Load( )
//
//  Description:		Loads the specified ChunkyStorage as this file format
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RNativeFileFormat::Load( RChunkStorage& rDocStorage )
	{
	RWaitCursor waitCursor;

	RStandaloneDocument* pDocument = NULL;

	try
		{
		//	Find and remember the root of the document tree.
		YChunkStorageId yDocumentRoot = rDocStorage.GetSelectedChunk( );
		rDocStorage.Start( kDocumentTypeTag );

		// Load the document type
		YDocumentType documentType;
		rDocStorage >> documentType;

		//	Start again at the doc's root.
		rDocStorage.SetSelectedChunk( yDocumentRoot );

		// Create a new document
		pDocument = ::GetApplication( ).CreateNewDocument( documentType, FALSE );

		// Read the document
		ReadDocument( pDocument, rDocStorage, kLoading );

		// Initial update
		pDocument->InitialUpdate( );
		}

	catch( ... )
		{
		// We failed for some reason, get the frame and kill it
		if( pDocument )
			{
			POSITION pos = pDocument->GetFirstViewPosition( );
			CFrameWnd* pFrame = pDocument->GetNextView( pos )->GetParentFrame( );
			pFrame->DestroyWindow( );
			}

		throw;
		}

	return pDocument;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::ReadDocument( )
//
//  Description:		Reads the document
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RNativeFileFormat::ReadDocument( RStandaloneDocument* pDocument, RChunkStorage& storage, EReadReason reason ) const
	{
	pDocument->Read( storage, reason );
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::Save( )
//
//  Description:		Saves the specified file as this file format
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RNativeFileFormat::Save( const RMBCString& filename, EWriteReason reason ) const
	{
	RWaitCursor waitCursor;

	// Alias the docuemnts storage to save typing
	RChunkStorage*& pDocumentStorage = m_pDocument->m_pDocumentStorage;

	try
		{
		//	Figure out whether someone renamed our file behind our backs;
		//	under win95 and nt4 the user can do this to open files
		if( pDocumentStorage && !pDocumentStorage->StreamExists( ) )
			throw kFileNotFound;

		// Delete the old storage
		delete pDocumentStorage;
		pDocumentStorage = NULL;

		// Create a new storage
		pDocumentStorage = new RChunkStorage( filename, kWrite );

		//	Call a virtual function to actually write the data
		WriteData( *pDocumentStorage, reason );

		//	Flush the just written document and open a placeholder
		delete pDocumentStorage;
		pDocumentStorage = NULL;

		pDocumentStorage = new RChunkStorage( filename, kRead, kPlaceholder );
		pDocumentStorage->SetSaveOnClose( FALSE );

		// Notify the undo manager that we sucessfully saved
		m_pDocument->m_pUndoManager->DocSaved( );
		}

	catch( ... )
		{
		// Clean up
		if( pDocumentStorage )
			{
			//	Remember not to try to write since we failed already.
			pDocumentStorage->WriteFailed( TRUE );
			//	if overwriting existing file, don't let us replace it with half written temp file
			pDocumentStorage->SetSaveOnClose( FALSE );
			}

		//	Delete our storage.
		delete pDocumentStorage;
		pDocumentStorage = NULL;

		// Rethrow
		throw;
		}
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::Save( )
//
//  Description:		Saves the specified file as this file format
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RNativeFileFormat::Save( const RMBCString& filename ) const
	{
	Save( filename, kSaving );
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::WriteData( )
//
//  Description:		Virtual function called to write data. This allows us
//							to override the native file format class and write
//							additional data.
//
//							The default just writes the document
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RNativeFileFormat::WriteData( RChunkStorage& storage, EWriteReason reason ) const
	{
	// Add a chunk and write the document id
	storage.AddChunk( kDocumentTypeTag );
	storage << m_pDocument->GetDocumentType( );
	storage.SelectParentChunk( );

	// Write the document
	m_pDocument->Write( storage, reason );
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::Revert( )
//
//  Description:		Reverts a document to an old verson
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RNativeFileFormat::Revert( const RMBCString& filename, RStandaloneDocument* pDocument ) const
	{
	RWaitCursor waitCursor;

	RChunkStorage* pStorage = NULL;

	TpsAssert( pDocument != NULL, "Must have a document to revert." );

	try
		{
		// Create a disk storage
		pStorage = new RChunkStorage( filename, kRead );

		// Read the document
		ReadDocument( pDocument, *pStorage, kLoading );

		// delete the storage. This closes it
		delete pStorage;
		pStorage = NULL;

		//	Reopen the placeholder
		pDocument->OpenPlaceholder( filename );
		}

 	catch( ... )
		{
		delete pStorage;
		throw;
		}
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::CheckFile( )
//
//  Description:		Checks the specified file to see if it is of this format
//
//  Returns:			TRUE if the specified file is of this format
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RNativeFileFormat::CheckFile( const RMBCString& filename ) const
	{
	// Open the file as a storage
	RStorage* pStorage = new RStorage( filename, kRead );

	// Verify that it is a chunky storage
	BOOLEAN fIsChunkStorage = RChunkStorage::IsChunkStorage( pStorage );

	// Delete the storage
	delete pStorage;

	// If it isn't a chunk storage, it isn't a native document
	if( !fIsChunkStorage )
		return FALSE;

	// Now we know that its a chunk storage, open it as such
	RChunkStorage* pChunkStorage = new RChunkStorage( filename, kRead );

	// Look for the document type chunk
	RChunkStorage::RChunkSearcher searcher = pChunkStorage->Start( kDocumentTypeTag, FALSE );
	BOOLEAN fDocumentTypeChunkFound = static_cast<BOOLEAN>( !searcher.End( ) );

	// Delete the storage
	delete pChunkStorage;

	return fDocumentTypeChunkFound;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::GetSaveFileType( )
//
//  Description:		Gets the file type to be used when saving. The object
//							releases ownership of the pointer to the caller.
//
//  Returns:			Pointer to the file type
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileType* RNativeFileFormat::GetSaveFileType( ) const
	{
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::MergeLoadFileTypes( )
//
//  Description:		Called to merge the file types supported for loading into
//							the specified collection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RNativeFileFormat::MergeLoadFileTypes( RFileTypeCollection& ) const
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::IsSymmetric( )
//
//  Description:		Determines if this is a symmetric format; ie. that the
//							data can be re-read from the format without loss.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RNativeFileFormat::IsSymmetric( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RNativeFileFormat::GetPreviewBitmap( )
//
//  Description:		Retrieves a preview of the specified file in bitmap format.
//
//  Returns:			A pointer to the preview bitmap if it was found, NULL
//							otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBitmapImage* RNativeFileFormat::GetPreviewBitmap( const RMBCString& ) const
	{
	return NULL;
	}

//*****************************************************************************
//
// Function Name:	RFileFormatCollection::~RFileFormatCollection
//
// Description:	Destructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
RFileFormatCollection::~RFileFormatCollection( )
	{
	::DeleteAllItems( *this );
	}

//*****************************************************************************
//
// Function Name:	RFileFormatCollection::GetSaveFileTypes
//
// Description:	Adds the file types supported for save by all formats in this
//						collection to the specified file type collection.
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
void RFileFormatCollection::GetSaveFileTypes( RFileTypeCollection& fileTypeCollection ) const
	{
	// Iterate the collection, adding the save file type from each format
	YFileFormatIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		fileTypeCollection.InsertAtEnd( ( *iterator )->GetSaveFileType( ) );
	}

//*****************************************************************************
//
// Function Name:	RFileFormatCollection::GetLoadFileTypes
//
// Description:	Adds the file types supported for load by all formats in this
//						collection to the specified file type collection.
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************
//
void RFileFormatCollection::GetLoadFileTypes( RFileTypeCollection& fileTypeCollection ) const
	{
	// Iterate the collection, merging the load file types from each format
	YFileFormatIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		( *iterator )->MergeLoadFileTypes( fileTypeCollection );
	}

//*****************************************************************************
//
// Function Name:	RFileFormatCollection::GetMatchingFileFormat
//
// Description:	Checks each file type in this collection to see if the
//						specified file is of its format.
//
// Returns:			The first type found that the specified file matches
//						NULL if no file format matches
//
// Exceptions:		None
//
//*****************************************************************************
//
RFileFormat* RFileFormatCollection::GetMatchingFileFormat( const RMBCString& filename ) const
	{
	// Iterate the collection looking for a matching file type
	YFileFormatIterator iterator = Start( );
	for( ; iterator != End( ); ++iterator )
		if( ( *iterator )->CheckFile( filename ) )
			return *iterator;

	return NULL;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFileFormat::Validate( )
//
//  Description:		Validation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFileFormat::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RFileFormat, this );
	}

#endif	//	TPSDEBUG
