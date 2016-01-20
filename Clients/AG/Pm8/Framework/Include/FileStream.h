// ****************************************************************************
//
//  File Name:			FileStream.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas, Greg Beddow
//
//  Description:		Declaration of the RFileStream class
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
//  $Logfile:: /PM8/Framework/Include/FileStream.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FILESTREAM_H_
#define		_FILESTREAM_H_

#ifndef		_MEMORY_MAPPED_STREAM_H_
#include		"MemoryMappedStream.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class XPlatformFileID;

// ****************************************************************************
//
// Class Name:			RFileStream
//
// Description:		Stream class for files
//
// ****************************************************************************
//
class FrameworkLinkage RFileStream : public RMemoryMappedStream
	{
	// Construction, destruction & Initialization
	public :
											RFileStream( );	//	for compatibility with existing code
											RFileStream( const RMBCString& rFilename );	
											RFileStream( const RFileStream& rFileStream );	
		virtual							~RFileStream( );

	// Operations
	public :
		virtual void					Open( EAccessMode eOpenMode );
		virtual void					Open( const RMBCString& rFilename, EAccessMode access );	//	for compatibility with existing code
#ifdef MAC
		virtual void					Open( const char* szFileName, EAccessMode access );	//	for compatibility with existing code
		virtual void					Open( const FSSpecPtr pFileSpec, EAccessMode access );	//	for compatibility with existing code
#endif
		virtual void					GetFileIdentifier( XPlatformFileID& xpID );
		virtual void					CreateTempFile( );
		virtual void					CreateTempFile( const RMBCString& rPath );
		virtual void					CreateTempFile( const XPlatformFileID& xpID );
		virtual void					Rename ( const RMBCString& rFilename );
		virtual void					Rename ( const XPlatformFileID& xpID );
		virtual void					Delete( );
		virtual void					DeleteOnClose( BOOLEAN fDelete = TRUE );

		virtual void					Close( );

		static void						TestFileWritable( const RMBCString& filename );

	// Implementation
	private :

	// Member data
	private :

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

#if !defined MAC

class XPlatformFileID : public RFileStream 
	{
	};

#endif

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _FILESTREAM_H_
