/*
// $Workfile: CDATAMAN.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDATAMAN.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:58   Fred
// Initial revision.
// 
//    Rev 1.3   04 Oct 1996 10:43:56   Jay
// Improvements, etc.
// 
//    Rev 1.2   01 Sep 1996 12:41:34   Jay
// More configurable albums
// 
//    Rev 1.1   30 Aug 1996 10:45:50   Jay
// Keyword/item stuff
// 
//    Rev 1.0   14 Mar 1996 13:27:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:02   FRED
// Initial revision.
// 
//    Rev 2.4   15 Sep 1995 14:04:50   JAY
// More 32-bit changes.
// 
//    Rev 2.3   13 Jun 1995 08:25:40   JAY
// Shareable album
// 
//    Rev 2.2   09 Jun 1995 10:49:10   JAY
// Loading and saving.
// 
//    Rev 2.1   08 Feb 1995 13:38:52   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:55:38   FRED
//  
// 
//    Rev 1.11   01 Nov 1994 09:59:44   JAY
// New data manager routines
// 
//    Rev 1.10   26 Oct 1994 12:00:42   JAY
// Removing 'WithChildren' stuff.
// 
//    Rev 1.9   26 Oct 1994 10:23:48   JAY
// Removed ULONG. CBaseObjects class.
// 
//    Rev 1.8   25 Oct 1994 16:21:26   JAY
// Release() now returns a ULONG.
// 
//    Rev 1.7   19 Oct 1994 08:40:10   JAY
// Changed the way deleting a record works.
// 
//    Rev 1.6   19 Oct 1994 08:11:46   JAY
// Restructuring
// 
//    Rev 1.5   14 Oct 1994 09:33:06   JAY
// Notify. Consistent use of AddRef() and Release()
// 
//    Rev 1.4   06 Oct 1994 13:19:00   JAY
// CDataRecord is now a property object.
// 
//    Rev 1.3   22 Sep 1994 11:21:38   JAY
// Added Duplicate and Assign methods to CDataRecord.
// Added DuplicateRecord to CDataManager.
// Added AllocateItemID to CDataManager.
// 
//    Rev 1.2   15 Sep 1994 14:46:44   JAY
// Added modified logic.
// Changed CreateRecord() to NewRecord().
// 
//    Rev 1.1   15 Sep 1994 08:24:06   JAY
// Generic implementation of Read/Write (I/O) functions.
// 
//    Rev 1.0   12 Sep 1994 10:47:32   JAY
// Initial revision.
*/

class CDataRecord;
class CDataRecordState;
class CDataManager;

#ifndef __CDATAMAN_H__
#define __CDATAMAN_H__

#include "bookdefs.h"

/////////////////////////////////////////////////////////////////////////////
// CDataRecordStub

class CDataRecordStub : public CPropertyObject
{
public:
	DATAID m_idData;
	DATAID Id(void) const
		{ return m_idData; }
private:
	virtual CPropertyState* CreateState(void) const
		{ return NULL; }
};

/////////////////////////////////////////////////////////////////////////////
// CDataRecordState

class CDataRecordState : public CPropertyState
{
public:

	// management

	CDataRecordState();
	virtual ~CDataRecordState();

	// implementation

	DECLARE_OBJECT_PROPERTIES(CDataRecordState)
public:
};

/////////////////////////////////////////////////////////////////////////////
// CDataRecord

/*
// This is the base component for any data managed by a CDataManager.
//
// The main attributes of a data record are its id, its type and the
// manager that manages it. Everything else useful will be done in
// user derived classes.
*/

class CDataRecord : public CPropertyObject
{
/*
// Construction/destruction.
*/
public:
	CDataRecord();
	virtual ~CDataRecord();

/*
// Properties.
*/
	DEFINE_PROPERTY_STATE(CDataRecordState)

/*
// Data access.
*/

public:
	virtual CDataManager* Manager(void) const = 0;		// Who manages us?
	virtual DATAID Id(void) const = 0;							// What is our id?
	virtual DATATYPE Type(void) const = 0;					// What is our type?

/*
// Reference counts (with respect to the data manager, not memory).
*/

public:
	virtual DWORD RecordAddRef(void) = 0;
	virtual DWORD RecordRelease(void) = 0;

