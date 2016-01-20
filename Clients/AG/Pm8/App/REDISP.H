/*
// $Header: /PM8/App/REDISP.H 1     3/03/99 6:11p Gbeddow $
//
// Redisplay context definitions.
//
// $Log: /PM8/App/REDISP.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
// 2     2/27/98 2:47p Jayn
// "Draw as bitmaps"
// 
//    Rev 1.0   14 Aug 1997 15:25:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:44   Fred
// Initial revision.
// 
//    Rev 1.11   11 Jun 1997 17:28:22   Fred
// Bitmap drawing optimizations
// 
//    Rev 1.10   05 Jun 1997 16:47:46   Jay
// New PostScript type check and handling
// 
//    Rev 1.9   26 Mar 1997 14:52:06   Jay
// Interruptable preview
// 
//    Rev 1.8   06 Mar 1997 17:28:10   Jay
// Print preview stuff
// 
//    Rev 1.7   07 Feb 1997 09:00:06   Fred
// Made convert_pbox static so it could be called without an rc
// 
//    Rev 1.6   04 Nov 1996 12:11:06   Jay
// Changed UINT for x_resolution and y_resolution.
// 
//    Rev 1.5   01 Nov 1996 17:11:54   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.4   28 Oct 1996 13:46:28   Jay
// Changes from 3.01 code
// 
//    Rev 1.4   23 Oct 1996 00:27:00   Jay
// Can override the postscript setting in the redisplay context.
// 
//    Rev 1.3   15 Oct 1996 09:05:14   Jay
// Ugh!!
// 
//    Rev 1.2   15 Oct 1996 09:04:14   Jay
// Ugh!
// 
//    Rev 1.1   15 Oct 1996 08:54:40   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.0   14 Mar 1996 13:47:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:04   FRED
// Initial revision.
// 
//    Rev 2.4   09 Oct 1995 15:15:06   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.3   07 Sep 1995 15:48:18   JAY
// Special code to skip the toggle for off-screen generation.
// 
//    Rev 2.2   30 Jun 1995 16:33:16   JAY
// New CPaperInfo class.
// 
//    Rev 2.1   15 Feb 1995 17:01:50   JAY
// Added an is_metafile member.
// 
//    Rev 2.0   07 Feb 1995 15:48:56   JAY
// Initial revision.
// 
//    Rev 1.14   30 Jan 1995 09:14:52   JAY
// Added an attribute DC (to support metafile case).
// 
//    Rev 1.13   13 Jul 1994 11:26:28   JAY
// update name code now takes an LPCSTR.
// 
//    Rev 1.12   16 May 1994 08:41:08   JAY
// Broke the gamma_curve into two separate curves, one for bitmaps and one for
// outlines.
// 
//    Rev 1.11   13 Apr 1994 17:15:18   JAY
// New code and variable to handle PostScript printer types.
// Added a PS type 2 (QMS color v 2.xx) to handle that printer.
// 
//    Rev 1.10   07 Apr 1994 10:45:22   JAY
// Added m_use_ddbs field.
// 
//    Rev 1.9   15 Mar 1994 13:22:14   JAY
// Broke draw_page() out of erase_area() so it can be called elsewhere.
// 
//    Rev 1.8   22 Feb 1994 15:00:46   JAY
// Added new field which tells whether the redisplay device is PostScript or not.
// 
//    Rev 1.7   17 Feb 1994 08:41:40   MACDUFF
// Hid the scaling numerator and denominator and added access methods
// for them.
// 
//    Rev 1.6   10 Feb 1994 15:14:24   MACDUFF
// Toggle functions now return flag indicating whether they drew anything.
// 
//    Rev 1.5   28 Jan 1994 17:11:28   JAY
//  
// 
//    Rev 1.4   19 Jan 1994 17:07:56   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:01:06   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:08:16   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:11:00   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:25:48   JAY
//  
*/

class RedisplayContext;
class CEditDesktop;

#ifndef __REDISP_H__
#define __REDISP_H__

#include "array.h"
#include "pagedefs.h"

class PMGPageObject;

typedef enum
{
	RASTER_TYPE_UNKNOWN = -1,			/* Unknown type */
	RASTER_TYPE_BILEVEL,					/* Black and white (or 2-color) */
	RASTER_TYPE_8,							/* 8-color primary device */
	RASTER_TYPE_PALETTE,					/* 256-color palette device */
	RASTER_TYPE_256,						/* 256-color non-palette device */
	RASTER_TYPE_24,						/* 24-bit color device */
	RASTER_TYPE_DIRECT					/* Ignore device characteristics */
} RASTER_TYPE;

/*
// A desktop.
// This is a class that embodies the edit space that the user sees. At this
// level, it's pretty simple (we only need to be able to draw the background).
*/

class CEditDesktop
{
public:
	virtual ~CEditDesktop() {}
	virtual void DrawBackground(LPCRECT pClip, RedisplayContext* rc) = 0;
};

/*
// Tri-state attribute selection.
//
// None: no objects have this attribute regardless of their internal setting.
// Selected: objects have the attribute of their internal setting.
// All: all objects have this attribute regardless of their internal setting.
*/

typedef enum
{
	TRISTATE_NONE,
	TRISTATE_SELECTED,
	TRISTATE_ALL
} TRISTATE;

class RasterInfo
{
public:
	RASTER_TYPE	type;
	SHORT	bpp;
	SHORT	planes;
	SHORT error_bytes;			/* Error diffusion bytes per pixel */

	RasterInfo() : type(RASTER_TYPE_BILEVEL), bpp(1), planes(1), error_bytes(1) {}
};

/*
// A redisplay context.
// The Windows version of this has an HDC.
// Other platforms will vary!
*/

typedef class RedisplayContext far * RedisplayContextPtr;

