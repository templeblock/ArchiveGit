// ****************************************************************************
//
//  File Name:			WindowsFileStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the Windows version of the RFileStream class
//
//  Portability:		Win32 specific
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WindowsFileStream.cpp                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "FileStream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#define kTEMP_FILE_EXTENSION "TMP"		//	temp file extension

// ****************************************************************************
//
// Function Name:		RFileStream::RFileStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RFileStream::RFileStream( )
	{
	m_fDeleteOnClose = FALSE;
	}


// ****************************************************************************
//
// Function Name:		RFileStream::RFileStream( const RMBCString& rFilename )
//
// Description:		Constructor with file name argument
//
// Returns:				nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RFileStream::RFileStream( const RMBCString& rFilename )
	:RMemoryMappedStream( rFilename )
	{
	m_fDeleteOnClose = FALSE;
	}

// ****************************************************************************
//
// Function Name:		RFileStream::RFileStream( const RFileStream& rFileStream )
//
// Description:		Constructor with file stream argument if file was already open
//
// Returns:				nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RFileStream::RFileStream( const RFileStream& rFileStream )
	:RMemoryMappedStream( rFileStream )
	{
	m_fDeleteOnClose = FALSE;
	}

// ****************************************************************************
//
// Function Name:		RFileStream::~RFileStream( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RFileStream::~RFileStream( )
	{
	try
		{
		Close();
		}
	catch( ... )
		{
		NULL;	//	we can't throw from a destructor so we have nothing to do
		}
	}

// ****************************************************************************
//
// Function Name:		RFileStream::Open( const RMBCString& rFilename, EAccessMode eAccess )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::Open( const RMBCString& rFilename, EAccessMode eAccess )
	{
	//	save given file name
	m_rFilename = rFilename;

	RMemoryMappedStream::Open( eAccess );
	}

// ****************************************************************************
//
// Function Name:		RFileStream::Open( EAccessMode eAccess )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::Open( EAccessMode eAccess )
	{
	RMemoryMappedStream::Open( eAccess );
	}

// ****************************************************************************
//
// Function Name:		RFileStream::Close( )
//
// Description:		close the file; delete it if requested
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::Close( )
	{
	RMemoryMappedStream::Close( );

	if ( m_fDeleteOnClose )
		{
		Delete( );
		}
	}


// ****************************************************************************
//
// Function Name:		RFileStream::CreateTempFile( )
//
// Description:		make this a file with a temp name in the system temp directory 
//
// Returns:				none
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::CreateTempFile( )
	{
	RMBCString rPath;
	TCHAR pcBuff[ MAX_PATH ];

	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if( 0 == ::GetTempPath( MAX_PATH, pcBuff ) )
		ThrowFileExceptionIfError();

	rPath = pcBuff;
	CreateTempFile( rPath );

	//	dont leave these around
	DeleteOnClose();
	}


// ****************************************************************************
//
// Function Name:		RFileStream::CreateTempFile( const XPlatformFileID& xpID )
//
// Description:		make this a file with a temp name in directory specified by the file id
//
// Returns:				none
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::CreateTempFile( const XPlatformFileID& xpID )
	{	
	//	If we are given a path use it else make a temp file in the temp path.
	if ( xpID.m_rFilename.IsEmpty( ) )
		CreateTempFile( );
	else
		CreateTempFile( xpID.m_rFilename );
	}


