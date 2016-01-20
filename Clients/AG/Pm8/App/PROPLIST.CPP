/*
// $Workfile: PROPLIST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROPLIST.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:12   Fred
// Initial revision.
// 
//    Rev 1.1   16 Jul 1996 17:30:34   Fred
// More address/sender book stuff
// 
//    Rev 1.0   14 Mar 1996 13:34:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:44   FRED
// Initial revision.
// 
//    Rev 1.9   06 Oct 1995 15:23:48   FRED
// Win32 SendMessage() changes
// 
//    Rev 1.8   13 Sep 1995 11:48:10   JAY
// New 32-bit changes
// 
//    Rev 1.7   27 Aug 1995 12:35:30   FRED
// New fill-in-fields stuff
// 
//    Rev 1.6   06 Aug 1995 16:33:02   FRED
// More Fill-in Fields dialog stuff.
// 
//    Rev 1.5   11 Jul 1995 14:19:26   JAY
// Fill-in fields dialog.
// 
//    Rev 1.4   28 Apr 1995 16:11:38   JAY
// Validation, better kill focus support, more stability, etc.
// 
//    Rev 1.3   26 Apr 1995 14:41:24   JAY
//  
// 
//    Rev 1.2   25 Apr 1995 15:24:34   JAY
//  
// 
//    Rev 1.1   25 Apr 1995 08:58:06   JAY
// More refinements.
// 
//    Rev 1.0   19 Apr 1995 12:37:18   JAY
// Initial revision.
*/

// proplist.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "proplist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Misc.

