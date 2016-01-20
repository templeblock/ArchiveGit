/*
// Embedded file record.
//
// $Workfile: EFILEDAT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/EFILEDAT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:16   Fred
// Initial revision.
// 
//    Rev 1.2   15 Aug 1996 15:56:26   Jay
// Improved read/write/sizeofdata
// 
//    Rev 1.1   24 May 1996 16:12:46   Fred
// TRACEx
// 
//    Rev 1.0   14 Mar 1996 13:33:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:18   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:36:22   JAY
// Reverted. New series.
// 
//    Rev 1.2   08 Nov 1994 11:14:30   JAY
// Strips off path to name before storing it in the document.
// 
//    Rev 1.1   04 Nov 1994 09:55:04   JAY
// Added ExtractFile().
// 
//    Rev 1.0   03 Nov 1994 15:53:52   JAY
// Initial revision.
// 
//    Rev 1.0   02 Nov 1994 12:54:48   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "efiledat.h"
#include "file.h"
#include "util.h"

/*****************************************************************************/
/*                           EmbeddedFile record                             */
/*****************************************************************************/

/*
// The creator for an embedded file record.
*/

CStandardDataRecord* CEmbeddedFileData::Create()
{
	return new CEmbeddedFileData;
}

/*
// The initialization routine for the embedded file record.
*/

