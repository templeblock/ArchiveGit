/*
// $Workfile: celobj.h $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/celobj.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 13    7/18/98 6:24p Jayn
// Table fixes.
// 
// 12    6/02/98 3:00p Jayn
// 
// 11    5/29/98 6:38p Jayn
// Insert column, etc.
*/

#ifndef __CELOBJ_H__
#define __CELOBJ_H__

#include "grpobj.h"

class CCellObject : public GroupObject
{
	INHERIT(CCellObject, GroupObject);

private:
	CCellObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

	// cell information to be stored in the database record
	struct
	{
		// What cell are we?
		short	m_nRow;			// row this cell is in 
		short	m_nColumn;		// column this cell is in
	} m_Record;

public:

	// functions for keeping track of where cell is in table
	short GetRow(void) const
		{ return m_Record.m_nRow; }
	void SetRow(short nRow)
		{ m_Record.m_nRow = nRow; }
	short GetColumn(void) const
		{ return m_Record.m_nColumn; }
	void SetColumn(short nColumn)
		{ m_Record.m_nColumn = nColumn; }
	CPoint GetCell(void) const
		{ return CPoint(GetColumn(), GetRow()); }
	void SetCell(const CPoint& cpCell)
		{ SetColumn((short)cpCell.x); SetRow((short)cpCell.y); }

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

	// Read the data for this cell
	virtual ERRORCODE read_data(StorageDevicePtr device);

	// Write the data for this cell
	virtual ERRORCODE write_data(StorageDevicePtr device);

	// Size the data for this cell
	virtual ST_MAN_SIZE size_data(StorageDevicePtr device);

	// Create a state for saving.
	// We return a pointer to a new object. This object needs to be freed by
	// the caller at some point.
	virtual PageObjectState* CreateState(void);

	// Save our state.
	virtual void SaveState(PageObjectState* pState);

	// Restore our state.
	// The state passed in is NOT deleted by us. It is still the responsibility
	// of the caller (or whomever).
	virtual void RestoreState(PageObjectState* pState);

	// Assign an object to this.
	virtual ERRORCODE assign(PageObjectRef sobject);

	// Recalc after a size or movement.
	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);
};

#endif						/* #ifndef __DATEOBJ_H__ */