static BOOL near ReturnOrEscape(MSG msg, BOOL fWantEscape = TRUE)
{
	if (msg.lParam != 0)
	{
		LPMSG pMsg = (LPMSG)msg.lParam;

	/* Get the message to be handled. */

		switch (pMsg->message)
		{
			case WM_CHAR:
			case WM_KEYDOWN:
			{
				if ((pMsg->wParam == VK_RETURN)
				 || (fWantEscape && (pMsg->wParam == VK_ESCAPE)))
				{
				/* We want to get this message ourselves. */
					return TRUE;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyEdit

CPropertyEdit::CPropertyEdit()
{
}

CPropertyEdit::~CPropertyEdit()
{
}

BEGIN_MESSAGE_MAP(CPropertyEdit, CEdit)
	//{{AFX_MSG_MAP(CPropertyEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyEdit message handlers

void CPropertyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_RETURN:
		case VK_ESCAPE:
		{
		/* Pass it up to our parent. Don't do any other processing. */
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
//			od("Edit: OnChar(%x)\r\n", nChar);
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CPropertyEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_RETURN:
		case VK_ESCAPE:
		case VK_UP:
		case VK_DOWN:
		case VK_TAB:
		{
		/* Pass it up to our parent. Don't do any other processing. */
			const MSG* pMsg = GetCurrentMessage();
			GetParent()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			break;
		}
		default:
		{
//			od("Edit: OnKeyDown(%x)\r\n", nChar);
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

UINT CPropertyEdit::OnGetDlgCode()
{
	UINT uRet = CEdit::OnGetDlgCode() | DLGC_WANTTAB;

	if (ReturnOrEscape(*GetCurrentMessage()))
	{
		uRet |= DLGC_WANTMESSAGE;
	}

	return uRet;
}

void CPropertyEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CPropertyListBox* pParent = (CPropertyListBox*)GetParent();

	pParent->OnEditorKillFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyStandardEditor

CPropertyStandardEditor::CPropertyStandardEditor(const CRect& crPosition, CWnd* pParent /*=NULL*/)
{
	m_Edit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
					  crPosition,
					  pParent,
					  (UINT)-1);
	m_Edit.SetFont(pParent->GetFont());
}

void CPropertyStandardEditor::SetText(LPCSTR pText)
{
	m_Edit.SetWindowText(pText);
	m_Edit.SetSel(0, -1);
	m_Edit.SetFocus();
}

void CPropertyStandardEditor::GetText(CString& csText)
{
	m_Edit.GetWindowText(csText);
}

void CPropertyStandardEditor::SetFocus(void)
{
	m_Edit.SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBoxItem

void CPropertyListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
#if 0
	od("DrawItem id:%d, act:%x[sta:%x] (fld:%d)\r\n",
		pDrawItemStruct->itemID,
		pDrawItemStruct->itemAction,
		pDrawItemStruct->itemState,
		((CPropertyListBox*)pList)->Field());
#endif

	if (pDrawItemStruct->itemID != -1)
	{
		if ((pDrawItemStruct->itemAction & (ODA_DRAWENTIRE|ODA_SELECT|ODA_FOCUS)) != 0)
		{
			// Paint the item.
			Paint(pList, pDrawItemStruct);
		}

#if 0
		if (IsFocused(pList, pDrawItemStruct))
		{
			 // This item is focused, preview it.
			Preview(pList, pDrawItemStruct);
		}
#endif
	}
}

void CPropertyListBoxItem::Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	CPropertyListBox* pPropList = (CPropertyListBox*)pList;
	int nField = pPropList->Field();

	// Get the DC and rectangle to use.

	CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);

	if (pDC != NULL)
	{
		// Paint the grid.

		PaintGrid(pPropList, pDC, pDrawItemStruct);

		PaintField(PROPERTY_NameField, pPropList, pDC, pDrawItemStruct);
		PaintField(PROPERTY_ValueField, pPropList, pDC, pDrawItemStruct);

	}
}

void CPropertyListBoxItem::PaintField(int nField, CPropertyListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Get the colors to use.

	COLORREF clForeground;
	COLORREF clBackground;

	// Compute the bounding rectangles for the check box and the item.

	CRect crField;

	ComputeColors(nField, pList, pDC, pDrawItemStruct, clForeground, clBackground);
	ComputeBounds(nField, pList, pDC, pDrawItemStruct, crField);

	// Draw the background.

	PaintBackground(nField, pList, pDC, pDrawItemStruct, crField, clBackground);

	// Draw the item text.

	PaintItem(nField, pList, pDC, pDrawItemStruct, crField, clForeground, clBackground);

	// If the item has the focus, draw the focus indicator.

	if (IsFocused(nField, pList, pDrawItemStruct))
	{
		PaintFocus(pList, pDC, pDrawItemStruct, &crField);
	}
}

void CPropertyListBoxItem::PaintGrid(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	CPropertyListBox* pPropList = (CPropertyListBox*)pList;
	CBrush cbGrid(pPropList->GridColor());

	CRect crDraw;

/* Draw the horizontal grid line. */

	crDraw.left = pDrawItemStruct->rcItem.left;
	crDraw.right = pDrawItemStruct->rcItem.right;
	crDraw.bottom = pDrawItemStruct->rcItem.bottom;
	crDraw.top = crDraw.bottom-GRID_THICKNESS;
	pDC->FillRect(&crDraw, &cbGrid);

/* Draw the vertical grid line. */

	int nLeft;
	int nRight;
	pPropList->FieldExtent(PROPERTY_SplitterField,
								  nLeft,
								  nRight);
	crDraw.left = nLeft;
	crDraw.right = nRight;

	crDraw.top = pDrawItemStruct->rcItem.top;
	crDraw.bottom = pDrawItemStruct->rcItem.bottom-GRID_THICKNESS;
	pDC->FillRect(&crDraw, &cbGrid);
}

void CPropertyListBoxItem::ComputeColors(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground)
{
	if (IsSelected(nField, pList, pDrawItemStruct))
	{
		// Selected colors.
		
		clForeground			= GetSysColor(COLOR_HIGHLIGHTTEXT);
		clBackground			= GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		// Non selected colors.
		
		clForeground			= GetSysColor(COLOR_WINDOWTEXT);
		clBackground			= GetSysColor(COLOR_WINDOW);
	}
	
#if 0
	if (Disable())
	{
		clForeground			= GetSysColor(COLOR_GRAYTEXT);
		clCheckBorder			= GetSysColor(COLOR_GRAYTEXT);
		clSplitter				= clForeground;
	}
#endif
}

void CPropertyListBoxItem::ComputeBounds(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBound)
{
	CRect crItem = pDrawItemStruct->rcItem;
	CPropertyListBox* pPropList = (CPropertyListBox*)pList;

	crBound.top = crItem.top;
	crBound.bottom = crItem.bottom-GRID_THICKNESS;
	int nLeft;
	int nRight;
	pPropList->FieldExtent(nField, nLeft, nRight);
	crBound.left = nLeft;
	crBound.right = nRight;
}

void CPropertyListBoxItem::PaintBackground(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crField, COLORREF clBackground)
{
	CBrush cbBackground(clBackground);
	pDC->FillRect(&crField, &cbBackground);
}

void CPropertyListBoxItem::PaintItem(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CString csField;

	PaintString(GetFieldText(nField, csField), pList, pDC, pDrawItemStruct, crBounds, clForeground, clBackground);
}

void CPropertyListBoxItem::PaintString(LPCSTR pString, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	if (pString != NULL)
	{
		int nSave = pDC->SaveDC();

		pDC->SetTextColor(clForeground);
		pDC->SetBkColor(clBackground);

		pDC->IntersectClipRect(crBounds.left+4,
									  crBounds.top+4,
									  crBounds.right-4,
									  crBounds.bottom-4);

		pDC->TextOut(crBounds.left+4, crBounds.top+4, pString, strlen(pString));

		pDC->RestoreDC(nSave);
	}
}

void CPropertyListBoxItem::PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect* pRect /*=NULL*/)
{
	pDC->DrawFocusRect(pRect == NULL ? &(pDrawItemStruct->rcItem) : *pRect);
}

BOOL CPropertyListBoxItem::IsSelected(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	CPropertyListBox* pPropList = (CPropertyListBox*)pList;

	return (pDrawItemStruct->itemState & ODS_SELECTED) != 0
				&& pPropList->Field() == nField
				&& !pPropList->Editing()
				&& !pPropList->PreparingToEdit();
}

BOOL CPropertyListBoxItem::IsFocused(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
	CPropertyListBox* pPropList = (CPropertyListBox*)pList;

	return (pDrawItemStruct->itemState & ODS_FOCUS) != 0
				&& pPropList->Field() == nField
				&& !pPropList->Editing()
				&& !pPropList->PreparingToEdit();
}

void CPropertyListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	((CPropertyListBox*)pList)->MeasureItem(pMeasureItemStruct);
}

int CPropertyListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void CPropertyListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox

CPropertyListBox::CPropertyListBox()
{
	m_nSplitter = 0;

	m_nField = PROPERTY_ValueField;
	m_clGrid = ::GetSysColor(COLOR_BTNFACE);

	m_pEditor = NULL;
	m_fPreparingToEdit = FALSE;
	m_fEditorKillFocusPending = FALSE;
}

CPropertyListBox::~CPropertyListBox()
{
	DestroyEditor(m_pEditor);
}

BEGIN_MESSAGE_MAP(CPropertyListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CPropertyListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EDITOR_KILL_FOCUS, OnEditorKillFocusNotify)
END_MESSAGE_MAP()

void CPropertyListBox::MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	// Set the item dimensions based on the current font.

	CClientDC cdcWnd(this);

	CFont* pcfWnd = GetFont();

	if (pcfWnd != NULL)
	{
		CFont* pcfOld = cdcWnd.SelectObject(pcfWnd);

		if (pcfOld != NULL)
		{
			TEXTMETRIC tmFont;

			if (cdcWnd.GetTextMetrics(&tmFont))
			{
				CRect crClient;
				GetClientRect(crClient);
						
				pMeasureItemStruct->itemWidth = crClient.Width();
				pMeasureItemStruct->itemHeight = tmFont.tmHeight + GRID_THICKNESS + 4*2;
			}
					
			cdcWnd.SelectObject(pcfOld);
		}
	}
}

BOOL CPropertyListBox::SubclassDlgItem(UINT nID, CWnd* pParent)
{
/*
// Do the basic thing.
*/

	if (!COwnerDrawListBox::SubclassDlgItem(nID, pParent))
	{
		return FALSE;
	}

/*
// Do whatever we need.
*/

#if 0
	//  Create our font.
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));

	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = -12;
		logFont.lfWeight = FW_BOLD;
		logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		CString strDefaultFont;
