// ****************************************************************************
//
//  File Name:			MemoryMappedBuffer.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of various memory mapped buffer class
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
//  $Logfile:: /PM8/Framework/Include/MemoryMappedBuffer.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _MEMORYMAPPEDBUFFER_H_
#define _MEMORYMAPPEDBUFFER_H_

#ifdef MAC
#error This file is Windows-only.
#endif

#ifndef _BUFFER_H_
#include "Buffer.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef uLONG	YBufferHandle;

// ****************************************************************************
//
//		RMemoryMappedBuffer
//
// ****************************************************************************
class FrameworkLinkage RMemoryMappedBuffer : public RBuffer
{
public:
	// Construction/Destruction
								RMemoryMappedBuffer();
	virtual					~RMemoryMappedBuffer();

	// Operations
	virtual void			Resize(uLONG uBufferSize);
	virtual void			Empty();

	// Accessors
	YBufferHandle			GetBufferHandle() {return (YBufferHandle)m_hFileMapping;};

protected:	
	// Windows handles to the file and the file mapping
	HANDLE					m_hFile;
	HANDLE					m_hFileMapping;	
};

// ****************************************************************************
//
//		RTempFileBuffer
//
// ****************************************************************************
class FrameworkLinkage RTempFileBuffer : public RMemoryMappedBuffer
{
public:	
								RTempFileBuffer(BOOLEAN fDeleteOnClose = TRUE);

	// Accessors
	const RMBCString&		GetTempFilePath() {return m_rTempFilePath;}

private:
	RMBCString				m_rTempFilePath;

	// Static members
protected:
	static CString			GetNextTempFile();
};


// ****************************************************************************
//
//		RPageFileBuffer
//
// ****************************************************************************
class FrameworkLinkage RPageFileBuffer : public RMemoryMappedBuffer
{
public:
							RPageFileBuffer();
};


// ****************************************************************************
//
//		RImportFileBuffer
//
// ****************************************************************************
class FrameworkLinkage RImportFileBuffer : public RMemoryMappedBuffer
{
public:
	// Construction
								RImportFileBuffer(const RMBCString& rPath);

	// Operations
	virtual void			Resize(uLONG) {throw ::kAccessDenied;}
};


// ****************************************************************************
//
//		RExportFileBuffer
//
// ****************************************************************************
class FrameworkLinkage RExportFileBuffer : public RMemoryMappedBuffer
{
public:
	// Construction
								RExportFileBuffer(const RMBCString& rPath);
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_MEMORYMAPPEDBUFFER_H_