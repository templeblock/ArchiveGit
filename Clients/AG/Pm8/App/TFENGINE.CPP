/*
// TFENGINE.CPP
//
//    Copyright (C) 1993 MicroLogic Software, Inc.
//
// Implementatin of the TrueType font engine class (tTTFontEngine).
//
// $Header: /PM8/App/TFENGINE.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TFENGINE.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:26:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:46   Fred
// Initial revision.
// 
//    Rev 1.12   15 May 1997 16:54:50   Fred
// New text effects
// 
//    Rev 1.11   22 Apr 1997 13:17:48   Fred
// Start of project and art browsers
// 
//    Rev 1.10   17 Apr 1997 15:35:20   Fred
// Rotated fill patterns
// 
//    Rev 1.9   15 Apr 1997 10:01:06   Fred
// Fixed some bugs including mapping the character when getting delta X.
// 
//    Rev 1.8   03 Apr 1997 09:31:30   Fred
// More text cleanup
// 
//    Rev 1.7   26 Mar 1997 15:55:42   Fred
// Removed old geronimo stuff
// 
//    Rev 1.6   26 Mar 1997 15:18:50   Fred
// Removed old geronimo stuff
// 
//    Rev 1.5   07 Feb 1997 08:54:50   Fred
// Added ability to offset curve in MakePath()
// 
//    Rev 1.4   24 Jan 1997 15:00:14   Fred
// Removed some Geronimo conventions including changing G_FRACTION to a long
// 
//    Rev 1.3   13 Jan 1997 15:25:38   Jay
// 16-bit now compiles.
// 
//    Rev 1.2   06 Jan 1997 10:16:34   Fred
// Start of new text object
// 
//    Rev 1.1   24 May 1996 16:16:02   Fred
// TRACEx
// 
//    Rev 1.0   14 Mar 1996 13:48:40   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:40   Jay
// Changes in 2.1
// 
//    Rev 2.13   20 Feb 1996 14:32:32   JAY
// Fixed some SWAPW's being cast to int without short first (sign extension!).
// This fixes the 32-bit underline bug.
// 
//    Rev 2.12   18 Oct 1995 18:39:32   FRED
// Fixed memory leak
// 
//    Rev 2.11   18 Oct 1995 15:58:10   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 2.10   04 Oct 1995 10:56:30   FRED
// 32 bit TrueType engine
// 
//    Rev 2.9   20 Sep 1995 10:01:16   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 2.8   14 Sep 1995 09:32:26   JAY
// 32-bit changes
// 
//    Rev 2.7   13 Sep 1995 11:33:44   JAY
// New 32-bit stuff
// 
//    Rev 2.6   08 May 1995 11:57:58   FRED
// Fixed rare overflow bug.
// 
// 
//    Rev 2.5   28 Apr 1995 13:49:16   FRED
//  
// 
//    Rev 2.4   27 Apr 1995 13:20:18   FRED
// 
//    Rev 2.3   25 Apr 1995 10:34:30   FRED
// 
//    Rev 2.2   15 Mar 1995 10:40:34   JAY
// Support for new text editing
// 
//    Rev 2.1   09 Mar 1995 08:26:58   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:12   JAY
// Initial revision.
// 
//    Rev 1.13   30 Nov 1994 16:37:26   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.12   04 Aug 1994 07:33:56   JAY
// Large model and 32-bit changes
// 
//    Rev 1.11   07 Jul 1994 11:27:14   JAY
// Now calls ClearLastFace in destructor to completely clean up.
// 
//    Rev 1.10   09 Jun 1994 11:10:44   JAY
// Now uses new reset() method of StorageFile.
// 
//    Rev 1.9   01 Jun 1994 16:42:44   JAY
// Added parameter to TypefaceCallback.
// Created tfcallb.h to get rid of duplicate declarations.
// Got rid of "0x8000" and "0x8002" special checks since they are totally
// incompatible with the current typeface numbering scheme.
// 
//    Rev 1.8   11 May 1994 16:33:04   JAY
// made some LPSTRs into LPCSTRs
// 
//    Rev 1.7   16 Mar 1994 16:02:18   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.6   04 Mar 1994 11:54:02   MACDUFF
// New DoGetFontData method allows reading of TT files either via
// file I/O or by the Windows GetFontData API.
// 
//    Rev 1.5   30 Jan 1994 09:49:18   JAY
//  
// 
//    Rev 1.4   28 Jan 1994 15:12:32   FRED
// Changes for new GERONIMO.H
// 
// 
//    Rev 1.3   19 Jan 1994 17:07:18   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:46   JAY
//  
// 
//    Rev 1.1   30 Dec 1993 16:10:34   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:25:30   JAY
//
// 
//    Rev 1.18   07 Jun 1993 17:14:24   MACDUFF
// MakeBitmap returns Done only when the last row of the character is
// drawn, not when the last row of the request is drawn.  This is to
// comply with the Geronimo documentation.
// 
//    Rev 1.17   01 Jun 1993 09:26:52   MACDUFF
// Do not use MLSSYM.TT for symbol (non-text) fonts.
// 
//    Rev 1.16   27 May 1993 18:20:48   MACDUFF
// Use Shared File I/O
// 
//    Rev 1.15   27 May 1993 15:06:32   MACDUFF
// Treats point_size numerator and denominator as unsigned for extra
// bit of precision.  FONT.CPP relies on this.
// 
//    Rev 1.14   25 Apr 1993 20:33:44   MACDUFF
// Support for get_min_pixel_height().
// 
//    Rev 1.13   21 Apr 1993 12:41:40   MACDUFF
// Fixed memory leak.
// 
//    Rev 1.12   04 Mar 1993 15:12:58   MACDUFF
// Caches three commonly-used subtables.
// 
//    Rev 1.11   04 Mar 1993 11:40:14   MACDUFF
// Made static functions near by using "LOCAL" macro.
// 
//    Rev 1.10   24 Feb 1993 14:12:16   MACDUFF
// sets is_text_font to TRUE for Mac fonts
// 
//    Rev 1.9   22 Feb 1993 16:55:50   MACDUFF
// Support for horizontal expansion
// 
//    Rev 1.8   22 Feb 1993 13:36:58   MACDUFF
// Support for Macintosh fonts
// 
//    Rev 1.7   22 Feb 1993 11:20:28   MACDUFF
// Added a SWAPW.
// 
//    Rev 1.6   22 Feb 1993 11:01:10   MACDUFF
// face_information() sets is_text_font
// 
//    Rev 1.5   19 Feb 1993 11:16:00   MACDUFF
// Fixed off-by-one error in returning bitmap_bbox
// 
//    Rev 1.4   19 Feb 1993 10:21:48   MACDUFF
// Sets fengine_flags.
// Will always reread the "local" typeface (0x8002)
// 
//    Rev 1.3   18 Feb 1993 12:16:24   MACDUFF
// First implementation of font_metrics, character_metrics, and make_bitmap.
// They seem to work correctly except that horizontal clipping in make_bitmap
// is sometimes wrong.
// 
//    Rev 1.2   17 Feb 1993 13:05:26   MACDUFF
// Moved some functions from fengine.h to .cpp files
// 
//    Rev 1.1   17 Feb 1993 11:13:14   MACDUFF
// Implemented face_information.
// 
//    Rev 1.0   11 Feb 1993 17:01:24   MACDUFF
// Initial revision.
//
*/

#include "stdafx.h"

#include <string.h>
#include <dos.h>

#include "file.h"
#include "utils.h"

#include "geronimo.h"
#include "ttdll.h"
#include "pmobject.h"
#include "tfengine.h"
#include "outpath.h"

/*
// The global TrueType engine.
*/

PUBLIC CFontEngine* pFontEngine;

// The indices for the font subtables we would like to cache.

static const sfnt_tableIndex tables_to_cache[number_of_tables_to_cache] =
{
   sfnt_indexToLoc,
   sfnt_horizontalMetrics,
   sfnt_charToIndexMap
};

