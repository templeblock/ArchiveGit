/*
// $Header: /PM8/App/OLDDOC.CPP 1     3/03/99 6:08p Gbeddow $
//
// Support for conversion of old PMG documents.
//
// $Log: /PM8/App/OLDDOC.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:23:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:26   Fred
// Initial revision.
// 
//    Rev 1.4   01 Nov 1996 17:11:44   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.3   28 Jul 1996 11:58:04   Jay
// 
//    Rev 1.2   26 Jun 1996 14:21:40   Jay
//  
// 
//    Rev 1.1   26 Jun 1996 09:06:50   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:42:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:22   FRED
// Initial revision.
// 
//    Rev 2.4   04 Sep 1995 14:24:44   JAY
// Fixed old card documents with new panel types.
// 
//    Rev 2.3   09 Jun 1995 10:47:46   JAY
// Improvements to preview code.
// 
//    Rev 2.2   07 Jun 1995 13:14:42   JAY
// Added destructor to OldProjectPreview to free string memory (memory leak).
// 
//    Rev 2.1   05 Jun 1995 11:31:06   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:58   JAY
// Initial revision.
// 
//    Rev 1.13   30 Nov 1994 16:33:36   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.12   06 Oct 1994 13:34:46   JAY
// Margin info is now passed around for card panels.
// 
//    Rev 1.11   09 Jun 1994 11:02:36   JAY
// Now NULLs strings before reading them (new convention).
// 
//    Rev 1.10   23 May 1994 08:31:08   JAY
// new_graphic_record() now takes a GRAPHIC_CREATE_STRUCT pointer, not just the
// name.
// 
//    Rev 1.9   18 Mar 1994 09:53:00   MACDUFF
// Adds new-new data: line spacing for text.
// 
//    Rev 1.8   28 Feb 1994 10:51:38   JAY
// New arg to create_helper() to only allow Unknown or illegal types at certain
// (not general) times.
// 
//    Rev 1.7   01 Feb 1994 17:44:16   JAY
// Converts MicroLogic codes to Ansi codes at the document conversion level.
// 
//    Rev 1.6   31 Jan 1994 07:14:40   JAY
// Removed od('s for release version.
// 
//    Rev 1.5   28 Jan 1994 17:10:46   JAY
//  
// 
//    Rev 1.4   25 Jan 1994 15:18:28   JAY
// More
// 
//    Rev 1.3   19 Jan 1994 17:06:28   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:02   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:32   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:46   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include "pmgdb.h"

#include "prevrec.h"
#include "docrec.h"
#include "fontrec.h"
#include "pagerec.h"
#include "framerec.h"
#include "pararec.h"
#include "textrec.h"
#include "layrec.h"

#include	"patobj.h"
#include	"grafobj.h"
#include	"grpobj.h"
#include	"frameobj.h"
#include	"backobj.h"
#include	"dateobj.h"
#include	"calobj.h"

#include "olddoc.h"
#include "action.h"
#include "textflow.h"

//
// Pm2Ugl
//
// maps PM character index to Microsoft UGL code.
//

static uint16 far PM2Ugl[] =
{
#if 1
0x0000, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660,    149,
0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
0x25ba, 0x25c4, 0x2195, 0x203c, 0x00b6, 0x00a7, 0x25ac, 0x21a8,
0x2191, 0x2193, 0x2192, 0x2190, 0x0000, 0x2194, 0x25b2, 0x25bc,
0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026,    146,
0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
   145, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2302,
0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x0000, 0x00a5, 0x20a7,    131,
0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
0x03b1, 0x0000, 0x0393, 0x03c0, 0x2211, 0x03c3, 0x00b5, 0x03c4,
0x03a6, 0x0398, 0x2126, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
0x00b0, 0x2022, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x0000, 0x0000,
0x00fe, 0x00d5, 0x00da,    159,    151, 0x00de, 0x2219, 0x00cc,
0x00f5,    132, 0x0060,    136, 0x00c1, 0x00e3, 0x00d0, 0x00f0,
0x00cd, 0x00d3,    154, 0x00be, 0x2212,    152, 0x00af, 0x00dd,
0x00fd, 0x00a8, 0x00db, 0x00b3, 0x00cb, 0x00ca, 0x00d9, 0x00b4,
0x00a4, 0x00a3,    135,    138,    147, 0x0000, 0x0000, 0x0131,
0x0027,    140,    148, 0x0000, 0x00c2, 0x00c8, 0x0000, 0x00ae,
   137,    156, 0x0000, 0x2017, 0x0000,    139, 0x0000,    134,
0x00d8, 0x00d7, 0x0000, 0x00c0, 0x00f8, 0x00b9, 0x00a6,    153,
   150,    133, 0x00b8, 0x00a9, 0x00df, 0x00d4, 0x25a0, 0x0000,
0x0000,    155, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2206,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x2202, 0x0000, 0x2260, 0x0000, 0x0000, 0x00c3, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x00cf, 0x00ce, 0x0000, 0x0000,
0x0000, 0x221f, 0x0000, 0x222b, 0x0000, 0x0000, 0x00d2, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000,    130, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x02d8, 0x02c7, 0x02d9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x25ca, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2215, 0x0000, 0x0000,
0x0000, 0x0000, 0x220f
#else
0x0000, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
0x25ba, 0x25c4, 0x2195, 0x203c, 0x00b6, 0x00a7, 0x25ac, 0x21a8,
0x2191, 0x2193, 0x2192, 0x2190, 0x0000, 0x2194, 0x25b2, 0x25bc,
0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x2019,
0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
0x2018, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2302,
0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x0000, 0x00a5, 0x20a7, 0x0192,
0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
0x03b1, 0x0000, 0x0393, 0x03c0, 0x2211, 0x03c3, 0x00b5, 0x03c4,
0x03a6, 0x0398, 0x2126, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
0x00b0, 0x2022, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x0000, 0x0000,
0x00fe, 0x00d5, 0x00da, 0x0178, 0x2014, 0x00de, 0x2219, 0x00cc,
0x00f5, 0x201e, 0x0060, 0x02c6, 0x00c1, 0x00e3, 0x00d0, 0x00f0,
0x00cd, 0x00d3, 0x0161, 0x00be, 0x2212, 0x02dc, 0x00af, 0x00dd,
0x00fd, 0x00a8, 0x00db, 0x00b3, 0x00cb, 0x00ca, 0x00d9, 0x00b4,
0x00a4, 0x00a3, 0x2021, 0x0160, 0x201c, 0x0000, 0x0000, 0x0131,
0x0027, 0x0152, 0x201d, 0x0000, 0x00c2, 0x00c8, 0x0000, 0x00ae,
0x2030, 0x0153, 0x0000, 0x2017, 0x0000, 0x2039, 0x0000, 0x2020,
0x00d8, 0x00d7, 0x0000, 0x00c0, 0x00f8, 0x00b9, 0x00a6, 0x2122,
0x2013, 0x2026, 0x00b8, 0x00a9, 0x00df, 0x00d4, 0x25a0, 0x0000,
0x0000, 0x203a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2206,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x2202, 0x0000, 0x2260, 0x0000, 0x0000, 0x00c3, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x00cf, 0x00ce, 0x0000, 0x0000,
0x0000, 0x221f, 0x0000, 0x222b, 0x0000, 0x0000, 0x00d2, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x201a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x02d8, 0x02c7, 0x02d9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x25ca, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2215, 0x0000, 0x0000,
0x0000, 0x0000, 0x220f
#endif
};

/*
// Translate from "MicroLogic font sequence" to ANSI.
*/

