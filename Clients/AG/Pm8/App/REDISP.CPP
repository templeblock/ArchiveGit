/*
// $Header: /PM8/App/REDISP.CPP 1     3/03/99 6:11p Gbeddow $
//
// Redisplay context routines.
//
// $Log: /PM8/App/REDISP.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 5     1/25/99 3:43p Rgrenfell
// Fixed a possible divide by zero problem.
// 
// 4     7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
// 3     2/27/98 2:47p Jayn
// "Draw as bitmaps"
// 
// 2     11/21/97 4:02p Hforman
// set  destination_hdc = NULL in constructor
// 
//    Rev 1.0   14 Aug 1997 15:25:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:16   Fred
// Initial revision.
// 
//    Rev 1.10   11 Jun 1997 17:28:24   Fred
// Bitmap drawing optimizations
// 
//    Rev 1.9   05 Jun 1997 16:47:46   Jay
// New PostScript type check and handling
// 
//    Rev 1.8   26 Mar 1997 14:52:02   Jay
// Interruptable preview
// 
//    Rev 1.7   06 Mar 1997 17:28:08   Jay
// Print preview stuff
// 
//    Rev 1.6   19 Feb 1997 16:53:22   Jay
// Snapping
// 
//    Rev 1.5   01 Nov 1996 17:11:48   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.4   28 Oct 1996 13:46:28   Jay
// Changes from 3.01 code
// 
//    Rev 1.4   23 Oct 1996 00:27:22   Jay
// Can override the postscript setting in the redisplay context.
// 
//    Rev 1.3   15 Oct 1996 09:15:06   Jay
// Ugh!!!
// 
//    Rev 1.2   15 Oct 1996 08:54:34   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.1   08 Jul 1996 08:49:08   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.0   14 Mar 1996 13:45:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:54   FRED
// Initial revision.
// 
//    Rev 2.5   07 Sep 1995 15:48:30   JAY
// Special code to skip the toggle for off-screen generation.
// 
//    Rev 2.4   20 Jul 1995 12:47:30   JAY
// Got rid of some unused code.
// 
//    Rev 2.3   30 Jun 1995 16:33:06   JAY
// New CPaperInfo class.
// 
//    Rev 2.2   10 May 1995 13:13:14   JAY
// Now has ExpandIfEmpty parameter.
// 
//    Rev 2.1   15 Feb 1995 17:01:44   JAY
// Added an is_metafile member.
// 
//    Rev 2.0   07 Feb 1995 15:48:06   JAY
// Initial revision.
// 
//    Rev 1.22   30 Jan 1995 09:15:10   JAY
// Added an attribute DC (to support metafile case).
// 
//    Rev 1.21   30 Nov 1994 16:34:14   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.20   04 Aug 1994 07:33:16   JAY
// Large model and 32-bit changes
// 
//    Rev 1.19   13 Jul 1994 11:26:14   JAY
// update name code now takes an LPCSTR.
// 
//    Rev 1.18   16 May 1994 08:41:28   JAY
// Broke the gamma_curve into two separate curves, one for bitmaps and one for
// outlines.
// 
//    Rev 1.17   13 Apr 1994 17:15:52   JAY
// New code and variable to handle PostScript printer types.
// Added a PS type 2 (QMS color v 2.xx) to handle that printer.
// 
//    Rev 1.16   07 Apr 1994 10:45:36   JAY
// Added m_use_ddbs field.
// Fixed check in get_info (type).
// 
//    Rev 1.15   16 Mar 1994 16:49:20   JAY
// Improved detection of 24 bpp devices.
// 
//    Rev 1.14   15 Mar 1994 13:22:30   JAY
// Broke draw_page() out of erase_area() so it can be called elsewhere.
// 
//    Rev 1.13   10 Mar 1994 09:54:20   JAY
// Does not do (unnecessary) WHITENESS erase if printing.
// 
//    Rev 1.12   02 Mar 1994 11:25:26   JAY
// "is_postscript == 2" is no longer valid. The boolean nature of the variable
// has been restored.
// 
//    Rev 1.11   01 Mar 1994 15:51:52   JAY
// New support code (and detection) for HP/GL driver (HP 4/4M).
// 
//    Rev 1.10   22 Feb 1994 15:01:16   JAY
// Added new field which tells whether the redisplay device is PostScript or not.
// 
//    Rev 1.9   17 Feb 1994 11:48:56   MACDUFF
// Uses 32-bit version of page_x_to_screen (also for page_y...).
// 
//    Rev 1.8   17 Feb 1994 08:41:00   MACDUFF
// Added SetScaling() method.  pbox_to_screen() will use 32-bit
// code if possible.
// 
//    Rev 1.7   11 Feb 1994 15:58:02   MACDUFF
// Shortcut: skip toggle(on) if the toggle(off) didn't do anything.
// 
//    Rev 1.6   01 Feb 1994 17:44:00   JAY
// Now round down and up in screen_to_pbox (or whataver it's called).
// 
//    Rev 1.5   31 Jan 1994 07:14:54   JAY
// Removed od('s for release version.
// 
//    Rev 1.4   28 Jan 1994 17:10:56   JAY
//  
// 
//    Rev 1.3   25 Jan 1994 15:18:10   JAY
// More
// 
//    Rev 1.2   19 Jan 1994 17:06:40   JAY
//  
// 
//    Rev 1.1   13 Jan 1994 17:00:18   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:25:00   JAY
// 
*/