// helper functions that don't need to be methods of the object
static G_RESULT near TT2Gerr(tTTResult i);
static void near UGL2Ascii(char *s, int16 len);

CFontEngine::CFontEngine(GetFontCallback pCallback) :
	FontFile((LPCSTR)NULL)
{
   pGetFontCallback = pCallback;
   CurrentTypeface = -1;

   m_hdcScreen = NULL;
   m_hfontCurrent = NULL;
   m_hfontPrevious = NULL;

   m_nEmUnderlinePosition = -1;
   m_nEmUnderlineThickness = -1;
   
	// Initialize pointers to input and output structures.
   FSIn = &FSInput;
   FSOut = &FSOutput;

	for (int i = 0; i < MEMORYFRAGMENTS; i++)
	{
		FSIn->memoryBases[i] = NULL;
	}
}

CFontEngine::~CFontEngine()
{
   dll.FreeAllocatedMemory(FSIn, FSOut);
   ClearLastFace();
   if (m_hdcScreen != NULL)
   {
      ReleaseDC(NULL, m_hdcScreen);
   }
}

G_RESULT CFontEngine::open_context()
{
   G_RESULT nResult;

	// Initialize pointer to transformation matrix.
   aTransformMatrix = &TransformationMatrix;

	// Initialize pointers to functions used to get and release font fragments.
	// New and delete are used to allocate and free this memory.
   FSIn->GetSfntFragmentPtr = GetFontFragment;
   FSIn->ReleaseSfntFrag = ReleaseFontFragment;

   // Initialize the table cache
   TableCacheValid = FALSE;
   for (int nTable = 0; nTable < number_of_tables_to_cache; nTable++)
   {
      TableCache[nTable].table_buffer = 0;
   }

	// Open up the font engine.
   nResult = TT2Gerr(dll.fs_OpenFonts(FSIn, FSOut));
   if (nResult == G_R_ok)
   {
		// Initialize the font engine.
      nResult = TT2Gerr(dll.fs_Initialize(FSIn, FSOut));
      if (nResult != G_R_ok)
      {
			// Initialization failed.
         dll.fs_CloseFonts(FSIn, FSOut);
      }
   }

	return nResult;
}

G_RESULT CFontEngine::close_context()
{
   return TT2Gerr(dll.fs_CloseFonts(FSIn, FSOut));
}

G_RESULT CFontEngine::make_bitmap(G_BITMAP_PARAMETERS* bitmap_parameters)
{
   BitmapParameters = *bitmap_parameters;
   FontDescription = BitmapParameters.font_description;

   switch (BitmapParameters.command)
   {
		case G_C_size:
		{
			SizeBitmap();
			BitmapParameters.command = G_C_continue;
			BitmapParameters.start_row = BitmapParameters.character_bbox.uly;
			break;
		}
		case G_C_start:
		{
			SetupForCharacter(BitmapParameters.character, BitmapParameters.outline, TRUE);
			if ((BitmapParameters.start_row == 0x8000)
			 || (BitmapParameters.start_row < BitmapParameters.character_bbox.uly))
			{
				BitmapParameters.start_row = BitmapParameters.character_bbox.uly;
			}
			// FALL THROUGH
		}
		case G_C_continue:
		{
			MakeBitmap_ComputeRowCount();
			MakeBitmap_ScanBand();
			break;
		}
   }

   *bitmap_parameters = BitmapParameters;

   if (Result == G_R_ok) switch (BitmapParameters.command)
   {
	   case G_C_continue:
		{
			Result = G_R_partial;
			break;
		}
		case G_C_done:
		{
			Result = G_R_complete;
			break;
		}
   }
   return Result;
}

G_RESULT CFontEngine::face_information(unsigned short typeface_number, CFontFaceInformation* pFaceInformation)
{
   pOffsetTableMap OffsetTable;
   sfnt_OS2 TTOS_2;
   sfnt_FontHeader TTHead;
   sfnt_HorizontalHeader TTHorizontalHeader;
   VOIDPTR TTNamePtr = NULL;
   uint16 w;
   int16 i;
   uint32 Addr;
   uint8 os2_class;
	uint8 os2_subclass;
   uint16 weight_class;

   static const G_CLASS os2_class_map[13] =
   {
      G_CLASS_serif,
      G_CLASS_serif,
      G_CLASS_serif,
      G_CLASS_serif,
      G_CLASS_square_serif,
      G_CLASS_square_serif,
      G_CLASS_serif,
      G_CLASS_serif,
      G_CLASS_sans_serif,
      G_CLASS_display,
      G_CLASS_script,
      G_CLASS_serif,
      G_CLASS_symbol
   };

   // Open the font file

   SetFace(typeface_number);

   if (Result != 0)
   {
      return Result;
   }

   // Get a table offset map
   OffsetTable = &FSOut->Key->offsetTableMap[0];

   // Read the HEAD table
   if (DoGetFontData(OffsetTable[sfnt_fontHeader].Offset, &TTHead, sizeof (TTHead)) == -1)
   {
      return (Result = G_R_file_io);
   }

   // Read the hhea table
   if (DoGetFontData(OffsetTable[sfnt_horiHeader].Offset, &TTHorizontalHeader, sizeof (TTHorizontalHeader)) == -1)
   {
      return (Result = G_R_file_io);
   }

   // Try to get most of the info from the OS/2 table, if available
   Addr = OffsetTable[sfnt_OS_2].Offset;

   if (Addr != 0)
   {
      uint16 usSelection;

      if (DoGetFontData(Addr, &TTOS_2, sizeof (TTOS_2)) == -1)
      {
         return (Result = G_R_file_io);
      }

      pFaceInformation->spacing_type = (TTOS_2.Panose[3] == 9 ? G_SPACING_fixed : G_SPACING_proportional);

      usSelection = SWAPW(TTOS_2.usSelection);

      pFaceInformation->style = (G_STYLE)(usSelection & 0x0001);

      if (((usSelection >> 5) & 0x0001) != 0)
      {
         pFaceInformation->stroke_weight = G_WEIGHT_bold;
      }
      else
      {
         pFaceInformation->stroke_weight = G_WEIGHT_medium;
      }

      pFaceInformation->windows_fsSelection = usSelection;

      os2_class = TTOS_2.sFamilyClass & 0x00ff;
      os2_subclass = ((uint16)TTOS_2.sFamilyClass) >> 8;
      if (os2_class > 12)
		{
			os2_class = 0;
		}

      pFaceInformation->typeface_class = os2_class_map[os2_class];

      // weight class
      weight_class = SWAPW(TTOS_2.usWeightClass);
      if (weight_class < 10)
      {
         if (weight_class == 5) weight_class = 4;
         weight_class *= 100;
      }
      pFaceInformation->windows_usWeightClass = weight_class;

      memcpy(pFaceInformation->vendor_id, TTOS_2.achVendID, 4);
      pFaceInformation->vendor_id[4] = 0;

      for (i = 0; i < 4; i++)
      {
         if (pFaceInformation->vendor_id[i] < 32)
         {
            pFaceInformation->vendor_id[i] = ' ';
         }
      }

      pFaceInformation->is_text_font = (SWAPW(TTOS_2.usFirstChar) < 0xf000);
      pFaceInformation->is_windows_font = TRUE;
   }
   else
   {
      uint16 macStyle;

      // No OS/2 table present, use HEAD table and nominal values.

      pFaceInformation->spacing_type = G_SPACING_proportional;
      pFaceInformation->typeface_class = G_CLASS_serif;

      // Use macStyle bits to set fsSelection and weight
      // Bit 0 = bold, Bit 1 = italic

      macStyle = SWAPW(TTHead.macStyle);

      pFaceInformation->windows_fsSelection = ((macStyle & 0x0001) << 5) | ((macStyle & 0x0002) >> 1);
      if ((macStyle & 0x0001) != 0)
      {
         pFaceInformation->windows_usWeightClass = 400;
         pFaceInformation->stroke_weight = G_WEIGHT_medium;
      }
      else
      {
         pFaceInformation->windows_usWeightClass = 700;
         pFaceInformation->stroke_weight = G_WEIGHT_bold;
      }

      pFaceInformation->is_text_font = TRUE;
      pFaceInformation->is_windows_font = FALSE;
      strcpy((LPSTR)&(pFaceInformation->vendor_id[0]), "????");
   }

   pFaceInformation->lowestRecPPEM = SWAPW(TTHead.lowestRecPPEM);

   // Read the PostScript info if available.

   InvalidateUnderlinePosition();
   InvalidateUnderlineThickness();

   Addr = OffsetTable[sfnt_Postscript].Offset;

   if (Addr != 0)
   {
      sfnt_PostScriptInfo TTPSInfo;

      if (DoGetFontData(Addr, &TTPSInfo, sizeof (TTPSInfo)) == -1)
      {
         return (Result = G_R_file_io);
      }

      // Save the em line spacing so it can be used by font_metrics().
      SetEmUnderlinePosition((short int)SWAPW(TTPSInfo.underlinePosition));
      SetEmUnderlineThickness((short int)SWAPW(TTPSInfo.underlineThickness));
   }

   // Read the name table

   w = (uint16)OffsetTable[sfnt_Names].Length;
	TRY
		TTNamePtr = new BYTE[w];
	END_TRY

   if (TTNamePtr == NULL)
   {
      return (Result = G_R_not_enough_memory);
   }
   
   if (DoGetFontData(OffsetTable[sfnt_Names].Offset, TTNamePtr, w) == -1)
   {
		delete [] (LPBYTE)TTNamePtr;
      return (Result = G_R_file_io);
   }

   TTName(TTName_Family, TTNamePtr, pFaceInformation->family_name, sizeof(pFaceInformation->family_name)-1);
   TTName(TTName_SubFamily, TTNamePtr, pFaceInformation->style_name, sizeof(pFaceInformation->style_name)-1);
   TTName(TTName_FullName, TTNamePtr, pFaceInformation->full_name, sizeof(pFaceInformation->full_name)-1);

   // SKIP UNIQUE_NAME and KERNING DATA for now
	delete [] (LPBYTE)TTNamePtr;
   return G_R_ok;
}