PRIVATE CHARACTER near
translate_char(CHARACTER ch)
{
	if (ch > 0x8000)
	{
		return ch;
	}

	// Text font -- use the regular mapping

	if (ch > (sizeof PM2Ugl) / sizeof PM2Ugl[0])
	{
		if (ch >= 545)
		{
			ch -= 545;
			if (ch >= (0x7f-0x21))
			{
				return 0xf0a1 + (ch - (0x7f-0x21));
			}
			else
			{
			/* First Dingbat. */
				return 0xf021 + ch;
			}
		}
		return 42;
	}

	if (PM2Ugl[ch] == 0)
	{
		return 42;
	}
	return PM2Ugl[ch];
}

/*
// Convert an old document's objects by reading them into the database.
*/

ERRORCODE PMGDatabase::convert_old_document(WORD signature, StorageDevicePtr old_device, ProjectInfo *info, LPCRECT margins)
{
	OldPMGDocument old_document(old_device);
	ERRORCODE error;
/*
// Initialize the old document.
*/

	if ((error = old_document.init()) == ERRORCODE_None)
	{
		error = old_document.convert(this, info, margins);
	}

	return error;
}

/**********************/
/* Old object methods */
/**********************/

PRIVATE OLD_OBJECT_CREATOR
object_creator[] =
{
	OldGraphicObject::create,
	OldFrameObject::create,
	OldPatternObject::create,
	OldBorderObject::create,
	OldGroupObject::create
};

/*
// Read an old object list.
// This is used by both the Page record and the group object.
*/

PRIVATE ERRORCODE
read_old_object_list(StorageDevicePtr device, OldObjectPtr far *list)
{
	ERRORCODE error;

	*list = NULL;

	for (;;)
	{
		SHORT objsize;
		OLD_OBJECT_TYPE type_to_create;

	/* Read the type of the next object. */

		if ((error = device->read(&type_to_create, sizeof(type_to_create))) != ERRORCODE_None)
		{
			return error;
		}

	/* See if we're done. */

		if (type_to_create == -2)
		{
		/* All done! */
			return ERRORCODE_None;
		}

	/* See if we need to read the size. */

		objsize = -1;

		if (type_to_create & OBJTYPE_FLAG_SavesSize)
		{
		/* Read the size. */

			if ((error = device->read(&objsize, sizeof(objsize))) != ERRORCODE_None)
			{
				return error;
			}
			type_to_create = (OLD_OBJECT_TYPE)(type_to_create & ~OBJTYPE_FLAG_SavesSize);
		}

	/* Validate the type. */

		if ((type_to_create < 0) || (type_to_create >= NUMBER_OF_OLD_TYPES))
		{
			return ERRORCODE_IllegalType;
		}

	/* Create the object. */

		OldObjectPtr object;

		if ((object = (object_creator[type_to_create])(type_to_create)) == NULL)
		{
			return ERRORCODE_Memory;
		}

	/* Invoke the read. */

		if ((error = object->read(device, objsize)) != ERRORCODE_None)
		{
			return error;
		}

	/* Append it to the list. */

		object->next = NULL;

		*list = object;
		list = &object->next;
	}
}

/*
// Free an old object list.
*/

PRIVATE VOID
free_old_object_list(OldObjectPtr far *list)
{
	OldObjectPtr o, next;

	for (o = *list; o != NULL; o = next)
	{
		next = o->next;

		delete o;
	}
	*list = NULL;
}

/*
// The old base object read method.
*/

ERRORCODE OldObject::read(StorageDevicePtr device, SHORT size)
{
	return device->read(&record, sizeof(record));
}

/*
// The old graphic object constructor.
*/

OldGraphicObject::OldGraphicObject(OLD_OBJECT_TYPE type) : OldObject(type)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// The old graphic object create method.
*/

OldObjectPtr OldGraphicObject::create(OLD_OBJECT_TYPE type)
{
	return new OldGraphicObject(type);
}

/*
// The old graphic object read method.
*/

ERRORCODE OldGraphicObject::read(StorageDevicePtr device, SHORT size)
{
	ERRORCODE error;

	if ((error = OldObject::read(device, size)) == ERRORCODE_None)
	{
		error = device->read(&grecord, sizeof(grecord));
	}
	return error;
}

/*
// The old border object constructor.
*/

OldBorderObject::OldBorderObject(OLD_OBJECT_TYPE type) : OldGraphicObject(type)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// The old border object create method.
*/

OldObjectPtr OldBorderObject::create(OLD_OBJECT_TYPE type)
{
	return new OldBorderObject(type);
}

/*
// The old border object read method.
*/

ERRORCODE OldBorderObject::read(StorageDevicePtr device, SHORT size)
{
	ERRORCODE error;

	if ((error = OldGraphicObject::read(device, size)) == ERRORCODE_None)
	{
		error = device->read(&brecord, sizeof(brecord));
	}
	return error;
}

/*
// The old frame object constructor.
*/

OldFrameObject::OldFrameObject(OLD_OBJECT_TYPE type) : OldObject(type)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// The old frame object create method.
*/

OldObjectPtr OldFrameObject::create(OLD_OBJECT_TYPE type)
{
	return new OldFrameObject(type);
}

/*
// The old frame object read method.
*/

ERRORCODE OldFrameObject::read(StorageDevicePtr device, SHORT size)
{
	ERRORCODE error;
	BOOL old_type = size == -1;

	size = sizeof(frecord);

	if (old_type)
	{
	/* No refresh offsets stored. */
		size -= sizeof(PBOX);
	}

	if ((error = OldObject::read(device, size)) == ERRORCODE_None)
	{
		if ((error = device->read(&frecord, sizeof(frecord))) == ERRORCODE_None
				&& old_type)
		{
			frecord.refresh_offsets.x0 = 0;
			frecord.refresh_offsets.y0 = 0;
			frecord.refresh_offsets.x1 = record.bound.x1 - record.bound.x0;
			frecord.refresh_offsets.y1 = record.bound.y1 - record.bound.y0;
		}
	}
	return error;
}

/*
// The old pattern object constructor.
*/

OldPatternObject::OldPatternObject(OLD_OBJECT_TYPE type) : OldObject(type)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// The old pattern object create method.
*/

OldObjectPtr OldPatternObject::create(OLD_OBJECT_TYPE type)
{
	return new OldPatternObject(type);
}

/*
// The old pattern object read method.
*/

ERRORCODE OldPatternObject::read(StorageDevicePtr device, SHORT size)
{
	ERRORCODE error;

	if ((error = OldObject::read(device, size)) == ERRORCODE_None)
	{
		error = device->read(&precord, sizeof(precord));
	}
	return error;
}

/*
// The old group object constructor.
*/

OldGroupObject::OldGroupObject(OLD_OBJECT_TYPE type) : OldObject(type)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// The old group object destructor.
*/

OldGroupObject::~OldGroupObject()
{
	free_old_object_list(&grecord.list);
}

/*
// The old group object create method.
*/

OldObjectPtr OldGroupObject::create(OLD_OBJECT_TYPE type)
{
	return new OldGroupObject(type);
}

/*
// The old group object read method.
*/

ERRORCODE OldGroupObject::read(StorageDevicePtr device, SHORT size)
{
	ERRORCODE error;

	if ((error = OldObject::read(device, size)) == ERRORCODE_None)
	{
		if ((error = device->read(&grecord, sizeof(grecord))) == ERRORCODE_None)
		{
			error = read_old_object_list(device, &grecord.list);
		}
	}
	return error;
}

/**********************/
/* Old record methods */
/**********************/

PRIVATE OLD_RECORD_CREATOR
record_creator[] =
{
	NULL,										// Old old frame
	OldPage::create,
	OldGraphic::create,
	OldFontData::create,
	NULL,										// OldBitmap::create,
	OldDocument::create,
	NULL,										// Old old paragraph
	OldObjectLayout::create,
	OldCalendarInfo::create,
	OldProjectPreview::create,
	OldFrame::create,
	OldParagraph::create
};

