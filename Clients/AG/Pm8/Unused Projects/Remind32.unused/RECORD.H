
/*
// $Header: /PM8/REMIND32/RECORD.H 1     3/03/99 6:27p Gbeddow $
//
// Database record definitions.
//
// $Log: /PM8/REMIND32/RECORD.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:56   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:28   Fred
// Initial revision.
// 
//    Rev 1.13   28 Jul 1996 11:53:44   Jay
// Got rid of calendar and brochure info records.
// 
//    Rev 1.12   28 Jul 1996 10:16:34   Jay
// Added brochure record.
// 
//    Rev 1.11   10 Jul 1996 10:25:22   Jay
// Text states no longer allocated.
// 
//    Rev 1.10   26 Jun 1996 14:22:36   Jay
//  
// 
//    Rev 1.9   04 Jun 1996 17:38:56   Jay
//  
// 
//    Rev 1.8   25 Apr 1996 12:16:26   Jay
//  
// 
//    Rev 1.7   25 Apr 1996 10:37:42   Jay
// New final release notify code
// 
//    Rev 1.6   24 Apr 1996 08:17:16   Fred
// Moved Array record type
// 
//    Rev 1.5   23 Apr 1996 08:07:38   Jay
// More new stuff
// 
//    Rev 1.4   12 Apr 1996 13:06:18   Jay
// Added new frame type.
// 
//    Rev 1.3   12 Apr 1996 12:59:42   Jay
// Added new text types.
// 
//    Rev 1.2   12 Apr 1996 12:48:58   Jay
// Added new text record types.
// 
//    Rev 1.1   11 Apr 1996 15:35:36   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:28   FRED
// Initial revision.
// 
//    Rev 2.5   11 May 1995 09:36:36   JAY
// Added OutlinePath record type.
// 
//    Rev 2.4   02 May 1995 15:37:12   JAY
// Added access function for the database of a record.
// 
//    Rev 2.3   27 Apr 1995 13:21:34   FRED
//  
// 
//    Rev 2.2   19 Apr 1995 12:31:28   JAY
//  
// 
//    Rev 2.1   29 Mar 1995 08:58:32   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:48:56   JAY
// Initial revision.
// 
//    Rev 1.6   30 Nov 1994 16:33:20   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.5   09 Jun 1994 11:04:36   JAY
// Added const typedefs for all types.
// Removed initialize().
// Added release_read_only().
// Added zap() method.
// 
//    Rev 1.4   01 Jun 1994 16:41:16   JAY
// Added record types for FileDatabase.
// 
//    Rev 1.3   23 May 1994 08:26:16   JAY
// Added EmbeddedFileRecord.
// Added a destroy() method to be called when the record is permanently deleted
// from the database.
// 
//    Rev 1.2   13 Feb 1994 15:23:28   MACDUFF
// Added modification count to database record.
// 
//    Rev 1.1   05 Jan 1994 08:08:24   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:56   JAY
//  
*/

#ifndef __RECORD_H__
#define __RECORD_H__

#include	"storeman.h"

/*
// A database record number (previously "INODE").
// Record numbers start from 1. Passing a zero as a record number
// to a database routine is considered an error.
*/

typedef ST_MAN_ID DB_RECORD_NUMBER;

/*
// A database record type.
// Record types start at 1. Passing a zero as a record type to a database
// routine is considered an error. The special record type of DB_ANY_TYPE
// may be passed to ignore types in certain routines.
*/

typedef SHORT DB_RECORD_TYPE;

#define	DB_NO_TYPE		0
#define	DB_ANY_TYPE		-1

/*
// A forward declaration for a database.
// Each record has a pointer to its owning database.
*/

typedef class Database far *DatabasePtr;

/*
// A database record.
// Anything which will be stored in the database must be derived from
// this base record object type.
//
// This object is a storage element, and is managed by the storage manager.
*/

typedef class DatabaseRecord far *DatabaseRecordPtr;
typedef class DatabaseRecord far &DatabaseRecordRef;
typedef const class DatabaseRecord far *DatabaseRecordCPtr;
typedef const class DatabaseRecord far &DatabaseRecordCRef;

class DatabaseRecord : public StorageElement
{
private:
	DB_RECORD_TYPE		my_type;			/* What kind of fool am I? */
	int m_nModificationCount;

protected:

	ST_DEV_POSITION far *where;		/* In case I need to move myself. */
	DatabasePtr			database;		/* Who we belong to. */

public:

/*
// The constructor for a database record.
// A database record must have a record number.
*/

	DatabaseRecord(DB_RECORD_NUMBER record_number, DB_RECORD_TYPE record_type, DatabasePtr owner, ST_DEV_POSITION far *in_where);

/*
// read()
//
// Read this record. Since we maintain a pointer back to the table,
// we can do it all ourselves. This implies, of course, that our table
// is always locked in memory while we exist.
// This invokes the StorageElement read() method.
*/

	ERRORCODE read() { return StorageElement::Read(*where); }

/*
// write()
//
// Write this record. Since we maintain a pointer back to the table,
// we can do it all ourselves. This implies, of course, that our table
// is always locked in memory while we exist.
// This invokes the StorageElement write() method.
*/

	ERRORCODE write() { return StorageElement::Write(where); }

/*
// Return the type of this record.
*/

