/*
// $Header: /PM8/App/FONTSRV.H 1     3/03/99 6:05p Gbeddow $
//
// Font server definitions.
//
// $Log: /PM8/App/FONTSRV.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     3/18/98 11:14a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:21:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:56   Fred
// Initial revision.
// 
//    Rev 1.7   09 Apr 1997 14:52:14   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.6   05 Mar 1997 14:36:44   Fred
// More text object stuff
// 
//    Rev 1.5   07 Feb 1997 08:59:28   Fred
// Changed prototypes for DrawString and other functions
// 
//    Rev 1.4   28 Jan 1997 16:56:46   Fred
// Start of component drawing
// 
//    Rev 1.3   12 Jul 1996 16:47:06   Jay
// removed old methods
// 
//    Rev 1.2   20 Jun 1996 17:39:20   Jay
// Made some shorts long.
// 
//    Rev 1.1   25 Apr 1996 12:16:28   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:48:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:18   FRED
// Initial revision.
// 
//    Rev 2.3   01 May 1995 15:24:46   JAY
// Restructured the macro stuff
// 
//    Rev 2.2   01 May 1995 08:43:24   JAY
// Now has access to macro server.
// 
//    Rev 2.1   27 Apr 1995 13:20:14   FRED
// 
//    Rev 2.0   07 Feb 1995 15:48:34   JAY
// Initial revision.
// 
//    Rev 1.1   09 Feb 1994 16:50:48   MACDUFF
// New param to draw_string()
// 
//    Rev 1.0   16 Dec 1993 16:26:04   JAY
//  
*/

#ifndef __FONTSRV_H__
#define __FONTSRV_H__

#include "style.h"
#include "redisp.h"

class CTextDisplayList;

/*
// A font server.
// This is an abstract class.
*/

class FontServer
{
public:
/*
// Virtual destructor so derived classes can have there own.
*/

	virtual ~FontServer() {}

////////////////////////////////////////
// New style routines.

/*
// Set the default style for a font style structure.
// This will cause a font lock on the default face.
*/

	virtual void SetDefault(CTextStyle* pStyle) = 0;

/*
// Fill out all relevant font metrics in the style structure.
// (This replaces the old fix_style() function.
*/

	virtual void FontMetrics(CTextStyle* pStyle) = 0;

/*
// Return the delta width for a character.
*/

	virtual PCOORD GetAdvanceWidth(CTextStyle* pStyle, CHARACTER Character, DB_RECORD_NUMBER lPage = 0) = 0;

/*
// Draw a string.
*/

	virtual CHARACTER_COUNT DrawString(
		RedisplayContextPtr rc,
		CHARACTER_PTR pCharacters,
		CHARACTER_COUNT nCharacters,
		CTextStyle* pStyle,
		PCOORD nX,
		PCOORD nY,
		BOOL fClearOut,
		PCOORD_PTR pNewX,
		const PBOX& Bounds,
		CTextDisplayList* pDisplayList) = 0;

/*
// Draw the contents of a display list.
*/

	virtual BOOL DrawDisplayList(CFrameObject* pFrameObject, RedisplayContextPtr rc, CTextDisplayList* pDisplayList) = 0;
};

typedef FontServer far *FontServerPtr;

#endif					// #ifndef __FONTSRV_H__

