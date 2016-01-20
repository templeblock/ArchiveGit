/*
// $Workfile: ADDREDIT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDREDIT.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 2     7/11/98 2:48p Jayn
// Got rid of some resource leaks.
// 
//    Rev 1.0   14 Aug 1997 15:18:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:38   Fred
// Initial revision.
// 
//    Rev 1.10   08 Aug 1996 11:10:40   Fred
// 16-bit fixes
// 
//    Rev 1.9   01 Aug 1996 09:22:36   Fred
// More 16-bit changes
// 
//    Rev 1.8   31 Jul 1996 11:57:44   Fred
// 16-bit compatibility
// 
//    Rev 1.7   17 Jul 1996 19:08:32   Fred
// Picture blobs now have compression type
// 
//    Rev 1.6   16 Jul 1996 08:50:50   Fred
// Hooked up address/sender book
// 
//    Rev 1.5   06 Jul 1996 09:25:22   Fred
// Picture editor and other improvements
// 
//    Rev 1.4   03 Jul 1996 18:52:24   Fred
// Picture editor
// 
//    Rev 1.3   01 Jul 1996 07:30:20   Fred
// Receiver list
// 
//    Rev 1.2   27 Jun 1996 10:42:50   Fred
// Receiver list. Before removing receiver from non-receiver views
// 
//    Rev 1.1   24 Jun 1996 19:16:20   Fred
// Some fixes and improvments
// 
//    Rev 1.0   24 Jun 1996 00:31:16   Fred
// Initial revision.
*/

class CAddressBookFieldEditor;
class CAddressBookFieldEditorNormalWrapper;
class CAddressBookFieldEditorListWrapper;
class CAddressBookFieldEditorMemoWrapper;
class CAddressBookFieldEditorPictureWrapper;
class CAddressBookFieldEditorNormal;
class CAddressBookFieldEditorListBox;
class CAddressBookFieldEditorListButton;
class CAddressBookFieldEditorList;
class CAddressBookFieldEditorMemo;
class CAddressBookFieldEditorPictureButton;
class CAddressBookFieldEditorPicture;

#ifndef __ADDREDIT_H__
#define __ADDREDIT_H__

#include "flatdb.h"
#include "addrbook.h"

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditor
//
// The base wrapper class for a field editor.