G_RESULT CFontEngine::font_metrics(G_FONT_DESCRIPTION* font_description)
{
   FontDescription = font_description;

   FontPrologue(FontDescription->typeface);
   if (Result != G_R_ok)
   {
      return Result;
   }

   FontDescription->font_bbox.ulx = FSOut->Key->GlobalBBox_EAE.xMin;
   FontDescription->font_bbox.uly = -FSOut->Key->GlobalBBox_EAE.yMax;
   FontDescription->font_bbox.lrx = FSOut->Key->GlobalBBox_EAE.xMax;
   FontDescription->font_bbox.lry = -FSOut->Key->GlobalBBox_EAE.yMin;

   FontDescription->font_space_width = FSOut->Key->SpaceWidth_EAE;
   
   // Determine line spacing

   if (!UnderlinePositionDefined()
    || !UnderlineThicknessDefined())
   {
      // Calling face_information() results in finding Em line spacing
      CFontFaceInformation FI;
      if (face_information(FontDescription->typeface, &FI) != G_R_ok)
      {
         return Result;
      }
   }

   FontDescription->font_underline_position = (long)(short)HIWORD(ext_SafeLongMulDiv(GetEmUnderlinePosition(), FSOut->Key->metricScalarY, FSOut->Key->emResolution)+0x8000);
   FontDescription->font_underline_thickness = (long)(short)HIWORD(ext_SafeLongMulDiv(GetEmUnderlineThickness(), FSOut->Key->metricScalarY, FSOut->Key->emResolution)+0x8000);

   return Result;
}

G_RESULT CFontEngine::character_metrics(unsigned short character, G_FONT_DESCRIPTION* font_description)
{
   FontDescription = font_description;
   SetupForCharacter(character, NULL, FALSE);

   if (Result != 0) return Result;

   FontDescription->character_bbox.ulx = FSOut->Key->CharacterBBox_EAE.xMin;
   FontDescription->character_bbox.uly = -FSOut->Key->CharacterBBox_EAE.yMax+1;
   FontDescription->character_bbox.lrx = FSOut->Key->CharacterBBox_EAE.xMax;
   FontDescription->character_bbox.lry = -FSOut->Key->CharacterBBox_EAE.yMin;

   FontDescription->character_delta_x = FSOut->Key->DeltaX_EAE;

   return Result;
}

G_RESULT CFontEngine::character_outline(unsigned short character, G_FONT_DESCRIPTION* font_description, CFontCharacterOutlineInformation* pInformation)
{
   FontDescription = font_description;
   SetupForCharacter(character, NULL, FALSE);
   if (Result != 0) return Result;
   
   FSIn->param.gridfit.traceFunc = NULL;
   Result = TT2Gerr(dll.fs_ContourGridFit(FSIn, FSOut));
   if (Result != 0) return Result;

   FontDescription->character_bbox.ulx = FSOut->Key->CharacterBBox_EAE.xMin;
   FontDescription->character_bbox.uly = -FSOut->Key->CharacterBBox_EAE.yMax+1;
   FontDescription->character_bbox.lrx = FSOut->Key->CharacterBBox_EAE.xMax;
   FontDescription->character_bbox.lry = -FSOut->Key->CharacterBBox_EAE.yMin;

   pInformation->FreeMemory();

   pInformation->m_fOutlinesExist = FSOut->outlinesExist;
   pInformation->m_nNumberOfContours = FSOut->numberOfContours;
   pInformation->m_pStart = FSOut->startPtr;
   pInformation->m_pEnd = FSOut->endPtr;
   pInformation->m_pX = FSOut->xPtr;
   pInformation->m_pY = FSOut->yPtr;
   pInformation->m_pOnCurve = FSOut->onCurve;

   return Result;
}

uint16 CFontEngine::DoGetFontFragment(
	uint32 Position,
	uint16 BytesToRead,
	VOIDPTR Address)
{
   VOIDPTR cachebuf, readbuf;
   CFontTableCacheItem* titem;
   BOOL already_cached;
   uint16 result;

   result = 0;

   // Try to find a cache item for this table
   cachebuf = 0;
   already_cached = FALSE;

   if (TableCacheValid)
   {
      for (titem = &TableCache[0]; titem < &TableCache[number_of_tables_to_cache]; titem++)
      {
         if (titem->file_position == Position)
         {
            // This is the item for this table
            cachebuf = titem->table_buffer;
            if (cachebuf == NULL)
            {
					TRY
						cachebuf = new BYTE[BytesToRead];
					END_TRY
               if (cachebuf != NULL)
               {
                  titem->table_buffer = cachebuf;
               }
            }
            else
            {
               already_cached = TRUE;
            }
            break;
         }
      }
   }

   if (!already_cached)
   {
      // Read to the cache buffer if there is one, otherwise directly to Addr
      readbuf = (cachebuf != 0) ? cachebuf : Address;

      if (DoGetFontData(Position, readbuf, BytesToRead) == -1)
      {
         result = 0xffff;     // FAILED!
      }
   }

   if (cachebuf != 0)
   {
      if (result == 0)
      {
         // We have the data in the cache, copy it to the destination
         memcpy(Address, cachebuf, BytesToRead);
      }
      else
      {
         // We tried to read to the cache and failed.  Free the cache buffer.
			delete [] (LPBYTE)cachebuf;
         titem->table_buffer = 0;
      }
   }
   return (result);
}

