/*
// $Header: /PM8/App/MODEDRAW.CPP 1     3/03/99 6:08p Gbeddow $
//
// Mode draw routines.
//
// $Log: /PM8/App/MODEDRAW.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:12   Fred
// Initial revision.
// 
//    Rev 1.4   23 May 1997 17:06:42   Fred
// New effects
// 
//    Rev 1.3   17 Mar 1997 14:48:10   Jay
// Warp shape palette and support.
// 
//    Rev 1.2   21 Feb 1997 15:09:10   Jay
// New mode handling; crop mode support
// 
//    Rev 1.1   19 Feb 1997 16:53:14   Jay
// Snapping
// 
//    Rev 1.0   11 Feb 1997 14:54:38   Jay
// Free draw tool
*/

#include "system.h"
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "outpath.h"
#include "pathrec.h"
#include "drawobj.h"
#include "utils.h"

// Move these to CPmwView someday!

static COutlinePath m_DrawPath;
static DWORD m_dwDrawStartTime;
static PBOX m_DrawBounds;
static POINT m_FirstDrawPoint;

static BOOL DrawToggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data)
{
   CPmwView *view = (CPmwView *)data;

	// We might want to toggle the line we are drawing here.
	// This would involve putting an End on the end of the path and
	// toggling it to screen, then removing the End afterwards.
	// At the present, COutlinePath (derived from CDynamicArray) does not
	// have a RemoveElement() method; since I'm not sure how important this
	// is, I'm not going to go through the motions of making it 100% correct.
	// When the user stops drawing, the whole thing refreshes anyway, so
	// any glitch is small and goes away.

	return FALSE;
}

/*
// Abort a draw.
*/

void CPmwView::AbortDraw(void)
{
	rc.remove_toggle(DrawToggle);

	if (m_pCreatedObject != NULL)
	{
		AbortCreate(FALSE);
//		add_refresh(&m_DrawBounds);
	}
}

/*
// Initialization state.
*/

BOOL CPmwView::draw_state_init(EVENT_INFO *info)
{
	// Empty our path.
	m_DrawPath.Free();

	ASSERT(m_pCreatedObject == NULL);
	m_pCreatedObject = NULL;

	UINT uCursor = IDC_GENERAL_CREATE_CURSOR;
	m_uUndoCreate = IDCmd_AddShape;

	set_cursor(AfxGetApp()->LoadCursor(uCursor));

	edit_state = STATE_NORMAL;

	return draw_state_normal(info);				/* Do it now, too. */
}

/*
// Normal state.
*/

BOOL CPmwView::draw_state_normal(EVENT_INFO *info)
{
	if (check_escape_event(info))
	{
		AbortDraw();
	}
	else if (info->flags & MK_LBUTTON)
	{
		m_dwDrawStartTime = ::GetTickCount();
		CPmwDoc* pDoc = GetDocument();

		// Deselect all selected objects.
		deselect_all();

		PPNT Here;
		rc.screen_to_ppnt(info->current_xy, &Here);
		m_FirstDrawPoint = info->current_xy;

		PMGDatabase* pDatabase = pDoc->get_database();
		// Create the correct object.
		m_pCreatedObject = (PMGPageObject*)pDatabase->new_object(OBJECT_TYPE_DrawingObject, NULL);

		// Set the bound to be where we are now.
		// We assume this is a RectPageObject (everything is as of now).
		m_DrawBounds.x0 = Here.x;
		m_DrawBounds.y0 = Here.y;
		m_DrawBounds.x1 = Here.x;
		m_DrawBounds.y1 = Here.y;
		((RectPageObject*)m_pCreatedObject)->set_bound(m_DrawBounds);

		if (m_pCreatedObject == NULL)
		{
			// Failed to create. Force a release.
			edit_state = STATE_RELEASE;
		}
		else
		{
			// Get the properties to apply to this object.
			CObjectProperties Properties;
			Properties.MergeWith(GetDocument()->GetDefaultObjectProperties());

			COLOR FillColor = UNDEFINED_COLOR;
			Properties.ApplyFillForegroundColor(FillColor);
			if (FillColor == UNDEFINED_COLOR)
			{
				// Fill color property not set yet. Set it ourselves.
				Properties.SetFillForegroundColor(TRANSPARENT_COLOR, FALSE);	// No fill color!
			}

			// Set the properties.
			m_pCreatedObject->SetObjectProperties(Properties);

			// We have the object. Begin dragging it.
			m_pCreatedObject->calc();

			// Set the panel for the new object.
			m_pCreatedObject->set_panel(pDoc->get_current_panel());

			// Add us to the document...
			pDoc->append_object(m_pCreatedObject);

			// Select us.
			m_pCreatedObject->remove_select_flags(SELECT_FLAG_rotate_handle);
			pDoc->select_object(m_pCreatedObject);

			// We are drawing this object.
			m_pCreatedObject->add_flags(OBJECT_FLAG_needs_calc);

			// Start the path.
			// Note that we use screen resolution for our path.
			m_DrawPath.MoveTo(MakeFixed(m_FirstDrawPoint.x),
									MakeFixed(m_FirstDrawPoint.y));

//         TRACE("MoveTo(%d, %d)\n", m_FirstDrawPoint);

			edit_state = DRAW_STATE_DRAWING;

			rc.register_toggle(DrawToggle, (VOIDPTR)this);
		}
	}
	return TRUE;
}

