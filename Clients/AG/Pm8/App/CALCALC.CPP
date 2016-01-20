/*
// CALCALC.CPP
//
// Calendar calculation routines.
//
// $Header: /PM8/App/CALCALC.CPP 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/CALCALC.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 6     12/01/97 8:46a Jstoner
// changed yearly calendar default style settings
// 
// 5     11/13/97 2:57p Jstoner
// fixed weekly calendars to span months
// 
// 4     11/12/97 1:57p Jstoner
// updates for multipage weekly calendars, weekly calendar wizard
// 
// 3     11/10/97 3:50p Jstoner
// fixed some problems regarding adding yearly calendars
// 
// 2     11/07/97 2:22p Jstoner
// year calendar updates...good stopping/saving point
// 
//    Rev 1.0   14 Aug 1997 15:18:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:20   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 09:06:16   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:46:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:38   FRED
// Initial revision.
// 
//    Rev 2.1   15 Sep 1995 14:04:16   JAY
// More 32-bit changes.
// 
//    Rev 2.0   07 Feb 1995 15:47:32   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:33:52   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   05 Jan 1994 08:06:46   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:18   JAY
// 
*/

#include "stdafx.h"

#include "pagedefs.h"
#include "style.h"
#include "utils.h"

#include "pmgobj.h"
#include "calendar.h"

/*
// The day of the week of the first day in the specified century.
*/

PRIVATE WEEKDAY
year_start[] = 
{
	THURSDAY,											/*    0 */
	WEDNESDAY,											/*  100 */
	TUESDAY,												/*  200 */
	MONDAY,												/*  300 */
	SUNDAY,												/*  400 */
	SATURDAY,											/*  500 */
	FRIDAY,												/*  600 */
	THURSDAY,											/*  700 */
	WEDNESDAY,											/*  800 */
	TUESDAY,												/*  900 */
	MONDAY,												/* 1000 */
	SUNDAY,												/* 1100 */
	SATURDAY,											/* 1200 */
	FRIDAY,												/* 1300 */
	THURSDAY,											/* 1400 */
	WEDNESDAY,											/* 1500 */
	TUESDAY,												/* 1600 */
	MONDAY,												/* 1700 */
	WEDNESDAY,											/* 1800 */
	MONDAY,												/* 1900 */
	SATURDAY,											/* 2000 */
	FRIDAY,												/* 2100 */
	WEDNESDAY,											/* 2200 */
	MONDAY,												/* 2300 */
	SATURDAY,											/* 2400 */
	FRIDAY,												/* 2500 */
	WEDNESDAY,											/* 2600 */
	MONDAY,												/* 2700 */
	SATURDAY,											/* 2800 */
	FRIDAY												/* 2900 */
};

/*
// How many days there are in the months of a normal year.
*/

PRIVATE CHAR
month_days[] =
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/*
// Test whether a year is a leap year or not.
*/

PRIVATE BOOL near
is_leap_year(SHORT year)
{
	SHORT century;

	century = year / 100;
	year = year % 100;

	if (year == 0)
	{
	/*
	// Century mark.
	// All centuries before the 18th century ARE LEAP.
 	*/
		return (century < 18) ? TRUE : (century & 3) == 0;
	}
	else
	{
	/*
	// Non-century mark.
	// Year is LEAP if year is a multiple of 4.
	*/
		return (year & 3) == 0;
	}
}

/*
// Return the number of days to advance in a week from year to year.
*/

PRIVATE SHORT near
year_day_offset(SHORT year)
{
	if (year == 1752)
	{
	/* 1752 was a weird year. */
		return 5;
	}
	else
	{
	/* Normal year (as years go). Difference is determined by leap-ness */
		return is_leap_year(year) ? 2 : 1;
	}
}

/*
// Return the number of days in a month in a particular year.
*/

PUBLIC SHORT
days_in_a_month(SHORT year, SHORT month)
{
	SHORT days;

	if (month == SEPTEMBER && year == 1752)
	{
	/* September 1752 had only 19 days in it. I don't know why... */
		return 19;
	}

	days = month_days[month];

	if (month == FEBRUARY && is_leap_year(year))
	{
	/* Leap through February. */
		return days+1;
	}

	return days;
}

