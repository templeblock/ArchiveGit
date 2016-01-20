//
// $Workfile: PMWPRINT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/PMWPRINT.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 35    12/01/98 12:27p Johno
// Fix for close minimized print preview crash
// From 7.01
// 
// 34    11/30/98 5:35p Johno
// Foo
// 
// 33    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 32    9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 31    9/08/98 4:53p Mwilson
// fixed remote printing
// 
// 30    9/01/98 6:21p Mwilson
// Changed placement of barcode
// 
// 29    9/01/98 11:28a Mwilson
// Changed bar code text box for remote fulfillment
// 
// 28    8/24/98 3:01p Jayn
// Uses the Windows Print Setup template (not ours).
// 
// 27    8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 26    7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
// 25    6/29/98 1:07p Psasse
// New dialog template for double-sided printing with no instruction page
// 
// 24    6/25/98 9:22a Dennis
// Changed long filenames to short
// 
// 23    6/22/98 1:10p Mwilson
// changed default print paper size for romete fulfillment
// 
// 22    6/18/98 4:06p Mwilson
// added static variable for use when printing a half fold as a quarter
// fold card.  Used for ag remote print
// 
// 21    6/09/98 10:32a Mwilson
// added code for ag remote print
// 
// 20    6/03/98 6:50p Rlovejoy
// Entry points for helpful hints.
// 
// 19    6/01/98 1:27p Mwilson
// added functions to help with personal devlivery fulfillment.
// 
// 18    5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 17    4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 16    4/24/98 6:33p Fredf
// Fixed bug with simultaneous print previews. Moved preview refresh
// extent pointer into CPmwView... it was a global variable.
// 
// 15    4/21/98 3:39p Fredf
// Reworked "reverse" printing to apply to all projects.
// 
// 14    4/20/98 9:01a Fredf
// Fixed bug where all projects were looking at "reversed" flag and
// printing out reversed!
// 
// 13    4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 12    3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 11    3/10/98 5:24p Jayn
// Master page changes
// 
// 10    3/06/98 3:59p Jayn
// Post card print code (double-sided).
// 
// 9     2/24/98 5:48p Johno
// Not my task anymore so NOTHING IS MY FAULT!
// Use #define LOOK_AND_FEEL in addrbook.h to turn on / off most changes
// 
// 8     2/20/98 7:12p Hforman
// remove param from delete_object()
// 
// 7     2/20/98 12:15p Johno
// Compile update
// 
// 6     2/17/98 4:58p Johno
// Added AllowDoubleSided parameter
// 
// 5     11/18/97 11:10a Jstoner
// enhanced calendar menu items to deal with yearly and/or multipage
// calendar projects
// 
// 4     11/06/97 12:36p Jstoner
// double sided printing mods for calendar projects
// 
// 3     11/05/97 2:31p Jstoner
// can now print multi-page, single-sided calendar projects
// todo: fix print preview and double-sided printing
// 
// 2     11/05/97 12:11p Jstoner
// 
//    Rev 1.0   14 Aug 1997 15:24:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:54   Fred
// Initial revision.
// 
//    Rev 1.34   29 Jul 1997 11:05:02   Jay
// Turns off is_postscript during print preview.
// 
//    Rev 1.33   16 Jul 1997 13:30:50   Jay
// Fixes for double-sided half-fold card printing
// 
//    Rev 1.32   11 Jul 1997 11:06:52   dennisp
// If 0 names, fill in fields will now display empty string in print dialog
// 
//    Rev 1.31   01 Jul 1997 17:16:26   johno
// Added call to IsGoodPrinter in PrintProject
// 
//    Rev 1.30   23 May 1997 17:06:46   Fred
// New effects
// 
//    Rev 1.29   28 Apr 1997 11:19:02   Jay
// 
//    Rev 1.28   21 Apr 1997 14:48:34   Jay
// Fixe crashing bug in Picture Catalog (pView == NULL).
// 
//    Rev 1.27   11 Apr 1997 11:23:36   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.26   08 Apr 1997 16:11:16   Jay
// Print Instructions control
// 
//    Rev 1.25   01 Apr 1997 16:33:40   Jay
// Beginning of half-fold cards.
// 
//    Rev 1.24   01 Apr 1997 13:16:28   Jay
// Banner preview
// 
//    Rev 1.23   26 Mar 1997 14:52:00   Jay
// Interruptable preview
// 
//    Rev 1.22   25 Mar 1997 12:40:20   Jay
// Partial interruptable preview.
// 
//    Rev 1.21   21 Mar 1997 15:48:56   Jay
// More preview changes.
// 
//    Rev 1.20   21 Mar 1997 11:10:40   Jay
// Print preview changes
// 
//    Rev 1.19   04 Mar 1997 17:03:46   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.18   08 Nov 1996 10:41:24   johno
// Oops - Got resource string "no fill in fields", but didn't save it anywhere!
// 
//    Rev 1.17   07 Nov 1996 14:02:24   johno
// Moved strings to resource DLL
// 
//    Rev 1.16   23 Oct 1996 16:32:18   johno
// Use AfxGetResourceHandle instead of AfxGetInstanceHandle (ctor)
// 
//    Rev 1.15   23 Sep 1996 10:42:12   Jay
// Reversed printout fixes.
// 
//    Rev 1.14   07 Sep 1996 09:31:48   Jay
// "Page %ld" was getting an int.
// 
//    Rev 1.13   15 Aug 1996 19:10:14   Fred
// Fixed print wizard parent problems
// 
//    Rev 1.12   07 Aug 1996 08:51:40   Jay
//  
// 
//    Rev 1.11   06 Aug 1996 16:26:30   Fred
//  
// 
//    Rev 1.10   06 Aug 1996 14:37:30   Jay
//  
// 
//    Rev 1.9   06 Aug 1996 11:17:40   Jay
//  
// 
//    Rev 1.8   31 Jul 1996 11:57:04   Fred
// 16-bit compatibility
// 
//    Rev 1.7   27 Jul 1996 10:11:24   Jay
// 
//    Rev 1.6   24 Jul 1996 18:25:32   Jay
//  
// 
//    Rev 1.5   19 Jul 1996 15:31:04   Fred
//  
// 
//    Rev 1.4   16 Jul 1996 17:31:58   Fred
// More address/sender book stuff
// 
//    Rev 1.3   16 Jul 1996 08:50:56   Fred
// Hooked up address/sender book
// 
//    Rev 1.2   24 May 1996 16:12:48   Fred
// TRACEx
// 
//    Rev 1.1   11 Apr 1996 12:26:00   FRED
// Copied from PMW2
// 
//    Rev 2.28   03 Apr 1996 09:06:40   FRED
// Fixes for DeskJet envelope printing
// 
//    Rev 2.27   24 Jan 1996 10:03:36   JAY
// New string management in Configuration
// 
//    Rev 2.26   08 Jan 1996 15:46:00   JAY
// Now uses Windows' Print Setup dialog template for WIN32.
// 
//    Rev 2.25   26 Dec 1995 10:30:16   FRED
// Project structure changes for mac port
// 
//    Rev 2.24   20 Dec 1995 09:56:28   JAY
// 32-bit fixes
// 
//    Rev 2.23   09 Oct 1995 15:15:56   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.22   06 Oct 1995 10:22:02   JAY
// New print code.
// 
//    Rev 2.21   03 Oct 1995 10:03:46   JAY
// Label/envelope printing fixes
// 
//    Rev 2.20   21 Sep 1995 13:47:28   JAY
// Changes in WIN32 to prevent "Printing" window from being disabled.
// 
//    Rev 2.19   15 Sep 1995 14:05:06   JAY
// More 32-bit changes.
// 
//    Rev 2.18   13 Sep 1995 11:48:36   JAY
// New 32-bit changes
// 
//    Rev 2.17   08 Sep 1995 09:07:44   JAY
// Now pays attention to copies in printer info.
// 
//    Rev 2.16   31 Aug 1995 10:04:18   FRED
// Miscellaneous fixes.
// 
//    Rev 2.15   30 Aug 1995 14:15:06   JAY
// Uses new PrintPaperInfo() call.
// 
//    Rev 2.14   30 Aug 1995 11:37:26   FRED
// Test print
// 
//    Rev 2.13   28 Aug 1995 20:33:02   FRED
// Print alignment
// 
//    Rev 2.12   25 Aug 1995 20:38:58   FRED
// First crack at print alignment dialog
*/

#include "stdafx.h"
#include "pmw.h"
#include "system.h"
#include "pmwdoc.h"
#include "carddoc.h"
#include "pmwview.h"
#include "pmgfont.h"
#include "typeface.h"
#include "pmwprint.h"
#include "pmwdlg.h"
#include "prntsize.h"
#include "prntrnge.h"
#include "printadv.h"
#include "utils.h"
#include "pmwini.h"

#include "macrorec.h"
#include "namerec.h"
#include "pagerec.h"
#include "textrec.h"
#include "framerec.h"

#include "frameobj.h"
#include "patobj.h"
#include "grafobj.h"
#include "lineobj.h"

#include "addrbook.h"
#include "macrostd.h"
#include "macrodlg.h"
#include "printer.h"
#include "paper.h"
#include "util.h"
#include "hint.h"

#include "pmwcfg.h"

#include <string.h>
#include <cderr.h>

#include <afxpriv.h>				// for WM_HELPHITTEST
#ifdef WIN32
#include <..\src\afximpl.h>				// for AfxGetSafeOwner
#else
#include <auxdata.h>				// for _AfxGetSafeOwner
#endif

#include "printwiz.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//used only for remote fulfillment.  Half fold cards are converted to quarter folds and then
//printed.  we need to remeber that it started out as a half fold card.
BOOL CPmwView::m_bPrintingHFasQF = FALSE;

extern BOOL standard_check_interrupt(VOIDPTR data);
extern CPalette* pOurPal;

BOOL NEAR UserAbort = FALSE;

PUBLIC BOOL want_cut_marks = TRUE;

BOOL CALLBACK AFX_EXPORT PrintAbortProc(HDC, int)
{
	MSG msg;
	while (!UserAbort &&
			::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
			return FALSE;               // Bag out if WM_QUIT received
	}
	return !UserAbort;
}

/*
// The printer interrupt check.
*/

PUBLIC BOOL printer_check_interrupt(VOIDPTR data)
{
	RedisplayContextPtr rc = (RedisplayContextPtr)data;

/*
// Since we're in Windows 3.1 and other conditions hold, we don't need
// to worry about MakeProcInstance, and we can call our AbortProc directly.
*/

	if (!PrintAbortProc(rc->destination_hdc, 0))
	{
	/*
 	// We need to call AbortDoc since we did the abort check ourselves.
	//
	// (When Windows calls our abort function and the user aborts, Windows
	// calls AbortDoc for us. We need to do it ourselves here. See the CD.)
 	*/
		::AbortDoc(rc->destination_hdc);
		return TRUE;								// Interrupted
	}
	return FALSE;									// Not interrupted
}

/*
// The printer name update routine.
// The data passed is pointer to the PrintContext.
*/

PUBLIC VOID printer_update_name(VOIDPTR object, LPCSTR name, VOIDPTR data)
{
	PrintContext *pc = (PrintContext *)data;
	char buffer[64];

	if (name == NULL)
	{
		if (!pc->m_name_shown)
		{
		/* Already set correctly. */
			return;
		}
		pc->m_name_shown = FALSE;
		name = "";
	}
	else
	{
		pc->m_name_shown = TRUE;

	/* Surround it with '()' */

		buffer[0] = '(';
		strncpy(buffer+1, name, 64-3);
		buffer[64-3] = '\0';			/* Just in case */
		strcat(buffer, ")");
		name = (LPCSTR)buffer;
	}

	pc->m_dlgstatus->SetDlgItemText(IDC_PRINTING_WHAT, name);

//	od("printing '%s', %d\r\n", name, type);
}

/*
// A little routine to see if the DC supports a particular Escape.
*/

PUBLIC BOOL SupportsEscape(CDC *pDC, int nEscape)
{
	return pDC->Escape(QUERYESCSUPPORT, sizeof(nEscape), (LPCSTR)&nEscape, NULL);
}

/*
// Add a cut mark.
*/

VOID CPmwView::add_cut_mark(PCOORD x0, PCOORD y0, PCOORD x1, PCOORD y1, SHORT pattern)
{
	PatternObjectPtr object;
	CPmwDoc *pDoc = GetDocument();

/* Fill out the pattern create struct. */

	PATTERN_CREATE_STRUCT pcs;

	pcs.bound.x0 = x0;
	pcs.bound.y0 = y0;
	pcs.bound.x1 = x1;
	pcs.bound.y1 = y1;
	pcs.pattern = pattern;
	pcs.color = COLOR_WHITE;
	pcs.bcolor = COLOR_BLACK;

/* Create a pattern object. */

	if ((object = pDoc->create_pattern_object(&pcs)) == NULL)
	{
		return;
	}

/* No panel. -1 && PATTERN means cut mark. */

	object->set_panel(-1);

/* Add it to the page list. */

	pDoc->append_object(object);
}