#include "stdafx.h"

#include <math.h>

#include "redisp.h"
#include "utils.h"

#ifndef _WIN32
#include "db386.h"
#endif

#define REDISP_NEW_STYLEx
#define MAX_REDISP_BITS	31

/*
// Transform a PBOX from page coordinates to output device coordinates.
*/

/* Remember the font bug - rounding must always go the same way! */
#define	CORRECT_MULDIV(x, n, d)		{ \
										(x) *= (PCOORD)(n); \
 										(x) /= (PCOORD)(d); \
										if ((x) < 0) \
 											(x)--; \
 									}

/*
// Do we use DDBs for drawing?
*/

BOOL RedisplayContext::m_use_ddbs = TRUE;

/*
// Is the current printer a PostScript printer?
*/

int RedisplayContext::m_nCurrentPostScriptType = 0;

/*
// The constructor.
*/

RedisplayContext::RedisplayContext() : check_data(NULL),
 				check_handler(NULL),
				toggle_array(sizeof(TOGGLE_STRUCT)),
				terminate_interrupts(FALSE),
				m_fSkipToggle(FALSE),
 				clip_region(NULL),
 				is_printing(FALSE),
 				is_metafile(FALSE),
 				is_thumbnail(FALSE),
 				is_print_preview(FALSE),
				smoothing(TRISTATE_SELECTED),
				coloring_book(TRISTATE_SELECTED),
				mirrored(FALSE),
				m_want_update_names(FALSE),
				m_fLastToggleChangedSomething(TRUE),
				using_images(FALSE),
				redisplay_x_pixel(0),
				redisplay_y_pixel(0),
				m_pDesktop(NULL),
				is_postscript(FALSE),
				outline_gamma_value(1000),
				bitmap_gamma_value(1000),
				m_AttributeDC(NULL),
				m_pUptoObject(NULL),
 				is_printing_as_bitmap(FALSE)
{
   destination_hdc = NULL;
   SetScaling();
}

BOOL RedisplayContext::RequiresBitmaps(void) const
{
	return is_printing && is_printing_as_bitmap;		// For now...
}


/*
// A safer (though potentially less accurate) convert.
*/

void RedisplayContext::pbox_to_screen_safer(PBOX_PTR src_pbox, BOOL expand_if_empty)
{
	PBOX pbox = *src_pbox;

/* X0 */

	pbox.x0 *= x_resolution;
	pbox.x0 -= scaled_source_x0;
	if (pbox.x0 < 0)
	{
		pbox.x0 -= PAGE_RESOLUTION-1;
	}
	pbox.x0 /= PAGE_RESOLUTION;

/* X1 */

	pbox.x1 *= x_resolution;
	pbox.x1 -= scaled_source_x0;
	if (pbox.x1 < 0)
	{
		pbox.x1 -= PAGE_RESOLUTION-1;
	}
	pbox.x1 /= PAGE_RESOLUTION;

/* Y0 */

	pbox.y0 *= y_resolution;
	pbox.y0 -= scaled_source_y0;
	if (pbox.y0 < 0)
	{
		pbox.y0 -= PAGE_RESOLUTION-1;
	}
	pbox.y0 /= PAGE_RESOLUTION;

/* Y1 */

	pbox.y1 *= y_resolution;
	pbox.y1 -= scaled_source_y0;
	if (pbox.y1 < 0)
	{
		pbox.y1 -= PAGE_RESOLUTION-1;
	}
	pbox.y1 /= PAGE_RESOLUTION;

/* View scale. */

	if (scale_numerator != scale_denominator)
	{
#if 0
		CORRECT_MULDIV(pbox.x0, scale_numerator, scale_denominator);
		CORRECT_MULDIV(pbox.x1, scale_numerator, scale_denominator);
		CORRECT_MULDIV(pbox.y0, scale_numerator, scale_denominator);
		CORRECT_MULDIV(pbox.y1, scale_numerator, scale_denominator);
#else
		pbox.x0 = scale_pcoord(pbox.x0, scale_numerator, scale_denominator);
		pbox.y0 = scale_pcoord(pbox.y0, scale_numerator, scale_denominator);
		pbox.x1 = scale_pcoord(pbox.x1, scale_numerator, scale_denominator);
		pbox.y1 = scale_pcoord(pbox.y1, scale_numerator, scale_denominator);
#endif
	}

	pbox.x0 += destination_x0;
	pbox.y0 += destination_y0;
	pbox.x1 += destination_x0;
	pbox.y1 += destination_y0;

	if (expand_if_empty)
	{
		if (pbox.x0 == pbox.x1)
		{
			pbox.x1++;
		}
		if (pbox.y0 == pbox.y1)
		{
			pbox.y1++;
		}
	}
//	printf("{%ld, %ld, %ld, %ld}\n", pbox);
	*src_pbox = pbox;
}

