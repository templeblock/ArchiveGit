/*
// $Workfile: TCOMBO.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TCOMBO.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 3     7/23/98 10:07a Psasse
// New functionality for 640x480 screen to try and fit all items on screen
// 
// 2     5/20/98 6:16p Hforman
// added flag for only showing current level text instead of hierarchy
// 
//    Rev 1.0   14 Aug 1997 15:26:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:10   Fred
// Initial revision.
// 
//    Rev 1.8   23 Jun 1997 14:00:36   Fred
// Draws to offscreen bitmap to eliminate flicker
// 
//    Rev 1.7   11 Jun 1997 12:27:36   Jay
// Added const
// 
//    Rev 1.6   10 Jun 1997 13:36:18   Fred
// Added sorting capability
// 
//    Rev 1.5   04 Jun 1997 15:49:42   dennisp
// Added SetCurSel(CString), GetText(), SetDelimiter(), and GetDelimiter()
// 
//    Rev 1.4   21 May 1997 14:37:12   dennisp
// Added support to control sub-list color
// 
//    Rev 1.3   24 Apr 1997 15:28:20   Fred
//  
// 
//    Rev 1.2   22 Apr 1997 17:48:22   Jay
// Got rid of some extra stdafx.h includes.
// 
//    Rev 1.1   22 Apr 1997 13:17:46   Fred
// Start of project and art browsers
*/

class CTreeComboBox;
class CTreeComboList;
class CTreeComboItemArray;
class CTreeComboItemArraySortHelper;

#if !defined(AFX_TCOMBO_H__747881E1_A237_11D0_8D79_444553540000__INCLUDED_)
#define AFX_TCOMBO_H__747881E1_A237_11D0_8D79_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "qsort.h"


/////////////////////////////////////////////////////////////////////////////
// CTreeComboItemArraySort class

class CTreeComboItemArraySort : public CQuickSortBase
{
public:
	CTreeComboItemArraySort(void);
	~CTreeComboItemArraySort(void);

protected:
	// return number of elements (must be static during sort)
	virtual DWORD Size(void);

	// Compare two elements.
	virtual int Compare(DWORD dwLeft, DWORD dwRight);

	// Compare data item to an element.
	virtual int Compare(void* pData, DWORD dwIndex);

	// Exchange two elements.
	virtual void Exchange(DWORD dwLeft, DWORD dwRight);
};


/////////////////////////////////////////////////////////////////////////////
// CTreeComboItemArray class

// YIKES! Mulitple Inheritance!!!
class CTreeComboItemArray : public CPtrArray, public CTreeComboItemArraySort
{

public:
	class Item
	{
	public:
		Item(LPCSTR pszText, DWORD dwData = (DWORD)-1, CBitmap* pBitmap = NULL);
		virtual ~Item();

	public:
		CTreeComboItemArray* CreateItems(CTreeComboItemArray* pPreviousItems);
		CTreeComboItemArray* GetItems(void) const
			{ return m_pItems; }
		const CString& GetText(void) const
			{ return m_csText; }
		void SetData(DWORD dwData)
			{ m_dwData = dwData; }
		DWORD GetData(void) const
			{ return m_dwData; }
		void SetBitmap(CBitmap* pBitmap)
			{ m_pBitmap = pBitmap; }
		CBitmap* GetBitmap(void) const
			{ return m_pBitmap; }
		BOOL HasItems(void) const
			{ return (m_pItems != NULL) && (m_pItems->GetSize() > 0); }

	protected:
		CTreeComboItemArray* m_pItems;
		CString m_csText;
		DWORD m_dwData;
		CBitmap* m_pBitmap;
	};

public:
	CTreeComboItemArray(CTreeComboItemArray* pPreviousItems = NULL);
	virtual ~CTreeComboItemArray();

public:
	Item* GetAt(int nIndex) const
		{ return (Item*)CPtrArray::GetAt(nIndex); }
	Item*& ElementAt(int nIndex)
		{ return (Item*&)CPtrArray::ElementAt(nIndex); }
	Item*& operator[](int nIndex)
		{ return ElementAt(nIndex); }
	Item* operator[](int nIndex) const
		{ return GetAt(nIndex); }
	void DeleteAll();
	void DeleteAt(int nIndex, int nCount = 1);
	int Add(Item* pItem, BOOL fSorted = FALSE);
	void InsertAt(int nIndex, Item* pItem);

	int FindString(int nStartAfter, LPCSTR pszString, BOOL fExact = FALSE) const;

	CTreeComboItemArray* GetSubItems(void) const;

	int GetPreviousIndex(void) const;
	BOOL IsSelected(void) const;

