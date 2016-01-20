// *****************************************************************************
//
// File name:			WinMemoryMappedBuffer.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RMemoryMappedBuffer class.
//
// Portability:		Windows only
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

#ifdef _WINDOWS

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
	m_hFile = INVALID_HANDLE_VALUE;
	m_hFileMapping = NULL;
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
	Empty();
	if (m_hFile != INVALID_HANDLE_VALUE)
		VERIFY(::CloseHandle(m_hFile));
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
	// Clear the current contents of the buffer
	Empty();

	// If the user specified 0 for the buffer size, return
	if (uBufferSize == 0) return;
	
	HANDLE hFileMapping = NULL;	
	void* pFileView = NULL;	
	try
	{
		hFileMapping = ::CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, uBufferSize, NULL);		
		if (!hFileMapping) ::ThrowExceptionOnOSError();

		pFileView = ::MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);		
		if (!pFileView) ::ThrowExceptionOnOSError();

		// Assign our members		
		m_hFileMapping = hFileMapping;		
		m_pBuffer = (uBYTE*)pFileView;
		m_uBufferSize = uBufferSize;
	}
	catch (YException)
	{
		if (pFileView) 
			VERIFY(::UnmapViewOfFile(pFileView));		
		if (hFileMapping) 
			VERIFY(::CloseHandle(hFileMapping));
		throw;
	}	
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
	if (m_pBuffer) 
	{
		VERIFY(::FlushViewOfFile(m_pBuffer, 0));
		VERIFY(::UnmapViewOfFile(m_pBuffer));
	}
	if (m_hFileMapping) 
		VERIFY(::CloseHandle(m_hFileMapping));
	m_pBuffer = NULL;
	m_hFileMapping = NULL;
	m_uBufferSize = 0;
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
RTempFileBuffer::RTempFileBuffer(BOOLEAN fDeleteOnClose)
{
	// Get the next available temp file
	CString cTempFilePath = GetNextTempFile();
	m_rTempFilePath = RMBCString(cTempFilePath);	
		
	// Get a handle to the file
	DWORD dwCreateFlags = 0;
	dwCreateFlags |= (fDeleteOnClose) ? FILE_FLAG_DELETE_ON_CLOSE : 0;	
	m_hFile = ::CreateFile(cTempFilePath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		dwCreateFlags,
		NULL);
	if (m_hFile == INVALID_HANDLE_VALUE) ::ThrowExceptionOnOSError();
}

// ****************************************************************************
//
//  Function Name:	GetNextTempFile()
//
//  Description:		Returns the full path to a unique temporary file name
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
CString RTempFileBuffer::GetNextTempFile()
{	
	char pTempDirectory[MAX_PATH + 1];
	char pTempPath[MAX_PATH + 1];
	::GetTempPath(MAX_PATH, pTempDirectory);
	::GetTempFileName(pTempDirectory, "IMG", 0, pTempPath);
	
	return (CString)pTempPath;
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
	m_hFile = INVALID_HANDLE_VALUE;
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
	// Convert RMBCString to a char*
	LPCSZ pPath = (LPCSZ)rPath;

	try
	{
		// Get a handle to the file
		m_hFile = ::CreateFile(pPath,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (m_hFile == INVALID_HANDLE_VALUE) ::ThrowExceptionOnOSError();

		m_uBufferSize = ::GetFileSize(m_hFile, NULL);

		m_hFileMapping = ::CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, m_uBufferSize, NULL);
		if (!m_hFileMapping) ::ThrowExceptionOnOSError();

		m_pBuffer = (uBYTE*)::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
		if (!m_pBuffer) ::ThrowExceptionOnOSError();
	}
	catch (YException)
	{
		if (m_pBuffer) 
			VERIFY(::UnmapViewOfFile(m_pBuffer));		
		if (m_hFileMapping) 
			VERIFY(::CloseHandle(m_hFileMapping));
		if (m_hFile != INVALID_HANDLE_VALUE)
			VERIFY(::CloseHandle(m_hFile));
		throw;
	}	
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
	// Convert RMBCString to a char*
	LPCSZ pPath = (LPCSZ)rPath;

	try
	{
		// Get a handle to the file
		m_hFile = ::CreateFile(pPath,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
			int i = 0; //j ::ThrowExceptionOnOSError();
		
		m_hFileMapping = NULL;
		m_pBuffer = NULL;
	}
	catch (YException)
	{
		if (m_pBuffer) 
			VERIFY(::UnmapViewOfFile(m_pBuffer));	
		if (m_hFileMapping) 
			VERIFY(::CloseHandle(m_hFileMapping));
		if (m_hFile != INVALID_HANDLE_VALUE)
			VERIFY(::CloseHandle(m_hFile));
		throw;
	}	
}

#endif // _WINDOWS