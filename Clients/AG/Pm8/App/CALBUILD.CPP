/*
// CALBUILD.CPP
//
// Calendar building routines.
//
// $Header: /PM8/App/CALBUILD.CPP 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/CALBUILD.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 14    9/03/98 5:06p Jayn
// Added calendar pages now propagate the label and number font styles.
// 
// 13    5/28/98 3:58p Cboggio
// Calendar changes for localization
// 
// 12    5/27/98 4:49p Cboggio
// 
// 11    5/27/98 4:35p Cboggio
// calendar changes for localization
// 
// 10    12/01/97 8:46a Jstoner
// changed yearly calendar default style settings
// 
// 9     11/25/97 8:54a Jstoner
// added new calendar type - cal with pic on same page
// 
// 8     11/13/97 2:57p Jstoner
// fixed weekly calendars to span months
// 
// 7     11/12/97 1:57p Jstoner
// updates for multipage weekly calendars, weekly calendar wizard
// 
// 6     11/10/97 3:50p Jstoner
// fixed some problems regarding adding yearly calendars
// 
// 5     11/07/97 2:22p Jstoner
// year calendar updates...good stopping/saving point
// 
// 4     11/05/97 2:31p Jstoner
// can now print multi-page, single-sided calendar projects
// todo: fix print preview and double-sided printing
 * 
 * 3     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.0   14 Aug 1997 15:18:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:20   Fred
// Initial revision.
// 
//    Rev 1.9   17 Jul 1997 14:27:20   Jay
// Won't change the fill color to white for inverted weekdays if has fill.
// 
//    Rev 1.8   29 May 1997 08:42:56   Fred
// New fill color
// 
//    Rev 1.7   09 Apr 1997 14:50:42   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.6   07 Feb 1997 08:44:02   Fred
// Passes GetCharacterFormatting() to AdvanceWidth()
// 
//    Rev 1.5   11 Nov 1996 12:22:22   johno
// Moved strings to resource DLL
// 
//    Rev 1.4   06 Aug 1996 10:43:04   Jay
// Changes to CTxp
// 
//    Rev 1.3   27 Jun 1996 17:20:32   Jay
//  
// 
//    Rev 1.2   26 Jun 1996 14:22:20   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 09:06:16   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:47:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:06   FRED
// Initial revision.
// 
//    Rev 2.2   07 Jul 1995 16:10:46   JAY
// Misc changes.
// 
//    Rev 2.1   29 Mar 1995 08:58:50   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:47:32   JAY
// Initial revision.
// 
//    Rev 1.8   30 Nov 1994 16:37:34   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.7   04 Nov 1994 12:23:14   JAY
// Now calls group's destroy_objects() method instead of empty() (to eliminate
// record leaks in the database).
// 
//    Rev 1.6   04 Aug 1994 07:33:48   JAY
// Large model and 32-bit changes
// 
//    Rev 1.5   01 Jun 1994 16:43:42   JAY
// Now calls new_object() instead of create_pmg_object().
// 
//    Rev 1.4   17 Mar 1994 10:20:40   JAY
// Added static functions to return a month or weekday name.
// 
//    Rev 1.3   28 Jan 1994 17:11:10   JAY
//  
// 
//    Rev 1.2   25 Jan 1994 15:18:14   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:07:06   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:24   JAY
// 
*/

#include "stdafx.h"

#include "pmw.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pmgdb.h"
#include "utils.h"
#include "calendar.h"
#include "textflow.h"
#include "action.h"
#include "fontsrv.h"

#include "patobj.h"
#include "framerec.h"
#include "frameobj.h"
#include "grafobj.h"
#include "grpobj.h"
#include "dateobj.h"
#include "ctxp.h"
#include "pmwdoc.h"
#include "caldoc.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif


/*
// For now!
*/

// JRO
/*
PRIVATE CHAR *
weekday_name[] =
{
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
},
* month_name[] =
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};
*/
PRIVATE UINT
weekday_name_id[] =
{
   IDS_SUNDAY,
   IDS_MONDAY,
   IDS_TUESDAY,
   IDS_WEDNESDAY,
   IDS_THURSDAY,
   IDS_FRIDAY,
   IDS_SATURDAY
},
month_name_id[] =
{
   IDS_JANUARY,
   IDS_FEBRUARY,
   IDS_MARCH,
   IDS_APRIL,
   IDS_MAY,
   IDS_JUNE,
   IDS_JULY,
   IDS_AUGUST,
   IDS_SEPTEMBER,
   IDS_OCTOBER,
   IDS_NOVEMBER,
   IDS_DECEMBER
};


extern SHORT days_in_a_month(SHORT year, SHORT month);
/*
// Add a box to the calendar.
*/

ERRORCODE near Calendar::add_line(PBOX_PTR bound, SHORT pattern, COLOR color, SHORT panel, ObjectListPtr list)
{
	PatternObjectPtr object;
	PATTERN_CREATE_STRUCT pcs;

	pcs.bound = *bound;
	pcs.pattern = pattern;
	pcs.color = COLOR_WHITE;
	pcs.bcolor = color;

/* Create the object. */

	if ((object = database->create_pattern_object(&pcs)) == NULL)
	{
		return database->last_creation_error();
	}
	object->set_panel(panel);
	object->set_select_flags(0);
	object->add_flags(OBJECT_FLAG_locked);

/* Add it to the list. */

	list->append(object);

	return ERRORCODE_None;
}

/*
// Add a box to the calendar.
*/

