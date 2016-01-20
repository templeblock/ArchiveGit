// Undo.cpp: implementation of the CUndo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Undo.h"
#include "AGLayer.h"
#include "DocWindow.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CUndo::CUndo(CDocWindow* pDocWindow)
{
	m_pDocWindow = pDocWindow;
	m_dqUndoItemList.clear();
	m_dqRedoItemList.clear();
	m_pSymTextCopy = NULL;
	m_pSymTextEdit = NULL;
}

//////////////////////////////////////////////////////////////////////
CUndo::~CUndo()
{
	ClearPendingTextEdit();
	ClearList(m_dqRedoItemList, true/*fClearAllItems*/);
	ClearList(m_dqUndoItemList, true/*fClearAllItems*/);
}

//////////////////////////////////////////////////////////////////////
bool CUndo::CanUndo()
{
	return (m_dqUndoItemList.size() > 0);
}

//////////////////////////////////////////////////////////////////////
void CUndo::Cleanup()
{
    ClearPendingTextEdit();
	ClearList(m_dqRedoItemList, true/*fClearAllItems*/);
	ClearList(m_dqUndoItemList, true/*fClearAllItems*/);
}

//////////////////////////////////////////////////////////////////////
void CUndo::OnEditUndo()
{
	if (m_pSymTextEdit && m_pSymTextEdit->IsDirty()) // if editing text, and edits have been made...
		CheckPointText(NULL, m_pSymTextEdit);

	// Unselect the current symbol
	// This must be done before working with the undo list below
	m_pDocWindow->SymbolUnselect(true/*bClearPointer*/);

	CUndoItem* pItem = GetListItem(m_dqUndoItemList);
	if (!pItem)
		return;

	CAGSym* pSym = pItem->GetUndoItem();
	if (pSym || pItem->GetUndoCommand() == IDC_DOC_PAGENAV)
	{
		switch (pItem->GetUndoCommand())
		{
			case IDC_DOC_ADDIMAGE:
			case IDC_DOC_ADDAPHOTO:
			case IDC_DOC_ADDTEXTBOX:
			case IDC_DOC_ADDSHAPE:
			case IDC_DOC_ADDCALENDAR:
			case IDC_DOC_PASTE:
			{
				m_pDocWindow->UndoAdd(pSym);
				break;
			}
			case IDC_DOC_CUT:
			case IDC_DOC_DELETE:
			{
				m_pDocWindow->UndoDelete(pSym);
				break;
			}
			case IDC_CONVERT_TO_JPG:
			{
				CAGSym* pSymCopy = (CAGSym*)pItem->GetItemValue();
				m_pDocWindow->UndoAdd(pSymCopy);
				m_pDocWindow->UndoDelete(pSym);
				break;
			}
			case IDC_DOC_MIRROR:
			{
				m_pDocWindow->UndoMirrorFlip(pSym, true/*bFlipX*/, false/*bFlipy*/);
				break;
			}
			case IDC_DOC_FLIP:
			{
				m_pDocWindow->UndoMirrorFlip(pSym, false/*bFlipX*/, true/*bFlipy*/);
				break;
			}
			case IDC_DOC_ROTATECW:
			{
				m_pDocWindow->UndoRotate(pSym, false/*bCW*/);
				break;
			}
			case IDC_DOC_ROTATECCW:
			{
				m_pDocWindow->UndoRotate(pSym, true/*bCW*/);
				break;
			}
			case IDC_DOC_TRANSFORM:
			case IDC_DOC_POSCENTER:
			case IDC_DOC_POSCENTERH:
			case IDC_DOC_POSCENTERV:
			case IDC_DOC_POSLEFT:
			case IDC_DOC_POSRIGHT:
			case IDC_DOC_POSTOP:
			case IDC_DOC_POSTOPLEFT:
			case IDC_DOC_POSTOPRIGHT:
			case IDC_DOC_POSBOTTOM:
			case IDC_DOC_POSBOTTOMLEFT:
			case IDC_DOC_POSBOTTOMRIGHT:
			{
				CAGMatrix CurrentMatrix = pSym->GetMatrix();
				m_pDocWindow->UndoTransform(pSym, pItem->GetItemMatrix());
				pItem->SetItemMatrix(CurrentMatrix);
				break;
			}
			case IDC_DOC_FILLCOLORCHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				COLORREF Color = pGraphicSym->GetFillColor();
				m_pDocWindow->UndoColor(pSym, pItem->GetItemValue(), false/*bLineColor*/);
				pItem->SetItemValue(Color);
				break;
			}
			case IDC_DOC_FILLCOLORCHANGE2:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				COLORREF Color = pGraphicSym->GetFillColor2();
				m_pDocWindow->UndoColor(pSym, pItem->GetItemValue(), false/*bLineColor*/, true/*FillColor2*/);
				pItem->SetItemValue(Color);
				break;
			}
			case IDC_DOC_LINECOLORCHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				COLORREF Color = pGraphicSym->GetLineColor();
				m_pDocWindow->UndoColor(pSym, pItem->GetItemValue(), true/*bLineColor*/);
				pItem->SetItemValue(Color);
				break;
			}
			case IDC_DOC_LINEWIDTHCHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				int nWidth = pGraphicSym->GetLineWidth();
				m_pDocWindow->UndoLineWidth(pSym, pItem->GetItemValue(), pItem->GetItemValue2());
				pItem->SetItemValue(nWidth);
				break;
			}
			case IDC_DOC_FILLTYPECHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				FillType Fill = pGraphicSym->GetFillType();
				m_pDocWindow->UndoFillType(pSym, pItem->GetItemValue());
				pItem->SetItemValue((int)Fill);
				break;
			}
			case IDC_DOC_DESTRECTCHANGE:
			{
				RECT CurrentDestRect = pSym->GetDestRect();
				m_pDocWindow->UndoDestRect(pSym, pItem->GetItemRect());
				pItem->SetItemRect(CurrentDestRect);
				break;
			}
			case IDC_DOC_CROP:
			{
				CAGSymImage* pImage = (CAGSymImage*)pSym;
				RECT CurrentCropRect = pImage->GetCropRect();
				m_pDocWindow->UndoCropRect(pSym, pItem->GetItemRect());
				pItem->SetItemRect(CurrentCropRect);
				break;
			}
			case IDC_DOC_BACKONE:
			case IDC_DOC_FORWARDONE:
			case IDC_DOC_BRINGTOFRONT:
			case IDC_DOC_PUSHTOBACK:
			{
				m_pDocWindow->UndoLayer(pSym, -pItem->GetItemValue());
				break;
			}
			case IDC_DOC_TEXT_EDIT_END:
			{
				m_pDocWindow->UndoTextEdit(pItem);
				break;
			}
			case IDC_DOC_LOCKED:
			{
				m_pDocWindow->UndoLocked(pSym);
				break;
			}
			case IDC_DOC_HIDDEN:
			{
				m_pDocWindow->UndoHidden(pSym);
				break;
			}
			case IDC_DOC_TRANSPARENT:
			{
				m_pDocWindow->UndoTransparent(pSym);
				break;
			}
			case IDC_DOC_PAGENAV:
			{
				int nLastPage = pItem->GetItemValue();
				int nPage = pItem->GetItemValue2();
				m_pDocWindow->UndoPageTurn(nLastPage);
				break;
			}
		}
	}

	// Select the modified symbol
	m_pDocWindow->SymbolSelect(pSym);

	TransferItem(m_dqUndoItemList, m_dqRedoItemList);
}