	void GetColumnRect(int nColumn, CRect& crColumn);
	int GetColumnFromItem(int nItem);
	int GetItemInColumn(int nItem);
	int GetFirstItemInColumn(int nColumn);
	int GetLastItemInColumn(int nColumn);

	CTreeComboItemArray* m_pPreviousItems;
	CTreeComboList* m_pList;
	int m_nSelection;
	int m_nTopIndex;

	CRect m_crBounds;
	int m_nItemsPerColumn;
	int m_nNumberOfColumns;
	CDWordArray m_dwaColumnWidth;

	void Sort(void);

	BOOL m_fNeedsSorting;
};


/////////////////////////////////////////////////////////////////////////////
// CTreeComboBox window

class CTreeComboBox : public CWnd
{
// Construction
public:
	CTreeComboBox();

// Attributes
public:

// Operations
public:

	// Create control from scratch.
	virtual BOOL Create(DWORD dwStyle, const RECT& Bounds, CWnd* pParent, UINT uID);

	// Replace an existing control.
	virtual BOOL Create(DWORD dwStyle, CWnd* pParent, UINT uID);

	// Register and unregister control class. The class must be
	// registered before a CTreeCombo control can be created.
	static BOOL RegisterClass(void);
	static BOOL UnregisterClass(void);

	// Border and margin constants.
	static CSize m_Border;
	static CSize m_Margin;

	// Font management.
	void DeleteFont(void);

	// Button bitmap management.
	void SetButtonBitmap(UINT uID);
	void DeleteButtonBitmap(void);
	void DrawButton(CDC& dc);

	// Normal arrow bitmap management.
	void SetNormalArrowBitmap(UINT uID);
	void DeleteNormalArrowBitmap(void);

	// Normal arrow bitmap management.
	void SetSelectedArrowBitmap(UINT uID);
	void DeleteSelectedArrowBitmap(void);

	// Methods for showing and hiding list.
	BOOL IsListShown(void) const
		{ return m_fListShown; }
	void ShowList(BOOL fShow = TRUE);
	void ShowList(BOOL fShow, CTreeComboItemArray* pItems, BOOL fDeselect);

	// Combo box button management.
	BOOL IsButtonPushed(void) const
		{ return m_fButtonPushed; }
	void PushButton(BOOL fPush = TRUE);

	// Palette management.
	void SelectPalette(CDC& dc, CPalette*& pOldPalette);
	void DeselectPalette(CDC& dc, CPalette*& pOldPalette);

	// The main method for finishing the selection process.
	void CancelMode(void);

	// Destroy any orpahned list boxes. This function is called
	// by the "hide" timer handler to get rid of list boxes on
	// a delayed basis.
	void HideUnselectedItems(void);

	// The main internal function for setting or changing the selection
	// in a list box.
	BOOL SetSelection(int nSelect, CTreeComboItemArray* pItems, BOOL fImmediate = FALSE, BOOL fTopLevelSelection = TRUE);

	// Return a pointer to the item array in which the user is
	// currently making a selection. When the user navigates
	// using the keyboard, these are the items that are effected.
	CTreeComboItemArray* GetActiveItems(void) const;

	// Utility functions for unpacking "list" argument in Combo Box functions.
	const CTreeComboItemArray* GetConstItems(void* pList = NULL) const;
	CTreeComboItemArray* GetItems(void* pList = NULL) const;
	const CTreeComboItemArray::Item* GetConstItem(int nIndex, void* pList = NULL) const;
	CTreeComboItemArray::Item* GetItem(int nIndex, void* pList = NULL) const;

	// Return the "sub list" of a given item. This method is used
	// to move between the various levels in the tree combo box.
	void* GetItemList(int nIndex, void* pList = NULL) const;