class CAddressBookFieldEditor
{
public:
	CAddressBookFieldEditor(CFlatFileDatabaseField* pField);
	virtual ~CAddressBookFieldEditor();

public:
	// Create a proper wrapper class given the field type.
	static CAddressBookFieldEditor* Create(CFlatFileDatabaseField* pField, CWnd* pParent);

public:
	virtual void Reset(void) = 0;
	virtual void Move(const CRect& crPosition);
	virtual BOOL IsEditing(void);
	virtual BOOL IsModified(void) = 0;
	virtual void SetModified(BOOL fModified = TRUE) = 0;
	virtual void Save(void);
	virtual void Refresh(void);
	virtual void Redraw(void);
	virtual void ExcludeClipRect(CDC* pParentDC);
	virtual void Show(BOOL fShow = TRUE);
	virtual void SetFocus(void);
	virtual void BringToTop(CWnd* pInsertAfter = NULL);

public:
	CFlatFileDatabaseField* GetField(void) const
		{ return m_pField; }
	CWnd* GetEditor(void) const
		{ return m_pEditor; }

protected:
	virtual void CreateEditor(CWnd* pParent) = 0;
	void DestroyEditor(void);

protected:
	CFlatFileDatabaseField* m_pField;
	CWnd* m_pEditor;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorNormalWrapper
//
// The wrapper class for the normal field editor.

class CAddressBookFieldEditorNormalWrapper : public CAddressBookFieldEditor
{
	INHERIT(CAddressBookFieldEditorNormalWrapper, CAddressBookFieldEditor)

public:
	CAddressBookFieldEditorNormalWrapper(CFlatFileDatabaseField* pField);
	virtual ~CAddressBookFieldEditorNormalWrapper();

public:
	virtual void Reset(void);
	virtual BOOL IsModified(void);
	virtual void SetModified(BOOL fModified = TRUE);

public:
	CAddressBookFieldEditorNormal* GetEditor(void) const
		{ return (CAddressBookFieldEditorNormal*)INHERITED::GetEditor(); }

protected:
	virtual void CreateEditor(CWnd* pParent);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListWrapper
//
// The wrapper class for the list field editor.

class CAddressBookFieldEditorListWrapper : public CAddressBookFieldEditorNormalWrapper
{
	INHERIT(CAddressBookFieldEditorListWrapper, CAddressBookFieldEditorNormalWrapper)

public:
	CAddressBookFieldEditorListWrapper(CFlatFileDatabaseField* pField);
	virtual ~CAddressBookFieldEditorListWrapper();

public:
	virtual void Move(const CRect& crPosition);
	virtual void Redraw(void);
	virtual void ExcludeClipRect(CDC* pParentDC);
	virtual void Show(BOOL fShow = TRUE);
	virtual void BringToTop(CWnd* pInsertAfter = NULL);

public:
	CAddressBookFieldEditorList* GetEditor(void) const
		{ return (CAddressBookFieldEditorList*)INHERITED::GetEditor(); }

protected:
	virtual void CreateEditor(CWnd* pParent);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorMemoWrapper
//
// The wrapper class for the memo field editor.

class CAddressBookFieldEditorMemoWrapper : public CAddressBookFieldEditor
{
	INHERIT(CAddressBookFieldEditorMemoWrapper, CAddressBookFieldEditor)

public:
	CAddressBookFieldEditorMemoWrapper(CFlatFileDatabaseField* pField);
	virtual ~CAddressBookFieldEditorMemoWrapper();

public:
	virtual void Reset(void);
	virtual BOOL IsModified(void);
	virtual void SetModified(BOOL fModified = TRUE);
	virtual void Save(void);
	virtual void Refresh(void);

public:
	CAddressBookFieldEditorMemo* GetEditor(void) const
		{ return (CAddressBookFieldEditorMemo*)INHERITED::GetEditor(); }

protected:
	virtual void CreateEditor(CWnd* pParent);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPictureWrapper
//
// The wrapper class for the picture field editor.

class CAddressBookFieldEditorPictureWrapper : public CAddressBookFieldEditor
{
	INHERIT(CAddressBookFieldEditorPictureWrapper, CAddressBookFieldEditor)

public:
	CAddressBookFieldEditorPictureWrapper(CFlatFileDatabaseField* pField);
	virtual ~CAddressBookFieldEditorPictureWrapper();

public:
	virtual void Reset(void);
	virtual void Move(const CRect& crPosition);
	virtual BOOL IsModified(void);
	virtual void SetModified(BOOL fModified = TRUE);
	virtual void Save(void);
	virtual void Refresh(void);

public:
	CAddressBookFieldEditorPicture* GetEditor(void) const
		{ return (CAddressBookFieldEditorPicture*)INHERITED::GetEditor(); }

protected:
	virtual void CreateEditor(CWnd* pParent);

