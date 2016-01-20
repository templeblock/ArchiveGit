/*
// $Header: /PM8/App/BORDER/command.cpp 1     3/03/99 6:03p Gbeddow $
//
// General command handler.
//
// $Log: /PM8/App/BORDER/command.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     4/15/98 6:42p Fredf
// Replaced color dialog with color palette.
// Fixed another release build bug.
// 
// 1     2/13/98 11:19a Rlovejoy
// Initial creation.
// 
//    Rev 1.0   14 Aug 1997 15:19:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:44   Fred
// Initial revision.
// 
//    Rev 1.1   16 Apr 1997 12:12:20   Jay
// Fixes for text undo
// 
//    Rev 1.0   14 Mar 1996 13:35:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:36   FRED
// Initial revision.
// 
//    Rev 1.6   24 Jan 1996 10:03:22   JAY
// New string management in Configuration
// 
//    Rev 1.5   14 Aug 1995 17:19:26   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.4   25 Apr 1995 09:00:06   JAY
// DatabaseMapping now can create mandatory records.
// 
//    Rev 1.3   15 Mar 1995 10:48:00   JAY
// Support for new text editing
// 
//    Rev 1.2   09 Mar 1995 08:29:30   JAY
// More undo.
// 
//    Rev 1.1   17 Feb 1995 16:48:16   JAY
// Text editing undo and redo.
// 
//    Rev 1.0   15 Feb 1995 17:17:06   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "command.h"

/////////////////////////////////////////////////////////////////////////////
// CCommand

CCommand::CCommand(WORD wCmdID /*=0*/)
{
	m_wCmdID = wCmdID;
	m_fClosed = FALSE;
}

CCommand::~CCommand()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCommandList

int CCommandList::m_nLimit = -1;

/*
// Constructor for the command list.
*/

CCommandList::CCommandList()
{
	m_nCurrent = 0;
	m_pNotify = NULL;
}

/*
// Destructor for the command list.
*/

CCommandList::~CCommandList()
{
	ClearAll();
}

/*
// Return whether there is a command to undo.
*/

BOOL CCommandList::CanUndo(void) const
{
	return m_nCurrent != 0;
}

/*
// Return the name for the current redo command.
*/

void CCommandList::UndoName(CString& csName) const
{
	if (CanUndo())
	{
		CommandName(m_nCurrent-1, csName);
	}
}

/*
// Return the current undo command.
*/

CCommand* CCommandList::UndoCommand(void) const
{
	if (CanUndo())
	{
		return GetCommand(m_nCurrent-1);
	}
	else
	{
		return NULL;
	}
}

/*
// Undo the last command.
*/

BOOL CCommandList::Undo(void)
{
	BOOL fRet = FALSE;
	if (CanUndo())
	{
		CCommand* pCommand;

/////
		CString csUndoName;
		UndoName(csUndoName);
		TRACE("Undo command #%d (%s)\n", m_nCurrent, (LPCSTR)csUndoName);
/////

		if ((pCommand = GetCommand(m_nCurrent-1)) != NULL)
		{
			if (DoNotify(pCommand, CCommandNotify::NOTIFY_Undo))
			{
				if (pCommand->UndoIt())
				{
					m_nCurrent--;
					fRet = TRUE;
				}
			}
		}
	}
	return fRet;
}

/*
// Return whether there is a command to redo.
*/

BOOL CCommandList::CanRedo(void) const
{
	return m_nCurrent != GetCount();
}

/*
// Return the name for the current redo command.
*/

void CCommandList::RedoName(CString& csName) const
{
	if (CanRedo())
	{
		CommandName(m_nCurrent, csName);
	}
}

/*
// Return the current redo command.
*/

CCommand* CCommandList::RedoCommand(void) const
{
	if (CanRedo())
	{
		return GetCommand(m_nCurrent);
	}
	else
	{
		return NULL;
	}
}

/*
// Redo the current command.
*/

BOOL CCommandList::Redo(void)
{
	BOOL fRet = FALSE;
	if (CanRedo())
	{
		CCommand* pCommand;

		if ((pCommand = GetCommand(m_nCurrent)) != NULL)
		{
			if (DoNotify(pCommand, CCommandNotify::NOTIFY_Redo))
			{
				if (pCommand->DoIt())
				{
					m_nCurrent++;
					fRet = TRUE;
				}
			}
		}
	}
	return fRet;
}

/*
// Add a command to the list.
// The command becomes the property of the list.
// It is assumed that the command has been executed, so it is considered
// undoable right away.
*/

void CCommandList::AddCommand(CCommand* pCommand)
{
	ClearPastCurrent();

	if (m_nLimit == 0)
	{
	/* Can't add it. Make sure we're empty. */
		ClearAll();
		delete pCommand;			// We were handed responsibility for this.
		return;
	}

	if (m_nLimit > 0)
	{
		while (GetCount() >= m_nLimit)
		{
			delete (CCommand*)RemoveHead();
			m_nCurrent--;
		}
	}

	if (GetCount() != m_nCurrent)
	{
	}

	if (GetCount() != 0)
	{
	/* Make sure this guy is closed. */
		((CCommand*)GetTail())->Close();
	}

	AddTail(pCommand);
	m_nCurrent++;				// Advance past this. It is now undoable.
}

/*
// Clear all commands including and after m_nCurrent.
*/

void CCommandList::ClearPastCurrent(void)
{
	POSITION pos = FindIndex(m_nCurrent);

	while (pos != NULL)
	{
		POSITION oldpos = pos;
		CCommand* pCommand = (CCommand*)GetNext(pos);

		RemoveAt(oldpos);
		delete pCommand;
	}
}

/*
// Make the current list empty.
*/

void CCommandList::ClearAll(void)
{
	m_nCurrent = 0;
	ClearPastCurrent();
}

/*
// Get the name for a command (by position).
*/

void CCommandList::CommandName(POSITION pos, CString& csName) const
{
	csName.Empty();

	if (pos != NULL)
	{
		CCommand* pCommand = GetCommand(pos);

		if (pCommand != NULL)
		{
			//pCommand->Name(csName);
		}
	}
}

/*
// Get the name for a command (by index).
*/

void CCommandList::CommandName(int nIndex, CString& csName) const
{
	CommandName(FindIndex(nIndex), csName);
}

/*
// Get a command (by position).
*/

CCommand* CCommandList::GetCommand(POSITION pos) const
{
	return (CCommand*)GetAt(pos);
}

/*
// Get a command (by index).
*/

CCommand* CCommandList::GetCommand(int nIndex) const
{
	return GetCommand(FindIndex(nIndex));
}

/*
// Set the undo limit.
*/

void CCommandList::SetUndoLimit(int nLimit)
{
	m_nLimit = nLimit;
}

int CCommandList::GetUndoLimit(void)
{
	return m_nLimit;
}

/*
// Notify that we are executing a command.
*/

BOOL CCommandList::DoNotify(CCommand* pCommand, int nNotifyCode)
{
	if (m_pNotify != NULL)
	{
		return m_pNotify->Notify(pCommand, nNotifyCode);
	}
	return TRUE;
}