	// The following methods mimic the normal combo box functionality.
	// In general, the parameters are the same with the addition of
	// a "pList" argument which specifies the list box on which to
	// perform the operation. For a normal combo box, it's clear which
	// list box to use since there's only one. For a tree combo box,
	// there are multiple levels of list boxes, and pList identifies
	// the target list box. If pList is NULL, the top level list box
	// is used. Use the GetItemList() method to get a pointer to the
	// sub list of a particular item.
	int GetCount(void* pList = NULL) const;
	int GetCurSel(void* pList = NULL) const;
	int SetCurSel(int nSelect, void* pList = NULL, BOOL fSelect = TRUE);
   int SetCurSel(const CString &csStringToFind);
	int SetItemData(int nIndex, DWORD dwData, void* pList = NULL);
	int SetItemDataPtr(int nIndex, void* pData, void* pList = NULL)
		{ return SetItemData(nIndex, (DWORD)pData, pList); }
	DWORD GetItemData(int nIndex, void* pList = NULL) const;
	void* GetItemDataPtr(int nIndex, void* pList = NULL) const
		{ return (void*)GetItemData(nIndex, pList); }
	int SetItemBitmap(int nIndex, CBitmap* pBitmap, void* pList = NULL);
	CBitmap* GetItemBitmap(int nIndex, void* pList = NULL) const;
	int GetTopIndex(void* pList = NULL) const;
	int SetTopIndex(int nIndex, void* pList = NULL);
	int GetLBText(int nIndex, LPSTR pszText, void* pList = NULL) const;
	int GetLBText(int nIndex, CString& csText, void* pList = NULL) const;
	int GetLBTextLen(int nIndex, void* pList = NULL) const;
   int GetText(int nIndex, CString &csText) const;
   // Calls to SetDelimiter should be made before filling it in.
   char SetDelimiter(char chDelimiter);
   char GetDelimiter() const;
   void SetShowCurLevelOnly(BOOL fSet = TRUE);
	void ShowDropDown(BOOL fShow = TRUE);
	BOOL GetDroppedState(void)
		{ return IsListShown(); }
	int AddString(LPCSTR pszString, DWORD dwData = (DWORD)-1, CBitmap* pBitmap = NULL, void* pList = NULL);
	int DeleteString(int nIndex, void* pList = NULL);
	int InsertString(int nIndex, LPCSTR pszString, DWORD dwData = (DWORD)-1, CBitmap* pBitmap = NULL, void* pList = NULL);
	void ResetContent(void* pList = NULL);
	int FindString(int nStartAfter, LPCSTR pszString, void* pList = NULL) const;
	int FindStringExact(int nStartAfter, LPCSTR pszString, void* pList = NULL) const;
	int SelectString(int nStartAfter, LPCSTR pszString, void* pList = NULL, BOOL fSelect = TRUE);
	int SelectStringExact(int nStartAfter, LPCSTR pszString, void* pList = NULL, BOOL fSelect = TRUE);

	// Painting methods called by list box windows. All drawing
	// is performed by the combo box.
	BOOL ListEraseBackground(CTreeComboList* pList, CDC& dc);
	void ListPaint(CTreeComboList* pList, CPaintDC& dc);
	void ListPaintItem(CTreeComboList* pList, CDC& dc, CTreeComboItemArray::Item* pItem, const CRect& crItem, BOOL fSelected);

	// Get the screen rectangle of a particular item.
	BOOL GetItemRect(CTreeComboItemArray* pItems, int nItem, CRect& crItem);

	// Invalidate screen rectangle of a particular item.
	void InvalidateItemRect(CTreeComboItemArray* pItems, int nItem);

	// Get the number of the item beneath a given point.
	int GetItemFromPoint(CTreeComboItemArray* pItems, CPoint& cpPoint);

	// Get a pointer to the list box window beneath a given point.
	CTreeComboList* GetListFromPoint(CPoint& cpPoint);

	// Figure out how to flow items in a list box given the current
	// screen geography. The tree combo box attempts to keep everything
	// on the screen and, in general, display items down and to the right.
	// If it needs to, it will flow items into multiple columns, up, and
	// even to the left.
	void Flow(CTreeComboItemArray* pItems = NULL);
	void FlowItems(CDC& dcFont, const CRect& crAvoid, const CRect& crScreen, CTreeComboItemArray* pItems, int nPositon);

	// Restore the list box selections that were saved when the
	// combo box list was initially shown. This is used, for example,
	// when the user presses ESC to restore the original selections.
	void RestoreCurrentSelections(void);

	// Make the current selections permanent. This is called when the
	// user performs some action that indicates they want to keep the
	// current selections.
	void CommitCurrentSelections(BOOL fNotify);

	// Set the delay timeouts for the showing and hiding item lists.
	// In general, the show and hide values should be the same. The
	// default values are 300ms for each.
	void SetTimeouts(UINT uShowTimeout, UINT uHideTimeout);

	// Timer management.
	void StartShowTimer(void);
	void StopShowTimer(void);
	void StartHideTimer(void);
	void StopHideTimer(void);

	// Sorting support.
	void SetSort(BOOL fSorted)
		{ m_fSorted = fSorted; }
	int GetSorted(void) const
		{ return m_fSorted; }
	void Sort(void);

public:

	// Flow types.
	enum
	{
		POSITION_Vertical		=	0,
		POSITION_Horizontal	=	1
	};

	// Default delay timeout values.
	enum
	{
		TICKS_SHOW				=	300,
		TICKS_HIDE				=	300
	};

	// Timer ID's.
	enum
	{
		TIMER_ID_SHOW			=	1000,
		TIMER_ID_HIDE			=	1001
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeComboBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void OnShowTimer(void);
	void OnHideTimer(void);

public:

	// Our palette.
	CPalette* m_pPalette;

