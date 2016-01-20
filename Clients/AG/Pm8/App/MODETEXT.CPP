/*
// $Header: /PM8/App/MODETEXT.CPP 1     3/03/99 6:08p Gbeddow $
//
// Text editing mode routines.
//
// $Log: /PM8/App/MODETEXT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 31    3/01/99 7:10p Psasse
// Support for Vertical Banner Text
// 
// 30    2/25/99 10:35a Cschende
// Added menu check marks to reflect drop cap settings in text. When
// applying drop caps turn off bullets and if applying bullets turn off
// drop caps. Now
// use ApplyDropCap() function.
// 
// 29    2/11/99 11:55a Cschende
// Added code for Drop Caps implementation
// 
// 28    1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 27    1/14/99 9:27a Cschende
// ixed one bug in insert symbol, added functionality for insert date
// 
// 26    1/07/99 3:58p Cschende
// added command handlers for InsertDate
// 
// 25    1/07/99 3:54p Cschende
// added an undo for InsertSymbol
// 
// 24    1/06/99 2:05p Cschende
// hide the apply button in the bullet property sheet the right way!
// 
// 23    1/06/99 10:31a Cschende
// modified the Insert Symbol dialog to take a string ID in the
// constructor
// 
// 22    1/05/99 5:46p Cschende
// added Insert Symbol command handler
// 
// 21    12/22/98 4:39p Cschende
// updated for BulletDialog constructor
// 
// 20    12/17/98 3:23p Cschende
// removed temporary bullet selection menu commands
// 
// 19    12/15/98 5:47p Cschende
// changed logic for bulletlevel to be 0 based to fix bugs
// 
// 18    12/10/98 4:10p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 17    8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 16    7/24/98 4:36p Hforman
// added STATE_RELEASE after doubleclick
// 
// 15    7/23/98 6:36p Johno
// Changed when m_Txp.CalcFrame() is called in CPmwView::reframe_on_txp().
// This eliminated a ThrowErrorcodeException() in CTxp::LockFrame() after
// a spell check was allowed to complete after changing some text. This
// may be related to phantom spell check errors seen by QA, or it may have
// done nothing.
// 
// 14    7/23/98 3:39p Johno
// Support for tabbing between table cells in text_edit_keystroke()
// 
// 13    7/23/98 9:14a Jayn
// Hooks for table cell next and previous.
// 
// 12    7/21/98 8:39a Jayn
// ChooseFont now supports tables
// 
// 11    7/19/98 6:26p Jayn
// Merge cells. Applying font attributes to cells.
// 
// 10    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 9     6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 8     5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 7     3/27/98 10:32a Rlovejoy
// Use UpdateMRU() function.
// 
// 6     3/25/98 12:06p Rlovejoy
// Callback to update font MRU.
// 
// 5     3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 4     2/25/98 10:40a Jstoner
// table functionality updates...merge/split cells
// row/col selection done
// 
// 3     2/11/98 11:38a Jstoner
// table object additions
// 
// 2     2/03/98 10:34a Jstoner
// saving incremental progress 
// 
//    Rev 1.0   14 Aug 1997 15:22:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:16   Fred
// Initial revision.
// 
//    Rev 1.66   16 Jul 1997 13:19:58   Fred
// Fixed text selection bugs
// 
//    Rev 1.65   11 Jul 1997 09:56:24   Jay
// Fixed a backspace problem
// 
//    Rev 1.64   08 Jul 1997 12:10:20   Jay
// Refresh bounds was too unstable to use for edit background.
// 
//    Rev 1.63   30 Jun 1997 11:55:14   Jay
// New stretching logic.
// 
//    Rev 1.62   26 Jun 1997 12:19:02   Jay
// Updates edit bitmap if area changes size or moves.
// 
//    Rev 1.61   06 Jun 1997 11:51:20   Jay
// Licensing changes
// 
//    Rev 1.60   05 Jun 1997 16:45:40   Jay
// Changed upto_object in DrawObjects
// 
//    Rev 1.59   30 May 1997 12:43:24   Jay
// PasteSentiment
// 
//    Rev 1.58   29 May 1997 08:45:34   Fred
// New fill color
// 
//    Rev 1.57   18 Apr 1997 13:41:50   Jay
// Default stretching
// 
//    Rev 1.56   16 Apr 1997 12:12:24   Jay
// Fixes for text undo
// 
//    Rev 1.55   14 Apr 1997 15:42:34   Jay
// Text box editing changes
// 
//    Rev 1.54   03 Apr 1997 17:22:54   Fred
// Fixed some warnings in 16-bit
// 
//    Rev 1.53   03 Apr 1997 14:31:16   Jay
// Resource cleanup
// 
//    Rev 1.52   28 Mar 1997 17:10:48   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.51   13 Mar 1997 15:06:28   Fred
// Fixed text editing redisplay glitch when rulers are on
// 
//    Rev 1.50   12 Mar 1997 14:53:58   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.49   11 Mar 1997 16:13:50   Jay
// Extensions to the workspace
// 
//    Rev 1.48   06 Mar 1997 15:46:14   Fred
// Reframes cursor in rotated and warped text box.
// 
//    Rev 1.47   05 Mar 1997 14:36:34   Fred
// More text object stuff
// 
//    Rev 1.46   21 Feb 1997 15:11:26   Jay
// New mode handling; crop mode support
// 
//    Rev 1.46   21 Feb 1997 15:09:16   Jay
// New mode handling; crop mode support
// 
//    Rev 1.45   07 Feb 1997 08:45:52   Fred
//  
// 
//    Rev 1.44   24 Jan 1997 14:57:30   Fred
// Added some comments
// 
//    Rev 1.43   17 Jan 1997 12:13:38   Jay
// New creation tools.
// 
//    Rev 1.42   15 Jan 1997 10:49:22   Jay
// New toolbars
// 
//    Rev 1.41   09 Jan 1997 16:56:08   Fred
//  
// 
//    Rev 1.41   09 Jan 1997 16:49:52   Fred
//  
// 
//    Rev 1.40   09 Jan 1997 08:58:10   Fred
// More text box changes
// 
//    Rev 1.39   08 Jan 1997 15:21:26   Fred
// More text box changes
// 
//    Rev 1.38   08 Jan 1997 15:15:36   Jay
// Character spacing
// 
//    Rev 1.37   08 Jan 1997 11:29:20   Fred
// More text object changes
// 
//    Rev 1.36   06 Jan 1997 17:07:04   Jay
// Character spacing.
// 
//    Rev 1.35   24 Dec 1996 12:07:20   Jay
// Text import.
// 
//    Rev 1.34   13 Dec 1996 13:14:06   JAY
// 
//    Rev 1.33   05 Nov 1996 15:31:44   johno
// Moved strings to resource DLL
// 
//    Rev 1.32   28 Oct 1996 10:19:40   johno
// Moved strings to resource file
// 
//    Rev 1.31   15 Oct 1996 09:02:04   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.30   06 Sep 1996 18:13:36   Jay
// Sound changes.
// 
//    Rev 1.29   08 Aug 1996 13:39:10   Jay
// Frame link tweaks.
// 
//    Rev 1.28   01 Aug 1996 17:46:02   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.27   28 Jul 1996 16:57:48   Jay
// Text tweaks
// 
//    Rev 1.26   28 Jul 1996 10:18:54   Jay
// Text color, new color dialog
// 
//    Rev 1.25   25 Jul 1996 13:14:22   Jay
// Multi-page frame glitch correction.
// 
//    Rev 1.24   25 Jul 1996 11:16:58   Jay
// More page stuff
// 
//    Rev 1.23   22 Jul 1996 13:06:18   Jay
// New text select mechanism
// 
//    Rev 1.22   15 Jul 1996 11:35:02   Jay
//  
// 
//    Rev 1.21   12 Jul 1996 18:01:46   Jay
// Previous and next frame links.
// 
//    Rev 1.20   11 Jul 1996 17:46:12   Jay
//  
// 
//    Rev 1.19   11 Jul 1996 11:35:48   Jay
//  
// 
//    Rev 1.18   09 Jul 1996 15:37:34   Jay
//  
// 
//    Rev 1.17   08 Jul 1996 17:53:32   Jay
// Linked frames
// 
//    Rev 1.16   08 Jul 1996 08:49:42   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.15   05 Jul 1996 10:47:42   Jay
//  
// 
//    Rev 1.14   02 Jul 1996 11:25:14   Jay
//  
// 
//    Rev 1.13   27 Jun 1996 17:21:06   Jay
//  
// 
//    Rev 1.12   26 Jun 1996 14:23:40   Jay
//  
// 
//    Rev 1.11   25 Jun 1996 13:13:28   Jay
//  
// 
//    Rev 1.10   25 Jun 1996 10:25:46   Jay
//  
// 
//    Rev 1.9   24 Jun 1996 18:06:52   Jay
//  
// 
//    Rev 1.8   21 Jun 1996 17:50:44   Jay
//  
// 
//    Rev 1.7   21 Jun 1996 14:31:14   Jay
// Stretch text, etc.
// 
//    Rev 1.6   20 Jun 1996 15:39:48   Jay
// Now handles multiple paragraph tabs correctly.
// 
//    Rev 1.5   18 Jun 1996 16:15:16   Jay
//  
// 
//    Rev 1.4   17 Jun 1996 12:02:44   Jay
//  
// 
//    Rev 1.3   14 Jun 1996 17:06:42   Jay
//  
// 
//    Rev 1.2   13 Jun 1996 15:53:52   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:21:48   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:39:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:32   FRED
// Initial revision.
// 
//    Rev 2.15   06 Sep 1995 08:02:44   JAY
// Now marks macros as modified after InsertField dialog.
// 
//    Rev 2.14   25 Aug 1995 15:00:56   JAY
// Fill-in fields stuff.
// 
//    Rev 2.13   18 Aug 1995 15:03:48   JAY
// No longer steals the focus unless the combo box (or edit) has it.
// 
//    Rev 2.12   15 Aug 1995 14:11:08   FRED
// More Zoom Stuff
// 
//    Rev 2.11   14 Aug 1995 11:44:26   JAY
// Only rebuilds fill-in field list when necessary.
// 
//    Rev 2.10   14 Aug 1995 09:36:02   JAY
// Tetx undo now handles fill-in fields correctly.
// 
//    Rev 2.9   06 Aug 1995 11:23:16   JAY
// Now draws the page background into the edit background bitmap.
// 
//    Rev 2.8   02 Aug 1995 12:57:46   JAY
// New insert fill-in fields
// 
//    Rev 2.7   26 Apr 1995 14:41:26   JAY
//  
// 
//    Rev 2.6   19 Apr 1995 12:34:30   JAY
//  
// 
//    Rev 2.5   29 Mar 1995 09:14:50   JAY
// Lots of changes
// 
//    Rev 2.4   15 Mar 1995 10:48:32   JAY
// Support for new text editing
// 
//    Rev 2.3   09 Mar 1995 13:51:00   JAY
// Blinking text cursor.
// 
//    Rev 2.2   17 Feb 1995 16:47:36   JAY
// Text editing undo and redo.
// 
//    Rev 2.1   08 Feb 1995 13:33:42   JAY
// Reverted. New series.
// 
//    Rev 1.21   04 Aug 1994 08:30:42   JAY
// Large model and 32-bit changes
// 
//    Rev 1.20   16 May 1994 08:46:44   JAY
// Broke the RC gamma curve into two separate curves, one for bitmaps and one
// for outlines.
// 
//    Rev 1.19   06 May 1994 17:09:04   JAY
// Added sound sequence support.
// 
//    Rev 1.18   03 May 1994 17:06:32   JAY
// Ignores mouse downs if we didn't get the rising or double click. This fixes
// problems with double-clicking out of a dialog box.
// 
//    Rev 1.17   08 Apr 1994 11:06:54   JAY
// Now reports the appropriate "Full" message when the paragraph will not receive
// any more characters.
// 
//    Rev 1.16   18 Mar 1994 11:33:06   JAY
// Tabs now work in banner text boxes that aren't the main one.
// 
//    Rev 1.15   18 Mar 1994 08:41:30   JAY
// Added support for CTRL-left, right, home and end.
// 
//    Rev 1.14   18 Mar 1994 08:13:50   JAY
// Fixed bug with text cursor I introduced in last rev.
// Maybe someday see why the assumption (cursor box when on is valid) wasn't true.
// 
//    Rev 1.13   16 Mar 1994 10:42:22   JAY
// reframe_on_txp() now calcs the cursor box before using it.
// 
//    Rev 1.12   15 Mar 1994 17:12:20   JAY
// Removed object argument. Gets it from the txp now.
// 
//    Rev 1.11   15 Mar 1994 13:46:34   JAY
// Added code for creating and freeing the edit background.
// 
//    Rev 1.10   11 Mar 1994 14:29:44   JAY
// Doesn't add a refresh extent when a text box is initially created. Since the
// box is initially empty, it caused some unnecessary redrawing.
// 
//    Rev 1.9   25 Feb 1994 14:13:36   JAY
// Won't turn the cursor on (or anything else special) if the event is a key up.
// 
//    Rev 1.8   10 Feb 1994 16:18:22   JAY
// Now changes whole paragraph style on font change if paragraph has no text.
// 
//    Rev 1.7   10 Feb 1994 15:53:58   MACDUFF
// Toggle functions now return a flag indicating whether they
// drew anything.
// 
//    Rev 1.6   31 Jan 1994 17:44:18   JAY
//  
// 
//    Rev 1.5   28 Jan 1994 17:12:50   JAY
//  
// 
//    Rev 1.4   25 Jan 1994 12:56:38   JAY
//  
// 
//    Rev 1.3   19 Jan 1994 17:09:22   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:03:32   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:11:06   JAY
// Restructuring, cut/paste, images, etc.
//
//   Rev 1.0   16 Dec 1993 16:28:22   JAY
//
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwcfg.h"
#include "pmgfont.h"
#include "typeface.h"

#include "textflow.h"
#include "utils.h"

#include "framerec.h"
#include "pararec.h"
#include "frameobj.h"
#include "tcommand.h"
#include "textrec.h"
#include "tblcmd.h"
#include "toolbar.h"

#include "macrorec.h"
#include "macrostd.h"

#include "desktop.h"
#include "fielddlg.h"

#include "warpfld.h"
#include "calobj.h"
#include "dateobj.h"
#include "tblobj.h"
#include "outpath.h"

#include "paradlg.h"
#include "tabsdlg.h"
#include "framedlg.h"
#include "spacedlg.h"
#include "sortarry.h"
#include "celobj.h"
#include "bulletdialog.h"
#include "InsertSymbolDialog.h"

extern CPalette *pOurPal;

/*
// Toggle a rectangle on the redisplay map.
// The destination_hdc field must be valid in the rc before calling this.
*/

