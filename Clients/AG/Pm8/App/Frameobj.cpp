/*
// $Header: /PM8/App/FRAMEOBJ.CPP 1     3/03/99 6:05p Gbeddow $
//
// Frame object routines.
//
// $Log: /PM8/App/FRAMEOBJ.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 54    3/01/99 12:19p Lwilson
// Removed change to primary_action, so selection rather then edit would
// be the primary action.  Also updated, GetContentCursor() to return the
// move cursor when not in editing mode.
// 
// 53    2/19/99 1:09p Cschende
// set non-compliance flag for drop caps
// 
// 52    2/15/99 10:02a Cschende
// hack to get the symbol font for bullets until I see why the font server
// is not getting the right font.
// 
// 51    2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 50    2/01/99 2:35p Cschendel
// commented out inflating teh path for text gutter
// 
// 49    1/21/99 12:02p Cschendel
// Added a ETextWrapGutter type to the framerec for serialization and the
// the creation struct. Old files will have not text wrap gutter but newer
// files will have the default setting of the creation struct wich is
// GUTTER_medium
// 
// 48    1/19/99 4:08p Cschendel
// added html non-compliance for text with bullets
// 
// 47    1/14/99 9:45a Cschende
// 
// 46    1/14/99 9:30a Cschende
// made the array used for counting numbered list paragraphs a member,
// added the function InitListCounter to initialize it correctly when
// bulleted lists flow across linked frames.  changed logic of the bullet
// style BULLET_symbol_mix to 
// be of the bullet type BULLET_symbol instead of BULLET_multilevel to
// simplify code
// 
// 45    12/24/98 12:56p Cschende
// use defines for bullet symbol characters in DrawBullet()
// 
// 44    12/15/98 5:49p Cschende
// changed logic for bulletlevel to be 0 based to fix bugs
// also fixed bugs where back to back bulleted lists didin't start
// renumbering and a bug when user indents 2 levels at once.
// 
// 43    12/10/98 4:10p Cschende
// Added bullet, numbered list and multilevel list support in text.
// 
// 42    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 41    9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 40    9/04/98 3:53p Johno
// Changes to SetHTMLAlign() for new CHTMLTagParagraph
// 
// 39    7/27/98 4:46p Johno
// ConvertToHTML() - fixed bug in macro charcter handling.
// 
// 38    7/22/98 10:01a Jayn
// Doesn't mangle incoming RC anymore during offscreen drawing.
// 
// 37    7/15/98 10:39a Rlovejoy
// Pull in 1-pt outlines by 1 pixel.
// 
// 36    7/01/98 5:34p Johno
// Checks for disabled hyperlinks in ConvertToHTML()
// 
// 35    7/01/98 4:18p Johno
// Added RemoveHyperlink(); checks for disabled hyperlinks in
// UpdateFrame().
// 
// 34    6/25/98 4:12p Johno
// Fix for multiple underlined spaces problem in UpdateFrame()
// 
// 33    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 32    6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 31    5/11/98 6:25p Johno
// Modified CFrameObject::ConvertToHTML() to conserve HTML Font objects
// more.
// 
// 30    5/08/98 11:09a Johno
// Fixed bug detecting HTML style changes in ConvertToHTML()
// 
// 29    5/07/98 4:50p Johno
// Added m_lFrameFlags to CFrameObjectRecord, and methods.
// Added remarks to ConvertToHTML().
// 
// 28    5/06/98 4:37p Johno
// Modified  CFrameObject::ConvertToHTML() to conserve HTML Font objects.
// 
// 27    5/05/98 4:43p Rlovejoy
// Don't change fill color if gradient is applied.
// 
// 26    4/30/98 6:44p Jayn
// Further fix for the FillFormat changes.
// 
// 25    4/30/98 5:16p Johno
// Compile update
// 
// 24    4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 23    4/29/98 7:19p Johno
// Moved out CHTMLCompliantInfo to CHTMLObject
// 
// 22    4/29/98 5:59p Rlovejoy
// Draw functions use fill objects.
// 
// 21    4/28/98 11:28a Johno
// Changes for rotated and flipped image maps
// 
// 20    4/24/98 2:41p Johno
// GetURLList() handles wrapped text as multiple image maps
// 
// 19    4/23/98 3:23p Johno
// HTML changes for rotated text with links (as graphic with polygon image
// map)
// 
// 18    4/17/98 3:56p Johno
// Changes for HTML design checker
// 
// 17    4/08/98 4:19p Johno
// Use AddLineBreak() instead of Add(CHTMLTagLineBreak()).
// 
// 16    4/03/98 4:26p Johno
// Compile update
// 
// 15    4/01/98 12:09p Johno
// Added GetURLList; changed TRACE to JTRACE; text formatting
// 
// 14    3/31/98 10:12a Johno
// Preliminary GetURLList
// 
// 13    3/27/98 5:35p Johno
// ConvertToHtml used CHTMLTagLink::SetHyperlinkData instead of SetURL
// 
// 12    3/25/98 6:31p Johno
// Changes for text links
// 
// 11    3/24/98 5:34p Johno
// Moved HTML character translation from ConvertToHTML to CHTMLFont
// 
// 10    3/17/98 3:41p Johno
// Got rid of CHTMLCharacter
// 
// 9     3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 8     3/12/98 10:23a Johno
// Process tag characters to HTML alias in CFrameObject::ConvertToHTML
// 
// 7     2/10/98 1:20p Dennis
// Added paragraph alignment logic for HTML generation
// 
// 6     12/18/97 3:37p Dennis
// Added state logic for compliance check.
// 
// 5     12/11/97 4:20p Dennis
// Tabs expand to 5 characters for HTML publishing.
// 
// 4     11/21/97 4:44p Hforman
// added new test in DrawFrameOutline() for user setting
// 
// 3     11/14/97 11:27a Dennis
// HTML conversion support
 * 
 * 2     10/28/97 5:41p Dennis
// 
//    Rev 1.2   02 Oct 1997 10:24:04   Jay
// Now passes rc to get_refresh_bound(). Was clipping printouts to the screen.
// 
//    Rev 1.1   18 Aug 1997 15:36:00   Jay
// Bug fix with fill color being set to none (and not re-settable).
// 
//    Rev 1.0   14 Aug 1997 15:21:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:34   Fred
// Initial revision.
//
//    Rev 1.82   28 Jul 1997 14:57:22   Fred
// Bumped out refresh bounds to take care of redisplay glitch
// 
//    Rev 1.81   21 Jul 1997 12:49:04   Fred
// Fixed 16-bit clipping bug
// 
//    Rev 1.80   16 Jul 1997 09:07:28   Jay
// Fixed refresh problem with center and right-aligned text (actually a more general bug).
// 
//    Rev 1.79   16 Jul 1997 08:36:18   Fred
// Removed TRACE
// 
//    Rev 1.78   15 Jul 1997 17:35:26   Fred
// Saves and restores effects in object state
// 
//    Rev 1.77   07 Jul 1997 14:30:40   Jay
// Fixed bug in title text conversion where point size ended up 0 (etc.).
// 
//    Rev 1.76   03 Jul 1997 13:50:12   Fred
// Fixed redraw bug
// 
//    Rev 1.75   30 Jun 1997 11:55:12   Jay
// New stretching logic.
// 
//    Rev 1.74   30 May 1997 11:26:10   Fred
// Checks Txp valid
// 
//    Rev 1.73   29 May 1997 08:44:38   Fred
// Converts old title text boxes to frame objects
// 
//    Rev 1.72   28 May 1997 11:05:48   Fred
// Frame background effects
// 
//    Rev 1.71   27 May 1997 11:06:38   Fred
// Fixed 16-bit internal compiler error.
// 
//    Rev 1.70   15 May 1997 16:42:26   Fred
// Overflow prevention
// 
//    Rev 1.69   18 Apr 1997 17:09:54   Jay
// Sets primary action on read_data.
// 
//    Rev 1.68   14 Apr 1997 15:42:28   Jay
// Text box editing changes
// 
//    Rev 1.67   09 Apr 1997 14:51:08   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.66   26 Mar 1997 16:40:06   Fred
// Remove divide by zero dependency
// 
//    Rev 1.65   24 Mar 1997 13:35:54   Fred
// Fixed flipping bug
// 
//    Rev 1.64   21 Mar 1997 11:09:50   Jay
// Print preview changes.
// 
//    Rev 1.63   17 Mar 1997 16:08:32   Jay
// Now draws background in title text case.
// 
//    Rev 1.62   17 Mar 1997 14:48:06   Jay
// Warp shape palette and support.
// 
//    Rev 1.61   14 Mar 1997 09:47:32   Jay
//  
// 
//    Rev 1.60   13 Mar 1997 15:05:34   Fred
// Turned off some traces
// 
//    Rev 1.59   13 Mar 1997 12:02:34   Fred
// Support for rotated text flowaround
// 
//    Rev 1.58   12 Mar 1997 14:55:06   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.57   06 Mar 1997 13:22:04   Fred
// More text stuff
// 
//    Rev 1.56   05 Mar 1997 14:36:28   Fred
// More text object stuff
// 
//    Rev 1.55   26 Feb 1997 10:57:22   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.54   25 Feb 1997 12:57:08   Jay
// Cropping and extensions
// 
//    Rev 1.53   25 Feb 1997 10:55:16   Fred
//  
// 
//    Rev 1.52   19 Feb 1997 17:16:40   Jay
// ApplyClipRgn.
// 
//    Rev 1.51   11 Feb 1997 14:53:58   Jay
// 16-bit changes
// 
//    Rev 1.50   07 Feb 1997 08:46:42   Fred
// Removed debugging line
// 
//    Rev 1.49   07 Feb 1997 08:44:52   Fred
// Start of new text drawing pipeline
// 
//    Rev 1.48   29 Jan 1997 09:35:28   Fred
// Fixed bug in get_refresh_bounds()
// 
//    Rev 1.47   29 Jan 1997 09:12:22   Jay
// Removed CreateSelectPen. Added ObjectProperties routines.
// 
//    Rev 1.46   28 Jan 1997 16:56:38   Fred
// Start of component drawing
// 
//    Rev 1.45   24 Jan 1997 14:57:02   Fred
// Added some comments
// 
//    Rev 1.44   09 Jan 1997 08:58:04   Fred
// More text box changes
// 
//    Rev 1.43   08 Jan 1997 15:21:16   Fred
// More text box changes
// 
//    Rev 1.42   06 Jan 1997 10:15:22   Fred
// Start of new text object
// 
//    Rev 1.41   07 Nov 1996 15:56:46   johno
// Moved strings into resource DLL
// 
//    Rev 1.40   05 Nov 1996 14:06:48   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.39   28 Oct 1996 13:46:30   Jay
// Changes from 3.01 code
// 
//    Rev 1.39   25 Oct 1996 10:29:24   Jay
// Frame not on page support.
// 
//    Rev 1.38   15 Oct 1996 08:54:30   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.37   02 Oct 1996 08:32:36   Jay
// SetParagraphStyle
// 
//    Rev 1.36   11 Sep 1996 10:20:50   Jay
// get_refresh_bound() will now not add in refresh offsets for empty frames
// (which have obscenely largem, flipped refresh offsets).
// 
//    Rev 1.35   09 Sep 1996 11:16:04   Jay
// Minor correction to last fix.
// 
//    Rev 1.34   09 Sep 1996 11:05:44   Jay
// Breaks out of stretch loop if all text flows out of frame.
// 
//    Rev 1.33   03 Sep 1996 17:35:02   Jay
// Text flow with extents improvements.
// 
//    Rev 1.32   26 Aug 1996 14:31:14   Jay
// Fixed refresh bounds.
// 
//    Rev 1.31   08 Aug 1996 12:58:46   Jay
// Refresh bounds must be full-object : frame has outline now.
// 
//    Rev 1.30   02 Aug 1996 15:25:54   Jay
//  
// 
//    Rev 1.29   30 Jul 1996 10:56:04   Jay
// HandleCreationData() was resetting angle.
// 
//    Rev 1.28   29 Jul 1996 14:58:18   Jay
// Frame outline control
// 
//    Rev 1.27   26 Jul 1996 11:38:52   Jay
// Page stuff
// 
//    Rev 1.26   22 Jul 1996 18:07:24   Jay
// Tab code now works with tabs in columns.
// 
//    Rev 1.25   18 Jul 1996 11:10:08   Jay
// Better flow around objects.
// 
//    Rev 1.24   15 Jul 1996 17:45:58   Jay
//  
// 
//    Rev 1.23   15 Jul 1996 11:34:26   Jay
//  
// 
//    Rev 1.22   12 Jul 1996 18:00:34   Jay
// Got rid of an invalid assertion.
// 
//    Rev 1.21   12 Jul 1996 16:47:48   Jay
// Text wrap
// 
//    Rev 1.20   10 Jul 1996 10:23:48   Jay
// Text states no longer allocated.
// 
//    Rev 1.19   08 Jul 1996 17:51:28   Jay
// Linked frames, etc.
// 
//    Rev 1.18   08 Jul 1996 08:49:12   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.17   02 Jul 1996 08:55:02   Jay
//  
// 
//    Rev 1.16   28 Jun 1996 17:31:58   Jay
// OnPage() routine.
// 
//    Rev 1.15   28 Jun 1996 16:18:14   Jay
//  
// 
//    Rev 1.14   27 Jun 1996 17:20:34   Jay
//  
// 
//    Rev 1.13   27 Jun 1996 13:27:34   Jay
//  
// 
//    Rev 1.12   26 Jun 1996 14:21:36   Jay
//  
// 
//    Rev 1.11   26 Jun 1996 09:06:24   Jay
//  
// 
//    Rev 1.10   24 Jun 1996 18:06:10   Jay
//  
// 
//    Rev 1.9   21 Jun 1996 15:43:36   Jay
// Stretch flow tweaks.
// 
//    Rev 1.8   21 Jun 1996 14:30:22   Jay
// Stretch text, etc.
// 
//    Rev 1.7   20 Jun 1996 16:15:58   Jay
// Frame refresh offsets
// 
//    Rev 1.6   20 Jun 1996 14:59:46   Jay
//  
// 
//    Rev 1.5   19 Jun 1996 17:32:10   Jay
//  
// 
//    Rev 1.4   19 Jun 1996 13:45:48   Jay
//  
// 
//    Rev 1.3   13 Jun 1996 15:53:02   Jay
//  
// 
//    Rev 1.2   04 Jun 1996 17:37:52   Jay
//  
// 
//    Rev 1.1   24 May 1996 09:26:54   Jay
// New text code.
// 
//    Rev 1.0   14 Mar 1996 13:42:18   Jay
// Initial revision.
// 
//    Rev 1.1   27 Feb 1996 17:05:24   VSI
// _NEWPORTABLE 'C' code replaces .asm -- not yet working
// 
//    Rev 1.0   16 Feb 1996 12:07:34   FRED
// Initial revision.
// 
//    Rev 2.18   20 Dec 1995 09:52:36   JAY
// 32-bit fix.
// 
//    Rev 2.17   07 Sep 1995 15:48:30   JAY
// Special code to skip the toggle for off-screen generation.
// 
//    Rev 2.16   06 Aug 1995 12:08:24   JAY
// Now handles "ystretch" refreshes correctly.
// 
//    Rev 2.15   06 Aug 1995 11:22:56   JAY
// Now handles new page backgrounds better during special-case drawing.
// 
//    Rev 2.14   07 Jul 1995 16:10:50   JAY
// Misc changes.
// 
//    Rev 2.13   17 May 1995 14:25:20   FRED
// Warp text objec refresh bounds.
// 
// 
//    Rev 2.12   02 May 1995 15:37:52   JAY
// Improved macro support.
// 
//    Rev 2.11   01 May 1995 15:24:48   JAY
// Restructured the macro stuff
// 
//    Rev 2.10   28 Apr 1995 17:26:32   JAY
// No longer creates separate macro words.
// 
//    Rev 2.9   27 Apr 1995 13:20:22   FRED
// 
//    Rev 2.8   25 Apr 1995 15:23:24   JAY
// Added margins to the frame object.
// 
//    Rev 2.7   25 Apr 1995 08:52:06   JAY
// Added y-stretch capabilities.
// 
//    Rev 2.6   20 Apr 1995 16:15:50   FRED
// Warp Text Stuff
// 
//    Rev 2.5   19 Apr 1995 16:04:48   JAY
// Removed some symbols.
// 
//    Rev 2.4   19 Apr 1995 12:31:32   JAY
//  
// 
//    Rev 2.3   29 Mar 1995 08:59:04   JAY
// Lots of changes
// 
//    Rev 2.2   15 Mar 1995 10:41:04   JAY
// Support for new text editing
// 
//    Rev 2.1   15 Feb 1995 17:03:10   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:48   JAY
// Initial revision.
// 
//    Rev 1.36   30 Jan 1995 09:16:16   JAY
// Now uses attrib DC of RC when necessary.
// 
//    Rev 1.35   30 Nov 1994 16:34:48   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.34   04 Aug 1994 07:33:28   JAY
// Large model and 32-bit changes
// 
//    Rev 1.33   01 Jun 1994 16:35:32   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.32   18 Apr 1994 14:52:50   JAY
// Fixed the "Finger" problem in the x-flipped case.
// 
//    Rev 1.31   11 Apr 1994 11:32:36   JAY
// Fixed a bug where it was tryng to update something outside the clipped
// frame bounds (pbox was an invalid PBOX). This was causing it to try and
// allocate a Bitmap with a negative height (resulting in a GlobalAlloc failure).
// 
//    Rev 1.30   07 Apr 1994 10:48:24   JAY
// Fixed a problem in computing word_rect which caused the "chopped pointing
// finger" problem.
//
//    Rev 1.29   18 Mar 1994 10:34:40   JAY
// Re-instated part of the fix I removed last rev.
// 
//    Rev 1.28   18 Mar 1994 09:27:30   JAY
// Re-instated some old (supposedly working) code.
// 
//    Rev 1.27   17 Mar 1994 17:25:46   JAY
// Another attempt to get rid of cursor glitches.
// 
//    Rev 1.26   17 Mar 1994 16:46:02   JAY
// No longer attempts to draw toggle marks in the off-screen bitmap since it
// doesn't seem to work.
// 
//    Rev 1.25   17 Mar 1994 14:33:16   JAY
// Won't become "opaque" until the edit background is built.
// 
//    Rev 1.24   17 Mar 1994 09:04:42   JAY
// Limits refresh bound to viewable area during edit.
// 
//    Rev 1.23   16 Mar 1994 16:02:22   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.22   15 Mar 1994 13:22:58   JAY
// Support for edit background.
// 
//    Rev 1.21   09 Mar 1994 11:28:54   JAY
// For colored text on palette devices, the palette is selected into the
// compatible DC.
// 
//    Rev 1.20   28 Feb 1994 14:31:18   JAY
// Now clips object_bound to the refresh extent passed in (so that draw_string()
// will know when it can clip characters).
// 
//    Rev 1.19   25 Feb 1994 13:34:54   MACDUFF
// Quit offscreen drawing after 1/2 second.
// 
//    Rev 1.18   24 Feb 1994 14:43:20   JAY
// Some changes from good friend Macduff.
// If text is black and white, will draw using a monochrome bitmap.
// Will attempt to draw the toggle marks in the offscreen bitmap instead of to
// the screen when possible.
// 
//    Rev 1.17   17 Feb 1994 09:25:56   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// Added a m_editing variable, and set_editing() in preparation for special
// frame editing code.
// 
//    Rev 1.16   16 Feb 1994 11:10:26   MACDUFF
// Does not do toggles on offscreen bitmap.
// 
//    Rev 1.15   16 Feb 1994 10:47:42   MACDUFF
// Fixed another problem with offscreen drawing.
// 
//    Rev 1.14   16 Feb 1994 09:06:44   MACDUFF
// 
//    Rev 1.13   13 Feb 1994 15:23:50   MACDUFF
// Checks paragraph modification count when restarting an
// interrupted refresh/.
// 
//    Rev 1.12   11 Feb 1994 16:32:02   MACDUFF
// Restored offscreen drawing, which had been temporarily disabled for
// debugging porpoises.
// 
//    Rev 1.11   11 Feb 1994 16:17:08   MACDUFF
// Fixed logic error in offscreen update.
// 
//    Rev 1.10   11 Feb 1994 14:48:48   MACDUFF
// Frame updates can go to offscreen bitmap if refresh type is REDRAW.
// 
//    Rev 1.9   11 Feb 1994 08:59:42   MACDUFF
// Removed unnecessary toggle_selects in is_screen_white_in
// 
//    Rev 1.8   09 Feb 1994 16:50:40   MACDUFF
// New param to draw_string()
// 
//    Rev 1.7   07 Feb 1994 15:59:54   MACDUFF
// Do toggle at word redisplay level.
// 
//    Rev 1.6   03 Feb 1994 12:45:54   JAY
// Removed a warning.
// 
//    Rev 1.5   03 Feb 1994 10:07:58   JAY
// Now does the white check from the screen.
// 
//    Rev 1.4   28 Jan 1994 17:11:06   JAY
//  
// 
//    Rev 1.3   19 Jan 1994 17:07:08   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:44   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:06:42   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:16   JAY
// 
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pmgdb.h"
#include "action.h"

#include "pararec.h"
#include "textrec.h"
#include "framerec.h"
#include "frameobj.h"

#include "textflow.h"
#include "styleit.h"
#include "fontsrv.h"
#include "pmgfont.h"
#include "macrosrv.h"
#include "warpfld.h"
#include "warpobj.h"

#include "utils.h"
#include "htmldoc.h"    // For HTMLTag classes
//#define  JTRACE_ON
#include "jtrace.h"

#include "SymbolGridCtrl.h" // for kSymbolFont

#define WRAP_TIGHT	0.0625
#define WRAP_MEDIUM	0.125
#define WRAP_LOOSE	0.1875

// Bullet drawing helper functions for converting an integer to a character atring
void GetRomanNumeral( CHARACTER *p, int number );
void GetNumberedLetter( CHARACTER *p,int number );
// Array used to convert arabic numbers to roman
char level[3][3] = {	{'I', 'V', 'X'},
						{'X', 'L', 'C'},
						{'C', 'D', 'M'}	};
//////////////
// function GetRomanNumeral()
// converts the number passed into a string representation of a roman numeral 
// written to the string passed. null terminates the string
// Used by numbered lists
// NOTE: does not support numbers 1000 or greater
void GetRomanNumeral(
		 CHARACTER *pString, // array to fll with Roman number character
		 int number )		// number to convert
{

	char string[10];	
	_itoa(  number, string, 10 );			// convert the number to a string
	int nlen = strlen( string );
	if (nlen > 2 )
	{
		return;
	}
	int index = nlen;
	CHARACTER *p = pString;
	for( int i = 0; i < nlen; i++ )			// parse the string, converting each digit to Roman 
	{
		int digit = string[i]; 
		--index;

		switch( digit )
		{
			case '3':
				*p++ = level[index][0];
			case '2':
				*p++ = level[index][0];
			case '1':
				*p++ = level[index][0];
				break;
			case '4':
				*p++ = level[index][0];
			case '5':
				*p++ = level[index][1];
				break;
			case '6': 
				*p++ = level[index][1];
				*p++ = level[index][0];
				break;
			case '7':
				*p++ = level[index][1];
				*p++ = level[index][0];
				*p++ = level[index][0];
				break;
			case '8':
				*p++ = level[index][1];
				*p++ = level[index][0];
				*p++ = level[index][0];
				*p++ = level[index][0];
				break;
			case '9':
				*p++ = level[index][0];
				*p++ = level[index][2];
				break;
		}
	}
	*p = '\0';
}

//////////////
// function GetNumberedLetter()
// converts the number passed into a string representation of the Letter  
// written to the string passed. null terminates the string.
// Used by numbered lists
void GetNumberedLetter( 
		 CHARACTER *p,	// array to fll with Roman number character
		 int num )		// number to convert

{
	int numLetters = num / 26;
	int c = num %  26;
	// if there is no remainder 
	if( c == 0 )
		c = 26;			// the character is Z
	else				// if there is a remainder
		numLetters++;	// there is one more character in the string
	c += 64;
	for( int i = 0;  i < numLetters; i++ )
		*p++ = c;
	*p = '\0';
}
/*
// The asm code below was converted to C code by Roger House 96/02/22
//
// mem_test()
//
// Test whether the first "count" bytes of a buffer all have the 
//    value "val".  Return 1 if they do, 0 otherwise.
//
// Restriction: count must be even.
*/

PRIVATE BOOL near
mem_test(LPBYTE buf, uint16 count, BYTE val)
{
#ifdef WIN32

#ifdef _NEWPORTABLE

   ASSERT((count & 1) == 0);  // Make sure count is even

   while (count--)
      if (*buf++ != val)
         return(0);
   return(1);

#else // _NEWPORTABLE
   
   BOOL rtn;

   _asm
   {
      mov   edi, buf
      movzx ecx, count
      mov   al, val

      mov   ah, al
      shr   ecx, 1
      repe  scasw

      mov   eax, 0
      jne   l_done
      inc   eax
   l_done:
      mov   rtn, eax
   }
   return rtn;

#endif   // else _NEWPORTABLE

#else // WIN32

   BOOL rtn;

   _asm
   {
      les   di, buf
      mov   cx, count
      mov   al, val

      mov   ah, al
      shr   cx, 1
      repe  scasw

      mov   ax, 0
      jne   l_done
      inc   ax
   l_done:
      mov   rtn, ax
   }
   return rtn;

#endif   // else WIN32
} // end mem_test

