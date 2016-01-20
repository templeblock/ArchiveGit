/*
// $Workfile: PMGFONT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMGFONT.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 10    3/01/99 2:29p Cschende
// fixed font names in the font substitution list
// 
// 9     3/01/99 1:38p Cschende
// check for the font in the substitutes list before checking if it's on
// the system, it's faster.
// 
// 8     3/01/99 12:16p Cschende
// fixed declarations
// 
// 7     3/01/99 11:55a Cschende
// added functions to make permanent font substitutions in the document
// after it is read in for font which are no longer going to be
// distributed with PrintMaster
// 
// 6     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 4     4/29/98 6:31p Rlovejoy
// Updated to use fill objects.
// 
// 3     3/18/98 11:14a Jayn
// Page numbers
// 
// 2     12/18/97 3:21p Dennis
// Added capability to set typeface at a per object basis.
// 
//    Rev 1.0   14 Aug 1997 15:23:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:46   Fred
// Initial revision.
// 
//    Rev 1.50   20 Jul 1997 14:37:10   Fred
// Does not allow handle wrap when cropping; handles empty clip regions
// 
//    Rev 1.49   16 Jul 1997 09:44:54   Fred
// Fixed bug with flipped text.
// 
//    Rev 1.48   12 Jun 1997 13:19:18   Fred
// Fixed thin character problem
// 
//    Rev 1.47   11 Jun 1997 17:28:18   Fred
// Bitmap drawing optimizations
// 
//    Rev 1.46   06 Jun 1997 14:57:46   Fred
// 16-bit PostScript changes
// 
//    Rev 1.45   06 Jun 1997 14:13:08   Fred
//  
// 
//    Rev 1.44   06 Jun 1997 11:47:36   Fred
// Fixed bug with colored text
// 
//    Rev 1.43   05 Jun 1997 17:47:00   Fred
// 16-bit change
// 
//    Rev 1.42   05 Jun 1997 17:10:54   Fred
//  
// 
//    Rev 1.41   05 Jun 1997 17:10:08   Fred
// 32-bit PostScript printing
// 
//    Rev 1.40   02 Jun 1997 18:20:00   Fred
// Bug fixes
// 
//    Rev 1.39   01 Jun 1997 14:24:24   Fred
// No longer tries to rasterize spaces
// 
//    Rev 1.38   29 May 1997 08:51:16   Fred
// New fill color
// 
//    Rev 1.37   15 May 1997 16:46:42   Fred
// New text effects
// 
//    Rev 1.36   22 Apr 1997 14:27:48   Fred
// Fixed bug
// 
//    Rev 1.35   22 Apr 1997 13:17:00   Fred
// Start of project and art browsers
// 
//    Rev 1.34   18 Apr 1997 12:48:54   Fred
// Only caches screen character bitmaps
// 
//    Rev 1.33   17 Apr 1997 16:58:56   Fred
// Fixed a bug with cached warped characters
// 
//    Rev 1.32   17 Apr 1997 16:30:52   Fred
// Fixed bug with rotated characters
// 
//    Rev 1.31   17 Apr 1997 15:36:06   Fred
// Rotated fill patterns
// 
//    Rev 1.30   15 Apr 1997 10:01:04   Fred
// Fixed some bugs including mapping the character when getting delta X.
// 
//    Rev 1.29   11 Apr 1997 15:25:12   Fred
// Sets warp field number
// 
//    Rev 1.28   11 Apr 1997 13:56:52   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.27   09 Apr 1997 14:51:12   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.26   03 Apr 1997 17:22:56   Fred
// Fixed some warnings in 16-bit
// 
//    Rev 1.25   03 Apr 1997 09:30:56   Fred
// More text cleanup
// 
//    Rev 1.24   26 Mar 1997 15:56:08   Fred
// Removed old geronimo stuff
// 
//    Rev 1.23   19 Mar 1997 17:55:28   Fred
// Fixed banding text drawing bug
// 
//    Rev 1.22   12 Mar 1997 14:54:40   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.21   06 Mar 1997 13:22:08   Fred
// More text stuff
// 
//    Rev 1.20   05 Mar 1997 14:36:36   Fred
// More text object stuff
// 
//    Rev 1.19   11 Feb 1997 14:53:52   Jay
// 16-bit changes
// 
//    Rev 1.18   07 Feb 1997 08:50:52   Fred
// Start of new text drawing pipeline
// 
//    Rev 1.17   28 Jan 1997 16:56:44   Fred
// Start of component drawing
// 
//    Rev 1.16   24 Jan 1997 15:00:10   Fred
// Removed some Geronimo conventions including changing G_FRACTION to a long
// 
//    Rev 1.15   09 Jan 1997 08:58:12   Fred
// More text box changes
// 
//    Rev 1.14   08 Jan 1997 15:20:56   Jay
// Character spacing
// 
//    Rev 1.13   08 Jan 1997 15:20:10   Fred
// More text box changes
// 
//    Rev 1.12   06 Jan 1997 10:16:28   Fred
// Start of new text object
// 
//    Rev 1.11   12 Nov 1996 11:43:42   johno
// Moved strings to resource DLL
// 
//    Rev 1.10   08 Nov 1996 08:57:44   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.9   05 Nov 1996 14:06:52   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.8   15 Oct 1996 09:02:06   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.7   16 Jul 1996 11:26:48   Jay
// Got rid of some old code.
// 
//    Rev 1.6   27 Jun 1996 13:26:10   Jay
//  
// 
//    Rev 1.5   20 Jun 1996 17:39:30   Jay
// Made some shorts long.
// 
//    Rev 1.4   17 Jun 1996 12:02:46   Jay
//  
// 
//    Rev 1.3   13 Jun 1996 15:53:52   Jay
//  
// 
//    Rev 1.2   24 May 1996 09:28:06   Jay
// New text code
// 
//    Rev 1.1   25 Apr 1996 12:18:16   Jay
// Beginnigs of new text support.
// 
//    Rev 1.0   14 Mar 1996 13:32:14   Jay
// Initial revision.
// 
//    Rev 1.1   27 Feb 1996 17:03:40   VSI
// _NEWPORTABLE 'C' code replaces .asm -- not yet working
// 
//    Rev 1.1   26 Feb 1996 17:21:20   VSI
// Integrated new 'C' version of .asm code
// 
//    Rev 1.0   16 Feb 1996 12:30:10   FRED
// Initial revision.
// 
//    Rev 2.12   04 Oct 1995 10:58:14   FRED
// 32 bit TrueType engine
// 
//    Rev 2.11   13 Sep 1995 11:47:10   JAY
// New 32-bit changes
// 
//    Rev 2.10   09 Aug 1995 13:10:30   JAY
// Misc changes.
// 
//    Rev 2.9   05 Jun 1995 11:33:38   JAY
//  
// 
//    Rev 2.8   30 May 1995 13:48:12   JAY
// Fixed bug where a large off screen bitmap could cause a mask which was
// larger than 64K.
// 
//    Rev 2.7   01 May 1995 15:27:42   JAY
// Restructured the macro stuff
// 
//    Rev 2.6   27 Apr 1995 13:26:28   FRED
//  
// 
//    Rev 2.5   25 Apr 1995 09:01:42   JAY
// Created macro server to be go-between for document and font engine.
// 
//    Rev 2.4   29 Mar 1995 09:14:44   JAY
// Lots of changes
// 
//    Rev 2.3   15 Mar 1995 10:48:08   JAY
// Support for new text editing
// 
//    Rev 2.2   15 Feb 1995 17:16:08   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:38:16   JAY
// Reverted. New series.
// 
//    Rev 1.36   30 Jan 1995 09:34:08   JAY
// Cut and paste
// 
//    Rev 1.35   30 Dec 1994 14:00:38   JAY
// New dithering scheme with expanded palette (based on WinG palette).
// 
//    Rev 1.34   04 Aug 1994 08:30:44   JAY
// Large model and 32-bit changes
// 
//    Rev 1.33   20 Jun 1994 13:29:20   JAY
// reset_fonts() now calls ClearLastFace for the truetype engine.
// 
//    Rev 1.32   01 Jun 1994 16:49:52   JAY
// Some minor changes.
// 
//    Rev 1.31   14 Apr 1994 08:05:36   JAY
// is_postscript is now an 'int' which takes on multiple values depending on
// the "type" of PostScript driver the printer is. Current possible values:
// 0: Normal printer (nothing special)
// 1: Initial PostScript hard-coded fixes. Correspond to normal Windows PS driver.
// 2: QMS ColorScript 230 Level 2 hard-coded fixes.
// 
//    Rev 1.30   18 Mar 1994 13:36:28   MACDUFF
// Old PMG line spacing is now stored in FONT_DESCRIPTION, so it
// will be cached.
// 
//    Rev 1.29   17 Mar 1994 14:45:00   MACDUFF
// draw_character_offscreen() can now handle clipping, via creative
// modification of parameters to or_rows().
// 
//    Rev 1.28   16 Mar 1994 15:54:58   MACDUFF
// Line spacing defaults to Old PMG.
// 
//    Rev 1.27   16 Mar 1994 15:53:14   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.26   09 Mar 1994 17:14:14   MACDUFF
// Removed the previous change, for now, because it breaks existing
// documnets by changing line spacing.  We will restore this fix, or
// something like it, after we separate line spacing calculation from
// the font bbox.
// 
//    Rev 1.25   09 Mar 1994 16:12:16   MACDUFF
// Round font bbox outward.  This fixes Journal 12 pt. problem, and probably
// other problems that have been fixed in the past by "fudging."
// 
//    Rev 1.24   09 Mar 1994 11:29:30   JAY
// Now uses DIB pattern brushes for non-black colors on palette screen devices.
// 
//    Rev 1.23   09 Mar 1994 09:00:30   JAY
// Now caches characters even when flipped.
// 
//    Rev 1.22   02 Mar 1994 13:41:24   JAY
// Reduced the bitmap size limit to 48K to prevent overflow in the clear_out
// mask generation code.
// 
//    Rev 1.21   02 Mar 1994 11:22:00   JAY
// Integrated the support for "is_postscript == 2" into the normal code.
// "is_postscript == 2" is no longer valid and the boolean nature of the
// variable is restored.
// 
//    Rev 1.20   01 Mar 1994 15:50:02   JAY
// Support for HPGL driver (is_postscript == 2) and modified what means
// postscript driver (now is_postscript == 1).
// 
//    Rev 1.19   25 Feb 1994 14:39:00   JAY
// Added and then commented out some code for creating DIB pattern brushes.
// 
//    Rev 1.18   22 Feb 1994 15:04:24   JAY
// New code for printing correctly to PostScript printers.
// 
//    Rev 1.17   17 Feb 1994 08:44:32   MACDUFF
// Support for scaling access methods in redisplay context.
// 
//    Rev 1.16   16 Feb 1994 14:14:48   MACDUFF
// Does not cache character if forced to band.
// 
//    Rev 1.15   10 Feb 1994 18:04:44   MACDUFF
// Organized the offscreen drawing in a more sane way, thereby fixing
// Fred's "y" bug, where masking did get done properly on characters
// that clipped.
// 
//    Rev 1.14   09 Feb 1994 15:22:56   MACDUFF
// Draw strings to offscreen bitmap, then blit whole string to
// destination DC at once.
// 
//    Rev 1.13   09 Feb 1994 09:01:56   MACDUFF
// Speed improvements: (1) Move TextColor and CreateBrush calls from
// character to string level. (2) In draw_string(), don't create a copy
// of the TextStyle object, because the constructor and destructor
// have a lot of overhead.
// 
//    Rev 1.12   07 Feb 1994 16:35:48   MACDUFF
// fixed typo
// 
//    Rev 1.11   07 Feb 1994 16:26:16   MACDUFF
// Changed maximum bits in font size computation to 31.
// 
//    Rev 1.10   02 Feb 1994 14:23:58   JAY
// Another speed improvement for black text.
// 
//    Rev 1.9   02 Feb 1994 13:44:46   JAY
// Added a sanity (zero) check for the mask routine.
// 
//    Rev 1.8   02 Feb 1994 13:21:24   JAY
// Improved draw_character_mask() for additional speed.
// 
//    Rev 1.7   02 Feb 1994 11:42:06   JAY
// New cache and fonts array flush routine.
// Global DCs for speeding up (ha!) text drawing.
// 
//    Rev 1.6   01 Feb 1994 17:45:58   JAY
// Now creates one DC per string call.
// Now assumes characters in a document are in ANSI (not MicroLogic numbering).
// 
//    Rev 1.5   28 Jan 1994 17:12:42   JAY
//  
// 
//    Rev 1.4   19 Jan 1994 17:09:02   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:03:14   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:10:48   JAY
// Restructuring, cut/paste, images, etc.
// 
//    Rev 1.1   30 Dec 1993 16:12:08   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:28:10   JAY
//  
*/

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "system.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmgdb.h"

#include "fontrec.h"
#include "stylerec.h"
#include "pmgfont.h"
#include "utils.h"
#include "fcache.h"
#include "outpath.h"
#include "warpfld.h"
#include "frameobj.h"
#include "framerec.h"
#include "tfengine.h"
#include "util.h"
#include "bmp.h"

#define PALDIB_TEXT

#ifdef PALDIB_TEXT
#include "dither.h"

typedef struct
{
   BITMAPINFOHEADER  bmiHeader;
   SHORT             bmiColors[256];         /* 256 palette entries */
   BYTE              bmiData[8*8];           /* bitmap data */
} DIBPATTERN, far *LPDIBPATTERN;

PRIVATE DIBPATTERN /*near*/ dib_pattern =
{
   {
      sizeof(BITMAPINFOHEADER),
      8L,
      8L,
      1,
      8,
      BI_RGB,
      8*8,
      0L,
      0L,
      256L,
      256L
   },
   {
      0, 0
   }
};
#endif

#define NOTSRCAND 0x00220326L

//#define GDI_TIMING_TEST
#ifdef GDI_TIMING_TEST
static HDC my_create_compatible_dc(HDC hdc)
{
   return ::CreateCompatibleDC(hdc);
}
#define CreateCompatibleDC my_create_compatible_dc

static HBITMAP my_create_bitmap(int a, int b, UINT c, UINT d, const void far *e)
{
   return ::CreateBitmap(a, b, c, d, e);
}
#define CreateBitmap my_create_bitmap

static BOOL my_bitblt(HDC a, int b, int c, int d, int e, HDC f, int g, int h, DWORD i)
{
   return ::BitBlt(a, b, c, d, e, f, g, h, i);
}
#define BitBlt my_bitblt

static BOOL my_patblt(HDC a, int b, int c, int d, int e, DWORD f)
{
   return ::PatBlt(a, b, c, d, e, f);
}
#define PatBlt my_patblt

static HGDIOBJ my_SelectObject(HDC a, HGDIOBJ b)
{
   return ::SelectObject(a, b);
}
#define SelectObject my_SelectObject

static BOOL my_DeleteObject(HGDIOBJ a)
{
   return ::DeleteObject(a);
}
#define DeleteObject my_DeleteObject

#define PatBlt my_patblt

#endif   // GDI_TIMING_TEST
      
/*
// The font cache.
*/

PUBLIC CFontCache* pFontCache = NULL;

/*
// Some DCs for general use.
*/

static HDC source_hdc = NULL;
static HDC temp_hdc = NULL;

static COLORREF text_color;

/*******************************/
/* PMGFontServer class support */
/*******************************/

#define NUMBER_OF_SUBSTITUTIONS 8 // number of font names in the array below
// These fonts were at one time illegally distributed and are no longer distributed.
char * OriginalFont[] = {	"Paris",	"Tiempo",	"Gourmand",	"Optimum",	"Ensign", 
								"OldText",	"Industry",	"Wedding",	"Poster",	"Fiorne"	};

// These are the replacements for the above fonts. 
// To be used if the above for is not found must be the same size as OriginalFont
char * SubstituteFont[] =	{	"Exotica",	"Imperial",	"Gallery",	"Exotica",	"Advantage",
									"Victorian","Downtown",	"Pageant",	"Metro",	"Fairy Scroll"	};

/*
// Characters in the range 130 (0x82) - 159 (0x9f)
*/

PRIVATE uint16 near
ansi_to_ugl[] =
{
   0x201a,           // 130
   0x0192,           // 131
   0x201e,           // 132
   0x2026,           // 133
   0x2020,           // 134
   0x2021,           // 135
   0x02c6,           // 136
   0x2030,           // 137
   0x0160,           // 138
   0x2039,           // 139
   0x0152,           // 140
   141,              // 141
   142,              // 142
   143,              // 143
   144,              // 144
   0x2018,           // 145
   0x2019,           // 146
   0x201c,           // 147
   0x201d,           // 148
   0x2022,           // 149
   0x2013,           // 150
   0x2014,           // 151
   0x02dc,           // 152
   0x2122,           // 153
   0x0161,           // 154
   0x203a,           // 155
   0x0153,           // 156
   157,              // 157
   158,              // 158
   0x0178            // 159
};

