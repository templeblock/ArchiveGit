/*
// $Header: /PM8/App/COMMANDS.CPP 1     3/03/99 6:04p Gbeddow $
//
// General command handler.
//
// $Log: /PM8/App/COMMANDS.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 11    9/03/98 5:06p Jayn
// Flip commands now turn off the Txp highlight before executing.
// This fixes a bug with flipping tables.
// 
// 10    7/25/98 1:47p Jayn
// Only tries to Close the OLE item if it's Open
// 
// 9     7/14/98 6:34p Jayn
// 
// 8     7/02/98 3:48p Johno
// Added ProcessHyperlinks() - use so ReDo works.
// 
// 7     7/01/98 4:10p Johno
// Added mHyperlinksToMe to CCmdDeletePage (saves hyperlink record numbers
// that were pointing to a deleted page)..
// Used in CCmdDeletePage::UndoIt(), CCmdDeletePage::Snapshot()
// 
// 6     3/10/98 5:24p Jayn
// Master page changes
// 
// 5     2/12/98 1:08p Hforman
// add CCmdBorderChange
// 
// 4     11/21/97 4:01p Hforman
// add rc param to DrawGuideline()
// 
// 3     11/14/97 6:54p Hforman
// redid CCmdGuideChange
// 
// 2     11/14/97 11:56a Hforman
// add undo command for Guides
// 
//    Rev 1.0   14 Aug 1997 15:19:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:44   Fred
// Initial revision.
// 
//    Rev 1.18   30 Jul 1997 12:48:14   Jay
// Sets the panel for objects when ungrouping them.
// 
//    Rev 1.17   30 Jul 1997 09:35:40   Jay
// Calls free_edit_background() after a relayer
// 
//    Rev 1.16   20 Jul 1997 11:20:10   Fred
// Fixed send to back bug when no overlapping objects
// 
//    Rev 1.15   18 Jul 1997 14:49:40   Jay
// Moved CObjectStateArray to pmgobj.
// 
//    Rev 1.14   15 Jul 1997 17:31:12   Jay
// Fixed a bug in CCmdSelects related to relayering. (-1, 0 were reversed).
// 
//    Rev 1.13   09 Jun 1997 17:30:50   Jay
// Text improvements
// 
//    Rev 1.12   28 Mar 1997 17:10:42   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.11   08 Jan 1997 15:21:12   Fred
// More text box changes
// 
//    Rev 1.10   28 Oct 1996 13:53:24   Jay
// Moved from 3.01 source
// 
//    Rev 1.11   22 Oct 1996 08:20:36   Jay
// Did more complete fix for last bug.
// 
//    Rev 1.10   21 Oct 1996 16:54:18   Jay
// Fixed a sequencing/ownership bug with deleted text frames and the text record.
// 
//    Rev 1.9   10 Sep 1996 09:11:38   Jay
// Moves objects OnPage() (and off) as necessary.
// 
//    Rev 1.8   01 Sep 1996 13:01:54   Fred
// Add and delete pages mark document as modified
// 
//    Rev 1.7   01 Aug 1996 17:45:32   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.6   26 Jul 1996 18:15:16   Jay
// Add/delete commands
// 
//    Rev 1.5   26 Jul 1996 11:39:22   Jay
// Page stuff
// 
//    Rev 1.4   16 Jul 1996 11:26:04   Jay
// Linked frames with cut/copy/paste/duplicate
// 
//    Rev 1.3   15 Jul 1996 17:46:26   Jay
//  
// 
//    Rev 1.2   12 Jul 1996 18:01:20   Jay
// Undo/redo commands now handle frame marking and updating
// 
//    Rev 1.1   28 Jun 1996 17:32:20   Jay
// OnPage() routine.
// 
//    Rev 1.0   14 Mar 1996 13:35:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:38   FRED
// Initial revision.
// 
//    Rev 1.6   08 Sep 1995 15:44:40   JAY
// Now treats note cards like cards.
// 
//    Rev 1.5   08 Sep 1995 09:35:22   JAY
// BigChange commands now passes command id to base class.
// 
//    Rev 1.4   01 Sep 1995 14:24:42   JAY
// Added undo for title text edit.
// 
//    Rev 1.3   10 May 1995 13:20:18   JAY
// Flip now toffles selects as needed for new objects.
// 
//    Rev 1.2   15 Mar 1995 10:49:06   JAY
// Support for new text editing
// 
//    Rev 1.1   09 Mar 1995 08:35:50   JAY
// More undo.
// 
//    Rev 1.0   15 Feb 1995 17:17:14   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "grafobj.h"
#include "backobj.h"
#include "grpobj.h"
#include "frameobj.h"
#include "brdrObj.h"
#include "OleObj.h"
#include "borders.h"
#include "utils.h"
#include "commands.h"
#include "cardview.h"
#include "textrec.h"
#include "framerec.h"
#include "textflow.h"
#include "pagerec.h"
#include "hyperrec.h"

/////////////////////////////////////////////////////////////////////////////
// CCmdSelects

CCmdSelects::CCmdSelects(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pView = NULL;
	m_pDoc = NULL;
	m_fDetached = FALSE;
}

CCmdSelects::~CCmdSelects()
{
	// If the objects have been detached,
	//   delete them now since they are orphans.

	DeleteObjects();
}

void CCmdSelects::DeleteObjects(BOOL fDoDelete /*=TRUE*/)
{
	if (m_fDetached)
	{
		POSITION pos = m_Objects.GetHeadPosition();

//		CMapPtrToWord Map;
		while (pos != NULL)
		{
			PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

//			DeleteFrames(pObject, Map);

			pObject->destroy();			// The object is finally being deleted.
			if (fDoDelete)
			{
				delete pObject;				// Get rid of memory as well.
			}
		}

		// Now run through the list and delete all text records.
		PMGDatabase* pDatabase = m_pDoc->get_database();

		for (POSITION Pos = m_mapDeletedText.GetStartPosition(); Pos != NULL;)
		{
			void* Key;
			WORD Value;
			m_mapDeletedText.GetNextAssoc(Pos, Key, Value);

			if (Key != NULL)
			{
				pDatabase->DeleteStory((DB_RECORD_NUMBER)Key);
			}
			else
			{
				// NULL???
				ASSERT(FALSE);
			}
		}
	}
}

#if 0
void CCmdSelects::DeleteFrames(PMGPageObject* pObject, CMapPtrToWord& Map)
{
	switch (pObject->type())
	{
		case OBJECT_TYPE_Frame:
		{
			// We have a frame. Delete the text if we need to.
			if (m_fDeleteText)
			{
				CTextRecord* pText = ((CFrameObject*)pObject)->LockTextRecord();
				ASSERT(pText->FirstFrame() == 0);
				if (pText->FirstFrame() == 0)
				{
					Map.SetAt((LPVOID)pText->Id(), 1);
				}
				pText->release();
			}
			break;
		}
		case OBJECT_TYPE_Group:
		{
			pObject = (PMGPageObject*)(((GroupObject*)pObject)->object_list()->first_object());
			while (pObject != NULL)
			{
				DeleteFrames(pObject, Map);
				pObject = (PMGPageObject*)pObject->next_object();
			}
			break;
		}
		default:
		{
			break;
		}
	}
}
#endif