PRIVATE BOOL near is_screen_white_in(RedisplayContextPtr rc, RECT r)
{
   int width = r.right - r.left;
   int height = r.bottom - r.top;
   int bm_byte_width = ((width + 7)/8 + 1) & ~1;
   int bm_width = bm_byte_width*8;
   DWORD bytes = (DWORD)bm_byte_width * height;
   BOOL ret = FALSE;

   if (bytes < 0x0FF00L)
   {
      HDC hdc = rc->destination_hdc;
      LPBYTE mem = NULL;

		TRY
			mem = new BYTE[bytes];
		END_TRY
      if (mem != NULL)
      {
         HDC bm_hdc;
         if ((bm_hdc = ::CreateCompatibleDC(rc->AttributeDC())) != NULL)
         {
            HBITMAP hBitmap;
            if ((hBitmap = ::CreateBitmap(bm_width, height, 1, 1, NULL)) != NULL)
            {
               HBITMAP hOldBitmap;

               if ((hOldBitmap = (HBITMAP)::SelectObject(bm_hdc, hBitmap)) != NULL)
               {
                  COLORREF old_textcolor = ::SetTextColor(hdc, RGB(0, 0, 0));
                  COLORREF old_bkcolor = ::SetBkColor(hdc, RGB(255, 255, 255));

               /* Clear the bitmap to white (especially the right edge). */
                  ::PatBlt(bm_hdc, 0, 0, bm_width, height, WHITENESS);
                  ::BitBlt(bm_hdc, 0, 0, width, height,
                           hdc, r.left, r.top, SRCCOPY);

                  ::SetBkColor(hdc, old_bkcolor);
                  ::SetTextColor(hdc, old_textcolor);

                  ::SelectObject(bm_hdc, hOldBitmap);

               /* Extract the bitmap bits. */

                  ::GetBitmapBits(hBitmap, bytes, mem);

               /* Do the test! */

                  ret = mem_test(mem, (uint16)bytes, (BYTE)0xff);
               }

               ::DeleteObject(hBitmap);
            }
            ::DeleteDC(bm_hdc);
         }
			delete [] mem; 
      }
   }
// od("ret = %d\r\n", ret);
   return ret;
}

static BOOL compute_lower_intersection(PMGPageObjectPtr object, PBOX_PTR extent, PBOX_PTR clear_pbox)
{
   PMGPageObjectPtr o;
   BOOL need_to_clear = FALSE;

   clear_pbox->x0 = clear_pbox->y0 = 0x7FFFFFFFL;
   clear_pbox->x1 = clear_pbox->y1 = 0x80000000L;

/* See if we overlap objects underneath us. */

   while (object != NULL)
   {
      o = object;

      while ((o = (PMGPageObjectPtr)o->previous_object()) != NULL)
      {
         PBOX int_pbox;
         PBOX bound;

         o->get_refresh_bound(&bound);

      /*
      // Compute the intersection of this bound and our extent.
      // If there is an intersection, add it to the clear extent.
      */

         if (IntersectBox(&int_pbox, &bound, extent))
         {
         /* This object touches our extent. Add it to what we have. */

            UnionBox(clear_pbox, clear_pbox, &int_pbox);
            need_to_clear = TRUE;
         }
      }
      object = object->get_parent();
   }

   return need_to_clear;
}

/////////////////////////////////////////////////////////////////////////////
// CFrameObject

/*
// The creator for a (new) frame object.
*/

ERRORCODE CFrameObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
   CFrameObject* pObject = NULL;
   ERRORCODE error = ERRORCODE_None;

/* Create the frame object. */

   if ((pObject = new CFrameObject(type, owner)) == NULL)
   {
      error = ERRORCODE_Memory;
   }
   else
   {
   /* We created the object. */
      if ((error = pObject->HandleCreationData(creation_data)) != ERRORCODE_None)
      {
         delete pObject;
         pObject = NULL;
      }
   }

   *record = (PageObjectPtr)pObject;
   return error;
}

/*
// The constructor for the frame object.
*/

CFrameObject::CFrameObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
      : RectPageObject(type, owner)
{
   record.select_flags = SELECT_FLAG_boundary
                       | SELECT_FLAG_handles;

/* We have no record yet. (The "clean slate" principle.) */

   memset(&m_FRecord, 0, sizeof(m_FRecord));

/* Are we editing yet? */

   m_fEditing = FALSE;
	m_hbmEditBackground = NULL;

//	record.primary_action = ACTION_DEFAULT_EDIT;
}

/*
// Handle any creation data.
// Use by this and derived classes.
*/

ERRORCODE CFrameObject::HandleCreationData(LPVOID pData)
{
   ERRORCODE error = ERRORCODE_None;

   if (pData != NULL)
   {
      PMGDatabasePtr pDatabase = (PMGDatabasePtr)database;
      FRAMEOBJ_CREATE_STRUCT_PTR fcs = (FRAMEOBJ_CREATE_STRUCT_PTR)pData;

   /* Try to make the frame. */

      CFrameRecord* pFrame;

      if ((pFrame = (CFrameRecord*)pDatabase->new_record(RECORD_TYPE_Frame, NULL)) != NULL)
      {
         // Save the frame record number.
         FrameRecord(pFrame->Id());

         // Set the flags.
         pFrame->FrameFlags((DWORD)fcs->flags);

         // Set the vertical alignment.
         pFrame->VerticalAlignment(fcs->vert_alignment);

         // We are a part of this page.
         pFrame->Page(pDatabase->CurrentPage());
         pFrame->Margins().Left = PageToInches(fcs->LeftMargin);
         pFrame->Margins().Top = PageToInches(fcs->TopMargin);
         pFrame->Margins().Right = PageToInches(fcs->RightMargin);
         pFrame->Margins().Bottom = PageToInches(fcs->BottomMargin);

			// set the text flow gutter
			pFrame->SetGutter( fcs->m_eGutter );
      /* Release the frame record. */

         pFrame->release(TRUE);

         if ((error = CreateTextRecord()) == ERRORCODE_None)
         {
            /*
            // Success!
            */
            // Set the bound. 
            set_unrotated_bound(fcs->bound);

            BOOL fOnPage = m_fOnPage;
            m_fOnPage = FALSE;
            calc();
            m_fOnPage = fOnPage;
         }
         else
         {
         // Free the frame record we created.
            pDatabase->delete_record(FrameRecord(), RECORD_TYPE_Frame);
            FrameRecord(0);
         }

			// Copy over effects.
			pFrame->Fill() = fcs->m_Fill;
			pFrame->Outline() = fcs->m_Outline;
			pFrame->Shadow() = fcs->m_Shadow;
      }
      else
      {
      /*
      // Failed!
      */
         error = pDatabase->last_creation_error();
      }
   }

   return error;
}

/*
// Create the text record.
// This is used by the create method as well as when frames are unlinked
// from another text stream.
*/

ERRORCODE CFrameObject::CreateTextRecord(void)
{
   ERRORCODE error = ERRORCODE_None;

   if (FrameRecord() != 0)
   {
      CFrameRecord* pFrame = (CFrameRecord*)database->get_record(FrameRecord(), &error, RECORD_TYPE_Frame);
      if (pFrame != NULL)
      {
         error = pFrame->CreateTextRecord();
         pFrame->release();
      }
   }
   return error;
}

/////////////////////////////////////////////////////////////////////////////
// Stretch methods
/*
// Defines for stretch_text.
*/

#define  MIN_EXPANSION        (MakeFixed(.005))
#define  MAX_EXPANSION        (MakeFixed(15))
#define  MAX_STRETCH_SIZE     1000

ERRORCODE CFrameObject::StretchText(BOOL fFlowingMacros /*=FALSE*/, CWordArray* pMacros /*=NULL*/)
{
/*
// Stretching text has the following steps:
//
// (1) Size all text to the original size and expansion (100%).
// (2) Compute the bounding extent of this text.
// (3) Calculate the adjusted size and expansion to make the text fit in
//     bound of the frame.
// (4) Resize the text to these new parameters.
//
// Repeat these steps until the error is within acceptable limits to
// a maximum of five iterations.
*/

   ERRORCODE error = ERRORCODE_None;

   CFrameRecord* pFrame = NULL;
   CTextRecord* pText = NULL;

   TRY
   {
   /*
   // Step 1: Size all text to a reference size and expansion.
   */
      // Lock the frame.
      if ((pFrame = (CFrameRecord*)database->get_record(FrameRecord(), &error, RECORD_TYPE_Frame)) == NULL)
      {
         ThrowErrorcodeException(error);
      }

      // Lock the text.
      if ((pText = (CTextRecord*)database->get_record(pFrame->TextRecord(), &error, RECORD_TYPE_Text)) == NULL)
      {
         ThrowErrorcodeException(error);
      }

   /* Extract pertinent information from the frame. */

      PPNT OriginalDims = pFrame->StretchDims();
      PPNT CurrentDims;
      PBOX Bound = get_unrotated_bound();

      CurrentDims.x = Bound.x1 - Bound.x0;
      CurrentDims.y = Bound.y1 - Bound.y0;

      if (pFrame->FrameFlags() & FRAME_FLAG_stretch_frame)
      {
         OriginalDims.x = CurrentDims.x;
      }

      /* Tentatively, max y error is 1/128th of the frame height. */
      PCOORD MaxYError = CurrentDims.y >> 7;

      /* Tentatively, max x error is 1/128 of frame width */
      PCOORD MaxXError = CurrentDims.x >> 7;

      const CFixedRect& Margins = pFrame->Margins();

      PCOORD Delta = MulFixed(Margins.Left + Margins.Right, PAGE_RESOLUTION);
      CurrentDims.x -= Delta;
      OriginalDims.x -= Delta;

      Delta = MulFixed(Margins.Top + Margins.Bottom, PAGE_RESOLUTION);
      CurrentDims.y -= Delta;
      OriginalDims.y -= Delta;

      /*
      // Shrink the target dimensions to make sure stretched
      // text can never clip.
      */
#if 1
      CurrentDims.x -= MaxXError;
      CurrentDims.y -= MaxYError;
#endif

      if (CurrentDims.x <= 0)
      {
         CurrentDims.x = 1;
      }

      if (CurrentDims.y <= 0)
      {
         CurrentDims.y = 1;
      }

   /* Set the reference size and expansion. */

      CFixed lReferenceNumerator = MakeFixed(1);
      CFixed lReferenceDenominator = MakeFixed(1);
      CFixed lReferenceExpansion = MakeFixed(1);

      pText->SetStretch(lReferenceNumerator,
                        lReferenceDenominator,
                        lReferenceExpansion);

      int nShift = 0;

      CFixed lStretchNumerator = lReferenceNumerator;
      CFixed lStretchDenominator = lReferenceDenominator;
      CFixed lStretchExpansion = lReferenceExpansion;

      /* Loop until error is acceptable */
      int nIteration = 0;
      for (;;)
      {
#if 0
      /* Scale up the reference size so that we will have a reasonable amount
         of precision */

         if (reference_size == 0)
         {
         /* Sanity! */
            reference_size = 1;
         }
         while (reference_size < 1000)
         {
            reference_size <<= 1;
            ++shift;
         }
         while (reference_size > 10000)
         {
            reference_size >>= 1;
            --shift;
         }
#endif
      /*
      // Step 2: Compute the bounding extent of this text.
      */
         PPNT TextExtent;

         ComputeFlowBounds(pFrame, pText, OriginalDims, &TextExtent, nIteration == 0, fFlowingMacros, pMacros);

         if (TextExtent.y == 0)
         {
            // No text fit. Abort this.
            break;
         }

         if (++nIteration > 5)
         {
            break;
         }

      /*
      // Step 3: Calculate the adjusted size and expansion to make the text fit in
      //         bound of the frame.
      */

         if (TextExtent.y <= 0)
         {
            TextExtent.y = 1;
         }

#ifdef DEBUG_ST
         JTRACE("Frame (%ld, %ld) Text (%ld, %ld)\n", CurrentDims, TextExtent);
#endif

         /* See how well we did */

         PCOORD XErr = labs(CurrentDims.x - TextExtent.x);
         PCOORD YErr = labs(CurrentDims.y - TextExtent.y);

#ifdef DEBUG_ST
         JTRACE("YErr = %ld Max = %ld XErr = %ld Max = %ld\n",
               YErr,
               MaxYError,
               XErr,
               MaxXError);
#endif
         if (YErr <= MaxYError)
         {
            if (XErr <= MaxXError ||
                (TextExtent.x < CurrentDims.x && lReferenceExpansion == MAX_EXPANSION) ||
                (TextExtent.x > CurrentDims.x && lReferenceExpansion == MIN_EXPANSION))
            {
               break;
            }
         }

         /* NOTE: if Y value was acceptable we do NOT recompute it, so as to
         // allow X to converge more rapidly.
         */

         /* Compute new size and new expansion as needed */
         if (YErr > MaxYError)
         {
//          unsigned long max_size = ((unsigned long)MAX_STRETCH_SIZE) << shift;

            lStretchNumerator =  MulDivFixed(lStretchNumerator,
                                             CurrentDims.y,
                                             TextExtent.y);
            if (lStretchNumerator == 0)
            {
               lStretchNumerator = 1;
            }

//          if (stretch_size > max_size)
//          {
//             stretch_size = max_size;
//          }
         }
         else
         {
            lStretchNumerator = lReferenceNumerator;
            lStretchDenominator = lReferenceDenominator;
         }

         if (TextExtent.x <= 0)
         {
         /* No text! */
            lStretchExpansion = MakeFixed(1);
         }
         else
         {
            lStretchExpansion = MulDivFixed(lReferenceExpansion, CurrentDims.x, TextExtent.x);
            lStretchExpansion = MulDivFixed(lStretchExpansion, lReferenceNumerator, lStretchNumerator);
            lStretchExpansion = MulDivFixed(lStretchExpansion, lStretchDenominator, lReferenceDenominator);

#ifdef DEBUG_ST
            JTRACE("Stretch %ld/%ld, expansion %ld\n",
                  lStretchNumerator,
                  lStretchDenominator,
                  lStretchExpansion);
#endif

            if (lStretchExpansion > MAX_EXPANSION)
            {
               lStretchExpansion = MAX_EXPANSION;
            }
            else if (lStretchExpansion < MIN_EXPANSION)
            {
               lStretchExpansion = MIN_EXPANSION;
            }
         }

         pText->SetStretch(lStretchNumerator, lStretchDenominator, lStretchExpansion);

         lReferenceNumerator = lStretchNumerator;
         lReferenceDenominator = lStretchDenominator;
         lReferenceExpansion = lStretchExpansion;
      }
#ifdef DEBUG_ST
      if (nIteration > 2) JTRACE("nIterations = %d\n", nIteration - 1);
#endif
   }
   CATCH(CErrorcodeException, e)
   {
      error = e->m_error;
   }
   AND_CATCH(CMemoryException, e)
   {
      error = ERRORCODE_Memory;
   }
   AND_CATCH_ALL(e)
   {
      error = ERRORCODE_IntError;
   }
   END_CATCH_ALL
   
   if (pFrame != NULL)
   {
      pFrame->release();
   }
   if (pText != NULL)
   {
      pText->release();
   }
   return error;
}

void CFrameObject::ComputeFlowBounds(CFrameRecord* pFrame, CTextRecord* pText, PPNT OriginalDims, PPNT* pTextExtent, BOOL fDoFlow, BOOL fFlowingMacros /*=FALSE*/, CWordArray* pMacros /*=NULL*/)
{
   // Flow the text with the appropriate stretch mode.
   CTextFlow TextFlow(this, 0, -1);
   TextFlow.SetStretchMode(fDoFlow
                              ? CTextFlow::STRETCH_MODE_Flow
                              : CTextFlow::STRETCH_MODE_Size);
   TextFlow.SetStretchDims(pFrame->StretchDims());
   TextFlow.FlowingMacros(fFlowingMacros, pMacros);
   TextFlow.FlowText();

   CTextIterator Iterator;

   TRY
   {
      Iterator.SetRecord(pText);

      // Compute the text extent.
      LINE_COUNT lLines = pFrame->NumberOfLines();
      LINE_COUNT lThisLines = 0;
      LINE_INDEX lLine = 0;
      CTextLine* pLine = NULL;

      WORD_COUNT lThisWords = 0;
      WORD_INDEX lWord = 0;
      CTextWord* pWord = NULL;

      CHARACTER_INDEX lCharacter = 0;

      PCOORD Y0 = 0, Y1 = 0;
      PCOORD XMax = 0;

      while (lLines-- > 0)
      {
         // Prime the line pointer if we need to.
         if (lThisLines == 0)
         {
            pLine = pFrame->GetLine(lLine, &lThisLines);
         }

         // Compute the line y extent.
         PCOORD Top = pLine->m_Baseline - pLine->m_Ascend;
         PCOORD Bottom = pLine->m_Baseline + pLine->m_Descend;

         // Compute the line x extent.
         // This is the right edge of the last solid word.

         Iterator.SetPosition(lCharacter);

         CFixed lPadding = Iterator.Style().LeftMargin() + Iterator.Style().RightMargin();
         if (Iterator.CharacterInParagraph() == 0)
         {
            lPadding += Iterator.Style().FirstLineIndent();
         }
/// added CES if it's a bulleted list and not on the first list add in bullet indent
        if (Iterator.Style().BulletType() != BULLET_none && Iterator.CharacterInParagraph() != 0)
        {
           lPadding += Iterator.Style().BulletIndent();
        }

         PCOORD XOffset = MulFixed(lPadding, PAGE_RESOLUTION);
         PCOORD SolidXOffset = XOffset;
         WORD_COUNT lWords = pLine->m_lWords;
         while (lWords-- > 0)
         {
            // Prime the word if we need to.
            if (lThisWords == 0)
            {
               pWord = pFrame->GetWord(lWord, &lThisWords);
				if (!pWord || lThisWords <= 0) //j
					break; //j
            }

            XOffset += pWord->m_Width;
            if (pWord->m_wType == WORD_TYPE_solid)
            {
               SolidXOffset = XOffset;
            }

            pWord++;
            lThisWords--;
            lWord++;
         }
         if (lLine == 0)
         {
            Y0 = Top;
            Y1 = Bottom;
            XMax = SolidXOffset;
         }
         else
         {
            if (Top < Y0)
            {
               Y0 = Top;
            }
            if (Bottom > Y1)
            {
               Y1 = Bottom;
            }
            if (SolidXOffset > XMax)
            {
               XMax = SolidXOffset;
            }
         }
         lCharacter += pLine->m_lCharacters;
         lThisLines--;
         pLine++;
         lLine++;
      }

      // Set the return size.
      pTextExtent->x = XMax;
      pTextExtent->y = Y1 - Y0;
   }
   CATCH_ALL(e)
   {
      TextFlow.Cleanup();
      Iterator.Reset();
      THROW_LAST();
   }
   END_CATCH_ALL
}

