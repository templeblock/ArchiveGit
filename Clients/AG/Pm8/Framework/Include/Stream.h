// ****************************************************************************
//
//  File Name:			Stream.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RStream class
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
//  $Logfile:: /PM8/Framework/Include/Stream.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_STREAM_H_
#define		_STREAM_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const uBYTE kExtraSpaceFillByte = '#';

//	local fcns
void FrameworkLinkage ThrowFileException( uLONG ulErrorCode );
void FrameworkLinkage ThrowFileExceptionIfError( );

// ****************************************************************************
//
// Class Name:			RStream
//
// Description:		Class to encapsulate streamable data sources, such as
//							files, memory blocks, or networks. This is an abstract
//							base class; specific implementations should derived from
//							RStream.
//
// ****************************************************************************
//
class FrameworkLinkage RStream : public RArchive
	{
	// Construction, destruction & Initialization
	public :
												RStream( );
		virtual								~RStream( );

			// Enums
	public :
		enum EExtraSize { kMemoryExtra = 4096, kDiskExtra = 32768 };			//	extra amount to allocate for different types of streams

		// Operations
	public :
		static void							SystemStartup( );
		static void							SystemShutdown( );
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer ) = 0;
		virtual void						Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer ) = 0;
		virtual YStreamPosition			GetPosition( ) const = 0;
		virtual void						SeekAbsolute( const YStreamLength ySeekPosition ) = 0;
		virtual void						SeekRelative( const YStreamOffset ySeekOffset ) = 0;
		virtual void						Flush( ) = 0;
		virtual void						Close( );
		virtual void						Open( EAccessMode eOpenMode );
		virtual void						Copy( RStream* pStream ) = 0;
		virtual void						Copy( RStream* pStream, const YStreamLength yLength ) = 0;
		virtual BOOLEAN					AtEnd( ) const;
		virtual YStreamLength			GetSize() const = 0;
		virtual void						FillWithByte( const uBYTE ubPattern, const YStreamLength yLength ) = 0;
		virtual void						Grow( const YStreamLength yLength ) = 0;
		virtual PUBYTE						GetBuffer( const YStreamLength yLength ) = 0;
		virtual void						ReleaseBuffer( PUBYTE pBuffer ) const = 0;
		virtual BOOLEAN					StreamExists( ) const = 0;
		virtual EAccessMode				GetAccessMode( );
		void									WriteFailed( const BOOLEAN fDeleteOnClose );
		virtual void						PreserveFileDate( const BOOLEAN fPreserveFileDate = TRUE );
		virtual BOOLEAN					ShouldPreserveFileDate( );

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	
	// Implementation
	protected :
	
		// Member data
	private :

	protected :
		EAccessMode							m_eOpenAccess;					//	stream access mode
		BOOLEAN								m_fCanRandomAccess;			//	TRUE: the stream supports random access
		BOOLEAN								m_fDeleteOnClose; 			// TRUE: the file backing the stream should be deleted when the stream is closed
		BOOLEAN								m_fPreserveFileDate;			//	TRUE: the file creation and modification dates of the file backing the stream should be preserved when the file is closed
	};


	class FrameworkLinkage ROpenStreamInfo
		{
		public:
			ROpenStreamInfo( );
			ROpenStreamInfo( RStream* pStream, BOOLEAN fOwner = FALSE );

			RStream* m_pStream;
			BOOLEAN	m_fOwner;
		};

	class ROpenStreamList : public RArray<ROpenStreamInfo> { };

//	Define the open streams list accessor to use in the file test project when we can't use
//	the one in Renaissance application globals.
#ifdef kFILETESTPROJECT
	ROpenStreamList*& GetOpenStreamList( );
#endif

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _STREAM_H_
