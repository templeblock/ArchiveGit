/*
// REFRESH.CPP
//
// Refresh code for views.
//
// 	Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/REFRESH.CPP 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/REFRESH.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 5     6/05/98 8:54a Fredf
// Brings up receiver list on refresh() for with help labels.
// 
// 4     4/30/98 7:57p Jayn
// Ignores all WM_TIMER messages.
// 
// 3     2/27/98 2:47p Jayn
// "Draw as bitmaps"
// 
// 2     11/21/97 4:00p Hforman
// (Jay's fix) -- set rc.clip_region to NULL in refresh()
// 
//    Rev 1.0   14 Aug 1997 15:25:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:16   Fred
// Initial revision.
// 
//    Rev 1.8   11 Jul 1997 17:15:50   Jay
// Refresh semaphore
// 
//    Rev 1.7   26 Jun 1997 12:19:04   Jay
// Updates edit bitmap if area changes size or moves.
// 
//    Rev 1.6   26 Mar 1997 14:52:04   Jay
// Interruptable preview
// 
//    Rev 1.5   07 Feb 1997 08:52:28   Fred
// Added comments
// 
//    Rev 1.4   24 Jan 1997 14:58:32   Fred
// Added some comments
// 
//    Rev 1.3   28 Aug 1996 11:13:02   Jay
// Filters out tool tip timer messages in interrupt check.
// 
//    Rev 1.2   11 Jul 1996 17:46:14   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:22:20   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:34:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:46   FRED
// Initial revision.
// 
//    Rev 2.4   27 Sep 1995 15:27:28   JAY
// New OLE code
// 
//    Rev 2.3   27 Apr 1995 13:26:34   FRED
//  
// 
//    Rev 2.2   26 Apr 1995 14:41:08   JAY
//  
// 
//    Rev 2.1   08 Feb 1995 13:34:52   JAY
// Reverted. New series.
// 
//    Rev 1.18   30 Jan 1995 09:32:46   JAY
// Cut and paste
// 
//    Rev 1.17   23 Jan 1995 16:46:10   JAY
// 
//    Rev 1.16   23 Jan 1995 16:09:16   JAY
// 
//    Rev 1.15   23 Jan 1995 14:03:16   JAY
// Fixed a couple of bugs in do_erase_draw_breakup().
// 
//    Rev 1.14   07 Jul 1994 11:32:58   JAY
// Correctly frees the update states if IsIconic() or extent goes out of view.
// 
//    Rev 1.13   21 Jun 1994 13:30:32   JAY
// Now interrupts on KEYUP of ALT.
// 
//    Rev 1.12   15 Mar 1994 17:13:12   JAY
// Now attempts to rebuild the edit background if it has been lost when done
// refreshing in idle handler.
// 
//    Rev 1.11   09 Mar 1994 09:23:50   MACDUFF
// Added ExtraPixels param to refresh extents
// 
//    Rev 1.10   25 Feb 1994 14:13:20   JAY
// Once drawing is completed, it makes sure the txp cursor is on if valid.
// 
//    Rev 1.9   14 Feb 1994 11:32:56   JAY
// KEY_UP events no longer interrupt.
// 
//    Rev 1.8   11 Feb 1994 16:55:20   JAY
// Refresh extents with objects are put at the end of the array (assuming they
// will be in some kind of order).
// If text is being edited, the refresh extent containing the text cursor is
// given priority.
// 
//    Rev 1.7   01 Feb 1994 14:04:48   JAY
// Will now not attempt to refresh when the window is iconic.
// 
//    Rev 1.6   31 Jan 1994 07:24:46   JAY
// Removed extra od('s for release version.
// 
//    Rev 1.5   28 Jan 1994 17:12:28   JAY
//  
// 
//    Rev 1.4   25 Jan 1994 12:56:10   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:02:16   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:10:44   JAY
// Restructuring, cut/paste, images, etc.
// 
//    Rev 1.1   30 Dec 1993 16:12:04   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:28:02   JAY
// 
// 
*/

#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"

#include "util.h"
#include "utils.h"

#include <afxpriv.h>

extern CPalette *pOurPal;