/*
// Add cut marks if desired.
*/

void CPmwView::add_cut_marks(BOOL left, BOOL top, BOOL right, BOOL bottom)
{
	SHORT overlap = (SHORT)((double)PAGE_TILE_OVERLAP / m_pc.m_print_scale);
	SHORT thick = (SHORT)((double)CUT_MARK_THICK / m_pc.m_print_scale);

	if (want_cut_marks)
	{
	/*
 	// Adding and removing cut marks modifies the document.
	// Save the state so we can restore it.
 	*/

		BOOL m_fIsModified = GetDocument()->IsModified();

		PBOX source = m_pc.m_source;
		PBOX bound;

		bound.x0 = source.x0 + (overlap/2) - thick;
		bound.y0 = source.y0 + (overlap/2) - thick;
		bound.x1 = source.x1 - (overlap/2) - thick;
		bound.y1 = source.y1 - (overlap/2) - thick;

		if (left)
		{
			add_cut_mark(bound.x0,
								source.y0,
								bound.x0+thick,
								source.y1,
								VERT_CUT_PATTERN);
		}
		if (top)
		{
			add_cut_mark(source.x0,
								bound.y0,
								bound.x0+thick+(overlap/2),
								bound.y0+thick,
								HORZ_CUT_PATTERN);
			add_cut_mark(bound.x1-(thick+(overlap/2)),
								bound.y0,
								source.x1,
								bound.y0+thick,
								HORZ_CUT_PATTERN);
		}
		if (right)
		{
			add_cut_mark(bound.x1-thick,
								source.y0,
								bound.x1,
								bound.y0+thick+(overlap/2),
								VERT_CUT_PATTERN);
			add_cut_mark(bound.x1-thick,
								bound.y1-(thick+(overlap/2)),
								bound.x1,
								source.y1,
								VERT_CUT_PATTERN);
		}
		if (bottom)
		{
			add_cut_mark(source.x0,
								bound.y1-thick,
								source.x1,
								bound.y1,
								HORZ_CUT_PATTERN);
		}

		// Adding and removing cut marks modifies the document.
		// Restore the state we saved.
		GetDocument()->SetModifiedFlag(m_fIsModified);
	}
}

/*
// Remove any added cut marks.
*/

void CPmwView::remove_cut_marks(void)
{
	if (want_cut_marks)
	{
		CPmwDoc *pDoc = GetDocument();
		BOOL fIsModified = pDoc->IsModified();

		PMGPageObjectPtr object, next;

		for (object = pDoc->objects(); object != NULL; object = next)
		{
			next = (PMGPageObjectPtr)object->next_object();

			if (object->type() == OBJECT_TYPE_Pattern && object->get_panel() == -1)
			{
				pDoc->delete_object(object);
			}
		}

		// Adding and removing cut marks modifies the document.
		// Restore the state we saved.
		pDoc->SetModifiedFlag(fIsModified);
	}
}

/*
// Print the current document, tiling if necessary (tiling is controlled
// by the print context).
// Copies are also handled at this level.
*/

SHORT CPmwView::OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments)

{
	CPmwDoc *pDoc = GetDocument();

	SHORT bError = TRUE;
	BOOL show_x_marks, show_y_marks;
	SHORT print_copies = pc.m_copies;
	PPNT dims = pDoc->get_dimensions();
	int32 current_page, num_pages;

/*
// Get the tile range.
*/

	LPBYTE tiles = pc.m_pPD->m_tiles;
	SHORT tiles_chosen = (tiles == NULL)
								? pc.m_tile_info.m_x_tiles*pc.m_tile_info.m_y_tiles
								: pc.m_pPD->m_tiles_chosen;

/* Determine if we need to show cut marks. */

	show_x_marks = (pc.m_tile_info.m_x_tile_overlap != 0);
	show_y_marks = (pc.m_tile_info.m_y_tile_overlap != 0);

/*
// Loop on tiles until done.
// The x axis is the primary loop. We loop on y tiles first (inner loop)
// because we want to take advantage of continuous form paper where
// possible (one long y strip down the paper). For cut-sheet (single)
// type paper, it doesn't matter much which way we loop.
// However, this is reversed if we are in landscape mode, because the
// orientation of the paper with respect to the document changes.
*/

	bError = FALSE;

	if (aCurrentDocument == 0)
	{
		current_page = 0;
	}
	else
	{
		current_page = aCurrentDocument - 1;		/* we add one later */
	}

	num_pages = print_copies * tiles_chosen;
	current_page = num_pages * (aCurrentDocument - 1) + 1;
	num_pages *= aNumDocuments;

	while (!bError && print_copies-- > 0)
	{
		SHORT tile, x_tile, y_tile;

		if (pDoc->get_orientation() == LANDSCAPE)
		{
		/*
		// Landscape mode.
		*/
		/* The y axis is the primary loop. */

			for (pc.m_source.y1 = pc.m_tile_info.m_y_tile_overlap,
 														y_tile = 0,
											tile = 0;
							y_tile != pc.m_tile_info.m_y_tiles;
							y_tile++)
			{
			/* Compute the next y strip. */

				pc.m_source.y0 = pc.m_source.y1 - pc.m_tile_info.m_y_tile_overlap;
				pc.m_source.y1 = pc.m_source.y0 + pc.m_tile_info.m_tile_height;

				if (pc.m_source.y1 > dims.y)
				{
					pc.m_source.y1 = dims.y;
				}

			/* The x axis is the secondary loop. */

				for (pc.m_source.x1 = pc.m_tile_info.m_x_tile_overlap, x_tile = 0;
									x_tile != pc.m_tile_info.m_x_tiles;
									x_tile++, tile++)
				{

				/* Compute the next y strip. */

					pc.m_source.x0 = pc.m_source.x1 - pc.m_tile_info.m_x_tile_overlap;
					pc.m_source.x1 = pc.m_source.x0 + pc.m_tile_info.m_tile_width;
					if (pc.m_source.x1 > dims.x)
					{
						pc.m_source.x1 = dims.x;
					}

					if (tiles != NULL && !tiles[tile])
					{
						continue;
					}

					if (!pc.m_tile_info.m_cut_sheet)
					{
//						last = (x_tile == pc.m_tile_info.m_x_tiles-1);
					}

					add_cut_marks(show_x_marks && x_tile != 0,
										show_y_marks && y_tile != 0,
										show_x_marks && x_tile != pc.m_tile_info.m_x_tiles - 1,
										show_y_marks && y_tile != pc.m_tile_info.m_y_tiles - 1);

					OnPrepareDC(&pc.m_dcPrint, pc.m_pInfo);

				// Set up drawing rect to entire page (in logical coordinates)
					pc.m_pInfo->m_rectDraw.SetRect(0, 0,
										pc.m_dcPrint.GetDeviceCaps(HORZRES),
									   pc.m_dcPrint.GetDeviceCaps(VERTRES));
					pc.m_dcPrint.DPtoLP(&pc.m_pInfo->m_rectDraw);

					CString strFmt;
					TRY
						LoadConfigurationString(IDS_PRINTPAGENUM, strFmt);
					END_TRY
					char szBuf[80];
					wsprintf(szBuf, strFmt, current_page, num_pages);
					pc.m_dlgstatus->SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

					if (!pc.m_fBanding)
					{
						VERIFY(pc.m_dcPrint.StartPage());
					}
					bError = OnPrint(&pc, pc.m_pInfo) || UserAbort;
					if (!pc.m_fBanding)
					{
						if (pc.m_dcPrint.EndPage() < 0)
							bError = TRUE;
					}

					remove_cut_marks();

				/* If error, stop now. */

					if (bError)
					{
						y_tile = pc.m_tile_info.m_y_tiles-1;
						break;
					}

				/* See if it's time to stop. */

//					if (++current_page > tiles_chosen)
					if (++current_page > num_pages)
					{
						break;
					}

				/* For next time around. */

//					first = last;
				}

			/* See if it's time to stop. */

//				if (current_page > tiles_chosen)
				if (current_page > num_pages)
				{
					break;
				}
			}
		}
		else
		{
		/*
		// Portrait mode.
		*/
		/* The x axis is the primary loop. */

			for (pc.m_source.x1 = pc.m_tile_info.m_x_tile_overlap, 
												x_tile = 0,
												tile = 0;
								x_tile != pc.m_tile_info.m_x_tiles;
								x_tile++)
			{
			/* Compute the next x strip. */

				pc.m_source.x0 = pc.m_source.x1 - pc.m_tile_info.m_x_tile_overlap;
				pc.m_source.x1 = pc.m_source.x0 + pc.m_tile_info.m_tile_width;

				if (pc.m_source.x1 > dims.x)
				{
					pc.m_source.x1 = dims.x;
				}

			/* The y axis is the secondary loop. */

				for (pc.m_source.y1 = pc.m_tile_info.m_y_tile_overlap, y_tile = 0;
								y_tile != pc.m_tile_info.m_y_tiles;
								y_tile++, tile++)
				{

				/* Compute the next y strip. */

					pc.m_source.y0 = pc.m_source.y1 - pc.m_tile_info.m_y_tile_overlap;
					pc.m_source.y1 = pc.m_source.y0 + pc.m_tile_info.m_tile_height;
					if (pc.m_source.y1 > dims.y)
					{
						pc.m_source.y1 = dims.y;
					}

					if (tiles != NULL && !tiles[tile])
					{
						continue;
					}

					if (!pc.m_tile_info.m_cut_sheet)
					{
//						last = (y_tile == pc.m_tile_info.m_y_tiles-1);
					}

					add_cut_marks(	show_x_marks && x_tile != 0,
										show_y_marks && y_tile != 0,
										show_x_marks && x_tile != pc.m_tile_info.m_x_tiles - 1,
										show_y_marks && y_tile != pc.m_tile_info.m_y_tiles - 1);

					OnPrepareDC(&pc.m_dcPrint, pc.m_pInfo);

				// Set up drawing rect to entire page (in logical coordinates)
					pc.m_pInfo->m_rectDraw.SetRect(0, 0,
										pc.m_dcPrint.GetDeviceCaps(HORZRES),
									   pc.m_dcPrint.GetDeviceCaps(VERTRES));
					pc.m_dcPrint.DPtoLP(&pc.m_pInfo->m_rectDraw);

					CString strFmt;
					TRY
						LoadConfigurationString(IDS_PRINTPAGENUM, strFmt);
					END_TRY
					char szBuf[80];
					wsprintf(szBuf, strFmt, current_page, num_pages);
					pc.m_dlgstatus->SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);
					ASSERT(pc.m_dlgstatus->IsWindowEnabled());

					if (!pc.m_fBanding)
					{
						VERIFY(pc.m_dcPrint.StartPage());
					}
					bError = OnPrint(&pc, pc.m_pInfo) || UserAbort;
					if (!pc.m_fBanding)
					{
						if (pc.m_dcPrint.EndPage() < 0)
							bError = TRUE;
					}
//					ret = dump_tile(&pc, first, last);

					remove_cut_marks();

				/* If an error, stop. */

					if (bError)
					{
						x_tile = pc.m_tile_info.m_x_tiles-1;
						break;
					}

				/* See if it's time to stop. */

//					if (++current_page > tiles_chosen)
					if (++current_page > num_pages)
					{
						break;
					}

				/* For next time around. */

//					first = last;
				}

			/* See if it's time to stop. */

//				if (current_page > tiles_chosen)
				if (current_page > num_pages)
				{
					break;
				}
			}
		}
	}

	return bError;
}

/*
// Multi-page entry-point (newsletter, brochure, etc).
*/

