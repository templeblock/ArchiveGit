/*
// $Header: /PM8/App/CALOBJ.CPP 1     3/03/99 6:03p Gbeddow $
//
// Calendar object routines.
//
// $Log: /PM8/App/CALOBJ.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     9/03/98 5:06p Jayn
// Added calendar pages now propagate the label and number font styles.
// 
// 3     11/10/97 3:50p Jstoner
// fixed some problems regarding adding yearly calendars
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.1   21 Oct 1997 15:15:12   JOE
// multi-page calendar mods
// 
//    Rev 1.0   14 Aug 1997 15:18:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:22   Fred
// Initial revision.
// 
//    Rev 1.3   08 Jul 1996 08:49:04   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.2   26 Jun 1996 14:22:22   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 09:06:20   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:48:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:36   FRED
// Initial revision.
// 
//    Rev 2.2   19 Apr 1995 16:04:56   JAY
// Removed some symbols.
// 
//    Rev 2.1   09 Mar 1995 08:26:50   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:47:34   JAY
// Initial revision.
// 
//    Rev 1.13   09 Jan 1995 16:26:44   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.12   30 Nov 1994 16:34:30   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.11   18 Jul 1994 09:55:04   JAY
// Yet another calendar resizing problem - now ALWAYS establishes.
// 
//    Rev 1.10   01 Jun 1994 16:35:36   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.9   12 May 1994 11:55:30   JAY
// Fixed the problem of date pictures being wrong-sized after the calendar is
// re-sized (e.g. the printer or paper size is changed).
// 
//    Rev 1.8   15 Mar 1994 17:15:52   JAY
// Removed an error added in the last rev.
// 
//    Rev 1.7   15 Mar 1994 17:11:36   JAY
// Re-establishes all the main calendar groups after establishing one of them.
// 
//    Rev 1.6   17 Feb 1994 09:25:16   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.5   15 Feb 1994 13:28:10   JAY
// Fixed loading of calendars corrupted by a bug in a previous version.
// 
//    Rev 1.4   01 Feb 1994 14:02:00   JAY
// Added a get_refresh_bounds() method to override the (not-working)
// GroupObject method.
// 
//    Rev 1.3   31 Jan 1994 17:42:56   JAY
//  
// 
//    Rev 1.2   25 Jan 1994 15:17:48   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:07:16   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:24   JAY
// 
*/

#include "stdafx.h"

#include "pmgdb.h"

#include "calobj.h"

#include "framerec.h"
#include "frameobj.h"
#include "grafobj.h"
#include "dateobj.h"

#include "utils.h"
#include "action.h"
#include "textflow.h"


/*****************************************************************************/
/*                              CalendarObject                               */
/*****************************************************************************/

/*
// The creator for a calendar object.
*/

ERRORCODE CalendarObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	CalendarObjectPtr object;
	short ps;

	if (creation_data != NULL)
	{
		CALENDAR_CREATE_STRUCT_PTR ccs = (CALENDAR_CREATE_STRUCT_PTR)creation_data;
		ps = ccs->picture_style;
	}
	else
		ps = 0;

/* Create the calendar object. */

	if ((object = new CalendarObject(type, owner, ps)) == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (PageObjectPtr)object;

	object->calendar.set_object_list(object->object_list());

	if (creation_data != NULL)
	{
		CALENDAR_CREATE_STRUCT_PTR ccs = (CALENDAR_CREATE_STRUCT_PTR)creation_data;

		object->calendar.set_info(*ccs->info);

		object->calendar.set_bound(ccs->bound);
		object->set_bound(ccs->bound);
		if (ccs->m_pLabelFontStyle != NULL)
		{
			object->calendar.SetLabelFontStyle(*(ccs->m_pLabelFontStyle), FALSE);
		}
		if (ccs->m_pNumberFontStyle != NULL)
		{
			object->calendar.SetNumberFontStyle(*(ccs->m_pNumberFontStyle), FALSE);
		}

		object->calendar.calculate();
		object->calendar.build(ccs->full_build);
		object->establish(FALSE);
		object->fix_weekly();
		object->SetPictureStyle(ccs->picture_style);
	}

	return ERRORCODE_None;
}

/*
// The constructor for the pattern object.
*/

CalendarObject::CalendarObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, short ps)
		:  GroupObject(type, owner), calendar(FALSE, (PMGDatabasePtr)owner)
{
	record.secondary_action = ACTION_CALENDAR_STYLE;
	current_date = -1;
	calendar.SetPictureStyle(ps);
}