//		strDefaultFont.LoadString(IDS_DEFAULT_FONT);
		strDefaultFont = "MS Sans Serif";			// Cheat a little (for now!)
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		if (!m_Font.CreateFontIndirect(&logFont))
		{
			TRACE0("Could not create font for property list box.\n");
			m_Font.CreateStockObject(SYSTEM_FONT);
		}
	}
	else
	{
		m_Font.CreateStockObject(SYSTEM_FONT);
	}
	SetFont(&m_Font);
#endif
	CRect crBounds;

	GetWindowRect(&crBounds);

	m_nSplitter = crBounds.Width()/2;

	return TRUE;
}

BOOL CPropertyListBox::PreparingToEdit(void)
{
	return m_fPreparingToEdit;
}

BOOL CPropertyListBox::BeginEdit(int nCurSel /*=-1*/)
{
	if ((nCurSel == -1) || (nCurSel == GetCurSel()))
	{
		if (m_pEditor != NULL)
		{
			return TRUE;
		}
	}
	
	if (FinishEdit())
	{
	/* Figure out where this is supposed to go. */

		if (nCurSel < 0)
		{
			nCurSel = GetCurSel();
		}
		else
		{
			if (nCurSel != GetCurSel())
			{
				SetCurSelNotify(nCurSel);
			}
		}

		if (nCurSel >= 0)
		{
		/* Figure out where this guy is supposed to go. */

			CRect crItem;

			GetItemRect(nCurSel, crItem);
			int nLeft;
			int nRight;
			FieldExtent(m_nField, nLeft, nRight);
			crItem.left = nLeft;
			crItem.right = nRight;

		/* Create the editor here. */

			m_pEditor = CreateEditor(m_nField, crItem);

			CPropertyListBoxItem* pItem = (CPropertyListBoxItem*)GetItemData(nCurSel);

			CString csField;

			m_pEditor->SetText(pItem->GetFieldText(m_nField, csField));

			return TRUE;
		}
	}
	return FALSE;
}