	virtual DWORD Release(void);
	virtual DWORD ReleaseReadOnly(void);

protected:
/* Small functions to provide limited access to CDataManager (as friends). */
	void SelfRelease(void);
	void SelfDestruct(void);

/*
// Notification functions.
*/

public:
/* This is called when the data manager is about to delete us. */
	virtual void OnDelete(void) = 0;
/* This is called when the data manager is about to flush us. */
	virtual void OnFlush(void) = 0;

/*
// Duplication functions.
*/
public:
	virtual CDataRecord* Duplicate(CDataManager* pDupManager = NULL);
	virtual void Assign(const CDataRecord& Record);

/*
// Say this record has been modified.
*/

	virtual void Modified(VOID) = 0;

/*
// Some child features.
// Children now require a release when released.
*/
public:
	// Add a child: What happened to you?
	virtual CDataRecord* ChildRecord(int nIndex, BOOL fReadOnly = FALSE);
	virtual CDataRecord* NewChildRecord(DATATYPE wType, LPVOID pData = NULL, int nIndex = -1);
	virtual CDataRecord* FindChildRecord(DATAID dwID, BOOL fReadOnly = FALSE);
	virtual void DeleteChildRecord(DATAID dwID);
	virtual void DeleteChildRecord(CDataRecord* pChild);
	virtual void MoveChildRecord(DATAID dwID, int nIndex);
	virtual void MoveChildRecord(CDataRecord* pChild, int nIndex);

	int GetChildIndex(CDataRecord* pRecord)
		{ return GetChildIndex(pRecord->Id()); }
	int GetChildIndex(DATAID dwID);
};

/*
// This is the data manager which manages the CDataRecord objects.
// This includes allocating, deleting, getting and releasing them
// (by id when necessary).
//
// This particular class is abstract. In fact, it has little code (the
// virtual destructor which must exist, and DeleteRecord(id) which is a
// helper function).
*/

class CDataManager : public CBaseObject
{
/*
// Construction/destruction.
*/
public:
	virtual ~CDataManager();

/*
// Initialization.
*/

	virtual void Init(void);

/*
// Data record support functions.
*/
protected:
/* Release a record. */

	friend void CDataRecord::SelfRelease(void);
	virtual void ReleaseRecord(CDataRecord *pRecord, BOOL fReadOnly = FALSE) = 0;

/* Delete a record. */

	friend void CDataRecord::SelfDestruct(void);
	virtual void DeleteRecord(CDataRecord* pRecord) = 0;

/*
// Record management.
*/

public:
/*
// Create a record, invoking its creator method with the passed creation data.
// Once the record is created, you can get its id from the id() method.
// The record is considered to have a single lock on it at creation time and
// must be Released (use the record's Release() method.)
*/

	virtual CDataRecord* NewRecord(DATATYPE type, LPVOID pData = NULL) = 0;

/* Get a pointer the to the record structure given its id. */

	virtual CDataRecord* GetRecord(DATAID idRecord, DATATYPE dwType = DB_ANY_TYPE, BOOL fReadOnly = FALSE, ERRORCODE* pError = NULL) = 0;

/* Delete a record. */

	virtual void DeleteRecord(DATAID idRecord);

/* Duplicate a record. */

	virtual CDataRecord* DuplicateRecord(CDataRecord* pRecord, CDataManager* pDupManager = NULL);
	virtual DATAID DuplicateRecord(DATAID dwID, CDataManager* pDupManager = NULL);

/*
// Generate the next id for a book item.
// This seems a little out of place (it's used by CBookItem, so it puts higher
// up knowledge at this level), but the CBookItem needs this utility and I
// can't figure where else to put it.
*/

	virtual ITEMID AllocateItemID(int nCount = 1) = 0;

	virtual ERRORCODE FlushRecord(DATAID Id) = 0;
};

// CDataRecord inlines.

inline CDataRecord::~CDataRecord()
	{}
inline void CDataRecord::SelfDestruct(void)
	{ Manager()->DeleteRecord(this); }
inline void CDataRecord::SelfRelease(void)
	{ Manager()->ReleaseRecord(this); }

// CDataManager inlines.

inline CDataManager::~CDataManager()
	{}

#endif			// __CDATAMAN_H__
