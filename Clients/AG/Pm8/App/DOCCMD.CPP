/*
// $Header: /PM8/App/DOCCMD.CPP 1     3/03/99 6:04p Gbeddow $
//
// Document commands.
//
// $Log: /PM8/App/DOCCMD.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     8/28/98 10:27a Johno
// Fix for text selection - position problem
// 
// 3     7/24/98 3:07p Psasse
// Fix handle remnants clean-up when object off of poster and set to full
// page
// 
// 2     11/24/97 8:04a Jstoner
// changed object center on page function to handle top/bottom and
// left/right page calendars
// 
//    Rev 1.0   14 Aug 1997 15:20:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:10   Fred
// Initial revision.
// 
//    Rev 1.5   28 Jul 1997 15:31:24   Jay
// Objects centered, etc. are calc'd before being refreshed (to update the bounds).
// 
//    Rev 1.4   20 Jul 1997 12:42:54   Jay
// Now marks frames and sets document modified
// 
//    Rev 1.3   26 Aug 1996 14:38:04   Jay
// Position "Full Page" now does to panel world, not document bounds.
// 
//    Rev 1.2   01 Aug 1996 17:45:32   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.1   29 Jul 1996 11:22:38   Jay
// Full page
// 
//    Rev 1.0   14 Mar 1996 13:36:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:08   FRED
// Initial revision.
// 
//    Rev 2.2   15 Feb 1995 17:15:46   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:32:06   JAY
// 
//    Rev 2.0   07 Feb 1995 16:23:32   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "utils.h"
#include "commands.h"
#include "pmwview.h"		// For EditingText() (etc.) in AdjustSelects()

/*
// Return the current view.
// Since PMW only has one view per document, it's just the first view.
*/

CPmwView* CPmwDoc::GetCurrentView(void) const
{
	CPmwView* pView = NULL;
	POSITION Pos = GetFirstViewPosition();
	if (Pos != NULL)
	{
		pView = (CPmwView*)GetNextView(Pos);
	}
	return pView;
}

BOOL CPmwDoc::NotBorderCriterion(ListElementPtr element, VOIDPTR data)
{
	return ((PMGPageObjectPtr)element)->type() != OBJECT_TYPE_Background;
}

BOOL CPmwDoc::LastBorderCriterion(ListElementPtr element, VOIDPTR data)
{
	PMGPageObjectPtr next = (PMGPageObjectPtr)element->next_element();

	return ((PMGPageObjectPtr)element)->type() == OBJECT_TYPE_Background
						&& (next == NULL || next->type() != OBJECT_TYPE_Background);
}

/*
// Do a relayer command.
*/

void CPmwDoc::DoRelayerCommand(CCmdRelayerSelects* pCommand)
{
	if (pCommand != NULL)
	{
		if (!pCommand->Snapshot(GetCurrentView()))
		{
			delete pCommand;
			return;
		}

		/*
		// Add the command to the document's list.
		// This will do the command and allow the user to undo it.
		*/

		AddCommand(pCommand);
	}
}

void CPmwDoc::BackOne()
{
	DoRelayerCommand(new CCmdBackOne);
}

void CPmwDoc::ForwardOne()
{
	DoRelayerCommand(new CCmdForwardOne);
}

void CPmwDoc::SendToBack()
{
	DoRelayerCommand(new CCmdSendToBack);
}

void CPmwDoc::SendToFront()
{
	DoRelayerCommand(new CCmdSendToFront);
}

void CPmwDoc::AdjustSelects(SHORT id)
{
	PMGPageObjectPtr object;
	SHORT world_panel = -1;
	PBOX world;

/* Set up for the undo/redo command. */
	CPmwView* pView = GetCurrentView();
	ASSERT(pView);
	CCmdChangeSelects* pCommand = new CCmdChangeSelects(id);
	if (!pCommand->Before(pView))
	{
		delete pCommand;
		pCommand = NULL;
	}
	// Invalidate any selected tex or the selection may not draw correctly
	if ((pView != NULL) && (pView->EditingText()))
	{
		CTxp	*pTxp;
		
		if ((pTxp = pView->GetTxp()) != NULL)
		{
			int nVisible = pTxp->Visible();
			pTxp->Off(nVisible);
			pTxp->InvalidateSelectBounds();
		}
		ASSERT(pTxp);
	}
	
	for (object = selected_objects();
		  object != NULL;
		  object = object->next_selected_object())
	{
		SHORT panel = object->get_panel();

	/* Get the panel for this object. */

		if (panel >= number_of_panels())
		{
			continue;
		}

	/* Get the panel world if we don't already have it. */
		
		
		// if this is not a calendar, do this
		if (get_project_type() != PROJECT_TYPE_Calendar)
		{
			if (panel != world_panel)
			{
				world_panel = panel;
				get_panel_world(&world, panel);
			}
		}
		else		// otherwise do this
			GetObjectPageDims(&world, object);

		// Mark frames below this object.
		MarkFramesBelow(object);

		PBOX old_bound = ((RectPageObject*)object)->get_unrotated_bound();
		PBOX bound = old_bound;
		ANGLE dRotation = object->get_rotation();
		BOOL fAngleChanged = FALSE;

		if (id == IDCmd_FullPage)
		{
			bound = world;
			// Reset the rotation.
			//object->set_rotation(0);
		}

		if (id == IDCmd_CenterSelects || id == IDCmd_HCenterSelects)
		{
			PCOORD dx = bound.x1 - bound.x0;

		/* Horizontally center. */

			bound.x0 = (world.x0+world.x1 - dx)/2;
			bound.x1 = bound.x0 + dx;
		}

		if (id == IDCmd_CenterSelects || id == IDCmd_VCenterSelects)
		{
			PCOORD dy = bound.y1 - bound.y0;

		/* Vertically center. */

			bound.y0 = (world.y0+world.y1 - dy)/2;
			bound.y1 = bound.y0 + dy;
		}

		if (memcmp(&bound, &old_bound, sizeof(PBOX)) != 0
				|| object->get_rotation() != dRotation)
		{
			toggle_object(object);

			if (id == IDCmd_FullPage)
			{
				// Reset the rotation.
				object->set_rotation(0);
			}

			refresh_object(object);

			((RectPageObject*)object)->set_unrotated_bound(bound);
         object->calc();

			refresh_object(object);
			toggle_object(object);
		}

		// Mark frames below this object.
		MarkFramesBelow(object);
	}

/* Recalc and set to manual layout. */

	calc_selects(TRUE);

	UpdateMarkedFrames(TRUE);
	SetModifiedFlag();

/* Finish the undo/redo command. */

	if (pCommand != NULL)
	{
		if (pCommand->After())
		{
			AddCommand(pCommand, FALSE);
		}
		else
		{
			delete pCommand;
		}
	}

}

void CPmwDoc::CenterSelects()
{
	AdjustSelects(IDCmd_CenterSelects);
}

void CPmwDoc::HCenterSelects()
{
	AdjustSelects(IDCmd_HCenterSelects);
}

void CPmwDoc::VCenterSelects()
{
	AdjustSelects(IDCmd_VCenterSelects);
}

void CPmwDoc::FullPageSelects()
{
	AdjustSelects(IDCmd_FullPage);
}

void CPmwDoc::GetObjectPageDims(PBOX * pBox, PageObject * pObject)
{

	// overloaded in calendar doc...should not be used otherwise!
}