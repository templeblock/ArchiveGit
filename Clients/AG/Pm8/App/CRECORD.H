/*
// $Header: /PM8/App/CRECORD.H 1     3/03/99 6:04p Gbeddow $
//
// Database record definitions.
//
// $Log: /PM8/App/CRECORD.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:18   Fred
// Initial revision.
// 
//    Rev 1.1   30 Aug 1996 10:45:20   Jay
// Keyword/item stuff
// 
//    Rev 1.0   14 Mar 1996 13:26:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:40   FRED
// Initial revision.
// 
//    Rev 2.2   15 Sep 1995 14:04:50   JAY
// More 32-bit changes.
// 
//    Rev 2.1   08 Feb 1995 13:39:18   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:46   FRED
//  
// 
//    Rev 1.2   01 Nov 1994 09:58:22   JAY
// New data manager routines
// 
//    Rev 1.1   26 Oct 1994 14:46:44   JAY
// Removed extraneous lines.
// 
//    Rev 1.0   25 Oct 1994 16:23:42   JAY
// Initial revision.
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

class CStandardDataRecordState;
class CStandardDataRecord;

#ifndef __CRECORD_H__
#define __CRECORD_H__

#include	"storeman.h"
#include	"cdataman.h"

class CStandardDataManager;

#ifndef __RECORD_H__
/*
// A database record number (previously "INODE").
// Record numbers start from 1. Passing a zero as a record number
// to a database routine is considered an error.
*/

typedef DATAID DB_RECORD_NUMBER;

/*
// A database record type.
// Record types start at 1. Passing a zero as a record type to a database
// routine is considered an error. The special record type of DB_ANY_TYPE
// may be passed to ignore types in certain routines.
*/

typedef DATATYPE DB_RECORD_TYPE;

#define	DB_NO_TYPE		((DATATYPE)0)
#define	DB_ANY_TYPE		((DATATYPE)-1)
#endif				// #ifndef __RECORD_H__

/*
// A standard data record.
// Anything which will be stored in the data manager must be derived from
// this base record object type.
//
// This object is a storage element, and is managed by the storage manager.
*/

class CStandardDataRecordState : public CDataRecordState
{
public:
	CStandardDataRecordState();
	~CStandardDataRecordState();
	virtual void Init(void);

	DECLARE_OBJECT_PROPERTIES(CStandardDataRecordState);
	DEFINE_OBJECT_PROPERTY_COPY()
};

class CStandardDataRecord : public CDataRecord, public StorageElement
{
public:

/*
// Constructor
*/

	CStandardDataRecord();

/*
// Destructor
*/

	virtual ~CStandardDataRecord();

/*
// Init()
//
// Initialize this record.
*/

	virtual BOOL Init(DATAID record_number, DATATYPE record_type, CStandardDataManager* owner, LPVOID pData, ST_DEV_POSITION* in_where);

/////////////////////////////////////////////
// Functions we override from CDataRecord. //
/////////////////////////////////////////////

	virtual CDataManager* Manager(void) const;		// Who manages us?
	virtual DATAID Id(void) const;						// What is our id?
	virtual DATATYPE Type(void) const;					// What is our type?

	virtual DWORD RecordAddRef(void);
	virtual DWORD RecordRelease(void);
	virtual DWORD RecordRefCount(void) const;

/*
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual void OnDelete(void);

	// The record is being flushed. Flush any subrecords.
	// Watch out for infinite recursion!
	virtual void OnFlush(void);

private:
	DATATYPE			m_nType;				/* What kind of fool am I? */
	int				m_nModifications;

protected:

	ST_DEV_POSITION*			m_pWhere;		/* In case I need to move myself. */
	CStandardDataManager*	m_pManager;		/* Who we belong to. */
	DWORD							m_dwRecordRef;

	DEFINE_PROPERTY_STATE(CStandardDataRecordState)

public:

/*
// Read()
//
// Read this record. Since we maintain a pointer back to the table,
// we can do it all ourselves. This implies, of course, that our table
// is always locked in memory while we exist.
// This invokes the StorageElement read() method.
// Do not override this (it's not virtual... you can't). Instead,
// override ReadData.
*/

	ERRORCODE Read(void)
		{ return StorageElement::Read(*m_pWhere); }

/*
// Write()
//
// Write this record. Since we maintain a pointer back to the table,
// we can do it all ourselves. This implies, of course, that our table
// is always locked in memory while we exist.
// This invokes the StorageElement write() method.
// Do not override this (it's not virtual... you can't). Instead,
// override WriteData.
*/

	ERRORCODE Write(void)
		{ return StorageElement::Write(m_pWhere); }

/*
// Release this record in the database.
// Equivalent to database->release_record(this);
// If modify is TRUE, modified() is called as well.
//
// To release a 'read_only' record, call release_read_only().
*/

	virtual DWORD AddRef(void);
	virtual DWORD Release(void);

/*
// Release this record in the database.
// Equivalent to database->release_record((CDataRecord*)this, TRUE);
//
// This is like release() except it takes a const 'this'.
*/

	virtual void ReleaseReadOnly(void) const;

/*
// Say this record has been modified.
*/

	virtual void Modified(VOID);

/*
// Duplicate this record.
*/

//	virtual CDataRecord* Duplicate(CDataManager* pDupManager = NULL);

	// How many times has this object been modified?

	int GetModifications(void) const
		{ return m_nModifications; }

/******** StorageElement support ********/

/*
// ReadData()
//
// This is a method defined by a class derived from this one.
// It handles the reading of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// This is a method defined by a class derived from this one.
// It handles the writing of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// This is a method defined by a class derived from this one.
// It returns the size of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
//
// A pointer to the device is passed to allow access to the device's
// sizing routines.
*/

	virtual	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif						/* #ifndef __CRECORD_H__ */