// character_map()
//
//    Map a PM character index to a UGL code for TrueType.

uint16 character_map(CHARACTER ch, FaceEntry* face)
{
   if (face->GetIsTextFont())
   {
      return (ch >= 130 && ch <= 159) ? ansi_to_ugl[ch-130] : (uint16)ch;
   }
   else
   {
      // Symbol font -- use high mapping
      return (ch >= 0x8000) ? 0 : 0xf000 + ch;
   }
}

// Check if the font parameters will cause an overflow in the TrueType engine.
// If so, scale back the resolution until this is no longer true. The function,
// RestoreFontSize(), should be called to properly scale up the results. Note
// that this technique can only be used for metric information, not for bitmaps.
void AdjustFontSize(G_FONT_DESCRIPTION& Font, int& nXAdjust, int& nYAdjust)
{
	long lPointSize = HIWORD(Font.point_size+0xFFFFL);
	long lXSize = ((long)HIWORD(Font.horizontal_expansion+0xFFFFL))*lPointSize*Font.x_resolution;
	long lYSize = ((long)HIWORD(Font.vertical_expansion+0xFFFFL))*lPointSize*Font.y_resolution;

	nXAdjust = 0;
	while (lXSize >= 72*32768L)
	{
		lXSize >>= 1;
		Font.x_resolution >>= 1;
		nXAdjust++;
	}

	nYAdjust = 0;
	while (lYSize >= 72*32768L)
	{
		lYSize >>= 1;
		Font.y_resolution >>= 1;
		nYAdjust++;
	}
}

// Scale the information in the font description to account for any scaling
// performed by AdjustFontSize().
void RestoreFontSize(G_FONT_DESCRIPTION& Font, int& nXAdjust, int& nYAdjust)
{
	if (nXAdjust != 0)
	{
		Font.font_space_width <<= nXAdjust;
		Font.font_bbox.ulx <<= nXAdjust;
		Font.font_bbox.lrx <<= nXAdjust;
		Font.character_bbox.ulx <<= nXAdjust;
		Font.character_bbox.lrx <<= nXAdjust;
		Font.character_delta_x <<= nXAdjust;
		Font.x_resolution <<= nXAdjust;
	}
	if (nYAdjust != 0)
	{
		Font.font_underline_position <<= nYAdjust;
		Font.font_underline_thickness <<= nYAdjust;
		Font.font_bbox.uly <<= nYAdjust;
		Font.font_bbox.lry <<= nYAdjust;
		Font.character_bbox.uly <<= nYAdjust;
		Font.character_bbox.lry <<= nYAdjust;
		Font.y_resolution <<= nYAdjust;
	}
}

/*
// Static configuration variables.
*/

CString PMGFontServer::default_face;
SHORT PMGFontServer::default_point_size = 36;

PMGFontServer::PMGFontServer() :
  database(NULL),
  m_dib_handle(NULL),
  m_dib_memory(NULL),
  m_dib_brush(NULL)
{
   m_nDefaultPointSize = 0;
   m_lDefaultStyle = 0;
   m_nLastFontIndex = -1;
   m_csDefaultFace = PMGFontServer::default_face;
}

/*
// The destructor.
*/

PMGFontServer::~PMGFontServer()
{
   if (m_dib_handle != NULL)
   {
      if (m_dib_brush != NULL)
      {
         ::DeleteObject(m_dib_brush);
      }
      if (m_dib_memory != NULL)
      {
         ::GlobalUnlock(m_dib_handle);
      }
      ::GlobalFree(m_dib_handle);
   }
}

/*
// Translate a face number into a font record.
// The record will have an additional temp reference count reflecting this
// usage (so you need to call 'free_font_record' when done).
*/

DB_RECORD_NUMBER PMGFontServer::font_face_to_record(SHORT face)
{
   if (database == NULL || face < 0)
   {
      return 0;
   }

/* Get the face entry so we can access its name. */

   FaceEntry* entry;

   entry = typeface_server.face_list.get_face(face);

/* Get a temp font reference. */

   return database->new_font_record((LPSTR)entry->get_name(), TRUE);
}

/*
// Translate a font record to a face.
*/

SHORT PMGFontServer::font_record_to_face(DB_RECORD_NUMBER record)
{
   if (database == NULL || record <= 0)
   {
      return -1;
   }

/*
// Read the font record to see if it has been installed yet.
*/

   FontDataPtr fontdata;

   if ((fontdata = (FontDataPtr)database->get_record(record, NULL, RECORD_TYPE_FontData)) == NULL)
   {
      return -1;
   }

   SHORT index;

   if ((index = fontdata->get_face_index()) == -1)
   {
   /* We must look up the face in the list. */

		// Note: there is a default second parameter which is TRUE and
		// instructs the typeface server to return the "default" face if
		// the requested face is not font. Thus, usually this function
		// will *not* return -1, even if the desired face is not on the
		// current computer.

      index = typeface_server.find_face(fontdata->get_face_name());

   /* Set the index we decided on. */

      fontdata->set_face_index(index);
      fontdata->modified();
   }

/* Release the font data now that we are done. */

   fontdata->release();

   return index;
}

#define EAE_RESOLUTION 72

//
// Support for offscreen bitmap drawing
//

MAP near mapOffscreen;
POINT near ptOffscreenMapOrigin;

// The asm code below was converted to C code by Roger House 96/02/24
//
// or_rows()
//
// OR some rows of bitmap data into a destination map, with the data 
// shifted 0-7 bits to the right.
//
// lpbDest:     ptr to 1st byte to modify in destination bitmap
// lpbSrc:      ptr to 1st byte to use in source map
// nShift:      number of bits to shift the source data to the right
// nRows:       number of rows of data to OR
// nBytesPerRow:    number of bytes to OR from each source row
// nDestRowOffset:  number of bytes to add to lpbDest to get next row
// nSrcRowOffset:   number of bytes to add to lpbSrc to get next row
// fLeftClip:      TRUE => don't OR upper bits of first byte of each 
//                      source row
// fRightClip:     TRUE => don't OR lower bits of last byte from each 
//                      source row
//
// NOTES: if nShift == 0, then fLeftClip and fRightClip should be FALSE.
// if nShift != 0, then the number of bytes modified in each destination
// row will be nBytesPerRow + 1.
//

static void or_rows(
   LPBYTE lpbDest,
   LPBYTE lpbSrc,
   int nShift,
   int nRows,
   int nBytesPerRow,
   int nDestRowOffset,
   int nSrcRowOffset,
   BOOL fLeftClip,
   BOOL fRightClip)
{  
   nDestRowOffset -= nBytesPerRow;
   nSrcRowOffset  -= nBytesPerRow;

   // Clip Flags:
   //
   //    0x02 = skip top of first byte, 
   //    0x01 = skip bottom of last byte

   BYTE clip_flags;

   if (nShift == 0)
   {
      clip_flags = 0x01;
   }
   else
   {
      clip_flags = 0x00;
      if (fLeftClip)
      {
         ++nDestRowOffset;
         clip_flags |= 0x02;
      }
      if (fRightClip)
      {
         clip_flags |= 0x01;
      }
   }

#ifdef _NEWPORTABLE
   // C version
   int   numBytes;
   USHORT   shiftWord;
   BYTE  b, save;

   ASSERT(nBytesPerRow != 0);

   // Row loop

   do
   {
      numBytes = nBytesPerRow;

      shiftWord = 0;

      if (clip_flags & 2)
      {
         // Clip left
         b = *lpbSrc++;
         shiftWord = MAKEWORD(0, b);
         shiftWord >>= nShift;
              --numBytes;
      }

      // Byte loop

      while (numBytes)
      {
         save = LOBYTE(shiftWord);
         b = *lpbSrc++;
         shiftWord = MAKEWORD(0, b);
         shiftWord >>= nShift;
         *lpbDest++ |= (HIBYTE(shiftWord) | save);
         --numBytes;
      }

      if ((clip_flags & 1) == 0)
         *lpbDest |= LOBYTE(shiftWord);

      // Done with row, go to next

      lpbSrc  += nSrcRowOffset;
      lpbDest += nDestRowOffset;

   } while (--nRows);

   return;

#else

#ifdef _WIN32
   // 32-bit version
   _asm
   {
      mov   edi, lpbDest
      mov   esi, lpbSrc
      
      mov   cx, word ptr nShift
      mov   ch, clip_flags

   row_loop:
      mov   edx, nBytesPerRow

      xor   al, al

      test  clip_flags, 2
      jz byte_loop

   // Clip left
      mov   ah, [esi]
      inc   esi
      shr   ax, cl
      dec   dx
      jz last_byte

   byte_loop:
      // AL = previous source byte << (8 - shift)
      // CL = shift
      // DX = bytes remaining in row

      mov   bl, al
      mov   ah, [esi]
      xor   al, al
      inc   esi
      shr   ax, cl      // AH = src >> shift, 
      or bl, ah      // AL = src << (8 - shift)
      or [edi], bl
      inc   edi

      dec   edx
      jnz   byte_loop

   last_byte:               
//    or cl, cl
//    jz row_done
      test  ch, 1
      jnz   row_done
      or [edi], al      // Last dest byte of row
   row_done:
      // Done with row, go to next
      add   esi, nSrcRowOffset
      add   edi, nDestRowOffset
      dec   nRows
      jnz   row_loop
   }
#else
   // 16-bit version
   _asm
   {
      push  ds
      
      les   di, lpbDest
      lds   si, lpbSrc
      
      mov   cx, nShift
      mov   ch, clip_flags

   row_loop:
      mov   dx, nBytesPerRow

      xor   al, al

      test  clip_flags, 2
      jz byte_loop

   // Clip left
      mov   ah, ds:[si]
      inc   si
      shr   ax, cl
      dec   dx
      jz last_byte

   byte_loop:
      // AL = previous source byte << (8 - shift)
      // CL = shift
      // DX = bytes remaining in row


      mov   bl, al
      mov   ah, ds:[si]
      xor   al, al
      inc   si
      shr   ax, cl      // AH = src >> shift, 
      or bl, ah      // AL = src << (8 - shift)
      or es:[di], bl
      inc   di

      dec   dx
      jnz   byte_loop
               
   last_byte:               
//    or cl, cl
//    jz row_done
      test  ch, 1
      jnz   row_done
      or es:[di], al    // Last dest byte of row
   row_done:
      // Done with row, go to next
      add   si, nSrcRowOffset
      add   di, nDestRowOffset
      dec   nRows
      jnz   row_loop
      
      pop   ds
   }

#endif   // _WIN32

#endif   // _NEWPORTABLE

} // end or_rows


// Initialize the global offscreen map structures to describe a
// map the covers the given rectangle.
//
// NOTE: This function assumes any existing offscreen bitmap
// has been released.
static void setup_offscreen_map(const CRect crRect)
{
   // Compute the offscreen bitmap metrics.
   mapOffscreen.map_memory = NULL;
   mapOffscreen.width = crRect.Width()+1;
   mapOffscreen.height = crRect.Height()+1;

   // NOTE: The offscreen MAP is created DWORD aligned. This is
   // the same as all the MAP structures initialized with bitmaps
   // from the TrueType engine. We use this convention because
   // we may at some point want to go to DIBs which are DWORD
   // aligned. It does, however, consume more memory and slow things
   // down a wee bit.
   WORD wRowBytes = 4*((mapOffscreen.width+31)/32);
   DWORD dwBytes = (DWORD)wRowBytes*(DWORD)mapOffscreen.height;

   // Make sure we don't allocate a map which could generate more
   // than more than 65520 bytes here or in other places.

   // PMGTODO: I'm (FF) not sure what the extra rows and columns are for.
   // This should be investigated to make sure it's something real or
   // just a fudge factor that someone guessed would be needed.

   if (dwBytes
         + wRowBytes*2                          // For extra rows
         + 4*mapOffscreen.height > 65520L)      // For extra columns (padded!)
   {
      return;
   }

   // Attempt to allocate the memory.
	mapOffscreen.map_memory = NULL;
	TRY
		mapOffscreen.map_memory = new BYTE[dwBytes];
	END_TRY

   if (mapOffscreen.map_memory == NULL)
   {
      return;
   }

   // Initialize the map bits to zero.
   memset(mapOffscreen.map_memory, 0, (int)dwBytes);

   // Remember the offset of the offscreen bitmap.
   ptOffscreenMapOrigin.x = crRect.left;
   ptOffscreenMapOrigin.y = crRect.top;
}


// Release any current offscreen bitmap.
static void release_offscreen_map()
{
   if (mapOffscreen.map_memory != NULL)
   {
		delete [] mapOffscreen.map_memory;
      mapOffscreen.map_memory = NULL;
   }
}


// Check if any offscreen bitmap is currently allocated.
inline BOOL offscreen_map_available()
{
   return (mapOffscreen.map_memory != NULL);
}


// Draw to the offscreen MAP
static void draw_character_offscreen(RedisplayContextPtr rc, MAPPTR map, int x, int y)
{
   // The values of ptOffscreenMapOrigin and mapOffscreen must
   // be valid before calling this function.
   //
   // ptOffscreenMapOrigin contains the pixel offsets from the coordinates of the
   // offscreen bitmap to the coordinate system of the passed in x and y.
   //
   // mapOffscreen is the MAP structure describing the offscreen bitmap. The offscreen
   // bitmap memory is set up with the rows DWORD aligned.
   //
   // NOTE: since we are dealing only with MAP's, we won't be using the GDI to move
   // bits around. Instead, we'll do it the old fashion way... or_rows().


   // Unpack some information about the offscreen bitmap.
   SHORT width = map->width;
   SHORT height = map->height;
   int nXOffset = (int)x-ptOffscreenMapOrigin.x;                   
   int nYOffset = (int)y-ptOffscreenMapOrigin.y;
   int nDestBytesPerRow = 4*(((int)mapOffscreen.width+31)/32);
   int nSourceBytesPerRow = 4*(((int)width+31)/32);

   // Figure out the parameters to use for or_rows(). Since the MAP's
   // can overlap (or not overlap) in any way, we have find the parameters
   // for the actual overlapping area.

   BOOL fClipLeft = FALSE;
   BOOL fClipRight = FALSE;
   LPBYTE lpbDest;
   LPBYTE lpbSrc;

   // TOP clip
   if (nYOffset < 0)
   {
      // Top is clipped.
      lpbDest = (LPBYTE)mapOffscreen.map_memory;
      lpbSrc = (LPBYTE)map->map_memory-(nYOffset*nSourceBytesPerRow);
      height += nYOffset;
      nYOffset = 0;
   }
   else
   {
      // Top is not clipped.
      lpbDest = (LPBYTE)mapOffscreen.map_memory+(nYOffset*nDestBytesPerRow);
      lpbSrc = (LPBYTE)map->map_memory;
   }

   // BOTTOM clip
   if (nYOffset + height > mapOffscreen.height)
   {
      // Bottom is clipped.
      height = mapOffscreen.height-nYOffset;
   }

   int nBytesPerRowToUse = nSourceBytesPerRow;
   int nShift = nXOffset % 8;

   // LEFT clip
   if (nXOffset < 0)
   {
      // Left is clipped.
      int nLeftBytesSkip = (-nXOffset) / 8;
      lpbSrc += nLeftBytesSkip;
      nBytesPerRowToUse -= nLeftBytesSkip;
      nShift = (nXOffset + 8 * (nLeftBytesSkip + 1)) % 8;
      width += nXOffset;
      nXOffset = 0;
      fClipLeft = (nShift != 0);
   }
   else
   {
      // Left is not clipped.
      lpbDest += (nXOffset / 8);
   }

   // RIGHT clip
   int nMaxDestBits = 8 * ((mapOffscreen.width + 7) / 8);
   if (nXOffset + width > nMaxDestBits)
   {
      // Right is clipped.
      int nRightBitsSkip = nXOffset + 8 * nBytesPerRowToUse - nMaxDestBits - (fClipLeft ? 8-nShift : 0);
      int nRightBytesSkip = nRightBitsSkip / 8;
      nBytesPerRowToUse -= nRightBytesSkip;
      fClipRight = ((nRightBitsSkip % 8) != 0);
      width -= nRightBitsSkip;
   }

   // If the overlapping area is not empty, call or_rows() to
   // merge the data into the offscreen bitmap.
   if ((nBytesPerRowToUse > 0)
    && (width > 0)
    && (height > 0))
   {
      or_rows(
         lpbDest,
         lpbSrc,
         nShift,
         height,
         nBytesPerRowToUse,
         nDestBytesPerRow,
         nSourceBytesPerRow,
         fClipLeft,
         fClipRight);
   }
}

