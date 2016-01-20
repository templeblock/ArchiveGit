/*
// $Header: /PM8/App/GRPOBJ.CPP 1     3/03/99 6:05p Gbeddow $
//
// Group object routines.
//
// $Log: /PM8/App/GRPOBJ.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 4     2/04/98 5:05p Jayn
// Added OnPage()
// 
// 3     12/04/97 3:12p Hforman
// remove unused local variable
// 
// 2     11/13/97 4:09p Jayn
// Finished rotation of groups (fixed get_refresh_bound, reading of old
// groups)
// 
//    Rev 1.0   14 Aug 1997 15:21:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:40   Fred
// Initial revision.
// 
//    Rev 1.11   23 Jul 1997 12:13:30   Jay
// Now recalcs group in SetFlowAround().
// 
//    Rev 1.10   18 Jul 1997 14:50:12   Jay
// Now saves states for contained objects.
// 
//    Rev 1.9   09 Jun 1997 17:26:36   Jay
// ASSERT was failing. Fixed the case.
// 
//    Rev 1.8   05 Jun 1997 16:45:36   Jay
// Changed upto_object in DrawObjects
// 
//    Rev 1.7   25 Feb 1997 12:57:16   Jay
// Cropping and extensions
// 
//    Rev 1.6   23 Jan 1997 11:05:32   Jay
// New popup palettes
// 
//    Rev 1.5   04 Sep 1996 08:22:54   Jay
// Reversed order to fix intersection problem.
// 
//    Rev 1.4   30 Aug 1996 19:48:12   Jay
// Fixed page setting.
// 
//    Rev 1.3   26 Jul 1996 11:38:56   Jay
// Page stuff
// 
//    Rev 1.2   18 Jul 1996 11:10:10   Jay
// Better flow around objects.
// 
//    Rev 1.1   12 Jul 1996 16:47:52   Jay
// Text wrap
// 
//    Rev 1.0   14 Mar 1996 13:45:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:16   FRED
// Initial revision.
// 
//    Rev 2.4   01 Sep 1995 18:37:38   JAY
// Some fixes... some TRACEs... some left over problems.
// 
//    Rev 2.3   18 Aug 1995 15:06:14   JAY
// Now calls destroy_objects() in object list.
// 
//    Rev 2.2   19 Apr 1995 16:05:24   JAY
// Removed some symbols.
// 
//    Rev 2.1   15 Feb 1995 17:03:12   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:50   JAY
// Initial revision.
// 
//    Rev 1.11   30 Nov 1994 16:35:00   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.10   04 Nov 1994 12:23:08   JAY
// Added destroy_objects().
// 
//    Rev 1.9   01 Jun 1994 16:35:30   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.8   14 Apr 1994 10:02:50   JAY
// Put in "special" code for "special" groups to prevent normalize in the
// group's get_refresh_bound() call. This all really needs to be firmed up
// some day to eliminate the need for these kind of special cases.
// 
//    Rev 1.7   28 Feb 1994 16:52:16   JAY
// Fixed a problem with trying to update a date while it's being dragged.
// 
//    Rev 1.6   17 Feb 1994 09:25:16   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.5   14 Feb 1994 11:28:02   JAY
// Put some extra code in to check for weird cases.
// 
//    Rev 1.4   28 Jan 1994 17:11:02   JAY
//  
// 
//    Rev 1.3   25 Jan 1994 15:18:06   JAY
// More
// 
//    Rev 1.2   19 Jan 1994 17:06:56   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:06:16   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:20   JAY
// 
*/

#include "stdafx.h"
#include "pmgobj.h"
#include "grpobj.h"
#include "utils.h"
#include "action.h"

/*****************************************************************************/
/*                              GroupObject                                  */
/*****************************************************************************/

/*
// The creator for a pattern object.
*/

