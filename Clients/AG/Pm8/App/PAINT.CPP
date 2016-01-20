/*
//	PAINT.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Generic image painting routines.
//
// $Header: /PM8/App/PAINT.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/PAINT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 4     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 3     4/07/98 8:43a Jayn
// Expanded size capabilities.
// 
// 2     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
//    Rev 1.1   17 Sep 1997 09:12:32   Jay
// Bitmap rotation
// 
//    Rev 1.0   14 Aug 1997 15:23:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:34   Fred
// Initial revision.
// 
//    Rev 1.6   14 Feb 1997 09:39:56   Jay
// Don't use MulDiv()! It rounds!
// 
//    Rev 1.5   06 Dec 1996 09:15:48   Jay
// More transparent bitmap stuff.
// 
//    Rev 1.4   05 Dec 1996 12:30:20   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.3   01 Nov 1996 17:11:46   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.2   15 Oct 1996 08:54:34   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.1   20 Sep 1996 10:42:00   Jay
// Fixed an overflow problem.
// 
//    Rev 1.0   14 Mar 1996 13:44:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:52   FRED
// Initial revision.
// 
//    Rev 2.4   20 Nov 1995 09:03:56   JAY
// GetRasterType() now returns PALETTE for BILEVEL source since it has already
// been converted to 8x1 internally.
// 
//    Rev 2.3   09 Oct 1995 15:15:06   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.2   19 Sep 1995 14:02:14   JAY
// 
//    Rev 2.1   05 Jun 1995 11:31:04   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:00   JAY
// Initial revision.
// 
//    Rev 1.9   30 Nov 1994 16:37:08   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.8   04 Aug 1994 14:21:38   JAY
// Fixed a bilevel problem (old code!).
// 
//    Rev 1.7   04 Aug 1994 07:33:20   JAY
// Large model and 32-bit changes
// 
//    Rev 1.6   18 Feb 1994 14:36:56   JAY
// Fixed an overflow in initialize_str]ip with large (printer output) @ 24 bpp.
// 
//    Rev 1.5   11 Feb 1994 08:07:56   JAY
// Doesn't add rc->destination_rect.y0 to the error diffusion variables.
// 
//    Rev 1.4   26 Jan 1994 16:33:56   JAY
// Now checks for negative as well as zero dimensions.
// 
//    Rev 1.3   19 Jan 1994 17:07:14   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:06:36   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:10:24   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:25:14   JAY
// 
*/

#include	"stdafx.h"

#include <stdlib.h>
#include <string.h>

#include	"pmgobj.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include "pagedefs.h"
#include	"utils.h"
#include	"paint.h"

/*
// Defines.
*/

#define	MAX_STRIP_SIZE	60000L
//#define	MAX_PRINT_SIZE	(MAX_STRIP_SIZE/8)
#define	MAX_PRINT_SIZE	MAX_STRIP_SIZE

extern const RGBQUAD* GetIdentityPalette(void);
extern CPalette *pOurPal;

/*
// The constructor for the paint context.
*/

PaintContext::PaintContext()
{
	no_expand_clip = FALSE;					// Kludge!!!
	render_method = ORDERED_DITHER;
	strip = NULL;
	m_pMask = NULL;
	m_fUseDest = TRUE;
	m_fHasMask = FALSE;
}

/*
// The destructor for the paint context.
*/

PaintContext::~PaintContext()
{
/* Make sure we're freed. */
	free_strip();
}

/*
// The default source extent computation.
// This merely sets the source extent to that of the destination (which is
// useful for outline objects).
*/

VOID PaintContext::compute_source_extent(VOID)
{
	source_width = dest_bound.right-dest_bound.left;
	source_height = dest_bound.bottom-dest_bound.top;
}

/*
// Compute the pre-paint variables.
*/