// Do a blit a number of times to clear out an area for a bitmap.
// This is the infamous "white out" area behind the text.
static void draw_character_whiteout(RedisplayContextPtr rc, MAPPTR map, int x, int y)
{
   // Any MAP structure passed into this function has rows that
   // are DWORD aligned. This function uses DDBs which are WORD
   // aligned, so we adjust the "width" to make sure the last
   // word is included in the DDB.
   SHORT width = map->width;
   if (((width-1) % 32) <= 16)
   {
      // Bump width so it extends one pixel into next word.
      // This will force DWORD and WORD alignment to have the
      // same width.
      width = width+16-((width-1) % 32);
   }
   SHORT height = map->height;
   SHORT bmWidth = width+2;
   SHORT bmHeight = height+2;

   HDC hdc = rc->destination_hdc;
   COLORREF oldcolor = SetTextColor(hdc, RGB(0,0,0));

   // Create our temporary bitmap which will receive the white-out mask. This bitmap
   // will be used to white-out the appropriate bits in the destination DC.
   HBITMAP temp_bitmap;
   if ((temp_bitmap = ::CreateBitmap(bmWidth, bmHeight, 1, 1, NULL)) != NULL)
   {
      HBITMAP old_temp_bitmap;
      if ((old_temp_bitmap = (HBITMAP)SelectObject(temp_hdc, temp_bitmap)) != NULL)
      {
         // Create a bitmap containing the character image.
         HBITMAP source_bitmap;
         if ((source_bitmap = ::CreateBitmap(width, height, 1, 1, map->map_memory)) != NULL)
         {
            HBITMAP old_source_bitmap;
            if ((old_source_bitmap = (HBITMAP)SelectObject(source_hdc, source_bitmap)) != NULL)
            {
               // Initialize the mask to all black (0).
               ::PatBlt(temp_hdc, 0, 0, bmWidth, bmHeight, BLACKNESS);

               // Draw the character into the temp bitmap nine times.
               // This will create an image of the character with a one-pixel
               // outline around it.

               // PMGTODO: There must be some way to get around doing nine blits!
               // PMG 3.0 (16-bit) used special ASM code which took the character
               // bitmap and built the mask data. We should profile the drawing
               // pipeline to see how much time the white-out takes.

               for (SHORT mask_y = 0; mask_y < 3; mask_y++)
               {
                  for (SHORT mask_x = 0; mask_x < 3; mask_x++)
                  {
                     ::BitBlt(temp_hdc, mask_x, mask_y, width, height, source_hdc, 0, 0, SRCPAINT);
                  }
               }

					// Bitmap has 1 in white-out area, 0 in non-white-out area.

               // We now have the white-out mask, BitBlt it to the destination
               // DC to do the white-out.
               //
               // NOTE: StretchBlt() is used instead of BitBlt() because it
               // works in metafiles.

					DWORD dwRop = SRCPAINT;

					if (rc->is_postscript)
					{
						// Handle PostScript case.
						dwRop = NOTSRCAND;
						SetTextColor(hdc, RGB(255,255,255));

						if (Util::IsWinNT())
						{
							::PatBlt(temp_hdc, 0, 0, bmWidth, bmHeight, DSTINVERT);
						}
					}

               ::StretchBlt(hdc, x-1, y-1, bmWidth, bmHeight, temp_hdc, 0, 0, bmWidth, bmHeight, dwRop);

               SelectObject(source_hdc, old_source_bitmap);
            }
            DeleteObject(source_bitmap);
         }
         SelectObject(temp_hdc, old_temp_bitmap);
      }
      DeleteObject(temp_bitmap);
   }

	SetTextColor(hdc, oldcolor);
}

#ifdef WIN32
static void Free24BitDibForPostScript(BITMAPINFOHEADER& Header, void*& pBits)
{
	delete [] (BYTE*)pBits;
	pBits = NULL;
}

static BOOL Make24BitDibForPostScript(MAPPTR map, COLORREF clColor, BITMAPINFOHEADER& Header, void*& pBits)
{
	ASSERT(clColor != RGB(255,255,255));

	BOOL fWinNT = Util::IsWinNT();

	BYTE bRed = GetRValue(clColor);
	BYTE bGreen = GetGValue(clColor);
	BYTE bBlue = GetBValue(clColor);

	BOOL fResult = FALSE;

	pBits = NULL;

	if (map->height > 0)
	{
		BYTE* pSource = (BYTE*)(map->map_memory);
		if (pSource != NULL)
		{
			int nSourceRowBytes = ((map->width+31) & ~31) >> 3;
			if (nSourceRowBytes > 0)
			{
				InitBitmapInfoHeader(&Header, map->width, map->height, 24);

				int nDestinationRowBytes = ((map->width*3)+3) & ~3;
				if (nDestinationRowBytes > 0)
				{
					ASSERT((DWORD)(nDestinationRowBytes*map->height) == Header.biSizeImage);

					TRY
					{
						pBits = new BYTE [Header.biSizeImage];
					}
					END_TRY

					if (pBits != NULL)
					{
						BYTE* pSourceRow = (BYTE*)(map->map_memory)+(map->height-1)*nSourceRowBytes;
						BYTE* pDestinationRow = (BYTE*)pBits;
						for (int nRow = 0; nRow < map->height; nRow++)
						{
							BYTE* pSourceByte = pSourceRow;
							BYTE* pDestinationByte = pDestinationRow;

							BYTE bBit = 0x80;

							BYTE bSourceByte = *pSourceByte;
							if (fWinNT)
							{
								bSourceByte = ~bSourceByte;
							}

							for (int nBit = 0; nBit < map->width; nBit++)
							{
								if ((bSourceByte & bBit) != 0)
								{
									*(pDestinationByte++) = 0xff;
									*(pDestinationByte++) = 0xff;
									*(pDestinationByte++) = 0xff;
								}
								else
								{
									if (fWinNT)
									{
										*(pDestinationByte++) = bRed;
										*(pDestinationByte++) = bGreen;
										*(pDestinationByte++) = bBlue;
									}
									else
									{
										*(pDestinationByte++) = 0x00;
										*(pDestinationByte++) = 0x00;
										*(pDestinationByte++) = 0x00;
									}
								}

								bBit >>= 1;
								if (bBit == 0x00)
								{
									bBit = 0x80;
									bSourceByte = *(++pSourceByte);
									if (fWinNT)
									{
										bSourceByte = ~bSourceByte;
									}
								}
							}

							pSourceRow -= nSourceRowBytes;
							pDestinationRow += nDestinationRowBytes;
						}

						fResult = TRUE;
					}
				}
			}
		}
	}

	if (!fResult)
	{
		Free24BitDibForPostScript(Header, pBits);
	}

	return fResult;
}
#endif

// Draw the character bitmap.
static void draw_character_bitmap(RedisplayContextPtr rc, MAPPTR map, int x, int y, BOOL fMask)
{
	BOOL fWinNT = Util::IsWinNT();

   // Any MAP structure passed into this function has rows that
   // are DWORD aligned. This function uses DDBs which are WORD
   // aligned, so we adjust the "width" to make sure the last
   // word is included in the DDB.
   SHORT nWidth = map->width;
   if (((nWidth-1) % 32) <= 16)
   {
      // Bump width so it extends one pixel into next word.
      // This will force DWORD and WORD alignment to have the
      // same width.
      nWidth = nWidth+16-((nWidth-1) % 32);
   }
   SHORT nHeight = map->height;

   HDC hdc = rc->destination_hdc;

	if (rc->is_postscript)
	{
		// PostScript case.
#ifdef WIN32
		if (!fMask && (text_color != RGB(0,0,0)) && (text_color != RGB(255,255,255)))
		{
			// Colored text (not black or white.)
			BITMAPINFO BitmapInfo;
			void* pBits = NULL;
			if (Make24BitDibForPostScript(map, text_color, BitmapInfo.bmiHeader, pBits))
			{
				StretchDIBits(
					hdc,
					x,
					y,
					BitmapInfo.bmiHeader.biWidth,
					BitmapInfo.bmiHeader.biHeight,
					0,
					0,
					BitmapInfo.bmiHeader.biWidth,
					BitmapInfo.bmiHeader.biHeight,
					pBits,
					&BitmapInfo,
					DIB_RGB_COLORS,
					NOTSRCAND);

				Free24BitDibForPostScript(BitmapInfo.bmiHeader, pBits);
			}
		}
		else
		{
			// Black or white text or mask.
			HBITMAP hBitmap;
			if ((hBitmap = ::CreateBitmap(nWidth, nHeight, 1, 1, map->map_memory)) != NULL)
			{
				HGDIOBJ hOldBitmap;
				if ((hOldBitmap = ::SelectObject(source_hdc, hBitmap)) != NULL)
				{
					DWORD dwRop = NOTSRCAND;

					if (fWinNT)
					{
						::PatBlt(source_hdc, 0, 0, nWidth, nHeight, DSTINVERT);
						if (!fMask && (text_color == RGB(0,0,0)))
						{
							dwRop = SRCAND;
						}
					}
					else
					{
						if (!fMask && (text_color == RGB(0,0,0)))
						{
							::PatBlt(source_hdc, 0, 0, nWidth, nHeight, DSTINVERT);
							dwRop = SRCAND;
						}
					}

	            ::StretchBlt(
						hdc,
						x,
						y,
						nWidth,
						nHeight,
						source_hdc,
						0,
						0,
						nWidth,
						nHeight,
						dwRop);

					::SelectObject(source_hdc, hOldBitmap);
					hOldBitmap = NULL;
				}

		      ::DeleteObject(hBitmap);
				hBitmap = NULL;
			}
		}
#else
		HBITMAP hBitmap;
		if ((hBitmap = ::CreateBitmap(nWidth, nHeight, 1, 1, map->map_memory)) != NULL)
		{
			HGDIOBJ hOldBitmap;
			if ((hOldBitmap = ::SelectObject(source_hdc, hBitmap)) != NULL)
			{
		      ::StretchBlt(hdc, x, y, nWidth, nHeight, source_hdc, 0, 0, nWidth, nHeight, NOTSRCAND);
		      
				::SelectObject(source_hdc, hOldBitmap);
				hOldBitmap = NULL;
			}

	      ::DeleteObject(hBitmap);
			hBitmap = NULL;
		}
#endif		
	}
	else
	{
		HBITMAP hBitmap;
		if ((hBitmap = ::CreateBitmap(nWidth, nHeight, 1, 1, map->map_memory)) != NULL)
		{
			// Non-PostScript case.
		   HGDIOBJ hOldBitmap;
		   if ((hOldBitmap = ::SelectObject(source_hdc, hBitmap)) != NULL)
			{
				if (fMask)
				{
					// Set D to 1 where S is 1, leave D unchanged where S is 0.
					::StretchBlt(hdc, x, y, nWidth, nHeight, source_hdc, 0, 0, nWidth, nHeight, SRCPAINT);
				}
				else
				{
					if (text_color == RGB(0,0,0))
					{
						// Set D to 0 (black) where S is 1, leave D unchanged where S is 0.
						::StretchBlt(hdc, x, y, nWidth, nHeight, source_hdc, 0, 0, nWidth, nHeight, NOTSRCAND);
					}
					else if (text_color == RGB(255,255,255))
					{
						// Set D to 1 (white) where S is 1, leave D unchanged where S is 0.
						::StretchBlt(hdc, x, y, nWidth, nHeight, source_hdc, 0, 0, nWidth, nHeight, SRCPAINT);
					}
					else
					{
						if (rc->is_printing)
						{
							// Printers do not in general support DSPDxax, so simulate it.
							// DSPDxax == PSnDPxax

							// Both set D to P where S is 1 and leave D unchanged where S is 0.
							::PatBlt(hdc, x, y, nWidth, nHeight, PATINVERT);
							::PatBlt(source_hdc, 0, 0, nWidth, nHeight, DSTINVERT);
							::StretchBlt(hdc, x, y, nWidth, nHeight, source_hdc, 0, 0, nWidth, nHeight, SRCAND);
							::PatBlt(hdc, x, y, nWidth, nHeight, PATINVERT);
						}
						else
						{
							// Use a single DSPDxax blit.

							::StretchBlt(hdc, x, y, nWidth, nHeight, source_hdc, 0, 0, nWidth, nHeight, 0x00e20746L);
						}
					}
				}

				::SelectObject(source_hdc, hOldBitmap);
				hOldBitmap = NULL;
			}

	      ::DeleteObject(hBitmap);
			hBitmap = NULL;
		}
   }
}

/*
// Draw a strip.
*/

VOID PMGFontServer::DrawStrip(
   RedisplayContextPtr rc,
   MAPPTR pMap,
   int nX,
   int nY,
	BOOL fMask,
   BOOL fClearOut)
{
   if (offscreen_map_available())
   {
      draw_character_offscreen(rc, pMap, nX, nY);
   }
   else
   {
      if (fClearOut)
      {
         draw_character_whiteout(rc, pMap, nX, nY);
      }
      else
      {
         draw_character_bitmap(rc, pMap, nX, nY, fMask);
      }
   }
}

#ifdef WIN32
#define MAX_BITMAP_SIZE 0x0f000L
#else
#define MAX_BITMAP_SIZE 0x08000L
#endif

HBRUSH PMGFontServer::create_dib_brush(COLORREF color)
{
/*
// Create the memory block if necessary.
*/

   if (m_dib_memory == NULL)
   {
   /* We don't have a handle yet. */

      if ((m_dib_handle = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(DIBPATTERN))) == NULL)
      {
         return NULL;
      }

      if ((m_dib_memory = (LPBYTE)::GlobalLock(m_dib_handle)) == NULL)
      {
         ::GlobalFree(m_dib_handle);
         m_dib_handle = NULL;
         return NULL;
      }

   /* Initialize the color array if not initialized yet. */

      if (dib_pattern.bmiColors[1] == 0)
      {
         SHORT *q;
         SHORT i;

         for (i = 0, q = dib_pattern.bmiColors; i < 256; )
         {
            *q++ = i++;
         }
      }

   /* Copy the base info over. */

      memcpy(m_dib_memory, &dib_pattern, sizeof(DIBPATTERN));

      m_dib_rgb = RGB(0, 0, 0);
   }

/*
// Prepare the dib pattern if it's not correct.
*/

   if (color != m_dib_rgb)
   {
   /* Delete any old brush we had since it's now obsolete. */

      if (m_dib_brush != NULL)
      {
         ::DeleteObject(m_dib_brush);
         m_dib_brush = NULL;
      }

      Create8x8Dither(GetRValue(color),
                      GetGValue(color),
                      GetBValue(color),
                      ((LPDIBPATTERN)m_dib_memory)->bmiData);

      m_dib_rgb = color;
   }

/*
// Create the brush if we need to.
*/

   if (m_dib_brush == NULL)
   {
      m_dib_brush = ::CreateDIBPatternBrush(m_dib_handle, DIB_PAL_COLORS);
   }

   return m_dib_brush;
}

/*
// Reset the fonts array (and anything else.)
*/

VOID PMGFontServer::reset_fonts(VOID)
{
	if (pFontCache != NULL)
	{
		pFontCache->DeleteAll();
	}

   if (pFontEngine != NULL)
   {
      pFontEngine->ClearLastFace();
   }
}

