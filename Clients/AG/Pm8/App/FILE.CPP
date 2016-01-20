/*
// $Header: /PM8/App/FILE.CPP 1     3/03/99 6:05p Gbeddow $
//
// File handling routines.
//
// $Log: /PM8/App/FILE.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 6     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 5     6/24/98 11:00a Rlovejoy
// Fixed endless loop mistake.
// 
// 4     6/24/98 10:17a Rlovejoy
// Added code to strip out quotes in excel format text files.
// 
// 3     11/21/97 1:23p Johno
// Removed redundant Getposition in CStorageFile::tell.
// It always failed anyway
// 
// 2     11/21/97 11:01a Jayn
// Moved StorageFileOnCFile from dbmap.* to file.*
// 
//    Rev 1.0   14 Aug 1997 15:20:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:24   Fred
// Initial revision.
// 
//    Rev 1.22   12 Jun 1997 09:07:08   Jay
// zap() now checks for empty file name.
// 
//    Rev 1.21   15 May 1997 17:05:16   dennisp
// Added capability to ignore characters between brackets in read_line
// 
//    Rev 1.20   05 May 1997 14:45:08   Jay
// Fixed a bug in huge_read of a device with a based device (buffering messed
// up the file position).
// 
//    Rev 1.19   23 Apr 1997 12:08:08   Jay
// path manager changes for collection items
// 
//    Rev 1.18   17 Apr 1997 16:05:50   Jay
// StorageFile routines can be made to throw exceptions
// 
//    Rev 1.17   15 Apr 1997 15:44:04   Jay
// ifdef'd out an error message firing at the wrong time.
// 
//    Rev 1.16   07 Apr 1997 14:55:54   Jay
// Fixed a signed/unsigned problem.
// 
//    Rev 1.15   31 Mar 1997 17:11:58   Jay
// Bug fix for based device without subfile end set.
// 
//    Rev 1.14   13 Mar 1997 13:17:06   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.13   13 Jan 1997 16:17:38   dennisp
// Added some comments to ReadOnlyFile functions.
// 
//    Rev 1.12   13 Jan 1997 15:56:54   dennisp
// Now depending on m_current_position variable for file position.
// 
// 
//    Rev 1.11   13 Dec 1996 13:20:38   JAY
// Trivial variable renaming
// 
//    Rev 1.10   05 Nov 1996 14:06:46   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.9   04 Nov 1996 16:05:34   Jay
// Separated out name generation for TmpFile
// 
//    Rev 1.8   07 Oct 1996 17:03:02   Jay
// Got rid of some inefficiencies.
// 
//    Rev 1.7   02 Oct 1996 08:32:14   Jay
// 
//    Rev 1.6   03 Sep 1996 12:35:08   Jay
// Added a TRACE.
// 
//    Rev 1.5   01 Sep 1996 16:44:38   Jay
// Optimization in TextReadFile::read_line().
// 
//    Rev 1.4   25 Aug 1996 13:00:32   Jay
// Fixed a bug with buffering code.
// 
//    Rev 1.3   18 Jul 1996 18:46:14   Fred
//  
// 
//    Rev 1.2   17 Jul 1996 12:53:24   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.1   24 May 1996 16:16:00   Fred
// TRACEx
// 
//    Rev 1.0   14 Mar 1996 13:44:04   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:46   Jay
// Changes in 2.1
// 
//    Rev 2.12   27 Feb 1996 16:43:32   JAY
// New filename translation system including subfile support.
// 
//    Rev 2.11   14 Sep 1995 09:32:24   JAY
// 32-bit changes
// 
//    Rev 2.10   13 Sep 1995 11:33:50   JAY
// New 32-bit stuff
// 
//    Rev 2.9   21 Aug 1995 18:44:44   FRED
// More OLE fixes.
// file.cpp -> L:\PMW2\DATABASE\FILE.CPV
// Enter description of changes made.
// 
//    Rev 2.8   18 Aug 1995 15:04:28   JAY
// Static function to translate a file path name.
// 
//    Rev 2.7   10 Aug 1995 07:43:32   JAY
// Doesn't try to copy zero byte files.
// 
//    Rev 2.6   17 Jul 1995 15:07:52   JAY
// Changed the file buffering a little (for bigger-than-buffer reads).
// 
//    Rev 2.5   07 Jul 1995 16:10:38   JAY
// Misc changes.
// 
//    Rev 2.4   17 May 1995 15:38:56   JAY
// Fixed huge_write.
// 
//    Rev 2.3   13 May 1995 11:14:40   JAY
//  
// 
//    Rev 2.2   11 May 1995 09:38:24   JAY
// Moved huge_read to base class.
// 
//    Rev 2.1   25 Apr 1995 08:50:24   JAY
// Added some debugging triggers for the mysterious "substitute file copy".
// 
//    Rev 2.0   07 Feb 1995 15:47:44   JAY
// Initial revision.
// 
//    Rev 1.23   30 Nov 1994 16:34:06   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   11 Nov 1994 17:02:48   JAY
// Fixed a bug in the io limits stuff.
// 
//    Rev 1.2   11 Nov 1994 12:59:48   JAY
// Changed meaning of I/O limits during read. Now clips instead of outright error.
// 
//    Rev 1.1   07 Nov 1994 15:46:10   JAY
// Fixed reverse logic in set_based_device().
// 
//    Rev 1.0   03 Nov 1994 12:54:00   FRED
//  
// 
//    Rev 1.1   01 Nov 1994 10:00:04   JAY
// New data manager routines
// 
//    Rev 1.0   25 Oct 1994 16:24:32   JAY
// Initial revision.
// 
//    Rev 1.21   04 Aug 1994 07:33:20   JAY
// Large model and 32-bit changes
// 
//    Rev 1.20   13 Jul 1994 11:25:34   JAY
// size_string() now takes a LPCSTR.
// 
//    Rev 1.19   11 Jul 1994 11:14:48   JAY
// Changed write_string() to take an LPCSTR.
// 
//    Rev 1.18   20 Jun 1994 13:25:12   JAY
// Now always reads the length of the file, since it can now be changed by some
// other process in the shared case.
// 
//    Rev 1.17   14 Jun 1994 16:50:08   JAY
// Is SHARE is not loaded, the lock() function ignores the error.
// 
//    Rev 1.16   10 Jun 1994 09:53:40   JAY
// reset() now always flushes (completion routine is always called).
// 
//    Rev 1.15   09 Jun 1994 11:02:12   JAY
// Added reset().
// Added file locking methods.
// Added completion notify hooks (when file is flushed()).
*/

#include "stdafx.h"

#include "file.h"
#include "util.h"

#include <malloc.h>

/************************/
/* StorageFile routines */
/************************/

IPathManager* StorageFile::m_pPathManager = NULL;

int StorageFile::m_nRetryCount = 5;
DWORD StorageFile::m_dwRetryDelay = 1000;				// In milliseconds

CString StorageFile::GetPathName(BOOL fLocate)
{
	return TranslateFileName(m_csName, fLocate, this);
}

CString StorageFile::TranslateFileName(LPCSTR pName, BOOL fLocate, StorageFile* pFile /*=NULL*/)
{
	if (m_pPathManager == NULL)
	{
		return pName;
	}
	else
	{
		if (fLocate)
		{
			// Locate the path.
			return m_pPathManager->LocatePath(pName, pFile);
		}
		else
		{
			// Just expand the path.
			return m_pPathManager->ExpandPath(pName);
		}
	}
}

BOOL StorageFile::IsAssigned() const
{
	if (m_based_device == NULL)
	{
		return (!m_csName.IsEmpty());
	}
	else
	{
	/* Somebody has assigned us to something. */
		return TRUE;
	}
}

/**************************/
/* StorageDevice routines */
/**************************/

/*
// Read data from the file.
*/