void CPropertyListBox::AbortEdit(void)
{
	if (Editing())
	{
		OnAbortEdit();
	}
}

void CPropertyListBox::OnAbortEdit(void)
{
	DestroyEditor(m_pEditor);
}

BOOL CPropertyListBox::FinishEdit(BOOL fValidate /*=TRUE*/, BOOL fByReturn /*=FALSE*/)
{
	ASSERT(!m_fEditorKillFocusPending);

	if (Editing())
	{
		return OnFinishEdit(fValidate, fByReturn);
	}
	return TRUE;
}

BOOL CPropertyListBox::OnFinishEdit(BOOL fValidate /*=TRUE*/, BOOL fByReturn /*=FALSE*/)
{
	if (m_pEditor != NULL)
	{
		if (m_pEditor->GetEditor()->GetModify())
		{
			int nCurSel = GetCurSel();
			ASSERT(nCurSel >= 0);
	
			CPropertyListBoxItem* pItem = (CPropertyListBoxItem*)GetItemData(nCurSel);
			CString csText;
	
			m_pEditor->GetText(csText);
	
			pItem->SetFieldText(m_nField, csText);
			RedrawItem(nCurSel);
		}

		DestroyEditor(m_pEditor);
	}
	return TRUE;
}

BOOL CPropertyListBox::Editing(void)
{
	return m_pEditor != NULL;
}