void CPmwView::toggle_square(PBOX pbox, LPRECT pClip /*=NULL*/, CFrameObject* pFrameObject /*=NULL*/)
{
	if (m_Txp.Valid()
			&& m_Txp.Object() != NULL
			&& pbox.x0 != pbox.x1
			&& pbox.y0 != pbox.y1)
	{
		CRect r;

		if (pbox.x0 > pbox.x1)
		{
			PCOORD x0 = pbox.x0;
			pbox.x0 = pbox.x1;
			pbox.x1 = x0;
		}

		if (pbox.y0 > pbox.y1)
		{
			PCOORD y0 = pbox.y0;
			pbox.y0 = pbox.y1;
			pbox.y1 = y0;
		}

		if (pbox.x0 <= pbox.x1 && pbox.y0 <= pbox.y1)
		{
			rc.pbox_to_screen(&pbox, TRUE);
			if (rc.convert_pbox(&pbox, r, NULL))
			{
				if (pFrameObject == NULL)
				{
					// Use default frame object if there is one.
					if (m_Txp.Object()->type() == OBJECT_TYPE_Frame)
					{
						pFrameObject = m_Txp.Object();
					}
				}

				// There are two cases: Frame (text) object or not. For frame objects
				// the toggle rectangle has not yet been warped, flipped, or rotated.
				// For non-frame objects, we just need to toggle the square.
				if (pFrameObject == NULL)
				{
					// Not a frame object, toggle the rectangle without changing it.
					PatBlt(rc.destination_hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, DSTINVERT);
				}
				else
				{
					// We have a frame object so the rectangle is "normalized" and still needs to be
					// warped, flipped, and rotated.

					// Get the unrotated bounds of the object.
					PBOX UnrotatedBound = pFrameObject->get_unrotated_bound();
					rc.pbox_to_screen(&UnrotatedBound, TRUE);

					// Check if the object is warped, flipped, and/or rotated.
					CWarpField* pWarpField = pFrameObject->GetWarpField();
					ANGLE dRotation = pFrameObject->get_rotation();
					BOOL fWarped = pWarpField != NULL;
					BOOL fRotated = dRotation != 0.0;
					BOOL fXFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_xflipped) != 0;
					BOOL fYFlipped = (pFrameObject->get_flags() & OBJECT_FLAG_yflipped) != 0;

					if (fWarped || fRotated)
					{
						// Object is warped and/or rotated.
						if ((UnrotatedBound.x1 - UnrotatedBound.x0 > 16384)
						 || (UnrotatedBound.y1 - UnrotatedBound.y0 > 16384))
						{
							// This object will be too big to handle.
							return;
						}

						CRect crObject;
						if (!rc.convert_pbox(&UnrotatedBound, crObject, NULL))
						{
							// This object is clipped or too small.
							return;
						}

						// Create a transformation matrix that embodies and X and Y
						// flipping along with any rotation. Remember, PMG maintains
						// object properties so that the final object position can
						// always be reached by applying flipping first and then
						// rotation.
						COutlinePathMatrix RotateMatrix;
						RotateMatrix.Identity();
						
						if (fXFlipped)
						{
							RotateMatrix.XFlip(MakeFixed(crObject.left), MakeFixed(crObject.right));
						}
						if (fYFlipped)
						{
							RotateMatrix.YFlip(MakeFixed(crObject.top), MakeFixed(crObject.bottom));
						}

						if (fRotated)
						{
							RotateMatrix.TranslateBy(
								-MakeFixed(crObject.left+crObject.Width()/2),
								-MakeFixed(crObject.top+crObject.Height()/2));
							RotateMatrix.RotateBy(MakeFixed(dRotation*RAD2DEGC));
							RotateMatrix.TranslateBy(
								MakeFixed(crObject.left+crObject.Width()/2),
								MakeFixed(crObject.top+crObject.Height()/2));
						}

						// Build a path for the unwarped, unflipped, and unrotated toggle
						// square (in device coordinates.) Paths that have collapsed down to
						// a single pixel in width and height are treated as a special case and
						// drawn using polyline() instead of polygon(). The usual case
						// where this is done is for the cursor and makes the cursor look
						// a lot better.
						COutlinePath Path;
						Path.Free();

						if (r.Width() == 1)
						{
							// Build the path for this vertical line.
							Path.UsePolyline(TRUE);	// force line draw
							Path.MoveTo(MakeFixed(r.left), MakeFixed(r.top));
							Path.LineTo(MakeFixed(r.left), MakeFixed(r.bottom));
						}
						else if (r.Height() == 1)
						{
							// Build the path for this horizontal line.
							Path.UsePolyline(TRUE);	// force line draw
							Path.MoveTo(MakeFixed(r.left), MakeFixed(r.top));
							Path.LineTo(MakeFixed(r.right), MakeFixed(r.top));
						}
						else
						{
							// Build the path for this square.
							Path.MoveTo(MakeFixed(r.left), MakeFixed(r.top));
							Path.LineTo(MakeFixed(r.right), MakeFixed(r.top));
							Path.LineTo(MakeFixed(r.right), MakeFixed(r.bottom));
							Path.LineTo(MakeFixed(r.left), MakeFixed(r.bottom));
							Path.LineTo(MakeFixed(r.left), MakeFixed(r.top));
						}

						Path.Close();
						Path.End();

						// Warp the object if there's a warp field.
						if (fWarped)
						{
							pWarpField->Transform(crObject);
							Path.Warp(pWarpField);
						}

						if (fXFlipped || fYFlipped || fRotated)
						{
							Path.Transform(&RotateMatrix);
						}

						int nROP2 = ::SetROP2(rc.destination_hdc, R2_NOT);
						Path.Draw(
							CDC::FromHandle(rc.destination_hdc),
							RGB(255,255,255),
							!Path.UsePolyline(),
							0,
							RGB(255,255,255),
							Path.UsePolyline(),
							WINDING);
						::SetROP2(rc.destination_hdc, nROP2);
					}
					else
					{
						// Object is not warped or rotated (it may still be flipped.)
						// In this case, we can use PatBlt().

						if (fXFlipped)
						{
							r.left = (int)(UnrotatedBound.x1+UnrotatedBound.x0)-r.left;
							r.right = (int)(UnrotatedBound.x1+UnrotatedBound.x0)-r.right;
						}

						if (fYFlipped)
						{
							r.top = (int)(UnrotatedBound.y1+UnrotatedBound.y0)-r.top;
							r.bottom = (int)(UnrotatedBound.y1+UnrotatedBound.y0)-r.bottom;
						}

						PatBlt(rc.destination_hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, DSTINVERT);
					}
				}
			}
		}
	}
}

/*
// A little routine to help this damn flipped thing.
*/

static void near
do_UnionBox(PBOX_PTR dest, PBOX src, BOOL first_time)
{
	if (src.x1 < src.x0)
	{
		PCOORD x = src.x0;
		src.x0 = src.x1;
		src.x1 = x;
	}

	if (first_time)
	{
		*dest = src;
	}
	else
	{
		UnionBox(dest, dest, &src);
	}
}

/*
// Toggle the select.
// The destination_hdc field must be valid in the rc before calling this.
*/

void CPmwView::ToggleSelect(CTxp* pTxp, LPRECT pClip /*=NULL*/)
{
	if (pTxp->SomethingSelected())
	{
		pTxp->UpdateSelectBounds();
		int nBounds = pTxp->NumberOfSelectBounds();
		for (int nBound = 0; nBound < nBounds; nBound++)
		{
			PBOX Bound = pTxp->SelectBound(nBound);
			CFrameObject* pFrameObject = pTxp->SelectFrame(nBound);
			ASSERT(pFrameObject != NULL);

			toggle_square(Bound, pClip, pFrameObject);
		}
	}
}

/*
// The toggle function for the txp.
*/

void CPmwView::TxpToggleFunction(CTxp* pTxp, int nWhich, LPVOID pData)
{
	CPmwView *view = (CPmwView *)pData;
	HWND hWnd = view->GetSafeHwnd();
	HDC hdcSave = view->rc.destination_hdc;

/*
// Get an HDC to use.
*/

	view->rc.destination_hdc = ::GetDC(hWnd);

	if (nWhich & CTxp::TOGGLE_Cursor)
	{
		if (pTxp->Visible() & CTxp::TOGGLE_Cursor)
		{
//			SHORT position;
			BOOL was_visible = view->cursor_visible;

		/* Turning on! */

//			od("Cursor going on...\r\n");
			view->cursor_on = TRUE;
//			view->cursor_visible = TRUE;

			if (view->cursor_visible)
			{
				view->toggle_square(pTxp->CursorBox());
			}

			view->SetTimer(TXP_CURSOR_TIMER, ::GetCaretBlinkTime(), NULL);
		}
		else
		{
		/* Turning off! */

//			od("Cursor going off...\r\n");
			if (view->cursor_on && view->cursor_visible)
			{
				view->toggle_square(pTxp->CursorBox());
			}
			view->KillTimer(TXP_CURSOR_TIMER);
//			on_time = 0;

			view->cursor_on = FALSE;
//			invoke_region(edit_context.text_echo_region, TEXT_EDIT_CURSOR_OFF, NULL);
		}
	}

	if (nWhich & CTxp::TOGGLE_Select)
	{
		view->ToggleSelect(pTxp);
	}

	::ReleaseDC(hWnd, view->rc.destination_hdc);
	view->rc.destination_hdc = hdcSave;
}

/*
// Our toggle for when redisplaying is happening.
*/

BOOL CPmwView::redisp_toggle_function(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data)
{
	CPmwView* pView = (CPmwView *)data;
	FLAGS fVisible = pView->m_Txp.Visible();

	if (pView->cursor_visible && (fVisible & CTxp::TOGGLE_Cursor))
	{
		pView->toggle_square(pView->m_Txp.CursorBox(), clip);
	}

	if (fVisible & CTxp::TOGGLE_Select)
	{
		pView->ToggleSelect(&(pView->m_Txp), clip);
	}

	return TRUE;
}

