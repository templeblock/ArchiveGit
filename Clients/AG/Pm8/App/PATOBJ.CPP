/*
// $Header: /PM8/App/PATOBJ.CPP 1     3/03/99 6:08p Gbeddow $
//
// Pattern object routines.
//
// $Log: /PM8/App/PATOBJ.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:32   FRED
// Initial revision.
// 
//    Rev 2.2   19 Apr 1995 16:04:50   JAY
// Removed some symbols.
// 
//    Rev 2.1   15 Feb 1995 17:03:08   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:00   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:33:50   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:28   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:06:00   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:06   JAY
// 
*/

#include "stdafx.h"
#include "pmgobj.h"
#include "utils.h"

#include "patobj.h"

/*****************************************************************************/
/*                              PatternObject                                */
/*****************************************************************************/

/*
// The creator for a pattern object.
*/

ERRORCODE PatternObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	PatternObjectPtr object;

/* Create the pattern object. */

	if ((object = new PatternObject(type, owner)) == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (PageObjectPtr)object;

	if (creation_data != NULL)
	{
		PATTERN_CREATE_STRUCT_PTR pcs = (PATTERN_CREATE_STRUCT_PTR)creation_data;

		object->set_bound(pcs->bound);
		object->precord.pattern = pcs->pattern;
		object->precord.color = pcs->color;
		object->precord.bcolor = pcs->bcolor;
	}

	return ERRORCODE_None;
}

/*
// The constructor for the pattern object.
*/

PatternObject::PatternObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner)
{
	record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
//									| SELECT_FLAG_rotate_handle
									;
	precord.pattern = 0;
	precord.color = COLOR_WHITE;
	precord.bcolor = COLOR_BLACK;
}

/*
// Update a pattern object.
*/

UpdateStatePtr PatternObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	PBOX bound;
	RECT r;
	HDC hdc = rc->destination_hdc;

	if (rc->want_update_names())
	{
		rc->update_name(this, NULL);
	}

/* Convert the bound to destination coordinates. */

	bound = get_bound();
	rc->pbox_to_screen(&bound, TRUE);

	if (rc->convert_pbox(&bound, &r, clip))
	{
		HBRUSH hBrush;
		RGBCOLOR color = rgbcolor_from_color(precord.bcolor);
		SHORT red = RED_COMPONENT_OF(color),
				green = GREEN_COMPONENT_OF(color),
				blue = BLUE_COMPONENT_OF(color);

		if (precord.pattern > 100)
		{
		/* Special pattern. */
			hBrush = CreateHatchBrush(precord.pattern == 101
								? HS_VERTICAL				/* Horizontal cut pattern */
								: HS_HORIZONTAL,			/* Vertical cut pattern */
							RGB(red, green, blue));		/* Color */
		}
		else
		{
			SHORT pattern = 100 - precord.pattern;

		/*
		// Handle the pattern:
		//
		// precord.pattern		color
		// ---------------		-----
		//        0					bcolor (default)
		//       100				WHITE
		//     the rest			lerp()
		//
		// It is the INTENSITY of color, in a CMYK sense.
		//
		// (Note that 'pattern' equals 100-precord.pattern at this point.)
		*/

			if (pattern != 100)
			{
				red = 255-scale_number(255-red, pattern, 100);
				green = 255-scale_number(255-green, pattern, 100);
				blue = 255-scale_number(255-blue, pattern, 100);
			}

			hBrush = CreateSolidBrush(RGB(red, green, blue));
		}

		HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		rc->toggle(FALSE, &r);
		PatBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, PATCOPY);
		rc->toggle(TRUE, &r);

		DeleteObject(SelectObject(hdc, hOldBrush));
	}
	return NULL;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE PatternObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		error = device->read_record(&precord, sizeof(precord));
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE PatternObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&precord, sizeof(precord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE PatternObject::size_data(StorageDevicePtr device)
{
	return INHERITED::size_data(device) + device->size_record(sizeof(precord));
}

/*
// Assign an object to this.
*/

ERRORCODE PatternObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;

	if ((error = INHERITED::assign(sobject)) == ERRORCODE_None)
	{
		precord = ((PatternObject&)sobject).precord;
	}
	return error;
}

PageObjectState* PatternObject::CreateState(void)
{
	return new PatternObjectState;
}

void PatternObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);

	((PatternObjectState*)pState)->m_PRecord = precord;
}

void PatternObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);

	precord = ((PatternObjectState*)pState)->m_PRecord;
}
