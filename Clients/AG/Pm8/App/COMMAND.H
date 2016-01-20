/*
// General command support.
//
// $Workfile: COMMAND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/COMMAND.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/29/98 1:06p Jayn
// Some changes to keep the code from using a deleted path manager.
// 
//    Rev 1.0   14 Aug 1997 15:19:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:12   Fred
// Initial revision.
// 
//    Rev 1.1   16 Apr 1997 12:12:32   Jay
// Fixes for text undo
// 
//    Rev 1.0   14 Mar 1996 13:26:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:28   FRED
// Initial revision.
// 
//    Rev 1.3   15 Mar 1995 10:48:16   JAY
// Support for new text editing
// 
//    Rev 1.2   09 Mar 1995 08:36:50   JAY
// More undo.
// 
//    Rev 1.1   17 Feb 1995 16:47:22   JAY
// Text editing undo and redo.
// 
//    Rev 1.0   15 Feb 1995 17:17:06   JAY
// Initial revision.
*/

class CCommand;
class CCommandNotify;
class CCommandList;

#ifndef __COMMAND_H__
#define __COMMAND_H__

/*
// A command.
// This is made useful through derivation.
*/

class CCommand
{
public:
	CCommand(WORD wCmdID = 0);
	virtual ~CCommand();

	// Fill in the passed CString with the name of this command.
	virtual void Name(CString& csName) const;
	// Do this command.
	virtual BOOL DoIt(void);
	// Undo this command.
	virtual BOOL UndoIt(void);

	WORD ID(void) const
		{ return m_wCmdID; }

	void Close(void)
		{ m_fClosed = TRUE; }

	BOOL Closed(void) const
		{ return m_fClosed; }

	// Hopefully, this is the extent of the text edit corruption.
	virtual BOOL IsTextEditCommand(void);

/*
// Database support.
*/
	static char m_cbUndoTemplate[5+1];
	static PMGDatabase* GetUndoDatabase(void);

protected:
	WORD m_wCmdID;
	BOOL m_fClosed;

	friend class CPmwApp;			// So it can free the mapping.
	static DatabaseMapping m_Mapping;
};

/*
// A notification function.
// This function is called whenever an undo or redo happens.
*/

class CCommandNotify
{
public:
	enum
	{
		NOTIFY_Undo,
		NOTIFY_Redo,
	};

	CCommandNotify() {}
	virtual ~CCommandNotify() {}

	// The reason this exists!
	// If this returns FALSE, then the one notified does not want this
	// command executed.
	virtual BOOL Notify(CCommand* pCommand, int nNotifyCode) = 0;
};

/*
// Our command queue.
*/

class CCommandList : private CPtrList
{
public:
	CCommandList();
	virtual ~CCommandList();

	BOOL CanUndo(void) const;
	void UndoName(CString& csName) const;
	CCommand* UndoCommand(void) const;			// Current pending
	BOOL Undo(void);

	BOOL CanRedo(void) const;
	void RedoName(CString& csName) const;
	CCommand* RedoCommand(void) const;			// Current pending
	BOOL Redo(void);

	void AddCommand(CCommand* pCommand);

	void ClearPastCurrent(void);
	void ClearAll(void);

	// Set the notify object.
	void SetNotify(CCommandNotify* pNotify)
		{ m_pNotify = pNotify; }

	/*
	// Set the undo buffer size.
	//
	// -1 means unlimited undos.
	// 0 means no undos allowed.
	// n > 0 means a limit of n undos at any one time (with the oldest
	//   undo being lost as new ones come in at the limit).
	*/

	static void SetUndoLimit(int nLimit);

	/*
	// Get the undo buffer size.
	*/

	static int GetUndoLimit(void);

protected:
	CCommand* GetCommand(POSITION pos) const;
	CCommand* GetCommand(int nIndex) const;

	void CommandName(POSITION pos, CString& csName) const;
	void CommandName(int nIndex, CString& csName) const;

	BOOL DoNotify(CCommand* pCommand, int nNotifyCode);

	// nCurrent points to where the next command will be inserted (or
	// to which command Redo() will act upon next). Commands which Undo can
	// operate on are before nCurrent in the list.

	int m_nCurrent;

	// The notify object.

	CCommandNotify* m_pNotify;

	// The undo limit.
	// -1 means unlimited undos.
	// 0 means no undos allowed.
	// n > 0 implies a limit of n undos at any one time (with the oldest
	//   undo being lost as new ones come in at the the limit).

	static int m_nLimit;
};

#endif					// #ifndef __COMMAND_H__
