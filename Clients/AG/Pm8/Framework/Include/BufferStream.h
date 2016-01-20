// ****************************************************************************
//
//  File Name:			BufferStream.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RBufferStream class
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
//  $Logfile:: /PM8/Framework/Include/BufferStream.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_BUFFERSTREAM_H_
#define		_BUFFERSTREAM_H_

#ifndef		_STREAM_H_
#include		"Stream.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
// Class Name:			RBufferStream
//
// Description:		Class for managine a buffer of memory for a read/write stream
//							of data
//
// ****************************************************************************
//
class FrameworkLinkage RBufferStream : public RStream
	{
	// Construction, destruction & Initialization
	public :
												RBufferStream( YStreamLength growSize = kMemoryExtra );
												RBufferStream( LPUBYTE pBuffer, YStreamLength len );
		virtual								~RBufferStream( );

		// Operations
	public :
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer );
		virtual void						Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer );
		virtual YStreamPosition			GetPosition( ) const;
		virtual void						SeekAbsolute( const YStreamLength ySeekPosition );
		virtual void						SeekRelative( const YStreamOffset ySeekOffset );
		virtual void						Flush( );
		virtual void						Copy( RStream* pStream );
		virtual void						Copy( RStream* pStream, const YStreamLength yLength );
		virtual YStreamLength			GetSize() const;
		virtual void						FillWithByte( const uBYTE ubPattern, const YStreamLength yLength );
		virtual void						Grow( const YStreamLength yLength );
		virtual PUBYTE						GetBuffer( const YStreamLength yLength );
		virtual void						ReleaseBuffer( PUBYTE pBuffer ) const;
		virtual BOOLEAN					StreamExists( ) const;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	

		// Member data
	private :
		BOOLEAN								m_fAllocated;
		LPUBYTE								m_pData;
		YStreamLength						m_DataLength;
		YStreamLength						m_StreamLength;
		YStreamPosition					m_Position;
		YStreamLength						m_GrowSize;
	};



// ****************************************************************************
// 					Inlines
// ****************************************************************************


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _BUFFERSTREAM_H_