ERRORCODE GroupObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{     
	GroupObjectPtr group;

/* Create the group object. */

	if ((group = new GroupObject(type, owner)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	*record = group;

/* Handle any creation data. */

	if (creation_data != NULL)
	{
		GROUP_CREATE_STRUCT_PTR gcs = (GROUP_CREATE_STRUCT_PTR)creation_data;
		group->rrecord.unrotated_bound
			= group->grecord.original_bound = gcs->original_bound;
	}
	return ERRORCODE_None;
}

/*
// The constructor for the group object.
*/

GroupObject::GroupObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		: RectPageObject(type, owner), list(owner)
{
	record.select_flags = SELECT_FLAG_boundary
 									| SELECT_FLAG_size_handles
									| SELECT_FLAG_move_handle
									| SELECT_FLAG_rotate_handle;
}

/*
// Update a group object.
*/

UpdateStatePtr GroupObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	PBOX local_extent;
	RECT local_clip;
	GroupUpdateStatePtr gustate = (GroupUpdateStatePtr)ustate;
	PMGPageObjectPtr object;

	if (rc->want_update_names())
	{
		rc->update_name(this, NULL);
	}

/*
// Handle the update state.
*/

	if (gustate == NULL)
	{
		object = (PMGPageObjectPtr)list.first_object();
		ustate = NULL;
	}
	else
	{
		object = gustate->object;
		ustate = gustate->object_state;
		gustate->object_state = NULL;				/* We've taken it out. */
	}

/*
// A little kludge for card groups.
*/

	if (record.primary_action == ACTION_EDIT_PANEL)
	{
	/* A card panel group. Clip to it. */

		local_extent = get_bound();

		if (extent != NULL)
		{
			IntersectBox(&local_extent, &local_extent, extent);
		}

	/* Transform to get a clipping rect. */
		PBOX pbox = local_extent;

		rc->pbox_to_screen(&pbox, TRUE);

		if (!rc->convert_pbox(&pbox, &local_clip, clip))
		{
			if (ustate != NULL)
			{
				delete ustate;
			}
			return NULL;
		}

		extent = &local_extent;
		clip = &local_clip;
	}

	PBOX test_extent;

	test_extent.x0 = extent->x0 - rc->redisplay_x_pixel;
	test_extent.y0 = extent->y0 - rc->redisplay_y_pixel;
	test_extent.x1 = extent->x1 + rc->redisplay_x_pixel;
	test_extent.y1 = extent->y1 + rc->redisplay_y_pixel;

/*
// Run through and update all objects in the group.
*/

/* Update objects in the group. */

	for (; object != NULL;
					ustate = NULL, object = (PMGPageObjectPtr)object->next_object())
	{
		if (object == rc->m_pUptoObject)
		{
			return error_update_state(ustate, UPDATE_TYPE_HitUptoObject, ERRORCODE_None);
		}

		if ((object->get_flags() & OBJECT_FLAG_hidden))
		{
			continue;
		}

	/* See if we need to abort here. */

		if (rc->check_interrupt())
		{
			if (gustate == NULL)
			{
				gustate = new GroupUpdateState;
			}

			gustate->object = object;
			gustate->object_state = ustate;
			gustate->type = UPDATE_TYPE_Interrupted;
			gustate->allocates_memory = FALSE;

			ustate = gustate;
			break;
		}

	/*
 	// Check the next object.
	// The lines below (with redisplay_?_pixel) attempts to handle those
	// case where objects are smaller than one displayed pixel (like lines on
	// the calendar), but get expanded to one pixel when drawn. They draw
	// outside their bounds because they're so small. We need to catch those
	// objects and draw them because otherwise they don't get drawn when they
	// need to.
 	*/

		if (object->get_flags() & OBJECT_FLAG_needs_calc)
		{
		/* Object is being moved or otherwise busy. */
			object->add_flags(OBJECT_FLAG_needs_update);
			if (ustate != NULL)
			{
			/* We must free this update state. */
				delete ustate;
				ustate = NULL;
			}
		}
		else
		{
			object->remove_flags(OBJECT_FLAG_needs_update);

			PBOX bound = object->get_bound();

			if (IsBoxOnBox(&bound, &test_extent))
			{
				if ((ustate = object->update(rc, extent, clip, ustate, refresh_type)) != NULL)
				{
					if (ustate->type == UPDATE_TYPE_Interrupted)
					{
						if (gustate == NULL)
						{
							gustate = new GroupUpdateState;
						}

						gustate->object = object;
						gustate->object_state = ustate;
						gustate->type = ustate->type;
						gustate->allocates_memory = ustate->allocates_memory;

						ustate = gustate;
						break;
					}

				/* What to do for errors? Just go to next object for now. */

				}
			}
			else
			{
				/* Was in the extent, now isn't? This update state needs to be freed. */
				if (ustate != NULL)
				{
					delete ustate;
				}
			}
		}
	}

/* Delete our update state if we had one but no longer. */

	if (object == NULL)
	{
	/* Finished! */

		if (gustate != NULL)
		{
			delete gustate;
			ustate = NULL;				// Make sure we're NULL.
		}
	}

	return ustate;
}

/*
// adopt_children()
//
// Make sure all our children know we're the parent.
*/

VOID GroupObject::adopt_children()
{
	PMGPageObjectPtr object;

	for (object = (PMGPageObjectPtr)list.first_object();
				object != NULL;
				object = (PMGPageObjectPtr)object->next_object())
	{
		object->set_parent(this);
	}
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE GroupObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = RectPageObject::read_data(device)) == ERRORCODE_None
			&& (error = device->read_record(&grecord, sizeof(grecord))) == ERRORCODE_None)
	{
		if ((error = list.read(device)) == ERRORCODE_None)
		{
			adopt_children();

			// Old groups didn't have the rotate handle.
			// Turn it on now if it's not a special group.
			if (!(record.select_flags & SELECT_FLAG_solo_select)
				 && (record.select_flags & SELECT_FLAG_size_handles))
			{
				record.select_flags |= SELECT_FLAG_rotate_handle;
			}
		}
	}
	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE GroupObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = RectPageObject::write_data(device)) == ERRORCODE_None
			&& (error = device->write_record(&grecord, sizeof(grecord))) == ERRORCODE_None)
	{
		error = list.write(device);
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE GroupObject::size_data(StorageDevicePtr device)
{
	return RectPageObject::size_data(device)
				+ device->size_record(sizeof(grecord))
 				+ list.size(device);
}

/*
// Recalc a group.
*/

VOID near GroupObject::recalc_group()
{
	PMGPageObjectPtr object;

/* Do all the objects in the list. */

	for (object = (PMGPageObjectPtr)list.last_object();
							object != NULL;
							object = (PMGPageObjectPtr)object->previous_object())
	{
		if (!(object->get_flags() & OBJECT_FLAG_hidden))
		{
			object->group_calc(&rrecord.unrotated_bound, &grecord.original_bound, record.flags, &record.rotc);
		}
	}
}

/*
// Destroy the objects in this group.
*/

void GroupObject::destroy_objects(void)
{
	list.destroy_objects();
}

/*
// Destroy a group object.
// This involves destroying each object in the group.
*/

VOID GroupObject::destroy()
{
/* Destroy all objects in this group. */

	destroy_objects();

/* And don't forget ourselves. */

	RectPageObject::destroy();
}

/*
// Do a normal calc (when this group is not grouped).
*/

VOID GroupObject::calc(PBOX_PTR panel_world, FLAGS panel_flags)
{
	RectPageObject::calc(panel_world, panel_flags);
	recalc_group();
}

/*
// Flip an object left to right.
*/

VOID GroupObject::xflip(BOOL fChangeAngle /*=TRUE*/)
{
	PMGPageObjectPtr object;

/* Do all the objects in the list. */

	for (object = (PMGPageObjectPtr)list.last_object();
							object != NULL;
							object = (PMGPageObjectPtr)object->previous_object())
	{
		object->xflip();
	}

/* Flip us! */

	RectPageObject::xflip();

/* Calc the group if we ourselves aren't grouped. */

	if (!(record.flags & OBJECT_FLAG_grouped))
	{
		recalc_group();
	}
}

/*
// Flip an object top to bottom.
*/

VOID GroupObject::yflip(BOOL fChangeAngle /*=TRUE*/)
{
	PMGPageObjectPtr object;

/* Do all the objects in the list. */

	for (object = (PMGPageObjectPtr)list.last_object();
							object != NULL;
							object = (PMGPageObjectPtr)object->previous_object())
	{
		object->yflip();
	}

/* Flip us! */

	RectPageObject::yflip();

/* Calc the group if we ourselves aren't grouped. */

	if (!(record.flags & OBJECT_FLAG_grouped))
	{
		recalc_group();
	}
}

/*
// Notify an object that its image has changed.
*/

VOID GroupObject::changed_image(VOID)
{
	PMGPageObjectPtr object;

/* Do all the objects in the list. */

	for (object = (PMGPageObjectPtr)list.last_object();
							object != NULL;
							object = (PMGPageObjectPtr)object->previous_object())
	{
		object->changed_image();
	}
}

/*
// Do a group calc (when this group is grouped).
*/

VOID GroupObject::group_calc(PBOX_PTR current_group_bound,
 									PBOX_PTR org_group_bound,
 									FLAGS group_flags,
 									ROTATION_COMPONENT_PTR group_rotc)
{
	RectPageObject::group_calc(current_group_bound, org_group_bound, group_flags, group_rotc);
	recalc_group();
}

/*
// Construct a group from the object's in its list.
// All objects are assumed to be joining (since they're already in the list).
*/

VOID GroupObject::establish(BOOL compute_bound)
{
	PBOX bound;
	SHORT count = 0;
	PMGPageObjectPtr object;

	if (compute_bound)
	{
	/* Compute the bound for all the objects. */

		for (object = (PMGPageObjectPtr)list.first_object();
	 					object != NULL;
	 					object = (PMGPageObjectPtr)object->next_object())
		{
			if (!(object->get_flags() & OBJECT_FLAG_hidden))
			{
				PBOX obj_bound = object->get_bound();

				if (count == 0)
				{
					bound = obj_bound;
				}
				else
				{
					UnionBox(&bound, &bound, &obj_bound);
				}
				count++;
			}
		}
		rrecord.bound = bound;
	}

/* This is the group bound. */

	rrecord.unrotated_bound = grecord.original_bound = rrecord.bound;

/* Now join all objects into the group. */

	for (object = (PMGPageObjectPtr)list.first_object();
 					object != NULL;
 					object = (PMGPageObjectPtr)object->next_object())
	{
		if (!(object->get_flags() & OBJECT_FLAG_hidden))
		{
			object->join_group(&rrecord.bound, this);
		}
	}
}

/*
// Set the page for this object.
*/

void GroupObject::Page(DB_RECORD_NUMBER lPage)
{
	INHERITED::Page(lPage);
	for (PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		pObject->Page(lPage);
	}
}

/*****************************************************************************/
/*                            GroupUpdateState                               */
/*****************************************************************************/

/*
// The destructor for the group update state.
*/

GroupUpdateState::~GroupUpdateState()
{
	if (object_state != NULL)
	{
		delete object_state;
	}
}

/*
// Assign an object to this.
*/

ERRORCODE GroupObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;
	GroupObject& gobject = (GroupObject&)sobject;

/* Assign the base object. */

	if ((error = RectPageObject::assign(sobject)) == ERRORCODE_None)
	{
	/* Copy the record. */

		grecord = gobject.grecord;

	/* Copy the page list. */

		error = list.assign(gobject.list);
		adopt_children();
	}
	return error;
}

