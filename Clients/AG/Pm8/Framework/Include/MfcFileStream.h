// ****************************************************************************
//
//  File Name:			MfcFileStream.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMfcFileStream class
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
//  $Logfile:: /PM8/Framework/Include/MfcFileStream.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MFCFILESTREAM_H_
#define		_MFCFILESTREAM_H_

#ifndef		_STREAM_H_
#include		"Stream.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
// Class Name:			RMfcFileStream
//
// Description:		Class to encapsulate an MFC CFile as a stream.
//
// ****************************************************************************
//
class FrameworkLinkage RMfcFileStream : public RStream
	{
	// Construction, destruction & Initialization
	public :
												RMfcFileStream( CFile& file );

	// Operations
	public :
		virtual void						Write( YStreamLength ulBytesToRead, const uBYTE* pBuffer );
		virtual void						Read( YStreamLength ulBytesToRead, uBYTE* pBuffer );
 		virtual YStreamPosition			GetPosition( ) const;
		virtual void						SeekAbsolute( const YStreamLength ySeekPosition );
		virtual void						SeekRelative( const YStreamOffset ySeekOffset );
		virtual void						Flush( );
		virtual void						Close( );
		virtual void						Open(EAccessMode eOpenMode);
		virtual YStreamLength			GetSize() const;
		virtual void						Copy( RStream* pStream );
		virtual void						Copy( RStream* pStream, const YStreamLength yLength );
		virtual void						FillWithByte( const uBYTE ubPattern, const YStreamLength yLength );
		virtual void						Grow( const YStreamLength yLength );
		virtual PUBYTE						GetBuffer( const YStreamLength yLength );
		virtual void						ReleaseBuffer( PUBYTE pBuffer ) const;
		virtual BOOLEAN					StreamExists( ) const;

	// Member data
	private :
		CFile*								m_pFile;
	
#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _MFCFILESTREAM_H_
