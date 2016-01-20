/*
// $Header: /PM8/App/MODESIZE.CPP 1     3/03/99 6:08p Gbeddow $
//
// Mode size routines.
//
// $Log: /PM8/App/MODESIZE.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 29    2/11/99 10:09a Lwilson
// Added context menu support for component objects.
// 
// 28    1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 27    1/07/99 5:11p Rgrenfel
// Added a trigger for drag, and ensured it would not activate on keyboard
// movement.
// 
// 26    7/22/98 7:13p Johno
// Call pDoc->MarkFramesBelowSelectedObjects(),
// pDoc->UpdateMarkedFrames(), in CPmwView::resize_key()
// 
// 25    6/05/98 2:12p Jayn
// Overlapping object flashing rectangle(s) in web projects.
// 
// 24    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 23    5/26/98 2:39p Jayn
// New table code.
// 
// 22    4/30/98 4:48p Jayn
// Resizing of table rows and columns.
// 
// 21    4/28/98 3:39p Jayn
// New table cursors.
// 
// 20    4/22/98 6:27p Jayn
// Table development (primarily selection).
// 
// 19    4/14/98 4:41p Jayn
// Changes for the new table object.
// 
// 18    3/21/98 1:17p Fredf
// No longer confirms object deletion.
// 
// 17    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 16    3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 15    3/11/98 5:32p Johno
// Added case OBJECT_TYPE_Hyperlink: to process_right_button
// 
// 14    3/10/98 5:24p Jayn
// Master page changes
// 
// 13    2/25/98 2:25p Hforman
// add right-click menu for Border objects
// 
// 12    2/25/98 10:40a Jstoner
// table functionality updates...merge/split cells
// row/col selection done
// 
// 11    2/11/98 11:38a Jstoner
// table object additions
// 
// 10    2/03/98 10:34a Jstoner
// saving incremental progress 
// 
// 9     1/28/98 10:39a Jstoner
// incremental update to table project
// 
// 8     1/21/98 9:16a Jstoner
// cell handling stuff
// 
// 7     1/07/98 7:48a Jstoner
// modify arrow behavior on table objects
// 
// 6     12/22/97 10:48a Jstoner
// 
// 5     11/21/97 5:01p Hforman
// Guideline fixes, added toggle, etc.
// 
// 4     11/19/97 6:45p Hforman
// added guide_toggle
// 
// 3     11/17/97 3:58p Hforman
// more guide implementation
// 
// 2     11/14/97 11:55a Hforman
// add guide functionality
// 
//    Rev 1.0   14 Aug 1997 15:22:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:14   Fred
// Initial revision.
// 
//    Rev 1.21   30 Jul 1997 10:43:44   Jay
// Increased refresh bounds for line. More flow frame cases.
// 
//    Rev 1.20   16 Jul 1997 17:56:26   Jay
// Calendar date improvements
// 
//    Rev 1.19   09 Jun 1997 17:30:52   Jay
// Text improvements
// 
//    Rev 1.18   02 Jun 1997 15:20:36   Jay
// Won't snap the rotate handle. 
// 
//    Rev 1.17   01 Jun 1997 17:01:20   Jay
// Fixed move cursor.
// 
//    Rev 1.16   16 Apr 1997 09:37:28   Jay
// More text box enhancements.
// 
//    Rev 1.15   14 Apr 1997 15:42:34   Jay
// Text box editing changes
// 
//    Rev 1.14   21 Feb 1997 15:09:14   Jay
// New mode handling; crop mode support
// 
//    Rev 1.13   19 Feb 1997 16:53:14   Jay
// Snapping
// 
//    Rev 1.12   31 Jan 1997 15:50:50   Jay
// New desktop background color
// 
//    Rev 1.11   29 Jan 1997 17:24:30   Jay
// Next and previous object support
// 
//    Rev 1.10   08 Jan 1997 15:21:24   Fred
// More text box changes
// 
//    Rev 1.9   09 Dec 1996 15:48:58   Jay
// Added constrained movement.
// 
//    Rev 1.8   04 Sep 1996 18:11:24   Jay
// Spell-check fixes.
// 
//    Rev 1.7   01 Aug 1996 17:46:00   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.6   24 Jul 1996 10:26:12   Jay
// Corrected menu indices for objects (background is now gone).
// 
//    Rev 1.5   15 Jul 1996 17:46:30   Jay
//  
// 
//    Rev 1.4   12 Jul 1996 16:49:52   Jay
// Text wrap
// 
//    Rev 1.3   11 Jul 1996 17:46:12   Jay
//  
// 
//    Rev 1.2   08 Jul 1996 08:49:42   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.1   23 May 1996 14:58:46   Jay
// Removed a test that messed things up.
// 
//    Rev 1.0   14 Mar 1996 13:30:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:30   FRED
// Initial revision.
// 
//    Rev 2.17   15 Jan 1996 10:26:14   JAY
//  
// 
//    Rev 2.16   29 Sep 1995 16:00:18   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.15   22 Sep 1995 15:43:52   JAY
// Uses standard cursors when possible in WIN32
// 
//    Rev 2.14   31 Aug 1995 10:37:34   JAY
// Fixed 180 degree rotation not updating the image.
// 
//    Rev 2.13   20 Aug 1995 15:39:44   FRED
// Rotate object control
// 
//    Rev 2.12   14 Aug 1995 11:43:02   JAY
// Now knows when the angle changes (to help those cases when the bound does not).
// 
//    Rev 2.11   10 Aug 1995 17:13:42   JAY
// New rotate cursor.
// 
//    Rev 2.10   08 Aug 1995 13:24:58   JAY
// Right mouse button menu and side bar fixes.
// 
//    Rev 2.9   13 May 1995 11:19:46   JAY
//  
// 
//    Rev 2.8   10 May 1995 13:16:38   JAY
// Support for lines, ellipses, and rectangles.
// 
//    Rev 2.7   19 Apr 1995 12:33:56   JAY
//  
// 
//    Rev 2.6   29 Mar 1995 09:15:02   JAY
// Lots of changes
// 
//    Rev 2.5   15 Mar 1995 10:50:16   JAY
// Support for new text editing
// 
//    Rev 2.4   09 Mar 1995 08:36:38   JAY
// More undo.
// 
//    Rev 2.3   15 Feb 1995 17:16:02   JAY
// Undo
// 
//    Rev 2.2   08 Feb 1995 13:34:00   JAY
// Reverted. New series.
// 
//    Rev 1.17   09 Jan 1995 10:14:10   JAY
// Now uses standard Windows cursors.
// 
//    Rev 1.16   04 Aug 1994 07:34:42   JAY
// Large model and 32-bit changes
// 
//    Rev 1.15   20 Jul 1994 16:46:06   JAY
// Ignores a right button down if it's not a RISING condition.
// 
//    Rev 1.14   14 Mar 1994 08:40:46   JAY
// Won't start dragging on mouse button held down from some other process. Must
// get the RISING event to begin. This fixed problem with double clicking out of
// a dialog.
// 
//    Rev 1.13   09 Mar 1994 14:32:08   JAY
// Support for improved select drag, including new background state and CONTROL
// key with click code.
// 
//    Rev 1.12   24 Feb 1994 15:30:28   JAY
// All toggling of selects when an object is moved or resized is now done by
// the object itself.
// 
//    Rev 1.11   14 Feb 1994 11:31:46   JAY
// Fixed problem with right mouse button on calendar date.
// 
//    Rev 1.10   11 Feb 1994 09:21:12   JAY
// process_action() and edit_frame() now take PPNT parameter which tells where
// the mouse cursor currently is.
// 
//    Rev 1.9   10 Feb 1994 15:53:58   MACDUFF
// Toggle functions now return a flag indicating whether they
// drew anything.
// 
//    Rev 1.8   02 Feb 1994 11:39:40   JAY
// Shift click was not selecting the sub object.
// 
//    Rev 1.7   31 Jan 1994 07:24:40   JAY
// Removed extra od('s for release version.
// 
//    Rev 1.6   28 Jan 1994 17:12:24   JAY
//  
// 
//    Rev 1.5   25 Jan 1994 12:56:08   JAY
//  
// 
//    Rev 1.4   19 Jan 1994 17:08:54   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:02:12   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:10:32   JAY
// Restructuring, cut/paste, images, etc.
// 
//    Rev 1.1   30 Dec 1993 16:12:02   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:27:58   JAY
// 
*/

#include "system.h"
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "action.h"
#include "utils.h"
#include "commands.h"				// for Undo support
#include "ccommand.h"				// for Calendar undo support

#include "grafobj.h"
#include "calobj.h"
#include "caldoc.h"
#include "tblobj.h"
#include "celobj.h"
#include "compobj.h"

#include "clip.h"						// Clipboard format definitions.
#include "FeedbackDropSource.h"	// Drop source support.
#include "DragDataSource.h"		// Drop source data support.

/*
// Clicking on a background can either mean select the background or begin
// a select box drag. This define tells how much the mouse has to move before
// it considers it a select drag.
*/

#define MIN_SELECT_DELTA 3