int CPmwView::OnPrintMultiPageDocument(PrintContext &pc,
						int32 aCurrentDocument, int32 aNumDocuments)
{
	CPmwDoc *pDoc = GetDocument();

	int nError = 0;
	SHORT print_copies = pc.m_copies;
	PPNT dims = pDoc->get_dimensions();

	pc.m_source.x0 = 0;
	pc.m_source.y0 = 0;
	pc.m_source.x1 = dims.x;
	pc.m_source.y1 = dims.y;

	int nFromPage = pc.m_pInfo->GetFromPage();
	int nToPage = pc.m_pInfo->GetToPage();

	// Remember the current page.
	DB_RECORD_NUMBER lCurrentPage = pDoc->CurrentPageRecord();
	int nPanel = 0;
	if (pDoc->get_project_type() == PROJECT_TYPE_HalfCard)
	{
		nPanel = ((CHalfCardDoc*)pDoc)->get_current_panel();
      BOOL fAddRefreshes = pDoc->AddRefreshes();

      pDoc->AddRefreshes(FALSE);
		((CHalfCardDoc*)pDoc)->set_panel(-2);
      pDoc->AddRefreshes(fAddRefreshes);
	}

	
	// Do the multi-page print.
	while (nError == 0 && print_copies-- > 0)
	{
		if (pc.m_fDoubleSided && nFromPage != nToPage)
		{
			CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(pDoc->get_orientation());

			for (;;)
			{
				//
				// 1. Put up the dialog telling the user we are about to begin.
				//

				CPmwDialog BeginDialog(IDD_DOUBLE_SIDED_BEGIN);
				if (BeginDialog.DoModal() != IDOK)
				{
					nError = -1;
					break;
				}

				// We need to do the double-sided thing.
				//
				// 2. Print the first half of the document.
				//

				// Print the key sheet now if it goes first.
				if ((pc.m_wDoubleSidedType & DBLPRN_FRONTKEYMASK) == DBLPRN_FrontKeyFirst)
				{
					if ((nError = PrintKeySheetFront(pc)) != 0)
					{
						break;
					}
				}
				
				// Print the document pages.
				if ((nError = PrintDocumentFront(pc)) != 0)
				{
					break;
				}

				// Print the key sheet now if it goes last.
				if ((pc.m_wDoubleSidedType & DBLPRN_FRONTKEYMASK) == DBLPRN_FrontKeyLast)
				{
					if ((nError = PrintKeySheetFront(pc)) != 0)
					{
						break;
					}
				}

				// End the first print job.
				EndPrintDoc(pc, 0);

				//
				// 3. Put up the dialog telling the user to re-insert the stack.
				//
				// Only reference the key sheet if requested.
				
				UINT uiID = 0;

				if (pPrinterInfo->GetPrintInstructions())
				{
					uiID = IDD_DOUBLE_SIDED_NOTIFY;
				}
				else
				{
					uiID = IDD_DOUBLE_SIDED_NOTIFY_NO_INSTRUCTION;
				}

				CPmwDialog NotifyDialog(uiID);
				
				if (NotifyDialog.DoModal() != IDOK)
				{
					nError = -1;
					break;
				}

				//
				// 4. Print the second half of the document.
				//

				// Start up the second print job.
				if ((nError = StartPrintDoc(pc)) != 0)
				{
					break;
				}

				// Print the key sheet now if it goes first.
				if ((pc.m_wDoubleSidedType & DBLPRN_BACKKEYMASK) == DBLPRN_BackKeyFirst)
				{
					if ((nError = PrintKeySheetBack(pc)) != 0)
					{
						break;
					}
				}

				// Print the document pages.
				if ((nError = PrintDocumentBack(pc)) != 0)
				{
					break;
				}

				// Print the key sheet now if it goes last.
				if ((pc.m_wDoubleSidedType & DBLPRN_BACKKEYMASK) == DBLPRN_BackKeyLast)
				{
					if ((nError = PrintKeySheetBack(pc)) != 0)
					{
						break;
					}
				}

				// Done. Always leave!
				break;
			}
		}
		else
		{
			nError = PrintSingleSided(nFromPage, nToPage, pc);
		}
	}

	// Reload the page we were looking at.
	if (pDoc->get_project_type() == PROJECT_TYPE_HalfCard)
	{
      BOOL fAddRefreshes = pDoc->AddRefreshes();

      pDoc->AddRefreshes(FALSE);
		((CHalfCardDoc*)pDoc)->set_panel(nPanel);
      pDoc->AddRefreshes(fAddRefreshes);
	}
	else if (lCurrentPage != pDoc->CurrentPageRecord())
	{
		pDoc->LoadPage(lCurrentPage, TRUE, FALSE);
	}

	return nError;
}

int CPmwView::PrintCurrentPage(PrintContext& pc, int nPage, BOOL fMakeBlank /*=FALSE*/)
{
	int nError = 0;
#if 0
	if (fMakeBlank)
	{
		TRACE0(">>> Print blank page <<<\n");
	}
	else if (nPage == -1)
	{
		TRACE0(">>> Print key page <<<\n");
	}
	else
	{
		TRACE1(">>> Print page %d <<<\n", nPage);
	}
#else
	OnPrepareDC(&pc.m_dcPrint, pc.m_pInfo);

	// Set up drawing rect to entire page (in logical coordinates)

	pc.m_pInfo->m_rectDraw.SetRect(0, 0,
						pc.m_dcPrint.GetDeviceCaps(HORZRES),
					   pc.m_dcPrint.GetDeviceCaps(VERTRES));
	pc.m_dcPrint.DPtoLP(&pc.m_pInfo->m_rectDraw);
	CString strFmt;
	if (!fMakeBlank)
	{
		TRY
			LoadConfigurationString(nPage == -1 ? IDS_PRINTKEYPAGE : IDS_PRINTMULTIPAGENUM, strFmt);
		END_TRY
	}
	char szBuf[80];
	wsprintf(szBuf, strFmt, (long)nPage);
	pc.m_dlgstatus->SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);
	ASSERT(pc.m_dlgstatus->IsWindowEnabled());

	if (!pc.m_fBanding)
	{
		VERIFY(pc.m_dcPrint.StartPage());
	}
	if (!fMakeBlank)
	{
		// This is not a blank page. Print the contents now.
		nError = OnPrint(&pc, pc.m_pInfo);
		if (nError == 0 && UserAbort)
		{
			nError = SP_USERABORT;
		}
	}
	if (!pc.m_fBanding)
	{
		int nThisError = pc.m_dcPrint.EndPage();
		if (nError == 0 && nThisError < 0)
		{
			nError = nThisError;
		}
	}
#endif
	return nError;
}

int CPmwView::PrintKeySheetFront(PrintContext& pc)
{
	CPmwDoc* pDoc = GetDocument();

	// Only print the key sheet if requested.
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(pDoc->get_orientation());
	if (!pPrinterInfo->GetPrintInstructions())
	{
		return 0;
	}

	int nError = -1;
	PMGDatabase* pDatabase = pDoc->get_database();

	// Create a page to use for our output.
	PageRecord* pPage = pDatabase->create_page();

	if (pPage != NULL)
	{
		// Make it the current page in the document.
		DB_RECORD_NUMBER lCurrentPage = pDoc->CurrentPageRecord();
		pDoc->LoadPage(pPage->Id(), TRUE, FALSE);

		// Add the objects to it.
		CreateKeyFrame(pc.m_wDoubleSidedType);
		CreateKeyArrow(pc.m_wDoubleSidedType);

		// Force the print to the full area.
		int nOrientation = pDoc->get_orientation();
		CPaperInfo* pPaperInfo = GET_PMWAPP()->GetPaperInfo(nOrientation);
		PPNT Dims = pPaperInfo->GetProjectDims();
		if (nOrientation == LANDSCAPE)
		{
			PCOORD t = Dims.x;
			Dims.x = Dims.y;
			Dims.y = t;
		}

		PBOX SourceSave = pc.m_source;
		pc.m_source.x0 = 0;
		pc.m_source.y0 = 0;
		pc.m_source.x1 = Dims.x;
		pc.m_source.y1 = Dims.y;

		// Print the page.
		nError = PrintCurrentPage(pc, -1);

		// Restore the saved source.
		pc.m_source = SourceSave;

		// Re-load the previous page.
		pDoc->LoadPage(lCurrentPage, TRUE, FALSE);

		// Delete the key page.
		pPage->zap();
	}
	return nError;
}

int CPmwView::PrintDocumentFront(PrintContext& pc)
{
	int nError = 0;

	if ((pc.m_wDoubleSidedType & DBLPRN_FRONTDIRMASK) == DBLPRN_FrontForward)
	{
		// Print in the forward direction.
		nError = PrintDocumentForward(pc, 0);
	}
	else
	{
		// Print in the reverse direction.
		nError = PrintDocumentBackward(pc, 0);
	}

	return nError;
}

BOOL CPmwView::CreateKeyFrame(WORD wDoubleSidedType)
{
	BOOL fResult = FALSE;
	CPmwDoc* pDoc = GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();
	int nOrientation = pDoc->get_orientation();
	CPaperInfo* pPaperInfo = GET_PMWAPP()->GetPaperInfo(nOrientation);
//	PPNT Dims = pDoc->get_dimensions();
	PPNT Dims = pPaperInfo->GetProjectDims();
	if (nOrientation == LANDSCAPE)
	{
		PCOORD t = Dims.x;
		Dims.x = Dims.y;
		Dims.y = t;
	}

	// Add the text frame.
	FRAMEOBJ_CREATE_STRUCT fcs;
	memset(&fcs, 0, sizeof(fcs));
	CFrameObject* pFrame;

	fcs.bound.x0 = 0;
	fcs.bound.x1 = Dims.x;
	fcs.bound.y0 = 0;
	fcs.bound.y1 = Dims.y;

	fcs.alignment = ALIGN_center;
	fcs.vert_alignment = ALIGN_middle;
	fcs.flags = 0;
	fcs.pStyle = NULL;

	CTextStyle Style(pDatabase);
	Style.SetDefault();

	int nFace = typeface_server.find_face("Geneva", FALSE);
	if (nFace != -1)
	{
		PMGFontServer* pFontServer = (PMGFontServer*)GetDocument()->get_font_server();

		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			Style.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}

	Style.Size(MakeFixed(20));
	Style.BaseSize(MakeFixed(20));
	Style.UpdateFontMetrics();

	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		pDoc->append_object(pFrame);

		CTxp Txp(pDatabase);
		CString csString;

		// Set its panel.
		pFrame->set_panel(pDoc->get_current_panel());

		TRY
		{
			Txp.Init(pFrame, 0);
			Txp.SetHorizontalAlignment(fcs.alignment);

			// Put in the first line of text.
			Txp.Style(Style);
			csString.LoadString(IDS_DBLPRN_InsertStack);
			Txp.InsertString(csString);

			// Put in the second line of text.
			Style.Bold(TRUE);
			Style.UpdateFontMetrics();
			Txp.Style(Style);
			csString.LoadString((wDoubleSidedType & DBLPRN_STACKMASK) == DBLPRN_StackUp
										? IDS_DBLPRN_FaceUp
										: IDS_DBLPRN_FaceDown);
			Txp.InsertString(csString);

			// Put in the third line of text.
			Style.Bold(FALSE);
			Style.UpdateFontMetrics();
			Txp.Style(Style);
			csString.LoadString(IDS_DBLPRN_WithArrowFacing);
			Txp.InsertString(csString);

			pFrame->calc();
			fResult = TRUE;
		}
		END_TRY
	}
	return fResult;
}

