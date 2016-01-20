/*
// OUTLINE.CPP
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/OUTLINE.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/OUTLINE.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 14    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 13    7/21/98 6:09p Rlovejoy
// Fixed y text placement.
// 
// 12    7/12/98 5:33p Jayn
// Fix for WMFs with Polylines drawn with NULL pen.
// 
// 11    5/29/98 2:39p Jayn
// Can now read non-placeable WMFs
// 
// 10    4/17/98 5:53p Jayn
// Fixed a rectangle problem.
// 
// 9     4/07/98 8:44a Jayn
// Default for m_nBltMode.
// 
// 8     2/05/98 11:07a Jayn
// Fixed a missing sign.
// 
// 7     12/17/97 2:02p Hforman
// commented out some TRACE statements
// 
// 6     12/10/97 12:31p Jayn
// Fixes for bitmap rotation.
// 
// 5     12/09/97 4:31p Jayn
// Bitmap and bitmap rotation (incomplete)
// 
// 4     12/08/97 2:52p Jayn
// Text, arcs, pie charts, improved transform, etc.
// 
// 3     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
// 2     11/19/97 3:43p Jayn
// New WMF parser.
// 
//    Rev 1.0   14 Aug 1997 15:23:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:30   Fred
// Initial revision.
// 
//    Rev 1.16   29 Jul 1997 10:34:34   Jay
// Disabled brush caching.
// 
//    Rev 1.15   20 Jul 1997 14:36:58   Fred
// Does not allow handle wrap when cropping; handles empty clip regions
// 
//    Rev 1.14   03 Jul 1997 10:48:08   Jay
// Now uses two polygon fills (for outline and fill).
// 
//    Rev 1.13   23 May 1997 16:13:48   Jay
// Support for nFX winding
// 
//    Rev 1.12   24 Apr 1997 15:08:46   Jay
// Fixed a bug in GetPointBuffer() with large number of points
// 
//    Rev 1.11   26 Mar 1997 15:55:10   Fred
// Removed old geronimo stuff
// 
//    Rev 1.10   24 Mar 1997 10:34:48   Jay
// Fixed pallete/non-palette bug with brush caching.
// 
//    Rev 1.9   19 Feb 1997 16:54:08   Jay
// Beginning of cropping
// 
//    Rev 1.8   22 Jan 1997 10:26:04   dennisp
// Optimized draw_polygon by retaining allocated buffer for repeated calls to
// draw_polygon.  Added GetPointBuffer().
// 
//    Rev 1.7   15 Jan 1997 16:43:22   dennisp
// Added caching of brushes.
// 
//    Rev 1.6   09 Dec 1996 10:19:34   Jay
// More image stuff.
// 
//    Rev 1.5   05 Dec 1996 12:30:18   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.4   04 Nov 1996 16:06:04   Jay
// New object image routines.
// 
//    Rev 1.3   01 Nov 1996 17:11:44   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.2   15 Oct 1996 08:54:34   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.1   07 Oct 1996 13:19:48   Jay
// prep_storage_file()
// 
//    Rev 1.0   14 Mar 1996 13:43:28   Jay
// Initial revision.
// 
//    Rev 1.1   27 Feb 1996 17:06:44   VSI
// _NEWPORTABLE 'C' code replaces .asm -- not yet working
// 
//    Rev 2.10   09 Oct 1995 15:13:48   JAY
// Attempt to not render polylines or polygons with less than 2 points.
// 
//    Rev 2.9   22 Sep 1995 12:01:46   JAY
// Fixed a bug in transform_point for 32-bit.
// 
//    Rev 2.8   26 Jul 1995 07:39:24   JAY
// Uninitialized variable.
// 
//    Rev 2.7   13 Jul 1995 13:38:28   JAY
// Coloring book now draws outlines on everything.
// 
//    Rev 2.6   07 Jul 1995 16:10:40   JAY
// Misc changes.
// 
//    Rev 2.5   26 Jun 1995 09:37:34   JAY
// Passes UsePalette as FALSE to Draw() for non-palette devices.
// 
//    Rev 2.4   23 Jun 1995 18:03:38   JAY
// Now uses polygons to draw rectangles.
// 
//    Rev 2.3   14 Jun 1995 16:26:58   JAY
// Supports rotated ellipses.
// 
//    Rev 2.2   29 Mar 1995 08:58:32   JAY
// Lots of changes
// 
//    Rev 2.1   15 Feb 1995 17:02:06   JAY
// Fixed some alignment problems with boxes and circles (fills vs outlines).
// 
//    Rev 2.0   07 Feb 1995 15:47:58   JAY
// Initial revision.
// 
//    Rev 1.34   30 Jan 1995 09:15:50   JAY
// Now uses attribute DC of RC when necessary.
// 
//    Rev 1.33   30 Dec 1994 14:00:04   JAY
// New dither method with expanded color set (based on WinG palette).
// 
//    Rev 1.32   29 Dec 1994 10:43:30   JAY
// Fixed some problems with interrupted drawing (especially with embedded data).
// 
//    Rev 1.31   30 Nov 1994 16:33:58   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.30   04 Aug 1994 07:32:56   JAY
// Large model and 32-bit changes
// 
//    Rev 1.29   07 Jul 1994 11:30:02   JAY
// Throws half of line width to right now instead of left.
// Doubles polyline lines now to match polygon edge lines.
// 
//    Rev 1.28   28 Jun 1994 10:32:20   JAY
// Centers the edges of objects as best it can (for objects using INSIDEFRAME).
// 
//    Rev 1.27   14 Jun 1994 16:51:32   JAY
// Doesn't use Geronimo anymore (code-wise); it's commented out.
// 
//    Rev 1.26   23 May 1994 08:23:30   JAY
// Moved constructor into here; now calls prep_storage_file() instead of the
// direct init so we can base the file on the document database device if needed.
// 
//    Rev 1.25   16 May 1994 08:42:34   JAY
// Broke the RC gamma curve into two separate curves, one for bitmaps and one
// for outlines.
// 
//    Rev 1.24   11 May 1994 11:56:04   JAY
//  
// 
//    Rev 1.23   07 Apr 1994 10:47:22   JAY
// Checks m_use_ddbs and won't use DDBs if it's FALSE.
// 
//    Rev 1.22   15 Mar 1994 15:13:50   JAY
// Now picks the smaller scaled pen width (x vs y expansion) to keep it from
// getting really large from a single dimension.
// 
//    Rev 1.21   10 Mar 1994 09:55:58   JAY
// Now globally reduces dest_extent.*, not just for polygons and polylines.
// 
//    Rev 1.20   08 Mar 1994 12:27:26   JAY
// Shrinks the extent in when drawing polygons and polylines.
// Rounds polygon and polyline coordinates.
// 
//    Rev 1.19   02 Mar 1994 14:53:46   JAY
// Implemented coloring book option for GDI calls code.
// 
//    Rev 1.18   02 Mar 1994 11:59:22   JAY
// Ignores PostScript stuff if rop is SRCCOPY in draw_mono_map.
// 
//    Rev 1.17   02 Mar 1994 11:18:14   JAY
// Made draw_mono_map public so image.cpp could use it.
// 
//    Rev 1.16   01 Mar 1994 17:54:16   JAY
// Single width lines are converted to at least two pixels if color other than
// black or white is needed.
// 
//    Rev 1.15   01 Mar 1994 15:51:32   JAY
// Now will not attempt to show_outline_drawn() if source_hdc == NULL.
// 
//    Rev 1.14   25 Feb 1994 14:15:24   JAY
// Now creates pens with PS_INSIDEFRAME instead of PS_SOLID.
// Some experimental 8-color code is in but commented out.
// 
//    Rev 1.13   25 Feb 1994 09:43:08   JAY
// If it's unable to create the offscreen bitmaps, drawing proceeds with GDI
// calls straight to the screen.
// 
//    Rev 1.12   24 Feb 1994 14:44:28   JAY
// Fixed the problem of it losing the accumulated images if "gdi_calls" drawing
// was interrupted.
// 
//    Rev 1.11   24 Feb 1994 13:44:40   JAY
// Corrected a problem I introduced with the last rev.
// 
//    Rev 1.10   24 Feb 1994 11:49:14   JAY
// Sets the background color correctly when drawing.
// Handles flipped "gdi-call" renderings correctly (disable all banding code).
// WPGs now handled as old way (so that they work).
// 
//    Rev 1.9   22 Feb 1994 15:02:22   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.8   01 Feb 1994 10:21:00   JAY
// Added dummy FillRect (with NULL_BRUSH) on solo StretchDIBits to fix a bug
// in Windows where dialogs don't know a StretchDIBits occurred behind them.
// 
//    Rev 1.7   31 Jan 1994 17:43:28   JAY
//  
// 
//    Rev 1.6   26 Jan 1994 16:34:14   JAY
// Now deletes the image bitmaps if the object image goes away.
// 
//    Rev 1.5   25 Jan 1994 15:17:50   JAY
// More
// 
//    Rev 1.4   19 Jan 1994 17:06:52   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:00:32   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:06:04   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:10:18   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:25:06   JAY
// 
*/

#include "stdafx.h"

#include <stdlib.h>              // for __min()
#include <stdio.h>               // for sprintf()
#include <math.h>
#include <memory.h>

#include "pmgobj.h"
#include "grafrec.h"
#include "grafobj.h"
#include "outline.h"
#include "utils.h"
#include "dither.h"
#include "outpath.h"
#include "objimg.h"
#include "cdibdc.h"
#include "rotutils.h"
#include "bmp.h"

#define NOTSRCAND 0x00220326L

#define CACHE_OUTLINE_BRUSHESx

#define CENTER_EDGES

/*
// pattern structure
*/

typedef struct
{
   BYTE pattern[8];
} PAT;

typedef PAT far *PATPTR;

extern BOOL init_image_update(void);
extern VOID notify_image_update(SHORT current, SHORT total);

/////////////////////////////////////////////////////////////////////////////
// GDIDrawingSet

/*
// A GDI drawing set class.
*/

struct GDIDrawingSet
{
   HBRUSH hBrush;
   HPEN hPen;
   BOOL stock_brush;
   BOOL stock_pen;
   BOOL fill_is_null;
   BOOL outline_is_null;
   int pen_width;

   GDIDrawingSet(OutlineUpdateStatePtr ous);
   ~GDIDrawingSet();
   BOOL valid()
      { return hBrush != NULL && hPen != NULL; }
};

// Static member linked listed of brushes - DGP
tDoubleLinkedList OutlineUpdateState::m_BrushLinkedList;

/* Constructor. */

GDIDrawingSet::GDIDrawingSet(OutlineUpdateStatePtr ous)
{
   switch (ous->interior_style)
   {
      case 0:              /* Hollow */
      case 4:              /* Empty */
      default:
      {
         hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
         stock_brush = TRUE;
         fill_is_null = TRUE;
         break;
      }
      case 1:                 /* solid */
      case 2:                 /* pattern */
      case 3:                 /* hatch */
      {
         if (ous->coloring_book)
         {
            hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
            stock_brush = TRUE;
         }
         else
         {
            hBrush = ous->create_solid_brush(ous->fill_trans_table[1],
                                          ous->fill_trans_table[2],
                                          ous->fill_trans_table[3]);
            stock_brush = FALSE;
         }
         fill_is_null = FALSE;
         break;
      }
   }

/* Create the pen we need to use. */

   if (ous->edge_visibility || ous->coloring_book)
   {
      pen_width = ous->compute_scaled_width(ous->edge_width);

      if (!ous->edge_visibility)
      {
      /* Use single-width pens for non-outline fills in coloring book. */
         pen_width = 1;
      }

      if (!ous->coloring_book
            && ((ous->edge_trans_table[1] != 0
            && ous->edge_trans_table[1] != 255)
            || ous->edge_trans_table[2] != ous->edge_trans_table[1]
            || ous->edge_trans_table[3] != ous->edge_trans_table[1]))
      {
      /* Not black or white. */
#if 0
         if (pen_width < 2)
         {
         /* Make it at least two so it will dither the color. */
            pen_width = 2;
         }
#endif
      }

//		TRACE(">>> pen_width: %d <<<\n", pen_width);

      if (ous->coloring_book)
      {
         hPen = CreatePen(PS_INSIDEFRAME,
               pen_width,
               RGB(0, 0, 0));
      }
      else
      {
         hPen = CreatePen(PS_INSIDEFRAME,
               pen_width,
                  RGB(ous->edge_trans_table[1],
                  ous->edge_trans_table[2],
                  ous->edge_trans_table[3]));
      }
      stock_pen = FALSE;
      outline_is_null = FALSE;
   }
   else
   {
      hPen = (HPEN)GetStockObject(NULL_PEN);
      stock_pen = TRUE;
      outline_is_null = TRUE;
      pen_width = 0;             /* Just in case. */
   }
}

/* Destructor. */

GDIDrawingSet::~GDIDrawingSet()
{
   if (!stock_pen && hPen != NULL)
   {
      ::DeleteObject(hPen);
   }
   if (!stock_brush && hBrush != NULL)
   {
// If NOT caching brush, then cleanup object
// else if caching then linked list will clean up.
#ifndef CACHE_OUTLINE_BRUSHES
      ::DeleteObject(hBrush); // DGP
#endif
   }
}

/////////////////////////////////////////////////////////////////////////////
// COutlinePaintParams

COutlinePaintParams::COutlinePaintParams()
{
	m_pRC = NULL;
	m_dRotation = 0.0;
	m_Flags = 0;
	m_Color = COLOR_WHITE;
	m_BColor = COLOR_BLACK;
	m_pObject = NULL;
	m_fColoringBook = FALSE;
	m_pGraphic = NULL;
	m_pClip = NULL;
}

void COutlinePaintParams::FromGraphicObject(GraphicObject* pObject)
{
	FromObject(pObject);
	m_fColoringBook = (pObject->get_graphic_flags() & GRAPHIC_FLAG_coloring_book) != 0;
	m_Color = pObject->get_color();
	m_BColor = pObject->get_bcolor();
}

void COutlinePaintParams::FromObject(RectPageObject* pObject)
{
	m_pObject = pObject;
	m_Bound = pObject->GetDrawBound();
	m_UnrotatedBound = pObject->GetUnrotatedDrawBound();
	m_dRotation = pObject->get_rotation();
	m_Flags = pObject->get_flags();
}

/////////////////////////////////////////////////////////////////////////////
// OutlineUpdateState

int OUS_NEAR OutlineUpdateState::compute_scaled_width(int in_width)
{
   int nWidth = 0x7fff;

/*
// Compute both scale values. Use the smaller one.
*/

   if (source_extent.y != 0)
   {
      nWidth = MulDiv(in_width, dest_extent.y, source_extent.y);
   }
   if (source_extent.x != 0)
   {
      int xwidth = MulDiv(in_width, dest_extent.x, source_extent.x);
      if (xwidth < nWidth)
      {
         nWidth = xwidth;
      }
   }

   if (nWidth == 0)
   {
      nWidth = 1;
   }
   return nWidth;
}

int OutlineUpdateState::ScaleX(int n)
{
	n = MulDiv(n, dest_extent.x, source_extent.x);
	if (n == 0)
	{
		n++;
	}
	return n;
}

