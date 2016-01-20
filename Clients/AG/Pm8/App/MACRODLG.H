/*
// macrodlg.h : header file
//
// $Workfile: MACRODLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MACRODLG.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:32   Fred
// Initial revision.
// 
//    Rev 1.2   07 Jul 1997 16:07:46   Jay
// Better handling of saved state.
// 
//    Rev 1.1   16 Jul 1996 17:30:38   Fred
// More address/sender book stuff
// 
//    Rev 1.0   14 Mar 1996 13:38:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:44   FRED
// Initial revision.
// 
//    Rev 1.22   15 Jan 1996 10:26:24   JAY
//  
// 
//    Rev 1.21   09 Nov 1995 16:59:56   JAY
// Restores document modified state on Cancel.
// 
//    Rev 1.20   27 Aug 1995 12:34:10   FRED
// New fill-in-fields stuff
// 
//    Rev 1.19   09 Aug 1995 13:11:22   JAY
// Misc changes.
// 
//    Rev 1.18   06 Aug 1995 16:33:02   FRED
// More Fill-in Fields dialog stuff.
// 
//    Rev 1.17   06 Aug 1995 09:45:40   FRED
// More fill-in fields stuff.
// 
//    Rev 1.16   03 Aug 1995 14:33:12   FRED
//  
// 
//    Rev 1.15   03 Aug 1995 08:00:30   FRED
// Fill in stuff.
// 
//    Rev 1.14   02 Aug 1995 14:56:26   JAY
//  
// 
//    Rev 1.13   02 Aug 1995 12:58:14   FRED
// Some more fill-in fields stuff.
// 
//    Rev 1.12   02 Aug 1995 08:08:16   FRED
// Fill-in fields stuff.
// 
//    Rev 1.11   11 Jul 1995 14:18:56   JAY
// Fill-in fields dialog.
// 
//    Rev 1.10   11 Jul 1995 09:41:42   JAY
// Initial fill-ins dialog for doc.
// 
//    Rev 1.9   03 May 1995 09:40:28   JAY
// Window position remembering code
// 
//    Rev 1.8   02 May 1995 17:09:18   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.7   02 May 1995 15:39:32   JAY
// More smart fields stuff.
// 
//    Rev 1.6   01 May 1995 15:28:00   JAY
// Restructured the macro stuff
// 
//    Rev 1.5   28 Apr 1995 17:26:46   JAY
// Got rid of list double click for insert.
// 
//    Rev 1.4   28 Apr 1995 16:11:18   JAY
// Validation, better kill focus support, more stability, etc.
// 
//    Rev 1.3   26 Apr 1995 14:41:30   JAY
//  
// 
//    Rev 1.2   25 Apr 1995 09:02:56   JAY
// More refinements.
// 
//    Rev 1.1   19 Apr 1995 12:33:30   JAY
//  
// 
//    Rev 1.0   29 Mar 1995 09:18:32   JAY
// Initial revision.
*/

#ifndef __MACRODLG_H__
#define __MACRODLG_H__

class CPmwDoc;
class CPmwView;
class CStdMacroServer;

#include "pmwdlg.h"
#include "macrorec.h"
#include "proplist.h"

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldListBoxItem

class CSmartFieldListBoxItem : public CPropertyListBoxItem
{
	INHERIT(CSmartFieldListBoxItem, CPropertyListBoxItem)
	
public:
	CSmartFieldListBoxItem(CMacro* pMacro, int nIndex, CStdMacroServer* pMacroServer, BOOL fNew = FALSE);

/* Field access. */

	virtual CString& GetFieldText(int nField, CString& csField);
	virtual void SetFieldText(int nField, LPCSTR pNewField);
	virtual BOOL CanEditField(int nField);

	BOOL New(void) const
		{ return m_fNew; }
	void New(BOOL fNew)
		{ m_fNew = fNew; }

	CMacro* Macro(void) const
		{ return m_pMacro; }
	int Index(void) const
		{ return m_nIndex; }
	CStdMacroServer* MacroServer(void) const
		{ return m_pMacroServer; }

	virtual void SaveState(void);
	virtual void RestoreState(void);
	
protected:
	CMacro* m_pMacro;
	int m_nIndex;
	CStdMacroServer* m_pMacroServer;
	BOOL m_fNew;
	BOOL IsMacroValid(void);
	
