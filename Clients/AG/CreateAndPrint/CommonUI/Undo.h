#pragma once

#include "AGSym.h"
#include <deque>

#define UNDO_COMMAND -1
#define MAX_UNDO_ITEMS 100

class CUndoItem
{
public:
	CUndoItem()
	{
		m_nUndoCommand = UNDO_COMMAND;
		m_pSym = NULL;
		m_pSymCopy = NULL;
		m_lValue = 0;
		m_lValue2 = 0;
		::SetRectEmpty(&m_Rect);
	}
	inline CAGSym* GetUndoItem() 
		{ return m_pSym; }
	inline void SetUndoItem(CAGSym* pSym)
		{ m_pSym = pSym; }
	inline UINT GetUndoCommand()
		{ return m_nUndoCommand; }
	inline void SetUndoCommand(UINT nCommand)
		{ m_nUndoCommand = nCommand; }
	inline CAGMatrix GetItemMatrix()
		{ return m_Matrix; }
	inline void SetItemMatrix(CAGMatrix Matrix)
		{ m_Matrix = Matrix; }
	inline void SetItemRect(RECT& Rect)
		{ m_Rect = Rect; }
	inline RECT GetItemRect()
		{ return m_Rect; }
	inline void SetItemValue(long lValue)
		{ m_lValue = lValue; }
	inline long GetItemValue()
		{ return m_lValue; }
	inline void SetItemValue2(long lValue2)
		{ m_lValue2 = lValue2; }
	inline int  GetItemValue2()
		{ return m_lValue2; }
	inline void SetUndoCopy(CAGSym* pSym)
		{ m_pSymCopy = pSym; }
	inline CAGSym* GetUndoCopy()
		{ return m_pSymCopy; }

private:
	UINT m_nUndoCommand;
	CAGSym* m_pSym;
	CAGSym* m_pSymCopy;
	long m_lValue;
	long m_lValue2;
	CAGMatrix m_Matrix;
	RECT m_Rect;
};

typedef std::deque <CUndoItem*> CUndoList;

class CDocWindow;

class CUndo  
{
public:
	CUndo(CDocWindow* pDocWindow);
	virtual ~CUndo();
	bool CanUndo();
	void OnEditUndo();
	void OnEditRedo();
	void CheckPoint(UINT nCommand, CAGSym* pSym, long lValue = NULL, long lValue2 = NULL);
	void CheckPoint(UINT nCommand, CAGSym* pSym, CAGMatrix Matrix);
	void CheckPoint(UINT nCommand, CAGSym* pSym, RECT DestRect);
	void CheckPointText(UINT nCommand, CAGSymText* pSym);
	void DereferenceItem(CAGSym* pSym);
    void Cleanup();

private:
    void ClearList(CUndoList& List, bool fClearAllItems);
	CUndoItem* GetListItem(CUndoList& List);
	void ClearPendingTextEdit();
	void AddUndoItem(CUndoItem* pUndoItem);
	void TransferItem(CUndoList& ListSrc, CUndoList& ListDst);
	CUndoItem* RemoveItem(CUndoList& List, bool fClearAllItems);

private:
	CDocWindow* m_pDocWindow;
	CUndoList m_dqUndoItemList;
	CUndoList m_dqRedoItemList;
	CAGSymText* m_pSymTextCopy;
	CAGSymText* m_pSymTextEdit;
};