/////////////////////////////////////////////////////////////////////////////
// Storage methods

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE CFrameObject::read_data(StorageDevice* pDevice)
{
   ERRORCODE error;

   if ((error = RectPageObject::read_data(pDevice)) == ERRORCODE_None)
   {
      if (my_type == OBJECT_TYPE_OldFrame
       || my_type == OBJECT_TYPE_OldWarpText
       || my_type == OBJECT_TYPE_OldCalendarDate)
      {
         // Read the old data.

         FrameObjectRecord frecord;

         if ((error = pDevice->read_record(&frecord, sizeof(&frecord))) == ERRORCODE_None)
         {
            // Convert the frame record.
            // The frame object record contains all we need, so we pass
            // that to the conversion routine.
            // Note that we ignore any errors. This will attempt to load as
            // much as possible, even if the data is messed up.

            ST_DEV_POSITION Here;
            pDevice->tell(&Here);

            ConvertFrameRecord(&frecord);

            pDevice->seek(Here, ST_DEV_SEEK_SET);

            // Convert the type for next time.
            if (my_type == OBJECT_TYPE_OldFrame)
            {
               my_type = OBJECT_TYPE_Frame;
            }
            else if (my_type == OBJECT_TYPE_OldWarpText)
            {
               my_type = OBJECT_TYPE_WarpText;
            }
            else
            {
               ASSERT(my_type == OBJECT_TYPE_OldCalendarDate);
               my_type = OBJECT_TYPE_CalendarDate;
            }
         }
      }
      else
      {
         // Read the new data.
         error = pDevice->read_record(&m_FRecord, sizeof(m_FRecord));
      }

		if (((get_select_flags() & SELECT_FLAG_size_handles) != 0)
		 && ((get_select_flags() & SELECT_FLAG_rotate_handle) == 0))
		{
			add_select_flags(SELECT_FLAG_rotate_handle);
		}

		if (my_type == OBJECT_TYPE_WarpText)
		{
			my_type = OBJECT_TYPE_Frame;
			WarpTextObjectRecord WarpRecord;
			error = pDevice->read_record(&WarpRecord, sizeof(WarpRecord));
			if (error == ERRORCODE_None)
			{
            ST_DEV_POSITION Here;
            pDevice->tell(&Here);

				CFrameRecord* pFrameRecord = LockFrameRecord();

				if (pFrameRecord != NULL)
				{
					if (WarpRecord.m_WarpField != 0)
					{
						pFrameRecord->set_new_warp_field(WarpRecord.m_WarpField);
					}

					if (WarpRecord.m_fBackgroundFillColorExists)
					{
						pFrameRecord->Fill().SetType(FillFormatV1::FillSolid, WarpRecord.m_clBackgroundFillColor);
					}
					else
					{
						pFrameRecord->Fill().SetType(FillFormatV1::FillNone);
					}

					if ((WarpRecord.m_nBackgroundLineWidth >= 0) && WarpRecord.m_fBackgroundLineColorExists)
					{
						pFrameRecord->Outline().SetType(OutlineFormat::OutlineAbsolute, MakeFixed(WarpRecord.m_nBackgroundLineWidth));
						pFrameRecord->Outline().SetColor(FillFormatV1::FillSolid, WarpRecord.m_clBackgroundLineColor);
					}
					else
					{
						pFrameRecord->Outline().SetType(OutlineFormat::OutlineNone);
					}

					if (WarpRecord.m_fBackgroundShadowColorExists)
					{
						pFrameRecord->Shadow().SetType(ShadowFormat::ShadowDropDownRight, MakeFixed(OBJECT_SHADOW_FRACTION));
						pFrameRecord->Shadow().SetColor(FillFormatV1::FillSolid, WarpRecord.m_clBackgroundShadowColor);
					}
					else
					{
						pFrameRecord->Shadow().SetType(ShadowFormat::ShadowNone);
					}

					pFrameRecord->release(TRUE);
					pFrameRecord = NULL;
				}

				CTxp Txp(get_database());
				Txp.Init(this, 0);
				if (Txp.Valid())
				{
					Txp.SelectAll();
               Txp.CalcStyle();
					if (Txp.SomethingSelected())
					{
						CTextStyle Style = Txp.SelectStyle();

						if (WarpRecord.m_fTextFillColorExists)
						{
							Style.FillSetType(FillFormatV1::FillSolid, WarpRecord.m_clTextFillColor);
						}
						else
						{
							Style.FillSetType(FillFormatV1::FillNone);
						}

						if ((WarpRecord.m_nTextLineWidth >= 0) && WarpRecord.m_fTextLineColorExists)
						{
							Style.OutlineSetType(OutlineFormat::OutlineAbsolute, MakeFixed(WarpRecord.m_nTextLineWidth));
							Style.OutlineSetColor(FillFormatV1::FillSolid, WarpRecord.m_clTextLineColor);
						}
						else
						{
							Style.OutlineSetType(OutlineFormat::OutlineNone);
						}

						if (WarpRecord.m_fTextShadowColorExists)
						{
							Style.ShadowSetType(ShadowFormat::ShadowDropDownRight);
							Style.ShadowSetColor(FillFormatV1::FillSolid, WarpRecord.m_clTextShadowColor);
						}
						else
						{
							Style.ShadowSetType(ShadowFormat::ShadowNone);
						}

						// Change whole range.
						CHARACTER_INDEX lStart, lEnd;
						Txp.GetRange(&lStart, &lEnd);

						Txp.Off();

						CTextRecord* pText = Txp.LockText();
						ASSERT(pText != NULL);

						if (pText->ChangeRangeStyle(lStart, lEnd, &Style) > 0)
						{
							// Finish by showing the changed style.
							Txp.UpdateRange();
						}

						pText->release();
						pText = NULL;

						Txp.SetTransition();
						Txp.On();
					}
				}

            pDevice->seek(Here, ST_DEV_SEEK_SET);
			}
		}

		// We want our primary action to be default edit now.
		record.primary_action = ACTION_DEFAULT_EDIT;
   }
   return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE CFrameObject::write_data(StorageDevice* pDevice)
{
   ERRORCODE error;

   if ((error = RectPageObject::write_data(pDevice)) == ERRORCODE_None)
   {
      error = pDevice->write_record(&m_FRecord, sizeof(m_FRecord));
   }
   return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE CFrameObject::size_data(StorageDevice* pDevice)
{
   return RectPageObject::size_data(pDevice) + pDevice->size_record(sizeof(m_FRecord));
}

/*
// The object is being destroyed. Destroy all resources.
*/

void CFrameObject::destroy(void)
{
   DestroyFrameAndText();

   RectPageObject::destroy();
}

void CFrameObject::DestroyFrameAndText(void)
{
   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      BOOL fDeleteIt = FALSE;
      CTextRecord* pText = LockTextRecord();
      if (pText != NULL)
      {
         if (pFrame->PreviousFrame() == 0
               && pText->FirstFrame() == pFrame->Id())
         {
            fDeleteIt = TRUE;
         }
         pText->release();
      }

      if (fDeleteIt)
      {
         pFrame->DeleteTextRecord();
      }
      pFrame->zap();
   }
   FrameRecord(0);
}

/*
// Set the style for a paragraph in the text in this frame.
// This is used in special cases (like calendar, banner, etc).
*/

void CFrameObject::SetTextStyle(const CTextStyle* pStyle)
{
   CTextRecord* pText = LockTextRecord();
   if (pText != NULL)
   {
      if (pText->NumberOfCharacters() == 0)
      {
         // Set the base paragraph style
         pText->SetParagraphStyle(0, pStyle);
      }
      else
      {
         // Set all the text to this style.
         pText->ChangeRangeStyle(-1, -1, pStyle);
      }
      pText->release();
   }

   // Reflow the text to accomodate the new size.
   CTextFlow TextFlow(this, 0, -1);
   TextFlow.FlowText();
}

/*
// Set the style for a paragraph in this frame.
*/

void CFrameObject::SetParagraphStyle(const CTextStyle* pStyle)
{
   CTextRecord* pText = LockTextRecord();
   if (pText != NULL)
   {
      pText->ChangeParagraphStyle(-1, -1, pStyle);
      pText->release();
   }

   // Reflow the text to accomodate the new style.
   CTextFlow TextFlow(this, 0, -1);
   TextFlow.FlowText();
}

/*
// Toggle select marks for an object.
*/

BOOL CFrameObject::toggle_selects(RedisplayContextPtr rc, FLAGS which)
{
   return RectPageObject::toggle_selects(rc, which);
}

/*
// Get an object's rotation.
*/

HCURSOR CFrameObject::GetContentCursor(void)
{
	if (m_fEditing)
		return ::LoadCursor(NULL, IDC_IBEAM);

	return RectPageObject::GetContentCursor();
}

/*
// Return whether a point is over an object's move area or not.
*/

BOOL CFrameObject::pt_on_move_area(PPNT p, RedisplayContextPtr rc)
{
	BOOL fInBound = FALSE;


	POINT sp;
	rc->ppnt_to_screen(p, &sp);

	PBOX Box;
	rc->mouse_box(sp, &Box);

	// Not in our regular bound.
	// See if we are on the edge.

	if (record.rotc.rotation != 0.0)
	{
   /*
   // We have a rotated bound.
   // Rotate the box into our rotated space. Then compare to our
   // unrotated bound.
   */
		compute_rotated_bound(Box, -record.rotc.rotation, &Box, record.rotc.origin);
	}

	return IsBoxOnBox(&Box, &rrecord.unrotated_bound);
}

void CFrameObject::calc(PBOX_PTR panel_world, FLAGS panel_flags)
{
   RectPageObject::calc(panel_world, panel_flags);

   PBOX bound = get_unrotated_bound();

/* Update the frame record. */

   DB_RECORD_NUMBER lFrameRecord = FrameRecord();
   CFrameRecord* pFrame;

   if ((pFrame = (CFrameRecord*)database->get_record(lFrameRecord, NULL, RECORD_TYPE_Frame)) != NULL)
   {
      PBOX RecBounds = pFrame->ObjectBounds();
      WORD wOldFlags = pFrame->ObjectFlags();
      BOOL fDoFlow;
      BOOL fMoved;
      CHARACTER_INDEX lFirst = 0, lLast = -1;      // non-optimized defaults.

      if (get_flags() & OBJECT_FLAG_calc_ignore)
      {
      /* This is a special calendar case. */
         bound.x1 = bound.x0 + (RecBounds.x1 - RecBounds.x0);
         bound.y1 = bound.y0 + (RecBounds.y1 - RecBounds.y0);
         fDoFlow = FALSE;
         fMoved = FALSE;
      }
      else
      {
         fDoFlow = (RecBounds.x1 - RecBounds.x0 != bound.x1 - bound.x0)
                || (RecBounds.y1 - RecBounds.y0 != bound.y1 - bound.y0)
                || (wOldFlags != get_flags());
         lFirst = pFrame->FirstCharacter();
         lLast = lFirst + pFrame->NumberOfCharacters();
         fMoved = (RecBounds.x0 != bound.x0)
               || (RecBounds.y0 != bound.y0);
      }

      pFrame->ObjectFlags(get_flags());
      pFrame->ObjectBounds(bound);

      if (ComputeIntersections())
      {
         // Intersections changes. We have already been flowed.
         // Do not need to flow again.
         fDoFlow = FALSE;
      }
      else
      {
         if (!fDoFlow && fMoved && pFrame->Intersections().GetElementCount() != 0)
         {
            // We have intersections (that did not change), but we moved.
            // This requires a reflow.
            fDoFlow = TRUE;
         }
      }

   /* Release the record. */

      pFrame->release(TRUE);

      if (fDoFlow)
      {
      /* It changed. We probably want to do a text flow here. */
         if (lFirst != -1)
         {
            CTextFlow Textflow(this, lFirst, lLast);
            Textflow.FlowText();

            JTRACE("Flow status = %d, error = %d\n",
                  Textflow.GetStatus(), Textflow.GetError());
         }
      }
   }
}

/*
// Flip an object left to right.
*/

void CFrameObject::xflip(BOOL fChangeAngle /*=TRUE*/)
{
   INHERITED::xflip(fChangeAngle);
	// Always calc!
   calc();
}

/*
// Flip an object top to bottom.
*/

void CFrameObject::yflip(BOOL fChangeAngle /*=TRUE*/)
{
   INHERITED::yflip(fChangeAngle);
	// Always calc!
   calc();
}

/*
// The method for updating a frame.
*/

UpdateStatePtr CFrameObject::update(
   RedisplayContextPtr rc,
   PBOX_PTR extent,
   LPRECT clip,
   UpdateStatePtr ustate,
   REFRESH_TYPE refresh_type)
{
   // rc = redisplay context
   // extent = full refresh extent intersected with source pbox (client rectangle)
   //          and then clipped to card panel if project is card and printing
   // clip = the clipping rectangle in screen coordinates (actually client window
   //        coordinates). The rectangle passed in by DrawObjects in PMG 4.0 is always
   //        the screen coordinate version of the PBOX rectangle in "extent".
   // ustate = pointer to current update state
   // refresh_type = the refresh type
   //
   //    REFRESH_ALL
   //
   //       Do a full refresh - our caller, DrawObjects(), has done the erase and
   //       painted any objects underneath us. We just draw our characters and
   //       frame outline (our foreground.)
   //
   //    REFRESH_ERASE
   //
   //       Erase a portion of the frame contents. We can assume that DrawObjects()
   //       has not drawn anything underneath us, either because there are no objects
   //       beneath us in the refresh extent, or because we have an edit background
   //       containing an image of all the lower objects. Since we are just erasing,
   //       we do not have to call UpdateFrame() which is going to spend time
   //       traversing the line and word arrays when there is nothing to draw.
   //
   //    REFRESH_REDRAW
   //
   //       Erase and redraw a portion of the frame contents. We can assume that
   //       DrawObjects() has not drawn anything underneath us, either because
   //       there are no objects beneath us in the refresh extent, or because we
   //       have an edit background containing an image of all the lower objects.
   //
   // Note: It's easy to overlook the *hidden* mechanism that makes some of this stuff
   // work. DrawObjects() searches for an opaque object which covers the entire
   // refresh extent. If it finds such an object, it STARTS drawing with that object.
   // The frame object reports itself to be opaque (CFrameObject::is_opaque) if it has
   // a background bitmap (ASSUMPTION: the editing background bitmap encloses any
   // refresh extents generated for the frame object.) So, the opaque test is what
   // tells DrawObjects() if the frame object has the capability to draw the objects
   // behind it (using its background bitmap.)

	ASSERT(refresh_type != REFRESH_DRAW);		// *** no longer supported ***

   CUpdateState* pUpdateState = (CUpdateState*)ustate;

   // Offscreen drawing variables.
   HDC hdcSave = NULL;
   HDC hdcMem = NULL;
   HBITMAP hbmNew = NULL;
   HBITMAP hbmSave = NULL;
   HRGN hrgnClipSave = NULL;
   int nOffscreenWidth, nOffscreenHeight;
   int nOffscreenXOffset, nOffscreenYOffset;
   RECT rectDest;
   RECT rectClipSave;
	
	RedisplayContext* pInRC = rc;
	RedisplayContext OffscreenRC;

   // Update sidebar.
   if (rc->want_update_names())
   {
      rc->update_name(this, GET_PMWAPP()->GetResourceStringPointer(IDS_TEXT_BOX)); 
   }

   // Compute the warp field to use.
   CWarpField* pWarpField = GetWarpField();

   // Remember if the frame object is warped or rotated.
   BOOL fWarped = pWarpField != NULL;
   BOOL fRotated = get_rotation() != 0.0;

   // Compute the extent for updating (put it in pbox).
   PBOX pbox;
   get_refresh_bound(&pbox, rc);
   PBOX object_bound = get_bound();

#if 0
   JTRACE("Update Type %d Frame(%d,%d,%d,%d) Refresh(%d,%d,%d,%d) Update(%d,%d,%d,%d)\n",
      refresh_type,
      object_bound,
      pbox,
      *extent);
#endif

   // object_bound = full normalized extent of the object (with rotation)
   // pbox = refresh bound of object
   // extent = full refresh extent intersected with source pbox (client rectangle)
   //          and then clipped to card panel if project is card and printing
   // clip = the clipping rectangle in screen coordinates (actually client window
   //        coordinates). The rectangle passed in by DrawObjects() in PMG 4.0 is
   //        always the screen coordinate version of the PBOX rectangle in "extent".

   // PMGTODO: Document the conditions in which the extent is NULL.
   if (extent != NULL)
   {
      // If we are not doing a REFRESH_ALL, then we must be doing a REFRESH_ERASE
      // or REFRESH_REDRAW. In this case we do not clip horizontally since the frame
      // size may have changed in such a way that the refresh extent lies outside of
      // the new refresh bounds. One example: editing in the banner text box causes
      // the frame size to change.
      if (refresh_type != REFRESH_ALL)
      {
         // REFRESH_ERASE, REFRESH_REDRAW

         // Clip vertically but not horizontally because we could have an erase
         // extent for text that used to be outside our current bounds.

         pbox.x0 = extent->x0;
         pbox.x1 = extent->x1;

         // Clip to object bounds.
         pbox.y0 = __max(object_bound.y0, extent->y0);
         pbox.y1 = __min(object_bound.y1, extent->y1);

         // pbox has the full width of the refresh extent (not refresh bound)
         // intersected with the source pbox (client rectangle). pbox has the
         // height of the object bound clipped to the refresh extent (only
         // rows actually in the object are in pbox.)
         //
         // PMGTODO: should the refresh extent be used instead of the object extent?
      }
      else
      {
         // REFRESH_ALL

         // Clip to refresh bounds.
         pbox.x0 = __max(pbox.x0, extent->x0);
         pbox.x1 = __min(pbox.x1, extent->x1);
         pbox.y0 = __max(pbox.y0, extent->y0);
         pbox.y1 = __min(pbox.y1, extent->y1);

         // pbox has the refresh bounds clipped to the refresh extent
         // intersected with the source pbox (client rectangle). Thus,
         // pbox contains only area which is inside the the refresh bounds,
         // the refresh extent, and the source pbox.
      }
   }
   else
   {
      // No refresh extent was passed in (when does this happen?)
      // In this case, we use the union of the refresh bounds (pbox)
      // and the object bounds (object_bound).
      //
      // NOTE: Normally, the refresh bounds completely contains the
      // object bounds, is there a case where this isn't true, or is
      // this union a "just-in-case" kind of thing.
      //
      // PMGTODO: Document the conditions in which the extent is NULL.
      UnionBox(&pbox, &pbox, &object_bound);
   }

   // pbox = area to refresh (see above code for how pbox is computed.)

   // If the pbox is clipped out, it may mean that the extent was outside our
   // bound. This can happen. We just ignore this and go home.
   //
   // NOTE: DrawObjects() (this main function which calls us) normally filters
   // out these kinds of empty updates. It is common?

   if ((pbox.x0 >= pbox.x1)
    || (pbox.y0 >= pbox.y1))
   {
      ASSERT(ustate == NULL);    // This shouldn't be able to happen.
      delete ustate;             // Handle it anyway...

      return NULL;
   }

   // See if we really need to clear.
   // Only clear if:
   //
   // 1) Refresh type is not REFRESH_ERASE (there will be some drawing.)
   // 2) We aren't continuing an update which wasn't clearing.
   // 3) We are on top of other objects within the refresh extent.

   // NOTE: compute_lower_intersection() computes the union of all
   // the intersections between the refresh area (pbox) and the refresh
   // bounds of objects beneath this frame object (us).

   BOOL fNeedToClear = FALSE;
   PBOX clear_pbox;

#if 0
   if (refresh_type != REFRESH_ERASE)
   {
      if (pUpdateState == NULL)
      {
         // Not continuing.
         fNeedToClear = compute_lower_intersection(this, &pbox, &clear_pbox);
      }
      else
      {
         // Continuing a previous update.
         if (pUpdateState->m_fClearing)
         {
            // Compute the clear extent.
            fNeedToClear = compute_lower_intersection(this, &pbox, &clear_pbox);

            if (!fNeedToClear)
            {
               // We were clearing, now we don't want to. Skip the clear and
               // fall through to the normal text draw.
               delete pUpdateState;
               pUpdateState = NULL;
            }
         }
      }
   }
#endif

   // Figure out whether we should use an offscreen bitmap.
   // One should be used if:
   //
   //    1) Refresh type is not REFRESH_ALL (is REFRESH_ERASE or REFRESH_REDRAW.)
   //    2) We not restarting a previously interrupted update.
   //    3) We're updating to the screen (we're not printing).

   BOOL fUseOffscreen = FALSE;
	BOOL fSaveSkipToggle = FALSE;

   if (refresh_type != REFRESH_ALL)
   {
      // Since the refresh type is not REFRESH_ALL, we will be doing some
      // erasing inside this block.

		// Only use and offscreen bitmap if we're drawing to the
		// screen and we are not restarting after an interrupt.
      fUseOffscreen = (pUpdateState == NULL) && rc->IsScreen();

      // Compute the rectangle in screen coordinates of the area
      // which will be refreshed (pbox).
      PBOX p = pbox;
      rc->pbox_to_screen(&p, TRUE);
      rc->convert_pbox(&p, &rectDest, NULL);

      // Compute dimensions of offscreen bitmap.
      nOffscreenWidth = rectDest.right-rectDest.left;
      nOffscreenHeight = rectDest.bottom-rectDest.top;

      // For now, assume we're not doing an offscreen bitmap and set the
		// offsets to zero.
      nOffscreenXOffset = 0;
      nOffscreenYOffset = 0;

      // If we to want use an offscreen bitmap, set it up.
      if (fUseOffscreen)
      {
         // We are going to try to use an offscreen bitmap for
         // refreshing the frame.

         // Create a DC for the offscreen bitmap.
         hdcMem = ::CreateCompatibleDC(rc->AttributeDC());
         if (hdcMem != NULL)
         {
            // Create the offscreen bitmap.
            hbmNew = CreateCompatibleBitmap(rc->AttributeDC(), nOffscreenWidth, nOffscreenHeight);
            if (rc->info.type == RASTER_TYPE_PALETTE)
            {
               // We need to copy the palette over. First, get it from the destination
               // DC by selecting in the default palette (temporarily).
               HPALETTE hPal = ::SelectPalette(rc->destination_hdc, (HPALETTE)::GetStockObject(DEFAULT_PALETTE), FALSE);
               if (hPal != NULL)
               {
                  // Got the palette. Select it back.
                  ::SelectPalette(rc->destination_hdc, hPal, FALSE);

                  // And select it into our source hdc.
                  ::SelectPalette(hdcMem, hPal, FALSE);
               }
            }
         }

         // We should now have the offscreen DC and bitmap.
         if ((hdcMem != NULL) && (hbmNew != NULL))
         {
            // We have an offscreen bitmap, so we need to adjust the drawing
            // coordinates to match the bitmap instead of the client area.

				// Compute the translation offsets that when added to a screen/client coordinate
				// will give the coordinate in the offscreen bitmap.
				nOffscreenXOffset = -rectDest.left;
				nOffscreenYOffset = -rectDest.top;

				// Switch to the offscreen RC.
				OffscreenRC = *rc;
				rc = &OffscreenRC;

            // Offset the "destination" coordinate information.
            rc->destination_x0 += nOffscreenXOffset;
            rc->destination_y0 += nOffscreenYOffset;
            OffsetRect(&rc->destination_rect, nOffscreenXOffset, nOffscreenYOffset);

            // Set up the new destination DC.
            hdcSave = rc->destination_hdc;
            rc->destination_hdc = hdcMem;

            // Set the new clipping region and rectangle.
            hrgnClipSave = rc->clip_region;
            rc->clip_region = NULL;
            rectClipSave = rc->clip_rect;
            SetRect(&rc->clip_rect, 0, 0, nOffscreenWidth, nOffscreenHeight);

            // Select the offscreen bitmap into the bitmap DC.
            hbmSave = (HBITMAP)::SelectObject(hdcMem, hbmNew);

            // Set the "is printing" flag. This may seem strange (and it is), but
            // it prevents other code from doing things which work on the screen but
            // not with an offscreen bitmap. No, I don't know exactly what they are.
            // PMGTODO: Document what setting is_printing to TRUE does.
            rc->is_printing = TRUE;

				// Set the "skip toggle" flag.
				// PMGTODO: This is basically a kludge that is necessary since we
				// are taking over the rc to use for the offscreen bitmap. Perhaps
				// we should allocate an "offscreen" rc?
				fSaveSkipToggle = rc->m_fSkipToggle;
				rc->m_fSkipToggle = TRUE;
         }
         else
         {
            // Well, we wanted to use an offscreen bitmap, but something went wrong
            // and we couldn't get a DC or create the bitmap. So, we simply switch back
            // to the non-offscreen case.
            fUseOffscreen = FALSE;
         }
      }

      // The refresh type is REFRESH_ERASE or REFRESH_REDRAW, so we're responsible
      // for erasing. If there's a background bitmap, use it.

		if (pUpdateState == NULL)
		{
			BOOL fDrew = FALSE;     // in case something fails

			if (m_hbmEditBackground != NULL)
			{
				// Create a DC for the background bitmap.
				HDC hdcBack;
				if ((hdcBack = ::CreateCompatibleDC(rc->AttributeDC())) != NULL)
				{
					// Select in the background bitmap.
					HBITMAP hOldBitmap;
					if ((hOldBitmap = (HBITMAP)::SelectObject(hdcBack, m_hbmEditBackground)) != NULL)
					{
						// Copy the background bitmap onto the offscreen bitmap.
						// This code assumes that the edit background bitmap
						// encloses all of the possible refresh areas that we
						// may have.
						//
						// NOTE: m_rEdit is set in set_edit_background() and has the
						// screen coordinates of the rectangle bounding the edit
						// background bitmap.
						BitBlt(
							rc->destination_hdc,
							rectDest.left+nOffscreenXOffset,
							rectDest.top+nOffscreenYOffset,
							nOffscreenWidth,
							nOffscreenHeight,
							hdcBack,
							rectDest.left-m_rEdit.left,
							rectDest.top-m_rEdit.top,
							SRCCOPY);

						// The background bitmap did not include any "toggles", so we have to turn
						// them on now to match the rest of the screen.
						rc->toggle(TRUE, &rectDest);

						fDrew = TRUE;

						::SelectObject(hdcBack, hOldBitmap);
					}

					::DeleteDC(hdcBack);
				}
			}

			else
			{
				// Do the clear.
				if (rc->m_pDesktop != NULL)
				{
					CRect crBackground(rectDest);
					if (fUseOffscreen)
					{
						crBackground.OffsetRect(-rectDest.left, -rectDest.top);
					}
					rc->m_pDesktop->DrawBackground(crBackground, rc);
					fDrew = TRUE;
				}
			}

			// If, for some reason, we could not draw with the background bitmap,
			// then an error has occurred... just fill with white.
			if (!fDrew)
			{
				PatBlt(
					rc->destination_hdc,
					rectDest.left+nOffscreenXOffset,
					rectDest.top+nOffscreenYOffset,
					nOffscreenWidth,
					nOffscreenHeight,
					WHITENESS);
			}
		}
   }

	if (pUpdateState == NULL)
	{
		DrawBackground(rc);
	}

   // If the refresh type is not REFRESH_ERASE, then we have to draw something.
   if (refresh_type != REFRESH_ERASE)
   {
      // Draw the text.
      pUpdateState = (CUpdateState*)UpdateFrame(rc, &pbox, &clear_pbox, fNeedToClear, pUpdateState, fUseOffscreen);
   }

   // If we were using an offscreen bitmap, then we need to copy the bitmap
   // to the destination DC and restore the RC items that we saved.
   if (fUseOffscreen)
   {
      // Restore the destination coordinate information.
      rc->destination_x0 -= nOffscreenXOffset;
      rc->destination_y0 -= nOffscreenYOffset;
      OffsetRect(&rc->destination_rect, -nOffscreenXOffset, -nOffscreenYOffset);

      // Restore the destination DC.
      rc->destination_hdc = hdcSave;

      // Restore the destination clip region and rectangle.
      rc->clip_region = hrgnClipSave;
      rc->clip_rect = rectClipSave;

      // We set this to TRUE and now we know we can set it to FALSE because
      // offscreen bitmaps can only be used for the screen.
      rc->is_printing = FALSE;

		// Set back the toggle.
		rc->m_fSkipToggle = fSaveSkipToggle;

		// Switch back to the originalRC.
		rc = pInRC;

      // Copy the offscreen bitmap to the screen!
      BitBlt(
         rc->destination_hdc,
         rectDest.left,
         rectDest.top,
         nOffscreenWidth,
         nOffscreenHeight,
         hdcMem,
         0, 0,
         SRCCOPY);

      // The offscreen bitmap did not include any "toggles", so we have to turn
      // them on now to match the rest of the screen.
      rc->toggle(TRUE, &rectDest);
   }

   // Free any resources allocated for the offscreen bitmap.
   if (hdcMem != NULL)
   {
      if (hbmNew != NULL)
      {
         SelectObject(hdcMem, hbmSave);
         DeleteObject(hbmNew);
         hbmNew = NULL;
      }
      DeleteDC(hdcMem);
      hdcMem = NULL;
   }

   // If we finished without without interrupting, draw the frame outline.
   if (pUpdateState == NULL)
   {
      DrawFrameOutline(rc);
   }

   // Return our update state (NULL if finished successfully.)
   return (UpdateStatePtr)pUpdateState;
}

// Draw the frame outline. This includes the line around the content
// and the column markers.

void CFrameObject::DrawFrameOutline(RedisplayContextPtr rc)
{
   if (GenericFrame()
    && (get_select_flags() & SELECT_FLAG_boundary)
    && rc->IsScreen()
    && !rc->is_metafile
    && !rc->is_print_preview
    && get_database()->ShowFrameOutlines()
    && GET_PMWAPP()->ShowTextBoxOutlines())
   {
      HDC hDC = rc->destination_hdc;

      // First, draw the frame rectangle. This is drawn one pixel outside
      // of the content area.

      // Get the unrotated bound and move the upper left corner out one
      // pixel. We don't have to move the lower right corner out because
      // Lineto() is used to draw the frame and it automatically will draw
      // to the right and below the computed coordinates (unlike Rectangle).

      PBOX UnrotatedBound = get_unrotated_bound();
      UnrotatedBound.x0 -= rc->redisplay_x_pixel;
      UnrotatedBound.y0 -= rc->redisplay_y_pixel;

      // Initialize v[] with the coordinates of the rotated corners.
      PBOX RotatedBound;
      PPNT v[4];
      compute_rotated_bound(UnrotatedBound, get_rotation(), &RotatedBound, get_origin(), v);

      // Convert the PPNT coordinates to screen coordinates in p[]. Along the way
      // we accumulate the bounding rectangle which will be used for the toggle.
      POINT p[4];
      RECT r;
      int i;

      for (i = 0; i < 4; i++)
      {
         // Convert point to screen coordinates.
         p[i].x = rc->page_x_to_screen(v[i].x);
         p[i].y = rc->page_y_to_screen(v[i].y);

         // Accumulate point into bounding rectangle.
         if (i == 0)
         {
            r.left = r.right = p[i].x;
            r.top = r.bottom = p[i].y;
         }
         else
         {
            if (p[i].x < r.left) r.left = p[i].x;
            if (p[i].x > r.right) r.right = p[i].x;
            if (p[i].y < r.top) r.top = p[i].y;
            if (p[i].y > r.bottom) r.bottom = p[i].y;
         }
      }

      // Bump out the lower right corner of the toggle rectangle since the frame
      // will be drawn with LineTo which draws to the right and below the computed
      // coordinates.
      r.right++;
      r.bottom++;

      // Draw the frame using a gray "dotted" pen.
      HPEN hPen = ::CreatePen(PS_DOT, 1, RGB(192, 192, 192));
      if (hPen != 0)
      {
         HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
         if (hOldPen != NULL)
         {
            // Erase any toggles.
            rc->toggle(FALSE, &r);

            // Draw opaque.
            int oldROP2 = SetROP2(hDC, R2_COPYPEN);
            int oldBkMode = SetBkMode(hDC, OPAQUE);

            // Draw the frame.
            ::MoveToEx(hDC, p[0].x, p[0].y, NULL);
            ::LineTo(hDC, p[1].x, p[1].y);
            ::LineTo(hDC, p[2].x, p[2].y);
            ::LineTo(hDC, p[3].x, p[3].y);
            ::LineTo(hDC, p[0].x, p[0].y);

            // Draw the columns.
            CFrameRecord* pFrame;

            // Column information is in frame record.
            if ((pFrame = LockFrameRecord()) != NULL)
            {
               int nColumns;
               PCOORD ColumnSpacing;
               PCOORD ColumnWidth;

               // Get unrotated column positions.
               UnrotatedBound = get_unrotated_bound();
               pFrame->GetColumnData(&nColumns, &ColumnSpacing, &ColumnWidth, &UnrotatedBound);

               if (nColumns > 1)
               {
                  // For now, we just draw nColumns-1 lines.
                  // This is considerably less clutter than showing all
                  // the margins. Maybe if we use another color...

                  PCOORD ColumnLeft = UnrotatedBound.x0;
                  
                  // Draw each column.
                  for (int nColumn = 1; nColumn < nColumns; nColumn++)
                  {
                     ColumnLeft += ColumnWidth;

                     // Compute rotated coordinates of column marker.
                     v[0].x = ColumnLeft;
                     v[0].y = UnrotatedBound.y0;
                     v[1].x = ColumnLeft;
                     v[1].y = UnrotatedBound.y1;
                     rotate_points(v, 2, get_origin(), get_rotation());

                     // Convert to screen coordinates.
                     for (i = 0; i < 2; i++)
                     {
                        p[i].x = rc->page_x_to_screen(v[i].x);
                        p[i].y = rc->page_y_to_screen(v[i].y);
                     }
                     
                     // Draw the column marker.
                     ::MoveToEx(hDC, p[0].x, p[0].y, NULL);
                     ::LineTo(hDC, p[1].x, p[1].y);
                  }
               }

               // All done with the frame record.
               pFrame->release();
            }

            // Restore old DC settings.
            SetBkMode(hDC, oldBkMode);
            SetROP2(hDC, oldROP2);
            ::SelectObject(hDC, hOldPen);

            // Turn any toggles back on.
            rc->toggle(TRUE, &r);
         }

         ::DeleteObject(hPen);
      }
   }
}

BOOL CFrameObject::DrawBackground(RedisplayContextPtr rc)
{
	CFrameRecord* pFrameRecord = LockFrameRecord();
	if (pFrameRecord != NULL)
	{
		if (pFrameRecord->Fill().IsVisible()
		 || pFrameRecord->Outline().IsVisible()
		 || pFrameRecord->Shadow().IsVisible())
		{
			PCOORD lX, lY;
			pFrameRecord->GetOutlineWidth(&lX, &lY);

			int nX = rc->page_x_to_screen(lX)-rc->page_x_to_screen(0);
			int nY = rc->page_y_to_screen(lY)-rc->page_y_to_screen(0);

			int nWidth = nX;
			if (nY > nWidth)
			{
				nWidth = nY;
			}

			PBOX UnrotatedBounds = get_unrotated_bound();
			rc->pbox_to_screen(&UnrotatedBounds, TRUE);

			if (nWidth <= 1)
			{
				UnrotatedBounds.x1--;
				UnrotatedBounds.y1--;
			}

			CRect crBounds;
			if (rc->convert_pbox(&UnrotatedBounds, crBounds, NULL))
			{
				COutlinePath Path;

				CWarpField* pWarpField = GetWarpField();

				if (pWarpField != NULL)
				{
					pWarpField->Transform(crBounds);
					pWarpField->TransformedBounds()->Path(&Path);
				}
				else
				{
					Path.Free();
					Path.MoveTo(MakeFixed(crBounds.left), MakeFixed(crBounds.top));
					Path.LineTo(MakeFixed(crBounds.right), MakeFixed(crBounds.top));
					Path.LineTo(MakeFixed(crBounds.right), MakeFixed(crBounds.bottom));
					Path.LineTo(MakeFixed(crBounds.left), MakeFixed(crBounds.bottom));
					Path.LineTo(MakeFixed(crBounds.left), MakeFixed(crBounds.top));
					Path.Close();
					Path.End();
				}

				BOOL fRotated = get_rotation() != 0.0;
				BOOL fFlipped = (get_flags() & (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)) != 0;
				BOOL fNegateInflate = FALSE;
				COutlinePathMatrix RotateMatrix;

				RotateMatrix.Identity();
		
				if (fFlipped)
				{
					if (get_flags() & OBJECT_FLAG_xflipped)
					{
						RotateMatrix.XFlip(MakeFixed(crBounds.left), MakeFixed(crBounds.right));
						fNegateInflate = !fNegateInflate;
					}
					if (get_flags() & OBJECT_FLAG_yflipped)
					{
						RotateMatrix.YFlip(MakeFixed(crBounds.top), MakeFixed(crBounds.bottom));
						fNegateInflate = !fNegateInflate;
					}
				}
				if (fRotated)
				{
					POINT Origin;
					rc->ppnt_to_screen(get_origin(), &Origin);
					RotateMatrix.TranslateBy(-MakeFixed(Origin.x), -MakeFixed(Origin.y));
					RotateMatrix.RotateBy(MakeFixed(get_rotation()*RAD2DEGC));
					RotateMatrix.TranslateBy(MakeFixed(Origin.x), MakeFixed(Origin.y));
				}

				if (nWidth < 0)
				{
					nWidth = COutlinePath::NoLine;
				}
				else
				{
					if (fNegateInflate)
					{
						nWidth = -nWidth;
					}
				}

				CFixedRect PathBounds;
				Path.QuickBoundingBox(&PathBounds);

				COutlinePath ShadowPath;

				if (pFrameRecord->Shadow().IsVisible())
				{
					COutlinePathMatrix ShadowMatrix;
			
					// Get height and width in units of X resolution.
	
					PCOORD ShadowX, ShadowY;
					pFrameRecord->GetShadowOffsets(&ShadowX, &ShadowY);
					
					// Build the matrix.
			
					ShadowMatrix.Identity();
					ShadowMatrix.TranslateBy(
						MakeFixed((int)(rc->page_x_to_screen(ShadowX)-rc->page_x_to_screen(0))),
						MakeFixed((int)(rc->page_y_to_screen(ShadowY)-rc->page_y_to_screen(0))));
				
					// Build the shadow path.
			
					ShadowPath.Copy(&Path);
					ShadowPath.Transform(&ShadowMatrix);
			
					if (fRotated || fFlipped)
					{
						ShadowPath.Transform(&RotateMatrix);
					}

					CFixedRect ShadowBounds;
					ShadowPath.QuickBoundingBox(&ShadowBounds);

					if (ShadowBounds.Left < PathBounds.Left)
					{
						PathBounds.Left = ShadowBounds.Left;
					}
					if (ShadowBounds.Top < PathBounds.Top)
					{
						PathBounds.Top = ShadowBounds.Top;
					}
					if (ShadowBounds.Right > PathBounds.Right)
					{
						PathBounds.Right = ShadowBounds.Right;
					}
					if (ShadowBounds.Bottom > PathBounds.Bottom)
					{
						PathBounds.Bottom = ShadowBounds.Bottom;
					}
				}

				CRect crToggle;
				crToggle.left = FixedInteger(PathBounds.Left);
				crToggle.top = FixedInteger(PathBounds.Top);
				crToggle.right = FixedInteger(PathBounds.Right+0x0000ffffL);
				crToggle.bottom = FixedInteger(PathBounds.Bottom+0x0000ffffL);
				rc->toggle(FALSE, crToggle);

				HDC hdc = rc->destination_hdc;
				CDC* pDC = CDC::FromHandle(hdc);
				BOOL fUsePalette = (rc->info.type == RASTER_TYPE_PALETTE);

				// Build a matrix for making the shadow.

				if (pFrameRecord->Shadow().IsVisible())
				{
					// Draw the shadow.
						
					if (pFrameRecord->Fill().IsVisible())
					{
						ShadowPath.Draw2(
							pDC,
							colorref_from_color(pFrameRecord->Shadow().m_ForegroundColor),
							pFrameRecord->Shadow().IsVisible(),
							COutlinePath::NoLine,
							RGB(0,0,0),
							FALSE,
							ALTERNATE,
							fUsePalette);
					}
					else
					{
						ShadowPath.Draw2(
							pDC,
							RGB(0,0,0),
							FALSE,
							nWidth,
							colorref_from_color(pFrameRecord->Shadow().m_ForegroundColor),
							pFrameRecord->Shadow().IsVisible(),
							ALTERNATE,
							fUsePalette);
					}
				}

				if (fRotated || fFlipped)
				{
					Path.Transform(&RotateMatrix);
				}
				
				Path.Draw2(
					pDC,
					colorref_from_color(pFrameRecord->Fill().m_ForegroundColor),
					pFrameRecord->Fill().IsVisible(),
					nWidth,
					colorref_from_color(pFrameRecord->Outline().m_ForegroundColor),
					pFrameRecord->Outline().IsVisible(),
					WINDING,
					fUsePalette);

				rc->toggle(TRUE, crToggle);
			}
		}

		pFrameRecord->release();
		pFrameRecord = NULL;
	}

	return TRUE;
}

/*
// Is this frame a generic text frame (where "generic" means a frame
// create by the user which behaves in a normal fashion - not special
// banner or calendar frames)?
*/

BOOL CFrameObject::GenericFrame(void)
{
   return (get_panel() < MAIN_CALENDAR_PANEL)
         && !(get_select_flags() & SELECT_FLAG_solo_select);
}

// IsHTMLCompliant returns TRUE if this object can be converted to HTML
// without losing its current attributes.  We may want to specify which
// version of HTML to check for compliance at some point.
BOOL CFrameObject::IsHTMLTextCompliant(CHTMLCompliantInfo *pCompliantInfo)
{
   BOOL              bFirstParagraph = TRUE;
   int               nNumColumns;
   short int         nAlignment;
   FLAGS             flagsObject;
   ANGLE             angleObject;
   CFrameRecord      *pFrameRecord;
   CTextRecord       *pTextRecord;
   CFrameLocation    frameLocation;
   ERRORCODE         errorcode;
   FillFormatV1      fillFormat;
   OutlineFormat     outlineFormat;
   ShadowFormat      shadowFormat;
   CTextIterator     textIterator;
   CTextStyle        textStyle;
   CWarpField        *pWarpField;
   CHARACTER_INDEX   lFirstCharacterInFrame, charIndex, charIndexLastVisibleChar=-1;
   CHARACTER_COUNT   lLastCharacterInFrame, lFrameCharacterCount;
   PBOX              pboxFrameBounds;
   PCOORD            pcoordLineTop, pcoordLineBottom, pcoordLineCenter;
   CHTMLCompliantInfo  compliantInfo;
/*
   if(pCompliantInfo)
      pCompliantInfo->Empty(); // reset to default state
   else
      pCompliantInfo = &compliantInfo;
*/
   if(pCompliantInfo == NULL)
      pCompliantInfo = &compliantInfo;

	TRY
	{
      // If object has is flipped, text is not HTML compliant
      flagsObject = get_flags();
      if((flagsObject & OBJECT_FLAG_xflipped) || (flagsObject & OBJECT_FLAG_yflipped))
      {
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectFlipped);
      }
      // If frame is rotated, not HTML compliant
      angleObject = get_rotation();
      if(angleObject != 0)
      {
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectRotated);
      }
      // Check frame for special properties that are not HTML compliant
      pFrameRecord = LockFrameRecord(&errorcode);
      ASSERT(pFrameRecord);
      if(pFrameRecord == NULL || errorcode != ERRORCODE_None)
         return FALSE;

      lFirstCharacterInFrame = pFrameRecord->FirstCharacter();
      lLastCharacterInFrame = lFirstCharacterInFrame + pFrameRecord->NumberOfCharacters();
      lFrameCharacterCount = pFrameRecord->NumberOfCharacters();
      if(lFrameCharacterCount > 0)
         lLastCharacterInFrame--;

      fillFormat = pFrameRecord->Fill();
      outlineFormat = pFrameRecord->Outline();
      shadowFormat = pFrameRecord->Shadow();

      pWarpField = pFrameRecord->GetWarpField();

      if(pWarpField)
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectWarped);

      if(fillFormat.IsVisible())
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectFilled);

      if(outlineFormat.IsVisible())
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectHasOutline);

      if(shadowFormat.IsVisible())
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectHasShadow);

      nNumColumns = pFrameRecord->Columns();
      if(nNumColumns > 1)
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectHasColumns);
      // Check for empty frame
      if(lFrameCharacterCount <= 0)
      {
         pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctObjectIsEmpty);
         pFrameRecord->release();
         return TRUE;   // Nothing else left to check
      }
      // Check text attributes for non-HTML compliant attributes
      pTextRecord = LockTextRecord(&errorcode);
      ASSERT(pTextRecord);
      if(pTextRecord == NULL || errorcode != ERRORCODE_None)
      {
         pFrameRecord->release();
         return FALSE;
      }

      textIterator.SetRecord(pTextRecord);
      textIterator.Style().Frame(this);

      pboxFrameBounds = pFrameRecord->ObjectBounds();
      // Loop through paragraphs and check for compliance
      charIndex = lFirstCharacterInFrame;
      while(charIndex >= 0 && charIndex <= lLastCharacterInFrame)
      {
         textIterator.SetPosition(charIndex);
         textStyle = textIterator.Style();
         // Get position information about 1st line is paragraph for checking if
         // line is getting clipped.  If it is getting clipped, then we are done with checking paragraphs
         frameLocation.m_lCharacter = charIndex;
         pFrameRecord->GetCharacterLocation(&frameLocation);
         pcoordLineTop = frameLocation.m_Line.m_Baseline - frameLocation.m_Line.m_Ascend;
         pcoordLineBottom = frameLocation.m_Line.m_Baseline + frameLocation.m_Line.m_Descend;
         pcoordLineCenter = pcoordLineTop + ((pcoordLineBottom - pcoordLineTop) / 2);
         // Check if top of 1st paragraph line is getting clipped
         // If so, stop checking paragraphs since we can assume remaining ones are also clipped
         // If line is more than half clipped, stop checking
         if( (pcoordLineCenter + pboxFrameBounds.y0) > pboxFrameBounds.y1)
         {
            break;
         }

         CFixed   cfixedLeading = textStyle.Leading();

         if( (textStyle.m_Paragraph.m_nLeadingType == LEADING_lines && cfixedLeading != MakeFixed(1)) ||
            textStyle.m_Paragraph.m_nLeadingType != LEADING_lines)
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphNonStandardLeading);

         if(textStyle.m_Paragraph.m_LeftMargin != 0)
            pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasLeftMargin);

         if(textStyle.m_Paragraph.m_RightMargin != 0)
            pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasRightMargin);

         if(textStyle.m_Paragraph.m_SpaceBefore != 0)
            pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasLeadingSpace);

         if(textStyle.m_Paragraph.m_SpaceAfter != 0)
            pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasTrailingSpace);

         if(textStyle.m_Paragraph.m_FirstLineIndent != 0)
            pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasIndent);
         // If all paragraphs have same alignment, pass compliance
         if(bFirstParagraph)
            nAlignment = textStyle.m_Paragraph.m_nAlignment;
         else if(textStyle.m_Paragraph.m_nAlignment != nAlignment)
            pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasAlignment);

         if(textStyle.m_Paragraph.m_nAlignment == ALIGN_justify_center ||
            textStyle.m_Paragraph.m_nAlignment == ALIGN_justify_right)
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasAlignment);

			 if( textStyle.BulletType() != BULLET_none )
				pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasBullets);

			 if( textStyle.DropLines() > 0 )
				pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctParagraphHasDropCaps);

			charIndex = textIterator.NextParagraphStart();
         bFirstParagraph = FALSE;
      }
      // Get last Visible character in frame
      charIndexLastVisibleChar = FindLastVisibleChar();

      PMGFontServer* pFontServer = (PMGFontServer*)(get_database()->get_font_server());
      // Loop through text and check for non-HTML compliant attributes
      if(charIndexLastVisibleChar >= 0 && lFirstCharacterInFrame != -1)
      {
		   CHARACTER_INDEX lCharacterIndex = -1;
   	   CHARACTER_COUNT lThisCharacters = 0;
         CHARACTER*      pCharacter = NULL;

         for(lCharacterIndex = lFirstCharacterInFrame;
            lCharacterIndex <= charIndexLastVisibleChar;
            lCharacterIndex++)
         {
            textIterator.SetPosition(lCharacterIndex);
		      if (lThisCharacters == 0)
		      {
			      pCharacter = pTextRecord->GetCharacters(lCharacterIndex, &lThisCharacters);
		      }

            ASSERT(pCharacter);
            if(pCharacter == NULL)
               break;

            textStyle = textIterator.Style();

            if(textStyle.FillPattern())
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasFillPattern);

            if(textStyle.OutlineStyle())
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasOutline);

            if(textStyle.ShadowStyle())
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasShadow);

            if(textStyle.WhiteOut())
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasWhiteOut);

            if(!(textStyle.Spacing()==0 || textStyle.Spacing() == MakeFixed(1)))
               pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasNonStandardSpacing);

         }  // End for loop
         // Loop through style changes and check font compliance
         charIndex = lFirstCharacterInFrame;
         // if there are no visible characters, don't allow loop to start
         if(charIndexLastVisibleChar < lFirstCharacterInFrame)
            charIndex = -1;

         while(charIndex >= 0 && charIndex <= charIndexLastVisibleChar)
         {
            textIterator.SetPosition(charIndex);
            textStyle = textIterator.Style();
            // Get position information about 1st line in paragraph for checking if
            // line is getting clipped.  If it is getting clipped, then we are done
            frameLocation.m_lCharacter = charIndex;
            pFrameRecord->GetCharacterLocation(&frameLocation);
            pcoordLineTop = frameLocation.m_Line.m_Baseline - frameLocation.m_Line.m_Ascend;
            pcoordLineBottom = frameLocation.m_Line.m_Baseline + frameLocation.m_Line.m_Descend;
            pcoordLineCenter = pcoordLineTop + ((pcoordLineBottom - pcoordLineTop) / 2);
            // Check if top of 1st paragraph line is getting clipped
            // If so, stop checking paragraphs since we can assume remaining ones are also clipped
            // If line is more than half clipped, stop checking
            if( (pcoordLineCenter + pboxFrameBounds.y0) > pboxFrameBounds.y1)
            {
               break;
            }

			   int nFace = pFontServer->font_record_to_face(textStyle.Font());
			   if (nFace != -1)
			   {
				   FaceEntry* pEntry = typeface_server.get_face(nFace);
				   if (pEntry != NULL)
                  {
                     if(pEntry->GetFontOrigin() != FaceEntry::fotWindows95Standard)
                        pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasNonStandardFace);
                  }
			   }
		      CFixed lSize = textStyle.Size();
		      if (lSize != UNDEFINED_CFIXED)
		      {
               // Get Point Size
			      int nSize = FixedInteger(lSize);
               if(nSize > 36)
                  pCompliantInfo->SetNonCompliant(CHTMLCompliantInfo::nctCharHasNonStandardSize);
		      }
            // Get next style change
            charIndex = textIterator.NextChangeStart();
         }
      }
   }  // Close brace for try
	CATCH(CErrorcodeException, e)
	{
		errorcode = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		errorcode = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		errorcode = ERRORCODE_IntError;
	}
	END_CATCH_ALL
   // Done with frame record
   pFrameRecord->release();
   pFrameRecord = NULL;
   // Done with text record
   pTextRecord->release();
   pTextRecord = NULL;

   return !pCompliantInfo->HasComplianceErrors();
}