void RedisplayContext::pbox_to_screen(PBOX_PTR src_pbox, BOOL expand_if_empty)
{
//od("In: %ld %ld %ld %ld...", src_pbox->x0, src_pbox->y0, src_pbox->x1, src_pbox->y1);

#ifndef _WIN32
	PboxToScreen32Data Data32;
	if (g_fProcessorIs386Compatible)
	{
		Data32.Pbox = *src_pbox;
		Data32.scaled_source_x0 = scaled_source_x0;
		Data32.scaled_source_y0 = scaled_source_y0;
		Data32.scaled_page_res_denominator = scaled_page_res_denominator;
		Data32.x_resolution = x_resolution;
		Data32.y_resolution = y_resolution;
		Data32.destination_x0 = destination_x0;
		Data32.destination_y0 = destination_y0;
		Data32.scale_numerator = scale_numerator;
		Data32.scale_denominator = scale_denominator;
		Data32.fExpandIfEmpty = expand_if_empty;

		PboxToScreen32(&Data32);
//od("32out: %ld %ld %ld %ld...", Data32.Pbox.x0, Data32.Pbox.y0, Data32.Pbox.x1, Data32.Pbox.y1);

		*src_pbox = Data32.Pbox;
		return;
	}
#endif

// Old way (more or less!)
//	pbox_to_screen_safer(src_pbox, expand_if_empty);

	PBOX pbox = *src_pbox;
	PCOORD d = PAGE_RESOLUTION;

/* X0 */

	pbox.x0 *= x_resolution;
	pbox.x0 -= scaled_source_x0;

	pbox.x1 *= x_resolution;
	pbox.x1 -= scaled_source_x0;

	pbox.y0 *= y_resolution;
	pbox.y0 -= scaled_source_y0;

	pbox.y1 *= y_resolution;
	pbox.y1 -= scaled_source_y0;

	if (scale_numerator != scale_denominator)
	{
		SHORT numer_bits = bits_required_by((uint32)scale_numerator);

		if (bits_required_by_long(pbox.x0) + numer_bits > MAX_REDISP_BITS
				|| bits_required_by_long(pbox.y0) + numer_bits > MAX_REDISP_BITS
				|| bits_required_by_long(pbox.x1) + numer_bits > MAX_REDISP_BITS
				|| bits_required_by_long(pbox.y1) + numer_bits > MAX_REDISP_BITS)
		{
			pbox_to_screen_safer(src_pbox, expand_if_empty);
/*od("16out(safe): %ld %ld %ld %ld\n\r", src_pbox->x0, src_pbox->y0, src_pbox->x1, src_pbox->y1); 
if (g_fProcessorIs386Compatible)
{
	if (src_pbox->x0 != Data32.Pbox.x0 || src_pbox->x1 != Data32.Pbox.x1 ||
		src_pbox->y0 != Data32.Pbox.y0 || src_pbox->y1 != Data32.Pbox.y1)
	{
		od("pbox_to_screen: 32bit result does not match\n\r");
	}
} */
			return;
		}

		pbox.x0 *= scale_numerator;
		pbox.y0 *= scale_numerator;
		pbox.x1 *= scale_numerator;
		pbox.y1 *= scale_numerator;

		d *= scale_denominator;
	}

	// Ensure our denominator is legal!  (No divide by zero allowed!)
	if (d == 0)
	{
		d = 1;
	}

	if (pbox.x0 < 0)
	{
		pbox.x0 -= d-1;
	}
	pbox.x0 /= d;

/* X1 */

	if (pbox.x1 < 0)
	{
		pbox.x1 -= d-1;
	}
	pbox.x1 /= d;

/* Y0 */

	if (pbox.y0 < 0)
	{
		pbox.y0 -= d-1;
	}
	pbox.y0 /= d;

/* Y1 */

	if (pbox.y1 < 0)
	{
		pbox.y1 -= d-1;
	}
	pbox.y1 /= d;

	pbox.x0 += destination_x0;
	pbox.y0 += destination_y0;
	pbox.x1 += destination_x0;
	pbox.y1 += destination_y0;

	if (expand_if_empty)
	{
		if (pbox.x0 == pbox.x1)
		{
			pbox.x1++;
		}
		if (pbox.y0 == pbox.y1)
		{
			pbox.y1++;
		}
	}
//	printf("{%ld, %ld, %ld, %ld}\n", pbox);
	*src_pbox = pbox;

/*od("16out(fast): %ld %ld %ld %ld\n\r", src_pbox->x0, src_pbox->y0, src_pbox->x1, src_pbox->y1); 
if (g_fProcessorIs386Compatible)
{
	if (src_pbox->x0 != Data32.Pbox.x0 || src_pbox->x1 != Data32.Pbox.x1 ||
		src_pbox->y0 != Data32.Pbox.y0 || src_pbox->y1 != Data32.Pbox.y1)
	{
		od("pbox_to_screen: 32bit result does not match\n\r");
	}
}*/

}

