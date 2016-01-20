/*
// $Workfile: ADDRLIST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:02p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRLIST.CPP $
// 
// 1     3/03/99 6:02p Gbeddow
// 
// 8     12/23/98 1:50p Psasse
// Code clean up
// 
// 7     11/20/98 2:13p Johno
// Kludged crash bug fix in COverviewPositionArray::GetWidth():
// When called from CAddressBookOverviewListBox::OnPaint(), can get a NULL
// pointer.
// 
// 6     9/02/98 5:34p Hforman
// resource leak: add DeleteFont() to ~CAddressBookReceiverListBox()
// 
// 5     7/11/98 2:48p Jayn
// Got rid of some resource leaks.
// 
// 4     6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 3     5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 2     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:18:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:04   Fred
// Initial revision.
// 
//    Rev 1.26   11 Sep 1996 10:25:50   Fred
// Fixed some small bugs:
//    Can't grab first splitter bar anymore (was was confusing)
//    Editors properly positioned on overview list box resize.
//    Header area properly refreshed which no address book loaded.
// 
//    Rev 1.25   09 Aug 1996 16:50:14   Fred
// More 16-bit fixes
// 
//    Rev 1.24   08 Aug 1996 11:12:28   Fred
// 16-bit fixes
// 
//    Rev 1.23   07 Aug 1996 08:42:14   Fred
//  
// 
//    Rev 1.22   03 Aug 1996 16:42:50   Fred
//  
// 
//    Rev 1.21   02 Aug 1996 15:40:50   Fred
//  
// 
//    Rev 1.20   02 Aug 1996 15:02:20   Fred
// 16-bit changes
// 
//    Rev 1.19   01 Aug 1996 09:22:34   Fred
// More 16-bit changes
// 
//    Rev 1.18   31 Jul 1996 11:57:54   Fred
// 16-bit compatibility
// 
//    Rev 1.17   18 Jul 1996 18:43:52   Fred
// Palette fixes
// 
//    Rev 1.16   17 Jul 1996 16:38:34   Fred
// Uses addrdata; compresses pictures
// 
//    Rev 1.15   17 Jul 1996 09:04:52   Fred
// Some address book improvements
// 
//    Rev 1.14   16 Jul 1996 08:50:40   Fred
// Hooked up address/sender book
// 
//    Rev 1.13   12 Jul 1996 16:26:36   Fred
// Address book error handling
// 
//    Rev 1.12   11 Jul 1996 08:37:28   Fred
// More address book features
// 
//    Rev 1.11   06 Jul 1996 09:25:16   Fred
// Picture editor and other improvements
// 
//    Rev 1.10   03 Jul 1996 18:52:26   Fred
// Picture editor
// 
//    Rev 1.9   01 Jul 1996 07:30:18   Fred
// Receiver list
// 
//    Rev 1.8   27 Jun 1996 10:42:48   Fred
// Receiver list. Before removing receiver from non-receiver views
// 
//    Rev 1.7   24 Jun 1996 19:16:18   Fred
// Some fixes and improvments
// 
//    Rev 1.6   24 Jun 1996 00:29:50   Fred
// Address Book Overview
// 
//    Rev 1.5   21 Jun 1996 09:29:48   Fred
// More overview list
// 
//    Rev 1.4   21 Jun 1996 07:00:50   Fred
// Start of overview list box
// 
//    Rev 1.3   18 Jun 1996 15:20:36   Fred
// Field value list management and editing
// 
//    Rev 1.2   18 Jun 1996 08:25:02   Fred
// More adress book changes
// 
//    Rev 1.1   10 Jun 1996 18:43:46   Fred
// Memo editor
// 
//    Rev 1.0   10 Jun 1996 08:22:08   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "memdev.h"
#include "util.h"
#include "dbtabs.h"
#include "addrdata.h"
#include "addrbook.h"
#include "addrlist.h"

#define new DEBUG_NEW

extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

/////////////////////////////////////////////////////////////////////////////
// Helper functions

void FillRectangleWithColor(CDC& dc, const CRect& crRect, COLORREF clFill, COLORREF clBorder)
{
	CRect crFill;

	// Do the fill.
	crFill = crRect;
	crFill.right--;
	crFill.bottom--;
	Util::FillRectangleWithColor(dc, crFill, clFill);

	// Do the bottom line.
	crFill = crRect;
	crFill.top = crFill.bottom-1;
	Util::FillRectangleWithColor(dc, crFill, clBorder);

	// Do the right line.
	crFill = crRect;
	crFill.left = crFill.right-1;
	Util::FillRectangleWithColor(dc, crFill, clBorder);
}

void DrawCheckMark(CDC& dc, const CRect crRect, COLORREF clColor)
{
	if (!crRect.IsRectEmpty())
	{
		int nWidth = crRect.Width();
		int nHeight = crRect.Height();

		if (nWidth > nHeight)
		{
			nWidth = nHeight;
		}
		else
		{
			nHeight = nWidth;
		}

		int nX0 = crRect.left+(crRect.Width()-nWidth)/2;
		int nY0 = crRect.top+(crRect.Height()-nHeight)/2;
		int nX1 = nX0+nWidth;
		int nY1 = nY0+nHeight;

		// Draw "X"
		CPen cpCheck;
		TRY
		{
			cpCheck.CreatePen(PS_SOLID, 0, clColor);
			dc.SelectObject(&cpCheck);
			dc.MoveTo(nX0, nY0);
			dc.LineTo(nX1, nY1);
			dc.MoveTo(nX0, nY0+1);
			dc.LineTo(nX1-1, nY1);
			dc.MoveTo(nX0+1, nY0);
			dc.LineTo(nX1, nY1-1);
			dc.MoveTo(nX1-1, nY0);
			dc.LineTo(nX0-1, nY1);
			dc.MoveTo(nX1-1-1, nY0);
			dc.LineTo(nX0-1, nY1-1);
			dc.MoveTo(nX1-1, nY0+1);
			dc.LineTo(nX0-1+1, nY1);
		}
		END_TRY
	}
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldListBoxItem

CAddressBookFieldListBoxItem::CAddressBookFieldListBoxItem(CAddressBookFieldListBox* pListBox, CFlatFileDatabaseField* pField)
{
	m_pListBox = pListBox;
	m_pField = pField;
	m_pEditor = NULL;
	m_crName.SetRectEmpty();
	m_crValue.SetRectEmpty();
	CreateEditor();
}

CAddressBookFieldListBoxItem::~CAddressBookFieldListBoxItem()
{
	delete m_pEditor;
	m_pEditor = NULL;
}

void CAddressBookFieldListBoxItem::Edit(void)
{
	ASSERT(m_pField != NULL);
	if (m_pField != NULL)
	{
		if (m_pEditor != NULL)
		{
			m_pEditor->SetFocus();
		}
		else
		{
			m_pListBox->SetFocus();
		}
	}
}

void CAddressBookFieldListBoxItem::Reset(void)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->Reset();
	}
}

void CAddressBookFieldListBoxItem::Move(const CRect& crPosition)
{
	CRect crEdit(crPosition);
	EditRectangle(crEdit);

	if (m_pEditor != NULL)
	{
		m_pEditor->Move(crEdit);
		m_pEditor->Show();
	}
}

BOOL CAddressBookFieldListBoxItem::IsEditing(void)
{
	if (m_pEditor != NULL)
	{
		return m_pEditor->IsEditing();
	}
	return FALSE;
}

BOOL CAddressBookFieldListBoxItem::IsModified(void)
{
	if (m_pEditor != NULL)
	{
		return m_pEditor->IsModified();
	}
	return FALSE;
}

void CAddressBookFieldListBoxItem::SetModified(BOOL fModified /*=TRUE*/)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->SetModified(fModified);
	}
}

void CAddressBookFieldListBoxItem::Save(void)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->Save();
	}
}

void CAddressBookFieldListBoxItem::Refresh(void)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->Refresh();
	}
}

void CAddressBookFieldListBoxItem::Redraw(void)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->Redraw();
	}
}

void CAddressBookFieldListBoxItem::ExcludeClipRect(CDC* pParentDC)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->ExcludeClipRect(pParentDC);
	}
}

void CAddressBookFieldListBoxItem::Show(BOOL fShow /*=TRUE*/)
{
	if (m_pEditor != NULL)
	{
		m_pEditor->Show(fShow);
	}
}

void CAddressBookFieldListBoxItem::CreateEditor(void)
{
	ASSERT(m_pField != NULL);
	if (m_pField != NULL)
	{
		// If we don't have an editor for this field, create one.
		if (m_pEditor == NULL)
		{
			m_pEditor = CAddressBookFieldEditor::Create(m_pField, m_pListBox);
		}
	}
}

void CAddressBookFieldListBoxItem::EditRectangle(CRect& crRect)
{
	if (m_pField != NULL)
	{
		crRect.left = m_pListBox->GetSplitter();
		crRect.bottom -= 1;
		
		switch (FIELD_USER_DATA_EDITOR(m_pField->GetDescription()->GetUserData()))
		{
			case FIELD_EDIT_NORMAL:
			case FIELD_EDIT_LIST:
			case FIELD_EDIT_MEMO:
			{
				// Magic numbers.
				crRect.top += 3;
				crRect.bottom -= 3;
				crRect.left += 6;
				break;
			}
			case FIELD_EDIT_PICTURE:
			{
				// Magic numbers.
				crRect.top += 3;
				crRect.bottom -= 3;
				crRect.left += 3;
				break;
			}
		}
	}
}

void CAddressBookFieldListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if (pDrawItemStruct->itemID != -1)
	{
		if ((pDrawItemStruct->itemAction & ODA_DRAWENTIRE) != 0)
		{
			// Paint the item.
			Paint(pList, pDrawItemStruct);
		}
	}
}

void CAddressBookFieldListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	CAddressBookFieldListBox* pListBox = (CAddressBookFieldListBox*)pList;
	pListBox->Move();
	
	// Get the DC and rectangle to use.

	CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);

	if (pDC != NULL)
	{
		pDC->SaveDC();
		
		CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();

		// Exclude any child control (editor) rectangles.
		ExcludeClipRect(pDC);

		// Draw background rectangle for name.
		m_crName = pDrawItemStruct->rcItem;
		m_crName.right = pListBox->GetSplitter();
		FillRectangleWithColor(*pDC, m_crName, FIELD_BACKGROUND_COLOR, GetSysColor(COLOR_BTNSHADOW));

		// Draw background rectangle for value.
		m_crValue = pDrawItemStruct->rcItem;
		m_crValue.left = pListBox->GetSplitter();
		m_crValue.bottom--;
		Util::FillRectangleWithColor(*pDC, m_crValue, GetSysColor(COLOR_WINDOW));

		// Draw line under value.
		CRect crLine(m_crValue);
		crLine.top = crLine.bottom;
		crLine.bottom++;
		Util::FillRectangleWithColor(*pDC, crLine, GetSysColor(COLOR_BTNFACE));

		if (m_pField != NULL)
		{
			// Draw name.
			CString csName;
			TRY
			{
				csName = m_pField->GetName();
				pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
				pDC->SetBkMode(TRANSPARENT);
				Util::DrawText(pDC, csName, m_crName, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			}
			END_TRY
		}

		if (pOldPal != NULL)
		{
			pDC->SelectPalette(pOldPal, FALSE);
		}
		
		pDC->RestoreDC(-1);

		// Force a redraw now (works well with NT real-time resizing.)
//		Redraw();
	}
}

void CAddressBookFieldListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	CRect crClient;
	pList->GetClientRect(crClient);
					
	pMeasureItemStruct->itemWidth = crClient.Width();

	if (m_pField->GetType() != FFT_Blob)
	{
		pMeasureItemStruct->itemHeight = ((CAddressBookFieldListBox*)pList)->GetLineHeight();
	}
	else
	{
		if (FIELD_USER_DATA_EDITOR(m_pField->GetDescription()->GetUserData()) == FIELD_EDIT_PICTURE)
		{
			pMeasureItemStruct->itemHeight = ((CAddressBookFieldListBox*)pList)->GetLineHeight()*12;
		}
		if (FIELD_USER_DATA_EDITOR(m_pField->GetDescription()->GetUserData()) == FIELD_EDIT_MEMO)
		{
			pMeasureItemStruct->itemHeight = ((CAddressBookFieldListBox*)pList)->GetLineHeight()*5;
		}
	}

	pMeasureItemStruct->itemHeight += 3+3+1;
}

int CAddressBookFieldListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CAddressBookFieldListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldListBox

WNDPROC CAddressBookFieldListBox::m_pfnSuperWndProc = NULL;

CAddressBookFieldListBox::CAddressBookFieldListBox()
{
	m_nSplitter = -1;
	m_nLineHeight = -1;
	m_nCharWidth = -1;
	m_nEditIndex = -1;
}

CAddressBookFieldListBox::~CAddressBookFieldListBox()
{
}

BEGIN_MESSAGE_MAP(CAddressBookFieldListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CAddressBookFieldListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EDITOR_ACTIVATED, OnEditorActivated)
	ON_MESSAGE(WM_EDITOR_DEACTIVATED, OnEditorDeactivated)
	ON_MESSAGE(WM_GET_FIELD_LIST, OnGetFieldList)
	ON_MESSAGE(WM_GET_DATABASE_TABS, OnGetDatabaseTabs)
	ON_MESSAGE(WM_GET_RECEIVER_LIST, OnGetReceiverList)
END_MESSAGE_MAP()