void CPmwView::add_refresh(PBOX_PTR extent, REFRESH_TYPE type, PMGPageObjectPtr object, LPRECT lprExtraPixels)
{
	CBoolSemaphore Semaphore(m_fRefreshSemaphore);

	if (!Semaphore.Lock())
	{
		// Semaphore is locked. We must leave now.
		// We are not allowed to add a refresh while refreshing!
		return;
	}

	REFRESH_EXTENT re;
	PBOX r = *extent;

	if (lprExtraPixels != NULL)
	{
		r.x0 -= lprExtraPixels->left * rc.redisplay_x_pixel;
		r.x1 += lprExtraPixels->right * rc.redisplay_x_pixel;
		r.y0 -= lprExtraPixels->top * rc.redisplay_x_pixel;
		r.y1 += lprExtraPixels->bottom * rc.redisplay_x_pixel;
	}

//	od("add_refresh: %ld, %ld, %ld, %ld [%d] (source: %ld, %ld, %ld, %ld)\r\n",
//					r, type, rc.source_pbox);

	if (extent->x1 > 1000000L)
	{
		od("Big!\r\n");
	}

	if (object == NULL)
	{
	// Special refresh types are not defined for null object
		type = REFRESH_ALL;
	}

/* Make sure there's something we need to do about this. */

   if (IntersectBox(&r, &r, &rc.source_pbox) == 0
			|| r.x0 == r.x1
			|| r.y0 == r.y1)
	{
      return;
	}

	// Add the extent.

	re.extent = r;
	re.refresh_type = type;
	re.my_object = object;

	re.update_object = NULL;
	re.update_state = NULL;

	m_RefreshArray.Add(&re);
}

/*
// Check whether we need to interrupt our refresh or not.
*/

#ifdef WIN32
#define ID_TIMER_WAIT   0xE000  // timer while waiting to show status
#define ID_TIMER_CHECK  0xE001  // timer to check for removal of status
#endif