static LPCSTR
ResizeCursor[] =
{
	IDC_SIZENWSE,							// OBJECT_HANDLE_UL
	IDC_SIZENS,								// OBJECT_HANDLE_UPPER
	IDC_SIZENESW,							// OBJECT_HANDLE_UR
	IDC_SIZEWE,								// OBJECT_HANDLE_LEFT
	IDC_SIZEALL,							// OBJECT_HANDLE_MOVE (*not used*)
	IDC_SIZEWE,								// OBJECT_HANDLE_RIGHT
	IDC_SIZENESW,							// OBJECT_HANDLE_LL
	IDC_SIZENS,								// OBJECT_HANDLE_LOWER
	IDC_SIZENWSE,							// OBJECT_HANDLE_LR
	MAKEINTRESOURCE(IDC_ROTATE),		// OBJECT_HANDLE_ROTATE
	MAKEINTRESOURCE(IDC_SPLITH),		//	MAKEINTRESOURCE(IDC_RESIZE_COL),
	MAKEINTRESOURCE(IDC_SPLITV),		//	MAKEINTRESOURCE(IDC_RESIZE_ROW),
	MAKEINTRESOURCE(IDC_SELECT_COL),
	MAKEINTRESOURCE(IDC_SELECT_ROW)
};

static LPCSTR
SplitCursor[] =
{
	MAKEINTRESOURCE(IDC_SPLITH),
	MAKEINTRESOURCE(IDC_SPLIT135),
	MAKEINTRESOURCE(IDC_SPLITV),
	MAKEINTRESOURCE(IDC_SPLIT45)
};

static LPCSTR
SelectRCCursor[] =
{
	MAKEINTRESOURCE(IDC_SELECT_RC0),
	MAKEINTRESOURCE(IDC_SELECT_RC45),
	MAKEINTRESOURCE(IDC_SELECT_RC90),
	MAKEINTRESOURCE(IDC_SELECT_RC135),
	MAKEINTRESOURCE(IDC_SELECT_RC180),
	MAKEINTRESOURCE(IDC_SELECT_RC225),
	MAKEINTRESOURCE(IDC_SELECT_RC270),
	MAKEINTRESOURCE(IDC_SELECT_RC315)
};

static HCURSOR GetResizeCursor(PMGPageObject* pObject, OBJECT_HANDLE nHandle)
{
// Table which rotates handles by 45 degrees.
	static OBJECT_HANDLE RotateHandle[] =
	{
		OBJECT_HANDLE_LEFT,			// OBJECT_HANDLE_UL
		OBJECT_HANDLE_UL,				// OBJECT_HANDLE_UPPER
		OBJECT_HANDLE_UPPER,			// OBJECT_HANDLE_UR
		OBJECT_HANDLE_LL,				// OBJECT_HANDLE_LEFT
		OBJECT_HANDLE_MOVE,			// OBJECT_HANDLE_MOVE (same)
		OBJECT_HANDLE_UR,				// OBJECT_HANDLE_RIGHT
		OBJECT_HANDLE_LOWER,			// OBJECT_HANDLE_LL
		OBJECT_HANDLE_LR,				// OBJECT_HANDLE_LOWER
		OBJECT_HANDLE_RIGHT,			// OBJECT_HANDLE_LR
		OBJECT_HANDLE_ROTATE,		// OBJECT_HANDLE_ROTATE (same)
		OBJECT_HANDLE_RESIZE_COL,	// OBJECT_HANDLE_RESIZE_COL (same)
		OBJECT_HANDLE_RESIZE_ROW,	// OBJECT_HANDLE_RESIZE_ROW (same)
		OBJECT_HANDLE_SELECT_COL,	// OBJECT_HANDLE_SELECT_COL (same)
		OBJECT_HANDLE_SELECT_ROW	// OBJECT_HANDLE_SELECT_ROW (same)
	};
	nHandle = (OBJECT_HANDLE)(nHandle & 0x00ff);			// Mask off handle type.
	LPCSTR Cursor;

	double dAngle = RAD2DEG(normalize_angle(pObject->get_rotation())) + 22.5;

	if (nHandle == OBJECT_HANDLE_RESIZE_COL
		 || nHandle == OBJECT_HANDLE_RESIZE_ROW)
	{
		// Split handle (table row/column resize).
		int nIndex = (nHandle == OBJECT_HANDLE_RESIZE_COL) ? 0 : 2;
		while (dAngle >= 45.0)
		{
			nIndex++;
			dAngle -= 45.0;
		}
		Cursor = SplitCursor[nIndex & 3];
	}
	else if (nHandle == OBJECT_HANDLE_SELECT_COL
		 || nHandle == OBJECT_HANDLE_SELECT_ROW)
	{
		// Split handle (table row/column resize).
		int nIndex = (nHandle == OBJECT_HANDLE_SELECT_COL) ? 6 : 0;
		while (dAngle >= 45.0)
		{
			nIndex++;
			dAngle -= 45.0;
		}
		Cursor = SelectRCCursor[nIndex & 7];
	}
	else
	{
		// Normal resize handle.
		while (dAngle >= 45.0)
		{
			dAngle -= 45.0;
			nHandle = RotateHandle[nHandle];
		}
		Cursor = ResizeCursor[nHandle];
	}

	if (LOWORD((DWORD)Cursor) > 32000)
	{
		return AfxGetApp()->LoadStandardCursor(Cursor);
	}
	else
	{
		return AfxGetApp()->LoadCursor(Cursor);
	}
}

/*
// Process a right mouse button click.
*/

VOID CPmwView::process_right_button(POINT where)
{
	edit_state = STATE_RELEASE;			/* Ignore until release. */

/*
// This is right mouse event. Invoke the menu if we can.
// Compute the menu to use.
*/

	CPmwDoc *pDoc = GetDocument();
	PPNT p;

	rc.screen_to_ppnt(where, &p);

	SHORT type;
	PMGPageObjectPtr object = pDoc->selected_objects();

	BOOL multi = (object != NULL && object->next_selected_object() != NULL);

	for (type = 0; object != NULL; object = object->next_selected_object())
	{
		if (object->pt_in_object(p, &rc, NULL))
		{
			SHORT object_type = object->type();
			if (object_type != OBJECT_TYPE_Background && multi)
			{
				if (object_type == OBJECT_TYPE_Cell)
					type = OBJECT_TYPE_Cell;
				else if (object_type == OBJECT_TYPE_Table)
					type = OBJECT_TYPE_Table;
				else
					type = -1;
			}
			else
			{
				type = object_type;
			}
			break;
		}
	}

	SHORT index = -1;

	switch (type)
	{
		case -1:
		{
		/* Multi! */
			index = 0;
			break;
		}
		case OBJECT_TYPE_Frame:
		//case OBJECT_TYPE_Cell:
		{
			index = (edit_event_handler == mode_text_edit) ? 5 : 1;
			break;
		}
		case OBJECT_TYPE_Graphic:
		{
			index = 2;
			break;
		}
		case OBJECT_TYPE_Group:
		{
			index = 3;
			break;
		}
		case OBJECT_TYPE_CalendarDate:
		{
			index = 4;
			break;
		}
		case OBJECT_TYPE_Background:
		{
			ASSERT(FALSE);
			break;
		}
		case OBJECT_TYPE_Rectangle:
		case OBJECT_TYPE_Ellipse:
		case OBJECT_TYPE_Line:
		case OBJECT_TYPE_DrawingObject:
      case OBJECT_TYPE_Hyperlink:
		{
			index = 6;
			break;
		}
		case OBJECT_TYPE_OleObject:
		{
			index = 7;
			break;
		}
		case OBJECT_TYPE_Table:
		{
			index = 8;
			break;
		}
		case OBJECT_TYPE_Cell:
		{
			index = (edit_event_handler == mode_text_edit) ? 10 : 9;
			break;
		}
		case OBJECT_TYPE_Border:
		{
			index = 11;
			break;
		}
		case OBJECT_TYPE_Serif:
		{
			index = 12;
			break;
		}
		case OBJECT_TYPE_Component:
		{
			index = ((ComponentObject*) object)->GetContextMenuIndex();
			break;
		}
		default:
		{
			break;
		}
	}

	if (index != -1)
	{
		CMenu menu, *submenu;
		POINT menu_p = where;

		ClientToScreen(&menu_p);

		if (menu.LoadMenu(IDR_OBJECT_MENU))
		{
			if ((submenu = menu.GetSubMenu(index)) != NULL)
			{
#if 0
				AfxGetApp()->m_pMainWnd->SendMessage(WM_INITMENUPOPUP,
 										(WPARAM)submenu->GetSafeHmenu(),
										MAKELPARAM(0, FALSE));
#endif

				submenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					menu_p.x,
					menu_p.y,
					this);
			}
		}
	}
}

/*
// Release state.
// Wait for the user to release the mouse button.
*/

BOOL CPmwView::standard_state_release(EVENT_INFO *info)
{
	if (!(info->flags & (MK_LBUTTON | MK_RBUTTON)))
	{
	/* Done waiting! */
		edit_state = STATE_NORMAL;
	}
	return TRUE;
}

