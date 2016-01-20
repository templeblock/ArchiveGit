// ****************************************************************************
//
//  File Name:			MemoryMappedStream.h
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene, Greg Beddow
//
//  Description:		Implementation of the RMemoryMappedStream class
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
//  $Logfile:: /PM8/Framework/Include/MemoryMappedStream.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MEMORY_MAPPED_STREAM_H_
#define		_MEMORY_MAPPED_STREAM_H_

#ifndef		_STREAM_H_
#include		"Stream.h"
#endif

#if 0 // GCB 10/18/96
#ifdef		MAC
#include		"LStream.h"
#endif
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

void ThrowFileExceptionIfTesting( uLONG ulError );

#ifdef MAC
void ThrowFileExceptionIfError( );
void ThrowFileException( uLONG ulError );

class XPlatformFileID;
#endif

//	this file handle identifies a file living in the system paging file
//	for RMemoryStream
#ifdef _WINDOWS
const HANDLE hUseSystemPageFile = (HANDLE)0xFFFFFFFF;
#endif

// ****************************************************************************
//
// Class Name:			FileTimeInfo
//
// Description:		Record a file's date info.
//
// ****************************************************************************
//
struct FileTimeInfo
{
#ifdef _WINDOWS
	FileTimeInfo( );
	FILETIME m_fileCreationDate;
	FILETIME m_fileLastAccessDate;
	FILETIME m_fileLastWriteDate;
	BOOLEAN m_fFileDatesValid;
#endif
};

// ****************************************************************************
//
// Class Name:			RMemoryMappedStream
//
// Description:		Class to encapsulate streamable data sources, such as
//							files, memory blocks, or networks using win32 memory mapped files.
//
// ****************************************************************************
//
class FrameworkLinkage RMemoryMappedStream : public RStream
	{
	// Construction, destruction & Initialization
	public :
												RMemoryMappedStream( );
												RMemoryMappedStream( const RMBCString& rFilename );	
												RMemoryMappedStream( const RMemoryMappedStream& rMemoryStream );	
		virtual								~RMemoryMappedStream( );

			// Enums
	public :

		// Operations
	public :
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer );
		virtual void						Write( YStreamLength yBytesToRead, const uBYTE* pBuffer );
		virtual YStreamPosition			GetPosition( ) const;
		virtual void						SeekAbsolute( const YStreamLength ySeekPosition );
		virtual void						SeekRelative( const YStreamOffset ySeekOffset );
		virtual void						Flush( );
		virtual void						Close( );
		virtual void						CloseRemap( BOOLEAN fRemapping = FALSE );
		virtual void						Open(EAccessMode eOpenMode);
#ifdef MAC // GCB 10/25/96 fix Mac compile error "RFileStream::Open(EAccessMode) hides inherited virtual function RMemoryMappedStream::Open(EAccesMode,unsigned long,int)"
		void									Open(EAccessMode eOpenMode, YStreamLength yMinSize, BOOLEAN fRemapping = FALSE );
#elif defined _WINDOWS
		virtual void						Open(EAccessMode eOpenMode, YStreamLength yMinSize, BOOLEAN fRemapping = FALSE );
#endif
		virtual YStreamLength			GetSize() const;
		virtual BOOLEAN					AtEnd( ) const;
		virtual void						Copy( RStream* pStream );
		virtual void						Copy( RStream* pStream, const YStreamLength yLength );
		virtual void						Copy( RMemoryMappedStream* pStream );
		virtual void						Copy( RMemoryMappedStream* pStream, const YStreamLength yLength );
		virtual void						FillWithByte( const uBYTE ubPattern, const YStreamLength yLength );
		virtual void						Grow( const YStreamLength yLength );
		virtual PUBYTE						GetBuffer( const YStreamLength yLength );
		virtual void						ReleaseBuffer( PUBYTE pBuffer ) const;
		BOOLEAN								StreamExists( ) const;
		RMBCString&							GetFilename( );
