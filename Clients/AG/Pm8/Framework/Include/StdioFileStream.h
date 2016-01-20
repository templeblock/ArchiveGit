// ****************************************************************************
//
//  File Name:			StdioFileStream.h
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RCStdioFileStream class
//								This is a wrapper for buffered version of the MFC CFile 
//								routines. This code is used only in testing 
//								to verify that the memory mapped files 
//								implementation is faster than the c standard 
//								libraries buffered IO. It should not be used in Renaisance.
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/StdioFileStream.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STDIOFILE_STREAM_H_
#define		_STDIOFILE_STREAM_H_

#ifndef		_STREAM_H_
#include		"Stream.h"
#endif

// ****************************************************************************
//
// Class Name:			RCStdioFileStream
//
// Description:		Class to encapsulate streamable data sources, such as
//							files, memory blocks, or networks using win32 memory mapped files.
//
// ****************************************************************************
//
class FrameworkLinkage RCStdioFileStream : public RStream
	{
	// Construction, destruction & Initialization
	public :
												RCStdioFileStream( );
												RCStdioFileStream( const char* szFilename );
		virtual								~RCStdioFileStream( ){delete m_pFile; delete m_szFilename;}

			// Enums
	public :

		// Operations
	public :
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer );
		virtual void						Write( YStreamLength yBytesToWrite, uBYTE* pBuffer );
		virtual YStreamPosition			GetPosition( ) const;
		virtual void						SeekAbsolute( const YStreamLength ySeekPosition );
		virtual void						SeekRelative( const YStreamOffset ySeekOffset );
		virtual void						Flush( );
		virtual void						Close( );
		virtual void						Open(const char* szFilename, EAccessMode access);
		virtual void						Open(EAccessMode eOpenMode);
		void									ThrowFileExceptionIfError( );
		void									ThrowFileException( uLONG ulErrorCode )					{TpsAssert(FALSE, "exception thrown");}
		YStreamLength						GetSize();
	
#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	
	// Implementation
	private :

	protected :
		virtual void						InitMVars( BOOL fClearName = TRUE )							{m_pFile = NULL;}
	
		// Member data
	protected :
		char*									m_szFilename;		//	name of file backing stream

#ifdef _WINDOWS
		CStdioFile*							m_pFile;
		HANDLE								m_hFileHandle;		//	file mapping handle for memory mapped files
		uLONG									m_ulAllocGran;		//	saved system info including allocation granularity
		HANDLE								m_hFileMapping;	//	base ptr of file when mapped into our address space
		YStreamLength						m_ySize;				//	currently allocated size of file
		PUBYTE								m_pFileBase;		//	pointer to base of the memory in the mapped file
		YStreamPosition					m_yMarker;			//	current file pointer
#endif

#ifdef MAC
		OSErr									m_LastError;		//	last error encountered
		FSSpecPtr							m_pFileSpec;		//	spec of file backing stream; currently unused for RMemoryStream
		LStream*								m_pStream;			//	stream object; either LFileStream or LHandleStream
#endif

	protected :
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************
RCStdioFileStream::RCStdioFileStream( )
	{
	m_pFile = 0;
	m_szFilename = NULL;
	}

RCStdioFileStream::RCStdioFileStream( const char* szFilename )
	{
	m_szFilename = new char[strlen(szFilename)+1];
	memcpy(m_szFilename, szFilename, strlen(szFilename)+1);
	m_pFile = NULL;
	}	

// ****************************************************************************
//
// Function Name:		RCStdioFileStream::Read( )
//
// Description:		Reads data
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RCStdioFileStream::Read( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	m_pFile->Read( pBuffer, yBytesToRead );
	}

// ****************************************************************************
//
// Function Name:		RCStdioFileStream::Write( )
//
// Description:		Writes data
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RCStdioFileStream::Write( YStreamLength yBytesToRead, uBYTE* pBuffer )
	{
	m_pFile->Write( pBuffer, yBytesToRead );
	}

// ****************************************************************************
//
// Function Name:		RCStdioFileStream::SeekRelative( const YStreamOffset ySeekOffset )
//
// Description:		Reposition the file pointer based on the given relative position.
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RCStdioFileStream::SeekRelative( const YStreamOffset ySeekOffset )
	{
	m_pFile->Seek( ySeekOffset, CFile::current );
	}

// ****************************************************************************
//
// Function Name:		RCStdioFileStream::SeekAbsolute( const YStreamLength position )
//
// Description:		Reposition the file pointer to the given absolute position.
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RCStdioFileStream::SeekAbsolute( const YStreamLength yPosition )
	{
	m_pFile->Seek( yPosition, CFile::begin );
	}

// ****************************************************************************
//
// Function Name:		RCStdioFileStream::GetPosition( )
//
// Description:		GetPosition
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
YStreamPosition RCStdioFileStream::GetPosition( ) const
	{
	return m_pFile->GetPosition( );
	}



// ****************************************************************************
//
// Function Name:		RCStdioFileStream::Close( )
//
// Description:		close the file
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RCStdioFileStream::Close( )
	{
	m_pFile->Close( );
	}



// ****************************************************************************
//
// Function Name:		RCStdioFileStream::Open( EAccessMode eOpenMode )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCStdioFileStream::Open( EAccessMode eOpenMode )
	{
	UINT nOpenFlags;

	TpsAssert( NULL != m_szFilename, "Null filename" );
	if (NULL == m_pFile)
		m_pFile = new CStdioFile;

	switch(eOpenMode)
		{
		case kRead:
			nOpenFlags = CFile::modeRead | CFile::shareExclusive | CFile::typeBinary;
			break;
		case kWrite:
			nOpenFlags = CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::typeBinary;
			break;
		case kReadWrite:
			nOpenFlags = CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive | CFile::typeBinary;
			break;
		default:
				TpsAssert( FALSE, "invalid access mode" );
		}
	m_pFile->Open( m_szFilename, nOpenFlags );
	}



// ****************************************************************************
//
// Function Name:		RCStdioFileStream::Flush( )
//
// Description:		flush file's buffers to disk
//
// Returns:				nothing
//
// Exceptions:			mfc file exceptions
//
// ****************************************************************************
//
void RCStdioFileStream::Flush( )
	{
	m_pFile->Flush( );
	}


// ****************************************************************************
//
// Function Name:		RCStdioFileStream::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RCStdioFileStream::Validate( ) const
	{
	RStream::Validate( );
	TpsAssertIsObject( RCStdioFileStream, this );
	ASSERT_VALID( m_pFile );
	}


void RCStdioFileStream::Open(const char* szFilename, EAccessMode eOpenMode)
	{
	m_szFilename = new char[strlen(szFilename)+1];
	memcpy(m_szFilename, szFilename, strlen(szFilename)+1);
	Open( eOpenMode );
	}

YStreamLength RCStdioFileStream::GetSize()
	{
	return m_pFile->GetLength();
	}

#endif	// _CSTDIOFILE_STREAM_H_
