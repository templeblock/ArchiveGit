/*
// $Header: /PM8/App/MODELINK.CPP 1     3/03/99 6:08p Gbeddow $
//
// Mode link routines.
//
// $Log: /PM8/App/MODELINK.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:14   Fred
// Initial revision.
// 
//    Rev 1.8   27 May 1997 09:42:24   Jay
// Stretch undo with linked frames.
// 
//    Rev 1.7   23 May 1997 16:15:32   Jay
// Turns off stretching during link.
// 
//    Rev 1.6   21 Feb 1997 15:09:12   Jay
// New mode handling; crop mode support
// 
//    Rev 1.5   08 Jan 1997 15:21:22   Fred
// More text box changes
// 
//    Rev 1.4   16 Aug 1996 12:25:44   Jay
// 
//    Rev 1.3   08 Aug 1996 13:39:10   Jay
// Frame link tweaks.
// 
//    Rev 1.2   15 Jul 1996 11:35:02   Jay
//  
// 
//    Rev 1.1   12 Jul 1996 16:48:56   Jay
// New link cursors.
// 
//    Rev 1.0   08 Jul 1996 17:53:30   Jay
// Linked frames
*/

#include "system.h"
#include "stdafx.h"
#include "pmw.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "frameobj.h"
#include "framerec.h"
#include "textrec.h"
#include "utils.h"
#include "textflow.h"

#include "tcommand.h"

void CPmwView::IllegalCursor(void)
{
#ifdef WIN32
	set_cursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
#else
	set_cursor(AfxGetApp()->LoadCursor(IDC_ILLEGAL_CURSOR));
#endif
}

/*
// Initialization state.
*/

BOOL CPmwView::link_state_init(EVENT_INFO *info)
{
	BOOL fResult = FALSE;
	PMGPageObject* pObject = GetDocument()->selected_objects();
	if (pObject != NULL && pObject->type() == OBJECT_TYPE_Frame)
	{
		// We need to save the source frame's record number.
		// We can't store the object itself because the user may change pages
		// during the link, in which case the object would go away.
		CFrameObject* pFrameObject = (CFrameObject*)pObject;
		CFrameRecord* pFrame = pFrameObject->LockFrameRecord();
		if (pFrame != NULL)
		{
			m_lLinkSource = pFrame->Id();
			m_lLinkSourceText = pFrame->TextRecord();
			m_lOldLink = pFrame->NextFrame();

			// Say goodbye to the frame.
			pFrame->release(TRUE);

			GetDocument()->UnlinkFrame(m_lLinkSource);

			// Flow the new text.
			CTextFlow TextFlow(pFrameObject, 0, -1);
			TextFlow.FlowText();

			// Default to the "bad" cursor since that is most likely the start state.
//			set_cursor(AfxGetApp()->LoadCursor(IDC_UNLINK));
			set_cursor(AfxGetApp()->LoadCursor(IDC_CANT_LINK));

			// Switch to normal state.
			edit_state = STATE_NORMAL;

			return link_state_normal(info);				/* Do it now, too. */
		}
	}

	// Something failed. Set back the arrow tool and return FALSE.
	set_arrow_tool();
	return FALSE;
}

static BOOL LinkStretchState(DB_RECORD_NUMBER lFrame, PMGDatabase* pDatabase, CStretchState& State)
{
	BOOL fSuccess = FALSE;
	CFrameRecord* pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame);
	if (pFrame != NULL)
	{
		State.m_fStretch = !!(pFrame->FrameFlags() & FRAME_FLAG_stretch_text);
		if (State.m_fStretch)
		{
			// Get the rest of the stretch parameters.
			CTextRecord* pText = pFrame->LockTextRecord();
			if (pText != NULL)
			{
				State.m_StretchDims = pFrame->StretchDims();
				State.m_lStretchNumerator = pText->StretchNumerator();
				State.m_lStretchDenominator = pText->StretchDenominator();
				State.m_lStretchExpansion = pText->StretchExpansion();

				// Turn the stretching off.
				pFrame->FrameFlags(pFrame->FrameFlags() & ~FRAME_FLAG_stretch_text);
				pText->ClearStretch();

				pText->release();

				fSuccess = TRUE;
			}
		}
		else
		{
			fSuccess = TRUE;
		}
		pFrame->release();
	}
	return fSuccess;
}

/*
// Normal state.
*/