/*
// Handle a keystroke.
*/

VOID CPmwView::text_edit_keystroke(EVENT_INFO *info)
{
	BOOL shifted = SHIFT_KEY(info->flags);
	BOOL modified = FALSE;
	CPmwDoc *pDoc = GetDocument();
	PBOX bound;
	FLAGS flags = m_Txp.Flags();
	BOOL	fVerticalText = FALSE;

	if (flags & CTxp::FLAG_stretch_frame)
	{
		bound = m_Txp.Object()->get_bound();
	}

	switch (info->event)
	{
		case EVENT_KEY_RISING:
		case EVENT_KEY_DOWN:
		{
			switch (info->v_key)
			{
				case VK_UP:
				{
					m_Txp.Up(shifted);
					break;
				}
				case VK_DOWN:
				{
					m_Txp.Down(shifted);
					break;
				}
				case VK_LEFT:
				{
					if (CTRL_KEY(info->flags))
					{
						m_Txp.LeftWord(shifted);
					}
					else
					{
						m_Txp.Left(shifted);
					}
					break;
				}
				case VK_RIGHT:
				{
					if (CTRL_KEY(info->flags))
					{
						m_Txp.RightWord(shifted);
					}
					else
					{
						m_Txp.Right(shifted);
					}
					break;
				}
				case VK_HOME:
				{
					if (CTRL_KEY(info->flags))
					{
						m_Txp.Bot(shifted);
					}
					else
					{
						m_Txp.Bol(shifted);
					}
					break;
				}
				case VK_END:
				{
					if (CTRL_KEY(info->flags))
					{
						m_Txp.Eot(shifted);
					}
					else
					{
						m_Txp.Eol(shifted);
					}
					break;
				}
				case VK_PRIOR:
				{
					m_Txp.Bot(shifted);
					break;
				}
				case VK_NEXT:
				{
					m_Txp.Eot(shifted);
					break;
				}
				case VK_BACK:
				{
               int nCount = info->repeat_count;
					
					if (pDoc->get_project_type() == PROJECT_TYPE_Banner
							&& pDoc->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical
							&& m_Txp.Object()->get_panel() == MAIN_BANNER_PANEL)
					{
						nCount++;
					}

					UpdateUndoBackspace(&m_Txp, nCount);   // May change nCount.

					m_Txp.Backspace(nCount);

					modified = TRUE;
					break;
				}
				case VK_DELETE:
				{
					UpdateUndoDelete(&m_Txp, info->repeat_count);

					m_Txp.DeleteText(info->repeat_count);

					modified = TRUE;
					break;
				}
				default:
				{
					if (CTRL_KEY(info->flags))
					{
					/* See if this is a control key we trap. */
						switch (info->v_key)
						{
							case 'B':
							{
								OnTextBold();
								break;
							}
							case 'I':
							{
								OnTextItalic();
								break;
							}
							case 'U':
							{
								OnTextUnderline();
								break;
							}
							default:
							{
								break;
							}
						}
					}
//					od("Key %x pressed [%x]\r\n", info->v_key, info->flags);
					break;
				}
			}
			break;
		}
		case EVENT_KEY:
		{
//			od("Got key %d (%d) [%x]\r\n", info->v_key, info->repeat_count, info->flags);

			CHARACTER c;
			BOOL do_insert = TRUE;

			c = info->v_key;

			if (c < ' ' && CTRL_KEY(info->flags))
			{
			/* Don't insert any control keys. */
				do_insert = FALSE;
			}
			else
			{
				switch (c)
				{
					case VK_BACK:
					{
						do_insert = FALSE;
						break;
					}
					case VK_TAB:
					{
						// See if we are editing a table cell.
						if (m_Txp.Object()->get_panel() == OBJECT_IN_CELL_PANEL)
						{
							CCellObject		*pCell;
							CTableObject	*pTable;
							// Get the pointers we need and check their type
							if ((pCell = (CCellObject*)m_Txp.Object()->get_container()) != NULL
								&& pCell->type() == OBJECT_TYPE_Cell
								&& (pTable = (CTableObject*)pCell->get_container()) != NULL
								&& pTable->type() == OBJECT_TYPE_Table)
							{
								short TableWidth = pTable->GetColumns();
								short TableHeight = pTable->GetRows();
								if ((1 < TableWidth) && (1 < TableHeight))
								{
									CPoint	CellCoords = pCell->GetCell();
									if (shifted)
									{
										// We want to go to the previous cell.
										CellCoords.x --;
									}
									else
									{
										// We want to go to the next cell.
										CellCoords.x ++;
									}

									if (CellCoords.x >= TableWidth)
									{
										CellCoords.x = 0;
										CellCoords.y ++;
									}
									else
									if (CellCoords.x < 0)
									{
										CellCoords.x = TableWidth - 1;
										CellCoords.y --;
									}

									if (CellCoords.y >= TableHeight)
										CellCoords.y = 0;
									else
									if (CellCoords.y < 0)
										CellCoords.y = TableHeight - 1;
								
									CFrameObject* pFrame = pTable->GetCellFrame(CellCoords);
									if (pFrame != NULL)
									{
										//set_arrow_tool();
										edit_frame(pFrame);
									}
								}
							}
							// And do not insert the tab
							do_insert = FALSE;
						}
						// if we are in a bulleted list see if we need to indent or outdent the list
						else if( m_Txp.Style().BulletType() != BULLET_none )
						{

							CTextStyle Style(m_Txp.Style());
							if( !shifted )	// indent a level
								Style.BulletLevel( min( (Style.BulletLevel() + 1), BULLET_LEVEL_MAX - 1 ) ); 
							else			// go back a level
								Style.BulletLevel( max( 0, Style.BulletLevel() - 1 ) );

							CCmdParagraphFormatting* pCommand = new CCmdParagraphFormatting;
							if (pCommand->Before(this))
							{
								pCommand->After(Style.m_Paragraph);
								GetDocument()->AddCommand(pCommand);
								GetDocument()->SetModifiedFlag();

								do_insert = FALSE;
							}

						}
						else
						{
							c = '\t';
						}
						break;
					}
					case VK_RETURN:
					{
						// If in a bulleted list 2 returns ends the list. Check for this case
						if( m_Txp.Style().BulletType() != BULLET_none )
						{
							CHARACTER_INDEX index = m_Txp.Index();
							CTextRecord *pText = m_Txp.LockText();
							// if the current character is a return end the bullets
							if(index > 0  && *(pText->GetCharacter(index -1)) == '\n')
							{
								CTextStyle Style(m_Txp.Style());
								Style.BulletType( BULLET_none );
								CCmdParagraphFormatting* pCommand = new CCmdParagraphFormatting;
								if (pCommand->Before(this))
								{
									pCommand->After(Style.m_Paragraph);
									GetDocument()->AddCommand(pCommand);
									GetDocument()->SetModifiedFlag();
								}
							}
							pText->release();
						}
						c = '\n';
						break;
					}
					default:
					{
						break;
					}
				}
			}

		/* Insert the character. */

			if (do_insert)
			{
				if (pDoc->get_project_type() == PROJECT_TYPE_Banner
						&& m_Txp.Object()->get_panel() == MAIN_BANNER_PANEL)
				{
				
					m_Txp.SetIgnoreBoundaries(TRUE);
					

					if(c == '\t')
					{
						/* Don't use TABS in banners. They just don't work! */
						c = ' ';
					}
				}
				else
				{
					m_Txp.SetIgnoreBoundaries(FALSE);
				}

				UpdateUndoTyping(&m_Txp, c, info->repeat_count);

				if (m_Txp.InsertText(&c, info->repeat_count, TRUE) == ERRORCODE_Full)
				{
					AfxMessageBox(IDS_TEXT_BOX_FULL, MB_OK | MB_ICONEXCLAMATION);
				}

				if (c != 10
						&& m_Txp.Object()->get_panel() == MAIN_BANNER_PANEL	
						&& pDoc->get_project_type() == PROJECT_TYPE_Banner
						&& pDoc->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
				{
					fVerticalText = TRUE;

					unsigned short ch = '\n';

					UpdateUndoTyping(&m_Txp, ch, 1);

					if (m_Txp.InsertText(&ch, 1, TRUE) == ERRORCODE_Full)
					{
						AfxMessageBox(IDS_TEXT_BOX_FULL, MB_OK | MB_ICONEXCLAMATION);
					}
				}

				//				od("Inserted %x\r\n", c);
				modified = TRUE;
			}

			break;
		}
		default:
		{
			break;
		}
	}

/* See if the txp moved. */

	TxpCursorOn();

/* See if we did something to modify us. */

	if (modified)
	{
	/* We have been modified. */

		pDoc->SetModifiedFlag();

		CFrameObject* pObject = m_Txp.Object();
		if (flags & CTxp::FLAG_stretch_frame)
		{
			PBOX new_bound = pObject->get_bound();

			if ((new_bound.x1 != bound.x1)
				|| (new_bound.y1 != bound.y1))
			{
			/* We need to resize the project. */
				pDoc->resize_project(pObject);
			}
				
		}
		else if (pObject->get_panel() == OBJECT_IN_CELL_PANEL)
		{
			pDoc->resize_project(pObject);
		}
	}
	reframe_on_txp();
}

/*
// Normal state.
*/

BOOL CPmwView::text_edit_state_normal(EVENT_INFO *info)
{
	EVENT_INFO new_info;

	PPNT p;
	rc.screen_to_ppnt(info->current_xy, &p);

	PMGPageObjectPtr this_object = m_Txp.Object();
//	current_object = this_object;
	BOOL fOverFrame = this_object->pt_in_object(p, &rc, NULL);

	if (fOverFrame)
	{
//		set_cursor(::LoadCursor(NULL, IDC_IBEAM));
		set_cursor(this_object->GetContentCursor());
	}

#if 0
	if ((info->flags & MK_LBUTTON)
			&& info->event != EVENT_BUTTON_RISING
			&& info->event != EVENT_BUTTON_DBLCLK)
	{
		// Button is down from some other click. Ignore this.
		edit_state = STATE_RELEASE;
		return TRUE;
	}
#endif

	if (info->event == EVENT_KEY_FALLING)
	{
		// Ignore these.
		return -1;
	}

	if (check_escape_event(info))
	{
		set_arrow_tool();
		return TRUE;
	}

	switch (info->event)
	{
		case EVENT_EDIT_DELETE:
		{
		/* Simulate a DEL key pressed (for modularity's sake) */
			new_info.event = EVENT_KEY_DOWN;
			new_info.v_key = VK_DELETE;
			new_info.repeat_count = 1;

			info = &new_info;

		/* Fall through to... */
		}
		case EVENT_KEY_RISING:
		case EVENT_KEY_DOWN:
		case EVENT_KEY_FALLING:
		case EVENT_KEY:
		{
		/* Handle keys elsewhere. */
			text_edit_keystroke(info);
			return TRUE;				// Handled
		}
		default:
		{
			break;
		}
	}

/*
// See if we're over our frame.
*/

	if (fOverFrame)
	{
		if (info->flags & MK_LBUTTON)
		{
			if (info->event == EVENT_BUTTON_DBLCLK)
			{
			/* Double click! Let everybody know! */

				m_Txp.SelectAll();
				edit_state = STATE_RELEASE;
			}
			else
			{
				SetFocus();
				m_Txp.MoveToPoint(p,
										SHIFT_KEY(info->flags)
												? CTxp::END_Unknown
												: CTxp::END_NoSelection,
										TRUE);
				
				select_start = p;
				edit_state = TEXT_EDIT_STATE_DRAGGING;
				TxpCursorOn();
			}
		}
		else if (info->event == EVENT_RBUTTON_RISING)
		{
		/* Do our pop-up menu. */
			process_right_button(info->current_xy);
		}

		reframe_on_txp();
		return TRUE;			/* Handled */
	}

/* Outside the text frame. */

	if (info->event == EVENT_RBUTTON_RISING)
	{
		set_arrow_tool(FALSE, FALSE);
		return TRUE;
	}

	if (info->flags & MK_LBUTTON)
	{
		// Save the info around this since we want to chain to mode_resize.
		EVENT_INFO InfoSave = *info;
		set_arrow_tool(FALSE, FALSE);
		*info = InfoSave;
	}

	return FALSE;				/* Not handled */
}

/*
// Initialization state.
*/

BOOL CPmwView::text_edit_state_init(EVENT_INFO *info)
{
	set_cursor(::LoadCursor(NULL, IDC_ARROW));

	edit_state = STATE_NORMAL;

	return text_edit_state_normal(info);				/* Do it now, too. */
}

#define TEXT_SCROLL_DELAY	2

/*
// The "dragging" state of text entry.
*/

BOOL CPmwView::text_edit_state_dragging(EVENT_INFO *info)
{
	CFrameObject* pEditObject = m_Txp.Object();

	if (!(info->flags & MK_LBUTTON))
	{
	/* Done dragging! */
		edit_state = STATE_NORMAL;
	}
	else
	{
		PPNT p;

		static ULONG last_scroll = 0L;
		ULONG now = GetTickCount();

		rc.screen_to_ppnt(info->current_xy, &p);

		// if object is a cell, and the cursor leaves the cell boundary,
		// switch to drag/select mode
		if (pEditObject->get_panel() == OBJECT_IN_CELL_PANEL)
		{
			if (current_object != NULL && current_object->type() == OBJECT_TYPE_Table)
			{
				// The current object is correctly set.
				PBOX cursorbox;
				PBOX cellbox = pEditObject->get_bound();
				rc.mouse_box(info->current_xy, &cursorbox);
				if (!IsBoxOnBox(&cellbox, &cursorbox))
				{
					// Switch to the arrow tool so we can select cells.
					set_arrow_tool();
					edit_state = RESIZE_STATE_SELECT_CELLS;
					return TRUE;
				}
			}
			else
			{
				ASSERT(FALSE);
			}
		}


		if (p.x > rc.source_pbox.x1 && rc.source_pbox.x1 < view_extent.x1)
		{
			if (now > last_scroll+TEXT_SCROLL_DELAY)
			{
				CHARACTER_INDEX last = m_Txp.LastCharacterInLine();

				last_scroll = now;

				if (m_Txp.Index() < last)
				{
					PBOX pbox;
					PCOORD dx;

					m_Txp.Right(TRUE);
					m_Txp.CalcCursor();

					dx = m_Txp.CursorBox().x1 - rc.source_pbox.x1;

					pbox.y0 = rc.source_pbox.y0;
					pbox.y1 = rc.source_pbox.y1;
					pbox.x0 = rc.source_pbox.x0 + dx;
					pbox.x1 = rc.source_pbox.x1 + dx;
					reframe_view(pbox);
				}
			}
		}
		else if (p.x < rc.source_pbox.x0 && rc.source_pbox.x0 > view_extent.x0)
		{
			if (now > last_scroll+TEXT_SCROLL_DELAY)
			{
				CHARACTER_INDEX first = m_Txp.FirstCharacterInLine();

				last_scroll = now;

				if (m_Txp.Index() > first)
				{
					PBOX pbox;
					PCOORD dx;

					m_Txp.Left(TRUE);
					m_Txp.CalcCursor();

					dx = m_Txp.CursorBox().x0 - rc.source_pbox.x0;

					pbox.y0 = rc.source_pbox.y0;
					pbox.y1 = rc.source_pbox.y1;
					pbox.x0 = rc.source_pbox.x0 + dx;
					pbox.x1 = rc.source_pbox.x1 + dx;
					reframe_view(pbox);
				}
			}
		}
		else if (info->new_position)
		{
			m_Txp.MoveToPoint(p, CTxp::END_Current, TRUE);
			TxpCursorOn();
		}
	}
	return TRUE;
}

/*
// The states for the text mode.
*/

CPmwView::STATE_HANDLER CPmwView::text_edit_state[] =
{
	CPmwView::text_edit_state_init,
	CPmwView::text_edit_state_normal,
	CPmwView::standard_state_release,

	CPmwView::text_edit_state_dragging
};

#define TEXT_EDIT_STATE_COUNT		(sizeof(text_edit_state)/sizeof(text_edit_state[0]))

/*
// The text mode handler.
*/

void CPmwView::mode_text_edit(EVENT_INFO *info)
{
	CPmwDoc* pDoc = GetDocument();

	if (info->event >= EVENT_ENTERING_MODE)
	{
		if (info->event != EVENT_ENTERING_MODE)
		{
			if (m_Txp.Valid())
			{
				CFrameObject* pObject = m_Txp.Object();

				stop_editing_frame();

				if (info->event == EVENT_REENTERING_MODE)
				{
					if (pObject->get_panel() >= TEXT_PANEL)
					{
						CalendarObjectPtr calobj;
						// This was a calendar enhanced date frame. Reselect the date.
						if ((calobj = pDoc->get_current_calendar(TRUE)) != NULL)
						{
							select_object(calobj->get_date_object());
						}
					}
				}
			}
		}

		return;
	}

	// If we do not have a valid txp, we have nothing.
	if (!m_Txp.Valid())
	{
		set_arrow_tool();
		return;
	}

	// Process the normal state.
	SHORT state = edit_state;

	if (state > STATE_BASE_END)
	{
		if (state >= TEXT_EDIT_STATE_BASE)
		{
		/* One of our special states! */
			state -= (TEXT_EDIT_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = TEXT_EDIT_STATE_COUNT;
		}
	}

/* See if we need to handle the state. */

	if (state < TEXT_EDIT_STATE_COUNT)
	{
		BOOL ret;				// "BOOL"
	/* Valid state, process. */
		if (ret = (this->*text_edit_state[state])(info))
		{
		/* Handled! */
			if (ret != -1)
			{
				TxpOn();
			}
			return;
		}
	}

	/* Make sure we're on. */

	TxpOn();

	mode_resize(info);
}

void CPmwView::reframe_on_txp(VOID)
{
	if (m_Txp.Valid())
	{
		m_Txp.CalcFrame();
		if (!(m_Txp.Visible() & CTxp::TOGGLE_Cursor))
		{
		/* Make sure the cursor box is calculated if not on. */

			m_Txp.CalcCursor();
		}

	/* Get the bounds for the txp cursor and make sure we're there. */

		if (m_Txp.CursorFrame() != m_Txp.FrameRecord())
		{
			ASSERT(GetDocument()->selected_objects() == m_Txp.Object());

			if (!SetTxpFrame(m_Txp.CursorFrame()))
			{
				SetTxpFrame(m_Txp.FrameRecord());
			}

			ASSERT(GetDocument()->selected_objects() == m_Txp.Object());
		}

		PBOX pbox = m_Txp.CursorBox();
		ASSERT(m_Txp.Valid());		// It changed once!
		if (m_Txp.Object != NULL)
		{
			m_Txp.Object()->GetWarpedFlippedAndRotatedBoundingBox(&pbox);
		}

		if (memcmp(&text_cursor_pbox, &pbox, sizeof(PBOX)) != 0)
		{
		/* Cursor has changed position! */
			text_cursor_pbox = pbox;
			reframe_view(pbox);
		}
	}
}

BOOL CPmwView::SetTxpFrame(DB_RECORD_NUMBER lFrame, CHARACTER_INDEX lCharacter /*=-1*/)
{
	// We need to change frames.
	CPmwDoc* pDoc = GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();
	DB_RECORD_NUMBER lCurrentPage = pDatabase->CurrentPage();

	pDoc->deselect_all(FALSE);
	CFrameObject* pNewObject = pDoc->get_database()->FindFrameObject(lFrame);
	if (pNewObject == NULL)
	{
		return FALSE;
	}

	if (lCurrentPage != pDatabase->CurrentPage())
	{
		pDoc->LoadPage(pDatabase->CurrentPage(), !m_Txp.Valid());
	}

	SetPanel(pNewObject->get_panel(), FALSE);

	if (m_Txp.Valid())
	{
		free_edit_background();

		int nVisible = m_Txp.Visible();
		m_Txp.Off(nVisible);
		m_Txp.InvalidateSelectBounds();

		m_Txp.Object()->SetEditing(FALSE);
		m_Txp.NewFrame(pNewObject);
		m_Txp.Object()->SetEditing(TRUE);
		
		if (lCharacter != -1)
		{
			m_Txp.Index(lCharacter);
			m_Txp.SetTransition();
		}
		m_Txp.On(nVisible);
	}

	pDoc->select_object(pNewObject, !m_Txp.Valid());
	return TRUE;
}

/********************************/
/* Initial text frame creation. */
/********************************/

/*
// Create a dummy frame.
*/

ERRORCODE CPmwView::create_initial_frame()
{
	FRAMEOBJ_CREATE_STRUCT fcs;
	CPmwDoc *pDoc = GetDocument();
	PMGDatabasePtr database = pDoc->get_database();

/*
// Construct the default frame bound.
// Place the chosen frame in the center of the world.
*/

	PBOX world;

	GetDocument()->get_panel_world(&world);

	IntersectBox(&world, &world, &rc.source_pbox);

	fcs.bound.x1 = ((world.x1-world.x0)*2)/3;
	fcs.bound.y1 = ((world.y1-world.y0)*1)/3;

	fcs.bound.x0 = (world.x0 + world.x1 - fcs.bound.x1)/2;
	fcs.bound.y0 = (world.y0 + world.y1 - fcs.bound.y1)/2;
	fcs.bound.x1 += fcs.bound.x0;
	fcs.bound.y1 += fcs.bound.y0;

/*
// Set the rest of the creation arguments.
*/

//	fcs.text = "";
	fcs.alignment = ALIGN_left;
	fcs.vert_alignment = ALIGN_top;
	fcs.flags = 0;
	// Set the creation style to the last used.
//	fcs.pStyle = &m_Txp.get_style();
//	if (fcs.pStyle->get_face() == 0)
	{
	// None used yet.
		fcs.pStyle = NULL;
	}

	CFrameObject* object;

	if ((object = database->create_frame_object(&fcs)) == NULL)
	{
		return database->last_creation_error();
	}

/* Set its panel. */

	object->set_panel(pDoc->get_current_panel());

/* Add it to the list. */

	pDoc->append_object(object);

/* ...and make us show up. */

	// A frame has nothing to show on initial creation.
	pDoc->refresh_object(object);

/* Make it the only selected object. */

	set_arrow_tool();

	pDoc->deselect_all();
	pDoc->select_object(object);

	return ERRORCODE_None;
}

/************************************/
/* Command handlers in text editor. */
/************************************/

void CPmwView::OnTextEditDone()
{
	set_arrow_tool();
}

void CPmwView::realign_frame(ALIGN_TYPE align_type)
{
	if (m_Txp.Valid())
	{
		CPmwDoc* pDoc = GetDocument();

		SetFocus();

		// Create the undo/redo command.

		CCmdHAlignText* pCommand = new CCmdHAlignText(align_type);

		if (pCommand->Snapshot(this))
		{
		/* Add and execute the command. */
			pDoc->AddCommand(pCommand);
		}
		else
		{
			delete pCommand;
		}
	}
	else
	{
		m_Txp.Style().Alignment(align_type);
	}
}

void CPmwView::reposition_frame(VERT_ALIGN_TYPE align_type)
{
	if (m_Txp.Valid())
	{
		CPmwDoc* pDoc = GetDocument();

		SetFocus();

	/* Create the undo/redo command. */

		CCmdVAlignText* pCommand = new CCmdVAlignText(align_type);

		if (pCommand->Snapshot(this))
		{
		/* Add and execute the command. */
			pDoc->AddCommand(pCommand);
		}
		else
		{
			delete pCommand;
		}
	}
	else
	{
		m_Txp.FrameAttributes().m_nVerticalAlignment = align_type;
	}
}

void CPmwView::GetStretchState(BOOL& fEnable, int& nCheck, int& nType)
{
	// The type:
	// 0 = nothing selected
	// 1 = invalid object selected
	// 2 = stretchable frame(s) selected
	nType = 0;

	// The check status.
	// 0 = unchecked
	// 1 = checked
	// 2 = indeterminate
	nCheck = 0;
	fEnable = FALSE;

	// See if we have a text frame we can apply stretching to. 
	CPmwDoc* pDoc = GetDocument();
	if (m_Txp.Valid())
	{
		if (m_Txp.Object()->CanStretch())
		{
			nType = 2;
			fEnable = TRUE;
			nCheck = m_Txp.Object()->IsStretched() ? 1 : 0;
		}
		else
		{
			nType = 1;
			fEnable = FALSE;
			nCheck = 0;
		}
	}
	else
	{
		// No insertion point.
		// See if we have any objects that are stretchable.

		PMGPageObject* pObject;
		for (pObject = pDoc->selected_objects();
			  pObject != NULL;
			  pObject = pObject->next_selected_object())
		{
			if (pObject->type() == OBJECT_TYPE_Frame)
			{
				CFrameObject* pFrame = (CFrameObject*)pObject;
				// We have a frame.
				if (pFrame->CanStretch())
				{
					int nThisCheck = pFrame->IsStretched() ? 1 : 0;
					// We have a stretchable frame.
					if (nType != 2)
					{
						// First stretchable frame 
						nCheck = nThisCheck;
					}
					else
					{
						// Already had a stretchable frame.
						if (nCheck != nThisCheck)
						{
							// States do not match. Indeterminate.
							nCheck = 2;
						}
					}
					// Remember we have a stretchable frame.
					nType = 2;
				}
				else
				{
					// This is a frame that cannot be stretched.
					// If this is the first one, state we have an invalid object
					// (a valid one will switch it later).
					if (nType == 0)
					{
						nType = 1;
					}
				}
			}
#if 0
			else
			{
				if (nType == 0)
				{
					nType = 1;		// Invalid object selected.
				}
			}
#endif
		}
		switch (nType)
		{
			case 0:
			{
				// No object selected. Set the default.
				fEnable = TRUE;
				nCheck = m_fDefaultStretching;
				break;
			}
			case 1:
			{
				// No stretchable frame selected (but something is).
				fEnable = FALSE;
				nCheck = FALSE;
				break;
			}
			case 2:
			{
				// Stretchable frames selected. nCheck is already set.
				fEnable = TRUE;
				break;
			}
		}
	}
}

void CPmwView::OnTextStretching()
{
	BOOL fEnable;
	int nCheck;
	int nType;

	GetStretchState(fEnable, nCheck, nType);

	switch (nType)
	{
		case 0:
		{
			// Nothing selected.
			// Set the default state.
			m_fDefaultStretching = !m_fDefaultStretching;
			break;
		}
		case 1:
		{
			// No valid frame selected.
			// Should not get here.
			break;
		}
		case 2:
		{
			// Valid frame selected.
			SetFocus();

			CCmdStretchText* pCommand = new CCmdStretchText;
			CPmwDoc* pDoc = GetDocument();

			if (pCommand->Snapshot(this))
			{
				// Add the command and execute it.
				pDoc->AddCommand(pCommand);
			}
			else
			{
				delete pCommand;
			}
		}
	}
}

void CPmwView::OnUpdateTextStretching(CCmdUI* pCmdUI)
{
	BOOL fEnable;
	int nCheck;
	int nType;

	// Get the stretch state.
   GetStretchState(fEnable, nCheck, nType);

   // Set the status.
	pCmdUI->Enable(fEnable);
	pCmdUI->SetCheck(nCheck);
}

void CPmwView::update_horz_align(CCmdUI* pCmdUI, ALIGN_TYPE type)
{
	if (pCmdUI->m_pSubMenu != NULL)
	{
	/* Handle the menu. */
		CMenu *save = pCmdUI->m_pSubMenu;
		pCmdUI->m_pSubMenu = NULL;
		pCmdUI->Enable(TRUE);
		pCmdUI->m_pSubMenu = save;
	}
	else
	{
	/* Handle an item only. */
		pCmdUI->Enable(TRUE);
		CTextStyle& Style = GetCurrentTextStyle();
		pCmdUI->SetCheck(Style.Alignment() == type);
	}
}

void CPmwView::update_vert_align(CCmdUI* pCmdUI, VERT_ALIGN_TYPE type)
{
	if (pCmdUI->m_pSubMenu != NULL)
	{
	/* Handle the menu. */
		CMenu *save = pCmdUI->m_pSubMenu;
		pCmdUI->m_pSubMenu = NULL;
		pCmdUI->Enable(TRUE);
		pCmdUI->m_pSubMenu = save;
	}
	else
	{
	/* Handle an item only. */
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(m_Txp.FrameAttributes().m_nVerticalAlignment == type);
	}
}

void CPmwView::OnAlignBottom()
{
	reposition_frame(ALIGN_bottom);
}

void CPmwView::OnUpdateAlignBottom(CCmdUI* pCmdUI)
{
	update_vert_align(pCmdUI, ALIGN_bottom);
}

void CPmwView::OnAlignCenter()
{
	realign_frame(ALIGN_center);
}

void CPmwView::OnUpdateAlignCenter(CCmdUI* pCmdUI)
{
	update_horz_align(pCmdUI, ALIGN_center);
}

void CPmwView::OnAlignLeft()
{
	realign_frame(ALIGN_left);
}

void CPmwView::OnUpdateAlignLeft(CCmdUI* pCmdUI)
{
	update_horz_align(pCmdUI, ALIGN_left);
}

void CPmwView::OnAlignMiddle()
{
	reposition_frame(ALIGN_middle);
}

void CPmwView::OnUpdateAlignMiddle(CCmdUI* pCmdUI)
{
	update_vert_align(pCmdUI, ALIGN_middle);
}

void CPmwView::OnAlignRight()
{
	realign_frame(ALIGN_right);
}

void CPmwView::OnUpdateAlignRight(CCmdUI* pCmdUI)
{
	update_horz_align(pCmdUI, ALIGN_right);
}

void CPmwView::OnAlignJustify()
{
	realign_frame(ALIGN_justify_left);
}

void CPmwView::OnUpdateAlignJustify(CCmdUI* pCmdUI)
{
	update_horz_align(pCmdUI, ALIGN_justify_left);
}

void CPmwView::OnAlignTop()
{
	reposition_frame(ALIGN_top);
}

void CPmwView::OnUpdateAlignTop(CCmdUI* pCmdUI)
{
	update_vert_align(pCmdUI, ALIGN_top);
}

void CPmwView::OnTextChoosefont()
{
	// Put up the dialog.
	CPmwDoc *pDoc = GetDocument();

	CTextStyle Style = GetCurrentTextStyle();

	if (pDoc->ChooseFont(Style) == IDOK)
	{
		ChangeFont(&Style, IDCmd_ChangeFont);
	}
}

void CPmwView::OnUpdateTextChoosefont(CCmdUI* pCmdUI)
{
#if 0
	BOOL fEnable = FALSE;
	CPmwDoc* pDoc = GetDocument();
	PMGPageObject* pObject = pDoc->selected_objects();
	if (m_Txp.Valid()
			|| (pObject != NULL
					&& pObject->type() == OBJECT_TYPE_Table
					&& ((CTableObject*)pObject)->SomethingSelected()))
	{
		fEnable = TRUE;
	}
	pCmdUI->Enable(fEnable);
#else
	pCmdUI->Enable(TRUE);
#endif
}

void CPmwView::OnTextChoosecolor() 
{
	ASSERT(FALSE);
#if 0
/* Shouldn't get here, but just in case... */

	if (!m_Txp.Valid())
	{
		return;
	}

	// Put up the dialog.

	CPmwDoc *pDoc = GetDocument();

	CTextStyle Style = m_Txp.SelectStyle();

	COLOR Color = Style.Color();
	if (pDoc->choose_color(Color) == IDOK
		 && Color != UNDEFINED_COLOR)
	{
		Style.Color(Color);
		ChangeFont(&Style, IDCmd_ChangeFontColor);
	}
#endif
}

void CPmwView::ChangeFont(CTextStyle* pStyle, WORD wCmdID)
{
	CPmwDoc* pDoc = GetDocument();
	PMGPageObject* pObject = pDoc->selected_objects();
	CTextRecord* pText = NULL;

	TRY
	{
	/* Make the style change. */

		if (m_Txp.Valid() && (m_Txp.SomethingSelected() /*|| m_Txp.LastOffsetInText() == 0*/))
		{
			CTextCommand* pCommand = NULL;

			if (m_Txp.SomethingSelected())
			{
				pCommand = new CTextCommand(wCmdID);
				if (!pCommand->Before(this))
				{
					delete pCommand;
					pCommand = NULL;
				}
			}

		/* Change whole range. */

			CHARACTER_INDEX lStart, lEnd;

			m_Txp.GetRange(&lStart, &lEnd);

			m_Txp.Off();

			pText = m_Txp.LockText();
			ASSERT(pText != NULL);

			if (pText->ChangeRangeStyle(lStart, lEnd, pStyle) > 0)
			{
			/* Finish by showing the changed style. */
				m_Txp.UpdateRange();
			}
			pText->release();
			pText = NULL;

		/* Finish the undo/redo command, if there is one. */

			if (pCommand != NULL)
			{
				if (pCommand->After())
				{
					pDoc->AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
				}
			}

			m_Txp.SetTransition();
			m_Txp.On();
			pDoc->SetModifiedFlag();
			if (m_Txp.Flags() & CTxp::FLAG_stretch_frame
				 || m_Txp.Object()->get_panel() == OBJECT_IN_CELL_PANEL)
			{
				pDoc->resize_project();
			}
		}
		else if (pObject != NULL
					&& pObject->type() == OBJECT_TYPE_Table
					&& ((CTableObject*)pObject)->SomethingSelected())
		{
			CTableObject* pTable = (CTableObject*)pObject;
			CTableTextCommand* pCommand = new CTableTextCommand(wCmdID);
			if (pCommand->Before(this, pTable))
			{
				// Change the font.
				pTable->ChangeSelectFont(*pStyle);
				pTable->InvalidateSelection();
				pTable->ComputeSelectTextStyle();

				// Remember after.
				pCommand->After();

				// Add the undo command.
				pDoc->AddCommand(pCommand, FALSE);
				pDoc->SetModifiedFlag();
			}
			else
			{
				delete pCommand;
			}
		}
		else
		{
			// Just set the txp style.
			m_Txp.Style() = *pStyle;
			// Update the font metrics. This is especially good in the case of
			// a face change since it pulls in the needed font from CD now.
			m_Txp.Style().UpdateFontMetrics();
//			m_Txp.SetTransition();
		}
	}
	END_TRY

	if (pText != NULL)
	{
		pText->release();
	}
}

void CPmwView::OnEditingText(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Txp.Valid());
}

/*
// Update the edit background for the current frame.
*/

void CPmwView::UpdateEditBackground(void)
{
	// If we have an edit background, make sure it is current.
	CFrameObject* pObject = m_Txp.Object();
	if (pObject != NULL && m_edit_background != NULL)
	{
		PBOX rb;

	// Get the bound. We can not use the refresh bound because it changes.
		rb = pObject->get_bound();

	// Convert to a screen bound so we can create a bitmap.

		rc.pbox_to_screen(&rb, TRUE);

		RECT r;
		rc.convert_pbox(&rb, &r, NULL);

		if (!EqualRect(&m_rEditBackground, &r))
		{
			free_edit_background();
		}
	}

	// If we do not have an edit background, make it now.
	if (m_edit_background == NULL)
	{
		// Hopefully this will be snappy.
		create_edit_background();
	}
}

/*
// Create the edit background for this frame.
*/

void CPmwView::create_edit_background(void)
{
	CFrameObject* object = m_Txp.Object();
	CPmwDoc *pDoc = GetDocument();

/*
// Check for the case(s) where we don't want to make an edit background.
*/

	// Special things like calendar date text boxes, etc. have special
	// panel numbers which are greater than the number of panels defined
	// by the document.

	if (object == NULL || object->get_panel() >= pDoc->number_of_panels())
	{
	/* Don't even bother for special frames. */
		return;
	}

/*
// Well, here goes...
// Attempt to create a bitmap to hold the frame's background.
*/

	PBOX rb;
	rb = object->get_bound();
	rc.pbox_to_screen(&rb, TRUE);
	RECT r;
	rc.convert_pbox(&rb, &r, NULL);
	m_rEditBackground = r;

// Get the refresh bound. This should be everything possible for the frame.
// (The special case where an RC is passed.)
	REFRESH_EXTENT re;

	object->get_refresh_bound(&rb, &rc);
	re.extent = rb;

// Convert to a screen bound so we can create a bitmap.

	rc.pbox_to_screen(&rb, TRUE);
	rc.convert_pbox(&rb, &r, NULL);

	int width = r.right - r.left;
	int height = r.bottom - r.top;

/* Update all the objects into the DC. */

	HWND hWnd = GetSafeHwnd();
	HDC hdc;

	if ((hdc = ::GetDC(hWnd)) != NULL)
	{
	/* Create the bitmap. */
		if ((m_edit_background = ::CreateCompatibleBitmap(hdc, width, height)) != NULL)
		{
		/* Initialize the bitmap. */
			HDC hdcBack;
			BOOL cleared = FALSE;

			if ((hdcBack = ::CreateCompatibleDC(hdc)) != NULL)
			{
				HBITMAP hOldBitmap;

				if ((hOldBitmap = (HBITMAP)::SelectObject(hdcBack, m_edit_background)) != NULL)
				{
					::PatBlt(hdcBack, 0, 0, width, height, WHITENESS);

				/* Create a redisplay context to draw into. */

					RedisplayContext orc;

					orc.destination_hdc = hdcBack;
					orc.hwnd = NULL;

					orc.x_resolution = rc.x_resolution;
					orc.y_resolution = rc.y_resolution;

					orc.SetScaling(rc.GetScaleNumerator(), rc.GetScaleDenominator());

					orc.destination_rect = rc.destination_rect;

					orc.clip_rect = orc.destination_rect;
					OffsetRect(&(orc.clip_rect), -r.left, -r.top);

					orc.source_pbox = rc.source_pbox;

					orc.scaled_source_x0 = rc.scaled_source_x0;
					orc.scaled_source_y0 = rc.scaled_source_y0;

					orc.destination_x0 = (short)(rc.destination_x0 - r.left);
					orc.destination_y0 = (short)(rc.destination_y0 - r.top);

					orc.outline_gamma_curve = rc.outline_gamma_curve;
					orc.bitmap_gamma_curve = rc.bitmap_gamma_curve;

					orc.info = rc.info;

					orc.is_thumbnail = TRUE;
					orc.using_images = TRUE;		/* Use images if possible. */

				/* Finish building the refresh extent. */

					re.refresh_type = REFRESH_ALL;
					re.my_object = NULL;
					re.update_object = NULL;
					re.update_state = NULL;

					HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
					HPALETTE hOldPal = SelectPalette(hdcBack, hOurPal, FALSE);

					BeginWaitCursor();

					m_pDesktop->DrawBackground(&orc.clip_rect, &orc);

					orc.m_pUptoObject = object;
					pDoc->draw_objects(&re, &orc);
					EndWaitCursor();

					SelectPalette(hdcBack, hOldPal, FALSE);

					::SelectObject(hdcBack, hOldBitmap);
					cleared = TRUE;
				}
				::DeleteDC(hdcBack);
			}

		/* Set the edit background. */

			if (cleared)
			{
				object->SetEditBackground(m_edit_background, r);
			}
			else
			{
			/* Failed. */
				::DeleteObject(m_edit_background);
				m_edit_background = NULL;
			}
		}
		::ReleaseDC(hWnd, hdc);
	}
}

/*
// Free the edit background if it has been created.
*/

VOID CPmwView::free_edit_background(VOID)
{
	if (m_edit_background != NULL)
	{
		::DeleteObject(m_edit_background);
		m_edit_background = NULL;
	}

	CFrameObject* pObject;
	if (m_Txp.Valid() && (pObject = m_Txp.Object()) != NULL)
	{
		pObject->ClearEditBackground();
	}
}

BOOL CPmwView::UpdateUndoTyping(CTxp* pTxp, CHARACTER c, int nCount)
{
/* Get the document. */

	CPmwDoc* pDoc = GetDocument();

/* See if there is an existing typing command that we can extend. */

	CCmdAddText* pCommand = (CCmdAddText*)pDoc->UndoCommand();

	if (pCommand != NULL
		 && pCommand->ID() == IDCmd_TypeText
		 && pCommand->UpdateTyping(this, c, nCount))
	{
		return TRUE;
	}

/* Doesn't exist yet. Create one. */

	pCommand = new CCmdAddText;

/* Save state before the typing begins. */

	if (!pCommand->Before(this))
	{
		delete pCommand;
		return FALSE;
	}

/* Add this typing into the undo command. */

	pCommand->UpdateTyping(this, c, nCount);

/* Add the undo/redo command. */

	pDoc->AddCommand(pCommand, FALSE);

	return TRUE;
}

BOOL CPmwView::UpdateUndoDelete(CTxp* pTxp, int nCount)
{
/* Get the document. */

	CPmwDoc* pDoc = GetDocument();

/* Fixup the txp to have a selection. */

	if (!pTxp->SomethingSelected())
	{
		pTxp->SelectText(pTxp->Index(), pTxp->Index()+nCount);
		nCount = 1;
		if (!pTxp->SomethingSelected())
		{
		/* Nothing to do. */
			return FALSE;
		}
	}

/* See if there is an existing delete command that we can extend. */

	CCmdDeleteText* pCommand = (CCmdDeleteText*)pDoc->UndoCommand();

	if (pCommand != NULL
		 && pCommand->ID() == IDCmd_DeleteText
		 && pCommand->UpdateDelete(this, nCount))
	{
		return TRUE;
	}

/* Doesn't exist yet. Create one. */

	pCommand = new CCmdDeleteText;
	pCommand->SetStuff(this);

/* Add this backspace into the undo command. */

	pCommand->UpdateDelete(this, nCount);

/* Add the undo/redo command. */

	pDoc->AddCommand(pCommand, FALSE);

	return TRUE;
}

BOOL CPmwView::UpdateUndoBackspace(CTxp* pTxp, int& nCount)
{
/* Get the document. */

	CPmwDoc* pDoc = GetDocument();

/* Fixup the txp to have a selection. */

	if (!pTxp->SomethingSelected())
	{
      CHARACTER_INDEX lStart = pTxp->Index()-nCount;
      if (lStart < 0)
         lStart = 0;
		pTxp->SelectText(lStart, pTxp->Index());
		if (!pTxp->SomethingSelected())
		{
		/* Nothing to do. */
			return FALSE;
		}
	}

   // Always do just one!
	nCount = 1;

   // See if there is an existing backspace command that we can extend.

	CCmdBackspace* pCommand = (CCmdBackspace*)pDoc->UndoCommand();

	if (pCommand != NULL
		 && pCommand->ID() == IDCmd_Backspace
		 && pCommand->UpdateBackspace(this, nCount))
	{
		return TRUE;
	}

/* Doesn't exist yet. Create one. */

	pCommand = new CCmdBackspace;
	pCommand->SetStuff(this);

/* Add this backspace into the undo command. */

	pCommand->UpdateBackspace(this, nCount);

/* Add the undo/redo command. */

	pDoc->AddCommand(pCommand, FALSE);

	return TRUE;
}

CTextStyle& CPmwView::GetCurrentTextStyle(void)
{
	CPmwDoc* pDoc = GetDocument();
	PMGPageObject* pObject = pDoc->selected_objects();

	if (m_Txp.Valid() && m_Txp.SomethingSelected())
	{
		return m_Txp.SelectStyle();
	}
	else if (pObject != NULL
					&& pObject->type() == OBJECT_TYPE_Table
					&& ((CTableObject*)pObject)->SomethingSelected())
	{
		return ((CTableObject*)pObject)->GetSelectStyle();
	}
	else
	{
		// Default style.
		return m_Txp.Style();
	}
}

void CPmwView::OnTextBold()
{
	// We need to do something.
	CTextStyle NewStyle = GetCurrentTextStyle();
	int nNewBold = (NewStyle.Bold() == 0);
	NewStyle.Bold(nNewBold);
	ChangeFont(&NewStyle, IDCmd_Bold);
}

void CPmwView::OnUpdateTextBold(CCmdUI* pCmdUI)
{
	int nStyle = FALSE;
	CTextStyle& Style = GetCurrentTextStyle();
	nStyle = Style.Bold();

	pCmdUI->SetCheck(nStyle == -1 ? 2 : nStyle);
	pCmdUI->Enable(TRUE);
}

void CPmwView::OnTextItalic()
{
	CTextStyle NewStyle = GetCurrentTextStyle();
	int nNewItalic = (NewStyle.Italic() == 0);
	NewStyle.Italic(nNewItalic);
	ChangeFont(&NewStyle, IDCmd_Italic);
}

void CPmwView::OnUpdateTextItalic(CCmdUI* pCmdUI)
{
	int nStyle = FALSE;
	CTextStyle& Style = GetCurrentTextStyle();
	nStyle = Style.Italic();

	pCmdUI->SetCheck(nStyle == -1 ? 2 : nStyle);
	pCmdUI->Enable(TRUE);
}

void CPmwView::OnTextUnderline()
{
	CTextStyle NewStyle = GetCurrentTextStyle();
	UNDERLINE_STYLE nNewUnderline = (NewStyle.Underline() == UNDERLINE_STYLE_None)
												? UNDERLINE_STYLE_Single
												: UNDERLINE_STYLE_None;
	NewStyle.Underline(nNewUnderline);
	ChangeFont(&NewStyle, IDCmd_Underline);
}

void CPmwView::OnUpdateTextUnderline(CCmdUI* pCmdUI)
{
	int nStyle = UNDERLINE_STYLE_None;

	CTextStyle& Style = GetCurrentTextStyle();
	nStyle = Style.Underline();

	switch (nStyle)
	{
		case UNDERLINE_STYLE_Single:
		{
			nStyle = 1;
			break;
		}
		case UNDERLINE_STYLE_Undefined:
		{
			nStyle = 2;
			break;
		}
		case UNDERLINE_STYLE_None:
		default:
		{
			nStyle = 0;
			break;
		}
	}
	pCmdUI->SetCheck(nStyle);
	pCmdUI->Enable(TRUE);
}

void CPmwView::OnTextTypeface()
{
	CToolBar* pFormatBar = ((CMainFrame*)AfxGetMainWnd())->GetToolBar(TOOLBAR_FORMAT);

	CString csTypeface;

	pFormatBar->SendMessage(UM_GET_FONT,0, (LPARAM)&csTypeface);

	// Find the face in the typeface list.

	int nFace = typeface_server.find_face(csTypeface, FALSE);

	if (nFace != -1)
	{
		PMGFontServer* pFontServer = (PMGFontServer*)GetDocument()->get_font_server();

		DB_RECORD_NUMBER Face = pFontServer->font_face_to_record(nFace);
		if (Face > 0)
		{
			// We need to do something.
			CTextStyle NewStyle = GetCurrentTextStyle();
			NewStyle.Font(Face);
			ChangeFont(&NewStyle, IDCmd_TextTypeface);
		}
	}

	// Update the font MRU
	pFormatBar->SendMessage(UM_UPDATE_FONT_MRU,0, 0);
}

void CPmwView::OnUpdateTextTypeface(CCmdUI* pCmdUI)
{
	char buffer[80] = "";

	/*
	// Only change the text of the combo box if it doesn't have the focus.
	*/

	if (GetFocus() == this)
	{
		CTextStyle& Style = GetCurrentTextStyle();
		DB_RECORD_NUMBER Face = Style.Font();

		if (Face > 0)
		{
			BOOL fSetText = FALSE;

			// Get the face name to put in the buffer.

			PMGFontServer* pFontServer = (PMGFontServer*)GetDocument()->get_font_server();

			int nFace = pFontServer->font_record_to_face(Face);
			if (nFace != -1)
			{
				FaceEntry* pEntry = typeface_server.get_face(nFace);
				if (pEntry != NULL)
				{
					strcpy(buffer, pEntry->get_name());
					fSetText = TRUE;
				}
			}

			if (!fSetText)
			{
				// JRO strcpy(buffer, "*Error*");
				strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_Error));
			}
		}

		pCmdUI->SetText(buffer);
	}

	pCmdUI->Enable(TRUE);
}