PRIVATE PCOORD near
scale_pcoord_round(PCOORD x, PCOORD n, PCOORD d)
{
	return (x*n + d-1)/d;
}

void RedisplayContext::screen_to_pbox(LPRECT r, PBOX_PTR src_pbox)
{
#ifdef REDISP_NEW_STYLE
	PBOX pbox;
	PCOORD sx0 = scaled_source_x0;
	PCOORD sy0 = scaled_source_y0;
	PCOORD dx = x_resolution;
	PCOORD dy = y_resolution;

	pbox.x0 = (PCOORD)(r->left - destination_x0);
	pbox.y0 = (PCOORD)(r->top - destination_y0);
	pbox.x1 = (PCOORD)(r->right - destination_x0);
	pbox.y1 = (PCOORD)(r->bottom - destination_y0);

	pbox.x0 *= PAGE_RESOLUTION;
	pbox.x1 *= PAGE_RESOLUTION;
	pbox.y0 *= PAGE_RESOLUTION;
	pbox.y1 *= PAGE_RESOLUTION;

/* View scale */

	if (scale_numerator != scale_denominator)
	{
		sx0 *= scale_numerator;
		sy0 *= scale_numerator;
		dx *= scale_numerator;
		dy *= scale_numerator;
		pbox.x0 *= scale_denominator;
		pbox.y0 *= scale_denominator;
		pbox.x1 *= scale_denominator;
		pbox.y1 *= scale_denominator;
	}

	pbox.x0 += sx0;
	if (pbox.x0 < 0)
	{
		pbox.x0 -= dx-1;
	}

	pbox.x1 += sx0;
	if (pbox.x1 < 0)
	{
		pbox.x1 -= dx-1;
	}

	pbox.y0 += sy0;
	if (pbox.y0 < 0)
	{
		pbox.y0 -= dy-1;
	}

	pbox.y1 += sy0;
	if (pbox.y1 < 0)
	{
		pbox.y1 -= dy-1;
	}

	pbox.x0 /= dx;
	pbox.x1 /= dx;
	pbox.y0 /= dy;
	pbox.y1 /= dy;

//	printf("{%ld, %ld, %ld, %ld}\n", pbox);
	*src_pbox = pbox;
#else
	PBOX pbox;

	pbox.x0 = (PCOORD)(r->left - destination_x0);
	pbox.y0 = (PCOORD)(r->top - destination_y0);
	pbox.x1 = (PCOORD)(r->right - destination_x0);
	pbox.y1 = (PCOORD)(r->bottom - destination_y0);

/* View scale */

	if (scale_numerator != scale_denominator)
	{
#if 0
		CORRECT_MULDIV(pbox.x0, scale_denominator, scale_numerator);
		CORRECT_MULDIV(pbox.x1, scale_denominator, scale_numerator);
		CORRECT_MULDIV(pbox.y0, scale_denominator, scale_numerator);
		CORRECT_MULDIV(pbox.y1, scale_denominator, scale_numerator);
#else
		pbox.x0 = scale_pcoord(pbox.x0, scale_denominator, scale_numerator);
		pbox.y0 = scale_pcoord(pbox.y0, scale_denominator, scale_numerator);
		pbox.x1 = scale_pcoord_round(pbox.x1, scale_denominator, scale_numerator);
		pbox.y1 = scale_pcoord_round(pbox.y1, scale_denominator, scale_numerator);
#endif
	}

/* X0 */

	pbox.x0 *= PAGE_RESOLUTION;
	if (pbox.x0 < 0)
	{
		pbox.x0 -= x_resolution-1;
	}
	pbox.x0 += scaled_source_x0;
	pbox.x0 /= x_resolution;

/* X1 */

	pbox.x1 *= PAGE_RESOLUTION;
	if (pbox.x1 < 0)
	{
		pbox.x1 -= x_resolution-1;
	}
	pbox.x1 += scaled_source_x0;
	pbox.x1 /= x_resolution;

/* Y0 */

	pbox.y0 *= PAGE_RESOLUTION;
	if (pbox.y0 < 0)
	{
		pbox.y0 -= y_resolution-1;
	}
	pbox.y0 += scaled_source_y0;
	pbox.y0 /= y_resolution;

/* Y1 */

	pbox.y1 *= PAGE_RESOLUTION;
	if (pbox.y1 < 0)
	{
		pbox.y1 -= y_resolution-1;
	}
	pbox.y1 += scaled_source_y0;
	pbox.y1 /= y_resolution;

//	printf("{%ld, %ld, %ld, %ld}\n", pbox);
	*src_pbox = pbox;
#endif
}