/*
// Return the original dimensions of an object.
*/

BOOL GroupObject::original_dims(PPNT_PTR p)
{
	p->x = grecord.original_bound.x1 - grecord.original_bound.x0;
	p->y = grecord.original_bound.y1 - grecord.original_bound.y0;
	return TRUE;
}

/*
// Return the refresh bounds for this object.
*/

VOID GroupObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc)
{
	PMGPageObjectPtr object;

/*
// We need to skip special groups that can't be moved under normal circumstances.
*/

	if ((object = (PMGPageObjectPtr)list.first_object()) == NULL
			|| (get_select_flags() & SELECT_FLAG_solo_select))
	{
		RectPageObject::get_refresh_bound(refresh_bound, rc);
	}
	else
	{
		// Accumulate the union of all the refresh bounds for the
		// contained objects.
		object->get_refresh_bound(refresh_bound, rc);

		while ((object = (PMGPageObjectPtr)object->next_object()) != NULL)
		{
			PBOX pbox;
			object->get_refresh_bound(&pbox, rc);
			UnionBox(refresh_bound, refresh_bound, &pbox);
		}
	}
}

PageObjectState* GroupObject::CreateState(void)
{
	return new GroupObjectState;
}

void GroupObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);

	((GroupObjectState*)pState)->m_GRecord = grecord;

   // Save the states for the contained objects.

   CObjectStateArray& States = ((GroupObjectState*)pState)->m_States;

	PMGPageObject* pObject;
	for (pObject = (PMGPageObjectPtr)list.first_object();
				pObject != NULL;
				pObject = (PMGPageObjectPtr)pObject->next_object())
	{
      // Create a state for this object.
		PageObjectState* pSubState = pObject->CreateState();
		if (pSubState != NULL)
		{
			pObject->SaveState(pSubState);
		}
      // Will add NULL if that was returned.
		States.Add(pSubState);
	}
}

void GroupObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);

	// Restore our state.
	grecord = ((GroupObjectState*)pState)->m_GRecord;

   CObjectStateArray& States = ((GroupObjectState*)pState)->m_States;

   // Restore the states for all the contained objects.
	int nState = 0;
	PMGPageObject* pObject;
	for (pObject = (PMGPageObjectPtr)list.first_object();
				pObject != NULL;
				pObject = (PMGPageObjectPtr)pObject->next_object())
	{
		if (nState >= States.GetSize())
		{
			ASSERT(FALSE);
			break;
		}

      // Get the state that was saved.
		PageObjectState* pSubState = States.GetAt(nState++);

      // If it is not NULL, restore it.
		if (pSubState != NULL)
		{
			pObject->RestoreState(pSubState);
		}
	}

	recalc_group();
}

/*
// Set whether the object is on or off the page at the moment.
*/

void GroupObject::OnPage(BOOL fOnPage)
{
	// Tell ourselves.
	INHERITED::OnPage(fOnPage);

	// Tell all of our sub-objects, too.
	for (PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		pObject->OnPage(fOnPage);
	}
}

/*
// Get the properties for this object.
*/

void GroupObject::GetObjectProperties(CObjectProperties& Properties)
{
	for (PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		pObject->GetObjectProperties(Properties);
	}
}

