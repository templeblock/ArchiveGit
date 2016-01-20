/*
// Storage manager definitions.
// The storage manager manages the space on a storage device. It allocates,
// deallocates, compacts, and otherwise maintains the integrity of the
// information stored on a storage device.
//
// Oct. 14, 1992 - Jay N. : Initial definitions
//
// $Header: /PM8/REMIND32/STOREMAN.H 1     3/03/99 6:27p Gbeddow $
//
// $Log: /PM8/REMIND32/STOREMAN.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:25:00   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:32   Fred
// Initial revision.
// 
//    Rev 1.3   28 Jul 1996 10:17:08   Jay
// Lock count can be non-zero during Reinitialize()!
// 
//    Rev 1.2   17 Jul 1996 14:10:08   Jay
// Database bookkeeping
// 
//    Rev 1.1   17 Jul 1996 12:53:30   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.0   14 Mar 1996 13:43:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:24   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:49:00   JAY
// Initial revision.
// 
//    Rev 1.7   30 Nov 1994 16:38:56   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.0   03 Nov 1994 12:56:58   FRED
//  
// 
//    Rev 1.1   01 Nov 1994 09:59:36   JAY
// New data manager routines
// 
//    Rev 1.0   25 Oct 1994 16:24:58   JAY
// Initial revision.
// 
//    Rev 1.6   13 Jul 1994 11:59:26   JAY
// New access and freeing methods to enable document database utility apps.
// 
//    Rev 1.5   07 Jul 1994 11:23:46   JAY
// Added set_sharing().
// 
//    Rev 1.4   20 Jun 1994 13:23:50   JAY
// Added reinitialize() to StorageManager.
// Added drop_all_locks() to StorageElement.
// 
//    Rev 1.3   09 Jun 1994 11:06:28   JAY
// Support for "shareable" databases. File locking, etc.
// 
//    Rev 1.2   01 Jun 1994 16:42:02   JAY
// Added size limit variable.
// 
//    Rev 1.1   08 Feb 1994 09:34:32   JAY
// Changed "storage.h" to "dbstore.h".
// Added PVCS headers.
*/

class StorageManager;
class StorageElement;

#ifndef __STOREMAN_H__
#define __STOREMAN_H__

#ifdef _DEBUG
#define STOREMAN_STATISTICS
#endif

#include "dbstore.h"				/* Make sure we have storage definitions. */
#include "rwsem.h"

/*
// A storage size.
// It's an error to pass a number larger than ST_MAN_MAX_SIZE or negative.
*/

typedef LONG	ST_MAN_SIZE;
#define	ST_MAN_MAX_SIZE	0x007FFFFFL		/* 24-bit number used */
#define	ST_MAN_NO_SIZE		-1L				/* No size set yet. */

/*
// A storage type.
// It's considered an error to pass a type of 0 to any storage manager routine.
*/

typedef SHORT ST_MAN_TYPE;			/* type is written as one byte, though. */
#define	ST_TYPE_ANY			((ST_MAN_TYPE)-1)
#define	ST_TYPE_ILLEGAL	((ST_MAN_TYPE)0)
#define	ST_TYPE_FREE		((ST_MAN_TYPE)1)

/*
// A storage ID.
// An ID is an arbitrary number, so feel free to use what you want.
*/

typedef LONG ST_MAN_ID;

/*
// A little header placed in the front of each piece of storage allocated.
*/

struct ST_MAN_HDR_BLOCK
{
	ST_MAN_TYPE		type;					/* type of block */
	ST_MAN_SIZE		size;					/* size of block */
	ST_MAN_ID		id;					/* id of block */
};

#define	ST_MAN_HDR_SIZE	8			/* 1 + 3 + 4  = Old PMW style */
//#define	ST_MAN_HDR_SIZE	sizeof(ST_MAN_HDR_BLOCK)

/*
// Standard types of storage we will be using.
// You should probably keep them all here even though they are details of
// higher levels.
//
// There is no element type with value of 0. This is too common a value in
// the event that the storage arena gets trashed. Hopefully, this will make
// it less likely that data will get overwritten.
*/

#define	ELEMENT_TYPE_FREE		1			// Same as ST_TYPE_FREE
#define	ELEMENT_TYPE_RECORD	2
#define	ELEMENT_TYPE_TABLE	3
#define	ELEMENT_TYPE_LOCATOR	4

