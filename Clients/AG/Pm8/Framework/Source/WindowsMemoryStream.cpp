// ****************************************************************************
//
//  File Name:			WindowsMemoryStream.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the Windows version of the RMemoryStream class
//
//  Portability:		Win32 specific
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
//  $Logfile:: /PM8/Framework/Source/WindowsMemoryStream.cpp                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "MemoryStream.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
// Function Name:		RMemoryStream::~RMemoryStream( )
//
// Description:		dtor
//
// Returns:				nothing
//
// Exceptions:			File exceptions
//
// ****************************************************************************
//
RMemoryStream::~RMemoryStream( )
	{
	NULL;
	}

// ****************************************************************************
//
// Function Name:		RMemoryStream::RMemoryStream( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			File exceptions
//
// ****************************************************************************
//
RMemoryStream::RMemoryStream( )
	{
	NULL;
	}


// ****************************************************************************
//
// Function Name:		RMemoryStream::CreateFile( )
//
// Description:		save the file handle that will place our data in the system paging file
//
// Returns:				none
//
// Exceptions:			none
//
// ****************************************************************************
//
void RMemoryStream::CreateFile( LPCTSTR /*lpFileName*/, DWORD dwDesiredAccess
						, DWORD dwShareMode
						, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD  dwCreationDistribution
						, DWORD  dwFlagsAndAttributes, HANDLE  hTemplateFile ) 
	{	
	if ( m_rFilename.IsEmpty() )			//	if we aren't recreating the file during a remaplarger then
		CreateTempFile();						//	fill in the file name with a temp name determined by the os
	m_yAllocSize = GetExtraSize();
	RMemoryMappedStream::CreateFile( m_rFilename, dwDesiredAccess, dwShareMode, lpSecurityAttributes
		, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
	}

// ****************************************************************************
//
// Function Name:		RMemoryStream::GetExtraSize( )
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
YStreamLength RMemoryStream::GetExtraSize( ) const
	{
	return RStream::kMemoryExtra;
	}
