/*
// $Header: /PM8/App/GRAFOBJ.CPP 1     3/03/99 6:05p Gbeddow $
//
// Graphic object routines.
//
// $Log: /PM8/App/GRAFOBJ.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 11    2/26/99 8:39p Rgrenfel
// Supported the mechanism to get the original object's data path.
// 
// 10    9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 9     8/10/98 3:47p Hforman
// Crop Copyright modifications
// 
// 8     4/07/98 8:42a Jayn
// DirectBitmapPrint is TRUE for metafile rcs.
// 
// 7     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 6     1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 5     12/11/97 9:20a Jstoner
// vector to bitmap conversion for photo enhancer
// 
// 4     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
// 3     11/19/97 3:43p Jayn
// Rotated bitmap support.
 * 
 * 2     10/28/97 5:40p Dennis
 * Added IsEmpty()
// 
//    Rev 1.1   17 Sep 1997 09:11:42   Jay
// Bitmap rotation, PNG
// 
//    Rev 1.0   14 Aug 1997 15:21:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:38   Fred
// Initial revision.
// 
//    Rev 1.15   23 Jun 1997 12:37:10   Fred
// Object properties (color) support
// 
//    Rev 1.14   16 Jun 1997 15:04:22   Jay
// Acquire now works
// 
//    Rev 1.13   05 Jun 1997 16:48:26   Jay
// Transparent bitmaps and PostScript.
// 
//    Rev 1.12   09 Apr 1997 17:13:28   Jay
// LockGraphicRecord
// 
//    Rev 1.11   04 Mar 1997 17:03:34   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.10   25 Feb 1997 17:01:16   Jay
// More crop goodies.
// 
//    Rev 1.9   21 Jan 1997 14:25:42   dennisp
// Added object caching to ::Update() so Groups get cached.
// 
//    Rev 1.8   04 Nov 1996 16:06:16   Jay
// JPEG is opaque!
// 
//    Rev 1.7   01 Nov 1996 10:26:34   Jay
// Photoshop support
// 
//    Rev 1.6   16 Oct 1996 13:05:16   Jay
// Fixed direct 24-bit graphic with color.
// 
//    Rev 1.5   15 Oct 1996 08:54:32   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.4   10 Oct 1996 14:30:38   Jay
// More time stamps
// 
//    Rev 1.3   07 Oct 1996 17:02:34   Jay
// TimeStamp
// 
//    Rev 1.2   20 Sep 1996 13:41:20   Jay
// Support for Temp DIB graphic records.
// 
//    Rev 1.1   16 May 1996 14:24:38   Jay
// JPEG
// 
//    Rev 1.0   14 Mar 1996 13:43:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:26   FRED
// Initial revision.
// 
//    Rev 2.7   09 Oct 1995 15:15:02   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.6   18 Aug 1995 15:05:58   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.5   10 Aug 1995 14:48:08   JAY
// Saves and restores the file position around the UpdateRotateHandle() call.
// 
//    Rev 2.4   08 Aug 1995 17:34:26   JAY
// Support for setting the rotate handle on a dynamic basis.
// 
//    Rev 2.3   19 Apr 1995 16:05:04   JAY
// Removed some symbols.
// 
//    Rev 2.2   29 Mar 1995 08:58:44   JAY
// Lots of changes
// 
//    Rev 2.1   15 Feb 1995 17:03:14   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:48   JAY
// Initial revision.
// 
//    Rev 1.14   30 Jan 1995 09:18:04   JAY
// Now uses attribute DC when necessary.
// 
//    Rev 1.13   09 Jan 1995 16:27:12   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.12   30 Nov 1994 16:34:26   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.11   04 Aug 1994 07:33:22   JAY
// Large model and 32-bit changes
// 
//    Rev 1.10   07 Jul 1994 11:24:40   JAY
// Now empties (frees) the error array in destructor.
// 
//    Rev 1.9   09 Jun 1994 11:07:56   JAY
// Now checks for DoesNotExist in addition to Open ERRORCODE.
// 
//    Rev 1.8   01 Jun 1994 16:35:28   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.7   23 May 1994 08:32:00   JAY
// new_graphic_record() now take a CREATE_GRAPHIC_STRUCT pointer, not just
// the name.
// 
//    Rev 1.6   11 Mar 1994 14:23:58   JAY
// Additional code for WMF support.
// 
//    Rev 1.5   17 Feb 1994 09:24:50   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.4   31 Jan 1994 07:15:00   JAY
// Removed od('s for release version.
// 
//    Rev 1.3   13 Jan 1994 17:00:26   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:05:52   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:10:14   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:25:04   JAY
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"

#include <string.h>

#include "pmgdb.h"
#include "docrec.h"

#include "grafobj.h"

#include "image.h"
#include "pcx.h"
#include "tiff.h"
#include "bmp.h"
#include "gif.h"
#include "halo.h"
#include "cgm.h"
#include "wpg.h"
#include "wmf.h"
#include "pcd.h"
#include "jpeg.h"
#include "psd.h"
#include "pngimpl.h"
#include "utils.h"
#include "cdibdc.h"

extern CPalette *pOurPal;
extern BYTE far screen_gamma_curve[];

PRIVATE BYTE
missing_pattern[16] =
{
#if 1
	0x3c,0x00,			/* 00111100 (padded) */
	0x66,0x00,			/* 01100110 (padded) */
	0x66,0x00,			/* 01100110 (padded) */
	0x0c,0x00,			/* 00001100 (padded) */
	0x18,0x00,			/* 00011000 (padded) */
	0x00,0x00,			/* 00000000 (padded) */
	0x18,0x00,			/* 00011000 (padded) */
	0x00,0x00			/* 00000000 (padded) */