/*
// The standard way to create an editor for a property listbox.
*/

CPropertyEditor* CPropertyListBox::CreateEditor(int m_nField, const CRect& crPosition)
{
	return new CPropertyStandardEditor(crPosition, this);
}

/*
// Destroy the editor.
*/

void CPropertyListBox::DestroyEditor(CPropertyEditor*& pEditor)
{
	delete pEditor;
	pEditor = NULL;
}

BOOL CPropertyListBox::CanEditItem(int nItem, int nField)
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox message handlers

void CPropertyListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nField = FindField(point.x);
	int nNewSel = Index(point);
	
	EditField(nField, nNewSel);
}

BOOL CPropertyListBox::EditField(int nField, int nSelection, UINT nChar /*=0*/, UINT nRepCnt /*=0*/, UINT nFlags /*=0*/)
{
	BOOL fResult = FALSE;
	
	if (FinishEdit())			/* In case we're editing...*/
	{
		if (GetCount() > 0)
		{
			if (nSelection >= 0)
			{
				BOOL fPreparingSave = m_fPreparingToEdit;
	
				CPropertyListBoxItem* pItem = (CPropertyListBoxItem*)GetItemData(nSelection);
				BOOL fCanEditItem = CanEditItem(nSelection, nField);
	
				if (!fCanEditItem)
				{
					if ((nField == PROPERTY_NameField) && CanEditItem(nSelection, PROPERTY_ValueField))
					{
						nField = PROPERTY_ValueField;
						fCanEditItem = TRUE;
					}
					else if ((nField == PROPERTY_ValueField) && CanEditItem(nSelection, PROPERTY_NameField))
					{
						nField = PROPERTY_NameField;
						fCanEditItem = TRUE;
					}
				}
	
				m_nField = nField;
	
				BOOL fCanEdit = pItem->CanEditField(m_nField) && fCanEditItem;
	
				m_fPreparingToEdit = fCanEdit;
				SetCurSelNotify(nSelection);
				m_fPreparingToEdit = fPreparingSave;
	
				if (fCanEdit)
				{
					if (BeginEdit())
					{
						if (nChar != 0)
						{
							if (m_pEditor != NULL)
							{
								m_pEditor->GetEditor()->SendMessage(WM_CHAR, nChar, MAKELPARAM(nRepCnt, nFlags));
							}
						}
						
						fResult = TRUE;
					}
				}
			}
		}
	}
	
	return fResult;
}

void CPropertyListBox::SetCurSelNotify(int nNewSel)
{
	SetCurSel(nNewSel);
	CWnd* pParent = GetParent();

/*
// Send the SELCHANGE notification to our parent if we have one.
*/

	if (pParent != NULL)
	{
#ifdef WIN32
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), LBN_SELCHANGE), (LPARAM)GetSafeHwnd());
#else
		pParent->SendMessage(WM_COMMAND, GetDlgCtrlID(), MAKELPARAM(GetSafeHwnd(), LBN_SELCHANGE));
#endif
	}
}

void CPropertyListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	COwnerDrawListBox::OnLButtonUp(nFlags, point);
}

void CPropertyListBox::OnMouseMove(UINT nFlags, CPoint point)
{
#if 0
	if (nFlags & MK_LBUTTON)
	{
		int nField = FindField(point.x);
		int nCurSel = GetCurSel();
		int nNewSel = Index(point);

		if (nField == PROPERTY_NameField || nField == PROPERTY_ValueField)
		{	
			if (m_nField != nField && nCurSel == nNewSel)
			{
			/* Only a field change. */
				RedrawItem(nCurSel);
			}
			m_nField = nField;
		}
	}

#endif
	COwnerDrawListBox::OnMouseMove(nFlags, point);
}