BOOL CPmwView::link_state_normal(EVENT_INFO *info)
{
	// Get the document for our further use.
	CPmwDoc *pDocument = GetDocument();

	EVENT Event = info->event;

	if ((info->flags & MK_LBUTTON)
			&& Event != EVENT_BUTTON_RISING
			&& Event != EVENT_BUTTON_DBLCLK)
	{
	/* Button is down from some other click. Ignore this. */
		edit_state = STATE_RELEASE;
		return TRUE;
	}

	if (Event == EVENT_KEY_FALLING)
	{
	/* Ignore these. */
		return -1;
	}

	// Escape here means back to arrow tool.
	if (check_escape_event(info))
	{
		AbortLink();
		return TRUE;
	}

	BOOL fButtonRising = FALSE;
	if (Event == EVENT_BUTTON_RISING)
	{
		SetFocus();
		fButtonRising = TRUE;
	}

	// Compute what object we are currently over.
	PPNT p;
	rc.screen_to_ppnt(info->current_xy, &p);
	PBOX pbox;
	rc.mouse_box(info->current_xy, &pbox);

	/*
	// See if we're on any objects.
	*/

	PMGPageObject* pOnObject = NULL;
	for (PMGPageObject* pObject = pDocument->objects();
								pObject != NULL;
								pObject = (PMGPageObjectPtr)pObject->next_object())
	{
		PBOX bound = pObject->get_bound();
		PBOX refresh_bound;
		pObject->get_refresh_bound(&refresh_bound, &rc);
		UnionBox(&refresh_bound, &refresh_bound, &bound);

		if (IsBoxOnBox(&pbox, &refresh_bound))
		{
			PMGPageObject* pSubObject;

			if (pObject->pt_in_object(p, &rc, &pSubObject)
						&& !(pSubObject->get_flags() & OBJECT_FLAG_locked))
			{
				pOnObject = pSubObject;
			}
		}
	}

	// Can we link to this object?
	int nLinkStatus = GetLinkStatus(pOnObject, fButtonRising);

	if (nLinkStatus != LINK_STATUS_NotHere && fButtonRising)
	{
		DB_RECORD_NUMBER lTarget = 0;
		DB_RECORD_NUMBER lTargetPrevious = 0;

		CStretchState SourceState;
		CStretchState TargetState;

		if (nLinkStatus == LINK_STATUS_Link)
		{
			if (pOnObject != NULL)
			{
				// Button is pressed. Do the link!
				lTarget = ((CFrameObject*)pOnObject)->FrameRecord();

				// We need to determine if the frames are stretched.
				PMGDatabase* pDatabase = pDocument->get_database();
				LinkStretchState(m_lLinkSource, pDatabase, SourceState);
				LinkStretchState(lTarget, pDatabase, TargetState);

				pDocument->LinkFrame(m_lLinkSource, lTarget, &lTargetPrevious);
			}
			else
			{
				// Link to NULL object!
				ASSERT(FALSE);
			}
		}

		// Create the undo command.
		CCmdLinkFrames* pCommand = new CCmdLinkFrames;
		pCommand->Snapshot(pDocument, m_lLinkSource, m_lOldLink, lTarget, lTargetPrevious, SourceState, TargetState);

		// Add the undo command (it has already been executed).
		pDocument->AddCommand(pCommand, FALSE);

		// Back to edit mode.
		set_arrow_tool();
		return TRUE;
	}

	// Set the cursor depending on whether we can link or not.
	if (nLinkStatus == LINK_STATUS_Link)
	{
		set_cursor(AfxGetApp()->LoadCursor(IDC_CAN_LINK));
	}
	else if (nLinkStatus == LINK_STATUS_Unlink)
	{
//		set_cursor(AfxGetApp()->LoadCursor(IDC_UNLINK));
		set_cursor(AfxGetApp()->LoadCursor(IDC_CANT_LINK));
	}
	else
	{
		ASSERT(nLinkStatus == LINK_STATUS_NotHere);
		set_cursor(AfxGetApp()->LoadCursor(IDC_CANT_LINK));
	}
	return TRUE;
}

/*
// Can we link to the passed object?
*/

int CPmwView::GetLinkStatus(PMGPageObject* pObject, BOOL fComplain /*=FALSE*/)
{
	CFrameRecord* pFrame = NULL;
	int nResult = LINK_STATUS_NotHere;

	TRY
	{
		if (pObject == NULL)
		{
			// No object there. This is an unlink.
			nResult = LINK_STATUS_Unlink;
		}
		else if (pObject->type() != OBJECT_TYPE_Frame)
		{
			if (fComplain)
			{
				// Tell the user he needs a frame to link to!
				AfxMessageBox(IDS_ErrNotAFrame);
			}
		}
		else if (((CFrameObject*)pObject)->FrameRecord() == m_lLinkSource)
		{
			// This is an unlink!
			nResult = LINK_STATUS_Unlink;
#if 0
			if (fComplain)
			{
				// Tell the user he cannot link a frame to itself.
				AfxMessageBox(IDS_ErrSameFrame);
			}
#endif
		}
		else if (((CFrameObject*)pObject)->TextRecord() == m_lLinkSourceText)
		{
			if (fComplain)
			{
				// Tell the user he cannot link a frame back into the same chain.
				// (This makes a loop.)
				AfxMessageBox(IDS_ErrSameText);
			}
		}
		else
		{
			if ((pFrame = ((CFrameObject*)pObject)->LockFrameRecord()) != NULL)
			{
				if (pFrame->NumberOfCharacters() == 0)
				{
					nResult = LINK_STATUS_Link;
				}
				else
				{
					// Frame is not empty.
					if (fComplain)
					{
						// Tell the user the frame must be empty.
						AfxMessageBox(IDS_ErrFrameNotEmpty);
					}
				}
			}
		}
	}
	END_TRY

	if (pFrame != NULL)
	{
		pFrame->release();
	}
	return nResult;
}

/*
// Release state.
// Wait for the user to release the mouse button.
*/

BOOL CPmwView::link_state_release(EVENT_INFO *info)
{
	if (!(info->flags & MK_LBUTTON))
	{
	/* Done waiting! */
		edit_state = STATE_NORMAL;
	}
	return TRUE;
}

/*
// The states for the link mode.
*/

CPmwView::STATE_HANDLER CPmwView::link_state[] =
{
	CPmwView::link_state_init,
	CPmwView::link_state_normal,
	CPmwView::link_state_release
};

#define	LINK_STATE_COUNT			(sizeof(link_state)/sizeof(link_state[0]))

/*
// The link mode handler.
*/

void CPmwView::mode_link(EVENT_INFO *info)
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
		if (state >= LINK_STATE_BASE)
		{
		/* One of our special states! */
			state -= (LINK_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			state = LINK_STATE_COUNT;
		}
	}

	if (state < LINK_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*link_state[state])(info);
	}
}

void CPmwView::AbortLink(void)
{
	GetDocument()->LinkFrame(m_lLinkSource, m_lOldLink);
	set_arrow_tool();
}
