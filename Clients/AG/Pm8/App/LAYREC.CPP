/*
// $Header: /PM8/App/LAYREC.CPP 1     3/03/99 6:06p Gbeddow $
//
// Object layout record routines.
//
// $Log: /PM8/App/LAYREC.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:22:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:06   FRED
// Initial revision.
// 
//    Rev 2.1   19 Apr 1995 12:31:44   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:47:54   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:35:54   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   09 Jun 1994 11:10:10   JAY
// Now emptys element list before reading (in case it's being refreshed).
// 
//    Rev 1.2   01 Jun 1994 16:35:24   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:05:46   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:00   JAY
// 
*/

#include "stdafx.h"
#include "pmgobj.h"
#include "layrec.h"
#include "utils.h"

/*****************************************************************************/
/*                              LayoutElement                                */
/*****************************************************************************/

/*
// Read a layout element.
*/

ERRORCODE LayoutElement::read(StorageDevicePtr device)
{
	return device->read(&record, sizeof(record));
}

/*
// Write a layout element.
*/

ERRORCODE LayoutElement::write(StorageDevicePtr device)
{
	return device->write(&record, sizeof(record));
}

/*
// Return the size of a layout element.
*/

ST_DEV_IO_SIZE LayoutElement::size(StorageDevicePtr device)
{
	return sizeof(LErecord);
}

/*
// Generate real rectangles from a layout element.
// "world" defines the coordinate system
// "size" gives the size of the object which will be fit in the element
// "func" is the function to pass rectangles to
// "user" is user supplied data which is passed to the rect function
*/

VOID LayoutElement::generate_rects(PBOX world, POINT size, FLAGS obj_flags,
									ELEM_RECT_FUNC func, VOIDPTR user)
{
	PBOX r;
	PPNT element_center, set_size;
 	PPNT world_dims;
	PCOORD calc_y;
	FLAGS flags;
	PBOX scaled_elem;

//	printf("world: %ld, %ld, %ld, %ld...", world);
/* Calculate world dims. */

	world_dims.x = world.x1 - world.x0;
	world_dims.y = world.y1 - world.y0;

/* Extract the flags from the structure for speed. */

	flags = record.flags;

/* Generate the scaled element rectangle. */

	scaled_elem.x0 = scale_pcoord(record.where.left, world_dims.x, ELEMENT_X_UNIT) + world.x0;
	scaled_elem.y0 = scale_pcoord(record.where.top, world_dims.y, ELEMENT_Y_UNIT) + world.y0;

	scaled_elem.x1 = scale_pcoord(record.where.right, world_dims.x, ELEMENT_X_UNIT) + world.x0;
	scaled_elem.y1 = scale_pcoord(record.where.bottom, world_dims.y, ELEMENT_Y_UNIT) + world.y0;

/* Generate the size. */

	if (flags & ELEMENT_RELATIVE_SIZE)
	{
	/* Compute the size based on the actual size of the object. */
		set_size.x = scale_pcoord(size.x, record.where.right, 100L);
		set_size.y = scale_pcoord(size.y, record.where.bottom, 100L);
	}
	else
	{
		if (flags & (ELEMENT_FLOW_LEFT | ELEMENT_FLOW_RIGHT))
		{
		/* Special fixed size graphic. */
#if 0
			if (CurrentDocument->orientation == LANDSCAPE)
#endif
			{
			/* Width becomes height. */
				set_size.x = set_size.y = scaled_elem.y1 - scaled_elem.y0;
				if (flags & ELEMENT_FLOW_RIGHT)
				{
					scaled_elem.x1 = scaled_elem.x0 + set_size.x;
				}
				else
				{
					scaled_elem.x0 = scaled_elem.x1 - set_size.x;
				}
			}
#if 0
			else
			{
			/* Height becomes width. */
				set_size.x = set_size.y = scaled_elem.x1 - scaled_elem.x0;
				if (flags & ELEMENT_FLOW_RIGHT)
				{
					scaled_elem.y1 = scaled_elem.y0 + set_size.y;
				}
				else
				{
					scaled_elem.y0 = scaled_elem.y1 - set_size.y;
				}
			}
#endif
		}
		else
		{
		/* Compute the dimension of the element rectangle. */
			set_size.x = scaled_elem.x1 - scaled_elem.x0;
			set_size.y = scaled_elem.y1 - scaled_elem.y0;
		}

		if (size.x != 0 && size.y != 0)
		{
			if ((calc_y = scale_pcoord(size.y, set_size.x, size.x)) > set_size.y)
			{
				set_size.x = scale_pcoord(size.x, set_size.y, size.y);
			}
			else
			{
				set_size.y = calc_y;
			}
		}
	}

	element_center.x = (scaled_elem.x0+scaled_elem.x1)/2;
	element_center.y = (scaled_elem.y0+scaled_elem.y1)/2;

/* Generate the rectangle. */

	if (flags & ELEMENT_CENTERED_X)
	{
		r.x1 = (r.x0 = (world.x0 + world.x1 - set_size.x)/2) + set_size.x;
	}
	else if (flags & ELEMENT_ALIGN_LEFT)
	{
		r.x0 = scaled_elem.x0;
		if (flags & ELEMENT_ALIGN_RIGHT)
		{
			r.x1 = scaled_elem.x1;
		}
		else
		{
			r.x1 = r.x0 + set_size.x;
		}
	}
	else if (flags & ELEMENT_ALIGN_RIGHT)
	{
		r.x0 = (r.x1 = scaled_elem.x1) - set_size.x;
	}
	else
	{
	/* Defaults to center alignment. */
		r.x1 = (r.x0 = element_center.x - set_size.x/2) + set_size.x;
	}

	if (flags & ELEMENT_CENTERED_Y)
	{
		r.y1 = (r.y0 = (world.y0 + world.y1 - set_size.y)/2) + set_size.y;
	}
	else if (flags & ELEMENT_ALIGN_TOP)
	{
		r.y0 = scaled_elem.y0;
		if (flags & ELEMENT_ALIGN_BOTTOM)
		{
			r.y1 = scaled_elem.y1;
		}
		else
		{
			r.y1 = r.y0 + set_size.y;
		}
	}
	else if (flags & ELEMENT_ALIGN_BOTTOM)
	{
		r.y0 = (r.y1 = scaled_elem.y1) - set_size.y;
	}
	else
	{
	/* Defaults to center alignment. */
		r.y1 = (r.y0 = element_center.y - set_size.y/2) + set_size.y;
	}

	if (obj_flags & OBJECT_FLAG_xflipped)
	{
		PCOORD tmp = r.x0;
		r.x0 = world.x1 - (r.x1 - world.x0);
		r.x1 = world.x1 - (tmp - world.x0);
	}
	if (obj_flags & OBJECT_FLAG_yflipped)
	{
		PCOORD tmp = r.y0;
		r.y0 = world.y1 - (r.y1 - world.y0);
		r.y1 = world.y1 - (tmp - world.y0);
	}
//	printf("r: %ld, %ld, %ld, %ld\n", r);
	func(&r, user);
}