CHARACTER_INDEX CFrameObject::FindLastVisibleChar()
{
   CHARACTER_INDEX   charIndexLastVisibleChar=-1, lFirstCharacterInFrame;
   CHARACTER_INDEX   lFirstCharacterInLine;
   CHARACTER_COUNT   shLineCount, shCurLine;
   PCOORD            pcoordLineTop, pcoordLineBottom, pcoordLineCenter;
   CFrameLocation    frameLocation;
   PBOX              pboxFrameBounds;
   CFrameRecord      *pFrameRecord;
   ERRORCODE         errorcode;

   // Check frame for special properties that are not HTML compliant
   pFrameRecord = LockFrameRecord(&errorcode);
   ASSERT(pFrameRecord);
   if(pFrameRecord == NULL || errorcode != ERRORCODE_None)
      return -1;

   pboxFrameBounds = pFrameRecord->ObjectBounds();

   lFirstCharacterInLine = lFirstCharacterInFrame = pFrameRecord->FirstCharacter();
   charIndexLastVisibleChar = 0;
   shLineCount = pFrameRecord->NumberOfLines();
   for(shCurLine = 0;shCurLine < shLineCount;shCurLine++)
   {
      CTextLine* pLine = pFrameRecord->GetLine(shCurLine);
		ASSERT(pLine != NULL);

      // Get position information 1st character in current line
      frameLocation.m_lCharacter = lFirstCharacterInLine;
      pFrameRecord->GetCharacterLocation(&frameLocation);
      pcoordLineTop = frameLocation.m_Line.m_Baseline - frameLocation.m_Line.m_Ascend;
      pcoordLineBottom = frameLocation.m_Line.m_Baseline + frameLocation.m_Line.m_Descend;
      pcoordLineCenter = pcoordLineTop + ((pcoordLineBottom - pcoordLineTop) / 2);
      // Check if line is getting clipped
      // If so, stop checking since we can assume remaining ones are also clipped
      if( (pcoordLineCenter + pboxFrameBounds.y0) > pboxFrameBounds.y1)
      {
         break;
      }
      charIndexLastVisibleChar += pLine->m_lCharacters;
      lFirstCharacterInLine += pLine->m_lCharacters;
   }
   charIndexLastVisibleChar--;   // Make it 0 based

   charIndexLastVisibleChar += lFirstCharacterInFrame;

   // Done with frame record
   pFrameRecord->release();
   pFrameRecord = NULL;

   return charIndexLastVisibleChar;
}

void
CFrameObject::SetHTMLAlign(CHTMLTagParagraph &tp, CTextStyle &ts)
{
   switch(ts.m_Paragraph.m_nAlignment)
   {
      case ALIGN_left:
      default:
         tp.SetAlignment(CHTMLTagParagraph::typeLeft);
         break;
      case ALIGN_center:
         tp.SetAlignment(CHTMLTagParagraph::typeCenter);
         break;
      case ALIGN_right:
         tp.SetAlignment(CHTMLTagParagraph::typeRight);
         break;
      case ALIGN_justify_left:
         tp.SetAlignment(CHTMLTagParagraph::typeTextTop);
         break;
   }
}

void
CFrameObject::GetURLList(CImageMapElementArray *pMap)
{
   ERRORCODE         errorcode;
   CHARACTER_INDEX   charIndex;
   
   CHARACTER_INDEX   charIndexLastVisibleChar = FindLastVisibleChar();
   CFrameRecord      *pFrameRecord = LockFrameRecord(&errorcode);;

   if (errorcode == ERRORCODE_None)
   {
      CTextRecord *pTextRecord = LockTextRecord(&errorcode);

      if (pTextRecord != NULL && errorcode == ERRORCODE_None)
      {
         CTextIterator     textIterator;
         CTextStyle        textStyle;

         textIterator.SetRecord(pTextRecord);
         // Look through text for hyperlinks
         for (charIndex = pFrameRecord->FirstCharacter(); 
               (charIndex >= 0) && (charIndex <= charIndexLastVisibleChar); 
               charIndex = textIterator.NextChangeStart())
         {
            textIterator.SetPosition(charIndex);
            textStyle = textIterator.Style();
      
            HyperlinkData  HData;
            if (textStyle.GetHyperlinkData(HData) == TRUE)
            {
               if (HData.IsValid() == TRUE)
               {
JTRACE1("CFrameObject::GetURLList adding link \"%s\"\n", HData.FilePathOrURL);
                  // Get the area of the text
                  // it will NOT be in screen coords, and have to be converted later
                  CTxp Txp(get_database());
                  Txp.Init(this, charIndex);

                  CHARACTER_INDEX   ncs = textIterator.NextChangeStart();
                  PBOX  PBoxStart, PBoxCur, BoxPrev;
                  for (CHARACTER_INDEX ci = charIndex; ci <= ncs; ci ++)
                  {
                     Txp.SetPosition(ci, 0, TRUE);
                     Txp.CalcFrame();
                     Txp.CalcCursor();
                     PBoxCur = Txp.CursorBox();
                     if (ci == charIndex)                            // Always get the first one
                     {
                        PBoxStart = PBoxCur;   
                     }
                     else
                     if (ci == ncs)                                  // Always include the last one
                     {
                        UnionBox(&PBoxStart, &PBoxStart, &PBoxCur);
                     }
                     else
                     if (PBoxCur.y0 != BoxPrev.y0)                   // text is wrapping
                     {
                        UnionBox(&PBoxStart, &PBoxStart, &BoxPrev); 
                        AddImageMapElement(pMap, PBoxStart, HData);  // save image map
                        PBoxStart = PBoxCur;                         // start over
                     }
                     BoxPrev = PBoxCur;
                  }
                  
                  AddImageMapElement(pMap, PBoxStart, HData);
               }
            }
         }
         
         pTextRecord->release();      
      }

      pFrameRecord->release();
   }
}

void
CFrameObject::RemoveHyperlink(DB_RECORD_NUMBER linkno)
{
	TRY
	{
		PMGDatabase* pDatabase = get_database();
		CTxp	txp(pDatabase);
		txp.Init(this, 0);

		CTextRecord* pText;
		if ((pText = txp.LockText()) != NULL)
		{
			DB_RECORD_NUMBER  dbrn = 0;
			CHARACTER_INDEX   i;
			
			CTextIterator  Iterator(pText);
			// Look through text for hyperlinks
			for (i = 0; i < pText->NumberOfCharacters(); i++)
			{
				Iterator.SetPosition(i);
				if ((dbrn = Iterator.Style().HyperlinkStyle()) != 0)
				{
					int LinkEnd = i, LinkStart = i;	
					// Found start; now find end...
					for (i; i < pText->NumberOfCharacters(); LinkEnd = ++i)
					{
						Iterator.SetPosition(i);
						if (Iterator.Style().HyperlinkStyle() != dbrn)
							break;
					}
					// Looks good?
					if (LinkStart < LinkEnd)
					{
						if ((linkno == 0) || (linkno == dbrn))
						{
							CTextStyle	StyleNoHyperlink(pDatabase);
							// Make a style with only the "no hyperlinkness" defined
							StyleNoHyperlink.MakeTextUndefined();
							StyleNoHyperlink.HyperlinkStyle(0);
							// Apply this "no hyperlinkness" style to the link's range of text
							pText->ChangeRangeStyle(LinkStart, LinkEnd, &StyleNoHyperlink);
						}
						i --;
					}
				}
			}
			pText->release();
		}
	}
	END_TRY
}

