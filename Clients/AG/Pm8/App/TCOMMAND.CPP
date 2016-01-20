/*
// $Header: /PM8/App/TCOMMAND.CPP 1     3/03/99 6:12p Gbeddow $
//
// Text command handlers.
//
// $Log: /PM8/App/TCOMMAND.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 4     6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 3     5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 2     3/18/98 11:15a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:26:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:42   Fred
// Initial revision.
// 
//    Rev 1.25   30 Jun 1997 11:55:18   Jay
// New stretching logic.
// 
//    Rev 1.24   27 May 1997 09:42:22   Jay
// Stretch undo with linked frames.
// 
//    Rev 1.23   16 Apr 1997 12:12:28   Jay
// Fixes for text undo
// 
//    Rev 1.22   21 Feb 1997 15:14:34   Jay
// New mode handling; crop mode support
// 
//    Rev 1.21   08 Jan 1997 15:22:18   Fred
// More text box changes
// 
//    Rev 1.20   07 Oct 1996 16:59:34   Jay
// Fixed a redo bug. 
// 
//    Rev 1.19   01 Aug 1996 14:40:20   Jay
// Was accessing m_pDoc when NULL.
// 
//    Rev 1.18   25 Jul 1996 13:14:28   Jay
// Multi-page frame glitch correction.
// 
//    Rev 1.17   25 Jul 1996 11:17:02   Jay
// More page stuff
// 
//    Rev 1.16   22 Jul 1996 18:08:30   Jay
// Added missing release().
// 
//    Rev 1.15   17 Jul 1996 15:43:10   Jay
//  
// 
//    Rev 1.14   15 Jul 1996 11:35:04   Jay
//  
// 
//    Rev 1.13   11 Jul 1996 11:35:46   Jay
//  
// 
//    Rev 1.12   08 Jul 1996 17:53:36   Jay
// Linked frames
// 
//    Rev 1.11   05 Jul 1996 10:47:44   Jay
//  
// 
//    Rev 1.10   03 Jul 1996 14:31:14   Jay
//  
// 
//    Rev 1.9   02 Jul 1996 08:55:32   Jay
//  
// 
//    Rev 1.8   25 Jun 1996 13:13:28   Jay
//  
// 
//    Rev 1.7   21 Jun 1996 14:31:12   Jay
// Stretch text, etc.
// 
//    Rev 1.6   20 Jun 1996 15:00:08   Jay
//  
// 
//    Rev 1.5   19 Jun 1996 17:32:52   Jay
//  
// 
//    Rev 1.4   17 Jun 1996 12:02:54   Jay
//  
// 
//    Rev 1.3   14 Jun 1996 17:06:46   Jay
//  
// 
//    Rev 1.2   05 Jun 1996 08:22:20   Jay
//  
// 
//    Rev 1.1   23 Apr 1996 08:09:10   Jay
// Renamed CTextState
// 
//    Rev 1.0   14 Mar 1996 13:35:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:14   FRED
// Initial revision.
// 
//    Rev 1.10   08 Sep 1995 11:25:28   JAY
// 
//    Rev 1.9   08 Sep 1995 11:05:46   JAY
// Banner resizing fixes.
// 
//    Rev 1.8   07 Sep 1995 15:50:26   JAY
// Now turns the txp back on in all cases.
// 
//    Rev 1.7   24 Aug 1995 14:58:10   JAY
// Fixed (again) the text highlight stuff.
// 
//    Rev 1.6   24 Aug 1995 14:08:56   JAY
// Fixed to text highlight.
// 
//    Rev 1.5   23 Aug 1995 11:19:42   JAY
// No longer turns the txp back on.
// 
//    Rev 1.4   14 Aug 1995 17:19:28   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.3   14 Aug 1995 09:35:06   JAY
// Text undo now handles fill-in fields correctly.
// 
//    Rev 1.2   15 Mar 1995 10:48:40   JAY
// Support for new text editing
// 
//    Rev 1.1   09 Mar 1995 08:29:14   JAY
// More undo.
// 
//    Rev 1.0   17 Feb 1995 16:45:58   JAY
// Initial revision.
// 
//    Rev 1.0   15 Feb 1995 17:17:14   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "utils.h"
#include "tcommand.h"
#include "framerec.h"
#include "macrorec.h"
#include "frameobj.h"
#include "tblobj.h"
#include "clip.h"
#include "textflow.h"
#include "textrec.h"

/////////////////////////////////////////////////////////////////////////////
// Generally useful routines.

/////////////////////////////////////////////////////////////////////////////
// CTextEditState

CTextEditState::CTextEditState()
{
// No record yet.
	m_Record = 0;
	m_TextStart = -1;
	m_TextEnd = -1;
}

CTextEditState::~CTextEditState()
{
// Free the paragraph if we have one.
	if (m_Record != 0)
	{
		PMGDatabase* pDatabase;
		if ((pDatabase = CCommand::GetUndoDatabase()) != NULL)
		{
			pDatabase->delete_record(m_Record, RECORD_TYPE_Text);
		}
		m_Record = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBaseTextCommand

CBaseTextCommand::CBaseTextCommand(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
	m_pView = NULL;
}

BOOL CBaseTextCommand::IsTextEditCommand(void)
{
	return TRUE;
}

/*
// Set the important fields.
*/