void CPmwView::OnTextSizeKillFocus()
{
//	od("OnTextSizeKillFocus\r\n");
	CToolBar* pFormatBar = ((CMainFrame*)AfxGetMainWnd())->GetToolBar(TOOLBAR_FORMAT);
	int nSize = pFormatBar->SendMessage(UM_GET_FONT_SIZE, 0, 0);

	if (nSize != 0)
	{
		if (nSize < 4)
		{
			nSize = 4;
		}
		else if (nSize > 1000)
		{
			nSize = 1000;
		}
		ChangeTextSize(nSize, IDCmd_TextSize);
	}
}

void CPmwView::ChangeTextSize(int nNewSize, WORD wCmdID)
{
	CFixed lSize = m_Txp.SelectStyle().BaseSize();
	int nSize = FixedInteger(lSize);

	if (nNewSize != 0 && nNewSize != nSize)
	{
		// We need to do something.
		CTextStyle NewStyle = GetCurrentTextStyle();
		NewStyle.BaseSize(MakeFixed(nNewSize));
		ChangeFont(&NewStyle, wCmdID);
	}
}

void CPmwView::OnUpdateTextSize(CCmdUI* pCmdUI)
{
	char buffer[20] = "";

	/*
	// Only change the text of the combo box if it doesn't have the focus.
	*/
	if (GetFocus() == this)
	{
		CTextStyle& Style = GetCurrentTextStyle();
		CFixed lSize = Style.BaseSize();

		if (lSize != UNDEFINED_CFIXED)
		{
			int nSize = FixedInteger(lSize);
			itoa(nSize, buffer, 10);
		}

		pCmdUI->SetText(buffer);
	}

	pCmdUI->Enable(TRUE);
}