#ifndef WIN32
#pragma optimize("",off)
#endif
BOOL CPmwView::CreateKeyArrow(WORD wDoubleSidedType)
{
	BOOL fResult = FALSE;
	CPmwDoc* pDoc = GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();
//	PPNT Dims = pDoc->get_dimensions();
	int nOrientation = pDoc->get_orientation();
	CPaperInfo* pPaperInfo = GET_PMWAPP()->GetPaperInfo(nOrientation);
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(nOrientation);

	PPNT Dims = pPaperInfo->GetProjectDims();
	if (nOrientation == LANDSCAPE)
	{
		PCOORD t = Dims.x;
		Dims.x = Dims.y;
		Dims.y = t;
	}

	const int ARROW_WIDTH = PAGE_RESOLUTION;		// 1 inch
	const int ARROW_HEIGHT = PAGE_RESOLUTION;		// 1 inch

	// Note: this enum must be such that easy reversing is possible.
	// The current scheme allows reversing by XORing with 1.
	enum
	{
		ARROW_Up		= 0,
		ARROW_Down	= 1,
		ARROW_Left	= 2,
		ARROW_Right	= 3
	};

	// There are three types.
	int nType = (pDoc->get_orientation() == PORTRAIT)
							? ARROW_Up
							: pPrinterInfo->Orientation() == 90
								? ARROW_Left
								: ARROW_Right;

	if ((wDoubleSidedType & DBLPRN_REVERSEDMASK) == DBLPRN_Reversed)
	{
		nType ^= 1;
	}
	PCOORD XOffset;
	PCOORD YOffset;
	ANGLE dRotation;

	switch (nType)
	{
		case ARROW_Up:
		{
			XOffset = (Dims.x-ARROW_WIDTH)/2;
			YOffset = PAGE_RESOLUTION/2;			// 1/2 inch
			dRotation = 0;
			break;
		}
		case ARROW_Down:
		{
			XOffset = (Dims.x-ARROW_WIDTH)/2;
			YOffset = Dims.y - (ARROW_HEIGHT+PAGE_RESOLUTION/2);	// 1/2 inch
			dRotation = DEG2RAD(180);
			break;
		}
		case ARROW_Left:
		{
			XOffset = PAGE_RESOLUTION/2;			// 1/2 inch
			YOffset = (Dims.y-ARROW_HEIGHT)/2;
			dRotation = DEG2RAD(90);
			break;
		}
		case ARROW_Right:
		{
			XOffset = Dims.x - (ARROW_WIDTH+PAGE_RESOLUTION/2);	// 1/2 inch
			YOffset = (Dims.y-ARROW_HEIGHT)/2;
			dRotation = DEG2RAD(270);
			break;
		}
		default:
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	CKeyArrowObject* pObject = new CKeyArrowObject(-1, pDatabase);
	PBOX Bound;

	Bound.x0 = 0+XOffset;
	Bound.y0 = 0+YOffset;
	Bound.x1 = ARROW_WIDTH + XOffset;
	Bound.y1 = ARROW_HEIGHT + YOffset;

	pObject->set_unrotated_bound(Bound);
	pObject->set_rotation(dRotation);
	pObject->calc();

	pObject->set_panel(pDoc->get_current_panel());

	pDoc->append_object(pObject);

	return TRUE;
}
#ifndef WIN32
#pragma optimize("",on)
#endif

int CPmwView::PrintKeySheetBack(PrintContext& pc)
{
	CPmwDoc* pDoc = GetDocument();

	// Only print the key sheet if requested.
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(pDoc->get_orientation());
	if (!pPrinterInfo->GetPrintInstructions())
	{
		return 0;
	}

	return PrintBlankPage(pc);
}

int CPmwView::PrintBlankPage(PrintContext& pc)
{
	int nError = -1;
	CPmwDoc* pDoc = GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();

	// Create a page to use for our output.
	PageRecord* pPage = pDatabase->create_page();

	if (pPage != NULL)
	{
		// Make it the current one in the document.
		DB_RECORD_NUMBER lCurrentPage = pDoc->CurrentPageRecord();
		pDoc->LoadPage(pPage->Id(), TRUE, FALSE);

		// Add the objects to it.
		PATTERN_CREATE_STRUCT pcs;
		pcs.bound.x0 = 0;
		pcs.bound.y0 = 0;
		pcs.bound.x1 = 1;
		pcs.bound.y1 = 1;
		pcs.pattern = 0;	// solid
		pcs.color = COLOR_WHITE;
		pcs.bcolor = COLOR_BLACK;

		PMGPageObject* pObject = pDoc->create_pattern_object(&pcs);
		if (pObject != NULL)
		{
			pDoc->append_object(pObject);
		}

		// Now print the page!
		nError = PrintCurrentPage(pc, -1, !(pc.m_wDoubleSidedType & DBLPRN_PrintOnBlank));

		// Re-load the previous page.
		pDoc->LoadPage(lCurrentPage, TRUE, FALSE);

		// Delete the key page.
		pPage->zap();
	}
	return nError;
}

int CPmwView::PrintDocumentBack(PrintContext& pc)
{
	int nError = 0;

	if ((pc.m_wDoubleSidedType & DBLPRN_BACKDIRMASK) == DBLPRN_BackForward)
	{
		// Print in the forward direction.
		nError = PrintDocumentForward(pc, 1);
	}
	else
	{
		// Print in the reverse direction.
		nError = PrintDocumentBackward(pc, 1);
	}

	return nError;
}

int CPmwView::PrintDocumentForward(PrintContext& pc, int nSide)
{
	CPmwDoc* pDoc = GetDocument();
	int nError = 0;

	int nFromPage = pc.m_pInfo->GetFromPage()-1;
	int nToPage = pc.m_pInfo->GetToPage()-1;

	for (int nPage = nSide + nFromPage; nPage <= nToPage; nPage += 2)
	{
		DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage);
		if (lPage == 0)
		{
			nError = -1;
			break;
		}
		pDoc->LoadPage(lPage, TRUE, FALSE);
		// Now print the page!
		if ((nError = PrintCurrentPage(pc, nPage+1)) != 0)
		{
			break;
		}
	}
	return nError;
}

int CPmwView::PrintDocumentBackward(PrintContext& pc, int nSide)
{
	CPmwDoc* pDoc = GetDocument();
	int nError = 0;

	// Get the from and to pages.
	int nFromPage = pc.m_pInfo->GetFromPage()-1;
	int nToPage = pc.m_pInfo->GetToPage()-1;

	// Normalize the page numbers.
	int nPageOrigin = nFromPage;
	nToPage -= nFromPage;
	nFromPage = 0;

	// Compute the page to start from.
	if (nSide == 0)
	{
		nToPage &= ~1;
	}
	else
	{
		if (!(nToPage & 1))
		{
			// Back sides of odd number of pages. Print a blank one.
			if ((nError = PrintBlankPage(pc)) != 0)
			{
				return nError;
			}
			nToPage--;
		}
	}

	// Now, print the regular pages.
	for (int nPage = nToPage; nPage >= nFromPage; nPage -= 2)
	{
		DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage + nPageOrigin);
		if (lPage == 0)
		{
			nError = -1;
			break;
		}
		pDoc->LoadPage(lPage, TRUE, FALSE);
		// Now print the page!
		if ((nError = PrintCurrentPage(pc, nPage+nPageOrigin+1)) != 0)
		{
			break;
		}
	}
	return nError;
}

/*
// Initialize the print context.
*/

#pragma optimize("",off)
void CPmwView::SetupToPrint(PrintContext& pc)
{
	CPrintInfo* pInfo = pc.m_pInfo;
	CPmwPrinting* pDlgStatus = (CPmwPrinting*)pc.m_dlgstatus;

	pc.m_name_shown = FALSE;

/*
// Set a pointer to our print dialog.
*/

	pc.m_pPD = (CPmwPrint*)pInfo->m_pPD;

/*
// Compute the tile info so we know how many tiles we need to print
// and how large they are.
*/

	pc.m_print_scale = pc.m_pPD->m_scale/100.0;
	pc.m_tile_info.compute(GetDocument(), pc.m_print_scale);

/* Set the printing DC. */

	ASSERT(pInfo->m_pPD->m_pd.hDC != NULL);
			// must be set (did you remember to call DoPreparePrinting?)

	pc.m_dcPrint.Attach(pInfo->m_pPD->m_pd.hDC);	// attach printer dc

/*
// Handle the copies command.
*/

	HGLOBAL hDevMode = pc.m_pPD->m_pd.hDevMode;
	LPDEVMODE lpDevMode;

	pc.m_copies = m_fNoPrintDialog ? 1 : pc.m_pPD->m_pd.nCopies;

	if ((!pc.m_fDoubleSided
			|| pc.m_pInfo->GetFromPage() == pc.m_pInfo->GetToPage())
		 && SupportsEscape(&pc.m_dcPrint, SETCOPYCOUNT)
		 && hDevMode != NULL
		 && (lpDevMode = (LPDEVMODE)::GlobalLock(hDevMode)) != NULL)
	{
		if (lpDevMode->dmFields & DM_COPIES)
		{
		/* Let the driver do the copies. */
			int nPrinterCopies = GET_PMWAPP()->GetPrinterInfo(GetDocument()->get_orientation())->Copies();
			if (nPrinterCopies > 1 || nPrinterCopies == -1)
			{
				lpDevMode->dmCopies = pc.m_copies;
				pc.m_copies = 1;
				pc.m_dcPrint.ResetDC(lpDevMode);		// Put it in the DC
			}
		}
			::GlobalUnlock(hDevMode);
	}

	if(GET_PMWAPP()->m_bAgRemotePrint)
	{
		lpDevMode = (LPDEVMODE)::GlobalLock(hDevMode);
		//get the ini file path
		CString csIniPath = GetDocument()->GetPathManager()->ExpandPath("AgRemote.ini");
		CIniFile iniFile(csIniPath);
		UINT nPaperSize;
		if(m_bPrintingHFasQF)
		{
			nPaperSize = iniFile.GetInteger("Paper Size", "tabloid", DMPAPER_TABLOID);
		}	
		else
		{
			nPaperSize = iniFile.GetInteger("Paper Size", "letter", DMPAPER_LETTER);
		}
		lpDevMode->dmPaperSize = nPaperSize;			
		pc.m_dcPrint.ResetDC(lpDevMode);		// Put it in the DC
		
		::GlobalUnlock(hDevMode);
		HGLOBAL hDevNames = pc.m_pPD->m_pd.hDevNames;
		

		CPmwApp *pApp = (CPmwApp *)AfxGetApp();

		pApp->set_dev_handles(hDevMode, hDevNames);

		pApp->update_paper_info(pInfo->m_pPD->m_pd.hDC);

	}

/* Do we do that banding thing? */

#ifdef WIN32
	pc.m_fBanding = FALSE;
#else
	pc.m_fBanding = pc.m_dcPrint.GetDeviceCaps(RASTERCAPS) & RC_BANDING;
#endif

	if (pc.m_dcPrint.Escape(GETSCALINGFACTOR, NULL, NULL, &pc.m_scaling) <= 0)
	{
		pc.m_scaling.x =
			pc.m_scaling.y = 0;
	}
//	od("ScalingFactor: %d, %d\r\n", pc.m_scaling);

/* Begin printing now. */

	pc.m_dcPrint.m_bPrinting = TRUE;

/*
// Now setup the printing dialog.
*/

	CString docTitle = GetDocument()->GetTitle();

	pDlgStatus->SetDlgItemText(AFX_IDC_PRINT_DOCNAME, docTitle);
	pDlgStatus->SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME,
		pInfo->m_pPD->GetDeviceName());

	CString strPort;
	AfxFormatString1(strPort, AFX_IDS_PRINTONPORT,
		pInfo->m_pPD->GetPortName());
	pDlgStatus->SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strPort);

/* Show the printing dialog. */

	pDlgStatus->ShowWindow(SW_SHOW);
	pDlgStatus->UpdateWindow();			// Make it show completely

	OnBeginPrinting(&pc, pInfo);

/* Create the printing dialog. */

#ifndef WIN32n
	if (pc.m_hWndParent != NULL)
	{
		::EnableWindow(pc.m_hWndParent, FALSE);   // Disable parent window
	}
//	AfxGetApp()->m_pMainWnd->EnableWindow(FALSE);
#endif

	pc.m_dcPrint.SetAbortProc(PrintAbortProc);
}
#pragma optimize("",on)

int CPmwView::StartPrintDoc(PrintContext& pc)
{
	CString docTitle = GetDocument()->GetTitle();

	if (docTitle.GetLength() > 31)
		docTitle.ReleaseBuffer(31);

/* Start the document. */

	DOCINFO docInfo;
	memset(&docInfo, 0, sizeof(DOCINFO));
	docInfo.cbSize = sizeof(DOCINFO);
	docInfo.lpszDocName = docTitle;
//	docInfo.lpszOutput = NULL;				/* Zero'd out! */

	int nError = pc.m_dcPrint.StartDoc(&docInfo);
	if (nError != SP_ERROR)
	{
		nError = 0;
		pc.m_fStarted = TRUE;
	}

	return nError;
}

void CPmwView::EndPrintDoc(PrintContext& pc, int nError)
{
	if (nError == 0)
	{
		pc.m_dcPrint.EndDoc();
	}
}

void CPmwView::FinishPrinting(PrintContext& pc, int nError)
{
	if (pc.m_hWndParent != NULL)
	{
		::EnableWindow(pc.m_hWndParent, TRUE);   // Re-enable parent window
	}

	if (pc.m_fStarted)
	{
		pc.m_fStarted = FALSE;
		OnEndPrinting(&pc.m_dcPrint, pc.m_pInfo);       // Clean up after printing
	}
	pc.m_dlgstatus->DestroyWindow();
#ifdef WIN32
	pc.m_dcPrint.Detach();
#endif
}

/*
// Generic document print routine.
// Main print entry.
*/

int CPmwView::PrintProject(CPrintInfo* pInfo, WORD wSoundID, BOOL AllowDoubleSided /* TRUE */)
{
	CPmwDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);

   if (IsGoodPrinter () == FALSE)
      return -1;

   CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(GetDocument()->get_orientation());

	// Initialize double-sided information in the print context.
	// We need a print context and a print status dialog.
	m_pc.m_wDoubleSidedType = pPrinterInfo->GetDoubleSidedType();
	if (AllowDoubleSided && !m_fNoPrintDialog)
	{
      m_pc.m_fDoubleSided = pPrinterInfo->GetProfileInt(ENTRY_DoubleSided, TRUE);
	}
   else
	{
      m_pc.m_fDoubleSided = FALSE;
	}

	// See if we need to init for double-sided printing.
	if (m_pc.m_fDoubleSided
			&& pInfo->GetFromPage() != pInfo->GetToPage()
			&& !(m_pc.m_wDoubleSidedType & DBLPRN_Valid))
	{
		// We need to set the double-sided type.
		if (CDoubleSidedPrintWizard::Run(NULL))
		{
			m_pc.m_wDoubleSidedType = pPrinterInfo->GetDoubleSidedType();
			ASSERT(m_pc.m_wDoubleSidedType & DBLPRN_Valid);
		}
		else
		{
			// User does not want to continue.
			return -1;
		}
	}

	// Initialize reverse print information in the print context.
	m_pc.m_fReversed = ((CPmwPrint*)pInfo->m_pPD)->m_fReversed;

	// If we are printing reversed, flip the project.
	if (m_pc.m_fReversed)
	{
	   BOOL fAddRefreshes = pDoc->AddRefreshes();
	   pDoc->AddRefreshes(FALSE);
		pDoc->FlipProject(OBJECT_FLAG_xflipped);
	   pDoc->AddRefreshes(fAddRefreshes);
	}