void
CFrameObject::AddImageMapElement(CImageMapElementArray *pMap, PBOX box, HyperlinkData &HData)
{
   PBOX  Pbox;

   Pbox.x0 = box.x0 < box.x1 ? box.x0 : box.x1;
   Pbox.x1 = box.x0 < box.x1 ? box.x1 : box.x0;;
   Pbox.y0 = box.y0 < box.y1 ? box.y0 : box.y1;
   Pbox.y1 = box.y0 < box.y1 ? box.y1 : box.y0;
//
   PPNT pp [4];
   RotatePBOXToFourPoints(Pbox, pp, 0.0); // Just get 4 points

   PBOX  UnrotatedBound = get_unrotated_bound();
   PPNT  point;

   CWarpField* pWarpField = GetWarpField();
   if (pWarpField != NULL)
   {
      // Scale down to a reasonable screen DPI
      UnrotatedBound.x0 = scale_pcoord(UnrotatedBound.x0, 96, PAGE_RESOLUTION);
      UnrotatedBound.y0 = scale_pcoord(UnrotatedBound.y0, 96, PAGE_RESOLUTION);
      UnrotatedBound.x1 = scale_pcoord(UnrotatedBound.x1, 96, PAGE_RESOLUTION);
      UnrotatedBound.y1 = scale_pcoord(UnrotatedBound.y1, 96, PAGE_RESOLUTION);
      CRect Rect;
      RedisplayContext::convert_pbox(&UnrotatedBound, &Rect, NULL);
      pWarpField->Transform(Rect);

      for(int j = 0; j < 4; j ++)
      {
         point = pp[j];
         CFixed px, py; 
         px = MakeFixed(scale_pcoord(point.x, 96, PAGE_RESOLUTION));
         py = MakeFixed(scale_pcoord(point.y, 96, PAGE_RESOLUTION));
         pWarpField->WarpPoint(&px, &py);
         point.x = scale_pcoord(FixedRound(px), PAGE_RESOLUTION, 96);
         point.y = scale_pcoord(FixedRound(py), PAGE_RESOLUTION, 96);
         pp[j] = point;
      }
   }

   if (get_flags() & OBJECT_FLAG_xflipped)
	{
	   for(int j = 0; j < 4; j ++)
      {
         point = pp[j];
         point.x = (UnrotatedBound.x1 + UnrotatedBound.x0) - point.x;
         pp[j] = point;
      }   
   }

	if (get_flags() & OBJECT_FLAG_yflipped)
	{
	   for(int j = 0; j < 4; j ++)
      {
         point = pp[j];
         point.y = (UnrotatedBound.y1 + UnrotatedBound.y0) - point.y;
         pp[j] = point;
      }   
	}

   double rotation  = get_rotation(); 
   if (rotation != 0.0)
   {
      rotate_points(pp, 4, get_origin(), rotation);   // Really rotate this time
   }

	PMObjectShapeData sd;
   sd.SetPoints(pp, 4);
   pMap->Add(HData, sd);         
}

ERRORCODE CFrameObject::ConvertToHTML(CHTMLTag& tag)
{
	ERRORCODE errorcode;

	TRY
	{
		PMGFontServer* pFontServer = (PMGFontServer*)(get_database()->get_font_server());
		if (!pFontServer)
			return ERRORCODE_Fail;

		CFrameRecord* pFrameRecord = LockFrameRecord(&errorcode);
		if (!pFrameRecord || errorcode != ERRORCODE_None)
			return ERRORCODE_Fail;

		CHARACTER_INDEX lFirstCharacterInFrame = pFrameRecord->FirstCharacter();
		CHARACTER_COUNT lLastCharacterInFrame = lFirstCharacterInFrame + pFrameRecord->NumberOfCharacters();
		CHARACTER_COUNT lFrameCharacterCount = pFrameRecord->NumberOfCharacters();
		if (lFrameCharacterCount > 0)
			lLastCharacterInFrame--;

		// Check for empty frame
		if (lFrameCharacterCount <= 0)
		{
			pFrameRecord->release();
			return ERRORCODE_Fail;
		}

		CTextRecord* pTextRecord = LockTextRecord(&errorcode);
		if (!pTextRecord || errorcode != ERRORCODE_None)
		{
			pFrameRecord->release();
			return ERRORCODE_Fail;
		}

		// Get last Visible character in frame
		CHARACTER_INDEX charIndexLastVisibleChar = FindLastVisibleChar();
		// if there are no visible characters, don't allow loop to start
		if (charIndexLastVisibleChar < lFirstCharacterInFrame)
		{
			pTextRecord->release();
			pFrameRecord->release();
			return ERRORCODE_Fail;
		}

		CMacroServer *pMacroServer = get_database()->GetMacroServer();
		BOOL bSetParagraphStyle = FALSE;
		CFrameLocation frameLocation;
		CHTMLFont htmlFontCurrent;
		CHTMLFont htmlFontHold;
		CHTMLTagParagraph htmlTagParagraph;
		PBOX pboxFrameBounds = pFrameRecord->ObjectBounds();
//j		FillFormatV1 fillFormat = pFrameRecord->Fill();

		// Loop through style changes and generate HTML 
		CTextIterator textIterator;
		textIterator.SetRecord(pTextRecord);
		textIterator.Style().Frame(this);

		CHARACTER_INDEX charIndex = lFirstCharacterInFrame;
		while (charIndex >= 0 && charIndex <= charIndexLastVisibleChar)
		{
			textIterator.SetPosition(charIndex);
			CTextStyle textStyle;
			textStyle = textIterator.Style();
			// Get next style change
			CHARACTER_INDEX charIndexStyleEnd = textIterator.NextChangeStart() - 1;
			if (charIndexStyleEnd < 0)
				charIndexStyleEnd = charIndexLastVisibleChar;
			// Get position information about 1st line in paragraph for checking if
			// line is getting clipped.  If it is getting clipped, then we are done
			frameLocation.m_lCharacter = charIndex;
			pFrameRecord->GetCharacterLocation(&frameLocation);
			PCOORD pcoordLineTop = frameLocation.m_Line.m_Baseline - frameLocation.m_Line.m_Ascend;
			PCOORD pcoordLineBottom = frameLocation.m_Line.m_Baseline + frameLocation.m_Line.m_Descend;
			PCOORD pcoordLineCenter = pcoordLineTop + ((pcoordLineBottom - pcoordLineTop) / 2);
			// Check if top of 1st paragraph line is getting clipped
			// If so, stop checking paragraphs since we can assume remaining ones are also clipped
			// If line is more than half clipped, stop checking
			if ((pcoordLineCenter + pboxFrameBounds.y0) > pboxFrameBounds.y1)
				break;

			if (!bSetParagraphStyle)
			{
				SetHTMLAlign(htmlTagParagraph, textStyle);
				bSetParagraphStyle = TRUE;
			}

			// Collect HTML style information
			int nFace = pFontServer->font_record_to_face(textStyle.Font());
			if (nFace != -1)
			{
				FaceEntry* pEntry = typeface_server.get_face(nFace);
				if (pEntry)
					htmlFontCurrent.SetFace(pEntry->get_name());
			}
		  
			CFixed lSize = textStyle.Size();
			if (lSize != UNDEFINED_CFIXED)
			{
				int nSize = FixedInteger(lSize);
				htmlFontCurrent.SetSize(nSize);
			}

			FillFormatV1 fillFormat = textStyle.Fill();
			COLOR ColorText = fillFormat.m_ForegroundColor;
			if (ColorText != UNDEFINED_COLOR)
				htmlFontCurrent.SetColor(ColorText);

			int nStyle = CHTMLFont::styleNone;
			if (textStyle.Bold())
				nStyle |= CHTMLFont::styleBold;
			if (textStyle.Italic())
				nStyle |= CHTMLFont::styleItalic;
			if (textStyle.Underline())
				nStyle |= CHTMLFont::styleUnderline;
			htmlFontCurrent.SetStyle(nStyle);

			// If this is the first time through, initialize the font thing
			// that will actually get the text
			if (charIndex == lFirstCharacterInFrame)
				htmlFontHold.CopyFontInfoFrom(htmlFontCurrent);

			// Get all characters of this style
			CHARACTER_INDEX lCharacterIndex = -1;
			CHARACTER_COUNT lThisCharacters = 0;
			CHARACTER* pCharacter = NULL;

			// Have macro server handy if there are fill-in fields in character stream
			CString Text;
			while (charIndex <= charIndexStyleEnd && charIndex <= charIndexLastVisibleChar)
			{
				textIterator.SetPosition(charIndex);
				if (lThisCharacters == 0)
					pCharacter = pTextRecord->GetCharacters(charIndex, &lThisCharacters);

				if (!pCharacter)
					break;

				CHARACTER c = *pCharacter++;
				lThisCharacters--;
				if (c < MACRO_CHARACTER)
					Text += (char)c;
				else
				{
					if (pMacroServer)
					{
						// Get the macro text.
						CHARACTER* pText = pMacroServer->GetMacroText(c);
						if (pText)
						{
							for (CHARACTER* p = pText; *p != 0; p++)
								Text += (char)*p;
						}
					}
				}

				charIndex++;
			}

			// If the font has changed, add any text we have accumulated
			// and prepare to start a new font object
			if (!htmlFontCurrent.CompareFontInfo(htmlFontHold))
			{
				if (htmlFontHold.IsBody())
					htmlTagParagraph.Add(htmlFontHold);

				htmlFontHold.Empty();

				htmlFontHold.CopyFontInfoFrom(htmlFontCurrent);
			}

			// If this text is a hyperlink, add it as that
			HyperlinkData HData;
			if (textStyle.GetHyperlinkData(HData))
			{
				if (HData.IsEnabled())
				{
					CHTMLTagLink Link;
					Link.SetHyperlinkData(HData, ((CPmwDoc*)(((PMGDatabase*)database))->GetDocument())->DocumentRecord());
					Link.AddText(Text);
					Text.Empty();
					htmlFontHold.Add(Link, htmlFontCurrent.GetStyle());
				}
			}

			// Add any text we may have accumulated
			htmlFontHold.AddText(Text, htmlFontCurrent.GetStyle());
		}

		// Add any text that has not been added yet
		if (htmlFontHold.IsBody())
			htmlTagParagraph.Add(htmlFontHold);

		tag.Add(htmlTagParagraph);
		tag.AddLineBreak();

		pFrameRecord->release();
		pTextRecord->release();
	}  // Close brace for try

	CATCH(CErrorcodeException, e)
	{
		errorcode = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		errorcode = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		errorcode = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return ERRORCODE_None;
}

// Set the page for this object.
void CFrameObject::Page(DB_RECORD_NUMBER lPage)
{
   // Do the normal page set.
   INHERITED::Page(lPage);
   // Set the page in the frame record.
   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      pFrame->Page(lPage);
      pFrame->release();
   }
}

/*
// Assign an object to this.
*/

ERRORCODE CFrameObject::assign(PageObjectRef sobject)
{
   ERRORCODE error;
   CFrameObject& SourceObject = (CFrameObject&)sobject;

   // Get rid of our old frame and text.
   DestroyFrameAndText();

   /* Assign the base object. */

   if ((error = RectPageObject::assign(sobject)) == ERRORCODE_None)
   {
      PMGDatabase* pSourceDatabase = SourceObject.get_database();
      PMGDatabase* pDestDatabase = get_database();

      // Nothing we want here...
//    m_FRecord = SourceObject.m_FRecord;

      CFrameRecord* pSourceFrame = SourceObject.LockFrameRecord(&error);
      if (pSourceFrame != NULL)
      {
         if (pSourceFrame->PreviousFrame() == 0
               && pSourceFrame->NextFrame() == 0)
         {
            // Duplicate the frame.
            CFrameRecord* pNewFrame = (CFrameRecord*)pSourceFrame->duplicate(pDestDatabase);
            if (pNewFrame == NULL)
            {
               error = pDestDatabase->last_creation_error();
               ASSERT(error != ERRORCODE_None);
            }
            else
            {
               // Duplicate the paragraph.
               DB_RECORD_NUMBER lText = pSourceDatabase->duplicate_record(pSourceFrame->TextRecord(), pDestDatabase);
               if (lText == 0)
               {
                  error = pDestDatabase->last_creation_error();
                  ASSERT(error != ERRORCODE_None);
                  pNewFrame->zap();
               }
               else
               {
                  pNewFrame->Page(pDestDatabase->CurrentPage());
                  pNewFrame->TextRecord(lText);

                  FrameRecord(pNewFrame->Id());
                  CTextRecord* pText = pNewFrame->LockTextRecord();
                  if (pText != NULL)
                  {
                     pText->FirstFrame(pNewFrame->Id());
                     pText->release();
                  }
                  pNewFrame->release();
               }
            }
         }
         else
         {
            // Create empty stuff for our amusement.
            FRAMEOBJ_CREATE_STRUCT fcs;

            fcs.bound = pSourceFrame->ObjectBounds();
            fcs.flags = pSourceFrame->FrameFlags();
            fcs.vert_alignment = (VERT_ALIGN_TYPE)pSourceFrame->VerticalAlignment();
            PBOX Margins = SourceObject.Margins();
            fcs.LeftMargin = Margins.x0;
            fcs.TopMargin = Margins.y0;
            fcs.RightMargin = Margins.x1;
            fcs.RightMargin = Margins.y1;

            HandleCreationData(&fcs);
         }
         pSourceFrame->release();
      }
   }
   if (error == ERRORCODE_None)
   {
      // Remove the story if we are not supposed to be on the page.
      if (!m_fOnPage)
      {
         RemoveStory();
      }
   }
   return error;
}

/*
// Get a frame's name.
*/

BOOL CFrameObject::get_name(LPSTR buffer, SHORT size, SHORT max_width)
{
   size--;        /* Leave room in buffer for terminator. */

   *buffer = '\0';

   size -= 5;     /* Leave room for quotes and ellipses. */

   // A default value in case of error.
   // JRO strcpy(buffer, "(Text Box)");
   strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_TEXT_BOX_P)); 

   // Lock the frame. We want to find the text for the first line.

   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      // Find a line to show text for.
      LINE_COUNT lLines = pFrame->NumberOfLines();

      if (pFrame->NumberOfCharacters() == 0)
      {
         // JRO strcpy(buffer, "(Empty Text Box)");
         strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_TEXT_BOX_EMPTY_P)); 
      }
      else
      {
         CTextLine* pLine = NULL;
         CHARACTER_INDEX lCharacterInFrame = 0;

         for (LINE_INDEX lLine = 0; lLine < lLines; lLine++)
         {
            pLine = pFrame->GetLine(lLine);
            if (pLine->m_lCharacters > 1
                || (pLine->m_lCharacters == 1 && lLine == lLines-1))
            {
               break;
            }
            lCharacterInFrame += pLine->m_lCharacters;
         }
         ASSERT(pLine != NULL);

         CHARACTER_COUNT lCount = pLine->m_lCharacters;

         // Get the text record for this frame.

         CTextRecord* pText = pFrame->LockTextRecord();
         if (pText != NULL)
         {
            CHARACTER_INDEX lFirstCharacterInFrame = pFrame->FirstCharacter();

            CMacroServer* pMacroServer = ((PMGDatabase*)database)->GetMacroServer();

            if (pText->HasMacro())
            {
               CHARACTER_INDEX lCharacter = lFirstCharacterInFrame + lCharacterInFrame;

               // We need to count by hand. There may be a macro in this
               // line which will change the ultimate character count.
               CHARACTER_COUNT lChars = lCount;
               lCount = 0;
               while (lChars-- > 0)
               {
                  CHARACTER* cp = pText->GetCharacter(lCharacter++);
                  CHARACTER* pMC;

                  if (*cp >= MACRO_CHARACTER
                      && pMacroServer != NULL
                      && (pMC = pMacroServer->GetMacroText(*cp)) != NULL)
                  {
                     while (*pMC++ != 0)
                     {
                        lCount++;
                     }
                  }
                  else
                  {
                     lCount++;
                  }
               }
            }

            BOOL fAddDots = TRUE;

            LPSTR s = buffer;
            *s++ = '"';

         /* Build some text. */

            if ((int)lCount < size)
            {
               size = (int)lCount;
               fAddDots = FALSE;             /* Can fit all text! */
            }

            CHARACTER_INDEX lCharacter = lFirstCharacterInFrame + lCharacterInFrame;

            while (size > 0)
            {
               CHARACTER c = *(pText->GetCharacter(lCharacter++));

               if (c >= MACRO_CHARACTER)
               {
                  if (pMacroServer == NULL)
                  {
                     *s++ = '?';
                     size--;
                  }
                  else
                  {
                     CHARACTER* pText = pMacroServer->GetMacroText(c);

                  /* This is a macro character! */

                     while ((c = *pText++) != 0 && size > 0)
                     {
                        if (c > 127 || c < 32)
                        {
                           c = '*';
                        }

                        *s++ = (char)c;
                        size--;
                     }
                  }
               }
               else if (c == '\t')
               {
                   *s++ = ' ';
                   size--;
               }
               else if (c == '\n')
               {
                  // Hit a break character. Stop.
                  break;
               }
               else
               {
               /* Handle a normal character. */
                  if (c > 127 || c < 32)
                  {
                     c = '*';
                  }

                  *s++ = (char)c;
                  size--;
               }
            }

            if (size != 0 || fAddDots)
            {
               *s++ = '.';
               *s++ = '.';
               *s++ = '.';
            }
            *s++ = '"';
            *s = '\0';
         }
      }

      pFrame->release();
   }

   return TRUE;
}

// Expand a PBOX to include the specified point.
//
// NOTE: Moving this function in front of IntersectVertical and
// IntersectHorizontal sidesteps a 16-bit internal compiler error.

void IncludePoint(PBOX_PTR Box, PCOORD x, PCOORD y, PBOX& ClipBound)
{
   if ((x >= ClipBound.x0)
    && (x <= ClipBound.x1)
    && (y >= ClipBound.y0)
    && (y <= ClipBound.y1))
   {
      if (x < Box->x0)
      {
         Box->x0 = x;
      }
      if (x > Box->x1)
      {
         Box->x1 = x;
      }
      if (y < Box->y0)
      {
         Box->y0 = y;
      }
      if (y > Box->y1)
      {
         Box->y1 = y;
      }
   }
}

// Compute the Y coordinates of the intersections of the line through points
// p0 and p1 with the lines X=x0 and X=x1. The computed coordinates are saved
// in y0 and y1.

void IntersectVertical(PPNT& p0, PPNT& p1, PCOORD x0, PCOORD x1, PCOORD& y0, PCOORD& y1)
{
   double y;

   // Compute the slope of the line.
   double DeltaX = (double)(p1.x-p0.x);
   double DeltaY = (double)(p1.y-p0.y);

	if (DeltaX == 0.0)
	{
		// Line is vertical, use +/- infinity.
		x0 = ((x0-p0.x)*DeltaY) >= 0.0 ? 0x7fffffffL : -0x7fffffffL;
		x1 = ((x1-p1.x)*DeltaY) >= 0.0 ? 0x7fffffffL : -0x7fffffffL;
	}
	else
	{
		// Compute y0.
		y = p0.y+((x0-p0.x)*DeltaY)/DeltaX;
		if (y > (double)(0x7fffffffL))
		{
			y = (double)(0x7fffffffL);
		}
		else if (y < (double)(-0x7fffffffL))
		{
			y = (double)(-0x7fffffffL);
		}
		y0 = (PCOORD)(y+0.5);

		// Compute y1.
		y = p1.y+((x1-p1.x)*DeltaY)/DeltaX;
		if (y > (double)(0x7fffffffL))
		{
			y = (double)(0x7fffffffL);
		}
		else if (y < (double)(-0x7fffffffL))
		{
			y = (double)(-0x7fffffffL);
		}
		y1 = (PCOORD)(y+0.5);
	}
}

// Compute the X coordinates of the intersections of the line through points
// p0 and p1 with the lines Y=y0 and Y=y1. The computed coordinates are saved
// in x0 and x1.

void IntersectHorizontal(PPNT& p0, PPNT& p1, PCOORD y0, PCOORD y1, PCOORD& x0, PCOORD& x1)
{
   double x;

   // Compute the slope of the line.
   double DeltaX = (double)(p1.x-p0.x);
   double DeltaY = (double)(p1.y-p0.y);

	if (DeltaY == 0.0)
	{
		// Line is horizontal, use +/- infinity.
		x0 = ((y0-p0.y)*DeltaX) >= 0.0 ? 0x7fffffffL : -0x7fffffffL;
		x1 = ((y1-p1.y)*DeltaX) >= 0.0 ? 0x7fffffffL : -0x7fffffffL;
	}
	else
	{
		// Compute x.
		x = p0.x+((y0-p0.y)*DeltaX)/DeltaY;
		if (x > (double)(0x7fffffffL))
		{
			x = (double)(0x7fffffffL);
		}
		else if (x < (double)(-0x7fffffffL))
		{
			x = (double)(-0x7fffffffL);
		}
		x0 = (PCOORD)(x+0.5);

		// Compute x1.
		x = p1.x+((y1-p1.y)*DeltaX)/DeltaY;
		if (x > (double)(0x7fffffffL))
		{
			x = (double)(0x7fffffffL);
		}
		else if (x < (double)(-0x7fffffffL))
		{
			x = (double)(-0x7fffffffL);
		}
		x1 = (PCOORD)(x+0.5);
	}
}

BOOL CFrameObject::GetClipRgn(CRgn& Rgn, RedisplayContextPtr rc, int nType /*=CLIP_TYPE_Boundary*/)
{
	ASSERT(nType == CLIP_TYPE_Boundary);
   BOOL fResult = FALSE;

   // Get the unrotated bounds.
   PBOX UnrotatedBounds = get_unrotated_bound();

   // For text boxes, we let text extend past the sides of the text box,
   // but clip it to the top and bottom.

   if (get_rotation() == 0.0)
   {
      // The frame object is not rotated, just use a rectangular region.

      UnrotatedBounds.x0 = rc->source_pbox.x0;
      UnrotatedBounds.x1 = rc->source_pbox.x1;
      rc->pbox_to_screen(&UnrotatedBounds, FALSE);

      CRect crRgnBounds;
      rc->convert_pbox(&UnrotatedBounds, crRgnBounds, NULL);

		if (rc->is_print_preview)
		{
			::LPtoDP(rc->destination_hdc, (LPPOINT)(LPRECT)crRgnBounds, 2);
		}

      fResult = Rgn.CreateRectRgnIndirect(crRgnBounds);
   }
   else
   {
      // We do some extra work here in order to limit the size of the region.
      // Hopefully, this will turn out to be useful and prevent some overflow
      // conditions.

      // Normalize rotation to between 0 and 2 Pi.
      ANGLE dRotation = get_rotation();
      while (dRotation < 0.0)
      {
         dRotation += PI2;
      }
      while (dRotation >= PI2)
      {
         dRotation -= PI2;
      }
      ASSERT((dRotation >= 0.0) && (dRotation < PI2));

      // Figure out which quadrant in which the rotation angle lies.
      int nQuadrant = (int)((dRotation/PI2)*4.0);
      ASSERT((nQuadrant >= 0) && (nQuadrant < 4));

      // Modify the X coordinates of the bounds to make sure it will
      // lie outside the source box when rotated.

      double d0, d1;

      if ((nQuadrant & 1) == 0)
      {
         // Quadrants 0 and 2.
         d0 = hypot(rc->source_pbox.x0-get_origin().x, rc->source_pbox.y1-get_origin().y);
         d1 = hypot(rc->source_pbox.x1-get_origin().x, rc->source_pbox.y0-get_origin().y);
      }
      else
      {
         // Quadrants 1 and 3.
         d0 = hypot(rc->source_pbox.x0-get_origin().x, rc->source_pbox.y0-get_origin().y);
         d1 = hypot(rc->source_pbox.x1-get_origin().x, rc->source_pbox.y1-get_origin().y);
      }

      if (nQuadrant >= 2)
      {
         double t = d0;
         d0 = d1;
         d1 = t;
      }

      // Adjust the unrotated bounds by the computed amounts.
      // This adjustment should make the bounds always lie
      // outside of the source pbox no matter how it is rotated.
      UnrotatedBounds.x0 -= (PCOORD)(ceil(d0));
      UnrotatedBounds.x1 += (PCOORD)(ceil(d1));

      // Rotate the bounds and create a polygon region based on the rotated points.
      PBOX RotatedBounds;
      PPNT v[4];

      compute_rotated_bound(UnrotatedBounds, get_rotation(), &RotatedBounds, get_origin(), v);

      POINT p[5];
      rc->ppnt_to_screen(v[0], &(p[0]));
      rc->ppnt_to_screen(v[1], &(p[1]));
      rc->ppnt_to_screen(v[2], &(p[2]));
      rc->ppnt_to_screen(v[3], &(p[3]));
      p[4] = p[0];

		if (rc->is_print_preview)
		{
			::LPtoDP(rc->destination_hdc, (LPPOINT)p, sizeof(p)/sizeof(p[0]));
		}

      fResult = Rgn.CreatePolygonRgn(p, sizeof(p)/sizeof(p[0]), ALTERNATE);
   }

   return fResult;
}