//////////////////////////////////////////////////////////////////////
void CUndo::OnEditRedo()
{
	if (m_pSymTextEdit && m_pSymTextEdit->IsDirty()) // if editing text, and edits have been made...
		CheckPointText(NULL, m_pSymTextEdit);

	// Unselect the current symbol
	// This must be done before working with the undo list below
	m_pDocWindow->SymbolUnselect(true/*bClearPointer*/);

	CUndoItem* pItem = GetListItem(m_dqRedoItemList);
	if (!pItem)
		return;

	CAGSym* pSym = pItem->GetUndoItem();
	if (pSym || pItem->GetUndoCommand() == IDC_DOC_PAGENAV)
	{
		switch (pItem->GetUndoCommand())
		{
			case IDC_DOC_ADDIMAGE:
			case IDC_DOC_ADDAPHOTO:
			case IDC_DOC_ADDTEXTBOX:
			case IDC_DOC_ADDSHAPE:
			case IDC_DOC_ADDCALENDAR:
			case IDC_DOC_PASTE:
			{
				m_pDocWindow->UndoDelete(pSym);
				break;
			}
			case IDC_DOC_CUT:
			case IDC_DOC_DELETE:
			{
				m_pDocWindow->UndoAdd(pSym);
				break;
			}
			case IDC_CONVERT_TO_JPG:
			{
				CAGSym* pSymCopy = (CAGSym*)pItem->GetItemValue();
				m_pDocWindow->UndoAdd(pSym);
				m_pDocWindow->UndoDelete(pSymCopy);
				break;
			}
			case IDC_DOC_MIRROR:
			{
				m_pDocWindow->UndoMirrorFlip(pSym, true/*bFlipX*/, false/*bFlipy*/);
				break;
			}
			case IDC_DOC_FLIP:
			{
				m_pDocWindow->UndoMirrorFlip(pSym, false/*bFlipX*/, true/*bFlipy*/);
				break;
			}
			case IDC_DOC_ROTATECW:
			{
				m_pDocWindow->UndoRotate(pSym, false/*bCW*/);
				break;
			}
			case IDC_DOC_ROTATECCW:
			{
				m_pDocWindow->UndoRotate(pSym, true/*bCW*/);
				break;
			}
			case IDC_DOC_TRANSFORM:
			case IDC_DOC_POSCENTER:
			case IDC_DOC_POSCENTERH:
			case IDC_DOC_POSCENTERV:
			case IDC_DOC_POSLEFT:
			case IDC_DOC_POSRIGHT:
			case IDC_DOC_POSTOP:
			case IDC_DOC_POSTOPLEFT:
			case IDC_DOC_POSTOPRIGHT:
			case IDC_DOC_POSBOTTOM:
			case IDC_DOC_POSBOTTOMLEFT:
			case IDC_DOC_POSBOTTOMRIGHT:
			{
				CAGMatrix CurrentMatrix = pSym->GetMatrix();
				m_pDocWindow->UndoTransform(pSym, pItem->GetItemMatrix());
				pItem->SetItemMatrix(CurrentMatrix);
				break;
			}
			case IDC_DOC_FILLCOLORCHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				COLORREF Color = pGraphicSym->GetFillColor();
				m_pDocWindow->UndoColor(pSym, pItem->GetItemValue(), false/*bLineColor*/);
				pItem->SetItemValue(Color);
				break;
			}
			case IDC_DOC_FILLCOLORCHANGE2:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				COLORREF Color = pGraphicSym->GetFillColor2();
				m_pDocWindow->UndoColor(pSym, pItem->GetItemValue(), false/*bLineColor*/, true/*FillColor2*/);
				pItem->SetItemValue(Color);
				break;
			}
			case IDC_DOC_LINECOLORCHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				COLORREF Color = pGraphicSym->GetLineColor();
				m_pDocWindow->UndoColor(pSym, pItem->GetItemValue(), true/*bLineColor*/);
				pItem->SetItemValue(Color);
				break;
			}
			case IDC_DOC_LINEWIDTHCHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				int nWidth = pGraphicSym->GetLineWidth();
				m_pDocWindow->UndoLineWidth(pSym, pItem->GetItemValue(), pItem->GetItemValue2());
				pItem->SetItemValue(nWidth);
				break;
			}
			case IDC_DOC_FILLTYPECHANGE:
			{
				CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;
				FillType Fill = pGraphicSym->GetFillType();
				m_pDocWindow->UndoFillType(pSym, pItem->GetItemValue());
				pItem->SetItemValue((int)Fill);
				break;
			}
			case IDC_DOC_DESTRECTCHANGE:
			{
				RECT CurrentDestRect = pSym->GetDestRect();
				m_pDocWindow->UndoDestRect(pSym, pItem->GetItemRect());
				pItem->SetItemRect(CurrentDestRect);
				break;
			}
			case IDC_DOC_CROP:
			{
				CAGSymImage* pImage = (CAGSymImage*)pSym;
				RECT CurrentCropRect = pImage->GetCropRect();
				m_pDocWindow->UndoCropRect(pSym, pItem->GetItemRect());
				pItem->SetItemRect(CurrentCropRect);
				break;
			}
			case IDC_DOC_BACKONE:
			case IDC_DOC_FORWARDONE:
			case IDC_DOC_BRINGTOFRONT:
			case IDC_DOC_PUSHTOBACK:
			{
				m_pDocWindow->UndoLayer(pSym, pItem->GetItemValue());
				break;
			}
			case IDC_DOC_TEXT_EDIT_END:
			{
				m_pDocWindow->UndoTextEdit(pItem);
				break;
			}
			case IDC_DOC_LOCKED:
			{
				m_pDocWindow->UndoLocked(pSym);
				break;
			}
			case IDC_DOC_HIDDEN:
			{
				m_pDocWindow->UndoHidden(pSym);
				break;
			}
			case IDC_DOC_TRANSPARENT:
			{
				m_pDocWindow->UndoTransparent(pSym);
				break;
			}
			case IDC_DOC_PAGENAV:
			{
				int nLastPage = pItem->GetItemValue();
				int nPage = pItem->GetItemValue2();
				m_pDocWindow->UndoPageTurn(nPage);
				break;
			}
		}
	}

	// Select the modified symbol
	m_pDocWindow->SymbolSelect(pSym);

	TransferItem(m_dqRedoItemList, m_dqUndoItemList);
}

