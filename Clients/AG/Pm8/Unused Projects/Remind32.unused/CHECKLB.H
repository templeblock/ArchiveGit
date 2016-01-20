/*
// $Workfile: CHECKLB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/CHECKLB.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:46   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:12   Fred
// Initial revision.
*/

class CCheckedListBoxItem;
class CCheckedListBox;

#ifndef __CHECKLB_H__
#define __CHECKLB_H__

#include "ownerlb.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckedListBoxItem object
//

class CCheckedListBoxItem : public COwnerDrawListBoxItem
{

public:

	CCheckedListBoxItem(int nIndent = 0);
	virtual ~CCheckedListBoxItem();
	
public:

	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);
	
	virtual int Indent(void) const
		{ return m_nIndent; }
	
	virtual void Mark(BOOL fMark)
		{ m_fMark = fMark; }
	virtual BOOL Mark(void) const
		{ return m_fMark; }
	virtual void Select(BOOL fSelect)
		{ m_fSelect = fSelect; }
	virtual BOOL Select(void) const
		{ return m_fSelect; }
	virtual void Disable(BOOL fDisable)
		{ m_fDisable = fDisable; }
	virtual BOOL Disable(void) const
		{ return m_fDisable; }
		
protected:

	virtual void Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void ComputeColors(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground, COLORREF& clCheckBorder, COLORREF& clCheckBackground, COLORREF& clCheck);
	virtual void ComputeBounds(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crCheckBox, CRect& crItem);
	virtual void PaintBackground(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF clBackground);
	virtual void PaintCheckBox(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clBorder, COLORREF clBackground, COLORREF clCheck);
	virtual void PaintItem(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void Preview(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsChecked(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsSelected(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsFocused(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	
	int m_nIndent;
	BOOL m_fMark;
	BOOL m_fSelect;
	BOOL m_fDisable;
};

/////////////////////////////////////////////////////////////////////////////
// CCheckedListBox object
//

#define	CHECK_LIST_BOX_DisableAutoMarkParent	(0x00000001L)

class CCheckedListBox : public COwnerDrawListBox
{
public:
	enum
	{
		TIMER_ID	=	31415
	};
	
// Construction
public:
	CCheckedListBox(DWORD dwStyle = 0);

// Attributes
public:

// Operations
public:
	virtual int GetSel(int nIndex) const;
	
	CCheckedListBoxItem* GetListBoxItem(int nIndex) const;
	
	virtual BOOL Select(int nIndex, BOOL fMark, BOOL fDragging);
	virtual void MarkItem(CCheckedListBoxItem* pItem, int nIndex, BOOL fMark);
	virtual void SelectItem(CCheckedListBoxItem* pItem, int nIndex, BOOL fSelect);
	virtual void DisableItem(CCheckedListBoxItem* pListBoxItem, int nIndex, BOOL fDisable);
	virtual void AdjustFree(void);
	virtual void AdjustChildren(int nIndex);
	virtual BOOL AdjustParents(int nIndex = 0);
	virtual int AdjustSelections(int nIndex = 0, int nIndent = 0, BOOL fForceDeselect = FALSE);
	virtual void MouseSelect(UINT nFlags, CPoint point);
	
	virtual BOOL Scrolling(void) const
		{ return m_fScrolling; }

	virtual BOOL SiblingIsMarked(int nIndex);

protected:
	virtual BOOL RemoveSelectIfNotFocused(void);

// Implementation
public:
	virtual ~CCheckedListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CCheckedListBox)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void StartTimer(void);
	void StopTimer(void);
	
	int m_nFocus;
	DWORD m_dwStyle;
	BOOL m_fMark;
	int m_nCurrent;
	BOOL m_fScrolling;
};

#endif