void CFontEngine::FontPrologue(uint16 TypefaceID)
{
   struct _tNewTrans* NewTrans;

   SetFace(TypefaceID);
   if (Result != 0)
   {
      return;
   }

   NewTrans = &FSIn->param.newtrans;

   NewTrans->xResolution = FontDescription->x_resolution;
   NewTrans->yResolution = FontDescription->y_resolution;

   NewTrans->pointSize = FontDescription->point_size;

   NewTrans->pixelDiameter = Sqrt2;

   NewTrans->traceFunc = NULL;

	NewTrans->Fill.m_nPattern = FontDescription->Fill.m_nPattern;
	NewTrans->Outline.m_nStyle = FontDescription->Outline.m_nStyle;
	NewTrans->Outline.m_nWidthType = FontDescription->Outline.m_nWidthType;
	NewTrans->Outline.m_lWidth = FontDescription->Outline.m_lWidth;
	NewTrans->Outline.m_nPattern = FontDescription->Outline.m_nPattern;
	NewTrans->Shadow.m_nStyle = FontDescription->Shadow.m_nStyle;
	NewTrans->Shadow.m_nOffsetType = FontDescription->Shadow.m_nOffsetType;
	NewTrans->Shadow.m_lXOffset = FontDescription->Shadow.m_lXOffset;
	NewTrans->Shadow.m_lYOffset = FontDescription->Shadow.m_lYOffset;
	NewTrans->Shadow.m_nPattern = FontDescription->Shadow.m_nPattern;

   NewTrans->xStretch = 0x10000L;
   NewTrans->yStretch = 0x10000L;

	NewTrans->effects_rotation = FontDescription->effects_rotation;

   NewTrans->transformMatrix = aTransformMatrix;
   IdentityMatrix(aTransformMatrix);

   aTransformMatrix->transform[0][0] = FontDescription->horizontal_expansion;

   if (TransformationInvalid())
   {
      // Remember the transformation.  This is done before the
      // fs_newtransform call because fs_newtransform changes the newtrans
      // values if the point size is 0.

      RememberTransformation();

      Result = TT2Gerr(dll.fs_NewTransformation(FSIn, FSOut));
      if (Result != 0)
      {
         InvalidateTransformation();
      }
   }

   // HERE WE CAN READ TABLES THAT ARE CACHED IN GERONIMO.PAS
}

void CFontEngine::SetFace(int typeface)
{
   int16 i;
   pOffsetTableMap   OffsetTable;
   LOGFONT lf;

   Result = G_R_ok;

   if (typeface == CurrentTypeface)
   {
      return;
   }

   if (CurrentTypeface != -1)
   {
      ClearLastFace();
   }

   // Get the path name or LOGFONT of the typeface 

   switch ((*pGetFontCallback)(typeface, &lf, &FontFile))
   {
      case GFC_RESULT_failure:
      default:
      {
         Result = G_R_internal_error;
         return;
      }
      case GFC_RESULT_file:
      {
         // Force it to open
         if (FontFile.seek(0, ST_DEV_SEEK_SET) != ERRORCODE_None)
         {
            FontFile.reset();

            Result = TT2Gerr(TTResult_FontFileNotFound);
            return;
         }
         break;
      }
      case GFC_RESULT_logfont:
      {
         // Create the font and select it into our DC

         if (m_hdcScreen == NULL)
         {
            m_hdcScreen = GetDC(NULL);
         }

         if (m_hdcScreen != NULL)
         {
            lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
            if ((m_hfontCurrent = CreateFontIndirect(&lf)) == NULL)
            {
               Result = TT2Gerr(TTResult_FontFileNotFound);
               return;
            }

            m_hfontPrevious = (HFONT)SelectObject(m_hdcScreen, m_hfontCurrent);
         }
         break;
      }
   }

   CurrentTypeface = typeface;

   // Set up for NewSFNT.  Try platform=Microsoft, specific=UGL first.

   FSIn->clientID = (int32)this;
   FSIn->param.newsfnt.platformID = PlatformId_Microsoft;
   FSIn->param.newsfnt.specificID = 1;    // UGL

   Result = (G_RESULT)dll.fs_NewSfnt(FSIn, FSOut);

   InvalidateTransformation();

   if (Result == TTResult_OUT_OF_RANGE_SUBTABLE)
   {
      // Retry for subtable 0 (non-text)
      FSIn->param.newsfnt.specificID = 0;
      Result = (G_RESULT)dll.fs_NewSfnt(FSIn, FSOut);
   }

   if (Result == TTResult_OUT_OF_RANGE_SUBTABLE)
   {
      // Retry for Macintosh
      FSIn->param.newsfnt.platformID = PlatformId_Macintosh;
      FSIn->param.newsfnt.specificID = 0;
      Result = (G_RESULT)dll.fs_NewSfnt(FSIn, FSOut);
   }

   CurrentPlatformID = FSIn->param.newsfnt.platformID;
   CurrentSpecificID = FSIn->param.newsfnt.specificID;

   // Initialize the table cache for this font
   OffsetTable = &FSOut->Key->offsetTableMap[0];

   for (i = 0; i < number_of_tables_to_cache; i++)
   {
      uint32 addr;
      addr = OffsetTable[tables_to_cache[i]].Offset;
      if (addr == 0)
      {
         Result = (G_RESULT)TTResult_MISSING_SFNT_TABLE;
         break;
      }
      TableCache[i].file_position = addr;
      TableCache[i].table_buffer = 0;
   }
   TableCacheValid = TRUE;

   Result = TT2Gerr(Result);
}

void CFontEngine::ClearLastFace(void)
{
   if (CurrentTypeface != -1)
   {
   /* Make sure old file is done with. */

      FontFile.reset();

      CurrentTypeface = -1;

      // Discard any cached tables
      for (int16 i = 0; i < number_of_tables_to_cache; i++)
      {
         VOIDPTR p;

         if ((p = TableCache[i].table_buffer) != NULL)
         {
				delete [] (LPBYTE)p;
         }
      }
      TableCacheValid = FALSE;
   }

   if (m_hfontPrevious != NULL)
   {
      SelectObject(m_hdcScreen, m_hfontPrevious);
      m_hfontPrevious = NULL;
   }
   if (m_hfontCurrent != NULL)
   {
      DeleteObject(m_hfontCurrent);
      m_hfontCurrent = NULL;
   }

   InvalidateUnderlinePosition();
   InvalidateUnderlineThickness();
}

void CFontEngine::SetupForCharacter(uint16 Character, void* pOutline, uint16 BitmapMetrics)
{
	FontPrologue(FontDescription->typeface);
	if (Result != 0) return;

   if (pOutline == NULL)
   {
      FSIn->param.newglyph.characterCode = Character;

      if (CurrentPlatformID == PlatformId_Macintosh)
      {
         MapToMacintosh();
      }

      Result = TT2Gerr(dll.fs_NewGlyph(FSIn, FSOut));
      if (Result != 0) return;

      // Did a character come back?
      if (FSOut->glyphIndex == 0)
      {
         // Do not useML SSYM.TT for symbol fonts
         if (!(CurrentPlatformID == PlatformId_Microsoft &&
               CurrentSpecificID == 0))
         {
            // Try switching to MLSSYM.TT
            FontPrologue(Character >= 0xf000
                           ? TRUETYPE_TYPEFACE_ZBATS
                           : TRUETYPE_TYPEFACE_MLSSYM);
            if (Result != 0)
            {
               if (Result == G_R_no_typeface)
               {
                  Result = G_R_no_symbol_otl;
               }
               return;
            }

            FSIn->param.newglyph.characterCode = Character;
            Result = TT2Gerr(dll.fs_NewGlyph(FSIn, FSOut));

            if (Result != 0)
            {
               od("Unavailable character %x\r\n", Character);
               return;
            }
         }
      }

      // We have a glyph.  Get its advance width
      Result = TT2Gerr(dll.fs_GetAdvanceWidth(FSIn, FSOut));
      if (Result != 0) return;

      // If BitmapMetrics, get exact bitmap metrics

      if (BitmapMetrics)
      {
         // Do contour grid fit
         Result = TT2Gerr(dll.fs_ContourGridFit(FSIn, FSOut));
         if (Result != 0) return;

         // Get the bounding box
         Result = TT2Gerr(dll.fs_FindBitmapSize(FSIn, FSOut));
         if (Result != 0) return;
      }
   }
   else
   {
      FSIn->param.newoutline.x = ((CFontCharacterOutlineInformation*)pOutline)->m_pX;
      FSIn->param.newoutline.y = ((CFontCharacterOutlineInformation*)pOutline)->m_pY;
      FSIn->param.newoutline.onCurve = ((CFontCharacterOutlineInformation*)pOutline)->m_pOnCurve;
      FSIn->param.newoutline.sp = ((CFontCharacterOutlineInformation*)pOutline)->m_pStart;
      FSIn->param.newoutline.ep = ((CFontCharacterOutlineInformation*)pOutline)->m_pEnd;
      FSIn->param.newoutline.nc = ((CFontCharacterOutlineInformation*)pOutline)->m_nNumberOfContours;
		FSIn->param.newoutline.use_new_metrics = ((CFontCharacterOutlineInformation*)pOutline)->m_fUseNewMetrics;
		FSIn->param.newoutline.fx_cell_height = ((CFontCharacterOutlineInformation*)pOutline)->m_dwCellHeight;
		FSIn->param.newoutline.fx_cell_width = ((CFontCharacterOutlineInformation*)pOutline)->m_dwCellWidth;
		FSIn->param.newoutline.fx_cell_ascent = ((CFontCharacterOutlineInformation*)pOutline)->m_lCellAscent;
		FSIn->param.newoutline.fx_char_x0 = ((CFontCharacterOutlineInformation*)pOutline)->m_lCharX0;
		FSIn->param.newoutline.fx_char_x1 = ((CFontCharacterOutlineInformation*)pOutline)->m_lCharX1;
		FSIn->param.newoutline.fx_char_y0 = ((CFontCharacterOutlineInformation*)pOutline)->m_lCharY0;
		FSIn->param.newoutline.fx_char_y1 = ((CFontCharacterOutlineInformation*)pOutline)->m_lCharY1;

      // Set the outline.
      Result = TT2Gerr(dll.fs_NewOutline(FSIn, FSOut));
      if (Result != 0) return;

      // Get the bounding box
      Result = TT2Gerr(dll.fs_FindBitmapSize(FSIn, FSOut));
      if (Result != 0) return;
   }
}