void CPmwView::OnFormatSpacing() 
{
	CCharacterSpacingDialog Dialog(m_Txp.Style(), this);

	if (Dialog.DoModal() == IDOK)
	{
		CFixed lSpacing = Dialog.GetSpacing();

		if (lSpacing != m_Txp.SelectStyle().Spacing())
		{
		/* We need to do something. */
			CTextStyle NewStyle = GetCurrentTextStyle();
			NewStyle.Spacing(lSpacing);
			ChangeFont(&NewStyle, IDCmd_CharacterSpacing);
		}
	}
}

void CPmwView::OnInsertField()
{
/*
// Create a helper based on this view.
*/

	CInsertFieldViewHelper Helper(this);

/*
// Invoke the dialog using this helper.
*/

	CInsertFieldDialog Dialog(&Helper);
	Dialog.DoModal();

	// Assume something changed.
	GetDocument()->GetMacroServer()->ModifiedMacros();

	SetFocus();
}

void CPmwView::OnInsertMailMergeField()
{
/*
// Create a helper based on this view.
*/

	CInsertFieldViewHelper Helper(this);

/*
// Invoke the dialog using this helper.
*/

	CInsertFieldDialog Dialog(&Helper, TRUE, IDD_MERGE_FIELD);
	Dialog.DoModal();

	// Assume something changed.
	GetDocument()->GetMacroServer()->ModifiedMacros();

	SetFocus();
}