ERRORCODE StorageDevice::huge_read(LPVOID ptr, DWORD size, DWORD *pdwSizeRead)
{
	ERRORCODE error;
	DWORD dwAmountRead;
	DWORD dwSizeLeft = size;

/* Do the read. */

#ifdef WIN32
	char* rp = (char*)ptr;
#else
	char huge *rp = (char huge *)ptr;
#endif
	dwAmountRead = 0;

	while (dwSizeLeft != 0)
	{
		unsigned uAmountToRead = (unsigned)__min(dwSizeLeft, 32767);

#ifndef WIN32
		DWORD dwBytesLeftInSeg = 0x10000 - (DWORD)OFFSETOF(rp);

		if ((DWORD)uAmountToRead > dwBytesLeftInSeg)
		{
			uAmountToRead = (WORD)dwBytesLeftInSeg;
		}
#endif

		ST_DEV_IO_SIZE uAmountReadNow;

		if ((error = read(rp, uAmountToRead, (ST_DEV_IO_SIZE *)&uAmountReadNow)) != ERRORCODE_None)
		{
			return error;
		}

	/* Update our reading state. */

		dwSizeLeft -= uAmountReadNow;
		rp += uAmountReadNow;
		dwAmountRead += uAmountReadNow;

	/* Probably ran out of data. */

		if (uAmountReadNow != uAmountToRead)
		{
			break;
		}
	}

/* Generate the return code. */

	if (pdwSizeRead != NULL)
	{
	/* Return the amount actually read with no error reported. */
		*pdwSizeRead = dwAmountRead;
		return ERRORCODE_None;
	}

	return (dwAmountRead == size) ? ERRORCODE_None : ERRORCODE_Read;
}

/*
// Write data from the file.
*/

ERRORCODE StorageDevice::huge_write(LPVOID ptr, DWORD size, DWORD *pdwSizeWritten)
{
	ERRORCODE error;
	DWORD dwAmountWritten = 0;
	DWORD dwSizeLeft = size;

/* Do the write. */

#ifdef WIN32
	char* wp = (char*)ptr;
#else
	char huge *wp = (char huge *)ptr;
#endif

	while (dwSizeLeft != 0)
	{
		unsigned uAmountToWrite = (unsigned)__min(dwSizeLeft, 32767);

		if ((error = write(wp, uAmountToWrite)) != ERRORCODE_None)
		{
			return error;
		}

	/* Update our writing state. */

		dwSizeLeft -= uAmountToWrite;
		wp += uAmountToWrite;
		dwAmountWritten += uAmountToWrite;
	}

/* Generate the return code. */

	if (pdwSizeWritten != NULL)
	{
	/* Return the amount actually written with no error reported. */
		*pdwSizeWritten = dwAmountWritten;
		return ERRORCODE_None;
	}

	return (dwAmountWritten == size) ? ERRORCODE_None : ERRORCODE_Write;
}

/*
// Move some data from one place to another.
*/