VOID CPmwView::resize_key(SHORT dx, SHORT dy, EVENT_INFO *info, PPNT p)
{
	CPmwDoc *pDoc = GetDocument();

   // first check if over Guide
   CRect rcUsable;
   GetUsableClientRect(&rcUsable);
   
   int index;
   int oldindex = -1;
   int newindex = -1;
   DWORD oldpos, newpos;
   int guideType;
   if (dy != 0 &&
       PtOnGuide(info->current_xy, GUIDE_AREA_MOVE, GUIDE_HORIZ, &index))
   {
      guideType = GUIDE_HORIZ;
      Array* pGuides = pDoc->get_horiz_guides();
      oldindex = index;
      oldpos = *((DWORD*)pGuides->get_element(oldindex));
      int screenypos = rc.page_y_to_screen(oldpos) + dy;
      if (screenypos >= rcUsable.top && screenypos < rcUsable.bottom)
      {
         DeleteGuide(GUIDE_HORIZ, oldindex);

         newindex = pGuides->count();
         newpos = oldpos + (dy*rc.redisplay_y_pixel);
         pGuides->add_element((void*)&newpos);

         CPoint pt(0, screenypos);
         DrawGuideLine(&rc, GUIDE_HORIZ, pt, TRUE);
      }
   }
   else if (PtOnGuide(info->current_xy, GUIDE_AREA_MOVE, GUIDE_VERT, &index)
            && dx != 0)
   {
      guideType = GUIDE_VERT;
      Array* pGuides = pDoc->get_vert_guides();
      oldindex = index;
      oldpos = *((DWORD*)pGuides->get_element(oldindex));
      int screenxpos = rc.page_x_to_screen(oldpos) + dx;
      if (screenxpos >= rcUsable.left && screenxpos < rcUsable.right)
      {
         DeleteGuide(GUIDE_VERT, index);

         newindex = pGuides->count();
         newpos = oldpos + (dx*rc.redisplay_x_pixel);
         pGuides->add_element((void*)&newpos);

         CPoint pt(screenxpos, 0);
         DrawGuideLine(&rc, GUIDE_VERT, pt, TRUE);
      }
   }
   // setup undo for guide move, if move was done
   if (newindex != -1)
   {
      CCmdGuideChange* pGuideUndo = new CCmdGuideChange(IDCmd_GuideMove);
      ASSERT(pGuideUndo);

      pGuideUndo->Setup(this, guideType,
                        pDoc->CurrentPageIndex(), pDoc->get_current_panel(),
                        oldpos, oldindex,
                        newpos, newindex);
      pDoc->AddCommand(pGuideUndo, FALSE);

      return;
   }

   PMGPageObjectPtr object;
	SHORT count = (SHORT)info->repeat_count;

	if ((object = pDoc->selected_objects()) != NULL)
	{
		switch (object->type())
		{
			case OBJECT_TYPE_Background:
			{
				break;
			}
			case OBJECT_TYPE_CalendarDate:
			{
				((CCalendarDoc*)pDoc)->ChangeDate((dy+dx)*count);
				break;
			}
			default:
			{
				CCmdChangeSelects* pCommand;
				BOOL fNewCommand = FALSE;

				if ((pCommand = (CCmdChangeSelects*)pDoc->UndoCommand()) == NULL
					 || pCommand->ID() != IDCmd_KbdMoveSelects
					 || pCommand->Closed())
				{
					fNewCommand = TRUE;
					pCommand = new CCmdChangeSelects(IDCmd_KbdMoveSelects);
					if (!pCommand->Before(this))
					{
						delete pCommand;
						pCommand = NULL;
					}
				}

			/* Move the object. */

				PPNT vector;
				vector.x = rc.redisplay_x_pixel * count * dx;
				vector.y = rc.redisplay_y_pixel * count * dy;

				// Get the refresh bound for later.
				pDoc->GetSelectRefreshBound(&original_refresh_bound, &rc);
				// Mark frames before the move.
				pDoc->MarkFramesBelowSelectedObjects();
				
				move_selected_objects(DRAG_STATE_begin, p, p, FALSE);

				p.x += vector.x;
				p.y += vector.y;

				move_selected_objects(DRAG_STATE_continue, vector, p);

				move_selected_objects(DRAG_STATE_end, p, p);

			/*
 			// Finish up the move...
 			*/

				pDoc->SetModifiedFlag();

			/* Do the calc. */

				pDoc->calc_selects(TRUE);

			/* In with the new... */

				PBOX bound;
				pDoc->GetSelectRefreshBound(&bound, &rc);
				pDoc->add_refresh(&bound);

			/* ...and out with the old. */

				pDoc->add_refresh(&original_refresh_bound);

			/* In case if goes off the edge of a banner. */

				pDoc->resize_project();

				// Handle anything else after a position change.
				AfterPositionChange();
				// Unmark frames marked before the move.
				pDoc->UpdateMarkedFrames(TRUE);
			/* Finish up the command. */
				if (pCommand != NULL)
				{
				/*
				// If After() fails, we need to get rid of the command.
				// If this is a new command, we can just free it.
				// If this is NOT a new command, then we just trashed the latest
				// undo command, and we need to remove it from the document.
				*/
					if (pCommand->After())
					{
						if (fNewCommand)
						{
							pDoc->AddCommand(pCommand, FALSE);
						}
					}
					else
					{
						if (fNewCommand)
						{
							// Just get rid of the new command.
							delete pCommand;
						}
						else
						{
							// What do we do here?
						}
					}
				}
				break;
			}
		}
	}
}

/*
// Normal state.
*/


