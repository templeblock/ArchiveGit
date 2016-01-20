/*
// CALENDAR.CPP
//
// Generic calendar routines.
//
// $Header: /PM8/App/CALENDAR.CPP 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/CALENDAR.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 5     12/01/97 8:46a Jstoner
// changed yearly calendar default style settings
// 
// 4     11/25/97 8:54a Jstoner
// added new calendar type - cal with pic on same page
 * 
 * 3     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.0   14 Aug 1997 15:18:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:20   Fred
// Initial revision.
// 
//    Rev 1.3   17 Jul 1997 14:27:40   Jay
// Refreshes frame after text style change.
// 
//    Rev 1.2   27 Jun 1996 17:20:32   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 09:06:18   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:42:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:00   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:32   JAY
// Initial revision.
// 
//    Rev 1.4   30 Nov 1994 16:35:48   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Nov 1994 12:23:50   JAY
// Now calls group's destroy_objects() method instead of empty() (to eliminate
// record leaks in the database).
// 
//    Rev 1.2   25 Jan 1994 15:18:16   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:06:16   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:10   JAY
*/

#include "stdafx.h"

#include	<time.h>
#include	<ctype.h>
#include	<string.h>

#include "pmgdb.h"
#include "calobj.h"

#include "frameobj.h"
#include "dateobj.h"

/*
// A little info routine.
*/

void CALENDAR_INFO::set_default_style(CALENDAR_TYPE type)
{
	style.type = type;
	switch (type)
	{
		case YEARLY:
		{
			style.line_style = CAL_LINES_NONE;
			style.shadow_style = CAL_SHADOW_25;
			style.weekday_style = WEEKDAY_STYLE_INITIALS;
			style.label_style = CAL_LABEL_BOXED;
			style.outline_style = CAL_OUTLINE_THIN;
			style.color = COLOR_BLACK;
			break;
		}
		default:
		{
			style.line_style = CAL_LINES_ALL;
			style.label_style = CAL_LABEL_BOXED;
			style.outline_style = CAL_OUTLINE_NONE;
			style.shadow_style = CAL_SHADOW_NONE;
			style.weekday_style = WEEKDAY_STYLE_ABBREV;
			style.color = COLOR_BLACK;
			break;
		}
	};	
}

/*
// When it's being destroyed.
*/

void Calendar::destroy(void)
{
	m_LabelFontStyle.Destroy();
	m_NumberFontStyle.Destroy();
}

void Calendar::set_object_list(ObjectListPtr l, BOOL set_database)
{
	list = l;

	if (set_database)
	{
	 	database = (PMGDatabasePtr)l->get_database();

	/* Setup the styles. */

		m_NumberFontStyle.Database(database);
		m_LabelFontStyle.Database(database);

		m_NumberFontStyle.SetDefault();
		m_LabelFontStyle = m_NumberFontStyle;
	}
}

/*
// Find the desired group.
*/

GroupObjectPtr Calendar::find_group(SHORT panel)
{
	PMGPageObjectPtr object;

	for (object = (PMGPageObjectPtr)list->first_object(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		if (object->type() == OBJECT_TYPE_Group && object->get_panel() == panel)
		{
			break;
		}
	}
	return (GroupObjectPtr)object;
}

/*
// Delete a group's objects.
*/

void Calendar::delete_group(SHORT panel)
{
	GroupObjectPtr group;

	if ((group = find_group(panel)) != NULL)
	{
	/* Make sure we're refreshed. */

		group->get_database()->refresh_object(group);
		group->destroy_objects();
	}
}

/*
// Read the data for a calendar.
*/

ERRORCODE Calendar::read(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		if (m_Record.m_wVersion == 0)
		{
			// We need to convert the old styles.
			TextStyle Style(database);
			ST_DEV_POSITION Here;

			if ((error = Style.read(device)) == ERRORCODE_None)
			{
				device->tell(&Here);
				m_LabelFontStyle.FromOldStyle(Style);
				device->seek(Here, ST_DEV_SEEK_SET);
				
				if ((error = Style.read(device)) == ERRORCODE_None)
				{
					device->tell(&Here);
					m_NumberFontStyle.FromOldStyle(Style);
					device->seek(Here, ST_DEV_SEEK_SET);
				}
			}
		}
		else
		{
			TRY
			{
				m_LabelFontStyle.ReadData(device);
				m_NumberFontStyle.ReadData(device);
			}
			CATCH(CErrorcodeException, e)
			{
				error = e->m_error;
			}
			AND_CATCH(CMemoryException, e)
			{
				error = ERRORCODE_Memory;
			}
			AND_CATCH_ALL(e)
			{
				error = ERRORCODE_IntError;
			}
			END_CATCH_ALL
		}
		if (error == ERRORCODE_None)
		{
			//calc_calendar_size();
			calculate();
		}
	}
	return error;
}

/*
// Write the data for a calendar.
*/