/*
// Get the owner for the status dialog.
*/

#ifdef WIN32
	m_pc.m_hWndParent = CWnd::GetSafeOwner(NULL)->GetSafeHwnd();
	if (m_pc.m_hWndParent != NULL)
	{
		::EnableWindow(m_pc.m_hWndParent, FALSE);   // Disable parent window
	}
#else
	m_pc.m_hWndParent = _AfxGetSafeOwner(NULL);
#endif

	// Set a pointer to the status dialog and our print info.
	CPmwPrinting dlgPrintStatus(wSoundID, this);

	m_pc.m_dlgstatus = &dlgPrintStatus;
	m_pc.m_pInfo = pInfo;

	SetupToPrint(m_pc);

	// Start the print job.
	int nError;
	if ((nError = StartPrintDoc(m_pc)) == SP_ERROR)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
	}
	else
	{
		// Do the actual bulk of printing.
		nError = GrindOutPrintJob(m_pc);
		EndPrintDoc(m_pc, nError);
	}

	// We are now done.
	FinishPrinting(m_pc, nError);

	// If we flipped the project for reverse printing, flip it back.
	if (m_pc.m_fReversed)
	{
	   BOOL fAddRefreshes = pDoc->AddRefreshes();
	   pDoc->AddRefreshes(FALSE);
		pDoc->FlipProject(OBJECT_FLAG_xflipped);
	   pDoc->AddRefreshes(fAddRefreshes);
	}
	
	return nError;
}

int CPmwView::GrindOutPrintJob(PrintContext& pc)
{
/*
// Print our document(s).
*/

	CPmwDoc* pDoc = GetDocument();

	int32 aCurrentDocument = 1, aNumDocuments = 1;

	CStdMacroServer* pMacroServer = pDoc->GetMacroServer();
	NameListRecord* pNameList = pMacroServer->GetNameList();
	BOOL fMergePrint = FALSE;

	if (pNameList->Names() != 0)
	{
		aNumDocuments = pNameList->Names();
		fMergePrint = TRUE;
	}

	int nError = TRUE;

	BOOL fOldShow = pMacroServer->ShowMacroValues();
	pMacroServer->ShowMacroValues(TRUE);
	pDoc->ReflowMacros();

	if (fMergePrint)
	{
		// Construct the full name of the address book we're using.
		CString csFullName;
		TRY
		{
			csFullName = pDoc->GetPathManager()->LocatePath(pNameList->AddressBookName());

			if (!csFullName.IsEmpty())
			{
				// get sender values for address book.	
				pMacroServer->BindSender();

				TRACE("Open database %s for data!\r\n", (LPCSTR)csFullName);

				// Open up the address book, do not create it.
				StorageFile* pDatabaseFile = NULL;
				CFlatFileDatabase* pDatabase = NULL;
				CFlatFileDatabaseCursor* pCursor = NULL;

				ERRORCODE Error = CAddressBook::OpenAddressBookDatabase(csFullName, pDatabaseFile, pDatabase, FALSE);
				if (Error == ERRORCODE_None)
				{
					TRY
					{
						// Create a cursor on the database.
						pCursor = new CFlatFileDatabaseCursor;
						pCursor->Attach(pDatabase);

						// Print a document for each receiver.
						while (aCurrentDocument <= aNumDocuments)
						{
						/*
						// Get the next name to print.
						*/

							pMacroServer->BindToRecord(pCursor, pNameList->Name((int)aCurrentDocument-1));
							pDoc->ReflowMacros();

							nError = OnPrintDocument(pc, aCurrentDocument, aNumDocuments);

							if (nError != 0)
							{
							/* Aack! We got an error. Leave now. */
								break;
							}
							aCurrentDocument++;
						}

						if (aCurrentDocument > 1)
						{
						/* Go back to the first name. */
							pMacroServer->BindToRecord(pCursor, pNameList->Name(0));
							pDoc->ReflowMacros();
						}
					}
					END_TRY
				}

				delete pCursor;
				pCursor = NULL;
				CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);
			}
		}
		END_TRY
	}
	else
	{
		nError = OnPrintDocument(pc, aCurrentDocument, aNumDocuments);
	}

	/* Revert back. */

	if (!fOldShow)
	{
		pMacroServer->ShowMacroValues(fOldShow);
		pDoc->ReflowMacros();
	}

	return nError;
}

/*
// Command handler for OnFilePrint.
// Most of this code is from the MFC (at least, initially).
// A few bugs in the MFC code have been straightened out.
*/

void CPmwView::OnFilePrint()
{
#ifdef WIN32XXX
	CView::OnFilePrint();
#else
	CPrintInfo printInfo;           // Get Default print info
	ASSERT(printInfo.m_pPD != NULL);    // must be set

	if (OnPreparePrinting(&printInfo))
	{
		// print if OK
		PrintProject(&printInfo);
	}
#endif
}

void CPmwView::PrintHFtoQF(void)
{
	//get the HF doc
	CHalfCardDoc* pCurDoc  = (CHalfCardDoc*)GetDocument();
	//set the doc info in the app
	NEWINFO& pInfo = GET_PMWAPP()->last_new_info;
	pInfo.pTemplate = GET_PMWAPP()->m_pCardTemplate;
	pInfo.type = PROJECT_TYPE_Card;
	pInfo.subtype = pCurDoc->FoldType();   
	pInfo.orientation = pInfo.subtype == CARD_SideFold ? PORTRAIT : LANDSCAPE;
	
	//create the QF card
	CCardDoc* pDoc  = (CCardDoc*)pInfo.pTemplate->OpenDocumentFile(NULL);
	PMGDatabase* pDatabase = pDoc->get_database();
	PMGPageObjectPtr object;

	//loop through the panels
	for(int i = 0; i < 3; i++)
	{
		//set the panel for both docs
		pDoc->set_panel(i);
		pCurDoc->set_panel(i);
		//get the object list for this panel
		ObjectList* pObjects = pCurDoc->object_list();
		//get the panels world bounding boxes
		PBOX rcCurPanelWorld;
		PBOX rcNewPanelWorld;
		pCurDoc->get_panel_world(&rcCurPanelWorld);
		pDoc->get_panel_world(&rcNewPanelWorld);
		//set up the scaling factors
		double dXScale  = (double)(rcNewPanelWorld.x1 - rcNewPanelWorld.x0) / (double)(rcCurPanelWorld.x1 - rcCurPanelWorld.x0);
		double dYScale  = (double)(rcNewPanelWorld.y1 - rcNewPanelWorld.y0) / (double)(rcCurPanelWorld.y1 - rcCurPanelWorld.y0);
		//loop through the object list
		for (object = (PMGPageObjectPtr)pObjects->first_object();
					object != NULL;
					object = (PMGPageObjectPtr)object->next_object())
		{
			//create a copy of the object
			PMGPageObjectPtr newObject = (PMGPageObjectPtr)object->duplicate(pDatabase);
			newObject->set_panel(i);
			//set the bounds for the new object and scale accordingly
			PBOX pbBounds = newObject->get_bound();
			int nWidth = (int)((pbBounds.x1 - pbBounds.x0)  * dXScale);
			int nHeight = (int)((pbBounds.y1 - pbBounds.y0)  * dYScale);
			int nXOffset = (int)((pbBounds.x0 - rcCurPanelWorld.x0) * dXScale);
			int nYOffset = (int)((pbBounds.y0 - rcCurPanelWorld.y0) * dYScale);
			pbBounds.x0 = rcNewPanelWorld.x0 + nXOffset;
			pbBounds.x1 = pbBounds.x0 + nWidth;
			pbBounds.y0 = rcNewPanelWorld.y0 + nYOffset;
			pbBounds.y1 = pbBounds.y0 + nHeight;
			((RectPageObject*)newObject)->set_bound(pbBounds);
			newObject->calc();
			//add the object to the document
			pDoc->append_object(newObject);
		}
	}
	//print the quarter fold document
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT);
	//close the new document
	pDoc->OnCloseDocument();

}

void CPmwView::InsertAgBarCode(CCardDoc* pDoc)
{
	CPoint ptBarCodeLoc(0,0);
	int nBarCodeSize;
	int nAgcSize;
	CString csBCKeyFont;
	CString csBCKeySize;
	CString csAGCKeyFont;
	CString csAGCKeySize;
	CString csBarCodeFont;
	CString csAgcFont;
	CString csAgcText;
	CString csBarCodeText;

	//get the ini keys depending on project type and subtype
	ProjectInfo info = pDoc->GetProjectInfo();
	if(m_bPrintingHFasQF)
	{
		if(info.project_sub_type == CARD_SideFold)
		{
			csBCKeyFont = "bc_tabloid_v_font";
			csBCKeySize = "bc_tabloid_v_point_size";
			csAGCKeyFont = "agc_tabloid_v_font";
			csAGCKeySize = "agc_tabloid_v_point_size";
		}
		else
		{
			csBCKeyFont = "bc_tabloid_h_font";
			csBCKeySize = "bc_tabloid_h_point_size";
			csAGCKeyFont = "agc_tabloid_h_font";
			csAGCKeySize = "agc_tabloid_h_point_size";
		}
	}
	else
	{
		if(info.project_sub_type == CARD_SideFold)
		{
			csBCKeyFont = "bc_letter_v_font";
			csBCKeySize = "bc_letter_v_point_size";
			csAGCKeyFont = "agc_letter_v_font";
			csAGCKeySize = "agc_letter_v_point_size";
		}
		else
		{
			csBCKeyFont = "bc_letter_h_font";
			csBCKeySize = "bc_letter_h_point_size";
			csAGCKeyFont = "agc_letter_h_font";
			csAGCKeySize = "agc_letter_h_point_size";
		}
	}

	//get the ini file path
	CString csIniPath = pDoc->GetPathManager()->ExpandPath("AgRemote.ini");
	CIniFile iniFile(csIniPath);
	//get the values
	csBarCodeFont = iniFile.GetString("Bar Code", csBCKeyFont, "Code39EightText");
	nBarCodeSize = iniFile.GetInteger("Bar Code", csBCKeySize, 14);

	csAgcFont = iniFile.GetString("AGC", csAGCKeyFont, "CAC Logo Alternate");
	nAgcSize = iniFile.GetInteger("AGC", csAGCKeySize, 8);
	
	csAgcText = iniFile.GetString("AGC", "agc_text", "©AGC Inc.");
	csBarCodeText = iniFile.GetString("Bar Code", "bc_text", "");

	PMGDatabase* pDatabase = pDoc->get_database();
	CTxp Txp(pDatabase);
	PMGFontServer* pFontServer = (PMGFontServer*)pDoc->get_font_server();

	CTextStyle BarCodeStyle(pDatabase);
	CTextStyle AgcStyle(pDatabase);
	BarCodeStyle.SetDefault();
	AgcStyle.SetDefault();

	int nFace = typeface_server.find_face(csBarCodeFont, FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			BarCodeStyle.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}
	nFace = typeface_server.find_face(csAgcFont, FALSE);
	if (nFace != -1)
	{
		DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
		if (lFace != 0)
		{
			AgcStyle.Font(lFace);
			pDatabase->free_font_record(lFace, TRUE);
		}
	}
	//set the spacing
	BarCodeStyle.Size(MakeFixed(nBarCodeSize));
	BarCodeStyle.BaseSize(MakeFixed(nBarCodeSize));
	BarCodeStyle.UpdateFontMetrics();
	BarCodeStyle.Alignment(ALIGN_center);
	BarCodeStyle.LeadingType(1);
	BarCodeStyle.Leading(MakeFixed(nBarCodeSize));

	AgcStyle.Size(MakeFixed(nAgcSize));
	AgcStyle.BaseSize(MakeFixed(nAgcSize));
	AgcStyle.UpdateFontMetrics();
	AgcStyle.Alignment(ALIGN_center);
	AgcStyle.LeadingType(1);
	AgcStyle.Leading(MakeFixed(nAgcSize));


	pDoc->set_panel(2);
	PBOX pbPanelWorld;
	pDoc->get_panel_world(&pbPanelWorld);
#if 0
	//Get the text diminsions.
	CDC* pDC = GetDC();
	int nOldMapMode = pDC->SetMapMode(MM_LOENGLISH);
	CFont barCodeFont;
	barCodeFont.CreatePointFont(nBarCodeSize*10, csBarCodeFont, pDC);
	CFont* pOldFont = pDC->SelectObject(&barCodeFont);
	CSize szTextBarCode = pDC->GetTextExtent(csBarCodeText);

	CFont agcFont;
	agcFont.CreatePointFont(nAgcSize*10, csAgcFont, pDC);
	pDC->SelectObject(&agcFont);
	CSize szTextAgc = pDC->GetTextExtent( csAgcText );

	pDC->SelectObject(pOldFont);
	pDC->SetMapMode(nOldMapMode);
	ReleaseDC(pDC);
	//create text rect in pmw coords with padding(multiply by 20 instead of 18)
	CRect rcText(0,0, max(szTextBarCode.cx, szTextAgc.cx) * 20, (szTextBarCode.cy + szTextAgc.cy) * 19);
	pDoc->set_panel(2);

	PBOX pbBounds = {	pbPanelWorld.x1 - rcText.Width(), 
						pbPanelWorld.y1 - rcText.Height(), 
						pbPanelWorld.x1, 
						pbPanelWorld.y1 };

#else
	int nWidth = 2450;
	int nHeight = 800;
	int nX0 = (pbPanelWorld.x1 - pbPanelWorld.x0) / 2 - (nWidth / 2) + pbPanelWorld.x0; 
	PBOX pbBounds = {	nX0, 
						pbPanelWorld.y0, 
						nX0 + nWidth, 
						pbPanelWorld.y0 + nHeight };

#endif

	FRAMEOBJ_CREATE_STRUCT fcs;
	memset(&fcs, 0, sizeof(fcs));
	fcs.bound = pbBounds;
	fcs.vert_alignment = ALIGN_top;
	fcs.alignment = ALIGN_center;
	fcs.m_Fill.SetColor(FillFormatV1::FillSolid, COLOR_FROM_RGBCOLOR(RGB(255,255,255)));
	
	CFrameObject* pFrame;

	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		pFrame->set_panel(2);
		pDoc->append_object(pFrame);
		Txp.Init(pFrame, 0);
	}
	else
	{
		ASSERT(0);
		return;
	}
	//insert the bar code
	BarCodeStyle.SpaceBefore(MakeFixed(.1));
	Txp.Style(BarCodeStyle);
	CTextRecord* pRecord = Txp.LockText();
	pRecord->ChangeParagraphStyle(0, 0, &BarCodeStyle);
	pRecord->release();
	Txp.InsertString(csBarCodeText);
	Txp.InsertString("\n");
	//insert the Agc text
	Txp.Style(AgcStyle);
	pRecord = Txp.LockText();
	pRecord->ChangeParagraphStyle(1, 1, &AgcStyle);
	pRecord->release();
	Txp.InsertString(csAgcText);

	pFrame->calc();
}