UINT CPropertyListBox::OnGetDlgCode()
{
	UINT uRet = COwnerDrawListBox::OnGetDlgCode() | DLGC_WANTTAB;
	if (ReturnOrEscape(*GetCurrentMessage(), m_pEditor != NULL))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

int CPropertyListBox::FindField(int x)
{
	for (int nField = 0; nField < NUMBER_OF_PROPERTY_FIELDS; nField++)
	{	
		int nLeft, nRight;

		FieldExtent(nField, nLeft, nRight);
		if (x >= nLeft && x < nRight)
		{
			return nField;
		}
	}
	return PROPERTY_UnknownField;
}

void CPropertyListBox::FieldExtent(int nField, int& nLeft, int& nRight, int nBoundLeft /*=0*/)
{
	CRect crClient;

	GetClientRect(crClient);

	switch (nField)
	{
		case PROPERTY_NameField:
		{
			nLeft = 0;
			nRight = m_nSplitter;
			break;
		}
		case PROPERTY_SplitterField:
		{
			nLeft = m_nSplitter;
			nRight = nLeft+GRID_THICKNESS;
			break;
		}
		case PROPERTY_ValueField:
		{
			nLeft = m_nSplitter+GRID_THICKNESS;
			nRight = crClient.Width();
			break;
		}
		default:
		{
			ASSERT(FALSE);
			nLeft = -1;
			nRight = -1;
			return;
		}
	}

	nLeft += nBoundLeft;
	nRight += nBoundLeft;
}

void CPropertyListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_LEFT:
		{
			PreviousField();
			break;
		}
		case VK_RIGHT:
		{
			NextField();
			break;
		}
		case VK_TAB:
		{
			if (::GetKeyState(VK_SHIFT) < 0)
			{
			/* Shift key is down. Do back tab. */
				PreviousField(TRUE);
			}
			else
			{
			/* Shift key is not down. Do forward tab. */
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
			AbortEdit();
			break;
		}
		case VK_UP:
		{
			PreviousField();
			break;
		}
		case VK_DOWN:
		{
			NextField();
			break;
		}
		default:
		{
//			od("ListBox: OnKeyDown(%x)\r\n", nChar);
			COwnerDrawListBox::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}
}

void CPropertyListBox::PreviousField(BOOL fOkToLeaveListBox /*=FALSE*/)
{
	int nCurSel = GetCurSel();
	int nCount = GetCount();

	if (nCurSel < 0)
	{
	/* Nothing selected yet. */
		if (nCount > 0)
		{
//			m_nField = PROPERTY_NameField;
			FinishEdit();
			SetCurSelNotify(GetTopIndex());
		}
	}
	else if ((m_nField == PROPERTY_NameField) || (!CanEditItem(nCurSel, PROPERTY_NameField)))
	{
	/* We need to go up one. */
		if (nCurSel > 0)
		{
			m_nField = CanEditItem(nCurSel-1, PROPERTY_ValueField)
							? PROPERTY_ValueField
							: PROPERTY_NameField;
			FinishEdit();
			SetCurSelNotify(nCurSel-1);
		}
		else if (fOkToLeaveListBox)
		{
			CDialog* pDialog = (CDialog*)GetParent();
			if (pDialog != NULL)
			{
				FinishEdit();
				pDialog->PrevDlgCtrl();
			}
		}
	}
}

void CPropertyListBox::NextField(BOOL fOkToLeaveListBox /*=FALSE*/)
{
	int nCurSel = GetCurSel();
	int nCount = GetCount();

	if (nCurSel < 0)
	{
	/* Nothing selected yet. */
		if (nCount > 0)
		{
//			m_nField = PROPERTY_NameField;
			FinishEdit();
			SetCurSelNotify(GetTopIndex());
		}
	}
	else if ((m_nField == PROPERTY_ValueField) || (!CanEditItem(nCurSel, PROPERTY_ValueField)))
	{
	/* We need to go down one. */
		if (nCurSel < nCount-1)
		{
			m_nField = CanEditItem(nCurSel+1, PROPERTY_NameField)
							? PROPERTY_NameField
							: PROPERTY_ValueField;
			FinishEdit();
			SetCurSelNotify(nCurSel+1);
		}
		else if (fOkToLeaveListBox)
		{
			CDialog* pDialog = (CDialog*)GetParent();
			if (pDialog != NULL)
			{
				FinishEdit();
				pDialog->NextDlgCtrl();
			}
		}
	}
}

void CPropertyListBox::RedrawItem(int nItem /*=-1*/)
{
	if (nItem < 0)
	{
		if ((nItem = GetCurSel()) < 0)
		{
		/* No current. Nothing to do. */
			return;
		}
	}

/*
// Get the item rect so we can invalidate it.
*/

	CRect crItem;

	GetItemRect(nItem, crItem);
	InvalidateRect(crItem);
}

void CPropertyListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_TAB:
		case VK_RETURN:
		case VK_ESCAPE:
		{
		/* Swallow it. */
			break;
		}
		default:
		{
			BOOL fHandled = FALSE;
			
			if (!Editing())
			{
				int nItem = GetCurSel();
				if (nItem >= 0)
				{
					fHandled = EditField(m_nField, nItem, nChar, nRepCnt, nFlags);
				}
			}
			
			if (!fHandled)
			{
			/* Pass it through. */
				COwnerDrawListBox::OnChar(nChar, nRepCnt, nFlags);
			}
		}
	}
}