void PMGFontServer::DrawCharacter(
   RedisplayContextPtr rc,
   CHARACTER Character,
   const CCharacterFormatting* pCharacterFormat,
	CFontCache::FontCacheItem* pFontItem,
   PCOORD nX,
   PCOORD nY,
	DWORD dwComponent,
   CFrameObject* pFrameObject)
{
	if (Character == ' ')
	{
		return;
	}

	BOOL fMask = (dwComponent == COMPONENT_WhiteOutMask)
				 || (dwComponent == COMPONENT_ShadowMask)
				 || (dwComponent == COMPONENT_FillMask)
				 || (dwComponent == COMPONENT_OutlineMask);

	// Check if object is warped, rotated, or flipped.
   CWarpField* pWarpField = pFrameObject->GetWarpField();
	BOOL fXFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_xflipped) != 0;
	BOOL fYFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_yflipped) != 0;
   BOOL fWarped = pWarpField != NULL;
	BOOL fInflateOutline = FALSE;

   ANGLE dRotation = pFrameObject->get_rotation();
	if (fXFlipped != fYFlipped)
	{
		dRotation = -dRotation;
	}

   BOOL fRotated = dRotation != 0.0;

	// Check if this is an outline component for which we have to do an inflate.
	if ((dwComponent == COMPONENT_Outline) || (dwComponent == COMPONENT_OutlineMask))
	{
		if (pCharacterFormat->m_Outline.m_nStyle == OutlineFormat::Inflate)
		{
			fInflateOutline = TRUE;
		}
	}

	// Build a structure that will describe the font to the font engine.
	CFontDescription* pFontDescription = &(((CFontCache::FontCacheItem::FontCacheKey*)(pFontItem->GetKey()))->m_Description);

	G_FONT_DESCRIPTION Font;

	Font.typeface					= pFontDescription->m_nTypeface;
	Font.Fill						= pFontDescription->m_Fill;
	Font.Outline					= pFontDescription->m_Outline;
	Font.Shadow						= pFontDescription->m_Shadow;
	Font.point_size				= pFontDescription->m_lPointSize;
	Font.vertical_expansion		= MakeFixed(1);
	Font.horizontal_expansion  = pFontDescription->m_lHorizontalExpansion;
	Font.oblique					= MakeFixed(0);
	Font.x_resolution				= rc->GetScaledXResolution();
	Font.y_resolution				= rc->GetScaledYResolution();
	Font.effects_rotation		= MakeFixed(dRotation*RAD2DEGC);

	// Get a pointer to the face entry for the font.
	FaceEntry* pFace = typeface_server.get_face(typeface_index(pFontDescription->m_nTypeface));

	// Get unrotated bounds of frame object.
	PBOX Bound = pFrameObject->get_unrotated_bound();

	// Compute the unrotated bounds in screen coordinates.
	PBOX ScreenBound = Bound;
	rc->pbox_to_screen(&ScreenBound, TRUE);
	CRect crFrameObject;
	rc->convert_pbox(&ScreenBound, crFrameObject, NULL);

	// Pointer to object used for warped and/or rotated characters.
	CFontCharacterOutlineInformation* pOutlineInformation = NULL;

	// Map PM character index to a UGL code for TrueType.
   Character = character_map(Character, pFace);

	// Create a key for looking up the character in the character cache.
	CCharacterCache::CharacterCacheItem::CharacterCacheKey CharacterKey;
	CharacterKey.m_Description.m_nXResolution = Font.x_resolution;
	CharacterKey.m_Description.m_nYResolution = Font.y_resolution;

	CharacterKey.m_Description.m_lRotation = MakeFixed(dRotation*RAD2DEGC);

	if (fWarped)
	{
		// Character is warped, so need to fill in the key warp fields.
		CharacterKey.m_Description.m_dwWarpDocument = (DWORD)(pFrameObject->get_database());
		CharacterKey.m_Description.m_lWarpNumber = 0;
		CFrameRecord* pFrameRecord = pFrameObject->LockFrameRecord();
		if (pFrameRecord != NULL)
		{
			CharacterKey.m_Description.m_lWarpNumber = pFrameRecord->GetWarpFieldRecord();
			pFrameRecord->release();
		}
		CharacterKey.m_Description.m_nWarpWidth = Bound.x1-Bound.x0;
		CharacterKey.m_Description.m_nWarpHeight = Bound.y1-Bound.y0;
		CharacterKey.m_Description.m_WarpPoint.x = nX-Bound.x0;
		CharacterKey.m_Description.m_WarpPoint.y = nY-Bound.y0;
	}
	else
	{
		// Character is not warped, set warp fields to zero by convention.
		CharacterKey.m_Description.m_dwWarpDocument = 0;
		CharacterKey.m_Description.m_lWarpNumber = 0;
		CharacterKey.m_Description.m_nWarpWidth = 0;
		CharacterKey.m_Description.m_nWarpHeight = 0;
		CharacterKey.m_Description.m_WarpPoint.x = 0;
		CharacterKey.m_Description.m_WarpPoint.y = 0;
	}

	// Look up the character in the character cache.
	CCharacterCache::CharacterCacheItem* pCharacterItem = (CCharacterCache::CharacterCacheItem*)(pFontItem->m_CharacterCache.Lookup(&CharacterKey));
	if (pCharacterItem == NULL)
	{
		// The character attributes are not represented in the font's character
		// cache at the moment. Add a new character cache item so we have a place
		// to put bitmaps.
		TRY
		{
			// Create font cache item.
			pCharacterItem = new CCharacterCache::CharacterCacheItem(pFontCache, CharacterKey.m_Description);
			if (!pFontItem->m_CharacterCache.Insert(pCharacterItem))
			{
				// Could not add item to cache? Just delete it.
				ASSERT(FALSE);
				delete pCharacterItem;
				pCharacterItem = NULL;
			}
		}
		END_TRY
	}

	// We should have a character cache item by now.
	// PMGTODO: We don't know yet if we're going to cache the character bitmap,
	// so it might seem wasteful to create a character cache item for the
	// character at this point. We could delay the creation or delete the item
	// if it's character bitmap cache is empty.
	ASSERT(pCharacterItem != NULL);
	if (pCharacterItem != NULL)
	{
		// If the character is warped, it's origin will probably be moved from
		// the "normalized" value relative to the frame. The character bitmap
		// allows us to store the transformed origin so we won't have to rewarp
		// and rotate the origin when we want to draw the cached bitmap.
		CFixedPoint OriginWarpOffset;
		OriginWarpOffset.x = 0;
		OriginWarpOffset.y = 0;

		// Check if there is a cached bitmap for the character.
		CCharacterBitmapCache::CharacterBitmapCacheItem::CharacterBitmapCacheKey BitmapKey(Character, dwComponent);

		// Lookup the bitmap in the character bitmap cache.
		CCharacterBitmapCache::CharacterBitmapCacheItem* pBitmapItem = (CCharacterBitmapCache::CharacterBitmapCacheItem*)(pCharacterItem->m_CharacterBitmapCache.Lookup(&BitmapKey));

		// Compute the bounding box for the character. If the character is
		// cached, then the bounding box is in the cache item. If the character
		// is not cached, then we'll need to size the character bitmap using the
		// font engine.

		CRect crCharacter;
		crCharacter.SetRectEmpty();

		G_BITMAP_PARAMETERS bitmap_parameters;

		// Check if the have a cached bitmap.
		if (pBitmapItem != NULL)
		{
			// We have a cached bitmap, use it to draw.
			ASSERT(pBitmapItem->m_Bitmap.m_Map.map_memory != NULL);

			// Get the cache character size.
			crCharacter = pBitmapItem->m_Bitmap.m_crCharacter;

			if (fWarped || fRotated)
			{
				// The character is warped and/or rotated so we're going
				// to have to adjust the origin at which to draw the cached
				// bitmap to match the transformation. For warping, the offset
				// for the origin is cached with the character bitmap. We can
				// do this because each bitmap within a warp field is individually
				// cached and thus has a unique offset. Rotated characters can be
				// reused within a text box and thus don't have a single offset
				// which can be cached with the character. So, for rotated
				// cached bitmaps we have to re-rotate the origin.

				CFixedPoint Origin;
				Origin.x = MakeFixed(rc->page_x_to_screen(nX));
				Origin.y = MakeFixed(rc->page_y_to_screen(nY));

				if (fWarped)
				{
					// Apply the origin offset to account for any warping (and rotation.)
					Origin.x += pBitmapItem->m_Bitmap.m_OriginWarpOffset.x;
					Origin.y += pBitmapItem->m_Bitmap.m_OriginWarpOffset.y;
				}
				else
				{
					ASSERT(fRotated);

					// Must be rotated if we get here.
					COutlinePathMatrix RotateMatrix;
					RotateMatrix.Identity();

					RotateMatrix.TranslateBy(
						-MakeFixed(crFrameObject.left+crFrameObject.Width()/2),
						-MakeFixed(crFrameObject.top+crFrameObject.Height()/2));
					RotateMatrix.RotateBy(MakeFixed(dRotation*RAD2DEGC));
					RotateMatrix.TranslateBy(
						MakeFixed(crFrameObject.left+crFrameObject.Width()/2),
						MakeFixed(crFrameObject.top+crFrameObject.Height()/2));

					// Rotate the origin.)
					RotateMatrix.Transform(&Origin.x, &Origin.y);
				}

				// Translate origin back into page coordinates for common code.
				nX = rc->screen_x_to_page(FixedRound(Origin.x));
				nY = rc->screen_y_to_page(FixedRound(Origin.y));
			}
		}
		else
		{
			// We don't have a cached bitmap, so we're going to have to generate the
			// bitmap from scratch. First we need to compute the size information.
			// Warped and/or rotated characters need are generated from the character
			// outline data. Normal characters are passed directly to the TrueType engine.

			if (fWarped || fRotated || fInflateOutline)
			{
				// We have a warped and/or rotated character. These characters are
				// built using the character outline. The outline data is sourced by
				// the font engine, warped and/or rotated by code in PrintMaster,
				// and then sent back to the font engine for rasterization. The final
				// bitmap is sent to the destination DC. If the bitmap is not clipped
				// and is a reasonable size, then it will be added to the cache.

				// Get the character outline. If this fails, no character is drawn.
				//
				// NOTE: In this case, the path returned by CharacterOutline will
				// be in the device coordinate system (using CFixed numbers.)

				COutlinePath Path;
				CFixedRect PathBounds;
				if (CharacterOutline(rc, Character, pFontDescription, &PathBounds, &Path))
				{
					// Allocate a CFontCharacterOutlineInformation object which knows how to get
					// the character path into a form which the font engine can rasterize.
					pOutlineInformation = new CFontCharacterOutlineInformation;
					if (pOutlineInformation != NULL)
					{
						// Compute the origin of the character. Note that this is the
						// "normalized" origin without any warping, rotation, or flipping.
						// This point will later be used to compute the offset to the
						// warped and rotated origin. If the character bitmap is cached,
						// the offset will be saved along with the bitmap so it will not
						// have to be recomputed.

						CFixedPoint DrawOrigin;
						DrawOrigin.x = MakeFixed(rc->page_x_to_screen(nX));
						DrawOrigin.y = MakeFixed(rc->page_y_to_screen(nY));

						// Compute another origin that will will have warping and
						// rotation applied.
						CFixedPoint Origin = DrawOrigin;

						// Translate the character data to the normalized coordinates (the character path
						// returned by CharacterOutline() is in the coordinate system of the character where
						// the character reference point is at (0,0).
						COutlinePathMatrix Matrix;
						Matrix.Identity();

						if (fWarped)
						{
							// We are warping the outline and need to do something special to get the
							// effects to look "right". Some of the PMG font effects (e.g., sunset)
							// use the font cell height, width, ascent, and character cell extents
							// to generate their patterns. The problem is that warping distorts the
							// character cell in ways that makes it difficult to apply the effect
							// across a range of characters within a warp envelope and have it look
							// right. Basically, what gets lost is a symmetric look to the patterns
							// being applied to characters in different areas of the warp envelope.
							// To do things exactly right, the pattern should be warped, but this
							// would make things REALLY slow. So, to achieve some symmetry, the
							// following method is used:
							//
							//		1) The character is translated so that the origin is at the
							//			horizontal center of the character glyph.
							//
							//		2) The line through the origin running between the top and bottom
							//			of the font	cell bounding box is warped. The resulting line
							//			height is used as the font cell height. The distance the warped
							//			line rises above the origin is used as the cell height.
							//
							//		3) The line through the origin running between the left and right
							//			sides of the font bounding box is warped. The resulting line
							//			width is used as the font cell width.
							//
							//		4) The bounding box of the warped path is used for the character
							//			cell extents.
							//
							// Empirically, this seems to produce pattern fills which are symmetric
							// in the warp envelopes provided by PMG.

							// Since the path is warped, translate the path so that the X coordinate
							// of the origin is on the center line of the character glyph.

							CFixed lXOffset = (PathBounds.Left+PathBounds.Right)/2;
							Matrix.TranslateBy(-lXOffset, 0);
							Origin.x += lXOffset;
						}

						Matrix.TranslateBy(Origin.x, Origin.y);
						Path.Transform(&Matrix);

						// If there is a warp field, then warp the path data. Note that the coordinates
						// in the outline path must be offset by the character origin (they are) because
						// warping is highly dependent on the position of the character within the warp field.
						if (fWarped)
						{
							// Make sure the warp field is set to warp in the device coordinate system.
							pWarpField->Transform(crFrameObject);

							// Warp the path.
							Path.Warp(pWarpField);

							// As described above, compute the new metrics for pattern generation.
							pOutlineInformation->m_fUseNewMetrics = TRUE;

							// Get the extents of the font cell.
							CFixed lXFactor = MakeFixed((long)Font.x_resolution)/PAGE_RESOLUTION;
							CFixed lLeft = lXFactor*pFontItem->m_Metrics.m_Bounds.x0;
							CFixed lRight = lXFactor*pFontItem->m_Metrics.m_Bounds.x1;
							CFixed lYFactor = MakeFixed((long)Font.y_resolution)/PAGE_RESOLUTION;
							CFixed lTop = lYFactor*pFontItem->m_Metrics.m_Bounds.y0;
							CFixed lBottom = lYFactor*pFontItem->m_Metrics.m_Bounds.y1;

							// Warp the line running through the origin from the top to bottom
							// sides of font cell.
							CFixed lHeightX0 = Origin.x;
							CFixed lHeightY0 = lTop+Origin.y;
							pWarpField->WarpPoint(&lHeightX0, &lHeightY0);
							CFixed lHeightX1 = Origin.x;
							CFixed lHeightY1 = lBottom+Origin.y;
							pWarpField->WarpPoint(&lHeightX1, &lHeightY1);

							// Warp the line running through the origin from the left to right
							// sides of font cell.
							CFixed lWidthX0 = lLeft+Origin.x;
							CFixed lWidthY0 = Origin.y;
							pWarpField->WarpPoint(&lWidthX0, &lWidthY0);
							CFixed lWidthX1 = lRight+Origin.x;
							CFixed lWidthY1 = Origin.y;
							pWarpField->WarpPoint(&lWidthX1, &lWidthY1);

							// Warp the origin. This is needed when we want to rasterize
							// the character. For rasterization, we want the character
							// origin to be at (0,0).
							pWarpField->WarpPoint(&Origin.x, &Origin.y);

							// Save the new font cell metrics for pattern generation.
							pOutlineInformation->m_dwCellHeight = FixedRound(lHeightY1-lHeightY0);
							pOutlineInformation->m_dwCellWidth = FixedRound(lWidthX1-lWidthX0);
							pOutlineInformation->m_lCellAscent = FixedRound(-(lHeightY0-Origin.y));
							
							// Save the new character cell metrics for pattern generation.
							CFixedRect WarpedPathBounds;
							Path.QuickBoundingBox(&WarpedPathBounds);
							pOutlineInformation->m_lCharX0 = FixedRound(WarpedPathBounds.Left-Origin.x);
							pOutlineInformation->m_lCharX1 = FixedRound(WarpedPathBounds.Right-Origin.x);
							pOutlineInformation->m_lCharY0 = FixedRound(WarpedPathBounds.Top-Origin.y);
							pOutlineInformation->m_lCharY1 = FixedRound(WarpedPathBounds.Bottom-Origin.y);
						}

						// If the object is rotated, then we need to rotate it. Note that
						// the we're going to do the flipping afterward, so we have to
						// adjust the rotation angle if the object is flipped.
						//
						// Note: If the rasterization engine knew about flipping we could
						// flip the path before rasterization. But, it does not, so to make
						// the pattern data flipped, we have to flip the bitmap by hand.

						if (fRotated)
						{
							COutlinePathMatrix RotateMatrix;
							RotateMatrix.Identity();

							RotateMatrix.TranslateBy(
								-MakeFixed(crFrameObject.left+crFrameObject.Width()/2),
								-MakeFixed(crFrameObject.top+crFrameObject.Height()/2));
							RotateMatrix.RotateBy(MakeFixed(dRotation*RAD2DEGC));
							RotateMatrix.TranslateBy(
								MakeFixed(crFrameObject.left+crFrameObject.Width()/2),
								MakeFixed(crFrameObject.top+crFrameObject.Height()/2));

							// Rotate the path (and the origin.)
							Path.Transform(&RotateMatrix);
							RotateMatrix.Transform(&Origin.x, &Origin.y);
						}


						if (fWarped)
						{
							// Remember the offsets from the warped origin to the original origin.
							// These offsets will be saved along with the character bitmap in the
							// bitmap cache.
							OriginWarpOffset.x = Origin.x-DrawOrigin.x;
							OriginWarpOffset.y = Origin.y-DrawOrigin.y;
						}

						DWORD dwRasterizeComponent = dwComponent;

						if (fInflateOutline)
						{
							CFixed lWidth = MakeFixed(0);

							// Compute the width of the outline.
							if (pCharacterFormat->m_Outline.m_nWidthType == OutlineFormat::Absolute)
							{
								// Convert width from points.
								if (pCharacterFormat->m_Outline.m_lWidth > MakeFixed(0))
								{
									CFixed lXSize = MulDivFixed(pCharacterFormat->m_Outline.m_lWidth, Font.x_resolution, 72);
									CFixed lYSize = MulDivFixed(pCharacterFormat->m_Outline.m_lWidth, Font.y_resolution, 72);

									if (lXSize > lYSize)
									{
										lWidth = lXSize;
									}
									else
									{
										lWidth = lYSize;
									}
								}
							}
							else
							{
								// Compute the width of an outline relative to the font cell dimensions.
								CFixed lFontWidth = MakeFixed((long)Font.x_resolution)/PAGE_RESOLUTION;
								lFontWidth *= pFontItem->m_Metrics.m_Bounds.x1-pFontItem->m_Metrics.m_Bounds.x0;

								CFixed lFontHeight = MakeFixed((long)Font.y_resolution)/PAGE_RESOLUTION;
								lFontHeight *= pFontItem->m_Metrics.m_Bounds.y1-pFontItem->m_Metrics.m_Bounds.y0;

								CFixed lXSize = MulFixed(pCharacterFormat->m_Outline.m_lWidth, lFontWidth);
								CFixed lYSize = MulFixed(pCharacterFormat->m_Outline.m_lWidth, lFontHeight);

								if (lXSize > lYSize)
								{
									lWidth = lXSize;
								}
								else
								{
									lWidth = lYSize;
								}
							}

							if (lWidth >= MakeFixed(1.5))
							{
								// Compute two polygons: one for the border and one for the interior.
								COutlinePath InteriorPath;
								InteriorPath.Copy(&Path);
								InteriorPath.Inflate(-lWidth);
								InteriorPath.Reverse();
								Path.Append(&InteriorPath);

								dwRasterizeComponent = COMPONENT_Outline ? COMPONENT_Fill : COMPONENT_FillMask;

								Font.Fill.m_nPattern = FillFormatV1::Black;
							}
							else
							{
								Font.Outline.m_nStyle = OutlineFormat::Light;
							}
						}
						
						// Convert path to format which can be used by font engine.
						// Since the only font engine we have is TrueType, the format
						// used will be the TrueType character outline format.
						//
						// NOTE: The path data is translated back to its original origin (0,0)
						// so that the special effects will be generated correctly. The special
						// effects code assumes that the character origin is at (0,0).
						//
						// NOTE: The path is in device coordinates, so the resulting outline
						// information can be used to produce a bitmap for the device.
						pOutlineInformation->FromPath(&Path, -Origin.x, -Origin.y);

						// Set up to size the character bitmap.
						bitmap_parameters.command               = G_C_size;
						bitmap_parameters.font_description      = &Font;
						bitmap_parameters.character             = 0;
						bitmap_parameters.component             = dwRasterizeComponent;
						bitmap_parameters.outline               = pOutlineInformation;
						bitmap_parameters.buffer_pad_left       = 0;
						bitmap_parameters.buffer_pad_right      = 0;
						bitmap_parameters.flags                 = 0;
						bitmap_parameters.row_count             = 0;

						// Call the font engine to size the character. Remember, since the character
						// was translated back to its original origin, the bitmap bounding box values
						// that are returned reflect this translation.
						if (pFontEngine->make_bitmap(&bitmap_parameters) >= 0)
						{
							// Unpack the character size.
							crCharacter.left = bitmap_parameters.character_bbox.ulx;
							crCharacter.top = bitmap_parameters.character_bbox.uly;
							crCharacter.right = bitmap_parameters.character_bbox.lrx;
							crCharacter.bottom = bitmap_parameters.character_bbox.lry;
						}

						if (fWarped || fRotated)
						{
							// Translate origin back into page coordinates for common code.
							nX = rc->screen_x_to_page(FixedRound(Origin.x));
							nY = rc->screen_y_to_page(FixedRound(Origin.y));
						}
					}
				}
			}
			else
			{
				// The character is not warped or rotated, compute its size.

				// Set up to size the character bitmap.
				bitmap_parameters.command               = G_C_size;
				bitmap_parameters.font_description      = &Font;
				bitmap_parameters.character             = (USHORT)Character;
				bitmap_parameters.component             = dwComponent;
				bitmap_parameters.outline               = NULL;
				bitmap_parameters.buffer_pad_left       = 0;
				bitmap_parameters.buffer_pad_right      = 0;
				bitmap_parameters.flags                 = 0;
				bitmap_parameters.row_count             = 0;

				// Call the font engine to size the character bitmap.
				if ((pFontEngine->make_bitmap(&bitmap_parameters)) >= 0)
				{
					// Unpack the character size.
					crCharacter.left = bitmap_parameters.character_bbox.ulx;
					crCharacter.top = bitmap_parameters.character_bbox.uly;
					crCharacter.right = bitmap_parameters.character_bbox.lrx;
					crCharacter.bottom = bitmap_parameters.character_bbox.lry;
				}
			}
		}

		// Make sure we have a character.
		if ((crCharacter.Width() >= 0) && (crCharacter.Height() >= 0))
		{
			// Compute some metrics used for drawing.
			int nTop = -crCharacter.top;
			int nBottom = crCharacter.bottom+1;
			int nLeft = -crCharacter.left;
			int nRight = crCharacter.right+1;

			// If the character is flipped, adjust the metrics.
			if (fXFlipped)
			{
				nX = Bound.x0+Bound.x1-nX;

				int nTemp = nLeft;
				nLeft = nRight;
				nRight = nTemp;
			}
			if (fYFlipped)
			{
				nY = Bound.y0+Bound.y1-nY;

				int nTemp = nTop;
				nTop = nBottom;
				nBottom = nTemp;
			}

			// Get the reference point of the character.
			int nDrawX = rc->page_x_to_screen(nX);
			int nDrawY = rc->page_y_to_screen(nY);

			// If we have a cached bitmap, draw it now.
			if (pBitmapItem != NULL)
			{
				ASSERT(pBitmapItem->m_Bitmap.m_Map.map_memory != NULL);

				// Draw using the cached bitmap.
				MAP* pMap = &pBitmapItem->m_Bitmap.m_Map;

				// Note that all cached bitmaps come from the TrueType font engine
				// which DWORD aligns every row. Thus, the byte width should always
				// be a multiple of four.
				int nByteWidth = ((pMap->width + 7)/8 + 3) & ~3;

				// If we have to flip, save the map memory in a temporary buffer before flipping.
				LPVOID pBitmapSave = NULL;
				if (fXFlipped || fYFlipped)
				{
					// Save a pointer to the memory containing the unflipped bitmap.
					pBitmapSave = pMap->map_memory;
					unsigned int nBytes = pMap->height * nByteWidth;

					// Allocate memory for a saved copy of cached bitmap.
					TRY
					{
						pBitmapSave = new char[nBytes];
					}
					END_TRY

					// If we got the memory, copy the bitmap into it.
					if (pBitmapSave != NULL)
					{
						memcpy(pBitmapSave, pMap->map_memory, nBytes);
					}

					// Flip the data.
					if (fXFlipped)
					{
						xflip(
							(LPBYTE)pMap->map_memory,
							pMap->width,
							pMap->height,
							nByteWidth);
					}
					if (fYFlipped)
					{
						yflip(
							(LPBYTE)pMap->map_memory,
							pMap->width,
							pMap->height,
							nByteWidth);
					}
				}

				// Draw the entire bitmap.
				DrawStrip(
					rc,
					pMap,
					nDrawX-nLeft,
					nDrawY-nTop,
					fMask,
					dwComponent == COMPONENT_WhiteOutMask);

				// If we flipped the bitmap, then restore the unflipped version.
				if (fXFlipped || fYFlipped)
				{
					// Check if we made a backup copy.
					if (pBitmapSave != NULL)
					{
						// Delete the flipped version and attach the saved version.
						delete [] (char*)(pMap->map_memory);
						pMap->map_memory = pBitmapSave;
						pBitmapSave = NULL;
					}
					else
					{
						// We didn't make a backup copy of the bitmap. In this case
						// we have to "unflip" the data.
						if (fYFlipped)
						{
							yflip(
								(LPBYTE)pMap->map_memory,
								pMap->width,
								pMap->height,
								nByteWidth);
						}
						if (fXFlipped)
						{
							xflip(
								(LPBYTE)pMap->map_memory,
								pMap->width,
								pMap->height,
								nByteWidth);
						}
					}
				}
			}
			else
			{
				// We don't have a cache bitmap, so we're going to have to generate
				// the bitmap ourselves using the font engine and draw it onto the
				// destination DC. If the the bitmap is of reasonable size, it will
				// be added to the character cache. If the bitmap is large, it will
				// be broken into bands and each band will be drawn on a separate pass
				// through the main loop.

				// Get the clipping parameters.
				int nClipX0 = rc->clip_rect.left-1;
				int nClipX1 = rc->clip_rect.right+1;
				int nClipY0 = rc->clip_rect.top-1;
				int nClipY1 = rc->clip_rect.bottom+1;

				ASSERT(nClipX0 < nClipX1);
				ASSERT(nClipY0 < nClipY1);

				// As we proceed, we will be able to determine if the character should
				// be added to the cache. The "fCacheBitmap" flag will be set accordingly.
				// In general, the character will be cached if it's not clipped, fits in
				// one band, and the cache has enough room.
				BOOL fCacheBitmap = rc->IsScreen();

				// Check if the character is completely clipped.
				if ((nDrawY-nTop < nClipY1)
				 && (nDrawY+nBottom > nClipY0)
				 && (nDrawX-nLeft < nClipX1)
				 && (nDrawX+nRight > nClipX0))
				{
					// Character is not completely clipped. Check for partial
					// clipping and adjust the top and bottom rows accordingly.
					// If the character is partially clipped, then the character
					// bitmap is not cached.
      
					// Clip the bottom bound of the character.
					if (nDrawY+nBottom > nClipY1)
					{
						nBottom = nClipY1-nDrawY;
						fCacheBitmap = FALSE;
					}

					// Clip the top bound of the character.
					if (nDrawY-nTop < nClipY0)
					{
						nTop = nDrawY-nClipY0;
						fCacheBitmap = FALSE;
					}

					// Compute the total number of rows we will be drawing.
					int nYLen = nTop+nBottom;
					if (nYLen > 0)
					{
						MAP Map;

						// Set the starting row in the bitmap request structure.
						bitmap_parameters.start_row = fYFlipped ? -nBottom : -nTop;

						// Get the byte width of the bitmap. Since this bitmap came from
						// the TrueType font engine, this width will always be a multiple
						// of four bytes.
						int nMapOffset = bitmap_parameters.byte_width;

						// Set the width of the map.
						Map.width = nLeft+nRight;

						// Allocate the memory for the bitmap. In general, we try to allocate
						// as much memory as is reasonable.
						Map.map_memory = NULL;

						long lSize;
						int nYStrip;
				      for (int nNumberOfBands = 1; ; nNumberOfBands++)
						{
		               nYStrip = (nYLen+nNumberOfBands-1)/nNumberOfBands;
				         if ((lSize = (long)(nMapOffset) * (long)nYStrip) < MAX_BITMAP_SIZE)
                     {
		                  if (nYStrip == 0)
			               {
				               // Band cannot be allocated at all (> MAX_BITMAP_SIZE pixels wide?)
					            break;
						      }

								TRY
								{
									Map.map_memory = new char[(USHORT)lSize];
								}
								END_TRY

								if (Map.map_memory != NULL)
								{
									// Got some memory.
									break;
								}
		               }
				      }

			         if (Map.map_memory != NULL)
						{
							// Only cache bitmap if everything fits in one band.
							if (nNumberOfBands > 1)
							{
								fCacheBitmap = FALSE;
							}

							// Get partial bitmap.
							bitmap_parameters.output_buffer_address = Map.map_memory;
							bitmap_parameters.output_buffer_size = (USHORT)lSize;

							// Loop until all rows have been done.
							while ((nYLen != 0) && (bitmap_parameters.command != G_C_done))
							{
								// Set the number of rows to request for this bitmap.
								if (nYStrip > nYLen)
								{
									nYStrip = nYLen;
								}
								bitmap_parameters.row_count = nYStrip;

								// Build the band.
								if (pFontEngine->make_bitmap(&bitmap_parameters) < 0)
								{
									// Failed.
									break;
								}

								// Set the height of the map.
								Map.height = bitmap_parameters.bitmap_bbox.lry - bitmap_parameters.bitmap_bbox.uly + 1;

								// If the character is flipped, flip the bitmap data.
								if (fXFlipped)
								{
									xflip(
										(LPBYTE)Map.map_memory,
										Map.width,
										Map.height,
										nMapOffset);
								}
								if (fYFlipped)
								{
									yflip(
										(LPBYTE)Map.map_memory,
										Map.width,
										Map.height,
										nMapOffset);
								}

								// Draw the current band.
								DrawStrip(
									rc,
									&Map,
									nDrawX-nLeft,
									nDrawY+(fYFlipped ? -(bitmap_parameters.bitmap_bbox.lry+1) : bitmap_parameters.bitmap_bbox.uly),
									fMask,
									dwComponent == COMPONENT_WhiteOutMask);

								// If we are caching the bitmap, make sure it's not stored flipped.
								if (fCacheBitmap)
								{
									if (fXFlipped)
									{
										xflip(
											(LPBYTE)Map.map_memory,
											Map.width,
											Map.height,
											nMapOffset);
									}
									if (fYFlipped)
									{
										yflip(
											(LPBYTE)Map.map_memory,
											Map.width,
											Map.height,
											nMapOffset);
									}
								}

								// Decrement the number of rows left to do
								// by the number of rows we just did.
								nYLen -= nYStrip;
							}

							if (fCacheBitmap)
							{
								// The character bitmap is not in the bitmap cache at the moment.
								// Attempt to add the bitmap to the bitmap cache.
								TRY
								{
									// Build a description of the bitmap.
									CCharacterBitmapCache::CharacterBitmapCacheItem::CharacterBitmap Bitmap;
									Bitmap.m_Map = Map;
									Bitmap.m_crCharacter = crCharacter;
									Bitmap.m_OriginWarpOffset = OriginWarpOffset;

									// MAP no longer owns memory.
									Map.map_memory = NULL;

									// Create the new bitmap cache item.
									pBitmapItem = new CCharacterBitmapCache::CharacterBitmapCacheItem(Character, dwComponent, Bitmap);
									if (!pCharacterItem->m_CharacterBitmapCache.Insert(pBitmapItem))
									{
										// Could not add item to cache? Just delete it.
										delete pBitmapItem;
										pBitmapItem = NULL;
									}
								}
								END_TRY
							}
							else
							{
								// If we did not want to cache the bitmap, then free the bitmap memory.
								// Note that if we want to, but fail, then the above code deletes the memory.
								delete [] (char*)(Map.map_memory);
								Map.map_memory = NULL;
							}
						}
					}
				}
			}
		}
	}

	// Delete any temporary resources that were allocated.
	delete pOutlineInformation;
	pOutlineInformation = NULL;
}


