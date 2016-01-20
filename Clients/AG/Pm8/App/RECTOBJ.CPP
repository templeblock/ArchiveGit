/*
// $Header: /PM8/App/RECTOBJ.CPP 1     3/03/99 6:11p Gbeddow $
//
// Rectangle object routines.
//
// $Log: /PM8/App/RECTOBJ.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:16   Fred
// Initial revision.
// 
//    Rev 1.3   04 Mar 1997 17:03:54   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.2   16 Jan 1997 13:00:36   Jay
// Square original dims so shift-resize works!
// 
//    Rev 1.1   07 Nov 1996 16:25:36   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:46:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:58   FRED
// Initial revision.
// 
//    Rev 1.5   01 Aug 1995 12:59:40   JAY
// Added correct name.
// 
//    Rev 1.4   13 May 1995 11:14:08   JAY
//  
// 
//    Rev 1.3   10 May 1995 13:14:20   JAY
// Now it's really a rectangle.
// 
//    Rev 1.2   09 May 1995 08:05:44   JAY
// Junk.
// 
//    Rev 1.1   05 May 1995 16:49:30   JAY
// Exciting new additions to OutlinePaths!
// 
//    Rev 1.0   04 May 1995 17:15:42   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmgobj.h"

#include "rectobj.h"

#include "outpath.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////
// RectangleObject

/*
// The creator.
// This is a static function which is registered with the database.
*/

ERRORCODE RectangleObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	RectangleObject* object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

	if ((object = new RectangleObject(type, owner)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* The object was created. Handle any creation data. */
	}

	*record = (PageObjectPtr)object;
	return error;
}

/*
// The constructor.
*/

RectangleObject::RectangleObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: DrawingObject(type, owner)
{
	memset(&m_RectRecord, 0, sizeof(m_RectRecord));
}

/*
// The destructor for the Warp object.
*/

RectangleObject::~RectangleObject(void)
{
}

/*
// Get the path for this object.
*/

BOOL RectangleObject::GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject /*=NULL*/)
{
	long lX0 = MakeFixed(crObject.left);
	long lY0 = MakeFixed(crObject.top);
	long lX1 = MakeFixed(crObject.left+crObject.Width());
	long lY1 = MakeFixed(crObject.top+crObject.Height());

	Path.MoveTo(lX0, lY0);
	Path.LineTo(lX1, lY0);
	Path.LineTo(lX1, lY1);
	Path.LineTo(lX0, lY1);
	Path.LineTo(lX0, lY0);
	Path.Close();
	Path.End();

	return TRUE;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE RectangleObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&m_RectRecord, sizeof(m_RectRecord));
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE RectangleObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&m_RectRecord, sizeof(m_RectRecord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE RectangleObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device) + device->size_record(sizeof(m_RectRecord));
}

/*
// Get an object's name.
*/

BOOL RectangleObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
	// JRO strcpy(buffer, "Rectangle");
   strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_RECTANGLE));
	return TRUE;
}

/*
// Assign an object to this.
*/

ERRORCODE RectangleObject::assign(PageObjectRef pobject)
{
	ERRORCODE error;
	RectangleObject& sobject = (RectangleObject&)pobject;

	if ((error = INHERITED::assign(pobject)) == ERRORCODE_None)
	{
		m_RectRecord = sobject.m_RectRecord;
	}
	return error;
}

/*
// Get the original dimensions of this drawing object.
*/

BOOL RectangleObject::original_dims(PPNT_PTR p)
{
	p->x = p->y = PAGE_RESOLUTION;
	return TRUE;
}
	
PageObjectState* RectangleObject::CreateState(void)
{
	return new RectangleObjectState;
}

void RectangleObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);

	((RectangleObjectState*)pState)->m_RectRecord = m_RectRecord;
}

void RectangleObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);

	m_RectRecord = ((RectangleObjectState*)pState)->m_RectRecord;
}