int OutlineUpdateState::ScaleY(int n)
{
	n = MulDiv(n, dest_extent.y, source_extent.y);
	if (n == 0)
	{
		n++;
	}
	return n;
}

OUTLINE_COLOR OutlineUpdateState::RGBToOutlineColor(COLORREF c)
{
	OUTLINE_COLOR Color;
	if (c & 0x01000000)
	{
		BYTE bIndex = (BYTE)(c & 0xFF);
		// Palette index.
		if (m_pCurrentPalette == NULL || bIndex > m_pCurrentPalette->palNumEntries)
		{
			// Invalid palette or index.
			ASSERT(FALSE);
			Color.red = Color.green = Color.blue = 0;
		}
		else
		{
			// Extract the palette entry.
			Color.red = m_pCurrentPalette->palPalEntry[bIndex].peRed;
			Color.green = m_pCurrentPalette->palPalEntry[bIndex].peGreen;
			Color.blue = m_pCurrentPalette->palPalEntry[bIndex].peBlue;
		}
	}
	else
	{
		// Move the RGB values over.
		Color.red = GetRValue(c);
		Color.green = GetGValue(c);
		Color.blue = GetBValue(c);
	}
	return Color;
}

/*
// Set a current outline color.
*/

VOID OUS_NEAR OutlineUpdateState::set_outline_color(OUTLINE_COLOR far *color, OUTLINE_COLOR new_color, LPBYTE trans_table)
{
   if (color->red != new_color.red
            || color->blue != new_color.blue
            || color->green != new_color.green)
   {
   /* Set the values. */

      color->red = new_color.red;
      color->green = new_color.green;
      color->blue = new_color.blue;

   /* Remap if necessary. */

      if (do_remapping)
      {
         new_color.red = red_min + scale_number(new_color.red, red_delta, 255);
         new_color.green = green_min + scale_number(new_color.green, green_delta, 255);
         new_color.blue = blue_min + scale_number(new_color.blue, blue_delta, 255);
      }

   /* Adjust for gamma correction. */

      trans_table[0] = 0;
#if 1
      trans_table[1] = pc.rc->outline_gamma_curve[new_color.red];
      trans_table[2] = pc.rc->outline_gamma_curve[new_color.green];
      trans_table[3] = pc.rc->outline_gamma_curve[new_color.blue];
#else
      trans_table[1] = new_color.red;
      trans_table[2] = new_color.green;
      trans_table[3] = new_color.blue;
#endif

   /* Translate from RGB to destination planes. */

#if 0
      if (translate != NULL)
      {
         translate(trans_table);
      }
#endif
   }
}

/*
// Set the fill color.
*/

VOID OutlineUpdateState::set_fill_color(OUTLINE_COLOR color)
{
   set_outline_color(&fill_color, color, fill_trans_table);
}

/*
// Set the edge color.
*/

VOID OutlineUpdateState::set_edge_color(OUTLINE_COLOR color)
{
   set_outline_color(&edge_color, color, edge_trans_table);
}

/*
// Set the line color.
*/

VOID OutlineUpdateState::set_line_color(OUTLINE_COLOR color)
{
   set_outline_color(&line_color, color, line_trans_table);
}

/*
// Set the marker color.
*/

VOID OutlineUpdateState::set_marker_color(OUTLINE_COLOR color)
{
   set_outline_color(&marker_color, color, marker_trans_table);
}

/*
// Set the background color.
*/

VOID OutlineUpdateState::set_background_color(OUTLINE_COLOR color)
{
   set_outline_color(&background_color, color, background_trans_table);
}

void OutlineUpdateState::SetCurrentFont(const LOGFONT& Font)
{
	m_CurrentFont = Font;
	// Transform the font as necessary.
	m_lfUnscaledHeight = m_CurrentFont.lfHeight;
	m_CurrentFont.lfHeight = ScaleY(m_CurrentFont.lfHeight);
	if (m_CurrentFont.lfWidth != 0)
	{
		m_CurrentFont.lfWidth = ScaleX(m_CurrentFont.lfWidth);
	}
	int nAngle = -RAD2DEG(dest_angle)*10;
	m_CurrentFont.lfEscapement += nAngle;
	m_CurrentFont.lfOrientation += nAngle;
}

void OutlineUpdateState::SetCurrentPalette(LOGPALETTE* pPalette)
{
	m_pCurrentPalette = pPalette;
}

VOID OUS_NEAR OutlineUpdateState::update_what_drawn(RECT r)
{
   if (r.left < what_drawn.left)
   {
      what_drawn.left = r.left;
   }

   if (r.top < what_drawn.top)
   {
      what_drawn.top = r.top;
   }

   if (r.right > what_drawn.right)
   {
      what_drawn.right = r.right;
   }

   if (r.bottom > what_drawn.bottom)
   {
      what_drawn.bottom = r.bottom;
   }
}

/*
// The asm code below was converted to C code by Roger House 96/02/25
//
// Transform a point from outline space to destination space.
*/

void OutlineUpdateState::transform_point(OUTLINE_POINT p, CFixedPoint* new_p)
{
#if 1
	new_p->x = MakeFixed(p.x);
	new_p->y = MakeFixed(p.y);
	m_Transform.Transform(&new_p->x, &new_p->y);
#else
   G_FIXED  new_x, new_y;

   POINT so = source_origin,
      se = source_extent,
      de = dest_extent;

#if 0
   if (debug_flags & 1024)
   {
      printf("(%d, %d)", p);
   }
#endif

   if (landscape_transform)
   {
      SHORT tmp;
      tmp = p.x;
      p.x = p.y;
      p.y = tmp;
   }

#if 0
   printf("p:%d, %d; so:%d, %d; se:%d, %d; de:%d, %d ->",
               p,
               source_origin,
               source_extent,
               dest_extent);
#endif

#ifdef WIN32

#ifdef _NEWPORTABLE

      int32   t;

      // Compute x whole number and fraction

      t = p.x - so.x;
      if (t < 0)
             t = 0;
      t <<= 16;
      new_x = MulDiv(t, de.x, se.x);

      // Compute y whole number and fraction

      t = p.y - so.y;
      if (t < 0)
             t = 0;
      t <<= 16;
      new_y = MulDiv(t, de.y, se.y);

#else // _NEWPORTABLE

   _asm
   {
      movsx eax, p.x
      sub   eax, so.x
      jns   tp0
      xor   eax, eax
   tp0:

      shl   eax, 16
      imul  de.x
      idiv  se.x
   ; Compute x whole number and fraction
      mov   dword ptr new_x, eax

      movsx eax, p.y
      sub   eax, so.y
      jns   tp1
      xor   eax, eax

   tp1:

      shl   eax, 16
      imul  de.y
      idiv  se.y
   ; Compute y whole number and fraction
      mov   dword ptr new_y, eax
   }


#endif   // _NEWPORTABLE

#else    // WIN32

   _asm
   {
      mov   ax, p.x
      sub   ax, so.x
      jns   tp0
      xor   ax, ax

   tp0:

      imul  de.x
      idiv  se.x
   ; Compute x whole number
;;    add   ax, dest_offset.x
      mov   word ptr new_x+2, ax
   ; Compute x fraction
      xor   ax, ax
      div   se.x
      mov   word ptr new_x+0, ax

      mov   ax, p.y
      sub   ax, so.y
      jns   tp1
      xor   ax, ax
   tp1:

      imul  de.y
      idiv  se.y
   ; Compute y whole number
;;    add   ax, dest_offset.y
      mov   word ptr new_y+2, ax
   ; Compute y fraction
      xor   ax, ax
      div   se.y
      mov   word ptr new_y+0, ax

   }

#endif   // else WIN32

   if (reverse_x_transform)
   {
      new_x = (((PCOORD)dest_extent.x) << 16) - new_x;
   }

   if (reverse_y_transform)
   {
      new_y = (((PCOORD)dest_extent.y) << 16) - new_y;
   }

   G_FIXED_integer(new_x) += dest_x_offset;
   G_FIXED_integer(new_y) += dest_y_offset;

   new_p->x = new_x;
   new_p->y = new_y;

// printf("new_p:%d, %d\n", *new_p);
#endif
} // end OutlineUpdateState::transform_point

#define  OUTLINE_ROP    0x00B8074A

BOOL do_we_clear = TRUE;

/*
// Compute a center point from 3 points on the circle.
// The 3 points are assumed to not be collinear.
//
// The equation generally is (for computation of the center y):
//
//    B = y0*( x2 - x1 ) + y1*( x0 - x2 ) + y2*( x1 - x0)
//
//           ( x2 - x1 )*(x0ý + y0ý - (x2ý + y2ý)) + ( x0 - x2 )*(x1ý + y1ý - (x2ý + y2ý))
//    yc =  -------------------------------------------------------------------------------
//                                                 2B
//
//
// Note that there is no dependency in this equation on which point is
// considered p0 or p1 or p2. There is a definite necessity for the bottom
// of this fraction to not be 0. As long as the three points are not collinear,
// this will not happen.
*/

VOID OutlineUpdateState::center_from_3pt(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT *yc)
{
   double x0y0, x1y1, x2y2, numerator, denominator;

/* Compute the denominator. */

   denominator = 2.0* ((double)y0*(double)(x2-x1) + 
                       (double)y1*(double)(x0-x2) + 
                        (double)y2*(double)(x1-x0));

/* Compute the numerator. */

   x0y0 = (double)x0 * (double)x0 + (double)y0 * (double)y0;
   x1y1 = (double)x1 * (double)x1 + (double)y1 * (double)y1;
   x2y2 = (double)x2 * (double)x2 + (double)y2 * (double)y2;

   numerator = ((double)(x2 - x1))*(x0y0 - x2y2) + ((double)(x0 - x2))*(x1y1 - x2y2);

   *yc = (SHORT)(numerator/denominator);
}

/*
// Compute the distance between two points.
*/

double OutlineUpdateState::distance(OUTLINE_POINT a, OUTLINE_POINT b)
{
   a.x -= b.x;
   a.y -= b.y;

   return sqrt((double)a.x*(double)a.x + (double)a.y*(double)a.y);
}

/*
// Compute an angle from a vector.
*/

double OutlineUpdateState::angle_from_vector(SHORT dx, SHORT dy)
{
   return atan2((double)dy, (double)dx);
}

// Compare caller's brush color to item's color in linked list-DGP.
#ifdef CACHE_OUTLINE_BRUSHES
static int BrushCompare(const void far *pItem, const void far *pData)
{
   CBrushCompareStruct* pBCS = (CBrushCompareStruct*)pData;
   CBrushDoubleLinkedListItem* pBrushItem = (CBrushDoubleLinkedListItem*)pItem;
   if (pBrushItem->GetColor() == pBCS->m_Color && pBrushItem->GetPalette() == pBCS->m_fPalette)
      return 1; // Match successful!
   else
      return 0; // Match failed!
}
#endif

HBRUSH OutlineUpdateState::create_solid_brush(BYTE red, BYTE green, BYTE blue)
{
   COLORREF    BrushColor = RGB(red, green, blue);
   BOOL        fPalette = (pc.raster_info.type == RASTER_TYPE_PALETTE);

#ifdef CACHE_OUTLINE_BRUSHES
   CBrushDoubleLinkedListItem  *pBrushItem;

   CBrushCompareStruct bcs;
   bcs.m_Color = BrushColor;
   bcs.m_fPalette = fPalette;

   // Try to find an existing brush with matching RGB color
   pBrushItem = (CBrushDoubleLinkedListItem *) m_BrushLinkedList.FirstThat(BrushCompare, (void *)&bcs);
   // Not NULL if we found one
   if (pBrushItem != NULL)
   {
      m_BrushLinkedList.MoveToFront(pBrushItem);  // Make if first
      return pBrushItem->GetBrush();
   }
#endif

   HBRUSH      hBrush;
   // Didn't find a brush so create one
   if (fPalette)
   {
      // Stuff the data into the DIB pattern.
      Create8x8Dither(red, green, blue, dib_info->bmiData);

      // Create a DIB pattern brush so we get good dithering.
      hBrush = CreateDIBPatternBrush(dib_info_mem, DIB_PAL_COLORS);
   }
   else
   {
      hBrush = CreateSolidBrush(BrushColor);
   }

#ifdef CACHE_OUTLINE_BRUSHES
   if (hBrush != NULL)
   {
      // Allocate new item for linked list
      pBrushItem = new CBrushDoubleLinkedListItem;
      pBrushItem->SetBrush(hBrush);
      pBrushItem->SetColor(RGB(red, green, blue));
      pBrushItem->SetPalette(fPalette);
   
      // Add item to list (first position) with new brush
      m_BrushLinkedList.InsertAtFront(pBrushItem);

      // Potentially remove last item to contrain list size from growing too large
      if(m_BrushLinkedList.GetCount() > 50)
         m_BrushLinkedList.RemoveLastItem();
   }
#endif
  return hBrush;
}

/*
// Draw a gdi ellipse.
*/

PRIVATE VOID OUS_NEAR
draw_gdi_ellipse(HDC hdc, RECT r, GDIDrawingSet *gds)
{
   HBRUSH hOldBrush;
   HPEN hOldPen;

#ifdef CENTER_EDGES

   if (!gds->fill_is_null)
   {
      if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
      {
         if ((hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);

            Ellipse(hdc,
                     r.left,
                     r.top,
                     r.right,
                     r.bottom);

            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldPen);
         }
         SelectObject(hdc, hOldBrush);
      }
   }

   if (!gds->outline_is_null)
   {
      if ((hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH))) != NULL)
      {
         if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
         {
            int left_width;
            int right_width;

            if (gds->pen_width < 2)
            {
               left_width =
                  right_width = 0;
            }
            else
            {
               left_width = gds->pen_width/2;
               right_width = gds->pen_width - left_width;
            }

            Ellipse(hdc,
                     r.left - left_width,
                     r.top - left_width,
                     r.right + right_width,
                     r.bottom + right_width);

            SelectObject(hdc, hOldPen);
         }
         SelectObject(hdc, hOldBrush);
      }
   }
#else

   if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
   {
      if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
      {
         int oldBkMode = SetBkMode(hdc, OPAQUE);

         Ellipse(hdc,
                  r.left,
                  r.top,
                  r.right,
                  r.bottom);

         SetBkMode(hdc, oldBkMode);
         SelectObject(hdc, hOldPen);
      }

      SelectObject(hdc, hOldBrush);
   }
#endif
}

/*
// Add and process an ellipse.
*/

