/*
// $Workfile: ADDRLIST.H $
// $Revision: 1 $
// $Date: 3/03/99 6:02p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRLIST.H $
// 
// 1     3/03/99 6:02p Gbeddow
// 
// 2     5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
//    Rev 1.0   14 Aug 1997 15:18:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:38   Fred
// Initial revision.
// 
//    Rev 1.14   08 Aug 1996 11:12:30   Fred
// 16-bit fixes
// 
//    Rev 1.13   02 Aug 1996 15:02:18   Fred
// 16-bit changes
// 
//    Rev 1.12   01 Aug 1996 09:22:36   Fred
// More 16-bit changes
// 
//    Rev 1.11   17 Jul 1996 09:04:54   Fred
// Some address book improvements
// 
//    Rev 1.10   11 Jul 1996 08:37:26   Fred
// More address book features
// 
//    Rev 1.9   06 Jul 1996 09:25:14   Fred
// Picture editor and other improvements
// 
//    Rev 1.8   01 Jul 1996 07:30:20   Fred
// Receiver list
// 
//    Rev 1.7   27 Jun 1996 10:42:48   Fred
// Receiver list. Before removing receiver from non-receiver views
// 
//    Rev 1.6   24 Jun 1996 00:29:50   Fred
// Address Book Overview
// 
//    Rev 1.5   21 Jun 1996 09:29:50   Fred
// More overview list
// 
//    Rev 1.4   21 Jun 1996 07:00:50   Fred
// Start of overview list box
// 
//    Rev 1.3   18 Jun 1996 15:20:34   Fred
// Field value list management and editing
// 
//    Rev 1.2   18 Jun 1996 08:25:04   Fred
// More adress book changes
// 
//    Rev 1.1   10 Jun 1996 18:43:46   Fred
// Memo editor
// 
//    Rev 1.0   10 Jun 1996 08:22:08   Fred
// Initial revision.
*/

class CSplitterCursor;
class CAddressBookFieldListBoxItem;
class CAddressBookFieldListBox;
class COverviewListBoxItem;
class COverviewListBox;
class CAddressBookOverviewListBox;
class CReceiverListBoxItem;
class CReceiverListBox;
class CAddressBookReceiverListBox;

#ifndef __ADDRLIST_H__
#define __ADDRLIST_H__

#include "flatdb.h"
#include "ownerlb.h"
#include "splitter.h"
#include "addredit.h"

// Background color for field names.
#define	FIELD_BACKGROUND_COLOR	PALETTEINDEX(160)

// Timer ID for receiver list box scroll.
#define	SCROLL_TIMER	(12345)

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldListBoxItem

class CAddressBookFieldListBoxItem : public COwnerDrawListBoxItem
{
	INHERIT(CAddressBookFieldListBoxItem, COwnerDrawListBoxItem)

public:
	CAddressBookFieldListBoxItem(CAddressBookFieldListBox* pListBox, CFlatFileDatabaseField* pField);
	virtual ~CAddressBookFieldListBoxItem();

public:
	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);

protected:
	virtual void Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);

public:
	CFlatFileDatabaseField* GetField(void) const
		{ return m_pField; }

	void Edit(void);
	void Reset(void);
	void Move(const CRect& crPosition);
	BOOL IsEditing(void);
	BOOL IsModified(void);
	void SetModified(BOOL fModified = TRUE);
	void Save(void);
	void Refresh(void);
	void Redraw(void);
	void ExcludeClipRect(CDC* pParentDC);
	void Show(BOOL fShow = TRUE);
	virtual void EditRectangle(CRect& crRect);

protected:
	void CreateEditor(void);
	CAddressBookFieldListBox* m_pListBox;
	CFlatFileDatabaseField* m_pField;
	CAddressBookFieldEditor* m_pEditor;
	CRect m_crName;
	CRect m_crValue;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldListBox

class CAddressBookFieldListBox : public COwnerDrawListBox
{
	INHERIT(CAddressBookFieldListBox, COwnerDrawListBox)

public:
	CAddressBookFieldListBox();
	virtual ~CAddressBookFieldListBox();

	BOOL SubclassDlgItem(UINT nID, CWnd* pParent);

	int GetSplitter(void)
		{ return m_nSplitter; }
	void SetSplitter(int nSplitter);
	int GetLineHeight(void)
		{ return m_nLineHeight; }
	int GetCharWidth(void)
		{ return m_nCharWidth; }

	void Edit(int nIndex = -1);
	void Reset(void);
	void Move(void);
	BOOL IsModified(void);
	void SetModified(BOOL fModified = TRUE);
	void Save(void);
	void Refresh(void);
	void Show(BOOL fShow = TRUE);

	BOOL IsEditing(void)
		{ return m_nEditIndex != -1; }

	void BackField(BOOL fMoveOffListBox);
	void NextField(BOOL fMoveOffListBox);

	void RefreshItem(int nItem = -1);

	int GetFieldIndex(CFlatFileDatabaseField* pField);

