/*
// $Header: /PM8/App/PAINT.H 1     3/03/99 6:08p Gbeddow $
//
// Graphic paint definitions.
//
// $Log: /PM8/App/PAINT.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
//    Rev 1.0   14 Aug 1997 15:23:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:58   Fred
// Initial revision.
// 
//    Rev 1.3   06 Dec 1996 09:15:50   Jay
// More transparent bitmap stuff.
// 
//    Rev 1.2   05 Dec 1996 12:30:38   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   01 Nov 1996 17:11:54   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:46:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:12   FRED
// Initial revision.
// 
//    Rev 2.3   09 Oct 1995 15:15:06   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.2   19 Sep 1995 15:43:14   JAY
// Changed a SHORT to USHORT since it is an offset.
// 
//    Rev 2.1   09 Mar 1995 08:27:30   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:50   JAY
// Initial revision.
// 
//    Rev 1.2   19 Jan 1994 17:08:14   JAY
//  
// 
//    Rev 1.1   30 Dec 1993 16:11:06   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:02   JAY
//  
*/

#ifndef __PAINT_H__
#define __PAINT_H__

#include "imagedef.h"

/*
// A paint session base info structure.
*/

class PaintContext
{
protected:
	virtual VOID compute_source_extent(VOID);
public:
	PaintContext();
	virtual ~PaintContext();

//	GraphicObjectPtr		object;		/* The object this is */
//	GraphicPtr				record;		/* The graphic data for this object */

	FLAGS						graphic_flags;
	PBOX						bound;		/* Page bound of "object" */
	LPRECT					clip;			/* Clipping rectangle (dest coordinates) */
	RedisplayContextPtr	rc;			/* The redisplay context */

	RasterInfo				raster_info;	/* Destination characteristics */

//	SHORT				mode;
	BOOL				do_xflip;
	BOOL				do_yflip;
	BOOL				no_expand_clip;		// Kludge!!!

/*
// The render method.
*/

	RENDER_METHOD	render_method;

/*
// Error info used by pre_paint, etc.
*/

	ERROR_INFO_PTR	far *error_info;

/*
// Filled in by image_info() method of image object.
*/

	IMAGE_TYPE	image_type;

/*
// Filled in by pre_paint().
*/

/* Dest variables. */

	RECT			dest_bound;				/* Full transformed bound of graphic. */
	int			dest_width;				/* Full width of transformed bound. */
	int			dest_height;			/* Full height of transformed bound. */
	int			current_dest_line;

	RECT			dest;						/* Clipped transformed bound of graphic. */
	int			clipped_dest_height;	/* Clipped height of transformed bound. */
	int			clipped_dest_width;	/* Clipped width of transformed bound. */
	int			min_dest_line;			/* Minimum line to draw */
	int			max_dest_line;			/* Maximum line to draw */

/* Source variables. */

	/* source_width and height are initialized by compute_source_extent(). */
	int			source_width;
	int			source_height;		/* Height of source file. */
	RECT			srce;					/* Clipped rect in source file. */
	int			clipped_source_height;
	int			current_source_line;

/* Scaling variables. */

	POINT			dest_factor;				/* Destination scaling factors. */
	POINT			srce_factor;				/* Source scaling factors. */

/* Smoothing variables. */

	BOOL			smoothing;					/* Are we smoothing? */
	int			smooth_pixel_width;		/* The width of a scaled pixel */
	int			smooth_pixel_height;		/* The height of a scaled pixel */

	int			y_fraction;					/* Current y scaling fraction */
	int			y_compare;					/* Current y scaling compare */
	int			y_delta;						/* Current y scaling delta */

/*
// Variables set by init_strip() and compute_strip().
*/

	int			strip_byte_width;		/* byte width of row. */
	int			m_nMaskByteWidth;		/* byte width of mask row. */
	int			strip_height;			/* height of full strip */
	int			last_strip_height;	/* height of last strip */

	LPBYTE		strip;					/* pointer to strip data */

	int			next_strip_row;

	int			n_strips;				/* Number of strips we will be doing */
	int			current_strip;			/* Which of 0 to n_strips are we doing? */

	UINT			strip_offset;
	int			strip_row;

	RECT			dest_rect;
	BOOL			m_fUseDest;				// Are we stripping the destination space?

	// Mask variables.
	BOOL			m_fHasMask;				// Are we doing masked stuff?

	LPBYTE		m_pMask;					/* pointer to mask strip data */
	int			m_nNextMaskRow;
	UINT			m_uMaskOffset;

/*
// The destination DIB header we use.
*/

	struct
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[256];		// Enough for 256 colors.
	} bitmap_info;

	struct
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[2];			// Enough for 2 colors.
	} m_MaskInfo;

/*
// Methods.
*/

	VOID set_render_method(RENDER_METHOD method);

	BOOL pre_paint(void);
	BOOL initialize_strip(int x_size, BOOL use_dest, BOOL use_clipped_x, BOOL fHasMask = FALSE);
	void build_graphic_map(int x_size);
	void compute_strip(int i);
	void free_strip(void);

	IMAGE_CODE check_interrupt(VOID);
	// Get the raster type, translating RASTER_TYPE_DIRECT.
	int GetRasterType(void);

	void show_graphic_map(RECT rdest, int nSrcY, int nSrcLines);
};

typedef PaintContext far *PaintContextPtr;

#endif					// #ifndef __PAINT_H__