BOOL CPmwView::draw_state_drawing(EVENT_INFO *info)
{
	if (check_escape_event(info))
	{
		// We want to get out of here.
		AbortDraw();
		return TRUE;
	}

	// Compute where we are.

	PPNT p;
	BOOL fFinish = TRUE;

	rc.screen_to_ppnt(info->current_xy, &p);

	if (info->flags & MK_LBUTTON)
	{
		if (info->new_position)
		{
			CClientDC dc(this);
			int nOldROP2 = dc.SetROP2(R2_NOT);

			dc.MoveTo(info->old_xy.x, info->old_xy.y);
			dc.LineTo(info->current_xy.x, info->current_xy.y);

			dc.SetROP2(nOldROP2);

			// Add this component.
			// Note that we use screen resolution for our path.

			m_DrawPath.LineTo(MakeFixed(info->current_xy.x),
									MakeFixed(info->current_xy.y));
//			TRACE("LineTo(%d, %d)\n", info->current_xy);

			// Update our bounds.
			if (p.x < m_DrawBounds.x0)
			{
				m_DrawBounds.x0 = p.x;
			}
			if (p.y < m_DrawBounds.y0)
			{
				m_DrawBounds.y0 = p.y;
			}
			if (p.x > m_DrawBounds.x1)
			{
				m_DrawBounds.x1 = p.x;
			}
			if (p.y > m_DrawBounds.y1)
			{
				m_DrawBounds.y1 = p.y;
			}
		}
		fFinish = m_DrawPath.GetElementCount() > 10000;
	}

	if (fFinish)
	{
		// Remove the toggle.
		rc.remove_toggle(DrawToggle);

		// See if we want to close.
		CRect r;
		r.left = info->current_xy.x-CONTROL_DOT_WIDTH;
		r.top = info->current_xy.y-CONTROL_DOT_WIDTH;
		r.right = info->current_xy.x+CONTROL_DOT_WIDTH;
		r.bottom = info->current_xy.y+CONTROL_DOT_WIDTH;

		if (r.PtInRect(m_FirstDrawPoint))
		{
			// Connect end to start point.
			m_DrawPath.LineTo(MakeFixed(m_FirstDrawPoint.x),
									MakeFixed(m_FirstDrawPoint.y));
		}

		// Add the end.
   	m_DrawPath.End();

      CPmwDoc* pDoc = GetDocument();
      PMGDatabase* pDatabase = pDoc->get_database();
		// All done drawing.
		// We need to set the path.
		OutlinePathCreateStruct cs;

		// Generate a (more or less) random name from the
		// time of the beginning and end of drawing.
		char pszName[32];

		DWORD dwDrawEndTime = ::GetTickCount();
		sprintf(pszName,
				  "DRAW%08lx%08lx",
				  m_dwDrawStartTime,
				  dwDrawEndTime);

		// Compute the bounds.
		CFixedRect Bounds;
		m_DrawPath.BoundingBox(&Bounds);

      // See if this is a real object or not.
      if (Bounds.Width() == 0 || Bounds.Height() == 0)
      {
         AbortDraw();
      }
      else
      {
         // Normalize the path.
         COutlinePathMatrix Matrix;

		   Matrix.Identity();
		   Matrix.TranslateBy(-Bounds.Left, -Bounds.Top);
		   Matrix.ScaleBy(DivFixed(MakeFixed(1), Bounds.Width()),
							   DivFixed(MakeFixed(1), Bounds.Height()));

		   m_DrawPath.Transform(&Matrix);

         // Create the path record.
         cs.m_pszName = pszName;
		   cs.m_pPath = &m_DrawPath;
		   cs.m_pDesignBounds = &Bounds;

		   DB_RECORD_NUMBER PathRecord = pDatabase->new_outline_path_record(&cs);
		   if (PathRecord != 0)
		   {
			   ((DrawingObject*)m_pCreatedObject)->set_new_outline_path(PathRecord);
		   }

		   // So drawing can happen.
		   m_pCreatedObject->remove_flags(OBJECT_FLAG_needs_calc);
		   // Set the bounds.

		   toggle_object();
		   ((DrawingObject*)m_pCreatedObject)->set_bound(m_DrawBounds);
			m_pCreatedObject->add_select_flags(SELECT_FLAG_rotate_handle);
		   toggle_object();

		   // Make us show up right.
		   pDoc->refresh_object(m_pCreatedObject);
		   pDoc->AddCreateCommand(m_uUndoCreate);

		   // And we are done.
		   m_pCreatedObject = NULL;

		   // Done with this object. What now?
		   if (m_fStuckTool)
		   {
			   // Start over within this tool (mode).
			   edit_state = STATE_INIT;
		   }
		   else
		   {
			   // Go back to arrow tool state.
			   set_arrow_tool();
		   }
      }
	}
	return TRUE;
}

/*
// The states for the text create mode.
*/

CPmwView::STATE_HANDLER CPmwView::draw_state[] =
{
	CPmwView::draw_state_init,
	CPmwView::draw_state_normal,
	CPmwView::create_state_release,
	CPmwView::draw_state_drawing
};

#define	DRAW_STATE_COUNT			(sizeof(draw_state)/sizeof(draw_state[0]))

/*
// The draw mode handler.
*/

void CPmwView::mode_draw(EVENT_INFO *info)
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
		if (state >= DRAW_STATE_BASE)
		{
		/* One of our special states! */
			state -= (DRAW_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = DRAW_STATE_COUNT;
		}
	}

	if (state < DRAW_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*draw_state[state])(info);
	}
}