	// Colors used by the control.
	COLORREF m_clWindow;
	COLORREF m_clChildWindow;
	COLORREF m_clWindowText;
	COLORREF m_clHighlight;
	COLORREF m_clHighlightText;
	COLORREF m_clBorderShadow;
	COLORREF m_clBorderHighlight;
	COLORREF m_clBorderLight;
	COLORREF m_clBorderDark;
	COLORREF m_clButtonFace;
	COLORREF m_clListBorder;

	// Important rectangles for the control.
	CRect m_crClient;
	CRect m_crBackground;
	CRect m_crButton;
	CRect m_crText;

	// When a mouse move message is received, its location is recorded
	// in m_LastPoint. This value is used to ignore mouse move messages
	// which are not actually a move. This turns out to be important when
	// the user is navigating the tree using the keyboard since Windows
	// will send a mouse move message to a window when it is uncovered.
	// As the user uses the arrow keys, list box windows can appear and
	// disappear. If the mouse happens to be in the right place, it will
	// receive a mouse move message and make a mouse selection when it
	// should not.
	// PMGTODO: Perhaps the mouse cursor should be hidden while the user
	// is using the arrow keys to navigate.
	BOOL m_fLastPointValid;
	CPoint m_LastPoint;

	// Font information.
	BOOL m_fFontValid;
	CFont m_Font;
	TEXTMETRIC m_TextMetric;
	int m_nItemHeight;

	// Flag indicating whether the list is shown or not.
	BOOL m_fListShown;

	// Button bitmap management. The m_fButtonCapture is used to indicate
	// a mode where the user clicked on the combo box button to drop down
	// the list. In this mode, moving the mouse on and off the button
	// will move the button in and out until either the user releases the
	// mouse button or moves over a list box.
	BOOL m_fButtonBitmapValid;
	CBitmap m_ButtonBitmap;
	int m_nButtonBitmapWidth;
	int m_nButtonBitmapHeight;
	BOOL m_fButtonPushed;
	BOOL m_fButtonCapture;

	// Normal arrow bitmap management.
	BOOL m_fNormalArrowBitmapValid;
	CBitmap m_NormalArrowBitmap;
	int m_nNormalArrowBitmapWidth;
	int m_nNormalArrowBitmapHeight;

	// Selected arrow bitmap management.
	BOOL m_fSelectedArrowBitmapValid;
	CBitmap m_SelectedArrowBitmap;
	int m_nSelectedArrowBitmapWidth;
	int m_nSelectedArrowBitmapHeight;

public:

	// Global static names of combo box and combo list box classes.
	static LPCSTR m_pszComboBoxClassName;
	static LPCSTR m_pszComboListClassName;

protected:

	// Base set of items.
	CTreeComboItemArray m_Items;

	// Pointer to active list box. This is set in OnMouseMove when
	// the mouse is moved over a list box.
	CTreeComboList* m_pActiveList;

	// This pointer is set to the items to show when the show timer
	// goes off.
	CTreeComboItemArray* m_pShowItems;

	// The current selection text. It is probably not a good idea to use
	// this since it will be changing to support bitmaps. It is better
	// to use GetCurSel() and GetItemList() to query the combo box
	// selections at each level.
	CString m_csCurrentSelectionText;

	// Determines whether to show text for the entire hierarchy separated
	// by delimiters, or just the bottom level text only.
	BOOL m_fShowCurLevelOnly;

   // Delimiter text used to separate items  
   char    m_chDelimiter;
   CString m_csDelimiter;  // Contains delimiter surrounded by spaces

	// Record of current selections. Note that since the various selections
	// are represented as a tree (as opposed to a network), naming just the
	// terminal selection defines the entire selection path.
	CTreeComboItemArray* m_pCurrentSelectionItems;
	int m_nCurrentSelectionIndex;

	// Current timeout delays.
	UINT m_uShowTimeout;
	UINT m_uHideTimeout;

	// Flags indicating whether timers are active.
	BOOL m_fShowTimerActive;
	BOOL m_fHideTimerActive;

	// Sorting support.
	BOOL m_fSorted;
};


/////////////////////////////////////////////////////////////////////////////
// CTreeComboList window

class CTreeComboList : public CWnd
{
	DECLARE_DYNAMIC(CTreeComboList)

// Construction
public:
	CTreeComboList(CTreeComboBox* pComboBox, CTreeComboItemArray* pItems);

// Attributes
public:

	void DeleteFont(void);

// Operations
public:

	virtual BOOL Create(DWORD dwStyle, const RECT& Bounds, CWnd* pParent, UINT uID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeComboList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeComboList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeComboList)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CTreeComboBox* m_pComboBox;
	CTreeComboItemArray* m_pItems;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TCOMBO_H__747881E1_A237_11D0_8D79_444553540000__INCLUDED_)