//////////////////////////////////////////////////////////////////////
void CUndo::AddUndoItem(CUndoItem* pUndoItem)
{
	ClearList(m_dqRedoItemList, true/*fClearAllItems*/);
	ClearList(m_dqUndoItemList, false/*fClearAllItems*/);

	m_dqUndoItemList.push_back(pUndoItem);
}

//////////////////////////////////////////////////////////////////////
CUndoItem* CUndo::RemoveItem(CUndoList& List, bool fClearAllItems)
{
	int nSize = List.size();
	bool bRemove = (fClearAllItems ? (nSize > 0) : (nSize > MAX_UNDO_ITEMS));
	if (!bRemove)
		return NULL;

	// Pop list item head
	CUndoItem* pItem = List[0];
	List.pop_front();
	return pItem;
}

//////////////////////////////////////////////////////////////////////
void CUndo::DereferenceItem(CAGSym* pSym)
{
	if (!pSym)
		return;

	CUndoList& UndoList = m_dqUndoItemList;
	int nSize = UndoList.size();
	for (int i = 0; i < nSize; i++)
	{
		CUndoItem* pItem = UndoList[i];
		if (pItem && (pItem->GetUndoItem() == pSym))
			pItem->SetUndoItem(NULL);
	}

	CUndoList& RedoList = m_dqRedoItemList;
	nSize = RedoList.size();
	for (int i = 0; i < nSize; i++)
	{
		CUndoItem* pItem = RedoList[i];
		if (pItem && (pItem->GetUndoItem() == pSym))
			pItem->SetUndoItem(NULL);
	}
}