/*
// Initialize the default style for this document.
*/

DB_RECORD_NUMBER PMGFontServer::GetDefaultStyle(void)
{
   ASSERT(database != NULL);
   if (m_lDefaultStyle == 0)
   {
      if (database != NULL)
      {
         // Try to lock the document so we can access it.
         DocumentRecord* pDocument = (DocumentRecord*)database->get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document);
         if (pDocument != NULL)
         {
            DWORD dwStyles = pDocument->NumberOfTextStyles();
            for (DWORD dwStyle = 0; dwStyle < dwStyles; dwStyle++)
            {
					// We need to get the record number of the default style.
               DB_RECORD_NUMBER lRecord = pDocument->GetTextStyle(dwStyle);
               if (lRecord != 0)
               {
                  // Verify that it is a paragraph style.
                  CTextStyleRecord* pRecord = (CTextStyleRecord*)database->get_record(lRecord, NULL, RECORD_TYPE_TextStyle);
                  ASSERT(pRecord != NULL);
                  if (pRecord != NULL)
                  {
                     // See if the record is the one we want.
                     if (pRecord->Type() == CTextStyleRecord::TEXT_STYLE_Paragraph
                           && pRecord->Flags() & CTextStyleRecord::FLAG_Default)
                     {
                        m_lDefaultStyle = lRecord;
                     }
                     pRecord->release();
                     // If we found it, stop.
                     if (m_lDefaultStyle != 0)
                     {
                        break;
                     }
                  }
               }
            }
            pDocument->release();

            // See if we found a default style.

            if (m_lDefaultStyle == 0)
            {
					// We need to create a default paragraph style.
               CTextStyle Style(database);

               // Everything is zero at this point.
               // Set all the non-zero ones.

               // Default the paragraph style.
               Style.Alignment(ALIGN_left);
               Style.LeadingType(LEADING_lines);
               Style.Leading(MakeFixed(1));

               // Default the character style.
               Style.m_Character.m_lFont =  database->allocate_face((LPSTR)(LPCSTR)m_csDefaultFace);
               CFixed Size = MakeFixed(m_nDefaultPointSize == 0 ? default_point_size : m_nDefaultPointSize);
               Style.m_Character.m_Size = Size;
               Style.m_Character.m_BaseSize = Size;
               Style.m_Character.m_Expansion = MakeFixed(1);

					Style.m_Character.m_OldColor = COLOR_BLACK;
					Style.m_Character.m_nOldFill = FillFormatV1::Black;
					Style.m_Character.m_nOldOutline = OutlineFormat::None;
					Style.m_Character.m_nOldShadow = ShadowFormat::None;

					Style.m_Character.m_nEffectsVersion = 1;

					Style.m_Character.m_fWhiteOut = FALSE;

					Style.Fill().SetType(FillFormatV1::FillSolid, COLOR_BLACK);
					Style.Outline().SetType(OutlineFormat::OutlineNone);
					Style.OutlineForegroundColor(TRANSPARENT_COLOR);
					Style.Shadow().SetType(ShadowFormat::ShadowNone);
					Style.ShadowForegroundColor(TRANSPARENT_COLOR);

               // Finally, set the font metrics.
               FontMetrics(&Style);

               // Create the text style record.
               m_lDefaultStyle = database->NewTextStyleRecord(GET_PMWAPP()->GetResourceStringPointer (IDS_NORMAL),
                                                              CTextStyleRecord::TEXT_STYLE_Paragraph,
                                                              &Style);
               // Get it and set some additional attributes.
               CTextStyleRecord* pRecord = (CTextStyleRecord*)database->get_record(m_lDefaultStyle, NULL, RECORD_TYPE_TextStyle);
               ASSERT(pRecord != NULL);
               if (pRecord != NULL)
               {
                  TRACE("Created default style '%s'\n", (LPCSTR)pRecord->Name());
                  ASSERT(pRecord->Type() == CTextStyleRecord::TEXT_STYLE_Paragraph);
                  pRecord->Flags(pRecord->Flags()
                                 | CTextStyleRecord::FLAG_Builtin
                                 | CTextStyleRecord::FLAG_Default);
                  pRecord->release();
               }
            }
         }
      }
   }

   ASSERT(m_lDefaultStyle != 0);

   return m_lDefaultStyle;
}