/*
// Convert a pbox to a rect.
*/

BOOL RedisplayContext::convert_pbox(PBOX_PTR src_pbox, LPRECT dest_r, LPRECT clip_r)
{
	RECT our_clip;
	PBOX pbox = *src_pbox;

	if (clip_r == NULL)
	{
		our_clip.left = -32000;
		our_clip.top = -32000;
		our_clip.right = 32000;
		our_clip.bottom = 32000;
	}
	else
	{
		our_clip = *clip_r;
	}

/*
// See if it is totally clipped.
*/

	if (pbox.x0 >= (PCOORD)our_clip.right
			|| pbox.y0 >= (PCOORD)our_clip.bottom
			|| pbox.x1 <= (PCOORD)our_clip.left
			|| pbox.y1 <= (PCOORD)our_clip.top)
	{
	/* Totally clipped. */
//		printf("\nTotally clipped!\n");
//		printf("[%ld, %ld, %ld, %ld vs. %d, %d, %d, %d]\n", pbox, our_clip);
		return FALSE;
	}

/*
// Assign the values back, clipped.
*/

/* X0 */

	if (pbox.x0 < (PCOORD)our_clip.left)
	{
		dest_r->left = our_clip.left;
	}
	else
	{
		dest_r->left = (SHORT)pbox.x0;
	}

/* Y0! */

	if (pbox.y0 < (PCOORD)our_clip.top)
	{
		dest_r->top = our_clip.top;
	}
	else
	{
		dest_r->top = (SHORT)pbox.y0;
	}

/* X1 */

	if (pbox.x1 > (PCOORD)our_clip.right)
	{
		dest_r->right = our_clip.right;
	}
	else
	{
		dest_r->right = (SHORT)pbox.x1;
	}

/* Y1 */

	if (pbox.y1 > (PCOORD)our_clip.bottom)
	{
		dest_r->bottom = our_clip.bottom;
	}
	else
	{
		dest_r->bottom = (SHORT)pbox.y1;
	}

	return TRUE;
}

/*
// page_x_to_screen()
//
//		This duplicates what is done by pbox_to_screen(), applied only to a 
// single X coordinate.  It is here for speed, when we only need to convert 
// an X coordinate to destination space.
//
// Arguments:
//
//		PCOORD		x				X coordinate to transform (at 1800 dpi)
//
//	Returns:
//		X coordinate in destination space
*/

int RedisplayContext::page_x_to_screen(PCOORD x)
{
#ifndef _WIN32
	if (g_fProcessorIs386Compatible)
	{
		PageXToScreen32Data Data32;

		Data32.page_x = x;
		Data32.scaled_source_x0 = scaled_source_x0;
		Data32.scaled_page_res_denominator = scaled_page_res_denominator;
		Data32.x_resolution = x_resolution;
		Data32.destination_x0 = destination_x0;
		Data32.scale_numerator = scale_numerator;
		Data32.scale_denominator = scale_denominator;

		PageXToScreen32(&Data32);
		return (int)Data32.page_x;
	}
#endif

	BOOL needs_scale = FALSE;
	PCOORD d = PAGE_RESOLUTION;

	x *= x_resolution;
	x -= scaled_source_x0;

	if (scale_numerator != scale_denominator)
	{
		if (bits_required_by_long(x) + bits_required_by((uint32)scale_numerator) > MAX_REDISP_BITS)
		{
			needs_scale = TRUE;
		}
		else
		{
			x *= scale_numerator;
			d *= scale_denominator;
		}
	}
	if (x < 0)
	{
		x -= d-1;
	}
	x /= d;

	if (needs_scale)
	{
		x = scale_pcoord(x, scale_numerator, scale_denominator);
	}

	x += destination_x0;
	return (int)x;
}

/*
// screen_x_to_page()
//
//		This duplicates what is done by screen_to_pbox(), applied only to a 
// single X coordinate.  It is here for speed, when we only need to convert 
// an X coordinate to page space.
//
// Arguments:
//
//		short X coordinate in destination space
//
//	Returns:
//		PCOORD x				transformed X coordinate (at 1800 dpi)
*/