BOOL CAddressBookFieldListBox::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	// Hook the dialog item.
	if (!INHERITED::SubclassDlgItem(nID, pParent))
	{
		return FALSE;
	}

	// Get some information that will be useful for ourselves and our list item objects,
	TEXTMETRIC tm;
	FontMetrics(tm);

	m_nLineHeight = tm.tmHeight;
	m_nCharWidth = tm.tmAveCharWidth;

	m_nSplitter = 25*m_nCharWidth;

	CRect crBounds;
	GetWindowRect(&crBounds);
	if (m_nSplitter > (3*crBounds.Width())/4)
	{
		m_nSplitter = (3*crBounds.Width())/4;
	}

	m_SplitterCursor.SetWindow(this);

	return TRUE;
}

void CAddressBookFieldListBox::SetSplitter(int nSplitter)
{
	if (m_nSplitter != nSplitter)
	{
		m_nSplitter = nSplitter;
		Move();
	}
}

void CAddressBookFieldListBox::Edit(int nIndex /*=-1*/)
{
	if ((nIndex == -1) && (m_nEditIndex != -1))
	{
		// The caller requested an arbitrary index and we're already editing.
		return;
	}

	if ((nIndex != -1) && (nIndex == m_nEditIndex))
	{
		// The caller requested the index that we're already editing.
		return;
	}

	if (nIndex == -1)
	{
		nIndex = GetCurSel();
	}

	if (nIndex == LB_ERR)
	{
		nIndex = GetTopIndex();
	}

	if ((nIndex != LB_ERR) && (nIndex < GetCount()))
	{
		// Start up the new editor.
		CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nIndex);
		if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
		{
			pItem->Reset();
			pItem->Edit();
			m_nEditIndex = nIndex;
		}
	}
}

BOOL CAddressBookFieldListBox::IsModified(void)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
		if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
		{
			if (pItem->IsModified())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CAddressBookFieldListBox::SetModified(BOOL fModified)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
		if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
		{
			pItem->SetModified(fModified);
		}
	}
}

void CAddressBookFieldListBox::Save(void)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
		if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
		{
			pItem->Save();
		}
	}
}

void CAddressBookFieldListBox::Refresh(void)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
		if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
		{
			pItem->Refresh();
		}
	}
}

void CAddressBookFieldListBox::Move(void)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CRect crItem;
		if (GetItemRect(nItem, crItem) != LB_ERR)
		{
			CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
			if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
			{
				pItem->Move(crItem);
			}
		}
	}
}

// Refresh an item in the list box.
void CAddressBookFieldListBox::RefreshItem(int nItem /*=-1*/)
{
	if (nItem < 0)
	{
		if ((nItem = GetCurSel()) == LB_ERR)
		{
			// No current selection.
			return;
		}
	}

	CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
	if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
	{
		pItem->Refresh();
	}
}

void CAddressBookFieldListBox::Show(BOOL fShow /*=TRUE*/)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CRect crItem;
		if (GetItemRect(nItem, crItem) != LB_ERR)
		{
			CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
			if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
			{
				pItem->Show(fShow);
			}
		}
	}
}

int CAddressBookFieldListBox::GetFieldIndex(CFlatFileDatabaseField* pField)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData(nItem);
		if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
		{
			if (pItem->GetField() == pField)
			{
				return nItem;
			}
		}
	}

	return -1;
}

DWORD CAddressBookFieldListBox::GetFieldCount(void)
{
	return (DWORD)GetCount();
}

CFlatFileDatabaseField* CAddressBookFieldListBox::GetField(DWORD dwField)
{
	CFlatFileDatabaseField* pField = NULL;
	CAddressBookFieldListBoxItem* pItem = (CAddressBookFieldListBoxItem*)GetItemData((int)dwField);
	if (pItem != (CAddressBookFieldListBoxItem*)LB_ERR)
	{
		pField = pItem->GetField();
	}
	return pField;
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookFieldListBox message handlers

void CAddressBookFieldListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_SplitterCursor.IsShowing())
	{
		CRect crListBox;
		GetClientRect(crListBox);
		m_SplitterCursor.StartDrag(point.x, m_nSplitter-point.x, crListBox.left+GetCharWidth(), crListBox.right-GetCharWidth());
	}
}

void CAddressBookFieldListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	INHERITED::OnLButtonUp(nFlags, point);

	if (m_SplitterCursor.IsDragging())
	{
		m_SplitterCursor.EndDrag(point.x);
		if (m_nSplitter != m_SplitterCursor.GetX())
		{
			SetSplitter(m_SplitterCursor.GetX());
			Invalidate();
		}
	}
}

void CAddressBookFieldListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CAddressBookFieldListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	INHERITED::OnMouseMove(nFlags, point);

	// If the splitter bar is being dragged, continue the drag.
	if (m_SplitterCursor.IsDragging())
	{
		m_SplitterCursor.Drag(point.x);
	}
	else
	{
		// Check if the mouse is over the splitter bar. If so, show the splitter cursor.
		if ((point.x >= m_nSplitter-CSplitterCursor::Padding) && (point.x <= m_nSplitter+CSplitterCursor::Padding))
		{
			m_SplitterCursor.StartShow();
		}
		else
		{
			m_SplitterCursor.EndShow();
		}
	}
}

UINT CAddressBookFieldListBox::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode() | DLGC_WANTTAB;
	if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_RETURN, VK_ESCAPE, 0))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

void CAddressBookFieldListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsEditing())
	{
		switch (nChar)
		{
			case VK_TAB:
			{
				if (::GetKeyState(VK_SHIFT) < 0)
				{
					// Shift key is down. Do back tab.
					BackField(TRUE);
				}
				else
				{
					// Shift key is not down. Do forward tab.
					NextField(TRUE);
				}
				break;
			}
			case VK_RETURN:
			{
				NextField(TRUE);
				break;
			}
			case VK_ESCAPE:
			{
				if (!IsModified())
				{
					// If we are adding a new record and nothing has been input,
					// then Escape will abort the edit.
					Util::SendWmCommand(GetParent(), IDC_UNDO, NULL, 0);
				}
				else
				{
					RefreshItem();
				}
				break;
			}
			default:
			{
				INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
				Edit(GetCurSel());
				break;
			}
		}
	}
	else
	{
		INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CAddressBookFieldListBox::BackField(BOOL fMoveOffListBox)
{
	int nCurSel = GetCurSel();
	int nCount = GetCount();

	if (nCurSel == LB_ERR)
	{
		// No current selection, use top index.
		if ((nCount != LB_ERR) && (nCount > 0))
		{
			SetCurSel(GetTopIndex());
			nCurSel = GetCurSel();
		}
	}

	// Move up one item.
	if ((nCurSel != LB_ERR) && (nCount != LB_ERR))
	{
		nCurSel--;

		if (nCurSel < 0)
		{
			if (fMoveOffListBox)
			{
				// At end of list. Move to next dialog control.
				CDialog* pDialog = (CDialog*)GetParent();
				if (pDialog != NULL)
				{
					pDialog->PrevDlgCtrl();
				}
			}
		}
		else
		{
			// Start up the edit for the next field.
			SetCurSel(nCurSel);
			Edit(nCurSel);
		}
	}
}

void CAddressBookFieldListBox::NextField(BOOL fMoveOffListBox)
{
	int nCurSel = GetCurSel();
	int nCount = GetCount();

	if (nCurSel == LB_ERR)
	{
		// No current selection, use top index.
		if ((nCount != LB_ERR) && (nCount > 0))
		{
			SetCurSel(GetTopIndex());
			nCurSel = GetCurSel();
		}
	}

	// Move down one item.
	if ((nCurSel != LB_ERR) && (nCount != LB_ERR))
	{
		nCurSel++;

		if (nCurSel >= nCount)
		{
			if (fMoveOffListBox)
			{
				// At end of list. Move to next dialog control.
				CDialog* pDialog = (CDialog*)GetParent();
				if (pDialog != NULL)
				{
					pDialog->NextDlgCtrl();
				}
			}
		}
		else
		{
			// Start up the edit for the next field.
			SetCurSel(nCurSel);
			Edit(nCurSel);
		}
	}
}

void CAddressBookFieldListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	INHERITED::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CAddressBookFieldListBox::OnEraseBkgnd(CDC* pDC)
{
	// Erase the part of the list box which does not have any items.
	// This eliminates a lot of flicker.

	CRect crBackground;
	GetClientRect(&crBackground);

	int nTopIndex = GetTopIndex();
	int nCount = GetCount();

	for (int nItem = nTopIndex; nItem < nCount; nItem++)
	{
		CRect crItem;
		if (GetItemRect(nItem, crItem) != LB_ERR)
		{
			crBackground.top = crItem.bottom;
			if (crBackground.IsRectEmpty())
			{
				break;
			}
		}
	}

	if (!crBackground.IsRectEmpty())
	{
		Util::FillRectangleWithColor(*pDC, crBackground, GetSysColor(COLOR_BTNFACE));
	}

	return TRUE;
}

BOOL CAddressBookFieldListBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Pass the message along if we're not in the client area or
	// we're not showing the splitter cursor.
	if ((nHitTest != HTCLIENT) || !m_SplitterCursor.IsShowing())
	{
		return INHERITED::OnSetCursor(pWnd, nHitTest, message);
	}
	return FALSE;
#if 0
	if (m_SplitterCursor.IsShowing())
	{
		return FALSE;
	}

	return INHERITED::OnSetCursor(pWnd, nHitTest, message);
#endif
}

void CAddressBookFieldListBox::OnSize(UINT nType, int cx, int cy) 
{
	INHERITED::OnSize(nType, cx, cy);
	Move();
}

void CAddressBookFieldListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	INHERITED::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CAddressBookFieldListBox::OnEditorActivated(WPARAM wParam, LPARAM lParam)
{
	CAddressBookFieldEditor* pEditor = (CAddressBookFieldEditor*)lParam;
	if (pEditor != NULL)
	{
		CFlatFileDatabaseField* pField = pEditor->GetField();
		if (pField != NULL)
		{
			m_nEditIndex = GetFieldIndex(pField);
			SetCurSel(m_nEditIndex);

			// Notify our parent (the address book) that an editor was activated.
			// This message is used to allow the user to go into "Add New Name" mode
			// by simply clicking on a field in an empty address book.
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		}
	}

	return 0;
}

LRESULT CAddressBookFieldListBox::OnEditorDeactivated(WPARAM wParam, LPARAM lParam)
{
	m_nEditIndex = -1;
	return 0;
}

// One of the editors wants to get a pointer to the list of unique field values for its field.
// To do this, we pass the message on to our parent and hope they are listening!
LRESULT CAddressBookFieldListBox::OnGetFieldList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT CAddressBookFieldListBox::OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT CAddressBookFieldListBox::OnGetReceiverList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

/////////////////////////////////////////////////////////
// COverviewPositionArray

COverviewPositionArray::COverviewPositionArray()
{
}

COverviewPositionArray::~COverviewPositionArray()
{
	DeleteAll();
}
	
void COverviewPositionArray::AddField(DWORD dwField, int nWidth, CAddressBookFieldEditor* pEditor)
{
	TRY
	{
		Item* pItem = new Item(dwField, nWidth, pEditor);
		Add(pItem);
	}
	END_TRY
}

DWORD COverviewPositionArray::GetField(int nIndex)
{
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));
	return GetAt(nIndex)->GetField();
}

int COverviewPositionArray::GetWidth(int nIndex)
{
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));
	Item	*p = GetAt(nIndex);
//	ASSERT(p != NULL);	// ASSERT doesn't work here?
	// KLUDGE ALERT!
	// Something in the flow of things can cause this pointer to be NULL
	// when called from CAddressBookOverviewListBox::OnPaint() 
	// when sort is chosen from the menu while the cursor is in a field.
	// We will fake out the caller by returning 0 as the screen width of this item
	// This was bug #137
	if (p == NULL)
		return 0;
	return p->GetWidth();
}

void COverviewPositionArray::SetWidth(int nIndex, int nWidth)
{
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));
	GetAt(nIndex)->SetWidth(nWidth);
}

int COverviewPositionArray::GetOffset(int nIndex)
{
	int nOffset = 0;
	for (int i = 0; i < nIndex; i++)
	{
		nOffset += GetWidth(i);
	}

	return nOffset;
}

void COverviewPositionArray::SetOffset(int nIndex, int nOffset)
{
	ASSERT((nIndex >= 0) && (nIndex <= GetSize()));

	// Setting the offset of index 0 is a special case.
	if (nIndex == 0)
	{
		// Transform this into terms of index 1.
		nIndex++;
		nOffset = GetWidth(0)-nOffset;
	}

	int nDelta = GetOffset(nIndex)-nOffset;
	if (nDelta < 0)
	{
		// We are moving the entry to the left. Make the previous entry wider.
		SetWidth(nIndex-1, GetWidth(nIndex-1)-nDelta);
	}
	else
	{
		// We are moving the entry to the right. Make the previous entries thinner.
		while ((--nIndex >= 0) && (nDelta > 0))
		{
			int nCurrentWidth = GetWidth(nIndex);
			int nNewWidth = nCurrentWidth-nDelta;
			if (nNewWidth < MinimumWidth)
			{
				nNewWidth = MinimumWidth;
			}
			SetWidth(nIndex, nNewWidth);
			nDelta -= (nCurrentWidth-nNewWidth);
		}
	}
}

int COverviewPositionArray::GetIndex(int nOffset)
{
	for (int nIndex = 0; nIndex < GetSize(); nIndex++)
	{
		int nWidth = GetWidth(nIndex);
		if (nOffset < nWidth)
		{
			break;
		}
		nOffset -= nWidth;
	}

	return nIndex;
}

void COverviewPositionArray::SetIndex(int nIndex, int nNewIndex)
{
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));
	ASSERT((nNewIndex >= 0) && (nNewIndex < GetSize()));
	Item* pItem = GetAt(nIndex);
	RemoveAt(nIndex);
	InsertAt(nNewIndex, pItem);
}