#else
	0xc3,0x00,			/* 11000011 (padded) */
	0x99,0x00,			/* 10011001 (padded) */
	0x99,0x00,			/* 10011001 (padded) */
	0xf3,0x00,			/* 11110011 (padded) */
	0xe7,0x00,			/* 11100111 (padded) */
	0xff,0x00,			/* 11111111 (padded) */
	0xe7,0x00,			/* 11100111 (padded) */
	0xff,0x00			/* 11111111 (padded) */
#endif
};

/*****************************************************************************/
/*                              GraphicObject                                */
/*****************************************************************************/

BOOL GraphicObject::m_fDirectBitmapPrinting = FALSE;

/*
// The creator for a graphic object.
*/

ERRORCODE GraphicObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	GraphicObjectPtr object = NULL;
	ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

	if ((object = new GraphicObject(type, owner)) == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* We created the object. */
		if ((error = object->handle_creation_data(creation_data)) != ERRORCODE_None)
		{
			delete object;
			object = NULL;
		}
	}

	*record = (PageObjectPtr)object;
	return error;
}

/*
// Handle the creation data.
*/
ERRORCODE GraphicObject::handle_creation_data(VOIDPTR creation_data)
{
	ERRORCODE error = ERRORCODE_None;

	if (creation_data != NULL)
	{
		PMGDatabasePtr db = (PMGDatabasePtr)database;

	/* We want a full object. */

		DB_RECORD_NUMBER g_record;

		if ((g_record = db->new_graphic_record((GRAPHIC_CREATE_STRUCT_PTR)creation_data)) == NULL)
		{
			error = db->last_creation_error();

//			od("Unable to create graphic '%s' (%d)\r\n", creation_data, error);
		}
		else
		{
#if 0
			GraphicPtr graphic = (GraphicPtr)db->get_record(g_record, &error, RECORD_TYPE_Graphic);
			if (graphic == NULL)
			{
				return error;
			}
			else
			{
				// See if we need to crop the copyright off the bottom of image
				if (graphic->record.flags & GraphicRecord::FLAG_SpecialCrop)
				{
					double ysize = graphic->record.y_size;
					record.m_VisibleRect.bottom = (ysize - AG_COPYRIGHT_CROP_HEIGHT) / ysize;
				}
				graphic->release();
			}
#endif
			set_graphic_record(g_record);
			CropIfNeeded();

//			od("Got data %ld for %s\r\n", get_graphic_record(), creation_data);
		}
	}
	return error;
}

ERRORCODE GraphicObject::CropIfNeeded()
{
	ERRORCODE error;
	GraphicPtr graphic = (GraphicPtr)database->get_record(grecord.g_record, &error, RECORD_TYPE_Graphic);
	if (graphic == NULL)
		return error;

	// See if we need to crop the copyright off the bottom of image
	if (graphic->record.flags & GraphicRecord::FLAG_SpecialCrop)
	{
		double ysize = graphic->record.y_size;
		record.m_VisibleRect.bottom = (ysize - graphic->m_nCopyrightCropHeight) / ysize;
	}
	graphic->release();

	return ERRORCODE_None;
}


/*
// The constructor for the graphic object.
*/

GraphicObject::GraphicObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner),
		  error_array_size(0),
		  error_info(NULL)
{
	record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
#if 1
									| SELECT_FLAG_rotate_handle
#endif
									;

	grecord.element_count = 1;
	grecord.g_record = 0;
	grecord.color = COLOR_WHITE;
	grecord.bcolor = COLOR_BLACK;
	grecord.graphic_flags = 0;
	grecord.render_method = ERROR_DIFFUSION;

	refresh_flags = REFRESH_FLAG_makes_image;

	allocate_error_array(1);
	m_DrawnTimeStamp.Invalidate();
}

/*
// The destructor for a graphic object.
*/

GraphicObject::~GraphicObject()
{
/* Free any error data we have. */
	allocate_error_array(0);
}

/*
// Show this graphic as a pattern (or solid).
*/

UpdateStatePtr GraphicObject::draw_pattern_image(LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate, BYTE *pattern)
{
/* Only show a replacement pattern if not printing. */
	if (rc->IsScreen())
	{
		PBOX bound;
		RECT r;
		HDC hdc = rc->destination_hdc;

	/* Convert the bound to destination coordinates. */

		bound = get_bound();
		rc->pbox_to_screen(&bound, TRUE);
		rc->convert_pbox(&bound, &r, clip);

		HBRUSH hBrush = NULL;
		COLORREF colorref = colorref_from_color(get_bcolor());

		if (pattern != NULL)
		{
			HBITMAP hbm;

			if ((hbm = CreateBitmap(8, 8, 1, 1, pattern)) != NULL)
			{
				hBrush = CreatePatternBrush(hbm);
				DeleteObject(hbm);
			}
		}

		if (hBrush == NULL)
		{
			hBrush = CreateSolidBrush(colorref);
		}

		if (hBrush != NULL)
		{
			HBRUSH hOldBrush;
		 	if ((hOldBrush = (HBRUSH)SelectObject(hdc, hBrush)) != NULL)
			{
				rc->toggle(FALSE, &r);

				COLORREF oldtc = SetTextColor(hdc, colorref);
				COLORREF oldbc = SetBkColor(hdc, RGB(255, 255, 255));
				PatBlt(hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, PATCOPY);
				SetBkColor(hdc, oldbc);
				SetTextColor(hdc, oldtc);

				rc->toggle(TRUE, &r);

				SelectObject(hdc, hOldBrush);
			}

			DeleteObject(hBrush);
		}
	}

	if (ustate != NULL)
	{
		delete ustate;
	}
	return NULL;
}

/*
// Draw the in-memory image for this graphic.
*/