/*
// Read an old array.
*/

PRIVATE ERRORCODE
read_array(StorageDevicePtr device, ARRAY_PTR array, SHORT element_size)
{
	array->data = NULL;

	if (array->count == 0)
	{
		return ERRORCODE_None;
	}
	else
	{
		return device->read_block(element_size*array->count, &array->data);
	}
}

/*
// Old frame creator.
*/

OldRecordPtr OldFrame::create(INODE_TYPE type)
{
	return new OldFrame(type);
}

/*
// Old frame constructor.
*/

OldFrame::OldFrame(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.line.data = NULL;
}

OldFrame::~OldFrame()
{
	delete [] (char*)record.line.data;
}

/*
// Old frame read routine.
*/

ERRORCODE OldFrame::read(StorageDevicePtr device)
{
	ERRORCODE error;
	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None)
	{
		error = read_array(device, &record.line, sizeof(PMD_LINE));
	}
	return error;
}

/*
// Old page creator.
*/

OldRecordPtr OldPage::create(INODE_TYPE type)
{
	return new OldPage(type);
}

/*
// Old page constructor.
*/

OldPage::OldPage(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

OldPage::~OldPage()
{
	free_old_object_list(&record.list);
}

/*
// Old page read routine.
*/

ERRORCODE OldPage::read(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None)
	{
		error = read_old_object_list(device, &record.list);
	}
	return error;
}

/*
// Old graphic creator.
*/

OldRecordPtr OldGraphic::create(INODE_TYPE type)
{
	return new OldGraphic(type);
}

/*
// Old graphic constructor.
*/

OldGraphic::OldGraphic(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.OldName = NULL;
	record.OldFileName = NULL;
}

/*
// Old graphic destructor.
*/

OldGraphic::~OldGraphic()
{
}

/*
// Old graphic read routine.
*/

ERRORCODE OldGraphic::read(StorageDevicePtr device)
{
	ERRORCODE error;

	converted_record = 0;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None)
	{
		record.OldName = NULL;
		record.OldFileName = NULL;

		if ((error = device->read_cstring(m_csName)) == ERRORCODE_None)
		{
			if ((error = device->read_cstring(m_csFileName)) == ERRORCODE_None)
			{
				if (record.storage == OLD_GRAPHIC_STORAGE_MEMORY)
				{
				/* Read the record. */
					SHORT size = (record.x_size+7)/8*record.y_size;
					record.bitmap = NULL;
					if ((error = device->read_block(size, (VOIDPTR far *)&record.bitmap)) != ERRORCODE_None)
					{
					/* Just ignore this. But it'll probably fail. */
						record.storage = OLD_GRAPHIC_STORAGE_FILE;
					}
				}
			}
		}
	}
	return error;
}

/*
// Old font data creator.
*/

OldRecordPtr OldFontData::create(INODE_TYPE type)
{
	return new OldFontData(type);
}

/*
// Old font data constructor.
*/

OldFontData::OldFontData(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.OldName = NULL;
}

/*
// Old font data destructor.
*/

OldFontData::~OldFontData()
{
}

/*
// Old font data read routine.
*/

ERRORCODE OldFontData::read(StorageDevicePtr device)
{
	ERRORCODE error;

	converted_font = 0;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None)
	{
		record.OldName = NULL;
		error = device->read_cstring(m_csName);
	}

	return error;
}

/*
// Old document creator.
*/

OldRecordPtr OldDocument::create(INODE_TYPE type)
{
	return new OldDocument(type);
}

/*
// Old document constructor.
*/

OldDocument::OldDocument(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// Old document read routine.
*/

ERRORCODE OldDocument::read(StorageDevicePtr device)
{
	return device->read(&record, sizeof(record));
}

/*
// Old paragraph creator.
*/

OldRecordPtr OldParagraph::create(INODE_TYPE type)
{
	return new OldParagraph(type);
}

/*
// Old paragraph constructor.
*/

OldParagraph::OldParagraph(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.text.data = NULL;
	record.word.data = NULL;
	record.change.data = NULL;
}

/*
// Old paragraph destructor.
*/

OldParagraph::~OldParagraph()
{
	delete [] (char*)record.text.data;
	delete [] (char*)record.word.data;
	delete [] (char*)record.change.data;
}

/*
// Old paragraph read routine.
*/

ERRORCODE OldParagraph::read(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None
		&& (error = read_array(device, &record.text, sizeof(CHARACTER))) == ERRORCODE_None
		&& (error = read_array(device, &record.word, sizeof(PMD_TEXT_WORD))) == ERRORCODE_None)
	{
		error = read_array(device, &record.change, sizeof(STYLE_CHANGE));
	}
	return error;
}

/*
// Old object layout creator.
*/

OldRecordPtr OldObjectLayout::create(INODE_TYPE type)
{
	return new OldObjectLayout(type);
}

/*
// Old object layout constructor.
*/

OldObjectLayout::OldObjectLayout(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.OldName = NULL;
	record.elements = NULL;

	converted_record = 0;
}

/*
// Old object layout destructor.
*/

OldObjectLayout::~OldObjectLayout(VOID)
{
/* Free all the elements. */

	OLD_LAYOUT_ELEMENT_PTR element, next;

	for (element = record.elements; element != NULL; element = next)
	{
		next = element->next;
		delete element;
	}
}

/*
// Old object layout read routine.
*/

ERRORCODE OldObjectLayout::read(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None)
	{
		SHORT count;

		record.OldName = NULL;
		record.elements = NULL;

		if ((error = device->read_cstring(m_csName)) == ERRORCODE_None
				&& (error = device->read(&count, sizeof(count))) == ERRORCODE_None)
		{
		/* Read the elements. */

			OLD_LAYOUT_ELEMENT_PTR far *e;

			for (e = &record.elements; count-- > 0;)
			{
				OLD_LAYOUT_ELEMENT element;

			/* Read the next element. */

				if ((error = device->read(&element, sizeof(element))) != ERRORCODE_None)
				{
					return error;
				}

			/* Now allocate one for it. */

				*e = NULL;
				TRY
					*e = new OLD_LAYOUT_ELEMENT;
				END_TRY

				if (*e == NULL)
				{
					return ERRORCODE_Memory;
				}

				**e = element;
				e = &(*e)->next;
			}
		}
	}
	return error;
}

/*
// Old calendar info creator.
*/

OldRecordPtr OldCalendarInfo::create(INODE_TYPE type)
{
	return new OldCalendarInfo(type);
}

/*
// Old calendar info constructor.
*/

OldCalendarInfo::OldCalendarInfo(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
}

/*
// Old calendar info read routine.
*/

ERRORCODE OldCalendarInfo::read(StorageDevicePtr device)
{
	return device->read(&record, sizeof(record));
}

/*
// Old project preview creator.
*/

OldRecordPtr OldProjectPreview::create(INODE_TYPE type)
{
	return new OldProjectPreview(type);
}

/*
// Default old project preview constructor.
*/

OldProjectPreview::OldProjectPreview() : OldRecord(INODE_ProjectPreview)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.OldDescription = NULL;
	record.bitmap = NULL;
}

/*
// Old project preview constructor.
*/

OldProjectPreview::OldProjectPreview(INODE_TYPE t) : OldRecord(t)
{
/*
// Normally we would initialize any fields, but this is just going to be
// read and converted, so why bother?
*/
	record.OldDescription = NULL;
	record.bitmap = NULL;
}

/*
// Old project preview destructor.
*/