/*
// Set the default style for a font style structure.
*/

void PMGFontServer::SetDefault(CTextStyle* pStyle)
{
   ASSERT(database != NULL);
   if (database != NULL)
   {
      DB_RECORD_NUMBER lStyle = GetDefaultStyle();
      CTextStyleRecord* pRecord = (CTextStyleRecord*)database->get_record(lStyle, NULL, RECORD_TYPE_TextStyle);
      if (pRecord != NULL)
      {
         // Assign the style.
         *pStyle = pRecord->Style();

         // Release the record.
         pRecord->release();
      }
   }
}

////////////////////////////////////////////////////////////////////
// SubstituteFonts() 
// Performs font substitutions on the fonts in the database
// Looks at the fonts in the database and if the the font is in the "OriginalFont" list
// and not present on the system, it makes the substitution.
// These substitutions are preformed because the fonts in the "OriginalFont" list
// were distributed illegally.
//
// returns true if a substitution has been performed.
bool PMGFontServer::SubstituteFonts()
{
	bool fSubstituted = false;

	if (database != NULL)
	{
		// Try to lock the document so we can access it.
		DocumentRecord* pDocument = (DocumentRecord*)database->get_record(PMG_DOCUMENT_RECORD, NULL, RECORD_TYPE_Document);
		if( pDocument != NULL )
		{
			DWORD dwStyles = pDocument->NumberOfTextStyles();
			for( DWORD dwStyle = 0; dwStyle < dwStyles; dwStyle++ )
			{
				// We need to get the record number of the default style.
				DB_RECORD_NUMBER lRecord = pDocument->GetTextStyle(dwStyle);
				if( lRecord != 0 )
				{
					// Verify that it is a paragraph style. (need to set modified flag?)
					CTextStyleRecord* pStyleRecord = (CTextStyleRecord*)database->get_record(lRecord, NULL, RECORD_TYPE_TextStyle);
					ASSERT(pStyleRecord != NULL);
					if( pStyleRecord )
					{
						DB_RECORD_NUMBER subID = FindSubstitute( pStyleRecord->Style().Font() );
						if( subID > 0 )
						{
							pStyleRecord->Style().Font( subID );
							pStyleRecord->Style().UpdateFontMetrics();
							pStyleRecord->modified();
							fSubstituted = true;
						}
						pStyleRecord->release();
					}
            }
			}
			pDocument->release();
		}
	}
	return fSubstituted;
}


////////////////////////////////////////////////////////////
// FindSubstitute()
//
// Test to see if the font exists on the system. If it doesn't,
// looks up the font record number passed in the database to get the
// font name. Looks for the font name in the original font list.
//
// If it exists returns the database ID for the substitutefont from 
// in the substitute font list.
DB_RECORD_NUMBER PMGFontServer::FindSubstitute(DB_RECORD_NUMBER record)
{
   if (database == NULL || record <= 0)
   {
      return -1;
   }

	DB_RECORD_NUMBER fontID = -1;
	FontDataPtr pFontData;
	pFontData = (FontDataPtr)database->get_record(record, NULL, RECORD_TYPE_FontData);
	if( pFontData )
	{
		int index;
		// first check to see if this is a font that needs to be changed
		for( index = 0; index < NUMBER_OF_SUBSTITUTIONS; index++ )
		{
			if( !stricmp( pFontData->get_face_name(), OriginalFont[index] ) )
				break;
		}
		// if the font was found in the list find the substitution
		if( index < NUMBER_OF_SUBSTITUTIONS )
		{
			// is the font on the system?
			int faceID = typeface_server.find_face(pFontData->get_face_name(), FALSE);
			// if the font is not on the system, use the substitute
			if( faceID == -1 )
			{
				faceID = typeface_server.find_face(SubstituteFont[index], FALSE);
				if (faceID != -1)
				{
					TRACE("Found Substitution, %s for %s \n", SubstituteFont[index], OriginalFont[index]);
					fontID = font_face_to_record(faceID);
				}
			}
		}
		pFontData->release();
	}
   return fontID;
}

/*
// Fill out all relevant font metrics in the style structure.
*/

void PMGFontServer::FontMetrics(CTextStyle* pStyle)
{
	// Get the font item.
	CFontCache::FontCacheItem* pFontCacheItem = GetFontCacheItem(pStyle);
	if (pFontCacheItem != NULL)
	{
		pStyle->Ascend(-pFontCacheItem->m_Metrics.m_Bounds.y0);
		pStyle->Descend(pFontCacheItem->m_Metrics.m_Bounds.y1);
		pStyle->FontXMin(pFontCacheItem->m_Metrics.m_Bounds.x0);
		pStyle->FontXMax(pFontCacheItem->m_Metrics.m_Bounds.x1);
		pStyle->UnderlinePosition(pFontCacheItem->m_Metrics.m_lUnderlinePosition);
		pStyle->UnderlineThickness(pFontCacheItem->m_Metrics.m_lUnderlineThickness);
	}
}


/*
// Return the delta width for a character.
*/

PCOORD PMGFontServer::GetAdvanceWidth(CTextStyle* pStyle, CHARACTER Character, DB_RECORD_NUMBER lPage /*=0*/)
{
	PCOORD nAdvanceWidth = 0;

   if (Character >= MACRO_CHARACTER)
   {
      nAdvanceWidth = MacroWidth(pStyle, Character, lPage);
   }
	else
	{
		// Get the metrics cache item for the character.
		CCharacterMetricsCache::CharacterMetricsCacheItem* pItem = GetCharacterMetricsCacheItem(pStyle, Character);
//j		ASSERT(pItem != NULL);
		if (pItem != NULL)
		{
			// Get the advance width from the character metrics cache.
			nAdvanceWidth = pItem->m_Metrics.m_lDeltaX;

			// Adjust width by current spacing.
	      CFixed lSpacing = pStyle->Spacing();
	      if ((lSpacing != MakeFixed(0.0)) && (lSpacing != MakeFixed(1.0)))
		   {
				// Amount to add is 0.75 * (PtSize/72) * (Spacing-1.0) * Expansion
				CFixed lDelta = lSpacing-MakeFixed(1.0);
				lDelta = MulFixed(lDelta, MakeFixed(0.75));
				lDelta = MulDivFixed(lDelta, pStyle->Size(), MakeFixed(72));
				if (pStyle->Expansion() != MakeFixed(1.0))
				{
					lDelta = MulFixed(lDelta, pStyle->Expansion());
				}
				nAdvanceWidth += MulFixed(lDelta, PAGE_RESOLUTION);
			}
		}
	}

	return nAdvanceWidth;
}


/*
// Draw a string.
*/

CHARACTER_COUNT PMGFontServer::DrawString(
   RedisplayContextPtr rc,
   CHARACTER_PTR pCharacters,
   CHARACTER_COUNT nCharacters,
   CTextStyle* pStyle,
   PCOORD nX,
	PCOORD nY,
   BOOL fClearOut,
   PCOORD_PTR pNewX,
   const PBOX& Bounds,
   CTextDisplayList* pDisplayList)
{
   ASSERT(pDisplayList != NULL);

   // If the character is a macro character, call DrawMacro() to
   // do the drawing. DrawMacro() calls DrawString() (this function)
   // with the expanded macro text.
   if ((nCharacters == 1) && (*pCharacters >= MACRO_CHARACTER))
   {
      return DrawMacro(
         rc,
         pCharacters,
         pStyle,
         nX,
         nY,
         fClearOut,
         pNewX,
         Bounds,
         pDisplayList);
   }

	// Get the font cache item.
	CFontCache::FontCacheItem* pItem = GetFontCacheItem(pStyle);
	if (pItem == NULL)
	{
		return -1;
	}

	// Get the associated face structure from the typeface server.
	FaceEntry* pFace = typeface_server.get_face(typeface_index(pItem->m_Key.m_Description.m_nTypeface));
	if (pFace == NULL)
	{
		return -1;
	}

   // Figure out if we should just greek or not.
   BOOL fJustGreek = FALSE;
   PCOORD yp = scale_pcoord(
                   scale_pcoord(pStyle->Ascend()+pStyle->Descend(), rc->y_resolution, PAGE_RESOLUTION),
                   rc->GetScaleNumerator(),
                   rc->GetScaleDenominator());

   if ((yp >= 0) && (yp < (PCOORD)(pFace->GetMinimumPixelHeight())))
   {
      fJustGreek = TRUE;
   }

	// Create a display list element for the current output.
	pDisplayList->AddItem(nCharacters, pStyle, fJustGreek, Bounds);

   PCOORD nDeltaX;
   for (int nCharacter = 0; nCharacter < nCharacters; nCharacter++)
   {
		// Get the character.
      CHARACTER Character = pCharacters[nCharacter];

		// Get the width of the current character.
		nDeltaX = GetAdvanceWidth(pStyle, Character);

		// Add the current character to the current display list element.
		pDisplayList->AddCharacter(Character, nDeltaX, nX, nY);

		// Advance the X coordinate.
		nX += nDeltaX;
	}

	// Save the new X coordinate.
   if (pNewX != NULL)
   {
      *pNewX = nX;
   }

	// Return the number of characters.
   return nCharacters;
}

void PMGFontServer::GreekCharacter(
   RedisplayContextPtr rc,
   CHARACTER Character,
	CFontCache::FontCacheItem* pFontItem,
   PCOORD nDeltaX,
   PCOORD nX,
   PCOORD nY,
	COLOR Color,
   CFrameObject* pFrameObject)
{
   // We "greek" a character by drawing it as a box.
	// Compute the box to draw.
   PBOX Bounds;

	// Compute the left and right of the character box.
   if (nDeltaX >= 0)
   {
      Bounds.x0 = nX;
		Bounds.x1 = Bounds.x0+nDeltaX;
   }
   else
   {
      Bounds.x1 = nX;
		Bounds.x0 = Bounds.x1-nDeltaX;
   }

   // Compute the top and bottom of character box.
	Bounds.y0 = nY+pFontItem->m_Metrics.m_Bounds.y0;
	Bounds.y1 = nY+pFontItem->m_Metrics.m_Bounds.y1;

   // Draw the character box in a 50% shade of the text color.
   COLOR_COMPONENT cyan = CYAN_COMPONENT_OF(Color);
   COLOR_COMPONENT magenta = MAGENTA_COMPONENT_OF(Color);
   COLOR_COMPONENT yellow = YELLOW_COMPONENT_OF(Color);
   COLOR_COMPONENT black = BLACK_COMPONENT_OF(Color);

   if (black == 0)
   {
      cyan /= 2;
      magenta /= 2;
      yellow /= 2;
   }
   else
   {
      black /= 2;
   }

   // Draw the greeked character.
	DrawRectangle(rc, Bounds, colorref_from_color(MAKE_COLOR(cyan, magenta, yellow, black)), pFrameObject);
}

// Draw a rectangle in a given color. The retangle specified is in the "normalized" coordinate
// system of the frame object. Any warping, flipping, and rotation is applied by this function.
void PMGFontServer::DrawRectangle(
	RedisplayContextPtr rc,
	const PBOX& Bound,
	COLORREF clColor,
	CFrameObject* pFrameObject,
	BOOL fToggle /*=FALSE*/)
{
	ASSERT(pFrameObject != NULL);

	// Only toggle if we're drawing to the screen.
	if (!rc->IsScreen())
	{
		fToggle = FALSE;
	}

   // Convert to screen coordinates.
	PBOX ScreenBound = Bound;
   rc->pbox_to_screen(&ScreenBound, TRUE);

   // Build an outline path for the box. If the normalized box has collapsed
	// to a single pixel, then draw the box using a line.
	COutlinePath Path;

	if (ScreenBound.x1-ScreenBound.x0 <= 1)
	{
		// Build the path for this vertical line.
		Path.UsePolyline(TRUE);	// force line draw
		Path.MoveTo(MakeFixed(ScreenBound.x0), MakeFixed(ScreenBound.y0));
		Path.LineTo(MakeFixed(ScreenBound.x0), MakeFixed(ScreenBound.y1));
	}
	else if (ScreenBound.y1-ScreenBound.y0 <= 1)
	{
		// Build the path for this horizontal line.
		Path.UsePolyline(TRUE);	// force line draw
		Path.MoveTo(MakeFixed(ScreenBound.x0), MakeFixed(ScreenBound.y0));
		Path.LineTo(MakeFixed(ScreenBound.x1), MakeFixed(ScreenBound.y0));
	}
	else
	{
		// Build the full path.
		Path.MoveTo(MakeFixed(ScreenBound.x0), MakeFixed(ScreenBound.y0));
		Path.LineTo(MakeFixed(ScreenBound.x1), MakeFixed(ScreenBound.y0));
		Path.LineTo(MakeFixed(ScreenBound.x1), MakeFixed(ScreenBound.y1));
		Path.LineTo(MakeFixed(ScreenBound.x0), MakeFixed(ScreenBound.y1));
		Path.LineTo(MakeFixed(ScreenBound.x0), MakeFixed(ScreenBound.y0));
	}

	Path.Close();
	Path.End();

   // Check for warping, flipping and rotation.
   ANGLE dRotation = pFrameObject->get_rotation();
   CWarpField* pWarpField = pFrameObject->GetWarpField();
   BOOL fWarped = pWarpField != NULL;
   BOOL fRotated = dRotation != 0.0;
   BOOL fXFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_xflipped) != 0;
   BOOL fYFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_yflipped) != 0;

	if (fXFlipped || fYFlipped || fWarped || fRotated)
	{
		// Compute the unrotated screen bounds for the frame object.
		// We need this for warping and flipping.
		PBOX UnrotatedBound = pFrameObject->get_unrotated_bound();
		rc->pbox_to_screen(&UnrotatedBound, TRUE);
		CRect crFrameObject;
		rc->convert_pbox(&UnrotatedBound, crFrameObject, NULL);

		// If there is a warp field, then warp the path data.
		if (pWarpField != NULL)
		{
			// Make sure the warp field is set up for warping device coordinates.
			pWarpField->Transform(crFrameObject);

			// Warp the path.
			Path.Warp(pWarpField);
		}

		// Apply any flipping and rotation.
		if (fXFlipped || fYFlipped || fRotated)
		{
			// Create a transformation matrix that accounts for any flipping and rotation.
			COutlinePathMatrix RotateMatrix;
			RotateMatrix.Identity();

			if (fXFlipped)
			{
				RotateMatrix.XFlip(MakeFixed(crFrameObject.left), MakeFixed(crFrameObject.right));
			}
			if (fYFlipped)
			{
				RotateMatrix.YFlip(MakeFixed(crFrameObject.top), MakeFixed(crFrameObject.bottom));
			}
			if (fRotated)
			{
				int px = rc->page_x_to_screen(pFrameObject->get_origin().x);
				int py = rc->page_y_to_screen(pFrameObject->get_origin().y);
				RotateMatrix.TranslateBy(-MakeFixed(px), -MakeFixed(py));
				RotateMatrix.RotateBy(MakeFixed(dRotation*RAD2DEGC));
				RotateMatrix.TranslateBy(MakeFixed(px), MakeFixed(py));
			}

			Path.Transform(&RotateMatrix);
		}
	}

	// Turn off any toggles where we are going to draw.
	CRect ToggleRect;

	if (fToggle)
	{
		CFixedRect FixedBounds;
		Path.QuickBoundingBox(&FixedBounds);

		ToggleRect.left = FixedInteger(FixedBounds.Left),
		ToggleRect.top = FixedInteger(FixedBounds.Top),
		ToggleRect.right = FixedInteger(FixedBounds.Right)+1;
		ToggleRect.bottom = FixedInteger(FixedBounds.Bottom)+1;

		rc->toggle(FALSE, ToggleRect);
	}

   // Draw the rectangle.
	Path.Draw(
		CDC::FromHandle(rc->destination_hdc),
      clColor,
		!Path.UsePolyline(),
		0,
      clColor,
		Path.UsePolyline(),
		ALTERNATE,
		rc->info.type == RASTER_TYPE_PALETTE);

	// Turn any toggles back on.
	if (fToggle)
	{
		rc->toggle(FALSE, ToggleRect);
	}
}