/*
// A pointer to a storage element.
*/

typedef StorageElement far *StorageElementPtr;

struct ST_MAN_STATE
{
	ST_DEV_POSITION	storage_start;
	ST_DEV_POSITION	storage_end;
	ST_DEV_POSITION	free_start;
	ST_MAN_SIZE			free_biggest;
};

struct ST_MAN_SHARED_STATE : public ST_MAN_STATE
{
	long					m_lUsers;
};

/*
// The storage manager class.
*/

class StorageManager
{
	friend StorageElement;
protected:
	BOOL initialized;
	ST_MAN_SHARED_STATE state;
	RW_Semaphore			m_sem;

#ifdef STOREMAN_STATISTICS
	long m_lAllocate;
	long m_lFree;
	long m_lResize;
	long m_lElementRead;
	long m_lElementWrite;
	long m_lElementReadLock[5];
	long m_lElementReadUnlock[5];
	long m_lElementModifyLock[5];
	long m_lElementModifyUnlock[5];
#endif

	long m_lTotalLocks;

/* Positions of state and state semaphore things. */
	ST_DEV_POSITION		m_state_position;
	ST_DEV_POSITION		m_sem_position;

/*
// Largest size the storage can attain.
*/

	ST_DEV_POSITION		m_size_limit;

/*
// Is the storage manager in "sharing" mode?
*/

	BOOL						m_is_sharing;

/*
// Is the storage manager in "exclusive" mode?
*/

	BOOL						m_fExclusive;

	// Are we attached?
	BOOL						m_fAttached;
/*
// The size of a header block.
*/

	SHORT						m_nHdrBlockSize;

/* Refresh the state. */

	ERRORCODE RefreshState(void);

/* Flush the state. */

	ERRORCODE FlushState(void);

/*
// Read lock the state.
*/

	ERRORCODE ReadLockState(void);

/*
// Read unlock the state.
*/

	ERRORCODE ReadUnlockState(void);

/*
// Modify lock the state.
// This locks the entire free chain.
*/

	ERRORCODE ModifyLockState(void);

/*
// Modify unlock the state.
*/

	ERRORCODE ModifyUnlockState(void);

/*
// Read a storage block header.
*/

	ERRORCODE	ReadHeaderBlock(ST_MAN_HDR_BLOCK far *header, ST_DEV_POSITION where = -1L, BOOL do_lock = TRUE);

/*
// Write a storage block header.
*/

	ERRORCODE	WriteHeaderBlock(ST_MAN_HDR_BLOCK far *header, ST_DEV_POSITION where = -1L);

/*
// Expand a block into an existing free block.
*/

	ERRORCODE	ExpandBlock(ST_MAN_HDR_BLOCK hdr, ST_DEV_POSITION pos,
										ST_MAN_HDR_BLOCK free_hdr, ST_DEV_POSITION free_pos,
										ST_MAN_SIZE expand_amount);


/*
// Return the size of the storage manager state record.
// This is a simple internal routine used to replace.
*/

	ST_DEV_IO_SIZE SizeStateRecord()
		{ return IsSharing() ? sizeof(ST_MAN_SHARED_STATE) : sizeof(ST_MAN_STATE); }

public:

/*
// The device this manager is based on.
// This is public so objects containing a StorageManager don't also need to
// keep a pointer to a StorageDevice.
*/

	StorageDevicePtr const m_pDevice;

/*
// The constructor for a storage manager.
// Note that the storage manager will not be usable (except where noted)
// until either initialize() or read_information() is called.
*/

	StorageManager(StorageDevicePtr dev, BOOL is_sharing = FALSE);

	virtual ~StorageManager();

/*
// Initialize()
//
// Initialize the storage manager.
// This is done in lieu of calling read_information().
// The storage manager will be initialized (and usable) after this call.
*/

	ERRORCODE Initialize(ST_DEV_POSITION state_offset, ST_DEV_POSITION storage_base);

/*
// Reinitialize()
//
// Reinitialize the storage manager.
// This will free all storage in one fell swoop.
*/

	ERRORCODE Reinitialize(long lLocks = 0);

/*
// SizeState()
//
// Return the size of the storage manager state information as it resides
// on the storage device.
// This call works even if the storage manager is not initialized.
*/

