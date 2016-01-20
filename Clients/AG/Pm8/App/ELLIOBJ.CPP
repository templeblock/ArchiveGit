/*
// $Header: /PM8/App/ELLIOBJ.CPP 1     3/03/99 6:05p Gbeddow $
//
// Ellipse object routines.
//
// $Log: /PM8/App/ELLIOBJ.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:18   Fred
// Initial revision.
// 
//    Rev 1.4   04 Mar 1997 17:03:34   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.3   16 Jan 1997 13:00:34   Jay
// Square original dims so shift-resize works!
// 
//    Rev 1.2   08 Nov 1996 13:17:56   johno
// 
//    Rev 1.1   08 Nov 1996 13:14:04   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:46:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:16   FRED
// Initial revision.
// 
//    Rev 1.3   01 Aug 1995 12:59:38   JAY
// Added correct name.
// 
//    Rev 1.2   13 May 1995 11:15:02   JAY
//  
// 
//    Rev 1.1   10 May 1995 13:13:42   JAY
// Real ellipse drawing code.
// 
//    Rev 1.0   09 May 1995 08:06:52   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmgobj.h"

#include "elliobj.h"

#include "outpath.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////
// EllipseObject

/*
// The creator.
// This is a static function which is registered with the database.
*/

ERRORCODE EllipseObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	EllipseObject* object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

	if ((object = new EllipseObject(type, owner)) == NULL)
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

EllipseObject::EllipseObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: DrawingObject(type, owner)
{
	memset(&m_EllipseRecord, 0, sizeof(m_EllipseRecord));
}

/*
// The destructor for the Warp object.
*/

EllipseObject::~EllipseObject(void)
{
}

/*
// Get the path for this object.
*/

BOOL EllipseObject::GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject /*=NULL*/)
{
	long lX0 = MakeFixed(crObject.left);
	long lY0 = MakeFixed(crObject.top);
	long lX1 = MakeFixed(crObject.left+crObject.Width());
	long lY1 = MakeFixed(crObject.top+crObject.Height());

	CFixedPoint Center;

	Center.x = (lX0 + lX1)/2;
	Center.y = (lY0 + lY1)/2;

	CFixedPoint Radius;

	Radius.x = Center.x - lX0;
	Radius.y = Center.y - lY0;

	CFixedPoint Offset;

	Offset.x = MulDivFixed(Radius.x, MakeFixed(11.0), MakeFixed(20.0));
	Offset.y = MulDivFixed(Radius.y, MakeFixed(11.0), MakeFixed(20.0));

	Path.MoveTo(Center.x, lY0);
	Path.BezierTo(Center.x + Offset.x, lY0,
					  lX1, Center.y - Offset.y,
					  lX1, Center.y);
	Path.BezierTo(lX1, Center.y + Offset.y,
					  Center.x + Offset.x, lY1,
					  Center.x, lY1);
	Path.BezierTo(Center.x - Offset.x, lY1,
					  lX0, Center.y + Offset.y,
					  lX0, Center.y);
	Path.BezierTo(lX0, Center.y - Offset.y,
					  Center.x - Offset.x, lY0,
					  Center.x, lY0);
	Path.Close();
	Path.End();

	return TRUE;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE EllipseObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&m_EllipseRecord, sizeof(m_EllipseRecord));
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE EllipseObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&m_EllipseRecord, sizeof(m_EllipseRecord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE EllipseObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device) + device->size_record(sizeof(m_EllipseRecord));
}

/*
// Get an object's name.
*/

BOOL EllipseObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
	// JRO strcpy(buffer, "Circle");
   strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_CIRCLE));
	return TRUE;
}

/*
// Assign an object to this.
*/

ERRORCODE EllipseObject::assign(PageObjectRef pobject)
{
	ERRORCODE error;
	EllipseObject& sobject = (EllipseObject&)pobject;

	if ((error = INHERITED::assign(pobject)) == ERRORCODE_None)
	{
		m_EllipseRecord = sobject.m_EllipseRecord;
	}
	return error;
}

/*
// Get the original dimensions of this drawing object.
*/

BOOL EllipseObject::original_dims(PPNT_PTR p)
{
	p->x = p->y = PAGE_RESOLUTION;
	return TRUE;
}
	
PageObjectState* EllipseObject::CreateState(void)
{
	return new EllipseObjectState;
}

void EllipseObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);

	((EllipseObjectState*)pState)->m_EllipseRecord = m_EllipseRecord;
}

void EllipseObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);

	m_EllipseRecord = ((EllipseObjectState*)pState)->m_EllipseRecord;
}

