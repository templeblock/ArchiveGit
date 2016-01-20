/*
// $Header: /PM8/REMIND32/RECORD.CPP 1     3/03/99 6:27p Gbeddow $
//
// Database record routines.
//
// $Log: /PM8/REMIND32/RECORD.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:36   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:02   Fred
// Initial revision.
// 
//    Rev 1.3   04 Jun 1996 17:37:54   Jay
//  
// 
//    Rev 1.2   25 Apr 1996 10:37:40   Jay
// New final release notify code
// 
//    Rev 1.1   23 Apr 1996 08:07:36   Jay
// More new stuff
// 
//    Rev 1.0   14 Mar 1996 13:46:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:22   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:04   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:38:20   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   09 Jun 1994 11:05:46   JAY
// Added const typedefs for all types.
// Removed initialize().
// Added release_read_only().
// Added zap() method.
// 
//    Rev 1.2   31 May 1994 12:43:20   JAY
// Added a default destroy() method.
// 
//    Rev 1.1   13 Feb 1994 15:23:42   MACDUFF
// Added modification count to database record.
//
//   Rev 1.0   16 Dec 1993 16:25:24   JAY
// 
*/

#include "stdafx.h"

/*
// The constructor for the database record.
*/

DatabaseRecord::DatabaseRecord(DB_RECORD_NUMBER record_number, DB_RECORD_TYPE record_type, DatabasePtr owner, ST_DEV_POSITION far *in_where)
				: my_type(record_type), where(in_where), database(owner), m_nModificationCount(0)
{
	StorageElement::Init(ELEMENT_TYPE_RECORD, record_number, &owner->storage_manager);
}

/*
// Release this record.
*/

ERRORCODE DatabaseRecord::release(BOOL modify)
{
	if (modify)
	{
		modified();
	}
	return database->release_record(this);
}

/*
// Release this record in the database.
// This is like 'release' except it take a 'const' 'this'.
*/

ERRORCODE DatabaseRecord::release_read_only(void) const
{
	return database->release_record((DatabaseRecordPtr)this, TRUE);
}

/*
// Say we're modified.
*/

ERRORCODE DatabaseRecord::modified(VOID)
{
	++m_nModificationCount;
	return database->modified_record(this);
}

/*
// Delete (zap) me.
*/

ERRORCODE DatabaseRecord::zap(VOID)
{
	return database->delete_record(this);
}

/*
// Make a duplicate of this record.
// This standard method creates a new record and does an assignment.
*/

DatabaseRecordPtr DatabaseRecord::duplicate(DatabasePtr dup_database, LPERRORCODE lperror)
{
	DatabaseRecordPtr dup_record;
	ERRORCODE error;

/* Figure out where the new record goes. */

	if (dup_database == NULL)
	{
	/* Put it back in the same database. */
		dup_database = database;
	}

/* Create the new record. */

	if ((error = dup_database->new_record(type(), NULL, &dup_record)) == ERRORCODE_None)
	{
	/* Do the assignment. */
		error = dup_record->assign(*this);
	}
	if (lperror != NULL)
	{
		*lperror = error;
	}
	return dup_record;
}

/*
// The base object assignment method.
*/

ERRORCODE DatabaseRecord::assign(DatabaseRecordRef srecord)
{
// Nothing to do. Sorry to disappoint you!
	return ERRORCODE_None;
}

/*
// A record is being destroyed.
*/

void DatabaseRecord::destroy(void)
{
/* Nothing to do by default. */
}

/*
// This record is being released down to zero.
*/

void DatabaseRecord::FinalReleaseNotify(void)
{
/* Nothing to do by default. */
}

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

void DatabaseRecord::ReleaseSubrecords(void)
{
// Nothing to do by default.
}

/////////////////////////////////////////////////////////////////////////////
// Subrecord management.
// It is assumed that the subrecord's id is kept in the parent record.

// Generic subrecord locker.
void DatabaseRecord::LockSubrecord(DatabaseRecord*& pRecord, DB_RECORD_NUMBER& lRecord, DB_RECORD_TYPE nType, BOOL fCreate /*=FALSE*/)
{
	if (pRecord == NULL)
	{
		ERRORCODE error;

	/* We need to get the array. */

		if (lRecord == 0)
		{
			if (fCreate)
			{
			/* No record, yet. Allocate one. */
				pRecord = database->new_record(nType, NULL);
				if (pRecord != NULL)
				{
				/* Got the record. Remember its id. */
					lRecord = pRecord->Id();
					modified();
				}
				error = database->last_creation_error();
			}
			else
			{
			/* Don't have a record, and can't create. */
				error = ERRORCODE_ZeroRecord;
			}
		}
		else
		{
		/* Get the existing array record. */
			pRecord = database->get_record(lRecord, &error, nType);
		}

		if (pRecord == NULL)
		{
			ThrowErrorcodeException(error);
		}
	}
}

// Generic subrecord unlocker.
void DatabaseRecord::ReleaseSubrecord(DatabaseRecord*& pRecord)
{
	if (pRecord != NULL)
	{
		pRecord->release();
		pRecord = NULL;
	}
}

// Generic subrecord deleter.
void DatabaseRecord::DeleteSubrecord(DB_RECORD_NUMBER& lRecord, DB_RECORD_TYPE nType)
{
	if (lRecord != 0)
	{
		database->delete_record(lRecord, nType);
		lRecord = 0;
	}
}