BOOL CPmwView::resize_state_normal(EVENT_INFO *info)
{
	EVENT event = info->event;

	if (event == EVENT_BUTTON_DBLCLK && SHIFT_KEY(info->flags))
	{
	/* Ignore double-click if shift key is down. */
		event = EVENT_BUTTON_RISING;
	}

	// We only handle right button clicks in this mode.
	// If the right button is down, wait until it gets released.

	if ((info->flags & MK_RBUTTON) && event != EVENT_RBUTTON_RISING)
	{
		edit_state = STATE_RELEASE;
		return TRUE;
	}

	// Escape here means deselect everything.

	if (check_escape_event(info))
	{
		deselect_all();
		return TRUE;
	}

	PPNT p;
	rc.screen_to_ppnt(info->current_xy, &p);

	CPmwDoc *pDoc = GetDocument();

	// Commonly used case.

	BOOL button_rising = FALSE;

	switch (event)
	{
		case EVENT_EDIT_DELETE:
		{
			pDoc->delete_selects(FALSE);	
			return TRUE;
		}
		case EVENT_KEY_RISING:
		case EVENT_KEY_DOWN:
		{
			PMGPageObject* pObject = GetDocument()->selected_objects();
			BOOL fTableSelect = pObject != NULL
										&& pObject->type() == OBJECT_TYPE_Table
										&& ((CTableObject*)pObject)->SomethingSelected();
			switch (info->v_key)
			{
				case VK_UP:
				{
#if 0
					if (fTableSelect)
					{
						((CTableObject*)pObject)->SelectUp();
					}
					else
#endif
					{
						resize_key(0, -1, info, p);
					}
					break;
				}
				case VK_DOWN:
				{
#if 0
					if (fTableSelect)
					{
						((CTableObject*)pObject)->SelectDown();
					}
					else
#endif
					{
						resize_key(0, 1, info, p);
					}
					break;
				}
				case VK_LEFT:
				{
#if 0
					if (fTableSelect)
					{
						((CTableObject*)pObject)->SelectLeft();
					}
					else
#endif
					{
						resize_key(-1, 0, info, p);
					}
					break;
				}
				case VK_RIGHT:
				{
#if 0
					if (fTableSelect)
					{
						((CTableObject*)pObject)->SelectRight();
					}
					else
#endif
					{
						resize_key(1, 0, info, p);
					}
					break;
				}
				case VK_TAB:
				{
					if (SHIFT_KEY(info->flags))
					{
						// Back tab.
						OnPreviousObject();
					}
					else
					{
						// Tab.
						OnNextObject();
					}
					break;
				}
				default:
				{
//					od("Key %x pressed\r\n", info->v_key);
					break;
				}
			}
			break;
		}
		case EVENT_BUTTON_RISING:
		{
			SetFocus();
			button_rising = TRUE;
			break;
		}
		default:
		{
			break;
		}
	}

   //
   // Check if we're over a Guide (if guides are on!)
   //
   if (GET_PMWAPP()->ShowGuides() && !GET_PMWAPP()->GuidesLocked())
   {
      CPoint pt;
      rc.ppnt_to_screen(p, &pt);

      if (PtOnGuide(info->current_xy, GUIDE_AREA_MOVE, GUIDE_HORIZ, &m_GuideIndex))
         m_GuideType = GUIDE_HORIZ;
      else if (PtOnGuide(info->current_xy, GUIDE_AREA_MOVE, GUIDE_VERT, &m_GuideIndex))
         m_GuideType = GUIDE_VERT;

      if (m_GuideIndex != -1)
      {
         // we're over a guide
         if (button_rising)
         {
            // mouse clicked on guide -- start moving state
            DrawGuideLine(&rc, m_GuideType, pt);
            m_LastGuidePt = pt;

            rc.register_toggle(guide_toggle, this);
            edit_state = RESIZE_STATE_MOVING_GUIDE;
         }
         else
         {
            // show "move guide" cursor
            HCURSOR hCursor;
            if (m_GuideType == GUIDE_HORIZ)
               hCursor = AfxGetApp()->LoadCursor(IDC_MOVE_HGUIDE);
            else
               hCursor = AfxGetApp()->LoadCursor(IDC_MOVE_VGUIDE);
            set_cursor(hCursor);
         }
         return TRUE;
      }
   }

	PBOX pbox;
	PMGPageObject* pObject;
	PMGPageObject* pHandleObject;
	PMGPageObject* pMoveObject;
	PMGPageObject* pOnObject;
	OBJECT_HANDLE handle;
	OBJECT_HANDLE test_handle;


	rc.mouse_box(info->current_xy, &pbox);

	pHandleObject = pOnObject = pMoveObject = NULL;

	
	//
	// Look through the list of selected objects, and see if the cursor
	// is over any handles or over the objects themselves.
	// Selected objects take precedence over non-selected objects.
	//

	for (pObject = pDoc->selected_objects();
		  pObject != NULL;
		  pObject = pObject->next_selected_object())
	{
		
		PBOX bound = pObject->get_bound();

		if (!(pObject->get_flags() & OBJECT_FLAG_locked)
			 && !pDoc->IsGroupedFrame(pObject))
		{
			// See if we are over a handle.
			if ((pObject->get_select_flags() & SELECT_FLAG_size_handles)
						&& (test_handle = pObject->box_on_handle(&pbox, &rc)) != OBJECT_HANDLE_NONE)
			{
				if (test_handle != OBJECT_HANDLE_MOVE)
				{
					pHandleObject = pObject;
					handle = test_handle;
				}
			}

			// See if we are over the object.
			PBOX refresh_bound;

			pObject->get_refresh_bound(&refresh_bound, &rc);
			UnionBox(&refresh_bound, &refresh_bound, &bound);

			if (IsBoxOnBox(&pbox, &refresh_bound) && pObject->pt_in_object(p, &rc, NULL))
			{
				pOnObject = pObject;
			}
			else if (pObject->pt_on_move_area(p, &rc))
			{
				pOnObject = pMoveObject = pObject;
			}
			
			if (test_handle == OBJECT_HANDLE_MOVE) // special for table objects only
			{
				pOnObject = pMoveObject = pObject;
				set_cursor(GetResizeCursor(pObject, test_handle));

			}
	
		}
	}

	//
	// Now, check the results. First, we check the handle case.
	//

	if (pHandleObject != NULL)
	{
		// Set the resize cursor.
		set_cursor(GetResizeCursor(pHandleObject, handle));

		// Handle a button press on the handle.
		if (info->flags & MK_LBUTTON)
//		if (button_rising)
		{
			// We just grabbed it!
			//if (handle = OBJECT_HANDLE_MOVE)
			//	edit_state = RESIZE_STATE_MOVING;
			//else

			switch (handle)
			{
				case OBJECT_HANDLE_SELECT_COL:
				{
					edit_state = RESIZE_STATE_SELECT_COL;
					select_start = p;
					CTableObject* pTable = (CTableObject*)pHandleObject;
					int nColumn = pTable->ColumnOfPoint(select_start);
					pTable->SelectColumns(nColumn, nColumn);
					break;
				}
				case OBJECT_HANDLE_SELECT_ROW:
				{
					edit_state = RESIZE_STATE_SELECT_ROW;
					select_start = p;
					CTableObject* pTable = (CTableObject*)pHandleObject;
					int nRow = pTable->RowOfPoint(select_start);
					pTable->SelectRows(nRow, nRow);
					break;
				}
				default:
				{
					edit_state = RESIZE_STATE_HANDLE;
					break;
				}
			}

			// Begin moving the handle.
			current_handle = handle;
			current_object = pHandleObject;

			if (edit_state == RESIZE_STATE_HANDLE)
			{
				UpdateFrameButtons();

				// Save the initial bounds.
				original_bound = pHandleObject->get_bound();
				original_angle = pHandleObject->get_rotation();
				pHandleObject->get_refresh_bound(&original_refresh_bound, &rc);

				// Create the undo/redo command for this action.
				if (m_pCommand != NULL)
				{
					od("m_pCommand is not NULL!\n");
					delete m_pCommand;
				}
				m_pCommand = new CCmdChangeSelects(IDCmd_ResizeSelects);

				if (!m_pCommand->Before(this))
				{
					delete m_pCommand;
					m_pCommand = NULL;
				}

				// Mark frames below this object.
				pDoc->MarkFramesBelow(current_object);

				LPVOID data = current_object;
				PMG_OBJECT_ACTION action;

				// Move the handle.
				action = current_object->move_handle(DRAG_STATE_begin, p, current_handle, info->flags, &data);

				// Handle any action spawned by this move.
				if (action != ACTION_DO_NOTHING)
				{
					pDoc->process_action(this, action, current_object, data, p);
				}
			}
		}
		return TRUE;
	}

	//
	// We aren't over a handle. See if we're over an object.
	//

	if (pOnObject != NULL)
	{
		HCURSOR hCursor;

		if ((pOnObject->get_select_flags() & SELECT_FLAG_move_handle)
					&& pOnObject->type() != OBJECT_TYPE_Table)
		{
			// We can indeed move it. Show the appropriate cursor.
			if (pMoveObject == pOnObject
			 || (hCursor = pOnObject->GetContentCursor()) == NULL)
			{
				#ifdef WIN32
   				hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
				#else
	  				hCursor = AfxGetApp()->LoadCursor(IDC_FOURARROW_CURSOR);
				#endif
			}
		}
		else
		{
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}

		if (test_handle != OBJECT_HANDLE_MOVE) // cursor for this was already set above, in table case only
			set_cursor(hCursor);

		// Get the primary action.
		LPVOID data = pOnObject;
		PMG_OBJECT_ACTION action;
		if (pMoveObject == pOnObject)
		{
			action = ACTION_SELECT;
		}
		else
		{
			action = pOnObject->primary_action(&data);
		}

		BOOL selecting = (action == ACTION_SELECT || action == ACTION_SELECT_DATE);

		if (info->flags & (MK_LBUTTON | MK_RBUTTON))
		{
			if (event == EVENT_BUTTON_DBLCLK)
			{
				// Double click! Secondary action time!
				// Process the secondary action.

				LPVOID data = pOnObject;
				PMG_OBJECT_ACTION action = pOnObject->secondary_action(&data);

				pDoc->process_action(this, action, pOnObject, data, p);
			}
			else if (action == ACTION_SELECT_CELLS && event != EVENT_RBUTTON_RISING)
			{
				// Select the first cell.
				CTableObject* pTable = (CTableObject*)pOnObject;
				CPoint cpHere = pTable->CellOfPoint(p);
				pTable->SetSelect(CRect(cpHere, cpHere));

				// And move to the "select cells" state.
				select_start = p;
				current_object = pOnObject;
				edit_state = RESIZE_STATE_SELECT_CELLS;
				return TRUE;
			}
			else if (action == ACTION_DEFAULT_EDIT
							&& pOnObject->type() == OBJECT_TYPE_Table
							&& event != EVENT_RBUTTON_RISING)
			{
				// Begin editing the cell in the table.
				CTableObject* pTable = (CTableObject*)pOnObject;
				CPoint cpHere = pTable->CellOfPoint(p);
				CFrameObject* pFrame = pTable->GetCellFrame(cpHere);
				current_object = pOnObject;
				if (pFrame != NULL)
				{
					pTable->DeselectAll();
					edit_frame(pFrame, &p);
				}
			}
			else if (button_rising && SHIFT_KEY(info->flags))
			{
				// Toggle off! Also remember this object so that we can cycle
				// through objects if necessary.
				m_pLastSelectedObject = pOnObject;
				deselect_object(pOnObject);

				edit_state = STATE_RELEASE;
			}
			else if (event == EVENT_RBUTTON_RISING)
			{
				process_right_button(info->current_xy);
			}
			else if (!selecting)
			{
				// Over the object content area. Handle the action.
				pDoc->process_action(this, action, pOnObject, data, p);
			}
			else if ((pOnObject->get_select_flags() & (SELECT_FLAG_move_handle | SELECT_FLAG_drag_handle)))
			{
				// We just grabbed it!
				current_object = pOnObject;
				current_handle = OBJECT_HANDLE_MOVE;

				// Save the initial bounds.
				GetDocument()->GetSelectBound(&original_bound);
				GetDocument()->GetSelectRefreshBound(&original_refresh_bound);

				// Compute grab location relative to upper corner.
				grab_offset.x = p.x - original_bound.x0;
				grab_offset.y = p.y - original_bound.y0;

				// Create the undo/redo command for this action.
				if (m_pCommand != NULL)
				{
					od("m_pCommand is not NULL!\n");
					delete m_pCommand;
				}
				m_pCommand = new CCmdChangeSelects(IDCmd_MoveSelects);

				if (!m_pCommand->Before(this))
				{
					delete m_pCommand;
					m_pCommand = NULL;
				}

				// Begin moving the object.
				edit_state = RESIZE_STATE_MOVING;
				UpdateFrameButtons();

				// Mark frames before the move.
				pDoc->MarkFramesBelowSelectedObjects();

				// Initiate moving. Send the "begin" movement.
				move_selected_objects(DRAG_STATE_begin, p, p);

				// Verify that we moved within ourselves.
				if (!m_fLocalDragDrop)
				{
					/* We dropped outside the application, so signal an abort of
					 *	handle movement.
					 */

					move_selected_objects(DRAG_STATE_abort, p, p);

					FLAGS flags = current_object->get_flags();

					if (flags & OBJECT_FLAG_needs_update)
					{
						pDoc->add_refresh(&original_bound);
					}

					edit_state = STATE_RELEASE;
				}
			}
			else
			{
				// Not able to do anything to this object.
				// Wait for a release.
				edit_state = STATE_RELEASE;
			}
		}
		return TRUE;
	}

	//
	// We are not over a selected object (handle or object proper).
	//

	set_cursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	// If the user clicks with the CONTROL key pressed, start a select box.
	// Otherwise, look for the object we are over.

	if (!(button_rising && CTRL_KEY(info->flags)))
	{
		// Not forcing a drag box. See if we are on any objects.

		for (pObject = pDoc->objects();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
			PMGPageObject* pSubObject;

			if (pObject->pt_in_object(p, &rc, &pSubObject)
               && !(pSubObject->get_flags() & OBJECT_FLAG_locked)
				   && !pDoc->IsGroupedFrame(pSubObject))
			{
 				pOnObject = pSubObject;
 			}
			else if (pObject->pt_on_move_area(p, &rc)
               && !(pObject->get_flags() & OBJECT_FLAG_locked)
				   && !pDoc->IsGroupedFrame(pObject))
			{
				pOnObject = pMoveObject = pObject;
			}
		}
	}

	// We have finished looking through the non-selected objects.
	// See if we found one.

	if (pOnObject != NULL)
	{
		if (button_rising || event == EVENT_RBUTTON_RISING)
		{
			// Clicked on an object!
			if (SHIFT_KEY(info->flags))
			{
				// The user has shift-clicked on an object.
				// We want to cycle through objects if the user has been
				// shift-clicking. Otherwise, we just select the object without
				// deselecting any other.

				if (m_pLastSelectedObject != NULL)
				{
					PMGPageObjectPtr high_object = pOnObject;

					pOnObject = NULL;

					for (pObject = pDoc->objects();
								pObject != NULL;
								pObject = (PMGPageObjectPtr)pObject->next_object())
					{
						PMGPageObjectPtr sub_object;

						if (pObject->pt_in_object(p, &rc, &sub_object)
								&& !(sub_object->get_flags() & OBJECT_FLAG_locked)
								&& !pDoc->IsGroupedFrame(sub_object))
						{
							if (sub_object == m_pLastSelectedObject)
							{
								// Hit original object.
								break;
							}
							pOnObject = sub_object;
						}
					}

					if (pOnObject == NULL)
					{
						// Nothing below. Start over at the top.
						pOnObject = high_object;
					}
				}
			}
			else
			{
				// The shift key is not down while clicking on an object
				// We need to deselect all objects prior to acting on the new one.
				deselect_all();
			}

			PMGPageObjectPtr first_selected_object = pDoc->selected_objects();

			if (pOnObject == first_selected_object)
			{
				// Don't reselect currently selected object.
				// This should only happen with backgrounds.

				ASSERT(FALSE);		// We don't have backgrounds anymore.
				return TRUE;
			}

			// We want to do special stuff for this.
			// Particularly, we want to allow right clicks on text boxes
			// (whose primary action is *not* select)
			if (event == EVENT_RBUTTON_RISING)
			{
				if (!(pOnObject->get_select_flags() & SELECT_FLAG_solo_select))
				{
					pMoveObject = pOnObject;
				}
			}

			/* Process the object's primary action. */
			LPVOID data = pOnObject;
			PMG_OBJECT_ACTION action;

			if (pMoveObject == pOnObject)
			{
				action = ACTION_SELECT;
			}
			else
			{
				action = pOnObject->primary_action(first_selected_object == NULL
																? &data
																: NULL);
			}

			BOOL selecting = (action == ACTION_SELECT || action == ACTION_SELECT_DATE);

			if (event == EVENT_RBUTTON_RISING && !selecting)
			{
			/* Not a good object. It doesn't select. */
				edit_state = STATE_RELEASE;
			}
			else
			{
				pDoc->process_action(this, action, pOnObject, data, p);

				m_pLastSelectedObject = NULL;
				if (event == EVENT_RBUTTON_RISING)
				{
					process_right_button(info->current_xy);
				}
				else if (selecting)
				{
				/* Force us into the "move object" state. */
					signal_mouse_event(MK_LBUTTON, info->current_xy, EVENT_MOUSE_MOVE);
				}
			}
		}
		return TRUE;
	}

	// Not on anything in particular.
	// If the button is down, wait until they release.

	if (button_rising)
	{
		// The user has just clicked the button. Begin dragging out a select box.
		select_start = p;
		edit_state = RESIZE_STATE_SELECT;

		// Normally, we want to deselect all the currently selected objects.
		// If the shift key is down, we extend the selection (we don't deselect).
		if (!SHIFT_KEY(info->flags))
		{
			deselect_all();
		}
	}
	else if (info->flags & MK_LBUTTON)
	{
		// The button is down, but it didn't just go down.
		edit_state = STATE_RELEASE;
	}

	return FALSE;
}

