// ****************************************************************************
//
//  File Name:			UIContextData.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RUIContextData class
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
//  $Logfile:: /PM8/Framework/Include/UIContextData.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_UIContextData_H_
#define		_UIContextData_H_

#include "BufferStream.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RChunkStorage;

// ****************************************************************************
//
// Class Name:			RUIContextData
//
// Description:		Class to encapsulate data that object UI will need to save
//							and restore
//
// ****************************************************************************
//
class FrameworkLinkage RUIContextData : public RArchive
	{
	// Construction
	public :
												RUIContextData( );

	// Operations
	public :
		void									Read( RChunkStorage& storage );
		void									Write( RChunkStorage& storage ) const;

		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer );
		virtual void						Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer );
		virtual YStreamLength			GetSize( ) const;
		virtual YStreamLength			GetPosition( ) const;

		void									SeekAbsolute( const YStreamLength ySeekPosition );

	// Members
	private :
		RBufferStream						m_BufferStream;

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif	// TPSDEBUG
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _UIContextData_H_