PCOORD RedisplayContext::screen_x_to_page(int xc)
{
	PCOORD x = (PCOORD)xc;

	x -= destination_x0;

#ifdef REDISP_NEW_STYLE
	PCOORD sx0 = scaled_source_x0;
	PCOORD dx = x_resolution;

	x *= PAGE_RESOLUTION;

	if (scale_numerator != scale_denominator)
	{
		sx0 *= scale_numerator;
		dx *= scale_numerator;
		x *= scale_denominator;
	}
	x += sx0;
	if (x < 0)
	{
		x -= dx-1;
	}
	x /= dx;
#else
	if (scale_numerator != scale_denominator)
	{
		x = scale_pcoord(x, scale_denominator, scale_numerator);
	}

	x *= PAGE_RESOLUTION;

	if (x < 0)
	{
		x -= x_resolution-1;
	}

	x += scaled_source_x0;
	x /= x_resolution;
#endif

	return x;
}

/*
// page_y_to_screen()
//
//		This duplicates what is done by pbox_to_screen(), applied only to a 
// single Y coordinate.  It is here for speed, when we only need to convert 
// a Y coordinate to destination space.
//
// Arguments:
//
//		PCOORD		y				Y coordinate to transform (at 1800 dpi)
//
//	Returns:
//		Y coordinate in destination space
*/

int RedisplayContext::page_y_to_screen(PCOORD y)
{
#ifndef _WIN32
	if (g_fProcessorIs386Compatible)
	{
		PageXToScreen32Data Data32;

		Data32.page_x = y;
		Data32.scaled_source_x0 = scaled_source_y0;
		Data32.scaled_page_res_denominator = scaled_page_res_denominator;
		Data32.x_resolution = y_resolution;
		Data32.destination_x0 = destination_y0;
		Data32.scale_numerator = scale_numerator;
		Data32.scale_denominator = scale_denominator;

		PageXToScreen32(&Data32);
		return (int)Data32.page_x;
	}
#endif

	BOOL needs_scale = FALSE;
	PCOORD d = PAGE_RESOLUTION;

	y *= y_resolution;
	y -= scaled_source_y0;

	if (scale_numerator != scale_denominator)
	{
		if (bits_required_by_long(y) + bits_required_by((uint32)scale_numerator) > MAX_REDISP_BITS)
		{
			needs_scale = TRUE;
		}
		else
		{
			y *= scale_numerator;
			d *= scale_denominator;
		}
	}
	if (y < 0)
	{
		y -= d-1;
	}
	y /= d;

	if (needs_scale)
	{
		y = scale_pcoord(y, scale_numerator, scale_denominator);
	}

	y += destination_y0;

	return (int)y;
}

/*
// screen_y_to_page()
//
//		This duplicates what is done by screen_to_pbox(), applied only to a 
// single Y coordinate.  It is here for speed, when we only need to convert 
// an Y coordinate to page space.
//
// Arguments:
//
//		short Y coordinate in destination space
//
//	Returns:
//		PCOORD y				transformed Y coordinate (at 1800 dpi)
*/

PCOORD RedisplayContext::screen_y_to_page(int yc)
{
	PCOORD y = (PCOORD)yc;

	y -= destination_y0;

#ifdef REDISP_NEW_STYLE
	PCOORD sy0 = scaled_source_y0;
	PCOORD dy = y_resolution;

	y *= PAGE_RESOLUTION;

	if (scale_numerator != scale_denominator)
	{
		sy0 *= scale_numerator;
		dy *= scale_numerator;
		y *= scale_denominator;
	}
	y += sy0;
	if (y < 0)
	{
		y -= dy-1;
	}
	y /= dy;
#else
	if (scale_numerator != scale_denominator)
	{
		y = scale_pcoord(y, scale_denominator, scale_numerator);
	}

	y *= PAGE_RESOLUTION;

	if (y < 0)
	{
		y -= y_resolution-1;
	}

	y += scaled_source_y0;
	y /= y_resolution;
#endif

	return y;
}

/*
// Convert a screen point to a ppnt.
*/

void RedisplayContext::screen_to_ppnt(POINT p, PPNT *pp)
{
	pp->x = screen_x_to_page(p.x);
	pp->y = screen_y_to_page(p.y);
}

/*
// Convert a ppnt to a screen point.
*/

void RedisplayContext::ppnt_to_screen(PPNT pp, POINT *p)
{
	p->x = page_x_to_screen(pp.x);
	p->y = page_y_to_screen(pp.y);
}