	ST_DEV_IO_SIZE SizeState();

/*
// ReadState()
//
// Read storage manager state information from the storage device.
// This is done in lieu of calling initialize(), and returns the
// storage manager to the state it was in when the information was saved.
// The storage manager will be initialized (and usable) after this call.
*/

	ERRORCODE ReadState(BOOL validate = FALSE, BOOL reset_semaphores = TRUE);

/*
// WriteState()
//
// Write storage manager state information to the storage device.
// This is done so that the storage manager can be returned to this state
// at a later time.
*/

	ERRORCODE WriteState();

/*
// Flush()
//
//
// Write anything which needs to be written to the StorageDevice.
*/

	ERRORCODE Flush();

/*
// AllocateStorage()
//
// Allocate some storage in the storage device.
// Requesting storage which is too large or which has negative size will result
// in an error.
*/

	ERRORCODE AllocateStorage(ST_DEV_POSITION far *position, ST_MAN_SIZE size, ST_MAN_TYPE type, ST_MAN_ID id);

/*
// FreeStorage()
//
// Free some storage in the storage device.
*/

	ERRORCODE FreeStorage(ST_DEV_POSITION position);

/*
// ResizeStorage()
//
// Resize some storage in the storage device.
// The position may change if necessary to satisfy the request
// (which is why we pass it via a pointer).
// NOTE: If a NULL element is passed, no attempt is made to preserve the
// contents of the block. If a non-NULL element is passed, the element's
// relocate() member function is called before the old block is freed.
*/

	ERRORCODE ResizeStorage(ST_DEV_POSITION far *position, ST_MAN_SIZE new_size, StorageElementPtr element = NULL);

/*
// Dump the free list.
*/

	VOID DumpFreeList(LPSTR s);

/*
// Force a new device to be used.
// This routine will fail if the storage manager has been initialized.
*/

	ERRORCODE ForceNewDevice(StorageDevicePtr new_device);

/*
// Set a size limit on this storage manager.
// The storage area will not grow if the new size would exceed this amount.
*/

	void SetSizeLimit(ST_DEV_POSITION new_size = POSITION_MAX);

/*
// Get the current size limit for this storage manager.
*/

	ST_DEV_POSITION GetSizeLimit(void) const;

/*
// Is this in sharing mode?
*/

	BOOL IsSharing(void) const;

/*
// Set the sharing flag.
// This should only be done before the state has been read (i.e. before
// this guy gets initialized).
*/

	ERRORCODE SetSharing(BOOL is_sharing);

/*
// Is this in exclusive mode?
*/

	BOOL IsExclusive(void) const;

	ERRORCODE SetExclusive(BOOL fExclusive);

	// User count management.

	ERRORCODE Attach(void);
	ERRORCODE Detach(void);

	// Lock count management.
	void AddedLock(void);
	void RemovedLock(int nCount = 1);

/*
// What is the header block size?
*/

	SHORT HdrBlockSize(void) const;

/*
// RebuildFreeList()
//
// Rebuild the free list.
*/

	ERRORCODE RebuildFreeList(void);

/*
// IntegrityCheck()
//
// Perform an integrity check on the storage area. This will also reset
// the read locks on all storage element semaphores if requested.
*/

	ERRORCODE IntegrityCheck(BOOL reset_semaphores);
};

/*
// A pointer to a storage manager object.
*/

typedef StorageManager far *StorageManagerPtr;

/*
// The storage element class.
//
// These objects use the storage manager to maintain themselves.
// If you want to allocate a piece of storage, base your class (you ARE
// using classes, right?) on this class.
*/

class StorageElement
{
protected:

	ST_MAN_HDR_BLOCK	hdr;				/* in the file */
	RW_Semaphore		m_sem;

	StorageManagerPtr manager;			/* who owns us? */
	ST_DEV_POSITION	m_position;		/* Last read from or written to. */
	BOOL					m_modified;

public:

/*
// The constructor for a storage element.
*/

	StorageElement(void);
	virtual void Init(ST_MAN_TYPE type, ST_MAN_ID id, StorageManagerPtr stman);

/*
// The destructor for a storage element.
//
// The destructor is virtual so that it can be overloaded by derived classes.
*/

