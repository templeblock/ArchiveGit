/*
// $Header: /PM8/App/DATEOBJ.CPP 1     3/03/99 6:04p Gbeddow $
//
// Calendar date object routines.
//
// $Log: /PM8/App/DATEOBJ.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     11/10/97 3:50p Jstoner
// fixed some problems regarding adding yearly calendars
// 
// 2     11/07/97 2:22p Jstoner
// year calendar updates...good stopping/saving point
// 
//    Rev 1.0   14 Aug 1997 15:19:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:02   Fred
// Initial revision.
// 
//    Rev 1.3   16 Jul 1997 17:56:24   Jay
// Calendar date improvements
// 
//    Rev 1.2   21 Feb 1997 15:08:58   Jay
// New mode handling; crop mode support
// 
//    Rev 1.1   26 Jun 1996 09:06:22   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:48:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:28   FRED
// Initial revision.
// 
//    Rev 2.1   09 Mar 1995 08:27:16   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:47:38   JAY
// Initial revision.
// 
//    Rev 1.8   30 Nov 1994 16:35:24   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.7   01 Jun 1994 16:35:36   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.6   07 Apr 1994 14:12:54   JAY
// Re-establishes the main calendar object after one of the sub-groups changes.
// 
//    Rev 1.5   17 Feb 1994 09:25:18   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.4   10 Feb 1994 15:14:26   MACDUFF
// Toggle functions now return flag indicating whether they drew anything.
// 
//    Rev 1.3   28 Jan 1994 17:11:16   JAY
//  
// 
//    Rev 1.2   25 Jan 1994 15:18:04   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:07:14   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:26   JAY
// 
*/

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "pmgdb.h"
#include "utils.h"
#include "action.h"

#include "calobj.h"

#include "grafobj.h"
#include "frameobj.h"
#include "dateobj.h"
#include "layrec.h"

/*****************************************************************************/
/*                             CalendarDateObject                            */
/*****************************************************************************/

/*
// The creator for a calendar date object.
*/

ERRORCODE CalendarDateObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	CalendarDateObjectPtr object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

	if ((object = new CalendarDateObject(type, owner)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* We created the object. */
		if ((error = object->HandleCreationData(creation_data)) != ERRORCODE_None)
		{
			delete object;
			object = NULL;
		}
	}

	*record = (PageObjectPtr)object;
	return error;
}

/*
// The constructor for the calendar date object.
*/

CalendarDateObject::CalendarDateObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
				: CFrameObject(type, owner)
{
	record.select_flags = SELECT_FLAG_boundary | SELECT_FLAG_drag_handle | SELECT_FLAG_solo_select;
}

BOOL CalendarDateObject::toggle_selects(RedisplayContextPtr rc, FLAGS which)
{
	if (which == 0)
	{
		which = record.select_flags;
	}

	if ((which &= record.select_flags) != 0)
	{
		HDC hdc = rc->destination_hdc;
	/* Need to toggle something. */

		if (record.rotc.rotation == 0)
		{
			RECT r;
			PBOX pbox;

			pbox = rrecord.unrotated_bound;

			if (pbox.x0 > pbox.x1)
			{
				PCOORD tmp = pbox.x0;
				pbox.x0 = pbox.x1;
				pbox.x1 = tmp;
			}

			if (pbox.y0 > pbox.y1)
			{
				PCOORD tmp = pbox.y0;
				pbox.y0 = pbox.y1;
				pbox.y1 = tmp;
			}

			rc->pbox_to_screen(&pbox, FALSE);

			if (rc->convert_pbox(&pbox, &r, NULL))
			{
#if 0
				r.left--;
				r.top--;
				r.right++;
				r.bottom++;
#endif

				if (which & SELECT_FLAG_boundary)
				{
					do_XorBox(hdc, r, 3);
				}
			}
		}
	}
	return TRUE;
}

/*
// Return the refresh bounds for this object.
*/

