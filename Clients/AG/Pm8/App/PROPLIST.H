/*
// $Workfile: PROPLIST.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROPLIST.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:38   Fred
// Initial revision.
// 
//    Rev 1.1   16 Jul 1996 17:30:32   Fred
// More address/sender book stuff
// 
//    Rev 1.0   14 Mar 1996 13:30:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:08   FRED
// Initial revision.
// 
//    Rev 1.7   27 Aug 1995 12:35:28   FRED
// New fill-in-fields stuff
// 
//    Rev 1.6   06 Aug 1995 16:33:04   FRED
// More Fill-in Fields dialog stuff.
// 
//    Rev 1.5   11 Jul 1995 14:18:58   JAY
// Fill-in fields dialog.
// 
//    Rev 1.4   28 Apr 1995 16:11:40   JAY
// Validation, better kill focus support, more stability, etc.
// 
//    Rev 1.3   26 Apr 1995 14:40:48   JAY
//  
// 
//    Rev 1.2   25 Apr 1995 15:24:36   JAY
//  
// 
//    Rev 1.1   25 Apr 1995 08:58:02   JAY
// More refinements.
// 
//    Rev 1.0   19 Apr 1995 12:37:18   JAY
// Initial revision.
*/

class CPropertyEditor;

class CPropertyEdit;
class CPropertytStandardEditor;

class CPropertyListBoxItem;
class CPropertyListBox;

#ifndef __PROPLIST_H__
#define __PROPLIST_H__

#include "ownerlb.h"

#define WM_EDITOR_KILL_FOCUS		WM_USER+0x1000

// proplist.h : header file
//

enum EPropertyField
{
	PROPERTY_UnknownField = -1,
	PROPERTY_NameField = 0,
	PROPERTY_SplitterField,
	PROPERTY_ValueField,
	NUMBER_OF_PROPERTY_FIELDS
};

#define GRID_THICKNESS	1

/////////////////////////////////////////////////////////////////////////////
// CPropertyEditor window

class CPropertyEditor
{
public:
	virtual ~CPropertyEditor() {}

	virtual void SetText(LPCSTR pText) = 0;
	virtual void GetText(CString& csText) = 0;
	virtual void SetFocus(void) = 0;
	virtual CPropertyEdit* GetEditor(void) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyEdit window

class CPropertyEdit : public CEdit
{
// Construction
public:
	CPropertyEdit();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CPropertyEdit();

protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyStandardEditor window

class CPropertyStandardEditor : public CPropertyEditor
{
public:
	CPropertyStandardEditor(const CRect& crPosition, CWnd* pParent);

	virtual void SetText(LPCSTR pText);
	virtual void GetText(CString& csText);
	virtual void SetFocus(void);
	virtual CPropertyEdit* GetEditor(void)
		{ return &m_Edit; }
protected:
	CPropertyEdit m_Edit;
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBoxItem

class CPropertyListBoxItem : public COwnerDrawListBoxItem
{
// Construction
public:
/* Field access. */

	virtual CString& GetFieldText(int nField, CString& csField) = 0;
	virtual void SetFieldText(int nField, LPCSTR pNewField) = 0;
	virtual BOOL CanEditField(int nField) = 0;

public:

	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);
protected:
	virtual void Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void PaintField(int nField, CPropertyListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void ComputeColors(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground);
	virtual void ComputeBounds(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crField);
	virtual void PaintGrid(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void PaintBackground(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crField, COLORREF clBackground);
	virtual void PaintItem(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintString(LPCSTR pString, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect* pRect = NULL);
	virtual BOOL IsSelected(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsFocused(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyListBox window

class CPropertyListBox : public COwnerDrawListBox
{
// Construction
public:
	CPropertyListBox();

// Attributes
public:

	int SplitterOffset(void) const
		{ return m_nSplitter; }
	void SplitterOffset(int nSplitter)
		{ m_nSplitter = nSplitter; }
	CFont& Font(void)
		{ return m_Font; }
	int Field(void) const			// Either NameField or ValueField
		{ return m_nField; }
	void Field(int nField)
		{ m_nField = nField; }
	COLORREF GridColor(void) const
		{ return m_clGrid; }
	void GridColor(COLORREF clGrid)
		{ m_clGrid = clGrid; }

	int FindField(int x);

	CPropertyEditor* GetEditor(void) const
		{ return m_pEditor; }

	virtual void FieldExtent(int nField, int& nLeft, int& nRight, int nBoundLeft = 0);

	BOOL SubclassDlgItem(UINT nID, CWnd* pParent);

	void RedrawItem(int nItem = -1);

/* Edit support. */

	virtual BOOL EditField(int nField, int nSelection, UINT nChar = 0, UINT nRepCnt = 0, UINT nFlags = 0);
	virtual BOOL BeginEdit(int nCurSel = -1);
	virtual BOOL FinishEdit(BOOL fValidate = TRUE, BOOL fByReturn = FALSE);
	virtual void AbortEdit(void);

	virtual BOOL OnFinishEdit(BOOL fValidate = TRUE, BOOL fByReturn = FALSE);
	virtual void OnAbortEdit(void);

	virtual BOOL Editing(void);

/* Helps prevent flicker of highlights. (So it's a little hackish...) */

	virtual BOOL PreparingToEdit(void);

// Operations
public:

	void SetCurSelNotify(int nNewSel);
	void Reframe(int nSel = -1);		// -1 means current top.

	virtual void PreviousField(BOOL fOkToLeaveListBox = FALSE);
	virtual void NextField(BOOL fOkToLeaveListBox = FALSE);

// Implementation
public:
	virtual ~CPropertyListBox();
	virtual void MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct);

	virtual void OnEditorKillFocus(void);

protected:
	int m_nSplitter;
	CFont m_Font;
	int m_nField;				// Which field are we editing?
	COLORREF m_clGrid;
	CPropertyEditor* m_pEditor;
	BOOL m_fPreparingToEdit;
	BOOL m_fEditorKillFocusPending;		// WM_EDITOR_KILL_FOCUS in flight

	virtual CPropertyEditor* CreateEditor(int nField, const CRect& crPosition);
	virtual void DestroyEditor(CPropertyEditor*& pEditor);

	virtual BOOL CanEditItem(int nItem, int nField);

	// Generated message map functions
	//{{AFX_MSG(CPropertyListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	// KILL_FOCUS notification from edit field.
	afx_msg LRESULT OnEditorKillFocusNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif			// #ifndef __PROPLIST_H__
