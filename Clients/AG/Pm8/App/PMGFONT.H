/*
// $Workfile: PMGFONT.H $
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
// $Log: /PM8/App/PMGFONT.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 4     3/01/99 11:55a Cschende
// added functions to make permanent font substitutions in the document
// after it is read in for font which are no longer going to be
// distributed with PrintMaster
// 
// 3     3/18/98 11:14a Jayn
// Page numbers
// 
// 2     12/18/97 3:21p Dennis
// Added capability to set typeface at a per object basis.
// 
//    Rev 1.0   14 Aug 1997 15:23:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:10   Fred
// Initial revision.
// 
//    Rev 1.21   15 May 1997 16:46:44   Fred
// New text effects
// 
//    Rev 1.20   22 Apr 1997 13:17:04   Fred
// Start of project and art browsers
// 
//    Rev 1.19   17 Apr 1997 15:36:04   Fred
// Rotated fill patterns
// 
//    Rev 1.18   11 Apr 1997 13:56:50   Fred
// Font Caching (bitmaps, rotated, warped)
// 
//    Rev 1.17   09 Apr 1997 14:51:10   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.16   03 Apr 1997 17:22:56   Fred
// Fixed some warnings in 16-bit
// 
//    Rev 1.15   03 Apr 1997 09:30:56   Fred
// More text cleanup
// 
//    Rev 1.14   13 Mar 1997 13:17:18   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.13   12 Mar 1997 14:55:06   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.12   06 Mar 1997 13:22:14   Fred
// More text stuff
// 
//    Rev 1.11   05 Mar 1997 14:37:44   Fred
// More text object stuff
// 
//    Rev 1.10   11 Feb 1997 14:53:56   Jay
// 16-bit changes
// 
//    Rev 1.9   07 Feb 1997 08:50:56   Fred
// Start of new text drawing pipeline
// 
//    Rev 1.8   28 Jan 1997 16:56:48   Fred
// Start of component drawing
// 
//    Rev 1.7   24 Jan 1997 15:00:32   Fred
// Removed some Geronimo conventions including changing G_FRACTION to a long
// 
//    Rev 1.6   06 Jan 1997 10:16:32   Fred
// Start of new text object
// 
//    Rev 1.5   05 Nov 1996 14:07:00   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.4   16 Jul 1996 11:26:48   Jay
// Got rid of some old code.
// 
//    Rev 1.3   20 Jun 1996 17:39:32   Jay
// Made some shorts long.
// 
//    Rev 1.2   24 May 1996 09:28:08   Jay
// New text code
// 
//    Rev 1.1   25 Apr 1996 12:18:16   Jay
// Beginnigs of new text support.
// 
//    Rev 1.0   14 Mar 1996 13:29:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:32   FRED
// Initial revision.
// 
//    Rev 2.7   09 Aug 1995 13:10:32   JAY
// Misc changes.
// 
//    Rev 2.6   01 May 1995 15:27:48   JAY
// Restructured the macro stuff
// 
//    Rev 2.5   01 May 1995 08:45:06   JAY
// Changed macrosvr.h to macrosrv.h (now in DATABASE).
// 
//    Rev 2.4   27 Apr 1995 13:27:02   FRED
//  
// 
//    Rev 2.3   25 Apr 1995 09:02:16   JAY
// Added macro server to be go-between for document and font engine.
// 
//    Rev 2.2   29 Mar 1995 09:14:36   JAY
// Lots of changes
// 
//    Rev 2.1   08 Feb 1995 13:33:40   JAY
// Reverted. New series.
// 
//    Rev 1.10   04 Aug 1994 07:34:44   JAY
// Large model and 32-bit changes
// 
//    Rev 1.9   18 Mar 1994 13:35:44   MACDUFF
// Removed old pmg line spacing from font structure.  It's now
// in font_description, so it will be cached.
// 
//    Rev 1.8   16 Mar 1994 15:53:12   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.7   09 Mar 1994 11:29:54   JAY
// Now uses DIB pattern brushes for non-black colors on palette screen devices.
// 
//    Rev 1.6   09 Feb 1994 16:42:08   MACDUFF
// Added screen bounds param to draw_string().
// 
//    Rev 1.5   02 Feb 1994 11:42:32   JAY
// New cache and fonts array flush routine.
// Global DCs for speeding up (ha!) text drawing.
// 
//    Rev 1.4   01 Feb 1994 17:45:18   JAY
// Now creates one DC per string call.
// Now assumes characters in a document are in ANSI (not MicroLogic numbering).
// 
//    Rev 1.3   28 Jan 1994 17:13:22   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:13:50   JAY
//  
// 
//    Rev 1.1   30 Dec 1993 16:12:40   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:28:56   JAY
//  
*/