UpdateStatePtr GraphicObject::draw_memory_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate)
{
/*
// This graphic has an in-memory bitmap.
// Due to this usage's origins in the DOS world, the rows of the bitmap are
// only byte-aligned. For Windows, we need to have word-aligned bitmap. So
// we convert if necessary.
*/
	SHORT x_size = graphic->record.x_size;
	SHORT y_size = graphic->record.y_size;

	SHORT byte_width = (x_size + 7)/8;
	SHORT word_width = (byte_width + 1) & ~1;
	LPBYTE bm = (LPBYTE)graphic->bitmap;
	LPBYTE data = NULL;

	if (byte_width == word_width)
	{
		data = bm;
	}
	else
	{
		TRY
			data = new BYTE[word_width*y_size];
		END_TRY
		if (data != NULL)
		{
		/* Copy the data over. */
			LPBYTE s, d;
			SHORT y;

			for (s = bm, d = data, y = 0;
 									y < y_size;
 									s += byte_width, d += word_width, y++)
			{
				memcpy(d, s, byte_width);
			}
		}
	}

/* Do the Blt if we have data. */

	if (data != NULL)
	{
	/* Convert the bound to destination coordinates. */

		PBOX bound = get_bound();
		RECT r;

		rc->pbox_to_screen(&bound, TRUE);
		rc->convert_pbox(&bound, &r, NULL);

		HDC dest_hdc = rc->destination_hdc;
		HDC source_hdc;

		if ((source_hdc = CreateCompatibleDC(rc->AttributeDC())) != NULL)
		{
			HBITMAP hBitmap;

			if ((hBitmap = CreateBitmap(x_size, y_size, 1, 1, data)) != NULL)
			{
				HBITMAP hOldBitmap;

				if ((hOldBitmap = (HBITMAP)SelectObject(source_hdc, hBitmap)) != NULL)
				{
					FLAGS flags = get_flags();
					COLORREF bcolorref = colorref_from_color(get_bcolor());
					COLORREF colorref = colorref_from_color(get_color());

					rc->toggle(FALSE, &r);

					RECT dst_r;

					if (flags & OBJECT_FLAG_xflipped)
					{
						dst_r.left = r.right-1;
						dst_r.right = r.left-1;
					}
					else
					{
						dst_r.left = r.left;
						dst_r.right = r.right;
					}

					if (flags & OBJECT_FLAG_yflipped)
					{
						dst_r.top = r.bottom-1;
						dst_r.bottom = r.top-1;
					}
					else
					{
						dst_r.top = r.top;
						dst_r.bottom = r.bottom;
					}

					COLORREF oldtc = SetTextColor(dest_hdc, bcolorref);
					COLORREF oldbc = SetBkColor(dest_hdc, colorref);
					StretchBlt(dest_hdc,
 									dst_r.left, dst_r.top,
									dst_r.right-dst_r.left,
									dst_r.bottom-dst_r.top,
									source_hdc,
 									0, 0,
									x_size, y_size,
									SRCCOPY);
					SetBkColor(dest_hdc, oldbc);
					SetTextColor(dest_hdc, oldtc);

					rc->toggle(TRUE, &r);

					SelectObject(source_hdc, hOldBitmap);
				}
				DeleteObject(hBitmap);
			}
			DeleteDC(source_hdc);
		}

	/* Free data if we allocated. */

		if (data != bm)
		{
			delete [] data;
		}
	}

	if (ustate != NULL)
	{
		delete ustate;
	}
	return NULL;
}

/*
// Draw the actual source image for this graphic.
*/