void CPmwView::OnSelectMergeNames()
{
	if (GetConfiguration()->SupportsAddressBook())
	{
		// Assume something changed.
		GetDocument()->EditNameList(TRUE);
	}
}

void CPmwView::OnUpdateSelectMergeNames(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasDatabaseMacro());
}

/*
// Set the paragraph format.
*/

void CPmwView::OnFormatParagraph() 
{
	CPmwDoc* pDocument = GetDocument();
	if (m_Txp.Valid())
	{
		CParagraphFormatDialog Dialog(&m_Txp.SelectStyle());

		if (Dialog.DoModal() == IDOK)
		{
			CTextStyle Style(pDocument->get_database());
			Dialog.GetExitStyle(&Style);

			CCmdParagraphFormatting* pCommand = new CCmdParagraphFormatting;
			if (pCommand->Before(this))
			{
				pCommand->After(Style.m_Paragraph);
				pDocument->AddCommand(pCommand);
				pDocument->SetModifiedFlag();
			}
		}
	}
}

void CPmwView::OnFormatTabs() 
{
	CPmwDoc* pDocument = GetDocument();
	if (m_Txp.Valid())
	{
		// Get the range.
		PARAGRAPH_INDEX lStart, lEnd;
		m_Txp.GetParagraphRange(&lStart, &lEnd);

		CTabsDialog Dialog(&m_Txp.SelectStyle().m_Tabs, lStart != lEnd);

		if (Dialog.DoModal() == IDOK)
		{
			CCmdTabs* pCommand = new CCmdTabs;
			if (pCommand->Before(this) && pCommand->After(Dialog.GetTabs()))
			{
				pDocument->AddCommand(pCommand);
				pDocument->SetModifiedFlag();
			}
			else
			{
				delete pCommand;
			}
		}
	}
}