/*
// Take a snapshot of the current objects on the page.
// This remembers the objects as well as their layers.
*/

BOOL CCmdSelects::Snapshot(CPmwView* pView, BOOL fCanRelayer /*=TRUE*/)
{
// Remember the document.

	m_pView = pView;
	m_pDoc = pView->GetDocument();

// Objects are not detached yet.

	m_fDetached = FALSE;

// Build a list of objects so we will know which ones to take.

	PMGPageObject* pObject = m_pDoc->objects();
	PMGPageObject* pSelectedObject;
	int nIndex = fCanRelayer ? 0 : -1;		// -1 means no layer change

	for (pSelectedObject = m_pDoc->selected_objects();
		  pSelectedObject != NULL;
		  pSelectedObject = pSelectedObject->next_selected_object())
	{
	/* Compute the index for this selected object. */

		if (fCanRelayer)
		{
			while (pObject != pSelectedObject && pObject != NULL)
			{
				pObject = (PMGPageObject*)pObject->next_object();
				nIndex++;
			}

			if (pObject == NULL)
			{
			/* Bad news. The lists are not consistent. */
				m_Objects.RemoveAll();
				m_Positions.RemoveAll();
				return FALSE;
			}
		}

	/* Add the next object. */

		m_Objects.AddTail(pSelectedObject);
		m_Positions.Add((WORD)nIndex);
	}
	return TRUE;
}

void CCmdSelects::SelectAll(void)
{
/*
// Run through all objects.
*/
	PMGPageObject* pObject = m_pDoc->objects();
	while (pObject != NULL)
	{
		if (m_Objects.Find(pObject))
		{
			m_pDoc->select_object(pObject);
		}
		else
		{
			m_pDoc->deselect_object(pObject);
		}

		pObject = (PMGPageObject*)pObject->next_object();
	}
}

void CCmdSelects::RelayerObjects(BOOL fUpdateMenu /*=TRUE*/)
{
   // MLSTODO: fUpdateMenu is no longer used. Remove it!
	BOOL fWasDetached = m_fDetached;

/*
// Adjust the position of the objects in the document list.
*/

// Remove the objects from the document list in preparation for layering.

	DetachObjects(FALSE, FALSE);

	int nIndex = 0;

	POSITION pos = m_Objects.GetHeadPosition();
	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL)
		{
			m_pDoc->insert_object(pObject, (int)m_Positions.GetAt(nIndex));
			if (fUpdateMenu)
			{
				pObject->OnPage(TRUE);
			}
			m_pDoc->refresh_object(pObject);
			m_pDoc->select_object(pObject);
			m_pDoc->MarkFramesBelow(pObject);
		// Object is in our list still.
		}
		nIndex++;
	}

	if (fWasDetached)
	{
		AttachFrames();
	}

	// Objects are no longer detached.
	m_fDetached = FALSE;
	m_pDoc->SetModifiedFlag();
	m_pDoc->UpdateMarkedFrames(TRUE);
}

void CCmdSelects::DetachObjects(BOOL fUpdateMenu /*=TRUE*/, BOOL fDoIntersect /*=TRUE*/)
{
   // MLSTODO: fUpdateMenu is no longer used. Remove it!
	m_pDoc->deselect_all();

	if (!m_fDetached)
	{
	/* Remove them from the document's list. */

		POSITION pos = m_Objects.GetHeadPosition();

		while (pos != NULL)
		{
			PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

			if (pObject != NULL)
			{
				m_pDoc->MarkFramesBelow(pObject);
				m_pDoc->refresh_object(pObject);
				m_pDoc->detach_object(pObject);
				if (fUpdateMenu)
				{
					pObject->OnPage(FALSE);
				}
			/* Object is in our list already. */
			}
		}

		m_fDetached = TRUE;
		m_pDoc->SetModifiedFlag();

		if (fDoIntersect)
		{
			m_pDoc->UpdateMarkedFrames(TRUE);
		}
	}
}

void CCmdSelects::DetachFrames(void)
{
	CMapPtrToPtr Map;
	POSITION pos = m_Objects.GetHeadPosition();

	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);
		ASSERT(pObject != NULL);

		DetachFrames(pObject, Map);
		ReflowFrames(Map);
	}
}