ERRORCODE near Calendar::add_box(PBOX_PTR bound, SHORT pattern, COLOR color, SHORT panel, PCOORD line_thickness, ObjectListPtr list)
{
	PBOX r, box = *bound;
	ERRORCODE error;

	r.x0 = box.x0;
	r.x1 = box.x1;
	r.y0 = box.y0;
	r.y1 = box.y0+line_thickness;

	if ((error = add_line(&r, pattern, color, panel, list)) != ERRORCODE_None)
	{
		return error;
	}

	r.y0 = box.y1-line_thickness;
	r.y1 = box.y1;

	if ((error = add_line(&r, pattern, color, panel, list)) != ERRORCODE_None)
	{
		return error;
	}

	r.x0 = box.x0;
	r.x1 = box.x0+line_thickness;
	r.y0 = box.y0;
	r.y1 = box.y1;

	if ((error = add_line(&r, pattern, color, panel, list)) != ERRORCODE_None)
	{
		return error;
	}

	r.x0 = box.x1-line_thickness;
	r.x1 = box.x1;

	return add_line(&r, pattern, color, panel, list);
}

ERRORCODE Calendar::add_frame(PBOX_PTR bound, LPSTR text, ALIGN_TYPE align, VERT_ALIGN_TYPE valign, SHORT panel, FLAGS flags, FLAGS sel_flags, ObjectListPtr list, DB_OBJECT_TYPE type, CTextStyle* style)
{
	FRAMEOBJ_CREATE_STRUCT fcs;
	CFrameObject* pObject;

/* Fill out the frame object creation structure. */

	fcs.bound = *bound;
//	fcs.text = text;
	fcs.alignment = align;
	fcs.vert_alignment = valign;
	fcs.flags = flags;
//	fcs.pStyle = style;

/* Create the object. */

	if ((pObject = (CFrameObject*)database->new_object(type, &fcs)) == NULL)
	{
		return database->last_creation_error();
	}
	pObject->set_panel(panel);
	pObject->set_select_flags(sel_flags);

/* Add it to the list. */

	list->append(pObject);

	CTxp Txp(database);

	/* Stick in some text. */
	Txp.Init(pObject, 0);
	Txp.SetHorizontalAlignment(align);

	if (text == NULL || *text == '\0')
	{
		// No text. Set the base paragraph style.
		pObject->SetTextStyle(style);
	}
	else
	{
		Txp.Style(*style);
		Txp.InsertString(text);
	}

//	object->PutText(text, align, valign, style, FALSE);

	return ERRORCODE_None;
}

/*
// Add a title to a calendar.
*/

ERRORCODE near Calendar::add_title(ObjectListPtr l, CTextStyle* pStyle)
{
	CHAR buffer[30];
	PBOX b;
	PageObjectPtr object;
	ERRORCODE error;

/* See if one already exists. */

	for (object = l->first_object(); object != NULL; object = object->next_object())
	{
		if (object->type() == OBJECT_TYPE_Frame
					&& ((PMGPageObjectPtr)object)->get_panel() == TITLE_PANEL)
		{
			return ERRORCODE_None;			/* Already exists. */
		}
	}

/* We need to create a new one. */

	b.x0 = bound.x0;
	b.x1 = bound.x1;

	
	// picture on top requires special calculation!
	/*if (GetPictureStyle() == CAL_PICTURE_TOP) 
	{
		b.y0 = full_bound.y0 - 1650; 
		b.y1 = full_bound.y0;
	}
	else // landscape w/ or w/o picture, or portrait w/o picture
	{
		b.y0 = PAGE_RESOLUTION / 4;
		b.y1 = full_bound.y0;//(bound.y1 - bound.y0); //full_bound.y0;
	}
*/
	b.y0 = full_bound.y0 - ((full_bound.y1 - full_bound.y0) / 9);
	b.y1 = full_bound.y0;

	SHORT size = (SHORT)scale_pcoord(b.y1 - b.y0, (72*3)/4, PAGE_RESOLUTION);
	CFixed lSize = MakeFixed(size);

	pStyle->BaseSize(lSize);
	pStyle->Size(lSize);

	pStyle->UpdateFontMetrics();

/* Build the name. */
#ifdef LOCALIZE
	sprintf(buffer, "%s %d",
			// JRO month_name[get_info().month],
         (LPCSTR) CurrentLocale.MonthName((MONTH)get_info().month),
			get_info().year);
#else
	sprintf(buffer, "%s %d",
			// JRO month_name[get_info().month],
         GET_PMWAPP()->GetResourceStringPointer (month_name_id[get_info().month]),
			get_info().year);
#endif

	if ((error = add_frame(&b,
							buffer,
							ALIGN_center,
							ALIGN_middle,
							TITLE_PANEL,
							FRAME_FLAG_fit_text,
							SELECT_FLAG_boundary | SELECT_FLAG_solo_select,
							l,
							OBJECT_TYPE_Frame,
							pStyle)) == ERRORCODE_None)
	{
	/* Get the object we just added. */

		PMGPageObjectPtr o = (PMGPageObjectPtr)l->last_object();

		if (o != NULL)				/* Be really sure... */
		{
		/* Single-click is edit. */
			o->set_primary_action(ACTION_DEFAULT_EDIT);
		/* Double-click is nothing. */
			o->set_secondary_action(ACTION_DO_NOTHING);
		}
	}
	return error;
}

/*
// Add a group to the current calendar if it doesn't already exist.
*/

GroupObjectPtr near Calendar::add_group(SHORT panel, FLAGS flags)
{
	GroupObjectPtr group;

	if ((group = (GroupObjectPtr)find_group(panel)) == NULL)
	{
		if ((group = ((PMGDatabasePtr)database)->create_group_object()) != NULL)
		{
			list->prepend(group);
			group->set_panel(panel);
			group->set_flags(group->get_flags() | flags);
		}
	}
	return group;
}

/*
// Build the groups used by a calendar.
*/

