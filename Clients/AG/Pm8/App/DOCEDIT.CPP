/*
// $Header: /PM8/App/DOCEDIT.CPP 1     3/03/99 6:05p Gbeddow $
//
// Document editing routines.
//
// $Log: /PM8/App/DOCEDIT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 32    12/23/98 1:50p Psasse
// Code clean up
// 
// 31    12/17/98 2:33p Lwilson
// Added double click edit support for component objects
// 
// 30    7/18/98 6:24p Jayn
// Table fixes.
// 
// 29    7/12/98 5:32p Jayn
// Serif objects now set to 2" by 2" at create time.
// 
// 28    7/01/98 4:15p Johno
// Calls CFrameObject*->RemoveHyperlink() instead of doing it here.	
// 
// 27    6/30/98 1:55p Johno
// Fixed (?) bug with hyperlink removal in DoPasteObject()
// 
// 26    6/29/98 9:40a Johno
// Fixed hyperlink bug in DoPasteObject()
// 
// 25    6/26/98 5:01p Johno
// Hyperlinks are removed from objects in DoPasteObject()
// 
// 24    6/02/98 3:00p Jayn
// 
// 23    5/31/98 2:35p Jayn
// Delete row/columns
// 
// 22    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 21    5/29/98 6:38p Jayn
// Insert column, etc.
// 
// 20    5/26/98 2:39p Jayn
// New table code.
// 
// 19    4/14/98 4:41p Jayn
// Changes for the new table object.
// 
// 18    4/07/98 8:45a Jayn
// Changes for DPP files.
// 
// 17    3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 16    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 15    3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 14    2/25/98 10:40a Jstoner
// table functionality updates...merge/split cells
// row/col selection done
// 
// 13    2/20/98 7:59p Hforman
// remove unused param from delete_object()
// 
// 12    2/11/98 11:36a Jstoner
// changes for table object
// 
// 11    2/04/98 4:50p Hforman
// add "fAddUndo" param to PostPaste()
// 
// 10    2/03/98 10:34a Jstoner
// saving incremental progress 
// 
// 9     1/28/98 10:39a Jstoner
// incremental update to table project
// 
// 8     1/21/98 9:15a Jstoner
// added table cell handling stuff
// 
// 7     1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 6     1/09/98 6:51p Hforman
// add call to ShowBorderEditor()
// 
// 5     1/07/98 8:27a Jstoner
// can now tab thru table
// 
// 4     1/07/98 7:48a Jstoner
// modify behavior of selection on table object
// 
// 3     12/19/97 5:17p Hforman
// add Helpful Hints
// 
// 2     11/19/97 2:03p Jstoner
// multi-calendar mods to calendar cell functions
// 
//    Rev 1.0   14 Aug 1997 15:20:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:12   Fred
// Initial revision.
// 
//    Rev 1.42   30 Jul 1997 10:43:42   Jay
// Increased refresh bounds for line. More flow frame cases.
// 
//    Rev 1.41   20 Jul 1997 12:42:56   Jay
// Now marks frames and sets document modified
// 
//    Rev 1.40   16 Jul 1997 17:56:24   Jay
// Calendar date improvements
// 
//    Rev 1.39   16 Jul 1997 13:30:22   Jay
// Sets Modified flag when object properties set.
// 
//    Rev 1.38   07 Jul 1997 16:06:52   Jay
// Adds the before as well as the after bounds during an attribte change.
// 
//    Rev 1.37   10 Jun 1997 15:29:46   Jay
// New color dialog; saves custom colors.
// 
//    Rev 1.36   09 Jun 1997 17:30:52   Jay
// Text improvements
// 
//    Rev 1.35   30 May 1997 12:43:22   Jay
// PasteSentiment
// 
//    Rev 1.34   29 May 1997 13:44:36   Jay
// Added PasteSentiment.
// 
//    Rev 1.33   03 Apr 1997 14:31:12   Jay
// Resource cleanup
// 
//    Rev 1.32   01 Apr 1997 13:16:28   Jay
// Banner preview
// 
//    Rev 1.31   28 Mar 1997 17:10:42   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.30   21 Mar 1997 15:48:52   Jay
// More preview changes.
// 
//    Rev 1.29   19 Feb 1997 16:53:12   Jay
// Snapping
// 
//    Rev 1.28   29 Jan 1997 17:24:28   Jay
// Next and previous object support
// 
//    Rev 1.27   28 Jan 1997 14:10:16   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.26   27 Jan 1997 13:55:16   Jay
// Default object attributes and size.
// 
//    Rev 1.25   23 Jan 1997 11:05:28   Jay
// New popup palettes
// 
//    Rev 1.24   08 Jan 1997 15:21:14   Fred
// More text box changes
// 
//    Rev 1.23   06 Jan 1997 10:15:18   Fred
// Start of new text object
// 
//    Rev 1.22   23 Dec 1996 10:57:06   Fred
// Enabled Jay's in-place title text editing
// 
//    Rev 1.21   06 Nov 1996 09:34:04   johno
// Changed object_name and object_plural
// from 'CHAR *' to 'const CHAR *'
// 
//    Rev 1.20   05 Nov 1996 16:59:04   johno
// Moved strings to resource DLL
// 
//    Rev 1.19   04 Sep 1996 17:00:32   Jay
// IsGroupedFrame was not allowing an edit of calendar title.
// 
//    Rev 1.18   04 Sep 1996 08:23:14   Jay
// Reversed order to fix intersection problem.
// 
//    Rev 1.17   29 Aug 1996 18:34:34   Jay
// Further refinement of the update sidebar crash fix.
// 
//    Rev 1.16   28 Aug 1996 11:13:30   Jay
// Generalized solution to special non-edit documents with sidebar crashes.
// 
//    Rev 1.15   21 Aug 1996 16:07:56   Jay
// Fixed recursion problem with empty card panels.
// 
//    Rev 1.14   21 Aug 1996 15:20:02   Jay
// Resize now updates frame intersections
// 
//    Rev 1.13   02 Aug 1996 15:25:28   Jay
// Bug fixes
// 
//    Rev 1.12   01 Aug 1996 17:45:32   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.11   23 Jul 1996 10:41:10   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.10   16 Jul 1996 11:26:30   Jay
// Linked frames with cut/copy/paste/duplicate
// 
//    Rev 1.9   15 Jul 1996 17:46:28   Jay
//  
// 
//    Rev 1.8   15 Jul 1996 11:35:02   Jay
//  
// 
//    Rev 1.7   12 Jul 1996 16:49:48   Jay
// Text wrap
// 
//    Rev 1.6   09 Jul 1996 15:37:34   Jay
//  
// 
//    Rev 1.5   03 Jul 1996 18:52:32   Fred
// Picture editor
// 
//    Rev 1.4   28 Jun 1996 16:18:56   Jay
//  
// 
//    Rev 1.3   26 Jun 1996 14:23:38   Jay
//  
// 
//    Rev 1.2   26 Jun 1996 09:07:32   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:21:46   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:36:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:10   FRED
// Initial revision.
// 
//    Rev 2.35   15 Jan 1996 10:26:28   JAY
//  
// 
//    Rev 2.34   15 Nov 1995 15:47:50   JAY
// More OLE fixes.
// 
//    Rev 2.33   15 Nov 1995 15:23:16   JAY
// OLE fixes, etc.
// 
//    Rev 2.32   29 Sep 1995 16:00:16   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.31   28 Sep 1995 16:49:50   JAY
// Paste Special
// 
//    Rev 2.30   28 Sep 1995 14:53:22   JAY
// Multi-file drops now work.
// 
//    Rev 2.29   27 Sep 1995 15:27:32   JAY
// New OLE code
// 
//    Rev 2.28   31 Aug 1995 10:04:16   FRED
// Miscellaneous fixes.
// 
//    Rev 2.27   23 Aug 1995 11:19:14   JAY
// Commented out reference to OleObject panel.
// 
//    Rev 2.26   22 Aug 1995 07:43:32   JAY
// Moved format bar code to mainfrm.
// 
//    Rev 2.25   20 Aug 1995 15:40:52   FRED
// Rotate object control
// 
//    Rev 2.24   16 Aug 1995 16:23:26   JAY
//  
// 
//    Rev 2.23   15 Aug 1995 13:47:54   JAY
// Now scales unrotated bounds for pasted objects - was unrotating them.
// 
//    Rev 2.22   11 Aug 1995 09:38:26   JAY
// Fixed/extended duplicate.
// 
//    Rev 2.21   10 Aug 1995 17:13:32   JAY
// Now sets the format bar.
// 
//    Rev 2.20   10 Aug 1995 15:33:48   FRED
// Changed "ellipse" to "circle".
// 
// 
//    Rev 2.19   09 Aug 1995 13:12:22   JAY
// Misc changes.
// 
//    Rev 2.18   08 Aug 1995 13:24:54   JAY
// Right mouse button menu and side bar fixes.
// 
//    Rev 2.17   13 Jul 1995 13:39:04   JAY
// Only sets "landscape" for backgrounds if the aspect warrants it.
// 
//    Rev 2.16   12 Jul 1995 08:24:04   JAY
// New card style.
// 
//    Rev 2.15   26 Jun 1995 16:56:16   JAY
// No longer sets the select handles for a pasted object.
// I don't know why it was doing that. Hopefully, I'll find out why someday.
// 
//    Rev 2.14   23 Jun 1995 13:12:40   JAY
// Now uses MulDivLong to prevent overflow.
// 
//    Rev 2.13   17 May 1995 15:42:26   JAY
//  
// 
//    Rev 2.12   11 May 1995 09:39:06   JAY
// Now uses unrotated bounds in duplicate.
// 
//    Rev 2.11   10 May 1995 13:18:58   JAY
// Support for lines, ellipses, and rectangles.
// 
//    Rev 2.10   02 May 1995 15:39:36   JAY
// More smart fields stuff.
// 
//    Rev 2.9   27 Apr 1995 13:26:58   FRED
//  
// 
//    Rev 2.8   25 Apr 1995 08:58:26   JAY
// Added view parameter to warp text dialog.
// 
//    Rev 2.7   20 Apr 1995 16:17:24   FRED
// Warp Text Stuff
// 
//    Rev 2.6   19 Apr 1995 14:45:30   FRED
// Added preliminary support for warped text object
// 
//    Rev 2.5   29 Mar 1995 09:16:14   JAY
// Lots of changes
// 
//    Rev 2.4   20 Mar 1995 16:36:10   JAY
// Beginning of OLE support.
// 
//    Rev 2.3   09 Mar 1995 08:29:00   JAY
// More undo.
// 
//    Rev 2.2   15 Feb 1995 17:15:32   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:36:14   JAY
// Reverted. New series.
// 
//    Rev 1.4   07 Feb 1995 08:39:00   JAY
// Added PrePaste() to allow control of conditions before pasting objects.
// 
//    Rev 1.3   30 Jan 1995 09:34:26   JAY
// Cut and paste
// 
//    Rev 1.2   09 Jan 1995 16:58:36   JAY
// Cut & paste, etc.
// 
//    Rev 1.1   04 Aug 1994 14:31:20   JAY
// Code and support for fixing picture panel missing name bug.
// 
//    Rev 1.0   23 May 1994 10:50:28   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "action.h"
#include "pmwcolor.h"
#include "utils.h"

#include "grpobj.h"
#include "calobj.h"
#include "grafobj.h"
#include "backobj.h"
#include "frameobj.h"
#include "dateobj.h"
#include "oleobj.h"
#include "brdrobj.h"

#include "framerec.h"
#include "textrec.h"
#include "textflow.h"

#include "commands.h"
#include "ccommand.h"
#include "tblcmd.h"
#include "db386.h"			// for MulDivLong

#include "tblobj.h"
#include "celobj.h"

#include "cntritem.h"


/*
// Process the action of an object.
// Calendar actions are in here, too, in case some other project type has
// created calendar objects.
*/