void CPmwView::PrintNoDialog(void)
{

	//if this product is being used by american greetings to do remote fulfillment
	//we need to convert Half fold cards to quarter fold and then print as a QF.  The 
	//reason for this is that they use a special large paper for HF and then quarter fold
	//it instead of printing on both sides.
	BOOL bAgRemotePrint = GET_PMWAPP()->m_bAgRemotePrint;
	if(bAgRemotePrint)
	{
		if(GetDocument()->get_project_type() == PROJECT_TYPE_HalfCard  )
		{
			m_bPrintingHFasQF = TRUE;
			PrintHFtoQF();
			return;
		}	
		else
		{
			InsertAgBarCode( (CCardDoc*)GetDocument() );
		}
	}

	m_fNoPrintDialog = TRUE;

	TRY
	{
		OnFilePrint();
	}
	END_TRY

	m_bPrintingHFasQF = FALSE;
	m_fNoPrintDialog = FALSE;
	
	//this code is used by AG for remote fulfillment only
	//this event is a shared handle with the remote print dll.  We use this
	//event to signal that the printing is complete
	if(bAgRemotePrint)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		HANDLE hInitDone = CreateEvent(&sa, FALSE, FALSE, "AG_DDE_Event");
		SetEvent(hInitDone);
		CloseHandle(hInitDone);	
	}
}

/////////////////////////////////////////////////////////////////////////////
// PrintContext

void PrintContext::ToStartOfBand(void)
{
	m_nStartingLabel = m_nBandLabel;
	m_nCurrentName = m_nBandName;
	m_nCurrentCopy = m_nBandCopy;
}

void PrintContext::SaveStartOfBand(void)
{
	m_nBandLabel = m_nStartingLabel;
	m_nBandName = m_nCurrentName;
	m_nBandCopy = m_nCurrentCopy;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwPrint dialog

CPmwPrint::CPmwPrint(CPmwView *pView, CPmwDoc* pDoc, UINT id)
				: CPrintDialog(FALSE)
{
	//{{AFX_DATA_INIT(CPmwPrint)
	m_smoothing = FALSE;
	m_coloring_book = -1;
	//}}AFX_DATA_INIT

	m_fDoubleSided = FALSE;

	m_scale = 100.0;
	m_pView = pView;
	if (pDoc == NULL)
	{
		ASSERT(pView != NULL);
		m_pDoc = pView->GetDocument();
	}
	else
	{
		m_pDoc = pDoc;
	}

	// Default to reversed printout for t-shirts.
	m_fReversed = (m_pDoc->get_project_type() == PROJECT_TYPE_TShirt);

	m_tiles = NULL;

	m_pd.Flags &= ~PD_USEDEVMODECOPIES;			// Always want copies.

	m_pd.Flags |= PD_ENABLEPRINTTEMPLATE;
	m_pd.lpPrintTemplateName = MAKEINTRESOURCE(id);
#ifndef WIN32
	m_pd.Flags |= PD_ENABLESETUPTEMPLATE;
	m_pd.lpSetupTemplateName = MAKEINTRESOURCE(IDD_PRINT_SETUP);
#endif
	
  m_pd.hInstance = AfxGetResourceHandle();


/*
// Initialize some variables in the dialog from the printer rc.
// (Be sure the printer rc is initialized itself first.)
*/

	if (!printer_rc.is_printing)
	{
	/*
	// Not set yet! Initialize now.
	// If the variable becomes tristate in the dialog (as I plan for
	// someday), it should default to TRISTATE_SELECTED.
	*/
		printer_rc.is_printing = TRUE;
		printer_rc.smoothing = TRISTATE_ALL;
		printer_rc.coloring_book = TRISTATE_SELECTED;
	}

	m_smoothing = (printer_rc.smoothing == TRISTATE_ALL);
	m_coloring_book = printer_rc.coloring_book;

	m_pd.nMinPage = 1;
   m_pd.nMaxPage = (USHORT)(pView == NULL ? pDoc : pView->GetDocument())->NumberOfPages();

	m_pd.nFromPage = m_pd.nMinPage;
	m_pd.nToPage = m_pd.nMaxPage;
}

CPmwPrint::~CPmwPrint()
{
	delete [] m_tiles;
}

void CPmwPrint::set_all_tiles()
{
	delete [] m_tiles;
	update_tiles();
}

void CPmwPrint::DoDataExchange(CDataExchange* pDX)
{
	CPrintDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPmwPrint)
	DDX_Check(pDX, IDC_PRINT_SMOOTHING, m_smoothing);
	DDX_CBIndex(pDX, IDC_PRINT_COLORING_BOOK, m_coloring_book);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		// Record the state of the "print reversed" checkbox.
		CButton* pReversed = (CButton*)GetDlgItem(IDC_PRINT_REVERSED);
		if (pReversed != NULL)
		{
			m_fReversed = (pReversed->GetCheck() != 0);
		}

		if (m_pDoc->get_project_type() == PROJECT_TYPE_Envelope)
		{
			CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(m_pDoc->get_orientation());
			if (pPrinterInfo != NULL)
			{
				if (pPrinterInfo->IsEnvelopePaperSize())
				{
					CPmwDialog Dialog(IDD_CHANGE_ENVELOPE_PAPER_SIZE, this);
					Dialog.DoModal();
					AfxThrowUserException();
				}
			}
		}
	}
	else
	{
		// Set the state of the "print reversed" checkbox.
		CButton* pReversed = (CButton*)GetDlgItem(IDC_PRINT_REVERSED);
		if (pReversed != NULL)
		{
			pReversed->SetCheck(m_fReversed ? 1 : 0);
		}
	}
}

/*
// Show the current scale value in the static text field.
*/

void CPmwPrint::update_scale()
{
	CString cs;
	
	TRY
	{
      FormatDouble(m_scale, PRINT_SIZE_PERCENT_PRECISION, cs);
      cs += '%';
		SetDlgItemText(IDC_PRINT_PERCENT, cs);
	}
	CATCH_ALL(e)
	{
		cs.Empty();
	}
	END_CATCH_ALL;
}

void CPmwPrint::update_tiles()
{
	CWnd *child;

	if ((child = GetDlgItem(IDC_PRINT_TILES)) == NULL)
	{
		return;
	}

	TileInfo tile_info;

	tile_info.compute(m_pDoc, m_scale/100.0);

	CString string;

	UINT id;
	BOOL all;

	if (tile_info.m_x_tiles == 1 && tile_info.m_y_tiles == 1)
	{
	/* Fits on page! */

		child->EnableWindow(FALSE);

		all = TRUE;
		id = IDS_FITS_ON_PAGE;
	}
	else
	{
	/* Doesn't fit on page. */

		child->EnableWindow(TRUE);

		all = (m_tiles == NULL);
		id = all ? IDS_ALL_TILES_CHOSEN : IDS_SOME_TILES_CHOSEN;
	}

	TRY
	{
		LoadConfigurationString(id, string);
	}
	CATCH_ALL(e)
	{
		return;
	}
	END_CATCH_ALL

	CHAR buffer[64];

	if (all)
	{
		strcpy(buffer, (char const *)string);
	}
	else
	{
		sprintf(buffer, (char const *)string, m_tiles_chosen);
	}

	SetDlgItemText(IDC_PRINT_TILES_CHOSEN, buffer);
}

void CPmwPrint::UpdateFastBitmaps()
{
/* Make sure the fast bitmaps setting matches that of the printer info. */
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(m_pDoc->get_orientation());

	BOOL fFastBitmaps = pPrinterInfo->FastBitmaps();

	CheckDlgButton(IDC_FAST_BITMAP_PRINTING, fFastBitmaps);

	CWnd* pChild = GetDlgItem(IDC_PRINT_SMOOTHING);

	if (fFastBitmaps)
	{
		m_smoothing = FALSE;
	}
	else
	{
		if (pChild != NULL)
		{
			pChild->EnableWindow(TRUE);
		}
		m_smoothing = (printer_rc.smoothing == TRISTATE_ALL);
	}
	CheckDlgButton(IDC_PRINT_SMOOTHING, m_smoothing);
	if (fFastBitmaps && pChild != NULL)
	{
		pChild->EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(CPmwPrint, CPrintDialog)
	//{{AFX_MSG_MAP(CPmwPrint)
	ON_BN_CLICKED(IDC_PRINT_TILES, OnClickedPrintTiles)
	ON_BN_CLICKED(IDC_PRINT_SIZE, OnClickedPrintSize)
	ON_BN_CLICKED(IDC_PRINT_SETUP, OnClickedPrintSetup)
	ON_BN_CLICKED(IDC_PRINT_ADVANCED, OnClickedPrintAdvanced)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PRINT_NAME_LIST, OnPrintNameList)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_FAST_BITMAP_PRINTING, OnFastBitmapPrinting)
	ON_BN_CLICKED(IDC_PRINT_DOUBLE_SIDED, OnPrintDoubleSided)
	ON_BN_CLICKED(IDC_PRINT_INSTRUCTIONS, OnPrintInstructions)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

void CPmwPrint::OnOK()
{
	if (UpdateData(TRUE))
	{
		CPrintDialog::OnOK();
	}
}

int CPmwPrint::DoModal()
{
	int nResult;
	if ((nResult = CPrintDialog::DoModal()) == IDCANCEL)
	{
		int cres = (int)::CommDlgExtendedError();

		od("Cancel result is %x\r\n", cres);
		if (cres != 0)
		{
			nResult = -1;			/* Return something other than IDOK or IDCANCEL. */
		}
	}
	
   return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwPrint message handlers

BOOL CPmwPrint::OnInitDialog()
{
/* Make us be centered. */

	standard_dialog_startup(this, (UINT)(DWORD)m_pd.lpPrintTemplateName);

/* Update the name list. */

	update_name_list();

/* Make sure we're current. */

	update_paper_info();

/* Show the scale value. */

	update_scale();

/* Handle the tiles button. */

	update_tiles();

/* Update the fast bitmaps button. */

	UpdateFastBitmaps();

/* Stuff the strings into the coloring book list. */

	CComboBox *list = (CComboBox *)GetDlgItem(IDC_PRINT_COLORING_BOOK);
	if (list != NULL)
	{
		// JRO
      //list->AddString("None");
		//list->AddString("As Selected");
		//list->AddString("All");
      list->AddString(GET_PMWAPP()->GetResourceStringPointer (IDS_NONE));
		list->AddString(GET_PMWAPP()->GetResourceStringPointer (IDS_AS_SELECTED));
		list->AddString(GET_PMWAPP()->GetResourceStringPointer (IDS_ALL));
	}

	if (m_pDoc->get_project_type() == PROJECT_TYPE_TShirt)
	{
		GET_PMWAPP()->ShowHelpfulHint("PrintTShirt", this);
	}
	else if (m_pDoc->get_project_type() == PROJECT_TYPE_Banner)
	{
		GET_PMWAPP()->ShowHelpfulHint("PrintBanner", this);
	}
	else
	{
		GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_PrintDialog, this);
	}

	return CPrintDialog::OnInitDialog();
}