BOOL near Calendar::build_calendar_groups(GroupObjectPtr *skeleton_group,
								GroupObjectPtr *date_group,
								GroupObjectPtr *graphic_group,
								GroupObjectPtr *text_group)
{
	return	   (*date_group = add_group(DATE_PANEL, 0)) != NULL
				&& (*text_group = add_group(TEXT_PANEL, OBJECT_FLAG_locked)) != NULL
				&& (*graphic_group = add_group(GRAPHIC_PANEL, OBJECT_FLAG_locked)) != NULL
				&& (*skeleton_group = add_group(SKELETON_PANEL, 0)) != NULL;
}

/*
// Create all the objects for a weekly/generic calendar.
*/

ERRORCODE Calendar::build_weekly(CTextStyle* pTitleStyle)
{
	COLOR shadow_color = 0;
	SHORT shadow_pattern = 0;
	PBOX r;
	CHAR buffer[45];
//	WEEKDAY weekday;
	SHORT weekday;
	SHORT i, day;
	PCOORD label_width;
	BOOL stop_looking;
	GroupObjectPtr skeleton_group, date_group, text_group, graphic_group;
	ERRORCODE error;
	CTextStyle text_style(database);
	FontServerPtr font_server = database->get_font_server();

/* Compute shadow parameter. */

	if (has_shadow)
	{
	/* Set the shadow color. */

		switch (get_info().style.shadow_style)
		{
			case CAL_SHADOW_BLACK:
			case CAL_SHADOW_GRAY:
			case CAL_SHADOW_LIGHT_GRAY:
			{
				shadow_color = COLOR_BLACK;
				break;
			}
			case CAL_SHADOW_100:
			case CAL_SHADOW_50:
			case CAL_SHADOW_25:
			{
				shadow_color = get_info().style.color;
				break;
			}
			default:
			{
				return ERRORCODE_IntError;
			}
		}

	/* Set the shadow pattern. */

		switch (get_info().style.shadow_style)
		{
			case CAL_SHADOW_BLACK:
			case CAL_SHADOW_100:
			{
				shadow_pattern = 0;
				break;
			}
			case CAL_SHADOW_GRAY:
			case CAL_SHADOW_50:
			{
				shadow_pattern = 50;
				break;
			}
			case CAL_SHADOW_LIGHT_GRAY:
			case CAL_SHADOW_25:
			{
				shadow_pattern = 75;
				break;
			}
			default:
			{
				return ERRORCODE_IntError;
			}
		}
	}

/* Create the groups. */

	if (!build_calendar_groups(&skeleton_group, &date_group,
								&graphic_group, &text_group))
	{
		return ERRORCODE_Memory;
	}

/* Add a title to the calendar. */

	if ((error = add_title(skeleton_group->object_list(), pTitleStyle)) != ERRORCODE_None)
	{
		return error;
	}

	if (has_border)
	{
		if ((error = add_box(&bound, 0, get_info().style.color, SKELETON_PANEL, line_thickness, skeleton_group->object_list())) != ERRORCODE_None)
		{
			return error;
		}
	}

	switch (get_info().style.label_style)
	{
		case CAL_LABEL_TEXT:
		{
			break;
		}
		case CAL_LABEL_BOXED:
		{
			if ((error = add_box(&label, 0, get_info().style.color, LABEL_PANEL, line_thickness, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}
			break;
		}
		case CAL_LABEL_INVERTED:
		{
			if ((error = add_line(&label, 0, get_info().style.color, LABEL_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}
			break;
		}
		default:
		{
			break;
		}
	}

/* Draw the horizontal lines. */

	switch (get_info().style.line_style)
	{
		case CAL_LINES_ALL:
		case CAL_LINES_HORIZONTAL:
		{
		/* Draw horizontal lines. */

			r.x0 = bound.x0;
			r.x1 = bound.x1;

			if (get_info().style.label_style == CAL_LABEL_INVERTED)
			{
				r.x0 = label.x1;
			}

			r.y0 = bound.y0;

			for (i = 0; i <= days_in_week; i++)
			{
				r.y1 = r.y0 + line_thickness;
				if ((error = add_line(&r, 0, get_info().style.color, SKELETON_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
				{
					return error;
				}

				if (get_info().style.label_style == CAL_LABEL_INVERTED
						&& i != 0
						&& i != days_in_week)
				{
					PBOX r2;
					r2.y0 = r.y0;
					r2.y1 = r.y1;

					r2.x0 = label.x0;
 					if (has_border)
					{
						r2.x0 += line_thickness;
					}
					r2.x1 = label.x1;
					if ((error = add_line(&r2, 100, get_info().style.color, SKELETON_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
					{
						return error;
					}
				}
				r.y0 += cell_height;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	if (has_shadow)
	{
	/* Right piece. */

		r.x0 = bound.x1;
		r.x1 = r.x0+shadow_thickness;
		r.y0 = bound.y0+shadow_thickness;
		r.y1 = bound.y1+shadow_thickness;
		if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
		{
			return error;
		}

	/* Bottom piece. */

		r.x0 = bound.x0+shadow_thickness;
		r.x1 = bound.x1+shadow_thickness;
		r.y0 = bound.y1;
		r.y1 = r.y0+shadow_thickness;
		if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
		{
			return error;
		}
	}

/*
// Draw the calendar text.
*/

/* Draw the label text. */

	r.x0 = label.x0 + 2*line_thickness;
	r.y0 = label.y0 + 2*line_thickness;
	r.x1 = label.x1 + cell_width;
	r.y1 = label.y0 + cell_height;

	SHORT size= (SHORT)scale_pcoord(cell_height, 72/6, PAGE_RESOLUTION);

	m_LabelFontStyle.BaseSize(MakeFixed(size));
	m_LabelFontStyle.Size(MakeFixed(size));

/*
// Calculate the size of the text for these labels.
// If the text falls outside the frame, keep making it smaller.
*/

	label_width = label.x1 - r.x0
						- InchesToPage(m_LabelFontStyle.LeftMargin() + m_LabelFontStyle.RightMargin());

	for (stop_looking = FALSE; !stop_looking;)
	{
		BOOL too_big = FALSE;

		m_LabelFontStyle.UpdateFontMetrics();

		for (weekday = first_day, i = 0;
							i < days_in_week && !too_big;
	 						i++, weekday = next_weekday(weekday))
		{
//			CHAR *p;
			TCHAR *p;
			PCOORD size;

#ifdef LOCALIZE
			CurrentLocale.WeekDayName((WEEKDAY) weekday, buffer, sizeof(buffer));
#else
			// JRO strcpy(buffer, weekday_name[weekday]);
	         strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (weekday_name_id[weekday]));
#endif

			switch (get_info().style.weekday_style)
			{
				case WEEKDAY_STYLE_INITIALS:
				{
					buffer[1] = '\0';
					break;
				}
				case WEEKDAY_STYLE_ABBREV:
				{
#ifdef LOCALIZE
					CurrentLocale.AbbrevWeekDayName((WEEKDAY)weekday, buffer, sizeof(buffer));
#else
					buffer[3] = '\0';
#endif
					break;
				}
				default:
				{
					break;
				}
			}



		/* Compute the size for the text. */

			for (size = 0, p = buffer; *p != '\0'; )
			{
				size += font_server->GetAdvanceWidth(&m_LabelFontStyle, (CHARACTER)*p);
				p = CharNext(p);
			}

		/* See if we are too big. */

			if (size > label_width)
			{
				int nNewSize = (int)scale_pcoord(FixedInteger(m_LabelFontStyle.Size()), label_width, size);
				CFixed lNewSize = MakeFixed(nNewSize);
				m_LabelFontStyle.BaseSize(lNewSize);
				m_LabelFontStyle.Size(lNewSize);

				too_big = TRUE;

				if (nNewSize < MINIMUM_LABEL_POINT_SIZE)
				{
					m_LabelFontStyle.Size(MakeFixed(MINIMUM_LABEL_POINT_SIZE));
					m_LabelFontStyle.BaseSize(MakeFixed(MINIMUM_LABEL_POINT_SIZE));
					m_LabelFontStyle.UpdateFontMetrics();
					stop_looking = TRUE;
				}
			}
		}
		if (!too_big)
		{
		/* All done! */
			stop_looking = TRUE;
		}
	}

	m_LabelFontStyle.Alignment(ALIGN_left);

	text_style = m_LabelFontStyle;

	if (get_info().style.label_style == CAL_LABEL_INVERTED)
	{
		if (text_style.FillForegroundColor() == get_info().style.color
					&& text_style.FillPattern() == 0)
		{
			text_style.FillForegroundColor(COLOR_WHITE);
		}
	}
	else
	{
		if (text_style.FillForegroundColor() == COLOR_WHITE)
		{
			text_style.FillForegroundColor(COLOR_BLACK);
		}
	}

/*
// Run through the days now and add all the frames.
*/

	ObjectListPtr skeleton_list = skeleton_group->object_list();

	for (weekday = first_day, i = 0;
						i < days_in_week;
 						i++, weekday = next_weekday(weekday))
	{

#ifdef LOCALIZE
			CurrentLocale.WeekDayName((WEEKDAY)weekday, buffer, sizeof(buffer));
#else
			// JRO strcpy(buffer, weekday_name[weekday]);
	         strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (weekday_name_id[weekday]));

#endif

			switch (get_info().style.weekday_style)
			{
				case WEEKDAY_STYLE_INITIALS:
				{
					buffer[1] = '\0';
					break;
				}
				case WEEKDAY_STYLE_ABBREV:
				{
#ifdef LOCALIZE
					CurrentLocale.AbbrevWeekDayName((WEEKDAY)weekday, buffer, sizeof(buffer));
#else
					buffer[3] = '\0';
#endif
					break;
				}
				default:
				{
					break;
				}
			}


	/* Add the actual text. */

		if ((error = add_frame(&r,
 							buffer,
 							ALIGN_left,
 							ALIGN_top,
 							LABEL_FRAME_PANEL+i,
 							0,
							0,
 							skeleton_list,
							OBJECT_TYPE_Frame,
							&text_style)) != ERRORCODE_None)
		{
			return error;
		}

		PMGPageObjectPtr object = (PMGPageObjectPtr)skeleton_list->last_object();
		object->add_flags(OBJECT_FLAG_locked);

		r.y0 += cell_height;
		r.y1 += cell_height;
	}

/* Draw the numbers if this is a weekly calendar. */

	r.x0 = label.x0;
	r.y0 = label.y0;
	r.x1 = label.x1 + cell_width;
	r.y1 = label.y0 + cell_height;

	*buffer = '\0';					/* In case of GENERIC. */

	day = first_day_of_month;

	if (get_info().year == 1752 && get_info().month == SEPTEMBER)
	{
		if (day >= 3)
		{
			day += 14-3;
		}
	}

	m_NumberFontStyle.Alignment(ALIGN_center);

	for (i = 0; i < days_in_week; i++, day++)
	{
		short days_in_month = days_in_a_month(get_info().year, get_info().month);
		
		if (day > days_in_month)
			day = 1;

		if (get_info().style.type == WEEKLY)
		{
			itoa(day, buffer, 10);
		}

	/* Add the actual text. */

		if ((error = add_frame(&r,
							buffer,
							ALIGN_center,
							ALIGN_middle,
							i+DATE_PANEL,
							0,
							SELECT_FLAG_boundary | SELECT_FLAG_drag_handle | SELECT_FLAG_solo_select,
							date_group->object_list(),
							OBJECT_TYPE_CalendarDate,
							&m_NumberFontStyle)) != ERRORCODE_None)
		{
			return error;
		}

		r.y0 += cell_height;
		r.y1 += cell_height;

		if (get_info().year == 1752 && get_info().month == SEPTEMBER && day == 2)
		{
			day = 13;
		}
	}

	skeleton_group->establish();
	date_group->establish();
	text_group->establish();
	graphic_group->establish();

	return ERRORCODE_None;
}

/*
// Create all the objects for this calendar.
*/

ERRORCODE Calendar::build_monthly(CTextStyle* pTitleStyle)
{
	PBOX r;
	SHORT i;
	COLOR shadow_color = 0;
	SHORT shadow_pattern = 0;
	ALIGN_TYPE h_align;
	VERT_ALIGN_TYPE v_align;
	SHORT day, week;
//	WEEKDAY weekday;
	SHORT weekday;
	CHAR buffer[45];
	GroupObjectPtr skeleton_group, date_group, text_group, graphic_group;
	BOOL stop_looking;
	ERRORCODE error;
	CTextStyle text_style(database);
	FontServerPtr font_server = database->get_font_server();

/* Compute shadow parameter. */

	if (has_shadow)
	{
	/* Set the shadow color. */

		switch (get_info().style.shadow_style)
		{
			case CAL_SHADOW_BLACK:
			case CAL_SHADOW_GRAY:
			case CAL_SHADOW_LIGHT_GRAY:
			{
				shadow_color = COLOR_BLACK;
				break;
			}
			case CAL_SHADOW_100:
			case CAL_SHADOW_50:
			case CAL_SHADOW_25:
			{
				shadow_color = get_info().style.color;
				break;
			}
			default:
			{
				return ERRORCODE_IntError;
			}
		}

	/* Set the shadow pattern. */

		switch (get_info().style.shadow_style)
		{
			case CAL_SHADOW_BLACK:
			case CAL_SHADOW_100:
			{
				shadow_pattern = 0;
				break;
			}
			case CAL_SHADOW_GRAY:
			case CAL_SHADOW_50:
			{
				shadow_pattern = 50;
				break;
			}
			case CAL_SHADOW_LIGHT_GRAY:
			case CAL_SHADOW_25:
			{
				shadow_pattern = 75;
				break;
			}
			default:
			{
				return ERRORCODE_IntError;
			}
		}
	}

/* Create the groups. */

	if (!build_calendar_groups(&skeleton_group, &date_group,
								&graphic_group, &text_group))
	{
		return ERRORCODE_Memory;
	}

/* Add a default calendar title. */

	if ((error = add_title(skeleton_group->object_list(), pTitleStyle)) != ERRORCODE_None)
	{
		return error;
	}

/* Draw the label. */

	switch (get_info().style.label_style)
	{
		case CAL_LABEL_TEXT:
		{
			break;
		}
		case CAL_LABEL_BOXED:
		{
			if ((error = add_box(&label, 0, get_info().style.color, LABEL_PANEL, line_thickness, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}
			break;
		}
		case CAL_LABEL_INVERTED:
		{
			if ((error = add_line(&label, 0, get_info().style.color, LABEL_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}
			break;
		}
		default:
		{
			break;
		}
	}

/* Draw the calendar frame. */

	if (has_border)
	{
	/*
	// Draw calendar with outline.
	*/

	/* Draw the outline. */

		if ((error = add_box(&bound, 0, get_info().style.color, SKELETON_PANEL, line_thickness, skeleton_group->object_list())) != ERRORCODE_None)
		{
			return error;
		}

	/* Draw the horizontal lines. */

		switch (get_info().style.line_style)
		{
			case CAL_LINES_ALL:
			{
			/* Draw vertical lines. */

				r.y0 = bound.y0;
				r.y1 = bound.y1;
				r.x0 = bound.x0 + cell_width;
				for (weekday = (WEEKDAY)1; weekday < days_in_week; weekday++)
				{
					r.x1 = r.x0 + line_thickness;
					if ((error = add_line(&r, 0, get_info().style.color, SKELETON_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
					{
						return error;
					}
					r.x0 += cell_width;
				}

			/* Fall through to... */
			}
			case CAL_LINES_HORIZONTAL:
			{
			/* Draw horizontal lines. */

				r.x0 = bound.x0;
				r.x1 = bound.x1;
				r.y0 = bound.y0 + cell_height;
				for (week = 1; week < weeks_in_month; week++)
				{
					r.y1 = r.y0 + line_thickness;
					if ((error = add_line(&r, 0, get_info().style.color, SKELETON_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
					{
						return error;
					}
					r.y0 += cell_height;
				}
				break;
			}
			default:
			{
				break;
			}
		}

		if (has_shadow)
		{
		/* Right piece. */

			r.x0 = bound.x1;
			r.x1 = r.x0 + shadow_thickness;
			r.y0 = (has_label ? label.y0 : bound.y0)+shadow_thickness;
			r.y1 = bound.y1+shadow_thickness;
			if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}

		/* Bottom piece. */

			r.x0 = bound.x0+shadow_thickness;
			r.x1 = bound.x1+shadow_thickness;
			r.y0 = bound.y1;
			r.y1 = r.y0 + shadow_thickness;
			if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}
		}
	}
	else
	{
	/*
	// Draw calendar without outline.
	*/

	/* Draw the horizontal lines. */

		switch (get_info().style.line_style)
		{
			case CAL_LINES_ALL:
			{
			/* Draw vertical lines. */

				r.x0 = bound.x0;

				for (weekday = (WEEKDAY)0; weekday <= days_in_week; weekday++)
				{
					r.y0 = bound.y0;
					r.y1 = bound.y1;

					if (weekday < skewed_first_day)
					{
						r.y0 += cell_height;
					}
					if (weekday > skewed_last_day+1)
					{
						r.y1 -= cell_height;
					}

					r.x1 = r.x0 + line_thickness;
					if ((error = add_line(&r, 0, get_info().style.color, SKELETON_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
					{
						return error;
					}
					r.x0 += cell_width;
				}

			/* Fall through to... */
			}
			case CAL_LINES_HORIZONTAL:
			{
			/* Draw horizontal lines. */

				r.x0 = bound.x0;
				r.x1 = bound.x1;
				r.y0 = bound.y0;

				for (week = 0; week <= weeks_in_month; week++)
				{
					if (get_info().style.line_style == CAL_LINES_ALL)
					{
						r.x0 = bound.x0;
						r.x1 = bound.x1;

						if (week == 0)
						{
							r.x0 += skewed_first_day*cell_width;
						}
						if (week == weeks_in_month)
						{
							r.x1 -= (days_in_week-(skewed_last_day+1))*cell_width;
						}
					}

					r.y1 = r.y0 + line_thickness;
					if ((error = add_line(&r, 0, get_info().style.color, SKELETON_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
					{
						return error;
					}
					r.y0 += cell_height;
				}
				break;
			}
			default:
			{
				break;
			}
		}

		if (has_shadow)
		{
			BOOL full_shadow = skewed_last_day+1 == days_in_week;

		/* Right piece. */

			r.x0 = bound.x1;
			r.x1 = r.x0 + shadow_thickness;
			r.y0 = (has_label ? label.y0 : bound.y0)+shadow_thickness;
			r.y1 = bound.y1+shadow_thickness;
			if (!full_shadow)
			{
				r.y1 -= cell_height;
			}
			if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}

			if (!full_shadow)
			{
			/* Mid horizontal piece. */

				r.y0 = r.y1 - shadow_thickness;
				r.x0 = bound.x1 - (days_in_week-(skewed_last_day+1))*cell_width;
				if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
				{
					return error;
				}

			/* Mid vertical piece. */

				r.x1 = r.x0 + shadow_thickness;
				r.y1 = bound.y1 + shadow_thickness;
				if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
				{
					return error;
				}
			}

		/* Bottom piece. */

			r.x0 = bound.x0+shadow_thickness;
			r.y0 = bound.y1;
			if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
			{
				return error;
			}

		/* Draw the label shadow if it has a label. */

			if (has_label)
			{
			/* Bottom of label where there is no calendar. */

				r.y0 = label.y1;
				r.x0 = label.x0 + shadow_thickness;
				r.x1 = label.x1 + shadow_thickness;

				r.y1 = r.y0 + shadow_thickness;
				if ((error = add_line(&r, shadow_pattern, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
				{
					return error;
				}

			/* Little white gap in shadow on right. */

				r.x0 = label.x1;
				r.y0 = label.y1 + shadow_thickness;
				r.y1 = r.y0 + 3*shadow_thickness/2;
				/* 100 == white */
				if ((error = add_line(&r, 100, shadow_color, SHADOW_PANEL, skeleton_group->object_list())) != ERRORCODE_None)
				{
					return error;
				}
			}
		}
	}

/*
// Draw the calendar text.
*/

/* Draw the label text. */

	r.y0 = label.y0;
	r.x0 = label.x0;
	r.y1 = label.y1;

	m_LabelFontStyle.Size(MakeFixed(heading_size));
	m_LabelFontStyle.BaseSize(MakeFixed(heading_size));

/*
// Calculate the size of the text for these labels.
// If the text falls outside the frame, keep making it smaller.
*/

	for (stop_looking = FALSE; !stop_looking;)
	{
		BOOL too_big = FALSE;

		m_LabelFontStyle.UpdateFontMetrics();

		for (weekday = get_info().first_day_of_week, i = 0;
							i < days_in_week && !too_big;
	 						i++, weekday = next_weekday(weekday))
		{
			// CHAR *p;
			TCHAR *p;
			PCOORD size;

#ifdef LOCALIZE
			CurrentLocale.WeekDayName((WEEKDAY) weekday, buffer, sizeof(buffer));
#else
			// JRO strcpy(buffer, weekday_name[weekday]);
			strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (weekday_name_id[weekday]));
#endif


			switch (get_info().style.weekday_style)
			{
				case WEEKDAY_STYLE_INITIALS:
				{
					buffer[1] = '\0';
					break;
				}
				case WEEKDAY_STYLE_ABBREV:
				{
#ifdef LOCALIZE
					CurrentLocale.AbbrevWeekDayName((WEEKDAY)weekday, buffer, sizeof(buffer));
#else
					buffer[3] = '\0';
#endif
					break;
				}
				default:
				{
					break;
				}
			}

		/* Compute the size for the text. */

			for (size = 0, p = buffer; *p != '\0'; )
			{
				size += font_server->GetAdvanceWidth(&m_LabelFontStyle, (CHARACTER)*p);
				p = CharNext(p);
			}

		/* See if we are too big. */

			if (size > cell_width)
			{
				int nNewSize = (int)scale_pcoord(FixedInteger(m_LabelFontStyle.Size()), cell_width, size);
				CFixed lNewSize = MakeFixed(nNewSize);
				m_LabelFontStyle.Size(lNewSize);
				m_LabelFontStyle.BaseSize(lNewSize);
				too_big = TRUE;
				if (lNewSize < MINIMUM_LABEL_POINT_SIZE)
				{
					m_LabelFontStyle.Size(MakeFixed(MINIMUM_LABEL_POINT_SIZE));
					m_LabelFontStyle.BaseSize(MakeFixed(MINIMUM_LABEL_POINT_SIZE));
					m_LabelFontStyle.UpdateFontMetrics();

					stop_looking = TRUE;
				}
			}
		}
		if (!too_big)
		{
		/* All done! */
			stop_looking = TRUE;
		}
	}

	m_LabelFontStyle.Alignment(ALIGN_center);
	m_LabelFontStyle.UpdateFontMetrics();

	text_style = m_LabelFontStyle;

	if (get_info().style.label_style == CAL_LABEL_INVERTED)
	{
		if (text_style.FillForegroundColor() == get_info().style.color
					&& text_style.FillPattern() == 0)
		{
			text_style.FillForegroundColor(COLOR_WHITE);
		}
	}
	else
	{
		if (text_style.FillForegroundColor() == COLOR_WHITE)
		{
			text_style.FillForegroundColor(COLOR_BLACK);
		}
	}

	ObjectListPtr skeleton_list = skeleton_group->object_list();

	for (weekday = get_info().first_day_of_week, i = 0;
									i < days_in_week;
 									i++, weekday = next_weekday(weekday))
	{
#ifdef LOCALIZE
		CurrentLocale.WeekDayName((WEEKDAY)weekday, buffer, sizeof(buffer));
#else
		// JRO strcpy(buffer, weekday_name[weekday]);
      strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (weekday_name_id[weekday]));
#endif

		switch (get_info().style.weekday_style)
		{
			case WEEKDAY_STYLE_INITIALS:
			{
				buffer[1] = '\0';
				break;
			}
			case WEEKDAY_STYLE_ABBREV:
			{
#ifdef LOCALIZE
				CurrentLocale.AbbrevWeekDayName((WEEKDAY)weekday, buffer, sizeof(buffer));
#else
				buffer[3] = '\0';
#endif
				break;
			}
			default:
			{
				break;
			}
		}

	/* Add a frame for the next string. */

		r.x1 = r.x0 + cell_width;
		if ((error = add_frame(&r,
 							buffer,
 							ALIGN_center,
 							ALIGN_middle,
 							LABEL_FRAME_PANEL+i,
 							0,
							0,
 							skeleton_list,
							OBJECT_TYPE_Frame,
							&text_style)) != ERRORCODE_None)
		{
			return error;
		}
		PMGPageObjectPtr object = (PMGPageObjectPtr)skeleton_list->last_object();
		object->add_flags(OBJECT_FLAG_locked);
		r.x0 += cell_width;
	}

/* We want the point size to be 1/6 the height of a cell. */

	r.y0 = bound.y0;
	r.y1 = r.y0 + cell_height;
	r.x0 = bound.x0 + skewed_first_day*cell_width;
	r.x1 = r.x0 + cell_width;

	SHORT size;

	if (get_info().style.line_style == CAL_LINES_NONE)
	{
		h_align = ALIGN_center;
		v_align = ALIGN_middle;
	}
	else
	{
		h_align = ALIGN_right;
		v_align = ALIGN_top;
	}

	// font size needs to be larger to be legible on a yearly calendar
	short type = get_info().style.type;
	if (type == YEARLY)
	{
		PCOORD height = (bound.y1 - bound.y0 - line_thickness)/5;
		size = (SHORT)scale_pcoord(height, 72/4, PAGE_RESOLUTION);
	}
	else
		size = (SHORT)scale_pcoord(cell_height, 72/6, PAGE_RESOLUTION);

	m_NumberFontStyle.BaseSize(MakeFixed(size));
	m_NumberFontStyle.Size(MakeFixed(size));

	m_NumberFontStyle.Alignment(h_align);
	m_NumberFontStyle.UpdateFontMetrics();

	for (i = DATE_PANEL, weekday = skewed_first_day, day = first_day_of_month;
									i < last_day_of_month+DATE_PANEL;
									i++, day++)
	{
		itoa(day, buffer, 10);

		if ((error = add_frame(&r,
 					buffer,
 					h_align,
 					v_align,
 					i,
					0,
					SELECT_FLAG_boundary | SELECT_FLAG_drag_handle | SELECT_FLAG_solo_select,
 					date_group->object_list(),
					OBJECT_TYPE_CalendarDate,
					&m_NumberFontStyle)) != ERRORCODE_None)
		{
			return error;
		}

		if (++weekday == days_in_week)
		{
			weekday = (WEEKDAY)0;

#if 0
			r.x0 = bound.x0 + 1;
			r.x1 = r.x0 + cell_width - 2;
#else
			r.x0 = bound.x0;
			r.x1 = r.x0 + cell_width;
//			r.x0 += line_thickness + line_thickness;
#endif

			r.y0 += cell_height;
			r.y1 += cell_height;
			day += DAYS_IN_FULL_WEEK - days_in_week;
		}
		else
		{
			r.x0 += cell_width;
			r.x1 += cell_width;
		}

	/* If this is the unusual month, fix the day now. */

		if (get_info().year == 1752
					&& get_info().month == SEPTEMBER
					&& day == 2)
		{
			day = 13;
		}
	}
	skeleton_group->establish();
	date_group->establish();
	text_group->establish();
	graphic_group->establish();

//	printf("Objects created: %d\n", listlen(CurrentDocument->CurrentList));
	return ERRORCODE_None;
}

/*
// Build the calendar.
*/

ERRORCODE Calendar::build(BOOL full_build, CTextStyle* pTitleStyle)
{
	ERRORCODE error;

/*
// Make sure somebody set our object list and bound.
*/

	if (list == NULL || bound.x0 == bound.x1)
	{
		return ERRORCODE_NotInitialized;
	}

/*
// If this isn't a full build, just build the groups.
// So far, this is only used by old document importation.
*/

	if (!full_build)
	{
		GroupObjectPtr skeleton_group, date_group, text_group, graphic_group;

	/* Create the groups. */

		return build_calendar_groups(&skeleton_group, &date_group,
									&graphic_group, &text_group)
						? ERRORCODE_None
						: ERRORCODE_Memory;
	}

/*
// Make sure title style means something.
*/

	CTextStyle our_style(database);

	if (pTitleStyle == NULL)
	{
		our_style.SetDefault();
		pTitleStyle = &our_style;
	}

#if 0
	if (REGION_IS_DRAWN(display_window))
	{
		begin_notify(IDS_BUILDING_CALENDAR);
	}
#endif

	switch (get_info().style.type)
	{
		case MONTHLY:
		case YEARLY:
		{
			error = build_monthly(pTitleStyle);
			break;
		}
		case WEEKLY:
		case GENERIC:
		{
			error = build_weekly(pTitleStyle);
			break;
		}
		default:
		{
			error = ERRORCODE_IllegalType;
		}
	}
#if 0
	if (REGION_IS_DRAWN(display_window))
	{
		end_notify();
	}
#endif
	return error;
}

/*
// Do the actual building of a calendar.
*/

ERRORCODE Calendar::calc_and_build(CTextStyle* pTitleStyle)
{
/* In case the frame was stretched. */

	pTitleStyle->Size(pTitleStyle->BaseSize());
	pTitleStyle->Expansion(MakeFixed(1));
	pTitleStyle->UpdateFontMetrics();

/* Construct the calendar objects. */

	calculate();
	return build(TRUE, pTitleStyle);
}

/*
// Rebuild the dates for a panel.
*/

VOID Calendar::rebuild_dates(GroupObjectPtr group)
{
	PMGPageObjectPtr object;
	CalendarDateObjectPtr dateobj;

	if (group == NULL)
	{
		return;
	}

	PBOX default_bound;

	default_bound.x0 =
		default_bound.y0 = 0;
	default_bound.x1 =
		default_bound.y1 = PAGE_RESOLUTION;

	for (object = (PMGPageObjectPtr)group->object_list()->first_object();
					object != NULL;
 					object = (PMGPageObjectPtr)object->next_object())
	{
		SHORT panel = object->get_panel();

		switch (object->type())
		{
			case OBJECT_TYPE_Graphic:
			{
				panel -= GRAPHIC_PANEL;

				if ((dateobj = get_date_object(panel)) == NULL)
				{
					object->add_flags(OBJECT_FLAG_hidden);
					((GraphicObjectPtr)object)->set_bound(default_bound);
				}
				else
				{
					object->remove_flags(OBJECT_FLAG_hidden);
					dateobj->layout_enhanced_graphic((GraphicObjectPtr)object, *this);
				}
				break;
			}
			case OBJECT_TYPE_Frame:
			{
				panel -= TEXT_PANEL;

				if ((dateobj = get_date_object(panel)) == NULL)
				{
					object->add_flags(OBJECT_FLAG_hidden);
					((CFrameObject*)object)->set_bound(default_bound);
				}
				else
				{
					object->remove_flags(OBJECT_FLAG_hidden);
					dateobj->layout_enhanced_frame((CFrameObject*)object, *this);
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

/*
// Delete the skeleton group.
// The title may be preserved in this case.
*/

VOID Calendar::delete_skeleton_group(BOOL reset_title, CTextStyle* pStyle)
{
	GroupObjectPtr group;

	if ((group = find_group(SKELETON_PANEL)) != NULL)
	{
		ObjectListPtr list = group->object_list();

		PMGPageObjectPtr object;

	/* Make sure we're refreshed. */

		group->get_database()->refresh_object(group);

	/* Detach the title if so requested. */

		if ((object = find_object(SKELETON_PANEL, TITLE_PANEL, OBJECT_TYPE_Frame)) != NULL)
		{
			if (reset_title)
			{
			/* Get the style to rebuild the title with. */
				CTxp Txp(NULL);
				Txp.Init((CFrameObject*)object, 0);
				*pStyle = Txp.Style();
			}
			else
			{
				list->detach(object);
			}
		}

	/* Delete all objects. */

		group->destroy_objects();

	/* Reattach the title if we found it. */

		if (!reset_title && object != NULL)
		{
			list->prepend(object);
		}
	}
}

/*
// Rebuild the calendar.
*/

VOID Calendar::rebuild(BOOL reset_title)
{
	CTextStyle Style(database);

/* Extract the current styles. */

//	get_frame_style(&m_LabelFontStyle, SKELETON_PANEL, LABEL_FRAME_PANEL);
//	get_frame_style(&m_NumberFontStyle, DATE_PANEL, DATE_PANEL);

/* Delete what we rebuild. */

	Style.SetDefault();

	delete_skeleton_group(reset_title, &Style);
	delete_group(DATE_PANEL);

/* Do the construction. */

	calc_and_build(&Style);

/* Relayout the enhanced dates. */

	rebuild_dates(find_group(TEXT_PANEL));
	rebuild_dates(find_group(GRAPHIC_PANEL));
}

/*
// Return the name of a month.
*/

CHAR *Calendar::get_month_name(MONTH month)
{
	if (month < JANUARY || month > DECEMBER)
	{
		return NULL;
	}

#ifdef LOCALIZE
	static TCHAR szMonthName[MAX_MONTHNAME];
	CurrentLocale.MonthName(month, szMonthName, sizeof(szMonthName));
	return szMonthName;
#else
	// JRO return ::month_name[month];
   return (CHAR *)GET_PMWAPP()->GetResourceStringPointer (::month_name_id[month]);
#endif
}

/*
// Return the name of a weekday.
*/

CHAR *Calendar::get_weekday_name(WEEKDAY weekday)
{
	if (weekday < SUNDAY || weekday > SATURDAY)
	{
		return NULL;
	}

#ifdef LOCALIZE
	static TCHAR szWeekDayName[MAX_WEEKDAYNAME];
	CurrentLocale.WeekDayName((WEEKDAY)weekday, szWeekDayName, sizeof(szWeekDayName));
	return szWeekDayName;
#else
	// JRO return ::weekday_name[weekday];
   return (CHAR *)GET_PMWAPP()->GetResourceStringPointer (::weekday_name_id[weekday]);
#endif
}
