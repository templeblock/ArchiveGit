/*
// $Header: /PM8/App/MODECROP.CPP 1     3/03/99 6:08p Gbeddow $
//
// Cropping mode routines.
//
// $Log: /PM8/App/MODECROP.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
 * 
 * 3     1/19/99 2:53p Lwilson
 * Enabled cropping for component objects.
// 
// 2     12/23/98 1:50p Psasse
// Code clean up
// 
//    Rev 1.0   14 Aug 1997 15:22:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:12   Fred
// Initial revision.
// 
//    Rev 1.5   16 Jun 1997 14:27:08   Jay
// Can now grab non-opaque areas.
// 
//    Rev 1.4   04 Mar 1997 17:03:38   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.3   26 Feb 1997 10:57:28   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.2   25 Feb 1997 17:01:18   Jay
// More crop goodies.
// 
//    Rev 1.1   25 Feb 1997 12:57:16   Jay
// Cropping and extensions
// 
//    Rev 1.0   21 Feb 1997 15:09:08   Jay
// New mode handling; crop mode support
*/

#include "system.h"
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "action.h"
#include "utils.h"
#include "commands.h"				// for Undo support

static PMGPageObject* m_pCropObject = NULL;

/*
// Initialization state.
*/

BOOL CPmwView::crop_state_init(EVENT_INFO *info)
{
	set_cursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	edit_state = STATE_NORMAL;

	return crop_state_normal(info);				/* Do it now, too. */
}

/*
// Normal state.
*/