// Return the outline path for a character.
// The returned path is in the coordinates of the device described
// by the redisplay context (rc).

// Note: the character must have been remapped before calling this function.

BOOL PMGFontServer::CharacterOutline(
	RedisplayContextPtr rc,
	CHARACTER Character,
	const CFontDescription* pFontDescription,
	CFixedRect* pBounds,
	COutlinePath* pPath)
{
   BOOL Result = FALSE;

	ASSERT(pFontDescription != NULL);
	ASSERT(pPath != NULL);
   
	// Build a structure that will describe the font to the font engine.
	G_FONT_DESCRIPTION Font;

	Font.typeface					= pFontDescription->m_nTypeface;
	Font.Fill						= pFontDescription->m_Fill;
	Font.Outline					= pFontDescription->m_Outline;
	Font.Shadow						= pFontDescription->m_Shadow;
	Font.point_size				= pFontDescription->m_lPointSize;
	Font.vertical_expansion		= MakeFixed(1);
	Font.horizontal_expansion  = pFontDescription->m_lHorizontalExpansion;
	Font.oblique					= MakeFixed(0);
	Font.x_resolution				= rc->GetScaledXResolution();
	Font.y_resolution				= rc->GetScaledYResolution();
	Font.effects_rotation		= MakeFixed(0);

   // Get the outline information and turn it into a path.
   CFontCharacterOutlineInformation* pOutlineInformation = new CFontCharacterOutlineInformation;
   if (pOutlineInformation != NULL)
   {
      if (pFontEngine->character_outline(Character, &Font, pOutlineInformation) >= 0)
      {
			// Save the bounding box.
			if (pBounds != NULL)
			{
				pBounds->Left = MakeFixed(Font.character_bbox.ulx);
				pBounds->Top = MakeFixed(Font.character_bbox.uly);
				pBounds->Right = MakeFixed(Font.character_bbox.lrx);
				pBounds->Bottom = MakeFixed(Font.character_bbox.lry);
			}

         Result = pOutlineInformation->MakePath(pPath);
      }
      
      delete pOutlineInformation;
      pOutlineInformation = NULL;
   }
   
   return Result;
}

CFontCache::FontCacheItem* PMGFontServer::GetFontCacheItem(CTextStyle* pStyle)
{
	CFontCache::FontCacheItem* pFontCacheItem = NULL;

	// Make sure the parameters are reasonable.
	ASSERT(pStyle != NULL);
	if (pStyle != NULL)
	{
		// Get the description of the font reference by the style.
		CFontDescription FontDescription;
		if (pStyle->GetFontDescription(&FontDescription))
		{
			// Get the font item.
			pFontCacheItem = GetFontCacheItem(&FontDescription);
		}
	}

	return pFontCacheItem;
}

CFontCache::FontCacheItem* PMGFontServer::GetFontCacheItem(CFontDescription* pFontDescription)
{
	CFontCache::FontCacheItem* pFontCacheItem = NULL;

	// Make sure the parameters are reasonable.
	ASSERT(pFontDescription != NULL);
	if (pFontDescription != NULL)
	{
		// Create a font item key from the font description.
		CFontCache::FontCacheItem::FontCacheKey Key(*pFontDescription);

		// Look up the font item by its key.
		pFontCacheItem = (CFontCache::FontCacheItem*)(pFontCache->Lookup(&Key));
		if (pFontCacheItem == NULL)
		{
			// The font cache does not contain an item for the specified font.
			// In this case we need to create a new font cache item for the font
			// and  add it to the font cache.

			// Get the typeface list entry for the requested face.
			FaceEntry* pFace = typeface_server.face_list.get_face(typeface_index(Key.m_Description.m_nTypeface));
			ASSERT(pFace != NULL);
			if (pFace != NULL)
			{
				// Build a description structure for the font which can be used
				// to communicate with the font engine. Attributes which are
				// assigned fixed numbers cannot effect the font metrics.
				G_FONT_DESCRIPTION Font;
				Font.typeface					= Key.m_Description.m_nTypeface;
				Font.Fill						= pFontDescription->m_Fill;
				Font.Outline					= pFontDescription->m_Outline;
				Font.Shadow						= pFontDescription->m_Shadow;
				Font.point_size				= Key.m_Description.m_lPointSize;
				Font.vertical_expansion		= MakeFixed(1);
				Font.horizontal_expansion	= Key.m_Description.m_lHorizontalExpansion;
				Font.oblique					= MakeFixed(0);
				Font.effects_rotation		= MakeFixed(0);

				// Get font metrics at EAE_RESOLUTION and scale them up manually.
				// this will prevent fonts with nonlinear EAEs from overflowing
				// at lower resolutions.

				Font.x_resolution = EAE_RESOLUTION;
				Font.y_resolution = EAE_RESOLUTION;

				// If this is the first time we have used the current face, then compute the
				// extended field information for the face. This information is basically
				// typeface specific and belongs in the face structure, but it is not
				// something the typeface server can easily compute. Also, by deferring the
				// work we can do it "on demand" and thus avoid extra work for typefaces which
				// are not being used.

				if (!pFace->GetIsExtendedInformationValid())
				{
					// Compute the extended information and save it back into the face list entry.
					CFontFaceInformation FaceInfo;
					if (pFontEngine->face_information(Font.typeface, (CFontFaceInformation*)&FaceInfo) >= 0)
					{
						pFace->SetMinimumPixelHeight(FaceInfo.get_min_pixel_height());
						pFace->SetIsTextFont((BOOL)FaceInfo.is_text_font);
						pFace->SetIsExtendedInformationValid();
					}
				}

				int nXAdjust = 0;
				int nYAdjust = 0;
				AdjustFontSize(Font, nXAdjust, nYAdjust);

				// Get the font metrics. We need these to create the font cache entry.
			   int nResult = pFontEngine->font_metrics(&Font);

				RestoreFontSize(Font, nXAdjust, nYAdjust);

				if (nResult == G_R_ok)
				{
					// Round font BBOX outward.
			      Font.font_bbox.ulx--;
			      Font.font_bbox.uly--;
			      Font.font_bbox.lrx++;
			      Font.font_bbox.lry++;

					// Scale from EAE_RESOLUTION (72 dpi) back to PAGE_RESOLUTION (1800 dpi)
					// WARNING: These numbers are hardcoded for speed.
					Font.font_space_width *= (1800/72);
					Font.font_underline_position *= (1800/72);
					Font.font_underline_thickness *= (1800/72);
					Font.font_bbox.ulx *= (1800/72);
					Font.font_bbox.uly *= (1800/72);
					Font.font_bbox.lrx *= (1800/72);
					Font.font_bbox.lry *= (1800/72);

					// Build metrics structure.
					CFontCache::FontCacheItem::FontMetrics Metrics;
					Metrics.m_lSpaceWidth = Font.font_space_width;
					Metrics.m_lUnderlinePosition = Font.font_underline_position;
					Metrics.m_lUnderlineThickness = Font.font_underline_thickness;
					Metrics.m_Bounds.x0 = Font.font_bbox.ulx;
					Metrics.m_Bounds.y0 = Font.font_bbox.uly;
					Metrics.m_Bounds.x1 = Font.font_bbox.lrx;
					Metrics.m_Bounds.y1 = Font.font_bbox.lry;

					// Create font cache item.
					TRY
					{
						pFontCacheItem = new CFontCache::FontCacheItem(Key.m_Description, Metrics);
						if (!pFontCache->Insert(pFontCacheItem))
						{
							// Could not add item to cache? Just delete it.
							delete pFontCacheItem;
							pFontCacheItem = NULL;
						}
					}
					END_TRY
				}
			}
		}
	}

	return pFontCacheItem;
}

CCharacterMetricsCache::CharacterMetricsCacheItem* PMGFontServer::GetCharacterMetricsCacheItem(CTextStyle* pStyle, CHARACTER Character)
{
	CCharacterMetricsCache::CharacterMetricsCacheItem* pCharacterMetricsCacheItem = NULL;

	// Make sure the input parameters are reasonable.
	ASSERT(Character < MACRO_CHARACTER);
	ASSERT(pStyle != NULL);
	if (pStyle != NULL)
	{
		// Get the description of the font reference by the style.
		CFontDescription FontDescription;
		if (pStyle->GetFontDescription(&FontDescription))
		{
			// Get the character metrics.
			pCharacterMetricsCacheItem = GetCharacterMetricsCacheItem(&FontDescription, Character);
		}
	}

	return pCharacterMetricsCacheItem;
}

CCharacterMetricsCache::CharacterMetricsCacheItem* PMGFontServer::GetCharacterMetricsCacheItem(CFontDescription* pFontDescription, CHARACTER Character)
{
	CCharacterMetricsCache::CharacterMetricsCacheItem* pCharacterMetricsCacheItem = NULL;

	// Make sure the input parameters are reasonable.
	ASSERT(Character < MACRO_CHARACTER);
	ASSERT(pFontDescription != NULL);

	// Map PM character index to a UGL code for TrueType.
	FaceEntry* pFace = typeface_server.get_face(typeface_index(pFontDescription->m_nTypeface));
   Character = character_map(Character, pFace);

	// Get the font item.
	CFontCache::FontCacheItem* pFontCacheItem = GetFontCacheItem(pFontDescription);
	if (pFontCacheItem != NULL)
	{
		// Create a key for the character metrics.
		CCharacterMetricsCache::CharacterMetricsCacheItem::CharacterMetricsCacheKey Key(Character);

		// Look up the character metrics by its key.
		pCharacterMetricsCacheItem = (CCharacterMetricsCache::CharacterMetricsCacheItem*)(pFontCacheItem->m_CharacterMetricsCache.Lookup(&Key));
		if (pCharacterMetricsCacheItem == NULL)
		{
			// The character metrics cache does not contain an item for the specified
			// character. In this case we need to create a new character metrics cache
			// item for the character and  add it to the character metrics cache.

			// Get a pointer to the font cache item key. It will be needed to build
			// the structure used to communicate with the font engine.
			CFontCache::FontCacheItem::FontCacheKey* pFontKey = (CFontCache::FontCacheItem::FontCacheKey*)(pFontCacheItem->GetKey());
			ASSERT(pFontKey != NULL);
			if (pFontKey != NULL)
			{
				// Build a description structure for the font which can be used
				// to communicate with the font engine.
				G_FONT_DESCRIPTION Font;
				Font.typeface					= pFontKey->m_Description.m_nTypeface;
				Font.Fill						= pFontDescription->m_Fill;
				Font.Outline					= pFontDescription->m_Outline;
				Font.Shadow						= pFontDescription->m_Shadow;
				Font.point_size				= pFontKey->m_Description.m_lPointSize;
				Font.vertical_expansion		= MakeFixed(1);
				Font.horizontal_expansion	= pFontKey->m_Description.m_lHorizontalExpansion;
				Font.oblique					= MakeFixed(0);
				Font.effects_rotation		= MakeFixed(0);
				Font.x_resolution				= PAGE_RESOLUTION;
				Font.y_resolution				= PAGE_RESOLUTION;

				int nXAdjust = 0;
				int nYAdjust = 0;
				AdjustFontSize(Font, nXAdjust, nYAdjust);

			   // Call the font engine to get the character metrics.
				int nResult = pFontEngine->character_metrics(Character, &Font);

				RestoreFontSize(Font, nXAdjust, nYAdjust);

				CCharacterMetricsCache::CharacterMetricsCacheItem::CharacterMetrics Metrics;

				if (nResult == G_R_ok)
				{
					// Build metrics structure.
					Metrics.m_lDeltaX = Font.character_delta_x;
				}
				else
				{
					// Use the font space width.
					Metrics.m_lDeltaX = pFontCacheItem->m_Metrics.m_lSpaceWidth;
				}

				// Create character metrics cache item.
				TRY
				{
					pCharacterMetricsCacheItem = new CCharacterMetricsCache::CharacterMetricsCacheItem(Character, Metrics);
					if (!pFontCacheItem->m_CharacterMetricsCache.Insert(pCharacterMetricsCacheItem))
					{
						// Could not add item to cache? Just delete it.
						delete pCharacterMetricsCacheItem;
						pCharacterMetricsCacheItem = NULL;
					}
				}
				END_TRY
			}
		}
	}

	return pCharacterMetricsCacheItem;
}

/////////////////////////////////////////////////////////////////////////////
// Macro support in new style routines.

PCOORD PMGFontServer::MacroWidth(CTextStyle* pStyle, CHARACTER Character, DB_RECORD_NUMBER lPage /*=0*/)
{
	CHARACTER* pText = NULL;

	CMacroServer* pMacroServer = database->GetMacroServer();
	ASSERT(pMacroServer != NULL);

   PCOORD Size = 0;
	if (pMacroServer != NULL)
	{
		pText = pMacroServer->GetMacroText(Character, lPage);
		if (pText != NULL)
		{
			while (*pText != 0)
			{
				Size += GetAdvanceWidth(pStyle, *(pText++));
			}
		}
	}
   return Size;
}

CHARACTER_COUNT PMGFontServer::DrawMacro(
	RedisplayContextPtr rc,
	CHARACTER_PTR pCharacters,
	CTextStyle* pStyle,
	PCOORD nX,
	PCOORD nY,
	BOOL fClearOut,
	PCOORD_PTR pNewX,
	const PBOX& Bounds,
	CTextDisplayList* pDisplayList)
{
   PCOORD Size = 0;

   *pNewX = nX;

	// Get the macro server so we can get the macro text.
   CMacroServer* pMacroServer = database->GetMacroServer();
   ASSERT(pMacroServer != NULL);
   if (pMacroServer != NULL)
   {
		// Get the macro text.
      CHARACTER* pText = pMacroServer->GetMacroText(*pCharacters);
      if (pText != NULL)
      {
			// Count the number of characters.
         CHARACTER_COUNT nCharacters = 0;
         for (CHARACTER* p = pText; *p != 0; p++)
         {
            nCharacters++;
         }

			// Use DrawString() to draw the macro text.
         if (nCharacters != 0)
         {
            PCOORD nNewX;
            CHARACTER_COUNT lRet = DrawString(
                                       rc,
                                       pText,
                                       nCharacters,
                                       pStyle,
                                       nX,
                                       nY,
                                       fClearOut,
                                       &nNewX,
                                       Bounds,
                                       pDisplayList);
            if (lRet < 0)
            {
					// Error.
               return lRet;
            }
            else if (lRet < nCharacters)
            {
					// Interrupted or something.
               return 0;
            }

            *pNewX = nNewX;
         }

			// Only one character was requested for drawing.
         return 1;
      }
   }

	// Error!
   return -1;
}


// Draw the contents of a display list.

// Special components used locally.
enum
{
	COMPONENT_Greek		=	1000,
	COMPONENT_Underline	=	1001
};

static DWORD DrawComponents[] = 
{
	COMPONENT_WhiteOutMask,
	COMPONENT_ShadowMask,
	COMPONENT_Shadow,
	COMPONENT_FillMask,
	COMPONENT_Fill,
	COMPONENT_OutlineMask,
	COMPONENT_Outline,
	COMPONENT_Greek,
	COMPONENT_Underline
};

#define NUMBER_OF_COMPONENTS	(sizeof(DrawComponents)/sizeof(DrawComponents[0]))

