/*
// File definitions.
//
// $Header: /PM8/Download/FILE.H 1     3/03/99 6:15p Gbeddow $
//
// $Log: /PM8/Download/FILE.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 4     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 3     6/24/98 10:16a Rlovejoy
// Added flag to accomodate excel format text files.
// 
// 2     11/21/97 11:01a Jayn
// Moved StorageFileOnCFile from dbmap.* to file.*
// 
//    Rev 1.0   14 Aug 1997 15:20:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:50   Fred
// Initial revision.
// 
//    Rev 1.6   15 May 1997 17:05:40   dennisp
// Added capability to ignore characters between brackets in read_line
// 
//    Rev 1.5   23 Apr 1997 12:08:12   Jay
// path manager changes for collection items
// 
//    Rev 1.4   17 Apr 1997 16:05:50   Jay
// StorageFile routines can be made to throw exceptions
// 
//    Rev 1.3   13 Dec 1996 13:20:38   JAY
// Trivial variable renaming
// 
//    Rev 1.2   04 Nov 1996 16:05:34   Jay
// Separated out name generation for TmpFile
// 
//    Rev 1.1   18 Jul 1996 18:46:16   Fred
//  
// 
//    Rev 1.0   14 Mar 1996 13:47:00   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:44   Jay
// Changes in 2.1
// 
//    Rev 2.3   27 Feb 1996 16:43:14   JAY
// New filename translation system including subfile support.
// 
//    Rev 2.2   18 Aug 1995 15:04:44   JAY
// Static function to translate a file path name.
// 
//    Rev 2.1   13 May 1995 11:14:58   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:32   JAY
// Initial revision.
// 
//    Rev 1.12   29 Dec 1994 10:43:54   JAY
// Added get_based_device().
// 
//    Rev 1.11   30 Nov 1994 16:38:38   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.0   03 Nov 1994 12:56:34   FRED
//  
// 
//    Rev 1.1   01 Nov 1994 10:00:08   JAY
// New data manager routines
// 
//    Rev 1.0   25 Oct 1994 16:24:32   JAY
// Initial revision.
// 
//    Rev 1.10   04 Aug 1994 07:33:12   JAY
// Large model and 32-bit changes
// 
//    Rev 1.9   09 Jun 1994 11:00:48   JAY
// Added reset().
// Added file locking methods.
// Added completion notify hooks (when file is flushed()).
// 
//    Rev 1.8   31 May 1994 12:40:30   JAY
// IsAssigned() now works correctly with based devices.
// 
//    Rev 1.7   31 May 1994 10:41:36   JAY
// TextReadFile constructor now takes an LPCSTR.
// 
//    Rev 1.6   23 May 1994 08:28:16   JAY
// Added a PATH_TRANSLATOR.
// Added based_device support.
// Changed all member names to begin with "m_".
// 
//    Rev 1.5   13 May 1994 13:30:24   JAY
// Added "unbuffered_read" to ReadOnlyFile to help TIFF code with strip offsets.
// Fixed ReadOnlyFile::seek() to handle buffers > 32767.
// Increased size of TIFF read buffer.
// 
//    Rev 1.4   11 Mar 1994 14:22:48   JAY
// Added huge_read.
// 
//    Rev 1.3   04 Mar 1994 11:53:20   MACDUFF
// Added IsAssigned() method.
// 
//    Rev 1.2   08 Feb 1994 09:34:50   JAY
// Changed "storage.h" to "dbstore.h".
// Added PVCS headers.
*/

#ifndef __FILE_H__
#define __FILE_H__

#include "dbstore.h"				/* Define our base clase. */
#include "ipathmgr.h"			// IPathManager definitions.

/*
// The largest size to try and allocate for a buffer while copying a file.
*/

#define	MAX_ALLOC_SIZE		32000

/*
// Class structure:
//
//						StorageDevice					(abstract)
//								|
//				----- StorageFile ----				(first concrete base)
//				|							|
//		ReadOnlyFile				TmpFile
//				|
//		TextReadFile
*/

/*
// A storage file.
// The file has implicit opening (for any operation) and closing (when flushed).
*/

typedef class StorageFile far *StorageFilePtr;

class StorageFile : public StorageDevice
{
	friend class TmpFile;

protected:

/* Are we currently open? */

	BOOL						m_fOpen;
	BOOL						m_fUseExceptions;
	UINT						m_nOpenFlags;				/* Mode to open file with. */

/* The path translator. */

	typedef LPCSTR (*PATH_TRANSLATOR)(LPCSTR name, char *buffer, BOOL locate, StorageFile* pFile, LPVOID data);
	typedef VOID (*COMPLETION_NOTIFY)(StorageFilePtr file, LPVOID data);

	StorageDevicePtr		m_based_device;			/* Are we based on anything */

	CString					m_csName;					/* The name of the file. */
	ST_DEV_POSITION		m_current_position;		/* Current position. */

/* I/O limit variables. */

	ST_DEV_POSITION		m_limit_start;				/* start of I/O limits */
	ST_DEV_POSITION		m_limit_end;				/* end of I/O limits */

/* Subfile management variables. */

