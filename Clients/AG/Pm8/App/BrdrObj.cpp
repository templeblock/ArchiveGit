//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/BrdrObj.cpp 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/BrdrObj.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 18    7/25/98 1:45p Jayn
// Added get_refresh_bound.
// 
// 17    7/21/98 8:34a Jayn
// Saves and restores the position around SetData.
// 
// 16    6/02/98 1:20p Rlovejoy
// Removed helpful hint message post from ShowBorderEditor().
// 
// 15    6/01/98 9:12p Psasse
// an attempt to get helpful hints on top of border editor
// 
// 14    3/03/98 9:34p Hforman
// 
// 13    2/24/98 1:23p Hforman
// default size is now full page
// 
// 12    2/20/98 8:03p Hforman
// 
// 11    2/19/98 6:44p Hforman
// background color, etc.
// 
// 10    2/17/98 7:26p Hforman
// interrupting, background color, etc.
// 
// 9     2/12/98 6:02p Hforman
// add param to ShowBorderEditor()
// 
// 8     2/12/98 1:14p Hforman
// added Undo capabilities, other cleanup and fixes
// 
// 7     2/03/98 12:48p Hforman
// turn off interupability (for now)
// 
// 6     1/13/98 6:13p Hforman
// interim checkin
// 
// 5     1/12/98 5:06p Hforman
// 
// 4     1/09/98 6:52p Hforman
// 
// 3     1/09/98 11:05a Hforman
// interim checkin
// 
// 2     1/06/98 7:05p Hforman
// work-in-progress
// 
// 1     12/24/97 1:08p Hforman
// 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmgobj.h"
#include "grafobj.h"
#include "utils.h"
#include "commands.h"
#include "BrdrObj.h"

/*****************************************************************************/
/*                             BorderObject                                  */
/*****************************************************************************/


//
// The creator for a Border object.
//
ERRORCODE BorderObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	// make sure the DLL has been initialized!
	if (!GET_PMWAPP()->BorderEditorInitialized())
		return ERRORCODE_NotInitialized;

	// Create the Border object
	BorderObject* pObject;
	if ((pObject = new BorderObject(type, owner)) == NULL)
		return ERRORCODE_Memory;

	// make sure the DLL created its object
	if (pObject->m_pBEObject == NULL)
		return ERRORCODE_IntError;

	*record = (PageObjectPtr)pObject;

	return ERRORCODE_None;
}

// The constructor for the Border object.
BorderObject::BorderObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
	: RectPageObject(type, owner)
{
	record.select_flags = SELECT_FLAG_boundary		|
								 SELECT_FLAG_size_handles	|
								 SELECT_FLAG_move_handle	|
								 SELECT_FLAG_rotate_handle;

	m_pBEObject = NULL;

	// call the DLL to init our Border Editor Object
	CBEInterface*	pBEInterface;
	if ((pBEInterface = GET_PMWAPP()->GetBorderEditorInterface()) == NULL)
	{
		ASSERT(0);
		return;
	}

	if (pBEInterface->NewObject(m_pBEObject, get_database()) != 0)
	{
		ASSERT(0);
	}
}

BorderObject::~BorderObject()
{
	// object may have already been deleted (and released) in destroy()
	if (m_pBEObject)
		m_pBEObject->Release();
}

int BorderObject::ShowBorderEditor(BOOL fCreating /*=FALSE*/)
{
	CPmwDoc* pDoc = (CPmwDoc*)get_database()->GetDocument();

	// Set up an undo command if not first creating border
	if (!fCreating)
	{
		CCmdBorderChange* pBorderUndo = new CCmdBorderChange(IDCmd_BorderModify);
		ASSERT(pBorderUndo);

		CPmwView* pView = pDoc->GetCurrentView();

		// Create a copy of current BEObject to use for switching during undo/redo.
		// We'll send the current one to the Undo, and use the duplicate --
		// that way everything stays in sync.
		// NOTE: the extra BEObject will get deleted by the Undo command.
		CBEObject* pSaveBEObj;
		m_pBEObject->Duplicate(pSaveBEObj);
		pBorderUndo->Setup(pView, this, m_pBEObject);
		m_pBEObject = pSaveBEObj;

		pDoc->AddCommand(pBorderUndo, FALSE);
	}

	int retval = m_pBEObject->Edit(AfxGetMainWnd()->GetSafeHwnd());

	pDoc->SetModifiedFlag(); // assume they've modified the border

	return retval;
}