void OutlineUpdateState::draw_ellipse(OUTLINE_POINT center, SHORT rx, SHORT ry, double angle)
{
   if (dest_angle == 0.0)
   {
   /* Calculate the ellipse. */

      OUTLINE_POINT p;
      CFixedPoint ul, lr;

      p.x = center.x - rx;
      p.y = center.y - ry;
      transform_point(p, &ul);

      p.x = center.x + rx;
      p.y = center.y + ry;
      transform_point(p, &lr);

      RECT r;

      r.left = FixedInteger(ul.x);
      r.top = FixedInteger(ul.y);
      r.right = FixedInteger(lr.x);
      r.bottom = FixedInteger(lr.y);

      if (r.left > r.right)
      {
         int t = r.right;
         r.right = r.left;
         r.left = t;
      }
      if (r.top > r.bottom)
      {
         int t = r.bottom;
         r.bottom = r.top;
         r.top = t;
      }
		CRect crCheck = r;
		crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

      if (gdi_box_valid(crCheck, edge_width))
      {
      /* Do the ellipse. */

         GDIDrawingSet ds(this);

         if (ds.valid())
         {
            /* Handle before. */

#ifndef CENTER_EDGESxx
            r.right++;
            r.bottom++;
#endif
            if (m_fDrawToDC)
            {
               pc.rc->toggle(FALSE, &r);
            }

            draw_gdi_ellipse(m_hdc, r, &ds);

         /* Handle after. */

            if (m_fDrawToDC)
            {
               pc.rc->toggle(TRUE, &r);
            }
            else
            {
               /* We also need to update the mask. */
               int oldROP2 = SetROP2(mask_hdc, R2_BLACK);
               draw_gdi_ellipse(mask_hdc, r, &ds);
               SetROP2(mask_hdc, oldROP2);
            }
         }
      }
   }
   else
   {
      COutlinePath Path;

      int nXOffset, nYOffset;
      OUTLINE_POINT left, top, bottom, right, p0, p1;

   /* Construct left and right points. */

      left.x = center.x - rx;
      right.x = center.x + rx;
      right.y = left.y = center.y;

   /* Construct top and bottom points. */

      top.y = center.y - ry;
      bottom.y = center.y + ry;
      top.x = bottom.x = center.x;

   /* Construct intermediate points. These aren't used as points. */

      nXOffset = scale_number(rx, 11, 20);
      nYOffset = scale_number(ry, 11, 20);

      p0.x = center.x - nXOffset;
      p0.y = center.y - nYOffset;
      p1.x = center.x + nXOffset;
      p1.y = center.y + nYOffset;

   /* Transform all points now. */

      CFixedPoint g_left, g_top, g_bottom, g_right, g_p0, g_p1;

		g_left.x = MakeFixed(left.x);
		g_left.y = MakeFixed(left.y);
		g_top.x = MakeFixed(top.x);
		g_top.y = MakeFixed(top.y);
		g_right.x = MakeFixed(right.x);
		g_right.y = MakeFixed(right.y);
		g_bottom.x = MakeFixed(bottom.x);
		g_bottom.y = MakeFixed(bottom.y);
		g_p0.x = MakeFixed(p0.x);
		g_p0.y = MakeFixed(p0.y);
		g_p1.x = MakeFixed(p1.x);
		g_p1.y = MakeFixed(p1.y);

   /* Now build the path. */

      Path.MoveTo(g_top.x, g_top.y);
      Path.BezierTo(g_p1.x, g_top.y, g_right.x, g_p0.y, g_right.x, g_right.y);
      Path.BezierTo(g_right.x, g_p1.y, g_p1.x, g_bottom.y, g_bottom.x, g_bottom.y);
      Path.BezierTo(g_p0.x, g_bottom.y, g_left.x, g_p1.y, g_left.x, g_left.y);
      Path.BezierTo(g_left.x, g_p0.y, g_p0.x, g_top.y, g_top.x, g_top.y);
      Path.Close();
      Path.End();

   /* Build the transformation matrix for this. */

		Path.Transform(&m_Transform);

		DrawPath(Path);
	}
}

void OutlineUpdateState::DrawPath(COutlinePath& Path)
{
	CFixedRect Bound;

	Path.QuickBoundingBox(&Bound);
	RECT r;
	r.left = FixedInteger(Bound.Left - 0x8000);
	r.top = FixedInteger(Bound.Top - 0x8000);
	r.right = FixedInteger(Bound.Right + 0x8000);
	r.bottom = FixedInteger(Bound.Bottom + 0x8000);

	CRect crCheck = r;
	crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

	if (gdi_box_valid(crCheck, edge_width))
	{
		if (m_fDrawToDC)
		{
			pc.rc->toggle(FALSE, &r);
		}

		CDC dc;

		COLORREF clFill;
		BOOL fFillExists;
		COLORREF clEdge;
		BOOL fEdgeExists;
		int nEdgeWidth;

		switch (interior_style)
		{
			case 0:              /* Hollow */
			case 4:              /* Empty */
			default:
			{
				fFillExists = FALSE;
				break;
			}
			case 1:                 /* solid */
			case 2:                 /* pattern */
			case 3:                 /* hatch */
			{
				if (coloring_book)
				{
					clFill = RGB(255, 255, 255);
				}
				else
				{
					clFill = RGB(fill_trans_table[1],
									 fill_trans_table[2],
									 fill_trans_table[3]);
				}
				fFillExists = TRUE;
				break;
			}
		}

	/* Create the pen we need to use. */

		if (edge_visibility)
		{
			nEdgeWidth = compute_scaled_width(edge_width);

			if ((edge_trans_table[1] != 0
					&& edge_trans_table[1] != 255)
					|| edge_trans_table[2] != edge_trans_table[1]
					|| edge_trans_table[3] != edge_trans_table[1])
			{
			/* Not black or white. */
				if (nEdgeWidth < 2)
				{
				/* Make it at least two so it will dither the color. */
					nEdgeWidth = 2;
				}
			}

			if (coloring_book)
			{
				clEdge = RGB(0, 0, 0);
			}
			else
			{
				clEdge = RGB(edge_trans_table[1],
								 edge_trans_table[2],
								 edge_trans_table[3]);
			}
			fEdgeExists = TRUE;
		}
		else
		{
			fEdgeExists = FALSE;
			nEdgeWidth = 0;               /* Just in case. */
		}

		dc.Attach(m_hdc);
		Path.Draw(&dc,
					 clFill,
					 fFillExists,
					 nEdgeWidth,
					 clEdge,
					 fEdgeExists,
					 WINDING,
					 pc.raster_info.type == RASTER_TYPE_PALETTE);
		dc.Detach();

	/* Handle after. */

		if (m_fDrawToDC)
		{
			pc.rc->toggle(TRUE, &r);
		}
		else
		{
			/* We also need to update the mask. */
			int oldROP2 = SetROP2(mask_hdc, R2_BLACK);
			dc.Attach(mask_hdc);
			Path.Draw(&dc,
						 clFill,
						 fFillExists,
						 nEdgeWidth,
						 clEdge,
						 fEdgeExists,
						 WINDING,
						 pc.raster_info.type == RASTER_TYPE_PALETTE);
			dc.Detach();

			SetROP2(mask_hdc, oldROP2);
		}
	}
}

/*
// Draw a gdi rectangle.
*/

PRIVATE VOID OUS_NEAR
draw_gdi_rectangle(HDC hdc, RECT r, GDIDrawingSet *gds)
{
   HBRUSH hOldBrush;
   HPEN hOldPen;
#ifdef CENTER_EDGES

   if (!gds->fill_is_null)
   {
      if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
      {
         if ((hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);

            Rectangle(hdc,
                     r.left,
                     r.top,
                     r.right,
                     r.bottom);

            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldPen);
         }
         SelectObject(hdc, hOldBrush);
      }
   }

   if (!gds->outline_is_null)
   {
      if ((hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH))) != NULL)
      {
         if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
         {
            int left_width;
            int right_width;

            if (gds->pen_width < 2)
            {
               left_width =
                  right_width = 0;
            }
            else
            {
               left_width = gds->pen_width/2;
               right_width = gds->pen_width - left_width;
            }

            Rectangle(hdc,
                     r.left - left_width,
                     r.top - left_width,
                     r.right + right_width,
                     r.bottom + right_width);

            SelectObject(hdc, hOldPen);
         }
         SelectObject(hdc, hOldBrush);
      }
   }
#else

   if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
   {
      HPEN hOldPen;

      if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
      {
         int oldBkMode = SetBkMode(hdc, OPAQUE);

         Rectangle(hdc,
                  r.left,
                  r.top,
                  r.right,
                  r.bottom);

         SetBkMode(hdc, oldBkMode);
         SelectObject(hdc, hOldPen);
      }
      SelectObject(hdc, hOldBrush);
   }
#endif
}

BOOL OUS_NEAR OutlineUpdateState::gdi_box_valid(RECT r, SHORT width)
{
/* Normalize. */

   r.left -= dest_origin.x;
   r.top -= dest_origin.y;
   r.right -= dest_origin.x;
   r.bottom -= dest_origin.y;

   width = compute_scaled_width(width);

   r.left -= width;
   r.top -= width;
   r.right += width;
   r.bottom += width;

   if (r.left < 0)
   {
      r.left = 0;
   }
   if (r.right > bitmap_width)
   {
      r.right = bitmap_width;
   }

   if (r.top < 0)
   {
      r.top = 0;
   }
   if (r.bottom > bitmap_height)
   {
      r.bottom = bitmap_height;
   }

   if (r.right <= r.left || r.bottom <= r.top)
   {
      return FALSE;
   }

   update_what_drawn(r);
   something_drawn = TRUE;

   return TRUE;
}

/*
// Add and process a rectangle.
*/

void OutlineUpdateState::draw_rectangle(OUTLINE_POINT p0, OUTLINE_POINT p1)
{
	CRect crDraw;
	crDraw.left = p0.x;
	crDraw.top = p0.y;
	crDraw.right = p1.x;
	crDraw.bottom = p1.y;

	crDraw.NormalizeRect();

   POINT Points[4];
//	GeneratePolygonPoints(crDraw, Points);
	Points[0].x = crDraw.left;
	Points[0].y = crDraw.top;
	Points[1].x = crDraw.right;
	Points[1].y = crDraw.top;
	Points[2].x = crDraw.right;
	Points[2].y = crDraw.bottom;
	Points[3].x = crDraw.left;
	Points[3].y = crDraw.bottom;

   DrawPolygon(Points, 4);
}

/*
// Process a section of elliptical arc.
*/

struct CDoublePoint
{
	double x;
	double y;
};

//
// Split a bezier curve at a particular "time" (0.0 - 1.0).
//

static void SplitCurveAt(const CDoublePoint* pSourceBezier, double dTime, CDoublePoint* pNewLeft, CDoublePoint* pNewRight)
{
	// Extract the points first. This allows us to put it back into the
	// same point memory (if required by the caller).
	CDoublePoint A = pSourceBezier[0];
	CDoublePoint B = pSourceBezier[1];
	CDoublePoint C = pSourceBezier[2];
	CDoublePoint D = pSourceBezier[3];

   pNewLeft[0] = A;
   pNewRight[3] = D;

   pNewLeft[1].x = A.x + (B.x - A.x)*dTime;
   pNewLeft[1].y = A.y + (B.y - A.y)*dTime;

   pNewRight[2].x = C.x + (D.x - C.x)*dTime;
   pNewRight[2].y = C.y + (D.y - C.y)*dTime;

   CDoublePoint MBC;

   MBC.x = B.x + (C.x - B.x)*dTime;
   MBC.y = B.y + (C.y - B.y)*dTime;

   pNewLeft[2].x = pNewLeft[1].x + (MBC.x - pNewLeft[1].x)*dTime;
   pNewLeft[2].y = pNewLeft[1].y + (MBC.y - pNewLeft[1].y)*dTime;

   pNewRight[1].x = MBC.x + (pNewRight[2].x - MBC.x)*dTime;
   pNewRight[1].y = MBC.y + (pNewRight[2].y - MBC.y)*dTime;

   pNewLeft[3].x = pNewRight[0].x = pNewLeft[2].x + (pNewRight[1].x - pNewLeft[2].x)*dTime;
   pNewLeft[3].y = pNewRight[0].y = pNewLeft[2].y + (pNewRight[1].y - pNewLeft[2].y)*dTime;
}

void OutlineUpdateState::draw_elliptical_arc(OUTLINE_POINT Center, double rx, double ry, double dStartAngle, double dEndAngle, SHORT nClosure)
{
	// Closure values:
	// -1 = no closure (simple arc)
	//  0 = chord closure (end to end)
	//  1 = pie closure (end to center to end)
	TRACE("Arc ->  c: (%d, %d); r: (%f, %f); start: %d, end: %d; closure: %d\n",
			Center.x,
			Center.y,
			rx,
			ry,
			RAD2DEG(dStartAngle),
			RAD2DEG(dEndAngle),
			nClosure);

	// Compute points for bezier generation of an ellipse.
	CDoublePoint Points[13];

	double dLeft = Center.x - rx;
	double dTop = Center.y - ry;
	double dRight = Center.x + rx;
	double dBottom = Center.y + ry;
	double dXOffset = rx*11/20;
	double dYOffset = ry*11/20;

	// Construct the points for a full "bezier ellipse".
	Points[0].x = dRight;
	Points[0].y = Center.y;
	Points[1].x = dRight;
	Points[1].y = Center.y - dYOffset;
	Points[2].x = Center.x + dXOffset;
	Points[2].y = dTop;
	Points[3].x = Center.x;
	Points[3].y = dTop;
	Points[4].x = Center.x - dXOffset;
	Points[4].y = dTop;
	Points[5].x = dLeft;
	Points[5].y = Center.y - dYOffset;
	Points[6].x = dLeft;
	Points[6].y = Center.y;
	Points[7].x = dLeft;
	Points[7].y = Center.y + dYOffset;
	Points[8].x = Center.x - dXOffset;
	Points[8].y = dBottom;
	Points[9].x = Center.x;
	Points[9].y = dBottom;
	Points[10].x = Center.x + dXOffset;
	Points[10].y = dBottom;
	Points[11].x = dRight;
	Points[11].y = Center.y + dYOffset;
	Points[12] = Points[0];

	// Keep angles between 0 and 2*PI.
	dStartAngle = normalize_angle(dStartAngle);
	dEndAngle = normalize_angle(dEndAngle);

	const double PIOver2 = PI/2.0;
	const double PI3Over2 = PI + PIOver2;

	// Loop until done.
	double dAngle = dStartAngle;
	BOOL fFirst = TRUE;
	COutlinePath Path;
	CFixedPoint FirstPoint;

	do
	{
		double dMin, dMax;
		int nPoint;

		// Figure out which quadrant we are in.
		if (dAngle < PIOver2)
		{
			dMin = 0;
			dMax = PIOver2;
			nPoint = 0;
		}
		else if (dAngle < PI)
		{
			// Quadrant 1.
			dMin = PIOver2;
			dMax = PI;
			nPoint = 3;
		}
		else if (dAngle < PI3Over2)
		{
			// Quadrant 2.
			dMin = PI;
			dMax = PI3Over2;
			nPoint = 6;
		}
		else
		{
			// Quadrant 3.
			dMin = PI3Over2;
			dMax = PI2;
			nPoint = 9;
		}

		// We have computed the quadrant. The 4 points starting at nPoint
		// define the bezier curve for this quadrant.

		// Compute where to stop within this quadrant.
		double dThisEnd = (dEndAngle >= dMin && dEndAngle <= dMax) ? dEndAngle : dMax;

		// Extract the four points into Curve.
		CDoublePoint Curve[4];
		CDoublePoint OtherCurve[4];		// The "other side" of a curve split.

		// See if we need to truncate the left side.
		if (dAngle == dMin)
		{
			memcpy(Curve, Points+nPoint, sizeof(Curve));
		}
		else
		{
			ASSERT(fFirst);
			double dTime = (dAngle - dMin)/(dMax-dMin);
			SplitCurveAt(Points+nPoint, dTime, OtherCurve, Curve);
		}

		// See if we need to truncate the right side.
		if (dThisEnd != dMax)
		{
			// We need to split the right side, too.
			double dTime = (dThisEnd - dAngle)/(dMax - dAngle);
			SplitCurveAt(Curve, dTime, Curve, OtherCurve);
		}

		// If this is the first time, we need a MoveTo.
		// Otherwise, the end of the last curve should be the first point of
		// this one (and is not placed).
		if (fFirst)
		{
			// We need a MoveTo for the first point.
			FirstPoint.x = MakeFixed(Curve[0].x);
			FirstPoint.y = MakeFixed(Curve[0].y);

			Path.MoveTo(FirstPoint.x, FirstPoint.y);
		}
		Path.BezierTo(MakeFixed(Curve[1].x), MakeFixed(Curve[1].y),
						  MakeFixed(Curve[2].x), MakeFixed(Curve[2].y),
						  MakeFixed(Curve[3].x), MakeFixed(Curve[3].y));

		// No longer first time.
		fFirst = FALSE;

		// Keep the angle between 0 and 2*PI.
		dAngle = normalize_angle(dThisEnd);
	} while (dAngle != dEndAngle);

	// We must have placed something!
	ASSERT(!fFirst);

	// See if we want any closure.
	switch (nClosure)
	{
		case -1:
		{
			// Nothing to do. Leave open.
			break;
		}
		case 1:
		{
			// Pie closure.
			Path.LineTo(MakeFixed(Center.x), MakeFixed(Center.y));
			// Fall through to...
		}
		case 0:
		{
			// Chord closure.
			Path.LineTo(FirstPoint.x, FirstPoint.y);
			Path.Close();
			break;
		}
		default:
		{
			// Illegal closure value.
			ASSERT(FALSE);
			break;
		}
	}

	Path.End();

	// Transform the curve into the destination space.
	Path.Transform(&m_Transform);

	// And draw the curve!
	DrawPath(Path);
}