#ifdef _WINDOWS
		HANDLE								GetHandle( );
#endif
#ifdef MAC
		virtual sWORD						GetHandle( );
#endif

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	
	// Implementation
	private :

	protected :
		void									ResetMVars( BOOLEAN fClearName = TRUE );
		virtual YStreamLength			GetExtraSize() const;
		virtual void						RemoveFileExtraBuffer();
		void									RemapLarger( const YStreamPosition yNewFileEnd );
		void									GetFileTime( FileTimeInfo& fileTimeInfo );
		void									SetFileTime( const FileTimeInfo& fileTimeInfo );
#ifdef _WINDOWS
		virtual BOOLEAN					TestFileMapping( ) const;
		void									GetOpenMode( EAccessMode eOpenMode, uLONG& ulDesiredAccess, uLONG& ulProtection, uLONG& ulMapAccess, uLONG& ulCreateDist );
		virtual void						CreateFile( LPCTSTR lpFileName, DWORD dwDesiredAccess
													, DWORD dwShareMode
													, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDistribution
													, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ); 
#endif
#ifdef MAC
		// GCB 10/25/96 since the Mac doesn't have memory mapped files, implement these as
		// protected member functions here but expose them as public in subclass RFileStream
		// so the public API of both classes is (almost) the same for both Mac and Windows

		virtual void						Open( const char* szFileName, EAccessMode access );	//	for compatibility with existing code
		virtual void						Open( const RMBCString& rFilename, EAccessMode access );	//	for compatibility with existing code
		virtual void						Open( const FSSpecPtr pFileSpec, EAccessMode access );	//	for compatibility with existing code
		virtual void						GetFileIdentifier( XPlatformFileID& xpID );
		virtual void						CreateTempFile( );
		virtual void						CreateTempFile( const XPlatformFileID& xpID );
		virtual void						CreateTempFile( const RMBCString& rPath );
		virtual void						Rename ( const RMBCString& rFilename );
		virtual void						Rename ( const XPlatformFileID& xpID );
		virtual void						Delete( );
		virtual void						DeleteOnClose( BOOLEAN fDelete = TRUE );

		// GCB 10/25/96 internal function that might need to be overridden in subclass
		virtual void						CloseFileRef( );

public:
		static void							ThrowFileExceptionIfError( );
#endif
	
		// Member data
	protected :
		RMBCString							m_rFilename;		//	name of file backing stream

#ifdef _WINDOWS
		HANDLE								m_hFileHandle;		//	file mapping handle for memory mapped files
		uLONG									m_ulAllocGran;		//	saved system info including allocation granularity
		HANDLE								m_hFileMapping;		//	base ptr of file when mapped into our address space
		YStreamLength						m_ySize;			//	currently used size of file
		YStreamLength						m_yAllocSize;		//	currently allocated size of file
		PUBYTE								m_pFileBase;		//	pointer to base of the memory in the mapped file
		YStreamPosition					m_yMarker;			//	current file pointer
#endif

#ifdef MAC
		FSSpec								m_fileSpec;			//	spec of file backing stream
		sWORD									m_FileRef;
		static uWORD						m_nextTmpFile;		// to help create uniquely named tmp files

public:
		static OSErr						m_LastError;		//	last error encountered
#endif // MAC
	};

#ifdef MAC

class XPlatformFileID : public RMemoryMappedStream 
	{
	};

#endif

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
// Function Name:		RMemoryMappedStream::TestFileMapping( )
//
// Description:		
//
// Returns:				whether the file mapping member is valid
//
// Exceptions:			none
//
// ****************************************************************************
//
#ifdef _WINDOWS
inline BOOLEAN RMemoryMappedStream::TestFileMapping( ) const
	{
	return static_cast<BOOLEAN>( (INVALID_HANDLE_VALUE != m_hFileMapping) && (NULL != m_hFileMapping) );
	}
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _MEMORY_MAPPED_STREAM_H_