BOOL PaintContext::pre_paint(void)
{
/*
//	dest_bound = object extent at redisplay resolution 
*/
	PBOX pbox;

	pbox = bound;

	rc->pbox_to_screen(&pbox, TRUE);

	if (pbox.x1 - pbox.x0 > 32766 || pbox.y1 - pbox.y0 > 32766)
	{
	/* This graphic will be too big to handle. */
		return FALSE;
	}

	if (!rc->convert_pbox(&pbox, &dest_bound, NULL))
	{
	/* This graphic is clipped or too small. */
		return FALSE;
	}

	dest_width = dest_bound.right - dest_bound.left;
	dest_height = dest_bound.bottom - dest_bound.top;

/*
// Default the destination values to be the full destination rect.
// (No clipping yet.)
*/

	dest = dest_bound;

/* Set min and max to be what we have calculated so far. */

	min_dest_line = dest.top;
	max_dest_line = dest.bottom;

/* Compute the full source extent. For outline images, source = destination. */

	compute_source_extent();

/* Fill out the source rectangle. */

	srce.left = srce.top = 0;
	srce.right = source_width;
	srce.bottom = source_height;

/* Make sure we have something to do. */

	if (dest_width <= 0
			|| dest_height <= 0
			|| source_width <= 0
			|| source_height <= 0)
	{
	/* No image to paint. */
		return FALSE;
	}

/*
// Compute the y scaling information.
// This includes the y scaling factor and the size of the scaled pixel.
*/

	srce_factor.x = source_width;
	srce_factor.y = source_height;
	dest_factor.x = dest_width;
	dest_factor.y = dest_height;

/* Make sure they are good values. */

	/* X */

	while (srce_factor.x >= 0x4000 || dest_factor.x >= 0x4000)
	{
		if ((srce_factor.x >>= 1) == 0 || (dest_factor.x >>= 1) == 0)
		{
		/* Expansion or contraction is too great. */
			return FALSE;
		}
	}

	/* Y */

	while (srce_factor.y >= 0x4000 || dest_factor.y >= 0x4000)
	{
		if ((srce_factor.y >>= 1) == 0 || (dest_factor.y >>= 1) == 0)
		{
		/* Expansion or contraction is too great. */
			return FALSE;
		}
	}

/* Compute smoothed pixel sizes (approximate). */

	smooth_pixel_width = dest_factor.x/srce_factor.x;
	smooth_pixel_height = dest_factor.y/srce_factor.y;

/* Are we smoothing? */

	smoothing = (rc->smoothing == TRISTATE_ALL
						|| (rc->smoothing == TRISTATE_SELECTED
								&& (graphic_flags & GRAPHIC_FLAG_smoothed)))
				&& image_type != IMAGE_OUTLINE
				&& render_method != -1
				&& smooth_pixel_width >= 2
				&& smooth_pixel_height >= 2;

/* Compute the source and destination clipping extents. */

	if (clip != NULL)
	{
		RECT cr;

		cr = *clip;

		if (min_dest_line < cr.top)
		{
			min_dest_line = cr.top;
		}
		if (max_dest_line > cr.bottom)
		{
			max_dest_line = cr.bottom;
		}

		if (!no_expand_clip)						// Kludge!!!
		{
		/*
		// We aren't making an image. Do something special to make edges
		// line up right.
		*/
			if (render_method == ERROR_DIFFUSION)
			{
			/* Expand both sides since we alternate. */
				cr.left -= 2*smooth_pixel_width;
				cr.right += 2*smooth_pixel_width;
			}
			else if (smoothing)
			{
#if 0
				if (image_type == IMAGE_BILEVEL)
				{
					cr.left--;
					cr.top--;
					cr.right++;
					cr.bottom++;
				}
				else
#endif
				{
				/* Always processed from the left. */
					cr.left -= 2*smooth_pixel_width;
					cr.right++;
				}
			}
 
			if (smoothing || (do_yflip && render_method == ERROR_DIFFUSION))
			{
				if (do_yflip)
				{
					cr.top--;
					cr.bottom += 2*smooth_pixel_height;
				}
				else
				{
					cr.top -= 2*smooth_pixel_height;
					cr.bottom++;
				}
			}
		}

	/* Use the clipping rectangle. */

		dest.left = __max(cr.left, dest_bound.left);
		dest.top = __max(cr.top, dest_bound.top);
		dest.right = __min(cr.right, dest_bound.right);
		dest.bottom = __min(cr.bottom, dest_bound.bottom);

	/* If this is error diffusion and y flipped, reset min and max here. */

		if (do_yflip && render_method == ERROR_DIFFUSION)
		{
			min_dest_line = dest.top;
			max_dest_line = dest.bottom;
			if (*error_info != NULL)
			{
				(*error_info)->y = -1;
			}
		}

		// Recompute the source area.
		// WARNING! DANGER! Do not use MulDiv() here. MulDiv() rounds!

		srce.left = scale_number(dest.left - dest_bound.left,
											srce_factor.x,
											dest_factor.x);
		srce.right = scale_number(dest.right - dest_bound.left,
											srce_factor.x,
											dest_factor.x);
		srce.top = scale_number(dest.top - dest_bound.top,
											srce_factor.y,
											dest_factor.y);
		srce.bottom = scale_number(dest.bottom - dest_bound.top,
											srce_factor.y,
											dest_factor.y);
	}

	if (do_xflip)
	{
		int t = source_width - srce.left;
		srce.left = source_width - srce.right;
		srce.right = t;
	}

	if (do_yflip)
	{
		int t = source_height - srce.top;
		srce.top = source_height - srce.bottom;
		srce.bottom = t;
		current_dest_line = dest.bottom;
	}
	else
	{
		current_dest_line = dest.top;
	}

/* Set the current line. */

	current_source_line = srce.top;

/* Setup the scaling variables. */

	y_compare = srce_factor.y;
	y_delta = dest_factor.y;

#if 0
	od("cdl: %d; dest: %d, %d; dest_bound: %d, %d\r\n",
					current_dest_line,
					dest.top, dest.bottom,
					dest_bound.top, dest_bound.bottom);
	od("[%d, %d]\r\n", srce.top, srce.bottom);
#endif

	if (current_dest_line == (do_yflip ? dest_bound.bottom : dest_bound.top))
	{
		y_fraction = 0;
	}
	else
	{
		int d;

	/* Need to recompute the scaling variables for this partial strip. */

		if (do_yflip)
		{
			d = dest_bound.bottom-dest.bottom;
		}
		else
		{
			d = dest.top-dest_bound.top;
		}

	/* (Nifty empirically-derived calculations which I hope work.) */

		// WARNING! DANGER! Do not use MulDiv() here. MulDiv() rounds!
		srce.top = scale_number(d+1, srce_factor.y, dest_factor.y);
		if ((y_fraction = dest_factor.y
				- mulrem(d+1, srce_factor.y, dest_factor.y)) == dest_factor.y)
		{
			y_fraction = 0;
			srce.top--;
		}
		y_fraction += srce_factor.y - dest_factor.y;
	}

#if 0
	od("dest top:%d, bottom:%d; map:%d, %d\r\n",
						dest.top, dest.bottom,
						0,
						redisplay_destination_map->map_ypsize);

	od("%d, %d ; f:%d, s:%d, d:%d]\r\n", srce.top, srce.bottom,
						y_fraction,
						srce_factor.y, dest_factor.y);
#endif

	clipped_dest_width = dest.right - dest.left;
	clipped_dest_height = dest.bottom - dest.top;
	clipped_source_height = srce.bottom - srce.top;

	if (do_yflip)
	{
		min_dest_line++;
	}
	else
	{
		max_dest_line--;
	}

	return clipped_dest_height > 0
					&& clipped_source_height > 0;
}

