// ****************************************************************************
//
//  File Name:			MacMemMapFileStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene, Greg Beddow
//
//  Description:		Implementation of the RMemoryMappedStream class
//
//  Portability:		Macintosh
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
//  $Logfile:: /PM8/Framework/Source/MacMemMapFileStream.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include <stdio.h>

#include "FrameworkIncludes.h"

ASSERTNAME

#include "MemoryMappedStream.h"
#include "TPSDEBUG.H"

uWORD RMemoryMappedStream::m_nextTmpFile = 0;	// to help create uniquely named tmp files
OSErr	RMemoryMappedStream::m_LastError = 0;		//	last error encountered


// ****************************************************************************
//
// Function Name:		ThrowFileExceptionIfError( )
//
// Description:		Converts the error code of the last file operation to an
//							exception.
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
// ****************************************************************************
//
void ThrowFileExceptionIfError( )
	{
		RMemoryMappedStream::ThrowFileExceptionIfError();
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
		RMemoryMappedStream::m_LastError = (OSErr)ulError;
		RMemoryMappedStream::ThrowFileExceptionIfError();
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
void ThrowFileExceptionIfTesting( uLONG /* ulError */ )
	{
	// REVIEW GCB 10/18/96 not implemented on the Mac
#if 0
	UnimplementedCode();
#else
	NULL;
#endif
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::ResetMVars( )
//
// Description:		Clears member variables
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RMemoryMappedStream::ResetMVars( BOOLEAN fClearName )
	{
	if ( fClearName )
		m_rFilename.Empty();

	m_LastError = noErr;
	m_FileRef = 0;
	m_fDeleteOnClose = FALSE;
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
	// REVIEW GCB 10/18/96 not implemented on the Mac
	UnimplementedCode();
	return FALSE;
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
	{
	ResetMVars();
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::RMemoryMappedStream( const RMBCString& rFilename )
//
// Description:			Constructor with file name argument
//
// Returns:				nothing
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RMemoryMappedStream::RMemoryMappedStream( const RMBCString& rFilename )
	: m_rFilename(rFilename)
	{
	ResetMVars( FALSE );
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
		Close();
		}
	catch( ... )
		{
		NULL;	//	we can't throw from a destructor so we have nothing to do
		}
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
PUBYTE RMemoryMappedStream::GetBuffer( const YStreamLength /*yLength*/ )
	{
	UnimplementedCode();
	return 0;
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
	UnimplementedCode();
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
void RMemoryMappedStream::Grow( const YStreamLength /*yLength*/ )
	{
	UnimplementedCode();
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
		UnimplementedCode();
		return FALSE;
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
void RMemoryMappedStream::FillWithByte( const uBYTE /*ubPattern*/, const YStreamLength /*yLength*/ )
	{
	UnimplementedCode();
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
 	// REVIEW GCB 10/18/96 not implemented on the Mac
 	UnimplementedCode();
	return 0;
	}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::Open( EAccessMode eOpenMode )
{
	Open( eOpenMode, 0 );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::Open( EAccessMode eOpenMode, YStreamLength /* yMinSize */, BOOLEAN /* fRemapping */  )
{
	// GCB 10/25/96 IMPORTANT: use a temporary variable here to avoid aliasing problems
	char tmpFilename[256];
	strcpy( tmpFilename, LPCSZ(m_rFilename) );
	Open( tmpFilename, eOpenMode );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
//
// ****************************************************************************
//
void RMemoryMappedStream::Open( const char* szFilename, EAccessMode access )
{
	sBYTE 			cDesiredAccess;
	sWORD				fRefNum;
	BOOLEAN			fReplaceExisting = FALSE;

	// save the access mode
	m_eOpenAccess = access;

	switch( access )
	{
		case kRead :
			cDesiredAccess = fsRdPerm;
			break;

		case kWriteReplaceExisting :
		case kWrite :
			cDesiredAccess = fsRdWrPerm;
			break;

		case kReadWriteReplaceExisting :
		case kReadWrite :
			cDesiredAccess = fsRdWrPerm;
			break;

		default :
			TpsAssert( NULL, "Invalid access mode." );
			break;
	};

	// convert to pascal string
	char pascalFilename[256];
	::strcpy(pascalFilename, szFilename);
	::CtoPstr(pascalFilename);

	// convert to FSSpec
	m_LastError = ::FSMakeFSSpec( 0, 0, (const unsigned char *)pascalFilename, &m_fileSpec );

	if ((access == kReadWrite || access == kWrite) && m_LastError == fnfErr)
	{
		// file doesn't exist, so create it then convert to FSSpec
		// REVIEW GCB 10/24/96 creator and filetype?
		if ((m_LastError = ::HCreate( 0, 0, (const unsigned char *)pascalFilename, 'RENA', 'CHNK' )) != noErr)
			ThrowFileExceptionIfError( );
		m_LastError = ::FSMakeFSSpec( 0, 0, (const unsigned char *)pascalFilename, &m_fileSpec );
	}
	else if ( (access == kReadWriteReplaceExisting) || (access == kWriteReplaceExisting) )
	{
		//	Delete the existing file.
		//	It's ok to fail to delete the existing file so ignore any error here.
		::HDelete( m_fileSpec.vRefNum, m_fileSpec.parID, m_fileSpec.name );
	}

	if ( m_LastError != noErr )
		ThrowFileExceptionIfError( );

	m_LastError = ::HOpen( m_fileSpec.vRefNum, m_fileSpec.parID, m_fileSpec.name, cDesiredAccess, &fRefNum );
	if ( m_LastError != noErr )
		ThrowFileExceptionIfError( );

	m_FileRef = fRefNum;

	//	save given file name
	m_rFilename = szFilename;
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
//
// ****************************************************************************
//
void RMemoryMappedStream::Open( const FSSpecPtr pFileSpec, EAccessMode access )
{
sBYTE 			cDesiredAccess;
sWORD				fRefNum;
BOOLEAN			fReplaceExisting = FALSE;

	// save the access mode
	m_eOpenAccess = access;

	switch( access )
	{
		case kRead :
			cDesiredAccess = fsRdPerm;
			break;

		case kWriteReplaceExisting :
		case kWrite :
			cDesiredAccess = fsRdWrPerm;
			break;

		case kReadWriteReplaceExisting :
		case kReadWrite :
			cDesiredAccess = fsRdWrPerm;
			break;

		default :
			TpsAssert( NULL, "Invalid access mode." );
			break;
	};

	if ( (access == kReadWriteReplaceExisting) || (access == kWriteReplaceExisting) )
		{
		//	If the file exists delete it.
		FInfo fInfoUnused;
		if ( noErr == ::GetFInfo( pFileSpec->name, pFileSpec->vRefNum, &fInfoUnused ) )
			//	It's ok to fail to delete the existing file so ignore any error here.
			::HDelete( pFileSpec->vRefNum, pFileSpec->parID, pFileSpec->name );
		}

	m_LastError = ::HOpen( pFileSpec->vRefNum, pFileSpec->parID, pFileSpec->name, cDesiredAccess, &fRefNum);

	if ((access == kReadWrite || access == kWrite) && m_LastError == fnfErr)
	{
		// file doesn't exist, so create it then open it
		// REVIEW GCB 10/24/96 creator and filetype?
		if ((m_LastError = ::HCreate( pFileSpec->vRefNum, pFileSpec->parID, pFileSpec->name, 'RENA', 'CHNK' )) != noErr)
			ThrowFileExceptionIfError( );
		m_LastError = ::HOpen( pFileSpec->vRefNum, pFileSpec->parID, pFileSpec->name, cDesiredAccess, &fRefNum);
	}
	if ( m_LastError != noErr )
		ThrowFileExceptionIfError( );

	m_FileRef = fRefNum;

	m_fileSpec.vRefNum = pFileSpec->vRefNum;
	m_fileSpec.parID = pFileSpec->parID;
	::memcpy( m_fileSpec.name, pFileSpec->name, pFileSpec->name[0]+1 );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Open( )
//
// Description:		Opens a file
//
// Returns:				Nothing
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::Open( const RMBCString& rFilename, EAccessMode access )
{
	//	save given file name
	m_rFilename = rFilename;

	Open( access, 0 );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Read( )
//
// Description:		Reads data from this file
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
// ****************************************************************************
//
void RMemoryMappedStream::Read( YStreamLength ulBytesToRead, uBYTE* pBuffer )
{
	sLONG 			ulBytesRead = ulBytesToRead;

	TpsAssert( ( ( ulBytesToRead & 0xFF000000 ) == 0 ), "Can not read a block > 16MB!");
	TpsAssert( ( ulBytesToRead != 0 ), "Can not read a zero length block!");
	TpsAssert( m_FileRef != 0, "file is not open" );	
	if ( (m_LastError = ::FSRead( m_FileRef, &ulBytesRead, pBuffer )) != noErr )
		ThrowFileExceptionIfError( );
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

void RMemoryMappedStream::CloseRemap( BOOLEAN /* fRemapping */ )
	{
	// REVIEW GCB 10/18/96 not implemented on the Mac
	UnimplementedCode();
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Write( )
//
// Description:		Reads data from this file
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
	TpsAssert( 0 < yBytesToWrite, "Attempt to write zero bytes" );
	TpsAssert( 0xFFFFFF > yBytesToWrite, "Attempt to write more than 16MB" );
	
	TpsAssert( m_FileRef != 0, "file is not open" );	
	if ( (m_LastError = ::FSWrite( m_FileRef, (long *)&yBytesToWrite, pBuffer )) != noErr )
		ThrowFileExceptionIfError( );
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::SeekAbsolute( )
//
// Description:		Repositions the stream pointer to an absolute position
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
// ****************************************************************************
//
void RMemoryMappedStream::SeekAbsolute( YStreamPosition ulPosition )
{
	TpsAssert( m_FileRef != 0, "file is not open" );	
	if ( (m_LastError = ::SetFPos( m_FileRef, fsFromStart, ulPosition )) != noErr )
		ThrowFileExceptionIfError( );
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
	TpsAssert( m_FileRef != 0, "file is not open" );	
	if ( (m_LastError = ::SetFPos( m_FileRef, fsFromMark, ySeekOffset )) != noErr )
		ThrowFileExceptionIfError( );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetPosition( )
//
// Description:		Return the stream pointer as an absolute position
//
// Returns:				Nothing
//
//  Exceptions:		throws file exception
//
// ****************************************************************************
//
YStreamPosition RMemoryMappedStream::GetPosition( ) const
{
	///xxx	We want to change this to use a mutable variable or const_cast the member variable
	RMemoryMappedStream*	thisThing = (RMemoryMappedStream*)this;
	YStreamPosition	yPos;
	TpsAssert( m_FileRef != 0, "file is not open" );	
	if ( (thisThing->m_LastError = ::GetFPos( m_FileRef, (sLONG*)&yPos )) != noErr )
		ThrowFileExceptionIfError( );
	return yPos;
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
 	// REVIEW GCB 10/18/96 not implemented on the Mac
 	UnimplementedCode();
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
	TpsAssert( m_FileRef != 0, "file is not open" );	

	RMemoryMappedStream* thisPtr =
		dynamic_cast<RMemoryMappedStream*>(const_cast<RMemoryMappedStream*>(this));
	TpsAssert( thisPtr != NULL, "can't cast this pointer to non-const" );

	// return logical length of the file's data fork
	ParamBlockRec pb;
	pb.fileParam.ioVRefNum = 0;
	pb.fileParam.ioFVersNum = 0;
	pb.fileParam.ioFDirIndex = 0;
	char pascalFileName[256];
	strcpy( pascalFileName, LPCSZ(m_rFilename) );
	CtoPstr( pascalFileName );
	pb.fileParam.ioNamePtr = (unsigned char *)pascalFileName;
	if ( (thisPtr->m_LastError = ::PBGetFInfoSync( &pb )) != noErr )
		ThrowFileExceptionIfError( );
	return( pb.fileParam.ioFlLgLen );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Close( )
//
// Description:		close the file, then delete it if necessary
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Close( )
{
	CloseFileRef();

	if ( m_fDeleteOnClose )
		Delete( );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::CloseFileRef( )
//
// Description:		close the file specified by m_FileRef
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::CloseFileRef( )
{
	if ( m_FileRef != 0 )
	{
		if ( (m_LastError = ::FSClose( m_FileRef )) != noErr )
			ThrowFileExceptionIfError( );
		m_FileRef = 0;
	}
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Delete( )
//
// Description:		close the file if it's open, then delete it
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Delete( )
{
	// close the file if its open
	CloseFileRef();

	// delete the file
	char pascalFileName[256];
	strcpy( pascalFileName, LPCSZ(m_rFilename) );
	CtoPstr( pascalFileName );
	if ( (m_LastError = ::FSDelete( (const unsigned char *)pascalFileName, 0 )) != noErr )
		ThrowFileExceptionIfError( );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::DeleteOnClose( )
//
// Description:		remembers to delete the file backing this stream when it is closed
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::DeleteOnClose( BOOLEAN fDelete )
{
	m_fDeleteOnClose = fDelete;
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
	TpsAssert( m_FileRef != 0, "file is not open" );	

	ParamBlockRec pb;
	pb.ioParam.ioRefNum = m_FileRef;
	if ( (m_LastError = ::PBFlushFileSync( &pb )) != noErr )
		ThrowFileExceptionIfError( );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Copy( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Copy( RStream* pStream )
{
	TpsAssert( NULL != pStream, "Null Stream ptr" );

	Copy( pStream, pStream->GetSize() - pStream->GetPosition() );
}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Copy( RStream* pStream, const YStreamLength yLength )
//
// Description:		
//
// Returns:				
//
// Exceptions:			
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
	double qNewFilePos = (double)GetPosition() + (double)yLength;
	TpsAssert( ULONG_MAX > qNewFilePos, "Stream position overflowed" );

	//	copy into this stream from the given stream
	const YStreamLength kBufSize = 512;
	uBYTE buf[kBufSize];
	YStreamLength amountToCopy = 0;
	YStreamLength copiedSoFar = 0;
	for (; copiedSoFar < yLength; copiedSoFar += amountToCopy)
		{
		amountToCopy = Min( kBufSize, yLength - copiedSoFar );
		pMStream->Read( amountToCopy, buf );
		Write( amountToCopy, buf);
		}
	}

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetFileIdentifier( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::GetFileIdentifier( XPlatformFileID& xpID )
{
	xpID.m_rFilename				= m_rFilename;
#if 0
	// GCB 10/24/96 fix "illegal access to protected member" compile error on Mac
	xpID.m_eOpenAccess			= m_eOpenAccess;
	xpID.m_fCanRandomAccess		= m_fCanRandomAccess;
#endif
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::CreateTempFile( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::CreateTempFile( )
{
	char tmpFileName[20];
	sprintf( tmpFileName, "rTmpFile%d", m_nextTmpFile++ );
	RMBCString rPath( tmpFileName );

	CreateTempFile( rPath );

	//	dont leave these around
	DeleteOnClose();
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::CreateTempFile( const XPlatformFileID& xpID )
//
// Description:		make this a file with a temp name in directory specified by the file id
//
// Returns:				none
//
// Exceptions:			File Exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::CreateTempFile( const XPlatformFileID& /* xpID */ )
{	
	// REVIEW GCB 10/24/96 just create a tmp file in the current directory
	// rather than the one specified by xpID
	char tmpFileName[20];
	sprintf( tmpFileName, "rTmpFile%d", m_nextTmpFile++ );
	RMBCString rPath( tmpFileName );

	CreateTempFile( rPath );

	// don't delete on close
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::CreateTempFile( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::CreateTempFile( const RMBCString& rPath )
{
	Debug( ThrowFileExceptionIfTesting( kAccessDenied ) );

	// convert to C and pascal strings
	char cFilename[256], pascalFilename[256];
	::strcpy(cFilename, (LPCSZ)rPath);
	::strcpy(pascalFilename, cFilename);
	::CtoPstr(pascalFilename);

	// create the file
	// REVIEW GCB 10/15/96 creator and filetype?
	if ( (m_LastError = ::Create((const unsigned char *)pascalFilename, 0, 'RENA', 'CHNK')) != noErr )
		ThrowFileExceptionIfError( );

	// remember the name of the file
	m_rFilename = rPath;
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Rename(const RMBCString& rFilename )
//
// Description:		rename the disk file specified by rFilename; it must not be open
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Rename ( const RMBCString& rFilename )
{
	TpsAssert( m_FileRef == 0, "File to rename is open" );

	char oldPascalFileName[256], newPascalFileName[256];
	::strcpy( oldPascalFileName, LPCSZ(m_rFilename) );
	::CtoPstr( oldPascalFileName );
	::strcpy( newPascalFileName, LPCSZ(rFilename) );
	::CtoPstr( newPascalFileName );

	//	rename the file
	//	if the new file exists then delete it first
	m_LastError = ::FSDelete( (const unsigned char *)newPascalFileName, 0 );
	if (m_LastError != noErr && m_LastError != fnfErr)
		ThrowFileExceptionIfError( );
	if ((m_LastError = ::HRename( 0, 0, (const unsigned char *)oldPascalFileName,
		(const unsigned char *)newPascalFileName )) != noErr)
		ThrowFileExceptionIfError();

	//	remember the new name in the object
	m_rFilename = rFilename;
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::Rename(const XPlatformFileID& xpID )
//
// Description:		rename the disk file specified by xpID; it must not be open
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void RMemoryMappedStream::Rename ( const XPlatformFileID& xpID )
{
	Rename( xpID.m_rFilename );
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::GetHandle( )
//
// Description:		the files spec ptr
//
// Returns:				^
//
// Exceptions:			file
//
// ****************************************************************************
//
sWORD RMemoryMappedStream::GetHandle ( )
{
	UntestedCode();
#if 0 // REVIEW GCB 10/15/96 return m_FileRef instead of m_pFileSpec - is this function necessary?
	return m_pFileSpec;
#else
	return m_FileRef;
#endif
}


// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::ThrowFileExceptionIfError( )
//
// Description:		Converts the error code in m_LastError to an exception.
//
// Returns:				Nothing
//
// Exceptions:			File exceptions
//
// ****************************************************************************
//
void RMemoryMappedStream::ThrowFileExceptionIfError( )
	{
	switch ( m_LastError )
		{
		case noErr :
			break;

		case fnfErr :
			ThrowException( kFileNotFound );
			break;

   	case paramErr:
   	case dirNFErr:
			ThrowException( kInvalidPath );
			break;

   	case eofErr:
			ThrowException( kEndOfFile );
			break;

		case tmfoErr :
			ThrowException( kTooManyOpenFiles );
			break;

    	case fnOpnErr:
    	case bdNamErr:
    	case badUnitErr:
    	case unitEmptyErr:
    	case rfNumErr:
    	case nsvErr:
			ThrowException( kAccessDenied );
			break;

   	case dirFulErr:
			ThrowException( kDirectoryFull );
			break;

   	case dskFulErr:
			ThrowException( kDiskFull );
			break;

		case posErr :
			ThrowException( kBadSeek );
			break;

		case opWrErr :
			ThrowException( kShareViolation );
			break;

   	case fLckdErr:
   	case vLckdErr:
   	case wPrErr:
   	case fBsyErr:
   	case extFSErr:
			ThrowException( kLockViolation );
			break;

   	case ioErr:
		default :
			ThrowException( kGenericFile );
			break;
		}
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