void CCmdSelects::DetachFrames(PMGPageObject* pObject, CMapPtrToPtr& Map)
{
	switch (pObject->type())
	{
		case OBJECT_TYPE_Frame:
		{
			// We have a frame. Detach it.
			PMGDatabase* pDatabase = m_pDoc->get_database();

			// Lock the frame record to do the actual detach.
			CFrameRecord* pFrame = ((CFrameObject*)pObject)->LockFrameRecord();
			if (pFrame != NULL)
			{
				// Handle the previous frame.
				if (pFrame->PreviousFrame() == 0)
				{
					// We need to detach from the text record.
					CTextRecord* pText = pFrame->LockTextRecord();
					if (pText != NULL)
					{
						if (pText->FirstFrame() == pFrame->Id())
						{
							pText->FirstFrame(pFrame->NextFrame());
							if (pFrame->NextFrame() == 0)
							{
								// We are detaching first frame.
								pDatabase->RemoveStory(pText->Id());
								m_mapDeletedText.SetAt((LPVOID)pText->Id(), 1);
							}
						}
						else
						{
							// We think we are first frame, but text record does not.
							ASSERT(FALSE);
						}
						pText->release(TRUE);
					}
				}
				else
				{
					// Lock the previous frame.
					CFrameRecord* pPreviousFrame = pFrame->LockPreviousFrame();
					if (pPreviousFrame != NULL)
					{
						if (pPreviousFrame->NextFrame() == pFrame->Id())
						{
							pPreviousFrame->NextFrame(pFrame->NextFrame());
						}
						else
						{
							// Link to previous frame is broken.
							ASSERT(FALSE);
						}
						pPreviousFrame->release();
					}
				}
				// Handle the next frame.
				if (pFrame->NextFrame() != 0)
				{
					// Lock the next frame.
					CFrameRecord* pNextFrame = pFrame->LockNextFrame();
					if (pNextFrame != NULL)
					{
						if (pNextFrame->PreviousFrame() == pFrame->Id())
						{
							pNextFrame->PreviousFrame(pFrame->PreviousFrame());

							// Local correction. All frames after this are hosed,
							// though. But, textflow should start before us.
							pNextFrame->TextState() = pFrame->TextState();
						}
						else
						{
							// Link to next frame is broken.
							ASSERT(FALSE);
						}
						pNextFrame->release();
					}
				}

				// Record our first position for reflow.
				DB_RECORD_NUMBER lText = pFrame->TextRecord();
				CHARACTER_INDEX lFirstCharacter = pFrame->FirstCharacter();
				if (lText != 0 && lFirstCharacter != -1)
				{
					void* pValue;
					if (!Map.Lookup((void*)lText, pValue)
						 || (DB_RECORD_NUMBER)pValue > lFirstCharacter)
					{
						Map.SetAt((void*)lText, (void*)lFirstCharacter);
					}
				}

				pFrame->release(TRUE);
			}
			break;
		}
		case OBJECT_TYPE_Group:
		{
			pObject = (PMGPageObject*)(((GroupObject*)pObject)->object_list()->first_object());
			while (pObject != NULL)
			{
				DetachFrames(pObject, Map);
				pObject = (PMGPageObject*)pObject->next_object();
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

/*
// Attach the frames that have been previously detached.
// Note that we need to traverse the list in reverse order for this to
// work correctly.
*/

void CCmdSelects::AttachFrames(void)
{
	CMapPtrToPtr Map;
	POSITION pos = m_Objects.GetTailPosition();

	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetPrev(pos);
		ASSERT(pObject != NULL);

		AttachFrames(pObject, Map);
		ReflowFrames(Map);
	}
}

void CCmdSelects::AttachFrames(PMGPageObject* pObject, CMapPtrToPtr& Map)
{
	switch (pObject->type())
	{
		case OBJECT_TYPE_Frame:
		{
			// We have a frame. Attach it.
			PMGDatabase* pDatabase = m_pDoc->get_database();

			// Lock the frame record to do the actual attach.
			CFrameRecord* pFrame = ((CFrameObject*)pObject)->LockFrameRecord();
			if (pFrame != NULL)
			{
				// Handle the previous frame.
				if (pFrame->PreviousFrame() == 0)
				{
					// We need to attach to the text record.
					CTextRecord* pText = pFrame->LockTextRecord();
					if (pText != NULL)
					{
						if (pText->FirstFrame() == pFrame->NextFrame())
						{
							pText->FirstFrame(pFrame->Id());
							if (pFrame->NextFrame() == 0)
							{
								// We are attaching first frame.
								pDatabase->AddStory(pText->Id());
							}
						}
						else
						{
							// Attach failed at text record.
							ASSERT(FALSE);
						}
						pText->release(TRUE);
					}
				}
				else
				{
					// Lock the previous frame.
					CFrameRecord* pPreviousFrame = pFrame->LockPreviousFrame();
					if (pPreviousFrame != NULL)
					{
						if (pPreviousFrame->NextFrame() == pFrame->NextFrame())
						{
							pPreviousFrame->NextFrame(pFrame->Id());
						}
						else
						{
							// Attach: link to previous frame is broken.
							ASSERT(FALSE);
						}
						pPreviousFrame->release();
					}
				}
				// Handle the next frame.
				if (pFrame->NextFrame() != 0)
				{
					// Lock the next frame.
					CFrameRecord* pNextFrame = pFrame->LockNextFrame();
					if (pNextFrame != NULL)
					{
						if (pNextFrame->PreviousFrame() == pFrame->PreviousFrame())
						{
							pNextFrame->PreviousFrame(pFrame->Id());
						}
						else
						{
							// Attach: link to next frame is broken.
							ASSERT(FALSE);
						}
						pNextFrame->release();
					}
				}

				// Record our first position for reflow.
				DB_RECORD_NUMBER lText = pFrame->TextRecord();
				CHARACTER_INDEX lFirstCharacter = pFrame->FirstCharacter();
				if (lText != 0 && lFirstCharacter != -1)
				{
					void* pValue;
					if (!Map.Lookup((void*)lText, pValue)
						 || (DB_RECORD_NUMBER)pValue > lFirstCharacter)
					{
						Map.SetAt((void*)lText, (void*)lFirstCharacter);
					}
				}
				pFrame->release(TRUE);
			}
			break;
		}
		case OBJECT_TYPE_Group:
		{
			pObject = (PMGPageObject*)(((GroupObject*)pObject)->object_list()->last_object());
			while (pObject != NULL)
			{
				AttachFrames(pObject, Map);
				pObject = (PMGPageObject*)pObject->previous_object();
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void CCmdSelects::ReflowFrames(CMapPtrToPtr& Map)
{
	// Now run through the list and delete all text records.
	PMGDatabase* pDatabase = m_pDoc->get_database();
	
	for (POSITION Pos = Map.GetStartPosition(); Pos != NULL; )
	{
		void* pKey;
		void* pValue;
		Map.GetNextAssoc(Pos, pKey, pValue);

		if (pKey != NULL)
		{
			CTextRecord* pText = (CTextRecord*)pDatabase->get_record((DB_RECORD_NUMBER)pKey, NULL, RECORD_TYPE_Text);
			if (pText != NULL)
			{
				if (pText->FirstFrame() != 0)
				{
					// Some frames left. Flow them.
					CTextFlow TextFlow(pText->FirstFrame(),
											 pDatabase,
											 (CHARACTER_INDEX)pValue,
											 -1);
					TextFlow.FlowText();
				}
				pText->release();
			}
		}
		else
		{
			// NULL???
			ASSERT(FALSE);
		}
	}
}

void CCmdSelects::ViewObjects(PMGPageObject* pObject /*=NULL*/)
{
	if (pObject == NULL)
	{
		POSITION pos = m_Objects.GetTailPosition();

		if (pos != NULL)
		{
			pObject = (PMGPageObject*)m_Objects.GetPrev(pos);
		}
	}

	if (pObject != NULL)
	{
		m_pView->ViewObject(pObject);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCmdRelayerSelects

CCmdRelayerSelects::CCmdRelayerSelects(WORD wCmdID /*=0*/)
	: CCmdSelects(wCmdID)
{
}

BOOL CCmdRelayerSelects::UndoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	// Put the objects at the correct location in the list.
	RelayerObjects();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdDeleteSelects

CCmdDeleteSelects::CCmdDeleteSelects(WORD wCmdID /*=0*/)
	: CCmdRelayerSelects(wCmdID == 0 ? IDCmd_DeleteSelects : wCmdID)
{
}

BOOL CCmdDeleteSelects::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	// Actually detach the objects from the list.
	// This is the basic delete.
	CloseOLEItems();
	DetachFrames();
	DetachObjects();

	return TRUE;
}

// Close any open OLE client items.
void CCmdDeleteSelects::CloseOLEItems(void)
{
	POSITION pos = m_Objects.GetHeadPosition();

	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL
				&& (pObject->type() == OBJECT_TYPE_OleObject
					 || pObject->type() == OBJECT_TYPE_Serif))
		{
			COleClientItem* pItem = ((OleObject*)pObject)->Item();
			if (pItem != NULL && pItem->IsOpen())
			{
				pItem->Close(OLECLOSE_PROMPTSAVE);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCmdCreateSelects

CCmdCreateSelects::CCmdCreateSelects(WORD wCmdID /*=0*/)
	: CCmdRelayerSelects(wCmdID)
{
}

BOOL CCmdCreateSelects::UndoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	// Actually detach the objects from the list.
	// This undoes the create.
	DetachFrames();
	DetachObjects();

	return TRUE;
}

BOOL CCmdCreateSelects::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	// Put the objects at the correct location in the list.
	// This redoes the create.
	RelayerObjects();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdSendToFront

CCmdSendToFront::CCmdSendToFront()
	: CCmdRelayerSelects(IDCmd_SendToFront)
{
}

BOOL CCmdSendToFront::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	// Remove them from the list.
	DetachObjects(FALSE, FALSE);

	// Reattach them in the front.
	POSITION pos = m_Objects.GetHeadPosition();
	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL)
		{
			m_pDoc->append_object(pObject);
			m_pDoc->refresh_object(pObject);
			m_pDoc->select_object(pObject);
			m_pDoc->MarkFramesBelow(pObject);
		// Object is in our list still.
		}
	}

	// Objects are no longer detached.
	m_fDetached = FALSE;
	m_pDoc->SetModifiedFlag();
	m_pDoc->UpdateMarkedFrames(TRUE);

   m_pView->free_edit_background();

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdSendToBack

CCmdSendToBack::CCmdSendToBack()
	: CCmdRelayerSelects(IDCmd_SendToBack)
{
}

BOOL CCmdSendToBack::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	// Remove them from the list.
	DetachObjects(FALSE, FALSE);

	// Find the place to re-insert the objects.
	ObjectListPtr pList = m_pDoc->object_list();

	PMGPageObjectPtr pBefore = (PMGPageObjectPtr)pList->first_that(CPmwDoc::NotBorderCriterion);

	// Reattach them in the back.
	POSITION pos = m_Objects.GetHeadPosition();
	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL)
		{
			pList->insert_before(pObject, pBefore);
			m_pDoc->refresh_object(pObject);
			m_pDoc->select_object(pObject);
			m_pDoc->MarkFramesBelow(pObject);
		// Object is in our list still.
		}
	}

	// Objects are no longer detached.
	m_fDetached = FALSE;
	m_pDoc->SetModifiedFlag();
	m_pDoc->UpdateMarkedFrames(TRUE);

   m_pView->free_edit_background();

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Common code for ForwardOne and BackOne.

typedef struct
{
	PBOX test_pbox;
	SHORT panel;
} OVERLAPPER_STRUCT, far *OVERLAPPER_STRUCT_PTR;

static BOOL OverlapCriterion(ListElementPtr element, VOIDPTR data)
{
	OVERLAPPER_STRUCT_PTR osp = (OVERLAPPER_STRUCT_PTR)data;
	PBOX pbox;

	if (osp->panel != -1 && ((PMGPageObjectPtr)element)->get_panel() != osp->panel)
	{
	/* Ignore this object on a different panel. */
		return FALSE;
	}
	((PMGPageObjectPtr)element)->get_refresh_bound(&pbox, NULL);

	return IntersectBox(&pbox, &pbox, &osp->test_pbox);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdBackOne

CCmdBackOne::CCmdBackOne()
	: CCmdRelayerSelects(IDCmd_BackOne)
{
}

BOOL CCmdBackOne::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	ObjectListPtr pList = m_pDoc->object_list();

	PMGPageObjectPtr pBefore = (PMGPageObjectPtr)pList->first_that(CPmwDoc::NotBorderCriterion);

	// Reattach them in the back.
	POSITION pos = m_Objects.GetHeadPosition();
	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL)
		{
			OVERLAPPER_STRUCT os;

			pObject->get_refresh_bound(&os.test_pbox, NULL);

			if (m_pDoc->get_project_type() == PROJECT_TYPE_Card
					|| m_pDoc->get_project_type() == PROJECT_TYPE_NoteCard)
			{
				os.panel = pObject->get_panel();
			}
			else
			{
				os.panel = -1;
			}

			PMGPageObjectPtr pFirstObject;

		/* Always insert after borders. */

//			pFirstObject = (PMGPageObjectPtr)pList->first_that(CPmwDoc::LastBorderCriterion);
			pFirstObject = (PMGPageObjectPtr)pList->first_that(CPmwDoc::NotBorderCriterion);

			/*
			// Get our object's bounds and find the last object before it
			// that overlaps it.
			*/

			ListElementPtr pOverlapper = pList->last_that(OverlapCriterion,
								&os,
								pFirstObject,
								pObject->previous_object());

//			if (pOverlapper != pFirstObject)
			{
				m_pDoc->MarkFramesBelow(pObject);
				m_pDoc->detach_object(pObject);

				/*
				// ...and insert before its overlapper, or at the beginning if none
				// found.
				*/

				if (pOverlapper != NULL)
				{
					pList->insert_before(pObject, pOverlapper);
				}
				else
				{
//					pList->insert_after(pObject, pFirstObject);
					pList->insert_before(pObject, pFirstObject);
				}

				m_pDoc->refresh_object(pObject);
				m_pDoc->select_object(pObject);
				m_pDoc->MarkFramesBelow(pObject);
				// Object is in our list still.
			}
		}
	}

	// Objects are no longer detached.
	m_fDetached = FALSE;
	m_pDoc->SetModifiedFlag();
	m_pDoc->UpdateMarkedFrames(TRUE);

   m_pView->free_edit_background();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdForwardOne

CCmdForwardOne::CCmdForwardOne()
	: CCmdRelayerSelects(IDCmd_ForwardOne)
{
}

BOOL CCmdForwardOne::DoIt(void)
{
	if (m_Objects.GetCount() != 1)
	{
	// We shouldn't even be here.
		return FALSE;
	}

	// Make sure we are on the right page.
	ViewObjects();

	// Move them forward a layer.
	ObjectListPtr pList = m_pDoc->object_list();

	POSITION pos = m_Objects.GetHeadPosition();
	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL)
		{
			OVERLAPPER_STRUCT os;

			pObject->get_refresh_bound(&os.test_pbox, NULL);

			if (m_pDoc->get_project_type() == PROJECT_TYPE_Card
				 || m_pDoc->get_project_type() == PROJECT_TYPE_NoteCard)
			{
				os.panel = pObject->get_panel();
			}
			else
			{
				os.panel = -1;
			}

		/*
		// Find the first overlapper.
		*/

			ListElementPtr pOverlapper = pList->first_that(OverlapCriterion,
 							&os,
							pObject->next_object());

			m_pDoc->MarkFramesBelow(pObject);
			m_pDoc->detach_object(pObject);

			/*
			// and insert after its overlapper, or at the end of the list if
			// there is no overlapper
			*/

			if (pOverlapper != NULL)
			{
				/* Insert object after overlapper */
				pList->insert_after(pObject, pOverlapper);
			}
			else
			{
				pList->append(pObject);
			}

			m_pDoc->select_object(pObject);
			m_pDoc->refresh_object(pObject);
			m_pDoc->MarkFramesBelow(pObject);
		// Object is in our list still.
		}
	}

	// Objects are no longer detached.
	m_fDetached = FALSE;
	m_pDoc->SetModifiedFlag();
	m_pDoc->UpdateMarkedFrames(TRUE);

   m_pView->free_edit_background();

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdChangeSelects

CCmdChangeSelects::CCmdChangeSelects(WORD wCmdID /*=0*/)
	: CCmdSelects(wCmdID)
{
}

/*
// Our overload of snapshot. This should not be called.
*/

BOOL CCmdChangeSelects::Snapshot(CPmwView* pView)
{
	return FALSE;
}

/*
// Set things up before the change happens.
*/

BOOL CCmdChangeSelects::Before(CPmwView* pView, BOOL fCanRelayer /*=TRUE*/)
{
	return INHERITED::Snapshot(pView, fCanRelayer) && SaveStates(&m_UndoStates);
}

/*
// Set things up before the change happens.
*/

BOOL CCmdChangeSelects::After(void)
{
	return SomethingChanged() && SaveStates(&m_RedoStates);
}

/*
// Undo this command.
*/

BOOL CCmdChangeSelects::UndoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	SelectAll();
	return RestoreStates(&m_UndoStates);
}

/*
// Do this command.
*/

BOOL CCmdChangeSelects::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	SelectAll();
	return RestoreStates(&m_RedoStates);
}

/*
// Save the states of the selected objects.
*/

BOOL CCmdChangeSelects::SaveStates(CObjectStateArray* pStates)
{
// Free any old states saved here.

	pStates->DeleteAll();

// Run through all objects and save their states.
	POSITION pos = m_Objects.GetHeadPosition();

	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		PageObjectState* pState = pObject->CreateState();
		if (pState == NULL)
		{
		// Failure!
			return FALSE;
		}
		pObject->SaveState(pState);
		pStates->Add(pState);
	}
	return TRUE;
}