UpdateStatePtr BorderObject::update(RedisplayContextPtr rc, PBOX_PTR extent,
												LPRECT clip, UpdateStatePtr ustate,
												REFRESH_TYPE refresh_type)
{
	// fill out a CBEDrawInfo struct and send it to Border DLL's Draw() method
	CBEDrawInfo drawInfo;
	drawInfo.m_hdc = NULL;

	PBOX bound = get_unrotated_bound();
	CRect rect(bound.x0, bound.y0, bound.x1, bound.y1);
	drawInfo.m_Bounds = rect;

	drawInfo.m_pClip = clip;
	drawInfo.m_dAngle = get_rotation();

	FLAGS flags = get_flags();
	drawInfo.m_fXFlipped = (flags & OBJECT_FLAG_xflipped);
	drawInfo.m_fYFlipped = (flags & OBJECT_FLAG_yflipped);

	drawInfo.m_hWnd = NULL;
	drawInfo.m_pClientData = rc;

	drawInfo.m_fAllowInterrupt = TRUE;
	BorderUpdateState* pBUpdate = (BorderUpdateState*)ustate;
	if (pBUpdate != NULL)
	{
		drawInfo.m_pDrawState = pBUpdate->m_pBEDrawState;
		pBUpdate->m_pBEDrawState = NULL;
		delete pBUpdate;
		pBUpdate = NULL;
	}
	else
	{
		drawInfo.m_pDrawState = NULL;
	}

	m_pBEObject->Draw(&drawInfo);

	
	if (drawInfo.m_pDrawState)
	{
		// we've been interrupted -- save state
		pBUpdate = new BorderUpdateState;
		pBUpdate->type = UPDATE_TYPE_Interrupted;
		pBUpdate->m_pBEDrawState = drawInfo.m_pDrawState;
	}

	return pBUpdate;
}

// Recalc after a size or movement.
void BorderObject::calc(PBOX_PTR panel_world /*=NULL*/, FLAGS panel_flags /*=0*/)
{
	RectPageObject::calc(panel_world, panel_flags);

	SetObjectSize();
}

// Destroy this object.
// The object is being removed from the database.
void BorderObject::destroy()
{
	RectPageObject::destroy();

	if (m_pBEObject)
	{
		m_pBEObject->Destroy();
		m_pBEObject = NULL;
	}
}

// read_data()
// (This is a method invoked by the read method to handle object-specific data.)
//
// For a Border Object: after we read the border-specific data, we call SetData()
// in the DLL so the border editor has the data it needs to create the object.
ERRORCODE BorderObject::read_data(StorageDevicePtr device)
{
	ERRORCODE error;
	int dllerr = 0;

	// make sure the DLL has been initialized!
	if (!GET_PMWAPP()->BorderEditorInitialized())
		return ERRORCODE_NotInitialized;

	if ((error = INHERITED::read_data(device)) == ERRORCODE_None)
	{
		if ((error = device->read_record(&m_brecord, sizeof(m_brecord))) == ERRORCODE_None)
		{
			BYTE* data = new BYTE[m_brecord.nDataLength];
			if ((error = device->read(data, m_brecord.nDataLength)) == ERRORCODE_None)
			{
				// Calling SetData can cause the file pointer to change.
				// We save it here.
				ST_DEV_POSITION CurPos;
				device->tell(&CurPos);
				dllerr = m_pBEObject->SetData(m_brecord.nDataLength, data);
				device->seek(CurPos, ST_DEV_SEEK_SET);
			}
			delete[] data;
		}
		
		record.select_flags |= SELECT_FLAG_rotate_handle;
	}

	if (dllerr)
		return ERRORCODE_IntError;
	else
		return error;
}

// write_data()
// This is a method invoked by the write method to handle object-specific data.
ERRORCODE BorderObject::write_data(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(device)) == ERRORCODE_None)
	{
		// get length of data
		int nLength;
		if (m_pBEObject->GetData(nLength) != 0)
			return ERRORCODE_IntError;

		m_brecord.nDataLength = nLength;
		if ((error = device->write_record(&m_brecord, sizeof(m_brecord))) == ERRORCODE_None)
		{
			BYTE* data = new BYTE[nLength];
			m_pBEObject->GetData(nLength, data);
			error = device->write(data, nLength);
			delete[] data;
		}
	}

	return error;
}