	BOOL SavePictureBlob(CFlatFileDatabase* pDatabase, LPCSTR pszName, LPBITMAPINFO pData, DB_RECORD_NUMBER& dwNumber);
	BOOL LoadPictureBlob(CFlatFileDatabase* pDatabase, CString& csName, LPBITMAPINFO& pData, DB_RECORD_NUMBER dwNumber);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorNormal
//
// The "normal" field editor which lets users type in text.

class CAddressBookFieldEditorNormal : public CEdit
{
	INHERIT(CAddressBookFieldEditorNormal, CEdit)

public:
	CAddressBookFieldEditorNormal(CAddressBookFieldEditor* pWrapper, CWnd* pParent);
	virtual ~CAddressBookFieldEditorNormal();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
public:
	// Calls to common wrapper functions.
	CFlatFileDatabaseField* GetField(void)
		{ return GetWrapper()->GetField(); }
	void Reset(void)
		{ GetWrapper()->Reset(); }
	BOOL IsEditing(void)
		{ return GetWrapper()->IsEditing(); }
	BOOL IsModified(void)
		{ return GetWrapper()->IsModified(); }
	void SetModified(BOOL fModified = TRUE)
		{ GetWrapper()->SetModified(fModified); }
	void Save(void)
		{ GetWrapper()->Save(); }
	void Refresh(void)
		{ GetWrapper()->Refresh(); }
	void Redraw(void)
		{ GetWrapper()->Redraw(); }
	void Show(BOOL fShow = TRUE)
		{ GetWrapper()->Show(fShow); }
		
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorNormal)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	CAddressBookFieldEditor* GetWrapper(void) const
		{ return m_pWrapper; }

protected:
	CAddressBookFieldEditor* m_pWrapper;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListBox
//
// The list box used by the list field editor.

class CAddressBookFieldEditorListBox : public CListBox
{
	INHERIT(CAddressBookFieldEditorListBox, CListBox)

public:
	CAddressBookFieldEditorListBox();
	virtual ~CAddressBookFieldEditorListBox();

	BOOL CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID);
	
protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	int Index(const CPoint& cpPoint);

protected:
	void SelectIfChanged(BOOL fMouseSelected, int nOldSelection = LB_ERR);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorListButton
//
// The list box drop button used by the list field editor.
//
// Originally, this was an owner draw button, but there were
// problems in assigning unique ID's so that the ownerdraw mechanism
// could distinguish between different buttons. The solution was to
// base the control on a generic CWnd and trapping WM_PAINT.

class CAddressBookFieldEditorListButton : public CWnd
{
	INHERIT(CAddressBookFieldEditorListButton, CWnd)

public:
	CAddressBookFieldEditorListButton();
	virtual ~CAddressBookFieldEditorListButton();

	BOOL CreateControl(DWORD dwStyle, CWnd* pParent, UINT nID);
	
	// Set bitmap to use.
	void SetBitmap(CBitmap* pBitmap);

	// Get/Set select state.
	BOOL GetSelect(void)
		{ return m_fSelect; }
	void SetSelect(BOOL fSelect = TRUE);

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorListButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBitmap* m_pBitmap;

	int m_nBitmapWidth;
	int m_nBitmapHeight;