/*
// Initialization state.
*/

BOOL CPmwView::resize_state_init(EVENT_INFO *info)
{
	set_cursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	edit_state = STATE_NORMAL;

	return resize_state_normal(info);				/* Do it now, too. */
}

/*
// Grabbed and moving a handle state.
*/

BOOL CPmwView::resize_state_handle(EVENT_INFO *info)
{
	BOOL stop_now = check_escape_event(info);
	PPNT p = {0, 0};
	CPmwDoc *pDoc = GetDocument();

	if (!stop_now && (info->flags & MK_LBUTTON))
	{
		if (info->new_position)
		{
			// Mouse moved.
			rc.screen_to_ppnt(info->current_xy, &p);
			if (current_handle != OBJECT_HANDLE_ROTATE)
			{
				SnapPosition(&p);
			}

			// Move the handle.
			LPVOID data = current_object;
			PMG_OBJECT_ACTION action;

			action = current_object->move_handle(DRAG_STATE_continue, p, current_handle, info->flags, &data);
			if (action != ACTION_DO_NOTHING)
			{
				pDoc->process_action(this, action, current_object, data, p);
			}
		}
	}
	else
	{
	/*
	// Do we turn the move handle back on before or after the Calc method is
	// invoked? Hmmm..., I wonder.
	*/

		VOIDPTR data;
		PMG_OBJECT_ACTION action;
		BOOL fDoIntersect = FALSE;

		if (stop_now)
		{
		/* Signal an abort of handle movement. */
			action = current_object->move_handle(DRAG_STATE_abort, p, current_handle, info->flags, &data);
		}
		else
		{
		/* Signal an end of handle movement. */
			action = current_object->move_handle(DRAG_STATE_end, p, current_handle, info->flags, &data);
		}

		if (action != ACTION_DO_NOTHING)
		{
			pDoc->process_action(this, action, current_object, data, p);
		}

	/* See if we moved or sized at all. */

		BOOL redraw = TRUE;
		PBOX bound = current_object->get_bound();

		if (memcmp(&original_bound,
							&bound,
							sizeof(PBOX)) != 0
			|| original_angle != current_object->get_rotation()
			|| (current_object->get_flags() & OBJECT_FLAG_needs_calc))
		{
			pDoc->SetModifiedFlag();

		/* Force manual layout. */

			current_object->set_new_layout(MANUAL_LAYOUT);

		/* Do the calc. */

			current_object->calc();
			current_object->changed_image();				// Always force this.

		/* Out with the old... */

			pDoc->add_refresh(&original_refresh_bound);

		/* ...and in with the new. */

			current_object->get_refresh_bound(&bound, &rc);
			pDoc->add_refresh(&bound);

			redraw = FALSE;

		/* In case if goes off the edge of a banner. */

			pDoc->resize_project();
		/* Set up for undo/redo. */

			if (m_pCommand != NULL && m_pCommand->After())
			{
			/* Add the command for undoing later. */
			/* Don't execute! */
				pDoc->AddCommand(m_pCommand, FALSE);
				m_pCommand = NULL;
			}

			fDoIntersect = TRUE;
			// Mark frames below this object.
			pDoc->MarkFramesBelow(current_object);

			// Handle anything else after a position change.
			AfterPositionChange();
		}
		else
		{
			FLAGS flags = current_object->get_flags();
			current_object->set_flags(flags & ~OBJECT_FLAG_needs_calc);

			if (flags & OBJECT_FLAG_needs_update)
			{
				pDoc->add_refresh(&bound);
			}
		}

		// Handle marked frames.
		pDoc->UpdateMarkedFrames(fDoIntersect);

		edit_state = stop_now ? STATE_RELEASE : STATE_NORMAL;

	/* Clean up any remaining command. */

		if (m_pCommand != NULL)
		{
			delete m_pCommand;
			m_pCommand = NULL;
		}

#if 0
	/* Inform that we've stopped moving the handle. */

		do_inform(INFORM_STOP_MOVING_HANDLE, (MSG_DATA)resize_context.object);
#endif
	}
	return TRUE;
}

/*
// Move all selected objects.
*/

VOID CPmwView::move_selected_objects(DRAG_STATE state, PPNT vector, PPNT current_xy, bool fAllowDrag /* = TRUE */ )
{
	CPmwDoc *pDoc = GetDocument();

/* Move the objects. */

	VOIDPTR data;
	SHORT count = 0;
	PMG_OBJECT_ACTION action = ACTION_DO_NOTHING;

	for (PMGPageObjectPtr object = pDoc->selected_objects();
					object != NULL;
					object = object->next_selected_object())
	{
		data = object;
		action = object->move_object(state, vector, current_xy, &data);
		count++;
	}

/* Process the action. */

	if (count == 1 && action != ACTION_DO_NOTHING)
	{
		pDoc->process_action(this, action, object, data, current_xy);
	}

	if ((state == DRAG_STATE_begin) && fAllowDrag)
	{
		InitiateDrag();
	}
}