VOID CalendarDateObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
	*refresh_bound = get_bound();
}

/*
// Return whether a point is in an object or not.
*/

BOOL CalendarDateObject::pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
	return RectPageObject::pt_in_object(p, rc, sub_object);
}

/*
// Return whether a point is over an object's move area or not.
*/

BOOL CalendarDateObject::pt_on_move_area(PPNT p, RedisplayContextPtr rc)
{
	return RectPageObject::pt_on_move_area(p, rc);
}

/*
// Move our object.
// This handles the end_drag state.
*/

PMG_OBJECT_ACTION CalendarDateObject::move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data /*=NULL*/, BOOL fPanning /*=FALSE*/)
{
	if (state == DRAG_STATE_end_drag)
	{
	/* Move us back. */

		RectPageObject::move_object(DRAG_STATE_abort, vector, current_xy, NULL, fPanning);

	/* Do a copy! */

		SHORT source_panel = get_panel() - DATE_PANEL;

	/* Get the calendar object so we can use it. */

		CalendarObjectPtr calobj = (CalendarObjectPtr)get_container();

		SHORT dest_panel = -1;
		PMGPageObjectPtr object = calobj->FindDate(current_xy, &dest_panel);

		if (object != NULL && source_panel != dest_panel)
		{
		/*
 		// We need to do the copy.
 		*/

		/*
		// Delete the old date enhancements.
		// We pass FALSE here assuming that an undo command somewhere above
		// is handling all of this. We will end up with memory loss if that
		// assumption is incorrect, but in the 99.9999% of the cases it will
		// be correct (assuming the code remains the same).
		*/

			calobj->delete_date_text(dest_panel, FALSE);
			calobj->delete_date_graphic(dest_panel, FALSE);

		/*
		// Create copies of any text and picture.
		*/

			PageObjectPtr sub_object, dup_object;
			CalendarRef calendar = calobj->get_calendar();
			GroupObjectPtr group;

		/* Duplicate any enhanced graphic. */

			if ((sub_object = calobj->get_date_graphic(source_panel)) != NULL)
			{
				if ((dup_object = sub_object->duplicate()) != NULL)
				{
				/* Add the graphic to the correct list. */

					if ((group = calendar.find_group(GRAPHIC_PANEL)) == NULL)
					{
						return ACTION_DO_NOTHING;
					}

					group->object_list()->append(dup_object);

				/* Lay it out with the help of the calendar date object. */

					((CalendarDateObjectPtr)object)->layout_enhanced_graphic((GraphicObjectPtr)dup_object, calendar);

				/* Re-establish group with newly layed-out object. */

					group->establish();
				}
			}

		/* Duplicate any enhanced text. */

			if ((sub_object = calobj->get_date_text(source_panel)) != NULL)
			{
				if ((dup_object = sub_object->duplicate()) != NULL)
				{
				/* Add the frame to the correct list. */

					if ((group = calendar.find_group(TEXT_PANEL)) == NULL)
					{
						return ACTION_DO_NOTHING;
					}

					group->object_list()->append(dup_object);

				/* Lay it out with the help of the calendar date object. */

					((CalendarDateObjectPtr)object)->layout_enhanced_frame((CFrameObject*)dup_object, calendar);

				/* Re-establish group with newly layed-out object. */

					group->establish();
				}
			}

		/* To show the change. */

			get_database()->refresh_object(object);
			get_database()->document_modified();

		/* Make sure the container is set (since we're selecting this way). */

			object->set_container(calobj);

		/* Make this the current object. */

			calobj->set_current_date(object->get_panel());
			calobj->establish(FALSE);

			*data = object;
			return ACTION_SELECT;
		}
		return ACTION_DO_NOTHING;
	}
	else
	{
		return RectPageObject::move_object(state, vector, current_xy, NULL, fPanning);
	}
}

/*
// Our primary click action.
*/