/*
// Get the first day of the year.
*/

PRIVATE WEEKDAY near
first_day_of_year(SHORT year)
{
	SHORT century, y;
	SHORT weekday;

/* Compute the "century" for this year. */

	century = year / 100;

/* Compute the day of the week for the first day in this century. */

	weekday = year_start[century];

/* Compute the year corresponding to the first year in this century. */

	y = century * 100;

	while (y != year)
	{
		weekday += year_day_offset(y++);
	}
	return (WEEKDAY)(weekday % DAYS_IN_FULL_WEEK);
}

PUBLIC WEEKDAY
first_day_of_a_month(SHORT year, MONTH month)
{
	SHORT weekday;
	SHORT m;

	weekday = first_day_of_year(year);
	m = JANUARY;

	while (m != month)
	{
		weekday += days_in_a_month(year, (MONTH)m++);
	}

	return (WEEKDAY)(weekday % DAYS_IN_FULL_WEEK);
}

/*
// Skew a weekday to account for a new first day of week.
// Clip it into the (possibly shortened) week.
*/

PRIVATE WEEKDAY near
skew_weekday(WEEKDAY weekday, SHORT first_day_of_week)
{
	SHORT wd = weekday - first_day_of_week;

	while (wd < 0)
	{
		wd += DAYS_IN_FULL_WEEK;
	}
	return (WEEKDAY)wd;
}

/*
// Calculate the parameters for a monthly calendar.
*/