	BOOL m_fSelect;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorList
//
// The list field editor which lets users type in text or select
// from a list (like a combo box.)

class CAddressBookFieldEditorList : public CAddressBookFieldEditorNormal
{
	INHERIT(CAddressBookFieldEditorList, CAddressBookFieldEditorNormal)

public:
	CAddressBookFieldEditorList(CAddressBookFieldEditor* pWrapper, CWnd* pParent);
	virtual ~CAddressBookFieldEditorList();

public:
	void Move(const CRect& crPosition);
	void Show(BOOL fShow = TRUE);
	CAddressBookFieldEditorListButton* GetButton(void)
		{ return &m_Button; }
	CAddressBookFieldEditorListBox* GetListBox(void)
		{ return &m_List; }

protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorList)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	afx_msg LRESULT OnListItemSelected(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListButtonClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	// Button.
	CAddressBookFieldEditorListButton m_Button;
	CBitmap m_bmButton;
	BOOL m_fButtonHidden;

	// List.
	CAddressBookFieldEditorListBox m_List;
	void ListRectangle(CRect& crList);
	void HideList(void);
	void ShowList(void);
	void FindString(void);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorMemo
//
// The memo field editor which lets users type in multi-line text.

class CAddressBookFieldEditorMemo : public CEdit
{
	INHERIT(CAddressBookFieldEditorMemo, CEdit)

public:
	CAddressBookFieldEditorMemo(CAddressBookFieldEditor* pWrapper, CWnd* pParent);
	virtual ~CAddressBookFieldEditorMemo();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
public:
	// Calls to common wrapper functions.
	CFlatFileDatabaseField* GetField(void)
		{ return GetWrapper()->GetField(); }
	void Reset(void)
		{ GetWrapper()->Reset(); }
	BOOL IsEditing(void)
		{ return GetWrapper()->IsEditing(); }
	BOOL IsModified(void)
		{ return GetWrapper()->IsModified(); }
	void SetModified(BOOL fModified = TRUE)
		{ GetWrapper()->SetModified(fModified); }
	void Save(void)
		{ GetWrapper()->Save(); }
	void Refresh(void)
		{ GetWrapper()->Refresh(); }
	void Redraw(void)
		{ GetWrapper()->Redraw(); }
	void Show(BOOL fShow = TRUE)
		{ GetWrapper()->Show(fShow); }

protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorMemo)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CAddressBookFieldEditor* GetWrapper(void) const
		{ return m_pWrapper; }

protected:
	CAddressBookFieldEditor* m_pWrapper;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPictureButton
//
// A special button window used by the picture editor.

class CAddressBookFieldEditorPictureButton : public CButton
{
	INHERIT(CAddressBookFieldEditorPictureButton, CButton)

public:
	CAddressBookFieldEditorPictureButton();
	virtual ~CAddressBookFieldEditorPictureButton();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorPictureButton)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookFieldEditorPicture
//
// The picture field editor which lets users pick a picture.

class CAddressBookFieldEditorPicture : public CWnd
{
	INHERIT(CAddressBookFieldEditorPicture, CWnd)

public:
	CAddressBookFieldEditorPicture(CAddressBookFieldEditor* pWrapper, CWnd* pParent);
	virtual ~CAddressBookFieldEditorPicture();

public:
	// Calls to common wrapper functions.
	CFlatFileDatabaseField* GetField(void)
		{ return GetWrapper()->GetField(); }
	void Reset(void)
		{ GetWrapper()->Reset(); }
	void Move(const CRect& crPosition);
	BOOL IsEditing(void)
		{ return GetWrapper()->IsEditing(); }
	BOOL IsModified(void)
		{ return m_fModified; }
	void SetModified(BOOL fModified = TRUE)
		{ m_fModified = fModified; }
	void Save(void)
		{ GetWrapper()->Save(); }
	void Refresh(void)
		{ GetWrapper()->Refresh(); }
	void Redraw(void)
		{ GetWrapper()->Redraw(); }
	void Show(BOOL fShow = TRUE)
		{ GetWrapper()->Show(fShow); }

protected:
	// Generated message map functions
	//{{AFX_MSG(CAddressBookFieldEditorPicture)
	afx_msg void OnNewPicture();
	afx_msg void OnDeletePicture();
	afx_msg void OnCopyPicture();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFocus(CWnd* pNewWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnEditorActivated(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	CAddressBookFieldEditor* GetWrapper(void) const
		{ return m_pWrapper; }

	void ShowButtons(void);

	BOOL HasBitmap(void)
		{ return (m_pBitmap != NULL) && (!m_csName.IsEmpty()); }
	LPBITMAPINFO GetBitmapData(void)
		{ return m_pBitmap; }
	LPCSTR GetBitmapName(void)
		{ return m_csName; }
	void SetBitmap(LPCSTR pszName, LPBITMAPINFO pData);
	void DeleteBitmap(void);

protected:
	CAddressBookFieldEditor* m_pWrapper;

	LPBITMAPINFO m_pBitmap;
	CString m_csName;

	void GetImageRectangle(CRect& crImage);
	void ExcludeButtonRectangles(CDC& dc);

	CRect m_crPicture;
	BOOL m_fModified;

	CAddressBookFieldEditorPictureButton m_NewButton;
	CAddressBookFieldEditorPictureButton m_DeleteButton;
	CAddressBookFieldEditorPictureButton m_CopyButton;
};

#endif