BOOL CPmwView::crop_state_normal(EVENT_INFO *info)
{
	EVENT event = info->event;

	// Ignore double-click if shift key is down.
	if (event == EVENT_BUTTON_DBLCLK && SHIFT_KEY(info->flags))
	{
		// Map to non-double-click.
		event = EVENT_BUTTON_RISING;
	}

/*
// We only handle right button clicks in this mode.
*/

	if ((info->flags & MK_RBUTTON) && event != EVENT_RBUTTON_RISING)
	{
		edit_state = STATE_RELEASE;
		return TRUE;
	}

	// Escape here means done with cropping.
	if (check_escape_event(info))
	{
		set_arrow_tool();
		return TRUE;
	}

	// Is this event handled?
	BOOL fHandled = FALSE;

	PPNT p;
	rc.screen_to_ppnt(info->current_xy, &p);

	CPmwDoc *pDoc = GetDocument();

	BOOL button_rising = FALSE;

	switch (event)
	{
		case EVENT_KEY_RISING:
		case EVENT_KEY_DOWN:
		{
#if 0
			switch (info->v_key)
			{
				case VK_UP:
				{
					resize_key(0, -1, info, p);
					break;
				}
				case VK_DOWN:
				{
					resize_key(0, 1, info, p);
					break;
				}
				case VK_LEFT:
				{
					resize_key(-1, 0, info, p);
					break;
				}
				case VK_RIGHT:
				{
					resize_key(1, 0, info, p);
					break;
				}
				default:
				{
//					od("Key %x pressed\r\n", info->v_key);
					break;
				}
			}
#endif
			/* We don't want these passed on to mode_resize(). */
			fHandled = TRUE;
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

	PBOX pbox;
	PMGPageObjectPtr object, handle_object, on_object;
	OBJECT_HANDLE handle;

	rc.mouse_box(info->current_xy, &pbox);

	handle_object = on_object = NULL;

/*
// Look in the selected list for handle cases.
*/

	for (object = pDoc->selected_objects();
								object != NULL;
								object = object->next_selected_object())
	{
		OBJECT_HANDLE test_handle;
		PBOX bound = object->get_bound();

		if ((object->get_select_flags() & SELECT_FLAG_size_handles)
					&& (test_handle = object->box_on_handle(&pbox, &rc)) != OBJECT_HANDLE_NONE)
		{
			handle_object = object;
			handle = test_handle;
		}

		PBOX refresh_bound;

		object->get_refresh_bound(&refresh_bound, &rc);
		UnionBox(&refresh_bound, &refresh_bound, &bound);

		if (IsBoxOnBox(&pbox, &refresh_bound) /*&& object->pt_in_object(p, &rc, NULL)*/)
		{
			on_object = object;
		}
	}

	// See if we found a handle.

	if (handle_object != NULL)
	{
		HCURSOR hCursor = AfxGetApp()->LoadCursor(handle == OBJECT_HANDLE_ROTATE
									? IDC_ROTATE
									: IDC_CROP_CURSOR);

		set_cursor(hCursor);
		if (button_rising)
		{
//			set_cursor(AfxGetApp()->LoadCursor(IDC_CROP_CURSOR));

			// We just grabbed it!

			edit_state = CROP_STATE_HANDLE;
			UpdateFrameButtons();

			// Begin moving the handle.

			current_handle = handle;
			current_object = handle_object;

			// Save the initial bounds.

			original_bound = handle_object->get_bound();
			original_angle = handle_object->get_rotation();
			handle_object->get_refresh_bound(&original_refresh_bound, &rc);

			// Create the undo/redo command for this action.

			if (m_pCommand != NULL)
			{
				TRACE("m_pCommand is not NULL!\n");
				delete m_pCommand;
			}
			m_pCommand = new CCmdChangeSelects(IDCmd_Crop);

			if (!m_pCommand->Before(this))
			{
				delete m_pCommand;
				m_pCommand = NULL;
			}

			// Mark frames below this object.
			pDoc->MarkFramesBelow(current_object);

			// Signal the beginning of handle movement.

			LPVOID data = current_object;
			PMG_OBJECT_ACTION action;

			action = current_object->move_handle(DRAG_STATE_begin, p, current_handle, info->flags, &data, TRUE);

			if (action != ACTION_DO_NOTHING)
			{
				pDoc->process_action(this, action, current_object, data, p);
			}
		}
		else
		{
//			set_cursor(AfxGetApp()->LoadCursor(IDC_CROP_CURSOR));
		}
		return TRUE;
	}

/*
// Not on a handle.
// See if we're over an object.
*/

	if (on_object != NULL)
	{
		HCURSOR hCursor;

		if (on_object->get_select_flags() & SELECT_FLAG_move_handle)
		{
		/* We can indeed move it. Show the appropriate cursor. */
			hCursor = AfxGetApp()->LoadCursor(IDC_CANPAN_CURSOR);
		}
		else
		{
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}
		set_cursor(hCursor);

		if (info->flags & (MK_LBUTTON | MK_RBUTTON))
		{
			if (event == EVENT_BUTTON_DBLCLK)
			{
				// Double click!
			}
			else if (event == EVENT_RBUTTON_RISING)
			{
				process_right_button(info->current_xy);
			}
			else if ((on_object->get_select_flags() & (SELECT_FLAG_move_handle | SELECT_FLAG_drag_handle)))
			{
				// We just grabbed it!
				current_object = on_object;
				current_handle = OBJECT_HANDLE_MOVE;

				// Save the initial bounds.
				original_bound = current_object->GetDrawBound();
				current_object->get_refresh_bound(&original_refresh_bound, &rc);

				// Compute grab location relative to upper corner.
				grab_offset.x = p.x - original_bound.x0;
				grab_offset.y = p.y - original_bound.y0;

				// Create the undo/redo command for this action.

				if (m_pCommand != NULL)
				{
					TRACE("m_pCommand is not NULL!\n");
					delete m_pCommand;
				}
				m_pCommand = new CCmdChangeSelects(IDCmd_Pan);

				if (!m_pCommand->Before(this))
				{
					delete m_pCommand;
					m_pCommand = NULL;
				}

				// Begin panning the object.
				edit_state = CROP_STATE_PANNING;
				UpdateFrameButtons();

				// Mark frames before the move.
				pDoc->MarkFramesBelowSelectedObjects();

				// Initiate panning.
				m_pCropObject->move_object(DRAG_STATE_begin, p, p, NULL, TRUE);
				set_cursor(AfxGetApp()->LoadCursor(IDC_PANNING_CURSOR));
			}
			else
			{
				edit_state = STATE_RELEASE;
			}
		}
		return TRUE;
	}

	return fHandled;
}

/*
// Grabbed and moving a handle state (while cropping).
*/

BOOL CPmwView::crop_state_handle(EVENT_INFO *info)
{
	BOOL stop_now = check_escape_event(info);
	PPNT p = {0, 0};
	CPmwDoc *pDoc = GetDocument();

	if (!stop_now && (info->flags & MK_LBUTTON))
	{
		if (info->new_position)
		{
		/* Mouse moved. */

			rc.screen_to_ppnt(info->current_xy, &p);
			SnapPosition(&p);

		/* Move the handle. */

			LPVOID data = current_object;
			PMG_OBJECT_ACTION action;

			action = current_object->move_handle(DRAG_STATE_continue, p, current_handle, info->flags, &data, TRUE);
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
			action = current_object->move_handle(DRAG_STATE_abort, p, current_handle, info->flags, &data, TRUE);
		}
		else
		{
		/* Signal an end of handle movement. */
			action = current_object->move_handle(DRAG_STATE_end, p, current_handle, info->flags, &data, TRUE);
		}

		if (action != ACTION_DO_NOTHING)
		{
			pDoc->process_action(this, action, current_object, data, p);
		}

	/* See if we moved or sized at all. */

		BOOL redraw = TRUE;
		CPmwDoc *document = GetDocument();
		PBOX bound = current_object->get_bound();

		if (memcmp(&original_bound,
							&bound,
							sizeof(PBOX)) != 0
			|| original_angle != current_object->get_rotation())
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
				document->AddCommand(m_pCommand, FALSE);
				m_pCommand = NULL;
			}

			fDoIntersect = TRUE;
			// Mark frames below this object.
			pDoc->MarkFramesBelow(current_object);
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
	}
	return TRUE;
}

/*
// Panning an object.
*/

BOOL CPmwView::crop_state_panning(EVENT_INFO *info)
{
	CPmwDoc *document = GetDocument();
	BOOL stop_now = check_escape_event(info);
	PPNT p;

	rc.screen_to_ppnt(info->current_xy, &p);

	if (!stop_now && (info->flags & MK_LBUTTON))
	{
		if (info->new_position)
		{
		/* Mouse moved. */

			//PBOX bound = current_object->GetDrawBound();

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
#if 0
			vector.x = p.x
					- (grab_offset.x + bound.x0);
			vector.y = p.y
					- (grab_offset.y + bound.y0);
#else
			vector.x = p.x - OriginalGrabPoint.x;
			vector.y = p.y - OriginalGrabPoint.y;
#endif

#if 0
			bound.x0 += vector.x;
			bound.y0 += vector.y;
			bound.x1 += vector.x;
			bound.y1 += vector.y;

			PPNT SnappedUL;
			SnappedUL.x = bound.x0;
			SnappedUL.y = bound.y0;
			SnapPosition(&SnappedUL);

			PPNT DeltaUL;
			if ((DeltaUL.x = SnappedUL.x - bound.x0) < 0)
			{
				DeltaUL.x = -DeltaUL.x;
			}
			if ((DeltaUL.y = SnappedUL.y - bound.y0) < 0)
			{
				DeltaUL.y = -DeltaUL.y;
			}

			PPNT SnappedLR;
			SnappedLR.x = bound.x1;
			SnappedLR.y = bound.y1;
			SnapPosition(&SnappedLR);

			PPNT DeltaLR;
			if ((DeltaLR.x = SnappedLR.x - bound.x1) < 0)
			{
				DeltaLR.x = -DeltaLR.x;
			}
			if ((DeltaLR.y = SnappedLR.y - bound.y1) < 0)
			{
				DeltaLR.y = -DeltaLR.y;
			}

			if (DeltaLR.x < DeltaUL.x)
			{
				vector.x += SnappedLR.x - bound.x1;
			}
			else
			{
				vector.x += SnappedUL.x - bound.x0;
			}
			
			if (DeltaLR.y < DeltaUL.y)
			{
				vector.y += SnappedLR.y - bound.y1;
			}
			else
			{
				vector.y += SnappedUL.y - bound.y0;
			}
#endif

			// Pan the object.

			m_pCropObject->move_object(DRAG_STATE_continue, vector, p, NULL, TRUE);
		}
	}
	else
	{
	/* If things moved, refresh both. */

		BOOL fDoIntersect = FALSE;

		if (stop_now)
		{
			/* Signal an abort of handle movement. */

			m_pCropObject->move_object(DRAG_STATE_abort, p, p, NULL, TRUE);

			FLAGS flags = current_object->get_flags();

			if (flags & OBJECT_FLAG_needs_update)
			{
				document->add_refresh(&original_refresh_bound);
			}
		}
		else
		{
			if (current_object->get_select_flags() & SELECT_FLAG_move_handle)
			{
				PBOX bound = current_object->GetDrawBound();

				// Signal the end of moving.

				m_pCropObject->move_object(DRAG_STATE_end, p, p, NULL, TRUE);

				if (memcmp(&original_bound,
									&bound,
									sizeof(PBOX)) != 0)
				{
					document->SetModifiedFlag();

				/* Do the calc. */

					current_object->calc();
					current_object->changed_image();				// Always force this.

				/* In with the new... */

					document->GetSelectRefreshBound(&bound);
					document->add_refresh(&bound);

				/* ...and out with the old. */

					document->add_refresh(&original_refresh_bound);

				/* In case if goes off the edge of a banner. */

					document->resize_project();

				/* Set up for undo/redo. */

					if (m_pCommand != NULL && m_pCommand->After())
					{
					/* Add the command for undoing later. */
					/* Don't execute! */
						document->AddCommand(m_pCommand, FALSE);
					}
					else
					{
						delete m_pCommand;
					}
					m_pCommand = NULL;

					// Handle frame intersections.
					document->MarkFramesBelowSelectedObjects();
					fDoIntersect = TRUE;
				}
				else
				{
				/* Get rid of our undo command. */
					FLAGS flags = current_object->get_flags();

					current_object->set_flags(flags & ~OBJECT_FLAG_needs_calc);

					if (flags & OBJECT_FLAG_needs_update)
					{
						document->GetSelectRefreshBound(&bound);
						document->add_refresh(&bound);
					}
				}
			}
			else
			{
            ASSERT(FALSE);
			}
		}

		// Unmark frames marked before the move.
		document->UpdateMarkedFrames(fDoIntersect);

		edit_state = stop_now ? STATE_RELEASE : STATE_NORMAL;

	/* Clean up any remaining undo command. */

		if (m_pCommand != NULL)
		{
			delete m_pCommand;
			m_pCommand = NULL;
		}
	}
	return TRUE;
}

/*
// The states for the crop mode.
*/

CPmwView::STATE_HANDLER CPmwView::crop_state[] =
{
	CPmwView::crop_state_init,
	CPmwView::crop_state_normal,
	CPmwView::standard_state_release,

	CPmwView::crop_state_handle,
	CPmwView::crop_state_panning,
};

#define	CROP_STATE_COUNT			(sizeof(crop_state)/sizeof(crop_state[0]))

/*
// The crop mode handler.
*/

void CPmwView::mode_crop(EVENT_INFO *info)
{
	CPmwDoc* pDoc = GetDocument();

	if (info->event >= EVENT_ENTERING_MODE)
	{
		// Special mode event.
		switch (info->event)
		{
			case EVENT_ENTERING_MODE:
			{
				if (CanCrop())
				{
               m_pCropObject = pDoc->selected_objects();
//					toggle_selects(SELECT_FLAG_draw_boundary, m_pCropObject);
				}
				else
				{
					// !CanCrop(); Why are we here?
					ASSERT(FALSE);
				}
				break;
			}
			case EVENT_LEAVING_MODE:
			case EVENT_REENTERING_MODE:
			{
//				toggle_selects(SELECT_FLAG_draw_boundary, m_pCropObject);
				break;
			}
			default:
			{
				break;
			}
		}
		return;
	}

	/*
	// If things have changed such that we can't crop,
	// drop out of crop mode.
	*/

	if (CanCrop())
	{
		// Keep this going even if they change objects.
		m_pCropObject = pDoc->selected_objects();
	   // Prepare to handle the state.
	   SHORT state = edit_state;

	   if (state > STATE_BASE_END)
	   {
		   if (state >= CROP_STATE_BASE)
		   {
			   // One of our special states!
			   state -= (CROP_STATE_BASE-STATE_BASE_END);
		   }
		   else
		   {
			   // Not a valid state for here.
			   state = CROP_STATE_COUNT;
		   }
	   }

	   if (state < CROP_STATE_COUNT)
	   {
		   // Valid state, process.
		   if ((this->*crop_state[state])(info))
		   {
			   return;
		   }
	   }
   }
   else
   {
      // Go back to the arrow tool.
		set_arrow_tool();
   }

	// Pass it down to the resize mode.
	mode_resize(info);
}

/*
// Return whether we currently are capable of cropping or not.
*/

BOOL CPmwView::CanCrop(void)
{
	// See if we can crop.
	BOOL fEnable = FALSE;
	CPmwDoc* pDoc = GetDocument();
	PMGPageObject* pObject = pDoc->selected_objects();
	
	if (pObject != NULL
		 && pObject->next_selected_object() == NULL
		 && pObject->get_panel() < pDoc->number_of_panels())
	{
		switch (pObject->type())
		{
			case OBJECT_TYPE_Graphic:
			case OBJECT_TYPE_Rectangle:
			case OBJECT_TYPE_Ellipse:
			case OBJECT_TYPE_DrawingObject:
			case OBJECT_TYPE_Component:
			{
				fEnable = TRUE;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	return fEnable;
}