VOID Calendar::calculate_monthly_calendar(short type)
{
	SHORT i, x_cells, y_cells;
	PCOORD gap;

/* Preserve the styles. */

//	label_font_style = params->label_font_style;
//	number_font_style = params->number_font_style;

	has_border = get_info().style.outline_style != CAL_OUTLINE_NONE;
	has_shadow = get_info().style.shadow_style != CAL_SHADOW_NONE
				&& (has_border || get_info().style.line_style == CAL_LINES_ALL);
	has_label = get_info().style.label_style != CAL_LABEL_TEXT;

/* Calculate number of days in this month. */

	days_in_month =
		days_in_a_month(get_info().year, get_info().month);

	first_day =
			first_day_of_a_month(get_info().year, (MONTH)get_info().month);

	first_day_of_month = 1;
	last_day_of_month = days_in_month;

	i = days_in_month
							+ first_day
							+ DAYS_IN_FULL_WEEK-1;

	last_day = (WEEKDAY)(i % DAYS_IN_FULL_WEEK);

/* Calculate the number of day in the week of the month we will be creating. */

	days_in_week = get_info().last_day_of_week + 1 - get_info().first_day_of_week;
	while (days_in_week <= 0)
	{
		days_in_week += DAYS_IN_FULL_WEEK;
	}

/* Create skewed versions of the first and last days to simplify later. */

	skewed_first_day = skew_weekday(first_day, get_info().first_day_of_week);
	skewed_last_day = skew_weekday(last_day, get_info().first_day_of_week);

	if (skewed_first_day >= days_in_week)
	{
		first_day_of_month += DAYS_IN_FULL_WEEK - skewed_first_day;
		skewed_first_day = (WEEKDAY)0;
	}
	if (skewed_last_day >= days_in_week)
	{
		last_day_of_month -= skewed_last_day - (days_in_week-1);
		skewed_last_day = (WEEKDAY)(days_in_week-1);
	}

/* Adjust the number of weeks in the month. */

	weeks_in_month = (last_day_of_month + 1
										- first_day_of_month
										+ skewed_first_day
										+ DAYS_IN_FULL_WEEK-1)/DAYS_IN_FULL_WEEK;

	days_in_month = weeks_in_month*days_in_week	
										 - skewed_first_day
										 - (days_in_week-1 - skewed_last_day);

/* Compute some thicknesses. */

	if (type == MONTHLY)
	{
		line_thickness = LINE_THICKNESS;	/* 1 pt. */
		shadow_thickness = LINE_THICKNESS*8;	/* 8 pt. */
	}
	else // yearly calendar
	{
		line_thickness = LINE_THICKNESS / 2;	/* 1/2 pt. */
		shadow_thickness = LINE_THICKNESS * 4;	/* 4 pt. */
	}

/* Compute the number of cells we will have in each direction. */

	x_cells = days_in_week;
	y_cells = weeks_in_month;

/* Compute the full bound that everything will fit in (label and frame). */

	gap = (calendar_bound.y1-calendar_bound.y0)/9;
	full_bound.y0 = calendar_bound.y0 + gap;
	full_bound.y1 = calendar_bound.y1 - /*gap*/ PAGE_RESOLUTION/4;

	gap = (PCOORD)(PAGE_RESOLUTION/4);
	full_bound.x0 = calendar_bound.x0 + gap;
	full_bound.x1 = calendar_bound.x1 - gap;

/* Compute initial frame bounds from the full bound. */

	bound.x1 = full_bound.x1;
	bound.y1 = full_bound.y1;

	if (has_shadow)
	{
		bound.x1 -= shadow_thickness;
		bound.y1 -= shadow_thickness;
	}

	label.x0 =
		bound.x0 = full_bound.x0;
	label.y0 = full_bound.y0;

	bound.y0 = label.y0 + 2*line_thickness;

/* Compute the cell size from this preliminary bound. */

	cell_width = (bound.x1 - bound.x0 - line_thickness)/x_cells;
	cell_height = (bound.y1 - bound.y0 - line_thickness)/y_cells;

/* Compute the header text size from this cell size. */
if (type == MONTHLY)
	{
		heading_gap = cell_height/6;
		heading_size = (SHORT)scale_pcoord(heading_gap, (72*3)/4, PAGE_RESOLUTION);
	}
	else // yearly calendar
	{
		heading_gap = (bound.y1-bound.y0)/18;
		heading_size = (SHORT)scale_pcoord(heading_gap, (72*3)/4, PAGE_RESOLUTION);
	}



/* Recompute the bounds now. */

	label.y1 =
		(bound.y0 += heading_gap);

/* Account for the label. */

	if (has_label)
	{
		if (has_border)
		{
			label.y1 += line_thickness;
		}
		else
		{
		/* Move down for the shadow around the label. */
			bound.y0 += 3*shadow_thickness/2;
		}
	}

/* Recompute cell height with these new bounds. */

	cell_height = (bound.y1 - bound.y0 - line_thickness)/y_cells;

/* Recompute the bounds from cell size to be exact. */

	bound.x1 = bound.x0 + x_cells*cell_width + line_thickness;
	bound.y1 = bound.y0 + y_cells*cell_height + line_thickness;
	label.x1 = bound.x1;

/* Recompute the cell height now if we use MAX_WEEKS. */

#if 0
	if (calendar_config.use_max_weeks)
	{
		cell_height = (bound.y1 - bound.y0)/MAX_WEEKS;

		if (weeks_in_month != MAX_WEEKS)
		{
			gap = (cell_height * (MAX_WEEKS-weeks_in_month))/2;
			bound.y0 += gap;
			bound.y1 -= gap;
		}
	}
#endif

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
				break;
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
				break;
			}
		}
	}
}

/*
// Calculate the parameters for a weekly calendar.
*/

