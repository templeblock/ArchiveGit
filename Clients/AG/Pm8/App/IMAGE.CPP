/*
//	IMAGE.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Generic image handling routines.
//
// $Header: /PM8/App/IMAGE.CPP 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/IMAGE.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
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
//    Rev 1.1   17 Sep 1997 09:12:30   Jay
// Bitmap rotation
// 
//    Rev 1.0   14 Aug 1997 15:21:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:46   Fred
// Initial revision.
// 
//    Rev 1.16   20 Jul 1997 14:36:42   Fred
// Does not allow handle wrap when cropping; handles empty clip regions
// 
//    Rev 1.15   02 Jul 1997 14:25:50   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.14   12 Jun 1997 17:10:30   Jay
// Fixed masked image problems
// 
//    Rev 1.13   22 May 1997 10:30:22   Jay
// New Pegasus code
// 
//    Rev 1.12   30 Apr 1997 10:46:14   Jay
// Optional TRUE for DirectBitmapPrinting.
// 
//    Rev 1.11   11 Apr 1997 11:23:32   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.10   19 Feb 1997 16:54:04   Jay
// Beginning of cropping
// 
//    Rev 1.9   09 Dec 1996 10:19:30   Jay
// More image stuff.
// 
//    Rev 1.8   06 Dec 1996 09:15:42   Jay
// More transparent bitmap stuff.
// 
//    Rev 1.7   05 Dec 1996 12:30:10   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.6   05 Nov 1996 14:06:50   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.5   04 Nov 1996 16:06:04   Jay
// New object image routines.
// 
//    Rev 1.4   01 Nov 1996 17:11:40   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.3   16 Oct 1996 13:05:16   Jay
// Fixed direct 24-bit graphic with color.
// 
//    Rev 1.2   15 Oct 1996 08:54:32   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.1   07 Aug 1996 10:12:12   Jay
// Fixed a bug in show_current_map().
// 
//    Rev 1.0   14 Mar 1996 13:42:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:05:56   FRED
// Initial revision.
// 
//    Rev 2.7   22 Dec 1995 08:00:44   JAY
// Variable used without being initialized!
// 
//    Rev 2.6   20 Nov 1995 09:04:36   JAY
// 
//    Rev 2.5   12 Oct 1995 15:43:04   JAY
// Fixed an interrupt update bug introduced by the fast bitmap printing code.
// 
//    Rev 2.4   09 Oct 1995 15:15:04   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.3   19 Sep 1995 14:03:20   JAY
// 
//    Rev 2.2   22 Jun 1995 16:06:50   JAY
// Fixed a typo in the 24-bit renderer that caused apparent banding.
// 
//    Rev 2.1   29 Mar 1995 08:58:40   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:47:52   JAY
// Initial revision.
// 
//    Rev 1.17   30 Dec 1994 14:00:00   JAY
// New dither method with expanded color set (based on WinG palette).
// 
//    Rev 1.16   30 Nov 1994 16:38:06   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.15   04 Aug 1994 14:21:06   JAY
// A debugging message.
// 
//    Rev 1.14   04 Aug 1994 07:32:48   JAY
// Large model and 32-bit changes
// 
//    Rev 1.13   09 Jun 1994 11:07:40   JAY
// Now checks for DoesNotExist in addition to Open ERRORCODE.
// 
//    Rev 1.12   26 May 1994 13:41:08   JAY
// Now handles OPEN error correctly.
// 
//    Rev 1.11   16 May 1994 08:42:32   JAY
// Broke the RC gamma curve into two separate curves, one for bitmaps and one
// for outlines.
// 
//    Rev 1.10   02 Mar 1994 11:59:44   JAY
// Now calls draw_mono_map() even on PostScript printers.
// 
//    Rev 1.9   02 Mar 1994 11:18:32   JAY
// Added support for draw_mono_map() to fix problem with HP 4/4M.
// 
//    Rev 1.8   22 Feb 1994 15:02:30   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.7   10 Feb 1994 15:35:18   JAY
// Fixed bug in setup_translate_tables (and elsewhere) relating to rc gamma
// curve and also BMP red/blue flip.
// 
//    Rev 1.6   01 Feb 1994 10:20:16   JAY
// Added dummy FillRect (with NULL_BRUSH) on solo StretchDIBits to fix a bug
// in Windows where dialogs don't know a StretchDIBits occurred behind them.
// 
//    Rev 1.5   31 Jan 1994 07:14:58   JAY
// Removed od('s for release version.
// 
//    Rev 1.4   19 Jan 1994 17:06:46   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:00:24   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:05:50   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:10:12   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:25:02   JAY
// 
*/

#include	"stdafx.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <malloc.h>
#include	<string.h>

#include	"pmgobj.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include "pagedefs.h"
#include	"utils.h"
#include	"image.h"
#include	"dither.h"
#include	"objimg.h"

#define DO_ROTATION

//PUBLIC RENDER_METHOD global_render_method = ORDERED_DITHER;
PUBLIC RENDER_METHOD global_render_method = ERROR_DIFFUSION;

#define	NOTIFY_WAIT		250L						// 1/4 second

PRIVATE ULONG next_notify_time;
PUBLIC VOID (*image_update_hook)(SHORT current, SHORT total) = NULL;
extern CPalette *pOurPal;

/////////////////////////////////////////////////////////////////////////////
// IMAGE_INFO() constructor

IMAGE_INFO::IMAGE_INFO()
{
	memset(this, 0, sizeof(*this));
}

/////////////////////////////////////////////////////////////////////////////
// draw_mono_map

// The asm code below was converted to C code by Roger House 96/02/23.
// Well, not exactly.  The C code was taken from condense_bitmap in
// pmgfont.cpp.  See that file for a comment.
//
// condense_bitmap()
//
// Take a bitmap that has horizontal padding on the right side, and
// copy it to a destination bitmap, eliminating the padding.
//
// Arguments:
//	void *source		Pointer to first byte of source bitmap
//	void *dest		Pointer to first byte of destination bitmap
//	int nrows		Number of rows to copy
//	int rowbytes		Bytes per row in destination (unpadded)
//	int source_pad		Number of pad bytes in each row of source
//
// It is allowed to have source=dest.
//

static void near condense_bitmap(
	LPBYTE	source,
	LPBYTE	dest,
	int	nrows,
	int	rowbytes,
	int	source_pad)
{
#ifdef WIN32

#ifdef _NEWPORTABLE

	for (SHORT i = 0; i < nrows; i++)
	{
		memcpy(dest, source, rowbytes);
		dest += rowbytes;
		source += rowbytes + source_pad;
	}

	return;

#else // _NEWPORTABLE

   _asm {
		mov	esi, source
		mov	edi, dest
		mov	edx, nrows
		mov	ecx, rowbytes
		mov	eax, source_pad
		mov	ebx, ecx				// Save rowbytes in BX
	L_top:
		// Any rows left to do?
		or		edx, edx
		jz		L_done

		// Copy a row
		mov	ecx, ebx
		shr	ecx, 1
		rep	movsw
		adc	ecx, ecx
		rep	movsb

		// Move to the next row
		add	esi, eax
		dec	edx
		jmp	L_top
	L_done:
	}

#endif	// _NEWPORTABLE

#else // WIN32
	_asm {
		push	ds
		cld
		lds	si, source
		les	di, dest
		mov	dx, nrows
		mov	cx, rowbytes
		mov	ax, source_pad
		mov	bx, cx				// Save rowbytes in BX
	L_top:
		// Any rows left to do?
		or		dx, dx
		jz		L_done

		// Copy a row
		mov	cx, bx
		shr	cx, 1
		rep	movsw
		adc	cx, cx
		rep	movsb

		// Move to the next row
		add	si, ax
		dec	dx
		jmp	L_top
	L_done:
		pop	ds
	}

#endif	// else WIN32
} // end condense_bitmap



