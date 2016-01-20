/*
// $Header: /PM8/App/EFILEREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// Graphic source record routines.
//
// $Log: /PM8/App/EFILEREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:20:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:18   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 09:06:26   Jay
// Matched read-only in get_record (with later release).
// 
//    Rev 1.0   14 Mar 1996 13:46:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:40   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:42   JAY
// Initial revision.
// 
//    Rev 1.8   09 Jan 1995 16:26:36   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.7   30 Nov 1994 16:34:56   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.6   04 Aug 1994 07:33:30   JAY
// Large model and 32-bit changes
// 
//    Rev 1.5   10 Jun 1994 10:04:50   JAY
// Fixed a minor glitch in the last rev.
// 
//    Rev 1.4   10 Jun 1994 10:01:24   JAY
// prep_file now always releases (or will cause a release).
// 
//    Rev 1.3   10 Jun 1994 09:54:56   JAY
// Some various debugging messages (commented out).
// 
//    Rev 1.2   09 Jun 1994 11:07:14   JAY
// Added prep_file().
// 
//    Rev 1.1   01 Jun 1994 16:35:26   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.0   23 May 1994 08:38:06   JAY
// Initial revision.
*/

#include "stdafx.h"

#include <string.h>

#include "file.h"
#include "efilerec.h"

/*****************************************************************************/
/*                           EmbeddedFile record                             */
/*****************************************************************************/

/*
// The creator for an embedded file record.
*/

ERRORCODE EmbeddedFileRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	EmbeddedFilePtr efile;

/* Create the new calendar info record. */

	if ((efile = new EmbeddedFileRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	if (creation_data != NULL)
	{
		ERRORCODE error;
		if ((error = efile->embed_file((LPCSTR)creation_data)) != ERRORCODE_None)
		{
			delete efile;
			return error;
		}
	}

	*record = (DatabaseRecordPtr)efile;

	return ERRORCODE_None;
}

/*
// The constructor for an embedded file record.
*/

EmbeddedFileRecord::EmbeddedFileRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&m_record, 0, sizeof(m_record));
}

/*
// The destructor.
*/

EmbeddedFileRecord::~EmbeddedFileRecord()
{
}

/*
// Embed a file in this record.
*/

ERRORCODE EmbeddedFileRecord::embed_file(LPCSTR filename)
{
/* Attach the file name. */

	m_csFileName = filename;

	ReadOnlyFile source_file(filename);

	return embed_file(source_file);
}

/*
// Embed a file in this record.
*/

ERRORCODE EmbeddedFileRecord::embed_file(StorageDevice& Source)
{
	ERRORCODE error;

/* Set up the two devices we will be dealing with. */

	StorageDevicePtr database_device = database->get_storage_device();

/* Get the length of the file. */

	if ((error = Source.length(&m_record.m_data_size)) != ERRORCODE_None)
	{
		return error;
	}

/* Finish filling out m_record. */

	m_record.m_type = EMBEDDED_TYPE_FILE;

/*
// We need storage to be allocated now.
// Preallocate us.
*/

	if ((error = Allocate(StorageElement::Type(), StorageElement::Id())) != ERRORCODE_None)
	{
		return error;
	}

	*where = m_position;

/* Compute where the data will be. */

	m_data_start = *where
			+ (SizeofData(database_device) - m_record.m_data_size);

/* Compute the range for the embedded file. */

	ST_DEV_POSITION start = m_data_start;
	ST_DEV_POSITION end = start + m_record.m_data_size;

/* Restrict ourselves to this range. */

	database_device->io_limits(start, end);

/* Create a device on top of the database one so we can write to it virtually. */

	StorageFile dest_file(database_device);

	dest_file.set_subfile(start, end);

	error = copy_file(&Source, &dest_file);

	if (error != ERRORCODE_None)
	{
		od("Got error %d while copying %ld bytes of data [%ld, %ld]\r\n",
				error, end - start, start, end);
	}

/* Restore unrestricted access. */

	database_device->io_limits();

	return error;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE EmbeddedFileRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&m_record, sizeof(m_record))) == ERRORCODE_None)
	{
		if ((error = device->read_cstring(m_csFileName)) == ERRORCODE_None)
		{
			error = device->tell(&m_data_start);
		}
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

ERRORCODE EmbeddedFileRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&m_record, sizeof(m_record))) == ERRORCODE_None)
	{
		if ((error = device->write_cstring(m_csFileName)) == ERRORCODE_None)
		{
			error = device->tell(&m_data_start);
		}
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
// This is the complete size of the written part and the file image.
*/

ST_MAN_SIZE EmbeddedFileRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(m_record))
				+ device->size_cstring(m_csFileName)
				+ m_record.m_data_size;
}