VOID Calendar::calculate_weekly_calendar(VOID)
{
	SHORT i, y_cells;
	PCOORD gap;
	SHORT first, last;

/* Preserve the styles. */

//	label_font_style = params->label_font_style;
//	number_font_style = params->number_font_style;

	has_border = get_info().style.outline_style != CAL_OUTLINE_NONE
				|| get_info().style.line_style == CAL_LINES_ALL;
	has_shadow = get_info().style.shadow_style != CAL_SHADOW_NONE
				&& has_border;
	has_label = get_info().style.label_style != CAL_LABEL_TEXT;

/* Calculate number of days in this month. */

	days_in_month =
		days_in_a_month(get_info().year, get_info().month);

	first_day =
			first_day_of_a_month(get_info().year, (MONTH)get_info().month);

	first_day_of_month = 1;
	last_day_of_month = days_in_month;

	i = days_in_month
							+ first_day
							+ DAYS_IN_FULL_WEEK-1;

	last_day = (WEEKDAY)(i % DAYS_IN_FULL_WEEK);

/* Create skewed versions of the first and last days to simplify later. */

	skewed_first_day = skew_weekday(first_day, get_info().first_day_of_week);
	skewed_last_day = skew_weekday(last_day, get_info().first_day_of_week);

/* Calculate the number of day in the week of the month we will be creating. */

	days_in_week = get_info().last_day_of_week + 1 - get_info().first_day_of_week;

	if (skewed_first_day >= days_in_week)
	{
		first_day_of_month += DAYS_IN_FULL_WEEK - skewed_first_day;
		skewed_first_day = (WEEKDAY)0;
	}
	if (skewed_last_day >= days_in_week)
	{
		last_day_of_month -= skewed_last_day - (days_in_week-1);
		skewed_last_day = (WEEKDAY)(days_in_week-1);
	}

	while (days_in_week <= 0)
	{
		days_in_week += DAYS_IN_FULL_WEEK;
	}

/* Compute the first and last day of this week. */

	if (get_info().style.type == GENERIC)
	{
		first = 1;
		last = days_in_week;
		first_day = (WEEKDAY)get_info().first_day_of_week;
		last_day = (WEEKDAY)get_info().last_day_of_week;
	}
	else
	{
	/* Compute first and last day from the week number. */
		first = first_day_of_month
							- skewed_first_day
							+ get_info().week*DAYS_IN_FULL_WEEK;
		last = first + days_in_week - 1;

		if (first < first_day_of_month)
		{
			first = first_day_of_month;
		}
		else
		{
		/* Further weeks always start at the front. */
			first_day = (WEEKDAY)get_info().first_day_of_week;
		}

		if (last > last_day_of_month)
		{
			last = last_day_of_month;
		}
		else
		{
			last_day = (WEEKDAY)get_info().last_day_of_week;
		}

		days_in_week = 7;//last - first + 1;
	}

	first_day_of_month = first;
	last_day_of_month = last;

	days_in_month = days_in_week;

/* Compute some thicknesses. */

	line_thickness = LINE_THICKNESS;	/* 1 pt. */
	shadow_thickness = LINE_THICKNESS*10;	/* 10 pt. */

/* Compute the number of cells we will have in each direction. */

	y_cells = days_in_week;

/* Compute the full bound that everything will fit in (label and frame). */

	gap = (calendar_bound.y1-calendar_bound.y0)/9;
	full_bound.y0 = calendar_bound.y0 + gap;
	full_bound.y1 = calendar_bound.y1 - /*gap*/ PAGE_RESOLUTION/4;

	gap = (PCOORD)(PAGE_RESOLUTION/4);
	full_bound.x0 = calendar_bound.x0 + gap;
	full_bound.x1 = calendar_bound.x1 - gap;

/* Compute initial frame bounds from the full bound. */

	bound.x1 = full_bound.x1;
	bound.y1 = full_bound.y1;

	if (has_shadow)
	{
		bound.x1 -= shadow_thickness;
		bound.y1 -= shadow_thickness;
	}

	bound.x0 = full_bound.x0;
	bound.y0 = full_bound.y0;

/* Compute the cell size from this preliminary bound. */

	cell_height = (bound.y1 - bound.y0 - line_thickness)/y_cells;

/* Recompute the bounds from cell size to be exact. */

	bound.y1 = bound.y0 + y_cells*cell_height + line_thickness;

	label.x0 = bound.x0;
	label.y0 = bound.y0;
	label.x1 = bound.x0 + (bound.x1-bound.x0)/4;
	label.y1 = bound.y1;

	cell_width = bound.x1 - label.x1 - line_thickness;

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
				break;
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
				break;
			}
		}
	}
}

/*
// Calculate the parameters for the current calendar.
*/

VOID Calendar::calculate(VOID)
{
	switch (get_info().style.type)
	{
		// ADD YEARLYHERE
		case YEARLY:
		case MONTHLY:
		{
			calculate_monthly_calendar(get_info().style.type);
			break;
		}
		case WEEKLY:
		case GENERIC:
		{
			calculate_weekly_calendar();
			break;
		}
		default:
		{
			break;
		}
	}
}