#define	MAX_HEIGHT	80

/*
// Initialize the strip.
*/

BOOL PaintContext::initialize_strip(int x_size, BOOL fUseDest, BOOL use_clipped_x, BOOL fHasMask /*=FALSE*/)
{
   m_fUseDest = fUseDest;
	m_fHasMask = fHasMask;

	USHORT use_height;

	if (use_clipped_x)
	{
		dest_rect.left = dest.left;
		dest_rect.right = dest.right;
	}
	else
	{
		dest_rect.left = dest_bound.left;
		dest_rect.right = dest_bound.right;
	}

	if (fUseDest)
	{
		x_size = dest_rect.right - dest_rect.left;
		use_height = clipped_dest_height;
	}
	else
	{
		use_height = clipped_source_height;
	}

	if (x_size <= 0)
	{
		return FALSE;
	}

/*
// For Windows, we need to pad to a DWORD bound since we use the strip as
// DIB memory.
*/

	// This needs to cast to LONG in case we have large output with 24 bpp.
	strip_byte_width = ((SHORT)(((LONG)x_size*raster_info.bpp + 7)/8) + 3) & ~3;
//	od("**** Initialize strip Width:%d, sbw:%d\r\n", x_size, strip_byte_width);

/*
// Initialize the error diffusion variables.
// Note that we allocate two more than required so we don't need to do edge
// checking in the algorithm.
*/

	if (render_method == ERROR_DIFFUSION
			&& raster_info.type != RASTER_TYPE_DIRECT)
	{
//		SHORT current_dest_y = __max(dest.top, min_dest_line) + rc->destination_rect.top;
		SHORT current_dest_y = __max(dest.top, min_dest_line);

		if (*error_info != NULL)
		{
#if 0
			SHORT dy = srce.top - (*error_info)->y;
			if (dy < 0)
			{
				dy = -dy;
			}
#endif

			if ((*error_info)->x0 != dest.left
					|| (*error_info)->x1 != dest.right
					|| (*error_info)->y != current_dest_y
//					|| dy > 8
					|| (*error_info)->planes != raster_info.planes)
			{
#if 0
				od("*** Free it (%08lx, %08lx) (%d, %d @ %d & %d) ***\r\n",
								error_info,
								(*error_info),
								(*error_info)->x0,
								(*error_info)->x1,
								(*error_info)->y,
								(*error_info)->planes);
#endif
				delete [] (LPBYTE)(*error_info);
				*error_info = NULL;
			}
		}

		if (*error_info == NULL)
		{
			USHORT size = sizeof(ERROR_INFO)
							+ (USHORT)raster_info.error_bytes
								* (USHORT)raster_info.planes
								* (USHORT)((x_size+2)*sizeof(SHORT));

			TRY
				*error_info = (ERROR_INFO_PTR)new BYTE[size];
			END_TRY
			if (*error_info == NULL)
			{
				render_method = ORDERED_DITHER;
			}
			else
			{
			/* Initialize it now. */

				memset(*error_info, 0, size);

				(*error_info)->x0 = dest.left;
				(*error_info)->x1 = dest.right;
				(*error_info)->y = current_dest_y;
				(*error_info)->planes = raster_info.planes;
				(*error_info)->lineno = 0;
#if 0
				od("### Alloced (%d, %d @ %d & %d) ###\r\n",
									(*error_info)->x0,
									(*error_info)->x1,
									(*error_info)->y,
									(*error_info)->planes);
#endif
			}
		}
#if 0
		od("allocated %d for error rows\r\n", size);
#endif
	}

	/* Allocate the strip to use. */

	LONG max_strip_size = rc->IsScreen() ? MAX_STRIP_SIZE : MAX_PRINT_SIZE;

	USHORT max_height = (unsigned int) (max_strip_size / ((long)strip_byte_width*(long)raster_info.planes));

#if 1
	if (rc->IsScreen()
 				&& image_type != IMAGE_OUTLINE
 				&& max_height > MAX_HEIGHT)
	{
		max_height = MAX_HEIGHT;
	}
#endif

	if (rc->is_metafile)
	{
		// Try to use full height to prevent banding.
		strip_height = use_height;
	}
	else
	{
		strip_height = __min(use_height, max_height);
	}

	do
	{
		TRY
		{
			strip = new BYTE[strip_byte_width*strip_height*raster_info.planes + 4];
		}
		CATCH_ALL(e)
		{
			// Try a smaller size.
			strip_height >>= 1;
		}
		END_CATCH_ALL
	} while ((strip == NULL) && (strip_height != 0));

//	od("strip size:%u\r\n",
//				strip_byte_width*strip_height*raster_info.planes+4);

	if (strip != NULL)
	{
		if (m_fHasMask)
		{
			// Allocate the mask strip.

			// Compute the width of the mask.
			m_nMaskByteWidth = ((x_size + 7)/8 + 3) & ~3;

			// We assume that strip_byte_width >= m_nMaskByteWidth.
			// That is, if strip size < 64K, then mask strip size < 64K, too.
			ASSERT(strip_byte_width >= m_nMaskByteWidth);

			// Allocate it. Either we succeed or fail (no retries).
			TRY
			{
				m_pMask = new BYTE[m_nMaskByteWidth*strip_height + 4];
			}
			END_TRY
			if (m_pMask == NULL)
			{
				TRACE("initialize_strip: Mask allocation failed.\n");
				return FALSE;
			}
		}

		// Compute strip height and blit parameters.
		n_strips = (use_height + strip_height - 1)/strip_height;

		// Compute the sizes of the last source and dest strip.
		if ((last_strip_height = use_height - strip_height*(n_strips-1)) == 0)
		{
			last_strip_height = strip_height;
		}

		// Initialize the map we will use.
		build_graphic_map(x_size);

		// Initialize the end of last strip.
		if (do_yflip)
		{
			dest_rect.top = dest.bottom;
			next_strip_row = -strip_byte_width;
			m_nNextMaskRow = -m_nMaskByteWidth;
		}
		else
		{
			dest_rect.bottom = dest.top;
			next_strip_row = strip_byte_width;
			m_nNextMaskRow = m_nMaskByteWidth;
		}

		return TRUE;
	}

	return FALSE;
}