void CPmwView::OnFormatFrame() 
{
	CPmwDoc* pDoc = GetDocument();
	PMGPageObjectPtr object = m_Txp.Valid()
			? m_Txp.Object()
			: GetDocument()->selected_objects();

	if (object != NULL && object->type() == OBJECT_TYPE_Frame)
	{
		CFrameAttributes Attributes;
		BOOL fEditing = m_Txp.Valid();
		CFrameRecord* pFrame = NULL;

		if (fEditing)
		{
			Attributes = m_Txp.FrameAttributes();
		}
		else
		{
			PMGDatabase* pDatabase = object->get_database();
			DB_RECORD_NUMBER lFrame = ((CFrameObject*)object)->FrameRecord();
			pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
			if (pFrame == NULL)
			{
				return;
			}

			// Setup the entry attributes from the frame.
			pFrame->GetAttributes(&Attributes);
		}

		ASSERT(Attributes.m_fSet);

		// Run the dialog.
		CFrameAttributesDialog Dialog(&Attributes);

		// If the user said ok, set the new attributes.
		if (Dialog.DoModal() == IDOK)
		{
			// Get the attributes to set.
			Dialog.GetExitAttributes(&Attributes);

			CCmdFrameAttributes* pCommand = new CCmdFrameAttributes;
			if (pCommand->Before(this) && pCommand->After(Attributes))
			{
				pDoc->AddCommand(pCommand, TRUE);
				pDoc->SetModifiedFlag();
			}
			else
			{
				delete pCommand;
			}
		}

		// Final cleanup.
		if (pFrame != NULL)
		{
			pFrame->release();
		}
	}
}

void CPmwView::OnUpdateFormatFrame(CCmdUI* pCmdUI) 
{
	PMGPageObjectPtr object = m_Txp.Valid()
			? m_Txp.Object()
			: GetDocument()->selected_objects();

	pCmdUI->Enable(object != NULL										// Have an object
					&& object->next_selected_object() == NULL		// Single object
					&& object->type() == OBJECT_TYPE_Frame			// Is a frame
					&& (((CFrameObject*)object)->GenericFrame()	// Is generic
						 || object->get_panel() == OBJECT_IN_CELL_PANEL));	// Or in a table cell
}

/////////////////////////////////////////////////////////////////////////////
// Frame button support.

static void ShowButton(CWnd* pButton, BOOL fShow)
{
	if (fShow != pButton->IsWindowVisible())
	{
		pButton->ShowWindow(fShow ? SW_SHOW : SW_HIDE);
	}
}