void CPmwDoc::process_action(CView* view, PMG_OBJECT_ACTION action, PMGPageObjectPtr object, LPVOID data, PPNT p)
{
	switch (action)
	{
		case ACTION_DO_NOTHING:
		{
		/* Won't you listen to what the man said...? */
			break;
		}
		case ACTION_SELECT:
		{
			PMGPageObjectPtr new_obj = (PMGPageObjectPtr)data, old_obj;

			if ((old_obj = selected_objects()) != NULL)
			{
				if ((old_obj->get_select_flags() & SELECT_FLAG_solo_select)
						|| (new_obj->get_select_flags() & SELECT_FLAG_solo_select)
						|| IsGroupedFrame(old_obj))
				{
				/* Only me! */
					deselect_all();
				}
			}
			select_object(new_obj);
			break;
		}
		case ACTION_DEFAULT_EDIT:
		{
			PMGPageObjectPtr o = (PMGPageObjectPtr)data;

			switch (o->type())
			{
				case OBJECT_TYPE_Component:
				{
					((CPmwView *)view)->EditComponentObject((ComponentObject*)o, &p);
					break;
				}
				case OBJECT_TYPE_Frame:
				{
					((CPmwView *)view)->edit_frame((CFrameObject*)o, &p);
					break;
				}
				case OBJECT_TYPE_Table:
				{
					if (!object_selected(object))
					{
						select_object(object);
					}
					break;
				}
				case OBJECT_TYPE_OleObject:
				{
					COleClientItem* pItem = ((OleObject*)o)->Item();
					if (pItem != NULL)
					{
						if (view->GetSafeHwnd() == ::GetCapture())
						{
							ReleaseCapture();
						}
						pItem->DoVerb(OLEIVERB_PRIMARY, view);
						if (pItem->IsInPlaceActive())
						{
							((CPmwCntrItem*)pItem)->SetExtentAndRects();
						}
					}
					break;
				}
				case OBJECT_TYPE_Border:
				{
					((BorderObject*)o)->ShowBorderEditor();
					break;
				}
				case OBJECT_TYPE_Ellipse:
				case OBJECT_TYPE_Rectangle:
				case OBJECT_TYPE_Line:
				case OBJECT_TYPE_DrawingObject:
				{
					((CPmwView*)view)->OnPictureAttributes();
					break;
				}
				case OBJECT_TYPE_Serif:
				{
					COleClientItem* pItem = ((OleObject*)o)->Item();
					if (pItem != NULL)
					{
						if (view->GetSafeHwnd() == ::GetCapture())
						{
							ReleaseCapture();
						}
#if 0
						pItem->Run();
						if (pItem->IsRunning())
						{
							// Convert bound dimensions to HIMETRIC.
							PBOX Bound = o->get_bound();
							CSize czExtent;
							czExtent.cx = scale_pcoord(Bound.x1-Bound.x0, 2540, PAGE_RESOLUTION);
							czExtent.cy = scale_pcoord(Bound.y1-Bound.y0, 2540, PAGE_RESOLUTION);
							TRY
								pItem->SetExtent(czExtent);
							END_TRY
							pItem->DoVerb(OLEIVERB_OPEN, view);
							ASSERT(!pItem->IsInPlaceActive());
						}
#else
						pItem->DoVerb(OLEIVERB_OPEN, view);
						ASSERT(!pItem->IsInPlaceActive());
#endif
					}
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		case ACTION_SELECT_DATE:
		{
			CalendarObjectPtr calobj = (CalendarObjectPtr)object->get_container();

			calobj->set_current_date(*(SHORT far *)data);

			deselect_all();
			select_object(object);
//			od("Selected date %d\r\n", calobj->get_current_date());
			break;
		}
		case ACTION_EDIT_DATE:
		{
			CalendarObjectPtr calobj = (CalendarObjectPtr)object->get_container();
			CFrameObject* fobject;

			if ((fobject = calobj->get_date_text(*(SHORT far *)data, TRUE)) != NULL)
			{
				((CPmwView *)view)->edit_frame(fobject, &p);
			}
			break;
		}
		case ACTION_EDIT_TITLE:
		{
			break;
		}
		case ACTION_SELECT_CELLS:
		{
			select_object(object);
			break;
		}
		default:
		{
			break;
		}
	}
}

/*
// Add a refresh extent for an object.
*/

VOID CPmwDoc::refresh_object(PMGPageObjectPtr object)
{
	PBOX bound;

	object->get_refresh_bound(&bound);

	add_refresh(&bound);
}

/*
// Select an object.
*/

VOID CPmwDoc::select_object(PMGPageObjectPtr object, BOOL fUpdateMenu /*=TRUE*/)
{
	// MLSTODO: fUpdateMenu is now unused. Remove it!

	PMGPageObjectPtr sobj, obj;
	PMGPageObjectPtr far *psobj;

	if (object == NULL)
	{
	/* As a safety. This will actually happen. */
		return;
	}

/*
// Select an object.
// The order of objects in the selected list is the same as that of the
// current page list. This will make other sections of code easier.
*/

	obj = objects();

	psobj = &selected_list;
	sobj = *psobj;

/*
// Search in the current list for the object we want selected.
// As we move along, we update psobj to indicate where we'll insert.
*/

	while (obj != NULL)
	{
		if (obj == object)
		{
		/* We found the object. */
			if (object == sobj)
			{
			/* Already selected! */
				return;
			}
			break;
		}

	/* Hit the next object. Move to the next object. */

		if (obj == sobj)
		{
			psobj = sobj->next_selected_address();
			sobj = *psobj;
		}
		obj = (PMGPageObjectPtr)obj->next_object();
	}

/*
// If we ran out of objects,
// search the remainder of the selected list for the object we want to select
// to make sure it's not already selected. Then insert at the end.
*/

	if (obj == NULL)
	{
		while (sobj != NULL)
		{
			if (object == sobj)
			{
			/* Already selected. */
				return;
			}
			psobj = sobj->next_selected_address();
			sobj = *psobj;
		}
	}

/*
// Not selected yet!
// Do the insert now.
*/

	object->set_next_selected_object(sobj);
	*psobj = object;

	object->OnSelectChange(TRUE);
	toggle_object(object);

	if (object->type() == OBJECT_TYPE_CalendarDate)
		GET_PMWAPP()->ShowHelpfulHint("CalMarkSpecial");
}

/*
// Select all objects, or all objects within an area.
*/

VOID CPmwDoc::select_all(PBOX_PTR pbox)
{
	PMGPageObjectPtr object;


/* A solo object should go away. */

	if (selected_list != NULL
 				&& ((selected_list->get_select_flags() & SELECT_FLAG_solo_select)
						|| IsGroupedFrame(selected_list)))
					 
	{
		deselect_all();
	}

/* Select all the objects we can. */

	for (object = objects(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		PBOX bound = object->get_bound();

		if ((pbox == NULL || IsBoxInBox(&bound, pbox))
				&& !(object->get_select_flags() & SELECT_FLAG_solo_select)
				&& !(object->get_flags() & OBJECT_FLAG_locked))
		{
			select_object(object);
		}
	}
}

void CPmwDoc::DeselectInPlaceObject(void)
{
	UpdateAllViews(NULL, HINT_CancelEdit);
}

/*
// Deselect an object.
*/

VOID CPmwDoc::deselect_object(PMGPageObjectPtr object)
{
	PMGPageObjectPtr object_next;
	BOOL was_selected = FALSE;

	object_next = object->next_selected_object();

	if (selected_list == object)
	{
	/* First in list. Handle specially (code is simpler). */
		selected_list = object_next;
		was_selected = TRUE;
	}
	else
	{
		PMGPageObjectPtr obj, next;

		for (obj = selected_list; obj != NULL; obj = next)
		{
			next = obj->next_selected_object();
			if (next == object)
			{
				obj->set_next_selected_object(object_next);
				was_selected = TRUE;
				break;
			}
		}
	}

/* Toggle it. */

	if (was_selected)
	{
		object->OnSelectChange(FALSE);
		toggle_object(object);
	}

	DeselectInPlaceObject();
}

/*
// Deselect all objects.
*/

void CPmwDoc::deselect_all(BOOL fUpdateMenu /*=TRUE*/)
{
	// MLSTODO: fUpdateMenu is now unused. Remove it!

	// Tell each object it is no longer selected.
	PMGPageObject* pObject = selected_list;
	while (pObject != NULL)
	{
		pObject->OnSelectChange(FALSE);
		pObject = pObject->next_selected_object();
	}

	toggle_selects();

	selected_list = NULL;

	DeselectInPlaceObject();
}

/*
// Toggle the select marks for all objects in a view.
// Multi-purpose routine.
//
// view = view to toggle in or NULL (all views)
// hdc = hdc or 0 (use view's hdc)
// object = object to toggle or NULL (all objects)
// which = which selection attribute to toggle
*/

BOOL CPmwDoc::toggle_selects(CView *view, HDC hdc, PMGPageObjectPtr object, SHORT which)
{
	BOOL result = FALSE;
	if (view == NULL)
	{
	/* Do all views. */

		POSITION pos = GetFirstViewPosition();
		while (pos)
		{
		/* hdc can't be valid here. */
			if (toggle_selects(GetNextView(pos), 0, object, which))
			{
				result = TRUE;
			}
		}
	}
	else
	{
	/* In this view. */

		if (object == NULL)
		{
		/* Do all objects. */
			for (object = selected_list; object != NULL; object = object->next_selected_object())
			{
				if (toggle_selects(view, hdc, object, which))
				{
					result = TRUE;
				}
			}
		}
		else
		{
		/* This object. */

			if (((CPmwView *)view)->toggle_selects(which, object, hdc))
			{
				result = TRUE;
			}
		}
	}
	return result;
}

/*
// Append an object to the list.
*/

VOID CPmwDoc::append_object(PMGPageObjectPtr object)
{
	if (page_objects != NULL)
	{
		page_objects->append(object);
		SetModifiedFlag();
	}
}

/*
// Insert an object at a specific location.
*/

void CPmwDoc::insert_object(PMGPageObjectPtr object, int nIndex)
{
	if (nIndex <= 0)
	{
		page_objects->prepend(object);
	}
	else if (nIndex >= page_objects->count())
	{
		page_objects->append(object);
	}
	else
	{
		page_objects->insert_before(object, page_objects->element_in_list(nIndex));
	}
}

/*
// Detach an object from the list.
// All references to this object must be removed.
*/

VOID CPmwDoc::detach_object(PMGPageObjectPtr object)
{
	if (page_objects != NULL)
	{
	/* Inject a feeling of detachment. */

		page_objects->detach(object);

	/* Update all view references to this object. */

		POSITION pos = GetFirstViewPosition();
		while (pos)
		{
			CView* pView = GetNextView(pos);
			if (pView->IsKindOf(RUNTIME_CLASS(CPmwView)))
			{
				((CPmwView *)pView)->removing_object(object);
			}
		}
	}
}

/*
// Delete an object from the document.
*/

VOID CPmwDoc::delete_object(PMGPageObjectPtr object)
{
	if (object != NULL)
	{
	/* Make sure we're not selected. */
		deselect_object(object);

	/* Refresh our bound to show our disappearance.	*/
		refresh_object(object);

	/* Remove the object from the document list. */
		detach_object(object);

	/* Delete any of the object's dependents in the document. */
		object->destroy();

	/* Free the object from memory. */
		delete object;

	/* We're now modified. */
		SetModifiedFlag();
	}
}

/*
// Send a calc message to all selected objects.
*/

FLAGS CPmwDoc::calc_selects(BOOL set_manual_layout)
{
	FLAGS change_flags = 0;

	for (PMGPageObjectPtr object = selected_list; object != NULL; object = object->next_selected_object())
	{
		FLAGS obj_flags = object->get_flags();

	/* Set manual layout if desired. */

		if (set_manual_layout)
		{
			object->set_new_layout(MANUAL_LAYOUT);
		}

	/* Calc the object. */

		calc_object(object);

		change_flags |= (obj_flags ^ object->get_flags());
	}
	return change_flags;
}

/*
// Confirm the deletion of one or more objects.
*/

BOOL CPmwDoc::confirm_deletion(PMGPageObjectPtr object)
{
	SHORT type;
	SHORT count;

	if (object == NULL)
	{
		type = 0;
		count = 0;

	/* Determine the nature of the list. */

		for (object = selected_list; object != NULL; object = object->next_selected_object())
		{
			count++;
			SHORT object_type = object->type();

			if (type == 0)
			{
				type = object_type;
			}
			else if (type != object_type)
			{
				type = -1;
			}
		}
	}
	else
	{
		type = object->type();
		count = 1;
	}

/* See if the user really wants to delete the objects. */

	// JRO CHAR *object_plural = "s";
	// JRO CHAR *object_name;
   const CHAR *object_plural = "s";
   const CHAR *object_name;
	CHAR name_buffer[40];

	switch (type)
	{
		case OBJECT_TYPE_Graphic:
		{
			object_name = NULL;

			if (count == 1)
			{
				if (object == NULL)
				{
					object = selected_list;
				}
				if (object->get_name(name_buffer+1, sizeof(name_buffer)-2, -1))
				{
					object_name = name_buffer;
					name_buffer[0] = '\"';
					strcat(name_buffer, "\"");
				}
			}

			if (object_name == NULL)
			{
				// JRO object_name = "the selected picture";
            object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_PICTURE);
			}
			break;
		}
		case OBJECT_TYPE_Frame:
		{
			// JRO object_name = "the selected text box";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_TBOX);
			//object_plural = "es";
         object_plural = GET_PMWAPP()->GetResourceStringPointer (IDS_IDS_THE_SEL_TBOX_PL);
			break;
		}
		case OBJECT_TYPE_Background:
		{
			// JRO object_name = "the background";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_BACKGROUND);
			break;
		}
		case OBJECT_TYPE_Group:
		{
			// JRO object_name = "the selected group";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_GROUP);
			break;
		}
		case OBJECT_TYPE_Line:
		{
			// JRO object_name = "the selected line";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_LINE);
			break;
		}
		case OBJECT_TYPE_Rectangle:
		{
			// JRO object_name = "the selected rectangle";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_RECT);
			break;
		}
		case OBJECT_TYPE_Ellipse:
		{
			// JRO object_name = "the selected circle";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_CIRCLE);
			break;
		}
		case OBJECT_TYPE_DrawingObject:
		{
			// JRO object_name = "the selected shape";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_SHAPE);
			break;
		}
		default:
		{
			// JRO object_name = "the selected object";
         object_name = GET_PMWAPP()->GetResourceStringPointer (IDS_THE_SEL_OBJ);
			break;
		}
	}

	char buffer[120];

	sprintf(buffer,
					// JRO "Are you sure you want to remove %s%s?",
               GET_PMWAPP()->GetResourceStringPointer (IDS_Q_SURE_REMOVE),
					object_name,
					count > 1 ? object_plural : "");

	return AfxMessageBox(buffer, MB_YESNO | MB_ICONQUESTION) == IDYES;
}

/*
// Delete all selected objects.
*/

VOID CPmwDoc::delete_selects(BOOL fConfirm /*=TRUE*/, WORD wCmdID /*=0*/)
{
	PMGPageObjectPtr pObject;

	if ((pObject = selected_objects()) != NULL)
	{
		if (pObject->type() == OBJECT_TYPE_Table
				&& ((CTableObject*)pObject)->SomethingSelected())
		{
			// Delete the cells in the table.
			CTableObject* pTable = (CTableObject*)pObject;
			CCellDeleteCommand* pCommand = new CCellDeleteCommand(IDCmd_ClearCells);
			if (pCommand->Snapshot(pTable))
			{
				// Add (and execute) the command.
				AddCommand(pCommand);
			}
			else
			{
				delete pCommand;
			}
		}
		else if (pObject->type() == OBJECT_TYPE_CalendarDate)
		{
		/*
		// We need to clear the text and picture for the current date.
		*/
			SHORT number;			/* <- this is the date number (like 1st, 2nd, etc) */
			SHORT index = get_date_selected(&number);

			//CalendarObjectPtr calobj = get_current_calendar();
			CalendarObjectPtr calobj = NULL;
			calobj = (CalendarObjectPtr)pObject->get_container();
			if (calobj == NULL)
				return;
			
			SHORT type = 0;
			char buffer[80];
			// JRO
/*
         static char *mess[] =
			{
				"nothing",
				"picture",
				"text",
				"picture and text"
			};
*/
			static const char *mess[4];
         mess [0] = GET_PMWAPP()->GetResourceStringPointer (IDS_NOTHING);
         mess [1] = GET_PMWAPP()->GetResourceStringPointer (IDS_PICTURE);
         mess [2] = GET_PMWAPP()->GetResourceStringPointer (IDS_TEXT);
         mess [3] = GET_PMWAPP()->GetResourceStringPointer (IDS_PICTURE_TEXT);

         CalendarRef calendar = calobj->get_calendar();
			SHORT first_day = calendar.get_first_day();

			if (calobj->get_date_graphic() != NULL)
			{
				type |= 1;
			}
			if (calobj->get_date_text() != NULL)
			{
				type |= 2;
			}

			if (type == 0)
			{
			/* Nothing to do. This can actually happen (DEL key from MODESIZE)... */
				return;
			}

			if (fConfirm)
			{
				// JRO sprintf(buffer, "Clear the %s for the current day?", mess[type]);
            sprintf(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_Q_CLEAR_FOR_DAY), mess[type]);
				if (AfxMessageBox(buffer, MB_YESNO | MB_ICONQUESTION) != IDYES)
				{
				/* User didn't say yes. */
					return;
				}
			}

		/* Set up the clear undo/redo command. */

			CCmdCalendarDate* pCommand = new CCmdCalendarDate(wCmdID);

			if (!pCommand->Before(this, calobj, index))
			{
				delete pCommand;
				pCommand = NULL;
			}

		/* Do the clear. */

			calobj->delete_date_text(index, FALSE);
			calobj->delete_date_graphic(index, FALSE);
			refresh_object(calobj->get_date_object(index));
			SetModifiedFlag();

		/* Finish the clear undo/redo command. */

			if (pCommand != NULL)
			{
				if (pCommand->After())
				{
					AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
					pCommand = NULL;
				}
			}
		}
		else
		{
		/*
		// Non-calendar date.
		// Handle all the objects.
		*/
			if (fConfirm && !confirm_deletion())
			{
			/* User said no. */
				return;
			}

			DoRelayerCommand(new CCmdDeleteSelects(wCmdID));
		}
	}
}

/*
// Compute the bound enclosing all selected objects. 
// You really shouldn't call this when no objects are selected!
*/

void CPmwDoc::GetSelectRefreshBound(PBOX_PTR bound, RedisplayContext* rc /*=NULL*/)
{
	PMGPageObjectPtr object = selected_list;

	if (object != NULL)
	{
		object->get_refresh_bound(bound, rc);

		while ((object = object->next_selected_object()) != NULL)
		{
			PBOX pbox;

			object->get_refresh_bound(&pbox, rc);

			UnionBox(bound, bound, &pbox);
		}
	}
}

BOOL CPmwDoc::GetSelectBound(PBOX_PTR pBounds)
{
	PMGPageObjectPtr pObject;
	BOOL fGotOne = FALSE;

	for (pObject = selected_objects(); pObject != NULL; pObject = pObject->next_selected_object())
	{
		PBOX ObjBound;

		ObjBound = ((RectPageObjectPtr)pObject)->get_bound();

		if (fGotOne)
		{
			UnionBox(pBounds, pBounds, &ObjBound);
		}
		else
		{
			*pBounds = ObjBound;
			fGotOne = TRUE;
		}
	}

	if (!fGotOne)
	{
		pBounds->x0 =
			pBounds->y0 =
			pBounds->x1 =
			pBounds->y1 = 0;
	}

	return fGotOne;
}

BOOL CPmwDoc::something_selected(VOID)
{
	return selected_list != NULL
				&& !(selected_list->get_select_flags() & SELECT_FLAG_solo_select)
				&& !(selected_list->get_flags() & OBJECT_FLAG_grouped)
				&& selected_list->get_panel() < number_of_panels();
}

/*
// Return whether we can perform the ungroup command or not.
*/

BOOL CPmwDoc::can_ungroup(VOID)
{
/* Look through the objects for a group. */

	for (PMGPageObjectPtr object = selected_list;
 					object != NULL;
 					object = object->next_selected_object())
	{
		if (object->type() == OBJECT_TYPE_Group)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
// Return whether we can perform the group command or not.
*/

BOOL CPmwDoc::can_group(PBOX_PTR bound)
{
	SHORT count = 0;

/* See if any objects are going to join the group. */

	for (PMGPageObjectPtr object = selected_list;
 					object != NULL;
 					object = object->next_selected_object())
	{
		if (object->join_group(NULL, NULL))
		{
			if (bound != NULL)
			{
				PBOX obj_bound = object->get_bound();
				if (count == 0)
				{
					*bound = obj_bound;
				}
				else
				{
					UnionBox(bound, bound, &obj_bound);
				}
			}
			count++;
		}
	}

	return count > 1;
}

/*
// Group all selected objects.
*/

VOID CPmwDoc::group_selects(VOID)
{
	GROUP_CREATE_STRUCT gcs;

	SetModifiedFlag();

/* See if anybody can come out to play. */

	if (can_group(&gcs.original_bound))
	{
		GroupObjectPtr group;

	/* Make the group. */

		if ((group = mapping.database->create_group_object(&gcs)) != NULL)
		{
			group->set_panel(get_current_panel());

			CCmdGroupSelects* pCommand = new CCmdGroupSelects;

			if (pCommand->Snapshot(GetCurrentView(), group))
			{
			/* Add the command and "do it". */
				AddCommand(pCommand);
			}
			else
			{
			// Deleting the command takes care of the group.
				delete pCommand;
			}
		}
	}
}

/*
// Ungroup all selected groups.
*/

VOID CPmwDoc::ungroup_selects(VOID)
{
	PMGPageObjectPtr object, next;

	SetModifiedFlag();

/*
// Create the undo/redo object.
*/

	CCmdMulti* pCommand = new CCmdMulti(IDCmd_UngroupSelects);

/* Look through the objects for a group. */

	for (object = selected_list; object != NULL; object = next)
	{
		next = object->next_selected_object();

		if (object->type() == OBJECT_TYPE_Group)
		{
			CCmdUngroupSelects* pSubCommand = new CCmdUngroupSelects;

			if (pSubCommand->Snapshot(GetCurrentView(), (GroupObject*)object))
			{
				pCommand->AddCommand(pSubCommand);
			}
			else
			{
				delete pSubCommand;
			}
		}
	}

	if (pCommand->HasCommands())
	{
		AddCommand(pCommand);
	}
	else
	{
		delete pCommand;
	}
}

/*
// Send a xflip message to all selected objects.
*/

VOID CPmwDoc::xflip_selects(VOID)
{
	CCmdFlipSelects* pCommand = new CCmdFlipSelects(IDCmd_Mirror);

	if (pCommand->Snapshot(GetCurrentView()))
	{
	/* Add the undo/redo command and do it! */
		AddCommand(pCommand);
	}
	else
	{
		delete pCommand;
	/* Do it by hand. */
		SetModifiedFlag();

		toggle_selects();

		for (PMGPageObjectPtr object = selected_list; object != NULL; object = object->next_selected_object())
		{
			refresh_object(object);
			object->xflip();
			refresh_object(object);
		}

		toggle_selects();
	}
}

/*
// Send a yflip message to all selected objects.
*/

VOID CPmwDoc::yflip_selects(VOID)
{
	CCmdFlipSelects* pCommand = new CCmdFlipSelects(IDCmd_Flip);

	if (pCommand->Snapshot(GetCurrentView()))
	{
	/* Add the undo/redo command and do it! */
		AddCommand(pCommand);
	}
	else
	{
		delete pCommand;
	/* Do it by hand. */
		SetModifiedFlag();

		for (PMGPageObjectPtr object = selected_list; object != NULL; object = object->next_selected_object())
		{
			refresh_object(object);
			object->yflip();
			refresh_object(object);
		}
	}
}

/*
// Duplicate all objects on the selected list.
*/

VOID CPmwDoc::duplicate_selects(VOID)
{
	PMGPageObjectPtr object = selected_list;

	if (object != NULL											/* something selected */
 			&& object->next_selected_object() == NULL		/* one object */
//			&& object->type() == OBJECT_TYPE_Graphic		/* one picture */
		 )
	{
		if (duplicate_picture())
		{
		/* Handled! */
			return;
		}
	}

	CMapPtrToPtr Map;
	PMGDatabase* pDatabase = get_database();

	SetModifiedFlag();

	ObjectList dup_list(pDatabase);

	for ( ; object != NULL; object = object->next_selected_object())
	{
		PMGPageObjectPtr new_object;

		if ((new_object = (PMGPageObjectPtr)object->duplicate()) != NULL)
		{
			PMGDatabase::MapCopiedFrames(object, new_object, Map);

			new_object->set_new_layout(MANUAL_LAYOUT);

			PBOX bound = ((RectPageObjectPtr)new_object)->get_unrotated_bound();

		/* Offset it by a quarter inch. */

			bound.x0 += PAGE_RESOLUTION/4;
			bound.y0 += PAGE_RESOLUTION/4;
			bound.x1 += PAGE_RESOLUTION/4;
			bound.y1 += PAGE_RESOLUTION/4;

			((RectPageObjectPtr)new_object)->set_unrotated_bound(bound);
			new_object->calc();

		/*
 		// Where does the new object go? Maybe right next to the original?
 		// Append it for now.
 		*/

			dup_list.append(new_object);
		}
	}

	// Do a local deselect_all().
	toggle_selects();
	selected_list = NULL;

	// Now add all the created objects.
	for (object = (PMGPageObjectPtr)dup_list.first_object();
					object != NULL;)
	{
		PMGPageObjectPtr next = (PMGPageObjectPtr)object->next_object();

		dup_list.detach(object);			/* Remove it from the old list. */
		append_object(object);				/* Add to list */
		refresh_object(object);				/* Make it show up */
		select_object(object);				/* Select it */

		object = next;
	}

	// Relink the copied frames.
	pDatabase->LinkCopiedFrames(pDatabase, Map, TRUE);

	// Add a create event for this.
	AddCreateCommand(IDCmd_Duplicate);

	// Handle flow.
	MarkFramesBelowSelectedObjects();
	UpdateMarkedFrames(TRUE);
}

void CPmwDoc::SetSelectedProperties(CObjectProperties& Properties, CPmwView* pView, WORD wCmdID)
{
	PMGPageObject* pObject = selected_list;

	if (pObject == NULL)
	{
		// Merge in the new properties.
		m_DefaultProperties.MergeWith(Properties);
	}
	else
	{
	// Create an undo/redo command.

		CCmdChangeSelects* pCommand = NULL;

		if (pView != NULL && wCmdID != 0)
		{
			pCommand = new CCmdChangeSelects(wCmdID);
			if (!pCommand->Before(pView, FALSE))
			{
				delete pCommand;
				pCommand = NULL;
			}
		}

		BOOL fModified = FALSE;

		// Do the change.
		for (; pObject != NULL; pObject = pObject->next_selected_object())
		{
			PBOX OldBound;
			pObject->get_refresh_bound(&OldBound);
			if (pObject->SetObjectProperties(Properties))
			{
				fModified = TRUE;
				add_refresh(&OldBound);
				refresh_object(pObject);
			}
		}

		// Finish the undo/redo command.
		if (pCommand != NULL)
		{
			if (fModified && pCommand->After())
			{
				AddCommand(pCommand, FALSE);
			}
			else
			{
				delete pCommand;
			}
		}
		if (fModified)
		{
			SetModifiedFlag();
		}
	}
}

void CPmwDoc::GetSelectedProperties(CObjectProperties& Properties)
{
	PMGPageObject* pObject = selected_list;

	if (pObject == NULL)
	{
		// We want to set some default somewhere.
		Properties = m_DefaultProperties;
	}
	else
	{
		// Do the change.
		for (; pObject != NULL; pObject = pObject->next_selected_object())
		{
			pObject->GetObjectProperties(Properties);
		}
	}
}

void CPmwDoc::AddCreateCommand(WORD wCmdID)
{
/* Create the undo state for this event. */
	CCmdCreateSelects* pCommand = new CCmdCreateSelects(wCmdID);
	if (pCommand != NULL)
	{
		if (pCommand->Snapshot(GetCurrentView()))
		{
			AddCommand(pCommand, FALSE);
		}
		else
		{
			delete pCommand;
		}
	}
}

/*
// Say whether this object is selected or not.
*/

BOOL CPmwDoc::object_selected(PMGPageObjectPtr object)
{
	PMGPageObjectPtr o;

	for (o = selected_list; o != NULL; o = o->next_selected_object())
	{
		if (o == object)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
// Return the calendar object for the currently selected date object
// (if there is one).
// If use_global is TRUE, then the calendar object remembered in the
// CCalendarDoc structure is used if nothing else is appropriate (but
// only if this is a calendar document!).
*/

CalendarObjectPtr CPmwDoc::get_current_calendar(BOOL use_global)
{
	PMGPageObjectPtr object;

	if ((object = selected_objects()) != NULL)
	{
		switch (object->type())
		{
			case OBJECT_TYPE_CalendarDate:
			{
				return (CalendarObjectPtr)object->get_container();
			}
			case OBJECT_TYPE_Calendar:
			{
				return (CalendarObjectPtr)object;
			}
			default:
			{
				break;
			}
		}
	}
	return NULL;
}

/*
// Invoke the calc() method for this object passing the current panel
// info. This enables us to process layouts correctly.
*/

VOID CPmwDoc::calc_object(PMGPageObjectPtr object, BOOL ignore_current)
{
	DB_RECORD_NUMBER layout = object->get_layout();

	if (layout <= 0)
	{
	/* The simple case. */
		object->calc();
	}
	else
	{
		SHORT panel = object->get_panel();

	/* Correct nasty panel values. */

		if (panel >= number_of_panels())
		{
			panel = 0;
		}

	/* Get the panel world and flags, and calc(). */

		PANEL_INFO *pip = info.pip + panel;

		PBOX world;
		get_panel_world(&world, panel);

		FLAGS flags = pip->flags;

		if (panel_flags != 0)
		{
		/* Panels are all flipped right-side-up. */
			flags = 0;
		}

#if 0
		if (!ignore_current && panel == current_panel)
		{
			flags ^= panel_flags;
		}
#endif

		object->calc(&world, flags);
	}
}

/*
// See if there is one object selected.
*/

BOOL CPmwDoc::one_selection(VOID)
{
	return selected_list != NULL
 				&& selected_list->next_selected_object() == NULL
				&& selected_list->type() != OBJECT_TYPE_Background
				&& !(selected_list->get_select_flags() & SELECT_FLAG_solo_select);
}

/*
// Return the currently selected date index (and possibly number).
*/

SHORT CPmwDoc::get_date_selected(LPSHORT number)
{
	PMGPageObjectPtr object;

/* See if a date is selected. */

	if ((object = selected_objects()) != NULL
			&& object->type() == OBJECT_TYPE_CalendarDate)
	{
		CalendarObjectPtr calobj = (CalendarObjectPtr)object->get_container();

		SHORT index = calobj->get_current_date();
		if (number != NULL)
		{
			*number = index + calobj->first_day();
		}
		return index;
	}

	return -1;
}

/*
// Choose a color.
*/

SHORT CPmwDoc::choose_color(COLOR &color)
{
	SHORT ret;

	CPmwColorDialog Dialog(colorref_from_color(color));

	if ((ret = Dialog.DoModal()) == IDOK)
	{
		color = color_from_colorref(Dialog.GetColor());
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Paste commands.

/*
// High-level entry for pasting an object.
*/

PMGPageObject* CPmwDoc::PasteObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds /*=NULL*/, BOOL fDuplicate /*=TRUE*/)
{
	switch (pObject->type())
	{
		case OBJECT_TYPE_Background:
		{
			return PasteBackground(pObject, pOldWorld, pBounds, fDuplicate);
		}
		default:
		{
			return DoPasteObject(pObject, pOldWorld, pBounds, fDuplicate);
		}
	}
}

/*
// Standard method for pasting a background.
*/

PMGPageObject* CPmwDoc::PasteBackground(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds /*=NULL*/, BOOL fDuplicate /*=TRUE*/)
{
	ERRORCODE error = ERRORCODE_None;
	BackgroundObjectPtr pNewObject = (BackgroundObjectPtr)DoDuplicateObject(pObject, NULL /*pOldWorld*/, NULL, fDuplicate, &error);

	if (pNewObject != NULL)
	{
		pNewObject->set_panel(get_current_panel());

	/* Out with any existing background. */

		delete_object(find_background());

	/*
	// Make the "orientation" flag match the actual document orientation.
	// This is an annoying kludge which will someday go away.
	*/

		if (get_orientation() == PORTRAIT)
		{
			pNewObject->remove_flags(OBJECT_FLAG_landscape);
		}
		else
		{
			PPNT dims;
			pNewObject->original_dims(&dims);
			if (dims.x <= dims.y)
			{
				pNewObject->add_flags(OBJECT_FLAG_landscape);
			}
		}

	/* We should get the panel dimensions here. */

		PBOX bound;
		get_panel_world(&bound);

		pNewObject->set_bound(bound);

	/* Prepend the new background. */

		attach_background(pNewObject);
		refresh_object(pNewObject);
		select_object(pNewObject);
	}
	return pNewObject;
}

/*
// Alternate method for pasting a background.
*/

PMGPageObject* CPmwDoc::PasteBackgroundAsPicture(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds)
{
	GraphicObjectPtr pNewObject = ConvertBackgroundToGraphic((BackgroundObjectPtr)pObject);

	if (pNewObject != NULL)
	{
	/* Give the object the correct size. */

		ResizePastedObject(pNewObject, pOldWorld, pBounds);

	/* Prepend the new "background". */

		page_objects->prepend(pNewObject);
		refresh_object(pNewObject);
		select_object(pNewObject);
	}
	return pNewObject;
}

/*
// Convert a background object to a graphic object.
*/

GraphicObjectPtr CPmwDoc::ConvertBackgroundToGraphic(BackgroundObjectPtr pObject)
{
	GraphicObjectPtr pNewObject = get_database()->create_graphic_object();

	if (pNewObject != NULL)
	{
	/* Assign the *graphic* component of the background. */

		FLAGS SelectFlags = pNewObject->get_select_flags();

		pNewObject->assign(*(BackgroundObjectPtr)pObject);

		// "Fixup" the object to make it act like a graphic again.
		pNewObject->set_select_flags(SelectFlags);
		pNewObject->remove_flags(OBJECT_FLAG_no_mask | OBJECT_FLAG_landscape);
		pNewObject->remove_refresh_flags(REFRESH_FLAG_opaque);
		pNewObject->set_panel(get_current_panel());
	}
	return pNewObject;
}

/*
// Low-level entry for pasting an object.
*/

PMGPageObject* CPmwDoc::DoPasteObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds /*=NULL*/, BOOL fDuplicate /*=TRUE*/)
{
	ERRORCODE error = ERRORCODE_None;
	PMGPageObjectPtr pNewObject = (PMGPageObjectPtr)DoDuplicateObject(pObject, pOldWorld, pBounds, fDuplicate, &error);

	if (pNewObject != NULL)
	{
		// We need to do this in case we are pasting a calendar date.
		if (pNewObject->get_flags() & OBJECT_FLAG_grouped)
		{
			ASSERT(pNewObject->get_panel() >= MAIN_CALENDAR_PANEL);
			// This is probably a calendar date paste.
			pNewObject->remove_flags(OBJECT_FLAG_grouped);
			pNewObject->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_handles_no_rotate);
			if (pNewObject->type() == OBJECT_TYPE_Graphic)
			{
				((GraphicObject*)pNewObject)->UpdateRotateHandle();
			}
		}

		pNewObject->set_panel(get_current_panel());

		append_object(pNewObject);
		// Remove hyperlinks from pasted objects
		if (pNewObject->type() == OBJECT_TYPE_Frame)
		{
			((CFrameObject*)pNewObject)->RemoveHyperlink();	
		}
		else
		{
			pNewObject->KillHyperlinkRecord();
		}
	
		switch (pNewObject->type())
		{
			case OBJECT_TYPE_Frame:
			{
				((CFrameObject*)pNewObject)->ReflowMacros();
				break;
			}
		}
		
		refresh_object(pNewObject);
		select_object(pNewObject);
	}
	return pNewObject;
}

/*
// Low-level entry for duplicating an object into our database.
*/

PMGPageObject* CPmwDoc::DoDuplicateObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate, ERRORCODE* pError)
{
	PMGPageObject* pNewObject;

	if (fDuplicate)
	{
		if ((pNewObject = (PMGPageObject*)pObject->duplicate(mapping.database, pError)) != NULL)
		{
			ResizePastedObject((RectPageObject*)pNewObject, pOldWorld, pBounds);
		}
	}
	else
	{
		pNewObject = (PMGPageObject*)pObject;
		handle_new_object((RectPageObject*)pNewObject, NULL, FALSE);
	}
	calc_object(pNewObject);

	return pNewObject;
}

static void ScalePBOX(PBOX* pBox, const PBOX* pNewBound, const PBOX* pOldBound, BOOL fProportional)
{
	PPNT OldOrigin;
	OldOrigin.x = pOldBound->x0;
	OldOrigin.y = pOldBound->y0;

	PPNT OldDims;
	OldDims.x = pOldBound->x1 - pOldBound->x0;
	OldDims.y = pOldBound->y1 - pOldBound->y0;

	PPNT Dims;
	Dims.x = pNewBound->x1 - pNewBound->x0;
	Dims.y = pNewBound->y1 - pNewBound->y0;

	PCOORD OldWidth = pBox->x1 - pBox->x0;
	PCOORD OldHeight = pBox->y1 - pBox->y0;

/* Normalize */

	pBox->x0 -= OldOrigin.x;
	pBox->y0 -= OldOrigin.y;
	pBox->x1 -= OldOrigin.x;
	pBox->y1 -= OldOrigin.y;

/* Scale. */

	pBox->x0 = MulDivLong(pBox->x0, Dims.x, OldDims.x);
	pBox->y0 = MulDivLong(pBox->y0, Dims.y, OldDims.y);
	pBox->x1 = MulDivLong(pBox->x1, Dims.x, OldDims.x);
	pBox->y1 = MulDivLong(pBox->y1, Dims.y, OldDims.y);

/* Denormalize */

	pBox->x0 += pNewBound->x0;
	pBox->y0 += pNewBound->y0;
	pBox->x1 += pNewBound->x0;
	pBox->y1 += pNewBound->y0;

/* Now make it proportional if desired. */

	if (fProportional)
	{
	/*
	// Make it proportional. This means fitting it (proportionally)
	// into the box that was just formed.
	*/
		PCOORD NewWidth = pBox->x1 - pBox->x0;
		PCOORD NewHeight = pBox->y1 - pBox->y0;

		PCOORD PropHeight;

		if ((PropHeight = MulDivLong(NewWidth, OldHeight, OldWidth)) > NewHeight)
		{
			pBox->x1 = pBox->x0 + MulDivLong(NewHeight, OldWidth, OldHeight);
		}
		else
		{
			pBox->y1 = pBox->y0 + PropHeight;
		}
	}
}

void CPmwDoc::ResizePastedObject(RectPageObjectPtr pNewObject, PBOX* pOldWorld, PBOX* pBounds /*=NULL*/, BOOL fProportional /*=TRUE*/)
{
/* Resize the object to fit the new world if we have an old world. */
	if (pOldWorld != NULL)
	{
	/* Get the new world dimensions. */
		PBOX World;
		get_panel_world(&World);

		if (memcmp(pOldWorld, &World, sizeof(World)) != 0)
		{
			PBOX ObjectBound = pNewObject->get_unrotated_bound();

			if (pBounds == NULL)
			{
				ScalePBOX(&ObjectBound, &World, pOldWorld, fProportional);
			}
			else
			{
				PBOX Bounds = *pBounds;

				ScalePBOX(&Bounds, &World, pOldWorld, fProportional);
				ScalePBOX(&ObjectBound, &Bounds, pBounds, FALSE);
			}

			pNewObject->set_unrotated_bound(ObjectBound);
			pNewObject->calc();
		}
	}
}

PMGPageObject* CPmwDoc::CopyObject(PMGPageObjectPtr pObject, ObjectListPtr pObjects)
{
	PMGPageObjectPtr pNewObject = (PMGPageObjectPtr)pObject->duplicate(pObjects->get_database());
	pObjects->append(pNewObject);
	return pNewObject;
}

void CPmwDoc::PrePaste(void)
{
	deselect_all();
}

void CPmwDoc::PostPaste(BOOL fAddUndo /*=TRUE*/)
{
	resize_project();
	SetModifiedFlag();

	if (fAddUndo)
	{
		// add an undo command if requested
		AddCreateCommand(IDCmd_PasteObjects);
	}

	MarkFramesBelowSelectedObjects();
	UpdateMarkedFrames(TRUE);
}

void CPmwDoc::AbortPaste(void)
{
}

/*
// Paste the sentiment into the document.
*/

void CPmwDoc::PasteSentiment(const CMlsStringArray& csaSentiment)
{
	// Damn! Can't handle it here. Send it up to the view.
	// Remove this when the browser is changed to call the view directly.
	GetCurrentView()->PasteSentiment(csaSentiment);
}

/*
// Add a command to the command list, "doing" the command if requested.
*/

BOOL CPmwDoc::AddCommand(CCommand* pCommand, BOOL fDoIt /*=TRUE*/)
{
// Actually do the command if requested.
	if (fDoIt)
	{
		if (!pCommand->DoIt())
		{
			return FALSE;
		}
	}
// Add the command to the undo list.
	m_CommandList.AddCommand(pCommand);
	return TRUE;
}

void CPmwDoc::ClearAllCommands(void)
{
	m_CommandList.ClearAll();
}

ERRORCODE CPmwDoc::CreateObjectFromFile(LPCSTR pName, PPNT* pWhere)
{
	ERRORCODE error = ERRORCODE_None;

	BeginWaitCursor();

	ASSERT(pName != NULL);

	CPmwCntrItem* pItem = NULL;

	TRY
	{
	/*
	// Get the object size before we create the item.
	*/
		// Create new item connected to this document.
		ASSERT_VALID(this);
		pItem = new CPmwCntrItem(this);
		ASSERT_VALID(pItem);

		// Initialize the item from the data object.
		if (!pItem->CreateFromFile(pName))
		{
			error = ERRORCODE_IllegalType;
			AfxThrowMemoryException();  // any exception will do
		}
		ASSERT_VALID(pItem);

		if ((error = FinishOleObjectCreate(pItem, pWhere)) != ERRORCODE_None)
		{
			AfxThrowMemoryException();  // any exception will do
		}
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}

		if (error == ERRORCODE_None)
		{
			error = ERRORCODE_Memory;
		}
	}
	END_CATCH

	EndWaitCursor();
	return error;
}

ERRORCODE CPmwDoc::FinishOleObjectCreate(CPmwCntrItem* pItem, PPNT* pWhere /*=NULL*/, int nObjectType /*=OBJECT_TYPE_OleObject*/)
{
	ERRORCODE error = ERRORCODE_None;
	OleObject* pObject = NULL;

	TRY
	{
		// Create the object to hold this item.

		PMGDatabase* pDatabase = get_database();

		OLEOBJECT_CREATE_STRUCT ocs;

		CSize czExtent;

		pItem->GetExtent(&czExtent);

		if (nObjectType == OBJECT_TYPE_Serif)
		{
			// Serif drawing objects come in 1" by 1". This is too small.
			// Try 2" by 2". That way, they will not be too big for labels, etc.
			ocs.m_OriginalSize.x = 2*PAGE_RESOLUTION;
			ocs.m_OriginalSize.y = 2*PAGE_RESOLUTION;
		}
		else
		{
			ocs.m_OriginalSize.x = scale_pcoord(czExtent.cx, PAGE_RESOLUTION, 2540);
			ocs.m_OriginalSize.y = scale_pcoord(czExtent.cy, PAGE_RESOLUTION, 2540);
		}

		if (pWhere == NULL)
		{
			ocs.m_Bound.x0 = 0;
			ocs.m_Bound.y0 = 0;
		}
		else
		{
			ocs.m_Bound.x0 = pWhere->x;
			ocs.m_Bound.y0 = pWhere->y;
		}
		ocs.m_Bound.x1 = ocs.m_Bound.x0 + ocs.m_OriginalSize.x;
		ocs.m_Bound.y1 = ocs.m_Bound.y0 + ocs.m_OriginalSize.y;
		ocs.m_pItem = pItem;

		pObject = (OleObject*)pDatabase->new_object(nObjectType, &ocs);
		if (pObject == NULL)
		{
			error = pDatabase->last_creation_error();
			AfxThrowMemoryException();
		}

		// Hook us the other way.
		pItem->Object(pObject);
		pObject->set_panel(get_current_panel());

		ASSERT_VALID(pItem);

		if (pWhere == NULL)
		{
			PasteObject(pObject, NULL, NULL, FALSE);
		}
		else
		{
			append_object(pObject);
			select_object(pObject);
			refresh_object(pObject);
			SetModifiedFlag();
		}
	}
	CATCH(CException, e)
	{
		if (pObject != NULL)
		{
			delete pObject;
		}

		if (error == ERRORCODE_None)
		{
			error = ERRORCODE_Memory;
		}
	}
	END_CATCH

	return error;
}

void CPmwDoc::PasteDropFiles(HDROP hDropInfo, PPNT* pWhere)
{
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		CreateObjectFromFile(szFileName, pWhere);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Frame wrap operations

BOOL CPmwDoc::MarkFramesBelow(PMGPageObject* pBelowObject, PMGPageObject* pFirstObject /*=NULL*/)
{
	BOOL fFoundObject = FALSE;
	if (pFirstObject == NULL)
	{
		if (pBelowObject != NULL)
		{
			if (pBelowObject->type() == OBJECT_TYPE_Frame)
			{
				MarkFrame((CFrameObject*)pBelowObject);
			}

			// Continue...
			long lFlowAround = pBelowObject->GetFlowAround();
			if (lFlowAround == FLOW_AROUND_none)
			{
				// We do not flow around!
				return TRUE;
			}
		}
		pFirstObject = objects();
	}

	PBOX BelowBound;
	if (pBelowObject != NULL)
	{
		BelowBound = pBelowObject->get_bound();
	}

	for (PMGPageObject* pObject = pFirstObject;
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		if (pObject == pBelowObject)
		{
			fFoundObject = TRUE;
			break;
		}

		if (pObject->type() == OBJECT_TYPE_Frame)
		{
			PBOX Bound = pObject->get_bound();
			if (pBelowObject == NULL || IsBoxOnBox(&BelowBound, &Bound))
			{
				MarkFrame((CFrameObject*)pObject);
			}
		}
		else if (pObject->type() == OBJECT_TYPE_Group)
		{
			PMGPageObject* pGroupObject = (PMGPageObject*)((GroupObject*)pObject)->object_list()->first_object();
			if (pGroupObject != NULL)
			{
				if (fFoundObject = MarkFramesBelow(pBelowObject, pGroupObject))
				{
					break;
				}
			}
		}
	}
	return fFoundObject;
}

void CPmwDoc::MarkFramesBelowSelectedObjects(void)
{
	for (PMGPageObject* pObject = selected_objects();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_selected_object())
	{
		MarkFramesBelow(pObject);
	}
}

/*
// Mark a frame for intersection computation.
*/

void CPmwDoc::MarkFrame(CFrameObject* pObject)
{
	if (pObject->GenericFrame())
	{
		pObject->add_refresh_flags(REFRESH_FLAG_intersect);
	}
}

/*
// Update all the marked frames.
*/

void CPmwDoc::UpdateMarkedFrames(BOOL fDoIt, PMGPageObject* pObject /*=NULL*/)
{
	if (pObject == NULL)
	{
		pObject = objects();
	}

	for (; pObject != NULL; pObject = (PMGPageObject*)pObject->next_object())
	{
		if (pObject->type() == OBJECT_TYPE_Group)
		{
			PMGPageObject* pGroupObject = (PMGPageObject*)((GroupObject*)pObject)->object_list()->first_object();
			if (pGroupObject != NULL)
			{
				UpdateMarkedFrames(fDoIt, pGroupObject);
			}
		}
		else if (pObject->get_refresh_flags() & REFRESH_FLAG_intersect)
		{
			pObject->remove_refresh_flags(REFRESH_FLAG_intersect);
			if (fDoIt)
			{
				((CFrameObject*)pObject)->ComputeIntersections();
			}
		}
	}
}

BOOL CPmwDoc::LinkFrame(DB_RECORD_NUMBER lSourceFrame, DB_RECORD_NUMBER lLinkFrame, DB_RECORD_NUMBER* pPreviousFrame /*=NULL*/)
{
	ERRORCODE error;
	PMGDatabase *pDatabase = get_database();

	BOOL fResult = FALSE;

	// Get the link frame (the one we are linking to).
	CFrameRecord* pLinkFrame = (CFrameRecord*)pDatabase->get_record(lLinkFrame, &error, RECORD_TYPE_Frame);
	if (pLinkFrame != NULL)
	{
		// Get the source frame (the one we are linking from).
		CFrameRecord* pSourceFrame = (CFrameRecord*)pDatabase->get_record(lSourceFrame, &error, RECORD_TYPE_Frame);
		if (pSourceFrame != NULL)
		{
			DB_RECORD_NUMBER lLinkText = pLinkFrame->TextRecord();
			DB_RECORD_NUMBER lSourceText = pSourceFrame->TextRecord();
			BOOL fDeleteLinkText = FALSE;

			// Prepare to unlink the link frame.
			// If it is the first frame, the text record will need to go
			// away.

			if (pLinkFrame->PreviousFrame() == 0)
			{
				// We are the first frame. Get rid of the text record.
				fDeleteLinkText = (lLinkText != 0);
			}
			else
			{
				if (pPreviousFrame != NULL)
				{
					*pPreviousFrame = pLinkFrame->PreviousFrame();
				}

				// Lock the previous frame.
				CFrameRecord* pPreviousFrame = (CFrameRecord*)pDatabase->get_record(pLinkFrame->PreviousFrame(), &error, RECORD_TYPE_Frame);
				if (pPreviousFrame != NULL)
				{
					pPreviousFrame->NextFrame(0);
					pLinkFrame->PreviousFrame(0);
				}
				else
				{
					// Could not access the previous frame!
					// (Check 'error' for reason why).
					ASSERT(FALSE);
				}
			}

			// Remember the first character in this frame for later flow.
			CHARACTER_INDEX lSourceCharacter = pSourceFrame->FirstCharacter();

			// Attach the frame to the source text record.
			pLinkFrame->AttachTextRecord(lSourceText);

			ASSERT(pLinkFrame->PreviousFrame() == 0);
			pLinkFrame->PreviousFrame(lSourceFrame);

			ASSERT(pSourceFrame->NextFrame() == 0);
			pSourceFrame->NextFrame(lLinkFrame);

			pSourceFrame->release(TRUE);

			DB_RECORD_NUMBER lNext;
			while ((lNext = pLinkFrame->NextFrame()) != 0)
			{
				pLinkFrame->release(TRUE);
				pLinkFrame = (CFrameRecord*)pDatabase->get_record(lNext, &error, RECORD_TYPE_Frame);
				if (pLinkFrame == NULL)
				{
					break;
				}
				pLinkFrame->AttachTextRecord(lSourceText);
			}

			if (fDeleteLinkText)
			{
				pDatabase->DeleteStory(lLinkText);
			}

			// Flow the text into the new frame.
			CTextFlow TextFlow(lSourceFrame, pDatabase, lSourceCharacter, -1);
			TextFlow.FlowText();

			// We were successful.
			fResult = TRUE;
		}
		else
		{
			// We shouldn't have gotten here if we can't load the frame!
			ASSERT(FALSE);
		}

		if (pLinkFrame != NULL)
		{
			pLinkFrame->release(TRUE);
		}
	}

	return fResult;
}

/*
// Unlink the frame after the one passed.
*/

BOOL CPmwDoc::UnlinkFrame(DB_RECORD_NUMBER lFrame, BOOL fCreateText /*=TRUE*/)
{
	BOOL fResult = FALSE;
	PMGDatabase* pDatabase = get_database();
	CFrameRecord* pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
	if (pFrame != NULL)
	{
		fResult = TRUE;
		if (pFrame->NextFrame() != 0)
		{
			// Detach the frames after this frame. They will become a new
			// chain with a new text record.
			fResult = DetachFrame(pFrame->NextFrame(), fCreateText);
			ASSERT(fResult);
			pFrame->NextFrame(0);
		}
		pFrame->release();
	}
	return fResult;
}

/*
// Detach this frame from its parent.
// This routine assumes that somebody has already changed the parent's
// "next frame" pointer.
*/

BOOL CPmwDoc::DetachFrame(DB_RECORD_NUMBER lFrame, BOOL fCreateText /*=TRUE*/)
{
	BOOL fResult = FALSE;
	PMGDatabase* pDatabase = get_database();
	CFrameRecord* pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
	if (pFrame != NULL)
	{
		fResult = TRUE;
		// Unhook us from the doubly-linked chain.
		pFrame->PreviousFrame(0);

		pFrame->AttachTextRecord(0);

		if (fCreateText)
		{
			// Create a new story for this frame chain.
			pFrame->CreateTextRecord();
		}
		DB_RECORD_NUMBER lNewText = pFrame->TextRecord();
		ASSERT(!fCreateText || lNewText != 0);

		// Set the text record in all succeeding frames.
		DB_RECORD_NUMBER lNext;
		while ((lNext = pFrame->NextFrame()) != 0)
		{
			pFrame->release(TRUE);
			pFrame = (CFrameRecord*)pDatabase->get_record(lNext, NULL, RECORD_TYPE_Frame);
			if (pFrame == NULL)
			{
				break;
			}
			pFrame->AttachTextRecord(lNewText);
		}

		// If we still have a frame (and we should), release it.
		if (pFrame != NULL)
		{
			pFrame->release();
		}

		// Flow the new text.
		if (lNewText != 0)
		{
			CTextFlow TextFlow(lFrame, pDatabase, 0, -1);
			TextFlow.FlowText();
		}
	}
	else
	{
		// We could not access the next frame. What should we do in
		// this case? One solution is to hook the next frame back up
		// to this one and abort the re-link. Another solution is to
		// leave the next frame in limbo and continue with the new link.
		// I prefer the second solution (at this time) because it allows
		// the user to do what they want, and it allows them to unhook
		// from bad frames if the frame list ever gets corrupted.
		ASSERT(FALSE);
	}
	return fResult;
}

BOOL CPmwDoc::IsGroupedFrame(PMGPageObject* pObject)
{
	return pObject->type() == OBJECT_TYPE_Frame
			&& (pObject->get_flags() & OBJECT_FLAG_grouped)
			&& pObject->get_panel() < SKELETON_PANEL;
}

void CPmwDoc::SelectNextObject(void)
{
	if (CanSelectNextObject())
	{

		PMGPageObject* pObject = selected_objects();
		if (pObject == NULL)
		{
			// Nothing selected. Select first object.
			pObject = (PMGPageObject*)object_list()->first_object();
		}
#if 0
		else if (pObject->type() == OBJECT_TYPE_Table)
		{
			GroupObject * pGroup = ((CTableObject*)pObject)->FindGroup(CELL_PANEL);
			if (pGroup != NULL)
				pObject = (PMGPageObject*)pGroup->object_list()->first_object();
			else
				pObject = NULL;
		}
		else if (pObject->type() == OBJECT_TYPE_Cell)
		{
			pObject = (PMGPageObject*)pObject->next_object();
			// if cell is merged, we want to step around the "merged" cells, so that tab key works properly	
			if (pObject != NULL && ((CellObject*)pObject)->GetMergeState() > CELL_NOT_MERGED)
			{
				BOOL breakstupidloop = FALSE;
				while (pObject != NULL && !breakstupidloop)
				{
					// find next cell that is not a merge child
					if (((CellObject*)pObject)->GetMergeState() == CELL_MERGED_CHILD)
					{
						pObject = (PMGPageObject*)pObject->next_object();
					}
					else
						breakstupidloop = TRUE;
				};
			}	
		}
#endif
		else if (pObject->next_selected_object() == NULL
					&& !(pObject->get_flags() & OBJECT_FLAG_grouped))
		{
			// One object selected. Select the next object.
			pObject = (PMGPageObject*)pObject->next_object();
		}
		else
		{
			// More than one object selected. Reset.
			pObject = NULL;
		}

		// Select the new object.
		deselect_all();
		if (pObject != NULL)
		{
			select_object(pObject);
		}
	}
}

BOOL CPmwDoc::CanSelectNextObject(void)
{
	return object_list()->count() > 0;
}

void CPmwDoc::SelectPreviousObject(void)
{
	if (CanSelectPreviousObject())
	{
		PMGPageObject* pObject = selected_objects();
		if (pObject == NULL)
		{
			// Nothing selected. Select last object.
			pObject = (PMGPageObject*)object_list()->last_object();
		}
#if 0
		else if (pObject->type() == OBJECT_TYPE_Cell)
		{
			pObject = (PMGPageObject*)pObject->previous_object();

			// if cell is merged, we want to step around the "merged" cells, so that tab key works properly
			if (pObject != NULL && ((CellObject*)pObject)->GetMergeState() > CELL_NOT_MERGED)
			{
				BOOL breakstupidloop = FALSE;
				// find the previous cell which is not a merge child
				while (pObject != NULL && !breakstupidloop)
				{
					if (((CellObject*)pObject)->GetMergeState() == CELL_MERGED_CHILD)
					{
						pObject = (PMGPageObject*)pObject->previous_object();
					}
					else
						breakstupidloop = TRUE;
				};
			}	
		}
#endif
		else if (pObject->next_selected_object() == NULL
					&& !(pObject->get_flags() & OBJECT_FLAG_grouped))
		{
			// One object selected. Select previous object.
			pObject = (PMGPageObject*)pObject->previous_object();
		}
		else
		{
			// More than one object selected. Reset.
			pObject = NULL;
		}

		// Select the new object.
		deselect_all();
		if (pObject != NULL)
		{
			select_object(pObject);
		}
	}
}

BOOL CPmwDoc::CanSelectPreviousObject(void)
{
	return object_list()->count() > 0;
}


//
// special function for selecting cells in a table
// will select all cells inside and touching box
// set bDeselect TRUE for rubberbanding box
//
VOID CPmwDoc::select_all_cells(PBOX_PTR pbox, BOOL bDeselect)
{
	//PMGPageObjectPtr object;


/* A solo object should go away. */
//
//	if (selected_list != NULL
 //				&& ((selected_list->get_select_flags() & SELECT_FLAG_solo_select)
//						|| IsGroupedFrame(selected_list)))
					 
//	{
	if (bDeselect)
		deselect_all();
//	}

/* Select all the objects we can. */

#if 0
	for (object = objects(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		PBOX bound = object->get_bound();

		//
		// handle table objects only
		//
		if (object->type() == OBJECT_TYPE_Table)
		{
			PBOX tbound = object->get_bound();
			// check to see whether entire table is to be selected
			if (!IsBoxInBox(&tbound, pbox))
			{
				PMGPageObjectPtr tblobject = ((CTableObject*)object)->FindGroup(CELL_PANEL);  
				// check to see if any cells are in the selection box
				tblobject = (PMGPageObjectPtr)((GroupObject*)tblobject)->object_list()->first_object();
				while (tblobject != NULL)
				{
					PBOX cellbound = tblobject->get_bound();
					if ((pbox == NULL || IsBoxInBox(&cellbound, pbox) || IsBoxOnBox(&cellbound, pbox))
				//	&& !(object->get_select_flags() & SELECT_FLAG_solo_select)
					&& !(tblobject->get_flags() & OBJECT_FLAG_locked))
						{
							select_object(tblobject);
						}
					tblobject = (PMGPageObjectPtr)tblobject->next_object();
				}
			}
		}
	}
#endif		
}