BOOL PMGFontServer::DrawDisplayList(CFrameObject* pFrameObject, RedisplayContextPtr rc, CTextDisplayList* pDisplayList)
{
//    DWORD dwOffscreenTimeLimit = GetTickCount() + 500; // 500 millisec limit on offscreen draw

	if (pDisplayList->GetCount() == 0)
	{
		return TRUE;
	}

	// Set the starting state. If we were interrupted, just use the saved values.
	if (!pDisplayList->m_fInterrupted)
	{
		// Not interrupted, start at the beginning.
		pDisplayList->m_nComponent = 0;
		pDisplayList->m_pItem = (CTextDisplayListItem*)(pDisplayList->GetFirst());
		pDisplayList->m_nCharacter = 0;
	}
	else
	{
		pDisplayList->m_fInterrupted = FALSE;
	}

   // Unpack information global to all items in the display list.
   BOOL fClearOut = pDisplayList->IsClearOut();

   // Check for warping, flipping and rotation.
   ANGLE dRotation = pFrameObject->get_rotation();
   CWarpField* pWarpField = pFrameObject->GetWarpField();
   BOOL fWarped = pWarpField != NULL;
   BOOL fRotated = dRotation != 0.0;
   BOOL fXFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_xflipped) != 0;
   BOOL fYFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_yflipped) != 0;

#if 0
   // PMGTODO: Right now, we don't do offscreen stuff for warped and
   // rotated text. This is wrong since we must always go to a bitmap
   // if we are going to cache frame object images.
   if (rc->IsScreen() && !fWarped && !fRotated)
   {
      setup_offscreen_map(crScreenBounds);
   }
   else
   {
      mapOffscreen.map_memory = NULL;
   }
#endif

   // Create a clipping region for the refresh area.
	BOOL fRestore = pFrameObject->ApplyClipRgn(rc);
	if (fRestore != -1)
	{
		// Create DC's that will be used during the drawing.
		ASSERT((source_hdc == NULL) && (temp_hdc == NULL));
		if (((source_hdc = ::CreateCompatibleDC(rc->AttributeDC())) != NULL)
		 && ((temp_hdc = ::CreateCompatibleDC(rc->AttributeDC())) != NULL))
		{
			// Save the state of the current destination DC.
			::SaveDC(rc->destination_hdc);

			// Draw each component layer separately.
			for (int nComponent = pDisplayList->m_nComponent; nComponent < NUMBER_OF_COMPONENTS; nComponent++)
			{
				DWORD dwComponent = DrawComponents[nComponent];

				// Loop through each item and draw it.
				CTextDisplayListItem* pItem = pDisplayList->m_pItem;
				while (pItem != NULL)
				{
					// Get character formatting.
					CCharacterFormatting* pFormat = pItem->GetCharacterFormatting();
					CFontDescription* pFontDescription = pItem->GetFontDescription();

					// Check if there is a reason to ignore this item.
					BOOL fIgnoreItem = FALSE;

#if 0
					if (dwComponent == COMPONENT_WhiteOutMask)
					{
						TRACE(pFormat->DrawWhiteOut() ? "Draw White Out\n" : "Don't draw White Out\n");
					}
					if (dwComponent == COMPONENT_FillMask)
					{
						TRACE(pFormat->DrawFillMask() ? "Draw Fill Mask\n" : "Don't draw Fill Mask\n");
					}
					if (dwComponent == COMPONENT_OutlineMask)
					{
						TRACE(pFormat->DrawOutlineMask() ? "Draw Outline Mask\n" : "Don't draw Outline Mask\n");
					}
					if (dwComponent == COMPONENT_ShadowMask)
					{
						TRACE(pFormat->DrawShadowMask() ? "Draw Shadow Mask\n" : "Don't draw Shadow Mask\n");
					}
					if (dwComponent == COMPONENT_Fill)
					{
						TRACE(pFormat->DrawFill() ? "Draw Fill\n" : "Don't draw Fill\n");
					}
					if (dwComponent == COMPONENT_Outline)
					{
						TRACE(pFormat->DrawOutline() ? "Draw Outline\n" : "Don't draw Outline\n");
					}
					if (dwComponent == COMPONENT_Shadow)
					{
						TRACE(pFormat->DrawShadow() ? "Draw Shadow\n" : "Don't draw Shadow\n");
					}
#endif

					if (((dwComponent == COMPONENT_WhiteOutMask) && (!pFormat->DrawWhiteOut()))
					 || ((dwComponent == COMPONENT_Fill) && (!pFormat->DrawFill()))
					 || ((dwComponent == COMPONENT_FillMask) && (!pFormat->DrawFillMask()))
					 || ((dwComponent == COMPONENT_Outline) && (!pFormat->DrawOutline()))
					 || ((dwComponent == COMPONENT_OutlineMask) && (!pFormat->DrawOutlineMask()))
					 || ((dwComponent == COMPONENT_Shadow) && (!pFormat->DrawShadow()))
					 || ((dwComponent == COMPONENT_ShadowMask) && (!pFormat->DrawShadowMask()))
					 || ((pItem->IsGreek()) != (dwComponent == COMPONENT_Greek))
					 || ((pItem->CharacterCount() == 0) != (dwComponent == COMPONENT_Underline)))
					{
						// Don't draw anything for this item component.
						fIgnoreItem = TRUE;
					}

					// Get font cache item associated with the font.
					CFontCache::FontCacheItem* pFontItem = (CFontCache::FontCacheItem*)GetFontCacheItem(pFontDescription);
					ASSERT(pFontItem != NULL);
					if (pFontItem == NULL)
					{
						// We don't have a font!
						fIgnoreItem = TRUE;
					}

					if (!fIgnoreItem)
					{
						// Set up DC for drawing
						COLORREF colorOld, colorOldBk;
						HBRUSH hBrush = NULL;
						HBRUSH hBrushOld = NULL;

						if (dwComponent == COMPONENT_Fill)
						{
							text_color = pFormat->m_Fill.m_ForegroundColor;
						}
						else if (dwComponent == COMPONENT_Outline)
						{
							text_color = pFormat->m_Outline.m_ForegroundColor;
						}
						else if (dwComponent == COMPONENT_Shadow)
						{
							text_color = pFormat->m_Shadow.m_ForegroundColor;
						}
						else if ((dwComponent == COMPONENT_WhiteOutMask)
								|| (dwComponent == COMPONENT_ShadowMask)
								|| (dwComponent == COMPONENT_OutlineMask)
								|| (dwComponent == COMPONENT_FillMask))
						{
							text_color = COLOR_WHITE;
						}
						else
						{
							text_color = COLOR_BLACK;
						}

						text_color = colorref_from_color(text_color);

						colorOld = SetTextColor(rc->destination_hdc, RGB(0,0,0));
						colorOldBk = SetBkColor(rc->destination_hdc, RGB(255,255,255));

						// Check for colored text.
						if (rc->is_postscript)
						{
							// Set text color for a PostScript device
							SetTextColor(rc->destination_hdc, text_color);
						}
						else
						{
							if (text_color != RGB(0,0,0))
							{
								if ((rc->info.type != RASTER_TYPE_PALETTE)
								 || ((hBrush = create_dib_brush(text_color)) == NULL))
								{
									hBrush = ::CreateSolidBrush(text_color);
								}
								if (hBrush != NULL)
								{
									hBrushOld = (HBRUSH)::SelectObject(rc->destination_hdc, hBrush);
								}
							}
						}

						// Check if the item has characters. If not, then just draw a rectangle.
						if (pItem->CharacterCount() == 0)
						{
							ASSERT(dwComponent == COMPONENT_Underline);
							PBOX Bound = pItem->GetBounds();
							DrawRectangle(rc, Bound, colorref_from_color(pFormat->m_Fill.m_ForegroundColor), pFrameObject, TRUE);
						}
						else
						{
							// Hide toggle selects and mouse cursor if necessary.
							PBOX ToggleBounds = pItem->GetBounds();
							rc->pbox_to_screen(&ToggleBounds, TRUE);
							CRect ToggleRect;
							rc->convert_pbox(&ToggleBounds, ToggleRect, NULL);

							if (rc->IsScreen())
							{
								rc->toggle(FALSE, ToggleRect);
							}

							// Draw each of the characters.
							for (int nChar = (int)(pDisplayList->m_nCharacter); nChar < pItem->CharacterCount(); nChar++)
							{
								CTextDisplayListItem::CharacterInfo* pCharacterInfo = pItem->GetCharacter(nChar);
								CHARACTER Character = pCharacterInfo->Character;

								PCOORD nX = pCharacterInfo->nX;
								PCOORD nY = pCharacterInfo->nY;

								// If we want to greek the character, do it now.
								if (pItem->IsGreek())
								{
									GreekCharacter(
										rc,
										Character,
										pFontItem,
										pCharacterInfo->nDeltaX,
										nX,
										nY,
										pFormat->m_Fill.m_ForegroundColor,
										pFrameObject);
								}
								else
								{
									DrawCharacter(
										rc,
										Character,
										pItem->GetCharacterFormatting(),
										pFontItem,
										nX,
										nY,
										dwComponent,
										pFrameObject);
								}

								// Let the user interrupt us.
								if (rc->check_interrupt())
								{
									// Set "interrupted" flag. This will cause all loops to remember
									// their state and then exit.
									pDisplayList->m_fInterrupted = TRUE;

									// Save the place we want to start the next time through.
									pDisplayList->m_nCharacter = nChar+1;

									break;
								}

#if 0
								if (offscreen_map_available())
								{
									if (fClearOut)
									{
										draw_character_whiteout(rc, &mapOffscreen, crScreenBounds.left, crScreenBounds.top);
									}
									else
									{
										draw_character_bitmap(rc, &mapOffscreen, crScreenBounds.left, crScreenBounds.top);
									}
									release_offscreen_map();
								}
#endif
							} // end of character loop

							// Clean up after drawing characters in current item.

							// Hide toggle selects and mouse cursor if necessary.
							if (rc->IsScreen())
							{
								rc->toggle(TRUE, ToggleRect);
							}
						}

						// restore DC state
						SetTextColor(rc->destination_hdc, colorOld);
						SetBkColor(rc->destination_hdc, colorOldBk);
						if (hBrushOld != NULL)
						{
							::SelectObject(rc->destination_hdc, hBrushOld);
						}
						if ((hBrush != NULL)
						 && (hBrush != m_dib_brush))
						{
							::DeleteObject(hBrush);
						}

						// Check if we were interrupted.
						if (pDisplayList->m_fInterrupted)
						{
							// Save were we want to restart.
							pDisplayList->m_pItem = pItem;
							break;
						}
					}

					// Move to the next item.
					pItem = (CTextDisplayListItem*)(pItem->GetNext());

					// Reset first character number back to start of next item.
					pDisplayList->m_nCharacter = 0;

				} // end of item loop

				// Check if we were interrupted.
				if (pDisplayList->m_fInterrupted)
				{
					// Save were we want to restart.
					pDisplayList->m_nComponent = nComponent;
					break;
				}

				// Move back to first item for next component.
				pDisplayList->m_pItem = (CTextDisplayListItem*)(pDisplayList->GetFirst());

			} // end of component loop

			// Restore the destination DC back to its original state.
			::RestoreDC(rc->destination_hdc, -1);
		}

		// Delete any DC's we created and restore the previous DC's.
		if (source_hdc != NULL)
		{
			::DeleteDC(source_hdc);
			source_hdc = NULL;
		}
		if (temp_hdc != NULL)
		{
			::DeleteDC(temp_hdc);
			temp_hdc = NULL;
		}
	}

	// Restore the previous clipping region.
	pFrameObject->RestoreClipRgn(rc, fRestore);

   return !pDisplayList->m_fInterrupted;
}


/////////////////////////////////////////////////////////////
// CTextDisplayList
// CTextDisplayListItem
//
//    These classes are used to hold a display list of text
//    to draw for a given refresh extent. It is necessary
//    to know *all* the text which is to be displayed before
//    actually drawing so that the various layers (shadow,
//    fill, outline) can be drawn in the proper order. So,
//    a display list is built and then all the text is drawn
//    all at once.
//
//    The display list is composed of elements (CTextDisplayListItem)
//    which represent collections of characters with the same style.
//    Conveniently, these collections map precisely to calls to
//    the DrawString function.
//
//    The text display list is implemented as a doubly linked list.


/////////////////////////////////////////////////////////////
// CTextDisplayList

CTextDisplayList::CTextDisplayList(BOOL fClearOut)
{
   m_fClearOut = fClearOut;

	m_nComponent = -1;
	m_pItem = NULL;
	m_nCharacter = -1;
	m_fInterrupted = FALSE;
}

CTextDisplayList::~CTextDisplayList()
{
}

// Add an item to the end of the display list.
void CTextDisplayList::AddItem(CHARACTER_COUNT nCharacters, CTextStyle* pStyle, BOOL fGreek, const PBOX& Bounds)
{
   CTextDisplayListItem* pItem = NULL;
   TRY
   {
      pItem = new CTextDisplayListItem(nCharacters, pStyle, fGreek, Bounds);
      InsertAtEnd(pItem);
   }
   END_TRY
}

// Add an item to the end of the display list.
void CTextDisplayList::AddItem(CTextStyle* pStyle, const PBOX& Bounds)
{
   CTextDisplayListItem* pItem = NULL;
   TRY
   {
      pItem = new CTextDisplayListItem(pStyle, Bounds);
      InsertAtEnd(pItem);
   }
   END_TRY
}

// Add a character to the current (last) display list item.
void CTextDisplayList::AddCharacter(CHARACTER Character, PCOORD nDeltaX, PCOORD nX, PCOORD nY)
{
   CTextDisplayListItem* pItem = (CTextDisplayListItem*)GetLast();
   ASSERT(pItem != NULL);
   pItem->AddCharacter(Character, nDeltaX, nX, nY);
}

#ifdef _DEBUG
void CTextDisplayList::Dump(void)
{
   TRACE("-------------\n");
   CTextDisplayListItem* pItem = (CTextDisplayListItem*)GetFirst();
   while (pItem != NULL)
   {
      pItem->Dump();
      pItem = (CTextDisplayListItem*)(pItem->GetNext());
   }
}
#endif

/////////////////////////////////////////////////////////////
// CTextDisplayListItem

CTextDisplayListItem::CTextDisplayListItem(CHARACTER_COUNT nCharacters, CTextStyle* pStyle, BOOL fGreek, const PBOX& Bounds)
{
   ASSERT(nCharacters > 0);
   ASSERT(pStyle != NULL);

   // Attributes of font to use to draw this chunk of text.
   memcpy(&m_CharacterFormat, pStyle->GetCharacterFormatting(), sizeof(m_CharacterFormat));

   // Attributes of font to use to draw this chunk of text.
	pStyle->GetFontDescription(&m_FontDescription);

   // Number of entries in character information array.
   m_nCharacters = nCharacters;

   // Index of next free entry in character information array.
   m_nCharacterIndex = 0;

   // Pointer to array of CCharacterInfo structures that record
   // information about each character to draw.
   m_pCharacterInfo = NULL;
   if (m_nCharacters > 0)
   {
      ASSERT((long)m_nCharacters*sizeof(CharacterInfo) < 65535L);
      TRY
      {
         m_pCharacterInfo = new CharacterInfo[(int)m_nCharacters];
      }
      END_TRY
   }

   // Save "Greeked" flag.
   m_fGreek = fGreek;

   // Save bounds.
   m_Bounds = Bounds;
}

CTextDisplayListItem::CTextDisplayListItem(CTextStyle* pStyle, const PBOX& Bounds)
{
   // Attributes of font to use to draw this chunk of text.
	ASSERT(pStyle->Frame() != NULL);
   memcpy(&m_CharacterFormat, pStyle->GetCharacterFormatting(), sizeof(m_CharacterFormat));
	
   // Attributes of font to use to draw this chunk of text.
	pStyle->GetFontDescription(&m_FontDescription);

   // Number of entries in character information array.
   m_nCharacters = 0;

   // Index of next free entry in character information array.
   m_nCharacterIndex = 0;

   // Pointer to array of CCharacterInfo structures that record
   // information about each character to draw.
   m_pCharacterInfo = NULL;

   // Save "Greeked" flag.
   m_fGreek = FALSE;

   // Save bounds.
   m_Bounds = Bounds;
}

CTextDisplayListItem::~CTextDisplayListItem()
{
   delete [] m_pCharacterInfo;
   m_pCharacterInfo = NULL;
}

#ifdef _DEBUG
void CTextDisplayListItem::Dump(void)
{
   TRACE("F %d Z %ld E %ld P %d O %d S %d B %d I %d\n",
      m_CharacterFormat.m_lFont,
      m_CharacterFormat.m_Size,
      m_CharacterFormat.m_Expansion,
      m_CharacterFormat.m_Fill.m_nPattern,
      m_CharacterFormat.m_Outline.m_nStyle,
      m_CharacterFormat.m_Shadow.m_nStyle,
      m_CharacterFormat.m_bBold,
      m_CharacterFormat.m_bItalic);
   for (int nChar = 0; nChar < m_nCharacters; nChar++)
   {
      CharacterInfo* pInfo = GetCharacter(nChar);
      TRACE("%c (%d,%d) ", (char)(pInfo->Character), pInfo->nX, pInfo->nY);
   }
   TRACE("\n");
}
#endif