CAddressBookFieldEditor* COverviewPositionArray::GetEditor(int nIndex)
{
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));
	return GetAt(nIndex)->GetEditor();
}

void COverviewPositionArray::DeleteAll(void)
{
	DeleteAt(0, GetSize());
}

void COverviewPositionArray::DeleteAt(int nIndex, int nCount /*=1*/)
{
	for (int i = 0; i < nCount; i++)
	{
		if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
		{
			delete GetAt(nIndex+i);
			SetAt(nIndex+i, NULL);
		}
	}

	INHERITED::RemoveAt(nIndex, nCount);
}

/////////////////////////////////////////////////////////
// COverviewPositionArray::Item

COverviewPositionArray::Item::Item(DWORD dwField, int nWidth, CAddressBookFieldEditor* pEditor)
{
	m_dwField = dwField;
	m_nWidth = nWidth;
	m_pEditor = pEditor;
}

COverviewPositionArray::Item::~Item()
{
	delete m_pEditor;
	m_pEditor = NULL;
}

//////////////////////////////////////////////////////////////////////////
// COverviewListBoxItem

COverviewListBoxItem::COverviewListBoxItem(CAddressBookOverviewListBox* pListBox)
{
	m_pListBox = pListBox;
}

COverviewListBoxItem::~COverviewListBoxItem()
{
}

void COverviewListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if (pDrawItemStruct->itemID != -1)
	{
		if ((pDrawItemStruct->itemAction & (ODA_DRAWENTIRE|ODA_SELECT|ODA_FOCUS)) != 0)
		{
			// Paint the item.
			Paint(pList, pDrawItemStruct);
		}
	}
}

void COverviewListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Get the DC and rectangle to use.
	CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);
	if (pDC != NULL)
	{
		pDC->SaveDC();
		CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();

		CRect crItem(pDrawItemStruct->rcItem);
//		TRACE("Draw overview item (%d,%d,%d,%d)\n", crItem.left, crItem.top, crItem.right, crItem.bottom);

		CFlatFileDatabaseCursor* pDatabaseCursor = m_pListBox->GetDatabaseCursor();
		if (pDatabaseCursor == NULL)
		{
			Util::FillRectangleWithColor(*pDC, crItem, GetSysColor(COLOR_BTNFACE));
		}
		else
		{
			DWORD dwNumber = m_pListBox->GetFirstRecord()+pDrawItemStruct->itemID;
			CFlatFileDatabaseCursor* pSearchCursor = m_pListBox->GetSearchCursor();
			pSearchCursor->SetPosition(dwNumber);
			DWORD dwRecordCount = pSearchCursor->GetRecordCount();

			// Check if the current record is the last in its sort section.
			BOOL fLastRecordInSection = dwNumber+1 >= dwRecordCount;
			if (!fLastRecordInSection)
			{
				CDatabaseTabs* pTabs = (CDatabaseTabs*)(m_pListBox->SendMessage(WM_GET_DATABASE_TABS, 0, 0));
				if (pTabs != NULL)
				{
					CTabDescriptionArray* pDescriptions = pTabs->GetTabDescriptions();
					if (pDescriptions != NULL)
					{
						int nCount = pDescriptions->GetSize();
						for (int nTab = 0; nTab < nCount; nTab++)
						{
							CTabDescriptionBase* pTab = pDescriptions->GetAt(nTab);
							if (pTab != NULL)
							{
								if (dwNumber+1 == pTab->GetNumber())
								{
									fLastRecordInSection = TRUE;
								}
							}
						}
					}
				}
			}

			if (dwNumber <= pSearchCursor->GetRecordCount())
			{
				pDC->OffsetViewportOrg(-m_pListBox->GetHorizontalOffset(), 0);

				COverviewPositionArray* pPositions = m_pListBox->GetPositions();
				for (int i = 0; i < pPositions->GetSize(); i++)
				{
					CRect crField(crItem);
					crField.left = pPositions->GetOffset(i);
					crField.right = crField.left+pPositions->GetWidth(i);
					if (pDC->RectVisible(crField))
					{
						FillRectangleWithColor(*pDC, crField, GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_BTNFACE));
						crField.right--;
						crField.bottom--;

						// If the record is the last in its sorting section, then underline it with
						// a different color.
						if (fLastRecordInSection)
						{
							CRect crDivider(crField);
							crDivider.top = crDivider.bottom;
							crDivider.bottom++;
							crDivider.right++;
							Util::FillRectangleWithColor(*pDC, crDivider, GetSysColor(COLOR_BTNSHADOW));
						}

						CString csText;
						TRY
						{
							DWORD dwField = pPositions->GetField(i);

							csText = pSearchCursor->GetField(dwField)->GetAsString();

							CRect crText(crField);
							crText.left += 3;
							crText.top += 3;
							crText.right -= 3;
							crText.bottom -= 3;

							pDC->SetBkMode(TRANSPARENT);
							pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
							Util::DrawText(pDC, csText, crText, DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
						}
						END_TRY
					}
				}

				// Fill any unsed space.
				CRect crField(crItem);
				crField.left = pPositions->GetTotalWidth();
				if (!crField.IsRectEmpty())
				{
					Util::FillRectangleWithColor(*pDC, crField, GetSysColor(COLOR_BTNFACE));
				}
			}
			else
			{
				// Complete item is empty.
				Util::FillRectangleWithColor(*pDC, crItem, GetSysColor(COLOR_BTNFACE));
			}
		}

		if (pOldPal != NULL)
		{
			pDC->SelectPalette(pOldPal, FALSE);
		}

		pDC->RestoreDC(-1);
	}
}

void COverviewListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	pMeasureItemStruct->itemWidth = m_pListBox->GetPositions()->GetTotalWidth();
	pMeasureItemStruct->itemHeight = m_pListBox->GetItemHeight();
//TRACE("Overview list box item height = %d\n", pMeasureItemStruct->itemHeight);
}

int COverviewListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void COverviewListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// COverviewListBox

WNDPROC COverviewListBox::m_pfnSuperWndProc = NULL;

COverviewListBox::COverviewListBox()
{
}

COverviewListBox::~COverviewListBox()
{
}

BOOL COverviewListBox::CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID)
{
	return CreateEx(0, "ListBox", "Overview ListBox", dwStyle, 0, 0, 0, 0, pParent->GetSafeHwnd(), (HMENU)nID, NULL);
}