void CFrameObject::get_refresh_bound(PBOX_PTR result_pbox, RedisplayContextPtr rc)
{
   if (rc == NULL)
   {
      CPmwDoc* pDoc = (CPmwDoc*)(((PMGDatabase*)database)->GetDocument());
      if (pDoc != NULL)
      {
         CPmwView* pView = pDoc->GetCurrentView();
         rc = pView->get_rc();
      }
   }

   if (m_fEditing && rc != NULL && rc->IsScreen())
   {
      // If we're editing and refreshing to the screen, then use the full width.
      // This bounds will be used to build the off-screen background bitmap, so it
      // must enclose the entire area in which text could potentially be displayed.
      // Since we are editing, we don't know exactly the fonts the user could choose,
      // so we have to assume worst case and use the entire width of the view.

      PBOX UnrotatedBound = get_unrotated_bound();

// Don't do this until we actually determine that it's necessary.
#if 0
      // Bump the top an bottom out a little because of possible differences between
      // the coordinates determined by IntersectHorizontal() and IntersectVertical()
      // using floating point and the final character positions which are computed
      // using integer math.
      //
      // FF 2/5/97 AHA! I think I know what these were for... The refresh bound is used
      // to size the editing background bitmap. Refresh extents used to be expanded
      // by 2 pixels for some reason. The following code was probably to make sure the
      // editing bitmap completely encloses any refresh extents create by textflow, etc.
      // For the moment, the refresh expansion (see TEXFLOW.CPP) is disabled, so we'll
      // leave this code disabled as well.
      UnrotatedBound.y0 -= 2*rc->redisplay_y_pixel;
      UnrotatedBound.y1 += 2*rc->redisplay_y_pixel;
#endif

      // Rotate the bounding box and get the coordinates of the four corners into v[].
      // v[0] and v[1] will be the rotated upper left and upper right points.
      // v[3] and v[2] will be the rotated lower left and lower right points.
      PBOX RotatedBound;
      PPNT v[4];
      compute_rotated_bound(UnrotatedBound, get_rotation(), &RotatedBound, get_origin(), v);

      // Compute the intersection points of the top and bottom lines of the rotated
      // bounds with the source bounds. This will give us enough information to
      // compute the final refresh bounds. Remember, we're trying to get the bounds
      // for the area which could potentially be updated while the user types using
      // any font and any point size.
      //
      // NOTE: I was stung by the following gotcha... The source PBOX in the RC is the
      // entire client area, not just the page area (obvious once you think about it).
      // So, the refresh bounds is extended to the sides of the client area, not just
      // to the sides of the page. I was expecting the clipping to occur to the page
      // borders and of course it "wasn't working!"

		PBOX ClipBound;
		UnionBox(&ClipBound, &(rc->source_pbox), &RotatedBound);

      PCOORD x010, x011;
      IntersectHorizontal(v[0], v[1], ClipBound.y0, ClipBound.y1, x010, x011);

      PCOORD x320, x321;
      IntersectHorizontal(v[3], v[2], ClipBound.y0, ClipBound.y1, x320, x321);

      PCOORD y010, y011;
      IntersectVertical(v[0], v[1], ClipBound.x0, ClipBound.x1, y010, y011);

      PCOORD y320, y321;
      IntersectVertical(v[3], v[2], ClipBound.x0, ClipBound.x1, y320, y321);

      // OK, we have the points. You may have to draw a picture to understand this
      // totally (I did), but basically, we just need find the maximum bounding
      // box for those points which actually lie within the bounds of the source pbox.
      // This will give us the bounding box for the redisplay area clipped to the source
      // bounding box.

      // Initialize to minimum values so comparisons will work.
      result_pbox->x0 = 0x7fffffffL;
      result_pbox->y0 = 0x7fffffffL;
      result_pbox->x1 = -0x7fffffffL;
      result_pbox->y1 = -0x7fffffffL;

      // merge in points.
      IncludePoint(result_pbox, x010, rc->source_pbox.y0, ClipBound);
      IncludePoint(result_pbox, x011, rc->source_pbox.y1, ClipBound);
      IncludePoint(result_pbox, x320, rc->source_pbox.y0, ClipBound);
      IncludePoint(result_pbox, x321, rc->source_pbox.y1, ClipBound);
      IncludePoint(result_pbox, rc->source_pbox.x0, y010, ClipBound);
      IncludePoint(result_pbox, rc->source_pbox.x1, y011, ClipBound);
      IncludePoint(result_pbox, rc->source_pbox.x0, y320, ClipBound);
      IncludePoint(result_pbox, rc->source_pbox.x1, y321, ClipBound);

		IntersectBox(result_pbox, result_pbox, &(rc->source_pbox));

      // PMGTODO:In theory, this should just be a one pixel adjustment on
      // all sides, but the sides are bumped out two pixels to account for
      // glitches on the corners when the frame is rotated that are probably
      // from rounding errors. It would be good to figure out exactly what
      // is going on here so we could make sure we are generating the minimum
      // refresh bounds (refreshing takes time!)

      result_pbox->x0 -= 2*rc->redisplay_x_pixel;
      result_pbox->y0 -= 2*rc->redisplay_y_pixel;
      result_pbox->x1 += 2*rc->redisplay_x_pixel;
      result_pbox->y1 += 2*rc->redisplay_y_pixel;

#if 0
      JTRACE("(%ld,%ld,%ld,%ld) --> ", result_pbox->x0, result_pbox->y0, result_pbox->x1, result_pbox->y1);

      PBOX b = *result_pbox;
      rc->pbox_to_screen(&b, TRUE);
      RECT r;
      rc->convert_pbox(&b, &r, NULL);

      HDC hDC = GetDC(rc->hwnd);
      FillRect(hDC, &r, (HBRUSH)GetStockObject(GRAY_BRUSH));
      ReleaseDC(rc->hwnd, hDC);

      JTRACE("(%d,%d,%d,%d)\n", r.left, r.top, r.right, r.bottom);
#endif
   }
   else
   {
      // In this case, we are not editing and not creating a background bitmap.
      // So, the refresh exent is the rotated bounds with any refresh offsets
      // computed by the last text flow. The refresh offsets are the maximum
      // distances that the line extents extend outside the frame bounds. This
      // can happen with some fonts which have fancy characters that extend
      // outside of their delta X area (script fonts do this a lot.)
      //
      // NOTE: The refresh offsets are stored in the frame record and are normalized
      // for the unflipped, unrotated object.

      // Get the unrotated bound.
      PBOX UnrotatedBound = get_unrotated_bound();
		PCOORD UnrotatedDX = UnrotatedBound.x1 - UnrotatedBound.x0;
		PCOORD UnrotatedDY = UnrotatedBound.y1 - UnrotatedBound.y0;

      // Adjust the unrotated bound by the refresh offsets.
      //
      // NOTE: PMG sets things up so that the flipping is applied first,
      // then rotation to get to the final object. If you rotate an object
      // and then flip it, PMG adjusts the rotation to maintain the "flip,
      // then rotate", property.

      // Get the frame record.
      CFrameRecord* pFrame = NULL;
      DB_RECORD_NUMBER lFrame = FrameRecord();

      if ((lFrame != 0)
       && ((pFrame = (CFrameRecord*)database->get_record(lFrame, NULL, RECORD_TYPE_Frame)) != NULL)
       && (pFrame->NumberOfLines() > 0))
      {
         // Get the flags and offsets. We need the flags to apply the offsets to the
         // correct size of the bounding box. Remember, the offset are stored for
         // an unflipped object.
         FLAGS flags = get_flags();
         PBOX Offsets = pFrame->RefreshOffsets();

			if (pFrame->Fill().IsVisible()
			 || pFrame->Outline().IsVisible()
			 || pFrame->Shadow().IsVisible()
          || (rc != NULL && rc->IsScreen()))       // We have frame outline on screen.
			{
				if (Offsets.x0 > 0)
				{
					Offsets.x0 = 0;
				}
				if (Offsets.x1 < UnrotatedDX)
				{
					Offsets.x1 = UnrotatedDX;
				}
				if (Offsets.y0 > 0)
				{
					Offsets.y0 = 0;
				}
				if (Offsets.y1 < UnrotatedDY)
				{
					Offsets.y1 = UnrotatedDY;
				}
			}

			PPNT ShadowOffset;
			ShadowOffset.x = 0;
			ShadowOffset.y = 0;

			pFrame->GetShadowOffsets(&ShadowOffset.x, &ShadowOffset.y);

//			JTRACE("Offsets = (%d,%d,%d,%d), Shadow Offsets = (%d,%d)\n",
//				Offsets.x0, Offsets.y0, Offsets.x1, Offsets.y1, ShadowOffset.x, ShadowOffset.y);

			if (ShadowOffset.x < 0)
			{
				if (ShadowOffset.x < Offsets.x0)
				{
					Offsets.x0 = ShadowOffset.x;
				}
			}
			else if (ShadowOffset.x > 0)
			{
				if (ShadowOffset.x+UnrotatedDX > Offsets.x1)
				{
					Offsets.x1 = ShadowOffset.x+UnrotatedDX;
				}
			}
			
			if (ShadowOffset.y < 0)
			{
				if (ShadowOffset.y < Offsets.y0)
				{
					Offsets.y0 = ShadowOffset.y;
				}
			}
			else if (ShadowOffset.y > 0)
			{
				if (ShadowOffset.y+UnrotatedDY > Offsets.y1)
				{
					Offsets.y1 = ShadowOffset.y+UnrotatedDY;
				}
			}
			
         if ((Offsets.x0 <= Offsets.x1) && (Offsets.y0 <= Offsets.y1))
         {
            if ((flags & OBJECT_FLAG_xflipped) != 0)
            {
               UnrotatedBound.x0 = UnrotatedBound.x1 - Offsets.x1;
               UnrotatedBound.x1 -= Offsets.x0;
            }
            else
            {
               UnrotatedBound.x1 = UnrotatedBound.x0 + Offsets.x1;
               UnrotatedBound.x0 += Offsets.x0;
            }

            if ((flags & OBJECT_FLAG_yflipped) != 0)
            {
               UnrotatedBound.y0 = UnrotatedBound.y1 - Offsets.y1;
               UnrotatedBound.y1 -= Offsets.y0;
            }
            else
            {
               UnrotatedBound.y1 = UnrotatedBound.y0 + Offsets.y1;
               UnrotatedBound.y0 += Offsets.y0;
            }
         }
      }

      // Done with frame.
      if (pFrame != NULL)
      {
         pFrame->release();
      }

      // Ok, we now have the unrotated refresh bounds (including any offsets.)
      // Now rotate it to get the final bounds.
      PPNT v[4];
      compute_rotated_bound(UnrotatedBound, get_rotation(), result_pbox, get_origin(), v);

      if (rc != NULL)
      {
         // We need to include the boundary line surrounding the frame content in
         // its refresh bounds.

         // PMGTODO:In theory, this should just be a one pixel adjustment on
         // all sides, but the sides are bumped out two pixels to account for
         // glitches on the corners when the frame is rotated that are probably
         // from rounding errors. It would be good to figure out exactly what
         // is going on here so we could make sure we are generating the minimum
         // refresh bounds (refreshing takes time!)

         result_pbox->x0 -= 2*rc->redisplay_x_pixel;
         result_pbox->y0 -= 2*rc->redisplay_y_pixel;
         result_pbox->x1 += 2*rc->redisplay_x_pixel;
         result_pbox->y1 += 2*rc->redisplay_y_pixel;
      }

#if 0
      JTRACE("(%ld,%ld,%ld,%ld) --> ", result_pbox->x0, result_pbox->y0, result_pbox->x1, result_pbox->y1);

      PBOX b = *result_pbox;
      rc->pbox_to_screen(&b, TRUE);
      RECT r;
      rc->convert_pbox(&b, &r, NULL);

      HDC hDC = GetDC(rc->hwnd);
      FillRect(hDC, &r, (HBRUSH)GetStockObject(GRAY_BRUSH));
      ReleaseDC(rc->hwnd, hDC);

      JTRACE("(%d,%d,%d,%d)\n", r.left, r.top, r.right, r.bottom);
#endif
   }
}

/*
// Is the frame opaque?
*/

BOOL CFrameObject::is_opaque(RedisplayContextPtr rc)
{
   return m_fEditing && m_hbmEditBackground != NULL && rc->IsScreen();
}

BOOL CFrameObject::IsEmpty()
{
   BOOL bIsEmpty = TRUE;
   ERRORCODE      errorcode;
   CFrameRecord      *pFrameRecord;

   pFrameRecord = LockFrameRecord(&errorcode);
   ASSERT(pFrameRecord);
   if(pFrameRecord == NULL || errorcode != ERRORCODE_None)
      return bIsEmpty;

   bIsEmpty = (pFrameRecord->NumberOfCharacters() > 0 ? FALSE : TRUE);
   pFrameRecord->release();
   return bIsEmpty;
}

PageObjectState* CFrameObject::CreateState(void)
{
   return new CFrameObjectState;
}

void CFrameObject::SaveState(PageObjectState* pState)
{
	CFrameObjectState* pMyState = (CFrameObjectState*)pState;

   INHERITED::SaveState(pState);

	pMyState->m_FRecord = m_FRecord;
   pMyState->m_lWarpField = 0;		// Default

	CFrameRecord* pFrameRecord = LockFrameRecord();
	if (pFrameRecord != NULL)
	{
		pMyState->m_lWarpField = pFrameRecord->GetWarpFieldRecord();
		pFrameRecord->release();
	}

	// If the frame has a warp field, increment its reference count to account
	// for the added reference to it in the state.
	pMyState->m_lWarpField = pFrameRecord->GetWarpFieldRecord();

	if (pMyState->m_lWarpField > 0)
	{
		pMyState->m_pDatabase->inc_warp_field_record(pMyState->m_lWarpField);
	}

	pMyState->m_Fill = pFrameRecord->Fill();
	pMyState->m_Outline = pFrameRecord->Outline();
	pMyState->m_Shadow = pFrameRecord->Shadow();
}

void CFrameObject::RestoreState(PageObjectState* pState)
{
	CFrameObjectState* pMyState = (CFrameObjectState*)pState;

   INHERITED::RestoreState(pState);

   m_FRecord = pMyState->m_FRecord;

	// Copy the warp field back into the frame record.

	CFrameRecord* pFrameRecord = LockFrameRecord();
	if (pFrameRecord != NULL)
	{
		DB_RECORD_NUMBER lNewWarpField = pMyState->m_lWarpField;

		if (lNewWarpField != pFrameRecord->GetWarpFieldRecord())
		{
			pFrameRecord->set_new_warp_field(lNewWarpField);
		}
		pFrameRecord->release(TRUE);
	}

	pFrameRecord->Fill() = pMyState->m_Fill;
	pFrameRecord->Outline() = pMyState->m_Outline;
	pFrameRecord->Shadow() = pMyState->m_Shadow;

   calc();
}

/*
// Set whether the object is on or off the page at the moment.
*/

void CFrameObject::OnPage(BOOL fOnPage)
{
   // Pass it down.
   RectPageObject::OnPage(fOnPage);

   // If we are on the page, add our story. Otherwise, remove it.
   if (fOnPage)
   {
      AddStory();
   }
   else
   {
      RemoveStory();
   }
}

static void DrawUnderline(CTextStyle* pStyle, 
									PCOORD lBaseline, 
									const PBOX& Bound, 
									CTextDisplayList* pDisplayList)
{
	// Compute the normalized bounds for the underline rectangle
	// (no warping, flipping, or rotation.)
   PBOX UnderlineBound;

   PCOORD UnderlinePosition = pStyle->UnderlinePosition();
   PCOORD UnderlineThickness = pStyle->UnderlineThickness();

	UnderlineBound.x0 = Bound.x0;
	UnderlineBound.x1 = Bound.x1;
	UnderlineBound.y0 = lBaseline - UnderlinePosition;
	UnderlineBound.y1 = UnderlineBound.y0 + UnderlineThickness;
	// Add the underline to the display list.
	ASSERT(pDisplayList != NULL);
   if (pDisplayList != NULL)
   {
      pDisplayList->AddItem(pStyle, UnderlineBound);
   }
}
/*
// Update a frame.
*/
UpdateStatePtr CFrameObject::UpdateFrame(
   RedisplayContextPtr rc,
   PBOX_PTR pExtent,
   PBOX_PTR pClearExtent,
   BOOL fClearOut,
   CFrameObject::CUpdateState* pUpdateState,
   BOOL fIsOffscreen)
{
   // rc = redisplay content pointer
   // pExtent = bounding box of area to refresh

	FontServerPtr pFontServer = get_database()->get_font_server();
	CFrameRecord* pFrame = NULL;
	CTextRecord* pTextRecord = NULL;

	CTextDisplayList* pDisplayList = NULL;
//// need to add counter for numbered list to the CUpdateState?
//// is the update state used? see comment below.

	// Get any display list saved because we were interrupted.
	if ((pUpdateState != NULL) && (pUpdateState->m_pDisplayList != NULL))
	{
		pDisplayList = pUpdateState->m_pDisplayList;
		pUpdateState->m_pDisplayList = NULL;
		delete pUpdateState;
		pUpdateState = NULL;
	}

	// Check if we have a display list. If not, we'll have to build one.
	if (pDisplayList == NULL)
	{
		// Delete the update state since there is nothing useful in it.
		// In fact, we should never have an update state at this point.
		// In the future, we may want the process of building the display
		// list to be interruptable in which case the update state might
		// have stuff in which we are interested.
		ASSERT(pUpdateState == NULL);
		delete pUpdateState;
		pUpdateState = NULL;

	   HDC hdc = rc->destination_hdc;

		// Get the frame record associated with this frame object.
		// The frame record contains all the really useful information
		// about the properties of the frame and its content.
		ERRORCODE error;
		if ((pFrame = (CFrameRecord*)database->get_record(FrameRecord(), &error, RECORD_TYPE_Frame)) == NULL)
		{
			// Oh, well. Can't do much here, can we?
			// Note: error_update_state() frees the current update state.
			return error_update_state(pUpdateState, UPDATE_TYPE_Error, error);
		}

		// Get the text record attached to the frame.
		DB_RECORD_NUMBER lText = pFrame->TextRecord();
		ASSERT(lText != 0);
		if (lText == 0)
		{
			pFrame->release();
			return error_update_state(pUpdateState, UPDATE_TYPE_Error, error);
		}

		// Get the bounds of the frame.
		// NOTE: **** For frames, ObjectBounds() is the unrotated bounds. ****
		PBOX UnrotatedBound = pFrame->ObjectBounds();
		PBOX bound = UnrotatedBound;

	   FLAGS Flags = get_flags();

		// Compute the starting point for the flow.
		PCOORD startx = bound.x0;
		PCOORD starty = bound.y0;

		PBOX Extent = *pExtent;

		// Find start of frame.
		if ((pTextRecord = (CTextRecord*)database->get_record(lText, &error, RECORD_TYPE_Text)) == NULL)
		{
			// No text record.
			pFrame->release();
			return error_update_state(pUpdateState, UPDATE_TYPE_Error, error);
		}

		TRY
		{
			// Get index of first character.
			CHARACTER_INDEX lFirstCharacterInFrame = pFrame->FirstCharacter();
			if (lFirstCharacterInFrame != -1)
			{
				CTextIterator Iterator(pTextRecord);
				Iterator.Style().Frame(this);

				LINE_INDEX lLineIndex = -1;
				CHARACTER_INDEX lFirstCharacterInLine = -1;
				WORD_INDEX lFirstWordInLine = -1;
				WORD_INDEX lWordIndex = -1;
				CHARACTER_INDEX lFirstCharacterInWord = -1;
				PCOORD UnalignedOffset = 0;
				CHARACTER_INDEX lCharacterIndex = -1;
				PCOORD XOffset = 0;


				// Get the column data. This is needed for tabs.
				int nColumns;
				PCOORD ColumnSpacing;
				PCOORD ColumnWidth;

				pFrame->GetColumnData(&nColumns, &ColumnSpacing, &ColumnWidth, &bound);

				// Create a display list for text drawing.
				pDisplayList = new CTextDisplayList(fClearOut);

				// Initialize the state.

				// Run through all the lines.
				// We need to keep track of:
				//  - The current line.
				//  - The current word.
				//  - The current character.
				//  - The current character in the current word.

				if (lLineIndex == -1)
				{
					// First line in frame.
					lLineIndex = 0;

					// First word in frame.
					lFirstWordInLine = 0;

					// First character in frame.
					lFirstCharacterInLine = lFirstCharacterInFrame;
				}

				LINE_COUNT lLineCount = pFrame->NumberOfLines();
				
				// initially assume no bulleted or numbered lists -ces
				int nLastLevel = 0;
				int nLastBulletType = BULLET_none;
				int nLastBulletStyle = BULLET_none;

				// Index of first character and word in line.
				for (; lLineIndex < lLineCount; lLineIndex++)
				{
					CTextLine* pLine = pFrame->GetLine(lLineIndex);


					// Compute the extent of this line to see if it needs updating.

					PCOORD lBaseline;
					PCOORD lDropCapBaseline = starty+pLine->m_DropCapBaseline;

					lBaseline = starty+pLine->m_Baseline;
					bound.y0 = lBaseline-pLine->m_Ascend;
					bound.y1 = lBaseline+pLine->m_Descend;

					// Compute the bounds of the line and check if it's clipped.
					BOOL fLineClipped = FALSE;

					PBOX LineBound;
					LineBound.x0 = startx+pLine->m_ExtentLeft + pLine->m_LeftOffset;
					LineBound.x1 = startx+pLine->m_ExtentRight;
					LineBound.y0 = starty+pLine->m_Baseline-pLine->m_Ascend;
					LineBound.y1 = starty+pLine->m_Baseline+pLine->m_Descend;

					// Do a quick check of the line against the frame bounds.
					fLineClipped = !IsBoxOnBox(&LineBound, &UnrotatedBound);
					if (!fLineClipped)
					{
						// Perform a more sophisticated (and time consuming) test.

						// Get the bounding box for the warped and rotated version of the line.
						GetWarpedFlippedAndRotatedBoundingBox(&LineBound);

						fLineClipped = !IsBoxOnBox(&LineBound, &Extent);
					}

					// moved so we can update the bullet counter if we are not drawing -ces
					if (lWordIndex == -1)
					{
						// First word in line.
						lWordIndex = lFirstWordInLine;
						// First character in line.
						lFirstCharacterInWord = lFirstCharacterInLine;
						// Start width off at left margin.
						Iterator.SetPosition(lFirstCharacterInWord);

						CFixed Offset = Iterator.Style().LeftMargin();
						if (Iterator.CharacterInParagraph() == 0)
						{
							Offset += Iterator.Style().FirstLineIndent();
						}
						// bulleted list and not the first line add in bullet indent -ces
						if (Iterator.Style().BulletType() != BULLET_none && Iterator.CharacterInParagraph() != 0)
						{
							Offset += Iterator.Style().BulletIndent();
						}
						UnalignedOffset = MulFixed(Offset, PAGE_RESOLUTION);
					}
					// if start of paragraph, check the bullet counter
					if (Iterator.CharacterInParagraph() == 0 )
					{
						// if bullet type or style changed need to reset the variables
						if(  Iterator.Style().BulletType() != nLastBulletType
							|| Iterator.Style().BulletStyle() != nLastBulletStyle )
						{
							nLastBulletType = Iterator.Style().BulletType();
							nLastBulletStyle = Iterator.Style().BulletStyle();
							// if a numbered lists initialize the counter and last level
							// Should this function use the same iterator? Any advantage to this?
							if( Iterator.Style().BulletType() == BULLET_numbered || 
								Iterator.Style().BulletType() == BULLET_multilevel )
								nLastLevel = InitListCounter( Iterator.CharacterIndex() );
						}

						if( Iterator.Style().BulletType() != BULLET_none )
						{
							// if numbered just use the first element in the array to count
							if( Iterator.Style().BulletType() == BULLET_numbered )
								m_NumberedListCounter[ 0 ]++;
							// multilevel keep track of all level counters
							else if( Iterator.Style().BulletType() == BULLET_multilevel )
							{
								// increment the counter for this level
								m_NumberedListCounter[ Iterator.Style().BulletLevel()]++;
								// if they moved forward more the one level at a time increment skipped levels
								if( nLastLevel < Iterator.Style().BulletLevel() - 1 )
								{
									for( int i = nLastLevel + 1; i < Iterator.Style().BulletLevel(); i++ )
										m_NumberedListCounter[i]++;
								}
								// if we have moved back a level restart the counters for greater levels
								if( Iterator.Style().BulletLevel() < nLastLevel )
								{
									for( int i = Iterator.Style().BulletLevel() + 1; i < BULLET_LEVEL_MAX; i++ )
										m_NumberedListCounter[i] = 0;
								}
								nLastLevel = Iterator.Style().BulletLevel();
							}
						}
					}

					if(!fLineClipped )
					{
						// Line is in the drawing extent.

						// If first word in paragraph and bullet draw bullet character -ces
						if( Iterator.CharacterInParagraph() == 0  && Iterator.Style().BulletType() != BULLET_none)
						{
							PBOX BulletBound;
							BulletBound.x0 = startx;
							BulletBound.x1 = startx + pLine->m_ExtentLeft;
							BulletBound.y0 = starty+pLine->m_Baseline-pLine->m_Ascend;
							BulletBound.y1 = starty+pLine->m_Baseline+pLine->m_Descend;
							DrawBullet( &Iterator, rc, startx, lBaseline, fClearOut, BulletBound, pDisplayList);
						}

						// We need to update the line. Run through all the words.

						WORD_INDEX lWordInLine = lWordIndex - lFirstWordInLine;
						ASSERT(lWordInLine >= 0);
						for (; lWordInLine < pLine->m_lWords; lWordInLine++, lWordIndex++)
						{
							CTextWord* pWord = pFrame->GetWord(lWordIndex);

							// Don't bother drawing goofy words.

							if ( (pWord->m_wType != WORD_TYPE_break)
							 && (pWord->m_wType != WORD_TYPE_eop))
							{
								PCOORD draw_x0, draw_x1;      /* actual bounds of drawing */

								bound.x0 = startx + pWord->m_XOffset;
								bound.x1 = bound.x0 + pWord->m_Width;

								draw_x0 = bound.x0 + pWord->m_DrawLeft;
								draw_x1 = bound.x0 + pWord->m_DrawWidth;

								// Compute the bounds of the word and check if it's clipped.
								BOOL fWordClipped = FALSE;

								PBOX WordBound;
								WordBound.x0 = startx+pWord->m_XOffset+pWord->m_DrawLeft;
								WordBound.x1 = startx+pWord->m_XOffset+pWord->m_DrawWidth;
								WordBound.y0 = starty+pLine->m_Baseline-pLine->m_Ascend;
								if( pWord->m_wType == WORD_TYPE_dropcap )
									WordBound.y1 = starty+pLine->m_DropCapBaseline+pLine->m_Descend;
								else
									WordBound.y1 = starty+pLine->m_Baseline+pLine->m_Descend;

								// Do a quick check of the word against the frame bounds.
								fWordClipped = !IsBoxOnBox(&WordBound, &UnrotatedBound);
								if (!fWordClipped)
								{
									// Perform a more sophisticated (and time consuming) test.

									// Get the bounding box for the warped and rotated version of the word.
									GetWarpedFlippedAndRotatedBoundingBox(&WordBound);

									fWordClipped = !IsBoxOnBox(&WordBound, &Extent);
								}

								// Only bother with this word if it is in the draw extent.
								if (!fWordClipped)
								{
									// Word is in extent.
									// This word needs updating. Set an index to use.

									if (lCharacterIndex == -1)
									{
										// First character in word.
										lCharacterIndex = lFirstCharacterInWord;

										// First offset in word.
										XOffset = bound.x0;
									}
									CHARACTER_INDEX lCharacterInWord = lCharacterIndex - lFirstCharacterInWord;

									// Run through the text and update any visible characters.

									if (lCharacterInWord < pWord->m_lCharacters)
									{
										RECT rWord;
										IntersectBox(&WordBound, &WordBound, &rc->source_pbox);
										PBOX ScreenWordBound = WordBound;
										rc->pbox_to_screen(&ScreenWordBound, TRUE);
										rc->convert_pbox(&ScreenWordBound, &rWord, NULL);

										// Optimization: skip fClearOut draws if the screen is
										// already white in the rectangle we're going to draw in.

										if (!(fClearOut && rc->IsScreen() && is_screen_white_in(rc, rWord)))
										{
											// Set up the "previous" condition for this loop.

											while (lCharacterInWord < pWord->m_lCharacters)
											{
												CHARACTER_COUNT lCount;

												// Get a pointer to this character and find out
												// how many characters are consecutively readable
												// via this pointer. We will restrict ourselves to
												// drawing that many characters at once since
												// DrawString needs a pointer to the 'n' characters
												// to draw.

												CHARACTER_COUNT lCharactersAvailable;
												CHARACTER* cp = pTextRecord->GetCharacters(lCharacterIndex, &lCharactersAvailable);
												ASSERT(lCharactersAvailable > 0);

												Iterator.SetPosition(lCharacterIndex);

												// Draw all characters that are in the same style
												lCount = pWord->m_lCharacters - lCharacterInWord;

												if (Iterator.NextChangeStart() < lCharacterIndex + lCount)
												{
													lCount = Iterator.NextChangeStart() - lCharacterIndex;
												}

												// Only attempt what we can read directly.
												if (lCount > lCharactersAvailable)
												{
													lCount = lCharactersAvailable;
												}

												// We need to break macro characters out and draw them singly.

												if (lCount > 1)
												{
													// See if there are any macro characters in this block.
													if (*cp >= MACRO_CHARACTER)
													{
														lCount = 1;
													}
													else
													{
														// First character is not a macro.
														// Look for a macro character further on.
														for (CHARACTER_COUNT l = 1; l < lCount; l++)
														{
															if (cp[l] >= MACRO_CHARACTER)
															{
																// Stop just before this character.
																lCount = l;
																break;
															}
														}
													}
												}

												// Get the underline style.
												int nUnderline = Iterator.Style().Underline();
                                    // Save text color - we may change it for hyperlink
                                    COLOR NonHyperlinkColor = (((Iterator.Style()).m_Character).m_Fill).m_ForegroundColor;

                                    if(Iterator.Style().HyperlinkStyle() != 0)
                                    {
                                       HyperlinkData	hld;
													Iterator.Style().GetHyperlinkData(hld);
													if (hld.IsEnabled())
													{
														JTRACE("Setting style to Underline for Hyperlink\n");
														nUnderline = UNDERLINE_STYLE_Single;
														// Change text color for hyperlinks
														CPmwDoc* pDoc = (CPmwDoc*)(((PMGDatabase*)database)->GetDocument());
														if (pDoc != NULL)
														{
															CPageProperties *pPageProp = pDoc->GetPageProperties();
															if (pPageProp != NULL)
																(((Iterator.Style()).m_Character).m_Fill).m_ForegroundColor = pPageProp->GetNonVisitedLinkColor();
														}
													}
                                    }

												// General underline criteria.
												BOOL fDrawUnderline = !fClearOut && (nUnderline != UNDERLINE_STYLE_None);

												// Draw the word itself if necessary.

												CHARACTER_COUNT lResult;
												bound.x0 = XOffset;

												if (pWord->m_wType == WORD_TYPE_tab)
												{
													lResult = lCount;
													ASSERT(lResult == 1);

													// Compute the column left.
													PCOORD ColumnLeft = get_bound().x0+pLine->m_nColumn*ColumnWidth;
													if (!DrawTab(
																rc,
																pWord,
																&Iterator,
																XOffset,
																lBaseline,
																fClearOut,
																ColumnLeft,
																ColumnLeft+ColumnWidth,
																&XOffset,
																WordBound,
																&fDrawUnderline,
																UnalignedOffset,
																pDisplayList))
													{
														lResult = 0;
													}

													bound.x1 = XOffset;
												}
												else if ((pWord->m_wType == WORD_TYPE_solid) || (pWord->m_wType == WORD_TYPE_macro))
												{
													lResult = pFontServer->DrawString(
																						rc,
																						cp,
																						lCount,
																						&Iterator.Style(),
																						XOffset,
																						lBaseline,
																						fClearOut,
																						&XOffset,
																						WordBound,
																						pDisplayList);

													bound.x1 = XOffset;

													if (lResult < 0)
													{
														break;
													}
												}
												else if ( pWord->m_wType == WORD_TYPE_dropcap )
												{
													CTextStyle style = Iterator.Style();
													style.Size( pLine->m_DropCapSize );
													lResult = pFontServer->DrawString(
																						rc,
																						cp,
																						lCount,
																						&style,
																						XOffset,
																						lDropCapBaseline,
																						fClearOut,
																						&XOffset,
																						WordBound,
																						pDisplayList);

													bound.x1 = XOffset;

													if (lResult < 0)
													{
														break;
													}
												}
												else
												{
													// Fake-O results.
													lResult = lCount;

													// Compute the new position after "drawing".

													if (lWordInLine == pLine->m_lWords-1)
													{
														fDrawUnderline = FALSE;
													}
													else
													{
														int nCharacter;
														for (nCharacter = 0; nCharacter < lCount; nCharacter++)
														{
															CHARACTER Character = cp [nCharacter];
															PCOORD nDeltaX = pFontServer->GetAdvanceWidth(&Iterator.Style(), Character);
															XOffset += nDeltaX;
														}
														bound.x1 = XOffset;

														if ((nUnderline == UNDERLINE_STYLE_WordsOnly) || (lWordInLine == pLine->m_lWords))
														{
															fDrawUnderline = FALSE;
														}
													}
												}

												if (fDrawUnderline)
												{
													DrawUnderline(
														&Iterator.Style(),
														lBaseline,
														bound,
														pDisplayList);
												}
                                    // Restore text color - we may have changed it for hyperlink
                                    (((Iterator.Style()).m_Character).m_Fill).m_ForegroundColor = NonHyperlinkColor;

												// m_XOffset is already updated.
												lCharacterIndex += lResult;

												lCharacterInWord += lResult;
#if 0
												if ((lResult < lCount)
												 || rc->check_interrupt()
												 || (fIsOffscreen && (GetTickCount() > dwOffscreenTimeLimit)))
												{  
													pUpdateState->type = UPDATE_TYPE_Interrupted;
													pUpdateState->m_nTextModificationCount = pTextRecord->GetModificationCount();
													JTRACE("fBroken...");
													fBroken = TRUE;
													break;
												}
#endif
											}
                              }
                           }
				   
                        }
                     }

							// Advance the character index.
							lFirstCharacterInWord += pWord->m_lCharacters;

							// Advance the unaligned offset.
							UnalignedOffset += pWord->m_Width;

							// Start at the beginning of the word.
							lCharacterIndex = -1;
						} 
					}

					// Advance to the next line.
					lFirstWordInLine += pLine->m_lWords;
					lFirstCharacterInLine += pLine->m_lCharacters;

					// Start at the beginning of the line.
					lWordIndex = -1;
				}
			}
		}
		CATCH(CErrorcodeException, e)
		{
			pUpdateState = (CFrameObject::CUpdateState*)error_update_state(pUpdateState, UPDATE_TYPE_Error, e->m_error);
		}
		AND_CATCH_ALL(e)
		{
			pUpdateState = (CFrameObject::CUpdateState*)error_update_state(pUpdateState, UPDATE_TYPE_Error, ERRORCODE_IntError);
		}
		END_CATCH_ALL
	}

	if (pDisplayList != NULL)
	{
		TRACE( "In FrameObject DisplayFrame with a valid updateList\n" );
		if (pUpdateState == NULL)
		{
			if (!pFontServer->DrawDisplayList(this, rc, pDisplayList))
			{
				TRY
				{
					pUpdateState = new CUpdateState;
               pUpdateState->type = UPDATE_TYPE_Interrupted;
					pUpdateState->m_pDisplayList = pDisplayList;
					pDisplayList = NULL;
				}
				END_TRY
			}
		}

		delete pDisplayList;
		pDisplayList = NULL;
	}

   // Release our captives.
   if (pTextRecord != NULL)
	{
		pTextRecord->release();
	}
	if (pFrame != NULL)
	{
		pFrame->release();
	}

   return pUpdateState;
}