void CFontEngine::InvalidateTransformation(void)
{
   memset(&OldNewTrans, 0, sizeof (OldNewTrans));
   memset(&OldTransMatrix, 0, sizeof (OldTransMatrix));
}

uint16 CFontEngine::TransformationInvalid(void)
{
   return (memcmp(&OldNewTrans, &FSIn->param.newtrans, sizeof (OldNewTrans)) != 0)
		 || (memcmp(&OldTransMatrix, aTransformMatrix, sizeof (OldTransMatrix)) != 0);
}

void CFontEngine::RememberTransformation(void)
{
   OldNewTrans = FSIn->param.newtrans;
   OldTransMatrix = *aTransformMatrix;
}

void CFontEngine::IdentityMatrix(pTransMatrix M)
{
   const Fixed FIXED_ONE = 1L << 16;

   memset(&M->transform, 0, sizeof (M->transform));

   M->transform[0][0] = FIXED_ONE;
   M->transform[1][1] = FIXED_ONE;
   M->transform[2][2] = FIXED_ONE;
}


void CFontEngine::TTName(tTTNameIndex TTNameID, VOIDPTR TTNamePtr, LPSTR result, int16 maxlen)
{
   char s[255];
   int16 i;
   uint16 w;
   uint16 Language;
   sfnt_NameRecord* NameRecord;

   const uint16 LANGUAGE_ID_MICROSOFT = 0x0409;    // US English
   const uint16 LANGUAGE_ID_MACINTOSH = 0x0000;    // English

   // Select language
   Language = (CurrentPlatformID == PlatformId_Microsoft ? LANGUAGE_ID_MICROSOFT : LANGUAGE_ID_MACINTOSH);

   // Scan through NameRecord's looking for records that contain the correct
   // PlatformID, NameID, and language.

   i = SWAPW(((sfnt_NamingTable*)TTNamePtr)->count) - 1;  // number of records
   NameRecord = &((sfnt_NamingTable*)TTNamePtr)->NameRecords[i];
   while (
      (i >= 0) &&
      (  ((WORD)CurrentPlatformID != SWAPW(NameRecord->platformID)) ||
         (Language != SWAPW(NameRecord->languageID)) ||
         (TTNameID != (tTTNameIndex)SWAPW(NameRecord->nameID))
      )
   )
   {
      i--;
      NameRecord--;
   }

   *s = 0;     // default to null string

   if (i >= 0)
   {
      // Get length and address of string
      i = (int16)SWAPW(NameRecord->length);
      if (i >= sizeof (s)) i = sizeof (s) - 1;

      w = SWAPW(((sfnt_NamingTable*)TTNamePtr)->stringOffset) +
          SWAPW(NameRecord->offset);

      // move the string
      memcpy(s, (LPSTR)TTNamePtr + w, i);
      s[i] = 0;

      // Convert to UGL if Microsoft platform
      if (CurrentPlatformID == PlatformId_Microsoft)
      {
         UGL2Ascii(s, i/2);
      }

      // Set the current SpecificID here
      CurrentSpecificID = SWAPW(NameRecord->specificID);
   }
   
   // Save the result as LZ-string
   i = strlen(s);
   if (i > maxlen) i = maxlen;
   strncpy(result, s, maxlen);
}

static const uint16 MapMacToUgl[256 - 128] =
{
   0x00c4,
   0x00c5,
   0x00c7,
   0x00c9,
   0x00d1,
   0x00d6,
   0x00dc,
   0x00e1,
   0x00e0,
   0x00e2,
   0x00e4,
   0x00e3,
   0x00e5,
   0x00e7,
   0x00e9,
   0x00e8,
   0x00ea,
   0x00eb,
   0x00ed,
   0x00ec,
   0x00ee,
   0x00ef,
   0x00f1,
   0x00f3,
   0x00f2,
   0x00f4,
   0x00f6,
   0x00f5,
   0x00fa,
   0x00f9,
   0x00fb,
   0x00fc,
   0x2020,
   0x00b0,
   0x00a2,
   0x00a3,
   0x00a7,
   0x2022,
   0x00b6,
   0x00df,
   0x00ae,
   0x00a9,
   0x2122,
   0x00b4,
   0x00a8,
   0x2260,
   0x00c6,
   0x00d8,
   0x221e,
   0x00b1,
   0x2264,
   0x2265,
   0x00a5,
   0x00b5,
   0x2202,
   0x2211,
   0x220f,
   0x03c0,
   0x222b,
   0x00aa,
   0x00ba,
   0x2126,
   0x00e6,
   0x00f8,
   0x00bf,
   0x00a1,
   0x00ac,
   0x221a,
   0x0192,
   0x2248,
   0x2206,
   0x00ab,
   0x00bb,
   0x2026,
   0x00a0,
   0x00c0,
   0x00c3,
   0x00d5,
   0x0152,
   0x0153,
   0x2013,
   0x2014,
   0x201c,
   0x201d,
   0x2018,
   0x2019,
   0x00f7,
   0x25ca,
   0x00ff,
   0x0178,
   0x2215,
   0x00a4,
   0x2039,
   0x203a,
   0xf001,
   0xf002,
   0x2021,
   0x00b7,
   0x201a,
   0x201e,
   0x2030,
   0x00c2,
   0x00ca,
   0x00c1,
   0x00cb,
   0x00c8,
   0x00cd,
   0x00ce,
   0x00cf,
   0x00cc,
   0x00d3,
   0x00d4,
   0xf000,
   0x00d2,
   0x00da,
   0x00db,
   0x00d9,
   0x0131,
   0x02c6,
   0x02d6,
   0x00af,
   0x02d8,
   0x02d9,
   0x02da,
   0x00b8,
   0x02dd,
   0x02db,
   0x02c7
};

void CFontEngine::MapToMacintosh(void)
{
   uint16 c;
   int16 i;

   c = FSIn->param.newglyph.characterCode;
   if (c < 32)
   {
      c = 1;
   }
   else if (c >= 0x0080 && c < 0xffe0)
   {
      i = 128;
      while (i < 255 && MapMacToUgl[i - 128] != c)
      {
         ++i;
      }

      if (i == 256)
      {
         c = 1;
      }
      else
      {
         c = i;
      }
   }

   FSIn->param.newglyph.characterCode = c;
}