BEGIN_MESSAGE_MAP(COverviewListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(COverviewListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EDITOR_ACTIVATED, OnEditorActivated)
	ON_MESSAGE(WM_EDITOR_DEACTIVATED, OnEditorDeactivated)
	ON_MESSAGE(WM_GET_FIELD_LIST, OnGetFieldList)
	ON_MESSAGE(WM_GET_DATABASE_TABS, OnGetDatabaseTabs)
	ON_MESSAGE(WM_GET_RECEIVER_LIST, OnGetReceiverList)
END_MESSAGE_MAP()

// Redraw an item.
void COverviewListBox::RedrawItem(int nItem)
{
	CRect crItem;
	if (GetItemRect(nItem, crItem) != LB_ERR)
	{
		InvalidateRect(crItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// COverviewListBox message handlers

void COverviewListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Translate point to parent coordinates and pass the message up.
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	GetParent()->SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void COverviewListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Translate point to parent coordinates and pass the message up.
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	GetParent()->SendMessage(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void COverviewListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void COverviewListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	// Translate point to parent coordinates and pass the message up.
	ClientToScreen(&point);
	GetParent()->ScreenToClient(&point);
	GetParent()->SendMessage(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

BOOL COverviewListBox::OnEraseBkgnd(CDC* pDC)
{
	// There are always enough items to fill the list box.
	return TRUE;
}

BOOL COverviewListBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	const MSG* pMsg = GetCurrentMessage();
	return (BOOL)(GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam));
}

void COverviewListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	const MSG* pMsg = GetCurrentMessage();
	GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void COverviewListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	const MSG* pMsg = GetCurrentMessage();
	GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT COverviewListBox::OnEditorActivated(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT COverviewListBox::OnEditorDeactivated(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

// One of the editors wants to get a pointer to the list of unique field values for its field.
// To do this, we pass the message on to our parent and hope they are listening!
LRESULT COverviewListBox::OnGetFieldList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT COverviewListBox::OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT COverviewListBox::OnGetReceiverList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookOverviewListBox

WNDPROC CAddressBookOverviewListBox::m_pfnSuperWndProc = NULL;

CAddressBookOverviewListBox::CAddressBookOverviewListBox()
{
	m_pDatabaseCursor = NULL;
	m_nLineHeight = -1;
	m_nCharWidth = -1;
	m_dwFirstRecord = 0;
	m_dwRecordsShowing = 0;
	m_dwFullRecordsShowing = 0;
	m_dwMaximumRecord = 0;
	m_nSplitterIndex = -1;
	m_hFont = NULL;
	m_nEditIndex = -1;
	m_dwRecordModifiedStart = (DWORD)-1;
	m_dwRecordModifiedEnd = (DWORD)-1;
	m_fAddingNewRecord = FALSE;
}

CAddressBookOverviewListBox::~CAddressBookOverviewListBox()
{
	DeleteFont();
}

void CAddressBookOverviewListBox::DeleteFont(void)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

BEGIN_MESSAGE_MAP(CAddressBookOverviewListBox, CWnd)
	//{{AFX_MSG_MAP(CAddressBookOverviewListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EDITOR_ACTIVATED, OnEditorActivated)
	ON_MESSAGE(WM_EDITOR_DEACTIVATED, OnEditorDeactivated)
	ON_MESSAGE(WM_GET_FIELD_LIST, OnGetFieldList)
	ON_MESSAGE(WM_GET_DATABASE_TABS, OnGetDatabaseTabs)
	ON_MESSAGE(WM_GET_RECEIVER_LIST, OnGetReceiverList)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

void CAddressBookOverviewListBox::SetDatabaseCursor(CFlatFileDatabaseCursor* pCursor)
{
	// Save the new cursor.
	m_pDatabaseCursor = pCursor;

	// The saved positions depend on the fields, so any current positions are invalid.
	m_Positions.DeleteAll();

	// Initialize some font and metric information.
	CDC* pDC = GetDC();
	if (pDC != NULL)
	{
		CFont* pFont = GetFont();
		if (pFont != NULL)
		{
			m_List.SetFont(pFont);
	
			CFont* pOldFont = pDC->SelectObject(pFont);
			if (pOldFont != NULL)
			{
				TEXTMETRIC tm;
				if (pDC->GetTextMetrics(&tm))
				{
					m_nLineHeight = tm.tmHeight;
					m_nCharWidth = tm.tmAveCharWidth;
				}
					
				pDC->SelectObject(pOldFont);
			}
		}

		// Initialize for the new cursor.
		if (m_pDatabaseCursor == NULL)
		{
			m_SearchCursor.Detach();
		}
		else
		{
			// Initialize the Search cursor.
			m_SearchCursor.SetCursor(*m_pDatabaseCursor);
	
			// Build the new field positions.
			DWORD dwFieldCount = m_pDatabaseCursor->GetFieldCount();
			for (DWORD dwField = 0; dwField < dwFieldCount; dwField++)
			{
				AddField(dwField, *pDC);
			}
		}

		ReleaseDC(pDC);
		pDC = NULL;
	}

	// Redraw ourselves.
	Invalidate();
}

// Helper method for adding a field to the position array. The field is
// added to the end of the array with a default width.
void CAddressBookOverviewListBox::AddField(DWORD dwField, CDC& dc)
{
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		TRY
		{
			int nWidth = -1;
			CFlatFileDatabaseField* pField = NULL;

			pField = pCursor->GetField(dwField);
			if (pField != NULL)
			{
				// Don't list Blob fields or those which cannot be edited.
				if ((pField->GetType() != FFT_Blob)
				 && (FIELD_USER_DATA_EDITOR(pField->GetDescription()->GetUserData()) != FIELD_EDIT_NONE))
				{
					// Compute the default size for the field. Eventually, we could get some saved
					// size at this point if we start saving and restoring the field width information.

					// Get the size of the name text.
					nWidth = Util::GetTextExtent(&dc, pField->GetName()).cx;

					// Adjust width based on size of the value text.
					int nValueWidth = 0;

					switch (pField->GetType())
					{
						case FFT_String:
						{
							nValueWidth = (int)(((pField->GetSize()*2)/3)*GetCharWidth());
							break;
						}
						case FFT_Counter32:
						case FFT_Signed32:
						case FFT_Unsigned32:
						{
							nValueWidth = 8*GetCharWidth();
							break;
						}

						// Everything else gets 0.
					}

					// Get max of name and value widths.
					if (nWidth < nValueWidth)
					{
						nWidth = nValueWidth;
					}

				}
			}

			// Add field if we have a proper width.
			if (nWidth >= 0)
			{
				// Add in minimal padding.
				nWidth += COverviewPositionArray::MinimumPadding*2;

				// Create the editor for the given field.
				CAddressBookFieldEditor* pEditor = NULL;
				if (pField != NULL)
				{
					pEditor = CAddressBookFieldEditor::Create(pField, &m_List);
				}

				if (pEditor != NULL)
				{
					pEditor->BringToTop();
					pEditor->Show();
				}

				// Add field to array.
				m_Positions.AddField(dwField, nWidth, pEditor);
			}
		}
		END_TRY
	}
}

void CAddressBookOverviewListBox::UpdateScrollBars(void)
{
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	m_dwMaximumRecord = (pCursor != NULL) ? pCursor->GetRecordCount() : 0;
	if (m_dwFirstRecord > m_dwMaximumRecord-(m_dwFullRecordsShowing-1))
	{
		if (m_dwMaximumRecord <= m_dwFullRecordsShowing-1)
		{
			m_dwFirstRecord = 0;
		}
		else
		{
			m_dwFirstRecord = m_dwMaximumRecord-(m_dwFullRecordsShowing-1);
		}
	}
	
	CRect crList;
	m_List.GetClientRect(crList);
	
	SCROLLINFO si;

	// Do vertical scroll bar.
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = m_dwMaximumRecord;
	si.nPos = m_dwFirstRecord;
	si.nPage = m_dwFullRecordsShowing;
	SetScrollInfo(SB_VERT, &si);
	
	// Do horizontal scroll bar.
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE;
	si.nMin = 0;
	si.nMax = GetPositions()->GetTotalWidth()-1;
	si.nPage = crList.Width();
	SetScrollInfo(SB_HORZ, &si);
}

int CAddressBookOverviewListBox::GetHorizontalOffset(void)
{
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	return si.nPos;
}

void CAddressBookOverviewListBox::Edit(int nIndex /*=-1*/)
{
	if ((nIndex == -1) && (m_nEditIndex != -1))
	{
		// The caller requested an arbitrary index and we're already editing.
		return;
	}

	if ((nIndex != -1) && (nIndex == m_nEditIndex))
	{
		// The caller requested the index that we're already editing.
		return;
	}

	if (nIndex == -1)
	{
		nIndex = 1;
	}

	// Start up the new editor.
	COverviewPositionArray* pPositions = GetPositions();
	if (nIndex < pPositions->GetSize())
	{
		CAddressBookFieldEditor* pEditor = pPositions->GetEditor(nIndex);
		if (pEditor != NULL)
		{
			pEditor->Reset();
			pEditor->SetFocus();
			m_nEditIndex = nIndex;
			Reframe(FALSE, TRUE);
		}
	}
}

BOOL CAddressBookOverviewListBox::IsModified(void)
{
	COverviewPositionArray* pPositions = GetPositions();
	for (int i = 0; i < pPositions->GetSize(); i++)
	{
		CAddressBookFieldEditor* pEditor = pPositions->GetEditor(i);
		if (pEditor != NULL)
		{
			if (pEditor->IsModified())
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CAddressBookOverviewListBox::SetModified(BOOL fModified)
{
	COverviewPositionArray* pPositions = GetPositions();
	for (int i = 0; i < pPositions->GetSize(); i++)
	{
		CAddressBookFieldEditor* pEditor = pPositions->GetEditor(i);
		if (pEditor != NULL)
		{
			pEditor->SetModified(fModified);
		}
	}
}

void CAddressBookOverviewListBox::Save(void)
{
	COverviewPositionArray* pPositions = GetPositions();
	for (int i = 0; i < pPositions->GetSize(); i++)
	{
		CAddressBookFieldEditor* pEditor = pPositions->GetEditor(i);
		if (pEditor != NULL)
		{
			pEditor->Save();
		}
	}
}

void CAddressBookOverviewListBox::Refresh(void)
{
	COverviewPositionArray* pPositions = GetPositions();
	for (int i = 0; i < pPositions->GetSize(); i++)
	{
		CAddressBookFieldEditor* pEditor = pPositions->GetEditor(i);
		if (pEditor != NULL)
		{
			pEditor->Refresh();
		}
	}
}

// Refresh a particular item.
void CAddressBookOverviewListBox::RefreshItem(int nItem /*=-1*/)
{
	if (nItem < 0)
	{
		nItem = m_nEditIndex;
	}

	COverviewPositionArray* pPositions = GetPositions();
	if ((nItem >= 0) && (nItem < pPositions->GetSize()))
	{
		CAddressBookFieldEditor* pEditor = pPositions->GetEditor(nItem);
		if (pEditor != NULL)
		{
			pEditor->Refresh();
		}
	}
}

int CAddressBookOverviewListBox::GetEditorIndex(CAddressBookFieldEditor* pEditor)
{
	COverviewPositionArray* pPositions = GetPositions();
	for (int i = 0; i < pPositions->GetSize(); i++)
	{
		if (pEditor == pPositions->GetEditor(i))
		{
			return i;
		}
	}

	return -1;
}

DWORD CAddressBookOverviewListBox::GetFieldCount(void)
{
	return (DWORD)(GetPositions()->GetSize());
}

CFlatFileDatabaseField* CAddressBookOverviewListBox::GetField(DWORD dwField)
{
	CFlatFileDatabaseField* pField = NULL;
	dwField = GetPositions()->GetField((int)dwField);
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		pField = pCursor->GetField(dwField);
	}
	return pField;
}

void CAddressBookOverviewListBox::BeforeRecordChange(void)
{
}

void CAddressBookOverviewListBox::RecordModified(DWORD dwStartPosition, DWORD dwEndPosition)
{
	// The record was modified, remember the details for Reframe().
	m_dwRecordModifiedStart = dwStartPosition;
	m_dwRecordModifiedEnd = dwEndPosition;
}

void CAddressBookOverviewListBox::AfterRecordChange(void)
{
	Reframe();
}

void CAddressBookOverviewListBox::ActivateView(BOOL fActive /*=TRUE*/)
{
	if (fActive)
	{
		Reframe();
		Refresh();
	}
}

void CAddressBookOverviewListBox::Reframe(BOOL fVertical /*=TRUE*/, BOOL fHorizontal /*=FALSE*/)
{
	BOOL fInvalidateListBox = FALSE;

	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		// Check if the number of records has changed. If so, force a vertical
		// reframe and a list box invalidation.
		DWORD dwNewMaximumRecord = pCursor->GetRecordCount();
		if (m_dwMaximumRecord != dwNewMaximumRecord)
		{
			UpdateScrollBars();
			fVertical = TRUE;
			fInvalidateListBox = TRUE;
		}

		// Check if we are adding a new record (current position is -1).
		m_fAddingNewRecord = (pCursor->GetPosition() == (DWORD)-1);

		// Do vertical reframe.
		if (fVertical)
		{
			DWORD dwCurrentRecord = m_fAddingNewRecord ? m_dwMaximumRecord : pCursor->GetPosition();

			// Check if current record is already showing.
			if ((dwCurrentRecord < m_dwFirstRecord)
			 || (dwCurrentRecord >= m_dwFirstRecord+m_dwFullRecordsShowing))
			{
				// Compute new first record for list box.
				DWORD dwNewFirstRecord;
				DWORD dwCenter = (m_dwFullRecordsShowing+1)/2;

				// Make sure we don't scroll too far.
				if (dwCurrentRecord <= dwCenter)
				{
					dwNewFirstRecord = 0;
				}
				else
				{
					dwNewFirstRecord = dwCurrentRecord-dwCenter;

					if (m_dwMaximumRecord <= m_dwFullRecordsShowing-1)
					{
						dwNewFirstRecord = 0;
					}
					else
					{
						if (dwNewFirstRecord > m_dwMaximumRecord-(m_dwFullRecordsShowing-1))
						{
							dwNewFirstRecord = m_dwMaximumRecord-(m_dwFullRecordsShowing-1);
						}
					}
				}

				if (m_dwFirstRecord != dwNewFirstRecord)
				{
					m_dwFirstRecord = dwNewFirstRecord;
					SetScrollPos(SB_VERT, (int)m_dwFirstRecord);
					fInvalidateListBox = TRUE;
				}
			}
			else
			{
				if (m_dwFirstRecord > m_dwMaximumRecord-(m_dwFullRecordsShowing-1))
				{
					DWORD dwNewFirstRecord;
					if (m_dwMaximumRecord <= m_dwFullRecordsShowing-1)
					{
						dwNewFirstRecord = 0;
					}
					else
					{
						dwNewFirstRecord = m_dwMaximumRecord-(m_dwFullRecordsShowing-1);
					}

					if (m_dwFirstRecord != dwNewFirstRecord)
					{
						m_dwFirstRecord = dwNewFirstRecord;
						SetScrollPos(SB_VERT, (int)m_dwFirstRecord);
						fInvalidateListBox = TRUE;
					}
				}

				// The current record is still in range, but we may need to
				// move the selection arrow.
				CRect crSelect;
				GetClientRect(crSelect);
				crSelect.top = GetHeaderHeight();
				crSelect.right = GetSelectWidth();
				InvalidateRect(crSelect);
			}
		}

		// Do Horizontal reframe.
		if (fHorizontal)
		{
			COverviewPositionArray* pPositions = GetPositions();
			if ((m_nEditIndex >= 0) && (m_nEditIndex < pPositions->GetSize()))
			{
				int nOffset = pPositions->GetOffset(m_nEditIndex)-GetHorizontalOffset();
				int nWidth = pPositions->GetWidth(m_nEditIndex);

				CRect crList;
				m_List.GetClientRect(crList);
				if ((nOffset < 0) || (nOffset+nWidth > crList.Width()))
				{
					nOffset = (crList.Width()-nWidth)/2;
					if (nOffset < 0)
					{
						nOffset = 0;
					}
					SetScrollPos(SB_HORZ, pPositions->GetOffset(m_nEditIndex)-nOffset);
					fInvalidateListBox = TRUE;
				}
			}
		}
	}

	PositionEditors();

	if (fInvalidateListBox)
	{
		Invalidate();
	}
	else
	{
		// Invalidate all the list box items in the range.
		DWORD dwStart = m_dwRecordModifiedStart;
		DWORD dwEnd = m_dwRecordModifiedEnd;

		// If they're both -1, then there's nothing to do.
		if ((dwStart != (DWORD)-1) || (dwEnd != (DWORD)-1))
		{
			// Changed -1 into maximum record.
			if (dwStart == (DWORD)-1)
			{
				dwStart = m_dwMaximumRecord;
			}
			if (dwEnd == (DWORD)-1)
			{
				dwEnd = m_dwMaximumRecord;
			}

			// Normalize start and end.
			if (dwStart > dwEnd)
			{
				DWORD dwTemp = dwStart;
				dwStart = dwEnd;
				dwEnd = dwTemp;
			}

			for (DWORD dwItem = dwStart; dwItem <= dwEnd; dwItem++)
			{
				m_List.RedrawItem((int)(dwItem-m_dwFirstRecord));
			}
		}
	}

	// These are only valid one time through.
	m_dwRecordModifiedStart = (DWORD)-1;
	m_dwRecordModifiedEnd = (DWORD)-1;
}

void CAddressBookOverviewListBox::PositionEditors(void)
{
	COverviewPositionArray* pPositions = GetPositions();
	DWORD dwItem = (DWORD)-1;

	// Get the index of the selected record.
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		DWORD dwCurrentRecord = m_fAddingNewRecord ? m_dwMaximumRecord : pCursor->GetPosition();
		if (dwCurrentRecord != (DWORD)-1)
		{
			if ((dwCurrentRecord >= m_dwFirstRecord) && (dwCurrentRecord < m_dwFirstRecord+m_dwRecordsShowing))
			{
				// The currently selected record is in view. Move the editors on top of it.
				dwItem = dwCurrentRecord-m_dwFirstRecord;
			}
		}

		// Show the editors if the current selected record is visible.
		if (dwItem != (DWORD)-1)
		{
			// Get the position of the item.
			CRect crItem;
			if (m_List.GetItemRect((int)dwItem, crItem))
			{
				// Run through all the editors and make sure they're hidden.
				for (int i = 0; i < pPositions->GetSize(); i++)
				{
					// Make sure the field has an editor.
					CAddressBookFieldEditor* pEditor = pPositions->GetEditor(i);
					if (pEditor != NULL)
					{
						// Compute the position of the editor.
						CRect crEdit;

						crEdit.top = crItem.top;
						crEdit.bottom = crItem.bottom;
						crEdit.left = pPositions->GetOffset(i)-GetHorizontalOffset();
						crEdit.right = crEdit.left+pPositions->GetWidth(i);

						// Magic numbers.
						crEdit.top += 3;
						crEdit.bottom -= 3;
						crEdit.left += 3;
						crEdit.right -= 3;

						if (!crEdit.IsRectEmpty())
						{
							pEditor->Move(crEdit);
							pEditor->BringToTop();
							pEditor->Show();
						}
						else
						{
							pEditor->Show(FALSE);
						}
					}
				}
			}
		}
		else
		{
			// Run through all the editors and make sure they're hidden.
			for (int i = 0; i < pPositions->GetSize(); i++)
			{
				CAddressBookFieldEditor* pEditor = pPositions->GetEditor(i);
				if (pEditor != NULL)
				{
					pEditor->Show(FALSE);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookOverviewListBox message handlers

// Save the font to use.
LRESULT CAddressBookOverviewListBox::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	HFONT hNewFont = NULL;
	if (wParam != 0)
	{
		LOGFONT LogFont;
		if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
		{
			hNewFont = CreateFontIndirect(&LogFont);
		}
	}
		
	DeleteFont();
	m_hFont = hNewFont;
	
	if (LOWORD(lParam) != 0)
	{
		Invalidate();
	}
	
	return 0;
}

// Return the font to use.
LRESULT CAddressBookOverviewListBox::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(UINT)m_hFont;
}

int CAddressBookOverviewListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont* pFont = GetParent()->GetFont();
	if (pFont != NULL)
	{
		SetFont(pFont);

		CDC* pDC = GetDC();
		if (pDC != NULL)
		{
			CFont* pOldFont = pDC->SelectObject(pFont);
			if (pOldFont != NULL)
			{
				TEXTMETRIC tm;
				if (pDC->GetTextMetrics(&tm))
				{
					m_nLineHeight = tm.tmHeight;
					m_nCharWidth = tm.tmAveCharWidth;
				}
				
				pDC->SelectObject(pOldFont);
			}
	
			ReleaseDC(pDC);
			pDC = NULL;
		}
	}
	
	m_List.CreateControl(WS_CHILD | /*WS_CLIPSIBLINGS |*/ WS_VISIBLE | LBS_OWNERDRAWVARIABLE | LBS_NOINTEGRALHEIGHT | LBS_WANTKEYBOARDINPUT, this, 1234);

	m_SplitterCursor.SetWindow(this);

	return 0;
}

void CAddressBookOverviewListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	INHERITED::OnLButtonDown(nFlags, point);

	if (m_SplitterCursor.IsShowing())
	{
		CRect crScreen(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		ScreenToClient(crScreen);
		m_SplitterCursor.StartDrag(point.x, GetPositions()->GetOffset(m_nSplitterIndex)-GetHorizontalOffset()+GetSelectWidth()-point.x, crScreen.left, crScreen.right);
	}
	else
	{
		// If the mouse click happened over one of the records, tell our parent to select that record.
		CRect crRect;
		m_List.GetWindowRect(crRect);
		ScreenToClient(crRect);
		crRect.left = 0;		// include the select area
		if (crRect.PtInRect(point))
		{
			UINT uIndex = (point.y-crRect.top)/GetItemHeight();
			if (m_dwFirstRecord+uIndex < m_dwMaximumRecord)
			{
				GetParent()->SendMessage(WM_SELECT_RECORD, (WPARAM)0, (LPARAM)(m_dwFirstRecord+uIndex));
			}
			else
			{
				Util::SendWmCommand(GetParent(), IDC_ADD_WITHOUT_EDIT, NULL, 1);
			}

			// We have changed records and we want to have the mouse click look like it
			// occurred in the editor of the new field. So, look for an editor (child of
			// list box) which contains the point. If one is found, send the mouse down
			// message to it with the point translated into its client coordinates.
			CPoint cpPoint(point);
			ClientToScreen(&cpPoint);
			m_List.ScreenToClient(&cpPoint);
			CWnd* pChild = m_List.ChildWindowFromPoint(cpPoint);
			if ((pChild != NULL) && (pChild != &m_List))
			{
				m_List.ClientToScreen(&cpPoint);
				pChild->ScreenToClient(&cpPoint);
				pChild->SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, *((LPARAM*)&cpPoint));
			}
			else
			{
				// Could not find a window. What is the user trying to click on?
				// Just set the focus to us.
				SetFocus();
			}
		}
		else
		{
			// Check if the click happened in that wierd little rectangle at the intersection
			// of the header row and the select column. We do a reframe if the user clicks in
			// this spot.
			GetClientRect(crRect);
			crRect.right = GetSelectWidth();
			crRect.bottom = GetHeaderHeight();
			if (crRect.PtInRect(point))
			{
				// If we're editing, do a full reframe.
				Reframe(TRUE, TRUE);
			}
		}
	}
}

void CAddressBookOverviewListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	INHERITED::OnLButtonUp(nFlags, point);

	if (m_SplitterCursor.IsDragging())
	{
		m_SplitterCursor.EndDrag(point.x);
		if (GetPositions()->GetOffset(m_nSplitterIndex) != m_SplitterCursor.GetX())
		{
			GetPositions()->SetOffset(m_nSplitterIndex, m_SplitterCursor.GetX()+GetHorizontalOffset()-GetSelectWidth());
			UpdateScrollBars();
			PositionEditors();
			Invalidate();
		}
	}
}

void CAddressBookOverviewListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CAddressBookOverviewListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	INHERITED::OnMouseMove(nFlags, point);

	// If the splitter cursor is dragging, continue the drag.
	if (m_SplitterCursor.IsDragging())
	{
		m_SplitterCursor.Drag(point.x);
	}
	else
	{
		// Check if the mouse is over a splitter bar. The variable m_sSplitterIndex
		// is used to record the index of the splitter bar found. It will be -1
		// the cursor is not over a splitter bar.
		m_nSplitterIndex = -1;
		COverviewPositionArray* pPositions = GetPositions();
		int nOffset = -GetHorizontalOffset()+GetSelectWidth();

		// Check each position.
		for (int i = 0; i <= pPositions->GetSize(); i++)
		{
			if ((nOffset > GetSelectWidth())
			 && (point.x >= nOffset-CSplitterCursor::Padding)
			 && (point.x <= nOffset+CSplitterCursor::Padding))
			{
				// Over a splitter bar.
				m_SplitterCursor.StartShow();
				m_nSplitterIndex = i;
				break;
			}
			if (i < pPositions->GetSize())
			{
				nOffset += pPositions->GetWidth(i);
			}
		}

		if (m_nSplitterIndex == -1)
		{
			// Not over a splitter bar, stop showing the splitter cursor.
			m_SplitterCursor.EndShow();
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
	}
}

UINT CAddressBookOverviewListBox::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode();
	if (IsEditing())
	{
		uRet |= DLGC_WANTALLKEYS;
	}
	else
	{
		if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_UP, VK_DOWN, VK_NEXT, VK_PRIOR, 0))
		{
			uRet |= DLGC_WANTMESSAGE;
		}
	}
	return uRet;
}

void CAddressBookOverviewListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_TAB:
		{
			if (::GetKeyState(VK_SHIFT) < 0)
			{
				// Shift key is down. Do back tab.
				BackField(TRUE);
			}
			else
			{
				// Shift key is not down. Do forward tab.
				NextField(TRUE);
			}
			break;
		}
		case VK_RETURN:
		{
			NextField(TRUE);
			break;
		}
		case VK_ESCAPE:
		{
			RefreshItem();
			break;
		}
		case VK_LEFT:
		{
			BackField(FALSE);
			break;
		}
		case VK_RIGHT:
		{
			NextField(FALSE);
			break;
		}
		case VK_UP:
		{
			MoveRecord(-1);
			break;
		}
		case VK_DOWN:
		{
			MoveRecord(1);
			break;
		}
		case VK_PRIOR:
		{
			BackPage();
			break;
		}
		case VK_NEXT:
		{
			NextPage();
			break;
		}
		default:
		{
			INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CAddressBookOverviewListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	INHERITED::OnChar(nChar, nRepCnt, nFlags);
}

void CAddressBookOverviewListBox::BackField(BOOL fMoveOffRecord)
{
	int nIndex = m_nEditIndex;
	if (nIndex > 0)
	{
		Edit(nIndex-1);
	}
	else
	{
		if (fMoveOffRecord)
		{
			if (MoveRecord(-1))
			{
				Edit(GetPositions()->GetSize()-1);
			}
		}
	}

	Reframe(FALSE, TRUE);
}

void CAddressBookOverviewListBox::NextField(BOOL fMoveOffRecord)
{
	int nIndex = m_nEditIndex;
	if (nIndex < GetPositions()->GetSize()-1)
	{
		Edit(nIndex+1);
	}
	else
	{
		if (fMoveOffRecord)
		{
			if (MoveRecord(1))
			{
				Edit(0);
			}
		}
	}

	Reframe(FALSE, TRUE);
}

void CAddressBookOverviewListBox::BackPage(void)
{
	MoveRecord(-(long)m_dwFullRecordsShowing);
}

void CAddressBookOverviewListBox::NextPage(void)
{
	MoveRecord((long)m_dwFullRecordsShowing);
}

BOOL CAddressBookOverviewListBox::MoveRecord(long lDelta)
{
	BOOL fMoved = FALSE;

	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		DWORD dwCurrentRecord = m_fAddingNewRecord ? m_dwMaximumRecord : pCursor->GetPosition();
		if (dwCurrentRecord != (DWORD)-1)
		{
			DWORD dwNewRecord;
			if ((lDelta < 0) && (dwCurrentRecord < (DWORD)(-lDelta)))
			{
				dwNewRecord = 0;
			}
			else
			{
				dwNewRecord = dwCurrentRecord+lDelta;
			}

			if (dwNewRecord != dwCurrentRecord)
			{
				if (dwNewRecord < m_dwMaximumRecord)
				{
					GetParent()->SendMessage(WM_SELECT_RECORD, (WPARAM)0, (LPARAM)dwNewRecord);
				}
				else
				{
					Util::SendWmCommand(GetParent(), IDC_ADD, NULL, 1);
				}
			}

			fMoved = TRUE;
		}
	}

	return fMoved;
}

BOOL CAddressBookOverviewListBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	if (!m_List.IsWindowVisible())
	{
		CRect crBackground;
		GetClientRect(&crBackground);
		crBackground.top += GetHeaderHeight();
		crBackground.left += GetSelectWidth();
		if (!crBackground.IsRectEmpty())
		{
			Util::FillRectangleWithColor(*pDC, crBackground, GetSysColor(COLOR_WINDOW));
		}
	}

	return TRUE;
}