/*****************************************************************************/
/*                           ObjectLayout record                             */
/*****************************************************************************/

/*
// The creator for a object layout record.
*/

ERRORCODE ObjectLayoutRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	if ((*record = (DatabaseRecordPtr)(new ObjectLayoutRecord(number, type, owner, where))) == NULL)
	{
		return ERRORCODE_Memory;
	}
	return ERRORCODE_None;
}

/*
// The constructor for a project preview record.
*/

ObjectLayoutRecord::ObjectLayoutRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where),
				  elements(0, TRUE)
{
	record.reference_count = 0;
	record.type = -1;
}

/*
// Destructor for a layout record.
*/

ObjectLayoutRecord::~ObjectLayoutRecord()
{
}

/*
// Set the name for this layout record.
*/

VOID ObjectLayoutRecord::set_name(LPCSTR n)
{
	m_csName = n;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE ObjectLayoutRecord::ReadData(StorageDevicePtr device)
{
	SHORT count;
	ERRORCODE error;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->read_cstring(m_csName)) == ERRORCODE_None
			&& (error = device->read(&count, sizeof(count))) == ERRORCODE_None)
	{
		LayoutElement element;		/* For transfer. */

	/* Read the elements. */

		elements.empty();

		while (count-- > 0)
		{
			LayoutElementPtr pelement;

		/* Make sure we can read the element before allocating. */

			if ((error = element.read(device)) != ERRORCODE_None)
			{
				return error;
			}

		/* Create a new layout element. */

			if ((pelement = new LayoutElement) == NULL)
			{
				return ERRORCODE_Memory;
			}
			pelement->record = element.record;

		/* Add it to the end of the list. */

			elements.append(pelement);
		}
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE ObjectLayoutRecord::WriteData(StorageDevicePtr device)
{
	SHORT count = elements.count();
	ERRORCODE error;

	if ((error = device->write(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->write_cstring(m_csName)) == ERRORCODE_None
			&& (error = device->write(&count, sizeof(count))) == ERRORCODE_None)
	{
		LayoutElementPtr element;

	/* Write all the elements. */

		for (element = (LayoutElementPtr)elements.first_element();
							element != NULL;
							element = (LayoutElementPtr)element->next_element())
		{
			if ((error = element->write(device)) != ERRORCODE_None)
			{
				break;
			}
		}
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE ObjectLayoutRecord::SizeofData(StorageDevicePtr device)
{
	return sizeof(record)
 				+ device->size_cstring(m_csName)
 				+ sizeof(SHORT)
				+ elements.count() * LayoutElement::size(device);
}

/*
// Assign method.
*/

ERRORCODE ObjectLayoutRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}