DWORD CFontEngine::DoGetFontData(DWORD dwOffset, LPVOID lpvBuffer, DWORD dwBytes)
{
#if 0
   // Fail semi-randomly.
   if ((GetTickCount() & 0x03L) == 0L)
   {
      return -1;
   }
#endif

   // The StorageDevice::read() method only handles transactions < 64K.
   if (dwBytes > 0xffffL)
   {
      return (DWORD)-1;
   }
   // Now we can safely case to USHORT below.


   // First choice is to direct file I/O, if we found the font file path.
   // Second choice is to use the Windows GetFontData() API, if we have
   // a DC to do it with.
   // If we can't do either of those things, we fail.

   if (FontFile.IsAssigned())
   {
      if (FontFile.seek(dwOffset, ST_DEV_SEEK_SET) != ERRORCODE_None
            || FontFile.read(lpvBuffer, (USHORT)dwBytes) != ERRORCODE_None)
      {
         return (DWORD)-1;
      }
      else
      {
         return dwBytes;
      }
   }
   else if (m_hdcScreen != NULL)
   {
      return GetFontData(m_hdcScreen, 0, dwOffset, lpvBuffer, dwBytes);
   }
   return (DWORD)-1;
}


void CFontEngine::MakeBitmap_ExtractValues(void)
{
   BBOX* pBbox = (BBOX*)&FSOut->bitMapInfo.bounds;

	BitmapParameters.byte_width = (short)(FSOut->bitMapInfo.rowBytes);
   BitmapParameters.bitmap_size = BitmapParameters.byte_width * (short)(pBbox->yMax - pBbox->yMin);

   BitmapParameters.bitmap_bbox.ulx = (short)(pBbox->xMin);
   BitmapParameters.bitmap_bbox.uly = (short)(-pBbox->yMax+1);
   BitmapParameters.bitmap_bbox.lrx = (short)(pBbox->xMax - 1);
   BitmapParameters.bitmap_bbox.lry = (short)(-pBbox->yMin);
         
   BitmapParameters.character_bbox = BitmapParameters.bitmap_bbox;
}

void CFontEngine::MakeBitmap_ComputeRowCount(void)
{
   if (BitmapParameters.row_count != 0)
   {
      MakeBitmap_LastRow =
			BitmapParameters.start_row +
			BitmapParameters.row_count - 1;
   }
   else
   {
      MakeBitmap_LastRow =
         BitmapParameters.start_row +
         BitmapParameters.output_buffer_size / BitmapParameters.byte_width - 1;
   }
   if (MakeBitmap_LastRow > BitmapParameters.character_bbox.lry)
   {
      MakeBitmap_LastRow = BitmapParameters.character_bbox.lry;
   }
}

void CFontEngine::MakeBitmap_ScanBand(void)
{
   // Setup Input structure for fs_ContourScan()

   FSIn->param.scan.bottomClip = -MakeBitmap_LastRow;
   FSIn->param.scan.topClip = 1 - BitmapParameters.start_row;
	FSIn->param.scan.Component = BitmapParameters.component;
	FSIn->param.scan.BitMapBuffer = (int32*)(BitmapParameters.output_buffer_address);

#if 0
   od("Buffer address is %lx; size is %u\r\n",
               BitmapParameters.output_buffer_address,
               BitmapParameters.output_buffer_size);

   if (IsBadWritePtr(BitmapParameters.output_buffer_address,
            BitmapParameters.output_buffer_size)
      || IsBadReadPtr(BitmapParameters.output_buffer_address,
            BitmapParameters.output_buffer_size))
   {
      od("***** Bad pointer! *****\r\n");
      Result = G_R_internal_error;
      return;
   }
#endif

   // Validate that buffer is large enough for requested bitmap

   if (BitmapParameters.output_buffer_size <
			(uint16)(FSOut->bitMapInfo.rowBytes *
				(FSIn->param.scan.topClip - FSIn->param.scan.bottomClip)))
   {
      Result = G_R_buffer_too_small;
      return;
   }

   // Do the scan
   Result = TT2Gerr(dll.fs_ContourScan(FSIn, FSOut));
   if (Result != 0) return;

   MakeBitmap_ExtractValues();

   BitmapParameters.bitmap_size = BitmapParameters.byte_width * (short)(FSIn->param.scan.topClip - FSIn->param.scan.bottomClip);

   BBOX* pBbox = (BBOX*)&FSOut->bitMapInfo.bounds;

   BitmapParameters.bitmap_bbox.ulx = (short)(pBbox->xMin);
   BitmapParameters.bitmap_bbox.uly = (short)(1 - FSIn->param.scan.topClip);
   BitmapParameters.bitmap_bbox.lrx = (short)(pBbox->xMax);
   BitmapParameters.bitmap_bbox.lry = (short)(-FSIn->param.scan.bottomClip);

   if (BitmapParameters.command == G_C_start)
   {
      BitmapParameters.command = G_C_continue;
   }
   else if (BitmapParameters.character_bbox.lry == BitmapParameters.bitmap_bbox.lry)
   {
      BitmapParameters.command = G_C_done;
   }
   BitmapParameters.start_row = MakeBitmap_LastRow + 1;
}

G_RESULT CFontEngine::SizeBitmap(void)
{
   BitmapParameters.command = G_C_size;

   SetupForCharacter(BitmapParameters.character, BitmapParameters.outline, TRUE);

   if (Result != G_R_ok) return Result;

   MakeBitmap_ExtractValues();

   CharacterByteWidth = BitmapParameters.byte_width;
   CharacterHeight =
      BitmapParameters.character_bbox.lry -
      BitmapParameters.character_bbox.uly + 1;
   CharacterWidth =
      BitmapParameters.character_bbox.lrx -
      BitmapParameters.character_bbox.ulx + 1;

   return Result;
}

/*
// Perform initialization for a TrueType engine.
*/

BOOL CFontEngine::Initialize(LPCSTR pszDllPath)
{
	// Make sure the DLL is loaded.
   if (!dll.load(pszDllPath))
   {
      return FALSE;
   }

   return open_context() == G_R_ok;
}

static G_RESULT near TT2Gerr(tTTResult i)
{
   switch (i)
   {
      case 0                                    : return G_R_ok;
      case TTResult_DLLNotInitialized           : return G_R_internal_error;
      case TTResult_FontFileNotFound            : return G_R_no_typeface;
      case TTResult_NULL_KEY_ERR                : return G_R_internal_error;
      case TTResult_NULL_INPUT_PTR_ERR          : return G_R_internal_error;
      case TTResult_NULL_MEMORY_BASES_ERR       : return G_R_internal_error;
      case TTResult_VOID_FUNC_PTR_BASE_ERR      : return G_R_internal_error;
      case TTResult_OUT_OFF_SEQUENCE_CALL_ERR   : return G_R_internal_error;
      case TTResult_BAD_CLIENT_ID_ERR           : return G_R_internal_error;
      case TTResult_NULL_SFNT_DIR_ERR           : return G_R_internal_error;
      case TTResult_NULL_SFNT_FRAG_PTR_ERR      : return G_R_internal_error;
      case TTResult_NULL_OUTPUT_PTR_ERR         : return G_R_internal_error;
      case TTResult_INVALID_GLYPH_INDEX         : return G_R_no_character;
      case TTResult_UNDEFINED_INSTRUCTION_ERR   : return G_R_invalid_hint_data;
      case TTResult_TRASHED_MEM_ERR             : return G_R_internal_error;
      case TTResult_POINT_MIGRATION_ERR         : return G_R_internal_error;
      case TTResult_BAD_START_POINT_ERR         : return G_R_invalid_path_data;
      case TTResult_SCAN_ERR                    : return G_R_path_generation_error;
      case TTResult_SFNT_DATA_ERR               : return G_R_invalid_path_data;
      case TTResult_POINTS_DATA_ERR             : return G_R_invalid_path_data;
      case TTResult_INSTRUCTION_SIZE_ERR        : return G_R_invalid_hint_data;
      case TTResult_CONTOUR_DATA_ERR            : return G_R_invalid_path_data;
      case TTResult_GLYPH_INDEX_ERR             : return G_R_invalid_character;
      case TTResult_BAD_MAGIC_ERR               : return G_R_invalid_typeface;
      case TTResult_OUT_OF_RANGE_SUBTABLE       : return G_R_context_too_small;
      case TTResult_UNKNOWN_COMPOSITE_VERSION   : return G_R_invalid_character;
      case TTResult_CLIENT_RETURNED_NULL        : return G_R_context_too_small;
      case TTResult_MISSING_SFNT_TABLE          : return G_R_invalid_typeface;
      case TTResult_UNKNOWN_CMAP_FORMAT         : return G_R_no_character;
      case TTResult_BAD_CALL_ERR                : return G_R_invalid_command;
      case TTResult_TRASHED_OUTLINE_CACHE       : return G_R_error;
      default                                   : return G_R_internal_error;
   }
}