/*
// Draw a tab.
*/

BOOL CFrameObject::DrawTab(
   RedisplayContext* rc,
   CTextWord* pWord,
   CTextIterator* pIterator,
   PCOORD x,
   PCOORD y,
   BOOL fClearOut,
   PCOORD ColumnLeft,
   PCOORD ColumnRight,
   PCOORD* new_x,
   const PBOX& Bounds,
   BOOL* pfUnderline,
   PCOORD UnalignedOffset,
   CTextDisplayList* pDisplayList)
{
   // Assume we draw until we discover otherwise.
   BOOL fDrawn = TRUE;

   // Compute the normalized bound and the left and right.
   PBOX Bound = get_bound();
   PCOORD Left;
   PCOORD Right;
   Left = ColumnLeft;
   Right = ColumnRight;

   PBOX Extent;
   Extent.x0 = 0;
   Extent.y0 = 0;
   Extent.x1 = Bound.x1-Bound.x0;
   Extent.y1 = Bound.y1-Bound.y0;

   // Get the window width. If we have no width, there is nothing to draw.
   PCOORD LeaderWindowWidth = pWord->m_Width;

   // Compute the right edge of the window. This is the default new position.
   PCOORD LeaderWindowX1 = (pWord->m_XOffset-(ColumnLeft-Bound.x0)) + LeaderWindowWidth;
   *new_x = LeaderWindowX1;

   if (LeaderWindowWidth != 0)
   {
      // Normalize the x coordinate.
      x = x - Left;

      // Find the tab we should be drawing.
      CTextTab Tab;
      if (pIterator->Style().m_Tabs.FindTab(Extent, UnalignedOffset, &Tab))
      {
         // Unpack leader characters into a local string.
         CHARACTER LeaderString[3];
         LeaderString[0] = Tab.m_Leader[0];
         LeaderString[1] = Tab.m_Leader[1];
         LeaderString[2] = 0;

         if (LeaderString[0] == UNDERLINE_LEADER)
         {
            ASSERT(LeaderString[1] == 0);
            ASSERT(LeaderString[2] == 0);
            // We want the underline leader. Pretend we drew and underline it.
            *pfUnderline = TRUE;
         }
         // Do we have something to draw?
         else if (LeaderString[0] != 0)
         {
            // Get a pointer to the font server. We need it from here on out.

            FontServerPtr pFontServer = get_database()->get_font_server();

            // Leaders are drawn as if they were are infinite repeating string with
            // origin at 0 (frame left). Only that portion of the leader that is
            // completely within the window is drawn. This ensures that leaders
            // for vertically consecutive tabs will be aligned.

            // Compute the width of the leader in the current style.

            PCOORD LeaderWidth = 0;
            CHARACTER* pLeader = LeaderString;
            while (*pLeader != 0)
            {
               LeaderWidth += pFontServer->GetAdvanceWidth(&pIterator->Style(), *(pLeader++));
            }

            if (LeaderWidth != 0)
            {
               // Get the offset into the leader string of the start of the
               // tab leader window.

               PCOORD LeaderOffset = x % LeaderWidth;

               // Locate the first character in the leader string which
               // begins at or after the start of the tab leader window

               pLeader = LeaderString;
               while (LeaderOffset > 0)
               {
                  if (*pLeader == 0)
                  {
                     pLeader = LeaderString;
                  }
                  // The current character starts before the leader window.
                  // Account for it and move on to the next character.

                  LeaderOffset -= pFontServer->GetAdvanceWidth(&pIterator->Style(), *(pLeader++));
               }
               // Adjust the position to draw the tab leader characters by
               // the amount which was moved for any partial character at
               // the start of the tab leader window.
               x -= LeaderOffset;

               // Draw characters from the tab leader until the entire window
               // is filled. Only whole characters are drawn.

               for (;;)
               {
                  if (*pLeader == 0)
                  {
                     pLeader = LeaderString;
                  }
                  /* Check if the current character will fit. */
                  PCOORD DeltaX = pFontServer->GetAdvanceWidth(&pIterator->Style(), *pLeader);
                  if (x + DeltaX > LeaderWindowX1)
                  {
                     break;
                  }
                  // Draw the current character.
                  CHARACTER_COUNT lResult = 1;
                  if (*pLeader != ' ')
                  {
                     PCOORD DrawX = Left + x;
                     lResult = pFontServer->DrawString(
                                                rc,
                                                pLeader,
                                                1,
                                                &pIterator->Style(),
                                                DrawX, y,
                                                fClearOut,
                                                new_x,
                                                Bounds,
                                                pDisplayList);
                  }
                  if (lResult < 1)
                  {
                     // We did not finish drawing.
                     fDrawn = FALSE;
                     break;
                  }
                  // Advance to the next character.
                  x += DeltaX;
                  pLeader++;
               }
               *new_x = x;
            }
         }
      }
   }
   // Un-normalize the new x.
   *new_x = Left + *new_x;
   return fDrawn;
}

//////////////
// CFrameObject::DrawBullet() draws the bullet character based on the style's BulletType, BulletStyle and BulletIndent
//
/////////////
BOOL CFrameObject::DrawBullet( CTextIterator* pIterator, RedisplayContextPtr rc, PCOORD x, PCOORD y,
					BOOL fClearOut, const PBOX& Bounds, CTextDisplayList* pDisplayList)
{
	// make a copy of the style we need to modify it
	CTextStyle currentStyle = pIterator->Style();

	// array to hold the bullet character or numbered string to display
	CHARACTER strBullet[40];
	// numbre of bullet character in the bull array
	unsigned char nCount = 0;
	// temporarily change the bullet level so the bullet lines up with the last level.
	ASSERT( currentStyle.BulletLevel() >= 0 && currentStyle.BulletLevel() < BULLET_LEVEL_MAX );
	currentStyle.BulletLevel( currentStyle.BulletLevel() -1 );
	// offset for the bullet character
	PCOORD xlocation = x + currentStyle.BulletIndent();
	// reset the level
	currentStyle.BulletLevel( currentStyle.BulletLevel() + 1 );

	if( currentStyle. BulletType() == BULLET_symbol )
	{
		if ( currentStyle.BulletStyle() == BULLET_symbol_mix )
		{
			// set the bullet character
			if( currentStyle.BulletLevel() == 0 || currentStyle.BulletLevel() == 4 )
				strBullet[nCount++] = BULLET_circle;
			else if( currentStyle.BulletLevel() == 1 || currentStyle.BulletLevel() == 3)
				strBullet[nCount++] = BULLET_square;
			else if( currentStyle.BulletLevel() == 2 )
				strBullet[nCount++] = BULLET_diamond;
		}
		else 	// get the bullet from the style
			strBullet[nCount++] = currentStyle.BulletCharacter();
	}
	else if( currentStyle.BulletType() == BULLET_numbered )
	{
		if( currentStyle.BulletStyle() & BULLET_letter )
		{
			// convert the number in pnCounter to letter(s)
			GetNumberedLetter( strBullet, m_NumberedListCounter[0] );
		}
		else			
		{
			// convert the number in pnCounter to a string
			_i64tow(  m_NumberedListCounter[0], &strBullet[nCount], 10 );
		}
		nCount = wcslen( strBullet );
		// draw the ending period or parenthese
		if( currentStyle.BulletStyle() == BULLET_letter_paren
			|| currentStyle.BulletStyle() == BULLET_arabic_paren )
			strBullet[nCount++] = ')';
		else
			strBullet[nCount++] = '.';
	}
	else if ( currentStyle.BulletType() == BULLET_multilevel )
	{
		// I.  A. 1. a. i. BULLET_roman_mix
		if( currentStyle.BulletStyle() == BULLET_roman_mix ) 
		{
			switch( currentStyle.BulletLevel() )
			{
				case 0:					// I.
				{
					GetRomanNumeral( strBullet, m_NumberedListCounter[0] );
					break;
				}
				case 1:					// A.
				{
					GetNumberedLetter( strBullet, m_NumberedListCounter[1] );
					break;
				}						// 1.
				case 2:
				{
					_i64tow(  m_NumberedListCounter[2], &strBullet[nCount], 10 );
					break;
				}
				case 3:					// a.
				{
					GetNumberedLetter( strBullet, m_NumberedListCounter[3] );
					for( CHARACTER *p = strBullet; *p != '\0'; p++ )
						*p = tolower( *p );
					break;
				}
				case 4:					// i.
				{
					GetRomanNumeral(strBullet, m_NumberedListCounter[4] );
					for( CHARACTER *p = strBullet; *p != '\0'; p++ )
						*p = tolower( *p );
					break;
				}
			}
			nCount = wcslen( strBullet );
			strBullet[nCount++] = '.';
		}
		else
		{
			// concatinate the numbers in the bulletcounter array
			for( int k = 0; k <= currentStyle.BulletLevel(); k++ )
			{
				_i64tow(  m_NumberedListCounter[k], &strBullet[nCount], 10 );
				nCount = wcslen( strBullet );
				strBullet[nCount++] = '.';
			}
		}
	}

	PMGFontServer *pFontServer = dynamic_cast<PMGFontServer*>(get_database()->get_font_server());
	ASSERT( pFontServer );
	// if a bullet symbol use the symbol font
	if( currentStyle. BulletType() == BULLET_symbol )
	{
		int nFace = typeface_server.find_face(kSymbolFont, FALSE);
		if( nFace != -1 )
		{
			DB_RECORD_NUMBER Face = pFontServer->font_face_to_record(nFace);
			currentStyle.Font( Face );
		}
	}

	long lRet = 0;
	if( nCount > 0 )
		lRet = pFontServer->DrawString( rc, strBullet, nCount, &currentStyle, xlocation,
										y, fClearOut, &xlocation, Bounds, pDisplayList);
	return( lRet > 0 );
}


/*
// Set the frame's editing state.
*/

void CFrameObject::SetEditing(BOOL fEditing)
{
   m_fEditing = fEditing;
   if (!fEditing)
   {
      m_hbmEditBackground = NULL;   // Make sure we aren't confused.
   }
}

/*
// Set the edit background info.
*/

void CFrameObject::SetEditBackground(HBITMAP hbmBackground, RECT rBitmap)
{
   m_hbmEditBackground = hbmBackground;
   m_rEdit = rBitmap;
}

// Return the bounding box of a bounds after it has been warped and rotated.
BOOL CFrameObject::GetWarpedFlippedAndRotatedBoundingBox(PBOX* pBox)
{
	BOOL fResult = FALSE;

   // Get any warping, flipping, and rotation.
   CWarpField* pWarpField = GetWarpField();
	BOOL fXFlipped = (get_flags() & OBJECT_FLAG_xflipped) != 0;
	BOOL fYFlipped = (get_flags() & OBJECT_FLAG_yflipped) != 0;
   ANGLE dRotation = get_rotation();

   if (pWarpField != NULL)
   {
		int nShift = 0;

      // We have warping and possibly rotation, build an outline path,
      // munge it, and find its bounding box. The bounding bos is in PCOORDs
		// (1800 dpi) which we have to convert to CFixed in order to warp. To
		// prevent overflows, we have to scale things down if they are out of range.

      PBOX UnrotatedBound = get_unrotated_bound();

		while (!SafeToConvertPboxToFixed(pBox) || !SafeToConvertPboxToFixed(&UnrotatedBound))
		{
			nShift++;
			pBox->x0 >>= 1;
			pBox->y0 >>= 1;
			pBox->x1 >>= 1;
			pBox->y1 >>= 1;
			UnrotatedBound.x0 >>= 1;
			UnrotatedBound.y0 >>= 1;
			UnrotatedBound.x1 >>= 1;
			UnrotatedBound.y1 >>= 1;
		}

#ifdef _DEBUG
		if (nShift > 0)
		{
			JTRACE("Scaling down PCOORDS (%d)\n", nShift);
		}
#endif

      COutlinePath Path;
      CFixed lx0 = MakeFixed(pBox->x0);
      CFixed ly0 = MakeFixed(pBox->y0);
      CFixed lx1 = MakeFixed(pBox->x1);
      CFixed ly1 = MakeFixed(pBox->y1);
      Path.MoveTo(lx0, ly0);
      Path.LineTo(lx1, ly0);
      Path.LineTo(lx1, ly1);
      Path.LineTo(lx0, ly1);
      Path.LineTo(lx0, ly0);
      Path.Close();
      Path.End();

      // Transform the warp field for our unrotated bounding box in page coordinates.
		UnrotatedBound.x0 >>= nShift;
		UnrotatedBound.x1 >>= nShift;
		UnrotatedBound.y0 >>= nShift;
		UnrotatedBound.y1 >>= nShift;
      RECT UnrotatedBoundRect;
      RedisplayContext::convert_pbox(&UnrotatedBound, &UnrotatedBoundRect, NULL);
      pWarpField->Transform(UnrotatedBoundRect);

      // Warp the path data.
      Path.Warp(pWarpField);

		fResult = TRUE;		// path is warped

      // Create a transformation matrix that embodies and X and Y
      // flipping along with any rotation. Remember, PMG maintains
      // object properties so that the final object position can
      // always be reached by applying flipping first and then
      // rotation.
      COutlinePathMatrix RotateMatrix;
      RotateMatrix.Identity();

		// Flip the character if necessary.
      if (fXFlipped)
      {
         RotateMatrix.XFlip(MakeFixed(UnrotatedBoundRect.left), MakeFixed(UnrotatedBoundRect.right));
      }
      if (fYFlipped)
      {
         RotateMatrix.YFlip(MakeFixed(UnrotatedBoundRect.top), MakeFixed(UnrotatedBoundRect.bottom));
      }

      // If there's any rotation, rotate the path.
      if (dRotation != 0.0)
      {
			int nOriginX = (int)(get_origin().x >> nShift);
			int nOriginY = (int)(get_origin().y >> nShift);

         RotateMatrix.TranslateBy(-MakeFixed(nOriginX), -MakeFixed(nOriginY));
         RotateMatrix.RotateBy(MakeFixed(dRotation*RAD2DEGC));
         RotateMatrix.TranslateBy(MakeFixed(nOriginX), MakeFixed(nOriginY));
      }

      // Apply any flipping and rotation.
      if (fXFlipped || fYFlipped || (dRotation != 0.0))
      {
         Path.Transform(&RotateMatrix);
      }

      // We now have the warped and rotated path, compute the bounding box.
      CFixedRect FixedBounds;
      Path.QuickBoundingBox(&FixedBounds);

      // Convert back to PBOX coordinates.
      pBox->x0 = FixedRound(FixedBounds.Left) << nShift,
      pBox->y0 = FixedRound(FixedBounds.Top) << nShift,
      pBox->x1 = FixedRound(FixedBounds.Right) << nShift,
      pBox->y1 = FixedRound(FixedBounds.Bottom) << nShift;
   }
   else if (fXFlipped || fYFlipped || (dRotation != 0.0))
   {
      // No warping, but we have flipping and/or rotation.
      PBOX UnrotatedBound = get_unrotated_bound();

		if (fXFlipped)
		{
			pBox->x0 = UnrotatedBound.x1+UnrotatedBound.x0-pBox->x0;
			pBox->x1 = UnrotatedBound.x1+UnrotatedBound.x0-pBox->x1;
		}

		if (fYFlipped)
		{
			pBox->y0 = UnrotatedBound.y1+UnrotatedBound.y0-pBox->y0;
			pBox->y1 = UnrotatedBound.y1+UnrotatedBound.y0-pBox->y1;
		}

		if (dRotation != 0.0)
		{
			compute_rotated_bound(*pBox, dRotation, pBox, get_origin());
			fResult = TRUE;		// path is rotated
		}
   }

	// Normalize the resulting box.
	if (pBox->x0 > pBox->x1)
	{
		PCOORD t = pBox->x0;
		pBox->x0 = pBox->x1;
		pBox->x1 = t;
	}
	if (pBox->y0 > pBox->y1)
	{
		PCOORD t = pBox->y0;
		pBox->y0 = pBox->y1;
		pBox->y1 = t;
	}

	return fResult;
}