/*
// Draw a marker.
*/

VOID OutlineUpdateState::draw_marker(OUTLINE_POINT center)
{
}

void OutlineUpdateState::draw_polymarker(SHORT count)
{
   while (count--)
   {
      OUTLINE_POINT center;

   /* Read the center of the marker. */

      read_next_point(&center);

      draw_marker(center);
   }
}

PRIVATE VOID OUS_NEAR
draw_gdi_polyline(HDC hdc, LPPOINT point_buffer, SHORT number_of_points, HPEN hPen)
{
   HPEN hOldPen;

   if ((hOldPen = (HPEN)SelectObject(hdc, hPen)) != NULL)
   {
      Polyline(hdc, point_buffer, number_of_points);
      SelectObject(hdc, hOldPen);
   }
}

VOID OutlineUpdateState::draw_polyline(SHORT number_of_points, BOOL fCheckVisibility /*=FALSE*/)
{
	// Originally, this code didn't check the edge visibility.
	// However, for WMFs, this is necessary. Since I don't want to break the
	// CGM code with this WMF fix, I made it optional. If, in the future, it's
	// determined that this check's also necessary for CGMs, feel free to make
	// it universal.
	if (fCheckVisibility && !edge_visibility)
	{
		return;
	}

   LPPOINT point_buffer = NULL;

	TRY
		point_buffer = new POINT[number_of_points];
	END_TRY
   if (point_buffer == NULL)
   {
   /* Ugh! Failed. */
      return;
   }

   LPPOINT cp = point_buffer;
   SHORT i = 0;
   RECT r;

   r.left =
      r.top = (SHORT)0x7FFF;
   r.right =
      r.bottom = (SHORT)0x8000;

   BOOL fRotate = (dest_angle != 0.0);

   while (i++ != number_of_points)
   {
      CFixedPoint new_point;
      OUTLINE_POINT p;

      read_next_point(&p);
      transform_point(p, &new_point);

      int it;
      it = cp->x = FixedRound(new_point.x);
      if (it < r.left) r.left = it;
      if (it > r.right) r.right = it;
      it = cp->y = FixedRound(new_point.y);
      if (it < r.top) r.top = it;
      if (it > r.bottom) r.bottom = it;
      cp++;
   }

   CRect crCheck = r;
	crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

   if (number_of_points >= 2 && gdi_box_valid(crCheck, line_width))
   {
   /* Do the polyline. */

      HPEN hPen;
      BOOL stock_pen;

      SHORT width = compute_scaled_width(line_width);

      if ((line_trans_table[1] != 0
            && line_trans_table[1] != 255)
            || line_trans_table[2] != line_trans_table[1]
            || line_trans_table[3] != line_trans_table[1])
      {
      /* Not black or white. */
         if (width < 2)
         {
         /* Make it at least two so it will dither the color. */
            width = 2;
         }
      }

      if (coloring_book)
      {
         hPen = CreatePen(PS_INSIDEFRAME,
                  width,
                  RGB(0, 0, 0));
      }
      else
      {
         hPen = CreatePen(PS_INSIDEFRAME,
                  width,
                  RGB(line_trans_table[1],
                  line_trans_table[2],
                  line_trans_table[3]));
      }
      stock_pen = FALSE;

      if (hPen != NULL)
      {
         if (m_fDrawToDC)
         {
            pc.rc->toggle(FALSE, &r);
         }

         draw_gdi_polyline(m_hdc, point_buffer, number_of_points, hPen);

         if (m_fDrawToDC)
         {
            pc.rc->toggle(TRUE, &r);
         }
         else
         {
            int oldROP2 = SetROP2(mask_hdc, R2_BLACK);
            draw_gdi_polyline(mask_hdc, point_buffer, number_of_points, hPen);
            SetROP2(mask_hdc, oldROP2);
         }

         if (!stock_pen)
         {
            DeleteObject(hPen);
         }
      }
   }


/* Free our path buffer. */

	delete [] point_buffer;
}

static void draw_gdi_polygon(HDC hdc, LPPOINT point_buffer, SHORT number_of_points, GDIDrawingSet *gds, int nFillMode)
{
#if 1
   /*
   // Some printers (HP DeskJet - boo! hiss!) don't print right if both
   // a fill and outline are done with the same Polygon call. So we break
   // them up.
   */
   HBRUSH hOldBrush;
   HPEN hOldPen;
   if (!gds->fill_is_null)
   {
      if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
      {
         if ((hOldPen = (HPEN)::SelectObject(hdc, ::GetStockObject(NULL_PEN))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);
            int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

            Polygon(hdc, point_buffer, number_of_points);

            SetPolyFillMode(hdc, nOldFillMode);
            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldPen);
         }
      }
      SelectObject(hdc, hOldBrush);
   }
   if (!gds->outline_is_null)
   {
      if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
      {
         if ((hOldBrush = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);
            int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

            Polygon(hdc, point_buffer, number_of_points);

            SetPolyFillMode(hdc, nOldFillMode);
            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldBrush);
         }
      }
      SelectObject(hdc, hOldPen);
   }
#else
   HBRUSH hOldBrush;

   if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
   {
      HPEN hOldPen;

#ifdef CENTER_EDGESx
   /* Draw the fill. */

      if (!gds->fill_is_null)
      {
         if ((hOldPen = (HPEN)::SelectObject(hdc, ::GetStockObject(NULL_PEN))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);
            int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

            Polygon(hdc, point_buffer, number_of_points);

            SetPolyFillMode(hdc, nOldFillMode);
            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldPen);
         }
      }

   /* Draw the outline. */

      if (!gds->outline_is_null)
      {
         if ((hOldPen = (HPEN)::SelectObject(hdc, gds->hPen)) != NULL)
         {
            Polyline(hdc, point_buffer, number_of_points);
            SelectObject(hdc, hOldPen);
         }
      }
#else
      if ((hOldPen = (HPEN)::SelectObject(hdc, gds->hPen)) != NULL)
      {
         int oldBkMode = ::SetBkMode(hdc, OPAQUE);
         int nOldFillMode = ::SetPolyFillMode(hdc, nFillMode);

         ::Polygon(hdc, point_buffer, number_of_points);

         ::SetPolyFillMode(hdc, nOldFillMode);
         ::SetBkMode(hdc, oldBkMode);
         ::SelectObject(hdc, hOldPen);
      }
#endif

      SelectObject(hdc, hOldBrush);
   }
#endif
}

static void draw_gdi_polypolygon(HDC hdc, LPPOINT pPoints, const int* pPolyCounts, int nPolygons, GDIDrawingSet *gds, int nFillMode)
{
#if 1
   /*
   // Some printers (HP DeskJet - boo! hiss!) don't print right if both
   // a fill and outline are done with the same Polygon call. So we break
   // them up.
   */
   HBRUSH hOldBrush;
   HPEN hOldPen;
   if (!gds->fill_is_null)
   {
      if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
      {
         if ((hOldPen = (HPEN)::SelectObject(hdc, ::GetStockObject(NULL_PEN))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);
            int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

            PolyPolygon(hdc, pPoints, pPolyCounts, nPolygons);

            SetPolyFillMode(hdc, nOldFillMode);
            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldPen);
         }
      }
      SelectObject(hdc, hOldBrush);
   }
   if (!gds->outline_is_null)
   {
      if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
      {
         if ((hOldBrush = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);
            int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

            PolyPolygon(hdc, pPoints, pPolyCounts, nPolygons);

            SetPolyFillMode(hdc, nOldFillMode);
            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldBrush);
         }
      }
      SelectObject(hdc, hOldPen);
   }
#else
	//
	// NOTE! This is unfinished!
	//
   HBRUSH hOldBrush;

   if ((hOldBrush = (HBRUSH)SelectObject(hdc, gds->hBrush)) != NULL)
   {
      HPEN hOldPen;

#ifdef CENTER_EDGESx
   /* Draw the fill. */

      if (!gds->fill_is_null)
      {
         if ((hOldPen = (HPEN)::SelectObject(hdc, ::GetStockObject(NULL_PEN))) != NULL)
         {
            int oldBkMode = SetBkMode(hdc, OPAQUE);
            int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

            PolyPolygon(hdc, pPoints, pPolyCounts, nPolygons);

            SetPolyFillMode(hdc, nOldFillMode);
            SetBkMode(hdc, oldBkMode);
            SelectObject(hdc, hOldPen);
         }
      }

   /* Draw the outline. */

      if (!gds->outline_is_null)
      {
         if ((hOldPen = (HPEN)::SelectObject(hdc, gds->hPen)) != NULL)
         {
            Polyline(hdc, point_buffer, number_of_points);
            SelectObject(hdc, hOldPen);
         }
      }
#else
      if ((hOldPen = (HPEN)SelectObject(hdc, gds->hPen)) != NULL)
      {
         int oldBkMode = SetBkMode(hdc, OPAQUE);
         int nOldFillMode = SetPolyFillMode(hdc, nFillMode);

         PolyPolygon(hdc, pPoints, pPolyCounts, nPolygons);

         SetPolyFillMode(hdc, nOldFillMode);
         SetBkMode(hdc, oldBkMode);
         SelectObject(hdc, hOldPen);
      }
#endif

      SelectObject(hdc, hOldBrush);
   }
#endif
}

// Returns a pointer to the current point buffer, 
// allocates or grows it as needed - DGP.
LPPOINT OutlineUpdateState::GetPointBuffer(int nRequestedPoints)
{
   if (m_pPointBuffer == NULL || m_nPointBufferSize < nRequestedPoints)
   {
      if (nRequestedPoints > (int)(65535L / sizeof(POINT)))
      {
         return NULL;
      }

      // Get rid of any existing point buffer.
      delete [] m_pPointBuffer;
      m_pPointBuffer = NULL;

      // Allocate a minimum of 1024 points.
      if (nRequestedPoints < 1024)
      {
         nRequestedPoints = 1024;    // Make it a min. of 1024 bytes
      }
      TRY
      {
         m_pPointBuffer = new POINT[nRequestedPoints];
      }
      END_TRY
      m_nPointBufferSize = nRequestedPoints;
   }
   return m_pPointBuffer;
}

void OutlineUpdateState::draw_polygon(SHORT number_of_points)
{
   LPPOINT point_buffer;

   if ((point_buffer = GetPointBuffer(number_of_points)) == NULL)
   {
   /* Ugh! Failed. */
      return;
   }

   LPPOINT cp = point_buffer;

   for (int i = 0; i < number_of_points; i++)
   {
      OUTLINE_POINT p;

      read_next_point(&p);
      cp->x = p.x;
      cp->y = p.y;
      cp++;
   }

   DrawPolygon(point_buffer, number_of_points);
}

// This modifies the point_buffer passed.
void OutlineUpdateState::DrawPolygon(LPPOINT point_buffer, int number_of_points)
{
   if (number_of_points < 2)
   {
      return;
   }

   SHORT i = 0;
   RECT r;

   r.left =
      r.top = (SHORT)0x7FFF;
   r.right =
      r.bottom = (SHORT)0x8000;

   LPPOINT cp = point_buffer;

   while (i++ != number_of_points)
   {
      CFixedPoint new_point;
      OUTLINE_POINT p;

      p.x = (SHORT)cp->x;
      p.y = (SHORT)cp->y;
      transform_point(p, &new_point);

      int it;
      it = cp->x = FixedRound(new_point.x);
      if (it < r.left) r.left = it;
      if (it > r.right) r.right = it;
      it = cp->y = FixedRound(new_point.y);
      if (it < r.top) r.top = it;
      if (it > r.bottom) r.bottom = it;
      cp++;
   }

   CRect crCheck = r;
	crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

   if (gdi_box_valid(crCheck, edge_width))
   {
      GDIDrawingSet ds(this);

      if (ds.valid())
      {
         if (m_fDrawToDC)
         {
            pc.rc->toggle(FALSE, &r);
         }

         draw_gdi_polygon(m_hdc, point_buffer, number_of_points, &ds, m_nFillMode);

         if (m_fDrawToDC)
         {
            pc.rc->toggle(TRUE, &r);
         }
         else
         {
            int oldROP2 = SetROP2(mask_hdc, R2_BLACK);
            draw_gdi_polygon(mask_hdc, point_buffer, number_of_points, &ds, m_nFillMode);
            SetROP2(mask_hdc, oldROP2);
         }
      }
   }
}