class PMGFontServer;
class CTextDisplayList;
class CTextDisplayListItem;

#ifndef __PMGFONT_H__
#define __PMGFONT_H__

#include "fontsrv.h"
#include "typeface.h"
#include "geronimo.h"
#include "fcache.h"
#include "dlist.h"
#include "macrosrv.h"

class COutlinePath;

#define ACCESS_LOCKED      ((ULONG)-1)

/*
// An array to hold TrueType fonts.
*/

#define  FONT_ARRAY_SIZE   10

typedef struct
{
   FaceEntry*           m_pFace;					// pointer to typeface list entry
   ULONG                m_lLastAccess;			// for LRU replacement (if ACCESS_LOCKED, then entry is locked)
   G_FONT_DESCRIPTION   m_FontDescription;	// font description (typeface, effects, resolution, font metrics)
   CHARACTER            m_nLastCharacter;
   FONT_STYLE           m_nFontStyle;			// Bold, italic etc.
   PMGFontServer*       m_pOwner;				// Pointer to FontServer object that initialized structure
} FONT_DESCRIPTION;

/*
// The PMG (sic) server.
*/

class PMGFontServer : public FontServer
{
private:
   PMGDatabasePtr database;

/* A handle to global memory for DIB pattern brushes. */

   HGLOBAL m_dib_handle;
   LPBYTE m_dib_memory;
   COLORREF m_dib_rgb;
   HBRUSH m_dib_brush;

/* Last style used and its index. */

   CCharacterFormatting m_LastCharacterFormat;
   int m_nLastFontIndex;            /* last font_index */

   DB_RECORD_NUMBER m_lDefaultStyle;

   CString           m_csDefaultFace;

   HBRUSH create_dib_brush(COLORREF color);

   VOID DrawStrip(
		RedisplayContextPtr rc,
		MAPPTR pMap,
		int nX,
		int nY,
		BOOL fMask,
		BOOL fClearOut);

	// Look up a font item in the font cache.
	CFontCache::FontCacheItem* GetFontCacheItem(CTextStyle* pStyle);
	CFontCache::FontCacheItem* GetFontCacheItem(CFontDescription* pFontDescription);

	// Get the font dependent character metrics for a character in a font.
	CCharacterMetricsCache::CharacterMetricsCacheItem* GetCharacterMetricsCacheItem(CTextStyle* pStyle, CHARACTER Character);
	CCharacterMetricsCache::CharacterMetricsCacheItem* GetCharacterMetricsCacheItem(CFontDescription* pFontDescription, CHARACTER Character);

protected:
   // New text style versions.
   PCOORD MacroWidth(CTextStyle* pStyle, CHARACTER Character, DB_RECORD_NUMBER lPage = 0);

   CHARACTER_COUNT DrawMacro(
		RedisplayContextPtr rc,
		CHARACTER_PTR pCharacters,
		CTextStyle* pStyle,
		PCOORD nX,
		PCOORD nY,
		BOOL fClearOut,
		PCOORD_PTR pNewX,
		const PBOX& Bounds,
		CTextDisplayList* pDisplayList);