/*
// Update a calendar object.
*/

UpdateStatePtr CalendarObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	return GroupObject::update(rc, extent, clip, ustate, refresh_type);
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE CalendarObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = GroupObject::read_data(device)) == ERRORCODE_None)
	{
		PBOX bound = get_bound();
		establish(FALSE);
		calendar.set_bound(bound);						/* So it can calc right. */
		error = calendar.read(device);
	}

	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE CalendarObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = GroupObject::write_data(device)) == ERRORCODE_None)
	{
		error = calendar.write(device);
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE CalendarObject::size_data(StorageDevicePtr device)
{
	return GroupObject::size_data(device) + calendar.size(device);
}

/*
// Fix a weekly calendar.
*/

VOID CalendarObject::fix_weekly()
{
	CALENDAR_INFO_REF info = calendar.get_info();

/* Do a fix to get the date numbers in the right place. */

	if (info.style.type == WEEKLY)
	{
		GroupObjectPtr date_group = calendar.find_group(DATE_PANEL);
		CalendarDateObjectPtr object;
		PBOX label = calendar.get_label_bound();

		for (object = (CalendarDateObjectPtr)date_group->object_list()->first_object();
 						object != NULL;
 						object = (CalendarDateObjectPtr)object->next_object())
		{
			DB_RECORD_NUMBER f_record = object->FrameRecord();
			CFrameRecord* frame;
			PMGDatabasePtr db = object->get_database();

		/* Do something special to make this look right. */

			if ((frame = (CFrameRecord*)db->get_record(f_record, NULL, RECORD_TYPE_Frame)) != NULL)
			{
				PBOX bound = frame->ObjectBounds();
				bound.x1 = label.x1;
				frame->ObjectBounds(bound);

				frame->release(TRUE);

				CTextFlow TextFlow(object, 0, -1);
				TextFlow.FlowText();
			}

		/* We've usurped the usual Calc definition. Don't let it change later. */

			object->set_flags(object->get_flags() | OBJECT_FLAG_calc_ignore);
		}
	}
}

/*
// Calculate.
*/

VOID CalendarObject::calc(PBOX_PTR panel_world, FLAGS panel_flags)
{
	GroupObject::calc(panel_world, panel_flags);

	calendar.set_bound(get_bound());
	calendar.calculate();
	fix_weekly();

/* Relayout all the graphics. */

	GroupObjectPtr group;

	if ((group = calendar.find_group(GRAPHIC_PANEL)) != NULL)
	{
	/*
	// Run through all graphics in this panel.
	// (BTW: There actually should only BE graphics on this panel.)
	*/
		GraphicObjectPtr object;
		for (object = (GraphicObjectPtr)group->object_list()->first_object();
				object != NULL;
				object = (GraphicObjectPtr)object->next_object())
		{
			SHORT date = object->get_panel() - GRAPHIC_PANEL;
			CalendarDateObjectPtr caldate;

			if ((caldate = get_date_object(date)) != NULL)
			{
				caldate->layout_enhanced_graphic(object, calendar);
			}
		}
	}
	establish(FALSE);
}

/*
// Group Calculate.
*/

VOID CalendarObject::group_calc(PBOX_PTR current_group_bound,
 									PBOX_PTR org_group_bound,
 									FLAGS group_flags,
 									ROTATION_COMPONENT_PTR group_rotc)
{
	GroupObject::group_calc(current_group_bound, org_group_bound, group_flags, group_rotc);

	calendar.set_bound(get_bound());
}

/*
// Destroy this object.
*/

VOID CalendarObject::destroy(VOID)
{
	calendar.destroy();
	GroupObject::destroy();
}

/*
// Assign an object to this.
*/

ERRORCODE CalendarObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;
	CalendarObject& cobject = (CalendarObject&)sobject;

/* Copy all the objects over. */

	if ((error = GroupObject::assign(sobject)) == ERRORCODE_None)
	{
	/* Copy the calendar-specific stuff over. */

		calendar = cobject.calendar;
		calendar.set_object_list(object_list(), FALSE);
	}
	return error;
}

/*
// Look to see if a point is within the object of a calendar group.
*/