/*
// Assign method.
*/

ERRORCODE EmbeddedFileRecord::assign(DatabaseRecordRef srecord)
{
	EmbeddedFileRecord& record = (EmbeddedFileRecord&)srecord;
	ERRORCODE error;
/* Assign the base record first. */

	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
	/* Copy the file name over. */

		m_csFileName = record.m_csFileName;

	/* Use the built in routines to move the data over. */

		ReadOnlyFile SourceFile;

	/* Increase the lock count for this object (hack!). */

		record.database->get_record(record.Id(), NULL, record.type(), TRUE);

	/* Now do the file prep and copy. */

		if ((error = record.prep_file(&SourceFile, TRUE)) == ERRORCODE_None)
		{
			error = embed_file(SourceFile);
		}
	}
	return error;
}

/*
// RelocateData()
//
// This is a function called by the storage manager when our storage is
// ever relocated. This allows us to move any data we want saved.
// We need this so that we can move the file image.
*/

ERRORCODE EmbeddedFileRecord::RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
{
	ERRORCODE error;
	ST_DEV_POSITION new_data_pos = new_position
							+ (SizeofData(device) - m_record.m_data_size);

/* Move the file image from the old location to the new location. */

	if ((error = device->move(m_data_start, new_data_pos, m_record.m_data_size)) == ERRORCODE_None)
	{
	/* Data is now here. */
		m_data_start = new_data_pos;
	}
	return error;
}

/*
// The file completion notify for a prepped normal embedded file record.
*/

VOID EmbeddedFileRecord::efile_normal_completion(StorageFilePtr file, LPVOID data)
{
	EmbeddedFilePtr efile = (EmbeddedFilePtr)data;

//	od("Normal completion for record %ld\r\n", efile->id());

	efile->release();
}

/*
// The file completion notify for a prepped read-only embedded file record.
*/

VOID EmbeddedFileRecord::efile_readonly_completion(StorageFilePtr file, LPVOID data)
{
	EmbeddedFileCPtr efile = (EmbeddedFileCPtr)data;

//	od("Read-only completion for record %ld\r\n", efile->id());

	efile->release_read_only();
}

/*
// Prep a file using this record.
*/

ERRORCODE EmbeddedFileRecord::prep_file(StorageFilePtr file, BOOL read_only) const
{
	ERRORCODE error;

	if ((error = file->set_based_device(database->get_storage_device())) == ERRORCODE_None)
	{
		ST_DEV_POSITION start = get_data_start();
		ST_DEV_POSITION end = start + get_data_size();

	/* Total success. 'error' is set correctly. */

		od("Using subfile %ld to %ld\r\n", start, end);

		file->set_subfile(start, end);
		file->set_completion_notify(read_only
												? efile_readonly_completion
												: efile_normal_completion,
											 (LPVOID)this);

//		od("Prep for record %ld (%d)\r\n", id(), read_only);

	/* Make sure we can actually access the file. */

		if ((error = file->seek(0, ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
		/* Undo what we just did. */
//			od("Inaccessible!\r\n");
			file->reset();
		}
	}
	else
	{
	/* Caller assumes record release is always handled. So we handle it. */

		if (read_only)
		{
			release_read_only();
		}
		else
		{
		/*
		// Ugh! So the routine isn't entirely const...
		// But then, you should always pass TRUE for 'read_only' if it's
		// a const record anyway.
		*/
			((EmbeddedFilePtr)this)->release();
		}
	}

	return error;
}
