// ****************************************************************************
//
//  File Name:			FileStream.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMemoryStream class
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
//  $Logfile:: /PM8/Framework/Include/MemoryStream.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MEMORYSTREAM_H_
#define		_MEMORYSTREAM_H_

#ifndef		_FILESTREAM_H_
#include		"FileStream.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
// Class Name:			RMemoryStream
//
// Description:		Stream class for files
//
// ****************************************************************************
//
class FrameworkLinkage RMemoryStream : public RFileStream
	{
	// Construction, destruction & Initialization
	public :
												RMemoryStream( );
		virtual								~RMemoryStream( );

			// Enums
	public :

	// Operations
	public :
		virtual YStreamLength			GetExtraSize() const;

	// Implementation
	private :

	protected :
#ifdef _WINDOWS
		virtual BOOLEAN					TestFileMapping( ) const;
		virtual void						CreateFile( LPCTSTR lpFileName, DWORD dwDesiredAccess
													, DWORD dwShareMode
													, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDistribution
													, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ); 
#endif

	// Member data
	private :
	};


// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
// Function Name:		RMemoryStream::TestFileMapping( )
//
// Description:		
//
// Returns:				whether the file mapping member is valid; memory mapped
//								files backed by the system paging file don't use
//								meaningful mapping members
//
// Exceptions:			none
//
// ****************************************************************************
//
#ifdef _WINDOWS
inline BOOLEAN RMemoryStream::TestFileMapping( ) const
	{
	return TRUE;
	}
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _MEMORYSTREAM_H_
