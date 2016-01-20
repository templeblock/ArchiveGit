/*
// $Header: /PM8/App/STYLEIT.CPP 1     3/03/99 6:12p Gbeddow $
//
// Style iterator routines.
//
// $Log: /PM8/App/STYLEIT.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:48:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:24   FRED
// Initial revision.
// 
//    Rev 2.1   22 Sep 1995 09:41:32   JAY
// 32-bit change of Change structure.
// 
//    Rev 2.0   07 Feb 1995 15:48:10   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:33:54   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   05 Jan 1994 08:08:06   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:26   JAY
//
*/

#include "stdafx.h"
#include "pagedefs.h"
#include "style.h"
#include "pararec.h"

#include "styleit.h"

/*
// The constructor for the style iterator.
*/

StyleIterator::StyleIterator(ParagraphPtr p) : paragraph(NULL), style(NULL)
{
	if (p != NULL)
	{
		init(p);
	}
}

/*
// Init a style iterator.
*/

VOID StyleIterator::init(ParagraphPtr p)
{
	paragraph = p;

	c_index = 1;					/* Some number > 0 */
	compute(0);						/* Compute position 0 style. */
}

/*
// Grab a style change at the current style point.
*/

SHORT StyleIterator::grab_style_change(BOOL fix_style)
{
/* Just apply the change to the current style. */
	return style.apply_change(scp, fix_style);
}

/*
// Advance a style iterator.
*/

BOOL StyleIterator::sip_advance(VOID)
{
	c_index = scp->c_index;
	if (++s_index == paragraph->number_of_changes())
	{
		scp = NULL;
		return FALSE;
	}
	scp++;
	return TRUE;
}

/*
// Compute the style at a certain point in the text.
// A style iterator which has been initialized must be passed.
*/

VOID StyleIterator::compute(C_INDEX c_where)
{
	BOOL grabbed = FALSE;
/*
// See if we went the wrong way.
*/
	if (c_where < c_index)
	{
	/* Backing up. Must start over from the front. Restart the iterator. */

		c_index = -1;
		style = paragraph->get_style();

		if (paragraph->number_of_changes() == 0)
		{
		/* No style changes! Not much to do after this. */
			scp = NULL;
		}
		else
		{
		/* Start at the front of the changes. */
			scp = paragraph->get_style_change(0);
			s_index = 0;
		}
	}

/* If there are no more style changes, the style in sip is correct. */

	if (scp == NULL)
	{
	/* No more style changes at this point. */
		return;
	}

/*
// At this point s_index should always be less than change.count.
*/

	while (c_where >= scp->c_index)
	{
	/* Grab the style change. */

		grab_style_change();
		grabbed = TRUE;

	/* Advance to the next style change. */

		if (!sip_advance())
		{
			break;
		}
	}

	if (grabbed)
	{
		style.update_font_metrics();
	}
}

/*
// compute_style_range()
//
//		Compute a "composite" style for a range of characters.  A composite
// style is one which has a defined value for for each field which is the
// same for all characters in a range, and undefined for all fields which
// change within the range.
//
// Arguments:
//		sip			Pointer to INITIALIZED style iterator for this paragraph.
//		c_start		First character (inclusive) in range to get style for.
//		c_end			Last character (inclusive).
//		style_result		STYLE structure to hold resulting composite style
// Returns:
//		Nothing
*/

VOID StyleIterator::compute_style_range(C_INDEX c_start, C_INDEX c_end, TextStylePtr style_result)
{
	/* Get start style */
	compute(c_start);
	*style_result = style;

	/* For each change, clear the appropriate field */
	while (scp != NULL && scp->c_index <= c_end)
	{
		style_result->make_undefined((TEXT_TOKEN)scp->token);
		sip_advance();
	}

	/* We have invalidated the sip; restart it */
	compute(0);
}