/*
// Grabbed and moving an object.
*/

BOOL CPmwView::resize_state_moving(EVENT_INFO *info)
{
	CPmwDoc *pDoc = GetDocument();
	BOOL stop_now = check_escape_event(info);
	PPNT p;

	rc.screen_to_ppnt(info->current_xy, &p);

	if (!stop_now && (info->flags & MK_LBUTTON))
	{
		if (info->new_position)
		{
		/* Mouse moved. */

			PBOX bound;    
			pDoc->GetSelectBound(&bound);

			PPNT OriginalGrabPoint;
			OriginalGrabPoint.x = original_bound.x0 + grab_offset.x;
			OriginalGrabPoint.y = original_bound.y0 + grab_offset.y;

			PPNT ThisGrabPoint;
			ThisGrabPoint.x = p.x;
			ThisGrabPoint.y = p.y;

			if (CTRL_KEY(info->flags))
			{
				// We want constrained movement.
				PPNT Delta;
				Delta.x = ThisGrabPoint.x - OriginalGrabPoint.x;
				if (Delta.x < 0)
				{
					Delta.x = -Delta.x;
				}
				Delta.y = ThisGrabPoint.y - OriginalGrabPoint.y;
				if (Delta.y < 0)
				{
					Delta.y = -Delta.y;
				}
				if (Delta.x < Delta.y)
				{
					// X distance is smaller. Snap y.
					p.x = OriginalGrabPoint.x;
				}
				else
				{
					p.y = OriginalGrabPoint.y;
				}
			}

			PPNT vector;
			vector.x = p.x - (grab_offset.x + bound.x0);
			vector.y = p.y - (grab_offset.y + bound.y0);

			bound.x0 += vector.x;
			bound.y0 += vector.y;
			bound.x1 += vector.x;
			bound.y1 += vector.y;

         PPNT SnappedUL;
			SnappedUL.x = bound.x0;
			SnappedUL.y = bound.y0;
			int guideSnap = SnapPosition(&SnappedUL);
         BOOL fSnapGuideUL_X = guideSnap & GUIDE_VERT;
         BOOL fSnapGuideUL_Y = guideSnap & GUIDE_HORIZ;

			PPNT DeltaUL;
			if ((DeltaUL.x = SnappedUL.x - bound.x0) < 0)
				DeltaUL.x = -DeltaUL.x;
			if ((DeltaUL.y = SnappedUL.y - bound.y0) < 0)
				DeltaUL.y = -DeltaUL.y;

			PPNT SnappedLR;
			SnappedLR.x = bound.x1;
			SnappedLR.y = bound.y1;
			guideSnap = SnapPosition(&SnappedLR);
         BOOL fSnapGuideLR_X = guideSnap & GUIDE_VERT;
         BOOL fSnapGuideLR_Y = guideSnap & GUIDE_HORIZ;

			PPNT DeltaLR;
			if ((DeltaLR.x = SnappedLR.x - bound.x1) < 0)
				DeltaLR.x = -DeltaLR.x;
			if ((DeltaLR.y = SnappedLR.y - bound.y1) < 0)
				DeltaLR.y = -DeltaLR.y;

         if ( (!fSnapGuideUL_X && !fSnapGuideLR_X) ||
              ( fSnapGuideUL_X &&  fSnapGuideLR_X) )
         {
            // If no vertical guides snapped, resolve ruler conflict.
            // If both vertical guides snapped, resolve guide conflict.
            if (DeltaLR.x < DeltaUL.x)
				   vector.x += SnappedLR.x - bound.x1;
			   else
				   vector.x += SnappedUL.x - bound.x0;
         }			   
         else  // snap to whichever vertical guide was hit
         {
            if (fSnapGuideUL_X)
               vector.x += SnappedUL.x - bound.x0;
            else // fSnapGuideLR_X
               vector.x += SnappedLR.x - bound.x1;
         }

         if ( (!fSnapGuideUL_Y && !fSnapGuideLR_Y) ||
              ( fSnapGuideUL_Y &&  fSnapGuideLR_Y) )
         {
            if (DeltaLR.y < DeltaUL.y)
				   vector.y += SnappedLR.y - bound.y1;
			   else
				   vector.y += SnappedUL.y - bound.y0;
         }
         else
         {
            if (fSnapGuideUL_Y)
               vector.y += SnappedUL.y - bound.y0;
            else // fSnapGuideLR_Y
               vector.y += SnappedLR.y - bound.y1;
         }

         // Move all objects.
			move_selected_objects(DRAG_STATE_continue, vector, p);
		}
	}
	else
	{
	/*
	// Do we turn resize handles back on before or after the Calc method is
	// invoked? Hmmm..., I wonder.
	*/

	/* If things moved, refresh both. */

		BOOL fDoIntersect = FALSE;

		if (stop_now)
		{
			/* Signal an abort of handle movement. */

			move_selected_objects(DRAG_STATE_abort, p, p);

			FLAGS flags = current_object->get_flags();

			if (flags & OBJECT_FLAG_needs_update)
			{
				pDoc->add_refresh(&original_bound);
			}
		}
		else
		{
			if (current_object->get_select_flags() & SELECT_FLAG_move_handle)
			{
				PBOX bound;

	 			pDoc->GetSelectBound(&bound);

			/* Signal the end of moving. */

				move_selected_objects(DRAG_STATE_end, p, p);

				if (memcmp(&original_bound,
									&bound,
									sizeof(PBOX)) != 0)
				{
					pDoc->SetModifiedFlag();

				/* Do the calc. */

					pDoc->calc_selects(TRUE);

				/* In with the new... */

					pDoc->GetSelectRefreshBound(&bound);
					pDoc->add_refresh(&bound);

				/* ...and out with the old. */

					pDoc->add_refresh(&original_refresh_bound);

				/* In case if goes off the edge of a banner. */

					pDoc->resize_project();

				/* Set up for undo/redo. */

					if (m_pCommand != NULL && m_pCommand->After())
					{
					/* Add the command for undoing later. */
					/* Don't execute! */
						pDoc->AddCommand(m_pCommand, FALSE);
					}
					else
					{
						delete m_pCommand;
					}
					m_pCommand = NULL;

					// Handle frame intersections.
					pDoc->MarkFramesBelowSelectedObjects();

					// Handle anything else after a position change.
					AfterPositionChange();
					fDoIntersect = TRUE;
				}
				else
				{
				/* Get rid of our undo command. */
					FLAGS flags = current_object->get_flags();

					current_object->set_flags(flags & ~OBJECT_FLAG_needs_calc);

					if (flags & OBJECT_FLAG_needs_update)
					{
						pDoc->add_refresh(&bound);
					}
				}
			}
			else
			{
				FLAGS flags = current_object->get_flags();

			/*
			// Must be a drag.
			// If this is a calendar date, we need to establish our undo command.
			*/

				CCommand* pCommand = NULL;

				switch (current_object->type())
				{
					case OBJECT_TYPE_CalendarDate:
					{
						CalendarObjectPtr pCalObj = (CalendarObjectPtr)current_object->get_container();
						SHORT nPanel;
						PMGPageObjectPtr pDestObject = pCalObj->FindDate(p, &nPanel);

						if (pDestObject != NULL
								&& nPanel != current_object->get_panel() - DATE_PANEL)
						{
						/* A drag will occur. Create the undo command. */
							pCommand = new CCmdCalendarDate(IDCmd_CopyDate);
							if (!((CCmdCalendarDate*)pCommand)->Before(pDoc, pCalObj, nPanel))
							{
							/*
							// Failed to setup. Forget trying.
							*/
								delete pCommand;
								pCommand = NULL;
							}
						}

						break;
					}
					default:
					{
						break;
					}
				}

			/*
			// Move the object back to where it was.
			*/

				move_selected_objects(DRAG_STATE_end_drag, p, p);

			/*
			// Handle the undo command now if there is one.
			*/

				switch (current_object->type())
				{
					case OBJECT_TYPE_CalendarDate:
					{
						if (pCommand != NULL)
						{
							if (((CCmdCalendarDate*)pCommand)->After())
							{
								pDoc->AddCommand(pCommand, FALSE);
							}
							else
							{
								delete pCommand;
							}
						}
						break;
					}
					default:
					{
						break;
					}
				}

				if (flags & OBJECT_FLAG_needs_update)
				{
					pDoc->add_refresh(&original_bound);
				}
			}
		}

		// Unmark frames marked before the move.
		pDoc->UpdateMarkedFrames(fDoIntersect);

		edit_state = stop_now ? STATE_RELEASE : STATE_NORMAL;

	/* Clean up any remaining undo command. */

		if (m_pCommand != NULL)
		{
			delete m_pCommand;
			m_pCommand = NULL;
		}

#if 0
	/* Inform that we've stopped moving. */

		do_inform(INFORM_STOP_MOVING_OBJECT, (MSG_DATA)resize_context.object);
#endif
	}
	return TRUE;
}

BOOL CPmwView::marquee_toggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data)
{
	Marquee *marquee = (Marquee *)data;

	if (turn_on)
	{
		marquee->draw(rc->destination_hdc);
	}
	else
	{
		marquee->undraw(rc->destination_hdc);
	}
	return TRUE;
}

/*
// Drag out a select box.
*/