// This modifies the point_buffer passed.
void OutlineUpdateState::DrawPolyPolygon(LPPOINT pPoints, const int* pPolyCounts, int nPolygons)
{
   RECT r;

   r.left =
      r.top = (SHORT)0x7FFF;
   r.right =
      r.bottom = (SHORT)0x8000;

   BOOL fRotate = (dest_angle != 0.0);

   LPPOINT cp = pPoints;
	const int* pPolyCount = pPolyCounts;

	for (int nPolygon = 0; nPolygon < nPolygons; nPolygon++)
   {
		int nPoints = *pPolyCount++;
		for (int nPoint = 0; nPoint < nPoints; nPoint++)
		{
			CFixedPoint new_point;
			OUTLINE_POINT p;

			p.x = (SHORT)cp->x;
			p.y = (SHORT)cp->y;
			transform_point(p, &new_point);

			int it;
			it = cp->x = FixedRound(new_point.x);
			if (it < r.left ) r.left = it;
			if (it > r.right ) r.right = it;
			it = cp->y = FixedRound(new_point.y);
			if (it < r.top ) r.top = it;
			if (it > r.bottom ) r.bottom = it;
			cp++;
		}
   }

   CRect crCheck = r;
	crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

   if (gdi_box_valid(crCheck, edge_width))
   {
      GDIDrawingSet ds(this);

      if (ds.valid())
      {
         if (m_fDrawToDC)
         {
            pc.rc->toggle(FALSE, &r);
         }

         draw_gdi_polypolygon(m_hdc, pPoints, pPolyCounts, nPolygons, &ds, m_nFillMode);

         if (m_fDrawToDC)
         {
            pc.rc->toggle(TRUE, &r);
         }
         else
         {
            int oldROP2 = SetROP2(mask_hdc, R2_BLACK);
            draw_gdi_polypolygon(mask_hdc, pPoints, pPolyCounts, nPolygons, &ds, m_nFillMode);
            SetROP2(mask_hdc, oldROP2);
         }
      }
   }
}

void OutlineUpdateState::draw_polycurve_triples(SHORT number_of_points, SHORT closure)
{
}

void OutlineUpdateState::draw_elliptical_arc_3pt(OUTLINE_POINT ps, OUTLINE_POINT pi, OUTLINE_POINT pe, SHORT closure)
{
}

void ComputeRotatedBound(CRect crBound, ANGLE dRotation, CRect* pcrRotatedBound, CPoint cpCenter)
{
	PBOX pbox;
	pbox.x0 = crBound.left;
	pbox.y0 = crBound.top;
	pbox.x1 = crBound.right;
	pbox.y1 = crBound.bottom;

	PPNT c;
	c.x = cpCenter.x;
	c.y = cpCenter.y;

	compute_rotated_bound(pbox, dRotation, &pbox, c);

	pcrRotatedBound->left = pbox.x0;
	pcrRotatedBound->top = pbox.y0;
	pcrRotatedBound->right = pbox.x1;
	pcrRotatedBound->bottom = pbox.y1;
}

//
// Draw a bitmap. The bitmap is passed in as a DIB.
//

void OutlineUpdateState::DrawBitmap(CRect crDest, CRect crSource, LPBYTE pData, BITMAPINFO* pbi, WORD wUsage, DWORD dwROP)
{
	if (dest_angle == 0.0)
	{
		// No rotation. Do it the easy way.

		// Transform the destination rectangle.
		CFixedPoint p0, p1;
		p0.x = MakeFixed(crDest.left);
		p0.y = MakeFixed(crDest.top);
		p1.x = MakeFixed(crDest.right);
		p1.y = MakeFixed(crDest.bottom);
		m_Transform.Transform(&p0.x, &p0.y);
		m_Transform.Transform(&p1.x, &p1.y);

		crDest.left = FixedInteger(p0.x);
		crDest.top = FixedInteger(p0.y);
		crDest.right = FixedRound(p1.x);
		crDest.bottom = FixedRound(p1.y);

		CRect crCheck = crDest;
		crCheck.NormalizeRect();
		crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

		if (gdi_box_valid(crCheck, 0))
		{
			if (m_fDrawToDC)
			{
				pc.rc->toggle(FALSE, &crDest);
			}

			COLORREF clOldText = ::SetTextColor(m_hdc, m_clText);
			COLORREF clOldBk = ::SetBkColor(m_hdc, m_clBk);
			int nOldMode = ::SetStretchBltMode(m_hdc, m_nBltMode);
			LOGBRUSH lb;
			switch (interior_style)
			{
				case 0:
				case 4:
				{
					lb.lbStyle = BS_NULL;
					break;
				}
				default:
				{
					lb.lbStyle = BS_SOLID;
					break;
				}
			}
			lb.lbColor = RGB(fill_trans_table[1],
								  fill_trans_table[2],
								  fill_trans_table[3]);

			HBRUSH hBrush = CreateBrushIndirect(&lb);
			HBRUSH hOldBrush = NULL;
			if (hBrush != NULL)
			{
				hOldBrush = (HBRUSH)SelectObject(m_hdc, hBrush);
			}
			// Draw it!
			::StretchDIBits(m_hdc,
								 crDest.left, crDest.top,
								 crDest.Width(), crDest.Height(),
								 crSource.left, crSource.top,
								 crSource.Width(), crSource.Height(),
								 pData,
								 pbi,
								 wUsage,
								 dwROP);
			if (hOldBrush != NULL)
			{
				SelectObject(m_hdc, hOldBrush);
			}
			if (hBrush != NULL)
			{
				::DeleteObject(hBrush);
			}

			::SetBkColor(m_hdc, clOldBk);
			::SetTextColor(m_hdc, clOldText);
			::SetStretchBltMode(m_hdc, nOldMode);

			if (m_fDrawToDC)
			{
				pc.rc->toggle(TRUE, &crDest);
			}
			else
			{
				// Black out behind the image if a solid ROP.
//				TRACE("ROP: %08lx\n", dwROP);
//				if (HIWORD(dwROP) == HIWORD(SRCCOPY) || HIWORD(dwROP) == HIWORD(NOTSRCCOPY))
				{
					PatBlt(mask_hdc, crDest.left, crDest.top, crDest.Width(), crDest.Height(), BLACKNESS);
				}
#if 0
				else
				{
					COLORREF clOldText = ::SetTextColor(mask_hdc, m_clText);
					COLORREF clOldBk = ::SetBkColor(mask_hdc, m_clBk);
					// Clear the bits that are on.
					::StretchDIBits(mask_hdc,
										 crDest.left, crDest.top,
										 crDest.Width(), crDest.Height(),
										 crSource.left, crSource.top,
										 crSource.Width(), crSource.Height(),
										 pData,
										 pbi,
										 wUsage,
										 SRCERASE);
					::SetBkColor(mask_hdc, clOldBk);
					::SetTextColor(mask_hdc, clOldText);
				}
#endif
			}
		}
	}
	else
	{
		double dRotation = normalize_angle(dest_angle);

		POINT Points[4];
		GeneratePolygonPoints(crDest, Points);

		// Compute the unrotated bound.
		CFixedPoint up0, up1;
		up0.x = MakeFixed(crDest.left);
		up0.y = MakeFixed(crDest.top);
		up1.x = MakeFixed(crDest.right);
		up1.y = MakeFixed(crDest.bottom);

		m_UnrotatedTransform.Transform(&up0.x, &up0.y);
		m_UnrotatedTransform.Transform(&up1.x, &up1.y);

		// Handle rotations greater than PI/2 away from 0 (both directions).
		if (dRotation > PI/2 && dRotation < 3*PI/2)
		{
			CFixedPoint l = up0;
			up0 = up1;
			up1 = l;
			dRotation -= PI;
		}

		CRect crUnrotatedDest;
		crUnrotatedDest.left = FixedInteger(up0.x);
		crUnrotatedDest.top = FixedInteger(up0.y);
		crUnrotatedDest.right = FixedInteger(up1.x);
		crUnrotatedDest.bottom = FixedInteger(up1.y);

		// This is the unrotated rectangle. No rotation has been applied, not
		// even to the center. We want the unrotated rectangle centered in the
		// right place (we want to move the center of rotation to the center
		// of the bitmap, not the center of the metafile).

		// Compute the current center.
		CPoint Center;
		Center.x = (crUnrotatedDest.left + crUnrotatedDest.right)/2;
		Center.y = (crUnrotatedDest.top + crUnrotatedDest.bottom)/2;

		// Compute the rotated center.
		CFixedPoint p;
		p.x = MakeFixed(Center.x);
		p.y = MakeFixed(Center.y);
		m_RotationTransform.Transform(&p.x, &p.y);

		CPoint NewCenter;
		NewCenter.x = FixedInteger(p.x);
		NewCenter.y = FixedInteger(p.y);

		// Offset the unrotated bound by the difference.
		crUnrotatedDest.OffsetRect(NewCenter.x - Center.x, NewCenter.y - Center.y);

		// Compute the rotated bound (the bound that encompasses the unrotated
		// bound after it has been rotated). This will be the destination of the
		// final StretchBlt (putting the rotated bitmap onto the screen).

		ComputeRotatedBound(crUnrotatedDest, dest_angle, &crDest, NewCenter);
			// crDest = final StretchBlt destination;

		//
		// Compute the size of the bitmap we are going to be rotating.
		// We will use the smaller of the original bitmap bound and the transformed
		// bound, rotated.
		//

		// Compute the smaller of the two bounds.
		int nWorkWidth = crUnrotatedDest.Width();
		int nWorkHeight = crUnrotatedDest.Height();

		int nWidthSign = 1;
		int nHeightSign = 1;
		if (nWorkWidth < 0)
		{
			nWidthSign = -nWidthSign;
			nWorkWidth = -nWorkWidth;
		}
		if (nWorkHeight < 0)
		{
			nHeightSign = -nHeightSign;
			nWorkHeight = -nWorkHeight;
		}

		if (crSource.Width()*crSource.Height() < nWorkWidth*nWorkHeight)
		{
			// Source is smaller. We want to use it, scaled to the destination
			// proportions.
			int nSourceWidth = crSource.Width();
			int nSourceHeight = crSource.Height();

			// Try it first one way.

			int nNewWidth = scale_number(nSourceHeight, nWorkWidth, nWorkHeight);
			if (nNewWidth < nSourceWidth)
			{
				// Getting smaller. Go the other way.
				nWorkHeight = scale_number(nSourceWidth, nWorkHeight, nWorkWidth);
				nWorkWidth = nSourceWidth;
			}
			else
			{
				// Use this.
				nWorkWidth = nNewWidth;
				nWorkHeight = nSourceHeight;
			}
		}

		nWorkWidth *= nWidthSign;
		nWorkHeight *= nHeightSign;

		CRect crUnrotatedWork;
		crUnrotatedWork = CRect(CPoint(0, 0), CSize(nWorkWidth, nWorkHeight));

		CRect crWork;
		// Compute the rotated work bound. (Center doesn't matter since we're going
		// to normalize.)
		// This will be the size of the work bitmap.
		ComputeRotatedBound(crUnrotatedWork, dest_angle, &crWork, CPoint(0, 0));

		// Normalize the work bound (i.e. make it have a (0, 0) upper left).
		crWork.OffsetRect(-crWork.left, -crWork.top);

		// Remember these dimensions for the final blit.
		CRect crRotatedWork = crWork;

		// Enlarge the bitmap as necessary so it contains the original plus the
		// horizontally skewed data.

		double dAlpha = -tan(dRotation/2);
		int nXPad = skew_number(crUnrotatedWork.Height()/2, dAlpha);
		if (nXPad < 0) nXPad = -nXPad;
		int nMinWidth = abs(crUnrotatedWork.Width()) + 2*nXPad;
		int nMinHeight = abs(crUnrotatedWork.Height());
		if (crWork.right < nMinWidth)
		{
			crWork.right = nMinWidth;
		}
		if (crWork.bottom < nMinHeight)
		{
			crWork.bottom = nMinHeight;
		}

		// Create the work bitmap.
		// We will use a CDibDC so we can access the bits easily.

		CDibDC dc;
		CDibDC MaskDc;

		BITMAPINFOHEADER bmi;
		InitBitmapInfoHeader(&bmi, crWork.Width(), crWork.Height(), 24);
		if (dc.Create((BITMAPINFO*)&bmi))
		{
			// Compute where to blt to in the middle of this bitmap.
			CPoint cpWorkOffset;
			crUnrotatedWork.OffsetRect((crWork.Width() - crUnrotatedWork.Width())/2,
											   (crWork.Height() - crUnrotatedWork.Height())/2);

			// Blt from the source bitmap to the work bitmap.
			COLORREF clOldText = dc.SetTextColor(m_clText);
			COLORREF clOldBk = dc.SetBkColor(m_clBk);
			LOGBRUSH lb;
			switch (interior_style)
			{
				case 0:
				case 4:
				{
					lb.lbStyle = BS_NULL;
					break;
				}
				default:
				{
					lb.lbStyle = BS_SOLID;
					break;
				}
			}
			lb.lbColor = RGB(fill_trans_table[1],
								  fill_trans_table[2],
								  fill_trans_table[3]);

			CBrush Brush;
			if (Brush.CreateBrushIndirect(&lb))
			{
				CBrush* pOldBrush = dc.SelectObject(&Brush);
				if (pOldBrush != NULL)
				{
					// Draw it!
					int nOldMode = dc.SetStretchBltMode(m_nBltMode);
					::StretchDIBits(dc.GetSafeHdc(),
										 crUnrotatedWork.left, crUnrotatedWork.top, 
										 crUnrotatedWork.Width(), crUnrotatedWork.Height(),
										 crSource.left, crSource.top,
										 crSource.Width(), crSource.Height(),
										 pData,
										 pbi,
										 wUsage,
										 dwROP);
					dc.SetStretchBltMode(nOldMode);
					dc.SelectObject(pOldBrush);
				}
				Brush.DeleteObject();
			}

			dc.SetBkColor(clOldBk);
			dc.SetTextColor(clOldText);

			int nBPP = 3;
			// Rotate the bitmap.
			ROTATE_MAP Map;
			Map.data = dc.GetData();
			Map.width = crWork.right;
			Map.height = crWork.bottom;
			Map.bytes_per_row = (crWork.right*nBPP + 3) & ~3;
			Map.bytes_per_pixel = nBPP;
			Map.bound = crWork;
			::OffsetRect(&Map.bound, -crUnrotatedWork.left, -crUnrotatedWork.top);

			ROTATE_PARMS Parms;
			Parms.center.x = (Map.bound.left+Map.bound.right)/2;
			Parms.center.y = (Map.bound.top+Map.bound.bottom)/2;
			Parms.angle = dRotation;

			rotate_it(&Map, &Parms);

			//
			// Do the final blt.
			//

			CRect crCheck = crDest;
			crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

			if (gdi_box_valid(crCheck, 0))
			{
				// Compute the mask polygon.

				if (m_fDrawToDC)
				{
					pc.rc->toggle(FALSE, &crDest);
				}

				// Create a region to clip to.
				HRGN hRgn = ::CreatePolygonRgn(Points, 4, ALTERNATE);

				// Massive changes ahead. Save the DC.
				::SaveDC(m_hdc);

				// Since we did a SaveDC, we do not need to save the old values for these.
				::SetTextColor(m_hdc, m_clText);
				::SetBkColor(m_hdc, m_clBk);
				::SetStretchBltMode(m_hdc, m_nBltMode);

				if (::ExtSelectClipRgn(m_hdc, hRgn, RGN_AND) >= SIMPLEREGION)
				{
					// Draw it!
					::StretchDIBits(m_hdc,
										 crDest.left,
										 crDest.top,
										 crDest.Width(),
										 crDest.Height(),
										 (crWork.Width() - crRotatedWork.Width())/2,
										 (crWork.Height() - crRotatedWork.Height())/2,
										 crRotatedWork.Width(),
										 crRotatedWork.Height(),
										 dc.GetData(),
										 (BITMAPINFO*)&bmi,
										 DIB_RGB_COLORS,
										 dwROP);
				}
				::RestoreDC(m_hdc, -1);
				::DeleteObject(hRgn);

				if (m_fDrawToDC)
				{
					pc.rc->toggle(TRUE, &crDest);
				}
				else
				{
					// Black out behind the image if a solid ROP.
//					TRACE("ROP: %08lx\n", dwROP);
//					if (HIWORD(dwROP) == HIWORD(SRCCOPY) || HIWORD(dwROP) == HIWORD(NOTSRCCOPY))
					{
						int nOldROP2 = ::SetROP2(mask_hdc, R2_BLACK);
						::Polygon(mask_hdc, Points, 4);
						::SetROP2(mask_hdc, nOldROP2);
					}
#if 0
					else
					{
						COLORREF clOldText = ::SetTextColor(mask_hdc, m_clText);
						COLORREF clOldBk = ::SetBkColor(mask_hdc, m_clBk);
						// Clear the bits that are on.
						::StretchDIBits(mask_hdc,
											 crDest.left, crDest.top,
											 crDest.Width(), crDest.Height(),
											 crSource.left, crSource.top,
											 crSource.Width(), crSource.Height(),
											 pData,
											 pbi,
											 wUsage,
											 SRCERASE);
						::SetBkColor(mask_hdc, clOldBk);
						::SetTextColor(mask_hdc, clOldText);
					}
#endif
				}
			}
		}
	}
}