void RedisplayContext::mouse_box(POINT p, PBOX_PTR pbox)
{
	RECT r;

	r.left = p.x-CONTROL_DOT_WIDTH;
	r.top = p.y-CONTROL_DOT_WIDTH;
	r.right = p.x+CONTROL_DOT_WIDTH;
	r.bottom = p.y+CONTROL_DOT_WIDTH;

	screen_to_pbox(&r, pbox);
}

/*
// Check whether an interrupt has occurred.
*/

BOOL RedisplayContext::check_interrupt(void)
{
	if (check_handler == NULL)
	{
		return FALSE;
	}
	else
	{
		return check_handler(check_data);
	}
}

/*
// Toggle anything which needs to be removed before drawing takes place.
*/


void RedisplayContext::toggle(BOOL turn_on, LPRECT clip)
{
	if (m_fSkipToggle)
	{
		return;
	}

	SHORT count;

//	TRACE("RC::toggle(%d)\n", turn_on);
	// Shortcut: don't do a "turn on" if the last "turn off" had no effect
	if (turn_on == TRUE && !m_fLastToggleChangedSomething)
	{
//		TRACE("RC::toggle -> skip off\n");
		m_fLastToggleChangedSomething = TRUE;
//		return;
	}
	m_fLastToggleChangedSomething = TRUE;

	if ((count = toggle_array.count()) != 0)
	{
		HRGN hrgnClip = NULL;

		if (clip != NULL)
		{
		/* Create a clip region for our toggle. */

			hrgnClip = CreateRectRgn(clip->left, clip->top, clip->right, clip->bottom);

			if (clip_region != NULL)
			{
				switch (CombineRgn(hrgnClip, hrgnClip, clip_region, RGN_AND))
				{
					case NULLREGION:
					case ERROR:
					{
						DeleteObject(hrgnClip);
						return;
					}
					default:
					{
						break;
					}
				}
			}
		}

		SaveDC(destination_hdc);

		if (hrgnClip != NULL)
		{
			SelectClipRgn(destination_hdc, hrgnClip);
			DeleteObject(hrgnClip);
		}

	/*
 	// Do the toggle.
	// Because of the check above for count, we know we have at least one.
 	*/

		TOGGLE_STRUCT_PTR toggle = (TOGGLE_STRUCT_PTR)toggle_array.get_element(0);

		m_fLastToggleChangedSomething = turn_on;

		for (SHORT i = 0; i < count; i++, toggle++)
		{
			m_fLastToggleChangedSomething |=
				toggle->function(this, turn_on, clip, toggle->data);
		}

	/* Reselect the original DC. */

		RestoreDC(destination_hdc, -1);
	}
}

/*
// Derive the Raster Info from a passed DC.
*/

void RedisplayContext::set_info(HDC hDC, BOOL fPrinter /*=FALSE*/)
{
	WORD raster_caps;
	WORD colors = GetDeviceCaps(hDC, NUMCOLORS);

	raster_caps = GetDeviceCaps(hDC, RASTERCAPS);

	info.bpp = GetDeviceCaps(hDC, BITSPIXEL);
	info.planes = GetDeviceCaps(hDC, PLANES);

	info.type = RASTER_TYPE_UNKNOWN;

/////
//	info.bpp = 1;
//	info.planes = 1;
//	colors = 2;
/////

	WORD color_bits = info.planes * info.bpp;

	if (color_bits == 24)
	{
		info.type = RASTER_TYPE_24;
		info.bpp = 24;
		info.planes = 1;
	}
	else if (info.planes == 1)
	{
		if (info.bpp == 1 && colors == 2)
		{
			info.type = RASTER_TYPE_BILEVEL;
		}
		else if (info.bpp == 8 && (raster_caps & RC_PALETTE))
		{
			info.type = RASTER_TYPE_PALETTE;		/* 256 color palette device */
		}
	}
	else if (info.bpp == 1 && (info.planes == 3 || info.planes == 4))
	{
		info.type = RASTER_TYPE_8;				/* 8-color primary device */
	}

	if (info.type == RASTER_TYPE_UNKNOWN)
	{
	/* Pick something. */
		if (color_bits > 8 || colors > 256)
		{
			info.type = RASTER_TYPE_24;
			info.bpp = 24;
		 	info.planes = 1;
		}
		else
		{
			info.type = RASTER_TYPE_256;			/* 256-color, but no palette! */
			info.bpp = 8;
		 	info.planes = 1;
		}
	}
#if 0

	od("Technology: %d, caps: %x\r\n", GetDeviceCaps(hDC, TECHNOLOGY), raster_caps);

	if (caps & RC_PALETTE)
	{
		od("RC_PALETTE: palsize: %d, colorres: %d\r\n",
				GetDeviceCaps(hDC, SIZEPALETTE),
				GetDeviceCaps(hDC, COLORRES));
	}
#endif

   /* Set whether it's a PostScript printer or not. */

	is_postscript = fPrinter ? m_nCurrentPostScriptType : FALSE;

	od("raster type is %d (%d, %d) clr:%d (RC: %x, PS: %d)\r\n", info.type, info.bpp, info.planes, colors, raster_caps, is_postscript);
}