BOOL CPmwView::resize_state_select(EVENT_INFO *info)
{
	BOOL stop_now = check_escape_event(info);

/* Compute where we are. */

	PPNT p;

	rc.screen_to_ppnt(info->current_xy, &p);

	if (!stop_now && (info->flags & MK_LBUTTON))
	{
		if (info->new_position)
		{
			RECT r;

		/* Mouse moved. */

			if (marquee == NULL)
			{
				if ((marquee = new Marquee(GetSafeHwnd())) == NULL)
				{
					edit_state = STATE_RELEASE;
					return TRUE;
				}
				rc.register_toggle(marquee_toggle, marquee);
			}

		/* We have the marquee now. */

		/* Construct the rectangle for it. */

			r.left = info->current_xy.x;
			r.top = info->current_xy.y;
			r.right = rc.page_x_to_screen(select_start.x);
			r.bottom = rc.page_y_to_screen(select_start.y);

		/* Normalize. */

			if (r.left > r.right)
			{
				int tmp = r.left;
				r.left = r.right;
				r.right = tmp;
			}

			if (r.top > r.bottom)
			{
				int tmp = r.top;
				r.top = r.bottom;
				r.bottom = tmp;
			}

		/* Update the rectangle if it changed. */

			if (!EqualRect(&marquee->r, &r))
			{
				marquee->undraw();
				marquee->r = r;
				marquee->draw();
			}
		}
	}
	else
	{
		if (marquee != NULL)
		{                             
			PBOX pbox;

		/* Get rid of the marquee. */

			rc.remove_toggle(marquee_toggle);
			marquee->undraw();

			delete marquee;
			marquee = NULL;

			if (!stop_now)
			{
			/* See what objects we need to select. */

				if ((pbox.x0 = p.x) > (pbox.x1 = select_start.x))
				{
					pbox.x0 = pbox.x1;
					pbox.x1 = p.x;
				}

				if ((pbox.y0 = p.y) > (pbox.y1 = select_start.y))
				{
					pbox.y0 = pbox.y1;
					pbox.y1 = p.y;
				}

			/* Select all objects within this box. */

				GetDocument()->select_all(&pbox);
			}
		}

	/* Go back to normal state. */

		edit_state = STATE_NORMAL;
	}
	return TRUE;
}

BOOL CPmwView::guide_toggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data)
{
   CPmwView* pView = (CPmwView*)data;
   pView->DrawGuideLine(rc, pView->m_GuideType, pView->m_LastGuidePt);

   return TRUE;
}

BOOL CPmwView::resize_state_moving_guide(EVENT_INFO *info)
{
	CPmwDoc *pDoc = GetDocument();

	BOOL stop_now = check_escape_event(info);

   // need to convert current_xy to page and back or else off-by-one :-(
   PPNT pagePos;
   CPoint pt;
   rc.screen_to_ppnt(info->current_xy, &pagePos);
   SnapPosition(&pagePos); // snap to rulers, if necessary
   rc.ppnt_to_screen(pagePos, &pt);

   if (!stop_now && info->event == EVENT_BUTTON_FALLING)
   {
      // finished dragging guide...

      edit_state = STATE_NORMAL;
      rc.remove_toggle(guide_toggle);

      Array* pGuides = (m_GuideType == GUIDE_HORIZ) ?
         pDoc->get_horiz_guides() : pDoc->get_vert_guides();

      CRect rcUsable;
      GetUsableClientRect(&rcUsable);

      DWORD oldpos = 0;
      if (m_GuideIndex != -1)
      {
         // we're moving an existing guide
         oldpos = *((DWORD*)pGuides->get_element(m_GuideIndex));

         // Remove guideline from original position
         DeleteGuide(m_GuideType, m_GuideIndex);
      }

      // add guide to array if we're within the view, otherwise drop on floor
      int newindex = -1;
      DWORD newpos;
      if ((m_GuideType == GUIDE_HORIZ && (pt.y >= rcUsable.top && pt.y < rcUsable.bottom)) ||
          (m_GuideType == GUIDE_VERT && (pt.x >= rcUsable.left && pt.x < rcUsable.right)) )
      {
         newpos = (m_GuideType == GUIDE_HORIZ) ? pagePos.y : pagePos.x;
         newindex = pGuides->count();
         pGuides->add_element((void*)&newpos);
         DrawGuideLine(&rc, m_GuideType, pt, ShowGuides() ? TRUE : FALSE);
         pDoc->SetModifiedFlag();
      }

      // Create the undo/redo command for this action, if appropriate
      if (m_GuideIndex != -1 || newindex != -1)
      {
         int undoID = IDCmd_GuideMove;
         if (m_GuideIndex == -1)
            undoID = IDCmd_GuideCreate;
         else if (newindex == -1)
            undoID = IDCmd_GuideDelete;

         CCmdGuideChange* pGuideUndo = new CCmdGuideChange(undoID);
         ASSERT(pGuideUndo);

         pGuideUndo->Setup(this, m_GuideType,
                           pDoc->CurrentPageIndex(), pDoc->get_current_panel(),
                           oldpos, m_GuideIndex,
                           newpos, newindex);

         pDoc->AddCommand(pGuideUndo, FALSE);
      }
   }
	else if (stop_now)
   {
      // cancel guide dragging
      rc.remove_toggle(guide_toggle);
      DrawGuideLine(&rc, m_GuideType, m_LastGuidePt);
      edit_state = STATE_RELEASE;
   }
   else // we're dragging a guide
   {
      DrawGuideLine(&rc, m_GuideType, m_LastGuidePt);
      DrawGuideLine(&rc, m_GuideType, pt);
      m_LastGuidePt = pt;
   }

   return TRUE;
}

/*
// The states for the resize mode.
*/

CPmwView::STATE_HANDLER CPmwView::resize_state[] =
{
	CPmwView::resize_state_init,
	CPmwView::resize_state_normal,
	CPmwView::standard_state_release,

	CPmwView::resize_state_handle,
	CPmwView::resize_state_moving,
	CPmwView::resize_state_select,
	CPmwView::resize_state_moving_guide,
	CPmwView::resize_state_select_cell,
	CPmwView::resize_state_resize_table_col,
	CPmwView::resize_state_resize_table_row,
	CPmwView::resize_state_select_table_col,
	CPmwView::resize_state_select_table_row
};

#define	RESIZE_STATE_COUNT			(sizeof(resize_state)/sizeof(resize_state[0]))

/*
// The resize mode handler.
*/

VOID CPmwView::mode_resize(EVENT_INFO *info)
{
	if (info->event >= EVENT_ENTERING_MODE)
	{
		// Special mode event.
		return;
	}

	// Process a normal event.
	SHORT state = edit_state;

	if (state > STATE_BASE_END)
	{
		if (state >= RESIZE_STATE_BASE)
		{
		/* One of our special states! */
			state -= (RESIZE_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = RESIZE_STATE_COUNT;
		}
	}

	if (state < RESIZE_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*resize_state[state])(info);
	}
}

BOOL CPmwView::resize_state_select_cell(EVENT_INFO *info)
{
	if (info->event == EVENT_MOUSE_MOVE)
	{
		if (info->new_position)
		{
			CTableObject* pTable = (CTableObject*)current_object;
			ASSERT(pTable->type() == OBJECT_TYPE_Table);

			CPoint cpStart = pTable->CellOfPoint(select_start);
			PPNT p;
			p.x = rc.screen_x_to_page(info->current_xy.x);
			p.y = rc.screen_y_to_page(info->current_xy.y);
			CPoint cpEnd = pTable->CellOfPoint(p);

			CRect crSelect(cpStart, cpEnd);

			crSelect.NormalizeRect();

			pTable->SetSelect(crSelect);
		}
	}
	else
	{
		edit_state = STATE_NORMAL;
	}
	return TRUE;
	
}

// resize column of cells
BOOL CPmwView::resize_state_resize_table_col(EVENT_INFO *info)
{
#if 0
	if (current_object == NULL || current_object->type() != OBJECT_TYPE_Table)
	{
		ASSERT(FALSE);
		edit_state = STATE_RELEASE;
		return TRUE;
	}

	CTableObject* pTable = (CTableObject*)current_object;

	PBOX pbox;
	rc.mouse_box(info->old_xy, &pbox);

	CPmwDoc *pDoc = GetDocument();

	if (info->event == EVENT_MOUSE_MOVE)
	{
		PPNT pp;
		rc.screen_to_ppnt(info->current_xy, &pp);
		pTable->ResizeColumn(m_nResizer, pp);
		pDoc->refresh_object(pTable);			
	}
	else if (info->event == EVENT_BUTTON_FALLING)
	{
//		pDoc->refresh_object(pTable);
		edit_state = STATE_NORMAL;
	}
#endif
	return TRUE;	
}