/*
// Build the graphic map from the strip parameters.
*/

void PaintContext::build_graphic_map(int x_size)
{
	RASTER_TYPE type = raster_info.type;

	// Initialize the DIB header.
	memset(&bitmap_info.bmiHeader, 0, sizeof(bitmap_info.bmiHeader));

	bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
	bitmap_info.bmiHeader.biWidth = x_size;
	bitmap_info.bmiHeader.biHeight = strip_height;
	bitmap_info.bmiHeader.biPlanes = 1;			/* Says "always". */
	bitmap_info.bmiHeader.biBitCount = raster_info.planes*raster_info.bpp;

	// Construct the palette for the DIB.

//	switch (GetRasterType())
	switch (raster_info.type)
	{
		case RASTER_TYPE_BILEVEL:
		{
			bitmap_info.bmiColors[0].rgbBlue =
			bitmap_info.bmiColors[0].rgbRed =
			bitmap_info.bmiColors[0].rgbGreen = 0;
			bitmap_info.bmiColors[0].rgbReserved = 0;

			bitmap_info.bmiColors[1].rgbBlue =
			bitmap_info.bmiColors[1].rgbRed =
			bitmap_info.bmiColors[1].rgbGreen = 255;
			bitmap_info.bmiColors[1].rgbReserved = 0;
			break;
		}
		case RASTER_TYPE_PALETTE:
		{
			SHORT far *q, i;

			/*
 			// Fix up the bitmap header colors.
			// Use a one-to-one map. 
 			*/

			for (i = 0, q = (SHORT far *)bitmap_info.bmiColors; i < 256; )
			{
				*q++ = i++;
			}
			break;
		}
		case RASTER_TYPE_8:
		{
			RGBQUAD far *q;
			int i;

			for (i = 0, q = bitmap_info.bmiColors; i < 8; i++, q++)
			{
				q->rgbRed = (i & 4) ? 255 : 0;
				q->rgbGreen = (i & 2) ? 255 : 0;
				q->rgbBlue = (i & 1) ? 255 : 0;
				q->rgbReserved = 0;
			}

			break;
		}
		case RASTER_TYPE_256:
		{
		/* Build a palette matching our PALETTE type. */

			memcpy(bitmap_info.bmiColors, GetIdentityPalette(), sizeof(RGBQUAD)*256);

			break;
		}
		default:
		{
			break;
		}
	}

	// Setup the mask info (for the StretchDIBits).
	if (m_fHasMask)
	{
		memset(&m_MaskInfo.bmiHeader, 0, sizeof(m_MaskInfo.bmiHeader));

		m_MaskInfo.bmiHeader.biSize = sizeof(m_MaskInfo.bmiHeader);
		m_MaskInfo.bmiHeader.biWidth = x_size;
   	m_MaskInfo.bmiHeader.biHeight = strip_height;
		m_MaskInfo.bmiHeader.biPlanes = 1;
		m_MaskInfo.bmiHeader.biBitCount = 1;		// For mono map.

		m_MaskInfo.bmiColors[0].rgbBlue =
		m_MaskInfo.bmiColors[0].rgbRed =
		m_MaskInfo.bmiColors[0].rgbGreen = 0;
		m_MaskInfo.bmiColors[0].rgbReserved = 0;

		m_MaskInfo.bmiColors[1].rgbBlue =
		m_MaskInfo.bmiColors[1].rgbRed =
		m_MaskInfo.bmiColors[1].rgbGreen = 255;
		m_MaskInfo.bmiColors[1].rgbReserved = 0;
	}
}

