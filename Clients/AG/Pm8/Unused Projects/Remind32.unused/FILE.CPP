/*
// $Header: /PM8/REMIND32/FILE.CPP 1     3/03/99 6:26p Gbeddow $
//
// File handling routines.
//
// $Log: /PM8/REMIND32/FILE.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:28   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:48   Fred
// Initial revision.
// 
//    Rev 1.9   14 Nov 1996 08:49:24   Jay
// New compressed files
// 
//    Rev 1.7   13 Nov 1996 17:20:26   JAY
//  
// 
//    Rev 1.6   12 Nov 1996 15:29:50   JAY
// New compression stuff.
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
//#include "cdeflate.h"

#include <malloc.h>
#include <windowsx.h>

/////////////////////////////////////////////////////////////////////////////
// CFileRelocationInfo

void CFileRelocationInfo::Reset(void)
{
	m_lStart = POSITION_UNKNOWN;
	m_lEnd = POSITION_UNKNOWN;
	m_nCompression = COMPRESSION_None;
}

/************************/
/* StorageFile routines */
/************************/

LPSTR StorageFile::m_base_path = NULL;
StorageFile::PATH_TRANSLATOR StorageFile::m_path_translator = NULL;
LPVOID StorageFile::m_translate_data = NULL;

int StorageFile::m_nRetryCount = 5;
DWORD StorageFile::m_dwRetryDelay = 1000;				// In milliseconds

LPCSTR StorageFile::get_path_name(BOOL locate, char *out_path, CFileRelocationInfo* pInfo /*=NULL*/)
{
	return TranslateFileName(m_csName, locate, out_path, pInfo);
}