	virtual ~StorageElement() { }

/*
// Allocate()
//
// Allocate storage for this element.
// This is done by write() automatically, so this should only be needed in
// special circumstances where allocation prior to write time is necessary.
*/

	ERRORCODE Allocate(ST_MAN_TYPE type, ST_MAN_ID id, ST_MAN_SIZE size = ST_MAN_NO_SIZE);

/*
// Deallocate()
//
// Deallocate storage for this element.
// This is usually done in preparation for an element being deleted, though
// there may be other needs for this.
// If deallocation is successful, *where is set to POSITION_UNALLOCATED.
*/

	ERRORCODE Deallocate(void);

/*
// Read()
//
//	Read this storage element into memory.
//
// Basic header checking will take place when the header is read unless
// the element is created with type ST_TYPE_ANY (and the appropriate error
// will be returned if the headers don't match excluding size).
*/

	ERRORCODE Read(ST_DEV_POSITION where = POSITION_UNKNOWN);

/*
// Write()
//
//	Write this storage element from memory.
//
// This routine takes a pointer to a position so that it can update it if
// the position needs to be changed (e.g. if it gets larger).
// If the element has never been written before, space will be allocated in
// the storage device for it. If the element changes size, a realloc will
// be called on it which may move its position within the storage device.
*/

	ERRORCODE Write(ST_DEV_POSITION far *where = NULL);

/*
// ReadLock()
//
// Lock the header in preparation for (potentially) extensive reading.
*/

	ERRORCODE ReadLock(BOOL is_reading = FALSE);

/*
// ReadUnlock()
//
// Unlock the header. This is the pair for read_lock().
*/

	ERRORCODE ReadUnlock(void);

/*
// ModifyLock()
//
// Lock the header in preparation for modification.
*/

	ERRORCODE ModifyLock(void);

/*
// ModifyUnlock()
//
// Unlock the header. This is the pair for modify_lock().
*/

	ERRORCODE ModifyUnlock(void);

/*
// DropAllLocks()
//
// Drop all locks on this storage element.
*/

	ERRORCODE DropAllLocks(void);

#if 0
/*
// Refresh()
//
// Refresh this object from the file.
// This is only important for sharing cases.
*/

	virtual ERRORCODE Refresh(ST_DEV_POSITION where, BOOL do_lock = TRUE);
#endif

/*
// Type()
//
// Return the type of this storage element.
*/

	ST_MAN_TYPE Type(void) const { return hdr.type; }

/*
// Id()
//
// Return the id for this storage element.
*/

	ST_MAN_ID Id() const { return hdr.id; }

/*
// Position()
//
// Return the current storage position for this storage element.
// If the element has never been written, its position is POSITION_UNALLOCATED.
*/

	ST_DEV_POSITION Position(void) const { return m_position; }

/*
// ElementModified()
//
// Say we are now modified.
*/

	void ElementModified(void) { m_modified = TRUE; }

/*
// IsElementModified()
//
// Are we modified?
*/

	BOOL IsElementModified(void) const { return m_modified; }

/*
// ReadData()
//
// This is a method defined by a class derived from this one.
// It handles the reading of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE ReadData(StorageDevicePtr device) { return ERRORCODE_None; }

/*
// WriteData()
//
// This is a method defined by a class derived from this one.
// It handles the writing of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE WriteData(StorageDevicePtr device) { return ERRORCODE_None; }

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

	virtual	ST_MAN_SIZE SizeofData(StorageDevicePtr device) { return 0; }

/*
// RelocateData()
//
// This is a method which may be defined by derived classes.
// This function is called whenever an element is moved by the storage
// manager (via resize_storage()) prior to writing.
*/

	virtual ERRORCODE RelocateData(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
			{ return ERRORCODE_None; }
};

/*
// Inline functions.
*/

inline void StorageManager::SetSizeLimit(ST_DEV_POSITION new_size)
	{ m_size_limit = new_size; }
inline ST_DEV_POSITION StorageManager::GetSizeLimit(void) const
	{ return m_size_limit; }
inline BOOL StorageManager::IsSharing() const
	{ return m_is_sharing; }
inline BOOL StorageManager::IsExclusive() const
	{ return m_fExclusive; }
inline SHORT StorageManager::HdrBlockSize(void) const
	{ return m_nHdrBlockSize; }

#endif							/* #ifndef __STOREMAN_H__ */