void CPmwPrint::OnDestroy()
{
	standard_dialog_shutdown(this, (UINT)(DWORD)m_pd.lpPrintTemplateName);

	CPrintDialog::OnDestroy();
}

LRESULT CPmwPrint::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this,
									(UINT)(DWORD)m_pd.lpPrintTemplateName,
									lParam);
}

void CPmwPrint::OnClickedPrintTiles()
{
	CPrintRange rdlg(m_pDoc, m_scale, this);

	rdlg.m_tiles = m_tiles;
	rdlg.m_tiles_chosen = m_tiles_chosen;

	if (rdlg.DoModal() == IDOK)
	{
		m_tiles = rdlg.m_tiles;
		m_tiles_chosen = rdlg.m_tiles_chosen;
		update_tiles();
	}
}

void CPmwPrint::OnClickedPrintSize()
{
	CPrintSize sdlg(m_pDoc, this);

	sdlg.m_scale = m_scale;

	if (sdlg.DoModal() == IDOK)
	{
		m_scale = sdlg.m_scale;
		update_scale();
		set_all_tiles();
	}
}

void CPmwPrint::OnClickedPrintSetup()
{
	HGLOBAL hSaveDevMode = m_pd.hDevMode;
	HGLOBAL hSaveDevNames = m_pd.hDevNames;
	
	CPrintDialog::OnPrintSetup();

	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

/*
// Hopefully, this will correct the MFC bug when you cancel out of the
// print dialog.
*/

	pApp->set_dev_handles(m_pd.hDevMode, m_pd.hDevNames);

/* Make sure our paper info is correct. */

	update_paper_info();

/* Reset the output scale to 100% if a new printer was selected. */

	if ((m_pd.hDevMode != hSaveDevMode)
	 || (m_pd.hDevNames != hSaveDevNames))
	{
		ChangedPrinter();
	}
}

CPrintDialog* CPmwPrint::AttachOnSetup()
{
	ASSERT_VALID(this);

	CPrintSetupDialog* pDlgSetup;

	pDlgSetup = new CPrintSetupDialog(m_pd, m_pParentWnd, AFX_IDD_PRINTSETUP);
	return pDlgSetup;
}

void CPmwPrint::ChangedPrinter(void)
{
	m_scale = 100.0;
	update_scale();
	set_all_tiles();
}

void CPmwPrint::OnClickedPrintAdvanced()
{
	CPrinterInfo* pPortraitInfo = GET_PMWAPP()->GetPrinterInfo(PORTRAIT);
	CPrinterInfo* pLandscapeInfo = GET_PMWAPP()->GetPrinterInfo(LANDSCAPE);

	CAdvancedDlg adlg(pPortraitInfo->GetOutlineGamma(),
							pPortraitInfo->GetBitmapGamma(),
							pPortraitInfo->GetPrintAsBitmap(),
							this);

	if (adlg.DoModal() == IDOK)
	{
	/* Outline gamma value. */
		pPortraitInfo->SetOutlineGamma(adlg.m_outline_gamma);
		pPortraitInfo->SetBitmapGamma(adlg.m_bitmap_gamma);
		pPortraitInfo->SetPrintAsBitmap(adlg.m_fPrintAsBitmap);

		pPortraitInfo->WriteSettings();		// This writes common keys for both.

		pLandscapeInfo->SetOutlineGamma(adlg.m_outline_gamma);
		pLandscapeInfo->SetBitmapGamma(adlg.m_bitmap_gamma);
	}
}

void CPmwPrint::OnFastBitmapPrinting() 
{
	BOOL fFastBitmaps = IsDlgButtonChecked(IDC_FAST_BITMAP_PRINTING);

	CPrinterInfo* pPortraitInfo = GET_PMWAPP()->GetPrinterInfo(PORTRAIT);
	ASSERT(pPortraitInfo != NULL);
	if (pPortraitInfo != NULL)
	{
		pPortraitInfo->FastBitmaps(fFastBitmaps);
		pPortraitInfo->WriteSettings();
	}
	CPrinterInfo* pLandscapeInfo = GET_PMWAPP()->GetPrinterInfo(LANDSCAPE);
	ASSERT(pLandscapeInfo != NULL);
	if (pLandscapeInfo != NULL)
	{
		pLandscapeInfo->FastBitmaps(fFastBitmaps);
		if (pPortraitInfo == NULL)
		{
			pLandscapeInfo->WriteSettings();
		}
	}
	GraphicObject::SetDirectBitmapPrinting(fFastBitmaps);
	UpdateFastBitmaps();
}

void CPmwPrint::update_paper_info()
{
/*
// Create the printer DC.
// This changes m_pd.hDC, so save and restore it.
*/
	HDC dc_save = m_pd.hDC;

	HDC hpDC = CreatePrinterDC();
	m_pd.hDC = dc_save;

/* Now handle the DC we got. */

	if (hpDC != NULL)
	{
		((CPmwApp *)AfxGetApp())->update_paper_info(hpDC);

		DeleteDC(hpDC);
	}
}

void CPmwPrint::OnPrintNameList()
{
	if (GetConfiguration()->SupportsAddressBook())
	{
		if (m_pDoc->EditNameList(TRUE))
		{
			update_names_chosen();
		}
	}
}

void CPmwPrint::update_name_list(void)
{
	CWnd* pChild = GetDlgItem(IDC_PRINT_NAME_LIST);

	if (pChild != NULL)
	{
		pChild->EnableWindow(m_pDoc->HasDatabaseMacro());
	}

/*
// Now update the "Chosen" item.
*/

	update_names_chosen();
}

void CPmwPrint::update_names_chosen(void)
{
	CWnd* pNameButton = GetDlgItem(IDC_PRINT_NAME_LIST);
	BOOL fEnabled = (pNameButton != NULL) && pNameButton->IsWindowEnabled();
	CString csText;

	char buffer[50];

	if (fEnabled)
	{
		buffer[0] = 0;	// Default to empty string - DGP
		NameListRecord* pNameList = m_pDoc->GetMacroServer()->GetNameList();
		int nNames = pNameList->Names();
		// JRO sprintf(buffer, "%d name%s", nNames, nNames == 1 ? "" : "s");
		if(nNames > 0)
	      sprintf(buffer, 
               GET_PMWAPP()->GetResourceStringPointer (IDS_D_NAME_S), 
               nNames, nNames == 1 ? "" : GET_PMWAPP()->GetResourceStringPointer (IDS_NAME_PLURAL));
	}
	else
	{
		// JRO sprintf(buffer, "(No fill-in fields)");
      sprintf(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_FILL_FIELDS));
	}

	SetDlgItemText(IDC_PRINT_NAMES_CHOSEN, buffer);
}

void CPmwPrint::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	standard_dialog_activate((UINT)(DWORD)m_pd.lpPrintTemplateName, nState, pWndOther, bMinimized);
	CPrintDialog::OnActivate(nState, pWndOther, bMinimized);
}

void CPmwPrint::OnPrintInstructions() 
{
	BOOL fPrintInstructions = IsDlgButtonChecked(IDC_PRINT_INSTRUCTIONS);

	CPrinterInfo* pPortraitInfo = GET_PMWAPP()->GetPrinterInfo(PORTRAIT);
	ASSERT(pPortraitInfo != NULL);
	if (pPortraitInfo != NULL)
	{
		pPortraitInfo->SetPrintInstructions(fPrintInstructions);
		pPortraitInfo->WriteSettings();
	}
	CPrinterInfo* pLandscapeInfo = GET_PMWAPP()->GetPrinterInfo(LANDSCAPE);
	ASSERT(pLandscapeInfo != NULL);
	if (pLandscapeInfo != NULL)
	{
		pLandscapeInfo->SetPrintInstructions(fPrintInstructions);
		if (pPortraitInfo == NULL)
		{
			pLandscapeInfo->WriteSettings();
		}
	}
}

void CPmwPrint::UpdateInstructions(void)
{
	CButton* pWnd = (CButton*)GetDlgItem(IDC_PRINT_INSTRUCTIONS);
	if (pWnd != NULL)
	{
		BOOL fEnabled = IsDlgButtonChecked(IDC_PRINT_DOUBLE_SIDED);
		pWnd->SetCheck(fEnabled && GET_PMWAPP()->GetPrinterInfo(m_pDoc->get_orientation())->GetPrintInstructions());
		pWnd->EnableWindow(fEnabled);
	}
}

void CPmwPrint::OnPrintDoubleSided() 
{
	UpdateInstructions();
}

/////////////////////////////////////////////////////////////////////////////
// CPmwPrinting dialog

CPmwPrinting::CPmwPrinting(WORD wSoundID, CWnd* pParent)
{
	//{{AFX_DATA_INIT(CPmwPrinting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_wSoundID = wSoundID;
	Create(CPmwPrinting::IDD, pParent);      // modeless !
	UserAbort = FALSE;
}

void CPmwPrinting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPmwPrinting)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPmwPrinting, CDialog)
	//{{AFX_MSG_MAP(CPmwPrinting)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPmwPrinting message handlers

BOOL CPmwPrinting::OnInitDialog()
{
//	SetWindowText(AfxGetAppName());

	standard_dialog_startup(this, m_wSoundID);

	return CDialog::OnInitDialog();
}

void CPmwPrinting::OnDestroy()
{
	standard_dialog_shutdown(this, m_wSoundID);

	CDialog::OnDestroy();
}

LRESULT CPmwPrinting::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, m_wSoundID, lParam);
}