BOOL CalendarObject::pt_in_group(GroupObjectPtr group, PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
	if (group == NULL)
	{
		return FALSE;
	}

	ObjectListPtr list;
	PMGPageObjectPtr object;

/* Look in the group for a match. */

	list = group->object_list();

	for (object = (PMGPageObjectPtr)list->first_object(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		if (!(object->get_flags() & OBJECT_FLAG_locked))
		{
			if (object->pt_in_object(p, rc, NULL))
			{
				*sub_object = object;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CalendarObject::pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
	if (sub_object != NULL)
	{
		if (pt_in_group(calendar.find_group(DATE_PANEL), p, rc, sub_object)
				|| pt_in_group(calendar.find_group(SKELETON_PANEL), p, rc, sub_object))
		{
		/* Remember this guy's container. */
			(*sub_object)->set_container(this);
			return TRUE;
		}

	/* Assume us as a default. */
		*sub_object = this;
	}

	return IsPtInBox(p.x, p.y, &rrecord.bound);
}

/*
// Return the refresh bounds for this object.
// We bypass the group refresh bound method since it doesn't work.
*/

VOID CalendarObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
	RectPageObject::get_refresh_bound(refresh_bound, rc);
}

/*
// Return the number of days in the calendar.
*/

SHORT CalendarObject::days_in_calendar(VOID)
{
	return calendar.get_days_in_calendar();
}

/*
// Return the number of the first day of the calendar.
*/

SHORT CalendarObject::first_day(VOID)
{
	return calendar.get_first_day();
}

/*
// Find an object in a group.
*/

PMGPageObjectPtr CalendarObject::find_object(SHORT panel, SHORT date, PMG_OBJECT_TYPE type)
{
	SHORT days = days_in_calendar();
	PMGPageObjectPtr object;

	if (date == -1)
	{
		date = current_date;
	}

	if (date >= DATE_PANEL)
	{
		date -= DATE_PANEL;
	}

	if (date < 0 || date >= days)
	{
		return NULL;
	}

	if ((object = calendar.find_object(panel, date+panel, type)) != NULL)
	{
		object->set_container(this);
	}
	return object;
}

/*
// Get the date object for a particular date.
*/

CalendarDateObjectPtr CalendarObject::get_date_object(SHORT date)
{
	return (CalendarDateObjectPtr)find_object(DATE_PANEL,
								date,
								OBJECT_TYPE_CalendarDate);
}
	
/*
// Set this date as the current date.
// The date object is returned if successful.
*/

CalendarDateObjectPtr CalendarObject::set_current_date(SHORT date)
{
	CalendarDateObjectPtr object;

	if ((object = get_date_object(date)) == NULL)
	{
		current_date = -1;
	}
	else
	{
		current_date = object->get_panel() - DATE_PANEL;
	}
	return object;
}

/*
// Get the picture for a date.
*/

GraphicObjectPtr CalendarObject::get_date_graphic(SHORT date)
{
	return (GraphicObjectPtr)find_object(GRAPHIC_PANEL,
								date,
								OBJECT_TYPE_Graphic);
}

/*
// Get the text frame for a date.
// If the frame doesn't exist, one is created.
*/

CFrameObject* CalendarObject::get_date_text(SHORT date, BOOL create)
{
	CFrameObject* object;

	if ((object = (CFrameObject*)find_object(TEXT_PANEL,
								date,
								OBJECT_TYPE_Frame)) == NULL
			&& create)
	{
		CalendarDateObjectPtr caldate;
		GroupObjectPtr group;

	/* Get the calendar date object. */

		if ((caldate = get_date_object(date)) == NULL
				|| (group = calendar.find_group(TEXT_PANEL)) == NULL)
		{
			return NULL;
		}

		PBOX bound = caldate->get_bound();
		short ctype = calendar.get_type();
		BOOL monthly = (ctype == MONTHLY || ctype == YEARLY);
		SHORT factor = monthly ? 10 : 5;
		SHORT size = (SHORT)scale_pcoord(bound.y1-bound.y0,
						(72*3)/4, factor*PAGE_RESOLUTION);

		date = caldate->get_panel() - DATE_PANEL;

		CTextStyle Style(get_database(), TRUE);

		Style.SetDefault();
		Style.Size(MakeFixed(size));
		Style.BaseSize(MakeFixed(size));
		Style.UpdateFontMetrics();

		ObjectListPtr list = group->object_list();

		if (calendar.add_frame(&bound,
				 		NULL,
						monthly
							? ALIGN_center
							: ALIGN_left,
						ALIGN_middle,
						date + TEXT_PANEL,
						FRAME_FLAG_fit_text,
						0,
 						list,
						OBJECT_TYPE_Frame,
						&Style) == ERRORCODE_None)
		{
			object = (CFrameObject*)list->last_object();
			caldate->layout_enhanced_frame(object, calendar);
			group->establish();
			establish(FALSE);
			get_database()->document_modified();
		}
	}
	return object;
}

/*
// Get the title frame.
*/

CFrameObject* CalendarObject::get_title_frame(VOID)
{
	CFrameObject* pFrame;
	if ((pFrame = (CFrameObject*)calendar.find_object(SKELETON_PANEL, TITLE_PANEL, OBJECT_TYPE_Frame)) != NULL)
	{
		pFrame->set_container(this);
	}
	return pFrame;
}

/*
// Delete the text for a date.
*/

VOID CalendarObject::delete_date_text(SHORT date, BOOL fDelete /*=TRUE*/)
{
	CFrameObject* object;
	GroupObjectPtr group;

	if ((object = get_date_text(date)) != NULL
			&& (group = calendar.find_group(TEXT_PANEL)) != NULL)
	{
	/* Detach from the list. */

		group->object_list()->detach(object);
		group->establish();
		establish(FALSE);

	/* Delete it. */

		if (fDelete)
		{
			get_database()->delete_object(object);
		}
	}
}

/*
// Delete the graphic for a date.
*/

VOID CalendarObject::delete_date_graphic(SHORT date, BOOL fDelete /*=TRUE*/)
{
	GraphicObjectPtr object;
	GroupObjectPtr group;

/* Find the object and its group. */

	if ((object = get_date_graphic(date)) != NULL
			&& (group = calendar.find_group(GRAPHIC_PANEL)) != NULL)
	{
	/* Detach from the list. */

		group->object_list()->detach(object);
		group->establish();
		establish(FALSE);

	/* Delete it. */

		if (fDelete)
		{
			get_database()->delete_object(object);
		}
	}
}

/*
// Make a graphic into a date graphic by adding it to the right list and
// laying it out.
*/

VOID CalendarObject::make_date_graphic(GraphicObjectPtr object, SHORT date, BOOL fDeleteOld /*=TRUE*/)
{
/* Get rid of any graphic that already exists. */

	delete_date_graphic(date, fDeleteOld);

/* Add the graphic to the correct list. */

	if (object != NULL)
	{
		GroupObjectPtr group;
	
		if ((group = calendar.find_group(GRAPHIC_PANEL)) == NULL)
		{
			return;
		}
	
		group->object_list()->append(object);
	
	/* Lay it out with the help of the calendar date object. */
	
		CalendarDateObjectPtr caldate = get_date_object(date);
	
		caldate->layout_enhanced_graphic(object, calendar);
		group->establish();
		establish(FALSE);
	}
}

/*
// Make a frame into a date frame by adding it to the right list and
// laying it out.
*/

VOID CalendarObject::make_date_frame(CFrameObject* object, SHORT date, BOOL fDeleteOld /*=TRUE*/)
{
/* Get rid of any frame that already exists. */

	delete_date_text(date, fDeleteOld);

	if (object != NULL)
	{
	/* Add the frame to the correct list. */
	
		GroupObjectPtr group;
	
		if ((group = calendar.find_group(TEXT_PANEL)) == NULL)
		{
			return;
		}
	
		group->object_list()->append(object);
	
	/* Lay it out with the help of the calendar date object. */
	
		CalendarDateObjectPtr caldate = get_date_object(date);
	
		caldate->layout_enhanced_frame(object, calendar);
		group->establish();
		establish(FALSE);
	}
}

/*
// Rebuild a calendar after making a style change.
*/

VOID CalendarObject::rebuild(BOOL reset_title)
{
	calendar.rebuild(reset_title);
	establish(FALSE);
	fix_weekly();
}

/*
// Return the date object under a certain point.
*/

PMGPageObjectPtr CalendarObject::FindDate(PPNT pPnt, SHORT* pPanel)
{
/* Figure out which date object we're over (if any). */

	PMGPageObjectPtr pObject;
	GroupObjectPtr group = find_group(DATE_PANEL);

	for (pObject = (PMGPageObjectPtr)group->object_list()->first_object();
				pObject != NULL;
				pObject = (PMGPageObjectPtr)pObject->next_object())
	{
		if (!(pObject->get_flags() & OBJECT_FLAG_needs_calc)
					&& pObject->pt_in_object(pPnt, NULL, NULL))
		{
		/* This is the one. */
			*pPanel = pObject->get_panel() - DATE_PANEL;
			break;
		}
	}
	return pObject;
}