LPCSTR StorageFile::TranslateFileName(LPCSTR pName, BOOL locate, char *out_path /*=NULL*/, CFileRelocationInfo* pInfo /*=NULL*/)
{
	if (pInfo != NULL)
	{
		pInfo->Reset();
	}

	if (m_path_translator == NULL)
	{
		if (out_path == NULL)
		{
		/* We don't bother with this case. */
			return pName;
		}
		else
		{
			strcpy(out_path, pName);
			return out_path;
		}
	}
	else
	{
		return m_path_translator(pName, out_path, locate, pInfo, m_translate_data);
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

	return (dwAmountRead == size)
						? ERRORCODE_None
 						: ERRORCODE_Read;
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

#ifndef WIN32
		DWORD dwBytesLeftInSeg = 0x10000 - (DWORD)OFFSETOF(wp);

		if ((DWORD)uAmountToWrite > dwBytesLeftInSeg)
		{
			uAmountToWrite = (WORD)dwBytesLeftInSeg;
		}
#endif
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

	return (dwAmountWritten == size)
						? ERRORCODE_None
 						: ERRORCODE_Write;
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

/* Out with any old. */

	if (*string != NULL)
	{
		delete [] *string;
	}
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
		char* pBuffer;
		TRY
		{
			pBuffer = csString.GetBuffer(size);
		}
		CATCH_ALL(e)
		{
			return ERRORCODE_Memory;
		}
		END_CATCH_ALL
		error = read(pBuffer, size);
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

/* Get our block. */

	if ((b = *block) == NULL)
	{
		TRY
		{
			b = (LPVOID)(new char[size]);
		}
		CATCH_ALL(e)
		{
			return ERRORCODE_Memory;
		}
		END_CATCH_ALL
	}

/* Allocated. Read. */

	if (size == 0 || (error = read(b, size)) == ERRORCODE_None)
	{
		*block = b;
	}
	else
	{
	/* Failed! Only free if we allocated ourselves. */

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
	// We should not get here if we are compressed.
	if (m_nCompression != COMPRESSION_None)
	{
		ASSERT(FALSE);
		return ERRORCODE_None;
	}
/* Are we named? */

	if (m_csName.IsEmpty())
	{
		return ERRORCODE_Open;
	}

/* Are we already open? */

	ERRORCODE error = ERRORCODE_None;

	if (!m_fOpen)
	{
		if (m_pFile == NULL)
		{
			if ((m_pFile = NewCFile()) == NULL)
			{
				return ERRORCODE_Memory;
			}
		}

	/* Build the file name and invoke the CFile to open the file. */

		CFileException e;
		CFileRelocationInfo Info;
		LPCSTR pPath = get_path_name(TRUE, NULL, &Info);
		BOOL fGotSubfile = FALSE;

		ASSERT(pPath != NULL);

		/* We only override the subfile if we have definite values to set. */

		if (Info.m_lStart != POSITION_UNKNOWN || Info.m_lEnd != POSITION_UNKNOWN)
		{
			set_subfile(Info.m_lStart, Info.m_lEnd);
			fGotSubfile = TRUE;
		}
#ifdef _DEBUG
		else
		{
			CString csSourceName;
			CString csDestName;

			TRACE2("Name '%s' got converted to '%s'...\r\n",
						(LPCSTR)m_csName, (LPCSTR)pPath);

			Util::SplitPath(pPath, NULL, &csDestName);
			Util::SplitPath(m_csName, NULL, &csSourceName);
			if (stricmp(csSourceName, csDestName) != 0)
			{
				CString csMessage;

				csMessage = "This is the big one!\n";
				csMessage += "File '";
				csMessage += m_csName;
				csMessage += "' converted to '";
				csMessage += pPath;
				csMessage += "'.\nPlease tell Jay at once!";
				AfxMessageBox(csMessage, MB_OK | MB_ICONHAND);
			}
		}
#endif
		if (m_pFile->Open(pPath, m_nOpenFlags, &e))
		{
		/* Set the internal variables. */
			m_fOpen = TRUE;
			m_current_position = 0;

			if (fGotSubfile)
			{
			// Position to start of subfile
				seek(0, ST_DEV_SEEK_SET);
			}
			if (Info.m_nCompression != COMPRESSION_None)
			{
				// We need to setup for a compressed read.
				error = InitDecompression(Info);
			}
		}
		else
		{
		/* Some kind of error */

			return file_error_to_errorcode(&e);
		}
	}

/* Opened successfully. */

	return error;
}

/*
// We are going to be doing a read from compressed data.
*/

ERRORCODE StorageFile::InitDecompression(CFileRelocationInfo& Info)
{
   ERRORCODE error;

	if (Info.m_nCompression >= COMPRESSION_LAST)
	{
		error = ERRORCODE_IllegalType;
	}
	else
	{
		m_dwCompressedSize = Info.m_lEnd - Info.m_lStart;
		error = Decompress(Info.m_nCompression);
	}
	return error;
}

ERRORCODE StorageFile::Decompress(CompressionType nCompression)
{
	// No source? Nothing to do.
	if (m_dwCompressedSize <= 0)
	{
		ASSERT(m_pUncompressedImage = NULL);
		return ERRORCODE_None;
	}

	// Do the decompress.
	switch (nCompression)
	{
#if 0
		case COMPRESSION_Deflate:
		{
			// We need to do that deflation thing.

			LPBYTE pSourceData = (LPBYTE)GlobalAllocPtr(GMEM_MOVEABLE, m_dwCompressedSize);
			if (pSourceData == NULL)
			{
				return ERRORCODE_Memory;
			}

			ERRORCODE error;

			if ((error = seek(0, ST_DEV_SEEK_SET)) == ERRORCODE_None
				 && (error = huge_read(pSourceData, m_dwCompressedSize)) == ERRORCODE_None)
			{
				CDeflate Deflate;

				if (!Deflate.Decompress(pSourceData, m_dwCompressedSize, &m_pUncompressedImage, &m_dwUncompressedSize))
				{
					error = ERRORCODE_Read;
				}
				else
				{
					// Success!
					// Set the compression type now.
					m_nCompression = nCompression;
				}
			}
			// Free our source data.
			GlobalFreePtr(pSourceData);
			return error;
		}
#endif
		default:
		{
			ASSERT(FALSE);
			return ERRORCODE_IllegalType;
		}
	}
}

void StorageFile::FreeCompressionBuffers(void)
{
	if (m_pUncompressedImage != NULL)
	{
		GlobalFreePtr(m_pUncompressedImage);
		m_pUncompressedImage = NULL;
	}
}

/*
// The common constructor code for StorageFiles.
*/

void StorageFile::common_construct(void)
{
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

	// Compression variables.
	m_nCompression = COMPRESSION_None;
	m_pUncompressedImage = NULL;
	m_dwUncompressedSize = 0;
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
	FreeCompressionBuffers();
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
/* Pass the buck as required. */

	if (m_based_device != NULL)
	{
		if (m_subfile_start != POSITION_UNKNOWN || m_subfile_end != POSITION_UNKNOWN)
		{
		/*
		// Subfile of existing file. We don't really want to initialize.
		// Just seek to the beginning.
		*/
			return seek(0, ST_DEV_SEEK_SET);
		}

	/* Do the destructive initialize. */

		return m_based_device->initialize();
	}

	if (m_nCompression != COMPRESSION_None)
	{
		// This should not be set now!
		return ERRORCODE_IntError;
	}

/* Blank the file if we have a name. */

	if (m_csName.IsEmpty())
	{
		return ERRORCODE_Open;
	}

	UINT nOriginalFlags = m_nOpenFlags;

	m_nOpenFlags |= CFile::modeCreate;		// open with create
	ERRORCODE error = StorageFile::Open();
	m_nOpenFlags = nOriginalFlags;

	return error;
}

/*
// Read data from the file.
*/

ERRORCODE StorageFile::read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *pSizeRead)
{
	ERRORCODE error;
	UINT uAmountRead = 0;

	if (m_nCompression == COMPRESSION_None)
	{
		// Pass this off to the based device if we have one.
		if (m_based_device != NULL)
		{
			// Find out where we are.
			ST_DEV_POSITION here;
			ERRORCODE error;

			if ((error = m_based_device->tell(&here)) != ERRORCODE_None)
			{
				return error;
			}

			// Clip to end of file.
			if (here + size > m_subfile_end)
			{
				ST_DEV_POSITION new_size;

				if ((new_size = m_subfile_end - here) < 0)
				{
					new_size = 0;
				}
				size = (ST_DEV_IO_SIZE)new_size;
			}
//			od("Read %d from position %ld\r\n", size, ((StorageFilePtr)m_based_device)->m_current_position);
			return m_based_device->read(ptr, size, pSizeRead);
		}

	/* Make sure the file is open. */

		if (!m_fOpen)
		{
			if ((error = Open()) != ERRORCODE_None)
			{
				return error;
			}
		}
	}

/*
// Validate limits as necessary.
// Reads which try to read beyond limit end are clipped.
*/

	ST_DEV_POSITION read_end = m_current_position+(ST_DEV_POSITION)size;
	ST_DEV_POSITION lSubfileStart = m_subfile_start == POSITION_UNKNOWN
												? 0
												: m_subfile_start;

	if (m_nCompression == COMPRESSION_None)
	{
		if (m_limit_start != POSITION_UNKNOWN)
		{
			if (m_current_position < m_limit_start)
			{
			/* This is an error! */
				return ERRORCODE_AccessLimit;
			}
		}
		if (m_limit_end != POSITION_UNKNOWN)
		{
			if (m_current_position > m_limit_end)
			{
				return ERRORCODE_AccessLimit;
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
	}
	else
	{
		// Clip to the end of the uncompressed data, if necessary.
		if (m_nCompression != COMPRESSION_None)
		{
			if (read_end > (ST_DEV_POSITION)(m_dwUncompressedSize + lSubfileStart))
			{
				read_end = (ST_DEV_POSITION)(m_dwUncompressedSize + lSubfileStart);
			}
		}
	}

	// Compute the (possibly clipped) size.
	ASSERT(read_end >= m_current_position);
	size = ST_DEV_IO_SIZE(read_end - m_current_position);

	// Handle compression here.
	ASSERT(m_nCompression != COMPRESSION_Unknown);
	if (m_nCompression == COMPRESSION_None)
	{
		if (size > 0)
		{
		/* Do the read. */

			TRY
			{
				uAmountRead = m_pFile->Read(ptr, size);
			}
			CATCH(CFileException, e)
			{
				return file_error_to_errorcode(e);
			}
			AND_CATCH_ALL(e)
			{
				return ERRORCODE_IntError;
			}
			END_CATCH_ALL
		}
	}
	else
	{
		ST_DEV_POSITION Position = m_current_position;
		if (m_subfile_start != POSITION_UNKNOWN)
		{
			Position -= m_subfile_start;
		}

		// We have some compressed data.
		if (m_pUncompressedImage == NULL
				|| Position > (ST_DEV_POSITION)m_dwUncompressedSize)
		{
			return ERRORCODE_Read;
		}

#ifdef WIN32
		memcpy(ptr, m_pUncompressedImage + Position, size);
#else
		hmemcpy(ptr, ((BYTE huge *)m_pUncompressedImage) + Position, size);
#endif
		uAmountRead = size;
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

	return (uAmountRead == size)
						? ERRORCODE_None
 						: ERRORCODE_Read;
}

/*
// Read data from the file.
*/

ERRORCODE StorageFile::huge_read(LPVOID ptr, DWORD size, DWORD *pdwSizeRead)
{
/* Pass the buck as required. */

	if (m_based_device != NULL)
	{
		return m_based_device->huge_read(ptr, size, pdwSizeRead);
	}

	ERRORCODE error;

/* Make sure the file is open. */

	if (!m_fOpen)
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
		return ERRORCODE_AccessLimit;
	}

	return StorageDevice::huge_read(ptr, size, pdwSizeRead);
}

/*
// Write data to the file. Only a write can increase the file size.
*/

ERRORCODE StorageFile::write(LPVOID ptr, ST_DEV_IO_SIZE size)
{
/* Pass the buck as required. */

	if (m_based_device != NULL)
	{
		return m_based_device->write(ptr, size);
	}

	ERRORCODE error;

/* Make sure the file is open. */

	if (!m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
	}
	
	if (m_nCompression != COMPRESSION_None)
	{
		// For now, we do not write.
		return ERRORCODE_IntError;
	}

/* Validate limits as necessary. */

	if ((m_limit_start != POSITION_UNKNOWN && m_current_position < m_limit_start)
		|| (m_limit_end != POSITION_UNKNOWN && m_current_position+(ST_DEV_POSITION)size > m_limit_end))
	{
#if 0
		od("LIMIT start:%ld, end:%ld, cp:%ld, size:%d\n",
						m_limit_start, m_limit_end, m_current_position, size);
#endif

		return ERRORCODE_AccessLimit;
	}

/* Do the read. */

	TRY
	{
		m_pFile->Write(ptr, size);
	}
	CATCH(CFileException, e)
	{
		return file_error_to_errorcode(e);
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
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
			return ERRORCODE_IntError;
		}
	}

/* Pass the buck as required. */

	if (m_nCompression == COMPRESSION_None && m_based_device != NULL)
	{
		return m_based_device->seek(position, mode);
	}

	// File is open. Do the seek now.

	TRY
	{
//		TRACE("Seek: %08lx (%d) [%08lx]\n", position, nFrom, m_pFile);
		if (m_nCompression == COMPRESSION_None)
		{
			m_current_position = m_pFile->Seek(position, nFrom);
		}
		else
		{
			// Just update the internal variable ourselves.
			switch (nFrom)
			{
				case CFile::begin:
				{
					m_current_position = 0;
					break;
				}
				case CFile::end:
				{
					m_current_position = m_dwUncompressedSize;
					break;
				}
				default:
				{
					break;
				}
			}
			m_current_position += position;
		}
	}
	CATCH(CFileException, e)
	{
		return file_error_to_errorcode(e);
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
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

	if (m_based_device == NULL || m_nCompression != COMPRESSION_None)
	{
	/* Make sure the file is open. */

		if (m_based_device == NULL)
		{
			if (!m_fOpen)
			{
				if ((error = Open()) != ERRORCODE_None)
				{
					return error;
				}
			}
		}

		TRY
		{
			// If we are compressed, do not go to the file.
			if (m_nCompression == COMPRESSION_None)
			{
				// We need to seek.
				m_current_position = m_pFile->GetPosition();
			}
		}
		CATCH(CFileException, e)
		{
			return file_error_to_errorcode(e);
		}
		AND_CATCH_ALL(e)
		{
			return ERRORCODE_IntError;
		}
		END_CATCH_ALL

		*pos = m_current_position;
	}
	else
	{
	/* Pass the request up the line. */
		if ((error = m_based_device->tell(pos)) != ERRORCODE_None)
		{
			return error;
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

	// If we are compressed, then return our uncompressed size.
	if (m_nCompression != COMPRESSION_None)
	{
		*len = (ST_DEV_POSITION)m_dwUncompressedSize;
		return ERRORCODE_None;
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
				return file_error_to_errorcode(e);
			}
			AND_CATCH_ALL(e)
			{
				return ERRORCODE_IntError;
			}
			END_CATCH_ALL
		}
		else
		{
			error = m_based_device->length(len);
		}
	}

	return ERRORCODE_None;
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

	if (m_fOpen)
	{
		TRY
		{
			CloseCFile(m_pFile);
		}
		CATCH(CFileException, e)
		{
			return file_error_to_errorcode(e);
		}
		AND_CATCH_ALL(e)
		{
			return ERRORCODE_IntError;
		}
		END_CATCH_ALL
	}
	return ERRORCODE_None;
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
		return m_based_device->io_limits(start, end);
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
		return m_based_device->truncate();
	}

/* Make sure the file is open. */

	if (!m_fOpen)
	{
		ERRORCODE error;
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
	}

	if (m_nCompression != COMPRESSION_None)
	{
		return ERRORCODE_IntError;
	}

	TRY
	{
		m_current_position = m_pFile->GetPosition();
		m_pFile->SetLength(m_current_position);
	}
	CATCH(CFileException, e)
	{
		return file_error_to_errorcode(e);
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
	}
	END_CATCH_ALL

/* Success! Update end of subfile if necessary. */

	if (m_subfile_end != POSITION_UNKNOWN && m_subfile_end < m_current_position)
	{
		m_subfile_end = m_current_position;
	}

	return ERRORCODE_None;
}

/*
// Remove a file.
*/

ERRORCODE StorageFile::zap(void)
{
	flush();

	TRY
	{
		CFile::Remove(get_path_name(FALSE));
	}
	CATCH(CFileException, e)
	{
		return file_error_to_errorcode(e);
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return ERRORCODE_None;
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
		return ERRORCODE_IntError;		/* For now! Until needed! */
	}

	ERRORCODE error;

/* Make sure the file is open. */

	if (!m_fOpen)
	{
		if ((error = Open()) != ERRORCODE_None)
		{
			return error;
		}
	}

	if (m_nCompression != COMPRESSION_None)
	{
		return ERRORCODE_None;
	}

	// Do the lock or unlock.

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
			return file_error_to_errorcode(e);
		}
	/* This could be an "Invalid Function" return. Ignore it. */
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return ERRORCODE_None;
}

/*
// Set the name.
// A copy of the name is used.
// If NULL is passed, the file becomes unnamed (and unusable).
*/

ERRORCODE StorageFile::set_name(LPCSTR new_name)
{
/* We can't do this with based devices. */
	if (m_based_device != NULL)
	{
		return ERRORCODE_Access;
	}

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
			return ERRORCODE_Memory;
		}
		END_CATCH_ALL	
	}
	return ERRORCODE_None;
}

