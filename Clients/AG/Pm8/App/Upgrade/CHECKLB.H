/*
// $Workfile: CHECKLB.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CHECKLB.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:32   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:02   JAY
// Initial revision.
// 
//    Rev 1.13   24 Feb 1995 14:35:08   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.12   20 Jan 1995 15:26:30   FRED
// New "free item marking" code.
// 
// 
//    Rev 1.11   19 Jan 1995 14:20:46   JAY
// New free pictures!
// 
//    Rev 1.10   22 Dec 1994 17:30:28   JAY
// Scrolling() method to help breathing functions.
// 
//    Rev 1.9   22 Dec 1994 15:16:50   FRED
//  
// 
//    Rev 1.8   22 Dec 1994 10:40:58   FRED
// 
//    Rev 1.7   22 Dec 1994 08:58:08   FRED
//  
// 
//    Rev 1.6   16 Dec 1994 09:52:36   FRED
//  
// 
//    Rev 1.5   15 Dec 1994 17:12:42   FRED
//  
// 
//    Rev 1.4   15 Dec 1994 13:09:24   FRED
//  
// 
//    Rev 1.3   14 Dec 1994 15:26:32   FRED
//  
// 
//    Rev 1.2   14 Dec 1994 10:18:06   FRED
//  
// 
//    Rev 1.1   13 Dec 1994 18:29:18   FRED
//  
// 
//    Rev 1.0   11 Dec 1994 11:03:04   FRED
// Initial revision.
*/

class CCheckListBoxItem;
class CProductCheckListBoxItem;
class CProductCheckListBox;

#ifdef WIN32
// CCheckListBox is already defined in WIN32.
#define CCheckListBox CMyCheckListBox
#endif

#ifndef __CHECKLB_H__
#define __CHECKLB_H__

// checklb.h : header file
//

#include "ownerlb.h"
#include "prod.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckListBoxItem object
//

class CCheckListBoxItem : public COwnerDrawListBoxItem
{

public:

	CCheckListBoxItem(int nIndent = 0);
	virtual ~CCheckListBoxItem();
	
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
// CProductCheckListBoxItem object
//

class CProductCheckListBoxItem : public CCheckListBoxItem
{

public:

	CProductCheckListBoxItem(CProductItem* pItem, int nIndent = 0);
	virtual ~CProductCheckListBoxItem();
	
	CProductItem* Item(void) const
		{ return m_pItem; }
	
	virtual void UpdateProductItem(void);
	
protected:

//	virtual void PriceString(CString& csPrice, long lPrice, BOOL fDollarSign = TRUE, BOOL fBlankIfZero = FALSE);
	
	CProductItem* m_pItem;
};

/////////////////////////////////////////////////////////////////////////////
// CCheckListBox object
//

#define	CHECK_LIST_BOX_DisableAutoMarkParent	(0x00000001L)

class CCheckListBox : public COwnerDrawListBox
{
public:
	enum
	{
		TIMER_ID	=	31415
	};
	
// Construction
public:
	CCheckListBox(DWORD dwStyle = 0);

// Attributes
public:

// Operations
public:
	virtual int GetSel(int nIndex) const;
	
	CCheckListBoxItem* GetListBoxItem(int nIndex) const;
	
	virtual BOOL Select(int nIndex, BOOL fMark, BOOL fDragging);
	virtual void MarkItem(CCheckListBoxItem* pItem, int nIndex, BOOL fMark);
	virtual void SelectItem(CCheckListBoxItem* pItem, int nIndex, BOOL fSelect);
	virtual void DisableItem(CCheckListBoxItem* pListBoxItem, int nIndex, BOOL fDisable);
	virtual void AdjustFree(void);
	virtual void AdjustChildren(int nIndex);
	virtual BOOL AdjustParents(int nIndex = 0);
	virtual int AdjustSelections(int nIndex = 0, int nIndent = 0, BOOL fForceDeselect = FALSE);
	virtual void MouseSelect(UINT nFlags, CPoint point);

	virtual BOOL Scrolling(void) const
		{ return m_fScrolling; }

	virtual BOOL SiblingIsMarked(int nIndex);

// Implementation
public:
	virtual ~CCheckListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CCheckListBox)
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

/////////////////////////////////////////////////////////////////////////////
// CProductCheckListBox object
//

class CProductCheckListBox : public CCheckListBox
{
// Construction
public:
	CProductCheckListBox(DWORD dwStyle = 0);

// Attributes
public:

// Operations
public:
	CProductCheckListBoxItem* GetListBoxItem(int nIndex) const;
	virtual CProductItem* GetItem(int nIndex) const;
	
	virtual void DisableLicensedItems(void);
	virtual void UpdateProductItems(void);

// Implementation
public:
	virtual ~CProductCheckListBox();

protected:
	// Generated message map functions
	//{{AFX_MSG(CProductCheckListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