	ST_DEV_POSITION		m_subfile_start;			/* Subfile start position. */
	ST_DEV_POSITION		m_subfile_end;				/* Subfile end position. */

/* Completion notification. */

	COMPLETION_NOTIFY		m_completion_notify;
	LPVOID					m_completion_data;

/* The common construct code used by all constructors. */

	void common_construct(void);

/* Overridden method to open the storage file. */

	virtual ERRORCODE	Open(void);						/* open the file. */

/* Overridden method to provide the CFile for this device. */

	virtual CFile* NewCFile(void);
	virtual void DeleteCFile(CFile* pFile);
	virtual void CloseCFile(CFile* pFile);

/* The file which handles all of our transport. */

	CFile*					m_pFile;

	static IPathManager*		m_pPathManager;

	// A useful function to handle exception/errorcode branching on errors.
	// We put this inline to keep things speedy.
	ERRORCODE CheckError(ERRORCODE Error)
	{
		if (m_fUseExceptions && Error != ERRORCODE_None) ThrowErrorcodeException(Error);
		return Error;
	}
public:

/* Virtual functions from StorageDevice. */

	StorageFile(LPCSTR in_name = NULL);
	StorageFile(StorageDevicePtr existing_device);
	~StorageFile();
	virtual	ERRORCODE	reset(VOID);
	virtual	ERRORCODE	initialize(VOID);
	virtual	ERRORCODE	read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL);
	virtual	ERRORCODE	huge_read(LPVOID ptr, DWORD size, DWORD *size_read = NULL);
	virtual	ERRORCODE	write(LPVOID ptr, ST_DEV_IO_SIZE size);
	virtual	ERRORCODE	seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode);
	virtual	ERRORCODE	tell(ST_DEV_POSITION far *position);
	virtual	ERRORCODE	length(ST_DEV_POSITION far *size);
	virtual	ERRORCODE	truncate(VOID);
	virtual	ERRORCODE	flush(VOID);
	virtual	ERRORCODE	io_limits(ST_DEV_POSITION start = POSITION_UNKNOWN, ST_DEV_POSITION end = POSITION_UNKNOWN);
	virtual	ERRORCODE	lock(ST_DEV_POSITION range_start, ST_DEV_POSITION range_length, BOOL unlock = FALSE);

/* Delete (zap) the file. This removes it from the file system. */

				ERRORCODE	zap(void);

/* Additional functions specific to a StorageFile. */

				VOID			set_open_flags(UINT nOpenFlags) { m_nOpenFlags = nOpenFlags; }

				LPCSTR		get_name() { return m_csName; }
	virtual	VOID			clear_name(VOID);
	virtual	ERRORCODE	set_name(LPCSTR new_name);

				ERRORCODE	set_based_device(StorageDevicePtr device, BOOL fCheckName = TRUE);
				StorageDevicePtr get_based_device(void) const
					{ return m_based_device; }
				VOID			set_completion_notify(COMPLETION_NOTIFY notify, LPVOID data = NULL);

				void			UseExceptions(BOOL fUseExceptions)
					{ m_fUseExceptions = fUseExceptions; }

/*
// Create a subfile on this file.
// All accesses become relative to this subfile range.
//
// Call with no (aka default) arguments to turn off the subfile.
*/

	void set_subfile(ST_DEV_POSITION start = POSITION_UNKNOWN, ST_DEV_POSITION end = POSITION_UNKNOWN);

/*
// Set the path manager.
// This routine does not do an AddRef since it would never get Release'd.
// Ideally, it would; just make sure this pointer remains valid.
*/

	static void SetPathManager(IPathManager* pPathManager)
		{ m_pPathManager = pPathManager; }

/*
// Translate a file name.
*/

	static CString TranslateFileName(LPCSTR pName, BOOL fLocate, StorageFile* pFile = NULL);

	CString GetPathName(BOOL fLocate);

	virtual BOOL IsAssigned() const;

	static int m_nRetryCount;
	static DWORD m_dwRetryDelay;				// In milliseconds
};

/*
// A read-only file. The file MUST exist.
*/

class ReadOnlyFile : public StorageFile
{
private:
	void common_construct(void);
protected:
	// Variables for buffered reading.
	BOOL						m_fBuffering;		// Are we currently buffering?
	BYTE far *				m_pBuffer;			// The buffer we are using.
	ST_DEV_IO_SIZE			m_uBufferSize;		// The size of the allocated buffer.

	BYTE far *				m_pBufferPtr;		// Pointer into buffer.
	ST_DEV_IO_SIZE			m_uBufferLeft;		// How much data is left?

//	ST_DEV_POSITION		m_lBufferPosition;	// Where did we read?
public:
	ReadOnlyFile(LPCSTR pName = NULL);
	ReadOnlyFile(StorageDevicePtr existing_device);
	~ReadOnlyFile();