ERRORCODE Calendar::write(StorageDevicePtr device)
{
	ERRORCODE error;

	m_Record.m_wVersion = 1;

	if ((error = device->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		TRY
		{
			m_LabelFontStyle.WriteData(device);
			m_NumberFontStyle.WriteData(device);
		}
		CATCH(CErrorcodeException, e)
		{
			error = e->m_error;
		}
		AND_CATCH(CMemoryException, e)
		{
			error = ERRORCODE_Memory;
		}
		AND_CATCH_ALL(e)
		{
			error = ERRORCODE_IntError;
		}
		END_CATCH_ALL
	}
	return error;
}

/*
// Return the size of the data to be written.
*/

ST_MAN_SIZE Calendar::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(m_Record))
				+ m_LabelFontStyle.SizeofData(device)
				+ m_NumberFontStyle.SizeofData(device);
}

/*
// Find a date object.
*/

PMGPageObjectPtr Calendar::find_object(SHORT group_panel, SHORT panel, PMG_OBJECT_TYPE type)
{
	GroupObjectPtr group;
	PMGPageObjectPtr object = NULL;

	if ((group = find_group(group_panel)) != NULL)
	{
	/* Look in the group for a match. */

		for (object = (PMGPageObjectPtr)group->object_list()->first_object();
								object != NULL;
								object = (PMGPageObjectPtr)object->next_object())
		{
			if (object->type() == type && object->get_panel() == panel)
			{
				break;
			}
		}
	}
	return object;
}

/*
// Get the style for the first frame of a particular kind.
*/

ERRORCODE Calendar::get_frame_style(CTextStyle* pStyle, SHORT group_panel, SHORT panel_start)
{
#if 0
	FrameObjectPtr object;

	if ((object = (FrameObjectPtr)find_object(group_panel, panel_start, OBJECT_TYPE_Frame)) == NULL
			|| object->get_paragraph_style(style) != ERRORCODE_None)
#endif
	{
	/* Error recovery. */
		pStyle->SetDefault();
	}
	return ERRORCODE_None;
}

/*
// Get a date object.
*/

CalendarDateObjectPtr Calendar::get_date_object(SHORT date)
{
	return (CalendarDateObjectPtr)find_object(DATE_PANEL,
													date+DATE_PANEL,
													OBJECT_TYPE_CalendarDate);
}

/*
// Set the label font style.
// This will set the style for all the labels.
*/

void Calendar::SetLabelFontStyle(const CTextStyle& style, BOOL update_all)
{
/* Remember our style. */

	m_LabelFontStyle = style;

/* And change all the labels. */

	if (update_all)
	{
		for (SHORT i = 0; i < days_in_week; i++)
		{
			CFrameObject* pFrame;

			if ((pFrame = (CFrameObject*)find_object(SKELETON_PANEL, LABEL_FRAME_PANEL+i, OBJECT_TYPE_Frame)) != NULL)
			{
				pFrame->SetTextStyle(&style);

				// Refresh us.
				PBOX pbox;
				pFrame->get_refresh_bound(&pbox);

				database->do_refresh_notify(&pbox, REFRESH_ALL, NULL);
			}
		}
	}
}

/*
// Set the number font style.
// This will set the style for all the date numbers.
*/

void Calendar::SetNumberFontStyle(const CTextStyle& style, BOOL update_all)
{
/* Remember our style. */

	m_NumberFontStyle = style;

/* And change all the dates. */

	if (update_all)
	{
		for (SHORT i = 0; i < days_in_month; i++)
		{
			CalendarDateObjectPtr date;

			if ((date = get_date_object(i)) != NULL)
			{
				date->SetTextStyle(&style);

				// Refresh us.
				PBOX pbox;
				date->get_refresh_bound(&pbox);

				database->do_refresh_notify(&pbox, REFRESH_ALL, NULL);
			}
		}
	}
}

/*
// Get cell dimensions.
*/

PPNT Calendar::get_cell_dimensions(void)
{
	PPNT p;

	p.x = cell_width;
	p.y = cell_height;
	return p;
}


void Calendar::calc_calendar_size()
{
	PBOX box;
	//PBOX dimensions = calnedar_bound;
	switch (m_picture_style)
	{
		case CAL_PICTURE_NONE:
			{
				box.x0 = 0;
				box.y0 = 0;
				box.x1 = calendar_bound.x1; //dimensions.x;
				box.y1 = calendar_bound.y1; //dimensions.y;
				break;
			}
		case CAL_PICTURE_TOP:
			{
				box.x0 = 0;
				box.y0 = calendar_bound.y1 / 2; //dimensions.y / 2;
				box.x1 = calendar_bound.x1; //dimensions.x;
				box.y1 = calendar_bound.y1; //dimensions.y;
				break;
			}
		case CAL_PICTURE_LEFT:
			{
				box.x0 = calendar_bound.x1 / 2; //dimensions.x / 2;
				box.y0 = 0;
				box.x1 = calendar_bound.x1; //dimensions.x;
				box.y1 = calendar_bound.y1; //dimensions.y;
				break;
			}
		calendar_bound = box;
	};
		
}