/*
// Restore the states of the selected objects.
//
// This routine can be used for Undo or Redo (depending on which state array
// you pass in!)
//
// It is assumed that there is a one-to-one correspondence between objects
// and states.
*/

BOOL CCmdChangeSelects::RestoreStates(CObjectStateArray* pStates, BOOL fSelected /*=TRUE*/)
{
	int nCount = pStates->GetSize();
// Sanity check.
	if (nCount != m_Objects.GetCount())
	{
		od("Counts don't match\r\n");
		return FALSE;
	}
// Run through all objects and restore their states.
	POSITION pos = m_Objects.GetHeadPosition();

	for (int nIndex = 0; nIndex < nCount && pos != NULL; nIndex++)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (fSelected)
		{
			m_pDoc->toggle_object(pObject);
		}
		m_pDoc->MarkFramesBelow(pObject);
		pObject->RestoreState(pStates->GetAt(nIndex));
		m_pDoc->MarkFramesBelow(pObject);
		if (fSelected)
		{
			m_pDoc->toggle_object(pObject);
		}
	}
	m_pDoc->SetModifiedFlag();
	m_pDoc->UpdateMarkedFrames(TRUE);
	m_pDoc->resize_project();		// In case of a positional change.
	return TRUE;
}

BOOL CCmdChangeSelects::SomethingChanged(void)
{
/* Since we don't know... */
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdChangeObject

CCmdChangeObject::CCmdChangeObject(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pView = NULL;
	m_pDoc = NULL;
	m_pObject = NULL;
	m_pOldState = NULL;
	m_pNewState = NULL;
}

CCmdChangeObject::~CCmdChangeObject()
{
	delete m_pOldState;
	delete m_pNewState;
}

void CCmdChangeObject::ViewObject(void)
{
	if (m_pObject != NULL)
	{
		m_pView->ViewObject(m_pObject);
	}
}

BOOL CCmdChangeObject::Before(CPmwView* pView, PMGPageObject* pObject)
{
	m_pView = pView;
	m_pDoc = pView->GetDocument();
	m_pObject = pObject;
	m_pOldState = pObject->CreateState();
	if (m_pOldState == NULL)
	{
		return FALSE;
	}
	pObject->SaveState(m_pOldState);
	return TRUE;
}

BOOL CCmdChangeObject::After(void)
{
	m_pNewState = m_pObject->CreateState();
	if (m_pNewState == NULL)
	{
		return FALSE;
	}
	m_pObject->SaveState(m_pNewState);
	return TRUE;
}

/*
// Undo this command.
*/

BOOL CCmdChangeObject::UndoIt(void)
{
	// Make sure we are on the right page.
	ViewObject();

	m_pDoc->MarkFramesBelow(m_pObject);
	m_pObject->RestoreState(m_pOldState);
	m_pDoc->MarkFramesBelow(m_pObject);

	m_pDoc->UpdateMarkedFrames(TRUE);
	m_pDoc->SetModifiedFlag();
	return TRUE;
}

/*
// Do this command.
*/

BOOL CCmdChangeObject::DoIt(void)
{
	// Make sure we are on the right page.
	ViewObject();

	m_pDoc->MarkFramesBelow(m_pObject);
	m_pObject->RestoreState(m_pNewState);
	m_pDoc->MarkFramesBelow(m_pObject);

	m_pDoc->UpdateMarkedFrames(TRUE);
	m_pDoc->SetModifiedFlag();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdFlipSelects

CCmdFlipSelects::CCmdFlipSelects(WORD wCmdID)
	: CCmdSelects(wCmdID)
{
}

BOOL CCmdFlipSelects::FlipSelects(void)
{
	// Make sure we are on the right page.
	ViewObjects();

	CPmwDoc* pDoc = m_pDoc;

	SelectAll();

	pDoc->toggle_selects();

	// Turn off any text select (this happens with tables).
	CTxp* pTxp = m_pView->GetTxp();
	if (pTxp != NULL && pTxp->Valid())
	{
		int nVisible = pTxp->Visible();
		pTxp->Off(nVisible);
		pTxp->InvalidateSelectBounds();
	}

	POSITION pos = m_Objects.GetHeadPosition();
	while (pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		m_pDoc->MarkFramesBelow(pObject);
		pDoc->refresh_object(pObject);
		if (m_wCmdID == IDCmd_Mirror)
		{
			pObject->xflip();
		}
		else
		{
			pObject->yflip();
		}
		m_pDoc->MarkFramesBelow(pObject);
		pDoc->refresh_object(pObject);
	}

	pDoc->SetModifiedFlag();
	pDoc->UpdateMarkedFrames(TRUE);
	pDoc->toggle_selects();

	return TRUE;
}

/*
// Undo this command.
*/

BOOL CCmdFlipSelects::UndoIt(void)
{
	return FlipSelects();
}

/*
// Do this command.
*/

BOOL CCmdFlipSelects::DoIt(void)
{
	return FlipSelects();
}

/////////////////////////////////////////////////////////////////////////////
// CCmdNewBackground

CCmdNewBackground::CCmdNewBackground(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pDoc = NULL;
	m_pSwapBackground = NULL;
}

CCmdNewBackground::~CCmdNewBackground()
{
	delete m_pSwapBackground;
}

BackgroundObject* CCmdNewBackground::SelectBackground(BackgroundObject* pBackground)
{
	m_pDoc->deselect_all();

/* Remove the old background. */

	BackgroundObject* pOldBackground = m_pDoc->find_background();

	if (pOldBackground != NULL)
	{
		m_pDoc->refresh_object(pOldBackground);
		m_pDoc->detach_object(pOldBackground);
	}

/* Attach the new background. */

	if (pBackground != NULL)
	{
		m_pDoc->attach_background(pBackground);
		m_pDoc->refresh_object(pBackground);
		m_pDoc->select_object(pBackground);
	}

	return pOldBackground;
}

BOOL CCmdNewBackground::SwapBackground(void)
{
	m_pSwapBackground = SelectBackground(m_pSwapBackground);
	return TRUE;
}

/*
// Undo this command.
*/

BOOL CCmdNewBackground::UndoIt(void)
{
	return SwapBackground();
}

/*
// Do this command.
*/

BOOL CCmdNewBackground::DoIt(void)
{
	return SwapBackground();
}

/////////////////////////////////////////////////////////////////////////////
// CCmdGroup

CCmdGroup::CCmdGroup(WORD wCmdID /*=0*/)
	: CCmdChangeSelects(wCmdID)
{
	m_fInUse = FALSE;
	m_pGroup = NULL;
}

CCmdGroup::~CCmdGroup()
{
	if (!m_fInUse)
	{
		delete m_pGroup;
	}
}

/*
// Move the objects out of the group.
*/

BOOL CCmdGroup::Ungroup()
{
	// Make sure we are on the right page.
	ViewObjects();

/*
// This is, in effect, an ungroup.
*/
/* Get the group list. */

	ObjectList* pGroupList = m_pGroup->object_list();

/* Remove all objects from the list */

	while (pGroupList->count() != 0)
	{
		PMGPageObject* pObject = (PMGPageObject*)pGroupList->first_object();

		pObject->leave_group();

		pGroupList->detach(pObject);
	}

/* Remove the group itself from the document list. */

	m_pDoc->detach_object(m_pGroup);
	m_fInUse = FALSE;

/* Add the objects into the list in the correct place. */

	RelayerObjects();

/* And restore all of their states. */

	if (m_fUngrouping)
	{
		return TRUE;
	}
	else
	{
		return RestoreStates(&m_UndoStates);
	}
}

/*
// Move the objects into the group.
*/

BOOL CCmdGroup::Group()
{
	// Make sure we are on the right page.
	ViewObjects();

/*
// This is the actual grouping code.
*/
	PBOX OriginalBound = m_pGroup->get_original_bound();

	ObjectList* pGroupList = m_pGroup->object_list();
	PMGPageObject* pBefore = NULL;

	m_pDoc->deselect_all();

/* Do the grouping thing. */

	POSITION pos = m_Objects.GetHeadPosition();

	while (pos != NULL)
	{
		POSITION oldpos = pos;
		PMGPageObject* pObject = (PMGPageObject*)m_Objects.GetNext(pos);

		if (pObject != NULL && pObject->join_group(&OriginalBound, m_pGroup))
		{
			pBefore = (PMGPageObject*)pObject->next_object();

		/*
		// Remove us from the page list.
		*/

			m_pDoc->MarkFramesBelow(pObject);
			m_pDoc->detach_object(pObject);

		/*
		// Hook us into the group.
		*/

			pGroupList->append(pObject);

		/*
		// Get rid of any layout.
		*/

			pObject->set_new_layout(MANUAL_LAYOUT);
		}
		else
		{
		/* We don't want this in the list. */
			m_Objects.RemoveAt(oldpos);
		}
	}

	if (m_fUngrouping)
	{
		RestoreStates(&m_RedoStates, FALSE);
	}

/* Select the new group. */

	m_pDoc->object_list()->insert_before(m_pGroup, pBefore);
	m_fInUse = TRUE;

	m_pGroup->calc();

	m_pDoc->refresh_object(m_pGroup);	// In case an object changed layering.
	m_pDoc->MarkFramesBelow(m_pGroup);
	m_pDoc->UpdateMarkedFrames(TRUE);

	m_pDoc->select_object(m_pGroup);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdGroupSelects

CCmdGroupSelects::CCmdGroupSelects()
	: CCmdGroup(IDCmd_GroupSelects)
{
	m_fUngrouping = FALSE;
}

/*
// Undo this command.
*/

BOOL CCmdGroupSelects::UndoIt(void)
{
	return Ungroup();
}

/*
// Do this command.
*/

BOOL CCmdGroupSelects::DoIt(void)
{
	return Group();
}

/*
// Take a snapshot of things just prior to the grouping.
*/

BOOL CCmdGroupSelects::Snapshot(CPmwView* pView, GroupObject* pGroup)
{
	m_pGroup = pGroup;
	m_fInUse = FALSE;

// This will remember the positions and the states.

	return INHERITED::Before(pView);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdUngroupSelects

CCmdUngroupSelects::CCmdUngroupSelects()
	: CCmdGroup(IDCmd_UngroupSelects)
{
	m_fUngrouping = TRUE;
}

/*
// Undo this command.
*/

BOOL CCmdUngroupSelects::UndoIt(void)
{
	return Group();
}

/*
// Do this command.
*/

BOOL CCmdUngroupSelects::DoIt(void)
{
	return Ungroup();
}

/*
// Take a snapshot of things just prior to the ungrouping.
*/

BOOL CCmdUngroupSelects::Snapshot(CPmwView* pView, GroupObject* pGroup)
{
	m_pGroup = pGroup;
	m_fInUse = TRUE;
	m_pView = pView;
	m_pDoc = pView->GetDocument();

/*
// Simulate what we would have seen BEFORE the objects were grouped.
// In other words, generate the state AFTER the objects are ungrouped.
*/

/* Find the index of the group in the list. */

	PMGPageObject* pObject = m_pDoc->objects();
	int nIndex = 0;

	while (pObject != pGroup && pObject != NULL)
	{
		pObject = (PMGPageObject*)pObject->next_object();
		nIndex++;
	}

/* This is the base index for the objects. */

	for (pObject = (PMGPageObject*)pGroup->object_list()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		m_Objects.AddTail(pObject);
		// Fixup the panel number now.
		pObject->set_panel(pGroup->get_panel());
		m_Positions.Add((WORD)nIndex++);
	}

/* Save states before the ungroup takes place. */

	return SaveStates(&m_RedoStates);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdMulti

CCmdMulti::CCmdMulti(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
}

CCmdMulti::~CCmdMulti()
{
	int nCount = m_Commands.GetSize();

	for (int i = 0; i < nCount; i++)
	{
		delete (CCommand*)m_Commands.GetAt(i);
	}
}

/*
// Add a command.
*/

void CCmdMulti::AddCommand(CCommand* pCommand)
{
	m_Commands.Add(pCommand);
}

/*
// Undo this command.
*/

BOOL CCmdMulti::UndoIt(void)
{
	int nCount = m_Commands.GetSize();

	for (int i = 0; i < nCount; i++)
	{
		if (!((CCommand*)m_Commands.GetAt(i))->UndoIt())
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
// Do this command.
*/

BOOL CCmdMulti::DoIt(void)
{
	int nCount = m_Commands.GetSize();

	for (int i = 0; i < nCount; i++)
	{
		if (!((CCommand*)m_Commands.GetAt(i))->DoIt())
		{
			return FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdBigChangeObject

CCmdBigChangeObject::CCmdBigChangeObject(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pView = NULL;
	m_pDoc = NULL;
	m_pObject = NULL;
	m_pBeforeObject = NULL;
	m_pAfterObject = NULL;
}

CCmdBigChangeObject::~CCmdBigChangeObject()
{
	delete m_pBeforeObject;
	delete m_pAfterObject;
}

void CCmdBigChangeObject::ViewObject(void)
{
	if (m_pBeforeObject != NULL)
	{
		m_pView->ViewObject(m_pBeforeObject);
	}
	else if (m_pAfterObject != NULL)
	{
		m_pView->ViewObject(m_pAfterObject);
	}
}

BOOL CCmdBigChangeObject::Before(PMGPageObject* pObject, CPmwView* pView)
{
	m_pObject = pObject;
	m_pView = pView;
	m_pDoc = pView->GetDocument();
	m_pBeforeObject = (PMGPageObject*)m_pObject->duplicate();
	if (m_pBeforeObject == NULL)
	{
		return FALSE;
	}
	// The object is not on the page.
	m_pBeforeObject->OnPage(FALSE);

	return TRUE;
}

BOOL CCmdBigChangeObject::After(void)
{
	m_pAfterObject = (PMGPageObject*)m_pObject->duplicate();
	if (m_pAfterObject == NULL)
	{
		return FALSE;
	}
	// The object is not on the page.
	m_pAfterObject->OnPage(FALSE);

	return TRUE;
}

BOOL CCmdBigChangeObject::UndoIt(void)
{
	ViewObject();
	AssignObject(m_pBeforeObject);
	return TRUE;
}

BOOL CCmdBigChangeObject::DoIt(void)
{
	ViewObject();
	AssignObject(m_pAfterObject);
	return TRUE;
}

void CCmdBigChangeObject::AssignObject(PMGPageObject* pStateObject)
{
	if (!m_pDoc->object_selected(m_pObject))
	{
		m_pDoc->deselect_all();
		m_pDoc->select_object(m_pObject);
	}

	m_pDoc->toggle_object(m_pObject);
	m_pDoc->refresh_object(m_pObject);
	m_pDoc->MarkFramesBelow(m_pObject);

	m_pObject->assign(*pStateObject);

	m_pDoc->toggle_object(m_pObject);
	m_pDoc->refresh_object(m_pObject);
	m_pDoc->MarkFramesBelow(m_pObject);

	m_pDoc->UpdateMarkedFrames(TRUE);
	m_pDoc->SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
// CCmdChangePages

CCmdChangePages::CCmdChangePages(WORD wCmdID /*=0*/)
	: CCmdMulti(wCmdID)
{
	m_pView = NULL;
	m_pDoc = NULL;
	m_dwFirstPage = m_dwLastPage = 0;
}

void CCmdChangePages::Snapshot(CPmwView* pView, DWORD dwFirstPage, DWORD dwLastPage)
{
	m_pView = pView;
	m_pDoc = pView->GetDocument();
	m_dwFirstPage = dwFirstPage;
	m_dwLastPage = dwLastPage;
}

/*
// Fixup the current page number after an insertion.
*/

void CCmdChangePages::AfterInsert(void)
{
	// The pages have been added.
	m_pView->BeforePageChange();
	m_pDoc->GotoPage(LastPage());

	m_pDoc->InvalidateLastKnownPage();
}

/*
// Fixup the current page before a deletion.
*/

void CCmdChangePages::BeforeDelete(void)
{
	DWORD dwCurrentPage = m_pDoc->CurrentPageIndex();
	if (dwCurrentPage >= FirstPage() && dwCurrentPage <= LastPage())
	{
		// We need to move the current page.
		if ((dwCurrentPage = LastPage()+1) == m_pDoc->NumberOfPages())
		{
			ASSERT(FirstPage() != 0);
			dwCurrentPage = FirstPage()-1;
		}
		m_pView->BeforePageChange();
		m_pDoc->GotoPage(dwCurrentPage);
	}

	m_pDoc->InvalidateLastKnownPage();
}

/////////////////////////////////////////////////////////////////////////////
// CCmdInsertPages

CCmdInsertPages::CCmdInsertPages(void)
	: CCmdChangePages(IDCmd_AddPages)
{
}

/*
// Do this command.
*/

BOOL CCmdInsertPages::DoIt(void)
{
	if (CCmdChangePages::DoIt())
	{
		AfterInsert();
		return TRUE;
	}
	return FALSE;
}

/*
// Undo this command.
*/

BOOL CCmdInsertPages::UndoIt(void)
{
	BeforeDelete();
	return CCmdChangePages::UndoIt();
}

/////////////////////////////////////////////////////////////////////////////
// CCmdDeletePages

CCmdDeletePages::CCmdDeletePages(void)
	: CCmdChangePages(IDCmd_DeletePages)
{
}

/*
// Do this command.
*/

BOOL CCmdDeletePages::DoIt(void)
{
	BeforeDelete();
	return CCmdChangePages::DoIt();
}

/*
// Undo this command.
*/

BOOL CCmdDeletePages::UndoIt(void)
{
	if (CCmdChangePages::UndoIt())
	{
		AfterInsert();
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdDeletePage

CCmdDeletePage::CCmdDeletePage(WORD wCmdID /*=0*/)
	: CCmdRelayerSelects(wCmdID)
{
	m_pDoc = NULL;
	m_lPage = 0;
	m_fDetached = FALSE;
}

CCmdDeletePage::~CCmdDeletePage()
{
	if (m_pDoc = NULL && m_fDetached && m_lPage != 0)
	{
		DeleteObjects(FALSE);		// Delete withing deleting.
		m_pDoc->get_database()->delete_record(m_lPage);
	}

	// We do not want the base destructor to delete the objects again!
	m_fDetached = FALSE;
}

BOOL CCmdDeletePage::Snapshot(CPmwView* pView, DB_RECORD_NUMBER lPage, DWORD dwIndex)
{
	// Objects are not detached yet.
	BOOL fResult = FALSE;

	// Remember the info.
	m_pView = pView;
	m_pDoc = pView->GetDocument();
	m_lPage = lPage;
	m_dwIndex = dwIndex;
	m_fDetached = FALSE;
	
	PMGDatabase	*pDatabase = m_pDoc->get_database();

	PageRecord* pPage = (PageRecord*)pDatabase->get_record(lPage, NULL, RECORD_TYPE_Page);
	if (pPage != NULL)
	{
		// Build a list of objects so we will know which ones to take.
		int nIndex = 0;

		for (PMGPageObject* pObject = (PMGPageObject*)pPage->objects()->first_object();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
		/* Add the next object. */
			m_Objects.AddTail(pObject);
			m_Positions.Add((WORD)nIndex);

			nIndex++;
		}
		fResult = TRUE;

		if (m_pDoc != NULL)
		{
			DocumentRecord	*pDocRec = m_pDoc->DocumentRecord();
			// Collect hyperlinks
			for (DWORD d = 0; d < pDocRec->NumberOfHyperlinks(); d ++)
			{
				DB_RECORD_NUMBER dbrn = pDocRec->GetHyperlink(d);
				HyperlinkData Data;
				if (pDatabase->GetHyperlinkData(dbrn, Data) == TRUE)
				{
					if ((Data.StringType == TYPE_PageURL) && (Data.PageRecordNumber == lPage))
						mHyperlinksToMe.Add(dbrn);
				}
				else
					ASSERT(FALSE);
			}
		}

		pPage->release();
	}
	return fResult;
}

BOOL CCmdDeletePage::DoIt(void)
{
// Detach the frames. The objects remain part of the page.
	if (m_pDoc == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pDoc->DocumentRecord()->RemovePage(m_lPage);
	m_pDoc->SetModifiedFlag();
	DetachFrames();
	m_fDetached = TRUE;

	PMGDatabase	*pDatabase;
	if ((pDatabase = m_pDoc->get_database()) != NULL)
	{
		if (ProcessHyperlinks(pDatabase, FALSE))
			m_pDoc->UpdateAllViews(NULL);	
	}
	else
		ASSERT(FALSE);

	return TRUE;
}

BOOL CCmdDeletePage::UndoIt(void)
{
	if (m_pDoc == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pDoc->DocumentRecord()->AddPage(m_lPage, m_dwIndex);
	AttachFrames();
	
	PMGDatabase	*pDatabase = m_pDoc->get_database();
	ASSERT(pDatabase != NULL);

	m_fDetached = FALSE;

	if (pDatabase != NULL)
		if (ProcessHyperlinks(pDatabase, TRUE))
			m_pDoc->UpdateAllViews(NULL);

	return TRUE;
}

int
CCmdDeletePage::ProcessHyperlinks(PMGDatabase *pDatabase, BOOL Enable)
{
	for (int i = 0; i < mHyperlinksToMe.GetSize(); i ++)
	{
		DB_RECORD_NUMBER dbrn = mHyperlinksToMe[i];
		HyperlinkData Data;
		pDatabase->GetHyperlinkData(dbrn, Data);
		
		ASSERT(Data.StringType == TYPE_PageURL);
		ASSERT(Data.PageRecordNumber == m_lPage);
		
		if (Enable == TRUE)
		{
			ASSERT(Data.IsEnabled() == FALSE);
			Data.Enable();
		}
		else
		{
			ASSERT(Data.IsEnabled() == TRUE);
			Data.Disable();
		}

		pDatabase->SetHyperlinkData(dbrn, Data);
	}
	// return the number o links processed
	return i;
}
/////////////////////////////////////////////////////////////////////////////
// CCmdInsertPage

CCmdInsertPage::CCmdInsertPage(WORD wCmdID /*=0*/)
	: CCmdDeletePage(wCmdID)
{
}

BOOL CCmdInsertPage::DoIt(void)
{
	m_pDoc->DocumentRecord()->AddPage(m_lPage, m_dwIndex);
	m_pDoc->SetModifiedFlag();
	AttachFrames();
	m_fDetached = FALSE;

	return TRUE;
}

BOOL CCmdInsertPage::UndoIt(void)
{
// Detach the frames. The objects remain part of the page.

	m_pDoc->DocumentRecord()->RemovePage(m_lPage);
	DetachFrames();
	m_fDetached = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdGuideChange

CCmdGuideChange::CCmdGuideChange(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
   m_pView = NULL;
}

void CCmdGuideChange::Setup(CPmwView* pView, int guideType,
                            int page, int panel,
                            DWORD oldpos, int oldindex,
                            DWORD newpos, int newindex)
{
   m_pView = pView;
   m_guideType = guideType;
   m_nPage = page;
   m_nPanel = panel;
   m_dwOldPos = oldpos;
   m_nOldIndex = oldindex;
   m_dwNewPos = newpos;
   m_nNewIndex = newindex;
}

BOOL CCmdGuideChange::DoIt()
{
   return RestoreState(m_dwNewPos, m_nNewIndex, m_dwOldPos, m_nOldIndex);
}

BOOL CCmdGuideChange::UndoIt()
{
   return RestoreState(m_dwOldPos, m_nOldIndex, m_dwNewPos, m_nNewIndex);
}

BOOL CCmdGuideChange::RestoreState(DWORD oldpos, int oldindex,
                                   DWORD newpos, int newindex)
{
   if (!m_pView)
   {
      ASSERT (m_pView);
      return FALSE;
   }
   CPmwDoc* pDoc = m_pView->GetDocument();
   if (!pDoc)
   {
      ASSERT(pDoc);
      return FALSE;
   }

   // Make sure we're on the right page (and panel)
   SetPage(pDoc);

   // delete new guide, if there was one
   if (newindex != -1)
   {
      m_pView->DeleteGuide(m_guideType, newindex);
   }

   // add back old guide, if there was one
   if (oldindex != -1)
   {
      CPoint pt(0,0);
      Array* pGuideArray;
      if (m_guideType == CPmwView::GUIDE_HORIZ)
      {
         pGuideArray = pDoc->get_horiz_guides();
         pt.y = m_pView->get_rc()->page_y_to_screen(oldpos);
      }
      else
      {
         pGuideArray = pDoc->get_vert_guides();
         pt.x = m_pView->get_rc()->page_x_to_screen(oldpos);
      }

      pGuideArray->insert_element((void*)&oldpos, oldindex);
      m_pView->DrawGuideLine(m_pView->get_rc(), m_guideType, pt, TRUE);
   }

   return TRUE;
}

void CCmdGuideChange::SetPage(CPmwDoc* pDoc)
{
   ASSERT(pDoc);

   // Are we on the right page?
   if (m_nPage != (int)pDoc->CurrentPageIndex())
      pDoc->GotoPage(m_nPage);

   // Are we on the right panel?
   PROJECT_TYPE type = pDoc->get_project_type();
   if ( (type == PROJECT_TYPE_Card ||
         type == PROJECT_TYPE_NoteCard ||
         type == PROJECT_TYPE_HalfCard)
        && (pDoc->get_current_panel() != m_nPanel) )
   {
      ((CCardView*)m_pView)->SetPanel(m_nPanel);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCmdBorderChange

CCmdBorderChange::CCmdBorderChange(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pView = NULL;
	m_pBEObject = NULL;
	m_pBorderObject = NULL;
}

CCmdBorderChange::~CCmdBorderChange()
{
	m_pBEObject->Destroy();
}

void CCmdBorderChange::Setup(CPmwView* pView, BorderObject* pObject, CBEObject* pBEObject)
{
	ASSERT(pView);
	ASSERT(pObject);
	ASSERT(pBEObject);

	m_pView = pView;
	m_pBorderObject = pObject;
	m_pBEObject = pBEObject;
}

BOOL CCmdBorderChange::DoIt()
{
	DoSwap();
	return TRUE;
}

BOOL CCmdBorderChange::UndoIt()
{
	DoSwap();
	return TRUE;
}

void CCmdBorderChange::DoSwap()
{
	// make sure we're on the correct page
	m_pView->ViewObject(m_pBorderObject);

	// save the current BEObject
	CBEObject* curBEObj = m_pBorderObject->GetBEObject();

	m_pBorderObject->RestoreBorder(m_pBEObject);
	m_pBEObject = curBEObj;
}