/*
// Build the gamma curve for this context.
// It's assumed that the gamma curve pointer is valid.
*/

void RedisplayContext::build_gamma_curve(int outline_gamma, int bitmap_gamma)
{
/* Build the new outline curve. */

	if (outline_gamma != -1)
	{
		if (outline_gamma > 0)
		{
			outline_gamma_value = outline_gamma;
		}
		build_gamma_curve(outline_gamma_curve, outline_gamma_value);
	}

/* Build the new bitmap curve. */

	if (bitmap_gamma != -1)
	{
		if (bitmap_gamma > 0)
		{
			bitmap_gamma_value = bitmap_gamma;
		}
		build_gamma_curve(bitmap_gamma_curve, bitmap_gamma_value);
	}
}

/*
// Build a gamma curve.
*/

void RedisplayContext::build_gamma_curve(LPBYTE curve, int gamma)
{
	double gammaVal = (double)gamma/1000.0;

/*
// Crank out the gamma curve.
// The 255.0 in the next computation is simply 256 (steps) - 1.
*/

	for (int i = 0; i < 256; i++)
 	{
		curve[i] = (BYTE)(255.0 * 
			pow((double)i/255.0, gammaVal) + 0.5);
	}
}

/*
// Register a toggle function.
*/

void RedisplayContext::register_toggle(TOGGLE_HANDLER function, LPVOID data)
{
	TOGGLE_STRUCT toggle;

/* Build the toggle structure. */

	toggle.function = function;
	toggle.data = data;

/* Add it to the array. */

	toggle_array.insert_element(&toggle, 0);
}

/*
// Remove a toggle function.
// If the same function is registered more than once, the most recent one
// is removed.
*/

void RedisplayContext::remove_toggle(TOGGLE_HANDLER function)
{
/*
// Find and remove the toggle function.
*/

	TOGGLE_STRUCT_PTR toggle = (TOGGLE_STRUCT_PTR)toggle_array.get_element(0);
	SHORT count = toggle_array.count();

	for (SHORT i = 0; i < count; i++, toggle++)
	{
		if (toggle->function == function)
		{
		/* Only remove the first one we find. */
			toggle_array.delete_element(i);
			break;
		}
	}
}

/*
// Register the routine to handle update names.
*/

void RedisplayContext::register_update_name(UPDATE_NAME_HANDLER handler, LPVOID data)
{
	update_name_data = data;
	update_name_handler = handler;
	m_want_update_names = (handler != NULL);
}

/*
// Show the current update name.
*/

void RedisplayContext::update_name(LPVOID object, LPCSTR name)
{
	if (m_want_update_names)
	{
		update_name_handler(object, name, update_name_data);
	}
}

void RedisplayContext::set_redisplay_pixel()
{
	PCOORD pixel;

	pixel = scale_pcoord(scale_number(100, PAGE_RESOLUTION, x_resolution),
               scale_denominator, scale_numerator)/100L;
	redisplay_x_pixel = (pixel > 0x7FFFL) ? 0x7FFF : (SHORT)pixel;

	if (y_resolution == x_resolution)
	{
	/* Quick 'calc'. */
		redisplay_y_pixel = redisplay_x_pixel;
	}
	else
	{
		pixel = scale_pcoord(scale_number(100, PAGE_RESOLUTION, y_resolution),
	               scale_denominator, scale_numerator)/100L;
		redisplay_y_pixel = (pixel > 0x7FFFL) ? 0x7FFF : (SHORT)pixel;
	}
}

void RedisplayContext::erase_area(RECT clip)
{
	if (IsScreen() && m_pDesktop != NULL)
	{
		m_pDesktop->DrawBackground(&clip, this);
	}
}

void RedisplayContext::SetScaling(USHORT aNumerator, USHORT aDenominator)
{
	scale_numerator = aNumerator;
	scale_denominator = aDenominator;
	scaled_page_res_denominator = (DWORD)PAGE_RESOLUTION * (DWORD)scale_denominator;
}

int RedisplayContext::GetScaledXResolution(void)
{
	int nResolution = MulDiv(x_resolution, GetScaleNumerator(), GetScaleDenominator());
	if (nResolution == 0)
	{
		nResolution = 1;
	}
	return nResolution;
}

int RedisplayContext::GetScaledYResolution(void)
{
	int nResolution = MulDiv(y_resolution, GetScaleNumerator(), GetScaleDenominator());
	if (nResolution == 0)
	{
		nResolution = 1;
	}
	return nResolution;
}
