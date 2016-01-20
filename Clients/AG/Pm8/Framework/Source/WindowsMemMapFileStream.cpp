// ****************************************************************************
//
//  File Name:			WindowsMemoryMappedStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RMemoryMappedStream class
//
//  Portability:		Windows
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
//  $Logfile:: /PM8/Framework/Source/WindowsMemMapFileStream.cpp     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
// $Revision:: 1                                                              $
//
// ****************************************************************************

//							Implementation notes: 
//								Coherence is not guaranteed under win 95 for multiple
//								views into the same file so only one view is active at
//								a time. Stream positioning is emulated and stream data is
//								accessed directly. ::ReadFile and ::WriteFile
//								are not used.
//								Exceptions are handled in the class dtor. 
//								Resources are left allocated when exceptions are thrown
//								so that the caller has all available info to handle the
//								exception.

#include "FrameworkIncludes.h"

ASSERTNAME

#include		"MemoryMappedStream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

//	we can't include this in the testing code since it would require us to include too much of the framework
#ifndef kFILETESTPROJECT
#include		"ApplicationGlobals.h"
#endif

//	argument to open that informs it we are reopening the given file
const BOOLEAN kRemapping = TRUE;

#ifdef DEBUG
//	define a list of test points that can be caused to fail
static TESTPOINT* _TestList = NULL;
static BOOLEAN		_fAddTestToList = FALSE;
#endif

// ****************************************************************************
//
// Function Name:		FileTimeInfo( )
//
// Description:		ctor.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
	FileTimeInfo::FileTimeInfo( )
		: m_fFileDatesValid( FALSE )
	{
	}

// ****************************************************************************
//
// Function Name:		ThrowFileExceptionIfError( )
//
// Description:		Converts the error code returned from GetLastError to an
//							exception.
//
// Returns:				Nothing
//
// Exceptions:			File exceptions
//
// ****************************************************************************
//
void ThrowFileExceptionIfError( )
	{
	DWORD dwError = ::GetLastError( );

	TpsAssert( NO_ERROR != dwError, "a file exception occured but we can't figure out what kind" );	

	switch( dwError )
		{
		case NO_ERROR :
			::ThrowException( kGenericFile );
			break;

		case ERROR_GEN_FAILURE :
			::ThrowException( kGenericFile );
			break;

		case ERROR_NOT_READY:
			::ThrowException( kDriveNotReady );
			break;

		case ERROR_FILE_NOT_FOUND :
			::ThrowException( kFileNotFound );
			break;

		case ERROR_PATH_NOT_FOUND :
			::ThrowException( kInvalidPath );
			break;

		case ERROR_TOO_MANY_OPEN_FILES :
			::ThrowException( kTooManyOpenFiles );
			break;

		case ERROR_ACCESS_DENIED :
			//	REVIEW - ESV 6/20/96 - would like to distinguish between network file 
			//		access restrictions and locked file opened for write access
			::ThrowException( kAccessDenied );
			break;

		case ERROR_DISK_FULL :
			::ThrowException( kDiskFull );
			break;

		case ERROR_SEEK :
			::ThrowException( kBadSeek );
			break;

		case ERROR_NOT_ENOUGH_MEMORY:
			::ThrowException( kSwapfileFull );
			break;

		case ERROR_SHARING_VIOLATION :
		case ERROR_ALREADY_EXISTS :
			::ThrowException( kShareViolation );
			break;

		case ERROR_LOCK_VIOLATION :
			::ThrowException( kLockViolation );
			break;

		case ERROR_USER_MAPPED_FILE :
			::ThrowException( kGenericFile );
			break;

		case ERROR_FILE_INVALID :
			::ThrowException( kGenericFile );
			break;

		case ERROR_INVALID_DRIVE :
			::ThrowException( kGenericFile );
			break;

		case ERROR_INVALID_ADDRESS :
			::ThrowException( kGenericFile );
			break;

		case kResource :
			::ThrowException( kResource );
			break;

		case kUnknownError :
			::ThrowException( kUnknownError );
			break;

		case kDataFormatInvalid :
			::ThrowException( kDataFormatInvalid );
			break;

		case kDataFormatTooOld :
			::ThrowException( kDataFormatTooOld );
			break;

		case kMemory :
			::ThrowException( kMemory );
			break;

		case kWriteProtected :
			::ThrowException( kWriteProtected );
			break;

		case kEndOfFile :
			::ThrowException( kEndOfFile );
			break;

		case CFileException::hardIO:
			::ThrowException( kDriveNotReady );
			break;

		default :
			TpsAssertAlways( "generic exception used" );
			::ThrowException( kGenericFile );
		}
	}