/*
// Free the strip.
*/

void PaintContext::free_strip(void)
{
	if (strip != NULL)
	{
		delete [] strip;
		strip = NULL;
	}

	if (m_pMask != NULL)
	{
		delete [] m_pMask;
		m_pMask = NULL;
	}

/* Advance the appropriate variables. */

	if (render_method == ERROR_DIFFUSION)
	{
//		SHORT d = current_dest_line + rc->destination_rect.top;
		SHORT d = current_dest_line;

//		printf("current_dest_line: %d, max:%d\n",
//						d, redisplay_destination_rect->bottom);

		if (*error_info != NULL)
		{
			(*error_info)->y = __min(d, rc->destination_rect.bottom);
		}
	}
//	printf("last y_fraction: %d\n", y_fraction);
}

/*
// Compute the strip values for strip i;
*/

void PaintContext::compute_strip(int i)
{
/* Start at the end of the last strip. */

	if (do_yflip)
	{
		dest_rect.bottom = dest_rect.top;
	}
	else
	{
		dest_rect.top = dest_rect.bottom;
	}

/* If the last strip, adjust accordingly. */

	if (i == n_strips-1)
	{
		bitmap_info.bmiHeader.biHeight = last_strip_height;
		m_MaskInfo.bmiHeader.biHeight = last_strip_height;

		if (do_yflip)
		{
			dest_rect.top = dest.top;
		}
		else
		{
			dest_rect.bottom = dest.bottom;
		}
	}
	else
	{
		int temp = (i+1)*strip_height;

		if (!m_fUseDest)
		{
			temp = scale_number(temp, dest_height, source_height);
		}

		if (do_yflip)
		{
			dest_rect.top = dest.bottom - temp;
		}
		else
		{
			dest_rect.bottom = dest.top + temp;
		}
	}
}