BOOL CAddressBookOverviewListBox::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest != HTCLIENT)
	{
		return INHERITED::OnSetCursor(pWnd, nHitTest, message);
	}

	return pWnd == this;
#if 0
	const MSG* pMsg = GetCurrentMessage();

	return pWnd == this;

	if (pWnd == this)
	{
		return TRUE;
	}

	if (pWnd != this)
	{
		return FALSE;
	}
	if ((nHitTest == HTCLIENT) && (m_SplitterCursor.IsShowing()))
	{
		return TRUE;
	}

	return INHERITED::OnSetCursor(pWnd, nHitTest, message);
#endif
}

void CAddressBookOverviewListBox::OnSize(UINT nType, int cx, int cy) 
{
	// Do the normal size.
	INHERITED::OnSize(nType, cx, cy);

	// Size the list box to the new area.
	for (;;)
	{
		CRect crClient;
		GetClientRect(crClient);
		
		CRect crList(crClient);
		crList.top += GetHeaderHeight();
		crList.left += GetSelectWidth();
	
		// Figure out how many items we should have and recreate them.
		m_List.SetRedraw(FALSE);
		m_List.ResetContent();
		m_dwRecordsShowing = (DWORD)(((crList.Height()-1)/GetItemHeight())+1);
		m_dwFullRecordsShowing = (DWORD)(crList.Height()/GetItemHeight());
		for (DWORD d = 0; d < m_dwRecordsShowing; d++)
		{
			COverviewListBoxItem* pItem = NULL;
			TRY
			{
				pItem = new COverviewListBoxItem(this);
			}
			END_TRY
	
			if (pItem != NULL)
			{
				m_List.AddString((LPCSTR)pItem);
			}
		}
		
		// Move the window and redraw it.
		m_List.MoveWindow(crList);
	
		// Update the scroll bars.
		UpdateScrollBars();
		
		// Reposition the editors since things may have moved.
		PositionEditors();

		// Check if the scroll bars moved. This can change the shape of the
		// client area and thus cause us to need to recalculate the contents
		// of the list box. A semi-quick analysis suggests that this loop can't
		// get into state where its loops forever, I hope this is true.
		
		CRect crNewClient;
		GetClientRect(crNewClient);
		
		if (crClient == crNewClient)
		{
			m_List.SetRedraw(TRUE);
			m_List.Invalidate();
			break;
		}
	}
}

void CAddressBookOverviewListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Preload some useful things.
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		DWORD dwNewFirstRecord = m_dwFirstRecord;
		switch (nSBCode)
		{
			case SB_LINEUP:
			{
				// Move up a record.
				if (dwNewFirstRecord > 0)
				{
					dwNewFirstRecord--;
				}
				break;
			}
			case SB_LINEDOWN:
			{
				// Move down a record.
				if (dwNewFirstRecord < m_dwMaximumRecord)
				{
					dwNewFirstRecord++;
				}
				break;
			}
			case SB_PAGEUP:
			{
				// Move up a page of records.
				if (dwNewFirstRecord > m_dwFullRecordsShowing)
				{
					dwNewFirstRecord -= m_dwFullRecordsShowing;
				}
				else
				{
					dwNewFirstRecord = 0;
				}
				break;
			}
			case SB_PAGEDOWN:
			{
				// Move down a page of records.
				if (dwNewFirstRecord+m_dwFullRecordsShowing < m_dwMaximumRecord)
				{
					dwNewFirstRecord += m_dwFullRecordsShowing;
				}
				else
				{
					dwNewFirstRecord = m_dwMaximumRecord;
				}
				break;
			}
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			{
				// Move to the position the user has selected.
				dwNewFirstRecord = (DWORD)nPos;
				break;
			}
		}

		// Make sure we don't scroll too far.
		if (m_dwMaximumRecord < m_dwFullRecordsShowing-1)
		{
			dwNewFirstRecord = 0;
		}
		else
		{
			if (dwNewFirstRecord > m_dwMaximumRecord-(m_dwFullRecordsShowing-1))
			{
				dwNewFirstRecord = m_dwMaximumRecord-(m_dwFullRecordsShowing-1);
			}
		}

		if (m_dwFirstRecord != dwNewFirstRecord)
		{
			m_dwFirstRecord = dwNewFirstRecord;
			SetScrollPos(SB_VERT, (int)m_dwFirstRecord);
			PositionEditors();
			Invalidate();
		}
	}

	INHERITED::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CAddressBookOverviewListBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (nSBCode == SB_THUMBTRACK)
	{
		m_List.ShowWindow(SW_HIDE);
	}

	// Preload some useful things.
	CRect crList;
	GetClientRect(crList);
	COverviewPositionArray* pPositions = GetPositions();
	int nCurrentPosition = GetHorizontalOffset();
	int nPosition = -1;

	switch (nSBCode)
	{
		case SB_LINEUP:
		{
			// Move to the previous index.
			nPosition = pPositions->GetOffset(pPositions->GetIndex(nCurrentPosition-1));
			break;
		}
		case SB_LINEDOWN:
		{
			// Move to the next index.
			nPosition = pPositions->GetOffset(pPositions->GetIndex(nCurrentPosition)+1);
			break;
		}
		case SB_PAGEUP:
		{
			// Get the offset we have to keep in sight.
			int nMustSeeIndex = pPositions->GetIndex(nCurrentPosition-1);
			int nMustSeeOffset = pPositions->GetOffset(nMustSeeIndex+1);

			// Search for the leftmost index which still shows the "must see" offset.
			int nIndex = nMustSeeIndex;
			while (nIndex >= 0)
			{
				// Check if index leaves "must see" offset in sight.
				int nOffset = pPositions->GetOffset(nIndex);
				if ((nMustSeeOffset-pPositions->GetOffset(nIndex)) > crList.Width())
				{
					if (nIndex < nMustSeeIndex)
					{
						nIndex++;
						break;
					}
				}

				// Try the next index.
				nIndex--;
			}

			nPosition = pPositions->GetOffset(nIndex);
			break;
		}
		case SB_PAGEDOWN:
		{
			// Get the last, partially visible index. This will become the new first index.
			int nIndex = pPositions->GetIndex(nCurrentPosition+crList.Width());

			// If the field is really wide, just move to the next one.
			if (nIndex == pPositions->GetIndex(nCurrentPosition))
			{
				nIndex++;
			}

			nPosition = pPositions->GetOffset(nIndex);
			break;
		}
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
		{
			// Move to the position the user has selected.
			nPosition = nPos;
			break;
		}
	}

	// If we have a new position, move to it and redraw the list box.
	if ((nPosition >= 0) && (nPosition != nCurrentPosition))
	{
		SetScrollPos(SB_HORZ, nPosition);
		PositionEditors();
		Invalidate();
	}

	if (nSBCode == SB_THUMBPOSITION)
	{
		m_List.ShowWindow(SW_SHOW);
	}