void CBaseTextCommand::SetStuff(CPmwView* pView)
{
	m_pView = pView;
	CTxp* pTxp = pView->GetTxp();
	if (pTxp->Valid())
	{
		m_pObject = pTxp->Object();
		m_lIndex = pTxp->Index();
	}
	else
	{
		m_pObject = (CFrameObject*)pView->GetDocument()->selected_objects();
		m_lIndex = -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTextCommand

CTextCommand::CTextCommand(WORD wCmdID /*=0*/)
	: CBaseTextCommand(wCmdID)
{
}

/*
// Before the change.
*/

BOOL CTextCommand::Before(CPmwView* pView)
{
	SetStuff(pView);
	return SaveText(pView->GetTxp(), m_BeforeState);
}

/*
// After the change.
*/

BOOL CTextCommand::After(void)
{
	return SaveText(m_pView->GetTxp(), m_AfterState);
}

/*
// Undo this command.
*/

BOOL CTextCommand::UndoIt(void)
{
	if (RemoveText(m_pView->GetTxp(), m_AfterState)
				&& RestoreText(m_pView->GetTxp(), m_BeforeState, FALSE))
	{
		CPmwDoc* pDoc = m_pView->GetDocument();
		if (pDoc != NULL)
		{
			if ((m_pView->GetTxp()->Flags() & CTxp::FLAG_stretch_frame)
				 || m_pView->GetTxp()->Object()->get_panel() == OBJECT_IN_CELL_PANEL)
			{
				pDoc->resize_project();
			}
			pDoc->ReframeOnTxp();
		}
		return TRUE;
	}
	return FALSE;
}

/*
// Do this command.
*/

BOOL CTextCommand::DoIt(void)
{
	if (RemoveText(m_pView->GetTxp(), m_BeforeState)
				&& RestoreText(m_pView->GetTxp(), m_AfterState, ID() == IDCmd_TypeText))
	{
		CPmwDoc* pDoc = m_pView->GetDocument();
		if (pDoc != NULL)
		{
			if ((m_pView->GetTxp()->Flags() & CTxp::FLAG_stretch_frame)
				 || m_pView->GetTxp()->Object()->get_panel() == OBJECT_IN_CELL_PANEL)
			{
				pDoc->resize_project();
			}
			pDoc->ReframeOnTxp();
		}
		return TRUE;
	}
	return FALSE;
}

/*
// Save the text currently indicated by the passed txp.
*/

BOOL CTextCommand::SaveText(CTxp* pTxp, CTextEditState& State)
{
/* If we have no text to save, just remember where we were. */

	if (!pTxp->SomethingSelected())
	{
		State.m_Record = 0;
		State.m_TextStart = State.m_TextEnd = pTxp->Index();
		return TRUE;
	}

/* We have some text to save. Get the database and save the text. */

	PMGDatabase* pDatabase = GetUndoDatabase();
	if (pDatabase == NULL)
	{
		return FALSE;
	}

	State.m_TextStart = pTxp->SelectStart();
	State.m_TextEnd = pTxp->SelectEnd();
	od("Saved text %d to %d\r\n", State.m_TextStart, State.m_TextEnd);

	return CopyTextToDatabase(pTxp, pDatabase, &State.m_Record, UNDO_Normal) == ERRORCODE_None;
}

BOOL CTextCommand::RestoreText(CTxp* pTxp, CTextEditState& State, BOOL fTyping)
{
	pTxp->SelectText(State.m_TextStart, State.m_TextStart);

/* If we have any text to restore, do it now. */

	if (State.m_Record != 0)
	{
		PMGDatabase* pDatabase = GetUndoDatabase();
		if (pDatabase != NULL)
		{
			// Restore the text in this record.

			/*
			// We check for "typing" here. Normal undos want to restore the
			// style following a final CR since that was the style of the text
			// in the original text. In typing cases, that style is the style
			// of the typed text, not the previous text. This is not what we
			// want. So we turn off that aspect of it here.
			*/

         if (PasteTextFromText(pTxp,
                               pDatabase,
                               State.m_Record, 
									    fTyping
											? UNDO_None
											: UNDO_Normal) == ERRORCODE_None)
			{
				pTxp->SelectText(State.m_TextStart, State.m_TextEnd);
				m_pView->GetDocument()->SetModifiedFlag();
				return TRUE;
			}
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CTextCommand::RemoveText(CTxp* pTxp, CTextEditState& State)
{
	if (State.m_Record != 0)
	{
		pTxp->SelectText(State.m_TextStart, State.m_TextStart);
//		od("Delete %d characters\r\n", State.m_TextEnd - State.m_TextStart);
		pTxp->DeleteText(State.m_TextEnd - State.m_TextStart);

		m_pView->GetDocument()->SetModifiedFlag();
	}
	return TRUE;
}

CTextRecord* CTextCommand::CreateTextRecord(PMGDatabase* pDatabase, CTxp* pTxp)
{
	CTextRecord* pText = (CTextRecord*)pDatabase->new_record(RECORD_TYPE_Text, NULL);
	if (pText != NULL)
	{
		TRY
		{
			CTextParagraph Paragraph;

			Paragraph.m_lBaseParagraphStyle = pDatabase->ImportTextStyleRecord(pTxp->Style().ParagraphStyle(), pTxp->Database());
			Paragraph.m_StyleChange.m_lValue = Paragraph.m_lBaseParagraphStyle;

			pText->InsertParagraph(0, &Paragraph);

			// Get rid of the extra ref we have on the style.
			// InsertParagraph() incremented it for us.
			pDatabase->FreeTextStyleRecord(Paragraph.m_lBaseParagraphStyle);

			pText->SetParagraphStyle(0, &pTxp->Style());
		}
		CATCH_ALL(e)
		{
			pText->zap();
			pText = NULL;
		}
		END_CATCH_ALL
	}
	return pText;
}

CTextRecord* CTextCommand::LockTextRecord(PMGDatabase* pDatabase, DB_RECORD_NUMBER lText)
{
	return (CTextRecord*)pDatabase->get_record(lText, NULL, RECORD_TYPE_Text);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdDeleteText

CCmdDeleteText::CCmdDeleteText(WORD wCmdID /*=0*/)
	: CTextCommand(wCmdID == 0 ? IDCmd_DeleteText : wCmdID)
{
}

BOOL CCmdDeleteText::UpdateDelete(CPmwView* pView, CHARACTER_COUNT lCount)
{
/* Set the Txp now. */

	if (m_pView == NULL)
	{
		m_pView = pView;
	}

/* Txps don't match. This fails automatically. */

	CTxp* pTxp = pView->GetTxp();
	if (m_fClosed || pView != m_pView || !pTxp->SomethingSelected())
	{
		Close();					// Just to be sure.
		return FALSE;
	}

/*
// If the after state is non-empty, then we must match for typing.
*/

	if (m_BeforeState.m_TextStart != -1)
	{
		if (pTxp->SelectStart() != m_BeforeState.m_TextStart)
		{
		/* Unable to extend the deleting. */
			Close();
			return FALSE;
		}
	}

/*
// We can extend the deleting.
*/

	PMGDatabase* pDatabase;

	if ((pDatabase = GetUndoDatabase()) == NULL)
	{
		Close();
		return FALSE;
	}

	CTextRecord* pText;

	if (m_BeforeState.m_Record == 0)
	{
	/* We don't have a paragraph yet. */
		if ((pText = CreateTextRecord(pDatabase, pTxp)) != NULL)
		{
			/* The start is a very important number! */

			m_AfterState.m_TextStart =
				m_AfterState.m_TextEnd = 
				m_BeforeState.m_TextStart =
				m_BeforeState.m_TextEnd = pTxp->SelectStart();
			m_BeforeState.m_Record = pText->Id();
		}
	}
	else
	{
		pText = LockTextRecord(pDatabase, m_BeforeState.m_Record);
	}

	// If we don't have text, we can't do much.

	if (pText == NULL)
	{
		Close();
		return FALSE;
	}

	// Get the source text record.

	CTextRecord* pSrcText;

	if ((pSrcText = LockTextRecord(pTxp->Database(), pTxp->TextRecord())) == NULL)
	{
		pText->release(TRUE);
		Close();
		return FALSE;
	}

/* Copy the character(s) to the text frame. */

	CopyTextToText(pText,
						pText->NumberOfCharacters(),	/* Always at the end. */
						pSrcText,
						pTxp->SelectStart(),
						pTxp->SelectEnd(),
						UNDO_Deleting);

/* Release the text records. */

	pSrcText->release();
	pText->release(TRUE);

	m_BeforeState.m_TextEnd += pTxp->SelectEnd() - pTxp->SelectStart();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdAddText

CCmdAddText::CCmdAddText()
	: CTextCommand(IDCmd_TypeText)
{
}

BOOL CCmdAddText::UpdateTyping(CPmwView* pView, CHARACTER c, CHARACTER_COUNT lCount)
{
/* Txps don't match. This fails automatically. */

	if (m_fClosed || pView != m_pView)
	{
		Close();					// Just to be sure.
		return FALSE;
	}

	CTxp* pTxp = pView->GetTxp();

	CHARACTER_INDEX ciInsert = pTxp->SomethingSelected()
			? pTxp->SelectStart()			// Going to be here after delete.
			: pTxp->Index();

/*
// If the after state is non-empty, then we must match for typing.
*/

	if (m_AfterState.m_TextStart != -1)
	{
		if (pTxp->SomethingSelected()
			 || ciInsert != m_AfterState.m_TextEnd)
		{
		/* Unable to extend the typing. */
			Close();
			return FALSE;
		}
	}

/*
// We can extend the typing.
*/

	PMGDatabase* pDatabase;

	if ((pDatabase = GetUndoDatabase()) == NULL)
	{
		Close();
		return FALSE;
	}

	CTextRecord* pText;

	if (m_AfterState.m_Record == 0)
	{
	/* We don't have a text record yet. */
		if ((pText = CreateTextRecord(pDatabase, pTxp)) != NULL)
		{
			m_AfterState.m_TextStart =
				m_AfterState.m_TextEnd = ciInsert;
			m_AfterState.m_Record = pText->Id();

			pText->SetParagraphStyle(0, &pTxp->Style());
		}
	}
	else
	{
		pText = LockTextRecord(pDatabase, m_AfterState.m_Record);
	}

	// If we don't have a paragraph, we can't do much.
	if (pText == NULL)
	{
		Close();
		return FALSE;
	}

	MacroListRecord* pSrcList = pTxp->Database()->GetMacroList();
	MacroListRecord* pDstList = pDatabase->GetMacroList();

/* Add the character(s) to the text frame. */

	// Convert a macro character to the destination database.
	if (c >= MACRO_CHARACTER && c <= MACRO_CHARACTER_MAX && pSrcList != pDstList)
	{
		if ((c = pDstList->ImportMacro(c - MACRO_CHARACTER, pSrcList)) == 0)
		{
			c = '?';
		}
		else
		{
			c += MACRO_CHARACTER;
		}
	}

	CHARACTER_INDEX lCharacter = m_AfterState.m_TextEnd - m_AfterState.m_TextStart;
	for (CHARACTER_COUNT l = 0; l < lCount; l++)
	{
		pText->InsertText(lCharacter++, &c, 1);
	}
	pText->ChangeRangeStyle(lCharacter-lCount, lCharacter, &pTxp->Style());
	m_AfterState.m_TextEnd += lCount;

	if (pSrcList != NULL)
	{
		pSrcList->release();
	}
	if (pDstList != NULL)
	{
		pDstList->release();
	}

	pText->release(TRUE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdBackspace

CCmdBackspace::CCmdBackspace()
	: CTextCommand(IDCmd_Backspace)
{
}

BOOL CCmdBackspace::UpdateBackspace(CPmwView* pView, CHARACTER_COUNT lCount)
{
/* Set the view now. */

	if (m_pView == NULL)
	{
		m_pView = pView;
	}

/* Txps don't match. This fails automatically. */

	CTxp* pTxp = pView->GetTxp();
	if (m_fClosed || pView != m_pView || !pTxp->SomethingSelected())
	{
		Close();					// Just to be sure.
		return FALSE;
	}

/*
// If the after state is non-empty, then we must match for backspacing.
*/

	if (m_BeforeState.m_TextStart != -1)
	{

		if (pTxp->SelectEnd() != m_BeforeState.m_TextStart)
		{
		/* Unable to extend the backspacing. */
			Close();
			return FALSE;
		}
	}

/*
// We can extend the backspacing.
*/

	PMGDatabase* pDatabase;

	if ((pDatabase = GetUndoDatabase()) == NULL)
	{
		Close();
		return FALSE;
	}

	CTextRecord* pText;

	if (m_BeforeState.m_Record == 0)
	{
	/* We don't have a paragraph yet. */
		if ((pText = CreateTextRecord(pDatabase, pTxp)) != NULL)
		{
			m_BeforeState.m_TextEnd = pTxp->SelectEnd();
			m_BeforeState.m_Record = pText->Id();

			pText->SetParagraphStyle(0, &pTxp->Style());
		}
	}
	else
	{
		pText = LockTextRecord(pDatabase, m_BeforeState.m_Record);
	}

	// If we don't have a paragraph, we can't do much.
	if (pText == NULL)
	{
		Close();
		return FALSE;
	}

	// Get the source paragraph.

	CTextRecord* pSrcText;

	if ((pSrcText = LockTextRecord(pTxp->Database(), pTxp->TextRecord())) == NULL)
	{
		pText->release(TRUE);
		Close();
		return FALSE;
	}

/* Copy the character(s) to the text frame. */

	CopyTextToText(pText,
						0,			/* New text always goes in the front. */
						pSrcText,
						pTxp->SelectStart(),
						pTxp->SelectEnd(),
						UNDO_Normal);

	// Release the text records. */

	pSrcText->release();
	pText->release(TRUE);

/* The start is a very important number! */

	m_AfterState.m_TextStart =
		m_AfterState.m_TextEnd =
		m_BeforeState.m_TextStart = pTxp->SelectStart();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdStretchText

CCmdStretchText::CCmdStretchText()
	: CBaseTextCommand(IDCmd_StretchText)
{
}

CCmdStretchText::~CCmdStretchText()
{
	int n;
	for (n = 0; n < m_BeforeStates.GetSize(); n++)
	{
		delete (CStretchState*)m_BeforeStates.GetAt(n);
	}
	for (n = 0; n < m_AfterStates.GetSize(); n++)
	{
		delete (CStretchState*)m_AfterStates.GetAt(n);
	}
}

/*
// Take a snapshot prior to a stretch change.
*/

BOOL CCmdStretchText::Snapshot(CPmwView* pView)
{
	BOOL fResult = TRUE;

	SetStuff(pView);

	// Run through all the objects in the selected list.

	CPmwDoc* pDoc = pView->GetDocument();
	PMGPageObject* pObject;
	CTxp* pTxp = m_pView->GetTxp();
	if (pTxp->Valid())
	{
		pObject = pTxp->Object();
		ASSERT(pObject->next_selected_object() == NULL);
	}
	else
	{
		pObject = pDoc->selected_objects();
	}
	for ( ; pObject != NULL; pObject = pObject->next_selected_object())
	{
		CFrameObject* pFrameObject;
		if (pObject->type() == OBJECT_TYPE_Frame
				&& (pFrameObject = (CFrameObject*)pObject)->CanStretch())
		{
   		fResult = FALSE;

			CFrameRecord* pFrame = pFrameObject->LockFrameRecord();
			if (pFrame != NULL)
			{
				CTextRecord* pText = pFrame->LockTextRecord();
				if (pText != NULL)
				{
					// Create the states to add.
					CStretchState* pBeforeState = new CStretchState;
					CStretchState* pAfterState = new CStretchState;

					// Setup all the parameters.
					BOOL bStretched = !!(pFrame->FrameFlags() & FRAME_FLAG_stretch_text);

					// Set the before state.
					pBeforeState->m_fStretch = bStretched;
					pBeforeState->m_StretchDims = pFrame->StretchDims();
					pBeforeState->m_lStretchNumerator = pText->StretchNumerator();
					pBeforeState->m_lStretchDenominator = pText->StretchDenominator();
					pBeforeState->m_lStretchExpansion = pText->StretchExpansion();

					// Set the after state.
					pAfterState->m_fStretch = !bStretched;

					if (bStretched)
					{
						// We will be turning off.
						pAfterState->m_StretchDims = pBeforeState->m_StretchDims;

						// These don't really matter since they're not used.
//						pAfterState->m_lStretchNumerator = MakeFixed(1);
//						pAfterState->m_lStretchDenominator = MakeFixed(1);
//						pAfterState->m_lStretchExpansion = MakeFixed(1);
					}
					else
					{
						// We will be turning on.
						// Remember the original stretch dimensions.

						PBOX Bound = pFrame->ObjectBounds();
						pAfterState->m_StretchDims.x = Bound.x1 - Bound.x0;
						pAfterState->m_StretchDims.y = Bound.y1 - Bound.y0;

						pAfterState->m_lStretchNumerator = pBeforeState->m_lStretchNumerator;
						pAfterState->m_lStretchDenominator = pBeforeState->m_lStretchDenominator;
						pAfterState->m_lStretchExpansion = pBeforeState->m_lStretchExpansion;

					}
					// Add this object to the list.
					m_Objects.Add(pObject);

					// Add the states.
					m_BeforeStates.Add(pBeforeState);
					m_AfterStates.Add(pAfterState);
					pText->release();
					fResult = TRUE;
				}
				pFrame->release();
			}
		}
		if (!fResult)
		{
			break;
		}
	}

	return fResult;
}

/*
// Undo this command.
*/

BOOL CCmdStretchText::UndoIt(void)
{
	return SetStretch(m_BeforeStates);
}

/*
// Do this command.
*/

BOOL CCmdStretchText::DoIt(void)
{
	return SetStretch(m_AfterStates);
}

/*
// Set the stretch state of the frame.
*/

BOOL CCmdStretchText::SetStretch(CPtrArray& States)
{
	BOOL fResult = FALSE;

	// Get a pointer to the view txp.
	CTxp* pTxp = m_pView->GetTxp();
	CPmwDoc* pDoc = m_pView->GetDocument();
	pDoc->deselect_all();

	// Handle the txp in the beginning.
	if (pTxp->Valid())
	{
		pTxp->Off();
	}

   BOOL fViewed = FALSE;

	// Run through all the objects.
	for (int n = 0; n < m_Objects.GetSize(); n++)
	{
		// Process the next object.
		CFrameObject* pObject = (CFrameObject*)m_Objects.GetAt(n);
		if (!fViewed)
		{
			// Make sure we are on the right page.
			m_pView->ViewObject(pObject);
         fViewed = TRUE;
		}
		pDoc->select_object(pObject);

		CStretchState* pState = (CStretchState*)States.GetAt(n);
		CFrameRecord* pFrame;
		if ((pFrame = pObject->LockFrameRecord()) != NULL)
		{
			CTextRecord* pText;
			if ((pText = pFrame->LockTextRecord()) != NULL)
			{
				// Compute the new stretching flag.
				DWORD dwFrameFlags = pFrame->FrameFlags();

				if (pState->m_fStretch)
				{
					dwFrameFlags |= FRAME_FLAG_stretch_text;
//					pText->SetStretch(pState->m_lStretchNumerator, pState->m_lStretchDenominator, State.m_lStretchExpansion);
				}
				else
				{
					dwFrameFlags &= ~FRAME_FLAG_stretch_text;
					pText->ClearStretch();
				}

				// Set all the frame attributes.
				pFrame->FrameFlags(dwFrameFlags);
				pFrame->StretchDims(pState->m_StretchDims);

				// Reflow the frame.
				CTextFlow TextFlow(pObject, 0, -1);
				TextFlow.FlowText();

				pText->release();
			}
			pFrame->release();
		}
	}

	// Finish up after if we have a txp.
	if (pTxp->Valid())
	{
		pTxp->SetTransition();
		pTxp->On();

		pDoc->SetModifiedFlag();
		pDoc->ReframeOnTxp();
	}

	return TRUE;
}

#if 0
/////////////////////////////////////////////////////////////////////////////
// CCmdEditText

CCmdEditText::CCmdEditText(WORD wCmdID /*=0*/)
	: CBaseTextCommand(wCmdID)
{
	m_pView = NULL;
	m_pFrame = NULL;
}

BOOL CCmdEditText::Snapshot(CPmwView* pView, CFrameObject* pFrame, PPNT_PTR p)
{
	m_pView = pView;
	m_pFrame = pFrame;

	if (p == NULL)
	{
		m_fPValid = FALSE;	// We don't have a point.
	}
	else
	{
		m_fPValid = TRUE;		// We have a point.
		m_p = *p;				// Save the point.
	}
	return TRUE;
}

BOOL CCmdEditText::StartEditingText(void)
{
	m_pView->edit_frame(m_pFrame,
							  m_fPValid ? &m_p : NULL,
							  FALSE);
	return TRUE;
}

BOOL CCmdEditText::StopEditingText(void)
{
#if 1
	m_pView->set_arrow_tool(TRUE);
#else
	m_pView->stop_editing_frame();
	m_pView->NullEventHandler();
	m_pView->set_arrow_tool();
	m_pView->GetDocument()->update_object_menu();
#endif

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdStartEditText

CCmdStartEditText::CCmdStartEditText()
	: CCmdEditText(IDCmd_StartEditText)
{
}

/*
// Undo this command.
*/

BOOL CCmdStartEditText::UndoIt(void)
{
	return StopEditingText();
}

/*
// Do this command.
*/

BOOL CCmdStartEditText::DoIt(void)
{
	return StartEditingText();
}

/////////////////////////////////////////////////////////////////////////////
// CCmdStopEditText

CCmdStopEditText::CCmdStopEditText()
	: CCmdEditText(IDCmd_StopEditText)
{
}

/*
// Undo this command.
*/

BOOL CCmdStopEditText::UndoIt(void)
{
	return StartEditingText();
}

/*
// Do this command.
*/

BOOL CCmdStopEditText::DoIt(void)
{
	return StopEditingText();
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdFrameAttributes

// Constructor.
CCmdFrameAttributes::CCmdFrameAttributes(WORD wCmdID /*=0*/)
	: CBaseTextCommand(wCmdID)
{
}

// Destructor.
CCmdFrameAttributes::~CCmdFrameAttributes()
{
	FreeFrames();
}

/*
// Before the change.
*/

BOOL CCmdFrameAttributes::Before(CPmwView* pView)
{
	SetStuff(pView);
	return SaveFrames();
}

/*
// After the change.
*/

BOOL CCmdFrameAttributes::After(const CFrameAttributes& AfterAttributes)
{
	m_AfterAttributes = AfterAttributes;
	return TRUE;
}

// Get the frame range.
BOOL CCmdFrameAttributes::GetFrameRange(DB_RECORD_NUMBER& lStartFrame, DB_RECORD_NUMBER& lEndFrame)
{
	lStartFrame = 0;
	lEndFrame = 0;

	// The txp needs to be up to date or we need a selected object.
	CTxp* pTxp = m_pView->GetTxp();
	if (pTxp->Valid())
	{
		pTxp->GetFrameRange(&lStartFrame, &lEndFrame);
	}
	else
	{
		if (m_pObject != NULL && m_pObject->type() == OBJECT_TYPE_Frame)
		{
			lStartFrame = lEndFrame = ((CFrameObject*)m_pObject)->FrameRecord();
		}
		else
		{
			// Not a frame selected!
			ASSERT(FALSE);
		}
	}
	return (lStartFrame != 0) && (lEndFrame != 0);
}

// Save a list of frame attributes.
BOOL CCmdFrameAttributes::SaveFrames(void)
{
	// Get the frame range.
	BOOL fResult = FALSE;
	DB_RECORD_NUMBER lStartFrame, lEndFrame;

	if (GetFrameRange(lStartFrame, lEndFrame))
	{
		m_lStartFrame = lStartFrame;
		m_lEndFrame = lEndFrame;

		fResult = TRUE;
		// Loop on frames until we process the last frame.
		PMGDatabase* pDatabase = m_pView->GetDocument()->get_database();
		DB_RECORD_NUMBER lNextFrame = lStartFrame;
		do
		{
			// Lock the frame.
			// We should never see a 0.

			CFrameRecord* pFrame;
			if ((lStartFrame = lNextFrame) == 0
				 || (pFrame = (CFrameRecord*)pDatabase->get_record(lStartFrame, NULL, RECORD_TYPE_Frame)) == NULL)
			{
				ASSERT(FALSE);
				fResult = FALSE;
				break;
			}

			// Save the attributes of the frame.
			SaveFrame(pFrame);
			// Get the next frame.
			lNextFrame = pFrame->NextFrame();
			// Release the current frame.
			pFrame->release();

		} while (lStartFrame != lEndFrame);
	}
	return fResult;
}

// Save a list of frame attributes.
BOOL CCmdFrameAttributes::RestoreFrames(BOOL fBefore)
{
	// Get the frame range.
	BOOL fResult = TRUE;

	DB_RECORD_NUMBER lStartFrame = m_lStartFrame;
	DB_RECORD_NUMBER lEndFrame = m_lEndFrame;

	// Loop on frames until we process the last frame.
	// We need to do a reflow.

	CTxp* pTxp = m_pView->GetTxp();
	CPmwDoc* pDoc = m_pView->GetDocument();
	BOOL fDoToggle = (pTxp->Object() == m_pObject)
							|| (pDoc->selected_objects() == m_pObject);
	int nVisible = 0;
	if (fDoToggle)
	{
		pDoc->toggle_object(m_pObject);
		if (pTxp->Valid())
		{
			nVisible = pTxp->Visible();
			pTxp->Off(nVisible);
		}
	}

	PMGDatabase* pDatabase = pDoc->get_database();
	DB_RECORD_NUMBER lNextFrame = lStartFrame;
	int nElement = fBefore ? 0 : -1;
	CHARACTER_INDEX lStart = 0, lEnd = -1;
	do
	{
		// Lock the frame.
		// We should never see a 0.
		CFrameRecord* pFrame;
		if ((lStartFrame = lNextFrame) == 0
			 || (pFrame = (CFrameRecord*)pDatabase->get_record(lStartFrame, NULL, RECORD_TYPE_Frame)) == NULL)
		{
			ASSERT(FALSE);
			fResult = FALSE;
			break;
		}

		// Do the attributes thing.
		RestoreFrame(pFrame, nElement);
		// If this is the last frame, get its end position.
		if (lStart == m_lStartFrame)
		{
			lStart = pFrame->FirstCharacter();
		}
		if (lStartFrame == m_lEndFrame)
		{
			lEnd = pFrame->FirstCharacter() + pFrame->NumberOfCharacters();
		}
		// Get the next frame.
		lNextFrame = pFrame->NextFrame();
		// Release the current frame.
		pFrame->release();
		if (fBefore)
		{
			nElement++;
		}
	} while (lStartFrame != lEndFrame);

	CTextFlow TextFlow(m_pObject, lStart, lEnd);
	TextFlow.FlowText();

	if (fDoToggle)
	{
		if (pTxp->Valid())
		{
			pTxp->SetTransition();
			pTxp->On(nVisible);
		}
		pDoc->toggle_object(m_pObject);
	}

	return fResult;
}

// Free a list of frame attributes.
void CCmdFrameAttributes::FreeFrames(void)
{
	int nElements = m_BeforeAttributes.GetSize();
	for (int nElement = 0; nElement < nElements; nElement++)
	{
		delete (CFrameAttributes*)m_BeforeAttributes.GetAt(nElement);
	}
	m_BeforeAttributes.RemoveAll();
}

/*
// Undo this command.
*/

BOOL CCmdFrameAttributes::UndoIt(void)
{
	if (RestoreFrames(TRUE))
	{
#if 0
		if (m_pDoc != NULL && m_pTxp->Flags() & CTxp::FLAG_stretch_frame)
		{
			m_pDoc->resize_project();
		}
#endif
		CPmwDoc* pDoc = m_pView->GetDocument();
		if (pDoc != NULL)
		{
			pDoc->SetModifiedFlag();
			pDoc->ReframeOnTxp();
		}
		return TRUE;
	}
	return FALSE;
}

/*
// Do this command.
*/

BOOL CCmdFrameAttributes::DoIt(void)
{
	if (RestoreFrames(FALSE))
	{
#if 0
		if (m_pDoc != NULL && m_pTxp->Flags() & CTxp::FLAG_stretch_frame)
		{
			m_pDoc->resize_project();
		}
#endif
		CPmwDoc* pDoc = m_pView->GetDocument();
		if (pDoc != NULL)
		{
			pDoc->SetModifiedFlag();
			pDoc->ReframeOnTxp();
		}
		return TRUE;
	}
	return FALSE;
}

void CCmdFrameAttributes::RestoreFrame(CFrameRecord* pFrame, int nIndex)
{
	CPmwDoc* pDoc = m_pView->GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();

	if (nIndex == -1)
	{
		// This is the "after" case.
		pFrame->SetAttributes(m_AfterAttributes);
	}
	else
	{
		pFrame->SetAttributes(*(CFrameAttributes*)m_BeforeAttributes.GetAt(nIndex));
	}

	// Refresh the object if it is on the current page.
	if (pFrame->Page() == pDatabase->CurrentPage())
	{
		CFrameObject* pFrameObject = pDatabase->FindFrameObject(pFrame->Id(), pDoc->object_list());
		if (pFrameObject != NULL)
		{
			pDoc->refresh_object(pFrameObject);
		}
	}
}

void CCmdFrameAttributes::SaveFrame(CFrameRecord* pFrame)
{
	// Allocate the frame attributes.
	CFrameAttributes* pAttributes = new CFrameAttributes;
	// Get the attributes.
	pFrame->GetAttributes(pAttributes);
	// Save the attributes in the list.
	m_BeforeAttributes.Add(pAttributes);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdVAlignText

static WORD IDCmd_VAlign[] =
{
	IDCmd_TopAlign,
	IDCmd_MiddleAlign,
	IDCmd_BottomAlign
};

CCmdVAlignText::CCmdVAlignText(VERT_ALIGN_TYPE Type)
	: CCmdFrameAttributes(IDCmd_VAlign[Type])
{
	m_AfterAttributes.m_fSet = FALSE;
	m_AfterAttributes.m_nVerticalAlignment = Type;
}

CCmdVAlignText::~CCmdVAlignText()
{
	// Remove all the frames while we are still a CCmdVAlignText object.
	FreeFrames();
}

void CCmdVAlignText::FreeFrames(void)
{
	m_BeforeAttributes.RemoveAll();
}

void CCmdVAlignText::SaveFrame(CFrameRecord* pFrame)
{
	int nAlignment = pFrame->VerticalAlignment();
	m_BeforeAttributes.Add((LPVOID)(DWORD)nAlignment);
}

void CCmdVAlignText::RestoreFrame(CFrameRecord* pFrame, int nIndex)
{
	// Get the vertical alignment to set.
	int nAlignment = m_AfterAttributes.m_nVerticalAlignment;
	if (nIndex >= 0)
	{
		nAlignment = (int)(DWORD)m_BeforeAttributes.GetAt(nIndex);
	}
	// Set the vertical alignment.
	pFrame->VerticalAlignment(nAlignment);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdParagraphFormatting

// Constructor.
CCmdParagraphFormatting::CCmdParagraphFormatting(WORD wCmdID /*=0*/)
	: CBaseTextCommand(wCmdID)
{
}

// Destructor.
CCmdParagraphFormatting::~CCmdParagraphFormatting()
{
	FreeParagraphs();
}

/*
// Before the change.
*/

BOOL CCmdParagraphFormatting::Before(CPmwView* pView)
{
	SetStuff(pView);
	CTxp* pTxp = pView->GetTxp();
	pTxp->GetParagraphRange(&m_lStart, &m_lEnd);
	pTxp->GetRange(&m_lTextStart, &m_lTextEnd);

	// Compute the character range that encompasses these paragraphs.
	CTextRecord* pText = NULL;
	CTextIterator Iterator;
	TRY
	{
		// Lock the text record.
		pText = pTxp->LockText();
		Iterator.SetRecord(pText);

		// Handle the start paragraph.
		Iterator.SetPosition(m_lTextStart);
		ASSERT(Iterator.ParagraphIndex() == m_lStart);
		m_lTextStart = Iterator.CharacterIndex() - Iterator.CharacterInParagraph();

		// Handle the end paragraph.
		Iterator.SetPosition(m_lTextEnd);
		ASSERT(Iterator.ParagraphIndex() == m_lEnd);
		CTextParagraph* pParagraph = pText->GetParagraph(Iterator.ParagraphIndex());
		m_lTextEnd = Iterator.CharacterIndex() + (pParagraph->m_lCharacters - Iterator.CharacterInParagraph());
	}
	END_TRY
	if (pText != NULL)
	{
		pText->release();
	}
	return SaveParagraphs();
}

/*
// After the change.
*/

BOOL CCmdParagraphFormatting::After(const CParagraphFormatting& AfterFormatting)
{
	m_AfterFormatting = AfterFormatting;
	return TRUE;
}

// Save a list of paragraph formatting.
BOOL CCmdParagraphFormatting::SaveParagraphs(void)
{
	BOOL fResult = TRUE;

	CTextRecord* pText = NULL;
	TRY
	{
		pText = m_pView->GetTxp()->LockText();
		
		// Loop on paragraphs until we process the last frame.
		for (PARAGRAPH_INDEX lParagraph = m_lStart; lParagraph <= m_lEnd; lParagraph++)
		{
			// Save the formatting of the paragraph.
			SaveParagraph(pText, lParagraph);
		}
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL
	if (pText != NULL)
	{
		pText->release();
	}
	return fResult;
}

// Restore a list of paragraph formatting.
BOOL CCmdParagraphFormatting::RestoreParagraphs(BOOL fBefore)
{
	CTextRecord* pText = NULL;
	BOOL fResult = TRUE;

	TRY
	{
		CPmwDoc* pDoc = m_pView->GetDocument();
		CTxp* pTxp = m_pView->GetTxp();

		pText = pTxp->LockText();

		// Turn the selected frame off.
		if (pDoc != NULL)
		{
			pDoc->toggle_object(pTxp->Object());
		}
		int nVisible = pTxp->Visible();
		pTxp->Off(nVisible);

		int nElement = fBefore ? 0 : -1;

		// Loop on paragraphs until we process the last frame.
		for (PARAGRAPH_INDEX lParagraph = m_lStart; lParagraph <= m_lEnd; lParagraph++)
		{
			// Save the formatting of the paragraph.
			RestoreParagraph(pText, lParagraph, nElement);

			if (fBefore)
			{
				nElement++;
			}
		}

		pTxp->UpdateRange(&m_lTextStart, &m_lTextEnd);
		pTxp->SetTransition();
		pTxp->On(nVisible);

		if (pDoc != NULL)
		{
			pDoc->toggle_object(pTxp->Object());
		}
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL

	if (pText != NULL)
	{
		pText->release();
	}

	return fResult;
}

// Free a list of paragraph formatting.
void CCmdParagraphFormatting::FreeParagraphs(void)
{
	int nElements = m_BeforeFormatting.GetSize();
	for (int nElement = 0; nElement < nElements; nElement++)
	{
		delete (CParagraphFormatting*)m_BeforeFormatting.GetAt(nElement);
	}
	m_BeforeFormatting.RemoveAll();
}

/*
// Undo this command.
*/

BOOL CCmdParagraphFormatting::UndoIt(void)
{
	if (RestoreParagraphs(TRUE))
	{
#if 0
		if (m_pDoc != NULL && m_pTxp->Flags() & CTxp::FLAG_stretch_frame)
		{
			m_pDoc->resize_project();
		}
#endif
		return TRUE;
	}
	return FALSE;
}

/*
// Do this command.
*/

BOOL CCmdParagraphFormatting::DoIt(void)
{
	if (RestoreParagraphs(FALSE))
	{
#if 0
		if (m_pDoc != NULL && m_pTxp->Flags() & CTxp::FLAG_stretch_frame)
		{
			m_pDoc->resize_project();
		}
#endif
		CPmwDoc* pDoc = m_pView->GetDocument();
		if (pDoc != NULL)
		{
			pDoc->SetModifiedFlag();
			pDoc->ReframeOnTxp();
		}
		return TRUE;
	}
	return FALSE;
}

void CCmdParagraphFormatting::RestoreParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph, int nIndex)
{
	CTextStyle Style;

	if (nIndex == -1)
	{
		// This is the "after" case.
		Style.m_Paragraph = m_AfterFormatting;
	}
	else
	{
		Style.m_Paragraph = *(CParagraphFormatting*)m_BeforeFormatting.GetAt(nIndex);
	}
	pText->ChangeParagraphStyle(lParagraph, lParagraph, &Style);
}

void CCmdParagraphFormatting::SaveParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph)
{
	CTextStyle Style;

	pText->GetParagraphStyle(lParagraph, Style);
	// Allocate the paragraph formatting.
	CParagraphFormatting* pFormatting = new CParagraphFormatting;
	// Get the formatting.
	*pFormatting = Style.m_Paragraph;
	// Save the attributes in the list.
	m_BeforeFormatting.Add(pFormatting);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdHAlignText

static WORD IDCmd_HAlign[] =
{
	IDCmd_LeftAlign,
	IDCmd_CenterAlign,
	IDCmd_RightAlign,
	IDCmd_JustifyAlign
};

CCmdHAlignText::CCmdHAlignText(ALIGN_TYPE Type)
	: CCmdParagraphFormatting(IDCmd_HAlign[Type])
{
	m_AfterFormatting.m_nAlignment = Type;
}

CCmdHAlignText::~CCmdHAlignText()
{
	// Remove all the formatting while we are still a CCmdHAlignText object.
	FreeParagraphs();
}

void CCmdHAlignText::FreeParagraphs(void)
{
	m_BeforeFormatting.RemoveAll();
}

void CCmdHAlignText::SaveParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph)
{
	CTextStyle Style;
	pText->GetParagraphStyle(lParagraph, Style);

	m_BeforeFormatting.Add((LPVOID)(DWORD)Style.Alignment());
}

void CCmdHAlignText::RestoreParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph, int nIndex)
{
	// Get the horizontal alignment to set.
	int nAlignment = m_AfterFormatting.m_nAlignment;
	if (nIndex >= 0)
	{
		nAlignment = (int)(DWORD)m_BeforeFormatting.GetAt(nIndex);
	}

	CTextStyle Style;
	Style.Alignment(nAlignment);

	pText->ChangeParagraphStyle(lParagraph, lParagraph, &Style, TOKEN_Alignment);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdTabs

CCmdTabs::CCmdTabs()
	: CCmdParagraphFormatting(IDCmd_Tabs)
{
}

CCmdTabs::~CCmdTabs()
{
	// Remove all the tabs while we are still a CCmdTabs object.
	FreeParagraphs();
}

BOOL CCmdTabs::After(const CTabArray* pTabs)
{
	m_AfterTabs = *pTabs;
	return TRUE;
}

// Free a list of paragraph tabs.
void CCmdTabs::FreeParagraphs(void)
{
	int nElements = m_BeforeFormatting.GetSize();
	for (int nElement = 0; nElement < nElements; nElement++)
	{
		delete (CTabArray*)m_BeforeFormatting.GetAt(nElement);
	}
	m_BeforeFormatting.RemoveAll();
}

void CCmdTabs::SaveParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph)
{
	// Get the paragraph style. This will give us the tabs.
	CTextStyle Style;
	pText->GetParagraphStyle(lParagraph, Style);

	// Create a new tab array to save the current tab state.
	CTabArray* pNewArray = new CTabArray;
	*pNewArray = Style.m_Tabs;
	m_BeforeFormatting.Add(pNewArray);
}

void CCmdTabs::RestoreParagraph(CTextRecord* pText, PARAGRAPH_INDEX lParagraph, int nIndex)
{
	CTabArray* pTabs;
	if (nIndex == -1)
	{
		pTabs = &m_AfterTabs;
	}
	else
	{
		pTabs = (CTabArray*)m_BeforeFormatting.GetAt(nIndex);		
	}

	pText->ChangeParagraphTabs(lParagraph, lParagraph, pTabs, m_lStart != m_lEnd);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdLinkFrames

CCmdLinkFrames::CCmdLinkFrames()
	: CCommand(IDCmd_LinkFrames)
{
	m_pDoc = NULL;

	m_lLinkSource = 0;
	m_lLinkSourceNext = 0;
	m_lLinkTarget = 0;
	m_lLinkTargetPrevious = 0;
}

/*
// Destructor.
*/

CCmdLinkFrames::~CCmdLinkFrames()
{
}

/*
// Take a snapshot of the link scene.
*/

void CCmdLinkFrames::Snapshot(CPmwDoc* pDoc,
										DB_RECORD_NUMBER lLinkSource,
										DB_RECORD_NUMBER lLinkSourceNext,
										DB_RECORD_NUMBER lLinkTarget,
										DB_RECORD_NUMBER lLinkTargetPrevious,
										const CStretchState& SourceStretch,
										const CStretchState& TargetStretch)
{
	m_pDoc = pDoc;
	m_lLinkSource = lLinkSource;
	m_lLinkSourceNext = lLinkSourceNext;
	m_lLinkTarget = lLinkTarget;
	m_lLinkTargetPrevious = lLinkTargetPrevious;
	m_SourceStretch = SourceStretch;
	m_TargetStretch = TargetStretch;
}

/*
// Undo this command.
*/

BOOL CCmdLinkFrames::UndoIt(void)
{
	// The source always gets unlinked.
	m_pDoc->UnlinkFrame(m_lLinkSource);

	// If we have a target and it had a predecessor, link it back.
	// Otherwise, restore its stretch state.
	if (m_lLinkTarget != 0)
	{
		if (m_lLinkTargetPrevious == 0)
		{
			SetStretch(m_lLinkTarget, m_TargetStretch);
		}
		else
		{
			m_pDoc->LinkFrame(m_lLinkTargetPrevious, m_lLinkTarget);
		}

		if (m_lLinkSourceNext == 0)
		{
			SetStretch(m_lLinkSource, m_SourceStretch);
		}
	}

	// If the source had a next frame, link it back.
	if (m_lLinkSourceNext != 0)
	{
		m_pDoc->LinkFrame(m_lLinkSource, m_lLinkSourceNext);
	}

	m_pDoc->SetModifiedFlag();
	return TRUE;
}

/*
// Do this command.
*/

BOOL CCmdLinkFrames::DoIt(void)
{
	// The source always gets unlinked.
	m_pDoc->UnlinkFrame(m_lLinkSource);

	// If we have a target, link to it.
	if (m_lLinkTarget != 0)
	{
		// Turn off stretching.
		CStretchState State;
		State.m_fStretch = FALSE;

		SetStretch(m_lLinkSource, State);
		SetStretch(m_lLinkTarget, State);

		// Link the frames.
		m_pDoc->LinkFrame(m_lLinkSource, m_lLinkTarget);
	}


	m_pDoc->SetModifiedFlag();
	return TRUE;
}

/*
// Set the stretch state of the frame.
*/

BOOL CCmdLinkFrames::SetStretch(DB_RECORD_NUMBER lFrame, CStretchState& State)
{
	PMGDatabase* pDatabase = m_pDoc->get_database();
	BOOL fResult = FALSE;
	CFrameRecord* pFrame = NULL;
	CTextRecord* pText = NULL;

	if ((pFrame = (CFrameRecord*)pDatabase->get_record(lFrame, NULL, RECORD_TYPE_Frame)) != NULL)
	{
		pText = pFrame->LockTextRecord();
		if (pText != NULL)
		{
			// Compute the new stretching flag.
			DWORD dwFrameFlags = pFrame->FrameFlags();

			if (State.m_fStretch)
			{
				dwFrameFlags |= FRAME_FLAG_stretch_text;
//				pText->SetStretch(State.m_lStretchNumerator, State.m_lStretchDenominator, State.m_lStretchExpansion);
				pFrame->StretchDims(State.m_StretchDims);
			}
			else
			{
				dwFrameFlags &= ~FRAME_FLAG_stretch_text;
				pText->ClearStretch();
			}

			// Set all the frame attributes.
			pFrame->FrameFlags(dwFrameFlags);

			// Reflow the frame.
			CTextFlow TextFlow(lFrame, pDatabase, 0, -1);
			TextFlow.FlowText();
		}
		fResult = TRUE;
	}

	if (pText != NULL)
	{
		pText->release();
	}
	if (pFrame != NULL)
	{
		pFrame->release();
	}

	return fResult;
}