	DWORD GetFieldCount(void);
	CFlatFileDatabaseField* GetField(DWORD dwField);

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg LRESULT OnEditorActivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditorDeactivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFieldList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetReceiverList(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	int m_nSplitter;
	int m_nLineHeight;
	int m_nCharWidth;

	CSplitterCursor m_SplitterCursor;

	int m_nEditIndex;
};

//////////////////////////////////////////////////////////////////////////
// COverviewPositionArray

class COverviewPositionArray : public CPtrArray
{
	INHERIT(COverviewPositionArray, CPtrArray)

public:
	enum
	{
		MinimumWidth	=	8,
		MinimumPadding	=	4
	};

	// Array item
	class Item
	{
	public:
		Item(DWORD dwField, int nWidth, CAddressBookFieldEditor* pEditor);
		virtual ~Item();

	public:
		DWORD GetField(void)
			{ return m_dwField; }
		int GetWidth(void)
			{ return m_nWidth; }
		void SetWidth(int nWidth)
			{ m_nWidth = nWidth; }
		CAddressBookFieldEditor* GetEditor(void)
			{ return m_pEditor; }

	protected:
		DWORD m_dwField;
		int m_nWidth;
		CAddressBookFieldEditor* m_pEditor;	// field editor (object owned by array item)
	};

public:

	// construction and destruction
	COverviewPositionArray();
	virtual ~COverviewPositionArray();
	
public:

	void AddField(DWORD dwField, int nWidth, CAddressBookFieldEditor* pEditor);
	DWORD GetField(int nIndex);
	int GetWidth(int nIndex);
	void SetWidth(int nIndex, int nWidth);
	int GetOffset(int nIndex);
	void SetOffset(int nIndex, int nOffset);
	int GetTotalWidth(void)
		{ return GetOffset(GetSize()); }
	int GetIndex(int nOffset);
	void SetIndex(int nIndex, int nNewIndex);
	CAddressBookFieldEditor* GetEditor(int nIndex);

public:

	void DeleteAll(void);
	void DeleteAt(int nIndex, int nCount = 1);

	Item* GetAt(int nIndex) const
		{ return (Item*)(INHERITED::GetAt(nIndex)); }
	Item*& ElementAt(int nIndex)
		{ return (Item*&)(INHERITED::ElementAt(nIndex)); }
	Item* operator[](int nIndex) const
		{ return (Item*)(INHERITED::GetAt(nIndex)); }
	Item*& operator[](int nIndex)
		{ return (Item*&)(INHERITED::ElementAt(nIndex)); }
};

//////////////////////////////////////////////////////////////////////////
// COverviewListBoxItem

class COverviewListBoxItem : public COwnerDrawListBoxItem
{
	INHERIT(COverviewListBoxItem, COwnerDrawListBoxItem)

public:
	COverviewListBoxItem(CAddressBookOverviewListBox* pListBox);
	virtual ~COverviewListBoxItem();

public:
	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);

protected:
	virtual void Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);

protected:
	CAddressBookOverviewListBox* m_pListBox;
};

//////////////////////////////////////////////////////////////////////////
// COverviewListBox

class COverviewListBox : public COwnerDrawListBox
{
	INHERIT(COverviewListBox, COwnerDrawListBox)

public:
	COverviewListBox();
	virtual ~COverviewListBox();
	