PUBLIC BOOL
draw_mono_map(HDC hdc, RECT r, LPRECT src_r, BITMAPINFO far *info, LPBYTE data, RedisplayContextPtr rc, DWORD rop)
{
	HDC src_hdc;
	BOOL ret = FALSE;
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;
	int bm_width = (int)info->bmiHeader.biWidth;
	int bm_height = (int)info->bmiHeader.biHeight;
	UINT dib_byte_width = (bm_width + 7)/8;
	UINT byte_width = (dib_byte_width + 1) & ~1;
	dib_byte_width = (dib_byte_width + 3) & ~3;

	UINT map_size = byte_width * bm_height;

	if (rc->is_postscript && rop != SRCCOPY)
	{
		SetTextColor(hdc, RGB(255, 255, 255));
	}

	LPBYTE new_map = NULL;
	TRY
		new_map = new BYTE[map_size];
	END_TRY
	if (new_map != NULL)
	{
	/* By setting it up this way, the bitmap is copied upside-down. */
		condense_bitmap(data + dib_byte_width*(bm_height-1),
								new_map,
								bm_height,
								byte_width,
								-(SHORT)(byte_width + dib_byte_width));

		if ((src_hdc = ::CreateCompatibleDC(hdc)) != NULL)
		{
			HBITMAP hBitmap;

			if ((hBitmap = ::CreateBitmap(bm_width, bm_height, 1, 1, new_map)) != NULL)
			{
				HBITMAP hOldBitmap;

				if ((hOldBitmap = (HBITMAP)::SelectObject(src_hdc, hBitmap)) != NULL)
				{
//					od("BitBlt: %d, %d, %d, %d (src:%d, %d, %d, %d, %d)\r\n", r, *src_r);

					::BitBlt(hdc,
	 							r.left,
	 							r.top,
	 							r.right-r.left,
	 							r.bottom-r.top,
								src_hdc,
								src_r->left,
								src_r->top,
								rop);
					ret = TRUE;
					::SelectObject(src_hdc, hOldBitmap);
				}
				::DeleteObject(hBitmap);
			}
			::DeleteDC(src_hdc);
		}
		delete [] new_map;
	}

	if (rc->is_postscript && rop != SRCCOPY)
	{
		SetTextColor(hdc, RGB(0, 0, 0));
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// ImageContext

/* For all to use. */

ImageContext::ImageContext(ImageSourcePtr s)
			: source(s)
{
	memset(translate_table, 0, sizeof(translate_table));
	memset(&smooth, 0, sizeof(smooth));
	source_row = NULL;
	m_pMaskRow = NULL;

	if (source != NULL)
	{
		source->attach_image_context(this);
	}
}

/*
// The destructor for an image context.
*/

ImageContext::~ImageContext()
{
/* Free the palette before continuing. */

	free_smoothing();
	free_render_buffers();
}

/*
// Set the render method based on the passed variable.
// All ERROR_DIFFUSION_? types translate to ERROR_DIFFUSION with corresponding
// values set.
*/

VOID PaintContext::set_render_method(RENDER_METHOD method)
{
	if ((render_method = method) > ERROR_DIFFUSION)
	{
		render_method = ERROR_DIFFUSION;
	}
}

/*
// Free the render buffer variables.
*/

VOID ImageContext::free_render_buffers(VOID)
{
	SHORT i;

	if (raster_info.type != RASTER_TYPE_DIRECT)
	{
		if (raster_info.planes != 1 || image_info.image_type == IMAGE_FULL_COLOR)
		{
			for (i = 0; i < raster_info.planes; i++)
			{
				delete [] translate_table[i];
			}
		}
	}

	delete [] palette.m_pData;
	delete [] source_row;
	delete [] m_pMaskRow;
}

/*
// Initialize rendering buffers and other neat stuff.
*/

BOOL ImageContext::init_render_buffers(VOID)
{
	SHORT translate_size;

/* Allocate the source row. */

#if 0
	od("Source row size: %d (w:%d, h:%d)\r\n",
				image_info.row_bytes,
				image_info.width,
				image_info.height);
#endif

	TRY
		source_row = new BYTE[(image_info.row_bytes+3) & ~3];
	END_TRY
	if (source_row == NULL)
	{
		return FALSE;
	}

	if (image_info.m_fHasMask)
	{
		TRY
			m_pMaskRow = new BYTE[image_info.m_nMaskRowBytes];
		END_TRY
		if (m_pMaskRow == NULL)
		{
			return FALSE;
		}
	}

/* Allocate the palette now. */

	if (image_info.image_type != IMAGE_FULL_COLOR)
	{
		palette.m_pData = NULL;
		TRY
			palette.m_pData = new RGBTRIPLE[palette.m_nColors];
		END_TRY
		if (palette.m_pData == NULL)
		{
			return FALSE;
		}
	}

	translate_size = image_info.image_type == IMAGE_FULL_COLOR
									? source_width
									: palette.m_nColors;

/* Allocate a translation table for each plane. */

	if (raster_info.type == RASTER_TYPE_DIRECT)
	{
		translate_table[0] = NULL;
	}
	else if (raster_info.planes == 1 && image_info.image_type != IMAGE_FULL_COLOR)
	{
	/* Use the static table memory. */
		translate_table[0] = gray_translate_table;
	}
	else
	{
		SHORT i;
	/* Allocate a table for each plane. */

		for (i = 0; i < raster_info.planes; i++)
		{
			translate_table[i] = NULL;
			TRY
				translate_table[i] = new BYTE[translate_size];
			END_TRY
			if (translate_table[i] == NULL)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*
// Initialize smoothing variables.
*/

BOOL ImageContext::init_smoothing(VOID)
{
	if (smoothing)
	{
		smoothing_size = image_info.row_bytes*raster_info.planes;

		if (image_info.image_type != IMAGE_FULL_COLOR)
		{
			smoothing_size *= raster_info.error_bytes;
		}

	/* Allocate all the smoothing rows. */

		TRY
			smooth.value = new BYTE[smoothing_size];
		END_TRY
		if (smooth.value != NULL)
		{
		/* Initial values are zero. */

			memset(smooth.value, 0, smoothing_size);

			TRY
			{
				smooth.delta = new BYTE[smoothing_size];
				smooth.fraction = new BYTE[smoothing_size];
				smooth.final = new BYTE[smoothing_size];
				smooth.inc = new char[smoothing_size];
			}
			END_TRY

			if (smooth.delta != NULL
				&& smooth.fraction != NULL
				&& smooth.final != NULL
				&& smooth.inc != NULL)
			{
				return TRUE;
			}

		/* Free anything we allocated. */

			free_smoothing();
		}

	/* Continue unsmoothed! */

		smoothing = FALSE;
	}
	return TRUE;
}

VOID ImageContext::free_smoothing(VOID)
{
	if (smoothing)
	{
		delete [] smooth.value;
		smooth.value = NULL;
		delete [] smooth.delta;
		smooth.delta = NULL;
		delete [] smooth.fraction;
		smooth.fraction = NULL;
		delete [] smooth.final;
		smooth.final = NULL;
		delete [] smooth.inc;
		smooth.inc = NULL;
	}
}

/*
// Setup the translation tables.
*/

void ImageContext::setup_translate_tables(COLOR_COMPONENT far *rgbdata, SHORT data_size, BOOL fReversed /*= FALSE*/)
{
	RGBCOLOR white_color, black_color;
	COLOR_COMPONENT far *p;
	UCHAR trans_table[MAX_COLOR_PLANES+1];
	SHORT i;
	SHORT red_index = image_info.red_index, blue_index = red_index ^ 2;
	COLOR object_color = object->get_color(),
 			object_bcolor = object->get_bcolor();
	SHORT nDstRedIndex = 0;
	SHORT nDstBlueIndex = 2;
	if (fReversed)
	{
		nDstRedIndex ^= 2;
		nDstBlueIndex ^= 2;
	}

/*
// Adjust the values according to the gamma correction.
*/

/*
// Now we must munge the palette according to the color and bcolor of
// the graphic object. We basically map black to the "bcolor", white to
// the "color", and everything else to linear in-between values.
*/

	white_color = RGBCOLOR_FROM_COLOR(object_color);
	black_color = RGBCOLOR_FROM_COLOR(object_bcolor);

	if (white_color != RGBCOLOR_WHITE || black_color != RGBCOLOR_BLACK)
	{
		SHORT red_min, red_delta, green_min, green_delta, blue_min, blue_delta;

	/* We must do the remapping. */

		red_min = (SHORT)RED_COMPONENT_OF(black_color);
		green_min = (SHORT)GREEN_COMPONENT_OF(black_color);
		blue_min = (SHORT)BLUE_COMPONENT_OF(black_color);

		red_delta = (SHORT)RED_COMPONENT_OF(white_color) - red_min;
		green_delta = (SHORT)GREEN_COMPONENT_OF(white_color) - green_min;
		blue_delta = (SHORT)BLUE_COMPONENT_OF(white_color) - blue_min;

		for (p = rgbdata, i = 0; i < data_size; i++, p += sizeof(RGBTRIPLE))
		{
			p[red_index] = (BYTE)(red_min + scale_number(p[red_index], red_delta, 255));
			p[1] = (BYTE)(green_min + scale_number(p[1], green_delta, 255));
			p[blue_index] = (BYTE)(blue_min + scale_number(p[blue_index], blue_delta, 255));
		}
	}

/* Construct the translation tables, one for each destination plane. */

	for (p = rgbdata, i = 0; i < data_size; i++)
	{
		trans_table[red_index+1] = rc->bitmap_gamma_curve[p[0]];
		trans_table[2] = rc->bitmap_gamma_curve[p[1]];
		trans_table[blue_index+1] = rc->bitmap_gamma_curve[p[2]];

	/* Translate from RGB to destination planes. */

		if (raster_info.type == RASTER_TYPE_BILEVEL)
		{
			*(translate_table[0]+i) = rgbcolor_to_gray(MAKE_RGBCOLOR(
							trans_table[1], trans_table[2], trans_table[3]));
		}
		else
		{
			p[nDstRedIndex] = trans_table[1];
			p[1] = trans_table[2];
			p[nDstBlueIndex] = trans_table[3];
		}
		p += 3;
	}
}

VOID ImageContext::set_current_strip(VOID)
{
	compute_strip(current_strip);
	strip_row = 0;
	strip_offset = 0;
	m_uMaskOffset = 0;

	if (next_strip_row < 0)
	{
		next_strip_row = -next_strip_row;
	}

	if (m_nNextMaskRow < 0)
	{
		m_nNextMaskRow = -m_nNextMaskRow;
	}

	if (!do_yflip)
	{
		// Start at the last row in the map.
		strip_offset += (bitmap_info.bmiHeader.biHeight-1)*strip_byte_width;
		next_strip_row = -next_strip_row;

		// Do the same for the mask.
		if (m_fHasMask)
		{
			m_uMaskOffset += (bitmap_info.bmiHeader.biHeight-1)*m_nMaskByteWidth;
			m_nNextMaskRow = -m_nNextMaskRow;
		}
	}
}

/*
// Initialize the image update stuff.
// Returns whether or not to bother.
*/

PUBLIC BOOL
init_image_update(VOID)
{
	if (image_update_hook != NULL)
	{
		next_notify_time = GetTickCount() + NOTIFY_WAIT;
		return TRUE;
	}
	return FALSE;
}

/*
// Notify whomever that we are updating the image.
*/

PUBLIC VOID
notify_image_update(SHORT current, SHORT total)
{
	DWORD now = GetTickCount();

	if (current > total)
	{
		current = total;
	}
	if (now >= next_notify_time || current == total)
	{
		image_update_hook(current, total);
		next_notify_time = now + NOTIFY_WAIT;
	}
}

/*
// Notify whomever that we are updating the image.
*/

void ImageContext::notify_paint_image_update(VOID)
{
	int current, total;

	current = current_source_line - srce.top;
	total = clipped_source_height;

	if (current_strip == n_strips)
	{
	/* Make sure we know we are done. */
		current = total;
	}

	notify_image_update(current, total);
}

/*
// Mirror a bitmap about a vertical axis.
*/

static void near
xflip8(LPBYTE strip, int width, int height)
{
	int byte_width = (width + 3) & ~3;
	int y;

//	od("xflip8: %d, %d (%d)\r\n", width, height, byte_width);

	if (width != 1)
	{
		for (y = height; y-- > 0; strip += byte_width)
		{
			LPBYTE s, d;

		/* Do the next row. */

			for (s = strip, d = strip + width-1; s < d; s++, d--)
			{
				BYTE b;

				b = *s;
				*s = *d;
				*d = b;
			}
		}
	}
}

/*
// Mirror a bitmap about a vertical axis.
*/

static void near
xflip24(LPBYTE strip, int width, int height)
{
	int byte_width = (width*3 + 3) & ~3;
	int y;

//	od("xflip24: %d, %d (%d)\r\n", width, height, byte_width);

	if (width != 1)
	{
		for (y = height; y-- > 0; strip += byte_width)
		{
			LPBYTE s, d;

		/* Do the next row. */

			for (s = strip, d = strip + (width-1)*3; s < d; s += 3, d -= 3)
			{
				BYTE r, g, b;

				r = s[0];
				g = s[1];
				b = s[2];
				s[0] = d[0];
				s[1] = d[1];
				s[2] = d[2];
				d[0] = r;
				d[1] = g;
				d[2] = b;
			}
		}
	}
}

/*
// Flip the local map in an x direction.
*/

VOID ImageContext::xflip_strip(VOID)
{
	switch (GetRasterType())
	{
		case RASTER_TYPE_BILEVEL:
		{
			// 1-bit per pixel.
			xflip(strip,
					bitmap_info.bmiHeader.biWidth,
					bitmap_info.bmiHeader.biHeight,
					strip_byte_width);
			break;
		}
		case RASTER_TYPE_PALETTE:
		case RASTER_TYPE_8:
		case RASTER_TYPE_256:
		{
			// 1-byte per pixel.
			xflip8(strip,
					bitmap_info.bmiHeader.biWidth,
					bitmap_info.bmiHeader.biHeight);
			break;
		}
		case RASTER_TYPE_24:
		{
			// 3-bytes per pixel.
			xflip24(strip,
					bitmap_info.bmiHeader.biWidth,
					bitmap_info.bmiHeader.biHeight);
			break;
		}
		default:
		{
			break;
		}
	}

	// Handle the mask.
	if (m_fHasMask)
	{
		xflip(m_pMask,
				bitmap_info.bmiHeader.biWidth,
				bitmap_info.bmiHeader.biHeight,
				m_nMaskByteWidth);
	}
}

/*
// Show the current contents of the graphic map.
// If the display has been interrupted, the clip rectangle is modified
// to only show what has been generated so far.
*/

void ImageContext::show_graphic_map(BOOL interrupted)
{
//	LPBYTE s = strip;
	RECT rdest = dest_rect;

/* Create initial parameters. */

	if (interrupted)
	{
		if (do_yflip)
		{
			rdest.top = current_dest_line;
		}
		else
		{
			rdest.bottom = current_dest_line;
//			s += (dest_rect.bottom - rdest.bottom)*strip_byte_width;
		}
	}

	int nDestWidth = rdest.right - rdest.left;
	int nDestHeight = rdest.bottom - rdest.top;

	// Assume full strip.
	int nSrcY = 0;
	int nSrcLines = (int)bitmap_info.bmiHeader.biHeight;

	if (interrupted)
	{
		nSrcLines = nDestHeight;
		nSrcY = dest_rect.bottom - rdest.bottom;
	}

	// Blit.

	if (do_xflip)
	{
		xflip_strip();
	}

	od("SetDIBitsToDevice %d, %d, %d, %d ", rdest);

	bitmap_info.bmiHeader.biSizeImage = (DWORD)strip_byte_width * nSrcLines;
	m_MaskInfo.bmiHeader.biSizeImage = (DWORD)m_nMaskByteWidth * nSrcLines;

#if 0
	/* Debugging */
	if (!rc->IsScreen())
	{
		static BYTE fill = 255;
		memset(strip, fill, (USHORT)bitmap_info.bmiHeader.biSizeImage);
	}
#endif

	RECT rsrc;

	rsrc.left = 0;
	rsrc.top = 0;
	rsrc.right = (int)bitmap_info.bmiHeader.biWidth;
	rsrc.bottom = (int)bitmap_info.bmiHeader.biHeight;

	od("biHeight: %ld, lines: %d\r\n",
		bitmap_info.bmiHeader.biHeight,
		nDestHeight);

	if (interrupted
			|| m_fHasMask
			|| rc->IsScreen()
			|| raster_info.type != RASTER_TYPE_BILEVEL
			|| !draw_mono_map(rc->destination_hdc,
 										rdest,
										&rsrc,
										(BITMAPINFO far *)&bitmap_info,
 										strip,
										rc,
										SRCCOPY))
	{
		PaintContext::show_graphic_map(rdest, nSrcY, nSrcLines);
	}

//	od("returned %d\r\n", ret);

	if (object_image != NULL)
	{
		object_image->record_image(rdest.top-image_top,
											nDestHeight,
											strip + (dest_rect.bottom - rdest.bottom)*strip_byte_width,
											m_pMask == NULL
												? NULL
												: m_pMask + (dest_rect.bottom - rdest.bottom)*m_nMaskByteWidth);
	}
}

/*
// The default init_block() method for a renderer.
*/

IMAGE_CODE ImageContext::init_block(VOID)
{
	current_strip = 0;
	set_current_strip();

	return IMAGE_CODE_DONE;
}

/*
// The default ProcessMaskLine() method for a renderer.
*/

void ImageContext::ProcessMaskLine(void)
{
	if (current_dest_line >= min_dest_line
			&& current_dest_line <= max_dest_line)
	{
		if (m_pMask != NULL)
		{
			ASSERT(m_pMaskRow != NULL);

			if (m_pMaskRow != NULL)
			{
				BYTE b0 = gray_translate_table[0];
				BYTE b255 = gray_translate_table[255];

				BYTE b = image_info.m_fReverseMask ? 255 : 0;
				gray_translate_table[0] = b;
				gray_translate_table[255] = ~b;

				if (m_fUseDest)
				{
					convertGrayScale(m_pMaskRow,
									m_pMask + m_uMaskOffset,	
									srce_factor.x,
									dest_factor.x,
									8,
									current_dest_line,
									do_xflip
											? dest_bound.right-dest.right
											: dest.left-dest_bound.left,
									clipped_dest_width,
									FALSE);
				}
				else
				{
					convertGrayScale(m_pMaskRow,
									m_pMask + m_uMaskOffset,	
									srce_factor.x,
									srce_factor.x,
									8,
									current_dest_line,
									0,
									source_width,
									FALSE);
				}

				gray_translate_table[0] = b0;
				gray_translate_table[255] = b255;
				m_uMaskOffset += m_nNextMaskRow;
			}
		}
	}
}

/*
// The default deinit_block() method for a renderer.
*/

VOID ImageContext::deinit_block(VOID)
{
}

/////////////////////////////////////////////////////////////////////////////
// PaletteContext

/*
// The init_block() method for the full color renderer.
*/

IMAGE_CODE PaletteContext::init_block(VOID)
{
	process_error_diffusion = smoothing
					? process_errdiff_smoothed
					: process_errdiff_unsmoothed;

	return ImageContext::init_block();
}

/*
// The process_line() method for the palette renderer.
*/

IMAGE_CODE PaletteContext::process_line(SHORT *skip)
{
	LPBYTE frac, final, delta, value, table;
	LPSTR inc;
	SHORT i;
	BOOL first_time = current_strip == 0 && strip_row == 0;
	LPBYTE data_row;
	IMAGE_CODE code;

/* Start us out pointing at the converted data. */

	data_row = source_row;

/* We have a line to do. */

	y_fraction += y_delta;

	if (y_fraction >= y_compare && smoothing)
	{
	/*
	// We are going to be keeping this line.
	// Handling initialization of smoothing.
	*/

	/*
	// Translate the row. At the same time, compute the
	// DDA variables.
	*/

		frac = smooth.fraction;
		final = smooth.final;
		delta = smooth.delta;
		value = smooth.value;
		inc = smooth.inc;

		for (i = 0; i < raster_info.planes; i++)
		{
			SHORT byte;
			LPBYTE this_byte = data_row;

		/* Get a pointer to the translate table. */

			table = translate_table[i];

			for (byte = 0; byte < image_info.row_bytes; byte++)
			{
				SHORT d;

				*final = table[*this_byte++];
				if (first_time)
				{
					*value = *final;
				}
				*inc = 1;
				d = (SHORT)*final - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta = (BYTE)d;

				delta++;
				final++;
				value++;
				inc++;
			}
		}
	}

/* Process the line DDA. */

	while (y_fraction >= y_compare)
	{

//		od("(%d)\r\n", current_dest_line);

	/* This line is a keeper. */

		y_fraction -= y_compare;

		// Process the mask line.
		ProcessMaskLine();

		// Init pointers to the start of the data.

		if (smoothing)
		{
			frac = smooth.fraction;
			final = smooth.final;
			delta = smooth.delta;
			value = smooth.value;
			inc = smooth.inc;
		}

		for (i = 0; i < raster_info.planes; i++)
		{
			table = translate_table[i];

			if (smoothing)
			{
			/* If we are smoothing, do that smoothin' thing. */
				SHORT byte;

			/* Data is the current value row. */

				data_row = value;

			/* Update the DDA's for the current row. */

				for (byte = 0; byte < image_info.row_bytes; byte++)
				{
					SHORT f;

					if (*delta != 0)
					{
						f = (SHORT)*frac + (SHORT)*delta;
						while (f >= smooth_pixel_height)
						{
							f -= smooth_pixel_height;
							*value += *inc;
						}
						*frac = (UCHAR)f;

					/* If we reached our goal, stop. */

						if (*value == *final)
						{
							*delta = 0;
						}
					}
					frac++;
					final++;
					delta++;
					value++;
					inc++;
				}
			}
			else if (raster_info.planes != 1)
			{
				memcpy(gray_translate_table, table, palette.m_nColors);
			}

			if (current_dest_line >= min_dest_line
					&& current_dest_line <= max_dest_line)
			{
				if (render_method == ERROR_DIFFUSION)
				{
					(this->*process_error_diffusion)(i, data_row,
							strip + strip_offset,
							srce_factor.x,
							dest_factor.x,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width,
							source_width);
				}
				else
				{
#if 0
					od("[%d*%d=%u] off:%u, d:%d (%d of %d)\r\n",
								strip_byte_width,
								strip_height,
								strip_byte_width*strip_height,
								strip_offset,
								clipped_dest_width,
								strip_row,
								graphic_map.map_ypsize);
#endif

					convertGrayScale(data_row,
							strip + strip_offset,	
							srce_factor.x,
							dest_factor.x,
							8,
							current_dest_line,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width,
							smoothing);
				}
			}
#if 0
			else
			{
				od("OUT cdl:%d, min:%d, max:%d\n\r",
								current_dest_line,
								min_dest_line,
								max_dest_line);
			}
#endif
		}

		if (render_method == ERROR_DIFFUSION)
		{
			(*error_info)->lineno++;
		}

	/* Advance the pointers. */

		strip_offset += next_strip_row;

	/* Advance the current destination line number. */

		if (do_yflip)
		{
			current_dest_line--;
		}
		else
		{
			current_dest_line++;
		}

		if ((code = check_interrupt()) != IMAGE_CODE_CONTINUE)
		{
			return code;
		}

		if (++strip_row == (SHORT)bitmap_info.bmiHeader.biHeight)
		{
			show_graphic_map(FALSE);

			if (++current_strip == n_strips)
			{
			/* Done! */
				break;		/* Out of DDA loop. */
			}

		/* Compute next strip parameters. */

			set_current_strip();
		}
	}

/* Update the in-process thing if need be. */

	if (image_update_hook != NULL)
	{
		notify_paint_image_update();
	}

/* Compute how much to skip for the next good line. */

	if (current_strip == n_strips)
	{
		return IMAGE_CODE_DONE;
	}
	else
	{
		*skip = 0;
		do
		{
			(*skip)++;
			y_fraction += y_delta;
		} while (y_fraction < y_compare);

		y_fraction -= y_delta;
		return IMAGE_CODE_CONTINUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// FullColorContext

/*
// The init_block() method for the full color renderer.
*/

IMAGE_CODE FullColorContext::init_block(VOID)
{
	process_error_diffusion = smoothing
					? process_errdiff_smoothed
					: process_errdiff_unsmoothed;

/* Set up a one-to-one translation. */

	for (SHORT i = 0; i < 256; i++)
	{
		gray_translate_table[i] = (BYTE)i;
	}

	return ImageContext::init_block();
}

/*
// The process_line() method for the full color renderer.
*/

IMAGE_CODE FullColorContext::process_line(SHORT *skip)
{
	LPBYTE frac, final, delta, value;
	LPSTR inc;
	SHORT i;
	BOOL first_time = current_strip == 0 && strip_row == 0;
	LPBYTE data_row;
	IMAGE_CODE code;

/* We have a line to do. */

	y_fraction += y_delta;

	if (y_fraction >= y_compare)
	{
	/*
	// We are going to be keeping this line.
	// Translate the RGB values to destination planes.
	*/

		setup_translate_tables(source_row, source_width);

		if (smoothing)
		{
		/*
		// Handling initialization of smoothing.
		*/

		/*
		// Compute the DDA variables.
		*/

			frac = smooth.fraction;
			final = smooth.final;
			delta = smooth.delta;
			value = smooth.value;
			inc = smooth.inc;

			for (i = 0; i < raster_info.planes; i++)
			{
				SHORT byte;
				LPBYTE this_byte = translate_table[i];

			/* Get a pointer to the translate table. */

				for (byte = 0; byte < source_width; byte++)
				{
					SHORT d;

					*final = *this_byte++;
					if (first_time)
					{
						*value = *final;
					}
					*inc = 1;
					d = (SHORT)*final - (SHORT)*value;
					if (d < 0)
					{
						d = -d;
						*inc = -1;
					}
					*frac++ = 0;
					*delta = (BYTE)d;

					delta++;
					final++;
					value++;
					inc++;
				}
			}
		}
	}

/* Process the line DDA. */

	while (y_fraction >= y_compare)
	{

//		printf("(%d)\n", current_dest_line);

	/* This line is a keeper. */

		y_fraction -= y_compare;

		// Process the mask line.
		ProcessMaskLine();

	/* Init pointers to the start of the data. */

		if (smoothing)
		{
			frac = smooth.fraction;
			final = smooth.final;
			delta = smooth.delta;
			value = smooth.value;
			inc = smooth.inc;
		}

		for (i = 0; i < raster_info.planes; i++)
		{
			data_row = translate_table[i];

			if (smoothing)
			{
			/* If we are smoothing, do that smoothin' thing. */
				SHORT byte;

			/* Data is the current value row. */

				data_row = value;

			/* Update the DDA's for the current row. */

				for (byte = 0; byte < image_info.row_bytes; byte++)
				{
					SHORT f;

					if (*delta != 0)
					{
						f = (SHORT)*frac + (SHORT)*delta;
						while (f >= smooth_pixel_height)
						{
							f -= smooth_pixel_height;
							*value += *inc;
						}
						*frac = (UCHAR)f;

					/* If we reached our goal, stop. */

						if (*value == *final)
						{
							*delta = 0;
						}
					}
					frac++;
					final++;
					delta++;
					value++;
					inc++;
				}
			}

			if (current_dest_line >= min_dest_line
					&& current_dest_line <= max_dest_line)
			{
				if (render_method == ERROR_DIFFUSION)
				{
					(this->*process_error_diffusion)(i, data_row,
							strip + strip_offset,
							srce_factor.x,
							dest_factor.x,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width,
							source_width);
				}
				else
				{
					convertGrayScale(data_row,
							strip + strip_offset,	
							srce_factor.x,
							dest_factor.x,
							8,
							current_dest_line,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width,
							smoothing);
				}
			}
#if 0
			else
			{
				printf("24 OUT cdl:%d, min:%d, max:%d\n",
								current_dest_line,
								min_dest_line,
								max_dest_line);
			}
#endif
		}

		if (render_method == ERROR_DIFFUSION)
		{
			(*error_info)->lineno++;
		}

	/* Advance the pointer. */

		strip_offset += next_strip_row;

	/* Advance the current destination line number. */

		if (do_yflip)
		{
			current_dest_line--;
		}
		else
		{
			current_dest_line++;
		}

		if ((code = check_interrupt()) != IMAGE_CODE_CONTINUE)
		{
			return code;
		}

		if (++strip_row == (SHORT)bitmap_info.bmiHeader.biHeight)
		{
		/* Show the map on the screen. */

			show_graphic_map(FALSE);

			if (++current_strip == n_strips)
			{
			/* Done! */
				break;		/* Out of DDA loop. */
			}

		/* Compute next strip parameters. */

			set_current_strip();
		}
	}

/* Update the in-process thing if need be. */

	if (image_update_hook != NULL)
	{
		notify_paint_image_update();
	}

/* Compute how much to skip for the next good line. */

	if (current_strip == n_strips)
	{
		return IMAGE_CODE_DONE;
	}
	else
	{
		*skip = 0;
		do
		{
			(*skip)++;
			y_fraction += y_delta;
		} while (y_fraction < y_compare);

		y_fraction -= y_delta;
		return IMAGE_CODE_CONTINUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Bit24Context

/*
// Block initialization for 24-bit renderer.
*/

IMAGE_CODE Bit24Context::init_block(VOID)
{
/* Skip unused error-diffusion stuff from PalPaletteContext. */
	return ImageContext::init_block();
}

/*
// The process_line() method for the 24-bit color renderer.
*/

IMAGE_CODE Bit24Context::process_line(SHORT *skip)
{
	LPBYTE frac, final, delta, value;
	LPSTR inc;
	BOOL first_time = current_strip == 0 && strip_row == 0;
	LPBYTE data_row;
	IMAGE_CODE code;
	BOOL src24 = image_info.image_type == IMAGE_FULL_COLOR;

/* Start us out pointing at the converted data. */

	data_row = source_row;

/* We have a line to do. */

	y_fraction += y_delta;

	if (y_fraction >= y_compare)
	{
		if (src24)
		{
			setup_translate_tables(data_row, source_width);
		}

		if (smoothing)
		{
		/*
		// We are going to be keeping this line.
		// Handling initialization of smoothing.
		*/

		/*
		// Translate the row. At the same time, compute the
		// DDA variables.
		*/

			frac = smooth.fraction;
			final = smooth.final;
			delta = smooth.delta;
			value = smooth.value;
			inc = smooth.inc;

			LPBYTE this_byte = data_row;

			for (SHORT byte = 0; byte < (SHORT)smoothing_size; byte += 3)
			{
				SHORT d;
				RGBTRIPLE rgb;

				if (src24)
				{
					rgb = *(RGBTRIPLE far *)this_byte;
					this_byte += 3;
				}
				else
				{
					rgb = palette.m_pData[*this_byte];
					this_byte++;
				}

			/* Process three bytes (RGB) at a time. */

				UCHAR red = ((UCHAR *)&rgb)[0];
				UCHAR green = ((UCHAR *)&rgb)[1];
				UCHAR blue = ((UCHAR *)&rgb)[2];

				/* Process red. */
				*final = red;
				if (first_time)
				{
					*value = red;
				}
				*inc = 1;
				d = (SHORT)red - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta++ = (BYTE)d;

				final++;
				value++;
				inc++;

				/* Process green. */

				*final = green;
				if (first_time)
				{
					*value = green;
				}
				*inc = 1;
				d = (SHORT)green - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta++ = (BYTE)d;

				final++;
				value++;
				inc++;

				/* Process blue. */

				*final = blue;
				if (first_time)
				{
					*value = blue;
				}
				*inc = 1;
				d = (SHORT)blue - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta++ = (BYTE)d;

				final++;
				value++;
				inc++;
			}
		}

	/* Process the line DDA. */

		while (y_fraction >= y_compare)
		{

//			od("(%d)", current_dest_line);

		/* This line is a keeper. */

			y_fraction -= y_compare;

			// Process the mask line.
			ProcessMaskLine();

		/* Init pointers to the start of the data. */

			if (smoothing)
			{
			/* If we are smoothing, do that smoothin' thing. */

				frac = smooth.fraction;
				final = smooth.final;
				delta = smooth.delta;
				value = smooth.value;
				inc = smooth.inc;

			/* Data is the current value row. */

				data_row = value;

			/* Update the DDA's for the current row. */

				for (SHORT byte = 0; byte < (SHORT)smoothing_size; byte++)
				{
					SHORT f;

					if (*delta != 0)
					{
						f = (SHORT)*frac + (SHORT)*delta;
						while (f >= smooth_pixel_height)
						{
							f -= smooth_pixel_height;
							*value += *inc;
						}
						*frac = (UCHAR)f;

					/* If we reached our goal, stop. */

						if (*value == *final)
						{
							*delta = 0;
						}
					}
					frac++;
					final++;
					delta++;
					value++;
					inc++;
				}
			}

			if (current_dest_line >= min_dest_line
					&& current_dest_line <= max_dest_line)
			{
				convertPalette(data_row, strip + strip_offset,
							srce_factor.x, dest_factor.x,
							current_dest_line,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width);
			}

		/* Advance the pointer. */

			strip_offset += next_strip_row;

		/* Advance the current destination line number. */

			if (do_yflip)
			{
				current_dest_line--;
			}
			else
			{
				current_dest_line++;
			}

			if ((code = check_interrupt()) != IMAGE_CODE_CONTINUE)
			{
				return code;
			}

			if (++strip_row == (SHORT)bitmap_info.bmiHeader.biHeight)
			{
				show_graphic_map(FALSE);

				if (++current_strip == n_strips)
				{
				/* Done! */
					break;		/* Out of DDA loop. */
				}

			/* Compute next strip parameters. */

				set_current_strip();
			}
		}
	}

/* Update the in-process thing if need be. */

	if (image_update_hook != NULL)
	{
		notify_paint_image_update();
	}

/* Compute how much to skip for the next good line. */

	if (current_strip == n_strips)
	{
		return IMAGE_CODE_DONE;
	}
	else
	{
		*skip = 0;
		do
		{
			(*skip)++;
			y_fraction += y_delta;
		} while (y_fraction < y_compare);

		y_fraction -= y_delta;
		return IMAGE_CODE_CONTINUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// DirectBitmapContext

/*
// Block initialization for the direct renderer.
*/

IMAGE_CODE DirectBitmapContext::init_block(VOID)
{
/*
// Setup the colors in the bitmap we will be using as our source.
*/
	switch (image_type)
	{
		case IMAGE_BILEVEL:
		case IMAGE_PALETTE:
		{
			int nColors = palette.m_nColors;

			// Copy the palette over to the bitmap.
			RGBQUAD* pQuad = bitmap_info.bmiColors;
			LPBYTE pRGB = (LPBYTE)palette.m_pData;
			ASSERT(nColors <= 256);

			for (int nColor = 0; nColor < nColors; nColor++)
			{
				pQuad->rgbReserved = 0;
				pQuad->rgbRed = pRGB[0];
				pQuad->rgbGreen = pRGB[1];
				pQuad->rgbBlue = pRGB[2];

				pRGB += 3;
				pQuad++;
			}
		}
	}

	return ImageContext::init_block();
}

/*
// The process_line() method for the direct renderer.
*/

IMAGE_CODE DirectBitmapContext::process_line(SHORT *skip)
{
	BOOL src24 = (image_info.image_type == IMAGE_FULL_COLOR);

/* Start us out pointing at the converted data. */

/* We have a line to do. */

	if (src24)
	{
		// Setup the translate tables. Reverse the red and blue since we
		// are going to be outputting as a DIB.
		setup_translate_tables(source_row, source_width, TRUE);
	}

#if 0
	od("(%d, %d) (%d, %ld)\r\n",
		current_source_line,
		current_dest_line,
		strip_row,
		bitmap_info.bmiHeader.biHeight);
#endif

	if (current_dest_line >= min_dest_line
			&& current_dest_line <= max_dest_line)
	{
		ProcessMaskLine();

		memcpy(strip + strip_offset, source_row, strip_byte_width);
	}

	y_fraction += y_delta;

	if (y_fraction >= y_compare)
	{
		y_fraction -= y_compare;
	/* Advance the current destination line number. */

		if (do_yflip)
		{
			current_dest_line--;
		}
		else
		{
			current_dest_line++;
		}
	}

/* Advance the pointer. */

	strip_offset += next_strip_row;

	IMAGE_CODE code;
	if ((code = check_interrupt()) != IMAGE_CODE_CONTINUE)
	{
		return code;
	}

	if (++strip_row == (SHORT)bitmap_info.bmiHeader.biHeight)
	{
		show_graphic_map(FALSE);

		if (++current_strip != n_strips)
		{
		/* Compute next strip parameters. */

			set_current_strip();
		}
	}

/* Update the in-process thing if need be. */

	if (image_update_hook != NULL)
	{
		notify_paint_image_update();
	}

/* Compute how much to skip for the next good line. */

	if (current_strip == n_strips)
	{
		return IMAGE_CODE_DONE;
	}
	else
	{
		*skip = 1;
		return IMAGE_CODE_CONTINUE;
	}
}

void DirectBitmapContext::ProcessMaskLine(void)
{
	ImageContext::ProcessMaskLine();
#if 0
	if (m_pMask != NULL)
	{
		ASSERT(m_pMaskRow != NULL);
		if (m_pMaskRow != NULL)
		{
			memcpy(m_pMask + m_uMaskOffset, m_pMaskRow, m_nMaskByteWidth);
         m_uMaskOffset += m_nNextMaskRow;
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
// PalPaletteContext

VOID PalPaletteContext::convertPalette(LPBYTE sp, LPBYTE dp,
							int src_factor, int dst_factor,
 							int row,
							int x_offset, int width)
{
/* Initialize the DDA variables. */

	int delta = dst_factor;
	int compare = src_factor;
	int fraction = 0;
//	int red_index = image_info.red_index;
//	int blue_index = red_index ^ 2;

	SHORT smooth_inc[3], smooth_final[3], smooth_value[3];
	SHORT smooth_fraction[3], smooth_compare, smooth_delta[3];

/* Initialize pointers. */

	LPBYTE s = sp;
	LPBYTE d = dp;

	SHORT step = 1;
	SHORT key;

	if (raster_info.type == RASTER_TYPE_24)
	{
		key = 24;
	}
	else if (raster_info.type == RASTER_TYPE_8)
	{
		key = 8;
	}
	else
	{
	/* PALETTE or 256. */
		key = 256;
	}

	BOOL src24 = smoothing || (image_info.image_type == IMAGE_FULL_COLOR);

	if (src24)
	{
		step = 3;
	}

	BYTE *dither_row = ((key == 256)
									? palette_dither_matrix
									: dither_matrix)[row & 7];
	int soff = 0;

	if (x_offset != 0)
	{
		soff = MulDiv(x_offset+1, compare, delta)*step;
		if ((fraction = delta - mulrem(x_offset+1, compare, delta)) == delta)
		{
			fraction = 0;
			soff += step;
		}
		s += soff;
		fraction += compare - delta;
	}

	if (smoothing)
	{
		if (soff != 0)
		{
			s -= 3;
		}
#if 0
		smooth_value[0] = s[red_index];
		smooth_value[1] = s[1];
		smooth_value[2] = s[blue_index];
#else
		smooth_value[0] = s[0];
		smooth_value[1] = s[1];
		smooth_value[2] = s[2];
#endif
		if (soff != 0)
		{
			s += 3;
		}

		smooth_compare = smooth_pixel_width;		/* Same for all channels. */
	}

	for (SHORT count = width; count > 0; )
	{
		if ((fraction += delta) >= compare)
		{
			BYTE red, blue, green;
			BYTE bBaseIndex;
			BYTE bRedDither, bGreenDither, bBlueDither;

		/* This byte is a keeper. */

		/* Get the current byte and translate it to its correct value. */

			RGBTRIPLE rgb;

			if (src24)
			{
				rgb = *(RGBTRIPLE far *)s;
			}
			else
			{
				rgb = palette.m_pData[*s];
			}

#if 0
			red = ((UCHAR *)&rgb)[red_index];
			green = ((UCHAR *)&rgb)[1];
			blue = ((UCHAR *)&rgb)[blue_index];
#else
			red = ((UCHAR *)&rgb)[0];
			green = ((UCHAR *)&rgb)[1];
			blue = ((UCHAR *)&rgb)[2];
#endif

			BOOL fNewPixel = TRUE;

			if (smoothing)
			{
				smooth_final[0] = red;
				smooth_final[1] = green;
				smooth_final[2] = blue;

				smooth_fraction[0] =
					smooth_fraction[1] =
					smooth_fraction[2] = 0;
				smooth_inc[0] =
					smooth_inc[1] =
					smooth_inc[2] = 1;

				if ((smooth_delta[0] = smooth_final[0] - smooth_value[0]) < 0)
				{
					smooth_delta[0] = -smooth_delta[0];
					smooth_inc[0] = -1;
				}
				if ((smooth_delta[1] = smooth_final[1] - smooth_value[1]) < 0)
				{
					smooth_delta[1] = -smooth_delta[1];
					smooth_inc[1] = -1;
				}
				if ((smooth_delta[2] = smooth_final[2] - smooth_value[2]) < 0)
				{
					smooth_delta[2] = -smooth_delta[2];
					smooth_inc[2] = -1;
				}
			}

			while (fraction >= compare)
			{
				fraction -= compare;

				if (smoothing)
				{
               // JNTO: Does smooth_value need to be SHORT or can it be BYTE?
					red = (BYTE)smooth_value[0];
					green = (BYTE)smooth_value[1];
					blue = (BYTE)smooth_value[2];
					fNewPixel = TRUE;
				}

				if (fNewPixel)
				{
					if (key == 8)
					{
					/*
					// Bottom two bits are dropped, leaving six
					// for dither.
					*/
						red >>= 2;
						green >>= 2;
						blue >>= 2;
					}
					else if (key == 256)
					{
					/*
					// Div is "plateau".
					// Mod is dither at that level.
					*/
						bRedDither = aModulo51[red];
						bGreenDither = aModulo51[green];
						bBlueDither = aModulo51[blue];

						bBaseIndex = aTimes36[aDividedBy51[blue]]
									+ aTimes6[aDividedBy51[green]]
									+ aDividedBy51[red];
					}
					fNewPixel = FALSE;
				}

				if (key == 24)
				{
				/* DIB RGB ordering. */
					*d++ = blue;
					*d++ = green;
					*d++ = red;
				}
				else if (key == 8)
				{
				/* Get the dither entry. */

					BYTE dither_entry = dither_row[x_offset & 7];

				/* Dither all three simultaneously. */

					BYTE bIndex = 0;

					if (red >= dither_entry)
					{
						bIndex |= 4;
					}
					if (green >= dither_entry)
					{
						bIndex |= 2;
					}
					if (blue >= dither_entry)
					{
						bIndex |= 1;
					}

					*d++ = bIndex;
				}
				else			// key == 256
				{
					BYTE bIndex = bBaseIndex;

				/* Get the dither entry. */

					BYTE dither_entry = dither_row[x_offset & 7];

				/* Dither all three simultaneously. */

					if (bRedDither >= dither_entry)
					{
						bIndex += 1;
					}
					if (bGreenDither >= dither_entry)
					{
						bIndex += 6;
					}
					if (bBlueDither >= dither_entry)
					{
						bIndex += 36;
					}

					*d++ = aWinGHalftoneTranslation[bIndex];
				}

				if (smoothing)
				{
				/* Update the smoothing if smoothing is happening. */
					/* Red */
					if (smooth_delta[0] != 0)
					{
						smooth_fraction[0] += smooth_delta[0];
						while (smooth_fraction[0] >= smooth_compare)
						{
							smooth_fraction[0] -= smooth_compare;
							smooth_value[0] += smooth_inc[0];
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value[0] == smooth_final[0])
						{
							smooth_delta[0] = 0;
						}
					}

					/* Green */
					if (smooth_delta[1] != 0)
					{
						smooth_fraction[1] += smooth_delta[1];
						while (smooth_fraction[1] >= smooth_compare)
						{
							smooth_fraction[1] -= smooth_compare;
							smooth_value[1] += smooth_inc[1];
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value[1] == smooth_final[1])
						{
							smooth_delta[1] = 0;
						}
					}
					/* Blue */
					if (smooth_delta[2] != 0)
					{
						smooth_fraction[2] += smooth_delta[2];
						while (smooth_fraction[2] >= smooth_compare)
						{
							smooth_fraction[2] -= smooth_compare;
							smooth_value[2] += smooth_inc[2];
						}
					/* Stop smoothing if we hit the final value. */
						if (smooth_value[2] == smooth_final[2])
						{
							smooth_delta[2] = 0;
						}
					}
				}
				x_offset++;

				if (--count == 0)
				{
					break;
				}
			}
		}
		s += step;
	}
}

/*
// Block initialization for the palette to palette renderer.
*/

IMAGE_CODE PalPaletteContext::init_block(VOID)
{
//	memcpy(gray_translate_table, rc->bitmap_gamma_curve, 256);

	process_error_diffusion = smoothing
					? process_errdiff_smoothed
					: process_errdiff_unsmoothed;

	return ImageContext::init_block();
}

/*
// The process_line() method for the palette renderer.
*/

IMAGE_CODE PalPaletteContext::process_line(SHORT *skip)
{
	LPBYTE frac, final, delta, value;
	LPSTR inc;
	BOOL first_time = current_strip == 0 && strip_row == 0;
	LPBYTE data_row;
	IMAGE_CODE code;
	BOOL src24 = image_info.image_type == IMAGE_FULL_COLOR;

/* Start us out pointing at the converted data. */

	data_row = source_row;

/* We have a line to do. */

	y_fraction += y_delta;

	if (y_fraction >= y_compare)
	{
		if (src24)
		{
			setup_translate_tables(data_row, source_width);
		}

		if (smoothing)
		{
		/*
		// We are going to be keeping this line.
		// Handling initialization of smoothing.
		*/

		/*
		// Translate the row. At the same time, compute the
		// DDA variables.
		*/

			frac = smooth.fraction;
			final = smooth.final;
			delta = smooth.delta;
			value = smooth.value;
			inc = smooth.inc;

			LPBYTE this_byte = data_row;

			for (SHORT byte = 0; byte < (SHORT)smoothing_size; byte += 3)
			{
				SHORT d;
				RGBTRIPLE rgb;

				if (src24)
				{
					rgb = *(RGBTRIPLE far *)this_byte;
					this_byte += 3;
				}
				else
				{
					rgb = palette.m_pData[*this_byte];
					this_byte++;
				}

			/* Process three bytes (RGB) at a time. */

				UCHAR red = ((UCHAR *)&rgb)[0];
				UCHAR green = ((UCHAR *)&rgb)[1];
				UCHAR blue = ((UCHAR *)&rgb)[2];

				/* Process red. */
				*final = red;
				if (first_time)
				{
					*value = red;
				}
				*inc = 1;
				d = (SHORT)red - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta++ = (BYTE)d;

				final++;
				value++;
				inc++;

				/* Process green. */

				*final = green;
				if (first_time)
				{
					*value = green;
				}
				*inc = 1;
				d = (SHORT)green - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta++ = (BYTE)d;

				final++;
				value++;
				inc++;

				/* Process blue. */

				*final = blue;
				if (first_time)
				{
					*value = blue;
				}
				*inc = 1;
				d = (SHORT)blue - (SHORT)*value;
				if (d < 0)
				{
					d = -d;
					*inc = -1;
				}
				*frac++ = 0;
				*delta++ = (BYTE)d;

				final++;
				value++;
				inc++;
			}
		}

	/* Process the line DDA. */

		while (y_fraction >= y_compare)
		{

//			od("(%d)", current_dest_line);

		/* This line is a keeper. */

			y_fraction -= y_compare;

			// Process the mask line.
			ProcessMaskLine();

		/* Init pointers to the start of the data. */

			if (smoothing)
			{
			/* If we are smoothing, do that smoothin' thing. */

				frac = smooth.fraction;
				final = smooth.final;
				delta = smooth.delta;
				value = smooth.value;
				inc = smooth.inc;

			/* Data is the current value row. */

				data_row = value;

			/* Update the DDA's for the current row. */

				for (SHORT byte = 0; byte < (SHORT)smoothing_size; byte++)
				{
					SHORT f;

					if (*delta != 0)
					{
						f = (SHORT)*frac + (SHORT)*delta;
						while (f >= smooth_pixel_height)
						{
							f -= smooth_pixel_height;
							*value += *inc;
						}
						*frac = (UCHAR)f;

					/* If we reached our goal, stop. */

						if (*value == *final)
						{
							*delta = 0;
						}
					}
					frac++;
					final++;
					delta++;
					value++;
					inc++;
				}
			}

			if (current_dest_line >= min_dest_line
					&& current_dest_line <= max_dest_line)
			{
				if (render_method == ERROR_DIFFUSION)
				{
					(this->*process_error_diffusion)(data_row,
							strip + strip_offset,	
							srce_factor.x,
							dest_factor.x,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width,
							source_width);
				}
				else
				{
#if 0
					printf("[%d*%d=%u] off:%u, d:%d (%d of %d)\n",
								strip_byte_width,
								strip_height,
								strip_byte_width*strip_height,
								strip_offset,
								clipped_dest_width,
								strip_row,
								graphic_map.map_ypsize);
#endif
					convertPalette(data_row,
							strip + strip_offset,	
							srce_factor.x,
							dest_factor.x,
							current_dest_line,
							do_xflip
									? dest_bound.right-dest.right
									: dest.left-dest_bound.left,
							clipped_dest_width);
				}
			}
#if 0
			else
			{
				od("OUT cdl:%d, min:%d, max:%d\n\r",
								current_dest_line,
								min_dest_line,
								max_dest_line);
			}
#endif

			if (render_method == ERROR_DIFFUSION)
			{
				(*error_info)->lineno++;
			}

		/* Advance the pointer. */

			strip_offset += next_strip_row;

		/* Advance the current destination line number. */

			if (do_yflip)
			{
				current_dest_line--;
			}
			else
			{
				current_dest_line++;
			}

			if ((code = check_interrupt()) != IMAGE_CODE_CONTINUE)
			{
				return code;
			}

			if (++strip_row == (SHORT)bitmap_info.bmiHeader.biHeight)
			{
				show_graphic_map(FALSE);

				if (++current_strip == n_strips)
				{
				/* Done! */
					break;		/* Out of DDA loop. */
				}

			/* Compute next strip parameters. */

				set_current_strip();
			}
		}
	}

/* Update the in-process thing if need be. */

	if (image_update_hook != NULL)
	{
		notify_paint_image_update();
	}

/* Compute how much to skip for the next good line. */

	if (current_strip == n_strips)
	{
		return IMAGE_CODE_DONE;
	}
	else
	{
		*skip = 0;
		do
		{
			(*skip)++;
			y_fraction += y_delta;
		} while (y_fraction < y_compare);

		y_fraction -= y_delta;
		return IMAGE_CODE_CONTINUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// More ImageContext

/*
// Initialize the palette if one is required.
*/

VOID ImageContext::init_palette(VOID)
{
	if (palette.m_pData != NULL)
	{
		if (!palette.m_fRead)
		{
			palette.BuildDefaultPalette(image_info);
		}

	/*
	// Setup the destination color tables.
	*/

		setup_translate_tables((LPBYTE)palette.m_pData, palette.m_nColors);
	}
}

/*
// Compute the source for this image.
*/

void ImageContext::compute_source_extent(void)
{
	POINT p;

	source->compute_source_extent(&p);
	source_width = p.x;
	source_height = p.y;
}

/*
// Initialize color and smoothing variables.
*/

BOOL ImageContext::init_render_machine(VOID)
{
/*
// Let the source compute how big buffers should be.
// This may update the ic's image_info structure if necessary.
*/

	source->compute_render_needs();

	if (!init_render_buffers())
	{
		return FALSE;
	}

/*
// Initialize the smoothing variables.
*/

	if (smoothing)
	{
		if (!init_smoothing())
		{
			return FALSE;
		}
	}

	return TRUE;
}

IMAGE_CODE ImageContext::check_interrupt(VOID)
{
	IMAGE_CODE code = PaintContext::check_interrupt();

	if (code == IMAGE_CODE_INTERRUPTED)
	{
		show_graphic_map(TRUE);
	}
	return code;
}

/*
// Process the image block.
*/

IMAGE_CODE ImageContext::process_block(VOID)
{
	ERRORCODE error;
	IMAGE_CODE code;
	SHORT advance;

	if ((code = init_block()) != IMAGE_CODE_DONE)
	{
		return code;
	}

	current_source_line = srce.top;

	while (current_strip < n_strips)
	{
	/* Read the next line we want. */

//		od("Read line %d\r\n", current_source_line);

		if ((error = source->read(current_source_line,
								source_row,
 								1,
								m_pMaskRow)) != ERRORCODE_None)
		{
			if (error == ERRORCODE_Open || error == ERRORCODE_DoesNotExist)
			{
			/* Handle an open error. */
				code = IMAGE_CODE_OPEN;
			}
			else
			{
				code = IMAGE_CODE_ERROR;
				od(">>> Got error %d reading source. <<<\r\n", error);
			}
			break;
		}

		// Process the read line.
		if ((code = process_line(&advance)) != IMAGE_CODE_CONTINUE)
		{
			break;
		}

		current_source_line += advance;
	}

	deinit_block();

	if (code == IMAGE_CODE_CONTINUE)
	{
		code = IMAGE_CODE_DONE;
	}

	return code;
}

/*
// Paint an image file.
*/

PUBLIC UpdateStatePtr
paint_bitmap_image(GraphicObjectPtr object, LPRECT clip,
					GraphicPtr record, ImageDriverPtr driver,
					RedisplayContextPtr rc, UpdateStatePtr ustate)
{
	IMAGE_CODE code;
	ImageSourcePtr source;
	ImageContextPtr ic;
	ERRORCODE error;
   RasterInfo raster_info;
	RENDER_METHOD nRenderMethod = object->get_render_method();

	build_dither_matrix(rc);

/*
// Create our image source.
// This is based more on transformation than image type, so we can create
// it even now.
*/

#ifdef DO_ROTATION
	if (object->get_rotation() != 0)
	{
		source = new RotatedImageSource(driver);
	}
	else
#endif
	{
		source = new ImageSource(driver);
	}

	if (source == NULL)
	{
		return graphic_error_state(ustate, UPDATE_TYPE_Error, ERRORCODE_Memory);
	}

/* Get the info from the driver. */

	IMAGE_INFO image_info;
	if ((error = source->info(&image_info)) != ERRORCODE_None)
	{
		delete source;
		return graphic_error_state(ustate, UPDATE_TYPE_Error, error);
	}

/* Setup the raster info. */

	raster_info = rc->info;

/* Construct our image paint context. */

	if (object->DirectBitmapPrint(rc))
	{
		raster_info.type = RASTER_TYPE_DIRECT;
		raster_info.bpp = image_info.bpp;
		raster_info.planes = image_info.planes;
		switch (image_info.image_type)
		{
			case IMAGE_FULL_COLOR:
			{
			// Raster wants to be this way.
				raster_info.bpp = 24;
				raster_info.planes = 1;
				break;
			}
			case IMAGE_BILEVEL:
			case IMAGE_PALETTE:
			{
				raster_info.bpp = 8;			// Converted at the source...
				raster_info.planes = 1;
				break;
			}
			default:
			{
			// The rest are OK as is.
				break;
			}
		}
	// We have no render method.
		nRenderMethod = (RENDER_METHOD)-1;
	}
	else if (raster_info.type == RASTER_TYPE_8)
	{
		raster_info.bpp = 8;
		raster_info.planes = 1;
	}

	ic = NULL;

	switch (raster_info.type)
	{
		case RASTER_TYPE_BILEVEL:
		{
			switch (image_info.image_type)
			{
				case IMAGE_BILEVEL:
				case IMAGE_PALETTE:
				{
					ic = new PaletteContext(source);
					break;
				}
				case IMAGE_FULL_COLOR:
				{
					ic = new FullColorContext(source);
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case RASTER_TYPE_8:
		case RASTER_TYPE_PALETTE:
		case RASTER_TYPE_256:
		{
			ic = new PalPaletteContext(source);
			raster_info.error_bytes = 3;
			break;
		}
		case RASTER_TYPE_24:
		{
			ic = new Bit24Context(source);
			raster_info.error_bytes = 3;
			break;
		}
		case RASTER_TYPE_DIRECT:
		{
			ic = new DirectBitmapContext(source);
			break;
		}
		default:
		{
			break;
		}
	}

	if (ic == NULL)
	{
		delete source;
		return graphic_error_state(ustate);
	}

/* Plug in the raster info. */

	ic->raster_info = raster_info;

/* Handle our update state. */

	if (ustate != NULL)
	{
	/* Only do the rest. */
		clip = &((ImageUpdateStatePtr)ustate)->clip;
	}

/* Initialize the image update. */

	init_image_update();

/* Plug in the necessary parameter info. */

	ic->image_info = image_info;
	ic->image_type = image_info.image_type;			// for PaintContext

	ic->object = object;
	ic->bound = object->GetDrawBound();
	ic->clip = clip;
	ic->rc = rc;
	ic->palette.m_nColors = image_info.colors;
	ic->object_image = (CBitmapObjectImage*)object->get_object_image();
	if (ic->object_image != NULL)
	{
		if (!ic->object_image->making_image() || !rc->using_images)
		{
			ic->object_image = NULL;
		}
	}

/* Plug in the object info. */

	ic->do_xflip = object->get_flags() & OBJECT_FLAG_xflipped;
	ic->do_yflip = object->get_flags() & OBJECT_FLAG_yflipped;
	ic->error_info = object->get_error_info();
	ic->graphic_flags = object->get_graphic_flags();

/* Handle upside-down images. */

	if (image_info.do_yflip)
	{
		ic->do_yflip = !ic->do_yflip;
	}

/*
// Set the render method.
*/

	ic->set_render_method(nRenderMethod);

	ic->no_expand_clip = (ic->object_image != NULL);

	BOOL fDirect = (raster_info.type == RASTER_TYPE_DIRECT);

/*
// Calculate the non-bitmap-dependent pre-paint variables.
*/

	if (!ic->pre_paint())
	{
		delete ic;
		delete source;
		return graphic_error_state(ustate);
	}

	if (!ic->init_render_machine())
	{
		delete ic;
		delete source;
		return graphic_error_state(ustate);
	}

	if (ustate == NULL)
	{
		ic->image_top = ic->dest.top;
	}
	else
	{
	/* Restore the top of the recording image. */
		ic->image_top = ((ImageUpdateStatePtr)ustate)->image_top;
	}

/*
// Open the file.
*/

	if ((error = source->open(&ic->palette)) == ERRORCODE_None)
	{
	/* 
	// If the palette was not read, construct a default one.
 	*/

		ic->init_palette();

	/*
	// Initialize the strip.
	*/

		code = IMAGE_CODE_ERROR;

		if (ic->initialize_strip(record->record.x_size, !fDirect, !fDirect, ic->image_info.m_fHasMask))
		{
			// Select a clip region if possible.
			BOOL f = object->ApplyClipRgn(rc);
			if (f != -1)
			{
				// Process the block.
				code = ic->process_block();
			}

			// Restore the DC if we selected a clip region.
			object->RestoreClipRgn(rc, f);

			// Free the strip.
			ic->free_strip();
		}

	/* Close the file. */

		source->close();
	}
   else if (error == ERRORCODE_LZWNotSupported)
   {
      code = IMAGE_CODE_ERROR;
   }
	else
	{
		code = IMAGE_CODE_OPEN;
	}

/* Compute our state. */

	switch (code)
	{
		case IMAGE_CODE_DONE:
		case IMAGE_CODE_CONTINUE:
		{
		/* Completed. */
			if (ustate != NULL)
			{
				delete ustate;
			}
			ustate = NULL;
			break;
		}
		case IMAGE_CODE_INTERRUPTED:
		case IMAGE_CODE_TERMINATED:
		{
			if (rc->terminate_interrupts)
			{
				ustate = graphic_terminated_state(ustate);
			}
			else
			{
				ImageUpdateStatePtr iustate;

				if (ustate == NULL)
				{
					iustate = new ImageUpdateState;
				}
				else
				{
					iustate = (ImageUpdateStatePtr)ustate;
				}

				iustate->type = UPDATE_TYPE_Interrupted;
				iustate->clip = *clip;
				iustate->image_top = ic->image_top;			// Save the image top

				if (ic->do_yflip)
				{
					if (ic->current_dest_line < iustate->clip.bottom)
					{
						iustate->clip.bottom = ic->current_dest_line;
						if (code == IMAGE_CODE_TERMINATED)
						{
						/* Do whole strip over. */
							iustate->clip.bottom += (ic->strip_row+1);
						}
					}
				}
				else
				{
					if (ic->current_dest_line > iustate->clip.top)
					{
						iustate->clip.top = ic->current_dest_line;
						if (code == IMAGE_CODE_TERMINATED)
						{
						/* Do whole strip over. */
							iustate->clip.top -= (ic->strip_row+1);
						}
					}
				}
				ustate = iustate;
			}
			break;
		}
		case IMAGE_CODE_OPEN:
		{
		/* Say goodbye to our context. */

			delete ic;
			delete source;
			return record->handle_open_error(ustate);
		}
		case IMAGE_CODE_ERROR:
		default:
		{
			ustate = graphic_error_state(ustate, UPDATE_TYPE_Error, error);
			break;
		}
	}

/* Say goodbye to our context. */

	delete ic;
	delete source;

	return ustate;
}