//
// Generate polygon points for a rectangle.
// This assumes you pass a POINT array big enough for four points.
//

void OutlineUpdateState::GeneratePolygonPoints(CRect cr, POINT* Points, CRect* pcrBound /*=NULL*/)
{
	Points[0].x = cr.left;
	Points[0].y = cr.top;
	Points[1].x = cr.right;
	Points[1].y = cr.top;
	Points[2].x = cr.right;
	Points[2].y = cr.bottom;
	Points[3].x = cr.left;
	Points[3].y = cr.bottom;

	CRect r;

   r.left =
      r.top = (short)0x7FFF;
   r.right =
      r.bottom = (short)0x8000;

   LPPOINT cp = Points;

	for (int i = 0; i < 4; i++)
	{
      CFixedPoint new_point;
		new_point.x = MakeFixed(cp->x);
		new_point.y = MakeFixed(cp->y);
		m_Transform.Transform(&new_point.x, &new_point.y);

      int it;
      it = cp->x = FixedRound(new_point.x);
      if (it < r.left ) r.left = it;
      if (it > r.right ) r.right = it;
      it = cp->y = FixedRound(new_point.y);
      if (it < r.top ) r.top = it;
      if (it > r.bottom ) r.bottom = it;
      cp++;
	}

	if (pcrBound != NULL)
	{
		*pcrBound = r;
	}
}

//
// Draw some text.
//

void OutlineUpdateState::DrawText(OUTLINE_POINT p, WORD wOptions, LPCSTR pText, int nStrLength, LPRECT pRectangle, short* pDX /*=NULL*/)
{
	// If we have been requested to draw a background, do it now.
	if (pRectangle != NULL && (wOptions & ETO_OPAQUE))
	{
		OUTLINE_POINT p0, p1;
		p0.x = (short)pRectangle->left;
		p0.y = (short)pRectangle->top;
		p1.x = (short)pRectangle->right;
		p1.y = (short)pRectangle->bottom;

		draw_rectangle(p0, p1);
	}

	// Note about flipping:
	// I currently do not know of any way to have ExtTextOut generate flipped
	// characters (x or y). This code does the next best thing: it reverses
	// all the positioning as needed as well as the string if x-flipped. This
	// offers some illusion of being x-flipped (especially for horizontally
	// symmetric characters).

	// Initialize a string we can play with if needed.
	CString csText(pText, nStrLength);

	// Create and select the font.
	HFONT hFont = ::CreateFontIndirect(&m_CurrentFont);
	if (hFont == NULL)
	{
		return;
	}
	HFONT hOldFont;
	hOldFont = (HFONT)::SelectObject(m_hdc, hFont);

	// If we have DX values, scale them now.
	int* pScaledDX = NULL;
	if (pDX != NULL)
	{
		pScaledDX = new int[nStrLength];

		int j = 0, d = 1;
		if (pc.do_xflip)
		{
			// Flipped. Reverse the sizes since the string will be flipped.
			j = nStrLength-1;
			d = -1;
		}
		for (int i = 0; i < nStrLength; i++)
		{
			pScaledDX[j] = ScaleX(pDX[i]);
			j += d;
		}
	}

	// If we have no width, make a new font with a scaled one.
	if (m_CurrentFont.lfWidth == 0)
	{
		// Create a font with original (unscaled) height.
		LOGFONT lf = m_CurrentFont;
		lf.lfHeight = m_lfUnscaledHeight;
		HFONT hNewFont = ::CreateFontIndirect(&lf);
		if (hNewFont != NULL)
		{
			// Select th new font to get its metrics.
			SelectObject(m_hdc, hNewFont);

			// Get the text metrics.
			TEXTMETRIC tm;
			::GetTextMetrics(m_hdc, &tm);

			SelectObject(m_hdc, hFont);		// Restore previous font.
			DeleteObject(hNewFont);				// Delete our temp font.

			// Restore scaled height.
			lf.lfHeight = m_CurrentFont.lfHeight;
			// Compute scaled width.
			lf.lfWidth = ScaleX(tm.tmAveCharWidth);

			// Make the font we really want.
			hNewFont = ::CreateFontIndirect(&lf);
			if (hNewFont != NULL)
			{
				SelectObject(m_hdc, hNewFont);
				DeleteObject(hFont);
				hFont = hNewFont;
			}
		}
	}

	TEXTMETRIC tm;
	::GetTextMetrics(m_hdc, &tm);

	// Compute the extent of the text.
	SIZE czText;
	::GetTextExtentPoint32(m_hdc, pText, nStrLength, &czText);

	// If we have character extents, recompute the width.
	if (pScaledDX != NULL)
	{
		czText.cx = 0;
		for (int i = 0; i < nStrLength; i++)
		{
			czText.cx += pScaledDX[i];
		}
	}

	// Transform the destination point.
	CFixedPoint ptext;
	ptext.x = MakeFixed(p.x);
	ptext.y = MakeFixed(p.y);
	m_UnrotatedTransform.Transform(&ptext.x, &ptext.y);

	CFixedPoint Extent;
	Extent.x = MakeFixed(czText.cx);
	Extent.y = MakeFixed(czText.cy);

	int nTextAlign = m_nTextAlign;

	if (pc.do_xflip)
	{
		if ((nTextAlign & 0x06) != TA_CENTER)
		{
			nTextAlign ^= TA_RIGHT;
		}
		csText.MakeReverse();
	}

	if (pc.do_yflip)
	{
		if ((nTextAlign & 0x18) != TA_BASELINE)
		{
			nTextAlign ^= TA_BOTTOM;
		}
	}

	// Handle any horizontal alignment.
	switch (nTextAlign & 0x06)
	{
		case TA_CENTER:
		{
			ptext.x -= Extent.x/2;
			break;
		}
		case TA_RIGHT:
		{
			ptext.x -= Extent.x;
			break;
		}
	}

	// Handle any vertical alignment.
	switch (nTextAlign & 0x018)
	{
		case TA_BOTTOM:
		{
			ptext.y -= Extent.y;
			break;
		}
		case TA_BASELINE:
		{
			ptext.y -= pc.do_yflip ? MakeFixed(tm.tmDescent) : MakeFixed(tm.tmAscent);
			break;
		}
	}

	CFixedPoint p0, p1;
	p0 = ptext;

	p1.x = p0.x + Extent.x;
	p1.y = p0.y + Extent.y;

	if (dest_angle != 0.0)
	{
		CFixedPoint pcorner[4];
		// 0 = ul, 1 = ur, 2 = lr, 3 = ll
		pcorner[0].x = pcorner[3].x = p0.x;
		pcorner[0].y = pcorner[1].y = p0.y;
		pcorner[1].x = pcorner[2].x = p1.x;
		pcorner[3].y = pcorner[2].y = p1.y;
		m_RotationTransform.Transform(&ptext.x, &ptext.y);
		m_RotationTransform.Transform(&pcorner[0].x, &pcorner[0].y);
		p0 = p1 = pcorner[0];
		for (int i = 1; i < 4; i++)
		{
			m_RotationTransform.Transform(&pcorner[i].x, &pcorner[i].y);
			if (pcorner[i].x < p0.x)
			{
				p0.x = pcorner[i].x;
			}
			if (pcorner[i].x > p1.x)
			{
				p1.x = pcorner[i].x;
			}
			if (pcorner[i].y < p0.y)
			{
				p0.y = pcorner[i].y;
			}
			if (pcorner[i].y > p1.y)
			{
				p1.y = pcorner[i].y;
			}
		}
	}
	else
	{
		// Normalize the unrotated point.
		ptext.x += MakeFixed(m_nXNorm);
		ptext.y += MakeFixed(m_nYNorm);
		p0.x += MakeFixed(m_nXNorm);
		p0.y += MakeFixed(m_nYNorm);
		p1.x += MakeFixed(m_nXNorm);
		p1.y += MakeFixed(m_nYNorm);
	}

	// Compute the rectangle enclosing these two points.

	CRect crDest;
	crDest.left = FixedInteger(p0.x);
	crDest.top = FixedInteger(p0.y);
	crDest.right = FixedRound(p1.x);
	crDest.bottom = FixedRound(p1.y);
	crDest.OffsetRect(-m_nXNorm, -m_nYNorm);

	CPoint cpText;
	cpText.x = FixedInteger(ptext.x);
	cpText.y = FixedInteger(ptext.y);

   if (gdi_box_valid(crDest, 0))
	{
		if (m_fDrawToDC)
		{
			pc.rc->toggle(FALSE, &crDest);
		}

      COLORREF oldBkColor = ::SetBkColor(m_hdc, m_clBk);
      COLORREF oldTextColor = ::SetTextColor(m_hdc, m_clText);
		int nOldMode = ::SetBkMode(m_hdc, m_nBkMode);

		::ExtTextOut(m_hdc, cpText.x, cpText.y, 0, NULL, csText, nStrLength, pScaledDX);

		::SetBkMode(m_hdc, nOldMode);
		::SetBkColor(m_hdc, oldBkColor);
      ::SetTextColor(m_hdc, oldTextColor);

		if (hOldFont != NULL)
		{
			::SelectObject(m_hdc, hOldFont);
		}

		if (m_fDrawToDC)
		{
			pc.rc->toggle(TRUE, &crDest);
		}
		else
		{
			// Draw the text into the mask as well.
			HFONT hOldFont = NULL;
			if (hFont != NULL)
			{
				hOldFont = (HFONT)::SelectObject(mask_hdc, hFont);
			}

			int nOldMode = ::SetBkMode(mask_hdc, m_nBkMode);
			COLORREF oldBkColor = ::SetBkColor(mask_hdc, RGB(0, 0, 0));
			COLORREF oldTextColor = ::SetTextColor(mask_hdc, RGB(0, 0, 0));

			::ExtTextOut(mask_hdc, cpText.x, cpText.y, 0, NULL, csText, nStrLength, pScaledDX);

			::SetBkColor(mask_hdc, oldBkColor);
			::SetTextColor(mask_hdc, oldTextColor);
			::SetBkMode(mask_hdc, nOldMode);

			if (hOldFont != NULL)
			{
				::SelectObject(mask_hdc, hOldFont);
			}
		}

		// Free the font if we created one.
		if (hFont != NULL)
		{
			::DeleteObject(hFont);
		}
	}

	delete [] pScaledDX;
}

void OutlineUpdateState::MoveTo(OUTLINE_POINT p)
{
	// Transform the destination point.
	CFixedPoint new_point;
	transform_point(p, &new_point);

	CPoint cp;
	cp.x = FixedRound(new_point.x);
	cp.y = FixedRound(new_point.y);

	// We actually do the MoveTo now.

	::MoveToEx(m_hdc, cp.x, cp.y, NULL);

	if (!m_fDrawToDC)
	{
		::MoveToEx(mask_hdc, cp.x, cp.y, NULL);
	}

	m_cpCurrentPoint = cp;
}

void OutlineUpdateState::LineTo(OUTLINE_POINT p)
{
	// Create a drawing set.
	GDIDrawingSet gds(this);

	// Transform the destination point.
	CFixedPoint new_point;
	transform_point(p, &new_point);

	CPoint cp;
	cp.x = FixedRound(new_point.x);
	cp.y = FixedRound(new_point.y);

	CRect crCheck;
	crCheck.SetRect(cp.x, cp.y, m_cpCurrentPoint.x, m_cpCurrentPoint.y);
	crCheck.NormalizeRect();
	crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

	if (gdi_box_valid(crCheck, line_width))
	{
		HPEN hOldPen = (HPEN)::SelectObject(m_hdc, gds.hPen);
		if (hOldPen != NULL)
		{
			::LineTo(m_hdc, cp.x, cp.y);
			::SelectObject(m_hdc, gds.hPen);
		}

		if (!m_fDrawToDC)
		{
			HPEN hOldPen = (HPEN)::SelectObject(mask_hdc, gds.hPen);
			if (hOldPen != NULL)
			{
				::LineTo(mask_hdc, cp.x, cp.y);
				::SelectObject(mask_hdc, gds.hPen);
			}
		}
	}

	m_cpCurrentPoint = cp;
}

void OutlineUpdateState::DoPatBlt(OUTLINE_POINT p0, OUTLINE_POINT p1, DWORD dwRop)
{
	CRect crDest;
	crDest.left = p0.x;
	crDest.top = p0.y;
	crDest.right = p1.x;
	crDest.bottom = p1.y;

   POINT Points[4];
	CRect r;
	GeneratePolygonPoints(crDest, Points, &r);

	CRect crCheck = r;
	crCheck.OffsetRect(-m_nXNorm, -m_nYNorm);

   if (gdi_box_valid(crCheck, 0))
   {
		HRGN hRgn = NULL;
		if (dest_angle != 0.0)
		{
			hRgn = ::CreatePolygonRgn(Points, 4, ALTERNATE);
		}

		GDIDrawingSet ds(this);

		if (ds.valid())
		{
			BOOL fDraw = TRUE;
			if (hRgn != NULL)
			{
				::SaveDC(m_hdc);
				if (::ExtSelectClipRgn(m_hdc, hRgn, RGN_AND) < SIMPLEREGION)
				{
					fDraw = FALSE;
				}
			}

			if (fDraw)
			{
				if (m_fDrawToDC)
				{
					pc.rc->toggle(FALSE, &r);
				}

				HBRUSH hOldBrush;
				if ((hOldBrush = (HBRUSH)::SelectObject(m_hdc, ds.hBrush)) != NULL)
				{
					::PatBlt(m_hdc, r.left, r.top, r.Width(), r.Height(), dwRop);
					::SelectObject(m_hdc, hOldBrush);
				}

				if (m_fDrawToDC)
				{
					pc.rc->toggle(TRUE, &r);
				}
				else
				{
					// Draw to the mask as well.
					if (hRgn != NULL)
					{
						::SaveDC(mask_hdc);
						::ExtSelectClipRgn(mask_hdc, hRgn, RGN_AND);
					}

					::PatBlt(mask_hdc, r.left, r.top, r.Width(), r.Height(), BLACKNESS);

					if (hRgn != NULL)
					{
						::RestoreDC(mask_hdc, -1);
					}
				}
			}

			if (hRgn != NULL)
			{
				::RestoreDC(m_hdc, -1);
				::DeleteObject(hRgn);
			}
		}
	}
}