// size_data()
// This is a method invoked by the size method to handle object-specific data.
ST_MAN_SIZE BorderObject::size_data(StorageDevicePtr device)
{
	int nLength = 0;
	m_pBEObject->GetData(nLength);
	return INHERITED::size_data(device) + device->size_record(sizeof(m_brecord)) + nLength;
}

// Return the "original" dimensions of this object.
BOOL BorderObject::original_dims(PPNT_PTR p)
{
	// set default size to full panel size
	CPmwDoc* pDoc = (CPmwDoc*)get_database()->GetDocument();
	PBOX world;
	pDoc->get_panel_world(&world, get_panel());
	p->x = world.x1 - world.x0;
	p->y = world.y1 - world.y0;

	return TRUE;
}

// Assign an object to this.
ERRORCODE BorderObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;

	if ((error = INHERITED::assign(sobject)) == ERRORCODE_None)
	{
		BorderObject& object = (BorderObject&)sobject;
		m_brecord = object.m_brecord;
		if (object.m_pBEObject)
		{
			if (object.m_pBEObject->Duplicate(m_pBEObject, get_database()) != 0)
				error = ERRORCODE_IntError;
		}
	}

	return error;
}

BOOL BorderObject::PtIsOpaque(PPNT pt, RedisplayContextPtr rc)
{
	// return FALSE if we're over the center (transparent) part of the border
	RECT borderInnerRect;
	m_pBEObject->GetInnerRect(&borderInnerRect);
	PBOX innerBound = { borderInnerRect.left, borderInnerRect.top,
							  borderInnerRect.right, borderInnerRect.bottom };

	if (get_rotation() != 0.0)
	{
		// We have a rotated bound. Rotate the point negatively,
		// then compare to our unrotated bound.
		rotate_points(&pt, 1, get_origin(), -get_rotation());
   }

	return (!IsPtInBox(pt.x, pt.y, &innerBound));
}

void BorderObject::GetObjectProperties(CObjectProperties& Properties)
{
	// anything to do here?
}
BOOL BorderObject::SetObjectProperties(CObjectProperties& Properties)
{
	// or here?
	return TRUE;
}

PageObjectState* BorderObject::CreateState()
{
	return INHERITED::CreateState();
}
void BorderObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);
}

void BorderObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);
	SetObjectSize();
}

void BorderObject::RestoreBorder(CBEObject* pBEObject)
{
	m_pBEObject = pBEObject;
	CPmwDoc* pDoc = (CPmwDoc*)get_database()->GetDocument();

	pDoc->refresh_object(this);
}

//
// helper functions
//
void BorderObject::SetObjectSize()
{
	// recalc dimensions
	PBOX bound = get_unrotated_bound();
	// convert to inches
	double dXSize, dYSize;
	dXSize = double(bound.x1 - bound.x0) / PAGE_RESOLUTION;
	dYSize = double(bound.y1 - bound.y0) / PAGE_RESOLUTION;

	// call Border Editor with new size (if not 0)
	if (m_pBEObject && dXSize > 0.0 && dYSize > 0.0)
		m_pBEObject->SetSize(dXSize, dYSize);
}

void BorderObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc /*=NULL*/)
{
	INHERITED::get_refresh_bound(refresh_bound);
	if (get_rotation() != 0.0)
	{
		refresh_bound->x0 -= PAGE_RESOLUTION/10;
		refresh_bound->y0 -= PAGE_RESOLUTION/10;
		refresh_bound->x1 += PAGE_RESOLUTION/10;
		refresh_bound->y1 += PAGE_RESOLUTION/10;
	}
}

//
// BorderUpdateState class
//
BorderUpdateState::BorderUpdateState()
{
	m_pBEDrawState = NULL;
}

BorderUpdateState::~BorderUpdateState()
{
	CBEInterface*	pBEInterface = GET_PMWAPP()->GetBorderEditorInterface();
	if (m_pBEDrawState)
		pBEInterface->FreeDrawState(m_pBEDrawState);
}