// ****************************************************************************
//
// Function Name:		RFileStream::CreateTempFile( const RMBCString& rPath )
//
// Description:		make this a file with a temp name in the given directory 
//
// Returns:				none
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::CreateTempFile( const RMBCString& rPathIn )
	{	
	HANDLE hFileHandle = INVALID_HANDLE_VALUE;
	
	//	get the directory we should use
	//	if the given path/file doesn't exist then create it 
	//	temporarily
	DWORD dwPathAttribs = ::GetFileAttributes( rPathIn );
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( 0xFFFFFFFF == dwPathAttribs )
		{
		hFileHandle = ::CreateFile( rPathIn, GENERIC_READ
			, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( INVALID_HANDLE_VALUE == hFileHandle )
			ThrowFileExceptionIfError( );
		dwPathAttribs = ::GetFileAttributes( rPathIn );
		}

	try
		{
		TCHAR pcBuff[ MAX_PATH ];
		RMBCString rPath;
		
		rPath = rPathIn;	//	make modifiable path

		//	if we don't have a directory then get the parent of the file name
		//	specified
		if ( !(dwPathAttribs & FILE_ATTRIBUTE_DIRECTORY) )
			{
			//	Get parent directory by stripping the filename from the given path.
			//	If the resulting path is empty then specify the current directory.
			HANDLE hSearch = INVALID_HANDLE_VALUE;
			try
				{
				//	Try to find the filename using ::FindFirstFile.
				WIN32_FIND_DATA  fdData;
				hSearch = ::FindFirstFile( rPathIn, &fdData );
				Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
				if ( INVALID_HANDLE_VALUE == hSearch )
					ThrowFileExceptionIfError();

				RMBCString rFileName(fdData.cFileName);
				
				//	Remove the file name from the given path. If the resulting path is empty then specify the current directory.
				rPath = rPathIn.Head( rPathIn.GetStringLength() - rFileName.GetStringLength() );
				if ( rPath.IsEmpty() )
					rPath = ".\\";

				Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
				if ( !::FindClose( hSearch ) )
					ThrowFileExceptionIfError();
				}
			catch( YException eError )
				{
				//	clean up search handle
				if ( INVALID_HANDLE_VALUE != hSearch )
					::FindClose( hSearch );
				ThrowFileException( eError );
				}
			}

		//	get a file in the given directory with the standard temp file
		//	extension into this files name 
		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		if ( 0 == ::GetTempFileName( rPath, kTEMP_FILE_EXTENSION, 0, pcBuff ) )
			ThrowFileExceptionIfError();
		
		//	save the file name
		m_rFilename = pcBuff;

		//	if we created the file above then delete it here
		if ( INVALID_HANDLE_VALUE != hFileHandle )
			if ( ::CloseHandle( hFileHandle ) )
				::DeleteFile( rPathIn );

		}
	catch(...)
		{
		if ( INVALID_HANDLE_VALUE != hFileHandle )
			if ( ::CloseHandle( hFileHandle ) )
				::DeleteFile( rPathIn );

		throw;
		}
	}


// ****************************************************************************
//
// Function Name:		RFileStream::GetFileIdentifier( XPlatformFileID& xpID )
//
// Description:		get a file identifier
//
// Returns:				none
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::GetFileIdentifier( XPlatformFileID& xpID )
	{
	xpID.m_rFilename				= m_rFilename;
	xpID.m_eOpenAccess			= m_eOpenAccess;
	xpID.m_fCanRandomAccess		= m_fCanRandomAccess;
	}


// ****************************************************************************
//
// Function Name:		RFileStream::Delete( )
//
// Description:		deletes a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::Delete( )
	{
	//	close the file if its open
	if (((INVALID_HANDLE_VALUE != m_hFileHandle) && (NULL != m_hFileHandle))
	|| ((INVALID_HANDLE_VALUE != m_hFileMapping) && (NULL != m_hFileMapping)))
		RMemoryMappedStream::Close( );

	if ( StreamExists() )
		{
//		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		::SetFileAttributes( m_rFilename, FILE_ATTRIBUTE_NORMAL );
//		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		::DeleteFile( m_rFilename );
		}
	}


// ****************************************************************************
//
// Function Name:		RFileStream::DeleteOnClose( )
//
// Description:		remembers to delete the file backing this stream when it is closed
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RFileStream::DeleteOnClose( BOOLEAN fDelete )
	{
	m_fDeleteOnClose = fDelete;
	}


// ****************************************************************************
//
// Function Name:		RFileStream::Rename( )
//
// Description:		rename the disk file; it must not be open
//
// Returns:				Nothing
//
// Exceptions:			Memory, File Exceptions
//
// ****************************************************************************
//
void RFileStream::Rename( const XPlatformFileID& xpID )
	{
	Rename( xpID.m_rFilename );
	}

void RFileStream::Rename( const RMBCString& rFilename )
	{
	TpsAssert((INVALID_HANDLE_VALUE == m_hFileMapping) || (NULL == m_hFileMapping), "File to rename is open");

	try
		{
			//	remember the disk file attributes
		DWORD dwSavedAttribs = ::GetFileAttributes( m_rFilename );
		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		if ( 0xFFFFFFFF == dwSavedAttribs )
			ThrowFileExceptionIfError();
		
		//	make sure we can rename the file
		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		if ( !::SetFileAttributes( m_rFilename, FILE_ATTRIBUTE_NORMAL ) )
			ThrowFileExceptionIfError();
		
		//	rename the file
		//	if the file exists then delete it first
		if ( 0xFFFFFFFF != ::GetFileAttributes( rFilename ) )
			if ( ! ::DeleteFile( rFilename ) )
				ThrowFileExceptionIfError();

		if ( ! ::MoveFile( m_rFilename, rFilename ) )
			ThrowFileExceptionIfError();

		//	restore the file's attributes
		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		if ( ! ::SetFileAttributes( rFilename, dwSavedAttribs ) )
			ThrowFileExceptionIfError();

		//	remember the new name in the object
		m_rFilename = rFilename;
		}

	catch( YException exception )
		{
		::ReportException( exception );
		throw;
		}
	}

// ****************************************************************************
//
// Function Name:		RFileStream::TestFileWritable( )
//
// Description:		Determines if a file is writable
//
// Returns:				Nothing
//
// Exceptions:			If the file is not writable, an exception is thrown 
//							containing the reason why.
//
// ****************************************************************************
//
void RFileStream::TestFileWritable( const RMBCString& filename )
	{
	// Try to open the file
	HANDLE handle = ::CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

	// If the operation failed, throw and exception
	if( handle == INVALID_HANDLE_VALUE )
		{
		// Throw the appropriate exception
		ThrowFileExceptionIfError( );
		}

	// Clean up
	::CloseHandle( handle );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RFileStream::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RFileStream::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RFileStream, this );
	}

#endif	// TPSDEBUG