int OutlineUpdateState::source_scale(int w)
{
   if (dest_extent.x != 0)
   {
      return MulDiv(w, source_extent.x, dest_extent.x);
   }
   else if (dest_extent.y != 0)
   {
      return MulDiv(w, source_extent.y, dest_extent.y);
   }
   else
   {
      return w;
   }
}

VOID OutlineUpdateState::set_source_extent(OUTLINE_POINT p0, OUTLINE_POINT p1)
{
   source_extent.x = p1.x - (source_origin.x = p0.x);
   source_extent.y = p1.y - (source_origin.y = p0.y);

// printf("Source extent: %u, %u\n", source_extent);

   if (landscape_transform)
   {
      int tmp;

      tmp = source_origin.x;
      source_origin.x = source_origin.y;
      source_origin.y = tmp;

      tmp = source_extent.x;
      source_extent.x = source_extent.y;
      source_extent.y = tmp;

      tmp = reverse_x_transform;
      reverse_x_transform = reverse_y_transform;
      reverse_y_transform = tmp;
   }

	// Build the transformation matrix.

	m_UnrotatedTransform.Identity();
	m_UnrotatedTransform.TranslateBy(-MakeFixed(source_origin.x), -MakeFixed(source_origin.y));
	double dXScale = (double)dest_extent.x/(double)source_extent.x;
	double dYScale = (double)dest_extent.y/(double)source_extent.y;
	m_UnrotatedTransform.ScaleBy(MakeFixed(dXScale), MakeFixed(dYScale));

	// Handle flipping.
	m_UnrotatedTransform.ScaleBy(MakeFixed(reverse_x_transform ? -1 : 1),
						MakeFixed(reverse_y_transform ? -1 : 1));
	m_UnrotatedTransform.TranslateBy(MakeFixed(reverse_x_transform ? dest_extent.x : 0),
							 MakeFixed(reverse_y_transform ? dest_extent.y : 0));

	// Handle rotation center.
	m_UnrotatedTransform.TranslateBy(MakeFixed(dest_x_offset), MakeFixed(dest_y_offset));

	// Compute min and max in destination space.
   CFixedPoint min, max;
	min.x = MakeFixed(p0.x);
	min.y = MakeFixed(p0.y);
	m_UnrotatedTransform.Transform(&min.x, &min.y);
	max.x = MakeFixed(p1.x);
	max.y = MakeFixed(p1.y);
	m_UnrotatedTransform.Transform(&max.x, &max.y);

   if (min.x > max.x)
   {
      CFixed x = min.x;
      min.x = max.x;
      max.x = x;
   }

   if (min.y > max.y)
   {
      CFixed y = min.y;
      min.y = max.y;
      max.y = y;
   }

   dest_center.x = (min.x + max.x) / 2;
   dest_center.y = (min.y + max.y) / 2;

	// Handle rotation.
	m_RotationTransform.Identity();
	m_RotationTransform.TranslateBy(-dest_center.x, -dest_center.y);
	m_RotationTransform.RotateBy(-MakeFixed(RAD2DEGC*dest_angle));
	m_RotationTransform.TranslateBy(dest_center.x + MakeFixed(m_nXNorm),
											  dest_center.y + MakeFixed(m_nYNorm));

	// Build the master transform.
	m_Transform = m_UnrotatedTransform;
	m_Transform.Concatenate(&m_RotationTransform);

#if 0

   od("min: %08lx, %08lx; max:%08lx, %08lx (%08lx, %08lx)\r\n",
               min, max,
               max.x - min.x,
               max.y - min.y);

   printf("dest_origin:%x, %x; extent: %x, %x\n",
               dest_origin, dest_extent);

   printf("bmwidth: %x, bmheight:%x\n",

#endif
}

/*
// Reset the outline drawn variables. Nothing has been drawn yet.
*/

VOID OUS_NEAR OutlineUpdateState::reset_outline_drawn(BOOL reset)
{
// if (reset)
   {

   /* Clear the mask bitmap. */

      if (mask_hdc != NULL)
      {
         if (reset)
         {
         /* Always. */
            PatBlt(mask_hdc, 0, 0, bitmap_width, bitmap_height, WHITENESS);
         }
#if 0
         else
         {
            if (something_drawn
                  && what_drawn.left < what_drawn.right
                  && what_drawn.top < what_drawn.bottom)
            {
               PatBlt(mask_hdc, what_drawn.left, what_drawn.top,
                           what_drawn.right - what_drawn.left,
                           what_drawn.bottom - what_drawn.top,
                           WHITENESS);
            }
         }
#endif
      }
   }

/* Reset the area drawn. */

   what_drawn.left =
   what_drawn.top = 32767;
   what_drawn.right =
   what_drawn.bottom = -32767;

   something_drawn = FALSE;
   all_white = all_black = TRUE;    /* Seems strange but it's true. */
}

/*
// Add mask data to another mask.
// This assumes that the mask data size is a word multiple (i.e. even).
// No accounting is taken of any odd byte left over. Since we're in Windows,
// this is completely safe for our uses.
*/

// Roger House, 96/02/23:  The asm below is not converted because it is
// inside an "#if 0" block.

#if 0
PRIVATE VOID add_to_mask(LPBYTE d, LPBYTE s, WORD mask_size)
{
   _asm
   {
      push  si
      push  di
      push  ds
      lds   si, s
      les   di, d
      mov   cx, mask_size
      shr   cx, 1
      jcxz  done
   lp:
      lodsw                   ; get a mask word
      and   es:[di], ax       ; mask it into the destination
      inc   di
      inc   di
      loop  lp
   done:
      pop   ds
      pop   di
      pop   si
   }
}
#endif


/*
// Show the outline buffer if something has been drawn.
*/

VOID OUS_NEAR OutlineUpdateState::show_outline_drawn(BOOL reset)
{
#if 0
   printf("drew %d, %d, %d, %d vs. %d, %d, %d, %d\n",
            what_drawn,
            paint_info.dest_rect);
#endif
   if (something_drawn)
   {
      if (what_drawn.right > what_drawn.left
         && what_drawn.bottom > what_drawn.top)
      {
         if (source_hdc != NULL && mask_hdc != NULL)
         {
            RECT r = what_drawn;
            HDC hdc = pc.rc->destination_hdc;
            r.left += pc.dest_rect.left;
            r.top += pc.dest_rect.top;
            r.right += pc.dest_rect.left;
            r.bottom += pc.dest_rect.top;

         /* Always just draw. */

            COLORREF oldBkColor = ::SetBkColor(hdc, RGB(255,255,255));
            COLORREF oldTextColor = ::SetTextColor(hdc, RGB(0,0,0));
            pc.rc->toggle(FALSE, &r);
            ::BitBlt(hdc,
                     r.left, r.top,
                     r.right - r.left, r.bottom - r.top,
                     mask_hdc,
                     what_drawn.left, what_drawn.top,
                     SRCAND);
            ::BitBlt(hdc,
                     r.left, r.top,
                     r.right - r.left, r.bottom - r.top,
                     source_hdc,
                     what_drawn.left, what_drawn.top,
                     SRCPAINT);
            pc.rc->toggle(TRUE, &r);
            ::SetTextColor(hdc, oldTextColor);
            ::SetBkColor(hdc, oldBkColor);
         }
      }
//    reset_outline_drawn(reset);
      reset_outline_drawn(FALSE);
   }
   else
   {
//    printf("Nothing drawn...\n");
   }
}

/*
// Setup the outline color mapping system.
*/

void OUS_NEAR OutlineUpdateState::setup_outline_color(COLOR Color, COLOR BColor)
{
   RGBCOLOR white_color, black_color;

   white_color = RGBCOLOR_FROM_COLOR(Color);
   black_color = RGBCOLOR_FROM_COLOR(BColor);

   if (white_color != RGBCOLOR_WHITE || black_color != RGBCOLOR_BLACK)
   {
      do_remapping = TRUE;

   /* We must do the remapping. */

      red_min = (SHORT)RED_COMPONENT_OF(black_color);
      green_min = (SHORT)GREEN_COMPONENT_OF(black_color);
      blue_min = (SHORT)BLUE_COMPONENT_OF(black_color);

      red_delta = (SHORT)RED_COMPONENT_OF(white_color) - red_min;
      green_delta = (SHORT)GREEN_COMPONENT_OF(white_color) - green_min;
      blue_delta = (SHORT)BLUE_COMPONENT_OF(white_color) - blue_min;
   }
   else
   {
      do_remapping = FALSE;
   }
}

/*
// Set defaults.
*/

VOID OutlineUpdateState::set_defaults(VOID)
{
   OUTLINE_COLOR color;

/* Set the extent defaults. */

   source_origin.x = 0;
   source_origin.y = 0;
   source_extent.x = 32767;
   source_extent.y = 32767;

/* Say whether the extent needs to be flipped or not (as far as we know). */

   reverse_x_transform = pc.do_xflip;
   reverse_y_transform = pc.do_yflip;     /* DIBs are upside-down. */

/* Default all colors to black. */

   color.red = color.blue = color.green = 0;

/* Fill defaults. */

   hatch_index = 1;
   interior_style = 0;           /* Hollow. */

   fill_color.red = 255;
   set_fill_color(color);

/* Edge defaults. */

   edge_visibility = 0;       /* Not visible. */
   edge_type = 1;             /* Solid. */
   edge_width = 1;               /* Default to 1. */

   edge_color.red = 255;
   set_edge_color(color);

/* Line defaults. */

   line_type = 1;                /* Solid. */
   line_width = 1;               /* Default to 1. */

   line_color.red = 255;
   set_line_color(color);

/* Marker defaults. */

   marker_type = MARKER_DOT;

   marker_color.red = 255;
   set_marker_color(color);
}

/*
// Parse an outline file.
*/

IMAGE_CODE OUS_NEAR OutlineUpdateState::parse_outline_file(BOOL do_update,
                           BOOL can_interrupt, ST_DEV_POSITION position)
{     
   ST_DEV_POSITION filesize;
   DWORD last_show_time = GetTickCount();

   file.length(&filesize);

   if (do_update)
   {
      do_update = init_image_update();
   }

/* Initialize the buffered read routines. */

#ifndef FIX_CGMS
   file.set_read_buffer(2048);
#endif
   file.seek(position, ST_DEV_SEEK_SET);

// static long count = 0;

   if (position == file_start())
   {
      set_defaults();
//    count = 0;
   }

/* Go through the whole file. */

   for (;;)
   {
      BOOL primitive;

      if (!parse_next_command(&primitive))
      {
//       od("Primitives: %ld\r\n", count);
         break;
      }

#if 0
      IMAGE_CODE code;
      if ((code = pc.check_interrupt()) != IMAGE_CODE_CONTINUE)
      {
         return code;
      }
#endif
   /* Perform some special code only if we just parsed a primitive. */

      if (primitive)
      {
//       count++;

         IMAGE_CODE code;
         if ((code = pc.check_interrupt()) != IMAGE_CODE_CONTINUE)
         {
            return code;
         }

         if (can_interrupt)         // Misnomer
         {
            if (!do_update)
            {
               DWORD now = GetTickCount();

               if (now > last_show_time + OUTLINE_UPDATE_DELAY)
               {
                  show_outline_drawn(TRUE);
                  last_show_time = GetTickCount();
               }
            }
         }
      }

      if (do_update)
      {
      /* This code seems horrible in terms of speed. */

         ST_DEV_POSITION current, total;

         file.tell(&current);
         total = filesize;

         while (current > 32000L || total > 32000L)
         {
            current >>= 1;
            total >>= 1;
         }
         notify_image_update((SHORT)current, (SHORT)total);
      }
   }

   if (do_update)
   {
      notify_image_update(1, 1);
   }

   return IMAGE_CODE_DONE;
}

/*
// Plot an outline image at a specified location in the paint_info bitmap.
*/

IMAGE_CODE OUS_NEAR OutlineUpdateState::
plot_outline(int x, int y,
               int width, int height,
               LONG file_offset,
               BOOL can_interrupt,
               ANGLE angle)
{
   dest_origin.x = x;
   dest_origin.y = y;
   dest_extent.x = (width == 0)  ? 0 : width-1;
   dest_extent.y = (height == 0) ? 0 : height-1;
   dest_angle = angle;

   if (source_hdc == NULL)
   {
		// Drawing to device.
      m_hdc = pc.rc->destination_hdc;
      m_nXNorm = pc.dest_rect.left - dest_origin.x;
      m_nYNorm = pc.dest_rect.top - dest_origin.y;
      m_fDrawToDC = TRUE;
   }
   else
   {
		// Drawing to offscreen bitmap.
      m_hdc = source_hdc;
      m_nXNorm = -dest_origin.x;
      m_nYNorm = -dest_origin.y;
      m_fDrawToDC = FALSE;
   }

// A test!
// can_interrupt = FALSE;
// A test!

   return parse_outline_file(can_interrupt, can_interrupt, file_offset);
}

/*
// Paint an outline object.
*/

UpdateStatePtr OutlineUpdateState::paint(const COutlinePaintParams& Params, UpdateStatePtr ustate)
{
   ERRORCODE error;
   IMAGE_CODE code = IMAGE_CODE_ERROR;
   FLAGS object_flags = Params.m_Flags;
	RedisplayContextPtr rc = Params.m_pRC;
	ASSERT(Params.m_pRC != NULL);

// Disable what I did.
// gdi_calls = gdi_calls && rc->is_printing && rc->is_postscript;

   mask_plane = !(object_flags & OBJECT_FLAG_no_mask);

/* Build the correct dither matrix for what we want. */

   build_dither_matrix(rc);

/* Set the coloring book setting. */

   coloring_book = rc->coloring_book == TRISTATE_ALL
               || (rc->coloring_book == TRISTATE_SELECTED
                     && Params.m_fColoringBook);

/* Setup the raster info. */

   RasterInfo raster_info = rc->info;

/* Construct our image paint context. */

   if (raster_info.type == RASTER_TYPE_8)
   {
      raster_info.bpp = 8;
      raster_info.planes = 1;
   }

/* Plug in the raster info. */

   pc.raster_info = raster_info;

// printf("paint %Fp (%d)...", object, ustate != NULL);

/* Plug in the necessary parameter info. */

   pc.bound = Params.m_Bound;
   pc.clip = Params.m_pClip;

   pc.rc = rc;

/* Remember the object image so we can operate on it (if it's not NULL). */

	if (Params.m_pObject == NULL)
	{
		object_image = NULL;
	}
	else
	{
		object_image = (CBitmapObjectImage*)Params.m_pObject->get_object_image();
		if (object_image != NULL)
		{
			if (!object_image->making_image() || !rc->using_images)
			{
				object_image = NULL;
			}
      }
   }

   BOOL making_image = (object_image != NULL);

   if (!create_DCs())
   {
      return graphic_error_state(this);
   }

   if (making_image && source_hdc == NULL)
   {
		ASSERT(Params.m_pObject != NULL);
		Params.m_pObject->free_image();
      making_image = FALSE;
		object_image = NULL;
   }

   if (raster_info.type == RASTER_TYPE_PALETTE && source_hdc != NULL)
   {
   /* Select another palette into the DC to get what's there. */
      HPALETTE hPal = SelectPalette(pc.rc->destination_hdc,
                  (HPALETTE)GetStockObject(DEFAULT_PALETTE),
                  FALSE);
      SelectPalette(pc.rc->destination_hdc, hPal, FALSE);

   /* And select it into our source hdc. */

      SelectPalette(source_hdc, hPal, FALSE);
   }

/* Make the map be COLOR if appropriate. */

	BOOL fToStart = FALSE;

   if (ustate == NULL)
   {
      OUTLINE_COLOR color;

   /* Plug in the object info. */

      pc.do_xflip = !!(object_flags & OBJECT_FLAG_xflipped);
      pc.do_yflip = !!(object_flags & OBJECT_FLAG_yflipped);
      pc.graphic_flags = 0;

   /* Are we landscaping or not? */

      landscape_transform = (object_flags & OBJECT_FLAG_landscape) != 0;

   /* Plug in our info. */

      pc.image_type = IMAGE_OUTLINE;

   /* Setup the color scaling variables. */

      setup_outline_color(Params.m_Color, Params.m_BColor);

//      file_offset = lFileStart;
		fToStart = TRUE;

      pc.set_render_method(ORDERED_DITHER);

   /* Compute the rectangle where we will be drawing. */

      if (!pc.pre_paint())
      {
         return graphic_error_state(this);
      }

//    od("dest_bound:%d, %d, %d, %d; dest: %d, %d, %d, %d\r\n",
//          pc.dest_bound,
//          pc.dest);

   /*
   // This is going to be generated to destination specifications.
   // We add mask because we're going to generate a mask plane while we're at it.
   */

   /* Set a background color. This is mostly done to get the color system. */

      background_color.red = 0;
      color.red = color.green = color.blue = 255;
      set_background_color(color);

   /* Compute the number of bytes in a source row. */

//    paint_info.bytes = (paint_info.clipped_dest_width+7)/8;
   }
   else
   {
//    printf("Continuing (%ld, %d)...\n", file_offset, strip);
   }

/* Open the file. */

   if ((error = open_outline()) == ERRORCODE_None)
   {
		ST_DEV_POSITION lFileStart = file_start();
		if (fToStart)
		{
	      file_offset = lFileStart;
		}
   /*
   // Initialize the strip we will be using.
   */

      if (ustate == NULL)
      {
      /* No prior state. Initialize the strip now. */

//       printf("new strip...");

//       if (pc.initialize_strip(pc.clipped_dest_width, TRUE, TRUE))
         {
            where.x = -pc.srce.left;
            where.y = -pc.srce.top;

            if (pc.do_yflip)
            {
               where.y = -(pc.source_height - pc.srce.bottom);
            }
            if (pc.do_xflip)
            {
               where.x = -(pc.source_width - pc.srce.right);
            }
            pc.current_strip = 0;
         }
      }

      // Plot the file.
      ANGLE rotation = Params.m_dRotation;

      // Continue doing strips now.
      if (file_offset == lFileStart)
      {
      /* A new beginning! Compute the strip! */

         pc.dest_rect = pc.dest;

//       od("Strip (%d) %d, %d, %d, %d\r\n", pc.current_strip, pc.dest_rect);
      }

      SHORT width = pc.clipped_dest_width;
      SHORT height = pc.clipped_dest_height;

      bitmap_width = width;
      bitmap_height = height;

      if (!create_bitmaps(width, height))
      {
         code = IMAGE_CODE_ERROR;
      }
      else
      {
         SHORT dest_width, dest_height;

         if (rotation == 0)
         {
            dest_width = pc.dest_width;
            dest_height = pc.dest_height;
            dest_x_offset = dest_y_offset = 0;
         }
         else
         {
         /*
         // Get the unrotated bound to compute the actual unrotated
         // dest width and height.
         */
            PBOX b = Params.m_UnrotatedBound;

            rc->pbox_to_screen(&b, TRUE);
            dest_width = (SHORT)(b.x1 - b.x0);
            dest_height = (SHORT)(b.y1 - b.y0);
            dest_x_offset = (pc.dest_width - dest_width)/2;
            dest_y_offset = (pc.dest_height - dest_height)/2;
         }

#ifdef DEBUG_OUTLINE
         printf("sh: %d; ", graphic_map.map_ypsize);
         printf("plot_outline [%d of %d] (%d, %d) (%d, %d) (%d, %d)...\n", 
                     pc.current_strip, pc.n_strips,
                     where.x, where.y,
                     pc.dest_width,
                     pc.dest_height,
                     pc.clipped_dest_width,
                     pc.dest_rect.bottom - pc.dest_rect.top);
#endif

			BOOL f = FALSE;
			if (Params.m_pObject != NULL)
			{
				f = Params.m_pObject->ApplyClipRgn(pc.rc);
			}
			if (f != -1)
			{
				code = plot_outline(-where.x, -where.y,
					dest_width, dest_height,
					file_offset, rc->IsScreen(),
					-rotation);

				if (code == IMAGE_CODE_TERMINATED)
				{
				/* We skip the draw if we got terminated by check. */
					code = IMAGE_CODE_INTERRUPTED;
				}
				else
				{
					show_outline_drawn(TRUE);

					if (code == IMAGE_CODE_DONE)
					{
						if (making_image)
						{
							// Record the DDB we have.
							object_image->record_ddb(source_bitmap,
															 mask_bitmap,
															 mask_plane);
						}
					}
				}
			}

         // Restore any changed clip region.
			if (Params.m_pObject != NULL)
			{
				Params.m_pObject->RestoreClipRgn(pc.rc, f);
			}

         /* Delete the bitmaps we just used. */

         delete_bitmaps(FALSE);

         if (code == IMAGE_CODE_INTERRUPTED)
         {
         /* Free any existing context here. */

            if (rc->terminate_interrupts)
            {
               code = IMAGE_CODE_TERMINATED;
            }
            else
            {
            /* Do our interrupted thing. */

               type = UPDATE_TYPE_Interrupted;
               allocates_memory = TRUE;

               file.tell(&file_offset);
               if (file.get_based_device() == NULL)
               {
               /* Close the file handle for now. */
                  file.flush();
               }
            }
//          TRACE("Interrupted...\n");
         }
      }
      close_outline();
   }
   else
   {
      code = IMAGE_CODE_OPEN;
   }

/*
// Get rid of the source DC.
// We don't keep this around even if interrupted.
*/

   delete_DCs();

/* Free the palette if allocated. */

   if (code != IMAGE_CODE_INTERRUPTED)
   {
      delete this;
   }

/* If we got an open error, handle it now. */

// printf("[%d]\n", code);

   switch (code)
   {
      case IMAGE_CODE_DONE:
      {
         return NULL;
      }
      case IMAGE_CODE_OPEN:
      {
			if (Params.m_pGraphic == NULL)
			{
				return graphic_error_state(NULL);
			}
         return Params.m_pGraphic->handle_open_error(NULL);
      }
      case IMAGE_CODE_TERMINATED:
      {
         return graphic_terminated_state(NULL);
      }
      case IMAGE_CODE_INTERRUPTED:
      {
         return (UpdateStatePtr)this;
      }
      case IMAGE_CODE_ERROR:
      default:
      {
         return graphic_error_state(NULL);
      }
   }
}

/*
// The standard open method for outline files.
*/

PUBLIC ERRORCODE
OutlineUpdateState::open_outline(VOID)
{
/* Make sure the file is open. */

// od("Open outline %s\r\n", (LPSTR)file.get_name());

   return file.seek(0, ST_DEV_SEEK_SET);
}

/*
// Close an outline.
*/

VOID OutlineUpdateState::close_outline(VOID)
{
}

/*
// Create the DCs we will be using.
*/

BOOL OUS_NEAR OutlineUpdateState::create_DCs(VOID)
{
   if (!pc.rc->IsScreen() || !pc.rc->m_use_ddbs)
   {
   /* Don't generate bitmaps (do calls straight to DC). */
      return TRUE;
   }

   HDC hdc = pc.rc->AttributeDC();

   delete_DCs();

   if ((source_hdc = CreateCompatibleDC(hdc)) != 0
         && (mask_hdc = CreateCompatibleDC(hdc)) != 0)
   {
      return TRUE;
   }

   delete_DCs();
   return FALSE;
}

/*
// Delete the DCs we're using.
*/

VOID OUS_NEAR OutlineUpdateState::delete_DCs(VOID)
{
/* Handle the source HDC. */

   if (source_hdc != 0)
   {
      DeleteDC(source_hdc);
      source_hdc = 0;
   }

/* Handle the mask HDC. */

   if (mask_hdc != 0)
   {
      DeleteDC(mask_hdc);
      mask_hdc = 0;
   }
}

/*
// Create the bitmaps we need for rasterization.
*/

BOOL OUS_NEAR OutlineUpdateState::create_bitmaps(SHORT width, SHORT height)
{
/*
// Create the global handle which will contain our DIB brush info.
*/
   if ((dib_info_mem = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(DIBPATTERN))) == NULL)
   {
      return FALSE;
   }

   if ((dib_info = (LPDIBPATTERN)::GlobalLock(dib_info_mem)) == NULL)
   {
      delete_bitmaps();
      return FALSE;
   }

/* Initialize it. */

   dib_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   dib_info->bmiHeader.biWidth = 8;
   dib_info->bmiHeader.biHeight = 8;
   dib_info->bmiHeader.biPlanes = 1;
   dib_info->bmiHeader.biBitCount = 8;
   dib_info->bmiHeader.biCompression = BI_RGB;
   dib_info->bmiHeader.biSizeImage = 8*8;
   dib_info->bmiHeader.biXPelsPerMeter = 0;
   dib_info->bmiHeader.biYPelsPerMeter = 0;

   switch (pc.raster_info.type)
   {
      case RASTER_TYPE_PALETTE:
      {
         SHORT far *q;
         SHORT i;

         for (i = 0, q = dib_info->bmiColors; i < 256; )
         {
            *q++ = i++;
         }

         dib_info->bmiHeader.biClrUsed =
            dib_info->bmiHeader.biClrImportant = 256;
         break;
      }
#if 0
      case RASTER_TYPE_8:
      {
         RGBQUAD far *q;
         int i;

         for (i = 0, q = (RGBQUAD far *)dib_info->bmiColors; i < 8; i++, q++)
         {
            q->rgbRed = (i & 4) ? 255 : 0;
            q->rgbGreen = (i & 2) ? 255 : 0;
            q->rgbBlue = (i & 1) ? 255 : 0;
            q->rgbReserved = 0;
         }
         dib_info->bmiHeader.biClrUsed =
            dib_info->bmiHeader.biClrImportant = 128;    // Fake!
         break;
      }
#endif
      default:
      {
         break;
      }
   }

   if (source_hdc == NULL)
   {
      return TRUE;
   }

   {
   /*
   // Create the source bitmap.
   // If using gdi calls, this is compatible with the screen.
   // Otherwise, it's a monochrome mask only.
   */

      BOOL continuing = source_bitmap != NULL;

      if (!continuing)
      {
         source_bitmap = ::CreateCompatibleBitmap(pc.rc->AttributeDC(), width, height);

         if (source_bitmap == NULL)
         {
         /* We can recover! */
            delete_DCs();
            return TRUE;
         }
      }

      if (source_bitmap != NULL)
      {
         source_old_bitmap = (HBITMAP)SelectObject(source_hdc, source_bitmap);

         if (!continuing)
         {
            ::PatBlt(source_hdc, 0, 0, width, height, BLACKNESS);
            ::GetObject(source_bitmap, sizeof(m_bm), &m_bm);
//          od("** Type: %d, w: %d, h: %d, bytes: %d, bpp: %02x bits: %lx\r\n", m_bm);
//          od("Total memory: %ld\r\n",
//                (long)m_bm.bmWidthBytes*(long)m_bm.bmPlanes*m_bm.bmHeight);
         }

      /* Create the mask bitmap. */

         if (mask_bitmap == NULL)
         {
            mask_bitmap = CreateBitmap(width, height, 1, 1, NULL);
            if (mask_bitmap == NULL)
            {
               /* We can recover! */
               delete_bitmaps(-1);     /* Get rid of source bitmap. */
               delete_DCs();           /* Get rid of the source DCs. */
               return TRUE;
            }
         }

         if (mask_bitmap != NULL)
         {
            mask_old_bitmap = (HBITMAP)SelectObject(mask_hdc, mask_bitmap);

            reset_outline_drawn(!continuing);

            return TRUE;
         }
      }
   }

   delete_bitmaps();
   return FALSE;
}