void CPmwPrinting::OnCancel()
{
	UserAbort = TRUE;				// flag that user aborted print
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
// TileInfo structure

/*
// Get the information about tile counts, sizes and overlaps.
*/

VOID TileInfo::compute(CPmwDoc *pDoc, double scale)
{
	PCOORD tile_width_gap, tile_height_gap;
	SHORT x_tile_overlap, y_tile_overlap;
	SHORT x_tile_extra, y_tile_extra;
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();
	PPNT dims = pDoc->get_dimensions();

/*
// Continuous paper:
//		first sheet is paper height - top margin
//		middle sheet is paper height
//		last sheet is paper height - bottom margin
//
// Cut sheet paper:
//		all sheets are paper height - (top + bottom margins) - overlap
*/

/*
// Compute the maximum dimensions of a tile (in 1000ths of an inch).
*/

	CPaperInfo* pPaperInfo = pDoc->PrintPaperInfo();
	
	m_cut_sheet = (pPaperInfo->Type() == CPaperInfo::PAPER_CutSheet);

	CFixedPoint Dims = pPaperInfo->SlotDims();
	CFixedRect Margins = pPaperInfo->Margins();

	m_tile_width = MulFixed(Dims.x - (Margins.Left + Margins.Right),
									PAGE_RESOLUTION);

	m_tile_height = MulFixed(Dims.y - (Margins.Top + Margins.Bottom),
									PAGE_RESOLUTION);

	x_tile_overlap = y_tile_overlap = PAGE_TILE_OVERLAP;
	x_tile_extra = y_tile_extra = 0;

	if (!m_cut_sheet)
	{
		y_tile_overlap = 0;
	/* Full height page. */
		m_tile_height = MulFixed(Dims.y, PAGE_RESOLUTION);
	}

/* Account for the scale. */

//	printf("scale: %f\n", scale);

	if (scale != 1.0)
	{
		m_tile_width = (PCOORD)((double)m_tile_width/scale)+1;
		m_tile_height = (PCOORD)((double)m_tile_height/scale)+1;

		x_tile_overlap = (SHORT)((double)x_tile_overlap/scale)+1;
		if (y_tile_overlap != 0)
		{
			y_tile_overlap = (SHORT)((double)y_tile_overlap/scale)+1;
		}
	}

	if (pDoc->get_orientation() == LANDSCAPE)
	{
		PCOORD tmp;
		SHORT short_tmp;

	/* Swap everything around. */

		tmp = m_tile_height;
		m_tile_height = m_tile_width;
		m_tile_width = tmp;

		short_tmp = x_tile_extra;
		x_tile_extra = y_tile_extra;
		y_tile_extra = short_tmp;

		short_tmp = y_tile_overlap;
		y_tile_overlap = x_tile_overlap;
		x_tile_overlap = short_tmp;

	}

//	od("tile dimensions: %ld, %ld\r\n", m_tile_width, m_tile_height);

/*
// Now compute the number of tiles in each direction.
*/

	tile_width_gap = m_tile_width - x_tile_overlap + x_tile_extra;
	tile_height_gap = m_tile_height - y_tile_overlap + y_tile_extra;

	m_x_tiles = (SHORT)((dims.x - x_tile_overlap + (tile_width_gap-1))/tile_width_gap);
	m_y_tiles = (SHORT)((dims.y - y_tile_overlap + (tile_height_gap-1))/tile_height_gap);

/*
// Compute the actual width and height of the tiles in page coordinates.
// We use these scaling factors in case there is a scale going on beyond
// the mere page_?_resolution/1000.
*/

#if 0
	od("scaled tile dimensions: %ld, %ld; overlap %d, %d\r\n",
					m_tile_width, m_tile_height,
					x_tile_overlap, y_tile_overlap);

	od("actual page: %ld, %ld\r\n", dims);
	od("x tiles: %d, y tiles: %d\r\n", m_x_tiles, m_y_tiles);
#endif
	
	m_x_tile_overlap = x_tile_overlap;
	m_y_tile_overlap = y_tile_overlap;
}
/////////////////////////////////////////////////////////////////////////////
// CPrintSetupDialog dialog


CPrintSetupDialog::CPrintSetupDialog(CWnd* pParent /*=NULL*/)
	: CPrintDialog(TRUE)
{
	//{{AFX_DATA_INIT(CPrintSetupDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPrintSetupDialog::CPrintSetupDialog(PRINTDLG& pdInit, CWnd* pParent, int nHelpId)
	: CPrintDialog(pdInit)
{
	m_hWnd = NULL;
	m_pParentWnd = pParent;
	m_nIDHelp = nHelpId;
}

void CPrintSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CPrintDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintSetupDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintSetupDialog, CPrintDialog)
	//{{AFX_MSG_MAP(CPrintSetupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPrintSetupDialog message handlers

BOOL CPrintSetupDialog::OnInitDialog()
{
	CenterWindow();

	CPrintDialog::OnInitDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// PrintContext

PrintContext::PrintContext()
{
	SetDefaults();
}

void PrintContext::SetDefaults(void)
{
	m_fDoubleSided = FALSE;
	m_fReversed = FALSE;
	m_fStarted = FALSE;
   m_print_scale = 1.0;
}

/////////////////////////////////////////////////////////////////////////////
// CKeyArrowObject

/*
// The constructor.
*/

CKeyArrowObject::CKeyArrowObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: DrawingObject(type, owner)
{
	m_Outline.SetType(OutlineFormat::OutlineNone);
}

/*
// Get the path for this object.
*/

BOOL CKeyArrowObject::GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject /*=NULL*/)
{
	long lX0 = MakeFixed(crObject.left);
	long lY0 = MakeFixed(crObject.top);
	long lX1 = MakeFixed(crObject.left+crObject.Width());
	long lY1 = MakeFixed(crObject.top+crObject.Height());

	CFixedPoint Center;

	Center.x = (lX0 + lX1)/2;
	Center.y = (lY0 + lY1)/2;

	CFixed lArrowBodyLeft = lX0 + MakeFixed(crObject.Width())/4;
	CFixed lArrowBodyRight = lX1 - MakeFixed(crObject.Width())/4;

	Path.MoveTo(Center.x, lY0);
	Path.LineTo(lX1, Center.y);
	Path.LineTo(lArrowBodyRight, Center.y);
	Path.LineTo(lArrowBodyRight, lY1);
	Path.LineTo(lArrowBodyLeft, lY1);
	Path.LineTo(lArrowBodyLeft, Center.y);
	Path.LineTo(lX0, Center.y);
	Path.LineTo(Center.x, lY0);
	Path.Close();
	Path.End();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Print Preview stuff.

void CPmwView::OnFilePrintPreview()
{
	// In derived classes, implement special window handling here
	// Be sure to Unhook Frame Window close if hooked.

	// must not create this on the frame.  Must outlive this function
	CPrintPreviewState* pState = new CPrintPreviewState;

	// DoPrintPreview's return value does not necessarily indicate that
	// Print preview succeeded or failed, but rather what actions are necessary
	// at this point.  If DoPrintPreview returns TRUE, it means that
	// OnEndPrintPreview will be (or has already been) called and the
	// pState structure will be/has been deleted.
	// If DoPrintPreview returns FALSE, it means that OnEndPrintPreview
	// WILL NOT be called and that cleanup, including deleting pState
	// must be done here.

	if (!DoPrintPreview(
		GetPrintPreviewToolbar(), this,
							RUNTIME_CLASS(CPmwPreviewView), pState))
	{
		// In derived classes, reverse special window handling here for
		// Preview failure case

		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
	}
}

UINT CPmwView::GetPrintPreviewToolbar(void)
{
	return PrintPreviewPageCount() == 1 ? IDD_SINGLE_PREVIEW_TOOLBAR : IDD_PREVIEW_TOOLBAR;
}

int CPmwView::PrintPreviewPageCount(void)
{
	return (int)GetDocument()->NumberOfPages();
}

int CPmwView::SetPrintPreviewPage(int nPage)
{
	CPmwDoc* pDoc = GetDocument();

	DWORD dwDesiredPage = nPage-1;
	DWORD dwCurrentPage = pDoc->CurrentPageIndex();

	if (dwDesiredPage != dwCurrentPage)
	{
		pDoc->GotoPage(dwDesiredPage, FALSE);
	}

	return (int)dwCurrentPage+1;
}

void 
CPmwView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
	// JRO This is based on MFC (CView::DoPrintPreview()).
	// Selecting the close icon ('X') on a minimized print preview window
	// will cause it to be restored (beats crashing).
	CFrameWnd* pParent;
	CWnd* pNaturalParent = GetParentFrame();
	pParent = DYNAMIC_DOWNCAST(CFrameWnd, pNaturalParent);
	if ((pParent != NULL) && pParent->IsIconic())
		pParent->ShowWindow(SW_RESTORE);

	INHERITED::OnEndPrintPreview(pDC, pInfo, point, pView);
}
/////////////////////////////////////////////////////////////////////////////
// CPmwPreviewView

IMPLEMENT_DYNCREATE(CPmwPreviewView, CPreviewView)

CPmwPreviewView::CPmwPreviewView()
{
}

void CPmwPreviewView::OnUpdate(CView *sender, LPARAM lHint, CObject *pHint)
{
	switch (lHint)
	{
		case HINT_RefreshPreview:
		{
			// Add the extent.
			m_RefreshArray.Add((REFRESH_EXTENT*)pHint);
			break;
		}
		default:
		{
			// Do nothing!
			break;
		}
	}
}

/*
// Perform any refreshes which are pending.
*/

BOOL CPmwPreviewView::Refresh(void)
{
	BOOL fMore = FALSE;

	static BOOL fSemaphore = FALSE;
	CBoolSemaphore Semaphore(fSemaphore);

	if (!Semaphore.Lock())
	{
		// Pretend we got interupted. Everything stays the same.
		TRACE("Preview semaphore is locked...\n");
		return FALSE;
	}

	int nCount = m_RefreshArray.NumberOfExtents();

	if (nCount != 0)
	{
		if (IsIconic())
		{
			// All bets are off.
			// Throw out all refresh extents.
			m_RefreshArray.FreeAllExtents();

			return FALSE;
		}

		CPmwDoc *pDoc = (CPmwDoc*)GetDocument();
		HWND hWnd = GetSafeHwnd();
		HPALETTE hOurPal, hOldPal;

		BOOL fReversed = pDoc->get_project_type() == PROJECT_TYPE_TShirt;

		// Preview T-Shirt projects as reversed (they're default setting.)
		if (fReversed)
		{
			BOOL fAddRefreshes = pDoc->AddRefreshes();
			pDoc->AddRefreshes(FALSE);
			pDoc->FlipProject(OBJECT_FLAG_xflipped);
			pDoc->AddRefreshes(fAddRefreshes);
		}

//		od("refresh view %d...\r\n", m_RefreshArray.NumberOfExtents());

		// Save the hdc, just in case.

		HDC hdcSave = printer_rc.destination_hdc;

		/* Get a DC for our use. (Bypass the alloc of the CWnd's GetDC().) */

		printer_rc.destination_hdc = ::GetDC(hWnd);

		BOOL fIsPostscriptSave = printer_rc.is_postscript;
		printer_rc.is_postscript = FALSE;

	/*
	// When we are not allowed to draw, our window update gets locked.
	// During this time, we can still draw but it's all wasted and just
	// generates a PAINT message later, which isn't what we want.
	// When our update has been locked, the visible region of any DC we
	// get is of type NULLREGION. So we look at our visible region to see
	// if it's NULL and only draw if it's not.
	*/

		RECT rClipDC;
		int nRgnType;

		if ((nRgnType = GetClipBox(printer_rc.destination_hdc, &rClipDC)) == NULLREGION)
		{
		/* Try again later. */
//			od("GCB == NULL\r\n");
			fMore = TRUE;
		}
		else
		{
			// Select and realize THE PrintMaster palette.
			hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
			hOldPal = SelectPalette(printer_rc.destination_hdc, hOurPal, FALSE);
			RealizePalette(printer_rc.destination_hdc);

			// Set the function which will be called to check for interrupts.
			printer_rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&printer_rc);

			// Process the refresh extents.
			while (m_RefreshArray.NumberOfExtents() != 0)
			{
				// Get the next extent.
				REFRESH_EXTENT* pExtent = m_RefreshArray.GetExtent(0);
				PBOX pbox = pExtent->extent;

				// Clip the refresh extent against the boundaries of the view (source_pbox).
				if (IntersectBox(&pbox, &pbox, &printer_rc.source_pbox))
				{
					RECT rClip;

//					od("do_refresh %ld, %ld, %ld, %ld\r\n", pbox);

					// Compute the clipping rectangle in window coordinates.
					printer_rc.pbox_to_screen(&pbox, TRUE);
					printer_rc.convert_pbox(&pbox, &rClip, NULL);

					// See if we have anything left.
					if (!IsRectEmpty(&rClip))
					{
					/* Create a clipping region to force us to stay within this area. */

						HRGN hrgnClip = CreateRectRgn(rClip.left, rClip.top, rClip.right, rClip.bottom);

					/* Select a copy of the clip region into the DC. */

						int nRet = SelectClipRgn(printer_rc.destination_hdc, hrgnClip);

						if (nRet == NULLREGION || nRet == ERROR)
						{
//							od("SCR ret: %d\r\n", nRet);

							// Try again later.
							DeleteObject(hrgnClip);
							fMore = TRUE;
							break;
						}
						else
						{
							printer_rc.clip_region = hrgnClip;

							// Remember the clipping rectangle in the RC.
							GetClipBox(printer_rc.destination_hdc, &printer_rc.clip_rect);

							// Update the objects.
							CPmwView* pView = (CPmwView*)m_pPrintView;

							pView->m_pPreviewExtent = pExtent;

							// Set the page to be previewed.
							int nOldPage = pView->SetPrintPreviewPage(pExtent->m_nPreviewPage);

							CDC* pDC = CDC::FromHandle(printer_rc.destination_hdc);
							BOOL fPrintingSave = pDC->m_bPrinting;
							pDC->m_bPrinting = TRUE;
							pView->OnDraw(pDC);
							pDC->m_bPrinting = fPrintingSave;

							pView->m_pPreviewExtent = NULL;

							// Restore the old page.
							pView->SetPrintPreviewPage(nOldPage);

							// Delete the clip region.
							printer_rc.clip_region = NULL;
							DeleteObject(hrgnClip);

							if (pExtent->update_object != NULL)
							{
								// Interrupted.
								fMore = TRUE;
								break;
							}
							ASSERT(pExtent->update_state == NULL);
						}
					}
				}

				// This finished! Delete any update state.
				pExtent->FreeUpdateState();

				// Remove it!
				m_RefreshArray.DeleteExtent(0);
			}

			// Select the other palette in.
			SelectPalette(printer_rc.destination_hdc, hOldPal, FALSE);
		}

		// If we are previewing reversed, flip the project.
		if (fReversed)
		{
			BOOL fAddRefreshes = pDoc->AddRefreshes();
			pDoc->AddRefreshes(FALSE);
			pDoc->FlipProject(OBJECT_FLAG_xflipped);
			pDoc->AddRefreshes(fAddRefreshes);
		}

		// Release the DC we got.
		::ReleaseDC(hWnd, printer_rc.destination_hdc);
		printer_rc.destination_hdc = hdcSave;
		printer_rc.is_postscript = fIsPostscriptSave;
	}

	return fMore;
}

BEGIN_MESSAGE_MAP(CPmwPreviewView, CPreviewView)
   //{{AFX_MSG_MAP(CPmwPreviewView)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CPmwView::PrintSingleSided (int nFromPage, int nToPage, PrintContext& pc)
{
	CPmwDoc *pDoc = GetDocument();
	int nError=0;
	// Single sided. Print out all the pages in the desired range.
	for (int nPage = nFromPage; nError == 0 && nPage <= nToPage; nPage++)
	{
		DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage-1);
		if (lPage == 0)
		{
			nError = -1;
		}
		else
		{
			pDoc->LoadPage(lPage, TRUE, FALSE);
			nError = PrintCurrentPage(pc, nPage);
		}
	}
	return nError;
}

void CPmwPrint::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();
	
	CPrintDialog::OnRButtonDown(nFlags, point);
}

void CPmwPrint::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();
	
	CPrintDialog::OnLButtonDown(nFlags, point);
}