	BOOL CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID);

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(COverviewListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnEditorActivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditorDeactivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFieldList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetReceiverList(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void RedrawItem(int nItem);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookOverviewListBox

class CAddressBookOverviewListBox : public CWnd
{
	INHERIT(CAddressBookOverviewListBox, CWnd)

public:
	CAddressBookOverviewListBox();
	virtual ~CAddressBookOverviewListBox();

	CFlatFileDatabaseCursor* GetDatabaseCursor(void)
		{ return m_pDatabaseCursor; }
	CFlatFileDatabaseCursor* GetSearchCursor(void)
		{ return &m_SearchCursor; }
	void SetDatabaseCursor(CFlatFileDatabaseCursor* pCursor);
	COverviewPositionArray* GetPositions(void)
		{ return &m_Positions; }
	int GetLineHeight(void)
		{ return m_nLineHeight; }
	int GetCharWidth(void)
		{ return m_nCharWidth; }
	int GetHeaderHeight(void)
		{ return 3+m_nLineHeight+3+1; }
	int GetSelectWidth(void)
		{ return 3+m_nLineHeight+3+1; }
	int GetItemHeight(void)
		{ return 3+m_nLineHeight+3+1; }
	DWORD GetFirstRecord(void)
		{ return m_dwFirstRecord; }
	void AddField(DWORD dwField, CDC& dc);
	void UpdateScrollBars(void);
	int GetHorizontalOffset(void);
	void BeforeRecordChange(void);
	void RecordModified(DWORD dwStartPosition, DWORD dwEndPosition);
	void AfterRecordChange(void);
	void ActivateView(BOOL fActive = TRUE);
	void Reframe(BOOL fVertical = TRUE, BOOL fHorizontal = FALSE);
	void PositionEditors(void);

	void Edit(int nIndex = -1);
	BOOL IsModified(void);
	void SetModified(BOOL fModified = TRUE);
	void Save(void);
	void Refresh(void);

	BOOL IsEditing(void)
		{ return m_nEditIndex != -1; }

	void BackField(BOOL fMoveOffRecord);
	void NextField(BOOL fMoveOffRecord);

	void BackPage(void);
	void NextPage(void);

	BOOL MoveRecord(long lDelta);

	void RefreshItem(int nItem = -1);

	int GetEditorIndex(CAddressBookFieldEditor* pEditor);

	DWORD GetFieldCount(void);
	CFlatFileDatabaseField* GetField(DWORD dwField);

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookOverviewListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg LRESULT OnEditorActivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditorDeactivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFieldList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetReceiverList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void DeleteFont(void);
	
	CFlatFileDatabaseCursor* m_pDatabaseCursor;
	CFlatFileDatabaseCursor m_SearchCursor;
	int m_nLineHeight;
	int m_nCharWidth;

	CSplitterCursor m_SplitterCursor;
	int m_nSplitterIndex;

	COverviewListBox m_List;
	COverviewPositionArray m_Positions;

	DWORD m_dwFirstRecord;
	DWORD m_dwMaximumRecord;
	DWORD m_dwRecordsShowing;
	DWORD m_dwFullRecordsShowing;

	DWORD m_dwRecordModifiedStart;
	DWORD m_dwRecordModifiedEnd;

	BOOL m_fAddingNewRecord;

	HFONT m_hFont;

	int m_nEditIndex;
};

//////////////////////////////////////////////////////////////////////////
// CReceiverListBoxItem

class CReceiverListBoxItem : public COwnerDrawListBoxItem
{
	INHERIT(CReceiverListBoxItem, COwnerDrawListBoxItem)

public:
	CReceiverListBoxItem(CAddressBookReceiverListBox* pListBox);
	virtual ~CReceiverListBoxItem();

public:
	virtual void DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);
	virtual int CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct);
	virtual void DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct);

protected:
	virtual void Paint(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);

protected:
	CAddressBookReceiverListBox* m_pListBox;
};

//////////////////////////////////////////////////////////////////////////
// CReceiverListBox

class CReceiverListBox : public COwnerDrawListBox
{
	INHERIT(CReceiverListBox, COwnerDrawListBox)

public:
	CReceiverListBox();
	virtual ~CReceiverListBox();

	BOOL CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID);
	
protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CReceiverListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void RedrawItem(int nItem);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookReceiverListBox

class CAddressBookReceiverListBox : public CWnd
{
	INHERIT(CAddressBookReceiverListBox, CWnd)

public:
	CAddressBookReceiverListBox();
	virtual ~CAddressBookReceiverListBox();

	CFlatFileDatabaseCursor* GetDatabaseCursor(void)
		{ return m_pDatabaseCursor; }
	CFlatFileDatabaseCursor* GetSearchCursor(void)
		{ return &m_SearchCursor; }
	void SetDatabaseCursor(CFlatFileDatabaseCursor* pCursor);
	int GetLineHeight(void)
		{ return m_nLineHeight; }
	int GetCharWidth(void)
		{ return m_nCharWidth; }
	int GetHeaderHeight(void)
		{ return 3+m_nLineHeight+3+1; }
	int GetItemHeight(void)
		{ return 3+m_nLineHeight+3+1; }
	int GetSelectWidth(void)
//		{ return 0; /*3+m_nLineHeight+3+1;*/ }
		{ return 0; }
	DWORD GetFirstRecord(void)
		{ return m_dwFirstRecord; }
	void UpdateScrollBars(void);
	void BeforeRecordChange(void);
	void AfterRecordChange(void);
	void ActivateView(BOOL fActive = TRUE);
	void Reframe(void);
	BOOL SetFirstRecord(DWORD dwFirstRecord);
	void RefreshFocus(BOOL fRedraw = TRUE);
	BOOL IsReceiver(CFlatFileDatabaseCursor* pCursor);
	void RefreshSelectCount(void);

	void BackPage(void);
	void NextPage(void);

	BOOL MoveRecord(long lDelta);

protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookReceiverListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnGetReceiverList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void DeleteFont(void);
	
	CFlatFileDatabaseCursor* m_pDatabaseCursor;
	CFlatFileDatabaseCursor m_SearchCursor;
	int m_nLineHeight;
	int m_nCharWidth;

	CReceiverListBox m_List;

	DWORD m_dwFirstRecord;
	DWORD m_dwMaximumRecord;
	DWORD m_dwRecordsShowing;
	DWORD m_dwFullRecordsShowing;
	DWORD m_dwSaveFullRecordsShowing;

	BOOL m_fDragging;
	BOOL m_fSelectReceiver;
	DWORD m_dwSelectStart;
	DWORD m_dwSelectEnd;
	BOOL m_fScrollTimer;
	int m_nScrollDirection;

	HFONT m_hFont;

	int m_nListBoxIndex;
};

#endif