/*
// Delete the bitmaps we use.
*/

VOID OUS_NEAR OutlineUpdateState::delete_bitmaps(BOOL force)
{
   if (force != -1)
   {
      if (dib_info_mem != NULL)
      {
         if (dib_info != NULL)
         {
            ::GlobalUnlock(dib_info_mem);
         }
         ::GlobalFree(dib_info_mem);
         dib_info_mem = NULL;
      }
   }

   if (source_bitmap != 0)
   {
      if (source_hdc != NULL)
      {
         SelectObject(source_hdc, source_old_bitmap);
      }
      if (force)
      {
         DeleteObject(source_bitmap);
         source_bitmap = 0;
      }
   }

   if (mask_bitmap != 0)
   {
      if (mask_hdc != NULL)
      {
         SelectObject(mask_hdc, mask_old_bitmap);
      }
      if (force)
      {
         DeleteObject(mask_bitmap);
         mask_bitmap = 0;
      }
   }
}

OutlineUpdateState::OutlineUpdateState(GraphicPtr graphic)
{
	CommonConstruct();
	if (graphic != NULL)
	{
		graphic->prep_storage_file(&file);
	}
}

OutlineUpdateState::OutlineUpdateState(StorageDevice* pDevice)
{
	CommonConstruct();
	if (pDevice != NULL)
	{
		file.set_based_device(pDevice);
	}
}

//
// Common construction code.
//

void OutlineUpdateState::CommonConstruct(void)
{
	source_hdc = NULL;
	source_bitmap = NULL;
	mask_hdc = NULL;
	mask_bitmap = NULL;
	dib_info_mem = NULL;

   m_pPointBuffer = NULL;
   m_nPointBufferSize = 0;
   m_nFillMode = ALTERNATE;            // Default

	m_clText = RGB(0, 0, 0);
	m_clBk = RGB(255, 255, 255);
	m_nTextAlign = TA_TOP | TA_LEFT;
	m_nBkMode = OPAQUE;
	m_nBltMode = COLORONCOLOR;

	m_pCurrentPalette = NULL;
}

OutlineUpdateState::~OutlineUpdateState()
{
  // Free point buffer if one was allocated - DGP
   delete [] m_pPointBuffer;
   m_nPointBufferSize = 0;

   delete_bitmaps();
   delete_DCs();
}