void CFrameObject::Margins(const PBOX& InMargins)
{
   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      CFixedRect Margins;
      Margins.Left = PageToInches(InMargins.x0);
      Margins.Top = PageToInches(InMargins.y0);
      Margins.Right = PageToInches(InMargins.x1);
      Margins.Bottom = PageToInches(InMargins.y1);

      pFrame->Margins(Margins);
      pFrame->release(TRUE);
   }
}

PBOX CFrameObject::Margins(void)
{
   PBOX OutMargins;

   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame == NULL)
   {
      OutMargins.x0 =
      OutMargins.y0 =
      OutMargins.x1 =
      OutMargins.y1 = 0;
   }
   else
   {
      const CFixedRect& Margins = pFrame->Margins();

      OutMargins.x0 = InchesToPage(Margins.Left);
      OutMargins.y0 = InchesToPage(Margins.Top);
      OutMargins.x1 = InchesToPage(Margins.Right);
      OutMargins.y1 = InchesToPage(Margins.Bottom);

      pFrame->release(TRUE);
   }
   return OutMargins;
}

/*
// Lock the frame record.
*/

CFrameRecord* CFrameObject::LockFrameRecord(ERRORCODE* pError /*=NULL*/)
{
   return (CFrameRecord*)database->get_record(FrameRecord(), pError, RECORD_TYPE_Frame);
}

/*
// Lock the frame record.
*/

CTextRecord* CFrameObject::LockTextRecord(ERRORCODE* pError /*=NULL*/)
{
   CTextRecord* pText = NULL;
   CFrameRecord* pFrame = LockFrameRecord(pError);
   if (pFrame != NULL)
   {
      pText = pFrame->LockTextRecord(pError);
      pFrame->release();
   }
   return pText;
}

/*
// Return the text record number.
*/

DB_RECORD_NUMBER CFrameObject::TextRecord(void)
{
   DB_RECORD_NUMBER lText = 0;
   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      lText = pFrame->TextRecord();
      pFrame->release();
   }
   return lText;
}

/*
// Convert an old frame record to a new one.
*/

ERRORCODE CFrameObject::ConvertFrameRecord(const FrameObjectRecord* pOldData)
{
   ERRORCODE error;

   // Lock the old frame record.
   FramePtr pOldFrame;
   pOldFrame = (FramePtr)database->get_record(pOldData->f_record, &error, RECORD_TYPE_OldFrame);
   if (pOldFrame != NULL)
   {
      // Create a new record. We leverage the code in HandleCreationData().
      FRAMEOBJ_CREATE_STRUCT fcs;

      fcs.bound = get_bound();
      fcs.flags = pOldFrame->get_flags();
      fcs.alignment = ALIGN_left;            // for now.
      fcs.vert_alignment = ALIGN_top;        // for now.
//    fcs.LeftMargin = pOldData->LeftMargin;
//    fcs.TopMargin = pOldData->TopMargin;
//    fcs.RightMargin = pOldData->RightMargin;
//    fcs.BottomMargin = pOldData->BottomMargin;

      if ((error = HandleCreationData(&fcs)) == ERRORCODE_None)
      {
         CFrameRecord* pNewFrame = (CFrameRecord*)database->get_record(FrameRecord(), &error, RECORD_TYPE_Frame);
         if (pNewFrame != NULL)
         {
            // We now have a new frame and an old frame.
            // Convert the old frame to new.
            error = pNewFrame->FromOldFrame(pOldFrame);
            pNewFrame->release(TRUE);

            // Reflow all the text.
            CTextFlow TextFlow(this, 0, -1);
            TextFlow.FlowText();
         }
      }

      // Get rid of the old paragraph.
      ((PMGDatabase*)database)->delete_paragraph(pOldFrame->get_paragraph());
      // Get rid of the old frame record.
      pOldFrame->zap();
   }
   return error;
}

/*
// Special banner stuff.
// Stretch the frame to fit the text.
//
// This assumes that textflow has already occurred and that the following
// have been done by textflow (as a special case):
//
// (1) No line breaks have occurred except where explicit break words appear.
*/

ERRORCODE CFrameObject::StretchFrame(void)
{
   ERRORCODE error;
   PMGDatabasePtr pDatabase = get_database();

   // Lock the frame record.
   CFrameRecord* pFrame = LockFrameRecord(&error);
   if (pFrame == NULL)
   {
      return error;
   }

   // Lock the text record.
   // We need the text record to determine paragraph spacing.

   CTextRecord* pText = pFrame->LockTextRecord(&error);
   if (pText == NULL)
   {
      pFrame->release();
      return error;
   }

   BOOL fXStretch = (pFrame->FrameFlags() & FRAME_FLAG_stretch_frame) != 0;
   BOOL fYStretch = (pFrame->FrameFlags() & FRAME_FLAG_ystretch_frame) != 0;
   CTextIterator Iterator;

   TRY
   {
      Iterator.SetRecord(pText);

      Iterator.State() = pFrame->TextState();

      // Compute the maximum line width. Since textflow has already happened,
      // we can assume that the lines are as we want them and use them.
      //
      // The width for a line includes the frame margins and the paragraph
      // spacing.
      //
      // We also compute the y sizes if we are doing a y-stretch as well.

      PCOORD MaxWidth = 0;
      PCOORD Y0 = 0x7fffffff;
      PCOORD Y1 = -Y0;

      CFixed lFrameSpacing = pFrame->Margins().Left + pFrame->Margins().Right;

      LINE_COUNT lLines = pFrame->NumberOfLines();
      LINE_COUNT lThisLines = 0;
      CTextLine* pLine = NULL;
      CHARACTER_INDEX lFirstCharacterInLine = pFrame->FirstCharacter();

      WORD_INDEX lWord = 0;
      WORD_COUNT lThisWords = 0;
      CTextWord* pWord = NULL;

      for (LINE_INDEX lLine = 0; lLine < lLines; lLine++)
      {
         if (lThisLines == 0)
         {
            pLine = pFrame->GetLine(lLine, &lThisLines);
         }

         if (fYStretch)
         {
            PCOORD ThisY0 = pLine->m_Baseline - pLine->m_Ascend;
            PCOORD ThisY1 = pLine->m_Baseline + pLine->m_Descend;
            if (ThisY0 < Y0)
            {
               Y0 = ThisY0;
            }
            if (ThisY1 > Y1)
            {
               Y1 = ThisY1;
            }
         }
         // Compute the padding for this line.
         if (fXStretch)
         {
            Iterator.SetPosition(lFirstCharacterInLine);
            CFixed lSpacing = lFrameSpacing
                              + Iterator.Style().LeftMargin()
                              + Iterator.Style().RightMargin();
            if (Iterator.CharacterInParagraph() == 0)
            {
               lSpacing += Iterator.Style().FirstLineIndent();
            }
/// added CES if it's a bulleted lits and not on the first list add in bullet indent
            if (Iterator.Style().BulletType() != BULLET_none && Iterator.CharacterInParagraph() != 0)
            {
               lSpacing += Iterator.Style().BulletIndent();// * Iterator.Style().BulletLevel();
            }

            PCOORD Width = InchesToPage(lSpacing);

            // Compute the total width including all words.
            for (WORD_INDEX lWordInLine = 0; lWordInLine < pLine->m_lWords; lWordInLine++)
            {
               // Prime the words if we need to.
               if (lThisWords == 0)
               {
                  pWord = pFrame->GetWord(lWord, &lThisWords);
               }

               // Add in the width for this word.
               Width += pWord->m_Width;

               // Advance the word.
               pWord++;
               lThisWords--;
               lWord++;
            }

            // Update the largest width.
            if (Width > MaxWidth)
            {
               MaxWidth = Width;
            }
         }

         // Advance the line.
         lFirstCharacterInLine += pLine->m_lCharacters;
         pLine++;
         lThisLines--;
      }

      PCOORD new_x1 = rrecord.unrotated_bound.x1;
      PCOORD new_y1 = rrecord.unrotated_bound.y1;
      if (fXStretch)
      {
         new_x1 = rrecord.unrotated_bound.x0 + MaxWidth;
      }
      if (fYStretch)
      {
         new_y1 = rrecord.unrotated_bound.y0 + (Y1 - Y0);
      }

   /* Update the frame if necessary. */

      if (rrecord.unrotated_bound.x1 != new_x1
          || rrecord.unrotated_bound.y1 != new_y1)
      {
      /*
      // Turn us back off.
      // This assumes that we're currently selected.
      // This will cause problems if we ever aren't.
      */

         pDatabase->toggle_object(this, 0);

      /* See if we need to erase. */

         if (new_x1 < rrecord.unrotated_bound.x1)
         {
         /* The frame is getting smaller! We need to erase outside it. */
            PBOX pbox;

            get_refresh_bound(&pbox);

            pbox.x0 = new_x1;
            
            pDatabase->do_refresh_notify(&pbox, REFRESH_ERASE, this);
         }

         if (new_y1 < rrecord.unrotated_bound.y1)
         {
         /* The frame is getting smaller! We need to erase outside it. */
            PBOX pbox;

            get_refresh_bound(&pbox);

            pbox.y0 = new_y1;
            
            pDatabase->do_refresh_notify(&pbox, REFRESH_ERASE, this);
         }

      /* Set the new bound end. */

         rrecord.unrotated_bound.x1 = new_x1;
         rrecord.unrotated_bound.y1 = new_y1;

      /* Assume unrotated (CHANGE ME someday!) */

         rrecord.bound.x1 = new_x1;
         rrecord.bound.y1 = new_y1;

      /* Recalc the frame. This should do a text flow (don't stretch!). */

         DWORD dwFlags = pFrame->FrameFlags();

         pFrame->FrameFlags(dwFlags & ~(FRAME_FLAG_stretch_frame|FRAME_FLAG_ystretch_frame));
         calc();
         pFrame->FrameFlags(dwFlags);

      /* Turn us back on. */

         pDatabase->toggle_object(this, 0);
      }
   }
   CATCH(CErrorcodeException, e)
   {
      error = e->m_error;
   }
   AND_CATCH(CMemoryException, e)
   {
      error = ERRORCODE_Memory;
   }
   AND_CATCH_ALL(e)
   {
      error = ERRORCODE_IntError;
   }
   END_CATCH_ALL

   pText->release();
   pFrame->release();

   return error;
}

PCOORD CFrameObject::ComputeTextHeight(void)
{
   ERRORCODE error;
   PMGDatabasePtr pDatabase = get_database();

   // Lock the frame record.
   CFrameRecord* pFrame = LockFrameRecord(&error);
   if (pFrame == NULL)
   {
      return error;
   }

	PCOORD Height = 0;
   TRY
   {
		PCOORD Y0 = 0x7fffffff;
		PCOORD Y1 = -Y0;

      LINE_COUNT lLines = pFrame->NumberOfLines();
      LINE_COUNT lThisLines = 0;
      CTextLine* pLine = NULL;

      for (LINE_INDEX lLine = 0; lLine < lLines; lLine++)
      {
         if (lThisLines == 0)
         {
            pLine = pFrame->GetLine(lLine, &lThisLines);
         }

			PCOORD ThisY0 = pLine->m_Baseline - pLine->m_Ascend;
			PCOORD ThisY1 = pLine->m_Baseline + pLine->m_Descend;
			if (ThisY0 < Y0)
			{
				Y0 = ThisY0;
			}
			if (ThisY1 > Y1)
			{
				Y1 = ThisY1;
			}

         // Advance the line.
         pLine++;
         lThisLines--;
      }
		Height = Y1-Y0;
   }
   CATCH(CErrorcodeException, e)
   {
      error = e->m_error;
   }
   AND_CATCH(CMemoryException, e)
   {
      error = ERRORCODE_Memory;
   }
   AND_CATCH_ALL(e)
   {
      error = ERRORCODE_IntError;
   }
   END_CATCH_ALL

   pFrame->release();

   return Height;
}

BOOL CFrameObject::CanStretch(void)
{
	BOOL fCanStretch = FALSE;
	// Cannot stretch the main banner panel.
	// The main calendar panel is the same, but we shouldn't be getting
	// here with such a value - and it's not stretchable anyway.
	if (get_panel() != MAIN_BANNER_PANEL)
	{
		CFrameRecord* pFrame = LockFrameRecord();
		if (pFrame != NULL)
		{
			if (pFrame->PreviousFrame() == 0 && pFrame->NextFrame() == 0)
			{
				fCanStretch = TRUE;
			}
			pFrame->release();
		}
	}
	return fCanStretch;
}

BOOL CFrameObject::IsStretched(void)
{
	BOOL fIsStretched = FALSE;
	CFrameRecord* pFrame = LockFrameRecord();
	if (pFrame != NULL)
	{
		fIsStretched = (pFrame->FrameFlags() & FRAME_FLAG_stretch_text) != 0;
		pFrame->release();
	}
	return fIsStretched;
}

/*
// Reflow macros for the story in this frame.
*/

ERRORCODE CFrameObject::ReflowMacros(CWordArray* pMacros /*=NULL*/)
{
   ERRORCODE error;
   // Lock the text record.
   CTextRecord* pText = LockTextRecord(&error);
   if (pText != NULL)
   {
      // Reflow the macros with this frame as the object.
      error = pText->ReflowMacros(pMacros, FrameRecord() == pText->FirstFrame() ? this : NULL);
      pText->release();
   }
   return error;
}

/////////////////////////////////////////////////////////////////////////////
// Story list support.

/*
// Make sure we are removed from the story list.
*/

void CFrameObject::RemoveStory(void)
{
   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      get_database()->RemoveStory(pFrame->TextRecord());
      pFrame->release();
   }
}

/*
// Make sure we are in the story list.
*/

void CFrameObject::AddStory(void)
{
   CFrameRecord* pFrame = LockFrameRecord();
   if (pFrame != NULL)
   {
      get_database()->AddStory(pFrame->TextRecord());
      pFrame->release();
   }
}

/*
// Compute the intersections of this object with all following objects.
// Returns whether things changed or not.
*/

BOOL CFrameObject::ComputeIntersections(BOOL fReflow /*=TRUE*/)
{
   BOOL fChanged = FALSE;

   // Only generic frames have intersections.
   if (GenericFrame())
   {
      CFrameRecord* pFrame = LockFrameRecord();
      if (pFrame != NULL)
      {
         // In order to optimize the need for reflow, we will build the
         // intersections in a new path. If they are not equal, we will
         // assign them over and reflow.

         COutlinePath Path;

         // Empty the array.
         Path.End();

         // Get our bound for clipping purposes.
         PBOX Bound = get_bound();

         // Run through all the objects after us and compute the intersections.
         PMGPageObject* pObject = this;
         while ((pObject = (PMGPageObject*)pObject->next_object()) != NULL)
         {
            PBOX ObjectBound = pObject->get_bound();
            if (pObject->GetFlowAround() != FLOW_AROUND_none
                     && IsBoxOnBox(&ObjectBound, &Bound))
            {
               pObject->AddOutline(Path, &Bound);
            }
         }

			ANGLE dRotation = get_rotation();
			if (dRotation != 0.0)
			{
				COutlinePathMatrix Matrix;
				Matrix.Identity();

				PCOORD XOffset = PageToInches(get_origin().x);
				PCOORD YOffset = PageToInches(get_origin().y);

				Matrix.TranslateBy(-XOffset, -YOffset);
				Matrix.RotateBy(MakeFixed(-dRotation*RAD2DEGC));
				Matrix.TranslateBy(XOffset, YOffset);

				Path.Transform(&Matrix);
			}
#if 0			
			// inflate the path to add white space around avoided objects
			ETextWrapGutter eGutter = pFrame->GetGutter();
			switch( eGutter )
			{
				case GUTTER_tight:
					Path.Inflate( MakeFixed( WRAP_TIGHT ) ) ;
					break;
				case GUTTER_medium:
					Path.Inflate( MakeFixed( WRAP_MEDIUM ) ) ;
					break;
				case GUTTER_loose:
					Path.Inflate( MakeFixed( WRAP_LOOSE ) ) ;
					break;
				default:
					break;
			}
#endif
         // Now, see if things changed.
         COutlinePath& CurrentPath = pFrame->Intersections();
         if (!CurrentPath.Compare(Path))
         {
            // Things have changed.
            CurrentPath.Assign(&Path);
            fChanged = TRUE;

            // If a reflow is desired...
            if (fReflow)
            {
               // Reflow the text for the whole frame.
               CHARACTER_INDEX lFirst = pFrame->FirstCharacter();
               if (lFirst != -1)
               {
                  // We have some text in this frame.
                  CHARACTER_INDEX lLast = lFirst+pFrame->NumberOfCharacters();

                  CTextFlow TextFlow(this, lFirst, lLast);
                  TextFlow.FlowText();
               }
            }
         }
         pFrame->release(TRUE);
      }
   }
   return fChanged;
}

CWarpField* CFrameObject::GetWarpField()
{
   CWarpField* pWarpField = NULL;

   CFrameRecord* pFrameRecord = LockFrameRecord();
   if (pFrameRecord != NULL)
   {
      pWarpField = pFrameRecord->GetWarpField();
      pFrameRecord->release();
   }

   return pWarpField;
}

/*
// Get the properties for this object.
*/

void CFrameObject::GetObjectProperties(CObjectProperties& Properties)
{
	// Stick in the warp field.
	CFrameRecord* pFrameRecord = LockFrameRecord();
   if (pFrameRecord != NULL)
   {
		Properties.SetWarpField(pFrameRecord->GetWarpFieldRecord());

		// Fill
		Properties.SetFillPattern(pFrameRecord->Fill().m_nPattern);
		Properties.SetFillForegroundColor(pFrameRecord->Fill().m_ForegroundColor);		

		// Outline
		Properties.SetOutlineStyle(pFrameRecord->Outline().m_nStyle);
		Properties.SetOutlineWidthType(pFrameRecord->Outline().m_nWidthType);
		Properties.SetOutlineWidth(pFrameRecord->Outline().m_lWidth);
		Properties.SetOutlinePattern(pFrameRecord->Outline().m_nPattern);
		Properties.SetOutlineForegroundColor(pFrameRecord->Outline().m_ForegroundColor);

		// Shadow
		Properties.SetShadowStyle(pFrameRecord->Shadow().m_nStyle);
		Properties.SetShadowOffsetType(pFrameRecord->Shadow().m_nOffsetType);
		Properties.SetShadowXOffset(pFrameRecord->Shadow().m_lXOffset);
		Properties.SetShadowYOffset(pFrameRecord->Shadow().m_lYOffset);
		Properties.SetShadowPattern(pFrameRecord->Shadow().m_nPattern);
		Properties.SetShadowForegroundColor(pFrameRecord->Shadow().m_ForegroundColor);

      pFrameRecord->release();
   }
}

/*
// Set the properties for this object.
*/

BOOL CFrameObject::SetObjectProperties(CObjectProperties& Properties)
{
	BOOL fSet = FALSE;
	SHORT n;
	CFixed l;
	COLOR c;

	CFrameRecord* pFrameRecord = LockFrameRecord();
	if (pFrameRecord != NULL)
	{
		DB_RECORD_NUMBER lWarpField;
		if (Properties.ApplyWarpField(lWarpField))
		{
			pFrameRecord->set_new_warp_field(lWarpField);
		}

		fSet = TRUE;

		// Fill

		c = pFrameRecord->Fill().m_ForegroundColor;
		fSet |= Properties.ApplyFillForegroundColor(c);
		Properties.ApplyFillType(n);
		if (n == FillFormatV1::FillSolid)
		{
			pFrameRecord->Fill().m_ForegroundColor = c;
		}

		n = pFrameRecord->Fill().m_nPattern;
		if (c != UNDEFINED_COLOR && c != TRANSPARENT_COLOR && n == FillFormatV1::None)
		{
			n = FillFormatV1::Black;
		}
		fSet |= Properties.ApplyFillPattern(n);
		pFrameRecord->Fill().m_nPattern = n;

		// Outline

		n = pFrameRecord->Outline().m_nStyle;
		fSet |= Properties.ApplyOutlineStyle(n);
		pFrameRecord->Outline().m_nStyle = n;

		n = pFrameRecord->Outline().m_nWidthType;
		fSet |= Properties.ApplyOutlineWidthType(n);
		pFrameRecord->Outline().m_nWidthType = n;

		l = pFrameRecord->Outline().m_lWidth;
		fSet |= Properties.ApplyOutlineWidth(l);
		pFrameRecord->Outline().m_lWidth = l;

		n = pFrameRecord->Outline().m_nPattern;
		fSet |= Properties.ApplyOutlinePattern(n);
		pFrameRecord->Outline().m_nPattern = n;

		c = pFrameRecord->Outline().m_ForegroundColor;
		fSet |= Properties.ApplyOutlineForegroundColor(c);
		pFrameRecord->Outline().m_ForegroundColor = c;

		// Shadow

		n = pFrameRecord->Shadow().m_nStyle;
		fSet |= Properties.ApplyShadowStyle(n);
		pFrameRecord->Shadow().m_nStyle = n;

		n = pFrameRecord->Shadow().m_nOffsetType;
		fSet |= Properties.ApplyShadowOffsetType(n);
		pFrameRecord->Shadow().m_nOffsetType = n;

		l = pFrameRecord->Shadow().m_lXOffset;
		fSet |= Properties.ApplyShadowXOffset(l);
		pFrameRecord->Shadow().m_lXOffset = l;

		l = pFrameRecord->Shadow().m_lYOffset;
		fSet |= Properties.ApplyShadowYOffset(l);
		pFrameRecord->Shadow().m_lYOffset = l;

		n = pFrameRecord->Shadow().m_nPattern;
		fSet |= Properties.ApplyShadowPattern(n);
		pFrameRecord->Shadow().m_nPattern = n;

		c = pFrameRecord->Shadow().m_ForegroundColor;
		fSet |= Properties.ApplyShadowForegroundColor(c);
		pFrameRecord->Shadow().m_ForegroundColor = c;

		pFrameRecord->release();
	}

   return fSet;
}

CFrameObject::CUpdateState::CUpdateState()
{
	m_pDisplayList = NULL;
}

CFrameObject::CUpdateState::~CUpdateState()
{
	delete m_pDisplayList;
}
//////////////////////////
//		CFrameObject::InitListCounter
// This function is used to initialize the numberd list counter for numbered
// and multilevel lists. If the list is not a multilevel list only 
// m_NumberedListCounter[0] is used to keep count. Otherwise counts are kept at all
// levels up to BULLET_LEVEL_MAX.
// This function first 0's the counter array and then steps backwards through paragraphs
// incrementing the counter until a paragraph is found that has a different
// bulleted, numbered, list type.
//
// The function returns the bullet indent level of the paragraph just before the
// CHARACTER_INDEX passed. This is needed to accurately count multilevel lists.
//
// This function was written to support lists that flow from one frame to another.
int CFrameObject::InitListCounter( CHARACTER_INDEX charIndex )
{
   CTextRecord       *pTextRecord;
   ERRORCODE         errorcode;

	int nFirstLevel = 0;
	for( int i = 0; i < BULLET_LEVEL_MAX; i++ )
		m_NumberedListCounter[i] = 0;

	TRY
	{
 		pTextRecord = LockTextRecord(&errorcode);
		ASSERT(pTextRecord);
		if(pTextRecord == NULL || errorcode != ERRORCODE_None)
		{
			return 0;
		}

		// set up the text iterator
		CTextIterator textIterator;
		textIterator.SetRecord(pTextRecord);
		textIterator.Style().Frame(this);
		textIterator.SetPosition(charIndex);

		CTextStyle currentStyle = textIterator.Style();
//		ASSERT( currentStyle.BulletType() == BULLET_numbered || 
//				currentStyle.BulletType() == BULLET_multilevel );


		int nLastLevel = nFirstLevel = currentStyle.BulletLevel();
	
		CHARACTER_INDEX paraIndex = textIterator.PreviousParagraphStart();
		while(paraIndex >= 0)
		{
			textIterator.SetPosition(paraIndex);
			CTextStyle prevStyle = textIterator.Style();
			// if the bullet type or style has changed we're done counting
			if( prevStyle.BulletType() != currentStyle.BulletType() ||
				prevStyle.BulletStyle() != currentStyle.BulletStyle() )
				break;
			// else update the counter
			if( prevStyle.BulletType() == BULLET_multilevel )
			{
				if( nLastLevel >= prevStyle.BulletLevel() )
				{
					nLastLevel = prevStyle.BulletLevel();
					m_NumberedListCounter[ nLastLevel ]++;
				}
			}
			else //simple numbered list we don't care about level just increment
				m_NumberedListCounter[ 0 ]++;

			paraIndex = textIterator.PreviousParagraphStart();
		}

	}  // Close brace for try
	CATCH(CErrorcodeException, e)
	{
		errorcode = e->m_error;
	}
	AND_CATCH(CMemoryException, e)
	{
		errorcode = ERRORCODE_Memory;
	}
	AND_CATCH_ALL(e)
	{
		errorcode = ERRORCODE_IntError;
	}
	END_CATCH_ALL

   // Done with text record
   pTextRecord->release();
   pTextRecord = NULL;

   return nFirstLevel;
}

		 