static BOOL PositionButton(CWnd* pButton, const CRect& crNew, const CRect& crOld, const CRect& crVisible)
{
	if (crNew.top >= crVisible.bottom
			|| crNew.bottom <= crVisible.top
			|| crNew.left >= crVisible.right
			|| crNew.right <= crVisible.left)
	{
		// Not visible.
		return FALSE;
	}

	if (crNew != crOld)
	{
		pButton->SetWindowPos(NULL, crNew.left, crNew.top, crNew.Width(), crNew.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
	return TRUE;
}

void CPmwView::UpdateFrameButtons(void)
{
	// If a button is missing, jump out now.
	if (m_pLinkFrameButton->GetSafeHwnd() == 0
		 || m_pNextFrameButton->GetSafeHwnd() == 0
		 || m_pPreviousFrameButton->GetSafeHwnd() == 0)
	{
		return;
	}

	CPmwDoc* pDoc = GetDocument();

	PMGPageObject* pObject = pDoc->selected_objects();
	BOOL fShowLink = FALSE;
	BOOL fShowPrevious = FALSE;
	BOOL fShowNext = FALSE;

	// If we have a single frame selected, show buttons for it.
	if (pObject != NULL											// Have a object
		 && pObject->next_selected_object() == NULL		// Single object
		 && pObject->type() == OBJECT_TYPE_Frame			// Is a frame
		 && ((CFrameObject*)pObject)->GenericFrame()		// Is generic
		 && !(pObject->get_refresh_flags() & (REFRESH_FLAG_moving|REFRESH_FLAG_sizing)))
	{
		BOOL fHasPrevious = FALSE;
		BOOL fHasNext = FALSE;
		CFrameRecord* pFrame = ((CFrameObject*)pObject)->LockFrameRecord();
		if (pFrame != NULL)
		{
			fHasPrevious = (pFrame->PreviousFrame() != 0);
			fHasNext = (pFrame->NextFrame() != 0);
			pFrame->release();
		}
		// We want to put the controls on the frame.

		// Get the rect for the view.
		CRect crView;
		GetClientRect(crView);

		// Get the rect for the object.
		PBOX Bound = pObject->get_bound();
		rc.pbox_to_screen(&Bound, FALSE);

		CRect crObject;
		rc.convert_pbox(&Bound, &crObject, NULL);

		// Get the rect for the three buttons.
		CRect crLink, crNewLink;
		m_pLinkFrameButton->GetWindowRect(crLink);
		ScreenToClient(crLink);

		CRect crNext, crNewNext;
		m_pNextFrameButton->GetWindowRect(crNext);
		ScreenToClient(crNext);

		CRect crPrevious, crNewPrevious;
		m_pPreviousFrameButton->GetWindowRect(crPrevious);
		ScreenToClient(crPrevious);

		if (fHasPrevious)
		{
			int nCenter = crObject.left + crObject.Width()/4;

			crNewPrevious.bottom = crObject.top-1;
			crNewPrevious.top = crNewPrevious.bottom - crPrevious.Height();
			crNewPrevious.left = nCenter - crPrevious.Width()/2;
			crNewPrevious.right = crNewPrevious.left + crPrevious.Width();

			fShowPrevious = PositionButton(m_pPreviousFrameButton, crNewPrevious, crPrevious, crView);
		}

		int nWidth = crLink.Width();
		if (fHasNext)
		{
			nWidth += crNext.Width();
		}

		int nCenter = crObject.right - crObject.Width()/4;
		int nLeft = nCenter - nWidth/2;

		// Position the link button.

		crNewLink.left = nLeft;
		crNewLink.right = crNewLink.left + crLink.Width();
		crNewLink.top = crObject.bottom+1;
		crNewLink.bottom = crNewLink.top + crLink.Height();

		fShowLink = PositionButton(m_pLinkFrameButton, crNewLink, crLink, crView);

		if (fHasNext)
		{
			crNewNext.left = crNewLink.right;
			crNewNext.right = crNewNext.left + crNext.Width();
			crNewNext.top = crObject.bottom+1;
			crNewNext.bottom = crNewNext.top + crNext.Height();

			fShowNext = PositionButton(m_pNextFrameButton, crNewNext, crNext, crView);
		}
	}

	// Show the buttons as needed.
	ShowButton(m_pLinkFrameButton, fShowLink);
	ShowButton(m_pPreviousFrameButton, fShowPrevious);
	ShowButton(m_pNextFrameButton, fShowNext);
}

void CPmwView::OnTextLink() 
{
	if (edit_event_handler == mode_link)
	{
		AbortLink();
	}
	else
	{
		set_mode(mode_link);
	}
}

void CPmwView::OnTextPreviousFrame() 
{
	ChangeFrame(-1);
}

void CPmwView::OnTextNextFrame() 
{
	ChangeFrame(1);
}

void CPmwView::ChangeFrame(int nDirection)
{
	CPmwDoc* pDoc = GetDocument();
	CFrameObject* pObject = (CFrameObject*)pDoc->selected_objects();
	if (pObject != NULL
		 && pObject->next_selected_object() == NULL
		 && pObject->GenericFrame())
	{
		CFrameRecord* pFrame = pObject->LockFrameRecord();
		if (pFrame != NULL)
		{
			DB_RECORD_NUMBER lNewFrame = nDirection == -1
														? pFrame->PreviousFrame()
														: pFrame->NextFrame();
			pFrame->release();

			if (lNewFrame != 0)
			{
				CHARACTER_INDEX lCharacter = -1;

				if (m_Txp.Valid())
				{
					pFrame = (CFrameRecord*)pDoc->get_database()->get_record(lNewFrame, NULL, RECORD_TYPE_Frame);
					if (pFrame != NULL)
					{
						if (pFrame->NumberOfLines() == 0)
						{
							// There is no text in this frame. If we are editing, we
							// want to stop editing and just select the new frame.
							set_arrow_tool();
						}
						else
						{
							// Assume start at the front.
							lCharacter = pFrame->FirstCharacter();
							if (nDirection == -1)
							{
								// Come in on the end if traveling backwards.
								lCharacter += pFrame->NumberOfCharacters()-1;
							}
						}
						pFrame->release();
					}
				}

				// Do the move.
				SetTxpFrame(lNewFrame, lCharacter);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Sentiment support.

void CPmwView::PasteSentiment(const CMlsStringArray& csaSentiment)
{
	// Prepare!
	set_arrow_tool();
	GetDocument()->deselect_all();

	// Paste front if we have one.
	if (!csaSentiment.GetAt(0).IsEmpty())
	{
		PasteSentiment(csaSentiment.GetAt(0), 0, FALSE, TRUE);
	}
	// Paste inside if we have one.
	if (!csaSentiment.GetAt(1).IsEmpty())
	{
		PasteSentiment(csaSentiment.GetAt(1), 1, FALSE, TRUE);
	}
}

/*
// Helper for sentiment pasting.
// nWhere is:
//		-1 center
//		 0 top
//		 1 bottom
*/

void CPmwView::PasteSentiment(const CString& csSentiment, int nWhere, BOOL fOnCard, BOOL fSelect)
{
	CPmwDoc* pDoc = GetDocument();

	// Get the world for this panel.
	PBOX World;
	pDoc->get_panel_world(&World);

	PCOORD DX = World.x1 - World.x0;
	PCOORD DY = World.y1 - World.y0;
	BOOL fLandscape = fOnCard && (DX > DY);

	// Get upper left and dimensions.
	PBOX Bound;
	PPNT p;			// Dimensions.

   if (fLandscape)
	{
		p.x = (DX*1)/3;
		p.y = (DY*2)/3;
	}
	else
	{
		p.x = (DX*2)/3;
		p.y = (DY*1)/3;
	}

	switch (nWhere)
	{
		case -1:
		{
			// Centered on panel.
			Bound.x0 = (DX-p.x)/2;
			Bound.y0 = (DY-p.y)/2;
			break;
		}
		case 0:
		{
			// Top/left of panel.
			if (fLandscape)
			{
				// Left
				Bound.x0 = (DX/2 - p.x)/2;
				Bound.y0 = (DY - p.y)/2;
			}
			else
			{
				// Top
				Bound.x0 = (DX-p.x)/2;
				Bound.y0 = (DY/2 - p.y)/2;
			}
			break;
		}
		case 1:
		{
			// Bottom/right of panel.
			if (fLandscape)
			{
				// Right
				Bound.x0 = DX/2 + (DX/2 - p.x)/2;
				Bound.y0 = (DY - p.y)/2;
			}
			else
			{
				// Bottom
				Bound.x0 = (DX-p.x)/2;
				Bound.y0 = DY/2 + (DY/2 - p.y)/2;
			}
			break;
		}
		default:
		{
			ASSERT(FALSE);
			return;
		}
	}

	// Set the other side.
	Bound.x0 += World.x0;
	Bound.y0 += World.y0;
	Bound.x1 = Bound.x0 + p.x;
	Bound.y1 = Bound.y0 + p.y;

	FRAMEOBJ_CREATE_STRUCT fcs;
	fcs.bound = Bound;

	fcs.alignment = (ALIGN_TYPE)m_Txp.Style().Alignment();
//	fcs.vert_alignment = (VERT_ALIGN_TYPE)m_Txp.FrameAttributes().m_nVerticalAlignment;
	fcs.vert_alignment = ALIGN_top;

	CFrameObject* pObject = pDoc->create_frame_object(&fcs);

	if (pObject != NULL)
	{
		CTxp Txp(pDoc->get_database());

		/* Stick in some text. */
		Txp.Init(pObject, 0);
		Txp.SetHorizontalAlignment(fcs.alignment);
		Txp.CalcStyle();
		Txp.InsertString(csSentiment);

		// Set the default style to what the global Txp currently has.
		pObject->SetTextStyle(&m_Txp.Style());

		// Apply the properties to this object.
		pObject->SetObjectProperties(pDoc->GetDefaultObjectProperties());

		// We have the object. Begin dragging it.
		pObject->calc();

		// Set the panel for the object.
		pObject->set_panel(pDoc->get_current_panel());

		// Add us to the document...
		pDoc->append_object(pObject);
		pDoc->refresh_object(pObject);

		// Select us.
		if (fSelect)
		{
			pDoc->select_object(pObject);
		}
	}
}

void CPmwView::OnFormatBullets() 
{
	CPBulletPropertySheet dialog(&GetCurrentTextStyle(), (PMGFontServer*)GetDocument()->get_font_server(), IDS_BULLET_DLG_TITLE, this, IDD_BULLET_SYMBOL_PAGE);
	// remove the apply button do after the contructor and before DoModal
	dialog.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	if( dialog.DoModal() == IDOK )
	{
		CPmwDoc* pDocument = GetDocument();
		CCmdParagraphFormatting* pCommand = new CCmdParagraphFormatting;
		if (pCommand->Before(this))
		{
			// if applying bullets turn off drop caps
			if( dialog.GetStyle().BulletType() != BULLET_none )
				dialog.GetStyle().DropLines(0);
			pCommand->After(dialog.GetStyle().m_Paragraph);
			pDocument->AddCommand(pCommand);
			pDocument->SetModifiedFlag();
		}	
	}
}

void CPmwView::OnUpdateFormatBullets(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );
}


// Inserts a symbol character from the PressWriter symbol font into text 
// and then changes the font back to what it was before the insert
void CPmwView::OnFormatInsertSymbol() 
{
	CBInsertSymbol dialog( this );
	if( dialog.DoModal() == IDOK )
	{
		int nFace = typeface_server.find_face(dialog.GetFont(), FALSE);
		if( nFace != -1 )
		{
			CPmwDoc* pDoc = GetDocument();
			CTextCommand* pCommand = new CTextCommand(IDCmd_InsertSymbol);
			if (!pCommand->Before(this))
			{
				delete pCommand;
				pCommand = NULL;
			}
			// delete selected text 1st to avoid multiple styles undo/redo bug
			if (m_Txp.SomethingSelected())
			{
				m_Txp.DeleteText(1);
			}

			PMGFontServer* pFontServer = (PMGFontServer*)GetDocument()->get_font_server();
			DB_RECORD_NUMBER fontID = pFontServer->font_face_to_record(nFace);

			// create a new style based off the current style
			CTextStyle newStyle = m_Txp.Style();
			SHORT nOriginalFont = pFontServer->font_record_to_face(newStyle.Font());
			// set the new style's font to PressWriter Symbol
			newStyle.Font(fontID);
			// set the new style
//			m_Txp.Style() = newStyle;
//			m_Txp.Style().UpdateFontMetrics();

			// get the symbol character and insert it into text
			unsigned short symbol = dialog.GetChar();
			CHARACTER_INDEX c_start = m_Txp.Index();
			m_Txp.InsertText( &symbol, 1 );
			// select the symbol for the undo/redo
			m_Txp.SelectText(c_start, m_Txp.Index());

			// change the inserted text to the symbol font
			CHARACTER_INDEX lStart, lEnd;
			m_Txp.GetRange(&lStart, &lEnd);
			m_Txp.Off();
			CTextRecord* pText = m_Txp.LockText();
			ASSERT(pText != NULL);

			if (pText->ChangeRangeStyle(lStart, lEnd, &newStyle) )
			{
				m_Txp.UpdateRange();
			}
			pText->release();
			pText = NULL;

			pDoc->SetModifiedFlag();

			// Finish the undo/redo command, if there is one.
			if (pCommand != NULL)
			{
				if (pCommand->After())
				{
					pDoc->AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
				}
			}
			// deselect the symbol
			m_Txp.SetPosition(m_Txp.SelectEnd(), CTxp::END_NoSelection, FALSE );

			// Refresh now and then set the temporary style in the txp back
			// to the original because the refresh resets the style in the txp
			pDoc->idle_refresh();
			// set the style back to what it was before the symbol was inserted
			fontID = pFontServer->font_face_to_record(nOriginalFont);
			CTextStyle oldStyle = GetCurrentTextStyle();
			oldStyle.Font( fontID );
			m_Txp.Style() = oldStyle;
			m_Txp.Style().UpdateFontMetrics();
		}
	}
}

void CPmwView::OnUpdateFormatInsertSymbol(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );	
}


void CPmwView::OnFormatInsertdate() 
{
	CPmwApp* pApp = GET_PMWAPP();
	CString strDate;
	// get the fo rmatted date string from the application
	if( pApp && pApp->GetUserDateFormattedString( &strDate ) )
	{
		// create an undo before inserting the text
		CTextCommand* pCommand = new CTextCommand(IDCmd_InsertDate);
		if (!pCommand->Before(this))
		{
			delete pCommand;
			pCommand = NULL;
		}
		// delete selected text 1st to avoid multiple styles undo/redo bug
		if (m_Txp.SomethingSelected())
		{
			m_Txp.DeleteText(1);
		}
		// get the start of the selection or caret
		CHARACTER_INDEX c_start = m_Txp.Index();
		// insert the string
		m_Txp.InsertString( strDate );
		// select the date
		m_Txp.SelectText(c_start, m_Txp.Index());

		CPmwDoc* pDoc = GetDocument();
		if( pDoc )
			pDoc->SetModifiedFlag();
		// Finish the undo/redo command, if there is one.
		if( pDoc && pCommand )
		{
			if (pCommand->After())
			{
				pDoc->AddCommand(pCommand, FALSE);
			}
			else
			{
				delete pCommand;
			}
		}
		// deselect the date
		m_Txp.SetPosition(m_Txp.SelectEnd(), CTxp::END_NoSelection, FALSE );
	}
}

void CPmwView::OnUpdateFormatInsertdate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );
}

void CPmwView::OnFormatDropcapsNone() 
{
	ApplyDropCap( 0 );
}

void CPmwView::OnUpdateFormatDropcapsNone(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );
	pCmdUI->SetCheck( m_Txp.SelectStyle().DropLines() == 0 );
}

void CPmwView::OnFormatDropcapsFourlines() 
{
	ApplyDropCap( 4 );
}

void CPmwView::OnUpdateFormatDropcapsFourlines(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );
	pCmdUI->SetCheck( m_Txp.SelectStyle().DropLines() == 4 );
}

void CPmwView::OnFormatDropcapsThreelines() 
{
	ApplyDropCap( 3 );
}

void CPmwView::OnUpdateFormatDropcapsThreelines(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );	
	pCmdUI->SetCheck( m_Txp.SelectStyle().DropLines() == 3 );
}

void CPmwView::OnFormatDropcapsTwolines() 
{
	ApplyDropCap( 2 );
}

void CPmwView::OnUpdateFormatDropcapsTwolines(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_Txp.Valid() );
	pCmdUI->SetCheck( m_Txp.SelectStyle().DropLines() == 2 );
}

void CPmwView::ApplyDropCap( int nLines ) 
{
	ASSERT( nLines >= 0 );
	CCmdParagraphFormatting* pCommand = new CCmdParagraphFormatting;
	if (pCommand->Before(this))
	{
		CPmwDoc* pDocument = GetDocument();
		CTextStyle style = m_Txp.SelectStyle();
		style.DropLines(nLines);
		// if applying drop caps make sure bullets are turned off
		if( nLines > 0 )
		{
			style.BulletType( BULLET_none );
		}
		pCommand->After(style.m_Paragraph);
		pDocument->AddCommand(pCommand);
		pDocument->SetModifiedFlag();
	}
}