OldProjectPreview::~OldProjectPreview()
{
	delete [] record.bitmap;
}

/*
// Old project preview read routine.
*/

ERRORCODE OldProjectPreview::read(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read(&record, sizeof(record))) == ERRORCODE_None)
	{
		record.OldDescription = NULL;
		record.bitmap = NULL;
		if ((error = device->read_cstring(m_csDescription)) == ERRORCODE_None)
		{
			if (record.valid)
			{
				USHORT source_byte_width = (record.width+7)/8;
				USHORT byte_width = ((record.width+7)/8 + 1) & ~1;

				USHORT bitmap_size = byte_width*record.height;

				TRY
					record.bitmap = new BYTE[bitmap_size];
				END_TRY
				if (record.bitmap != NULL)
				{
					if (byte_width == source_byte_width)
					{
					/* Read the bitmap directly. */
						error = device->read(record.bitmap, bitmap_size);
					}
					else
					{
					/* Convert to padded rows. */
						LPBYTE p = (LPBYTE)record.bitmap;
						for (SHORT i = record.height; i-- > 0;)
						{
							if ((error = device->read(p, source_byte_width)) != ERRORCODE_None)
							{
								break;
							}
							p[source_byte_width] = 255;
							p += byte_width;
						}
					}
				}
			}
		}
	}
	return error;
}

/************************/
/* Old document methods */
/************************/

/*
// Create an old PMG document.
*/ 

OldPMGDocument::OldPMGDocument(StorageDevicePtr dev) : device(dev)
{
	secondary = NULL;
	document_inode = -1;				/* No document inode yet */

	calobj = NULL;
	skeleton_group = text_group = graphic_group = NULL;
	got_label_style = got_number_style = FALSE;
	got_calendar = TRUE;
}

/*
// Destructor for the old PMG document.
*/

OldPMGDocument::~OldPMGDocument()
{
	if (secondary != NULL)
	{
		SHORT i;

		for (i = 0; i < NUMBER_OF_SECONDARIES; i++)
		{
			free_primary(i);
		}

		delete [] (char*)secondary;
	}
}

/*
// Initialize an old PMG document.
// This involves reading in the secondaries.
// The device is assumed to be pointing past the signature and file type
// (which would point it at the start of the secondary tables).
*/