//////////////////////////////////////////////////////////////////////
void CUndo::ClearList(CUndoList& List, bool fClearAllItems)
{
	CUndoItem* pItem = NULL;
	while (pItem = RemoveItem(List, fClearAllItems))
	{
		CAGSym* pUndoCopy = pItem->GetUndoCopy();
		if (pUndoCopy && pUndoCopy->IsDeleted())
			delete pUndoCopy;

		if (!fClearAllItems)
		{
			CAGSym* pSym = pItem->GetUndoItem();
			if (pSym && pSym->IsDeleted())
				m_pDocWindow->UndoPermDelete(pSym);
		}

		pItem->SetUndoItem(NULL);
		delete pItem;
	}
}

//////////////////////////////////////////////////////////////////////
void CUndo::CheckPoint(UINT nCommand, CAGSym* pSym, long lValue, long lValue2)
{
//j	if (!pSym)
//j		return;

	CUndoItem* pUndoItem = new CUndoItem;
	if (!pUndoItem)
		return;

	pUndoItem->SetUndoCommand(nCommand);
	pUndoItem->SetUndoItem(pSym);
	pUndoItem->SetItemValue(lValue);
	pUndoItem->SetItemValue2(lValue2);
	ClearPendingTextEdit();
	AddUndoItem(pUndoItem);
}