UpdateStatePtr GraphicObject::draw_source_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate, PBOX* pExtent)
{
/* Draw the object. */

	if (graphic->record.image != 0)
	{
		GraphicImageDriver ImageDriver(graphic);

		if (ImageDriver.Init())
		{
		/* We will paint from the image. */
			return PaintBitmapImage(clip, graphic, &ImageDriver, rc, ustate, pExtent);
		}
	}

	switch (graphic->record.type)
	{
		case GRAPHIC_TYPE_PCX:
		{
			PCXDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_TIFF:
		case GRAPHIC_TYPE_EPS:
		{
			TIFFDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_BMP:
		{
			BMPDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_GIF:
		{
			GIFDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_HALO:
		{
			HALODriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_CGM:
		{
			CGMUpdateStatePtr cgm_ustate;

			if ((cgm_ustate = (CGMUpdateStatePtr)ustate) == NULL)
			{
				cgm_ustate = new CGMUpdateState(graphic);
			}

			COutlinePaintParams Params;

			Params.FromGraphicObject(this);
			Params.m_pClip = clip;
			Params.m_pRC = rc;
			Params.m_pGraphic = graphic;

			ustate = cgm_ustate->paint(Params, ustate);
			break;
		}
		case GRAPHIC_TYPE_WPG:
		{
			WPGUpdateStatePtr wpg_ustate;

			if ((wpg_ustate = (WPGUpdateStatePtr)ustate) == NULL)
			{
				wpg_ustate = new WPGUpdateState(graphic);
			}

			COutlinePaintParams Params;

			Params.FromGraphicObject(this);
			Params.m_pClip = clip;
			Params.m_pRC = rc;
			Params.m_pGraphic = graphic;

			ustate = wpg_ustate->paint(Params, ustate);
			break;
		}
		case GRAPHIC_TYPE_WMF:
		{
			WMFUpdateState* wmf_ustate;

			if ((wmf_ustate = (WMFUpdateState*)ustate) == NULL)
			{
				wmf_ustate = new WMFUpdateState(graphic);
			}

			COutlinePaintParams Params;

			Params.FromGraphicObject(this);
			Params.m_pClip = clip;
			Params.m_pRC = rc;
			Params.m_pGraphic = graphic;

			ustate = wmf_ustate->paint(Params, ustate);
			break;
		}
		case GRAPHIC_TYPE_PCD:
		{
			PCDDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_JPEG:
		{
			JPEGDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_PSD:
		{
			PSDDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		case GRAPHIC_TYPE_PNG:
		{
			PNGDriver driver(graphic);

			ustate = PaintBitmapImage(clip, graphic, &driver, rc, ustate, pExtent);
			break;
		}
		default:
		{
			ustate = draw_pattern_image(clip, rc, ustate);
			break;
		}
	}
	return ustate;
}

/*
// Draw the DIB image for this graphic.
*/

UpdateStatePtr GraphicObject::draw_dib_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate)
{
/* Draw the object. */

	if (graphic->m_pTempDIB == NULL)
	{
		ASSERT(ustate == NULL);
		return NULL;
	}
	else
	{
		DIBImageDriver ImageDriver(graphic->m_pTempDIB);
		return PaintBitmapImage(clip, graphic, &ImageDriver, rc, ustate);
	}
}

/*
// Draw the image (alternate source) for this graphic.
// Mostly used by Photo CD.
// (Sorry for the name confusion.)
*/

UpdateStatePtr GraphicObject::draw_graphic_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate)
{
	return NULL;
}

/*
// Update a graphic object.
*/

UpdateStatePtr GraphicObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	GraphicPtr  graphic;
	ERRORCODE   error;
   FLAGS       refreshFlags = get_refresh_flags(); // DGP
   BOOL        bMakingImage = FALSE;

	PBOX DrawBound = GetDrawBound();
	PBOX Bound = get_bound();
	PBOX ClippedExtent;

	if (!IntersectBox(&ClippedExtent, extent, &Bound)
			|| !IntersectBox(&ClippedExtent, &ClippedExtent, &DrawBound))
	{
		ASSERT(ustate == NULL);
		delete ustate;			// Just in case.
		return NULL;
	}

	PBOX box = ClippedExtent;
	CRect crDraw;
	rc->pbox_to_screen(&box, FALSE);
	if (!rc->convert_pbox(&box, &crDraw, NULL))
	{
		ASSERT(ustate == NULL);
		delete ustate;			// Just in case.
		return NULL;
	}

	extent = &ClippedExtent;
	ASSERT(clip != NULL);
	if (clip != NULL)
	{
		if (!crDraw.IntersectRect(crDraw, clip))
      {
		   ASSERT(ustate == NULL);
		   delete ustate;			// Just in case.
		   return NULL;
      }
	}
	clip = &crDraw;

   // IF not in the middle of drawing something AND
   // we allow cached images (not printing) AND
   // object has a cached image 
   // THEN try to draw cached image - DGP
   if(ustate == NULL && rc->using_images &&
    (refreshFlags & REFRESH_FLAG_has_image))
    {
      // Draw cached image if one exists
      error = draw_image(rc, *extent);
      if(error == ERRORCODE_None)
        return NULL;              // Successfully displayed cached image
      // If error, fall thru and do normal drawing operation
    }

	if ((graphic = (GraphicPtr)database->get_record(grecord.g_record, &error, RECORD_TYPE_Graphic)) == NULL)
	{
		return graphic_error_state(ustate, UPDATE_TYPE_Error, error);
	}

   // Create cache for image if context wants us to (using_images == TRUE)
   // note: using_images == FALSE when printing - DGP
   if(rc->using_images && (refreshFlags & REFRESH_FLAG_makes_image))
	{
		if(ustate == NULL)
		{
			/* First time through, init the image. */
			begin_image(rc, *extent);
		}
		bMakingImage = making_image();
	}

	if (rc->want_update_names())
	{
		rc->update_name(this, graphic->m_csName);
	}

	for (;;)
	{
		switch (graphic->record.storage)
		{
			case GRAPHIC_STORAGE_MEMORY:
			{
				free_image();
				ustate = draw_memory_image(graphic, clip, rc, ustate);
				break;
			}
			case GRAPHIC_STORAGE_FILE:
			{
				ustate = draw_source_image(graphic, clip, rc, ustate, extent);
				break;
			}
			case GRAPHIC_STORAGE_TEMPDIB:
			{
				ustate = draw_dib_image(graphic, clip, rc, ustate);
				break;
			}
/*			case GRAPHIC_STORAGE_MISSING:*/
			default:
			{
				free_image();
				ustate = draw_pattern_image(clip, rc, ustate, missing_pattern);
				break;
			}
		}

		if (ustate != NULL
         && ustate->type == UPDATE_TYPE_Error
         && (ustate->error == ERRORCODE_Open || ustate->error == ERRORCODE_DoesNotExist))
		{
		/*
		// We got a return value of NoFile. The graphic was missing and the user
		// picked a new one. We need to go back and update the graphic again,
		// and without any kind of update information.
		*/
			ustate = NULL;
		}
		else
		{
		/* Something else; jump out. */
			break;
		}
	}

	// Update the time stamp.
	m_DrawnTimeStamp = graphic->GetTimeStamp();

	graphic->release();

   // if caching object then check if we're finished drawing 
   // or an error occurred in drawing process - DGP
   if(bMakingImage)
   {
    // if object finished drawing, commit cache to disk
   	if (ustate == NULL)
      {
        /*
        // An object has been drawn entirely on
        // screen. We can make an image from this.
        */
        end_image(rc);
      }
      else if (ustate->type == UPDATE_TYPE_Error)
      {
        /* The image is probably bogus. */
        free_image();
      }
      // If we fall thru then interupt probably occurred
   }

	return ustate;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE GraphicObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = RectPageObject::read_data(device)) == ERRORCODE_None)
	{
		if ((error = device->read_record(&grecord, sizeof(grecord))) == ERRORCODE_None)
		{
		/*
		// We want to update the rotate handle.
		// This can cause the file position to change within the device.
		// So we save and restore it around the UpdateRotateHandle() call.
		*/
			ST_DEV_POSITION pos;
			device->tell(&pos);
			UpdateRotateHandle();
			device->seek(pos, ST_DEV_SEEK_SET);
		}
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE GraphicObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = RectPageObject::write_data(device)) == ERRORCODE_None)
	{
		error = device->write_record(&grecord, sizeof(grecord));
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE GraphicObject::size_data(StorageDevicePtr device)
{
	return RectPageObject::size_data(device) + device->size_record(sizeof(grecord));
}

void GraphicObject::set_graphic_record(DB_RECORD_NUMBER n)
{
	grecord.g_record = n;

	UpdateRotateHandle();
}

/*
// Lock the graphic record for this object.
// The caller must do a release().
*/

GraphicRecord* GraphicObject::LockGraphicRecord(void)
{
	DB_RECORD_NUMBER lRecord = get_graphic_record();
	// We must have a record attached.
	if (lRecord == 0)
	{
		return NULL;
	}
	// Lock and return the graphic record.
	return (GraphicRecord*)get_database()->get_record(lRecord, NULL, RECORD_TYPE_Graphic);
}

void GraphicObject::UpdateRotateHandle(void)
{
	GraphicPtr graphic;
	ERRORCODE error;
	BOOL fIsRotatable = FALSE;

	if ((record.select_flags & SELECT_FLAG_size_handles)
		 && grecord.g_record > 0
		 && (graphic = (GraphicPtr)database->get_record(grecord.g_record, &error, RECORD_TYPE_Graphic)) != NULL)
	{
		switch (graphic->record.type)
		{
			case GRAPHIC_TYPE_CGM:
			{
				fIsRotatable = TRUE;
				break;
			}
			default:
			{
				break;
			}
		}
		graphic->release();
	}

///
	fIsRotatable = TRUE;
///

	if (fIsRotatable)
	{
		record.select_flags |= SELECT_FLAG_rotate_handle;
	}
	else
	{
		record.select_flags &= ~SELECT_FLAG_rotate_handle;
	}
}

/*
// Allocate the error diffusion array.
*/

BOOL GraphicObject::allocate_error_array(SHORT new_count)
{
	SHORT i;

/* Set the new count. */

	if (new_count != -1)
	{
		grecord.element_count = new_count;
	}

/* Free an old array if necessary. */

	if (error_info != NULL)
	{
		for (i = 0; i < error_array_size; i++)
		{
			if (error_info[i] != NULL)
			{
				delete [] (LPBYTE)error_info[i];
				error_info[i] = NULL;
			}
		}
		if (error_array_size == grecord.element_count)
		{
			return TRUE;
		}
		delete [] error_info;
		error_info = NULL;
	}

/* Allocate a new array. */

	if (grecord.element_count != 0)
	{
		TRY
			error_info = new ERROR_INFO_PTR[grecord.element_count];
		END_TRY
		if (error_info == NULL)
		{
			return FALSE;
		}
		memset(error_info, 0, grecord.element_count*sizeof(ERROR_INFO_PTR));
	}
	error_array_size = grecord.element_count;
	return TRUE;
}

/*
// The object is being destroyed. Destroy all resources.
*/

VOID GraphicObject::destroy(VOID)
{
	if (grecord.g_record != 0)
	{
		((PMGDatabasePtr)database)->free_graphic_record(grecord.g_record);
		grecord.g_record = 0;
	}
	allocate_error_array(0);
	RectPageObject::destroy();
}

/*
// Is this object opaque?
*/

BOOL GraphicObject::is_opaque(RedisplayContextPtr rc)
{
	GraphicPtr graphic;
	BOOL opaque = FALSE;

	if (get_rotation() == 0)
	{
		if ((graphic = (GraphicPtr)database->get_record(grecord.g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
		{
			BOOL fCovering = record.m_VisibleRect.left >= 0
								&& record.m_VisibleRect.top >= 0
								&& record.m_VisibleRect.right <= 1.0
								&& record.m_VisibleRect.bottom <= 1.0;

			switch (graphic->record.type)
			{
				case GRAPHIC_TYPE_PCX:
				case GRAPHIC_TYPE_HALO:
				case GRAPHIC_TYPE_BMP:
				case GRAPHIC_TYPE_TIFF:
				case GRAPHIC_TYPE_PCD:
				case GRAPHIC_TYPE_JPEG:
				{
					// Make sure we cover all our bound.
					if (fCovering)
					{
						opaque = TRUE;
					}
					break;
				}
				case GRAPHIC_TYPE_GIF:
				{
					// See if this GIF file has transparency.
					GIFHelper* pHelper = (GIFHelper*)graphic->my_little_helper();
					opaque = fCovering && !pHelper->record.m_fHasTransparency;
					break;
				}
				default:
				{
					opaque = PMGPageObject::is_opaque(rc);
					break;
				}
			}
			graphic->release();
		}
	}
	return opaque;
}

BOOL GraphicObject::IsEmpty()
{
   BOOL bIsEmpty = TRUE;
   GraphicRecord* pGraphicRecord;

   pGraphicRecord = LockGraphicRecord();
   if(pGraphicRecord == NULL)
      return bIsEmpty;

   if(pGraphicRecord->record.storage == GRAPHIC_STORAGE_MISSING ||
      pGraphicRecord->record.storage == GRAPHIC_STORAGE_UNKNOWN)
         bIsEmpty = TRUE;
   else
         bIsEmpty = FALSE;

   pGraphicRecord->release();

   return bIsEmpty;
}

/*
// Assign an object to this.
*/

ERRORCODE GraphicObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;

	GraphicObject& gobject = (GraphicObject&)sobject;

/* Copy the base object data. */

	if ((error = RectPageObject::assign(sobject)) == ERRORCODE_None)
	{
	/* Copy the specific graphic data. */

		grecord = gobject.grecord;

	/* Increment the reference for our graphic data. */

		if (database == gobject.database)
		{
			((PMGDatabasePtr)database)->inc_graphic_record(grecord.g_record);
		}
		else
		{
			DocumentPtr pDocument;
			ERRORCODE error;

		/*
		// Try to lock the document so we can access it.
		// This should always work, but we'll check for error anyway.
		*/

			if ((pDocument = (DocumentPtr)database->get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) == NULL)
			{
				return error;
			}

		/*
		// Get the source graphic so we can look it up in the document list.
		// If it's there, we just bump the ref count.
		// If not, we copy it over.
		*/

			DB_RECORD_NUMBER g_record;

			GraphicPtr pGraphic;

			if ((pGraphic = (GraphicPtr)gobject.database->get_record(gobject.get_graphic_record(),
								&error, RECORD_TYPE_Graphic)) != NULL)
			{
			/*
			// Try to find the graphic in the document.
			*/

				if ((g_record = pDocument->find_graphic(pGraphic->m_csFileName)) == 0)
				{
				/*
				// Duplicate the record.
				// Extreme care must be taken to maintain the correct database at all times.
				*/

					GraphicPtr pNewGraphic = (GraphicPtr)pGraphic->duplicate(database, &error);

				/* See if we got the new graphic. */

					if (pNewGraphic != NULL)
					{
					/* Set it in the graphic object. */

						g_record = pNewGraphic->Id();
						set_graphic_record(g_record);

						pNewGraphic->release();

					/* Insert the graphic number into the document graphic array. */

						pDocument->add_graphic(g_record);
					}
				}
				else
				{
				/* Found it. Just bump the record ref count. */
					((PMGDatabasePtr)database)->inc_graphic_record(g_record);
					set_graphic_record(g_record);
				}
				pGraphic->release();
			}
			pDocument->release();
		}

	/* Make sure the error array is fully allocated. */

		allocate_error_array(-1);
	}
	return error;
}

/*
// Return the original dimensions of an object.
*/

BOOL GraphicObject::original_dims(PPNT_PTR p)
{
	DB_RECORD_NUMBER g_record = get_graphic_record();
	GraphicPtr graphic;

	if ((graphic = (GraphicPtr)database->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
	{
		p->x = scale_pcoord(graphic->record.x_size, PAGE_RESOLUTION, graphic->record.x_resolution);
		p->y = scale_pcoord(graphic->record.y_size, PAGE_RESOLUTION, graphic->record.y_resolution);
		graphic->release();
		return TRUE;
	}
	return FALSE;
}

/*
// Get an object's name.
*/

BOOL GraphicObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
	GraphicPtr graphic;

/* Lock the graphic data so we can know its name. */

	if ((graphic = (GraphicPtr)database->get_record(get_graphic_record(),
										NULL,
										RECORD_TYPE_Graphic)) == NULL)
	{
		return FALSE;
	}

	strncpy(buffer, graphic->m_csName, size-1);

	graphic->release();

/* Make sure it's terminated. */

	buffer[size-1] = '\0';

	return TRUE;
}

/*
// Begin the generation of an object image.
*/
ERRORCODE GraphicObject::begin_image(RedisplayContextPtr rc, PBOX world)
{
	GraphicPtr graphic;
	ERRORCODE error;

	if ((graphic = (GraphicPtr)database->get_record(get_graphic_record(), &error, RECORD_TYPE_Graphic)) == NULL)
	{
		return error;
	}

	if (graphic->record.storage != GRAPHIC_STORAGE_FILE)
	{
	/* Not something we make an image of. */
		graphic->release();
		return ERRORCODE_IllegalType;
	}

	graphic->release();

	return PMGPageObject::begin_image(rc, world);
}

/*
// Get the object's time stamp.
*/

CTimeStamp GraphicObject::GetTimeStamp(void)
{
	CTimeStamp TimeStamp;
	TimeStamp.Invalidate();

	GraphicPtr graphic;
	if ((graphic = (GraphicPtr)database->get_record(get_graphic_record(), NULL, RECORD_TYPE_Graphic)) != NULL)
	{
		TimeStamp = graphic->GetTimeStamp();
		graphic->release();
	}
	return TimeStamp;
}

/*
// Update the object's time stamp.
*/

BOOL GraphicObject::UpdateTimeStamp(void)
{
	BOOL fChanged = FALSE;
	GraphicPtr graphic;
	if ((graphic = (GraphicPtr)database->get_record(get_graphic_record(), NULL, RECORD_TYPE_Graphic)) != NULL)
	{
		// Get the current time stamp.
		CTimeStamp TimeStamp = graphic->GetTimeStamp();

		// Update the time stamp.
		graphic->reinit(TRUE);

		// See if we changed.
		CTimeStamp NewTimeStamp = graphic->GetTimeStamp();
		fChanged = (NewTimeStamp != TimeStamp) || (NewTimeStamp != m_DrawnTimeStamp);

		graphic->release();
	}

   return fChanged;
}

PageObjectState* GraphicObject::CreateState(void)
{
	return new GraphicObjectState;
}

void GraphicObject::SaveState(PageObjectState* pState)
{
	GraphicObjectState* pMyState = (GraphicObjectState*)pState;

	INHERITED::SaveState(pState);

	pMyState->m_GRecord = grecord;
	if (grecord.g_record > 0)
	{
		pMyState->m_pDatabase->inc_graphic_record(grecord.g_record);
	}
}

BOOL GraphicObject::ImageChanged(GraphicObjectState* pState)
{
	return pState->m_GRecord.g_record != grecord.g_record
			|| pState->m_GRecord.color != grecord.color
			|| pState->m_GRecord.bcolor != grecord.bcolor
			|| pState->m_GRecord.render_method != grecord.render_method
			|| pState->m_GRecord.operation != grecord.operation
			|| pState->m_GRecord.graphic_flags != grecord.graphic_flags;
}

void GraphicObject::RestoreState(PageObjectState* pState)
{
	GraphicObjectState* pMyState = (GraphicObjectState*)pState;

	if (ImageChanged(pMyState))
	{
		changed_image();
	}

	INHERITED::RestoreState(pState);

	DB_RECORD_NUMBER new_grecord = pMyState->m_GRecord.g_record;

/* Maintain the reference counts on the graphic records. */

	if (new_grecord != grecord.g_record)
	{
		PMGDatabase* pDatabase = pMyState->m_pDatabase;

	/* In with the new. */
		if (new_grecord > 0)
		{
			pDatabase->inc_graphic_record(new_grecord);
		}
	/* Out with the old. */
		if (grecord.g_record > 0)
		{
			pDatabase->free_graphic_record(grecord.g_record);
		}
	}
	grecord = pMyState->m_GRecord;
}

/*
// Get the properties for this object.
*/

void GraphicObject::GetObjectProperties(CObjectProperties& Properties)
{
	Properties.SetFillForegroundColor(grecord.bcolor);
}

/*
// Set the properties for this object.
*/

BOOL GraphicObject::SetObjectProperties(CObjectProperties& Properties)
{
	BOOL fSet = FALSE;
	COLOR c;

	c = grecord.bcolor;
	fSet |= Properties.ApplyFillForegroundColor(c);
	grecord.bcolor = c;

	if (fSet)
	{
		changed_image();
	}

	return fSet;
}

/*
// Set the flag for direct bitmap printing.
// Currently this is global, so the function is static.
*/

void GraphicObject::SetDirectBitmapPrinting(BOOL fDirectBitmapPrinting)
{
	m_fDirectBitmapPrinting = fDirectBitmapPrinting;
}

/*
// Get the flag for direct bitmap printing.
// This is used for dialog display (i.e. user interface).
*/

BOOL GraphicObject::GetDirectBitmapPrinting(void)
{
	return m_fDirectBitmapPrinting;
}

/*
// Get the direct bitmap printing flag for this object.
// This requires a redisplay context in order to make intelligent
// decisions based on the current output state.
*/

BOOL GraphicObject::DirectBitmapPrint(RedisplayContext* pContext)
{
/* We currently only do this is printing (may change some day) */
#if 1
	return ((pContext->is_printing
					&& m_fDirectBitmapPrinting)
				|| pContext->is_metafile)
					&& get_rotation() == 0.0;
#else
	return TRUE;
#endif
}

/*
// The error state for a graphic.
*/

PUBLIC UpdateStatePtr 
graphic_error_state(UpdateStatePtr ustate, UPDATE_TYPE type, ERRORCODE error)
{
	return error_update_state(ustate, type, error);
}

//
// Paint a bitmap graphic.
// This primarily exists to handle transparent bitmap graphics on PostScript
// printers.
//

UpdateStatePtr GraphicObject::PaintBitmapImage(LPRECT clip,
															  GraphicPtr record, ImageDriverPtr driver,
															  RedisplayContextPtr rc, UpdateStatePtr ustate,
															  PBOX* pExtent /*=NULL*/)
{
	CPmwDoc* pDoc = (CPmwDoc*)((PMGDatabase*)database)->GetDocument();

	if (pDoc != NULL
       && record->my_little_helper() != NULL
		 && record->my_little_helper()->HasTransparency()
		 && rc->is_postscript
		 && rc->is_printing)
	{
		ASSERT(ustate == NULL);

		//
		// We need to create a composite image of this graphic and the
		// background. Then, we can do a straight copy.
		//

		PBOX rb;
		REFRESH_EXTENT re;

	// Get the refresh bound. This should be everything possible for the frame.
	// (The special case where an RC is passed.)
		get_refresh_bound(&rb, rc);
		if (pExtent == NULL)
		{
			re.extent = rb;
		}
		else
		{
			IntersectBox(&re.extent, &rb, pExtent);
			rb = re.extent;
		}

		// Convert to a screen bound so we can create a bitmap.
		rc->pbox_to_screen(&rb, TRUE);

		RECT rDest;
		rc->convert_pbox(&rb, &rDest, NULL);

		int width = rDest.right - rDest.left;
		int height = rDest.bottom - rDest.top;

		// Update all the objects into the DC.
		HDC hdc = rc->destination_hdc;

		// Create the bitmap.

		HBITMAP hBitmap;
		if ((hBitmap = ::CreateCompatibleBitmap(hdc, width, height)) != NULL)
		{
			BOOL fDrawn = FALSE;

			// Initialize the bitmap.
			HDC hdcTemp;

			if ((hdcTemp = ::CreateCompatibleDC(hdc)) != NULL)
			{
				HBITMAP hOldBitmap;

				if ((hOldBitmap = (HBITMAP)::SelectObject(hdcTemp, hBitmap)) != NULL)
				{
					::PatBlt(hdcTemp, 0, 0, width, height, WHITENESS);

					// Create a redisplay context to draw into.
					RedisplayContext orc;

					orc.destination_hdc = hdcTemp;
					orc.hwnd = NULL;

					orc.x_resolution = rc->x_resolution;
					orc.y_resolution = rc->y_resolution;

					orc.SetScaling(rc->GetScaleNumerator(), rc->GetScaleDenominator());

					orc.destination_rect = rc->destination_rect;

					orc.clip_rect = orc.destination_rect;
					OffsetRect(&(orc.clip_rect), -rDest.left, -rDest.top);

					orc.source_pbox = rc->source_pbox;

					orc.scaled_source_x0 = rc->scaled_source_x0;
					orc.scaled_source_y0 = rc->scaled_source_y0;

					orc.destination_x0 = (short)(rc->destination_x0 - rDest.left);
					orc.destination_y0 = (short)(rc->destination_y0 - rDest.top);

					orc.outline_gamma_curve = rc->outline_gamma_curve;
					orc.bitmap_gamma_curve = rc->bitmap_gamma_curve;

					orc.info = rc->info;

					orc.is_printing = TRUE;
					orc.is_postscript = FALSE;		// But not PostScript!

					// Finish building the refresh extent.

					re.refresh_type = REFRESH_ALL;
					re.my_object = NULL;
					re.update_object = NULL;
					re.update_state = NULL;

					HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
					HPALETTE hOldPal = SelectPalette(hdcTemp, hOurPal, FALSE);

					orc.m_pUptoObject = this;
					pDoc->draw_objects(&re, &orc);
					paint_bitmap_image(this, &orc.clip_rect, record, driver, &orc, ustate);

					StretchBlt(hdc,
								  rDest.left,
								  rDest.top,
								  width,
								  height,
								  hdcTemp,
								  0,
								  0,
								  width,
								  height,
								  SRCCOPY);

					SelectPalette(hdcTemp, hOldPal, FALSE);

					::SelectObject(hdcTemp, hOldBitmap);

					fDrawn = TRUE;
				}

				::DeleteDC(hdcTemp);
			}

			::DeleteObject(hBitmap);

			if (fDrawn)
			{
				// Success!
				return NULL;			// No further update state.
			}
		}
	}
	return paint_bitmap_image(this, clip, record, driver, rc, ustate);
}

/////////////////////////////////////////////////////////////////////////////
// GraphicObjectState

GraphicObjectState::~GraphicObjectState()
{
	if (m_GRecord.g_record > 0)
	{
		m_pDatabase->free_graphic_record(m_GRecord.g_record);
	}
}


/*
// Convert the vector data for this object and write as BMP data.
*/

ERRORCODE GraphicObject::WriteVectorAsBMP(StorageDevice* pDevice, CPmwDoc* pDoc)
{
	PBOX Bounds = GetDrawBound();
	
	ERRORCODE error = ERRORCODE_IntError;
	
	PCOORD DimX = Bounds.x1 - Bounds.x0;
	PCOORD DimY = Bounds.y1 - Bounds.y0;

	int nXScreenResolution = 96;
	int nYScreenResolution = 96;

	DimX = scale_pcoord(DimX, nXScreenResolution, PAGE_RESOLUTION);
	DimY = scale_pcoord(DimY, nYScreenResolution, PAGE_RESOLUTION);

	/*
	// Create the object now.
	*/
	GraphicObjectPtr object;

	
	PMGDatabase* pDatabase = pDoc->get_database(); // current database

	if ((object = pDatabase->create_graphic_object()) == NULL)
		return error;
		/*
		// We have the object.
		//
	 	// Attach the record to the object.
		// The record now belongs to the object.
	 	*/

		
		object->set_graphic_record(get_graphic_record());
		object->set_bound(Bounds);

	CDibDC dc;

	BITMAPINFOHEADER bmi;
	InitBitmapInfoHeader(&bmi, DimX, DimY, 24);

	if (dc.Create((BITMAPINFO*)&bmi))
	{
			
		RedisplayContext rc;
		rc.destination_hdc = dc.GetSafeHdc();
		rc.AttributeDC(dc.GetSafeHdc());
		rc.hwnd = NULL;
		rc.set_info(dc.GetSafeHdc());

		rc.x_resolution = nXScreenResolution;
		rc.y_resolution = nYScreenResolution;

		rc.scaled_source_x0 =  Bounds.x0*rc.x_resolution;
		rc.scaled_source_y0 =  Bounds.y0*rc.y_resolution;

		rc.destination_rect.left =
		rc.destination_rect.top =
		rc.destination_x0 =
		rc.destination_y0 = 0;

		rc.destination_rect.right = (int)DimX;
		rc.destination_rect.bottom = (int)DimY;
		rc.clip_rect = rc.destination_rect;

					rc.outline_gamma_curve =
						rc.bitmap_gamma_curve = screen_gamma_curve;
	
		rc.source_pbox = Bounds;
		rc.is_thumbnail = TRUE;
		
		rc.info.type = RASTER_TYPE_24;
		rc.info.bpp = 24;
		rc.info.planes = 1;
		
		dc.PatBlt(0, 0, (int)DimX, (int)DimY, WHITENESS);
				
		// this should render the vector image into the DC created above
		object->update (&rc, &Bounds, &rc.clip_rect, NULL, REFRESH_ALL);
			
		// get bitmap from rc
		LPBYTE lpbData = dc.GetData();
		
		// write out bmp

		// Init the file header.
		BITMAPFILEHEADER Header;

		DWORD dwHeaderSize = sizeof(Header) + bmi.biSize;

		Header.bfType = (SHORT)'B' + (((SHORT)'M') << 8);
		Header.bfSize = dwHeaderSize + bmi.biSizeImage;
		Header.bfReserved1 = 0;
		Header.bfReserved2 = 0;
  	Header.bfOffBits = dwHeaderSize;

		if ((error = pDevice->write(&Header, sizeof (Header))) == ERRORCODE_None)
			if ((error = pDevice->write(&bmi, sizeof (bmi))) == ERRORCODE_None)
			{
				error = pDevice->huge_write(lpbData, bmi.biSizeImage);
			}
	
	}

	object->set_graphic_record(0);
	object->destroy();			// Remove from the document.
	delete object;
	
	return error;
}

/* Retrieves the original path of the image stored for the graphic.
 */
const CString *GraphicObject::GetOriginalDataPath()
{
	CString *pstrFileName = NULL;

	// Get the record for this graphic.
	DB_RECORD_NUMBER g_record = get_graphic_record();
	GraphicPtr graphic;

	// Get the graphic from the database so we can retrieve its filename.
	if ((graphic = (GraphicPtr)get_database()->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
	{
		/* Store a pointer to the filename for the picture */
		pstrFileName = &graphic->m_csFileName;

		graphic->release();
	}

	// Give back the results to the caller.  NULL if we failed.
	return pstrFileName;
}
