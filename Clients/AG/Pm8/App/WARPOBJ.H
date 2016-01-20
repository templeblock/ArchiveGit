/*
// $Header: /PM8/App/WARPOBJ.H 1     3/03/99 6:13p Gbeddow $
//
// $Log: /PM8/App/WARPOBJ.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:32   Fred
// Initial revision.
// 
//    Rev 1.2   28 Jun 1996 16:18:40   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 14:22:28   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:46:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:56   FRED
// Initial revision.
// 
//    Rev 2.12   14 Sep 1995 09:32:20   JAY
// 32-bit changes
// 
//    Rev 2.11   09 Aug 1995 13:09:46   JAY
// Virtual override of MacrosInUse
// 
//    Rev 2.10   17 May 1995 14:25:22   FRED
// Warp text objec refresh bounds.
// 
// 
//    Rev 2.9   13 May 1995 11:14:34   JAY
//  
// 
//    Rev 2.8   10 May 1995 12:34:04   FRED
// Shadows on warp text objects.
// 
// 
//    Rev 2.7   03 May 1995 10:39:40   FRED
//  
// 
//    Rev 2.6   02 May 1995 15:36:32   FRED
// 
//    Rev 2.5   28 Apr 1995 13:49:20   FRED
//  
// 
//    Rev 2.4   27 Apr 1995 13:20:12   FRED
// 
//    Rev 2.3   25 Apr 1995 16:02:28   FRED
// No change.
// 
//    Rev 2.2   25 Apr 1995 10:34:48   FRED
// 
//    Rev 2.1   20 Apr 1995 16:15:58   FRED
// Warp Text Stuff
// 
//    Rev 2.0   19 Apr 1995 14:46:16   FRED
// Initial revision.
*/

class CWarpTextObject;
class CWarpTextObjectState;

#ifndef __WARPOBJ_H__
#define __WARPOBJ_H__

class CWarpField;

#include "frameobj.h"

struct WARPTEXTOBJECT_CREATE_STRUCT : public FRAMEOBJ_CREATE_STRUCT
{
};

struct WarpTextObjectRecord
{
	DB_RECORD_NUMBER m_WarpField;       	/* inode of warp field */
	
	COLOR m_clTextFillColor;
	SHORT m_fTextFillColorExists;				// BOOL
	
	SHORT m_nTextLineWidth;						// int
	
	COLOR m_clTextLineColor;
	SHORT m_fTextLineColorExists;				// BOOL
	
	COLOR m_clTextShadowColor;
	SHORT m_fTextShadowColorExists;			// BOOL
	
	COLOR m_clBackgroundFillColor;
	SHORT m_fBackgroundFillColorExists;		// BOOL
	
	SHORT m_nBackgroundLineWidth;				// int
	
	COLOR m_clBackgroundLineColor;
	SHORT m_fBackgroundLineColorExists;		// BOOL
	
	COLOR m_clBackgroundShadowColor;
	SHORT m_fBackgroundShadowColorExists;	// BOOL
};

class WarpTextObjectState : public CFrameObjectState
{
public:
	WarpTextObjectState();
	virtual ~WarpTextObjectState();

	WarpTextObjectRecord	m_WRecord;
};

class WarpTextObject : public CFrameObject
{
	INHERIT(WarpTextObject, CFrameObject)
private:
	WarpTextObjectRecord m_WRecord;

/*
// Private constructor.
// Call the create method.
*/

	WarpTextObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

	~WarpTextObject(void);

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Update an object into a redisplay context.
*/

	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// Return the refresh bounds for this object.
*/

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Toggle select marks for an object.
*/

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which);

/*
// Set the editing flag.
*/

	void SetEditingObject(CFrameObject* pEditingObject);

/*
// Set a new warp field, freeing an old one.
*/

	VOID set_new_warp_field(DB_RECORD_NUMBER l);
	
/*
// Get a pointer to the current warp field object.
*/

	CWarpField* GetWarpField(void);

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

	ERRORCODE read_data(StorageDevicePtr device);

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

	ERRORCODE write_data(StorageDevicePtr device);

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

	ST_MAN_SIZE size_data(StorageDevicePtr device);

/*
// Assign an object to this.
*/

	virtual ERRORCODE assign(PageObjectRef sobject);

/*
// Create a state for saving.
// We return a pointer to a new object. This object needs to be freed by
// the caller at some point.
*/

	virtual PageObjectState* CreateState(void);

/*
// Save our state.
*/

	virtual void SaveState(PageObjectState* pState);

/*
// Restore our state.
// The state passed in is NOT deleted by us. It is still the responsibility
// of the caller (or whomever).
*/

	virtual void RestoreState(PageObjectState* pState);
	
/*
// Return a pointer to the warp text record.
*/

	WarpTextObjectRecord& Record(void)
		{ return m_WRecord; }

/*
// Figure out the X and Y shadow offsets for the background.
*/
	
	virtual VOID GetBackgroundShadowOffsets(PCOORD* pXOffset, PCOORD* pYOffset);
	
/*
// Figure out the X and Y shadow offsets for the text.
*/
	
	virtual VOID GetTextShadowOffsets(int nLines, PCOORD* pXOffset, PCOORD* pYOffset);
	
protected:

	// Pointer to warp field object. Allocated by GetWarpField().
	CWarpField* m_pWarpField;
	CFrameObject* m_pEditingObject;
};

#endif					// #ifndef __WARPOBJ_H__