	DB_RECORD_TYPE		type() { return my_type; }

/*
// Release this record in the database.
// Equivalent to database->release_record(this);
// If modify is TRUE, modified() is called as well.
//
// To release a 'read_only' record, call release_read_only().
*/

	ERRORCODE release(BOOL modify = FALSE);

/*
// Release this record in the database.
// Equivalent to database->release_record((DatabaseRecordPtr)this, TRUE);
//
// This is like release() except it takes a const 'this'.
*/

	ERRORCODE release_read_only(void) const;

/*
// Say this record has been modified.
// Equivalent to database->modified_record(this);
*/

	ERRORCODE modified(VOID);

/*
// Delete this record.
// Equivalent to database->delete_record(this);
*/

	ERRORCODE zap(VOID);

/*
// Duplicate this record.
*/

	virtual DatabaseRecordPtr duplicate(DatabasePtr new_database = NULL, LPERRORCODE lperror = NULL);

/*
// Assign method.
// This could ALMOST have been the assignment operator. Oh, well...
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// This record is being released down to zero.
*/

	virtual void FinalReleaseNotify(void);

/*
// The database is going away. All records are being freed.
// Release any sub-records we have locked.
*/

	virtual void ReleaseSubrecords(void);

/*
// How many times has this object been modified?
*/

	int GetModificationCount() const { return m_nModificationCount; }

/*
// Return the database this record belongs to.
*/

	DatabasePtr GetDatabase(void)
		{ return database; }

/*
// Subrecord management.
// It is assumed that the subrecord's id is kept in the parent record.
*/

	// Generic subrecord locker.
	void LockSubrecord(DatabaseRecord*& pRecord, DB_RECORD_NUMBER& lRecord, DB_RECORD_TYPE nType, BOOL fCreate = FALSE);
	// Generic subrecord unlocker.
	void ReleaseSubrecord(DatabaseRecord*& pRecord);
	// Generic subrecord deleter.
	void DeleteSubrecord(DB_RECORD_NUMBER& lRecord, DB_RECORD_TYPE nType);
};

/*
// Record types for a PMGDatabase.
*/

typedef enum
{
	RECORD_TYPE_ProjectPreview = 1,
	RECORD_TYPE_Document,
	RECORD_TYPE_FontData,
	RECORD_TYPE_Page,
	RECORD_TYPE_Graphic,
	RECORD_TYPE_OldFrame,
	RECORD_TYPE_Paragraph,
	RECORD_TYPE_ObjectLayout,
	UNUSED1,							// Was CalendarInfo
	UNUSED2,							// Was BrochureInfo (briefly)
	RECORD_TYPE_EmbeddedFile,
	RECORD_TYPE_MacroList,
	RECORD_TYPE_NameList,
	RECORD_TYPE_WarpField,
	RECORD_TYPE_OutlinePath,

	// Record types above are used in PMW 2.1 (or less) documents

	RECORD_TYPE_Array,					// Newly redesigned array
	RECORD_TYPE_ArrayBlock,				// Subtype for new Array record (Must be Array+1)
	RECORD_TYPE_RecordArray,			// New general array (of DB_RECORD_NUMBER)
	RECORD_TYPE_RecordArrayBlock,
	RECORD_TYPE_CharacterArray,		// New text type
	RECORD_TYPE_CharacterArrayBlock,
	RECORD_TYPE_WordArray,				// New text type
	RECORD_TYPE_WordArrayBlock,
	RECORD_TYPE_StyleArray,				// New text type
	RECORD_TYPE_StyleArrayBlock,
	RECORD_TYPE_ParagraphArray,		// New text type
	RECORD_TYPE_ParagraphArrayBlock,
	RECORD_TYPE_LineArray,				// New text type
	RECORD_TYPE_LineArrayBlock,
	RECORD_TYPE_Text,						// New text type
	RECORD_TYPE_Frame,					// New text type
	RECORD_TYPE_TextStyle,				// New text type
} PMG_RECORD_TYPE;

class ProjectPreviewRecord;
typedef ProjectPreviewRecord far *ProjectPreviewPtr;
typedef ProjectPreviewRecord far &ProjectPreviewRef;

class DocumentRecord;
typedef DocumentRecord far *DocumentPtr;

class FontDataRecord;
typedef FontDataRecord far *FontDataPtr;

class PageRecord;
typedef PageRecord far *PagePtr;

class GraphicRecord;
typedef GraphicRecord far *GraphicPtr;
typedef GraphicRecord far &GraphicRef;

class FrameRecord;
typedef FrameRecord far *FramePtr;
typedef FrameRecord far &FrameRef;

class ParagraphRecord;
typedef ParagraphRecord far *ParagraphPtr;
typedef ParagraphRecord far &ParagraphRef;

class ObjectLayoutRecord;
typedef ObjectLayoutRecord far *ObjectLayoutPtr;

class CalendarInfoRecord;
typedef CalendarInfoRecord far *CalendarInfoPtr;

class ArrayRecord;
typedef ArrayRecord far *ArrayRecordPtr;
typedef ArrayRecord far &ArrayRecordRef;

class EmbeddedFileRecord;
typedef EmbeddedFileRecord far *EmbeddedFilePtr;
typedef const EmbeddedFileRecord far *EmbeddedFileCPtr;

class WarpFieldRecord;
class OutlinePathRecord;

#endif						/* #ifndef __RECORD_H__ */