//	INHERITED::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CAddressBookOverviewListBox::OnEditorActivated(WPARAM wParam, LPARAM lParam)
{
	CAddressBookFieldEditor* pEditor = (CAddressBookFieldEditor*)lParam;
	if (pEditor != NULL)
	{
		m_nEditIndex = GetEditorIndex(pEditor);

		// Notify our parent (the address book) that an editor was activated.
		// This message is used to allow the user to go into "Add New Name" mode
		// by simply clicking on a field in an empty address book.
		const MSG* pMsg = GetCurrentMessage();
		return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	}

	return 0;
}

LRESULT CAddressBookOverviewListBox::OnEditorDeactivated(WPARAM wParam, LPARAM lParam)
{
	m_nEditIndex = -1;
	return 0;
}

// One of the editors wants to get a pointer to the list of unique field values for its field.
// To do this, we pass the message on to our parent and hope they are listening!
LRESULT CAddressBookOverviewListBox::OnGetFieldList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT CAddressBookOverviewListBox::OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

LRESULT CAddressBookOverviewListBox::OnGetReceiverList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

void CAddressBookOverviewListBox::OnPaint() 
{
	CPaintDC dc(this);

	CPalette* pOldPal = dc.SelectPalette(pOurPal, FALSE);
	dc.RealizePalette();

	CFont* pOldFont = dc.SelectObject(GetParent()->GetFont());
	if (pOldFont != NULL)
	{
		CRect crClient;
		GetClientRect(crClient);

		CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();

		// Draw the corner box.
		CRect crSelect(crClient);
		crSelect.right = crSelect.left+GetSelectWidth();
		crSelect.bottom = crSelect.top+GetHeaderHeight();
		if (pCursor != NULL)
		{
			FillRectangleWithColor(dc, crSelect, FIELD_BACKGROUND_COLOR, GetSysColor(COLOR_BTNSHADOW));
		}
		else
		{
			Util::FillRectangleWithColor(dc, crSelect, GetSysColor(COLOR_BTNFACE));
		}
		dc.ExcludeClipRect(crSelect);

		// Draw rest of the select boxes
		crSelect.OffsetRect(0, GetHeaderHeight());
		crSelect.bottom = crSelect.top+GetItemHeight();

		if (pCursor != NULL)
		{
			DWORD dwRecordCount = pCursor->GetRecordCount();
			for (DWORD dwSelect = 0; dwSelect < m_dwRecordsShowing; dwSelect++)
			{
				DWORD dwRecord = dwSelect+m_dwFirstRecord;
				if (dwRecord > dwRecordCount)
				{
					// No more records.
					break;
				}

				// Draw the select background.
				FillRectangleWithColor(dc, crSelect, FIELD_BACKGROUND_COLOR, GetSysColor(COLOR_BTNSHADOW));

				// Draw the current record indicator.
				DWORD dwCurrentRecord = m_fAddingNewRecord ? m_dwMaximumRecord : pCursor->GetPosition();
				if (dwRecord == dwCurrentRecord)
				{
					POINT Point[3];
					Point[0].x = crSelect.left+5;
					Point[1].x = crSelect.right-1-5;
					Point[2].x = crSelect.left+5;
					Point[0].y = crSelect.top+4;
					Point[1].y = (crSelect.top+crSelect.bottom-1)/2;
					Point[2].y = Point[1].y+(Point[1].y-Point[0].y);

					CBrush cbFill;
					cbFill.CreateStockObject(BLACK_BRUSH);
					CBrush* pOldBrush = dc.SelectObject(&cbFill);

					CPen cpBorder;
					cpBorder.CreateStockObject(BLACK_PEN);
					CPen* pOldPen = dc.SelectObject(&cpBorder);

					dc.Polygon(&(Point[0]), sizeof(Point)/sizeof(Point[0]));

					dc.SelectObject(pOldPen);
					dc.SelectObject(pOldBrush);
				}

				// Advance the select rectangle.
				crSelect.OffsetRect(0, GetItemHeight());
			}
		}

		// Erase any empty space at the bottom of the select column.
		crSelect.bottom = crClient.bottom;
		if (!crSelect.IsRectEmpty())
		{
			Util::FillRectangleWithColor(dc, crSelect, GetSysColor(COLOR_BTNFACE));
		}
		
		// Draw the header.
		CPoint cpOldViewportOrigin = dc.OffsetViewportOrg(-GetHorizontalOffset()+GetSelectWidth(), 0);

		CRect crHeader(crClient);
		crHeader.bottom = crHeader.top+GetHeaderHeight();

		COverviewPositionArray* pPositions = GetPositions();
		for (int i = 0; i < pPositions->GetSize(); i++)
		{
			CRect crField(crHeader);
			crField.left = pPositions->GetOffset(i);
			crField.right = crField.left+pPositions->GetWidth(i);
			if (dc.RectVisible(crField))
			{
				FillRectangleWithColor(dc, crField, FIELD_BACKGROUND_COLOR, GetSysColor(COLOR_BTNSHADOW));
				crField.right--;
				crField.bottom--;

				CString csText;
				TRY
				{
					DWORD dwField = pPositions->GetField(i);
					csText = pCursor->GetField(dwField)->GetName();

					dc.SetBkMode(TRANSPARENT);
					dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
					Util::DrawText(&dc, csText, crField, DT_CENTER|DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
				}
				END_TRY
			}
		}

		// Fill any used space to the right of the header.
		CRect crField(crHeader);
		crField.left = pPositions->GetTotalWidth();
		crField.right += GetHorizontalOffset()-GetSelectWidth();
		if (!crField.IsRectEmpty())
		{
			Util::FillRectangleWithColor(dc, crField, GetSysColor(COLOR_BTNFACE));
		}

		dc.SetViewportOrg(cpOldViewportOrigin);
		dc.SelectObject(pOldFont);
	}

	if (pOldPal != NULL)
	{
		dc.SelectPalette(pOldPal, FALSE);
	}
}

#if 0
void CAddressBookOverviewListBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// Get default dimensions.
	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);

	// Make room for cool 3D border.
	((RECT*)lpncsp)->left += 2;
	((RECT*)lpncsp)->top += 2;
	((RECT*)lpncsp)->right -= 2;
	((RECT*)lpncsp)->bottom -= 2;
}

void CAddressBookOverviewListBox::OnNcPaint() 
{
	// Draw the scroll bars, etc.
	Default();

	// Draw our 3D border.
	CDC* pDC = GetWindowDC();
	if (pDC != NULL)
	{
		CRect crWindow;
		GetWindowRect(crWindow);
		int nWidth = crWindow.Width();
		int nHeight = crWindow.Height();

		// Draw the sunken 3D border:
		//
		//    D-----------DW
		//    |B---------BL|
		//    ||          ||
		//    ||          ||
		//    |B          ||
		//    DL----------L|
		//    W------------W


		CRect crSide;

		// D horizontal
		crSide.SetRect(0, 0, nWidth-1, 1);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNSHADOW));

		// D vertical
		crSide.SetRect(0, 1, 1, nHeight-1);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNSHADOW));

		// B horizontal
		crSide.SetRect(1, 1, nWidth-2, 2);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNTEXT));

		// B vertical
		crSide.SetRect(1, 2, 2, nHeight-2);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNTEXT));

		// W horizontal
		crSide.SetRect(0, nHeight-1, nWidth, nHeight);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// W vertical
		crSide.SetRect(nWidth-1, 0, nWidth, nHeight-1);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// L horizontal
		crSide.SetRect(1, nHeight-2, nWidth-1, nHeight-1);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNFACE));

		// L vertical
		crSide.SetRect(nWidth-2, 1, nWidth-1, nHeight-2);
		Util::FillRectangleWithColor(*pDC, crSide, GetSysColor(COLOR_BTNFACE));

		ReleaseDC(pDC);
		pDC = NULL;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
// CReceiverListBoxItem

CReceiverListBoxItem::CReceiverListBoxItem(CAddressBookReceiverListBox* pListBox)
{
	m_pListBox = pListBox;
}

CReceiverListBoxItem::~CReceiverListBoxItem()
{
}

void CReceiverListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if (pDrawItemStruct->itemID != -1)
	{
		if ((pDrawItemStruct->itemAction & (ODA_DRAWENTIRE|ODA_SELECT|ODA_FOCUS)) != 0)
		{
			// Paint the item.
			Paint(pList, pDrawItemStruct);
		}
	}
}

void CReceiverListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Get the DC and rectangle to use.
	CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);
	if (pDC != NULL)
	{
		pDC->SaveDC();
		
		CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();

		CRect crItem(pDrawItemStruct->rcItem);

		CFlatFileDatabaseCursor* pDatabaseCursor = m_pListBox->GetDatabaseCursor();
		if (pDatabaseCursor == NULL)
		{
			Util::FillRectangleWithColor(*pDC, crItem, GetSysColor(COLOR_BTNFACE));
		}
		else
		{
			DWORD dwNumber = m_pListBox->GetFirstRecord()+pDrawItemStruct->itemID;

			CFlatFileDatabaseCursor* pSearchCursor = m_pListBox->GetSearchCursor();

			if (dwNumber < pSearchCursor->GetRecordCount())
			{
				pSearchCursor->SetPosition(dwNumber);

				BOOL fIsReceiver = m_pListBox->IsReceiver(pSearchCursor);

				Util::FillRectangleWithColor(*pDC, crItem, fIsReceiver ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_WINDOW));

				CString csText;
				TRY
				{
					((CAddressBook*)(m_pListBox->GetParent()))->GetDisplayName(pSearchCursor, csText);

					CRect crText(crItem);
					crText.left += m_pListBox->GetSelectWidth()+3;
					crText.top += 3;
					crText.bottom -= 3;

					pDC->SetBkMode(TRANSPARENT);
					pDC->SetTextColor(fIsReceiver ? GetSysColor(COLOR_HIGHLIGHTTEXT) : GetSysColor(COLOR_WINDOWTEXT));
					Util::DrawText(pDC, csText, crText, DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
				}
				END_TRY

				CFlatFileDatabaseCursor* pDatabaseCursor = m_pListBox->GetDatabaseCursor();
				if (pDatabaseCursor != NULL)
				{
					DWORD dwCurrentRecord = pDatabaseCursor->GetPosition();
					if (dwNumber == dwCurrentRecord)
					{
						if (m_pListBox->GetSelectWidth() > 0)
						{
							CRect crSelect(
								crItem.left,
								crItem.top,
								crItem.left+m_pListBox->GetSelectWidth(),
								crItem.top+m_pListBox->GetItemHeight());

							POINT Point[3];
							Point[0].x = crSelect.left+4;
							Point[1].x = crSelect.right-1-4;
							Point[2].x = crSelect.left+4;
							Point[0].y = crSelect.top+3;
							Point[1].y = (crSelect.top+crSelect.bottom-1)/2;
							Point[2].y = Point[1].y+(Point[1].y-Point[0].y);

							CBrush cbFill;
							cbFill.CreateStockObject(BLACK_BRUSH);
							CBrush* pOldBrush = pDC->SelectObject(&cbFill);

							CPen cpBorder;
							cpBorder.CreateStockObject(WHITE_PEN);
							CPen* pOldPen = pDC->SelectObject(&cpBorder);

							pDC->Polygon(&(Point[0]), sizeof(Point)/sizeof(Point[0]));

							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}

						if (CWnd::GetFocus() == m_pListBox)
						{
							CRect crFocus(crItem);
							crFocus.InflateRect(-1,-1);
							pDC->DrawFocusRect(crFocus);
						}
					}
				}
			}
			else
			{
				// Complete item is empty.
				Util::FillRectangleWithColor(*pDC, crItem, GetSysColor(COLOR_BTNFACE));
			}
		}

		if (pOldPal != NULL)
		{
			pDC->SelectPalette(pOldPal, FALSE);
		}

		pDC->RestoreDC(-1);
	}
}

void CReceiverListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	CRect crListBox;
	pList->GetClientRect(crListBox);
	pMeasureItemStruct->itemWidth = crListBox.Width();
	pMeasureItemStruct->itemHeight = m_pListBox->GetItemHeight();
}

int CReceiverListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CReceiverListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// CReceiverListBox
//
// This window is actually disabled. It's only used to simplify drawing
// using  owner list box control.

WNDPROC CReceiverListBox::m_pfnSuperWndProc = NULL;

CReceiverListBox::CReceiverListBox()
{
}

CReceiverListBox::~CReceiverListBox()
{
}

BOOL CReceiverListBox::CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID)
{
	return CreateEx(0, "ListBox", "Receiver ListBox", dwStyle, 0, 0, 0, 0, pParent->GetSafeHwnd(), (HMENU)nID, NULL);
}

BEGIN_MESSAGE_MAP(CReceiverListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CReceiverListBox)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Redraw an item.
void CReceiverListBox::RedrawItem(int nItem)
{
	CRect crItem;
	if (GetItemRect(nItem, crItem) != LB_ERR)
	{
		InvalidateRect(crItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CReceiverListBox message handlers

BOOL CReceiverListBox::OnEraseBkgnd(CDC* pDC)
{
	// There are always enough items to fill the list box.
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookReceiverListBox

CAddressBookReceiverListBox::CAddressBookReceiverListBox()
{
	m_pDatabaseCursor = NULL;
	m_nLineHeight = -1;
	m_nCharWidth = -1;
	m_dwFirstRecord = 0;
	m_dwRecordsShowing = 0;
	m_dwFullRecordsShowing = 0;
	m_dwMaximumRecord = 0;
	m_fDragging = FALSE;
	m_hFont = NULL;
	m_nListBoxIndex = -1;
	m_fScrollTimer = TRUE;
}

CAddressBookReceiverListBox::~CAddressBookReceiverListBox()
{
	DeleteFont();
}

void CAddressBookReceiverListBox::DeleteFont(void)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

BEGIN_MESSAGE_MAP(CAddressBookReceiverListBox, CWnd)
	//{{AFX_MSG_MAP(CAddressBookReceiverListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GET_RECEIVER_LIST, OnGetReceiverList)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

void CAddressBookReceiverListBox::SetDatabaseCursor(CFlatFileDatabaseCursor* pCursor)
{
	// Save the new cursor.
	m_pDatabaseCursor = pCursor;

	// Initialize some font and metric information.
	CDC* pDC = GetDC();
	if (pDC != NULL)
	{
		CFont* pFont = GetFont();
		if (pFont != NULL)
		{
			m_List.SetFont(pFont);

			CFont* pOldFont = pDC->SelectObject(pFont);
			if (pOldFont != NULL)
			{
				TEXTMETRIC tm;
				if (pDC->GetTextMetrics(&tm))
				{
					m_nLineHeight = tm.tmHeight;
					m_nCharWidth = tm.tmAveCharWidth;
				}
				pDC->SelectObject(pOldFont);
			}
		}

		// Initialize for the new cursor.
		if (pCursor == NULL)
		{
			m_SearchCursor.Detach();
		}
		else
		{
			// Initialize the Search cursor.
			m_SearchCursor.SetCursor(*m_pDatabaseCursor);
		}

		ReleaseDC(pDC);
		pDC = NULL;
	}

	// Redraw ourselves.
	Invalidate();
}

void CAddressBookReceiverListBox::UpdateScrollBars(void)
{
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	m_dwMaximumRecord = (pCursor != NULL) ? pCursor->GetRecordCount() : 0;

	SCROLLINFO si;

	// Do vertical scroll bar.
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE;
	si.nMin = 0;
	si.nMax = m_dwMaximumRecord-1;
	si.nPage = m_dwFullRecordsShowing;

	SetScrollInfo(SB_VERT, &si);

	// Make sure the scroll bar position is properly set.
	if (SetFirstRecord(m_dwFirstRecord))
	{
		// The first record changed, so the current index may also have changed.
		RefreshFocus(FALSE);
	}

	// This function basically assumes that someone else is going to invalidate the listbox.
}

void CAddressBookReceiverListBox::BeforeRecordChange(void)
{
}

void CAddressBookReceiverListBox::AfterRecordChange(void)
{
	Reframe();
}

void CAddressBookReceiverListBox::ActivateView(BOOL fActive /*=TRUE*/)
{
	if (fActive)
	{
		if (GetParent()->GetDlgItem(IDC_ALL) != NULL)
		{
			GetParent()->GetDlgItem(IDC_ALL)->EnableWindow(TRUE);
			GetParent()->GetDlgItem(IDC_ALL)->ShowWindow(SW_SHOW);
		}
		if (GetParent()->GetDlgItem(IDC_NONE) != NULL)
		{
			GetParent()->GetDlgItem(IDC_NONE)->EnableWindow(TRUE);
			GetParent()->GetDlgItem(IDC_NONE)->ShowWindow(SW_SHOW);
		}
		if (GetParent()->GetDlgItem(IDC_SAVE) != NULL)
		{
			GetParent()->GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
			GetParent()->GetDlgItem(IDC_SAVE)->ShowWindow(SW_HIDE);
		}
		if (GetParent()->GetDlgItem(IDC_UNDO) != NULL)
		{
			GetParent()->GetDlgItem(IDC_UNDO)->EnableWindow(FALSE);
			GetParent()->GetDlgItem(IDC_UNDO)->ShowWindow(SW_HIDE);
		}
		Reframe();
	}
	else
	{
		if (GetParent()->GetDlgItem(IDC_ALL) != NULL)
		{
			GetParent()->GetDlgItem(IDC_ALL)->EnableWindow(FALSE);
			GetParent()->GetDlgItem(IDC_ALL)->ShowWindow(SW_HIDE);
		}
		if (GetParent()->GetDlgItem(IDC_NONE) != NULL)
		{
			GetParent()->GetDlgItem(IDC_NONE)->EnableWindow(FALSE);
			GetParent()->GetDlgItem(IDC_NONE)->ShowWindow(SW_HIDE);
		}
		if (GetParent()->GetDlgItem(IDC_SAVE) != NULL)
		{
			GetParent()->GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
			GetParent()->GetDlgItem(IDC_SAVE)->ShowWindow(SW_SHOW);
		}
		if (GetParent()->GetDlgItem(IDC_UNDO) != NULL)
		{
			GetParent()->GetDlgItem(IDC_UNDO)->EnableWindow(TRUE);
			GetParent()->GetDlgItem(IDC_UNDO)->ShowWindow(SW_SHOW);
		}
	}
}

void CAddressBookReceiverListBox::Reframe(void)
{
	BOOL fInvalidateListBox = FALSE;

	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		// Check if the number of records has changed.
		// If so, force a list box invalidation.
		DWORD dwNewMaximumRecord = pCursor->GetRecordCount();
		if (m_dwMaximumRecord != dwNewMaximumRecord)
		{
			UpdateScrollBars();
			fInvalidateListBox = TRUE;
		}

		DWORD dwCurrentRecord = pCursor->GetPosition();

		// Check if current record is already showing.
		if ((dwCurrentRecord < m_dwFirstRecord)
		 || (dwCurrentRecord >= m_dwFirstRecord+m_dwFullRecordsShowing))
		{
			DWORD dwNewFirstRecord;

			if (dwCurrentRecord < m_dwFirstRecord)
			{
				dwNewFirstRecord = dwCurrentRecord;
			}
			else
			{
				if (!m_fDragging)
				{
					dwNewFirstRecord = dwCurrentRecord-m_dwFullRecordsShowing+1;
				}
				else
				{
					dwNewFirstRecord = dwCurrentRecord-m_dwSaveFullRecordsShowing+1;
				}
			}

			// Set the new first record
			fInvalidateListBox |= SetFirstRecord(dwNewFirstRecord);
		}
		else
		{
			// Make sure the first record is properly set.
			fInvalidateListBox |= SetFirstRecord(m_dwFirstRecord);
		}
	}

	if (fInvalidateListBox)
	{
		Invalidate();
	}

	// The current index may have changed with either a change in the first record index,
	// the current cursor position, or both.
	RefreshFocus(!fInvalidateListBox);
}

BOOL CAddressBookReceiverListBox::SetFirstRecord(DWORD dwFirstRecord)
{
	BOOL fResult = FALSE;

	DWORD dwFullRecordsShowing = m_fDragging ? m_dwSaveFullRecordsShowing : m_dwFullRecordsShowing;

	// Don't allow a first record which would scroll up too far.
	if (m_dwMaximumRecord <= m_dwFullRecordsShowing)
	{
		dwFirstRecord = 0;
	}
	else
	{
		if (dwFirstRecord > m_dwMaximumRecord-dwFullRecordsShowing)
		{
			dwFirstRecord = m_dwMaximumRecord-dwFullRecordsShowing;
		}
	}

	if (m_dwFirstRecord != dwFirstRecord)
	{
		m_dwFirstRecord = dwFirstRecord;
		SetScrollPos(SB_VERT, (int)m_dwFirstRecord);
		fResult = TRUE;
	}

	return fResult;
}

void CAddressBookReceiverListBox::RefreshFocus(BOOL fRedraw /*=TRUE*/)
{
	int nCurrentIndex = -1;

	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		DWORD dwRecord = pCursor->GetPosition();
		if (dwRecord != (DWORD)-1)
		{
			if ((dwRecord >= m_dwFirstRecord)
			 || (dwRecord < m_dwFirstRecord+m_dwRecordsShowing))
			{
				nCurrentIndex = (int)(dwRecord-m_dwFirstRecord);
			}
		}
	}

	if (m_nListBoxIndex != nCurrentIndex)
	{
		if (fRedraw)
		{
			if (m_nListBoxIndex != -1)
			{
				m_List.RedrawItem(m_nListBoxIndex);
			}
			if (nCurrentIndex != -1)
			{
				m_List.RedrawItem(nCurrentIndex);
			}
		}

		m_nListBoxIndex = nCurrentIndex;
	}
}

BOOL CAddressBookReceiverListBox::IsReceiver(CFlatFileDatabaseCursor* pCursor)
{
	BOOL fIsReceiver = FALSE;

	if (pCursor != NULL)
	{
		// Get the record number.
		DWORD dwRecord = pCursor->GetPosition();
		if (dwRecord != (DWORD)-1)
		{
			// Check if ID is in receiver list.
			CAddressBookNameList* pReceivers = (CAddressBookNameList*)(GetParent()->SendMessage(WM_GET_RECEIVER_LIST, 0, 0));
			if (pReceivers != NULL)
			{
				CFlatFileDatabaseField* pField = pCursor->GetField("__ID__");
				if (pField != NULL)
				{
					fIsReceiver = pReceivers->FindName(pField->GetAsUnsigned32()) != -1;
				}
			}

			// If we are dragging out a selection, override the the receiver status of
			// records in the the selection range.
			if (m_fDragging)
			{
				if (((dwRecord >= m_dwSelectStart) && (dwRecord <= m_dwSelectEnd))
				 || ((dwRecord >= m_dwSelectEnd) && (dwRecord <= m_dwSelectStart)))
				{
					fIsReceiver = m_fSelectReceiver;
				}
			}
		}
	}

	return fIsReceiver;
}

void CAddressBookReceiverListBox::RefreshSelectCount(void)
{
	CRect crHeader;
	GetClientRect(crHeader);
	crHeader.bottom = GetHeaderHeight();
	InvalidateRect(crHeader);
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookReceiverListBox message handlers

LRESULT CAddressBookReceiverListBox::OnGetReceiverList(WPARAM wParam, LPARAM lParam)
{
	const MSG* pMsg = GetCurrentMessage();
	return GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
}

// Save the font to use.
LRESULT CAddressBookReceiverListBox::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	HFONT hNewFont = NULL;
	if (wParam != 0)
	{
		LOGFONT LogFont;
		if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
		{
			hNewFont = CreateFontIndirect(&LogFont);
		}
	}
		
	DeleteFont();
	m_hFont = hNewFont;
	
	if (LOWORD(lParam) != 0)
	{
		Invalidate();
	}
	
	return 0;
}

// Return the font to use.
LRESULT CAddressBookReceiverListBox::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(UINT)m_hFont;
}

int CAddressBookReceiverListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
		return -1;

	CFont* pFont = GetParent()->GetFont();
	if (pFont != NULL)
	{
		SetFont(pFont);

		CDC* pDC = GetDC();
		if (pDC != NULL)
		{
			CFont* pOldFont = pDC->SelectObject(pFont);
			if (pOldFont != NULL)
			{
				TEXTMETRIC tm;
				if (pDC->GetTextMetrics(&tm))
				{
					m_nLineHeight = tm.tmHeight;
					m_nCharWidth = tm.tmAveCharWidth;
				}

				pDC->SelectObject(pOldFont);
			}

			ReleaseDC(pDC);
			pDC = NULL;
		}
	}

	m_List.CreateControl(WS_CHILD | /*WS_CLIPSIBLINGS |*/ WS_DISABLED | WS_VISIBLE | LBS_OWNERDRAWVARIABLE | LBS_NOINTEGRALHEIGHT | LBS_WANTKEYBOARDINPUT, this, 0);
	
	return 0;
}

void CAddressBookReceiverListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_fScrollTimer = FALSE;

	if (GetFocus() != this)
	{
		SetFocus();
	}

	// If the mouse click happened over one of the records, tell our parent to select that record.
	CRect crRect;
	m_List.GetWindowRect(crRect);
	ScreenToClient(crRect);
	if (crRect.PtInRect(point))
	{
		DWORD dwRecord = m_dwFirstRecord+(point.y-crRect.top)/GetItemHeight();
		if (dwRecord < m_dwMaximumRecord)
		{
			CFlatFileDatabaseCursor* pCursor = GetSearchCursor();
			if (pCursor != NULL)
			{
				pCursor->SetPosition(dwRecord);
				m_fSelectReceiver = !IsReceiver(pCursor);
				m_dwSelectStart = m_dwSelectEnd = dwRecord;
				m_fDragging = TRUE;
				m_dwSaveFullRecordsShowing = m_dwFullRecordsShowing;
				m_dwFullRecordsShowing = m_dwRecordsShowing;
				SetCapture();
				GetParent()->SendMessage(WM_SELECT_RECORD, (WPARAM)0, (LPARAM)dwRecord);
				m_List.Invalidate();
				RefreshSelectCount();
			}
		}
	}
}

void CAddressBookReceiverListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_fScrollTimer = FALSE;

	if (m_fDragging)
	{
		CAddressBookNameList* pReceivers = (CAddressBookNameList*)(GetParent()->SendMessage(WM_GET_RECEIVER_LIST, 0, 0));
		if (pReceivers != NULL)
		{
			CFlatFileDatabaseCursor* pCursor = GetSearchCursor();
			if (pCursor != NULL)
			{
				CFlatFileDatabaseField* pField = pCursor->GetField("__ID__");
				if (pField != NULL)
				{
					if (m_dwSelectStart > m_dwSelectEnd)
					{
						DWORD d = m_dwSelectStart;
						m_dwSelectStart = m_dwSelectEnd;
						m_dwSelectEnd = d;
					}

					for (DWORD dwRecord = m_dwSelectStart; dwRecord <= m_dwSelectEnd; dwRecord++)
					{
						pCursor->SetPosition(dwRecord);

						DWORD dwID = pField->GetAsUnsigned32();
						if (dwID != 0)
						{
							if (m_fSelectReceiver)
							{
								pReceivers->AddName(dwID);
							}
							else
							{
								pReceivers->DeleteName(dwID);
							}
						}
					}
				}
			}
		}

		m_fDragging = FALSE;
		m_dwFullRecordsShowing = m_dwSaveFullRecordsShowing;
		ReleaseCapture();
		Reframe();
		m_List.Invalidate();
	}
}

void CAddressBookReceiverListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	INHERITED::OnMouseMove(nFlags, point);
	if (m_fDragging)
	{
		DWORD dwNewRecord = (DWORD)-1;

		CRect crRect;
		m_List.GetWindowRect(crRect);
		ScreenToClient(crRect);

		if (point.y < crRect.top)
		{
			m_nScrollDirection = -1;
			if (!m_fScrollTimer)
			{
				m_fScrollTimer = TRUE;
				SetTimer(SCROLL_TIMER, 50, NULL);
			}
		}
		else if (point.y >= crRect.top+(int)(m_dwFullRecordsShowing*GetItemHeight()))
		{
			m_nScrollDirection = 1;
			if (!m_fScrollTimer)
			{
				m_fScrollTimer = TRUE;
				SetTimer(SCROLL_TIMER, 50, NULL);
			}
		}
		else
		{
			dwNewRecord = m_dwFirstRecord+(point.y-crRect.top)/GetItemHeight();
			m_fScrollTimer = FALSE;
			if (m_dwSelectEnd != dwNewRecord)
			{
				m_dwSelectEnd = dwNewRecord;
				GetParent()->SendMessage(WM_SELECT_RECORD, (WPARAM)0, (LPARAM)dwNewRecord);
				m_List.Invalidate();
				RefreshSelectCount();
			}
		}
	}
}

UINT CAddressBookReceiverListBox::OnGetDlgCode()
{
	UINT uRet = INHERITED::OnGetDlgCode() | DLGC_WANTCHARS | DLGC_WANTARROWS;
	if (Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_RETURN, VK_NEXT, VK_PRIOR, 0))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

void CAddressBookReceiverListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_fDragging)
	{
		switch (nChar)
		{
			case VK_RETURN:
			case VK_SPACE:
			{
				// Toggle the receiver state of the current record.
				Reframe();
				CAddressBookNameList* pReceivers = (CAddressBookNameList*)(GetParent()->SendMessage(WM_GET_RECEIVER_LIST, 0, 0));
				if (pReceivers != NULL)
				{
					CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
					if (pCursor != NULL)
					{
						CFlatFileDatabaseField* pField = pCursor->GetField("__ID__");
						if (pField != NULL)
						{
							DWORD dwID = pField->GetAsUnsigned32();
							if (dwID != 0)
							{
								int nIndex = pReceivers->Find(dwID);
								if (nIndex == -1)
								{
									pReceivers->AddName(dwID);
								}
								else
								{
									pReceivers->DeleteName(dwID);
								}
								if (m_nListBoxIndex != -1)
								{
									m_List.RedrawItem(m_nListBoxIndex);
								}
								RefreshSelectCount();
							}
						}
					}
				}
				break;
			}
			case VK_UP:
			{
				MoveRecord(-1);
				break;
			}
			case VK_DOWN:
			{
				MoveRecord(1);
				break;
			}
			case VK_PRIOR:
			{
				BackPage();
				break;
			}
			case VK_NEXT:
			{
				NextPage();
				break;
			}
			default:
			{
				INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
				break;
			}
		}
	}
	else
	{
		INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CAddressBookReceiverListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	INHERITED::OnChar(nChar, nRepCnt, nFlags);
}

void CAddressBookReceiverListBox::BackPage(void)
{
	MoveRecord(-(long)m_dwFullRecordsShowing);
}

void CAddressBookReceiverListBox::NextPage(void)
{
	MoveRecord((long)m_dwFullRecordsShowing);
}

BOOL CAddressBookReceiverListBox::MoveRecord(long lDelta)
{
	BOOL fMoved = FALSE;

	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		DWORD dwCurrentRecord = pCursor->GetPosition();
		if (dwCurrentRecord != (DWORD)-1)
		{
			DWORD dwNewRecord;
			if ((lDelta < 0) && (dwCurrentRecord < (DWORD)(-lDelta)))
			{
				dwNewRecord = 0;
			}
			else
			{
				dwNewRecord = dwCurrentRecord+lDelta;
			}

			if (dwNewRecord != dwCurrentRecord)
			{
				GetParent()->SendMessage(WM_SELECT_RECORD, (WPARAM)0, (LPARAM)dwNewRecord);
			}

			fMoved = TRUE;
		}
	}

	return fMoved;
}

BOOL CAddressBookReceiverListBox::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CAddressBookReceiverListBox::OnSize(UINT nType, int cx, int cy) 
{
	// Do the normal size.
	INHERITED::OnSize(nType, cx, cy);

	// Size the list box to the new area.
	CRect crList;
	GetClientRect(crList);
	crList.top += GetHeaderHeight();

	// Figure out how many items we should have and recreate them.
	m_List.SetRedraw(FALSE);
	m_List.ResetContent();
	m_dwRecordsShowing = (DWORD)(((crList.Height()-1)/GetItemHeight())+1);
	m_dwFullRecordsShowing = (DWORD)(crList.Height()/GetItemHeight());
	for (DWORD d = 0; d < m_dwRecordsShowing; d++)
	{
		CReceiverListBoxItem* pItem = NULL;
		TRY
		{
			pItem = new CReceiverListBoxItem(this);
		}
		END_TRY

		if (pItem != NULL)
		{
			m_List.AddString((LPCSTR)pItem);
		}
	}

	m_List.SetRedraw(TRUE);

	// Move the window and redraw it.
	m_List.MoveWindow(crList);

	// Update the scroll bars.
	UpdateScrollBars();
}


void CAddressBookReceiverListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Preload some useful things.
	CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
	if (pCursor != NULL)
	{
		DWORD dwNewFirstRecord = m_dwFirstRecord;
		switch (nSBCode)
		{
			case SB_LINEUP:
			{
				// Move up a record.
				if (dwNewFirstRecord > 0)
				{
					dwNewFirstRecord--;
				}
				break;
			}
			case SB_LINEDOWN:
			{
				// Move down a record.
				if (dwNewFirstRecord+1 < m_dwMaximumRecord)
				{
					dwNewFirstRecord++;
				}
				break;
			}
			case SB_PAGEUP:
			{
				// Move up a page of records.
				if (dwNewFirstRecord > m_dwFullRecordsShowing)
				{
					dwNewFirstRecord -= m_dwFullRecordsShowing;
				}
				else
				{
					dwNewFirstRecord = 0;
				}
				break;
			}
			case SB_PAGEDOWN:
			{
				// Move down a page of records.
				if (dwNewFirstRecord+m_dwFullRecordsShowing+1 < m_dwMaximumRecord)
				{
					dwNewFirstRecord += m_dwFullRecordsShowing;
				}
				else
				{
					dwNewFirstRecord = m_dwMaximumRecord-1;
				}
				break;
			}
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			{
				// Move to the position the user has selected.
				dwNewFirstRecord = (DWORD)nPos;
				break;
			}
		}

		// Set the new first record.
		if (SetFirstRecord(dwNewFirstRecord))
		{
			// First record changed, invalidate ourselves.
			Invalidate();

			// The first record changed, so the index probably did as well.
			RefreshFocus(FALSE);
		}
	}

	INHERITED::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CAddressBookReceiverListBox::OnPaint() 
{
	CPaintDC dc(this);

	CPalette* pOldPal = dc.SelectPalette(pOurPal, FALSE);
	dc.RealizePalette();

	CFont* pOldFont = dc.SelectObject(GetParent()->GetFont());
	if (pOldFont != NULL)
	{
		// Draw the header.
		CRect crHeader;
		GetClientRect(crHeader);
		crHeader.bottom = crHeader.top+GetHeaderHeight();
		crHeader.right++;
		FillRectangleWithColor(dc, crHeader, FIELD_BACKGROUND_COLOR, GetSysColor(COLOR_BTNSHADOW));
		crHeader.bottom--;

		CString csText;
		CString csCount;
		CString csFormat;
		TRY
		{
			int nReceiverCount = 0;
			CAddressBookNameList* pReceivers = (CAddressBookNameList*)(GetParent()->SendMessage(WM_GET_RECEIVER_LIST, 0, 0));
			if (pReceivers != NULL)
			{
				nReceiverCount = pReceivers->GetNameCount();
				if (m_fDragging)
				{
					CFlatFileDatabaseCursor* pCursor = GetSearchCursor();
					if (pCursor != NULL)
					{
						CFlatFileDatabaseField* pField = pCursor->GetField("__ID__");
						if (pField != NULL)
						{
							DWORD dwStart;
							DWORD dwEnd;
							if (m_dwSelectStart < m_dwSelectEnd)
							{
								dwStart = m_dwSelectStart;
								dwEnd = m_dwSelectEnd;
							}
							else
							{
								dwStart = m_dwSelectEnd;
								dwEnd = m_dwSelectStart;
							}

							for (DWORD dwRecord = dwStart; dwRecord <= dwEnd; dwRecord++)
							{
								pCursor->SetPosition(dwRecord);
								DWORD dwID = pField->GetAsUnsigned32();
								if (dwID != 0)
								{
									if (pReceivers->FindName(dwID) != -1)
									{
										if (!m_fSelectReceiver)
										{
											nReceiverCount--;
										}
									}
									else
									{
										if (m_fSelectReceiver)
										{
											nReceiverCount++;
										}
									}
								}
							}
						}
					}
				}
			}

			if (nReceiverCount == 0)
			{
				csText.LoadString(IDS_CLICK_ON_RECEIVER_NAME_NONE_SELECTED);
			}
			else
			{
				csFormat.LoadString(IDS_CLICK_ON_RECEIVER_NAME);
				csText.Format(csFormat, nReceiverCount);
			}

			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
			Util::DrawText(&dc, csText, crHeader, DT_CENTER|DT_NOPREFIX|DT_VCENTER|DT_SINGLELINE);
		}
		END_TRY

		dc.SelectObject(pOldFont);
	}

	if (pOldPal != NULL)
	{
		dc.SelectPalette(pOldPal, FALSE);
	}
}

void CAddressBookReceiverListBox::OnTimer(UINT nIDEvent) 
{
	CWnd::OnTimer(nIDEvent);
	if (nIDEvent == SCROLL_TIMER)
	{
		if (m_fScrollTimer)
		{
			CFlatFileDatabaseCursor* pCursor = GetDatabaseCursor();
			if (pCursor != NULL)
			{
				DWORD dwCurrentRecord = pCursor->GetPosition();
				if (dwCurrentRecord != (DWORD)-1)
				{
					DWORD dwNewRecord;
					if ((m_nScrollDirection < 0) && (dwCurrentRecord < (DWORD)(-m_nScrollDirection)))
					{
						dwNewRecord = 0;
					}
					else
					{
						dwNewRecord = dwCurrentRecord+m_nScrollDirection;
					}

					if (m_dwSelectEnd != dwNewRecord)
					{
						m_dwSelectEnd = dwNewRecord;
						GetParent()->SendMessage(WM_SELECT_RECORD, (WPARAM)0, (LPARAM)dwNewRecord);
						RefreshSelectCount();
					}
				}
			}
		}
		else
		{
			KillTimer(SCROLL_TIMER);
		}
	}
}

void CAddressBookReceiverListBox::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	if (m_nListBoxIndex != -1)
	{
		m_List.RedrawItem(m_nListBoxIndex);
	}
}

void CAddressBookReceiverListBox::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	if (m_nListBoxIndex != -1)
	{
		m_List.RedrawItem(m_nListBoxIndex);
	}
}
