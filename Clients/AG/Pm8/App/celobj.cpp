/*
// $Workfile: celobj.cpp $
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
// $Log: /PM8/App/celobj.cpp $
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

// table cell object
// The purpose of a cell object is to be a container for whatever goes in
// the cell (be it text or graphic or whatever).a

#include "stdafx.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "pmgdb.h"

#include "celobj.h"

/////////////////////////////////////////////////////////////////////////////
// CCellObject

/*
// The creator for a table cell object.
*/

ERRORCODE CCellObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *ppObject)
{
	ERRORCODE error = ERRORCODE_None;

	// Create the cell object.
	CCellObject* pObject = NULL;
	TRY
	{
		pObject = new CCellObject(type, owner);
	}
	END_TRY

	if (pObject == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
		// We created the object.
		// If we had creation data, we would handle it here.
	}

	*ppObject = pObject;
	return error;
}

/*
// The constructor for the table cell object.
*/

CCellObject::CCellObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
				: INHERITED(type, owner)
{
	record.select_flags = 0;
	memset(&m_Record, 0, sizeof(m_Record));
	m_Record.m_nRow = -1;
	m_Record.m_nColumn = -1;
}

/*
// Read the data for a CCellObject
*/

ERRORCODE CCellObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None
		 && (error = device->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
	}
	return error;
}

/*
// Write the data for a CCellObject
*/

ERRORCODE CCellObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

   if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
   {
      error = device->write_record(&m_Record, sizeof(m_Record));
   }
   return error;
}

/*
// Return the size of the data to be written.
*/

ST_MAN_SIZE CCellObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device)
				+ device->size_record(sizeof(m_Record));
}

// Create a state for saving.
// We return a pointer to a new object. This object needs to be freed by
// the caller at some point.
PageObjectState* CCellObject::CreateState(void)
{
	return RectPageObject::CreateState();
}

// Save our state.
void CCellObject::SaveState(PageObjectState* pState)
{
	RectPageObject::SaveState(pState);
}

// Restore our state.
// The state passed in is NOT deleted by us. It is still the responsibility
// of the caller (or whomever).
void CCellObject::RestoreState(PageObjectState* pState)
{
	RectPageObject::RestoreState(pState);
}

/*
// Assign an object to this.
*/

ERRORCODE CCellObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;
	CCellObject& cobject = (CCellObject&)sobject;

	// Assign the base object.
	if ((error = GroupObject::assign(sobject)) == ERRORCODE_None)
	{
		// Copy the record.
		m_Record = cobject.m_Record;
	}
	return error;
}

void CCellObject::calc(PBOX_PTR panel_world /*=NULL*/, FLAGS panel_flags /*=0*/)
{
	// Handle any changes in the flipped state.
	// This is to offset the fact that RestoreState does not do the full
	// group restore.

	BOOL fXFlipped = (get_flags() & OBJECT_FLAG_xflipped);
	BOOL fYFlipped = (get_flags() & OBJECT_FLAG_yflipped);

	PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
	while (pObject != NULL)
	{
		if (fXFlipped != (pObject->get_flags() & OBJECT_FLAG_xflipped))
		{
			pObject->xflip();
		}
		if (fYFlipped != (pObject->get_flags() & OBJECT_FLAG_yflipped))
		{
			pObject->yflip();
		}
		pObject = (PMGPageObject*)pObject->next_object();
	}

	INHERITED::calc();
}