// resize row of cells
BOOL CPmwView::resize_state_resize_table_row(EVENT_INFO *info)
{
#if 0
	static CTableObject * pTable = NULL;
	static GroupObject * pCells = NULL;
	
	static PBOX tablebound;

	CPmwDoc *pDoc = GetDocument();
	PBOX pbox;
	
	rc.mouse_box(info->old_xy, &pbox);
	
	if (info->event == EVENT_MOUSE_MOVE)
	{
		if (current_object->type() == OBJECT_TYPE_Table)
		{
			pTable = (CTableObject *)current_object;
			tablebound = pTable->get_bound();
			pCells = pTable->FindGroup(CELL_PANEL);
			if (pCells != NULL)//info->new_position)
			{
				// do resizing here
				// first find which cell we're on 
				CellObject * pTest = (CellObject*)pCells->object_list()->first_object();
				BOOL bExit = FALSE;
				while (pTest != NULL && !bExit)
				{
					if(pTest->box_on_row_edge(&pbox))
						bExit = TRUE;
					else
						pTest = (CellObject*)pTest->next_object();
				}
				if (pTest != NULL)
					current_object = pTest;
			}
		}
		
		if (current_object->type() == OBJECT_TYPE_Cell)
		{
			pTable->ResizeRow((CellObject*)current_object,
									rc.screen_y_to_page(info->current_xy.y),
									pTable->GetColumns());
			pCells->establish();
		}

		if (pTable != NULL)
			pDoc->refresh_object(pTable);			
	}
	else
	{
		if (pTable != NULL)
		{
			pDoc->refresh_object(pTable);
			pTable = NULL;
		}
		if (pCells != NULL)
			pCells = NULL;
		edit_state = STATE_NORMAL;
	}
	
#endif
	return TRUE;	
}

//
// The handler for the "SELECT_COL" state.
//

BOOL CPmwView::resize_state_select_table_col(EVENT_INFO *info)
{
	if (current_object->type() == OBJECT_TYPE_Table)
	{
		if (info->event == EVENT_MOUSE_MOVE || info->event == EVENT_BUTTON_FALLING)
		{
			CTableObject* pTable = (CTableObject*)current_object;

			PPNT Here;
			Here.x = rc.screen_x_to_page(info->current_xy.x);
			Here.y = rc.screen_y_to_page(info->current_xy.y);

			pTable->SelectColumns(pTable->ColumnOfPoint(select_start), pTable->ColumnOfPoint(Here));

#if 0
				pTable = 
				tablebound = pTable->get_bound();

				PBOX r;
				GroupObject * pObject = pTable->FindGroup(CELL_PANEL);
				CellObject * pCell = NULL;
				if (pObject != NULL)
					pCell = (CellObject*)pObject->object_list()->first_object();
									
				if (pCell != NULL)
				{
					for (int col=0; col<pTable->GetColumns(); col++)
					{
						pCell = pTable->GetCellAt(0, col);
						PBOX cellbound = pCell->get_bound();
						if(IsBoxOnBox(&cellbound, &pbox))
						{

							r.x0 = select_start.x;
							r.y0 = select_start.y;
							r.x1 = rc.screen_x_to_page(info->current_xy.x);
							r.y1 = tablebound.y1;

							if (r.x0 > r.x1)
							{
								PCOORD tmpx = r.x0;
								r.x0 = r.x1;
								r.x1 = tmpx;
							}
			
							if (r.y0 > r.y1)
							{
								PCOORD tmpy = r.y0;
								r.y0 = r.y1;
								r.y1 = tmpy;
							}

													// check for merged cells
							for (int row=0; row<pTable->GetRows(); row++)
							{
								pCell = pTable->GetCellAt(row, col);
								
								if (pCell->GetMergeState() == CELL_MERGED_CHILD) // get master cell
									pCell = pTable->GetCellAt(pCell->GetMergeRow(), pCell->GetMergeColumn());

								if (pCell->GetMergeState() == CELL_MERGED) // if master cell
								{
									// expand rect
									PBOX temp = pCell->get_bound();

									if (r.x0 > temp.x0)
										r.x0 = temp.x0;

									if (r.x1 < temp.x1)
										r.x1 = temp.x1;

									if (r.y0 > temp.y0)
										r.y0 = temp.y0;

									if (r.y1 < temp.y1)
										r.y1 = temp.y1;

								}
							}

							GetDocument()->select_all_cells(&r, TRUE);
							GetDocument()->select_object(current_object);	
							break;
						}
					}
				}
			}
#endif
			if (info->event == EVENT_BUTTON_FALLING)
				edit_state = STATE_NORMAL;
		}
		else
		{
			edit_state = STATE_NORMAL;
		}
	}
	return TRUE;
}



BOOL CPmwView::resize_state_select_table_row(EVENT_INFO *info)
{
	if (current_object->type() == OBJECT_TYPE_Table)
	{
		if (info->event == EVENT_MOUSE_MOVE || info->event == EVENT_BUTTON_FALLING)
		{
			CTableObject* pTable = (CTableObject*)current_object;

			PPNT Here;
			Here.x = rc.screen_x_to_page(info->current_xy.x);
			Here.y = rc.screen_y_to_page(info->current_xy.y);

			pTable->SelectRows(pTable->RowOfPoint(select_start), pTable->RowOfPoint(Here));

#if 0
		// if the current object is a table, find the col we're currently over
		if(current_object->type() == OBJECT_TYPE_Table)
		{
			
			pTable = (CTableObject*)current_object;
			tablebound = pTable->get_bound();
			

			GroupObject * pObject = pTable->FindGroup(CELL_PANEL);
			CellObject * pCell = NULL;
			if (pObject != NULL)
				pCell = (CellObject*)pObject->object_list()->first_object();
			if (pCell != NULL)
			{
				PBOX r;
				for (int row=0; row<pTable->GetRows(); row++)
				{
					pCell = pTable->GetCellAt(row, 0);
					PBOX cellbound = pCell->get_bound();
					if(IsBoxOnBox(&cellbound, &pbox))
					{
						r.x0 = select_start.x;
						r.y0 = select_start.y;
						r.x1 = tablebound.x1;
						r.y1 = rc.screen_y_to_page(info->current_xy.y);

						if (r.x0 > r.x1)
						{
							PCOORD tmpx = r.x0;
							r.x0 = r.x1;
							r.x1 = tmpx;
						}
		
						if (r.y0 > r.y1)
						{
							PCOORD tmpy = r.y0;
							r.y0 = r.y1;
							r.y1 = tmpy;
						}
						// check for merged cells
						for (int col=0; col<pTable->GetColumns(); col++)
						{
							pCell = pTable->GetCellAt(row, col);
							
							if (pCell->GetMergeState() == CELL_MERGED_CHILD)
								pCell = pTable->GetCellAt(pCell->GetMergeRow(), pCell->GetMergeColumn());

							if (pCell->GetMergeState() == CELL_MERGED)
							{
								// expand rect
								PBOX temp = pCell->get_bound();
								
								if (r.y0 > temp.y0)
									r.y0 = temp.y0;

								if (r.y1 < temp.y1)
									r.y1 = temp.y1;
								
								if (r.x0 > temp.x0)
									r.x0 = temp.x0;
								
								if (r.x1 < temp.x1)
									r.x1 = temp.x1;
								
							}
						}

						pDoc->select_all_cells(&r, TRUE);
						pDoc->select_object(current_object);	
						break;
					}
				}
			}
		}
#endif
			if (info->event == EVENT_BUTTON_FALLING)
				edit_state = STATE_NORMAL;
		}
		else
		{
			edit_state = STATE_NORMAL;
		}
	}
	return TRUE;
}


/* Sets up a drag source from the list of currently selected objects.
*/
void CPmwView::InitiateDrag()
{
	// don't allow recursion!
	if (m_fDragActive)
	{
		ASSERT( 0 );
		return;
	}

	// Mark ourselves as being in an active drag.
	m_fDragActive = TRUE;
	m_fLocalDragDrop = FALSE;	// Clear the local drag flag.

	// initialize the handlers.
	CPDragDataSource *pDropDataHandler = NULL;
	COleDropSource *pDropSourceHandler = NULL;

	pDropDataHandler = new CPDragDataSource();
	pDropSourceHandler = new CPFeedbackDropSource();

	pDropDataHandler->SetDocument( GetDocument() );

	// Inform the system that we can supply a Metafile.
	FORMATETC formatMetafile;

	// Set generic settings.
	formatMetafile.ptd = NULL;
	formatMetafile.dwAspect = DVASPECT_CONTENT;
	formatMetafile.lindex = -1;

	// Register for Enhanced Metafile output.
	formatMetafile.cfFormat = CF_ENHMETAFILE;
	formatMetafile.tymed = TYMED_ENHMF;
	pDropDataHandler->DelayRenderData(CF_ENHMETAFILE, &formatMetafile);

	// Register standard Metafile output.
	formatMetafile.cfFormat = CF_METAFILEPICT;
	formatMetafile.tymed = TYMED_MFPICT;
	pDropDataHandler->DelayRenderData(CF_METAFILEPICT, &formatMetafile);

	// Register Bitmap image output.
	formatMetafile.cfFormat = CF_BITMAP;
	formatMetafile.tymed = TYMED_GDI;
	pDropDataHandler->DelayRenderData(CF_BITMAP, &formatMetafile);

	// Register DIB image output.
	pDropDataHandler->DelayRenderData(CF_DIB);

	// Register the Object Descriptor for the drag
	pDropDataHandler->DelayRenderData(Clipboard::m_cfObjectDescriptor);

	// Activate the drop process with these data handlers.
	pDropDataHandler->DoDragDrop( DROPEFFECT_COPY | DROPEFFECT_MOVE, NULL, pDropSourceHandler );

	// The drop is complete, so clean up the data handlers.
	delete pDropDataHandler;
	delete pDropSourceHandler;

	// Clear ourselves as being in an active drag.
	m_fDragActive = FALSE;
}
