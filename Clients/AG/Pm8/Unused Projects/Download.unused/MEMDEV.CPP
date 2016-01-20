/*
// $Header: /PM8/Download/MEMDEV.CPP 1     3/03/99 6:15p Gbeddow $
//
// A simple memory-based device (read-only).
//
// $Log: /PM8/Download/MEMDEV.CPP $
// 
// 1     3/03/99 6:15p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:50   FRED
// Initial revision.
// 
//    Rev 2.2   13 Sep 1995 11:33:40   JAY
// New 32-bit stuff
// 
//    Rev 2.1   18 Aug 1995 15:06:00   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.0   07 Feb 1995 15:47:56   JAY
// Initial revision.
// 
//    Rev 1.0   10 Jan 1995 15:41:32   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "memdev.h"

#include <malloc.h>						// for _heapmin()

MemoryDevice::MemoryDevice(BOOL fOwnsPieces)
	: m_Pieces(sizeof(MEMORY_DEVICE_PIECE))
{
	m_Position = 0;
	m_Length = 0;
	m_fOwnsPieces = fOwnsPieces;
}

MemoryDevice::~MemoryDevice()
{
	if (m_fOwnsPieces)
	{
	// All pieces are assumed to be BYTE arrays.
		for (int nPiece = 0; nPiece < m_Pieces.count(); nPiece++)
		{
			delete [] (LPBYTE)((MEMORY_DEVICE_PIECE*)m_Pieces.get_element(nPiece))->m_pData;
		}
	// A little heap compaction test.
		_heapmin();
	}
}

void MemoryDevice::AddPiece(LPVOID pData, DWORD dwDataSize)
{
	MEMORY_DEVICE_PIECE Piece;

// Build the block. This is going on the end of the device.

	Piece.m_Start = m_Length;
	Piece.m_pData = pData;
	Piece.m_lSize = (long)dwDataSize;

// Add the block to our list.

	m_Pieces.add_element(&Piece);

// Update the length.

	m_Length += dwDataSize;
}

ERRORCODE MemoryDevice::initialize()
{
	return ERRORCODE_Access;
}

ERRORCODE MemoryDevice::read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read /*=NULL*/)
{
	if (m_Position < 0 || m_Position >= m_Length)
	{
		if (size_read != NULL)
		{
			*size_read = 0;
			return ERRORCODE_None;
		}
	/* Can't read from here. */
		return ERRORCODE_Read;
	}

	ST_DEV_POSITION Size = (ST_DEV_POSITION)size;

	if (m_Position + Size > m_Length)
	{
		if (size_read == NULL)
		{
		// Just not enough data to even try.
			return ERRORCODE_Read;
		}
		Size = m_Length - m_Position;
	}

	int nBlocks = m_Pieces.count();
	ST_DEV_IO_SIZE AmountRead = 0;			// How much we actually have read.

/*
// Prep to search for blocks.
// If we do any block "remembering", we can do it here to start further on.
*/

	int nBlock = 0;
	MEMORY_DEVICE_PIECE* pPiece = NULL;

/*
// Find the block the data starts in.
*/

	for (; nBlock < nBlocks; nBlock++)
	{
		pPiece = (MEMORY_DEVICE_PIECE*)m_Pieces.get_element(nBlock);

		if (pPiece->m_Start + pPiece->m_lSize > m_Position)
		{
			break;
		}
	}

	if (nBlock == nBlocks)
	{
		return ERRORCODE_IntError;
	}

/*
// Do the reading thing now.
*/

	LPBYTE pDest = (LPBYTE)ptr;

	while (Size != 0)
	{
		// Compute how far into this block we need to start reading.
		ST_DEV_POSITION Offset = m_Position - pPiece->m_Start;
		// Compute how much data is left in the block.
		ST_DEV_POSITION ReadSize = pPiece->m_lSize - Offset;
		if (ReadSize > Size)
		{
			ReadSize = Size;
		}

#ifdef WIN32
		BYTE* pData = (BYTE*)pPiece->m_pData;
#else
		BYTE huge* pData = (BYTE huge*)pPiece->m_pData;
#endif

		// Offset to data.
		pData += Offset;

		// Do the copy.
#ifdef WIN32
		memcpy(pDest, pData, ReadSize);
#else
		hmemcpy(pDest, pData, ReadSize);
#endif

		// Advance the destination pointer.
		pDest += (int)ReadSize;

		// Update the copy variables.
		m_Position += ReadSize;
		AmountRead += (ST_DEV_IO_SIZE)ReadSize;
		if ((Size -= ReadSize) == 0)
		{
			break;
		}

		// Advance to the next block. This 'if' is a sanity check only.
		if (++nBlock == nBlocks)
		{
			return ERRORCODE_IntError;
		}
		pPiece++;
	}

/* Report how much we read if that is desired. */

	if (size_read != NULL)
	{
		*size_read = AmountRead;
	}

/* No errors to report. */

	return ERRORCODE_None;
}

ERRORCODE MemoryDevice::write(LPVOID ptr, ST_DEV_IO_SIZE size)
{
	return ERRORCODE_Access;
}

ERRORCODE MemoryDevice::seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode)
{
	switch (mode)
	{
		case ST_DEV_SEEK_SET:
		{
			m_Position = position;
			break;
		}
		case ST_DEV_SEEK_CUR:
		{
			m_Position += position;
			break;
		}
		case ST_DEV_SEEK_END:
		{
			m_Position = m_Length + position;
			break;
		}
		default:
		{
			return ERRORCODE_BadParameter;
		}
	}
	return ERRORCODE_None;
}

ERRORCODE MemoryDevice::tell(ST_DEV_POSITION far *position)
{
	*position = m_Position;
	return ERRORCODE_None;
}

ERRORCODE MemoryDevice::length(ST_DEV_POSITION far *size)
{
	*size = m_Length;
	return ERRORCODE_None;
}

ERRORCODE MemoryDevice::truncate()
{
	return ERRORCODE_Access;
}

ERRORCODE MemoryDevice::flush()
{
	return ERRORCODE_None;
}

ERRORCODE MemoryDevice::io_limits(ST_DEV_POSITION limit_start /*=POSITION_UNKNOWN*/,
											 ST_DEV_POSITION limit_end /*=POSITION_UNKNOWN*/)
{
	return ERRORCODE_None;
}
