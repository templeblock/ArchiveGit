// *****************************************************************************
//
// File name:			MacMemoryMappedBuffer.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RMemoryMappedBuffer class.
//
// Portability:		Mac only
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef MAC

#include "MemoryMappedBuffer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RMemoryMappedBuffer::RMemoryMappedBuffer()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RMemoryMappedBuffer::RMemoryMappedBuffer()
{
}

// ****************************************************************************
//
//  Function Name:	RMemoryMappedBuffer::RMemoryMappedBuffer()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RMemoryMappedBuffer::~RMemoryMappedBuffer()
{
}

// ****************************************************************************
//
//  Function Name:	RMemoryMappedBuffer::Resize()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RMemoryMappedBuffer::Resize(uLONG uBufferSize)
{
}

// ****************************************************************************
//
//  Function Name:	RMemoryMappedBuffer::Empty()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RMemoryMappedBuffer::Empty()
{
}

// ****************************************************************************
//
//  Function Name:	RTempFileBuffer::RTempFileBuffer()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RTempFileBuffer::RTempFileBuffer()
{
}

// ****************************************************************************
//
//  Function Name:	RPageFileBuffer::RPageFileBuffer()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RPageFileBuffer::RPageFileBuffer()
{
}

// ****************************************************************************
//
//  Function Name:	RImportFileBuffer::RImportFileBuffer()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImportFileBuffer::RImportFileBuffer(const RMBCString& rPath)
{
}

// ****************************************************************************
//
//  Function Name:	RExportFileBuffer::RExportFileBuffer()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RExportFileBuffer::RExportFileBuffer(const RMBCString& rPath)
{
}

#endif //MAC