/*
// Set the device this storage file is based on.
// If the device already has a name, this is an error.
*/

ERRORCODE StorageFile::set_based_device(StorageDevicePtr device)
{
	if (!m_csName.IsEmpty())
	{
		return ERRORCODE_Access;
	}
	m_based_device = device;
	return ERRORCODE_None;
}

/*
// Clear an existing name.
*/

VOID StorageFile::clear_name()
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

	buffer = NULL;
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

VOID ReadOnlyFile::set_read_buffer(ST_DEV_IO_SIZE size)
{
	free_read_buffer();

	if (size != 0)
	{
//		od("Set buffer size to %ld\r\n", size);
		TRY
		{
			buffer = new BYTE[size];
			buffer_size = size;
			bytes_in_buffer = 0;
			current_byte_in_buffer = NULL;
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

VOID ReadOnlyFile::free_read_buffer()
{
	if (buffer != NULL)
	{
		delete [] buffer;
		buffer = NULL;
		buffer_size = 0;
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

	if (!buffering)
	{
//		TRACE2("[%u @ %ld]\n", size, here);
		return StorageFile::read(ptr, size, size_read);
	}
	else
	{
//		od("Buffered read: %u\r\n", size);

		ERRORCODE error = ERRORCODE_None;
		ST_DEV_IO_SIZE we_read = 0;

		while (size != 0)
		{
			ASSERT(size > 0);

			SHORT amount;

			if (bytes_in_buffer == 0)
			{
				ST_DEV_IO_SIZE buffered_bytes_read;

				if (size >= buffer_size)
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
					current_byte_in_buffer = NULL;

				// And exit the loop. We read all the rest of the data.
					break;
				}
				else
				{
				/* Read the next chunk. */

//					TRACE2("read %u bytes into buffer (%08lx) -> ", buffer_size, here);

					error = StorageFile::read(buffer, buffer_size, &buffered_bytes_read);

//					TRACE1("%u read\n", buffered_bytes_read);

					if (error != ERRORCODE_None)
					{
						break;
					}

				/* See if we read anything. */

					if ((bytes_in_buffer = buffered_bytes_read) == 0)
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

					current_byte_in_buffer = buffer;
				}
			}

		/* Copy out however much we can. */

			amount = __min(size, bytes_in_buffer);
			memcpy(ptr, current_byte_in_buffer, amount);

			we_read += amount;
			size -= amount;
			bytes_in_buffer -= amount;

			*((LPSTR *)&ptr) += amount;
			current_byte_in_buffer += amount;
		}

		if (size_read != NULL)
		{
		/* Report what we read. */
			*size_read = we_read;
		}
		return error;
	}
}

/*
// Read without messing with the buffer.
*/

ERRORCODE ReadOnlyFile::unbuffered_read(ST_DEV_POSITION position, LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read)
{
	ERRORCODE error;

	if (buffering && current_byte_in_buffer != NULL)
	{
		ST_DEV_POSITION here;
		ST_DEV_POSITION start, end;

		tell(&here);

		start = here - (USHORT)(current_byte_in_buffer - buffer);
		end = here + bytes_in_buffer;

		if (position >= start && position + size < end)
		{
//			od("[Pull from buffer...]\r\n");
			memcpy(ptr, (position - start)+buffer, size);
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
	if (!buffering || current_byte_in_buffer == NULL)
	{
		if (buffering)
		{
			bytes_in_buffer = 0;
		}
		return StorageFile::seek(position, mode);
	}
	else
	{
		ST_DEV_POSITION here;
		ST_DEV_POSITION start, end;

		tell(&here);

//		TRACE("sib: %08lx, %d (from %08lx) -> ", position, mode, here);

		start = here - (USHORT)(current_byte_in_buffer - buffer);
		end = here + bytes_in_buffer;

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
		if (position >= start && position <= end)
		{
		/* Can stay within buffer. */
			current_byte_in_buffer = buffer + (position - start);
			bytes_in_buffer = (ST_DEV_IO_SIZE)(end - position);

			return ERRORCODE_None;
		}
		else
		{
		/* Must seek there explicitly. */
			current_byte_in_buffer = NULL;
			bytes_in_buffer = 0;
			return StorageFile::seek(position, ST_DEV_SEEK_SET);
		}         
	}
}

ERRORCODE ReadOnlyFile::tell(ST_DEV_POSITION far *position)
{
	ERRORCODE error = StorageFile::tell(position);

	if (buffering)
	{
		*position -= bytes_in_buffer;
	}
	return error;
}

/********************/
/* TmpFile methods. */
/********************/

TmpFile::TmpFile(char *prefix) : StorageFile(), m_fCreated(FALSE)
{
/* Generate the temporary file name. */

	LPSTR pName = NULL;
	BOOL fSuccess = FALSE;

	TRY
	{
		CString csPath = m_base_path;
		Util::RemoveBackslashFromPath(csPath);

	/* Generate the temporary name. */

		if ((pName = _tempnam((LPSTR)(LPCSTR)csPath, prefix)) != NULL)
		{
		/* Set our name to this. */
			m_csName = pName;
			fSuccess = TRUE;
		}
	}
	END_TRY

	::free(pName);			/* A temp name is allocated with malloc. */

	if (!fSuccess)
	{
	/* Failed for some reason. */
		m_csName.Empty();
	}
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

VOID TmpFile::clear_name()
{
/* Delete the temporary file if we created one. */

	DeleteTempFile();

/* Pass the buck. */

	StorageFile::clear_name();
}

/*
// Copy one file to another.
*/

ERRORCODE
copy_file(StorageDevicePtr source_file, StorageDevicePtr dest_file)
{
	ERRORCODE error;
	ST_DEV_POSITION size;

/* Initialize the destination file. */

	if ((error = dest_file->initialize()) == ERRORCODE_None)
	{
	/* Allocate some memory to use as a buffer. */

		if ((error = source_file->length(&size)) == ERRORCODE_None && size != 0)
		{
			LPVOID buffer;

		/* Make sure we don't try to allocate too much. */

			if (size > MAX_ALLOC_SIZE)
			{
				size = MAX_ALLOC_SIZE;
			}

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

		/* Check the result of the allocation. */

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

			/* Free the buffer we allocated. */

				delete [] (char*)buffer;
			}
		}
	}
	return error;
}

/*
// Get an image for the temp file from another file.
*/

ERRORCODE TmpFile::get_image(LPCSTR source_name)
{
	ReadOnlyFilePtr source_file;
	ERRORCODE error;

/* Make a file for the source. */

	if ((source_file = new ReadOnlyFile(source_name)) == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Copy the source file to our file. */

	error = get_image(source_file);

	delete source_file;
	return error;
}

/*
// Write the temp file image to another file.
*/

ERRORCODE TmpFile::put_image(LPCSTR dest_name)
{
	StorageFilePtr dest_file;
	ERRORCODE error;

/* Make a file for the source. */

	if ((dest_file = new StorageFile(dest_name)) == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Copy the file over. */

	error = put_image(dest_file);

	delete dest_file;
	return error;
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
	ERRORCODE error = ERRORCODE_None;

/* Read lines until we get a good one. */

	for (;;)
	{
	/*
 	// Read the next line.
 	*/

		LPSTR line = buffer;
		int nMax = nMaxLine - 1; /* Do one less for the terminator ('\n'). */
		char ch;

		for (;;)
		{
			if (buffering && bytes_in_buffer > 0)
			{
				ch = *current_byte_in_buffer++;
				bytes_in_buffer--;
			}
			else
			{
				error = read(&ch, sizeof(ch));
				if (error != ERRORCODE_None)
				{
					break;
				}
			}

			if (ch == '\r')
			{
				continue;
			}
			if (ch == '\n')
			{
				break;
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
