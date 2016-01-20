/*
// Storage manager definitions.
// The storage manager manages the space on a storage device. It allocates,
// deallocates, compacts, and otherwise maintains the integrity of the
// information stored on a storage device.
//
// Oct. 14, 1992 - Jay N. : Initial definitions
//
// $Header: /PM8/App/STMANBK.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/STMANBK.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:24   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:58   JAY
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

#ifndef __STOREMAN_H__
#define __STOREMAN_H__

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

#define	ST_MAN_HDR_SIZE	8			/* 1 + 3 + 4 */

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

typedef class StorageElement far *StorageElementPtr;

/*
// The storage manager class.
*/

FARCLASS StorageManager
{
	friend class StorageElement;
protected:
	BOOL initialized;
	struct ST_MAN_STATE
	{
		ST_DEV_POSITION	storage_start;
		ST_DEV_POSITION	storage_end;
		ST_DEV_POSITION	free_start;
		ST_MAN_SIZE			free_biggest;
	} state;
	RW_Semaphore			m_sem;

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
// The size of a header block.
*/

	SHORT						m_hdr_block_size;

/* Refresh the state. */

	ERRORCODE refresh_state(void);

/* Flush the state. */

	ERRORCODE flush_state(void);

/*
// Read lock the state.
*/

	ERRORCODE read_lock_state(void);

/*
// Read unlock the state.
*/

	ERRORCODE read_unlock_state(void);

/*
// Modify lock the state.
// This locks the entire free chain.
*/

	ERRORCODE modify_lock_state(void);

/*
// Modify unlock the state.
*/

	ERRORCODE modify_unlock_state(void);

/*
// Read a storage block header.
*/

	ERRORCODE	read_header_block(ST_MAN_HDR_BLOCK far *header, ST_DEV_POSITION where = -1L, BOOL do_lock = TRUE);

/*
// Write a storage block header.
*/

	ERRORCODE	write_header_block(ST_MAN_HDR_BLOCK far *header, ST_DEV_POSITION where = -1L);

/*
// Expand a block into an existing free block.
*/

	ERRORCODE	expand_block(ST_MAN_HDR_BLOCK hdr, ST_DEV_POSITION pos,
										ST_MAN_HDR_BLOCK free_hdr, ST_DEV_POSITION free_pos,
										ST_MAN_SIZE expand_amount);

public:

/*
// The device this manager is based on.
// This is public so objects containing a StorageManager don't also need to
// keep a pointer to a StorageDevice.
*/

	StorageDevicePtr const device;

/*
// The constructor for a storage manager.
// Note that the storage manager will not be usable (except where noted)
// until either initialize() or read_information() is called.
*/

	StorageManager(StorageDevicePtr dev, BOOL is_sharing = FALSE);

/*
// initialize()
//
// Initialize the storage manager.
// This is done in lieu of calling read_information().
// The storage manager will be initialized (and usable) after this call.
*/

	ERRORCODE initialize(ST_DEV_POSITION state_offset, ST_DEV_POSITION storage_base);

/*
// reinitialize()
//
// Reinitialize the storage manager.
// This will free all storage in one fell swoop.
*/

	ERRORCODE reinitialize(void);

/*
// size_state()
//
// Return the size of the storage manager state information as it resides
// on the storage device.
// This call works even if the storage manager is not initialized.
*/

	ST_DEV_IO_SIZE size_state();

/*
// read_state()
//
// Read storage manager state information from the storage device.
// This is done in lieu of calling initialize(), and returns the
// storage manager to the state it was in when the information was saved.
// The storage manager will be initialized (and usable) after this call.
*/

	ERRORCODE read_state(BOOL validate = FALSE, BOOL reset_semaphores = TRUE);

/*
// write_state()
//
// Write storage manager state information to the storage device.
// This is done so that the storage manager can be returned to this state
// at a later time.
*/

	ERRORCODE write_state();

/*
// flush()
//
//
// Write anything which needs to be written to the StorageDevice.
*/

	ERRORCODE flush();

/*
// allocate_storage()
//
// Allocate some storage in the storage device.
// Requesting storage which is too large or which has negative size will result
// in an error.
*/

	ERRORCODE allocate_storage(ST_DEV_POSITION far *position, ST_MAN_SIZE size, ST_MAN_TYPE type, ST_MAN_ID id);

/*
// free_storage()
//
// Free some storage in the storage device.
*/

	ERRORCODE free_storage(ST_DEV_POSITION position);

/*
// resize_storage()
//
// Resize some storage in the storage device.
// The position may change if necessary to satisfy the request
// (which is why we pass it via a pointer).
// NOTE: If a NULL element is passed, no attempt is made to preserve the
// contents of the block. If a non-NULL element is passed, the element's
// relocate() member function is called before the old block is freed.
*/

	ERRORCODE resize_storage(ST_DEV_POSITION far *position, ST_MAN_SIZE new_size, StorageElementPtr element = NULL);

/*
// Dump the free list.
*/

	VOID dump_free_list(LPSTR s);

/*
// Force a new device to be used.
// This routine will fail if the storage manager has been initialized.
*/

	ERRORCODE force_new_device(StorageDevicePtr new_device);

/*
// Set a size limit on this storage manager.
// The storage area will not grow if the new size would exceed this amount.
*/

	void set_size_limit(ST_DEV_POSITION new_size = POSITION_MAX);

/*
// Get the current size limit for this storage manager.
*/

	ST_DEV_POSITION get_size_limit(void) const;

/*
// Is this in sharing mode?
*/

	BOOL is_sharing(void) const;

/*
// Set the sharing flag.
// This should only be done before the state has been read (i.e. before
// this guy gets initialized).
*/

	ERRORCODE set_sharing(BOOL is_sharing);

/*
// What is the header block size?
*/

	SHORT hdr_block_size(void) const;

/*
// rebuild_free_list()
//
// Rebuild the free list.
*/

	ERRORCODE rebuild_free_list(void);

/*
// integrity_check()
//
// Perform an integrity check on the storage area. This will also reset
// the read locks on all storage element semaphores if requested.
*/

	ERRORCODE integrity_check(BOOL reset_semaphores);
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

FARCLASS StorageElement
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

	StorageElement(ST_MAN_TYPE type, ST_MAN_ID id, StorageManagerPtr stman);

/*
// The destructor for a storage element.
//
// The destructor is virtual so that it can be overloaded by derived classes.
*/

	virtual ~StorageElement() { }

/*
// allocate()
//
// Allocate storage for this element.
// This is done by write() automatically, so this should only be needed in
// special circumstances where allocation prior to write time is necessary.
*/

	ERRORCODE allocate(ST_MAN_TYPE type, ST_MAN_ID id, ST_MAN_SIZE size = ST_MAN_NO_SIZE);

/*
// deallocate()
//
// Deallocate storage for this element.
// This is usually done in preparation for an element being deleted, though
// there may be other needs for this.
// If deallocation is successful, *where is set to POSITION_UNALLOCATED.
*/

	ERRORCODE deallocate(void);

/*
// read()
//
//	Read this storage element into memory.
//
// Basic header checking will take place when the header is read unless
// the element is created with type ST_TYPE_ANY (and the appropriate error
// will be returned if the headers don't match excluding size).
*/

	ERRORCODE read(ST_DEV_POSITION where = POSITION_UNKNOWN);

/*
// write()
//
//	Write this storage element from memory.
//
// This routine takes a pointer to a position so that it can update it if
// the position needs to be changed (e.g. if it gets larger).
// If the element has never been written before, space will be allocated in
// the storage device for it. If the element changes size, a realloc will
// be called on it which may move its position within the storage device.
*/

	ERRORCODE write(ST_DEV_POSITION far *where = NULL);

/*
// read_lock()
//
// Lock the header in preparation for (potentially) extensive reading.
*/

	ERRORCODE read_lock(BOOL is_reading = FALSE);

/*
// read_unlock()
//
// Unlock the header. This is the pair for read_lock().
*/

	ERRORCODE read_unlock(void);

/*
// modify_lock()
//
// Lock the header in preparation for modification.
*/

	ERRORCODE modify_lock(void);

/*
// modify_unlock()
//
// Unlock the header. This is the pair for modify_lock().
*/

	ERRORCODE modify_unlock(void);

/*
// drop_all_locks()
//
// Drop all locks on this storage element.
*/

	ERRORCODE drop_all_locks(void);

#if 0
/*
// refresh()
//
// Refresh this object from the file.
// This is only important for sharing cases.
*/

	virtual ERRORCODE refresh(ST_DEV_POSITION where, BOOL do_lock = TRUE);
#endif

/*
// type()
//
// Return the type of this storage element.
*/

	ST_MAN_TYPE type() const { return hdr.type; }

/*
// id()
//
// Return the id for this storage element.
*/

	ST_MAN_ID id() const { return hdr.id; }

/*
// position()
//
// Return the current storage position for this storage element.
// If the element has never been written, its position is POSITION_UNALLOCATED.
*/

	ST_DEV_POSITION position(void) const { return m_position; }

/*
// element_modified()
//
// Say we are now modified.
*/

	void element_modified(void) { m_modified = TRUE; }

/*
// is_element_modified()
//
// Are we modified?
*/

	BOOL is_element_modified(void) const { return m_modified; }

/*
// read_data()
//
// This is a method defined by a class derived from this one.
// It handles the reading of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE read_data(StorageDevicePtr device) { return ERRORCODE_None; }

/*
// write_data()
//
// This is a method defined by a class derived from this one.
// It handles the writing of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
*/

	virtual	ERRORCODE write_data(StorageDevicePtr device) { return ERRORCODE_None; }

/*
// sizeof_data()
//
// This is a method defined by a class derived from this one.
// It returns the size of any data which is specific to the derived class.
// This routine should not be called by anyone but the StorageManager/
// StorageElement routines.
//
// A pointer to the device is passed to allow access to the device's
// sizing routines.
*/

	virtual	ST_MAN_SIZE sizeof_data(StorageDevicePtr device) { return 0; }

/*
// relocate_data()
//
// This is a method which may be defined by derived classes.
// This function is called whenever an element is moved by the storage
// manager (via resize_storage()) prior to writing.
*/

	virtual ERRORCODE relocate_data(StorageDevicePtr device, ST_DEV_POSITION old_position, ST_DEV_POSITION new_position)
			{ return ERRORCODE_None; }

};

/*
// Inline functions.
*/

inline void StorageManager::set_size_limit(ST_DEV_POSITION new_size)
	{ m_size_limit = new_size; }
inline ST_DEV_POSITION StorageManager::get_size_limit(void) const
	{ return m_size_limit; }
inline BOOL StorageManager::is_sharing() const
	{ return m_is_sharing; }
inline SHORT StorageManager::hdr_block_size(void) const
	{ return m_hdr_block_size; }


#endif							/* #ifndef __STOREMAN_H__ */