// ****************************************************************************
//
// Function Name:		ThrowFileException( )
//
// Description:		Sets the given error code and throws an
//							exception.
//
// Returns:				Nothing
//
// Exceptions:			File exceptions
//
// ****************************************************************************
//
void ThrowFileException( uLONG ulError )
	{
	::SetLastError( GetPlatformException(ulError) );
	ThrowFileExceptionIfError();
	
	//	we should not reach this statement
	TpsAssert( FALSE, "An exception was not thrown when an error occured" );
	}

// ****************************************************************************
//
// Function Name:		ThrowFileExceptionIfTesting( uLONG ulError )
//
// Description:		throws an exception if global testing controls say we should
//
// Returns:				Nothing
//
// Exceptions:			File exceptions
//
// ****************************************************************************
//
void ThrowFileExceptionIfTesting( uLONG ulError )
	{
//	we can't do this in the testing code since it would require us to include too much of the framework
#ifndef kFILETESTPROJECT
	if ( (0 == GetFileNumOperationsTillFailCount()) && GetFileNumTimesToFailCount( ) )
		{
		--GetFileNumTimesToFailCount( );
		ThrowException( ulError );
		}

	if ( 0 < GetFileNumOperationsTillFailCount() )
		--GetFileNumOperationsTillFailCount();
#else
	ulError = 0;	//	dummy code to prevent unreferenced parameter warning
#endif
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::ResetMVars( )
//
// Description:		Clears member variables; save some members values if the
//							object contains some re-useable flags (fClearAll == TRUE)
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMemoryMappedStream::ResetMVars( BOOLEAN fClearAll )
	{
	if ( fClearAll )
		m_rFilename.Empty();

	m_hFileHandle = INVALID_HANDLE_VALUE;
	m_hFileMapping = INVALID_HANDLE_VALUE;
	m_pFileBase = NULL;
	m_ySize = 0;
	m_yMarker = 0;
	if ( fClearAll )
		m_yAllocSize = 0;

	if ( fClearAll )
		m_ulAllocGran = 0;
	}

// ****************************************************************************
//
// Function Name:		RStream::AtEnd( )
//
// Description:		Return TRUE if at end of file
//
// Returns:				end of file condition
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RMemoryMappedStream::AtEnd( ) const
	{
	return RStream::AtEnd();
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::RMemoryMappedStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RMemoryMappedStream::RMemoryMappedStream( )
	: m_hFileHandle(INVALID_HANDLE_VALUE), m_hFileMapping(INVALID_HANDLE_VALUE),
	m_pFileBase(NULL), m_ySize(0), m_yMarker(0),	m_yAllocSize(0), m_ulAllocGran(0)
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::RMemoryMappedStream( const RMBCString& rFilename )
//
// Description:		Constructor with file name argument
//
// Returns:				nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RMemoryMappedStream::RMemoryMappedStream( const RMBCString& rFilename )
	: m_hFileHandle(INVALID_HANDLE_VALUE), m_hFileMapping(INVALID_HANDLE_VALUE),
	m_pFileBase(NULL), m_ySize(0), m_yMarker(0),	m_yAllocSize(0), m_ulAllocGran(0),
	m_rFilename(rFilename)
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::RMemoryMappedStream( const RMemoryMappedStream& rMemoryStream )
//
// Description:		Constructor with file stream in case file was already open argument
//
// Returns:				nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RMemoryMappedStream::RMemoryMappedStream( const RMemoryMappedStream& /* rMemoryStream */ )
	: m_hFileHandle(INVALID_HANDLE_VALUE), m_hFileMapping(INVALID_HANDLE_VALUE),
	m_pFileBase(NULL), m_ySize(0), m_yMarker(0),	m_yAllocSize(0), m_ulAllocGran(0),
	m_rFilename(NULL)
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::~RMemoryMappedStream( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RMemoryMappedStream::~RMemoryMappedStream( )
	{
	try
		{
		if (TestFileMapping())
			Close( );

		//	close the file mapping if its open
		if ((INVALID_HANDLE_VALUE != m_hFileHandle) && (NULL != m_hFileHandle))
			{
			BOOLEAN fSuccess = static_cast<BOOLEAN>( ::CloseHandle(m_hFileHandle) );
			m_hFileHandle = INVALID_HANDLE_VALUE;
			}
		}
	catch( ... )
		{
		NULL;	//	we can't throw from a destructor so we have nothing to do
		}
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::StreamExists( ) const
//
// Description:		return whether the file backing our stream exists
//
// Returns:				^
//
// Exceptions:			File
//
// ****************************************************************************
//
BOOLEAN RMemoryMappedStream::StreamExists( ) const
	{
	//	under win95 and nt the file can change its name while its open
	//		so if its gone then throw an exception
	DWORD dwAttrib = ::GetFileAttributes( m_rFilename );
	
	if ( 0xFFFFFFFF == dwAttrib )
		return FALSE;
	else
		return TRUE;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetFilename( ) const
//
// Description:		return the name of the file backing this stream
//
// Returns:				m_rFilename
//
// Exceptions:			none
//
// ****************************************************************************
//
RMBCString& RMemoryMappedStream::GetFilename( )
	{
	return m_rFilename;
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetHandle( )
//
// Description:		the files handle
//
// Returns:				^
//
// Exceptions:			file
//
// ****************************************************************************
//
HANDLE RMemoryMappedStream::GetHandle ( )
{
	return m_hFileHandle;
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Close( )
//
// Description:		close the file
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Close( )
	{
	CloseRemap( FALSE );
	}



// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetFileTime( FileTimeInfo& fileTimeInfo )
//
// Description:		Get the file creation date
//
// Returns:				^
//
// Exceptions:			none
//
// ****************************************************************************
//
void RMemoryMappedStream::GetFileTime( FileTimeInfo& fileTimeInfo )
	{
	//	Get the file time.
	//	Clear the initial time values until we retreive new valid ones.
	fileTimeInfo.m_fFileDatesValid = FALSE;

	//	Get the file times if our file info is valid.
	if ( (INVALID_HANDLE_VALUE != m_hFileHandle) && (NULL != m_hFileHandle) )
		{
		if ( ::GetFileTime( m_hFileHandle, &fileTimeInfo.m_fileCreationDate
			, &fileTimeInfo.m_fileLastAccessDate, &fileTimeInfo.m_fileLastWriteDate ) )
			fileTimeInfo.m_fFileDatesValid = TRUE;
		}
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::SetFileTime( const FileTimeInfo& fileTimeInfo )
//
// Description:		Install the given file creation date
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
void RMemoryMappedStream::SetFileTime( const FileTimeInfo& fileTimeInfo )
	{
	//	note that we should not throw an exception here since this fcn is called from our dtor

	TpsAssert( !m_rFilename.IsEmpty(), "no file name" );
	TpsAssert( (INVALID_HANDLE_VALUE == m_hFileMapping) || (NULL == m_hFileMapping), "A File Mapping exists; cannot re-date an open file" );

	m_hFileHandle = ::CreateFile( m_rFilename, GENERIC_WRITE | GENERIC_READ
			, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	//	Install the given file creation date.
	if ( fileTimeInfo.m_fFileDatesValid && (INVALID_HANDLE_VALUE != m_hFileHandle) && (NULL != m_hFileHandle) )
		{
		//	Note we ignore any errors we encounter.
		::SetFileTime( m_hFileHandle, &fileTimeInfo.m_fileCreationDate, &fileTimeInfo.m_fileLastAccessDate, &fileTimeInfo.m_fileLastWriteDate );
		}

	::CloseHandle(m_hFileHandle);
	m_hFileHandle = INVALID_HANDLE_VALUE;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::CloseRemap( )
//
// Description:		close the file; reset its size if fRemapping is TRUE
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::CloseRemap( BOOLEAN fRemapping )
	{
	//	note that we can't throw an exception from here because this gets called from our dtor

	//	Pick up the current file creation and modification dates if we need to preserve them.
	FileTimeInfo fileTimeInfo;	
	if ( m_fPreserveFileDate && (INVALID_HANDLE_VALUE != m_hFileHandle) && (NULL != m_hFileHandle) )
		GetFileTime( fileTimeInfo );
	
	//	close the file handle and mapping if their open
	if ((INVALID_HANDLE_VALUE != m_hFileHandle) && (NULL != m_hFileHandle))
		{
		::CloseHandle(m_hFileHandle);
		m_hFileHandle = INVALID_HANDLE_VALUE;
		}
	
	//	if we threw an exception we could return here when the destructor
	//	tries to close the file so just exit.
	if( (INVALID_HANDLE_VALUE == m_hFileMapping) || (NULL == m_hFileMapping) )
		return;

	::FlushViewOfFile( m_pFileBase, 0  );	//	flush entire file

	::UnmapViewOfFile(m_pFileBase);
	m_pFileBase = NULL;

	::CloseHandle(m_hFileMapping);
	m_hFileMapping = INVALID_HANDLE_VALUE;

	//	reset end of file to the used area; exclude the extra area allocated 
	//	skip this step for read only files
	if ( !fRemapping && (kRead != m_eOpenAccess) )
		{
		RemoveFileExtraBuffer();
		ResetMVars( FALSE );
		}

	//	Preserve file creation date if requested.
	if ( m_fPreserveFileDate )
		SetFileTime( fileTimeInfo );

	RStream::Close( );		//	call base method to enable leak tracing
//	TRACE("close %s\n", m_rFilename );
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Write( )
//
// Description:		Writes data to this file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::Write( YStreamLength yBytesToWrite, const uBYTE* pBuffer )
	{
	TpsAssert( NULL != pBuffer, "Null buffer" );
	TpsAssert( TestFileMapping(), "No File Mapping" );
	TpsAssert( 0 < yBytesToWrite, "Attempt to write zero bytes" );
	TpsAssert( 0xFFFFFF > yBytesToWrite, "Attempt to write more than 16MB" );
	
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( kRead == m_eOpenAccess )							//	prevent write to read only file
		ThrowFileException( kAccessDenied );

	__int64 qNewFileEnd = m_yMarker + yBytesToWrite;
	TpsAssert( ULONG_MAX > qNewFileEnd, "Stream position overflowed" );			//	verify the cast on the next line
	YStreamPosition yNewFileEnd = m_yMarker + yBytesToWrite;

	//	make sure the current mapping is large enough to accomodate this operation
	RemapLarger( yNewFileEnd );

	//	copy between buffer and mapping
	//	we are not currently required to support overlapping buffer and mapping
	TpsAssert( !( 
		//	start of destination is in source
		( ((m_pFileBase + m_yMarker) > pBuffer) && ((m_pFileBase + m_yMarker) < (pBuffer + yBytesToWrite)) ) || 
		//	end of destination is in source
		( ((m_pFileBase + m_yMarker + yBytesToWrite) > pBuffer) && ((m_pFileBase + m_yMarker + yBytesToWrite) < (pBuffer + yBytesToWrite)) ) ||
		//	destination encloses source
		( ((m_pFileBase + m_yMarker) < pBuffer) && ((m_pFileBase + m_yMarker + yBytesToWrite) > (pBuffer + yBytesToWrite)) ) )
		, "overlapping source and destination not supported" );

	::memcpy( m_pFileBase + m_yMarker, pBuffer, yBytesToWrite);

	m_yMarker = m_yMarker + yBytesToWrite;
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::RemapLarger( )
//
// Description:		Enlarge the file mapping to accomodate the specified end
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::RemapLarger( const YStreamPosition yNewFileEnd )
	{
	YStreamPosition ySizeSave = m_ySize;

		//	if the write takes us past the end of the file
	//	close and then grow the file and remap it
	if ( yNewFileEnd > m_yAllocSize) 
		{
		//	move the actual end of the file to accomodate the new data
		//	use file size which is an integral number of extra size buffers
		YStreamLength yExtraBytes = ((yNewFileEnd - m_yAllocSize)/GetExtraSize() + 1) * GetExtraSize();
		m_yAllocSize += yExtraBytes;

		//	when were read only dont grow the mapping past the end of the file
		if (kRead == m_eOpenAccess )
			m_yAllocSize = Min( m_yAllocSize, m_ySize );

		//	save values that will be cleared
		//	If we opened the stream to replace existing then we must 
		//	open it to preserve the existing file now or we will lose
		//	the existing file contents.
		EAccessMode eOpenModeSave = m_eOpenAccess;
		if ( kWriteReplaceExisting == eOpenModeSave )
			eOpenModeSave = kWrite;
		else if ( kReadWriteReplaceExisting == eOpenModeSave )
			eOpenModeSave = kReadWrite;

		YStreamPosition yMarkerSave = m_yMarker;
		CloseRemap( TRUE );									//	close the file; this clears m_ySize but were using yNewFileEnd
		
		//	reopen the file with the new minimum size
		Open( eOpenModeSave, m_yAllocSize, kRemapping );

		//	restore values
		m_yMarker = yMarkerSave;
		}
	//	if we grew the file then remember the new size
	m_ySize = Max( ySizeSave, yNewFileEnd );
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Read( )
//
// Description:		Reads data from this file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	TpsAssert( NULL != pBuffer, "Null buffer" );
	TpsAssert( TestFileMapping(), "No File Mapping" );
	TpsAssert( 0xFFFFFF > yBytesToRead, "Attempt to read more than 16MB" );
	TpsAssert( 0 < yBytesToRead, "Attempt to read zero bytes" );
	
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( kWrite == m_eOpenAccess )							//	prevent read from write only file
		ThrowFileException( kAccessDenied );

	__int64 qNewFilePos = m_yMarker + yBytesToRead;
	TpsAssert( ULONG_MAX > qNewFilePos, "Stream position overflowed" );			//	verify the cast on the next line
	YStreamPosition yNewFilePos = m_yMarker + yBytesToRead;

	BOOL fReadPastEnd = FALSE;
	if (yNewFilePos > m_ySize)	
		{
		//	if we read past the end of the file read what we can
		//	and then throw an exception
		yNewFilePos		= m_ySize;
		yBytesToRead	= m_ySize - m_yMarker;
		fReadPastEnd	= TRUE;
		}

	//	make sure the current mapping is large enough to accomodate this operation
	if (!fReadPastEnd)
		RemapLarger( yNewFilePos );

	//	we are not currently required to support overlapping buffer
	TpsAssert( !(
		//	start of source is in destination
		( ((m_pFileBase + m_yMarker) > pBuffer) && ((m_pFileBase + m_yMarker) < (pBuffer + yBytesToRead)) ) || 
		//	end of source is in destination
		( ((m_pFileBase + m_yMarker + yBytesToRead) > pBuffer) && ((m_pFileBase + m_yMarker + yBytesToRead) < (pBuffer + yBytesToRead)) ) ||
		//	source encloses destination
		( ((m_pFileBase + m_yMarker) < pBuffer) && ((m_pFileBase + m_yMarker + yBytesToRead) > (pBuffer + yBytesToRead)) ) )
		, "overlapping source and destination not supported" );

	::memcpy( pBuffer, m_pFileBase + m_yMarker, yBytesToRead);
	m_yMarker = yNewFilePos;

	Debug( ThrowFileExceptionIfTesting( kEndOfFile ) );
	if (fReadPastEnd)
		ThrowFileException( kEndOfFile );
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Copy( RStream* pStream )
//
// Description:		Copy given stream into this stream from the current 
//								stream position to the current stream position
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Copy( RStream* pStream )
	{
	Copy( pStream, pStream->GetSize() - pStream->GetPosition() );
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Copy( RMemoryMappedStream* pStream )
//
// Description:		Copy given stream into this stream from the current 
//								stream position to the current stream position
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Copy( RMemoryMappedStream* pStream )
	{
	TpsAssert( NULL != pStream, "Null Stream ptr" );

	Copy( pStream, pStream->GetSize() - pStream->GetPosition());
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Copy( RStream* pStream, YStreamLength yLength )
//
// Description:		Copy yLength bytes from the given stream into this stream from the current 
//								stream position to the current stream position
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Copy( RStream* pStream, const YStreamLength yLength )
	{
	TpsAssert( NULL != pStream, "Null Stream ptr" );

	RMemoryMappedStream *pMStream = dynamic_cast<RMemoryMappedStream*>(pStream);
	TpsAssert( NULL != pMStream, "dynamic_cast failed" );

	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( NULL == pMStream )
		ThrowFileException( kAccessDenied );
	
	Copy( pMStream, yLength );
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Copy( RMemoryMappedStream* pStream, YStreamLength yLength )
//
// Description:		Copy yLength bytes from the given stream into this stream from the current 
//								stream position to the current stream position
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Copy( RMemoryMappedStream* pMStream, const YStreamLength yLength )
	{
	TpsAssert( NULL != pMStream, "Null Stream ptr" );

	//	make sure the this stream is large enough to accomodate the write
	__int64 qNewFilePos = GetPosition() + yLength;
	TpsAssert( ULONG_MAX > qNewFilePos, "Stream position overflowed" );
	YStreamPosition yNewFilePos = GetPosition() + yLength;
	RemapLarger( yNewFilePos );

	//	copy into this stream from the given stream
	pMStream->Read( yLength, m_pFileBase + m_yMarker );

	//	if we grew the file then remember the new size
	m_ySize = Max( m_ySize, yNewFilePos );
	
	//	move fp
	m_yMarker = yNewFilePos;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::CreateFile( )
//
// Description:		opens specified file; places its handle in m_hFileHandle
//							NOTE: this function is overridden because it is not needed
//								for files backed by the system paging file (in RMemoryStream)
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::CreateFile( LPCTSTR lpFileName, DWORD ulDesiredAccess
						, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes
						, DWORD  dwCreationDistribution, DWORD  dwFlagsAndAttributes
						, HANDLE  hTemplateFile )
	{
	m_hFileHandle =  ::CreateFile( lpFileName, ulDesiredAccess, dwShareMode, lpSecurityAttributes
		, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
	
	//	it's ok to open a file that exists already
	//	preserve other errors
	if( INVALID_HANDLE_VALUE == m_hFileHandle )
		{
		DWORD dwError = ::GetLastError();
		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		if ( ERROR_ALREADY_EXISTS == dwError )
			::SetLastError(NO_ERROR);
		else if (NO_ERROR != dwError)
			ThrowFileException( GetCrossPlatformException( dwError ) );
		}
	
	//	don't open a read only file for writing
	DWORD dwAttrib = ::GetFileAttributes( lpFileName );
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( 0xFFFFFFFF == dwAttrib )
		ThrowFileExceptionIfError( );
	if ( (ulDesiredAccess & GENERIC_WRITE) &&  (FILE_ATTRIBUTE_READONLY & dwAttrib) )
		ThrowFileException( kAccessDenied );

	uLONG ulHi;
	m_ySize = ::GetFileSize( m_hFileHandle, &ulHi );
	TpsAssert( 0 == ulHi, "Opening a file larger than 4GB" );
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( 0xFFFFFFFF == m_ySize )
		ThrowFileExceptionIfError( );
	
	m_yAllocSize = GetExtraSize();

	//	Can't seem to map files with 0 size so grow the file a little
	//	this is a pain for read only files so assert and ::WriteFile causes an exception;
	//	The user couldn't do anything except close the file anyway
	if (0 == m_ySize)
		{
		if ( m_eOpenAccess == kRead )
			::ThrowException( kEndOfFile );
		BOOLEAN fSuccess = static_cast<BOOLEAN>( ::WriteFile( m_hFileHandle, &m_ySize, 1, &m_ySize, NULL ) );
		m_ySize = 0;
		Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
		if ( !fSuccess )
			ThrowFileExceptionIfError( );

		Debug( ThrowFileExceptionIfTesting( kBadSeek ) );
		if ( 0xFFFFFFFF == ::SetFilePointer( m_hFileHandle, 0, NULL, FILE_BEGIN ) )
			ThrowFileExceptionIfError( );
		}
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::SeekAbsolute( )
//
// Description:		Repositions the stream pointer to an absolute position
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::SeekAbsolute( const YStreamPosition position )
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	Debug( ThrowFileExceptionIfTesting( kBadSeek ) );
	if (position > m_ySize)
			ThrowFileException( kBadSeek );

	m_yMarker = position;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::SeekRelative( )
//
// Description:		Repositions the stream pointer relative to the current marker
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::SeekRelative( const YStreamOffset ySeekOffset )
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	__int64 qTestOffset = m_yMarker + ySeekOffset;
	TpsAssert( ULONG_MAX > qTestOffset, "Stream position overflowed" );			//	verify the cast on the next line
	
	//	don't seek to before beginning or after end of file
	Debug( ThrowFileExceptionIfTesting( kBadSeek ) );
	if ( (qTestOffset < 0) || (qTestOffset > m_ySize) )
		ThrowFileException ( kBadSeek );

	YStreamPosition position = m_yMarker + ySeekOffset;
	SeekAbsolute( position );
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetPosition( )
//
// Description:		Gets the current stream pointer
//
// Returns:				The stream pointer
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
YStreamPosition RMemoryMappedStream::GetPosition( ) const
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	return m_yMarker;
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::RemoveFileExtraBuffer( )
//
// Description:		reset end of file to the used area; exclude the extra area allocated
//							to enhance performace
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::RemoveFileExtraBuffer( )
	{
	//	note that we should not throw an exception here since this fcn is called from our dtor

	TpsAssert( !m_rFilename.IsEmpty(), "no file name" );
	TpsAssert( (INVALID_HANDLE_VALUE == m_hFileMapping) || (NULL == m_hFileMapping), "A File Mapping exists; cannot resize an open file" );

	//	adjust the file size on disk so that it excludes the extra allocation
	//	area
	//	reopen the file; goto the used size; set the end of file position; reclose it
	m_hFileHandle = ::CreateFile( m_rFilename, GENERIC_WRITE | GENERIC_READ
			, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	::SetFilePointer( m_hFileHandle, m_ySize, NULL, FILE_BEGIN );
	
	//	this can fail if we already have this file open in another document
	//	and we are here when cleaning up in the dtor so ignore errors from this call
	::SetEndOfFile( m_hFileHandle );

	::CloseHandle(m_hFileHandle);
	m_hFileHandle = INVALID_HANDLE_VALUE;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetOpenMode( EAccessMode eOpenMode, uLONG& ulDesiredAccess, uLONG& ulProtection, uLONG& ulMapAccess )
//
// Description:		open the file with the given access mode
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMemoryMappedStream::GetOpenMode( EAccessMode eOpenMode, uLONG& ulDesiredAccess, uLONG& ulProtection, uLONG& ulMapAccess, uLONG& ulCreateDist )
	{
	switch( eOpenMode )
		{
		case kRead :
			ulDesiredAccess = GENERIC_READ;
			ulProtection = PAGE_READONLY | SEC_COMMIT;
			ulMapAccess = FILE_MAP_READ;
			ulCreateDist = OPEN_EXISTING;
			break;

		case kWriteReplaceExisting :
		case kWrite :
			ulDesiredAccess = GENERIC_WRITE | GENERIC_READ;	//	there is no write only protection so we use read/write
			ulProtection = PAGE_READWRITE | SEC_COMMIT;
			ulMapAccess = FILE_MAP_WRITE;
			ulCreateDist = OPEN_ALWAYS;
			break;

		case kReadWriteReplaceExisting :
		case kReadWrite :
			ulDesiredAccess = GENERIC_READ | GENERIC_WRITE;
			ulProtection = PAGE_READWRITE | SEC_COMMIT;
			ulMapAccess = FILE_MAP_WRITE;
			ulCreateDist = OPEN_ALWAYS;
			break;

		default :
			TpsAssert( FALSE, "Invalid access mode." );
		};
	
	//	Change the CreateDist to replace the existing file if we are asked to.
	if ( ( kWriteReplaceExisting == eOpenMode ) || ( kReadWriteReplaceExisting == eOpenMode ) )
		ulCreateDist = CREATE_ALWAYS;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetSize( )
//
// Description:		return the size of the stream
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
YStreamLength RMemoryMappedStream::GetSize( ) const
	{
	return m_ySize;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetExtraSize( )
//
// Description:		return the actually allocated size of the stream; we pad allocations
//							so that we don't have to reallocate the file everytime it grows by this amount
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
YStreamLength RMemoryMappedStream::GetExtraSize( ) const
	{
	return RStream::kDiskExtra;
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Open( EAccessMode eOpenMode )
//
// Description:		open the file with the given access mode
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMemoryMappedStream::Open( EAccessMode eOpenMode )
	{
	Open( eOpenMode, 0 );
	}

void RMemoryMappedStream::Open( EAccessMode eOpenMode, YStreamLength yMinSize, BOOLEAN /*fRemapping*/ /*= FALSE*/ )
	{
	TpsAssert( INVALID_HANDLE_VALUE == m_hFileMapping, "A File Mapping Already Exists" );
	TpsAssert( INVALID_HANDLE_VALUE == m_hFileHandle, "A File Handle Already Exists" );
	TpsAssert( (kRead == eOpenMode) || (kReadWrite == eOpenMode) || (kWrite == eOpenMode) || (kWriteReplaceExisting == eOpenMode) || (kReadWriteReplaceExisting == eOpenMode), "Invalid access mode" );
	TpsAssert( 0x1FFFFFFF > yMinSize, "Initial size more than 500 MB" );

	uLONG ulDesiredAccess;
	uLONG ulProtection;
	uLONG ulMapAccess;
	uLONG ulCreateDist;
	GetOpenMode( eOpenMode, ulDesiredAccess, ulProtection, ulMapAccess, ulCreateDist );

	//	Following lines of code added to make flags consistent as part of
	//	trying to share open files to fix revert to save under Windows 95
	uLONG ulFileAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN;
	uLONG ulFileShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	if(kRead == eOpenMode)
		{
		ulFileAttributes = FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN;
		ulFileShareMode = FILE_SHARE_READ;
		}

	//	get allocation granularity size
	SYSTEM_INFO si;
	::GetSystemInfo( &si );
	m_ulAllocGran = si.dwAllocationGranularity;
	
	m_eOpenAccess = eOpenMode;

	CreateFile(	m_rFilename,
					ulDesiredAccess,
//					0,								//	allow no sharing 
					ulFileShareMode,				//	allow sharing for read access 
					NULL,							//	default security
					ulCreateDist,
					ulFileAttributes,
					NULL );							//	no template file

	//	make sure the file is at least as large as it needs to be
	if (yMinSize > m_yAllocSize)
		{
		YStreamLength yExtraBytes = ((yMinSize - m_yAllocSize)/GetExtraSize() + 1) * GetExtraSize();
		m_yAllocSize += yExtraBytes;
		}

	//	when were read only dont grow the mapping past the end of the file
	if (kRead == m_eOpenAccess )
		m_yAllocSize = Min( m_yAllocSize, m_ySize );

	m_hFileMapping = ::CreateFileMapping( m_hFileHandle, 
											NULL,						//	default security attributes
											ulProtection,	
											0,							//	hi 32 bits of object
											m_yAllocSize,			//	reserve space in the page file for the entire file
											NULL );					//	no name

	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( NULL == m_hFileMapping )
		ThrowFileExceptionIfError( );	

	m_pFileBase = (PUBYTE)::MapViewOfFile( m_hFileMapping,
							ulMapAccess,
							0,									//	hi 32 bits of offset; 0 for files less than 4gb
							0,									//	lo 32 bits of offset
							m_yAllocSize );				//	map entire file

	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );

	if ( NULL == m_pFileBase )
		{
		m_hFileMapping = INVALID_HANDLE_VALUE;
		ThrowFileExceptionIfError( );
		}

	RStream::Open( eOpenMode );		//	call base method to enable leak tracing
//	TRACE("open %s\n", m_rFilename );
	}

// ************************************************************************k****
//
// Function Name:		RMemoryMappedStream::GetBuffer( const YStreamLength yLength )
//
// Description:		returns a buffer of the specified size containing stream
//							contents after the current marker
//
// Returns:				^
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
PUBYTE RMemoryMappedStream::GetBuffer( const YStreamLength yLength )
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	//	make room for the new bytes as necessary
	YStreamPosition yOldMarker = m_yMarker;
	Grow( yLength );
	m_yMarker = yOldMarker;

	return m_pFileBase + m_yMarker;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::ReleaseBuffer( PUBYTE pBuffer )
//
// Description:		release the given buffer which was allocated by getbuffer
//
// Returns:				none
//
// Exceptions:			none
//
// ****************************************************************************
//
void RMemoryMappedStream::ReleaseBuffer( PUBYTE /*pBuffer*/ ) const
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	//	buffers from memory mapped streams don't need to be released so do nothing
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Grow( const YStreamLength yLength )
//
// Description:		Increase the stream size by the given size.
//							Use this fcn when you don't care about the contents
//							of a new stream area.
//
// Returns:				none
//
// Exceptions:			File
//
// ****************************************************************************
//
void RMemoryMappedStream::Grow( const YStreamLength yLength )
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	__int64 qNewFileEnd = m_yMarker + yLength;
	TpsAssert( ULONG_MAX > qNewFileEnd, "Stream position overflowed" );			//	verify the cast on the next line
	YStreamPosition yNewFileEnd = m_yMarker + yLength;

	//	make sure the current mapping is large enough to accomodate this operation
	RemapLarger( yNewFileEnd );
	m_yMarker += yLength;
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::FillWithByte( const uBYTE ubPattern, const YStreamLength yLength )
//
// Description:		write the given byte into the stream starting at the current marker. Write yLength bytes
//
// Returns:				none
//
// Exceptions:			File
//
// ****************************************************************************
//
void RMemoryMappedStream::FillWithByte( const uBYTE ubPattern, const YStreamLength yLength )
	{
	TpsAssert( TestFileMapping(), "No File Mapping" );

	//	make room for the new bytes
	YStreamPosition yOldMarker = m_yMarker;
	Grow( yLength );
	m_yMarker = yOldMarker;
	
	//	write them
	memset( m_pFileBase + m_yMarker, ubPattern, yLength ); 
	m_yMarker += yLength;
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Flush( )
//
// Description:		flush file's buffers to disk
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Flush( )
	{
	TpsAssert( NULL != m_hFileMapping, "File not mapped" );
	TpsAssert( m_eOpenAccess != kRead, "Attempt to write to read only file" );
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );
	if ( !::FlushViewOfFile( m_pFileBase, 0  ) )	//	flush entire file
		ThrowFileExceptionIfError();	
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMemoryMappedStream::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RMemoryMappedStream, this );
	}

#endif	// TPSDEBUG