static void near UGL2Ascii(char *s, int16 len)
{
   uint16 *wp;

   wp = (uint16 *)s;
   while (len--)
   {
      *s++ = (*wp++) >> 8;
   }
   *s = 0;
}

#ifdef WIN32
void* __stdcall GetFontFragment(long dwClientID, long dwPosition, long dwBytesToRead)
#else
void* pascal _loadds GetFontFragment(long dwClientID, long dwPosition, long dwBytesToRead)
#endif
{
   ASSERT(dwBytesToRead < 65536L);
	void* pData = new BYTE[(uint16)dwBytesToRead];
   ((CFontEngine*)dwClientID)->DoGetFontFragment(dwPosition, (uint16)dwBytesToRead, pData);
   return pData;
}

#ifdef WIN32
void __stdcall ReleaseFontFragment(void* pData)
#else
void pascal _loadds ReleaseFontFragment(void* pData)
#endif
{
	delete [] pData;
}


/************************************************************************/

// Implementation of CFontFaceInformation class

char* CFontFaceInformation::get_full_name(void)
{
   return (char*)full_name;
}

char* CFontFaceInformation::get_family_name(void)
{
   return (char*)family_name;
}

char* CFontFaceInformation::get_style_name(void)
{
   return (char*)style_name;
}

uint16 CFontFaceInformation::get_min_pixel_height(void)
{
   return lowestRecPPEM;
}


///////////////////////////////////////////////////////////////
// tTTCharacterOutlineInformation

CFontCharacterOutlineInformation::CFontCharacterOutlineInformation()
{
   m_fAllocatedMemory = FALSE;
	m_fUseNewMetrics = FALSE;
}

CFontCharacterOutlineInformation::~CFontCharacterOutlineInformation()
{
   FreeMemory();
}

void CFontCharacterOutlineInformation::FreeMemory(void)
{
   m_fOutlinesExist = FALSE;
   m_nNumberOfContours = 0;

   if (m_fAllocatedMemory)
   {
      delete [] m_pStart;
      delete [] m_pEnd;
      delete [] m_pX;
      delete [] m_pY;
      delete [] m_pOnCurve;
      m_fAllocatedMemory = FALSE;
   }

   m_pStart = NULL;
   m_pEnd = NULL;
   m_pX = NULL;
   m_pY = NULL;
   m_pOnCurve = NULL;
}

BOOL CFontCharacterOutlineInformation::MakePath(COutlinePath* pPath, long lXOffset /*=0*/, long lYOffset /*=0*/)
{
   BOOL fResult = FALSE;

#if 0
   TRACE1("outlinesExist = %d\n", m_fOutlinesExist);
   TRACE1("numberOfContours = %d\n", m_nNumberOfContours);
   TRACE1("coordinatePrecision = %d\n", m_nPrecision);
   
   for (WORD i = 0; i < m_nNumberOfContours; i++)
   {
      TRACE3("Contour #%d - Points(%d to %d)\n", i, m_pStart[i], m_pEnd[i]);
      
      for (unsigned short j = m_pStart[i]; j <= m_pEnd[i]; j++)
      {
         TRACE("   Point #%d - (%d, %d, %d)\n",
            j,
            m_pX[j]>>m_nPrecision,
            m_pY[j]>>m_nPrecision,
            m_pOnCurve[j]);
      }
   }
#endif

   pPath->Free();
   
   if (m_fOutlinesExist)
   {
      fResult = TRUE;
      
      for (WORD nContour = 0; nContour < m_nNumberOfContours; nContour++)
      {
         unsigned short nStart = m_pStart[nContour];
         unsigned short nEnd = m_pEnd[nContour];
         
         if (nEnd > nStart)
         {
            unsigned short nIndex = nStart;
            long lX, lY;
            
            // Process first point in contour.
            
            if (m_pOnCurve[nIndex] != 0)
            {
               // First point is on curve.
               
               lX = TrueTypeToFixed(m_pX[nIndex])+lXOffset;
               lY = TrueTypeToFixed(m_pY[nIndex])+lYOffset;
               pPath->MoveTo(lX, -lY);
               nIndex++;
            }
            else
            {
               // First point is off curve, check if the last point is on the curve.
               
               if (m_pOnCurve[nEnd] != 0)
               {
                  // First point is off curve, last point is on curve.
               
                  nStart = nEnd;
                  nEnd--;
                  
                  lX = TrueTypeToFixed(m_pX[nStart])+lXOffset;
                  lY = TrueTypeToFixed(m_pY[nStart])+lYOffset;
                  pPath->MoveTo(lX, -lY);
               }
               else
               {
                  // First and last points are off curve.
                  
                  lX = (TrueTypeToFixed(m_pX[nIndex])+TrueTypeToFixed(m_pX[nEnd])+1)/2+lXOffset;
                  lY = (TrueTypeToFixed(m_pY[nIndex])+TrueTypeToFixed(m_pY[nEnd])+1)/2+lYOffset;
                  pPath->MoveTo(lX, -lY);
               }
            }
            
            // Process the rest of the points in the contour.
            
            BOOL fDone = FALSE;
            
            while (!fDone)
            {
               // Collect all the on curve points into a series of lines.
               
               while (m_pOnCurve[nIndex] != 0)
               {
                  lX = TrueTypeToFixed(m_pX[nIndex])+lXOffset;
                  lY = TrueTypeToFixed(m_pY[nIndex])+lYOffset;
                  pPath->LineTo(lX, -lY);
                  
                  if (fDone)
                  {
                     break;
                  }
                  
                  // Move to the next point.
                  
                  if (nIndex == nEnd)
                  {
                     nIndex = nStart;
                     fDone = TRUE;
                  }
                  else
                  {
                     nIndex++;
                  }
               }
               
               if (!fDone)
               {
                  // Collect all the off curve points into a series of spline curves.
                  
                  do
                  {
                     // Save the index of the middle point in the spline.
                     
                     unsigned short nCurrentIndex = nIndex;
                     
                     // Compute the index of the next point. If this points is on the curve,
                     // the it is the final point in the curve. Otherwise the final point
                     // is the midpoint between the current point and the next point.
                     
                     unsigned short nNextIndex;
                     
                     if (nIndex == nEnd)
                     {
                        nNextIndex = nStart;
                        fDone = TRUE;
                     }
                     else
                     {
                        nNextIndex = nIndex+1;
                     }
                     
                     if (m_pOnCurve[nNextIndex] != 0)
                     {
                        // Next point is on curve, use it as the final point in the spline.
                        
                        lX = TrueTypeToFixed(m_pX[nNextIndex])+lXOffset;
                        lY = TrueTypeToFixed(m_pY[nNextIndex])+lYOffset;
                        nIndex = nNextIndex;
                     }
                     else
                     {
                        // Next point is off curve, use the midpoint between the current point
                        // and the next point as the final point in the curve.
                        
                        lX = (TrueTypeToFixed(m_pX[nIndex])+TrueTypeToFixed(m_pX[nNextIndex])+1)/2+lXOffset;
                        lY = (TrueTypeToFixed(m_pY[nIndex])+TrueTypeToFixed(m_pY[nNextIndex])+1)/2+lYOffset;
                     }
                     
                     // Add the curve to the path.
                     
                     pPath->SplineTo(TrueTypeToFixed(m_pX[nCurrentIndex])+lXOffset, -TrueTypeToFixed(m_pY[nCurrentIndex])+lYOffset, lX, -lY);
                     
                     if (fDone)
                     {
                        break;
                     }
                     
                     // Move to the next point.
                     
                     if (nIndex == nEnd)
                     {
                        nIndex = nStart;
                        fDone = TRUE;
                     }
                     else
                     {
                        nIndex++;
                     }
                  } while (m_pOnCurve[nIndex] == 0);
               }
            }
            
            pPath->Close();
         }
      }
      
      pPath->End();
   }
   
   return fResult;
}
                  