	virtual void PaintField(int nField, CPropertyListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void ComputeColors(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, COLORREF& clForeground, COLORREF& clBackground);
	virtual void ComputeBounds(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crField);
	virtual void PaintGrid(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual void PaintBackground(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crField, COLORREF clBackground);
	virtual void PaintItem(int nField, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintString(LPCSTR pString, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintIndicator(LPCSTR pString, COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
	virtual void PaintFocus(COwnerDrawListBox* pList, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, const CRect* pRect = NULL);
	virtual BOOL IsSelected(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	virtual BOOL IsFocused(int nField, COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct);
	
	int m_nSaveType;
	CString m_csSaveValue;
};

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldList

class CSmartFieldListBox : public CPropertyListBox
{
	INHERIT(CSmartFieldListBox, CPropertyListBox)

public:
	CSmartFieldListBox();
	~CSmartFieldListBox();

	void Fields(CStdMacroServer* pMacroServer, CMacroList* pMacros = NULL, CWordArray* pInUse = NULL);

	virtual BOOL BeginEdit(int nCurSel = -1);
	virtual BOOL FinishEdit(BOOL fValidate = TRUE, BOOL fByReturn = FALSE);

	BOOL EditDialog(void) const
		{ return m_fEditDialog; }
	BOOL EditDialog(BOOL fEditDialog)
		{ BOOL fED = m_fEditDialog; m_fEditDialog = fEditDialog; return fED; }

	virtual void SaveState(void);
	virtual void RestoreState(void);
	
	void Rebuild(void);
	
protected:
	virtual BOOL ShowMacro(CMacro* pMacro);

	virtual WNDPROC* GetSuperWndProcAddr(void);
	virtual BOOL OnFinishEdit(BOOL fValidate = TRUE, BOOL fByReturn = FALSE);
	virtual void OnAbortEdit(void);
	virtual BOOL CanEditItem(int nItem, int nField);

	virtual CPropertyEditor* CreateEditor(int nField, const CRect& crPosition);
	virtual void DestroyEditor(CPropertyEditor*& pEditor);

	// Generated message map functions
	//{{AFX_MSG(CSmartFieldListBox)
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CStdMacroServer* m_pMacroServer;
	CMacroList* m_pMacros;
	CWordArray* m_pInUse;
	BOOL m_fEditDialog;
	BOOL m_fSavedState;
};

/////////////////////////////////////////////////////////////////////////////
// CSmartFieldsDialog dialog

class CSmartFieldsDialog : public CDialog
{
// Construction
public:
	CSmartFieldsDialog(CPmwView* pView, CWnd* pParent = NULL, BOOL fStartup = FALSE);	// standard constructor
	virtual ~CSmartFieldsDialog();

	void SetView(CPmwView* pView, BOOL fDoItAnyway = FALSE);
	CPmwView* GetView(void);

// Called to update during idle time.
	void OnIdleUpdate(void);

	void UpdateFields(BOOL fReflow = FALSE, int nMacro = -1);

	virtual BOOL CanBeginEdit(int nItem, int nField);

// Hook support.
	typedef void (* REFLOW_HOOK)(LPVOID pHookData);
	void SetReflowHook(REFLOW_HOOK pHook = NULL, LPVOID pHookData = NULL);

// Dialog Data
	//{{AFX_DATA(CSmartFieldsDialog)
	enum { IDD = IDD_SMART_FIELDS_DIALOG };
	//}}AFX_DATA
	CSmartFieldListBox* m_pList;
	BOOL m_fChangedMacro;
	BOOL m_fShowValues;
	CWordArray m_cwaMacrosInUse;

	virtual void SaveState(void);
	virtual void RestoreState(void);

// Implementation

protected:
	CPmwView* m_pView;
	CPmwDoc* m_pDoc;
	CStdMacroServer* m_pMacroServer;
	CMacroList* m_pMacros;
	UINT m_uSoundID;
	BOOL m_fStartup;
	BOOL m_fOriginalModified;

	REFLOW_HOOK m_pReflowHook;
	LPVOID m_pReflowData;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void InitList(void);
	void ResetDialog(void);
	void UpdateButton(WORD wID, BOOL fEnable);
	void UpdateButtons(void);
	void EnableDlgItem(int nId, BOOL fEnable);
	
	virtual CSmartFieldListBox* CreateListBox(void);

	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CSmartFieldsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMacroList();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	afx_msg void OnClickedClose();
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnReceivers();
	afx_msg void OnSender();
	virtual void OnCancel();
	afx_msg void OnSetfocusMacroList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif					// #ifndef __MACRODLG_H__