BOOL CPropertyListBox::OnEraseBkgnd(CDC* pDC)
{
/*
// We only erase where we don't have items.
// The only time this can happen is if we have fewer items than will fit
// in the window.
*/

	CBrush cbBackground(GetSysColor(COLOR_WINDOW));

	CRect crClient;
	GetClientRect(&crClient);

	int nTopIndex = GetTopIndex();
	int nItemHeight = GetItemHeight(0);
	int nCount = GetCount();

	int nItemsPossible = crClient.Height()/nItemHeight;	// Ignore rounding.
	int nItemsShown = nCount - nTopIndex;

#if 0
	od("nTopIndex: %d, nCount: %d, nItemsShown: %d, nItemsPossible: %d\r\n",
		nTopIndex,
		nCount,
		nItemsShown,
		nItemsPossible);
#endif

	if (nItemsShown < nItemsPossible)
	{
	/* We need to draw white space below us. */
		CRect crErase;

		crErase.left = crClient.left;
		crErase.top = crClient.top + (nItemsShown*nItemHeight);
		crErase.right = crClient.right;
		crErase.bottom = crClient.bottom;

		pDC->FillRect(crErase, &cbBackground);
	}

	return TRUE;
}

void CPropertyListBox::Reframe(int nSel /*=-1*/)
{
	int nTopIndex = GetTopIndex();

	if (nSel == -1)
	{
	/* Just reframe here. This is useful for cleaning up after a delete. */
		SetTopIndex(nTopIndex);
	}
	else if (nSel < nTopIndex)
	{
		SetTopIndex(nSel);
	}
	else
	{
		CRect crItem;
		GetItemRect(nSel, crItem);

		CRect crClient;
		GetClientRect(crClient);

		if (crItem.top > crClient.top)
		{
		/* Scroll down. */
			int nDelta = (crItem.top - crClient.top)/crItem.Height();
			SetTopIndex(nTopIndex + nDelta);
		}
	}
}

void CPropertyListBox::OnEditorKillFocus(void)
{
	m_fEditorKillFocusPending = TRUE;
	PostMessage(WM_EDITOR_KILL_FOCUS);
}

LRESULT CPropertyListBox::OnEditorKillFocusNotify(WPARAM wParam, LPARAM lParam)
{
	m_fEditorKillFocusPending = FALSE;
	return FinishEdit();
}