BOOL CFontCharacterOutlineInformation::FromPath(COutlinePath* pPath, long lXOffset /*=0*/, long lYOffset /*=0*/)
{
   BOOL fResult = FALSE;

   // Free any existing allocated memory.
   FreeMemory();

   if (pPath != NULL)
   {
      // Make a pass through the path and determine the number of points, etc.

      int nNumberOfContours = 0;
      int nNumberOfPoints = 0;

      long* pElement = (long*)(pPath->GetElement(0));

      while (pElement != NULL)
      {
         switch (*(pElement++))
         {
            case COutlinePath::OPE_End:
            {
               pElement = NULL;
               break;
            }

            case COutlinePath::OPE_Close:
            {
               break;
            }

            case COutlinePath::OPE_MoveTo:
            {
               nNumberOfContours++;
               nNumberOfPoints += 1;
               pElement += 1*2;
               break;
            }

            case COutlinePath::OPE_LineTo:
            {
               nNumberOfPoints += 1;
               pElement += 1*2;
               break;
            }

            case COutlinePath::OPE_SplineTo:
            {
               nNumberOfPoints += 2;
               pElement += 2*2;
               break;
            }

            case COutlinePath::OPE_BezierTo:
            {
					// Bezier is converted into a single spline.
//					nNumberOfPoints += 3;
					nNumberOfPoints += 2;
               pElement += 3*2;
               break;
            }

            default:
            {
               ASSERT(FALSE);
               break;
            }
         }
      }

      // Allocate the memory for the various arrays.
      if (nNumberOfContours > 0)
      {
         TRY
         {
            m_fAllocatedMemory = TRUE;
            m_pStart = new short[nNumberOfContours];
            m_pEnd =  new short[nNumberOfContours];
            m_pX = new long[nNumberOfPoints+PHANTOMCOUNT];
            m_pY = new long[nNumberOfPoints+PHANTOMCOUNT];
            m_pOnCurve = new unsigned char[nNumberOfPoints+PHANTOMCOUNT];
            m_fOutlinesExist = TRUE;
            m_nNumberOfContours = nNumberOfContours;
         }
         CATCH_ALL(e)
         {
            FreeMemory();
         }
         END_CATCH_ALL

         if (m_fOutlinesExist)
         {
            short* pStart = m_pStart;
            short* pEnd = m_pEnd;
            long* pX = m_pX;
            long* pY = m_pY;
            unsigned char* pOnCurve = m_pOnCurve;

            long* pElement = (long*)(pPath->GetElement(0));

            while (pElement != NULL)
            {
               switch (*(pElement++))
               {
                  case COutlinePath::OPE_End:
                  {
                     pElement = NULL;
                     break;
                  }

                  case COutlinePath::OPE_Close:
                  {
                     *pEnd++ = pX-m_pX-1;
                     break;
                  }

                  case COutlinePath::OPE_MoveTo:
                  {
                     *pStart++ = pX-m_pX;
                     *pX++ = FixedToTrueType(*(pElement++)+lXOffset);
                     *pY++ = FixedToTrueType(-(*(pElement++)+lYOffset));
                     *pOnCurve++ = 1;
                     break;
                  }

                  case COutlinePath::OPE_LineTo:
                  {
                     *pX++ = FixedToTrueType(*(pElement++)+lXOffset);
                     *pY++ = FixedToTrueType(-(*(pElement++)+lYOffset));
                     *pOnCurve++ = 1;
                     break;
                  }

                  case COutlinePath::OPE_SplineTo:
                  {
                     *pX++ = FixedToTrueType(*(pElement++)+lXOffset);
                     *pY++ = FixedToTrueType(-(*(pElement++)+lYOffset));
                     *pOnCurve++ = 0;
                     *pX++ = FixedToTrueType(*(pElement++)+lXOffset);
                     *pY++ = FixedToTrueType(-(*(pElement++)+lYOffset));
                     *pOnCurve++ = 1;
                     break;
                  }

                  case COutlinePath::OPE_BezierTo:
                  {
							// The following is an approximation. It seems that converting the
							// bezier back into a single spline works OK if the original curve
							// was a spline. This will be the case if the data originally came
							// from a TrueType character.

							// Other possible solutions to get more accurate curve data:
							//
							//		1) Add bezier rasterization to TrueType engine. This also
							//			requires extending the data format to allow bezier curves.
							//
							//		2) Break bezier into one or more quadratic splines which
							//			give the required accuracy.

							CFixed lAX = TrueTypeToFixed(*(pX-1))-lXOffset;
							CFixed lAY = -(TrueTypeToFixed(*(pY-1)))-lYOffset;

							CFixed lBX = *(pElement++);
							CFixed lBY = *(pElement++);

							CFixed lCX = *(pElement++);
							CFixed lCY = *(pElement++);

							CFixed lDX = *(pElement++);
							CFixed lDY = *(pElement++);

							// Reverse of Spline --> Bezier equation.
							CFixed lX = MulDivFixed(lBX,3,4)+MulDivFixed(lCX,3,4)-MulDivFixed(lAX,1,4)-MulDivFixed(lDX,1,4);
							CFixed lY = MulDivFixed(lBY,3,4)+MulDivFixed(lCY,3,4)-MulDivFixed(lAY,1,4)-MulDivFixed(lDY,1,4);

							lX += lXOffset;
							lY += lYOffset;
							lY = -lY;

							*pX++ = FixedToTrueType(lX);
							*pY++ = FixedToTrueType(lY);
							*pOnCurve++ = 0;

							lDX += lXOffset;
							lDY += lYOffset;
							lDY = -lDY;

							*pX++ = FixedToTrueType(lDX);
							*pY++ = FixedToTrueType(lDY);
							*pOnCurve++ = 1;

                     break;
                  }

                  default:
                  {
                     ASSERT(FALSE);
                     break;
                  }
               }
            }

            for (int nPhantomPoint = 0; nPhantomPoint < PHANTOMCOUNT; nPhantomPoint++)
            {
               m_pX[nNumberOfPoints+nPhantomPoint] = 0;
               m_pY[nNumberOfPoints+nPhantomPoint] = 0;
               m_pOnCurve[nNumberOfPoints+nPhantomPoint] = 0;
            }

            fResult = TRUE;
         }
      }
   }

   return fResult;
}

/*
// Initialize the global font engine.
*/

BOOL InitializeFontEngine(GetFontCallback pCallback, LPCSTR pszDllPath)
{
	if (pFontEngine == NULL)
	{
	   if ((pFontEngine = new CFontEngine(pCallback)) != NULL)
		{
			if (pFontEngine->Initialize(pszDllPath))
			{
				return TRUE;
			}
			delete pFontEngine;
			pFontEngine = NULL;
		}
	}
   return FALSE;
}

/*
// Get rid of the global font engine.
*/

VOID DeinitializeFontEngine(VOID)
{
   if (pFontEngine != NULL)
   {
      delete pFontEngine;
		pFontEngine = NULL;
   }
}