/*
// Check if an interrupt has happened.
*/

IMAGE_CODE PaintContext::check_interrupt(VOID)
{
	switch (rc->check_interrupt())
	{
		case TRUE:
		{
			return IMAGE_CODE_INTERRUPTED;
		}
		case -1:
		{
			return IMAGE_CODE_TERMINATED;
		}
		default:
		{
			return IMAGE_CODE_CONTINUE;
		}
	}
}

// Get the raster type, translating RASTER_TYPE_DIRECT.
int PaintContext::GetRasterType(void)
{
	int nType = raster_info.type;

	if (nType == RASTER_TYPE_DIRECT)
	{
	// Dest matches source.
		switch (image_type)
		{
#if 0
			case IMAGE_BILEVEL:							/* 1 bit per pixel (1x1) */
			{
				nType = RASTER_TYPE_BILEVEL;
				break;
			}
#else
			case IMAGE_BILEVEL:							/* converted to (8x1) */
#endif
			case IMAGE_PALETTE:							/* n bits per pixel (nx1) */
			{
				nType = RASTER_TYPE_PALETTE;
				break;
			}
			case IMAGE_FULL_COLOR:						/* 24 bits per pixel (8x3) */
			{
				nType = RASTER_TYPE_24;
				break;
			}
			default:
			{
				ASSERT(FALSE);
				break;
			}
		} 
	}
	return nType;
}