   void GreekCharacter(
      RedisplayContextPtr rc,
      CHARACTER Character,
		CFontCache::FontCacheItem* pFontItem,
      PCOORD nDeltaX,
      PCOORD nX,
      PCOORD nY,
		COLOR Color,
      CFrameObject* pFrameObject);

	void DrawRectangle(
		RedisplayContextPtr rc,
		const PBOX& Bound,
		COLORREF clColor,
		CFrameObject* pFrameObject,
		BOOL fToggle = FALSE);

   void DrawCharacter(
      RedisplayContextPtr rc,
      CHARACTER Character,
      const CCharacterFormatting* pCharacterFormat,
		CFontCache::FontCacheItem* pFontItem,
      PCOORD nX,
      PCOORD nY,
		DWORD dwComponent,
      CFrameObject* pFrameObject);

public:

   PMGFontServer();
   ~PMGFontServer();

/*
// Configuration variables.
*/

   static CString default_face;
   static SHORT default_point_size;

/*
// Overridden per object point size.
*/

   int m_nDefaultPointSize;

/*
// Set the database for this font server.
// This must be done before font_record_to_face() or font_face_to_record()
// is called.
*/

   void set_database(PMGDatabasePtr db)
      { database = db; }

/*
// Translate a face number into a font record.
// The record will have an additional temp reference count reflecting this
// usage (so you need to call 'free_font_record' when done).
*/

   DB_RECORD_NUMBER font_face_to_record(SHORT face);

/*
// Translate a font record to a face.
*/

   SHORT font_record_to_face(DB_RECORD_NUMBER record);

////////////////////////////////////
// New style routines.

/*
// Initialize the default style for this document.
*/

   DB_RECORD_NUMBER GetDefaultStyle(void);

/*
// Set the default style for a font style structure.
*/

   void SetDefault(CTextStyle* pStyle);

   void SetDefaultFace(LPCSTR szFace)
      { m_csDefaultFace = szFace; }

   LPCSTR GetDefaultFace(void)
      { return m_csDefaultFace; }
/*
// Fill out all relevant font metrics in the style structure.
// (This replaces the old fix_style() function.
*/

   void FontMetrics(CTextStyle* pStyle);

/*
// Return the advance width for a character.
*/

   PCOORD GetAdvanceWidth(CTextStyle* pStyle, CHARACTER Character, DB_RECORD_NUMBER lPage = 0);

/*
// Draw a string.
*/

   CHARACTER_COUNT DrawString(
		RedisplayContextPtr rc,
		CHARACTER_PTR pCharacters,
		CHARACTER_COUNT nCharacters,
		CTextStyle* pStyle,
		PCOORD nX,
		PCOORD nY,
		BOOL fClearOut,
		PCOORD_PTR pNewX,
		const PBOX& Bounds,
		CTextDisplayList* pDisplayList);

/*
// Return the outline path for a character.
*/

	BOOL CharacterOutline(
		RedisplayContextPtr rc,
		CHARACTER Character,
		const CFontDescription* pFontDescription,
		CFixedRect* pBounds,
		COutlinePath* pPath);
   
/*
// Draw a display list.
*/

   BOOL DrawDisplayList(CFrameObject* pFrameObject, RedisplayContextPtr rc, CTextDisplayList* pDisplayList);

/*
// Reset the fonts array (and anything else.)
*/
   static VOID reset_fonts(VOID);


	bool SubstituteFonts();
	DB_RECORD_NUMBER FindSubstitute(DB_RECORD_NUMBER record);

};

extern CFontCache* pFontCache;


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