typedef BOOL (*INTERRUPT_CHECK)(LPVOID data);
typedef BOOL (*TOGGLE_HANDLER)(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, LPVOID data);
typedef void (*UPDATE_NAME_HANDLER)(LPVOID object, LPCSTR name, LPVOID data);

typedef struct
{
	TOGGLE_HANDLER	function;
	LPVOID			data;
} TOGGLE_STRUCT, far *TOGGLE_STRUCT_PTR;

class RedisplayContext
{
private:
	LPVOID check_data;
	INTERRUPT_CHECK check_handler;

	LPVOID update_name_data;
	UPDATE_NAME_HANDLER update_name_handler;

	Array toggle_array;

	BOOL m_want_update_names;
	BOOL m_fLastToggleChangedSomething;

	UINT scale_numerator;
	UINT scale_denominator;
	DWORD scaled_page_res_denominator;

	HDC	m_AttributeDC;				/* The attribute DC. */
	BOOL m_fPrintAsBitmap;

public:
	RedisplayContext();

	RasterInfo info;

	BOOL is_printing;
	BOOL is_postscript;
	BOOL is_metafile;
	BOOL is_thumbnail;
	BOOL is_print_preview;
	BOOL is_printing_as_bitmap;

	int x_resolution;
	int y_resolution;

	PCOORD scaled_source_x0;
	PCOORD scaled_source_y0;

	RECT destination_rect;

	int destination_x0;
	int destination_y0;

	HDC	destination_hdc;				/* The destination DC. */
	HRGN	clip_region;					/* What region is selected in the HDC. */
	HWND	hwnd;								/* Destination window. */

	RECT	clip_rect;

	PBOX	source_pbox;
	CEditDesktop* m_pDesktop;			/* For erasing purposes */

	int outline_gamma_value;
	LPBYTE outline_gamma_curve;

	int bitmap_gamma_value;
	LPBYTE bitmap_gamma_curve;

	BOOL terminate_interrupts;

	BOOL m_fSkipToggle;

	PMGPageObject* m_pUptoObject;

	// Use this instead of is_printing in most cases.
	BOOL IsScreen(void) const
		{ return !is_printing && !is_thumbnail; }

	BOOL RequiresBitmaps(void) const;

/*
// Attribute DC support.
*/

	HDC AttributeDC(void) const
		{ return m_AttributeDC == NULL ? destination_hdc : m_AttributeDC; }
	void AttributeDC(HDC hdc)
		{ m_AttributeDC = hdc; }

/*
// The size of a pixel in page coordinates.
*/

	int redisplay_x_pixel;
	int redisplay_y_pixel;
	void set_redisplay_pixel();

/* Are we making an image? */

	BOOL using_images;

/*
// Smoothing control.
*/

	TRISTATE	smoothing;

/*
// Coloring book control.
*/

	TRISTATE coloring_book;

	BOOL mirrored;					/* In case we want it. */

	static BOOL convert_pbox(PBOX_PTR pbox, LPRECT rect, LPRECT clip_rect);

	void pbox_to_screen(PBOX_PTR pbox, BOOL expand_if_empty);
	void pbox_to_screen_safer(PBOX_PTR pbox, BOOL expand_if_empty);
	void screen_to_pbox(LPRECT rect, PBOX_PTR pbox);

	void screen_to_ppnt(POINT p, PPNT *pp);
	void ppnt_to_screen(PPNT pp, POINT *p);

	int page_x_to_screen(PCOORD x);
	PCOORD screen_x_to_page(int x);

	int page_y_to_screen(PCOORD y);
	PCOORD screen_y_to_page(int y);

	void mouse_box(POINT p, PBOX_PTR pbox);

	BOOL check_interrupt(void);
	void set_check_interrupt(INTERRUPT_CHECK check, LPVOID data = NULL) { check_handler = check; check_data = data; }

	void toggle(BOOL turn_on, LPRECT clip = NULL);

	void erase_area(RECT clip);

/*
// Register a toggle function.
*/

	void register_toggle(TOGGLE_HANDLER toggle, LPVOID data = NULL);

/*
// Remove a toggle function.
// If the same function is registered more than once, the most recent one
// is removed.
*/

	void remove_toggle(TOGGLE_HANDLER toggle);

/*
// Build the raster_info structure from a passed HDC.
*/

	void set_info(HDC hDC, BOOL fPrinter = FALSE);
	static void build_gamma_curve(LPBYTE curve, int bitmap_gamma);
	void build_gamma_curve(int bitmap_gamma = 0, int outline_gamma = 0);

/*
// Update name support.
// This will allow us to show what object is being updated during printing,
// etc.
// Since objects aren't defined at this point, we just use a LPVOID which we
// assume is actually a PMGPageObjectPtr, and we then cast it in the routine
// to the right thing.
*/

	BOOL want_update_names()
		{ return m_want_update_names; }

	void register_update_name(UPDATE_NAME_HANDLER handler, LPVOID data = NULL);

	void update_name(LPVOID object, LPCSTR name);

	void SetScaling(USHORT aNumerator = 1, USHORT aDenominator = 1);
	USHORT GetScaleNumerator() const { return scale_numerator; }
	USHORT GetScaleDenominator() const { return scale_denominator; }

	int GetScaledXResolution(void);
	int GetScaledYResolution(void);

/*
// Do we use device-dependent bitmaps when drawing?
*/

	static BOOL m_use_ddbs;

/*
// Is this printer a PostScript printer?
*/

	static int m_nCurrentPostScriptType;

/*
// Print as a bitmap?
*/

	void SetPrintAsBitmap(BOOL fSet)
	{ m_fPrintAsBitmap = fSet; }

};

#endif					// #ifndef __REDISP_H__