	virtual	ERRORCODE	initialize() { return ERRORCODE_Access; }
	virtual	ERRORCODE	write(LPVOID ptr, ST_DEV_IO_SIZE size) { return ERRORCODE_Access; }
	virtual	ERRORCODE	move(ST_DEV_POSITION src, ST_DEV_POSITION dst, ST_DEV_POSITION size) { return ERRORCODE_Access; }
	virtual	ERRORCODE	truncate() { return ERRORCODE_Access; }

	virtual	ERRORCODE	read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL);
	virtual	ERRORCODE	seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode);
	virtual	ERRORCODE	tell(ST_DEV_POSITION far *position);
	LPBYTE		fast_access(ST_DEV_IO_SIZE size);
	ERRORCODE	unbuffered_read(ST_DEV_POSITION position, LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL);

/*
// Set the read buffer for this file.
// If it fails (due to memory reasons), reads continue unbuffered.
*/

	void			set_read_buffer(ST_DEV_IO_SIZE size);
	void			free_read_buffer(void);

	void set_buffering(BOOL m_fWantBuffering)
		{ m_fBuffering = (m_fWantBuffering && m_pBuffer != NULL); }
};

typedef ReadOnlyFile far *ReadOnlyFilePtr;

/* Helper routine to copy a file. */

extern ERRORCODE copy_file(StorageDevicePtr source_file, StorageDevicePtr dest_file);

/*
// A temporary file.
*/

class TmpFile : public StorageFile
{
private:
	BOOL m_fCreated;

	void DeleteTempFile(void);

	virtual ERRORCODE	Open(void);						/* open the file. */

public:

	TmpFile(LPCSTR pPrefix = NULL);
	~TmpFile();

	// Generate the Tmp file name.
	BOOL GenerateTempName(LPCSTR pPrefix);

/* Initialize the temp file. */

	ERRORCODE initialize();

/* Set the name of the temp file. */

	ERRORCODE set_name(LPCSTR name) { return ERRORCODE_IntError;	}
	VOID clear_name();

/* Routines for getting and putting the tmp file's image. */

	ERRORCODE get_image(LPCSTR from_file_name);
	ERRORCODE put_image(LPCSTR to_file_name);
	ERRORCODE get_image(StorageDevicePtr from_device)
		{ 	return copy_file(from_device, this); }

	ERRORCODE put_image(StorageDevicePtr to_device)
		{ 	return copy_file(this, to_device); }

};

typedef TmpFile far *TmpFilePtr;

/*
// A read-only text file. The file MUST exist.
*/

#define TRF_skip_comments		1
#define TRF_skip_blank_lines	2
#define TRF_ignore_eol_in_brackets  4  // Ignore CR/LF between [] chars
#define TRF_excel_format		8

class TextReadFile : public ReadOnlyFile
{
public:
	TextReadFile(LPCSTR name);

	ERRORCODE	read_line(LPSTR buffer, int nMaxLine, WORD wFlags = 0);
	ERRORCODE	read_char(char* ch);
};

typedef TextReadFile far *TextReadFilePtr;

inline LPBYTE ReadOnlyFile::fast_access(ST_DEV_IO_SIZE size)
{
	if (!m_fBuffering || m_uBufferLeft < size)
	{
		return NULL;
	}
	LPBYTE p = m_pBufferPtr;

	m_pBufferPtr += size;
	m_uBufferLeft -= size;

	return p;
}

inline VOID StorageFile::set_completion_notify(COMPLETION_NOTIFY notify, LPVOID data)
{
	m_completion_notify = notify;
	m_completion_data = data;
}

class StorageFileOnCFile : public StorageFile
{
public:
	StorageFileOnCFile(CFile* pFile = NULL);
	~StorageFileOnCFile();

/* Overridden method to open the storage file. */

	virtual	ERRORCODE initialize(VOID);
	virtual ERRORCODE	Open(void);						/* open the file. */

	virtual CFile* NewCFile(void);
	virtual void DeleteCFile(CFile* pFile);
	virtual void CloseCFile(CFile* pFile);

	void SetCFile(CFile* pFile = NULL);
	CFile* GetCFile(void) const;

	virtual BOOL IsAssigned() const;
};

#if 0
class CFileOnStorageDevice : public CFile
{
public:
// Constructors
	CFileOnStorageDevice(StorageDevice* pDevice);
	virtual ~CFileOnStorageDevice();

// Attributes
	StorageDevice* m_pDevice;

	virtual DWORD GetPosition() const;

// Operations
	virtual BOOL Open(const char* pszFileName, UINT nOpenFlags,
		CFileException* pError = NULL);

// Overridables
	virtual CFile* Duplicate() const;

	virtual LONG Seek(LONG lOff, UINT nFrom);
	virtual void SetLength(DWORD dwNewLen);
	virtual DWORD GetLength() const;

	virtual UINT Read(void FAR* lpBuf, UINT nCount);
	virtual void Write(const void FAR* lpBuf, UINT nCount);

	virtual void LockRange(DWORD dwPos, DWORD dwCount);
	virtual void UnlockRange(DWORD dwPos, DWORD dwCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

};
#endif

#endif				/* #ifndef __FILE_H__ */