//////////////////////////////////////////////////////////////////////
void CUndo::CheckPoint(UINT nCommand, CAGSym* pSym, CAGMatrix Matrix)
{
	if (!pSym)
		return;

	CUndoItem* pUndoItem = new CUndoItem;
	if (!pUndoItem)
		return;

	pUndoItem->SetUndoCommand(nCommand);
	pUndoItem->SetUndoItem(pSym);
	pUndoItem->SetItemMatrix(Matrix);
	ClearPendingTextEdit();
	AddUndoItem(pUndoItem);
}

//////////////////////////////////////////////////////////////////////
void CUndo::CheckPoint(UINT nCommand, CAGSym* pSym, RECT Rect)
{
	if (!pSym)
		return;
		
	CUndoItem* pUndoItem = new CUndoItem;
	if (!pUndoItem)
		return;

	pUndoItem->SetUndoCommand(nCommand);
	pUndoItem->SetUndoItem(pSym);
	pUndoItem->SetItemRect(Rect);
	ClearPendingTextEdit();
	AddUndoItem(pUndoItem);
}

//////////////////////////////////////////////////////////////////////
void CUndo::CheckPointText(UINT nCommand, CAGSymText* pSymText)
{
	if (!pSymText)
		return;

	bool bRestart = !nCommand;
	if (nCommand == IDC_DOC_TEXT_EDIT_END || bRestart)
	{
		if (m_pSymTextEdit != pSymText)
			return; // should never happen

		if (!m_pSymTextEdit->IsDirty())
		{
			if (!bRestart)
				ClearPendingTextEdit();
			return;
		}

		m_pSymTextEdit->SetDirty(false);

 		if (m_pSymTextCopy)
		{
			CUndoItem* pUndoItem = new CUndoItem;
			if (pUndoItem)
			{
				pUndoItem->SetUndoCommand(IDC_DOC_TEXT_EDIT_END);
				pUndoItem->SetUndoItem(m_pSymTextEdit);
				pUndoItem->SetUndoCopy(m_pSymTextCopy);
				AddUndoItem(pUndoItem);
			}
			else
				delete m_pSymTextCopy;

			m_pSymTextCopy = NULL;
		}

		m_pSymTextEdit = NULL;
	}

	if (nCommand == IDC_DOC_TEXT_EDIT_START || bRestart)
	{
		ClearPendingTextEdit();
		m_pSymTextEdit = pSymText;
		m_pSymTextEdit->SetDirty(false);

		if (m_pSymTextCopy = (CAGSymText*)m_pSymTextEdit->Duplicate())
			m_pSymTextCopy->SetDeleted(true);
	}
}

//////////////////////////////////////////////////////////////////////
void CUndo::TransferItem(CUndoList& ListSrc, CUndoList& ListDst)
{
	CUndoItem* pItem = GetListItem(ListSrc);
	if (!pItem)
		return;

	// Pop list item
	int nSize = ListSrc.size();
	if (nSize > 0)
		ListSrc.pop_back();

	// Push list item
	ListDst.push_back(pItem);
	nSize = ListDst.size();
}

//////////////////////////////////////////////////////////////////////
CUndoItem* CUndo::GetListItem(CUndoList& List)
{
	int nSize = List.size();
	if (nSize <= 0)
		return NULL;

	return List[nSize - 1];
}

//////////////////////////////////////////////////////////////////////
void CUndo::ClearPendingTextEdit()
{
	m_pSymTextEdit = NULL;

	if (m_pSymTextCopy)
	{
		delete m_pSymTextCopy;
		m_pSymTextCopy = NULL;
	}
}