PMG_OBJECT_ACTION CalendarDateObject::primary_action(LPVOID far *data)
{
	if (data == NULL)
	{
		return ACTION_DO_NOTHING;
	}
	else
	{
		*data = &record.panel;
		return ACTION_SELECT_DATE;
	}
}

/*
// Our secondary click action.
*/

PMG_OBJECT_ACTION CalendarDateObject::secondary_action(LPVOID far *data)
{
	if (data == NULL)
	{
		return ACTION_DO_NOTHING;
	}
	else
	{
		*data = &record.panel;
		return ACTION_EDIT_DATE;
	}
}

/*
// Get the properties for this object.
*/

void CalendarDateObject::GetObjectProperties(CObjectProperties& Properties)
{
}

/*
// Set the properties for this object.
*/

BOOL CalendarDateObject::SetObjectProperties(CObjectProperties& Properties)
{
	return FALSE;
}

/*
// The callback function used when laying out a graphic for an enhanced date.
*/

PRIVATE BOOL
size_func(PBOX_PTR r, VOIDPTR data)
{
/* Set the bound to the calc'ed size. */
	((RectPageObjectPtr)data)->set_bound(*r);
	return TRUE;
}

/*
// Compute the enhanced date bound.
*/

VOID CalendarDateObject::get_enhanced_bound(CalendarRef calendar, PBOX_PTR ebound)
{
	PBOX bound = get_bound();

	
	CALENDAR_TYPE type = calendar.get_type();

	ebound->x0 = (type == MONTHLY || type == YEARLY)
						? bound.x0
						: calendar.get_label_bound().x1;
	ebound->y0 = bound.y0;

	PPNT dims = calendar.get_cell_dimensions();

	ebound->x1 = ebound->x0 + dims.x;
	ebound->y1 = ebound->y0 + dims.y;
}

/*
// Layout an enhanced graphic.
*/

VOID CalendarDateObject::layout_enhanced_graphic(GraphicObjectPtr object, CalendarRef calendar)
{
	CALENDAR_TYPE type = calendar.get_type();
 	PCOORD line_thickness = calendar.get_line_thickness();

/* Now add the graphic in. */

	if (object != NULL)
	{
		SHORT date = get_panel() - DATE_PANEL;

		object->set_select_flags(SELECT_FLAG_boundary);
		object->set_panel(date + GRAPHIC_PANEL);

	/* Get our full bound. */

		PBOX bound;

		get_enhanced_bound(calendar, &bound);

		PPNT dims;

		if (object->original_dims(&dims))
		{
			LayoutElement layout_element;
			POINT size;

			size.x = (SHORT)dims.x;
			size.y = (SHORT)dims.y;

			layout_element.record.offset.x = 0;
			layout_element.record.offset.y = 0;

			if (type == MONTHLY || type == YEARLY)
			{
				bound.x0 += line_thickness;
				bound.y0 += line_thickness;
				layout_element.record.flags = ELEMENT_ALIGN_BOTTOM;
				layout_element.record.where.left = 0;
				layout_element.record.where.top = 0;
				layout_element.record.where.right = 1000;
				layout_element.record.where.bottom = 1000;
			}
			else
			{
				layout_element.record.flags = 0;
				layout_element.record.where.left = 750;
				layout_element.record.where.top = 150;
				layout_element.record.where.right = 950;
				layout_element.record.where.bottom = 950;
			}
		/*
		// Generate the rect for this element with the graphic we have chosen.
		*/

			layout_element.generate_rects(bound, size,
											0, size_func, object);
		}
		else
		{
		/* Recovery in case of failure. */

			object->set_bound(bound);
		}
		object->calc();
	}
}

/*
// Layout an enhanced text frame.
*/

VOID CalendarDateObject::layout_enhanced_frame(CFrameObject* object, CalendarRef calendar)
{
	PBOX bound;

	get_enhanced_bound(calendar, &bound);

	object->set_bound(bound);
	object->set_panel(get_panel() - DATE_PANEL + TEXT_PANEL);
	object->calc();
}