ERRORCODE StorageDevice::move(ST_DEV_POSITION src, ST_DEV_POSITION dst, ST_DEV_POSITION size)
{
	ERRORCODE error = ERRORCODE_None;
	char buffer[256];
	BOOL backward = FALSE;

/* See if we need to copy at all. */

	if (src == dst)
	{
		return ERRORCODE_None;
	}

/* See if we need to copy forward or backward. */

	if (dst > src && dst < src+size)
	{
	/* Overlap! We need to copy backward. */
		
		src += size;
		dst += size;

		backward = TRUE;
	}

/* Do the whole amount. */

	while (size != 0 )
	{
		ST_DEV_IO_SIZE io_size = sizeof(buffer);

		if ((ST_DEV_POSITION)io_size > size)
		{
			io_size = (ST_DEV_IO_SIZE)size;
		}

		if (backward)
		{
		/* Adjust the pointers to the start of the block. */

			src -= io_size;
			dst -= io_size;
		}

	/* Seek to the next source area. */

		if ((error = seek(src, ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
			break;
		}

	/* Read the next source data. */

		if ((error = read(buffer, io_size)) != ERRORCODE_None)
		{
			break;
		}

	/* Seek to the next destination area. */

		if ((error = seek(dst, ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
			break;
		}

	/* Write the next destination data. */

		if ((error = write(buffer, io_size)) != ERRORCODE_None)
		{
			break;
		}

	/* Update where we are. */

		if (!backward)
		{
			src += io_size;
			dst += io_size;
		}
		size -= io_size;
	}

	return error;
}

/*
// Read a far string.
// The string is allocated with a heap allocate.
// A string which was written with NULL is read as NULL; i.e. it's not
// allocated.
*/

ERRORCODE StorageDevice::read_string(LPSTR far *string)
{
	SHORT size;
	ERRORCODE error;
	LPSTR s = NULL;

	if ((error = read(&size, sizeof(size))) == ERRORCODE_None && size != 0)
	{
		error = read_block(size, (LPVOID far *)&s);
	}

	// Out with any old.

	delete [] *string;
	*string = s;
	return error;
}

/*
// Write a far string.
*/

ERRORCODE StorageDevice::write_string(LPCSTR string)
{
	SHORT size;
	ERRORCODE error;

	size = (string == NULL) ? 0 : (strlen(string)+1);

	if ((error = write(&size, sizeof(size))) == ERRORCODE_None)
	{
	/* Allocated. Read. */
		if (size != 0)
		{
			error = write((LPVOID)string, size);
		}
	}
	return error;
}

/*
// Return the size of a far string.
*/

SHORT StorageDevice::size_string(LPCSTR string)
{
	SHORT size = sizeof(size);

	if (string != NULL)
	{
		size += strlen(string) + 1;
	}
	return size;
}

/*
// Read a CString.
*/

ERRORCODE StorageDevice::read_cstring(CString& csString)
{
	SHORT size;
	ERRORCODE error;

	csString.Empty();

	if ((error = read(&size, sizeof(size))) == ERRORCODE_None && size != 0)
	{
		char* pBuffer = NULL;
		TRY
		{
			pBuffer = csString.GetBuffer(size);
			error = read(pBuffer, size);
		}
		CATCH_ALL(e)
		{
         if (pBuffer != NULL)
         {
            csString.ReleaseBuffer();
         }
         THROW_LAST();
		}
		END_CATCH_ALL
		csString.ReleaseBuffer();
	}

	return error;
}

/*
// Write a CString.
*/

ERRORCODE StorageDevice::write_cstring(const CString& csString)
{
	return write_string(csString);
}

/*
// Return the size of a cstring.
*/

SHORT StorageDevice::size_cstring(const CString& csString)
{
	return size_string(csString);
}

/*
// Read a block of data.
// The data is allocated from the far heap if the incoming pointer is not NULL.
// THIS MEANS: block MUST point somewhere valid or be NULL.
*/

ERRORCODE StorageDevice::read_block(ST_DEV_IO_SIZE size, LPVOID far *block)
{
	ERRORCODE error = ERRORCODE_None;
	LPVOID b;

	TRY
	{
		// Get our block.
		if ((b = *block) == NULL)
		{
			b = (LPVOID)(new char[size]);
		}

		// Allocated. Read.
		if (size == 0 || (error = read(b, size)) == ERRORCODE_None)
		{
			*block = b;
		}
	}
	CATCH_ALL(e)
	{
		// Failed! Only free if we allocated ourselves.
		if (*block == NULL)
		{
			delete [] (char *)b;
		}
      THROW_LAST();
	}
	END_CATCH_ALL

	// Handle error cleanup (non-exception case).
	if (error != ERRORCODE_None)
	{
		// Failed! Only free if we allocated ourselves.
		if (*block == NULL)
		{
			delete [] (char *)b;
		}
	}

	return error;
}

/*
// Read a sized record.
// The first word in the file is assumed to be the record size.
// Then comes the record.
// If the file record size is larger than the memory record size, only the
// memory record size is read, then the rest is skipped.
// If the file record size is smaller than the memory record size, only the
// file record size is read and the rest is filled with zeros.
// Either way, the size of the file record is returned if desired.
//
// The memory size of the file should not include the size word itself.
*/

ERRORCODE StorageDevice::read_record(LPVOID record, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE far *out_file_size)
{
	ST_DEV_IO_SIZE file_size;
	ERRORCODE error;

/* Read the size of the record. */

	if ((error = read(&file_size, sizeof(file_size))) == ERRORCODE_None)
	{
	/* Report our file size if requested. */

		if (out_file_size != NULL)
		{
			*out_file_size = file_size;
		}

	/* Read the data if there is any. */

		ST_DEV_IO_SIZE read_size = __min(file_size, size);

		if (read_size != 0)
		{
			error = read(record, read_size);
		}

	/* Zero remainder if larger than what was read. */

		if (size > file_size)
		{
		/* Blank the rest. */
			memset(((LPBYTE)record)+file_size, 0, size - file_size);
		}
		else if (file_size > size)
		{
		/* Skip the rest. */
			seek(file_size - size, ST_DEV_SEEK_CUR);
		}
	}
	return error;
}

/*
// Write a record to file.
// The size of the record is written first followed by the data.
*/

ERRORCODE StorageDevice::write_record(LPVOID record, ST_DEV_IO_SIZE size)
{
	ERRORCODE error;

	if ((error = write(&size, sizeof(size))) == ERRORCODE_None)
	{
		error = write(record, size);
	}
	return error;
}

/********/
/* Misc */
/********/

static ERRORCODE
file_error_to_errorcode(CFileException *e)
{
	switch (e->m_cause)
	{
		case CFileException::none:
		{
			return ERRORCODE_None;
		}
		case CFileException::generic:
		case CFileException::hardIO:
		{
			return ERRORCODE_IoError;
		}
		case CFileException::accessDenied:
		{
			return ERRORCODE_Access;
		}
		case CFileException::fileNotFound:
		case CFileException::badPath:
		{
			return ERRORCODE_DoesNotExist;
		}
		case CFileException::diskFull:
		{
			return ERRORCODE_Write;
		}
		case CFileException::directoryFull:
		{
			return ERRORCODE_Full;
		}
		case CFileException::tooManyOpenFiles:
		{
			return ERRORCODE_NoHandles;
		}
		case CFileException::sharingViolation:
		case CFileException::lockViolation:
		{
			return ERRORCODE_Busy;
		}
		default:
		{
			return ERRORCODE_IntError;
		}
	}
}

/************************/
/* StorageFile methods. */
/************************/

/*
// An overridable method to get the CFile to use.
*/

CFile* StorageFile::NewCFile(void)
{
	CFile* pFile = NULL;

	TRY
	{
		pFile = new CFile;
	}
	END_TRY

	return pFile;
}

void StorageFile::DeleteCFile(CFile* pFile)
{
	delete pFile;
}

void StorageFile::CloseCFile(CFile* pFile)
{
	if (pFile != NULL)
	{
		pFile->Close();
		m_fOpen = FALSE;
	}
}

/*
// An internal routine to open the file.
*/

ERRORCODE StorageFile::Open(void)
{
	ERRORCODE error = ERRORCODE_None;

	// Are we named?
	if (m_csName.IsEmpty())
	{
		error = ERRORCODE_Open;
	}
	else
	{
		// Are we already open?
		if (!m_fOpen)
		{
			if (m_pFile == NULL)
			{
				m_pFile = NewCFile();
			}

			if (m_pFile == NULL)
			{
				// Still NULL. Fail with memory error.
				error = ERRORCODE_Memory;
			}
			else
			{
				// Build the file name and invoke the CFile to open the file.
				// We try to locate the file if not creating it.
				CFileException e;
				CString csPath = GetPathName((m_nOpenFlags & CFile::modeCreate) == 0);

				// If we have been attached to another device
				// we do not do the open.

				if (m_based_device != NULL)
				{
					DeleteCFile(m_pFile);
					m_pFile = NULL;
				}

				if (m_based_device != NULL
					 || m_pFile->Open(csPath, m_nOpenFlags, &e))
				{
					// Set the internal variables.
					m_fOpen = TRUE;
					m_current_position = 0;

					if (m_subfile_start != POSITION_UNKNOWN)
					{
					// Position to start of subfile
						seek(0, ST_DEV_SEEK_SET);
					}
				}
				else
				{
					// Some kind of error.
					error = file_error_to_errorcode(&e);
				}
			}
		}
	}

	return CheckError(error);
}

/*
// The common constructor code for StorageFiles.
*/

void StorageFile::common_construct(void)
{
	// Do not use exceptions by default.
	m_fUseExceptions = FALSE;

/* We don't have a file yet. */

	m_pFile = NULL;

/* The file is not open yet. */

	m_fOpen = FALSE;

/* Set the default I/O limits to none. */

	m_limit_start = m_limit_end = POSITION_UNKNOWN;

/* No subfile in effect at this time. */

	m_subfile_start = m_subfile_end = POSITION_UNKNOWN;

/* No completion notify routine. */

	m_completion_notify = NULL;
}

/*
// The constructor for a StorageFile.
*/

StorageFile::StorageFile(LPCSTR in_name)
{
	common_construct();

/* There is no based device in this case. */

	m_based_device = NULL;

/* Set the default open mode. */

	m_nOpenFlags = CFile::modeReadWrite | CFile::shareExclusive;

/* Set the name for opening. */

	set_name(in_name);
}

/*
// Construct a storage file from an existing device.
// Do not pass NULL to this!
*/

StorageFile::StorageFile(StorageDevicePtr existing_device)
{
	common_construct();

	m_based_device = existing_device;

	m_nOpenFlags = 0;						// We should never try to open!
}

/*
// The destructor for a StorageFile.
*/

StorageFile::~StorageFile()
{
	flush();
	clear_name();
	DeleteCFile(m_pFile);
	m_pFile = NULL;
}

/*
// Reset the file to not bound or named.
*/

ERRORCODE StorageFile::reset(void)
{
	flush();

	clear_name();
	set_based_device(NULL);
	set_subfile();

	return ERRORCODE_None;
}

/*
// Initialize the file.
// This involves blanking it out.
// The file is left open.
*/

ERRORCODE StorageFile::initialize()
{
	ERRORCODE error;

	// Pass the buck as required.
	if (m_based_device != NULL)
	{
		if (m_subfile_start != POSITION_UNKNOWN || m_subfile_end != POSITION_UNKNOWN)
		{
		/*
		// Subfile of existing file. We don't really want to initialize.
		// Just seek to the beginning.
		*/
			error = seek(0, ST_DEV_SEEK_SET);
		}
		else
		{
			// Do the destructive initialize.
			error = m_based_device->initialize();
		}
	}
	else
	{
		// Blank the file if we have a name.

		if (m_csName.IsEmpty())
		{
			error = ERRORCODE_Open;
		}
		else
		{
			UINT nOriginalFlags = m_nOpenFlags;

			m_nOpenFlags |= CFile::modeCreate;		// open with create
			error = StorageFile::Open();
			m_nOpenFlags = nOriginalFlags;
		}
	}
	return CheckError(error);
}

/*
// Read data from the file.
*/

ERRORCODE StorageFile::read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *pSizeRead)
{
	ERRORCODE error;

	// Pass the buck as required.
	if (m_based_device != NULL)
	{
		// Find out where we are.
		ST_DEV_POSITION here;

		if ((error = m_based_device->tell(&here)) == ERRORCODE_None)
		{
			// Clip to end of file.
			if (m_subfile_end != POSITION_UNKNOWN && here + size > m_subfile_end)
			{
				ST_DEV_POSITION new_size;

				if ((new_size = m_subfile_end - here) < 0)
				{
					new_size = 0;
				}
				size = (ST_DEV_IO_SIZE)new_size;
			}
	//		od("Read %d from position %ld\r\n", size, ((StorageFilePtr)m_based_device)->m_current_position);
			error = m_based_device->read(ptr, size, pSizeRead);
		}
		return CheckError(error);
	}

	UINT uAmountRead = 0;

	// Make sure the file is open.
	if (!m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
		// Open can now set a based device (for collection items).
		// If we now have a based device, we want to the based read.
		if (m_based_device != NULL)
		{
			// Recurse back in and do the based read.
			return StorageFile::read(ptr, size, pSizeRead);
		}
	}

/*
// Validate limits as necessary.
// Reads which try to read beyond limit end are clipped.
*/

	ST_DEV_POSITION read_end = m_current_position+(ST_DEV_POSITION)size;

	if (m_limit_start != POSITION_UNKNOWN)
	{
		if (m_current_position < m_limit_start)
		{
		/* This is an error! */
			return CheckError(ERRORCODE_AccessLimit);
		}
	}
	if (m_limit_end != POSITION_UNKNOWN)
	{
		if (m_current_position > m_limit_end)
		{
			return CheckError(ERRORCODE_AccessLimit);
		}
		if (read_end > m_limit_end)
		{
		/* Just clip. */
			read_end = m_limit_end;
		}
	}
	
/* Clip to the end of the subfile if we have one. */

	if (m_subfile_end != POSITION_UNKNOWN && read_end > m_subfile_end)
	{
		read_end = m_subfile_end;
	}

/* Compute the (possibly clipped) size. */

	if (m_current_position < read_end)
	{
		size = ST_DEV_IO_SIZE(read_end - m_current_position);

		// Do the read.
		TRY
		{
			uAmountRead = m_pFile->Read(ptr, size);
		}
		CATCH(CFileException, e)
		{
			return CheckError(file_error_to_errorcode(e));
		}
		AND_CATCH_ALL(e)
		{
			return CheckError(ERRORCODE_IntError);
		}
		END_CATCH_ALL
	}

/*
// Update our internal variables.
// Note that we should not be able to read beyond end of file.
*/

	m_current_position += uAmountRead;

/* Generate the return code. */

	if (pSizeRead != NULL)
	{
	/* Return the amount actually read with no error reported. */
		*pSizeRead = uAmountRead;
		return ERRORCODE_None;
	}

	return CheckError((uAmountRead == size) ? ERRORCODE_None : ERRORCODE_Read);
}

/*
// Read data from the file.
*/

ERRORCODE StorageFile::huge_read(LPVOID ptr, DWORD size, DWORD *pdwSizeRead)
{
	ERRORCODE error;

/* Make sure the file is open. */

	if (m_based_device == NULL && !m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Validate limits as necessary. */

	if ((m_limit_start != POSITION_UNKNOWN && m_current_position < m_limit_start)
		|| (m_limit_end != POSITION_UNKNOWN && m_current_position+(ST_DEV_POSITION)size > m_limit_end))
	{
		error = ERRORCODE_AccessLimit;
	}
	else
	{
		error = StorageDevice::huge_read(ptr, size, pdwSizeRead);
	}

	return CheckError(error);
}

/*
// Write data to the file. Only a write can increase the file size.
*/

ERRORCODE StorageFile::write(LPVOID ptr, ST_DEV_IO_SIZE size)
{
/* Pass the buck as required. */

	if (m_based_device != NULL)
	{
		return CheckError(m_based_device->write(ptr, size));
	}

	ERRORCODE error;

/* Make sure the file is open. */

	if (!m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
		// Open can now set a based device (for collection items).
		// If we now have a based device, we want to the based write.
		// Theoretically, this will not happen.
		if (m_based_device != NULL)
		{
			// Recurse back in and do the based write.
			return StorageFile::write(ptr, size);
		}
	}

/* Validate limits as necessary. */

	if ((m_limit_start != POSITION_UNKNOWN && m_current_position < m_limit_start)
		|| (m_limit_end != POSITION_UNKNOWN && m_current_position+(ST_DEV_POSITION)size > m_limit_end))
	{
#if 0
		od("LIMIT start:%ld, end:%ld, cp:%ld, size:%d\n",
						m_limit_start, m_limit_end, m_current_position, size);
#endif

		return CheckError(ERRORCODE_AccessLimit);
	}

/* Do the read. */

	TRY
	{
		m_pFile->Write(ptr, size);
	}
	CATCH(CFileException, e)
	{
		return CheckError(file_error_to_errorcode(e));
	}
	AND_CATCH_ALL(e)
	{
		return CheckError(ERRORCODE_IntError);
	}
	END_CATCH_ALL

/* Update our internal variables. */

	m_current_position += size;

	if (m_subfile_end != POSITION_UNKNOWN && m_current_position > m_subfile_end)
	{
		m_subfile_end = m_current_position;
	}

/* Say whether we succeeded or not. */

	return ERRORCODE_None;
}

/*
// Set the file position.
*/

ERRORCODE StorageFile::seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode)
{
/*
// We need to open the file now if this is not a based device.
// The reason why is because the Open call may establish a subfile.
*/

	if (m_based_device == NULL)
	{
	/*
	// We need to do it ourselves.
	*/

		ERRORCODE error;

	/* Make sure the file is open. */

		if (!m_fOpen)
		{
			if ((error = Open()) != ERRORCODE_None)
			{
				return error;
			}
		}
	}

	UINT nFrom;

/* If this is relative to the beginning of the file, we need to offset it. */

	switch (mode)
	{
		case ST_DEV_SEEK_SET:
		{
			if (m_subfile_start != POSITION_UNKNOWN)
			{
				position += m_subfile_start;
			}
			nFrom = CFile::begin;
			break;
		}
		case ST_DEV_SEEK_END:
		{
			if (m_subfile_end != POSITION_UNKNOWN)
			{
				position = m_subfile_end + position;

				mode = ST_DEV_SEEK_SET;
				nFrom = CFile::begin;
			}
			else
			{
				nFrom = CFile::end;
			}
			break;
		}
		case ST_DEV_SEEK_CUR:
		{
			nFrom = CFile::current;
			break;
		}
		default:
		{
			return CheckError(ERRORCODE_IntError);
		}
	}

/* Pass the buck as required. */

	if (m_based_device != NULL)
	{
		return CheckError(m_based_device->seek(position, mode));
	}

/* File is open. Do the seek now. */

	TRY
	{
//		TRACE("Seek: %08lx (%d) [%08lx]\n", position, nFrom, m_pFile);
		m_current_position = m_pFile->Seek(position, nFrom);
	}
	CATCH(CFileException, e)
	{
		return CheckError(file_error_to_errorcode(e));
	}
	AND_CATCH_ALL(e)
	{
		return CheckError(ERRORCODE_IntError);
	}
	END_CATCH_ALL

	return ERRORCODE_None;
}

/*
// Return the current position of the file.
*/

ERRORCODE StorageFile::tell(ST_DEV_POSITION far *pos)
{
	ERRORCODE error;

	if (m_based_device == NULL)
	{
	/* Make sure the file is open. */

		if (!m_fOpen)
		{
			if ((error = Open()) != ERRORCODE_None)
			{
				return error;
			}
			// Open can now set a based device (for collection items).
			// If we now have a based device, we want to the based seek.
			if (m_based_device != NULL)
			{
				// Recurse back in and do the based seek.
				return StorageFile::tell(pos);
			}
		}

		TRY
		{
//        if(m_current_position != (ST_DEV_POSITION)m_pFile->GetPosition())
//          AfxMessageBox("Tell() has invalid m_current_position-See Dennis!");
			m_current_position = m_pFile->GetPosition();
		}
		CATCH(CFileException, e)
		{
			return CheckError(file_error_to_errorcode(e));
		}
		AND_CATCH_ALL(e)
		{
			return CheckError(ERRORCODE_IntError);
		}
		END_CATCH_ALL

		*pos = m_current_position;
	}
	else
	{
	/* Pass the request up the line. */
		if ((error = m_based_device->tell(pos)) != ERRORCODE_None)
		{
			return CheckError(error);
		}
	}

	if (m_subfile_start != POSITION_UNKNOWN)
	{
		*pos -= m_subfile_start;
	}
	return ERRORCODE_None;
}

/*
// Return the length of the file.
*/

ERRORCODE StorageFile::length(ST_DEV_POSITION far *len)
{
	ERRORCODE error = ERRORCODE_None;

/*
// Open the file now because Open() can set a subfile.
*/

	if (m_based_device == NULL)
	{
	/* See if we've opened the file yet. */

		if (!m_fOpen)
		{
			if ((error = Open()) != ERRORCODE_None)
			{
				return error;
			}
		}
	}

	if (m_subfile_start != POSITION_UNKNOWN && m_subfile_end != POSITION_UNKNOWN)
	{
		*len = m_subfile_end - m_subfile_start;
	}
	else
	{
		if (m_based_device == NULL)
		{
		/* Always read the size (in case it's a shared file). */

			TRY
			{
				*len = m_pFile->GetLength();
			}
			CATCH(CFileException, e)
			{
				error = file_error_to_errorcode(e);
			}
			AND_CATCH_ALL(e)
			{
				error = ERRORCODE_IntError;
			}
			END_CATCH_ALL
		}
		else
		{
			error = m_based_device->length(len);
		}
	}

	return CheckError(error);
}

/*
// Make sure the file is consistent and clean.
*/

ERRORCODE StorageFile::flush()
{
/* Notify of completion if desired. */

	if (m_completion_notify != NULL)
	{
		m_completion_notify(this, m_completion_data);
	/* We only notify once! */
		m_completion_notify = NULL;
	}

	if (m_based_device != NULL)
	{
//		return m_based_device->flush();
	/* Let the other device flush itself when necessary. */
		return ERRORCODE_None;
	}

	ERRORCODE error = ERRORCODE_None;

	if (m_fOpen)
	{
		TRY
		{
			CloseCFile(m_pFile);
		}
		CATCH(CFileException, e)
		{
			error = file_error_to_errorcode(e);
		}
		AND_CATCH_ALL(e)
		{
			error = ERRORCODE_IntError;
		}
		END_CATCH_ALL
	}
	return CheckError(error);
}

/*
// Set the limits for I/O operations.
*/

ERRORCODE StorageFile::io_limits(ST_DEV_POSITION start, ST_DEV_POSITION end)
{
/* Adjust for any subfile in effect. */
	if (m_subfile_start != POSITION_UNKNOWN)
	{
		start -= m_subfile_start;
		end -= m_subfile_start;
	}

	if (m_based_device != NULL)
	{
		return CheckError(m_based_device->io_limits(start, end));
	}

/* Do it ourselves. */

	m_limit_start = start;
	m_limit_end = end;

	return ERRORCODE_None;
}

/*
// Truncate the file at this position.
// It's considered an error for the file to not be open at this point.
*/

ERRORCODE StorageFile::truncate()
{
/*
// See if we need to pass the buck.
*/

	if (m_based_device != NULL)
	{
		return CheckError(m_based_device->truncate());
	}

/* Make sure the file is open. */

	ERRORCODE error = ERRORCODE_None;

	if (!m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
		// Open can now set a based device (for collection items).
		// If we now have a based device, we want to the based truncate.
		if (m_based_device != NULL)
		{
			// Recurse back in and do the based truncate.
			return StorageFile::truncate();
		}
	}

	TRY
	{
		m_current_position = m_pFile->GetPosition();
		m_pFile->SetLength(m_current_position);

		// Success! Update end of subfile if necessary.
		if (m_subfile_end != POSITION_UNKNOWN && m_subfile_end < m_current_position)

		{
			m_subfile_end = m_current_position;
		}
	}
	CATCH(CFileException, e)
	{
		error = file_error_to_errorcode(e);
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return CheckError(error);
}

/*
// Remove a file.
*/

ERRORCODE StorageFile::zap(void)
{
	ERRORCODE error = ERRORCODE_None;

	if (m_csName.IsEmpty())
	{
		error = ERRORCODE_NotInitialized;
	}
	else
	{
		flush();

		TRY
		{
			CFile::Remove(GetPathName(FALSE));
		}
		CATCH(CFileException, e)
		{
			error = file_error_to_errorcode(e);
		}
		AND_CATCH_ALL(e)
		{
			error = ERRORCODE_IntError;
		}
		END_CATCH_ALL
	}

	return CheckError(error);
}

/*
// Lock a range of bytes in a file.
*/

ERRORCODE StorageFile::lock(ST_DEV_POSITION dwStart, ST_DEV_POSITION dwLen, BOOL fUnlock)
{
#if 0
	static int locks = 0;

	if (fUnlock)
	{
		locks--;
	}
	else
	{
		locks++;
	}
	od("[%d] lock %ld, %ld (%d)\r\n", locks, dwStart, dwLen, fUnlock);
#endif

/*
// If this is a based device, do something special.
*/

	if (m_based_device != NULL)
	{
	/* We need to do some locking thing on the based device! */
		return CheckError(ERRORCODE_IntError);		/* For now! Until needed! */
	}

	ERRORCODE error = ERRORCODE_None;

/* Make sure the file is open. */

	if (!m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
		// Open can now set a based device (for collection items).
		// If we now have a based device, we want to the based lock.
		if (m_based_device != NULL)
		{
			// Recurse back in and do the based lock.
			return StorageFile::lock(dwStart, dwLen, fUnlock);
		}
	}

/* Do the lock or unlock. */

	TRY
	{
	/* We want to lock with some retrying. */

		BOOL fGotLock = FALSE;

		for (int i = 0;; )
		{
			TRY
			{
				if (fUnlock)
				{
					m_pFile->UnlockRange(dwStart, dwLen);
				}
				else
				{
					m_pFile->LockRange(dwStart, dwLen);
				}
			/* We got the lock. */
				fGotLock = TRUE;
			}
			CATCH(CFileException, e)
			{
				if ((e->m_cause != CFileException::sharingViolation
					 && e->m_cause != CFileException::lockViolation)
						|| ++i >= m_nRetryCount)
				{
					THROW_LAST();
				}
			}
			AND_CATCH_ALL(e)
			{
				THROW_LAST();
			}
			END_CATCH_ALL

			if (fGotLock)
			{
				break;
			}

		/* We want to delay the appropriate amount. */

			for (DWORD dwEnd = ::GetTickCount() + m_dwRetryDelay;
						::GetTickCount() < dwEnd; )
				;
		}
	}
	CATCH(CFileException, e)
	{
		if (e->m_cause != CFileException::generic)
		{
			error = file_error_to_errorcode(e);
		}
	/* This could be an "Invalid Function" return. Ignore it. */
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return CheckError(error);
}

/*
// Set the name.
// A copy of the name is used.
// If NULL is passed, the file becomes unnamed (and unusable).
*/

ERRORCODE StorageFile::set_name(LPCSTR new_name)
{
	ERRORCODE error = ERRORCODE_None;
/* We can't do this with based devices. */
	if (m_based_device != NULL)
	{
		error = ERRORCODE_Access;
	}
	else
	{
	/* Handle any old name. */

		clear_name();

	/* Handle any new name. */

		if (new_name != NULL)
		{
	//		new_name = skip_collections(new_name);
			TRY
			{
				m_csName = new_name;
			}
			CATCH_ALL(e)
			{
				error = ERRORCODE_Memory;
			}
			END_CATCH_ALL	
		}
	}
	return CheckError(error);
}

/*
// Set the device this storage file is based on.
// If the device already has a name, this is an error.
*/

ERRORCODE StorageFile::set_based_device(StorageDevicePtr device, BOOL fCheckName /*=TRUE*/)
{
	ERRORCODE error = ERRORCODE_None;
	if (fCheckName && !m_csName.IsEmpty())
	{
		error = ERRORCODE_Access;
	}
	else
	{
		m_based_device = device;
	}
	return CheckError(error);
}

/*
// Clear an existing name.
*/

void StorageFile::clear_name()
{
/* We can't do this with based devices. */
	if (m_based_device == NULL)
	{
		if (!m_csName.IsEmpty())
		{
			flush();				/* Make sure any old file is closed. */

			m_csName.Empty();
		}
	}
}

/*
// Create a subfile on this file.
// All accesses become relative to this subfile range.
//
// Call with no (aka default) arguments to turn off the subfile.
*/

void StorageFile::set_subfile(ST_DEV_POSITION start, ST_DEV_POSITION end)
{
	m_subfile_start = start;
	m_subfile_end = end;
}

/*************************/
/* ReadOnlyFile methods. */
/*************************/

void ReadOnlyFile::common_construct(void)
{
/* Set the open mode. */

	set_open_flags(CFile::modeRead | CFile::shareDenyNone);

/* The read buffer is not allocated. */

	m_pBuffer = NULL;
	set_buffering(FALSE);
}

/*
// The constructor for a ReadOnlyFile.
*/

ReadOnlyFile::ReadOnlyFile(LPCSTR name) : StorageFile(name)
{
	common_construct();
}

ReadOnlyFile::ReadOnlyFile(StorageDevicePtr existing_device)
	: StorageFile(existing_device)
{
	common_construct();
}

ReadOnlyFile::~ReadOnlyFile()
{
	free_read_buffer();
}

VOID ReadOnlyFile::set_read_buffer(ST_DEV_IO_SIZE uSize)
{
	free_read_buffer();

	if (uSize != 0)
	{
//		od("Set buffer size to %ld\r\n", uSize);
		TRY
		{
			// Allocate the buffer.
			m_pBuffer = new BYTE[uSize];
			m_uBufferSize = uSize;

			// Buffer is empty.
			m_uBufferLeft = 0;
			m_pBufferPtr = NULL;

			// We are now buffering.
			set_buffering(TRUE);
		}
		CATCH_ALL(e)
		{
			TRACE0("BUFFER SET FAILED!\n");
		}
		END_CATCH_ALL
	}
}

/*
// Free the read buffer.
*/

void ReadOnlyFile::free_read_buffer()
{
	if (m_pBuffer != NULL)
	{
		// Free the buffer memory.
		delete [] m_pBuffer;
		m_pBuffer = NULL;

		// We have no buffer.
		m_uBufferSize = 0;

		// We are no longer buffering.
		set_buffering(FALSE);
	}
}

ERRORCODE ReadOnlyFile::read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read)
{
#if 0
	ST_DEV_POSITION here;
	if (m_based_device == NULL)
	{
		tell(&here);
	}
	else
	{
		m_based_device->tell(&here);
	}
#endif

	if (!m_fBuffering)
	{
//		TRACE2("[%u @ %ld]\n", size, here);
		return StorageFile::read(ptr, size, size_read);
	}
	else
	{
//		od("Buffered read: %u\r\n", size);

		ERRORCODE error = ERRORCODE_None;
		ST_DEV_IO_SIZE we_read = 0;

      // Loop until all data is read
      // Loop repeats when only part of data was in cache, which requires a disk read
		while (size != 0)
		{
			ASSERT(size > 0);

			SHORT amount;

         // If buffer is empty, try to fill it
			if (m_uBufferLeft == 0)
			{
				ST_DEV_IO_SIZE buffered_bytes_read;

            // If read request >= buffer size then skip buffering logic
				if (size >= m_uBufferSize)
				{
				/*
				// It would be silly to read this into the buffer, then
				// just copy it all out. So we just read it straight to the
				// destination.
				*/
					error = StorageFile::read(ptr, size, &buffered_bytes_read);
					if (error == ERRORCODE_None)
					{
						we_read += buffered_bytes_read;
						if (buffered_bytes_read < size && size_read == NULL)
						{
						/* Didn't read enough. */
							error = ERRORCODE_Read;
						}
					}
					// The buffer is now invalid.
					m_pBufferPtr = NULL;

				// And exit the loop. We read all the rest of the data.
					break;
				}
				else
				{
				/* Read the next chunk. */

//					TRACE2("read %u bytes into buffer (%08lx) -> ", buffer_size, here);

               // Read full buffer size of data.
					error = StorageFile::read(m_pBuffer, m_uBufferSize, &buffered_bytes_read);

//					TRACE1("%u read\n", buffered_bytes_read);

					if (error != ERRORCODE_None)
					{
						break;
					}

				/* See if we read anything. */

					if ((m_uBufferLeft = buffered_bytes_read) == 0)
					{
					/* Hit the end of the data. */
						if (size_read == NULL)
						{
						/* Didn't read enough. */
							error = ERRORCODE_Read;
						}
						break;
					}

				/* Back to the start. */

					m_pBufferPtr = m_pBuffer;
				}
			}  // End of empty buffer logic

		/* Copy out however much we can. */

         // Transfer buffered data to callers destination buffer.
         //
         // If amount of data in buffer(cache) is less than amount of data requested,
         // remaining buffer data is transferred into callers destination buffer, then loop
         // will repeat to read remaining data from disk and refill cache with new data.
			amount = __min(size, m_uBufferLeft);
			memcpy(ptr, m_pBufferPtr, amount);

			we_read += amount;
			size -= amount;
			m_uBufferLeft -= amount;

			*((LPSTR *)&ptr) += amount;
			m_pBufferPtr += amount;
		}

		if (size_read != NULL)
		{
		/* Report what we read. */
			*size_read = we_read;
		}
		return CheckError(error);
	}
}

/*
// Read without messing with the buffer.
*/

ERRORCODE ReadOnlyFile::unbuffered_read(ST_DEV_POSITION position, LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read)
{
	ERRORCODE error;

	if (m_fBuffering && m_pBufferPtr != NULL)
	{
		ST_DEV_POSITION here;
		ST_DEV_POSITION start, end;

		tell(&here);

		start = here - (USHORT)(m_pBufferPtr - m_pBuffer);
		end = here + m_uBufferLeft;

		if (position >= start && position + size < end)
		{
//			od("[Pull from buffer...]\r\n");
			memcpy(ptr, (position - start)+m_pBuffer, size);
			if (size_read != NULL)
			{
				*size_read = size;
			}
			return ERRORCODE_None;
		}
	}

/*
// We need to do it the old hard way.
// Hook right into the StorageFile methods to bypass buffering.
*/

//	od("[Pull from file...]\r\n");
	ST_DEV_POSITION here;

	StorageFile::tell(&here);

/* Seek to the position. */

	if ((error = StorageFile::seek(position, ST_DEV_SEEK_SET)) != ERRORCODE_None)
	{
		return error;
	}

/* Read the offset for this strip. */

	if ((error = StorageFile::read(ptr, size, size_read)) != ERRORCODE_None)
	{
		return error;
	}

	return StorageFile::seek(here, ST_DEV_SEEK_SET);
}

ERRORCODE ReadOnlyFile::seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode)
{
	if (!m_fBuffering || m_pBufferPtr == NULL)
	{
		if (m_fBuffering)
		{
			m_uBufferLeft = 0;
		}
		return StorageFile::seek(position, mode);
	}
	else
	{
		ST_DEV_POSITION here;
		ST_DEV_POSITION start, end;

		tell(&here);

//		TRACE("sib: %08lx, %d (from %08lx) -> ", position, mode, here);

      // Start = physical offset in file of buffer
      // End = physical offset in file of buffer end
		start = here - (USHORT)(m_pBufferPtr - m_pBuffer);
		end = here + m_uBufferLeft;

		switch (mode)
		{
			case ST_DEV_SEEK_CUR:
			{
				position += here;
				break;
			}
			case ST_DEV_SEEK_END:
			{
				ST_DEV_POSITION l;
				length(&l);

				position = l + position;
				break;
			}
			default:
			{
				break;
			}
		}
//		TRACE("[%08lx <= %08lx < %08lx ??]\r\n", start, position, end);

		// We allow end. This will just read next time.
      // If new file position is within buffer bounds then reposition buffer pointer
		if (position >= start && position <= end)
		{
		/* Can stay within buffer. */
			m_pBufferPtr = m_pBuffer + (position - start);
			m_uBufferLeft = (ST_DEV_IO_SIZE)(end - position);

			return ERRORCODE_None;
		}
		else
		{
		/* Must seek there explicitly. */
			m_pBufferPtr = NULL;
			m_uBufferLeft = 0;
			return StorageFile::seek(position, ST_DEV_SEEK_SET);
		}         
	}
}

ERRORCODE ReadOnlyFile::tell(ST_DEV_POSITION far *position)
{
	ERRORCODE error = StorageFile::tell(position);

	if (m_fBuffering)
	{
		*position -= m_uBufferLeft;
	}
	return error;
}

/********************/
/* TmpFile methods. */
/********************/

TmpFile::TmpFile(LPCSTR pPrefix) : StorageFile(), m_fCreated(FALSE)
{
	if (pPrefix != NULL)
	{
		GenerateTempName(pPrefix);
	}
}

BOOL TmpFile::GenerateTempName(LPCSTR pszPrefix)
{
	// Generate the temporary file name.
	LPSTR pName = NULL;
	BOOL fSuccess = FALSE;

	CString csPath;
	TRY
	{
		LPCSTR pszPath;
		if (m_pPathManager == NULL)
		{
			pszPath = NULL;
		}
		else
		{
			csPath = m_pPathManager->GetWorkingDirectory();
			Util::RemoveBackslashFromPath(csPath);
			pszPath = csPath;
		}

		// Generate the temporary name.
		if ((pName = _tempnam((LPSTR)pszPath, (LPSTR)pszPrefix)) != NULL)
		{
			// Set our name to this.
			m_csName = pName;
			fSuccess = TRUE;
		}
	}
	END_TRY

	::free(pName);			/* A temp name is allocated with malloc. */

	if (!fSuccess)
	{
		// Failed for some reason. Make sure we have no name.
		m_csName.Empty();
	}
	return fSuccess;
}

/*
// A private method to delete an existing temp file.
*/

void TmpFile::DeleteTempFile(void)
{ 
	if (m_fCreated)
	{
		m_fCreated = FALSE;
		zap();
	}
}

/*
// Temp file destructor.
// Deletes the temporary file.
*/

TmpFile::~TmpFile()
{
/* Delete the temporary file if we created one. */

	DeleteTempFile();
}

/*
// Initialize the TmpFile.
*/

ERRORCODE TmpFile::initialize()
{
	ERRORCODE error;

/* Delete the temporary file if we created one. */

	DeleteTempFile();

/* Initialize us. */

	if ((error = StorageFile::initialize()) == ERRORCODE_None)
	{
		m_fCreated = TRUE;
	}
	return error;
}

/*
// Open the TmpFile. If this is the first time, initialize us.
*/

ERRORCODE TmpFile::Open(void)
{
	return m_fCreated ? StorageFile::Open() : initialize();
}

/*
// Clear the temporary file name.
*/

void TmpFile::clear_name()
{
/* Delete the temporary file if we created one. */

	DeleteTempFile();

/* Pass the buck. */

	StorageFile::clear_name();
}

/*
// Copy one file to another.
*/

ERRORCODE copy_file(StorageDevicePtr source_file, StorageDevicePtr dest_file)
{
	ERRORCODE error;
	ST_DEV_POSITION size;

/* Initialize the destination file. */

	if ((error = dest_file->initialize()) == ERRORCODE_None)
	{
	/* Allocate some memory to use as a buffer. */

		if ((error = source_file->length(&size)) == ERRORCODE_None && size != 0)
		{
			LPVOID buffer = NULL;

		/* Make sure we don't try to allocate too much. */

			if (size > MAX_ALLOC_SIZE)
			{
				size = MAX_ALLOC_SIZE;
			}

			TRY
			{
				do
				{
					TRY
					{
						buffer = (LPVOID)(new char[(int)size]);
					}
					CATCH_ALL(e)
					{
						buffer = NULL;
					}
					END_CATCH_ALL

					if (buffer != NULL)
					{
						break;
					}
					size >>= 1;
				} while (size != 0);

				// Check the result of the allocation.

				if (buffer == NULL)
				{
					error = ERRORCODE_Memory;
				}
				else
				{
				/* We got some memory. */
				/* Start at the front of each file. */

					source_file->seek(0L, ST_DEV_SEEK_SET);
					dest_file->seek(0L, ST_DEV_SEEK_SET);

				/* Loop reading and writing until we can read no more. */

					do
					{
						ST_DEV_IO_SIZE amount_read;

						if ((error = source_file->read(buffer, (ST_DEV_IO_SIZE)size, &amount_read)) == ERRORCODE_None)
						{
						/* See if we read anything. */

							if (amount_read == 0)
							{
							/* All done! */
								break;
							}

						/* Write the data we read. */

							error = dest_file->write(buffer, amount_read);
						}
					} while (error == ERRORCODE_None);
				}
			}
			CATCH_ALL(e)
			{
   			delete [] (char*)buffer;
            THROW_LAST();
			}
			END_CATCH_ALL

			// Free the buffer we allocated.
			delete [] (char*)buffer;
		}
	}
	return error;
}

/*
// Get an image for the temp file from another file.
*/

ERRORCODE TmpFile::get_image(LPCSTR source_name)
{
	ERRORCODE error;

	// Make a file for the source.
	ReadOnlyFile SourceFile(source_name);

	TRY
	{
		// Copy the source file to our file.
		error = get_image(&SourceFile);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	TRY
	{
		SourceFile.flush();
	}
	END_TRY

	return CheckError(error);
}

/*
// Write the temp file image to another file.
*/

ERRORCODE TmpFile::put_image(LPCSTR dest_name)
{
	ERRORCODE error;

	// Make a file for the destination.
	StorageFile DestFile(dest_name);

	TRY
	{
		// Copy the source file to our file.
		// Copy the file over.
		error = put_image(&DestFile);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	TRY
	{
		DestFile.flush();
	}
	END_TRY
	return CheckError(error);
}

/*************************/
/* TextReadFile methods. */
/*************************/

/*
// Constructor for a read-only text file.
*/

TextReadFile::TextReadFile(LPCSTR name) : ReadOnlyFile(name)
{
}

ERRORCODE TextReadFile::read_line(LPSTR buffer, int nMaxLine, WORD wFlags)
{
   BOOL        bInBrackets = FALSE;
	ERRORCODE   error = ERRORCODE_None;
	char			ch = EOF;

/* Read lines until we get a good one. */
	
	if (wFlags & TRF_excel_format)
	{
		BOOL fInQuotes = FALSE;
		BOOL fFinishElement = FALSE;
		int nTrimLength = 0;
		int nElementLength = 0;

		LPSTR line = buffer;
		int nMax = nMaxLine - 1; /* Do one less for the terminator ('\n'). */

		for (;;)
		{
			// Get next character, filter out carriage returns.
			do 
			{
				if ((error = read_char(&ch)) != ERRORCODE_None)
				{
					break;
				}
			} 
			while (ch == '\r');
			if (error != ERRORCODE_None)
			{
				break;
			}

			// Check for a quote.
			if (ch == '"')
			{
				// Got a quote, if this is the first character, then start quoting.
				if (!fInQuotes)
				{
					// We found a first quote. If it's the first character, then
					// assume the element value is enclosed in quotes.
					if (nElementLength == 0)
					{
						// The element is enclosed in quotes.
						fInQuotes = TRUE;

						// Continue reading with the next character.
						continue;
					}
				}
				else
				{
					// We are in quotes and we have another quote. Check for
					// two quotes, otherwise it must be an ending quote.

					// Get the next character after the quote, filter out carriage returns.
					do 
					{
						if ((error = read_char(&ch)) != ERRORCODE_None)
						{
							break;
						}
					} 
					while (ch == '\r');
					if (error != ERRORCODE_None)
					{
						break;
					}

					if (ch != '"')
					{
						// We have a situation where the element is enclosed
						// in quotes, we've found the ending quote, and we've
						// read the character after it. Process the character
						// normally.
						fInQuotes = FALSE;
						fFinishElement = TRUE;
					}

					// We have two consecutive quote characters in a quoted element.
					// Turn the two quotes into one by processing only the second one.
				}
			}

			// Check for end of file.
			if (ch == EOF)
			{
				// If we reached the end of file before reaching the ending quote,
				// then we have a bad format.
				error = ERRORCODE_NotRegistered;

				// The EOF terminates the current element.
				break;
			}

			// We have a character to process. If the character is not
			// quoted, then check for separators.
			if (!fInQuotes)
			{
				// Check for new lines.
				if (ch == '\n')
				{
					// Found the end of line (not quoted), this terminates the element.
					break;
				}

				// Check for specific separators.
				if (ch == '\t')
				{
					// Found a separator.
					break;
				}
			}

			// Not a separator. Change strange characters which aren't supported
			// by PrintMaster's fill-in fields into spaces.
			if (ch < ' ')
			{
				ch = ' ';
			}

			// Check if the character should be considered white space. Quoted elements
			// always contain all characters including white space. White space is trimmed
			// from the start and end of unquoted elements.
			BOOL fIsWhite = !fInQuotes && (ch == ' ');

			if (fFinishElement && !fIsWhite)
			{
				break;
			}

			// Add the character to the element.
			if (!fIsWhite || (nElementLength > 0))
			{
				if (nElementLength > nMaxLine)
				{
					// Element is a ridiculous length, so we probably have a bad format.
					error = ERRORCODE_NotRegistered;
				}

				// Save the character and advance the length;
				if (nMax != 0)
				{
					*line++ = ch;
					nMax--;
					nElementLength++;
				}

				// If the character is not white, then adjust the trim length
				// to ensure the character is not trimmed.
				if (!fIsWhite)
				{
					nTrimLength = nElementLength;
				}
			}
		}

		// Trim the element if necessary.
		line[nTrimLength] = '\0';
	}
	else	// (wFlags & TRF_excel_format)
	{
		for (;;)
		{
		/*
 		// Read the next line.
 		*/

			LPSTR line = buffer;
			int nMax = nMaxLine - 1; /* Do one less for the terminator ('\n'). */

			for (;;)
			{
				if ((error = read_char(&ch)) != ERRORCODE_None)
				{
					break;
				}

				if(wFlags & TRF_ignore_eol_in_brackets)
					{
						if(ch == '[')
							{
								bInBrackets = TRUE;
								continue;
							}
						else if(ch == ']')
							{
								bInBrackets = FALSE;
								continue;
							}
					}

				if( !bInBrackets )
					{
						if (ch == '\r')
						{
							continue;
						}
						if (ch == '\n')
						{
							break;
						}
					}

			/* Add the character to the line if we have not already done so. */

				if (nMax != 0)
				{
					*line++ = ch;
					nMax--;
				}
			}

			*line = '\0';

			if (error != ERRORCODE_None)
			{
			/* Return with that error. */
				break;
			}

		/* We got a line. See if we need to skip. */

			if (((wFlags & TRF_skip_comments) && buffer[0] == '#')
					|| ((wFlags & TRF_skip_blank_lines) && buffer[0] == '\0'))
			{
			/* Try again. */
			}
			else
			{
			/* We want this line. */
				break;
			}
		}
	}

	return CheckError(error);
}

ERRORCODE TextReadFile::read_char(char* ch)
{
	// Get the next character.
	ERRORCODE error = ERRORCODE_None;
	if (m_fBuffering && m_uBufferLeft > 0)
	{
		*ch = *m_pBufferPtr++;
		m_uBufferLeft--;
	}
	else
	{
		error = read(ch, sizeof(*ch));
	}

	return error;
}

#if 0
/////////////////////////////////////////////////////////////////////////////
// CFileOnStorageDevice
// Suitable for archives.

CFileOnStorageDevice::CFileOnStorageDevice(StorageDevice* pDevice)
{
	ASSERT(pDevice != NULL);
	m_pDevice = pDevice;
}

CFileOnStorageDevice::~CFileOnStorageDevice()
{
}

DWORD CFileOnStorageDevice::GetPosition() const
{
	ST_DEV_POSITION pos;

	m_pDevice->tell(&pos);
	return pos;
}

// Operations

BOOL CFileOnStorageDevice::Open(const char* pszFileName, UINT nOpenFlags,
		CFileException* pError /*=NULL*/)
{
	ASSERT(FALSE);
	return FALSE;
}

// Overridables
CFile* CFileOnStorageDevice::Duplicate() const
{
	ASSERT(FALSE);
	return NULL;
}

LONG CFileOnStorageDevice::Seek(LONG lOff, UINT nFrom)
{
	switch (nFrom)
	{
		case CFile::begin:
		{
			nFrom = ST_DEV_SEEK_SET;
			break;
		}
		case CFile::current:
		{
			nFrom = ST_DEV_SEEK_CUR;
			break;
		}
		case CFile::end:
		{
			nFrom = ST_DEV_SEEK_END;
			break;
		}
		default:
		{
		// Strange 'nFrom' passed.
			ASSERT(FALSE);
		}
	}

	LONG lOldPos = GetPosition();
	m_pDevice->seek(lOff, nFrom);
	return lOldPos;			// Return old position.
}

void CFileOnStorageDevice::SetLength(DWORD dwNewLen)
{
	ASSERT(FALSE);
}

DWORD CFileOnStorageDevice::GetLength() const
{
	ST_DEV_POSITION length;
	m_pDevice->length(&length);
	return length;
}

UINT CFileOnStorageDevice::Read(void FAR* lpBuf, UINT nCount);
{
	ST_DEV_SIZE size_read;
	m_pDevice->read(lpBuf, size, &size_read);
	return size_read;
}

void CFileOnStorageDevice::Write(const void FAR* lpBuf, UINT nCount);
{
	m_pDevice->write(lpBuf, size);
}

void CFileOnStorageDevice::Flush()
{
	m_pDevice->Flush();
}

void CFileOnStorageDevice::Close()
{
	m_pDevice->Flush();
}

void CFileOnStorageDevice::LockRange(DWORD dwPos, DWORD dwCount)
{
// Not implemented.
	ASSERT(FALSE);
}

void CFileOnStorageDevice::UnlockRange(DWORD dwPos, DWORD dwCount)
{
// Not implemented.
	ASSERT(FALSE);
}

void CFileOnStorageDevice::Abort()
{
// Not implemented.
	ASSERT(FALSE);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// StorageFileOnCFile

StorageFileOnCFile::StorageFileOnCFile(CFile* pFile /*=NULL*/)
{
	SetCFile(pFile);
}

StorageFileOnCFile::~StorageFileOnCFile()
{
	m_pFile = NULL;
}

ERRORCODE StorageFileOnCFile::initialize(VOID)
{
	return ERRORCODE_None;
}

ERRORCODE StorageFileOnCFile::Open(void)
{
// This should not be called! m_fOpen should always be valid.
	ASSERT(FALSE);
	return m_pFile == NULL ? ERRORCODE_Open : ERRORCODE_None;
}

BOOL StorageFileOnCFile::IsAssigned() const
{
	return (m_pFile != NULL) || (m_based_device != NULL);
}

CFile* StorageFileOnCFile::NewCFile()
{
// This should not be called! m_pFile should always be valid.
	ASSERT(FALSE);
	return FALSE;
}

void StorageFileOnCFile::DeleteCFile(CFile* pFile)
{
}

void StorageFileOnCFile::CloseCFile(CFile* pFile)
{
	if (m_pFile->IsKindOf(RUNTIME_CLASS(COleStreamFile)))
	{
		if (((COleStreamFile*)m_pFile)->m_lpStream == NULL)
		{
			return;
		}
	}
	m_pFile->Flush();
}

void StorageFileOnCFile::SetCFile(CFile* pFile /*=NULL*/)
{
	m_pFile = pFile;
	m_fOpen = (pFile != NULL);
}

CFile* StorageFileOnCFile::GetCFile(void) const
{
	return m_pFile;
}