BOOL standard_check_interrupt(LPVOID data)
{
	MSG msg;
	RedisplayContextPtr rc = (RedisplayContextPtr)data;

//	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD))
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{       
	/* See if it's a casual mouse move. */

		if (msg.message != WM_NCMOUSEMOVE
					&& (msg.message != 0x118)		// undocumented WM_SYSTIMER
#ifdef WIN32
					&& (msg.message != WM_TIMER
						 /*|| (msg.wParam != ID_TIMER_WAIT && msg.wParam != ID_TIMER_CHECK)*/)
#endif
					&& (msg.message != WM_KICKIDLE)		// defined in afxpriv.h (used by property sheets)
 					&& (msg.message != WM_MOUSEMOVE
	 					|| (msg.wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
					&& (msg.message != WM_PAINT
						|| rc->hwnd == 0
						|| rc->hwnd == msg.hwnd)
					&& (msg.message != WM_KEYDOWN
							|| !(msg.wParam == VK_SHIFT
									|| msg.wParam == VK_CONTROL))
					&& (msg.message != WM_KEYUP
								|| msg.wParam == VK_MENU))
		{
			TRACE("Interrupt for %x (%d) [%08x vs %08x]\r\n", msg.message, msg.wParam, msg.hwnd, rc->hwnd);
			return TRUE;
		}

		// Pump the message through.
		AfxGetApp()->PumpMessage();
	}

/*
// Check #2:
// If our window has suddenly had its update locked, our visible region will
// suddenly have gone to NULL.
// In this case, we don't want to do any drawing; simply terminate.
*/

	RECT r;

	if (GetClipBox(rc->destination_hdc, &r) == NULLREGION)
	{
//		od("Now NULL\r\n");
		return -1;
	}

	return FALSE;
}

/*
// Perform any refreshes which are pending.
*/

BOOL CPmwView::refresh(VOID)
{
	// The receiver list is brought up here because we know at this
	// point that PrintMaster has reached an idle point. Other mechanisms
	// ran afoul of race conditions with destroying the hub window and
	// the initial poster which caused bad parent window selections or
	// improper enabled states for the main frame window and/or the
	// receiver list.
	if (m_fBringUpReceiverList)
	{
		m_fBringUpReceiverList = FALSE;
		PostMessage(WM_COMMAND, PMWM_WH_VIEW_ADDRESS_BOOK, 0);
	}

	BOOL more = FALSE;

	CBoolSemaphore Semaphore(m_fRefreshSemaphore);

	if (!Semaphore.Lock())
	{
	/* Pretend we got interupted. Everything stays the same. */
		TRACE("View semaphore is locked...\n");
		return FALSE;
	}

	int nExtents = m_RefreshArray.NumberOfExtents();

	if (nExtents != 0)
	{
		int nExtent;
		REFRESH_EXTENT* pExtent;

		if (IsIconic())
		{
		/*
		// All bets are off.
		// Throw out all refresh extents.
		*/

			m_RefreshArray.FreeAllExtents();

			return FALSE;
		}

		// If we are currently editing text in the view, then move any
		// refresh extents which are under the cursor to the front. The
		// idea is to give refresh priority to those areas closest to the
		// user's working area.
		if (nExtents > 1 && m_Txp.Valid())
		{
			// Editing text!
			PBOX cursor = m_Txp.CursorBox();

			for (nExtent = 0, pExtent = m_RefreshArray.GetExtent(0);
						nExtent < nExtents; nExtent++, pExtent++)
			{
				if (IsBoxOnBox(&cursor, &pExtent->extent))
				{
					if (nExtent != 0)
					{
					/* Move it to the front. */
						REFRESH_EXTENT it = *pExtent;
						m_RefreshArray.DeleteExtent(nExtent);
						m_RefreshArray.insert_element(&it, 0);
					}
					break;
				}
			}
		}

		CPmwDoc *document = GetDocument();
		HWND hWnd = GetSafeHwnd();
		HPALETTE hOurPal, hOldPal;

//		od("refresh view %d...\r\n", m_RefreshArray.NumberOfExtents());

		// Save the hdc, just in case.

		HDC hdcSave = rc.destination_hdc;

		// Get a DC for our use. (Bypass the alloc of the CWnd GetDC().)

		rc.destination_hdc = ::GetDC(hWnd);

	/*
	// When we are not allowed to draw, our window update gets locked.
	// During this time, we can still draw but it's all wasted and just
	// generates a PAINT message later, which isn't what we want.
	// When our update has been locked, the visible region of any DC we
	// get is of type NULLREGION. So we look at our visible region to see
	// if it's NULL and only draw if it's not.
	*/

		RECT dc_clip_rect;
		SHORT rgntype;

		if ((rgntype = GetClipBox(rc.destination_hdc, &dc_clip_rect)) == NULLREGION)
		{
		/* Try again later. */
//			od("GCB == NULL\r\n");
			more = TRUE;
		}
		else
		{
			// Select and realize THE PrintMaster palette.
			hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
			hOldPal = SelectPalette(rc.destination_hdc, hOurPal, FALSE);
			RealizePalette(rc.destination_hdc);

			// Set the function which will be called to check for interrupts.
			rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&rc);

			// Process the refresh extents.
			while (m_RefreshArray.NumberOfExtents() != 0)
			{
				REFRESH_EXTENT* pExtent;

				// Get the next extent.

				pExtent = m_RefreshArray.GetExtent(0);
				PBOX pbox = pExtent->extent;

				// Clip the refresh extent against the boundaries of the view (source_pbox).
				if (IntersectBox(&pbox, &pbox, &rc.source_pbox))
				{
					RECT clip;
//					od("do_refresh %ld, %ld, %ld, %ld\r\n", pbox);

					// Compute the clipping rectangle in window coordinates.
					rc.pbox_to_screen(&pbox, TRUE);
					rc.convert_pbox(&pbox, &clip, NULL);

				/* See if we have anything left. */

					if (!IsRectEmpty(&clip))
					{
					/* Create a clipping region to force us to stay within this area. */

						HRGN hrgnClip = CreateRectRgn(clip.left, clip.top, clip.right, clip.bottom);

					/* Select a copy of the clip region into the DC. */

						SHORT ret = SelectClipRgn(rc.destination_hdc, hrgnClip);

						if (ret == NULLREGION || ret == ERROR)
						{
//							od("SCR ret: %d\r\n", ret);

						/* Try again later. */

							DeleteObject(hrgnClip);
/*1*/							more = TRUE;
/*1*/							break;
						}
						else
						{
							BOOL r;

							rc.clip_region = hrgnClip;

							// Remember the clipping rectangle in the RC.
							GetClipBox(rc.destination_hdc, &rc.clip_rect);

						/* Update the objects. */

							// extent is pointer to refresh array element
							r = document->draw_objects(pExtent, &rc);

						/* Delete the clip region. */

							rc.clip_region = NULL;
							DeleteObject(hrgnClip);

							if (r)
							{
							/* Interrupted. */
								more = TRUE;
								break;
							}
						}
					}
				}

				// Get rid of the update state.
				pExtent->FreeUpdateState();

				// Remove it!
				m_RefreshArray.DeleteExtent(0);
			}

			// Select the other palette in.
			SelectPalette(rc.destination_hdc, hOldPal, FALSE);
		}

		// Release the DC we got.
		::ReleaseDC(hWnd, rc.destination_hdc);
		rc.destination_hdc = hdcSave;
	}

	if (!more)
	{
		if (m_Txp.Valid())
		{
			UpdateEditBackground();
		/* Make sure we're showing in case something turned us off. */
			TxpOn();
		}
		UpdateFrameButtons();
	}
	return more;
}

/////////////////////////////////////////////////////////////////////////////
// CRefreshArray

extern void breakup_overlap(PBOX_PTR r1, PBOX_PTR r2, PBOX_PTR output, int16 *r1_pieces, int16 *r2_pieces);

CRefreshArray::CRefreshArray() : Array(sizeof(REFRESH_EXTENT))
{
}

CRefreshArray::~CRefreshArray()
{
	FreeAllExtents();
}

void CRefreshArray::Add(REFRESH_EXTENT* pre, BOOL fCompact /*=TRUE*/)
{
	if (pre->my_object == NULL)
	{
		// Most recent event to beginning.
		insert_element(pre, 0);
	}
	else
	{
		// For an object. Expect a lot of them. Add at end.
		add_element(pre);
	}

	// If we want to compact the list, do it now.
	if (fCompact)
	{
		Compact();
	}
}

//
// do_erase_draw_breakup()
//
//		Take two extents with different refresh types for an object, where the 
// two extents overlap.  Break them up into 0-2 regions of the first type, 
// 0-2 regions of the second type, and one redraw region (the overlap).
//
// Arguments:
//		box1				pointer to PBOX containing first region
//		box2				pointer to PBOX containing second region
//		type1				type of refresh for first box
//		type2				type of refresh for second box
//		overlap_extent	pointer to refresh extent which will be used as
// destination for the overlap region.  The extent must already be
// initialized with a my_object field, as this will be used for the new
// extents we create.
//
// Returns:
//		TRUE 				success
//		FALSE				not enough room in refresh list; nothing done
//

BOOL CRefreshArray::do_erase_draw_breakup(PBOX* box1,
														PBOX* box2,
														REFRESH_TYPE type1,
														REFRESH_TYPE type2,
														REFRESH_EXTENT* overlap_extent)
{
	PBOX new_boxes[5];
	int16 new_count, count1, count2;

	// Break the two boxes up into temporary storage
	breakup_overlap(box1, box2, new_boxes, &count1, &count2);
	new_count = count1 + count2;
	ASSERT(new_count <= 5);

	int16 i;
	REFRESH_EXTENT extent;

	// Yes.  First store the overlap extent.
	overlap_extent->extent = new_boxes[0];
	overlap_extent->refresh_type = REFRESH_REDRAW;

	// Now add the new extents

	extent.update_object = NULL;
	extent.update_state = NULL;
	extent.my_object = overlap_extent->my_object;

	for (i = 1; i <= new_count; i++)
	{                                                           
		ERRORCODE error;

		extent.extent = new_boxes[i];
		extent.refresh_type = (i <= count1) ? type1 : type2;

		if ((error = AddExtent(&extent)) != ERRORCODE_None)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*
// Compact the refresh list.
*/

//#define DEBUG_IT

void CRefreshArray::Compact(void)
{
#ifdef DEBUG_IT
od("compact start: count = %d\r\n", NumberOfExtents());
#endif

	BOOL fAnyMerge;
	do
	{
		fAnyMerge = FALSE;

		int i;
		REFRESH_EXTENT* prei;

		for (prei = GetExtent(i=0); i < NumberOfExtents(); i++, prei++)
		{
			PBOX r1 = prei->extent;

			REFRESH_EXTENT* prej = prei+1;
			int j = i+1;

			for (; j < NumberOfExtents(); j++, prej++)
			{
				BOOL fThisMerge = FALSE;
				PBOX r2 = prej->extent;

				if (IsBoxOnBox(&r1, &r2))
				{
					// The two extents have at least one interior point in common.
					// In most cases, just merge them.  The exception is when both
					// belong to the same object, and neither is a full refresh.

					if ((prei->my_object == NULL)
					 || (prei->my_object != prej->my_object)
					 || (prei->refresh_type == REFRESH_ALL)
					 || (prej->refresh_type == REFRESH_ALL)
					 || (prei->refresh_type == prej->refresh_type))
					{
						// Nothing special to do, just unionize the extents
						UnionBox(&prei->extent, &prei->extent, &prej->extent);
						fThisMerge = TRUE;

						if ((prei->my_object != prej->my_object)
						 || (prei->refresh_type != prej->refresh_type))
						{
							// Force this to be a generic full refresh
							prei->my_object = NULL;
							prei->refresh_type = REFRESH_ALL;
						}
					}
					else
					{
						// The two extents belong to the same object, and neither
						// is a full-refresh request, but they are different types

						fThisMerge = TRUE;

						if (IsBoxInBox(&r2, &r1) && (prei->refresh_type == REFRESH_ERASE))
						{
							// Extent i is an erase extent that completely
							// contains extent j, a draw or redraw extent.
							// Delete j, and force i to be a redraw extent.
							prei->refresh_type = REFRESH_REDRAW;
						}
						else if (IsBoxInBox(&r1, &r2) && (prei->refresh_type == REFRESH_ERASE))
						{
							// Same as above, except j contains i:
							//
							// Extent j is an erase extent that completely
							// contains extent i, a draw or redraw extent.
							// Delete i, and force j to be a redraw extent.
							prei->extent = prej->extent;
							prei->refresh_type = REFRESH_REDRAW;
						}
						else
						{
							// We have two extents of different types that overlap.
							// Try to break them up.
							BOOL fSuccess = do_erase_draw_breakup(
														&prei->extent,
														&prej->extent,
														prei->refresh_type,
														prej->refresh_type,
														prei);

							// Get the elements back in case the array moved.
							prej = GetExtent(j);
							prei = GetExtent(i);

							if (!fSuccess)
							{
								// Failed! Just do a merge and make it a full refresh
								UnionBox(&prei->extent, &prei->extent, &prej->extent);
								prei->my_object = NULL;
								prei->refresh_type = REFRESH_ALL;
							}
						}
					}
				}
				else if ((prei->refresh_type == prej->refresh_type)
					   && (prei->my_object == prej->my_object))
				{
					// Check if these regions share a side exactly.

					if ((r1.y0 == r2.y0) && (r1.y1 == r2.y1))
					{
					/* Check for [r1|r2] */
						if (r1.x1 == r2.x0)
						{
						/* Merge. */
							prei->extent.x1 = r2.x1;
							fThisMerge = TRUE;
						}
					/* Check for [r2|r1] */
						else if (r1.x0 == r2.x1)
						{
						/* Merge. */
							prei->extent.x0 = r2.x0;
							fThisMerge = TRUE;
						}
					}
					else if ((r1.x0 == r2.x0) && (r1.x1 == r2.x1))
					{
					/*
					// Check for [r1]
					//           [r2]
					*/
						if (r1.y1 == r2.y0)
						{
						/* Merge. */
							prei->extent.y1 = r2.y1;
							fThisMerge = TRUE;
						}
					/*
					// Check for [r2]
					//           [r1]
					*/
						else if (r1.y0 == r2.y1)
						{
						/* Merge. */
							prei->extent.y0 = r2.y0;
							fThisMerge = TRUE;
						}
					}
				}

				if (fThisMerge)
				{
					// Merged this time.
					prei->FreeUpdateState();
					prej->FreeUpdateState();
					prei->update_object = NULL;				// Restart this extent.
					fAnyMerge = TRUE;

#ifdef DEBUG_IT
					od("\r\nCompacted %d into %d.  ", j, i);
#endif
					DeleteExtent(j);

					// Get the elements back in case the array moved.

					prej = GetExtent(--j);	/* Stay here. */
					prei = GetExtent(i);

					// Get the new r1
					r1 = prei->extent;
				}
			}
		}
	} while (fAnyMerge);
#ifdef DEBUG_IT
od("compact end: count = %d\r\n", NumberOfExtents());
#endif
}

void CRefreshArray::FreeAllExtents(void)
{
	int nExtent;
	REFRESH_EXTENT* pExtent;
   int nExtents = NumberOfExtents();

	// Free all update states.
	for (nExtent = 0, pExtent = GetExtent(0); nExtent < nExtents; nExtent++, pExtent++)
	{
		// Free any update state this extent has.
		pExtent->FreeUpdateState();
	}

	// Empty the extent array.
	empty_array();
}