void PaintContext::show_graphic_map(RECT rdest, int nSrcY, int nSrcLines)
{
	int nDestWidth = rdest.right - rdest.left;
	int nDestHeight = rdest.bottom - rdest.top;

	od("SetDIBitsToDevice %d, %d, %d, %d ", rdest);

	bitmap_info.bmiHeader.biSizeImage = (DWORD)strip_byte_width * nSrcLines;
	m_MaskInfo.bmiHeader.biSizeImage = (DWORD)m_nMaskByteWidth * nSrcLines;

	od("biHeight: %ld, lines: %d\r\n",
		bitmap_info.bmiHeader.biHeight,
		nDestHeight);

	HDC hdc = rc->destination_hdc;
	HDC hAttributeDC = rc->AttributeDC();
	BOOL fDrew = FALSE;

	if (rc->IsScreen() && m_fHasMask)
	{
		// In order to prevent flicker from the two DSTINVERTs, we want to
		// compose the image off-screen.
		HDC hMemDC;

		if ((hMemDC = ::CreateCompatibleDC(hAttributeDC)) != NULL)
		{
			HPALETTE hOldPalette = ::SelectPalette(hMemDC, (HPALETTE)pOurPal->GetSafeHandle(), TRUE);

			// Create the bitmap we will be using.
			HBITMAP hBitmap = ::CreateCompatibleBitmap(hAttributeDC,
																	 nDestWidth,
																	 nDestHeight);
			if (hBitmap != NULL)
			{
				HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
				if (hOldBitmap != NULL)
				{
					// Blit from the screen to the map.
					::SetStretchBltMode(hMemDC, COLORONCOLOR);
					rc->toggle(FALSE, &rdest);

					::BitBlt(hMemDC,
								0,
								0,
								nDestWidth,
								nDestHeight,
								hdc,
								rdest.left,
								rdest.top,
								SRCCOPY);

					// Xor the screen and the image.
					StretchDIBits(hMemDC, 
									  0,									// DstX
									  0,									// DstY
									  nDestWidth,									// DstWidth
									  nDestHeight,									// DstHeight
									  0,												// SrcX
									  nSrcY,											// SrcY
									  (int)bitmap_info.bmiHeader.biWidth,	// SrcWidth
									  nSrcLines,									// SrcHeight
									  strip,
									  (BITMAPINFO far *)&bitmap_info,
									  raster_info.type == RASTER_TYPE_PALETTE
											? DIB_PAL_COLORS
											: DIB_RGB_COLORS,
									  SRCINVERT);

					// Mask out the masked parts.
					StretchDIBits(hMemDC, 
									  0,												// DstX
									  0,												// DstY
									  nDestWidth,									// DstWidth
									  nDestHeight,									// DstHeight
									  0,												// SrcX
									  nSrcY,											// SrcY
									  (int)m_MaskInfo.bmiHeader.biWidth,	// SrcWidth
									  nSrcLines,									// SrcHeight
									  m_pMask,
									  (BITMAPINFO far *)&m_MaskInfo,
									  DIB_RGB_COLORS,
									  SRCAND);

					// Xor the screen and image again.
					StretchDIBits(hMemDC, 
									  0,												// DstX
									  0,												// DstY
									  nDestWidth,									// DstWidth
									  nDestHeight,									// DstHeight
									  0,												// SrcX
									  nSrcY,											// SrcY
									  (int)bitmap_info.bmiHeader.biWidth,	// SrcWidth
									  nSrcLines,									// SrcHeight
									  strip,
									  (BITMAPINFO far *)&bitmap_info,
									  raster_info.type == RASTER_TYPE_PALETTE
											? DIB_PAL_COLORS
											: DIB_RGB_COLORS,
									  SRCINVERT);

					// Transfer the final image to the screen.
					::BitBlt(hdc,
								rdest.left,
								rdest.top,
								nDestWidth,
								nDestHeight,
								hMemDC,
								0,
								0,
								SRCCOPY);
					rc->toggle(TRUE, &rdest);
					fDrew = TRUE;
					::SelectObject(hMemDC, hOldBitmap);
				}
				::DeleteObject(hBitmap);
			}

			if (hOldPalette != NULL)
			{
				::SelectPalette(hMemDC, hOldPalette, TRUE);
			}
			DeleteDC(hMemDC);
		}
	}

	if (!fDrew)
	{
		if (rc->IsScreen())
		{
			// This fixes a Windows bug where StretchDIBits behind a dialog
			// does not mark the dialog's saved bits as modified; when the dialoh
			// goes away, the old image comes back. The FillRect (even with a
			// NULL brush) marks it as modified the way it's supposed to.
			HBRUSH hBrush;
			if ((hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH)) != NULL)
			{
				::FillRect(rc->destination_hdc, &rdest, hBrush);
			}
		}

#if 0
		od("DIB (%d, %d; %d, %d) to (%d, %d; %ld, %d) (%ld, %ld) (%ld)\r\n",
				rdest.left,
				rdest.top,
				nDestWidth,
				nDestHeight,
				0,												// SrcX
				nSrcY,										// SrcY
				bitmap_info.bmiHeader.biWidth,		// SrcWidth
				nSrcLines,									// SrcHeight
				bitmap_info.bmiHeader.biWidth,
				bitmap_info.bmiHeader.biHeight,
				bitmap_info.bmiHeader.biSizeImage);
#endif

		rc->toggle(FALSE, &rdest);

		DWORD dwROP = SRCCOPY;
		int old_sbm = ::GetStretchBltMode(hdc);

		// Process the mask data.
		if (m_fHasMask)
		{
			dwROP = SRCINVERT;
			ASSERT(m_pMask != NULL);
			::SetStretchBltMode(hdc, COLORONCOLOR);
			StretchDIBits(hdc, 
							  rdest.left,									// DstX
							  rdest.top,									// DstY
							  nDestWidth,									// DstWidth
							  nDestHeight,									// DstHeight
							  0,												// SrcX
							  nSrcY,											// SrcY
							  (int)bitmap_info.bmiHeader.biWidth,	// SrcWidth
							  nSrcLines,									// SrcHeight
							  strip,
							  (BITMAPINFO far *)&bitmap_info,
							  raster_info.type == RASTER_TYPE_PALETTE
									? DIB_PAL_COLORS
									: DIB_RGB_COLORS,
							  dwROP);
			::SetStretchBltMode(hdc, old_sbm);

			StretchDIBits(hdc, 
							  rdest.left,									// DstX
							  rdest.top,									// DstY
							  nDestWidth,									// DstWidth
							  nDestHeight,									// DstHeight
							  0,												// SrcX
							  nSrcY,											// SrcY
							  (int)m_MaskInfo.bmiHeader.biWidth,	// SrcWidth
							  nSrcLines,									// SrcHeight
							  m_pMask,
							  (BITMAPINFO far *)&m_MaskInfo,
							  DIB_RGB_COLORS,
							  SRCAND);
		}

		// Process the image data.
		::SetStretchBltMode(hdc, COLORONCOLOR);
		StretchDIBits(hdc, 
						  rdest.left,									// DstX
						  rdest.top,									// DstY
						  nDestWidth,									// DstWidth
						  nDestHeight,									// DstHeight
						  0,												// SrcX
						  nSrcY,											// SrcY
						  (int)bitmap_info.bmiHeader.biWidth,	// SrcWidth
						  nSrcLines,									// SrcHeight
						  strip,
						  (BITMAPINFO far *)&bitmap_info,
						  raster_info.type == RASTER_TYPE_PALETTE
									? DIB_PAL_COLORS
									: DIB_RGB_COLORS,
						  dwROP);
		::SetStretchBltMode(hdc, old_sbm);
		rc->toggle(TRUE, &rdest);
	}
}