/*
// Set the properties for this object.
*/

BOOL GroupObject::SetObjectProperties(CObjectProperties& Properties)
{
	BOOL fSet = FALSE;

	for (PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		fSet |= pObject->SetObjectProperties(Properties);
	}

	return fSet;
}

/*
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

void GroupObject::AddOutline(COutlinePath& Path, PBOX* pClipBox /*=NULL*/)
{
	PMGPageObject* pObject;
	for (pObject = (PMGPageObject*)object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		PBOX Bound = pObject->get_bound();
		if (pObject->GetFlowAround() != FLOW_AROUND_none
				&& (pClipBox == NULL || IsBoxOnBox(&Bound, pClipBox)))
		{
			pObject->AddOutline(Path, pClipBox);
		}
	}
}

/*
// How does text flow around us?
*/

long GroupObject::GetFlowAround(void)
{
	long lFlow = -2;
	for (PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		long lThisFlow = pObject->GetFlowAround();
		if (lFlow == -2)
		{
			lFlow = lThisFlow;
		}
		else if (lFlow != lThisFlow)
		{
			// Undefined.
			lFlow = -1;
		}
	}
	if (lFlow == -2)
	{
		// No objects.
		lFlow = -1;
	}
	ASSERT(lFlow != -2);
	return lFlow;
}

void GroupObject::SetFlowAround(long lFlowAround)
{
	if (lFlowAround >= 0)
	{
		for (PMGPageObject* pObject = (PMGPageObject*)object_list()->first_object();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
			pObject->SetFlowAround(lFlowAround);
		}
	}
	else
	{
		// lFlowAround < 0!
		ASSERT(FALSE);
	}
	// Recalc the individual group objects.
	recalc_group();
}