ERRORCODE OldPMGDocument::init()
{
	ERRORCODE error;
	SHORT secondary_size = sizeof(SECONDARY)*NUMBER_OF_SECONDARIES;
	SECONDARY_PTR s;
	SHORT i;

/* 
// Allocate the secondary table memory area.
*/

	if ((error = device->read_block(secondary_size, (VOIDPTR far *)&secondary)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Initialize the secondary tables to not have a memory pointer.
*/

	SHORT count = 0;

   for (s = secondary, i = 0; i < NUMBER_OF_SECONDARIES; (s++)->table = NULL, i++)
			if (s->addr != 0) count++
				;

#ifdef DEBUG_OLDDOC
	od("Number of secondaries: %d\r\n", count);
#endif

/*
// Try to find the document inode.
// It should be within the first table.
*/

	if ((error = read_primary(0)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Get a pointer to the first table.
*/

	INODE_TABLE table = secondary[0].table;
	_INODE_PTR _i;

	for (_i = table, i = 0; i < INODE_BLOCK; _i++, i++)
	{
		if (!is_free(_i) && _i->type == INODE_Document)
		{
			document_inode = i;
			break;
		}
	}

	if (document_inode == -1)
	{
	/* Document inode does not exist. */
		return ERRORCODE_DoesNotExist;
	}

#ifdef DEBUG_OLDDOC
	od("Document inode found at %d\r\n", document_inode);
#endif

/* Get the document record itself. */

	if ((document = (OldDocumentPtr)get_record(document_inode, &error)) != NULL)
	{
	/* We have the document. */
#ifdef DEBUG_OLDDOC
		od("Loaded document. Page is %d\r\n", document->record.CurrentPage);
#endif
	}

	return ERRORCODE_None;
}

/*
// Convert a layout to new format.
*/

ERRORCODE OldPMGDocument::convert_layout(INODE old_layout, SHORT type, DB_RECORD_NUMBER *layout, PMGDatabasePtr database)
{
	OldObjectLayoutPtr olp;
	ERRORCODE error;

	if ((olp = (OldObjectLayoutPtr)get_record(old_layout, &error)) == NULL)
	{
		return error;
	}

	if (olp->converted_record == 0)
	{
		List element_list(0, TRUE);

	/* Build the new element list. */

		for (OLD_LAYOUT_ELEMENT_PTR oe = olp->record.elements;
								oe != NULL;
								oe = oe->next)
		{
			LayoutElementPtr element;

			if ((element = new LayoutElement) == NULL)
			{
				return ERRORCODE_Memory;
			}

			element->record.flags = oe->flags;
			element->record.where = oe->where;
			element->record.offset = oe->offset;

			element_list.append(element);
		}

	/* We need to make a new record for this. */

		if ((olp->converted_record = database->new_layout_record((LPSTR)(LPCSTR)olp->m_csName,
							type,
							&element_list)) == 0)
		{
			return ERRORCODE_Memory;		// Return SOMETHING...
		}
	}
	else
	{
	/* Got another one, Pa... */
		database->inc_layout_record(olp->converted_record);
	}

#ifdef DEBUG_OLDDOC
	od("\t[Layout %d to %ld]\r\n", old_layout, olp->converted_record);
#endif

	*layout = olp->converted_record;

	return ERRORCODE_None;
}

/*
// Convert a style structure to the new format.
*/

ERRORCODE OldPMGDocument::convert_style(PMD_STYLE_PTR old_style, TextStyle& style)
{
	PMGDatabasePtr database = style.get_database();

	DB_RECORD_NUMBER f_record;

	convert_font(old_style->face, &f_record, database);

	style.set_face(f_record);
	style.set_font_style(old_style->fstyle);
	style.set_size(old_style->size);
	style.set_size_fraction(old_style->size_fraction);
	style.set_base_size(old_style->base_size);
	style.set_base_size_fraction(old_style->base_size_fraction);
	style.set_expansion(old_style->expansion);
	style.set_expansion_fraction(old_style->expansion_fraction);
	style.set_pattern(old_style->pattern);
	style.set_outline(old_style->outline);
	style.set_background(old_style->background);
	style.set_shadow(old_style->shadow);
	style.set_xflipped(old_style->x_flipped);
	style.set_yflipped(old_style->y_flipped);
	style.set_color(old_style->color);
	style.set_bcolor(old_style->bcolor);
	style.set_line_alignment(old_style->line_alignment);
	style.set_vertical_alignment(old_style->vertical_alignment);
	style.set_left_margin(old_style->left_margin);
	style.set_right_margin(old_style->right_margin);
	style.set_line_spacing(LINE_SPACING_OldPmg);

	style.update_font_metrics();
	return ERRORCODE_None;
}

/*
// Convert a paragraph to new format text record.
*/

ERRORCODE OldPMGDocument::convert_paragraph(INODE old_paragraph, DB_RECORD_NUMBER *paragraph, PMGDatabasePtr database, DB_RECORD_NUMBER lFirstFrame)
{
	OldParagraphPtr op;
	ERRORCODE error;
	SHORT i;

/* Get the old paragraph record. */

	if ((op = (OldParagraphPtr)get_record(old_paragraph, &error)) == NULL)
	{
		return error;
	}

/*
// Create a new paragraph in the new format.
// For the new scheme that doesn't use paragraphs anymore, we will convert
// to the paragraph, then convert the paragraph to a text record. It seems
// a little retarded, but the code exists, and old-document loading doesn't
// seem to be a time-critical operation.
*/

	ParagraphPtr new_paragraph;

	if ((new_paragraph = database->create_paragraph(NULL)) == NULL)
	{
		return database->last_creation_error();
	}

/* Convert the style over. */

	TextStyleRef style = new_paragraph->get_style();

	convert_style(&op->record.style, style);

/* Copy the pertinent stuff over. */

	CHARACTER_PTR cp;
	for (i = 0, cp = (CHARACTER_PTR)op->record.text.data; i < op->record.text.count; )
	{
		new_paragraph->insert_character(i++, translate_char(*cp++));
	}

	PMD_TEXT_WORD_PTR wp;
	for (i = 0, wp = (PMD_TEXT_WORD_PTR)op->record.word.data; i < op->record.word.count; )
	{
		new_paragraph->insert_word(i++, (TEXT_WORD_PTR)wp++);
	}

	new_paragraph->set_new_data_defaults();

	STYLE_CHANGE_PTR sp;
	for (i = 0, sp = (STYLE_CHANGE_PTR)op->record.change.data; i < op->record.change.count; )
	{
		if (sp->token == TOKEN_Face)
		{
			DB_RECORD_NUMBER f_record;
		/* Convert this face token over. */
			convert_font((INODE)sp->new_value, &f_record, database);
			sp->new_value = f_record;
		}
		new_paragraph->insert_change(i++, sp++);
	}

	// Now, we have the paragraph. Create the text record to convert to.

	CTextRecord* pTextRecord = database->NewStory();
	if (pTextRecord == NULL)
	{
		error = database->last_creation_error();
	}
	else
	{
		if ((error = pTextRecord->FromOldParagraph(new_paragraph)) == ERRORCODE_None)
		{
			pTextRecord->FirstFrame(lFirstFrame);
			*paragraph = pTextRecord->Id();
			pTextRecord->release();
		}
		else
		{
			// Get rid of the malformed thing.
			pTextRecord->zap();
		}
	}

	// Kill the intermediate paragraph.
	new_paragraph->zap();

	// And now, we are all done.

#ifdef DEBUG_OLDDOC
	od("\t[Paragraph %d to %ld]\r\n", old_paragraph, *paragraph);
#endif

	return error;
}

/*
// Convert a frame to new format.
*/

ERRORCODE OldPMGDocument::convert_frame(INODE old_frame, DB_RECORD_NUMBER *frame, OldFrameObjectPtr f, PMGDatabasePtr database)
{
	OldFramePtr of;
	ERRORCODE error;

/* Get the old frame record. */

	if ((of = (OldFramePtr)get_record(old_frame, &error)) == NULL)
	{
		return error;
	}

/* Create a new frame in the new format. */

	CFrameRecord* pNewFrame;

	if ((pNewFrame = database->create_frame(NULL)) == NULL)
	{
		return database->last_creation_error();
	}

/* Copy the pertinent stuff over. */

	pNewFrame->StretchDims(of->record.stretch_dims);
	pNewFrame->ObjectBounds(f->record.bound);
	pNewFrame->FrameFlags((DWORD)of->record.flags);

/* Duplicate the paragraph (text) now. */

	DB_RECORD_NUMBER lText;

	if ((error = convert_paragraph(of->record.paragraph, &lText, database, pNewFrame->Id())) != ERRORCODE_None)
	{
		delete pNewFrame;
		return error;
	}

	pNewFrame->TextRecord(lText);

/* And now, we're all done. */

	*frame = pNewFrame->Id();
	pNewFrame->release(TRUE);

#ifdef DEBUG_OLDDOC
	od("\t[Frame %d to %ld]\r\n", old_frame, *frame);
#endif

	return ERRORCODE_None;
}

/*
// Convert a graphic to new format.
*/

ERRORCODE OldPMGDocument::convert_graphic(INODE old_graphic, DB_RECORD_NUMBER *graphic, PMGDatabasePtr database)
{
	OldGraphicPtr og;
	ERRORCODE error;

/* Get the old graphic record. */

	if ((og = (OldGraphicPtr)get_record(old_graphic, &error)) == NULL)
	{
		return error;
	}

	if (og->converted_record == 0)
	{
	/* We need to make a new record for this. */

		GRAPHIC_CREATE_STRUCT gcs;

		gcs.m_csFileName = og->m_csFileName;
//		gcs.number = og->record.number;
//		gcs.offset = og->record.where.file_info.offset;

		if ((og->converted_record = database->new_graphic_record(&gcs)) == 0)
		{
			ERRORCODE error = database->last_creation_error();

			if (error != ERRORCODE_DoesNotExist
 						&& error != ERRORCODE_Open
						&& error != ERRORCODE_IllegalType)		/* SHP files, etc. */
			{
				return error;
			}

		/*
		// The graphic was not found. This means the file is not locateable
		// and has not been added to the database yet.
		// We will create a minimal record with the info from the old graphic
		// record and let it be missing.
 		*/

			DB_RECORD_NUMBER g_record;

			if ((g_record = database->new_graphic_record(NULL)) == 0)
			{
				return database->last_creation_error();
			}

			GraphicPtr graphic;

			if ((graphic = (GraphicPtr)database->get_record(g_record, &error, RECORD_TYPE_Graphic)) == NULL)
			{
				database->delete_record(g_record);
				return error;
			}

		/* Copy all the info over. */

			graphic->m_csName = og->m_csName;
			graphic->m_csFileName = og->m_csFileName;
//			graphic->record.type = GRAPHIC_TYPE_UNKNOWN;
			graphic->record.type = (GRAPHIC_TYPE)og->record.type;
			graphic->record.reference_count = 1;
			graphic->record.x_resolution = og->record.x_resolution;
			graphic->record.y_resolution = og->record.y_resolution;
			graphic->record.x_size = og->record.x_size;
			graphic->record.y_size = og->record.y_size;

			if (og->record.storage == OLD_GRAPHIC_STORAGE_MEMORY)
			{
			/* Copy over the bitmap. */
				graphic->record.storage = GRAPHIC_STORAGE_MEMORY;
				graphic->bitmap = og->record.bitmap;
				og->record.bitmap = NULL;
			}
			else
			{
			/* Leave it missing. */
//				graphic->record.storage = GRAPHIC_STORAGE_MISSING;
				graphic->record.storage = GRAPHIC_STORAGE_FILE;
			}

			if ((error = graphic->create_helper(TRUE)) != ERRORCODE_None)
			{
				graphic->release();
				database->delete_record(g_record);
				return error;
			}

			GraphicHelperPtr helper;
			if ((helper = graphic->my_little_helper()) != NULL)
			{
				GRAPHIC_CREATE_STRUCT gcs;

				gcs.m_csFileName = graphic->m_csFileName;
//				gcs.number = -1;
//				gcs.file = -1;
//				gcs.offset = (DWORD)-1;
				gcs.proof = TRUE;

				helper->init(&gcs);
			}

			graphic->release(TRUE);

			DocumentPtr document;

			if ((document = (DocumentPtr)database->get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) == NULL)
			{
				database->delete_record(g_record);
				return error;
			}

		/* Insert the graphic number into the document graphic array. */

			document->add_graphic(g_record);
			document->release(TRUE);

		/* This is our converted record number. */

			og->converted_record = g_record;
		}
	}
	else
	{
	/* Got another one, Pa... */
		database->inc_graphic_record(og->converted_record);
	}

#ifdef DEBUG_OLDDOC
	od("\t[Graphic %d to %ld]\r\n", old_graphic, og->converted_record);
#endif

	*graphic = og->converted_record;

	return ERRORCODE_None;
}

/*
// Convert a font to new format.
*/

ERRORCODE OldPMGDocument::convert_font(INODE old_font, DB_RECORD_NUMBER *font, PMGDatabasePtr database)
{
	OldFontDataPtr ofd;
	ERRORCODE error;

/* Get the old font record. */

	if ((ofd = (OldFontDataPtr)get_record(old_font, &error)) == NULL)
	{
		return error;
	}

/* See if we've converted this yet. */

	if (ofd->converted_font == 0)
	{
	/* We need to create a font for this. */
		if ((ofd->converted_font = database->new_font_record((LPSTR)(LPCSTR)ofd->m_csName, FALSE, &error)) == 0)
		{
			return error;
		}
	}
	else
	{
		database->inc_font_record(ofd->converted_font);
	}

#ifdef DEBUG_OLDDOC
	od("\t[Font %d to %ld]\r\n", old_font, ofd->converted_font);
#endif

	*font = ofd->converted_font;

	return error;
}

/*
// Convert the base of a rectangular object.
*/

ERRORCODE OldPMGDocument::convert_rectangle_object(OldObjectPtr object, RectPageObjectPtr new_object)
{
	new_object->set_bound(object->record.bound);
	new_object->set_flags(object->record.flags);
	new_object->set_select_flags(object->record.select_flags);
	new_object->set_panel(object->record.panel);

	if (object->record.layout > 0)
	{
		ERRORCODE error;
		DB_RECORD_NUMBER layout;

		if ((error = convert_layout(object->record.layout,
 									new_object->type(),
 									&layout,
									new_object->get_database())) != ERRORCODE_None)
		{
			return error;
		}
		new_object->set_layout(layout);
	}
	else if (object->record.layout == OLD_MANUAL_LAYOUT)
	{
	/* Manual layout! */
		new_object->set_layout(MANUAL_LAYOUT);
	}
	
	return ERRORCODE_None;
}

/*
// Get the style from a frame object.
*/

ERRORCODE OldPMGDocument::get_frame_style(INODE f_inode, CTextStyle& NewStyle)
{
	ERRORCODE error;

/* Get the old frame record. */

	OldFramePtr of;
	if ((of = (OldFramePtr)get_record(f_inode, &error)) == NULL)
	{
		return error;
	}

/* Get the old paragraph record. */

	OldParagraphPtr op;
	if ((op = (OldParagraphPtr)get_record(of->record.paragraph, &error)) == NULL)
	{
		return error;
	}

	TextStyle Style(NewStyle.Database());
	if ((error = convert_style(&op->record.style, Style)) == ERRORCODE_None)
	{
		NewStyle.FromOldStyle(Style);
	}
	return error;
}

/*
// Convert an object.
*/

ERRORCODE OldPMGDocument::convert_object(OldObjectPtr object, ObjectListPtr list)
{
	PMGDatabasePtr database = (PMGDatabasePtr)list->get_database();
	ERRORCODE error = ERRORCODE_None;

#ifdef DEBUG_OLDDOC
	od("Convert object of type %d...", object->type);
#endif

	switch (object->type)
	{
		case OBJ_GRAPHIC:
		{
#ifdef DEBUG_OLDDOC
			od("Graphic ");
#endif
			GraphicObjectPtr graphic;

			if ((graphic = database->create_graphic_object(NULL)) == NULL)
			{
				error = database->last_creation_error();
			}
			else
			{
				if ((error = convert_rectangle_object(object, graphic)) == ERRORCODE_None)
				{
					graphic->set_color(((OldGraphicObjectPtr)object)->grecord.color);
					graphic->set_bcolor(((OldGraphicObjectPtr)object)->grecord.bcolor);

					DB_RECORD_NUMBER g_record;

					if ((error = convert_graphic(((OldGraphicObjectPtr)object)->grecord.data, &g_record, database)) == ERRORCODE_None)
					{
						graphic->set_graphic_record(g_record);
					}
				}

				if (error == ERRORCODE_None)
				{
					if (project_type == PROJECT_TYPE_Card)
					{
						SHORT panel = object->record.panel;

						if (panel < 0 || panel >= 4)
						{
						/* Throw it out. We don't know where it goes. */
							graphic->destroy();
							delete graphic;
						}
						else
						{
							card_group[panel]->object_list()->append(graphic);
						}
					}
					else if (!got_calendar)
					{
						list->insert_before(graphic, calobj);
					}
					else
					{
						list->append(graphic);
					}
				}
				else
				{
					graphic->destroy();
					delete graphic;
				}

			}

			break;
		}
		case OBJ_FRAME:
		{
#ifdef DEBUG_OLDDOC
			od("Frame ");
#endif

			CFrameObject* pFrame;
			SHORT panel = object->record.panel;
			INODE f_inode = ((OldFrameObjectPtr)object)->frecord.frame;

			if (panel >= DATE_PANEL && panel < GRAPHIC_PANEL)
			{
			/* This is a date frame. We don't import these per se. */
				if (!got_number_style)
				{
					CTextStyle Style(database);

					get_frame_style(f_inode, Style);
					calobj->get_calendar().SetNumberFontStyle(Style, FALSE);
					got_number_style = TRUE;
				}
			}
			else if (panel >= LABEL_FRAME_PANEL && panel < DATE_PANEL)
			{
			/* This is a label frame. We don't import these per se. */
				if (!got_label_style)
				{
					CTextStyle Style(database);

					get_frame_style(f_inode, Style);
					calobj->get_calendar().SetLabelFontStyle(Style, FALSE);
					got_label_style = TRUE;
				}
			}
			else
			{
			/* Make a new frame for this. */
				if ((pFrame = database->create_frame_object(NULL)) == NULL)
				{
					error = database->last_creation_error();
				}
				else
				{
					if ((error = convert_rectangle_object(object, pFrame)) == ERRORCODE_None)
					{
						DB_RECORD_NUMBER lFrame;

						if ((error = convert_frame(f_inode, &lFrame, (OldFrameObjectPtr)object, database)) == ERRORCODE_None)
						{
							pFrame->FrameRecord(lFrame);

							// Make sure everything is flowed correctly.
							CTextFlow TextFlow(pFrame, 0, -1);
							TextFlow.FlowText();
						}
					}

					if (panel == TITLE_PANEL)
					{
					/*
					// Setup title frame actions.
					*/
					/* Single-click is edit. */
						pFrame->set_primary_action(ACTION_DEFAULT_EDIT);
					/* Double-click is nothing. */
						pFrame->set_secondary_action(ACTION_DO_NOTHING);
						pFrame->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_solo_select);
					}

					if (error == ERRORCODE_None)
					{
						if (project_type == PROJECT_TYPE_Card)
						{
							if (panel < 0 || panel >= 4)
							{
							/* Throw it out. We don't know where it goes. */
								pFrame->destroy();
								delete pFrame;
							}
							else
							{
								card_group[panel]->object_list()->append(pFrame);
							}
						}
						else if (!got_calendar)
						{
							list->insert_before(pFrame, calobj);
						}
						else
						{
						/* Add it to the regular list. */
							list->append(pFrame);
						}
					}
					else
					{
						pFrame->destroy();
						delete pFrame;
					}
				}
			}

			break;
		}
		case OBJ_PATTERN:
		{
#ifdef DEBUG_OLDDOC
			od("Pattern ");
#endif

			if (project_type != PROJECT_TYPE_Calendar)
			{
				PATTERN_CREATE_STRUCT pcs;

				pcs.bound = ((OldPatternObjectPtr)object)->record.bound;
				pcs.pattern = ((OldPatternObjectPtr)object)->precord.pattern;
				pcs.color = ((OldPatternObjectPtr)object)->precord.color;
				pcs.bcolor = ((OldPatternObjectPtr)object)->precord.bcolor;

				PatternObjectPtr pattern;

				if ((pattern = database->create_pattern_object(&pcs)) == NULL)
				{
					error = database->last_creation_error();
				}
				else
				{
					error = convert_rectangle_object(object, pattern);

					if (error == ERRORCODE_None)
					{
						list->append(pattern);
					}
					else
					{
						pattern->destroy();
						delete pattern;
					}
				}
			}
			break;
		}
		case OBJ_BORDER:
		{
#ifdef DEBUG_OLDDOC
			od("Border ");
#endif

			BackgroundObjectPtr background;

			if ((background = database->create_background_object(NULL)) == NULL)
			{
				error = database->last_creation_error();
			}
			else
			{
				if ((error = convert_rectangle_object(object, background)) == ERRORCODE_None)
				{
					DB_RECORD_NUMBER g_record;

					background->set_color(((OldBorderObjectPtr)object)->grecord.color);
					background->set_bcolor(((OldBorderObjectPtr)object)->grecord.bcolor);
					background->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_solo_select);
					background->add_flags(OBJECT_FLAG_no_mask);

					if ((error = convert_graphic(((OldBorderObjectPtr)object)->grecord.data, &g_record, database)) == ERRORCODE_None)
					{
						background->set_graphic_record(g_record);
					}
				}

				if (error == ERRORCODE_None)
				{
					if (project_type == PROJECT_TYPE_Card)
					{
						SHORT panel = object->record.panel;

						if (panel < 0 || panel >= 4)
						{
						/* Throw it out. We don't know where it goes. */
							background->destroy();
							delete background;
						}
						else
						{
							card_group[panel]->object_list()->append(background);
						}
					}
					else
					{
					/* Add it to the regular list. */
						list->append(background);
					}
				}
				else
				{
					background->destroy();
					delete background;
				}

			}

			break;
		}
		case OBJ_GROUP:
		{
#ifdef DEBUG_OLDDOC
			od("Group ");
#endif

		/*
		// The only old document with a group is a calendar.
		// We don't translate these groups directly.
		*/

#if 0
			GROUP_CREATE_STRUCT gcs;

			gcs.original_bound = object->record.bound;

			GroupObjectPtr group;

			if ((group = database->create_group_object(&gcs)) == NULL)
			{
				error = database->last_creation_error();
			}
			else
			{
				if ((error = convert_rectangle_object(object, group)) == ERRORCODE_None)
				{
#ifdef DEBUG_OLDDOC
					od(" Convert:\r\n");
#endif
					error = convert_list(((OldGroupObjectPtr)object)->grecord.list, group->object_list());
				}

				if (error == ERRORCODE_None)
				{
					list->append(group);
				}
				else
				{
					group->destroy();
					delete group;
				}
			}
#else
			ObjectListPtr this_list;

		/* We've hit the calendar finally! */

			got_calendar = TRUE;

			switch (object->record.panel)
			{
				case TEXT_PANEL:
				{
					this_list = text_group->object_list();
					break;
				}
				case GRAPHIC_PANEL:
				{
					this_list = graphic_group->object_list();
					break;
				}
				case SKELETON_PANEL:
				{
					this_list = skeleton_group->object_list();
					break;
				}
				default:
				{
				/* A default. Shouldn't be used. */
					this_list = list;
					break;
				}
			}

			error = convert_list(((OldGroupObjectPtr)object)->grecord.list, this_list);
#endif
			break;
		}
		default:
		{
#ifdef DEBUG_OLDDOC
			od("Unknown ");
#endif
			break;
		}
	}
#ifdef DEBUG_OLDDOC
	od("\r\n");
#endif
	return error;
}

/*
// Convert a list of objects.
// This will convert both a page's or a group's object list.
*/

ERRORCODE OldPMGDocument::convert_list(OldObjectPtr old_list, ObjectListPtr list)
{
	OldObjectPtr old_object;
	ERRORCODE error = ERRORCODE_None;

	for (old_object = old_list; old_object != NULL; old_object = old_object->next)
	{
		convert_object(old_object, list);

	/* Special case for banners. */

		if (project_type == PROJECT_TYPE_Banner && old_object->type == OBJ_FRAME)
		{
		/*
 		// An old banner only had one frame.
		// This must be it!
 		*/
			PMGPageObjectPtr object = (PMGPageObjectPtr)list->last_object();

		/* Panel so we can find it. */
			object->set_panel(MAIN_BANNER_PANEL);

		/* Select flags. */
			object->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_solo_select);

		/* Primary action. */
			object->set_primary_action(ACTION_DEFAULT_EDIT);

		/* Secondary action. */
			object->set_secondary_action(ACTION_DO_NOTHING);
		}
	}
	return error;
}

/*
// Initialize a calendar document.
// This involves creating the calendar object.
*/

ERRORCODE OldPMGDocument::init_calendar(PMGDatabasePtr database, ObjectListPtr list)
{
	INODE info_inode;
	ERRORCODE error;

/* Get the info inode. */

	if ((info_inode = document->record.info_inode) <= 0)
	{
		return ERRORCODE_InvalidRecord;
	}

/* Get the calendar info. */

	OldCalendarInfoPtr oci;

	if ((oci = (OldCalendarInfoPtr)get_record(info_inode, &error)) == NULL)
	{
		return error;
	}

	CALENDAR_CREATE_STRUCT ccs;

/* Create the calendar object. */

	ccs.info = &oci->record;

	ccs.bound.x0 = 0;
	ccs.bound.y0 = 0;
	ccs.bound.x1 = document->record.xsize;
	ccs.bound.y1 = document->record.ysize;

	ccs.full_build = FALSE;		/* Only build the groups. */

	if ((calobj = database->create_calendar_object(&ccs)) == NULL)
	{
		return database->last_creation_error();
	}

	calobj->set_panel(MAIN_CALENDAR_PANEL);
	list->append(calobj);

/* Get some pointers to the groups we care about. */

	CalendarRef calendar = calobj->get_calendar();

	skeleton_group = calendar.find_group(SKELETON_PANEL);
	text_group = calendar.find_group(TEXT_PANEL);
	graphic_group = calendar.find_group(GRAPHIC_PANEL);

/* We haven't hit the calendar object yet! */

	got_calendar = FALSE;

	return ERRORCODE_None;
}

/*
// Initialize a card document.
// This involves creating the card group objects.
*/

ERRORCODE OldPMGDocument::init_card(PMGDatabasePtr database, ObjectListPtr list, ProjectInfo *info, LPCRECT margins)
{
	PCOORD xsize, ysize;

	xsize = document->record.xsize;
	ysize = document->record.ysize;

	for (SHORT panel = 0; panel < 4; panel++)
	{
		GROUP_CREATE_STRUCT gcs;

		info->get_panel_world(&gcs.original_bound, xsize, ysize, panel, margins);

		GroupObjectPtr group;

		if ((group = database->create_group_object(&gcs)) == NULL)
		{
			return database->last_creation_error();
		}

		group->set_panel(panel);
		group->calc();

		card_group[panel] = group;

		list->append(group);
	}

	return ERRORCODE_None;
}

/*
// Convert the preview over.
//
// We only take the description since the preview will be regenerated at
// save time.
*/

ERRORCODE OldPMGDocument::convert_preview(PMGDatabasePtr new_database)
{
	ERRORCODE error;
	INODE_TABLE table = secondary[0].table;

	if (table == NULL)
	{
		return ERRORCODE_IntError;
	}

	if (table[0].type != INODE_ProjectPreview)
	{
	/* No preview to convert. */
		return ERRORCODE_None;
	}

	OldProjectPreviewPtr old_preview;
	ProjectPreviewPtr preview;

/* This is the preview! */

	if ((old_preview = (OldProjectPreviewPtr)get_record(0, &error)) == NULL)
	{
	/* Ignore this. */
		return ERRORCODE_None;
	}

	if ((preview = (ProjectPreviewPtr)new_database->get_record(PMG_PREVIEW_RECORD, &error, RECORD_TYPE_ProjectPreview)) == NULL)
	{
	/* Our new database has a problem. Don't continue. */
		return error;
	}

/* Just copy the description. */

	if (old_preview->record.valid)
	{
		preview->init_preview(old_preview->record.width,
							old_preview->record.height,
							old_preview->record.x_resolution,
							old_preview->record.y_resolution);
		if (old_preview->record.bitmap != NULL)
		{
			preview->attach_preview_bitmap(old_preview->record.bitmap);
			old_preview->record.bitmap = NULL;
		}
	}
	preview->set_description((LPSTR)(LPCSTR)old_preview->m_csDescription);
	preview->release();

	return ERRORCODE_None;
}

/*
// Convert the document.
// This involves reading the current page for the document and converting
// all the objects.
*/

ERRORCODE OldPMGDocument::convert(PMGDatabasePtr new_database, ProjectInfo *info, LPCRECT margins)
{
	OldPagePtr page;
	ERRORCODE error;

/*
// Copy the project preview info over if possible.
// Primary zero should already be read.
*/

	if ((error = convert_preview(new_database)) != ERRORCODE_None)
	{
		return error;
	}

/* Copy any relevant info over to the document record. */

	DocumentPtr new_doc;
	ObjectListPtr new_list;

	if ((new_doc = (DocumentPtr)new_database->get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) == NULL)
	{
		return error;
	}

	project_type = new_doc->get_project_type();

	new_doc->set_orientation(document->record.orientation);
	PPNT dims;

	dims.x = document->record.xsize;
	dims.y = document->record.ysize;

	new_doc->set_dimensions(dims);

/*
// Get the page we will be putting objects on.
*/

	new_list = new_doc->load_page(-1);

	if (project_type == PROJECT_TYPE_Card)
	{
		new_doc->SetProjectSubtype(CARD_Unknown);
	}
	new_doc->release(TRUE);

	if (new_list == NULL)
	{
		return ERRORCODE_NotInitialized;			// pick one!
	}

/*
// If this is a calendar, set up the calendar object now.
*/

	switch (project_type)
	{
		case PROJECT_TYPE_Calendar:
		{
			if ((error = init_calendar(new_database, new_list)) != ERRORCODE_None)
			{
				return error;
			}
			break;
		}
		case PROJECT_TYPE_Card:
		{
			if ((error = init_card(new_database, new_list, info, margins)) != ERRORCODE_None)
			{
				return error;
			}
		}
		default:
		{
			break;
		}
	}

/* Get the page of objects. */

	if ((page = (OldPagePtr)get_record(document->record.CurrentPage, &error)) == NULL)
	{
		return error;
	}

/* Convert each object in the list. */

	if ((error = convert_list(page->record.list, new_list)) == ERRORCODE_None)
	{
		switch (project_type)
		{
			case PROJECT_TYPE_Calendar:
			{
				if (calobj != NULL)
				{
					calobj->rebuild(FALSE);
				}
				break;
			}
			case PROJECT_TYPE_Card:
			{
				for (SHORT i = 0; i < 4; i++)
				{
					card_group[i]->establish(FALSE);
				}
				break;
			}
		}
	}
	return error;
}

/*
// Fix a primary table which was just read.
// This means mark all inodes as not being in memory or locked.
*/

VOID OldPMGDocument::fix_primary(INODE_TABLE table)
{
	SHORT i;

	for (i = 0; i < INODE_BLOCK; i++, table++)
	{
		table->flags &= ~(IS_LOCKED | IS_HERE);
	}
}

/*
// Read a primary table.
*/

ERRORCODE OldPMGDocument::read_primary(SHORT primary)
{
//	LONG position;
	SECONDARY_PTR s = secondary + primary;
	ERRORCODE error;

#if 0
	if (borrow_virtual_file(&position) == -1)
	{
		basic_error(routine, MAKEINTRESOURCE(IDS_OPEN));
		return -1;	
	}
#endif

	if ((error = device->seek(s->addr, ST_DEV_SEEK_SET)) == ERRORCODE_None
			&& (error = device->read_block(INODE_BLOCK_SIZE, (VOIDPTR far *)&s->table)) == ERRORCODE_None)
	{
		fix_primary(s->table);
	}

#if 0
	return_virtual_file(position);
#endif
	return error;
}

/*
// Free a primary table.
*/

VOID OldPMGDocument::free_primary(SHORT primary)
{
	SECONDARY_PTR s = secondary + primary;
	
	if (s->table != NULL)
	{
		_INODE_PTR ip;
		SHORT i;

		for (ip = s->table, i = 0; i < INODE_BLOCK; i++, ip++)
		{
			if (!is_free(ip) && is_here(ip))
			{
			/* Free the data. */
				delete ip->data;
			}
		}

		delete [] (char*)s->table;
		s->table = NULL;
	}
}

/*
// Lock a table.
// This means, make sure it's addressable in memory.
*/

ERRORCODE OldPMGDocument::lock_table(SHORT primary, INODE_TABLE_PTR table)
{
   SECONDARY_PTR s = secondary + primary;
	ERRORCODE error;

//	od( "in lock_table( primary = %d, s = %p, s->table = %p  )\r\n", primary, s, s->table );

	if( s->table == NULL )
	{
		// a new table...
		if (s->addr < 0L)
		{	
		/* We don't allocate new tables. */
			return ERRORCODE_NotAllocated;
		}
		else if (s->addr > 0L)
		{
//			od( "( reading table )\r\n" );

			if ((error = read_primary(primary)) != ERRORCODE_None)
			{
				return error;
			}
		}
		else
		{
//			od( "( UGH! )\r\n" );
			return ERRORCODE_DoesNotExist;
      }
   }

//	printf( "returning %p \r\n", s->table );

	*table = s->table;

   return ERRORCODE_None;
}

/*
// Get a record from the old PMG document.
*/

OldRecordPtr OldPMGDocument::get_record(INODE inode, LPERRORCODE perror)
{
   _INODE_PTR     t;
	SHORT				primary;
	ERRORCODE		our_error;

	if (perror == NULL)
	{
		perror = &our_error;
	}

   if (inode == -1)
   {
		*perror = ERRORCODE_InvalidRecord;
      return NULL;
   }

	primary = PRIMARY_TABLE( inode );

   if ((*perror = lock_table(primary, &t)) == ERRORCODE_None)
   {
		t += TABLE_ENTRY(inode);	

      if ((unsigned int)t->type >= NUMBER_OF_INODE_TYPES
									|| record_creator[t->type] == NULL)
      {
			*perror = ERRORCODE_IllegalType;
         return NULL;
		}

      if ( is_free(t))
      {
			*perror = ERRORCODE_NotAllocated;
         return NULL;
      }

	/* We must lock the inode flag so the table won't get swapped here! */

      set_inode_flag( t, IS_LOCKED );

      if (!is_here(t))
      {
			device->seek(t->addr, ST_DEV_SEEK_SET);

		/* Create the new record. */

			if ((t->data = (record_creator[t->type])(t->type)) == NULL)
			{
				*perror = ERRORCODE_Memory;
			}
			else
			{
			/* Read the object. This will convert old formats as required. */

				if ((*perror = t->data->read(device)) != ERRORCODE_None)
				{
					delete t->data;
					t->data = NULL;
				}
			}

         set_inode_flag(t, IS_HERE);
      }

	   return t->data;
   }
	return NULL;
}