class CTextDisplayList : public tDoubleLinkedList
{
public:
   CTextDisplayList(BOOL fClearOut);
   virtual ~CTextDisplayList();

public:
   void AddItem(CHARACTER_COUNT nCharacters, CTextStyle* pStyle, BOOL fGreek, const PBOX& Bounds);
   void AddItem(CTextStyle* pStyle, const PBOX& Bounds);
   void AddCharacter(CHARACTER Character, PCOORD nDeltaX, PCOORD nX, PCOORD nY);

public:
   BOOL IsClearOut(void) const
      { return m_fClearOut; }
   
protected:
   BOOL m_fClearOut;

public:
	int m_nComponent;
	CTextDisplayListItem* m_pItem;
	CHARACTER_COUNT m_nCharacter;
	BOOL m_fInterrupted;

#ifdef _DEBUG
public:
   void Dump(void);
#endif
};

/////////////////////////////////////////////////////////////
// CTextDisplayListItem
//
// This object records a call to PMGFontServer::DrawString.

class CTextDisplayListItem : public tDoubleLinkedListItem
{
public:
	// Local structure definition to record a character and
	// a position. An array of these is allocated and each
	// character along with its position is saved in this
	// array.
   struct CharacterInfo
   {
      CHARACTER Character;
		PCOORD nDeltaX;
      PCOORD nX;
      PCOORD nY;
   };

public:
   CTextDisplayListItem(CHARACTER_COUNT nCharacters, CTextStyle* pStyle, BOOL fGreek, const PBOX& Bounds);
   CTextDisplayListItem(CTextStyle* pStyle, const PBOX& Bounds);
   virtual ~CTextDisplayListItem();

public:
   // Return a pointer to the character formatting information for the
   // characters in this display list item.
   CCharacterFormatting* GetCharacterFormatting(void)
      { return &m_CharacterFormat; }

   // Return a pointer to the font description information for the
   // characters in this display list item.
   CFontDescription* GetFontDescription(void)
      { return &m_FontDescription; }

   // Return the number of defined entries in the character information array.
   CHARACTER_COUNT CharacterCount(void) const
      { return m_nCharacterIndex; }

   // Save character information in the character information array.
   void AddCharacter(CHARACTER Character, PCOORD nDeltaX, PCOORD nX, PCOORD nY)
   {
      ASSERT(m_nCharacterIndex < m_nCharacters);
      ASSERT(m_pCharacterInfo != NULL);
      m_pCharacterInfo[m_nCharacterIndex].Character = Character;
      m_pCharacterInfo[m_nCharacterIndex].nDeltaX = nDeltaX;
      m_pCharacterInfo[m_nCharacterIndex].nX = nX;
      m_pCharacterInfo[m_nCharacterIndex].nY = nY;
      m_nCharacterIndex++;
   }

   // Return a pointer to a character information at the given index.
   CharacterInfo* GetCharacter(CHARACTER_COUNT nIndex) const
   {
      ASSERT(nIndex < m_nCharacterIndex);
      ASSERT(m_pCharacterInfo != NULL);
      return m_pCharacterInfo+nIndex;
   }

   // Return whether the font is greeked or not.
   BOOL IsGreek(void) const
      { return m_fGreek; }

   // Return PBOX bounds.
   const PBOX& GetBounds(void) const
      { return m_Bounds; }

protected:
   // Attributes of font to use to draw this chunk of text.
   CCharacterFormatting m_CharacterFormat;

   // Attributes of font to use to draw this chunk of text.
   CFontDescription m_FontDescription;

   // Number of entries in character information array.
   CHARACTER_COUNT m_nCharacters;

   // Index of next free entry in character information array.
   CHARACTER_COUNT m_nCharacterIndex;

   // Pointer to array of CCharacterInfo structures that record
   // information about each character to draw.
   CharacterInfo* m_pCharacterInfo;

   // "Greek" flag.
   BOOL m_fGreek;

   // Bounding box of display list item.
   PBOX m_Bounds;

#ifdef _DEBUG
public:
   void Dump(void);
#endif
};

#endif               // #ifndef __PMGFONT_H__