BOOL CEmbeddedFileData::Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where)
{
	if (CStandardDataRecord::Init(id, type, pManager, NULL, in_where))
	{
		if (pData != NULL)
		{
			if (EmbedFile((LPCSTR)pData) != ERRORCODE_None)
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*
// The constructor for an embedded file record.
*/

CEmbeddedFileData::CEmbeddedFileData()
{
	memset(&m_Record, 0, sizeof(m_Record));
}

/*
// The destructor.
*/

CEmbeddedFileData::~CEmbeddedFileData()
{
}

/*
// Embed a file in this record.
*/

ERRORCODE CEmbeddedFileData::EmbedFile(LPCSTR pFileName)
{
	ERRORCODE error;

/* Attach the file name. */

	TRY
	{
#if 0
		m_csFileName = pFileName;
#else
	// Save only the base file name.
		Util::SplitPath(pFileName, NULL, &m_csFileName);
#endif
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_Memory;
	}
	END_CATCH_ALL

/* Set up the two devices we will be dealing with. */

	StorageDevicePtr pDevice = m_pManager->GetStorageDevice();
	ReadOnlyFile SourceFile(pFileName);

/* Get the length of the file. */

	if ((error = SourceFile.length(&m_Record.m_DataSize)) != ERRORCODE_None)
	{
		return error;
	}

/* Finish filling out m_record. */

	m_Record.m_Type = EMBEDDED_TYPE_FILE;

/*
// We need storage to be allocated now.
// Preallocate us.
*/

	if ((error = Allocate(StorageElement::Type(), StorageElement::Id())) != ERRORCODE_None)
	{
		return error;
	}

	*m_pWhere = m_position;

/* Compute where the data will be. */

	m_DataStart = *m_pWhere
			+ (SizeofData(pDevice) - m_Record.m_DataSize);

/* Compute the range for the embedded file. */

	ST_DEV_POSITION start = m_DataStart;
	ST_DEV_POSITION end = start + m_Record.m_DataSize;

/* Restrict ourselves to this range. */

	pDevice->io_limits(start, end);

/* Create a device on top of the database one so we can write to it virtually. */

	StorageFile DestFile(pDevice);

	DestFile.set_subfile(start, end);

	error = copy_file(&SourceFile, &DestFile);

	if (error != ERRORCODE_None)
	{
//		TRACE("Got error %d while copying %ld bytes of data [%ld, %ld]\n",
//				error, end - start, start, end);
	}

/* Restore unrestricted access. */

	pDevice->io_limits();

	return error;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CEmbeddedFileData::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::ReadData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
		 && (error = pDevice->read_cstring(m_csFileName)) == ERRORCODE_None)
	{
		error = pDevice->tell(&m_DataStart);
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
// Only the record is written. The actual data should already have been
// written.
*/

ERRORCODE CEmbeddedFileData::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::WriteData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None
		 && (error = pDevice->write_cstring(m_csFileName)) == ERRORCODE_None)
	{
		error = pDevice->tell(&m_DataStart);
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
// This is the complete size of the written part and the file image.
*/

ST_MAN_SIZE CEmbeddedFileData::SizeofData(StorageDevicePtr pDevice)
{
	return CStandardDataRecord::SizeofData(pDevice)
				+ pDevice->size_record(sizeof(m_Record))
				+ pDevice->size_cstring(m_csFileName)
				+ m_Record.m_DataSize;
}

/*
// Assign method.
*/

void CEmbeddedFileData::Assign(const CDataRecord& Record)
{
/* Assign the base record first. */
	CStandardDataRecord::Assign(Record);
/**** INCOMPLETE ****/
// This NEEDS to take into account transferring between databases.
// Perhaps EmbedFile should take a StorageFile (optionally) which could
// be a mapped onto this database. Then we could just call EmbedFile to
// move the data across.

}

/*
// RelocateData()
//
// This is a function called by the storage manager if our storage is
// ever relocated. This allows us to move any data we want saved.
// We need this so that we can move the file image.
*/

ERRORCODE CEmbeddedFileData::RelocateData(StorageDevicePtr pDevice, ST_DEV_POSITION OldPosition, ST_DEV_POSITION NewPosition)
{
	ERRORCODE error;
	ST_DEV_POSITION NewDataPos = NewPosition
							+ (SizeofData(pDevice) - m_Record.m_DataSize);

/* Move the file image from the old location to the new location. */

	if ((error = pDevice->move(m_DataStart, NewDataPos, m_Record.m_DataSize)) == ERRORCODE_None)
	{
	/* Data is now here. */
		m_DataStart = NewDataPos;
	}
	return error;
}

/*
// The file completion notify for a prepped normal embedded file record.
*/

void CEmbeddedFileData::EfileNormalCompletion(StorageFile* pFile, LPVOID pData)
{
	CEmbeddedFileData* efile = (CEmbeddedFileData*)pData;

//	TRACE1("Normal completion for record %ld\n", efile->Id());

	efile->Release();
}

/*
// The file completion notify for a prepped read-only embedded file record.
*/

void CEmbeddedFileData::EfileReadonlyCompletion(StorageFile* pFile, LPVOID pData)
{
	const CEmbeddedFileData* efile = (const CEmbeddedFileData*)pData;

//	TRACE1("Read-only completion for record %ld\n", efile->Id());

	efile->ReleaseReadOnly();
}

/*
// Prep a file using this record.
*/

ERRORCODE CEmbeddedFileData::PrepFile(StorageFile* pFile, BOOL fReadOnly) const
{
	ERRORCODE error;

	if ((error = pFile->set_based_device(m_pManager->GetStorageDevice())) == ERRORCODE_None)
	{
		ST_DEV_POSITION start = DataStart();
		ST_DEV_POSITION end = start + DataSize();

	/* Add a reference to us since the StorageFile now "owns" us. */

		((CEmbeddedFileData*)this)->AddRef();

	/* Total success. 'error' is set correctly. */

		TRACE2("Using subfile %ld to %ld\n", start, end);

		pFile->set_subfile(start, end);
		pFile->set_completion_notify(fReadOnly
												? EfileReadonlyCompletion
												: EfileNormalCompletion,
											 (LPVOID)this);

//		TRACE2("Prep for record %ld (%d)\n", Id(), fReadOnly);

	/* Make sure we can actually access the file. */

		if ((error = pFile->seek(0, ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
		/* Undo what we just did. */
//			od("Inaccessible!\r\n");
			pFile->reset();
		}
	}

	return error;
}

/*
// Extract the file from this record.
*/

ERRORCODE CEmbeddedFileData::ExtractFile(LPCSTR pFileName)
{
	ERRORCODE error;

/* Set up the two devices we will be dealing with. */

	StorageDevicePtr pDevice = m_pManager->GetStorageDevice();

/* Compute the range for the embedded file. */

	ST_DEV_POSITION start = m_DataStart;
	ST_DEV_POSITION end = start + m_Record.m_DataSize;

/* Restrict ourselves to this range. */

	pDevice->io_limits(start, end);

/* Create a device on top of the database one so we can read from it virtually. */

	ReadOnlyFile SourceFile(pDevice);

	SourceFile.set_subfile(start, end);

/* Set up the destination file so we can write to it. */

	StorageFile DestFile(pFileName);

	error = copy_file(&SourceFile, &DestFile);

/* Restore unrestricted access. */

	pDevice->io_limits();

	return error;
}
